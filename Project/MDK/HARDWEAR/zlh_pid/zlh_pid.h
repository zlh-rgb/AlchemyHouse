#ifndef ZLH_PID_H

#include "SEEKFREE_OLED.h"
#include "zf_uart.h"

typedef struct{
    float KP;
    float kD;
    float KI;
    float temp_pid;
    float temp_i;
	  float last_position;
}PID;

float pidOut(float position, PID *myPID);
float VpidOut(float position, PID *myPID);
void pidInit(PID *myPID,float kp,float ki,float kd);
int16 PID_Realize(int16 ActualSpeed, int16 SetSpeed);
int16 PID_RealizeL(int16 ActualSpeed, int16 SetSpeed);
int16 PID_RealizeR(int16 ActualSpeed, int16 SetSpeed);
#endif