/*
	SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
	Copyright(c) 2010 by LG Electronics Inc.

	This program is free software; you can redistribute it and/or 
	modify it under the terms of the GNU General Public License
	version 2 as published by the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of 
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
	GNU General Public License for more details.
*/ 


/** @file
 *
 *  ADEC core driver functions.
 *	adec device will teach you how to make device driver with new platform.
 *
 *  author		Sudhaher (kanthiraj.s@lge.com)
 *  version		1.0
 *  date		2010.05.20
 *  note		Additional information.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "os_util.h" 	//for alloc functions
#include "proc_util.h"
#include "debug_util.h"

#include "adec_drv.h"
#include "../i2c/i2c_core.h"
#include "i2c_kapi.h"


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
//define NTP7000 Reg.
#define	NTP7000_REG_INPUT_FORMAT				( 0x00 )	// default value 0x00
#define	NTP7000_REG_SOFTMUTE_CTRL0				( 0x0B )	// default value 0x03
#define	NTP7000_REG_VOL_MASTER					( 0x0E )	// default value 0x00
#define	NTP7000_REG_VOL_CH1						( 0x0F )	// default value 0xCF
#define	NTP7000_REG_VOL_CH2						( 0x10 )	// default value 0xCF
#define	NTP7000_REG_SOFT_VOL_CTRL				( 0x12 )	// default value 0x00
#define	NTP7000_REG_PWM_MASK_CTRL0				( 0x19 )	// default value 0x00
#define	NTP7000_REG_MASTER_CLK_FREQ				( 0x1D )	// default value 0x00
#define	NTP7000_REG_I2C_GLITCH_FILTER			( 0x20 )	// default value 0x8F
#define	NTP7000_REG_PWM_SWITCHING_OFF_CTRL		( 0x14 )	// default value 0x03

//define NTP7400 Reg.
#define	NTP7400_REG_SOFTMUTE_CTRL0				( 0x0F )	// ( 0x0B )default value 0x03
#define	NTP7400_REG_VOL_MASTER					( 0x17 )	// ( 0x0E )default value 0x00


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define DAMP_I2C_CHANNEL			0			/* [i2c]     NTP7400 :   1   :   0x54	:	1 */
#define DAMP_I2C_SLAVE_ADDRESS		0x54		/* [i2c]     NTP7400 :   1   :   0x54	:	1 */


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
/* i2c handler for external i2c */
static LX_I2C_DEV_HANDLE _pAdecI2cHandle;
static BOOLEAN 	bDampInitCalled = FALSE;


/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * I2C Init function.
 *
*/
static int ADEC_I2C_Init(void)
{
	//open I2C device handle
	_pAdecI2cHandle = I2C_DevOpen(DAMP_I2C_CHANNEL);

	if(_pAdecI2cHandle != NULL)
	{
		I2C_DevSetClock(_pAdecI2cHandle, I2C_CLOCK_100KHZ);
	}
	else
	{
		ADEC_ERROR("I2C_DevOpen Error !!!\n");

		return -1;
	}

	ADEC_ERROR("ADEC_I2C_Init Ok !!!\n");

	return 0;
}

/**
 * I2C write function.
 *
*/
static int ADEC_I2C_Write(UINT8 reg, UINT8 data)
{
	int rc = -1;

	LX_I2C_RW_DATA_T param;

	if(_pAdecI2cHandle == NULL)
	{
		ADEC_ERROR("I2C Not supported !!!\n");

		return -1;
	}

	param.slaveAddr		= DAMP_I2C_SLAVE_ADDRESS;
	param.subAddrSize	= 1;
	param.subAddr[0]	= reg;
	param.buf			= &data;
	param.bufSize		= 1;
	param.clock			= I2C_CLOCK_INVALID;	//use initial clock
	param.flag			= 0;

	rc = I2C_DevTransfer(_pAdecI2cHandle, &param, I2C_WRITE_MODE);
	if(rc < 0)
	{
		ADEC_ERROR("I2C_DevTransfer Write Error(reg = 0x%X, data = 0x%X) !!!\n", reg, data);
	}

	return rc;
}

