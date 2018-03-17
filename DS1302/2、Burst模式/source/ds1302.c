/*
*******************************************************************************
* �ļ�����ds1302.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#include "config.h"
#include <intrins.h>

void DS1302ByteWrite(u8 dat) 
{
	u8 mask;

	for(mask=0x01; mask!=0; mask<<=1)
	{
	 	if((dat&mask) != 0)
			DS1302_IO = 1;
		else
			DS1302_IO = 0;
		DS1302_CK = 1;
		DS1302_CK = 0;
	}
	DS1302_IO = 1;	//д��֮��ȷ���ͷ�IO����
}  

u8 DS1302ByteRead()
{
	u8 mask, dat=0;

	for(mask=0x01; mask!=0; mask<<=1)
	{
	 	if(DS1302_IO)
		{
		 	dat |= mask;
		}
		DS1302_CK = 1;
		DS1302_CK = 0;
	}
	return dat;
} 

void DS1302SingleWrite(u8 reg,u8 dat)     
{
	DS1302_CE = 1;
	DS1302ByteWrite((reg<<1) | 0x80);
	DS1302ByteWrite(dat);
	DS1302_CE = 0;
}

u8 DS1302SingleRead(u8 reg)
{
	u8 dat;

	DS1302_CE = 1;
	DS1302ByteWrite((reg<<1) | 0x81);
	dat = DS1302ByteRead();
	DS1302_CE = 0;
	DS1302_IO = 0;//���ֽڶ�д����ӵģ�
	
	return dat;			
}

void DS1302BurstWrite(u8 *dat)
{
	u8 i;

	DS1302_CE = 1;
	DS1302ByteWrite(0xBE);
	for(i=0; i<7; i++)
	{
		DS1302ByteWrite(*dat++);		
	}
	DS1302_CE = 0;		
}

void DS1302BurstRead (u8 *dat)
{
	u8 i;

	DS1302_CE = 1;
	DS1302ByteWrite(0xBF);
	for(i=0; i<7; i++)
	{
		dat[i] = DS1302ByteRead();		
	}
	DS1302_CE = 0;		
}

void InitDS1302()
{
    u8 InitTime[] = {  //2018��3��17�� ������ 11:22:00
        0x00,0x22,0x11, 0x17, 0x03, 0x06, 0x18
    };
	DS1302_CE = 0;//��ʼ��ͨ������
	DS1302_CK = 0;
    DS1302SingleWrite(7, 0x00);  //����д����������д������
	DS1302BurstWrite(InitTime);
}