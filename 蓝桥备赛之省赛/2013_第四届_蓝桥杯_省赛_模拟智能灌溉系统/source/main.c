/*
*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע����һ��С���������ΪADC�ɼ����ٽ�ֵ��������Ծ������ֵ���¸�����
*         ��������ֶ�ģʽ�£�AD�����ٽ���ֵ������ʱ��̵����Լ�������Ƶ��������
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "e2prom.h"
#include "keyboard.h"
#include "ds1302.h"
#include "time.h"
#include "pcf8581.h"
#include "main.h"

enum eSysSta SysSta = E_AUTO;//ϵͳ����״̬
bit flag200ms = 1;
u8 T0RH, T0RL;

void CloseFucker();
void ConfigTimer0(u16 ms);

void main()
{
 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();

	OpenLed(1);//��LED1

	while(1)
	{
		KeyDriver();
		if(flag200ms)
		{
			flag200ms = 0;
			if(!SetLimitSta)
			{
		 		RefreshTime(1);
			}
			else
			{
			 	RefreshAdjust();
			}

			if(SysSta == E_AUTO)//�Զ�ģʽ
			{
			 	LimitMonitorAUTO();	
			}	
			else//�ֶ�ģʽ
			{
				LimitMonitorHAND();	
			}
		}

	}
}

void CloseFucker()
{
 	P2 = (P2&0x1F) | 0xA0;
	P0 = P0&0xAF;
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)
{
 	u32 tmp;

	tmp = 11059200/12;
	tmp = (tmp*ms)/1000;
	tmp = 65536 - tmp;
	T0RH = (u8)(tmp>>8);
	T0RL = (u8)tmp;
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TH0 = T0RH;
	TL0 = T0RL;
	ET0 = 1;
	TR0 = 1;
}

void InterruptTimer0() interrupt 1
{
	static u8 tmr200ms = 0;
	
 	TH0 = T0RH;
	TL0 = T0RL;
	tmr200ms++;

	if(tmr200ms >= 200)
	{
	 	tmr200ms = 0;
		flag200ms = 1;
	}

	LedScan();
	KeyScan();
}