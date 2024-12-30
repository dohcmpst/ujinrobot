/*
 * Parameter.c
 *
 *  Created on: 2022. 12. 28.
 *      Author: king
 */

#include "Parameter.h"
#include "Memory.h"

System_Parameter Parameter,rd_para;

void init_Parameter(void)
{
	//----------------------------------------
	Parameter.s16_SET_FLAG		=	_FRAM_SET_DATA;	// 초기 데이터 저장 여부 확인
	//
	Parameter.s16_OVP_CELL 		= 	4200; //  mV
	Parameter.s16_UVP_CELL		=	3000;//  mV
	//
	Parameter.s16_COCP			=	6500;	// 30.00 A
	Parameter.s16_DOCP			= (-7000);	//-85.00 A
	//
	Parameter.s8_OTP_charge		=	  55;	// 60;//℃
	Parameter.s8_UTP_charge		=	 (-5);	//-15;//℃
	Parameter.s8_OTP_Discharge	=	  65;	//		℃
	Parameter.s8_UTP_Discharge	=	(-20);	//		℃
	//	230727	PF 조건 추가
	//Parameter.s16_PFP_DV		=    250;	//	mV
	Parameter.s16_CPFP_CELL		=	3600;	//	mV
	Parameter.s16_CPFP_DV		=	 250;	//	mV	//230907 추가
	Parameter.s16_DPFP_CELL		=	3200;	//	mV
	Parameter.s16_DPFP_DV		=	 300;	//	mV	//230907 추가
	//----------------------------------------
	Parameter.s16_CB_AVR_V		=	3700;	//	mV	//	Cell Balancing Check Average Cell Volt
	//Parameter.s16_CB_AVR_V	=	3200;	//	mV	//	Cell Balancing Check Average Cell Volt
	Parameter.s16_CB_SDV		=	  30;	//	mV	//	Cell Balancing Check Start DV
	Parameter.s16_CB_EDV		=	  20;	//	mV	//	Cell Balancing Check Ended DV
	//----------------------------------------
	Parameter.s16_CD_AMP		=	 300;	//	3.00A
	Parameter.s16_DD_AMP		=  (-300);	// -3.00A
	//----------------------------------------
	//----------------------------------------
	Parameter.s16_CAPACITY		=	 765;	//	390;//39.0 Ah	//LB_SN: GP-M50LT-1-G206-0001(230914)-V1
	//Parameter.s16_CAPACITY	=	 280;	//					//LB_SN: GP-M50LT-1-FC27-0004(230914)-V1 (한정으로 변경 한것임)
	//----------------------------------------
	Parameter.s16_SOH_CC		= 0;
	//---------------------------------------
	Parameter.s16_PF_FLAG		= _EVENT_NORMAL;
	//---------------------------------------
	Parameter.s32_EVENT_SEC		= 0;
	//---------------------------------------
	Parameter.s16_EVENT_INDEX	= 0;
	//---------------------------------------
	Parameter.s16_RESET_INDEX	= 0;
	//---------------------------------------
}