/**
 * I2C read function.
 *
*/
static int ADEC_I2C_Read(UINT8 reg, UINT8 *data)
{
	int rc = -1;

	LX_I2C_RW_DATA_T param;

	if(_pAdecI2cHandle == NULL)
	{
		ADEC_ERROR("I2C Not supported !!!\n");
		return -1;
	}

	param.slaveAddr		= DAMP_I2C_SLAVE_ADDRESS;
	param.subAddrSize	= 1;
	param.subAddr[0]	= reg;
	param.buf			= data;
	param.bufSize		= 1;
	param.clock			= I2C_CLOCK_INVALID;	//use initial clock
	param.flag			= 0;

	rc = I2C_DevTransfer(_pAdecI2cHandle, &param, I2C_READ_MODE);
	if(rc < 0)
	{
		ADEC_ERROR("I2C_DevTransfer Read Error(reg = 0x%X, data = 0x%X) !!!\n", reg, *data);
	}

	return rc;
}

#if 0
/**
 * Initializes the adec audio amp output.
 * for the audio amp AMP chip NTP7000
 * @see
*/
int ADEC_NTP7000_Init (void)
{
	int iRetVal;

	iRetVal = ADEC_I2C_Init();
	if(iRetVal)
	{
		ADEC_ERROR("ADEC_I2C_Initn Failed !!!\n");

		return RET_ERROR;
	}

	//Initialize
	ADEC_I2C_Write(NTP7000_REG_I2C_GLITCH_FILTER, 0x1E);
	ADEC_I2C_Write(NTP7000_REG_MASTER_CLK_FREQ, 0x00);
	ADEC_I2C_Write(NTP7000_REG_INPUT_FORMAT, 0x00);
	ADEC_I2C_Write(NTP7000_REG_VOL_MASTER, 0xAA);//0xA0
	ADEC_I2C_Write(NTP7000_REG_VOL_CH1, 0xCF);
	ADEC_I2C_Write(NTP7000_REG_VOL_CH2, 0xCF);
	ADEC_I2C_Write(NTP7000_REG_SOFT_VOL_CTRL, 0xE7);

	//Sound ON
	ADEC_I2C_Write(NTP7000_REG_PWM_MASK_CTRL0, 0x04);
	ADEC_I2C_Write(NTP7000_REG_PWM_SWITCHING_OFF_CTRL, 0x00);
	ADEC_I2C_Write(NTP7000_REG_SOFTMUTE_CTRL0, 0x00);

	ADEC_ERROR("Audio D-AMP(NTP7000) Init Done!!!\n");

	return RET_OK;
}


/**
 * Mute or Unmute for audio NTP7000 AMP.
 *
*/
int ADEC_NTP7000_SetMute (UINT8 bMuteOnOff)
{
	/*
	 * 1 - Mute
	 * 0 - UnMute
	 */
	if (bMuteOnOff == TRUE) //Mute
	{
		//Sound Off
		ADEC_I2C_Write(NTP7000_REG_SOFTMUTE_CTRL0, 0x03);
		ADEC_I2C_Write(NTP7000_REG_PWM_SWITCHING_OFF_CTRL, 0x03);
		ADEC_I2C_Write(NTP7000_REG_PWM_MASK_CTRL0, 0x06);

		ADEC_ERROR("Audio D-AMP(NTP7000) Mute !!!\n");
	}
	else //UnMute
	{
		//Sound ON
		ADEC_I2C_Write(NTP7000_REG_PWM_MASK_CTRL0, 0x04);
		ADEC_I2C_Write(NTP7000_REG_PWM_SWITCHING_OFF_CTRL, 0x00);
		ADEC_I2C_Write(NTP7000_REG_SOFTMUTE_CTRL0, 0x00);

		ADEC_ERROR("Audio D-AMP(NTP7000) Unmute !!!\n");
	}

	return RET_OK;
}
#endif	//#if 0

