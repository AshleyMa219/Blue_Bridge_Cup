#ifndef MAIN_H
#define MAIN_H

enum eSysSta {//��Ӧ����ϵͳ״̬������������洢��ʾ����������
	E_RUN, E_VIEW, E_SET
}; 

extern enum eSysSta SysSta;
extern u16 distance, LastDistance, FixDistance;
extern bit OpenLed1;
extern u8 Led1Cnt;

void ResetE2();

#endif