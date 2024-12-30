/*
 * System_Info.h
 *
 *  Created on: 2022. 12. 23.
 *      Author: king
 */

#ifndef SYSTEM_INFO_H_
#define SYSTEM_INFO_H_

#include "main.h"

//fault1
#define f_OVP				0x0001	//230907	//0x01
#define f_UVP				0x0002				//0x02
#define f_OCP				0x0004				//0x04
#define	f_DOCP				0x0008				//0x08
#define f_COTP				0x0010				//0x10
#define f_DOTP				0x0020				//0x20
#define f_CUTP				0x0040				//0x40
#define	f_DUTP				0x0080				//0x80
//--------------------------------
//	230906
#define	f_CPF				0x0100			//
#define	f_DPF				0x0200			//
//--------------------------------

typedef enum
{
	_STATUS_INIT					= 0x0000,
	_STATUS_IGNITE_KEY_SW			= 0x0100,
	_STATUS_CHARGE_KEY				= 0x0200,
	_STATUS_SOC_DISPLAY				= 0x0400,
	_STATUS_DISCHARGE				= 0x0800,
	_STATUS_CHARGE					= 0x1000,
	_STATUS_DISCHARGE_FET			= 0x2000,
	_STATUS_CHARGE_FET				= 0x4000,
	_STATUS_PRECHARGE_FET			= 0x8000,
}	_SYSTEM_STATUS_BitState;

typedef struct
{
	uint8_t 	u8_FW_Version;
	uint8_t 	u8_Serial_Number;
	uint16_t 	u16_Manufacture_Year;
	uint8_t 	u8_Manufacture_Month;
	uint8_t 	u8_Manufacture_Day;
}Manufacture_Info;

typedef struct
{
	uint16_t		u16_Cycle_Count;
	uint16_t		u16_SOC;
	uint16_t		u16_SOH;
	uint16_t		u16_SOP;
	uint8_t			u8_SOC;
	uint8_t			u8_SOH;
	uint8_t			u8_SOP;
	int				s32_now_c;
	int 			s32_max_c;

}Lifecycle_Info;

typedef struct
{
	int16_t			s16_Pack_Current;	//resolution 0.01 (EX :3A -> 300)
	uint32_t		u32_Pack_Voltage;

	int				s32_Sum_of_1sec_Current;

	uint16_t		u16_Pack_In_Voltage; //충전시 Pack 전압(프로토콜용)
	uint16_t		u16_Pack_Out_Voltage;//방전시 Pack 전압(프로토콜용)
}Electric_Info;

#if(1)
//230907	CPF,DPF 추가
typedef struct
{
	uint32_t		u32_Fault;	//
	uint16_t		u16_Fault;  //	아래 Fault들의 OR합산. Self_Diagnose, 통신 용도
	boolean			OVP;		//0x0001	(Cell Over Voltage Protection)
	boolean			UVP;		//0x0002	(Cell Under Voltage Protection)
	boolean			COCP;		//0x0004	(Charge Over Current Protection)
	boolean			DOCP;		//0x0008	(Discharge Over Current Protection)
	boolean			COTP;		//0x0010	(Charge Over Temperature Protection)
	boolean			DOTP;		//0x0020	(Discharge Over Temperature Protection)
	boolean			CUTP;		//0x0040	(Charge Under Temperature Protection)
	boolean			DUTP;		//0x0080	(Discharge Under Temperature Protection)
	boolean			CPF;		//0x0100	(Charge PF)
	boolean			DPF;		//0x0200	(DisCharge PF)
	boolean			SHTP;		//0x0400	(Short Protection)
	boolean			PWDN;		//0x0800	(Power Down)
	boolean			IMBP;		//0x1000	(Imbalance Protection)
	boolean			PUVP;		//0x2000	(Permanent Under Voltage Protection)
	boolean			FOTP;		//0x4000	(FET Over Temp Protection)
}Fault_Info;

