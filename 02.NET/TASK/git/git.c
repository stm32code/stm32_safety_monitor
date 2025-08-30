#include "git.h"

Data_TypeDef Data_init;						  // �豸���ݽṹ��
Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
Device_Satte_Typedef device_state_init;		  // �豸״̬

extern int32_t n_sp02;		 // SPO2 value
extern int8_t ch_spo2_valid; // indicator to show if the SP02 calculation is valid
extern int32_t n_heart_rate; // heart rate value
extern int8_t ch_hr_valid;	 // indicator to show if the heart rate calculation is valid
// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data)
{
	obtain_bus(); // ����
	// �¶Ȳ���
	Data_init.Temp_Test = DS18B20_GetTemp_MatchRom(ucDs18b20Id); // ��ȡ�¶�
	if (Data_init.Temp_Test > 1)
	{
		Data_init.temperatuer = Data_init.Temp_Test;
	}
	return MY_SUCCESSFUL;
}
// ��ʼ��
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{

	// ��
	R_Test();

	return MY_SUCCESSFUL;
}
// ����OLED��ʾ��������
mySta Update_oled_massage()
{
#if OLED // �Ƿ��
	char str[50];
	// ����
	if (0 < n_heart_rate && n_heart_rate < 150 && ch_spo2_valid)
	{
		sprintf(str, "���� : %03d", n_heart_rate);
		OLED_ShowCH(0, 0, (unsigned char *)str);
		sprintf(str, "Ѫ�� : %03d", n_sp02);
		OLED_ShowCH(0, 2, (unsigned char *)str);
	}
	else
	{
		sprintf(str, "���� : %03d", 0);
		OLED_ShowCH(0, 0, (unsigned char *)str);
		sprintf(str, "Ѫ�� : %03d", 0);
		OLED_ShowCH(0, 2, (unsigned char *)str);
	}
	// ����
	if(Data_init.temperatuer>100){
		sprintf(str, "Temp  :  000  ");
	}else{
		sprintf(str, "Temp  : %.2f C",	Data_init.temperatuer);
	}
	OLED_ShowCH(0, 4, (unsigned char *)str);

	if (device_state_init.Fall_State == 0)
	{
		sprintf(str, "����״̬ : ����");
	}
	else
	{
		sprintf(str, "����״̬ : ����");
	}
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// �����豸״̬
mySta Update_device_massage()
{
	// �Զ�ģʽ
	if (device_state_init.Waning)
	{
		BEEP=1;
		delay_ms(50);
		BEEP=0;
	}else{
		BEEP=0;
	}
	// ��ͬ����
	if ((threshold_value_init.heart_value < n_heart_rate && n_heart_rate < 150 && ch_spo2_valid))
	{
		device_state_init.Waning = 1;
	}
	else if ((threshold_value_init.spo2_value < n_sp02) && ch_spo2_valid)
	{
		device_state_init.Waning = 2;
	}
	else if (threshold_value_init.temp_value < Data_init.temperatuer)
	{
		device_state_init.Waning = 3;
	}
	else	if (device_state_init.Fall_State)
	{
		device_state_init.Waning = 4;
	}
	else	if (device_state_init.Key_State)
	{
		device_state_init.Waning = 5;
	}
	else 
	{
		device_state_init.Waning = 0;
	}

	if (Data_init.App)
	{
		switch (Data_init.App)
		{
		case 1:
			SendMqtt(1); // �������ݵ�APP
			break;
		case 2:
			// SendData(); //  ���͵���ƽ̨
			break;
		}
		Data_init.App = 0;
		Data_init.SendTime = 0;
	}

	return MY_SUCCESSFUL;
}

// ��ʱ��
void Automation_Close(void)
{
	// ʵ��30s�Զ��л�����
}
// ��ⰴ���Ƿ���
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// ����һ�εļ�ֵ�Ƚ� �������ȣ������м�ֵ�ı仯����ʼ��ʱ
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // ʱ���¼��
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // �̰�
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Short\n");
		
			if (device_state_init.Fall_State)
			{
				device_state_init.Fall_State = 0;
			}else{
				if (device_state_init.Key_State == 1)
				{
					device_state_init.Key_State = 0;
				}
				else
				{
					device_state_init.Key_State = 1;
				}
			}

			break;


		default:
			break;
		}
		num_on = 0;
	}
	else if (key == 0 && num_on >= Key_Scan_Time) // ����
	{
		switch (key_old)
		{
		case KEY1_PRES:
			printf("Key1_Long\n");

			if (device_state_init.Fall_State)
			{
				device_state_init.Fall_State = 0;
			}else{
				if (device_state_init.Key_State == 1)
				{
					device_state_init.Key_State = 0;
				}
				else
				{
					device_state_init.Key_State = 1;
				}
			}

			break;

		default:
			break;
		}
		num_on = 0;
	}
}
// ����json����
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // ���json��ʽ
	//cJSON *cjson_data = NULL; // ����
	const char *massage;
	// ������������
	u8 cjson_cmd; // ָ��,����

	/* ��������JSO���� */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// ����ʧ��
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* ���θ���������ȡJSON���ݣ���ֵ�ԣ� */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* ����Ƕ��json���� */
	//cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // ��Ϣ��

		threshold_value_init.heart_value = cJSON_GetObjectItem(cjson_test, "heartv")->valueint;
		threshold_value_init.spo2_value = cJSON_GetObjectItem(cjson_test, "spo2v")->valueint;
		threshold_value_init.temp_value = cJSON_GetObjectItem(cjson_test, "tempv")->valueint;
		Data_init.App =1;
		W_Test();
		break;
	case 0x02: // ��Ϣ��

		break;
	case 0x03: // ���ݰ�

		break;
	case 0x04: // ���ݰ�
		Data_init.App = cjson_cmd + 1;

		break;
	default:
		break;
	}

	/* ���JSON����(��������)���������� */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
