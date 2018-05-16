/*
*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#include "config.h"
#include "keyboard.h"
#include "led.h"
#include "time.h"
#include "ds1302.h"
#include "ds18b20.h"
#include "main.h"

enum estaSys staSys = E_SELECT;
u8 temp1s = 1;//�¶ȼ����������������SetSec�Ƚ�
bit flagtemp = 0;//һ���¶Ȳɼ���ɱ�־
bit flag200ms = 0;//200msˢ��ʱ��
u8 T0RH, T0RL;
bit TurnSta = 0;
bit turn1s = 0;

void CloseFucker();
void ConfigTimer0(u16 ms);

void main()
{
	bit sta = 1;

 	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	RefreshSetSec();
	InitDS1302();
	Start18B20();//���б�Ҫ����һ�䣬��ǰת�����������õ�����ת���������������85C

	while(1)
	{
	 	KeyDriver();

		if(staSys == E_CLK)//����ʱ��ģʽ�£�200msˢ��һ��ʱ�ӣ�SetSec�ɼ�һ���¶ȣ�ֱ���ɹ�10��
		{
			if(flag200ms)
			{
				flag200ms = 0;
			   	RefreshTime(1);
			}
			if((flagtemp==1) && (flagtempOver==0))//һ���¶Ȳɼ����ʱ�䵽�����һ�δ�ɹ�10��
			{
				flagtemp = 0;
				CollectTemp();	
			}
			if(turn1s)
			{
			 	turn1s = 0;
				TurnSta = ~TurnSta;//�������˸��־
			}

		}
		else if(staSys == E_DISPLAY)
		{
		 	EnterTemp(0);
			if(flag200ms && flagLed)
			{
				flag200ms = 0;

			 	if(sta)//LED��˸
				{
					sta = 0;
				 	P2 = (P2&0x1F) | 0x80;
					P0 = 0xFE;
					P2 = P2&0x1F;	
				}
				else
				{
					sta = 1;
					P2 = (P2&0x1F) | 0x80;
					P0 = 0xFF;
					P2 = P2&0x1F; 	
				}
			}
		}
		
	
	}

}


void CloseFucker()
{
 	P2 = (P2&0x1F)|0xA0;
	P0 &= 0xAF;
	P2 &= 0x1F;
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
	static u16 tmr1s = 0;

 	TH0 = T0RH;
	TL0 = T0RL;
	tmr1s++;
	tmr200ms++;

	KeyScan();//����ܺͰ���ɨ��
	LedScan();

	if(tmr200ms >= 200)
	{
	 	tmr200ms = 0;
		flag200ms = 1;
	}

	if(tmr1s == 1000)
	{
	 	tmr1s = 0;
		turn1s = 1;
		if(!flagtempOver)
		{
			temp1s++;
			if(temp1s >= SetSec)
			{
			 	temp1s = 0;
				flagtemp = 1;
			}
		}
	}
}