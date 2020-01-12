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
 *  author	Jong-Sang Oh(jongsang.oh@lge.com)
 *  version	1.0
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
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "os_util.h" 	//for alloc functions
#include "proc_util.h"
#include "debug_util.h"
#include <asm/div64.h> //do_div

#include "base_drv.h"	// av lipsync

#include "adec_coredrv_l9.h"
#include "aac/adec_aac_probe.h"

//audio codec for DSP 0 and DSP1
#include "firmware/multi_codec_l9.h"	// PCM, MS10_DDC/DDT, MPEG23 audio codec for DSP0

#if (KDRV_PLATFORM != KDRV_COSMO_PLATFORM)
#ifndef ENABLE_DDCO_TEST
#include "firmware/multi_se_codec_l9.h" 		// se(sound engine) codec for DSP1(NetCast Model)
#else
#include "firmware/multi_se_codec_l9_gcd.h" 	// se(sound engine) codec for DSP1(GCD Model)
#endif
#else
#include "firmware/multi_se_codec_l9_gcd.h" 	// se(sound engine) codec for DSP1(GCD Model)
#endif

#include "adec_ipc_func.h"

//IPC header files
#include "adec_ipc/adec_ipc_cmd.h"
#include "adec_ipc/adec_ipc_req.h"
#include "adec_ipc/adec_ipc_list.h"
#include "adec_ipc/adec_ipc_reg_api.h"

//Buffer header files
#include "buffer/adec_buf.h"
#include "buffer/adec_common_buf.h"
#include "buffer/adec_internal_buf.h"
#include "buffer/adec_inter_buf_arm.h"
#include "buffer/adec_inter_buf_writer.h"
#include "buffer/adec_inter_buf_reader.h"
#include "buffer/adec_buf_helper.h"


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#ifndef ENABLE_DIRECT_MIX_VOLUME
/****************************************
*  Audio Mix Volume Table                             *
*  Volume = 20 log(volume/1024)                           *
*  4096 (     6dB, 2.000)   GP4 : +6dB(later)            *
*  2048 (     3dB, 2.000)   GP3 : +3dB                     *
*  1024 (     0dB, 1.000)                                          *
*    512 (   -3dB, 0.500)   GP3 :  -38dB                   *
*        0 (  -infdB, 0.56:10-6)                                  *
*****************************************/
const UINT32 g_ADEC_MixVolTable[ADEC_MIX_VOLUME_TABLE_SIZE + 1] =
{
       0,     318,	  338,	  358,    378,		/*  0 ~    4 */
     382,	  402,	  422,	  442,	  462,		/*  5 ~    9 */
     489,	  509,	  529,	  549,	  589,		/* 10 ~ 14 */
     625,	  655,	  685,	  715,	  755,		/* 15 ~ 19 */
     800,	  840,	  880,	  920,	  960,		/* 20 ~ 24 */
    1024,	 1044,	 1064,	 1084,	 1100,		/* 25 ~ 29 */
    1124,	 1144,	 1164,	 1184,	 1200,		/* 30 ~ 34 */
    1231,	 1251,	 1271,	 1291,	 1311,		/* 35 ~ 39 */
    1350,	 1370,	 1390,	 1410,	 1440,		/* 40 ~ 44 */
    1480,	 1500,	 1520,	 1540,	 1560,		/* 45 ~ 49 */
    1623,	 1653,	 1683,	 1713,	 1743,		/* 50 ~ 54 */
    1780,	 1810,	 1840,	 1870,	 1910,		/* 55 ~ 59 */
    1951,	 1971,	 1991,	 2101,	 2121,		/* 60 ~ 64 */
    2139,	 2179,	 2219,	 2259,	 2299,		/* 65 ~ 69 */
    2346,	 2386,	 2426,	 2466,	 2506,		/* 70 ~ 74 */
    2572,	 2622,	 2672,	 2722,	 2772,    	/* 75 ~ 79 */
    2820,	 2920,	 2960,	 3000,	 3040,		/* 80 ~ 84 */
    3092,	 3132,	 3172,	 3212,	 3252,    	/* 85 ~ 89 */
    3391,	 3441,	 3491,	 3591,	 3641,		/* 90 ~ 94 */
    3718,	 3758,	 3798,	 3838,	 3878,    	/* 95 ~ 99 */
	4077                                 		/* 100       */
};
#endif	//#ifndef ENABLE_DIRECT_MIX_VOLUME

#ifndef  ENABLE_NETCAST_PRINT
UINT8 *sourcetype[0x81] =
{
	"NONE",
	"MAIN",
	"ASSO","","","","","","","","","","","","","",
	"MIX0","","","","","","","","","","","","","","","",
	"MIX1","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",
	"MIX2","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","","",
};


UINT8 * codectype[28] =
{
	"UNKONWON",
	"PCM",
	"AC3",
	"EAC3",
	"MPEG",
	"AAC",
	"HEAAC",
	"DRA",
	"MP3",
	"DTS",
	"SIF",
	"SIF_BTSC",
	"SIF_A2",
	"DEFAULT",
	"NONE",
	"MULTI",
	"16",
	"17",
	"18",
	"19",
	"WMA_PRO",
	"ADPCM",
	"VORBIS",
	"AMR_NB",
	"AMR_WB",
	"RA10",
	"AAC_LC",
	"AAC_LC_ENC",
	"MS10_DDC0",
};
#endif


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define ARM_DSP0_BUF_STATUS_MEMBASE		(pstMemCfgADEC->debug_memory_base)
#define ARM_DSP_DEBUG_MEMSIZE			0x1000
#define ARM_DSP1_BUF_STATUS_MEMBASE		(ARM_DSP0_BUF_STATUS_MEMBASE + ARM_DSP_DEBUG_MEMSIZE)
#define ARM_PROFILE_INFO_MEMBASE		(ARM_DSP1_BUF_STATUS_MEMBASE + ARM_DSP_DEBUG_MEMSIZE)

//VDEC H/W Reg. Address
#define VDEC_LIPSYNC_REG_MEMBASE		(0xC00038F0)
#define VDEC_LIPSYNC_REG_MEMSIZE		(0x10)

/* Define number of AAC ES parser check */
#define AAC_ES_PARSER_CHECK_NUMBER	2

/* Define Google TV libsync offset */
#define GTV_MEDIA_LIBSYNC_OFFSET	200

//#define USE_ASK_ALIVE_DSP_MONITOR

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
/**
 * Context Information for ADEC Kernel device.
*/


/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern spinlock_t 			gADEC_DSP_event_lock;
extern spinlock_t 			gADEC_DoneNoti_lock;

extern wait_queue_head_t 	ADEC_wait_dsp0_ready;
extern int 					ADEC_dsp0_ready_waitq_flag;
extern wait_queue_head_t 	ADEC_wait_dsp1_ready;
extern int 					ADEC_dsp1_ready_waitq_flag;

//for adec monitor task
extern struct task_struct 		*g_pstADECMonitorTask;

extern UINT32	g_ui32CurEventType;
extern UINT32	g_ui32RecEventType;
extern LX_ADEC_DEC_MODE_T	g_ui32RecEventMode;

extern wait_queue_head_t 	adec_poll_wait_queue;

//For monitor task
extern LX_ADEC_TASK_MSG_T		g_suiWaitQForMonitor;
extern wait_queue_head_t 		Monitor_WaitQ;

extern LX_ADEC_SPDIF_DTO_RATE_T	g_prevDTORate[SPDIF_ES_OUT_DTO_CLOCK_TYPE][SPDIF_ES_OUT_FREQ_NUMBER];

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
/* define ADEC reg for L9 */
volatile AUD_REG_T	*gpRealRegAUD = NULL;
AUD_REG_T			*gpRegAUD = NULL;

VDEC_LIPSYNC_REG_T	*g_pVDEC_LipsyncReg = NULL;	//VDEC H/W Reg. for Lip Sync Control with ADEC H/W Reg.

//This value is set by ADEC_DRV_InitModule function
LX_ADEC_INIT_TYPE_T		g_eAdecInitType = LX_ADEC_INIT_TYPE_NONE;

//DSP loading flag after H/W reset
BOOLEAN g_bLoadingCodecForDebug   = FALSE;	//TRUE, FALSE
BOOLEAN g_bCopyingCodecForDebug   = TRUE;
BOOLEAN g_bCopyingSeCodecForDebug = TRUE;
BOOLEAN g_bIpcDebugReceive 		  = FALSE;

// DTO Rate
LX_ADEC_CLOCK_SRC_T	g_SetClcokSrc = LX_ADEC_CLOCK_UNKNOWN;

//ADEC KDRV Structure
LX_ADEC_KDRV_T	g_adec_kdrv =
{
	.bDecStarted[LX_ADEC_DEC_0]		= FALSE,
	.bDecStarted[LX_ADEC_DEC_1]		= FALSE,

	.bMixStarted[LX_ADEC_MIXER_0]	= FALSE,
	.bMixStarted[LX_ADEC_MIXER_1]	= FALSE,
	.bMixStarted[LX_ADEC_MIXER_2]	= FALSE,
	.bMixStarted[LX_ADEC_MIXER_3]	= FALSE,

	.bDecADStarted					= FALSE,
	.bEncStarted					= FALSE,
	.bInitDone						= FALSE,

	.soundEngineMask				= 0,
	.hpVolume						= 0x024F00,		/* UI : 10 */

	.adecSource[LX_ADEC_DEC_0]		= LX_ADEC_IN_PORT_NONE,
	.adecSource[LX_ADEC_DEC_1]		= LX_ADEC_IN_PORT_NONE,

	.adecType[LX_ADEC_DEC_0]		= LX_ADEC_SRC_TYPE_UNKNOWN,
	.adecType[LX_ADEC_DEC_1]		= LX_ADEC_SRC_TYPE_UNKNOWN,

	.adecHdmiInputFreq				= LX_ADEC_SAMPLING_FREQ_NONE,

	.drcMode[LX_ADEC_DEC_0]			= LX_ADEC_DOLBY_LINE_MODE,
	.drcMode[LX_ADEC_DEC_1]			= LX_ADEC_DOLBY_LINE_MODE,

	.downmixMode[LX_ADEC_DEC_0]		= LX_ADEC_LORO_MODE,
	.downmixMode[LX_ADEC_DEC_1]		= LX_ADEC_LORO_MODE,

	.spdifMode 						= LX_ADEC_SPDIF_PCM,
	.bSpdifPcm						= FALSE,
	.bSpdifMainOnly					= FALSE,

	.clipStatus[LX_ADEC_DEC_0]		= LX_ADEC_CLIP_MODE_NONE,
	.clipStatus[LX_ADEC_DEC_1]		= LX_ADEC_CLIP_MODE_NONE,

	.clipBufStatus[LX_ADEC_DEC_0]	= LX_ADEC_CLIP_BUF_NONE,
	.clipBufStatus[LX_ADEC_DEC_1]	= LX_ADEC_CLIP_BUF_NONE,

	.timePaused[LX_ADEC_DEC_0]		= 0,
	.timePaused[LX_ADEC_DEC_1]		= 0,

	.timePreCBT[LX_ADEC_DEC_0]		= 0xFFFFFFFF,
	.timePreCBT[LX_ADEC_DEC_1]		= 0xFFFFFFFF,

	.timePreSBT[LX_ADEC_DEC_0]		= 0xFFFFFFFF,
	.timePreSBT[LX_ADEC_DEC_1]		= 0xFFFFFFFF,

#ifdef ENABLE_AAC_PARSER
	.aacProbe[LX_ADEC_DEC_0]		= NULL,
	.aacProbe[LX_ADEC_DEC_1]		= NULL,

	.bAacParsed[LX_ADEC_DEC_0]		= FALSE,
	.bAacParsed[LX_ADEC_DEC_1]		= FALSE,

	.bAacDSIParsed[LX_ADEC_DEC_0]	= FALSE,
	.bAacDSIParsed[LX_ADEC_DEC_1]	= FALSE,
#endif

	.index[LX_ADEC_DEC_0].FeedIDX 	= 0,
	.index[LX_ADEC_DEC_0].FeedCNT 	= 0,
	.index[LX_ADEC_DEC_0].PrevNOTI 	= INITIAL_CONDITION, // no meaing just for diff with NewNOTI
	.index[LX_ADEC_DEC_0].NewNOTI 	= 0,
	.index[LX_ADEC_DEC_0].WriteCNT 	= INITIAL_CONDITION,
	.index[LX_ADEC_DEC_0].ReadCNT 	= 0,
	.index[LX_ADEC_DEC_0].FreeCNT 	= CPB_AUI_INDEX_COUNT,
	.index[LX_ADEC_DEC_0].UsedCNT 	= 0,
	.index[LX_ADEC_DEC_0].NotiCNT 	= 0,

	.index[LX_ADEC_DEC_1].FeedIDX 	= 0,
	.index[LX_ADEC_DEC_1].FeedCNT 	= 0,
	.index[LX_ADEC_DEC_1].PrevNOTI 	= INITIAL_CONDITION, // no meaing just for diff with NewNOTI
	.index[LX_ADEC_DEC_1].NewNOTI 	= 0,
	.index[LX_ADEC_DEC_1].WriteCNT 	= INITIAL_CONDITION,
	.index[LX_ADEC_DEC_1].ReadCNT 	= 0,
	.index[LX_ADEC_DEC_1].FreeCNT 	= CPB_AUI_INDEX_COUNT,
	.index[LX_ADEC_DEC_0].UsedCNT 	= 0,
	.index[LX_ADEC_DEC_1].NotiCNT 	= 0,

	.mixerStatus[LX_ADEC_MIXER_0]	= LX_ADEC_CLIP_MODE_NONE,
	.mixerStatus[LX_ADEC_MIXER_1]	= LX_ADEC_CLIP_MODE_NONE,
	.mixerStatus[LX_ADEC_MIXER_2]	= LX_ADEC_CLIP_MODE_NONE,
	.mixerStatus[LX_ADEC_MIXER_3]	= LX_ADEC_CLIP_MODE_NONE,
	.mixerStatus[LX_ADEC_MIXER_4]	= LX_ADEC_CLIP_MODE_NONE,

	.mixerIndex[LX_ADEC_MIXER_0]	= 0,
	.mixerIndex[LX_ADEC_MIXER_1]	= 0,
	.mixerIndex[LX_ADEC_MIXER_2]	= 0,
	.mixerIndex[LX_ADEC_MIXER_3]	= 0,
	.mixerIndex[LX_ADEC_MIXER_4]	= 0,
};


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int  ADEC_L9_Create_MIX_Interbuf_Writer(	\
		void **ppStruct, ARM_PROCESSOR_ADAP * bufAdapIf, UINT32 *pVirAddr,	\
		UINT32 bufAddr, UINT32 bufSize, void *pBufReg);

static int ADEC_L9_CreateAudioClipBuffer(LX_ADEC_DEC_MODE_T i_eAdecMode);

static void ADEC_RSP_CMD_Status(unsigned int _hdr, unsigned int _bodySize, void* _param);
static void ADEC_RSP_PROC_Alive(unsigned int _hdr, unsigned int _bodySize, void* _param);
static void ADEC_REQ_PROC_Underflow(unsigned int _hdr, unsigned int _bodySize, void* _param);
static void ADEC_REQ_PROC_PresentEnd(unsigned int _hdr, unsigned int _bodySize, void* _param);
static void ADEC_REQ_PROC_DSP0NotRespond(unsigned int _hdr, unsigned int _bodySize, void* _param);


static int ADEC_SetWMAProParams(LX_ADEC_CLIP_START_INFO_T stStartInfo);
static int ADEC_SetRA8Params(LX_ADEC_CLIP_START_INFO_T stStartInfo);
static int ADEC_ConvertCodecType( LX_ADEC_SRC_TYPE_T srcType);
static int ADEC_SetSPDIFOutputType(LX_ADEC_IN_PORT_T eInputPort, LX_ADEC_SRC_TYPE_T adecType);
static int ADEC_SetSPDIFOutputTypeForDual ( LX_ADEC_DEC_MODE_T i_eAdecMode );

static LX_ADEC_SPDIF_DTO_RATE_T ADEC_GetDTORate(UINT32 clockSrc, LX_ADEC_SAMPLING_FREQ_T samplingFreq);

#ifdef ENABLE_AAC_PARSER
static LX_ADEC_SAMPLING_FREQ_T ADEC_L9_GetAACSamplingFreq(ADEC_AAC_MEDIA_INFO_T mediaInfo);
#endif	//#ifdef ENABLE_AAC_PARSER


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT32	*g_pui32DSP0CodecVirAddr = NULL;
static UINT32	*g_pui32DSP1CodecVirAddr = NULL;
static UINT32	*g_pui32SEParamVirAddr   = NULL;

static void		*g_pCPBM_WriterStruct = NULL;
static UINT32	*g_pui32CPBM_VirAddr  = NULL;

static void		*g_pCPBA_WriterStruct = NULL;
static UINT32	*g_pui32CPBA_VirAddr  = NULL;

static void		*g_pAENC_ReaderStruct = NULL;
static UINT32	*g_pui32AENC_VirAddr  = NULL;

static void		*g_pMIX0_WriterStruct = NULL;
static UINT32	*g_pui32MIX0_VirAddr  = NULL;

static void		*g_pMIX1_WriterStruct = NULL;
static UINT32	*g_pui32MIX1_VirAddr  = NULL;

static void		*g_pMIX2_WriterStruct = NULL;
static UINT32	*g_pui32MIX2_VirAddr  = NULL;

static void		*g_pMIX3_WriterStruct = NULL;
static UINT32	*g_pui32MIX3_VirAddr  = NULL;

static void		*g_pAPCM_ReaderStruct = NULL;
static UINT32	*g_pui32APCM_VirAddr  = NULL;

static void		*g_pLLB_WriterStruct = NULL;
static UINT32	*g_pui32LLB_VirAddr  = NULL;

static void		*g_pAACR_ReaderStruct = NULL;
static UINT32	*g_pui32AACR_VirAddr  = NULL;

//For debug info. print
static UINT32	*g_pui32DSP0Status_VirAddr = NULL;
static UINT32	*g_pui32DSP1Status_VirAddr = NULL;
static KDRV_ADEC_PROFILE_REPORT_T *g_pstProfile_VirAddr = NULL;


/* Set for DSP 0/1 state for memory flush */
static UINT32	g_ui32ResetDsp0State = 0;
static UINT32	g_ui32ResetDsp1State = 0;

//Semaphore
static OS_SEM_T	g_ADEC_Sema;
static OS_SEM_T	g_ADEC_Sema_Pcm;
static OS_SEM_T	g_ADEC_Sema_Spdif;

//Current ADEC source input type
static LX_ADEC_IN_PORT_T	g_eCurAdecSource = LX_ADEC_IN_PORT_NONE;

//Current codec type for DSP0
static LX_ADEC_SRC_TYPE_T	g_eCurLoadedCodec = LX_ADEC_SRC_TYPE_NONE;

// DSP Monitoring Task
#ifndef ENABLE_ADEC_JTAG_FOR_CPU
static BOOLEAN	gbEnableDspMonitor = TRUE;
#else
static BOOLEAN	gbEnableDspMonitor = FALSE;
#endif

//DSP1 running counter
static UINT32	g_ui32CheckDSPRunningCnt = 0;
static UINT32	gPreDsp0Timer = 0;
static UINT32	gPreDsp1Timer = 0;

//g_ui32AdecPrintCount is only increased in AAD_SIF_GetCurAnalogMode function
static UINT32	g_ui32AdecPrintCount = 0;


/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * Run suspend the adec module.
 * ADEC module shall be suspend itself with pre-defined configuration value.
 * @see ADEC_suspend
*/
int ADEC_L9_RunSuspend(void)
{

	ADEC_PRINT("ADEC_RunSuspend : Done!\n");
	return 0;
}

/**
 * Run resume the adec module.
 * ADEC module shall be resumed itself with pre-defined configuration value.
 * @see ADEC_suspend
*/
int ADEC_L9_RunResume(void)
{
	ADEC_DEBUG("[%s] resume start...\n", ADEC_MODULE);

	//Initializes the CTOP CTRL Clock Source Registers.
	(void)MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_CLOCK_DTV );

	//Reset Low : APB, SRC, ADEC DSP and AAD etc...
	ADEC_L9_ClearReset(ADEC_OPTION_ALL);

	//Initializes the Mixed IP Registers.
	(void)MIXED_IP_ADEC_L9_InitCodecRegisters();

	/* MIC Power-down */
	MIXED_IP_ADEC_L9_PowerControl(LX_ADEC_POWER_MIC, LX_ADEC_POWER_DOWN);

	/* DAC Power-down */
	MIXED_IP_ADEC_L9_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_DOWN);

	//Reset High : APB, SRC and Mixed IP etc...
	(void)ADEC_L9_SetReset(ADEC_OPTION_OTHERS);

	// Initialize register
	(void)ADEC_L9_InitReg();
	ADEC_PRINT("ADEC_L9_RunResume : ADEC_InitReg\n");

	// Initialize register for check bit because DSP does not read reg. when CPU access reg.
	(void)ADEC_L9_InitRegForCheckbit();
	ADEC_PRINT("ADEC_L9_RunResume : ADEC_InitRegForCheckbit\n");

	//Load Codec for DSP0 & 1
	ADEC_L9_LoadDSP1Codec();
	ADEC_L9_LoadDSP0Codec( LX_ADEC_SRC_TYPE_AC3 );
	ADEC_PRINT("Init DSP0 Ready! Codec Ver : 0x%X\n", gpRealRegAUD->aud_dsp0_state.codec_ver );
	ADEC_PRINT("Init DSP1 Ready! Codec Ver : 0x%X\n", gpRealRegAUD->aud_dsp1_state.codec_ver );

	//Start-up the CODEC IP for mute off.
	(void)MIXED_IP_ADEC_L9_StartCodec();

	ADEC_PRINT("ADEC_RunResume : Done!\n");
	return 0;
}

/**
 * Initializes the adec module.
 * ADEC module shall be initialized itself with pre-defined configuration value.
 * ADEC module includes audio codec, post-processing, SPDIF parts
 * @see ADEC_Init
*/
int ADEC_L9_InitModule ( LX_ADEC_INIT_TYPE_T eInitType )
{
	int ret = RET_OK;

	KDRV_ADEC_STATUS_T 		*pu32StatusMemBase;

	ADEC_SET_PCM_INTERRUPT_T	pcmRate;
	ADEC_SET_SPDIF_DTO_T 	spdifDTO;

	//Initialize ADEC semaphore
	OS_InitMutex(&g_ADEC_Sema, OS_SEM_ATTR_DEFAULT);

	//Initialize ADEC PCM semaphore for HDMI Input
	OS_InitMutex(&g_ADEC_Sema_Pcm, OS_SEM_ATTR_DEFAULT);

	//Initialize ADEC SPDIF semaphore
	OS_InitMutex(&g_ADEC_Sema_Spdif, OS_SEM_ATTR_DEFAULT);

	//Init IPC Global memory and semaphore
	ADEC_IPC_InitFunc();

	//Copy ADEC init type
	g_eAdecInitType = eInitType;

#ifdef ENABLE_DDCO_TEST
	//Enable DDCO Encoder for  GCD DDCO test
	g_eAdecInitType = LX_ADEC_INIT_TYPE_SYS_ATSC_GCD;
#endif

	if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC )
	{
		ADEC_ERROR ("ATSC SELECTED\n");
	}
	else if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
	{
		ADEC_ERROR ("DVB SELECTED\n");
	}
	else if ( g_eAdecInitType == LX_ADEC_INIT_TYPE_SYS_ATSC_GCD )
	{
		ADEC_ERROR ("ATSC GCD SELECTED\n");
	}
	else if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		ADEC_ERROR ("ATSC GCD SELECTED(Enable DDCO Test)\n");
	}
	else
	{
		ADEC_ERROR ("LX_ADEC_INIT_TYPE_NONE(0x%X)!!!\n", g_eAdecInitType);
	}

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


	//Mapping virtual memory address for CPBM
	g_pui32CPBM_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->cpbm_memory_base,
											pstMemCfgADEC->cpbm_memory_size);
	if (NULL == g_pui32CPBM_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : EMP Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "ADEC CPB address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->cpbm_memory_base);
	ADEC_PRINT ( "ADEC CPB address-virt[0x%08X]\n", (UINT32) g_pui32CPBM_VirAddr);


	//Mapping virtual memory address for CPBA
	g_pui32CPBA_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->cpba_memory_base,
											pstMemCfgADEC->cpba_memory_size);
	if (NULL == g_pui32CPBA_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : EMP Virtual address ERR!");
		return RET_ERROR;
	}

	ADEC_PRINT ( "ADEC CPB ASSO address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->cpba_memory_base);
	ADEC_PRINT ( "ADEC CPB ASSO address-virt[0x%08X]\n", (UINT32) g_pui32CPBA_VirAddr);

	//Mapping virtual memory address for APCM Buffer
	g_pui32APCM_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->ddco_memory_base,
											pstMemCfgADEC->ddco_memory_size);
	if (NULL == g_pui32APCM_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : APCM Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "ADEC APCM address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->ddco_memory_base);
	ADEC_PRINT ( "ADEC APCM address-virt[0x%08X]\n", (UINT32) g_pui32APCM_VirAddr);

	//Mapping virtual memory address for AACR Buffer
	g_pui32AACR_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->acr_memory_base,
											pstMemCfgADEC->acr_memory_size);
	if (NULL == g_pui32AACR_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : AACR Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "ADEC AACR address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->acr_memory_base);
	ADEC_PRINT ( "ADEC AACR address-virt[0x%08X]\n", (UINT32) g_pui32AACR_VirAddr);

	//Mapping virtual memory address for AENC Buffer
	g_pui32AENC_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->enc_memory_base,
											pstMemCfgADEC->enc_memory_size);
	if (NULL == g_pui32AENC_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : AENC Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "ADEC AENC address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->enc_memory_base);
	ADEC_PRINT ( "ADEC AENC address-virt[0x%08X]\n", (UINT32) g_pui32AENC_VirAddr);

	//Mapping virtual memory address for MIX0
	g_pui32MIX0_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->mpb0_memory_base,
										    pstMemCfgADEC->mpb0_memory_size);
	if (NULL == g_pui32MIX0_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : MIX0 Virtual address ERR!");
		return RET_ERROR;
	}

	ADEC_PRINT ( "ADEC MIX0 address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->mpb0_memory_base);
	ADEC_PRINT ( "ADEC MIX0 address-virt[0x%08X]\n", (UINT32) g_pui32MIX0_VirAddr);

	//Mapping virtual memory address for MIX1
	g_pui32MIX1_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->mpb1_memory_base,
										    pstMemCfgADEC->mpb1_memory_size);
	if (NULL == g_pui32MIX1_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : MIX1 Virtual address ERR!");
		return RET_ERROR;
	}

	ADEC_PRINT ( "ADEC MIX1 address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->mpb1_memory_base);
	ADEC_PRINT ( "ADEC MIX1 address-virt[0x%08X]\n", (UINT32) g_pui32MIX1_VirAddr);

	//Mapping virtual memory address for MIX2
	g_pui32MIX2_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->mpb2_memory_base,
										    pstMemCfgADEC->mpb2_memory_size);
	if (NULL == g_pui32MIX2_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : MIX2 Virtual address ERR!");
		return RET_ERROR;
	}

	ADEC_PRINT ( "ADEC MIX2 address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->mpb2_memory_base);
	ADEC_PRINT ( "ADEC MIX2 address-virt[0x%08X]\n", (UINT32) g_pui32MIX2_VirAddr);

	//Mapping virtual memory address for MIX3
	g_pui32MIX3_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->mpb3_memory_base,
										    pstMemCfgADEC->mpb3_memory_size);
	if (NULL == g_pui32MIX3_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : MIX3 Virtual address ERR!");
		return RET_ERROR;
	}

	ADEC_PRINT ( "ADEC MIX3 address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->mpb3_memory_base);
	ADEC_PRINT ( "ADEC MIX3 address-virt[0x%08X]\n", (UINT32) g_pui32MIX3_VirAddr);

	//Mapping virtual memory address for LLB
	g_pui32LLB_VirAddr = (UINT32*) ioremap(pstMemCfgADEC->llb_memory_base,
										    pstMemCfgADEC->llb_memory_size);
	if (NULL == g_pui32LLB_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : LLB Virtual address ERR!");
		return RET_ERROR;
	}

	ADEC_PRINT ( "ADEC LLB address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->llb_memory_base);
	ADEC_PRINT ( "ADEC LLB address-virt[0x%08X]\n", (UINT32) g_pui32LLB_VirAddr);


	//Mapping virtual memory address for sound engine(SE) parameter memory
	g_pui32SEParamVirAddr = (UINT32 *) ioremap(pstMemCfgADEC->se_memory_base,
											   pstMemCfgADEC->se_memory_size);
	if (NULL == g_pui32SEParamVirAddr)
	{
		ADEC_ERROR ("ADEC_LoadCodec : LGSE Param Virtual address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "ADEC SE address-phy [0x%08X]\n", (UINT32)pstMemCfgADEC->se_memory_base);
	ADEC_PRINT ( "ADEC SE address-virt[0x%08X]\n", (UINT32)g_pui32SEParamVirAddr);



	//Mapping virtual memory address for dsp0 status memory
	g_pui32DSP0Status_VirAddr = (UINT32*) ioremap(ARM_DSP0_BUF_STATUS_MEMBASE,
											      ARM_DSP_DEBUG_MEMSIZE);
	if (NULL == g_pui32DSP0Status_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : DSP0 Status Regiseter Base address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "Status Regiseter Base address-phy [0x%08X]\n", (UINT32) ARM_DSP0_BUF_STATUS_MEMBASE);
	ADEC_PRINT ( "Status Regiseter Base address-virt[0x%08X]\n", (UINT32) g_pui32DSP0Status_VirAddr);

	//Mapping virtual memory address for dsp1 status memory
	g_pui32DSP1Status_VirAddr = (UINT32*) ioremap(ARM_DSP1_BUF_STATUS_MEMBASE,
											  	  ARM_DSP_DEBUG_MEMSIZE);
	if (NULL == g_pui32DSP1Status_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : DSP1 Status Regiseter Base address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "Status Regiseter Base address-phy [0x%08X]\n", (UINT32) ARM_DSP1_BUF_STATUS_MEMBASE);
	ADEC_PRINT ( "Status Regiseter Base address-virt[0x%08X]\n", (UINT32) g_pui32DSP1Status_VirAddr);

	pu32StatusMemBase = (KDRV_ADEC_STATUS_T *)g_pui32DSP1Status_VirAddr;
	pu32StatusMemBase->u32StatusBuffCnt = 0;

	//Mapping virtual memory address for profile memory
	g_pstProfile_VirAddr= (KDRV_ADEC_PROFILE_REPORT_T*) ioremap(ARM_PROFILE_INFO_MEMBASE,
											  					ARM_DSP_DEBUG_MEMSIZE);
	if (NULL == g_pstProfile_VirAddr)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : Profile Info Base address ERR!");
		return RET_ERROR;
	}
	g_pstProfile_VirAddr->moduleCount = 0;

	ADEC_PRINT ( "Profile Info Base address-phy [0x%08X]\n", (UINT32) ARM_PROFILE_INFO_MEMBASE);
	ADEC_PRINT ( "Profile Info Base address-virt[0x%08X]\n", (UINT32) g_pstProfile_VirAddr);

	//Mapping virtual memory address for vdec lipsync reg
	g_pVDEC_LipsyncReg = (VDEC_LIPSYNC_REG_T*) ioremap(VDEC_LIPSYNC_REG_MEMBASE,
													   VDEC_LIPSYNC_REG_MEMSIZE);
	if (NULL == g_pVDEC_LipsyncReg)
	{
		ADEC_ERROR ("ADEC_L9_InitModule : VDEC Lipsync Reg address ERR!");
		return RET_ERROR;
	}
	ADEC_PRINT ( "VDEC Lipsync Reg address-phy [0x%08X]\n", (UINT32) VDEC_LIPSYNC_REG_MEMBASE);
	ADEC_PRINT ( "VDEC Lipsync Reg address-virt[0x%08X]\n", (UINT32) g_pVDEC_LipsyncReg);

	//Initializes the CTOP CTRL Clock Source Registers.
	(void)MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_CLOCK_DTV );

	//Clear DSP 0 /1 State
	g_ui32ResetDsp0State = 0;
	g_ui32ResetDsp1State = 0;

	//Reset Low : APB, SRC, ADEC DSP and AAD etc...
	ADEC_L9_ClearReset(ADEC_OPTION_ALL);

	//Initializes the Mixed IP Registers.
	(void)MIXED_IP_ADEC_L9_InitCodecRegisters();

	/* MIC Power-down */
	MIXED_IP_ADEC_L9_PowerControl(LX_ADEC_POWER_MIC, LX_ADEC_POWER_DOWN);

	if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC )
	{
		/* DAC Power-down */
		MIXED_IP_ADEC_L9_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_DOWN);
	}
	else if ( g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB )
	{
		/* DAC Power-up */
		MIXED_IP_ADEC_L9_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_UP);
	}
	else
	{
		/* DAC Power-up */
		MIXED_IP_ADEC_L9_PowerControl(LX_ADEC_POWER_DAC, LX_ADEC_POWER_UP);
	}

	//Reset High : APB, SRC and Mixed IP etc...
	(void)ADEC_L9_SetReset(ADEC_OPTION_OTHERS);

	// Initialize register
	(void)ADEC_L9_InitReg();
	ADEC_PRINT("ADEC_L9_InitModule : ADEC_InitReg\n");

	// Initialize register for check bit because DSP does not read reg. when CPU access reg.
	(void)ADEC_L9_InitRegForCheckbit();
	ADEC_PRINT("ADEC_L9_InitModule : ADEC_InitRegForCheckbit\n");

	//Init IPC Register
	ADEC_IPC_REG_Init(
		(void *)&gpRealRegAUD->aud_ipc_cmd_rptr,
		(void *)&gpRealRegAUD->aud_ipc_req_rptr,
		(void *)&gpRealRegAUD->aud_ipc_dbg_rptr);
	ADEC_IPC_Init(pstMemCfgADEC->ipc_memory_base, pstMemCfgADEC->ipc_memory_size);

	//Register IPC callback function
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_UNDERFLOW,   ADEC_REQ_PROC_Underflow);
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_PRESENT_END, ADEC_REQ_PROC_PresentEnd);
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_DSP0NOTRSP,  ADEC_REQ_PROC_DSP0NotRespond);

	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_DEC_RESULT,  ADEC_RSP_CMD_Status);
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_ALIVE, 	  ADEC_RSP_PROC_Alive);

	//Create Mix0 buffer to protect abnormal operation
	if(g_pMIX0_WriterStruct == NULL)
	{
		/* Create MIX0 for EMP */
		ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
			  (void *)&g_pMIX0_WriterStruct, (void *)&gMIX0_ArmWriterAdap, (UINT32 *)g_pui32MIX0_VirAddr,	\
			   pstMemCfgADEC->mpb0_memory_base, pstMemCfgADEC->mpb0_memory_size, (void *)&gpRealRegAUD->aud_mpb0_wo);
		if(ret == WRITER_RET_OPEN_OK)
		{

			ADEC_PRINT("ADEC ADEC_L9_Create_MIX0_Interbuf_Writer Success!!!\n");
		}
		else
		{
			ADEC_ERROR("ADEC ADEC_L9_Create_MIX0_Interbuf_Writer Fail!!!\n");
		}
	}

	//Create Mix1 buffer to protect abnormal operation
	if(g_pMIX1_WriterStruct == NULL)
	{
		/* Create MIX1 for EMP */
		ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
			  (void *)&g_pMIX1_WriterStruct, (void *)&gMIX1_ArmWriterAdap, (UINT32 *)g_pui32MIX1_VirAddr,	\
			   pstMemCfgADEC->mpb1_memory_base, pstMemCfgADEC->mpb1_memory_size, (void *)&gpRealRegAUD->aud_mpb1_wo);
		if(ret == WRITER_RET_OPEN_OK)
		{

			ADEC_PRINT("ADEC ADEC_L9_Create_MIX1_Interbuf_Writer Success!!!\n");
		}
		else
		{
			ADEC_ERROR("ADEC ADEC_L9_Create_MIX1_Interbuf_Writer Fail!!!\n");
		}
	}

	//Create Mix2 buffer to protect abnormal operation
	if(g_pMIX2_WriterStruct == NULL)
	{
		/* Create MIX2 for EMP */
		ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
			  (void *)&g_pMIX2_WriterStruct, (void *)&gMIX2_ArmWriterAdap, (UINT32 *)g_pui32MIX2_VirAddr,	\
			   pstMemCfgADEC->mpb2_memory_base, pstMemCfgADEC->mpb2_memory_size, (void *)&gpRealRegAUD->aud_mpb2_wo);
		if(ret == WRITER_RET_OPEN_OK)
		{

			ADEC_PRINT("ADEC ADEC_L9_Create_MIX2_Interbuf_Writer Success!!!\n");
		}
		else
		{
			ADEC_ERROR("ADEC ADEC_L9_Create_MIX2_Interbuf_Writer Fail!!!\n");
		}
	}

	//Create Mix3 buffer to protect abnormal operation
	if(g_pMIX3_WriterStruct == NULL)
	{
		/* Create MIX3 for EMP */
		ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
			  (void *)&g_pMIX3_WriterStruct, (void *)&gMIX3_ArmWriterAdap, (UINT32 *)g_pui32MIX3_VirAddr,	\
			   pstMemCfgADEC->mpb3_memory_base, pstMemCfgADEC->mpb3_memory_size, (void *)&gpRealRegAUD->aud_mpb3_wo);
		if(ret == WRITER_RET_OPEN_OK)
		{

			ADEC_PRINT("ADEC ADEC_L9_Create_MIX3_Interbuf_Writer Success!!!\n");
		}
		else
		{
			ADEC_ERROR("ADEC ADEC_L9_Create_MIX3_Interbuf_Writer Fail!!!\n");
		}
	}

	//Create LLB buffer to protect abnormal operation
	if(g_pLLB_WriterStruct == NULL)
	{
		/* Create LLB for EMP Game */
		ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
			  (void *)&g_pLLB_WriterStruct, (void *)&gLLB_ArmWriterAdap, (UINT32 *)g_pui32LLB_VirAddr,	\
			   pstMemCfgADEC->llb_memory_base, pstMemCfgADEC->llb_memory_size, (void *)&gpRealRegAUD->aud_llb_wo);
		if(ret == WRITER_RET_OPEN_OK)
		{

			ADEC_PRINT("ADEC ADEC_L9_Create_LLB_Interbuf_Writer Success!!!\n");
		}
		else
		{
			ADEC_ERROR("ADEC ADEC_L9_Create_LLB_Interbuf_Writer Fail!!!\n");
		}
	}

	//Set a SPDIF output mode
	AUD_RdFL(aud_set_inout);

	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB)
	{
#ifndef ENABLE_DDCO_TEST_DEC1
		//Set SPDIF output mode : PCM Main Only Output mode for Audio Description in DVB model
		AUD_Wr01(aud_set_inout, spdif_mode, 0x1);	//Main Only

		/* Update Internal Status Value */
		g_adec_kdrv.bSpdifMainOnly = TRUE;
#else
		//Set SPDIF output mode : Normal PCM Mixed Output
		AUD_Wr01(aud_set_inout, spdif_mode, 0x0);	//Normal

		/* Update Internal Status Value */
		g_adec_kdrv.bSpdifMainOnly = FALSE;
#endif
	}
	else
	{
		//Set SPDIF output mode : Normal PCM Mixed Output
		AUD_Wr01(aud_set_inout, spdif_mode, 0x0);	//Normal

		/* Update Internal Status Value */
		g_adec_kdrv.bSpdifMainOnly = FALSE;
	}

	AUD_WrFL(aud_set_inout);

	//Load DSP1 codec for audio processing
	ret = ADEC_L9_LoadDSP1Codec();
	if (ret != RET_OK)
	{
		ADEC_ERROR("error in ADEC_L9_LoadDSP1Codec call\n");
		return RET_ERROR;
	}
	ADEC_PRINT("Init DSP1 Ready! Codec Ver : 0x%X\n", gpRealRegAUD->aud_dsp1_state.codec_ver );

	//Set a SPDIF DDCO Encoder in DSP0 decoder/encoder for GCD platform
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Set codec source by adec type
		ret = ADEC_L9_LoadDSP0Codec(LX_ADEC_SRC_TYPE_AC3);
		if (ret != RET_OK)
		{
			ADEC_ERROR("error in ADEC_L9_LoadDSP0Codec call\n");
			return RET_ERROR;
		}
	}

	//Start-up the CODEC IP for mute on.
	(void)MIXED_IP_ADEC_L9_StartCodec();

	/* Update Internal Status Value */
	g_adec_kdrv.bInitDone = TRUE;

#ifdef ENABLE_ADEC_JTAG
	//Enable Wiggler JTAG for Audio DSP connection
	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
#ifdef ENABLE_ADEC_JTAG_FOR_CPU
		CTOP_CTRL_L9B_RdFL(ctr29_reg_swrst);
		CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, jtag1_sel, 0x0);	//VDEC, default
		CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, jtag0_sel, 0x1);	//0 : CPU, 1 : ADEC
		CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, aud_jtag_no, 0x1);	//0 : JTAG1, 1 : JTAG0
		CTOP_CTRL_L9B_WrFL(ctr29_reg_swrst);
		ADEC_PRINT("ENABLE_ADEC_JTAG_FOR_CPU : 0x%X\n", CTOP_CTRL_L9B_Rd(ctr29_reg_swrst) );
#else
		CTOP_CTRL_L9B_RdFL(ctr29_reg_swrst);
		CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, jtag1_sel, 0x2);	//ADEC
		CTOP_CTRL_L9B_WrFL(ctr29_reg_swrst);
		ADEC_PRINT("ENABLE_ADEC_JTAG_FOR_PERI : 0x%X\n", CTOP_CTRL_L9B_Rd(ctr29_reg_swrst) );
#endif
	}
	else
	{
		CTOP_CTRL_L9_RdFL(ctr29);
		CTOP_CTRL_L9_Wr01(ctr29, jtag_sel, 0x2);
		CTOP_CTRL_L9_WrFL(ctr29);
	}
	ADEC_PRINT("ADEC DSP Wiggler JTAG Selected Now!!!\n");
#endif	//#ifdef ENABLE_ADEC_JTAG

	//Clears a PCM Clock interrupt count
	pcmRate.ui32InFs		  = (UINT32)LX_ADEC_SAMPLING_FREQ_48_KHZ;
	pcmRate.ui32PCMclkRate	  = (UINT32)APLL_CLOCK_VALUE_12_288MHZ;
	pcmRate.ui32ForceCntClear = (UINT32)1;

	ADEC_IPC_SetPCMRate(pcmRate);

	//Set a SPDIF DTO Frequency for IPC
	spdifDTO.ui32SampleFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;
	spdifDTO.ui32DtoRate	= LX_ADEC_SPDIF_DTO_NORMAL;
	spdifDTO.ui32ForceCntClear = (UINT32)1;
	spdifDTO.ui32isHDMI		   = (UINT32)0;
	spdifDTO.ui32HDMIFs		   = LX_ADEC_SAMPLING_FREQ_NONE;
	ADEC_IPC_SetSPDIFDTO(spdifDTO);

	ADEC_PRINT("ADEC_L9_InitModule : Done!\n");

	return RET_OK;
}


/**
 * Sets a ADEC event to be getted.
 * In order to get a device event, use a ADEC_IOR_GET_EVENT ioctl function.\n
 * If once setted event, send event and reset by ADEC_IOW_RESET_EVENT ioctl function.
 * @see ADEC_DRV_GetEvent
*/
int ADEC_L9_SetEvent ( LX_ADEC_EVENT_TYPE_T eEventType )
{
	/* Set event */
	OS_LockMutex(&g_ADEC_Sema);
	g_ui32CurEventType |= eEventType;
	OS_UnlockMutex(&g_ADEC_Sema);

	ADEC_DEBUG("ADEC_L9_SetEvent   : 0x%X\n", g_ui32CurEventType);

	return RET_OK;
}

/**
 * Resets a ADEC event to be getted.
 * This function clears a event to be getted.
 * Before clearing a event, a pending event must be notified.
 * @see ADEC_DRV_SetEvent
*/
int ADEC_L9_ResetEvent ( LX_ADEC_EVENT_TYPE_T eEventType )
{
	ULONG	flags = 0;

	/* Reset event */
	OS_LockMutex(&g_ADEC_Sema);
	g_ui32CurEventType &= (~eEventType);	//Clear current event mask
	OS_UnlockMutex(&g_ADEC_Sema);

	//spin lock for protection : lock
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
	g_ui32RecEventType &= (~eEventType);	//Clear current event received
	//spin lock for protection : unlock
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	ADEC_DEBUG("ADEC_L9_ResetEvent : 0x%X\n", g_ui32CurEventType);

	return RET_OK;
}

/**
 * Gets a ADEC event to have been setted and arised from device driver.
 * Returns any of the setted events from the ADEC device.\n
 * When the pending events have been read by the DDI module,\n
 * the ADEC device’s internal event bit field shall be cleared.\n
 * If any event does not arises, this function is polled.\n
 * If event has a memory data, use a LX_ADEC_EVENT_INFO_T with location and size info.
 * @see ADEC_DRV_SetEvent
*/
int ADEC_L9_GetEvent ( LX_ADEC_EVENT_INFO_T *pstEventInfo )
{
	ULONG	flags = 0;

	UINT32	eEventType = 0;

	LX_ADEC_DEC_MODE_T	eEventMode = LX_ADEC_DEC_MODE_NONE;

	//Sanity check
	if(pstEventInfo == NULL)
	{
		ADEC_ERROR("ADEC_L9_GetEvent : error!!!(pstEventInfo = %p)\n", pstEventInfo);
		return RET_ERROR;
	}

	//Check a adec mode to clear adec event
	//DEC0
	if(pstEventInfo->adecMode & LX_ADEC_DECODE_MAIN)
	{
		eEventType |= (LX_ADEC_EVENT_DEC0_INTERRUPT		\
				     | LX_ADEC_EVENT_DEC0_UNDERFLOW		\
				     | LX_ADEC_EVENT_DSP0_UNMASK_ALL	\
					 | LX_ADEC_EVENT_CLIP0_UNMASK_ALL);			//temp
		eEventMode |= LX_ADEC_DECODE_MAIN;
	}

	//DEC1
	if(pstEventInfo->adecMode & LX_ADEC_DECODE_ASSO)
	{
		eEventType |= (LX_ADEC_EVENT_DEC1_INTERRUPT		\
				     | LX_ADEC_EVENT_DEC1_UNDERFLOW		\
				     | LX_ADEC_EVENT_DSP1_UNMASK_ALL	\
					 | LX_ADEC_EVENT_CLIP1_UNMASK_ALL);			//temp
		eEventMode |= LX_ADEC_DECODE_ASSO;
	}

	//MIX0
	if(pstEventInfo->adecMode & LX_ADEC_MIX_BUF0)
	{
		eEventType |= (LX_ADEC_EVENT_DEC0_INTERRUPT | LX_ADEC_EVENT_DEC0_UNDERFLOW);	//temp
		eEventMode |= LX_ADEC_MIX_BUF0;
	}

	//MIX1
	if(pstEventInfo->adecMode & LX_ADEC_MIX_BUF1)
	{
		eEventType |= (LX_ADEC_EVENT_DEC0_INTERRUPT | LX_ADEC_EVENT_DEC0_UNDERFLOW);	//temp
		eEventMode |= LX_ADEC_MIX_BUF1;
	}

	//MIX2
	if(pstEventInfo->adecMode & LX_ADEC_MIX_BUF2)
	{
		eEventType |= (LX_ADEC_EVENT_DEC0_INTERRUPT | LX_ADEC_EVENT_DEC0_UNDERFLOW);	//temp
		eEventMode |= LX_ADEC_MIX_BUF2;
	}

	//MIX3
	if(pstEventInfo->adecMode & LX_ADEC_MIX_BUF3)
	{
		eEventType |= (LX_ADEC_EVENT_DEC0_INTERRUPT | LX_ADEC_EVENT_DEC0_UNDERFLOW);	//temp
		eEventMode |= LX_ADEC_MIX_BUF3;
	}

	//LLB
	if(pstEventInfo->adecMode & LX_ADEC_LLB_BUF)
	{
		eEventType |= (LX_ADEC_EVENT_DEC0_INTERRUPT | LX_ADEC_EVENT_DEC0_UNDERFLOW);	//temp
		eEventMode |= LX_ADEC_LLB_BUF;
	}

	//Copy of current event id
	pstEventInfo->eventID   = g_ui32RecEventType;
	pstEventInfo->eventMode = g_ui32RecEventMode;

	//spin lock for protection : lock
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);

	//After event handled, clear g_ui32RecEventType only getted
	g_ui32RecEventType &= (~eEventType);	//Clear current event
	g_ui32RecEventMode &= (~eEventMode);	//Clear current adec mode

	//spin lock for protection : unlock
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	ADEC_DEBUG_TMP("ADEC_DRV_GetEvent   : Mode = 0x%X(Event = 0x%X)\n", pstEventInfo->adecMode, pstEventInfo->eventID);
	return RET_OK;
}

/**
 * Gets a current ADEC Buffer Full Information.
 * Returns a CPB information from the LX_ADEC_BUF_INFO_T structure.
 * @see
*/
int ADEC_L9_GetBufferInfo ( LX_ADEC_BUF_INFO_T *pstBufferInfo )
{
	int eRetVal = RET_OK;

	UINT32 	maxMemSize = 0;
	UINT32  freeMemSize = 0;

#ifndef  ENABLE_NETCAST_PRINT
	UINT32	readIndex = 0 ;
	UINT64 ui64TS90kHzTick,ui64PTS90kHzTick  =0;
#else
	UINT32	readIndex = 0;
#endif
	ADEC_BUF_T  *pAdecBuf = NULL;

	//Sanity check
	if(pstBufferInfo == NULL)
	{
		ADEC_ERROR("ADEC_L9_GetBufferInfo : error!!!(pstBufferInfo = %p)\n", pstBufferInfo);
		return RET_ERROR;
	}

	//Clear a buffer information.
	pstBufferInfo->maxMemSize  = 0;
	pstBufferInfo->freeMemSize = 0;
	pstBufferInfo->maxAuiSize  = 0;
	pstBufferInfo->freeAuiSize = 0;

	//Clear a decoding / present notification index information
	pstBufferInfo->currentIndex		   = 0;
	pstBufferInfo->timestamp 		   = 0;
	pstBufferInfo->currentIndexPresent = 0;
	pstBufferInfo->timestampPresent	   = 0;

	//Get a buffer info. from TP input
	if(pstBufferInfo->adecMode == LX_ADEC_DECODE_MAIN)
	{
		//Get a current index count from aud_dec0_frame_cnt register.
		AUD_RdFL(aud_dec0_frame_cnt);
		pstBufferInfo->currentIndex = AUD_Rd(aud_dec0_frame_cnt);

		if(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_TP)
		{
			//Get a buffer status from aud_pes0_free_size register.
			AUD_RdFL(aud_pes0_free_size);
			AUD_Rd01(aud_pes0_free_size, pes0_max_size,  maxMemSize);
			AUD_Rd01(aud_pes0_free_size, pes0_free_size, freeMemSize);

			pstBufferInfo->maxMemSize  = maxMemSize * 1024;
			pstBufferInfo->freeMemSize = freeMemSize;
		}
		else if(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
		{
			//Create a buffer if not created
			eRetVal = ADEC_L9_CreateAudioClipBuffer(pstBufferInfo->adecMode);

			if(eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
				return RET_ERROR;
			}

			//assign a buffer for CPBM
			pAdecBuf = (ADEC_BUF_T *)g_pCPBM_WriterStruct;

			//Get a current index count from aud_pts_index0 register.
			AUD_RdFL(aud_pts_index0);
			readIndex = AUD_Rd(aud_pts_index0);

			//Check a index number
			if(readIndex >= CPB_AUI_INDEX_COUNT)
			{
				ADEC_ERROR("ADEC_L9_GetBufferInfo(LX_ADEC_DEC_0) : readIndex = %d\n", readIndex);

				readIndex = 0;
			}

			//Set a current index count from aud_pts_index0 register.
			pstBufferInfo->currentIndex = AUD_Rd(aud_pts_index0);
			pstBufferInfo->timestamp	= g_adec_kdrv.index[LX_ADEC_DEC_0].AUI[readIndex].streamtime;

			//Get a current index count from aud_in_index_present0 register.
			AUD_RdFL(aud_in_index_present0);
			readIndex = AUD_Rd(aud_in_index_present0);

			//Check a index number
			if(readIndex >= CPB_AUI_INDEX_COUNT)
			{
				ADEC_ERROR("ADEC_L9_GetBufferInfo(LX_ADEC_DEC_0) : Present readIndex = %d\n", readIndex);

				readIndex = 0;
			}

			//Set a current index count from aud_in_index_present0 register.
			pstBufferInfo->currentIndexPresent = AUD_Rd(aud_in_index_present0);
			pstBufferInfo->timestampPresent	   = g_adec_kdrv.index[LX_ADEC_DEC_0].AUI[readIndex].streamtime;

#ifndef  ENABLE_NETCAST_PRINT
			ui64TS90kHzTick = pstBufferInfo->timestamp;

			if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
			{
				ui64TS90kHzTick *= 9;

				//Check divide by zero value.
				if(ui64TS90kHzTick)
				{
					do_div(ui64TS90kHzTick, 100000);	// = Xns * 90 000 / 1 000 000 000
				}
			}

			ui64PTS90kHzTick = pstBufferInfo->timestampPresent;

			if(ui64PTS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
			{
				ui64PTS90kHzTick *= 9;

				//Check divide by zero value.
				if(ui64PTS90kHzTick)
				{
					do_div(ui64PTS90kHzTick, 100000);	// = Xns * 90 000 / 1 000 000 000
				}
			}

			ADEC_MIP_I2C(" [0]D[%llu]%d P[%llu]%d\n" ,ui64TS90kHzTick, pstBufferInfo->currentIndex,ui64PTS90kHzTick,pstBufferInfo->currentIndexPresent);
#endif
		}
	}
	else if(pstBufferInfo->adecMode == LX_ADEC_DECODE_ASSO)
	{
		//Get a current index count from aud_dec1_frame_cnt register.
		AUD_RdFL(aud_dec1_frame_cnt);
		pstBufferInfo->currentIndex = AUD_Rd(aud_dec1_frame_cnt);

		if(g_adec_kdrv.adecSource[LX_ADEC_DEC_1] == LX_ADEC_IN_PORT_TP)
		{
			//Get a buffer status from aud_pes1_free_size register.
			AUD_RdFL(aud_pes1_free_size);
			AUD_Rd01(aud_pes1_free_size, pes1_max_size,  maxMemSize);
			AUD_Rd01(aud_pes1_free_size, pes1_free_size, freeMemSize);

			pstBufferInfo->maxMemSize  = maxMemSize * 1024;
			pstBufferInfo->freeMemSize = freeMemSize;
		}
		else if(g_adec_kdrv.adecSource[LX_ADEC_DEC_1] == LX_ADEC_IN_PORT_SYSTEM)
		{
			//Create a buffer if not created
			eRetVal = ADEC_L9_CreateAudioClipBuffer(pstBufferInfo->adecMode);

			if(eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
				return RET_ERROR;
			}

			//assign a buffer for CPBA
			pAdecBuf = (ADEC_BUF_T *)g_pCPBA_WriterStruct;

			//Get a current index count from aud_pts_index1 register.
			AUD_RdFL(aud_pts_index1);
			readIndex = AUD_Rd(aud_pts_index1);

			//Check a index number
			if(readIndex >= CPB_AUI_INDEX_COUNT)
			{
				ADEC_ERROR("ADEC_L9_GetBufferInfo(LX_ADEC_DEC_1) : readIndex = %d\n", readIndex);

				readIndex = 0;
			}

			//Get a current index count from aud_pts_index1 register.
			pstBufferInfo->currentIndex = AUD_Rd(aud_pts_index1);
			pstBufferInfo->timestamp	= g_adec_kdrv.index[LX_ADEC_DEC_1].AUI[readIndex].streamtime;

			//Get a current index count from aud_in_index_present1 register.
			AUD_RdFL(aud_in_index_present1);
			readIndex = AUD_Rd(aud_in_index_present1);

			//Check a index number
			if(readIndex >= CPB_AUI_INDEX_COUNT)
			{
				ADEC_ERROR("ADEC_L9_GetBufferInfo(LX_ADEC_DEC_1) : Present readIndex = %d\n", readIndex);

				readIndex = 0;
			}

			//Set a current index count from aud_in_index_present1 register.
			pstBufferInfo->currentIndexPresent = AUD_Rd(aud_in_index_present1);
			pstBufferInfo->timestampPresent	   = g_adec_kdrv.index[LX_ADEC_DEC_1].AUI[readIndex].streamtime;
#ifndef  ENABLE_NETCAST_PRINT
			ui64TS90kHzTick = pstBufferInfo->timestamp;

			if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
			{
				ui64TS90kHzTick *= 9;

				//Check divide by zero value.
				if(ui64TS90kHzTick)
				{
					do_div(ui64TS90kHzTick, 100000);	// = Xns * 90 000 / 1 000 000 000
				}
			}

			ui64PTS90kHzTick = pstBufferInfo->timestampPresent;

			if(ui64PTS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
			{
				ui64PTS90kHzTick *= 9;

				//Check divide by zero value.
				if(ui64PTS90kHzTick)
				{
					do_div(ui64PTS90kHzTick, 100000);	// = Xns * 90 000 / 1 000 000 000
				}
			}

			ADEC_MIP_I2C(" [1]D[%llu]%d P[%llu]%d\n" ,ui64TS90kHzTick, pstBufferInfo->currentIndex,ui64PTS90kHzTick,pstBufferInfo->currentIndexPresent);
#endif
		}
	}
	else if(pstBufferInfo->adecMode == LX_ADEC_MIX_BUF0)
	{
		//Create a buffer if not created
		eRetVal = ADEC_L9_CreateAudioClipBuffer(pstBufferInfo->adecMode);

		if(eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
			return RET_ERROR;
		}

		//assign a buffer for MIX0
		pAdecBuf = (ADEC_BUF_T *)g_pMIX0_WriterStruct;

		//Get a current index count from g_adec_kdrv.mixerIndex global value.
		pstBufferInfo->currentIndex = g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_0];
		pstBufferInfo->timestamp	= 0;

		//Get a current index count from aud_in_index_present2 register.
		AUD_RdFL(aud_in_index_present2);
		pstBufferInfo->currentIndexPresent = AUD_Rd(aud_in_index_present2);
		pstBufferInfo->timestampPresent	   = 0;
	}
	else if(pstBufferInfo->adecMode == LX_ADEC_MIX_BUF1)
	{
		//Create a buffer if not created
		eRetVal = ADEC_L9_CreateAudioClipBuffer(pstBufferInfo->adecMode);

		if(eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
			return RET_ERROR;
		}

		pAdecBuf = (ADEC_BUF_T *)g_pMIX1_WriterStruct;

		//Get a current index count from g_adec_kdrv.mixerIndex global value.
		pstBufferInfo->currentIndex = g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_1];
		pstBufferInfo->timestamp	= 0;

		//Get a current index count from aud_in_index_present3 register.
		AUD_RdFL(aud_in_index_present3);
		pstBufferInfo->currentIndexPresent = AUD_Rd(aud_in_index_present3);
		pstBufferInfo->timestampPresent	   = 0;
	}
	else if(pstBufferInfo->adecMode == LX_ADEC_MIX_BUF2)
	{
		//Create a buffer if not created
		eRetVal = ADEC_L9_CreateAudioClipBuffer(pstBufferInfo->adecMode);

		if(eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
			return RET_ERROR;
		}

		pAdecBuf = (ADEC_BUF_T *)g_pMIX2_WriterStruct;

		//Get a current index count from g_adec_kdrv.mixerIndex global value.
		pstBufferInfo->currentIndex = g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_2];
		pstBufferInfo->timestamp	= 0;

		//Get a current index count from aud_in_index_present4 register.
		AUD_RdFL(aud_in_index_present4);
		pstBufferInfo->currentIndexPresent = AUD_Rd(aud_in_index_present4);
		pstBufferInfo->timestampPresent	   = 0;
	}
	else if(pstBufferInfo->adecMode == LX_ADEC_MIX_BUF3)
	{
		//Create a buffer if not created
		eRetVal = ADEC_L9_CreateAudioClipBuffer(pstBufferInfo->adecMode);

		if(eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
			return RET_ERROR;
		}

		pAdecBuf = (ADEC_BUF_T *)g_pMIX3_WriterStruct;

		//Get a current index count from g_adec_kdrv.mixerIndex global value.
		pstBufferInfo->currentIndex = g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_3];
		pstBufferInfo->timestamp	= 0;

		//Get a current index count from aud_in_index_present5 register.
		AUD_RdFL(aud_in_index_present5);
		pstBufferInfo->currentIndexPresent = AUD_Rd(aud_in_index_present5);
		pstBufferInfo->timestampPresent	   = 0;
	}
	else if(pstBufferInfo->adecMode == LX_ADEC_LLB_BUF)
	{
		//Create a buffer if not created
		eRetVal = ADEC_L9_CreateAudioClipBuffer(pstBufferInfo->adecMode);

		if(eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
			return RET_ERROR;
		}

		pAdecBuf = (ADEC_BUF_T *)g_pLLB_WriterStruct;

		//Get a current index count from g_adec_kdrv.mixerIndex global value.
		pstBufferInfo->currentIndex = g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_4];
		pstBufferInfo->timestamp	= 0;

		//Get a current index count from aud_in_index_present5 register.
		AUD_RdFL(aud_in_index_llb);
		pstBufferInfo->currentIndexPresent = AUD_Rd(aud_in_index_llb);
		pstBufferInfo->timestampPresent	   = 0;
	}
	else
	{
		ADEC_ERROR("ADEC_L9_GetBufferInfo : error!!!(adecMode = 0x%X)\n", pstBufferInfo->adecMode);
	}

	//Get a buffer info. from buffer interface
	if(pAdecBuf != NULL)
	{
		pstBufferInfo->maxMemSize  = pAdecBuf->get_max_size(pAdecBuf);
		pstBufferInfo->freeMemSize = pAdecBuf->get_free_size(pAdecBuf);
		pstBufferInfo->maxAuiSize  = pAdecBuf->get_max_au(pAdecBuf);
		pstBufferInfo->freeAuiSize = pAdecBuf->get_max_au(pAdecBuf) - pAdecBuf->get_au_cnt(pAdecBuf) - 1;
	}

	ADEC_DEBUG_TMP("ADEC_L9_GetBufferInfo(adecMode = 0x%X)\n", pstBufferInfo->adecMode);
	return RET_OK;
}

/**
 *  Resets a AAD Module using H/W reset signal.
 *  This function resets a AAD module to clear previous setting.
 * @see
*/
int ADEC_L9_ResetAADModule ( void )
{
	//Reset Low : AAD Module
	ADEC_L9_ClearReset(ADEC_OPTION_AAD);

	//Reset High : AAD Module
	ADEC_L9_SetReset(ADEC_OPTION_AAD);

	ADEC_DEBUG_TMP("ADEC_DRV_ResetAADModule\n");
	return RET_OK;
}

/**
 *  Sets a PCM data shift for boosting AAD(SAI) input.
 *  This function sets a PCM data shift for boosting AAD(SAI) input.
 * @see
*/
int ADEC_L9_SetAADBitShift ( UINT32 ui32BitShift )
{
	//Add protection code if volume gain is 0.
	if(ui32BitShift == 0)
	{
		ui32BitShift = 0x0800000; 	//0 dB SAI Input volume gain
		ADEC_ERROR("ADEC_L9_SetAADBitShift(sai_volgain = 0x%8X)\n", ui32BitShift);
	}

	//Set a shift bit for aud_dshift
	AUD_RdFL(aud_sai_gain);
	AUD_Wr01(aud_sai_gain, sai_volgain, ui32BitShift);	//max 28bit
	AUD_WrFL(aud_sai_gain);

	ADEC_DEBUG("ADEC_L9_SetAADBitShift(sai_volgain = 0x%8X)\n", ui32BitShift);
//	ADEC_ERROR("ADEC_L9_SetAADBitShift(sai_volgain = 0x%8X)\n", ui32BitShift);
	return RET_OK;
}


/**
 * Sets an audio device source to adec module.
 * ADEC module sets the input source, the number of ADC port and the input stream type.
 * @see ADEC_Init
*/
int ADEC_L9_SetSource ( LX_ADEC_SET_SOURCE_T stSetSource )
{
	int		eRetVal = RET_OK;

	UINT8	decNum = 0;

#ifndef ENABLE_DDCO_TEST	//SPDIF PCM Main Only
	BOOLEAN	bNeedToReset = FALSE;
#endif

	KDRV_ADEC_MOD_T 			decMode;
	KDRV_ADEC_DEC_PARAM_T		decParam;

	//Lock a ADEC PCM Semaphore for HDMI Input
	OS_LockMutex(&g_ADEC_Sema_Pcm);

	// Save audio HDMI Input Sampling Frequency for default
	g_adec_kdrv.adecHdmiInputFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;

	//Save a audio source input for protect  to set SPDIF clock
	if(stSetSource.adecMode == LX_ADEC_DECODE_MAIN)
	{
		g_eCurAdecSource = stSetSource.adecSource;
	}

	//Set a audio source for audio decoder
	if(stSetSource.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum  = LX_ADEC_DEC_0;
		decMode = KDRV_ADEC_DEC_0;

		//delete CPBM structure
		if( (stSetSource.adecSource != LX_ADEC_IN_PORT_SYSTEM)
		  &&(g_pCPBM_WriterStruct   != NULL) )
		{
			OS_KFree(g_pCPBM_WriterStruct);
			g_pCPBM_WriterStruct = NULL;
		}
	}
	else if(stSetSource.adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum  = LX_ADEC_DEC_1;
		decMode = KDRV_ADEC_DEC_1;

		//delete CPBA structure
		if( (stSetSource.adecSource != LX_ADEC_IN_PORT_SYSTEM)
		  &&(g_pCPBA_WriterStruct   != NULL) )
		{
			OS_KFree(g_pCPBA_WriterStruct);
			g_pCPBA_WriterStruct = NULL;
		}
	}
	else
	{
		decMode = KDRV_ADEC_DEC_0;
		ADEC_ERROR("ADEC_L9_SetSource : adecMode = 0x%X!!!\n", stSetSource.adecMode);
	}

	//Set a audio source for mixed IP block
	if(stSetSource.adecMode == LX_ADEC_DECODE_MAIN)
	{
		//Set clock source only L9 A0/A1 chip
		if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		{
			if( stSetSource.adecSource == LX_ADEC_IN_PORT_SIF )
			{
				(void)MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_CLOCK_ATV );
			}
			else if(stSetSource.adecSource == LX_ADEC_IN_PORT_HDMI)
			{
				// Save audio HDMI Input Sampling Frequency
				g_adec_kdrv.adecHdmiInputFreq = stSetSource.samplingFreq;

				(void)MIXED_IP_ADEC_L9_SetClockSource( stSetSource.samplingFreq, LX_ADEC_CLOCK_HDMI);
			}
			else if( stSetSource.adecSource == LX_ADEC_IN_PORT_ADC )
			{
				(void)MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_CLOCK_ADC );
			}
			else
			{
				(void)MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_CLOCK_DTV );
			}
		}
		else
		{
			if( (stSetSource.adecSource == LX_ADEC_IN_PORT_SIF)
			  ||(stSetSource.adecSource == LX_ADEC_IN_PORT_ADC)
			  ||(stSetSource.adecSource == LX_ADEC_IN_PORT_HDMI) )
			{
				(void)MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_CLOCK_ATV );
			}
			else
			{
				(void)MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_CLOCK_DTV );
			}
		}
	}

	//Unlock a ADEC PCM Semaphore for HDMI Input
	OS_UnlockMutex(&g_ADEC_Sema_Pcm);

#ifndef ENABLE_DDCO_TEST	//SPDIF PCM Main Only
	if( (g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB)	\
	  &&(stSetSource.adecMode == LX_ADEC_DECODE_MAIN) )
	{
		//Check a Not LX_ADEC_IN_PORT_SYSTEM input and previous status
		if( (stSetSource.adecSource != LX_ADEC_IN_PORT_SYSTEM)
		  &&(g_adec_kdrv.bSpdifMainOnly == FALSE) )
		{
			//Set a need to reset flag
			bNeedToReset = TRUE;

			//Set a SPDIF output mode
			AUD_RdFL(aud_set_inout);

			//Set SPDIF output mode : PCM Main Only Output mode for Audio Description in DVB model
			AUD_Wr01(aud_set_inout, spdif_mode, 0x1);	//Main Only

			AUD_WrFL(aud_set_inout);

			/* Update Internal Status Value */
			g_adec_kdrv.bSpdifMainOnly = TRUE;
		}

		//Check a Not LX_ADEC_IN_PORT_SYSTEM input and previous status
		if( (stSetSource.adecSource == LX_ADEC_IN_PORT_SYSTEM)
		  &&(g_adec_kdrv.bSpdifMainOnly == TRUE) )
		{
			//Set a need to reset flag
			bNeedToReset = TRUE;

			//Set a SPDIF output mode
			AUD_RdFL(aud_set_inout);

			//Set SPDIF output mode : Normal PCM Mixed Output
			AUD_Wr01(aud_set_inout, spdif_mode, 0x0);	//Normal

			AUD_WrFL(aud_set_inout);

			/* Update Internal Status Value */
			g_adec_kdrv.bSpdifMainOnly = FALSE;
		}

		//Reset a DSP0/DSP1 to change SPDIF mode
		if(bNeedToReset == TRUE)
		{
			//Change a global current codec to re-load codec
			g_eCurLoadedCodec = LX_ADEC_SRC_TYPE_NONE;

			//Load DSP1 codec for audio processing
			eRetVal = ADEC_L9_LoadDSP1Codec();
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_L9_SetSource : Error in ADEC_L9_LoadDSP1Codec call\n");
			}

			//Set codec source by adec type
			eRetVal = ADEC_L9_LoadDSP0Codec(stSetSource.adecType);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_L9_SetSource : Error in ADEC_L9_LoadDSP0Codec call(type = 0x%x)\n", stSetSource.adecType);
			}
		}
	}
#endif	//#ifndef ENABLE_DDCO_TEST

	//Set I2S and 24 bit mode for I2S input for default
	AUD_RdFL(aud_set_inout);
	AUD_Wr01(aud_set_inout, in_format, 0x0);	//I2S
	AUD_Wr01(aud_set_inout, in_res, 0x3);		//24 bit

	//Set a SCART bypass mode in Main decoder only
	if( (g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_DVB)	\
	  &&(stSetSource.adecMode == LX_ADEC_DECODE_MAIN) )
	{
		if( (stSetSource.adecSource == LX_ADEC_IN_PORT_SIF)	\
		  ||(stSetSource.adecSource == LX_ADEC_IN_PORT_ADC)	\
		  ||(stSetSource.adecSource == LX_ADEC_IN_PORT_HDMI) )
		{
			//Set AAD bypass for SCART port
			AUD_Wr01(aud_set_inout, scart_out_ctrl, 0x1);
		}
		else
		{
			//Clear AAD bypass for SCART port
			AUD_Wr01(aud_set_inout, scart_out_ctrl, 0x0);
		}
	}

	//Get input port and codec type for IPC command
	ADEC_IPC_GetConfig(decMode, &decParam);

	//Set input port and codec type for IPC command
	decParam.samplingFreq  = stSetSource.samplingFreq;
	decParam.adecCodecType = ADEC_ConvertCodecType(stSetSource.adecType);
	decParam.ui32Param[0]  = NUM_OF_PARAM_NONE_INFO;

	switch (stSetSource.adecSource)
	{
		case LX_ADEC_IN_PORT_NONE:
			decParam.adecInputPort = 0;
			break;

		case LX_ADEC_IN_PORT_TP:
			decParam.adecInputPort = KDRV_ADEC_INPUT_TP_0;
			break;

		case LX_ADEC_IN_PORT_SPDIF:
			decParam.adecInputPort = KDRV_ADEC_INPUT_SI_0;
			break;

		case LX_ADEC_IN_PORT_SIF:
			decParam.adecInputPort = KDRV_ADEC_INPUT_SIF;
			break;

		case LX_ADEC_IN_PORT_ADC:
			decParam.adecInputPort = KDRV_ADEC_INPUT_ADC;

			//Set ADC Input port for Mixed IP Module
			(void)MIXED_IP_ADEC_L9_SetADCPortNumber(stSetSource.portNum);

			//Set I2S and 24 bit mode for I2S input(L/R change)
			if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
			{
				//24 bit(0x03), SCLK polarity invert.(0x04)
				AUD_Wr01(aud_set_inout, in_res, 0x7);
			}
			break;

		case LX_ADEC_IN_PORT_HDMI:
			decParam.adecInputPort = KDRV_ADEC_INPUT_HDMI;
			break;

		case LX_ADEC_IN_PORT_I2S:
			decParam.adecInputPort = KDRV_ADEC_INPUT_I2S;
			break;

		case LX_ADEC_IN_PORT_SYSTEM:
			/* Clear aui count */
			g_adec_kdrv.index[decNum].FeedCNT = 0;
			g_adec_kdrv.index[decNum].FeedIDX = 0;

			g_adec_kdrv.index[decNum].PrevNOTI = INITIAL_CONDITION ;
			g_adec_kdrv.index[decNum].NewNOTI  = 0 ;
			g_adec_kdrv.index[decNum].WriteCNT = INITIAL_CONDITION;
			g_adec_kdrv.index[decNum].ReadCNT  = 0;
		 	g_adec_kdrv.index[decNum].FreeCNT  = CPB_AUI_INDEX_COUNT;
		 	g_adec_kdrv.index[decNum].UsedCNT  = 0;
		 	g_adec_kdrv.index[decNum].NotiCNT  = 0;

			if(stSetSource.adecMode == LX_ADEC_DECODE_MAIN)
			{
				decParam.adecInputPort = KDRV_ADEC_INPUT_SYSTEM_0;
			}
			else
			{
				decParam.adecInputPort = KDRV_ADEC_INPUT_SYSTEM_1;
			}

			//Create a buffer if not created
			eRetVal = ADEC_L9_CreateAudioClipBuffer(stSetSource.adecMode);
			break;

		case LX_ADEC_IN_PORT_MIC:
			decParam.adecInputPort = 0;
			break;

		case LX_ADEC_IN_PORT_MIRROR:
			ADEC_DEBUG("ADEC_DRV_SetSource : LX_ADEC_IN_PORT_MIRROR!\n");
			return eRetVal;

		default:
			ADEC_ERROR("ADEC_DRV_SetSource : Invalid Params!\n");
			eRetVal = RET_INVALID_PARAMS;
			return eRetVal;
	}

	//Set input port and codec type for IPC command
	ADEC_IPC_SetConfig(decMode, &decParam);

	//Write Input / Output port reg.
	AUD_WrFL(aud_set_inout);

	// Save audio source input
	g_adec_kdrv.adecSource[decNum] = stSetSource.adecSource;

	// Save audio codec type for aac codec parsing
	g_adec_kdrv.adecType[decNum] = stSetSource.adecType;

	//D-AMP Mute Control
#ifdef ENABLE_DAMP_VOLUME_CONTROL
	//Initialize D-AMP NTP7400(I2C CH. 0)
	ADEC_NTP7400_Init();

	//Set a D-AMP volume for I2C NTP7400 Device
	//(void)ADEC_NTP7400_SetVolume(15);

	//Set a D-AMP mute off for I2C NTP7400 Device
	//(void)ADEC_NTP7400_SetMute(FALSE);

	//Disable a sound engine
	ADEC_IPC_SetDisableSeEngine();
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("ADEC_DRV_SetSource : mode = %d, src = %d, port = %d, type = %d, freq = %d\n",	\
				stSetSource.adecMode, stSetSource.adecSource, stSetSource.portNum, stSetSource.adecType, stSetSource.samplingFreq);
#else
	ADEC_ERROR("[%s]ADEC_DRV_SetSource : mode = %d, src = %d, port = %d, type = %s, freq = %d\n",	\
				sourcetype[stSetSource.adecMode],stSetSource.adecMode, stSetSource.adecSource, stSetSource.portNum, codectype[stSetSource.adecType], stSetSource.samplingFreq);
#endif

	return eRetVal;
}


/**
 * Starts the decoding of the DTV Stream.
 * The source types defined in enum LX_ADEC_SRC_TYPE_T must be reset.
 * @see ADEC_DRV_StopDecoding
*/
int ADEC_L9_StartDecoding ( LX_ADEC_START_DEC_T stStartInfo )
{
	int		eRetVal = RET_OK;

	UINT8	decNum = 0;

	KDRV_ADEC_MOD_T 			module;
	KDRV_ADEC_MOD_T 			adecMixerModule;

	LX_ADEC_SRC_TYPE_T			eSrcType;

	KDRV_ADEC_DEC_PARAM_T		decParam;
	KDRV_ADEC_AC3_INFO_T 		ac3Info;
	KDRV_ADEC_AAC_INFO_T 		aacInfo;

	ADEC_CMD_SYNC_BASE_T 		syncBase;
	KDRV_ADEC_LIPSYNC_PARAM_T 	lipsyncParam;

	ADEC_MIX_EXT_PARAM_T 		mixExtParam;

	//Check a adec mode
	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum = LX_ADEC_DEC_0;
		module = KDRV_ADEC_DEC_0;

		adecMixerModule = KDRV_ADEC_MP_00;
	}
	else if(stStartInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum = LX_ADEC_DEC_1;
		module = KDRV_ADEC_DEC_1;

		adecMixerModule = KDRV_ADEC_MP_01;
	}
	else
	{
		decNum = LX_ADEC_DEC_0;
		module = KDRV_ADEC_DEC_0;

		adecMixerModule = KDRV_ADEC_MP_00;

		ADEC_ERROR("StartDecoding : adecMode = 0x%X!!!\n", stStartInfo.adecMode);
	}

	// Copy adec codec type
	eSrcType = stStartInfo.adecType;

	// Save audio codec type
	g_adec_kdrv.adecType[decNum] = stStartInfo.adecType;

	//Set a SPDIF output type
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Set a SPDIF output type for GCD platform
		eRetVal = ADEC_SetSPDIFOutputTypeForDual(stStartInfo.adecMode);
		if (eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_SetSPDIFOutputTypeForDual : Error! adecMode = 0x%x\n", stStartInfo.adecMode);
		}
	}
	else
	{
		/* SPDIF Output Decoding Mode 설정 : TP, EMP및 HDMI 출력 관련 강제 세팅  */
		eRetVal = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
		if (eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", stStartInfo.adecType);
		}
	}

	//Set codec source by adec type
	eRetVal = ADEC_L9_LoadDSP0Codec(eSrcType);
	if (eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_DRV_StartDecoding : Error!  eSrcType = 0x%x\n", eSrcType);
		return RET_ERROR;
	}

	//Get input port and codec type for IPC command
	ADEC_IPC_GetConfig(module, &decParam);

	decParam.adecCodecType = ADEC_ConvertCodecType(eSrcType);
	decParam.ui32Param[0]  = NUM_OF_PARAM_NONE_INFO;

	//Check a DRC, Downmix mode for AC-3(EAC-3) or AAC(HEAAC) decoding
	if(g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_TP)
	{
		if(eSrcType == LX_ADEC_SRC_TYPE_AC3 || eSrcType == LX_ADEC_SRC_TYPE_EAC3)
		{
			ac3Info.numOfParam 	= NUM_OF_PARAM_AC3_INFO;
			ac3Info.drcMode 	= g_adec_kdrv.drcMode[decNum];
			ac3Info.downmixMode = g_adec_kdrv.downmixMode[decNum];
			memcpy(&(decParam.ui32Param[0]), &ac3Info, sizeof(KDRV_ADEC_AC3_INFO_T));
			ADEC_DEBUG("DDC : DRC = %d, Downmix = %d Selected.\n", ac3Info.drcMode, ac3Info.downmixMode);
		}
		else if(eSrcType == LX_ADEC_SRC_TYPE_AAC || eSrcType == LX_ADEC_SRC_TYPE_HEAAC)
		{
			aacInfo.numOfParam 	= NUM_OF_PARAM_AAC_INFO;
			aacInfo.drcMode 	= g_adec_kdrv.drcMode[decNum];
			aacInfo.downmixMode = g_adec_kdrv.downmixMode[decNum];
			memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));
			ADEC_DEBUG("DDT : DRC = %d, Downmix = %d Selected.\n", aacInfo.drcMode, aacInfo.downmixMode);
		}
	}

	ADEC_IPC_SetConfig(module, &decParam);

	//Get a lip sync paramter by adec codec type
	ADEC_IPC_GetLipsync(module, &lipsyncParam);

	//Lip sync adjust : 2012.01.05, +100(vdec delay change, 2011.01.06)
	if( eSrcType == LX_ADEC_SRC_TYPE_MPEG )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 202 + VDEC_TP_LIPSYNC_OFFSET;		//0xF6 - 0x2C = 0xCA(202)
	}
	else if( eSrcType == LX_ADEC_SRC_TYPE_AC3 || eSrcType == LX_ADEC_SRC_TYPE_EAC3 )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 171 + VDEC_TP_LIPSYNC_OFFSET;		//0xCC - 0x21 = 0xAB(171)
	}
	else if( eSrcType == LX_ADEC_SRC_TYPE_AAC )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 120 + VDEC_TP_LIPSYNC_OFFSET;		//2002.02.06 by suji.baek
	}
	else if( eSrcType == LX_ADEC_SRC_TYPE_HEAAC )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 167 + VDEC_TP_LIPSYNC_OFFSET;		//0xE7 - 0x40 = 0xA7(167)
	}

	//Change a lip sync offset to compensate DDCO output delay
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		if(lipsyncParam.offset > DDCO_LIPSYNC_OFFSET_DELAY)
			lipsyncParam.offset -= DDCO_LIPSYNC_OFFSET_DELAY;	//30 ms
	}

	lipsyncParam.freerunubound = 5000;	// 5 sec, 0x1388(5000)
	lipsyncParam.freerunlbound = 5000;	// 5 sec, 0x1388(5000)

	ADEC_IPC_SetLipsync(module, &lipsyncParam);

	// Set a MHEG TP stream lipsync for DEC0
	// 0x0XXXXXXX : Now set BaseTime, 0x1FFFFFFF : No PCR, 0x2FFFFFFF : PCR mode.
	if( g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_TP )
	{
		if(stStartInfo.bPcrExist == FALSE)
		{
			// 0x1FFFFFFF : No PCR
			//Get lip sync parameter for IPC command
			ADEC_IPC_GetLipsync(module, &lipsyncParam);

			//Set a lip sync bound
			lipsyncParam.ubound		   = 100;		//0x64(100)
			lipsyncParam.lbound		   = 100;		//0x64(100)
			lipsyncParam.offset		   = 900;		//0x384(900), this value must be matched.(NetCast : gstvdecsync, GCD : vdec LGMD)
			lipsyncParam.freerunubound = 5000;		// 5 sec, 0x1388(5000)
			lipsyncParam.freerunlbound = 120000;	// 120 sec, 0x1D4C0(120000)

			//Set lip sync parameters
			ADEC_IPC_SetLipsync(module, &lipsyncParam);

			syncBase.module     = module;
			syncBase.clockBase  = 0x1FFFFFFF;
			syncBase.streamBase = 0x1FFFFFFF;
		}
		else
		{
			syncBase.module     = module;
			syncBase.clockBase  = 0x2FFFFFFF;
			syncBase.streamBase = 0x2FFFFFFF;
		}

		ADEC_IPC_SetBaseTime(syncBase);
	}

	//Set a Mixer Start Mute Time for ADC Input to fix pop-noise in ADC port from TP when Output Delay is changed.
	if(decNum == LX_ADEC_DEC_0 || decNum == LX_ADEC_DEC_1)
	{
		//Get a Mixer Ext. Parameters.
		ADEC_IPC_GetMixExtParam(&mixExtParam);

		// Set a Mix 0 port for DEC0 or DEC1
		if(decNum == LX_ADEC_DEC_0)
		{
			mixExtParam.mixPort	= 0;
		}
		else
		{
			mixExtParam.mixPort	= 1;
		}

		if( (eSrcType == LX_ADEC_SRC_TYPE_PCM)
		  &&(decParam.adecInputPort == KDRV_ADEC_INPUT_ADC) )
		{
			//Mixer Initial Mute Delay : 23*5 = 115ms(0 -> 5120)
			mixExtParam.initialMuteLength = FADE_IN_OUT_MIX_DELAY_115MS;
		}
		else
		{
			//Mixer Initial Mute Delay : 0 ms(0)
			mixExtParam.initialMuteLength = FADE_IN_OUT_MIX_DELAY_0MS;
		}

		//Set a Mixer Ext. Parameters.
		ADEC_IPC_SetMixExtParam(mixExtParam);
	}

	ADEC_IPC_Start(module);

	//Set a Mixer start for GCD platform to fix pop-noise in ADC port
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		if( (eSrcType == LX_ADEC_SRC_TYPE_PCM)
		  &&(decParam.adecInputPort != KDRV_ADEC_INPUT_HDMI) )
		{
			//Wait 50ms for start DSP 0 codec.
			msleep_interruptible(DSP_STOP_DELAY_10MS * 5);
		}

		//Send a IPC command
		ADEC_IPC_Start(adecMixerModule);
	}

	/* Update Internal Status Value */
	g_adec_kdrv.bDecStarted[decNum] = TRUE;

	//Sound Engine Control
#ifdef ENABLE_DAMP_VOLUME_CONTROL
	//Disable a sound engine
	ADEC_IPC_SetDisableSeEngine();
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("ADEC_DRV_StartDecoding : Started(ad_exist = %d, bPcrExist = %d)\n", stStartInfo.bAdExist, stStartInfo.bPcrExist);
#else
	ADEC_ERROR("[%s]ADEC_DRV_StartDecoding : Started(ad_exist = %d, bPcrExist = %d)\n", sourcetype[stStartInfo.adecMode],stStartInfo.bAdExist, stStartInfo.bPcrExist);
#endif

	return eRetVal;
}


/**
 * Stops the decoding of the DTV Stream.
 * This function also mutes audio output.
 * @see ADEC_DRV_StartDecoding
*/
int ADEC_L9_StopDecoding ( LX_ADEC_STOP_DEC_T stStopInfo )
{
	int		eRetVal = RET_OK;

	UINT8	decNum = 0;

	KDRV_ADEC_MOD_T 	module = 0;
	KDRV_ADEC_MOD_T 	adecMixerModule;

	//Check a adec mode
	if(stStopInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum = LX_ADEC_DEC_0;
		module = KDRV_ADEC_DEC_0;

		adecMixerModule = KDRV_ADEC_MP_00;
	}
	else if(stStopInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum = LX_ADEC_DEC_1;
		module = KDRV_ADEC_DEC_1;

		adecMixerModule = KDRV_ADEC_MP_01;
	}
	else
	{
		decNum = LX_ADEC_DEC_0;
		module = KDRV_ADEC_DEC_0;

		adecMixerModule = KDRV_ADEC_MP_00;

		ADEC_ERROR("StopDecoding : adecMode = 0x%X!!!\n", stStopInfo.adecMode);
	}

	//Set a Mixer stop for GCD platform
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Send a IPC command
		ADEC_IPC_Stop(adecMixerModule);
	}

	ADEC_IPC_Stop(module);

	// for MHEG TP stream lipsync
	// Set DEC0 Clock Basetime
	if( g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_TP )
	{
		AUD_Wr(aud_dec0_cbt, 0xFFFFFFFF);
		AUD_WrFL(aud_dec0_cbt);

		// Set DEC0 Stream Basetime
		AUD_Wr(aud_dec0_sbt, 0xFFFFFFFF);
		AUD_WrFL(aud_dec0_sbt);
	}

	/* Update Internal Status Value */
	g_adec_kdrv.bDecStarted[decNum] = FALSE;

	//Set a SPDIF output type for dual decoding case
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Set a SPDIF output type for GCD platform
		if(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
		{
			eRetVal = ADEC_SetSPDIFOutputTypeForDual(LX_ADEC_DECODE_MAIN);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputTypeForDual : Error! adecMode = 0x%x\n", LX_ADEC_DECODE_MAIN);
			}
		}
		else if(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE)
		{
			eRetVal = ADEC_SetSPDIFOutputTypeForDual(LX_ADEC_DECODE_ASSO);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputTypeForDual : Error! adecMode = 0x%x\n", LX_ADEC_DECODE_ASSO);
			}
		}
	}

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("ADEC_L9_StopDecoding : Stopped\n");
#else
	ADEC_ERROR("[%s]ADEC_L9_StopDecoding : Stopped\n", sourcetype[stStopInfo.adecMode]);
#endif

	return eRetVal;
}

/**
 * Gets a current ADEC decoding information.
 * Returns a decoding information from the LX_ADEC_DEC_INFO_T structure.
 * @see
*/
int ADEC_L9_GetDecodingInfo ( LX_ADEC_DEC_INFO_T *pstDecodingInfo )
{
	UINT8	decNum = LX_ADEC_DEC_0;

	UINT32 i= 0;
	UINT32 u32MAX_AUI;
	UINT32 u32MIN_AUI;
	UINT32 u32ReadedAUI;
	UINT32 u32IsWarpAroundCase = 0;
	UINT32 u32CopyNumA= 0;
	UINT32 u32CopyNumB= 0;
	ULONG	flagsNoti = 0;

	//Sanity check
	if(pstDecodingInfo == NULL)
	{
		ADEC_ERROR("ADEC_L9_GetDecodingInfo : error!!!(pstDecodingInfo = %p)\n", pstDecodingInfo);
		return RET_ERROR;
	}

	//Check a adec mode
	if(pstDecodingInfo->adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum = LX_ADEC_DEC_0;
	}
	else if(pstDecodingInfo->adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum = LX_ADEC_DEC_1;
	}
	else
	{
		decNum = LX_ADEC_DEC_0;
		ADEC_ERROR("GetDecodingInfo : adecMode = 0x%X!!!\n", pstDecodingInfo->adecMode);
	}

	/* Get a codec source type if type is defined value from adec_kapi.h */
	pstDecodingInfo->adecSource = g_adec_kdrv.adecSource[decNum];
	pstDecodingInfo->adecType   = g_adec_kdrv.adecType[decNum];
	pstDecodingInfo->spdifPcm   = g_adec_kdrv.bSpdifPcm;

	if(g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_SYSTEM)
	{
		pstDecodingInfo->decodedCount = 0;

		//spin lock for protection : lock
		spin_lock_irqsave(&gADEC_DoneNoti_lock, flagsNoti);

		// if 8 more AUI is queued in queue, read 8 AUI in a once
		if( g_adec_kdrv.index[decNum].UsedCNT >= (MAX_TRANSFER_NUM_MINUSONE_OF_INDEX +1) )
		{
			u32ReadedAUI = (MAX_TRANSFER_NUM_MINUSONE_OF_INDEX +1);

			u32MIN_AUI = g_adec_kdrv.index[decNum].ReadCNT;
			u32MAX_AUI = (g_adec_kdrv.index[decNum].ReadCNT + (u32ReadedAUI-1) )%CPB_AUI_INDEX_COUNT ;

			g_adec_kdrv.index[decNum].ReadCNT  = (g_adec_kdrv.index[decNum].ReadCNT +u32ReadedAUI) % CPB_AUI_INDEX_COUNT;
			g_adec_kdrv.index[decNum].UsedCNT -= u32ReadedAUI;
			g_adec_kdrv.index[decNum].FreeCNT += u32ReadedAUI;

			//spin lock for protection : unlock
			spin_unlock_irqrestore(&gADEC_DoneNoti_lock, flagsNoti);

			pstDecodingInfo->decodedCount = u32ReadedAUI;

			if(u32MAX_AUI < u32MIN_AUI)	//ex> u32MIN_AUI:510 u32MAX_AUI: 2
			{
				u32IsWarpAroundCase = 1;
			}

			ADEC_PRINT("RRD[%d-%dN%dF%dU%dW%3dR%3d]\n", u32MIN_AUI, u32MAX_AUI, 	\
						pstDecodingInfo->decodedCount, g_adec_kdrv.index[decNum].FreeCNT, 	\
						g_adec_kdrv.index[decNum].UsedCNT, g_adec_kdrv.index[decNum].WriteCNT, 	\
						g_adec_kdrv.index[decNum].ReadCNT);
			ADEC_IPC_DEBUG("R%d:%d\n", u32MIN_AUI, u32MAX_AUI);

			if( u32IsWarpAroundCase )
			{
				u32CopyNumA = CPB_AUI_INDEX_COUNT - u32MIN_AUI;
				u32CopyNumB = u32MAX_AUI +1;

				if (u32CopyNumA + u32CopyNumB > MAX_TRANSFER_NUM_OF_INDEX )
				{
					ADEC_ERROR("DONE NOTIFICATION NUM ERROR\n");
					return RET_ERROR;
				}

				for(i= 0; i< u32CopyNumA; i++ )// 1021, 1022, 1023
				{
					memcpy(&pstDecodingInfo->decInfo[i],&g_adec_kdrv.index[decNum].AUI[u32MIN_AUI+i] ,sizeof (KDRV_AUI_INFO) );
				}
				for(i= 0; i< u32CopyNumB; i++ )// 0, 1
				{
					memcpy(&pstDecodingInfo->decInfo[u32CopyNumA+i],&g_adec_kdrv.index[decNum].AUI[i] ,sizeof (KDRV_AUI_INFO) );
				}
			}
			else
			{
				for(i= 0; i< u32ReadedAUI; i++ )
				{
					memcpy(&pstDecodingInfo->decInfo[i],&g_adec_kdrv.index[decNum].AUI[u32MIN_AUI+i] ,sizeof (KDRV_AUI_INFO) );
				}
			}
		}
		else if (g_adec_kdrv.index[decNum].UsedCNT > 0)
		{
			u32ReadedAUI =g_adec_kdrv.index[decNum].UsedCNT;
			u32MIN_AUI = g_adec_kdrv.index[decNum].ReadCNT ;
			u32MAX_AUI = (g_adec_kdrv.index[decNum].ReadCNT + (u32ReadedAUI -1))%CPB_AUI_INDEX_COUNT ;

			g_adec_kdrv.index[decNum].ReadCNT  = (g_adec_kdrv.index[decNum].ReadCNT +u32ReadedAUI) % CPB_AUI_INDEX_COUNT;
			g_adec_kdrv.index[decNum].FreeCNT += u32ReadedAUI;

			g_adec_kdrv.index[decNum].UsedCNT -= u32ReadedAUI;

			//spin lock for protection : unlock
			spin_unlock_irqrestore(&gADEC_DoneNoti_lock, flagsNoti);

			pstDecodingInfo->decodedCount = u32ReadedAUI;//(UINT8)((u32MAX_AUI -u32MIN_AUI) +1);

			if(u32MAX_AUI < u32MIN_AUI)	//ex> u32MIN_AUI:510 u32MAX_AUI: 2
			{
				u32IsWarpAroundCase = 1;
			}

			ADEC_PRINT("RD[%d-%dN%dF%dU%dW%3dR%3d]\n", 	\
						u32MIN_AUI, u32MAX_AUI, pstDecodingInfo->decodedCount, 	\
						g_adec_kdrv.index[decNum].FreeCNT, g_adec_kdrv.index[decNum].UsedCNT, 	\
						g_adec_kdrv.index[decNum].WriteCNT, g_adec_kdrv.index[decNum].ReadCNT);
			ADEC_IPC_DEBUG("%d:%d\n", u32MIN_AUI, u32MAX_AUI);

			if( u32IsWarpAroundCase )
			{
				u32CopyNumA = CPB_AUI_INDEX_COUNT - u32MIN_AUI;
				u32CopyNumB = u32MAX_AUI +1;

				if (u32CopyNumA + u32CopyNumB  > MAX_TRANSFER_NUM_OF_INDEX )
				{
					ADEC_ERROR("DONE NOTIFICATION NUM ERROR\n");
					return RET_ERROR;
				}

				// 1021, 1022, 1023
				for(i= 0; i< u32CopyNumA; i++ )
				{
					memcpy(&pstDecodingInfo->decInfo[i],&g_adec_kdrv.index[decNum].AUI[u32MIN_AUI+i] ,sizeof (KDRV_AUI_INFO) );
				}

				// 0, 1
				for(i= 0; i< u32CopyNumB; i++ )
				{
					memcpy(&pstDecodingInfo->decInfo[u32CopyNumA+i],&g_adec_kdrv.index[decNum].AUI[i] ,sizeof (KDRV_AUI_INFO) );
				}
			}
			else
			{
				for(i= 0; i< u32ReadedAUI ; i++ )
				{
					memcpy(&pstDecodingInfo->decInfo[i],&g_adec_kdrv.index[decNum].AUI[u32MIN_AUI+i] ,sizeof (KDRV_AUI_INFO) );
				}
			}
		}
		else
		{
			//spin lock for protection : unlock
			spin_unlock_irqrestore(&gADEC_DoneNoti_lock, flagsNoti);
			ADEC_PRINT("KDRV AUI UNDERFLOW\n");
			return RET_OK;
		}
	}

	ADEC_DEBUG_TMP("ADEC_L9_GetDecodingInfo : mode = 0x%X\n", pstDecodingInfo->adecMode);
	return RET_OK;
}


/**
 * Checks whether audio ES(Elementary Stream) exists or not.
 * This function is used for checking the existence of the current ES.
 * @see ADEC_DRV_GetESInfo
*/
int ADEC_L9_GetESExistence ( LX_ADEC_ES_EXIST_T *pstEsExist )
{
	UINT32 esExist = 0;

	//Sanity check
	if(pstEsExist == NULL)
	{
		ADEC_ERROR("ADEC_L9_GetESExistence : error!!!(pstEsExist = %p)\n", pstEsExist);
		return RET_ERROR;
	}

	//Check a adec mode
	if(pstEsExist->adecMode == LX_ADEC_DECODE_MAIN)
	{
		//ES header is exist
		AUD_RdFL(aud_decstat0);
		AUD_Rd01(aud_decstat0, es_existence, esExist);
	}
	else if(pstEsExist->adecMode == LX_ADEC_DECODE_ASSO)
	{
		//ES header is exist
		AUD_RdFL(aud_decstat1);
		AUD_Rd01(aud_decstat1, es_existence, esExist);
	}
	else
	{
		//ES header is exist
		AUD_RdFL(aud_decstat0);
		AUD_Rd01(aud_decstat0, es_existence, esExist);

		ADEC_ERROR("ADEC_L9_GetESExistence : adecMode = 0x%X!!!\n", pstEsExist->adecMode);
	}

	if(esExist == 1)
	{
		pstEsExist->bEsExist = TRUE;
	}
	else
	{
		pstEsExist->bEsExist = FALSE;
	}

	if((g_ui32AdecPrintCount++ % DEBUG_MSG_PRINT_TIME) == 0)	//DDI calls every 30 ms in DVB mode or 20 ms in ATSC mode
		ADEC_PRINT("ADEC_L9_GetESExistence : %d\n", pstEsExist->bEsExist);

	return RET_OK;
}

/**
 * Gets the audio ES(Elementatry Stream) information such as HEAAC, MPEG and AC3.
 * ES info may be added other information further.
 * @see ADEC_DRV_GetESExistence
*/
int ADEC_L9_GetESInfo ( LX_ADEC_GET_ES_INFO_T *pstESInfo )
{
	UINT32 esInfo0 = 0, esInfo1 = 0, esInfo2 = 0, esInfo3 = 0;

	LX_ADEC_SRC_TYPE_T 	eSrcType = LX_ADEC_SRC_TYPE_NONE;

	//Check a adec mode
	if(pstESInfo->adecMode == LX_ADEC_DECODE_MAIN)
	{
		// Get a decoding status by codec type.
		AUD_RdFL(aud_decstat0);
		AUD_Rd01(aud_decstat0, es_info0, esInfo0);
		AUD_Rd01(aud_decstat0, es_info1, esInfo1);
		AUD_Rd01(aud_decstat0, es_info2, esInfo2);
		AUD_Rd01(aud_decstat0, es_info3, esInfo3);

		// Get a current adec source type.
		eSrcType = g_adec_kdrv.adecType[LX_ADEC_DEC_0];
	}
	else if(pstESInfo->adecMode == LX_ADEC_DECODE_ASSO)
	{
		// Get a decoding status by codec type.
		AUD_RdFL(aud_decstat1);
		AUD_Rd01(aud_decstat1, es_info0, esInfo0);
		AUD_Rd01(aud_decstat1, es_info1, esInfo1);
		AUD_Rd01(aud_decstat1, es_info2, esInfo2);
		AUD_Rd01(aud_decstat1, es_info3, esInfo3);

		// Get a current adec source type.
		eSrcType = g_adec_kdrv.adecType[LX_ADEC_DEC_1];
	}
	else
	{
		// Get a decoding status by codec type.
		AUD_RdFL(aud_decstat0);
		AUD_Rd01(aud_decstat0, es_info0, esInfo0);
		AUD_Rd01(aud_decstat0, es_info1, esInfo1);
		AUD_Rd01(aud_decstat0, es_info2, esInfo2);
		AUD_Rd01(aud_decstat0, es_info3, esInfo3);

		// Get a current adec source type.
		eSrcType = g_adec_kdrv.adecType[LX_ADEC_DEC_0];

		ADEC_ERROR("ADEC_L9_GetESInfo : adecMode = 0x%X!!!\n", pstESInfo->adecMode);
	}

	// Copy a adec source codec type
	pstESInfo->esInfo.adecFormat = eSrcType;

	// Change a decdoing info for DDI defined value.
	if (eSrcType == LX_ADEC_SRC_TYPE_AAC || eSrcType == LX_ADEC_SRC_TYPE_HEAAC)
	{
		pstESInfo->esInfo.heAAcEsInfo.version 			  = esInfo0; //HE-AAC Version, 0:AAC, 1:HE-AAC V1, 2:HE-AAC V2
		pstESInfo->esInfo.heAAcEsInfo.transmissionformat  = esInfo1; //HE-AAC transmission format, 0:LOAS/LATM, 1:ADTS
		pstESInfo->esInfo.heAAcEsInfo.channelNum 		  = esInfo2; //HE-AAC Channel number

       if (pstESInfo->esInfo.heAAcEsInfo.channelNum == 0x00)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MONO;
        }
        else if (pstESInfo->esInfo.heAAcEsInfo.channelNum == 0x01)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_DUAL_MONO;
			pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MONO;		//temp
        }
        else if (pstESInfo->esInfo.heAAcEsInfo.channelNum == 0x02)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_STEREO;
        }
        else if (pstESInfo->esInfo.heAAcEsInfo.channelNum == 0x03)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MULTI;
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_DUAL_MONO;	//temp
        }
        else
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_UNKNOWN;
        }
	}
	else if (eSrcType == LX_ADEC_SRC_TYPE_MPEG || eSrcType == LX_ADEC_SRC_TYPE_MP3)
	{
		pstESInfo->esInfo.mpegESInfo.bitRate 	= esInfo0/32; 	//MPEG Bitrate
		pstESInfo->esInfo.mpegESInfo.sampleRate = esInfo1; 		//MPEG Samplerate
		pstESInfo->esInfo.mpegESInfo.layer 		= esInfo2; 		//MPEG Layer
		pstESInfo->esInfo.mpegESInfo.channelNum = esInfo3; 		//MPEG Channel Number

        if (pstESInfo->esInfo.mpegESInfo.channelNum == 0x00)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_STEREO;	//mtk
        }
        else if (pstESInfo->esInfo.mpegESInfo.channelNum == 0x01)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_JOINT_STEREO;
			pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MONO;		//temp, mtk
        }
        else if (pstESInfo->esInfo.mpegESInfo.channelNum == 0x02)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_DUAL_MONO;
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_STEREO;	//temp, mtk
        }
        else if (pstESInfo->esInfo.mpegESInfo.channelNum == 0x03)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MONO;		//mtk
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_DUAL_MONO;//lg
        }
        else
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_UNKNOWN;
        }
	}
	else if (eSrcType == LX_ADEC_SRC_TYPE_AC3 || eSrcType == LX_ADEC_SRC_TYPE_EAC3)
	{
		pstESInfo->esInfo.ac3ESInfo.bitRate 	= esInfo0/32; 	//AC-3 Bitrate
		pstESInfo->esInfo.ac3ESInfo.sampleRate 	= esInfo1; 		//AC-3 Samplerate
		pstESInfo->esInfo.ac3ESInfo.channelNum 	= esInfo2; 		//AC-3 Channel Number
		pstESInfo->esInfo.ac3ESInfo.EAC3 		= esInfo3; 		//EAC3 Flag, 0:AC-3, 1:E-AC3(DD Plus)

        if (pstESInfo->esInfo.ac3ESInfo.channelNum == 0x00)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MONO;
        }
        else if (pstESInfo->esInfo.ac3ESInfo.channelNum == 0x01)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_DUAL_MONO;
			pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MONO;		//temp
        }
        else if (pstESInfo->esInfo.ac3ESInfo.channelNum == 0x02)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_STEREO;
        }
        else if (pstESInfo->esInfo.ac3ESInfo.channelNum == 0x03)
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_MULTI;
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_DUAL_MONO;	//temp
        }
        else
        {
            pstESInfo->esInfo.audioMode = LX_ADEC_AUDIO_MODE_UNKNOWN;
        }
	}
	else
	{
		ADEC_PRINT("ADEC_DRV_GetESInfo : Not any of MS10_DDT, MPEG or MS10_DDC(type = 0x%X)\n", eSrcType);
	}

	if((g_ui32AdecPrintCount % DEBUG_MSG_PRINT_TIME) == 0)	//DDI calls every 30 ms in DVB mode or 20 ms in ATSC mode
		ADEC_PRINT("ADEC_DRV_GetESInfo : %d ES Info set!\n", pstESInfo->esInfo.adecFormat);

	return RET_OK;
}


/**
 * Starts the audio description decoding.
 * Audio description sound should be mixed with the main audio sound.
 * @see ADEC_DRV_StopADDecoding
*/
int ADEC_L9_StartADDecoding ( LX_ADEC_SRC_TYPE_T eSrcType )
{
	int		eRetVal = RET_OK;

	KDRV_ADEC_DEC_PARAM_T		decParam;
	KDRV_ADEC_AC3_INFO_T 		ac3Info;
	KDRV_ADEC_AAC_INFO_T 		aacInfo;

	ADEC_CMD_SYNC_BASE_T 		syncBase;
	KDRV_ADEC_LIPSYNC_PARAM_T 	lipsyncParam;

	//Get input port and codec type for IPC command
	ADEC_IPC_GetConfig(KDRV_ADEC_DEC_1, &decParam);

	decParam.adecInputPort = KDRV_ADEC_INPUT_TP_1;
	decParam.adecCodecType = ADEC_ConvertCodecType(eSrcType);
	decParam.ui32Param[0]  = NUM_OF_PARAM_NONE_INFO;

	//Check a DRC, Downmix mode for AC-3(EAC-3) or AAC(HEAAC) decoding
	if(eSrcType == LX_ADEC_SRC_TYPE_AC3 || eSrcType == LX_ADEC_SRC_TYPE_EAC3)
	{
		ac3Info.numOfParam 	= NUM_OF_PARAM_AC3_INFO;
		ac3Info.drcMode 	= g_adec_kdrv.drcMode[LX_ADEC_DEC_0];
		ac3Info.downmixMode = g_adec_kdrv.downmixMode[LX_ADEC_DEC_0];
		memcpy(&(decParam.ui32Param[0]), &ac3Info, sizeof(KDRV_ADEC_AC3_INFO_T));
		ADEC_DEBUG("DDC : DRC = %d, Downmix = %d Selected.\n", ac3Info.drcMode, ac3Info.downmixMode);
	}
	else if(eSrcType == LX_ADEC_SRC_TYPE_AAC || eSrcType == LX_ADEC_SRC_TYPE_HEAAC)
	{
		aacInfo.numOfParam 	= NUM_OF_PARAM_AAC_INFO;
		aacInfo.drcMode 	= g_adec_kdrv.drcMode[LX_ADEC_DEC_0];
		aacInfo.downmixMode = g_adec_kdrv.downmixMode[LX_ADEC_DEC_0];
		memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));
		ADEC_DEBUG("DDT : DRC = %d, Downmix = %d Selected.\n", aacInfo.drcMode, aacInfo.downmixMode);
	}

	ADEC_IPC_SetConfig(KDRV_ADEC_DEC_1, &decParam);

	//Get a lip sync paramter by adec codec type
	ADEC_IPC_GetLipsync(KDRV_ADEC_DEC_1, &lipsyncParam);

	//Lip sync adjust : 2012.01.05, +100(vdec delay change, 2011.01.06)
	if( eSrcType == LX_ADEC_SRC_TYPE_MPEG )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 202 + VDEC_TP_LIPSYNC_OFFSET;		//0xF6 - 0x2C = 0xCA(202)
	}
	else if( eSrcType == LX_ADEC_SRC_TYPE_AC3 || eSrcType == LX_ADEC_SRC_TYPE_EAC3 )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 171 + VDEC_TP_LIPSYNC_OFFSET;		//0xCC - 0x21 = 0xAB(171)
	}
	else if( eSrcType == LX_ADEC_SRC_TYPE_AAC )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 120 + VDEC_TP_LIPSYNC_OFFSET;		//2002.02.06 by suji.baek
	}
	else if( eSrcType == LX_ADEC_SRC_TYPE_HEAAC )
	{
		lipsyncParam.ubound = 32;		//0x20(32)
		lipsyncParam.lbound = 32;		//0x20(32)
		lipsyncParam.offset = 167 + VDEC_TP_LIPSYNC_OFFSET;		//0xE7 - 0x40 = 0xA7(167)
	}
	else
	{
		ADEC_DEBUG("ADEC_L9_StartADDecoding : Error(type = %d)\n", eSrcType);
	}

	lipsyncParam.freerunubound = 5000;	// 5 sec, 0x1388(5000)
	lipsyncParam.freerunlbound = 5000;	// 5 sec, 0x1388(5000)

	ADEC_IPC_SetLipsync(KDRV_ADEC_DEC_1, &lipsyncParam);

	// for MHEG TP stream lipsync
	// 0x0XXXXXXX : Now set BaseTime, 0x1FFFFFFF : No PCR, 0x2FFFFFFF : PCR mode.
	syncBase.module     = KDRV_ADEC_DEC_1;
	syncBase.clockBase  = 0x2FFFFFFF;
	syncBase.streamBase = 0x2FFFFFFF;

	ADEC_IPC_SetBaseTime(syncBase);

	ADEC_IPC_Start(KDRV_ADEC_DEC_1);

	/* Update Internal Status Value */
	g_adec_kdrv.bDecADStarted = TRUE;

	ADEC_DEBUG("ADEC_DRV_StartADDecoding : Started...\n");
	return eRetVal;
}

/**
 * Stops the audio description decoding.
 * This function also mutes audio output.
 * @see ADEC_DRV_StartADDecoding
*/
int ADEC_L9_StopADDecoding ( void )
{
	int		eRetVal = RET_OK;

	ADEC_IPC_Stop(KDRV_ADEC_DEC_1);

	/* Update Internal Status Value */
	g_adec_kdrv.bDecADStarted = FALSE;

	ADEC_DEBUG("ADEC_DRV_StopADDecoding : Stopped...\n");
	return eRetVal;
}

/**
 * Sets a speaker output mode(L+R,L+L,R+R).
 * Controls the speaker output mode by DDI function.
 * @see ADEC_DRV_SetDACOutVolume
*/
int ADEC_L9_SetSPKOutMode ( LX_ADEC_OUTPUT_MODE_T eSPKOutMode )
{
	KDRV_ADEC_SPK_PARAM_T 		spkParam;
	KDRV_ADEC_SPDIF_PARAM_T 	spdifParam;
	KDRV_ADEC_SCART_PARAM_T 	scartParam;

	KDRV_ADEC_HP_PARAM_T 		hpParam;
	KDRV_ADEC_LINEOUT_PARAM_T 	lineoutParam;

	KDRV_ADEC_ENC_PARAM_T		encParam;

	//Check a adec mode
	if(eSPKOutMode.adecMode == LX_ADEC_DECODE_MAIN)
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_SPK,   &spkParam);
		ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
		ADEC_IPC_GetConfig(KDRV_ADEC_SCART, &scartParam);

		spkParam.outmode   = eSPKOutMode.outputMode;
		spdifParam.outmode = eSPKOutMode.outputMode;
		scartParam.outmode = eSPKOutMode.outputMode;
	}
	else
	{
		//Get a encoder parameters
		ADEC_IPC_GetConfig(KDRV_ADEC_ENC, &encParam);

		encParam.aencOutMode = eSPKOutMode.outputMode;

		//Set a encoder parameters
		ADEC_IPC_SetConfig(KDRV_ADEC_ENC, &encParam);

		ADEC_DEBUG("ADEC_DRV_SetSPKOutMode(AENC) %d : OK\n", eSPKOutMode.outputMode );
		return RET_OK;
	}

	ADEC_IPC_SetConfig(KDRV_ADEC_SPK,   &spkParam);
	ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);
	ADEC_IPC_SetConfig(KDRV_ADEC_SCART, &scartParam);

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_HP, &hpParam);

		//Check a adec mode
		if(eSPKOutMode.adecMode == LX_ADEC_DECODE_ASSO)
		{
			//temp
			hpParam.outmode = eSPKOutMode.outputMode;
		}
		else
		{
			hpParam.outmode = eSPKOutMode.outputMode;
		}

		ADEC_IPC_SetConfig(KDRV_ADEC_HP, &hpParam);
	}
	else
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);

		//Check a adec mode
		if(eSPKOutMode.adecMode == LX_ADEC_DECODE_ASSO)
		{
			//temp
			lineoutParam.outmode = eSPKOutMode.outputMode;
		}
		else
		{
			lineoutParam.outmode = eSPKOutMode.outputMode;
		}

		ADEC_IPC_SetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);
	}

	ADEC_PRINT("ADEC_DRV_SetSPKOutMode(MAIN) %d : OK\n", eSPKOutMode.outputMode );
	return RET_OK;
}

/**
 * Sets a decoder mode(DRC, Downmix).
 * Controls the decoder DRC and Downmix mode by DDI function.
 * @see ADEC_L9_StartDecoding
*/
int ADEC_L9_SetDecoderMode ( LX_ADEC_DECODER_MODE_T eDecoderMode )
{
	UINT8	decNum = 0;

	char *pdrcmodeStr[3] = {"LINE", "RF", "DRC OFF"};
	char *pdownmixStr[2] = {"LoRo", "LtRt"};

	//Check a adec mode
	if(eDecoderMode.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum = LX_ADEC_DEC_0;
	}
	else if(eDecoderMode.adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum = LX_ADEC_DEC_1;
	}
	else
	{
		decNum = LX_ADEC_DEC_0;

		ADEC_DEBUG("ADEC_L9_SetDecoderMode : adecMode = 0x%X!!!\n", eDecoderMode.adecMode);
	}

	//Check a decoder set mode
	if(eDecoderMode.setMode & LX_ADEC_DOLBY_DRC_MODE)
	{
		// Save a DRC mode
		g_adec_kdrv.drcMode[decNum] = eDecoderMode.drcMode;

		if(eDecoderMode.drcMode == LX_ADEC_DOLBY_DRC_OFF)
		{
			ADEC_ERROR("ADEC_L9_SetDecoderMode : LX_ADEC_DOLBY_DRC_OFF!!!\n");
		}

		if(eDecoderMode.drcMode <= LX_ADEC_DOLBY_DRC_OFF)
		{
			ADEC_DEBUG("ADEC_L9_SetDecoderMode : dec = %d, drc = %s\n",	\
						decNum, pdrcmodeStr[eDecoderMode.drcMode]);
		}
	}

	if(eDecoderMode.setMode & LX_ADEC_DOWNMIX_MODE)
	{
		// Save a downmix mode
		g_adec_kdrv.downmixMode[decNum] = eDecoderMode.downmixMode;

		if(eDecoderMode.downmixMode <= LX_ADEC_LTRT_MODE)
		{
			ADEC_DEBUG("ADEC_L9_SetDecoderMode : dec = %d, downmix = %s\n",	\
						decNum, pdownmixStr[eDecoderMode.downmixMode]);
		}
	}

	ADEC_DEBUG_TMP("ADEC_L9_SetDecoderMode : Started(dec = %d, drc = %d, downmix = %d)\n",	\
					decNum, eDecoderMode.drcMode, eDecoderMode.downmixMode);
	return RET_OK;
}

/**
 * Sets an audio volume.
 * Range of input parameter value is 0 to 0xFFF FFFF.
 * @see ADEC_DRV_SetADVolume
*/
int ADEC_L9_SetVolume ( UINT32 ui32Volume )
{
	KDRV_ADEC_SPK_PARAM_T spkParam;

	ADEC_IPC_GetConfig(KDRV_ADEC_SPK, &spkParam);

	spkParam.volctrl = 1;
	spkParam.volume  = ui32Volume;

	ADEC_IPC_SetConfig(KDRV_ADEC_SPK, &spkParam);

	ADEC_PRINT("ADEC_DRV_SetVolume 0x%X : OK\n", ui32Volume );
	return RET_OK;
}

/**
 * Sets an audio description volume before mixing main audio.
 * This function is used when only ADEC_IOW_START_AD_DECODING is called.
 * @see ADEC_DRV_SetVolume
*/
int ADEC_L9_SetADVolume ( UINT32 ui32Volume )
{
	KDRV_ADEC_MIX_START_PARAM_T mixStartParam;

	//Get a sampling frequency from SRC1 register.
	AUD_RdFL(aud_src1_sf);

	ADEC_IPC_GetConfig(KDRV_ADEC_MP_01, &mixStartParam);

	mixStartParam.ui32GainL 	 = ui32Volume;
	mixStartParam.ui32GainR 	 = ui32Volume;
	mixStartParam.ui32SampleRate = AUD_Rd(aud_src1_sf);

	ADEC_IPC_SetConfig(KDRV_ADEC_MP_01, &mixStartParam);

	ADEC_DEBUG("ADEC_DRV_SetADVolume %d(freq = %d) : OK\n", ui32Volume, AUD_Rd(aud_src1_sf));
	return RET_OK;
}

/**
 * Sets the volume of DAC output.
 * Range of input parameter value is 0 to 0xFFFF.
 * @see ADEC_DRV_SetVolume
*/
int ADEC_L9_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol )
{
	KDRV_ADEC_SCART_PARAM_T 	scartParam;
#if 0
	KDRV_ADEC_HP_PARAM_T 		hpParam;
#endif
	KDRV_ADEC_LINEOUT_PARAM_T 	lineoutParam;
	ADEC_SET_VOLUME_T			setVolume;

	if(stDACOutVol.mode & LX_ADEC_DAC_DSP)
	{
		//Set a SCART DSP volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			ADEC_IPC_GetConfig(KDRV_ADEC_SCART, &scartParam);

			scartParam.volctrl = 1;
			scartParam.volume  = stDACOutVol.swVolume;

			ADEC_IPC_SetConfig(KDRV_ADEC_SCART, &scartParam);
		}

		//Set a HeadPhone DSP volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			//Check a L9 chip revision
			if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
			{
				//Change a HP input gain setting from DSP to MixedIP to protect saturation(2013.01.25).
				#if	1
				//Set a DAC volume for Mixed IP Module
				if ( RET_OK != MIXED_IP_ADEC_L9_SetDACOutVolume( stDACOutVol ) )
				{
					ADEC_ERROR("MIXED_IP_ADEC_L9_SetDACOutVolume : Error!\n");
					return RET_ERROR;
				}
				#else
				ADEC_IPC_GetConfig(KDRV_ADEC_HP, &hpParam);

				hpParam.volctrl = 1;
				hpParam.volume  = stDACOutVol.swVolume;

				ADEC_IPC_SetConfig(KDRV_ADEC_HP, &hpParam);
				#endif
			}
			else
			{
				ADEC_IPC_GetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);

				lineoutParam.volctrl = 1;
				lineoutParam.volume  = stDACOutVol.swVolume;

				ADEC_IPC_SetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);
			}
		}
	}

	//MixedIP volume control
	if( (stDACOutVol.mode & LX_ADEC_DAC_MIXEDIP) ||(stDACOutVol.mode & LX_ADEC_DAC_SCART) )
	{
		if(stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC2)
		{
			setVolume.module = KDRV_ADEC_HP;
			setVolume.ui32Volume = stDACOutVol.swVolume;
			ADEC_IPC_SetVolume(setVolume);

			//Save a HP volume
			g_adec_kdrv.hpVolume = stDACOutVol.swVolume;
		}
		else
		{
			//Set a DAC volume for Mixed IP Module
			if ( RET_OK != MIXED_IP_ADEC_L9_SetDACOutVolume( stDACOutVol ) )
			{
				ADEC_ERROR("MIXED_IP_ADEC_L9_SetDACOutVolume : Error!\n");
				return RET_ERROR;
			}
		}
	}
	return RET_OK;
}

/**
 * Sets the mute of each DAC and I2S output port.
 * The SPDIF output is implemented by another IOCTL.(ADEC_IOW_SET_SPDIF_MUTE)
 * @see ADEC_DRV_SetSPDIFOutputType
*/
int ADEC_L9_SetOutputPortMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute )
{
	KDRV_ADEC_SCART_PARAM_T 	scartParam;
	KDRV_ADEC_HP_PARAM_T 		hpParam;
	KDRV_ADEC_LINEOUT_PARAM_T 	lineoutParam;

	//DSP mute control
	if(stOutPortMute.mode & LX_ADEC_DAC_DSP)
	{
		//Set a SCART DSP volume
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			ADEC_IPC_GetConfig(KDRV_ADEC_SCART, &scartParam);

			scartParam.mute = stOutPortMute.bSwOnOff;

			ADEC_IPC_SetConfig(KDRV_ADEC_SCART, &scartParam);
		}

		//Set a HeadPhone DSP volume
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			//Check a L9 chip revision
			if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
			{
				ADEC_IPC_GetConfig(KDRV_ADEC_HP, &hpParam);

				hpParam.mute = stOutPortMute.bSwOnOff;

				ADEC_IPC_SetConfig(KDRV_ADEC_HP, &hpParam);
			}
			else
			{
				ADEC_IPC_GetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);

				lineoutParam.mute = stOutPortMute.bSwOnOff;

				ADEC_IPC_SetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);
			}
		}
	}

	//MixedIP mute control
	if( (stOutPortMute.mode & LX_ADEC_DAC_MIXEDIP)		\
	  ||(stOutPortMute.mode & LX_ADEC_DAC_SCART_RESET) )
	{
		//Set a DAC mute for Mixed IP Module
		if ( RET_OK != MIXED_IP_ADEC_L9_SetDACOutMute( stOutPortMute ) )
		{
			ADEC_ERROR("MIXED_IP_ADEC_L9_SetDACOutMute : Error!\n");
			return RET_ERROR;
		}
	}

	ADEC_PRINT("ADEC_L9_SetOutputPortMute 0x%X : OK\n", stOutPortMute.portNum );
	return RET_OK;
}

/**
* Sets an audio mix volume before mixing main audio.
* Range of input volume parameter value is 0 to 4096.
 * @see ADEC_DRV_SetVolume
*/
int ADEC_L9_SetMixVolume ( LX_ADEC_MIX_VOL_T stMixVolume )
{
	int			ret = RET_OK;

	UINT8		decNum = LX_ADEC_DEC_0;

	UINT32		ui32Volume = 1024;

	KDRV_ADEC_MOD_T 	module;

	ADEC_SET_SPDIF_IEC_T			spdifIec;
	KDRV_ADEC_SPDIF_PARAM_T 		spdifParam;

	KDRV_ADEC_MIX_START_PARAM_T 	mixStartParam;

	UINT32		samplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;

	LX_ADEC_SPDIF_DTO_RATE_T	spdifDtoRate = LX_ADEC_SPDIF_DTO_NORMAL;

	//Check a adec mode
	if(stMixVolume.adecMode == LX_ADEC_DECODE_MAIN)
	{
		module = KDRV_ADEC_MP_00;
	}
	else if(stMixVolume.adecMode == LX_ADEC_DECODE_ASSO)
	{
		module = KDRV_ADEC_MP_01;
	}
	else if(stMixVolume.adecMode == LX_ADEC_MIX_BUF0)
	{
		module = KDRV_ADEC_MP_02;
	}
	else if(stMixVolume.adecMode == LX_ADEC_MIX_BUF1)
	{
		module = KDRV_ADEC_MP_03;
	}
	else if(stMixVolume.adecMode == LX_ADEC_MIX_BUF2)
	{
		module = KDRV_ADEC_MP_04;
	}
	else if(stMixVolume.adecMode == LX_ADEC_MIX_BUF3)
	{
		module = KDRV_ADEC_MP_05;
	}
	else if(stMixVolume.adecMode == LX_ADEC_LLB_BUF)
	{
		module = KDRV_ADEC_MP_06;
	}
	else
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mode is not defined(adecMode = 0x%X)!!!\n", stMixVolume.adecMode);
		return RET_ERROR;
	}

#ifdef ENABLE_DIRECT_MIX_VOLUME
	/* Check a max volume gain */
	if(stMixVolume.mixVolume > ADEC_MAX_MIX_VOLUME_GAIN)
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mixVolume is not large(mixVolume = %d)!!!\n", stMixVolume.mixVolume);
		return RET_ERROR;
	}

	/* Copy a mix volume */
	ui32Volume = stMixVolume.mixVolume;

	ADEC_IPC_GetConfig(module, &mixStartParam);

	mixStartParam.ui32GainL = ui32Volume;
	mixStartParam.ui32GainR = ui32Volume;
#else
	/* Check a max volume table size */
	if(stMixVolume.mixVolume > ADEC_MIX_VOLUME_TABLE_SIZE)
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mixVolume is not large(mixVolume = %d)!!!\n", stMixVolume.mixVolume);
		return RET_ERROR;
	}

	/* Convert a mix volume from table */
	ui32Volume = g_ADEC_MixVolTable[stMixVolume.mixVolume];

	ADEC_IPC_GetConfig(module, &mixStartParam);

	mixStartParam.ui32GainL = ui32Volume;
	mixStartParam.ui32GainR = ui32Volume;
#endif

	//Get a sampling frequency.
	if(stMixVolume.adecMode == LX_ADEC_DECODE_MAIN)
	{
		//Get a sampling frequency from SRC0register.
		AUD_RdFL(aud_src0_sf);
 		mixStartParam.ui32SampleRate = AUD_Rd(aud_src0_sf);
	}
	else if(stMixVolume.adecMode == LX_ADEC_DECODE_ASSO)
	{
		//Get a sampling frequency from SRC1 register.
		AUD_RdFL(aud_src1_sf);
 		mixStartParam.ui32SampleRate = AUD_Rd(aud_src1_sf);
	}

	ADEC_IPC_SetConfig(module, &mixStartParam);

	//Set a SPDIF Auto output type for GCD model when LG home dashboard is enabled.
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Check a dual decoding
		if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)	\
		  &&(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE) )
		{
			//Set a SPDIF decoder output if mix volume is 0.(opposite case)
			if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)	\
			  &&(ui32Volume == 0) )
			{
				if(stMixVolume.adecMode == LX_ADEC_DECODE_MAIN)
				{
					decNum = LX_ADEC_DEC_1;

					//Set a SPDIF Output type by adec source and codec type
					ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_1], g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
					if (ret != RET_OK)
					{
						ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
					}

					//Get a sampling frequency from SRC1 register.
					AUD_RdFL(aud_src1_sf);
					samplingFreq = AUD_Rd(aud_src1_sf);

					//Set a SPDIF IEC Source for IPC
					spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_1;

					//Set a SPDIF IEC lip sync type for IPC
					if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)	\
					  &&(g_adec_kdrv.bSpdifPcm == FALSE) )
					{
						spdifIec.ui32LipsyncOnOff = TRUE;
					}
					else
					{
						spdifIec.ui32LipsyncOnOff = FALSE;
					}

					ADEC_IPC_SetSPDIFIEC(spdifIec);
				}
				else if(stMixVolume.adecMode == LX_ADEC_DECODE_ASSO)
				{
					decNum = LX_ADEC_DEC_0;

					//Set a SPDIF Output type by adec source and codec type
					ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
					if (ret != RET_OK)
					{
						ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
					}

					//Get a sampling frequency from SRC0 register.
					AUD_RdFL(aud_src0_sf);
					samplingFreq = AUD_Rd(aud_src0_sf);

					//Set a SPDIF IEC Source for IPC
					spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;

					//Set a SPDIF IEC lip sync type for IPC
					if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)	\
					  &&(g_adec_kdrv.bSpdifPcm == FALSE) )
					{
						spdifIec.ui32LipsyncOnOff = TRUE;
					}
					else
					{
						spdifIec.ui32LipsyncOnOff = FALSE;
					}

					ADEC_IPC_SetSPDIFIEC(spdifIec);
				}

				//Set a SPDIF clock frequency for ES output and PCM output
				if(g_adec_kdrv.bSpdifPcm == FALSE)
				{
					if( (g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_AC3)
					  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_EAC3)
					  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_DTS) )
					{
						spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, samplingFreq);

						(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(samplingFreq, spdifDtoRate);
					}
					else	//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec.
					{
						//Set a SPDIF clock for 48Khz
						spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, LX_ADEC_SAMPLING_FREQ_48_KHZ);

						(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, spdifDtoRate);

						if(g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_TP)
						{
							//In boot time, SRC0 is not set before decoding started from fw if this function is called.
							if( (samplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ)
							  ||(samplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ) )
							{
								ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", AUD_Rd(aud_src0_sf));

								ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
								spdifParam.type = 0; //PCM output
								ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

								//update a global variable.
								g_adec_kdrv.bSpdifPcm = TRUE;
							}
						}
						else	//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
						{
							if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
							{
								ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", AUD_Rd(aud_src0_sf));

								ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
								spdifParam.type = 0; //PCM output
								ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

								//update a global variable.
								g_adec_kdrv.bSpdifPcm = TRUE;
							}
						}
					}
				}
				else
				{
					ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
					spdifParam.type = 0; //PCM output
					ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

					//update a global variable.
					g_adec_kdrv.bSpdifPcm = TRUE;
				}
			}
		}

		ADEC_DEBUG("ADEC_L9_SetMixVolume(initType = 0x%X) : SPDIF mode = %d, pcm = %d(bEncStarted = %d)\n",	\
					g_eAdecInitType, g_adec_kdrv.spdifMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bEncStarted);
	}

	ADEC_DEBUG("ADEC_L9_SetMixVolume : adecMode = 0x%X, mix = %d, gain = %d)\n",	\
									   stMixVolume.adecMode, stMixVolume.mixVolume, ui32Volume);
	return RET_OK;
}

/**
* Sets an audio mix channel volume before mixing main audio.
* Range of input volume parameter value is 0 to 4096.
 * @see ADEC_DRV_SetVolume
*/
int ADEC_L9_SetMixChannelVolume ( LX_ADEC_MIX_CH_VOL_T stMixChVolume )
{
	int			ret = RET_OK;

	UINT8		decNum 		 = LX_ADEC_DEC_0;
	UINT32		ui32VolumeLeft = 1024, ui32VolumeRight = 1024;
	UINT32		samplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;

	KDRV_ADEC_MOD_T 	module;

	ADEC_SET_SPDIF_IEC_T			spdifIec;
	KDRV_ADEC_SPDIF_PARAM_T 		spdifParam;
	KDRV_ADEC_MIX_START_PARAM_T 	mixStartParam;

	LX_ADEC_SPDIF_DTO_RATE_T	spdifDtoRate = LX_ADEC_SPDIF_DTO_NORMAL;

	//Check a adec mode
	if(stMixChVolume.adecMode == LX_ADEC_DECODE_MAIN)
	{
		module = KDRV_ADEC_MP_00;
	}
	else if(stMixChVolume.adecMode == LX_ADEC_DECODE_ASSO)
	{
		module = KDRV_ADEC_MP_01;
	}
	else if(stMixChVolume.adecMode == LX_ADEC_MIX_BUF0)
	{
		module = KDRV_ADEC_MP_02;
	}
	else if(stMixChVolume.adecMode == LX_ADEC_MIX_BUF1)
	{
		module = KDRV_ADEC_MP_03;
	}
	else if(stMixChVolume.adecMode == LX_ADEC_MIX_BUF2)
	{
		module = KDRV_ADEC_MP_04;
	}
	else if(stMixChVolume.adecMode == LX_ADEC_MIX_BUF3)
	{
		module = KDRV_ADEC_MP_05;
	}
	else if(stMixChVolume.adecMode == LX_ADEC_LLB_BUF)
	{
		module = KDRV_ADEC_MP_06;
	}
	else
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mode is not defined(adecMode = 0x%X)!!!\n", stMixChVolume.adecMode);
		return RET_ERROR;
	}

#ifdef ENABLE_DIRECT_MIX_VOLUME
	/* Check a max volume left gain */
	if(stMixChVolume.mixLeftVolume > ADEC_MAX_MIX_VOLUME_GAIN)
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mixVolume is not large(mixLeftVolume = %d)!!!\n", stMixChVolume.mixLeftVolume);
		return RET_ERROR;
	}

	/* Check a max volume right gain */
	if(stMixChVolume.mixRightVolume > ADEC_MAX_MIX_VOLUME_GAIN)
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mixVolume is not large(mixRightVolume = %d)!!!\n", stMixChVolume.mixRightVolume);
		return RET_ERROR;
	}

	/* Copy a mix volume */
	ui32VolumeLeft  = stMixChVolume.mixLeftVolume;
	ui32VolumeRight = stMixChVolume.mixRightVolume;

	ADEC_IPC_GetConfig(module, &mixStartParam);

	mixStartParam.ui32GainL = ui32VolumeLeft;
	mixStartParam.ui32GainR = ui32VolumeRight;
#else
	/* Check a max volume table size */
	if(stMixChVolume.mixLeftVolume > ADEC_MIX_VOLUME_TABLE_SIZE)
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mixVolume is not large(mixLeftVolume = %d)!!!\n", stMixChVolume.mixLeftVolume);
		return RET_ERROR;
	}

	/* Check a max volume table size */
	if(stMixChVolume.mixRightVolume > ADEC_MIX_VOLUME_TABLE_SIZE)
	{
		ADEC_ERROR("ADEC_L9_SetMixVolume : mixVolume is not large(mixRightVolume = %d)!!!\n", stMixChVolume.mixRightVolume);
		return RET_ERROR;
	}

	/* Convert a mix volume from table */
	ui32VolumeLeft  = g_ADEC_MixVolTable[stMixChVolume.mixLeftVolume];
	ui32VolumeRight = g_ADEC_MixVolTable[stMixChVolume.mixRightVolume];

	ADEC_IPC_GetConfig(module, &mixStartParam);

	mixStartParam.ui32GainL = ui32VolumeLeft;
	mixStartParam.ui32GainR = ui32VolumeRight;
#endif

	//Get a sampling frequency.
	if(stMixChVolume.adecMode == LX_ADEC_DECODE_MAIN)
	{
		//Get a sampling frequency from SRC0register.
		AUD_RdFL(aud_src0_sf);
 		mixStartParam.ui32SampleRate = AUD_Rd(aud_src0_sf);
	}
	else if(stMixChVolume.adecMode == LX_ADEC_DECODE_ASSO)
	{
		//Get a sampling frequency from SRC1 register.
		AUD_RdFL(aud_src1_sf);
 		mixStartParam.ui32SampleRate = AUD_Rd(aud_src1_sf);
	}

	ADEC_IPC_SetConfig(module, &mixStartParam);

	//Set a SPDIF Auto output type for GCD model when LG home dashboard is enabled.
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Check a dual decoding
		if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)	\
		  &&(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE) )
		{
			//Set a SPDIF decoder output if mix volume is 0.(opposite case)
			if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)	\
			  &&(ui32VolumeLeft == 0 && ui32VolumeRight == 0) )
			{
				if(stMixChVolume.adecMode == LX_ADEC_DECODE_MAIN)
				{
					decNum = LX_ADEC_DEC_1;

					//Set a SPDIF Output type by adec source and codec type
					ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_1], g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
					if (ret != RET_OK)
					{
						ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
					}

					//Get a sampling frequency from SRC1 register.
					AUD_RdFL(aud_src1_sf);
					samplingFreq = AUD_Rd(aud_src1_sf);

					//Set a SPDIF IEC Source for IPC
					spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_1;

					//Set a SPDIF IEC lip sync type for IPC
					if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)	\
					  &&(g_adec_kdrv.bSpdifPcm == FALSE) )
					{
						spdifIec.ui32LipsyncOnOff = TRUE;
					}
					else
					{
						spdifIec.ui32LipsyncOnOff = FALSE;
					}

					ADEC_IPC_SetSPDIFIEC(spdifIec);
				}
				else if(stMixChVolume.adecMode == LX_ADEC_DECODE_ASSO)
				{
					decNum = LX_ADEC_DEC_0;

					//Set a SPDIF Output type by adec source and codec type
					ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
					if (ret != RET_OK)
					{
						ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
					}

					//Get a sampling frequency from SRC0 register.
					AUD_RdFL(aud_src0_sf);
					samplingFreq = AUD_Rd(aud_src0_sf);

					//Set a SPDIF IEC Source for IPC
					spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;

					//Set a SPDIF IEC lip sync type for IPC
					if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)	\
					  &&(g_adec_kdrv.bSpdifPcm == FALSE) )
					{
						spdifIec.ui32LipsyncOnOff = TRUE;
					}
					else
					{
						spdifIec.ui32LipsyncOnOff = FALSE;
					}

					ADEC_IPC_SetSPDIFIEC(spdifIec);
				}

				//Set a SPDIF clock frequency for ES output and PCM output
				if(g_adec_kdrv.bSpdifPcm == FALSE)
				{
					if( (g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_AC3)
					  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_EAC3)
					  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_DTS) )
					{
						spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, samplingFreq);

						(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(samplingFreq, spdifDtoRate);
					}
					else	//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec.
					{
						//Set a SPDIF clock for 48Khz
						spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, LX_ADEC_SAMPLING_FREQ_48_KHZ);

						(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, spdifDtoRate);

						if(g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_TP)
						{
							//In boot time, SRC0 is not set before decoding started from fw if this function is called.
							if( (samplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ)
							  ||(samplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ) )
							{
								ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", AUD_Rd(aud_src0_sf));

								ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
								spdifParam.type = 0; //PCM output
								ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

								//update a global variable.
								g_adec_kdrv.bSpdifPcm = TRUE;
							}
						}
						else	//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
						{
							if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
							{
								ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", AUD_Rd(aud_src0_sf));

								ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
								spdifParam.type = 0; //PCM output
								ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

								//update a global variable.
								g_adec_kdrv.bSpdifPcm = TRUE;
							}
						}
					}
				}
				else
				{
					ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
					spdifParam.type = 0; //PCM output
					ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

					//update a global variable.
					g_adec_kdrv.bSpdifPcm = TRUE;
				}
			}
		}

		ADEC_DEBUG("ADEC_L9_SetMixVolume(initType = 0x%X) : SPDIF mode = %d, pcm = %d(bEncStarted = %d)\n",	\
					g_eAdecInitType, g_adec_kdrv.spdifMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bEncStarted);
	}

	ADEC_DEBUG("ADEC_L9_SetMixVolume : adecMode = 0x%X, mix left = %d, mix right = %d)\n",	\
									   stMixChVolume.adecMode, stMixChVolume.mixLeftVolume, stMixChVolume.mixRightVolume);
	return RET_OK;
}


/**
* Sets an audio D-AMP main volume after mixing main/sub audio.
* Range of input parameter value is 0 to 100.
 * @see ADEC_DRV_SetVolume
*/
int ADEC_L9_SetDAMPVolume ( UINT8 ui8Volume )
{
#ifdef ENABLE_DAMP_VOLUME_CONTROL
	//Set a D-AMP volume for I2C NTP7400 Device
	if(RET_OK != ADEC_NTP7400_SetVolume(ui8Volume))
	{
		ADEC_ERROR("ADEC_NTP7400_SetVolume for D-AMP : Error!\n");
		return RET_ERROR;
	}
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL

	ADEC_PRINT("ADEC_L9_SetDAMPVolume 0x%X : OK\n", ui8Volume );
	return RET_OK;
}

/**
 * Sets a SPDIF mute on or off.
 * Stops the audio digital data output.
 * @see ADEC_DRV_SetSPDIFVolume
*/
int ADEC_L9_SetDAMPMute ( BOOLEAN bMuteFlag )
{
	//D-AMP Mute Control
#ifdef ENABLE_DAMP_VOLUME_CONTROL
	//Set a D-AMP mute for I2C NTP5400 Device
	if ( RET_OK != ADEC_NTP7400_SetMute(bMuteFlag) )
	{
		ADEC_ERROR("ADEC_NTP7400_SetMute : Error!\n");
		return RET_ERROR;
	}
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL

	ADEC_PRINT("ADEC_DRV_SetSPDIFMute : %d\n", bMuteFlag );
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
int ADEC_L9_SetSPDIFOutputType ( LX_ADEC_SPDIF_MODE_T eSPDIFMode )
{
	int ret = RET_OK;

	UINT8		decNum = LX_ADEC_DEC_0;

	UINT32		samplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;

	BOOLEAN		bDec0SpdifPcm = FALSE;

	KDRV_ADEC_ENC_PARAM_T 		encParam;
	KDRV_ADEC_SPDIF_PARAM_T 	spdifParam;
	ADEC_SET_SPDIF_IEC_T		spdifIec;

	LX_ADEC_SPDIF_DTO_RATE_T	spdifDtoRate		= LX_ADEC_SPDIF_DTO_NORMAL;

#ifdef ENABLE_DDCO_TEST
	if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO)
	  &&(eSPDIFMode == LX_ADEC_SPDIF_AC3) )
	{
		eSPDIFMode = LX_ADEC_SPDIF_DDCO;
	}

	eSPDIFMode = LX_ADEC_SPDIF_DDCO;	//temp for DDCO test.
#endif

	//Set a SPDIF mode from app.
	g_adec_kdrv.spdifMode = eSPDIFMode;

	//Check a ADEC Started Status.
	if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == FALSE)
	  &&(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == FALSE) )
	{
		ADEC_DEBUG("ADEC_DRV_SetSPDIFOutputType : LX_ADEC_DEC_%d_%d is not running!!!\n", LX_ADEC_DEC_0, LX_ADEC_DEC_1);
		return RET_OK;
	}
	
	//Set a SPDIF Output type by adec aource and codec type
	ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
	if (ret != RET_OK)
	{
		ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
	}

	//Get a sampling frequency from SRC0 register.
	AUD_RdFL(aud_src0_sf);
	samplingFreq = AUD_Rd(aud_src0_sf);

	//Set a SPDIF output type for GCD platform
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Set a SPDIF clock for 48Khz
		spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, LX_ADEC_SAMPLING_FREQ_48_KHZ);

		(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, spdifDtoRate);

		//Save a DEC0 spdif mode
		bDec0SpdifPcm = g_adec_kdrv.bSpdifPcm;

		//Set a SPDIF IEC Source for IPC
		spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;

		ADEC_DEBUG("ADEC_L9_SetSPDIFOutputType : SPDIF mode = %d, pcm = %d(dec0 = %d, dec1 = %d)\n",	\
					eSPDIFMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0], g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1]);

		//Check a DEC1 start
		if(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE)
		{
			//Set a SPDIF Output type by adec aource and codec type
			ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_1], g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
			if (ret != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
			}

			ADEC_DEBUG("ADEC_L9_SetSPDIFOutputType 0 : SPDIF mode = %d, pcm = %d(dec0 = %d, dec1 = %d)\n",	\
						eSPDIFMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0], g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1]);

			//Set a DEC1 for SPDIF ES output if DEC1 has ES output and DEC0 is not running.
			if(g_adec_kdrv.bSpdifPcm == FALSE && g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == FALSE)
			{
				//Set a SPDIF Source
				decNum = LX_ADEC_DEC_1;

				//Get a sampling frequency from SRC1 register.
				AUD_RdFL(aud_src1_sf);
				samplingFreq = AUD_Rd(aud_src1_sf);

				//Set a SPDIF IEC Source for IPC
				spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_1;
			}
			//Set a DEC0 for SPDIF ES output if DEC0 is running.
			else if(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
			{
				//Restore a DEC0 spdif mode
				g_adec_kdrv.bSpdifPcm = bDec0SpdifPcm;
			}
		}

		//Start a DDCO encoder for GCD platform
		if( (eSPDIFMode == LX_ADEC_SPDIF_AC3 || eSPDIFMode == LX_ADEC_SPDIF_DTS)	\
		  &&(g_adec_kdrv.bSpdifPcm == FALSE) )
		{
			//Start a SPDIF PCM output
			if(g_adec_kdrv.bEncStarted == TRUE)
			{
				ADEC_IPC_Stop(KDRV_ADEC_ENC);

				/* Update Internal Status Value */
				g_adec_kdrv.bEncStarted = FALSE;
			}

			//Set a SPDIF IEC lip sync for IPC
			spdifIec.ui32LipsyncOnOff = TRUE;

			ADEC_IPC_SetSPDIFIEC(spdifIec);

			ADEC_DEBUG("ADEC_L9_SetSPDIFOutputType 1 : SPDIF mode = %d, pcm = %d(bEncStarted = %d)\n",	\
						eSPDIFMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bEncStarted);
		}
		//Start a DDCO encoder for GCD platform
		else if(eSPDIFMode == LX_ADEC_SPDIF_DDCO)
		{
			//Start a SPDIF DDCO output
			if(g_adec_kdrv.bEncStarted == FALSE)
			{
				//Get a encoder parameters
				ADEC_IPC_GetConfig(KDRV_ADEC_ENC, &encParam);

				encParam.adecInputMode 	   = KDRV_ADEC_INPUT_MIX_OUT;
				encParam.aencCodecType 	   = KDRV_ADEC_CODEC_DDCO;
				encParam.channelNumber	   = 2;
				encParam.bitPerSample	   = 16;
				encParam.samplingFrequency = 480;
				encParam.bitRate 		   = 224000;
			//	encParam.bitRate = stEncParam.duration;

				//Set a encoder parameters
				ADEC_IPC_SetConfig(KDRV_ADEC_ENC, &encParam);

				ADEC_IPC_Start(KDRV_ADEC_ENC);

				//Set a SPDIF IEC Source for IPC
				spdifIec.ui32SetIecSrc = KDRV_ADEC_ENC;

				//Set a SPDIF IEC lip sync for IPC
				spdifIec.ui32LipsyncOnOff = FALSE;

				ADEC_IPC_SetSPDIFIEC(spdifIec);

				/* Update Internal Status Value */
				g_adec_kdrv.bEncStarted = TRUE;

				ADEC_DEBUG("ADEC_L9_SetSPDIFOutputType 2 : SPDIF mode = %d, pcm = %d(bEncStarted = %d)\n",	\
							eSPDIFMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bEncStarted);

				return RET_OK;
			}
		}
		else
		{
			//Start a SPDIF PCM output
			if(g_adec_kdrv.bEncStarted == TRUE)
			{
				ADEC_IPC_Stop(KDRV_ADEC_ENC);

				/* Update Internal Status Value */
				g_adec_kdrv.bEncStarted = FALSE;

				//Set a SPDIF IEC Source for IPC
				spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;

				//Set a SPDIF IEC lip sync for IPC
				spdifIec.ui32LipsyncOnOff = TRUE;

				ADEC_IPC_SetSPDIFIEC(spdifIec);
			}

			ADEC_DEBUG("ADEC_L9_SetSPDIFOutputType 3 : SPDIF mode = %d, pcm = %d(bEncStarted = %d)\n",	\
						eSPDIFMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bEncStarted);

			return RET_OK;
		}
	}

	//Lock a ADEC SPDIF Semaphore
	OS_LockMutex(&g_ADEC_Sema_Spdif);

	//Set a SPDIF clock frequency for ES output
	if( (g_adec_kdrv.bDecStarted[decNum] == TRUE) && (g_adec_kdrv.bSpdifPcm == FALSE) )
	{
		if( (g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_AC3)
		  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_EAC3)
		  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_DTS) )
		{
			spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, samplingFreq);

			(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(samplingFreq, spdifDtoRate);
		}
		else	//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec.
		{
			//Set a SPDIF clock for 48Khz
			spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, LX_ADEC_SAMPLING_FREQ_48_KHZ);

			(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, spdifDtoRate);

			if(g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_TP)
			{
				//In boot time, SRC0 is not set before decoding started from fw if this function is called.
				if( (samplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ)
				  ||(samplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ) )
				{
					ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", AUD_Rd(aud_src0_sf));

					ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
					spdifParam.type = 0; //PCM output
					ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

					//update a global variable.
					g_adec_kdrv.bSpdifPcm = TRUE;
				}
			}
			else	//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
			{
				if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
				{
					ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", AUD_Rd(aud_src0_sf));

					ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
					spdifParam.type = 0; //PCM output
					ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

					//update a global variable.
					g_adec_kdrv.bSpdifPcm = TRUE;
				}
			}
		}
	}
	//Set a SPDIF clock frequency for PCM output
	else if(g_adec_kdrv.bDecStarted[decNum] == TRUE)
	{
		spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, LX_ADEC_SAMPLING_FREQ_48_KHZ);

		(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, spdifDtoRate);
	}
	else
	{
		ADEC_DEBUG("ADEC_DRV_SetSPDIFOutputType : LX_ADEC_DEC_%d is not running!!!\n", decNum);
	}

	//Unlock a ADEC SPDIF Semaphore
	OS_UnlockMutex(&g_ADEC_Sema_Spdif);

	ADEC_DEBUG("ADEC_L9_SetSPDIFOutputType(initType = 0x%X) : SPDIF mode = %d, pcm = %d(bEncStarted = %d)\n",	\
				g_eAdecInitType, eSPDIFMode, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bEncStarted);
	ADEC_DEBUG("decNum = %d, samplingFreq = %d\n", decNum, samplingFreq);
	return RET_OK;
}

/**
  * Sets a S/PDIF SCMS(Serial Copy Management System) control type.
  * Refer a LX_ADEC_SPDIF_SCMS_T structure.
 * @see ADEC_DRV_SetSPDIFVolume
*/
int ADEC_L9_SetSPDIFScmsCtrl ( LX_ADEC_SPDIF_SCMS_T eSPDIFScmsCtrl )
{
	ADEC_SET_SPDIF_SCMS_T	spdifSCMS;

	spdifSCMS.ui32Scms = (UINT32)eSPDIFScmsCtrl;

	ADEC_IPC_SetSPDIFSCMS(spdifSCMS);

	ADEC_PRINT("ADEC_L9_SetSPDIFScmsCtrl scms = %d\n", eSPDIFScmsCtrl );
	return RET_OK;
}


/**
 * Sets a SPDIF output volume.
 * Range of input parameter value is 0x00 to 0xFFF FFFF.
 * @see ADEC_DRV_SetSPDIFMute
*/
int ADEC_L9_SetSPDIFVolume ( UINT32 ui32Volume )
{
	KDRV_ADEC_SPDIF_PARAM_T spdifParam;

	ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	spdifParam.volctrl = 1;
	spdifParam.volume  = ui32Volume;

	ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	ADEC_PRINT("ADEC_DRV_SetSPDIFVolume 0x%X : OK\n", ui32Volume );
	return RET_OK;
}


/**
 * Sets a SPDIF mute on or off.
 * Stops the audio digital data output.
 * @see ADEC_DRV_SetSPDIFVolume
*/
int ADEC_L9_SetSPDIFMute ( BOOLEAN bMuteFlag )
{
	KDRV_ADEC_SPDIF_PARAM_T spdifParam;

	ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	spdifParam.mute = bMuteFlag;

	ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	ADEC_PRINT("ADEC_DRV_SetSPDIFMute : %d\n", bMuteFlag );
	return RET_OK;
}


/**
 * Enables or disables the AV Lipsync mechanism.
 * Controls the lipsync function manually.
 * @see
*/
int ADEC_L9_EnableAVLipSync ( LX_ADEC_SYNC_T i_eAVLipSync )
{
	KDRV_ADEC_MOD_T 			module;

	KDRV_ADEC_LIPSYNC_PARAM_T 	lipsyncParam;

	//Check a adec mode
	if(i_eAVLipSync.adecMode == LX_ADEC_DECODE_MAIN)
	{
		module = KDRV_ADEC_DEC_0;
	}
	else if(i_eAVLipSync.adecMode == LX_ADEC_DECODE_ASSO)
	{
		module = KDRV_ADEC_DEC_1;
	}
	else if(i_eAVLipSync.adecMode == LX_ADEC_MIX_BUF0)
	{
		module = KDRV_ADEC_MP_02;
	}
	else if(i_eAVLipSync.adecMode == LX_ADEC_MIX_BUF1)
	{
		module = KDRV_ADEC_MP_03;
	}
	else if(i_eAVLipSync.adecMode == LX_ADEC_MIX_BUF2)
	{
		module = KDRV_ADEC_MP_04;
	}
	else if(i_eAVLipSync.adecMode == LX_ADEC_MIX_BUF3)
	{
		module = KDRV_ADEC_MP_05;
	}
	else if(i_eAVLipSync.adecMode == LX_ADEC_LLB_BUF)
	{
		module = KDRV_ADEC_MP_06;
	}
	else
	{
		module = KDRV_ADEC_DEC_0;

		ADEC_ERROR("ADEC_L9_EnableAVLipSync : adecMode = 0x%X!!!\n", i_eAVLipSync.adecMode);
	}

	ADEC_IPC_GetLipsync(module, &lipsyncParam);

	lipsyncParam.bOnOff = i_eAVLipSync.bOnOff;

	ADEC_IPC_SetLipsync(module, &lipsyncParam);

#ifndef  ENABLE_NETCAST_PRINT
	ADEC_ERROR("ADEC_DRV_EnableAVLipSync : %d\n", i_eAVLipSync.bOnOff);
#else
	ADEC_DEBUG("ADEC_DRV_EnableAVLipSync : %d\n", i_eAVLipSync.bOnOff);
#endif

	return RET_OK;
}


/**
 * Sets a delay time in milli-second of I2S Out of PCM.
 * Add a delay time for PCM input audio data.
 * @see ADEC_DRV_SetSPDIFDelayTime
*/
int ADEC_L9_SetI2SOutDelayTime ( UINT32 ui32DelayinMS )
{
	KDRV_ADEC_SPK_PARAM_T	spkParam;

	ADEC_IPC_GetConfig(KDRV_ADEC_SPK, &spkParam);

	spkParam.delayMS = ui32DelayinMS;

	//Change a lip sync offset to compensate DDCO output delay
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		if(spkParam.delayMS > DDCO_LIPSYNC_OFFSET_DELAY)
			spkParam.delayMS += DDCO_LIPSYNC_OFFSET_DELAY;	//30 ms
	}

	ADEC_IPC_SetConfig(KDRV_ADEC_SPK, &spkParam);

	ADEC_PRINT("ADEC_DRV_SetI2SOutDelayTime delay : %d ms\n", ui32DelayinMS );
	return RET_OK;
}

/**
 * Sets a delay time in milli-second of DAC Out of PCM.
 * Add a delay time for PCM input audio data.
 * @see ADEC_DRV_SetSPDIFDelayTime
*/
int ADEC_L9_SetDACOutDelayTime ( UINT32 ui32DelayinMS )
{
	KDRV_ADEC_SCART_PARAM_T 	scartParam;
	KDRV_ADEC_HP_PARAM_T 		hpParam;
	KDRV_ADEC_LINEOUT_PARAM_T 	lineoutParam;

	ADEC_IPC_GetConfig(KDRV_ADEC_SCART, &scartParam);

	scartParam.delayMS = ui32DelayinMS;

	//Change a lip sync offset to compensate DDCO output delay
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		if(scartParam.delayMS > DDCO_LIPSYNC_OFFSET_DELAY)
			scartParam.delayMS += DDCO_LIPSYNC_OFFSET_DELAY;	//30 ms
	}

	ADEC_IPC_SetConfig(KDRV_ADEC_SCART, &scartParam);

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_HP, &hpParam);

		hpParam.delayMS = ui32DelayinMS;

		//Change a lip sync offset to compensate DDCO output delay
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			if(hpParam.delayMS > DDCO_LIPSYNC_OFFSET_DELAY)
				hpParam.delayMS += DDCO_LIPSYNC_OFFSET_DELAY;	//30 ms
		}

		ADEC_IPC_SetConfig(KDRV_ADEC_HP, &hpParam);
	}
	else
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);

		lineoutParam.delayMS = ui32DelayinMS;

		ADEC_IPC_SetConfig(KDRV_ADEC_LINEOUT, &lineoutParam);
	}

	ADEC_PRINT("ADEC_DRV_SetDACOutDelayTime delay : %d ms\n", ui32DelayinMS );
	return RET_OK;
}

/**
 * Sets a delay time in milli-second of SPDIF.
 * Add a delay time for SPDIF input audio data.
 * @see ADEC_DRV_SetPCMDelayTime
*/
int ADEC_L9_SetSPDIFDelayTime ( UINT32 ui32DelayinMS )
{
	KDRV_ADEC_SPDIF_PARAM_T spdifParam;

	ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	spdifParam.delayMS = ui32DelayinMS;

	ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	ADEC_PRINT("ADEC_DRV_SetSPDIFDelayTime delay  : %d ms\n", ui32DelayinMS );
	return RET_OK;
}


/**
 * Sets a trick state of ADEC.
 * If ADEC is running in DVR mode(Pause or Resume state), ADEC trick state can be changed.
 * @see
*/
int ADEC_L9_SetTrickState ( LX_ADEC_TRICK_MODE_T	eTrickMode )
{
	int eRetVal = RET_OK;

	KDRV_ADEC_TRICK_MODE_T	trickMode = KDRV_ADEC_TRICK_NORMAL_SPEED;

	switch (eTrickMode)
	{
		case LX_ADEC_TRICK_FORCED_STOP_OFF:
			trickMode = 0x0; //Normal Speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Play\n" );
			break;

		case LX_ADEC_TRICK_FORCED_STOP_ON:
			trickMode = 0x2; //Forced Stop On
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Stop\n" );
			break;

		case LX_ADEC_TRICK_PAUSE:
			trickMode = 0x1; //Pause Mode
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Pause\n" );
			break;

		case LX_ADEC_TRICK_NORMAL_PLAY:
			//Normal playing
			trickMode = 0x0; //Normal Speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Normal Play\n" );
			break;

		case LX_ADEC_TRICK_FAST_FOWARD_1P2X:
			trickMode = 0x3; //Fast forward x1.2 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Fast forward x1.2 speed\n" );
			break;

		case LX_ADEC_TRICK_FAST_FOWARD_1P5X:
			trickMode = 0x4; //Fast forward x1.5 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Fast forward x1.5 speed\n" );
			break;

		case LX_ADEC_TRICK_FAST_FOWARD_2X:
			trickMode = 0x5; //Fast forward x2 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Fast forward x2 speed\n" );
			break;

		case LX_ADEC_TRICK_SLOW_MOTION_0P8X:
			trickMode = 0x6; //slow x0.8 speed
			ADEC_DEBUG("ADEC_DRV_SetTrickState : Slow x0.8 speed\n" );
			break;

		case LX_ADEC_TRICK_SLOW_MOTION_0P5X:
			trickMode = 0x5; //slow x0.5 speed
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

	ADEC_IPC_TrickMode(trickMode);

	ADEC_PRINT("ADEC_DRV_SetTrickState mode : %d\n", eTrickMode );
	return eRetVal;
}

/**
 * Gets a clock information of ADEC driver.
 * This function gets and sets a clock information of ADEC driver.
 * A clock information has STC, PTS and GSTC value.
*/
int ADEC_L9_GetClockInfo ( LX_ADEC_CLOCK_INFO_T *pstClockInfo )
{
	//Read Clock reg.
	AUD_RdFL(aud_gstcchreg);
	AUD_RdFL(aud_gstcclreg);

	//Read GSTC, STC, PTSM and PTSA
	pstClockInfo->clockGSTCH = AUD_Rd(aud_gstcchreg);
	pstClockInfo->clockGSTCL = AUD_Rd(aud_gstcclreg);

	ADEC_PRINT("ADEC_DRV_GetAndSetClockInfo GSTCL : %d\n", pstClockInfo->clockGSTCL );
	return RET_OK;
}


/**
 * Sets a audio balance.
 * Range of input parameter value is 0 to 100.
 * Value 50 is the center value.
 * @see ADEC_DRV_SetBASS
*/
int ADEC_L9_SetBalance ( UINT8 ui8BalanceVal )
{
	KDRV_ADEC_SPK_PARAM_T spkParam;

	ADEC_IPC_GetConfig(KDRV_ADEC_SPK, &spkParam);

	spkParam.balance = ui8BalanceVal - 50;

	ADEC_IPC_SetConfig(KDRV_ADEC_SPK, &spkParam);

	ADEC_PRINT("ADEC_DRV_SetBalance level : %d\n", ui8BalanceVal );
	return RET_OK;
}


/**
 * Sets a audio bass.
 * Range of input parameter value is 0 to 100.
 * Value 50 is the center value.
 * @see ADEC_DRV_SetBalance
*/
int ADEC_L9_SetBASS ( UINT8 ui8BassVal )
{
	ADEC_IPC_SetBass(ui8BassVal);

	ADEC_PRINT("ADEC_DRV_SetBASS level : %d\n", ui8BassVal );
	return RET_OK;
}


/**
 * Sets a audio Treble.
 * Range of input parameter value is 0 to 100.
 * Value 50 is the center value.
 * @see ADEC_DRV_SetBASS
*/
int ADEC_L9_SetTreble ( UINT8 ui8TrebleVal )
{
	ADEC_IPC_SetTreble(ui8TrebleVal);

	ADEC_PRINT("ADEC_DRV_SetTreble level : %d\n", ui8TrebleVal );
	return RET_OK;
}

/**
 * Sets a Sound Engine(SE) Function mode and paramters.
 * The Function mode and each paramter size is defined.
 * @see
*/
int ADEC_L9_SESetFunction ( LX_ADEC_SE_FN_PARAM_T *pstSeFnParam )
{
	UINT32		seParamAddr = 0;
	UINT16		seParamSize	= 0;

	KDRV_ADEC_SE_PARAM_T seParam;

#ifdef ENABLE_SE_PARAM_DEBUG
	UINT16		ui16Count = 0;
#endif

	//Sanity check
	if( (pstSeFnParam->pParams == NULL)	\
	  ||(pstSeFnParam->noParam == 0) || (pstSeFnParam->noParam >= 1024) )
	{
		ADEC_ERROR("ADEC_L9_SESetFunction : error!!!(pParams = %p, noParam = %d)\n", pstSeFnParam->pParams, pstSeFnParam->noParam);
		return RET_ERROR;
	}

	/* Set a SE function mode */
	if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_SET)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		{
			seParamAddr = ADDR_SE_MODE_VARI_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_SET option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(seParamSize != LX_SE_MODE_VARI_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_SET size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN001)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		{
			seParamAddr = ADDR_SE_FN001_VARI_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN001 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN001;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(seParamSize != LX_SE_FN001_VARI_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN001 size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN000)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		{
			seParamAddr = ADDR_SE_FN000_INIT_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else if(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		{
			seParamAddr = ADDR_SE_FN000_VARI_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN000 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN000;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		  &&(seParamSize != LX_SE_FN000_INIT_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN000 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
		else if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(seParamSize != LX_SE_FN000_VARI_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN000 VARI size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN005)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		{
			seParamAddr = ADDR_SE_FN005_INIT_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN005 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN005;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		  &&(seParamSize != LX_SE_FN005_INIT_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN005 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN003)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		{
			seParamAddr = ADDR_SE_FN003_INIT_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN003 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN003;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		  &&(seParamSize != LX_SE_FN003_INIT_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN003 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN004_MODE1)
	{
		/* Set a SE parameter address and size */
		if(	(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(pstSeFnParam->varOption == LX_ADEC_SE_MODE_VARIABLESALL) )
		{
			seParamAddr = ADDR_SE_FN004_MOD1_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
	 		   &&(pstSeFnParam->varOption < LX_ADEC_SE_MODE_VARIABLESALL) )
		{
			seParamAddr = ADDR_SE_FN004_MOD1_PARAM + (pstSeFnParam->varOption * LX_SE_FN004_MOD1_PARAM_SIZE);
			seParamSize  = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN004_MODE1 option = %d, var = %d.\n", pstSeFnParam->dataOption, pstSeFnParam->varOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN004_MODE1;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(pstSeFnParam->varOption < LX_ADEC_SE_MODE_VARIABLESALL)
		  &&(seParamSize != LX_SE_FN004_MOD1_PARAM_SIZE) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN004_MODE1 VARI size = %d.\n", seParamSize);
			return RET_ERROR;
		}
		else if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		 	   &&(pstSeFnParam->varOption == LX_ADEC_SE_MODE_VARIABLESALL)
		  	   &&(seParamSize != LX_SE_FN004_MOD1_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN004_MODE1 VARI ALL size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN004_MODE2)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		{
			seParamAddr = ADDR_SE_FN004_MOD2_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN004_MODE2 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN004_MODE2;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(seParamSize != LX_SE_FN004_MOD2_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN004_MODE2 size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN004_MODE3)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		{
			seParamAddr = ADDR_SE_FN004_MOD3_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN004_MODE3 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN004_MODE3;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		  &&(seParamSize != LX_SE_FN004_MOD3_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN004_MODE3 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN002)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		{
			seParamAddr = ADDR_SE_FN002_INIT_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN002 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN002;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		  &&(seParamSize != LX_SE_FN002_INIT_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN002 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN009)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		{
			seParamAddr = ADDR_SE_FN009_INIT_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else if(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		{
			seParamAddr = ADDR_SE_FN009_VARI_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN009 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN009;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		  &&(seParamSize != LX_SE_FN009_INIT_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN009 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
		else if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(seParamSize != LX_SE_FN009_VARI_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN009 VARI size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN006)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		{
			seParamAddr = ADDR_SE_FN006_VARI_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN006 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN006;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(seParamSize != LX_SE_FN006_VARI_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN006 VARI size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN007)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		{
			seParamAddr = ADDR_SE_FN007_INIT_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else if(pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		{
			seParamAddr = ADDR_SE_FN007_VARI_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN007 option = %d.\n", pstSeFnParam->dataOption);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN007;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->dataOption == LX_ADEC_SE_INIT_ONLY)
		  &&(seParamSize != LX_SE_FN007_INIT_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN007 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
		else if( (pstSeFnParam->dataOption == LX_ADEC_SE_VARIABLES)
		  &&(seParamSize != LX_SE_FN007_VARI_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN007 VARI size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN010)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->accessMode == LX_ADEC_SE_WRITE)
		{
			//Clears the sound level for left and righr channel
			AUD_Wr(aud_levelmo_l, 0x0);
			AUD_WrFL(aud_levelmo_l);

			AUD_Wr(aud_levelmo_r, 0x0);
			AUD_WrFL(aud_levelmo_r);

			seParamAddr = ADDR_SE_FN010_INIT_PARAM;
			seParamSize = pstSeFnParam->noParam;
		}
		else
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN010 access = %d.\n", pstSeFnParam->accessMode);
			return RET_ERROR;
		}

		/* Update a Sound Engine Mask */
		g_adec_kdrv.soundEngineMask |= MASK_SE_FN_MODE_FN010;

#ifdef ENABLE_SE_PARAM_DEBUG
		/* Sanity Check */
		if( (pstSeFnParam->accessMode == LX_ADEC_SE_WRITE)
		  &&(seParamSize != LX_SE_FN010_INIT_PARAM_NUM) )
		{
			ADEC_ERROR("ADEC_L9_SESetFunction : FN_MODE_FN010 INIT size = %d.\n", seParamSize);
			return RET_ERROR;
		}
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG
	}
	else
	{
		ADEC_ERROR("ADEC_L9_SESetFunction : Function Mode Error!!!(%d)\n", pstSeFnParam->fnMode);
		return RET_ERROR;
	}

	ADEC_PRINT("ADEC_L9_SESetFunction : addr = 0x%04X, size = %d(%d)\n", (UINT32)seParamAddr, seParamSize, sizeof(UINT32)*seParamSize);
	ADEC_DEBUG("ADEC_L9_SESetFunction : addr = 0x%04X, size = %d(%d)\n", (UINT32)seParamAddr, seParamSize, sizeof(UINT32)*seParamSize);

	//Copy Sound Engine(SE)  parameters
#if 0
	for(ui16Count = 0; ui16Count < seParamSize; ui16Count++)
	{
		g_pui32SEParamVirAddr[seParamAddr + ui16Count] = *(pstSeFnParam->pParams + ui16Count);
	}
#else
	memcpy(g_pui32SEParamVirAddr + seParamAddr, pstSeFnParam->pParams, sizeof(UINT32) * seParamSize);
#endif

#ifdef ENABLE_SE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_ERROR("SE Param Start...[0x%8X]\n", g_pui32SEParamVirAddr[seParamAddr]);
	for(ui16Count = 0; ui16Count < seParamSize; ui16Count++)
	//for(ui16Count = 0; ui16Count < 3; ui16Count++)
	{
		ADEC_ERROR("Param[%3d] = 0x%8X\n", ui16Count + 1, g_pui32SEParamVirAddr[seParamAddr + ui16Count]);
	}
	ADEC_ERROR("SE Param End...[0x%8X]\n", g_pui32SEParamVirAddr[seParamAddr + seParamSize - 1]);
#endif		//#ifdef ENABLE_SE_PARAM_DEBUG

	ADEC_IPC_GetConfig(KDRV_ADEC_SE, &seParam);

	//Send IPC command
	if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_SET)
	{
		seParam.seFnParam.ui32FnMode = pstSeFnParam->fnMode;

		memcpy(seParam.seSetMode.ui32Param, pstSeFnParam->pParams, sizeof(KDRV_ADEC_SE_SET_MODE_T));
	}
	else
	{
		seParam.seFnParam.ui32FnMode 	 = pstSeFnParam->fnMode;
		seParam.seFnParam.ui32DataOption = pstSeFnParam->dataOption;
		seParam.seFnParam.ui32VarOption  = pstSeFnParam->varOption;
		seParam.seFnParam.ui32AccessMode = pstSeFnParam->accessMode;
		seParam.seFnParam.ui32NoParam 	 = pstSeFnParam->noParam;
	}

	ADEC_IPC_SetConfig(KDRV_ADEC_SE, &seParam);

	return RET_OK;
}

/**
 * Gets a Sound Engine(SE) Function mode and paramters.
 * The Function mode and each paramter size is defined.
 * @see
*/
int ADEC_L9_SEGetFunction ( LX_ADEC_SE_FN_PARAM_T *pstSeFnParam )
{
	if(pstSeFnParam->fnMode == LX_ADEC_SE_FN_MODE_FN010)
	{
		/* Set a SE parameter address and size */
		if(pstSeFnParam->accessMode == LX_ADEC_SE_READ)
		{
			//Read the sound level for left and righr channel
			AUD_RdFL(aud_levelmo_l);
			AUD_RdFL(aud_levelmo_r);

			//Copy value
			pstSeFnParam->pParams[0] = AUD_Rd(aud_levelmo_l);	//left volume
			pstSeFnParam->pParams[1] = AUD_Rd(aud_levelmo_r);	//right volume
		}
		else
		{
			//Copy value
			pstSeFnParam->pParams[0] = 0;	//left volume
			pstSeFnParam->pParams[1] = 0;	//right volume

			ADEC_ERROR("ADEC_L9_SEGetFunction : FN_MODE_FN010 access = %d.\n", pstSeFnParam->accessMode);
			return RET_ERROR;
		}

	}

	ADEC_PRINT("ADEC_L9_SEGetFunction : L(0x%x), R(0x%x)\n", pstSeFnParam->pParams[0], pstSeFnParam->pParams[1]);

#if 0
	UINT16	addrOfParam = 0, sizeOfParams = 0;
	UINT16	ui16Count;

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
	for(ui16Count = 0; ui16Count < sizeOfParams; ui16Count++)
	{
		pstCVGetParam->getParams[ui16Count] = g_pui32LGSEParamVirAddr[addrOfParam + ui16Count];
	}

#ifdef ENABLE_LGSE_PARAM_DEBUG
	//Read parameter value for debug
	ADEC_DEBUG("Getted Param Start...\n");
	for(ui16Count = 0; ui16Count < sizeOfParams; ui16Count++)
	{
		ADEC_DEBUG("Getted Param[%3d] = 0x%8X\n", ui16Count + 1, pstCVGetParam->getParams[ui16Count]);
	}
	ADEC_DEBUG("Getted Param End...\n");
#endif		//#ifdef ENABLE_LGSE_PARAM_DEBUG

	ADEC_DEBUG("ADEC_DRV_CVGetParam : Num = %d, Param[0] = 0x%X\n", pstCVGetParam->sizeOfParams, pstCVGetParam->getParams[0]);
#endif

	return RET_OK;
}


/**
 * Feeds an audio clip.
 * This function copies the audio clip data into ADEC driver memory.
 * @see ADEC_DRV_StartAudioClip
*/
int ADEC_L9_FeedAudioClip ( LX_ADEC_CLIP_MEM_INFO_T stMemInfo )
{
	int ret = ADEC_ERR_NONE;

	UINT8	decNum = LX_ADEC_DEC_0;
	UINT8	mixNum = LX_ADEC_MIXER_0;

	KDRV_ADEC_MOD_T 	adecModule = KDRV_ADEC_DEC_0;

	UINT32	ui32FreeAuCount;
	UINT64	ui64TS90kHzTick;

	ADEC_BUF_T  	*pAdecBuf = NULL;
	ADEC_AU_INFO_T 	info = {0, };

#ifdef ENABLE_AAC_PARSER
	int							aacProbeStatus;
	LX_ADEC_AAC_START_INFO_T 	aacStartInfo;
	ADEC_AAC_MEDIA_INFO_T		mediaInfo;
#endif	//#ifdef ENABLE_AAC_PARSER

	//Create a buffer if not created
	ret = ADEC_L9_CreateAudioClipBuffer(stMemInfo.adecMode);

	if(ret != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", ret);
		return RET_ERROR;
	}

	//Check a adec mode and create cpb buffer
	if(stMemInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum 	   = LX_ADEC_DEC_0;
		adecModule = KDRV_ADEC_DEC_0;

		//assign a buffer for CPBA
		pAdecBuf = (ADEC_BUF_T *)g_pCPBM_WriterStruct;
	}
	else if(stMemInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum 	   = LX_ADEC_DEC_1;
		adecModule = KDRV_ADEC_DEC_1;

		//assign a buffer for CPBA
		pAdecBuf = (ADEC_BUF_T *)g_pCPBA_WriterStruct;
	}
	else if(stMemInfo.adecMode == LX_ADEC_MIX_BUF0)
	{
		mixNum = LX_ADEC_MIXER_0;

		adecModule = KDRV_ADEC_MP_02;

		//assign a buffer for MIX0
		pAdecBuf = (ADEC_BUF_T *)g_pMIX0_WriterStruct;
	}
	else if(stMemInfo.adecMode == LX_ADEC_MIX_BUF1)
	{
		mixNum = LX_ADEC_MIXER_1;

		adecModule = KDRV_ADEC_MP_03;

		//assign a buffer for MIX1
		pAdecBuf = (ADEC_BUF_T *)g_pMIX1_WriterStruct;
	}
	else if(stMemInfo.adecMode == LX_ADEC_MIX_BUF2)
	{
		mixNum = LX_ADEC_MIXER_2;

		adecModule = KDRV_ADEC_MP_04;

		//assign a buffer for MIX2
		pAdecBuf = (ADEC_BUF_T *)g_pMIX2_WriterStruct;
	}
	else if(stMemInfo.adecMode == LX_ADEC_MIX_BUF3)
	{
		mixNum = LX_ADEC_MIXER_3;

		adecModule = KDRV_ADEC_MP_05;

		//assign a buffer for MIX3
		pAdecBuf = (ADEC_BUF_T *)g_pMIX3_WriterStruct;
	}
	else if(stMemInfo.adecMode == LX_ADEC_LLB_BUF)
	{
		mixNum = LX_ADEC_MIXER_4;

		adecModule = KDRV_ADEC_MP_06;

		//assign a buffer for LLB
		pAdecBuf = (ADEC_BUF_T *)g_pLLB_WriterStruct;
	}
	else
	{
		ADEC_ERROR("FeedAudioClip : adecMode = 0x%X!!!\n", stMemInfo.adecMode);
	}

	//sanity check
	if(pAdecBuf == NULL)
	{
		ADEC_ERROR("Feed : pAdecBuf is NULL(adecMode = 0x%X)!!!\n", stMemInfo.adecMode);
		return RET_ERROR;
	}

	/* Set a parameter for clip play */
	if(stMemInfo.adecMode == LX_ADEC_DECODE_MAIN || stMemInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		if(stMemInfo.bufSize > 0 || stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_END)	//In case of LX_ADEC_CLIP_BUF_END, bufSize is zero.
		{
			ADEC_PRINT("Feed : CPB(%d) => Free:%d, Used:%d\n",	\
					    decNum, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_used_size(pAdecBuf));

			/* Compute free au count */
			ui32FreeAuCount = pAdecBuf->get_max_au(pAdecBuf) - pAdecBuf->get_au_cnt(pAdecBuf);

			/* Check buffer overflow and AUI overflow */
			if( (pAdecBuf->get_free_size(pAdecBuf) >=  stMemInfo.bufSize)
			  &&(ui32FreeAuCount > 1) )
			{
				/* Check a TVP Path to manuplate 90Khz gstc clock value. */
				if(stMemInfo.bIsTvpPath == FALSE)
				{
					ui64TS90kHzTick = stMemInfo.timestamp;
					if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
					{
						ui64TS90kHzTick *= 9;

						//Check divide by zero value.
						if(ui64TS90kHzTick)
						{
							do_div(ui64TS90kHzTick, 100000);	// = Xns * 90 000 / 1 000 000 000
						}
					}
				}
				else
				{
					ui64TS90kHzTick = stMemInfo.timestamp;
				}

				//Set NEW AUI info.
				info.size	   = (UINT64)stMemInfo.bufSize;
				info.timestamp = (UINT32)(ui64TS90kHzTick) & 0x0FFFFFFF;	//28 bit
				info.gstc	   = (UINT32)stMemInfo.dtsGstc;
				info.index	   = (UINT16)g_adec_kdrv.index[decNum].FeedIDX;
				info.lastAu	   = 0;

				ADEC_PRINT("pts[%10d], dts[%10d], idx[%5d], size[%5llu]\n",	\
							info.timestamp, info.gstc, info.index, info.size);
				ADEC_IPC_DEBUG("D[%d]%10d+%5d\n", decNum, info.timestamp, info.index);

				//Write AUI info.
				ret = pAdecBuf->create_new_au(&info, pAdecBuf);
				if(ret != RET_OK)
				{
					ADEC_ERROR("Feed : CPB(%d) => create_new_au(ret = %d, cnt = %d, free = %d)!!!\n",	\
								decNum, ret, pAdecBuf->get_au_cnt(pAdecBuf), pAdecBuf->get_free_size(pAdecBuf));
					return RET_ERROR;
				}

			    //Write ES data
			    ret = pAdecBuf->write_data(stMemInfo.pClipBuffer, stMemInfo.bufSize, pAdecBuf);
				if(ret != RET_OK)
				{
					ADEC_ERROR("Feed : CPB(%d) => write_data(ret = %d, cnt = %d, free = %d)!!!\n",	\
								decNum, ret, pAdecBuf->get_au_cnt(pAdecBuf), pAdecBuf->get_free_size(pAdecBuf));
					return RET_ERROR;
				}

				//Check buffer status
				if(stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_END)
				{
					//Set last buffer
					ret = BufHelper_SetLast(pAdecBuf);
					if(ret != RET_OK)
					{
						ADEC_ERROR("Feed : CPB(%d) => BufHelper_SetLast(ret = %d, free = %d, cnt = %d)!!!\n",	\
									decNum, ret, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_au_cnt(pAdecBuf));
						return RET_ERROR;
					}
				}

				//Write pts info.
				g_adec_kdrv.index[decNum].AUI[g_adec_kdrv.index[decNum].FeedIDX].streamtime = stMemInfo.timestamp;
				g_adec_kdrv.index[decNum].AUI[g_adec_kdrv.index[decNum].FeedIDX].pts = stMemInfo.dtsGstc;

				//Increase ES AUI index
				g_adec_kdrv.index[decNum].FeedIDX++;

				//Check max value for ES AUI index
				if(g_adec_kdrv.index[decNum].FeedIDX == CPB_AUI_INDEX_COUNT)
					g_adec_kdrv.index[decNum].FeedIDX = 0;

				/* Increase CPBM buffer loading count */
				g_adec_kdrv.index[decNum].FeedCNT++;

				/* Update Internal Status Value */
				g_adec_kdrv.clipBufStatus[decNum] = stMemInfo.bufStatus;
			}
			else
			{
				ADEC_DEBUG("Feed : CPB(%d) => free = %d, count = %d!!!\n", 	\
							decNum, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_au_cnt(pAdecBuf));

				if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
				{
					ADEC_ERROR("Feed : CPB(%d) => free = %d, count = %d!!!\n", 	\
								decNum, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_au_cnt(pAdecBuf));
				}

			    ret = ADEC_ERR_OVERFLOW;
			}
		}
		else
		{
			ADEC_ERROR("Feed : CPB(%d) => stMemInfo.bufSize = %d!!!\n", 	\
						decNum, stMemInfo.bufSize);

		    ret = RET_ERROR;
		}

		//Set a Mixer stop for GCD platform if decoder is started after flush command
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			/* LX_ADEC_CLIP_MODE_PAUSE condition is added for abnormal  case
			     Case :  pause - flush -<abnormal pasue> - feed case */
			if( (g_adec_kdrv.clipStatus[decNum]  == LX_ADEC_CLIP_MODE_FLUSH)
			  ||(g_adec_kdrv.clipStatus[decNum]  == LX_ADEC_CLIP_MODE_PAUSE)
			  ||(g_adec_kdrv.bDecStarted[decNum] == FALSE) )
			{
				//Send a IPC command
				ADEC_IPC_Resume(adecModule);

				/* Update Internal Status Value */
				g_adec_kdrv.bDecStarted[decNum] = TRUE;
				g_adec_kdrv.clipStatus[decNum]  = LX_ADEC_CLIP_MODE_LOAD;
			}
		}

#ifdef ENABLE_AAC_PARSER
		/* Probe AAC codec ES data for AAC profile and format detection */
		if( (g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_AAC)
		  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_HEAAC) )
		{
			if(g_adec_kdrv.bAacParsed[decNum] == FALSE)
			{
				if(stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_START && g_adec_kdrv.aacProbe[decNum] == NULL)
				{
					/* Create AAC probe parser */
					g_adec_kdrv.aacProbe[decNum] = ADEC_AAC_Probe_Create();

					if(g_adec_kdrv.aacProbe[decNum] == NULL)
					{
						ADEC_ERROR("Feed : error in ADEC_AAC_Probe_Create call\n");
					}
				}

				/* Probe ES data */
				aacProbeStatus = ADEC_AAC_Probe_CheckEs(stMemInfo.pClipBuffer, stMemInfo.bufSize, g_adec_kdrv.aacProbe[decNum]);

				/* Check if AAC ES data is parsed. */
				if(g_adec_kdrv.bAacDSIParsed[decNum] == TRUE)
				{
					if((aacProbeStatus >= 0) && (g_adec_kdrv.index[decNum].FeedCNT >= AAC_ES_PARSER_CHECK_NUMBER))
					{
						aacStartInfo.adecMode = stMemInfo.adecMode;

						/* Get AAC Codec, Format type and sampling frequency from ES data */
						aacStartInfo.aacCodec  = (LX_ADEC_AAC_CODEC_TYPE_T)ADEC_AAC_Probe_GetCodecType(g_adec_kdrv.aacProbe[decNum]);
						aacStartInfo.aacFormat = (LX_ADEC_AAC_FORMAT_TYPE_T)ADEC_AAC_Probe_GetFormatType(g_adec_kdrv.aacProbe[decNum]);

						ret = ADEC_AAC_Probe_GetMediaInfo(&mediaInfo, g_adec_kdrv.aacProbe[decNum]);

						if(ret == RET_OK)
						{
							aacStartInfo.samplingFreq = ADEC_L9_GetAACSamplingFreq(mediaInfo);
						}
						else
						{
							aacStartInfo.samplingFreq = LX_ADEC_SAMPLING_FREQ_NONE;
						}

						ADEC_PRINT("AAC PROBE : codec = %d, foramt = %d, sfreq = %d\n", aacStartInfo.aacCodec, aacStartInfo.aacFormat, aacStartInfo.samplingFreq);

						/* ioctl function call */
						ret = ADEC_L9_StartAacAudioClip(aacStartInfo);

						if(ret != RET_OK)
						{
							ADEC_ERROR("Feed : error in ADEC_L9_StartAacAudioClip call\n");
						}

						/* Save AAC ES parsing status */
						g_adec_kdrv.bAacParsed[decNum] = TRUE;
					}

					ADEC_DEBUG("Feed : AAC Probe buffer status = %d(count = %d)!!!\n", stMemInfo.bufStatus, g_adec_kdrv.index[decNum].FeedCNT );
				}
			}
		}
#endif	//#ifdef ENABLE_AAC_PARSER
	}
	else if( (stMemInfo.adecMode >= LX_ADEC_MIX_BUF0)
		   &&(stMemInfo.adecMode <= LX_ADEC_MIX_BUF3 || stMemInfo.adecMode == LX_ADEC_LLB_BUF) )
	{
		if(stMemInfo.bufSize > 0 || stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_END)	//In case of LX_ADEC_CLIP_BUF_END, bufSize is zero.
		{
			ADEC_PRINT("Feed : Mix(0x%X) => Free:%d, Used:%d\n", stMemInfo.adecMode, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_used_size(pAdecBuf));

			/* Compute free au count */
			ui32FreeAuCount = pAdecBuf->get_max_au(pAdecBuf) - pAdecBuf->get_au_cnt(pAdecBuf);

			/* Check buffer overflow and AUI overflow */
			if( (pAdecBuf->get_free_size(pAdecBuf) >=  stMemInfo.bufSize)
			  &&(ui32FreeAuCount > 1) )
			{
				/* Check a TVP Path to manuplate 90Khz gstc clock value. */
				if(stMemInfo.bIsTvpPath == FALSE)
				{
					ui64TS90kHzTick = stMemInfo.timestamp;
					if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
					{
						ui64TS90kHzTick *= 9;

						//Check divide by zero value.
						if(ui64TS90kHzTick)
						{
							do_div(ui64TS90kHzTick, 100000);	// = Xns * 90 000 / 1 000 000 000
						}
					}
				}
				else
				{
					ui64TS90kHzTick = stMemInfo.timestamp;
				}

				//Set NEW AUI info.
				info.size	   = (UINT64)stMemInfo.bufSize;
				info.timestamp = (UINT32)(ui64TS90kHzTick) & 0x0FFFFFFF;	//28 bit
				info.gstc	   = (UINT32)stMemInfo.dtsGstc;
				info.index	   = (UINT16)g_adec_kdrv.mixerIndex[mixNum];
				info.lastAu	   = 0;

				ADEC_PRINT("pts[%10d], dts[%10d], idx[%5d], size[%5llu]\n",	\
							info.timestamp, info.gstc, info.index, info.size);
				ADEC_IPC_DEBUG("M[%d]%10d+%5d\n", mixNum, info.timestamp, info.index);

				//Write AUI info.
				ret = pAdecBuf->create_new_au(&info, pAdecBuf);
				if(ret != RET_OK)
				{
					ADEC_ERROR("Feed : Mix(0x%X) => create_new_au(ret = %d, cnt = %d, free = %d)!!!\n",	\
								stMemInfo.adecMode, ret, pAdecBuf->get_au_cnt(pAdecBuf), pAdecBuf->get_free_size(pAdecBuf));
					return RET_ERROR;
				}

			    //Write Mix data
			    ret = pAdecBuf->write_data(stMemInfo.pClipBuffer, stMemInfo.bufSize, pAdecBuf);
				if(ret != RET_OK)
				{
					ADEC_ERROR("Feed : Mix(0x%X) => write_data(ret = %d, cnt = %d, free = %d)!!!\n",	\
								stMemInfo.adecMode, ret, pAdecBuf->get_au_cnt(pAdecBuf), pAdecBuf->get_free_size(pAdecBuf));
					return RET_ERROR;
				}

				//Check buffer status
				if(stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_END)
				{
					//Set last buffer
					ret = BufHelper_SetLast(pAdecBuf);
					if(ret != RET_OK)
					{
						ADEC_ERROR("Feed : Mix(0x%X) => BufHelper_SetLast(ret = %d, free = %d, cnt = %d)!!!\n",	\
									stMemInfo.adecMode, ret, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_au_cnt(pAdecBuf));
						return RET_ERROR;
					}
				}

				//Increase Mix AUI index
				g_adec_kdrv.mixerIndex[mixNum]++;

				//Check max value for Mix AUI index
				if(g_adec_kdrv.mixerIndex[mixNum] == MPB_AUI_INDEX_COUNT)
					g_adec_kdrv.mixerIndex[mixNum] = 0;
			}
			else
			{
				ADEC_DEBUG("Feed : Mix(0x%X) => free = %d, count = %d!!!\n", 	\
							stMemInfo.adecMode, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_au_cnt(pAdecBuf));

				if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
				{
					ADEC_DEBUG("Feed : Mix(0x%X) => free = %d, count = %d!!!\n", 	\
								stMemInfo.adecMode, pAdecBuf->get_free_size(pAdecBuf), pAdecBuf->get_au_cnt(pAdecBuf));
				}

			    ret = ADEC_ERR_OVERFLOW;
			}
		}
		else
		{
			ADEC_ERROR("Feed : Mix(0x%X) => stMemInfo.bufSize = %d!!!\n", 	\
						stMemInfo.adecMode, stMemInfo.bufSize);
		    ret = RET_ERROR;
		}

		//Set a Mixer stop for GCD platform if mixer is started after flush command
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			if(g_adec_kdrv.mixerStatus[mixNum] == LX_ADEC_CLIP_MODE_FLUSH)
			{
				/* Update Internal Status Value */
				g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_LOAD;
			}
		}
	}
	else
	{
		ADEC_ERROR("Feed : mode is not defined(adecMode = 0x%X)!!!\n", stMemInfo.adecMode);
		return RET_ERROR;
	}

	//Check a buffer start or end case
	if( (stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_START)
	  ||(stMemInfo.bufStatus == LX_ADEC_CLIP_BUF_END) )
	{
		ADEC_DEBUG("Clip Feed   mode = 0x%x, bstat = %d, info.lastAu = %d\n",	\
					stMemInfo.adecMode, stMemInfo.bufStatus, info.lastAu);
	}

	ADEC_PRINT("ADEC_DRV_FeedAudioClip : mode [0x%x], bstat [0x%x]\n",	\
				stMemInfo.adecMode, stMemInfo.bufStatus );

#ifndef  ENABLE_NETCAST_PRINT
	if(ret != 0)
		ADEC_ERROR("FEED RET[%d]M[%d]F[%x]BS[%x]\n",ret,stMemInfo.adecMode,pAdecBuf->get_free_size(pAdecBuf),stMemInfo.bufSize );
#endif

 	return ret;
}

/**
 * Starts a playing of the audio clip.
 * Audio clip must be loaded by the ADEC_IOW_LOAD_AUDIO_CLIP function for using this function.
 * @see ADEC_DRV_LoadAudioClip
*/
int ADEC_L9_StartAudioClip ( LX_ADEC_CLIP_START_INFO_T stStartInfo )
{
	int eRetVal = RET_OK;

	UINT8	decNum = LX_ADEC_DEC_0;
	UINT8	mixNum = LX_ADEC_MIXER_0;
	UINT8	avLipSyncNum = 0;

	UINT32	vdecDecCbt = 0;
	UINT32	vdecDecSbt = 0;
	UINT32	adecDecSbt = 0;

	UINT64 	ui64TS90kHzTick = 0;
	UINT64 	clockBaseTime   = 0;
	UINT64 	streamBaseTime  = 0;

	UINT8 	*buffer = (UINT8 *)stStartInfo.pDSIBuffer;
	UINT32 	i, length, encoderOption0 = 0, encoderOption1 = 0;

	ADEC_BUF_T	*pAdecBuf = NULL;

	KDRV_ADEC_MOD_T 				adecModule 		= KDRV_ADEC_DEC_0;
	KDRV_ADEC_MOD_T 				adecMixerModule = KDRV_ADEC_MP_00;

	KDRV_ADEC_INPUT_PORT_T			inputMode =	KDRV_ADEC_INPUT_SYSTEM_0;

	KDRV_ADEC_DEC_PARAM_T			decParam;
	KDRV_ADEC_AC3_INFO_T 			ac3Info;
	KDRV_ADEC_AAC_INFO_T 			aacInfo;
	KDRV_ADEC_ADPCM_INFO_T 			adpcmInfo;

	KDRV_ADEC_MIX_START_PARAM_T		mixStartParam;

	ADEC_CMD_SYNC_BASE_T 			syncBase;
	KDRV_ADEC_LIPSYNC_PARAM_T 		decLipsyncParam;
	KDRV_ADEC_LIPSYNC_PARAM_T 		mixLipsyncParam;

	//Create a buffer if not created
	eRetVal = ADEC_L9_CreateAudioClipBuffer(stStartInfo.adecMode);

	if(eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
		return RET_ERROR;
	}

	//Check a adec mode
	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum	   		= LX_ADEC_DEC_0;
		adecModule 		= KDRV_ADEC_DEC_0;
		adecMixerModule = KDRV_ADEC_MP_00;
		inputMode  		= KDRV_ADEC_INPUT_SYSTEM_0;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pCPBM_WriterStruct;
	}
	else if(stStartInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum	   		= LX_ADEC_DEC_1;
		adecModule 		= KDRV_ADEC_DEC_1;
		adecMixerModule = KDRV_ADEC_MP_01;
		inputMode 		= KDRV_ADEC_INPUT_SYSTEM_1;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pCPBA_WriterStruct;
	}
	else if(stStartInfo.adecMode == LX_ADEC_MIX_BUF0)
	{
		mixNum			= LX_ADEC_MIXER_0;
		adecModule 		= KDRV_ADEC_MP_02;
		adecMixerModule	= KDRV_ADEC_MP_02;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pMIX0_WriterStruct;
	}
	else if(stStartInfo.adecMode == LX_ADEC_MIX_BUF1)
	{
		mixNum	   		= LX_ADEC_MIXER_1;
		adecModule 		= KDRV_ADEC_MP_03;
		adecMixerModule	= KDRV_ADEC_MP_03;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pMIX1_WriterStruct;
	}
	else if(stStartInfo.adecMode == LX_ADEC_MIX_BUF2)
	{
		mixNum	   		= LX_ADEC_MIXER_2;
		adecModule 		= KDRV_ADEC_MP_04;
		adecMixerModule	= KDRV_ADEC_MP_04;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pMIX2_WriterStruct;
	}
	else if(stStartInfo.adecMode == LX_ADEC_MIX_BUF3)
	{
		mixNum	   		= LX_ADEC_MIXER_3;
		adecModule 		= KDRV_ADEC_MP_05;
		adecMixerModule	= KDRV_ADEC_MP_05;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pMIX3_WriterStruct;
	}
	else if(stStartInfo.adecMode == LX_ADEC_LLB_BUF)
	{
		mixNum	   		= LX_ADEC_MIXER_4;
		adecModule 		= KDRV_ADEC_MP_06;
		adecMixerModule	= KDRV_ADEC_MP_06;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pLLB_WriterStruct;
	}
	else
	{
		ADEC_ERROR("FeedAudioClip : adecMode = 0x%X!!!\n", stStartInfo.adecMode);
		return RET_ERROR;
	}

	//sanity check
	if(pAdecBuf == NULL)
	{
		ADEC_ERROR("Start : pAdecBuf is NULL(adecMode = 0x%X)!!!\n", stStartInfo.adecMode);
		return RET_ERROR;
	}

	/* Set parameter for clip play */
	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN || stStartInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		//Set a SPDIF output type
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Set a SPDIF output type for GCD platform
			eRetVal = ADEC_SetSPDIFOutputTypeForDual(stStartInfo.adecMode);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputTypeForDual : Error! adecMode = 0x%x\n", stStartInfo.adecMode);
			}
		}
		else
		{
			/* SPDIF Output Decoding Mode 설정 : TP, EMP및 HDMI 출력 관련 강제 세팅  */
			eRetVal = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputType : Error!!! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
			}
		}

		ADEC_ERROR("Clip Info(%d) : codec = %d, rate = %d, freq = %d, ch = %d, bit = %d, rpt = %d\n",	\
					decNum, stStartInfo.adecType, stStartInfo.bitRate/1000, 	\
					stStartInfo.samplingFreq, stStartInfo.numberOfChannel,		\
					stStartInfo.pcmBitsPerSample, stStartInfo.repeatNumber);

		//Check a current clip audio status
		if(g_adec_kdrv.clipStatus[decNum] == LX_ADEC_CLIP_MODE_PLAY)
		{
			ADEC_DEBUG("ADEC_L9_StartAudioClip : Same(dec = %d, status = %d)\n", decNum, g_adec_kdrv.clipStatus[decNum]);

			return RET_OK;
		}

		//Set codec source by adec type
		eRetVal = ADEC_L9_LoadDSP0Codec(stStartInfo.adecType);

		if (eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_StartAudioClip : Error!\n");
			return RET_ERROR;
		}

		ADEC_IPC_GetConfig(adecModule, &decParam);

		decParam.adecInputPort 	  = inputMode;
		decParam.adecCodecType 	  = ADEC_ConvertCodecType(stStartInfo.adecType);
		decParam.bitRate 		  = stStartInfo.bitRate;		//(UINT16)(stStartInfo.bitRate/1000)); 	//kbps unit
		decParam.samplingFreq 	  = stStartInfo.samplingFreq;
		decParam.numberOfChannel  = stStartInfo.numberOfChannel;
		decParam.pcmBitsPerSample = stStartInfo.pcmBitsPerSample;
		decParam.ui32Param[0]  	  = NUM_OF_PARAM_NONE_INFO;

		//Set codec parameters by adec type
		//Check a DRC, Downmix mode for AC-3(EAC-3) or AAC(HEAAC) decoding
		if(stStartInfo.adecType == LX_ADEC_SRC_TYPE_AC3 || stStartInfo.adecType == LX_ADEC_SRC_TYPE_EAC3)
		{
			ac3Info.numOfParam 	= NUM_OF_PARAM_AC3_INFO;
			//ac3Info.drcMode 	= KDRV_ADEC_DRC_MODE_LINE;
			//Change a LINE/RF mode method : Amazon Level Issue.(2013.06.21)
			ac3Info.drcMode 	= g_adec_kdrv.drcMode[decNum];
			ac3Info.downmixMode = KDRV_ADEC_DOWNMIX_MODE_LTRT;
			memcpy(&(decParam.ui32Param[0]), &ac3Info, sizeof(KDRV_ADEC_AC3_INFO_T));
			ADEC_DEBUG("DDC : DRC = %d, Downmix = %d Selected.\n", ac3Info.drcMode, ac3Info.downmixMode);

			ADEC_IPC_SetConfig(adecModule, &decParam);
		}
		else if(stStartInfo.adecType == LX_ADEC_SRC_TYPE_AAC || stStartInfo.adecType == LX_ADEC_SRC_TYPE_HEAAC)
		{
			aacInfo.numOfParam 	 = NUM_OF_PARAM_AAC_INFO;
			aacInfo.aacFormat	 = KDRV_ADEC_AAC_FORMAT_TYPE_LOAS;
			aacInfo.drcMode 	 = KDRV_ADEC_DRC_MODE_LINE;
			aacInfo.downmixMode  = KDRV_ADEC_DOWNMIX_MODE_LORO;
			aacInfo.sizeOfEncOpt = stStartInfo.lengthOfDSI;

			ADEC_DEBUG("lengthOfDSI : %d\n", stStartInfo.lengthOfDSI);

			if( (buffer != NULL) && (stStartInfo.lengthOfDSI > 0) )
			{
				if(stStartInfo.lengthOfDSI < 4)
				{
					length = stStartInfo.lengthOfDSI;

					for(i = 0; i < length; i++)
					{
						encoderOption0 |= *(buffer + i) << i * 8;
						ADEC_DEBUG("buffer : %x\n", *(buffer + i));
					}
				}
				else if(stStartInfo.lengthOfDSI >= 4 && stStartInfo.lengthOfDSI < 8)
				{
					length = stStartInfo.lengthOfDSI % 4;

					for(i = 0; i < 4; i++)
					{
						encoderOption0 |= *(buffer + i) << i * 8;
						ADEC_DEBUG("buffer : %x\n", *(buffer + i));
					}

					for(i = 0; i < length; i++)
					{
						encoderOption1 |= *(buffer + 4 + i) << i * 8;
						ADEC_DEBUG("buffer : %x\n", *(buffer + 4 + i));
					}
				}
				else if(stStartInfo.lengthOfDSI >= 8)
				{
					for(i = 0; i < 4; i++)
					{
						encoderOption0 |= *(buffer + i) << i * 8;
						ADEC_DEBUG("buffer : %x\n", *(buffer + i));
					}

					for(i = 0; i < 4; i++)
					{
						encoderOption1 |= *(buffer + 4 + i) << i * 8;
						ADEC_DEBUG("buffer : %x\n", *(buffer + 4 + i));
					}
				}
			}
			else
			{
				aacInfo.sizeOfEncOpt = 0;
			}

			aacInfo.encoderOption0 = encoderOption0;
			aacInfo.encoderOption1 = encoderOption1;
			memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));

			ADEC_DEBUG("DDT : DRC = %d, Downmix = %d Selected.\n", aacInfo.drcMode, aacInfo.downmixMode);
			ADEC_DEBUG("DDT : Option0 = 0x%X, Option1 = 0x%X\n", aacInfo.encoderOption0, aacInfo.encoderOption1);

			ADEC_IPC_SetConfig(adecModule, &decParam);
		}
		else if(stStartInfo.adecType == LX_ADEC_SRC_TYPE_WMA_PRO)
		{
			//Set WMA /WMA Pro parameters.
			eRetVal = ADEC_SetWMAProParams(stStartInfo);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_SetWMAProParams : Error!\n");

				return RET_ERROR;
			}
		}
		else if(stStartInfo.adecType == LX_ADEC_SRC_TYPE_ADPCM)
		{
			adpcmInfo.numOfParam = NUM_OF_PARAM_ADPCM_INFO;
			adpcmInfo.adpcmType  = stStartInfo.adpcmType;
			adpcmInfo.blockAlign = stStartInfo.blockAlign;
			memcpy(&(decParam.ui32Param[0]), &adpcmInfo, sizeof(KDRV_ADEC_ADPCM_INFO_T));

			ADEC_IPC_SetConfig(adecModule, &decParam);

			ADEC_ERROR("ADPCM Info : adpcm_type = %d, block_align = %d\n",	\
						stStartInfo.adpcmType, stStartInfo.blockAlign);
		}
		else if(stStartInfo.adecType == LX_ADEC_SRC_TYPE_RA10)		// RA8
		{
			//Set RA8 parameters.
			eRetVal = ADEC_SetRA8Params(stStartInfo);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_SetRA8Params : Error!\n");

				return RET_ERROR;
			}
		}
		else
		{
			ADEC_IPC_SetConfig(adecModule, &decParam);
		}

		//Set a repeat function
		if(stStartInfo.repeatNumber > 1)
		{
			(void)pAdecBuf->set_repeat(stStartInfo.repeatNumber - 1, pAdecBuf);
		}

		//Get lip sync parameter for IPC command
		ADEC_IPC_GetLipsync(adecModule, &decLipsyncParam);

		//Set a lip sync bound
		if(stStartInfo.adecType == LX_ADEC_SRC_TYPE_WMA_PRO)
		{
			decLipsyncParam.ubound	  = 150;		//0x96(150)
			decLipsyncParam.lbound    = 150;		//0x96(150)
		}
		else
		{
			decLipsyncParam.ubound	  = 100;		//0x64(100)
			decLipsyncParam.lbound	  = 100;		//0x64(100)
		}
		// Daeseok.youn : 2013.3.21 Change libpsync offset from 900ms to 200ms
		decLipsyncParam.offset		  = GTV_MEDIA_LIBSYNC_OFFSET;		//0x384(900), this value must be matched.(NetCast : gstvdecsync, GCD : vdec LGMD)
		decLipsyncParam.freerunubound = 10000;		// 10 sec
		decLipsyncParam.freerunlbound = 120000;		// 120 sec, 0x1D4C0(120000)

		//Set lip sync parameters
		ADEC_IPC_SetLipsync(adecModule, &decLipsyncParam);

		BASE_AVLIPSYNC_LOCK();

		//Get a GTSC value
		AUD_RdFL(aud_gstcclreg);
		clockBaseTime = AUD_Rd(aud_gstcclreg) & 0x0FFFFFFF;

		//This case may be changed if play is called before feed function
		ui64TS90kHzTick = g_adec_kdrv.index[decNum].AUI[0].streamtime;

		if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
		{
			ui64TS90kHzTick *= 9;

			if(ui64TS90kHzTick)
			{
				do_div(ui64TS90kHzTick, 100000);		// = Xns * 90 000 / 1 000 000 000
			}
		}
		streamBaseTime = (ui64TS90kHzTick) & 0x0FFFFFFF;

		//Check a av lip sync register number
		if(decNum == LX_ADEC_DEC_0 && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(decNum == LX_ADEC_DEC_1 && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] != LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(decNum == LX_ADEC_DEC_1)
		{
			avLipSyncNum = LX_ADEC_DEC_1;
		}
		else
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}

		//Read a vdec cbt and sbt time
		if(avLipSyncNum == LX_ADEC_DEC_0)
		{
			vdecDecCbt = g_pVDEC_LipsyncReg->vdec_dec0_cbt;
			vdecDecSbt = g_pVDEC_LipsyncReg->vdec_dec0_sbt;
		}
		else
		{
			vdecDecCbt = g_pVDEC_LipsyncReg->vdec_dec1_cbt;
			vdecDecSbt = g_pVDEC_LipsyncReg->vdec_dec1_sbt;
		}

		if( ((stStartInfo.adpcmType == LX_ADPCM_CODEC_TYPE_G722) && (vdecDecCbt == g_adec_kdrv.timePreCBT[avLipSyncNum]))	\
		  ||(vdecDecCbt == 0xFFFFFFFF)	\
		  ||(vdecDecSbt == 0xFFFFFFFF) )
		{
			if(avLipSyncNum == LX_ADEC_DEC_0)
			{
				// Set ADEC DEC0 Clock Basetime
				AUD_Wr(aud_dec0_cbt, (UINT32)(clockBaseTime & 0x0FFFFFFF));
				AUD_WrFL(aud_dec0_cbt);
			}
			else
			{
				// Set ADEC DEC1 Clock Basetime
				AUD_Wr(aud_dec1_cbt, (UINT32)(clockBaseTime & 0x0FFFFFFF));
				AUD_WrFL(aud_dec1_cbt);
			}

			// Set DEC Stream Basetime
			if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
			{
				adecDecSbt = (UINT32)(streamBaseTime & 0x0FFFFFFF);
			}
			else
			{
				adecDecSbt = (UINT32)0xFFFFFFFF;
			}

			if(avLipSyncNum == LX_ADEC_DEC_0)
			{
				// Set DEC0 Stream Basetime
				AUD_Wr(aud_dec0_sbt, adecDecSbt);
				AUD_WrFL(aud_dec0_sbt);
			}
			else
			{
				// Set DEC1 Stream Basetime
				AUD_Wr(aud_dec1_sbt, adecDecSbt);
				AUD_WrFL(aud_dec1_sbt);
			}
		}
		else
		{
			if(avLipSyncNum == LX_ADEC_DEC_0)
			{
				// Set DEC0 Clock Basetime
				AUD_Wr(aud_dec0_cbt, g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec0_cbt);

				// Set DEC0 Stream Basetime
				AUD_Wr(aud_dec0_sbt, g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec0_sbt);

				clockBaseTime  = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF;
				streamBaseTime = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF;
			}
			else
			{
				// Set DEC1 Clock Basetime
				AUD_Wr(aud_dec1_cbt, g_pVDEC_LipsyncReg->vdec_dec1_cbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec1_cbt);

				// Set DEC1 Stream Basetime
				AUD_Wr(aud_dec1_sbt, g_pVDEC_LipsyncReg->vdec_dec1_sbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec1_sbt);

				clockBaseTime  = (UINT64)g_pVDEC_LipsyncReg->vdec_dec1_cbt & 0x0FFFFFFF;
				streamBaseTime = (UINT64)g_pVDEC_LipsyncReg->vdec_dec1_sbt & 0x0FFFFFFF;
			}
		}

		BASE_AVLIPSYNC_UNLOCK();

		if(avLipSyncNum == LX_ADEC_DEC_0)
		{
			ADEC_ERROR("vdec_dec0_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_cbt);

			ADEC_DEBUG("vdec_dec0_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_cbt);
			ADEC_DEBUG("vdec_dec0_sbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_sbt);
			ADEC_DEBUG("clockBaseTime  : 0x%08X!\n", AUD_Rd(aud_dec0_cbt));
			ADEC_DEBUG("streamBaseTime : 0x%08X!\n", AUD_Rd(aud_dec0_sbt));
		}
		else
		{
			ADEC_ERROR("vdec_dec1_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_cbt);

			ADEC_DEBUG("vdec_dec1_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_cbt);
			ADEC_DEBUG("vdec_dec1_sbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_sbt);
			ADEC_DEBUG("clockBaseTime  : 0x%08X!\n", AUD_Rd(aud_dec1_cbt));
			ADEC_DEBUG("streamBaseTime : 0x%08X!\n", AUD_Rd(aud_dec1_sbt));
		}

		//Send a base time for IPC
		syncBase.module 	= adecModule;
		syncBase.clockBase 	= clockBaseTime;
		syncBase.streamBase = streamBaseTime;

		//In GCD model, use ADEC_L9_SetAVClockControl function to sync audio and video
		if(g_eAdecInitType != LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			ADEC_IPC_SetBaseTime(syncBase);
		}

		//Send a IPC command
		ADEC_IPC_Start(adecModule);

		/* Update Internal Status Value */
		g_adec_kdrv.bDecStarted[decNum] = TRUE;
		g_adec_kdrv.clipStatus[decNum]  = LX_ADEC_CLIP_MODE_PLAY;

#ifdef ENABLE_AAC_PARSER
		/* Probe AAC codec ES data for AAC profile and format detection */
		if( (stStartInfo.adecType == LX_ADEC_SRC_TYPE_AAC)	\
		  ||(stStartInfo.adecType == LX_ADEC_SRC_TYPE_HEAAC) )
		{
			ADEC_PRINT("ADEC_AAC_Probe_SetDsi : DSI size = %d\n", stStartInfo.lengthOfDSI);

			/* Set a AAC DSI data */
			eRetVal = ADEC_AAC_Probe_SetDsi(stStartInfo.pDSIBuffer, stStartInfo.lengthOfDSI, g_adec_kdrv.aacProbe[decNum]);

			if(eRetVal < RET_OK)
			{
				ADEC_DEBUG("PLAY : ADEC_AAC_Probe_SetDsi(size = %d)!!!(eRetVal = %d)\n", stStartInfo.lengthOfDSI, eRetVal);
			}

			/* Save AAC DSI parsing status */
			g_adec_kdrv.bAacDSIParsed[decNum] = TRUE;
		}
#endif	//#ifdef ENABLE_AAC_PARSER
	}
	else if( (stStartInfo.adecMode >= LX_ADEC_MIX_BUF0)
		   &&(stStartInfo.adecMode <= LX_ADEC_MIX_BUF3 || stStartInfo.adecMode == LX_ADEC_LLB_BUF) )
	{
		ADEC_DEBUG("Mix[%d] Info  freq = %d, ch_num = %d, bit_num = %d\n",	\
					mixNum, stStartInfo.samplingFreq, stStartInfo.numberOfChannel, stStartInfo.pcmBitsPerSample);

		//Check a current clip audio status
		if(g_adec_kdrv.mixerStatus[mixNum] == LX_ADEC_CLIP_MODE_PLAY)
		{
			ADEC_DEBUG("ADEC_L9_StartAudioClip : Same(mix = %d, status = %d)\n", mixNum, g_adec_kdrv.mixerStatus[mixNum]);

			return RET_OK;
		}

		//Set a repeat function
		if(stStartInfo.repeatNumber > 1)
		{
			(void)pAdecBuf->set_repeat(stStartInfo.repeatNumber - 1, pAdecBuf);
		}

		ADEC_IPC_GetConfig(adecModule, &mixStartParam);

		mixStartParam.ui32NumOfCH 	   = stStartInfo.numberOfChannel;
		mixStartParam.ui32BitPerSample = stStartInfo.pcmBitsPerSample;
		mixStartParam.ui32SampleRate   = stStartInfo.samplingFreq;

		ADEC_IPC_SetConfig(adecModule, &mixStartParam);

		//In GCD model, use ADEC_L9_SetAVClockControl function to sync audio and video with mixer input
		if(g_eAdecInitType == LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Get lip sync parameter for IPC command
			ADEC_IPC_GetLipsync(adecModule, &mixLipsyncParam);

			mixLipsyncParam.ubound		  = 100;		//0x64(100)
			mixLipsyncParam.lbound		  = 100;		//0x64(100)
			// Daeseok.youn 2013.3.21 change libsync offset from 900 to 200
			mixLipsyncParam.offset		  = GTV_MEDIA_LIBSYNC_OFFSET;		//0x384(900), this value must be matched.(NetCast : gstvdecsync, GCD : vdec LGMD)
			mixLipsyncParam.freerunubound = 5000;		// 5 sec, 0x1388(5000)
			mixLipsyncParam.freerunlbound = 120000;		// 120 sec, 0x1D4C0(120000)

			//Set lip sync parameters
			ADEC_IPC_SetLipsync(adecModule, &mixLipsyncParam);
		}
		else
		{
			//Send a IPC command
			ADEC_IPC_Start(adecModule);

			/* Update Internal Status Value */
			g_adec_kdrv.bMixStarted[mixNum] = TRUE;
			g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_PLAY;
		}
	}
	else
	{
		ADEC_ERROR("PLAY : mode is not defined(adecMode = 0x%X)!!!\n", stStartInfo.adecMode);
		return RET_ERROR;
	}

	//Set a Mixer stop for GCD platform if decoder is started not buffer end status
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN || stStartInfo.adecMode == LX_ADEC_DECODE_ASSO)
		{
			if(g_adec_kdrv.clipBufStatus[decNum] == LX_ADEC_CLIP_BUF_END)
			{
				//Send a IPC command
				ADEC_IPC_Start(adecMixerModule);
			}
		}
	}

#ifdef ENABLE_DDCO_TEST
	//Send a IPC command
	ADEC_IPC_Start(adecMixerModule);
#endif

	//Sound Engine Control
#ifdef ENABLE_DAMP_VOLUME_CONTROL
	//Disable a sound engine
	ADEC_IPC_SetDisableSeEngine();
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("Audio clip play started(Mode = 0x%X, repeat = %d)\n", stStartInfo.adecMode, stStartInfo.repeatNumber);
#else
	ADEC_ERROR("[%s]Audio clip play started(Mode = 0x%X, repeat = %d) numberOfChannel[%d]pcmBitsPerSample[%d]samplingFreq[%d]\n", sourcetype[stStartInfo.adecMode],stStartInfo.adecMode, stStartInfo.repeatNumber,stStartInfo.numberOfChannel,stStartInfo.pcmBitsPerSample,stStartInfo.samplingFreq);
#endif

	return RET_OK;
}


/**
 * Stops a playing of the audio clip.
 * This function stops a playing of the audio clip.
 * @see ADEC_DRV_StartAudioClip
*/
int ADEC_L9_StopAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	UINT8	decNum = LX_ADEC_DEC_0;
	UINT8	mixNum = LX_ADEC_MIXER_0;

	UINT8	avLipSyncNum = 0;

	KDRV_ADEC_MOD_T 	adecModule 		= KDRV_ADEC_DEC_0;
	KDRV_ADEC_MOD_T 	adecMixerModule = KDRV_ADEC_MP_00;

	//Stop decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Check a adec mode
		if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
		{
			decNum	   		= LX_ADEC_DEC_0;
			adecModule 		= KDRV_ADEC_DEC_0;
			adecMixerModule = KDRV_ADEC_MP_00;

			//delete CPBM structure
			if(g_pCPBM_WriterStruct != NULL)
			{
				OS_KFree(g_pCPBM_WriterStruct);
				g_pCPBM_WriterStruct = NULL;
			}
		}
		else
		{
			decNum	   		= LX_ADEC_DEC_1;
			adecModule 		= KDRV_ADEC_DEC_1;
			adecMixerModule = KDRV_ADEC_MP_01;

			//delete CPBA structure
			if(g_pCPBA_WriterStruct != NULL)
			{
				OS_KFree(g_pCPBA_WriterStruct);
				g_pCPBA_WriterStruct = NULL;
			}
		}

		//Check a current clip audio status
		if(g_adec_kdrv.clipStatus[decNum] == LX_ADEC_CLIP_MODE_STOP)
		{
			ADEC_DEBUG("ADEC_L9_StopAudioClip : Same(dec = %d, status = %d)\n", decNum, g_adec_kdrv.clipStatus[decNum]);

			return RET_OK;
		}

		//Check a av lip sync register
		if(decNum == LX_ADEC_DEC_0 && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(decNum == LX_ADEC_DEC_1 && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] != LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(decNum == LX_ADEC_DEC_1)
		{
			avLipSyncNum = LX_ADEC_DEC_1;
		}
		else
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}

		BASE_AVLIPSYNC_LOCK();

		//Clear & save a av lip sync register number
		if(avLipSyncNum == LX_ADEC_DEC_0)
		{
			// Set DEC0 Clock Basetime
			AUD_RdFL(aud_dec0_cbt);
			g_adec_kdrv.timePreCBT[LX_ADEC_DEC_0] = AUD_Rd(aud_dec0_cbt);
			AUD_Wr(aud_dec0_cbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec0_cbt);

			// Set DEC0 Stream Basetime
			AUD_RdFL(aud_dec0_sbt);
			g_adec_kdrv.timePreSBT[LX_ADEC_DEC_0] = AUD_Rd(aud_dec0_sbt);
			AUD_Wr(aud_dec0_sbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec0_sbt);
		}
		else
		{
			// Set DEC1 Clock Basetime
			AUD_RdFL(aud_dec1_cbt);
			g_adec_kdrv.timePreCBT[LX_ADEC_DEC_1] = AUD_Rd(aud_dec1_cbt);
			AUD_Wr(aud_dec1_cbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec1_cbt);

			// Set DEC1 Stream Basetime
			AUD_RdFL(aud_dec1_sbt);
			g_adec_kdrv.timePreSBT[LX_ADEC_DEC_1] = AUD_Rd(aud_dec1_sbt);
			AUD_Wr(aud_dec1_sbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec1_sbt);
		}

		BASE_AVLIPSYNC_UNLOCK();

		/* Update Internal Status Value */
		g_adec_kdrv.bDecStarted[decNum]   = FALSE;
		g_adec_kdrv.clipStatus[decNum]	  = LX_ADEC_CLIP_MODE_STOP;
		g_adec_kdrv.clipBufStatus[decNum] = LX_ADEC_CLIP_BUF_NONE;

		//Set a Mixer stop for GCD platform
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Send a IPC command
			ADEC_IPC_Stop(adecMixerModule);
		}

#ifdef ENABLE_AAC_PARSER
		/* Terminate AAC codec parser */
		if( (g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_AAC)	\
		  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_HEAAC) )
		{
			/* Save AAC parsing status */
			g_adec_kdrv.bAacParsed[decNum] 	  = FALSE;
			g_adec_kdrv.bAacDSIParsed[decNum] = FALSE;

			if(g_adec_kdrv.aacProbe[decNum] != NULL)
			{
				/* Terminate AAC probe parser */
				(void)ADEC_AAC_Probe_Terminate(g_adec_kdrv.aacProbe[decNum]);

				/* Set NULL pointer */
				g_adec_kdrv.aacProbe[decNum] = NULL;
			}
		}
#endif	//#ifdef ENABLE_AAC_PARSER
	}
	else if( (i_eAdecMode >= LX_ADEC_MIX_BUF0)
		   &&(i_eAdecMode <= LX_ADEC_MIX_BUF3 || i_eAdecMode == LX_ADEC_LLB_BUF) )
	{
		if(i_eAdecMode == LX_ADEC_MIX_BUF0)
		{
			mixNum	   = LX_ADEC_MIXER_0;
			adecModule = KDRV_ADEC_MP_02;

			//delete MIX0 structure
			if(g_pMIX0_WriterStruct != NULL)
			{
				OS_KFree(g_pMIX0_WriterStruct);
				g_pMIX0_WriterStruct = NULL;
			}
		}
		else if(i_eAdecMode == LX_ADEC_MIX_BUF1)
		{
			mixNum	   = LX_ADEC_MIXER_1;
			adecModule = KDRV_ADEC_MP_03;

			//delete MIX1 structure
			if(g_pMIX1_WriterStruct != NULL)
			{
				OS_KFree(g_pMIX1_WriterStruct);
				g_pMIX1_WriterStruct = NULL;
			}
		}
		else if(i_eAdecMode == LX_ADEC_MIX_BUF2)
		{
			mixNum	   = LX_ADEC_MIXER_2;
			adecModule = KDRV_ADEC_MP_04;

			//delete MIX2 structure
			if(g_pMIX2_WriterStruct != NULL)
			{
				OS_KFree(g_pMIX2_WriterStruct);
				g_pMIX2_WriterStruct = NULL;
			}
		}
		else if(i_eAdecMode == LX_ADEC_MIX_BUF3)
		{
			mixNum	   = LX_ADEC_MIXER_3;
			adecModule = KDRV_ADEC_MP_05;

			//delete MIX3 structure
			if(g_pMIX2_WriterStruct != NULL)
			{
				OS_KFree(g_pMIX3_WriterStruct);
				g_pMIX3_WriterStruct = NULL;
			}
		}
		else if(i_eAdecMode == LX_ADEC_LLB_BUF)
		{
			mixNum	   = LX_ADEC_MIXER_4;
			adecModule = KDRV_ADEC_MP_06;

			//delete LLB structure
			if(g_pLLB_WriterStruct != NULL)
			{
				OS_KFree(g_pLLB_WriterStruct);
				g_pLLB_WriterStruct = NULL;
			}
		}
		else
		{
			ADEC_ERROR("ADEC_L9_StopAudioClip : adecMode = 0x%X!!!\n", i_eAdecMode);
			return RET_ERROR;
		}

		//Check a current clip audio status
		if(g_adec_kdrv.mixerStatus[mixNum] == LX_ADEC_CLIP_MODE_STOP)
		{
			ADEC_DEBUG("ADEC_L9_StopAudioClip : Same(mix = %d, status = %d)\n", mixNum, g_adec_kdrv.mixerStatus[mixNum]);

			return RET_OK;
		}

		/* Update Internal Status Value */
		g_adec_kdrv.bMixStarted[mixNum] = FALSE;
		g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_STOP;
	}
	else
	{
		ADEC_ERROR("STOP : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	//Send a IPC command
	ADEC_IPC_Stop(adecModule);

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("ADEC_DRV_StopAudioClip : Stopped(adecMode = 0x%X)...\n", i_eAdecMode);
#else
	ADEC_ERROR("[%s]ADEC_DRV_StopAudioClip : Stopped(adecMode = 0x%X)...\n", sourcetype[i_eAdecMode], i_eAdecMode);
#endif

	return RET_OK;
}


/**
 * Pauses a playing of the audio clip.
 * After calling this IOCTL function, resume must be enabled by the ADEC_IO_RESUME_AUDIO_CLIP IOCTL function.
 * @see ADEC_DRV_StopAudioClip
*/
int ADEC_L9_PauseAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	UINT8	decNum = LX_ADEC_DEC_0;
	UINT8	mixNum = LX_ADEC_MIXER_0;

	UINT8	avLipSyncNum = 0;

	KDRV_ADEC_MOD_T 	adecModule 		= KDRV_ADEC_DEC_0;
	KDRV_ADEC_MOD_T 	adecMixerModule = KDRV_ADEC_MP_00;

	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Check a adec mode
		if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
		{
			decNum 			= LX_ADEC_DEC_0;
			adecMixerModule = KDRV_ADEC_MP_00;
		}
		else
		{
			decNum 			= LX_ADEC_DEC_1;
			adecMixerModule = KDRV_ADEC_MP_01;
		}

		//Check a current clip audio status
		if(g_adec_kdrv.clipStatus[decNum] == LX_ADEC_CLIP_MODE_PAUSE)
		{
#ifdef ENABLE_NETCAST_PRINT
			ADEC_DEBUG("ADEC_L9_PauseAudioClip : Same(dec = %d, status = %d)\n", decNum, g_adec_kdrv.clipStatus[decNum]);
#else
			ADEC_ERROR("[%s]ADEC_L9_PauseAudioClip : Same(dec = %d, status = %d)\n", sourcetype[i_eAdecMode], decNum, g_adec_kdrv.clipStatus[decNum]);
#endif

			return RET_OK;
		}

		//Check a av lip sync register number
		if(i_eAdecMode == LX_ADEC_DECODE_MAIN && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(i_eAdecMode == LX_ADEC_DECODE_ASSO && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] != LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
		{
			avLipSyncNum = LX_ADEC_DEC_1;
		}
		else
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}

		//Save a av lip sync register
		if(avLipSyncNum == LX_ADEC_DEC_0)
		{
			// Set DEC0 Clock Basetime
			AUD_RdFL(aud_dec0_cbt);
			g_adec_kdrv.timePreCBT[LX_ADEC_DEC_0] = AUD_Rd(aud_dec0_cbt);
			AUD_Wr(aud_dec0_cbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec0_cbt);

			// Set DEC0 Stream Basetime
			AUD_RdFL(aud_dec0_sbt);
			g_adec_kdrv.timePreSBT[LX_ADEC_DEC_0] = AUD_Rd(aud_dec0_sbt);
			AUD_Wr(aud_dec0_sbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec0_sbt);

			AUD_RdFL(aud_gstcclreg);
			g_adec_kdrv.timePaused[LX_ADEC_DEC_0] = AUD_Rd(aud_gstcclreg) & 0x0FFFFFFF;
		}
		else
		{
			// Set DEC1 Clock Basetime
			AUD_RdFL(aud_dec1_cbt);
			g_adec_kdrv.timePreCBT[LX_ADEC_DEC_1] = AUD_Rd(aud_dec1_cbt);
			AUD_Wr(aud_dec1_cbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec1_cbt);

			// Set DEC1 Stream Basetime
			AUD_RdFL(aud_dec1_sbt);
			g_adec_kdrv.timePreSBT[LX_ADEC_DEC_1] = AUD_Rd(aud_dec1_sbt);
			AUD_Wr(aud_dec1_sbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec1_sbt);

			AUD_RdFL(aud_gstcclreg);
			g_adec_kdrv.timePaused[LX_ADEC_DEC_1] = AUD_Rd(aud_gstcclreg) & 0x0FFFFFFF;
		}
	}

	//Pause decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
	{
		adecModule = KDRV_ADEC_DEC_0;
	}
	else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		adecModule = KDRV_ADEC_DEC_1;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF0)
	{
		mixNum	   = LX_ADEC_MIXER_0;
		adecModule = KDRV_ADEC_MP_02;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF1)
	{
		mixNum	   = LX_ADEC_MIXER_1;
		adecModule = KDRV_ADEC_MP_03;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF2)
	{
		mixNum	   = LX_ADEC_MIXER_2;
		adecModule = KDRV_ADEC_MP_04;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF3)
	{
		mixNum	   = LX_ADEC_MIXER_3;
		adecModule = KDRV_ADEC_MP_05;
	}
	else if(i_eAdecMode == LX_ADEC_LLB_BUF)
	{
		mixNum	   = LX_ADEC_MIXER_4;
		adecModule = KDRV_ADEC_MP_06;
	}
	else
	{
		ADEC_ERROR("PAUSE : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	if( (i_eAdecMode >= LX_ADEC_MIX_BUF0)
	  &&(i_eAdecMode <= LX_ADEC_MIX_BUF3 || i_eAdecMode == LX_ADEC_LLB_BUF) )
	{
		//Check a current clip audio status
		if(g_adec_kdrv.mixerStatus[mixNum] == LX_ADEC_CLIP_MODE_PAUSE)
		{
			ADEC_DEBUG("ADEC_L9_PauseAudioClip : Same(mix = %d, status = %d)\n", mixNum, g_adec_kdrv.mixerStatus[mixNum]);

			return RET_OK;
		}
	}

	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		/* Update Internal Status Value */
		g_adec_kdrv.clipStatus[decNum] = LX_ADEC_CLIP_MODE_PAUSE;

		//Set a Mixer pause for GCD platform
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Send a IPC command
			ADEC_IPC_Pause(adecMixerModule);
		}
	}
	else if( (i_eAdecMode >= LX_ADEC_MIX_BUF0)
		   &&(i_eAdecMode <= LX_ADEC_MIX_BUF3 || i_eAdecMode == LX_ADEC_LLB_BUF) )
	{
		/* Update Internal Status Value */
		g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_PAUSE;
	}

	//Send a IPC command
	ADEC_IPC_Pause(adecModule);

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("Audio clip play paused(adecMode = 0x%X)...\n", i_eAdecMode);
#else
	ADEC_ERROR("[%s]Audio clip play paused(adecMode = 0x%X)...\n", sourcetype[i_eAdecMode], i_eAdecMode);
#endif

	return RET_OK;
}


/**
 * Resumes a paused audio clip playing.
 * After calling this IOCTL function, a audio clip is played at the paused position.
 * @see ADEC_DRV_PauseAudioClip
*/
int ADEC_L9_ResumeAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	UINT8	decNum = LX_ADEC_DEC_0;
	UINT8	mixNum = LX_ADEC_MIXER_0;

	UINT8	avLipSyncNum = 0;

	UINT64 clockBaseTime = 0;
	UINT64 streamBaseTime = 0;

	UINT32 timeResumeAudioClip = 0;
	UINT32 timeDurationPause = 0;

	ADEC_CMD_SYNC_BASE_T syncBase;

	KDRV_ADEC_MOD_T 	adecModule 		= KDRV_ADEC_DEC_0;
	KDRV_ADEC_MOD_T 	adecMixerModule = KDRV_ADEC_MP_00;

	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Check a adec mode
		if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
		{
			decNum 			= LX_ADEC_DEC_0;
			adecMixerModule = KDRV_ADEC_MP_00;
		}
		else
		{
			decNum 			= LX_ADEC_DEC_1;
			adecMixerModule = KDRV_ADEC_MP_01;
		}

		//Check a current clip audio status
		if(g_adec_kdrv.clipStatus[decNum] == LX_ADEC_CLIP_MODE_RESUME)
		{
			ADEC_DEBUG("ADEC_L9_ResumeAudioClip : Same(dec = %d, status = %d)\n", decNum, g_adec_kdrv.clipStatus[decNum]);

			return RET_OK;
		}

		//Check a av lip sync register number
		if(i_eAdecMode == LX_ADEC_DECODE_MAIN && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(i_eAdecMode == LX_ADEC_DECODE_ASSO && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] != LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
		{
			avLipSyncNum = LX_ADEC_DEC_1;
		}
		else
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}

		//Get a av lip sync register
		if(avLipSyncNum == LX_ADEC_DEC_0)
		{
			BASE_AVLIPSYNC_LOCK();

			if( (g_pVDEC_LipsyncReg->vdec_dec0_cbt == g_adec_kdrv.timePreCBT[LX_ADEC_DEC_0])	\
			  ||(g_pVDEC_LipsyncReg->vdec_dec0_cbt == 0xFFFFFFFF)	\
			  ||(g_pVDEC_LipsyncReg->vdec_dec0_sbt == 0xFFFFFFFF) )
			{
				clockBaseTime  = (UINT64)g_adec_kdrv.timePreCBT[LX_ADEC_DEC_0];
				streamBaseTime = (UINT64)g_adec_kdrv.timePreSBT[LX_ADEC_DEC_0];

				AUD_RdFL(aud_gstcclreg);
				timeResumeAudioClip = AUD_Rd(aud_gstcclreg) & 0x0FFFFFFF;

				if(g_adec_kdrv.timePaused[LX_ADEC_DEC_0] < timeResumeAudioClip)
					timeDurationPause = timeResumeAudioClip - g_adec_kdrv.timePaused[LX_ADEC_DEC_0] ;
				else
					timeDurationPause = (0x10000000 + timeResumeAudioClip - g_adec_kdrv.timePaused[LX_ADEC_DEC_0]) & 0x0FFFFFFF;

				clockBaseTime = (clockBaseTime + timeDurationPause) & 0x0FFFFFFF ;

				// Set DEC0 Clock Basetime
				AUD_Wr(aud_dec0_cbt, (UINT32)(clockBaseTime & 0x0FFFFFFF));
				AUD_WrFL(aud_dec0_cbt);

				// Set DEC0 Clock Basetime
				AUD_Wr(aud_dec0_sbt, (UINT32)(streamBaseTime & 0x0FFFFFFF));
				AUD_WrFL(aud_dec0_sbt);
			}
			else
			{
				// Set DEC0 Clock Basetime
				AUD_Wr(aud_dec0_cbt, g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec0_cbt);

				// Set DEC0 Clock Basetime
				AUD_Wr(aud_dec0_sbt, (UINT32)(g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF));
				AUD_WrFL(aud_dec0_sbt);

				clockBaseTime  = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF;
				streamBaseTime = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF;
			}

			BASE_AVLIPSYNC_UNLOCK();

			ADEC_ERROR("vdec_dec0_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_cbt);

			ADEC_DEBUG("vdec_dec0_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_cbt);
			ADEC_DEBUG("vdec_dec0_sbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_sbt);
			ADEC_DEBUG("clockBaseTime  : 0x%08X!\n", AUD_Rd(aud_dec0_cbt));
			ADEC_DEBUG("streamBaseTime : 0x%08X!\n", AUD_Rd(aud_dec0_sbt));

			syncBase.module 	= KDRV_ADEC_DEC_0;
			syncBase.clockBase 	= clockBaseTime;
			syncBase.streamBase = streamBaseTime;

			//In GCD model, use ADEC_L9_SetAVClockControl function to sync audio and video
			if(g_eAdecInitType != LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
			{
				ADEC_IPC_SetBaseTime(syncBase);
			}
		}
		else
		{
			BASE_AVLIPSYNC_LOCK();

			if( (g_pVDEC_LipsyncReg->vdec_dec1_cbt == g_adec_kdrv.timePreCBT[LX_ADEC_DEC_1])	\
			  ||(g_pVDEC_LipsyncReg->vdec_dec1_cbt == 0xFFFFFFFF)	\
			  ||(g_pVDEC_LipsyncReg->vdec_dec1_sbt == 0xFFFFFFFF) )
			{
				clockBaseTime  = (UINT64)g_adec_kdrv.timePreCBT[LX_ADEC_DEC_1];
				streamBaseTime = (UINT64)g_adec_kdrv.timePreSBT[LX_ADEC_DEC_1];

				AUD_RdFL(aud_gstcclreg);
				timeResumeAudioClip = AUD_Rd(aud_gstcclreg) & 0x0FFFFFFF;

				if(g_adec_kdrv.timePaused[LX_ADEC_DEC_1] < timeResumeAudioClip)
					timeDurationPause = timeResumeAudioClip - g_adec_kdrv.timePaused[LX_ADEC_DEC_1] ;
				else
					timeDurationPause = (0x10000000 + timeResumeAudioClip - g_adec_kdrv.timePaused[LX_ADEC_DEC_1]) & 0x0FFFFFFF;

				clockBaseTime = (clockBaseTime + timeDurationPause) & 0x0FFFFFFF ;

				// Set DEC1 Clock Basetime
				AUD_Wr(aud_dec1_cbt, (UINT32)(clockBaseTime & 0x0FFFFFFF));
				AUD_WrFL(aud_dec1_cbt);

				// Set DEC1 Clock Basetime
				AUD_Wr(aud_dec1_sbt, (UINT32)(streamBaseTime & 0x0FFFFFFF));
				AUD_WrFL(aud_dec1_sbt);
			}
			else
			{
				// Set DEC1 Clock Basetime
				AUD_Wr(aud_dec1_cbt, g_pVDEC_LipsyncReg->vdec_dec1_cbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec1_cbt);

				// Set DEC1 Clock Basetime
				AUD_Wr(aud_dec1_sbt, (UINT32)(g_pVDEC_LipsyncReg->vdec_dec1_sbt & 0x0FFFFFFF));
				AUD_WrFL(aud_dec1_sbt);

				clockBaseTime  = (UINT64)g_pVDEC_LipsyncReg->vdec_dec1_cbt & 0x0FFFFFFF;
				streamBaseTime = (UINT64)g_pVDEC_LipsyncReg->vdec_dec1_sbt & 0x0FFFFFFF;
			}

			BASE_AVLIPSYNC_UNLOCK();

			ADEC_ERROR("vdec_dec1_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_cbt);

			ADEC_DEBUG("vdec_dec1_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_cbt);
			ADEC_DEBUG("vdec_dec1_sbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_sbt);
			ADEC_DEBUG("clockBaseTime  : 0x%08X!\n", AUD_Rd(aud_dec1_cbt));
			ADEC_DEBUG("streamBaseTime : 0x%08X!\n", AUD_Rd(aud_dec1_sbt));

			syncBase.module 	= KDRV_ADEC_DEC_1;
			syncBase.clockBase	= clockBaseTime;
			syncBase.streamBase = streamBaseTime;

			//In GCD model, use ADEC_L9_SetAVClockControl function to sync audio and video
			if(g_eAdecInitType != LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
			{
				ADEC_IPC_SetBaseTime(syncBase);
			}
		}
	}

	//Re-start decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
	{
		adecModule = KDRV_ADEC_DEC_0;
	}
	else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		adecModule = KDRV_ADEC_DEC_1;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF0)
	{
		mixNum	   = LX_ADEC_MIXER_0;
		adecModule = KDRV_ADEC_MP_02;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF1)
	{
		mixNum	   = LX_ADEC_MIXER_1;
		adecModule = KDRV_ADEC_MP_03;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF2)
	{
		mixNum	   = LX_ADEC_MIXER_2;
		adecModule = KDRV_ADEC_MP_04;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF3)
	{
		mixNum	   = LX_ADEC_MIXER_3;
		adecModule = KDRV_ADEC_MP_05;
	}
	else if(i_eAdecMode == LX_ADEC_LLB_BUF)
	{
		mixNum	   = LX_ADEC_MIXER_4;
		adecModule = KDRV_ADEC_MP_06;
	}
	else
	{
		ADEC_ERROR("RESUME : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	if( (i_eAdecMode >= LX_ADEC_MIX_BUF0)
	  &&(i_eAdecMode <= LX_ADEC_MIX_BUF3 || i_eAdecMode == LX_ADEC_LLB_BUF) )
	{
		//Check a current clip audio status
		if(g_adec_kdrv.mixerStatus[mixNum] == LX_ADEC_CLIP_MODE_RESUME)
		{
#ifdef ENABLE_NETCAST_PRINT
			ADEC_DEBUG("ADEC_L9_ResumeAudioClip : Same(mix = %d, status = %d)\n", mixNum, g_adec_kdrv.clipStatus[mixNum]);
#else
			ADEC_ERROR("[%s]ADEC_L9_ResumeAudioClip : Same(mix = %d, status = %d)\n", sourcetype[i_eAdecMode], mixNum, g_adec_kdrv.clipStatus[mixNum]);
#endif

			return RET_OK;
		}
	}

	//Send a IPC command
	ADEC_IPC_Resume(adecModule);

	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		/* Update Internal Status Value */
		g_adec_kdrv.clipStatus[decNum] = LX_ADEC_CLIP_MODE_RESUME;

		//Set a Mixer resume for GCD platform
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Send a IPC command
			ADEC_IPC_Resume(adecMixerModule);
		}
	}
	else if( (i_eAdecMode >= LX_ADEC_MIX_BUF0)
		   &&(i_eAdecMode <= LX_ADEC_MIX_BUF3 || i_eAdecMode == LX_ADEC_LLB_BUF) )
	{
		/* Update Internal Status Value */
		g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_RESUME;
	}

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("Audio clip play resumed(adecMode = 0x%X)...\n", i_eAdecMode);
#else
	ADEC_ERROR("[%s]Audio clip play resumed(adecMode = 0x%X)...\n", sourcetype[i_eAdecMode], i_eAdecMode);
#endif

	return RET_OK;
}

/**
 * Flushes a already audio feeded clip data.
 * After calling this IOCTL function, a audio clip data is flushed.
 * @see ADEC_DRV_PauseAudioClip
*/
int ADEC_L9_FlushAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	UINT8	decNum = LX_ADEC_DEC_0;
	UINT8	mixNum = LX_ADEC_MIXER_0;

	KDRV_ADEC_MOD_T 	adecModule 		= KDRV_ADEC_DEC_0;
	KDRV_ADEC_MOD_T 	adecMixerModule = KDRV_ADEC_MP_00;

	//Flush decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Check a adec mode
		if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
		{
			decNum	   		= LX_ADEC_DEC_0;
			adecModule 		= KDRV_ADEC_DEC_0;
			adecMixerModule = KDRV_ADEC_MP_00;

			//clear a pts index0
			AUD_Wr(aud_pts_index0, 0x0);
			AUD_WrFL(aud_pts_index0);

			//clear a pts index0 for dec0
			AUD_Wr(aud_in_index_dec0, 0x0);
			AUD_WrFL(aud_in_index_dec0);

			//clear a present index0 for dec0
			AUD_Wr(aud_in_index_present0, 0x0);
			AUD_WrFL(aud_in_index_present0);

			//delete CPBM structure
			if(g_pCPBM_WriterStruct != NULL)
			{
				OS_KFree(g_pCPBM_WriterStruct);
				g_pCPBM_WriterStruct = NULL;
			}
		}
		else
		{
			decNum			= LX_ADEC_DEC_1;
			adecModule 		= KDRV_ADEC_DEC_1;
			adecMixerModule = KDRV_ADEC_MP_01;

			//clear a pts index1
			AUD_Wr(aud_pts_index1, 0x0);
			AUD_WrFL(aud_pts_index1);

			//clear a pts index1 for dec1
			AUD_Wr(aud_in_index_dec1, 0x0);
			AUD_WrFL(aud_in_index_dec1);

			//clear a present index1 for dec1
			AUD_Wr(aud_in_index_present1, 0x0);
			AUD_WrFL(aud_in_index_present1);

			//delete CPBA structure
			if(g_pCPBA_WriterStruct != NULL)
			{
				OS_KFree(g_pCPBA_WriterStruct);
				g_pCPBA_WriterStruct = NULL;
			}
		}

		//Check a current clip audio status
		if( (g_adec_kdrv.clipStatus[decNum]  == LX_ADEC_CLIP_MODE_FLUSH)
		  ||(g_adec_kdrv.bDecStarted[decNum] != TRUE) )
		{
#ifdef ENABLE_NETCAST_PRINT
			ADEC_DEBUG("ADEC_L9_FlushAudioClip : Same(dec = %d, status = %d)\n", decNum, g_adec_kdrv.clipStatus[decNum]);
#else
			ADEC_ERROR("[%s]ADEC_L9_FlushAudioClip : Same(dec = %d, status = %d)\n", sourcetype[i_eAdecMode] , decNum, g_adec_kdrv.clipStatus[decNum]);
#endif

			return RET_OK;
		}

		/* Clear aui index count */
		g_adec_kdrv.index[decNum].FeedCNT = 0;
		g_adec_kdrv.index[decNum].FeedIDX = 0;

		g_adec_kdrv.index[decNum].PrevNOTI = INITIAL_CONDITION ;
		g_adec_kdrv.index[decNum].NewNOTI  = 0 ;
		g_adec_kdrv.index[decNum].WriteCNT = INITIAL_CONDITION;
		g_adec_kdrv.index[decNum].ReadCNT  = 0;
	 	g_adec_kdrv.index[decNum].FreeCNT  = CPB_AUI_INDEX_COUNT;
	 	g_adec_kdrv.index[decNum].UsedCNT  = 0;
	 	g_adec_kdrv.index[decNum].NotiCNT  = 0;
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF0)
	{
		mixNum	   = LX_ADEC_MIXER_0;
		adecModule = KDRV_ADEC_MP_02;

		/* Clear mixer aui index count */
		g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_0] = 0;

		//clear a mixer index
		AUD_Wr(aud_in_index_mix0, 0x0);
		AUD_WrFL(aud_in_index_mix0);

		//clear a present index for mix0
		AUD_Wr(aud_in_index_present2, 0x0);
		AUD_WrFL(aud_in_index_present2);

		//delete MIX0 structure
		if(g_pMIX0_WriterStruct != NULL)
		{
			OS_KFree(g_pMIX0_WriterStruct);
			g_pMIX0_WriterStruct = NULL;
		}
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF1)
	{
		mixNum	   = LX_ADEC_MIXER_1;
		adecModule = KDRV_ADEC_MP_03;

		/* Clear mixer aui index count */
		g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_1] = 0;

		//clear a mixer index
		AUD_Wr(aud_in_index_mix1, 0x0);
		AUD_WrFL(aud_in_index_mix1);

		//clear a present index for mix1
		AUD_Wr(aud_in_index_present3, 0x0);
		AUD_WrFL(aud_in_index_present3);

		//delete MIX1 structure
		if(g_pMIX1_WriterStruct != NULL)
		{
			OS_KFree(g_pMIX1_WriterStruct);
			g_pMIX1_WriterStruct = NULL;
		}
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF2)
	{
		mixNum	   = LX_ADEC_MIXER_2;
		adecModule = KDRV_ADEC_MP_04;

		/* Clear mixer aui index count */
		g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_2] = 0;

		//clear a mixer index
		AUD_Wr(aud_in_index_mix2, 0x0);
		AUD_WrFL(aud_in_index_mix2);

		//clear a present index for mix2
		AUD_Wr(aud_in_index_present4, 0x0);
		AUD_WrFL(aud_in_index_present4);

		//delete MIX2 structure
		if(g_pMIX2_WriterStruct != NULL)
		{
			OS_KFree(g_pMIX2_WriterStruct);
			g_pMIX2_WriterStruct = NULL;
		}
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF3)
	{
		mixNum	   = LX_ADEC_MIXER_3;
		adecModule = KDRV_ADEC_MP_05;

		/* Clear mixer aui index count */
		g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_3] = 0;

		//clear a mixer index
		AUD_Wr(aud_in_index_mix3, 0x0);
		AUD_WrFL(aud_in_index_mix3);

		//clear a present index for mix3
		AUD_Wr(aud_in_index_present5, 0x0);
		AUD_WrFL(aud_in_index_present5);

		//delete MIX3 structure
		if(g_pMIX3_WriterStruct != NULL)
		{
			OS_KFree(g_pMIX3_WriterStruct);
			g_pMIX3_WriterStruct = NULL;
		}
	}
	else if(i_eAdecMode == LX_ADEC_LLB_BUF)
	{
		mixNum	   = LX_ADEC_MIXER_4;
		adecModule = KDRV_ADEC_MP_06;

		/* Clear mixer aui index count */
		g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_4] = 0;

		//clear a mixer index
		AUD_Wr(aud_in_index_llb, 0x0);
		AUD_WrFL(aud_in_index_llb);

		//clear a present index for mix3
		AUD_Wr(aud_in_index_llb, 0x0);
		AUD_WrFL(aud_in_index_llb);

		//delete LLB structure
		if(g_pLLB_WriterStruct != NULL)
		{
			OS_KFree(g_pLLB_WriterStruct);
			g_pLLB_WriterStruct = NULL;
		}
	}
	else
	{
		ADEC_ERROR("FLUSH : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Send a IPC command
		ADEC_IPC_Pause(adecModule);

		/* Update Internal Status Value */
		g_adec_kdrv.bDecStarted[decNum] = FALSE;
		g_adec_kdrv.clipStatus[decNum]  = LX_ADEC_CLIP_MODE_FLUSH;

		//Set a Mixer pause for GCD platform
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Send a IPC command
			ADEC_IPC_Pause(adecMixerModule);
		}
	}
	else if( (i_eAdecMode >= LX_ADEC_MIX_BUF0)
		   &&(i_eAdecMode <= LX_ADEC_MIX_BUF3 || i_eAdecMode == LX_ADEC_LLB_BUF) )
	{
		//Check a previous status
		if(g_adec_kdrv.mixerStatus[mixNum] != LX_ADEC_CLIP_MODE_PAUSE)
		{
			//Send a IPC command
			ADEC_IPC_Pause(adecModule);
		}

		/* Update Internal Status Value */
		g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_FLUSH;
	}

	//Send a IPC command
	ADEC_IPC_Flush(adecModule);

	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Set a Mixer flush for GCD platform
		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Send a IPC command
			ADEC_IPC_Flush(adecMixerModule);
		}
	}

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("Audio clip play flushed(adecMode = 0x%X)...\n", i_eAdecMode);
#else
	ADEC_ERROR("[%s]Audio clip play flushed(adecMode = 0x%X)...\n", sourcetype[i_eAdecMode] ,i_eAdecMode);
#endif

	return RET_OK;
}

/**
 * Resets a current audio clip playing.
 * After calling this IOCTL function, a audio clip is played forcedly.
 * @see ADEC_DRV_PauseAudioClip
*/
int ADEC_L9_ResetAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	int eRetVal = RET_OK;

	UINT8	decNum = LX_ADEC_DEC_0;

	UINT8	avLipSyncNum = 0;

	KDRV_ADEC_MOD_T 	decMode = KDRV_ADEC_DEC_0;

	LX_ADEC_SRC_TYPE_T		eCurLoadedCodec;

	//Reset decoding
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN || i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		//Check a adec mode
		if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
		{
			decNum  = LX_ADEC_DEC_0;
			decMode = KDRV_ADEC_DEC_0;

			//delete CPBM structure
			if(g_pCPBM_WriterStruct != NULL)
			{
				OS_KFree(g_pCPBM_WriterStruct);
				g_pCPBM_WriterStruct = NULL;
			}
		}
		else
		{
			decNum  = LX_ADEC_DEC_1;
			decMode = KDRV_ADEC_DEC_1;

			//delete CPBA structure
			if(g_pCPBA_WriterStruct != NULL)
			{
				OS_KFree(g_pCPBA_WriterStruct);
				g_pCPBA_WriterStruct = NULL;
			}
		}

		//Check a av lip sync register number
		if(decNum == LX_ADEC_DEC_0 && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(decNum == LX_ADEC_DEC_1 && g_adec_kdrv.adecSource[LX_ADEC_DEC_0] != LX_ADEC_IN_PORT_SYSTEM)
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}
		else if(decNum == LX_ADEC_DEC_1)
		{
			avLipSyncNum = LX_ADEC_DEC_1;
		}
		else
		{
			avLipSyncNum = LX_ADEC_DEC_0;
		}

		//Clear a av lip sync register
		if(avLipSyncNum == LX_ADEC_DEC_0)
		{
			// Set DEC0 Clock Basetime
			AUD_Wr(aud_dec0_cbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec0_cbt);

			// Set DEC0 Stream Basetime
			AUD_Wr(aud_dec0_sbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec0_sbt);
		}
		else
		{
			// Set DEC1 Clock Basetime
			AUD_Wr(aud_dec1_cbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec1_cbt);

			// Set DEC1 Stream Basetime
			AUD_Wr(aud_dec1_sbt, 0xFFFFFFFF);
			AUD_WrFL(aud_dec1_sbt);
		}

		ADEC_IPC_Stop(decMode);

		/* Update Internal Status Value */
		g_adec_kdrv.bDecStarted[decNum]   = FALSE;
		g_adec_kdrv.clipStatus[decNum]    = LX_ADEC_CLIP_MODE_NONE;
		g_adec_kdrv.clipBufStatus[decNum] = LX_ADEC_CLIP_BUF_NONE;

		//Save a global current codec to re-load codec
		eCurLoadedCodec = g_adec_kdrv.adecType[decNum];

#ifdef ENABLE_AAC_PARSER
		/* Terminate AAC codec parser */
		if( (g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_AAC)	\
		  ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_HEAAC) )
		{
			/* Save AAC parsing status */
			g_adec_kdrv.bAacParsed[decNum] 	  = FALSE;
			g_adec_kdrv.bAacDSIParsed[decNum] = FALSE;

			if(g_adec_kdrv.aacProbe[decNum] != NULL)
			{
				/* Terminate AAC probe parser */
				(void)ADEC_AAC_Probe_Terminate(g_adec_kdrv.aacProbe[decNum]);

				/* Set NULL pointer */
				g_adec_kdrv.aacProbe[decNum] = NULL;
			}
		}
#endif	//#ifdef ENABLE_AAC_PARSER
	}
	else
	{
		ADEC_ERROR("Reset : mode is not defined(adecMode = 0x%X)!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	g_eCurLoadedCodec = LX_ADEC_SRC_TYPE_NONE;

	//Init IPC Register and memory
	ADEC_IPC_REG_Init(
		(void *)&gpRealRegAUD->aud_ipc_cmd_rptr,
		(void *)&gpRealRegAUD->aud_ipc_req_rptr,
		(void *)&gpRealRegAUD->aud_ipc_dbg_rptr);
	ADEC_IPC_Init(pstMemCfgADEC->ipc_memory_base, pstMemCfgADEC->ipc_memory_size);

	//Register IPC callback function
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_UNDERFLOW,   ADEC_REQ_PROC_Underflow);
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_PRESENT_END, ADEC_REQ_PROC_PresentEnd);
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_DSP0NOTRSP,  ADEC_REQ_PROC_DSP0NotRespond);

	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_DEC_RESULT,  ADEC_RSP_CMD_Status);
	ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_ALIVE, 	  ADEC_RSP_PROC_Alive);

	//Set DSP1 codec for audio processing
	eRetVal = ADEC_L9_LoadDSP1Codec();
	if(eRetVal != RET_OK)
	{
		ADEC_ERROR("error in ADEC_L9_LoadDSP1Codec call\n");
		return RET_ERROR;
	}

	//Set codec source by adec type
	eRetVal = ADEC_L9_LoadDSP0Codec(eCurLoadedCodec);
	if (eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_DRV_StartDecoding : Error!  eSrcType = 0x%x\n", eCurLoadedCodec);
		return RET_ERROR;
	}

	ADEC_PRINT("ADEC_DRV_ResetAudioClip(adecMode = 0x%X)...\n", i_eAdecMode);
	return RET_OK;
}

/**
 * Controls a AV clock for av lip sync operation..
 * After calling this IOCTL function, a presenting audio is resumed and paused.
 * This function only is called in GCD model in clock control.
 * @see ADEC_DRV_ResumeAudioClip, ADEC_DRV_PauseAudioClip
*/
int ADEC_L9_SetAVClockControl ( LX_ADEC_AV_CLOCK_INFO_T	i_stAVClockInfo )
{
	UINT8	decNum = LX_ADEC_DEC_0;
	UINT8	mixNum = LX_ADEC_MIXER_0;

	ADEC_CMD_SYNC_BASE_T 	syncBase;

	KDRV_ADEC_MOD_T 	 	adecModule 		= KDRV_ADEC_DEC_0;
	KDRV_ADEC_MOD_T 	 	adecMixerModule = KDRV_ADEC_MP_00;

	//Check a adec mode
	if(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum = LX_ADEC_DEC_0;
	}
	else if(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO)
	{
		decNum = LX_ADEC_DEC_1;
	}
	else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF0)
	{
		mixNum = LX_ADEC_MIXER_0;
	}
	else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF1)
	{
		mixNum = LX_ADEC_MIXER_1;
	}
	else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF2)
	{
		mixNum = LX_ADEC_MIXER_2;
	}
	else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF3)
	{
		mixNum = LX_ADEC_MIXER_3;
	}
	else if(i_stAVClockInfo.adecMode == LX_ADEC_LLB_BUF)
	{
		mixNum = LX_ADEC_MIXER_4;
	}
	else
	{
		ADEC_ERROR("SetAVClockControl : mode is not defined(adecMode = 0x%X)!!!\n", i_stAVClockInfo.adecMode);
		return RET_ERROR;
	}

	//Set a base time for lip sync and Resume or Pause presenting
	if(i_stAVClockInfo.adecCtrl == LX_ADEC_CTRL_SET_TIME)
	{
		//Check a adec module
		if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)	\
		  &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM) )
		{
			adecModule = KDRV_ADEC_DEC_0;
		}
		else if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO)	\
		  	   &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_1] == LX_ADEC_IN_PORT_SYSTEM) )
		{
			adecModule = KDRV_ADEC_DEC_1;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF0)
		{
			adecModule = KDRV_ADEC_MP_02;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF1)
		{
			adecModule = KDRV_ADEC_MP_03;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF2)
		{
			adecModule = KDRV_ADEC_MP_04;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF3)
		{
			adecModule = KDRV_ADEC_MP_05;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_LLB_BUF)
		{
			adecModule = KDRV_ADEC_MP_06;
		}
		else
		{
			ADEC_ERROR("SetAVClockControl 1 : mode, src is not defined(adecMode = 0x%X)(src0 = %d, src1 = %d)!!!\n",	\
						i_stAVClockInfo.adecMode, g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecSource[LX_ADEC_DEC_1]);
			return RET_ERROR;
		}

		ADEC_ERROR("clockBaseTime (%d) : %8d ms\n", decNum, (UINT32)i_stAVClockInfo.clockBaseTime);
		ADEC_ERROR("streamBaseTime(%d) : %8d ms\n", decNum, (UINT32)i_stAVClockInfo.streamBaseTime);

		//Send a clock / stream base time for IPC
		syncBase.module 	= adecModule;
		syncBase.clockBase	= i_stAVClockInfo.clockBaseTime  & 0x0FFFFFFF;
		syncBase.streamBase = i_stAVClockInfo.streamBaseTime & 0x0FFFFFFF;

		ADEC_IPC_SetBaseTime(syncBase);
	}
	else
	{
		//Check a adec module
		if(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
		{
			adecMixerModule = KDRV_ADEC_MP_00;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO)
		{
			adecMixerModule = KDRV_ADEC_MP_01;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF0)
		{
			adecMixerModule = KDRV_ADEC_MP_02;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF1)
		{
			adecMixerModule = KDRV_ADEC_MP_03;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF2)
		{
			adecMixerModule = KDRV_ADEC_MP_04;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_MIX_BUF3)
		{
			adecMixerModule = KDRV_ADEC_MP_05;
		}
		else if(i_stAVClockInfo.adecMode == LX_ADEC_LLB_BUF)
		{
			adecMixerModule = KDRV_ADEC_MP_06;
		}
		else
		{
			ADEC_ERROR("SetAVClockControl 2 : mode, src is not defined(adecMode = 0x%X)(src0 = %d, src1 = %d)!!!\n",	\
						i_stAVClockInfo.adecMode, g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecSource[LX_ADEC_DEC_1]);
			return RET_ERROR;
		}

		//Set a base time for lip sync and Resume or Pause presenting
		if(i_stAVClockInfo.adecCtrl == LX_ADEC_CTRL_PLAY)
		{
			if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
			  ||(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO) )
			{
				//Check a current clip audio status
				if(g_adec_kdrv.clipStatus[decNum] == LX_ADEC_CLIP_MODE_RESUME)
				{
					ADEC_DEBUG("ADEC_L9_SetAVClockControl : Same(dec = %d, status = %d)\n", decNum, g_adec_kdrv.clipStatus[decNum]);
					return RET_OK;
				}
			}
			else
			{
				//Check a current clip audio status
				if(g_adec_kdrv.mixerStatus[mixNum] == LX_ADEC_CLIP_MODE_RESUME)
				{
					ADEC_DEBUG("ADEC_L9_SetAVClockControl : Same(mix = %d, status = %d)\n", mixNum, g_adec_kdrv.clipStatus[mixNum]);

					return RET_OK;
				}
			}

			if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
			  ||(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO) )
			{
				//Send a IPC command to delete a previous PCM data after SRC, LIPSYNC, MP0X buffer
				ADEC_IPC_Flush(adecMixerModule);
			}
			else
			{
				if(g_adec_kdrv.bMixStarted[mixNum] != TRUE)
				{
					/* Update Internal Status Value */
					g_adec_kdrv.bMixStarted[mixNum] = TRUE;
					g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_PLAY;
				}
				else
				{
					//Send a IPC command to delete a previous PCM data after pause command.
					ADEC_IPC_Flush(adecMixerModule);
				}
			}

			//Send a IPC command
			ADEC_IPC_Resume(adecMixerModule);

			if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
			  ||(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO) )
			{
				/* Update Internal Status Value */
				g_adec_kdrv.clipStatus[decNum]  = LX_ADEC_CLIP_MODE_RESUME;
			}
			else
			{
				/* Update Internal Status Value */
				g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_RESUME;
			}
		}
		else if(i_stAVClockInfo.adecCtrl == LX_ADEC_CTRL_PAUSE)
		{
			if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
			  ||(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO) )
			{
				//Check a current clip audio status
				if(g_adec_kdrv.clipStatus[decNum] == LX_ADEC_CLIP_MODE_PAUSE)
				{
					ADEC_DEBUG("ADEC_L9_SetAVClockControl : Same(dec = %d, status = %d)\n", decNum, g_adec_kdrv.clipStatus[decNum]);
					return RET_OK;
				}
			}
			else
			{
				//Check a current clip audio status
				if(g_adec_kdrv.mixerStatus[mixNum] == LX_ADEC_CLIP_MODE_PAUSE)
				{
					ADEC_DEBUG("ADEC_L9_SetAVClockControl : Same(mix = %d, status = %d)\n", mixNum, g_adec_kdrv.clipStatus[mixNum]);

					return RET_OK;
				}
			}

			//Send a IPC command
			ADEC_IPC_Pause(adecMixerModule);

			if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
			  ||(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO) )
			{
				/* Update Internal Status Value */
				g_adec_kdrv.clipStatus[decNum]  = LX_ADEC_CLIP_MODE_PAUSE;
			}
			else
			{
				/* Update Internal Status Value */
				g_adec_kdrv.mixerStatus[mixNum] = LX_ADEC_CLIP_MODE_PAUSE;
			}
		}
		else
		{
			ADEC_ERROR("SetAVClockControl : ctrl is not defined(ClkCtrl = 0x%X)!!!\n", i_stAVClockInfo.adecCtrl);
			return RET_ERROR;
		}
	}

	if( (i_stAVClockInfo.adecMode == LX_ADEC_DECODE_MAIN)
	  ||(i_stAVClockInfo.adecMode == LX_ADEC_DECODE_ASSO) )
	{
		ADEC_ERROR("ADEC_L9_SetAVClockControl(ClkCtrl = %d)(dec = %d)...\n", i_stAVClockInfo.adecCtrl, decNum);
	}
	else
	{
		ADEC_ERROR("ADEC_L9_SetAVClockControl(ClkCtrl = %d)(mix = %d)...\n", i_stAVClockInfo.adecCtrl, decNum);
	}

	return RET_OK;
}


#ifdef ENABLE_AAC_PARSER
/**
 * Starts a playing of the aac audio clip.
 * This function starts audio dsp only for aac audio clip after aac profile and format is parsed in kadp function.
 * @see ADEC_DRV_LoadAudioClip
*/
int ADEC_L9_StartAacAudioClip ( LX_ADEC_AAC_START_INFO_T stAacStartInfo )
{
	int eRetVal = RET_OK;

	UINT8	decNum = 0;

	ADEC_BUF_T	*pAdecBuf = NULL;

	UINT64 ui64TS90kHzTick = 0;
	UINT64 clockBaseTime  = 0;
	UINT64 streamBaseTime = 0;

	UINT32	vdecDecCbt = 0;
	UINT32	vdecDecSbt = 0;
	UINT32	adecDecSbt = 0;

	KDRV_ADEC_MOD_T 			adecModule = KDRV_ADEC_DEC_0;
	KDRV_ADEC_INPUT_PORT_T		inputMode  = KDRV_ADEC_INPUT_SYSTEM_0;

	KDRV_ADEC_DEC_PARAM_T		decParam;
	KDRV_ADEC_AAC_INFO_T 		aacInfo;

	ADEC_CMD_SYNC_BASE_T 		syncBase;
	KDRV_ADEC_LIPSYNC_PARAM_T 	decLipsyncParam;

	//Create a buffer if not created
	eRetVal = ADEC_L9_CreateAudioClipBuffer(stAacStartInfo.adecMode);

	if(eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer Fail!!!(0x%X)\n", eRetVal);
		return RET_ERROR;
	}

	//Check a adec mode
	if(stAacStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		decNum     = LX_ADEC_DEC_0;
		adecModule = KDRV_ADEC_DEC_0;
		inputMode  = KDRV_ADEC_INPUT_SYSTEM_0;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pCPBM_WriterStruct;
	}
	else
	{
		decNum     = LX_ADEC_DEC_1;
		adecModule = KDRV_ADEC_DEC_1;
		inputMode  = KDRV_ADEC_INPUT_SYSTEM_1;

		//Set a ADEC buffer
		pAdecBuf = (ADEC_BUF_T *)g_pCPBA_WriterStruct;
	}

	//Set codec source by adec type
	eRetVal = ADEC_L9_LoadDSP0Codec(LX_ADEC_SRC_TYPE_AAC);

	if (eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_StartAacAudioClip : Error!\n");
		return RET_ERROR;
	}

	ADEC_IPC_GetConfig(adecModule, &decParam);

	//Set a AAC codec info.
	decParam.adecInputPort = inputMode;
	decParam.adecCodecType = ADEC_ConvertCodecType(LX_ADEC_SRC_TYPE_AAC);
	if(stAacStartInfo.samplingFreq != LX_ADEC_SAMPLING_FREQ_NONE)
	{
		decParam.samplingFreq = stAacStartInfo.samplingFreq;
	}

	//Check a DRC, Downmix mode for AAC(HEAAC) decoding
	aacInfo.numOfParam = NUM_OF_PARAM_AAC_INFO;
	aacInfo.aacFormat 	= stAacStartInfo.aacFormat;
	aacInfo.drcMode 	= g_adec_kdrv.drcMode[decNum];
	aacInfo.downmixMode = KDRV_ADEC_DOWNMIX_MODE_LORO;
	memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));

	ADEC_IPC_SetConfig(adecModule, &decParam);

#if 0
	//Set a repeat function
	if(stAacStartInfo.repeatNumber > 1)
	{
		(void)pAdecBuf->set_repeat(stAacStartInfo.repeatNumber - 1, pAdecBuf);
	}
#endif

	//Get lip sync parameter for IPC command
	ADEC_IPC_GetLipsync(adecModule, &decLipsyncParam);

	//Set a lip sync bound
	decLipsyncParam.ubound			= 0x64;
	decLipsyncParam.lbound			= 0x64;
	decLipsyncParam.offset			= 0x384;
	decLipsyncParam.freerunubound	= 0x1388;	// 5 sec
	decLipsyncParam.freerunlbound	= 0x1D4C0;	// 120 sec

	//Set lip sync parameters
	ADEC_IPC_SetLipsync(adecModule, &decLipsyncParam);

	BASE_AVLIPSYNC_LOCK();

	AUD_RdFL(aud_gstcclreg);
	clockBaseTime = AUD_Rd(aud_gstcclreg) & 0x0FFFFFFF;

	ui64TS90kHzTick = g_adec_kdrv.index[decNum].AUI[0].streamtime;

	if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
	{
		ui64TS90kHzTick *= 9;

		if(ui64TS90kHzTick)
		{
			do_div(ui64TS90kHzTick, 100000);		// = Xns * 90 000 / 1 000 000 000
		}
	}
	streamBaseTime = (ui64TS90kHzTick) & 0x0FFFFFFF;

	//Read a vdec cbt and sbt time
	if(decNum == LX_ADEC_DEC_0)
	{
		vdecDecCbt = g_pVDEC_LipsyncReg->vdec_dec0_cbt;
		vdecDecSbt = g_pVDEC_LipsyncReg->vdec_dec0_sbt;
	}
	else
	{
		vdecDecCbt = g_pVDEC_LipsyncReg->vdec_dec1_cbt;
		vdecDecSbt = g_pVDEC_LipsyncReg->vdec_dec1_sbt;
	}

	if((vdecDecCbt == 0xFFFFFFFF) || (vdecDecSbt == 0xFFFFFFFF))
	{
		if(decNum == LX_ADEC_DEC_0)
		{
			// Set ADEC DEC0 Clock Basetime
			AUD_Wr(aud_dec0_cbt, (UINT32)(clockBaseTime & 0x0FFFFFFF));
			AUD_WrFL(aud_dec0_cbt);
		}
		else
		{
			// Set ADEC DEC1 Clock Basetime
			AUD_Wr(aud_dec1_cbt, (UINT32)(clockBaseTime & 0x0FFFFFFF));
			AUD_WrFL(aud_dec1_cbt);
		}

		// Set DEC Stream Basetime
		if(ui64TS90kHzTick != 0xFFFFFFFFFFFFFFFFULL)
		{
			adecDecSbt = (UINT32)(streamBaseTime & 0x0FFFFFFF);
		}
		else
		{
			adecDecSbt = (UINT32)0xFFFFFFFF;
		}

		if(decNum == LX_ADEC_DEC_0)
		{
			// Set DEC0 Stream Basetime
			AUD_Wr(aud_dec0_sbt, adecDecSbt);
			AUD_WrFL(aud_dec0_sbt);
		}
		else
		{
			// Set DEC1 Stream Basetime
			AUD_Wr(aud_dec1_sbt, adecDecSbt);
			AUD_WrFL(aud_dec1_sbt);
		}
	}
	else
	{
		if(decNum == LX_ADEC_DEC_0)
		{
			// Set DEC0 Clock Basetime
			AUD_Wr(aud_dec0_cbt, g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF);
			AUD_WrFL(aud_dec0_cbt);

			// Set DEC0 Stream Basetime
			AUD_Wr(aud_dec0_sbt, g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF);
			AUD_WrFL(aud_dec0_sbt);

			clockBaseTime  = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF;
			streamBaseTime = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF;
		}
		else
		{
			// Set DEC1 Clock Basetime
			AUD_Wr(aud_dec1_cbt, g_pVDEC_LipsyncReg->vdec_dec1_cbt & 0x0FFFFFFF);
			AUD_WrFL(aud_dec1_cbt);

			// Set DEC1 Stream Basetime
			AUD_Wr(aud_dec1_sbt, g_pVDEC_LipsyncReg->vdec_dec1_sbt & 0x0FFFFFFF);
			AUD_WrFL(aud_dec1_sbt);

			clockBaseTime  = (UINT64)g_pVDEC_LipsyncReg->vdec_dec1_cbt & 0x0FFFFFFF;
			streamBaseTime = (UINT64)g_pVDEC_LipsyncReg->vdec_dec1_sbt & 0x0FFFFFFF;
		}
	}

	BASE_AVLIPSYNC_UNLOCK();

	if(decNum == LX_ADEC_DEC_0)
	{
		ADEC_DEBUG("vdec_dec0_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_cbt);
		ADEC_DEBUG("vdec_dec0_sbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec0_sbt);
		ADEC_DEBUG("clockBaseTime  : 0x%08X!\n", AUD_Rd(aud_dec0_cbt));
		ADEC_DEBUG("streamBaseTime : 0x%08X!\n", AUD_Rd(aud_dec0_sbt));
	}
	else
	{
		ADEC_DEBUG("vdec_dec1_cbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_cbt);
		ADEC_DEBUG("vdec_dec1_sbt  : 0x%08X!\n", g_pVDEC_LipsyncReg->vdec_dec1_sbt);
		ADEC_DEBUG("clockBaseTime  : 0x%08X!\n", AUD_Rd(aud_dec1_cbt));
		ADEC_DEBUG("streamBaseTime : 0x%08X!\n", AUD_Rd(aud_dec1_sbt));
	}

	//Send a base time for IPC
	syncBase.module 	= adecModule;
	syncBase.clockBase 	= clockBaseTime;
	syncBase.streamBase = streamBaseTime;

	ADEC_IPC_SetBaseTime(syncBase);

	ADEC_IPC_Start(adecModule);

	/* Update Internal Status Value */
	g_adec_kdrv.bDecStarted[decNum] = TRUE;

	//Sound Engine Control
#ifdef ENABLE_DAMP_VOLUME_CONTROL
	ADEC_IPC_SetDisableSeEngine();
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL

	ADEC_ERROR("Clip AAC profile = %d, format = %d, sfreq = %d\n",	\
				stAacStartInfo.aacCodec, stAacStartInfo.aacFormat, stAacStartInfo.samplingFreq);

#ifdef ENABLE_NETCAST_PRINT
	ADEC_DEBUG("Audio clip AAC play started(adecMode = 0x%X)...\n", stAacStartInfo.adecMode);
#else
	ADEC_ERROR("[%s]Audio clip AAC play started(adecMode = 0x%X)...\n", sourcetype[stAacStartInfo.adecMode],stAacStartInfo.adecMode);
#endif

	return RET_OK;
}
#endif	//#ifdef ENABLE_AAC_PARSER

/**
 * Gets a audio encode memory buffer information.
 * This value is used for copying encoded ES and AUI info by mmap function.
 * @see.
*/
int ADEC_L9_GetEncBufInfo ( LX_ADEC_ENC_BUF_T *pstEncBufInfo )
{
	int		eRetVal = RET_OK;

	int		auCount  = 0;
    int 	auOffset = 0;

	ADEC_BUF_T  	*AENCReader;
	ADEC_AU_INFO_T  auInfo;

	//Sanity check
	if(pstEncBufInfo == NULL)
	{
		ADEC_ERROR("ADEC_L9_GetEncBufInfo : error!!!(pstEncBufInfo = %p)\n", pstEncBufInfo);
		return RET_ERROR;
	}

	//Set a default value
	pstEncBufInfo->writeIndex	= 0;
	pstEncBufInfo->sizeofAU		= 0;
	pstEncBufInfo->readIndex	= 0;
	pstEncBufInfo->remainIndex	= 0;

	pstEncBufInfo->encEsMemoryBase	= pstMemCfgADEC->enc_memory_base;
	pstEncBufInfo->encEsMemorySize	= pstMemCfgADEC->enc_memory_size;
	pstEncBufInfo->encAuiMemoryBase	= pstMemCfgADEC->enc_memory_base;
	pstEncBufInfo->encAuiMemorySize	= pstMemCfgADEC->enc_memory_size;

	//Create a encoder buffer
	if(g_pAENC_ReaderStruct == NULL)
	{
		/* Create CPBM for EMP */
		eRetVal = ADEC_L9_Create_AENC_Interbuf_Reader();
		if(eRetVal == WRITER_RET_OPEN_OK)
		{
			ADEC_PRINT("ADEC Create_Interbuf_Reader Success !!!\n");
		}
		else
		{
			ADEC_ERROR("ADEC Create_Interbuf_Reader Fail(%d)\n", eRetVal);
			return RET_ERROR;
		}
	}

	//Assign a encoder buffer
	AENCReader = (ADEC_BUF_T *)g_pAENC_ReaderStruct; 	//ADEC_COMMON_BUF_T *

	//Check a current buffer size
	if( (AENCReader->get_used_size(AENCReader) <= 0)
	  &&(AENCReader->get_free_size(AENCReader) < 0) )
	{
		ADEC_PRINT("ADEC_L9_GetEncBufInfo : used size = %d, free size = %d\n",	\
					AENCReader->get_used_size(AENCReader), AENCReader->get_free_size(AENCReader));
		return RET_OK;
	}

	//Read a ES AUI Count info.
	auCount = AENCReader->get_au_cnt(AENCReader);
	if(auCount < 2)
	{
		ADEC_PRINT("ADEC_L9_GetEncBufInfo : No AU Data(auCount = %d)\n", auCount);
		return RET_OK;
	}

	//Read a current ES AUI info.
	eRetVal = AENCReader->get_au_info(0, &auOffset, &auInfo, AENCReader);
	if(eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_GetEncBufInfo : get_au_info(ret = %d)\n", eRetVal);
		return RET_ERROR;
	}

	//Check a current offset
	if(auInfo.size + auOffset <= 0)
	{
		ADEC_ERROR("ADEC_L9_GetEncBufInfo : auInfo.size = %llu, auOffset = %d\n",	\
					auInfo.size, auOffset);
		return RET_ERROR;
	}

	//Set a current AU index and size of AU
	pstEncBufInfo->writeIndex  = (UINT16)auInfo.index;
	pstEncBufInfo->sizeofAU	   = (UINT16)auInfo.size;
	pstEncBufInfo->readIndex   = (UINT16)auInfo.index;
	pstEncBufInfo->remainIndex = (UINT16)(auCount - 1);

	ADEC_PRINT("ADEC_L9_GetEncBufInfo : write index = %d, size = %d\n", pstEncBufInfo->writeIndex, pstEncBufInfo->sizeofAU);
	return RET_OK;
}

/**
 * Sets a audio encoding parameter.
 * A audio encoding parameter is set by this function(audio codec, bitrate, sampling frequency,
 * number of channel, bit per sample).
 * @see.
*/
int ADEC_L9_SetEncParam ( LX_ADEC_ENC_PARAM_T stEncParam )
{
	KDRV_ADEC_ENC_PARAM_T 	encParam;

	//Get a encoder parameters
	ADEC_IPC_GetConfig(KDRV_ADEC_ENC, &encParam);

	//Set a encoder codec type
//	encParam.adecInputMode = KDRV_ADEC_INPUT_DEC0;
	encParam.aencCodecType	   = ADEC_ConvertCodecType(stEncParam.adecType);
	encParam.channelNumber	   = stEncParam.numberOfChannel;
	encParam.bitPerSample	   = stEncParam.pcmBitsPerSample;
	encParam.samplingFrequency = stEncParam.samplingFreq;
	encParam.bitRate 		   = stEncParam.bitRate;
//	encParam.bitRate = stEncParam.duration;

	//Set a encoder parameters
	ADEC_IPC_SetConfig(KDRV_ADEC_ENC, &encParam);

	ADEC_PRINT("ADEC_L9_SetEncParam\n" );
	return RET_OK;
}

/**
 * Gets a audio encoded parameter.
 * A audio encoded parameter is get by this function(audio codec, bitrate, sampling frequency,
 * number of channel, bit per sample).
 * @see.
*/
int ADEC_L9_GetEncParam ( LX_ADEC_ENC_PARAM_T *pstEncParam )
{
	UINT32	samplingfreq = 480;
	UINT32	duration 	 = 21333;

	KDRV_ADEC_ENC_PARAM_T 	encParam;

	//Sanity check
	if(pstEncParam == NULL)
	{
		ADEC_ERROR("ADEC_L9_GetEncParam : error!!!(pstEncParam = %p)\n", pstEncParam);
		return RET_ERROR;
	}

	//Get a encoder parameters
	ADEC_IPC_GetConfig(KDRV_ADEC_ENC, &encParam);

	//Get a encoder codec type
	if(encParam.aencCodecType == KDRV_ADEC_CODEC_AAC_ENC)
	{
		pstEncParam->adecType = LX_ADEC_SRC_TYPE_AAC_LC_ENC;
	}
	else if(encParam.aencCodecType == KDRV_ADEC_CODEC_DDCO)
	{
		pstEncParam->adecType = LX_ADEC_SRC_TYPE_MS10_DDCO;
	}
	else
	{
		ADEC_ERROR("ADEC_L9_GetEncParam : error!!!(encParam.aencCodecType = %d)\n", encParam.aencCodecType);
		return RET_ERROR;
	}

	//Get a current encoder paramters
	AUD_RdFL(aud_encstat0);
	AUD_Rd01(aud_encstat0, sfreq, samplingfreq);
	AUD_Rd01(aud_encstat0, duration, duration);

	//Copy a encoded ES paramters
	pstEncParam->bitRate 		  = encParam.bitRate;
	pstEncParam->samplingFreq  	  = samplingfreq * 100;
	pstEncParam->numberOfChannel  = encParam.channelNumber;
	pstEncParam->pcmBitsPerSample = encParam.bitPerSample;
	pstEncParam->duration 		  = duration;	//micro-seconds

	ADEC_PRINT("ADEC_L9_GetEncParam\n" );
	return RET_OK;
}

/**
 * Starts a encoding of the audio input source(TP, AAD, ADC and HDMI).
 * Audio encoding data is saved ES(Elementary Stream) data and AUI(Access Unit Information)
 * by the LX_ADEC_ENC_START_T parameters.
 * @see.
*/
int ADEC_L9_StartEncoding ( LX_ADEC_ENC_START_T stStartInfo )
{
	KDRV_ADEC_ENC_PARAM_T 	encParam;

	/* Check a Encoder start status */
	if(g_adec_kdrv.bEncStarted == TRUE)
	{
		ADEC_ERROR("ADEC_L9_StartEncoding : error!!!(Encoder is running.)\n");
		return RET_ERROR;
	}

	//Get a encoder parameters
	ADEC_IPC_GetConfig(KDRV_ADEC_ENC, &encParam);

	//Set a decoder for encoding
	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		encParam.adecInputMode = KDRV_ADEC_INPUT_DEC0;
	}
	else
	{
		encParam.adecInputMode = KDRV_ADEC_INPUT_DEC1;
	}

	//Set a encoder codec type
	encParam.aencCodecType = ADEC_ConvertCodecType(stStartInfo.adecType);
//	encParam.aencOutMode	   = 0;
	encParam.channelNumber	   = 2;
	encParam.bitPerSample	   = 16;
	encParam.samplingFrequency = 480;
	encParam.bitRate 		   = 224000;
//	encParam.bitRate = stEncParam.duration;

	//Set a encoder parameters
	ADEC_IPC_SetConfig(KDRV_ADEC_ENC, &encParam);

	ADEC_IPC_Start(KDRV_ADEC_ENC);

	/* Update Internal Status Value */
	g_adec_kdrv.bEncStarted = TRUE;

	ADEC_PRINT("ADEC_L9_StartEncoding\n" );
	return RET_OK;
}

/**
 * Stops a encoding of the audio input source.
 * This function stops a encoding of the audio input source(TP, AAD, ADC and HDMI).
 * by the LX_ADEC_ENC_START_T parameters.
 * @see.
*/
int ADEC_L9_StopEncoding ( LX_ADEC_ENC_STOP_T stStopInfo )
{
	ADEC_IPC_Stop(KDRV_ADEC_ENC);

	//Delete a encoder struct buffer
	if(g_pAENC_ReaderStruct != NULL)
	{
		OS_KFree(g_pAENC_ReaderStruct);
		g_pAENC_ReaderStruct = NULL;
	}

	/* Update Internal Status Value */
	g_adec_kdrv.bEncStarted = FALSE;

	ADEC_PRINT("ADEC_L9_StopEncoding\n");
	return RET_OK;
}

/**
 * Gets a encoding information of the audio clip.
 * This function gets a current encoding AUI buffer information to get ES data using mmap memory.
 * @see.
*/
int ADEC_L9_GetEncodingInfo ( LX_ADEC_ENC_INFO_T *pstEncodingInfo )
{
 	int		eRetVal = RET_OK;

	int		auCount = 0;
    int 	auOffset = 0;
	int 	auReadSize = 0;

	unsigned char	*pEsDataBuf = NULL;
	ADEC_BUF_T		*AENCReader = NULL;

	//Sanity check
	if( (pstEncodingInfo->pBuffer == NULL)	\
	  ||(pstEncodingInfo->bufSize == 0) )
	{
		ADEC_ERROR("ADEC_L9_GetEncodingInfo : error!!!(pBuffer = %p, bufSize = %d)\n", pstEncodingInfo->pBuffer, pstEncodingInfo->bufSize);
		return RET_ERROR;
	}

	//Check a current buffer status
	if(g_pAENC_ReaderStruct == NULL)
	{
		/* Create CPBM for EMP */
		eRetVal = ADEC_L9_Create_AENC_Interbuf_Reader();
		if(eRetVal == WRITER_RET_OPEN_OK)
		{
			ADEC_PRINT("ADEC Create_Interbuf_Reader Success !!!\n");
		}
		else
		{
			ADEC_ERROR("ADEC Create_Interbuf_Reader Fail(%d)\n", eRetVal);
			return RET_ERROR;
		}
	}

	AENCReader = (ADEC_BUF_T *)g_pAENC_ReaderStruct; 	//ADEC_COMMON_BUF_T *

	//Set a default value
	memset((void *)&pstEncodingInfo->auInfo, 0x0, sizeof(LX_ADEC_AU_INFO_T));
	pstEncodingInfo->writeIndex	 = 0;
	pstEncodingInfo->readIndex	 = 0;
	pstEncodingInfo->remainIndex = 0;
	pstEncodingInfo->writeOffset = 0;
	pstEncodingInfo->readOffset  = 0;

	//Read a ES AUI Count info.
	auCount = AENCReader->get_au_cnt(AENCReader);
	if(auCount >= 2)
	{
		pstEncodingInfo->remainIndex = (UINT16)(auCount - 1);
	}
	else
	{
		ADEC_PRINT("ADEC_L9_GetEncodingInfo : No AU Data\n" );
		return RET_OK;
	}

	//Read a current ES AUI info.
	eRetVal = AENCReader->get_au_info(0, &auOffset, (ADEC_AU_INFO_T *)&pstEncodingInfo->auInfo, AENCReader);
	if(eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_GetEncodingInfo : get_au_info(ret = %d)\n", eRetVal);
		return RET_ERROR;
	}
//	AENCReader->get_au_info(1, &au_offset, &auInfo, AENCReader); // 2일 경우

	//Check a AU read size
	if(pstEncodingInfo->bufSize != (UINT16)pstEncodingInfo->auInfo.size)
	{
		ADEC_ERROR("ADEC_L9_GetEncodingInfo : bufSize error(buf = %d, auSize = %ld)\n",	\
					pstEncodingInfo->bufSize, (long int)pstEncodingInfo->auInfo.size);
		return RET_ERROR;
	}

	//Read a AU ES data from buffer
	auReadSize = (UINT32)pstEncodingInfo->auInfo.size + auOffset;
	eRetVal	   = AENCReader->read_data(&pEsDataBuf, &auReadSize, NULL, AENCReader);
	if(eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_GetEncodingInfo : read_data(ret = %d)\n", eRetVal);
		return RET_ERROR;
	}

	//Check a AU read size
	if(auReadSize != (UINT32)pstEncodingInfo->auInfo.size)
	{
		pstEncodingInfo->auInfo.size = auReadSize;
		ADEC_ERROR("ADEC_L9_GetEncodingInfo : read size error(%d)\n", auReadSize);
		return RET_ERROR;
	}

	//Copy a from AU ES data from buffer to user memory
	if(copy_to_user((void *)pstEncodingInfo->pBuffer, (void *)pEsDataBuf, auReadSize))
	{
		ADEC_ERROR("ADEC_L9_GetEncodingInfo : copy_to_user error!!!\n");
		return RET_ERROR;
	}

	//Flush a current AUI and AU data
	eRetVal	= AENCReader->flush_data(auReadSize, AENCReader);
	if(eRetVal != RET_OK)
	{
		ADEC_ERROR("ADEC_L9_GetEncodingInfo : flush_data(ret = %d)\n", eRetVal);
		return RET_ERROR;
	}

	//Set a current AU index and size of AU
	pstEncodingInfo->writeIndex  = (UINT16)pstEncodingInfo->auInfo.index;
	pstEncodingInfo->readIndex   = (UINT16)pstEncodingInfo->auInfo.index;
	pstEncodingInfo->writeOffset = auOffset;
	pstEncodingInfo->readOffset	 = auOffset;

	ADEC_PRINT("ADEC_L9_GetEncodingInfo : auReadSize = %d\n", auReadSize);
	return RET_OK;
}


/**
 * Gets a audio pcm data memory buffer information.
 * This value is used for copying decpmpressed PCM and AUI info by mmap function.
 * @see.
*/
int ADEC_L9_GetPcmBufInfo ( LX_ADEC_PCM_BUF_T *pstPcmBufInfo )
{
	int		eRetVal = RET_OK;

	int		usedSize = 0;
	int		freeSize = 0;

	ADEC_BUF_T  	*APCMReader = NULL;

	//Sanity check
	if(pstPcmBufInfo == NULL)
	{
		ADEC_ERROR("ADEC_L9_GetPcmBufInfo : error!!!(pstPcmBufInfo = %p)\n", pstPcmBufInfo);
		return RET_ERROR;
	}

	//Set a default value
	pstPcmBufInfo->writeIndex	= 0;
	pstPcmBufInfo->sizeofAU		= 0;
	pstPcmBufInfo->readIndex	= 0;
	pstPcmBufInfo->remainIndex	= 0;

	//Check a GLD PCM audio capture case(ACR buffer)
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Set a default value
		pstPcmBufInfo->pcmMemoryBase	= pstMemCfgADEC->acr_memory_base;
		pstPcmBufInfo->pcmMemorySize	= pstMemCfgADEC->acr_memory_size;
		pstPcmBufInfo->pcmAuiMemoryBase = pstMemCfgADEC->acr_memory_base;
		pstPcmBufInfo->pcmAuiMemorySize = pstMemCfgADEC->acr_memory_size;

		//Create a PCM capturing buffer
		if(g_pAACR_ReaderStruct == NULL)
		{
			/* Create ACR PCM for Capturing */
			eRetVal = ADEC_L9_Create_AACR_Interbuf_Reader();
			if(eRetVal == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("ADEC ACR PCM Create_Interbuf_Reader Success !!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC ACR PCM Create_Interbuf_Reader Fail(%d)\n", eRetVal);
				return RET_ERROR;
			}

			//Assign a ACR PCM buffer
			APCMReader = (ADEC_BUF_T *)g_pAACR_ReaderStruct; 	//ADEC_COMMON_BUF_T *

			//Read a used ACR PCM buffer size
			usedSize = APCMReader->get_used_size(APCMReader);

			//Check a remain data size.
			if(usedSize > ADEC_ACR_PCM_186MSEC)
			{
				//Flush a current AUI and AU data
				eRetVal	= APCMReader->flush_data(usedSize-ADEC_ACR_PCM_186MSEC, APCMReader);
				if(eRetVal != RET_OK)
				{
					ADEC_ERROR("ADEC_L9_GetPcmBufInfo : flush_data(ret = %d)\n", eRetVal);
					return RET_ERROR;
				}
			}
		}
		else
		{
			//Assign a PCM buffer
			APCMReader = (ADEC_BUF_T *)g_pAACR_ReaderStruct; 	//ADEC_COMMON_BUF_T *
		}

		//Read a used PCM buffer size
		usedSize = APCMReader->get_used_size(APCMReader);
		freeSize = APCMReader->get_free_size(APCMReader);

		//Set a current AU index and size of AU
		pstPcmBufInfo->writeIndex  = (UINT32)usedSize;
		pstPcmBufInfo->sizeofAU	   = (UINT32)usedSize;
		pstPcmBufInfo->readIndex   = (UINT32)usedSize;
		pstPcmBufInfo->remainIndex = (UINT32)freeSize;

	}
	else	//NC3.0 case
	{
		//Set a default value
		pstPcmBufInfo->pcmMemoryBase	= pstMemCfgADEC->ddco_memory_base;
		pstPcmBufInfo->pcmMemorySize	= pstMemCfgADEC->ddco_memory_size;
		pstPcmBufInfo->pcmAuiMemoryBase = pstMemCfgADEC->ddco_memory_base;
		pstPcmBufInfo->pcmAuiMemorySize = pstMemCfgADEC->ddco_memory_size;

		//Create a PCM capturing buffer
		if(g_pAPCM_ReaderStruct == NULL)
		{
			/* Create PCM for Capturing */
			eRetVal = ADEC_L9_Create_APCM_Interbuf_Reader();
			if(eRetVal == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("ADEC PCM Create_Interbuf_Reader Success !!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC PCM Create_Interbuf_Reader Fail(%d)\n", eRetVal);
				return RET_ERROR;
			}

			//Assign a PCM buffer
			APCMReader = (ADEC_BUF_T *)g_pAPCM_ReaderStruct; 	//ADEC_COMMON_BUF_T *

			//Read a used PCM buffer size
			usedSize = APCMReader->get_used_size(APCMReader);

			//Check a remain data size.
			if(usedSize > ADEC_PCM_200MSEC)
			{
				//Flush a current AUI and AU data
				eRetVal	= APCMReader->flush_data(usedSize-ADEC_PCM_200MSEC, APCMReader);
				if(eRetVal != RET_OK)
				{
					ADEC_ERROR("ADEC_L9_GetPcmBufInfo : flush_data(ret = %d)\n", eRetVal);
					return RET_ERROR;
				}
			}
		}
		else
		{
			//Assign a PCM buffer
			APCMReader = (ADEC_BUF_T *)g_pAPCM_ReaderStruct; 	//ADEC_COMMON_BUF_T *
		}

		//Read a used PCM buffer size
		usedSize = APCMReader->get_used_size(APCMReader);
		freeSize = APCMReader->get_free_size(APCMReader);

		//Set a current AU index and size of AU
		pstPcmBufInfo->writeIndex  = (UINT32)usedSize;
		pstPcmBufInfo->sizeofAU	   = (UINT32)usedSize;
		pstPcmBufInfo->readIndex   = (UINT32)usedSize;
		pstPcmBufInfo->remainIndex = (UINT32)freeSize;
	}

	ADEC_PRINT("ADEC_L9_GetPcmBufInfo : write index = %d, size = %d\n", pstPcmBufInfo->writeIndex, pstPcmBufInfo->sizeofAU);
	return RET_OK;
}

/**
 * Copys a PCM audio data information of the audio clip.
 * This function copys a current AUI buffer information to get PCM data using mmap memory.
 * @see.
*/
int ADEC_L9_GetCapturingInfo ( LX_ADEC_CAP_INFO_T *pstCapturingInfo )
{
 	int		eRetVal = RET_OK;

	int		usedSize = 0;

    int 	auOffset   = 0;
	int 	auReadSize = 0;

	unsigned char	*pPcmDataBuf = NULL;
	ADEC_BUF_T		*APCMReader  = NULL;

	//Sanity check
	if( (pstCapturingInfo->pBuffer == NULL)	\
	  &&(pstCapturingInfo->bufSize != 0) )
	{
		ADEC_ERROR("ADEC_L9_GetCapturingInfo : error!!!(pBuffer = %p, bufSize = %d)\n", pstCapturingInfo->pBuffer, pstCapturingInfo->bufSize);
		return RET_ERROR;
	}

	//Check a GLD PCM audio capture case(ACR buffer)
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Check a current buffer status
		if(g_pAACR_ReaderStruct == NULL)
		{
			/* Create CPBM for EMP */
			eRetVal = ADEC_L9_Create_AACR_Interbuf_Reader();
			if(eRetVal == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("ADEC ACR PCM Create_Interbuf_Reader Success !!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC ACR PCM Create_Interbuf_Reader Fail(%d)\n", eRetVal);
				return RET_ERROR;
			}
		}

		APCMReader = (ADEC_BUF_T *)g_pAACR_ReaderStruct; 	//ADEC_COMMON_BUF_T *

		//flush buffer if this case is called in app.
		if( (pstCapturingInfo->pBuffer == NULL)	\
		  ||(pstCapturingInfo->bufSize == 0) )
		{
			//Read a used ACR PCM buffer size
			usedSize = APCMReader->get_used_size(APCMReader);

			//Check a remain data size.
			if(usedSize > ADEC_ACR_PCM_186MSEC)
			{
				//Flush a current AUI and AU data
				eRetVal	= APCMReader->flush_data(usedSize-ADEC_ACR_PCM_186MSEC, APCMReader);
				if(eRetVal != RET_OK)
				{
					ADEC_ERROR("ADEC_L9_GetCapturingInfo : flush_data(ret = %d)\n", eRetVal);
					return RET_ERROR;
				}
				else
				{
					ADEC_DEBUG("ADEC_L9_GetCapturingInfo : flush_data(ret = %d)\n", eRetVal);
					return RET_OK;
				}
			}
			else
			{
				ADEC_DEBUG("ADEC_L9_GetCapturingInfo : flush_data(used = %d)(ret = %d)\n", usedSize, eRetVal);
				return RET_OK;
			}
		}

		//check max buffer read size
		if(pstCapturingInfo->bufSize > ADEC_MAX_ACR_PCM_SIZE)
		{
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : read max size error(MAX_PCM_SIZE = %d)\n", ADEC_MAX_ACR_PCM_SIZE);
			return RET_ERROR;
		}

		//Set a default value
		memset((void *)&pstCapturingInfo->auInfo, 0x0, sizeof(LX_ADEC_AU_INFO_T));
		pstCapturingInfo->writeIndex  = 0;
		pstCapturingInfo->readIndex	  = 0;
		pstCapturingInfo->remainIndex = 0;
		pstCapturingInfo->writeOffset = 0;
		pstCapturingInfo->readOffset  = 0;

		//Read a current ES AUI info.
		eRetVal = APCMReader->get_au_info(0, &auOffset, (ADEC_AU_INFO_T *)&pstCapturingInfo->auInfo, APCMReader);
		if(eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : get_au_info(ret = %d)\n", eRetVal);
			return RET_ERROR;
		}

		//Read a used ACR PCM buffer size
		usedSize = APCMReader->get_used_size(APCMReader);

		//Check a AU read size
		if(pstCapturingInfo->bufSize > (UINT32)usedSize)
		{
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : bufSize error(buf = %d, auSize = %ld)\n",	\
						pstCapturingInfo->bufSize, (long int)pstCapturingInfo->auInfo.size);
			return RET_ERROR;
		}

		//Read a AU PCM size
		auReadSize = (UINT32)pstCapturingInfo->bufSize;

		//Read a AU PCM data from buffer
		eRetVal = APCMReader->read_data(&pPcmDataBuf, &auReadSize, NULL, APCMReader);
		if(eRetVal != RET_OK)
		{
		ADEC_ERROR("ADEC_L9_GetCapturingInfo : read_data(ret = %d)\n", eRetVal);
			return RET_ERROR;
		}

		//Check a AU read size
		if(auReadSize != (UINT32)pstCapturingInfo->bufSize)
		{
			pstCapturingInfo->auInfo.size = auReadSize;
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : read size error(%d)\n", auReadSize);
			return RET_ERROR;
		}

		//Copy a from PCM data from buffer to user memory
		if(copy_to_user((void *)pstCapturingInfo->pBuffer, (void *)pPcmDataBuf, auReadSize))
		{
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : copy_to_user error!!!\n");
			return RET_ERROR;
		}

		//Flush a current AUI and AU data
		eRetVal	= APCMReader->flush_data(auReadSize, APCMReader);
		if(eRetVal != RET_OK)
		{
		ADEC_ERROR("ADEC_L9_GetCapturingInfo : flush_data(ret = %d)\n", eRetVal);
			return RET_ERROR;
		}

		//Set a current AU index and size of AU
		pstCapturingInfo->writeIndex  = (UINT32)pstCapturingInfo->auInfo.index;
		pstCapturingInfo->readIndex   = (UINT32)pstCapturingInfo->auInfo.index;
		pstCapturingInfo->writeOffset = usedSize;
		pstCapturingInfo->readOffset  = auOffset;

	}
	else	//NC3.0 case
	{
		//Check a current buffer status
		if(g_pAPCM_ReaderStruct == NULL)
		{
			/* Create CPBM for EMP */
			eRetVal = ADEC_L9_Create_APCM_Interbuf_Reader();
			if(eRetVal == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("ADEC PCM Create_Interbuf_Reader Success !!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC PCM Create_Interbuf_Reader Fail(%d)\n", eRetVal);
				return RET_ERROR;
			}
		}

		APCMReader = (ADEC_BUF_T *)g_pAPCM_ReaderStruct; 	//ADEC_COMMON_BUF_T *

		//flush buffer
		if( (pstCapturingInfo->pBuffer == NULL)	\
		  ||(pstCapturingInfo->bufSize == 0) )
		{
			//Read a used PCM buffer size
			usedSize = APCMReader->get_used_size(APCMReader);

			//Check a remain data size.
			if(usedSize > ADEC_PCM_200MSEC)
			{
				//Flush a current AUI and AU data
				eRetVal	= APCMReader->flush_data(usedSize-ADEC_PCM_200MSEC, APCMReader);
				if(eRetVal != RET_OK)
				{
					ADEC_ERROR("ADEC_L9_GetCapturingInfo : flush_data(ret = %d)\n", eRetVal);
					return RET_ERROR;
				}
				else
				{
					ADEC_DEBUG("ADEC_L9_GetCapturingInfo : flush_data(ret = %d)\n", eRetVal);
					return RET_OK;
				}
			}
			else
			{
				ADEC_DEBUG("ADEC_L9_GetCapturingInfo : flush_data(used = %d)(ret = %d)\n", usedSize, eRetVal);
				return RET_OK;
			}
		}

		//Set a default value
		memset((void *)&pstCapturingInfo->auInfo, 0x0, sizeof(LX_ADEC_AU_INFO_T));
		pstCapturingInfo->writeIndex  = 0;
		pstCapturingInfo->readIndex	  = 0;
		pstCapturingInfo->remainIndex = 0;
		pstCapturingInfo->writeOffset = 0;
		pstCapturingInfo->readOffset  = 0;

		//Read a current ES AUI info.
		eRetVal = APCMReader->get_au_info(0, &auOffset, (ADEC_AU_INFO_T *)&pstCapturingInfo->auInfo, APCMReader);
		if(eRetVal != RET_OK)
		{
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : get_au_info(ret = %d)\n", eRetVal);
			return RET_ERROR;
		}

		//Read a used PCM buffer size
		usedSize = APCMReader->get_used_size(APCMReader);

		//Check a AU read size
		if(pstCapturingInfo->bufSize > (UINT32)usedSize)
		{
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : bufSize error(buf = %d, auSize = %ld)\n",	\
						pstCapturingInfo->bufSize, (long int)pstCapturingInfo->auInfo.size);
			return RET_ERROR;
		}

		//Read a AU PCM size
		auReadSize = (UINT32)pstCapturingInfo->bufSize;

		//Read a AU PCM data from buffer
		eRetVal = APCMReader->read_data(&pPcmDataBuf, &auReadSize, NULL, APCMReader);
		if(eRetVal != RET_OK)
		{
		ADEC_ERROR("ADEC_L9_GetCapturingInfo : read_data(ret = %d)\n", eRetVal);
			return RET_ERROR;
		}

		//Check a AU read size
		if(auReadSize != (UINT32)pstCapturingInfo->bufSize)
		{
			pstCapturingInfo->auInfo.size = auReadSize;
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : read size error(%d)\n", auReadSize);
			return RET_ERROR;
		}

		//Copy a from PCM data from buffer to user memory
		if(copy_to_user((void *)pstCapturingInfo->pBuffer, (void *)pPcmDataBuf, auReadSize))
		{
			ADEC_ERROR("ADEC_L9_GetCapturingInfo : copy_to_user error!!!\n");
			return RET_ERROR;
		}

		//Flush a current AUI and AU data
		eRetVal	= APCMReader->flush_data(auReadSize, APCMReader);
		if(eRetVal != RET_OK)
		{
		ADEC_ERROR("ADEC_L9_GetCapturingInfo : flush_data(ret = %d)\n", eRetVal);
			return RET_ERROR;
		}

		//Set a current AU index and size of AU
		pstCapturingInfo->writeIndex  = (UINT32)pstCapturingInfo->auInfo.index;
		pstCapturingInfo->readIndex   = (UINT32)pstCapturingInfo->auInfo.index;
		pstCapturingInfo->writeOffset = usedSize;
		pstCapturingInfo->readOffset  = auOffset;
	}

	ADEC_PRINT("ADEC_L9_GetCapturingInfo : auReadSize = %d\n", auReadSize);
	return RET_OK;
}


/**
 * Read write to registers.
 * This function is used for verification.
 * @see
*/
int ADEC_L9_ReadAndWriteReg ( LX_ADEC_REG_INFO_T *pstRegInfo )
{
	UINT8 slaveAddr = 0, regAddr = 0, value = 0;

	switch (pstRegInfo->mode)
	{
		case LX_ADEC_REG_READ:
		{
			memcpy ( &pstRegInfo->readdata, (UINT32*)(((UINT32)gpRealRegAUD) + pstRegInfo->addr), sizeof (UINT32));
			ADEC_PRINT ("<<< ADEC_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_ADEC_REG_WRITE:
		{
			ADEC_PRINT (">>> ADEC_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			memcpy ((UINT32*)(((UINT32)gpRealRegAUD) + pstRegInfo->addr), (UINT32*)&pstRegInfo->writedata, sizeof (UINT32));
		}
		break;

		case LX_ADEC_REG_WRNRD:
		{
			ADEC_PRINT (">>> ADEC_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			memcpy ((UINT32*)(((UINT32)gpRealRegAUD) + pstRegInfo->addr), (UINT32*)&pstRegInfo->writedata, sizeof (UINT32));

			memcpy ( (UINT32*)&pstRegInfo->readdata, (UINT32*)(((UINT32)gpRealRegAUD) + pstRegInfo->addr), sizeof (UINT32));
			ADEC_PRINT ("<<< ADEC_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_ADEC_REG_GET_MAX_ADDR:
		{
			if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
				pstRegInfo->readdata = pstAdecRegCfg->reg_size - 4;
			else
				pstRegInfo->readdata = sizeof (AUD_REG_T) - 4;
			ADEC_PRINT ("<<< LX_ADEC_REG_GET_MAX_ADDR	addr = 0x%2X\n", pstRegInfo->readdata );
		}
		break;

		case LX_ADEC_REG_READ_ARRAY:
		case LX_ADEC_REG_READ_ALL:
			return RET_ERROR;
			break;


		case LX_CTOP_REG_READ:
		{
			pstRegInfo->readdata = CTOP_CTRL_L9_READ(pstRegInfo->addr);
			ADEC_PRINT ("<<< CTOP_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_CTOP_REG_WRITE:
		{
			CTOP_CTRL_L9_WRITE(pstRegInfo->addr, pstRegInfo->writedata);
			ADEC_PRINT (">>> CTOP_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
		}
		break;

		case LX_CTOP_REG_WRNRD:
		{
			if((pstRegInfo->addr == 0x0084) && ((pstRegInfo->writedata & 0x0000FF00) == 0x1800))
			{
				gbEnableDspMonitor = FALSE;
			}

			if((pstRegInfo->addr == 0x0084) && ((pstRegInfo->writedata & 0x0000FF00) == 0x0200))
			{
				gbEnableDspMonitor = TRUE;
			}

			ADEC_PRINT (">>> CTOP_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			CTOP_CTRL_L9_WRITE(pstRegInfo->addr, pstRegInfo->writedata);

			pstRegInfo->readdata = CTOP_CTRL_L9_READ(pstRegInfo->addr);
			ADEC_PRINT ("<<< CTOP_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_CTOP_REG_GET_MAX_ADDR:
		{
			if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
				pstRegInfo->readdata = sizeof (CTOP_CTRL_REG_L9B0_T) - 4;
			else
				pstRegInfo->readdata = sizeof (CTOP_CTRL_REG_L9A0_T) - 4;

			ADEC_PRINT ("<<< LX_CTOP_REG_GET_MAX_ADDR	addr = 0x%2X\n", pstRegInfo->readdata );
		}
		break;

		case LX_CTOP_REG_READ_ARRAY:
		case LX_CTOP_REG_READ_ALL:
			return RET_ERROR;
			break;


		case LX_ANALOG_REG_READ:
		case LX_ANALOG_REG_READ_ARRAY:
		{
			slaveAddr = (pstRegInfo->addr >> 8) & 0xFF;
			regAddr   = (pstRegInfo->addr >> 0) & 0xFF;
			ANALOG_ReadRegister(slaveAddr, regAddr, &value);
			pstRegInfo->readdata = (UINT32)(value & 0xFF);

			ADEC_PRINT ("<<< ANALOG_REG_READ  slaveAddr = 0x%2X, regAddr = 0x%2X, data = 0x%2X\n", slaveAddr, regAddr, pstRegInfo->readdata );
		}
		break;

		case LX_ANALOG_REG_WRITE:
		{
			slaveAddr = (pstRegInfo->addr >> 8) & 0xFF;
			regAddr   = (pstRegInfo->addr >> 0) & 0xFF;
			value     = pstRegInfo->writedata   & 0xFF;
			ANALOG_WriteRegister(slaveAddr, regAddr, value);
			ADEC_PRINT ("<<< ANALOG_REG_WRITE  slaveAddr = 0x%2X, regAddr = 0x%2X, data = 0x%2X\n", slaveAddr, regAddr, pstRegInfo->writedata);
		}
		break;

		case LX_ANALOG_REG_WRNRD:
		{
			slaveAddr = (pstRegInfo->addr >> 8) & 0xFF;
			regAddr   = (pstRegInfo->addr >> 0) & 0xFF;
			value     = pstRegInfo->writedata   & 0xFF;
			ANALOG_WriteRegister(slaveAddr, regAddr, value);
			ADEC_PRINT ("<<< ANALOG_REG_WRITE  slaveAddr = 0x%2X, regAddr = 0x%2X, data = 0x%2X\n", slaveAddr, regAddr, pstRegInfo->writedata);

			ANALOG_ReadRegister(slaveAddr, regAddr, &value);
			pstRegInfo->readdata = (UINT32)(value & 0xFF);

			ADEC_PRINT ("<<< ANALOG_REG_READ  slaveAddr = 0x%2X, regAddr = 0x%2X, data = 0x%2X\n", slaveAddr, regAddr, pstRegInfo->readdata );
		}
		break;


		default:
			return RET_ERROR;
			break;
	}

	return RET_OK;
}

/**
 * Send custom IPC command.
 * @see
*/
int ADEC_L9_CustomIpc(LX_ADEC_CUSTOM_IPC_T* pstCustomIpc)
{
	UINT32		ui32ResetCount = 0;

	ADEC_TRACE("%s func started\r\n", __FUNCTION__);
	ADEC_PRINT("HDR : %d, SIZE : %d\r\n", pstCustomIpc->hdr, pstCustomIpc->bodySize);
	if((pstCustomIpc->hdr == 0x01) || (pstCustomIpc->hdr == 0x02))
	{
		g_bLoadingCodecForDebug   = pstCustomIpc->param[0];
	}
	else if((pstCustomIpc->hdr == 0x03) || (pstCustomIpc->hdr == 0x04))
	{
		g_bCopyingCodecForDebug   = pstCustomIpc->param[0];
		g_bCopyingSeCodecForDebug = pstCustomIpc->param[1];
	}
	else if(pstCustomIpc->hdr == 0x05)
	{
		//Enable ISR debug print flag
		g_bIpcDebugReceive = pstCustomIpc->param[0];
		OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 8, DBG_COLOR_CYAN);	//ADEC_IPC_MSG

		//Enable IPC debug print by module mask bit
		AUD_Wr(aud_dsp1_dbg_mask, pstCustomIpc->param[1]);
		AUD_WrFL(aud_dsp1_dbg_mask);

		//Print DSP1 buffer memory status
		pstCustomIpc->hdr = 0x11000;
		pstCustomIpc->bodySize = 0;
		ADEC_IPC_CMD_Send(pstCustomIpc->hdr, pstCustomIpc->bodySize, pstCustomIpc->param);
	}
	else if(pstCustomIpc->hdr == 0x06)
	{
		g_bIpcDebugReceive = pstCustomIpc->param[0];
		OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 8);

		AUD_Wr(aud_dsp1_dbg_mask, 0x0000);
		AUD_WrFL(aud_dsp1_dbg_mask);
	}
	else if(pstCustomIpc->hdr == 0x07)
	{
		pstCustomIpc->hdr = 0x40403;
		pstCustomIpc->bodySize = 4;
		pstCustomIpc->param[0] = 1;
		ADEC_IPC_CMD_Send(pstCustomIpc->hdr, pstCustomIpc->bodySize, pstCustomIpc->param);
	}
	else if(pstCustomIpc->hdr == 0x08)
	{
		pstCustomIpc->hdr = 0x40403;
		pstCustomIpc->bodySize = 4;
		pstCustomIpc->param[0] = 0;
		ADEC_IPC_CMD_Send(pstCustomIpc->hdr, pstCustomIpc->bodySize, pstCustomIpc->param);
	}
	else if(pstCustomIpc->hdr == 0x10)
	{
		ADEC_IPC_Start(pstCustomIpc->param[0]);
	}
	else if(pstCustomIpc->hdr == 0x11)
	{
		ADEC_IPC_Stop(pstCustomIpc->param[0]);
	}
	else if(pstCustomIpc->hdr == 0x12)
	{
		ADEC_IPC_Start(pstCustomIpc->param[0]);
	}
	else if(pstCustomIpc->hdr == 0x13)
	{
		ADEC_IPC_Stop(pstCustomIpc->param[0]);
	}
	else if(pstCustomIpc->hdr == 0x14)
	{
		ADEC_IPC_Start(pstCustomIpc->param[0]);
	}
	else if(pstCustomIpc->hdr == 0x15)
	{
		ADEC_IPC_Stop(pstCustomIpc->param[0]);
	}
	else if(pstCustomIpc->hdr == 0x16)
	{
		ADEC_IPC_REG_Init(
			(void *)&gpRealRegAUD->aud_ipc_cmd_rptr,
			(void *)&gpRealRegAUD->aud_ipc_req_rptr,
			(void *)&gpRealRegAUD->aud_ipc_dbg_rptr);
		ADEC_IPC_Init(pstMemCfgADEC->ipc_memory_base, pstMemCfgADEC->ipc_memory_size);
		ADEC_IPC_InitFunc();

		//Register IPC callback function
		ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_UNDERFLOW,   ADEC_REQ_PROC_Underflow);
		ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_PRESENT_END, ADEC_REQ_PROC_PresentEnd);
		ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_DSP0NOTRSP,  ADEC_REQ_PROC_DSP0NotRespond);

		ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_DEC_RESULT,  ADEC_RSP_CMD_Status);
		ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_ALIVE, 	  ADEC_RSP_PROC_Alive);
	}
	else if(pstCustomIpc->hdr == 0x17)
	{
		KDRV_ADEC_DEC_PARAM_T		decParam;
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_0, &decParam);
		if(decParam.adecCodecType	== KDRV_ADEC_CODEC_MS10_DDC)
		{
			KDRV_ADEC_AC3_INFO_T ac3Info;

			// AC-3 DRC mode
			ac3Info.numOfParam = NUM_OF_PARAM_AC3_INFO;
			ac3Info.drcMode   = KDRV_ADEC_DRC_MODE_LINE;
			memcpy(&(decParam.ui32Param[0]), &ac3Info, sizeof(KDRV_ADEC_AC3_INFO_T));
		}
		else // AAC
		{
			KDRV_ADEC_AAC_INFO_T aacInfo;

			aacInfo.numOfParam = NUM_OF_PARAM_AAC_INFO;
			aacInfo.drcMode = KDRV_ADEC_DRC_MODE_LINE;
			memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));
		}
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_0, &decParam);
	}
	else if(pstCustomIpc->hdr == 0x18)
	{
		KDRV_ADEC_DEC_PARAM_T		decParam;
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_0, &decParam);
		if(decParam.adecCodecType	== KDRV_ADEC_CODEC_MS10_DDC)
		{
			KDRV_ADEC_AC3_INFO_T ac3Info;

			// AC-3 DRC mode
			ac3Info.numOfParam = NUM_OF_PARAM_AC3_INFO;
			ac3Info.drcMode   = KDRV_ADEC_DRC_MODE_RF;
			memcpy(&(decParam.ui32Param[0]), &ac3Info, sizeof(KDRV_ADEC_AC3_INFO_T));
		}
		else // AAC
		{
			KDRV_ADEC_AAC_INFO_T aacInfo;

			aacInfo.numOfParam = NUM_OF_PARAM_AAC_INFO;
			aacInfo.drcMode = KDRV_ADEC_DRC_MODE_RF;
			memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));
		}
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_0, &decParam);
	}
	else if(pstCustomIpc->hdr == 0x19)
	{
		KDRV_ADEC_DEC_PARAM_T		decParam;
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_0, &decParam);
		if(decParam.adecCodecType	== KDRV_ADEC_CODEC_MS10_DDC)
		{
			KDRV_ADEC_AC3_INFO_T ac3Info;

			// AC-3 DRC mode
			ac3Info.numOfParam = NUM_OF_PARAM_AC3_INFO;
			ac3Info.downmixMode   = KDRV_ADEC_DOWNMIX_MODE_LTRT;
			memcpy(&(decParam.ui32Param[0]), &ac3Info, sizeof(KDRV_ADEC_AC3_INFO_T));
		}
		else // AAC
		{
			KDRV_ADEC_AAC_INFO_T aacInfo;

			aacInfo.numOfParam = NUM_OF_PARAM_AAC_INFO;
			aacInfo.downmixMode = KDRV_ADEC_DOWNMIX_MODE_LTRT;
			memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));
		}
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_0, &decParam);
	}
	else if(pstCustomIpc->hdr == 0x1A)
	{
		KDRV_ADEC_DEC_PARAM_T		decParam;
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_0, &decParam);
		if(decParam.adecCodecType	== KDRV_ADEC_CODEC_MS10_DDC)
		{
			KDRV_ADEC_AC3_INFO_T ac3Info;

			// AC-3 DRC mode
			ac3Info.numOfParam = NUM_OF_PARAM_AC3_INFO;
			ac3Info.downmixMode   = KDRV_ADEC_DOWNMIX_MODE_LORO;
			memcpy(&(decParam.ui32Param[0]), &ac3Info, sizeof(KDRV_ADEC_AC3_INFO_T));
		}
		else // AAC
		{
			KDRV_ADEC_AAC_INFO_T aacInfo;

			aacInfo.numOfParam = NUM_OF_PARAM_AAC_INFO;
			aacInfo.downmixMode = KDRV_ADEC_DOWNMIX_MODE_LORO;
			memcpy(&(decParam.ui32Param[0]), &aacInfo, sizeof(KDRV_ADEC_AAC_INFO_T));
		}
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_0, &decParam);
	}
	else if(pstCustomIpc->hdr == 0x1B)
	{
		KDRV_ADEC_LIPSYNC_PARAM_T 	lipsyncParam;
		ADEC_IPC_GetLipsync(KDRV_ADEC_DEC_0, &lipsyncParam);
		lipsyncParam.offset = pstCustomIpc->param[0];
		ADEC_IPC_SetLipsync(KDRV_ADEC_DEC_0, &lipsyncParam);
	}
	else if(pstCustomIpc->hdr == 0x20)
	{
		ADEC_IPC_CheckDSPRunning();
	}
	else if(pstCustomIpc->hdr == 0x21)
	{
		ADEC_IPC_ShowDEC0Param();
	}
	else if(pstCustomIpc->hdr == 0x22)
	{
		ADEC_IPC_ShowDEC1Param();
	}
	else if(pstCustomIpc->hdr == 0x23)
	{
		ADEC_IPC_ShowSEParam();
	}
	else if(pstCustomIpc->hdr == 0x24)
	{
		ADEC_IPC_ShowSPKParam();
	}
	else if(pstCustomIpc->hdr == 0x25)
	{
		ADEC_IPC_ShowSPDIFParam();
	}
	else if(pstCustomIpc->hdr == 0x26)
	{
		ADEC_IPC_ShowHPParam();
	}
	else if(pstCustomIpc->hdr == 0x27)
	{
		ADEC_IPC_ShowSCARTParam();
	}
	else if(pstCustomIpc->hdr == 0x28)
	{
		ADEC_IPC_ShowMIX0Param();
	}
	else if(pstCustomIpc->hdr == 0x29)
	{
		ADEC_IPC_ShowLIPSYNCParam();
	}
	else if(pstCustomIpc->hdr == 0x30)
	{
		ADEC_IPC_ShowDEC0MIXParam();
	}
	else if(pstCustomIpc->hdr == 0x31)
	{
		ADEC_IPC_ShowDEC1MIXParam();
	}
	else if(pstCustomIpc->hdr == 0x32)
	{
		ADEC_IPC_ShowMIXParam();
	}
	else if(pstCustomIpc->hdr == 0x33)
	{
		ADEC_IPC_ShowAENCParam();
	}
	else if(pstCustomIpc->hdr == 0x40)
	{
		MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq (LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_SPDIF_DTO_NORMAL);
	}
	else if(pstCustomIpc->hdr == 0x41)
	{
		MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq (LX_ADEC_SAMPLING_FREQ_44_1KHZ, LX_ADEC_SPDIF_DTO_NORMAL);
	}
	else if(pstCustomIpc->hdr == 0x42)
	{
		MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq (LX_ADEC_SAMPLING_FREQ_32_KHZ, LX_ADEC_SPDIF_DTO_NORMAL);
	}
	//Use this command to reset audio clock module in audio sif task module in ddi audio code
	else if(pstCustomIpc->hdr == 0x45)
	{
		if(g_adec_kdrv.bInitDone == TRUE)
		{
			ADEC_ERROR("#####L9B0_MIXED_IP_ADEC_L9_ResetClockRegisters######\n");

			//Reset a clockregisters if DSP checks a abnormal state for clock interrupt.
			(void)L9B0_MIXED_IP_ADEC_L9_ResetClockRegisters();

			//Update DSP1 Reset Counter for debug
			AUD_RdFL(aud_dbg_dsp1_reset);
			AUD_Rd01(aud_dbg_dsp1_reset, mclk_reset_count, ui32ResetCount);
			AUD_Wr01(aud_dbg_dsp1_reset, mclk_reset_count, ui32ResetCount + 1);
			AUD_WrFL(aud_dbg_dsp1_reset);
		}
	}
	else if(pstCustomIpc->hdr == 0x46)
	{
		MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq (LX_ADEC_SAMPLING_FREQ_48_KHZ, pstCustomIpc->param[0]);
	}
	else
	{
		ADEC_IPC_CMD_Send(pstCustomIpc->hdr, pstCustomIpc->bodySize, pstCustomIpc->param);
	}

	return 0;
}

/**
 * Show status regisers from DSP inner buffers.
 * @see
*/
int ADEC_L9_ShowStausRegister ( void )
{
	UINT32 i = 0;

	KDRV_ADEC_STATUS_T 			*pu32DSP0StatusMemBase;
	KDRV_ADEC_STATUS_T 			*pu32DSP1StatusMemBase;
	KDRV_ADEC_STATUS_REG_T 		*pu32StatusRegBase;

	KDRV_ADEC_PROFILE_NODE_T 	*pNode;

	pu32DSP0StatusMemBase = (KDRV_ADEC_STATUS_T *)g_pui32DSP0Status_VirAddr;
	pu32DSP1StatusMemBase = (KDRV_ADEC_STATUS_T *)g_pui32DSP1Status_VirAddr;

	ADEC_ERROR("\n");
	ADEC_ERROR("========================================\n");
	ADEC_ERROR("DSP1    : StatusBufferCnt:[%d]\n", pu32DSP1StatusMemBase->u32StatusBuffCnt);
	ADEC_ERROR("Version : %x (%02d:%02d:%02d.%03d)\n",
		pu32DSP1StatusMemBase->version,
		pu32DSP1StatusMemBase->hour,
		pu32DSP1StatusMemBase->minute,
		pu32DSP1StatusMemBase->sec,
		pu32DSP1StatusMemBase->centiSec);
	ADEC_ERROR("========================================\n");

	for( i = 0; i < pu32DSP1StatusMemBase->u32StatusBuffCnt; i++ )
	{
		pu32StatusRegBase = (KDRV_ADEC_STATUS_REG_T *)&pu32DSP1StatusMemBase->stStatusReg[i];
		ADEC_ERROR("[%8s][free:%8d][used:%8d]\n",
			(char *)pu32StatusRegBase->u32StatusBuffName,
			pu32StatusRegBase->u32freeSize,
			pu32StatusRegBase->u32usedSize);

		msleep_interruptible(1);
	}
	ADEC_ERROR("========================================\n");

	ADEC_ERROR("DSP0    : StatusBufferCnt:[%d]\n", pu32DSP0StatusMemBase->u32StatusBuffCnt);
	ADEC_ERROR("Version : %x (%02d:%02d:%02d.%03d)\n",
		pu32DSP0StatusMemBase->version,
		pu32DSP0StatusMemBase->hour,
		pu32DSP0StatusMemBase->minute,
		pu32DSP0StatusMemBase->sec,
		pu32DSP0StatusMemBase->centiSec);
	ADEC_ERROR("========================================\n");

	for( i = 0; i < pu32DSP0StatusMemBase->u32StatusBuffCnt; i++ )
	{
		pu32StatusRegBase = (KDRV_ADEC_STATUS_REG_T *)&pu32DSP0StatusMemBase->stStatusReg[i];
		ADEC_ERROR("[%8s][free:%8d][used:%8d]\n",
			(char *)pu32StatusRegBase->u32StatusBuffName,
			pu32StatusRegBase->u32freeSize,
			pu32StatusRegBase->u32usedSize);

		msleep_interruptible(1);
	}
	ADEC_ERROR("========================================\n");

	i = 0 ;
	ADEC_ERROR("\n");
	ADEC_ERROR("============================================================\n");
	ADEC_ERROR("ProfileInfoCnt:[%d]\n",g_pstProfile_VirAddr->moduleCount);
	ADEC_ERROR("------------------------------------------------------------\n");

	for( i=0; i < g_pstProfile_VirAddr->moduleCount ; i++ )
	{
		pNode = &g_pstProfile_VirAddr->reportNode[i];
		ADEC_ERROR("[%-8s][%4d MCPS][%8d cycles (%8d ~ %8d)]\n",
			pNode->moduleName,
			pNode->mips,
			pNode->cycle,
			pNode->min == 0x7fffffff ? 0 : pNode->min,
			pNode->max);

		msleep_interruptible(1);
	}
	ADEC_ERROR("------------------------------------------------------------\n");
	ADEC_ERROR("Total MCP: %d\n",g_pstProfile_VirAddr->mipsSum);
	ADEC_ERROR("============================================================\n");

	return RET_OK;

}

/**
 * Show ADEC undeflow status.
 * @see
*/
static void ADEC_REQ_PROC_Underflow(unsigned int _hdr, unsigned int _bodySize, void* _param)
{
	ULONG	flags = 0;
	UINT32	ui32RecEventType = 0;

	char	*pModuleName;

	ADEC_REQ_PARAM_UNDERFLOW_T	*pParam;

	pParam = (ADEC_REQ_PARAM_UNDERFLOW_T *)_param;

	//Check a underflow event
	if( (g_ui32CurEventType & LX_ADEC_EVENT_DEC0_UNDERFLOW)
	  &&(pParam->module == KDRV_ADEC_MP_00) )
	{
		ui32RecEventType = LX_ADEC_EVENT_DEC0_UNDERFLOW;
	}
	else if( (g_ui32CurEventType & LX_ADEC_EVENT_DEC1_UNDERFLOW)
	  &&(pParam->module == KDRV_ADEC_MP_01) )
	{
		ui32RecEventType = LX_ADEC_EVENT_DEC1_UNDERFLOW;
	}

	//spin lock for protection
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
	g_ui32RecEventType |= ui32RecEventType;
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	//Wake up poll if any int is triggered
	if(g_ui32RecEventType)
		wake_up_interruptible_all(&adec_poll_wait_queue);

	switch(pParam->module)
	{
		case KDRV_ADEC_MP_00:
			pModuleName = "MP_00";
			break;

		case KDRV_ADEC_MP_01:
			pModuleName = "MP_01";
			break;

		case KDRV_ADEC_MP_02:
			pModuleName = "MP_02";
			break;

		case KDRV_ADEC_MP_03:
			pModuleName = "MP_03";
			break;

		case KDRV_ADEC_MP_04:
			pModuleName = "MP_04";
			break;

		case KDRV_ADEC_MP_05:
			pModuleName = "MP_05";
			break;

		default:
			pModuleName = "UNKNOWN";
			break;
		}

	if( (g_ui32CurEventType & LX_ADEC_EVENT_DEC0_UNDERFLOW)
	  &&(pParam->module == KDRV_ADEC_MP_00) )
	{
   		ADEC_DEBUG("ADEC DEC0 Underflow [%s][max:%8d][used:%8d][free:%8d]\n",
        	        pModuleName, pParam->max, pParam->used, pParam->free);
	}
	else if( (g_ui32CurEventType & LX_ADEC_EVENT_DEC1_UNDERFLOW)
		   &&(pParam->module == KDRV_ADEC_MP_01) )
	{
   		ADEC_DEBUG("ADEC DEC1 Underflow [%s][max:%8d][used:%8d][free:%8d]\n",
        	        pModuleName, pParam->max, pParam->used, pParam->free);
	}

	ADEC_DEBUG("ADEC Underflow [%s][max:%8d][used:%8d][free:%8d]\n",
    	        pModuleName, pParam->max, pParam->used, pParam->free);

	return;
}

/**
 * Show ADEC present end status.
 * @see
*/
static void ADEC_REQ_PROC_PresentEnd(unsigned int _hdr, unsigned int _bodySize, void* _param)
{
	ULONG	flags = 0;
	UINT32	ui32RecEventType = 0;

	LX_ADEC_DEC_MODE_T	ui32RecEventMode = LX_ADEC_DEC_MODE_NONE;

	char	*pModuleName;

	ADEC_REQ_PARAM_PRESENT_NOTI_T	*pParam;

	pParam = (ADEC_REQ_PARAM_PRESENT_NOTI_T *)_param;

	//Set a event type
	ui32RecEventType = LX_ADEC_EVENT_PRESENT_END;

	//Check a present end event
	if(g_ui32CurEventType & LX_ADEC_EVENT_PRESENT_END)
	{
		//Decoder
		if(pParam->module == KDRV_ADEC_MP_00)
		{
			ui32RecEventMode = LX_ADEC_DECODE_MAIN;
		}
		else if(pParam->module == KDRV_ADEC_MP_01)
		{
			ui32RecEventMode = LX_ADEC_DECODE_ASSO;
		}
		//Mixer
		else if(pParam->module == KDRV_ADEC_MP_02)
		{
			ui32RecEventMode = LX_ADEC_MIX_BUF0;
		}
		else if(pParam->module == KDRV_ADEC_MP_03)
		{
			ui32RecEventMode = LX_ADEC_MIX_BUF1;
		}
		else if(pParam->module == KDRV_ADEC_MP_04)
		{
			ui32RecEventMode = LX_ADEC_MIX_BUF2;
		}
		else if(pParam->module == KDRV_ADEC_MP_05)
		{
			ui32RecEventMode = LX_ADEC_MIX_BUF3;
		}
		else if(pParam->module == KDRV_ADEC_MP_06)
		{
			ui32RecEventMode = LX_ADEC_LLB_BUF;
		}
	}

	//spin lock for protection
	spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
	g_ui32RecEventType |= ui32RecEventType;
	g_ui32RecEventMode |= ui32RecEventMode;
	spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

	//Wake up poll if any int is triggered
	if(g_ui32RecEventType)
		wake_up_interruptible_all(&adec_poll_wait_queue);

	switch(pParam->module)
	{
		case KDRV_ADEC_MP_00:
			pModuleName = "MP_00";
			break;

		case KDRV_ADEC_MP_01:
			pModuleName = "MP_01";
			break;

		case KDRV_ADEC_MP_02:
			pModuleName = "MP_02";
			break;

		case KDRV_ADEC_MP_03:
			pModuleName = "MP_03";
			break;

		case KDRV_ADEC_MP_04:
			pModuleName = "MP_04";
			break;

		case KDRV_ADEC_MP_05:
			pModuleName = "MP_05";
			break;

		case KDRV_ADEC_MP_06:
			pModuleName = "MP_06";
			break;

		default:
			pModuleName = "UNKNOWN";
			break;
		}

	ADEC_DEBUG("ADEC Present End [%s]\n", pModuleName);

	return;
}

/**
 * Show ADEC DSP0 arespond status.
 * @see
*/
static void ADEC_REQ_PROC_DSP0NotRespond(unsigned int _hdr, unsigned int _bodySize, void* _param)
{
	ADEC_ERROR("DSP0 does not respond.\n");

	return;
}

/**
 * Callback function for DSP command status.
 * @see
*/
static void ADEC_RSP_CMD_Status(unsigned int _hdr, unsigned int _bodySize, void* _param)
{
	ADEC_RSP_DEC_PARAM_T *pParam;

	pParam = (ADEC_RSP_DEC_PARAM_T *)_param;

	// If result is 0 success, otherwise failure.
	if(pParam->result != 0)
	{
		ADEC_ERROR("ADEC CMD Rsp(dec = 0x%X, cmd = 0x%X, result = 0x%X)\n",	\
					pParam->dec_num, pParam->action_type, pParam->result);
	}

	ADEC_IPC_DEBUG("ADEC CMD Rsp(dec = 0x%X, cmd = 0x%X, result = 0x%X)\n",	\
					pParam->dec_num, pParam->action_type, pParam->result);

	return;
}

/**
 * Callback function for DSP alive status.
 * @see
*/
static void ADEC_RSP_PROC_Alive(unsigned int _hdr, unsigned int _bodySize, void* _param)
{
	ADEC_CMD_ALIVE_PARAM_T *param;

	param = (ADEC_CMD_ALIVE_PARAM_T *)_param;

	// If aliveIdentifier is 0x10, DSP0 is dead.
	if(param->aliveIdentifier == 0x11)
		g_ui32CheckDSPRunningCnt = 0;

	ADEC_IPC_DEBUG("ADEC Alive Rsp : %d\n", param->aliveIdentifier);
	return;
}

/**
 * Load a DSP1 codec firmware.
 * @see
*/
int ADEC_L9_LoadDSP1Codec ( void )
{
	int 		eRetVal = RET_OK;

	UINT32		ui32CodecSize;
	UINT32		*pui32Codec;

	UINT8		codecName[ADEC_CODEC_NAME_SIZE];	//30
	UINT64		startTick = 0;

	KDRV_ADEC_SPK_PARAM_T 			spkParam;
	KDRV_ADEC_SPDIF_PARAM_T 		spdifParam;
	KDRV_ADEC_HP_PARAM_T 			hpParam;
	KDRV_ADEC_SCART_PARAM_T 		scartParam;

	KDRV_ADEC_SE_PARAM_T 			seParam;
	ADEC_SET_SPDIF_SCMS_T 			spdifSCMS;
	KDRV_ADEC_LIPSYNC_PARAM_T 		lipsyncParam;

	KDRV_ADEC_ENC_PARAM_T 			encParam;
	KDRV_ADEC_MIX_START_PARAM_T		mixStartParam;

	ADEC_SET_PCM_INTERRUPT_T		pcmRate;
	ADEC_SET_SPDIF_DTO_T 	 		spdifDTO;

	ADEC_SET_VOLUME_T				setVolume;

	//Get a current tick time
	startTick = jiffies;

	if(NULL == g_pui32DSP1CodecVirAddr)
	{
		ADEC_ERROR("ADEC_LoadCodec : Err - g_pui32DSP1CodecVirAddr NULL!\n");
		return RET_ERROR;
	}

	//Reset Low : ADEC DSP1
	ADEC_L9_ClearReset(ADEC_OPTION_DSP1);

	//Clear aud_dsp1_state reg. for codec ver and DSP1 running location
	AUD_RdFL(aud_dsp1_state);
	AUD_Wr01(aud_dsp1_state, codec_ver, 0x0);								\
	AUD_WrFL(aud_dsp1_state);

	//Load LGSE codec for DSP1
	ui32CodecSize = sizeof(se_codec_l9);
	pui32Codec	  = (UINT32 *)se_codec_l9;
	strcpy(codecName, "se_codec_l9");

	ADEC_PRINT ( "ADEC_L9_LoadDSP1 : Started... Codec(%s), Size(%d)\n", codecName, ui32CodecSize);

	if(ui32CodecSize > pstMemCfgADEC->fw1_memory_size)
	{
		ADEC_ERROR("ADEC_LoadCodec : %s size is bigber than memory.(%d > %d)\n",	\
					codecName, ui32CodecSize, pstMemCfgADEC->fw1_memory_size);
		return RET_ERROR;
	}

	//Copy codec fw from memory to dsp1 memory
	if(g_bCopyingSeCodecForDebug == TRUE)
	{
		memcpy(g_pui32DSP1CodecVirAddr, pui32Codec, ui32CodecSize);
		ADEC_PRINT ( "ADEC_LoadCodec : Done(%s)!!!\n", codecName );
	}

	//Set DSP1 swreset register
	ADEC_L9_SetReset(ADEC_OPTION_DSP1);

	//Wait for DSP1 fw download completion to DSP1 memory and DSP1 ready
	ADEC_L9_WaitForDSP1Ready(startTick);

	//Set a current Lip sync , sound engine, SPDIF SCMS and output control mode after dsp auto reset.
	if(g_adec_kdrv.bInitDone == TRUE)
	{
		//Lock a ADEC PCM Semaphore for HDMI Input
		OS_LockMutex(&g_ADEC_Sema_Pcm);

		//Check a HDMI audio input
		if( (g_eCurAdecSource == LX_ADEC_IN_PORT_HDMI)
		  &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_HDMI) )
		{
			pcmRate.ui32InFs = (UINT32)g_adec_kdrv.adecHdmiInputFreq;
		}
		else
		{
			pcmRate.ui32InFs = (UINT32)LX_ADEC_SAMPLING_FREQ_48_KHZ;
		}

		//Set default PCM frequency to 48Khz
		(void)L9B0_MIXED_IP_ADEC_L9_SetPcmClockRate(g_adec_kdrv.adecHdmiInputFreq, APLL_CLOCK_VALUE_12_288MHZ);

		//Set a default sampling frequency and DTO rate after dsp reset
		(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, LX_ADEC_SPDIF_DTO_NORMAL);

		//Clears a PCM Clock interrupt count
		pcmRate.ui32PCMclkRate	  = (UINT32)APLL_CLOCK_VALUE_12_288MHZ;
		pcmRate.ui32ForceCntClear = (UINT32)1;

		ADEC_IPC_SetPCMRate(pcmRate);

		//Set a SPDIF DTO Frequency for IPC
		spdifDTO.ui32SampleFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;
		spdifDTO.ui32DtoRate	= LX_ADEC_SPDIF_DTO_NORMAL;
		spdifDTO.ui32ForceCntClear = (UINT32)1;
		spdifDTO.ui32isHDMI		   = (UINT32)0;
		spdifDTO.ui32HDMIFs		   = LX_ADEC_SAMPLING_FREQ_NONE;
		ADEC_IPC_SetSPDIFDTO(spdifDTO);

		//Unlock a ADEC PCM Semaphore for HDMI Input
		OS_UnlockMutex(&g_ADEC_Sema_Pcm);

		//Update a lip sync parameters
		ADEC_IPC_GetLipsync(KDRV_ADEC_DEC_0, &lipsyncParam);
		ADEC_IPC_SetLipsync(KDRV_ADEC_DEC_0, &lipsyncParam);
		ADEC_IPC_GetLipsync(KDRV_ADEC_DEC_1, &lipsyncParam);
		ADEC_IPC_SetLipsync(KDRV_ADEC_DEC_1, &lipsyncParam);

		//Update a all SE parameters already written by APP.
		ADEC_IPC_GetConfig(KDRV_ADEC_SE, &seParam);
		seParam.seFnParam.ui32FnMode 	 = (g_adec_kdrv.soundEngineMask << MASK_SE_FN_MODE_SHIFT) | LX_ADEC_SE_FN_MODE_UPDTAE_PARAM;
		seParam.seFnParam.ui32DataOption = LX_ADEC_SE_ALL;
		ADEC_IPC_SetConfig(KDRV_ADEC_SE, &seParam);

		//Start a SE engine by enabled mode
		ADEC_IPC_GetConfig(KDRV_ADEC_SE, &seParam);
		seParam.seFnParam.ui32FnMode = LX_ADEC_SE_FN_MODE_SET;
		ADEC_IPC_SetConfig(KDRV_ADEC_SE, &seParam);

		//Set a SPDIF SCMS mode for DVB model
		ADEC_IPC_GetSPDIFSCMS(&spdifSCMS);
		ADEC_IPC_SetSPDIFSCMS(spdifSCMS);

		ADEC_IPC_GetConfig(KDRV_ADEC_SPK,   &spkParam);
		ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
		ADEC_IPC_GetConfig(KDRV_ADEC_HP,    &hpParam);
		ADEC_IPC_GetConfig(KDRV_ADEC_SCART, &scartParam);

		ADEC_IPC_SetConfig(KDRV_ADEC_SPK,   &spkParam);
		ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);
		ADEC_IPC_SetConfig(KDRV_ADEC_HP,    &hpParam);
		ADEC_IPC_SetConfig(KDRV_ADEC_SCART, &scartParam);

		//Set a HP volume
		setVolume.module = KDRV_ADEC_HP;
		setVolume.ui32Volume = g_adec_kdrv.hpVolume;
		ADEC_IPC_SetVolume(setVolume);

		if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
		{
			//Restart SPDIF output type for GCD platform
			eRetVal = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
			if (eRetVal != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
			}

			//Restart a DDCO encoder for GCD platform
			if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO)
			{
				//Get a encoder parameters
				ADEC_IPC_GetConfig(KDRV_ADEC_ENC, &encParam);

				encParam.adecInputMode 	   = KDRV_ADEC_INPUT_MIX_OUT;
				encParam.aencCodecType 	   = KDRV_ADEC_CODEC_DDCO;
				encParam.channelNumber	   = 2;
				encParam.bitPerSample	   = 16;
				encParam.samplingFrequency = 480;
				encParam.bitRate 		   = 224000;
			//	encParam.bitRate = stEncParam.duration;

				//Set a encoder parameters
				ADEC_IPC_SetConfig(KDRV_ADEC_ENC, &encParam);

				ADEC_IPC_Start(KDRV_ADEC_ENC);
			}

			//Restart mix0 after auto dsp reset for GCD platform
			if(g_adec_kdrv.bMixStarted[LX_ADEC_MIXER_0] == TRUE)
			{
				ADEC_IPC_GetConfig(KDRV_ADEC_MP_02, &mixStartParam);
				ADEC_IPC_SetConfig(KDRV_ADEC_MP_02, &mixStartParam);

				ADEC_IPC_Start(KDRV_ADEC_MP_02);
			}
		}
	}

	ADEC_DEBUG_TMP("ADEC_L9_LoadDSP1Codec\n");
	return RET_OK;
}

/**
 * Load a DSP0 codec firmware.
 * @see
*/
int ADEC_L9_LoadDSP0Codec ( LX_ADEC_SRC_TYPE_T i_eSrcType )
{
	UINT32		ui32CodecSize;
	UINT32		*pui32Codec;
	UINT8		codecName[ADEC_CODEC_NAME_SIZE];	//30

	UINT64		startTick = 0;

	//Get a current tick time
	startTick = jiffies;

	//Check a fw memory
	if(NULL == g_pui32DSP0CodecVirAddr)
	{
		ADEC_ERROR("ADEC_LoadCodec : Err - g_pui32DSP0CodecVirAddr NULL!\n");
		return RET_ERROR;
	}

	/* Mapping codec from ddi to DSP0 codec */
	switch (i_eSrcType)
	{
		case LX_ADEC_SRC_TYPE_UNKNOWN:
			ADEC_ERROR("Audio Codec is unknown!!!(%d)\n", i_eSrcType);
			return RET_OK;
			break;

		case LX_ADEC_SRC_TYPE_PCM:
		case LX_ADEC_SRC_TYPE_SIF:
		case LX_ADEC_SRC_TYPE_SIF_BTSC:
		case LX_ADEC_SRC_TYPE_SIF_A2:
		case LX_ADEC_SRC_TYPE_AC3:
		case LX_ADEC_SRC_TYPE_EAC3:
		case LX_ADEC_SRC_TYPE_MPEG:
		case LX_ADEC_SRC_TYPE_MP3:
		case LX_ADEC_SRC_TYPE_AAC:
		case LX_ADEC_SRC_TYPE_HEAAC:
		case LX_ADEC_SRC_TYPE_DTS:
		case LX_ADEC_SRC_TYPE_WMA_PRO:
		case LX_ADEC_SRC_TYPE_VORBIS:
		case LX_ADEC_SRC_TYPE_AMR_NB:
		case LX_ADEC_SRC_TYPE_AAC_LC_ENC:
		case LX_ADEC_SRC_TYPE_MS10_DDCO:
		case LX_ADEC_SRC_TYPE_RA10:
			i_eSrcType = LX_ADEC_SRC_TYPE_MULTI;
			break;

		case LX_ADEC_SRC_TYPE_DRA:			//not implemented
		case LX_ADEC_SRC_TYPE_ADPCM:		//not implemented
		case LX_ADEC_SRC_TYPE_AMR_WB:		//not implemented
		case LX_ADEC_SRC_TYPE_AAC_LC:		//not implemented
			break;

		case LX_ADEC_SRC_TYPE_DEFAULT:
		case LX_ADEC_SRC_TYPE_NONE:
		default:
			break;
	}

	//Check a current loaded codec.
	if(g_bLoadingCodecForDebug == FALSE)
	{
		if(g_eCurLoadedCodec == i_eSrcType)
		{
			ADEC_PRINT("ADEC_LoadCodec : Codec already loaded\n");
			return RET_OK;
		}
	}

	//Reset Low : ADEC DSP0
	ADEC_L9_ClearReset(ADEC_OPTION_DSP0);

	//Clear aud_dsp0_state reg.
	AUD_RdFL(aud_dsp0_state);
	AUD_Wr02(aud_dsp0_state, codec_ver, 0x0, codec_type, 0x0);
	AUD_WrFL(aud_dsp0_state);

	//Set audio codec firmware image
	if (i_eSrcType == LX_ADEC_SRC_TYPE_MULTI)
	{
		ui32CodecSize = sizeof(multi_codec_l9);
		pui32Codec	  = (UINT32 *)multi_codec_l9;
		strcpy(codecName, "multi_codec_l9");
	}
	else
	{
		ADEC_ERROR("ADEC_LoadCodec : Err - Codec Not Defined!(0x%X)\n", i_eSrcType);
		return RET_INVALID_PARAMS;
	}

	ADEC_DEBUG ( "ADEC_LoadCodec : Started... srcType(%d), Codec(%s), Size(%d)\n",i_eSrcType, codecName, ui32CodecSize);

	if(ui32CodecSize > pstMemCfgADEC->fw0_memory_size)
	{
		ADEC_ERROR("ADEC_LoadCodec : %s size is bigber than memory.(%d > %d)\n",	\
					codecName, ui32CodecSize, pstMemCfgADEC->fw0_memory_size);
		return RET_ERROR;
	}

	//Copy codec fw from memory to dsp0 memory
	if(g_bCopyingCodecForDebug == TRUE)
	{
		memcpy(g_pui32DSP0CodecVirAddr, pui32Codec, ui32CodecSize);
		ADEC_PRINT ( "ADEC_LoadCodec : Done(%s)!!!\n", codecName );
	}

	ADEC_DEBUG_TMP ( "ADEC_LoadCodec : Done(%s), prev type = 0x%X!!!\n", codecName, g_adec_kdrv.adecType[LX_ADEC_DEC_0] );

	//Set DSP swreset register
	ADEC_L9_SetReset(ADEC_OPTION_DSP0);

	//Wait for DSP fw download completion to DSP memory and DSP ready
	ADEC_L9_WaitForDSP0Ready(startTick);

	//Update the current laoded codec variable
	g_eCurLoadedCodec = i_eSrcType;

	ADEC_PRINT ("ADEC_L9_LoadDSP0Codec is called!!!\n");
	return RET_OK;
}

/**
 * Set a reset for ADEC block.
 * @see
*/
int ADEC_L9_SetReset ( UINT8 ui8ResetFlag )
{
	AUD_SWRESET		stSWReset = { 0 };

	memcpy ( &stSWReset, (UINT32*)&gpRealRegAUD->aud_swreset, sizeof (UINT32));

	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		//Clear interrupt status
		AUD_Wr(aud_int_state0, 0x0);
		AUD_WrFL(aud_int_state0);

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
		AUD_Wr(aud_int_state1, 0x0);
		AUD_WrFL(aud_int_state1);

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
		stSWReset.aadapbresetout 	= 1;		//15 	aadapbresetout     	reset for AAD register interface
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		stSWReset.fs21resetout 		= 1;		//5		fs21resetout       	reset for audio SPDIF output interface
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
		stSWReset.fs23resetout 		= 1;		//7	 	fs23resetout       	reset for audio HWSRC data input
		stSWReset.fs24resetout 		= 1;		//8	 	fs24resetout       	reset for audio HWSRC datat output
		stSWReset.asrcrstout 		= 1;		//9	 	asrcrstout         	reset for audio HWSRC operation

		stSWReset.aadresetout 		= 1;		//14 	aadresetout        	reest for operation of Analog audio decoder
		stSWReset.aadapbresetout 	= 1;		//15 	aadapbresetout     	reset for AAD register interface
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
	}
	else if (ui8ResetFlag == ADEC_OPTION_HWSRC)
	{
		stSWReset.fs23resetout 		= 1;		//7	 	fs23resetout       	reset for audio HWSRC data input
		stSWReset.fs24resetout 		= 1;		//8	 	fs24resetout       	reset for audio HWSRC datat output
		stSWReset.asrcrstout 		= 1;		//9	 	asrcrstout         	reset for audio HWSRC operation
	}

	memcpy ((UINT32*) &gpRealRegAUD->aud_swreset, &stSWReset, sizeof (UINT32));
	ADEC_PRINT ("Reset reg val [0x%x]\n", *(UINT32*)&gpRealRegAUD->aud_swreset );

	//Set delay for reset high duration for stable operation
	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_DSP0!!!\n");

		//Enable audio interrupt 0
		AUD_Wr(aud_int0_en, 0x1);
		AUD_WrFL(aud_int0_en);
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_DSP1!!!\n");

		//Enable audio interrupt 1
		AUD_Wr(aud_int1_en, 0x1);
		AUD_WrFL(aud_int1_en);
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP_DEBUG)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_DSP_DEBUG!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_AAD)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_AAD!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_SPDIF!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_OTHERS)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_OTHERS!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_ALL)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_ALL!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_HWSRC)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_SetReset : ADEC_OPTION_HWSRC!!!\n");
	}

	return RET_OK;
}

/**
 * Clear a reset for ADEC block.
 * @see
*/
int ADEC_L9_ClearReset ( UINT8 ui8ResetFlag )
{
	AUD_SWRESET		stSWReset = { 0 };
	UINT32			flushDoneStatus = 1, timeout = 0;

	memcpy (&stSWReset, (UINT32 *)&gpRealRegAUD->aud_swreset, sizeof(UINT32));

	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		//Disable audio interrupt DSP0
		AUD_Wr(aud_int0_en, 0x0);
		AUD_WrFL(aud_int0_en);

		stSWReset.dsp0ocdresetout 	= 0;        //10 dsp0ocdresetout    	reset for On chip debugger of DSP0
		stSWReset.dsp0bresetout 	= 0;        //12 dsp0bresetout      	reset for DSP0
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		//Disable audio interrupt DSP1
		AUD_Wr(aud_int1_en, 0x0);
		AUD_WrFL(aud_int1_en);

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
		stSWReset.aadapbresetout 	= 0;		//15 aadapbresetout     	reset for AAD register interface
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		stSWReset.fs21resetout 		= 0;		//5	 fs21resetout      		reset for audio SPDIF output interface
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
	}
	else if (ui8ResetFlag == ADEC_OPTION_ALL)
	{
		stSWReset.aresetout 		= 0;		//0 	aresetout			reset for operation of peripherals
		stSWReset.apbresetout		= 0;		//1 	apbresetout 		reset for register interface if this set, all registers are cleared
		stSWReset.aversetout		= 0;		//2 	aversetout			reset for TE data input interface,
		stSWReset.veresetout		= 0;		//3 	veresetout			reset for TE data output interface (encoder)
		stSWReset.fs20resetout		= 0;		//4 	fs20resetout		reset for audio PCM output interface
		stSWReset.fs21resetout		= 0;		//5 	fs21resetout		reset for audio SPDIF output interface
												//6 						reserved
		stSWReset.fs23resetout		= 0;		//7 	fs23resetout		reset for audio HWSRC data input
		stSWReset.fs24resetout		= 0;		//8 	fs24resetout		reset for audio HWSRC datat output
		stSWReset.asrcrstout		= 0;		//9 	asrcrstout			reset for audio HWSRC operation

		stSWReset.dsp0ocdresetout 	= 0;        //10	dsp0ocdresetout    	reset for On chip debugger of DSP0
		stSWReset.dsp1ocdresetout	= 0;        //11	dsp1ocdresetout    	reset for On chip debugger of DSP1
		stSWReset.dsp0bresetout 	= 0;        //12	dsp0bresetout      	reset for DSP0
		stSWReset.dsp1bresetout 	= 0;        //13	dsp1bresetout      	reset for DSP1

		stSWReset.aadresetout		= 0;		//14	aadresetout 		reest for operation of Analog audio decoder
		stSWReset.aadapbresetout	= 0;		//15	aadapbresetout		reset for AAD register interface
	}
	else if (ui8ResetFlag == ADEC_OPTION_HWSRC)
	{
		stSWReset.fs23resetout 		= 0;		//7	 fs23resetout       	reset for audio HWSRC data input
		stSWReset.fs24resetout 		= 0;		//8	 fs24resetout       	reset for audio HWSRC datat output
		stSWReset.asrcrstout 		= 0;		//9	 asrcrstout         	reset for audio HWSRC operation
	}

	//Flush ADEC DSP memory access
	if (g_ui32ResetDsp0State == 1 && ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		g_ui32ResetDsp0State = 0;

		//Stop ADEC DSP 0
		AUD_Wr(aud_runstall0, 0x1);
		AUD_WrFL(aud_runstall0);

		//Set flush request register for ADEC DSP0
		CTOP_CTRL_L9_RdFL(flush_req);
		CTOP_CTRL_L9_Wr01(flush_req, aud_dsp0_flush_en, 0x1);
		CTOP_CTRL_L9_WrFL(flush_req);

		//Check flush done for DSP0
		for(timeout = 0; timeout < ADEC_DSP_FLUSH_ITERATION; timeout++)
		{
			udelay(1);

			CTOP_CTRL_L9_RdFL(flush_done_status);
			CTOP_CTRL_L9_Rd01(flush_done_status, aud_dsp0_flush_done, flushDoneStatus);

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
		AUD_Wr(aud_runstall1, 0x1);
		AUD_WrFL(aud_runstall1);

		//Set flush request register for ADEC DSP1
		CTOP_CTRL_L9_RdFL(flush_req);
		CTOP_CTRL_L9_Wr01(flush_req, aud_dsp1_flush_en, 0x1);
		CTOP_CTRL_L9_WrFL(flush_req);

		//Check flush done for DSP1
		for(timeout = 0; timeout < ADEC_DSP_FLUSH_ITERATION; timeout++)
		{
			udelay(1);

			CTOP_CTRL_L9_RdFL(flush_done_status);
			CTOP_CTRL_L9_Rd01(flush_done_status, aud_dsp1_flush_done, flushDoneStatus);

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

	memcpy ((UINT32*) &gpRealRegAUD->aud_swreset, &stSWReset, sizeof (UINT32));
	ADEC_PRINT ("Reset reg val [0x%x]\n", *(UINT32*)&gpRealRegAUD->aud_swreset );

	//Set delay for reset low duration
	if (ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_DSP0!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_DSP1!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_DSP_DEBUG)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_DSP_DEBUG!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_AAD)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_AAD!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_SPDIF)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_SPDIF!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_OTHERS)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_OTHERS!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_ALL)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_ALL!!!\n");
	}
	else if (ui8ResetFlag == ADEC_OPTION_HWSRC)
	{
		udelay(ADEC_RESET_DELAY);	//IC => 50 clk : 125ns
		ADEC_PRINT ("ADEC_L9_ClearReset : ADEC_OPTION_HWSRC!!!\n");
	}

	//Reset Flush Bit for ADEC DSP memory access
	if (g_ui32ResetDsp0State == 0 && ui8ResetFlag == ADEC_OPTION_DSP0)
	{
		//Clear flush request register for ADEC DSP0
		CTOP_CTRL_L9_RdFL(flush_req);
		CTOP_CTRL_L9_Wr01(flush_req, aud_dsp0_flush_en, 0x0);
		CTOP_CTRL_L9_WrFL(flush_req);

		ADEC_PRINT ("Reset ADEC DSP0 Memory Flush!!!\n");

		//Start ADEC DSP 0
		gpRealRegAUD->aud_runstall0.aud_runstall0 = 0x0;
	}
	else if (g_ui32ResetDsp1State == 0 && ui8ResetFlag == ADEC_OPTION_DSP1)
	{
		//Clear flush request register for ADEC DSP1
		CTOP_CTRL_L9_RdFL(flush_req);
		CTOP_CTRL_L9_Wr01(flush_req, aud_dsp1_flush_en, 0x0);
		CTOP_CTRL_L9_WrFL(flush_req);
		ADEC_PRINT ("Reset ADEC DSP1 Memory Flush!!!\n");

		//Start ADEC DSP 1
		gpRealRegAUD->aud_runstall1.aud_runstall1 = 0x0;
	}

	return RET_OK;
}

/**
 * Initialize register
 * @see ADEC_DRV_InitModule
*/
int ADEC_L9_InitReg ( void )
{
 	UINT64		start = 0, elapsed = 0;

	LX_ADDR_SW_CFG_T			addrSwCfg;

	//Get a current mili-second time
	start = jiffies;

	//Check a L9 chip revision for SPDIF Buffer Setting in MMR and SCART Bypass Port.
	AUD_RdFL(aud_set_inout);

	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		AUD_Wr01(aud_set_inout, chip_type, 0xB0);
	}
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{
		AUD_Wr01(aud_set_inout, chip_type, 0xA1);
	}
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
	{
		AUD_Wr01(aud_set_inout, chip_type, 0xA0);
	}
	else
	{
		AUD_Wr01(aud_set_inout, chip_type, 0xB0);
	}

	AUD_WrFL(aud_set_inout);

	//Set download address to ADEC DSP memory
	//set address for DSP0
	AUD_Wr(aud_dsp0offset5, pstMemCfgADEC->fw0_memory_base);
	AUD_WrFL(aud_dsp0offset5);

	AUD_Wr(aud_dsp0offset6, pstMemCfgADEC->dsp0_memory_base);
	AUD_WrFL(aud_dsp0offset6);

	//Share area for DSP0
	AUD_Wr(aud_dsp0offset61, pstMemCfgADEC->dpbm_memory_base);
	AUD_WrFL(aud_dsp0offset61);

	//set address for DSP1
	AUD_Wr(aud_dsp1offset5, pstMemCfgADEC->fw1_memory_base);
	AUD_WrFL(aud_dsp1offset5);

	AUD_Wr(aud_dsp1offset6, pstMemCfgADEC->dsp1_memory_base);
	AUD_WrFL(aud_dsp1offset6);

	//Share area for DSP1
	AUD_Wr(aud_dsp1offset61, pstMemCfgADEC->dpbm_memory_base);
	AUD_WrFL(aud_dsp1offset61);

	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp0offset5[0x%08X]\n", AUD_Rd(aud_dsp0offset5));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp0offset6[0x%08X]\n", AUD_Rd(aud_dsp0offset6));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp0offset6[0x%08X]\n", AUD_Rd(aud_dsp0offset61));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp1offset5[0x%08X]\n", AUD_Rd(aud_dsp1offset5));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp1offset6[0x%08X]\n", AUD_Rd(aud_dsp1offset6));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - aud_dsp1offset6[0x%08X]\n", AUD_Rd(aud_dsp1offset61));

	//Set ADEC memory table setting(0x02c0 ~ 0x02d4)
	/* initialize L9 memory address switch */
	//address switch setting ADEC0
	(void)BASE_L9_GetAddrSwCfg( ADDR_SW_CFG_AUD0, &addrSwCfg );

	AUD_Wr(memtab0, addrSwCfg.de_sav);
	AUD_WrFL(memtab0);

	AUD_Wr(memtab1, addrSwCfg.cpu_gpu);
	AUD_WrFL(memtab1);

	AUD_Wr(memtab2, addrSwCfg.cpu_shadow);
	AUD_WrFL(memtab2);

	//address switch setting ADEC1
	(void)BASE_L9_GetAddrSwCfg( ADDR_SW_CFG_AUD1, &addrSwCfg );

	AUD_Wr(memtab3, addrSwCfg.de_sav);
	AUD_WrFL(memtab3);

	AUD_Wr(memtab4, addrSwCfg.cpu_gpu);
	AUD_WrFL(memtab4);

	AUD_Wr(memtab5, addrSwCfg.cpu_shadow);
	AUD_WrFL(memtab5);

	ADEC_DEBUG_TMP ( "ADEC_RegInit - memtab0[0x%08X]\n", AUD_Rd(memtab0));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - memtab1[0x%08X]\n", AUD_Rd(memtab1));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - memtab2[0x%08X]\n", AUD_Rd(memtab2));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - memtab3[0x%08X]\n", AUD_Rd(memtab3));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - memtab4[0x%08X]\n", AUD_Rd(memtab4));
	ADEC_DEBUG_TMP ( "ADEC_RegInit - memtab5[0x%08X]\n", AUD_Rd(memtab5));

	//Set base address and size
	AUD_Wr(aud_mem_share_base, pstMemCfgADEC->dpbm_memory_base);
	AUD_WrFL(aud_mem_share_base);

	AUD_Wr(aud_mem_dsp0_base, pstMemCfgADEC->dsp0_memory_base);
	AUD_WrFL(aud_mem_dsp0_base);

	AUD_Wr(aud_mem_dsp1_base, pstMemCfgADEC->dsp1_memory_base);
	AUD_WrFL(aud_mem_dsp1_base);


	AUD_Wr(aud_mem_cpbm_addr, pstMemCfgADEC->cpbm_memory_base);
	AUD_WrFL(aud_mem_cpbm_addr);

	AUD_Wr(aud_mem_cpbm_size, pstMemCfgADEC->cpbm_memory_size);
	AUD_WrFL(aud_mem_cpbm_size);

	AUD_Wr(aud_mem_cpba_addr, pstMemCfgADEC->cpba_memory_base);
	AUD_WrFL(aud_mem_cpba_addr);

	AUD_Wr(aud_mem_cpba_size, pstMemCfgADEC->cpba_memory_size);
	AUD_WrFL(aud_mem_cpba_size);

	AUD_Wr(aud_mem_enc_intra_addr, pstMemCfgADEC->enc_intra_memory_base);
	AUD_WrFL(aud_mem_enc_intra_addr);

	AUD_Wr(aud_mem_enc_intra_size, pstMemCfgADEC->enc_intra_memory_size);
	AUD_WrFL(aud_mem_enc_intra_size);

	AUD_Wr(aud_mem_mpb0_addr, pstMemCfgADEC->mpb0_memory_base);
	AUD_WrFL(aud_mem_mpb0_addr);

	AUD_Wr(aud_mem_mpb0_size, pstMemCfgADEC->mpb0_memory_size);
	AUD_WrFL(aud_mem_mpb0_size);

	//Share area for CPU, DSP0 /1
	AUD_Wr(aud_mem_dpbm_addr, pstMemCfgADEC->dpbm_memory_base);
	AUD_WrFL(aud_mem_dpbm_addr);

	AUD_Wr(aud_mem_dpbm_size, pstMemCfgADEC->dpbm_memory_size);
	AUD_WrFL(aud_mem_dpbm_size);

	AUD_Wr(aud_mem_dpba_addr, pstMemCfgADEC->dpba_memory_base);
	AUD_WrFL(aud_mem_dpba_addr);

	AUD_Wr(aud_mem_dpba_size, pstMemCfgADEC->dpba_memory_size);
	AUD_WrFL(aud_mem_dpba_size);

	AUD_Wr(aud_mem_iec_addr, pstMemCfgADEC->iec_memory_base);
	AUD_WrFL(aud_mem_iec_addr);

	AUD_Wr(aud_mem_iec_size, pstMemCfgADEC->iec_memory_size);
	AUD_WrFL(aud_mem_iec_size);

	AUD_Wr(aud_mem_ddco_addr, pstMemCfgADEC->ddco_memory_base);
	AUD_WrFL(aud_mem_ddco_addr);

	AUD_Wr(aud_mem_ddco_size, pstMemCfgADEC->ddco_memory_size);
	AUD_WrFL(aud_mem_ddco_size);

	AUD_Wr(aud_mem_enc_addr, pstMemCfgADEC->enc_memory_base);
	AUD_WrFL(aud_mem_enc_addr);

	AUD_Wr(aud_mem_enc_size, pstMemCfgADEC->enc_memory_size);
	AUD_WrFL(aud_mem_enc_size);

	//Debug
	AUD_Wr(aud_mem_buf_stat0_addr, pstMemCfgADEC->debug_memory_base);
	AUD_WrFL(aud_mem_buf_stat0_addr);

	AUD_Wr(aud_mem_buf_stat1_addr, pstMemCfgADEC->debug_memory_base + 0x1000);
	AUD_WrFL(aud_mem_buf_stat1_addr);

	AUD_Wr(aud_mem_profile_addr, pstMemCfgADEC->debug_memory_base + 0x2000);
	AUD_WrFL(aud_mem_profile_addr);

	//Sound Engine parameters
	AUD_Wr(aud_mem_se_param_addr, pstMemCfgADEC->se_memory_base);
	AUD_WrFL(aud_mem_se_param_addr);

	//IPC
	AUD_Wr(aud_mem_ipc_addr, pstMemCfgADEC->ipc_memory_base);
	AUD_WrFL(aud_mem_ipc_addr);

	AUD_Wr(aud_mem_ipc_size, pstMemCfgADEC->ipc_memory_size);
	AUD_WrFL(aud_mem_ipc_size);

	//MPB 1 ~ 3 : L9 B0 support
	AUD_Wr(aud_mem_mpb1_addr, pstMemCfgADEC->mpb1_memory_base);
	AUD_WrFL(aud_mem_mpb1_addr);

	AUD_Wr(aud_mem_mpb1_size, pstMemCfgADEC->mpb1_memory_size);
	AUD_WrFL(aud_mem_mpb1_size);

	AUD_Wr(aud_mem_mpb2_addr, pstMemCfgADEC->mpb2_memory_base);
	AUD_WrFL(aud_mem_mpb2_addr);

	AUD_Wr(aud_mem_mpb2_size, pstMemCfgADEC->mpb2_memory_size);
	AUD_WrFL(aud_mem_mpb2_size);

	AUD_Wr(aud_mem_mpb3_addr, pstMemCfgADEC->mpb3_memory_base);
	AUD_WrFL(aud_mem_mpb3_addr);

	AUD_Wr(aud_mem_mpb3_size, pstMemCfgADEC->mpb3_memory_size);
	AUD_WrFL(aud_mem_mpb3_size);

	//LLB : L9 B0 GLD support
	AUD_Wr(aud_mem_llb_addr, pstMemCfgADEC->llb_memory_base);
	AUD_WrFL(aud_mem_llb_addr);

	AUD_Wr(aud_mem_llb_size, pstMemCfgADEC->llb_memory_size);
	AUD_WrFL(aud_mem_llb_size);

	//ACR : L9 B0 GLD support
	AUD_Wr(aud_mem_acr_addr, pstMemCfgADEC->acr_memory_base);
	AUD_WrFL(aud_mem_acr_addr);

	AUD_Wr(aud_mem_acr_size, pstMemCfgADEC->acr_memory_size);
	AUD_WrFL(aud_mem_acr_size);

	//Set interrupt mask
	AUD_RdFL(aud_intmask);

	//un-mask interrupt for DSP0
	AUD_Wr01(aud_intmask, int_err_mask0,  0x1);
	AUD_Wr01(aud_intmask, int_info_mask0, 0x1);
	AUD_Wr01(aud_intmask, int_lip_mask0,  0x1);
	AUD_Wr01(aud_intmask, int_frm_mask0,  0x1);
	AUD_Wr01(aud_intmask, int_dec_event0, 0x1);

	//un-mask interrupt for DSP1
	AUD_Wr01(aud_intmask, int_err_mask1,  0x1);
	AUD_Wr01(aud_intmask, int_info_mask1, 0x1);
	AUD_Wr01(aud_intmask, int_lip_mask1,  0x1);
	AUD_Wr01(aud_intmask, int_frm_mask1,  0x1);
	AUD_Wr01(aud_intmask, int_dec_event1, 0x1);

	AUD_WrFL(aud_intmask);

	//Set I2S and 24 bit mode for I2S input for default
	AUD_RdFL(aud_set_inout);

	AUD_Wr01(aud_set_inout, in_format, 0x0);	//I2S
	AUD_Wr01(aud_set_inout, in_res,    0x3);	//24 bit

	//Set I2S and 24 bit mode for I2S output
	AUD_Wr01(aud_set_inout, pcm_bit,    0x3);	//24 bit
	AUD_Wr01(aud_set_inout, i2s_format, 0x0);	//I2S

	//Set SPDIF output mode : Normal PCM Mixed Output
	AUD_Wr01(aud_set_inout, spdif_mode, 0x0);	//Normal

	AUD_Wr01(aud_set_inout, check_bit,  0x1);	//check_bit

	AUD_WrFL(aud_set_inout);

	// EMP Lipsync Initialization of Clock Basetime & Stream Base
	// Init DEC0 Clock Basetime
	AUD_Wr(aud_dec0_cbt, 0xFFFFFFFF);
	AUD_WrFL(aud_dec0_cbt);

	// Init DEC0 Stream Basetime
	AUD_Wr(aud_dec0_sbt, 0xFFFFFFFF);
	AUD_WrFL(aud_dec0_sbt);

	// Init DEC1 Clock Basetime
	AUD_Wr(aud_dec1_cbt, 0xFFFFFFFF);
	AUD_WrFL(aud_dec1_cbt);

	// Init DEC1 Stream Basetime
	AUD_Wr(aud_dec1_sbt, 0xFFFFFFFF);
	AUD_WrFL(aud_dec1_sbt);

	//Get a elapsed nono-second time
	elapsed = jiffies - start;
	ADEC_DEBUG_TMP("ADEC_RegInit in %d ms.\n", (UINT32)elapsed);

 	return RET_OK;
}

/**
 * Initialize register for check bit
 * @see ADEC_DRV_InitModule
*/
int ADEC_L9_InitRegForCheckbit ( void )
{
	//CPB0
	AUD_Wr(aud_cpb0_wo, 0xA0000000);
	AUD_WrFL(aud_cpb0_wo);

	AUD_Wr(aud_cpb0_ro, 0xA0000000);
	AUD_WrFL(aud_cpb0_ro);

	AUD_Wr(aud_cpb0_wi, 0xA0000000);
	AUD_WrFL(aud_cpb0_wi);

	AUD_Wr(aud_cpb0_ri, 0xA0000000);
	AUD_WrFL(aud_cpb0_ri);

	//CPB1
	AUD_Wr(aud_cpb1_wo, 0xA0000000);
	AUD_WrFL(aud_cpb1_wo);

	AUD_Wr(aud_cpb1_ro, 0xA0000000);
	AUD_WrFL(aud_cpb1_ro);

	AUD_Wr(aud_cpb1_wi, 0xA0000000);
	AUD_WrFL(aud_cpb1_wi);

	AUD_Wr(aud_cpb1_ri, 0xA0000000);
	AUD_WrFL(aud_cpb1_ri);

	//MPB0
	AUD_Wr(aud_mpb0_wo, 0xA0000000);
	AUD_WrFL(aud_mpb0_wo);

	AUD_Wr(aud_mpb0_ro, 0xA0000000);
	AUD_WrFL(aud_mpb0_ro);

	AUD_Wr(aud_mpb0_wi, 0xA0000000);
	AUD_WrFL(aud_mpb0_wi);

	AUD_Wr(aud_mpb0_ri, 0xA0000000);
	AUD_WrFL(aud_mpb0_ri);

	//MPB1
	AUD_Wr(aud_mpb1_wo, 0xA0000000);
	AUD_WrFL(aud_mpb1_wo);

	AUD_Wr(aud_mpb1_ro, 0xA0000000);
	AUD_WrFL(aud_mpb1_ro);

	AUD_Wr(aud_mpb1_wi, 0xA0000000);
	AUD_WrFL(aud_mpb1_wi);

	AUD_Wr(aud_mpb1_ri, 0xA0000000);
	AUD_WrFL(aud_mpb1_ri);

	//MPB2
	AUD_Wr(aud_mpb2_wo, 0xA0000000);
	AUD_WrFL(aud_mpb2_wo);

	AUD_Wr(aud_mpb2_ro, 0xA0000000);
	AUD_WrFL(aud_mpb2_ro);

	AUD_Wr(aud_mpb2_wi, 0xA0000000);
	AUD_WrFL(aud_mpb2_wi);

	AUD_Wr(aud_mpb2_ri, 0xA0000000);
	AUD_WrFL(aud_mpb2_ri);

	//MPB3
	AUD_Wr(aud_mpb3_wo, 0xA0000000);
	AUD_WrFL(aud_mpb3_wo);

	AUD_Wr(aud_mpb3_ro, 0xA0000000);
	AUD_WrFL(aud_mpb3_ro);

	AUD_Wr(aud_mpb3_wi, 0xA0000000);
	AUD_WrFL(aud_mpb3_wi);

	AUD_Wr(aud_mpb3_ri, 0xA0000000);
	AUD_WrFL(aud_mpb3_ri);

	//DPB0
	AUD_Wr(aud_dpb0_wo, 0xA0000000);
	AUD_WrFL(aud_dpb0_wo);

	AUD_Wr(aud_dpb0_ro, 0xA0000000);
	AUD_WrFL(aud_dpb0_ro);

	AUD_Wr(aud_dpb0_wi, 0xA0000000);
	AUD_WrFL(aud_dpb0_wi);

	AUD_Wr(aud_dpb0_ri, 0xA0000000);
	AUD_WrFL(aud_dpb0_ri);

	//DPB1
	AUD_Wr(aud_dpb1_wo, 0xA0000000);
	AUD_WrFL(aud_dpb1_wo);

	AUD_Wr(aud_dpb1_ro, 0xA0000000);
	AUD_WrFL(aud_dpb1_ro);

	AUD_Wr(aud_dpb1_wi, 0xA0000000);
	AUD_WrFL(aud_dpb1_wi);

	AUD_Wr(aud_dpb1_ri, 0xA0000000);
	AUD_WrFL(aud_dpb1_ri);

	//IEC(SPDIF)
	AUD_Wr(aud_iecbit_wo, 0xA0000000);
	AUD_WrFL(aud_iecbit_wo);

	AUD_Wr(aud_iecbit_ro, 0xA0000000);
	AUD_WrFL(aud_iecbit_ro);

	AUD_Wr(aud_iecbit_wi, 0xA0000000);
	AUD_WrFL(aud_iecbit_wi);

	AUD_Wr(aud_iecbit_ri, 0xA0000000);
	AUD_WrFL(aud_iecbit_ri);

	//DDCO(SPDIF)
	AUD_Wr(aud_ddco_wo, 0xA0000000);
	AUD_WrFL(aud_ddco_wo);

	AUD_Wr(aud_ddco_ro, 0xA0000000);
	AUD_WrFL(aud_ddco_ro);

	AUD_Wr(aud_ddco_wi, 0xA0000000);
	AUD_WrFL(aud_ddco_wi);

	AUD_Wr(aud_ddco_wi, 0xA0000000);
	AUD_WrFL(aud_ddco_wi);

	//ENC
	AUD_Wr(aud_enc_wo, 0xA0000000);
	AUD_WrFL(aud_enc_wo);

	AUD_Wr(aud_enc_wo, 0xA0000000);
	AUD_WrFL(aud_enc_wo);

	AUD_Wr(aud_enc_wi, 0xA0000000);
	AUD_WrFL(aud_enc_wi);

	AUD_Wr(aud_enc_ri, 0xA0000000);
	AUD_WrFL(aud_enc_ri);

	//ETC
	AUD_Wr(aud_sai_gain, 0xA0800000);	//0 dB SAI Input volume gain
	AUD_WrFL(aud_sai_gain);

	AUD_Wr(aud_dsp0_state, 0xA0000000);
	AUD_WrFL(aud_dsp0_state);

	AUD_Wr(aud_dsp1_state, 0xA0000000);
	AUD_WrFL(aud_dsp1_state);

	ADEC_PRINT ( "ADEC_InitRegForCheckbit\n");
	return RET_OK;
}

/**
 * Wait for DSP0 Ready!.
 * @see
*/
void ADEC_L9_WaitForDSP0Ready ( UINT64 startTick )
{
	UINT8	codecType;
	UINT16	codecVer;
	UINT32	elapsed;

	UINT8 	*codecName[20] = {"Unknown ", "PCM     ", "MS10_DDC", "MS10_DDT", "MPEG1, 2",	\
							  "MP3     ", "WMA     ", "DTS     ", "VORBIS  ", "G.729   ",	\
							  "G.711   ", "DTS_BY  ", "PCM_WAV ", "WMAPRO  ", "ADPCM   ",	\
							  "MULTI   ", "AMR_WB  ", "AMR_NB  ", "VORBIS  ", "NONE2   "};

	ADEC_PRINT("Wait %d ms for DSP0 to Ready!!!\n", WAIT_DSP0_READY_TIMEOUT);
	wait_event_interruptible_timeout(ADEC_wait_dsp0_ready, ADEC_dsp0_ready_waitq_flag != 0, msecs_to_jiffies(WAIT_DSP0_READY_TIMEOUT));

	//Get codec type and version
	AUD_RdFL(aud_dsp0_state);
	AUD_Rd01(aud_dsp0_state, codec_type, codecType);
	AUD_Rd01(aud_dsp0_state, codec_ver,  codecVer);

	//sanity check for debug print
	if( codecType >= 19 )
		codecType = 19;	//Unknown

	//Get a elapsed tick
	elapsed = (UINT32)(jiffies - startTick);

	if(ADEC_dsp0_ready_waitq_flag)
		ADEC_ERROR("DSP0 to Ready : %s Codec Ver. = 0x%X(type = %d(%d))\n", codecName[codecType], codecVer, g_adec_kdrv.adecType[LX_ADEC_DEC_0], elapsed);
	else
		ADEC_ERROR("DSP0 to Not Ready : %s Codec Ver. = 0x%X(type = %d(%d))\n", codecName[codecType], codecVer, g_adec_kdrv.adecType[LX_ADEC_DEC_0], elapsed);

	return;
}

/**
 * Wait for DSP1 Ready!.
 * @see
*/
void ADEC_L9_WaitForDSP1Ready ( UINT64 startTick )
{
	UINT8	spdifMain;
	UINT16	codecVer;
	UINT32	elapsed;

	ADEC_PRINT("Wait %d ms for DSP1 to Ready!!!\n", WAIT_DSP1_READY_TIMEOUT);
	wait_event_interruptible_timeout(ADEC_wait_dsp1_ready, ADEC_dsp1_ready_waitq_flag != 0, msecs_to_jiffies(WAIT_DSP1_READY_TIMEOUT));

	//Get codec version
	AUD_RdFL(aud_dsp1_state);
	AUD_Rd01(aud_dsp1_state, codec_ver, codecVer);

	//Get a SPDIF output mode
	AUD_RdFL(aud_set_inout);
	AUD_Rd01(aud_set_inout, spdif_mode, spdifMain);

	//Get a elapsed tick
	elapsed = (UINT32)(jiffies - startTick);

	if(ADEC_dsp1_ready_waitq_flag)
		ADEC_ERROR("DSP1 to Ready : Present  Codec Ver. = 0x%X(%d), spdifMain = %d\n", codecVer, elapsed, spdifMain);
	else
		ADEC_ERROR("DSP1 to Not Ready : Present  Codec Ver. = 0x%X(%d), spdifMain = %d\n", codecVer, elapsed, spdifMain);

	return;
}

/**
 * Signal as DSP0 is ready.
 * @see
*/
void ADEC_L9_SignalDSP0Ready ( void )
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
void ADEC_L9_SignalDSP1Ready ( void )
{
	ADEC_PRINT("ADEC_SignalDSP1Ready\n");

	ADEC_dsp1_ready_waitq_flag = 1;
	wake_up_interruptible(&ADEC_wait_dsp1_ready);

	return;
}

/**
 * Set IPC interrupt to DSP 0 ,1
 * @see
*/
void ADEC_L9_SetDSP1IntrEvent(void)
{
	AUD_RdFL(aud_dirq0);
	AUD_Wr01(aud_dirq0, aud_dirq0, 0x1);
	AUD_WrFL(aud_dirq0);

	AUD_RdFL(aud_dirq1);
	AUD_Wr01(aud_dirq1, aud_dirq1, 0x1);
	AUD_WrFL(aud_dirq1);


	AUD_RdFL(aud_dirq0);
	AUD_Wr01(aud_dirq0, aud_dirq0, 0x0);
	AUD_WrFL(aud_dirq0);

	AUD_RdFL(aud_dirq1);
	AUD_Wr01(aud_dirq1, aud_dirq1, 0x0);
	AUD_WrFL(aud_dirq1);

	return;
}

/**
 *  ANALOG_ReadRegister
*/
int ANALOG_ReadRegister(UINT8 slaveAddr, UINT8 regAddr, UINT8 *value)
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		ret = L9B0_ANALOG_ReadRegister(slaveAddr, regAddr, value);
 	}
	else
	{
		ret = L9A0_ANALOG_ReadRegister(slaveAddr, regAddr, value);
	}

	return ret;
}

/**
 *  ANALOG_WriteRegister
*/
int ANALOG_WriteRegister(UINT8 slaveAddr, UINT8 regAddr, UINT8 value)
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_ANALOG_WriteRegister(slaveAddr, regAddr, value);
	else
		ret = L9A0_ANALOG_WriteRegister(slaveAddr, regAddr, value);

	return ret;
}

/**
 * Initializes the clock and reset module register value.
 * @see ADEC_Init
*/
int MIXED_IP_ADEC_L9_InitClockRegisters( void )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_InitClockRegisters();
	else
		ret = L9A0_MIXED_IP_ADEC_L9_InitClockRegisters();

	return ret;
}

/**
 * Initializes the Mixed IP ADC, MIC ADC, DAC and HP module register value.
 * @see ADEC_L9_InitModule
*/
int MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_T ui32SrcFreq, LX_ADEC_CLOCK_SRC_T ui8AdecClockSrc )
{
	int 	ret = RET_OK;

	g_SetClcokSrc = ui8AdecClockSrc;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_SetClockSource(ui32SrcFreq, ui8AdecClockSrc);
	else
		ret = L9A0_MIXED_IP_ADEC_L9_SetClockSource(ui8AdecClockSrc);

	return ret;
}


/**
 * Sets a PCM Clock frequency and Rate to sync with SPDIF clock for SPDIF output.
 * @see ADEC_L9_InitModule
*/
int MIXED_IP_ADEC_L9_SetPcmClockRate ( LX_ADEC_SAMPLING_FREQ_T ui32SrcFreq, UINT32 ui32PcmClock )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_SetPcmClockRate(ui32SrcFreq, ui32PcmClock);
	else
		ret = L9A0_MIXED_IP_ADEC_L9_SetPcmClockRate(ui32SrcFreq, ui32PcmClock);

	return ret;
}

int MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq ( LX_ADEC_SAMPLING_FREQ_T ui32SamplingFreq, LX_ADEC_SPDIF_DTO_RATE_T stSpdifDtoRate )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(ui32SamplingFreq, stSpdifDtoRate);
	else
		ret = L9A0_MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(ui32SamplingFreq, stSpdifDtoRate);

	return ret;
}

/**
 * Initializes the Mixed IP ADC, MIC ADC, DAC and HP module register value.
 * @see ADEC_L9_InitModule
*/
int MIXED_IP_ADEC_L9_InitCodecRegisters ( void )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_InitCodecRegisters();
	else
		ret = L9A0_MIXED_IP_ADEC_L9_InitCodecRegisters();

	return ret;
}

/**
 * Control the Mixed IP AAD, ADC, MIC ADC, DAC and HPDRV module to power on and mute off.
 * @see ADEC_L9_SetSource
*/
int MIXED_IP_ADEC_L9_PowerControl ( LX_ADEC_POWER_BLOCK_T block, LX_ADEC_POWER_MODE_T mode )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_PowerControl(block, mode);
	else
		ret = L9A0_MIXED_IP_ADEC_L9_PowerControl(block, mode);

	return ret;
}

/**
 * Start the Mixed IP ADC, MIC ADC, DAC and HP module to mute off.
 * @see ADEC_L9_InitModule
*/
int MIXED_IP_ADEC_L9_StartCodec ( void )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_StartCodec();
	else
		ret = L9A0_MIXED_IP_ADEC_L9_StartCodec();

	return ret;
}

/**
 * Stop the Mixed IP ADC, MIC ADC, DAC and HP module to mute off.(only upper L8 B0 Chip)
 * @see ADEC_L9_InitModule
*/
int MIXED_IP_ADEC_L9_StopCodec ( void )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_StopCodec();
	else
		ret = L9A0_MIXED_IP_ADEC_L9_StopCodec();

	return ret;
}

/**
 * Sets a ADC port number for ADEC I2S input.
 * @see ADEC_L9_SetSource
*/
int MIXED_IP_ADEC_L9_SetADCPortNumber ( UINT8 ui8PortNum )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_SetADCPortNumber(ui8PortNum);
	else
		ret = L9A0_MIXED_IP_ADEC_L9_SetADCPortNumber(ui8PortNum);

	return ret;
}

/**
 * Sets the volume of DAC output.
 * @see ADEC_L9_SetDACOutVolume
*/
int MIXED_IP_ADEC_L9_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_SetDACOutVolume(stDACOutVol);
	else
		ret = L9A0_MIXED_IP_ADEC_L9_SetDACOutVolume(stDACOutVol);

	return ret;
}

/**
 * Sets the mute of DAC output.
 * @see ADEC_L9_SetOutputPortMute
*/
int MIXED_IP_ADEC_L9_SetDACOutMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute )
{
	int 	ret = RET_OK;

	//Check a L9 chip revision
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
		ret = L9B0_MIXED_IP_ADEC_L9_SetDACOutMute(stOutPortMute);
	else
		ret = L9A0_MIXED_IP_ADEC_L9_SetDACOutMute(stOutPortMute);

	return ret;
}


/**
 * Create EMP Interbuf for ARM and DSP
 * @see
*/
int  ADEC_L9_Create_CPBM_Interbuf_Writer(void)
{
	int ret = WRITER_RET_OPEN_OK;

	unsigned int structSize;

	//Allocation memory for CPBM buffer structure
	if(g_pCPBM_WriterStruct == NULL)
	{
		g_pCPBM_WriterStruct = OS_KMalloc(InterBufWriter_GetStructSize());
	}

	//Get a Mix0 Struct Size
	structSize = CommonBuffer_GetStructSize(CPB_AUI_INDEX_COUNT);

	//Set a memory address for CPU and DSP
	gCPBM_ArmWriterAdap.dummyAddr = (unsigned char *)g_pui32CPBM_VirAddr;
	gCPBM_ArmWriterAdap.memBase   = (unsigned char *)(pstMemCfgADEC->cpbm_memory_base - CPB_CPU_TO_DSP_BASE_DIFF);
	gCPBM_ArmWriterAdap.memSize   = pstMemCfgADEC->cpbm_memory_size;

	//Creating buffer for CPBM
	ret = CommonBuffer_Create(
	(unsigned char *)((pstMemCfgADEC->cpbm_memory_base - CPB_CPU_TO_DSP_BASE_DIFF) + structSize),
	pstMemCfgADEC->cpbm_memory_size - structSize,
	CPB_MAX_ACCESS_SIZE,
	CPB_AUI_INDEX_COUNT,
	(ADEC_PROCESSOR_ADAP_IF_T *)&gCPBM_ArmWriterAdap,
	g_pui32CPBM_VirAddr,
	NULL);

	if(ret != WRITER_RET_OPEN_OK)
	{
		ADEC_ERROR("ADEC CommonBuffer_Create Error(ret = %d)!!!\n", ret);
	}

	//Creating AUI buffer for CPBM
	ret = InterBufWriter_CreateFromCommonBuffer(
	DEFAULT_MUTEX,
	(ADEC_PROCESSOR_ADAP_IF_T *)&gCPBM_ArmWriterAdap,
	(void *)&gpRealRegAUD->aud_cpb0_wo,
	g_pui32CPBM_VirAddr,
	g_pCPBM_WriterStruct);

    return ret;
}

/**
 * Create EMP Asso Interbuf for ARM and DSP
 * @see
*/
int  ADEC_L9_Create_CPBA_Interbuf_Writer(void)
{
	int ret = WRITER_RET_OPEN_OK;

	unsigned int structSize;

	//Allocation memory for CPBA buffer structure
	if(g_pCPBA_WriterStruct == NULL)
	{
		g_pCPBA_WriterStruct = OS_KMalloc(InterBufWriter_GetStructSize());
	}

	//Get a CPBA Struct Size
	structSize = CommonBuffer_GetStructSize(CPB_AUI_INDEX_COUNT);

	//Set a memory address for CPU and DSP
	gCPBA_ArmWriterAdap.dummyAddr = (unsigned char *)g_pui32CPBA_VirAddr;
	gCPBA_ArmWriterAdap.memBase   = (unsigned char *)(pstMemCfgADEC->cpba_memory_base - CPB_CPU_TO_DSP_BASE_DIFF);
	gCPBA_ArmWriterAdap.memSize   = pstMemCfgADEC->cpba_memory_size;

	//Creating buffer for CPBA
	ret = CommonBuffer_Create(
	(unsigned char*)((pstMemCfgADEC->cpba_memory_base - CPB_CPU_TO_DSP_BASE_DIFF) + structSize),
	pstMemCfgADEC->cpba_memory_size - structSize,
	CPB_MAX_ACCESS_SIZE,
	CPB_AUI_INDEX_COUNT,
	(ADEC_PROCESSOR_ADAP_IF_T *)&gCPBA_ArmWriterAdap,
	g_pui32CPBA_VirAddr,
	NULL);

	if(ret != WRITER_RET_OPEN_OK)
	{
		ADEC_ERROR("ADEC CommonBuffer_Create Error(ret = %d)!!!\n", ret);
	}

	//Creating AUI buffer for CPBM
	ret = InterBufWriter_CreateFromCommonBuffer(
	DEFAULT_MUTEX,
	(ADEC_PROCESSOR_ADAP_IF_T *)&gCPBA_ArmWriterAdap,
	(void *)&gpRealRegAUD->aud_cpb1_wo,
	g_pui32CPBA_VirAddr,
	g_pCPBA_WriterStruct);

    return ret;
}

/**
 * Create MIXER Interbuf for ARM and DSP
 * @see
*/
static int  ADEC_L9_Create_MIX_Interbuf_Writer(	\
		void **ppStruct, ARM_PROCESSOR_ADAP * bufAdapIf, UINT32 *pVirAddr,	\
		UINT32 bufAddr, UINT32 bufSize, void *pBufReg)
{
	int ret = WRITER_RET_OPEN_OK;

	unsigned int structSize;

	//Allocation memory for mix buffer structure
	if(*ppStruct == NULL)
	{
		*ppStruct = OS_KMalloc(InterBufWriter_GetStructSize());
	}

	//Get a Mix0 Struct Size
	structSize = CommonBuffer_GetStructSize(MPB_AUI_INDEX_COUNT);

	//Set a memory address for CPU and DSP
	bufAdapIf->dummyAddr = (unsigned char *)pVirAddr;
	bufAdapIf->memBase   = (unsigned char *)(bufAddr - MPB_CPU_TO_DSP_BASE_DIFF);
	bufAdapIf->memSize   = bufSize;

	//Create buffer for mixer
	ret = CommonBuffer_Create(
	(unsigned char *)((bufAddr - MPB_CPU_TO_DSP_BASE_DIFF) + structSize),
	bufSize - structSize,
	MPB_MAX_ACCESS_SIZE,
	MPB_AUI_INDEX_COUNT,
	(ADEC_PROCESSOR_ADAP_IF_T *)bufAdapIf,
	pVirAddr,
	NULL);

	if(ret != WRITER_RET_OPEN_OK)
	{
		ADEC_ERROR("ADEC Mix0 CommonBuffer_Create Error(ret = %d)!!!\n", ret);
	}

	//Create AUI buffer for mixer
	ret = InterBufWriter_CreateFromCommonBuffer(
	DEFAULT_MUTEX,
	(ADEC_PROCESSOR_ADAP_IF_T *)(bufAdapIf),
	(void *)pBufReg,
	pVirAddr,
	*ppStruct);

    return ret;
}

/**
 * Create an audio clip buffer.
 * This function copies the audio clip data into ADEC driver memory.
 * @see ADEC_DRV_StartAudioClip
*/
static int ADEC_L9_CreateAudioClipBuffer( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	int ret = RET_OK;

	//Check a adec mode and create cpb buffer
	if(i_eAdecMode == LX_ADEC_DECODE_MAIN)
	{
		if(g_pCPBM_WriterStruct == NULL)
		{
			/* Create / Init CPBM for EMP */
			ret = ADEC_L9_Create_CPBM_Interbuf_Writer();

			if(ret == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("CPBM : ADEC Create_Interbuf_Writer Success!!!\n");
			}
			else
			{
				ADEC_ERROR("CPBM : ADEC Create_Interbuf_Writer Fail!!!(0x%X)\n", ret);
				return RET_ERROR;
			}

			/* Clear aui index count */
			g_adec_kdrv.index[LX_ADEC_DEC_0].FeedCNT = 0;
			g_adec_kdrv.index[LX_ADEC_DEC_0].FeedIDX = 0;

			g_adec_kdrv.index[LX_ADEC_DEC_0].PrevNOTI = INITIAL_CONDITION ;
			g_adec_kdrv.index[LX_ADEC_DEC_0].NewNOTI  = 0 ;
			g_adec_kdrv.index[LX_ADEC_DEC_0].WriteCNT = INITIAL_CONDITION;
			g_adec_kdrv.index[LX_ADEC_DEC_0].ReadCNT  = 0;
		 	g_adec_kdrv.index[LX_ADEC_DEC_0].FreeCNT  = CPB_AUI_INDEX_COUNT;
		 	g_adec_kdrv.index[LX_ADEC_DEC_0].UsedCNT  = 0;
		 	g_adec_kdrv.index[LX_ADEC_DEC_0].NotiCNT  = 0;

			//clear a pts index0
			AUD_Wr(aud_pts_index0, 0x0);
			AUD_WrFL(aud_pts_index0);

			//clear a pts index0 for dec0
			AUD_Wr(aud_in_index_dec0, 0x0);
			AUD_WrFL(aud_in_index_dec0);

			//clear a present index0 for dec0
			AUD_Wr(aud_in_index_present0, 0x0);
			AUD_WrFL(aud_in_index_present0);
		}
	}
	else if(i_eAdecMode == LX_ADEC_DECODE_ASSO)
	{
		if(g_pCPBA_WriterStruct == NULL)
		{
			/* Create CPBM for EMP */
			ret = ADEC_L9_Create_CPBA_Interbuf_Writer();

			if(ret == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("CPBA : ADEC Create_Interbuf_Writer Success!!!\n");
			}
			else
			{
				ADEC_ERROR("CPBA : Create_Interbuf_Writer Fail!!!(0x%X)\n", ret);
				return RET_ERROR;
			}

			/* Clear aui index count */
			g_adec_kdrv.index[LX_ADEC_DEC_1].FeedCNT = 0;
			g_adec_kdrv.index[LX_ADEC_DEC_1].FeedIDX = 0;

			g_adec_kdrv.index[LX_ADEC_DEC_1].PrevNOTI = INITIAL_CONDITION ;
			g_adec_kdrv.index[LX_ADEC_DEC_1].NewNOTI  = 0 ;
			g_adec_kdrv.index[LX_ADEC_DEC_1].WriteCNT = INITIAL_CONDITION;
			g_adec_kdrv.index[LX_ADEC_DEC_1].ReadCNT  = 0;
		 	g_adec_kdrv.index[LX_ADEC_DEC_1].FreeCNT  = CPB_AUI_INDEX_COUNT;
		 	g_adec_kdrv.index[LX_ADEC_DEC_1].UsedCNT  = 0;
		 	g_adec_kdrv.index[LX_ADEC_DEC_1].NotiCNT  = 0;

			//clear a pts index1
			AUD_Wr(aud_pts_index1, 0x0);
			AUD_WrFL(aud_pts_index1);

			//clear a pts index1 for dec1
			AUD_Wr(aud_in_index_dec1, 0x0);
			AUD_WrFL(aud_in_index_dec1);

			//clear a present index1 for dec1
			AUD_Wr(aud_in_index_present1, 0x0);
			AUD_WrFL(aud_in_index_present1);
		}
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF0)
	{
		//Create MIX0 buffer to protect abnormal operation
		if(g_pMIX0_WriterStruct == NULL)
		{
			/* Create MIX0 for EMP */
			ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
				  (void *)&g_pMIX0_WriterStruct, (void *)&gMIX0_ArmWriterAdap, (UINT32 *)g_pui32MIX0_VirAddr,	\
				   pstMemCfgADEC->mpb0_memory_base, pstMemCfgADEC->mpb0_memory_size, (void *)&gpRealRegAUD->aud_mpb0_wo);

			if(ret == WRITER_RET_OPEN_OK)
			{

				ADEC_PRINT("ADEC ADEC_L9_Create_MIX0_Interbuf_Writer Success!!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC ADEC_L9_Create_MIX0_Interbuf_Writer Fail!!!\n");
				return RET_ERROR;
			}

			/* Clear mixer aui index count */
			g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_0] = 0;

			//clear a mixer index
			AUD_Wr(aud_in_index_mix0, 0x0);
			AUD_WrFL(aud_in_index_mix0);

			//clear a present index for mix0
			AUD_Wr(aud_in_index_present2, 0x0);
			AUD_WrFL(aud_in_index_present2);
		}
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF1)
	{
		//Create MIX1 buffer to protect abnormal operation
		if(g_pMIX1_WriterStruct == NULL)
		{
			/* Create MIX1 for EMP */
			ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
				  (void *)&g_pMIX1_WriterStruct, (void *)&gMIX1_ArmWriterAdap, (UINT32 *)g_pui32MIX1_VirAddr,	\
				   pstMemCfgADEC->mpb1_memory_base, pstMemCfgADEC->mpb1_memory_size, (void *)&gpRealRegAUD->aud_mpb1_wo);

			if(ret == WRITER_RET_OPEN_OK)
			{

				ADEC_PRINT("ADEC ADEC_L9_Create_MIX1_Interbuf_Writer Success!!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC ADEC_L9_Create_MIX1_Interbuf_Writer Fail!!!\n");
				return RET_ERROR;
			}

			/* Clear mixer aui index count */
			g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_1] = 0;

			//clear a mixer index
			AUD_Wr(aud_in_index_mix1, 0x0);
			AUD_WrFL(aud_in_index_mix1);

			//clear a present index for mix1
			AUD_Wr(aud_in_index_present3, 0x0);
			AUD_WrFL(aud_in_index_present3);
		}
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF2)
	{
		//Create MIX2 buffer to protect abnormal operation
		if(g_pMIX2_WriterStruct == NULL)
		{
			/* Create MIX2 for EMP */
			ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
				  (void *)&g_pMIX2_WriterStruct, (void *)&gMIX2_ArmWriterAdap, (UINT32 *)g_pui32MIX2_VirAddr,	\
				   pstMemCfgADEC->mpb2_memory_base, pstMemCfgADEC->mpb2_memory_size, (void *)&gpRealRegAUD->aud_mpb2_wo);

			if(ret == WRITER_RET_OPEN_OK)
			{

				ADEC_PRINT("ADEC ADEC_L9_Create_MIX2_Interbuf_Writer Success!!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC ADEC_L9_Create_MIX2_Interbuf_Writer Fail!!!\n");
				return RET_ERROR;
			}

			/* Clear mixer aui index count */
			g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_2] = 0;

			//clear a mixer index
			AUD_Wr(aud_in_index_mix2, 0x0);
			AUD_WrFL(aud_in_index_mix2);

			//clear a present index for mix2
			AUD_Wr(aud_in_index_present4, 0x0);
			AUD_WrFL(aud_in_index_present4);
		}
	}
	else if(i_eAdecMode == LX_ADEC_MIX_BUF3)
	{
		//Create MIX3 buffer to protect abnormal operation
		if(g_pMIX3_WriterStruct == NULL)
		{
			/* Create MIX3 for EMP */
			ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
				  (void *)&g_pMIX3_WriterStruct, (void *)&gMIX3_ArmWriterAdap, (UINT32 *)g_pui32MIX3_VirAddr,	\
				   pstMemCfgADEC->mpb3_memory_base, pstMemCfgADEC->mpb3_memory_size, (void *)&gpRealRegAUD->aud_mpb3_wo);

			if(ret == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("ADEC ADEC_L9_Create_MIX3_Interbuf_Writer Success!!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC ADEC_L9_Create_MIX3_Interbuf_Writer Fail!!!\n");
				return RET_ERROR;
			}

			/* Clear mixer aui index count */
			g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_3] = 0;

			//clear a mixer index
			AUD_Wr(aud_in_index_mix3, 0x0);
			AUD_WrFL(aud_in_index_mix3);

			//clear a present index for mix3
			AUD_Wr(aud_in_index_present5, 0x0);
			AUD_WrFL(aud_in_index_present5);
		}
	}
	else if(i_eAdecMode == LX_ADEC_LLB_BUF)
	{
		//Create LLB buffer to protect abnormal operation
		if(g_pLLB_WriterStruct == NULL)
		{
			/* Create LLB for EMP */
			ret = ADEC_L9_Create_MIX_Interbuf_Writer(	\
				  (void *)&g_pLLB_WriterStruct, (void *)&gLLB_ArmWriterAdap, (UINT32 *)g_pui32LLB_VirAddr,	\
				   pstMemCfgADEC->llb_memory_base, pstMemCfgADEC->llb_memory_size, (void *)&gpRealRegAUD->aud_llb_wo);

			if(ret == WRITER_RET_OPEN_OK)
			{
				ADEC_PRINT("ADEC ADEC_L9_Create_LLB_Interbuf_Writer Success!!!\n");
			}
			else
			{
				ADEC_ERROR("ADEC ADEC_L9_Create_LLB_Interbuf_Writer Fail!!!\n");
				return RET_ERROR;
			}

			/* Clear mixer aui index count */
			g_adec_kdrv.mixerIndex[LX_ADEC_MIXER_4] = 0;

			//clear a mixer index
			AUD_Wr(aud_in_index_llb, 0x0);
			AUD_WrFL(aud_in_index_llb);

			//clear a present index for llb
			AUD_Wr(aud_in_index_llb, 0x0);
			AUD_WrFL(aud_in_index_llb);
		}
	}

	else
	{
		ADEC_ERROR("ADEC_L9_CreateAudioClipBuffer : adecMode = 0x%X!!!\n", i_eAdecMode);
		return RET_ERROR;
	}

	ADEC_PRINT("ADEC_L9_CreateAudioClipBuffer : mode [0x%x]\n", i_eAdecMode);
 	return ret;
}

/**
 * Create AENC Interbuf for ARM and DSP
 * @see
*/
int  ADEC_L9_Create_AENC_Interbuf_Reader(void)
{
	int ret = WRITER_RET_OPEN_OK;

	ADEC_PRINT ( "AENC address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->enc_memory_base);
	ADEC_PRINT ( "AENC address-virt[0x%08X]\n", (UINT32) g_pui32AENC_VirAddr);

	//Allocation memory for ENC buffer structure
	if(g_pAENC_ReaderStruct == NULL)
	{
		g_pAENC_ReaderStruct = OS_KMalloc(InterBufReader_GetStructSize());
	}

	//Set a memory address for CPU and DSP
	gAENC_ArmReaderAdap.dummyAddr = (unsigned char *)g_pui32AENC_VirAddr;
	gAENC_ArmReaderAdap.memBase   = (unsigned char *)(pstMemCfgADEC->enc_memory_base - ENC_CPU_TO_DSP_BASE_DIFF);
	gAENC_ArmReaderAdap.memSize   = pstMemCfgADEC->enc_memory_size;

	ADEC_PRINT ( "AENC dummyAddr [0x%08X]\n", (UINT32) gAENC_ArmReaderAdap.dummyAddr);
	ADEC_PRINT ( "AENC memBase   [0x%08X]\n", (UINT32) gAENC_ArmReaderAdap.memBase);
	ADEC_PRINT ( "AENC memSize   [0x%08X]\n", (UINT32) gAENC_ArmReaderAdap.memSize);

	//Create AUI buffer for ENC
	ret = InterBufReader_CreateFromCommonBuffer(
	DEFAULT_MUTEX,
	(ADEC_PROCESSOR_ADAP_IF_T *)&gAENC_ArmReaderAdap,
	(void *)&gpRealRegAUD->aud_enc_wo,
	g_pui32AENC_VirAddr,
	g_pAENC_ReaderStruct);

    return ret;
}

/**
 * Create APCM Interbuf for ARM and DSP
 * @see
*/
int  ADEC_L9_Create_APCM_Interbuf_Reader(void)
{
	int ret = WRITER_RET_OPEN_OK;

	ADEC_PRINT ( "APCM address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->ddco_memory_base);
	ADEC_PRINT ( "APCM address-virt[0x%08X]\n", (UINT32) g_pui32APCM_VirAddr);

	//Allocation memory for PCM buffer structure
	if(g_pAPCM_ReaderStruct == NULL)
	{
		g_pAPCM_ReaderStruct = OS_KMalloc(InterBufReader_GetStructSize());
	}

	//Set a memory address for CPU and DSP
	gAPCM_ArmReaderAdap.dummyAddr = (unsigned char *)g_pui32APCM_VirAddr;
	gAPCM_ArmReaderAdap.memBase   = (unsigned char *)(pstMemCfgADEC->ddco_memory_base - ENC_CPU_TO_DSP_BASE_DIFF);
	gAPCM_ArmReaderAdap.memSize   = pstMemCfgADEC->ddco_memory_size;

	ADEC_PRINT ( "APCM dummyAddr [0x%08X]\n", (UINT32) gAPCM_ArmReaderAdap.dummyAddr);
	ADEC_PRINT ( "APCM memBase   [0x%08X]\n", (UINT32) gAPCM_ArmReaderAdap.memBase);
	ADEC_PRINT ( "APCM memSize   [0x%08X]\n", (UINT32) gAPCM_ArmReaderAdap.memSize);

	//Create AUI buffer for PCM
	ret = InterBufReader_CreateFromCommonBuffer(
	DEFAULT_MUTEX,
	(ADEC_PROCESSOR_ADAP_IF_T *)&gAPCM_ArmReaderAdap,
	(void *)&gpRealRegAUD->aud_ddco_wo,
	g_pui32APCM_VirAddr,
	g_pAPCM_ReaderStruct);

    return ret;
}

/**
 * Create AACR Interbuf for ARM and DSP for GLD function
 * @see
*/
int  ADEC_L9_Create_AACR_Interbuf_Reader(void)
{
	int ret = WRITER_RET_OPEN_OK;

	ADEC_PRINT ( "AACR address-phy [0x%08X]\n", (UINT32) pstMemCfgADEC->acr_memory_base);
	ADEC_PRINT ( "AACR address-virt[0x%08X]\n", (UINT32) g_pui32AACR_VirAddr);

	//Allocation memory for PCM buffer structure
	if(g_pAACR_ReaderStruct == NULL)
	{
		g_pAACR_ReaderStruct = OS_KMalloc(InterBufReader_GetStructSize());
	}

	//Set a memory address for CPU and DSP for GLD function
	gAACR_ArmReaderAdap.dummyAddr = (unsigned char *)g_pui32AACR_VirAddr;
	gAACR_ArmReaderAdap.memBase   = (unsigned char *)(pstMemCfgADEC->acr_memory_base - ENC_CPU_TO_DSP_BASE_DIFF);
	gAACR_ArmReaderAdap.memSize   = pstMemCfgADEC->acr_memory_size;

	ADEC_PRINT ( "AACR dummyAddr [0x%08X]\n", (UINT32) gAACR_ArmReaderAdap.dummyAddr);
	ADEC_PRINT ( "AACR memBase   [0x%08X]\n", (UINT32) gAACR_ArmReaderAdap.memBase);
	ADEC_PRINT ( "AACR memSize   [0x%08X]\n", (UINT32) gAACR_ArmReaderAdap.memSize);

	//Create AUI buffer for PCM
	ret = InterBufReader_CreateFromCommonBuffer(
	DEFAULT_MUTEX,
	(ADEC_PROCESSOR_ADAP_IF_T *)&gAACR_ArmReaderAdap,
	(void *)&gpRealRegAUD->aud_acr_wo,
	g_pui32AACR_VirAddr,
	g_pAACR_ReaderStruct);

    return ret;
}


/**
 * Set a WMAPro codec parameters.
 * @see ADEC_L9_StartAudioClip.
*/
static int ADEC_SetWMAProParams(LX_ADEC_CLIP_START_INFO_T stStartInfo)
{
	UINT16		samplePerBlock_wma3;			///< sample per block (2bytes)
	UINT32		channelMask_wma3;				///< channel mask (4bytes)
	UINT64		advancedEncodeOptions2_wma3;	///< advanced encode options 2 (8bytes)
	UINT16		encodeOptions_wma3;				///< encode options (2bytes)
	UINT16		advancedEncodeOptions_wma3;		///< advanced encode options (2bytes)

	UINT32		samplePerBlock_wma2;			///< sample per block (4bytes)
	UINT16		encodeOptions_wma2;				///< encode options (2bytes)
	UINT32		superBlockAlign_wma2;			// super block align (4bytes)

	KDRV_ADEC_DEC_PARAM_T		decParam;
	KDRV_ADEC_WMAPRO_INFO_T		wmaproInfo;

	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_0, &decParam);

		decParam.adecInputPort = KDRV_ADEC_INPUT_SYSTEM_0;
	}
	else
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_1, &decParam);

		decParam.adecInputPort = KDRV_ADEC_INPUT_SYSTEM_1;
	}

	decParam.adecCodecType 	  = ADEC_ConvertCodecType(stStartInfo.adecType);
	decParam.bitRate 		  = stStartInfo.bitRate;		//(UINT16)(stStartInfo.bitRate/1000)); 	//kbps unit
	decParam.samplingFreq 	  = stStartInfo.samplingFreq;
	decParam.numberOfChannel  = stStartInfo.numberOfChannel;
	decParam.pcmBitsPerSample = stStartInfo.pcmBitsPerSample;

	wmaproInfo.numOfParam     = NUM_OF_PARAM_WMAPRO_INFO;
	wmaproInfo.avgBytesPerSec = stStartInfo.avgBytesPerSec;
	wmaproInfo.blockAlign	  = stStartInfo.blockAlign;
//	wmaproInfo.encoderOption  = stStartInfo.encoderOption;
	ADEC_ERROR("WMA Info : avg_bytes = %d, block_align = %d, enc_opt = %d\n",	\
				stStartInfo.avgBytesPerSec, stStartInfo.blockAlign, stStartInfo.encoderOption);

	//Check Sanity
	if(stStartInfo.pDSIBuffer == NULL)
	{
		ADEC_ERROR("ADEC_SetWMAProParams : buf = %p\n", stStartInfo.pDSIBuffer);

		return RET_ERROR;
	}

	if((stStartInfo.wmaFormatTag == WMAPRO_FORMAT_TAG)	\
	 ||(stStartInfo.wmaFormatTag == WMALOSSLESS_FORMAT_TAG))
	{
		//Check Sanity
		if(stStartInfo.lengthOfDSI != LENGTH_OF_DSI_FOR_WMA3)
		{
			ADEC_ERROR("ADEC_SetWMAProParams : length = %d\n", stStartInfo.lengthOfDSI);

			return RET_ERROR;
		}

		//Get a parameters from DSI buffer.
		samplePerBlock_wma3			= ADEC_READ16_LE((UINT8 *)stStartInfo.pDSIBuffer);
		channelMask_wma3			= ADEC_READ32_LE((UINT8 *)stStartInfo.pDSIBuffer +  2);
		advancedEncodeOptions2_wma3	= ADEC_READ64_LE((UINT8 *)stStartInfo.pDSIBuffer +  6);
		encodeOptions_wma3			= ADEC_READ16_LE((UINT8 *)stStartInfo.pDSIBuffer + 14);
		advancedEncodeOptions_wma3	= ADEC_READ16_LE((UINT8 *)stStartInfo.pDSIBuffer + 16);

		wmaproInfo.encoderOption  	= encodeOptions_wma3;
		wmaproInfo.wmaFormatTag 	= stStartInfo.wmaFormatTag;
		wmaproInfo.channelMask	  	= channelMask_wma3;
		wmaproInfo.advEncoderOption = advancedEncodeOptions_wma3;
		wmaproInfo.advEncoderOption2Hi = (UINT32)(advancedEncodeOptions2_wma3 >> 32);
		wmaproInfo.advEncoderOption2Lo = (UINT32)(advancedEncodeOptions2_wma3 & 0xFFFFFFFF);

		ADEC_ERROR("WMA3 Info. 1 : tag  = 0x%x, blk = %d, chan = %d\n", stStartInfo.wmaFormatTag, samplePerBlock_wma3, channelMask_wma3);
		ADEC_ERROR("WMA3 Info. 2 : opt  = %5d, adv = %2d, adv2 = %llu\n", encodeOptions_wma3, advancedEncodeOptions_wma3, advancedEncodeOptions2_wma3);
	}
	else
	{
		//Check Sanity
		if(stStartInfo.lengthOfDSI != LENGTH_OF_DSI_FOR_WMA2)
		{
			ADEC_ERROR("ADEC_SetWMAProParams : length = %d\n", stStartInfo.lengthOfDSI);

			return RET_ERROR;
		}

		//Get a parameters from DSI buffer.
		samplePerBlock_wma2		= ADEC_READ32_LE((UINT8 *)stStartInfo.pDSIBuffer);
		encodeOptions_wma2		= ADEC_READ16_LE((UINT8 *)stStartInfo.pDSIBuffer + 4);
		superBlockAlign_wma2	= ADEC_READ32_LE((UINT8 *)stStartInfo.pDSIBuffer + 6);

		wmaproInfo.encoderOption  	= encodeOptions_wma2;
		wmaproInfo.wmaFormatTag 	= stStartInfo.wmaFormatTag;
		wmaproInfo.channelMask	  	= 0;
		wmaproInfo.advEncoderOption = 0;

		wmaproInfo.advEncoderOption2Hi = 0x0;
		wmaproInfo.advEncoderOption2Lo = 0x0;

		ADEC_ERROR("WMA2 Info. 1 : tag  = 0x%x, blk = %d\n", stStartInfo.wmaFormatTag, samplePerBlock_wma2);
		ADEC_ERROR("WMA2 Info. 2 : opt  = %5d, superblk = %d\n", encodeOptions_wma2, superBlockAlign_wma2);
	}

	memcpy(&(decParam.ui32Param[0]), &wmaproInfo, sizeof(KDRV_ADEC_WMAPRO_INFO_T));

	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_0, &decParam);
	}
	else
	{
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_1, &decParam);
	}

	return RET_OK;
}

/**
 * Set a RA8 codec parameters.
 * @see ADEC_L9_StartAudioClip.
*/
static int ADEC_SetRA8Params(LX_ADEC_CLIP_START_INFO_T stStartInfo)
{
	UINT8		*pDSIBuffer;
	UINT8		majorVer;
	UINT8		minorVer;
	UINT16		samplesPerFrame;		// number of samples per frame
	UINT16		freqResponse;			// determines frequency response
	UINT16		cplStartRegion	= 0;	// related to joint stereo encoding
	UINT16		cplQuantBits	= 0;	// related to joint stereo encoding

	KDRV_ADEC_DEC_PARAM_T		decParam;
	KDRV_ADEC_RA_INFO_T			ra8Info;

	//Check Sanity
	if(stStartInfo.pDSIBuffer == NULL)
	{
		ADEC_ERROR("ADEC_SetRA8Params : buf is NULL\n");

		return RET_ERROR;
	}

	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_0, &decParam);

		decParam.adecInputPort = KDRV_ADEC_INPUT_SYSTEM_0;
	}
	else
	{
		ADEC_IPC_GetConfig(KDRV_ADEC_DEC_1, &decParam);

		decParam.adecInputPort = KDRV_ADEC_INPUT_SYSTEM_1;
	}

	decParam.adecCodecType 	  = ADEC_ConvertCodecType(stStartInfo.adecType);
	decParam.bitRate 		  = stStartInfo.bitRate;		//(UINT16)(stStartInfo.bitRate/1000)); 	//kbps unit
	decParam.samplingFreq 	  = stStartInfo.samplingFreq;
	decParam.numberOfChannel  = stStartInfo.numberOfChannel;
	decParam.pcmBitsPerSample = stStartInfo.pcmBitsPerSample;

	//Check Sanity
	// 16bytes or 12bytes
	if(stStartInfo.lengthOfDSI < MIN_LENGTH_OF_DSI_FOR_RA8)
	{
		ADEC_ERROR("ADEC_SetRA8Params : length = %d\n", stStartInfo.lengthOfDSI);

		return RET_ERROR;
	}

	//Get a parameters from DSI buffer.
	pDSIBuffer		= (UINT8 *)stStartInfo.pDSIBuffer;
	majorVer		= pDSIBuffer[0];
	minorVer		= pDSIBuffer[3];
	samplesPerFrame = ADEC_READ16_BE(pDSIBuffer + 4);
	freqResponse	= ADEC_READ16_BE(pDSIBuffer + 6);

	if( (majorVer >= RA8_MAJOR_V1) && (minorVer >= RA8_MINOR_V3)
	  &&(stStartInfo.lengthOfDSI >= MIN_LENGTH_OF_DSI_FOR_RA8_V13) )
	{
		cplStartRegion = ADEC_READ16_BE(pDSIBuffer + 12);
		cplQuantBits   = ADEC_READ16_BE(pDSIBuffer + 14);
	}
	else
	{
		ADEC_ERROR("ADEC_SetRA8Params : ver(major = %d, minor = %d) is mismatch!!!\n", majorVer, minorVer);
	}

	ra8Info.numOfParam		= NUM_OF_PARAM_RA_INFO;
	ra8Info.samplesPerFrame	= (UINT32)samplesPerFrame;
	ra8Info.freqResponse	= (UINT32)freqResponse;
	ra8Info.leafSize		= stStartInfo.leafSize;
	ra8Info.cplStartRegion	= (UINT32)cplStartRegion;
	ra8Info.cplQuantBits	= (UINT32)cplQuantBits;

	ADEC_ERROR("RA8: SampleFrame = %d, freqRes = %d, leafSize = %d, cplRegion = %d, cplBits = %d\n",	\
			    ra8Info.samplesPerFrame, ra8Info.freqResponse, ra8Info.leafSize, ra8Info.cplStartRegion, ra8Info.cplQuantBits);

	memcpy(&(decParam.ui32Param[0]), &ra8Info, sizeof(KDRV_ADEC_RA_INFO_T));

	if(stStartInfo.adecMode == LX_ADEC_DECODE_MAIN)
	{
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_0, &decParam);
	}
	else
	{
		ADEC_IPC_SetConfig(KDRV_ADEC_DEC_1, &decParam);
	}

	return RET_OK;
}


/**
 * Convert codec type from LX_ADEC_SRC_TYPE_T to KDRV_ADEC_CODEC_T
 * @see
*/
static int ADEC_ConvertCodecType( LX_ADEC_SRC_TYPE_T srcType)
{
	KDRV_ADEC_CODEC_T adecCodecType;

	switch (srcType)
	{
		case LX_ADEC_SRC_TYPE_PCM:
		case LX_ADEC_SRC_TYPE_SIF:
		case LX_ADEC_SRC_TYPE_SIF_BTSC:
		case LX_ADEC_SRC_TYPE_SIF_A2:
			adecCodecType = KDRV_ADEC_CODEC_PCM;
			break;

		case LX_ADEC_SRC_TYPE_AC3:
		case LX_ADEC_SRC_TYPE_EAC3:
			adecCodecType = KDRV_ADEC_CODEC_MS10_DDC;
			break;

		case LX_ADEC_SRC_TYPE_AAC:
		case LX_ADEC_SRC_TYPE_HEAAC:
			adecCodecType = KDRV_ADEC_CODEC_MS10_DDT;
			break;

		case LX_ADEC_SRC_TYPE_MPEG:
		case LX_ADEC_SRC_TYPE_MP3:
			adecCodecType = KDRV_ADEC_CODEC_MP3;
			break;

		case LX_ADEC_SRC_TYPE_DTS:
			adecCodecType = KDRV_ADEC_CODEC_DTS;
			break;

		case LX_ADEC_SRC_TYPE_WMA_PRO:
			adecCodecType = KDRV_ADEC_CODEC_WMA;
			break;

		case LX_ADEC_SRC_TYPE_ADPCM:
			adecCodecType = KDRV_ADEC_CODEC_ADPCM;
			break;

		case LX_ADEC_SRC_TYPE_VORBIS:
			adecCodecType = KDRV_ADEC_CODEC_VORBIS;
			break;

		case LX_ADEC_SRC_TYPE_AMR_NB:
			adecCodecType = KDRV_ADEC_CODEC_AMR_NB;
			break;

		case LX_ADEC_SRC_TYPE_AMR_WB:
			adecCodecType = KDRV_ADEC_CODEC_AMR_WB;
			break;

		case LX_ADEC_SRC_TYPE_MS10_DDCO:
			adecCodecType = KDRV_ADEC_CODEC_DDCO;
			break;

		case LX_ADEC_SRC_TYPE_AAC_LC_ENC:
			adecCodecType = KDRV_ADEC_CODEC_AAC_ENC;
			break;

		case LX_ADEC_SRC_TYPE_RA10:
			adecCodecType = KDRV_ADEC_CODEC_RA8;
			break;

		case LX_ADEC_SRC_TYPE_AAC_LC:
		case LX_ADEC_SRC_TYPE_DRA:
		default:
			ADEC_ERROR("ADEC_ConvertCodecType is unknown!!!(%d)\n", srcType);
			adecCodecType = KDRV_ADEC_CODEC_MS10_DDC;
			break;
	}

	return adecCodecType;
}

/**
 * Gets the automatic SPDIF output type accoring to ADEC source input and type.
 * @see
*/
static int ADEC_SetSPDIFOutputType(LX_ADEC_IN_PORT_T eInputPort, LX_ADEC_SRC_TYPE_T adecType)
{
	LX_ADEC_SPDIF_MODE_T 	spdifMode;

	KDRV_ADEC_SPDIF_PARAM_T	spdifParam;

	//Lock a ADEC SPDIF Semaphore
	OS_LockMutex(&g_ADEC_Sema_Spdif);

	ADEC_DEBUG("ADEC_SetSPDIFOutputType(APP spdifMode = %d, eInputPort = %d, adecType = %d)\n",	\
										g_adec_kdrv.spdifMode, eInputPort, adecType);

	switch(eInputPort)
	{
		case LX_ADEC_IN_PORT_TP:
			/* USER가 설정한 SPDIF Output Type이 AC-3 or Auto (Bitstream)인 경우 */
			if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3)
			{
				if((adecType == LX_ADEC_SRC_TYPE_AC3) || (adecType == LX_ADEC_SRC_TYPE_EAC3))
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if((adecType == LX_ADEC_SRC_TYPE_AAC) || (adecType == LX_ADEC_SRC_TYPE_HEAAC))
				{
					spdifMode = LX_ADEC_SPDIF_AAC;
				}
				else	//MPEG계열, PCM, etc...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			/* USER가 설정한 SPDIF Output Type이 DDCO(Bitstream)인 경우 */
			else if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO)
			{
				if((adecType == LX_ADEC_SRC_TYPE_AC3) || (adecType == LX_ADEC_SRC_TYPE_EAC3))
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if((adecType == LX_ADEC_SRC_TYPE_AAC) || (adecType == LX_ADEC_SRC_TYPE_HEAAC))
				{
					spdifMode = LX_ADEC_SPDIF_AAC;
				}
				else	//MPEG계열, PCM, etc...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			else	/* User가 설정한 SPDIF Output Type이 PCM인 경우 */
			{
				spdifMode = LX_ADEC_SPDIF_PCM;
			}
			break;

		case LX_ADEC_IN_PORT_I2S:
		case LX_ADEC_IN_PORT_SIF:
		case LX_ADEC_IN_PORT_ADC:
			spdifMode = LX_ADEC_SPDIF_PCM;
			break;

		case LX_ADEC_IN_PORT_SPDIF:
		case LX_ADEC_IN_PORT_HDMI:
			/* USER가 설정한 SPDIF Output Type이 AC-3 or Auto (Bitstream)인 경우 */
			if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3)
			{
				if(adecType == LX_ADEC_SRC_TYPE_AC3)
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if(adecType == LX_ADEC_SRC_TYPE_DTS)
				{
					spdifMode = LX_ADEC_SPDIF_DTS;
				}
				else	// AAC, HE-AAC, MPEG계열, PCM, ...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			/* USER가 설정한 SPDIF Output Type이 DTS (Bitstream)인 경우 */
			else if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)
			{
				if(adecType == LX_ADEC_SRC_TYPE_AC3)
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if(adecType == LX_ADEC_SRC_TYPE_DTS)
				{
					spdifMode = LX_ADEC_SPDIF_DTS;
				}
				else	// AAC, HE-AAC, MPEG계열, PCM, ...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			/* USER가 설정한 SPDIF Output Type이 DDCO(Bitstream)인 경우 */
			else if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO)
			{
				if(adecType == LX_ADEC_SRC_TYPE_AC3)
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if(adecType == LX_ADEC_SRC_TYPE_DTS)
				{
					spdifMode = LX_ADEC_SPDIF_DTS;
				}
				else	// AAC, HE-AAC, MPEG계열, PCM, ...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			else
			{
				spdifMode = LX_ADEC_SPDIF_PCM;
			}
			break;

		case LX_ADEC_IN_PORT_SYSTEM:
			/* USER가 설정한 SPDIF Output Type이 AC-3 or Auto (Bitstream)인 경우 */
			if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3)
			{
				if((adecType == LX_ADEC_SRC_TYPE_AC3) || (adecType == LX_ADEC_SRC_TYPE_EAC3))
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if((adecType == LX_ADEC_SRC_TYPE_AAC) || (adecType == LX_ADEC_SRC_TYPE_HEAAC))
				{
					spdifMode = LX_ADEC_SPDIF_AAC;
				}
				else if(adecType == LX_ADEC_SRC_TYPE_DTS)
				{
					spdifMode = LX_ADEC_SPDIF_DTS;
				}
				else	// AAC, HE-AAC, MPEG계열, PCM, ...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			/* USER가 설정한 SPDIF Output Type이 DTS (Bitstream)인 경우 */
			else if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)
			{
				if((adecType == LX_ADEC_SRC_TYPE_AC3) || (adecType == LX_ADEC_SRC_TYPE_EAC3))
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if((adecType == LX_ADEC_SRC_TYPE_AAC) || (adecType == LX_ADEC_SRC_TYPE_HEAAC))
				{
					spdifMode = LX_ADEC_SPDIF_AAC;
				}
				else if(adecType == LX_ADEC_SRC_TYPE_DTS)
				{
					spdifMode = LX_ADEC_SPDIF_DTS;
				}
				else	// AAC, HE-AAC, MPEG계열, PCM, ...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			/* USER가 설정한 SPDIF Output Type이 DDCO(Bitstream)인 경우 */
			else if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO)
			{
				if((adecType == LX_ADEC_SRC_TYPE_AC3) || (adecType == LX_ADEC_SRC_TYPE_EAC3))
				{
					spdifMode = LX_ADEC_SPDIF_AC3;
				}
				else if((adecType == LX_ADEC_SRC_TYPE_AAC) || (adecType == LX_ADEC_SRC_TYPE_HEAAC))
				{
					spdifMode = LX_ADEC_SPDIF_AAC;
				}
				else if(adecType == LX_ADEC_SRC_TYPE_DTS)
				{
					spdifMode = LX_ADEC_SPDIF_DTS;
				}
				else	// AAC, HE-AAC, MPEG계열, PCM, ...
				{
					spdifMode = LX_ADEC_SPDIF_PCM;
				}
			}
			else
			{
				spdifMode = LX_ADEC_SPDIF_PCM;
			}
			break;

		default:
			spdifMode = LX_ADEC_SPDIF_PCM;
			break;
	}

	//Set a SPDIF output type for GCD platform
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		//Set a SPDIF output type if DDCO is set.
		if(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO)
		{
			spdifMode = LX_ADEC_SPDIF_DDCO;		//set forcedly
		}
	}

	//Send a IPC command to set SPDIF output type
	ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	switch (spdifMode)
	{
		case LX_ADEC_SPDIF_PCM:
			spdifParam.type = 0; //PCM output
			g_adec_kdrv.bSpdifPcm = TRUE;

			ADEC_PRINT("ADEC_DRV_SetSPDIFOutputType PCM\n" );
			break;

		case LX_ADEC_SPDIF_AC3:
		case LX_ADEC_SPDIF_DTS:
		case LX_ADEC_SPDIF_AAC:
		case LX_ADEC_SPDIF_MP1:
		case LX_ADEC_SPDIF_MP2:
		case LX_ADEC_SPDIF_MP3:
		case LX_ADEC_SPDIF_DDCO:
		case LX_ADEC_SPDIF_NONE:
			spdifParam.type = 1; //Bitstream Output
			g_adec_kdrv.bSpdifPcm = FALSE;

			ADEC_PRINT("ADEC_DRV_SetSPDIFOutputType AC3, MPEG, AAC or DTS\n" );
			break;
	}

	ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	//Unlock a ADEC SPDIF Semaphore
	OS_UnlockMutex(&g_ADEC_Sema_Spdif);

	ADEC_DEBUG("ADEC_SetSPDIFOutputType(KDRV spdifMode = %d, pcm = %d)!!!\n", spdifMode, g_adec_kdrv.bSpdifPcm);
	return RET_OK;
}

/**
 * Sets a SPDIF output type for GCD model.
 * When SPDIF output type is AC-3, main or asso decoder ES output is selected when dual decoding is running.
 * @see ADEC_L9_SetSPDIFOutputType
*/
int ADEC_SetSPDIFOutputTypeForDual ( LX_ADEC_DEC_MODE_T i_eAdecMode )
{
	int ret = RET_OK;

	UINT8		decNum = LX_ADEC_DEC_0;
	BOOLEAN		bCurSpdifPcm = TRUE;

	ADEC_SET_SPDIF_IEC_T	spdifIec;
	KDRV_ADEC_SPDIF_PARAM_T	spdifParam;

	UINT32		samplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;

	//Set a default SPDIF IEC Source and lip sync  for IPC
	spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;

	//Check a adec mode
	if( (i_eAdecMode == LX_ADEC_DECODE_MAIN)	\
	  &&(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS) )
	{
		//Set a SPDIF IEC Source and lip sync  for IPC
		decNum = LX_ADEC_DEC_0;
		spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;

		//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
		if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
		  &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
		  &&(g_adec_kdrv.adecType[LX_ADEC_DEC_0] == LX_ADEC_SRC_TYPE_AAC || g_adec_kdrv.adecType[LX_ADEC_DEC_0] == LX_ADEC_SRC_TYPE_HEAAC) )
		{
			//Get a sampling frequency from SRC0 register.
			AUD_RdFL(aud_src0_sf);
			samplingFreq = AUD_Rd(aud_src0_sf);

			if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
				g_adec_kdrv.bSpdifPcm = TRUE;
			else
				g_adec_kdrv.bSpdifPcm = FALSE;
		}
		else
		{
			//Set a SPDIF Output type by adec aource and codec type
			ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
			if (ret != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
			}
		}

		//Save a current DEC0 spdif mode
		bCurSpdifPcm = g_adec_kdrv.bSpdifPcm;

		//Check a DEC1 start
		if(g_adec_kdrv.bSpdifPcm == TRUE && g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE)
		{
			//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
			if( (g_adec_kdrv.adecSource[LX_ADEC_DEC_1] == LX_ADEC_IN_PORT_SYSTEM)
			  &&(g_adec_kdrv.adecType[LX_ADEC_DEC_1] == LX_ADEC_SRC_TYPE_AAC || g_adec_kdrv.adecType[LX_ADEC_DEC_1] == LX_ADEC_SRC_TYPE_HEAAC) )
			{
				//Get a sampling frequency from SRC1 register.
				AUD_RdFL(aud_src1_sf);
				samplingFreq = AUD_Rd(aud_src1_sf);

				if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
					g_adec_kdrv.bSpdifPcm = TRUE;
				else
					g_adec_kdrv.bSpdifPcm = FALSE;
			}
			else
			{
				//Set a SPDIF Output type by adec aource and codec type
				ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_1], g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
				if (ret != RET_OK)
				{
					ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
				}
			}

			//Set a DEC1 for SPDIF ES output if DEC1 has ES output.
			if(g_adec_kdrv.bSpdifPcm == FALSE)
			{
				//Set a SPDIF IEC Source and lip sync  for IPC
				decNum = LX_ADEC_DEC_1;
				spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_1;
			}
		}
		else
		{
			//Restore a previous setting
			 g_adec_kdrv.bSpdifPcm = bCurSpdifPcm;
		}
	}
	else if( (i_eAdecMode == LX_ADEC_DECODE_ASSO)	\
		   &&(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS) )
	{
		//Set a SPDIF IEC Source and lip sync  for IPC
		decNum = LX_ADEC_DEC_1;
		spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_1;

		//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
		if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE)
		  &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_1] == LX_ADEC_IN_PORT_SYSTEM)
		  &&(g_adec_kdrv.adecType[LX_ADEC_DEC_1] == LX_ADEC_SRC_TYPE_AAC || g_adec_kdrv.adecType[LX_ADEC_DEC_1] == LX_ADEC_SRC_TYPE_HEAAC) )
		{
			//Get a sampling frequency from SRC1 register.
			AUD_RdFL(aud_src1_sf);
			samplingFreq = AUD_Rd(aud_src1_sf);

			if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
				g_adec_kdrv.bSpdifPcm = TRUE;
			else
				g_adec_kdrv.bSpdifPcm = FALSE;
		}
		else
		{
			//Set a SPDIF Output type by adec aource and codec type
			ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_1], g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
			if (ret != RET_OK)
			{
				ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_1]);
			}
		}

		//Save a current DEC0 spdif mode
		bCurSpdifPcm = g_adec_kdrv.bSpdifPcm;

		//Check a DEC1 start
		if(g_adec_kdrv.bSpdifPcm == TRUE && g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
		{
			//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
			if( (g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
			  &&(g_adec_kdrv.adecType[LX_ADEC_DEC_0] == LX_ADEC_SRC_TYPE_AAC || g_adec_kdrv.adecType[LX_ADEC_DEC_0] == LX_ADEC_SRC_TYPE_HEAAC) )
			{
				//Get a sampling frequency from SRC0 register.
				AUD_RdFL(aud_src0_sf);
				samplingFreq = AUD_Rd(aud_src0_sf);

				if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
					g_adec_kdrv.bSpdifPcm = TRUE;
				else
					g_adec_kdrv.bSpdifPcm = FALSE;
			}
			else
			{
				//Set a SPDIF Output type by adec aource and codec type
				ret = ADEC_SetSPDIFOutputType(g_adec_kdrv.adecSource[LX_ADEC_DEC_0], g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
				if (ret != RET_OK)
				{
					ADEC_ERROR("ADEC_SetSPDIFOutputType : Error! eSrcType = 0x%x\n", g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
				}
			}

			//Set a DEC1 for SPDIF ES output if DEC1 has ES output.
			if(g_adec_kdrv.bSpdifPcm == FALSE)
			{
				//Set a SPDIF IEC Source and lip sync  for IPC
				decNum = LX_ADEC_DEC_0;
				spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;
			}
		}
		else
		{
			//Restore a previous setting
			 g_adec_kdrv.bSpdifPcm = bCurSpdifPcm;
		}
	}
	else if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_PCM)	\
		   ||(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO) )
	{
		ADEC_DEBUG("ADEC_SetSPDIFOutputTypeForDual 1 : SPDIF pcm = %d, mode = %d(bEnc = %d)\n",	\
					g_adec_kdrv.bSpdifPcm, g_adec_kdrv.spdifMode, g_adec_kdrv.bEncStarted);
		return RET_OK;
	}
	else
	{
		ADEC_DEBUG("ADEC_SetSPDIFOutputTypeForDual 2 : SPDIF pcm = %d, mode = %d(bEnc = %d)\n",	\
					g_adec_kdrv.bSpdifPcm, g_adec_kdrv.spdifMode, g_adec_kdrv.bEncStarted);
	}

	//Lock a ADEC SPDIF Semaphore
	OS_LockMutex(&g_ADEC_Sema_Spdif);

	//Set a SPDIF IEC Source for IPC
#ifdef ENABLE_DDCO_TEST_DEC1
	decNum = LX_ADEC_DEC_1;
	spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_1;
#endif

	//Set a SPDIF IEC lip sync type for IPC
	if( (g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS)
	  &&(g_adec_kdrv.bSpdifPcm == FALSE) )
	{
		if(g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_HDMI)
		{
			spdifIec.ui32LipsyncOnOff = FALSE;
		}
		else
		{
			spdifIec.ui32LipsyncOnOff = TRUE;
		}
	}
	else
	{
		spdifIec.ui32LipsyncOnOff = FALSE;
	}

	ADEC_IPC_SetSPDIFIEC(spdifIec);

	//Send a IPC command to set SPDIF output type
	ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	if(g_adec_kdrv.bSpdifPcm == TRUE)
	{
		spdifParam.type = 0; //PCM output
	}
	else
	{
		spdifParam.type = 1; //Bitstream Output
	}

	ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

	//Unlock a ADEC SPDIF Semaphore
	OS_UnlockMutex(&g_ADEC_Sema_Spdif);

	ADEC_DEBUG("ADEC_SetSPDIFOutputTypeForDual : pcm = %d(bEncStarted = %d)\n", g_adec_kdrv.bSpdifPcm, g_adec_kdrv.bEncStarted);
	return RET_OK;
}

/**
 * Get DTO Rate.
*/
static LX_ADEC_SPDIF_DTO_RATE_T ADEC_GetDTORate(UINT32 clockSrc, LX_ADEC_SAMPLING_FREQ_T samplingFreq)
{
	UINT32	spdifDTOClockType;
	UINT32	spdifDTOFreq;

	LX_ADEC_SPDIF_DTO_RATE_T	spdifDtoRate;

	if(samplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ)
	{
		spdifDTOFreq = SPDIF_ES_OUT_FREQ_32KHZ;
	}
	else if(samplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ)
	{
		spdifDTOFreq = SPDIF_ES_OUT_FREQ_44_1KHZ;
	}
	else
	{
		spdifDTOFreq = SPDIF_ES_OUT_FREQ_48KHZ;
	}

	if(clockSrc == LX_ADEC_CLOCK_HDMI)
		spdifDTOClockType = SPDIF_ES_OUT_DTO_CLOCK_HDMI;
	else
		spdifDTOClockType = SPDIF_ES_OUT_DTO_CLOCK_NON_HDMI;

	spdifDtoRate = g_prevDTORate[spdifDTOClockType][spdifDTOFreq];

	ADEC_DEBUG("[%s] Sc %d, Fs %d, Rd %d\n", __F__, clockSrc, samplingFreq, spdifDtoRate);

	return spdifDtoRate;
}

#ifdef ENABLE_AAC_PARSER
/**
 * Get a sampling frequency from AAC ES data.
 * @see ADEC_L9_LoadAudioClip.
*/
static LX_ADEC_SAMPLING_FREQ_T ADEC_L9_GetAACSamplingFreq(ADEC_AAC_MEDIA_INFO_T mediaInfo)
{
	LX_ADEC_SAMPLING_FREQ_T samplingFreq;

	switch (mediaInfo.samplingFrequency)
	{
		case 96000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 96_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_96_KHZ;
			break;

		case 88200:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 88_2KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_88_2KHZ;
			break;

		case 64000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 64_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_64_KHZ;
			break;

		case 48000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 48_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;
			break;

		case 44100:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 44_1KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_44_1KHZ;
			break;

		case 32000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 32_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_32_KHZ;
			break;

		case 24000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 24_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_24_KHZ;
			break;

		case 22050:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 22_05KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_22_05KHZ;
			break;

		case 16000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 16_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_16_KHZ;
			break;

		case 12000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 12_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_12_KHZ;
			break;

		case 11025:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 11_025KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_11_025KHZ;
			break;

		case 8000:
			ADEC_PRINT("ADEC_L9_GetAACSamplingFreq : 8_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_8_KHZ;
			break;

		case 7350:
			ADEC_ERROR("ADEC_L9_GetAACSamplingFreq : 7350 -> 8_KHZ\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_8_KHZ;
			break;

		default:
			ADEC_ERROR("ADEC_L9_GetAACSamplingFreq : Unknown\n");
			samplingFreq = LX_ADEC_SAMPLING_FREQ_NONE;
			break;
	}

	ADEC_ERROR("AAC Info. 1 : sfreq = %d, extSfreq = %d, ch = %d, extch= %d\n",	\
				mediaInfo.samplingFrequency, mediaInfo.extensionSamplingFrequency,	\
				mediaInfo.channelConfiguration, mediaInfo.extensionChannelConfiguration);
	ADEC_ERROR("AAC Info. 2 : type  = %d, exttype = %d\n", mediaInfo.objectType, mediaInfo.extensionObjectType);
	ADEC_ERROR("AAC Info. 3 : sbr   = %d, ps = %d\n", mediaInfo.sbrPresentFlag, mediaInfo.psPresentFlag);

	return samplingFreq;
}
#endif	//#ifdef ENABLE_AAC_PARSER

/**
 * Routine for monitor task to check audio underflow issue
 * @see
*/
int ADEC_L9_Monitor_Task( void *i_pvParam )
{
	BOOLEAN 	bChangedDsp0Timer = TRUE;
	BOOLEAN 	bChangedDsp1Timer = TRUE;

	BOOLEAN		bDecStarted[LX_ADEC_DEC_2] = {FALSE, FALSE};
	BOOLEAN		bDecADStarted = FALSE;

	UINT8		decNum = LX_ADEC_DEC_0;
	UINT8		ui8DtoRate = 0;
	UINT32		ui32PcmClock = APLL_CLOCK_VALUE_12_288MHZ;

	UINT32		samplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;

	UINT32		ui32Count = 0;
	UINT32		ui32Dsp0Timer = 0;
	UINT32		ui32Dsp1Timer = 0;

	UINT32		ui32ResetCount = 0;

	UINT64 		clockBaseTime  = 0;
	UINT64 		streamBaseTime = 0;

	ADEC_CMD_SYNC_BASE_T 	syncBase;

	LX_ADEC_START_DEC_T		stStartInfo;
	LX_ADEC_STOP_DEC_T 		stStopInfo;

	LX_ADEC_SAMPLING_FREQ_T stSamplingFreq;
	KDRV_ADEC_SPDIF_PARAM_T	spdifParam;
	ADEC_SET_SPDIF_IEC_T	spdifIec;

	UINT32		ui32RecEventType = 0;
	ULONG		flags = 0;

	LX_ADEC_DEC_MODE_T	ui32RecEventMode 	= LX_ADEC_DEC_MODE_NONE;
	LX_ADEC_TASK_MSG_T	prevWaitQForMonitor = LX_ADEC_TASK_NONE_MSG;

	KDRV_ADEC_MIX_START_PARAM_T 	mixStartParam;

	LX_ADEC_SPDIF_DTO_RATE_T	spdifDtoRate		= LX_ADEC_SPDIF_DTO_NORMAL;

	ADEC_PRINT( "ADEC> Task is created!\n" );

	do
	{
		/* Check stop condition when device is closed. */
		if( kthread_should_stop())
		{
			ADEC_PRINT( "ADEC> Task - exit!\n" );
			break;
		}

		/* Clears a previous g_suiWaitQForMonitor value to reset value */
		if(g_suiWaitQForMonitor == prevWaitQForMonitor)
		{
			/* Reset a wait queue monitor variable. */
			g_suiWaitQForMonitor = LX_ADEC_TASK_NONE_MSG;

			wait_event_interruptible_timeout( Monitor_WaitQ,	\
											  g_suiWaitQForMonitor != LX_ADEC_TASK_NONE_MSG,	\
											  msecs_to_jiffies(WAIT_MONOTOR_TASK_TIMEOUT) );	//200 ms
		}
		else
		{
			ADEC_ERROR("ADEC_L9_Monitor_Task(msg = 0x%X, prev msg = 0x%X)\n",	\
						g_suiWaitQForMonitor, prevWaitQForMonitor);

			/* Reset a wait queue monitor variable only previous value. */
			g_suiWaitQForMonitor &= (~prevWaitQForMonitor);
		}

		/* Save a previous g_suiWaitQForMonitor */
		prevWaitQForMonitor = g_suiWaitQForMonitor;

		if(g_suiWaitQForMonitor)
		{
			ADEC_DEBUG("ADEC_L9_Monitor_Task(msg = %d(SPDIF pcm = %d, mode = %d))\n",	\
						g_suiWaitQForMonitor, g_adec_kdrv.bSpdifPcm, g_adec_kdrv.spdifMode);
		}
		else
		{
			ADEC_TRACE("ADEC_L9_Monitor_Task(msg = %d)\n", g_suiWaitQForMonitor);
		}


		/////////////////////////////////////
		//Process DSP0 Interrupt for SPDIF clock setting
		//Set a SPDIF clock frequency for ES output
		if( ((g_suiWaitQForMonitor & LX_ADEC_TASK_SET_SPDIF) || (g_suiWaitQForMonitor & LX_ADEC_TASK_SET_SPDIF_DEC1) )	\
		   &&(g_adec_kdrv.bSpdifPcm == FALSE)
		   &&(g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_AC3 || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DTS) )
		{
			ADEC_DEBUG("ADEC_L9_Monitor_Task(msg = %d, pcm = %d)\n", g_suiWaitQForMonitor, g_adec_kdrv.bSpdifPcm);

			if(g_suiWaitQForMonitor & LX_ADEC_TASK_SET_SPDIF)
			{
				//Set a SPDIF Source
				decNum = LX_ADEC_DEC_0;

				//Get a Mixer Volume to process SPDIF output in DEC0 dual decoding case
				ADEC_IPC_GetConfig(KDRV_ADEC_MP_00, &mixStartParam);
			}
			else
			{
				//Set a SPDIF Source
				decNum = LX_ADEC_DEC_1;

				//Get a Mixer Volume to process SPDIF output in DEC1 dual decoding case
				ADEC_IPC_GetConfig(KDRV_ADEC_MP_01, &mixStartParam);
			}

			//Check if a mixer volume is 0 in dual decoding case in GCD model
			if( (g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)	\
			  &&(g_suiWaitQForMonitor & LX_ADEC_TASK_SET_SPDIF)	\
			  &&(mixStartParam.ui32GainL == 0 && mixStartParam.ui32GainR == 0) )
			{
				ADEC_ERROR("ADEC_L9_Monitor_Task(msg = %d, pcm = %d, gain = %d)\n", g_suiWaitQForMonitor, g_adec_kdrv.bSpdifPcm, mixStartParam.ui32GainL);
			}
			else if( (g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)	\
			  	   &&(g_suiWaitQForMonitor & LX_ADEC_TASK_SET_SPDIF_DEC1)	\
			  	   &&(mixStartParam.ui32GainL == 0 && mixStartParam.ui32GainR == 0) )
			{
				ADEC_ERROR("ADEC_L9_Monitor_Task(msg = %d, pcm = %d, gain = %d)\n", g_suiWaitQForMonitor, g_adec_kdrv.bSpdifPcm, mixStartParam.ui32GainL);
			}
			else
			{
				//Wait for setting SRC in DSP 0
				msleep_interruptible(DSP_SRC_DELAY_30MS);

				//Lock a ADEC SPDIF Semaphore
				OS_LockMutex(&g_ADEC_Sema_Spdif);

				if(decNum == LX_ADEC_DEC_0)
				{
					//Set a SPDIF IEC Source and lip sync  for IPC
					spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;

					//Get a sampling frequency from SRC0 register.
					AUD_RdFL(aud_src0_sf);
					samplingFreq = AUD_Rd(aud_src0_sf);

					ADEC_DEBUG("SPDIF [dec = %d] : AC3/DTS/AAC SAMPLING_FREQ_%dKHZ\n", decNum, samplingFreq);
				}
				else
				{
					//Set a SPDIF IEC Source and lip sync  for IPC
					spdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_1;

					//Get a sampling frequency from SRC1 register.
					AUD_RdFL(aud_src1_sf);
					samplingFreq = AUD_Rd(aud_src1_sf);

					ADEC_DEBUG("SPDIF [dec = %d] : AC3/DTS/AAC SAMPLING_FREQ_%dKHZ\n", decNum, samplingFreq);
				}

				if((g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_AC3)	\
				 ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_EAC3)	\
				 ||(g_adec_kdrv.adecType[decNum] == LX_ADEC_SRC_TYPE_DTS))
				{
					// for PCM Clock Rate setting if SPDIF output is ES in HDMI input.
					if( (decNum == LX_ADEC_DEC_0)
					  &&(g_eCurAdecSource == LX_ADEC_IN_PORT_HDMI)
					  &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_HDMI) )
					{
						//Lock a ADEC PCM Semaphore for HDMI Input
						OS_LockMutex(&g_ADEC_Sema_Pcm);

						//Get a sampling frequency from SRC0 register.
						AUD_RdFL(aud_src0_sf);
						samplingFreq = AUD_Rd(aud_src0_sf);

						//Set a sampling freq. from SRC0 registers by decoder.
						g_adec_kdrv.adecHdmiInputFreq = samplingFreq;

						//Set a Sampling frequency and DTO Rate to compensate clock rate in-accuracy.
						(void)L9B0_MIXED_IP_ADEC_L9_SetPcmClockRate(samplingFreq, APLL_CLOCK_VALUE_12_288MHZ);

						ADEC_DEBUG("SET_PCM_RATE(initType = 0x%X) : pcm = %d, freq = %dKHZ, rate = %d\n",	\
									g_eAdecInitType, g_adec_kdrv.bSpdifPcm, samplingFreq, ui32PcmClock);

						//Unlock a ADEC PCM Semaphore for HDMI Input
						OS_UnlockMutex(&g_ADEC_Sema_Pcm);
					}

					spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, samplingFreq);

					(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(samplingFreq, spdifDtoRate);
				}
				else	//AAC or HE-AAC is 48, 44.1 and 32Khz for transcoding to AC3 codec in TP input.
				{
					spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, LX_ADEC_SAMPLING_FREQ_48_KHZ);

					(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, spdifDtoRate);

					if(g_adec_kdrv.adecSource[decNum] == LX_ADEC_IN_PORT_TP)
					{
						//In boot time, SRC0 is not set before decoding started from fw if this function is called.
						if( (samplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ)
						  ||(samplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ) )
						{
							ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", samplingFreq);

							ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
							spdifParam.type = 0; //PCM output
							ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

							//add IPC command for GCD platform
							if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
							{
								//Set a SPDIF IEC lip sync type for IPC
								spdifIec.ui32LipsyncOnOff = FALSE;

								ADEC_IPC_SetSPDIFIEC(spdifIec);
							}

							//update a global variable.
							g_adec_kdrv.bSpdifPcm = TRUE;
						}
					}
					else	//AAC or HE-AAC is 48Khz only for transcoding to AC3 codec in SYSTEM input.
					{
						if(samplingFreq != LX_ADEC_SAMPLING_FREQ_48_KHZ)
						{
							ADEC_DEBUG("LX_ADEC_SAMPLING_FREQ_%d_KHZ\n", samplingFreq);

							ADEC_IPC_GetConfig(KDRV_ADEC_SPDIF, &spdifParam);
							spdifParam.type = 0; //PCM output
							ADEC_IPC_SetConfig(KDRV_ADEC_SPDIF, &spdifParam);

							//add IPC command for GCD platform
							if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
							{
								//Set a SPDIF IEC lip sync type for IPC
								spdifIec.ui32LipsyncOnOff = FALSE;

								ADEC_IPC_SetSPDIFIEC(spdifIec);
							}

							//update a global variable.
							g_adec_kdrv.bSpdifPcm = TRUE;

							ADEC_DEBUG("ADEC_L9_Monitor_Task(msg = %d, pcm = %d)\n", g_suiWaitQForMonitor, g_adec_kdrv.bSpdifPcm);
						}
					}
				}

				//Unlock a ADEC SPDIF Semaphore
				OS_UnlockMutex(&g_ADEC_Sema_Spdif);
			}
		}
		//Set a SPDIF clock frequency for PCM output
		else if( ((g_suiWaitQForMonitor & LX_ADEC_TASK_SET_SPDIF) || (g_suiWaitQForMonitor & LX_ADEC_TASK_SET_SPDIF_DEC1) )	\
			   &&(g_adec_kdrv.bSpdifPcm == TRUE) )
		{
			//Lock a ADEC SPDIF Semaphore
			OS_LockMutex(&g_ADEC_Sema_Spdif);

			spdifDtoRate = ADEC_GetDTORate(g_SetClcokSrc, LX_ADEC_SAMPLING_FREQ_48_KHZ);

			(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(LX_ADEC_SAMPLING_FREQ_48_KHZ, spdifDtoRate);

			//Unlock a ADEC SPDIF Semaphore
			OS_UnlockMutex(&g_ADEC_Sema_Spdif);
		}

		///////////////////////////////////////////////////////
		//Process DSP1 Interrupt for MHEG CBT & SBT clock and DTO rate setting
		// for MHEG TP stream lipsync for DEC0
		if( (g_suiWaitQForMonitor & LX_ADEC_TASK_USE_GSTC)	\
		  &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_TP) )
		{
			BASE_AVLIPSYNC_LOCK();

			if( (g_pVDEC_LipsyncReg->vdec_dec0_cbt == 0xFFFFFFFF)
			  ||(g_pVDEC_LipsyncReg->vdec_dec0_sbt == 0xFFFFFFFF) )
			{
				// Set DEC0 Clock Basetime
				AUD_RdFL(aud_basestc);
				AUD_Wr(aud_dec0_cbt, AUD_Rd(aud_basestc) & 0x0FFFFFFF);
				AUD_WrFL(aud_dec0_cbt);

				// Set DEC0 Stream Basetime
				AUD_RdFL(aud_basepts);
				AUD_Wr(aud_dec0_sbt, AUD_Rd(aud_basepts) & 0x0FFFFFFF);
				AUD_WrFL(aud_dec0_sbt);

				ADEC_ERROR("aud_dec0_cbt   : 0x%8X!!!\n", AUD_Rd(aud_dec0_cbt));
				ADEC_ERROR("aud_dec0_sbt   : 0x%8X!!!\n", AUD_Rd(aud_dec0_sbt));
			}
			else
			{
				// Set DEC0 Clock Basetime
				AUD_Wr(aud_dec0_cbt, g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec0_cbt);

				// Set DEC0 Stream Basetime
				AUD_Wr(aud_dec0_sbt, g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF);
				AUD_WrFL(aud_dec0_sbt);

				clockBaseTime  = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_cbt & 0x0FFFFFFF;
				streamBaseTime = (UINT64)g_pVDEC_LipsyncReg->vdec_dec0_sbt & 0x0FFFFFFF;

				syncBase.module 	= KDRV_ADEC_DEC_0;
				syncBase.clockBase 	= clockBaseTime;
				syncBase.streamBase = streamBaseTime;

				ADEC_IPC_SetBaseTime(syncBase);

				ADEC_ERROR("vdec_dec0_cbt  : 0x%8X!!!\n", g_pVDEC_LipsyncReg->vdec_dec0_cbt);
				ADEC_ERROR("vdec_dec0_sbt  : 0x%8X!!!\n", g_pVDEC_LipsyncReg->vdec_dec0_sbt);
			}

			BASE_AVLIPSYNC_UNLOCK();

			//Wait time for abnormal operation
			msleep_interruptible(WAIT_MONOTOR_TASK_TIMEOUT);	//200 ms
		}
		// for PCM Clock Rate setting
		else if( (g_suiWaitQForMonitor & LX_ADEC_TASK_SET_PCM_RATE)
			   &&(g_eCurAdecSource == LX_ADEC_IN_PORT_HDMI)
			   &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_HDMI) )
		{
			ADEC_DEBUG("ADEC_L9_Monitor_Task(msg = %d, pcm = %d, src = %d)\n", g_suiWaitQForMonitor, g_adec_kdrv.bSpdifPcm, g_eCurAdecSource);

			//Lock a ADEC PCM Semaphore for HDMI Input
			OS_LockMutex(&g_ADEC_Sema_Pcm);

			//Get a sampling frequency from SetSource.
			samplingFreq = g_adec_kdrv.adecHdmiInputFreq;

			//Get a PCM rate from Clock Rate register.
			AUD_RdFL(aud_clock_rate);
			AUD_Rd01(aud_clock_rate, pcmout_rate, ui32PcmClock);
			ADEC_DEBUG("SET_PCM_RATE(initType = 0x%X) : pcm = %d, freq = %dKHZ, rate = %d\n",	\
						g_eAdecInitType, g_adec_kdrv.bSpdifPcm, samplingFreq, ui32PcmClock);

			//Set a Sampling frequency and DTO Rate to compensate clock rate in-accuracy.
			(void)L9B0_MIXED_IP_ADEC_L9_SetPcmClockRate(samplingFreq, ui32PcmClock);

			//Unlock a ADEC PCM Semaphore for HDMI Input
			OS_UnlockMutex(&g_ADEC_Sema_Pcm);
		}
		// for SPDIF DTO Rate setting
		else if( (g_suiWaitQForMonitor & LX_ADEC_TASK_SET_DTO_RATE)
			   &&(g_eCurAdecSource != LX_ADEC_IN_PORT_SIF)	//Protection code for Analog Audio Noise Issue if SET_DTO_RATE interrupt is called when SetSource is also called.
			   &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] != LX_ADEC_IN_PORT_SIF) )
		{
			ADEC_DEBUG("ADEC_L9_Monitor_Task(msg = %d, pcm = %d, src = %d)\n", g_suiWaitQForMonitor, g_adec_kdrv.bSpdifPcm, g_eCurAdecSource);

			//Lock a ADEC SPDIF Semaphore
			OS_LockMutex(&g_ADEC_Sema_Spdif);

			//Check a SPDIF output mode : 48 Khz only mode
			if(g_adec_kdrv.bSpdifPcm == TRUE || g_adec_kdrv.spdifMode == LX_ADEC_SPDIF_DDCO)
			{
				stSamplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;
			}
			else
			{
				//Set a SPDIF DTO Rate
				if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
				{
					//Get a SPDIF IEC Source
					ADEC_IPC_GetSPDIFIEC(&spdifIec);

					if(spdifIec.ui32SetIecSrc == KDRV_ADEC_DEC_1)
					{
						//Get a sampling frequency from SRC1 register.
						AUD_RdFL(aud_src1_sf);

						samplingFreq = AUD_Rd(aud_src1_sf);
					}
					else
					{
						//Get a sampling frequency from SRC0 register.
						AUD_RdFL(aud_src0_sf);

						samplingFreq = AUD_Rd(aud_src0_sf);
					}
				}
				else
				{
					//Get a sampling frequency from SRC0 register.
					AUD_RdFL(aud_src0_sf);
					samplingFreq = AUD_Rd(aud_src0_sf);
				}

				if(samplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ)
				{
					stSamplingFreq = LX_ADEC_SAMPLING_FREQ_32_KHZ;
				}
				else if(samplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ)
				{
					stSamplingFreq = LX_ADEC_SAMPLING_FREQ_44_1KHZ;
				}
				else
				{
					stSamplingFreq = LX_ADEC_SAMPLING_FREQ_48_KHZ;
				}
			}

			//Get a DTO rate from Clock Rate register.
			AUD_RdFL(aud_clock_rate);
			AUD_Rd01(aud_clock_rate, dto_rate, ui8DtoRate);
			ADEC_DEBUG("SET_DTO_RATE(initType = 0x%X) : pcm = %d, freq = %dKHZ, rate = %d\n",	\
						g_eAdecInitType, g_adec_kdrv.bSpdifPcm, stSamplingFreq, ui8DtoRate);

			//Set a Sampling frequency and DTO Rate to compensate clock rate in-accuracy.
			(void)MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq(stSamplingFreq, ui8DtoRate);

			//Unlock a ADEC SPDIF Semaphore
			OS_UnlockMutex(&g_ADEC_Sema_Spdif);
		}
		else if(g_suiWaitQForMonitor & LX_ADEC_TASK_SET_PCM_RATE)
		{
			ADEC_DEBUG("ADEC_L9_Monitor_Task : SET_PCM_RATE(msg = %d)\n", g_suiWaitQForMonitor);
		}
		else if(g_suiWaitQForMonitor & LX_ADEC_TASK_SET_DTO_RATE)
		{
			ADEC_DEBUG("ADEC_L9_Monitor_Task : SET_DTO_RATE(msg = %d)\n", g_suiWaitQForMonitor);
		}
		else if(g_suiWaitQForMonitor & LX_ADEC_TASK_RESET_CLOCK)
		{
			ADEC_ERROR("ADEC_L9_Monitor_Task : RESET_CLOCK(msg = %d)\n", g_suiWaitQForMonitor);

			if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
			  ||(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE) )
			{
				//Reset a clockregisters if DSP checks a abnormal state for clock interrupt.
				(void)L9B0_MIXED_IP_ADEC_L9_ResetClockRegisters();

				//Update DSP1 Reset Counter for debug
				AUD_RdFL(aud_dbg_dsp1_reset);
				AUD_Rd01(aud_dbg_dsp1_reset, clock_reset_count, ui32ResetCount);
				AUD_Wr01(aud_dbg_dsp1_reset, clock_reset_count, ui32ResetCount + 1);
				AUD_WrFL(aud_dbg_dsp1_reset);
			}
		}


		///////////////////////////////////
		//Process for debug print : not interrupt value
		if ( (g_ui32CurEventType & LX_ADEC_EVENT_LIPSYNC_CHECK)
		   &&((ui32Count++ % MONOTOR_TASK_PRINT_TIME) == 0) )
		{
			AUD_RdFL(aud_debug00);
			AUD_RdFL(aud_debug01);
			AUD_RdFL(aud_debug02);
			AUD_RdFL(aud_debug03);

			ADEC_ERROR("[PTS %d] : [STC %d] : [DIFF %3d] : [ACT %5d]\n", \
						AUD_Rd(aud_debug00), AUD_Rd(aud_debug01), (SINT32)AUD_Rd(aud_debug02), AUD_Rd(aud_debug03));
		}


		////////////////////////////////////
		//Process and Check a DSP 0/1 latch-up status
		if(gbEnableDspMonitor == TRUE && g_suiWaitQForMonitor == LX_ADEC_TASK_NONE_MSG)
		{
			if( (g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
			  ||(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE) )
			{
				//Update a DSP running check count
				g_ui32CheckDSPRunningCnt++;

				//Check a DSP0 running status
				AUD_RdFL(aud_dsp0_stc);
				ui32Dsp0Timer = AUD_Rd(aud_dsp0_stc);

				if(gPreDsp0Timer == ui32Dsp0Timer)
				{
					ADEC_ERROR("DSP0 Timer not changed : [%d:%d]\n", ui32Dsp0Timer, gPreDsp0Timer);
					bChangedDsp0Timer = FALSE;
				}
				else
				{
					ADEC_DEBUG_TMP("DSP0 Timer changed : [%d:%d]\n", ui32Dsp0Timer, gPreDsp0Timer);
					bChangedDsp0Timer = TRUE;
				}

				//Check a DSP1 running status
				AUD_RdFL(aud_dsp1_stc);
				ui32Dsp1Timer = AUD_Rd(aud_dsp1_stc);

				if(gPreDsp1Timer == ui32Dsp1Timer)
				{
					ADEC_ERROR("DSP1 Timer not changed : [%d:%d]\n", ui32Dsp1Timer, gPreDsp1Timer);
					bChangedDsp1Timer = FALSE;
				}
				else
				{
					ADEC_DEBUG_TMP("DSP1 Timer changed : [%d:%d]\n", ui32Dsp1Timer, gPreDsp1Timer);
					bChangedDsp1Timer = TRUE;
				}

				//Update a  DSP 0 /1 Timer
				gPreDsp0Timer = ui32Dsp0Timer;
				gPreDsp1Timer = ui32Dsp1Timer;

#ifndef USE_ASK_ALIVE_DSP_MONITOR
				if((bChangedDsp0Timer == TRUE) && (bChangedDsp1Timer == TRUE))
				{
					g_ui32CheckDSPRunningCnt = 0;
				}
#else
				ADEC_IPC_CheckDSPRunning();
#endif

				//600 ms : 3 * 200 ms, audio out delay => 200 ms ==> total 1 seconds for ESD spec.
				if(g_ui32CheckDSPRunningCnt >= DSP_AUTO_RECOVERY_COUNT)
				{
					ADEC_ERROR( "Reset Audio...\n" );

					//Update DSP0 and DSP1 Reset Counter for debug
					if(gPreDsp0Timer == ui32Dsp0Timer && bChangedDsp0Timer == FALSE)
					{
						AUD_RdFL(aud_dbg_dsp0_reset);
						ui32ResetCount = AUD_Rd(aud_dbg_dsp0_reset);
						AUD_Wr(aud_dbg_dsp0_reset, ui32ResetCount + 1);
						AUD_WrFL(aud_dbg_dsp0_reset);
					}

					if(gPreDsp1Timer == ui32Dsp1Timer && bChangedDsp1Timer == FALSE)
					{
						AUD_RdFL(aud_dbg_dsp1_reset);
						AUD_Rd01(aud_dbg_dsp1_reset, dsp_reset_count, ui32ResetCount);
						AUD_Wr01(aud_dbg_dsp1_reset, dsp_reset_count, ui32ResetCount + 1);
						AUD_WrFL(aud_dbg_dsp1_reset);
					}

					//Clear a previous value
					bDecStarted[LX_ADEC_DEC_0] = FALSE;
					bDecStarted[LX_ADEC_DEC_1] = FALSE;
					bDecADStarted = FALSE;

					//Check a AD start status
					stStartInfo.bAdExist = FALSE;

					//Stop decoding
					//Check a current start status
					if(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_0] == TRUE)
					{
						stStopInfo.adecMode = LX_ADEC_DECODE_MAIN;
						(void)ADEC_L9_StopDecoding(stStopInfo);

						bDecStarted[LX_ADEC_DEC_0] = TRUE;
					}

					if(g_adec_kdrv.bDecADStarted == TRUE)
					{
						(void)ADEC_L9_StopADDecoding();

						stStartInfo.bAdExist = TRUE;

						bDecADStarted = TRUE;
					}
					else if(g_adec_kdrv.bDecStarted[LX_ADEC_DEC_1] == TRUE)
					{
						stStopInfo.adecMode = LX_ADEC_DECODE_ASSO;
						(void)ADEC_L9_StopDecoding(stStopInfo);

						bDecStarted[LX_ADEC_DEC_1] = TRUE;
					}

					//Wait for stopping DSP 0 / 1
					msleep_interruptible(DSP_STOP_DELAY_10MS);

					//Change a global current codec to re-load codec
					g_eCurLoadedCodec = LX_ADEC_SRC_TYPE_NONE;

					//Init IPC Register and memory
					ADEC_IPC_REG_Init(
						(void *)&gpRealRegAUD->aud_ipc_cmd_rptr,
						(void *)&gpRealRegAUD->aud_ipc_req_rptr,
						(void *)&gpRealRegAUD->aud_ipc_dbg_rptr);
					ADEC_IPC_Init(pstMemCfgADEC->ipc_memory_base, pstMemCfgADEC->ipc_memory_size);

					//Register IPC callback function
					ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_UNDERFLOW,   ADEC_REQ_PROC_Underflow);
					ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_PRESENT_END, ADEC_REQ_PROC_PresentEnd);
					ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_REQ_DSP0NOTRSP,  ADEC_REQ_PROC_DSP0NotRespond);

					ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_DEC_RESULT,  ADEC_RSP_CMD_Status);
					ADEC_IPC_REQ_Register_ReceivingCallback(ADEC_IPC_RSP_ALIVE, 	  ADEC_RSP_PROC_Alive);

					//Set DSP1 codec for audio processing
					(void)ADEC_L9_LoadDSP1Codec();

					//Reset DSP only if adec source is system
					if( (g_adec_kdrv.adecSource[LX_ADEC_DEC_0] == LX_ADEC_IN_PORT_SYSTEM)
					  ||(g_adec_kdrv.adecSource[LX_ADEC_DEC_1] == LX_ADEC_IN_PORT_SYSTEM) )
					{
						ADEC_ERROR( "ADEC_L9_LoadDSP0Codec...\n" );

						//Set codec source by adec type
						(void)ADEC_L9_LoadDSP0Codec(g_adec_kdrv.adecType[LX_ADEC_DEC_0]);

						//Send a Event to app. level to restart emp play again
						if ( g_ui32CurEventType & LX_ADEC_EVENT_DSP_RESET_DONE)
						{
							ui32RecEventType = LX_ADEC_EVENT_DSP_RESET_DONE;
							ui32RecEventMode = LX_ADEC_DECODE_MAIN | LX_ADEC_DECODE_ASSO;

							//spin lock for protection
							spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
							g_ui32RecEventType |= ui32RecEventType;
							spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

							//Wake up poll if any int is triggered
							if(g_ui32RecEventType)
								wake_up_interruptible_all(&adec_poll_wait_queue);
						}
					}

					//Re-start a DEC0 decoder
					stStartInfo.adecMode  = LX_ADEC_DECODE_MAIN;
					stStartInfo.adecType  = g_adec_kdrv.adecType[LX_ADEC_DEC_0];
					stStartInfo.bPcrExist = TRUE;

					//Check a saved current start status
					if( (bDecStarted[LX_ADEC_DEC_0] == TRUE)
					  &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_0] != LX_ADEC_IN_PORT_SYSTEM) )
					{
						ADEC_ERROR( "ADEC_L9_StartDecoding(DEC0)...\n" );

						(void)ADEC_L9_StartDecoding(stStartInfo);
					}

					//Re-start a DEC1 decoder
					stStartInfo.adecMode  = LX_ADEC_DECODE_ASSO;
					stStartInfo.adecType  = g_adec_kdrv.adecType[LX_ADEC_DEC_1];
					stStartInfo.bPcrExist = TRUE;

					if(bDecADStarted == TRUE)
					{
						ADEC_ERROR( "ADEC_L9_StartADDecoding(DEC1)...\n" );

						(void)ADEC_L9_StartADDecoding(g_adec_kdrv.adecType[LX_ADEC_DEC_0]);
					}
					else if( (bDecStarted[LX_ADEC_DEC_1] == TRUE)
						   &&(g_adec_kdrv.adecSource[LX_ADEC_DEC_1] != LX_ADEC_IN_PORT_SYSTEM) )
					{
						ADEC_ERROR( "ADEC_L9_StartDecoding(DEC1)...\n" );

						(void)ADEC_L9_StartDecoding(stStartInfo);
					}

					//Clear reset flag
					g_ui32CheckDSPRunningCnt = 0;
				}
			}
		}
	} while ( TRUE );

	return 0;
}


/** @} */


