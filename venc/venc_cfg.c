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
 *  main configuration file for venc device
 *	venc device will teach you how to make device driver with new platform.
 *
 *  author		youngwoo.jin (youngwoo.jin@lge.com)
 *  version		1.0
 *  date		2011.05.19
 *  note		Additional information.
 *
 *  @addtogroup lg1152_venc
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "os_util.h"
#include "venc_cfg.h"

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
LX_VENC_MEM_CFG_S_T	*gpstVencMemConfig;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
LX_VENC_MEM_CFG_S_T _stVencMemComfigPreset[2] =
{
	// For L9 Platform
	{
		.pcSRAMName 	= "VencSRAM",
		.uiSRAMBase 	= 0x0, //0x7C000000,//0x76C84000,
		.uiSRAMSize 	= 0x00080000,

		.pcSROMName 	= "VencSROM",
		.uiSROMBase 	= 0x0,//0x76D04000,
		.uiSROMSize 	= 0x00080000,

		.pcFrameBufName	= "VencFrameBuf",
		.uiFrameBufBase	= 0x0,//0x76D84000,
		.uiFrameBufSize	= 0x002D0000,

		.pcDPBName		= "VencDPB",
		.uiDPBBase		= 0x0,//0x77054000,
		.uiDPBSize		= 0x003F0000,

		.pcESBufName	= "VencESBuf",
		.uiESBufBase	= 0x0,//0x77444000,
		.uiESBufSize	= 0x0017F000,

		.pcAUIBufName	= "VencAUIBuf",
		.uiAUIBufBase	= 0x0,//0x775C3000,
		.uiAUIBufSize	= 0x00001000,

		.pcInBufName	= "VencBuf",
		.uiInBufBase	= 0x0,//0x775C4000,
		.uiInBufSize	= 0x00546000,

		.pcScalerName	= "VencScaler",
		.uiScalerBase	= 0x0,//0x77B0A000,
		.uiScalerSize	= 0x00261000,

		.pcAESBufName	= "AudioESBuf",
		.uiAESBufBase	= 0x0,//0x77720000,
		.uiAESBufSize	= 0x00060000,

		.pcAAUIBufName	= "AudioAUIBuf",
		.uiAAUIBufBase	= 0x0,//0x77800000,
		.uiAAUIBufSize	= 0x00001000,

		.pcTSBufName 	= "TSOutBuf",
		.uiTSBufBase 	= 0x0,//0x77B0A000,
		.uiTSBufSize 	= 0x260FB0,
	},
	// For L8 Platform
	{
		.pcAUIBufName	= "VencAUIBuf",
		.uiAUIBufBase	= 0x0CA8F000,
		.uiAUIBufSize	= 0x00001000,

		.pcSRAMName 	= "VencSRAM",
		.uiSRAMBase 	= 0x0AA00000,
		.uiSRAMSize 	= 0x00080000,

		.pcSROMName 	= "VencSROM",
		.uiSROMBase 	= 0x0AA00000 + 0x00080000,
		.uiSROMSize 	= 0x00080000,

		.pcScalerName	= "VencScaler",
		.uiScalerBase	= 0x0A000000,
		.uiScalerSize	= 0x00261000,

		.pcESBufName	= "VencESBuf",
		.uiESBufBase	= 0x0C910000,

		.pcScalarOutName= "VencScalar",
		.uiScalarOutBase= 0x0FC1C000,

		.pcInBufName	= "VencBuf",
		.uiInBufBase	= 0x3DDD0000,
		.uiInBufSize	= 0x0017F000,

		.pcPNGBufName	= "VencPNGBuf",
		.uiPNGBufBase	= 0x0CA90000,
		.uiPNGBufSize	= 0x00400000,
	}
};

/*========================================================================================
	Implementation Group
========================================================================================*/
void VENC_CFG_MemoryMap ( void )
{
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
	{
		gpstVencMemConfig = &_stVencMemComfigPreset[0];
	}
	else
	{
		gpstVencMemConfig = &_stVencMemComfigPreset[1];
	}
}


/** @} */

