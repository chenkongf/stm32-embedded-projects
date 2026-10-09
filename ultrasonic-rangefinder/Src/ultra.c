#include "ultra.h"

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init_Ultra(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    /**/
    //	  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
    //  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
    //  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    //  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0);

}

/**
  * @brief GPIO EXTI Initialization Function
  * @param None
  * @retval None
  */
void MX_EXTI_Init_Ultra(void)
{
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

    /* GPIO Ports Clock Enable */

    LL_GPIO_AF_SetEXTISource(LL_GPIO_AF_EXTI_PORTA, LL_GPIO_AF_EXTI_LINE1);

    /**/
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_1;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_FLOATING);
    /* EXTI interrupt init */
    NVIC_SetPriority(EXTI1_IRQn, 0);
    NVIC_EnableIRQ(EXTI1_IRQn);
}

/**
  * @brief This function handles EXTI line1 interrupt.
  */
uint16_t sonic_time = 0;
void EXTI1_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI1_IRQn 0 */

    /* USER CODE END EXTI1_IRQn 0 */
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_1) != RESET)
    {
        if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_1) == 1)  			// 如果上升沿，开始计时。
        {
            LL_TIM_EnableCounter(TIM4);
        }
        else
        {
            LL_TIM_DisableCounter(TIM4);														// 高电平结束，关闭计时器
            sonic_time = LL_TIM_GetCounter(TIM4);										// 获取计时器时间
            LL_TIM_SetCounter(TIM4, 0);															// 清除上计时器时间，准备下一次计时
        }
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
        /* USER CODE BEGIN LL_EXTI_LINE_1 */

        /* USER CODE END LL_EXTI_LINE_1 */
    }
    /* USER CODE BEGIN EXTI1_IRQn 1 */

    /* USER CODE END EXTI1_IRQn 1 */
}

