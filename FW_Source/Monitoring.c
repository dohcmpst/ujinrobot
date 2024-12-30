/*
 * Monitoring.c
 *
 *  Created on: 2023. 1. 2.
 *      Author: king
 */


#include "Monitoring.h"
#include "System_Info.h"
#include "Parameter.h"
#include "SOC.h"
#include "SOH.h"


#include "LB\LB_CAN_Protocol.h"
#include "Library\FM25V02.h"
#include "Library\CAN.h"
#include "Memory.h"
#include "Cell_Balancing.h"
#include "System_Info.h"
#include <stdio.h>
#include <string.h>

extern void CAN_recovery(void);
extern CAN_RxHeaderTypeDef RxHeader;
extern uint8_t CAN_rx_data[8];
extern uint8_t CAN_rx_flag;

boolean is_Monitoring_ON = FALSE;
static uint8_t	u8_can_frame[8];
static uint32_t	s_ticks,e_ticks;

/* Booter */
void BootActivate();
#define BOOT_COM_CAN_RX_MSG_ID	(0x7EB)


void Monitoring_CAN_Control(void)
{
	if(RxHeader.StdId == _CAN_ID_ON_MONITORING)
	{
		if(RxHeader.RTR)	{ is_Monitoring_ON = TRUE;	log_debug("\r\nCAN_O_RS485_X\r\n"); }
	}
	else if(RxHeader.StdId == _CAN_ID_OF_MONITORING)
	{
		if(RxHeader.RTR)	{ is_Monitoring_ON = FALSE;	log_debug("\r\nCAN_X_RS485_O\r\n"); }
	}
}


