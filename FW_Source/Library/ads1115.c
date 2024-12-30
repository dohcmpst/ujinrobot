/*
 * ads1115.c
 *
 *  Created on: 2022. 10. 18.
 *      Author: LEE-LAPTOP
 */

#include <stdio.h>
#include "ads1115.h"

//#ifdef ADS1115


ADS1115_InitTypeDefine ADS1115_ADDR_GND;
ADS1115_InitTypeDefine ADS1115_ADDR_VDD;
float ADS1115_CHn_Vol[4];
/** *  Global variables that can be called externally , Recorded ADS1115 Sampled raw 16 Bit data . call void ADS1115_RefreshAllChannel( ) Function can refresh this variable . *  By defining macros ADS1115_USE_FILTER, Can be ADS1115 The polling sampling data is filtered by sliding filter , Save to ADS1115_RawData[] in . *  adopt float ADS1115_RawDataToVoltage(int16_t rawData) The function can change ADS1115_RawData[] Convert to the corresponding voltage value . */
int16_t ADS1115_RawData[4] = {
    0};


/** * @brief  Complete the initialization of chip control port , And set the initial state  */
void ADS1115_Init()
{



}


/** * @brief Configuration of ADS1115, single-shot */
void ADS1115_UserConfig_SingleConver(ADS1115_InitTypeDefine* ADS1115_InitStruct,ADS1115_ADDRESS ADDRESS)
{

	ADS1115_InitStruct->COMP_LAT = ADS1115_COMP_LAT_0;
	ADS1115_InitStruct->COMP_MODE = ADS1115_COMP_MODE_0;
	ADS1115_InitStruct->COMP_POL = ADS1115_COMP_POL_0;
	ADS1115_InitStruct->DataRate = ADS1115_DataRate_475;
	ADS1115_InitStruct->MODE = ADS1115_MODE_SingleConver;
	ADS1115_InitStruct->MUX = ADS1115_MUX_Channel_0;
	ADS1115_InitStruct->OS = ADS1115_OS_SingleConverStart;
	ADS1115_InitStruct->PGA = ADS1115_PGA_4096;
	ADS1115_InitStruct->ADDRESS = ADDRESS;
	ADS1115_InitStruct->CHANNEL = ADS1115_CHANNEL0;
	ADS1115_Config(ADS1115_InitStruct);

}


/** * @brief Configuration of ADS1115, continuous conversion */
void ADS1115_UserConfig_ContinuConver(ADS1115_InitTypeDefine* ADS1115_InitStruct,ADS1115_ADDRESS ADDRESS)
{

	ADS1115_InitStruct->COMP_LAT = ADS1115_COMP_LAT_0;
	ADS1115_InitStruct->COMP_MODE = ADS1115_COMP_MODE_0;
	ADS1115_InitStruct->COMP_POL = ADS1115_COMP_POL_0;
	ADS1115_InitStruct->DataRate = ADS1115_DataRate_475;
	ADS1115_InitStruct->MODE = ADS1115_MODE_ContinuConver;
	ADS1115_InitStruct->MUX = ADS1115_MUX_Channel_0;
	ADS1115_InitStruct->OS = ADS1115_OS_OperationalStatus;
	ADS1115_InitStruct->PGA = ADS1115_PGA_4096;
	ADS1115_Config(ADS1115_InitStruct);
}


