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
 *  ADEC device will teach you how to make device driver with new platform.
 *
 *  author	Sudhaher (kanthiraj.s@lge.com), JongSang Oh(jongsang.oh@lge.com)
 *  version	1.0
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
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "os_util.h" 	//for alloc functions
#include "proc_util.h"
#include "debug_util.h"

#include "adec_coredrv_l8.h"
#include "mixedIP_coredrv_l8.h"
#include "adec_se_param_l8.h"

//audio codec for DSP 0 and DSP1
#include "firmware/ms10_ddc_l8.h"	// MS10_DDC audio codec for DSP0
#include "firmware/ms10_ddt_l8.h" 	// MS10_DDT audio codec for DSP0
#include "firmware/ms10_ddt_tp_l8.h"// MS10_DDT TP audio codec for DSP0
#include "firmware/mp23_codec_l8.h"	// MPEG23 audio codec for DSP0
#include "firmware/pcm_codec_l8.h"	// PCM audio codec for DSP0
#include "firmware/wma_codec_l8.h"	// WMA audio codec for DSP0
#include "firmware/dts_codec_l8.h"	// DTS audio codec for DSP0

#include "firmware/se_codec_l8.h" 	// se(sound engine) codec for DSP1

#ifdef ENABLE_DTS_CERTI
#include "firmware/dts_certi_dec_l8.h"	// DTS decoder codec for certification
#include "firmware/dts_certi_enc_l8.h"	// DTS encoder codec for certification
#include "firmware/se_certi_dts_l8.h" 	// se(sound engine) codec for certification
#endif

#ifdef ENABLE_MS10_DDC_CERTI
#include "firmware/ms10_ddc_certi_l8.h"		// MS10 DDC  decoder codec for certification
#include "firmware/se_certi_ms10_ddc_l8.h" 	// se(sound engine) codec for certification
#endif



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

extern wait_queue_head_t 	ADEC_wait_dsp0_ready;
extern int 					ADEC_dsp0_ready_waitq_flag;
extern wait_queue_head_t 	ADEC_wait_dsp1_ready;
extern int 					ADEC_dsp1_ready_waitq_flag;

//for adec monitor task
extern struct task_struct 		*g_pstADECMonitorTask;

// ADEC CPBM Index Decoded Done Interrupt Count
extern UINT32 gADECIndexDecDoneCount;

extern UINT32	g_ui32CurEventType;
extern UINT32	g_ui32RecEventType;
extern wait_queue_head_t 	adec_poll_wait_queue;

//For monitor task
extern UINT32					g_suiWaitQForMonitor;
extern wait_queue_head_t 		Monitor_WaitQ;


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
/* define ADEC reg for L8 */
volatile AUD_REG_L8_T	*g_pstADECReg_l8;
volatile AUD_REG_L8_T	*gpRealRegAUD_l8;
AUD_REG_L8_T			*gpRegAUD_l8;



/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
#ifndef USE_QEMU_SYSTEM
static UINT32	*g_pui32DSP0CodecVirAddr = NULL;
static UINT32	*g_pui32DSP1CodecVirAddr = NULL;
static UINT32	*g_pui32LGSEParamVirAddr = NULL;
#endif /* USE_QEMU_SYSTEM */

/* set for DSP 0/1 state for memory flush */
static UINT32	g_ui32ResetDsp0State = 0;
static UINT32	g_ui32ResetDsp1State = 0;

//semaphore
static OS_SEM_T	g_ADEC_Sema;

//This value is set by ADEC_DRV_InitModule function
static LX_ADEC_INIT_TYPE_T		g_eAdecInitType = LX_ADEC_INIT_TYPE_NONE;

//Current codec type for DSP0
static LX_ADEC_SRC_TYPE_T		g_eCurLoadedCodec = LX_ADEC_SRC_TYPE_NONE;

#ifdef ENABLE_DSP_MONITOR
static UINT32	g_ui32PrevPPBRptr = 0;		//Previous PPB Read pointer
#endif


/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 * Run suspend the adec module.
 * ADEC module shall be suspend itself with pre-defined configuration value.
 * @see ADEC_suspend
*/
int ADEC_L8_RunSuspend(void)
{
	//Temp test code
	memset ( (void*)g_pstADECReg_l8, 0x0, sizeof (AUD_REG_L8_T) );

	ADEC_PRINT("ADEC_RunSuspend : Done!\n");
	return 0;
}

/**
 * Run resume the adec module.
 * ADEC module shall be resumed itself with pre-defined configuration value.
 * @see ADEC_suspend
*/
int ADEC_L8_RunResume(void)
{
	ADEC_DEBUG("[%s] resume start...\n", ADEC_MODULE);

	//Initializes the CTOP CTRL Clock Source Registers.
	(void)MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_DTV );

	//Initializes the CTOP CTRL Clock PLL Registers.
	(void)MIXED_IP_ADEC_L8_SetSamplingFreq ( LX_ADEC_SAMPLING_FREQ_48_KHZ );

	//Reset Low : APB, SRC, ADEC DSP and AAD etc...
	ADEC_ClearReset(ADEC_OPTION_ALL);

	//Initializes the Mixed IP Registers.
	(void)MIXED_IP_ADEC_L8_InitCodecRegisters();

	if (lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{
		/* HPDRV Power-down */
		MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_HPDRV, LX_ADEC_POWER_DOWN);
	}

	/* MIC Power-down */
	MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_MIC, LX_ADEC_POWER_DOWN);

	/* DAC Power-down */
	MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_DOWN);

	//Reset High : APB, SRC and Mixed IP etc...
	(void)ADEC_SetReset(ADEC_OPTION_OTHERS);

	// Initialize SE parameters
	(void)ADEC_InitSEParams();
	ADEC_PRINT("ADEC_L8_InitModule : ADEC_InitSEParams\n");

	// Initialize register
	(void)ADEC_InitReg();
	ADEC_PRINT("ADEC_L8_InitModule : ADEC_InitReg\n");

	// Initialize register for check bit because DSP does not read reg. when CPU access reg.
	(void)ADEC_InitRegForCheckbit();
	ADEC_PRINT("ADEC_L8_InitModule : ADEC_InitRegForCheckbit\n");

	ADEC_LoadCodec( LX_ADEC_SRC_TYPE_AC3 );
	ADEC_PRINT("Init DSP0 Ready! Codec Ver : 0x%X\n", g_pstADECReg_l8->aud_dsp0_state.codec_ver );
	ADEC_PRINT("Init DSP1 Ready! Codec Ver : 0x%X\n", g_pstADECReg_l8->aud_dsp1_state.codec_ver );

	//Start-up the CODEC IP for mute off.
	(void)MIXED_IP_ADEC_L8_StartCodec();

	g_pstADECReg_l8->aud_set_inout.in_port = LX_ADEC_IN_PORT_TP;
	g_pstADECReg_l8->aud_deccfg1.sfreq = 48;
	g_pstADECReg_l8->aud_play_mode.play_mode = 0x1; //start decoding

	ADEC_PRINT("ADEC_RunResume : Done!\n");
	return 0;
}

/**
 * Initializes the adec module.
 * ADEC module shall be initialized itself with pre-defined configuration value.
 * ADEC module includes audio codec, post-processing, SPDIF parts
 * @see ADEC_Init
*/
int ADEC_L8_InitModule ( LX_ADEC_INIT_TYPE_T eInitType )
{
	LX_ADEC_SRC_TYPE_T 	i_eSrcType;

	//Initialize ADEC semaphore
	OS_InitMutex(&g_ADEC_Sema, OS_SEM_ATTR_DEFAULT);

#if 0
	ADEC_DEBUG("0xf6000000(Silkload0) : 0x%X\n", *(volatile UINT32 *) (0xf6000000) );

	ADEC_DEBUG("0xf6000040(Silkload3) : 0x%X\n", *(volatile UINT32 *) (0xf6000040) );
	ADEC_DEBUG("0xf6000044 		      : 0x%X\n", *(volatile UINT32 *) (0xf6000044) );
	ADEC_DEBUG("0xf6000048 		      : 0x%X\n", *(volatile UINT32 *) (0xf6000048) );
	ADEC_DEBUG("0xf600004c 		      : 0x%X\n", *(volatile UINT32 *) (0xf600004c) );


	ADEC_DEBUG("0xf6000060(Silkload4) : 0x%X\n", *(volatile UINT32 *) (0xf6000060) );
	ADEC_DEBUG("0xf6000064 		      : 0x%X\n", *(volatile UINT32 *) (0xf6000064) );
	ADEC_DEBUG("0xf6000068 		      : 0x%X\n", *(volatile UINT32 *) (0xf6000068) );
	ADEC_DEBUG("0xf600006c 		      : 0x%X\n", *(volatile UINT32 *) (0xf600006c) );
#endif

	//Copy ADEC init type
	g_eAdecInitType = eInitType;

	if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC )
	{
		ADEC_DEBUG ("ATSC SELECTED\n");
	}
	else if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
	{
		ADEC_DEBUG ("DVB SELECTED\n");
	}
	else if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_GED )
	{
		ADEC_DEBUG ("GED SELECTED\n");
	}
	else
	{
		ADEC_ERROR ("LX_ADEC_INIT_TYPE_NONE(0x%X)!!!\n", g_eAdecInitType);
	}

#ifndef USE_QEMU_SYSTEM
	//Mapping virtual memory address for dsp0 fw download memory
	g_pui32DSP0CodecVirAddr = (UINT32*) ioremap(pstMemCfgADEC->fw0_memory_base,
												pstMemCfgADEC->fw0_memory_size);
	if (NULL == g_pui32DSP0CodecVirAddr)
	{
		ADEC_ERROR ("ADEC_LoadCodec : DSP0 Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "DSP0 CODEC Download address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->fw0_memory_base);
	ADEC_PRINT ( "DSP0 CODEC Download address-virt[0x%08X]\n", (UINT32) g_pui32DSP0CodecVirAddr);

	//Mapping virtual memory address for dsp1 fw download memory
	g_pui32DSP1CodecVirAddr = (UINT32*) ioremap(pstMemCfgADEC->fw1_memory_base,
												pstMemCfgADEC->fw1_memory_size);
	if (NULL == g_pui32DSP1CodecVirAddr)
	{
		ADEC_ERROR ("ADEC_LoadCodec : DSP1 Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "DSP1 CODEC Download address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->fw1_memory_base);
	ADEC_PRINT ( "DSP1 CODEC Download address-virt[0x%08X]\n", (UINT32) g_pui32DSP1CodecVirAddr);

	//Mapping virtual memory address for LG sound engine parameter memory
	g_pui32LGSEParamVirAddr = (UINT32*) ioremap(ADDR_LGSE_PARAM_BASE, MAX_SIZE_OF_LGSE_PARAM);
	if (NULL == g_pui32LGSEParamVirAddr)
	{
		ADEC_ERROR ("ADEC_LoadCodec : LGSE Param Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "ADEC LGSE address-phy[0x%08X]\n", (UINT32) ADDR_LGSE_PARAM_BASE);
	ADEC_PRINT ( "ADEC LGSE address-virt[0x%08X]\n", (UINT32) g_pui32LGSEParamVirAddr);
#endif /* !USE_QEMU_SYSTEM */

	//Initializes the CTOP CTRL Clock Source Registers.
	(void)MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_DTV );

	//Initializes the CTOP CTRL Clock PLL Registers.
	(void)MIXED_IP_ADEC_L8_SetSamplingFreq ( LX_ADEC_SAMPLING_FREQ_48_KHZ );

	//Clear DSP 0 /1 State
	g_ui32ResetDsp0State = 0;
	g_ui32ResetDsp1State = 0;

	//Reset Low : APB, SRC, ADEC DSP and AAD etc...
	ADEC_ClearReset(ADEC_OPTION_ALL);

	//Initializes the Mixed IP Registers.
	(void)MIXED_IP_ADEC_L8_InitCodecRegisters();

	if (lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{
		/* HPDRV Power-down */
		MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_HPDRV, LX_ADEC_POWER_DOWN);
	}

	/* MIC Power-down */
	MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_MIC, LX_ADEC_POWER_DOWN);

	if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC )
	{
		/* DAC Power-down */
		MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_DOWN);
	}
	else if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
	{
		/* DAC Power-up */
		MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_UP);
	}
	else
	{
		/* DAC Power-up */
		MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_UP);
	}

	//Reset High : APB, SRC and Mixed IP etc...
	(void)ADEC_SetReset(ADEC_OPTION_OTHERS);

	// Initialize SE parameters
	(void)ADEC_InitSEParams();
	ADEC_PRINT("ADEC_L8_InitModule : ADEC_InitSEParams\n");

	// Initialize register
	(void)ADEC_InitReg();
	ADEC_PRINT("ADEC_L8_InitModule : ADEC_InitReg\n");

	// Initialize register for check bit because DSP does not read reg. when CPU access reg.
	(void)ADEC_InitRegForCheckbit();
	ADEC_PRINT("ADEC_L8_InitModule : ADEC_InitRegForCheckbit\n");

	//Set a PCM codec for default
	//Vdec has problem when sdec is reset.
	i_eSrcType = LX_ADEC_SRC_TYPE_AC3;

	//Load codec to local audio firmware memory to copy DSP0, DSP1 running memory
	//Ready for codec start
	ADEC_LoadCodec( i_eSrcType );
	ADEC_PRINT("Init DSP0 Ready! Codec Ver : 0x%X\n", g_pstADECReg_l8->aud_dsp0_state.codec_ver );
	ADEC_PRINT("Init DSP1 Ready! Codec Ver : 0x%X\n", g_pstADECReg_l8->aud_dsp1_state.codec_ver );

	//Start-up the CODEC IP for mute off.
	(void)MIXED_IP_ADEC_L8_StartCodec();

#ifdef ENABLE_ADEC_JTAG
	//Enable Wiggler JTAG for Audio DSP connection
	//D.S SD:0xFD300108 %LONG 0x00000002
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		CTOP_CTRL_B0_RdFL(ctr62);
		CTOP_CTRL_B0_Wr01(ctr62, jtag_sel, 0x2);
		CTOP_CTRL_B0_WrFL(ctr62);
	}
	else
	{
		CTOP_CTRL_A0_RdFL(ctr62);
		CTOP_CTRL_A0_Wr01(ctr62, jtag_sel, 0x2);
		CTOP_CTRL_A0_WrFL(ctr62);
	}
	ADEC_PRINT("ADEC DSP Wiggler JTAG Selected Now!!!\n");
#endif	//#ifdef ENABLE_ADEC_JTAG

	ADEC_PRINT("ADEC_L8_InitModule : Done!\n");
	return RET_OK;
}


/**
 * Sets a ADEC event to be getted.
 * In order to get a device event, use a ADEC_IOR_GET_EVENT ioctl function.\n
 * If once setted event, send event and reset by ADEC_IOW_RESET_EVENT ioctl function.
 * @see ADEC_DRV_GetEvent
*/
int ADEC_L8_SetEvent ( LX_ADEC_EVENT_TYPE_T eEventType )
{
	/* Set event */
	OS_LockMutex(&g_ADEC_Sema);

#ifdef ENABLE_DTS_CERTI
		eEventType |= LX_ADEC_EVENT_FRAME_DEC_DONE;
		eEventType |= LX_ADEC_EVENT_FRAME_ENC_DONE;
		eEventType |= LX_ADEC_EVENT_PRESENT_STOP;
#endif

	g_ui32CurEventType |= eEventType;
	OS_UnlockMutex(&g_ADEC_Sema);

	ADEC_PRINT("ADEC_L8_SetEvent   : 0x%X\n", g_ui32CurEventType);

	return RET_OK;
}

/**
 * Resets a ADEC event to be getted.
 * This function clears a event to be getted.
 * Before clearing a event, a pending event must be notified.
 * @see ADEC_DRV_SetEvent
*/
int ADEC_L8_ResetEvent ( LX_ADEC_EVENT_TYPE_T eEventType )
{
	/* Reset event */
	OS_LockMutex(&g_ADEC_Sema);
	g_ui32CurEventType &= (~eEventType);
	OS_UnlockMutex(&g_ADEC_Sema);

	ADEC_PRINT("ADEC_L8_ResetEvent : 0x%X\n", g_ui32CurEventType);

	return RET_OK;
}

/**
 * Gets a ADEC event to have been setted and arised from device driver.
 * Returns any of the setted events from the ADEC device.\n
 * When the pending events have been read by the DDI module,\n
 * the ADEC device¡¯s internal event bit field shall be cleared.\n
 * If any event does not arises, this function is polled.\n
 * If event has a memory data, use a LX_ADEC_EVENT_INFO_T with location and size info.
 * @see ADEC_DRV_SetEvent
*/
int ADEC_L8_GetEvent ( LX_ADEC_EVENT_INFO_T *pstEventInfo )
{
	ULONG		flags = 0;

	//Copy of current event id
	pstEventInfo->eventID 		= g_ui32RecEventType;
	pstEventInfo->dsp0Info 		= g_pstADECReg_l8->aud_int0_id;
	pstEventInfo->dsp1Info 		= g_pstADECReg_l8->aud_int1_id;
	pstEventInfo->decIndexCount = gADECIndexDecDoneCount;

	//spin lock for protection : lock
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);

	//After event handled, clear g_ui32RecEventType only getted
	g_ui32RecEventType = 0;

	//spin lock for protection : unlock
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	ADEC_PRINT("ADEC_DRV_GetEvent   : 0x%X(0x%X)\n", pstEventInfo->eventID, g_ui32RecEventType);

	return RET_OK;
}


/**
 * Gets a ADEC mmap information to have been setted.
 * Returns a mmap information from the mmap structure(LX_ADEC_MMAP_INFO_T).\n
 * Mmap function is used to copy a big size of kernel memory data.
 * @see
*/
int ADEC_L8_GetMMapInfo ( LX_ADEC_MMAP_INFO_T *pstMMapInfo )
{
	/* copy total mmap memory size */
	pstMMapInfo->totalMmapSize	= pstMemCfgADEC->cpbm_memory_size +
								  pstMemCfgADEC->cpba_memory_size +
								  pstMemCfgADEC->fw1_memory_size  +
								  pstMemCfgADEC->dsp1_memory_size +
								  pstMemCfgADEC->dpbm_memory_size +
								  pstMemCfgADEC->dpba_memory_size +
								  pstMemCfgADEC->mpb0_memory_size +
								  pstMemCfgADEC->mpb1_memory_size +
								  pstMemCfgADEC->mpb2_memory_size +
								  pstMemCfgADEC->mpb3_memory_size;

	/* Copy each mmap memory size */
	pstMMapInfo->cpbmIndexSize	= MAX_SIZE_OF_CPBM_INDEX_TABLE;
	pstMMapInfo->cpbmSize   	= pstMemCfgADEC->cpbm_memory_size - MAX_SIZE_OF_CPBM_INDEX_TABLE;
	pstMMapInfo->cpbaSize   	= pstMemCfgADEC->cpba_memory_size;
	pstMMapInfo->dsp1FwSize   	= pstMemCfgADEC->fw1_memory_size;
	pstMMapInfo->dsp1MemSize   	= pstMemCfgADEC->dsp1_memory_size;
	pstMMapInfo->seParamSize   	= MAX_SIZE_OF_LGSE_PARAM;
	pstMMapInfo->dpbmSize   	= pstMemCfgADEC->dpbm_memory_size - MAX_SIZE_OF_LGSE_PARAM;
	pstMMapInfo->dpbaSize	   	= pstMemCfgADEC->dpba_memory_size;
	pstMMapInfo->mpb0Size	   	= pstMemCfgADEC->mpb0_memory_size;
	pstMMapInfo->mpb1Size	   	= pstMemCfgADEC->mpb1_memory_size;
	pstMMapInfo->mpb2Size	   	= pstMemCfgADEC->mpb2_memory_size;
	pstMMapInfo->mpb3Size	   	= pstMemCfgADEC->mpb3_memory_size;

	ADEC_DEBUG_TMP("GetMMapInfo : totalMmapSize = 0x%8X(%4d KB)\n", pstMMapInfo->totalMmapSize, pstMMapInfo->totalMmapSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : cpbmIndexSize = 0x%8X(%4d KB)\n", pstMMapInfo->cpbmIndexSize, pstMMapInfo->cpbmIndexSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : cpbmSize      = 0x%8X(%4d KB)\n", pstMMapInfo->cpbmSize, pstMMapInfo->cpbmSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : cpbaSize      = 0x%8X(%4d KB)\n", pstMMapInfo->cpbaSize, pstMMapInfo->cpbaSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : dsp1FwSize    = 0x%8X(%4d KB)\n", pstMMapInfo->dsp1FwSize, pstMMapInfo->dsp1FwSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : dsp1MemSize   = 0x%8X(%4d KB)\n", pstMMapInfo->dsp1MemSize, pstMMapInfo->dsp1MemSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : seParamSize   = 0x%8X(%4d KB)\n", pstMMapInfo->seParamSize, pstMMapInfo->seParamSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : dpbmSize      = 0x%8X(%4d KB)\n", pstMMapInfo->dpbmSize, pstMMapInfo->dpbmSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : dpbaSize      = 0x%8X(%4d KB)\n", pstMMapInfo->dpbaSize, pstMMapInfo->dpbaSize/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : mpb0Size      = 0x%8X(%4d KB)\n", pstMMapInfo->mpb0Size, pstMMapInfo->mpb0Size/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : mpb1Size      = 0x%8X(%4d KB)\n", pstMMapInfo->mpb1Size, pstMMapInfo->mpb1Size/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : mpb2Size      = 0x%8X(%4d KB)\n", pstMMapInfo->mpb2Size, pstMMapInfo->mpb2Size/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : mpb3Size      = 0x%8X(%4d KB)\n", pstMMapInfo->mpb3Size, pstMMapInfo->mpb3Size/1024);
	ADEC_DEBUG_TMP("GetMMapInfo : inter_mem_size= 0x%8X(%4d KB)\n", pstMemCfgADEC->inter_memory_size, pstMemCfgADEC->inter_memory_size/1024);

	return RET_OK;
}


