#include "i2c_oled.h"

#define DELAY_TIME	2

//
static void delay1(unsigned int n)
{
    unsigned int i;
    for ( i = 0; i < n; ++i);
}

//
void sda_mode_input(void)
{
    LL_GPIO_SetPinPull(OLED_SDA_PORT, OLED_SDA_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(OLED_SDA_PORT, OLED_SDA_PIN, LL_GPIO_MODE_INPUT);
}

//
void sda_mode_output(void)
{
    LL_GPIO_SetPinMode(OLED_SDA_PORT, OLED_SDA_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(OLED_SDA_PORT, OLED_SDA_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

//
void sda_output( uint16_t val )
{
    if(val)
    {
        LL_GPIO_SetOutputPin(OLED_SDA_PORT, OLED_SDA_PIN);
    }
    else
    {
        LL_GPIO_ResetOutputPin(OLED_SDA_PORT, OLED_SDA_PIN);
    }
}

//
void scl_output( uint16_t val )
{
    if(val)
    {
        LL_GPIO_SetOutputPin(OLED_SCL_PORT, OLED_SCL_PIN);
    }
    else
    {
        LL_GPIO_ResetOutputPin(OLED_SCL_PORT, OLED_SCL_PIN);
    }
}

//
uint8_t sda_input(void)
{
    return(LL_GPIO_IsInputPinSet(OLED_SDA_PORT, OLED_SDA_PIN));
}

//
void i2c_start(void)
{
    sda_output(1);
    delay1(DELAY_TIME);
    scl_output(1);
    delay1(DELAY_TIME);
    sda_output(0);
    delay1(DELAY_TIME);
    scl_output(0);
    delay1(DELAY_TIME);
}

//
void i2c_stop(void)
{
    scl_output(0);
    delay1(DELAY_TIME);
    sda_output(0);
    delay1(DELAY_TIME);
    scl_output(1);
    delay1(DELAY_TIME);
    sda_output(1);
    delay1(DELAY_TIME);

}

//
unsigned char i2c_wait_ack(void)
{
    unsigned short cErrTime = 5;

    sda_mode_input();
    delay1(DELAY_TIME);
    scl_output(1);
    delay1(DELAY_TIME);
    while(sda_input())
    {
        cErrTime--;
        delay1(DELAY_TIME);
        if (0 == cErrTime)
        {
            sda_mode_output();
            i2c_stop();
            return ERROR;
        }
    }
    sda_mode_output();
    scl_output(0);
    delay1(DELAY_TIME);
    return SUCCESS;
}

//
void i2c_send_ack(void)
{
    sda_output(0);
    delay1(DELAY_TIME);
    delay1(DELAY_TIME);
    scl_output(1);
    delay1(DELAY_TIME);
    scl_output(0);
    delay1(DELAY_TIME);

}

//
void i2c_send_noack(void)
{
    sda_output(1);
    delay1(DELAY_TIME);
    scl_output(1);
    delay1(DELAY_TIME);
    scl_output(0);
    delay1(DELAY_TIME);

}

//
void i2c_send_byte(unsigned char cSendByte)
{
    unsigned char  i = 8;
    while (i--)
    {
        scl_output(0);
        delay1(DELAY_TIME);
        sda_output(cSendByte & 0x80);
        delay1(DELAY_TIME);
        cSendByte += cSendByte;
        delay1(DELAY_TIME);
        scl_output(1);
        delay1(DELAY_TIME);
    }
    scl_output(0);
    delay1(DELAY_TIME);
}

//
unsigned char i2c_receive_byte(void)
{
    unsigned char i = 8;
    unsigned char cR_Byte = 0;

    sda_mode_input();
    while (i--)
    {
        cR_Byte += cR_Byte;
        scl_output(0);
        delay1(DELAY_TIME);
        delay1(DELAY_TIME);
        scl_output(1);
        delay1(DELAY_TIME);
        cR_Byte |=  sda_input();
    }
    scl_output(0);
    delay1(DELAY_TIME);
    sda_mode_output();

    return cR_Byte;
}
