#include "config.h"
#include "led.h"
#include "ds18b20.h"

bit SetSta = 0;
u8 TempScale = 0;//�¶�����
u8 MaxTemp = 28;//Ĭ���¶����� 20~30
u8 MinTemp = 20;
char SetMaxTemp = 0;
char SetMinTemp = 0;
u8 SetCnt = 0;

void RefreshTemp()//�¶���ʾ����
{
 	int temp;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;

	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[TempScale];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;
	LedBuff[3] = 0xFF; 
	LedBuff[2] = 0xFF;
	LedBuff[1] = LedChar[temp/10];
	LedBuff[0] = LedChar[temp%10];
}

void EnterSet()//���ý���ĳ�ʼ��
{
	LedBuff[7] = 0xBF;
	LedBuff[6] = 0xFF;
	LedBuff[5] = 0xFF;

	LedBuff[4] = 0xFF; 
	LedBuff[3] = 0xFF;

	LedBuff[2] = 0xBF;
	LedBuff[1] = 0xFF;
	LedBuff[0] = 0xFF;		    	
}

void RefreshSetTemp()//������ʾ����ˢ��
{
 	LedBuff[6] = LedChar[SetMaxTemp/10];//�������ʾ�¶�����
	LedBuff[5] = LedChar[SetMaxTemp%10];

	LedBuff[1] = LedChar[SetMinTemp/10];//�������ʾ�¶�����
	LedBuff[0] = LedChar[SetMinTemp%10];
}

void TempMonitor()
{
 	int temp;

	Get18B20Temp(&temp);
	Start18B20();
	temp >>= 4;
	
	if(temp < MinTemp)
	{
		TempScale = 0;	 	
	}	
	else if(temp > MaxTemp)
	{
	 	TempScale = 2;
	}
	else
	{
	 	TempScale = 1;
	}

	if(!SetSta)//��ǰû�д�������ģʽ��ˢ���¶Ƚ���
	{
		RefreshTemp();
	}
}

void SaveSet()//�˳����ý��棬�������õ�����
{
	if(SetMaxTemp < SetMinTemp)
	{
	 	SetCnt = 0;
		SetMaxTemp = 0;
		SetMinTemp = 0;
		P0 = 0xFF;
		OpenLed(2);	
	}
	else
	{
		SetSta = 0;
		MaxTemp = SetMaxTemp;
		MinTemp = SetMinTemp;
		SetCnt = 0;//�˳�����ǵð������޵�����λ��0
		SetMaxTemp = 0;//��ᵽ��һ�������С�������ĺô��˰ɡ�����
		SetMinTemp = 0; 
		P0 = 0xFF;
		CloseLed(2);
	}	
}

void ChangeTemp(u8 keycode)
{
	if(SetCnt <= 1)//	0/1��ʾ�����¶�����
	{
		SetMaxTemp = (SetMaxTemp * 10) + (keycode - '0');
		if(SetMaxTemp > 99)
		{
		 	SetMaxTemp = 99;	
		} 	
	}
	else if(SetCnt <= 3)//	2/3��ʾ�����¶�����
	{
	 	SetMinTemp = (SetMinTemp * 10) + (keycode - '0');
		if(SetMinTemp > 99)
		{
		 	SetMinTemp = 99;	
		} 		
	}
	SetCnt++;
	RefreshSetTemp();
}

void ClearTemp()
{
	if((SetCnt >= 1) && (SetCnt <= 4))//�Ϸ��Լ���
	{
		SetCnt -= 1;
		if((SetCnt >= 0) && (SetCnt <= 1))
		{
		 	SetMaxTemp /= 10;	
		}
		else if((SetCnt >= 2) && (SetCnt <= 3))
		{
			SetMinTemp /= 10; 	
		}
		else
		{}
		RefreshSetTemp();
	}
}

void KeyAction(u8 keycode)
{
 	if(keycode == 0x1B)//�������ü�
	{
		P0 = 0xFF;
		CloseLed(1);//һ����������ҳ�棬�ر�LED1
		if(SetSta == 0)
		{
			SetSta = 1;
			EnterSet();//�������ý���
		}
		else
		{
			SaveSet();//�˳����ý��棬�������õ�������	
		}		
	}
	else if((keycode >= '0') && (keycode <= '9'))
	{
	 	if(SetSta)//�������ּ�����ǰ��������ģʽ
		{	
			ChangeTemp(keycode);//�����������¶�
		}	
	}
	else if(keycode == 0x0D)//�����������
	{
	 	if(SetSta)
		{
			ClearTemp();	 	
		}
	}
}