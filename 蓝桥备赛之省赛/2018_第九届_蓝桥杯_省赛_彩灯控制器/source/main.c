/*******************************************************************************
* �ļ������ھŽ�����ʡ��_�ʵƿ�����
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.1
* ��  ��: 2018��5��15�� 2018��5��17��
* ��  ע������ʵ�����й���
*         
*******************************************************************************
*/

#include "config.h"
#include "keyboard.h"
#include "led.h"
#include "time.h"
#include "ds18b20.h"
#include "ds1302.h"
#include "eeprom.h"
#include "pcf8591.h"
#include "main.h"

enum eSysSta SysSta = E_RUN;//ϵͳ����״̬���ϵ�Ĭ�ϴ�������ģʽ
u8 T0RH, T0RL;			    //��ʱ��T0������ֵ
bit flagLedCnt = 0;			//ָʾ�Ƶ���ת�����־������ָʾ���л���
bit flag800ms = 0;			//����״̬��ѡ�е�Ԫ��0.8sΪ���������˸
bit flag2s = 0;				//��������״̬��2s����һ����ת�����E2��
bit flag200ms = 0;			//ÿ��200ms��ȡһ�ε�ǰ�����ȵȼ�
u8 val = 0;					//��ȡRb2��ADֵ
u8 LedLevel = 1;			//���ȵȼ����ϵ�Ĭ��Ϊ1
u8 LedLevelDat[4] = {25, 50, 75, 100};//��ͬ�����ȣ�Խ��Խ��
u8 i, ModeCnt[4];			//i-E2���������õģ�ModeCnt����Ϊu8���������Բ�ͬ��תģʽ�ļ����С�洢
u16 ModeLedCnt[4] = {500, 500, 500, 500};//��ͬģʽ����ת���
u8 PWMSta = 0xFF;			//�ݴ�P0��״̬������PWM����ָʾ��

void GetLedLevel();
void SaveLedCnt();
void TurnOverLed();
void CloseFucker();
void ConfigTimer0(u16 ms);
void ConfigTimer1();

void main()
{
#if 0//��һ��д������ʱʹ�� (����ô�ż��أ���������΢��һ��ȷ���������д���ˣ�)
	
	//�Ը���ģʽ����ת�����С����
	for(i=0; i<4; i++)
	{
		ModeCnt[i] = ModeLedCnt[i] / 100;
	}
	E2Write(ModeCnt, 0x00, 4);
#endif

#if 1
	//ע����,���������ȡ����λ�ñ���������е��ж����ó���ǰ�棬�ϵ��ȶ�ȡE2�е���ת�������֤�������
	//�ϵ���ȡE2�е�����
	E2Read(ModeCnt, 0x00, 4);
	//�Ը���ģʽ����ת����Ŵ���
	for(i=0; i<4; i++)
	{
		ModeLedCnt[i]  = ModeCnt[i] * 100;
	}
	
 	EA = 1;//������ʱ����ʹ��
	CloseFucker();
	ConfigTimer0(1);//���ö�ʱ��0-1ms
	ConfigTimer1(); //���ö�ʱ��1-10us
	
	while(1)
	{
		KeyDriver();//��������ɨ��
		
		if(flag200ms)
		{
			flag200ms = 0;
			GetLedLevel();//ÿ200ms��ȡһ�����ȵȼ�
		}
		
		if(SysSta == E_RUN)//��ǰϵͳ������״̬������
		{
			if(LedSta)//LED��ת���ش򿪵�ǰ��
			{
				if(flagLedCnt)
				{
					flagLedCnt = 0;
					LedDriver();//ÿ����ת�������ָʾ�Ƶ��л�
				}	
			}
			
			if(flagLevel == 1)
			{
				RefreshLevel();//��������ģʽ����s4��ʱˢ�����ȵȼ�
			}
			else
			{
				CloseLed();//����Ļ��ر������
			}
		}
		
		if(SysSta == E_SET)//��ǰ��ϵͳ����״̬������
		{
			if(flag800ms)
			{
				flag800ms = 0;
				TurnOverLed();//����ģʽ�¶�Ӧ��Ԫ��˸��
			}
			//��������״̬��2s����һ����ת���
			if(flag2s == 1)
			{
				flag2s = 0;
				SaveLedCnt();//ÿ��2s��һ�µ�ǰ����ת���
			}
		}
	}
#endif
}

