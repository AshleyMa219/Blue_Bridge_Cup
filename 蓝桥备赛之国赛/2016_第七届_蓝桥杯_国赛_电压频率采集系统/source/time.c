#include "config.h"
#include "ds1302.h"
#include "led.h"
#include "ds18b20.h"
#include "main.h"
#include "pcf8591.h"
#include "eeprom.h"

struct sTime buffTime;
u8 VolUpLim = 20;//��ѹ����ĵ�ѹ����
u8 VolDoLim = 10;//��ѹ����ĵ�ѹ����

u8 SetClkIndex = 0; //0-������ģʽ 1-ʱ����   2-������ 3-������
u8 SetClkCursor = 0;//0-�ǹ������ 7-��Ӧʱ   4-��Ӧ�� 1-��Ӧ��

u8 SetVolIndex = 0; // 0-������ģʽ 1-�������� 2-��������
u8 SetVolCursor = 0;// 0-�ǹ������ 7-�������� 3-��������

bit FreMode = 0;//0-��ʾƵ�� 1-��ʾ����
bit QryMode = 0;//0-�������� 1-����ʱ��

void RefreshTime(bit sta)
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

void RefreshTemp()
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

void RefreshVol()
{
	u8 val;
	
	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[1];
	LedBuff[5] = 0xBF;
	LedBuff[4] = 0xFF;
	LedBuff[0] = LedChar[0];
	LedBuff[1] = LedChar[0];
	
	val = GetADCVal(3);
	LedBuff[2] = LedChar[val%10];
	LedBuff[3] = LedChar[val/10];
}

void RefreshVolSet()
{
	LedBuff[0] = LedChar[0];
	LedBuff[1] = LedChar[0];
	LedBuff[4] = LedChar[0];
	LedBuff[5] = LedChar[0];
	
	LedBuff[2] = LedChar[VolDoLim%10];
	LedBuff[3] = LedChar[VolDoLim/10];
	LedBuff[6] = LedChar[VolUpLim%10];
	LedBuff[7] = LedChar[VolUpLim/10];
}

void RefreshFre(bit sta)
{
	LedBuff[7] = 0xBF;
	LedBuff[6] = LedChar[2];
	LedBuff[5] = 0xBF;
	
	if(sta == 0)//Ƶ��Сʱ
	{
		LedBuff[0] = LedChar[FreNum%10];
		LedBuff[1] = LedChar[FreNum/10%10];
		LedBuff[2] = LedChar[FreNum/100%10];
		LedBuff[3] = LedChar[FreNum/1000%10];
		LedBuff[4] = LedChar[FreNum/10000%10];
	}
	else
	{
		TimNum = 1000000 / FreNum;//Ƶ�ʶ�Ӧ�����ڣ���λus
		
		LedBuff[0] = LedChar[TimNum%10];
		LedBuff[1] = LedChar[TimNum/10%10];
		LedBuff[2] = LedChar[TimNum/100%10];
		LedBuff[3] = LedChar[TimNum/1000%10];
		LedBuff[4] = LedChar[TimNum/10000%10];
	}
}

void RefreshTrig(bit sta)
{
	if(sta == 0)//����������ʾ
	{
		LedBuff[7] = 0xFF;
		LedBuff[6] = 0xFF;
		LedBuff[5] = 0xFF;
		LedBuff[4] = 0xFF;
		LedBuff[3] = 0xFF;
		LedBuff[2] = 0xFF;
		LedBuff[1] = LedChar[0];
		LedBuff[0] = LedChar[TrigType];
	}
	else if(sta == 1)//����ʱ����ʾ
	{
		LedBuff[7] = LedChar[TrigHour>>4];
		LedBuff[6] = LedChar[TrigHour&0x0F]; 
		LedBuff[5] = 0xBF;
		LedBuff[4] = LedChar[TrigMin>>4];
		LedBuff[3] = LedChar[TrigMin&0x0F];
		LedBuff[2] = 0xBF;
		LedBuff[1] = LedChar[TrigSec>>4];
		LedBuff[0] = LedChar[TrigSec&0x0F];
	}
}

