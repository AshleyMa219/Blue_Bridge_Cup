/*
*******************************************************************************
* �ļ�����ds1302.h
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 
* ��  ע��
*         
*******************************************************************************
*/

#ifndef DS1302_H
#define DS1302_H

struct sTime{
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec;
	u8 week;
};

void InitDS1302();
void GetRealTime(struct sTime* time);
void SetRealTime(struct sTime* time);
void DS1302BurstRead(u8 *dat);
void DS1302BurstWrite(u8* dat);


#endif