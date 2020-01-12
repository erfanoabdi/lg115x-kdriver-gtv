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
 *  author		Jong-Sang Oh(jongsang.oh@lge.com)
 *  version		1.0
 *  date		2011.04.09
 *  note		Additional information.
 *
 *  @addtogroup lg1152_adec
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
#include "proc_util.h"
#include "debug_util.h"

#include "../adec_drv.h"
#include "adec_coredrv_l9.h"
#include "adec_kapi.h"

#include "adec_ipc/adec_ipc_req.h"
#include "adec_ipc/adec_ipc_dbg.h"


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
extern spinlock_t 			gADEC_DSP_event_lock;
extern spinlock_t 			gADEC_DoneNoti_lock;

extern UINT32				g_ui32CurEventType;
extern UINT32				g_ui32RecEventType;
extern LX_ADEC_DEC_MODE_T	g_ui32RecEventMode;

extern wait_queue_head_t 	adec_poll_wait_queue;

//For monitor task
extern LX_ADEC_TASK_MSG_T		g_suiWaitQForMonitor;
extern wait_queue_head_t 		Monitor_WaitQ;


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
extern LX_ADEC_KDRV_T	g_adec_kdrv;

extern BOOLEAN g_bIpcDebugReceive;


/*----------------------------------------------------------------------------------------
	Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
irqreturn_t adec_l9_interrupt0 (int irq, void *dev_id, struct pt_regs *regs);
irqreturn_t adec_l9_interrupt1 (int irq, void *dev_id, struct pt_regs *regs);

void ADEC_L9_ISR0 ( void );
void ADEC_L9_ISR1 ( void );


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static int ADEC_L9_GetIndexDecodedCount(UINT8 decNum);



/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * Adec Interrupt 0 definition.
 *
 * @see ADEC_ISR0
*/
irqreturn_t adec_l9_interrupt0 (int irq, void *dev_id, struct pt_regs *regs)
{
	ADEC_L9_ISR0();

	return IRQ_HANDLED;
}


/**
 * Adec Interrupt 1 definition.
 *
 * @see ADEC_ISR1
*/
irqreturn_t adec_l9_interrupt1 (int irq, void *dev_id, struct pt_regs *regs)
{
	ADEC_L9_ISR1 ();

	return IRQ_HANDLED;
}


