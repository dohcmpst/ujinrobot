/************************************************************************************//**
* \file         Demo/ARMCM3_STM32F2_Olimex_STM32P207_IAR/Boot/main.c
* \brief        Bootloader application source file.
* \ingroup      Boot_ARMCM3_STM32F2_Olimex_STM32P207_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2018  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "stm32f2xx.h"                           /* STM32 registers and drivers        */
#include "stm32f2xx_ll_rcc.h"                    /* STM32 LL RCC header                */
#include "stm32f2xx_ll_bus.h"                    /* STM32 LL BUS header                */
#include "stm32f2xx_ll_system.h"                 /* STM32 LL SYSTEM header             */
#include "stm32f2xx_ll_utils.h"                  /* STM32 LL UTILS header              */
#include "stm32f2xx_ll_usart.h"                  /* STM32 LL USART header              */
#include "stm32f2xx_ll_gpio.h"                   /* STM32 LL GPIO header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);
static void SystemClock_Config(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called
**            by the reset interrupt vector after the C-startup routines executed.
** \return    none.
**
****************************************************************************************/
#if 0
void Boot_Init_Message(void)
{
    void Rs232TransmitByte(blt_int8u data);
    blt_int16u k;
    const blt_int8u initMsg[] = "\n\n\n/* -- STM32F205RC Booter - Ver1.10  -- */ \n";
  
    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);   // RS485 - TX
    HAL_Delay(1);
    Rs232TransmitByte('\n');
    for(k = 0; k < sizeof(initMsg); k++)
        Rs232TransmitByte(initMsg[k]);
}
#else
void Boot_Init_Message(void)
{
    Debug_Message("\n\n\n");
    Debug_Message("--------------------------------------------\n");
    Debug_Message("/*     STM32F205RC Booter - Ver1.10       */\n");
    Debug_Message("     . BOOT_NVM_SIZE_KB = 256               \n");
    Debug_Message("     . BOOT_CPU_USER_PROGRAM_START_HOOK (0) \n");
    Debug_Message("--------------------------------------------\n\n");
}
#endif

#if 1
void Debug_Message(const blt_int8u debugMsg[] )
{
    
    int k;
  
    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);   // RS485 - TX
    HAL_Delay(1);
    
    for(k = 0; k < 100 && debugMsg[k] != NULL; k++)
    {
        Rs232TransmitByte(debugMsg[k]);
    }
}
#else
void Debug_Message(const blt_int8u debugMsg[], int len )
{
    void Rs232TransmitByte(blt_int8u data);
    int k;
  
    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);   // RS485 - TX
    HAL_Delay(1);
    for(k = 0; k < len; k++)
    {
    //    Rs232TransmitByte(debugMsg[k]);
    }
    
    for(k = 0; k < len && debugMsg[k] != NULL; k++)
    {
        Rs232TransmitByte(debugMsg[k]);
    }
}
#endif




int main(void)
{
    /* initialize the microcontroller */
    Init();
    /* initialize the bootloader */
    BootInit();

    Boot_Init_Message();

    /* start the infinite program loop */
    while (1)
    {
        /* run the bootloader task */
        BootTask();
    }
    
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* HAL library initialization */
  HAL_Init();
  /* configure system clock */
  SystemClock_Config();
} /*** end of Init ***/


/************************************************************************************//**
** \brief     System Clock Configuration. This code was created by CubeMX and configures
**            the system clock to match the configuration in the bootloader's
**            configuration (blt_conf.h), specifically the macros:
**            BOOT_CPU_SYSTEM_SPEED_KHZ and BOOT_CPU_XTAL_SPEED_KHZ.
**            Note that the Lower Layer drivers were selected in CubeMX for the RCC
**            subsystem.
** \return    none.
**
****************************************************************************************/
static void SystemClock_Config(void)
{
    /* Set flash latency. */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
    /* Verify flash latency setting. */
    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_3)
    {
      /* Error setting flash latency. */
      ASSERT_RT(BLT_FALSE);
    }

    /* Enable the HSE clock. */
    LL_RCC_HSE_Enable();
    
    /* Wait till HSE is ready. */
    while(LL_RCC_HSE_IsReady() != 1)
    {
      ;
    }

    /* Configure and enable the PLL. */
    //LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 240, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_20, 192, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_Enable();
    
    /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1)
    {
      ;
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
      ;
    }

    /* Update the system clock speed setting. */
    LL_SetSystemCoreClock(BOOT_CPU_SYSTEM_SPEED_KHZ * 1000u);
} /*** end of SystemClock_Config ***/


