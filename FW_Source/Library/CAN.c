/*
 * CAN.c
 *
 *  Created on: Dec 21, 2022
 *      Author: king
 */

#include "CAN.h"

extern CAN_HandleTypeDef hcan2;

void Send_can_Ext( uint8_t *Data, uint32_t ExtId)
{
    uint32_t TxMailbox;
    CAN_TxHeaderTypeDef   TxHeader;

	TxHeader.StdId = 0x000;
	TxHeader.ExtId = ExtId;
	TxHeader.DLC = 8;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.RTR = CAN_RTR_DATA;

    TxHeader.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage( &hcan2, &TxHeader, Data, &TxMailbox );

}

void Send_can_Std( uint8_t *Data, uint32_t StdId)
{
    uint32_t TxMailbox;
    CAN_TxHeaderTypeDef   TxHeader;

	TxHeader.StdId = StdId;
	TxHeader.ExtId = 0x0000000;
	TxHeader.DLC = 8;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

    TxHeader.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage( &hcan2, &TxHeader, Data, &TxMailbox );

}

#if(0)

void Send_can_Std( uint8_t *Data, uint32_t ExtID)
{
    uint32_t TxMailbox;
    CAN_TxHeaderTypeDef   TxHeader;

	TxHeader.StdId = 0x00;
	TxHeader.ExtId = ExtID;
	TxHeader.DLC = 8;
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.RTR = CAN_RTR_DATA;

    TxHeader.TransmitGlobalTime = DISABLE;

    HAL_CAN_AddTxMessage( &hcan2, &TxHeader, Data, &TxMailbox );

}
#endif