void init_Parameter_re(void)
{
	//----------------------------------------
	Parameter.s16_SET_FLAG		=	_FRAM_SET_DATA;	// 초기 데이터 저장 여부 확인
	//
	Parameter.s16_OVP_CELL 		= 	4200; //  mV
	Parameter.s16_UVP_CELL		=	3000;//  mV
	//
	Parameter.s16_COCP			=	6500;	// 30.00 A
	Parameter.s16_DOCP			= (-7000);	//-85.00 A
	//
	Parameter.s8_OTP_charge		=	  55;	// 60;//℃
	Parameter.s8_UTP_charge		=	 (-5);	//-15;//℃
	Parameter.s8_OTP_Discharge	=	  65;	//		℃
	Parameter.s8_UTP_Discharge	=	(-20);	//		℃
	//	230727	PF 조건 추가
	//Parameter.s16_PFP_DV		=    250;	//	mV
	Parameter.s16_CPFP_CELL		=	3600;	//	mV
	Parameter.s16_CPFP_DV		=	 250;	//	mV	//230907 추가
	Parameter.s16_DPFP_CELL		=	3200;	//	mV
	Parameter.s16_DPFP_DV		=	 300;	//	mV	//230907 추가
	//----------------------------------------
	Parameter.s16_CB_AVR_V		=	3700;	//	mV	//	Cell Balancing Check Average Cell Volt
	//Parameter.s16_CB_AVR_V	=	3200;	//	mV	//	Cell Balancing Check Average Cell Volt
	Parameter.s16_CB_SDV		=	  30;	//	mV	//	Cell Balancing Check Start DV
	Parameter.s16_CB_EDV		=	  20;	//	mV	//	Cell Balancing Check Ended DV
	//----------------------------------------
	Parameter.s16_CD_AMP		=	 300;	//	3.00A
	Parameter.s16_DD_AMP		=  (-300);	// -3.00A
	//----------------------------------------
	//----------------------------------------
	Parameter.s16_CAPACITY		=	 765;	//	390;//39.0 Ah	//LB_SN: GP-M50LT-1-G206-0001(230914)-V1
	//Parameter.s16_CAPACITY	=	 280;	//					//LB_SN: GP-M50LT-1-FC27-0004(230914)-V1 (한정으로 변경 한것임)
	//----------------------------------------
//	Parameter.s16_SOH_CC		= 0;
//	//---------------------------------------
//	Parameter.s16_PF_FLAG		= _EVENT_NORMAL;
//	//---------------------------------------
//	Parameter.s32_EVENT_SEC		= 0;
//	//---------------------------------------
//	Parameter.s16_EVENT_INDEX	= 0;
//	//---------------------------------------
//	Parameter.s16_RESET_INDEX	= 0;
	//---------------------------------------
}















void init_Parameter_TEST(void) //LB요청 TEST용 코드
{
	//----------------------------------------
	Parameter.s16_SET_FLAG		=	_FRAM_SET_DATA;	// 초기 데이터 저장 여부 확인
	//
	Parameter.s16_OVP_CELL 		= 	4250; //231204 승인용 샘플용 변경 //4200(기존)	//  mV
	Parameter.s16_UVP_CELL		=	2850; //231204 승인용 샘플용 변경  //3000(기존)	//  mV
	//
	Parameter.s16_COCP			=	3000;	// 30.00 A
	Parameter.s16_DOCP			= (-10000);	//231204 승인용 샘플용 변경 //-90.00(기존) A				-80.00 A
	//
	Parameter.s8_OTP_charge		=	  55;	// 60;//℃
	Parameter.s8_UTP_charge		=	 (-5);	//-15;//℃
	Parameter.s8_OTP_Discharge	=	  65;	//		℃
	Parameter.s8_UTP_Discharge	=	(-20);	//		℃
	//	230727	PF 조건 추가
	//Parameter.s16_PFP_DV		=    250;	//	mV
	Parameter.s16_CPFP_CELL		=	3600;	//	mV
	Parameter.s16_CPFP_DV		=	 200;	//	mV	//230907 추가
	Parameter.s16_DPFP_CELL		=	3200;	//	mV
	Parameter.s16_DPFP_DV		=	 250;	//	mV	//230907 추가
	//----------------------------------------
	Parameter.s16_CB_AVR_V		=	3700;	//	mV	//	Cell Balancing Check Average Cell Volt
	//Parameter.s16_CB_AVR_V	=	3200;	//	mV	//	Cell Balancing Check Average Cell Volt
	Parameter.s16_CB_SDV		=	  30;	//	mV	//	Cell Balancing Check Start DV
	Parameter.s16_CB_EDV		=	  20;	//	mV	//	Cell Balancing Check Ended DV
	//----------------------------------------
	Parameter.s16_CD_AMP		=	 300;	//	3.00A
	Parameter.s16_DD_AMP		=  (-300);	// -3.00A
	//----------------------------------------
	//----------------------------------------
	Parameter.s16_CAPACITY		=	 380;	//	390;//39.0 Ah	//LB_SN: GP-M50LT-1-G206-0001(230914)-V1
	//Parameter.s16_CAPACITY	=	 280;	//					//LB_SN: GP-M50LT-1-FC27-0004(230914)-V1 (한정으로 변경 한것임)
	//----------------------------------------
	Parameter.s16_SOH_CC		= 0;
	//---------------------------------------
	Parameter.s16_PF_FLAG		= _EVENT_NORMAL;
	//---------------------------------------
	Parameter.s32_EVENT_SEC		= 0;
	//---------------------------------------
	Parameter.s16_EVENT_INDEX	= 0;
	//---------------------------------------
	Parameter.s16_RESET_INDEX	= 0;
	//---------------------------------------
}
