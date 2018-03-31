#include "config.h"
#include "led.h"
#include "ds18b20.h"
#include "main.h"

bit staTemp = 0;//�¶���ʾ����״̬
u8 CountTime = 0;

void RefreshTemp()
{
	int temp;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;

	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[4];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;		
	LedBuff[3] = 0xFF;
	LedBuff[2] = LedChar[temp/10];
	LedBuff[1] = LedChar[temp%10];
	LedBuff[0] = 0xC6;
}

void RefreshMode(u8 mode)//���������ֱ��Ӧ ˯��ģʽ����Ȼ�硢����
{
 	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[mode+1];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;
	ShowNumber(CountTime);	
}

void IncTime()
{
	if(ET1 == 0)
	{
	 	ET1 = 1;
	}
 	if(CountTime > 120)
	{
	 	CountTime = 0;
	}
	else
	{
	 	CountTime += 60;
		if(CountTime > 120)
			CountTime = 0;
	}
	ShowNumber(CountTime);
}

void LedTurn(u8 x)
{
	P2 = (P2&0x1F) | 0x80;
	if(x == 0)
	{
	 	P0 = 0xFF;	
	}
	else if(x == 3)
	{
		P0 = 0xFB;	
	}
	else
	{
	 	P0 = ~(0x00 | x);	
	}
}

void SwitchMode()
{
 	if(staSys == E_SLEEP)
	{
	 	staSys = E_NATURE;
		RefreshMode(1);
		ConfigPWM(30);
		if(CountTime != 0)
			LedTurn(2);//������2��С��
	}
	else if(staSys == E_NATURE)
	{
	 	staSys = E_NORMAL;
		RefreshMode(2);
		ConfigPWM(70);
		if(CountTime != 0)
			LedTurn(3);	
	}
	else if(staSys == E_NORMAL)
	{
		staSys = E_SLEEP;
		RefreshMode(0);
		ConfigPWM(20);
		if(CountTime != 0)	
			LedTurn(1);
	}

}

void StopPWM()
{
 	ET1 = 0;
	CountTime = 0;
	ShowNumber(CountTime);
	PWMOUT = 0;
	LedTurn(0);//�ر�����С��
}

void KeyAction(u8 keycode)
{
	if(keycode == '0')//�л�����
	{
		SwitchMode();	
	}
	else if(keycode == '7')//��ʱ����
	{
		LedTurn(staSys+1);
	 	IncTime();
	}
	else if(keycode == '4')//ֹͣ����
	{
	 	StopPWM();
	}
	else if(keycode == '1')//���°���
	{
		if(staTemp == 0)
		{
			staTemp = 1;
	 		RefreshTemp();
		}
		else
		{
		 	staTemp = 0;
			RefreshMode(staSys);//ȫ����ɧ�����ڴˣ�����
		}
	}	
}