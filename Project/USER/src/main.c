#include "headfile.h"

#define PI 3.1415926

uint8 example_rx_buffer;
lpuart_transfer_t example_receivexfer;
lpuart_handle_t example_g_lpuartHandle;
uint8 uart_data;
int8 buffer[50];
uint8 uart_send = 0;
float dgOffset = 0;
float dgOffsetsum = 0;
float last_dgOffsetsum = 0;
float dgOffsetFront = 0;
float dgOffsetMiddle = 0;
float dgOffsetBehind = 0;
float dgControlOffset = 0;
float sdgControlOffset = 0;
uint8 key_test = 0;
myAdcValue AdcValue;
int16 encoderLeftValue = 0;
int16 encoderRightValue = 0;
uint8 turnFlag = 0;
int16 dgMedium = 3900;       //4200-3900-3600 舵机中值
int16 initLVelocity = 13000; //初始左电机速度
int16 initRVelocity = 13000; //初始右电机速度
int16 LVelocity_temp = 0;
int16 RVelocity_temp = 0;
int16 LVelocity = 0; //左电机速度
int16 RVelocity = 0; //右电机速度
int16 S_LVelocity;   //设定速度，编码器
int16 S_RVelocity;
int16 L_PWM; //深度PID，增量
int16 R_PWM;

PID DG_PID; //舵机pid
PID SDG_PID;
PID SDG_PID_zhidao;   //直道pid
PID SDG_PID_zhidao_2; //直道pid
PID pid_MotorL;
PID pid_MotorR;

float qulv = 0;

float xielv = 0;
float DG_P = 4.5, DG_I = 0, DG_D = 28, ProDG_P = 2.8;
int Bend = 60;
float SDG_P = 6, SDG_I = 0, SDG_D = 8, SProDG_P = 0;
unsigned char uartBuffer[30] = {0};
int uartBufferLen = 0;
int rpmValue = 0;
char sendbuffertest[20];


void checkUartData()
{
    int index = 0;
    char state1 = 0, state2 = 0;
    char dataValid = 0;
    char xiaoshu = 0;
    float data = 0;
    int vdata = 0;

    if (uartBufferLen)
    {
        while (index < uartBufferLen)
        {
            if (index == 0)
            {
                state1 = uartBuffer[index];
            }
            else if (index == 1)
            {
                state2 = uartBuffer[index];
            }
            else if (state1 == 's' && (state2 == 'p' || state2 == 'i' || state2 == 'd'))
            {
                if (uartBuffer[index] <= '9' && uartBuffer[index] >= '0')
                {
                    if (!xiaoshu)
                    {
                        data = uartBuffer[index] - 48 + data * 10;
                    }
                    else
                    {
                        data = data + pow(0.1, xiaoshu) * (uartBuffer[index] - 48);
                        xiaoshu++;
                    }
                }
                else if (uartBuffer[index] == '.')
                {
                    xiaoshu = 1;
                }
                else if (uartBuffer[index] == '!')
                {
                    dataValid = 1;
                }
            }
            if (state1 == 'v')
            {
                if (uartBuffer[index] <= '9' && uartBuffer[index] >= '0')
                {
                    vdata = uartBuffer[index] - 48 + vdata * 10;
                }
                else if (uartBuffer[index] == '!')
                {
                    dataValid = 1;
                }
            }
            index++;
        }
        if (dataValid)
        {
            if (state1 == 's')
            {
                switch (state2)
                {
                case 'p':
                {
                    char buffer[30] = "";
                    sprintf(buffer, "set p from %f to %f\r\n", SDG_PID.KP, data);
                    uart_putbuff(USART_1, (uint8 *)buffer, strlen(buffer));
                }
                    //DG_PID.KP = data;
                    SDG_PID.KP = data;
                    break;
                case 'i':
                {
                    char buffer[30] = "";
                    sprintf(buffer, "set i from %f to %f\r\n", SDG_PID.KI, data);
                    uart_putbuff(USART_1, (uint8 *)buffer, strlen(buffer));
                }
                    //DG_PID.KI = data;
                    SDG_PID.KI = data;
                    break;
                case 'd':
                {
                    char buffer[30] = "";
                    sprintf(buffer, "set d from %f to %f\r\n", SDG_PID.kD, data);
                    uart_putbuff(USART_1, (uint8 *)buffer, strlen(buffer));
                }
                    //DG_PID.kD = data;
                    SDG_PID.kD = data;
                    break;
                default:
                    break;
                }
            }
            else if (state1 == 'v')
            {
                char buffer[30] = "";
                sprintf(buffer, "set v from %d to %d\r\n", initRVelocity, vdata);
                uart_putbuff(USART_1, (uint8 *)buffer, strlen(buffer));
                initRVelocity = vdata;
                initLVelocity = vdata;
            }
        }
        else
        {
            uart_putbuff(USART_1, "invalidData:", strlen("invalidData:"));
            uart_putbuff(USART_1, uartBuffer, strlen(uartBuffer));
            uart_putbuff(USART_1, "\r\n", strlen("\r\n"));
        }
    }
    uartBufferLen = 0;
}
void example_uart_callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
    if (kStatus_LPUART_RxIdle == status)
    {
        uart_data = example_rx_buffer;
        uartBuffer[uartBufferLen] = uart_data;
        uartBufferLen++;
    }
    handle->rxDataSize = example_receivexfer.dataSize;
    handle->rxData = example_receivexfer.data;
}


