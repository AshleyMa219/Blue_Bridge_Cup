/*
*******************************************************************************
* �ļ�����time.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.1.0
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

u8 setIndex = 0;
struct sTime buffTime, setTime;//setTime��buffTime���ݴ���
bit staMute=0, flagAlarm=0;

u8 AlarmHour = 0x11;
u8 AlarmMin  = 0x24;
u8 AlarmSec  = 0x02;

void AlarmMonitor()
{
	static u8 AlarmCnt=0;
	static bit staLED=0;

	if((AlarmHour==buffTime.hour) && (AlarmMin==buffTime.min) && (AlarmSec==buffTime.sec))//��ʱ���
	{	
		flagAlarm = 1;	
	}
	if(flagAlarm)	 	
	{
		AlarmCnt++;
		if(!staMute)
		{
			if(staLED==0)
			{
				staLED = 1;
				P2 = (P2&0x1F)|0x80;
				P0 = 0xFE;
				P2 = P2&0x1F;	
			}	 		
			else
			{
				staLED = 0;
			 	P2 = (P2&0x1F)|0x80;
				P0 = 0xFF;
				P2 = P2&0x1F;	
			}
		}
		if(AlarmCnt >= 25)
		{
			AlarmCnt = 0;
		 	flagAlarm = 0;
			P2 = (P2&0x1F)|0x80;
			P0 = 0xFF;
			P2 = P2&0x1F;	
		}

	}
}

void ShowLedNumber(u8 index, u8 num)
{
 	if(num == 0xBF)
	{
	 	LedBuff[index] = 0xBF;
	}
	else
	{
	 	LedBuff[index] = LedChar[num];
	}
}

void RefreshTime(bit sta)
{
 	if(sta)
	{
	 	GetRealTime(&buffTime);
	}
	ShowLedNumber(0, buffTime.sec&0x0F);
	ShowLedNumber(1, buffTime.sec>>4);
	ShowLedNumber(2, 0xBF);
	ShowLedNumber(3, buffTime.min&0x0F);
	ShowLedNumber(4, buffTime.min>>4);
	ShowLedNumber(5, 0xBF);
	ShowLedNumber(6, buffTime.hour&0x0F);
	ShowLedNumber(7, buffTime.hour>>4);
}

void RefreshAlarm()
{
	ShowLedNumber(0, AlarmSec&0x0F); 
	ShowLedNumber(1, AlarmSec>>4);
	ShowLedNumber(2, 0xBF);
	ShowLedNumber(3, AlarmMin&0x0F);
	ShowLedNumber(4, AlarmMin>>4);
	ShowLedNumber(5, 0xBF);
	ShowLedNumber(6, AlarmHour&0x0F);
	ShowLedNumber(7, AlarmHour>>4);		
}

void Refresh18B20()
{
 	int temp;
	u8 i;

	Get18B20Temp(&temp);
	DS18B20Start();
	temp >>= 4;//����С������

	for(i=7; i>=3; i--)
	{
	 	LedBuff[i] = 0xFF;
	}
	LedBuff[2] = LedChar[temp/10];
	LedBuff[1] = LedChar[temp%10];
	LedBuff[0] = LedChar[12];
}

void SwitchTimeSet()
{
	if(SystemSta == E_NORMAL)
	{
		SystemSta = E_SET_TIME;
		setTime = buffTime;//���ݽ�������֮ǰ��ֵ

	 	setIndex++;
		LedSetCursor(setIndex);
		LedOpenCursor();//�򿪹����˸����

		RefreshTime(0);
	}
	else if(SystemSta == E_SET_TIME)
	{
	 	setIndex++;
		if(setIndex < 4)
		{
		 	LedSetCursor(setIndex);
		}
		else//�˳�����ģʽ����Ҫ�����Ƿ����"����"��ʱ�䣡
		{
			if((buffTime.hour==setTime.hour) && (buffTime.min==setTime.min) && (buffTime.sec==setTime.sec))//�ͽ�������֮ǰֵһ��
			{
			 	RefreshTime(1);
			}
			else
			{
			 	SetRealTime(&buffTime);	
			}
		 	SystemSta = E_NORMAL;//ֻҪ���ʱ����ʾģʽ�����¶Ȱ���û�а��£��ͻ�ˢ�»�ʵʱʱ�䣡
			setIndex = 0;
			LedCloseCursor();

		}
	}
}

void SwitchAlarmSet()
{
	if(SystemSta == E_NORMAL)
	{
		SystemSta = E_SET_ALARM;
	 	setIndex++;
		LedSetCursor(setIndex);
		LedOpenCursor();//�򿪹����˸����

		RefreshAlarm();
	}
	else if(SystemSta == E_SET_ALARM)
	{
	 	setIndex++;
		if(setIndex < 4)
		{
		 	LedSetCursor(setIndex);
		}
		else
		{
		 	SystemSta = E_NORMAL;//ֻҪ���ʱ����ʾģʽ�����¶Ȱ���û�а��£��ͻ�ˢ�»�ʵʱʱ�䣡
			setIndex = 0;
			staMute = 0;//�µ�����������ɣ������پ����ˣ�
			LedCloseCursor();
		}
	}			 	
}	

u8 IncBcd(u8 bcd, bit mode) //1-0~23; 0-0~59
{
 	if(mode)
	{
		if(bcd < 0x23)
		{
			bcd += 0x01;
			if((bcd&0x0F) == 0x0A)
			{
			 	bcd += 0x10;
				bcd &= 0xF0;
			} 	
		}
		else
		{
		 	bcd = 0x00;
		}	 	
	}
	else
	{
	 	if(bcd < 0x59)
		{
			bcd += 0x01;
			if((bcd&0x0F) == 0x0A)
			{
			 	bcd += 0x10;
				bcd &= 0xF0;
			} 
		}
		else
		{
		 	bcd = 0x00;
		}
	}
	return bcd;
}

u8 DecBcd(u8 bcd, bit mode) //1-0~23; 0-0~59
{
 	if(mode)
	{
		if(bcd > 0x00)
		{
			bcd -= 0x01;
			if((bcd&0x0F) == 0x0F)
			{
				bcd &= 0xF9;
			} 	
		}
		else
		{
		 	bcd = 0x23;
		}	 	
	}
	else
	{
		if(bcd > 0x00)
		{
			bcd -= 0x01;
			if((bcd&0x0F) == 0x0F)
			{
				bcd &= 0xF9;
			} 	
		}
		else
		{
		 	bcd = 0x59;
		}
	}
	return bcd;
}


void IncTime()
{
	switch(setIndex)
	{
		case 1: buffTime.hour = IncBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = IncBcd(buffTime.min,  0); break;
		case 3: buffTime.sec  = IncBcd(buffTime.sec,  0); break;
		default: break;
	}	
	RefreshTime(0);
}

void DecTime()
{
	switch(setIndex)
	{
		case 1: buffTime.hour = DecBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = DecBcd(buffTime.min,  0); break;
		case 3: buffTime.sec  = DecBcd(buffTime.sec,  0); break;
		default: break;
	}	
	RefreshTime(0);
}

void IncAlarm()
{
	switch(setIndex)
	{
		case 1: AlarmHour = IncBcd(AlarmHour, 1); break;
		case 2: AlarmMin  = IncBcd(AlarmMin,  0); break;
		case 3: AlarmSec  = IncBcd(AlarmSec,  0); break;
		default: break;
	}	
	RefreshAlarm();
}

void DecAlarm()
{
	switch(setIndex)
	{
		case 1: AlarmHour = DecBcd(AlarmHour, 1); break;
		case 2: AlarmMin  = DecBcd(AlarmMin,  0); break;
		case 3: AlarmSec  = DecBcd(AlarmSec,  0); break;
		default: break;
	}	
	RefreshAlarm();
}
						

void KeyAction(u8 keycode)
{
	if((flagAlarm==1) && (SystemSta==E_NORMAL))//ʱ����ʾģʽ�£����ⰴ�����£�������ֹͣ����
	{
		staMute = 1;
		P2 = (P2&0x1F)|0x80;//ȷ��ֹͣ�����ʱ�������ģ�
		P0 = 0xFF;
		P2 = P2&0x1F;	
	}
 	else if(keycode == '1')
	{
		SwitchTimeSet();	
	}
	else if(keycode == '4')
	{
	 	SwitchAlarmSet();
	}
	else if(keycode == '7')
	{
		if(SystemSta == E_SET_TIME)
		{
	 		IncTime();
		}
		else if(SystemSta == E_SET_ALARM)
		{
		 	IncAlarm();
		}
	}
	else if(keycode == '0')
	{
	 	if(SystemSta == E_SET_TIME)
		{
		 	DecTime();
		}
		else if(SystemSta == E_SET_ALARM)
		{
		 	DecAlarm();
		}
	}
}