/**
 * Implementation for ISR handler 0.
 *
 * @see adec_interrupt0
*/
void ADEC_L9_ISR0 ( void )
{
	UINT32		ui32AudIntState;
	UINT32		ui32RecEventType = 0;

	ULONG		flags 	  = 0;

	//Clear CPU interrupt status
	AUD_Wr(aud_int0_clr, 0x1);
	AUD_WrFL(aud_int0_clr);

	AUD_Wr(aud_int0_clr, 0x0);
	AUD_WrFL(aud_int0_clr);

	//Read interrupt status
	AUD_RdFL(aud_int_state0);
	ui32AudIntState = AUD_Rd(aud_int_state0);

	//Clear interrupt status
	AUD_Wr(aud_int_state0, 0x0);
	AUD_WrFL(aud_int_state0);

	//ADEC_ISR_DEBUG("ISR0 [0x%X]\n", ui32AudIntState );

	//LX_ADEC_EVENT_DEC0_INTERRUPT
	if(ui32AudIntState & LX_ADEC_EVENT_DEC0_INTERRUPT)
	{
		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_DECODING_INFO)
		{
			ADEC_ISR_DEBUG("INT0>DEC0 Info[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_DECODING_INFO)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC0_DECODING_INFO;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_DECODING_ERROR)
		{
			ADEC_ISR_DEBUG("INT0>DEC0 Error[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_DECODING_ERROR)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC0_DECODING_ERROR;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_DECODING_START)
		{
			ADEC_ISR_DEBUG("INT0>DEC0 Start[0x%X]\n", ui32AudIntState );

			//Send a event for monitor task
			g_suiWaitQForMonitor |= LX_ADEC_TASK_SET_SPDIF;
			wake_up_interruptible_all(&Monitor_WaitQ);

			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_DECODING_START)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC0_DECODING_START;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_DECODING_STOP)
		{
			ADEC_ISR_DEBUG("INT0>DEC0 Stop[0x%X]\n\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_DECODING_STOP)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC0_DECODING_STOP;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_BUF_OVERFLOW)
		{
			ADEC_ISR_DEBUG("INT0>DEC0 Overflow[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_BUF_OVERFLOW)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC0_BUF_OVERFLOW;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_BUF_UNDERFLOW)
		{
			ADEC_ISR_DEBUG("INT0>DEC0 Underflow[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_BUF_UNDERFLOW)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC0_BUF_UNDERFLOW;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_FRAME_DEC_DONE)
		{
			AUD_RdFL(aud_gstcclreg);
			ADEC_ISR_DEBUG("INT0>DEC0 Decoded[0x%X]\n", ui32AudIntState );

			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_FRAME_DEC_DONE)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC0_FRAME_DEC_DONE;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC0_INDEX_DEC_DONE)
		{
			ADEC_ISR_DEBUG("INT0>DEC0 Index[0x%X]\n", ui32AudIntState);

			if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
			  &&(g_ui32CurEventType & LX_ADEC_EVENT_DEC0_INDEX_DEC_DONE) )
			{
				//For GCD decoding notification
				(void)ADEC_L9_GetIndexDecodedCount(LX_ADEC_DEC_0);

				ui32RecEventType |= LX_ADEC_EVENT_DEC0_INDEX_DEC_DONE;
			}
		}
	}

	//LX_ADEC_EVENT_DEC1_INTERRUPT
	if(ui32AudIntState & LX_ADEC_EVENT_DEC1_INTERRUPT)
	{
		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_DECODING_INFO)
		{
			ADEC_ISR_DEBUG("INT0>DEC1 Info[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC1_DECODING_INFO)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC1_DECODING_INFO;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_DECODING_ERROR)
		{
			ADEC_ISR_DEBUG("INT0>DEC1 Error[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC1_DECODING_ERROR)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC1_DECODING_ERROR;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_DECODING_START)
		{
			ADEC_ISR_DEBUG("INT0>DEC1 Start[0x%X]\n", ui32AudIntState );

			//Send event for monitor task
			g_suiWaitQForMonitor |= LX_ADEC_TASK_SET_SPDIF_DEC1;
			wake_up_interruptible_all(&Monitor_WaitQ);

			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC1_DECODING_START)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC1_DECODING_START;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_DECODING_STOP)
		{
			ADEC_ISR_DEBUG("INT0>DEC1 Stop[0x%X]\n\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC1_DECODING_STOP)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC1_DECODING_STOP;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_BUF_OVERFLOW)
		{
			ADEC_ISR_DEBUG("INT0>DEC1 Overflow[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC1_BUF_OVERFLOW)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC1_BUF_OVERFLOW;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_BUF_UNDERFLOW)
		{
			ADEC_ISR_DEBUG("INT0>DEC1 Underflow[0x%X]\n", ui32AudIntState );
			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC1_BUF_UNDERFLOW)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC1_BUF_UNDERFLOW;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_FRAME_DEC_DONE)
		{
			AUD_RdFL(aud_gstcclreg);
			ADEC_ISR_DEBUG("INT0>DEC1 Decoded[0x%X]\n", ui32AudIntState );

			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC1_FRAME_DEC_DONE)
			{
				ui32RecEventType |= LX_ADEC_EVENT_DEC1_FRAME_DEC_DONE;
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_DEC1_INDEX_DEC_DONE)
		{
			ADEC_ISR_DEBUG("INT0>DEC1 Index[0x%X]\n", ui32AudIntState);

			if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE)
			  &&(g_ui32CurEventType & LX_ADEC_EVENT_DEC1_INDEX_DEC_DONE) )
			{
				//For GCD decoding notification
				(void)ADEC_L9_GetIndexDecodedCount(LX_ADEC_DEC_1);

				ui32RecEventType |= LX_ADEC_EVENT_DEC1_INDEX_DEC_DONE;
			}
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_AUI_ENC_DONE)
	{
		ADEC_ISR_DEBUG("INT0>Encoding Done[0x%X]\n", ui32AudIntState );
#if 0
		if ( g_ui32CurEventType & LX_ADEC_EVENT_AUI_ENC_DONE)
		{
			ui32RecEventType |= LX_ADEC_EVENT_AUI_ENC_DONE;
		}
#endif
	}

	// debug for dec0, dec1 debug print
	//debug for SAI and TPI interrupt for receiving data packet.
	if ((g_ui32CurEventType & LX_ADEC_EVENT_DSP0_DEBUG) == LX_ADEC_EVENT_DSP0_DEBUG)
	{
		if(	(ui32AudIntState & LX_ADEC_EVENT_DEC0_DEBUG)
		  ||(ui32AudIntState & LX_ADEC_EVENT_DEC1_DEBUG) )
		{
			AUD_RdFL(aud_debug00_0);
			AUD_RdFL(aud_debug01_0);
			AUD_RdFL(aud_debug02_0);
			AUD_RdFL(aud_debug03_0);

			if ( g_ui32CurEventType & LX_ADEC_EVENT_DEC0_DEBUG)
			{
				ADEC_ERROR("DEC0 : [0x%x] : [0x%x] : [0x%x] : [0x%x] => [0x%X]\n", \
							AUD_Rd(aud_debug00_0), AUD_Rd(aud_debug01_0), AUD_Rd(aud_debug02_0), AUD_Rd(aud_debug03_0), ui32AudIntState);
			}
			else
			{
				ADEC_ERROR("DEC1 : [0x%x] : [0x%x] : [0x%x] : [0x%x] => [0x%X]\n", \
							AUD_Rd(aud_debug00_0), AUD_Rd(aud_debug01_0), AUD_Rd(aud_debug02_0), AUD_Rd(aud_debug03_0), ui32AudIntState);
			}
		}

		if(ui32AudIntState & LX_ADEC_EVENT_INPUT_DEBUG)
		{
			AUD_RdFL(aud_sai_data);

			if(ui32AudIntState & LX_ADEC_EVENT_SIF_DATA)
			{
				ADEC_ERROR("INT0>SIF [0x%X] [data = %d]\n", ui32AudIntState, AUD_Rd(aud_sai_data));
			}
			else if(ui32AudIntState & LX_ADEC_EVENT_ADC_DATA)
			{
				ADEC_ERROR("INT0>ADC [0x%X] [data = %d]\n", ui32AudIntState, AUD_Rd(aud_sai_data));
			}
			else if(ui32AudIntState & LX_ADEC_EVENT_HDMI_DATA)
			{
				ADEC_ERROR("INT0>HDMI [0x%X] [data = %d]\n", ui32AudIntState, AUD_Rd(aud_sai_data));
			}
			else if(ui32AudIntState & LX_ADEC_EVENT_TPI0_DATA)
			{
				ADEC_ERROR("INT0>TPI0 [0x%X] [data = %d]\n", ui32AudIntState, AUD_Rd(aud_sai_data));
			}
			else if(ui32AudIntState & LX_ADEC_EVENT_TPI1_DATA)
			{
				ADEC_ERROR("INT0>TPI1 [0x%X] [data = %d]\n", ui32AudIntState, AUD_Rd(aud_sai_data));
			}
			else
			{
				ADEC_ERROR("INT0>XXXX [0x%X] [data = %d]\n", ui32AudIntState, AUD_Rd(aud_sai_data));
			}
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_CODEC_DOWNLOAD_DONE)
	{
		ADEC_ISR_DEBUG("INT0>DSP0 Ready[0x%X]\n", ui32AudIntState );
		ADEC_L9_SignalDSP0Ready();
	}

	//spin lock for protection
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
	g_ui32RecEventType |= ui32RecEventType;
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	//Wake up poll if any int is triggered
	if(g_ui32RecEventType)
		wake_up_interruptible_all(&adec_poll_wait_queue);

	return;
}


/**
 * Implementation for ISR handler 1
 *
 * @see ADEC_L9_ISR0
*/
void ADEC_L9_ISR1 ( void )
{
	UINT32		ui32AudIntState;

	UINT32		ui32RecEventType = 0;
	ULONG		flags = 0;

	LX_ADEC_DEC_MODE_T	ui32RecEventMode = LX_ADEC_DEC_MODE_NONE;

	//Clear CPU interrupt status
	AUD_Wr(aud_int1_clr, 0x1);
	AUD_WrFL(aud_int1_clr);

	AUD_Wr(aud_int1_clr, 0x0);
	AUD_WrFL(aud_int1_clr);

	//Read interrupt status
	AUD_RdFL(aud_int_state1);
	ui32AudIntState = AUD_Rd(aud_int_state1);

	//Clear interrupt status
	AUD_Wr(aud_int_state1, 0x0);
	AUD_WrFL(aud_int_state1);

	//ADEC_ISR_DEBUG("ISR1 [0x%X]\n", ui32AudIntState );

	if(ui32AudIntState & LX_ADEC_EVENT_DSP1_INFO)
	{
		ADEC_ISR_DEBUG("INT1>DSP1 Info[0x%X]\n", ui32AudIntState );
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
			ui32RecEventMode  = LX_ADEC_DECODE_MAIN;
		}
	}

#if 0
	if(ui32AudIntState & LX_ADEC_EVENT_LIPSYNC_CHECK)
	{
		if ( g_ui32CurEventType & LX_ADEC_EVENT_LIPSYNC_CHECK)
		{
			ADEC_ERROR("INT1>LIP - STC [0x%X], PTSM [0x%X], PTSA [%3d ms], [CMD12:0x%X](count = %d)\n",
			 		    gpRealRegAUD->aud_stc24, gpRealRegAUD->aud_ptsm24, (SINT32)gpRealRegAUD->aud_ptsa24 / 90, gpRealRegAUD->cmd12, gADECIndexDecDoneCount);
			ui32RecEventType |= LX_ADEC_EVENT_LIPSYNC_CHECK;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_PRSENT_NOTI)
	{
		if ( g_ui32CurEventType & LX_ADEC_EVENT_PRSENT_NOTI)
		{
			ADEC_ISR_DEBUG("INT1>LIP - STC [0x%X], PTSM [0x%X], PTSA [%3d ms], [CMD12:0x%X](count = %d)\n",
			 		    gpRealRegAUD->aud_stc24, gpRealRegAUD->aud_ptsm24, (SINT32)gpRealRegAUD->aud_ptsa24 / 90, gpRealRegAUD->cmd12, gADECIndexDecDoneCount);
			ui32RecEventType |= LX_ADEC_EVENT_PRSENT_NOTI;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_PRSENT_NOTI_SKIP)
	{
		if ( g_ui32CurEventType & LX_ADEC_EVENT_PRSENT_NOTI_SKIP)
		{
			ADEC_ISR_DEBUG("INT1>LIP - STC [0x%X], PTSM [0x%X], PTSA [%3d ms], [CMD12:0x%X](count = %d)\n",
			 		    gpRealRegAUD->aud_stc24, gpRealRegAUD->aud_ptsm24, (SINT32)gpRealRegAUD->aud_ptsa24 / 90, gpRealRegAUD->cmd12, gADECIndexDecDoneCount);
			ui32RecEventType |= LX_ADEC_EVENT_PRSENT_NOTI_SKIP;
		}
	}
#endif

	if(ui32AudIntState & LX_ADEC_EVENT_DSP1_CLOCK_RESET)
	{
		ADEC_ISR_DEBUG("INT1>Clock Reset[0x%X]\n", ui32AudIntState);

		//Send event for monitor task
		g_suiWaitQForMonitor |= LX_ADEC_TASK_RESET_CLOCK;
		wake_up_interruptible_all(&Monitor_WaitQ);

		if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP1_CLOCK_RESET)
		{
			ui32RecEventType |= LX_ADEC_EVENT_DSP1_CLOCK_RESET;
		}
	}

	if(ui32AudIntState & LX_ADEC_EVENT_DSP1_LIPSYNC_INFO)
	{
		//For debug print : temp
		if ((g_ui32CurEventType & LX_ADEC_EVENT_DSP0_DEBUG) == LX_ADEC_EVENT_DSP0_DEBUG)
		//if (g_ui32CurEventType & LX_ADEC_EVENT_DSP1_LIPSYNC_INFO)	//temp
		{
			AUD_RdFL(aud_debug00);
			AUD_RdFL(aud_debug01);
			AUD_RdFL(aud_debug02);
			AUD_RdFL(aud_debug03);

			ADEC_ERROR("[%8d] : [%8d] : [%5d] : [%5d]\n", \
						AUD_Rd(aud_debug00), AUD_Rd(aud_debug01), (SINT32)AUD_Rd(aud_debug02), AUD_Rd(aud_debug03));
		}
	}

	// for MHEG TP stream lipsync
	if(ui32AudIntState & LX_ADEC_EVENT_USE_GSTC)
	{
		ADEC_ISR_DEBUG("INT1>LX_ADEC_EVENT_USE_GSTC[0x%X]\n", ui32AudIntState );

		//Send event for monitor task
		g_suiWaitQForMonitor |= LX_ADEC_TASK_USE_GSTC;
		wake_up_interruptible_all(&Monitor_WaitQ);
	}

	// for PCM Clock Rate setting
	if(ui32AudIntState & LX_ADEC_EVENT_SET_PCM_RATE)
	{
		ADEC_ISR_DEBUG("INT1>LX_ADEC_EVENT_SET_PCM_RATE[0x%X]\n", ui32AudIntState );

		//Send event for monitor task
		if(g_suiWaitQForMonitor)
		{
			ADEC_ERROR("INT1>g_suiWaitQForMonitor(PCM) = %d[0x%X]\n", g_suiWaitQForMonitor, ui32AudIntState );
		}

		ADEC_ISR_DEBUG("INT1>LX_ADEC_EVENT_SET_PCM_RATE[0x%X]\n", ui32AudIntState );

		g_suiWaitQForMonitor |= LX_ADEC_TASK_SET_PCM_RATE;
		wake_up_interruptible_all(&Monitor_WaitQ);
	}

	// for SPDIF DTO Rate setting
	if(ui32AudIntState & LX_ADEC_EVENT_SET_DTO_RATE)
	{
		ADEC_ISR_DEBUG("INT1>LX_ADEC_EVENT_SET_DTO_RATE[0x%X]\n", ui32AudIntState );

		//Send event for monitor task
		if(g_suiWaitQForMonitor)
		{
			ADEC_ERROR("INT1>g_suiWaitQForMonitor(DTO) = %d[0x%X]\n", g_suiWaitQForMonitor, ui32AudIntState );
		}

			ADEC_ISR_DEBUG("INT1>LX_ADEC_EVENT_SET_DTO_RATE[0x%X]\n", ui32AudIntState );

			g_suiWaitQForMonitor |= LX_ADEC_TASK_SET_DTO_RATE;
			wake_up_interruptible_all(&Monitor_WaitQ);
	}

	if(ui32AudIntState & LX_ADEC_EVENT_CODEC_DOWNLOAD_DONE)
	{
		ADEC_ISR_DEBUG("INT1>DSP1 Ready[0x%X]\n", ui32AudIntState );
		ADEC_L9_SignalDSP1Ready();
	}

	//spin lock for protection
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
	g_ui32RecEventType |= ui32RecEventType;
	g_ui32RecEventMode |= ui32RecEventMode;
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	//Wake up poll if any int is triggered
	if(g_ui32RecEventType)
		wake_up_interruptible_all(&adec_poll_wait_queue);

	//Receive IPC Request
	while(ADEC_IPC_REQ_Receive());

	//Receive Debug Print for IPC command ACK
	if(g_bIpcDebugReceive)
		ADEC_IPC_DBG_Receive();

	return;
}

/**
 * Implementation for Get Index Decoded Count from ADEC register
 *
 * @see ADEC_L9_ISR0
*/
static int ADEC_L9_GetIndexDecodedCount(UINT8 decNum)
{
	ULONG	flagsNoti = 0;

	//For GCD decoding notification
	if(decNum == LX_ADEC_DEC_0)
	{
		//Read aui index0
		AUD_RdFL(aud_pts_index0);
		g_adec_kdrv.index[decNum].NewNOTI = AUD_Rd(aud_pts_index0);
	}
	else
	{
		//Read aui index1
		AUD_RdFL(aud_pts_index1);
		g_adec_kdrv.index[decNum].NewNOTI = AUD_Rd(aud_pts_index1);
	}

	//spin lock for protection : lock
	spin_lock_irqsave(&gADEC_DoneNoti_lock, flagsNoti);

	//  we think g_adec_kdrv.index[decNum].NewNOTI as Wr ptr
	if(g_adec_kdrv.index[decNum].WriteCNT == INITIAL_CONDITION )
	{
		g_adec_kdrv.index[decNum].WriteCNT = g_adec_kdrv.index[decNum].NewNOTI;
		g_adec_kdrv.index[decNum].FreeCNT  = (CPB_AUI_INDEX_COUNT -(g_adec_kdrv.index[decNum].NewNOTI + 1));
		g_adec_kdrv.index[decNum].UsedCNT  = (g_adec_kdrv.index[decNum].NewNOTI + 1);
	}
	else
	{
		if(g_adec_kdrv.index[decNum].FreeCNT == 0)
		{
			g_adec_kdrv.index[decNum].WriteCNT = g_adec_kdrv.index[decNum].NewNOTI;
			g_adec_kdrv.index[decNum].ReadCNT  = (g_adec_kdrv.index[decNum].NewNOTI + 1)%CPB_AUI_INDEX_COUNT;
			g_adec_kdrv.index[decNum].UsedCNT  = CPB_AUI_INDEX_COUNT ;
		}
		else
		{
			// 1021                                                2
			if (g_adec_kdrv.index[decNum].PrevNOTI > g_adec_kdrv.index[decNum].NewNOTI) // warp around case
			{
				g_adec_kdrv.index[decNum].WriteCNT = g_adec_kdrv.index[decNum].NewNOTI;
																 // 1021 , 1022, 1023                                               0 , 1, 2
				g_adec_kdrv.index[decNum].FreeCNT -= ( ( (CPB_AUI_INDEX_COUNT - 1) -g_adec_kdrv.index[decNum].PrevNOTI  ) + ( g_adec_kdrv.index[decNum].NewNOTI+1) );
				g_adec_kdrv.index[decNum].UsedCNT =( g_adec_kdrv.index[decNum].UsedCNT + ( ( (CPB_AUI_INDEX_COUNT - 1) -g_adec_kdrv.index[decNum].PrevNOTI  ) + ( g_adec_kdrv.index[decNum].NewNOTI+1) ) ) % CPB_AUI_INDEX_COUNT ;
			}
			else
			{
				g_adec_kdrv.index[decNum].WriteCNT = g_adec_kdrv.index[decNum].NewNOTI;
				g_adec_kdrv.index[decNum].FreeCNT -= ( g_adec_kdrv.index[decNum].NewNOTI -g_adec_kdrv.index[decNum].PrevNOTI  );
				g_adec_kdrv.index[decNum].UsedCNT =( g_adec_kdrv.index[decNum].UsedCNT + ( g_adec_kdrv.index[decNum].NewNOTI -g_adec_kdrv.index[decNum].PrevNOTI  ) ) % CPB_AUI_INDEX_COUNT ;
			}
		}
	}

	//spin lock for protection : unlock
	spin_unlock_irqrestore(&gADEC_DoneNoti_lock, flagsNoti);

	if(g_adec_kdrv.index[decNum].FreeCNT == 0)
	{
		ADEC_ERROR("DEC%d : AUI OVERFLOW\n", decNum);
	}

	g_adec_kdrv.index[decNum].NotiCNT++;

	ADEC_DEBUG_TMP("DEC%d : {N%3dF%3d,U%d,W%3dR%3d}\n", decNum, g_adec_kdrv.index[decNum].NewNOTI,g_adec_kdrv.index[decNum].FreeCNT,g_adec_kdrv.index[decNum].UsedCNT, g_adec_kdrv.index[decNum].WriteCNT,g_adec_kdrv.index[decNum].ReadCNT);
	ADEC_IPC_DEBUG("DEC%d : %lluN%3d\n", decNum,g_adec_kdrv.index[decNum].AUI[g_adec_kdrv.index[decNum].NewNOTI].streamtime,g_adec_kdrv.index[decNum].NewNOTI );

	if(g_adec_kdrv.index[decNum].NewNOTI != g_adec_kdrv.index[decNum].PrevNOTI)
	{
		g_adec_kdrv.index[decNum].PrevNOTI = g_adec_kdrv.index[decNum].NewNOTI;
	}

	return RET_OK;
}

/** @} */


