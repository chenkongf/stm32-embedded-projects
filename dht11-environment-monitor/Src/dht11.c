#include "dht11.h"



#define Delay_us(X)  delay((X)*72/5)

void delay(unsigned int n)
{
    while(n--);
}

void moshi_out(uint8_t pin_value)
{

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /**/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    if (pin_value)
    {
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);
    }
    else
    {
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0);
    }
}

void moshi_in(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /**/
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


void dht11_init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    moshi_out(1);
}

uint8_t receive_data(void)
{
    uint8_t i, temp = 0, j = 220;

    for(i = 0; i < 8; i++)
    {
        while(!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0));	//50us低电平结束
        Delay_us(40);																				// 该延时严格是40us,因为这里延时函数不标准，所以数字不是40

        if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0)) //为高时
        {
            temp = (temp << 1) | 1;
            while(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0) && (j--));
        }
        else
        {
            temp = (temp << 1) | 0;
        }
    }
    return temp;
}


dht11_t rd_dht11(void)
{
    dht11_t	value;

    moshi_out(0);
    LL_mDelay(20); 
    moshi_out(1);
    Delay_us(50);		//30us
    moshi_in();			//输入模式
    if(!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0)) 				//判断从机的低电平相应
    {
        while(!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0));		// 从机发出的80us低电平相应信号结束
        while(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0));		// 从机发出的80us高电平信号结束
        value.humidity_high = receive_data(); 									//接收高8位湿度数据
        value.humidity_low = receive_data(); 										//接收低8位湿度数据
        value.temperature_high = receive_data(); 								//接收高8位温度数据
        value.temperature_low = receive_data();									//接收低8位温度数据
        value.checkout = receive_data();												//接收校验位
    }
    if (value.humidity_high
            + value.humidity_low
            + value.temperature_high
            + value.temperature_low == value.checkout)  				// 判断校验值
    {
        value.status = 1;
    }
    else
    {
        value.status = 0;
    }

    return value;
}
