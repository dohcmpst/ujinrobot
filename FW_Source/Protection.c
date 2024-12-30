/*
 * Protection.c
 *
 *  Created on: 2022. 12. 28.
 *      Author: king
 */

#include "Protection.h"
#include "System_Info.h"
#include "Parameter.h"

#include "Library\FM25V02.h"
#include "Memory.h"

#include <stdio.h>

static uint16_t tp_fault = 0;	//230907	//uint8_t
static uint16_t cp_fault = 0;
static uint16_t vp_fault = 0;
uint16_t	pf_fault = 0;	//230907

static uint16_t vp_ovp_delay = 0;
static uint16_t vp_uvp_delay = 0;

static uint16_t cp_cocp_delay = 0;
static uint16_t cp_docp_delay = 0;

static uint16_t tp_otp_delay = 0;
static uint16_t tp_utp_delay = 0;
static uint16_t tp_dotp_delay = 0;
static uint16_t tp_dutp_delay = 0;
//-----------------------------------------------
//	230907
static uint16_t	cpf_delay = 0;	//	CPF delay
static uint16_t	dpf_delay = 0;	//	DPF delay
//------------------------------
extern int pf_process_enable;
//-----------------------------------------------
extern int tp_process_enable;
extern int cp_process_enable;
extern int vp_process_enable;

void vp_process(void)
{
	if(vp_process_enable == 1)
	{
		if(vp_fault == 0)
		{
			//-----------
			// OVP	Check
			if(System_Info.Cell_Voltage.u16_Max_Cell_Voltage > Parameter.s16_OVP_CELL)		//	MAX_V > 4200 mV
			{
				if(++vp_ovp_delay > 3)														//	> 3 sec
				{
					vp_fault |= f_OVP;		System_Info.Fault.OVP = SET;	//log_debug("#### OVP ####\r\n");
					//---------------------------------------
					_EWR.u16_ISSUE_STATE	= _EVENT_OVP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
					//---------------------------------------
				}
			}
			else	vp_ovp_delay = 0;
			//-----------
			// UVP	Check
			if(System_Info.Cell_Voltage.u16_Min_Cell_Voltage < Parameter.s16_UVP_CELL)		//	MIN_V < 3000 mV
			{
				if(++vp_uvp_delay > 3)														//	> 3 sec
				{
					vp_fault |= f_UVP;		System_Info.Fault.UVP = SET;	//log_debug("#### UVP ####\r\n");
					//---------------------------------------
					_EWR.u16_ISSUE_STATE	= _EVENT_UVP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
					//---------------------------------------
				}
			}
			else	vp_uvp_delay = 0;
		}
	}
	System_Info.Fault.u16_Fault |= vp_fault;
}

void cp_process(void)
{
	if( cp_process_enable == 1 )
	{
		if(cp_fault == 0)
		{
			//-----------
			// COCP Check
			if(System_Info.Status.Charge_Status == CHARGE)									//	Charging > 3 Sec
			{
				if(System_Info.Electric.s32_Sum_of_1sec_Current > Parameter.s16_COCP)		//	SEC_A	> 30 A
				{
					if(++cp_cocp_delay > 3)													//	> 3 Sec
					{
						cp_fault |= f_OCP;		System_Info.Fault.COCP = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_COCP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
					}
				}
				else	cp_cocp_delay = 0;
			}
			//-------------
			// DOCP Check
			else if(System_Info.Status.Charge_Status == DISCHARGE)							//	DisCharging > 3 Sec
			{
				if(System_Info.Electric.s32_Sum_of_1sec_Current < Parameter.s16_DOCP)		//	SEC_A	> -80 A
				{
					if(++cp_docp_delay > 3)													//	> 3 Sec
					{
						cp_fault |= f_DOCP;		System_Info.Fault.DOCP = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_DOCP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
					}
				}
				else	cp_docp_delay = 0;
			}
		}
	}
	System_Info.Fault.u16_Fault |= cp_fault;
	//return;
}

