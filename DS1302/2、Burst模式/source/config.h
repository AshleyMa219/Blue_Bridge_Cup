/*
*******************************************************************************
* �ļ�����config.h
* ��  ����һЩ�����ļ�
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <stc15.h>
#include <intrins.h>

typedef unsigned char u8;
typedef unsigned int  u16;
typedef unsigned long u32;

#define LCD1602_DB P0
sbit LCD1602_RS = P2^0;
sbit LCD1602_RW = P2^1;
sbit LCD1602_E  = P1^2;

sbit I2C_SCL = P2^0;
sbit I2C_SDA = P2^1;

sbit DS1302_CE = P1^3;
sbit DS1302_CK = P1^7;
sbit DS1302_IO = P2^3;


#endif