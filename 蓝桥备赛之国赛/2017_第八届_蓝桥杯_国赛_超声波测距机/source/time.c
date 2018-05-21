#include "config.h"
#include "led.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "main.h"
#include "eeprom.h"

struct sTime buffTime;

u8 ViewIndex = 0;
u8 SaveDataCnt = 0;

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

void CloseLed()
{
	LedBuff[0] = 0xFF;
	LedBuff[1] = 0xFF;
	LedBuff[2] = 0xFF;
	LedBuff[3] = 0xFF;
	LedBuff[4] = 0xFF;
	LedBuff[5] = 0xFF;
	LedBuff[6] = 0xFF;
	LedBuff[7] = 0xFF;
}

void RefreshView()
{
	u8 dat;
	
	LedBuff[7] = LedChar[ViewIndex/10];
	LedBuff[6] = LedChar[ViewIndex%10];
	
	E2Read(&dat, ViewIndex, 1);
	LedBuff[0] = LedChar[dat%10];
	LedBuff[1] = LedChar[dat/10%10];
	LedBuff[2] = LedChar[dat/100%10];
}

void RefreshSet()//ˢ�����������������
{
	LedBuff[7] = 0x8E;
	
	LedBuff[0] = LedChar[FixDistance%10];
	LedBuff[1] = LedChar[FixDistance/10];
}

void FixData()//��������
{
	if(SysSta == E_RUN)//�����ǰϵͳ״̬�Ǿ��������Ȼ��ת����������
	{
		SysSta = E_SET;
		P2 = (P2&0x1F) | 0x80;//����LED8
		P0 = 0x7F;
		P2 = (P2&0x1F);
		CloseLed();
		RefreshSet();
	}
	else if(SysSta == E_SET)//�����ǰϵͳ��״̬���������棬�Ǿ����»ص������������
	{
		SysSta = E_RUN;
		P2 = (P2&0x1F) | 0x80;//�ر�LED8
		P0 = 0xFF;
		P2 = (P2&0x1F);
	}
}

void SaveData()
{
	u8 dat;
	
	if(distance < 255)
	{
		dat = distance;//ɵ�����ң��Ѹ�ֵ���򶼸���ˡ�����
	}
	
	E2Write(&dat, SaveDataCnt, 1);
	OpenLed1 = 1;//������һ�����ݴ�LED1�Ŀ���
	SaveDataCnt++;//10�����ݼ���
	
	if(SaveDataCnt >= 10)//����10�����ݣ�����
	{
		SaveDataCnt = 0;
	}
}

void VeiwData()
{
	if(SysSta == E_RUN)
	{
		SysSta = E_VIEW;//�Ӿ�����������л����洢��ʾ����
		P2 = (P2&0x1F) | 0x80;//����LED7
		P0 = 0xBF;
		P2 = (P2&0x1F);
		SaveDataCnt = 0;//��վ�����������´洢���ݼ�������
		
		CloseLed();
		RefreshView();
	}
	else if(SysSta == E_VIEW)
	{
		SysSta = E_RUN;
		P2 = (P2&0x1F) | 0x80;//�ر�LED7
		P0 = 0xFF;
		P2 = (P2&0x1F);
		ViewIndex   = 0;//�˳��洢��ʾ����ͬʱ������������
	}
}

void KeyAction(u8 keycode)
{
 	if(keycode == '0')
	{
		if(SysSta == E_RUN)
		{
			SaveData();//�洢��ǰ����
			Led1Cnt = 0;//�������ԭ�� - LED1ÿ��һ�����ݵ���˸����1s��Ϊ�����������α���ļ��С��1sʱ��LED1���ܸ������һ�ΰ��������ó���ʱ�䣡
		}
	}
	else if(keycode == '7')
	{
		VeiwData();//�鿴�洢������
	}
	else if(keycode == '1')
	{
		if(SysSta == E_VIEW)
		{
			ViewIndex++;//�洢���ݷ�ҳ
			if(ViewIndex >= 10)
			{
				ViewIndex = 0;
			}
			RefreshView();
			
		}
		if(SysSta == E_SET)
		{	//���������Լ�
			FixDistance += 10;
			
			if(FixDistance >= 100)
			{
				FixDistance = 0;
			}
			
			RefreshSet();
		}
	}
	else if(keycode == '4')
	{//��������
		FixData();
	}
}