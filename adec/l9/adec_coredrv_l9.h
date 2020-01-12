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
 *  driver interface header for adec device. ( used only within kdriver )
 *	adec device will teach you how to make device driver with new platform.
 *
 *  @author		Jong-Sang Oh(jongsang.oh@lge.com)
 *  @version	1.0
 *  @date		2011.04.09
 *
 *  @addtogroup lg1152_adec
 *	@{
 */

#ifndef	_ADEC_COREDRV_L9_H_
#define	_ADEC_COREDRV_L9_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "../adec_drv.h"
#include "../adec_cfg.h"
#include "adec_kapi.h"
#include "adec_reg_l9.h"
#include <linux/interrupt.h>    /* For isr function */
#include <asm/irq.h>			/* For isr function */

#include <mach/hardware.h> 		// For Register base address
#include "l9/base_addr_sw_l9.h"

#include "mixedIP_coredrv_l9_a0.h"
#include "mixedIP_coredrv_l9_b0.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define ENABLE_NETCAST_PRINT		//Enable Netcast Debug Print
#define	ENABLE_DIRECT_MIX_VOLUME	//Use a direct gain control for mix audio volume for GLD model.(2012.06.16)

#define ENABLE_ADEC_JTAG			//Enable ADEC DSP JTAG
//#define ENABLE_ADEC_JTAG_FOR_CPU	//Enable ADEC DSP JTAG for CPU PORT

//#define ENABLE_DDCO_TEST			//Enable DDCO Encoder Test for DEC0
//#define ENABLE_DDCO_TEST_DEC1		//Enable DDCO Encoder Test for DEC1

//#define ENABLE_AAC_PARSER			//Enable AAC Parser
//#define ENABLE_SCART1_MIC_PORT	//Enable Mixed IP SCART1 and MIC Input Port

//define register address
extern LX_ADEC_REG_CFG_T 		gstAdecRegCfg[];
extern LX_ADEC_REG_CFG_T 		*pstAdecRegCfg;

//define memory map
extern LX_ADECDEV_MEM_CFG_S_T 	gstMemCfgADEC[];
extern LX_ADECDEV_MEM_CFG_S_T 	*pstMemCfgADEC;

//define VDEC offset delay, +100(vdec delay change, 2011.01.06)
#define VDEC_TP_LIPSYNC_OFFSET			100

//define VDEC offset delay, -30(DDCO delay change, 2011.02.04)
#define DDCO_LIPSYNC_OFFSET_DELAY		 30

// define CPB / MPB / ENC buffer enumeration
#define CPB_CPU_TO_DSP_BASE_DIFF		(pstMemCfgADEC->dsp0_memory_base - 0x60000000)
#define MPB_CPU_TO_DSP_BASE_DIFF		(pstMemCfgADEC->dsp1_memory_base - 0x60000000)
#define ENC_CPU_TO_DSP_BASE_DIFF		(pstMemCfgADEC->dpbm_memory_base - 0x68000000)

#define CPB_MAX_ACCESS_SIZE         	(20*1024)
#define MPB_MAX_ACCESS_SIZE         	(10*1024)

#define CPB_AUI_INDEX_COUNT       		(0x1000)	//4096, GCD uses this value in fixed value. If change, notify to GCD.
#define MPB_AUI_INDEX_COUNT       		(0x1000)	//4096, GCD uses this value in fixed value. If change, notify to GCD.

#define INITIAL_CONDITION						0xffffffff
#define MAX_TRANSFER_NUM_MINUSONE_OF_INDEX     	(MAX_TRANSFER_NUM_OF_INDEX-1)

// define mix volume table size
#define ADEC_MIX_VOLUME_TABLE_SIZE		100

// define mix volume table size
#define ADEC_MAX_MIX_VOLUME_GAIN	   4096

//define APLL default value for 12.288MHZ
#define APLL_CLOCK_VALUE_12_288MHZ		0x1000	//4096
#define APLL_CLOCK_VALUE_10_752MHZ		0x0E00	//3584 : 4096 - 512
#define APLL_CLOCK_VALUE_13_824MHZ		0x1200	//4608 : 4096 + 512

// define enable Sound Engine param debug print
//#define	ENABLE_SE_PARAM_DEBUG

// Sound Engine(SE) Parameter Number(UNIT32)
#define LX_SE_MODE_ALL_PARAM_NUM		 4		///< SE MODE ALL PARAM NUMBER
#define LX_SE_MODE_VARI_PARAM_NUM		 3		///< SE MODE VARI PARAM NUMBER(Current Version)
#define LX_SE_FN001_VARI_PARAM_NUM		18		///< SE FN001 VARI PARAM NUMBER : AutoVolume
#define LX_SE_FN000_INIT_PARAM_NUM		17		///< SE FN000 INIT PARAM NUMBER : ClearVoice
#define LX_SE_FN000_VARI_PARAM_NUM		 3		///< SE FN000 VARI PARAM NUMBER
#define LX_SE_FN005_INIT_PARAM_NUM		13		///< SE FN005 INIT PARAM NUMBER
#define LX_SE_FN003_INIT_PARAM_NUM	   330		///< SE FN003 INIT PARAM NUMBER : Dynamic EQ(DEQ)
#define LX_SE_FN004_MOD1_PARAM_NUM	    25		///< SE FN004 MODE 1 VARI PARAM NUMBER : PEQ for Sound Mode
#define LX_SE_FN004_MOD1_PARAM_SIZE	     5		///< SE FN004 MODE 1 VARI PARAM Size NUMBER( 5 x 5)
#define LX_SE_FN004_MOD2_PARAM_NUM	    15		///< SE FN004 MODE 2 VARI PARAM NUMBER : PEQ for sound optimizer
#define LX_SE_FN004_MOD3_PARAM_NUM	    35		///< SE FN004 MODE 3 INIT PARAM NUMBER
#define LX_SE_FN002_INIT_PARAM_NUM	   123		///< SE FN002 INIT PARAM NUMBER: Dynamic perceptual bass enhancement(DPBE)
#define LX_SE_FN009_INIT_PARAM_NUM	   404		///< SE FN009 INIT PARAM NUMBER : OSD Volume
#define LX_SE_FN009_VARI_PARAM_NUM		 2		///< SE FN009 VARI PARAM NUMBER
#define LX_SE_FN006_VARI_PARAM_NUM		10		///< SE FN006 VARI PARAM NUMBER : Infinice Surround
#define LX_SE_FN007_INIT_PARAM_NUM	    76		///< SE FN007 INIT PARAM NUMBER : Sound Zooming
#define LX_SE_FN007_VARI_PARAM_NUM		 2		///< SE FN007 VARI PARAM NUMBER
#define LX_SE_FN010_INIT_PARAM_NUM	    10		///< SE FN010 INIT PARAM NUMBER: Input Level Check
#define LX_SE_FN010_VARI_PARAM_NUM		 2		///< SE FN010 VARI PARAM NUMBER

// Sound Engine(SE) Paramter Address define
#define ADDR_SE_PARAM_BASE				 0		//SE Parameter Base
#define ADDR_SE_MODE_VARI_PARAM			(ADDR_SE_PARAM_BASE)														//0x0000
#define ADDR_SE_FN001_VARI_PARAM		(ADDR_SE_MODE_VARI_PARAM  + LX_SE_MODE_ALL_PARAM_NUM)	//0x0010
#define ADDR_SE_FN000_INIT_PARAM		(ADDR_SE_FN001_VARI_PARAM + LX_SE_FN001_VARI_PARAM_NUM)	//0x0058
#define ADDR_SE_FN000_VARI_PARAM		(ADDR_SE_FN000_INIT_PARAM + LX_SE_FN000_INIT_PARAM_NUM)	//0x009C
#define ADDR_SE_FN005_INIT_PARAM		(ADDR_SE_FN000_VARI_PARAM + LX_SE_FN000_VARI_PARAM_NUM)	//0x00A8
#define ADDR_SE_FN003_INIT_PARAM		(ADDR_SE_FN005_INIT_PARAM + LX_SE_FN005_INIT_PARAM_NUM)	//0x00DC
#define ADDR_SE_FN004_MOD1_PARAM		(ADDR_SE_FN003_INIT_PARAM + LX_SE_FN003_INIT_PARAM_NUM)	//0x0604
#define ADDR_SE_FN004_MOD2_PARAM		(ADDR_SE_FN004_MOD1_PARAM + LX_SE_FN004_MOD1_PARAM_NUM)	//0x0668
#define ADDR_SE_FN004_MOD3_PARAM		(ADDR_SE_FN004_MOD2_PARAM + LX_SE_FN004_MOD2_PARAM_NUM)	//0x06A4
#define ADDR_SE_FN002_INIT_PARAM		(ADDR_SE_FN004_MOD3_PARAM + LX_SE_FN004_MOD3_PARAM_NUM)	//0x0730
#define ADDR_SE_FN009_INIT_PARAM		(ADDR_SE_FN002_INIT_PARAM + LX_SE_FN002_INIT_PARAM_NUM)	//0x091C
#define ADDR_SE_FN009_VARI_PARAM		(ADDR_SE_FN009_INIT_PARAM + LX_SE_FN009_INIT_PARAM_NUM)	//0x0F6C
#define ADDR_SE_FN006_VARI_PARAM		(ADDR_SE_FN009_VARI_PARAM + LX_SE_FN009_VARI_PARAM_NUM)	//0x0F74
#define ADDR_SE_FN007_INIT_PARAM		(ADDR_SE_FN006_VARI_PARAM + LX_SE_FN006_VARI_PARAM_NUM)	//0x0F9C
#define ADDR_SE_FN007_VARI_PARAM		(ADDR_SE_FN007_INIT_PARAM + LX_SE_FN007_INIT_PARAM_NUM)	//0x10CC
#define ADDR_SE_FN010_INIT_PARAM		(ADDR_SE_FN007_VARI_PARAM + LX_SE_FN007_VARI_PARAM_NUM)	//0x10D4
#define ADDR_SE_FN010_VARI_PARAM		(ADDR_SE_FN010_INIT_PARAM + LX_SE_FN010_INIT_PARAM_NUM)	//0x10FC

// Sound Engine Update Mask
#define MASK_SE_FN_MODE_SHIFT   		(8)
#define MASK_SE_FN_MODE_FN000   		(0x0001)
#define MASK_SE_FN_MODE_FN001   		(0x0002)
#define MASK_SE_FN_MODE_FN002   		(0x0004)
#define MASK_SE_FN_MODE_FN003   		(0x0008)
#define MASK_SE_FN_MODE_FN004_MODE1		(0x0010)
#define MASK_SE_FN_MODE_FN004_MODE2		(0x0020)
#define MASK_SE_FN_MODE_FN004_MODE3		(0x0040)
#define MASK_SE_FN_MODE_FN005   		(0x0080)
#define MASK_SE_FN_MODE_FN006   		(0x0100)
#define MASK_SE_FN_MODE_FN007   		(0x0200)
#define MASK_SE_FN_MODE_FN008   		(0x0400)
#define MASK_SE_FN_MODE_FN009   		(0x0800)
#define MASK_SE_FN_MODE_FN010   		(0x1000)

//define reset module
#define ADEC_OPTION_DSP0				0x0		//DSP0 only
#define ADEC_OPTION_DSP1				0x1		//DSP1 only
#define ADEC_OPTION_DSP_DEBUG			0x2		//DSP0,1 debug only
#define ADEC_OPTION_AAD					0x3		//AAD Module only
#define ADEC_OPTION_OTHERS				0x4		//Except DSP and Power-down
#define ADEC_OPTION_SPDIF				0x5		//SPDIF reset
#define ADEC_OPTION_ALL					0x6		//All reset
#define ADEC_OPTION_HWSRC				0x7		//HWSC reset

//define reset delay
#define ADEC_RESET_DELAY				  5		//5us

//define for constant value
#define ADEC_CODEC_NAME_SIZE			30

//define PCM sample data size for NC case
#define ADEC_PCM_1SEC					(48000*2*2*1)		///< 1 second PCM audio data(48Khz * 16 bits * 2 channel * 1 second)
#define ADEC_PCM_200MSEC				(ADEC_PCM_1SEC/5)	///< 200 mseconds PCM audio data

//define ACR PCM sample data size for GLD case
#define ADEC_ACR_PCM_1SEC				(11025*2*1*1)		///< 1 second ACR PCM audio data(11.025Khz * 16 bits * 1 channel * 1 second)
#define ADEC_ACR_PCM_186MSEC			(4096)				///< 185.760 mseconds ACR PCM audio data
#define ADEC_MAX_ACR_PCM_SIZE			(4096)				///< 185.760 mseconds ACR PCM audio data

//define DSP Ready timeout value
#define WAIT_DSP0_READY_TIMEOUT			500		// Wait 500 ms for DSP0 to Ready
#define WAIT_DSP1_READY_TIMEOUT			500		// Wait 500 ms for DSP1 to Ready
#define WAIT_MONOTOR_TASK_TIMEOUT		200		// Wait 200 ms for Monitor Task
#define MONOTOR_TASK_PRINT_TIME			((1000/WAIT_MONOTOR_TASK_TIMEOUT)+1)	// 1.xx seconds
#define DSP_AUTO_RECOVERY_COUNT			  3		// 3 : 200 * 3 = 600 ms for ESD recovery, 2011.11.02 after reset, audio outputs 200ms.
												//QE Spec. is 1 seconds.

//define DSP operation delay
#define DSP_STOP_DELAY_10MS				10		// Wait 10 ms for DSP0 to stop
#define DSP_SRC_DELAY_30MS				30		// Wait 30 ms for DSP0 to set SRC registers.

//define mixer initial delay
#define FADE_IN_OUT_MIX_DELAY_0MS		0		// 0ms
#define FADE_IN_OUT_MIX_DELAY_115MS		5120	// 115ms


//define DSP Memory Flush Iteration
#define	ADEC_DSP_FLUSH_ITERATION		10		//loop per 10 us

/* In DVB mode, calls 30ms and in ATSC mode, calls 20ms */
#define DEBUG_MSG_PRINT_TIME_1S			35		/* DVB mode */
#define DEBUG_MSG_PRINT_TIME_2S			70
#define DEBUG_MSG_PRINT_TIME_3S			105
#define DEBUG_MSG_PRINT_TIME_4S			140
#define DEBUG_MSG_PRINT_TIME_5S			175
#define DEBUG_MSG_PRINT_TIME_10S		350
#define DEBUG_MSG_PRINT_TIME			DEBUG_MSG_PRINT_TIME_10S
#define DEBUG_MSG_PRINT_TIME_HALF		(DEBUG_MSG_PRINT_TIME * 2)

//Define Debug Print
#define ADEC_OPT_USE_DEBUG          	1
#define ADEC_OPT_USE_ASSERT         	1

#if (ADEC_OPT_USE_DEBUG == 1)
#define ADEC_DEBUG_PRINT            	ADEC_PRINT
#define ADEC_DEBUG_LOW             		ADEC_DEBUG
#define ADEC_DEBUG_ERR              	ADEC_ERROR
#else
#define ADEC_DEBUG_PRINT(...)
#define ADEC_DEBUG_LOW(...)
#define ADEC_DEBUG_ERR(...)
#endif

#if (ADEC_OPT_USE_ASSERT == 1)
#define ADEC_ASSERT(_COND, _RETVAL, ...)    if((_COND) == 0){ADEC_DEBUG_ERR(__VA_ARGS__);return _RETVAL; }
#else
#define ADEC_ASSERT(...)
#endif


/// No Error
#define ADEC_ERR_NONE           0
/// Some parameter, should not be null, is null.
#define ADEC_ERR_NULL_PARAM     -1
/// Not specified. But error.
#define ADEC_ERR_GENERIC        -2
/// Not implemented yet.
#define ADEC_ERR_NOT_IMPLMENTED -3
/// Data is broken. Check if some module overwrite data area or current module has bug.
#define ADEC_ERR_DATA_CORRUPTED -4
/// Argument range is not acceptible
#define ADEC_ERR_ARG_RANGE      -5
/// Fail to acquire or releas mutex
#define ADEC_ERR_MUTEX          -6
/// Not enough memory
#define ADEC_ERR_NOT_ENOUGH_MEM -7
/// No data remain to process. Buffer underflow.
#define ADEC_ERR_UNDERFLOW      -200
/// Not enough buffer space to store data. Buffer overflow.
#define ADEC_ERR_OVERFLOW       -201
/// Has no permission for current operation.
#define ADEC_ERR_PERMISSION     -300

#define SPDIF_ES_OUT_DTO_CLOCK_TYPE		2
#define SPDIF_ES_OUT_DTO_CLOCK_NON_HDMI	0
#define SPDIF_ES_OUT_DTO_CLOCK_HDMI		1
#define SPDIF_ES_OUT_FREQ_NUMBER	3
#define SPDIF_ES_OUT_FREQ_32KHZ		0
#define SPDIF_ES_OUT_FREQ_44_1KHZ	1
#define SPDIF_ES_OUT_FREQ_48KHZ		2

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum {
	AOUT_DEVICE_NONE = 0,
	AOUT_DEVICE_DSP,
	AOUT_DEVICE_ARM,
	AOUT_DEVICE_MAX_NUM
}AOUT_DEVICE_TYPE;

typedef enum
{
	AOUT_SAMPLE_RATE_UNKNOWN   = 0,    /**< 0 Unkown  */
	AOUT_SAMPLE_RATE_8000	   = 1,    /**< 1 8k	  */
	AOUT_SAMPLE_RATE_11025, 		   /**< 2 11.025k */
	AOUT_SAMPLE_RATE_12000, 		   /**< 3 12k	  */
	AOUT_SAMPLE_RATE_16000, 		   /**< 4 16k	  */
	AOUT_SAMPLE_RATE_22050, 		   /**< 5 22.050k */
	AOUT_SAMPLE_RATE_24000, 		   /**< 6 24k	  */
	AOUT_SAMPLE_RATE_32000, 		   /**< 7 32k	  */
	AOUT_SAMPLE_RATE_44100, 		   /**< 8 44.1k   */
	AOUT_SAMPLE_RATE_48000, 		   /**< 9 48k	  */
	AOUT_SAMPLE_RATE_MAX
} AOUT_SAMPLE_RATE_TYPE;

typedef enum
{
	AOUT_CHANNEL_MONO		   = 0,    /**< 0 Mono data 				 */
	AOUT_CHANNEL_STEREO,			   /**< 1 Stereo data				 */
	AOUT_CHANNEL_DUAL,				   /**< 2 Dual channel (stereo) data */
	AOUT_CHANNEL_MAX
} AOUT_CHANNEL_TYPE;

typedef enum
{
   WRITER_STATE_OPEN = 0,
   WRITER_STATE_CLOSE,
   WRITER_STATE_INIT,
   WRITER_STATE_RUN,
   WRITER_STATE_UNDERFLOW,
   WRITER_STATE_OVERFLOW,
   WRITER_STATE_FLUSH,
} WRITER_STATE_TYPE;

typedef enum
{
	WRITER_RET_OPEN_OK = 0,
	WRITER_RET_CLOSE_OK,
	WRITER_RET_INIT_OK,
	WRITER_RET_WRITE_OK,
	WRITER_RET_FLUSH_OK,
	WRITER_RET_UNDERFLOW,
	WRITER_RET_OVERFLOW,
	WRITER_RET_ERROR,
	WRITER_RET_UNKNOWN
} WRITER_RET_TYPE;

typedef struct
{
	UINT32 u32StatusBuffName[2];
	UINT32 u32freeSize;
	UINT32 u32usedSize;
}KDRV_ADEC_STATUS_REG_T;


typedef struct
{
	UINT32 u32StatusBuffCnt;
	UINT32 rsvd;
	UINT32 version;
	UINT8	hour;
	UINT8	minute;
	UINT8	sec;
	UINT8	centiSec;
	//UINT32 rsvd[3];
	KDRV_ADEC_STATUS_REG_T stStatusReg[1];
}KDRV_ADEC_STATUS_T;

typedef struct
{
	char					moduleName[16];
	unsigned int			mips;
	unsigned int			cycle;
	unsigned int			max;
	unsigned int			min;
}KDRV_ADEC_PROFILE_NODE_T;

typedef struct
{
	int							moduleCount;
	int							mipsSum;
	int							reserved[2];
	KDRV_ADEC_PROFILE_NODE_T	reportNode[1];
}KDRV_ADEC_PROFILE_REPORT_T;

/**
 * ADEC Index Table format for Fileplay lipsync.
 * adec ioctl parameter.
 * @see ADEC_IOW_START_AUDIO_CLIP
*/
typedef struct LX_ADEC_AUI_INFO{
	UINT64					timestamp;		///< The DTS(Decoding Time Stamp) value of the media file content
	UINT64					dtsGstc;		///< The DTS(Decoding Time Stamp) value of the GSTC HW clock matching timestamp
} LX_ADEC_AUI_INFO_T;

typedef struct
{
	UINT64 streamtime;
	UINT64 pts;
//	UINT32 index;
}KDRV_AUI_INFO;

typedef struct
{
	KDRV_AUI_INFO	AUI[CPB_AUI_INDEX_COUNT];

	UINT32			FeedCNT;
	UINT32			FeedIDX;

	UINT32			PrevNOTI;
	UINT32			NewNOTI;
	UINT32			WriteCNT;		//0 ~511  // -> written data count
	UINT32			ReadCNT;   		//0 ~127	  //-> data to read in next read time
	UINT32			FreeCNT;		//0 ~511  // -> written data count
	UINT32			UsedCNT;		//0 ~511  // -> written data count
	UINT32			NotiCNT;		//0 ~511  // -> written data count
}KDRV_ADEC_INDEX_T;

typedef struct
{
	BOOLEAN					bDecStarted[LX_ADEC_DEC_2];		/* Current Audio Decoder Start Status */
	BOOLEAN					bMixStarted[LX_ADEC_MIXER_4];	/* Current Audio Mixer Start Status */
	BOOLEAN					bDecADStarted;					/* Current Audio AD Start Status */
	BOOLEAN					bEncStarted;					/* Current Audio Encoder Start Status */
	BOOLEAN					bInitDone;						/* Current Audio Init Done Status */

	UINT32 					soundEngineMask;				/* Current ADEC Sound Engine Mask */
	UINT32 					hpVolume;						/* Current ADEC HP volume for output gain*/

	LX_ADEC_IN_PORT_T 		adecSource[LX_ADEC_DEC_2];		/* ADEC Source Input Type. */
	LX_ADEC_SRC_TYPE_T		adecType[LX_ADEC_DEC_2];		/* ADEC Source Codec Type */

	LX_ADEC_SAMPLING_FREQ_T adecHdmiInputFreq;				/* ADEC HDMI Input Sampling Frequency */

	LX_ADEC_DRC_MODE_T		drcMode[LX_ADEC_DEC_2];			/* ADEC DRC Mode */
	LX_ADEC_DOWNMIX_MODE_T	downmixMode[LX_ADEC_DEC_2];		/* ADEC Downmix Mode */

	LX_ADEC_SPDIF_MODE_T	spdifMode;						/* ADEC SPDIF Mode */
	BOOLEAN					bSpdifPcm;						/* ADEC SPDIF PCM output Status  */
	BOOLEAN					bSpdifMainOnly;					/* ADEC SPDIF PCM Main Only Mode  */

	LX_ADEC_CLIP_MODE_T		clipStatus[LX_ADEC_DEC_2];		/* Current Audio Clip Status */
	LX_ADEC_CLIP_BUF_T		clipBufStatus[LX_ADEC_DEC_2];	/* Current status of clip play buffer */
	UINT32 					timePaused[LX_ADEC_DEC_2];		/* ADEC time clip paused */
	UINT32 					timePreCBT[LX_ADEC_DEC_2];		/* ADEC time clip pre clock based time */
	UINT32 					timePreSBT[LX_ADEC_DEC_2];		/* ADEC time clip pre stream based time */

#ifdef ENABLE_AAC_PARSER
	void*					aacProbe[LX_ADEC_DEC_2];		/* AAC Codec Probe */
	BOOLEAN					bAacParsed[LX_ADEC_DEC_2];		/* AAC Codec Probe ES Parse Status */
	BOOLEAN					bAacDSIParsed[LX_ADEC_DEC_2];	/* AAC Codec Probe DSI Parse Status */
#endif

	KDRV_ADEC_INDEX_T 		index[LX_ADEC_DEC_2];			/* ADEC index for decoder notification  */

	LX_ADEC_CLIP_MODE_T		mixerStatus[LX_ADEC_MIXER_5];	/* Current Audio Clip Mixer Status */
	UINT32 					mixerIndex[LX_ADEC_MIXER_5];	/* ADEC mixer index for mixer notification(This value is always incresed if not wraparound case  */
}LX_ADEC_KDRV_T;

typedef struct
{
	UINT32                   vdec_dec0_cbt;	// 0x08F0
	UINT32                   vdec_dec0_sbt;	// 0x08F4
	UINT32                   vdec_dec1_cbt;	// 0x08F8
	UINT32                   vdec_dec1_sbt;	// 0x08FC
} VDEC_LIPSYNC_REG_T;

/* define WMA and WMAPro codec parameters */
/****************************************************************
*	Format Tag
*****************************************************************
*	0x0160: WMA version1
*	0x0161:	WMA v7, v8, v9 (WMA version2)
*	0x0162:	WMA Professional (WMA version3)
*	0x0163:	WMA Lossless
****************************************************************/

/****************************************************************
*	DSI (Decoder Specific Info) of WMA
*****************************************************************
*	WMA1
*	size: 4 bytes
*	Samples Per Block(2bytes) + Encode Options(2bytes)
*****************************************************************
*	WMA2
*	size: 10 bytes
*	Samples Per Block(4bytes) + Encode Options(2bytes) + Super Block Align(4bytes)
*****************************************************************
*	WMA3 - WMA Pro(v10, Lossless)
*	size: 18 bytes
*	Samples Per Block(2bytes) + Channel Mask(4bytes) + Advanced Encode Options2(8bytes) + Encode Option(2bytes) + Advanced Encode Option(2bytes)
*	// Valid Bits Per Sample(2bytes) + Channel Mask(4bytes) + reserved1(4bytes) + reserved2(4bytes) + Encode Options(2bytes) + reserved3(2bytes)
****************************************************************/
#if 0
typedef struct _WMA1_ENCODE_DATA
{
	unsigned short		samplePerBlock;			///< sample per block (2bytes)
	unsigned short		encodeOptions;			///< encode options (2bytes)
} WMA1_ENCODE_DATA;
typedef struct _WMA2_ENCODE_DATA
{
	unsigned int		samplePerBlock;			///< sample per block (4bytes)
	unsigned short		encodeOptions;			///< encode options (2bytes)
	unsigned int		superBlockAlign;		// super block align (4bytes)
} WMA2_ENCODE_DATA;
typedef struct _WMA3_ENCODE_DATA
{
	unsigned short		samplePerBlock;			///< sample per block (2bytes)
	unsigned int		channelMask;			///< channel mask (4bytes)
	unsigned long long	advancedEncodeOptions2;	///< advanced encode options 2 (8bytes)
	unsigned short		encodeOptions;			///< encode options (2bytes)
	unsigned short		advancedEncodeOptions;	///< advanced encode options (2bytes)
} WMA3_ENCODE_DATA;
#endif

#define LENGTH_OF_DSI_FOR_WMA2			10
#define LENGTH_OF_DSI_FOR_WMA3			18

#define WMA_FORMAT_TAG					0x161
#define WMAPRO_FORMAT_TAG				0x162
#define WMALOSSLESS_FORMAT_TAG			0x163

#define ADEC_READ16_LE(buf)		(buf)[0] | ((UINT16)(buf)[1]<<8)
#define ADEC_READ24_LE(buf)		(buf)[0] | ((UINT32)(buf)[1]<<8) | ((UINT32)(buf)[2]<<16)
#define ADEC_READ32_LE(buf)		(buf)[0] | ((UINT32)(buf)[1]<<8) | ((UINT32)(buf)[2]<<16) | ((UINT32)(buf)[3]<<24)
#define ADEC_READ64_LE(buf)		ADEC_READ32_LE((buf)) | ((UINT64)ADEC_READ32_LE(&(buf)[4]))<<32

#define ADEC_READ16_BE(buf)		((UINT16)(buf)[0]<<8) | (buf)[1]
#define ADEC_READ24_BE(buf)		((UINT32)(buf)[0]<<16) | ((UINT32)(buf)[1]<<8) | (buf)[2]
#define ADEC_READ32_BE(buf)		((UINT32)(buf)[0]<<24) | ((UINT32)(buf)[1]<<16) | ((UINT32)(buf)[2]<<8) | (buf)[3]
#define ADEC_READ64_BE(buf)		(((UINT64)ADECF_READ32_BE((buf)))<<32) | ADEC_READ32_BE(&(buf)[4])


/* define RA8 codec parameters */
#define MIN_LENGTH_OF_DSI_FOR_RA8			11
#define MIN_LENGTH_OF_DSI_FOR_RA8_V13		16
#define RA8_MAJOR_V1						 1
#define RA8_MINOR_V3						 3


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
/* adec global info */
extern LX_ADEC_KDRV_T	g_adec_kdrv;


/*----------------------------------------------------------------------------------------
	Function Prototype Declaration
----------------------------------------------------------------------------------------*/

int ADEC_L9_InitModule ( LX_ADEC_INIT_TYPE_T eInitType );
int ADEC_L9_SetEvent ( LX_ADEC_EVENT_TYPE_T eEventType );
int ADEC_L9_ResetEvent ( LX_ADEC_EVENT_TYPE_T eEventType );
int ADEC_L9_GetEvent ( LX_ADEC_EVENT_INFO_T *pstEventInfo );
int ADEC_L9_GetBufferInfo ( LX_ADEC_BUF_INFO_T *pstBufferInfo );
int ADEC_L9_ResetAADModule ( void );
int ADEC_L9_SetAADBitShift ( UINT32 ui32BitShift );

int ADEC_L9_GetDecodingInfo ( LX_ADEC_DEC_INFO_T *pstDecodingInfo );
int ADEC_L9_SetSource ( LX_ADEC_SET_SOURCE_T stSetSource );
int ADEC_L9_StartDecoding ( LX_ADEC_START_DEC_T stStartInfo );
int ADEC_L9_StopDecoding ( LX_ADEC_STOP_DEC_T stStopInfo );
int ADEC_L9_GetESExistence ( LX_ADEC_ES_EXIST_T *pstEsExist );
int ADEC_L9_GetESInfo ( LX_ADEC_GET_ES_INFO_T *pstESInfo );
int ADEC_L9_StartADDecoding ( LX_ADEC_SRC_TYPE_T eSrcType );
int ADEC_L9_StopADDecoding ( void );
int ADEC_L9_SetSPKOutMode ( LX_ADEC_OUTPUT_MODE_T eSPKOutMode );
int ADEC_L9_SetDecoderMode ( LX_ADEC_DECODER_MODE_T eDecoderMode );

int ADEC_L9_SetVolume ( UINT32 ui32Volume );
int ADEC_L9_SetADVolume ( UINT32 ui32Volume );
int ADEC_L9_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol );
int ADEC_L9_SetOutputPortMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute );
int ADEC_L9_SetMixVolume ( LX_ADEC_MIX_VOL_T stMixVolume );
int ADEC_L9_SetDAMPVolume ( UINT8 ui8Volume );
int ADEC_L9_SetDAMPMute ( BOOLEAN bMuteFlag );
int ADEC_L9_SetMixChannelVolume ( LX_ADEC_MIX_CH_VOL_T stMixChVolume );

