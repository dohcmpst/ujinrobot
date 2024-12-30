/*
 * LB_CAN_Protocol.h
 *
 *  Created on: Jan 5, 2023
 *      Author: king
 */
/*
 *   LB CAN PROTOCOL
 *  1. BaudRate : 250 Kbps
 *	2. ID : Standard CAN ID, Extended CAN ID
 *	3. Method : Big Endian
 *  4. DLC : 8
 */
#ifndef LB_CAN_PROTOCOL_H_
#define LB_CAN_PROTOCOL_H_

#include "main.h"
#define BMS_Pack_Data1 				0x100 // period 1000ms
#define BMS_Pack_Data2 				0x101 // period  100ms
#define BMS_Pack_Data3 				0x102 // period  100ms

#define BMS_Flag_Data1 				0x103 // period  100ms
#define BMS_Flag_Data2 				0x104 // period  100ms

#define BMS_Cell_Voltage_Data1  	0x105 // period 1000ms
#define BMS_Cell_Voltage_Data2 		0x106 // period 1000ms
#define BMS_Cell_Voltage_Data3  	0x107 // period 1000ms
#define BMS_Cell_Voltage_Data4  	0x108 // period 1000ms
#define BMS_Cell_Voltage_Data5  	0x109 // period 1000ms
#define BMS_Cell_Voltage_Data6  	0x10D // period  100ms
#define BMS_Cell_Voltage_Data7  	0x10E // period  100ms

#define BMS_Temperature_Data1		0x10F // period 1000ms
#define BMS_Temperature_Data2		0x110 // period 1000ms
//BMS_Temperature_Data3 없음 프로토콜 상 X
#define BMS_Temperature_Data4		0x118 // period  100ms

//Charging_State
#define Battery_state_for_Charger	0x2C0 // period  100ms
//用于仪表的电池状态???
#define Battery_state_for_VCU		0x120 // period  100ms

