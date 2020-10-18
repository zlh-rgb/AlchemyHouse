#include "zlh_elma.h"

void elmaInit()
{
	pwm_init(PWM1_MODULE3_CHB_D1, 15000, 5000);
	pwm_init(PWM1_MODULE3_CHA_D0, 15000, 5000);
	pwm_init(PWM2_MODULE3_CHB_D3, 15000, 5000);
	pwm_init(PWM2_MODULE3_CHA_D2, 15000, 5000);
}

void carSpeed(int v_Lduty, int v_Rduty)
{
	// if (v_Lduty>=25000) {
	// 	v_Lduty = 25000;
	// }
	// if (v_Lduty<0)
	// {
	// 	v_Lduty = 0;
	// }
	// if (v_Rduty>=25000) {
	// 	v_Rduty = 25000;
	// }
	// if (v_Rduty<0)
	// {
	// 	v_Rduty = 0;
	// }
		if(v_Lduty>=0)
		{
		pwm_duty(PWM1_MODULE3_CHB_D1, 0);
		pwm_duty(PWM1_MODULE3_CHA_D0, v_Lduty);
		}
		else
		{
			pwm_duty(PWM1_MODULE3_CHB_D1, abs(v_Lduty));
			pwm_duty(PWM1_MODULE3_CHA_D0, 0);
		}
		if(v_Rduty>=0)
		{
			pwm_duty(PWM2_MODULE3_CHB_D3, 0);
			pwm_duty(PWM2_MODULE3_CHA_D2, v_Rduty);
		}
		else
		{
			pwm_duty(PWM2_MODULE3_CHB_D3, abs(v_Rduty));
			pwm_duty(PWM2_MODULE3_CHA_D2, 0);
		}
		
	

		

		
}