int ADEC_L9_SetSPDIFOutputType ( LX_ADEC_SPDIF_MODE_T eSPDIFMode );
int ADEC_L9_SetSPDIFScmsCtrl ( LX_ADEC_SPDIF_SCMS_T eSPDIFScmsCtrl );
int ADEC_L9_SetSPDIFVolume ( UINT32 ui32Volume );
int ADEC_L9_SetSPDIFMute ( BOOLEAN bMuteFlag );

int ADEC_L9_EnableAVLipSync ( LX_ADEC_SYNC_T i_eAVLipSync );
int ADEC_L9_SetI2SOutDelayTime ( UINT32 ui32DelayinMS );
int ADEC_L9_SetDACOutDelayTime ( UINT32 ui32DelayinMS );
int ADEC_L9_SetSPDIFDelayTime ( UINT32 ui32DelayinMS );
int ADEC_L9_SetTrickState ( LX_ADEC_TRICK_MODE_T	eTrickMode );
int ADEC_L9_GetClockInfo ( LX_ADEC_CLOCK_INFO_T *pstClockInfo );

int ADEC_L9_SetBalance ( UINT8 ui8BalanceVal );
int ADEC_L9_SetBASS ( UINT8 ui8BassVal );
int ADEC_L9_SetTreble ( UINT8 ui8TrebleVal );

