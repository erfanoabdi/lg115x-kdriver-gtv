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
 * author     youngki.lyu (youngki.lyu@lge.com)
 * version    1.0
 * date       2010.04.30
 * note       Additional information.
 *
 * @addtogroup lg1152_vdec
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "adec_ipc_mem.h"
#include "adec_ipc_dbg.h"

#ifdef __XTENSA__
#include <stdio.h>
#include <xtensa/xtruntime.h>
#include "adec_mcu_hal.h"
#else
#include <linux/kernel.h>
#include "../../adec_drv.h"
#endif


/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_MEM_NUM_OF_DIVIDE				64

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

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
static struct
{
	UINT32	ui32MemPtr;
	UINT32	ui32MemSize;

	UINT32	ui32UsedFragment[ADEC_IPC_MEM_NUM_OF_DIVIDE/32 + 1];	// Bit Flag
	UINT32	ui32_1FragmentSize;

	UINT32	ui32_AllocedFragmentSize[ADEC_IPC_MEM_NUM_OF_DIVIDE + 1];
} gsIpcMem;


/**
********************************************************************************
* @brief
*
* @remarks
*  DETAIL INFORMATION
* @par requirements:
*
* @param
*
* @return
*
********************************************************************************
*/
void ADEC_IPC_MEM_Init(UINT32 ui32MemPtr, UINT32 ui32MemSize)
{
	UINT32	i;

	gsIpcMem.ui32MemPtr = ui32MemPtr;
	gsIpcMem.ui32MemSize = ui32MemSize;

	for( i = 0; i < (ADEC_IPC_MEM_NUM_OF_DIVIDE/32); i++ )
		gsIpcMem.ui32UsedFragment[i] = 0;
	gsIpcMem.ui32_1FragmentSize = ui32MemSize/ADEC_IPC_MEM_NUM_OF_DIVIDE;

	for( i = 0; i < ADEC_IPC_MEM_NUM_OF_DIVIDE; i++ )
		gsIpcMem.ui32_AllocedFragmentSize[i] = 0;
}

/**
********************************************************************************
* @brief
*
* @remarks
*  DETAIL INFORMATION
* @par requirements:
*
* @param
*
* @return
*
********************************************************************************
*/
UINT32 ADEC_IPC_MEM_Malloc(UINT32 ui32MemSize)
{
	UINT32	ui32NeedFragments;
	UINT32	ui32FragmentsStart, ui32FragmentsCount;
	UINT32	i;

	if( gsIpcMem.ui32MemPtr == 0 )	// Not Initialised
		return 0x0;

	ui32NeedFragments = (ui32MemSize + (gsIpcMem.ui32_1FragmentSize - 1)) / gsIpcMem.ui32_1FragmentSize;

	ui32FragmentsStart = ADEC_IPC_MEM_NUM_OF_DIVIDE;	// invalid value
	ui32FragmentsCount = 0;
	for( i = 0; i < ADEC_IPC_MEM_NUM_OF_DIVIDE; i++ )
	{
		if( gsIpcMem.ui32UsedFragment[i / 32] & (0x1 << (i % 32)) )
		{
			ui32FragmentsStart = ADEC_IPC_MEM_NUM_OF_DIVIDE;	// invalid value
			ui32FragmentsCount = 0;
		}
		else
		{
			if( ui32FragmentsStart == ADEC_IPC_MEM_NUM_OF_DIVIDE )
			{
				ui32FragmentsStart = i;
				ui32FragmentsCount = 1;
			}
			else
			{
				ui32FragmentsCount++;
			}
		}

		if( ui32FragmentsCount >= ui32NeedFragments )
			break;
	}
	if( i == ADEC_IPC_MEM_NUM_OF_DIVIDE )
	{
		ADEC_KDRV_Message(ERROR,  "[VDEC][Err] Failed to Alloc ADEC_ADEC_IPC Memory: %d, %s", ui32MemSize, __FUNCTION__ );
		return 0x0;
	}

	for( i = ui32FragmentsStart; i < (ui32FragmentsStart + ui32FragmentsCount); i++ )
	{
		gsIpcMem.ui32UsedFragment[i / 32] |= (0x1 << (i % 32));
	}
	gsIpcMem.ui32_AllocedFragmentSize[ui32FragmentsStart] = ui32FragmentsCount;

	return (gsIpcMem.ui32MemPtr + (gsIpcMem.ui32_1FragmentSize * ui32FragmentsStart));
}

/**
********************************************************************************
* @brief
*
* @remarks
*  DETAIL INFORMATION
* @par requirements:
*
* @param
*
* @return
*
********************************************************************************
*/
void ADEC_IPC_MEM_Free(UINT32 ui32MemPtr)
{
	UINT32	ui32MemOffset;
	UINT32	ui32FragmentsStart, ui32FragmentsCount;
	UINT32	i;

	if( gsIpcMem.ui32MemPtr == 0 )	// Not Initialised
		return;

	ui32MemOffset = gsIpcMem.ui32MemPtr - ui32MemPtr;

	ui32FragmentsStart = ui32MemOffset / gsIpcMem.ui32_1FragmentSize;

	ui32FragmentsCount = gsIpcMem.ui32_AllocedFragmentSize[ui32FragmentsStart];
	if( ui32FragmentsCount == 0 )
	{
		ADEC_KDRV_Message(ERROR,  "[VDEC][Err] Failed to Free ADEC_ADEC_IPC Memory: 0x%X, %s", ui32MemPtr, __FUNCTION__ );
		return;
	}

	for( i = ui32FragmentsStart; i < (ui32FragmentsStart + ui32FragmentsCount); i++ )
	{
		gsIpcMem.ui32UsedFragment[i / 32] &= ~(0x1 << (i % 32));
	}
	gsIpcMem.ui32_AllocedFragmentSize[ui32FragmentsStart] = 0;
}

/** @} */

