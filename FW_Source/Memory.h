/*
 * Memory.h
 *
 *  Created on: 2022. 12. 28.
 *      Author: king
 */

#ifndef MEMORY_H_
#define MEMORY_H_
//------------------------------------------------------------------
//	230911
typedef	enum
{
	//----------------------------------------------------
	_FRAM_SET_DATA				= 0x55AA,
	_FRAM_256BYTE				= 0x0100,				//	256
	_FRAM_EVENT_SIZE			= 0x0010,				//	16 byte
	_FRAM_EVENT_COUNT			= 0x0014,				//	20 Q't
	//--------------------------------------------------
	_FRAM_EVENT_ISSUE_RECORD_SIZE	= 11,				//	이벤트 11개의 레코드, 이전 10개 발생데이터1개
	_FRAM_EVENT_TOTOL_RECORD_SIZE	= 21,				//	이벤트 21개의 레코드, 이전 10개, 발생 1개, 이후 10개
	//----------------------------------------------------
	//_FRAM_ADDR_START			= 0x0000,				//	 0
	_FRAM_ADDR_PARAMETER		= 0,					//	 0
	//----------------------------------------------------
	_FRAM_ADDR_SET_DATA			= 0x0000,				//	 0
	//----------------------------------------------------
	_FRAM_ADDR_OVP				= 0x0002,				//	 2
	_FRAM_ADDR_COTP				= 0x000A,				//	10
	_FRAM_ADDR_CPFP_MAX_V		= 0x000E,				//	14
	_FRAM_ADDR_CB_AVR_V			= 0x0016,				//	22
	_FRAM_ADDR_CD_AMP			= 0x001C,				//	28
	//----------------------------------------------------
	_FRAM_ADDR_CAPACITY			= 0x0020,				//	32
	//----------------------------------------------------
	_FRAM_ADDR_SOH_CC			= 0x0022,				//	34
	//----------------------------------------------------
	_FRAM_ADDR_PF_FLAG			= 0x0024,				//	36	CPF/DPF Flag
	//----------------------------------------------------
	_FRAM_ADDR_EVENT_SEC		= 0x0026,				//	38
	_FRAM_ADDR_EVENT_INDEX		= 0x002A,				//  42
	_FRAM_ADDR_RESET_INDEX		= 0x002C,				//  44
	//----------------------------------------------------
	_FRAM_ADDR_PARA_END			= 0x002D,				//	45	//	Parameter Data 46Byte
	//----------------------------------------------------
	//----------------------------------------------------
	_FRAM_ADDR_EVENT_START		= 0x0050,				//   80
	_FRAM_ADDR_EVENT_END		= 0x1A8F,				// 6800-1		//16 Byte X 21 Record X 20 Q't
	//----------------------------------------------------
	_FRAM_ADDR_TEST				= 0x6FFF,				//
	_FRAM_ADDR_END				= 0x7FFF,				// 32767(0x7FFF)
	_FRAM_SIZE					= 0x8000				// 32768(0x0000 ~ 0x7FFF)
	//----------------------------------------------------
} _FM25V02_INFOR;


typedef enum
{
	_EVENT_NORMAL	= 0x0000,
	_EVENT_OVP		= 0x0001,
	_EVENT_UVP		= 0x0002,
	_EVENT_COCP		= 0x0004,
	_EVENT_DOCP		= 0x0008,
	_EVENT_COTP		= 0x0010,
	_EVENT_DOTP		= 0x0020,
	_EVENT_CUTP		= 0x0040,
	_EVENT_DUTP		= 0x0080,
	_EVENT_CPF		= 0x0100,
	_EVENT_DPF		= 0x0200,
	_EVENT_RELEASE	= 0x8000,
}	_EVENT_CODE_BitState;

typedef enum
{
	_EVENT_FLAGS_NOMAL_DATA	= 0,
	_EVENT_FLAGS_ISSUE_DATA	= 1,
	_EVENT_FLAGS_AFTER_DATA	= 2
}	_EVENT_FLAGS_BitState;

#pragma pack(push,1)
typedef struct
{
	uint32_t		u32_SEC;		// 4,		1초 SEC 카운터
	uint16_t		u16_CODE;		// 2,		EVENT 보호/복구 코드
	uint16_t		u16_MAX_V;		// 2,		최대 전압
	uint16_t		u16_MIN_V;		// 2,		최소 전압
	uint16_t		u16_PAC_V;		// 2,		배터리 팩전압
	int16_t			s16_PAC_I;		// 2,		1초 펙 전류
	int8_t			s8_MAX_T;		// 1,		최대 온도
	int8_t			s8_MIN_T;		// 1,		최소 온도
}	_EVENT_DATA;					// 16,
#pragma pack(pop)

//extern	_EVENT_DATA		EWR_DATA;		//	미사용
extern	_EVENT_DATA			ERD_DATA;		//	FRAM data write 이후 memcmp() 함수 확인을 위한 READ 변수
extern	_EVENT_DATA			EVENT_LOAD_DATA[_FRAM_EVENT_TOTOL_RECORD_SIZE];		//	외부 FRAM 메모리 21개 레코드 DATA LOAD 데이터 저장 변수
extern	_EVENT_DATA			EVENT_EMPTY_DATA[_FRAM_EVENT_TOTOL_RECORD_SIZE];	//	외부 FRAM 메모리 21개 레코드 DATA CLEAR 데이터 0 변수
//#define	EVENT_RECORD_MAX_SIZE	11

#pragma pack(push,1)
typedef	struct
{
	uint16_t		u16_ISSUE_FLAG;								//	READ/RWITE
	uint16_t		u16_ISSUE_STATE;							//	EVENT CODE
	uint16_t		u16_AFTER_INDEX;							//	FRAM READ/WRITE POSITION INDEX
	uint16_t		u16_AFTER_COUNT;							//	이벤트 이후 데이터 10개 제어 변수
	uint16_t		u16_HEAD_INDEX;								//	환형 버퍼 시작 인덱스
	uint16_t		u16_TAIL_INDEX;								//	환형 버퍼 종료 인텍스
	_EVENT_DATA		sRECORD[_FRAM_EVENT_ISSUE_RECORD_SIZE];		//	11개 레코드 1초 마다 데이터 환형 버퍼링 제어 경신
}	_EVENT_ARRAY_DATA;
#pragma pack(pop)
//------------------------------------------
extern	_EVENT_ARRAY_DATA	_ERD;	//	EVENT DATA 읽기 내부 RAM 변수(레코드는 영역은 사용하지 않음) EVENT_LOAD_DATA 변수와 같이 사용
extern	_EVENT_ARRAY_DATA	_EWR;	//	EVENT DATA 저장 내부 RAM 변수(제어 및 레코드 변수)
//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FM25V02_PARAMETER_CHECK(void);
extern void EVENT_1SEC_DATA_MOVE(void);
extern void EVENT_DATA_SAVE_CHECK(void);
//------------------------------------------------------------------
//------------------------------------------
#endif /* MEMORY_H_ */
