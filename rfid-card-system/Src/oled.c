#include "oled.h"
#include "font.h"

//
void oled_write_command(uint8_t command)
{
    i2c_start();
    i2c_send_byte(0x78);
    i2c_wait_ack();

    i2c_send_byte(0x00);
    i2c_wait_ack();
    i2c_send_byte(command);
    i2c_wait_ack();
    i2c_stop();
}

//
void oled_write_data(uint8_t data)
{
    i2c_start();
    i2c_send_byte(0x78);
    i2c_wait_ack();

    i2c_send_byte(0x40);
    i2c_wait_ack();
    i2c_send_byte(data);
    i2c_wait_ack();
    i2c_stop();
}

//
void oled_display_on(void)
{
    oled_write_command(0x8D);
    oled_write_command(0x14);
    oled_write_command(0xAF);
}

//
void oled_display_off(void)
{
    oled_write_command(0x8D);
    oled_write_command(0x10);
    oled_write_command(0xAE);
}

//x:0-127
//y:0-3
void oled_set_pos(uint8_t x, uint8_t y)
{
    oled_write_command(0xb0 + y);
    oled_write_command(((x & 0xf0) >> 4) | 0x10);
    oled_write_command((x & 0x0f));
}

//
void oled_clear(void)
{
    uint8_t i, n;

    for(i = 0; i < 4; i++)
    {
        oled_write_command (0xb0 + i);
        oled_write_command (0x00);
        oled_write_command (0x10);
        for(n = 0; n < 128; n++)
        {
            oled_write_data(0);
        }
    }
}


//
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr)
{
    uint8_t  c = 0, i = 0;
    c = chr - ' ';

    if(x > Max_Column - 1)
    {
        x = 0;
        y = y + 2;
    }

    oled_set_pos(x, y);
    for(i = 0; i < 8; i++)
        oled_write_data(F8X16[c * 16 + i]);

    oled_set_pos(x, y + 1);
    for(i = 0; i < 8; i++)
        oled_write_data(F8X16[c * 16 + i + 8]);
}

//
void oled_show_string(uint8_t line, uint8_t *chr)
{
    unsigned char index = 0;
    unsigned char xpos = 0;

    while(chr[index] != 0)
    {
        oled_show_char(xpos, line, chr[index]);

        xpos += 8;

        if(xpos >= 128)
        {
            break;
        }

        index++;
    }
}

//
void oled_pin_config(void)
{
    LL_GPIO_InitTypeDef GPIO_Struct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);


    GPIO_Struct.Pin = OLED_SDA_PIN ;
    GPIO_Struct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_Struct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_Struct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_Struct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(OLED_SDA_PORT, &GPIO_Struct);

    GPIO_Struct.Pin = OLED_SCL_PIN;
    GPIO_Struct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_Struct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_Struct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_Struct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(OLED_SCL_PORT, &GPIO_Struct);

    GPIO_Struct.Pin = OLED_PWR_PIN;
    GPIO_Struct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_Struct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_Struct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_Struct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(OLED_PWR_PORT, &GPIO_Struct);
}

//
void oled_init(void)
{
    oled_pin_config();
    LL_mDelay(10);

    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);
    LL_mDelay(100);

    oled_write_command(0xAE);

    oled_write_command(0xD5);
    oled_write_command(0x80);

    oled_write_command(0xA8);
    oled_write_command(0x1F);

    oled_write_command(0xD3);
    oled_write_command(0x00);


    oled_write_command(0x40);

    oled_write_command(0x8D);
    oled_write_command(0x14);

    oled_write_command(0xA1);
    oled_write_command(0xC8);

    oled_write_command(0xDA);
    oled_write_command(0x00);
    oled_write_command(0x81);


    oled_write_command(0x8F);
    oled_write_command(0xD9);
    oled_write_command(0x1F);
    oled_write_command(0xDB);
    oled_write_command(0x40);
    oled_write_command(0xA4);

    oled_clear();
    oled_write_command(0xAF);

    LL_mDelay(100);
}
