/*
*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��Burst��Single�Ļ����������޸ģ�
*         Burst�� - 0xBF, Burstд - 0xBE
*******************************************************************************
*/

#include "config.h"
#include "ds1302.h"
#include "lcd1602.h"

u8 T0RH = 0;
u8 T0RL = 0;
bit flag200ms = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();

void main()
{
	u8 i;
	u8 psec = 0xAA;
	u8 time[8];
	u8 str[12];

	
	EA = 1;
	CloseFucker();
	ConfigTimer0(1);
	InitDS1302();
	InitLcd1602();
	

	while(1)
	{
		if(flag200ms)
		{
		 	flag200ms = 0;
			for(i=0; i<7; i++)
			{
			 	time[i] = DS1302SingleRead(i);
			}
			if(psec != time[0])
			{
			    str[0] = '2';  //�����ݵĸ�2λ��20
                str[1] = '0';
                str[2] = (time[6] >> 4) + '0';  //���ꡱ��λ����ת��ΪASCII��
                str[3] = (time[6]&0x0F) + '0';  //���ꡱ��λ����ת��ΪASCII��
                str[4] = '-';  //������ڷָ���
                str[5] = (time[4] >> 4) + '0';  //���¡�
                str[6] = (time[4]&0x0F) + '0';
                str[7] = '-';
                str[8] = (time[3] >> 4) + '0';  //���ա�
                str[9] = (time[3]&0x0F) + '0';
                str[10] = '\0';
                LcdShowStr(0, 0, str);  //��ʾ��Һ���ĵ�һ��
                
                str[0] = (time[5]&0x0F) + '0';  //�����ڡ�
                str[1] = '\0';
                LcdShowStr(11, 0, "week");
                LcdShowStr(15, 0, str);  //��ʾ��Һ���ĵ�һ��
                
                str[0] = (time[2] >> 4) + '0';  //��ʱ��
                str[1] = (time[2]&0x0F) + '0';
                str[2] = ':';  //���ʱ��ָ���
                str[3] = (time[1] >> 4) + '0';  //���֡�
                str[4] = (time[1]&0x0F) + '0';
                str[5] = ':';
                str[6] = (time[0] >> 4) + '0';  //���롱
                str[7] = (time[0]&0x0F) + '0';
                str[8] = '\0';
                LcdShowStr(4, 1, str);  //��ʾ��Һ���ĵڶ���
                
                psec = time[0];  //�õ�ǰֵ�����ϴ�����
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
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH; 
    TL0 = T0RL;
    tmr200ms++;
    if (tmr200ms >= 200) 
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
