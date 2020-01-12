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
 * date       2010.05.08
 * note       Additional information.
 *
 * @addtogroup lg1152_vdec
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_CMD_BUF_SIZE			0x2000
#define		ADEC_IPC_CMD_NUM_OF_HEADER		0x30

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "adec_ipc_cmd.h"
#include "adec_ipc_reg_api.h"
#include "adec_ipc_mem.h"
#include "adec_ipc_dbg.h"
#include "adec_ipc_req.h"
#include "adec_ipc_list.h"

#ifdef __XTENSA__
#include "adec_mcu_hal.h"
#else
#include <linux/kernel.h>
#include <asm/io.h> //ioremap
#include <asm/string.h>	// memset
#include "../../adec_drv.h"
#include "../adec_coredrv_l9.h"
#endif


/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_CMD_HEADER_INVALID		0x0FFF1234
#define		ADEC_IPC_CMD_MAGIC_CODE			0x01444542
#define		ADEC_IPC_CMD_PADDING_BITS		0x03DF9ADE


/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_CMD_NEXT_OFFSET( _offset, _bufsize )	\
				((((_offset) + 4) >= (_bufsize)) ? 0 : (_offset) + 4)
#define		ADEC_IPC_CMD_INCREASE_OFFSET( _offset, _nodesize, _bufsize )	\
				((((_offset) + (ADEC_IPC_REG_CEILING_4BYTES(_nodesize))) >= (_bufsize)) ? 0 : (_offset) + (ADEC_IPC_REG_CEILING_4BYTES(_nodesize)))

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
typedef struct
{
	UINT32 		ui32PhyBaseAddr;	// constant
	UINT32 		*pui32VirBasePtr;	// constant
	UINT32 		ui32BufSize;			// constant

#ifdef __XTENSA__
	struct
	{
		UINT32								ui32HdrId;
		ADEC_IPC_CMD_RECEIVER_CALLBACK_T 		fpCallback;
	} sReceive[ADEC_IPC_CMD_NUM_OF_HEADER+1];
#endif
}GS_IPC_CMD;

static GS_IPC_CMD gsIpcCmd =
{
	.ui32PhyBaseAddr	= 0,
	.pui32VirBasePtr	= NULL,
	.ui32BufSize		= 0,
};


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
void ADEC_IPC_CMD_Init(void)
{
	UINT32	ui32MemPtr;
	UINT32	ui32MemSize;
#ifdef __XTENSA__
	UINT32 i;

	ui32MemPtr = ADEC_IPC_REG_CMD_GetBaseAddr();
	ui32MemSize = ADEC_IPC_REG_CMD_GetBufSize();

	gsIpcCmd.pui32VirBasePtr = (UINT32*)ADEC_MCU_HAL_TranslateAddrforMCU(ui32MemPtr, ui32MemSize);

	for( i = 0; i <= ADEC_IPC_CMD_NUM_OF_HEADER; i++ )
	{
		gsIpcCmd.sReceive[i].ui32HdrId = ADEC_IPC_CMD_HEADER_INVALID;
		gsIpcCmd.sReceive[i].fpCallback = NULL;
	}

	ADEC_IPC_REG_CMD_SetRdOffset(ADEC_IPC_REG_CMD_GetWrOffset());


#else
	ui32MemPtr = ADEC_IPC_MEM_Malloc(ADEC_IPC_CMD_BUF_SIZE);
	ui32MemSize = ADEC_IPC_CMD_BUF_SIZE;

	ADEC_DEBUG("IPC_CMD [0x%08X (0x%08X)]\n", ui32MemPtr, ui32MemSize);

	if(gsIpcCmd.pui32VirBasePtr == NULL)
	{
		gsIpcCmd.pui32VirBasePtr = ioremap(ui32MemPtr, ui32MemSize);
	}

	memset(gsIpcCmd.pui32VirBasePtr , 0x0, ui32MemSize);

	ADEC_IPC_REG_CMD_SetBaseAddr(ui32MemPtr);
	ADEC_IPC_REG_CMD_SetBufSize(ui32MemSize);

	ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][CMD] Base:0x%X, Size: 0x%X", ui32MemPtr, ui32MemSize );

	ADEC_IPC_REG_CMD_SetWrOffset(0);
	ADEC_IPC_REG_CMD_SetRdOffset(0);
#endif

	gsIpcCmd.ui32PhyBaseAddr = ui32MemPtr;
	gsIpcCmd.ui32BufSize = ui32MemSize;
}

void ADEC_IPC_Init(UINT32 ui32QAddr, UINT32 ui32Qsize)
{

	ADEC_IPC_MEM_Init(ui32QAddr, ui32Qsize);
	ADEC_IPC_DBG_Init();
	ADEC_IPC_REQ_Init();
	ADEC_IPC_CMD_Init();
}

void ADEC_IPC_Reset(UINT8 ui8lqCh)
{

}

