/*
* Task_scheduler.c
*
*/

#include "Task_Scheduler.h"
#include "System_Info.h"
#include "Memory.h"
#include "Monitoring.h"
#include "LB/LB_Main.h"

#include "Cell_Balancing.h"
#include "Library\FM25V02.h"
#include "Parameter.h"

#include <string.h>
#include <stdio.h>


//float FW_Version = 0.309;			//	ORJ
//float FW_Version = 0.310;			//	LB_0002
//float FW_Version = 0.311;			//	LB_0004,	Cap 390 to 280 변경
//float FW_Version = 0.312;			//	LB_0001,	Cap 280 to 380 변경, BMS RESET SOC OCV Table 적용
//float	FW_Version = 0.313;			//	Cap 380 변경, BMS RESET SOC OCV Table 적용, PF, Balancing, FRAM, PARAMETER CAN I/F
//float	FW_Version = 0.315;			//	230913 30대분 시리얼 번호 뒤 V1 표기	Cap 380 변경, BMS RESET SOC OCV Table 적용, PF, Balancingv
float	FW_Version = 2.4;          // 250109 

static uint32_t System_time;
static uint16_t flag_10ms;
static uint16_t flag_25ms;
static uint16_t flag_100ms;
static uint16_t flag_200ms;
static uint16_t flag_1sec;
static uint16_t flag_5sec;
static uint8_t System_ON = FALSE;
extern CAN_HandleTypeDef hcan2;

void HAL_SYSTICK_Callback(void)
{
	if(System_ON == TRUE)
	{
		System_time++;
		flag_10ms++;
		flag_25ms++;
		flag_100ms++;
		flag_200ms++;
		flag_1sec++;
		flag_5sec++;
	}
}

extern void init_offset(void);
extern void Sensing_10times(void);
extern void	init_Parameter(void);
extern void	get_Cell_Datas(void);
extern void	set_Cell_datas(void);
extern void get_Cell_Datas_200ms(void);
extern void get_SOC_from_OCVTable(int volt);
extern void	set_Max_Capacity(void);
extern uint16_t SOC;
extern int now_c, max_c;

void initialize(void)
{
	memset( &System_Info, 0, sizeof( System_Info ) );

	HAL_GPIO_WritePin(GPIOA, CSBI_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, CSBI_2_Pin, GPIO_PIN_SET);

	System_Info.Status.u16_BMS_status = _STATUS_INIT;

	HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, u_PRE_G_Pin, GPIO_PIN_RESET);

	Sensing_10times(); //초기에 쓰레기값 날아오는 것 방지.

#if(1)

	//init_Parameter();

	FM25V02_PARAMETER_CHECK();

#endif
	get_Cell_Datas( );
	set_Cell_datas( );
	get_SOC_from_OCVTable(System_Info.Cell_Voltage.u16_Avg_Cell_Voltage);

	//
	//FM25V02_TESTING();
	//
	init_LB(); //set_Max_Capacity 보다 위에 있어야 함. 앨비 주행거리 테스트 지원 용도

#if (0)
	//	RESET 시 OCV Table 적용(230829)
	//	BMS Reset SOC OCV Table 대입
	//HAL_Delay(20);
	get_Cell_Datas();
	set_Cell_datas();
	get_SOC_from_OCVTable(System_Info.Cell_Voltage.u16_Avg_Cell_Voltage);
#endif

	set_Max_Capacity();

	SOC =  (uint16_t)((now_c * 1000) / max_c);

	System_Info.Lifecycle.s32_max_c = max_c;
	System_Info.Lifecycle.s32_now_c = now_c;
	System_Info.Lifecycle.u16_SOC = SOC;
	System_Info.Lifecycle.u8_SOC = (uint8_t)(SOC/10);

	//init_offset();
	System_ON = TRUE;

	//log_debug("FW_VER = %d \r\n", System_Info.Manufacture.u8_FW_Version);
	//log_debug("FW_VER = %2f \r\n", FW_Version);
	//log_debug("LB,CEEL1,CEEL2,CEEL3,CEEL4,CEEL5,CEEL6,CEEL7,CEEL8,CEEL9,CEE10,CEE11,CEE12,CEE13,CEE14,CEE15,CEE16,CEE17,CEE18,CEE19,CEE20,"
	//	   "SUM_V,AVG_V,MIN_V,MAX_V,d___V,T1,T2,T3,T4,tmpI,AvgI,SOC_,SOH_,AOFFS,FAULT\n");
}

void Task_10ms(void)
{
	if(flag_10ms >= 10)
	{
		flag_10ms = RESET;
	}
	//self test
}

/*
int c;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (System_ON == TRUE)
	{
		if (htim->Instance == TIM3) //25ms
		{
			if (__debug__ == 0 )
			{
				get_Current_Value();
				c++;
			}
			Sum_of_1sec_current();
		}
	}
}
*/


