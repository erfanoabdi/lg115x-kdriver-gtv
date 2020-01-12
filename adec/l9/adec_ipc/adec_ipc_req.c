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
#define		ADEC_IPC_REQ_BUF_SIZE				0x2000
#define		ADEC_IPC_REQ_NUM_OF_HEADER			0x30

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "adec_ipc_req.h"
#include "adec_ipc_reg_api.h"
#include "adec_ipc_mem.h"
#include "adec_ipc_dbg.h"

#ifdef __XTENSA__
#include <stdio.h>
#include "stdarg.h"
#include "adec_mcu_hal.h"
#else
#include <linux/kernel.h>
#include <asm/io.h> //ioremap
#include <asm/string.h>	// memset
#include "../../adec_drv.h"
#endif



/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_REQ_HEADER_INVALID	0xFFFF1234
#define		ADEC_IPC_REQ_MAGIC_CODE		0x19C08E90
#define		ADEC_IPC_REQ_PADDING_BITS		0x8E9F9ADE

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_REQ_NEXT_OFFSET( _offset, _bufsize )	\
				((((_offset) + 4) >= (_bufsize)) ? 0 : (_offset) + 4)
#define		ADEC_IPC_REQ_INCREASE_OFFSET( _offset, _nodesize, _bufsize )	\
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

#ifndef __XTENSA__
	struct
	{
		UINT32								ui32HdrId;
		ADEC_IPC_REQ_RECEIVER_CALLBACK_T 		fpCallback;
	} sReceive[ADEC_IPC_REQ_NUM_OF_HEADER+1];
#endif
}GS_IPC_REQ;

static GS_IPC_REQ gsIpcReq =
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
void ADEC_IPC_REQ_Init(void)
{
	UINT32	ui32MemPtr;
	UINT32	ui32MemSize;
#ifndef __XTENSA__
	UINT32 i;
#endif

#ifdef __XTENSA__
	ui32MemPtr = ADEC_IPC_REG_REQ_GetBaseAddr();
	ui32MemSize = ADEC_IPC_REG_REQ_GetBufSize();

	gsIpcReq.pui32VirBasePtr = (UINT32*)ADEC_MCU_HAL_TranslateAddrforMCU(ui32MemPtr, ui32MemSize);
#else
	ui32MemPtr = ADEC_IPC_MEM_Malloc(ADEC_IPC_REQ_BUF_SIZE);
	ui32MemSize = ADEC_IPC_REQ_BUF_SIZE;

	ADEC_DEBUG("IPC_REQ [0x%08X (0x%08X)]\n", ui32MemPtr, ui32MemSize);

	if(gsIpcReq.pui32VirBasePtr == NULL)
	{
		gsIpcReq.pui32VirBasePtr = ioremap(ui32MemPtr, ui32MemSize);
	}

	memset(gsIpcReq.pui32VirBasePtr , 0x0, ui32MemSize);

	for( i = 0; i <= ADEC_IPC_REQ_NUM_OF_HEADER; i++ )
	{
		gsIpcReq.sReceive[i].ui32HdrId = ADEC_IPC_REQ_HEADER_INVALID;
		gsIpcReq.sReceive[i].fpCallback = NULL;
	}

	ADEC_IPC_REG_REQ_SetBaseAddr(ui32MemPtr);
	ADEC_IPC_REG_REQ_SetBufSize(ui32MemSize);

	ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][REQ] Base:0x%X, Size: 0x%X", ui32MemPtr, ui32MemSize );
#endif

	ADEC_IPC_REG_REQ_SetWrOffset(0);
	ADEC_IPC_REG_REQ_SetRdOffset(0);

	gsIpcReq.ui32PhyBaseAddr = ui32MemPtr;
	gsIpcReq.ui32BufSize = ui32MemSize;
}