void GetLedLevel()//����Rb2��ADֵ����ȡ���ȵȼ�
{	
	val = GetADCVal(3);
	if(val <= 12)
	{
		LedLevel = 1;
	}
	else if(val <= 24)
	{
		LedLevel = 2;
	}
	else if(val <= 36)
	{
		LedLevel = 3;
	}
	else
	{
		LedLevel = 4;
	}
}

void TurnOverLed()//ʵ���������˸����Ч
{
	static bit LedCurSta=1;	
	
	if(LedCurSta)
	{	
		LedCurSta = 0;
		
		if(StaCnt == 1)
		{
			LedBuff[7] = 0xFF;
			LedBuff[6] = 0xFF;
			LedBuff[5] = 0xFF;
		}
		else if(StaCnt == 2)
		{
			LedBuff[3] = 0xFF;
			LedBuff[2] = 0xFF;
			LedBuff[1] = 0xFF;
			LedBuff[0] = 0xFF;
		}
	}
	else
	{
		LedCurSta = 1;
		
		if(StaCnt == 1)
		{
			LedBuff[7] = 0xBF;
			LedBuff[6] = LedChar[LedMode];
			LedBuff[5] = 0xBF;
		}
		else if(StaCnt == 2)
		{
			ShowNumber(LedCnt);
		}
	}
}

void SaveLedCnt()//�洢��ת���
{
	for(i=0; i<4; i++)
	{
		ModeCnt[i] = ModeLedCnt[i] / 100;
	}
	E2Write(ModeCnt, 0x00, 4);
}

void CloseFucker()//�رն��ĵķ�����
{
 	P2 = (P2&0x1F) | 0xA0;
	P0 &= 0xAF;	
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)//���ö�ʱ��0����λms
{
 	u32 tmp;

	tmp = 11059200/12;
	tmp = (tmp*ms) / 1000;
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

void ConfigTimer1()		//10΢��@11.0592MHz
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TMOD |= 0x10;		//���ö�ʱ��ģʽ
	TL1 = 0xF7;		//���ö�ʱ��ֵ
	TH1 = 0xFF;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
}

void InterruptTimer0() interrupt 1
{
	static u16 tmrLedCnt=0;
	static u16 tmr800ms = 0;
	static u16 tmr2s = 0;
	static u8 tmr200ms = 0;
	
 	TH0 = T0RH;
	TL0 = T0RL;
	tmr200ms++;
	if(tmr200ms >= 200)
	{
		tmr200ms = 0;
		flag200ms = 1;
	}
	
	if(SysSta == E_SET)//�������һ��ǰ�ᣬ�������ڴ��ڷ�����״̬�²���Ҫ���ж�
	{
		tmr800ms++;
		tmr2s++;
		
		if(tmr800ms >= 800)
		{
			tmr800ms = 0;
			flag800ms = 1;
		}
		if(tmr2s >= 2000)
		{
			tmr2s = 0;
			flag2s = 1;
		}
	}
	
	if(LedSta)//����Ҳ������ǰ���жϣ��и��ô����ٴ򿪲���ͻأ����!(ע�����)
	{
		tmrLedCnt++;
		
		if(tmrLedCnt >= LedCnt)
		{
			tmrLedCnt = 0;
			flagLedCnt = 1;
		}
	}
	
	KeyScan();
	LedScan();
}

void InterruptTimer1() interrupt 3//��ʱ��1����˵ֻ������ʵ��PWM����LED�ˣ�
{	
    static u8 PWMCnt = 0;

    TH1 = 0xFF;//�����ˣ�TH1д����TH0������
    TL1 = 0xF7;
    PWMCnt++;
    PWMCnt %= 101;//ȷ��ȡֵ��0-100֮��
	
	if(PWMCnt >= LedLevelDat[LedLevel-1])
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = 0xFF;
		P2 = P2&0x1F;
	}
	else
	{
		P2 = (P2&0x1F) | 0x80;
		P0 = PWMSta;
		P2 = P2&0x1F;
	}
}
