#ifndef MAIN_H
#define MAIN_H

enum eSysSta {//�ֱ��Ӧ ʱ�ӽ��桢��ѹ���桢Ƶ�ʽ��桢��ѯ����
	E_CLK, E_VOL, E_FRE, E_QRY
};

extern enum eSysSta SysSta;
extern bit flagLedOpen;
extern bit flagVolOpen;
extern u32 FreNum;
extern u32 TimNum;

extern u8 TrigType;
extern u8 TrigHour, TrigMin, TrigSec;

#endif