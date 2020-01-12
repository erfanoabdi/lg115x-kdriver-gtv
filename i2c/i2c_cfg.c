/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/


/** @file
 *
 *  main configuration file for i2c device
 *	i2c device will teach you how to make device driver with new platform.
 *
 *  author		ingyu.yang (ingyu.yang@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_i2c
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>

#include "i2c_cfg.h"
#include "i2c_kapi.h"
#include "i2c_regs.h"
#include "os_util.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

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
static LX_I2C_HW_MAP_T _stI2cHwMapL8Ax[] =
{
	{I2C0_REG_BASE_L8, 	IRQ_I2C0_L8},
	{I2C1_REG_BASE_L8, 	IRQ_I2C1_L8},
	{I2C2_REG_BASE_L8, 	IRQ_I2C2_L8},
	{I2C3_REG_BASE_L8, 	IRQ_I2C3_L8}
};

static LX_I2C_HW_MAP_T _stI2cHwMapL8Bx[] =
{
	{I2C0_REG_BASE_L8, 	IRQ_I2C0_L8},
	{I2C1_REG_BASE_L8, 	IRQ_I2C1_L8},
	{I2C2_REG_BASE_L8, 	IRQ_I2C2_L8},
	{I2C3_REG_BASE_L8, 	IRQ_I2C3_L8},
	{I2C4_REG_BASE_L8, 	IRQ_I2C4_L8},
	{I2C5_REG_BASE_L8, 	IRQ_I2C5_L8}
};

static LX_I2C_HW_MAP_T _stI2cHwMapL9Ax[] =
{
	{I2C0_REG_BASE_L9, 	IRQ_I2C0_L9},
	{I2C1_REG_BASE_L9, 	IRQ_I2C1_L9},
	{I2C2_REG_BASE_L9, 	IRQ_I2C2_L9},
	{I2C3_REG_BASE_L9, 	IRQ_I2C3_L9},
	{I2C4_REG_BASE_L9, 	IRQ_I2C4_L9},
	{I2C5_REG_BASE_L9,	IRQ_I2C5_L9},
	{I2C6_REG_BASE_L9,	IRQ_I2C6_L9},
	{I2C7_REG_BASE_L9,	IRQ_I2C7_L9},
	{I2C8_REG_BASE_L9, 	IRQ_I2C8_L9}
};

#ifdef SIC_LAB_BOARD
static LX_I2C_INTERNAL_T _stI2cInternalL9A0 = {1, I2C_CLOCK_400KHZ}; 	//--- SIC eval board
#else
static LX_I2C_INTERNAL_T _stI2cInternalL9A0 = {4, I2C_CLOCK_400KHZ};	//--- LCDTV PT board
#endif

//static LX_I2C_INTERNAL_T _stI2cInternalL9 = {8, I2C_CLOCK_400KHZ};	// Internal I2C

static LX_I2C_INTERNAL_T _stI2cInternalL9 = {8, I2C_CLOCK_2MHZ};	// Internal I2C

static LX_I2C_CFG_T _stI2cDefaultConfigs[] =
{
	{
		.chip = LX_CHIP_REV(L8,A0),
		.num_hw_devices = 4,
		.num_sw_devices = 3,
		.hw_map = _stI2cHwMapL8Ax,
		.eeprom_ch = 1
	},
	{
		.chip = LX_CHIP_REV(L8,B0),
		.num_hw_devices = 6,
		.num_sw_devices = 1,
		.hw_map = _stI2cHwMapL8Bx,
		.eeprom_ch = 4
	},
	{
		.chip = LX_CHIP_REV(L9,A0),
		.num_hw_devices = 9,
		.num_sw_devices = 1,
		.hw_map = _stI2cHwMapL9Ax,
		.internal = &_stI2cInternalL9A0,
		.eeprom_ch = 4
	},
	{
		.chip = LX_CHIP_REV(L9,A1),
		.num_hw_devices = 9,
		.num_sw_devices = 1,
		.hw_map = _stI2cHwMapL9Ax,
		.internal = &_stI2cInternalL9,
		.eeprom_ch = 4
	},
};


/*========================================================================================
	Implementation Group
========================================================================================*/
LX_I2C_CFG_T* I2C_GetConfig(void)
{
	int i;
	LX_I2C_CFG_T* config = NULL;
	UINT32 chip = lx_chip_rev();

	for(i=0; i < sizeof(_stI2cDefaultConfigs)/sizeof(LX_I2C_CFG_T); i++)
	{
		if(chip >= _stI2cDefaultConfigs[i].chip)
		{
			config = &_stI2cDefaultConfigs[i];
		}
		else
		{
			break;
		}
	}

	return config;
}


/** @} */