void tp_process(void)
{
	if(tp_process_enable == 1)
	{
		if(tp_fault == 0)
		{
			//	충전중
			if(System_Info.Status.Charge_Status == CHARGE)											//	Charging	> 3 Sec
			{
				//---------------
				//	COTP Check
				if(System_Info.Cell_Temp.s16_Max_Cell_Temperature > Parameter.s8_OTP_charge)		//	MAX_T > 55 도
				{
					if(++tp_otp_delay > 3)															//	> 3 Sec
					{
						tp_fault |= f_COTP;		System_Info.Fault.COTP = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_COTP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
					}
				}
				else	tp_otp_delay = 0;
				//------------
				//	CUTP Check
				if(System_Info.Cell_Temp.s16_Min_Cell_Temperature < Parameter.s8_UTP_charge)		//	MIN_T < -5 도
				{
					if(++tp_utp_delay > 3)															//	> 3 Sec
					{
						tp_fault |= f_CUTP;		System_Info.Fault.CUTP = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_CUTP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
					}
				}
				else	tp_utp_delay = 0;
			}
			//	방전중
			else if(System_Info.Status.Charge_Status == DISCHARGE)									//	DisCharging	> 3 Sec
			{
				//------------
				//	DOTP Check
				if(System_Info.Cell_Temp.s16_Max_Cell_Temperature > Parameter.s8_OTP_Discharge )	//	MAX_T > 65
				{
					if(++tp_dotp_delay> 3)															//	> 3 Sec
					{
						tp_fault |= f_DOTP;		System_Info.Fault.DOTP = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_DOTP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
					}
				}
				else	tp_dotp_delay = 0;
				//--------------
				//	DUTP Check
				if(System_Info.Cell_Temp.s16_Min_Cell_Temperature < Parameter.s8_UTP_Discharge)		//	MIN_T < -20 도
				{
					if(++tp_dutp_delay > 3)															//	> 3 Sec
					{
						tp_fault |= f_DUTP;		System_Info.Fault.DUTP = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_DUTP;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
					}
				}
				else	tp_dutp_delay = 0;
			}
		}
	}

	System_Info.Fault.u16_Fault |= tp_fault;
}

void pf_process(void)
{
	if(pf_process_enable)
	{
		if(pf_fault == 0)
		{
			//-----------
			//	CPF Check
			if(System_Info.Status.Charge_Status == CHARGE)										//	Charging > 3 Sec
			{
				if((System_Info.Cell_Voltage.u16_Max_Cell_Voltage	> Parameter.s16_CPFP_CELL)	//	MAX_V	> 3600 mV &&
				&& (System_Info.Cell_Voltage.u16_Diff_Cell_Voltage	> Parameter.s16_CPFP_DV))	//	CPF_DV	>  200 mV
				{
					if(++cpf_delay > 3)															//	> 3 Sec
					{
						pf_fault |= f_CPF;		System_Info.Fault.CPF = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_CPF;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
						//	EVENT CPF DATA WRITE
						Parameter.s16_PF_FLAG = _EVENT_CPF;
						FM25V02_wr_data( _FRAM_ADDR_PF_FLAG, sizeof(Parameter.s16_PF_FLAG), (uint8_t *)&Parameter.s16_PF_FLAG);
						//--------------------------------------------
					}
				}
				else	cpf_delay = 0;
			}
			//-----------
			//	DPF Check
			if(System_Info.Status.Charge_Status == DISCHARGE)									//	DisCharging > 3 Sec
			{
				if((System_Info.Cell_Voltage.u16_Min_Cell_Voltage	> Parameter.s16_DPFP_CELL)	//	MIN_V	> 3200 mV &&
				&& (System_Info.Cell_Voltage.u16_Diff_Cell_Voltage	> Parameter.s16_DPFP_DV))	//	DPF_DV	>  250 mV
				{
					if(++dpf_delay > 3)															//	> 3 Sec
					{
						pf_fault |= f_DPF;		System_Info.Fault.DPF = SET;
						//--------------------------------------------
						_EWR.u16_ISSUE_STATE	= _EVENT_DPF;		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
						//--------------------------------------------
						//	EVENT CPF DATA WRITE
						Parameter.s16_PF_FLAG = _EVENT_DPF;
						FM25V02_wr_data( _FRAM_ADDR_PF_FLAG, sizeof(Parameter.s16_PF_FLAG), (uint8_t *)&Parameter.s16_PF_FLAG);
						//--------------------------------------------
					}
				}
				else	dpf_delay = 0;
			}
		}
	}
	System_Info.Fault.u16_Fault |= pf_fault;
}

