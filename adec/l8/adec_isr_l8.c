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
 *  ADEC core driver functions.
 *	adec device will teach you how to make device driver with new platform.
 *
 *  author		Sudhaher (kanthiraj.s@lge.com)
 *  version		1.0
 *  date		2010.02.23
 *  note		Additional information.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/interrupt.h>    /* For isr function */
#include <asm/irq.h>			/* For isr function */
#include <linux/sched.h>    	/* For isr function */
#include <asm/uaccess.h>
#include "adec_drv.h"
#include "adec_coredrv_l8.h"
#include "mixedIP_coredrv_l8.h"
#include "adec_kapi.h"
#include "proc_util.h"
#include "debug_util.h"


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
//#define DSP_INT_STATUS		// temp for display DSP interrupt


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern spinlock_t 			gADEC_DSP_event_lock;
extern UINT32				g_ui32CurEventType;
extern UINT32				g_ui32RecEventType;
extern wait_queue_head_t 	adec_poll_wait_queue;

//For monitor task
extern UINT32					g_suiWaitQForMonitor;
extern wait_queue_head_t 		Monitor_WaitQ;

// ADEC CPBM Index Decoded Done Interrupt Count
extern UINT32 gADECIndexDecDoneCount;

#ifdef ENABLE_INTERVAL_CEHCK
/* interval time info of Interrupt */
extern LX_ADEC_INT_TIME_INFO_T g_ADECDecDoneInfo;
extern LX_ADEC_INT_TIME_INFO_T g_ADECFrameDoneInfo;
#endif


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
irqreturn_t adec_l8_interrupt0 (int irq, void *dev_id, struct pt_regs *regs);
irqreturn_t adec_l8_interrupt1 (int irq, void *dev_id, struct pt_regs *regs);

void ADEC_L8_ISR0 ( void );
void ADEC_L8_ISR1 ( void );


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
#ifdef DSP_INT_STATUS
static char *psDSP_int_status[] = {
	"Reserved",
	"Play mode ready",
	"Bitstream Ready & Start Decoding",
	"Lipsync : Skip",
	"Lipsync : Repeat",
	"Lipsync : Check Lipsync",
	"STC discontinuity Happens",
	"PES buffer Underflow",
	"ES Buffer Underflow",
	"PES/ES Buffer full",
	"PES header missing",
	"ES header missing",
	"PES decoding error",
	"Input port select error",
	"Source codec select error",
	"SAI State error",
	"Underflow wait state end",
	"MP3 buffer request",
	"MP3 end decoding",
	"read register error",
};
#endif


/*========================================================================================
	Implementation Group
========================================================================================*/

#ifdef ENABLE_INTERVAL_CEHCK
static void _adec_refresh_time_info(LX_ADEC_INT_TIME_INFO_T *pADECIntTimeInfo)
{
	UINT32 loop = 0;

	for (loop = 0; loop < MAX_INTERVAL_COUNT; loop++)
	{
		pADECIntTimeInfo->interval[loop] = 0;
	}

	pADECIntTimeInfo->max_interval = 0;
	pADECIntTimeInfo->min_interval = 0xFFFFFFFF;
	pADECIntTimeInfo->preGSTC = 0;
	pADECIntTimeInfo->curGSTC = 0;
	pADECIntTimeInfo->total_time = 0;
	pADECIntTimeInfo->count = 0;

	return;
}

