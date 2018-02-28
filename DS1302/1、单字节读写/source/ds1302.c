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

void DS1302ByteWrite(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		DS1302_CK=0;
		DS1302_IO=temp&0x01;
		temp>>=1; 
		DS1302_CK=1;
	}
}   

void DS1302SingleWrite( unsigned char address,unsigned char dat )     
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

unsigned char DS1302SingleRead ( unsigned char address )
{
 	unsigned char i,temp=0x00;
 	DS1302_CE=0;
	_nop_();
 	DS1302_CK=0;
	_nop_();
 	DS1302_CE=1;
	_nop_();
 	DS1302ByteWrite((address<<1)|0x81);
 	for (i=0;i<8;i++) 	
 	{		
		DS1302_CK=0;
		temp>>=1;	
 		if(DS1302_IO)
 		temp|=0x80;	
 		DS1302_CK=1;
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
	return (temp);			
}

void InitDS1302()
{
	u8 i;
    u8 InitTime[] = {  //2018��2��28�� ���ڶ� 22:28:00
        0x00,0x28,0x22, 0x28, 0x02, 0x03, 0x18
    };
    DS1302SingleWrite(7, 0x00);  //����д����������д������
    for (i=0; i<7; i++)          //����DS1302ΪĬ�ϵĳ�ʼʱ��
    {
        DS1302SingleWrite(i, InitTime[i]);
    }
}