extern void get_Current_Value(void);


static int current_count = 0;
static int temp_current = 0;
extern int relay_mode;
void Task_25ms(void)
{
	if(flag_25ms >= 25)
	{
		if ((__debug__ == 0) && (relay_mode == 100) )
		{
			get_Current_Value();
		}
		//Sum_of_1sec_current();

		current_count++;

		if(current_count>40)
		{
			//log_debug("\r\n sensing count : %d\r\n",current_count);
			//System_Info.Electric.s32_Sum_of_1sec_Current = (int)temp_current/40;//ORG
			//System_Info.Electric.s32_Sum_of_1sec_Current = (int)temp_current/40*0.549;//유진 #1 보정
			System_Info.Electric.s32_Sum_of_1sec_Current = (int)temp_current/40;//유진 #2 보정
			temp_current = 0;
			current_count = 0;
		}
		else
		{
			temp_current += System_Info.Electric.s16_Pack_Current;
		}
		flag_25ms = RESET;
	}
	//전류
}

void CAN_recovery(void)
{
	if (((CAN2->ESR) & (1 << 2)))
	{
		if (HAL_CAN_Init(&hcan2) != HAL_OK)
		{
			Error_Handler();
		}

		if (HAL_CAN_Start(&hcan2) != HAL_OK)
		{
			Error_Handler();
		}

		if (HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

extern void Send_CAN_100ms(void);
void Task_100ms(void)
{
	//CAN
	if(flag_100ms >= 100)
	{
		Send_CAN_100ms();
		CAN_recovery();
		flag_100ms = RESET;
	}
}

static int volt_count = 0;
static int temp_volt[20];

void Task_200ms(void)
{

	if(flag_200ms >= 200)
	{
		flag_200ms = RESET;

		get_Cell_Datas_200ms(); //200ms 마다 읽은 값을 System_Info.Cell_Voltage.u16_Cell_Voltage_200ms[k]에 저장

		volt_count++;



			if(volt_count >5) //200ms 마다 읽은 값을 5번 더해서 1초 평균을 구함
			{
				for(int k =0; k<20 ;k++)//K = 전체 셀의 갤수 (20개)
				{
					System_Info.Cell_Voltage.u16_Cell_Voltage[k] = temp_volt[k]/5;
					temp_volt[k] = 0;

					if(k == 19) //20번셀의 1초 평균 까지 모두 구했다면 카운트 초기화
					{
						volt_count = 0;

					}
				}
			}

			else
			{
				for(int k =0; k<20 ;k++)//K = 전체 셀의 갤수 (20개)
				{
					temp_volt[k] += System_Info.Cell_Voltage.u16_Cell_Voltage_200ms[k];
				}
			}


	}
	else {}
}

extern float ACSoffset;
extern void Calc_SOC(int current);
extern void get_Temp_Data(void);
extern void set_Cell_datas(void);
extern void	Cell_Balancing(void);	//230907
extern void relay_Process(void);
extern void charge_mode(void);
extern void cp_process(void);
extern void vp_process(void);
extern void tp_process(void);
extern void pf_process(void);
extern void Auto_Release(void);

extern boolean is_Monitoring_ON;
extern void canif_monitor_data_send(void);

#if(1)
void rs485_monitor_data_send(void)
{

#if(1)
	char	msg1[256] = {0,}, msg2[256]={0,};
	// 1 ~ 10 : not used
	sprintf(msg1,"BMS 030V07S090A 0000 0019 0x00 0x18 0x43 0x00 0x00 0x00 ");
	// 11 ~ 17 : cell voltage(mV)
	for(int i=0; i<7; i++)
	{
		sprintf(msg2, "%04d ", System_Info.Cell_Voltage.u16_Cell_Voltage[i]);
		strcat(msg1, msg2);
	}
    // 18 : pack voltage(mV) 19 ~ 22 : cell temp (decimal)
	sprintf(msg2 , "%05d %03d %03d %03d %03d ", (unsigned int)System_Info.Electric.u32_Pack_Voltage,
									  System_Info.Cell_Temp.s16_Cell_Temperature[0],
									  System_Info.Cell_Temp.s16_Cell_Temperature[1],
									  System_Info.Cell_Temp.s16_Cell_Temperature[2],
									  System_Info.Cell_Temp.s16_Cell_Temperature[3]);

	strcat(msg1, msg2);
	// 23 ~ 25 : not used
	sprintf(msg2 , "000 000 000 ");
	strcat(msg1, msg2);
	// 26 : pack status (IDL, CHG, DSG, PRT)
	if(System_Info.Fault.u16_Fault == 0x0000){
		if(System_Info.Status.Charge_Status == IDLE){
			sprintf(msg2 ,"%s ", "IDL");
		} else if(System_Info.Status.Charge_Status == CHARGE){
			sprintf(msg2 ,"%s ", "CHG");
		} else if(System_Info.Status.Charge_Status == DISCHARGE){
			sprintf(msg2 ,"%s ", "DSG");
		}
	} else {
		sprintf(msg2 ,"%s ", "PRT");
	}
	strcat(msg1, msg2);

	// 27 : pack current(mA), 28 - 29 : not used, 30 : pack soc
	sprintf(msg2 , "%06d 000 000 %d ", System_Info.Electric.s32_Sum_of_1sec_Current*10, System_Info.Lifecycle.u8_SOC);
	strcat(msg1, msg2);

	// 31 : pack status (protection info)
	switch(System_Info.Fault.u16_Fault)	{
		case f_OVP: // OVP
			sprintf(msg2 ,"%s ", "OV");
			break;

		case f_UVP: // UVP
			sprintf(msg2 ,"%s ", "UV");
			break;

		case f_OCP: // COCP
			sprintf(msg2 ,"%s ", "sCOC");
			break;

		case f_DOCP: // DOCP
			sprintf(msg2 ,"%s ", "sDOC");
			break;

		case f_COTP: // COTP
			sprintf(msg2 ,"%s ", "sCOT");
			break;

		case f_DOTP: // DOTP
			sprintf(msg2 ,"%s ", "sDOT");
			break;

		case f_CUTP: // CUTP
			sprintf(msg2 ,"%s ", "sCUT");
			break;

		case f_DUTP: // DUTP
			sprintf(msg2 ,"%s ", "sDUT");
			break;

		case f_CPF: // CPF
		case f_DPF: // DPF
			sprintf(msg2 ,"%s ", "sCDV");
			break;

		default:
			sprintf(msg2 ,"%s ", "OK");
			break;
	}
	strcat(msg1, msg2);

	// 32 : balance status
	if(System_Info.Balancing.is_Cell_Balancing_ON == ON)
	{
		sprintf(msg2 ,"%s ", "CB");
	} else {
		sprintf(msg2 ,"%s ", "IDL");
	}
	strcat(msg1, msg2);

	// 33 : Charging status
	if(System_Info.Status.Charge_Status == CHARGE){
		sprintf(msg2 ,"%s ", "ChargerIn");
	} else {
		sprintf(msg2 ,"%s ", "ChargerOut");
	}
	strcat(msg1, msg2);

	// 34 : CHG FET status
	if((System_Info.Status.u16_BMS_status & (_STATUS_CHARGE_FET)) == 0x4000){ // CHG FET ON
		sprintf(msg2 ,"%s ", "ChgH");
	} else {
		sprintf(msg2 ,"%s ", "ChgL");
	}
	strcat(msg1, msg2);

	// 35 : KEY ON 36: not used
	sprintf(msg2 ,"KeyH 000 \r\n");
	strcat(msg1, msg2);

	printf("%s",msg1);

#else
	char	msg1[256] = {0,}, msg2[256]={0,};

	sprintf(msg1,"LB,S=%ld,", Parameter.s32_EVENT_SEC);
	for(int i=0; i<20; i++)
	{
		sprintf(msg2, "%.3f,", 			(float)System_Info.Cell_Voltage.u16_Cell_Voltage[i]/1000);
		strcat(msg1, msg2);
	}
	sprintf(msg2 , "%.2f,%.3f,%.3f,%.3f,%.3f,",	(float)System_Info.Electric.u32_Pack_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Avg_Cell_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Min_Cell_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Max_Cell_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Diff_Cell_Voltage/1000);
	strcat(msg1, msg2);
	sprintf(msg2, "%d,%d,%d,%d,",		System_Info.Cell_Temp.s16_Cell_Temperature[0],
										System_Info.Cell_Temp.s16_Cell_Temperature[1],
										System_Info.Cell_Temp.s16_Cell_Temperature[2],
										System_Info.Cell_Temp.s16_Cell_Temperature[3]);
	strcat(msg1, msg2);
	sprintf(msg2, "%.2f,%.2f,",			(float)System_Info.Electric.s16_Pack_Current/100,
										(float)System_Info.Electric.s32_Sum_of_1sec_Current/100);
	strcat(msg1, msg2);
	sprintf(msg2, "%.1f,%.1f,%.1f,",	(float)System_Info.Lifecycle.u16_SOC/10,
										(float)System_Info.Lifecycle.u16_SOH,ACSoffset);		//printf("%.3f,",FW_Version);
	strcat(msg1, msg2);
	sprintf(msg2, "%04x,%lx\r\n", 		System_Info.Fault.u16_Fault,_CBD.DCC_FLAG);
	strcat(msg1, msg2);
	printf("%s",msg1);
#endif
}

#else

void rs485_monitor_data_send(void)
{
	//LB_driving_SOC(System_Info.Cell_Voltage.u16_Avg_Cell_Voltage);
	//printf("LB, CEEL1, CEEL2, CEEL3, CEEL4, CEEL5, CEEL6, CEEL7, CEEL8, CEEL9, CEE10, CEE11, CEE12, CEE13, CEE14, CEE15, CEE16, CEE17, CEE18, CEE19, CEE20, "
	//	   "SUM_V, AVG_V, MIN_V, MAX_V, d___V, T1, T2, T3, T4, tmpI, AvgI, SOC_, SOH_, AOFFS, FwVer, FAULT\n");
	printf("LB,");
	for(int i=0; i<20; i++)
	{
		printf("%.3f,",					(float)System_Info.Cell_Voltage.u16_Cell_Voltage[i]/1000);
	}

	printf("%.2f,%.3f,%.3f,%.3f,%.3f,",	(float)System_Info.Electric.u32_Pack_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Avg_Cell_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Min_Cell_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Max_Cell_Voltage/1000,
										(float)System_Info.Cell_Voltage.u16_Diff_Cell_Voltage/1000);

	printf("%d,%d,%d,%d,",				System_Info.Cell_Temp.s16_Cell_Temperature[0],
										System_Info.Cell_Temp.s16_Cell_Temperature[1],
										System_Info.Cell_Temp.s16_Cell_Temperature[2],
										System_Info.Cell_Temp.s16_Cell_Temperature[3]);

	printf("%.2f,%.2f,",				(float)System_Info.Electric.s16_Pack_Current/100,
										(float)System_Info.Electric.s32_Sum_of_1sec_Current/100);

										//printf("%d,",vp_fault );	//printf("%d,",cp_fault );	//printf("%d,",tp_fault );
	printf("%.1f,%.1f,%.1f,",			(float)System_Info.Lifecycle.u16_SOC/10,
										(float)System_Info.Lifecycle.u16_SOH,ACSoffset);		//printf("%.3f,",FW_Version);
	////
	printf("%04x,%lx\r\n", 				System_Info.Fault.u16_Fault,_CBD.DCC_FLAG);
}
#endif

//static uint32_t	s_ticks,e_ticks;
void Task_1sec(void)
{
	//get_SOC_from_OCVTable(System_Info.Cell_Voltage.u16_Avg_Cell_Voltage);

	if(flag_1sec >= 1000)
	{
		//s_ticks = HAL_GetTick();
#if(1)
		flag_1sec = RESET;
#endif
		Calc_SOC(System_Info.Electric.s32_Sum_of_1sec_Current);

		//get_Cell_Datas();
		get_Temp_Data();
		set_Cell_datas();

		Cell_Balancing();	//230907	추가

		EVENT_DATA_SAVE_CHECK();		//	EVENT ISSUE 11개 레코드 FRAM에 저장

		relay_Process();

		charge_mode();

		vp_process();

		cp_process();

		tp_process();

		pf_process();		//230907	추가

		Auto_Release();		//FM25V02_write_1sec();

		EVENT_1SEC_DATA_MOVE();			// EVENT ISSUE 전후 데이터 1초 마다 이동 링버퍼링 제어
										// EVENT 발생시 다음 1초후 EVENT_DATA_SAVE_CHECK();에서 FRAM에 저장
		if(is_Monitoring_ON)
		{
			canif_monitor_data_send();
		}// else {
		rs485_monitor_data_send();
		//}

		HAL_GPIO_TogglePin(GPIOA, u_LED_Pin);

		//	실행 시간 초 데이터 저장
		Parameter.s32_EVENT_SEC++;
		FM25V02_wr_data( _FRAM_ADDR_EVENT_SEC, sizeof(Parameter.s32_EVENT_SEC), (uint8_t *)&Parameter.s32_EVENT_SEC);
		FM25V02_rd_data( _FRAM_ADDR_EVENT_SEC, sizeof(rd_para.s32_EVENT_SEC), 	(uint8_t *)&rd_para.s32_EVENT_SEC);
		//log_debug("S=,%ld,", Parameter.s32_EVENT_SEC);

		//e_ticks = HAL_GetTick();	log_debug(" - Task_1sec Time(ms): %lu \r\n", e_ticks - s_ticks );		//23 ms mV 시 20ms

#if(0)
		flag_1sec = RESET;
#endif
	}
}

void Task_5sec(void)
{
	//SOC write to FM
	if(flag_5sec >= 5000)
	{
		//FM25V02_write_1sec();
		flag_5sec = RESET;
	}
}

extern void get_RTC_Data(void);
extern void __test_SOH__(void);


void Task_RealTime(void)
{
	//if(System_ON == TRUE)
	Monitoring_CAN();	//	CAN 통신 I/F 처리
}
