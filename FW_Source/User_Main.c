/*
 * User_Main.c
 *
 *  Created on: 2022. 10. 15.
 *      Author: LEE-LAPTOP
 */

#include <stdio.h>

#include "main.h"
#include "User_Main.h"
#include "Library\ads1115.h"
#include "stm32f2xx_it.h"
#include "Parameter.h"
//#include "Task_Scheduler.h" 이거 읽으면 구조상 관리가.. 될까?
#include "System_Info.h"

extern RTC_HandleTypeDef hrtc;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart6;
//---------------------------------
//	230726 추가
int pf_process_enable = 0;
//---------------------------------
int tp_process_enable = 0;
int cp_process_enable = 0;
int vp_process_enable = 0;

typedef struct
{
	uint8_t	u8_Year;
	uint8_t	u8_Mon;
	uint8_t	u8_Day;
	uint8_t	u8_Hore;
	uint8_t	u8_Min;
	uint8_t	u8_Sec;

	uint8_t  u8_Old_Sec;

}STT_DRV_IN_CPU_RTCTIME;
STT_DRV_IN_CPU_RTCTIME	stg_Drv_In_RTC_Cpu;


static int16_t	Adc_Sensor_Value[5];
extern uint32_t u32t_Buffer[5];

static int Main_Sec_Change = 0;


static uint8_t Charge_Status = 0;// charge=1/dis-charge=2

//static signed int System_Current_Value = 0;
int System_Current_Value = 0;

static float f_System_Current = 0.0;

//
extern CAN_HandleTypeDef hcan2;
extern int CAN_rx_data[8];
extern void Calc_SOC(int current);

extern uint8_t flash_rxbuf[10];


int get_SOC_flag=1;

#define ADS1115_ADDRESS 0x48

int temp_tbl[ ] =
{
	/**/
	 180, 200, 250, 300, 306, 326, 367, 412, 432, 528,
	 624, 720, 816, 933,1041,1167,1316,1468,1638,1825,
	2011,2207,2403,2596,2795,2981,3144,3304,3441,3562,
	3647,3732,3817,3902,3987,4072,4157,4242,4327,4412,4497,4582
};

float f32s_Sys_App_Temper_Sns_Read( uint16_t u16t_Adc_Value )
{
   float val = (int)( u16t_Adc_Value );

   int i;
   float temp=0,offset=0;

   for( i=0;i<43;i++ )
   {
       if(val < temp_tbl[i])
       {
       	break;
       }
   }

   if(i == 0)
   {
	   log_debug("ADC Value Error");
   }
   else
   {
	   if(val == temp_tbl[i-1])
	   {
		   offset = 5;
	   }
	   else
	   {
		   offset = 5 - ((val - temp_tbl[i-1]) * 5 / (temp_tbl[i] - temp_tbl[i-1]));
	   }
   }
   temp = /*65*/125 - (i * 5) +  offset;

   return (float)temp;
}


void get_RTC_Data( void )
{
	RTC_DateTypeDef rtc_date;
	RTC_TimeTypeDef rtc_time;

	HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BCD);

	stg_Drv_In_RTC_Cpu.u8_Year = rtc_date.Year;
	stg_Drv_In_RTC_Cpu.u8_Mon = rtc_date.Month;
	stg_Drv_In_RTC_Cpu.u8_Day = rtc_date.Date;
	stg_Drv_In_RTC_Cpu.u8_Hore = rtc_time.Hours;
	stg_Drv_In_RTC_Cpu.u8_Min = rtc_time.Minutes;
	stg_Drv_In_RTC_Cpu.u8_Sec = rtc_time.Seconds;

	if( stg_Drv_In_RTC_Cpu.u8_Sec != stg_Drv_In_RTC_Cpu.u8_Old_Sec )
	{
		Main_Sec_Change = 1;
		stg_Drv_In_RTC_Cpu.u8_Old_Sec = stg_Drv_In_RTC_Cpu.u8_Sec;

		/*
		log_debug("\r\n%02X-%02X-%02X/%02X:%02X:%02X, ",	stg_Drv_In_RTC_Cpu.u8_Year,
														stg_Drv_In_RTC_Cpu.u8_Mon,
														stg_Drv_In_RTC_Cpu.u8_Day,
														stg_Drv_In_RTC_Cpu.u8_Hore,
														stg_Drv_In_RTC_Cpu.u8_Min,
														stg_Drv_In_RTC_Cpu.u8_Sec );
		*/
	}
	else
	{
		Main_Sec_Change = 0;
	}

	return;
}