/************************************************************************************//**
** \brief     Initializes the Global MSP. This function is called from HAL_Init()
**            function to perform system level initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspInit(void)
{
    //LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* SYSCFG clock enable. */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

    /* GPIO ports clock enable. */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    //LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
    //LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);

    #if ( BOOT_DEBUG_UART_ENABLE > 0 )
      static void MSP_Init_USART(void);
      MSP_Init_USART();
    #endif 

    #if (BOOT_COM_CAN_ENABLE > 0)
      static void MSP_Init_CAN(void);
      MSP_Init_CAN();
    #endif
  
    static void  MSP_Init_GPIO(void);
    MSP_Init_GPIO();


} /*** end of HAL_MspInit ***/

#if ( ((BOOT_FILE_LOGGING_ENABLE > 0) && (BOOT_COM_RS232_ENABLE == 0)) || \
      (BOOT_DEBUG_UART_ENABLE > 0) ) 

static void MSP_Init_USART(void)
{
    //LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    
    /**USART6 GPIO Configuration
    PC6   ------> USART6_TX
    PC7   ------> USART6_RX
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

  
#endif


#if (BOOT_COM_CAN_ENABLE > 0)

static void MSP_Init_CAN(void)
{
    /* CAN clock enable. Note that when using CAN2, the clock for peripheral CAN1 must
    * also be enabled, because CAN2 is a slave of CAN1.
    */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_CAN1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_CAN2);

    /* CAN TX and RX GPIO pin configuration. */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_12 | LL_GPIO_PIN_13;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_9;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
#endif


static void  MSP_Init_GPIO(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Configure GPIO pin for the POARTA : LED = 11, PRE = 8, CHG = 9, DSG = 10, FRAM_CS = 15, LTC1_CS = 3, LTC2_CS = 4  */
    GPIO_InitStruct.Pin = (LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_15);
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);
        
    /* Configure GPIO pin for the LED. */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);
  
    /* RS485 Enable Pin */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);     // read mode
    //LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);
    
    
    /* Configure GPIO pin for (optional) backdoor entry input. */
    #if 0
      GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
      GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
      LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    #endif
    
}
  


/************************************************************************************//**
** \brief     DeInitializes the Global MSP. This function is called from HAL_DeInit()
**            function to perform system level de-initialization (GPIOs, clock, DMA,
**            interrupt).
** \return    none.
**
****************************************************************************************/
void HAL_MspDeInit(void)
{
  /* Reset the RCC clock configuration to the default reset state. */
  LL_RCC_DeInit();
  

  /* Deinit used GPIOs. */
  //LL_GPIO_DeInit(GPIOF);
  //LL_GPIO_DeInit(GPIOD);
  LL_GPIO_DeInit(GPIOC);
  LL_GPIO_DeInit(GPIOB);
  LL_GPIO_DeInit(GPIOA);

#if (BOOT_COM_CAN_ENABLE > 0)
  /* CAN clock disable. */
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_CAN1);
#endif

#if (BOOT_COM_RS232_ENABLE > 0) || (BOOT_FILE_LOGGING_ENABLE > 0)
  /* UART clock disable. */
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_USART3);
#endif

  /* GPIO ports clock disable. */
  //LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  /* SYSCFG clock disable. */
  LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
} /*** end of HAL_MspDeInit ***/


/************************************************************************************//**
** \brief     This function handles the SysTick interrupt. The HAL driver is initialized
**            before the bootloader disables the global interrupts and reconfigures the
**            SysTick. It is theoretically possible that the SysTick interrupt still
**            fires before the timer driver disables it. Therefore the handler is
**            implemented here. If not, then the default handler from the C startup 
**            code is used, which hangs the system.
** \return    none.
**
****************************************************************************************/
void SysTick_Handler(void)
{
  /* Nothing to do here. */
} /*** end of SysTick_Handler ***/


/*********************************** end of main.c *************************************/