#ifdef __XTENSA__
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
static BOOLEAN _ADEC_IPC_REQ_CheckBelongToAddress(UINT32 ui32StartAddr, UINT32 ui32EndAddr, UINT32 ui32TargetAddr)
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
static UINT32 _ADEC_IPC_REQ_CheckOverflow(UINT32 ui32ReqSize)
{
	UINT32		ui32WrOffset, ui32WrOffset_Org;
	UINT32		ui32RdOffset;

	if( ui32ReqSize > gsIpcReq.ui32BufSize )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err]REQ] Overflow - Too Big REQ Message Size: %d", ui32ReqSize );
		return ADEC_IPC_REG_INVALID_OFFSET;
	}

	ui32WrOffset_Org = ADEC_IPC_REG_REQ_GetWrOffset();
	ui32WrOffset = ui32WrOffset_Org;
	ui32RdOffset = ADEC_IPC_REG_REQ_GetRdOffset();
	if( (ui32WrOffset + ui32ReqSize) >= gsIpcReq.ui32BufSize )
	{
		ui32WrOffset = 0;
	}

	if( _ADEC_IPC_REQ_CheckBelongToAddress(ui32WrOffset_Org, ui32WrOffset + ui32ReqSize, ui32RdOffset) == TRUE )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][REQ] Overflow - Write:0x%X, Size:0x%X, Read:0x%X", ui32WrOffset, ui32ReqSize, ui32RdOffset );
		return ADEC_IPC_REG_INVALID_OFFSET;
	}

	if( ui32WrOffset != ui32WrOffset_Org )
	{
		UINT32		i, *pui32WrVirPtr, *paddingStart;

		paddingStart = pui32WrVirPtr = gsIpcReq.pui32VirBasePtr + (ui32WrOffset_Org>>2);
		for( i = 0; i < ((gsIpcReq.ui32BufSize -ui32WrOffset_Org) >> 2); i++ )
		{
			*pui32WrVirPtr = ADEC_IPC_REQ_PADDING_BITS;
			pui32WrVirPtr++;
		}
		ADEC_MCU_HAL_WriteBack(paddingStart, (gsIpcReq.ui32BufSize -ui32WrOffset_Org));
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
BOOLEAN ADEC_IPC_REQ_Send(UINT32 ui32HdrId, UINT32 ui32BodySize, void *pIpcBody)
{
	UINT32		*pui32WrVirPtr;
	UINT32		*cmdPtr;
	const UINT32	ui32ReqHeaderSize = 12;	// 4 * 3 : Header Size(MAGIC CODE + Header Type + Body Length)
	UINT32		ui32WrOffset;

	if( !gsIpcReq.pui32VirBasePtr )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][REQ] Not Initialised" );
		return FALSE;
	}


	// 1. Check Buffer Overflow
	ui32WrOffset = _ADEC_IPC_REQ_CheckOverflow(ui32ReqHeaderSize + ui32BodySize);
	if( ui32WrOffset == ADEC_IPC_REG_INVALID_OFFSET )
		return FALSE;

	pui32WrVirPtr = gsIpcReq.pui32VirBasePtr;

	cmdPtr = pui32WrVirPtr + (ui32WrOffset >> 2);

	// 2. Write Header
	pui32WrVirPtr[ui32WrOffset>>2] = ADEC_IPC_REQ_MAGIC_CODE;
	ui32WrOffset = ADEC_IPC_REQ_NEXT_OFFSET(ui32WrOffset, gsIpcReq.ui32BufSize);
	pui32WrVirPtr[ui32WrOffset>>2] = ui32HdrId;
	ui32WrOffset = ADEC_IPC_REQ_NEXT_OFFSET(ui32WrOffset, gsIpcReq.ui32BufSize);

	// 3. Write Length
	pui32WrVirPtr[ui32WrOffset>>2] = ui32BodySize;
	ui32WrOffset = ADEC_IPC_REQ_NEXT_OFFSET(ui32WrOffset, gsIpcReq.ui32BufSize);

	// 4. Write Body
	memcpy((void *)&pui32WrVirPtr[ui32WrOffset>>2], (void *)pIpcBody, ui32BodySize);
	ui32WrOffset = ADEC_IPC_REQ_INCREASE_OFFSET(ui32WrOffset, ui32BodySize, gsIpcReq.ui32BufSize);

	// 5. Update Write ADEC_ADEC_IPC Register
	ADEC_IPC_REG_REQ_SetWrOffset(ui32WrOffset);
	ADEC_IPC_REG_Writeback();
	ADEC_MCU_HAL_WriteBack(cmdPtr, ui32BodySize + 12);

	ADEC_MCU_HAL_SendRequest();

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
BOOLEAN ADEC_IPC_REQ_Register_ReceivingCallback(UINT32 ui32HdrId, ADEC_IPC_REQ_RECEIVER_CALLBACK_T fpCallback)	// for MCU
{
	UINT32	i;

	for( i = 0; i < ADEC_IPC_REQ_NUM_OF_HEADER; i++ )
	{
		if( gsIpcReq.sReceive[i].ui32HdrId == ADEC_IPC_REQ_HEADER_INVALID )
			break;
	}
	if( i == ADEC_IPC_REQ_NUM_OF_HEADER )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][REQ] Not Enough Header Space %d", ADEC_IPC_REQ_NUM_OF_HEADER );
		return FALSE;
	}

	gsIpcReq.sReceive[i].ui32HdrId = ui32HdrId;
	gsIpcReq.sReceive[i].fpCallback = fpCallback;

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
static UINT32 _ADEC_IPC_REQ_VerifyMagicCode(UINT32 *pui32RdVirPtr, UINT32 ui32RdOffset)
{
	if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_REQ_MAGIC_CODE )
	{
		return ADEC_IPC_REQ_NEXT_OFFSET(ui32RdOffset, gsIpcReq.ui32BufSize);
	}

