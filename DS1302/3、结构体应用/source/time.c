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

u8 setIndex = 0;//ʱ����������
struct sTime buffTime;//ʱ�仺����

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

u8 IncBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		bcd += 0x01;
		if((bcd&0x0F) == 0x0A)
		{
			bcd &= 0xF0;
		 	bcd += 0x10;
		}
		if(bcd == 0x24)
			bcd = 0x00;
	}
	else//0~59
	{
		bcd += 0x01;
		if((bcd&0x0F) == 0x0A)
		{
			bcd &= 0xF0;
		 	bcd += 0x10;
		}
		if(bcd == 0x60)
			bcd = 0x00;
	} 

	return bcd;	
}

u8 DecBcd(u8 bcd, u8 i)
{
	if(i == 1)//0~23
	{
		bcd -= 0x01;
		if((bcd&0x0F) == 0x0F)
		{
		 	bcd = (bcd&0xF0)|0x09;
		}
		if(bcd == 0xF9)
		{
		 	bcd = 0x23;
		}
	}
	else//0~59
	{
		bcd -= 0x01;
		if((bcd&0x0F) == 0x0F)
		{
		 	bcd = (bcd&0xF0)|0x09;
		}
		if(bcd == 0xF9)
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

void RightShiftTimeSet()
{
 	if(setIndex != 0)
	{
		if(setIndex < 3)
			setIndex++;
		else
			setIndex = 1;
		RefreshSet();
	}	
}

void LeftShiftTimeSet()
{
 	if(setIndex != 0)
	{
		if(setIndex > 1)
			setIndex--;
		else
			setIndex = 3;
		RefreshSet();
	}	
}

void EnterTimeSet()
{
	setIndex = 1;
	RefreshSet();
	LedOpenCursor();
}

void ExitTimeSet(bit save)
{
 	setIndex = 0;
	if(save)
	{
	 	SetRealTime(&buffTime);
	}
	LedCloseCursor();
}

void KeyAction(u8 keycode)
{
 	if((keycode >= '0') && (keycode <= '9'))
	{
	}
	else if(keycode == 0x19)//��
	{
	 	IncTimeSet();
	}
	else if(keycode == 0x18)//��
	{
	 	DecTimeSet();
	}
	else if(keycode == 0x17)//����
	{
	 	LeftShiftTimeSet();
	}
	else if(keycode == 0x16)//����
	{
	 	RightShiftTimeSet();
	}
	else if(keycode == 0x0D)
	{
	 	if(setIndex == 0)
		{
			EnterTimeSet();
		}
		else
		{
		 	ExitTimeSet(1);//�˳�ʱ�����ò�����
		}
	}
	else if(keycode == 0x1B)
	{
	 	ExitTimeSet(0);//�˳�ʱ�����ò�����!
	}
}