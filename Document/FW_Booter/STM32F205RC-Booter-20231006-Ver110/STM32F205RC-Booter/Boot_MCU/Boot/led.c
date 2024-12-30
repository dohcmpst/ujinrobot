/************************************************************************************//**
* \file         Demo/ARMCM3_STM32F2_Olimex_STM32P207_IAR/Boot/led.c
* \brief        LED driver source file.
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
#include "led.h"                                 /* module header                      */
#include "stm32f2xx.h"                           /* STM32 registers and drivers        */
#include "stm32f2xx_ll_gpio.h"                   /* STM32 LL GPIO header               */


/****************************************************************************************
* Local data declarations
****************************************************************************************/
/** \brief Holds the desired LED blink interval time. */
static blt_int16u ledBlinkIntervalMs;


/************************************************************************************//**
** \brief     Initializes the LED blink driver.
** \param     interval_ms Specifies the desired LED blink interval time in milliseconds.
** \return    none.
**
****************************************************************************************/
void LedBlinkInit(blt_int16u interval_ms)
{
  /* store the interval time between LED toggles */
  ledBlinkIntervalMs = interval_ms;
} /*** end of LedBlinkInit ***/


/************************************************************************************//**
** \brief     Task function for blinking the LED as a fixed timer interval.
** \return    none.
**
****************************************************************************************/


void LedBlinkTask(void)
{
    static blt_bool ledOn = BLT_FALSE;
    static blt_int32u nextBlinkEvent = 0;

    /* check for blink event */
    if (TimerGet() >= nextBlinkEvent)
    {
      /* toggle the LED state */
      if (ledOn == BLT_FALSE)
      {
        ledOn = BLT_TRUE;
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11);
      }
      else
      {
          ledOn = BLT_FALSE;
          LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);   // LL_GPIO_ResetOutputPin(GPIOF, LL_GPIO_PIN_8);
        }
      /* schedule the next blink event */
      nextBlinkEvent = TimerGet() + ledBlinkIntervalMs;
      

               
#if 0
      static blt_int8u testCanData[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
      static blt_int8u testByte = 0x55;
      /* CAN Test : dnpark */
      void CanTransmitPacket_StdId(blt_int8u *data, blt_int8u len);
      void CanTransmitPacket_ExtId(blt_int8u *data, blt_int8u len);
      CanTransmitPacket_StdId(testCanData, 8);
      CanTransmitPacket_ExtId(testCanData, 8);
#endif
      
#if 0
      LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);   // RS485 - TX
      //LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);   // RS485 - RX
      void Test_FFF(blt_int8u data);
      Test_FFF(testByte++);
      if (testByte >= 0x80 ) testByte = ' ';
#endif
      
    }
} /*** end of LedBlinkTask ***/


/************************************************************************************//**
** \brief     Cleans up the LED blink driver. This is intended to be used upon program
**            exit.
** \return    none.
**
****************************************************************************************/
void LedBlinkExit(void)
{
  /* turn the LED off */
  LL_GPIO_ResetOutputPin(GPIOF, LL_GPIO_PIN_8);
} /*** end of LedBlinkExit ***/


/*********************************** end of led.c **************************************/