void Monitoring_CAN(void)
{
	if(CAN_rx_flag == TRUE)
	{	s_ticks = HAL_GetTick();

		Monitoring_CAN_Control();

		if (is_Monitoring_ON == TRUE)
		{
			switch (RxHeader.StdId)
			{
				/* Booter */
		    	case BOOT_COM_CAN_RX_MSG_ID	:   // #define BOOT_COM_CAN_RX_MSG_ID	(0x7EB)		// 0x7F8 : 응답
		    	{
		    		BootActivate();
		    	}
		    	break;
				//----------------------------------------------------
				//	MONITOR to BMS DATA READ REQUEST
				//----------------------------------------------------
				case	_CAN_ID_RD_REQ_PARA:
				{
					//---------------------------------------------------------
					//Parameter.s16_CAPACITY = 380;
					//FM25V02_wr_data( _FRAM_ADDR_CAPACITY, 2, (uint8_t *)&Parameter.s16_CAPACITY);
					//FM25V02_rd_data( _FRAM_ADDR_CAPACITY, 2, (uint8_t *)&rd_para.s16_CAPACITY);

					u8_can_frame[0] = (Parameter.s16_CAPACITY	& 0x00ff);		//	(uint8_t)(Parameter.s16_CAPACITY/10);
					u8_can_frame[1] = (Parameter.s16_CAPACITY	& 0xff00) >> 8;	//	0;

					u8_can_frame[2] = 0;
					u8_can_frame[3] = 0;
					u8_can_frame[4] = Parameter.s8_OTP_charge;
					u8_can_frame[5] = Parameter.s8_UTP_charge;
					u8_can_frame[6] = Parameter.s8_OTP_Discharge;
					u8_can_frame[7] = Parameter.s8_UTP_Discharge;
					//---------------
					Send_can_Std( u8_can_frame, _CAN_ID_RD_RES_PARA_CAP );
					HAL_Delay(0);	//CAN_recovery();
					//---------------------------------------------------------
					u8_can_frame[0] = (Parameter.s16_OVP_CELL	& 0x00ff);
					u8_can_frame[1] = (Parameter.s16_OVP_CELL	& 0xff00) >> 8;
					u8_can_frame[2] = (Parameter.s16_UVP_CELL	& 0x00ff);
					u8_can_frame[3] = (Parameter.s16_UVP_CELL	& 0xff00) >> 8;
					u8_can_frame[4] = 0;
					u8_can_frame[5] = 0;
					u8_can_frame[6] = (int8_t)(Parameter.s16_COCP / 100);
					u8_can_frame[7] = (int8_t)(Parameter.s16_DOCP / 100);
					//---------------
					Send_can_Std( u8_can_frame, _CAN_ID_RD_RES_PARA_OVP );
					HAL_Delay(0);	//CAN_recovery();
					//---------------------------------------------------------
					u8_can_frame[0] = (Parameter.s16_CPFP_CELL	& 0x00ff);
					u8_can_frame[1] = (Parameter.s16_CPFP_CELL	& 0xff00) >> 8;
					u8_can_frame[2] = (Parameter.s16_CPFP_DV	& 0x00ff);
					u8_can_frame[3] = (Parameter.s16_CPFP_DV	& 0xff00) >> 8;
					u8_can_frame[4] = (Parameter.s16_DPFP_CELL	& 0x00ff);
					u8_can_frame[5] = (Parameter.s16_DPFP_CELL	& 0xff00) >> 8;
					u8_can_frame[6] = (Parameter.s16_DPFP_DV	& 0x00FF);
					u8_can_frame[7] = (Parameter.s16_DPFP_DV	& 0xFF00) >> 8;
					//---------------
					Send_can_Std( u8_can_frame, _CAN_ID_RD_RES_PARA_CPFP );
					HAL_Delay(0);	CAN_recovery();
					//---------------------------------------------------------
				}	break;

				case _CAN_ID_RD_REQ_EVENT_DATA:			//	 0x1A0	이벤트 데이터 요구
				{
					_ERD.u16_HEAD_INDEX = (uint16_t)(CAN_rx_data[0]);
					//_ERD.u16_HEAD_INDEX	= ((CAN_rx_data[0] & 0x00FF) << 8) + (CAN_rx_data[1]);		// 요청 이벤트 시작 데이터 번호 0~19
					//_ERD.u16_TAIL_INDEX	= ((CAN_rx_data[2] & 0x00FF) << 8) + (CAN_rx_data[3]);		// 미사용
					_ERD.u16_HEAD_INDEX %= _FRAM_EVENT_TOTOL_RECORD_SIZE;
					//_ERD.u16_TAIL_INDEX %= _FRAM_EVENT_TOTOL_RECORD_SIZE;							// 미사용
					//---------------------------------------------------------
					//Send_can_Ext( u8_can_frame, _CAN_ID_RD_RES_RESET_INDEX );
					//HAL_Delay(0);	CAN_recovery();
					//-------------------------------------------------------------------------------
					//	FRAM EVENT DATA LOADING 21 개의 Record Read(발생전10개/발생1개/발생후10개)
					_ERD.u16_AFTER_INDEX = _FRAM_ADDR_EVENT_START + (_ERD.u16_HEAD_INDEX * sizeof(EVENT_EMPTY_DATA) );
					FM25V02_rd_data(_ERD.u16_AFTER_INDEX, sizeof(EVENT_LOAD_DATA),  (uint8_t *)&EVENT_LOAD_DATA);
					//
					_ERD.u16_AFTER_COUNT = _EVENT_NORMAL;				//	이벤트 전송 인텍스 0 설정, 전송하는 레코드의 수(발생전10개/발생1개/발생후10개)
					_ERD.u16_ISSUE_FLAG	 = _EVENT_FLAGS_ISSUE_DATA;		//	이벤트 전송 플레그 1 설정
					//	밑에서 이벤트 데이터 전송 처리,
					//log_debug(" %d = EVENT_DATA_LOAD \r\n", _ERD.u16_HEAD_INDEX);
				}	break;

				//----------------------------------------------------
				//	FRAM DATA MEMORY WRITE
				//----------------------------------------------------
				//	MONITOR to BMS DATA WRITE REQUEST
				case	_CAN_ID_WR_REQ_PARA_CAP:		//	0x141
				{

					Parameter.s16_CAPACITY		= CAN_rx_data[0] + (CAN_rx_data[1] << 8);	//(int16_t)(CAN_rx_data[0] * 10);
					Parameter.s8_OTP_charge 	= (int8_t)CAN_rx_data[4];
					Parameter.s8_UTP_charge 	= (int8_t)CAN_rx_data[5];
					Parameter.s8_OTP_Discharge 	= (int8_t)CAN_rx_data[6];
					Parameter.s8_UTP_Discharge 	= (int8_t)CAN_rx_data[7];
					//---------------------------------------------------------------------------
					FM25V02_wr_data( _FRAM_ADDR_CAPACITY, 2, (uint8_t *)&Parameter.s16_CAPACITY);
					FM25V02_rd_data( _FRAM_ADDR_CAPACITY, 2, (uint8_t *)&rd_para.s16_CAPACITY);
					//---------------------------------------------------------------------------
					HAL_Delay(0);
					FM25V02_wr_data( _FRAM_ADDR_COTP, 4, (uint8_t *)&Parameter.s8_OTP_charge);
					FM25V02_rd_data( _FRAM_ADDR_COTP, 4, (uint8_t *)&rd_para.s8_OTP_charge);
					//---------------------------------------------------------------------------
				}	break;

				case	_CAN_ID_WR_REQ_PARA_OVP:		//	0x142
				{
					Parameter.s16_OVP_CELL		= CAN_rx_data[0] + (CAN_rx_data[1] << 8);
					Parameter.s16_UVP_CELL		= CAN_rx_data[2] + (CAN_rx_data[3] << 8);
					Parameter.s16_COCP			= (int16_t)(CAN_rx_data[6] * 100);
					Parameter.s16_DOCP			= (int16_t)((int8_t)CAN_rx_data[7] * 100);
					//----------------------------------------------------------------------
					FM25V02_wr_data( _FRAM_ADDR_OVP, 8, (uint8_t *)&Parameter.s16_OVP_CELL);
					FM25V02_rd_data( _FRAM_ADDR_OVP, 8, (uint8_t *)&rd_para.s16_OVP_CELL);
				}	break;

				case	_CAN_ID_WR_REQ_PARA_CPFP:		//	0x143
				{
					Parameter.s16_CPFP_CELL		= CAN_rx_data[0]  + (CAN_rx_data[1] << 8);
					Parameter.s16_CPFP_DV		= CAN_rx_data[2]  + (CAN_rx_data[3] << 8);
					Parameter.s16_DPFP_CELL		= CAN_rx_data[4]  + (CAN_rx_data[5] << 8);
					Parameter.s16_DPFP_DV		= CAN_rx_data[6]  + (CAN_rx_data[7] << 8);
					//------------------------------------------------------------------------------
					FM25V02_wr_data( _FRAM_ADDR_CPFP_MAX_V, 8, (uint8_t *)&Parameter.s16_CPFP_CELL);
					FM25V02_rd_data( _FRAM_ADDR_CPFP_MAX_V, 8, (uint8_t *)&rd_para.s16_CPFP_CELL);
				}	break;

				case	_CAN_ID_WR_REQ_PF_CLEAR:		//	0x121
				{	//	PF CLEAR BMS POWER RESET 후 적용
					Parameter.s16_PF_FLAG		= _EVENT_NORMAL;
					//-----------------------------------------------------------------------
					FM25V02_wr_data( _FRAM_ADDR_PF_FLAG, 2, (uint8_t *)&Parameter.s16_PF_FLAG);
					FM25V02_rd_data( _FRAM_ADDR_PF_FLAG, 2, (uint8_t *)&rd_para.s16_PF_FLAG);
				}	break;
			}
		}

		e_ticks = HAL_GetTick();
		log_debug("CAN_StdID=%lx:T_Real(%lu ms)\r\n", RxHeader.StdId, (e_ticks - s_ticks) );		//	11 ms
		//log_debug("CAN_rx_flag::StdID=%lx:Task_RealTime(ms):%lu\r\n", RxHeader.StdId, (e_ticks - s_ticks) );		//	11 ms
		CAN_rx_flag = FALSE;
	}

	else if(_ERD.u16_ISSUE_FLAG	== _EVENT_FLAGS_ISSUE_DATA)
	{	// 이벤트 데이터 전송 처리(전송 요청이 있을 경우 데이터 전송)
		//_ERD.u16_AFTER_COUNT	= _EVENT_NORMAL;			//	이벤트 전송 인텍스 0 설정, 전송하는 레코드의 수(발생전10개/발생1개/발생후10개)
		//_ERD.u16_ISSUE_FLAG	= _EVENT_FLAGS_ISSUE_DATA;	//	이벤트 전송 플레그 1 설정
		//s_ticks = HAL_GetTick();

		if (is_Monitoring_ON == TRUE)
		{
			{	//	0x1A1
				u8_can_frame[0] = (uint8_t)(_ERD.u16_HEAD_INDEX	& 0x00ff);
				u8_can_frame[1] = 0;	//	(_ERD.u16_HEAD_INDEX	& 0xff00) >> 8;		//	요청 이벤트 번호		0~19
				u8_can_frame[2] = (uint8_t)_ERD.u16_AFTER_COUNT;						//	Record 번호 		0~20
				u8_can_frame[3] = 0;
				u8_can_frame[4] = 0;
				u8_can_frame[5] = 0;
				u8_can_frame[6] = 0;
				u8_can_frame[7] = 0;
				//---------------------------------------------------------------------------------
				Send_can_Std( u8_can_frame, _CAN_ID_RD_RES_EVENT_DATA1);
				HAL_Delay(0);	//CAN_recovery();
			}
			//--------------------------------------------------------------------------------------------
			{	//	0x1A2
				u8_can_frame[0] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u32_SEC	& 0x000000ff);		//	BMS 동작 시간 초
				u8_can_frame[1] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u32_SEC	& 0x0000ff00) >> 8;	//
				u8_can_frame[2] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u32_SEC	& 0x00ff0000) >> 16;	//
				u8_can_frame[3] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u32_SEC	& 0xff000000) >> 24;	//
				u8_can_frame[4] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_CODE	& 0x00ff);			//	이벤트 코드
				u8_can_frame[5] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_CODE	& 0xff00) >> 8;		//
				u8_can_frame[6] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_PAC_V	& 0x00ff);			//	최대 셀 전압
				u8_can_frame[7] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_PAC_V	& 0xff00) >> 8;		//
				//---------------------------------------------------------------------------------
				Send_can_Std( u8_can_frame, _CAN_ID_RD_RES_EVENT_DATA2 );
				HAL_Delay(0);	//CAN_recovery();
			}
			//---------------------------------------------------------------------------------------------
			{	//	0x1A3
				u8_can_frame[0] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_MAX_V	& 0x00ff);			//	최소 셀 전압
				u8_can_frame[1] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_MAX_V	& 0xff00) >> 8;		//
				u8_can_frame[2] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_MIN_V	& 0x00ff);			//	Pack 전압
				u8_can_frame[3] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].u16_MIN_V	& 0xff00) >> 8;		//
				u8_can_frame[4] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].s16_PAC_I	& 0x00ff);			//	Pack 전류
				u8_can_frame[5] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].s16_PAC_I	& 0xff00) >> 8;		//
				u8_can_frame[6] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].s8_MAX_T	& 0x00ff);			//	최대 온도
				u8_can_frame[7] = (EVENT_LOAD_DATA[_ERD.u16_AFTER_COUNT].s8_MIN_T	& 0x00ff);		//	최소 온도
				//---------------------------------------------------------------------------------
				Send_can_Std( u8_can_frame, _CAN_ID_RD_RES_EVENT_DATA3 );
				HAL_Delay(0);	CAN_recovery();
				//log_debug("EVENT_DATA COUNT = %d \r\n",_ERD.u16_AFTER_COUNT);
			}
			if(++_ERD.u16_AFTER_COUNT >= _FRAM_EVENT_TOTOL_RECORD_SIZE)
			{	//	21개 레코드 체크
				_ERD.u16_ISSUE_FLAG	= _EVENT_FLAGS_NOMAL_DATA;		//	전송 종료
				log_debug("E_DATA END=%d,%d\r\n",_ERD.u16_HEAD_INDEX, _ERD.u16_AFTER_COUNT);
			}
		}
		else
		{
			_ERD.u16_ISSUE_FLAG	= _EVENT_FLAGS_NOMAL_DATA;			//	전송 중단
			log_debug("E_DATA EXIT=%d,%d\r\n",_ERD.u16_HEAD_INDEX, _ERD.u16_AFTER_COUNT);
		}
		//e_ticks = HAL_GetTick();	log_debug(" _EVENT_FLAGS_ISSUE_DATA::Task_RealTime Time(ms): %lu \r\n", e_ticks - s_ticks );		//	11 ms
	}

}



