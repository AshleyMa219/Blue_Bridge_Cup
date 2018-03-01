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

void DS1302ByteWrite(u8 temp) 
{
	u8 i;
	for (i=0;i<8;i++)     	
	{ 
		DS1302_CK=0;
		DS1302_IO=temp&0x01;
		temp>>=1; 
		DS1302_CK=1;
	}
}  

u8 DS1302ByteRead()
{
 	u8 i, temp=0x00;

	for (i=0;i<8;i++) 	
 	{		
		DS1302_CK=0;
		temp>>=1;	
 		if(DS1302_IO)
 		temp|=0x80;	
 		DS1302_CK=1;
	} 

	return temp;
} 

void DS1302SingleWrite(u8 address,u8 dat )     
{
 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;	
   	_nop_();  
 	DS1302ByteWrite((address<<1)|0x80);	
 	DS1302ByteWrite(dat);		
 	DS1302_CE=0; 
}

u8 DS1302SingleRead (u8 address )
{
 	u8 temp=0x00;

 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;
	_nop_();
 	DS1302ByteWrite((address<<1)|0x81);
	temp = DS1302ByteRead();
 	DS1302_CE=0;
	_nop_();
 	DS1302_CE=0;
	DS1302_CK=0;
	_nop_();
	DS1302_CK=1;
	_nop_();
	DS1302_IO=0;
	_nop_();
	DS1302_IO=1;
	_nop_();

	return temp;			
}

void DS1302BurstWrite(u8 *dat)
{
	u8 i;

 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;	
   	_nop_();  
 	DS1302ByteWrite(0xBE);
	for(i=0; i<8; i++)
	{	
 		DS1302ByteWrite(*dat++);					   
	}
 	DS1302_CE=0; 	
}

void DS1302BurstRead (u8 *dat)
{
  	u8 i;

 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;
	_nop_();
 	DS1302ByteWrite(0xBF);
	for(i=0; i<8; i++)
	{
		dat[i] = DS1302ByteRead();
	}
 	DS1302_CE=0;
	_nop_();
 	DS1302_CE=0;
	DS1302_CK=0;
	_nop_();
	DS1302_CK=1;
	_nop_();
	DS1302_IO=0;
	_nop_();
	DS1302_IO=1;
	_nop_();		
}

void InitDS1302()
{
    u8 InitTime[] = {  //2018��3��1�� ������ 9:44:00
        0x00,0x44,0x09, 0x01, 0x03, 0x04, 0x18
    };
    DS1302SingleWrite(7, 0x00);  //����д����������д������
	DS1302BurstWrite(InitTime);
}