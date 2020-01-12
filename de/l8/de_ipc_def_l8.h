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
 * author     jaemo.kim (jaemo.kim@lge.com)
 * version    1.0
 * date       2011.02.23
 * note       Additional information.
 *
 * @addtogroup lg1152_de
 * @{
 */

#ifndef  DE_IPC_DEF_L8_INC
#define  DE_IPC_DEF_L8_INC

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "de_ipc_def.h"

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define DTVSOC_IPC_BASE	 			 (DTVSOC_DE_BASE + 0x1E00)
#define DTVSOC_IPC_FROM_CPU_BASE	 DTVSOC_IPC_BASE
#define DTVSOC_IPC_FROM_MCU_BASE	 (DTVSOC_IPC_FROM_CPU_BASE + DTVSOC_IPC_FROM_CPU_SIZE)
#define VIDEO_REG_BASE				 DTVSOC_DE_BASE
#define VIDEO_REG_SIZE               0x2000

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef DE_IPC_DEF_L8_INC  ----- */
/**  @} */