/**
 * Initializes the adec audio amp output.
 * for the audio amp AMP chip NTP7000
 * @see
*/
int ADEC_NTP7400_Init (void)
{
	int iRetVal;

	UINT8	vol = 0;
	UINT8	mute = 0;

	//Check a D-AMP init called status
	if(bDampInitCalled == TRUE)
	{
		return RET_OK;
	}
	else
	{
		bDampInitCalled = TRUE;
	}

	iRetVal = ADEC_I2C_Init();
	if(iRetVal)
	{
		ADEC_ERROR("ADEC_I2C_Initn Failed !!!\n");

		return RET_ERROR;
	}

	ADEC_I2C_Write(0x00, 0x00);
	ADEC_I2C_Write(0x01, 0x00);
	ADEC_I2C_Write(0x02, 0x00);
	ADEC_I2C_Write(0x03, 0x4e);
	ADEC_I2C_Write(0x04, 0x00);
	ADEC_I2C_Write(0x05, 0x00);
	ADEC_I2C_Write(0x06, 0x4e);
	ADEC_I2C_Write(0x07, 0xab);
	ADEC_I2C_Write(0x08, 0x16);
	ADEC_I2C_Write(0x09, 0xab);
	ADEC_I2C_Write(0x0A, 0x70);
	ADEC_I2C_Write(0x0B, 0xaa);
	ADEC_I2C_Write(0x0C, 0x01);
	ADEC_I2C_Write(0x0D, 0x14);
	ADEC_I2C_Write(0x0E, 0x11);
	ADEC_I2C_Write(0x0F, 0x00);
	ADEC_I2C_Write(0x10, 0x00);
	ADEC_I2C_Write(0x11, 0x00);
	ADEC_I2C_Write(0x12, 0x00);
	ADEC_I2C_Write(0x13, 0x00);
	ADEC_I2C_Write(0x14, 0x0b);
	ADEC_I2C_Write(0x15, 0x43);
	ADEC_I2C_Write(0x16, 0x05);
	ADEC_I2C_Write(0x17, 0xb3);	//Volume
	ADEC_I2C_Write(0x18, 0xcf);
	ADEC_I2C_Write(0x19, 0xcf);
	ADEC_I2C_Write(0x1A, 0x00);
	ADEC_I2C_Write(0x1B, 0x00);
	ADEC_I2C_Write(0x1C, 0xe4);
	ADEC_I2C_Write(0x1D, 0x9a);
	ADEC_I2C_Write(0x1E, 0x00);
	ADEC_I2C_Write(0x1F, 0x0f);
	ADEC_I2C_Write(0x20, 0x7f);
	ADEC_I2C_Write(0x21, 0x7f);
	ADEC_I2C_Write(0x22, 0x15);
	ADEC_I2C_Write(0x23, 0x15);
	ADEC_I2C_Write(0x24, 0x00);
	ADEC_I2C_Write(0x25, 0x00);
	ADEC_I2C_Write(0x26, 0x00);
	ADEC_I2C_Write(0x27, 0x00);
	ADEC_I2C_Write(0x28, 0x00);
	ADEC_I2C_Write(0x29, 0x00);
	ADEC_I2C_Write(0x2A, 0x00);
	ADEC_I2C_Write(0x2B, 0x00);
	ADEC_I2C_Write(0x2C, 0x00);
	ADEC_I2C_Write(0x2D, 0x00);
	ADEC_I2C_Write(0x2E, 0x00);
	ADEC_I2C_Write(0x2F, 0x00);
	ADEC_I2C_Write(0x30, 0x00);
	ADEC_I2C_Write(0x31, 0x00);
	ADEC_I2C_Write(0x32, 0x00);
	ADEC_I2C_Write(0x33, 0x00);
	ADEC_I2C_Write(0x34, 0x00);
	ADEC_I2C_Write(0x35, 0x00);
	ADEC_I2C_Write(0x36, 0x00);
	ADEC_I2C_Write(0x37, 0x00);
	ADEC_I2C_Write(0x38, 0x00);
	ADEC_I2C_Write(0x39, 0x00);
	ADEC_I2C_Write(0x3A, 0x00);
	ADEC_I2C_Write(0x3B, 0x00);
	ADEC_I2C_Write(0x3C, 0x00);
	ADEC_I2C_Write(0x3D, 0x00);
	ADEC_I2C_Write(0x3E, 0x00);
	ADEC_I2C_Write(0x3F, 0x00);
	ADEC_I2C_Write(0x40, 0x00);
	ADEC_I2C_Write(0x41, 0x00);
	ADEC_I2C_Write(0x42, 0x00);
	ADEC_I2C_Write(0x43, 0x00);
	ADEC_I2C_Write(0x44, 0x00);
	ADEC_I2C_Write(0x45, 0x00);
	ADEC_I2C_Write(0x46, 0x00);
	ADEC_I2C_Write(0x47, 0x00);
	ADEC_I2C_Write(0x48, 0x00);
	ADEC_I2C_Write(0x49, 0x00);
	ADEC_I2C_Write(0x4A, 0x00);
	ADEC_I2C_Write(0x4B, 0x00);
	ADEC_I2C_Write(0x4C, 0x00);
	ADEC_I2C_Write(0x4D, 0x00);
	ADEC_I2C_Write(0x4E, 0x08);
	ADEC_I2C_Write(0x4F, 0x70);
	ADEC_I2C_Write(0x50, 0x00);
	ADEC_I2C_Write(0x51, 0x60);
	ADEC_I2C_Write(0x52, 0x90);
	ADEC_I2C_Write(0x53, 0x00);
	ADEC_I2C_Write(0x54, 0x2c);
	ADEC_I2C_Write(0x55, 0x03);
	ADEC_I2C_Write(0x56, 0x20);
	ADEC_I2C_Write(0x57, 0x0a);
	ADEC_I2C_Write(0x58, 0x01);
	ADEC_I2C_Write(0x59, 0x17);
	ADEC_I2C_Write(0x5A, 0x10);
	ADEC_I2C_Write(0x5B, 0x01);
	ADEC_I2C_Write(0x5C, 0x00);
	ADEC_I2C_Write(0x5D, 0x40);
	ADEC_I2C_Write(0x5E, 0x8c);
	ADEC_I2C_Write(0x5F, 0x00);
	ADEC_I2C_Write(0x60, 0x00);
	ADEC_I2C_Write(0x61, 0x81);
	ADEC_I2C_Write(0x62, 0x00);
	ADEC_I2C_Write(0x63, 0xa1);
	ADEC_I2C_Write(0x64, 0x1e);
	ADEC_I2C_Write(0x65, 0x00);
	ADEC_I2C_Write(0x66, 0x00);
	ADEC_I2C_Write(0x67, 0x00);
	ADEC_I2C_Write(0x68, 0x00);
	ADEC_I2C_Write(0x69, 0x00);
	ADEC_I2C_Write(0x6A, 0x00);
	ADEC_I2C_Write(0x6B, 0x00);
	ADEC_I2C_Write(0x6C, 0x00);
	ADEC_I2C_Write(0x6D, 0x00);
	ADEC_I2C_Write(0x6E, 0x00);
	ADEC_I2C_Write(0x6F, 0x00);
	ADEC_I2C_Write(0x70, 0x00);
	ADEC_I2C_Write(0x71, 0x00);
	ADEC_I2C_Write(0x72, 0x00);
	ADEC_I2C_Write(0x73, 0x00);
	ADEC_I2C_Write(0x74, 0x00);
	ADEC_I2C_Write(0x75, 0x00);
	ADEC_I2C_Write(0x76, 0x00);
	ADEC_I2C_Write(0x77, 0x00);
	ADEC_I2C_Write(0x78, 0x00);
	ADEC_I2C_Write(0x79, 0x00);
	ADEC_I2C_Write(0x7A, 0x00);
	ADEC_I2C_Write(0x7B, 0x00);
	ADEC_I2C_Write(0x7C, 0x00);
	ADEC_I2C_Write(0x7D, 0x00);
	ADEC_I2C_Write(0x7E, 0x00);

	//ADEC_I2C_Write(0x17, 0xed);	//Volume

	//Read a main volume
	(void)ADEC_I2C_Read(NTP7400_REG_VOL_MASTER, &vol);

	//Read a mute status
	(void)ADEC_I2C_Read(NTP7400_REG_SOFTMUTE_CTRL0, &mute);

	ADEC_ERROR("Audio D-AMP(NTP7400) Init Done!!!(VOL_MASTER = 0x%X, MUTE = %d)\n", vol, mute);

	return RET_OK;
}