void canif_monitor_data_send(void)
{
	//uint32_t	s_ticks, e_ticks;
	uint32_t	SendID;
	s_ticks = HAL_GetTick();

	//---------------------------------------------------------------------------------------------
	{	//	0x100	FW_Version, SerialNum, Yera month day, Cycle Count
		u8_can_frame[0] = System_Info.Manufacture.u8_FW_Version;
		u8_can_frame[1] = System_Info.Manufacture.u8_Serial_Number;
		u8_can_frame[2] = (System_Info.Manufacture.u16_Manufacture_Year & 0x00ff);
		u8_can_frame[3] = (System_Info.Manufacture.u16_Manufacture_Year & 0xff00) >> 8;
		u8_can_frame[4] = System_Info.Manufacture.u8_Manufacture_Month;
		u8_can_frame[5] = System_Info.Manufacture.u8_Manufacture_Day;
		u8_can_frame[6] = (System_Info.Lifecycle.u16_Cycle_Count & 0x00ff);
		u8_can_frame[7] = (System_Info.Lifecycle.u16_Cycle_Count & 0xff00) >> 8;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_INFOR );		//
		HAL_Delay(0);	// CAN_recovery();
	}
	//---------------------------------------------------------------------------------------------
	{	//	0x101	SOC, SOH
		u8_can_frame[0] = (System_Info.Lifecycle.u16_SOC & 0x00ff);
		u8_can_frame[1] = (System_Info.Lifecycle.u16_SOC & 0xff00) >> 8;
		u8_can_frame[2] = (System_Info.Lifecycle.u16_SOH & 0x00ff);
		u8_can_frame[3] = (System_Info.Lifecycle.u16_SOH & 0xff00) >> 8;
		u8_can_frame[4] = 0xEE;//( System_Info.Lifecycle.u16_SOP & 0x00ff );
		u8_can_frame[5] = 0x02;//( System_Info.Lifecycle.u16_SOP & 0xff00 ) >> 8;
		u8_can_frame[6] = 0;
		u8_can_frame[7] = 0;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_SOC_SOH );		//
		HAL_Delay(0);	// CAN_recovery();
	}
	//-------------------------------------------------------------------------------------
	{	//	0x102	Pack Current / Voltage
		//System_Info.Electric.s32_Sum_of_1sec_Current = -2;
		u8_can_frame[0] = ((int16_t)System_Info.Electric.s32_Sum_of_1sec_Current	& 0x00ff );
		u8_can_frame[1] = ((int16_t)System_Info.Electric.s32_Sum_of_1sec_Current	& 0xff00 ) >> 8;
		u8_can_frame[2] = ( System_Info.Electric.u16_Pack_In_Voltage	& 0x00ff );
		u8_can_frame[3] = ( System_Info.Electric.u16_Pack_In_Voltage	& 0xff00 ) >> 8;
		u8_can_frame[4] = ( System_Info.Electric.u16_Pack_Out_Voltage	& 0x00ff );
		u8_can_frame[5] = ( System_Info.Electric.u16_Pack_Out_Voltage	& 0xff00 ) >> 8;
		u8_can_frame[6] = 0;
		u8_can_frame[7] = 0;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_PACK_V_I );		//
		HAL_Delay(0);	// CAN_recovery();
	}
	//-------------------------------------------------------------------------------------------
	{	//	0x103	Fault		System_Info.Status.u16_BMS_status
		u8_can_frame[0] = (System_Info.Fault.u16_Fault & 0x00ff);
		u8_can_frame[1] = (System_Info.Fault.u16_Fault & 0xff00) >> 8;
		u8_can_frame[2] = 0;	//(System_Info.Alarm.u16_Alarm & 0x00ff);
		u8_can_frame[3] = 0;	//(System_Info.Alarm.u16_Alarm & 0xff00) >> 8;
		u8_can_frame[4] = (System_Info.Status.u16_BMS_status & 0x00ff);
		u8_can_frame[5] = (System_Info.Status.u16_BMS_status & 0xff00) >> 8;
		u8_can_frame[6] = 0;	//System_Info.Status.DFET + ( System_Info.Status.CFET << 1);
		u8_can_frame[7] = 0;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_FAULT );		//
		HAL_Delay(0);	// CAN_recovery();
	}
	//-------------------------------------------------------------------------------------------
	{	//	0x104	Balancing
		u8_can_frame[0] = (System_Info.Balancing.u32_Balancing_Cell	& 0x000000FF);			//
		u8_can_frame[1] = (System_Info.Balancing.u32_Balancing_Cell	& 0x0000FF00) >> 8;		//
		u8_can_frame[2] = (System_Info.Balancing.u32_Balancing_Cell	& 0x00FF0000) >> 16;	//
		u8_can_frame[3] = (System_Info.Balancing.u32_Balancing_Cell	& 0xFF000000) >> 24;	//
		u8_can_frame[4] = 0;	//( (FCC * 100 ) & 0x00ff );
		u8_can_frame[5] = 0;	//( (FCC * 100 ) & 0xff00 ) >> 8;
		u8_can_frame[6] = 0;	//( (RC * 100 ) & 0x00ff );
		u8_can_frame[7] = 0;	//( (RC * 100 ) & 0xff00 ) >> 8;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_BALANCE );		//
		HAL_Delay(0);	// CAN_recovery();
	}
	//----------------------------------------------------------------------------------
	{	//	0x105 ~ 0x109	BANK CELL Voltage Data TX
		SendID = _CAN_ID_BMS_DA_V01_04;
		for(int i=0; i<20; )			//i++)
		{
			u8_can_frame[0] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i] 	& 0x00ff);				//	mV
			u8_can_frame[1] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i]		& 0xff00) >> 8;			//	mV
			i++;
			u8_can_frame[2] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i]		& 0x00ff);				//	mV
			u8_can_frame[3] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i]		& 0xff00) >> 8;			//	mV
			i++;
			u8_can_frame[4] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i]		& 0x00ff);				//	mV
			u8_can_frame[5] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i]		& 0xff00) >> 8;			//	mV
			i++;
			u8_can_frame[6] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i]		& 0x00FF);				//	mV
			u8_can_frame[7] = (System_Info.Cell_Voltage.u16_Cell_Voltage[i]		& 0xFF00) >> 8;			//	mV
			i++;
			Send_can_Std( u8_can_frame, SendID );
			HAL_Delay(0);	// CAN_recovery();
			SendID++;
		}
	}
	//-------------------------------------------------------------------------------------
	{	//	0x10D		MIN_V, MAX_V,
		u8_can_frame[0] = ( System_Info.Cell_Voltage.u16_Min_Cell_Voltage		& 0x00ff);
		u8_can_frame[1] = ( System_Info.Cell_Voltage.u16_Min_Cell_Voltage		& 0xff00) >>8;
		u8_can_frame[2] = ( System_Info.Cell_Voltage.u16_Min_Cell_Voltage_Num	& 0x00ff);
		u8_can_frame[3] = ( System_Info.Cell_Voltage.u16_Min_Cell_Voltage_Num	& 0xff00) >>8;
		u8_can_frame[4] = ( System_Info.Cell_Voltage.u16_Max_Cell_Voltage		& 0x00ff);
		u8_can_frame[5] = ( System_Info.Cell_Voltage.u16_Max_Cell_Voltage		& 0xff00) >>8;
		u8_can_frame[6] = ( System_Info.Cell_Voltage.u16_Max_Cell_Voltage_Num	& 0x00ff);
		u8_can_frame[7] = ( System_Info.Cell_Voltage.u16_Max_Cell_Voltage_Num	& 0xff00) >>8;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_MIN_MAX_V );
		HAL_Delay(0);	// CAN_recovery();
	}
	//-------------------------------------------------------------------------------------------
	{	//	0x10E		AVG_V, Diff_V, AVG_T, DIFF_T
		u8_can_frame[0] = ( System_Info.Cell_Voltage.u16_Avg_Cell_Voltage 		& 0x00ff);
		u8_can_frame[1] = ( System_Info.Cell_Voltage.u16_Avg_Cell_Voltage 		& 0xff00) >> 8;
		u8_can_frame[2] = ( System_Info.Cell_Voltage.u16_Diff_Cell_Voltage 		& 0x00ff);
		u8_can_frame[3] = ( System_Info.Cell_Voltage.u16_Diff_Cell_Voltage 		& 0xff00) >> 8;
		u8_can_frame[4] = ((System_Info.Cell_Temp.s16_Avg_Cell_Temperature*10) 	& 0x00ff);
		u8_can_frame[5] = ((System_Info.Cell_Temp.s16_Avg_Cell_Temperature*10) 	& 0xff00) >> 8;
		u8_can_frame[6] = ((System_Info.Cell_Temp.s16_Diff_Cell_Temperature*10) & 0x00ff);
		u8_can_frame[7] = ((System_Info.Cell_Temp.s16_Diff_Cell_Temperature*10) & 0xff00) >> 8;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_AVR_DIF_V );
		HAL_Delay(0);	// CAN_recovery();
	}
	//-------------------------------------------------------------------------------------
	{	//	0x10F		//	Temperature 4
		u8_can_frame[0] = ((System_Info.Cell_Temp.s16_Cell_Temperature[0] *10)	& 0x00ff);			//
		u8_can_frame[1] = ((System_Info.Cell_Temp.s16_Cell_Temperature[0] *10)	& 0xff00) >> 8;		//
		u8_can_frame[2] = ((System_Info.Cell_Temp.s16_Cell_Temperature[1] *10)	& 0x00ff);			//
		u8_can_frame[3] = ((System_Info.Cell_Temp.s16_Cell_Temperature[1] *10)	& 0xff00) >> 8;		//
		u8_can_frame[4] = ((System_Info.Cell_Temp.s16_Cell_Temperature[2] *10)	& 0x00ff);			//
		u8_can_frame[5] = ((System_Info.Cell_Temp.s16_Cell_Temperature[2] *10)	& 0xff00) >> 8;		//
		u8_can_frame[6] = ((System_Info.Cell_Temp.s16_Cell_Temperature[3] *10)	& 0x00FF);			//
		u8_can_frame[7] = ((System_Info.Cell_Temp.s16_Cell_Temperature[3] *10)	& 0xFF00) >> 8;		//
		//---------------------------------------------------------------------------------
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_T01_04 );					//	TEMP 4개
		HAL_Delay(0);	// CAN_recovery();
	}
	//------------------------------------------------------------------------------------------
	{	//	0x118		// Temperature MIN_T, NIN_T_NUM, MAX_T, MAX_T_NUM
		u8_can_frame[0] = ((System_Info.Cell_Temp.s16_Min_Cell_Temperature *10) & 0x00ff);
		u8_can_frame[1] = ((System_Info.Cell_Temp.s16_Min_Cell_Temperature *10) & 0xff00) >> 8;
		u8_can_frame[2] = ( System_Info.Cell_Temp.u16_Min_Cell_Temperature_Num 	& 0x00ff);
		u8_can_frame[3] = ( System_Info.Cell_Temp.u16_Min_Cell_Temperature_Num 	& 0xff00) >> 8;
		u8_can_frame[4] = ((System_Info.Cell_Temp.s16_Max_Cell_Temperature *10) & 0x00ff);
		u8_can_frame[5] = ((System_Info.Cell_Temp.s16_Max_Cell_Temperature *10) & 0xff00) >> 8;
		u8_can_frame[6] = ( System_Info.Cell_Temp.u16_Max_Cell_Temperature_Num	& 0x00ff);
		u8_can_frame[7] = ( System_Info.Cell_Temp.u16_Max_Cell_Temperature_Num	& 0xff00) >> 8;
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_TEMP );					//	TEMP 4개
		HAL_Delay(0);	//CAN_recovery();
	}
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//-------------------------------------------------------------------------------------
	//	BMS STATE IDLE, CHARGE, DISCHARGE, FAULT, BALANCE DCC bit
	{	//	0x119
		u8_can_frame[0] = (System_Info.Status.Charge_Status			& 0x00ff);				//
		u8_can_frame[1] = (System_Info.Status.Charge_Status			& 0xff00) >> 8;			//
		u8_can_frame[2] = (System_Info.Fault.u16_Fault				& 0x00ff);				//
		u8_can_frame[3] = (System_Info.Fault.u16_Fault				& 0xff00) >> 8;			//
		u8_can_frame[4] = (System_Info.Balancing.u32_Balancing_Cell	& 0x000000ff);			//
		u8_can_frame[5] = (System_Info.Balancing.u32_Balancing_Cell	& 0x0000ff00) >> 8;		//
		u8_can_frame[6] = (System_Info.Balancing.u32_Balancing_Cell	& 0x00FF0000) >> 16;	//
		u8_can_frame[7] = (System_Info.Balancing.u32_Balancing_Cell	& 0xFF000000) >> 24;	//
		//---------------------------------------------------------------------------------
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_STATE );		//	SOC, SOH
		HAL_Delay(0);	// CAN_recovery();
	}
	//-------------------------------------------------------------------------------------
	//	EVENT SEC, EVENT_INDEX
	{	//	0x11A
		u8_can_frame[0] = (Parameter.s32_EVENT_SEC				& 0x000000ff);		//
		u8_can_frame[1] = (Parameter.s32_EVENT_SEC				& 0x0000ff00) >> 8;	//
		u8_can_frame[2] = (Parameter.s32_EVENT_SEC				& 0x00ff0000) >> 16;	//
		u8_can_frame[3] = (Parameter.s32_EVENT_SEC				& 0xff000000) >> 24;	//
		u8_can_frame[4] = (Parameter.s16_EVENT_INDEX			& 0x00ff);			//
		u8_can_frame[5] = (Parameter.s16_EVENT_INDEX			& 0xff00) >> 8;		//
		u8_can_frame[6] = 0;												//
		u8_can_frame[7] = 0;	//
		//---------------------------------------------------------------------------------
		Send_can_Std( u8_can_frame, _CAN_ID_BMS_DA_EVENT );					// SOC, SOH
		HAL_Delay(0);	CAN_recovery();
	}
	//-------------------------------------------------------------------------------------
	//e_ticks = HAL_GetTick();	log_debug("can_1sec(%lu ms)\r\n", e_ticks - s_ticks );	//log_debug("canif_send(): Task_1sec(ms):%lu \r\n", e_ticks - s_ticks );	// 16ms
}

//--------------------------------------------------------------------------------------------------
//	BootLoader



//--------------------------------------------------------------------------------------------------
