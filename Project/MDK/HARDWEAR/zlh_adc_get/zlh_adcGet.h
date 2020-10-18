#ifndef ADC_GET_H
#include "common.h"

typedef struct {
//车周围的7个电感
    uint8 adcValue1; 
    uint8 adcValue2;
    uint8 adcValue3;
    uint8 adcValue4;
    uint8 adcValue5;
    uint8 adcValue6;
    uint8 adcValue7;
    uint8 adcValue8;
    uint8 adcValue9;
//前瞻的三个电感
    uint8 adcLeftValue;
    uint8 adcMidValue;
    uint8 adcRightValue;
}myAdcValue;

void myAdcInit();
myAdcValue getAdcValue();
float adcDeviation(myAdcValue *Adc);
float adcDeviationFront(myAdcValue *Adc);
float adcDeviationMiddle(myAdcValue *Adc);
float adcDeviationBehind(myAdcValue *Adc);
#endif