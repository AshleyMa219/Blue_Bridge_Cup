/*
*******************************************************************************
* �ļ�����uart.h
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#ifndef UART_H
#define UART_H

void ConfigUart(u32 baud);
void UartDriver();
void UartRxdMonitor(u8 ms);
void UartWrite(u8 *buf, u8 len); 

#endif