/**
 * Gets a current ADEC Buffer Full Information.
 * Returns a CPB information from the LX_ADEC_BUF_INFO_T structure.
 * @see
*/
int ADEC_L8_GetBufferInfo ( LX_ADEC_BUF_INFO_T *pstBufferInfo )
{
	/* Copy CPBM status */
	pstBufferInfo->CPBM.readOffset  = g_pstADECReg_l8->aud_pesm_rptr0;
	pstBufferInfo->CPBM.writeOffset = g_pstADECReg_l8->aud_pesm_wptr0;

	/* Copy CPBA status */
	pstBufferInfo->CPBA.readOffset  = g_pstADECReg_l8->aud_pesa_rptr0;
	pstBufferInfo->CPBA.writeOffset = g_pstADECReg_l8->aud_pesa_wptr0;

	/* Copy DPBM status */
	pstBufferInfo->DPBM.readOffset  = g_pstADECReg_l8->aud_dpbm_rptr;
	pstBufferInfo->DPBM.writeOffset = g_pstADECReg_l8->aud_dpbm_wptr;

	/* Copy DPBA status */
	pstBufferInfo->DPBA.readOffset  = g_pstADECReg_l8->aud_dpba_rptr;
	pstBufferInfo->DPBA.writeOffset = g_pstADECReg_l8->aud_dpba_wptr;

	/* Copy MPB0 status */
	pstBufferInfo->MPB0.readOffset	= g_pstADECReg_l8->aud_dpba_rptr;
	pstBufferInfo->MPB0.writeOffset = g_pstADECReg_l8->aud_dpba_wptr;

#ifdef ENABLE_FROM_MPB1_TO_MPB3
	/* Copy MPB1 status */
	pstBufferInfo->MPB1.readOffset  = g_pstADECReg_l8->aud_dpba_rptr;	//temp
	pstBufferInfo->MPB1.writeOffset = g_pstADECReg_l8->aud_dpba_wptr;

	/* Copy MPB2 status */
	pstBufferInfo->MPB2.readOffset  = g_pstADECReg_l8->aud_dpba_rptr;	//temp
	pstBufferInfo->MPB2.writeOffset = g_pstADECReg_l8->aud_dpba_wptr;

	/* Copy MPB3 status */
	pstBufferInfo->MPB3.readOffset  = g_pstADECReg_l8->aud_dpba_rptr;	//temp
	pstBufferInfo->MPB3.writeOffset = g_pstADECReg_l8->aud_dpba_wptr;
#endif	//#ifdef ENABLE_FROM_MPB1_TO_MPB3

	return RET_OK;
}

/**
 *  Resets a AAD Module using H/W reset signal.
 *  This function resets a AAD module to clear previous setting.
 * @see
*/
int ADEC_L8_ResetAADModule ( void )
{
	//Reset Low : AAD Module only not AAD APB module
	ADEC_ClearReset(ADEC_OPTION_AAD);

	//Reset High : AAD Module only not AAD APB module
	ADEC_SetReset(ADEC_OPTION_AAD);

	ADEC_DEBUG_TMP("ADEC_DRV_ResetAADModule\n");
	return RET_OK;
}


/**
 * Sets an audio device source to adec module.
 * ADEC module sets the input source, the number of ADC port and the input stream type.
 * @see ADEC_Init
*/
int ADEC_L8_SetSource ( LX_ADEC_SET_SOURCE_T	stSetSource )
{
	AUD_SET_INOUT	stSetInOut;
	AUD_DECCFG1		stDecCfg1;
	AUD_MLIP_BOUND0	stMLipBound;
	AUD_ALIP_BOUND0	stALipBound;

	int				eRetVal = RET_OK;

	//Set Power Control for ATSC model
	if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC )
	{
		if(stSetSource.adecSource == LX_ADEC_IN_PORT_SIF)
		{
			/* AAD Power-up */
			MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_AAD, LX_ADEC_POWER_UP);
		}
		else
		{
			/* AAD Power-down */
			MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_AAD, LX_ADEC_POWER_DOWN);
		}
	}
	//Set Power Control for DVB model
	/* AAD is enabled automatically. */
	/* When ADC is power-down, SCART output is also disabled. */
	else if( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
	{
		if(stSetSource.adecSource == LX_ADEC_IN_PORT_TP)
		{
			/* AAD Power-down  */
			MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_AAD, LX_ADEC_POWER_DOWN);
		}
		else
		{
			/* AAD Power-up */
			MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_AAD, LX_ADEC_POWER_UP);
		}
	}
	else
	{
		/* AAD Power-up */
		MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_AAD, LX_ADEC_POWER_UP);

		/* ADC Power-up */
		MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_ADC, LX_ADEC_POWER_UP);
	}

	//Set clock source only L8 A0/1 chip
	if(lx_chip_rev() <= LX_CHIP_REV(L8, A1))
	{
		if(stSetSource.adecSource == LX_ADEC_IN_PORT_HDMI)
		{
			(void)MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_HDMI );
		}
		else if(stSetSource.adecSource == LX_ADEC_IN_PORT_SIF)
		{
			(void)MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_ATV );
		}
		else if(stSetSource.adecSource == LX_ADEC_IN_PORT_ADC)
		{
			(void)MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_ADC );
		}
		else
		{
			(void)MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_DTV );
		}
	}

	//Read decoder cfg1 reg.
	memcpy (&stDecCfg1, (UINT32*) &g_pstADECReg_l8->aud_deccfg1, sizeof (UINT32));

	/* Set for Sampling Frequency for SRC(Sample Rate Converter) module */
	stDecCfg1.sfreq = stSetSource.samplingFreq;

	//Write cfg1 mode reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_deccfg1, &stDecCfg1, sizeof (UINT32));


	//Read Input port reg.
	memcpy (&stSetInOut, (UINT32*) &g_pstADECReg_l8->aud_set_inout, sizeof (UINT32));

	//Read Main Lip Sync. Bound  reg.
	memcpy (&stMLipBound, (UINT32*) &g_pstADECReg_l8->aud_mlip_bound0, sizeof (UINT32));

	//Read Associate Lip Sync. Bound  reg.
	memcpy (&stALipBound, (UINT32*) &g_pstADECReg_l8->aud_alip_bound0, sizeof (UINT32));

	if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
	{
		//Clear AAD bypass for SCART port
		stSetInOut.scart_out_ctrl = 0x0;
	}

	switch (stSetSource.adecSource)
	{
		case LX_ADEC_IN_PORT_NONE:
			stSetInOut.in_port = 0x0;
			break;

		case LX_ADEC_IN_PORT_TP:
			stSetInOut.in_port = 0x1;

			/* Set a Main Lip Sync Bound Control */
			stMLipBound.main_ubound0 = 0x20;	//DSP0 main lipsync upper bound. Unit=ms : 32 ms
			stMLipBound.main_lbound0 = 0x20;	//DSP0 main lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
			stMLipBound.main_offset0 = 0x100;	//DSP0 main lipsync PTS offset. Unit=ms : 256 ms

			/* Set a Associate Lip Sync Bound Control */
			stALipBound.asoc_ubound0 = 0x20;	//DSP0 associate lipsync upper bound. Unit=ms : 32 ms
			stALipBound.asoc_lbound0 = 0x20;	//DSP0 associate lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
			stALipBound.asoc_offset0 = 0x100;	//DSP0 associate lipsync PTS offset. Unit=ms : 256 ms
			break;

		case LX_ADEC_IN_PORT_SPDIF:
			stSetInOut.in_port = 0x3;
			break;

		case LX_ADEC_IN_PORT_SIF:
			stSetInOut.in_port = 0x4;

			//Set I2S mode for I2S input
			stSetInOut.in_format = 0x0; 	//I2S
			break;

		case LX_ADEC_IN_PORT_ADC:
			stSetInOut.in_port = 0x5;

			if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
			{
				//Set AAD bypass for SCART port
				stSetInOut.scart_out_ctrl = 0x1;
			}

			//Set I2S mode for I2S input
			stSetInOut.in_format = 0x0; 	//I2S
			break;

		case LX_ADEC_IN_PORT_HDMI:
			stSetInOut.in_port = 0x6;

			/* Control a HDMI I2S input mode setting  */
			if (lx_chip_rev() < LX_CHIP_REV(L8, B0))
				stSetInOut.in_format = 0x1;		//:LJ
			break;

		case LX_ADEC_IN_PORT_I2S:
			stSetInOut.in_port = 0x7;

			//Set I2S mode for I2S input
			stSetInOut.in_format = 0x0; 	//I2S
			break;

		case LX_ADEC_IN_PORT_SYSTEM:
			stSetInOut.in_port = 0x8;

			/* Set a Main Lip Sync Bound Control */
			stMLipBound.main_ubound0 = 0x20;	//DSP0 main lipsync upper bound. Unit=ms : 32 ms
			stMLipBound.main_lbound0 = 0x20;	//DSP0 main lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
			stMLipBound.main_offset0 = 0x12C;	//DSP0 main lipsync PTS offset. Unit=ms : 300 ms

			/* Set a Associate Lip Sync Bound Control */
			stALipBound.asoc_ubound0 = 0x20;	//DSP0 associate lipsync upper bound. Unit=ms : 32 ms
			stALipBound.asoc_lbound0 = 0x20;	//DSP0 associate lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
			stALipBound.asoc_offset0 = 0x12C;	//DSP0 associate lipsync PTS offset. Unit=ms : 300 ms

			if (stSetSource.adecType == LX_ADEC_SRC_TYPE_PCM || stSetSource.adecType == LX_ADEC_SRC_TYPE_PCMWAV)
			{
				stMLipBound.main_ubound0 = 0x20;	//DSP0 main lipsync upper bound. Unit=ms : 32 ms
				stMLipBound.main_lbound0 = 0x20;	//DSP0 main lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
				stMLipBound.main_offset0 = 0x0;		//DSP0 main lipsync PTS offset. Unit=ms : 0 ms

				/* Set a Associate Lip Sync Bound Control */
				stALipBound.asoc_ubound0 = 0x20;	//DSP0 associate lipsync upper bound. Unit=ms : 32 ms
				stALipBound.asoc_lbound0 = 0x20;	//DSP0 associate lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
				stALipBound.asoc_offset0 = 0x0;		//DSP0 associate lipsync PTS offset. Unit=ms : 0 ms
			}

			/* Clear a PESM / CPBM buffer */
			g_pstADECReg_l8->aud_pesm_wptr0 = 0x0;
			g_pstADECReg_l8->aud_pesm_rptr0 = 0x0;

			/* Clear a PESA / CPBA buffer */
			g_pstADECReg_l8->aud_pesa_wptr0 = 0x0;
			g_pstADECReg_l8->aud_pesa_rptr0 = 0x0;
			break;

		case LX_ADEC_IN_PORT_MIC:
			stSetInOut.in_port = 0x9;

			//Set I2S mode for I2S input
			stSetInOut.in_format = 0x0; 	//I2S
			break;

		default:
			ADEC_ERROR("ADEC_DRV_SetSource : Invalid Params!\n");
			eRetVal = RET_INVALID_PARAMS;
			return eRetVal;
	}

	//Write Input port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_inout, &stSetInOut, sizeof (UINT32));

	//Write Main Lip Sync. Bound reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_mlip_bound0, &stMLipBound, sizeof (UINT32));

	//Write Associate Lip Sync. Bound reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_alip_bound0, &stALipBound, sizeof (UINT32));

	//Set codec source by adec type after clock is changed.
	eRetVal = ADEC_LoadCodec( stSetSource.adecType );
	if ( RET_OK != eRetVal )
	{
		ADEC_ERROR("ADEC_DRV_SetSource : Error!\n");
	}

	//Set ADC Input port for Mixed IP Module
	if(stSetSource.adecSource == LX_ADEC_IN_PORT_ADC)
	{
		eRetVal = MIXED_IP_ADEC_L8_SetADCPortNumber( stSetSource.portNum );
		if ( RET_OK != eRetVal )
		{
			ADEC_ERROR("MIXED_IP_ADEC_L8_SetADCPortNumber : Error!\n");
		}
	}

	ADEC_PRINT("ADEC_DRV_SetSource : src = %d, port = %d, type = %d, freq = %d(0x%X)\n", stSetSource.adecSource, stSetSource.portNum, stSetSource.adecType, stSetSource.samplingFreq, *(UINT32*)&g_pstADECReg_l8->aud_set_inout);
	ADEC_DEBUG_TMP("ADEC_DRV_SetSource : src = %d, port = %d, type = %d, freq = %d(0x%X)\n", stSetSource.adecSource, stSetSource.portNum, stSetSource.adecType, stSetSource.samplingFreq, *(UINT32*)&g_pstADECReg_l8->aud_set_inout);

	return eRetVal;
}


/**
 * Starts the decoding of the DTV Stream.
 * The source types defined in enum LX_ADEC_SRC_TYPE_T must be reset.
 * @see ADEC_DRV_StopDecoding
*/
int ADEC_L8_StartDecoding ( LX_ADEC_SRC_TYPE_T eSrcType )
{
	AUD_PLAY_MODE	stPlayMode;
	int				eRetVal = RET_OK;

#if 0
	//Reset Low : SPDIF Module
	ADEC_ClearReset(ADEC_OPTION_SPDIF);

	//Reset High : SPDIF Module
	ADEC_SetReset(ADEC_OPTION_SPDIF);

	ADEC_ERROR("ADEC_DRV_StartDecoding : ADEC_SetReset(ADEC_OPTION_SPDIF)\n");
#endif

	//Set codec source by adec type
	eRetVal = ADEC_LoadCodec(eSrcType);

	if (eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_DRV_StartDecoding : Error!  eSrcType = 0x%x\n", eSrcType);
		return RET_ERROR;
	}

	//Read play mode reg.
	memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

	//Start decoding
	stPlayMode.play_mode = 0x1; //Play

	//Write play mode reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_StartDecoding : Started\n");
	ADEC_DEBUG("StartDecoding\n");
	return eRetVal;
}


/**
 * Stops the decoding of the DTV Stream.
 * This function also mutes audio output.
 * @see ADEC_DRV_StartDecoding
*/
int ADEC_L8_StopDecoding ( void )
{
	AUD_PLAY_MODE	stPlayMode;
	AUD_DSP0_STATE	stDsp0State;

	//Read play mode & stop flag reg.
	memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));
	memcpy (&stDsp0State, (UINT32*) &g_pstADECReg_l8->aud_dsp0_state, sizeof (UINT32));

	//Stop decoding
	stPlayMode.play_mode  = LX_ADEC_PLAY_STOP; //Stop
	stDsp0State.stop_flag = 0x1; //Stop flag

	//Write play mode & stop flag reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_dsp0_state, &stDsp0State, sizeof (UINT32));

	/* Clear a PESM / CPBM buffer */
	g_pstADECReg_l8->aud_pesm_wptr0 = 0;
	g_pstADECReg_l8->aud_pesm_rptr0 = 0;

	/* Clear a DPBM buffer */
	g_pstADECReg_l8->aud_dpbm_wptr = 0;
	g_pstADECReg_l8->aud_dpbm_rptr = 0;

	ADEC_PRINT("ADEC_DRV_StopDecoding : Stopped\n");
	ADEC_DEBUG("StopDecoding\n");
	return RET_OK;
}

/**
 * Gets a current ADEC decoding information.
 * Returns a decoding information from the LX_ADEC_DEC_INFO_T structure.
 * @see
*/
int ADEC_L8_GetDecodingInfo ( LX_ADEC_DEC_INFO_T *pstDecodingInfo )
{
	AUD_FRM_CNT0	stFrameCount;

	/* Get a codec source type */
	if ( RET_OK != ADEC_GetCodecSrcType(&(pstDecodingInfo->adecType)))
	{
		return RET_ERROR;
	}

	/* Get a codec type if defined Value */
	pstDecodingInfo->adecType = g_pstADECReg_l8->aud_dsp0_state.codec_type;	//temp : need to chnage kapi value

	/* Get a frame count info. */
	memcpy (&stFrameCount, (UINT32*) &g_pstADECReg_l8->aud_frm_cnt0, sizeof (UINT32));
	pstDecodingInfo->frameCount = stFrameCount.main_frm;	//temp for update to use UINT32

	return RET_OK;
}


/**
 * Checks whether audio ES(Elementary Stream) exists or not.
 * This function is used for checking the existence of the current ES.
 * @see ADEC_DRV_GetESInfo
*/
int ADEC_L8_GetESExistence ( BOOLEAN *pbESExistFlag )
{
	int		iRetVal = RET_OK;

	//In AC-3 mode, ES header is exist...(not implemented)
	if (g_pstADECReg_l8->aud_decstat0.es_existence == 1)
	{
		*pbESExistFlag = TRUE;
	}
	else
	{
		*pbESExistFlag = FALSE;
	}

	ADEC_PRINT("ADEC_DRV_GetESExistence : %d\n", *pbESExistFlag);
	return iRetVal;
}

/**
 * Gets the audio ES(Elementatry Stream) information such as HEAAC, MPEG and AC3.
 * ES info may be added other information further.
 * @see ADEC_DRV_GetESExistence
*/
int ADEC_L8_GetESInfo ( LX_ADEC_ES_INFO_T *pstESInfo )
{
	LX_ADEC_SRC_TYPE_T 	eSrcType	= LX_ADEC_SRC_TYPE_NONE;
	AUD_DECSTAT0		stDecStat0	= { 0 };

	//Implement only AC-3 mode...
	memcpy (&stDecStat0, (UINT32*)&g_pstADECReg_l8->aud_decstat0, sizeof (UINT32));

	if ( RET_OK == ADEC_GetCodecSrcType(&eSrcType))
	{
		pstESInfo->adecFormat = eSrcType;
	}
	else
	{
		return RET_ERROR;
	}

	//o_stESInfo.audioMode = //will be added by myeonhoon lee

	if (eSrcType == LX_ADEC_SRC_TYPE_MS10_DDT)
	{
		pstESInfo->heAAcEsInfo.version 			  = stDecStat0.es_info0; //HE-AAC Version, 0:AAC, 1:HE-AAC V1, 2:HE-AAC V2
		pstESInfo->heAAcEsInfo.transmissionformat = stDecStat0.es_info1; //HE-AAC transmission format, 0:LOAS/LATM, 1:ADTS
		pstESInfo->heAAcEsInfo.channelNum 		  = stDecStat0.es_info2; //HE-AAC Channel number
		ADEC_PRINT("ADEC_DRV_GetESInfo : HEAAC ES Info set!\n");
	}
	else if (eSrcType == LX_ADEC_SRC_TYPE_MPEG || eSrcType == LX_ADEC_SRC_TYPE_MP3)
	{
		pstESInfo->mpegESInfo.bitRate 		= stDecStat0.es_info0; //MPEG Bitrate
		pstESInfo->mpegESInfo.sampleRate 	= stDecStat0.es_info1; //MPEG Samplerate
		pstESInfo->mpegESInfo.layer 		= stDecStat0.es_info2; //MPEG Layer
		pstESInfo->mpegESInfo.channelNum 	= stDecStat0.es_info3; //MPEG Channel Number
		ADEC_PRINT("ADEC_DRV_GetESInfo : MPEG ES Info set!\n");
	}
	else if (eSrcType == LX_ADEC_SRC_TYPE_MS10_DDC)
	{
		pstESInfo->ac3ESInfo.bitRate 		= stDecStat0.es_info0; //AC-3 Bitrate
		pstESInfo->ac3ESInfo.sampleRate 	= stDecStat0.es_info1; //AC-3 Samplerate
		pstESInfo->ac3ESInfo.channelNum 	= stDecStat0.es_info2; //AC-3 Channel Number
		pstESInfo->ac3ESInfo.EAC3 			= stDecStat0.es_info3; //EAC3 Flag, 0:AC-3, 1:E-AC3(DD Plus)
		ADEC_PRINT("ADEC_DRV_GetESInfo : AC3 ES Info set!\n");
	}
	else
	{
		ADEC_ERROR("ADEC_DRV_GetESInfo : Not any of MS10_DDT, MPEG or MS10_DDC(type = 0x%X)\n", eSrcType);
	}

	return RET_OK;
}


/**
 * Starts the audio description decoding.
 * Audio description sound should be mixed with the main audio sound.
 * @see ADEC_DRV_StopADDecoding
*/
int ADEC_L8_StartADDecoding ( LX_ADEC_SRC_TYPE_T eSrcType )
{
	AUD_PLAY_MODE	stPlayMode;
	AUD_SET_INOUT 	stSetInOut;

	//Read Input port reg.
	memcpy (&stSetInOut, (UINT32*) &g_pstADECReg_l8->aud_set_inout, sizeof (UINT32));

	//Set TP Main & Associate Input
	stSetInOut.in_port = 0x2;

	//Write Input port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_inout, &stSetInOut, sizeof (UINT32));

	//Read play mode reg.
	memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

	//Start decoding AD decoding
	stPlayMode.ad_play_mode = 1;

	//Write play mode reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_StartADDecoding : Started...\n");

	return RET_OK;
}

/**
 * Stops the audio description decoding.
 * This function also mutes audio output.
 * @see ADEC_DRV_StartADDecoding
*/
int ADEC_L8_StopADDecoding ( void )
{
	AUD_PLAY_MODE	stPlayMode;
	AUD_DSP0_STATE	stDsp0State;

	//Read play mode & aud_ver0 reg.
	memcpy ( &stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32) );
	memcpy ( &stDsp0State, (UINT32*) &g_pstADECReg_l8->aud_dsp0_state, sizeof (UINT32) );

	//Stop decoding
	stPlayMode.ad_play_mode = 0x0; 	//Stop
	stDsp0State.stop_flag	= 0x1;	//Stop flag

	//Write play mode & aud_ver0 reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_dsp0_state, &stDsp0State, sizeof (UINT32));

	/* Clear a PESA / CPBA buffer */
	g_pstADECReg_l8->aud_pesa_wptr0 = 0;
	g_pstADECReg_l8->aud_pesa_rptr0 = 0;

	ADEC_PRINT("ADEC_DRV_StopADDecoding : Stopped...\n");
	return RET_OK;
}