void ChangeFreMode()//Ƶ�ʺ����ڵ�ģʽ�л�
{
	if(FreMode == 0)
	{
		FreMode = 1;
	}
	else if(FreMode == 1)
	{
		FreMode = 0;
	}
	RefreshFre(FreMode);
}

void ChangeQryMode()//�������ͺʹ���ʱ���л�
{
	if(QryMode == 0)
	{
		QryMode = 1;
	}
	else if(QryMode == 1)
	{
		QryMode = 0;
	}
	RefreshTrig(QryMode);
}

u8 IncBcd(u8 bcd, bit i)
{
	bcd += 0x01;
	if((bcd&0x0F) == 0x0A)
	{
		bcd &= 0xF0;
		bcd += 0x10;
	}
	
	if(i == 1)
	{
		if(bcd == 0x24)//0~24
		{
			bcd = 0x00;
		}
	}
	else//0~59
	{
		if(bcd == 0x60)
		{
			bcd = 0x00;
		}
	}
	
	return bcd;
}

u8 DecBcd(u8 bcd, bit i)
{
	bcd -= 0x01;
	if((bcd&0x0F) == 0x0F)
	{
		bcd = (bcd&0xF0) | 0x09;
	}
	
	if(i == 1)
	{
		if(bcd == 0xF9)//ע����0-1=0xFF,����ǰ���жԵ�λ��F��Ϊ9�Ĳ��������������ж�0xF9����
		{
			bcd = 0x23;
		}
	}
	else
	{
		if(bcd == 0xF9)
		{
			bcd = 0x59;
		}
	}
	
	return bcd;
}

void UnitAdd()
{
	switch(SetClkIndex)
	{
		case 1: buffTime.hour = IncBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = IncBcd(buffTime.min,  0); break;
		case 3: buffTime.sec  = IncBcd(buffTime.sec,  0); break;
		default: break;
	}
}

void UnitSub()
{
	switch(SetClkIndex)
	{
		case 1: buffTime.hour = DecBcd(buffTime.hour, 1); break;
		case 2: buffTime.min  = DecBcd(buffTime.min,  0); break;
		case 3: buffTime.sec  = DecBcd(buffTime.sec,  0); break;
		default: break;
	}
}

void LedSetCursor()
{
	if(SetClkIndex == 1)//����Ϊʱ
	{
		SetClkCursor = 7;//���ù���Ӧ��ʱ
	}
	else if(SetClkIndex == 2)//����Ϊ��
	{
		SetClkCursor = 4;//���ù���Ӧ����
	}
	else if(SetClkIndex == 3)//����Ϊ��
	{
		SetClkCursor = 1;//���ù���Ӧ����
	}
	else//���������
	{
		SetClkCursor = 0;//����0
	}
}

void VolSetCursor()
{
	if(SetVolIndex == 1)
	{
		SetVolCursor = 7;
	}
	else if(SetVolIndex == 2)
	{
		SetVolCursor = 3;
	}
	else
	{
		SetVolCursor = 0;
	}
}

void VolAdd()
{
	if(SetVolIndex == 1)
	{
		VolUpLim += 5;
		if(VolUpLim >= 95)
		{
			VolUpLim = 95;
		}
		RefreshVolSet();
	}
	else if(SetVolIndex == 2)
	{
		VolDoLim += 5;
		if(VolDoLim >= 95)
		{
			VolDoLim = 95;
		}
		RefreshVolSet();
	}
}

void VolSub()
{
	if(SetVolIndex == 1)
	{
		VolUpLim -= 5;
		if(VolUpLim <= 5)
		{
			VolUpLim = 5;
		}
		RefreshVolSet();
	}
	else if(SetVolIndex == 2)
	{
		VolDoLim -= 5;
		if(VolDoLim <= 5)
		{
			VolDoLim = 5;
		}
		RefreshVolSet();
	}
}

