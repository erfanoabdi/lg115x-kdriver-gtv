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
 *  interrupt service routine implementation for venc device.
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
#include <linux/interrupt.h>
#include <linux/sched.h>

#include "venc_cfg.h"
#include "venc_drv.h"
#include "venc_reg_api.h"
#include "venc_core.h"
#include "venc_isr.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum {
	VENC_IPC_ISR_CFG			= 1 <<  0,
	VENC_IPC_ISR_IME			= 1 <<  1,
	VENC_IPC_ISR_FME_HPEL		= 1 <<  2,
	VENC_IPC_ISR_FME_QPEL		= 1 <<  3,
	VENC_IPC_ISR_VENC			= 1 <<  4,
	VENC_IPC_ISR_VLC			= 1 <<  5,
	VENC_IPC_ISR_DEBLOCK		= 1 <<  6,
	VENC_IPC_ISR_TOKEN			= 1 <<  7,
	VENC_IPC_ISR_TSO			= 1 <<  8,
	VENC_IPC_ISR_DMA			= 1 <<  9,
	VENC_IPC_ISR_IPC			= 1 << 10,
	VENC_IPC_ISR_VSYNC			= 1 << 11,
	VENC_IPC_ISR_PNGD			= 1 << 12,
} VENC_ISR_IPC_T;

typedef enum {
	VENC_INT_IPC_STATE_SENC_CTRL		= 1 <<  0,
	VENC_INT_IPC_STATE_VENC_RIDX_IBF	= 1 <<  9,
	VENC_INT_IPC_STATE_THUMB_CTRL		= 1 << 12,
	VENC_INT_IPC_STATE_THUMB_SZ_IN		= 1 << 13,
	VENC_INT_IPC_STATE_THUMB_SZ_OUT		= 1 << 14,
	VENC_INT_IPC_STATE_THUMB_BSA_IN		= 1 << 15,
	VENC_INT_IPC_STATE_THUMB_BSA_OUT	= 1 << 16,
	VENC_INT_IPC_STATE_VENC_GAUGE		= 1 << 22,
	VENC_INT_IPC_STATE_EXT_ESWPTR		= 1 << 23,
	VENC_INT_IPC_STATE_EXT_ESRPTR		= 1 << 24,
	VENC_INT_IPC_STATE_VENC_CTRL		= 1 << 25,
	VENC_INT_IPC_STATE_VES_BSA			= 1 << 26,
	VENC_INT_IPC_STATE_VES_EDA			= 1 << 27,
} VENC_INTERNAL_IPC_STATE_T;

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
static void _VENC_ISR_IPC( void );

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/
void VENC_ISR( void )
{
	UINT32	ui32IntrStat;

	/* Read Interrupt status register */
	ui32IntrStat = VENC_REG_GetInterruptStatus();

	/* Service interrupts */
	if ( ui32IntrStat & VENC_IPC_ISR_IPC )
	{
		/* Service IPC routine */
		_VENC_ISR_IPC();
	}

	if ( ui32IntrStat & VENC_IPC_ISR_DMA )
	{
		/* Service dma routine */
	}

	if ( ui32IntrStat & VENC_IPC_ISR_TSO )
	{
		/* Service tso routine */
	}

	if ( ui32IntrStat & VENC_IPC_ISR_TOKEN )
	{
		/* Service token routine */
	}

	/* Acknowledge interrupt */
	VENC_REG_ClearInterrupt( ui32IntrStat );
}

extern UINT32 uiTSBufBaseVirt;

