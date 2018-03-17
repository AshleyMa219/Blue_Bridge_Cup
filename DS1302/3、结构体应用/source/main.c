/*
*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע�������·�װ�ɽṹ�����ʵ�ֵ����ӣ�
*         S8-ESC����S12-�س�����S19-���ӣ�S18-��С��S17-���ƣ�S16-����
*******************************************************************************
*/

#include "config.h"
#include "ds1302.h"
#include "keyboard.h"
#include "led.h"
#include "time.h"

u8 T0RH = 0, T0RL = 0;
bit flag200ms = 1;
bit flag1s = 0;
bit sta = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();
void RefreshTimeShow();

void main()
{
	u8 psec = 0xAA;
	struct sTime buffTime;

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();

	while(1)
	{
		KeyDriver();
		if((flag200ms !=0) && (setIndex == 0)) //200ms��ʱ������δ��������ģʽ
		{
		 	flag200ms = 0;
			GetRealTime(&buffTime);
			if(psec != buffTime.sec)
			{
			 	RefreshTime(1);
				psec = buffTime.sec;
			}
		}	
		if(flagCursor)
		{
		 	if(flag1s)
			{
			 	flag1s = 0;
				if(sta == 1)
				{
				 	sta = 0;
					LedBuff[setCursorIndex] = 0xFF;
					LedBuff[setCursorIndex-1] = 0xFF;
				}
				else
				{
				 	sta = 1;
					RefreshTime(0);
				}
			}
		}
	}
}

void CloseFucker()
{
 	P2 = (P2 & 0x1F) | 0xA0;
	P0 = P0 & 0xAF;
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)
{
    u32 tmp; 
    
    tmp = 11059200 / 12;    
    tmp = (tmp * ms) / 1000;  
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
	KeyScan();
	LedScan();

    tmr200ms++;
	tmr1s++;

    if(tmr200ms >= 200) 
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
	if(tmr1s >= 1000)
	{
	 	tmr1s = 0;
		flag1s = 1;
	}
}