/**
 * Sets a speaker output mode(L+R,L+L,R+R).
 * Controls the speaker output mode by DDI function.
 * @see ADEC_DRV_SetDACOutVolume
*/
int ADEC_L8_SetSPKOutMode ( LX_ADEC_SPK_OUTPUT_MODE_T eSPKOutMode )
{
	AUD_SET_INOUT	stSetInout;
	int 			eRetVal = RET_OK;

	//Read Input port reg.
	memcpy (&stSetInout, (UINT32*) &g_pstADECReg_l8->aud_set_inout, sizeof (UINT32));

	switch (eSPKOutMode)
	{
		case LX_ADEC_SPK_MODE_LR:
			stSetInout.out_mode = 0x0;
			break;

		case LX_ADEC_SPK_MODE_LL:
			stSetInout.out_mode = 0x1;
			break;

		case LX_ADEC_SPK_MODE_RR:
			stSetInout.out_mode = 0x2;
			break;

		case LX_ADEC_SPK_MODE_MIX:
			stSetInout.out_mode = 0x3;
			break;

		default:
			return RET_INVALID_PARAMS;
	}

	//Write Input port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_inout, &stSetInout, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetSPKOutMode %d : OK\n", eSPKOutMode );
	return eRetVal;
}

/**
 * Sets an audio volume.
 * Range of input parameter value is 0 to 100.
 * @see ADEC_DRV_SetADVolume
*/
int ADEC_L8_SetVolume ( UINT32 ui32Volume )
{
	AUD_SET_VOLUME 	stSetVolume;

	//Read setVolume port reg.
	memcpy (&stSetVolume, (UINT32*) &g_pstADECReg_l8->aud_set_volume, sizeof (UINT32));

	//Set D-AMP volume
	stSetVolume.damp_volume      = ui32Volume;
	stSetVolume.damp_vol_ctrl_en = 1;

	//Write setVolume port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_volume, &stSetVolume, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetVolume %d : OK\n", ui32Volume );

	return RET_OK;
}

/**
 * Sets an audio description volume before mixing main audio.
 * This function is used when only ADEC_IOW_START_AD_DECODING is called.
 * @see ADEC_DRV_SetVolume
*/
int ADEC_L8_SetADVolume ( UINT32 ui32Volume )
{
	AUD_SET_VOLUME 	stSetVolume;

	//Read setVolume port reg.
	memcpy (&stSetVolume, (UINT32*) &g_pstADECReg_l8->aud_set_volume, sizeof (UINT32));

	//Set AD volume
	stSetVolume.ad_volume 	   = ui32Volume;
	stSetVolume.ad_vol_ctrl_en = 1;

	//Write setVolume port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_volume, &stSetVolume, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetADVolume %d : OK\n", ui32Volume );

	return RET_OK;
}


/**
 * Sets the volume of DAC output.
 * Range of input parameter value is 0 to 0xFFFF.
 * @see ADEC_DRV_SetVolume
*/
int ADEC_L8_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol )
{
	AUD_SET_DS_VOL 	stSetDSVolume;
	AUD_SET_SH_VOL	stSetSHVolume;

#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV
	AUD_SET_W_VOL	stSetWooferVol;
#endif

	//Sanity check
	if( (stDACOutVol.portNum & LX_ADEC_OUTPUT_ALL) == 0 )
	{
		ADEC_ERROR("MIXED_IP_ADEC_L8_SetDACOutVolume : Port Error!(%d)\n", stDACOutVol.portNum);
		return RET_ERROR;
	}

	//DSP volume control
	if(stDACOutVol.mode & LX_ADEC_DAC_DSP)
	{
		//Set a SCART DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			//Read setSHVolume port reg.
			memcpy (&stSetSHVolume, (UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, sizeof (UINT32));

			stSetSHVolume.scart_volume      = stDACOutVol.swVolume;
			stSetSHVolume.scart_vol_ctrl_en = 1;

			//Write setSHVolume port reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, &stSetSHVolume, sizeof (UINT32));
		}

		//Set a Line-out DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC1 )
		{
			//Read setDSVolume port reg.
			memcpy (&stSetDSVolume, (UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, sizeof (UINT32));

			stSetDSVolume.dac_volume      = stDACOutVol.swVolume;
			stSetDSVolume.dac_vol_ctrl_en = 1;

			//Write setDSVolume port reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, &stSetDSVolume, sizeof (UINT32));
		}

		//Set a HeadPhone DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			//Read setSHVolume port reg.
			memcpy (&stSetSHVolume, (UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, sizeof (UINT32));

			stSetSHVolume.hp_volume      = stDACOutVol.swVolume;
			stSetSHVolume.hp_vol_ctrl_en = 1;

			//Write setSHVolume port reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, &stSetSHVolume, sizeof (UINT32));
		}

#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV
		//Set a HeadPhone Gain volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_I2S3 )
		{
			//Read SetWooferVol reg.
			memcpy (&stSetWooferVol, (UINT32*) &g_pstADECReg_l8->aud_set_w_vol, sizeof (UINT32));

			//Set woofer volume
			if(stDACOutVol.swVolume == 255)
			{
				//Set Min value to 0x3FF when stDACOutVol.volume is 255.
				stSetWooferVol.hp_w_volume = 0x3FF;
			}
			else
			{
				stSetWooferVol.hp_w_volume = stDACOutVol.swVolume;
			}
			stSetWooferVol.hp_w_vol_ctrl_en = 1;

			//Write SetWooferVol reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_w_vol, &stSetWooferVol, sizeof (UINT32));

			ADEC_DEBUG("ADEC_DRV_SetDACOutVolume : HP Gain volume = %d(0x%X)\n", stDACOutVol.swVolume, *(UINT32*)&g_pstADECReg_l8->aud_set_w_vol );
		}
#endif	//#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV

		ADEC_PRINT("ADEC_DRV_SetDACOutVolume port = %d, swVolume = %d : OK\n", stDACOutVol.portNum, stDACOutVol.swVolume );
		//ADEC_DEBUG_TMP("ADEC_DRV_SetDACOutVolume port = %d, swVolume = %d : OK\n", stDACOutVol.portNum, stDACOutVol.swVolume );
	}

	//MixedIP volume control
	if(stDACOutVol.mode & LX_ADEC_DAC_MIXEDIP)
	{
#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV
		//Set a HeadPhone DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			//Read SetWooferVol reg.
			memcpy (&stSetWooferVol, (UINT32*) &g_pstADECReg_l8->aud_set_w_vol, sizeof (UINT32));

			//Set woofer volume
			if(stDACOutVol.swVolume == 255)
			{
				//Set Min value to 0x3FF when stDACOutVol.volume is 255.
				stSetWooferVol.w_volume = 0x3FF;
			}
			else
			{
				stSetWooferVol.w_volume = stDACOutVol.swVolume;
			}
			stSetWooferVol.w_vol_ctrl_en = 1;

			//Write SetWooferVol reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_w_vol, &stSetWooferVol, sizeof (UINT32));

			ADEC_PRINT("ADEC_DRV_SetDACOutVolume : Woofer volume = %d(0x%X)\n", stDACOutVol.swVolume, *(UINT32*)&g_pstADECReg_l8->aud_set_w_vol );
		}
#endif	//#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV

		//Set a DAC volume for Mixed IP Module
		if ( RET_OK != MIXED_IP_ADEC_L8_SetDACOutVolume( stDACOutVol ) )
		{
			ADEC_ERROR("MIXED_IP_ADEC_L8_SetDACOutVolume : Error!\n");
			return RET_ERROR;
		}
	}

	return RET_OK;
}


/**
 * Sets the mute of each DAC and I2S output port.
 * The SPDIF output is implemented by another IOCTL.(ADEC_IOW_SET_SPDIF_MUTE)
 * @see ADEC_DRV_SetSPDIFOutputType
*/
int ADEC_L8_SetOutputPortMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute )
{
	AUD_SET_DS_VOL 	stSetDSVolume;
	AUD_SET_SH_VOL	stSetSHVolume;

#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV
	AUD_SET_W_VOL	stSetWooferVol;
#endif

	//Sanity check
	if( (stOutPortMute.portNum & LX_ADEC_OUTPUT_ALL) == 0 )
	{
		ADEC_ERROR("ADEC_DRV_SetOutputPortMute : Port Error!(%d)\n", stOutPortMute.portNum);
		return RET_ERROR;
	}

	//DSP mute control
	if(stOutPortMute.mode & LX_ADEC_DAC_DSP)
	{
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			//Read setSHVolume port reg.
			memcpy (&stSetSHVolume, (UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, sizeof (UINT32));

			//Set a SCART DAC mute
			stSetSHVolume.scart_mute_ctrl = stOutPortMute.bSwOnOff;

			//Write setSHVolume port reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, &stSetSHVolume, sizeof (UINT32));
		}

		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC1 )
		{
			//Read setDSVolume port reg.
			memcpy (&stSetDSVolume, (UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, sizeof (UINT32));

			//Set a Line-out DAC mute
			stSetDSVolume.dac_mute_ctrl = stOutPortMute.bSwOnOff;

			//Write setDSVolume port reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, &stSetDSVolume, sizeof (UINT32));
		}

		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			//Read setSHVolume port reg.
			memcpy (&stSetSHVolume, (UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, sizeof (UINT32));

			//Set a HeadPhone DAC mute
			stSetSHVolume.hp_mute_ctrl = stOutPortMute.bSwOnOff;

			//Write setSHVolume port reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, &stSetSHVolume, sizeof (UINT32));
		}

		ADEC_PRINT("ADEC_DRV_SetOutputPortMute : port = %d, SWmute = %d\n", stOutPortMute.portNum, stOutPortMute.bSwOnOff );
		//ADEC_DEBUG_TMP("ADEC_DRV_SetOutputPortMute : port = %d, SWmute = %d\n", stOutPortMute.portNum, stOutPortMute.bSwOnOff );
	}

	//MixedIP volume control
	if(stOutPortMute.mode & LX_ADEC_DAC_MIXEDIP)
	{
#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV
		//Set a HeadPhone DAC mute
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			//Read SetWooferVol reg.
			memcpy (&stSetWooferVol, (UINT32*) &g_pstADECReg_l8->aud_set_w_vol, sizeof (UINT32));

			//Set woofer mute
			stSetWooferVol.w_mute_ctrl = stOutPortMute.bSwOnOff;

			//Write SetWooferVol reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_w_vol, &stSetWooferVol, sizeof (UINT32));

			ADEC_PRINT("ADEC_DRV_SetOutputPortMute : Woofer SWmute = %d(0x%X)\n", stOutPortMute.bSwOnOff, *(UINT32*)&g_pstADECReg_l8->aud_set_w_vol );
		}
#endif	//#ifdef ENABLE_WOOFER_I2S_FOR_HPDRV

		//Set a DAC mute for Mixed IP Module
		if ( RET_OK != MIXED_IP_ADEC_L8_SetDACOutMute( stOutPortMute ) )
		{
			ADEC_ERROR("MIXED_IP_ADEC_L8_SetDACOutMute : Error!\n");
			return RET_ERROR;
		}
	}

	return RET_OK;
}


/**
 * Sets a SPDIF output type.
 * When adecType of ADEC_IOW_START_DECODING is LX_ADEC_SRC_TYPE_EAC3:
 * If eSpdifMode is set as LX_ADEC_SPDIF_AC3, it outputs after transcoding.
 * When adecType of ADEC_IOW_START_DECODING is LX_ADEC_SRC_TYPE_HEAAC:
 * If eSpdifMode is set as LX_ADEC_SPDIF_DTS, it outputs after transcoding.
 * Only LX_ADEC_SPIDF_AC3, LX_ADEC_SPIDF_DTS and LX_ADEC_SPIDF_PCM are valid for this function.
 * @see ADEC_DRV_SetSPDIFVolume
*/
int ADEC_L8_SetSPDIFOutputType ( LX_ADEC_SPDIF_MODE_T eSPDIFMode )
{
	AUD_SET_INOUT	stSetInout;

	//Read Input port reg.
	memcpy (&stSetInout, (UINT32*) &g_pstADECReg_l8->aud_set_inout, sizeof (UINT32));

	switch (eSPDIFMode)
	{
		case LX_ADEC_SPDIF_PCM:
			stSetInout.spdif_type = 0;	//PCM output
			ADEC_PRINT("ADEC_DRV_SetSPDIFOutputType PCM\n" );
			break;

		case LX_ADEC_SPDIF_AC3:
		case LX_ADEC_SPDIF_NONE:
		case LX_ADEC_SPDIF_MP1:
		case LX_ADEC_SPDIF_MP2:
		case LX_ADEC_SPDIF_MP3:
		case LX_ADEC_SPDIF_AAC:
		case LX_ADEC_SPDIF_DTS:
			stSetInout.spdif_type = 1;	//Bitstream Output
			ADEC_PRINT("ADEC_DRV_SetSPDIFOutputType AC3, MPEG, AAC or DTS\n" );
			break;

		default:
			return RET_ERROR;
	}

	//Write Input port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_inout, &stSetInout, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetSPDIFOutputType mode = %d\n", eSPDIFMode );
	return RET_OK;
}

/**
  * Sets a S/PDIF SCMS(Serial Copy Management System) control type.
  * Refer a LX_ADEC_SPDIF_SCMS_T structure.
 * @see ADEC_DRV_SetSPDIFVolume
*/
int ADEC_L8_SetSPDIFScmsCtrl ( LX_ADEC_SPDIF_SCMS_T eSPDIFScmsCtrl )
{
#if 0
	DTV_STATUS_T DDI_ADEC_SetSPDIFSCMS(UINT8 spdif_ctl)
	{
		if(spdif_ctl == 0) //copy free
		{
		spdifOutputSettings.channelStatusInfo.swCopyRight = false;
		spdifOutputSettings.channelStatusInfo.categoryCode &= 0x7f;  /* 0x01 */
		}
		else if(spdif_ctl == 2) //copy once
		{
		spdifOutputSettings.channelStatusInfo.swCopyRight = true;
		spdifOutputSettings.channelStatusInfo.categoryCode &= 0x7f;
		}
		else if(spdif_ctl == 3 || spdif_ctl == 1) //copy no more copy never
		{
		spdifOutputSettings.channelStatusInfo.swCopyRight = true;
		spdifOutputSettings.channelStatusInfo.categoryCode |= 0x80;  /* 0x01 */
		}
	}
#endif

	return RET_OK;
}

