/*
 * Parameter.h
 *
 *  Created on: 2022. 12. 28.
 *      Author: king
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "main.h"
#pragma pack(push,1)
typedef struct
{
	int16_t		s16_SET_FLAG;			//	2  0(0x0000)	0x55AA
	int16_t		s16_OVP_CELL;			//	2  2(0x0002)	OVP			Charge Over Voltage Protection
	int16_t		s16_UVP_CELL;			//	2  4(0x0004)	UVP			DisCharge Under Voltage Protection
	int16_t		s16_COCP;				//	2  6(0x0006)	COCP		Charge Over Current Protection
	int16_t		s16_DOCP;				//	2  8(0x0008)	DOCP		DisCharge Over Current Protection
	int8_t		s8_OTP_charge;			//	1 10(0x000A)	COTP		Charge Over Temperature Protection
	int8_t		s8_UTP_charge;			//	1 11(0x000B)	CUTP		Charge Under Temperature Protection
	int8_t		s8_OTP_Discharge;		//	1 12(0x000C)	DOTP		DisCharge Over Temperature Protection
	int8_t		s8_UTP_Discharge;		//	1 13(0x000D)	DUTP		DisCharge Under Temperature Protection
	int16_t		s16_CPFP_CELL;			//	2 14(0x000E)	CPFP_MAX_V	Charge PF Protection Maximum Voltage
	int16_t		s16_CPFP_DV;			//	2 16(0x0010)	CPFP_DV		Charge PF Different Voltage
	int16_t		s16_DPFP_CELL;			//	2 18(0x0012)	DPFP_MIN_V	DisCharge PF Protection Minimum Voltage
	int16_t		s16_DPFP_DV;			//	2 20(0x0014)	DPFP_DV		DisCharge PF Different Voltage
	int16_t		s16_CB_AVR_V;			//	2 22(0x0016)	CB_AVR_V	Cell Balancing AVR Voltage
	int16_t		s16_CB_SDV;				//	2 24(0x0018)	CB_SDV		Cell Balancing Start Different Voltage
	int16_t		s16_CB_EDV;				//	2 26(0x001A)	CB_EDV		Cell Balancing Ended Different Voltage
	int16_t		s16_CD_AMP;				//	2 28(0x001C)	CD_AMP		Charge Detect Current
	int16_t		s16_DD_AMP;				//	2 30(0x001E)	DD_AMP		DisCharge Detect Current
	int16_t		s16_CAPACITY;			//	2 32(0x0020)	CAPACITY
	int16_t		s16_SOH_CC;				//	2 34(0x0022)	SOH_CC		SOH Cycle Counter
	int16_t		s16_PF_FLAG;			//	2 36(0x0024)	PF_FLAG		PF Flag
	int32_t		s32_EVENT_SEC;			//	4 38(0x0026)	EVENT_SEC	BMS Active Run SEC
	int16_t		s16_EVENT_INDEX;		//	2 42(0x002A)	EVENT_INDEX
	int16_t		s16_RESET_INDEX;		//	2 44(0x002C)	RESET_INDEX
} System_Parameter;
#pragma pack(pop)
	//------------------------------------12 NOT USED
	//int8_t	s8_OTR_charge;			//	1				COTPR		Charge Over Temperature Protection Release
	//int8_t	s8_UTR_charge;			//	1				CUTPR		Charge Under Temperature Protection Release
	//int8_t	s8_OTR_Discharge;		//	1				DOTPR		DisCharge Over Temperature Protection Release
	//int8_t	s8_UTR_Discharge;		//	1				DUTPR		DisCharge Under Temperature Protection Release
	//------------------------------------

extern System_Parameter Parameter, rd_para ;

extern void init_Parameter(void);
extern void init_Parameter_re(void);
extern void init_Parameter_TEST(void);

#endif /* PARAMETER_H_ */
