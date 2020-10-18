#include "zlh_dgControl.h"

void dgInit() { pwm_init(PWM4_MODULE2_CHA_C30, 50, 5000); }

void dgControl(float Offset, int dgMedium)
{
	int dgDuty = 0;
	dgDuty = dgMedium + (int)Offset;
	pwm_duty(PWM4_MODULE2_CHA_C30, dgDuty);
}