/**
 * Sets a SPDIF output volume.
 * Range of input parameter value is 0x00 to 0x7E.
 * @see ADEC_DRV_SetSPDIFMute
*/
int ADEC_L8_SetSPDIFVolume ( UINT32 ui32Volume )
{
	AUD_SET_DS_VOL stSetDSVolume;

	//Read setDSVolume port reg.
	memcpy (&stSetDSVolume, (UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, sizeof (UINT32));

	//Set SPDIF volume
	stSetDSVolume.spdif_volume      = ui32Volume;
	stSetDSVolume.spdif_vol_ctrl_en = 1;

	//Write setDSVolume port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, &stSetDSVolume, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetSPDIFVolume %d : OK\n", ui32Volume );
//	ADEC_DEBUG_TMP("ADEC_DRV_SetSPDIFVolume %d : OK\n", ui32Volume );

	return RET_OK;
}


/**
 * Sets a SPDIF mute on or off.
 * Stops the audio digital data output.
 * @see ADEC_DRV_SetSPDIFVolume
*/
int ADEC_L8_SetSPDIFMute ( BOOLEAN bMuteFlag )
{
	AUD_SET_DS_VOL stSetDSVolume;

	//Read setDSVolume port reg.
	memcpy (&stSetDSVolume, (UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, sizeof (UINT32));

	//Set S/PDIF mute
	stSetDSVolume.spdif_mute_ctrl = bMuteFlag;

	//Write setDSVolume port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, &stSetDSVolume, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetSPDIFMute : %d\n", bMuteFlag );
	//ADEC_DEBUG_TMP("ADEC_DRV_SetSPDIFMute : %d\n", bMuteFlag );

	return RET_OK;
}


/**
 * Enables or disables the AV Lipsync mechanism.
 * Controls the lipsync function manually.
 * @see
*/
int ADEC_L8_EnableAVLipSync ( UINT8 ui8AVLipSyncFlag )
{
	AUD_LIP_CFG stAudLipCFG;

	//Read AudLipCfg port reg.
	memcpy (&stAudLipCFG, (UINT32*) &g_pstADECReg_l8->aud_lip_cfg, sizeof (UINT32));

	//Check which deocder(Main or ASOC) is enabled.
	stAudLipCFG.main_lip_en0 = ui8AVLipSyncFlag;
	stAudLipCFG.asoc_lip_en0 = ui8AVLipSyncFlag;

	//Write AudLipCfg port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_lip_cfg, &stAudLipCFG, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_EnableAVLipSync : %d\n", ui8AVLipSyncFlag );
	ADEC_DEBUG("ADEC_DRV_EnableAVLipSync : %d\n", ui8AVLipSyncFlag );

	return RET_OK;
}


/**
 * Sets a delay time in milli-second of I2S Out of PCM.
 * Add a delay time for PCM input audio data.
 * @see ADEC_DRV_SetSPDIFDelayTime
*/
int ADEC_L8_SetI2SOutDelayTime ( UINT32 ui32DelayinMS )
{
	AUD_SET_DLY0	stSetDly0;

	//Read Set Delay0 reg.
	memcpy (&stSetDly0, (UINT32*) &g_pstADECReg_l8->aud_set_dly0, sizeof (UINT32));

	//Set I2S delay time
	stSetDly0.i2s_delay = ui32DelayinMS;

	//Write Set Delay0 reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_dly0, &stSetDly0, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetI2SOutDelayTime delay : %d ms\n", ui32DelayinMS );
//	ADEC_DEBUG_TMP("ADEC_DRV_SetI2SOutDelayTime delay : %d ms\n", ui32DelayinMS );

	return RET_OK;
}

/**
 * Sets a delay time in milli-second of DAC Out of PCM.
 * Add a delay time for PCM input audio data.
 * @see ADEC_DRV_SetSPDIFDelayTime
*/
int ADEC_L8_SetDACOutDelayTime ( UINT32 ui32DelayinMS )
{
	AUD_SET_DLY1	stSetDly1;

	//Read Set Delay1 reg.
	memcpy (&stSetDly1, (UINT32*) &g_pstADECReg_l8->aud_set_dly1, sizeof (UINT32));

	//Set check bit for delay1
	stSetDly1.dac_delay = ui32DelayinMS;

	//Write Set Delay1 reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_dly1, &stSetDly1, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetDACOutDelayTime delay : %d ms\n", ui32DelayinMS );
	ADEC_DEBUG_TMP("ADEC_DRV_SetDACOutDelayTime delay : %d ms\n", ui32DelayinMS );

	return RET_OK;
}

/**
 * Sets a delay time in milli-second of SPDIF.
 * Add a delay time for SPDIF input audio data.
 * @see ADEC_DRV_SetPCMDelayTime
*/
int ADEC_L8_SetSPDIFDelayTime ( UINT32 ui32DelayinMS )
{
	AUD_SET_DLY0	stSetDly0;

	//Read Set Delay0 reg.
	memcpy (&stSetDly0, (UINT32*) &g_pstADECReg_l8->aud_set_dly0, sizeof (UINT32));

	//Set S/PDIF delay time
	stSetDly0.spdif_delay = ui32DelayinMS;

	//Write Set Delay0 reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_dly0, &stSetDly0, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetSPDIFDelayTime delay  : %d ms\n", ui32DelayinMS );
	ADEC_DEBUG_TMP("ADEC_DRV_SetSPDIFDelayTime delay  : %d ms\n", ui32DelayinMS );

	return RET_OK;
}


/**
 * Sets a trick state of ADEC.
 * If ADEC is running in DVR mode(Pause or Resume state), ADEC trick state can be changed.
 * @see
*/
int ADEC_L8_SetTrickState ( LX_ADEC_TRICK_MODE_T	eTrickMode )
{
	int 				eRetVal = RET_OK;

	AUD_PLAY_MODE		stPlayMode;

	//Read Play Mode reg.
	memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

	//DSP 0 only currently
	switch (eTrickMode)
	{
		case LX_ADEC_TRICK_FORCED_STOP_OFF:
			stPlayMode.trick_play_mode = 0x0; //Normal Speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Play\n" );
			break;

		case LX_ADEC_TRICK_FORCED_STOP_ON:
			stPlayMode.trick_play_mode = 0x2; //Forced Stop On
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Stop\n" );
			break;

		case LX_ADEC_TRICK_PAUSE:
			stPlayMode.trick_play_mode = 0x1; //Pause Mode	
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Pause\n" );
			break;

		case LX_ADEC_TRICK_NORMAL_PLAY:
			//Normal playing
			stPlayMode.trick_play_mode = 0x0; //Normal Speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Normal Play\n" );
			break;

		case LX_ADEC_TRICK_FAST_FOWARD_1P2X:
			stPlayMode.trick_play_mode = 0x3; //Fast forward x1.2 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Fast forward x1.2 speed\n" );
			break;

		case LX_ADEC_TRICK_FAST_FOWARD_1P5X:
			stPlayMode.trick_play_mode = 0x4; //Fast forward x1.5 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Fast forward x1.5 speed\n" );
			break;

		case LX_ADEC_TRICK_FAST_FOWARD_2X:
			stPlayMode.trick_play_mode = 0x5; //Fast forward x2 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Fast forward x2 speed\n" );
			break;

		case LX_ADEC_TRICK_SLOW_MOTION_0P8X:
			stPlayMode.trick_play_mode = 0x6; //slow x0.8 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Slow x0.8 speed\n" );
			break;

		case LX_ADEC_TRICK_SLOW_MOTION_0P5X:
			stPlayMode.trick_play_mode = 0x5; //slow x0.5 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Slow x0.5 speed\n" );
			break;

		case LX_ADEC_TRICK_MUTE_OFF:
		case LX_ADEC_TRICK_MUTE_ON:
		case LX_ADEC_TRICK_TSM_DISABLED:
		case LX_ADEC_TRICK_TSM_ENABLED:
		default:
			ADEC_ERROR("ADEC_DRV_SetTrickState : Err Invalid Param\n" );
			eRetVal = RET_INVALID_PARAMS;
			break;
	}

	//Write play mode reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetTrickState mode : %d\n", eTrickMode );

	return eRetVal;
}

/**
 * Gets and sets a clock information of ADEC driver.
 * This function gets and sets a clock information of ADEC driver.
 * A clock information has STC, PTS and GSTC value.
*/
int ADEC_L8_GetAndSetClockInfo ( LX_ADEC_CLOCK_INFO_T *pstClockInfo )
{
	AUD_GSTCCHREG	stGSTCH;
	int 			eRetVal = RET_OK;

#ifdef ENABLE_INTERVAL_CEHCK
	UINT32 loop = 0, i = 0;

	g_ADECFrameDoneInfo.max_interval = 0;
	g_ADECFrameDoneInfo.total_time = 0;
	g_ADECFrameDoneInfo.min_interval = 0xFFFFFFFF;
	ADEC_DEBUG("\nFrame Decode Done Intterupt interval:\n");
	for (loop = g_ADECFrameDoneInfo.count; loop < MAX_INTERVAL_COUNT + g_ADECFrameDoneInfo.count; loop++)
	{
		if (loop >= MAX_INTERVAL_COUNT)		i = loop - MAX_INTERVAL_COUNT;

		g_ADECFrameDoneInfo.total_time += g_ADECFrameDoneInfo.interval[i];
		if (g_ADECFrameDoneInfo.max_interval < g_ADECFrameDoneInfo.interval[i])
		{
			g_ADECFrameDoneInfo.max_interval = g_ADECFrameDoneInfo.interval[i];
		}
		else if (g_ADECFrameDoneInfo.min_interval > g_ADECFrameDoneInfo.interval[i])
		{
			g_ADECFrameDoneInfo.min_interval = g_ADECFrameDoneInfo.interval[i];
		}

 		if ((loop - g_ADECFrameDoneInfo.count) % 10 == 0)
			printk("\n(line %3d)\t", (loop - g_ADECFrameDoneInfo.count) / 10 + 1);

		printk("%4d\t", g_ADECFrameDoneInfo.interval[i] / 90);
	}
	printk("\n");
	ADEC_DEBUG("\n\t MAX = %d, \t MIN = %d, \t AVER = %d\n",
				g_ADECFrameDoneInfo.max_interval / 90,
				g_ADECFrameDoneInfo.min_interval / 90,
				g_ADECFrameDoneInfo.total_time / 90 / MAX_INTERVAL_COUNT);

	g_ADECDecDoneInfo.max_interval = 0;
	g_ADECDecDoneInfo.total_time = 0;
	g_ADECDecDoneInfo.min_interval = 0xFFFFFFFF;
	ADEC_DEBUG("\nIndex Decode Done Intterupt interval:\n");
	for (loop = g_ADECDecDoneInfo.count; loop < MAX_INTERVAL_COUNT + g_ADECDecDoneInfo.count; loop++)
	{
		if (loop >= MAX_INTERVAL_COUNT)		i = loop - MAX_INTERVAL_COUNT;

		g_ADECDecDoneInfo.total_time += g_ADECDecDoneInfo.interval[i];
		if (g_ADECDecDoneInfo.max_interval < g_ADECDecDoneInfo.interval[i])
		{
			g_ADECDecDoneInfo.max_interval = g_ADECDecDoneInfo.interval[i];
		}
		else if (g_ADECDecDoneInfo.min_interval > g_ADECDecDoneInfo.interval[i])
		{
			g_ADECDecDoneInfo.min_interval = g_ADECDecDoneInfo.interval[i];
		}

 		if ((loop - g_ADECDecDoneInfo.count) % 10 == 0)
			printk("\n(line %3d)\t", (loop - g_ADECDecDoneInfo.count) / 10 + 1);

		printk("%4d\t", g_ADECDecDoneInfo.interval[i] / 90);
	}
	printk("\n");
	ADEC_DEBUG("\n\t MAX = %d, \t MIN = %d, \t AVER = %d\n",
				g_ADECDecDoneInfo.max_interval / 90,
				g_ADECDecDoneInfo.min_interval / 90,
				g_ADECDecDoneInfo.total_time / 90 / MAX_INTERVAL_COUNT);
#endif

	if(pstClockInfo->mode == LX_ADEC_CLOCK_MODE_GET)
	{
		//Read GSTCH reg.
		memcpy (&stGSTCH, (UINT32*) &g_pstADECReg_l8->aud_gstcchreg, sizeof (UINT32));
		pstClockInfo->clockGSTCH = (UINT32)stGSTCH.aud_gstcchreg;

		//Read GSTC, STC, PTSM and PTSA
		pstClockInfo->clockGSTCL = g_pstADECReg_l8->aud_gstcclreg;
		pstClockInfo->clockSTC	 = g_pstADECReg_l8->aud_stc24;
		pstClockInfo->clockPTSM  = g_pstADECReg_l8->aud_ptsm24;
		pstClockInfo->clockPTSA  = g_pstADECReg_l8->aud_ptsa24;
	}

	ADEC_PRINT("ADEC_DRV_GetAndSetClockInfo GSTCL : %d\n", pstClockInfo->clockGSTCL );

	return eRetVal;
}


/**
 * Sets a audio balance.
 * Range of input parameter value is 0 to 100.
 * Value 50 is the center value.
 * @see ADEC_DRV_SetBASS
*/
int ADEC_L8_SetBalance ( UINT8 ui8BalanceVal )
{
	ADEC_ERROR("ADEC_DRV_SetBalance level : %d ==> Not Supported!!!\n", ui8BalanceVal );



	ADEC_PRINT("ADEC_DRV_SetBalance level : %d\n", ui8BalanceVal );

	return RET_OK;
}


/**
 * Sets a audio bass.
 * Range of input parameter value is 0 to 100.
 * Value 50 is the center value.
 * @see ADEC_DRV_SetBalance
*/
int ADEC_L8_SetBASS ( UINT8 ui8BassVal )
{
	AUD_BT_LEVEL stBTlevel;

	//Read setBTlevel port reg.
	memcpy (&stBTlevel, (UINT32*) &g_pstADECReg_l8->aud_bt_level, sizeof (UINT32));

	//Set bass level
	stBTlevel.bass_level = ui8BassVal;

	//Write setBTlevel port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_bt_level, &stBTlevel, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetBASS level : %d\n", ui8BassVal );

	return RET_OK;
}


/**
 * Sets a audio Treble.
 * Range of input parameter value is 0 to 100.
 * Value 50 is the center value.
 * @see ADEC_DRV_SetBASS
*/
int ADEC_L8_SetTreble ( UINT8 ui8TrebleVal )
{
	AUD_BT_LEVEL stBTlevel;

	//Read setBTlevel reg.
	memcpy (&stBTlevel, (UINT32*) &g_pstADECReg_l8->aud_bt_level, sizeof (UINT32));

	//Set treble level
	stBTlevel.treble_level = ui8TrebleVal;

	//Write setBTlevel reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_bt_level, &stBTlevel, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_SetTreble level : %d\n", ui8TrebleVal );

	return RET_OK;
}


/**
 * Sets a Clear Voice(CV) mode.
 * CV version is defined by cvVersion parameter.
 * CV mode is enabled or disabled by LX_ADEC_CV_MODE_T's each field value.
 * @see
*/
int ADEC_L8_CVSetMode ( LX_ADEC_CV_SET_MODE_T stCVSetMode )
{
	AUD_SET_SE 		stSetSe;

	//Read set SE reg.
	memcpy (&stSetSe, (UINT32*) &g_pstADECReg_l8->aud_set_se, sizeof (UINT32));

	//Set Set SE reg.
	stSetSe.se_mode = stCVSetMode.cvMode;	//set CV Mode

	//Write set SE reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_se, &stSetSe, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_CVSetMode cvMode 0x%x[0x%x]\n", stCVSetMode.cvMode, *(UINT32*)&g_pstADECReg_l8->aud_set_se );

	return RET_OK;
}

/**
 * Sets the parameter's Clear Voice2(CV2) Level of CV.
 * The numbers of CV2 level are transferred by UINT32 cvParams[LX_CV_CV2_PARAM_NUM].
 * @see ADEC_DRV_CVSetMode
*/
int ADEC_L8_CVSetCV ( LX_ADEC_CV_SET_CV_T stCVSetCV )
{
	UINT32		ui32Count = 0;

	//Copy Clear Voice2(CV2) parameter
	for(ui32Count = 0; ui32Count < LX_CV_CV2_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_CV2_PARAM + ui32Count] = stCVSetCV.cvParams[ui32Count];
	}

#ifdef ENABLE_LGSE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_DEBUG("Clear Voice2 Param Start...\n");
	for(ui32Count = 0; ui32Count < LX_CV_CV2_PARAM_NUM; ui32Count++)
	{
		ADEC_DEBUG("CV2 Param[%3d] = 0x%8X\n", ui32Count + 1, g_pui32LGSEParamVirAddr[ADDR_LGSE_CV2_PARAM + ui32Count]);
	}
	ADEC_DEBUG("Clear Voice2 Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG

	return RET_OK;
}


/**
 * Sets the parameter's Surround(Infinite Sound) Level of CV.
  * The numbers of surround level are transferred by UINT32 surround_params[LX_CV_IFNSND_PARAM_NUM].
 * @see ADEC_DRV_CVSetMode
*/
int ADEC_L8_CVSetSurround ( LX_ADEC_CV_SET_SURROUND_T stCVSetSurround )
{
	UINT32		ui32Count = 0;

	//Copy Infinite Sound parameter
	for(ui32Count = 0; ui32Count < LX_CV_IFNSND_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_IFNSND_PARAM + ui32Count] = stCVSetSurround.cvParams[ui32Count];
	}

#ifdef ENABLE_LGSE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_DEBUG("Infinite Sound Param Start...\n");
	for(ui32Count = 0; ui32Count < LX_CV_IFNSND_PARAM_NUM; ui32Count++)
	{
		ADEC_DEBUG("IFNSND Param[%3d] = 0x%8X\n", ui32Count + 1, g_pui32LGSEParamVirAddr[ADDR_LGSE_IFNSND_PARAM + ui32Count]);
	}
	ADEC_DEBUG("Infinite Sound Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG

	return RET_OK;
}


/**
 * Sets the parameter's User Equalizer Band Level of CV.
 * The numbers of UEQ level are transferred by UINT32 ueqParams[LX_CV_UEQ_PARAM_NUM].
 * @see ADEC_DRV_CVSetMode
*/
int ADEC_L8_CVSetUEQ ( LX_ADEC_CV_SET_UEQ_T *pstCVSetUEQ )
{
	UINT32		ui32Count = 0;

	//Check UEQ mode
	if(pstCVSetUEQ->mode == LX_ADEC_CV_UEQ_MODE)
	{
		//Copy User Equalizer Band  parameter
		for(ui32Count = 0; ui32Count < LX_CV_UEQ_PARAM_NUM; ui32Count++)
		{
			g_pui32LGSEParamVirAddr[ADDR_LGSE_UEQ_PARAM + ui32Count] = pstCVSetUEQ->ueqParams[ui32Count];
		}

#ifdef ENABLE_LGSE_PARAM_DEBUG
		//Read parameter value for debug
		ADEC_DEBUG("UEQ Param Start...\n");
		for(ui32Count = 0; ui32Count < LX_CV_UEQ_PARAM_NUM; ui32Count++)
		{
			ADEC_DEBUG("UEQ Param[%3d] = 0x%8X\n", ui32Count + 1, g_pui32LGSEParamVirAddr[ADDR_LGSE_UEQ_PARAM + ui32Count]);
		}
		ADEC_DEBUG("UEQ Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG
	}
	else if(pstCVSetUEQ->mode == LX_ADEC_CV_UEQ2_MODE)
	{
		//Copy User Equalizer 2 Band parameter
		for(ui32Count = 0; ui32Count < LX_CV_UEQ2_PARAM_NUM; ui32Count++)
		{
			g_pui32LGSEParamVirAddr[ADDR_LGSE_UEQ2_PARAM + ui32Count] = pstCVSetUEQ->ueq2Params[ui32Count];
		}

#ifdef ENABLE_LGSE_PARAM_DEBUG
		//Read parameter value for debug
		ADEC_DEBUG("UEQ2 Param Start...\n");
		for(ui32Count = 0; ui32Count < LX_CV_UEQ2_PARAM_NUM; ui32Count++)
		{
			ADEC_DEBUG("UEQ2 Param[%3d] = 0x%8X\n", ui32Count + 1, g_pui32LGSEParamVirAddr[ADDR_LGSE_UEQ2_PARAM + ui32Count]);
		}
		ADEC_DEBUG("UEQ2 Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG
	}
	else
	{
		ADEC_ERROR("ADEC_DRV_CVSetUEQ : Err - Mode Not Defined!(0x%X)\n", pstCVSetUEQ->mode);
		return RET_ERROR;
	}

	return RET_OK;
}


/**
 * Sets the parameter's User Volume Level of CV.
 * The numbers of User Volume level are transferred by UINT8 volParam.
 * @see ADEC_DRV_CVSetMode
*/
int ADEC_L8_CVSetVolume ( UINT8 ui8CVSetVolume )
{
	AUD_SET_SE		stSetSe;

	//Read set SE reg.
	memcpy ( &stSetSe, (UINT32*) &g_pstADECReg_l8->aud_set_se, sizeof (UINT32));

	//Set osd volume
	stSetSe.osd_volume = ui8CVSetVolume;

	//Write set SE reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_se, &stSetSe, sizeof (UINT32));

	ADEC_PRINT("ADEC_DRV_CVSetVolume 0x%x[0x%x]\n", ui8CVSetVolume, *(UINT32*)&g_pstADECReg_l8->aud_set_se );
	return RET_OK;
}


/**
 * Sets the parameter's Auto(Perceptual) Volume Level of CV.
 * The numbers of Auto Volume Level are transferred by UINT32 avLevel[LX_CV_AUTOVOLUME_PARAM_NUM].
 * @see ADEC_DRV_CVSetMode
*/
int ADEC_L8_CVSetAVL ( LX_ADEC_CV_SET_AVL_T	stSetAVL )
{
	UINT32		ui32Count = 0;

	//Copy auto volume parameter
	for(ui32Count = 0; ui32Count < LX_CV_AUTOVOLUME_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_AUTOVOLUME_PARAM + ui32Count] = stSetAVL.avLevel[ui32Count];
	}

#ifdef ENABLE_LGSE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_DEBUG("Auto Volume Level Param Start...\n");
	for(ui32Count = 0; ui32Count < LX_CV_AUTOVOLUME_PARAM_NUM; ui32Count++)
	{
		ADEC_DEBUG("AVL Param[%3d] = 0x%8X\n", ui32Count + 1, g_pui32LGSEParamVirAddr[ADDR_LGSE_AUTOVOLUME_PARAM + ui32Count]);
	}
	ADEC_DEBUG("Auto Volume Level Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG

	return RET_OK;
}


/**
 * Sets the parameter's Bass EQ(Bass throughput compensation & enhancement) Level of CV.
 * The numbers of Bass EQ Level are transferred by UINT32 bassEQParams[LX_CV_DEQ_PARAM_NUM].
 * @see ADEC_DRV_CVSetMode
*/
int ADEC_L8_CVSetBASS ( LX_ADEC_CV_SET_BASS_EQ_T *pstCVSetBassEQ )
{
	UINT32		ui32Count = 0;

	//Copy Dynamic EQ parameter
	for(ui32Count = 0; ui32Count < LX_CV_DEQ_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_DEQ_PARAM + ui32Count] = pstCVSetBassEQ->bassEQParams[ui32Count];
	}

#ifdef ENABLE_LGSE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_DEBUG("DEQ Param Start...\n");
	for(ui32Count = 0; ui32Count < LX_CV_DEQ_PARAM_NUM; ui32Count++)
	{
		ADEC_DEBUG("DEQ Param[%3d] = 0x%8X\n", ui32Count + 1, g_pui32LGSEParamVirAddr[ADDR_LGSE_DEQ_PARAM + ui32Count]);
	}
	ADEC_DEBUG("DEQ Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG

	return RET_OK;
}

/**
 *Sets the parameter's INPUT Level of CV.
 *The numbers of INPUT Level are transferred by UINT32 inLevParams[LX_CV_INLEV_PARAM_NUM].
 * @see ADEC_DRV_CVSetMode
*/
int ADEC_L8_CVSetInLevel ( LX_ADEC_CV_SET_INLEV_T stCVSetInLev )
{
	UINT32		ui32Count = 0;

	//Clears the sound level for left and righr channel
	g_pstADECReg_l8->aud_levelmo_l = 0x0;
	g_pstADECReg_l8->aud_levelmo_r = 0x0;

	//Copy INPUT Level  parameter
	for(ui32Count = 0; ui32Count < LX_CV_INLEV_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_INLEV_PARAM + ui32Count] = stCVSetInLev.inLevParams[ui32Count];
	}

#ifdef ENABLE_LGSE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_DEBUG("INPUT Level Param Start...\n");
	for(ui32Count = 0; ui32Count < LX_CV_INLEV_PARAM_NUM; ui32Count++)
	{
		ADEC_DEBUG("INPUT Level Param[%3d] = 0x%8X\n", ui32Count + 1, g_pui32LGSEParamVirAddr[ADDR_LGSE_INLEV_PARAM + ui32Count]);
	}
	ADEC_DEBUG("INPUT Level Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG

	return RET_OK;
}

/**
 * Gets a sound level(dB unit) of audio output.
 * Check a analog audio input is exist.
 * @see
*/
int ADEC_L8_CVGetInLevel ( LX_ADEC_CV_GET_INLEV_T *pstui32SoundLevel )
{
	//Read the sound level for left and righr channel
	pstui32SoundLevel->leftChannel  = g_pstADECReg_l8->aud_levelmo_l;
	pstui32SoundLevel->rightChannel = g_pstADECReg_l8->aud_levelmo_r;

	ADEC_PRINT("ADEC_DRV_CVGetInLevel : L(0x%x), R(0x%x)\n", pstui32SoundLevel->leftChannel, pstui32SoundLevel->rightChannel);
	ADEC_DEBUG("ADEC_DRV_CVGetInLevel : L(0x%x), R(0x%x)\n", pstui32SoundLevel->leftChannel, pstui32SoundLevel->rightChannel);

	return RET_OK;
}

/**
 * Gets a CV paramter from kernel memory.
 * Check a CV parameter for debug function.
 * @see
*/
int ADEC_L8_CVGetParam ( LX_ADEC_CV_GET_PARAM_T *pstCVGetParam )
{
	UINT32	addrOfParam = 0, sizeOfParams = 0;
	UINT32	ui32Count;

	//Get a address and size of parameter
	switch (pstCVGetParam->cvMode)
	{
		case LX_CV_AUTOVOLUME:
			addrOfParam  = ADDR_LGSE_AUTOVOLUME_PARAM;
			sizeOfParams = LX_CV_AUTOVOLUME_PARAM_NUM;
			break;

		case LX_CV_CV2:
			addrOfParam  = ADDR_LGSE_CV2_PARAM;
			sizeOfParams = LX_CV_CV2_PARAM_NUM;
			break;

		case LX_CV_UEQ:
			addrOfParam  = ADDR_LGSE_UEQ_PARAM;
			sizeOfParams = LX_CV_UEQ_PARAM_NUM;
			break;

		case LX_CV_UEQ2:
			addrOfParam  = ADDR_LGSE_UEQ2_PARAM;
			sizeOfParams = LX_CV_UEQ2_PARAM_NUM;
			break;

		case LX_CV_DEQ:
			addrOfParam  = ADDR_LGSE_DEQ_PARAM;
			sizeOfParams = LX_CV_DEQ_PARAM_NUM;
			break;

		case LX_CV_INFINITESND:
			addrOfParam  = ADDR_LGSE_IFNSND_PARAM;
			sizeOfParams = LX_CV_IFNSND_PARAM_NUM;
			break;

		case LX_CV_INLEV_CHECK:
		case LX_CV_INFREQ_CHECK:
			addrOfParam  = ADDR_LGSE_INLEV_PARAM;
			sizeOfParams = LX_CV_INLEV_PARAM_NUM;
			break;

		default:
			ADEC_ERROR("ADEC_DRV_CVGetParam : Invalid Params!\n");
			return RET_ERROR;
	}

	//Copy sizeOfParams
	pstCVGetParam->sizeOfParams = sizeOfParams;

	//Copy each LG Sound Engine parameter
	for(ui32Count = 0; ui32Count < sizeOfParams; ui32Count++)
	{
		pstCVGetParam->getParams[ui32Count] = g_pui32LGSEParamVirAddr[addrOfParam + ui32Count];
	}

#ifdef ENABLE_LGSE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_DEBUG("Getted Param Start...\n");
	for(ui32Count = 0; ui32Count < sizeOfParams; ui32Count++)
	{
		ADEC_DEBUG("Getted Param[%3d] = 0x%8X\n", ui32Count + 1, pstCVGetParam->getParams[ui32Count]);
	}
	ADEC_DEBUG("Getted Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG

	ADEC_DEBUG("ADEC_DRV_CVGetParam : Num = %d, Param[0] = 0x%X\n", pstCVGetParam->sizeOfParams, pstCVGetParam->getParams[0]);

	return RET_OK;
}


/**
 * Loads an audio clip.
 * This function copies the audio clip data into ADEC driver memory.
 * @see ADEC_DRV_StartAudioClip
*/
int ADEC_L8_LoadAudioClip ( LX_ADEC_CLIP_MEM_INFO_T stMemInfo )
{
	AUD_PLAY_MODE		stPlayMode;
	AUD_MPB0_CONFIG0	stMPB0Cfg0;

	/* Set parameter for clip play */
	if(stMemInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		/* Set write pointer for clip play */
		g_pstADECReg_l8->aud_pesm_wptr0 = stMemInfo.clipBuf.writeOffset;

		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		/* Set a buffer state */
		stPlayMode.buffer_state = stMemInfo.bufStatus;

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(stMemInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		/* Set write pointer for clip play */
		g_pstADECReg_l8->aud_pesa_wptr0 = stMemInfo.clipBuf.writeOffset;

		/* Set a buffer state */
		//stPlayMode.buffer_state = stMemInfo.bufStatus;
	}
	else if(stMemInfo.adecMode == LX_ADEC_MIX_BUF0)
	{
		/* Set write pointer for PCM mix */
		g_pstADECReg_l8->aud_mpb0_wptr = stMemInfo.clipBuf.writeOffset;

		//Read MPB0 config0 reg.
		memcpy (&stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

		/* Set a buffer state */
		stMPB0Cfg0.buffer_state = stMemInfo.bufStatus;

		//Write MPB0 config0 reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));
	}
	else
	{
		ADEC_ERROR("LOAD : mode is not defined(adecMode = 0x%X)!!!\n", stMemInfo.adecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("ADEC_DRV_LoadAudioClip mode [0x%x], wptr [0x%x], bstat [0x%x]\n",	\
				stMemInfo.adecMode, stMemInfo.clipBuf.writeOffset, stMemInfo.bufStatus );

	if( (stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_START)
	  ||(stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_END) )
	{
		ADEC_DEBUG("Clip Load   mode = 0x%x, bstat = %d, wptr [0x%x]\n",	\
					stMemInfo.adecMode, stMemInfo.bufStatus, stMemInfo.clipBuf.writeOffset);
	}

 	return RET_OK;
}


/**
 * Starts a playing of the audio clip.
 * Audio clip must be loaded by the ADEC_IOW_LOAD_AUDIO_CLIP function for using this function.
 * @see ADEC_DRV_LoadAudioClip
*/
int ADEC_L8_StartAudioClip ( LX_ADEC_CLIP_START_INFO_T stStartInfo )
{
	int eRetVal = RET_OK;

	AUD_PLAY_MODE		stPlayMode;

	AUD_DECCFG1			stDecCfg1;
	AUD_DECCFG2			stDecCfg2;
	AUD_DECCFG3			stDecCfg3;

	AUD_MPB0_CONFIG0	stMPB0Cfg0;
	AUD_MPB0_CONFIG1	stMPB0Cfg1;

#if defined(ENABLE_MS10_DDC_CERTI) || defined(ENABLE_DTS_CERTI)
	//Sets the CTOP CTRL Clock PLL Registers.
	(void)MIXED_IP_ADEC_L8_SetSamplingFreq (stStartInfo.samplingFreq);
#endif	//#if defined(ENABLE_MS10_DDC_CERTI) || defined(ENABLE_DTS_CERTI)

	/* Set parameter for clip play */
	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		//Read decoder cfg0 reg.
		AUD_L8_RdFL(aud_deccfg0);
		AUD_L8_Wr01(aud_deccfg0, bitrate, (UINT16)(stStartInfo.bitRate/1000)); 	//kbps unit
		AUD_L8_Wr01(aud_deccfg0, aac_pf_format, 0);
		AUD_L8_Wr01(aud_deccfg0, aac_bs_format, 0);
		AUD_L8_WrFL(aud_deccfg0);

		//Read decoder cfg1 reg.
		memcpy (&stDecCfg1, (UINT32*) &g_pstADECReg_l8->aud_deccfg1, sizeof (UINT32));

		/* Set : sampling frequency and SRC(Sample Rate Converter) module */
		stDecCfg1.sfreq = stStartInfo.samplingFreq;

		/* Set:  bit per sample number */
		stDecCfg1.bit_num = stStartInfo.pcmBitsPerSample;

		/* Set : number of channel */
		stDecCfg1.ch_num  = stStartInfo.numberOfChannel;

		//Write cfg1 mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_deccfg1, &stDecCfg1, sizeof (UINT32));

		ADEC_DEBUG("Clip Info1  bitrate = %d, freq = %d, ch_num = %d, bit_num = %d\n",	\
					stStartInfo.bitRate/1000, stStartInfo.samplingFreq, stStartInfo.numberOfChannel, stStartInfo.pcmBitsPerSample);

		if(stStartInfo.adecType == LX_ADEC_SRC_TYPE_WMA)
		{
			//Read decoder cfg2 reg.
			memcpy (&stDecCfg2, (UINT32*) &g_pstADECReg_l8->aud_deccfg2, sizeof (UINT32));

			/* Set : average bytes per seconds for WMA codec */
			stDecCfg2.avg_bytes_per_sec = stStartInfo.avgBytesPerSec;

			//Write cfg2 mode reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_deccfg2, &stDecCfg2, sizeof (UINT32));


			//Read decoder cfg3 reg.
			memcpy (&stDecCfg3, (UINT32*) &g_pstADECReg_l8->aud_deccfg3, sizeof (UINT32));

			/* Set : block alignment for WMA codec */
			stDecCfg3.block_align = stStartInfo.blockAlign;

			/* Set : encoder option for WMA codec */
			stDecCfg3.encode_opt = stStartInfo.encoderOption;

			//Write cfg1 mode reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_deccfg3, &stDecCfg3, sizeof (UINT32));

			ADEC_DEBUG("Clip Info  avg_bytes = %d, block_align = %d, enc_opt = %d\n",	\
						stStartInfo.avgBytesPerSec, stStartInfo.blockAlign, stStartInfo.encoderOption);
		}

		//Set codec source by adec type
		eRetVal = ADEC_LoadCodec(stStartInfo.adecType);

		if (eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_DRV_StartAudioClip : Error!\n");
			return RET_ERROR;
		}

		/* Check AAC codec to start after AAC profile and format detection */
		if( (stStartInfo.adecType != LX_ADEC_SRC_TYPE_AAC) && (stStartInfo.adecType != LX_ADEC_SRC_TYPE_HEAAC) )
		{
			//Read play mode reg.
			memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

			//Set for repeat number
			stPlayMode.repeat_number = stStartInfo.repeatNumber;

			//Set for start decoding
			stPlayMode.play_mode = 0x1; 	//Start decoding in DSP0

			//Write play mode reg.
			memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));

			ADEC_DEBUG("Clip codec type = %d\n", stStartInfo.adecType);
		}
		else
		{
			ADEC_DEBUG("Clip AAC codec type = %d\n", stStartInfo.adecType);
		}
	}
	else if(stStartInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		//Start decoding AD decoding
		stPlayMode.ad_play_mode = 1;

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(stStartInfo.adecMode == LX_ADEC_MIX_BUF0)
	{
		//Read MPB0 config0 reg.
		memcpy (&stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

		/* Set a repeat number */
		stMPB0Cfg0.repeat_number = stStartInfo.repeatNumber;

		//Write MPB0 config0 reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));

		//Read MPB0 config1 reg.
		memcpy (&stMPB0Cfg1, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config1, sizeof (UINT32));

		/* Set : sampling frequency and SRC(Sample Rate Converter) module */
		stMPB0Cfg1.sfreq   = stStartInfo.samplingFreq;

		/* Set:  bit per sample number */
		stMPB0Cfg1.bit_num = stStartInfo.pcmBitsPerSample;

		/* Set : number of channel */
		stMPB0Cfg1.ch_num  = stStartInfo.numberOfChannel;

		//Write cfg1 mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config1, &stMPB0Cfg1, sizeof (UINT32));

		ADEC_DEBUG("Mix0 Info  freq = %d, ch_num = %d, bit_num = %d\n",	\
					stStartInfo.samplingFreq, stStartInfo.numberOfChannel, stStartInfo.pcmBitsPerSample);


		//Read MPB0 config0 reg.
		memcpy (&stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

		//Set for start mixing
		stMPB0Cfg0.play_mode = 0x1; 	//Start mixing in DSP1

		//Write MPB0 config0 reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));
	}
	else
	{
		ADEC_ERROR("PLAY : mode is not defined(adecMode = 0x%X)!!!\n", stStartInfo.adecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("Audio clip play started(adecMode = 0x%X, repeat = %d)...\n", stStartInfo.adecMode, stStartInfo.repeatNumber);
	ADEC_DEBUG("Clip Start (Mode = 0x%X, type = %d, repeat = %d)\n", stStartInfo.adecMode, stStartInfo.adecType, stStartInfo.repeatNumber);

	return RET_OK;
}


/**
 * Stops a playing of the audio clip.
 * This function stops a playing of the audio clip.
 * @see ADEC_DRV_StartAudioClip
*/
int ADEC_L8_StopAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	AUD_PLAY_MODE		stPlayMode  = {0, };
	AUD_DSP0_STATE		stDsp0State = {0, };
	AUD_MPB0_CONFIG0	stMPB0Cfg0  = {0, };

	//Stop decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
	{
		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		stPlayMode.play_mode = LX_ADEC_PLAY_STOP; 	//Stop decoding in DSP0

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));

		//Read DSP0 state reg.
		memcpy (&stDsp0State, (UINT32*) &g_pstADECReg_l8->aud_dsp0_state, sizeof (UINT32));

		//Set stop flag for stop decoding
		stDsp0State.stop_flag = 0x1; //Stop flag

		//Write DSP0 state reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_dsp0_state, &stDsp0State, sizeof (UINT32));
	}
	else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		stPlayMode.ad_play_mode = 0x0;	//Stop decoding AD decoding in DSP0

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF0)
	{
		//Read MPB0 config0 reg.
		memcpy (&stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

		/* Set a buffer state */
		stMPB0Cfg0.play_mode = 3;	//Stop mixing of PCM data in DSP1

		//Write MPB0 config0 reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));
	}
	else
	{
		ADEC_ERROR("STOP : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("ADEC_DRV_StopAudioClip : Stopped(adecMode = 0x%X, buffer = %d)...\n",	\
				i_eAdecMode, stPlayMode.buffer_state);
	ADEC_DEBUG("Clip Stop  (Mode = 0x%X, bstat = %d)\n", i_eAdecMode, stPlayMode.buffer_state);

	return RET_OK;
}


/**
 * Pauses a playing of the audio clip.
 * After calling this IOCTL function, resume must be enabled by the ADEC_IO_RESUME_AUDIO_CLIP IOCTL function.
 * @see ADEC_DRV_StopAudioClip
*/
int ADEC_L8_PauseAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	AUD_PLAY_MODE		stPlayMode;
	AUD_MPB0_CONFIG0	stMPB0Cfg0;

	//Pause decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
	{
		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		stPlayMode.play_mode = 0x2; 	//Pause decoding in DSP0

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		stPlayMode.ad_play_mode = 0x2;	//Pause decoding AD decoding in DSP0

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF0)
	{
		//Read MPB0 config0 reg.
		memcpy ( &stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

		/* Set a buffer state */
		stMPB0Cfg0.play_mode = 2;	//Pause mixing of PCM data in DSP1

		//Write MPB0 config0 reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));
	}
	else
	{
		ADEC_ERROR("PAUSE : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("Audio clip play paused(adecMode = 0x%X)...\n", i_eAdecMode);
	ADEC_DEBUG("Clip Pause (Mode = 0x%X)\n", i_eAdecMode);

	return RET_OK;
}


/**
 * Resumes a paused audio clip playing.
 * After calling this IOCTL function, a audio clip is played at the paused position.
 * @see ADEC_DRV_PauseAudioClip
*/
int ADEC_L8_ResumeAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	AUD_PLAY_MODE		stPlayMode;
	AUD_MPB0_CONFIG0	stMPB0Cfg0;

	//Re-start decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
	{
		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		stPlayMode.play_mode = 0x1; 	//Start decoding in DSP0

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		stPlayMode.ad_play_mode = 0x1;	//Start decoding AD decoding in DSP0

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF0)
	{
		//Read MPB0 config0 reg.
		memcpy (&stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

		/* Set a buffer state */
		stMPB0Cfg0.play_mode = 1;	//Start mixing of PCM data in DSP1

		//Write MPB0 config0 reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));
	}
	else
	{
		ADEC_ERROR("RESUME : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("Audio clip play resumed(adecMode = 0x%X)...\n", i_eAdecMode);
	ADEC_DEBUG("Clip Resume(Mode = 0x%X)\n", i_eAdecMode);

	return RET_OK;
}


/**
 * Deletes a audio clip data.
 * This function frees the memory of the current audio clip buffer and resets the related global variables.
 * @see ADEC_DRV_ResumeAudioClip
*/
int ADEC_L8_DeleteAudioClip ( LX_ADEC_CLIP_DEL_INFO_T stClipDeleteInfo )
{
	AUD_PLAY_MODE		stPlayMode;
	AUD_MPB0_CONFIG0	stMPB0Cfg0;

	/* Set write pointer for clip play */
	if(stClipDeleteInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		/* Clear a PESM / CPBM buffer */
		g_pstADECReg_l8->aud_pesm_wptr0 = 0x0;
		g_pstADECReg_l8->aud_pesm_rptr0 = 0x0;

		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		/* Clear buffer state */
		stPlayMode.buffer_state = LX_ADEC_CLIP_BUF_NONE;

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(stClipDeleteInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		/* Clear a PESA / CPBA buffer */
		g_pstADECReg_l8->aud_pesa_wptr0 = 0x0;
		g_pstADECReg_l8->aud_pesa_rptr0 = 0x0;

		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		/* Clear buffer state */
		stPlayMode.buffer_state = LX_ADEC_CLIP_BUF_NONE;

		//Write play mode reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));
	}
	else if(stClipDeleteInfo.adecMode == LX_ADEC_MIX_BUF0)
	{
		/* Clear a MPB0 buffer */
		g_pstADECReg_l8->aud_mpb0_wptr  = 0x0;
		g_pstADECReg_l8->aud_mpb0_rptr  = 0x0;

		//Read MPB0 config0 reg.
		memcpy (&stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

		/* Clear buffer state */
		stMPB0Cfg0.buffer_state = LX_ADEC_CLIP_BUF_NONE;

		//Write MPB0 config0 reg.
		memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));
	}
	else
	{
		ADEC_ERROR("ADEC mode is not defined(adecMode = 0x%X)!!!\n", stClipDeleteInfo.adecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("Audio clip play deleted(adecMode = 0x%X)!!!\n", stClipDeleteInfo.adecMode);
	ADEC_DEBUG("Clip Delete(Mode = 0x%X)\n", stClipDeleteInfo.adecMode);

	return RET_OK;
}

/**
 * Starts a playing of the aac audio clip.
 * This function starts audio dsp only for aac audio clip after aac profile and format is parsed in kadp function.
 * @see ADEC_DRV_LoadAudioClip
*/
int ADEC_L8_StartAacAudioClip ( LX_ADEC_AAC_START_INFO_T stAacStartInfo )
{
	/* Set parameter for clip play */
	if(stAacStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		//Change default profile if not defined
		if(stAacStartInfo.aacCodec == LX_AAC_CODEC_TYPE_NOT_DEF)
		{
			stAacStartInfo.aacCodec = LX_AAC_CODEC_TYPE_AAC;
		}

		AUD_L8_RdFL(aud_deccfg0);
		/* AAC profile format
			0 = AAC
			1 = HE-AAC
			2 = HE-AAC v2
			3 ~ 15 = Reserved	*/
		AUD_L8_Wr01(aud_deccfg0, aac_pf_format, (stAacStartInfo.aacCodec - 1));

		/* AAC bitstream format
			0 = RAW
			1 = ADTS
			2 = ADIF
			3 = LOAS
			4 ~ 8 = Reserved */
		AUD_L8_Wr01(aud_deccfg0, aac_bs_format, stAacStartInfo.aacFormat);
		AUD_L8_WrFL(aud_deccfg0);

		ADEC_ERROR("Clip AAC profile = %d, format = %d\n",	(stAacStartInfo.aacCodec - 1), stAacStartInfo.aacFormat);

		AUD_L8_RdFL(aud_play_mode);
		AUD_L8_Wr01(aud_play_mode, play_mode, LX_ADEC_PLAY_START);
		AUD_L8_WrFL(aud_play_mode);
	}
	else if(stAacStartInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		AUD_L8_RdFL(aud_deccfg0);
		/* AAC profile format
			0 = AAC
			1 = HE-AAC
			2 = HE-AAC v2
			3 ~ 15 = Reserved	*/
		AUD_L8_Wr01(aud_deccfg0, aac_pf_format, (stAacStartInfo.aacCodec - 1));

		/* AAC bitstream format
			0 = RAW
			1 = ADTS
			2 = ADIF
			3 = LOAS
			4 ~ 8 = Reserved */
		AUD_L8_Wr01(aud_deccfg0, aac_bs_format, stAacStartInfo.aacFormat);
		AUD_L8_WrFL(aud_deccfg0);

		ADEC_ERROR("Clip AAC profile = %d, format = %d\n",	(stAacStartInfo.aacCodec - 1), stAacStartInfo.aacFormat);

		AUD_L8_RdFL(aud_play_mode);
		AUD_L8_Wr01(aud_play_mode, ad_play_mode, LX_ADEC_PLAY_START);
		AUD_L8_WrFL(aud_play_mode);
	}
	else
	{
		ADEC_ERROR("PLAY : mode is not defined(adecMode = 0x%X)!!!\n", stAacStartInfo.adecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("Audio clip AAC play started(adecMode = 0x%X)...\n", stAacStartInfo.adecMode);
	ADEC_DEBUG("Audio clip AAC play started(adecMode = 0x%X)...\n", stAacStartInfo.adecMode);

	return RET_OK;
}

/**
 * Read write to registers.
 * This function is used for verification.
 * @see
*/
int ADEC_L8_ReadAndWriteReg ( LX_ADEC_REG_INFO_T *pstRegInfo )
{
	switch (pstRegInfo->mode)
	{
		case LX_ADEC_REG_READ:
		{
			memcpy ( &pstRegInfo->readdata, (UINT32*)(((UINT32)g_pstADECReg_l8) + pstRegInfo->addr), sizeof (UINT32));
			ADEC_PRINT ("<<< ADEC_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_ADEC_REG_WRITE:
		{
			ADEC_PRINT (">>> ADEC_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			memcpy ((UINT32*)(((UINT32)g_pstADECReg_l8) + pstRegInfo->addr), (UINT32*)&pstRegInfo->writedata, sizeof (UINT32));
		}
		break;

		case LX_ADEC_REG_WRNRD:
		{
			ADEC_PRINT (">>> ADEC_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			memcpy ((UINT32*)(((UINT32)g_pstADECReg_l8) + pstRegInfo->addr), (UINT32*)&pstRegInfo->writedata, sizeof (UINT32));

			memcpy ( (UINT32*)&pstRegInfo->readdata, (UINT32*)(((UINT32)g_pstADECReg_l8) + pstRegInfo->addr), sizeof (UINT32));
			ADEC_PRINT ("<<< ADEC_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_ADEC_REG_GET_MAX_ADDR:
		{
			pstRegInfo->readdata = sizeof (AUD_REG_L8_T);
			ADEC_PRINT ("<<< LX_ADEC_REG_GET_MAX_ADDR	addr = 0x%2X\n", pstRegInfo->readdata );
		}
		break;


		case LX_ADEC_REG_READ_ARRAY:
		case LX_ADEC_REG_READ_ALL:
			return RET_ERROR;
			break;


		case LX_CTOP_REG_READ:
		{
			pstRegInfo->readdata = CTOP_CTRL_READ(pstRegInfo->addr);
			ADEC_PRINT ("<<< CTOP_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_CTOP_REG_WRITE:
		{
			CTOP_CTRL_WRITE(pstRegInfo->addr, pstRegInfo->writedata);
			ADEC_PRINT (">>> CTOP_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
		}
		break;

		case LX_CTOP_REG_WRNRD:
		{
			ADEC_PRINT (">>> CTOP_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			CTOP_CTRL_WRITE(pstRegInfo->addr, pstRegInfo->writedata);

			pstRegInfo->readdata = CTOP_CTRL_READ(pstRegInfo->addr);
			ADEC_PRINT ("<<< CTOP_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_CTOP_REG_GET_MAX_ADDR:
		{
			if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
				pstRegInfo->readdata = sizeof (CTOP_CTRL_REG_B0_T);
			else
				pstRegInfo->readdata = sizeof (CTOP_CTRL_REG_A0_T);

			ADEC_PRINT ("<<< LX_CTOP_REG_GET_MAX_ADDR	addr = 0x%2X\n", pstRegInfo->readdata );
		}
		break;

		case LX_CTOP_REG_READ_ARRAY:
		case LX_CTOP_REG_READ_ALL:
		case LX_ANALOG_REG_READ:
		case LX_ANALOG_REG_WRITE:
		case LX_ANALOG_REG_WRNRD:
		default:
			return RET_ERROR;
			break;
	}

	return RET_OK;
}

int ADEC_GetCodecSrcType ( LX_ADEC_SRC_TYPE_T *peSrcType )
{
	int eRetVal = RET_OK;

	switch (g_pstADECReg_l8->aud_dsp0_state.codec_type)
	{
		case 0x0:
			ADEC_PRINT("ADEC_GetCodecSrcType : Unknown\n");
			*peSrcType = LX_ADEC_SRC_TYPE_UNKNOWN;
			break;

		case 0x1:
			ADEC_PRINT("ADEC_GetCodecSrcType : PCM\n");
			*peSrcType = LX_ADEC_SRC_TYPE_PCM;
			break;

		case 0x2:
			ADEC_PRINT("ADEC_GetCodecSrcType : MS10_DDC\n");
			*peSrcType = LX_ADEC_SRC_TYPE_MS10_DDC;
			break;

		case 0x3:
			ADEC_PRINT("ADEC_GetCodecSrcType : MS10_DDT\n");
			*peSrcType = LX_ADEC_SRC_TYPE_MS10_DDT;
			break;

		case 0x4:
			ADEC_PRINT("ADEC_GetCodecSrcType : MPEG\n");
			*peSrcType = LX_ADEC_SRC_TYPE_MPEG;
			break;

		case 0x5:
			ADEC_PRINT("ADEC_GetCodecSrcType : MP3\n");
			*peSrcType = LX_ADEC_SRC_TYPE_MP3;
			break;

		case 0x6:
			ADEC_PRINT("ADEC_GetCodecSrcType : WMA\n");
			*peSrcType = LX_ADEC_SRC_TYPE_WMA;
			break;

		case 0x7:
			ADEC_PRINT("ADEC_GetCodecSrcType : DTS\n");
			*peSrcType = LX_ADEC_SRC_TYPE_DTS;
			break;

		case 0x8:
			ADEC_PRINT("ADEC_GetCodecSrcType : SILK(Skype)\n");
			eRetVal = RET_ERROR;
			break;

		case 0x9:
			ADEC_PRINT("ADEC_GetCodecSrcType : G729\n");
			*peSrcType = LX_ADEC_SRC_TYPE_G729;
			break;

		case 0xA:
			ADEC_PRINT("ADEC_GetCodecSrcType : G711\n");
			eRetVal = RET_ERROR;
			break;

		case 0xB:
			ADEC_PRINT("ADEC_GetCodecSrcType : DTSBPS\n");
			*peSrcType = LX_ADEC_SRC_TYPE_DTSBPS;
			break;

		case 0xC:
			ADEC_PRINT("ADEC_GetCodecSrcType : PCMWAV\n");
			*peSrcType = LX_ADEC_SRC_TYPE_PCMWAV;
			break;

		case 0xD:
			ADEC_PRINT("ADEC_GetCodecSrcType : NONE\n");
			*peSrcType = LX_ADEC_SRC_TYPE_NONE;
			break;

		default:
			ADEC_ERROR("ADEC_GetCodecSrcType [0x%x] : Type Not Matched\n", g_pstADECReg_l8->aud_dsp0_state.codec_type );
			eRetVal = RET_ERROR;
			break;
	}

	return eRetVal;
}

int ADEC_LoadCodec ( LX_ADEC_SRC_TYPE_T i_eSrcType )
{
	UINT32		ui32CodecSize;
	UINT32		*pui32Codec;
	UINT32		ui32InputPort;

	UINT8		codecName[ADEC_CODEC_NAME_SIZE];	//30

	UINT64		start, elapsed;

	/* Mapping codec from ddi to DSP0 codec */
	switch (i_eSrcType)
	{
		case LX_ADEC_SRC_TYPE_UNKNOWN:
			return RET_OK;
			break;

		case LX_ADEC_SRC_TYPE_SIF:
		case LX_ADEC_SRC_TYPE_SIF_BTSC:
		case LX_ADEC_SRC_TYPE_SIF_A2:
		case LX_ADEC_SRC_TYPE_PCM:
			i_eSrcType = LX_ADEC_SRC_TYPE_PCM;
			break;

		case LX_ADEC_SRC_TYPE_AC3:
		case LX_ADEC_SRC_TYPE_EAC3:
		case LX_ADEC_SRC_TYPE_MS10_DDC:
			i_eSrcType = LX_ADEC_SRC_TYPE_MS10_DDC;
			break;

		case LX_ADEC_SRC_TYPE_AAC:
		case LX_ADEC_SRC_TYPE_HEAAC:
		case LX_ADEC_SRC_TYPE_MS10_DDT:
			i_eSrcType = LX_ADEC_SRC_TYPE_MS10_DDT;
			break;

		case LX_ADEC_SRC_TYPE_MPEG:
		case LX_ADEC_SRC_TYPE_MP3:
			i_eSrcType = LX_ADEC_SRC_TYPE_MPEG;
			break;

		case LX_ADEC_SRC_TYPE_WMA:
			break;

		case LX_ADEC_SRC_TYPE_DTS:
			break;

		case LX_ADEC_SRC_TYPE_DEFAULT:
		case LX_ADEC_SRC_TYPE_CDLPCM:
		case LX_ADEC_SRC_TYPE_RA8LBR:
		case LX_ADEC_SRC_TYPE_XPCM:
		case LX_ADEC_SRC_TYPE_MPEG_EN:
		case LX_ADEC_SRC_TYPE_WMA_PRO:
		case LX_ADEC_SRC_TYPE_CV3:
			break;

		case LX_ADEC_SRC_TYPE_G729:
			break;

		case LX_ADEC_SRC_TYPE_DTSBPS:
			break;

		case LX_ADEC_SRC_TYPE_PCMWAV:
			break;

		case LX_ADEC_SRC_TYPE_NONE:
			break;

#ifdef ENABLE_MS10_DDC_CERTI
		case LX_ADEC_SRC_TYPE_MS10_DDC_CERTI:
			break;
#endif

#ifdef ENABLE_DTS_CERTI
		case LX_ADEC_SRC_TYPE_DTS_CERTI_DEC:
			break;

		case LX_ADEC_SRC_TYPE_DTS_CERTI_ENC:
			break;
#endif

		default:
			break;
	}

#if 1	//temp for audio fw team
	if (g_eCurLoadedCodec == i_eSrcType)
	{
		ADEC_PRINT("ADEC_LoadCodec : Codec already loaded\n");
		return RET_OK;
	}
#endif

#ifndef USE_QEMU_SYSTEM
	if (NULL == g_pui32DSP1CodecVirAddr)
	{
		ADEC_ERROR("ADEC_LoadCodec : Err - g_pui32DSP1CodecVirAddr NULL!\n");
		return RET_ERROR;
	}

	if (NULL == g_pui32DSP0CodecVirAddr)
	{
		ADEC_ERROR("ADEC_LoadCodec : Err - g_pui32DSP0CodecVirAddr NULL!\n");
		return RET_ERROR;
	}
#endif

	//Reset Low : ADEC DSP1
	ADEC_ClearReset(ADEC_OPTION_DSP1);

	//Reset Low : ADEC DSP0
	ADEC_ClearReset(ADEC_OPTION_DSP0);

	//Set audio codec firmware image
	if (i_eSrcType == LX_ADEC_SRC_TYPE_MS10_DDC)
	{
		ui32CodecSize = sizeof(ms10_ddc_codec_l8);
		pui32Codec	  = (UINT32 *)ms10_ddc_codec_l8;
		strcpy(codecName, "ms10_ddc_codec_l8");
	}
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_MS10_DDT)
	{
		//Read Input port reg.
		AUD_L8_RdFL(aud_set_inout);
		AUD_L8_Rd01(aud_set_inout, in_port, ui32InputPort);
		
		if(ui32InputPort == 1)
		{
			ui32CodecSize = sizeof(ms10_ddt_tp_codec_l8);
			pui32Codec    = (UINT32 *)ms10_ddt_tp_codec_l8;
			strcpy(codecName, "ms10_ddt_tp_codec_l8");

			ADEC_ERROR ( "ADEC_LoadCodec : Done(%s)!!!\n", codecName );
		}
		else
		{
			ui32CodecSize = sizeof(ms10_ddt_codec_l8);
			pui32Codec    = (UINT32 *)ms10_ddt_codec_l8;
			strcpy(codecName, "ms10_ddt_codec_l8");

			ADEC_ERROR ( "ADEC_LoadCodec : Done(%s)!!!\n", codecName );
		}
	}
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_MPEG)
	{
		ui32CodecSize = sizeof(mp23_codec_l8);
		pui32Codec    = (UINT32 *)mp23_codec_l8;
		strcpy(codecName, "mp23_codec_l8");
	}
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_PCM)
	{
		ui32CodecSize = sizeof(pcm_codec_l8);
		pui32Codec    = (UINT32 *)pcm_codec_l8;
		strcpy(codecName, "pcm_codec_l8");
	}
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_WMA)
	{
		ui32CodecSize = sizeof(wma_codec_l8);
		pui32Codec	  = (UINT32 *)wma_codec_l8;
		strcpy(codecName, "wma_codec_l8");
	}
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_DTS)
	{
		ui32CodecSize = sizeof(dts_codec_l8);
		pui32Codec	  = (UINT32 *)dts_codec_l8;
		strcpy(codecName, "dts_codec_l8");
	}
#ifdef ENABLE_MS10_DDC_CERTI
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_MS10_DDC_CERTI)
	{
		ui32CodecSize = sizeof(ms10_ddc_certi_codec_l8);
		pui32Codec	  = (UINT32 *)ms10_ddc_certi_codec_l8;
		strcpy(codecName, "ms10_ddc_certi_codec_l8");
	}
#endif
#ifdef ENABLE_DTS_CERTI
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_DTS_CERTI_DEC)
	{
		ui32CodecSize = sizeof(dts_certi_dec_l8);
		pui32Codec	  = (UINT32 *)dts_certi_dec_l8;
		strcpy(codecName, "dts_certi_dec_l8");
	}
	else if (i_eSrcType == LX_ADEC_SRC_TYPE_DTS_CERTI_ENC)
	{
		ui32CodecSize = sizeof(dts_certi_enc_l8);
		pui32Codec	  = (UINT32 *)dts_certi_enc_l8;
		strcpy(codecName, "dts_certi_enc_l8");
	}
#endif	//#ifdef ENABLE_DTS_CERTI
	else
	{
		ADEC_ERROR("ADEC_LoadCodec : Err - Codec Not Defined!(0x%X)\n", i_eSrcType);
		return RET_INVALID_PARAMS;
	}

	//Get a current mili-second time
	start = jiffies;

	ADEC_PRINT ( "ADEC_LoadCodec : Started... srcType(%d), Codec(%s), Size(%d)\n",i_eSrcType, codecName, ui32CodecSize);

	if(ui32CodecSize > pstMemCfgADEC->fw0_memory_size)
	{
		ADEC_ERROR("ADEC_LoadCodec : %s size is bigber than memory.(%d > %d)\n",	\
					codecName, ui32CodecSize, pstMemCfgADEC->fw0_memory_size);
		return RET_ERROR;
	}

	//Copy codec fw from memory to dsp0 memory
	memcpy(g_pui32DSP0CodecVirAddr, pui32Codec, ui32CodecSize);

	ADEC_PRINT ( "ADEC_LoadCodec : Done(%s)!!!\n", codecName );
	ADEC_DEBUG_TMP ( "ADEC_LoadCodec : Done(%s), prev type = 0x%X!!!\n", codecName, g_pstADECReg_l8->aud_dsp0_state.codec_type );

	//Clear aud_dsp0_state reg.
	AUD_L8_RdFL(aud_dsp0_state);
	AUD_L8_Wr03(aud_dsp0_state, codec_ver, 0x0, codec_type, 0x0, stop_flag, 0x0);								\
	AUD_L8_WrFL(aud_dsp0_state);

	//Set DSP swreset register
	ADEC_SetReset(ADEC_OPTION_DSP0);

#ifndef USE_QEMU_SYSTEM
	//Wait for DSP fw download completion to DSP memory and DSP ready
	ADEC_WaitForDSP0Ready();
#endif /* !USE_QEMU_SYSTEM */

	//Get a elapsed nono-second time
	elapsed = jiffies - start;
	ADEC_DEBUG_TMP("DSP0 is ready in %d ms.\n", (UINT32)elapsed);

	//Update the current laoded codec variable
	g_eCurLoadedCodec = i_eSrcType;

	//Get a current mili-second time
	start = jiffies;

#if !defined(ENABLE_MS10_DDC_CERTI) && !defined(ENABLE_DTS_CERTI)
	//Load LGSE codec for DSP1
	ui32CodecSize = sizeof(se_codec_l8);
	pui32Codec	  = (UINT32 *)se_codec_l8;
	strcpy(codecName, "se_codec_l8");
#else
#ifdef ENABLE_MS10_DDC_CERTI
	if (i_eSrcType == LX_ADEC_SRC_TYPE_MS10_DDC_CERTI)
	{

		//Load MS10_DDC certi LGSE  codec for DSP1
		ui32CodecSize = sizeof(se_certi_ms10_ddc_codec_l8);
		pui32Codec	  = (UINT32 *)se_certi_ms10_ddc_codec_l8;
		strcpy(codecName, "se_certi_ms10_ddc_codec_l8");
	}
#endif
#ifdef ENABLE_DTS_CERTI
	if ( (i_eSrcType == LX_ADEC_SRC_TYPE_DTS_CERTI_DEC)
	   ||(i_eSrcType == LX_ADEC_SRC_TYPE_DTS_CERTI_ENC) )
	{
		//Load DTS certi LGSE  codec for DSP1
		ui32CodecSize = sizeof(se_certi_dts_l8);
		pui32Codec	  = (UINT32 *)se_certi_dts_l8;
		strcpy(codecName, "se_certi_dts_l8");
	}
#endif
	else
	{
		//Load LGSE codec for DSP1
		ui32CodecSize = sizeof(se_codec_l8);
		pui32Codec	  = (UINT32 *)se_codec_l8;
		strcpy(codecName, "se_codec_l8");
	}
#endif	//#if (defined !ENABLE_DTS_CERTI && defined !ENABLE_MS10_DDC_CERTI)

	ADEC_PRINT ( "ADEC_LoadCodec : Started... Codec(%s), Size(%d)\n", codecName, ui32CodecSize);

	if(ui32CodecSize > pstMemCfgADEC->fw1_memory_size)
	{
		ADEC_ERROR("ADEC_LoadCodec : %s size is bigber than memory.(%d > %d)\n",	\
					codecName, ui32CodecSize, pstMemCfgADEC->fw1_memory_size);
		return RET_ERROR;
	}

	//Copy codec fw from memory to dsp1 memory
	memcpy(g_pui32DSP1CodecVirAddr, pui32Codec, ui32CodecSize);
	ADEC_PRINT ( "ADEC_LoadCodec : Done(%s)!!!\n", codecName );

	//Clear aud_dsp1_state reg. for codec ver and DSP1 running location
	AUD_L8_RdFL(aud_dsp1_state);
	AUD_L8_Wr02(aud_dsp1_state, codec_ver, 0x0, run_location, 0x0);								\
	AUD_L8_WrFL(aud_dsp1_state);

	//Set DSP1 swreset register
	ADEC_SetReset(ADEC_OPTION_DSP1);

#ifndef USE_QEMU_SYSTEM
	//Wait for DSP1 fw download completion to DSP1 memory and DSP1 ready
	ADEC_WaitForDSP1Ready();
#endif /* !USE_QEMU_SYSTEM */

	//Get a elapsed nono-second time
	elapsed = jiffies - start;
	ADEC_DEBUG_TMP("DSP1 is ready in %d ms.\n", (UINT32)elapsed);

	return RET_OK;
}

int ADEC_SetReset ( UINT8 ui8ResetFlag )
{
	AUD_SWRESET		stSWReset = { 0 };

	memcpy ( &stSWReset, (UINT32*)&g_pstADECReg_l8->aud_swreset, sizeof (UINT32));

	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		//Clear interrupt status
		g_pstADECReg_l8->aud_int_state0 = 0;

		/* Reset ADEC_dsp0_ready_waitq_flag value */
		ADEC_dsp0_ready_waitq_flag  = 0;

		/* Set DSP0 state after first call */
		g_ui32ResetDsp0State		= 1;

		stSWReset.dsp0ocdresetout 	= 1;        //10	dsp0ocdresetout    	reset for On chip debugger of DSP0
		stSWReset.dsp0bresetout 	= 1;        //12	dsp0bresetout      	reset for DSP0
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		//Clear interrupt status
		g_pstADECReg_l8->aud_int_state1 = 0;

		/* Reset ADEC_dsp0_ready_waitq_flag value */
		ADEC_dsp1_ready_waitq_flag	= 0;

		/* Set DSP1 state after first call */
		g_ui32ResetDsp1State 		= 1;

		stSWReset.dsp1ocdresetout	= 1;        //11	dsp1ocdresetout    	reset for On chip debugger of DSP1
		stSWReset.dsp1bresetout 	= 1;        //13	dsp1bresetout      	reset for DSP1
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP_DEBUG)
	{
		stSWReset.dsp0ocdresetout 	= 1;        //10	dsp0ocdresetout    	reset for On chip debugger of DSP0
		stSWReset.dsp1ocdresetout	= 1;        //11	dsp1ocdresetout    	reset for On chip debugger of DSP1
	}
	else if (ui8ResetFlag == ADEC_OPTION_AAD)
	{
		stSWReset.aadresetout 		= 1;		//14 	aadresetout        	reset for operation of Analog audio decoder
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		stSWReset.fs21resetout 		= 1;		//5		fs21resetout       	reset for audio SPDIF output interface
	}
	else if (ui8ResetFlag == ADEC_OPTION_MIXEDIP)
	{
		//Mixed IP enable only in operation
		stSWReset.mipdacresetout 	= 1;		//16 	mipdacresetout     	reset for audio DAC
		stSWReset.mipadcresetout 	= 1;		//17 	mipadcresetout     	reset for audio ADC
		stSWReset.mipmicresetout 	= 1;		//18 	mipmicresetout     	reset for audio Mic ADC
		stSWReset.miphpdrvresetout 	= 1;		//19 	miphpdrvresetout   	reset for audio HP driver
		stSWReset.miphbdrvcntresetout=1;		//20 	miphbdrvcntresetout	reset for audio HP driver clock counter
	}
	else if (ui8ResetFlag == ADEC_OPTION_DAC)
	{
		//Mixed IP enable only in operation
		stSWReset.mipdacresetout 	= 1;		//16 	mipdacresetout     	reset for audio DAC
	}
	else if (ui8ResetFlag == ADEC_OPTION_ADC)
	{
		//Mixed IP enable only in operation
		stSWReset.mipadcresetout 	= 1;		//17 	mipadcresetout     	reset for audio ADC
		stSWReset.mipmicresetout 	= 1;		//18 	mipmicresetout     	reset for audio Mic ADC
	}
	else if (ui8ResetFlag == ADEC_OPTION_HPDRV)
	{
		//Mixed IP enable only in operation
		stSWReset.miphpdrvresetout 	= 1;		//19 	miphpdrvresetout   	reset for audio HP driver
		stSWReset.miphbdrvcntresetout=1;		//20 	miphbdrvcntresetout	reset for audio HP driver clock counter
	}
	else if (ui8ResetFlag == ADEC_OPTION_OTHERS)
	{
		stSWReset.aresetout 		= 1;		//0	 	aresetout          	reset for operation of peripherals
		stSWReset.apbresetout 		= 1;		//1	 	apbresetout        	reset for register interface if this set, all registers are cleared
		stSWReset.aversetout 		= 1;		//2  	aversetout         	reset for TE data input interface,
		stSWReset.veresetout 		= 0;        //3  	veresetout         	reset for TE data output interface (encoder)
		stSWReset.fs20resetout 		= 1;		//4	 	fs20resetout       	reset for audio PCM output interface
		stSWReset.fs21resetout 		= 1;		//5		fs21resetout       	reset for audio SPDIF output interface
												//6	                    	reserved
		stSWReset.fs23resetout 		= 0;		//7	 	fs23resetout       	reset for audio HWSRC data input
		stSWReset.fs24resetout 		= 0;		//8	 	fs24resetout       	reset for audio HWSRC datat output
		stSWReset.asrcrstout 		= 0;		//9	 	asrcrstout         	reset for audio HWSRC operation

		stSWReset.aadresetout 		= 1;		//14 	aadresetout        	reest for operation of Analog audio decoder
		stSWReset.aadapbresetout 	= 1;		//15 	aadapbresetout     	reset for AAD register interface

		if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC )
			stSWReset.mipdacresetout= 0;		//16	mipdacresetout	reset for audio DAC
		else if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
			stSWReset.mipdacresetout= 1;		//16	mipdacresetout	reset for audio DAC
		else
			stSWReset.mipdacresetout= 1;		//16	mipdacresetout	reset for audio DAC

		stSWReset.mipadcresetout 	= 1;		//17 	mipadcresetout     	reset for audio ADC
		stSWReset.mipmicresetout 	= 0;		//18 	mipmicresetout     	reset for audio Mic ADC
		stSWReset.miphpdrvresetout 	= 1;		//19 	miphpdrvresetout   	reset for audio HP driver
		stSWReset.miphbdrvcntresetout=1;		//20 	miphbdrvcntresetout	reset for audio HP driver clock counter
	}
	else if (ui8ResetFlag == ADEC_OPTION_ALL)
	{
		stSWReset.aresetout 		= 1;		//0 	aresetout			reset for operation of peripherals
		stSWReset.apbresetout		= 1;		//1 	apbresetout 		reset for register interface if this set, all registers are cleared
		stSWReset.aversetout		= 1;		//2 	aversetout			reset for TE data input interface,
		stSWReset.veresetout		= 1;		//3 	veresetout			reset for TE data output interface (encoder)
		stSWReset.fs20resetout		= 1;		//4 	fs20resetout		reset for audio PCM output interface
		stSWReset.fs21resetout		= 1;		//5 	fs21resetout		reset for audio SPDIF output interface
												//6 						reserved
		stSWReset.fs23resetout		= 1;		//7 	fs23resetout		reset for audio HWSRC data input
		stSWReset.fs24resetout		= 1;		//8 	fs24resetout		reset for audio HWSRC datat output
		stSWReset.asrcrstout		= 1;		//9 	asrcrstout			reset for audio HWSRC operation

		stSWReset.dsp0ocdresetout 	= 1;        //10	dsp0ocdresetout    	reset for On chip debugger of DSP0
		stSWReset.dsp1ocdresetout	= 1;        //11	dsp1ocdresetout    	reset for On chip debugger of DSP1
		stSWReset.dsp0bresetout 	= 1;        //12	dsp0bresetout      	reset for DSP0
		stSWReset.dsp1bresetout 	= 1;        //13	dsp1bresetout      	reset for DSP1

		stSWReset.aadresetout		= 1;		//14	aadresetout 		reest for operation of Analog audio decoder
		stSWReset.aadapbresetout	= 1;		//15	aadapbresetout		reset for AAD register interface

		stSWReset.mipdacresetout	= 1;		//16	mipdacresetout		reset for audio DAC
		stSWReset.mipadcresetout	= 1;		//17	mipadcresetout		reset for audio ADC
		stSWReset.mipmicresetout	= 1;		//18	mipmicresetout		reset for audio Mic ADC
		stSWReset.miphpdrvresetout	= 1;		//19	miphpdrvresetout	reset for audio HP driver
		stSWReset.miphbdrvcntresetout=1;		//20	miphbdrvcntresetout reset for audio HP driver clock counter
	}

	memcpy ((UINT32*) &g_pstADECReg_l8->aud_swreset, &stSWReset, sizeof (UINT32));
	ADEC_PRINT ("Reset reg val [0x%x]\n", *(UINT32*)&g_pstADECReg_l8->aud_swreset );

	//Set delay for reset high duration for stable operation
	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_DSP0!!!\n");

		//Enable audio interrupt 0
		g_pstADECReg_l8->aud_int0_en = 0x1;
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_DSP1!!!\n");

		//Enable audio interrupt 1
		g_pstADECReg_l8->aud_int1_en = 0x1;
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP_DEBUG)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_DSP_DEBUG!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_AAD)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_AAD!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		msleep_interruptible(2);	//SPDIF.
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_SPDIF!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_MIXEDIP)
	{
		msleep_interruptible(20);	//Mixed IP Spec. says 7.3ms in L8 A1.
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_MIXEDIP!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_DAC)
	{
		udelay(100);
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_DAC!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_ADC)
	{
		udelay(100);
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_ADC!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_HPDRV)
	{
		udelay(100);
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_HPDRV!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_OTHERS)
	{
		msleep_interruptible(20);	//Mixed IP Spec. says 7.3ms in L8 A1.
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_OTHERS!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_ALL)
	{
		msleep_interruptible(20);	//Mixed IP Spec. says 7.3ms in L8 A1.
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_ALL!!!\n");
	}

	return RET_OK;
}

int ADEC_ClearReset ( UINT8 ui8ResetFlag )
{
	AUD_SWRESET		stSWReset = { 0 };
	UINT32			flushDoneStatus, timeout;

	memcpy ( &stSWReset, (UINT32*) &g_pstADECReg_l8->aud_swreset, sizeof (UINT32));

	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		//Disable audio interrupt DSP0
		g_pstADECReg_l8->aud_int0_en = 0x0;

		stSWReset.dsp0ocdresetout 	= 0;        //10 dsp0ocdresetout    	reset for On chip debugger of DSP0
		stSWReset.dsp0bresetout 	= 0;        //12 dsp0bresetout      	reset for DSP0
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		//Disable audio interrupt DSP1
		g_pstADECReg_l8->aud_int1_en = 0x0;

		stSWReset.dsp1ocdresetout 	= 0;        //11 dsp1ocdresetout    	reset for On chip debugger of DSP1
		stSWReset.dsp1bresetout 	= 0;        //13 dsp1bresetout      	reset for DSP1
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP_DEBUG)
	{
		stSWReset.dsp0ocdresetout 	= 0;        //10	dsp0ocdresetout    	reset for On chip debugger of DSP0
		stSWReset.dsp1ocdresetout	= 0;        //11	dsp1ocdresetout    	reset for On chip debugger of DSP1
	}
	else if (ui8ResetFlag == ADEC_OPTION_AAD)
	{
		stSWReset.aadresetout 		= 0;		//14 aadresetout        	reest for operation of Analog audio decoder
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		stSWReset.fs21resetout 		= 0;		//5	 fs21resetout      		reset for audio SPDIF output interface
	}
	else if (ui8ResetFlag == ADEC_OPTION_MIXEDIP)
	{
		stSWReset.mipdacresetout 	= 0;		//16 mipdacresetout     	reset for audio DAC
		stSWReset.mipadcresetout 	= 0;		//17 mipadcresetout     	reset for audio ADC
		stSWReset.mipmicresetout 	= 0;		//18 mipmicresetout     	reset for audio Mic ADC
		stSWReset.miphpdrvresetout 	= 0;		//19 miphpdrvresetout   	reset for audio HP driver
		stSWReset.miphbdrvcntresetout=0;		//20 miphbdrvcntresetout	reset for audio HP driver clock counter
	}
	else if (ui8ResetFlag == ADEC_OPTION_DAC)
	{
		stSWReset.mipdacresetout 	= 0;		//16 	mipdacresetout     	reset for audio DAC
	}
	else if (ui8ResetFlag == ADEC_OPTION_ADC)
	{
		stSWReset.mipadcresetout 	= 0;		//17 	mipadcresetout     	reset for audio ADC
		stSWReset.mipmicresetout 	= 0;		//18 	mipmicresetout     	reset for audio Mic ADC
	}
	else if (ui8ResetFlag == ADEC_OPTION_HPDRV)
	{
		stSWReset.miphpdrvresetout 	= 0;		//19 	miphpdrvresetout   	reset for audio HP driver
		stSWReset.miphbdrvcntresetout=0;		//20 	miphbdrvcntresetout	reset for audio HP driver clock counter
	}
	else if (ui8ResetFlag == ADEC_OPTION_OTHERS)
	{
		stSWReset.aresetout 		= 0;		//0	 aresetout          	reset for operation of peripherals
		stSWReset.apbresetout 		= 0;		//1	 apbresetout        	reset for register interface if this set, all registers are cleared
		stSWReset.aversetout 		= 0;		//2  aversetout         	reset for TE data input interface,
		stSWReset.veresetout 		= 0;        //3	 veresetout         	reset for TE data output interface (encoder)

		stSWReset.fs20resetout 		= 0;		//4	 fs20resetout       	reset for audio PCM output interface
		stSWReset.fs21resetout 		= 0;		//5	 fs21resetout      		reset for audio SPDIF output interface
												//6	                    	reserved
		stSWReset.fs23resetout 		= 0;		//7	 fs23resetout       	reset for audio HWSRC data input
		stSWReset.fs24resetout 		= 0;		//8	 fs24resetout       	reset for audio HWSRC datat output
		stSWReset.asrcrstout 		= 0;		//9	 asrcrstout         	reset for audio HWSRC operation

		stSWReset.aadresetout 		= 0;		//14 aadresetout        	reest for operation of Analog audio decoder
		stSWReset.aadapbresetout 	= 0;		//15 aadapbresetout     	reset for AAD register interface

		stSWReset.mipdacresetout	= 0;		//16 mipdacresetout 		reset for audio DAC
		stSWReset.mipadcresetout	= 0;		//17 mipadcresetout 		reset for audio ADC
		stSWReset.mipmicresetout	= 0;		//18 mipmicresetout 		reset for audio Mic ADC
		stSWReset.miphpdrvresetout	= 0;		//19 miphpdrvresetout		reset for audio HP driver
		stSWReset.miphbdrvcntresetout=0;		//20 miphbdrvcntresetout	reset for audio HP driver clock counter
	}
	else if (ui8ResetFlag == ADEC_OPTION_ALL)
	{
		memset ( &stSWReset, 0, sizeof (UINT32));
	}

	//Flush ADEC DSP memory access
	if (g_ui32ResetDsp0State == 1 && ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		g_ui32ResetDsp0State = 0;

		//Stop ADEC DSP 0
		g_pstADECReg_l8->aud_runstall0.aud_runstall0 = 0x1;

		//Set flush request register for ADEC DSP0
		CTOP_CTRL_RdFL(flush_req);
		CTOP_CTRL_Wr01(flush_req, aud_dsp0_flush_en, 0x1);
		CTOP_CTRL_WrFL(flush_req);

		//Check flush done for DSP0
		for(timeout = 0; timeout < ADEC_DSP_FLUSH_ITERATION; timeout++)
		{
			udelay(1);

			CTOP_CTRL_RdFL(flush_done_status);
			CTOP_CTRL_Rd01(flush_done_status, aud_dsp0_flush_done, flushDoneStatus);

			if(flushDoneStatus)
				break;
			else
				ADEC_DEBUG ("Wait for ADEC DSP0 Memory Flush!!!\n");
		}

		//Check if timeout is reached
		if(timeout == ADEC_DSP_FLUSH_ITERATION)
		{
			ADEC_DEBUG ("CPU_TOP_MAU_FLUSH_DONE_REG : DSP0 ERROR!!!\n");
		}
	}
	else if (g_ui32ResetDsp1State == 1 && ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		g_ui32ResetDsp1State	= 0;

		//Stop ADEC DSP 1
		g_pstADECReg_l8->aud_runstall1.aud_runstall1 = 0x1;

		//Set flush request register for ADEC DSP1
		CTOP_CTRL_RdFL(flush_req);
		CTOP_CTRL_Wr01(flush_req, aud_dsp1_flush_en, 0x1);
		CTOP_CTRL_WrFL(flush_req);

		//Check flush done for DSP1
		for(timeout = 0; timeout < ADEC_DSP_FLUSH_ITERATION; timeout++)
		{
			udelay(1);

			CTOP_CTRL_RdFL(flush_done_status);
			CTOP_CTRL_Rd01(flush_done_status, aud_dsp1_flush_done, flushDoneStatus);

			if(flushDoneStatus)
				break;
			else
				ADEC_DEBUG ("Wait for ADEC DSP1 Memory Flush!!!\n");
		}

		//Check if timeout is reached
		if(timeout == ADEC_DSP_FLUSH_ITERATION)
		{
			ADEC_DEBUG ("CPU_TOP_MAU_FLUSH_DONE_REG : DSP1 ERROR!!!\n");
		}
	}

	memcpy ((UINT32*) &g_pstADECReg_l8->aud_swreset, &stSWReset, sizeof (UINT32));
	ADEC_PRINT ("Reset reg val [0x%x]\n", *(UINT32*)&g_pstADECReg_l8->aud_swreset );

	//Set delay for reset low duration
	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_DSP0!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_DSP1!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP_DEBUG)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_DSP_DEBUG!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_AAD)
	{
		udelay(2);					//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_AAD!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		msleep_interruptible(20);	//SPDIF.
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_SPDIF!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_MIXEDIP)
	{
		msleep_interruptible(6);	//Mixed IP Spec. says 6ms in L8 B0.
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_MIXEDIP!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_DAC)
	{
		udelay(100);
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_DAC!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_ADC)
	{
		udelay(100);
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_ADC!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_HPDRV)
	{
		udelay(100);
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_HPDRV!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_OTHERS)
	{
		msleep_interruptible(6);	//Mixed IP Spec. says 6ms in L8 B0.
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_OTHERS!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_ALL)
	{
		msleep_interruptible(6);	//Mixed IP Spec. says 2ms in L8 B0.
		ADEC_PRINT ("ADEC_ClearReset : ADEC_OPTION_ALL!!!\n");
	}

	//Reset Flush Bit for ADEC DSP memory access
	if (g_ui32ResetDsp0State == 0 && ui8ResetFlag == ADEC_OPTION_DSP0 )
	{
		//Clear flush request register for ADEC DSP0
		CTOP_CTRL_RdFL(flush_req);
		CTOP_CTRL_Wr01(flush_req, aud_dsp0_flush_en, 0x0);
		CTOP_CTRL_WrFL(flush_req);
		ADEC_PRINT ("Reset ADEC DSP0 Memory Flush!!!\n");

		//Start ADEC DSP 0
		g_pstADECReg_l8->aud_runstall0.aud_runstall0 = 0x0;
	}
	else if (g_ui32ResetDsp1State == 0 && ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		//Clear flush request register for ADEC DSP1
		CTOP_CTRL_RdFL(flush_req);
		CTOP_CTRL_Wr01(flush_req, aud_dsp1_flush_en, 0x0);
		CTOP_CTRL_WrFL(flush_req);
		ADEC_PRINT ("Reset ADEC DSP1 Memory Flush!!!\n");

		//Start ADEC DSP 1
		g_pstADECReg_l8->aud_runstall1.aud_runstall1 = 0x0;
	}

	return RET_OK;
}

/**
 * Initialize register
 * @see ADEC_DRV_InitModule
*/

int ADEC_InitReg ( void )
{
	AUD_INTMASK			stIntMask;
	AUD_SET_INOUT 		stSetInOut;

	AUD_LIP_CFG			stLipCfg;
	AUD_MLIP_BOUND0		stMLipBound;
	AUD_ALIP_BOUND0		stALipBound;

#if 1
	UINT32			aud_esmsaddr0    = pstMemCfgADEC->cpbm_memory_base + MAX_SIZE_OF_CPBM_INDEX_TABLE - pstMemCfgADEC->dsp0_memory_base;
	UINT32			aud_esmsize0	 = pstMemCfgADEC->cpbm_memory_size - MAX_SIZE_OF_CPBM_INDEX_TABLE;
	UINT32			aud_esasaddr0    = pstMemCfgADEC->cpba_memory_base - pstMemCfgADEC->dsp0_memory_base;
	UINT32			aud_esasize0	 = pstMemCfgADEC->cpba_memory_size;
	UINT32			aud_esidxtbl	 = pstMemCfgADEC->cpbm_memory_base - pstMemCfgADEC->dsp0_memory_base;
	UINT32			aud_esidxsize	 = MAX_SIZE_OF_CPBM_INDEX_TABLE;

	UINT32			aud_se_paramaddr = pstMemCfgADEC->dpbm_memory_base - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_se_paramsize = MAX_SIZE_OF_LGSE_PARAM;
	UINT32			aud_dpbm_saddr   = pstMemCfgADEC->dpbm_memory_base + MAX_SIZE_OF_LGSE_PARAM - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_dpbm_size    = pstMemCfgADEC->dpbm_memory_size - MAX_SIZE_OF_LGSE_PARAM;
	UINT32			aud_dpba_saddr   = pstMemCfgADEC->dpba_memory_base - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_dpba_size    = pstMemCfgADEC->dpba_memory_size;
	UINT32			aud_mpb0_saddr	 = pstMemCfgADEC->mpb0_memory_base - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_mpb0_size	 = pstMemCfgADEC->mpb0_memory_size;
	UINT32			aud_mpb1_saddr	 = pstMemCfgADEC->mpb1_memory_base - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_mpb1_size	 = pstMemCfgADEC->mpb1_memory_size;
	UINT32			aud_mpb2_saddr	 = pstMemCfgADEC->mpb2_memory_base - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_mpb2_size	 = pstMemCfgADEC->mpb2_memory_size;
	UINT32			aud_mpb3_saddr	 = pstMemCfgADEC->mpb3_memory_base - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_mpb3_size	 = pstMemCfgADEC->mpb3_memory_size;
	UINT32			aud_inter_saddr	 = pstMemCfgADEC->inter_memory_base - pstMemCfgADEC->dsp1_memory_base;
	UINT32			aud_inter_size	 = pstMemCfgADEC->inter_memory_size;
#endif

	UINT32 		loop = 0;
	UINT64		start, elapsed;

	//Get a current mili-second time
	start = jiffies;

	//This code must be run to clear register.
	ADEC_DEBUG_TMP("ADEC_RegInit Clear start!\n");
	for (loop = 0; loop < 384; loop = loop + 4)
	{
		*(((UINT32 *)g_pstADECReg_l8) + loop) = 0x00000000;
	}
	ADEC_DEBUG_TMP("ADEC_RegInit Clear end!\n");

	//Get a elapsed nono-second time
	elapsed = jiffies - start;
	ADEC_DEBUG_TMP("ADEC Reg. clears in %d ms.\n", (UINT32)elapsed);

	//Set download address to ADEC DSP memory
	if( pstMemCfgADEC->fw0_memory_base <= 0x10000000 )	// < 256MB DDR Memory
	{
		//set address for DSP0
		g_pstADECReg_l8->aud_dsp0offset5 = pstMemCfgADEC->fw0_memory_base + DSP_OFFSET5_MASK_UNDER_256MB;
		g_pstADECReg_l8->aud_dsp0offset6 = pstMemCfgADEC->dsp0_memory_base + DSP_OFFSET6_MASK_UNDER_256MB;

		//set address for DSP1
		g_pstADECReg_l8->aud_dsp1offset5 = pstMemCfgADEC->fw1_memory_base + DSP_OFFSET5_MASK_UNDER_256MB;
		g_pstADECReg_l8->aud_dsp1offset6 = pstMemCfgADEC->dsp1_memory_base + DSP_OFFSET6_MASK_UNDER_256MB;
	}
	else
	{
		//set address for DSP0
		g_pstADECReg_l8->aud_dsp0offset5 = pstMemCfgADEC->fw0_memory_base + DSP_OFFSET5_MASK_UPPER_256MB;
		g_pstADECReg_l8->aud_dsp0offset6 = pstMemCfgADEC->dsp0_memory_base + DSP_OFFSET6_MASK_UPPER_256MB;

		//set address for DSP1
		g_pstADECReg_l8->aud_dsp1offset5 = pstMemCfgADEC->fw1_memory_base + DSP_OFFSET5_MASK_UPPER_256MB;
		g_pstADECReg_l8->aud_dsp1offset6 = pstMemCfgADEC->dsp1_memory_base + DSP_OFFSET6_MASK_UPPER_256MB;
	}

	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp0offset5[0x%08X]\n", *(UINT32*)&g_pstADECReg_l8->aud_dsp0offset5);
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp0offset6[0x%08X]\n", *(UINT32*)&g_pstADECReg_l8->aud_dsp0offset6);
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp1offset5[0x%08X]\n", *(UINT32*)&g_pstADECReg_l8->aud_dsp1offset5);
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp1offset6[0x%08X]\n", *(UINT32*)&g_pstADECReg_l8->aud_dsp1offset6);

	//CPBM base address and size set for file play
	g_pstADECReg_l8->aud_esmsaddr0 = pstMemCfgADEC->cpbm_memory_base + MAX_SIZE_OF_CPBM_INDEX_TABLE - pstMemCfgADEC->dsp0_memory_base;
	g_pstADECReg_l8->aud_esmsize0	= pstMemCfgADEC->cpbm_memory_size - MAX_SIZE_OF_CPBM_INDEX_TABLE;

	//CPBA base address and size set for file play
	g_pstADECReg_l8->aud_esasaddr0 = pstMemCfgADEC->cpba_memory_base - pstMemCfgADEC->dsp0_memory_base;
	g_pstADECReg_l8->aud_esasize0	= pstMemCfgADEC->cpba_memory_size;

	// INDEX_TABLE base address and size set
	g_pstADECReg_l8->aud_esidxtbl	= pstMemCfgADEC->cpbm_memory_base - pstMemCfgADEC->dsp0_memory_base;
	g_pstADECReg_l8->aud_esidxsize	= MAX_SIZE_OF_CPBM_INDEX_TABLE;


	//SE param base address set for copying SE parameters
	g_pstADECReg_l8->aud_se_paramaddr	= pstMemCfgADEC->dpbm_memory_base - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_se_param_size	= MAX_SIZE_OF_LGSE_PARAM;

	//DPBM base address and size set for copying decoded file
	g_pstADECReg_l8->aud_dpbm_saddr = pstMemCfgADEC->dpbm_memory_base + MAX_SIZE_OF_LGSE_PARAM - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_dpbm_size  = pstMemCfgADEC->dpbm_memory_size - MAX_SIZE_OF_LGSE_PARAM;

	//DPBA base address and size set for copying decoded file
	g_pstADECReg_l8->aud_dpba_saddr = pstMemCfgADEC->dpba_memory_base - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_dpba_size  = pstMemCfgADEC->dpba_memory_size;

	//MPB0 base address and size set for copying decoded file
	g_pstADECReg_l8->aud_mpb0_saddr = pstMemCfgADEC->mpb0_memory_base - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_mpb0_size  = pstMemCfgADEC->mpb0_memory_size;

	//MPB1 base address and size set for copying decoded file
	g_pstADECReg_l8->aud_mpb1_saddr = pstMemCfgADEC->mpb1_memory_base - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_mpb1_size  = pstMemCfgADEC->mpb1_memory_size;

	//MPB2 base address and size set for copying decoded file
	g_pstADECReg_l8->aud_mpb2_saddr = pstMemCfgADEC->mpb2_memory_base - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_mpb2_size  = pstMemCfgADEC->mpb2_memory_size;

	//MPB3 base address and size set for copying decoded file
	g_pstADECReg_l8->aud_mpb3_saddr = pstMemCfgADEC->mpb3_memory_base - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_mpb3_size  = pstMemCfgADEC->mpb3_memory_size;

	//DSP1 Inter base address and size set for copying decoded file
	g_pstADECReg_l8->aud_dsp1inter_saddr = pstMemCfgADEC->inter_memory_base - pstMemCfgADEC->dsp1_memory_base;
	g_pstADECReg_l8->aud_dsp1inter_size  = pstMemCfgADEC->inter_memory_size;

	//Workaround for no audio
#if 1
	//0x000~
	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_esmsaddr0 == g_pstADECReg_l8->aud_esmsaddr0)
			break;

		g_pstADECReg_l8->aud_esmsaddr0 = aud_esmsaddr0;
		ADEC_DEBUG ( "aud_esmsaddr0[0x%X]\n", g_pstADECReg_l8->aud_esmsaddr0);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_esmsize0 == g_pstADECReg_l8->aud_esmsize0)
			break;

		g_pstADECReg_l8->aud_esmsize0 = aud_esmsize0;
		ADEC_DEBUG ( "aud_esmsize0[0x%X]\n", g_pstADECReg_l8->aud_esmsize0);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_esasaddr0 == g_pstADECReg_l8->aud_esasaddr0)
			break;

		g_pstADECReg_l8->aud_esasaddr0 = aud_esasaddr0;
		ADEC_DEBUG ( "aud_esasaddr0[0x%X]\n", g_pstADECReg_l8->aud_esasaddr0);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_esasize0 == g_pstADECReg_l8->aud_esasize0)
			break;

		g_pstADECReg_l8->aud_esasize0 = aud_esasize0;
		ADEC_DEBUG ( "aud_esasize0[0x%X]\n", g_pstADECReg_l8->aud_esasize0);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_esidxtbl == g_pstADECReg_l8->aud_esidxtbl)
			break;

		g_pstADECReg_l8->aud_esidxtbl = aud_esidxtbl;
		ADEC_DEBUG ( "aud_esidxtbl[0x%X]\n", g_pstADECReg_l8->aud_esidxtbl);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_esidxsize == g_pstADECReg_l8->aud_esidxsize)
			break;

		g_pstADECReg_l8->aud_esidxsize = aud_esidxsize;
		ADEC_DEBUG ( "aud_esidxsize[0x%X]\n", g_pstADECReg_l8->aud_esidxsize);
	}


	//0x200~
	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_se_paramaddr == g_pstADECReg_l8->aud_se_paramaddr)
			break;

		g_pstADECReg_l8->aud_se_paramaddr = aud_se_paramaddr;
		ADEC_DEBUG ( "aud_se_paramaddr[0x%X]\n", g_pstADECReg_l8->aud_se_paramaddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_se_paramsize == g_pstADECReg_l8->aud_se_param_size)
			break;

		g_pstADECReg_l8->aud_se_param_size = aud_se_paramsize;
		ADEC_DEBUG ( "aud_se_paramsize[0x%X]\n", g_pstADECReg_l8->aud_se_param_size);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_dpbm_saddr == g_pstADECReg_l8->aud_dpbm_saddr)
			break;

		g_pstADECReg_l8->aud_dpbm_saddr = aud_dpbm_saddr;
		ADEC_DEBUG ( "aud_dpbm_saddr[0x%X]\n", g_pstADECReg_l8->aud_dpbm_saddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_dpbm_size == g_pstADECReg_l8->aud_dpbm_size)
			break;

		g_pstADECReg_l8->aud_dpbm_size = aud_dpbm_size;
		ADEC_DEBUG ( "aud_dpbm_size[0x%X]\n", g_pstADECReg_l8->aud_dpbm_size);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_dpba_saddr == g_pstADECReg_l8->aud_dpba_saddr)
			break;

		g_pstADECReg_l8->aud_dpba_saddr = aud_dpba_saddr;
		ADEC_DEBUG ( "aud_dpba_saddr[0x%X]\n", g_pstADECReg_l8->aud_dpba_saddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_dpba_size == g_pstADECReg_l8->aud_dpba_size)
			break;

		g_pstADECReg_l8->aud_dpba_size = aud_dpba_size;
		ADEC_DEBUG ( "aud_dpba_size[0x%X]\n", g_pstADECReg_l8->aud_dpba_size);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb0_saddr == g_pstADECReg_l8->aud_mpb0_saddr)
			break;

		g_pstADECReg_l8->aud_mpb0_saddr = aud_mpb0_saddr;
		ADEC_DEBUG ( "aud_mpb0_saddr[0x%X]\n", g_pstADECReg_l8->aud_mpb0_saddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb0_size == g_pstADECReg_l8->aud_mpb0_size)
			break;

		g_pstADECReg_l8->aud_mpb0_size = aud_mpb0_size;
		ADEC_DEBUG ( "aud_mpb0_size[0x%X]\n", g_pstADECReg_l8->aud_mpb0_size);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb1_saddr == g_pstADECReg_l8->aud_mpb1_saddr)
			break;

		g_pstADECReg_l8->aud_mpb1_saddr = aud_mpb1_saddr;
		ADEC_DEBUG ( "aud_mpb1_saddr[0x%X]\n", g_pstADECReg_l8->aud_mpb1_saddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb1_size == g_pstADECReg_l8->aud_mpb1_size)
			break;

		g_pstADECReg_l8->aud_mpb1_size = aud_mpb1_size;
		ADEC_DEBUG ( "aud_mpb1_size[0x%X]\n", g_pstADECReg_l8->aud_mpb1_size);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb2_saddr == g_pstADECReg_l8->aud_mpb2_saddr)
			break;

		g_pstADECReg_l8->aud_mpb2_saddr = aud_mpb2_saddr;
		ADEC_DEBUG ( "aud_mpb2_saddr[0x%X]\n", g_pstADECReg_l8->aud_mpb2_saddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb2_size == g_pstADECReg_l8->aud_mpb2_size)
			break;

		g_pstADECReg_l8->aud_mpb2_size = aud_mpb2_size;
		ADEC_DEBUG ( "aud_mpb2_size[0x%X]\n", g_pstADECReg_l8->aud_mpb2_size);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb3_saddr == g_pstADECReg_l8->aud_mpb3_saddr)
			break;

		g_pstADECReg_l8->aud_mpb3_saddr = aud_mpb3_saddr;
		ADEC_DEBUG ( "aud_mpb3_saddr[0x%X]\n", g_pstADECReg_l8->aud_mpb3_saddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_mpb3_size == g_pstADECReg_l8->aud_mpb3_size)
			break;

		g_pstADECReg_l8->aud_mpb3_size = aud_mpb3_size;
		ADEC_DEBUG ( "aud_mpb3_size[0x%X]\n", g_pstADECReg_l8->aud_mpb3_size);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_inter_saddr == g_pstADECReg_l8->aud_dsp1inter_saddr)
			break;

		g_pstADECReg_l8->aud_dsp1inter_saddr = aud_inter_saddr;
		ADEC_DEBUG ( "aud_dsp1inter_saddr[0x%X]\n", g_pstADECReg_l8->aud_dsp1inter_saddr);
	}

	for(loop = 0; loop < ADEC_REG_WRITE_RETRY; loop++)
	{
		if(aud_inter_size == g_pstADECReg_l8->aud_dsp1inter_size)
			break;

		g_pstADECReg_l8->aud_dsp1inter_size = aud_inter_size;
		ADEC_DEBUG ( "aud_dsp1inter_size[0x%X]\n", g_pstADECReg_l8->aud_dsp1inter_size);
	}

