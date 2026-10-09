#ifndef __BUTTON_H
#define __BUTTON_H

#include "main.h"

enum KEY {SW1, SW2, SW3, SW4};

#define SW1_3_CLOCK		LL_APB2_GRP1_PERIPH_GPIOC
#define SW4_CLOCK		 	LL_APB2_GRP1_PERIPH_GPIOB

#define SW1_3_PORT		GPIOC
#define SW4_PORT			GPIOB

#define SW1_PIN				LL_GPIO_PIN_15
#define SW2_PIN				LL_GPIO_PIN_14
#define SW3_PIN				LL_GPIO_PIN_13
#define SW4_PIN				LL_GPIO_PIN_4
#define RD_SW1				LL_GPIO_IsInputPinSet(SW1_3_PORT,SW1_PIN)
#define RD_SW2				LL_GPIO_IsInputPinSet(SW1_3_PORT,SW2_PIN)
#define RD_SW3				LL_GPIO_IsInputPinSet(SW1_3_PORT,SW3_PIN)
#define RD_SW4				LL_GPIO_IsInputPinSet(SW4_PORT,SW4_PIN)

void button(void);
uint8_t *get_button_value (void);
void button_pin_init(void);

#endif