#ifndef __XTENSA__
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
static BOOLEAN _ADEC_IPC_CMD_CheckBelongToAddress(UINT32 ui32StartAddr, UINT32 ui32EndAddr, UINT32 ui32TargetAddr)
{
	if( ui32StartAddr <= ui32EndAddr )
	{
		if( (ui32TargetAddr > ui32StartAddr) &&
			(ui32TargetAddr <= ui32EndAddr) )
			return TRUE;
	}
	else
	{
		if( (ui32TargetAddr > ui32StartAddr) ||
			(ui32TargetAddr <= ui32EndAddr) )
			return TRUE;
	}

	return FALSE;
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
static UINT32 _ADEC_IPC_CMD_CheckOverflow(UINT32 ui32CmdSize)
{
	UINT32		ui32WrOffset, ui32WrOffset_Org;
	UINT32		ui32RdOffset;

	if( ui32CmdSize > gsIpcCmd.ui32BufSize )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err]CMD] Overflow - Too Big CMD Message Size: %d", ui32CmdSize );
		return ADEC_IPC_REG_INVALID_OFFSET;
	}

	ui32WrOffset_Org = ADEC_IPC_REG_CMD_GetWrOffset();
	ui32WrOffset = ui32WrOffset_Org;
	ui32RdOffset = ADEC_IPC_REG_CMD_GetRdOffset();

	if( (ui32WrOffset + ui32CmdSize) >= gsIpcCmd.ui32BufSize )
	{
		ui32WrOffset = 0;
	}

	if( _ADEC_IPC_CMD_CheckBelongToAddress(ui32WrOffset_Org, ui32WrOffset + ui32CmdSize, ui32RdOffset) == TRUE )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][CMD] Overflow - Write:0x%X, Size:0x%X, Read:0x%X", ui32WrOffset, ui32CmdSize, ui32RdOffset );
		return ADEC_IPC_REG_INVALID_OFFSET;
	}

	if( ui32WrOffset != ui32WrOffset_Org )
	{
		UINT32		i, *pui32WrVirPtr;

		pui32WrVirPtr = gsIpcCmd.pui32VirBasePtr + (ui32WrOffset_Org>>2);
		for( i = 0; i < ((gsIpcCmd.ui32BufSize -ui32WrOffset_Org) >> 2); i++ )
		{
			*pui32WrVirPtr = ADEC_IPC_CMD_PADDING_BITS;
			pui32WrVirPtr++;
		}
	}

	return ui32WrOffset;
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
BOOLEAN ADEC_IPC_CMD_Send(UINT32 ui32HdrId, UINT32 ui32BodySize, void *pIpcBody)
{
	UINT32		*pui32WrVirPtr;
	const UINT32	ui32CmdHeaderSize = 12;	// 4 * 3 : Header Size(MAGIC CODE + Header Type + Body Length)
	UINT32		ui32WrOffset;


	if( !gsIpcCmd.pui32VirBasePtr )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_IPC][Err][CMD] Not Initialised" );
		return FALSE;
	}

	// 1. Check Buffer Overflow
	ui32WrOffset = _ADEC_IPC_CMD_CheckOverflow(ui32CmdHeaderSize + ui32BodySize);
	if( ui32WrOffset == ADEC_IPC_REG_INVALID_OFFSET )
		return FALSE;

	pui32WrVirPtr = gsIpcCmd.pui32VirBasePtr;

	// 2. Write Header
	pui32WrVirPtr[ui32WrOffset>>2] = ADEC_IPC_CMD_MAGIC_CODE;
	ui32WrOffset = ADEC_IPC_CMD_NEXT_OFFSET(ui32WrOffset, gsIpcCmd.ui32BufSize);
	pui32WrVirPtr[ui32WrOffset>>2] = ui32HdrId;
	ui32WrOffset = ADEC_IPC_CMD_NEXT_OFFSET(ui32WrOffset, gsIpcCmd.ui32BufSize);

	// 3. Write Length
	pui32WrVirPtr[ui32WrOffset>>2] = ui32BodySize;
	ui32WrOffset = ADEC_IPC_CMD_NEXT_OFFSET(ui32WrOffset, gsIpcCmd.ui32BufSize);

	// 4. Write Body
	memcpy((void *)&pui32WrVirPtr[ui32WrOffset>>2], (void *)pIpcBody, ui32BodySize);
	ui32WrOffset = ADEC_IPC_CMD_INCREASE_OFFSET(ui32WrOffset, ui32BodySize, gsIpcCmd.ui32BufSize);

	// 5. Update Write ADEC_ADEC_IPC Register
	ADEC_IPC_REG_CMD_SetWrOffset(ui32WrOffset);

	ADEC_L9_SetDSP1IntrEvent();

	return TRUE;
}

