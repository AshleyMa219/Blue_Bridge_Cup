#ifndef TIME_H
#define TIME_H

extern bit SetLimitSta;//������ֵ����״̬����1

void LimitMonitorAUTO();
void LimitMonitorHAND();
void RefreshTime(bit sta);
void KeyAction(u8 keycode);
void RefreshAdjust();

#endif