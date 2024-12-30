/*
 * Memory.c
 *
 *  Created on: 2022. 12. 28.
 *      Author: king
 */

#include "Library\FM25V02.h"
#include "Memory.h"
//#include "SOC.h"
#include "System_Info.h"

#include "Parameter.h"
#include <stdio.h>
#include <string.h>

//_EVENT_DATA		EWR_DATA;		//	미사용
_EVENT_DATA			ERD_DATA;		//	FRAM data write 이후 memcmp() 함수 확인을 위한 READ 변수
_EVENT_DATA			EVENT_LOAD_DATA[_FRAM_EVENT_TOTOL_RECORD_SIZE];		//	외부 FRAM 메모리 21개 레코드 DATA LOAD 데이터 저장 변수
_EVENT_DATA			EVENT_EMPTY_DATA[_FRAM_EVENT_TOTOL_RECORD_SIZE];	//	외부 FRAM 메모리 21개 레코드 DATA CLEAR 데이터 0 변수

_EVENT_ARRAY_DATA	_ERD;			//	EVENT DATA 읽기 내부 RAM 변수(레코드는 영역은 사용하지 않음) EVENT_LOAD_DATA 변수와 같이 사용
_EVENT_ARRAY_DATA	_EWR;			//	EVENT DATA 저장 내부 RAM 변수(제어 및 레코드 변수)

//extern variable
extern SPI_HandleTypeDef hspi3;
extern uint16_t	pf_fault;
//---------------------------------------------------------------------------------
void FM25V02_PARAMETER_CHECK(void);
void EVENT_1SEC_DATA_MOVE(void);
void EVENT_DATA_SAVE_CHECK(void);
//---------------------------------------------------------------------------------
void FM25V02_PARAMETER_CHECK(void)
{
#if(1)
	FM25V02_rd_data( _FRAM_ADDR_EVENT_SEC, sizeof(Parameter.s32_EVENT_SEC), (uint8_t *)&Parameter.s32_EVENT_SEC);
	FM25V02_rd_data( _FRAM_ADDR_EVENT_INDEX, sizeof(Parameter.s16_EVENT_INDEX), (uint8_t *)&Parameter.s16_EVENT_INDEX);
	log_debug("E_SEC=%ld, E_INDEX=%d \r\n",Parameter.s32_EVENT_SEC,Parameter.s16_EVENT_INDEX);
#endif

#if(0)
	log_debug("_EVENT_DATA      Size = %d \r\n", sizeof(_EVENT_DATA));
	log_debug("System_Parameter Size = %d \r\n", sizeof(System_Parameter));
	log_debug("Parameter Size        = %d \r\n", sizeof(Parameter));
#endif

	FM25V02_rd_data( _FRAM_ADDR_PARAMETER, sizeof(System_Parameter), (uint8_t *)&Parameter);
	if(Parameter.s16_SET_FLAG != _FRAM_SET_DATA)
	{
		//log_debug("Parameter DATA INIT WRITE \r\n");
		init_Parameter();
		FM25V02_wr_data( _FRAM_ADDR_PARAMETER, sizeof(System_Parameter), (uint8_t *)&Parameter );
	}
	else
	{	init_Parameter_re();
		if(Parameter.s16_PF_FLAG == _EVENT_CPF)
		{	System_Info.Fault.u16_Fault = _EVENT_CPF;	System_Info.Fault.CPF = SET;	pf_fault |= f_CPF;	}
		if(Parameter.s16_PF_FLAG == _EVENT_DPF)
		{ 	System_Info.Fault.u16_Fault = _EVENT_DPF;	System_Info.Fault.DPF = SET;	pf_fault |= f_DPF;	}
	}

#if(1)
	//System_Parameter RD_prar;
	//FM25V02_rd_data( _FRAM_ADDR_PARAMETER, sizeof(System_Parameter), (uint8_t *)&RD_prar);
	log_debug("Prameter : 0x%04x, %d,%d, %d,%d, %d,%d,%d,%d, %d,%d,%d,%d, %d,%d,%d, %d,%d, %d,%d, 0x%04x, %ld, %d, %d \r\n",
			//----------------------------------------
			Parameter.s16_SET_FLAG,		//=	_FRAM_SET_DATA;	// 초기 데이터 저장 여부 확인
			//
			Parameter.s16_OVP_CELL, 	//= 4200;	//  mV
			Parameter.s16_UVP_CELL,		//=	3000;	//  mV
			//
			Parameter.s16_COCP,			//=	3000;	// 30.00 A
			Parameter.s16_DOCP,			//= (-9000);	//-90.00 A
			//
			Parameter.s8_OTP_charge,	//=	  55;	// 60;//℃
			Parameter.s8_UTP_charge,	//=	 (-5);	//-15;//℃
			Parameter.s8_OTP_Discharge,	//=	  65;	//		℃
			Parameter.s8_UTP_Discharge,	//=	(-20);	//		℃
			//	230727	PF 조건 추가
			//Parameter.s16_PFP_DV		=    250;	//	mV
			Parameter.s16_CPFP_CELL,	//=	3600;	//	mV
			Parameter.s16_CPFP_DV,		//=	 200;	//	mV	//230907 추가
			Parameter.s16_DPFP_CELL,	//=	3200;	//	mV
			Parameter.s16_DPFP_DV,		//=	 250;	//	mV	//230907 추가
			//----------------------------------------
			Parameter.s16_CB_AVR_V,		//=	3700;	//	mV	//	Cell Balancing Check Average Cell Volt
			Parameter.s16_CB_SDV,		//=	  30;	//	mV	//	Cell Balancing Check Start DV
			Parameter.s16_CB_EDV,		//=	  20;	//	mV	//	Cell Balancing Check Ended DV
			//----------------------------------------
			Parameter.s16_CD_AMP,		//=	 300;	//	3.00A
			Parameter.s16_DD_AMP,		//=  (-300);	// -3.00A
			//----------------------------------------
			//----------------------------------------
			Parameter.s16_CAPACITY,		//=	 380;	//	390;//39.0 Ah	//LB_SN_0001(230829)
			//----------------------------------------
			Parameter.s16_SOH_CC,		//= 0;
			//---------------------------------------
			Parameter.s16_PF_FLAG,		//= _EVENT_NORMAL;
			//---------------------------------------
			Parameter.s32_EVENT_SEC,	//= 0;
			//---------------------------------------
			Parameter.s16_EVENT_INDEX,	//= 0;
			//---------------------------------------
			Parameter.s16_RESET_INDEX);	//= 0;
			//---------------------------------------
#endif
}

