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
 *  main configuration file for png device
 *	png device will teach you how to make device driver with new platform.
 *
 *  author		raxis.lim (raxis.lim@lge.com).lim (raxis.lim@lge.com)
 *  version		1.0 
 *  date		2010.10.05
 *  note		Additional information. 
 *
 *  @addtogroup lg1150_png 
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"
#include "os_util.h"
#include "png_cfg.h"
#include "png_kapi.h"

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
// Default configuration is for L8
LX_PNG_MEM_CFG_T gMemCfgPng[] =
{
	[0] = /* L8 */
	{
		.memory_name = "png_buffer",
		.memory_base = 0x0,
		.memory_size = 0x80000,	/* 512KB. 512KB will support almost PNG image in GP UI */
	},

	[1] = /* L9 */
	{
		.memory_name = "png_buffer",
		.memory_base = 0x0, //0x7A000000
		.memory_size = 0x00200000,	/* L9 doesn't process the big sized image. so shrink memory size */
	},
};

LX_PNG_MEM_CFG_T*		gpMemCfgPng;
LX_PNG_DECODE_CAPS_T	g_png_decode_caps = LX_PNG_DECODE_CAPS_NONE;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/
void PNG_CFG_DeviceConfig(void)
{
	// Configuration for L9
	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )
	{
		gpMemCfgPng = &gMemCfgPng[1];
		g_png_decode_caps |= LX_PNG_DECODE_CAPS_PARTIAL_DECODE;
	}
	else 
	{ 
		gpMemCfgPng = &gMemCfgPng[0];
	}
}



/** @} */

