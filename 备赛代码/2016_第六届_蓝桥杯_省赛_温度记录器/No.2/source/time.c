/*
*******************************************************************************
* �ļ�����time.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "ds1302.h"
#include "ds18b20.h"
#include "main.h"

struct sTime buffTime;
u8 SetSec = 1;//Ĭ�ϲɼ����1s
int TempTable[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bit flagtempOver = 0;//ʮ���¶Ȳɼ���ɱ�־
bit flagLed = 0;//LED����
u8 tempIndex = 0;//�ɼ��¶����� 00 ~ 09


void RefreshTime(bit sta)
{ 	
	
 	if(sta == 1)
	{
	 	GetRealTime(&buffTime);
	}
	if(TurnSta)
	{
	 	LedBuff[2] = 0xBF;
		LedBuff[5] = 0xBF;	
	}
	else
	{
	 	LedBuff[2] = 0xFF;
		LedBuff[5] = 0xFF;	
	}

	LedBuff[0] = LedChar[(buffTime.sec)&0x0F];
	LedBuff[1] = LedChar[(buffTime.sec)>>4];
	LedBuff[3] = LedChar[(buffTime.min)&0x0F];
	LedBuff[4] = LedChar[(buffTime.min)>>4];	
	LedBuff[6] = LedChar[(buffTime.hour)&0x0F];
	LedBuff[7] = LedChar[(buffTime.hour)>>4];
}

void RefreshTemp()
{
 	int temp;
	u8 i;	

	Start18B20();
	Get18B20Temp(&temp);
	
	temp = temp >> 4;

	for(i=7; i>=3; i--)
	{
	 	LedBuff[i] = 0xFF;
	}
	LedBuff[1] = LedChar[temp%10];
	LedBuff[2] = LedChar[temp/10];
	LedBuff[0] = 0xC6;
}

void CollectTemp()
{
	static u8 i=0;
	int temp;

	if(i<10)
	{
	 	Start18B20();
		if(Get18B20Temp(&temp))//�������������ķ���ֵ
		{
			temp >>= 4;//�����м����װ�����飡
			TempTable[i] = temp;
			i++;
		}	
	}
	else
	{
		i = 0;
		flagtempOver = 1;//һ���¶�ȫ���ɼ���ɱ�־
		flagLed = 1;//��LED����
	 	staSys = E_DISPLAY;//ϵͳģʽ�ı�	
	}
}

void RefreshSetSec()
{
 	u8 i;

	for(i=7; i>=3; i--)
	{
	 	LedBuff[i] = 0xFF;
	}
	LedBuff[2] = 0xBF;
	LedBuff[1] = LedChar[SetSec/10];
	LedBuff[0] = LedChar[SetSec%10];
}

void EnterSec()
{
 	static u8 SecIndex = 0;//�ɼ��¶ȵ�ʱ����������ֵ

	switch(SecIndex)
	{
	 	case 0: SetSec = 5;  break;
		case 1: SetSec = 30; break;
		case 2: SetSec = 60; break;
		case 3: SetSec = 1; break;
		default: break;
	}
	SecIndex++;
	SecIndex &= 0x03;

	RefreshSetSec();
}

void EnterCLK()
{	
	staSys = E_CLK;
	RefreshTime(1);
}

void EnterTemp(bit sta)//�¶Ȳɼ���ʾ����
{
	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[tempIndex/10];
	LedBuff[5] = LedChar[tempIndex%10];
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[2] = 0xBF;
	LedBuff[1] = LedChar[TempTable[tempIndex]/10];
	LedBuff[0] = LedChar[TempTable[tempIndex]%10];

	if(sta)
	{
		tempIndex++;
		if(tempIndex == 10)
		{
		 	tempIndex = 0;	
		}
	}
}

void KeyAction(u8 keycode)
{
 	if(keycode == '0')//��ӦS4����
	{
		if(staSys == E_SELECT)//��ǰ��ϵͳģʽ��������
		{
			EnterSec();
		}	
	}
	else if(keycode == '7')//��ӦS5��
	{
		if(staSys == E_SELECT)//��ǰ��ϵͳģʽ�ǲ�������
		{
	 		EnterCLK();
		}	
	}
	else if(keycode == '4')//��ӦS6��
	{
		if(staSys == E_DISPLAY)//��ǰ��ϵͳģʽ���¶Ȳɼ�����
		{
			flagLed = 0;//�ر���˸��LED
			P2 = (P2&0x1F) | 0x80;//ȷ����ʱLED����ģ�
			P0 = 0xFF;
			P2 = P2&0x1F;
			 
	 		EnterTemp(1);//�����¶���ʾ�л�
		}
	}
	else if(keycode == '1')//��ӦS7��
	{
	 	if(staSys == E_DISPLAY)//��ǰ��ϵͳģʽ�¶Ȳɼ�����
		{
		 	staSys = E_SELECT;//�������óɲ������ý���
			SetSec = 1;//�ָ���ʼ����ʱ�����ü��
			tempIndex = 0;//�¶�����
			flagtempOver = 0;//�¶�ȫ���ɼ���ɱ�־
			RefreshSetSec();//ˢ�»ص��������ý���
		}
	}

}