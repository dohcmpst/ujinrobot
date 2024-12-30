/*
 * CAN.h
 *
 *  Created on: Dec 21, 2022
 *      Author: king
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "main.h"

extern void Send_can_Ext( uint8_t *Data, uint32_t ExtId);
extern void Send_can_Std( uint8_t *Data, uint32_t StdId);


#endif /* INC_CAN_H_ */