//int rpmControl(float dgControlOffset, int LVelocity, int RVelocity)
// {
//     float multiple = 1;
//     int abs_dgControlOffset;
//     int DF;
//     double tanValue;

//     abs_dgControlOffset = abs((int)dgControlOffset);
//     tanValue = tan(abs_dgControlOffset * 30 / 320 * PI / 180.0);
//     multiple = (20 / tanValue + 7.5) / (20 / tanValue - 7.5);

//     if (dgControlOffset == 0)
//     {
//         turnFlag = 0;
//         return 0;
//     }
//     else if (dgControlOffset > 0)
//     {
//         DF = (int)((LVelocity * multiple - RVelocity) / (1 + multiple));
//         turnFlag = 1;
//         return DF;
//     }
//     else if (dgControlOffset < 0)
//     {
//         DF = (int)((RVelocity * multiple - LVelocity) / (1 + multiple));
//         turnFlag = -1;
//         return DF;
//     }
// }

void prospectiveControl()
{
    //使用差比和计算偏差
    dgOffset = adcDeviation(&AdcValue);
    //5ms将偏差放入pid得出舵机控制值
    if (dgOffset >= Bend || dgOffset <= -Bend)
    {
        DG_PID.KP = DG_P + ProDG_P;
				gpio_set(B11, 0);
    }
    else
    {
				gpio_set(B11, 1);
        DG_PID.KP = DG_P;
    }
    dgControlOffset = pidOut(dgOffset, &DG_PID);
    if (dgControlOffset >= 320)
    {
        dgControlOffset = 320;
    }
    else if (dgControlOffset <= -320)
    {
        dgControlOffset = -320;
    }
    //sprintf(uartBuffer,"%.2f",dgControlOffset);
    //uart_putbuff(USART_1, uartBuffer, strlen(uartBuffer));
    if (dgControlOffset == 0)
    {
			
    }
    else
    {
        dgControl(dgControlOffset, dgMedium);
    }
    if (AdcValue.adcMidValue == 0 && AdcValue.adcLeftValue == 0 && AdcValue.adcRightValue == 0)
    {
        LVelocity = 0;
        RVelocity = 0;
    }
    else
    {
        LVelocity = initLVelocity;
        RVelocity = initRVelocity;
    }
    carSpeed(LVelocity, RVelocity);
}


