#ifndef __DS18B20_H
#define __DS18B20_H

#include "main.h"


#define OW_PIN_CLOCK	LL_APB2_GRP1_PERIPH_GPIOA
#define OW_PIN_PORT		GPIOA
#define OW_PIN				LL_GPIO_PIN_0

#define OW_DIR_OUT() 	mode_output1()
#define OW_DIR_IN() 	mode_input1()
#define OW_OUT_LOW()  LL_GPIO_ResetOutputPin(OW_PIN_PORT,OW_PIN);
#define OW_GET_IN()  	LL_GPIO_IsInputPinSet(OW_PIN_PORT,OW_PIN)

#define OW_SKIP_ROM 			0xCC
#define DS18B20_CONVERT 	0x44
#define DS18B20_READ 			0xBE


void ds18b20_pin_init(void);
uint16_t ds18b20_read(void);

#endif

