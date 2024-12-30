/*
 * Voltage.c
 *
 */

#include "Library\LTC6803.h"
#include "System_Info.h"

extern float cell_codes[30];

static uint16_t cell_Voltage[10];
static float *cell_ptr;

static uint16_t Get_Cell_Voltage_Value[20];
static uint32_t Get_Total_Voltage_Value;

static int Cell_number = 7;

void get_Cell_Datas(void)
{
	int i=0;
	for(int i=0; i<7; i++) cell_Voltage[i] = 0;

	LTC6803_wrcfg(1,0,0 );
	LTC6803_stcvad(1);
	cell_ptr = (float *)&cell_codes[0];
	LTC6803_rdcv(1, cell_ptr );

	for(i=0;i<7;i++)
	{
		cell_Voltage[i] = *(cell_ptr+i) * 1000;
		Get_Cell_Voltage_Value[i] = cell_Voltage[i];
	}

	for(int i=0; i<0; i++) cell_Voltage[i] = 0;

	LTC6803_wrcfg(0,0,0 );
	LTC6803_stcvad(0);
	cell_ptr = (float *)&cell_codes[0];
	LTC6803_rdcv(0, cell_ptr );

	for(i=0;i<0;i++)
	{
		cell_Voltage[i] = *(cell_ptr+i) * 1000;
		Get_Cell_Voltage_Value[10+i] = cell_Voltage[i];
	}

	for(int i=0; i<Cell_number;i++)
	{
		System_Info.Cell_Voltage.u16_Cell_Voltage[i] = Get_Cell_Voltage_Value[i];
	}
}


void get_Cell_Datas_200ms (void)
{
	int i=0;
	for(int i=0; i<7; i++) cell_Voltage[i] = 0;

	LTC6803_wrcfg(1,0,0 );
	LTC6803_stcvad(1);
	cell_ptr = (float *)&cell_codes[0];
	LTC6803_rdcv(1, cell_ptr );

	for(i=0;i<7;i++)
	{
		cell_Voltage[i] = *(cell_ptr+i) * 1000;
		Get_Cell_Voltage_Value[i] = cell_Voltage[i];
	}

	for(int i=0; i<0; i++) cell_Voltage[i] = 0;

	LTC6803_wrcfg(0,0,0 );
	LTC6803_stcvad(0);
	cell_ptr = (float *)&cell_codes[0];
	LTC6803_rdcv(0, cell_ptr );

	for(i=0;i<0;i++)
	{
		cell_Voltage[i] = *(cell_ptr+i) * 1000;
		Get_Cell_Voltage_Value[10+i] = cell_Voltage[i];
	}

	for(int i=0; i<Cell_number;i++)
	{
		//System_Info.Cell_Voltage.u16_Cell_Voltage[i] = Get_Cell_Voltage_Value[i];

		System_Info.Cell_Voltage.u16_Cell_Voltage_200ms[i] = Get_Cell_Voltage_Value[i];
	}
}

static int change = 0;
void get_Cell_Datas_ORG(void)
{
	if( change == 1 )
	{
		LTC6803_wrcfg(1,0,0 );
		LTC6803_stcvad(1);
		cell_ptr = (float *)&cell_codes[0];
		LTC6803_rdcv( 1, cell_ptr );
		change = 0;
	}
	else
	{
		LTC6803_wrcfg(0,0,0 );
		LTC6803_stcvad(0);
		cell_ptr = (float *)&cell_codes[0];
		LTC6803_rdcv( 0, cell_ptr );
		change = 1;
	}

	for(int i=0; i<10; i++)cell_Voltage[i] = 0;
	for(int i=0; i<10; i++)
	{
		cell_Voltage[i] = *(cell_ptr+i) * 1000;

		#if(0)
		log_debug("CELLV-%d=>(%d): %d V \r\n",change,i,cell_Voltage[i]);
		#endif

		if( change == 0 )
		{
			Get_Cell_Voltage_Value[i] = cell_Voltage[i];
		}
		else
		{
			Get_Cell_Voltage_Value[10+i] = cell_Voltage[i];
		}
	}

	for(int i=0; i<Cell_number;i++)
	{
		System_Info.Cell_Voltage.u16_Cell_Voltage[i] = Get_Cell_Voltage_Value[i];
	}

	return;
}

void set_Cell_datas( void )
{
	Get_Total_Voltage_Value = 0;
	int min = 5000;
	int max = 0;
	int i =0;

	for (i=0; i<Cell_number; i++)
	{
		Get_Total_Voltage_Value += System_Info.Cell_Voltage.u16_Cell_Voltage[i];

		if (System_Info.Cell_Voltage.u16_Cell_Voltage[i] < min)
		{
			min = System_Info.Cell_Voltage.u16_Cell_Voltage[i];
			System_Info.Cell_Voltage.u16_Min_Cell_Voltage_Num = i+1;
		}
		if (System_Info.Cell_Voltage.u16_Cell_Voltage[i] > max)
		{
			max = System_Info.Cell_Voltage.u16_Cell_Voltage[i];
			System_Info.Cell_Voltage.u16_Max_Cell_Voltage_Num = i+1;
		}
	}

	System_Info.Electric.u32_Pack_Voltage 		= 	Get_Total_Voltage_Value;

	System_Info.Electric.u16_Pack_In_Voltage 	=	(int)(System_Info.Electric.u32_Pack_Voltage / 10);
	System_Info.Electric.u16_Pack_Out_Voltage 	=	(int)(System_Info.Electric.u32_Pack_Voltage / 10);

	System_Info.Cell_Voltage.u16_Avg_Cell_Voltage	= (int)Get_Total_Voltage_Value/Cell_number;
	System_Info.Cell_Voltage.u16_Min_Cell_Voltage	= min;
	System_Info.Cell_Voltage.u16_Max_Cell_Voltage	= max;
	System_Info.Cell_Voltage.u16_Diff_Cell_Voltage	= max-min;

}
