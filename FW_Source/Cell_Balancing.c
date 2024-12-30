/*
 * Cell_Balancing.c
 *
 *  Created on: 2023. 1. 12.
 *      Author: king
 */

#include "Cell_Balancing.h"
#include "System_Info.h"
#include "Parameter.h"
#include "Library/LTC6803.h"

_CELL_BALANCE_DATA	_CBD;
//--------------------------------------------------------------------------------------------
/*
 * 					LTC6803 Configuration (CFG) Register Group
 *				BIT7	BIT6	BIT5	BIT4	BIT3	BIT2	BIT1	BIT0
 * 		CFGR0	WDT		GPIO2	GPIO1	LVLPL	CELL10	CDC[2]	CDC[1]	CDC[0]
 * 		CFGR1	DCC8	DCC7	DCC6	DCC5	DCC4	DCC3	DCC2	DCC1
 * 		CFGR2	MC4I	MC3I	MC2I	MC1I	DCC12	DCC11	DCC10	DCC9
 * 		CFGR3	MC12I 	MC11I 	MC10I 	MC9I 	MC8I 	MC7I 	MC6I 	MC5I
 * 		CFGR4	VUV[7]	VUV[6]	VUV[5]	VUV[4]	VUV[3]	VUV[2]	VUV[1]	VUV[0]
 * 		CFGR5	VOV[7]	VOV[6]	VOV[5]	VOV[4]	VOV[3]	VOV[2]	VOV[1]	VOV[0]
 */
