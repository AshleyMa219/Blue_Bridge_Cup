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
#define I2CDelay() Delay5us()

void Delay5us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 12;
	while (--i);
}

void I2CStart()
{
 	I2C_SCL = 1;
	I2C_SDA = 1;
	I2CDelay();
	I2C_SDA = 0;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();
}
void I2CStop()
{
 	I2C_SCL = 0;
	I2C_SCL = 0;
	I2CDelay();
	I2C_SCL = 1;
	I2CDelay();
	I2C_SDA = 1;
	I2CDelay();
}
bit I2CWrite(u8 dat)
{
	bit ack;
	u8 mask;

	I2C_SCL = 0;
	I2CDelay();
	for(mask=0x80; mask!=0; mask>>=1)
	{	
		if(dat&mask)
			I2C_SDA = 1;
		else
			I2C_SDA = 0;
		I2CDelay();
		I2C_SCL = 1;
		I2CDelay();
		I2C_SCL = 0;
		I2CDelay();	
	}
	I2C_SDA = 1;
	I2CDelay();
	I2C_SCL = 1;
	ack = I2C_SDA;
	I2CDelay();
	I2C_SCL = 0;
	I2CDelay();

	return ~ack;

}

u8 I2CReadACK()
{
	u8 dat=0;
	u8 mask;

	I2C_SDA = 1;
	I2CDelay();
	for(mask=0x80; mask!=0; mask>>=1)
	{
		I2C_SCL = 1;
		I2CDelay();
		if(I2C_SDA)
			dat |= mask;
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
 	u8 dat=0;
	u8 mask;

	I2C_SDA = 1;
	I2CDelay();
	for(mask=0x80; mask!=0; mask>>=1)
	{
		I2C_SCL = 1;
		I2CDelay();
		if(I2C_SDA)
			dat |= mask;
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