void EVENT_1SEC_DATA_MOVE(void)
{	//	1초마다 발생하는 데이터를 내부램에 임시 저장 한다.
	//	11 Record Ring Buffering Control.
	//	memset(&_EWR, 0,sizeof(_EVENT_ARRAY_DATA));
	_EWR.sRECORD[_EWR.u16_HEAD_INDEX].u32_SEC	= (uint32_t)Parameter.s32_EVENT_SEC;
	if(_EWR.u16_ISSUE_STATE)	//	보호/복구 이벤트 코드 체크 루틴에서(Protection.c)
	{	//	이벤트 발생 시점의 데이터
		_EWR.sRECORD[_EWR.u16_HEAD_INDEX].u16_CODE = _EWR.u16_ISSUE_STATE;
		//----------------------------------------------------
		_EWR.u16_ISSUE_STATE = _EVENT_NORMAL;				//	이벤트 발생 코드 클리어
		_EWR.u16_ISSUE_FLAG	 = _EVENT_FLAGS_ISSUE_DATA;		//	이벤트 발생 데이터를 FRAM 저장 체크 변수(발생전10개,발생1개)
															//	void EVENT_DATA_SAVE_CHECK(void) 함수에서 처리
		//---------------------------------------------------
	}
	else
	{	//	이벤트 발생 전/후의 데이터
		_EWR.sRECORD[_EWR.u16_HEAD_INDEX].u16_CODE	= System_Info.Fault.u16_Fault;
	}
	_EWR.sRECORD[_EWR.u16_HEAD_INDEX].u16_MAX_V	= System_Info.Cell_Voltage.u16_Max_Cell_Voltage;
	_EWR.sRECORD[_EWR.u16_HEAD_INDEX].u16_MIN_V	= System_Info.Cell_Voltage.u16_Min_Cell_Voltage;
	_EWR.sRECORD[_EWR.u16_HEAD_INDEX].u16_PAC_V	= (uint16_t)(System_Info.Electric.u32_Pack_Voltage/10);			// xxx --> XX.X V(/1000)
	_EWR.sRECORD[_EWR.u16_HEAD_INDEX].s16_PAC_I	= (int16_t)(System_Info.Electric.s32_Sum_of_1sec_Current);	//	/10);	// xxx --> XX.X A(/100)
	_EWR.sRECORD[_EWR.u16_HEAD_INDEX].s8_MAX_T	= (int8_t)(System_Info.Cell_Temp.s16_Max_Cell_Temperature);
	_EWR.sRECORD[_EWR.u16_HEAD_INDEX].s8_MIN_T	= (int8_t)(System_Info.Cell_Temp.s16_Min_Cell_Temperature);

	{	// INTERNAL RAM INDEX ++
		_EWR.u16_HEAD_INDEX++;
		_EWR.u16_HEAD_INDEX %= _FRAM_EVENT_ISSUE_RECORD_SIZE; // head Index ++
	}
	if(_EWR.u16_HEAD_INDEX == _EWR.u16_TAIL_INDEX)
	{	// 버퍼가 모두 찼다면
		_EWR.u16_TAIL_INDEX++;
		_EWR.u16_TAIL_INDEX %= _FRAM_EVENT_ISSUE_RECORD_SIZE; // tail Index ++
	}
}
//----------------------------------------------------------------------------------------------------
//	1.	이벤트 발생시 발생전 데이터 10개, 발생 데이터 1개 11개를 한번에 저장
//	2.	이후 1초마다 발생후 데이터를  10번 저장(_EWR 메모리 이용)
//	3.	_EWR.sRECORD[_FRAM_EVENT_ISSUE_RECORD_SIZE::11] 메모리는 1초 1개의 레코드 처리,이벤트 발생전/발생/발생후데이터
//----------------------------------------------------------------------------------------------------
static 	uint16_t	head, tail;
void EVENT_DATA_SAVE_CHECK(void)
{	//	EVENT DATA to FRAM WRITE 처리
	//log_debug("_EVENT_ARRAY_DATA = %d, _EVENT_DATA = %d \r\n",sizeof(_EVENT_ARRAY_DATA), sizeof(_EVENT_DATA));
	if(_EWR.u16_ISSUE_FLAG == _EVENT_FLAGS_ISSUE_DATA)
	{	//	이벤트 발생시, 이전의 이벤트 데이터 처리중(이전의 이벤트 발생 후 데이터::10개의 데이터) 이었다면 처리하지 못한 나머지 데이터 저장은 하지 않는다.
		//		(아래 참고 0으로 저장 되어 있음)
		//	이벤트 데이터 저장 11개 레코드(발생전 10개, 발생 1개)를 FRAM에 저장 한다.
		//		BMS 전원 인가 이후 이전 데이터가 10개 이하인데 이벤트 발생 했을 경우(11개 이하 저장 레코드 일 경우)
		//--------------------------------
		tail = _EWR.u16_HEAD_INDEX;		//	가장 오래전 발생 데이터 부터 순서대로(뒤에서 부터) _EWR.u16_TAIL_INDEX 쓰면 안됨.
		head = _EWR.u16_HEAD_INDEX;		//	가장 최근에 발생 데이터 까지 저장
		//------------------------------------------------
		//	FRAM 저장 시작 위치 인덱스를 계산 한다(21개의 레코드 크기)
		_EWR.u16_AFTER_INDEX = _FRAM_ADDR_EVENT_START + (Parameter.s16_EVENT_INDEX * sizeof(EVENT_EMPTY_DATA) );
		//-------------------------------------------------------------------------------
		//	EVENT 발생 위치의 CLEAR(0) FRAM MEMORY AREA 21 Record , 나중에 필요 없으면 지울것 ??(나두는것이 좋을듯)
		memset(&EVENT_EMPTY_DATA[0], 0,sizeof(EVENT_EMPTY_DATA));
		FM25V02_wr_data(_EWR.u16_AFTER_INDEX, sizeof(EVENT_EMPTY_DATA), (uint8_t *)&EVENT_EMPTY_DATA);
		//-------------------------------------------------------------------------------------------------------
		//	FRAM Clear WRITE / READ CHECK 확인
		//FM25V02_rd_data(_EWR.u16_AFTER_INDEX, sizeof(EVENT_LOAD_DATA),  (uint8_t *)&EVENT_LOAD_DATA);
		//	//EVENT_LOAD_DATA[0].u32_SEC	= 0xFF;
		//if(memcmp(&EVENT_EMPTY_DATA, &EVENT_LOAD_DATA, sizeof(EVENT_LOAD_DATA)))	log_debug("FRAN CLEAR NG \r\n");
		//else																		log_debug("FRAM CLEAR OK \r\n");
		//-------------------------------------------------------------------------------------------------------
		HAL_Delay(1);	// 혹시 몰라서 1ms delay 줌
		//-------------------------------------------------------------------------------------------------------
		for(int i=0; i < _FRAM_EVENT_ISSUE_RECORD_SIZE ; i++)
		{	//	이벤트 발생전 10개 이벤트 발생 1개 데이터 FRAM에 저장.
			//	이벤트 발생 코드 저장
			//-----------------------------------------------------------------------------------------
			FM25V02_wr_data(_EWR.u16_AFTER_INDEX, sizeof(_EVENT_DATA), (uint8_t *)&_EWR.sRECORD[tail]);
			//-----------------------------------------------------------------------------------------
			//	FRAM WRITE READ CHECK
			//FM25V02_rd_data(_EWR.u16_AFTER_INDEX, sizeof(_EVENT_DATA), (uint8_t *)&ERD_DATA );
			//if(memcmp(&ERD_DATA,&_EWR.sRECORD[tail],sizeof(_EVENT_DATA)))	log_debug("FRAN NG \r\n");
			//else															log_debug("FRAM OK \r\n");
			//------------------------------------------------------------------------------------
			//	저장 변수의 레코드 인덱스 위치 계산
			tail++;		tail %= _FRAM_EVENT_ISSUE_RECORD_SIZE;
			//	FRAM 저장 다음 레코드 위치 인덱스를 계산 한다(1개의 레코드 크기)
			_EWR.u16_AFTER_INDEX += sizeof(_EVENT_DATA);
			if(tail == head)	break;	// BMS 전원 인가 이후 이전 데이터가 10개 이하인데 이벤트 발생 했들경우(11개 이하 저장 레코드 일 경우)

		}
		//-----------------------------------------------------------------------------------------------------------------
		//	EVENT DATA WRITE
		//	EVENT INDEX DATA WRITE
		Parameter.s16_EVENT_INDEX++;	Parameter.s16_EVENT_INDEX %= _FRAM_EVENT_COUNT;
		FM25V02_wr_data( _FRAM_ADDR_EVENT_INDEX, sizeof(Parameter.s16_EVENT_INDEX), (uint8_t *)&Parameter.s16_EVENT_INDEX);
		//	EVENT CPF/DPF FLAG DATA WRITE
		//if((event_code == _EVENT_CPF) || (event_code == _EVENT_DPF))
		//{
		//	Parameter.s16_PF_FLAG = event_code;
		//	FM25V02_wr_data( _FRAM_ADDR_PF_FLAG, sizeof(Parameter.s16_PF_FLAG), (uint8_t *)&Parameter.s16_PF_FLAG);
		//}
		//----------------------------------------------------------------------------------------------------------------
		//	이벤트 이후의 10개 레코드 데이터 저장을 위한 변수 플레그 SET
		_EWR.u16_AFTER_COUNT	= _EVENT_NORMAL;
		_EWR.u16_ISSUE_FLAG		= _EVENT_FLAGS_AFTER_DATA;
		//----------------------------------------------------------------------------------------------------------------
	}
	else if(_EWR.u16_ISSUE_FLAG == _EVENT_FLAGS_AFTER_DATA)
	{	//	이벤트 발생 이후 데이터
		if(++_EWR.u16_AFTER_COUNT < _FRAM_EVENT_ISSUE_RECORD_SIZE)
		{	//	이벤트 발생 이후의 데이터 레코드 10개 까지 저장
			//	EVENT DATA WRITE
			FM25V02_wr_data(_EWR.u16_AFTER_INDEX, sizeof(_EVENT_DATA), (uint8_t *)&_EWR.sRECORD[tail]);
			//------------------------------------------------------------------------------------
			//	FRAM WRITE READ CHECK
			//FM25V02_rd_data(_EWR.u16_AFTER_INDEX, sizeof(_EVENT_DATA), (uint8_t *)&ERD_DATA );
			//if(memcmp(&ERD_DATA,&_EWR.sRECORD[tail],sizeof(_EVENT_DATA)))	log_debug("FRAN NG \r\n");
			//else															log_debug("FRAM OK \r\n");
			//------------------------------------------------------------------------------------
			tail++;		tail %= _FRAM_EVENT_ISSUE_RECORD_SIZE;	//	_EW.sRECORD[tail] Record 위치 변수 변경
			_EWR.u16_AFTER_INDEX += sizeof(_EVENT_DATA);		//	FRAM 저장 위치 변경
			//------------------------------------------------------------------------------------------
		}
		else
		{	//	10개 이상 저장 했으면 종료 Clear
			_EWR.u16_ISSUE_FLAG		= _EVENT_FLAGS_NOMAL_DATA;
			_EWR.u16_AFTER_COUNT	= _EVENT_NORMAL;
		}
	}
}


