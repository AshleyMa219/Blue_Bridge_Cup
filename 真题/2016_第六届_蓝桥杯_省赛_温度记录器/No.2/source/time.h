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

extern u8 SetSec;
extern bit flagLed;
extern bit flagtempOver;

void RefreshTime(bit sta);
void KeyAction(u8 keycode);
void RefreshTemp();
void RefreshSetSec();
void EnterTemp(bit sta);
void CollectTemp();

#endif