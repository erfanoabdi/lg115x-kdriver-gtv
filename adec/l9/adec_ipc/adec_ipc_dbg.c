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
#define		ADEC_IPC_DBG_BUF_SIZE				0x8000

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "adec_ipc_dbg.h"
#include "adec_ipc_reg_api.h"
#include "adec_ipc_mem.h"

#ifdef __XTENSA__
#include <stdio.h>
#include "stdarg.h"
#include "adec_mcu_hal.h"
#else
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <asm/io.h> //ioremap
#include <asm/string.h>	// memset
#include "../../adec_drv.h"
#endif


/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_DBG_MAGIC_CODE			0x19C0DB90
#define		ADEC_IPC_DBG_PADDING_BITS		0xDB9F9ADE

#define		IPC_DBG_MAX_STRING_LENGTH	0x100

#define		ADEC_IPC_DBG_ERROR_CODE_01	0xF0000001
#define		ADEC_IPC_DBG_ERROR_CODE_02	0xF0000002
#define		ADEC_IPC_DBG_ERROR_CODE_03	0xF0000003
#define		ADEC_IPC_DBG_ERROR_CODE_04	0xF0000004
#define		ADEC_IPC_DBG_DEBUG_CODE_01	0xF0000011

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define		ADEC_IPC_DBG_NEXT_OFFSET( _offset, _bufsize )	\
				((((_offset) + 4) >= (_bufsize)) ? 0 : (_offset) + 4)
#define		ADEC_IPC_DBG_INCREASE_OFFSET( _offset, _nodesize, _bufsize )	\
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
#ifndef __XTENSA__
struct workqueue_struct *_ADEC_IPC_DBG_workqueue = NULL;

void _ADEC_IPC_DBG_Receive_workfunc(struct work_struct *data);

DECLARE_WORK( _ADEC_IPC_DBG_work, _ADEC_IPC_DBG_Receive_workfunc );

static spinlock_t	stAdecIpcDbgSpinlock;
#endif

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
typedef struct
{
	UINT32 		ui32PhyBaseAddr;	// constant
	UINT32 		*pui32VirBasePtr;	// constant
	UINT32 		ui32BufSize;			// constant	// byte size
}GS_IPC_DBG;

static GS_IPC_DBG gsIpcDbg =
{
	.ui32PhyBaseAddr	= 0,
	.pui32VirBasePtr	= NULL,
	.ui32BufSize		= 0,
};

/*-------------------
 |	MAGIC CODE		|
 --------------------
 |	LENGTH			|
 --------------------
 |	DEBUG			|
 |	STRING			|
 |					|
 --------------------
 |	MAGIC CODE		|
 --------------------
 |	LENGTH			|
 --------------------
 |	DEBUG			|
 |	STRING			|
 |					|
 --------------------
 |	PADDING_BITS	|
 |	PADDING_BITS	|
 ------------------*/


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
void ADEC_IPC_DBG_Init(void)
{
	UINT32	ui32MemPtr;
	UINT32	ui32MemSize;


#ifdef __XTENSA__
	ui32MemPtr = ADEC_IPC_REG_DBG_GetBaseAddr();
	ui32MemSize = ADEC_IPC_REG_DBG_GetBufSize();

	gsIpcDbg.pui32VirBasePtr = (UINT32*)ADEC_MCU_HAL_TranslateAddrforMCU(ui32MemPtr, ui32MemSize);
#else
	ui32MemPtr = ADEC_IPC_MEM_Malloc(ADEC_IPC_DBG_BUF_SIZE);
	ui32MemSize = ADEC_IPC_DBG_BUF_SIZE;

	if(gsIpcDbg.pui32VirBasePtr == NULL)
	{
		gsIpcDbg.pui32VirBasePtr = ioremap(ui32MemPtr, ui32MemSize);
	}

	memset(gsIpcDbg.pui32VirBasePtr , 0x0, ui32MemSize);

	ADEC_IPC_REG_DBG_SetBaseAddr(ui32MemPtr);
	ADEC_IPC_REG_DBG_SetBufSize(ui32MemSize);

	ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][DBG] Base:0x%X, Size: 0x%X", ui32MemPtr, ui32MemSize );

	if(_ADEC_IPC_DBG_workqueue == NULL)
	{
		_ADEC_IPC_DBG_workqueue = create_workqueue("ADEC_IPC_DBG");
		spin_lock_init(&stAdecIpcDbgSpinlock);
	}
