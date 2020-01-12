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
 *  main configuration file for ttx device
 *	ttx device will teach you how to make device driver with new platform.
 *
 *  author		wonsik.do (wonsik.do@lge.com)
 *  version		1.0
 *  date		2010.01.06
 *  note		Additional information.
 *
 *  @addtogroup lg1150_ttx
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "os_util.h"
#include "ttx_cfg.h"
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

LX_VBI_REG_CFG_T gstVbiRegCfgTTXRev[] =
{
	{
		.vbi_register_name = "L8_TTX_REG",
		.vbi_register_base = 0xC001D000,
		.vbi_register_size = 0x1000,
	},

	{
		.vbi_register_name = "L9Ax_TTX_REG",
		.vbi_register_base = 0xc0025000,
		.vbi_register_size = 0x1000,
	},

	{
		.vbi_register_name = "L9Bx_TTX_REG",
		.vbi_register_base = 0xc0026000,
		.vbi_register_size = 0x800,
	},
};


LX_VBI_MEM_CFG_T gstBufMemCfgTTXRev[] =
{
	{
		.buf_memory_name = "L8_ttx_memory",
		.buf_memory_base = 0x27ca0000,
		.buf_memory_size = 0x00040000,
	},

	{
		.buf_memory_name = "L9_ttx_memory",
		.buf_memory_base = 0,//0x7BA00000,
		.buf_memory_size = 0x00100000,
	},

};
/*
LX_TTX_MEM_CFG_S_T gstMemCfgTTXRev[] =
{
	{
		.chip_name	= "L8_TTX",
		.pcMemoryName = "ttx_memory",
		.uiMemoryBase = 0x27ca0000,
		.uiMemorySize = 0x00040000,

		.pcRegName = "L8_TTX_REG",
		.uiRegBase = 0xC001D000,
		.uiRegSize = 0x1000,
	},

	{
		.chip_name	= "L9_TTX",
		.pcMemoryName = "ttx_memory",
		.uiMemoryBase = 0x7BA00000,
		.uiMemorySize = 0x00100000,

		.pcRegName = "L9_TTX_REG",
		.uiRegBase = 0xc0025000,
		.uiRegSize = 0x1000,
	},

	{
		.chip_name	= "L9Bx_TTX",
		.pcMemoryName = "ttx_memory",
		.uiMemoryBase = 0x7BA00000,
		.uiMemorySize = 0x00100000,

		.pcRegName = "L9Bx_TTX_REG",
		.uiRegBase = 0xc0026000,
		.uiRegSize = 0x800,
	},
};
*/

LX_VBI_MEM_CFG_T* 	gstMemCfgTTX	= NULL;
LX_VBI_REG_CFG_T* 	gstRegCfgTTX	= NULL;


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/
void    TTX_InitCfg ( void )
{

	/*-----------------------------------------------------------------------------------
	 * [L9] configuration
	 *----------------------------------------------------------------------------------*/
    if( lx_chip_rev( ) >= LX_CHIP_REV(L9,B0) )
    {
		gstMemCfgTTX = &gstBufMemCfgTTXRev[1];
		gstRegCfgTTX = &gstVbiRegCfgTTXRev[2];

    }
	else if( lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
    {
		gstMemCfgTTX = &gstBufMemCfgTTXRev[1];
		gstRegCfgTTX = &gstVbiRegCfgTTXRev[1];

    }
	/*-----------------------------------------------------------------------------------
	 * [L8] configuration
	 *----------------------------------------------------------------------------------*/
    else
    {
		gstMemCfgTTX = &gstBufMemCfgTTXRev[0];
		gstRegCfgTTX = &gstVbiRegCfgTTXRev[0];
    }
}



/** @} */

