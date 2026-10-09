/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
//CAN_HandleTypeDef hcan;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void LED_PIN_Init(void);
void charge_mode (void);
void recharge_mode (void);

/* USER CODE BEGIN PFP */
unsigned char snr, buf[16] = {20, 0}, TagType[2], SelectedSnr[4], DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //TagType[2]为卡片类型， SelectedSnr[4]为卡片的序列号
char str[20] = {0};
char status;
int p=10;
int d=1;
/* USER CODE END PFP */

/* Private user code ------------------------------------------------ ---------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void)
{
    /* USER CODE BEGIN 1 */
    uint8_t *key_value;
    uint8_t mode = 0;				// 定义了RFID模块的工作状态，0:消费模式。1:充值模式；

    /* USER CODE END 1 */


    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* System interrupt init*/

    /** DISABLE: JTAG-DP Disabled and SW-DP Disabled
    */
    LL_GPIO_AF_Remap_SWJ_NOJTAG();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    button_pin_init();
    LED_PIN_Init();
    spi_init();
    oled_init();

    MFRC522_Init();
    /* USER CODE END 2 */
    oled_show_string(OLED_DSP_LINE0, (uint8_t *)"Mode: charge    ");	  // 消费模式

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        /* USER CODE END WHILE */
        key_value = get_button_value();	// 获取键值

        if(*key_value == SW1)
        {
            *key_value = 0xff;
            mode = ~mode;

            if (mode)
            {
                oled_show_string(OLED_DSP_LINE0, (uint8_t *)"Mode: recharge  ");		// 充值模式
            }
            else
            {
                oled_show_string(OLED_DSP_LINE0, (uint8_t *)"Mode: charge    ");	  // 消费模式
            }
        }
		
		else if(*key_value == SW2)
		{
			*key_value = 0xff;
			if (mode)
            {
                p = p+1;
				if(p>20) p=20;
				sprintf(str, "p: %d     ", p);
				oled_show_string(OLED_DSP_LINE1, (uint8_t *)str);
            }
            else
            {
                d = d+1;
				if(d>10) d=10;
				sprintf(str, "d: %d     ", d);
				oled_show_string(OLED_DSP_LINE1, (uint8_t *)str);
            }
		}
		else if(*key_value == SW3)
		{
			*key_value = 0xff;
			if (mode)
            {
                p = p-1;
				if(p<0) p=0;
				sprintf(str, "p: %d     ", p);
				oled_show_string(OLED_DSP_LINE1, (uint8_t *)str);
            }
            else
            {
                d = d-1;
				if(d<0) d=0;
				sprintf(str, "d: %d     ", d);
				oled_show_string(OLED_DSP_LINE1, (uint8_t *)str);
            }
		}

        if (mode)
        {
            recharge_mode();					//充值模式
        }
        else
        {
            charge_mode();						//消费模式
        }

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief recharge mode
  * @retval None
  */
void recharge_mode (void)
{
    status = PcdRequest(REQ_ALL, TagType); //寻卡，寻感应区内所有符合14443A标准的卡片
    if(!status)
    {
        status = PcdAnticoll(SelectedSnr);																					  //防冲撞
        if(!status)
        {
            status = PcdSelect(SelectedSnr);																							//选定卡片
            if(!status)
            {
                snr = 1;  //扇区号1
                status = PcdAuthState(KEYA, (snr * 4 + 3), DefaultKey, SelectedSnr);					// 校验1扇区密码，密码位于每一扇区第3块,验证卡片密码
                {
                    if(!status)
                    {
                        status = PcdRead((snr * 4 + 0), buf);  																		// 读卡，读取1扇区0块数据到buf[0]-buf[16] (其中数字存储在buf[0]中)

                        buf[0] = buf[0] + p;
                        status = PcdWrite((snr * 4 + 0), buf); 																		// 写卡，将buf[0]-buf[16]写入1扇区0块
                        if(!status)
                        {
                            LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0);														// 读写成功，点亮LED
                            //oled_show_string(OLED_DSP_LINE1, (uint8_t *)"Recharge: 10$   ");				// 读卡，并且进行显示
							sprintf(str, "Remain: %3d$     ", buf[0]);
							oled_show_string(OLED_DSP_LINE1, (uint8_t *)str);											  // 读卡，并且进行显示
                            WaitCardOff();																													// 等待卡离开
                        }
                        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);															// 读写结束，关闭LED
                        oled_show_string(OLED_DSP_LINE1, (uint8_t *)"                ");				// 读写结束，清除显示
                    }
                }
            }
        }
    }
}


/**
  * @brief charge mode
  * @retval None
  */
void charge_mode (void)
{
    status = PcdRequest(REQ_ALL, TagType); //寻卡，寻感应区内所有符合14443A标准的卡片
    if(!status)
    {
        status = PcdAnticoll(SelectedSnr);//防冲撞
        if(!status)
        {
            status = PcdSelect(SelectedSnr); //选定卡片
            if(!status)
            {
                snr = 1;  //扇区号1
                status = PcdAuthState(KEYA, (snr * 4 + 3), DefaultKey, SelectedSnr); // 校验1扇区密码，密码位于每一扇区第3块,验证卡片密码
                {
                    if(!status)
                    {
                        status = PcdRead((snr * 4 + 0), buf); // 读卡，读取1扇区0块数据到buf[0]-buf[16] (其中数字存储在buf[0]中)
                        sprintf(str, "Remain: %3d$     ", buf[0]);
                        oled_show_string(OLED_DSP_LINE1, (uint8_t *)str);											  // 读卡，并且进行显示

                        buf[0] = buf[0] - d;
                        status = PcdWrite((snr * 4 + 0), buf);  																	 // 写卡，将buf[0]-buf[16]写入1扇区0块
                        if(!status)
                        {
                            LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0);												 // 读写成功，点亮LED
                            WaitCardOff();																												// 等待卡离开
                        }
                        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);														// 读卡借宿，关闭LED
                        oled_show_string(OLED_DSP_LINE1, (uint8_t *)"                ");				// 读卡结束，清除显示
                    }
                }
            }
        }
    }
}

/**
  * @brief LED Pin initial
  * @retval None
  */
void LED_PIN_Init(void)
{
    LL_GPIO_InitTypeDef		GPIO_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    GPIO_InitStruct.Pin 	=  LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode 	=  LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType 	=  LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Speed 	=  LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
    {
        Error_Handler();
    }
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while(LL_RCC_HSE_IsReady() != 1)
    {

    }
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1)
    {

    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {

    }
    LL_Init1msTick(72000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(72000000);
    LL_SYSTICK_EnableIT();
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