float i_adc_voltage = 0.0;
float v_out = 0;
int max_temp, min_temp, avr_temp;
int max_temp_num, min_temp_num =0;
void get_Temp_Data(void)
{

	Adc_Sensor_Value[0] = f32s_Sys_App_Temper_Sns_Read(u32t_Buffer[0]);
	Adc_Sensor_Value[1] = f32s_Sys_App_Temper_Sns_Read(u32t_Buffer[1]);
	Adc_Sensor_Value[2] = f32s_Sys_App_Temper_Sns_Read(u32t_Buffer[2]);
	Adc_Sensor_Value[3] = f32s_Sys_App_Temper_Sns_Read(u32t_Buffer[3]);


	i_adc_voltage = (float)( u32t_Buffer[4]/2048 ) * 5;//3.3;
	v_out = 3300 * u32t_Buffer[4] / 4096;


	max_temp = Adc_Sensor_Value[0];
	min_temp = Adc_Sensor_Value[0];
	avr_temp = 0;

	for(int i=0; i<4; i++)
	{
		if( max_temp<Adc_Sensor_Value[i] )
		{
			max_temp = Adc_Sensor_Value[i];
			max_temp_num = i;
		}
		if( min_temp>Adc_Sensor_Value[i] )
		{
			min_temp = Adc_Sensor_Value[i];
			min_temp_num = i;
		}
		avr_temp +=Adc_Sensor_Value[i];
	}


	System_Info.Cell_Temp.s16_Cell_Temperature[0] = Adc_Sensor_Value[0];
	System_Info.Cell_Temp.s16_Cell_Temperature[1] = Adc_Sensor_Value[1];
	System_Info.Cell_Temp.s16_Cell_Temperature[2] = Adc_Sensor_Value[2];
	System_Info.Cell_Temp.s16_Cell_Temperature[3] = Adc_Sensor_Value[3];

	//**************프로토콜상 8개 보내줘야 하는데 온도센서 4개밖에 없음..****************
	System_Info.Cell_Temp.s16_Cell_Temperature[4] = Adc_Sensor_Value[0];
	System_Info.Cell_Temp.s16_Cell_Temperature[5] = Adc_Sensor_Value[1];
	System_Info.Cell_Temp.s16_Cell_Temperature[6] = Adc_Sensor_Value[2];
	System_Info.Cell_Temp.s16_Cell_Temperature[7] = Adc_Sensor_Value[3];

	System_Info.Cell_Temp.s16_Max_Cell_Temperature =		max_temp;
	System_Info.Cell_Temp.u16_Max_Cell_Temperature_Num =	max_temp_num+1;
	System_Info.Cell_Temp.s16_Min_Cell_Temperature = 		min_temp;
	System_Info.Cell_Temp.u16_Min_Cell_Temperature_Num =	min_temp_num+1;

	System_Info.Cell_Temp.s16_Diff_Cell_Temperature = max_temp - min_temp;
	System_Info.Cell_Temp.s16_Avg_Cell_Temperature  = avr_temp / 4;

}

float f32t_Get_I_Adc0 = 0;
float f32t_Get_I_Adc1 = 0;
int flag = 0;
int init_setup = 0;

float mVperAmp 	= 5;//5V
float ACSoffset_org = 2515.0;//2.5V
float ACSoffset = 0.0;
float current_offset=0;
uint8_t offset_count=40;