typedef	enum
{
	//----------------------------------------------------
	//	29 Bit Extended ID
	//----------------------------------------------------
	_CAN_ID_START					= 0x00000000,
	//----------------------------------------------------
	_CAN_ID_RD_RES_VCU_SOC			= 0x10261051,
	//----------------------------------------------------
	//	MONITORING BMS DATA RESPONSE(TX)
	//				   -------------
	//---------------------------------------------------------------
	_CAN_ID_BMS_DA_INFOR			= 0x100,
	_CAN_ID_BMS_DA_SOC_SOH			= 0x101,
	_CAN_ID_BMS_DA_PACK_V_I			= 0x102,
	_CAN_ID_BMS_DA_FAULT			= 0x103,
	_CAN_ID_BMS_DA_BALANCE			= 0x104,
	//---------------------------------------------------------------
	_CAN_ID_BMS_DA_V01_04			= 0x105,		//	Cell V 01~04
	_CAN_ID_BMS_DA_V05_08			= 0x106,		//	Cell V 05~08
	_CAN_ID_BMS_DA_V09_12			= 0x107,		//	Cell V 09~12
	_CAN_ID_BMS_DA_V13_16			= 0x108,		//	Cell V 13~16
	_CAN_ID_BMS_DA_V17_20			= 0x109,		//	Cell V 17~20
	//---------------------------------------------------------------
	_CAN_ID_BMS_DA_MIN_MAX_V		= 0x10D,		//	MIN_V, MAX_V
	_CAN_ID_BMS_DA_AVR_DIF_V		= 0x10E,		//	AVR_V, DIFF_V
	_CAN_ID_BMS_DA_T01_04			= 0x10F,		//	= 0x1ED00008,		//	TEMP 4
	_CAN_ID_BMS_DA_T05_08			= 0x110,		//	= 0x1ED00008,		//	TEMP 4
	_CAN_ID_BMS_DA_TEMP				= 0x118,
	_CAN_ID_BMS_DA_STATE			= 0x119,		//	= 0x1ED0000A,		//	BMS_STATE_IDLE_CHARGE_DISCHARGE, FAULT, BALANCING DCC BIT
	_CAN_ID_BMS_DA_EVENT			= 0x11A,		//	= 0x1ED0000B,		//	EVENT_SEC, EVENT_INDEX
	//_CAN_ID_BMS_DA_PV_PI,			//	= 0x1ED00007,		//	PACK_V, PACK_I,
	//----------------------------------------------------
	//	EVENT DATA
	//----------------------------------------------------
	//	MONITOR to BMS PARAMETER DATA READ REQUEST(RX)
	//				   ---------------------------
	_CAN_ID_RD_REQ_PARA				= 0x120,		//	ALL PARAMETER READ
	_CAN_ID_WR_REQ_PF_CLEAR			= 0x121,		//	PF CLEAR WRITE
	//----------------------------------------------------
	//	MONITOR to BMS PARAMETR DATA READ RESPONSE(TX)
	//----------------------------------------------------
	_CAN_ID_RD_RES_PARA_CAP			= 0x131,
	_CAN_ID_RD_RES_PARA_OVP			= 0x132,
	_CAN_ID_RD_RES_PARA_CPFP		= 0x133,
	//_CAN_ID_RD_RES_CB_AVR_V		= 0x134,
	//_CAN_ID_RD_RES_CD_AMP			= 0x135,
	//_CAN_ID_RD_RES_CAPACITY		= 0x136,
	//_CAN_ID_RD_RES_SOH_CC			= 0x137,
	//_CAN_ID_RD_RES_PF_FLAG		= 0x138,
	//_CAN_ID_RD_RES_EVENT_SEC		= 0x139,
	//_CAN_ID_RD_RES_EVENT_INDEX	= 0x13A,
	//_CAN_ID_RD_RES_RESET_INDEX	= 0x13B,
	//----------------------------------------------------
	//	MONITOR to BMS PARAMETER DATA WRITE REQUEST(RX)
	//----------------------------------------------------
	_CAN_ID_WR_REQ_PARA_CAP			= 0x141,
	_CAN_ID_WR_REQ_PARA_OVP			= 0x142,
	_CAN_ID_WR_REQ_PARA_CPFP		= 0x143,
	//_CAN_ID_WR_REQ_CB_AVR_V		= 0x144,
	//_CAN_ID_WR_REQ_CD_AMP			= 0x145,
	//_CAN_ID_WR_REQ_CAPACITY		= 0x146,
	//_CAN_ID_WR_REQ_SOH_CC			= 0x147,
	//_CAN_ID_WR_REQ_PF_FLAG		= 0x148,
	//_CAN_ID_WR_REQ_EVENT_SEC		= 0x149,
	//_CAN_ID_WR_REQ_EVENT_INDEX	= 0x14A,
	//_CAN_ID_WR_REQ_RESET_INDEX	= 0x14B,
	//----------------------------------------------------
	//------------------------------
	_CAN_ID_RD_REQ_EVENT_DATA		= 0x1A0,
	_CAN_ID_RD_RES_EVENT_DATA1		= 0x1A1,
	_CAN_ID_RD_RES_EVENT_DATA2		= 0x1A2,
	_CAN_ID_RD_RES_EVENT_DATA3		= 0x1A3,
	//----------------------------------------------------
	_CAN_ID_OF_MONITORING			= 0x1FE,
	_CAN_ID_ON_MONITORING			= 0x1FF,
	_CAN_ID_END						= 0x1FF,
	//----------------------------------------------------
} _CAN_EXT_ID_INFOR;

#define VCU_x5_Old		0x014

#define VCU_x5_New		0x10261051
#define VCU_x10_New		0x10261051

#pragma pack(push,1)
typedef struct
{
	uint32_t	ID_SEND_TYPE;	// 4
	uint32_t	ID_RES_TYPE;	// 4
	uint16_t	CODE;		// 2
	uint16_t	MAX_V;		// 2
	uint16_t	MIN_V;		// 2
	uint16_t	PAC_V;		// 2
	int16_t		SEC_I;		// 2
	int8_t		MAX_T;		// 1
	int8_t		MIN_T;		// 1
}	_CAN_DATA;			// 16
#pragma pack(pop)


extern void Send_CAN_100ms();



#endif /* LB_CAN_PROTOCOL_H_ */
