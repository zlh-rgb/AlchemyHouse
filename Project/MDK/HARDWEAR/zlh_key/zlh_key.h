#ifndef ZLH_KEY_H

#include "zf_gpio.h"
#include "zf_systick.h"
#include "zf_iomuxc.h"

void getKey();
void keyInit();
uint8 KEY_Scan(uint8 mode);

#endif