int Auto_Release_vp_delay=0;
int Auto_Release_cp_delay=0;
int Auto_Release_tp_delay=0;
int Auto_Release_enable=1;

void Auto_Release(void)
{
	//if(Auto_Release_enable)
	{
		if(vp_fault != 0) Auto_Release_vp(); else {}
		if(cp_fault != 0) Auto_Release_cp(); else {}
		if(tp_fault != 0) Auto_Release_tp(); else {}
	}
}

void Auto_Release_vp(void)
{
	if(vp_fault == f_OVP)
	{
		//------------
		//	OVPR Check
		if ( System_Info.Cell_Voltage.u16_Max_Cell_Voltage < Parameter.s16_OVP_CELL - 50)		//	MAX_V < (OVP-50 :: 4150mV)
		{
			if (++Auto_Release_vp_delay > 3)													//	> 3 Sec
			{
				vp_fault &= !(f_OVP);
				System_Info.Fault.OVP = RESET;
				System_Info.Fault.u16_Fault &= vp_fault;
				System_Info.Status.u16_BMS_status |= _STATUS_CHARGE_FET;
				//------------------------------------------------------------
				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
				log_debug("######### CHG PIN HIGH -f_OVP Clear ###########\r\n");
				//------------------------------------------------------------
				_EWR.u16_ISSUE_STATE = (_EVENT_OVP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
				//------------------------------------------------------------
			}
		}
		else	Auto_Release_vp_delay = 0;

	}
	else if (vp_fault == f_UVP)
	{
		//------------
		//	UVPR Check
		if ( System_Info.Cell_Voltage.u16_Min_Cell_Voltage > Parameter.s16_UVP_CELL + 50)		//	MIN_V > (UVP+50 :: 3050mV)
		{
			if (++Auto_Release_vp_delay > 3)													//	> 3 Sec
			{
				vp_fault &= !(f_UVP);
				System_Info.Fault.UVP = RESET;
				System_Info.Fault.u16_Fault &= vp_fault;
				System_Info.Status.u16_BMS_status |= _STATUS_DISCHARGE_FET;

				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
				log_debug("######### DSG PIN HIGH -f_UVP Clear ###########\r\n");
				//------------------------------------------------------------
				_EWR.u16_ISSUE_STATE = (_EVENT_UVP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
				//------------------------------------------------------------
			}
		}
		else	Auto_Release_vp_delay = 0;
	}
}

void Auto_Release_cp(void)
{
	if (cp_fault == f_OCP)
	{
		//-------------
		//	COCPR Check
		if(System_Info.Status.Charge_Status == DISCHARGE)	//	DisCharging	> 3 Sec	(  SEC_A < -3A, > 3 Sec)
		{
			cp_fault &= !(f_OCP);
			System_Info.Fault.COCP = RESET;
			System_Info.Fault.u16_Fault &= cp_fault;
			System_Info.Status.u16_BMS_status |= _STATUS_CHARGE_FET;
			//------------------------------------------------------------
			HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
			log_debug("######### CHG PIN HIGH -f_OCP Clear ###########\r\n");
			//------------------------------------------------------------
			_EWR.u16_ISSUE_STATE = (_EVENT_COCP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
			//------------------------------------------------------------
		}
	}
	else if (cp_fault == f_DOCP)
	{
		//-------------
		//	DOCPR Check
		if(System_Info.Status.Charge_Status == CHARGE)		//	Charging > 3 Sec	(  SEC_A > 3A, > 3 Sec)
		{
			cp_fault &= !(f_DOCP);
			System_Info.Fault.DOCP = RESET;
			System_Info.Fault.u16_Fault &= cp_fault;
			System_Info.Status.u16_BMS_status |= _STATUS_DISCHARGE_FET;

			HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
			log_debug("######### DSG PIN HIGH -f_DOCP Clear ###########\r\n");
			//------------------------------------------------------------
			_EWR.u16_ISSUE_STATE = (_EVENT_DOCP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
			//------------------------------------------------------------
		}
	}
}

void Auto_Release_tp(void)
{
	if(tp_fault == f_COTP)
	{
		//-------------
		//	COTPR Check
		if(System_Info.Cell_Temp.s16_Max_Cell_Temperature < Parameter.s8_OTP_charge - 5)	//	MAX_T < (COTP-5 = 50)
		{
			if(++Auto_Release_tp_delay > 3)													//	> 3 Sec
			{
				tp_fault &= !(f_COTP);
				System_Info.Fault.COTP = RESET;
				System_Info.Fault.u16_Fault &= tp_fault;
				System_Info.Status.u16_BMS_status |= _STATUS_CHARGE_FET;
				//-------------------------------------------------------------
				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
				log_debug("######### CHG PIN HIGH -f_COTP Clear ###########\r\n");
				//-------------------------------------------------------------
				_EWR.u16_ISSUE_STATE = (_EVENT_COTP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
				//------------------------------------------------------------
			}
		}
		else	Auto_Release_tp_delay = 0;
	}
	else if (tp_fault == f_DOTP)
	{
		//-------------
		//	DOTPR Check
		if (System_Info.Cell_Temp.s16_Max_Cell_Temperature < Parameter.s8_OTP_Discharge - 5)	//	MAX_T < (DOTP-5 = 60)
		{
			if (++Auto_Release_tp_delay > 3)													//	> 3 Sec
			{
				tp_fault &= !(f_DOTP);
				System_Info.Fault.DOTP = RESET;
				System_Info.Fault.u16_Fault &= tp_fault;
				System_Info.Status.u16_BMS_status |= _STATUS_DISCHARGE_FET;
				//-------------------------------------------------------------
				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
				log_debug("######### DSG PIN HIGH -f_DOTP Clear ###########\r\n");
				//------------------------------------------------------------
				_EWR.u16_ISSUE_STATE = (_EVENT_DOTP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
				//------------------------------------------------------------
			}
		}
		else	Auto_Release_tp_delay = 0;
	}
	else if (tp_fault == f_CUTP)
	{
		//-------------
		//	CUTPR Check
		if (System_Info.Cell_Temp.s16_Min_Cell_Temperature > Parameter.s8_UTP_charge + 5)		//	MIN_T > (CUTP+5 = 0)
		{
			if (++Auto_Release_tp_delay > 3)													//	> 3 Sec
			{
				tp_fault &= !(f_CUTP);
				System_Info.Fault.CUTP = RESET;
				System_Info.Fault.u16_Fault &= tp_fault;
				System_Info.Status.u16_BMS_status |= _STATUS_CHARGE_FET;

				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
				log_debug("######### CHG PIN HIGH -f_CUTP Clear ###########\r\n");
				//------------------------------------------------------------
				_EWR.u16_ISSUE_STATE = (_EVENT_CUTP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
				//------------------------------------------------------------
			}
		}
		else	Auto_Release_tp_delay = 0;
	}
	else if (tp_fault == f_DUTP)
	{
		//-------------
		//	DUTPR Check
		if (System_Info.Cell_Temp.s16_Min_Cell_Temperature > Parameter.s8_UTP_Discharge + 5)	//	MIN_T > (DUTP+5 = -15)
		{
			if (++Auto_Release_tp_delay > 3)													//	> 3 Sec
			{
				tp_fault &= !(f_DUTP);
				System_Info.Fault.DUTP = RESET;
				System_Info.Fault.u16_Fault &= tp_fault;
				System_Info.Status.u16_BMS_status |= _STATUS_DISCHARGE_FET;
				//------------------------------------------------------------
				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
				log_debug("######### DSG PIN HIGH -f_DUTP Clear ###########\r\n");
				//------------------------------------------------------------
				_EWR.u16_ISSUE_STATE = (_EVENT_DUTP + _EVENT_RELEASE);		//	Memory.c	void EVENT_1SEC_DATA_MOVE(void)
				//------------------------------------------------------------
			}
		}
		else	Auto_Release_tp_delay = 0;
	}
}

#if(0)

void Auto_Release_cp_ORJ(void)
{
	if (cp_fault == f_OCP)
	{
		if (System_Info.Electric.s16_Pack_Current < -300) // -3A이상 방전
		{
			Auto_Release_cp_delay++;
			if (Auto_Release_cp_delay > 3)
			{
				cp_fault &= !(f_OCP);
				System_Info.Fault.COCP = RESET;
				System_Info.Fault.u16_Fault &= cp_fault;
				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
			}
		}
		else
		{
			Auto_Release_cp_delay = 0;
		}
	}
	else if (cp_fault == f_DOCP)
	{
		if (System_Info.Electric.s16_Pack_Current > 300) // 3A이상 충전
		{
			Auto_Release_cp_delay++;
			if (Auto_Release_cp_delay > 3)
			{
				cp_fault &= !(f_DOCP);
				System_Info.Fault.DOCP = RESET;
				System_Info.Fault.u16_Fault &= cp_fault;
				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
			}
		}
		else
		{
			Auto_Release_cp_delay = 0;
		}
	}
	//return ;
}

void Auto_Release_tp_ORJ(void)
{
	if(tp_fault == f_COTP)
	{
		if(System_Info.Cell_Temp.s16_Max_Cell_Temperature
				< Parameter.s8_OTR_charge - 5) // 경고 온도 이하로 내려가면,
		{
			Auto_Release_tp_delay++;
			if(Auto_Release_tp_delay>3)
			{
				tp_fault &= !(f_COTP);
				System_Info.Fault.COTP = RESET;
				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
			}
		}
		else
		{
			Auto_Release_tp_delay = 0;
		}
	}
	else if (tp_fault == f_DOTP)
	{
		if (System_Info.Cell_Temp.s16_Max_Cell_Temperature
				< Parameter.s8_OTR_Discharge - 5) // 경고 온도 이하로 내려가면,
		{
			Auto_Release_tp_delay++;
			if (Auto_Release_tp_delay > 3)
			{
				tp_fault &= !(f_DOTP);
				System_Info.Fault.DOTP = RESET;
				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
			}
		}
		else
		{
			Auto_Release_tp_delay = 0;
		}
	}
	else if (tp_fault == f_CUTP)
	{
		if (System_Info.Cell_Temp.s16_Max_Cell_Temperature
				> Parameter.s8_UTR_charge + 5) // 경고 온도 이상으로 올라가면,
		{
			Auto_Release_tp_delay++;
			if (Auto_Release_tp_delay > 3)
			{
				tp_fault &= !(f_CUTP);
				System_Info.Fault.CUTP = RESET;
				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
			}
		}
		else
		{
			Auto_Release_tp_delay = 0;
		}
	}
	else if (tp_fault == f_DUTP)
	{
		if (System_Info.Cell_Temp.s16_Max_Cell_Temperature
				> Parameter.s8_UTR_Discharge + 5) // 경고 온도 이상으로 올라가면,
		{
			Auto_Release_tp_delay++;
			if (Auto_Release_tp_delay > 3)
			{
				tp_fault &= !(f_DUTP);
				System_Info.Fault.DUTP = RESET;
				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
			}
		}
		else
		{
			Auto_Release_tp_delay = 0;
		}
	}
	//return ;
}

void Auto_Release_ORG(void)
{
	if(Auto_Release_enable == 1)
	{
		Auto_Release_vp();
		Auto_Release_cp();
		Auto_Release_tp();
	}
	//return ;
}

void Auto_Release_vp_ORG(void)
{
	if (vp_fault != 0)
	{
		if (vp_fault == f_OVP)
		{
			if ( System_Info.Cell_Voltage.u16_Max_Cell_Voltage
					< Parameter.s16_OVP_CELL - 50)
			{
				Auto_Release_vp_delay++;
				if (Auto_Release_vp_delay > 3)
				{
					vp_fault &= !(f_OVP);
					System_Info.Fault.OVP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_vp_delay = 0;
			}
		}
		else if (vp_fault == f_UVP)
		{
			if ( System_Info.Cell_Voltage.u16_Min_Cell_Voltage
					> Parameter.s16_UVP_CELL + 50)
			{
				Auto_Release_vp_delay++;
				if (Auto_Release_vp_delay > 3)
				{
					vp_fault &= !(f_UVP);
					System_Info.Fault.UVP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_vp_delay = 0;
			}
		}
	}
	//return ;
}

void Auto_Release_cp_ORG(void)
{
	if (cp_fault != 0)
	{
		if (cp_fault == f_OCP)
		{
			if (System_Info.Electric.s32_Sum_of_1sec_Current < -300) // -3A이상 방전
			{
				Auto_Release_cp_delay++;
				if (Auto_Release_cp_delay > 3)
				{
					cp_fault &= !(f_OCP);
					System_Info.Fault.COCP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_cp_delay = 0;
			}
		}
		else if (cp_fault == f_DOCP)
		{
			if (System_Info.Electric.s32_Sum_of_1sec_Current > 300) // 3A이상 충전
			{
				Auto_Release_cp_delay++;
				if (Auto_Release_cp_delay > 3)
				{
					cp_fault &= !(f_DOCP);
					System_Info.Fault.DOCP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_cp_delay = 0;
			}
		}
	}
	//return ;
}

void Auto_Release_tp_ORG(void)
{
	if(tp_fault != 0)
	{
		if(tp_fault == f_COTP)
		{
			if(System_Info.Cell_Temp.s16_Max_Cell_Temperature < Parameter.s8_OTR_charge) // 경고 온도 이하로 내려가면,
			{
				Auto_Release_tp_delay++;
				if(Auto_Release_tp_delay>3)
				{
					tp_fault &= !(f_COTP);
					System_Info.Fault.COTP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_tp_delay = 0;
			}
		}
		else if (tp_fault == f_DOTP)
		{
			if (System_Info.Cell_Temp.s16_Max_Cell_Temperature < Parameter.s8_OTR_Discharge) // 경고 온도 이하로 내려가면,
			{
				Auto_Release_tp_delay++;
				if (Auto_Release_tp_delay > 3)
				{
					tp_fault &= !(f_DOTP);
					System_Info.Fault.DOTP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_tp_delay = 0;
			}
		}
		else if (tp_fault == f_CUTP)
		{
			if (System_Info.Cell_Temp.s16_Max_Cell_Temperature > Parameter.s8_UTR_charge) // 경고 온도 이상으로 올라가면,
			{
				Auto_Release_tp_delay++;
				if (Auto_Release_tp_delay > 3)
				{
					tp_fault &= !(f_CUTP);
					System_Info.Fault.CUTP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_tp_delay = 0;
			}
		}
		else if (tp_fault == f_DUTP)
		{
			if (System_Info.Cell_Temp.s16_Max_Cell_Temperature > Parameter.s8_UTR_Discharge) // 경고 온도 이상으로 올라가면,
			{
				Auto_Release_tp_delay++;
				if (Auto_Release_tp_delay > 3)
				{
					tp_fault &= !(f_DUTP);
					System_Info.Fault.DUTP = RESET;
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
				}
			}
			else
			{
				Auto_Release_tp_delay = 0;
			}
		}
	}
	//return ;
}

#endif