float init_offset(void)
{
	int i=0;
	float temp=0.;
	for(i=0;i<offset_count;i++)
	{
		ADS1115_GetVoltage(&ADS1115_ADDR_GND);
		ADS1115_RefreshAllChannel(&ADS1115_ADDR_GND);

		temp += ADS1115_ADDR_GND.ADS1115_Vol[0];
	}
	current_offset = temp / (float)offset_count;
	return current_offset;
}

void get_Current_Value(void)
{
	//---------------------------
	//	Calibration parameter 추가 해야한다 231017.
	//float curr_calibration = 0.7;		//	#60
	//float curr_calibration = 0.8;		//	#35
	//float curr_calibration = 0.84;	//	#03,
	//float curr_calibration = 0.85;	//	#28,#11,
	//float curr_calibration = 0.9;		//	#27,#44,
	//float curr_calibration = 0.92;	//	#25
	float curr_calibration = 1.0;		//	Normal
	//float curr_calibration = 1.2;  //240104 1EA 보정용
	//---------------------------
	float curr_test0=0.0;
	float curr_test1=0.0;

	if(init_setup == 0)
	{
		ADS1115_UserConfig_SingleConver(&ADS1115_ADDR_GND,ADS1115_ADDRESS_GND);
		ADS1115_UserConfig_SingleConver(&ADS1115_ADDR_VDD,ADS1115_ADDRESS_VDD);

		ADS1115_GetVoltage(&ADS1115_ADDR_GND);
		ADS1115_RefreshAllChannel(&ADS1115_ADDR_GND);
		ACSoffset = init_offset();
		if(ACSoffset < 100) ACSoffset = 2515.0;//open the shunt case
		init_setup = 1;
	}

	ADS1115_GetVoltage(&ADS1115_ADDR_GND);
	ADS1115_RefreshAllChannel(&ADS1115_ADDR_GND);

	curr_test0 = (ADS1115_ADDR_GND.ADS1115_Vol[0] - ACSoffset);
	curr_test1 = curr_test0 / 5.0;
	//------------------------------------------------------------------
	//f_System_Current = (-1.)*(curr_test1);					//	A
	//	Calibration	parameter 추가 해야한다 231017.
	//f_System_Current = (-1.)*(curr_test1)*(curr_calibration);  //	A
	f_System_Current = (-1.)*(curr_test1)/(curr_calibration);	//	2400104 보정용
	//------------------------------------------------------------------

	if((-300 < f_System_Current) && (f_System_Current < 300)) //-300[A] 와 300[A] 사이 (이외의 값은 FUSE사양 초과).
	{
		//전류값 정상 센싱.
	}
	else
	{
		f_System_Current = 0;
	}

	System_Current_Value = (int)(float)(f_System_Current * 100);	//	X100  A

	System_Info.Electric.s16_Pack_Current = System_Current_Value;

	//return;
}

void get_Current_Value_ORG( void )
{
	if( init_setup == 0 )
	{
		ADS1115_UserConfig_SingleConver(&ADS1115_ADDR_GND,ADS1115_ADDRESS_GND);
		ADS1115_UserConfig_SingleConver(&ADS1115_ADDR_VDD,ADS1115_ADDRESS_VDD);
		init_setup = 1;
	}

	// convert to I Value
	// 2022-10-27
	// HW ======================================================================================================
	// VCC : 5V
	// Default V : 2500.4xx

	////// TEST
	ADS1115_GetVoltage(&ADS1115_ADDR_GND);
	ADS1115_RefreshAllChannel(&ADS1115_ADDR_GND);

	f_System_Current = (-1) * (float)((ADS1115_ADDR_GND.ADS1115_Vol[0] - (ACSoffset)) / mVperAmp);

	if( -300 < f_System_Current && f_System_Current < 300) //-300[A] 와 300[A] 사이 (이외의 값은 FUSE사양 초과).
	{
		//전류값 정상 센싱.
	}
	else
	{
		f_System_Current = 0;
	}

	System_Current_Value = (int)(float)( f_System_Current * 100 );

	System_Info.Electric.s16_Pack_Current = System_Current_Value;

	return;
}