#else
typedef struct
{
	uint16_t		u16_Fault;  //	아래 Fault들의 OR합산. Self_Diagnose, 통신 용도
	boolean			OVP;		//0x0001	(Cell Over Voltage Protection)
	boolean			UVP;		//0x0002	(Cell Under Voltage Protection)
	boolean			COCP;		//0x0004	(Charge Over Current Protection)
	boolean			DOCP;		//0x0008	(Discharge Over Current Protection)
	boolean			COTP;		//0x0010	(Charge Over Temperature Protection)
	boolean			DOTP;		//0x0020	(Discharge Over Temperature Protection)
	boolean			CUTP;		//0x0040	(Charge Under Temperature Protection)
	boolean			DUTP;		//0x0080	(Discharge Under Temperature Protection)
	boolean			SHTP;		//0x0100	(Short Protection)
	boolean			PWDN;		//0x0200	(Power Down)
	boolean			IMBP;		//0x0400	(Imbalance Protection)
	boolean			PUVP;		//0x0800	(Permanent Under Voltage Protection)
	boolean			FOTP;		//0x1000	(FET Over Temp Protection)
}Fault_Info;
#endif


typedef struct
{
	uint16_t		u16_Alarm;	//아래 Alarm들의 OR합산. Self_Diagnose, 통신 용도
	boolean			OVA;		//(Cell Over Voltage Alarm)
	boolean			UVA;		//(Cell Under Voltage Alarm)
	boolean			COCA;		//(Charge Over Current Alarm)
	boolean			DOCA;		//(Discharge Over Current Alarm)
	boolean			COTA;		//(Charge Over Temperature Alarm)
	boolean			DOTA;		//(Discharge Over Temperature Alarm)
	boolean			CUTA;		//(Charge Under Temperature Alarm)
	boolean			DUTA;		//(Discharge Under Temperature Alarm)
}Alarm_Info;

typedef struct
{
	uint16_t		u16_BMS_status;
	int				Charge_Status;
	boolean			SW; 		//(Ignite Key)
	boolean 		CKEY;		//(Charging KEY)
	boolean			DSOC;		//(SOC Display)
	boolean			IDCH;		//(Discharging)
	boolean 		ICHG; 		//(Charging)
	boolean			DFET; 		//(Discharge FET)
	boolean			CFET;		//(Charge FET)
	boolean			PFET; 		//(Predischarge FET)
}BMS_Status_Info;

typedef struct
{
	uint32_t		u32_Balancing_Cell;		//몇번 셀 밸런싱중인지
	uint8_t			is_Cell_Balancing_ON;	//230907

}Balancing_Info;

typedef struct
{
	//나중에 Parameter.c, Parameter.h 만들어서 가져올 것.
}Parameter_Info;

typedef struct
{
	uint16_t		u16_Cell_Voltage[20];
	uint16_t		u16_Cell_Voltage_200ms[20]; //ltc 200ms 마다 측정하기 위한 변수
	uint16_t		u16_Min_Cell_Voltage;
	uint16_t		u16_Max_Cell_Voltage;
	uint16_t		u16_Min_Cell_Voltage_Num;
	uint16_t		u16_Max_Cell_Voltage_Num;
	uint16_t		u16_Avg_Cell_Voltage;
	uint16_t		u16_Diff_Cell_Voltage;
}Cell_Voltage_Info;

typedef struct
{
	int16_t			s16_Cell_Temperature[8];
	int16_t			s16_Min_Cell_Temperature;
	int16_t			s16_Max_Cell_Temperature;
	uint16_t		u16_Min_Cell_Temperature_Num;
	uint16_t		u16_Max_Cell_Temperature_Num;
	int16_t			s16_Avg_Cell_Temperature;
	int16_t			s16_Diff_Cell_Temperature;
}Cell_Temperature_Info;

typedef struct
{
	Manufacture_Info 		Manufacture;
	Lifecycle_Info			Lifecycle;
	Electric_Info			Electric;
	Fault_Info				Fault;
	Alarm_Info				Alarm;
	BMS_Status_Info			Status;
	Balancing_Info			Balancing;
	Parameter_Info			Parameter;
	Cell_Voltage_Info		Cell_Voltage;
	Cell_Temperature_Info	Cell_Temp;
}System_Information;

extern System_Information System_Info;

#endif /* SYSTEM_INFO_H_ */