#endif


	//Enable audio interrupt DSP0
	g_pstADECReg_l8->aud_int0_en = 0x1;

	//Enable audio interrupt DSP1
	g_pstADECReg_l8->aud_int1_en = 0x1;


	//Unmask audio interrupt 0 /1
	//Read Interrupt Mask reg.
	memcpy (&stIntMask, (UINT32*) &g_pstADECReg_l8->aud_intmask, sizeof (UINT32));

	//un-mask interrupt for DSP0
	stIntMask.int_err_mask0  = 0x1;
	stIntMask.int_info_mask0 = 0x1;
	stIntMask.int_lip_mask0  = 0x1;
	stIntMask.int_frm_mask0  = 0x1;
	stIntMask.int_dec_event0 = 0x1;

	//un-mask interrupt for DSP1
	stIntMask.int_err_mask1  = 0x1;
	stIntMask.int_info_mask1 = 0x1;
	stIntMask.int_lip_mask1  = 0x1;
	stIntMask.int_frm_mask1  = 0x1;
	stIntMask.int_dec_event1 = 0x1;

	//Write Interrupt Mask reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_intmask, &stIntMask, sizeof (UINT32));


	//Read Input, Output port reg.
	memcpy (&stSetInOut, (UINT32*) &g_pstADECReg_l8->aud_set_inout, sizeof (UINT32));

	/* Set a output I2S mode : I2S, 24 bit */
	stSetInOut.out_format = 0x0;	//I2S
	stSetInOut.out_res    = 0x3;	//24 bit

	//Set I2S and 24 bit mode for I2S input
	stSetInOut.in_format  = 0x0; 	//I2S
	stSetInOut.in_res	  = 0x3; 	//24 bit

	//Set check bit for Input, Output port reg.
	stSetInOut.check_bit  = 0x1;

	//Write Input, Output port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_inout, &stSetInOut, sizeof (UINT32));


	//Read Lip Sync.  reg.
	memcpy (&stLipCfg, (UINT32*) &g_pstADECReg_l8->aud_lip_cfg, sizeof (UINT32));

	/* Set a Lip Sync Control : Enable Main and Associate Decoding */
	stLipCfg.main_lip_en0 = 0x1;	//Enable Main Decoding
	stLipCfg.asoc_lip_en0 = 0x1;	//Enable Associate Decoding

	//Write Lip Sync. reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_lip_cfg, &stLipCfg, sizeof (UINT32));


	//Read Main Lip Sync. Bound  reg.
	memcpy (&stMLipBound, (UINT32*) &g_pstADECReg_l8->aud_mlip_bound0, sizeof (UINT32));

	/* Set a Main Lip Sync Bound Control */
	stMLipBound.main_ubound0 = 0x20;	//DSP0 main lipsync upper bound. Unit=ms : 32 ms
	stMLipBound.main_lbound0 = 0x20;	//DSP0 main lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
	stMLipBound.main_offset0 = 0x100;	//DSP0 main lipsync PTS offset. Unit=ms : 256 ms

	//Write Main Lip Sync. Bound reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_mlip_bound0, &stMLipBound, sizeof (UINT32));


	//Read Associate Lip Sync. Bound  reg.
	memcpy (&stALipBound, (UINT32*) &g_pstADECReg_l8->aud_alip_bound0, sizeof (UINT32));

	/* Set a Associate Lip Sync Bound Control */
	stALipBound.asoc_ubound0 = 0x20;	//DSP0 associate lipsync upper bound. Unit=ms : 32 ms
	stALipBound.asoc_lbound0 = 0x20;	//DSP0 associate lipsync lower bound. Unit=ms. Value is taken as minus : -32 ms
	stALipBound.asoc_offset0 = 0x100;	//DSP0 associate lipsync PTS offset. Unit=ms : 256 ms

	//Write Associate Lip Sync. Bound reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_alip_bound0, &stALipBound, sizeof (UINT32));

	//Get a elapsed nono-second time
	elapsed = jiffies - start;
	ADEC_DEBUG_TMP("ADEC_RegInit in %d ms.\n", (UINT32)elapsed);

	ADEC_DEBUG_TMP ( "ADEC_RegInit - CPBM addr[0x%X], size[0x%X]\n", g_pstADECReg_l8->aud_esmsaddr0, g_pstADECReg_l8->aud_esmsize0);
	return RET_OK;
}

