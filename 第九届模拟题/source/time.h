/*
*******************************************************************************
* �ļ�����time.h
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#ifndef TIME_H
#define TIME_H

extern bit flagAlarm;
extern bit flagCursorCnt;

void KeyAction(u8 keycode);
void RefreshTime(bit ak);
void RefreshAlarm();
void AlarmMonitor();
void Refresh18B20();


#endif