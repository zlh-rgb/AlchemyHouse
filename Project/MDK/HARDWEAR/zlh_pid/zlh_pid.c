#include "zlh_pid.h"

int16 dianjiL_error;
int16 last_dianjiL_error;
int16 per_dianjiL_error;

int16 dianjiR_error;
int16 last_dianjiR_error;
int16 per_dianjiR_error;

float PID_dianji[3] = {19,20,0};//速度PID

int16 PID_Realize(int16 ActualSpeed, int16 SetSpeed);

void pidInit(PID *myPID,float kp,float ki,float kd){
	myPID->kD=kd;
	myPID->KP=kp;
	myPID->KI=ki;
	myPID->last_position=0;
	myPID->temp_i=0;
	myPID->temp_pid=0;
}
float pidOut(float position, PID *myPID)
{
	if(position == 0)
	{
		myPID->temp_i = 0;
		myPID->last_position = 0;
		return 0;
	}
	else
	{
		myPID->temp_i += position;
		if (myPID->temp_i>=30000)
		{
			myPID->temp_i = 30000.0f;
		}
		else if(myPID->temp_i<=-30000)
		{
			myPID->temp_i = -30000.0f;
		}
		myPID->temp_pid = myPID->KP * position + myPID->KI * myPID->temp_i + myPID->kD * (position-myPID->last_position);
		myPID->last_position = position;

		return myPID->temp_pid;
	}
}

float VpidOut(float position, PID *myPID)
{
	if(position == 0)
	{
		myPID->temp_i = 0;
		myPID->last_position = 0;
		return 0;
	}
	else
	{
		myPID->temp_i += position;
		if (myPID->temp_i>=300000)
		{
			myPID->temp_i = 300000.0f;
		}
		else if(myPID->temp_i<=-300000)
		{
			myPID->temp_i = -300000.0f;
		}
		myPID->temp_pid = myPID->KP * position + myPID->KI * myPID->temp_i + myPID->kD * (position-myPID->last_position);
		myPID->last_position = position;

		return myPID->temp_pid;
	}
}
int16 PID_Realize(int16 ActualSpeed, int16 SetSpeed)
{	
  	int16 Increase;	//最后得出的实际增量
    int16 dianji_error;
		dianji_error = SetSpeed - ActualSpeed;//计算当前误差
		Increase = 11*dianji_error;
    if(Increase>140) Increase = 140; //限幅
    if(Increase<-140) Increase = -140;
    return Increase;
}


int16 PID_RealizeL(int16 ActualSpeed, int16 SetSpeed)
{	
  int16 Increase;	//最后得出的实际增量

  dianjiL_error = SetSpeed - ActualSpeed;
  Increase = (int16)(PID_dianji[0]*(dianjiL_error-last_dianjiL_error)+PID_dianji[1]*dianjiL_error
           +PID_dianji[2]*(dianjiL_error-2*last_dianjiL_error+per_dianjiL_error));
  per_dianjiL_error = last_dianjiL_error;
  last_dianjiL_error = dianjiL_error;
  if(Increase>220) Increase = 220; //限幅
  if(Increase<-220) Increase = -220;
  return Increase;
}


int16 PID_RealizeR(int16 ActualSpeed, int16 SetSpeed)
{	
  int16 Increase;

  dianjiR_error = SetSpeed - ActualSpeed;
  Increase = (int16)(PID_dianji[0]*(dianjiR_error-last_dianjiR_error)+PID_dianji[1]*dianjiR_error
           +PID_dianji[2]*(dianjiR_error-2*last_dianjiR_error+per_dianjiR_error));
  per_dianjiR_error = last_dianjiR_error;
  last_dianjiR_error = dianjiR_error;
  if(Increase>220) Increase = 220; //限幅
  if(Increase<-220) Increase = -220;
  return Increase;
}