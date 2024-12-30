/*
 * LB_Main.c
 *
 *  Created on: 2023. 2. 3.
 *      Author: king
 */

#include <stdio.h>
#include "LB_Main.h"

#include "../Parameter.h"

#include "../System_Info.h"

#if(0)
#define LB_Driving_Test		TRUE //LB 주행 시험 용도, 39Ah 모델이 주행시험에서 Fail 나와서 주행 시험 목적으로 수정 용
#else
#define LB_Driving_Test		FALSE
#endif

//추후 이동 or System_Info에서 include 요망
char u8_FW_Version				=23;    //Ver2.2 10단위 이하는 임시로 소수점 이렇게 표시함
char u8_Serial_Number			=0x01; //1번 Pack 추후 시리얼 규정에 맞춰 수정요망
uint16_t u16_Manufacture_Year	=2024;
uint8_t u8_Manufacture_Month	=7;
uint8_t u8_Manufacture_Day		=8;

void init_LB(void)
{
	if (LB_Driving_Test == TRUE)
	{
		log_debug("It is only for LB Driving Test\r\n");
		//log_debug("It`s OVP Setting Value is 4300\r\n ");
		//log_debug("And Capacity is 38.6\r\n");
		Parameter.s16_OVP_CELL = 4300;
		Parameter.s16_UVP_CELL = 2900;
		Parameter.s16_CAPACITY = 386; //{시험중}
	}
	else
	{
		log_debug("It is LB Driving Normal \r\n");
	}
	System_Info.Manufacture.u8_FW_Version			= u8_FW_Version; //0.01version
	System_Info.Manufacture.u8_Serial_Number		= u8_Serial_Number; //1번 Pack 추후 시리얼 규정에 맞춰 수정요망
	System_Info.Manufacture.u16_Manufacture_Year	= u16_Manufacture_Year;
	System_Info.Manufacture.u8_Manufacture_Month	= u8_Manufacture_Month;
	System_Info.Manufacture.u8_Manufacture_Day		= u8_Manufacture_Day;

	//---------------------------------------------------------------------------------------------------------
	log_debug("OVP=%.3f, UVP=%.3f, COCP=%.2f, DOCP=%.3f, COTP=%4d, CUTP=%4d, DOTP=%4d, DUTP=%4d, CAP=%.1f \r\n",
		 (float)Parameter.s16_OVP_CELL/1000,	// 	= 	4300;	//  4.200	V
		 (float)Parameter.s16_UVP_CELL/1000,	//	=	2900;	//  3.000	V
		 (float)Parameter.s16_COCP/100,			//	=	3000;	// 30.0		A
		 (float)Parameter.s16_DOCP/100,			//	= (-8000);	//-80.0		A
				Parameter.s8_OTP_charge,		//	=	  60;	//			℃
				Parameter.s8_UTP_charge,		//	=	(-15);	//			℃
				Parameter.s8_OTP_Discharge,		//	=	  65;	//			℃
				Parameter.s8_UTP_Discharge,		//	=	(-20);	//			℃
		 (float)Parameter.s16_CAPACITY/10);		//	=	 386;	// 38.6
	//---------------------------------------------------------------------------------------------------------
}
