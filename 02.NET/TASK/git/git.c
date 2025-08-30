#include "git.h"

Data_TypeDef Data_init;						  // 设备数据结构体
Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
Device_Satte_Typedef device_state_init;		  // 设备状态

extern int32_t n_sp02;		 // SPO2 value
extern int8_t ch_spo2_valid; // indicator to show if the SP02 calculation is valid
extern int32_t n_heart_rate; // heart rate value
extern int8_t ch_hr_valid;	 // indicator to show if the heart rate calculation is valid
// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data)
{
	obtain_bus(); // 跌倒
	// 温度测量
	Data_init.Temp_Test = DS18B20_GetTemp_MatchRom(ucDs18b20Id); // 获取温度
	if (Data_init.Temp_Test > 1)
	{
		Data_init.temperatuer = Data_init.Temp_Test;
	}
	return MY_SUCCESSFUL;
}
// 初始化
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state)
{

	// 读
	R_Test();

	return MY_SUCCESSFUL;
}
// 更新OLED显示屏中内容
mySta Update_oled_massage()
{
#if OLED // 是否打开
	char str[50];
	// 心率
	if (0 < n_heart_rate && n_heart_rate < 150 && ch_spo2_valid)
	{
		sprintf(str, "心率 : %03d", n_heart_rate);
		OLED_ShowCH(0, 0, (unsigned char *)str);
		sprintf(str, "血氧 : %03d", n_sp02);
		OLED_ShowCH(0, 2, (unsigned char *)str);
	}
	else
	{
		sprintf(str, "心率 : %03d", 0);
		OLED_ShowCH(0, 0, (unsigned char *)str);
		sprintf(str, "血氧 : %03d", 0);
		OLED_ShowCH(0, 2, (unsigned char *)str);
	}
	// 体温
	if(Data_init.temperatuer>100){
		sprintf(str, "Temp  :  000  ");
	}else{
		sprintf(str, "Temp  : %.2f C",	Data_init.temperatuer);
	}
	OLED_ShowCH(0, 4, (unsigned char *)str);

	if (device_state_init.Fall_State == 0)
	{
		sprintf(str, "老人状态 : 正常");
	}
	else
	{
		sprintf(str, "老人状态 : 跌倒");
	}
	OLED_ShowCH(0, 6, (unsigned char *)str);
#endif

	return MY_SUCCESSFUL;
}

// 更新设备状态
mySta Update_device_massage()
{
	// 自动模式
	if (device_state_init.Waning)
	{
		BEEP=1;
		delay_ms(50);
		BEEP=0;
	}else{
		BEEP=0;
	}
	// 不同报警
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
			SendMqtt(1); // 发送数据到APP
			break;
		case 2:
			// SendData(); //  发送到云平台
			break;
		}
		Data_init.App = 0;
		Data_init.SendTime = 0;
	}

	return MY_SUCCESSFUL;
}

// 定时器
void Automation_Close(void)
{
	// 实现30s自动切换界面
}
// 检测按键是否按下
static U8 num_on = 0;
static U8 key_old = 0;
void Check_Key_ON_OFF()
{
	U8 key;
	key = KEY_Scan(1);
	// 与上一次的键值比较 如果不相等，表明有键值的变化，开始计时
	if (key != 0 && num_on == 0)
	{
		key_old = key;
		num_on = 1;
	}
	if (key != 0 && num_on >= 1 && num_on <= Key_Scan_Time) // 25*10ms
	{
		num_on++; // 时间记录器
	}
	if (key == 0 && num_on > 0 && num_on < Key_Scan_Time) // 短按
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
	else if (key == 0 && num_on >= Key_Scan_Time) // 长按
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
// 解析json数据
mySta massage_parse_json(char *message)
{

	cJSON *cjson_test = NULL; // 检测json格式
	//cJSON *cjson_data = NULL; // 数据
	const char *massage;
	// 定义数据类型
	u8 cjson_cmd; // 指令,方向

	/* 解析整段JSO数据 */
	cjson_test = cJSON_Parse(message);
	if (cjson_test == NULL)
	{
		// 解析失败
		printf("parse fail.\n");
		return MY_FAIL;
	}

	/* 依次根据名称提取JSON数据（键值对） */
	cjson_cmd = cJSON_GetObjectItem(cjson_test, "cmd")->valueint;
	/* 解析嵌套json数据 */
	//cjson_data = cJSON_GetObjectItem(cjson_test, "data");

	switch (cjson_cmd)
	{
	case 0x01: // 消息包

		threshold_value_init.heart_value = cJSON_GetObjectItem(cjson_test, "heartv")->valueint;
		threshold_value_init.spo2_value = cJSON_GetObjectItem(cjson_test, "spo2v")->valueint;
		threshold_value_init.temp_value = cJSON_GetObjectItem(cjson_test, "tempv")->valueint;
		Data_init.App =1;
		W_Test();
		break;
	case 0x02: // 消息包

		break;
	case 0x03: // 数据包

		break;
	case 0x04: // 数据包
		Data_init.App = cjson_cmd + 1;

		break;
	default:
		break;
	}

	/* 清空JSON对象(整条链表)的所有数据 */
	cJSON_Delete(cjson_test);

	return MY_SUCCESSFUL;
}
