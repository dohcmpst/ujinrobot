/**
  ******************************************************************************
  * @file           : fram.c
  * @brief          : fram
  ******************************************************************************
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "FM25V02.h"


extern SPI_HandleTypeDef hspi3;

void cs_low()
{
	HAL_GPIO_WritePin(FM_CS_GPIO_Port, FM_CS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void cs_high()
{
	HAL_Delay(1);
	HAL_GPIO_WritePin(FM_CS_GPIO_Port, FM_CS_Pin, GPIO_PIN_SET);
}

void SetWriteEnableLatch()
{
	uint8_t opCode = WREN_OP_CODE;

	cs_low();

	HAL_SPI_Transmit(&hspi3, &opCode, 1, 100);		// 100 is timeout

	cs_high();

	HAL_Delay(1);
}

void ResetWriteEnableLatch()
{
	uint8_t opCode = WRDI_OP_CODE;

	cs_low();

	HAL_SPI_Transmit(&hspi3, &opCode, 1, 100);		// 100 is timeout

	cs_high();

	HAL_Delay(1);
}

uint8_t flash_rxbuf[128];
uint16_t F_data=0;

uint16_t flash_read_addr=0;
uint16_t flash_write_addr=0;

// FM25V02 memory size : 0x00 ~ 0x7fff
void FM25V02_write_data(uint16_t addr, uint8_t data[], uint8_t count)
{
	uint8_t value[128];
    // count ũ�� �� ��� �߰�
	value[0] = WRITE_OP_CODE;
	value[1] = (addr & 0xff00) >> 8;
	value[2] = addr & 0x00ff;

	for(int i=0;i<count;i++) value[i+3] = data[i];

	SetWriteEnableLatch();

	cs_low();

	if (HAL_SPI_Transmit(&hspi3, (uint8_t*)&value, count+3, 1) != HAL_OK){
		log_debug("FRAM SPI Error0");
	}

	cs_high();

	ResetWriteEnableLatch();
}

void FM25V02_read_data(uint16_t addr, uint8_t count)
{
	uint8_t value[8];
	// count ũ�� �� ��� �߰�

	value[0] = READ_OP_CODE;
	value[1] = (addr & 0xff00) >> 8;
	value[2] = addr & 0x00ff;

    cs_low();

	if (HAL_SPI_Transmit(&hspi3, (uint8_t *)&value, 3, 1) != HAL_OK)
	{
		log_debug("FRAM SPI Error1");
	}
	if (HAL_SPI_Receive(&hspi3, (uint8_t*)&flash_rxbuf, count, 1) != HAL_OK)
	{
		log_debug("FRAM SPI Error2");
	}

	cs_high();
}

void FM25V02_Memory_Test()
{
	uint8_t opCode = WRITE_OP_CODE;
    uint8_t opCode1 = READ_OP_CODE;
	uint8_t i=0;
	uint8_t value[8];

	F_data++;
    flash_write_addr = F_data;
	value[0] = opCode;
	value[1] = (flash_write_addr >> 8);
	value[2] = flash_write_addr;

	for(i=0;i<10;i++) value[i+3] = F_data; // write data

	// Flash memory write
	SetWriteEnableLatch();

	cs_low();

	HAL_SPI_Transmit(&hspi3, (uint8_t*)&value, 4, 1);

	cs_high();

	ResetWriteEnableLatch();

   // flash memory read
	flash_read_addr = F_data-1;
	value[0] = opCode1;
	value[1] = (flash_read_addr >> 8);
	value[2] = flash_read_addr;
    cs_low();

	/* op code + address*/
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&value, 3, 1);

	HAL_SPI_Receive(&hspi3, (uint8_t*)&flash_rxbuf, 1, 1);

	cs_high();

}



//#include <stdio.h>
//#include "FM25V02.h"
//extern SPI_HandleTypeDef hspi3;

//---------------------------------------------------------------------------
void FM25V02_SetWriteEnableLatch(void);
void FM25V02_ResetWriteEnableLatch(void);
void FM25V02_wr_data(uint16_t addr, uint16_t count, uint8_t *wr_data);
void FM25V02_rd_data(uint16_t addr, uint16_t count, uint8_t *rd_data);
//---------------------------------------------------------------------------
//void FM20V02_READ_TEST(void);
//void FM25V02_Memory_Test(void);
//---------------------------------------------------------------------------
void FM25V02_SetWriteEnableLatch(void)
{
	uint8_t opCode = _FRAM_WREN;

	FM25V02_CS_ENABLE
	HAL_SPI_Transmit(&hspi3, &opCode, 1, 100);		// 100 is timeout
	FM25V02_CS_DISABLE
}

void FM25V02_ResetWriteEnableLatch(void)
{
	uint8_t opCode = _FRAM_WRDI;

	FM25V02_CS_ENABLE
	HAL_SPI_Transmit(&hspi3, &opCode, 1, 10);		// 100 is timeout
	FM25V02_CS_DISABLE
}

// FM25V02 memory size : 0x0000 ~ 0x7fff
void FM25V02_wr_data(uint16_t addr, uint16_t count, uint8_t *wr_data)
{
	uint8_t txd[count+3];

	txd[0] = _FRAM_WRITE;
	txd[1] = (addr & 0xff00) >> 8;
	txd[2] = (addr & 0x00ff);

	for(uint16_t i=0 ; i < count ; i++) txd[i+3] = wr_data[i];

	FM25V02_SetWriteEnableLatch();

    FM25V02_CS_ENABLE
	if(HAL_SPI_Transmit(&hspi3, (uint8_t*)&txd[0], count+3, 10) != HAL_OK)		log_debug("FRAM SPI Error0");
    FM25V02_CS_DISABLE
	FM25V02_ResetWriteEnableLatch();
}

void FM25V02_rd_data(uint16_t addr, uint16_t count , uint8_t *rd_data)
{
	uint8_t txd[10];
	uint8_t	*rxd;
	//uint8_t	rxd[count];

	rxd = rd_data;

	txd[0] = _FRAM_READ;
	txd[1] = (addr & 0xff00) >> 8;
	txd[2] = (addr & 0x00ff);

	FM25V02_CS_ENABLE
	if(HAL_SPI_Transmit(&hspi3, (uint8_t *)&txd[0], 3, 10) != HAL_OK)		log_debug("FRAM SPI Error1");
	if(HAL_SPI_Receive(&hspi3, (uint8_t *)&rxd[0], count, 10) != HAL_OK)	log_debug("FRAM SPI Error2");
	FM25V02_CS_DISABLE
#if (0)
	for(uint16_t i = 0 ; i < count ; i++) log_debug("%02x ",rxd[i]);
	log_debug("\r\n");
	log_debug("\r\n");
	log_debug("\r\n");

#endif
}

#if(0)
void FM25V02_READ_TEST(void)
{
	//uint8_t	twr_data[_FM_256B]={0};
	uint8_t	trd_data[_FM_256B]={0};
	//for(uint16_t i = 0; i< _FM_256B ; i++)	twr_data[i] = i;
	//for(uint16_t j = 0; j < 1 ; j++)
	//{
	//	FM25V02_rd_data(_FM_SADDR+(_FM_256B *j), sizeof(trd_data), trd_data);
	//	FM25V02_wr_data(_FM_SADDR+(_FM_256B *j), sizeof(twr_data), twr_data);
	//}
	FM25V02_rd_data(_FM_SADDR, sizeof(trd_data), trd_data);
}
#endif

