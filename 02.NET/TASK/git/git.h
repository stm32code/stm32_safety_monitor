#ifndef __GIT__H
#define __GIT__H


//  �豸ʹ�����趨��
#define OLED 1			// �Ƿ�ʹ��OLED
#define NET_SERVE 2		// ƽ̨ѡ��
#define NETWORK_CHAEK 1 // �Ƿ������߼��
#define KEY_OPEN 1		// �Ƿ��������Ͷ̰����
#define USART2_OPEN 0	// �Ƿ�ʹ�ô��ڶ�

// ���ݶ���
typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned int U32;
typedef signed int S32;
typedef float F32;

//  C��
#include "cjson.h"
#include <string.h>
#include <stdio.h>
// ��Ƭ��ͷ�ļ�
#include "sys.h"
#include "usart.h"	 
// ����Э���
#include "Net.h"
// �����豸
#include "esp8266.h"
// ������
#include "task.h"
#include "timer.h"
// Ӳ������
#include "delay.h"
#include "usart.h"
#include "git.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "flash.h"
#include "ds18b20.h"
// �����ļ�
#include "max30102.h"
#include "heartiic.h"
#include "algorithm.h"

// ���ٶ�
#include "adxl345.h"
#include "bus.h"
#if OLED // OLED�ļ�����
#include "oled.h"
#endif


// ��������Ϣ
#define SSID "NET"		// ·����SSIDxzc����
#define PASS "12345678" // ·��������
#if NET_SERVE == 0
// ��Э���������Onenet�ɰ�֧�֣�
#define ServerIP "183.230.40.39" // ������IP��ַ
#define ServerPort 6002			 // ������IP��ַ�˿ں�
#elif NET_SERVE == 1
// ����������ƽ̨��������֧�֣�
#define ServerIP "iot-06z00io8yqoxncb.mqtt.iothub.aliyuncs.com" // ������IP��ַ
#define ServerPort 1883											// ������IP��ַ�˿ں�
#elif NET_SERVE == 2
// EMQXƽ̨��������
#define ServerIP "8.133.197.10" // ������IP��ַ
#define ServerPort 1883			  // ������IP��ַ�˿ں�
#endif
// �豸��Ϣ
#define PROID "6-6-device"															 // ��ƷID
#define DEVID "6-6-device" // �豸ID
#define AUTH_INFO "6-6-device"						 // ��Ȩ��Ϣ
// MQTT���� /broadcast/
#define S_TOPIC_NAME "/broadcast/66/test1" // ��Ҫ���ĵ�����
#define P_TOPIC_NAME "/broadcast/66/test2" // ��Ҫ����������


// �Զ��岼������
typedef enum
{
	MY_TRUE,
	MY_FALSE
} myBool;

// �Զ���ִ�н������
typedef enum
{
	MY_SUCCESSFUL = 0x01, // �ɹ�
	MY_FAIL = 0x00		  // ʧ��

} mySta; // �ɹ���־λ

typedef enum
{
	OPEN = 0x01, // ��
	CLOSE = 0x00 // �ر�

} On_or_Off_TypeDef; // �ɹ���־λ

typedef enum
{
	DERVICE_SEND = 0x00, // �豸->ƽ̨
	PLATFORM_SEND = 0x01 // ƽ̨->�豸

} Send_directino; // ���ͷ���

typedef struct
{
	U8 App;			 // ָ��ģʽ
	 U8 AppTime;
  U8 SendTime;
	U8 Device_State; // ģʽ
	U8 Page;		 // ҳ��
	U8 Error_Time;
	U8 time_cut_page; // ҳ��
	U8 Time;
	F32 temperatuer; // �¶�
	F32 Temp_Test;// ��������
	
	F32 humiditr;	 // ʪ��
	U8 Flage;		 // ģʽѡ��

} Data_TypeDef; // ���ݲ����ṹ��

typedef struct
{

	U16 heart_value; // ����
	U16 temp_value; // ������ֵ
	U16 spo2_value; // Ѫ����ֵ
	U16 Distance_value; // ������ֵ
	
} Threshold_Value_TypeDef; // ���ݲ����ṹ��

typedef struct
{
	U8 check_device; // ״̬
	U8 check_open;	 // ����Ƿ�������
	U8 Key_State;	 // �������
	U8 Waning;  //����
	U8 check_time;
	
	U8 Fall_State; // ����


} Device_Satte_Typedef; // ״̬�����ṹ��

// ȫ������
extern Data_TypeDef Data_init;
extern Device_Satte_Typedef device_state_init; // �豸״̬

extern Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data);
// ��ʼ��
mySta Reset_Threshole_Value(Threshold_Value_TypeDef *Value, Device_Satte_Typedef *device_state);
// ����OLED��ʾ��������
mySta Update_oled_massage(void);
// �����豸״̬
mySta Update_device_massage(void);
// ����json����
mySta massage_parse_json(char *message);
// ����
void Check_Key_ON_OFF(void);
// ��ʱ
void Automation_Close(void);

#endif