/**
 * Adec update Interrupt interval info.
 *
 * @see
*/
static void _adec_update_time_info(LX_ADEC_INT_TIME_INFO_T *pADECIntTimeInfo)
{
	UINT32 gstc = g_pstADECReg_l8->aud_gstcclreg;

//	pADECIntTimeInfo->total_time -= pADECIntTimeInfo->interval[pADECIntTimeInfo->count];
	pADECIntTimeInfo->interval[pADECIntTimeInfo->count] = gstc - pADECIntTimeInfo->curGSTC;
	pADECIntTimeInfo->preGSTC = pADECIntTimeInfo->curGSTC;
	pADECIntTimeInfo->curGSTC = gstc;
#if 0
	if (pADECIntTimeInfo->max_interval < pADECIntTimeInfo->interval[pADECIntTimeInfo->count])
	{
		pADECIntTimeInfo->max_interval = pADECIntTimeInfo->interval[pADECIntTimeInfo->count];
	}
	else if (pADECIntTimeInfo->min_interval > pADECIntTimeInfo->interval[pADECIntTimeInfo->count])
	{
		pADECIntTimeInfo->min_interval = pADECIntTimeInfo->interval[pADECIntTimeInfo->count];
	}
	pADECIntTimeInfo->total_time += pADECIntTimeInfo->interval[pADECIntTimeInfo->count];
#endif
	pADECIntTimeInfo->count++;

	if (pADECIntTimeInfo->count == MAX_INTERVAL_COUNT)
	{
		pADECIntTimeInfo->count = 0;
	}

	return;
}
#endif

/**
 * Adec Interrupt 0 definition.
 *
 * @see ADEC_ISR0
*/
irqreturn_t adec_l8_interrupt0 (int irq, void *dev_id, struct pt_regs *regs)
{
	ADEC_L8_ISR0();

	return IRQ_HANDLED;
}


/**
 * Adec Interrupt 1 definition.
 *
 * @see ADEC_ISR1
*/
irqreturn_t adec_l8_interrupt1 (int irq, void *dev_id, struct pt_regs *regs)
{
	ADEC_L8_ISR1 ();

	return IRQ_HANDLED;
}


