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
 *  main configuration file for hdmi device
 *	hdmi device will teach you how to make device driver with new platform.
 *
 *  author		sh.myoung (sh.myoung@lge.com)
 *  version		1.0
 *  date		2010.01.06
 *  note		Additional information.
 *
 *  @addtogroup lg1150_hdmi
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"
#include "hdmi_cfg.h"
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


/*========================================================================================
	Implementation Group
========================================================================================*/
/**
HDMI Register configuration.
*/
LX_HDMI_REG_T gRegCfgHdmi[] =
{
	// for LG1150(L8) A0 & B0
	{
		.chip_name	= "L8",
		.vport_reg_base_addr 	= 0xc0007000,
		.vport_reg_size			= 0x00007ef0,
		.vport_qemu_base_addr	= 0x04007000,

		.phy_reg_base_addr		= 0xc001e000,
		.phy_reg_size			= 0x000003ff,
		.phy_qemu_base_addr		= 0x0401e000,
	},
	// for LG1151(L9) A0 & A1
	{
		.chip_name	= "L9_A0_A1",
		.vport_reg_base_addr 	= 0xc0008000,
		.vport_reg_size			= 0x00008ef0,
		.vport_qemu_base_addr	= 0x04008000,

		.phy_reg_base_addr		= 0xc0018000,
		.phy_reg_size			= 0x000008ff,
		.phy_qemu_base_addr		= 0x04018000,
	},
};

LX_HDMI_REG_T *pHdmiRegCfg = &gRegCfgHdmi[0];	// for Real.

/** @} */

