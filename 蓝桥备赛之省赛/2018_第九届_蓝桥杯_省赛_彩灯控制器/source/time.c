#include "config.h"
#include "led.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "main.h"
#include "keyboard.h"

struct sTime buffTime;

u16 LedCnt;//ָʾ�Ƶ���ת�������Χ400~1200ms
u8 LedMode = 1;//LED����תģʽ     ��Χ1~4
bit LedSta = 0;//LED��ת����
//״̬����
u8 StaCnt = 0;//0-����״̬ 1-ѡ������ģʽ 2-ѡ����ת���

void RefreshTime(bit sta)//����1302�ײ��õ�
{
 	if(sta)
	{
	 	GetRealTime(&buffTime);
	}
	LedBuff[7] = LedChar[buffTime.hour>>4];
	LedBuff[6] = LedChar[buffTime.hour&0x0F];
	LedBuff[5] = 0xBF;
	LedBuff[4] = LedChar[buffTime.min>>4];
	LedBuff[3] = LedChar[buffTime.min&0x0F];
	LedBuff[2] = 0xBF;
	LedBuff[1] = LedChar[buffTime.sec>>4];
	LedBuff[0] = LedChar[buffTime.sec&0x0F];
}

void RefreshTemp()//����18b20�ײ��õ�
{
	int temp, tempBuff;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;

	if((temp>0) && (temp<99))
	{
	 	tempBuff = temp;
	}

	LedBuff[1] = LedChar[tempBuff/10];
	LedBuff[0] = LedChar[tempBuff%10];
}

void CloseLed()//�ر����е������
{
	LedBuff[7] = 0xFF;
	LedBuff[6] = 0xFF;
	LedBuff[5] = 0xFF;
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[2] = 0xFF;
	LedBuff[1] = 0xFF;
	LedBuff[0] = 0xFF;
}

void RefreshSet()//ˢ�´������ý����µ������
{
	//�������ý���������״̬
	LedBuff[7] = 0xBF;
	//����ģʽ��ʾ
	LedBuff[6] = LedChar[LedMode];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;
	
	//��ת�����ʾ
	ShowNumber(LedCnt);
}

void RefreshLevel()//�ȼ�����ˢ����ʾ
{
	LedBuff[7] = 0xFF;
	LedBuff[6] = 0xFF;
	LedBuff[5] = 0xFF;
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[2] = 0xFF;
	
	LedBuff[1] = 0xBF;
	LedBuff[0] = LedChar[LedLevel];
}

void SetLedCnt()//����LED������ģʽ����ת�������
{	
	if(StaCnt < 2)
	{
		StaCnt++;
		SysSta = E_SET;
		
		LedCnt = ModeLedCnt[LedMode-1];
		RefreshSet();//ˢ�����ý����µ������
	}
	else 
	{
		//������������״̬���ر�ȫ�������
		StaCnt = 0;
		SysSta = E_RUN;
		
		CloseLed();//�ر�ȫ�������
	}
}

void UintAdd()//��Ӧ��Ԫ(����ģʽ����ת���)�ļ�
{
	if(StaCnt == 1)
	{
		if(LedMode < 4)
		{
			LedMode++;
			
			LedCnt = ModeLedCnt[LedMode-1];
			RefreshSet();
		}
		else
		{
			LedMode = 1;
			
			LedCnt = ModeLedCnt[0];
			RefreshSet();
		}
		
	}
	else if(StaCnt == 2)
	{
		if(LedCnt < 1200)
		{
			LedCnt += 100;
			ModeLedCnt[LedMode-1] = LedCnt;
			RefreshSet();
		}
	}
}

void UintSub()//��Ӧ��Ԫ(����ģʽ����ת���)�ļ�
{
	if(StaCnt == 1)
	{
		if(LedMode > 1)
		{
			LedMode--;
			
			LedCnt = ModeLedCnt[LedMode-1];
			RefreshSet();
		}
		else
		{
			LedMode = 4;
			
			LedCnt = ModeLedCnt[3];
			RefreshSet();
		}
		
	}
	else if(StaCnt == 2)
	{
		if(LedCnt > 400)
		{
			LedCnt -= 100;
			ModeLedCnt[LedMode-1] = LedCnt;
			RefreshSet();
		}
	}
}

void KeyAction(u8 keycode)//������������
{
	if(keycode == '1')//LED��ת��������ֹͣ
	{
		LedSta = ~LedSta;
	}
	else if(keycode == '4')//LED����ģʽ����ת�������
	{
		SetLedCnt();
	}
	else if(keycode == '7')//��
	{
		if(SysSta == E_SET)
		{
			UintAdd();
		}	
	}
	else if(keycode == '0')//��
	{
		if(SysSta == E_SET)
		{
			UintSub();
		}
	}
}

void Mode1()//����������
{
	static u8 cnt = 0;
		
	if(cnt <= 7)
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = ~(0x01<<cnt);
		PWMSta = ~(0x01<<cnt);//P0�ڵ�״̬�ݴ棬�õ���ʱ����PWM����
		P2 = P2&0x1F;
		cnt++;
	}
	else
	{
		cnt = 0;
		LedMode = 2;
	}
}

void Mode2()//����������
{
	static u8 cnt = 1;
		
	if(cnt <= 7)
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = ~(0x80>>cnt);
		PWMSta = ~(0x80>>cnt);
		P2 = P2&0x1F;
		cnt++;
	}
	else
	{
		cnt = 1;
		LedMode = 3;
	}	
}

void Mode3()//���������м���
{
	static u8 cnt = 0;
	
	if(cnt == 0)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0x7E;
		PWMSta = 0x7E;
		P2 = P2&0x1F;
	}
	else if(cnt == 1)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xBD;
		PWMSta = 0xBD;
		P2 = P2&0x1F;
	}
	else if(cnt == 2)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xDB;
		PWMSta = 0xDB;
		P2 = P2&0x1F;
	}
	else if(cnt == 3)
	{
		cnt = 0;
		LedMode = 4;
		
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xE7;
		PWMSta = 0xE7;
		P2 = P2&0x1F;
	}
	else{}	
}

void Mode4()//���м���������
{
	static u8 cnt = 0;
	
	if(cnt == 0)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xE7;
		PWMSta = 0xE7;
		P2 = P2&0x1F;
	}
	else if(cnt == 1)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xDB;
		PWMSta = 0xDB;
		P2 = P2&0x1F;
	}
	else if(cnt == 2)
	{
		cnt++;
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xBD;
		PWMSta = 0xBD;
		P2 = P2&0x1F;
	}
	else if(cnt == 3)
	{
		cnt = 0;
		LedMode = 1;
		
		P2 = (P2&0x1F) | 0x80;
		P0 = 0x7E;
		PWMSta = 0x7E;
		P2 = P2&0x1F;
	}
	else{}	
}

void LedDriver()//ָʾ��ģʽ
{	
	//ע��������ĳ�else if �л������ͣ�ٵ�����
	
	if(LedMode==1)
	{
		LedCnt = ModeLedCnt[0];
		Mode1();
	}
	if(LedMode==2)
	{
		LedCnt = ModeLedCnt[1];
		Mode2();
	}
	if(LedMode==3)
	{
		LedCnt = ModeLedCnt[2];
		Mode3();
	}
	if(LedMode==4)
	{
		LedCnt = ModeLedCnt[3];
		Mode4();
	}
}