int16 a = 0;
void surroundingElectromagneticControl()
{
#define BehindOffset
    //使用差比和计算偏差
    dgOffsetFront = -adcDeviationFront(&AdcValue);
    dgOffsetMiddle = adcDeviationMiddle(&AdcValue);
    dgOffsetBehind = adcDeviationBehind(&AdcValue);
    static float last_dgOffsetFront = 0;
#define frontRC (0)
    dgOffsetFront = dgOffsetFront * (1 - frontRC) + last_dgOffsetFront * frontRC;

    qulv = 0.5f * (dgOffsetFront + dgOffsetBehind) - dgOffsetMiddle;
    static float last_qulv;

    //qulv = 0.8f * qulv + 0.2f * last_qulv;
    float qulvDelta = qulv - last_qulv;
    last_qulv = qulv;

    xielv = dgOffsetFront - dgOffsetMiddle; //dgOffsetBehind;
    //dgOffsetsum = dgOffsetFront;
    // dgOffsetsum = dgOffsetFront + 0.1 * xielv;

    // LVelocity_temp = initLVelocity;
    // RVelocity_temp = initRVelocity;
    if (AdcValue.adcValue2 >= 10 && AdcValue.adcValue4 >= 10)
    {
        // LVelocity_temp = initLVelocity - 800;
        // RVelocity_temp = initRVelocity - 800;
        dgOffsetsum = dgOffsetFront + 0.3 * qulv + 0.2 * qulvDelta; //+0.04f*xielv;
        SDG_P = 80;
        gpio_set(B11, 0);
    }
    if (abs(dgOffsetFront) >= 15 && AdcValue.adcValue2 >= 10 && AdcValue.adcValue4 >= 10)
    {
        // LVelocity_temp = initLVelocity - 800;
        // RVelocity_temp = initRVelocity - 800;
        dgOffsetsum = dgOffsetFront + 0.3 * qulv + 0.2 * qulvDelta; //+0.04f*xielv;
        SDG_P = 7.5;
        gpio_set(B11, 0);
    }
    else
    {
        // LVelocity_temp = initLVelocity + 800;
        // RVelocity_temp = initRVelocity + 800;
        dgOffsetsum = dgOffsetFront;
        SDG_P = 5.5;
        gpio_set(B11, 1);
    }
    // #define qulvOffset 12
    // #define xielvOffser 0
    // if(fabs(qulv-qulvOffset)<10){
    //     dgOffsetsum *=0.8 ;
    //     gpio_set(B11,0);
    // }else{
    //     gpio_set(B11,1);
    // }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // if(abs(dgOffsetBehind)-25>=0)//弯
    // {
    //     #define wanRC (0.05)
    //     dgOffsetsum = 0.2*qulv+dgOffsetFront+0.04f*xielv;
    //     dgOffsetsum = (1-wanRC)*dgOffsetsum + wanRC*last_dgOffsetsum;
    //     last_dgOffsetsum = dgOffsetsum;
    //     gpio_set(B11,0);
    // }
    // else
    // {
    //     dgOffsetsum = 0.7f*(dgOffsetFront+0.04f*xielv);
    //     dgOffsetsum = 0.8f*dgOffsetsum + 0.2f*last_dgOffsetsum;
    //     last_dgOffsetsum = dgOffsetsum;
    //     gpio_set(B11,1);

    // }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //平方
    // dgOffsetsum=0.1*(dgOffsetsum>0?1:-1)*dgOffsetsum*dgOffsetsum;
    //dgOffsetsum=0.5*(dgOffsetsum>0?1:-1)*sqrt(fabs(dgOffsetsum));
    // if (abs(dgOffsetBehind) - 10 <= 0)
    // { //直
    //     LVelocity_temp = initLVelocity + 1000;
    //     RVelocity_temp = initRVelocity + 1000;
    // }
    // else
    // {
    //     LVelocity_temp = initLVelocity;
    //     RVelocity_temp = initRVelocity;
    // }

    // //5ms采集一次编码器值
    // encoderValue(&encoderRightValue, &encoderLeftValue);
    //dgOffsetsum = xielv;
    // sprintf(buffer, "%d %d %d %d\r\n", AdcValue.adcValue1, AdcValue.adcValue5, AdcValue.adcValue2, AdcValue.adcValue4);
    sprintf(buffer, "%.2f %.2f %.2f %.2f %.2f %.2f %d %d\r\n", dgOffsetFront, dgOffsetMiddle, dgOffsetBehind, qulv, xielv, dgOffsetsum, encoderRightValue, encoderLeftValue);
    uart_putbuff(USART_1, (uint8 *)buffer, strlen(buffer));

    //5ms将偏差放入pid得出舵机控制值
    // if (dgOffsetsum >= Bend || dgOffsetsum <= -Bend)
    // {
    //     SDG_PID.KP = SDG_P + SProDG_P;
    // }
    // else
    // {
    //     SDG_PID.KP = SDG_P;
    // }

    // if (fabs(xielv) > 25)
    // { //弯道
    //     float rc = 0.03;
    //     static float rclast = 0;
    //     dgOffsetsum = dgOffsetFront + 0.01 * xielv + 0.2 * qulv + 0.2 * qulvDelta;
    //     dgOffsetsum = rc * rclast + (1 - rc) * dgOffsetsum;
    //     rclast = rc;
    //     gpio_set(B11, 0);

    //     sdgControlOffset = pidOut(dgOffsetsum, &SDG_PID);
    //     if (initLVelocity > 1)
    //     {
    //         LVelocity_temp = initLVelocity - 1500;
    //         RVelocity_temp = initRVelocity - 1500;
    //     }
    //     else
    //     {
    //         LVelocity_temp = initLVelocity;
    //         RVelocity_temp = initRVelocity;
    //     }
    // }
    // else if (fabs(xielv) > 15)
    // {
    //     // static float last
    //     dgOffsetsum = dgOffsetFront + 0.05 * xielv + 0.2 * qulv + 0.2 * qulvDelta;
    //     // dgOffsetsum*=0.6;
    //     gpio_set(B11, 1);
    //     //dgOffsetsum=0.05*(dgOffsetsum>0?1:-1)*dgOffsetsum*dgOffsetsum;
    //      sdgControlOffset = pidOut(dgOffsetsum, &SDG_PID_zhidao);

    // }
    // else
    // { //直道
    //     dgOffsetsum = dgOffsetFront + 0.05 * xielv + 0.3 * qulvDelta;
    //     // dgOffsetsum*=0.6;
    //     gpio_set(B11, 1);
    //     //dgOffsetsum=0.05*(dgOffsetsum>0?1:-1)*dgOffsetsum*dgOffsetsum;
    //     sdgControlOffset = pidOut(dgOffsetsum, &SDG_PID_zhidao_2);

    //     if (initLVelocity > 1)
    //     {
    //         LVelocity_temp = initLVelocity + 1000;
    //         RVelocity_temp = initRVelocity + 1000;
    //     }
    //     else
    //     {
    //         LVelocity_temp = initLVelocity;
    //         RVelocity_temp = initRVelocity;
    //     }
    // }
    // static int cnt = 0;
    // if (AdcValue.adcValue3 > 200)
    // {
    //     cnt = 50;
    //     sdgControlOffset *= 0.5;
    //     if (initLVelocity > 1)
    //     {
    //         LVelocity_temp = initLVelocity + 1000;
    //         RVelocity_temp = initRVelocity + 1000;
    //     }
    //     else
    //     {
    //         LVelocity_temp = initLVelocity;
    //         RVelocity_temp = initRVelocity;
    //     }
    // }
    // else if (cnt > 0)
    // {
    //     cnt--;
    //     sdgControlOffset *= 0.5;
    //     if (initLVelocity > 1)
    //     {
    //         LVelocity_temp = initLVelocity + 1000;
    //         RVelocity_temp = initRVelocity + 1000;
    //     }
    //     else
    //     {
    //         LVelocity_temp = initLVelocity;
    //         RVelocity_temp = initRVelocity;
    //     }
    // }
    sdgControlOffset = pidOut(dgOffsetsum, &SDG_PID);
    if (sdgControlOffset >= 320)
    {
        sdgControlOffset = 320;
    }
    else if (sdgControlOffset <= -320)
    {
        sdgControlOffset = -320;
    }
    if (sdgControlOffset == 0)
    {
    }
    else
    {
        dgControl(sdgControlOffset, dgMedium);
    }
    // if (AdcValue.adcValue1 == 0 && AdcValue.adcValue3 == 0 && AdcValue.adcValue5 == 0)
    // {
    //     LVelocity = 0;
    //     RVelocity = 0;
    // }
    // else
    // {
    //     LVelocity = LVelocity_temp;
    //     RVelocity = RVelocity_temp;
    // }
    if (abs(dgOffsetFront) >= 15 && AdcValue.adcValue2 >= 10 && AdcValue.adcValue4 >= 10)
    {
        S_LVelocity = 100;
        S_RVelocity = 100;
    }
    else
    {
        S_LVelocity = 120;
        S_RVelocity = 120;
    }

//    if(abs(dgOffsetFront) >= 15 && AdcValue.adcValue2 >= 10 && AdcValue.adcValue4 >= 10) //开启差速
//  {
//    if(sdgControlOffset>0)//左
//    {
//      a = (int16)(30 * (sdgControlOffset) * 1.0 / 320);  //得到角度
//      if (a > 30)
//          a = 30;
//    //   if (a < 0)
//    //       a = 0;
//      S_LVelocity = (S_LVelocity * 85 / 100) - (S_LVelocity / 30) * a * 0;
//      S_RVelocity = S_RVelocity * 120 / 100;   //右轮给一点增速;
//    }
//    else//右
//    {
//      a = (int16)(30 * (sdgControlOffset) * 1.0 / (-320));  //得到角度
//      if (a < -30)
//        a = -30;
//    //   if (a < 0
//    //    )
//    //     a = 0;
//      S_LVelocity = S_LVelocity * 120 / 100;
//      S_RVelocity= (S_RVelocity * 85 / 100) + (S_RVelocity / 30) * a * 0;
//    }
//  }

    L_PWM = PID_RealizeL(encoderLeftValue, S_LVelocity);
    R_PWM = PID_RealizeR(encoderRightValue, S_RVelocity);

    LVelocity += L_PWM;
    RVelocity += R_PWM;
    if (LVelocity > 19000)
        LVelocity = 19000;
    if (RVelocity > 19000)
        RVelocity = 19000;
    carSpeed(LVelocity, RVelocity);
}