#else
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
BOOLEAN ADEC_IPC_CMD_Register_ReceiverCallback(UINT32 ui32HdrId, ADEC_IPC_CMD_RECEIVER_CALLBACK_T fpCallback)	// for MCU
{
	UINT32	i;

	for( i = 0; i < ADEC_IPC_CMD_NUM_OF_HEADER; i++ )
	{
		if( gsIpcCmd.sReceive[i].ui32HdrId == ADEC_IPC_CMD_HEADER_INVALID )
			break;
	}
	if( i == ADEC_IPC_CMD_NUM_OF_HEADER )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_IPC][Err][CMD] Not Enough Header Space %d", ADEC_IPC_CMD_NUM_OF_HEADER );
		return FALSE;
	}

	gsIpcCmd.sReceive[i].ui32HdrId = ui32HdrId;
	gsIpcCmd.sReceive[i].fpCallback = fpCallback;

	return TRUE;
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
static UINT32 _ADEC_IPC_CMD_VerifyMagicCode(UINT32 *pui32RdVirPtr, UINT32 ui32RdOffset)
{
	if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_CMD_MAGIC_CODE )
	{
		return ADEC_IPC_CMD_NEXT_OFFSET(ui32RdOffset, gsIpcCmd.ui32BufSize);
	}

//	ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][Dbg][CMD] Not Found MAGIC CODE - Read Offset: 0x%X", ui32RdOffset );

	if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_CMD_PADDING_BITS )
	{
//		ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][Dbg][CMD] PADDING BITS - Read Offset: 0x%X", ui32RdOffset );

		ui32RdOffset = 0;
		if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_CMD_MAGIC_CODE )
		{
			return ADEC_IPC_CMD_NEXT_OFFSET(ui32RdOffset, gsIpcCmd.ui32BufSize);
		}
	}

	ADEC_KDRV_Message(ERROR, "[ADEC_IPC][Err][CMD] No MAGIC CODE: 0x%X - Read Offset: 0x%X", pui32RdVirPtr[ui32RdOffset>>2], ui32RdOffset );

	return ADEC_IPC_REG_INVALID_OFFSET;
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
UINT32 ADEC_IPC_CMD_Receive(void)
{
	UINT32		ui32WrOffset;	// byte size
	UINT32		ui32RdOffset;	// byte size
	UINT32		*pui32RdVirPtr;
	UINT32		ui32HdrId;
	UINT32		ui32BodySize;
	UINT32		i;

	if( !gsIpcCmd.pui32VirBasePtr )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_IPC][Err][CMD] Not Initialised" );
		return 0;
	}

	ui32WrOffset = ADEC_IPC_REG_CMD_GetWrOffset();
	ui32RdOffset = ADEC_IPC_REG_CMD_GetRdOffset();

	if( ui32RdOffset == ui32WrOffset )
		return 0;

	pui32RdVirPtr = gsIpcCmd.pui32VirBasePtr;

	ADEC_MCU_HAL_Invalidate((void*)(pui32RdVirPtr + (ui32RdOffset >> 2)), 12);
	// 1. Check Magic Code
	ui32RdOffset = _ADEC_IPC_CMD_VerifyMagicCode(pui32RdVirPtr, ui32RdOffset);
	if( ui32RdOffset == ADEC_IPC_REG_INVALID_OFFSET )
	{
		//ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][CMD] No MAGIC Code" );
		return 0;
	}

	// 2. Read Header
	ui32HdrId = pui32RdVirPtr[ui32RdOffset>>2];
	ui32RdOffset = ADEC_IPC_CMD_NEXT_OFFSET(ui32RdOffset, gsIpcCmd.ui32BufSize);

	for( i = 0; i < ADEC_IPC_CMD_NUM_OF_HEADER; i++ )
	{
		if( gsIpcCmd.sReceive[i].ui32HdrId == ui32HdrId )
			break;
	}
	if( i == ADEC_IPC_CMD_NUM_OF_HEADER )
	{
		//ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][CMD] Not Enough Header Space %d", ADEC_IPC_CMD_NUM_OF_HEADER );
		//return 0;
		i = i ;
	}

	// 3. Read Length
	ui32BodySize = pui32RdVirPtr[ui32RdOffset>>2];
	ui32RdOffset = ADEC_IPC_CMD_NEXT_OFFSET(ui32RdOffset, gsIpcCmd.ui32BufSize);

	// 4. Call Registered Callback Function
	if( gsIpcCmd.sReceive[i].fpCallback == NULL )
	{
		//ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][CMD] Not Registered Callback Function of Header Type(%d)", pui32RdVirPtr[ui32RdOffset/4] );
	}
	else
	{
		ADEC_MCU_HAL_Invalidate((void*)(pui32RdVirPtr +(ui32RdOffset>>2)) , ui32BodySize);
		gsIpcCmd.sReceive[i].fpCallback( ui32HdrId, ui32BodySize, (void *)&pui32RdVirPtr[ui32RdOffset>>2] );
	}
	ui32RdOffset = ADEC_IPC_CMD_INCREASE_OFFSET(ui32RdOffset, ui32BodySize, gsIpcCmd.ui32BufSize);

	// 5. Update Read ADEC_ADEC_IPC Register
	ADEC_IPC_REG_CMD_SetRdOffset(ui32RdOffset);

	return 1;
}
#endif

/** @} */

