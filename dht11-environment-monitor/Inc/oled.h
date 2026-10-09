#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "i2c_oled.h"

#define Max_Column	128
#define Max_Row		32

//
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr);
void oled_show_string(uint8_t line, uint8_t *chr);
void oled_init(void);
void oled_clear(void);

//
typedef enum
{
    OLED_DSP_LINE0 = 0,
    OLED_DSP_LINE1 = 2,
} OLED_DSP_LINE;


#endif
