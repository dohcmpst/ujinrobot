/*
 * LB_CAN_Protocol.c
 *
 *  Created on: Jan 5, 2023
 *      Author: king
 */

#include "LB_CAN_Protocol.h"
#include "../Library\CAN.h"
#include "../System_Info.h"

uint8_t u8_SOC_frame[8]={0,};
void Send_CAN_100ms_ORG()
{
	u8_SOC_frame[0] = System_Info.Lifecycle.u8_SOC;
	u8_SOC_frame[1] = 0x00;
	u8_SOC_frame[2] = 0x00;
	u8_SOC_frame[3] = 0x00;
	u8_SOC_frame[4] = 0x00;
	u8_SOC_frame[5] = 0x00;
	u8_SOC_frame[6] = 0x00;
	u8_SOC_frame[7] = 0x00;

	Send_can_Std( u8_SOC_frame, 0x014 );
}

void Send_CAN_100ms()
{
	u8_SOC_frame[0] = 0x02;//System_Info.Lifecycle.u8_SOC;
	u8_SOC_frame[1] = 0x02;
	u8_SOC_frame[2] = 0x00;
	u8_SOC_frame[3] = 0x00;
	u8_SOC_frame[4] = System_Info.Lifecycle.u8_SOC;
	//u8_SOC_frame[4] = 0x64; //240327 충전기 TEST용
	u8_SOC_frame[5] = 0x00;
	u8_SOC_frame[6] = 0x00;
	u8_SOC_frame[7] = 0x00;

	Send_can_Ext( u8_SOC_frame, 0x10261051 );
}
