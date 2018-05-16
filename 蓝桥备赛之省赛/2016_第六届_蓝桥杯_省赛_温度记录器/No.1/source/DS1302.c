#define  _DS1302_C
#include "config.h"
#include "DS1302.h"


void DS1302ByteWrite(uint8 dat)
{
    uint8 mask;
    
    for (mask=0x01; mask!=0; mask<<=1)  
    {
        if ((mask&dat) != 0) 
            DS1302_IO = 1;
        else
            DS1302_IO = 0;
        DS1302_CK = 1;      
        DS1302_CK = 0;       
    }
    DS1302_IO = 1;           
}

uint8 DS1302ByteRead()
{
    uint8 mask;
    uint8 dat = 0;
    
    for (mask=0x01; mask!=0; mask<<=1)  
    {
        if (DS1302_IO != 0)  
        {
            dat |= mask;
        }
        DS1302_CK = 1;       
        DS1302_CK = 0;       
    }
    return dat;              
}

void DS1302SingleWrite(uint8 reg, uint8 dat)
{
    DS1302_CE = 1;                   
    DS1302ByteWrite((reg<<1)|0x80);  
    DS1302ByteWrite(dat);            
    DS1302_CE = 0;                   
}

uint8 DS1302SingleRead(uint8 reg)
{
    uint8 dat;
    
    DS1302_CE = 1;                   
    DS1302ByteWrite((reg<<1)|0x81);  
    dat = DS1302ByteRead();         
    DS1302_CE = 0;                  
    
    return dat;
}

void DS1302BurstWrite(uint8 *dat)
{
    uint8 i;
    
    DS1302_CE = 1;
    DS1302ByteWrite(0xBE); 
    for (i=0; i<8; i++)    
    {
        DS1302ByteWrite(dat[i]);
    }
    DS1302_CE = 0;
}

void DS1302BurstRead(uint8 *dat)
{
    uint8 i;
    
    DS1302_CE = 1;
    DS1302ByteWrite(0xBF);  
    for (i=0; i<8; i++)     
    {
        dat[i] = DS1302ByteRead();
    }
    DS1302_CE = 0;
}

void GetRealTime(struct sTime *time)
{
    uint8 buf[8];
    
    DS1302BurstRead(buf);
    time->year = buf[6] + 0x2000;
    time->mon  = buf[4];
    time->day  = buf[3];
    time->hour = buf[2];
    time->min  = buf[1];
    time->sec  = buf[0];
    time->week = buf[5];
}

void SetRealTime(struct sTime *time)
{
    uint8 buf[8];
    
    buf[7] = 0;
    buf[6] = time->year;
    buf[5] = time->week;
    buf[4] = time->mon;
    buf[3] = time->day;
    buf[2] = time->hour;
    buf[1] = time->min;
    buf[0] = time->sec;
    DS1302BurstWrite(buf);
}

void InitDS1302()
{
    uint8 dat;
    struct sTime code InitTime[] = {  //Ĭ�ϳ�ʼֵ�� 23:59:50
        0x2014,0x01,0x01, 0x23,0x59,0x50, 0x03
    };
    
    DS1302_CE = 0;  
    DS1302_CK = 0;
    dat = DS1302SingleRead(0); 
    if ((dat & 0x80) != 0)      
    {
        DS1302SingleWrite(7, 0x00);  
        SetRealTime(&InitTime);    
    }
}
