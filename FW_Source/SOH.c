/*
 * SOH.c
 *
 *  Created on: 2022. 12. 26.
 *      Author: king
 */

#include "SOH.h"
#include "System_Info.h"

uint16_t u16_cycle_count = 0;
uint16_t SOH = 100;

void __test_SOH__(void)
{
	System_Info.Lifecycle.u16_Cycle_Count = u16_cycle_count;
	System_Info.Lifecycle.u16_SOH = SOH;
}

void cycle_count()
{

}