//---------------------------------------------------------------------------------
#if(0)
void FM25V02_READ_TEST(void);
void FM25V02_CHECK_PARAMETER(void);
void FM25V02_CHECK_PF_FLAG(void);
void FM25V02_TESTING(void);

void FM25V02_READ_TEST(void)
{
	uint8_t	twr_data[_FRAM_256BYTE]={0};
	uint8_t	trd_data[_FRAM_256BYTE]={0};
	uint32_t	s_ticks,e_ticks;
	//for(uint16_t i = 0; i< _FM_256B ; i++)	twr_data[i] = i;
	for(uint16_t j = 0; j < 1 ; j++)
	{
    	s_ticks = HAL_GetTick();
    	FM25V02_rd_data(_FRAM_ADDR_TEST+(_FRAM_256BYTE *j), sizeof(trd_data), trd_data);
    	e_ticks = HAL_GetTick();	log_debug(" - RD Time(ms): %lu \r\n", e_ticks - s_ticks );
    	s_ticks = HAL_GetTick();
		FM25V02_wr_data(_FRAM_ADDR_TEST+(_FRAM_256BYTE *j), sizeof(twr_data), twr_data);
		e_ticks = HAL_GetTick();	log_debug(" - WR Time(ms): %lu \r\n", e_ticks - s_ticks );
	}
	s_ticks = HAL_GetTick();
	FM25V02_rd_data(_FRAM_ADDR_TEST, sizeof(trd_data), trd_data);
	e_ticks = HAL_GetTick();		log_debug(" - RD Time(ms): %lu \r\n", e_ticks - s_ticks );
}


