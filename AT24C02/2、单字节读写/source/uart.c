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
#include "uart.h"
#include "main.h"

bit flagFrame = 0;
bit flagTxd = 0;
u8 cntRxd = 0;
u8 bufRxd[64];

void ConfigUart(u32 baud)
{
 	PCON &= 0x7F;
	SCON = 0x50;
	AUXR &= 0xBF;
	AUXR &= 0xFE;
	TMOD &= 0x0F;
	TMOD |= 0x20;
	TH1 = 256 - (11059200/12/32)/baud;
	TL1 = TH1;
	ET1 = 0;
	ES = 1;
	TR1 = 1;
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

u8 UartRead(u8 *buf, u8 len)
{
	u8 i;

 	if(len > cntRxd)//��ָ�����ȴ��ڽ��ճ���ʱ������ָ������
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

void UartRxdMonitor(u8 ms)//��������ŵ���ʱ��0���ж���ɨ��
{
 	static u8 cntbkp = 0; //��һ��ʱ�����ڽ��յ������ݸ���
	static u8 idletmr = 0;//����ʱ���ۼ�����

	if(cntRxd > 0)
	{
	 	if(cntbkp != cntRxd)
		{
			cntbkp = cntRxd;
			idletmr = 0;
		}
	 	else//��������һ�����ҿ���ʱ��С��30
		{
			if(idletmr < 30)
			{
			 	idletmr += ms;
				if(idletmr >= 30)
				{
				 	flagFrame = 1;
				}
			}
		}
	}
	else
	{
	 	cntbkp = 0;
	}
}

void UartDriver()//��������while(1)����
{
 	u8 len;
	u8 buf[40];

	if(flagFrame)//һ֡���ݽ��������������
	{
	 	flagFrame = 0;
		len = UartRead(buf, sizeof(buf));
		UartAction(buf, len);
	}
}

void InterruptUart() interrupt 4
{
 	if(RI)
	{
	 	RI = 0;
		if(cntRxd < sizeof(bufRxd))//ÿ�յ�һ���ֽڣ�����һ�δ����ж�!
		{
		 	bufRxd[cntRxd++] = SBUF;//��ʱֻҪ������û�������ͰѶ�Ӧ�����ݴ��뻺������
		}
	}
	if(TI)
	{
	 	TI = 0;
		flagTxd = 1;    
	}
}