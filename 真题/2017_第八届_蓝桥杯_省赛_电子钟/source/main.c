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
#include "ds1302.h"
#include "time.h"
#include "keyboard.h"
#include "main.h"
#include "led.h"
#include "ds18b20.h"

u8 T0RH = 0, T0RL = 0;
bit flag200ms = 1;
bit flag1s = 0;

enum eStaSystem staSystem = E_NORMAL;

void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{
	bit sta = 0; //��������Ӧ�ı�־

	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();
	
	while(1)
	{
		KeyDriver();
		if(flag200ms) //200ms��ʱ��
		{
			flag200ms = 0;
			AlarmMonitor();
			if((staSystem == E_NORMAL) && (!flag18B20))//��ǰϵͳ��ģʽ��ʱ����ʾ����û�а���S4 - ˢ�µ�ǰʱ��
			{
				RefreshTime(1);	
			}
			else if((staSystem == E_NORMAL) && (flag18B20))//��ǰϵͳģʽ����ʱ����ʾ���Ұ���s4 - �����¶���ʾ
			{
			 	Refresh18B20();
			}

		}
		if(flagCursor)//������ģʽ��ʵ�ֹ����˸��Ч�����������˸��
		{
		 	if(flag1s)//1s�ļ������
			{
			 	flag1s = 0;
				if(sta == 1)//��ǰ������״̬
				{
					sta = 0;
					LedBuff[setCursorIndex] = 0xFF;//�ر������
					LedBuff[setCursorIndex-1] = 0xFF;
				}
				else //��ǰ�����״̬
				{
					sta = 1;
					if(staSystem == E_SET_TIME)//��������һ����Ҫ�ж�һ�µ�ǰ��ʲô����ģʽ��ˢ��ʱ�ӻ������ӣ�
					{
						RefreshTime(0);
					}
					else if(staSystem == E_SET_ALARM)//!!!ͬ��
					{
					 	RefreshAlarm();
					}
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
       
    tmp = (SYS_MCLK * ms) / 1000;  
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

    if (tmr200ms >= 200) 
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