/**
 * Implementation for ISR handler 0.
 *
 * @see adec_interrupt0
*/
void ADEC_L8_ISR0 ( void )
{
	UINT32		ui32AudIntState;
	
	UINT32		ui32RecEventType = 0;	
	ULONG		flags = 0;

#ifdef ENABLE_MS10_DDC_CERTI
	UINT32		ui32SamplingFreq;
#endif

#ifdef DSP_INT_STATUS
	UINT32		ui32AudIntId;

	// Read aud_int0_id registor
	ui32AudIntId = g_pstADECReg_l8->aud_int0_id;
#endif

	//Read interrupt status
	ui32AudIntState = g_pstADECReg_l8->aud_int_state0;

	//Clear interrupt status
	g_pstADECReg_l8->aud_int_state0 = 0x0;

	//Clear CPU interrupt status
	g_pstADECReg_l8->aud_int0_clr = 0x1;
	g_pstADECReg_l8->aud_int0_clr = 0x0;

	//ADEC_ISR_DEBUG("ISR0 [0x%X]\n", ui32AudIntState );
	//ADEC_ERROR("ISR0 [0x%X]\n", ui32AudIntState );

	if(ui32AudIntState & LX_ADEC_EVENT_DSP0_INFO)
	{
		ADEC_ISR_DEBUG("INT0>DSP0 Info[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP0_INFO)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DSP0_INFO;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DSP0_ERROR)
	{
		ADEC_ISR_DEBUG("INT0>DSP0 Error[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP0_ERROR)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DSP0_ERROR;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DECODING_INFO)
	{
		ADEC_ISR_DEBUG("INT0>Decoding Info[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DECODING_INFO)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DECODING_INFO;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DECODING_ERROR)
	{
		ADEC_ISR_DEBUG("INT0>Decoding Error[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DECODING_ERROR)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DECODING_ERROR;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DECODING_START)
	{
		//Clear gADECIndexDecDoneCount
		gADECIndexDecDoneCount = 0;

		ADEC_ISR_DEBUG("INT0>Decoding Start[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DECODING_START)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DECODING_START;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DECODING_STOP)
	{
		ADEC_ISR_DEBUG("INT0>Decoding Stop[0x%X]\n\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DECODING_STOP)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DECODING_STOP;
		}
	}

#if 0
	if(ui32AudIntState & LX_ADEC_EVENT_BUF_UNDERFLOW)
	{
		ADEC_ISR_DEBUG("INT0>Underflow[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_BUF_UNDERFLOW)
		{
			ui32RecEventType |= LX_ADEC_EVENT_BUF_UNDERFLOW;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_BUF_OVERFLOW)
	{
		ADEC_ISR_DEBUG("INT0>Overflow[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_BUF_OVERFLOW)
		{
			ui32RecEventType |= LX_ADEC_EVENT_BUF_OVERFLOW;
		}

#ifdef ENABLE_DSP_MONITOR	//Enable  DSP0 / 1 debug
		//Send event for monitor task
		g_suiWaitQForMonitor = 1;
		wake_up_interruptible_all(&Monitor_WaitQ);
#endif
	}
#endif

	if(ui32AudIntState & LX_ADEC_EVENT_FATAL_ERROR)
	{
		ADEC_ERROR("INT0>Fatal Error[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_FATAL_ERROR)
		{
			ui32RecEventType |= LX_ADEC_EVENT_FATAL_ERROR;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DSP0_ZEROLOOP)
	{
		ADEC_ERROR("INT0>Zero Loop[0x%X][addr = 0x%X]\n", ui32AudIntState, g_pstADECReg_l8->aud_int0_id);
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP0_ZEROLOOP)
		{
			ui32RecEventType |= LX_ADEC_EVENT_BUF_UNDERFLOW;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_FRAME_DEC_DONE)
	{
		//ADEC_ISR_DEBUG("INT0>Frame Decoded[0x%X]\n", ui32AudIntState );
		ADEC_ISR_DEBUG("Frame Decoded > STC [0x%X], PTSM [0x%x], PTSA [0x%x], GSTC [0x%x]\n",
						g_pstADECReg_l8->aud_stc24, g_pstADECReg_l8->aud_ptsm24, g_pstADECReg_l8->aud_ptsa24, g_pstADECReg_l8->aud_gstcclreg);

		if ( g_ui32CurEventType & LX_ADEC_EVENT_FRAME_DEC_DONE)
		{
			ui32RecEventType |= LX_ADEC_EVENT_FRAME_DEC_DONE;
		}

#ifdef ENABLE_INTERVAL_CEHCK
		_adec_update_time_info(&g_ADECFrameDoneInfo);
#endif
	}

	if(ui32AudIntState & LX_ADEC_EVENT_INDEX_DEC_DONE)
	{
		ADEC_ISR_DEBUG(">>> INT0>Index Decoded[0x%X], (count = %3d)\n", ui32AudIntState, gADECIndexDecDoneCount);
#if 0
		ADEC_DEBUG("I > G [%d], M [%d], d [%3d ms]\n",	\
						g_pstADECReg_l8->aud_gstcclreg, g_pstADECReg_l8->aud_ptsm24,	\
						(g_pstADECReg_l8->aud_ptsm24 - g_pstADECReg_l8->aud_gstcclreg)/90);
		ADEC_DEBUG("A [%d](c = %3d)\n", g_pstADECReg_l8->aud_ptsa24, gADECIndexDecDoneCount);
#endif

		if ( g_ui32CurEventType & LX_ADEC_EVENT_INDEX_DEC_DONE)
		{
			ui32RecEventType |= LX_ADEC_EVENT_INDEX_DEC_DONE;
		}

		//Increase gADECIndexDecDoneCount
		gADECIndexDecDoneCount++;

#ifdef ENABLE_INTERVAL_CEHCK
		_adec_update_time_info(&g_ADECDecDoneInfo);
#endif
	}

#ifdef ENABLE_DTS_CERTI
	if(ui32AudIntState & LX_ADEC_EVENT_FRAME_ENC_DONE)
	{
		ADEC_ISR_DEBUG("INT0>Frame Encoded[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_FRAME_ENC_DONE)
		{
			ui32RecEventType |= LX_ADEC_EVENT_FRAME_ENC_DONE;
		}
	}
#endif

#ifdef ENABLE_MS10_DDC_CERTI
	if(ui32AudIntState & LX_ADEC_EVENT_SAMPLE_FREQ_CHG)
	{
		ADEC_ISR_DEBUG("INT0>Sampling Frequency Changed[0x%X]\n", ui32AudIntState );

		if ( g_ui32CurEventType & LX_ADEC_EVENT_SAMPLE_FREQ_CHG)
		{
			ui32RecEventType |= LX_ADEC_EVENT_SAMPLE_FREQ_CHG;
		}

		//Get a sampling frequency from aud_dsp0_state reg.
		AUD_L8_RdFL(aud_deccfg1);
		AUD_L8_Rd01(aud_deccfg1, sfreq, ui32SamplingFreq);

		ADEC_ERROR("INT0>Sampling Frequency = %d\n", ui32SamplingFreq );

		//Sets the CTOP CTRL Clock PLL Registers.
		(void)MIXED_IP_ADEC_L8_SetSamplingFreq (ui32SamplingFreq);
	}
#endif

	if(ui32AudIntState & LX_ADEC_EVENT_CODEC_DOWNLOAD_DONE)
	{
		ADEC_ISR_DEBUG("INT0>DSP0 Ready[0x%X]\n", ui32AudIntState );
		ADEC_SignalDSP0Ready();

#ifdef DSP_INT_STATUS
		gADECPrevIntId0 = 0;
#endif
	}

	//spin lock for protection
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
	g_ui32RecEventType |= ui32RecEventType;
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	//Wake up poll if any int is triggered
	if(g_ui32RecEventType)
		wake_up_interruptible_all(&adec_poll_wait_queue);

#ifdef DSP_INT_STATUS
	if(gADECPrevIntId0 != ui32AudIntId)
	{
		gADECPrevIntId0 = ui32AudIntId;

		if (ui32AudIntId == 0x20)
		{
			ADEC_DEBUG("ISR0>> aud_int0_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[17]);
		}
		else if (ui32AudIntId == 0x21)
		{
			ADEC_DEBUG("ISR0>> aud_int0_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[18]);
		}
		else if (ui32AudIntId == 0x30)
		{
			ADEC_DEBUG("ISR0>> aud_int0_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[19]);
		}
		else if (ui32AudIntId <= 0x10)
		{
			ADEC_DEBUG("ISR0>> aud_int0_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[ui32AudIntId]);
		}
		else
		{
			ADEC_DEBUG("ISR0>> aud_int0_id [0x%X], %s\n", ui32AudIntId, "Not defined!");
		}
	}
#endif

	return;
}


/**
 * Implementation for ISR handler 1
 *
 * @see ADEC_ISR0
*/
void ADEC_L8_ISR1 ( void )
{
	UINT32		ui32AudIntState;

	UINT32		ui32RecEventType = 0;	
	ULONG		flags = 0;

#ifdef DSP_INT_STATUS
	UINT32		ui32AudIntId;

	// Read aud_int1_id registor
	ui32AudIntId = g_pstADECReg_l8->aud_int1_id;
#endif

	//Read  interrupt status
	ui32AudIntState = g_pstADECReg_l8->aud_int_state1;

	//Clear interrupt status
	g_pstADECReg_l8->aud_int_state1 = 0;

	//Clear CPU interrupt status
	g_pstADECReg_l8->aud_int1_clr = 0x1;
	g_pstADECReg_l8->aud_int1_clr = 0x0;

	//ADEC_ISR_DEBUG("ISR1 [0x%X]\n", ui32AudIntState );

	if(ui32AudIntState & LX_ADEC_EVENT_DSP1_INFO)
	{
		//ADEC_ISR_DEBUG("INT1>DSP1 Info[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP1_INFO)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DSP1_INFO;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DSP1_ERROR)
	{
		ADEC_ISR_DEBUG("INT1>DSP1 Error[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP1_ERROR)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DSP1_ERROR;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_PRESENT_START)
	{
		ADEC_ISR_DEBUG("INT1>Present Start[0x%X]\n\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_PRESENT_START)
		{
			ui32RecEventType |= LX_ADEC_EVENT_PRESENT_START;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_PRESENT_STOP)
	{
		ADEC_ISR_DEBUG("INT1>Present Stop[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_PRESENT_STOP)
		{
			ui32RecEventType |= LX_ADEC_EVENT_PRESENT_STOP;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_PRESENT_PAUSE)
	{
		ADEC_ISR_DEBUG("INT1>Present Pause[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_PRESENT_PAUSE)
		{
			ui32RecEventType |= LX_ADEC_EVENT_PRESENT_PAUSE;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_PRESENT_END)
	{
		ADEC_ISR_DEBUG("INT1>Present End[0x%X]\n", ui32AudIntState );
		if ( g_ui32CurEventType & LX_ADEC_EVENT_PRESENT_END)
		{
			ui32RecEventType |= LX_ADEC_EVENT_PRESENT_END;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_LIPSYNC_CHECK)
	{
		if ( g_ui32CurEventType & LX_ADEC_EVENT_LIPSYNC_CHECK)
		{
			ADEC_ERROR("INT1>LIP - STC [0x%X], PTSM [0x%X], PTSA [%3d ms], [CMD12:0x%X](count = %d)\n",
			 		    g_pstADECReg_l8->aud_stc24, g_pstADECReg_l8->aud_ptsm24, (SINT32)g_pstADECReg_l8->aud_ptsa24 / 90, g_pstADECReg_l8->cmd12, gADECIndexDecDoneCount);
			ui32RecEventType |= LX_ADEC_EVENT_LIPSYNC_CHECK;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DSP1_ZEROLOOP)
	{
		ADEC_ERROR("INT1>Zero Loop[0x%X][addr = 0x%X]\n", ui32AudIntState, g_pstADECReg_l8->aud_int1_id);
		if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP1_ZEROLOOP)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DSP1_ZEROLOOP;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_CODEC_DOWNLOAD_DONE)
	{
		ADEC_ISR_DEBUG("INT1>DSP1 Ready[0x%X]\n", ui32AudIntState );
		ADEC_SignalDSP1Ready();

#ifdef DSP_INT_STATUS
		gADECPrevIntId0 = 0;
#endif
	}

	//spin lock for protection
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
	g_ui32RecEventType |= ui32RecEventType;
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	//Wake up poll if any int is triggered
	if(g_ui32RecEventType)
		wake_up_interruptible_all(&adec_poll_wait_queue);

#ifdef DSP_INT_STATUS
	if(gADECPrevIntId1 != ui32AudIntId)
	{
		gADECPrevIntId1 = ui32AudIntId;

		if (ui32AudIntId == 0x20)
		{
			ADEC_ISR_DEBUG("ISR1>> aud_int1_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[17]);
		}
		else if (ui32AudIntId == 0x21)
		{
			ADEC_ISR_DEBUG("ISR1>> aud_int1_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[18]);
		}
		else if (ui32AudIntId == 0x30)
		{
			ADEC_ISR_DEBUG("ISR1>> aud_int1_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[19]);
		}
		else if (ui32AudIntId <= 0x10)
		{
			ADEC_ISR_DEBUG("ISR1>> aud_int1_id [0x%X], %s\n", ui32AudIntId, psDSP_int_status[ui32AudIntId]);
		}
		else
		{
			ADEC_ISR_DEBUG("ISR0>> aud_int0_id [0x%X], %s\n", ui32AudIntId, "Not defined!");
		}
	}
#endif

	return;
}

/** @} */
