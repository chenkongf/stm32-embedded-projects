#include "main.h"


void spi_init(void)
{
    LL_GPIO_InitTypeDef  GPIO_InitStruct;

    LL_APB2_GRP1_EnableClock(SPI_CLOCK);

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin 					= SCK;
    GPIO_InitStruct.Mode 					= LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType 		= LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull					= LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Speed 				= LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(SCK_PORT, &GPIO_InitStruct);


    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = MISO;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(MISO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin 					= MOSI;
    GPIO_InitStruct.Mode 					= LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType 		= LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull					= LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Speed 				= LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(MOSI_PORT, &GPIO_InitStruct);

    /* SPI NSS GPIO pin configuration  */
    GPIO_InitStruct.Pin 					= NSS;
    GPIO_InitStruct.Mode 					= LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType 		= LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull					= LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed 				= LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(NSS_PORT, &GPIO_InitStruct);

    /* SPI NSS GPIO pin configuration  */
    GPIO_InitStruct.Pin 					= RST;
    GPIO_InitStruct.Mode 					= LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType 		= LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull					= LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Speed 				= LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(RST_PORT, &GPIO_InitStruct);

    LL_GPIO_SetOutputPin(SCK_PORT, SCK);
    LL_GPIO_SetOutputPin(MOSI_PORT, MOSI);
    LL_GPIO_SetOutputPin(NSS_PORT, NSS);
    LL_GPIO_SetOutputPin(RST_PORT, RST);

}


void Delaymm(void)
{
    uint8_t index;
    for(index = 0; index < 20; index++);
}

/////////////////////////////////////////////////////////////////////
//功    能：读寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
    unsigned char i, ucAddr;
    unsigned char ucResult = 0;

    NSS_L;
    ucAddr = ((Address << 1) & 0x7E) | 0x80;

    for(i = 8; i > 0; i--)
    {
        SCK_L;
        if(ucAddr & 0x80)
            MOSI_H;
        else
            MOSI_L;
        SCK_H;
        Delaymm();
        ucAddr <<= 1;
        Delaymm();

    }

    for(i = 8; i > 0; i--)
    {
        SCK_L;
        ucResult <<= 1;
        SCK_H;
        if(READ_MISO == 1)
            ucResult |= 1;
    }

    NSS_H;
    SCK_H;
    return ucResult;
}

/////////////////////////////////////////////////////////////////////
//功    能：写寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{
    unsigned char i, ucAddr;

    SCK_L;
    NSS_L;
    ucAddr = ((Address << 1) & 0x7E);
    for(i = 8; i > 0; i--)
    {
        if(ucAddr & 0x80)
            MOSI_H;
        else
            MOSI_L;
        SCK_H;
        Delaymm();
        ucAddr <<= 1;
        SCK_L;
        Delaymm();
    }
    for(i = 8; i > 0; i--)
    {
        if(value & 0x80)
            MOSI_H;
        else
            MOSI_L;
        SCK_H;
        Delaymm();
        value <<= 1;
        SCK_L;
        Delaymm();
    }
    NSS_H;
    SCK_H;
}



