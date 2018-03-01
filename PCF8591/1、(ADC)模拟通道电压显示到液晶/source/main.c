/*
*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��AIN0��Ӧ�ұ����룬AIN1��Ӧ��������,AIN3��Ӧ���䣡
*         
*******************************************************************************
*/

#include "config.h"
#include "lcd1602.h"
#include "i2c.h"

u8 T0RH = 0, T0RL = 0;
bit flag300ms = 0;

void ConfigTimer0(u16 ms);
void CloseFucker();
u8 GetADCValue(u8 ch);
void ValueToString(u8 *str, u8 val);

void main()
{
	u8 val;
	u8 str[12];

	EA = 1;
	CloseFucker();
	ConfigTimer0(10);//��ʱ10ms
	InitLcd1602();
	LcdShowStr(0, 0, "AIN0  AIN1  AIN3");

	while(1)
	{
		if(flag300ms)
		{
			flag300ms = 0;

			val = GetADCValue(0);
			ValueToString(str, val);
			LcdShowStr(0, 1, str);

			val = GetADCValue(1);
			ValueToString(str, val);
			LcdShowStr(6, 1, str);

			val = GetADCValue(3);
			ValueToString(str, val);
			LcdShowStr(12, 1, str);
		}
	}
}

u8 GetADCValue(u8 ch)
{
	u8 val;

	I2CStart();
	if(I2CWrite(0x48<<1) == 0) //Ѱ������ַ - д
	{
	 	I2CStop();
		return 0;  //���ﲢ����I2C����ʹ��break�������Ƕ��������������͵÷���0.
	}
	I2CWrite(0x40 | ch);//��Ӧ����ģʽ - ͨ����
	I2CStart();
	I2CWrite((0x48<<1) | 0x01);//Ѱ������ַ - ��
	I2CReadACK();
	val = I2CReadNAK();//��ε�ֵ��������8��SCL����������ȿն�Ȼ����ȥNAK����
	I2CStop();

	return val;	
}

void ValueToString(u8 *str, u8 val)//!!ע������Ĵ����ɣ��ѵ�ѹ������10��������
{
 	val = (val*50) / 255;  //��ѹ5V��256���̶ȷֳ�255�ݣ�
	str[0] = (val/10) + '0';
	str[1] = '.';
	str[2] = (val%10) + '0';
	str[3] = 'V';
	str[4] = '\0';
}


void CloseFucker()
{
 	P2 = (P2 & 0x1F) | 0xA0;
	P0 = P0 & 0xAF;
	P2 = P2&0x1F;
}


void ConfigTimer0(u16 ms)
{
    u32 tmp; 
    
    tmp = 11059200 / 12;    
    tmp = (tmp * ms) / 1000;  
    tmp = 65536 - tmp;          
    T0RH = (u8)(tmp>>8);  
    T0RL = (u8)tmp;
    TMOD &= 0xF0;   
    TMOD |= 0x01;   
    TH0 = T0RH;     
    TL0 = T0RL;
    ET0 = 1;       
    TR0 = 1;       
}

void InterruptTimer0() interrupt 1
{ 
	static u8 tmr300ms = 0;
  
    TH0 = T0RH; 
    TL0 = T0RL;
	tmr300ms++;
	if(tmr300ms >= 30)
	{
	 	tmr300ms = 0;
		flag300ms = 1;
	}

}
