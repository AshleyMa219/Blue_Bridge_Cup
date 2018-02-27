/*
*******************************************************************************
* �ļ�����i2c.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#include "config.h"
#include "intrins.h"
#include "uart.h"

#define I2CDelay() Delay5us()

void Delay5us()		//@11.0592MHz �����ʱ��STC������ɵ�
{
	unsigned char i;

	_nop_();
	i = 11;
	while (--i);
}
 

void I2CStart()//��ʼ�ź�SCL/SDA�ߵ�ƽ�ڼ䣬������SDA��������SCL��
{
 	I2C_SCL = 1;
	I2C_SDA = 1;
	I2CDelay();
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();
}

void I2CStop()//ֹͣ�ź�SCL/SDA�͵�ƽ�ڼ䣬������SCL��������SDA��
{
	I2C_SCL = 0;
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SDA = 1;
	I2CDelay();
}

bit I2CWrite(u8 dat)//д����
{
	bit ack;
 	u8 mask;//������������д����ص�get����

	for(mask=0x80; mask!=0; mask>>=1)//I2C��λ���ȣ���mask��ֵ0x80
	{
	 	if((mask&dat) == 0)
			I2C_SDA = 0;
		else
			I2C_SDA = 1;
		I2CDelay();
		I2C_SCL = 1;
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();
	}					 //�ѷ�����(��Ƭ��)��һ���ֽ�����λд����ϣ��������ý�����(24C02)��Ӧһ��ACK
	I2C_SDA = 1;		 //�����ͷ�SDA(��Ȼ����һֱ���Ƿ������������ݿ���д�뵽������)���Ա�ӻ���Ӧ��Ӧ��ACK��
	I2CDelay();
	I2C_SCL = 1;		 //����SCL׼����SDA�϶�Ӧ��ACK
	ack = I2C_SDA;		 //��ȡACK
	I2CDelay();
	I2C_SCL = 0;		 //��ȡ��ϣ�����SCL,�Ա����������д��
	I2CDelay();

	return (~ack);
}

u8 I2CReadACK()
{
	u8 mask;
	u8 dat;

	I2C_SDA = 1;
	I2CDelay();
	for(mask=0x80; mask!=0; mask>>=1)
	{
		I2C_SCL = 1;
		I2CDelay();
		if(I2C_SDA == 1)
		{
		 	dat |= mask;
		}
		else
		{
		 	dat &= ~mask;
		}
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();
	}
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();

	return dat;
}

u8 I2CReadNAK()
{
 	u8 mask;
	u8 dat;

	I2C_SDA = 1;
	I2CDelay();
	for(mask=0x80; mask!=0; mask>>=1)
	{
	 	I2C_SCL	= 1;
		I2CDelay();
		if(I2C_SDA == 1)
			dat |= mask;
		else
			dat &= ~mask;
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();			
	}
	I2C_SDA = 1;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();

	return dat;
}

 	