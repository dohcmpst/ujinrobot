/*
 * LTC6803.h
 *
 *  Created on: 2022. 12. 23.
 *      Author: king
 */

#ifndef LTC6803_H_
#define LTC6803_H_

#include "main.h"

#define PEC_POLY 		7
#define BYTES_IN_REG  	6
#define CMD_MAX 		3
#define CMD_LEN 		9



//static uint8_t pec8_calc(uint8_t len, uint8_t *data);

void LTC6803_wrcfg( int id, uint8_t dcc1, uint8_t dcc2 );
int8_t LTC6803_rdcv(int id, float *cell_ptr);
void LTC6803_stcvad( int id);

void get_Cell_Datas(void);


#endif /* LTC6803_H_ */