int ADEC_L9_SESetFunction ( LX_ADEC_SE_FN_PARAM_T *pstSeFnParam );
int ADEC_L9_SEGetFunction ( LX_ADEC_SE_FN_PARAM_T *pstSeFnParam );

int ADEC_L9_FeedAudioClip ( LX_ADEC_CLIP_MEM_INFO_T stMemInfo );
int ADEC_L9_StartAudioClip ( LX_ADEC_CLIP_START_INFO_T stStartInfo );
int ADEC_L9_StopAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L9_PauseAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L9_ResumeAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L9_FlushAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L9_ResetAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L9_SetAVClockControl ( LX_ADEC_AV_CLOCK_INFO_T	i_stAVClockInfo );


#ifdef ENABLE_AAC_PARSER
int ADEC_L9_StartAacAudioClip ( LX_ADEC_AAC_START_INFO_T stAacStartInfo );
#endif	//#ifdef ENABLE_AAC_PARSER

int ADEC_L9_GetEncBufInfo ( LX_ADEC_ENC_BUF_T *pstEncBufInfo );
int ADEC_L9_SetEncParam ( LX_ADEC_ENC_PARAM_T stEncParam );
int ADEC_L9_GetEncParam ( LX_ADEC_ENC_PARAM_T *pstEncParam );
int ADEC_L9_StartEncoding ( LX_ADEC_ENC_START_T stStartInfo );
int ADEC_L9_StopEncoding ( LX_ADEC_ENC_STOP_T stStopInfo );
int ADEC_L9_GetEncodingInfo ( LX_ADEC_ENC_INFO_T *pstEncodingInfo );