/******************************************************************************
 * GCC Compiler workspace_1.9.0 , TouchGFX4.19 or CubeMX6.5
 * 1. File Name 	| User_Main.c
 * --------------------------------------------------------------------------
 * 2. Autor			| LEE-LAPTOP
 * 3. Date 			| 오후 1:52:53
 * 4. Description 	|
 *
******************************************************************************/

static int Charge_Mode_Delay_Count = 0;
static int Dis_Charge_Mode_Delay_Count = 0;

static int Charge_Mode_Proc = 0;
void charge_mode( void )
{
	switch(Charge_Mode_Proc)
	{
		case IDLE:
		{
			if(System_Info.Electric.s32_Sum_of_1sec_Current > 300)			// 3.00A
			{
				Charge_Mode_Delay_Count++;
				Dis_Charge_Mode_Delay_Count = 0;

				if(Charge_Mode_Delay_Count > 3)								// 3sec
				{
					Charge_Mode_Delay_Count = 0;
					Charge_Status = CHARGE;
					System_Info.Status.Charge_Status = CHARGE;

					Charge_Mode_Proc = CHARGE;
				}
			}
			else if(System_Info.Electric.s32_Sum_of_1sec_Current < -300)	// -3.00A
			{
				Charge_Mode_Delay_Count = 0;
				Dis_Charge_Mode_Delay_Count++;

				if(Dis_Charge_Mode_Delay_Count > 3)							// 3sec
				{
					Dis_Charge_Mode_Delay_Count = 0;
					Charge_Status = DISCHARGE;
					System_Info.Status.Charge_Status = DISCHARGE;

					Charge_Mode_Proc = DISCHARGE;
				}
			}
			else
			{
				Charge_Status = IDLE;//idle
				System_Info.Status.Charge_Status = IDLE;

				Charge_Mode_Delay_Count = 0;
				Dis_Charge_Mode_Delay_Count = 0;

				Charge_Mode_Proc = IDLE;
			}
		}
		break;

		case CHARGE:// now charge mode -? idle mode
		{
			if(System_Info.Electric.s32_Sum_of_1sec_Current > 300)// -3A
			{

			}
			else
			{
				Charge_Status = IDLE;//idle mode
				System_Info.Status.Charge_Status = IDLE;

				Charge_Mode_Delay_Count = 0;
				Dis_Charge_Mode_Delay_Count = 0;

				Charge_Mode_Proc = IDLE;
			}
		}
		break;

		case DISCHARGE:// now dis-charge mode -> idle
		{
			if(System_Info.Electric.s32_Sum_of_1sec_Current < -300)// -3A
			{

			}
			else
			{
				Charge_Status = IDLE;//idle mode
				System_Info.Status.Charge_Status = IDLE;

				Charge_Mode_Delay_Count = 0;
				Dis_Charge_Mode_Delay_Count = 0;

				Charge_Mode_Proc = IDLE;
			}
		}
		break;

		default:
		break;
	}

	//return;
}

/******************************************************************************
 * GCC Compiler workspace_1.9.0 , TouchGFX4.19 or CubeMX6.5
 * 1. File Name 	| User_Main.c
 * --------------------------------------------------------------------------
 * 2. Autor			| LEE-LAPTOP
 * 3. Date 			| 오후 7:46:38
 * 4. Description 	|
 *
******************************************************************************/
int relay_mode = 1;
int relay_mode_count = 0;

