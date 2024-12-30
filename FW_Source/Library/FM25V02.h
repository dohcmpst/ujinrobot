/**
  ******************************************************************************
  * @file           : fram.h
  * @brief          : fram
  ******************************************************************************
  *
  ******************************************************************************
  */
#ifndef __FRAM_H__
#define __FRAM_H__

#include <stdint.h>
#include "main.h"


#define WRSR_OP_CODE				0x01	/* Write Status Register */
#define WRITE_OP_CODE				0x02	/* Write memory data */
#define READ_OP_CODE				0x03	/* Read memory data */
#define WRDI_OP_CODE				0x04	/* Reset write enable latch */
#define RDSR_OP_CODE				0x05	/* Read Status Register */
#define WREN_OP_CODE				0x06	/* Set write enable latch */
#define FSTRD_OP_CODE				0x0b	/* Fast read memory data */
#define SLEEP_OP_CODE				0xb9	/* Enter sleep mode */
#define RDID_OP_CODE				0x9f	/* Read device ID */
#define SNR_OP_CODE	  				0xc3	/* Read S/N */

#define FM25Lx_DUMMY        		0xFF

#define FM_CS_GPIO_Port GPIOA
#define FM_CS_Pin GPIO_PIN_15




void cs_low();
void cs_high();
void SetWriteEnableLatch();
void ResetWriteEnableLatch();
extern void FM25V02_flash_write_data(uint16_t addr, uint8_t data[], uint8_t count);
extern void FM25V02_read_data(uint16_t addr, uint8_t count);
void FM25V02_Memory_Test();


#include <stdint.h>
#include "main.h"

typedef	enum
{
	_FRAM_WRSR	= 0x01,		// 0000 0001b	Write Status Register
	_FRAM_WRITE	= 0x02,		// 0000 0010b	Data Write Memory
	_FRAM_READ	= 0x03,		// 0000 0011b	Read Memory Data
	_FRAM_WRDI	= 0x04,		// 0000 0100b	Write Disable
	_FRAM_RDSR	= 0x05,		// 0000 0101b	Read Status Register
	_FRAM_WREN	= 0x06,		// 0000 0110b	Set Write Enable Latch
	_FRAM_FSTRD	= 0x0B,		// 0000 1011b	Fast Read Memory Data
	_FRAM_RDID	= 0x9F,		// 1001 1111b	Read Device ID
	_FRAM_SLEEP	= 0xB9,		// 1011 1001b	Enter Sleep Mode
	_FRAM_SNR	= 0xC3		// 1100 0011b	Read S/N
} FM25V02_OP_CODE;


typedef	struct
{
	uint8_t		OP;
	uint16_t	ADDR;
	uint8_t		DATA[];
} _FRAM_RD;

typedef	struct
{
	uint8_t		OP1;
	uint8_t		OP2;
	uint16_t	ADDR;
	uint8_t		DATA[];
} _FRAM_WR;

//---------------------------------------------------------------------------
extern void FM25V02_SetWriteEnableLatch(void);
extern void FM25V02_ResetWriteEnableLatch(void);
extern void FM25V02_wr_data(uint16_t addr, uint16_t count, uint8_t *wr_data);
extern void FM25V02_rd_data(uint16_t addr, uint16_t count, uint8_t *rd_data);
//--------------------------------------------------------------------------
//extern void FM25V02_READ_TEST(void);
//extern void FM25V02_Memory_Test(void);
//--------------------------------------------------------------------------

#endif /* __FRAM_H__ */