int ADEC_L9_GetPcmBufInfo ( LX_ADEC_PCM_BUF_T *pstPcmBufInfo );
int ADEC_L9_GetCapturingInfo ( LX_ADEC_CAP_INFO_T *pstCapturingInfo );


int ADEC_L9_ReadAndWriteReg ( LX_ADEC_REG_INFO_T *pstRegInfo );
int ADEC_L9_CustomIpc(LX_ADEC_CUSTOM_IPC_T* pstCustomIpc);
int ADEC_L9_ShowStausRegister ( void );


int ADEC_L9_Monitor_Task( void *i_pvParam );

irqreturn_t adec_l9_interrupt0 (int irq, void *dev_id, struct pt_regs *regs);
irqreturn_t adec_l9_interrupt1 (int irq, void *dev_id, struct pt_regs *regs);

int ADEC_L9_RunSuspend(void);
int ADEC_L9_RunResume(void);


int ADEC_L9_LoadDSP0Codec ( LX_ADEC_SRC_TYPE_T i_eSrcType );
int ADEC_L9_LoadDSP1Codec ( void );

int ADEC_L9_InitReg ( void );
int ADEC_L9_InitRegForCheckbit ( void );

int ADEC_L9_SetReset ( UINT8 ui8ResetFlag );
int ADEC_L9_ClearReset ( UINT8 ui8ResetFlag );

