/*
*******************************************************************************
* �ļ�����led.h
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#ifndef LED_H
#define LED_H

extern u8 code LedChar[];
extern u8 LedBuff[];
extern bit flagCursor;
extern u8 setCursorIndex;

void ShowNumber(u32 dat);
void LedScan();
void LedOpenCursor();
void LedCloseCursor();
void LedSetCursor(u8 index);

#endif