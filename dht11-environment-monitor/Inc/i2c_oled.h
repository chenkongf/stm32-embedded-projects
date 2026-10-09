#ifndef __I2C_OLED_H
#define __I2C_OLED_H

#include "main.h"

#define OLED_SCL_PORT		GPIOB
#define OLED_SDA_PORT		GPIOB
#define OLED_PWR_PORT		GPIOB
#define OLED_SCL_PIN		LL_GPIO_PIN_6	//PB6
#define	OLED_SDA_PIN		LL_GPIO_PIN_7	//PB7
#define OLED_PWR_PIN		LL_GPIO_PIN_5

//
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_wait_ack(void);
void i2c_send_ack(void);
void i2c_send_noack(void);
void i2c_send_byte(unsigned char cSendByte);
uint8_t i2c_receive_byte(void);

#endif
