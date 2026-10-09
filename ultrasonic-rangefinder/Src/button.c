#include "button.h"

static uint8_t key_state = 0;
uint8_t key_value = 0;


uint8_t *get_button_value (void)
{
    return &key_value;
}

void button_pin_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_APB2_GRP1_EnableClock(SW1_3_CLOCK);
    LL_APB2_GRP1_EnableClock(SW4_CLOCK);

    /**/
    GPIO_InitStruct.Pin = SW1_PIN | SW2_PIN | SW3_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(SW1_3_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SW4_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(SW4_PORT, &GPIO_InitStruct);

}

void button (void)
{
    switch (key_state)
    {
    case 0:
        if (!RD_SW1)
        {
            key_state = 1;
        }
        else if (!RD_SW2)
        {
            key_state = 1;
        }
        else if (!RD_SW3)
        {
            key_state = 1;
        }
        else if (!RD_SW4)
        {
            key_state = 1;
        }
        else
        {
            key_state = 0;
        }
        break;

    case 1:
        if (!RD_SW1)
        {
            key_value = SW1;
            key_state = 2;
        }
        else if (!RD_SW2)
        {
            key_value = SW2;
            key_state = 2;
        }
        else if (!RD_SW3)
        {
            key_value = SW3;
            key_state = 2;
        }
        else if (!RD_SW4)
        {
            key_value = SW4;
            key_state = 2;
        }
        else
        {
            key_value = 0xff;
            key_state = 0;
        }
        break;

    case 2:
        if (RD_SW1 && RD_SW2 && RD_SW3 && RD_SW4)
        {
            key_state = 0;
        }
        break;

    default :
        ;
        break;
    }
}

