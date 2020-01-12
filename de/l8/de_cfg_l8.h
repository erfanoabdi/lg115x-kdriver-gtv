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
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author	  jaemo.kim (jaemo.kim@lge.com)
 * version	  1.0
 * date		  2011.04.06
 * note		  Additional information.
 *
 * @addtogroup lg1152_de
 * @{
 */

#ifndef  DE_CFG_L8_INC
#define  DE_CFG_L8_INC

/*----------------------------------------------------------------------------------------
 *	 Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "de_cfg_def.h"

/*----------------------------------------------------------------------------------------
 *	 Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define VIDEO_L8_FRAME_DDR_MW_CAP			1692
#define VIDEO_L8_FRAME_DDR_MW_TNR_IPC		1504
#define VIDEO_L8_FRAME_DDR_MW_SCR			1692
#define VIDEO_L8_FRAME_DDR_SW_CAP			450
#define VIDEO_L8_FRAME_DDR_SW_PIP			180
#define VIDEO_L8_FRAME_DDR_SW_VCR			216
#define VIDEO_L8_FRAME_DDR_SW_DVR			216
#define VIDEO_L8_FRAME_DDR_SW_MDI			272
#define VIDEO_L8_FRAME_DDR_AUX_VCS			120
#define VIDEO_L8_FRAME_DDR_OSW_OSD			0 //1200

#define VIDEO_L8_FRAME_DDR_BASE				(0x20000000)
#define VIDEO_L8_FRAME_FIRMWARE_OFFSET		(0x00000000)

#define VIDEO_L8_ROW2PHY(_r)				(CONV_ROW2BYTE(_r) | VIDEO_L8_FRAME_DDR_BASE)

#define VIDEO_L8_FIRMWARE_ROW_SIZE_OF_DE	(VIDEO_FIRMWARE_ROW_SIZE)
#define VIDEO_L8_FIRMWARE_ROW_SIZE_OF_LED	(VIDEO_FIRMWARE_ROW_SIZE)
#define VIDEO_L8_FIRMWARE_ROW_SIZE_OF_IPC	(VIDEO_FIRMWARE_ROW_SIZE)
#define VIDEO_L8_FIRMWARE_ROW_SIZE_IPC_MC	(1)
#define VIDEO_L8_FIRMWARE_ROW_SIZE_DDR_UP	(1)
#define VIDEO_L8_FIRMWARE_ROW_SIZE_OF_TTX	(1)
#define VIDEO_L8_FIRMWARE_ROW_SIZE_REG_TR	(1)
#define VIDEO_L8_ROW_SIZE_OF_FRAME_MEMORY	(VIDEO_L8_FRAME_DDR_MW_CAP + VIDEO_L8_FRAME_DDR_MW_TNR_IPC + VIDEO_L8_FRAME_DDR_MW_SCR + VIDEO_L8_FRAME_DDR_SW_CAP + VIDEO_L8_FRAME_DDR_SW_PIP + VIDEO_L8_FRAME_DDR_SW_VCR + VIDEO_L8_FRAME_DDR_SW_DVR + VIDEO_L8_FRAME_DDR_SW_MDI + VIDEO_L8_FRAME_DDR_AUX_VCS + VIDEO_L8_FRAME_DDR_OSW_OSD)

#define VIDEO_L8_FIRMWARE_ROW_BASE_OF_DE	(VIDEO_L8_FRAME_FIRMWARE_OFFSET/VIDEO_ROW_STRIDE)
#define VIDEO_L8_FIRMWARE_ROW_BASE_OF_LED	(VIDEO_L8_FIRMWARE_ROW_BASE_OF_DE  + VIDEO_L8_FIRMWARE_ROW_SIZE_OF_DE)
#define VIDEO_L8_FIRMWARE_ROW_BASE_OF_IPC	(VIDEO_L8_FIRMWARE_ROW_BASE_OF_LED + VIDEO_L8_FIRMWARE_ROW_SIZE_OF_LED)
#define VIDEO_L8_START_OF_FRAME_MEMORY		(VIDEO_L8_FIRMWARE_ROW_BASE_OF_IPC + VIDEO_L8_FIRMWARE_ROW_SIZE_OF_IPC)

#define VIDEO_L8_FIRMWARE_ROW_BASE_DDR_UP	(VIDEO_L8_FIRMWARE_ROW_BASE_OF_IPC + VIDEO_L8_FIRMWARE_ROW_SIZE_IPC_MC)
#define VIDEO_L8_FIRMWARE_ROW_BASE_OF_TTX	(VIDEO_L8_FIRMWARE_ROW_BASE_DDR_UP + VIDEO_L8_FIRMWARE_ROW_SIZE_DDR_UP)
#define VIDEO_L8_FIRMWARE_ROW_BASE_REG_TR	(VIDEO_L8_FIRMWARE_ROW_BASE_OF_TTX + VIDEO_L8_FIRMWARE_ROW_SIZE_OF_TTX)

#define VIDEO_L8_FIRMWARE_MEM_BASE_OF_DE	VIDEO_L8_ROW2PHY(VIDEO_L8_FIRMWARE_ROW_BASE_OF_DE)
#define VIDEO_L8_FIRMWARE_MEM_BASE_OF_LED	VIDEO_L8_ROW2PHY(VIDEO_L8_FIRMWARE_ROW_BASE_OF_LED)
#define VIDEO_L8_FIRMWARE_MEM_BASE_OF_IPC	VIDEO_L8_ROW2PHY(VIDEO_L8_FIRMWARE_ROW_BASE_OF_IPC)
#define VIDEO_L8_FIRMWARE_MEM_BASE_DDR_UP	VIDEO_L8_ROW2PHY(VIDEO_L8_FIRMWARE_ROW_BASE_DDR_UP)
#define VIDEO_L8_FIRMWARE_MEM_BASE_OF_TTX	VIDEO_L8_ROW2PHY(VIDEO_L8_FIRMWARE_ROW_BASE_OF_TTX)
#define VIDEO_L8_FIRMWARE_MEM_BASE_REG_TR	VIDEO_L8_ROW2PHY(VIDEO_L8_FIRMWARE_ROW_BASE_REG_TR)
#define VIDEO_L8_MBASE_OF_FRAME_MEMORY		VIDEO_L8_ROW2PHY(VIDEO_L8_START_OF_FRAME_MEMORY)

#define VIDEO_L8_FIRMWARE_MEM_SIZE_OF_DE	CONV_ROW2BYTE(VIDEO_L8_FIRMWARE_ROW_SIZE_OF_DE)
#define VIDEO_L8_FIRMWARE_MEM_SIZE_OF_LED	CONV_ROW2BYTE(VIDEO_L8_FIRMWARE_ROW_SIZE_OF_LED)
#define VIDEO_L8_FIRMWARE_MEM_SIZE_OF_IPC	CONV_ROW2BYTE(VIDEO_L8_FIRMWARE_ROW_SIZE_OF_IPC)
#define VIDEO_L8_FIRMWARE_MEM_SIZE_DDR_UP	CONV_ROW2BYTE(VIDEO_L8_FIRMWARE_ROW_SIZE_DDR_UP)
#define VIDEO_L8_FIRMWARE_MEM_SIZE_OF_TTX	CONV_ROW2BYTE(VIDEO_L8_FIRMWARE_ROW_SIZE_OF_TTX)
#define VIDEO_L8_FIRMWARE_MEM_SIZE_REG_TR	CONV_ROW2BYTE(VIDEO_L8_FIRMWARE_ROW_SIZE_REG_TR)
#define VIDEO_L8_MEM_SIZE_OF_FRAME_MEMORY	CONV_ROW2BYTE(VIDEO_L8_ROW_SIZE_OF_FRAME_MEMORY)

/*----------------------------------------------------------------------------------------
 *	 Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 External Variables
 *---------------------------------------------------------------------------------------*/

#endif	 /* ----- #ifndef DE_CFG_L8_INC  ----- */
/**  @} */
