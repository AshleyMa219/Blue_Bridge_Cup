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
#include "main.h"
#include "ds18b20.h"

bit staMute = 0;//������־λ 0-�Ǿ��� 1-����
bit flagAlarm = 0;//���Ӷ�ʱ���ı�־

u8 AlarmHour = 0x00;//����-ʱ����
u8 AlarmMin  = 0x00;
u8 AlarmSec  = 0x00;
struct sTime buffTime;//ʱ�仺����

struct sTime setTime;//����������ģʽ�£�����ʱ��Ļ�����

u8 setIndex = 0; //��ʼ����������Ϊ0��	0-ʱ����ʾ 1-����'ʱ' 2-����'��' 3-����'��'
bit flagCursorCnt = 0;

u8 AlarmCnt = 0;//!!!!!!!!!!!!!!!!!!!			  

void AlarmMonitor()
{
	static bit AlarmSta = 0;

 	if((buffTime.hour == AlarmHour) && (buffTime.min == AlarmMin) && (buffTime.sec == AlarmSec))
	{ //!!!�˿�ʱ��ʱ�������ʱ��һ��
		flagAlarm = 1;
	}
	if(flagAlarm)
	{
		AlarmCnt++;
		if(!staMute)//��ǰ�ǷǾ���ģʽ
		{
			if(!AlarmSta)//��ǰ�����״̬
			{
				AlarmSta = 1;
			 	P2 = (P2&0x1F) | 0x80;
				P0 = 0xFE;
				P2 = P2&0x1F;
			}	
			else 
			{
				AlarmSta = 0;
			 	P2 = (P2&0x1F) | 0x80;
				P0 = 0xFF;
				P2 = P2&0x1F;
			}
		}
		if(AlarmCnt == 25)///!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			AlarmCnt = 0;
		 	staMute = 1;
			flagAlarm = 0;
			P2 = (P2&0x1F)|0x80;
			P0 = 0xFF;
			P2 = P2&0x1F;
		}
	}
}

void ShowLedNumber(u8 index, u8 num)
{
	if(num == 0xBF)//��2��5����ܵ�'-'���⴦��
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
	if(sta==1)//1��ʾ��ȡʵʱʱ���ˢ��
	{
	 	GetRealTime(&buffTime);//������ע�����ˢ��ʱ��ķ�װ�Ǵ���ȡʵʱʱ���
	}
	ShowLedNumber(7, buffTime.hour>>4);
	ShowLedNumber(6, buffTime.hour&0x0F);
	ShowLedNumber(5, 0xBF);
	ShowLedNumber(4, buffTime.min>>4);
	ShowLedNumber(3, buffTime.min&0x0F);
	ShowLedNumber(2, 0xBF);
	ShowLedNumber(1, buffTime.sec>>4);
	ShowLedNumber(0, buffTime.sec&0x0F);
}

void RefreshAlarm()
{
 	ShowLedNumber(7, AlarmHour>>4);
	ShowLedNumber(6, AlarmHour&0x0F);
	ShowLedNumber(5, 0xBF);
	ShowLedNumber(4, AlarmMin>>4);
	ShowLedNumber(3, AlarmMin&0x0F);
	ShowLedNumber(2, 0xBF);
	ShowLedNumber(1, AlarmSec>>4);
	ShowLedNumber(0, AlarmSec&0x0F);	
}

void RefreshSet()//ˢ�¹����ʾ��λ��
{
	switch(setIndex)// 1-ʱ 2-�� 3-��
	{
	 	case 1: LedSetCursor(1);  break;
		case 2: LedSetCursor(2);  break;
		case 3: LedSetCursor(3);  break;
		default: break;
	} 	
}

void Refresh18B20()
{
 	int temp;
	u8 i;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;//����С��λ
	
	for(i=7; i>=3; i--)
	{
	 	LedBuff[i] = 0xFF;
	}

	LedBuff[2] = LedChar[temp/10];//��������Ȼ����LedChar...
	LedBuff[1] = LedChar[temp%10];
	LedBuff[0] = 0xC6; //��Ӧ�ַ�'C'
}

u8 IncBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		if(bcd < 0x09)
		 	bcd += 0x01;
		else if(bcd == 0x09)
		 	bcd = 0x10;
		else if(bcd < 0x19)
		 	bcd += 0x01;
		else if(bcd == 0x19)
			bcd = 0x20;
		else if(bcd < 0x23)
		 	bcd += 0x01;	 	
		else 
		 	bcd = 0x00;
	}
	else//0~59
	{
	 	if(bcd < 0x09)
			bcd += 0x01;
		else if(bcd == 0x09)
			bcd = 0x10;
		else if(bcd < 0x19)
			bcd += 0x01;
		else if(bcd == 0x19)
			bcd = 0x20;
		else if(bcd < 29)
			bcd += 0x01;
		else if(bcd == 0x29)
			bcd = 0x30;
		else if(bcd < 39)
			bcd += 0x01;
		else if(bcd == 0x39)
			bcd =0x40;
		else if(bcd < 0x49)
			bcd += 0x01;
		else if(bcd == 0x49)
			bcd = 0x50;
		else if(bcd < 0x59)
			bcd+= 0x01;
		else
			bcd = 0x00;
	} 

	return bcd;	
}
u8 DecBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		if(bcd > 0x20)
			bcd -= 0x01;
		else if(bcd == 0x20)
			bcd = 0x19;
		else if(bcd > 0x10)
			bcd -= 0x01;
		else if(bcd == 0x10)
			bcd = 0x09;
		else if(bcd > 0x00)
			bcd -= 0x01;
		else 
			bcd = 0x23;
	}
	else//0~59
	{
		if(bcd > 0x50)
			bcd -= 0x01;
		else if(bcd == 0x50)
			bcd = 0x49;
		else if(bcd > 0x40)
			bcd -= 0x01;
		else if(bcd == 0x40)
			bcd = 0x39;
		else if(bcd > 0x30)
			bcd -= 0x01;
		else if(bcd == 0x30)
			bcd = 0x29;
		else if(bcd > 0x20)
			bcd -= 0x01;
		else if(bcd == 0x20)
			bcd = 0x19;
		else if(bcd > 0x10)
			bcd -= 0x01;
		else if(bcd == 0x10)
			bcd = 0x09;
		else if(bcd > 0x00)
			bcd -= 0x01;
		else 
			bcd = 0x59;
	} 

	return bcd;		
}