/**
 * Mute or Unmute for audio NTP7400 AMP.
 *
*/
int ADEC_NTP7400_SetMute (BOOLEAN bMuteOnOff)
{
	/*
	 * 1 - Mute
	 * 0 - UnMute
	 */
	if(bMuteOnOff == TRUE) //Mute
	{
		//Sound Off
		ADEC_I2C_Write(NTP7400_REG_SOFTMUTE_CTRL0, 0x03);

		ADEC_DEBUG("Audio D-AMP(NTP7000) Mute !!!\n");
	}
	else //UnMute
	{
		//Sound ON
		ADEC_I2C_Write(NTP7400_REG_SOFTMUTE_CTRL0, 0x00);

		ADEC_DEBUG("Audio D-AMP(NTP7000) Unmute !!!\n");
	}

	return RET_OK;
}

const UINT16 gAdec_NTP7500_VOLUME_CRV[] =
{
	0x0000, 0x5505, 0x6201, 0x6f01, 0x7d01, 0x8601, 0x8e01, 0x9301, 0x9b01, 0xa101, /*0 ~ 9 */
	0xa601, 0xae01, 0xb106, 0xb501, 0xb801, 0xba03, 0xbc03, 0xbe02, 0xbf04, 0xc007, /*10 ~ 19 */
	0xc201, 0xc302, 0xc402, 0xc502, 0xc601, 0xc701, 0xc705, 0xc801, 0xc805, 0xc901, /*20 ~ 29 */
	0xc905, 0xca01, 0xca05, 0xcb01, 0xcb05, 0xcc01, 0xcc04, 0xcc06, 0xcd04, 0xcd06, /*30 ~ 39 */
	0xce01, 0xce03, 0xce05, 0xce07, 0xcf00, 0xcf03, 0xcf05, 0xcf07, 0xd001, 0xd003, /*40 ~ 49 */
	0xd005, 0xd100, 0xd103, 0xd106, 0xd201, 0xd203, 0xd205, 0xd207, 0xd302, 0xd304, /*50 ~ 59 */
	0xd306, 0xd401, 0xd405, 0xd407, 0xd502, 0xd505, 0xd600, 0xd604, 0xd607, 0xd703, /*60 ~ 69 */
	0xd707, 0xd802, 0xd807, 0xd902, 0xd907, 0xda03, 0xda07, 0xdb03, 0xdc00, 0xdc07, /*70 ~ 79 */
	0xdd02, 0xde00, 0xde05, 0xdf02, 0xdf07, 0xe005, 0xe100, 0xe201, 0xe203, 0xe207, /*80 ~ 89 */
	0xe304, 0xe402, 0xe406, 0xe503, 0xe600, 0xe604, 0xe702, 0xe707, 0xe804, 0xe900, /*90 ~ 99 */
	0xe905 /*100*/
};

/**
 * Volume Control for audio NTP7400 AMP.
 *
*/
int ADEC_NTP7400_SetVolume (UINT8 volume)
{
	//Sound Off
	ADEC_I2C_Write(NTP7400_REG_VOL_MASTER, (UINT8)(gAdec_NTP7500_VOLUME_CRV[volume] >> 8));

	ADEC_DEBUG("Audio D-AMP(NTP7400) Volume = %d !!!\n", volume);
	return RET_OK;
}

/** @} */