void ADEC_L9_WaitForDSP0Ready ( UINT64 startTick );
void ADEC_L9_SignalDSP0Ready ( void );
void ADEC_L9_WaitForDSP1Ready ( UINT64 startTick );
void ADEC_L9_SignalDSP1Ready ( void );

void ADEC_L9_SetDSP1IntrEvent(void);

int ANALOG_ReadRegister(UINT8 slaveAddr, UINT8 regAddr, UINT8 *value);
int ANALOG_WriteRegister(UINT8 slaveAddr, UINT8 regAddr, UINT8 value);


int MIXED_IP_ADEC_L9_InitClockRegisters( void );
int MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_SAMPLING_FREQ_T ui32SrcFreq, LX_ADEC_CLOCK_SRC_T ui8AdecClockSrc );
int MIXED_IP_ADEC_L9_SetPcmClockRate ( LX_ADEC_SAMPLING_FREQ_T ui32SrcFreq, UINT32 ui32PcmClock );
int MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq ( LX_ADEC_SAMPLING_FREQ_T ui32SamplingFreq, LX_ADEC_SPDIF_DTO_RATE_T stSpdifDtoRate );

int MIXED_IP_ADEC_L9_InitCodecRegisters ( void );
int MIXED_IP_ADEC_L9_PowerControl ( LX_ADEC_POWER_BLOCK_T block, LX_ADEC_POWER_MODE_T mode );
int MIXED_IP_ADEC_L9_StartCodec ( void );
int MIXED_IP_ADEC_L9_StopCodec ( void );

int MIXED_IP_ADEC_L9_SetADCPortNumber ( UINT8 ui8PortNum );
int MIXED_IP_ADEC_L9_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol );
int MIXED_IP_ADEC_L9_SetDACOutMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute );


int  ADEC_L9_Create_CPBM_Interbuf_Writer(void);
int  ADEC_L9_Create_CPBA_Interbuf_Writer(void);
int  ADEC_L9_Create_MIX0_Interbuf_Writer(void);
int  ADEC_L9_Create_MIX1_Interbuf_Writer(void);
int  ADEC_L9_Create_MIX2_Interbuf_Writer(void);
int  ADEC_L9_Create_MIX3_Interbuf_Writer(void);
int  ADEC_L9_Create_AENC_Interbuf_Reader(void);
int  ADEC_L9_Create_APCM_Interbuf_Reader(void);
int  ADEC_L9_Create_AACR_Interbuf_Reader(void);


#ifdef ENABLE_DAMP_VOLUME_CONTROL
int ADEC_NTP7400_Init (void);
int ADEC_NTP7400_SetMute (BOOLEAN bMuteOnOff);
int ADEC_NTP7400_SetVolume (UINT8 volume);
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _ADEC_COREDRV_L9_H_ */

/** @} */