void relay_Process()
{
	switch(relay_mode)
	{
		//case 0:
		//{
			//HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
			//HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
			//HAL_GPIO_WritePin(GPIOA,u_PRE_G_Pin, GPIO_PIN_RESET);

			//relay_mode_count++;
			//if(relay_mode_count >= 3)
			//{
				//relay_mode_count = 0;
				//relay_mode = 1;
			//}
		//}
		//break;

		case 1:
		{
			HAL_GPIO_WritePin(GPIOA, u_PRE_G_Pin, GPIO_PIN_SET);
			System_Info.Status.u16_BMS_status |= _STATUS_PRECHARGE_FET;
			relay_mode_count++;

			if(relay_mode_count >= 1)
			{
				relay_mode_count = 0;
				relay_mode = 2;
			}
		}
		break;

		case 2:
		{
			System_Info.Status.u16_BMS_status |= _STATUS_CHARGE_FET;
			System_Info.Status.u16_BMS_status |= _STATUS_DISCHARGE_FET;
			System_Info.Status.u16_BMS_status &= ~(_STATUS_PRECHARGE_FET);

			HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, u_PRE_G_Pin, GPIO_PIN_RESET);


			relay_mode = 100;
		}
		break;

		case 100:// wait
		{
			if((System_Info.Fault.OVP || System_Info.Fault.UVP))
			{
				if( System_Info.Fault.OVP )// ovp
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_CHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_OVP ###########\r\n");
				}

				if( System_Info.Fault.UVP )// uvp
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_DISCHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW -f_UVP ###########\r\n");
				}
			}
			else {}

			if((System_Info.Fault.COCP || System_Info.Fault.DOCP))
			{
				if( System_Info.Fault.COCP )// ocp
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_CHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_OCP ###########\r\n");
				}

				if( System_Info.Fault.DOCP )// docp
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_DISCHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW -f_DOCP ###########\r\n");
				}
			}
			else {}

			if((System_Info.Fault.COTP || System_Info.Fault.CUTP ||
					System_Info.Fault.DOTP || System_Info.Fault.DUTP))
			{
				if(System_Info.Fault.COTP)
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_CHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_COTP ###########\r\n");
				}

				if(System_Info.Fault.CUTP)
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_CHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_CUTP ###########\r\n");
				}

				if(System_Info.Fault.DOTP)
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_DISCHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW -f_DOTP ###########\r\n");
				}

				if(System_Info.Fault.DUTP)
				{
					System_Info.Status.u16_BMS_status &= ~(_STATUS_DISCHARGE_FET);
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW-f_DUTP###########\r\n");
				}
			}
			else {}

			//if(System_Info.Fault.IMBP)	//230907
			//if((System_Info.Fault.CPF) || (System_Info.Fault.DPF))
			if(System_Info.Fault.CPF)
			{
				System_Info.Status.u16_BMS_status &= ~(_STATUS_CHARGE_FET);
				System_Info.Status.u16_BMS_status &= ~(_STATUS_DISCHARGE_FET);
				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
				//System_Info.Status.DFET = 0;	System_Info.Status.CFET = 0;
				log_debug("###### CHG,DSG PIN LOW -f_CPF #######\r\n");
			}
			else {}
			if(System_Info.Fault.DPF)
			{
				System_Info.Status.u16_BMS_status &= ~(_STATUS_CHARGE_FET);
				System_Info.Status.u16_BMS_status &= ~(_STATUS_DISCHARGE_FET);
				HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
				//System_Info.Status.DFET = 0;	System_Info.Status.CFET = 0;
				log_debug("###### CHG,DSG PIN LOW -f_DPF #######\r\n");
			}
			else {}


		}
		break;

		default:
		break;
	}
}