//---------------------------------------------------------------------------------------------
//	KHW 23.06.27~
//	20개 전체 밸런싱
//-----------------
void LTC6803_WRCFGR_Setup(_CELL_IndexState IndexState, _CFGR_BitState BitState )
{
	if(BitState == _CFGR_SET)
	{	//	DCC Discharge Cell x BIT SET
		switch(IndexState)
		{	//	CS0 Cell_01::10		// Cell DDC BIT_SET								// Cell DDC BIT FLAG_SET
			case _CELL01:	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_01_11_DCC);	_CBD.DCC_FLAG |= (1 << _CELL01);	break;
			case _CELL02:	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_02_12_DCC);	_CBD.DCC_FLAG |= (1 << _CELL02);	break;
			case _CELL03: 	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_03_13_DCC);	_CBD.DCC_FLAG |= (1 << _CELL03);	break;
			case _CELL04:	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_04_14_DCC);	_CBD.DCC_FLAG |= (1 << _CELL04);	break;
			case _CELL05:	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_05_15_DCC);	_CBD.DCC_FLAG |= (1 << _CELL05);	break;
			case _CELL06: 	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_06_16_DCC);	_CBD.DCC_FLAG |= (1 << _CELL06);	break;
			case _CELL07:	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_07_17_DCC);	_CBD.DCC_FLAG |= (1 << _CELL07);	break;
			case _CELL08:	_CBD.CS0_WRCFGR[CFGR1] |= (1 << _CELL_08_18_DCC);	_CBD.DCC_FLAG |= (1 << _CELL08);	break;
			case _CELL09: 	_CBD.CS0_WRCFGR[CFGR2] |= (1 << _CELL_09_19_DCC);	_CBD.DCC_FLAG |= (1 << _CELL09);	break;
			case _CELL10:	_CBD.CS0_WRCFGR[CFGR2] |= (1 << _CELL_10_20_DCC);	_CBD.DCC_FLAG |= (1 << _CELL10);	break;
			//	CS1 Cell_11::20		// Cell DDC BIT_SET								// Cell DDC BIT FLAG_SET
			case _CELL11:	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_01_11_DCC);	_CBD.DCC_FLAG |= (1 << _CELL11);	break;
			case _CELL12:	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_02_12_DCC);	_CBD.DCC_FLAG |= (1 << _CELL12);	break;
			case _CELL13: 	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_03_13_DCC);	_CBD.DCC_FLAG |= (1 << _CELL13);	break;
			case _CELL14:	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_04_14_DCC);	_CBD.DCC_FLAG |= (1 << _CELL14);	break;
			case _CELL15:	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_05_15_DCC);	_CBD.DCC_FLAG |= (1 << _CELL15);	break;
			case _CELL16: 	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_06_16_DCC);	_CBD.DCC_FLAG |= (1 << _CELL16);	break;
			case _CELL17:	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_07_17_DCC);	_CBD.DCC_FLAG |= (1 << _CELL17);	break;
			case _CELL18:	_CBD.CS1_WRCFGR[CFGR1] |= (1 << _CELL_08_18_DCC);	_CBD.DCC_FLAG |= (1 << _CELL18);	break;
			case _CELL19: 	_CBD.CS1_WRCFGR[CFGR2] |= (1 << _CELL_09_19_DCC);	_CBD.DCC_FLAG |= (1 << _CELL19);	break;
			case _CELL20:	_CBD.CS1_WRCFGR[CFGR2] |= (1 << _CELL_10_20_DCC);	_CBD.DCC_FLAG |= (1 << _CELL20);	break;
		}
	}
	else
	{	//	DCC Discharge Cell x BIT RESET
		switch(IndexState)
		{	//	CS0 Cell_01::10		// Cell DDC BIT_RESET							// Cell DDC BIT FLAG_RESET
			case _CELL01:	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_01_11_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL01);	break;
			case _CELL02:	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_02_12_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL02);	break;
			case _CELL03: 	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_03_13_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL03);	break;
			case _CELL04:	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_04_14_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL04);	break;
			case _CELL05:	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_05_15_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL05);	break;
			case _CELL06: 	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_06_16_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL06);	break;
			case _CELL07:	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_07_17_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL07);	break;
			case _CELL08:	_CBD.CS0_WRCFGR[CFGR1] &= ~(1 << _CELL_08_18_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL08);	break;
			case _CELL09: 	_CBD.CS0_WRCFGR[CFGR2] &= ~(1 << _CELL_09_19_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL09);	break;
			case _CELL10:	_CBD.CS0_WRCFGR[CFGR2] &= ~(1 << _CELL_10_20_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL10);	break;
			//	CS1 Cell_11::20		// Cell DDC BIT_RESET							// Cell DDC BIT FLAG_RESET
			case _CELL11:	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_01_11_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL11);	break;
			case _CELL12:	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_02_12_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL12);	break;
			case _CELL13: 	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_03_13_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL13);	break;
			case _CELL14:	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_04_14_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL14);	break;
			case _CELL15:	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_05_15_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL15);	break;
			case _CELL16: 	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_06_16_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL16);	break;
			case _CELL17:	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_07_17_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL17);	break;
			case _CELL18:	_CBD.CS1_WRCFGR[CFGR1] &= ~(1 << _CELL_08_18_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL18);	break;
			case _CELL19: 	_CBD.CS1_WRCFGR[CFGR2] &= ~(1 << _CELL_09_19_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL19);	break;
			case _CELL20:	_CBD.CS1_WRCFGR[CFGR2] &= ~(1 << _CELL_10_20_DCC);	_CBD.DCC_FLAG &= ~(1 << _CELL20);	break;
		}
	}
}