void FM25V02_CHECK_PARAMETER(void)
{
	System_Parameter RD_prar;
	FM25V02_rd_data( _FRAM_ADDR_PARAMETER, sizeof(Parameter), (uint8_t *)&RD_prar.s16_SET_FLAG);
	if(RD_prar.s16_SET_FLAG != 0x55AA)
	{
		Parameter.s16_SET_FLAG = 0x55AA;
		FM25V02_wr_data( _FRAM_ADDR_PARAMETER, sizeof(Parameter), (uint8_t *)&Parameter );
	}
	FM25V02_rd_data( _FRAM_ADDR_PARAMETER, sizeof(Parameter), (uint8_t *)&RD_prar);
}

void FM25V02_CHECK_PF_FLAG(void)
{
	FM25V02_rd_data( _FRAM_ADDR_PF_FLAG, sizeof(System_Info.Fault.u16_Fault), (uint8_t *)&System_Info.Fault.u16_Fault);
	if(System_Info.Fault.u16_Fault)
	{
		System_Info.Fault.u16_Fault = 0x0000;
		FM25V02_wr_data( _FRAM_ADDR_PF_FLAG, sizeof(System_Info.Fault.u16_Fault), (uint8_t *)&System_Info.Fault.u16_Fault );
	}
	FM25V02_rd_data( _FRAM_ADDR_PF_FLAG, sizeof(System_Info.Fault.u16_Fault), (uint8_t *)&System_Info.Fault.u16_Fault);
}

void FM25V02_TESTING(void)
{
	//FM25V02_READ_TEST();
	FM25V02_CHECK_PARAMETER();
	FM25V02_CHECK_PF_FLAG();
}
#endif
//---------------------------------------------------------------------------------


