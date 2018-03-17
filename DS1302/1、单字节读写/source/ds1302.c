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
#include "ds1302.h"

void DS1302ByteWrite(u8 dat) 
{
	u8 mask;
    
    for (mask=0x01; mask!=0; mask<<=1)  //��λ��ǰ����λ�Ƴ�
    {
        if ((mask&dat) != 0) //���������λ����
            DS1302_IO = 1;
        else
            DS1302_IO = 0;
        DS1302_CK = 1;       //Ȼ������ʱ��
        DS1302_CK = 0;       //������ʱ�ӣ����һ��λ�Ĳ���
    }
    DS1302_IO = 1;           //���ȷ���ͷ�IO����
}  

u8 DS1302ByteRead()
{
	u8 mask;
    u8 dat = 0;
    
    for (mask=0x01; mask!=0; mask<<=1)  //��λ��ǰ����λ��ȡ
    {
        if (DS1302_IO != 0)  //���ȶ�ȡ��ʱ��IO���ţ�������dat�еĶ�Ӧλ
        {
            dat |= mask;
        }
        DS1302_CK = 1;       //Ȼ������ʱ��
        DS1302_CK = 0;       //������ʱ�ӣ����һ��λ�Ĳ���
    }
    return dat;              //��󷵻ض������ֽ�����
} 

void DS1302SingleWrite(u8 reg,u8 dat )     
{
 	DS1302_CE = 1;	 
 	DS1302ByteWrite((reg<<1)|0x80);	
 	DS1302ByteWrite(dat);		
 	DS1302_CE = 0; 
}

u8 DS1302SingleRead (u8 reg)
{
 	u8 dat=0;

 	DS1302_CE = 1;
 	DS1302ByteWrite((reg<<1)|0x81);
	dat = DS1302ByteRead();
 	DS1302_CE = 0;
	DS1302_IO = 0;//���ֽڶ���֮��������һ�䣡��ѧ������

	return dat;			
}

void InitDS1302()
{
	u8 i;
    u8 code InitTime[] = {  //2018��3��17�� ������ 11:12:00
        0x00, 0x12, 0x11, 0x17, 0x03, 0x06, 0x18
    };

    DS1302_CE = 0;  //��ʼ��DS1302ͨ������
    DS1302_CK = 0;
    DS1302SingleWrite(7, 0x00);  //����д����������д������
	for (i=0; i<7; i++)          //����DS1302ΪĬ�ϵĳ�ʼʱ��
    {
        DS1302SingleWrite(i, InitTime[i]);
    }
}