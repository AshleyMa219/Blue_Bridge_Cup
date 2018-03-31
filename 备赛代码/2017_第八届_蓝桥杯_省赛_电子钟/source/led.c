/*
*******************************************************************************
* �ļ�����led.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע���������غ���
*         
*******************************************************************************
*/

#include "config.h"

u8 setCursorIndex = 0;
bit flagCursor = 0;

u8 code LedChar[] = {
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
	0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E
};


u8 LedBuff[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void LedScan()
{
 	static u8 index = 0;

	P2 = (P2&0x1F) | 0xE0;
	P0 = 0xFF;
	P2 = P2&0x1F;
	P2 = (P2&0x1F) | 0xC0;
	P0 = 0x80>>index;
	P2 = P2&0x1F;
	P2 = (P2&0x1F) | 0xE0;
	P0 = LedBuff[index];
	P2 = P2&0x1F;

	index++;
	index &= 0x07;
}	 

void LedOpenCursor()
{
	flagCursor = 1;
}

void LedCloseCursor()
{
	flagCursor = 0;
}

void LedSetCursor(u8 index)
{
	if(index == 1)//ʱ
	{
		setCursorIndex = 7;	 		
	}
	else if(index == 2)//��
	{
		setCursorIndex = 4;
	}
	else if(index == 3)//��
	{
		setCursorIndex = 1;
	} 		
}

