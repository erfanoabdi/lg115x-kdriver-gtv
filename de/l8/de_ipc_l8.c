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

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <asm-generic/errno-base.h>
#include <linux/delay.h>
#include <linux/string.h>

#include <linux/workqueue.h>
#include <mach/platform.h>

#include "os_util.h"

#include "de_model.h"
#include "de_kapi.h"
#include "de_def.h"
#include "l8/de_reg_def_l8.h"
#include "l8/de_ipc_def_l8.h"

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
	global Functions
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
 *   Implementation Group
 *=======================================================================================*/
/**
 * @callgraph
 * @callergraph
 *
 * @brief initialize IPC
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_IPC_L8_Init(UINT32 *pVideoBuffToMCU, UINT32 *pVideoBuffFrMCU)
{
#ifdef USE_QEMU_SYSTEM
   *pVideoBuffToMCU     = (UINT32)OS_KMalloc(DTVSOC_IPC_FROM_CPU_SIZE);
   *pVideoBuffFrMCU     = (UINT32)OS_KMalloc(DTVSOC_IPC_FROM_MCU_SIZE);
#else
   *pVideoBuffToMCU = DTVSOC_IPC_FROM_CPU_BASE;
   *pVideoBuffFrMCU = DTVSOC_IPC_FROM_MCU_BASE;
#endif

	return RET_OK;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief initialize IPC
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_IPC_L8_Free(UINT32 *pVideoBuffToMCU, UINT32 *pVideoBuffFrMCU)
{
#ifdef USE_QEMU_SYSTEM
   OS_Free(*pVideoBuffToMCU);
   OS_Free(*pVideoBuffFrMCU);
#endif

	return RET_OK;
}

/** @} */