int ADEC_InitRegForCheckbit ( void )
{
	AUD_PLAY_MODE		stPlayMode;
	AUD_DSP1_STATE		stDsp1State;

	AUD_SET_SE 			stSetSe;
	AUD_BT_LEVEL 		stBTlevel;

	AUD_SET_VOLUME 		stSetVolume;
	AUD_SET_DS_VOL 		stSetDSVolume;
	AUD_SET_W_VOL		stSetWooferVol;
	AUD_SET_SH_VOL		stSetSHVol;

	AUD_SET_DLY0		stSetDly0;
	AUD_SET_DLY1		stSetDly1;
	AUD_MPB0_CONFIG0	stMPB0Cfg0;


	//Set check bit for aud_lip_cfg
	AUD_L8_RdFL(aud_lip_cfg);
	AUD_L8_Wr01(aud_lip_cfg, check_bit, 0x6);
	AUD_L8_WrFL(aud_lip_cfg);

	//Set check bit for aud_mlip_bound0
	AUD_L8_RdFL(aud_mlip_bound0);
	AUD_L8_Wr01(aud_mlip_bound0, check_bit, 0x6);
	AUD_L8_WrFL(aud_mlip_bound0);

	//Set check bit for aud_alip_bound0
	AUD_L8_RdFL(aud_alip_bound0);
	AUD_L8_Wr01(aud_alip_bound0, check_bit, 0x6);
	AUD_L8_WrFL(aud_alip_bound0);

	//Set check bit for aud_deccfg0
	AUD_L8_RdFL(aud_deccfg0);
	AUD_L8_Wr01(aud_deccfg0, check_bit, 0x6);
	AUD_L8_WrFL(aud_deccfg0);

	//Set check bit for aud_deccfg1
	AUD_L8_RdFL(aud_deccfg1);
	AUD_L8_Wr01(aud_deccfg1, check_bit, 0x6);
	AUD_L8_WrFL(aud_deccfg1);

	//Set check bit for aud_mpb0_config1
	AUD_L8_RdFL(aud_mpb0_config1);
	AUD_L8_Wr01(aud_mpb0_config1, check_bit, 0x6);
	AUD_L8_WrFL(aud_mpb0_config1);

	//Read play mode reg.
	memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

	//Set check bit for play mode
	stPlayMode.check_bit = 0x6;

	//Write play mode reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_play_mode, &stPlayMode, sizeof (UINT32));


	//Read DSP1 State reg.
	memcpy (&stDsp1State, (UINT32*) &g_pstADECReg_l8->aud_dsp1_state, sizeof (UINT32));

	//Set check bit for SRC & Delay function
	stDsp1State.check_bit    = 0x6;
	stDsp1State.src_enable   = 1;
	stDsp1State.delay_enable = 1;

	//Write DSP1 State reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_dsp1_state, &stDsp1State, sizeof (UINT32));


	//Read set SE reg.
	memcpy (&stSetSe, (UINT32*) &g_pstADECReg_l8->aud_set_se, sizeof (UINT32));

	//Set check bit for SE
	stSetSe.check_bit = 0x6;

	//Write set SE reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_se, &stSetSe, sizeof (UINT32));


	//Read BT Level reg.
	memcpy (&stBTlevel, (UINT32*) &g_pstADECReg_l8->aud_bt_level, sizeof (UINT32));

	//Set check bit for BT Level
	stBTlevel.check_bit = 0x6;

	//Write BT Level reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_bt_level, &stBTlevel, sizeof (UINT32));


	//Read setVolume reg.
	memcpy (&stSetVolume, (UINT32*) &g_pstADECReg_l8->aud_set_volume, sizeof (UINT32));

	//Set check bit for D-AMP volume
	stSetVolume.check_bit = 1;

	//Write setVolume reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_volume, &stSetVolume, sizeof (UINT32));


	//Read setDSVolume port reg.
	memcpy (&stSetDSVolume, (UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, sizeof (UINT32));

	//Set check bit for DS volume
	stSetDSVolume.check_bit = 1;

	//Write setDSVolume port reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_ds_vol, &stSetDSVolume, sizeof (UINT32));


	//Read SetWooferVol reg.
	memcpy (&stSetWooferVol, (UINT32*) &g_pstADECReg_l8->aud_set_w_vol, sizeof (UINT32));

	//Set check bit for woofer volume
	stSetWooferVol.check_bit = 1;

	//Write SetWooferVol reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_w_vol, &stSetWooferVol, sizeof (UINT32));


	//Read SetSHVol reg.
	memcpy (&stSetSHVol, (UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, sizeof (UINT32));

	//Set check bit for SCART & HPDRV volume
	stSetSHVol.check_bit = 1;

	//Write SetSHVol reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_sh_vol, &stSetSHVol, sizeof (UINT32));


	//Read Set Delay0 reg.
	memcpy (&stSetDly0, (UINT32*) &g_pstADECReg_l8->aud_set_dly0, sizeof (UINT32));

	//Set check bit for delay0
	stSetDly0.check_bit = 0x6;

	//Write Set Delay0 reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_dly0, &stSetDly0, sizeof (UINT32));


	//Read Set Delay1 reg.
	memcpy (&stSetDly1, (UINT32*) &g_pstADECReg_l8->aud_set_dly1, sizeof (UINT32));

	//Set check bit for delay1
	stSetDly1.check_bit = 0x6;

	//Write Set Delay1 reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_set_dly1, &stSetDly1, sizeof (UINT32));


	//Read MPB0 config0 reg.
	memcpy (&stMPB0Cfg0, (UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, sizeof (UINT32));

	//Set check bit for MPB0 config0
	stMPB0Cfg0.check_bit = 0x6;

	//Write MPB0 config0 reg.
	memcpy ((UINT32*) &g_pstADECReg_l8->aud_mpb0_config0, &stMPB0Cfg0, sizeof (UINT32));


	ADEC_PRINT ( "ADEC_InitRegForCheckbit - CPBM addr[0x%X]\n", g_pstADECReg_l8->aud_esmsize0);

	return RET_OK;
}

/**
 * Initialize Sound Engine(SE) Parameters for default
 * @see ADEC_DRV_InitModule
*/
int ADEC_InitSEParams( void )
{
	UINT32	ui32Count = 0;

	//Copy auto volume parameter
	for(ui32Count = 0; ui32Count < LX_CV_AUTOVOLUME_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_AUTOVOLUME_PARAM + ui32Count] = gADEC_CVAVLParams[ui32Count];
	}

	//Copy Clear Voice2(CV2) parameter
	for(ui32Count = 0; ui32Count < LX_CV_CV2_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_CV2_PARAM + ui32Count] = gADEC_CVParams[ui32Count];
	}

	//Copy User Equalizer Band  parameter
	for(ui32Count = 0; ui32Count < LX_CV_UEQ_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_UEQ_PARAM + ui32Count] = gADEC_CVUEQParams[ui32Count];
	}

	//Copy User Equalizer Band 2 parameter
	for(ui32Count = 0; ui32Count < LX_CV_UEQ2_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_UEQ2_PARAM + ui32Count] = gADEC_CVUEQ2Params[ui32Count];
	}

	//Copy dynamic EQ parameter
	for(ui32Count = 0; ui32Count < LX_CV_DEQ_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_DEQ_PARAM + ui32Count] = gADEC_CVBassEQParams[ui32Count];
	}

	//Copy infinite sound parameter
	for(ui32Count = 0; ui32Count < LX_CV_IFNSND_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_IFNSND_PARAM + ui32Count] = gADEC_CVSurroundParams[ui32Count];
	}

	//Copy input level parameter
	for(ui32Count = 0; ui32Count < LX_CV_INLEV_PARAM_NUM; ui32Count++)
	{
		g_pui32LGSEParamVirAddr[ADDR_LGSE_INLEV_PARAM + ui32Count] = gADEC_CVINLEVParams[ui32Count];
	}

	return RET_OK;
}

