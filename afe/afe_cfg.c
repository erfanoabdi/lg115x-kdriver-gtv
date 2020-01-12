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
 *  CVD LDrv file for afe device
 *	afe device will teach you how to make device driver with new platform.
 *
 *  author		wonsik.do (wonsik.do@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_afe
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"
#include "afe_cfg.h"
#include "base_dev_cfg.h"
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

/**
Vport Register configuration.
*/
LX_VPORT_REG_T gRegCfgVport[] =
{
	// for LG1150(L8) A0 & B0
	{
		.chip_name	= "VPORT_L8",
		.vport_reg_base_addr 	= 0xc0007000,
		.vport_reg_size			= 0x00007ef0,
		.vport_qemu_base_addr	= 0x04007000,
		.chbcvd_reg_base_addr	= 0,
		.chbcvd_reg_size	= 0,
	},
	// for LG1151(L9) A0
	{
		.chip_name	= "CVD_L9",
//		.vport_reg_base_addr 	= 0xc0008000,
		.vport_reg_base_addr 	= 0xc0024100,
		//.vport_reg_size			= 0x00008ef0,
		.vport_reg_size			= 0x00000800,
		.vport_qemu_base_addr	= 0x04008000,
		.chbcvd_reg_base_addr	= 0xc000af00,
		.chbcvd_reg_size	= 	0xf4,
	},
};

LX_VPORT_REG_T *pVportRegCfg = &gRegCfgVport[0];	// for Real.

LX_CVD_MEM_CFG_T gMemCfgCvd[] =
{
	{
	.memory_name = "cvd_mem_L8",
	.memory_base = 0x27ce0000,
	.memory_size = 0x00320000,
	},
	{
	.memory_name = "cvd_mem_L9",
	.memory_base = 0x0,	//7bb00000,
	.memory_size = 0x00500000,
	}
};

LX_CVD_MEM_CFG_T*	gpCvdMemCfg = NULL;
/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/
void    AFE_InitCfg ( void )
{

	/*-----------------------------------------------------------------------------------
	 * [L9] configuration
	 *----------------------------------------------------------------------------------*/
    if( lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
    {
		gpCvdMemCfg = &gMemCfgCvd[1];

    }
	/*-----------------------------------------------------------------------------------
	 * [L8] configuration
	 *----------------------------------------------------------------------------------*/
    else
    {
		gpCvdMemCfg = &gMemCfgCvd[0];
    }
}


/** @} */