void IncTimeSet()
{
 	switch(setIndex)
	{
		case 1: buffTime.hour = IncBcd(buffTime.hour, 1);  break;
		case 2: buffTime.min  = IncBcd(buffTime.min,  0);  break;
		case 3: buffTime.sec  = IncBcd(buffTime.sec,  0);  break;
	    default : break;
	}
	RefreshTime(0);
}

void IncAlarmSet()
{
 	switch(setIndex)
	{
		case 1: AlarmHour = IncBcd(AlarmHour, 1);  break;
		case 2: AlarmMin  = IncBcd(AlarmMin,  0);  break;
		case 3: AlarmSec  = IncBcd(AlarmSec,  0);  break;
	    default : break;
	}
	RefreshAlarm();
}

void DecTimeSet()
{
 	switch(setIndex)
	{
		case 1: buffTime.hour = DecBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = DecBcd(buffTime.min,  0);  break;
		case 3: buffTime.sec  = DecBcd(buffTime.sec,  0);  break;
	    default : break;
	}
	RefreshTime(0);
}

void DecAlarmSet()
{
 	switch(setIndex)
	{
		case 1: AlarmHour = DecBcd(AlarmHour, 1);  break;
		case 2: AlarmMin  = DecBcd(AlarmMin,  0);  break;
		case 3: AlarmSec  = DecBcd(AlarmSec,  0);  break;
	    default : break;
	}
	RefreshAlarm();
}

void SwitchTimeSet()
{																
 	if(staSystem == E_NORMAL)
	{
	 	staSystem = E_SET_TIME;
		setIndex = 1;//����������ӦΪ - 'ʱ'
		
		setTime.hour = buffTime.hour;//������
		setTime.min = buffTime.min;//������
		setTime.sec = buffTime.sec;//������

		RefreshSet();
		flagCursorCnt = 0;
		LedOpenCursor();
	}	
	else if(staSystem == E_SET_TIME)
	{
	 	setIndex++;
		if(setIndex < 4)
		{
		 	RefreshSet();
		}
		else//һ���ӵ�4����ʾ�˳�ʱ�����ã�����ʱ����ʾģʽ��
		{
		 	setIndex = 0; 
			staSystem = E_NORMAL;//����������4�����˻ص�ʱ����ʾģʽ��
			if((setTime.hour == buffTime.hour) && (setTime.min == buffTime.min) && (setTime.sec == buffTime.sec))
			{//��������Ȼ����ʱ������ģʽ�����Ǹ���û�ı�ʱ�䣡
			 	RefreshTime(1);
			}
			else
			{//�������ı���ʱ��
				SetRealTime(&buffTime);//�������õ�ʱ��
			}
			LedCloseCursor();//�رչ��
		}	
	}
}

void SwitchAlarmSet()
{																
 	if(staSystem == E_NORMAL)
	{
	 	staSystem = E_SET_ALARM;
		setIndex = 1;//����������ӦΪ - 'ʱ'
		RefreshSet();
		flagCursorCnt = 0;
		LedOpenCursor();
	}	
	else if(staSystem == E_SET_ALARM)
	{
	 	setIndex++;
		if(setIndex < 4)
		{
		 	RefreshSet();
		}
		else//һ���ӵ�4����ʾ�˳��������ã�����ʱ����ʾģʽ��
		{
		 	setIndex = 0; 
			staSystem = E_NORMAL;//����������4�����˻ص�ʱ����ʾģʽ��
			staMute = 0;//!!!6666666666666һ�����Ӿ���֮��������һ�μ����ٶ�һ�����ӣ�
			RefreshTime(1);//�����л���ʱ����ʾģʽ��ֱ��ˢ�¾��У�
			LedCloseCursor();//�رչ��
		}	
	}
}


void KeyAction(u8 keycode)
{
	if((staSystem == E_NORMAL) && flagAlarm)//�������������1����ǰģʽ�ǡ�ʱ����ʾ�� 2�����Ӷ�ʱ�� ����
	{
	 	staMute = 1;
		flagAlarm = 0;
		AlarmCnt = 0;//!!!!!!!!!!!!!!!!!!!!!!!!!!
		P2 = (P2&0x1F)|0x80;
		P0 = 0xFF;
		P2 = P2&0x1F;
	}

	else if(keycode == '1')//S7
	{
	 	SwitchTimeSet();	
	}
	else if(keycode == '4')//S6
	{
		SwitchAlarmSet();
	}
	else if(keycode == '7')//S5
	{
		if(staSystem == E_SET_TIME)
		{
		 	IncTimeSet();
		}
		else if(staSystem == E_SET_ALARM)
		{
		 	IncAlarmSet();
		}
	}
	else if(keycode == '0')//S4
	{
		if(staSystem == E_SET_TIME)
		{
		 	DecTimeSet();
		}
		else if(staSystem == E_SET_ALARM)
		{
		 	DecAlarmSet();
		}
	}
}