/**
 * Wait for DSP0 Ready!.
 * @see
*/
void ADEC_WaitForDSP0Ready ( void )
{
	UINT8	codecNumber;
	UINT8 	*codecName[14] = {"Unknown ", "PCM     ", "MS10_DDC", "MS10_DDT", "MPEG1, 2",	\
							  "MP3     ", "WMA     ", "DTS     ", "SILK    ", "G.729   ",	\
							  "G.711   ", "DTS_BY  ", "PCM_WAV ", "NONE    " };

	ADEC_PRINT("Wait %d ms for DSP0 to Ready!!!\n", WAIT_DSP0_READY_TIMEOUT);
	wait_event_interruptible_timeout(ADEC_wait_dsp0_ready, ADEC_dsp0_ready_waitq_flag != 0, WAIT_DSP0_READY_TIMEOUT);

	//Get codec type
	codecNumber = g_pstADECReg_l8->aud_dsp0_state.codec_type;

	//sanity check for debug print
	if( codecNumber > 13 )
		codecNumber = 13;	//Unknown

	if(ADEC_dsp0_ready_waitq_flag)
		ADEC_ERROR("DSP0 to Ready : %s Codec Ver. = 0x%X(type = %d)\n", codecName[codecNumber], g_pstADECReg_l8->aud_dsp0_state.codec_ver, codecNumber);
	else
		ADEC_ERROR("DSP0 to Not Ready : %s Codec Ver. = 0x%X(type = %d)\n", codecName[codecNumber], g_pstADECReg_l8->aud_dsp0_state.codec_ver, codecNumber);

	return;
}

