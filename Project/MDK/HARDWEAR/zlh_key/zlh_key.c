#include "zlh_key.h"

uint8 key1 = 0, key2 = 0, key3 = 0, KEY = 0;
uint8 key1_flag, key2_flag, key3_flag;

void keyInit()
{
	gpio_init(C26, GPI, 0, GPIO_PIN_CONFIG);
	gpio_init(C27, GPI, 0, GPIO_PIN_CONFIG);
	gpio_init(C31, GPI, 0, GPIO_PIN_CONFIG);
}

uint8 KEY_Scan(uint8 mode)
{
	static uint8 key_up = 1;
	if (mode)
		key_up = 1;
	if (key_up && (key1 == 0 || key2 == 0 || key3 == 0))
	{
		systick_delay_ms(10);
		key_up = 0;
		if (key1 == 0)
			return 1;
		else if (key2 == 0)
			return 2;
		else if (key3 == 0)
			return 3;
	}
	else if (key1 == 1 && key2 == 1 && key3 == 1)
		key_up = 1;
	return 0;
}

void getKey()
{
	key1 = gpio_get(C26);
	key2 = gpio_get(C27);
	key3 = gpio_get(C31);
	KEY = KEY_Scan(0);
	if (KEY)
	{
		switch (KEY)
		{
		case 1:
			key1_flag = 1;
			break;
		case 2:
			key2_flag = 1;
			break;
		case 3:
			key3_flag = 1;
			break;
		default:
			break;
		}
	}
}

char check_Key()
{
	if (key1_flag)
	{
		key1_flag = 0;
		return 1;
	}
	else if (key2_flag)
	{
		key2_flag =0;
		return 2;
	}
	else if (key3_flag)
	{
		key3_flag = 0;
		return 3;
	}
	else
	{
		return 0;
	}
	
}
