/*
*******************************************************************************
* �ļ�����infrared.h
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#ifndef INFRARED_H
#define INFRARED_H

void InitInfrared();
void IRScan();

extern bit irflag;
extern u8 ircode[4];

#endif