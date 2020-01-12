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
 *  main configuration file for de device
 *  de device will teach you how to make device driver with new platform.
 *
 *  author      dongho7.park (dongho7.park@lge.com)
 *  version     1.0
 *  date        2009.12.30
 *  note        Additional information.
 *
 *  @addtogroup lg1150_de
 *  @{
 */

/*----------------------------------------------------------------------------------------
    Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "de_cfg.h"
#include "l8/de_cfg_l8.h"
#include "l9/de_cfg_l9.h"

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
LX_DE_MEM_CFG_T gMemCfgDe[] =
{
    // for LG1150 A0 real chip.
    {
        .frame_name = "ddr1",
        .frame_base = VIDEO_L8_MBASE_OF_FRAME_MEMORY,
        .frame_size = VIDEO_L8_MEM_SIZE_OF_FRAME_MEMORY,
        {
            {   "fw_de",  VIDEO_L8_FIRMWARE_MEM_BASE_OF_DE,  VIDEO_L8_FIRMWARE_MEM_SIZE_OF_DE  } ,
            {   "fw_led", VIDEO_L8_FIRMWARE_MEM_BASE_OF_LED, VIDEO_L8_FIRMWARE_MEM_SIZE_OF_LED } ,
            {   "fw_ipc", VIDEO_L8_FIRMWARE_MEM_BASE_OF_IPC, VIDEO_L8_FIRMWARE_MEM_SIZE_OF_IPC } ,
        }
    },

    // for LG1150 A0 FPGA
    {
        .frame_name = "ddr1",
        .frame_base = VIDEO_L8_ROW2PHY(8570),
        .frame_size = VIDEO_L8_MEM_SIZE_OF_FRAME_MEMORY,
        {
            {   "fw_de",  0x25e00000, VIDEO_L8_FIRMWARE_MEM_SIZE_OF_DE  } ,
            {   "fw_led", 0x25d00000, VIDEO_L8_FIRMWARE_MEM_SIZE_OF_LED } ,
            {   "fw_ipc", 0x25e80000, VIDEO_L8_FIRMWARE_MEM_SIZE_OF_IPC } ,
        }
    },

    // for LG1152 A0/B0 real chip.
    {
        .frame_name = "ddr2",
        .frame_base = VIDEO_L9_MBASE_OF_FRAME_MEMORY,
        .frame_size = VIDEO_L9_MEM_SIZE_OF_FRAME_MEMORY,
        {
            {   "fw_de",  VIDEO_L9_FIRMWARE_MEM_BASE_OF_DE,  VIDEO_L9_FIRMWARE_MEM_SIZE_OF_DE  } ,
            {   "fw_led", VIDEO_L9_FIRMWARE_MEM_BASE_OF_LED, VIDEO_L9_FIRMWARE_MEM_SIZE_OF_LED } ,
            {   "fw_ipc", VIDEO_L9_FIRMWARE_MEM_BASE_OF_IPC, VIDEO_L9_FIRMWARE_MEM_SIZE_OF_IPC } ,
        }
    }
};

//LX_DE_MEM_CFG_T *gpDeMem = &gMemCfgDe[1];   // for FPGA.
LX_DE_MEM_CFG_T *gpDeMem = &gMemCfgDe[0];   // for COSMO.
//LX_DE_MEM_CFG_T *gpDeMem = &gMemCfgDe[2];   // for GP3 L9.

/*----------------------------------------------------------------------------------------
    Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
    Implementation Group
========================================================================================*/

/** @} */