#endif

	ADEC_IPC_REG_DBG_SetWrOffset(0);
	ADEC_IPC_REG_DBG_SetRdOffset(0);

	gsIpcDbg.ui32PhyBaseAddr = ui32MemPtr;
	gsIpcDbg.ui32BufSize = ui32MemSize;
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
static BOOLEAN _ADEC_IPC_DBG_CheckBelongToAddress(UINT32 ui32StartAddr, UINT32 ui32EndAddr, UINT32 ui32TargetAddr)
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
static UINT32 _ADEC_IPC_DBG_CheckOverflow(UINT32 ui32DbgSize)	// byte size
{
	UINT32		ui32WrOffset, ui32WrOffset_Org;
	UINT32		ui32RdOffset;

	if( ui32DbgSize > gsIpcDbg.ui32BufSize )
	{
		//ADEC_IPC_REG_DBG_SetBaseAddr(ADEC_IPC_DBG_ERROR_CODE_01);
		ADEC_IPC_REG_DBG_SetBufSize(ui32DbgSize);
//		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err]DBG] Overflow - Too Big DBG Message Size: %d", ui32DbgSize );
		return ADEC_IPC_REG_INVALID_OFFSET;
	}

	ui32WrOffset_Org = ADEC_IPC_REG_DBG_GetWrOffset();
	ui32WrOffset = ui32WrOffset_Org;
	ui32RdOffset = ADEC_IPC_REG_DBG_GetRdOffset();
	if( (ui32WrOffset + ui32DbgSize) >= gsIpcDbg.ui32BufSize )
	{
		ui32WrOffset = 0;
	}

	if( _ADEC_IPC_DBG_CheckBelongToAddress(ui32WrOffset_Org, ui32WrOffset + ui32DbgSize, ui32RdOffset) == TRUE )
	{
		ADEC_IPC_REG_DBG_SetBaseAddr(ADEC_IPC_DBG_ERROR_CODE_02);
		ADEC_IPC_REG_DBG_SetBufSize(ui32WrOffset_Org);
//		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][DBG] Overflow - Write:0x%X, Size:0x%X, Read:0x%X", ui32WrOffset, ui32DbgSize, ui32RdOffset );
		return ADEC_IPC_REG_INVALID_OFFSET;
	}

	if( ui32WrOffset != ui32WrOffset_Org )
	{
		UINT32		i, *pui32WrVirPtr, *wrapPos;

		wrapPos = pui32WrVirPtr = gsIpcDbg.pui32VirBasePtr + (ui32WrOffset_Org>>2);
		for( i = 0; i < ((gsIpcDbg.ui32BufSize -ui32WrOffset_Org) >> 2); i++ )
		{
			*pui32WrVirPtr = ADEC_IPC_DBG_PADDING_BITS;
			pui32WrVirPtr++;
		}

		ADEC_MCU_HAL_WriteBack(wrapPos, gsIpcDbg.ui32BufSize - ui32WrOffset_Org);
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
extern BOOLEAN ADEC_IPC_DBG_Send(const char *fmt, ...)
{
	static char		ui8Buffer[IPC_DBG_MAX_STRING_LENGTH+4];
	__VALIST ap;
	int i, len_byte;
	UINT32		ui32WrOffset;
	UINT32		*pui32WrVirPtr;
	UINT32		*cmdPtr;

	if( !gsIpcDbg.pui32VirBasePtr )
	{
		//ADEC_IPC_REG_DBG_SetBaseAddr(ADEC_IPC_DBG_ERROR_CODE_03);
//		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][CMD] Not Initialised" );
		return FALSE;
	}

	switch( ADEC_IPC_REG_DBG_GetBaseAddr() )
	{
	case ADEC_IPC_DBG_ERROR_CODE_01 :
	case ADEC_IPC_DBG_ERROR_CODE_02 :
	case ADEC_IPC_DBG_ERROR_CODE_03 :
		return FALSE;
		break;
ADEC_IPC_DBG_ERROR_Recovery :
	default :
		break;
	}

	// 1. Calculate Arguments Length
	va_start(ap, fmt);
	len_byte = vsnprintf(ui8Buffer, IPC_DBG_MAX_STRING_LENGTH, fmt, ap);
	va_end(ap);

	ui8Buffer[len_byte] = 0x0;
	len_byte++;

	// 2. Check DBG Buffer Overflow
	ui32WrOffset = _ADEC_IPC_DBG_CheckOverflow(8 + len_byte);	// 8: MAGIC_CODE(4) + Length(4)
	if( ui32WrOffset == ADEC_IPC_REG_INVALID_OFFSET )
		return FALSE;

	pui32WrVirPtr = gsIpcDbg.pui32VirBasePtr;

	cmdPtr = pui32WrVirPtr + (ui32WrOffset >> 2);

	// 3. Write Length
	pui32WrVirPtr[ui32WrOffset>>2] = ADEC_IPC_DBG_MAGIC_CODE;
	ui32WrOffset = ADEC_IPC_DBG_NEXT_OFFSET(ui32WrOffset, gsIpcDbg.ui32BufSize);
	pui32WrVirPtr[ui32WrOffset>>2] = len_byte;
	ui32WrOffset = ADEC_IPC_DBG_NEXT_OFFSET(ui32WrOffset, gsIpcDbg.ui32BufSize);

	// 4. Write Debug String
	memcpy(&pui32WrVirPtr[ui32WrOffset>>2], ui8Buffer, len_byte);
	ui32WrOffset = ADEC_IPC_DBG_INCREASE_OFFSET(ui32WrOffset, len_byte, gsIpcDbg.ui32BufSize);

	// 5. Update Write ADEC_ADEC_IPC Register
	ADEC_IPC_REG_DBG_SetWrOffset(ui32WrOffset);

	ADEC_MCU_HAL_WriteBack(cmdPtr, len_byte + 8);

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
static UINT32 _ADEC_IPC_DBG_VerifyMagicCode(UINT32 *pui32RdVirPtr, UINT32 ui32RdOffset)
{
	if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_DBG_MAGIC_CODE )
	{
		return ADEC_IPC_DBG_NEXT_OFFSET(ui32RdOffset, gsIpcDbg.ui32BufSize);
	}

//	ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][Dbg][DBG] Not Found MAGIC CODE - Read Offset: 0x%X", ui32RdOffset );

	if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_DBG_PADDING_BITS )
	{
//		ADEC_KDRV_Message(NORMAL1, "[ADEC_ADEC_IPC][Dbg][DBG] PADDING BITS - Read Offset: 0x%X", ui32RdOffset );

		ui32RdOffset = 0;
		if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_DBG_MAGIC_CODE )
		{
			return ADEC_IPC_DBG_NEXT_OFFSET(ui32RdOffset, gsIpcDbg.ui32BufSize);
		}
	}

	ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][DBG] No MAGIC CODE: 0x%X - Read Offset: 0x%X", pui32RdVirPtr[ui32RdOffset>>2], ui32RdOffset );

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
static UINT32 _ADEC_IPC_DBG_FindNextMagicCode(UINT32 *pui32RdVirPtr, UINT32 ui32RdOffset, UINT32 ui32WrOffset)
{
	while( ui32RdOffset != ui32WrOffset )
	{
		if( pui32RdVirPtr[ui32RdOffset>>2] == ADEC_IPC_DBG_MAGIC_CODE )
		{
			return ADEC_IPC_DBG_NEXT_OFFSET(ui32RdOffset, gsIpcDbg.ui32BufSize);
		}
		else if( pui32RdVirPtr[ui32RdOffset>>2] != ADEC_IPC_DBG_PADDING_BITS )
		{
	ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][DBG] MAGIC CODE: 0x%X - Read Offset: 0x%X", pui32RdVirPtr[ui32RdOffset>>2], ui32RdOffset );
		}

		ui32RdOffset = ADEC_IPC_DBG_NEXT_OFFSET(ui32RdOffset, gsIpcDbg.ui32BufSize);
	}

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
UINT32 _ADEC_IPC_DBG_Receive(void)
{
	UINT32		ui32WrOffset;	// byte size
	UINT32		ui32RdOffset;	// byte size
	UINT32		*pui32RdVirPtr;
	UINT32		ui32DbgBodySize;

	ui32WrOffset = ADEC_IPC_REG_DBG_GetWrOffset();
	ui32RdOffset = ADEC_IPC_REG_DBG_GetRdOffset();

	switch( ADEC_IPC_REG_DBG_GetBaseAddr() )
	{
	case ADEC_IPC_DBG_ERROR_CODE_01 :
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err]DBG] Too Big DBG Message Size: %d", ADEC_IPC_REG_DBG_GetBufSize() );
		goto ADEC_IPC_DBG_ERROR_Recovery;
		break;
	case ADEC_IPC_DBG_ERROR_CODE_02 :
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][DBG] Overflow - Write Offset:0x%X", ADEC_IPC_REG_DBG_GetBufSize() );
		goto ADEC_IPC_DBG_ERROR_Recovery;
		break;
	case ADEC_IPC_DBG_ERROR_CODE_03 :
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][CMD] Not Initialised" );
		goto ADEC_IPC_DBG_ERROR_Recovery;
		break;