void SaveLimData()
{
	E2Write(&VolUpLim, 0x00, 1);
	E2Write(&VolDoLim, 0x01, 1);
}

void KeyAction(u8 keycode)
{
	if(keycode == '1')//s7 -  ʱ��
	{
		SysSta = E_CLK;//ֻҪs7���±����ϵͳ����״̬
		if(flagLedOpen)//��������flagLedOpen��ֵ�����˳�ʱ������ģʽʱ����ʱ��
		{
			flagLedOpen = 0;//LED��˸���ش�
			SetRealTime(&buffTime);
		}
		SetClkIndex = 0;//LED��˸��������
		LedSetCursor(); //�����������ù�� - ��������Ŀ����Ϊ���´ν���ʱ������ʱ���ܴ�'ʱ'��ʼ��˸
						//Ȼ�����while(1)�������ˢ��
	}
	else if(keycode == '0')//s4
	{
		if(SysSta == E_CLK)//��ǰϵͳ״̬��ʱ�ӣ����º������ʱ������
		{
			if(flagLedOpen == 0)//ֻ�ڿ����ǹرյ�����´���
			{
				flagLedOpen = 1;
			}
			
			SetClkIndex++;//����ʱ��ʱ��ģʽÿ����һ��s4������ʱ�������ѯ
			if(SetClkIndex >= 4)//�߽紦��
			{
				SetClkIndex = 1;
			}
			LedSetCursor();//��������ֵ���ù��
		}
		
		else if(SysSta == E_VOL)//��ǰϵͳ״̬�ǵ�ѹ����
		{
			if(flagVolOpen == 0) //��ѹ����LED��˸����
			{
				flagVolOpen = 1;
			}
			
			SetVolIndex++;
			if(SetVolIndex >= 3)
			{
				SetVolIndex = 1;
			}
			VolSetCursor();
			RefreshVolSet();//��ѹ����ģʽ��ˢ�� ��ʱ��ģʽû��д��䣬����Ϊ��while(1)����ˢ�µģ�
		}
		else if(SysSta == E_FRE)
		{
			ChangeFreMode();
		}
		else if(SysSta == E_QRY)
		{
			ChangeQryMode();
		}
	}
	else if(keycode == '2')//s11 - ��
	{
		if((SysSta == E_CLK) && (SetClkIndex!=0))//����ʱ������ģʽ (ע��ʱ��ģʽ ������ ʱ������ģʽ)
		{
			UnitAdd();//��Ӧ��Ԫ�����Լ�
			RefreshTime(0);
		}
		else if((SysSta == E_VOL) && (SetVolIndex!=0))
		{
			VolAdd();
			RefreshVolSet();
		}
	}
	else if(keycode == '5')//s10 - ��
	{
		if((SysSta == E_CLK) && (SetClkIndex!=0))//����ʱ������ģʽ (ע��ʱ��ģʽ ������ ʱ������ģʽ)
		{
			UnitSub();//��Ӧ��Ԫ�����Լ�
			RefreshTime(0);
		}
		else if((SysSta == E_VOL) && (SetVolIndex!=0))
		{
			VolSub();
			RefreshVolSet();
		}
	}
	else if(keycode == '4')//s6 - ��ѹ����
	{
		SysSta = E_VOL;//�޸�ϵͳ״̬Ϊ��ѹ����
		if(flagVolOpen)//��������flagVolOpen������ʶ���Ƿ����˳���ѹ�������õĹ���
		{
			flagVolOpen = 0;
			SaveLimData();//�����ѹ����ֵ
		}
		RefreshVol();  //��ѹ����������ʾ
		SetVolIndex = 0;
		VolSetCursor();
	}
	else if(keycode == '7')//s5 - Ƶ�ʲ���
	{
		SysSta = E_FRE;
		RefreshFre(FreMode);
	}
	else if(keycode == '8')//s9 - ��ѯ����
	{
		SysSta = E_QRY;
		RefreshTrig(QryMode);
	}
}