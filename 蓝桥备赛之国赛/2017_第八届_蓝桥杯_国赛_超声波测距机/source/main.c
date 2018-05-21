/*******************************************************************************
* �ļ������ڰ˽����֮����������
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2018��5��20�� 2018��5��21��
* ��  ע��*��Ŀ�е㲻ȫ��'����һ��������0.2sΪ�����˸'����û��˵������ã������Ұ�1s�����˴���
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

enum eSysSta SysSta = E_RUN;//Ĭ��ϵͳ����״̬

u8 T0RH, T0RL;//T0��װ��ֵ
bit flag200ms = 0;//200ms��ʱ����־
bit OpenLed1 = 0;//LED1������־
bit flagLed1 = 0;//LED10.2��ʱ����־
u8 tmrLed1 = 0;//���Ƽ��0.2s������
u8 Led1Cnt = 0;//���Ƴ���1s������
u16 OprtNum = 1;//�����������Ĳ���������
u16 distance=10, LastDistance=0, FixDistance=0;//�����������ı��ξ��롢�ϴξ���

void CloseFucker();//�رշ�����
void ConfigTimer0(u16 ms);//���ö�ʱ��0
void InitSonic();//��ʼ�����������õ��Ƕ�ʱ��1
void SonicDriver();//����������ɨ��
void ResetE2();//��ʼ��E2�е�0x00~0x09���⼸���ַ������
void TurnLed1();//��תLED1��״̬

void main()
{
	u8 val = 0;//����DAC���
	
 	EA = 1;//�������ж�
	CloseFucker();//�رշ��������޹�����
	ConfigTimer0(1);//���ö�ʱ��1ms
	InitSonic();//��ʼ��������(��ʱ��1)
	ResetE2();//��ʼ��0x00~0x09������Ϊ0
		
	while(1)
	{
		KeyDriver();//���󰴼���ɨ��
		
	 	if(flag200ms)//200ms��ʱ��
		{
		 	flag200ms = 0;//���200ms�Ķ�ʱ����־λ
			if(SysSta == E_RUN)//�жϵ�ǰϵͳ�Ƿ��ھ����������
			{
				SonicDriver();//������ɨ��
			}
			
			if(distance < FixDistance)//��ǰ����С���������룬�Ǿ���DAC���0
			{
				SetDACOut(0);
			}
			else//��ǰ��������������룬��ӦDAC��� (distance - FixDistance) * 0.02V
			{
				val = (u8)((distance-FixDistance)*0.2);
				SetDACOut(val);
			}
		}	
		if(flagLed1)//LED1 0.2s��ת�����
		{
			flagLed1 = 0;
			TurnLed1();//����LED1�ķ�ת
		}
	}
}

void TurnLed1()
{
	static bit Led1Sta = 0;//����LED��ת״̬�ľ�̬����
	
	if(Led1Sta == 0)//��ǰ������
	{
		Led1Sta = 1;//����״̬λ
		P2 = (P2&0x1F) | 0x80;//��LED1	
		P0 = 0xFE;
		P2 = P2&0x1F;
	}
	else//��ǰ������
	{
		Led1Sta = 0;//����״̬λ
		P2 = (P2&0x1F) | 0x80;//�ر�LED1
		P0 = 0xFF;
		P2 = P2&0x1F;
	}
	
	Led1Cnt++;//1s���������
	if(Led1Cnt >= 6)//ע������������1s
	{
		Led1Cnt = 0;//������Ƽ��ʱ��ļ����������Ա��´��ټ���
		OpenLed1 = 0;//�ر�LED1����
		tmrLed1 = 0;//���0.2ms�Ķ�ʱ������
		
		P2 = (P2&0x1F) | 0x80;//ȷ���ر�LED1
		P0 = 0xFF;
		P2 = P2&0x1F;
	}
	
}

void ResetE2()//��ʼ��E2�е����ݴ�0x00~0x09
{
	u8 i, dat=0;
	
	for(i=0; i<10; i++)
	{
		E2Write(&dat, i, 1);
	}
}

void CloseFucker()
{
 	P2 = (P2&0x1F) | 0xA0;
	P0 &= 0xAF;	
	P2 = P2&0x1F;
}

void ConfigTimer0(u16 ms)
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

void Delay13us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


void InitSonic()
{
	TMOD &= 0x0F;
	TMOD |= 0x10;
	TF1 = 0;
	TR1 = 0;
}

void SendWave()
{
	u8 i;
	
	while(i--)
	{
		Sonic_Txd = 1;
		Delay13us();
		Sonic_Txd = 0;
		Delay13us();
	}
}

void SonicDriver()
{
	u16 time;
	static u16 Result=0;//ΪʲôResult�ǵ��Ǿ�̬����ȫ�ֱ������ֲ������Ͳ����ˣ�
	
	TH1 = 0;
	TL1 = 0;
	SendWave();//��������8��40K�ķ���
	TR1 = 1;
	while((!TF1) && (Sonic_Rxd));//��TFδ�������RXD���ڸߵ�ƽ�ڼ���ü�����һֱ����
	TR1 = 0;
	
	LedBuff[7] = LedChar[0];//��Ӧ�������������̶ܹ���ʾ
	LedBuff[6] = 0xFF;
	
	if(TF1 == 1)
	{
		TF1 = 0;
		distance = LastDistance;//����˾����ϴεĲ���ֵ
	}
	else
	{
		time = (TH1 << 8) + TL1;//δ�������ʱ�任��ɾ��룬�������ٳ���2(ע�ⵥλ)
		distance = (u16)(time*0.017);
	}
	LedBuff[0] = LedChar[distance%10];//��Ӧ��Ԫ��ʾ��ǰ����
	LedBuff[1] = LedChar[distance/10%10];
	LedBuff[2] = LedChar[distance/100%10];
	
	if(OprtNum == 0)
	{
		Result = LastDistance;//��������0�����ʱ���м���ʾ�ϴεĲ�������
	}
	else
	{
		Result = LastDistance + distance;//��������1�����ʱ���м���ʾ�ϴεĲ�������+���β�������
	}
	LedBuff[3] = LedChar[Result%10];//��Ӧ��Ԫ��ʾ�Ķ�Ӧ����
	LedBuff[4] = LedChar[Result/10%10];
	LedBuff[5] = LedChar[Result/100%10];
	
	LastDistance = distance;//�����ϴεĲ���ֵ
}

void InterruptTimer0() interrupt 1
{
	static u8 tmr200ms = 0;	
	
 	TH0 = T0RH;
	TL0 = T0RL;
	tmr200ms++;
	
	if(OpenLed1)//�ж��Ƿ�LED1���ش�
	{
		tmrLed1++;
		if(tmrLed1 >= 200)//0.2s�ļ��
		{
			tmrLed1 = 0;
			flagLed1 = 1;
		}
	}

	if(tmr200ms >= 200)
	{
	 	tmr200ms = 0;
		flag200ms = 1;
	}
	
	KeyScan();//����ɨ��
	LedScan();//�����ˢ��
}