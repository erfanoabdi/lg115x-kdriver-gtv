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
 *  @author		Sudhaher (kanthiraj.s@lge.com)
 *  @version	1.0
 *  @date		2010.02.23
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

#ifndef	_ADEC_COREDRV_L8_H_
#define	_ADEC_COREDRV_L8_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "adec_drv.h"
#include "adec_cfg.h"
#include "adec_kapi.h"
#include "adec_reg_l8.h"
#include <linux/interrupt.h>    /* For isr function */
#include <asm/irq.h>			/* For isr function */

#include "../sys/ctop_regs.h"	//for CTOP CTRL Reg. map

#include <mach/hardware.h> // For Register base address

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
#define ENABLE_ADEC_JTAG			//Enable ADEC DSP JTAG


//#define ENABLE_INTERVAL_CEHCK		// Enable check interval time of Decode Done interrupt
#ifdef ENABLE_INTERVAL_CEHCK
#define MAX_INTERVAL_COUNT		1024
#endif

//#define ENABLE_SCART1_MIC_PORT	//Enable Mixed IP SCART1 and MIC Input Port
#define ENABLE_HPDRV_PORT			//Enable Mixed IP HPDRV Output Port
#define ENABLE_WOOFER_I2S_FOR_HPDRV	//Enable ADEC DSP Woofer I2S for HPDRV Output Port
#define ENABLE_DSP_SW_SRC			//Enable DSP S/W SRC(Samping Rate Converter)

//define register address
extern LX_ADEC_REG_CFG_T 		gstAdecRegCfg[];
extern LX_ADEC_REG_CFG_T 		*pstAdecRegCfg;

//define memory map
extern LX_ADECDEV_MEM_CFG_S_T 	gstMemCfgADEC[];
extern LX_ADECDEV_MEM_CFG_S_T 	*pstMemCfgADEC;

#ifdef USE_QEMU_SYSTEM
extern	LX_ADEC_REG_CFG_T		g_stQemuADECReg;
#endif /* USE_QEMU_SYSTEM */


//Extensa DSP memory address  and silkroad mask
#define DSP_OFFSET5_MASK_UNDER_256MB		0x60000000
#define DSP_OFFSET6_MASK_UNDER_256MB		0x60000000
#define DSP_OFFSET5_MASK_UPPER_256MB		0x50000000
#define DSP_OFFSET6_MASK_UPPER_256MB		0x60000000

// define CPBM Index Table for EMP Lip Sync
#define MAX_SIZE_OF_CPBM_INDEX_TABLE 		0x20000		//1024 * 32 * 4  bytes(128 KB) : 4096 tables
//#define MAX_SIZE_OF_CPBM_INDEX_TABLE		0x02000 	//256 * 32  bytes(8 KB) : 256 tables

// LG Sound Engine Paramter Address define : DPB - 1024 * 2
#define ADDR_LGSE_PARAM_BASE			pstMemCfgADEC->dpbm_memory_base	//DPBM base up to 2KB
#define ADDR_LGSE_AUTOVOLUME_PARAM		(0)								//0x0000
#define ADDR_LGSE_CV2_PARAM				(ADDR_LGSE_AUTOVOLUME_PARAM + LX_CV_AUTOVOLUME_PARAM_NUM)	//0x005C
#define ADDR_LGSE_UEQ_PARAM				(ADDR_LGSE_CV2_PARAM + LX_CV_CV2_PARAM_NUM)			//0x00C0
#define ADDR_LGSE_UEQ2_PARAM			(ADDR_LGSE_UEQ_PARAM + LX_CV_UEQ_PARAM_NUM)			//0x0110
#define ADDR_LGSE_DEQ_PARAM				(ADDR_LGSE_UEQ2_PARAM + LX_CV_UEQ2_PARAM_NUM)		//0x0160
#define ADDR_LGSE_IFNSND_PARAM			(ADDR_LGSE_DEQ_PARAM + LX_CV_DEQ_PARAM_NUM)			//0x0338
#define ADDR_LGSE_INLEV_PARAM			(ADDR_LGSE_IFNSND_PARAM + LX_CV_IFNSND_PARAM_NUM)	//0x0394
#define TOTAL_SIZE_OF_LGSE_PARAM		(LX_CV_AUTOVOLUME_PARAM_NUM +	\
										 LX_CV_CV2_PARAM_NUM + 	\
										 LX_CV_UEQ_PARAM_NUM +	\
										 LX_CV_UEQ2_PARAM_NUM +	\
										 LX_CV_DEQ_PARAM_NUM +	\
										 LX_CV_IFNSND_PARAM_NUM +	\
										 LX_CV_INLEV_PARAM_NUM)
