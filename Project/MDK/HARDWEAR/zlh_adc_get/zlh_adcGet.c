#include "zlh_adcGet.h"
#include "zf_adc.h"

myAdcValue myAdc;

void myAdcInit()
{
	//车周围adc初始化
	//前五从左到右
	adc_init(ADC_1, ADC1_CH9_B20, ADC_8BIT);
	adc_init(ADC_1,ADC2_CH0_B27, ADC_8BIT);
	adc_init(ADC_1, ADC1_CH15_B26, ADC_8BIT);
	adc_init(ADC_1, ADC1_CH13_B24, ADC_8BIT);
	adc_init(ADC_1, ADC1_CH11_B22, ADC_8BIT);
//	//中二从左到右
//	adc_init(ADC_1, ADC1_CH6_B17, ADC_8BIT);
//	adc_init(ADC_1, ADC1_CH5_B16, ADC_8BIT);
//	//后二从左到右
//	adc_init(ADC_1, ADC1_CH3_B14, ADC_8BIT);
//	adc_init(ADC_1, ADC1_CH4_B15, ADC_8BIT);
//	//前瞻adc初始化
//	adc_init(ADC_1, ADC1_CH13_B24, ADC_8BIT);//左
//	adc_init(ADC_1, ADC1_CH9_B20, ADC_8BIT);//中
//	adc_init(ADC_1, ADC1_CH15_B26, ADC_8BIT);//右
}

myAdcValue getAdcValue()
{
	myAdc.adcValue1 = adc_convert(ADC_1, ADC1_CH9_B20);
	myAdc.adcValue2 = adc_convert(ADC_1, ADC2_CH0_B27);
	myAdc.adcValue3 = adc_convert(ADC_1, ADC1_CH15_B26);
	myAdc.adcValue4 = adc_convert(ADC_1, ADC1_CH13_B24);
	myAdc.adcValue5 = adc_convert(ADC_1, ADC1_CH11_B22);

//	myAdc.adcValue6 = adc_convert(ADC_1, ADC1_CH6_B17);
//	myAdc.adcValue7 = adc_convert(ADC_1, ADC1_CH5_B16);
//	
//	myAdc.adcValue8 = adc_convert(ADC_1, ADC1_CH3_B14);
//	myAdc.adcValue9 = adc_convert(ADC_1, ADC1_CH4_B15);

	myAdc.adcLeftValue = adc_convert(ADC_1, ADC1_CH9_B20);
	myAdc.adcMidValue = adc_convert(ADC_1, ADC1_CH15_B26);
	myAdc.adcRightValue = adc_convert(ADC_1, ADC1_CH11_B22);
//	if(myAdc.adcLeftValue<=12 && myAdc.adcMidValue<=5){myAdc.adcLeftValue=0;}
//	if(myAdc.adcRightValue<=12 && myAdc.adcMidValue<=5){myAdc.adcRightValue=0;}

	return myAdc;
}

float adcDeviation(myAdcValue *Adc)
{
	float deviation;
	
	if(Adc->adcLeftValue ==0 && Adc->adcMidValue == 0 && Adc->adcRightValue == 0)
	{
		return 0;
	}

	deviation = (float)(Adc->adcLeftValue - Adc->adcRightValue) / (Adc->adcLeftValue + Adc->adcMidValue + Adc->adcRightValue);
	deviation = deviation * 100;

	return deviation;
}

float adcDeviationFront(myAdcValue *Adc)
{
	float deviation;
	
	if(Adc->adcValue1 ==0 && Adc->adcValue5 == 0 &&Adc->adcValue3 == 0 )// AdAdc->adcValue3 == 0 &&c->adcValue3 == 0 &&
	{
		return 0;
	}
	deviation = (float)(Adc->adcValue1 - Adc->adcValue5 ) / (Adc->adcValue1  + Adc->adcValue3+ Adc->adcValue5);//+ Adc->adcValue3
	deviation = deviation * 100;

	return deviation;
}

float adcDeviationMiddle(myAdcValue *Adc)
{
	float deviation;
	
	if(Adc->adcValue6 ==0 && Adc->adcValue9 == 0)
	{
		return 0;
	}
	deviation = (float)(Adc->adcValue6 - Adc->adcValue9 ) / (Adc->adcValue6 + Adc->adcValue9);
	deviation = deviation * 100;

	return deviation;
}

float adcDeviationBehind(myAdcValue *Adc)
{
	float deviation;
	
	if(Adc->adcValue7 ==0 && Adc->adcValue8 == 0)
	{
		return 0;
	}
	deviation = (float)(Adc->adcValue7 - Adc->adcValue8 ) / (Adc->adcValue7 + Adc->adcValue8);
	deviation = deviation * 100;

	return deviation;
}

