/*
 * LTC6803.c
 *
 *  Created on: 2022. 12. 23.
 *      Author: king
 */

#include <stdlib.h>
#include <stdio.h>
#include "LTC6803.h"

extern SPI_HandleTypeDef hspi1;

static uint8_t 	pec8_calc(uint8_t len, uint8_t *data);
static uint8_t config[BYTES_IN_REG];
static uint8_t rx[30];

float cell_codes[30]; //Voltage.c와 같이 사용

// LTC6803 Function START

void LTC6803_wrcfg( int id, uint8_t dcc1, uint8_t dcc2 )
{
	uint8_t *cmd;
	uint16_t cfg_pec;
	uint8_t i, cmd_index = 0; //command counter

	int len = sizeof(uint8_t);
	len *= CMD_LEN;
	cmd = malloc(len);

	cmd[cmd_index++] = 0x01;
	cmd[cmd_index++] = 0xc7;

	config[0] = 0x02;
	config[1] = dcc1;
	config[2] = dcc2;

	if(id == 1) config[3] = 0xc0;
	else        config[3] = 0xc0;

	config[4] = 0x98;
	config[5] = 0xb0;

	for (i = 0; i < BYTES_IN_REG; i++)
	{
		cmd[cmd_index] = config[i];
		cmd_index += 1;
	}

	cfg_pec = pec8_calc(BYTES_IN_REG, &config[0]);    // calculating the PEC for each ICs configuration register data
	cmd[cmd_index ] = (uint8_t)cfg_pec;
	cmd_index += 1;

	switch(id)
	{
		case 0 : HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_RESET); break;
		case 1 : HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_RESET); break;
		default : break;
	}
/*
	if( id == 1 )
	{
		HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_RESET);
	}
*/
   HAL_Delay(1);

	HAL_SPI_Transmit(&hspi1, cmd, CMD_LEN, 1000);

	switch(id)
	{
		case 1 : HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_SET); break;
		case 0 : HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_SET); break;
		default : break;
	}

   free(cmd);
}

int8_t LTC6803_rdcv(int id, float *cell_ptr)
{
	int i, data_counter =0;
	int 	pec_error = 0;
	uint8_t data_pec = 0;
	uint8_t received_pec = 0;
	uint8_t cmd[4];
	uint8_t cmd_index = 0;

	cmd[cmd_index++] = 0x04;
	cmd[cmd_index++] = 0xDC;

	switch(id)
	{
		case 0 : HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_RESET); break;
		case 1 : HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_RESET); break;
		default : break;
	}

/*	if(id==1)
	{
		HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_RESET);
	}
*/
	if( HAL_SPI_Transmit(&hspi1, cmd, 2, 1000) != HAL_OK )
	{
		//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, 1);
	}

	if( HAL_SPI_Receive(&hspi1, rx, 19, 1000) != HAL_OK )
	{
		//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, 1);
	}
	switch(id)
	{
		case 1 : HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_SET); break;
		case 0 : HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_SET);break;
		default : break;
	}

	received_pec =  rx[18];
	data_pec = pec8_calc(18, &rx[0]);
	if (received_pec != data_pec)
	{
		// LTC fault : 2021-11-11
		log_debug("--------------- LTC ERROR --------------\n\r");
		pec_error = -1;
	}
	else
	{

	}

	//int cell_counter = 0;
	data_counter = 0;
	uint16_t temp;
	float ftmp;

	for (i = 0; i<6; i++)
	{
		temp = rx[data_counter]; //0-7 bit
		temp |= (uint16_t)(rx[data_counter+1]&0x0f)<<8; //8-11 bit
		data_counter += 1;
		ftmp = (float)(temp - 0x200); //0-volt
		ftmp *= 0.0015;
		cell_codes[2*i] = ftmp;
		*(cell_ptr+2*i) = ftmp;

		temp = (uint16_t)(rx[data_counter]&0xf0) >> 4;
		temp |= (uint16_t)(rx[data_counter+1])<<4; //4-11 bit
		data_counter += 2;
		ftmp = (float)(temp - 0x200); //0-volt
		ftmp *= 0.0015; cell_codes[2*i+1] = ftmp;
		*(cell_ptr+2*i+1) = ftmp;
	}

	//free(rx_data);
	return(pec_error);
}

uint8_t pec8_calc(uint8_t len, uint8_t *data)
{
  uint8_t  remainder = 0x41;//PEC_SEED;
  /*
   * Perform modulo-2 division, a byte at a time.
   */
  for (int byte = 0; byte < len; ++byte)
  {
    /*
     * Bring the next byte into the remainder.
     */
    remainder ^= data[byte];

    /*
     * Perform modulo-2 division, a bit at a time.
     */
    for (uint8_t bit = 8; bit > 0; --bit)
    {
      /*
       * Try to divide the current data bit.
       */
      if (remainder & 128)
      {
        remainder = (remainder << 1) ^ PEC_POLY;
      }
      else
      {
        remainder = (remainder << 1);
      }
    }
  }

  /*
   * The final remainder is the CRC result.
   */
  return (remainder);
}

void LTC6803_stcvad( int id)
{
	uint8_t tx[2];

	switch(id)
	{
		case 0 : HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_RESET); break;
		case 1 : HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_RESET); break;
		default : break;
	}
/*
	if(id==1)
	{
		HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_RESET);
	}
*/
	tx[0] = 0x10;
	HAL_SPI_Transmit(&hspi1, tx, 1, 1000);
	tx[0] = 0xB0;
	HAL_SPI_Transmit(&hspi1, tx, 1, 1000);
	switch(id)
	{
		case 1 : HAL_GPIO_WritePin(CSBI_1_GPIO_Port, CSBI_1_Pin, GPIO_PIN_SET); break;
		case 0 : HAL_GPIO_WritePin(CSBI_2_GPIO_Port, CSBI_2_Pin, GPIO_PIN_SET);break;
		default : break;
	}

	//return;
}


// LTC6803 Function END


