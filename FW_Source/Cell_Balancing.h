/*
 * Cell_Balancing.h
 *
 *  Created on: 2023. 1. 12.
 *      Author: king
 */

#ifndef CELL_BALANCING_H_
#define CELL_BALANCING_H_

#include "main.h"

//--------------------------------------------------
//	KHW 23.06.27 ~
typedef	enum
{
	_CELL01 = 0,
	_CELL02,
	_CELL03,
	_CELL04,
	_CELL05,
	_CELL06,
	_CELL07,
	_CELL08,
	_CELL09,
	_CELL10,
	_CELL11,
	_CELL12,
	_CELL13,
	_CELL14,
	_CELL15,
	_CELL16,
	_CELL17,
	_CELL18,
	_CELL19,
	_CELL20
} _CELL_IndexState;

typedef enum
{
	_CFGR_RESET = 0,
	_CFGR_SET
}	_CFGR_BitState;

typedef	enum
{
	CFGR0 = 0,
	CFGR1,
	CFGR2,
	CFGR3,
	CFGR4,
	CFGR5,
} _WRCFG_GroupState;

typedef enum
{
	_CELL_01_11_DCC = 0,
	_CELL_02_12_DCC,
	_CELL_03_13_DCC,
	_CELL_04_14_DCC,
	_CELL_05_15_DCC,
	_CELL_06_16_DCC,
	_CELL_07_17_DCC,
	_CELL_08_18_DCC
}	_CFGR1_GroupState;
typedef enum
{
	_CELL_09_19_DCC = 0,
	_CELL_10_20_DCC,
	_DCC11,
	_DCC12,
	_MC1I,
	_MC2I,
	_MC3I,
	_MC4I
}	_CFGR2_GroupState;

typedef struct
{
	uint8_t		FLAG;				//	Balancing flag
	uint8_t		CS0_WRCFGR[6];		//	Cell 01 ~ 10 CFGR
	uint8_t		CS1_WRCFGR[6];		//	Cell 11 ~ 20 CFGR
	uint32_t	DCC_FLAG;			//
}	_CELL_BALANCE_DATA;
//------------------------------------------
extern _CELL_BALANCE_DATA	_CBD;	//
//------------------------------------------

extern void Cell_Balancing();

#endif /* CELL_BALANCING_H_ */