void relay_Process_ORG()
{
	switch(relay_mode)
	{
		case 0:
		{
			HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA,u_PRE_G_Pin, GPIO_PIN_RESET);

			relay_mode_count++;
			if(relay_mode_count >= 3)
			{
				relay_mode_count = 0;
				relay_mode = 1;
			}
		}
		break;

		case 1:
		{
			HAL_GPIO_WritePin(GPIOA,u_PRE_G_Pin, GPIO_PIN_SET);
			relay_mode_count++;

			if(relay_mode_count >= 1)// 100msec x 10=1sec
			{
				relay_mode_count = 0;
				relay_mode = 2;
			}
		}
		break;

		case 2:
		{
			HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, u_PRE_G_Pin, GPIO_PIN_RESET);

			relay_mode = 100;
		}
		break;

		case 100:// wait
		{
			if((System_Info.Fault.OVP || System_Info.Fault.UVP))
			{
				if( System_Info.Fault.OVP )// ovp
				{
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_OVP ###########\r\n");
				}

				if( System_Info.Fault.UVP )// uvp
				{
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW -f_UVP ###########\r\n");
				}
			}


			if((System_Info.Fault.COCP || System_Info.Fault.DOCP))
			{
				if( System_Info.Fault.COCP )// ocp
				{
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_OCP ###########\r\n");
				}

				if( System_Info.Fault.DOCP )// docp
				{
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW -f_DOCP ###########\r\n");
				}
			}

			if
			((System_Info.Fault.COTP || System_Info.Fault.CUTP ||
					System_Info.Fault.DOTP || System_Info.Fault.DUTP))
			{
				if(System_Info.Fault.COTP)
				{
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_COTP ###########\r\n");
				}

				if(System_Info.Fault.CUTP)
				{
					HAL_GPIO_WritePin(GPIOA, u_CHG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### CHG PIN LOW -f_CUTP ###########\r\n");
				}

				if(System_Info.Fault.DOTP)
				{
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW -f_DOTP ###########\r\n");
				}

				if(System_Info.Fault.DUTP)
				{
					HAL_GPIO_WritePin(GPIOA, u_DSG_G_Pin, GPIO_PIN_RESET);
					log_debug("######### DSG PIN LOW-f_DUTP###########\r\n");
				}
			}
		}
		break;

		default:
		break;
	}
}

void send_uart(uint8_t *buffer, int str_length, int timeout)
{

	HAL_GPIO_WritePin(u_485_CON_GPIO_Port, u_485_CON_Pin, GPIO_PIN_RESET);

	if(HAL_UART_Transmit(&huart6, buffer, str_length, timeout) != HAL_OK)
	{
			    //log_debug("error");
				//uart_errorcount++;
	}
	HAL_GPIO_WritePin(u_485_CON_GPIO_Port, u_485_CON_Pin, GPIO_PIN_SET);

	//return;
}


extern void get_Cell_Datas();

void Sensing_10times() //초기에 쓰레기값 날라오는 것을 방지, 전류 보정
{

	for(int i=0; i<10; i++)
	{
		//get_Current_Value( );
		get_Cell_Datas( );
		get_Temp_Data( );
	}

	//ACSoffset = ADS1115_ADDR_GND.ADS1115_Vol[0]; //전류 Offset 보정
}

/*
static int current_count = 0;
static int temp_current = 0;
void Sum_of_1sec_current()
{
	current_count++;
	temp_current += System_Info.Electric.s16_Pack_Current;
	if(current_count>=40)
	{
		//log_debug("\r\n sensing count : %d\r\n",current_count);
		System_Info.Electric.s32_Sum_of_1sec_Current = (int)temp_current/40;
		temp_current = 0;
		current_count = 0;

	}
}
*/
/******************************************************************************
 * GCC Compiler workspace_1.9.0 , TouchGFX4.19 or CubeMX6.5
 * 1. File Name 	| User_Main.c
 * --------------------------------------------------------------------------
 * 2. Autor			| LEE-LAPTOP
 * 3. Date 			| 오후 2:11:33
 * 4. Description 	|
 *
******************************************************************************/
extern void Task_10ms(void);
extern void Task_25ms(void);
extern void Task_100ms(void);
extern void Task_200ms(void);
extern void Task_1sec(void);
extern void Task_5sec(void);
extern void Task_RealTime(void);

void User_Process(void)
{
	pf_process_enable = 1;		// 230726 추가
	tp_process_enable = 1;
	cp_process_enable = 1;
	vp_process_enable = 1;

	//Task_10ms();

	Task_25ms();

	Task_100ms();

	Task_200ms();


	Task_1sec();

	Task_5sec();

	Task_RealTime();
}