#define MAX_SIZE_OF_LGSE_PARAM			 0x800	//1024 *2  bytes(2KB)  -> moved to adec-kapi.h


//define reset module
#define ADEC_OPTION_DSP0				0x0		//DSP0 only
#define ADEC_OPTION_DSP1				0x1		//DSP1 only
#define ADEC_OPTION_DSP_DEBUG			0x2		//DSP0,1 debug only
#define ADEC_OPTION_AAD					0x3		//AAD Module only
#define ADEC_OPTION_MIXEDIP				0x4		//Mixed IP only
#define ADEC_OPTION_DAC					0x5		//Mixed IP DAC only
#define ADEC_OPTION_ADC					0x6		//Mixed IP ADC only
#define ADEC_OPTION_HPDRV				0x7		//Mixed IP HPDRV only
#define ADEC_OPTION_OTHERS				0x8		//Except DSP and Power-down
#define ADEC_OPTION_SPDIF				0x9		//SPDIF reset
#define ADEC_OPTION_ALL					0xA		//All reset

//define ADEC Reg. Write Retry Number
#define	ADEC_REG_WRITE_RETRY			10	//10 times for loop

//define for constant value
#define ADEC_CODEC_NAME_SIZE			30

//define DSP Ready timeout value
#define WAIT_DSP0_READY_TIMEOUT			50	// Wait 50 ms for DSP0 to Ready
#define WAIT_DSP1_READY_TIMEOUT			20	// Wait 20 ms for DSP1 to Ready

//define DSP Memory Flush Iteration
#define	ADEC_DSP_FLUSH_ITERATION		10	//10 us for loop

//define DSP Stop Iteration
#define	ADEC_DSP_STOP_ITERATION			30	//30 ms for loop


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern volatile AUD_REG_L8_T	*g_pstADECReg_l8;
extern volatile AUD_REG_L8_T	*gpRealRegAUD_l8;
extern AUD_REG_L8_T				*gpRegAUD_l8;



/*----------------------------------------------------------------------------------------
	Function Prototype Declaration
----------------------------------------------------------------------------------------*/

int ADEC_L8_InitModule ( LX_ADEC_INIT_TYPE_T eInitType );
int ADEC_L8_SetEvent ( LX_ADEC_EVENT_TYPE_T eEventType );
int ADEC_L8_ResetEvent ( LX_ADEC_EVENT_TYPE_T eEventType );
int ADEC_L8_GetEvent ( LX_ADEC_EVENT_INFO_T *pstEventInfo );
int ADEC_L8_GetMMapInfo ( LX_ADEC_MMAP_INFO_T *pstMMapInfo );
int ADEC_L8_GetBufferInfo ( LX_ADEC_BUF_INFO_T *pstBufferInfo );
int ADEC_L8_ResetAADModule ( void );


int ADEC_L8_GetDecodingInfo ( LX_ADEC_DEC_INFO_T *pstDecodingInfo );
int ADEC_L8_SetSource ( LX_ADEC_SET_SOURCE_T	stSetSource );
int ADEC_L8_StartDecoding ( LX_ADEC_SRC_TYPE_T eSrcType );
int ADEC_L8_StopDecoding ( void );
int ADEC_L8_GetESExistence ( BOOLEAN *pbESExistFlag );
int ADEC_L8_GetESInfo ( LX_ADEC_ES_INFO_T *pstESInfo );
int ADEC_L8_StartADDecoding ( LX_ADEC_SRC_TYPE_T eSrcType );
int ADEC_L8_StopADDecoding ( void );

int ADEC_L8_SetSPKOutMode ( LX_ADEC_SPK_OUTPUT_MODE_T eSPKOutMode );
int ADEC_L8_SetVolume ( UINT32 ui32Volume );
int ADEC_L8_SetADVolume ( UINT32 ui32Volume );
int ADEC_L8_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol );
int ADEC_L8_SetOutputPortMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute );
int ADEC_L8_SetSPDIFOutputType ( LX_ADEC_SPDIF_MODE_T eSPDIFMode );
int ADEC_L8_SetSPDIFScmsCtrl ( LX_ADEC_SPDIF_SCMS_T eSPDIFScmsCtrl );
int ADEC_L8_SetSPDIFVolume ( UINT32 ui32Volume );
int ADEC_L8_SetSPDIFMute ( BOOLEAN bMuteFlag );

