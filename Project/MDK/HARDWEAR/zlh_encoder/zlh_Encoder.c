#include "zlh_Encoder.h"

void encoderInit(){
// QTIMER_1 QTIMER1_TIMER0_C0 A相QTIMER1_TIMER1_C1
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER1_C1,QTIMER1_TIMER0_C0);
// QTIMER_1 QTIMER1_TIMER2_C2 A相QTIMER1_TIMER3_C24
    qtimer_quad_init(QTIMER_1,QTIMER1_TIMER3_C24,QTIMER1_TIMER2_C2);    
}

void encoderValue(int16 *encoder1,int16 *encoder2){
    *encoder1 = qtimer_quad_get(QTIMER_1,QTIMER1_TIMER1_C1 );
    *encoder2 = -qtimer_quad_get(QTIMER_1,QTIMER1_TIMER3_C24 );		
    qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER1_C1 );
    qtimer_quad_clear(QTIMER_1,QTIMER1_TIMER3_C24 );
}
