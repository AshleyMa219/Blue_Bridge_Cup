/*
*******************************************************************************
* �ļ�����lcd1602.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#include "config.h"

void LcdWaitReady()
{
	u8 sta;

	LCD1602_DB = 0xFF;
	LCD1602_RS = 0;
	LCD1602_RW = 1;
	do{
		LCD1602_E = 1;
		sta = LCD1602_DB;
		LCD1602_E = 0;
	}while(sta&0x80);	
}

void LcdWriteCmd(u8 cmd)
{
	LcdWaitReady();
	LCD1602_RS = 0;
	LCD1602_RW = 0;
	LCD1602_DB = cmd;
	LCD1602_E = 1;
	LCD1602_E = 0;
}

void LcdWriteDat(u8 dat)
{
	LcdWaitReady();
 	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_DB = dat;
	LCD1602_E = 1;
	LCD1602_E = 0;
}

void LcdSetCursor(u8 x, u8 y)
{
 	u8 addr;

	if(y==0)
	{
	 	addr = 0x00 + x;
	}
	else
	{
		addr = 0x40 + x;
	}
	LcdWriteCmd(addr | 0x80);
}

void LcdShowStr(u8 x, u8 y, u8 *str)
{
	LcdSetCursor(x, y);

	while(*str != '\0')
	{
	 	LcdWriteDat(*str++);
	}
}

void LcdFullClear(u8 x, u8 y, u8 len)
{
	LcdSetCursor(x, y);
	while(len--)
	{
	 	LcdWriteDat(' ');
	}
}

void LcdAreaClear()
{
	LcdWriteCmd(0x01);
}

void InitLcd1602()
{
 	LcdWriteCmd(0x38);
	LcdWriteCmd(0x0C);
	LcdWriteCmd(0x06);
	LcdWriteCmd(0x01);
}