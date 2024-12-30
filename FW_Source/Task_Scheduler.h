/*
 * Task_scheduler.h
 *
 *  Created on: Dec 21, 2022
 *      Author: king
 */

#ifndef INC_TASK_SCHEDULER_H_
#define INC_TASK_SCHEDULER_H_

#include "main.h"
#include "stm32f2xx_it.h"



void initialize(void);
void Task_10ms(void);
void Task_25ms(void);
void Task_100ms(void);
void Task_1sec(void);
void Task_RealTime(void);

#endif /* INC_TASK_SCHEDULER_H_ */