ADEC_IPC_DBG_ERROR_Recovery :
		ADEC_IPC_REG_DBG_SetBaseAddr(gsIpcDbg.ui32PhyBaseAddr);
		ADEC_IPC_REG_DBG_SetBufSize(gsIpcDbg.ui32BufSize);
	default :
		break;
	}

	if( !gsIpcDbg.pui32VirBasePtr )
	{
		//ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][CMD] Not Initialised" );
		return 0;
	}

	if( ui32RdOffset == ui32WrOffset )
		return 0;

	pui32RdVirPtr = gsIpcDbg.pui32VirBasePtr;

	// 1. Check Magic Code
	ui32RdOffset = _ADEC_IPC_DBG_VerifyMagicCode(pui32RdVirPtr, ui32RdOffset);
	if( ui32RdOffset == ADEC_IPC_REG_INVALID_OFFSET )
	{
		ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][DBG] Find Next MAGIC Code" );

		ui32RdOffset = _ADEC_IPC_DBG_FindNextMagicCode(pui32RdVirPtr, ADEC_IPC_REG_DBG_GetRdOffset(), ui32WrOffset);
		if( ui32RdOffset == ADEC_IPC_REG_INVALID_OFFSET )
		{
			ADEC_KDRV_Message(ERROR, "[ADEC_ADEC_IPC][Err][DBG] No MAGIC Code" );
			return 0;
		}
	}

	// 2. Read Length Field
	ui32DbgBodySize = pui32RdVirPtr[ui32RdOffset>>2];
	ui32RdOffset = ADEC_IPC_DBG_NEXT_OFFSET(ui32RdOffset, gsIpcDbg.ui32BufSize);

	// 3. Print to host debug port
	ADEC_IPC_MSG("%s", (char *)&pui32RdVirPtr[ui32RdOffset>>2] );
	ui32RdOffset = ADEC_IPC_DBG_INCREASE_OFFSET(ui32RdOffset, ui32DbgBodySize, gsIpcDbg.ui32BufSize);

	// 4. Update Read ADEC_ADEC_IPC Register
	ADEC_IPC_REG_DBG_SetRdOffset(ui32RdOffset);

	return 1;
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
void _ADEC_IPC_DBG_Receive_workfunc(struct work_struct *data)
{
	spin_lock(&stAdecIpcDbgSpinlock);

	while( _ADEC_IPC_DBG_Receive() );

	spin_unlock(&stAdecIpcDbgSpinlock);
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
void ADEC_IPC_DBG_Receive(void)
{
	queue_work(_ADEC_IPC_DBG_workqueue, &_ADEC_IPC_DBG_work);
}
#endif
/** @} */