void Cell_Balancing(void)
{
	int	cell_dV	= 0;	// 각 Cell 비교 전압	V_Cell_XX  - MIN_V(Cell)

#if(1)
	if(System_Info.Status.Charge_Status == CHARGE)
#endif

	{
		if((System_Info.Cell_Voltage.u16_Avg_Cell_Voltage  > Parameter.s16_CB_AVR_V)	//	AVR_V > 3700 mV
		&& (System_Info.Cell_Voltage.u16_Diff_Cell_Voltage > Parameter.s16_CB_SDV))		//  dV	  >   30 mV
		{	// Start Cell Balance Check
			for(int i=0; i<20; i++)
			{	// Cell Voltage Check
				cell_dV = System_Info.Cell_Voltage.u16_Cell_Voltage[i] - System_Info.Cell_Voltage.u16_Min_Cell_Voltage;
				if(cell_dV > Parameter.s16_CB_SDV)										// CELL_V >   30mV	CFGR
				{
					LTC6803_WRCFGR_Setup(i, _CFGR_SET );								// CFGR SETUP
				}
			}
			System_Info.Balancing.is_Cell_Balancing_ON = SET;
		}
	}

#if(1)
	else
	{
		_CBD.DCC_FLAG = 0;
	}
#endif

	if( _CBD.DCC_FLAG )
	{	//	Cell Balance Setup
		System_Info.Balancing.u32_Balancing_Cell = _CBD.DCC_FLAG;

		LTC6803_wrcfg( 1, _CBD.CS0_WRCFGR[CFGR1], _CBD.CS0_WRCFGR[CFGR2]);
		LTC6803_wrcfg( 0, _CBD.CS1_WRCFGR[CFGR1], _CBD.CS1_WRCFGR[CFGR2]);
		for(int i=0; i<20; i++)
		{	// 각 Cell 전압 비교
			cell_dV = System_Info.Cell_Voltage.u16_Cell_Voltage[i] - System_Info.Cell_Voltage.u16_Min_Cell_Voltage;
			if(cell_dV < Parameter.s16_CB_EDV)											//	CELL_V <  20mV
			{
				LTC6803_WRCFGR_Setup(i, _CFGR_RESET );
			}
		}
	}
	else
	{
		_CBD.CS0_WRCFGR[CFGR1] = _CBD.CS0_WRCFGR[CFGR2] = 0;
		_CBD.CS1_WRCFGR[CFGR1] = _CBD.CS1_WRCFGR[CFGR2] = 0;
		System_Info.Balancing.is_Cell_Balancing_ON = OFF;
	}
}
//---------------------------------------------------------------------------------------------

#if(0)
//	1개씩 밸런싱 원본
void LTC6803_Balancing_Cell(void)
{
	uint8_t dcc1 = 0, dcc2 = 0;
	int temp1 = 0, temp2 = 0;
	int id=0;

	if ( System_Info.Cell_Voltage.u16_Max_Cell_Voltage_Num < 10 )
	{
		id=1;	// #1번 LTC

		temp1 = ( 1 << (System_Info.Cell_Voltage.u16_Max_Cell_Voltage_Num ) );
		dcc1 = temp1;

		if( temp1 > 255)
		{
			temp1 = (temp1 >> 8);
			dcc1 = 0;
			dcc2 = temp1;
		}
	}
	else
	{
		id=2;	// #2번 LTC

		temp2 = ( 1 << ( (System_Info.Cell_Voltage.u16_Max_Cell_Voltage_Num % 10) ) );
		dcc1 = temp2;

		if( temp2 > 255)
		{
			temp2 = (temp2 >> 8);
			dcc1 = 0;
			dcc2 = temp2;
		}
	}

	if(id!=0)
	{
		if(id==1)
		{
			//log_debug("\r\nID - #1 LTC is Now Balancing %d Cell dcc2:%d dcc1:%d \r\n", temp1, dcc2,dcc1);
		}
		else
		{
			//log_debug("\r\nID - #2 LTC is Now Balancing %d Cell dcc2:%d dcc1:%d \r\n", temp2, dcc2,dcc1);
		}
	}

	LTC6803_wrcfg( id, dcc1, dcc2);
}

void Cell_Balancing(void)
{
	int dV = System_Info.Cell_Voltage.u16_Diff_Cell_Voltage;

	if(dV >= Parameter.u8_start_Balancing_dV) //default : dV >= 30
	{
		if((System_Info.Status.Charge_Status == CHARGE) &&
		   (System_Info.Cell_Voltage.u16_Avg_Cell_Voltage > 3700))
		{
			System_Info.Balancing.is_Cell_Balancing_ON = ON;
		}
	}

	if( System_Info.Balancing.is_Cell_Balancing_ON == ON )
	{
		LTC6803_Balancing_Cell();

		System_Info.Balancing.u32_Balancing_Cell = (1 << System_Info.Cell_Voltage.u16_Max_Cell_Voltage_Num);

		if(dV < Parameter.u8_end_Balancing_dV)
		{
			System_Info.Balancing.is_Cell_Balancing_ON 	= OFF;
			System_Info.Balancing.u32_Balancing_Cell 	= 0;
		}
	}
}
#endif
//-------------------------------------------------------------------------------------------------------------
