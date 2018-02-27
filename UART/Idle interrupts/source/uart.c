/*
*******************************************************************************
* �ļ�����uart.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#include "config.h"
#include "main.h"

bit flagTxd = 0;
bit flagFrame = 0;
u8 cntRxd = 0;
u8 bufRxd[64];

void ConfigUart(u32 baud)
{
 	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xBF;		//��ʱ��1ʱ��ΪFosc/12,��12T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TH1 = 256 - (11059200/12/32)/baud;
	TL1 = TH1;
	ET1 = 0;
	ES = 1;
	TR1 = 1;	
}

u8 UartRead(u8 *buf, u8 len)
{
	u8 i;

	if(len > cntRxd)
	{
	 	len = cntRxd;
	}	
	for(i=0; i<len; i++)
	{
	 	*buf++ = bufRxd[i];
	}
	cntRxd = 0;

	return len;
}

void UartWrite(u8 *buf, u8 len)
{
	while(len--)
	{
	 	flagTxd = 0;
		SBUF = *buf++;
		while(!flagTxd);
	} 	
}

void UartDriver()
{
	u8 buff[40];
	u8 len;

	if(flagFrame)
	{
	 	flagFrame = 0;
		len = UartRead(buff, sizeof(buff));
		UartAction(buff, len);
	}
}

void UartRxdMonitor(u8 ms)
{
 	static u8 cntbkp = 0;
	static u8 idletmr = 0;

	if(cntRxd > 0)
	{
	 	if(cntRxd != cntbkp)
		{
		 	cntbkp = cntRxd;
			idletmr = 0;
		}
		else
		{
		 	if(idletmr < 30)
			{
			 	idletmr += ms;
			}
			if(idletmr >= 30)
			{
				idletmr = 0;
			 	flagFrame = 1;
			}
		}
	}
	else
	{
	 	cntbkp = 0;
	}
}

void InterruptUart() interrupt 4
{
 	if(RI)
	{
	 	RI = 0;
		if(cntRxd < sizeof(bufRxd))
		{
		 	bufRxd[cntRxd++] = SBUF;
		}
	}
	if(TI)
	{
	 	TI = 0;
		flagTxd = 1;
	}
}