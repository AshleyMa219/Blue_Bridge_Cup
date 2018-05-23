/*******************************************************************************
* �ļ��������߽����֮��ѹƵ�ʲɼ�ϵͳ
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2018��5��22��
* ��  ע��EEPROM�еĶ�Ӧ��ַ�洢����Ϣ˵����
*	      0x00-��ѹ���� 0x01-��ѹ����
*         0x02-�������� 
*		  0x03-������ʱ 0x04-�����ķ� 0x05-��������
*		  
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "keyboard.h"
#include "time.h"
#include "ds1302.h"
#include "eeprom.h"
#include "pcf8591.h"
#include "main.h"

enum eSysSta SysSta = -1;//ϵͳ��ʼ��״̬�������κ�һ����������״̬����ʱ��Ӧ�ϵ�ϵͳΪȫ����������״̬

struct sTime buff;

u8 T1RH, T1RL;
bit flag200ms = 0;
bit flagLedTurn = 0;//�������˸��ʱ����־
bit flagLedOpen = 0;//�������˸���ؿ���
bit flagVolOpen = 0;//��ѹ���������ܿ���
bit flagVolTurn = 0;
u32 FreCnt = 0;     //Ƶ�ʼ���ʹ��
u32 FreNum = 0;		//���Ƶ��
u32 TimNum = 0;		//�������
u8 CurVol  = 0;	//��ǰ�ĵ�ѹ
u8 LastVol = 0; //�ϴεĵ�ѹ
bit flagQry1s = 0;
u8 TrigType = 0;//�������� 
u8 TrigHour=0, TrigMin=0, TrigSec=0;

void LedTurn();
void VolTurn();
void CloseFucker();
void ConfigTimer1(u16 ms);
void ConfigTimer0();

void main()
{
	EA = 1;
	CloseFucker();
	ConfigTimer1(1);
	ConfigTimer0();
	InitDS1302();
	
	E2Read(&VolUpLim, 0x00, 1);
	E2Read(&VolDoLim, 0x01, 1);
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			
			if((SysSta == E_CLK) && (SetClkIndex == 0))//�������н���ʱ����ʾ��ǰ�� �жϵ�ǰϵͳ״̬ && ʱ���������� 
			{
				RefreshTime(1);
			}
			
			else if((SysSta == E_VOL) && (SetVolIndex == 0))
			{
				RefreshVol();
			}
			
			else if(SysSta == E_FRE)
			{
				RefreshFre(FreMode);
			}
		}
		if(flagQry1s)
		{
			flagQry1s = 0;
			
			CurVol = GetADCVal(3);

			if((CurVol==VolUpLim) && (CurVol>LastVol))//��������0
			{
				TrigType = 0;
				
				GetRealTime(&buff);
				TrigHour = (u8)buff.hour;
				TrigMin  = buff.min;
				TrigSec  = buff.sec;
				E2Write(&TrigType, 0x02, 1);
				E2Write(&TrigHour, 0x03, 1);
				E2Write(&TrigMin,  0x04, 1);
				E2Write(&TrigSec,  0x05, 1);
				
			}
			else if((CurVol==VolDoLim) && (CurVol<LastVol))//��������1
			{
				TrigType = 1;
				
				GetRealTime(&buff);
				TrigHour = (u8)buff.hour;
				TrigMin  = buff.min;
				TrigSec  = buff.sec;
				E2Write(&TrigType, 0x02, 1);
				E2Write(&TrigHour, 0x03, 1);
				E2Write(&TrigMin,  0x04, 1);
				E2Write(&TrigSec,  0x05, 1);
			}
			LastVol = CurVol;//�����˸����ϴεĵ�ѹֵ
			
		}
		if(flagLedOpen) //Led��˸��
		{
			if(flagLedTurn)
			{
				flagLedTurn = 0;
				LedTurn();//LED��˸
			}
		}
		else if(flagVolOpen)
		{
			if(flagVolTurn)
			{
				flagVolTurn = 0;
				VolTurn();
			}
		}
	}
}

void LedTurn()
{
	static u8 LedTurnSta = 1;//����״̬
	
	if(LedTurnSta == 1)//��ǰ������״̬
	{
		LedTurnSta = 0;
		
		LedBuff[SetClkCursor]   = 0xFF;
		LedBuff[SetClkCursor-1] = 0xFF;
	}
	else
	{
		LedTurnSta = 1;
		
		RefreshTime(0);
	}
}

void VolTurn()
{
	static u8 VolTurnSta = 1;
	
	if(VolTurnSta == 1)
	{
		VolTurnSta = 0;
		LedBuff[SetVolCursor]   = 0xFF;
		LedBuff[SetVolCursor-1] = 0xFF;
		LedBuff[SetVolCursor-2] = 0xFF;
		LedBuff[SetVolCursor-3] = 0xFF;
	}
	else
	{
		VolTurnSta = 1;
		
		RefreshVolSet();
	}
}

void CloseFucker()
{
	P2 = (P2&0x1F) | 0xA0;
	P0 &= 0xAF;
	P2 = P2&0x1F;
}

void ConfigTimer0()
{
	TH0 = 0xFF;
	TL0 = 0xFF;
	TMOD &= 0xF0;
	TMOD |= 0x04;
	ET0 = 1;
	TR0 = 1;
}

void ConfigTimer1(u16 ms)
{
	u32 tmp;
	
	tmp = 11059200/12;
	tmp = (tmp*ms) / 1000;
	tmp = 65536 - tmp;
	T1RH = (u8)(tmp>>8);
	T1RL = (u8)tmp;
	TMOD &= 0x0F;
	TMOD |= 0x10;
	TH1 = T1RH;
	TL1 = T1RL;
	ET1 = 1;
	TR1 = 1;
}

void InterruptTimer0() interrupt 1
{
	FreCnt++;
}

void InterruptTimer1() interrupt 3
{
	static u16 tmrQry1s = 0;//������¼
	static u16 tmrFre1s = 0;
	static u8 tmr200ms = 0;
	static u16 tmrLedTurn = 0;//����ܵļ����˸����
	static u16 tmrVolTurn = 0;//��ѹ������˸����
	
	TH1 = T1RH;
	TL1 = T1RL;
	tmr200ms++;
	
	if(tmr200ms >= 200)
	{
		tmr200ms = 0;
		flag200ms = 1;
	}
	
	tmrLedTurn++;
	tmrVolTurn++;
	tmrFre1s++;
	tmrQry1s++;
	if(tmrLedTurn >= 1000)
	{
		tmrLedTurn = 0;
		flagLedTurn = 1;//LED��˸��־λ
	}
	if(tmrVolTurn >= 1000)
	{
		tmrVolTurn = 0;
		flagVolTurn = 1;
	}
	if(tmrFre1s >= 1000)
	{
		tmrFre1s = 0;
		FreNum = FreCnt;
		FreCnt = 0;		//�ǵ����㰡������
	}
	if(tmrQry1s >= 1000)
	{
		tmrQry1s = 0;
		flagQry1s = 1;
	}
	
	LedScan();
	KeyScan();
}
