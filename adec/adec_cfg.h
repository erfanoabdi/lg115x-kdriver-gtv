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
 *  main configuration file for adec device
 *	adec device will teach you how to make device driver with new platform.
 *
 *  author		jongsang.oh (jongsang.oh@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

#ifndef	_ADEC_CFG_H_
#define	_ADEC_CFG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"
#ifdef INCLUDE_L8_CHIP_KDRV
#include "l8/adec_reg_l8.h"
#else
#include "l9/adec_reg_l9.h"
#endif

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	ADEC_MODULE			"adec"
#define ADEC_MAX_DEVICE		1


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
/**
 * Audio Decoder Number.
 * Audio decoder number is defined for dual decoding.
 */
typedef  enum
{
	LX_ADEC_DEC_0	= 0,		///< Audio Decoder 0
	LX_ADEC_DEC_1	= 1,		///< Audio Decoder 1
	LX_ADEC_DEC_2	= 2			///< Audio Decoder Max : 2
} LX_ADEC_DECODER_T;

/**
 * Audio Mixer Number.
 * Audio mixer number is defined for mixing.
 */
typedef  enum
{
	LX_ADEC_MIXER_0		= 0,	///< Audio Mixer 0
	LX_ADEC_MIXER_1		= 1,	///< Audio Mixer 1
	LX_ADEC_MIXER_2		= 2,	///< Audio Mixer 2
	LX_ADEC_MIXER_3		= 3,	///< Audio Mixer 3
	LX_ADEC_MIXER_4		= 4,	///< Audio Mixer 4(This mixer is used LLB for low delay mode..)
	LX_ADEC_MIXER_5 	= 5,	///< Audio Mixer Max : 5(1 channel is support stereo mode.)
} LX_ADEC_MIXER_T;

/**
 * ADEC MonitorTask Message Status
 * This enumeration describes the ADEC monitor task status.
 *
 */
typedef  enum
{
	LX_ADEC_TASK_NONE_MSG		= 0x000,	///< None
	LX_ADEC_TASK_RESET_DSP0		= 0x001,	///< Reset DSP0 with exception handler interrupt
	LX_ADEC_TASK_RESET_DSP1		= 0x002,	///< Reset DSP1 with exception handler interrupt
	LX_ADEC_TASK_SET_SPDIF		= 0x004,	///< Set SPDIF clock for DEC0
	LX_ADEC_TASK_USE_GSTC		= 0x008,	///< Use GSTC for MHEG TP stream lipsync
	LX_ADEC_TASK_SET_DTO_RATE	= 0x010,	///< Set SPDIF DTO Clock Rate
	LX_ADEC_TASK_SET_SPDIF_DEC1	= 0x020,	///< Set SPDIF clock for DEC1
	LX_ADEC_TASK_RESET_CLOCK	= 0x040,	///< Reset a I2S and SPDIF clock registers
	LX_ADEC_TASK_SET_PCM_RATE	= 0x080,	///< Set D-AMP PCM Clock Rate
} LX_ADEC_TASK_MSG_T;

/**
 * ADEC Play Mode Type.
 * ADEC Play Mode type is defined.
 *
 */
typedef  enum
{
	LX_ADEC_PLAY_UNKNOWN	= 0,	//ADEC plat is unknown
	LX_ADEC_PLAY_START		= 1,	//ADEC Play is started.
	LX_ADEC_PLAY_PAUSE		= 2,	//ADEC Play is paused.
	LX_ADEC_PLAY_STOP		= 3		//ADEC Play is stopped.
} LX_ADEC_PLAY_MODE_T;

/**
 * ADEC Clock Source Type.
 * Audio Clock Source type is defined.
 *
 */
typedef  enum
{
	LX_ADEC_CLOCK_UNKNOWN	= 0,	//Unknown
	LX_ADEC_CLOCK_DTV		= 1,	//DTV to SPK(System)
	LX_ADEC_CLOCK_ATV		= 2,	//ATV to SPK
	LX_ADEC_CLOCK_HDMI		= 3,	//HDMI to SPK
	LX_ADEC_CLOCK_ADC		= 4 	//ADC to SPK
} LX_ADEC_CLOCK_SRC_T;

/**
 * ADEC SPDIF DTO Clock Rate Type.
 * Audio SPDIF DTO Clock Rate Type is defined.
 *
 */
typedef  enum
{
	LX_ADEC_SPDIF_DTO_UNKNOWN			= 0,	//Unknown
	LX_ADEC_SPDIF_DTO_MINUS_19_5SAMPLES	= 1,	//-19.5 Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_12_5SAMPLES	= 2,	//-12.5 Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_10_9SAMPLES	= 3,	//-10.9 Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_9_4SAMPLES	= 4,	//-9.4  Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_7_8SAMPLES	= 5,	//-7.8  Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_6_3SAMPLES	= 6,	//-6.3  Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_4_7SAMPLES	= 7,	//-4.7  Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_3_3SAMPLES	= 8,	//-3.3 Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_1_6SAMPLES	= 9,	//-1.6 Samples Speed
	LX_ADEC_SPDIF_DTO_MINUS_0_7SAMPLES	= 10,	//-0.7 Samples Speed
	LX_ADEC_SPDIF_DTO_NORMAL			= 11,	//Normal Sample Speed
	LX_ADEC_SPDIF_DTO_PLUS_0_7SAMPLES	= 12,	//+0.7 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_1_6SAMPLES	= 13,	//+1.6 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_3_3SAMPLES	= 14,	//+3.3 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_4_7SAMPLES	= 15,	//+4.7 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_6_3SAMPLES	= 16,	//+6.3 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_7_8SAMPLES	= 17,	//+7.8 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_9_4SAMPLES	= 18,	//+9.4 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_10_9SAMPLES	= 19,	//+10.9 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_12_5SAMPLES	= 20,	//+12.5 Samples Speed
	LX_ADEC_SPDIF_DTO_PLUS_19_5SAMPLES	= 21,	//+19.5 Samples Speed
	LX_ADEC_SPDIF_DTO_RATE_NUMBER		= 21	//Number of DTO Rate Step
} LX_ADEC_SPDIF_DTO_RATE_T;

/**
 * ADEC Mixed IP Power Block Mode.
 * ADEC Mixed IP Power Block Mode is defined.
 *
 */
typedef  enum
{
	LX_ADEC_POWER_AAD 		= 0,	//AAD
	LX_ADEC_POWER_ADC		= 1,	//ADC
	LX_ADEC_POWER_HPDRV		= 2,	//HPDRV
	LX_ADEC_POWER_DAC		= 3,	//DAC(SCART)
	LX_ADEC_POWER_MIC		= 4, 	//MIC
	LX_ADEC_POWER_MAX		= 5		//Unknown
} LX_ADEC_POWER_BLOCK_T;

/**
 * ADEC Mixed IP Power Mode.
 * ADEC Mixed IP Power Mode is defined.
 *
 */
typedef  enum
{
	LX_ADEC_POWER_DOWN 		= 0,	//Power-down
	LX_ADEC_POWER_UP		= 1		//Power-up
} LX_ADEC_POWER_MODE_T;


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
ADEC Register configuration.
*/
typedef struct
{
	char*		chip_name;			// 	Chip Name
	UINT32		reg_base_addr;		//	ADEC Register Base Address for Real Chip.
	UINT32		reg_size;			//	ADEC Register Size for Real Chip.
	UINT32		irq0_num;			//	ADEC IRQ 0 Number for Real Chip.
	UINT32		irq1_num;			//	ADEC IRQ 0 Number for Real Chip.
} LX_ADEC_REG_CFG_T;

/**
ADEC Memory Map configuration.
*/
typedef struct
{
#ifdef INCLUDE_L8_CHIP_KDRV
	char* 	fw0_memory_name;
	UINT32	fw0_memory_base;
	UINT32 	fw0_memory_size;
	char* 	dsp0_memory_name;
	UINT32 	dsp0_memory_base;
	UINT32 	dsp0_memory_size;
	char* 	cpbm_memory_name;
	UINT32	cpbm_memory_base;
	UINT32 	cpbm_memory_size;
	char* 	cpba_memory_name;
	UINT32	cpba_memory_base;
	UINT32 	cpba_memory_size;

	char* 	fw1_memory_name;
	UINT32	fw1_memory_base;
	UINT32 	fw1_memory_size;
	char* 	dsp1_memory_name;
	UINT32 	dsp1_memory_base;
	UINT32 	dsp1_memory_size;
	char* 	dpbm_memory_name;
	UINT32 	dpbm_memory_base;
	UINT32 	dpbm_memory_size;
	char* 	dpba_memory_name;
	UINT32 	dpba_memory_base;
	UINT32 	dpba_memory_size;
	char* 	mpb0_memory_name;
	UINT32 	mpb0_memory_base;
	UINT32 	mpb0_memory_size;
	char* 	mpb1_memory_name;
	UINT32 	mpb1_memory_base;
	UINT32 	mpb1_memory_size;
	char* 	mpb2_memory_name;
	UINT32 	mpb2_memory_base;
	UINT32 	mpb2_memory_size;
	char* 	mpb3_memory_name;
	UINT32 	mpb3_memory_base;
	UINT32 	mpb3_memory_size;
	char* 	inter_memory_name;
	UINT32 	inter_memory_base;
	UINT32 	inter_memory_size;
	char* 	enc_memory_name;
	UINT32 	enc_memory_base;
	UINT32 	enc_memory_size;
	char* 	status_memory_name;
	UINT32 	status_memory_base;
	UINT32 	status_memory_size;
	char* 	se_memory_name;
	UINT32 	se_memory_base;
	UINT32 	se_memory_size;
	char* 	ipc_memory_name;
	UINT32 	ipc_memory_base;
	UINT32 	ipc_memory_size;

#else	//#ifdef INCLUDE_L8_CHIP_KDRV

	char* 	fw0_memory_name;
	UINT32	fw0_memory_base;
	UINT32 	fw0_memory_size;

	char* 	dsp0_memory_name;
	UINT32 	dsp0_memory_base;
	UINT32 	dsp0_memory_size;

	char* 	cpbm_memory_name;
	UINT32	cpbm_memory_base;
	UINT32 	cpbm_memory_size;

	char* 	cpba_memory_name;
	UINT32	cpba_memory_base;
	UINT32 	cpba_memory_size;

	char* 	enc_intra_memory_name;
	UINT32	enc_intra_memory_base;
	UINT32 	enc_intra_memory_size;

	char* 	fw1_memory_name;
	UINT32	fw1_memory_base;
	UINT32 	fw1_memory_size;

	char* 	dsp1_memory_name;
	UINT32 	dsp1_memory_base;
	UINT32 	dsp1_memory_size;

	char* 	mpb0_memory_name;
	UINT32 	mpb0_memory_base;
	UINT32 	mpb0_memory_size;

	char* 	mpb1_memory_name;
	UINT32 	mpb1_memory_base;
	UINT32 	mpb1_memory_size;

	char* 	mpb2_memory_name;
	UINT32 	mpb2_memory_base;
	UINT32 	mpb2_memory_size;

	char* 	mpb3_memory_name;
	UINT32 	mpb3_memory_base;
	UINT32 	mpb3_memory_size;

	char*	llb_memory_name;
	UINT32	llb_memory_base;
	UINT32	llb_memory_size;

	char* 	dpbm_memory_name;
	UINT32 	dpbm_memory_base;
	UINT32 	dpbm_memory_size;

	char* 	dpba_memory_name;
	UINT32 	dpba_memory_base;
	UINT32 	dpba_memory_size;

	char* 	iec_memory_name;
	UINT32 	iec_memory_base;
	UINT32 	iec_memory_size;

	char* 	ddco_memory_name;
	UINT32 	ddco_memory_base;
	UINT32 	ddco_memory_size;

	char* 	acr_memory_name;
	UINT32 	acr_memory_base;
	UINT32 	acr_memory_size;

	char* 	enc_memory_name;
	UINT32 	enc_memory_base;
	UINT32 	enc_memory_size;

	char* 	se_memory_name;
	UINT32 	se_memory_base;
	UINT32 	se_memory_size;

	char* 	debug_memory_name;
	UINT32 	debug_memory_base;
	UINT32 	debug_memory_size;

	char* 	ipc_memory_name;
	UINT32 	ipc_memory_base;
	UINT32 	ipc_memory_size;
#endif	//#ifdef INCLUDE_L8_CHIP_KDRV
} LX_ADECDEV_MEM_CFG_S_T;

/**
 * ADEC Mixed IP Power Status
 * ADEC Mixed IP Power Status is defined.
 *
*/
typedef struct LX_ADEC_POWER_STATUS
{
	LX_ADEC_POWER_BLOCK_T 	block;		///< ADEC Mixed IP Power Block Mode.
	LX_ADEC_POWER_MODE_T 	mode;		///< ADEC Mixed IP Power Mode.
} LX_ADEC_POWER_STATUS_T;


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern LX_ADECDEV_MEM_CFG_S_T 	gstMemCfgADEC[];

#ifdef INCLUDE_L8_CHIP_KDRV
/* define ADEC reg for L8 */
extern volatile AUD_REG_L8_T	*g_pstADECReg_l8;
extern volatile AUD_REG_L8_T	*gpRealRegAUD_l8;
extern AUD_REG_L8_T				*gpRegAUD_l8;
#else
/* define ADEC reg for L9 */
extern volatile AUD_REG_T		*gpRealRegAUD;
extern AUD_REG_T				*gpRegAUD;
#endif

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _ADEC_CFG_H_ */

/** @} */