//	ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][Dbg][REQ] Not Found MAGIC CODE - Read Offset: 0x%X", ui32RdOffset );

	if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_REQ_PADDING_BITS )
	{
//		ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][Dbg][REQ] PADDING BITS - Read Offset: 0x%X", ui32RdOffset );

		ui32RdOffset = 0;
		if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_REQ_MAGIC_CODE )
		{
			return ADEC_IPC_REQ_NEXT_OFFSET(ui32RdOffset, gsIpcReq.ui32BufSize);
		}
	}

	ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][REQ] No MAGIC CODE: 0x%X - Read Offset: 0x%X", pui32RdVirPtr[ui32RdOffset>>2], ui32RdOffset );

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
UINT32 ADEC_IPC_REQ_Receive(void)
{
	UINT32		ui32WrOffset;	// byte size
	UINT32		ui32RdOffset;	// byte size
	UINT32		*pui32RdVirPtr;
	UINT32		ui32HdrId;
	UINT32		ui32BodySize;
	UINT32		i;

	if( !gsIpcReq.pui32VirBasePtr )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][REQ] Not Initialised" );
		return 0;
	}

	ui32WrOffset = ADEC_IPC_REG_REQ_GetWrOffset();
	ui32RdOffset = ADEC_IPC_REG_REQ_GetRdOffset();

	if( ui32RdOffset == ui32WrOffset )
		return 0;

	pui32RdVirPtr = gsIpcReq.pui32VirBasePtr;

	// 1. Check Magic Code
	ui32RdOffset = _ADEC_IPC_REQ_VerifyMagicCode(pui32RdVirPtr, ui32RdOffset);
	if( ui32RdOffset == ADEC_IPC_REG_INVALID_OFFSET )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_IPC][Err][REQ] No MAGIC Code" );
		return 0;
	}

	// 2. Read Header
	ui32HdrId = pui32RdVirPtr[ui32RdOffset>>2];
	ui32RdOffset = ADEC_IPC_REQ_NEXT_OFFSET(ui32RdOffset, gsIpcReq.ui32BufSize);

	for( i = 0; i < ADEC_IPC_REQ_NUM_OF_HEADER; i++ )
	{
		if( gsIpcReq.sReceive[i].ui32HdrId == ui32HdrId )
			break;
	}
	if( i == ADEC_IPC_REQ_NUM_OF_HEADER )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][REQ] Not Enough Header Space %d", ADEC_IPC_REQ_NUM_OF_HEADER );
	}

	// 3. Read Length
	ui32BodySize = pui32RdVirPtr[ui32RdOffset>>2];
	ui32RdOffset = ADEC_IPC_REQ_NEXT_OFFSET(ui32RdOffset, gsIpcReq.ui32BufSize);

	// 4. Call Registered Callback Function
	if( gsIpcReq.sReceive[i].fpCallback == NULL )
		ADEC_KDRV_Message(ERROR, "[ADEC_IPC][Err][REQ] Not Registered Callback Function of Header Type(%d)", pui32RdVirPtr[ui32RdOffset>>2] );
	else
		gsIpcReq.sReceive[i].fpCallback( ui32HdrId, ui32BodySize, (void *)&pui32RdVirPtr[ui32RdOffset>>2] );

	ui32RdOffset = ADEC_IPC_REQ_INCREASE_OFFSET(ui32RdOffset, ui32BodySize, gsIpcReq.ui32BufSize);

	// 5. Update Read ADEC_ADEC_IPC Register
	ADEC_IPC_REG_REQ_SetRdOffset(ui32RdOffset);

	return 1;
}
#endif

/** @} */