/** * @brief  To configure ADS1115 * @param ADS1115_InitStruct:  Used to configure ADS1115 Pointer to the structure variable of  * @return  Configuration result  * @arg: fail * @arg: success */
void ADS1115_Config(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{

	uint16_t Config;
	uint8_t Writebuff[3];

	Config = ADS1115_InitStruct->OS + ADS1115_InitStruct->MUX + ADS1115_InitStruct->PGA + ADS1115_InitStruct->MODE
			+ ADS1115_InitStruct->DataRate + ADS1115_InitStruct->COMP_MODE + ADS1115_InitStruct->COMP_POL
			+ ADS1115_InitStruct->COMP_LAT + ADS1115_InitStruct->COMP_QUE;
  Writebuff[0] = ADS1115_Pointer_ConfigReg;
	Writebuff[1] = (unsigned char) ((Config >> 8) & 0xFF);
	Writebuff[2] = (unsigned char) (Config & 0xFF);

	HAL_I2C_Master_Transmit(&hi2c2, ADS1115_InitStruct->ADDRESS, Writebuff, 3, 100);

}



/** * @brief  Read ADS1115 Raw data under the current channel  * @param rawData:  Pass in a int16_t Pointer to integer variable ,ADS1115 The original data will be saved in this variable  * @return  Read the results  * @arg 0: fail * @arg 1: success */

void ADS1115_ReadRawData(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{

	unsigned char Result[2];
	uint8_t Writebuff[1];
	Writebuff[0] = ADS1115_Pointer_ConverReg;

	if (HAL_I2C_Master_Transmit(&hi2c2, ADS1115_InitStruct->ADDRESS, Writebuff, 3, 500) != HAL_OK){
		log_debug("i2c transmit error");
	}

	if (HAL_I2C_Master_Receive(&hi2c2, ADS1115_InitStruct->ADDRESS , Result, 2, 500) != HAL_OK){
		log_debug("i2c receive error");
	}

	ADS1115_InitStruct->ADS1115_RawData[ADS1115_InitStruct->CHANNEL]  =  (int16_t) (((Result[0] << 8) & 0xFF00) | (Result[1] & 0xFF));
}


/** * @brief Switch the channel of ADS1115 * @param channel */
void ADS1115_ScanChannel(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{

	switch (ADS1115_InitStruct->CHANNEL)
	{

	case ADS1115_CHANNEL0:
		ADS1115_InitStruct->MUX = ADS1115_MUX_Channel_0;
		break;
	case ADS1115_CHANNEL1:
		ADS1115_InitStruct->MUX = ADS1115_MUX_Channel_1;
		break;
	case ADS1115_CHANNEL2:
		ADS1115_InitStruct->MUX = ADS1115_MUX_Channel_2;
		break;
	case ADS1115_CHANNEL3:
		ADS1115_InitStruct->MUX = ADS1115_MUX_Channel_3;
		break;
	default:
		break;
	}
	ADS1115_Config(ADS1115_InitStruct);
}


/** * @brief  Convert the original sampling data of the sensor into voltage data , *  according to ADS1115_InitType The gain information contained in the structure is calculated  * @param rawData:  Raw data to be converted  * @retval  Returns the calculated voltage value  */
void ADS1115_RawDataToVoltage(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{
	switch (ADS1115_InitStruct->PGA)
	{

	case ADS1115_PGA_0256:
		//voltage = rawData * 0.0078125;
	ADS1115_InitStruct->ADS1115_Vol[ADS1115_InitStruct->CHANNEL] = ADS1115_InitStruct->ADS1115_RawData[ADS1115_InitStruct->CHANNEL]* 0.0078125;
		break;

	case ADS1115_PGA_0512:
	// voltage = rawData * 0.015625;
	ADS1115_InitStruct->ADS1115_Vol[ADS1115_InitStruct->CHANNEL] = ADS1115_InitStruct->ADS1115_RawData[ADS1115_InitStruct->CHANNEL]* 0.015625;
		break;

	case ADS1115_PGA_1024:
		//voltage = rawData * 0.03125;
	ADS1115_InitStruct->ADS1115_Vol[ADS1115_InitStruct->CHANNEL] = ADS1115_InitStruct->ADS1115_RawData[ADS1115_InitStruct->CHANNEL]* 0.03125;
		break;

	case ADS1115_PGA_2048:
	// voltage = rawData * 0.0625;
	ADS1115_InitStruct->ADS1115_Vol[ADS1115_InitStruct->CHANNEL] = ADS1115_InitStruct->ADS1115_RawData[ADS1115_InitStruct->CHANNEL]* 0.0625;
		break;

	case ADS1115_PGA_4096:
		//voltage = rawData * 0.125;
	ADS1115_InitStruct->ADS1115_Vol[ADS1115_InitStruct->CHANNEL] = ADS1115_InitStruct->ADS1115_RawData[ADS1115_InitStruct->CHANNEL]* 0.125;
		break;

	case ADS1115_PGA_6144:
	// voltage = rawData * 0.1875;
	ADS1115_InitStruct->ADS1115_Vol[ADS1115_InitStruct->CHANNEL] = ADS1115_InitStruct->ADS1115_RawData[ADS1115_InitStruct->CHANNEL]* 0.1875;
		break;

	default:
		//voltage = 0;
	ADS1115_InitStruct->ADS1115_Vol[ADS1115_InitStruct->CHANNEL] = 0;
		break;
	}


}


/** * @brief  Direct access to ADS1115 Voltage sampling value of current channel  * @return  Voltage sampling value  */
void ADS1115_GetVoltage(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{

	ADS1115_ReadRawData(ADS1115_InitStruct);
	ADS1115_RawDataToVoltage(ADS1115_InitStruct);

}


/** * @brief  Get and calculate ADC The average voltage value of the sample  * @param num:  Calculate the number of averages  * @retval  Average value of voltage sampling  */
void ADS1115_GetAverageVoltage(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{


// ADS1115_ReadRawData(&ADS1115_InitType);
//
// ADS1115_GetVoltage(&ADS1115_InitType);
// ADS1115_RefreshAllChannel(&ADS1115_InitType);

// return ADS1115_RawDataToVoltage(sum/num);
}

/** * @brief  Refresh ADS1115 Sampling data of all channels  *  because ADS1115 It takes a long time for the data to be stable after channel switching , *  During multi-channel data acquisition , Delay blocking after channel switching and waiting for the completion of switching will occupy too much system time , *  Therefore, it is necessary to poll the acquisition in a timer ADS1115 data , After each acquisition , Switch to the next channel  *  The efficiency of the system is greatly improved . * *  Call this function to refresh the global variable ADS1115_RawData[4] Value . * *  This function should be called periodically in a timer update interrupt service function , The update cycle should be less than 200Hz */
void ADS1115_RefreshAllChannel(ADS1115_InitTypeDefine *ADS1115_InitStruct)
{



	// Uncertain data may be read out during channel switching , Therefore, we need to put the front 1~2 The data read out for the first time is discarded
	ADS1115_ReadRawData(ADS1115_InitStruct);
	ADS1115_ReadRawData(ADS1115_InitStruct);
	ADS1115_ReadRawData(ADS1115_InitStruct);
	//ADS1115 in total 4 Channels
	///ADS1115_InitStruct->CHANNEL++;




	if(ADS1115_InitStruct->CHANNEL > ADS1115_MAX_CHANNEL-1)
		ADS1115_InitStruct->CHANNEL = ADS1115_CHANNEL0;

	// Switch to the next channel after sampling
	ADS1115_ScanChannel(ADS1115_InitStruct);
}