float cdgOffsetFront = 0;
float last_sgOffsetFront = 0;

int main(void)
{
    DisableGlobalIRQ();
    board_init();
    //DG_PIDINIT
    pidInit(&DG_PID, DG_P, DG_I, DG_D);
    // pidInit(&SDG_PID_zhidao, 4, 0, 3);
    // pidInit(&SDG_PID_zhidao_2, 3, 0, 1);
    // pidInit(&pid_MotorL, 25, 0, 0);
    // pidInit(&pid_MotorR, 25, 0, 0);
    //GPIO B9 GPIO_PIN_CONFIG   led/gpio_set(B9,0);/gpio_toggle(B9);
    gpio_init(B9, GPO, 0, GPIO_PIN_CONFIG);
    //BEEP B11
    gpio_init(B11, GPO, 1, GPIO_PIN_CONFIG);
    //oled SCL B0/ SDA B1/ RST B2/ DC C19/ CS B3
    oled_init();
    //串口115200 TXD16 RXD17
    uart_init(USART_1, 115200, UART1_TX_B12, UART1_RX_B13);
    NVIC_SetPriority(LPUART1_IRQn, 15); //0-15
    uart_rx_irq(USART_1, 1);
    example_receivexfer.dataSize = 1;
    example_receivexfer.data = &example_rx_buffer;
    uart_set_handle(USART_1, &example_g_lpuartHandle, example_uart_callback, NULL, 0, example_receivexfer.data, 1);
    //pit初始化
    pit_init();
    pit_interrupt_ms(PIT_CH0, 5);
    pit_interrupt_ms(PIT_CH1, 50);
    // pit_interrupt_ms(PIT_CH2, 50);
    // pit_interrupt_ms(PIT_CH3, 50);
    NVIC_SetPriority(PIT_IRQn, 15); //0-15
    //舵机初始化50hz
    dgInit();
    //ADC初始化
    myAdcInit();
    //电机初始化
    elmaInit();
    //编码器初始化
    encoderInit();
    //按键初始化
    keyInit();
    //检查重启
    //uart_putbuff(USART_1, "start", strlen("start:"));
    EnableGlobalIRQ(0);

    while (1)
    {

        if (checkTag_5ms())
        {
            AdcValue = getAdcValue();
            //前瞻控制
            prospectiveControl();
        }

        if (checkTag_50ms())
        {
            //50ms检测一次按键
            getKey();
            //50ms接收处理串口数据修改pid的值
            checkUartData();
            //闪灯
            gpio_toggle(B9);
        }
				sprintf(buffer, "%d %d\r\n",encoderRightValue, encoderLeftValue);
				uart_putbuff(USART_1, (uint8 *)buffer, strlen(buffer));
        //OLED显示
        sprintf(buffer, "P:%.2fI:%.2fD:%.2F", DG_PID.KP, DG_PID.KI, DG_PID.kD);
        oled_p6x8str(0, 0, buffer);
        sprintf(buffer, "EnL:%3d   EnR:%3d   ", encoderLeftValue, encoderRightValue);
        oled_p6x8str(0, 4, buffer);
        sprintf(buffer, "1:%3d 2:%3d 3:%3d", AdcValue.adcValue1, AdcValue.adcValue2, AdcValue.adcValue3);
        oled_p6x8str(0, 1, buffer);
        sprintf(buffer, "4:%3d 5:%3d", AdcValue.adcValue4, AdcValue.adcValue5);
        oled_p6x8str(0, 2, buffer);
        sprintf(buffer, "%5d", LVelocity);
        oled_p6x8str(0, 5, buffer);
        
        //systick_delay_ms(100);
    }
}
