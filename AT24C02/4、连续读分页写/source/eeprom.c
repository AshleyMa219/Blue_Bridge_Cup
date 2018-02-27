/*
*******************************************************************************
* �ļ�����eeprom.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#include "config.h"
#include "i2c.h"
#include "eeprom.h"

void E2Read(u8 *buf, u8 addr, u8 len)
{
	do{
		I2CStart();
		if(I2CWrite(0x50<<1))
		{
		 	break;
		}
		I2CStop();
	}while(1);
	I2CWrite(addr);
	I2CStart();
	I2CWrite((0x50<<1)|0x01);
	while(len > 1)
	{
	 	*buf++ = I2CReadACK();
		len--;
	}
	*buf = I2CReadNAK();
	I2CStop();
}

void E2Write(u8 *buf, u8 addr, u8 len)
{
 	while(len > 0)
	{
	 	do{
			I2CStart();
			if(I2CWrite(0x50<<1))
			{
			 	break;
			}
			I2CStop();
		}while(1);
		I2CWrite(addr);
		while(len > 0)
		{
			I2CWrite(*buf++);
			len--;
			addr++;
			if((addr&0x07) == 0)
			{
			 	break;
			}
		}
		I2CStop();
	}
}