int ADEC_L8_EnableAVLipSync ( UINT8 ui8AVLipSyncFlag );
int ADEC_L8_SetI2SOutDelayTime ( UINT32 ui32DelayinMS );
int ADEC_L8_SetDACOutDelayTime ( UINT32 ui32DelayinMS );
int ADEC_L8_SetSPDIFDelayTime ( UINT32 ui32DelayinMS );
int ADEC_L8_SetTrickState ( LX_ADEC_TRICK_MODE_T	eTrickMode );
int ADEC_L8_GetAndSetClockInfo ( LX_ADEC_CLOCK_INFO_T *pstClockInfo );


int ADEC_L8_SetBalance ( UINT8 ui8BalanceVal );
int ADEC_L8_SetBASS ( UINT8 ui8BassVal );
int ADEC_L8_SetTreble ( UINT8 ui8TrebleVal );

int ADEC_L8_CVSetMode ( LX_ADEC_CV_SET_MODE_T stCVSetMode );
int ADEC_L8_CVSetCV ( LX_ADEC_CV_SET_CV_T stCVSetCV );
int ADEC_L8_CVSetSurround ( LX_ADEC_CV_SET_SURROUND_T stCVSetSurround );
int ADEC_L8_CVSetUEQ ( LX_ADEC_CV_SET_UEQ_T *pstCVSetUEQ );
int ADEC_L8_CVSetVolume ( UINT8 ui8CVSetVolume );
int ADEC_L8_CVSetAVL ( LX_ADEC_CV_SET_AVL_T	stSetAVL );
int ADEC_L8_CVSetBASS ( LX_ADEC_CV_SET_BASS_EQ_T *pstCVSetBassEQ );
int ADEC_L8_CVSetInLevel ( LX_ADEC_CV_SET_INLEV_T stCVSetInLev );
int ADEC_L8_CVGetInLevel ( LX_ADEC_CV_GET_INLEV_T *pstui32SoundLevel );
int ADEC_L8_CVGetParam ( LX_ADEC_CV_GET_PARAM_T *pstCVGetParam );

int ADEC_L8_LoadAudioClip ( LX_ADEC_CLIP_MEM_INFO_T stMemInfo );
int ADEC_L8_StartAudioClip ( LX_ADEC_CLIP_START_INFO_T stStartInfo );
int ADEC_L8_StopAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L8_PauseAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L8_ResumeAudioClip ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int ADEC_L8_DeleteAudioClip ( LX_ADEC_CLIP_DEL_INFO_T stClipDeleteInfo );
int ADEC_L8_StartAacAudioClip ( LX_ADEC_AAC_START_INFO_T stAacStartInfo );

int ADEC_L8_ReadAndWriteReg ( LX_ADEC_REG_INFO_T *pstRegInfo );

int ADEC_L8_Monitor_Task( void *i_pvParam );

irqreturn_t adec_l8_interrupt0 (int irq, void *dev_id, struct pt_regs *regs);
irqreturn_t adec_l8_interrupt1 (int irq, void *dev_id, struct pt_regs *regs);

int ADEC_L8_RunSuspend(void);
int ADEC_L8_RunResume(void);


int ADEC_GetCodecSrcType ( LX_ADEC_SRC_TYPE_T *peSrcType );
int ADEC_LoadCodec ( LX_ADEC_SRC_TYPE_T i_eSrcType );
int ADEC_InitReg ( void );
int ADEC_InitRegForCheckbit ( void );
int ADEC_InitSEParams ( void );

int ADEC_SetReset ( UINT8 ui8ResetFlag );
int ADEC_ClearReset ( UINT8 ui8ResetFlag );

void ADEC_WaitForDSP0Ready ( void );
void ADEC_SignalDSP0Ready ( void );
void ADEC_WaitForDSP1Ready ( void );
void ADEC_SignalDSP1Ready ( void );

void ADEC_WaitForEvent ( void );
void ADEC_SignalEvent ( void );

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _ADEC_COREDRV_L8_H_ */

/** @} */