static void _VENC_ISR_IPC( void )
{
	UINT32 ui32State = VENC_REG_IPC_GetInternalIPCState();

//	VENC_PRINT( "Ext. IPC State : 0x%08X\n", ui32State );

	/* As of now the only event is from IPC */
	if ( ui32State & VENC_INT_IPC_STATE_THUMB_BSA_OUT )
	{
		OS_SendEvent( &gstEvent, VENC_EVENT_SCALER );
//		VENC_PRINT( "Internal IPC : THUMB_BSA\n" );
	}

#if 0	// FOR DEBUG
	if ( ui32State & VENC_INT_IPC_STATE_VENC_GAUGE )
	{
		UINT16 ui16Count, ui16Msec;

		VENC_REG_IPC_GetGauge( &ui16Count, &ui16Msec );
		VENC_PRINT( "[PERFORMANCE] #%04d: %d msec\n", ui16Count, ui16Msec );
	}
#endif

	if ( ui32State & VENC_INT_IPC_STATE_EXT_ESWPTR )	//ES Write pointer update
	{
		UINT32 ui32ESWptr, ui32ESRptr;
		UINT32 ui32AUIWptr, ui32AUIRptr;

#ifdef VENC_ENABLE_TS_OUT
		UINT32 ui32TSWptr, ui32TSRptr;
#endif /* VENC_ENABLE_TS_OUT */

		ui32ESWptr = VENC_REG_IPC_GetESWritePointer();
		ui32ESRptr = VENC_REG_IPC_GetESReadPointer();
		ui32AUIWptr = VENC_REG_IPC_GetVideoAUIWritePointer();
		ui32AUIRptr = VENC_REG_IPC_GetVideoAUIReadPointer();

		if ( gui32NumberOfFrames < VENC_BIT_RATE_BUFFER ) { ++gui32NumberOfFrames; }

		gaFrameBytes[gui32BufferIndex] = (ui32ESRptr<ui32ESWptr)?
										(ui32ESWptr-ui32ESRptr) : (gpstVencMemConfig->uiESBufSize-(ui32ESRptr-ui32ESWptr));
		gui32BufferIndex = (gui32BufferIndex+1)%VENC_BIT_RATE_BUFFER;

		// Flash ES buffer when buffer overflow
		if ( (gui32ESRptr == gui32ESWptr) ||
			 ( (gui32ESRptr <= ui32ESWptr) && (ui32ESWptr <= gui32ESWptr) ) )
		{
			gui32ESRptr = ui32ESRptr;
		}
		gui32ESWptr = ui32ESWptr;

		// Flash AUI buffer when buffer overflow
		if ( (gui32AUIRptr == gui32AUIWptr) ||
			( ( gui32AUIRptr <= ui32AUIWptr ) && (ui32AUIWptr <= gui32AUIWptr) ) )
		{
			gui32AUIRptr = ui32AUIRptr;
		}
		gui32AUIWptr = ui32AUIWptr;
		OS_SendEvent( &gstEvent, VENC_EVENT_ES_WPTR );

#ifdef VENC_ENABLE_TS_OUT
		// 2011.08.29
		// VENC_INT_IPC_STATE_EXT_ESWPTR interrupt에서 TS Output도 처리
		// 향후, 별도 IPC Interrupt로 분리 할 것
		ui32TSWptr = VENC_REG_IPC_GetTSWritePointer();
		ui32TSRptr = VENC_REG_IPC_GetTSReadPointer();

		// Check Write pointer
		if ( ui32TSWptr > gpstVencMemConfig->uiTSBufBase && ui32TSWptr < gpstVencMemConfig->uiTSBufBase + gpstVencMemConfig->uiTSBufSize )
		{
			// Flash TS buffer when buffer overflow
			if ( (gui32TSRptr == gui32TSWptr) ||
				 ( (gui32TSRptr <= ui32TSWptr) && (ui32TSWptr <= gui32TSWptr) ) )
			{
				gui32TSRptr = ui32TSRptr;
			}
			gui32TSWptr = ui32TSWptr;

			OS_SendEvent( &gstEvent, VENC_EVENT_TS_WPTR );
		}

#endif /* VENC_ENABLE_TS_OUT */

//		VENC_PRINT( "Internal IPC : ES_WPTR\n" );
	}
}

/** @} */