/**
 * Wait for DSP1 Ready!.
 * @see
*/
void ADEC_WaitForDSP1Ready ( void )
{
	ADEC_PRINT("Wait %d ms for DSP1 to Ready!!!\n", WAIT_DSP1_READY_TIMEOUT);
	wait_event_interruptible_timeout(ADEC_wait_dsp1_ready, ADEC_dsp1_ready_waitq_flag != 0, WAIT_DSP1_READY_TIMEOUT);

	if(ADEC_dsp1_ready_waitq_flag)
		ADEC_ERROR("DSP1 to Ready : Sound En Codec Ver. = 0x%X\n", g_pstADECReg_l8->aud_dsp1_state.codec_ver);
	else
		ADEC_ERROR("DSP1 to Not Ready : Sound En Codec Ver. = 0x%X\n", g_pstADECReg_l8->aud_dsp1_state.codec_ver);

	return;
}

/**
 * Signal as DSP0 is ready.
 * @see
*/
void ADEC_SignalDSP0Ready ( void )
{
	ADEC_PRINT("ADEC_SignalDSP0Ready\n");

	ADEC_dsp0_ready_waitq_flag = 1;
	wake_up_interruptible(&ADEC_wait_dsp0_ready);

	return;
}

/**
 * Signal as DSP1 is ready.
 * @see
*/
void ADEC_SignalDSP1Ready ( void )
{
	ADEC_PRINT("ADEC_SignalDSP1Ready\n");

	ADEC_dsp1_ready_waitq_flag = 1;
	wake_up_interruptible(&ADEC_wait_dsp1_ready);

	return;
}


/**
 * Routine for monitor task to check audio underflow issue
 * @see
*/
int ADEC_L8_Monitor_Task( void *i_pvParam )
{
#ifdef ENABLE_DSP_MONITOR
//	LX_ADEC_SRC_TYPE_T	eCurLoadedCodec;
	AUD_PLAY_MODE		stPlayMode;

	UINT32		ui32CurPPBRptr;
	BOOLEAN		bResetFlag = FALSE;
#endif

	ADEC_DEBUG( "ADEC> Task is created!\n" );

	do
	{
		if ( kthread_should_stop() )
		{
			ADEC_PRINT( "ADEC> Task - exit!\n" );
			break;
		}

		/* Reset a wait queue monitor variable. */
		g_suiWaitQForMonitor = 0;

#ifndef ENABLE_DSP_MONITOR	//Enable  DSP0 / 1 debug
		wait_event_interruptible( Monitor_WaitQ, g_suiWaitQForMonitor != 0 );
#else
		wait_event_interruptible_timeout( Monitor_WaitQ, g_suiWaitQForMonitor != 0, 1000 );

		//Read play mode reg.
		memcpy (&stPlayMode, (UINT32*) &g_pstADECReg_l8->aud_play_mode, sizeof (UINT32));

		if( stPlayMode.play_mode == LX_ADEC_PLAY_START )
		{
			/* Read DPBM Write Pointer */
			ui32CurPPBRptr = g_pstADECReg_l8->aud_ppb_rptr;

			/* Check PPB Read Pointer */
			if( (ui32CurPPBRptr == g_ui32PrevPPBRptr)
			  &&(ui32CurPPBRptr != 0) )
			{
				bResetFlag = TRUE;

				ADEC_ERROR( "DSP0 PES WR = 0x%X, RD = 0x%X\n",	g_pstADECReg_l8->aud_pesm_wptr0, g_pstADECReg_l8->aud_pesm_rptr0);
				ADEC_ERROR( "DSP1 DPB WR = 0x%X, RD = 0x%X\n",	g_pstADECReg_l8->aud_dpbm_wptr, g_pstADECReg_l8->aud_dpbm_rptr);
				ADEC_ERROR( "DSP1 PPB WR = 0x%X, RD = 0x%X\n",	g_pstADECReg_l8->aud_ppb_wptr, g_pstADECReg_l8->aud_ppb_rptr);
			}
			else
			{
				/* Update DPBM Read Pointer */
				g_ui32PrevPPBRptr = ui32CurPPBRptr;

				ADEC_DEBUG_TMP( "DSP0 PES WR = 0x%X, RD = 0x%X\n",	g_pstADECReg_l8->aud_pesm_wptr0, g_pstADECReg_l8->aud_pesm_rptr0);
			}

#if 0
			//Check reset flag
			if( (bResetFlag == TRUE) || (g_suiWaitQForMonitor == 1) )
			{
				ADEC_DEBUG( "Reset Audio...\n" );

				//Save a global current codec to re-load codec
				eCurLoadedCodec   = g_eCurLoadedCodec;
				g_eCurLoadedCodec = LX_ADEC_SRC_TYPE_NONE;

				//Stop decoding
				ADEC_L8_StopDecoding();

				//Wait for stopping DSP 0 / 1
				msleep_interruptible(30);

				//re-start DSP 0 /1
				ADEC_L8_StartDecoding( (unsigned long)eCurLoadedCodec );

				//Clear reset flag
				bResetFlag = FALSE;
			}
#endif

		}
#endif	//#ifndef ENABLE_DSP_MONITOR
	} while ( 1 );
	return 0;
}

/** @} */


