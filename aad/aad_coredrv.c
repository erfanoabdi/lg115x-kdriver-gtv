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
 *  AAD core driver functions.
 *	aad device will teach you how to make device driver with new platform.
 *
 *  author		Sudhaher (kanthiraj.s@lge.com)
 *  version		1.0
 *  date		2010.02.24
 *  note		Additional information.
 *
 *  @addtogroup lg1150_aad
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
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "os_util.h" //for alloc functions
#include <linux/kthread.h>

#include "aad_drv.h"
#include "aad_coredrv.h"
#include "aad_kapi.h"
#include "proc_util.h"
#include "debug_util.h"


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
extern wait_queue_head_t 	AAD_ASD_Task_wait;
extern int 					AAD_ASD_Task_waitq_flag;


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
//define AAD Reg.
volatile AAD_REG_T		*gpRealRegAAD;
AAD_REG_T				*gpRegAAD;


//This value is set by AAD_SIF_InitModule function.
LX_AAD_INIT_TYPE_T				g_eAadInitType = LX_AAD_INIT_TYPE_NONE;

//This value is used by AAD_RegInitAfterAADReset function.
LX_AAD_HIGH_DEV_LEVEL_T 		g_eHighDevLevel = LX_AAD_HIGH_DEV_LEVEL_OFF;
LX_AAD_SIF_THRESHOLD_T 			g_stThresholdLevel = {LX_AAD_THRES_NONE,	\
													  CARRIER2_MAGNITUDE_THRESHOLD,	\
													  CARRIER2_QUALITY_THRESHOLD_HI,	\
													  CARRIER2_QUALITY_THRESHOLD_LO,	\
													  ID_MAGNITUDE_THRESHOLD,	\
													  ID_MAGNITUDE_THRESHOLD,	\
													  NICAM_ERROR_AAOS_THD_HI,	\
													  NICAM_ERROR_AAOS_THD_LO};

//This value is set by AAD_SIF_SetBandSetup.
LX_AAD_SIF_SOUNDSYSTEM_T		g_i_eSystem = LX_AAD_SIF_SYSTEM_UNKNOWN;

//This value is set by AAD_SIF_SetModeSetup.
LX_AAD_SIF_STANDARD_T			g_i_eStandard = LX_AAD_SIF_NUM_SOUND_STD;

//This value is set for debug print
LX_AAD_SIF_STANDARD_INFO_T		g_io_stStandardInfo = { LX_AAD_SIF_DETECTING_AVALIBILITY,	\
													    LX_AAD_SIF_DETECTING_AVALIBILITY,
													    LX_AAD_SIF_DETECTING_EXSISTANCE };

LX_ATV_AUDIO_MODE_GET_T 		g_o_eAudioMode = LX_ATV_GET_NTSC_BTSC_UNKNOWN;

LX_ATV_AUDIO_MODE_SET_T 		g_i_eAudioModeSet = LX_ATV_SET_NTSC_BTSC_UNKNOWN;


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
//semaphore for AAD Module
static OS_SEM_T	g_AAD_Sema;

//g_ui32AadPrintCount is only increased in AAD_SIF_GetCurAnalogMode function
static UINT32	g_ui32AadPrintCount = 0;

/* Related to AAD monitor task */
static LX_AAD_TASK_MSG_T	g_suiAadWaitQForMonitor;
static wait_queue_head_t 	Aad_Monitor_WaitQ;
static DECLARE_WAIT_QUEUE_HEAD( Aad_Monitor_WaitQ );
static LX_ATV_AUDIO_MODE_GET_T 	g_MonitorAudioMode = LX_ATV_GET_NTSC_A2_UNKNOWN;


/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 * Initializes the aad module.
 * AAD module shall be initialized itself with pre-defined configuration value.\n
 * AAD module includes Analog Audio Decoder parts.\n
 * And, serveral global variable is set by LX_AAD_INIT_TYPE_T value.
 * @li register device and interrupts.
*/
int AAD_SIF_InitModule ( LX_AAD_INIT_TYPE_T eInitType )
{
	UINT8	ui8BtscEn;

	//Initialize AAD semaphore
	OS_InitMutex(&g_AAD_Sema, OS_SEM_ATTR_DEFAULT);

	if ( eInitType == LX_AAD_INIT_ATSC_A2_SELECT )
	{
		AAD_DEBUG ("ATSC A2 SELECTED(KOREA)\n");
	}
	else if ( eInitType == LX_AAD_INIT_ATSC_BTSC_SELECT )
	{
		AAD_DEBUG ("BTSC SELECTED(US)\n");
	}
	else if ( eInitType == LX_AAD_INIT_ATSC_EIAJ_SELECT )
	{
		AAD_DEBUG ("EIA-J SELECTED(JAPAN)\n");
	}
	else if ( eInitType == LX_AAD_INIT_DVB_SELECT )
	{
		AAD_DEBUG ("DVB SELECTED\n");
	}
	else if ( eInitType == LX_AAD_INIT_DVB_TW_SELECT )
	{
		AAD_DEBUG ("DVB SELECTED(TW)\n");
	}
	else if ( eInitType == LX_AAD_INIT_DVB_ID_SELECT )
	{
		AAD_DEBUG ("DVB SELECTED(ID)\n");
	}
	else if ( eInitType == LX_AAD_INIT_DVB_IN_SELECT )
	{
		AAD_DEBUG ("DVB SELECTED(IN)\n");
	}
	else if ( eInitType == LX_AAD_INIT_DVB_CN_SELECT )
	{
		AAD_DEBUG ("DVB SELECTED(CN)\n");
	}
	else if ( eInitType == LX_AAD_INIT_DVB_AU_SELECT )
	{
		AAD_DEBUG ("DVB SELECTED(AU)\n");
	}
	else if ( eInitType == LX_AAD_INIT_ATSC_BR_SELECT )
	{
		AAD_DEBUG ("ATSC SELECTED(BR)\n");
	}
	else
	{
		AAD_ERROR ("LX_AAD_INIT_TYPE_NONE(0x%X)!!!\n", g_eAadInitType);
	}

	//Copy AAD init type
	g_eAadInitType = eInitType;

	//Read System Select register for debug
	AAD_RdFL(system_select);
	AAD_PRINT("g_pstAADReg->system_select : 0x%X\n", AAD_Rd(system_select));

	//Read BTSC Enable Pin Status
	AAD_RdFL(btsc_en_status);
	AAD_Rd01(btsc_en_status, btsc_en, ui8BtscEn);
	if(ui8BtscEn == TRUE)
	{
		AAD_DEBUG ("BTSC Function is enabled.\n");
	}
	else
	{
		AAD_DEBUG ("BTSC Function is disabled.\n");
	}

	//Set a NICAM Error Rate AAOS Threshold High / Low Register
	if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT )
	{
		g_stThresholdLevel.nicamErrorRateHigh = NICAM_ERROR_AAOS_THD_HI_DVB;
		g_stThresholdLevel.nicamErrorRateLow  = NICAM_ERROR_AAOS_THD_LO_DVB;
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT )
	{
		//NICAM Error Rate AAOS Threshold High / Low Register
		//Org. threshold high is 0x50 and low is 0x30 Value
		//g_stThresholdLevel.nicamErrorRateHigh = NICAM_ERROR_AAOS_THD_HI_DVB;
		//g_stThresholdLevel.nicamErrorRateLow  = NICAM_ERROR_AAOS_THD_LO_DVB;
	}
	//In ID(Indonesia) model, set a ID xxxx
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_ID_SELECT )
	{
		g_stThresholdLevel.nicamErrorRateHigh = NICAM_ERROR_AAOS_THD_HI_DVB;
		g_stThresholdLevel.nicamErrorRateLow  = NICAM_ERROR_AAOS_THD_LO_DVB;
	}

	//NICAM Error Rate AAOS Threshold High / Low Register
	//Org. threshold high is 0x50 and low is 0x30 Value
	AAD_RdFL(nicam_error_rate_aaos_thd_hi);
	AAD_Wr01(nicam_error_rate_aaos_thd_hi, nierth, g_stThresholdLevel.nicamErrorRateHigh);
	AAD_WrFL(nicam_error_rate_aaos_thd_hi);

	AAD_RdFL(nicam_error_rate_aaos_thd_lo);
	AAD_Wr01(nicam_error_rate_aaos_thd_lo, niertl, g_stThresholdLevel.nicamErrorRateLow);
	AAD_WrFL(nicam_error_rate_aaos_thd_lo);

	//Disable AAOS(Automatic Audio Output Select)
	AAD_RdFL(audio_output_ctrl);
	AAD_Wr01(audio_output_ctrl, aouten, 0);
	AAD_WrFL(audio_output_ctrl);

	//Read registers
	AAD_RdFL(audio_output_ctrl);
	AAD_PRINT ("AAD Initialzed 0x%X(audio_output_ctrl = 0x%X)\n", eInitType, AAD_Rd(audio_output_ctrl));
	return RET_OK;
}

/**
 * Sets High Deviaton Level Value.
 * These settings do not apply to the single carrier hierarchical modulation modes(EIAJ, BTSC and FM-Radio).
 * @see
*/
int AAD_SIF_SetHighDeviationMode ( LX_AAD_HIGH_DEV_LEVEL_T level )
{
	UINT8 	*debugString[5] = {"OFF(50KHz)", "Level 1(100KHz)", "Level 2(200KHz)",	\
							   "Level 3(384KHz)", "Level 4(540KHz)" };

	//Mutes by MuteCt bit in Mute Control Register.
	AAD_RdFL(mute_control);
	AAD_Wr01(mute_control, mutect,  0b11);
	AAD_Wr01(mute_control, muteovov, 0b1);
	AAD_WrFL(mute_control);

	//Get high deviation level using FM AM demod 1 control register
	AAD_RdFL(fm_am_demod_ctrl);
	switch (level)
	{
		case LX_AAD_HIGH_DEV_LEVEL_OFF: ///< High Deviation OFF(50KHz)
			AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x0);
			AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);
			AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
			AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			break;

		case LX_AAD_HIGH_DEV_LEVEL_1: 	///< High Deviation Level 1(100KHz)
			//In DVB model, set also Demod 1 filter
			if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT		\
			  || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT 	\
			  || g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
			{
				//Demod 1 Maximum FM Deviation: 100kHz
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x1);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);

				//Demod 1 Filter: 200kHz lowpass to reduce noise in high deviation(300 ~ 400 %)
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x1);
			}
			else
			{
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x1);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			}
			break;

		case LX_AAD_HIGH_DEV_LEVEL_2: 	///< High Deviation Level 2(200KHz)
			//In DVB model, set also Demod 1 filter
			if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT		\
			  || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT 	\
			  || g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
			{
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x1);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x1);
			}
			else
			{
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x1);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			}
			break;

		case LX_AAD_HIGH_DEV_LEVEL_3: 	///< High Deviation Level 3(384KHz)
			//In DVB model, set also Demod 1 filter
			if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT		\
			  || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT 	\
			  || g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
			{
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x2);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x2);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			}
			else
			{
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x2);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			}
			break;

		case LX_AAD_HIGH_DEV_LEVEL_4: 	///< High Deviation Level 4(540KHz)
			//In IN(India) model, set also Demod 1 filter
			if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT		\
			  || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT 	\
			  || g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
			{
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x3);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x3);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			}
			else
			{
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x3);
				AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
				AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			}
			break;

		default:
			AAD_DEBUG ("High Deviation : RET_INVALID_PARAMS 1\n");
			AAD_Wr01(fm_am_demod_ctrl, dmd1dv,  0x0);
			AAD_Wr01(fm_am_demod_ctrl, dmd1dv2, 0x0);
			AAD_Wr01(fm_am_demod_ctrl, dmd1fl,  0x0);
			AAD_Wr01(fm_am_demod_ctrl, dmd1fl2, 0x0);
			break;
	}

	//Set high deviation level using FM AM demod 1 control register
	AAD_WrFL(fm_am_demod_ctrl);


	//Set DEMOD2 function
	//Get high deviation level using FM AM demod 2 control register
	AAD_RdFL(fm_dqpsk_demod_ctrl);

	switch (level)
	{
		case LX_AAD_HIGH_DEV_LEVEL_OFF: ///< High Deviation OFF(50KHz)
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv,  0x0);
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv2, 0x0);
			break;

		case LX_AAD_HIGH_DEV_LEVEL_1: 	///< High Deviation Level 1(100KHz)
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv,  0x1);
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv2, 0x0);
			break;

		case LX_AAD_HIGH_DEV_LEVEL_2: 	///< High Deviation Level 2(200KHz)
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv,  0x0);
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv2, 0x1);
			break;

		case LX_AAD_HIGH_DEV_LEVEL_3: 	///< High Deviation Level 3(384KHz)
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv,  0x2);
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv2, 0x0);
			break;

		case LX_AAD_HIGH_DEV_LEVEL_4: 	///< High Deviation Level 4(540KHz)
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv,  0x3);
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv2, 0x0);
			break;

		default:
			AAD_DEBUG ("High Deviation : RET_INVALID_PARAMS 1\n");
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv,  0x0);
			AAD_Wr01(fm_dqpsk_demod_ctrl, dmd2dv2, 0x0);
			break;
	}

	//Set high deviation level using FM AM demod 2 control register
	AAD_WrFL(fm_dqpsk_demod_ctrl);

	//Unmutes by MuteCt bit in Mute Control Register.
	AAD_RdFL(mute_control);
	AAD_Wr01(mute_control, mutect,  0b00);
	AAD_Wr01(mute_control, muteovov, 0b0);
	AAD_WrFL(mute_control);

	//Read registers
	AAD_RdFL(status);
	AAD_RdFL(fm_am_demod_ctrl);
	AAD_RdFL(fm_dqpsk_demod_ctrl);

	//Sanity check for debug print
	if( level >= LX_AAD_HIGH_DEV_LEVEL_4 )
		level = LX_AAD_HIGH_DEV_LEVEL_4;

	AAD_DEBUG_TMP ("High Deviation %s(status = 0x%X)\n", debugString[level], AAD_Rd(status));
	AAD_DEBUG_TMP ("demod1 = 0x%X, demod2 = 0x%X\n", AAD_Rd(fm_am_demod_ctrl), AAD_Rd(fm_dqpsk_demod_ctrl));

	/* Copy High Deviation Level */
	g_eHighDevLevel = level;

	return RET_OK;
}


/**
 * Sets a SIF Band(B/G, I, DK, L, M).
 * This function sets a SIF sound system.
 * @see
*/
int AAD_SIF_SetBandSetup ( LX_AAD_SIF_SOUNDSYSTEM_T eSoundSystem )
{
	UINT8	ui8SysSel;
	int		eRetVal = RET_OK;

	//LX_AAD_SIF_SOUNDSYSTEM_T 	eTVSoundSystem = LX_AAD_SIF_SYSTEM_UNKNOWN;

	UINT8 *debugString[6] = {"SYSTEM_BG", "SYSTEM_I", "SYSTEM_DK", "SYSTEM_L", "SYSTEM_MN",	\
							 "SYSTEM_UNKNOWN" };

	//do Mono/Stereo detect  function if DTV is ATSC A2 mode
	if ( g_eAadInitType == LX_AAD_INIT_ATSC_A2_SELECT )
	{
		//Set a Carrier 2 Quality AAOS Threshold High / Low Register
		//Org. threshold high is 0x34 and low is 0x21. Value is set according to live signal quality from CH7, 11 in KOREA M/N A2 signal.
		//Default value is changed to 0x21.
		AAD_RdFL(carrier2_quality_aaos_thd_hi);
		AAD_Wr01(carrier2_quality_aaos_thd_hi, mn2thh, (CAR2_QUAL_THRES_HI_A2 >> 3));
		AAD_WrFL(carrier2_quality_aaos_thd_hi);

		//AAD_RdFL(carrier2_quality_aaos_thd_lo);
		//AAD_Wr01(carrier2_quality_aaos_thd_lo, mn2thl, (g_stThresholdLevel.carrier2QualityLow >> 3));
		//AAD_WrFL(carrier2_quality_aaos_thd_lo);

		//Set a ID Magnitude Threshold High / Low Register
		//Org. threshold high is 0x64 and low is 0x14.
		//Value is set according to live signal quality from CH7, 11 in KOREA M/N A2 signal.
		AAD_RdFL(id_magnitude_thd_hi);
		AAD_Wr01(id_magnitude_thd_hi, idthh, g_stThresholdLevel.stereoIDMagHigh);
		AAD_WrFL(id_magnitude_thd_hi);

		AAD_RdFL(id_magnitude_thd_lo);
		AAD_Wr01(id_magnitude_thd_lo, idthl, g_stThresholdLevel.stereoIDMagLow);
		AAD_WrFL(id_magnitude_thd_lo);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)
	{
		//Set a ID Magnitude Threshold High / Low Register
		//Org. threshold high is 0x64 and low is 0x14.
		AAD_RdFL(id_magnitude_thd_hi);
		AAD_Wr01(id_magnitude_thd_hi, idthh, g_stThresholdLevel.stereoIDMagHigh);
		AAD_WrFL(id_magnitude_thd_hi);

		AAD_RdFL(id_magnitude_thd_lo);
		AAD_Wr01(id_magnitude_thd_lo, idthl, g_stThresholdLevel.stereoIDMagLow);
		AAD_WrFL(id_magnitude_thd_lo);

		//Set a FM Subcarrier Noise AAOS Threshold High / Low Register
		//Org. threshold high is 0x14 and low is 0x0A Value
		AAD_RdFL(fm_subcarrier_noise_aaos_thd_hi);
		AAD_Wr01(fm_subcarrier_noise_aaos_thd_hi, fmsnth, FM_SUBCARRIER_NOISE_THD_HI_US);
		AAD_WrFL(fm_subcarrier_noise_aaos_thd_hi);

		AAD_RdFL(fm_subcarrier_noise_aaos_thd_lo);
		AAD_Wr01(fm_subcarrier_noise_aaos_thd_lo, fmsntl, FM_SUBCARRIER_NOISE_THD_LO_US);
		AAD_WrFL(fm_subcarrier_noise_aaos_thd_lo);

		//Set a Pilot Mag AAOS Threshold High / Low Register
		//Org. threshold high is 0xB0 and low is 0x64 Value
		AAD_RdFL(pilot_mag_aaos_thd_hi);
		AAD_Wr01(pilot_mag_aaos_thd_hi, pilmth, PILOT_MAG_AAOS_THD_HI_US);
		AAD_WrFL(pilot_mag_aaos_thd_hi);

		AAD_RdFL(pilot_mag_aaos_thd_lo);
		AAD_Wr01(pilot_mag_aaos_thd_lo, pilmtl, PILOT_MAG_AAOS_THD_LO_US);
		AAD_WrFL(pilot_mag_aaos_thd_lo);
	}
	else if(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
	{
		//Set a ID Magnitude Threshold High / Low Register
		//Org. threshold high is 0x64 and low is 0x14.
		AAD_RdFL(id_magnitude_thd_hi);
		AAD_Wr01(id_magnitude_thd_hi, idthh, g_stThresholdLevel.stereoIDMagHigh);
		AAD_WrFL(id_magnitude_thd_hi);

		AAD_RdFL(id_magnitude_thd_lo);
		AAD_Wr01(id_magnitude_thd_lo, idthl, g_stThresholdLevel.stereoIDMagLow);
		AAD_WrFL(id_magnitude_thd_lo);

		//Set a FM Subcarrier Noise AAOS Threshold High / Low Register
		//Org. threshold high is 0x14 and low is 0x0A Value
		AAD_RdFL(fm_subcarrier_noise_aaos_thd_hi);
		AAD_Wr01(fm_subcarrier_noise_aaos_thd_hi, fmsnth, FM_SUBCARRIER_NOISE_THD_HI_BR);
		AAD_WrFL(fm_subcarrier_noise_aaos_thd_hi);

		AAD_RdFL(fm_subcarrier_noise_aaos_thd_lo);
		AAD_Wr01(fm_subcarrier_noise_aaos_thd_lo, fmsntl, FM_SUBCARRIER_NOISE_THD_LO_BR);
		AAD_WrFL(fm_subcarrier_noise_aaos_thd_lo);

		//Set a Pilot Mag AAOS Threshold High / Low Register
		//Org. threshold high is 0xB0 and low is 0x64 Value
		AAD_RdFL(pilot_mag_aaos_thd_hi);
		AAD_Wr01(pilot_mag_aaos_thd_hi, pilmth, PILOT_MAG_AAOS_THD_HI_BR);
		AAD_WrFL(pilot_mag_aaos_thd_hi);

		AAD_RdFL(pilot_mag_aaos_thd_lo);
		AAD_Wr01(pilot_mag_aaos_thd_lo, pilmtl, PILOT_MAG_AAOS_THD_LO_BR);
		AAD_WrFL(pilot_mag_aaos_thd_lo);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT )
	{
		//Set a FM Subcarrier Noise AAOS Threshold High / Low Register
		//Org. threshold high is 0x14 and low is 0x0A Value
		AAD_RdFL(fm_subcarrier_noise_aaos_thd_hi);
		AAD_Wr01(fm_subcarrier_noise_aaos_thd_hi, fmsnth, FM_SUBCARRIER_NOISE_THD_HI_TW);
		AAD_WrFL(fm_subcarrier_noise_aaos_thd_hi);

		AAD_RdFL(fm_subcarrier_noise_aaos_thd_lo);
		AAD_Wr01(fm_subcarrier_noise_aaos_thd_lo, fmsntl, FM_SUBCARRIER_NOISE_THD_LO_TW);
		AAD_WrFL(fm_subcarrier_noise_aaos_thd_lo);

		//Set a Pilot Mag AAOS Threshold High / Low Register
		//Org. threshold high is 0xB0 and low is 0x64 Value
		AAD_RdFL(pilot_mag_aaos_thd_hi);
		AAD_Wr01(pilot_mag_aaos_thd_hi, pilmth, PILOT_MAG_AAOS_THD_HI_TW);
		AAD_WrFL(pilot_mag_aaos_thd_hi);

		AAD_RdFL(pilot_mag_aaos_thd_lo);
		AAD_Wr01(pilot_mag_aaos_thd_lo, pilmtl, PILOT_MAG_AAOS_THD_LO_TW);
		AAD_WrFL(pilot_mag_aaos_thd_lo);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT )
	{
		//Carrier 1 Quality AAOS Threshold High / Low Register
		//Org. threshold high is 0x22 and low is 0x16 Value
		AAD_RdFL(carrier1_quality_aaos_thd_hi);
		AAD_Wr01(carrier1_quality_aaos_thd_hi, mn1thh, CARRIER1_QUAL_AAOS_THD_HI_DVB);
		AAD_WrFL(carrier1_quality_aaos_thd_hi);

		AAD_RdFL(carrier1_quality_aaos_thd_lo);
		AAD_Wr01(carrier1_quality_aaos_thd_lo, mn1thl, CARRIER1_QUAL_AAOS_THD_LO_DVB);
		AAD_WrFL(carrier1_quality_aaos_thd_lo);

		//NICAM Error Rate AAOS Threshold High / Low Register
		//Org. threshold high is 0x50 and low is 0x30 Value
		AAD_RdFL(nicam_error_rate_aaos_thd_hi);
		AAD_Wr01(nicam_error_rate_aaos_thd_hi, nierth, g_stThresholdLevel.nicamErrorRateHigh);
		AAD_WrFL(nicam_error_rate_aaos_thd_hi);

		AAD_RdFL(nicam_error_rate_aaos_thd_lo);
		AAD_Wr01(nicam_error_rate_aaos_thd_lo, niertl, g_stThresholdLevel.nicamErrorRateLow);
		AAD_WrFL(nicam_error_rate_aaos_thd_lo);

		//Enable AAOS(Automatic Audio Output Select)
		AAD_RdFL(audio_output_ctrl);
		AAD_Wr01(audio_output_ctrl, aouten, 1);
		AAD_WrFL(audio_output_ctrl);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT )
	{
		//Carrier 1 Quality AAOS Threshold High / Low Register
		//Org. threshold high is 0x22 and low is 0x16 Value
		// 2011.11.12 by suyeon.jung
		AAD_RdFL(carrier1_quality_aaos_thd_hi);
		AAD_Wr01(carrier1_quality_aaos_thd_hi, mn1thh, CARRIER1_QUAL_AAOS_THD_HI_DVB);
		AAD_WrFL(carrier1_quality_aaos_thd_hi);

		//AAD_RdFL(carrier1_quality_aaos_thd_lo);
		//AAD_Wr01(carrier1_quality_aaos_thd_lo, mn1thl, 0x0C);
		//AAD_WrFL(carrier1_quality_aaos_thd_lo);

		//NICAM Error Rate AAOS Threshold High / Low Register
		//Org. threshold high is 0x50 and low is 0x30 Value
#if 0
		AAD_RdFL(nicam_error_rate_aaos_thd_hi);
		AAD_Wr01(nicam_error_rate_aaos_thd_hi, nierth, g_stThresholdLevel.nicamErrorRateHigh);
		AAD_WrFL(nicam_error_rate_aaos_thd_hi);

		AAD_RdFL(nicam_error_rate_aaos_thd_lo);
		AAD_Wr01(nicam_error_rate_aaos_thd_lo, niertl, g_stThresholdLevel.nicamErrorRateLow);
		AAD_WrFL(nicam_error_rate_aaos_thd_lo);
#endif

		//Enable AAOS(Automatic Audio Output Select)
		AAD_RdFL(audio_output_ctrl);
		AAD_Wr01(audio_output_ctrl, aouten, 1);
		AAD_WrFL(audio_output_ctrl);
	}
	//In IN(India) model, set a ID Magnitude Threshold High Register to near max value
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
	{
		g_stThresholdLevel.stereoIDMagHigh = ID_MAG_THRES_INDIA_0xF8;

		AAD_RdFL(id_magnitude_thd_hi);
		AAD_Wr01(id_magnitude_thd_hi, idthh, g_stThresholdLevel.stereoIDMagHigh);
		AAD_WrFL(id_magnitude_thd_hi);
	}
	//In ID(Indonesia) model, set a ID xxxx
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_ID_SELECT )
	{
		//Carrier 1 Quality AAOS Threshold High / Low Register
		//Org. threshold high is 0x22 and low is 0x16 Value
		AAD_RdFL(carrier1_quality_aaos_thd_hi);
		AAD_Wr01(carrier1_quality_aaos_thd_hi, mn1thh, CARRIER1_QUAL_AAOS_THD_HI_ID);
		AAD_WrFL(carrier1_quality_aaos_thd_hi);

		AAD_RdFL(carrier1_quality_aaos_thd_lo);
		AAD_Wr01(carrier1_quality_aaos_thd_lo, mn1thl, CARRIER1_QUAL_AAOS_THD_LO_ID);
		AAD_WrFL(carrier1_quality_aaos_thd_lo);

		//NICAM Error Rate AAOS Threshold High / Low Register
		//Org. threshold high is 0x50 and low is 0x30 Value
		AAD_RdFL(nicam_error_rate_aaos_thd_hi);
		AAD_Wr01(nicam_error_rate_aaos_thd_hi, nierth, g_stThresholdLevel.nicamErrorRateHigh);
		AAD_WrFL(nicam_error_rate_aaos_thd_hi);

		AAD_RdFL(nicam_error_rate_aaos_thd_lo);
		AAD_Wr01(nicam_error_rate_aaos_thd_lo, niertl, g_stThresholdLevel.nicamErrorRateLow);
		AAD_WrFL(nicam_error_rate_aaos_thd_lo);
	}

#if 0	//disable ASD function
	//do ASD function if DTV is DVB mode
	if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT )
	{
		//Do ASD using both carrier (6.5, 4.5) and return tv sound system available or not
		if ( RET_OK == AAD_DoASD( eSoundSystem ) )
		{
			//Check a detected sound system
			eTVSoundSystem = AAD_GetSoundSystem();

			if (eTVSoundSystem != eSoundSystem )
			{
				//Sanity check for debug print
				if( eSoundSystem >= LX_AAD_SIF_SYSTEM_UNKNOWN )
					eSoundSystem = LX_AAD_SIF_SYSTEM_MN + 1;

				AAD_ERROR ("Sound system does not match.(%s -> %s)\n", debugString[eSoundSystem], debugString[eTVSoundSystem]);
			}
		}
		else
		{
			//Sanity check for debug print
			if( eSoundSystem >= LX_AAD_SIF_SYSTEM_UNKNOWN )
				eSoundSystem = LX_AAD_SIF_SYSTEM_MN + 1;

			AAD_ERROR ("AAD_SIF_SetBandSetup : ASD Failed!!!(%s)\n", debugString[eSoundSystem]);
		}
	}
#endif

	//Set High Deviation Level : The HDEV reg. is auto cleared by AAD H/W reset, Overwrite and AAOS function.
	AAD_SIF_SetHighDeviationMode(g_eHighDevLevel);

	//Check a current sound sysytem is detected.
	AAD_RdFL(system_select);
	AAD_Rd01(system_select, syssel, ui8SysSel);
	if ( ui8SysSel == 0xF )	//sound system is unknown
	{
		switch ( eSoundSystem )
		{
			case LX_AAD_SIF_SYSTEM_BG:
				AAD_Wr01(system_select, syssel, 0x1); //FM-Mono/NICAM

				//Start Stereo Carrier Search Control(Skip A2DK1, A2DK2 and A2DK3 in BG system)
				//Syssel change automatically when a stereo carrier is detected
				AAD_RdFL(stereo_carrier_search_ctrl);
				AAD_Wr01(stereo_carrier_search_ctrl, srchen, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, chgmod, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, vsphase_y_rnw, 0b111);
				AAD_WrFL(stereo_carrier_search_ctrl);

				//For workaround, NICAM high frequency area deviation is larger than low area.
				//Set a Carrier Average Frequency Threshold(ASD, SCS)
				//Org. threshold high is 0x18.
				AAD_RdFL(carrier_average_freq_thd);
				AAD_Wr01(carrier_average_freq_thd, asfth, CARRIER_AVG_FREQ_THD_DVB_BG);
				AAD_WrFL(carrier_average_freq_thd);
				break;

			case LX_AAD_SIF_SYSTEM_I:
				AAD_Wr01(system_select, syssel, 0x3); //FM-Mono/NICAM
				break;

			case LX_AAD_SIF_SYSTEM_DK:
				AAD_Wr01(system_select, syssel, 0x7); //FM-Mono/NICAM

				//Start Stereo Carrier Search Control
				//Syssel change automatically when a stereo carrier is detected
				AAD_RdFL(stereo_carrier_search_ctrl);
				AAD_Wr01(stereo_carrier_search_ctrl, srchen, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, chgmod, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, vsphase_y_rnw, 0b000);
				AAD_WrFL(stereo_carrier_search_ctrl);

				break;

			case LX_AAD_SIF_SYSTEM_L:
				AAD_Wr01(system_select, syssel, 0x2); //AM-Mono/NICAM
				break;

			case LX_AAD_SIF_SYSTEM_MN:
				if ( g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT
				  || g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT
				  || g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT)
				{
					AAD_Wr01(system_select, syssel, 0xA); //FM-Stereo + SAP (BTSC)
				}
				else if ( g_eAadInitType == LX_AAD_INIT_ATSC_EIAJ_SELECT )
				{
					AAD_Wr01(system_select, syssel, 0x9); //FM-Stereo (EIA-J)
				}
				else
				{
					AAD_Wr01(system_select, syssel, 0x8); //FM-Stereo (A2)
				}
				break;

			case LX_AAD_SIF_SYSTEM_UNKNOWN:
				eRetVal = RET_INVALID_PARAMS;
				break;

			default:
				eRetVal = RET_INVALID_PARAMS;
				break;
		}

		//Set a current sound system.
		AAD_WrFL(system_select);
	}

	//Copy for global value
	g_i_eSystem = eSoundSystem;

	//Reset audio mode in channel is changed.
	g_o_eAudioMode = LX_ATV_GET_NTSC_BTSC_UNKNOWN;
	g_io_stStandardInfo.detectedStandard = LX_AAD_SIF_NOT_AVAILE;
	g_io_stStandardInfo.availability 	 = LX_AAD_SIF_DETECTING_EXSISTANCE;

	//Read registers
	AAD_RdFL(system_select);
	AAD_RdFL(status);

	//Sanity check for debug print
	if( eSoundSystem > LX_AAD_SIF_SYSTEM_MN )
		eSoundSystem = LX_AAD_SIF_SYSTEM_MN + 1;

	AAD_DEBUG_TMP ("AAD_SIF_SetBandSetup : Sound System %d(%s)(syssel = %d, status = 0x%2X)\n",	\
					eSoundSystem, debugString[eSoundSystem], AAD_Rd(system_select), AAD_Rd(status));

	return eRetVal;
}

/**
 * Sets a SIF sound standard input mode.
 * This function sets a SIF sound standard.
 * @see
*/
int AAD_SIF_SetModeSetup ( LX_AAD_SIF_STANDARD_T eStandard )
{
	int		eRetVal = RET_OK;

	UINT8	ui8SysSel;
	BOOLEAN	bNeedToSet = FALSE;

	LX_AAD_SIF_SOUNDSYSTEM_T 	eTVSoundSystem = LX_AAD_SIF_SYSTEM_UNKNOWN;

	UINT8 	*debugString[17] = {"MODE_DETECT", 	"BG_NICAM", 	"BG_FM", 	"BG_A2", 	"I_NICAM",	\
							  	"I_FM", 		"DK_NICAM", 	"DK_FM", 	"DK1_A2", 	"DK2_A2",	\
							  	"DK3_A2", 		"L_NICAM", 		"L_AM", 	"MN_A2", 	"MN_BTSC",	\
							  	"MN_EIAJ", 		"NUM_SOUND_STD" };

	//Check a current sound sysytem is detected.
	AAD_RdFL(system_select);
	AAD_Rd01(system_select, syssel, ui8SysSel);

	//Set Mode using system select register based on the input parameter
	switch (eStandard)
	{
		case LX_AAD_SIF_MODE_DETECT:
			//do ASD same as above and does not blocking call
			//doing without blocking call is not possible
			eRetVal = RET_INVALID_PARAMS;
			break;

		case LX_AAD_SIF_BG_NICAM:
		case LX_AAD_SIF_BG_FM:
		case LX_AAD_SIF_BG_A2:
			//Check a detected sound system
			eTVSoundSystem = AAD_GetSoundSystem();
			if (eTVSoundSystem != LX_AAD_SIF_SYSTEM_BG )
			{
				bNeedToSet = TRUE;
				AAD_Wr01(system_select, syssel, 0x0);
			}
			break;

		case LX_AAD_SIF_I_NICAM:
		case LX_AAD_SIF_I_FM:
			//Check a detected sound system
			eTVSoundSystem = AAD_GetSoundSystem();
			if (eTVSoundSystem != LX_AAD_SIF_SYSTEM_I )
			{
				bNeedToSet = TRUE;
				AAD_Wr01(system_select, syssel, 0x3);
			}
			break;

		case LX_AAD_SIF_DK_NICAM:
		case LX_AAD_SIF_DK_FM:
		case LX_AAD_SIF_DK1_A2:
		case LX_AAD_SIF_DK2_A2:
		case LX_AAD_SIF_DK3_A2:
			//Check a detected sound system
			eTVSoundSystem = AAD_GetSoundSystem();
			if (eTVSoundSystem != LX_AAD_SIF_SYSTEM_DK )
			{
				bNeedToSet = TRUE;
				AAD_Wr01(system_select, syssel, 0x4);
			}
			break;

		case LX_AAD_SIF_L_NICAM:
		case LX_AAD_SIF_L_AM:
			//Check a detected sound system
			eTVSoundSystem = AAD_GetSoundSystem();
			if (eTVSoundSystem != LX_AAD_SIF_SYSTEM_L )
			{
				bNeedToSet = TRUE;
				AAD_Wr01(system_select, syssel, 0x2);
			}
			break;

		case LX_AAD_SIF_MN_A2:
			//Check a detected sound system
			if (ui8SysSel != 8)
			{
				bNeedToSet = TRUE;
				AAD_Wr01(system_select, syssel, 0x8);
			}
			break;

		case LX_AAD_SIF_MN_BTSC:
			if ( (g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT) )
			{
				//Check a detected sound system
				if (ui8SysSel != 0xA)
				{
					bNeedToSet = TRUE;
					AAD_Wr01(system_select, syssel, 0xA);
				}
			}
			else
			{
				//wait 1000 ms for debug print
				AAD_SIF_TIME_DELAY_MS (ERROR_PRINT_DELAY);

				AAD_ERROR("Error : BTSC Not Enabled!\n");
				eRetVal = RET_INVALID_PARAMS;
			}
			break;

		case LX_AAD_SIF_MN_EIAJ:
			//Check a detected sound system
			if (ui8SysSel != 0x9)
			{
				bNeedToSet = TRUE;
				AAD_Wr01(system_select, syssel, 0x9);
			}
			break;

		default:
			eRetVal = RET_INVALID_PARAMS;
			break;
	}

	//Sanity check for debug print
	if( eStandard > LX_AAD_SIF_NUM_SOUND_STD )
		eStandard = LX_AAD_SIF_NUM_SOUND_STD;

	//Check bNeedToSet flag
	if (bNeedToSet == TRUE)
	{
		//Set a current sound system and standard.
		AAD_WrFL(system_select);

		AAD_DEBUG ("AAD_SIF_SetModeSetup : Sound Standard %d(%s)(SysSel = %d, status = 0x%2X)\n",	\
					eStandard, debugString[eStandard], AAD_Rd(system_select), AAD_Rd(status));
	}

	//Copy for global value
	g_i_eStandard = eStandard;

	//Read registers
	AAD_RdFL(system_select);
	AAD_RdFL(status);

	//For debugging print
	if( g_i_eStandard != eStandard )
	{
		AAD_DEBUG_TMP ("AAD_SIF_SetModeSetup : Sound Standard %d(%s)(SysSel = %d, status = 0x%2X)\n",	\
						eStandard, debugString[eStandard], AAD_Rd(system_select), AAD_Rd(status) );
	}

	//For debugging print
	if((g_ui32AadPrintCount % DEBUG_MSG_PRINT_TIME) == 0)	//DDI calls every 30 ms
	{
		AAD_DEBUG_TMP ("AAD_SIF_SetModeSetup : Sound Standard %d(%s)(SysSel = %d, status = 0x%2X)\n",	\
						eStandard, debugString[eStandard], AAD_Rd(system_select), AAD_Rd(status));
	}

	AAD_PRINT ("AAD_SIF_SetModeSetup : Sound Standard %d(%s)(SysSel = %d, status = 0x%2X)\n",	\
				eStandard, debugString[eStandard], AAD_Rd(system_select), AAD_Rd(status));

	return eRetVal;
}


/**
 * Sets a user analog TV stereo mode.
 * Analog Audio Output Mode is setted to Analog Audio Decoder by User Mode or Analog Audio Task.
 * @see
*/
int AAD_SIF_SetUserAnalogMode ( LX_ATV_AUDIO_MODE_SET_T eAudioModeSet )
{
	int			 			 eRetVal = RET_OK;

	UINT8 *debugStrMoSt[25] = {"PAL_MONO", 	  "PAL_MONO_F",    "PAL_ST", 		"PAL_ST_F", 		"PAL_DUAL_I",	\
							   "PAL_DUAL_II", "PAL_DUAL_I_II", "NICAM_MONO", 	"NICAM_MONO_F", 	"NICAM_ST", \
							   "NICAM_ST_F",  "NICAM_DUAL_I",  "NICAM_DUAL_II", "NICAM_DUAL_I_II", 	"NICAM_DUAL_F", \
							   "PAL_UNKNOWN", "A2_MONO", 	   "A2_STEREO", 	"A2_SAP",			"A2_UNKNOWN", \
							   "BTSC_MONO",   "BTSC_ST", 	   "BTSC_SAP_MONO", "BTSC_SAP_ST", 		"BTSC_UNKNOWN" };

	UINT8 *debugStrInit[12] = {"TYPE_NONE", "ATSC_A2", "ATSC_BTSC",   "ATSC_EIAJ", "DVB",		\
							   "DVB_TW", 	"DVB_ID",  "DVB_IN(DIA)", "DVB_CN",    "DVB_AU",	\
							   "ATSC_BR", 	"TYPE_MAX" };


	UINT8 *debugStrSys[6] =   {"SYSTEM_BG", "SYSTEM_I", "SYSTEM_DK", "SYSTEM_L", "SYSTEM_MN",	\
							   "SYSTEM_UNKNOWN" };

	//Need to set in Indonesia level problem
	//Need to set in SECAM L level problem
	//Set volume level	 by audio sound modulation
	if ( (g_eAadInitType == LX_AAD_INIT_DVB_ID_SELECT )
	   ||(g_i_eSystem == LX_AAD_SIF_SYSTEM_L) )
	{
		//Read manual_output_level_left_ch reg.
		AAD_RdFL(manual_output_level_left_ch);

		//Read manual_output_level_right_ch reg.
		AAD_RdFL(manual_output_level_right_ch);

		switch (eAudioModeSet)
		{
			case LX_ATV_SET_PAL_NICAM_MONO:
			case LX_ATV_SET_PAL_NICAM_STEREO:
			case LX_ATV_SET_PAL_NICAM_DUALI:
			case LX_ATV_SET_PAL_NICAM_DUALII:
			case LX_ATV_SET_PAL_NICAM_DUALI_II:		//app. calls this mode in UI DUAL I+II osd.

				//Set  a 0.2 times for a left, right  volume
				AAD_Wr01(manual_output_level_left_ch,  levlft, 0x80);
				AAD_Wr01(manual_output_level_right_ch, levlrt, 0x80);
				break;

			case LX_ATV_SET_PAL_MONO:
			case LX_ATV_SET_PAL_UNKNOWN:
			case LX_ATV_SET_PAL_STEREO:
			case LX_ATV_SET_PAL_DUALI:
			case LX_ATV_SET_PAL_DUALII:
			case LX_ATV_SET_PAL_DUALI_II:
			case LX_ATV_SET_PAL_MONO_FORCED:
			case LX_ATV_SET_PAL_STEREO_FORCED:
			case LX_ATV_SET_PAL_NICAM_MONO_FORCED:
			case LX_ATV_SET_PAL_NICAM_STEREO_FORCED:
			case LX_ATV_SET_PAL_NICAM_DUAL_FORCED:	//app. calls this mode in UI Mono osd in DUAL mode

				//Set  a 2 times for a left, right  volume
				AAD_Wr01(manual_output_level_left_ch,  levlft, 0xFF);
				AAD_Wr01(manual_output_level_right_ch, levlrt, 0xFF);
				break;

			default:
				eAudioModeSet = LX_ATV_SET_NTSC_BTSC_UNKNOWN;
				eRetVal = RET_INVALID_PARAMS;
				break;
		}

		//Write manual_output_level_left_ch reg.
		AAD_WrFL(manual_output_level_left_ch);

		//Write manual_output_level_right_ch reg.
		AAD_WrFL(manual_output_level_right_ch);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT		\
	  	   || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT)
	{
		//Read manual_output_level_left_ch reg.
		AAD_RdFL(manual_output_level_left_ch);

		//Read manual_output_level_right_ch reg.
		AAD_RdFL(manual_output_level_right_ch);

		//Set  a 2 times for a left, right  volume
		AAD_Wr01(manual_output_level_left_ch,  levlft, 0xFF);
		AAD_Wr01(manual_output_level_right_ch, levlrt, 0xFF);

		//Write manual_output_level_left_ch reg.
		AAD_WrFL(manual_output_level_left_ch);

		//Write manual_output_level_right_ch reg.
		AAD_WrFL(manual_output_level_right_ch);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
	{
		//Read manual_output_level_left_ch reg.
		AAD_RdFL(manual_output_level_left_ch);

		//Read manual_output_level_right_ch reg.
		AAD_RdFL(manual_output_level_right_ch);

		//Set  a -6dB(allow the decoder to robustly decode 200% deviation in allmoded) level for a left, right  volume
		AAD_Wr01(manual_output_level_left_ch,  levlft, 0xFF); // HDEV option
		AAD_Wr01(manual_output_level_right_ch, levlrt, 0xFF);

		//Write manual_output_level_left_ch reg.
		AAD_WrFL(manual_output_level_left_ch);

		//Write manual_output_level_right_ch reg.
		AAD_WrFL(manual_output_level_right_ch);
	}

	//Read audio output control  reg.
	AAD_RdFL(audio_output_ctrl);

	if ( g_eAadInitType == LX_AAD_INIT_ATSC_A2_SELECT )
	{
		//Disable AAOS(Automatic Audio Output Select)
		AAD_Wr01(audio_output_ctrl, aouten, 0);
	}
	else if ( (g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)
			||(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
			||(g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT) )
	{
		//Disable AAOS(Automatic Audio Output Select)
		AAD_Wr01(audio_output_ctrl, aouten, 0);

		//Read Standard Select reg.
		AAD_RdFL(standard_select);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT )
	{
		//Enable AAOS(Automatic Audio Output Select)
		AAD_Wr01(audio_output_ctrl, aouten, 1);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
	{
		//Disable AAOS(Automatic Audio Output Select)
		AAD_Wr01(audio_output_ctrl, aouten, 0);
	}

	switch (eAudioModeSet)
	{
		case LX_ATV_SET_PAL_MONO:
		case LX_ATV_SET_PAL_UNKNOWN:
		case LX_ATV_SET_NTSC_A2_MONO:
		case LX_ATV_SET_NTSC_A2_UNKNOWN:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x0);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x0);
			break;

		case LX_ATV_SET_PAL_NICAM_MONO:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x0);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x0);
			break;

		case LX_ATV_SET_NTSC_BTSC_MONO:
		case LX_ATV_SET_NTSC_BTSC_UNKNOWN:
			AAD_Wr01(standard_select, stndrd, 0x0); 	//BTSC (Mono or Stereo)

			AAD_Wr01(audio_output_ctrl, lfsel, 0x0);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x0);
			break;

		case LX_ATV_SET_PAL_STEREO:
		case LX_ATV_SET_NTSC_A2_STEREO:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x1);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x1);
			break;

		case LX_ATV_SET_PAL_NICAM_STEREO:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x1);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x1);
			break;

		case LX_ATV_SET_NTSC_BTSC_STEREO:
			AAD_Wr01(standard_select, stndrd, 0x0); 	//BTSC (Mono or Stereo)

			AAD_Wr01(audio_output_ctrl, lfsel, 0x1);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x1);
			break;

		case LX_ATV_SET_PAL_DUALI:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x2);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x2);
			break;

		case LX_ATV_SET_PAL_NICAM_DUALI:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x2);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x2);
			break;

		case LX_ATV_SET_NTSC_BTSC_SAP_MONO:
		case LX_ATV_SET_NTSC_BTSC_SAP_STEREO:	//Not Used in DDI function
			AAD_Wr01(standard_select, stndrd, 0x1); 	// BTSC(Mono or SAP)

			AAD_Wr01(audio_output_ctrl, lfsel, 0x3);	//SAP
			AAD_Wr01(audio_output_ctrl, rtsel, 0x3);	//SAP
			break;

		case LX_ATV_SET_PAL_DUALII:
		case LX_ATV_SET_NTSC_A2_SAP:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x3);	//SAP
			AAD_Wr01(audio_output_ctrl, rtsel, 0x3);	//SAP
			break;

		case LX_ATV_SET_PAL_NICAM_DUALII:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x3);	//SAP
			AAD_Wr01(audio_output_ctrl, rtsel, 0x3);	//SAP
			break;

		case LX_ATV_SET_PAL_DUALI_II:
			AAD_Wr01(audio_output_ctrl, lfsel, 0x2);	//L : DUAL I
			AAD_Wr01(audio_output_ctrl, rtsel, 0x3);	//R : DUAL II
			break;

		case LX_ATV_SET_PAL_NICAM_DUALI_II:		//app. calls this mode in UI DUAL I+II osd.
			AAD_Wr01(audio_output_ctrl, lfsel, 0x2);	//L : DUAL I
			AAD_Wr01(audio_output_ctrl, rtsel, 0x3);	//R : DUAL II
			break;

		case LX_ATV_SET_PAL_MONO_FORCED:
		case LX_ATV_SET_PAL_STEREO_FORCED:
		case LX_ATV_SET_PAL_NICAM_MONO_FORCED:
		case LX_ATV_SET_PAL_NICAM_STEREO_FORCED:
		case LX_ATV_SET_PAL_NICAM_DUAL_FORCED:		//app. calls this mode in UI Mono osd in DUAL mode
			//Disables AAOS temporily for this mode
			AAD_Wr01(audio_output_ctrl, aouten, 0);

			AAD_Wr01(audio_output_ctrl, lfsel, 0x0);
			AAD_Wr01(audio_output_ctrl, rtsel, 0x0);
			break;

		default:
			eAudioModeSet = LX_ATV_SET_NTSC_BTSC_UNKNOWN;
			eRetVal = RET_INVALID_PARAMS;
			break;
	}

	//Set other value by model base
	if ( g_eAadInitType == LX_AAD_INIT_ATSC_A2_SELECT )
	{
		//Set High Deviation Level : The HDEV reg. is auto cleared by AAD H/W reset, Overwrite and AAOS function.
		AAD_SIF_SetHighDeviationMode(g_eHighDevLevel);
	}
	else if ( (g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)
		    ||(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
		    ||(g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT) )
	{
		//Write Standard Select reg.
		AAD_WrFL(standard_select);

		//Read registers
		AAD_RdFL(standard_select);
		AAD_DEBUG_TMP ("SetUserAnalogMode: standard_select = 0x%2X\n", AAD_Rd(standard_select));
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_IN_SELECT )
	{
		//Set High Deviation Level : The HDEV reg. is auto cleared by AAD H/W reset, Overwrite and AAOS function.
		AAD_SIF_SetHighDeviationMode(g_eHighDevLevel);
	}
	else if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT )
	{
		//Set High Deviation Level : The HDEV reg. is auto cleared by AAD H/W reset, Overwrite and AAOS function.
		AAD_SIF_SetHighDeviationMode(g_eHighDevLevel);
	}

	//Write audio output control  reg.
	AAD_WrFL(audio_output_ctrl);

	//Sanity check for debug print
	if( eAudioModeSet >= LX_ATV_SET_NTSC_BTSC_UNKNOWN )
		eAudioModeSet = LX_ATV_SET_NTSC_BTSC_UNKNOWN;

	//Sanity check for debug print
	if( g_eAadInitType > LX_AAD_INIT_TYPE_MAX )
		g_eAadInitType = LX_AAD_INIT_TYPE_MAX;

	//Sanity check for debug print
	if( g_i_eSystem > LX_AAD_SIF_SYSTEM_MN )
		g_i_eSystem = LX_AAD_SIF_SYSTEM_MN + 1;

	//For debugging print
	if( g_i_eAudioModeSet != eAudioModeSet )
	{
		//Read registers
		AAD_RdFL(manual_output_level_left_ch);
		AAD_RdFL(audio_output_ctrl);
		AAD_RdFL(status);
		AAD_DEBUG ("SetUserAnalogMode %10s[output = 0x%2X](status = 0x%2X)\n", debugStrMoSt[eAudioModeSet], AAD_Rd(audio_output_ctrl), AAD_Rd(status));
		AAD_DEBUG ("SetUserAnalogMode : InitType = %10s, eSystem = %10s, level = 0x%X\n", debugStrInit[g_eAadInitType], debugStrSys[g_i_eSystem], AAD_Rd(manual_output_level_left_ch));
	}

	//Copy for global value
	g_i_eAudioModeSet = eAudioModeSet;

	AAD_PRINT ("SetUserAnalogMode : InitType = %10s, eSystem = %10s\n", debugStrInit[g_eAadInitType], debugStrSys[g_i_eSystem]);

	return eRetVal;
}


/**
 * Sets a A2 threshold level of BTSC or A2 sound system.
 * This function sets the threshold level of stereo or mono sound detection in current SIF input.
 * @see
*/
int AAD_SIF_SetBTSCA2Level ( LX_AAD_SIF_THRESHOLD_T level )
{
	//Check the threshold mode
	if(level.mode & LX_AAD_THRES_CAR2_MAGITUDE)
	{
		//Copy A2 Threshold Level
		g_stThresholdLevel.carrier2Magitude = level.carrier2Magitude;
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : carrier2Magitude = 0x%X\n", g_stThresholdLevel.carrier2Magitude);
	}

	//Check the threshold mode
	if(level.mode & LX_AAD_THRES_CAR2_QUALITY_HI)
	{
		//Copy A2 Threshold Level
		g_stThresholdLevel.carrier2QualityHigh = level.carrier2QualityHigh;
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : carrier2QualityHigh = 0x%X\n", g_stThresholdLevel.carrier2QualityHigh);

		//Set a Carrier 2 Quality AAOS Threshold High / Low Register
		AAD_RdFL(carrier2_quality_aaos_thd_hi);
		AAD_Wr01(carrier2_quality_aaos_thd_hi, mn2thh, (g_stThresholdLevel.carrier2QualityHigh >> 3));
		AAD_WrFL(carrier2_quality_aaos_thd_hi);

		//Read registers
		AAD_RdFL(carrier2_quality_aaos_thd_hi);
		AAD_RdFL(carrier2_quality_aaos_thd_lo);
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : CAR2 Quality High  = 0x%X, Low = 0x%X\n",	\
				   AAD_Rd(carrier2_quality_aaos_thd_hi), AAD_Rd(carrier2_quality_aaos_thd_lo));
	}

	//Check the threshold mode
	if(level.mode & LX_AAD_THRES_CAR2_QUALITY_LO)
	{
		//Copy A2 Threshold Level
		g_stThresholdLevel.carrier2QualityLow = level.carrier2QualityLow;
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : carrier2QualityLow = 0x%X\n", g_stThresholdLevel.carrier2QualityLow);

		//Set a Carrier 2 Quality AAOS Threshold High / Low Register
		AAD_RdFL(carrier2_quality_aaos_thd_lo);
		AAD_Wr01(carrier2_quality_aaos_thd_lo, mn2thl, (g_stThresholdLevel.carrier2QualityLow >> 3));
		AAD_WrFL(carrier2_quality_aaos_thd_lo);

		//Read registers
		AAD_RdFL(carrier2_quality_aaos_thd_hi);
		AAD_RdFL(carrier2_quality_aaos_thd_lo);
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : CAR2 Quality High  = 0x%X, Low = 0x%X\n",	\
				   AAD_Rd(carrier2_quality_aaos_thd_hi), AAD_Rd(carrier2_quality_aaos_thd_lo));
	}

	//Check the threshold mode
	if(level.mode & LX_AAD_THRES_ID_MAGNITUDE)
	{
		//Copy A2 Threshold Level
		g_stThresholdLevel.stereoIDMagHigh = level.stereoIDMagHigh;
		g_stThresholdLevel.stereoIDMagLow  = level.stereoIDMagLow;
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : stereoIDMagHigh = 0x%X, stereoIDMagLow = 0x%X\n",	\
				   g_stThresholdLevel.stereoIDMagHigh, g_stThresholdLevel.stereoIDMagLow);

		//Set a ID Magnitude Threshold High / Low Register
		AAD_RdFL(id_magnitude_thd_hi);
		AAD_Wr01(id_magnitude_thd_hi, idthh, g_stThresholdLevel.stereoIDMagHigh);
		AAD_WrFL(id_magnitude_thd_hi);

		AAD_RdFL(id_magnitude_thd_lo);
		AAD_Wr01(id_magnitude_thd_lo, idthl, g_stThresholdLevel.stereoIDMagLow);
		AAD_WrFL(id_magnitude_thd_lo);

		//Read registers
		AAD_RdFL(id_magnitude_thd_hi);
		AAD_RdFL(id_magnitude_thd_lo);
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : ID High = 0x%X, Low = 0x%X\n",	\
				   AAD_Rd(id_magnitude_thd_hi), AAD_Rd(id_magnitude_thd_lo));
	}

	//Check the threshold mode
	if ( level.mode & LX_AAD_THRES_NICAM_ERROR )
	{
		g_stThresholdLevel.nicamErrorRateHigh = level.nicamErrorRateHigh;
		g_stThresholdLevel.nicamErrorRateLow  = level.nicamErrorRateLow;

		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : nicamErrorRateHigh = 0x%X, nicamErrorRateLow = 0x%X\n",	\
				   g_stThresholdLevel.nicamErrorRateHigh, g_stThresholdLevel.nicamErrorRateLow);

		//NICAM Error Rate AAOS Threshold High / Low Register
		//Org. threshold high is 0x50 and low is 0x30 Value
		AAD_RdFL(nicam_error_rate_aaos_thd_hi);
		AAD_Wr01(nicam_error_rate_aaos_thd_hi, nierth, g_stThresholdLevel.nicamErrorRateHigh);
		AAD_WrFL(nicam_error_rate_aaos_thd_hi);

		AAD_RdFL(nicam_error_rate_aaos_thd_lo);
		AAD_Wr01(nicam_error_rate_aaos_thd_lo, niertl, g_stThresholdLevel.nicamErrorRateLow);
		AAD_WrFL(nicam_error_rate_aaos_thd_lo);

		//Read registers
		AAD_RdFL(nicam_error_rate_aaos_thd_hi);
		AAD_RdFL(nicam_error_rate_aaos_thd_lo);
		AAD_DEBUG("AAD_SIF_SetBTSCA2Level : NICAM High = 0x%X, Low = 0x%X\n",	\
				   AAD_Rd(nicam_error_rate_aaos_thd_hi), AAD_Rd(nicam_error_rate_aaos_thd_lo));
	}

	return RET_OK;
}


/**
 * Sets a SIF Band(B/G, I, DK, L, M) and returns the strength of input signal.
 * Gets also a detected sound system and signal strength level.
 * @see
*/
int AAD_SIF_GetBandDetect ( LX_AAD_SIF_SYSTEM_INFO_T	*pstSystemInfo )
{
	UINT8		value;
	int			iRetVal = RET_OK;

	UINT16		ui16Carrier1Quality   = 0;
	UINT16		ui16Carrier1Magnitude = 0;

	UINT8 	  	*debugString[6] = {"BG", "I", "DK", "L", "MN", "UNKNOWN" };
	UINT8		ui8SetStrNum = 0, ui8DebugStrNum = 0;

	LX_AAD_SIF_SOUNDSYSTEM_T 	eTVSoundSystem = LX_AAD_SIF_SYSTEM_UNKNOWN;

#ifdef	AAD_BAND_DETECT_DEBUG
	UINT8	  i;
#endif

	//Do ASD using both carrier (6.5 and  4.5MHz carrier) and return detected TV system
	iRetVal = AAD_DoASD( pstSystemInfo->setSystem );
	if ( RET_OK != iRetVal )
	{
		AAD_DEBUG ("ASD Failed!!! \n");
	}

	//Check a detected sound system
	eTVSoundSystem = AAD_GetSoundSystem();

	//In case of China signal, M/N ASD function is sometimes failed. In this case, MW detects M/N signal by video signal.
	if (eTVSoundSystem != pstSystemInfo->setSystem)
	{
		AAD_DEBUG ("ASD system : SET = 0x%X, Get = 0x%X !!! \n", pstSystemInfo->setSystem, eTVSoundSystem);
	}

	//In case of China signal, M/N ASD function is sometimes faild, so do not check error case.
	if (iRetVal == RET_OK)
	{
#ifndef	AAD_BAND_DETECT_DEBUG
		//Wait to read signal strength
		AAD_SIF_TIME_DELAY_MS (SIGNAL_QUALITY_DELAY);

		AAD_RdFL(carrier1_fm_quality0);
		AAD_Rd01(carrier1_fm_quality0, cr1ql, value);
		ui16Carrier1Quality  = value << 8;

		AAD_RdFL(carrier1_fm_quality1);
		AAD_Rd01(carrier1_fm_quality1, cr1ql, value);
		ui16Carrier1Quality += value;
		AAD_DEBUG("Carrier1 Quality Current   0x%4X\n", ui16Carrier1Quality );

		AAD_RdFL(carrier1_average_mag0);
		AAD_Rd01(carrier1_average_mag0, cr1mg, value);
		ui16Carrier1Magnitude  = value << 8;

		AAD_RdFL(carrier1_average_mag1);
		AAD_Rd01(carrier1_average_mag1, cr1mg, value);
		ui16Carrier1Magnitude += value;
		AAD_DEBUG("Carrier1 Magnitude Current 0x%4X\n", ui16Carrier1Magnitude );

		//Copy a detected system
		pstSystemInfo->detectedSystem = eTVSoundSystem;

		//Copy SNR value for Carrier 1 Quality and Magnitude
		pstSystemInfo->bandStrength  = ui16Carrier1Quality;
		pstSystemInfo->bandMagnitude = ui16Carrier1Magnitude;

#else	//For test only
		for( i = 0; i < 10; i++ )
		{
			AAD_RdFL(carrier1_fm_quality0);
			AAD_Rd01(carrier1_fm_quality0, cr1ql, value);
			ui16Carrier1Quality  = value << 8;

			AAD_RdFL(carrier1_fm_quality1);
			AAD_Rd01(carrier1_fm_quality1, cr1ql, value);
			ui16Carrier1Quality += value;
			AAD_DEBUG_TMP("%d : Carrier1 Quality 0x%4X\n", i, ui16Carrier1Quality );

			AAD_RdFL(carrier1_average_mag0);
			AAD_Rd01(carrier1_average_mag0, cr1mg, value);
			ui16Carrier1Magnitude  = value << 8;

			AAD_RdFL(carrier1_average_mag1);
			AAD_Rd01(carrier1_average_mag1, cr1mg, value);
			ui16Carrier1Magnitude += value;
			AAD_DEBUG("Carrier1 Magnitude Current 0x%4X\n", ui16Carrier1Magnitude );

			AAD_SIF_TIME_DELAY_MS (100);
		}

		//Copy a detected system
		pstSystemInfo->detectedSystem = eTVSoundSystem;

		//Copy SNR value for Carrier 1 Quality and Magnitude
		pstSystemInfo->bandStrength  = ui16Carrier1Quality;
		pstSystemInfo->bandMagnitude = ui16Carrier1Magnitude;
#endif	//#ifndef	AAD_SIGNAL_QUALITY_DEBUG
	}
	else
	{
		//Check a detected sound system
		pstSystemInfo->detectedSystem = LX_AAD_SIF_SYSTEM_UNKNOWN;

		//Copy SNR value for Carrier 1 Quality and Magnitude to set default value.
		pstSystemInfo->bandStrength  = 0;
		pstSystemInfo->bandMagnitude = 0;
	}

	//Set set string number
	if( pstSystemInfo->setSystem <= LX_AAD_SIF_SYSTEM_MN )
		ui8SetStrNum = pstSystemInfo->setSystem;
	else
		ui8SetStrNum = LX_AAD_SIF_SYSTEM_MN + 1;

	//Set debug string number
	if( eTVSoundSystem <= LX_AAD_SIF_SYSTEM_MN )
		ui8DebugStrNum = eTVSoundSystem;
	else
		ui8DebugStrNum = LX_AAD_SIF_SYSTEM_MN + 1;

	AAD_DEBUG_TMP("AAD_SIF_GetBandDetect : set : %d(%s) => detect : %d(%s), Quality 0x%X, Magnitude 0x%xX\n\n",	\
			   	   pstSystemInfo->setSystem, debugString[ui8SetStrNum], eTVSoundSystem, debugString[ui8DebugStrNum],	\
				   pstSystemInfo->bandStrength, pstSystemInfo->bandMagnitude);

	return RET_OK;
}

/**
 * Checks if a SIF standard such as NICAM, A2(DK), FM exists in the input SIF signal.
 * Gets also a detected sound standard and signal existence..
 * @see
*/
int AAD_SIF_CheckSoundMode ( LX_AAD_SIF_STANDARD_INFO_T *pstStandardInfo )
{
	int			eRetVal = RET_OK;
	UINT8		ui8SysSel;
	UINT8		ui8Sndstat, ui8MonoSt, ui8BiSap;

	UINT8		debugMsg[DEBUG_MSG_BUFFER_SIZE];	//50

	//Set dafault value
	pstStandardInfo->detectedStandard = LX_AAD_SIF_NOT_AVAILE;
	pstStandardInfo->availability     = LX_AAD_SIF_ABSENT;
	strcpy(debugMsg, "RESET ");

	//Read a decoding TV system & standard select and status register
	AAD_RdFL(system_select);
	AAD_Rd01(system_select, syssel, ui8SysSel);

	AAD_RdFL(status);
	AAD_Rd01(status, sndstat,   ui8Sndstat);
	AAD_Rd01(status, mono_str,  ui8MonoSt);
	AAD_Rd01(status, bisap,	    ui8BiSap);
	AAD_PRINT("AAD_SIF_CheckSoundMode : SysSel = %d, status = 0x%2X, sndstat = %d\n",	\
										ui8SysSel, AAD_Rd(status), ui8Sndstat);

	switch ( pstStandardInfo->setStandard )
	{
		case LX_AAD_SIF_NICAM:

			//Check NICAM mode
			strcpy(debugMsg, "NICAM ");

			switch (ui8SysSel)
			{
				case 0x1:
				case 0x2:
				case 0x3:
				case 0x7:
					pstStandardInfo->detectedStandard = LX_AAD_SIF_NICAM;
					AAD_PRINT("LX_AAD_SIF_NICAM\n");

					//Check the availability of SIF carrier
					if ( ui8Sndstat == 0x2 )	//NICAM(digital sound) is available. This status needs long time to set value.
					{
						pstStandardInfo->availability = LX_AAD_SIF_PRESENT;
						AAD_PRINT("LX_AAD_SIF_PRESENT\n");
						strcat(debugMsg, "OK");
					}
					else
					{
						pstStandardInfo->availability = LX_AAD_SIF_ABSENT;
						AAD_PRINT("LX_AAD_SIF_ABSENT\n");
						strcat(debugMsg, "NG");
					}
					break;

				default:
					pstStandardInfo->availability = LX_AAD_SIF_ABSENT;
					AAD_PRINT("LX_AAD_SIF_ABSENT\n");
					strcat(debugMsg, "NG");
					break;
			}
			break;

		case LX_AAD_SIF_A2:

			//Check A2 mode
			strcpy(debugMsg, "A2 ");

			switch (ui8SysSel)
			{
				case 0x0:
				case 0x4:
				case 0x5:
				case 0x6:
				case 0x8:
					pstStandardInfo->detectedStandard = LX_AAD_SIF_A2;
					AAD_PRINT("LX_AAD_SIF_A2\n");
					strcpy(debugMsg, "A2 ");
					break;
			}

			//Check the availability of SIF carrier
			if ( ui8Sndstat == 0x2 )	//NICAM(digital sound) is available. This status needs long time to set value.
			{
				pstStandardInfo->availability = LX_AAD_SIF_ABSENT;
				AAD_PRINT("LX_AAD_SIF_ABSENT\n");
				strcat(debugMsg, "NG(NICAM)");
			}
			else if ( ui8Sndstat == 0x0 )	//Analog Sound Standard(AM/FM) active
			{
				pstStandardInfo->availability = LX_AAD_SIF_PRESENT;
				AAD_PRINT("LX_AAD_SIF_PRESENT\n");
				strcat(debugMsg, "PRESENT");
			}
			else
			{
				pstStandardInfo->availability = LX_AAD_SIF_ABSENT;
				AAD_PRINT("LX_AAD_SIF_ABSENT\n");
				strcat(debugMsg, "NG");
			}
			break;

		case LX_AAD_SIF_A2_DK1:
		case LX_AAD_SIF_A2_DK2:
		case LX_AAD_SIF_A2_DK3:

			//Check A2 mode
			strcpy(debugMsg, "A2 ");

			switch (ui8SysSel)
			{
				case 0x4:
					pstStandardInfo->detectedStandard = LX_AAD_SIF_A2_DK1;
					AAD_PRINT("LX_AAD_SIF_A2_DK1\n");
					strcpy(debugMsg, "A2_DK1 ");
					break;

				case 0x5:
					pstStandardInfo->detectedStandard = LX_AAD_SIF_A2_DK2;
					AAD_PRINT("LX_AAD_SIF_A2_DK2\n");
					strcpy(debugMsg, "A2_DK2 ");
					break;

				case 0x6:
					pstStandardInfo->detectedStandard = LX_AAD_SIF_A2_DK3;
					AAD_PRINT("LX_AAD_SIF_A2_DK3\n");
					strcpy(debugMsg, "A2_DK3 ");
					break;
			}

			//Check the availability of SIF carrier
			if ( ui8Sndstat == 0x2 )	//NICAM(digital sound) is available. This status needs long time to set value.
			{
				pstStandardInfo->availability = LX_AAD_SIF_ABSENT;
				AAD_PRINT("LX_AAD_SIF_ABSENT\n");
				strcat(debugMsg, "NG(NICAM)");
			}
			else if ( ui8Sndstat == 0x0 )	//Analog Sound Standard(AM/FM) active
			{
				pstStandardInfo->availability = LX_AAD_SIF_PRESENT;
				AAD_PRINT("LX_AAD_SIF_PRESENT\n");
				strcat(debugMsg, "PRESENT");
			}
			else
			{
				pstStandardInfo->availability = LX_AAD_SIF_ABSENT;
				AAD_PRINT("LX_AAD_SIF_ABSENT\n");
				strcat(debugMsg, "NG");
			}
			break;

		case LX_AAD_SIF_FM:

			//Check FM mode
			strcpy(debugMsg, "A2 ");

			if ( (g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT) )
			{
				if ( ui8SysSel == 0xB )
				{
					pstStandardInfo->detectedStandard = LX_AAD_SIF_FM;
					AAD_PRINT("LX_AAD_SIF_FM\n");
					strcpy(debugMsg, "BTSC FM ");
				}
			}
			else if ( g_eAadInitType == LX_AAD_INIT_ATSC_EIAJ_SELECT )
			{
				if ( ui8SysSel == 0x9 )
				{
					pstStandardInfo->detectedStandard = LX_AAD_SIF_FM;
					AAD_PRINT("LX_AAD_SIF_FM\n");
					strcpy(debugMsg, "EIA-J FM ");
				}
			}
			else
			{
				if ( (( 0x1 <= ui8SysSel ) && ( ui8SysSel <= 0x3 ))\
					||( 0x7 == ui8SysSel )
				   ||(( 0xB <= ui8SysSel ) && ( ui8SysSel <= 0xF )) )
				{
					pstStandardInfo->detectedStandard = LX_AAD_SIF_FM;
					AAD_PRINT("FM \n");
					strcpy(debugMsg, "FM ");
				}
			}

			//Check the availability of SIF carrier
			if ( ui8Sndstat == 0x2 )	//NICAM(digital sound) is available. This status needs long time to set value.
			{
				pstStandardInfo->availability = LX_AAD_SIF_ABSENT;
				AAD_PRINT("LX_AAD_SIF_ABSENT\n");
				strcat(debugMsg, "NG(NICAM)");
			}
			else if ( (ui8Sndstat == 0x0)	//Analog Sound Standard(AM/FM) active
				    ||(ui8MonoSt == 1) || (ui8BiSap == 1) )	//No signal case, this value is set.
			{
				pstStandardInfo->availability = LX_AAD_SIF_PRESENT;
				AAD_PRINT("LX_AAD_SIF_PRESENT\n");
				strcat(debugMsg, "PRESENT");
			}
			else	//Bad reception condition of analogl sound -> PRESENT
			{
				pstStandardInfo->availability = LX_AAD_SIF_PRESENT;
				AAD_PRINT("LX_AAD_SIF_PRESENT\n");
				strcat(debugMsg, "PRESENT");
			}
			break;

		case LX_AAD_SIF_DETECTING_AVALIBILITY:
			eRetVal = RET_INVALID_PARAMS;
			strcpy(debugMsg, "DETEC ");
			break;

		default:
			eRetVal = RET_INVALID_PARAMS;
			strcpy(debugMsg, "INVAL ");
			break;
	}

	//For debugging print
	if(g_io_stStandardInfo.availability != pstStandardInfo->availability)
	{
		AAD_DEBUG_TMP("CheckSoundMode : %d -> %d(%s)(SysSel = %d, status = 0x%2X)\n",	\
					   pstStandardInfo->setStandard, pstStandardInfo->detectedStandard, debugMsg, ui8SysSel, AAD_Rd(status));
	}
	else if((g_ui32AadPrintCount % DEBUG_MSG_PRINT_TIME) == 0) //DDI calls every 30 ms
	{
		AAD_DEBUG_TMP("CheckSoundMode : %d -> %d(%s)(SysSel = %d, status = 0x%2X)\n",	\
					   pstStandardInfo->setStandard, pstStandardInfo->detectedStandard, debugMsg, ui8SysSel, AAD_Rd(status));
	}

	//Copy for global value
	g_io_stStandardInfo.detectedStandard = pstStandardInfo->detectedStandard;
	g_io_stStandardInfo.availability 	 = pstStandardInfo->availability;

	AAD_PRINT("AAD_SIF_CheckSoundMode : %d -> %d(%s)(SysSel = %d, status = 0x%2X)\n",	\
			   pstStandardInfo->setStandard, pstStandardInfo->detectedStandard, debugMsg, ui8SysSel, AAD_Rd(status));

 	return eRetVal;
}

/**
 * Gets BTSC or A2 stereo level in the input SIF signal.
 * The current stereo level is returned using LX_AAD_SIF_THRESHOLD_T value.
 * @see
*/
int AAD_SIF_GetBTSCA2Level ( LX_AAD_SIF_THRESHOLD_T	*pstCurStereoLevel )
{
	//Check the threshold mode
	if(pstCurStereoLevel->mode & LX_AAD_THRES_CAR2_MAGITUDE)
	{
		//Get a BTSC and A2 Threshold level
		pstCurStereoLevel->carrier2Magitude = g_stThresholdLevel.carrier2Magitude;

		AAD_DEBUG("AAD_SIF_GetBTSCA2Level : carrier2Magitude = 0x%X\n", g_stThresholdLevel.carrier2Magitude);
	}

	//Check the threshold mode
	if(pstCurStereoLevel->mode & LX_AAD_THRES_CAR2_QUALITY_HI)
	{
		//Get a BTSC and A2 Threshold level
		pstCurStereoLevel->carrier2QualityHigh = g_stThresholdLevel.carrier2QualityHigh;

		AAD_DEBUG("AAD_SIF_GetBTSCA2Level : CAR2 Quality High = 0x%X, Low = 0x%X\n",	\
				   g_stThresholdLevel.carrier2QualityHigh, g_stThresholdLevel.carrier2QualityLow);
	}

	//Check the threshold mode
	if(pstCurStereoLevel->mode & LX_AAD_THRES_CAR2_QUALITY_LO)
	{
		//Get a BTSC and A2 Threshold level
		pstCurStereoLevel->carrier2QualityLow  = g_stThresholdLevel.carrier2QualityLow;

		AAD_DEBUG("AAD_SIF_GetBTSCA2Level : CAR2 Quality High = 0x%X, Low = 0x%X\n",	\
				   g_stThresholdLevel.carrier2QualityHigh, g_stThresholdLevel.carrier2QualityLow);
	}

	//Check the threshold mode
	if(pstCurStereoLevel->mode & LX_AAD_THRES_ID_MAGNITUDE)
	{
		//Get a BTSC and A2 Threshold level
		pstCurStereoLevel->stereoIDMagHigh = g_stThresholdLevel.stereoIDMagHigh;
		pstCurStereoLevel->stereoIDMagLow  = g_stThresholdLevel.stereoIDMagLow;

		AAD_DEBUG("AAD_SIF_GetBTSCA2Level : ID High = 0x%X, Low = 0x%X\n",	\
				   g_stThresholdLevel.stereoIDMagHigh, g_stThresholdLevel.stereoIDMagLow);
	}

	//Check the threshold mode
	if ( pstCurStereoLevel->mode & LX_AAD_THRES_NICAM_ERROR )
	{
		//Get a NICAM Error Rate Threshold level
		pstCurStereoLevel->nicamErrorRateHigh = g_stThresholdLevel.nicamErrorRateHigh;
		pstCurStereoLevel->nicamErrorRateLow  = g_stThresholdLevel.nicamErrorRateLow;

		AAD_DEBUG("AAD_SIF_GetBTSCA2Level : NICAM High = 0x%X, Low = 0x%X\n",	\
				   g_stThresholdLevel.nicamErrorRateHigh, g_stThresholdLevel.nicamErrorRateLow);
	}

	//Debug Print A2 Threshold Level
	//Read registers
	AAD_RdFL(carrier2_quality_aaos_thd_hi);
	AAD_RdFL(carrier2_quality_aaos_thd_lo);
	AAD_RdFL(id_magnitude_thd_hi);
	AAD_RdFL(id_magnitude_thd_lo);
	AAD_RdFL(nicam_error_rate_aaos_thd_hi);
	AAD_RdFL(nicam_error_rate_aaos_thd_lo);


	AAD_DEBUG("AAD_SIF_GetBTSCA2Level : carrier2Magitude = 0x%X\n", g_stThresholdLevel.carrier2Magitude);
	AAD_DEBUG("AAD_SIF_GetBTSCA2Level : CAR2 High  = 0x%X, Low = 0x%X, ID High = 0x%X, Low = 0x%X\n",	\
			   AAD_Rd(carrier2_quality_aaos_thd_hi), AAD_Rd(carrier2_quality_aaos_thd_lo),	\
			   AAD_Rd(id_magnitude_thd_hi), AAD_Rd(id_magnitude_thd_lo));
	AAD_DEBUG("AAD_SIF_GetBTSCA2Level : NICAM High = 0x%X, Low = 0x%X\n",	\
			   AAD_Rd(nicam_error_rate_aaos_thd_hi), AAD_Rd(nicam_error_rate_aaos_thd_lo));

	return RET_OK;
}


/**
 * Gets the setting value of analog TV stereo mode by user or analog task.
 * Analog Audio Output Mode is returned from User Mode or Analog Audio Task in Analog Audio Decoder.
 * @see
*/
int AAD_SIF_GetCurAnalogMode ( LX_ATV_AUDIO_MODE_GET_T *pAudioMode )
{
	UINT8 		ui8SysSel;
	UINT8		ui8PrmCarDet, ui8SecCarDet, ui8MonoSt, ui8BiSap;
	UINT8		ui8NicamCtrlBits;

	UINT8		value;
	UINT16	  	ui16Carrier2Magnitude = 0;
	UINT16	  	ui16Carrier2Quality   = 0;
	UINT8		ui8StIdMag = 0;
	UINT8		ui8BiIdMag = 0;

	LX_ATV_AUDIO_MODE_GET_T 	eAudioMode = LX_ATV_GET_PAL_MONO;

#ifndef AAD_SIGNAL_QUALITY_DEBUG
	UINT16	  	ui16Carrier1Magnitude = 0;
	UINT16	  	ui16Carrier1Quality   = 0;
#endif

	UINT8 *debugString[25] = {"PAL_MONO", 	"PAL_STEREO", 	"PAL_DUAL", 	"NICAM_MONO", 		"NICAM_STEREO",	\
							  "NICAM_DUAL", "PAL_UNKNOWN", 	"ERROR1", 		"ERROR2", 			"ERROR3", \
							  "ERROR4", 	"ERROR5", 		"ERROR6", 		"ERROR7", 			"ERROR8", \
							  "ERROR9", 	"A2_MONO", 		"A2_STEREO", 	"A2_SAP", 			"A2_UNKNOWN", \
							  "BTSC_MONO", 	"BTSC_STEREO", 	"BTSC_SAP_MONO","BTSC_SAP_STEREO", 	"BTSC_UNKNOWN" };

	//Mono, Stereo and SAP detection checks primary and secondardary carrier  because of THAT IP bug.
	//Read a decoding TV system & standard select and status register
	AAD_RdFL(system_select);
	AAD_Rd01(system_select, syssel, ui8SysSel);

	AAD_RdFL(status);
	AAD_Rd01(status, prmcardet, ui8PrmCarDet);
	AAD_Rd01(status, seccardet, ui8SecCarDet);
	AAD_Rd01(status, mono_str,  ui8MonoSt);
	AAD_Rd01(status, bisap,	    ui8BiSap);

	//Audio Output Result Register
	switch ( ui8SysSel )
	{
		case 0x1://FM-Mono/NICAM(B/G)
		case 0x2://AM-Mono/NICAM(L)
		case 0x3://FM-Mono/NICAM(I)
		case 0x7://FM-Mono/NICAM (D/K)
		{
			//Just read the high byte so that low byte will be updated
			AAD_RdFL(nicam_control_bits0);
			AAD_RdFL(nicam_control_bits1);
			AAD_Rd01(nicam_control_bits1, nicctl, ui8NicamCtrlBits);

			ui8NicamCtrlBits = (ui8NicamCtrlBits & 0xE) >> 1;
			AAD_PRINT("NICAM Control bits 0x%x\n", ui8NicamCtrlBits);

			if ((ui8BiSap == 1) && (ui8NicamCtrlBits == 0x2 ))			//Dual Mode
			{
				eAudioMode = LX_ATV_GET_PAL_NICAM_DUAL;
			}
			else if ((ui8MonoSt == 1) && (ui8NicamCtrlBits == 0x0 ))	//Stereo Mode
			{
				eAudioMode = LX_ATV_GET_PAL_NICAM_STEREO;
			}
			else if ((ui8MonoSt == 0) && (ui8NicamCtrlBits == 0x1 ))	//Mono Mode
			{
				eAudioMode = LX_ATV_GET_PAL_NICAM_MONO;
			}
			else if ((ui8MonoSt == 0) && (ui8NicamCtrlBits == 0x3 ))	//Data Mode
			{
				eAudioMode = LX_ATV_GET_PAL_NICAM_MONO;
			}
			else //default
			{
				AAD_ISR_DEBUG("PAL NICAM : Unknown -> Mono(status = 0x%X)\n", AAD_Rd(status));

				eAudioMode = LX_ATV_GET_PAL_MONO;
			}
		}
		break;

		case 0x0://FM-Stereo (A2)
		case 0x4://FM-Stereo (A2, D/K1)
		case 0x5://FM-Stereo (A2, D/K2)
		case 0x6://FM-Stereo (A2, D/K3)
		{
			if ((ui8PrmCarDet == 1) && (ui8SecCarDet == 1))
			{
				if (ui8MonoSt == 1)		//Stereo
				{
					eAudioMode = LX_ATV_GET_PAL_STEREO;
				}
				else if (ui8BiSap == 1)	//Bilingual
				{
					eAudioMode = LX_ATV_GET_PAL_DUAL;
				}
				else if (ui8MonoSt == 0)//Mono
				{
					eAudioMode = LX_ATV_GET_PAL_MONO;
				}
				else	//default 1 : Unknown -> Mono
				{
					AAD_ISR_DEBUG("PAL A2 : 1_Unknown -> Mono(status = 0x%X)\n", AAD_Rd(status));

					eAudioMode = LX_ATV_GET_PAL_UNKNOWN;
				}
			}
			else if (ui8PrmCarDet == 1)
			{
				if (ui8MonoSt == 0)		//Mono
				{
					eAudioMode = LX_ATV_GET_PAL_MONO;
				}
				else	//default 2 : Unknown -> Mono(GP3 SW search other sound mode if audio mode is unknown.)
				{
					AAD_ISR_DEBUG("PAL A2 : 2_Unknown -> Mono(status = 0x%X)\n", AAD_Rd(status));

					eAudioMode = LX_ATV_GET_PAL_UNKNOWN;
				}
			}
			else if (ui8SecCarDet == 1)
			{
				AAD_RdFL(stereo_id_magnitude);
				AAD_Rd01(stereo_id_magnitude, stmag, ui8StIdMag);

				AAD_RdFL(bilingual_id_magnitude);
				AAD_Rd01(bilingual_id_magnitude, bimag, ui8BiIdMag);

				//0xF8 : ID Magnitude for Turkey(field stream issue : Stereo)
				if( (ui8StIdMag >= ID_MAG_THRES_DVB_A2_BIG)	\
				  ||(ui8BiIdMag >= ID_MAG_THRES_DVB_A2_BIG) )
				{
					//This case statement check only ST/ID value.(The ui8MonoSt and ui8BiSap varies continuously.)
					if(ui8StIdMag >= ui8BiIdMag)
					{
						eAudioMode = LX_ATV_GET_PAL_STEREO;
					}
					else
					{
						eAudioMode = LX_ATV_GET_PAL_DUAL;
					}
				}
				else	//default 3 : Unknown -> Mono(GP3 SW search other sound mode if audio mode is unknown.)
				{
					AAD_ISR_DEBUG("PAL A2 : 3_Unknown -> Mono(status = 0x%X)\n", AAD_Rd(status));

					*pAudioMode = LX_ATV_GET_PAL_UNKNOWN;
				}
			}
			else		//default 4 : Unknown -> Mono
			{
				AAD_ISR_DEBUG("PAL A2 : 4_Unknown -> Mono(status = 0x%X)\n", AAD_Rd(status));

				eAudioMode = LX_ATV_GET_PAL_UNKNOWN;
			}
			break;
		}

		case 0x8://FM-Stereo (A2, M/N)
		{
			//For default case
			eAudioMode = LX_ATV_GET_NTSC_A2_MONO;

			if(ui8PrmCarDet == 1)
			{
				//Check for HE QE TEST SIF signal
				OS_LockMutex(&g_AAD_Sema);
				AAD_RdFL(carrier2_fm_quality0);
				AAD_Rd01(carrier2_fm_quality0, cr2ql, value);
				ui16Carrier2Quality  = value << 8;
				OS_UnlockMutex(&g_AAD_Sema);

				OS_LockMutex(&g_AAD_Sema);
				AAD_RdFL(carrier2_fm_quality1);
				AAD_Rd01(carrier2_fm_quality1, cr2ql, value);
				ui16Carrier2Quality += value;
				OS_UnlockMutex(&g_AAD_Sema);

				if(ui16Carrier2Quality >= CAR2_QUAL_THRES_HI_QC_TEST)	//0x600
				{
					//Org. threshold high is 0x64 and low is 0x14.
					AAD_RdFL(id_magnitude_thd_hi);
					AAD_Wr01(id_magnitude_thd_hi, idthh, g_stThresholdLevel.stereoIDMagHigh);
					AAD_WrFL(id_magnitude_thd_hi);

					AAD_RdFL(id_magnitude_thd_lo);
					AAD_Wr01(id_magnitude_thd_lo, idthl, g_stThresholdLevel.stereoIDMagLow);
					AAD_WrFL(id_magnitude_thd_lo);
				}
				else
				{
					//Org. threshold high is 0x64 and low is 0x14.
					AAD_RdFL(id_magnitude_thd_hi);
					AAD_Wr01(id_magnitude_thd_hi, idthh, ID_MAG_THRES_HI_ATSC_A2);	//0x70
					AAD_WrFL(id_magnitude_thd_hi);

					AAD_RdFL(id_magnitude_thd_lo);
					AAD_Wr01(id_magnitude_thd_lo, idthl, ID_MAG_THRES_LO_ATSC_A2);	//0x12
					AAD_WrFL(id_magnitude_thd_lo);
				}

				//Check for Live or Field SIF signal
				OS_LockMutex(&g_AAD_Sema);
				AAD_RdFL(carrier2_average_mag0);
				AAD_Rd01(carrier2_average_mag0, cr2mg, value);
				ui16Carrier2Magnitude  = value << 8;
				OS_UnlockMutex(&g_AAD_Sema);

				OS_LockMutex(&g_AAD_Sema);
				AAD_RdFL(carrier2_average_mag1);
				AAD_Rd01(carrier2_average_mag1, cr2mg, value);
				ui16Carrier2Magnitude += value;
				OS_UnlockMutex(&g_AAD_Sema);

				AAD_RdFL(stereo_id_magnitude);
				AAD_Rd01(stereo_id_magnitude, stmag, ui8StIdMag);

				AAD_RdFL(bilingual_id_magnitude);
				AAD_Rd01(bilingual_id_magnitude, bimag, ui8BiIdMag);

				//0xF8 : ID Magnitude for Korea(Chungnam Asan field stream issue : Stereo)
				if( (ui8SecCarDet == 1)
				  &&((ui8StIdMag >= ID_MAG_THRES_ATSC_A2_BIG) || (ui8BiIdMag >= ID_MAG_THRES_ATSC_A2_BIG)) )
				{
					//This case statement check only ST/ID value.(The ui8MonoSt and ui8BiSap varies continuously.)
					if(ui8StIdMag >= ui8BiIdMag)
					{
						eAudioMode = LX_ATV_GET_NTSC_A2_STEREO;
					}
					else
					{
						eAudioMode = LX_ATV_GET_NTSC_A2_SAP;
					}
				}
				//Normal Case  or 0x0CD0 : -22.5(?)dbFS for Korea(West Door field stream issue : Mono)
				//Normal Case  or 0x0800 : -22.5(?)dbFS for Korea(Ra Sung field stream issue : Stereo)
				else if( (ui8SecCarDet == 1) && (ui16Carrier2Magnitude >= CAR2_MAG_THRES_ATSC_A2) )
				{
					if(ui8MonoSt == 1)
					{
						eAudioMode = LX_ATV_GET_NTSC_A2_STEREO;
					}
					else if(ui8BiSap == 1)
					{
						eAudioMode = LX_ATV_GET_NTSC_A2_SAP;
					}
				}
			}
			break;
		}

		case 0xA://FM-Stereo + SAP (BTSC) or FM-Stereo (Radio - Europe)
		case 0xC://FM-Stereo (Radio - Europe)
		case 0xD://FM-Stereo (Radio - Europe)
		case 0xE://FM-Stereo (Radio - Europe)
		{
			if ( (g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)	 //FM-Stereo + SAP (BTSC)
			   ||(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT) )
			{
				if (ui8PrmCarDet == 1)
				{
					if ((ui8BiSap == 1) && (ui8MonoSt == 1))	//SAP and Stereo
					{
						eAudioMode = LX_ATV_GET_NTSC_BTSC_SAP_STEREO;
					}
					else if (ui8BiSap == 1)		//SAP and Mono
					{
						eAudioMode = LX_ATV_GET_NTSC_BTSC_SAP_MONO;
					}
					else if (ui8MonoSt == 1)	//Stereo
					{
						eAudioMode = LX_ATV_GET_NTSC_BTSC_STEREO;
					}
					else if (ui8MonoSt == 0)	//Mono
					{
						eAudioMode = LX_ATV_GET_NTSC_BTSC_MONO;
					}
					else	//default 1 : Unknown -> Mono(NTSC system request any audio mode.)
					{
						AAD_ISR_DEBUG("BTSC : 1_Undef -> Mono(status = 0x%X)\n", AAD_Rd(status));

						eAudioMode = LX_ATV_GET_NTSC_BTSC_MONO;
					}
				}
				else		//default 2 : Unknown -> Mono(NTSC system request any audio mode.)
				{
					AAD_ISR_DEBUG("BTSC : 2_Undef -> Mono(status = 0x%X)\n", AAD_Rd(status));

					eAudioMode = LX_ATV_GET_NTSC_BTSC_MONO;
				}
			}
			else //FM-Stereo (Radio - Europe)
			{
				if (ui8PrmCarDet == 1)
				{
					if (ui8BiSap == 1)
					{
						eAudioMode = LX_ATV_GET_PAL_DUAL;
					}
					else if (ui8MonoSt == 1)
					{
						eAudioMode = LX_ATV_GET_PAL_STEREO;
					}
					else if (ui8MonoSt == 0)
					{
						eAudioMode = LX_ATV_GET_PAL_MONO;
					}
					else	//default 1 : Unknown
					{
						AAD_ISR_DEBUG("FM-Radio : 1_Undef(status = 0x%X)\n", AAD_Rd(status));

						eAudioMode = LX_ATV_GET_PAL_UNKNOWN;
					}
				}
				else		//default 2 : Unknown
				{
					AAD_ISR_DEBUG("FM-Radio : 2_Undef(status = 0x%X)\n", AAD_Rd(status));

					eAudioMode = LX_ATV_GET_PAL_UNKNOWN;
				}
			}
			break;
		}

		case 0x9://FM-Stereo (EIA-J)
		case 0xB://FM-Stereo (Radio - US)
		{
			if (ui8PrmCarDet == 1)
			{
				if (ui8MonoSt == 1)		//Stereo
				{
					eAudioMode = LX_ATV_GET_NTSC_A2_STEREO;
				}
				else if (ui8BiSap == 1)	//Bilingual
				{
					eAudioMode = LX_ATV_GET_NTSC_A2_SAP;
				}
				else if (ui8MonoSt == 0)//Mono
				{
					eAudioMode = LX_ATV_GET_NTSC_A2_MONO;
				}
				else	//default 1 : Unknown -> Mono
				{
					AAD_ISR_DEBUG("NTSC : 1_Undef -> Mono(status = 0x%X)\n", AAD_Rd(status));

					eAudioMode = LX_ATV_GET_NTSC_A2_MONO;
				}
			}
			else		//default 2 : Unknown -> Mono
			{
				AAD_ISR_DEBUG("NTSC : 2_Undef -> Mono(status = 0x%X)\n", AAD_Rd(status));

				eAudioMode = LX_ATV_GET_NTSC_A2_MONO;
			}
		}
		break;

		case 0xF:	//unknown/FM-Stereo (Radio - Europe)
		default:
			AAD_ISR_DEBUG("default : PAL unknown(status = 0x%X)\n", AAD_Rd(status));

			eAudioMode = LX_ATV_GET_PAL_UNKNOWN;
			break;
	}

	//Sanity check for debug print
	if( eAudioMode >= LX_ATV_GET_NTSC_BTSC_UNKNOWN )
		eAudioMode = LX_ATV_GET_NTSC_BTSC_UNKNOWN;

	//Check a AAD monitor task for KOREA A2 Signal
	if ( g_eAadInitType == LX_AAD_INIT_ATSC_A2_SELECT )
	{
		//Check a forced ST / BI status
		if(g_MonitorAudioMode != LX_ATV_GET_NTSC_A2_UNKNOWN)
		{
			//Copy for return value to forced mono.
			eAudioMode = LX_ATV_GET_NTSC_A2_MONO;

			if((g_ui32AadPrintCount++ % DEBUG_MSG_PRINT_TIME) == 0)	//DDI calls every 30 ms in DVB mode or 20 ms in ATSC mode
			{
				AAD_DEBUG_TMP("AAD_SIF_GetCurAnalogMode : Get 0x%x(%s)(SysSel = %d, status = 0x%2X)\n",	\
						   	   eAudioMode, debugString[eAudioMode], ui8SysSel, AAD_Rd(status));
			}
		}
	}

	//For debugging print
	if( g_o_eAudioMode != eAudioMode )
	{
		//Read registers
		AAD_RdFL(audio_output_ctrl);

		AAD_DEBUG("GetAnalogMode : 0x%x(%s)(SysSel = %d, status = 0x%2X, output = 0x%2X)\n",	\
				   eAudioMode, debugString[eAudioMode], ui8SysSel, AAD_Rd(status), AAD_Rd(audio_output_ctrl));

#ifndef AAD_SIGNAL_QUALITY_DEBUG
		AAD_RdFL(carrier1_average_mag0);
		AAD_Rd01(carrier1_average_mag0, cr1mg, value);
		ui16Carrier1Magnitude  = value << 8;

		AAD_RdFL(carrier1_average_mag1);
		AAD_Rd01(carrier1_average_mag1, cr1mg, value);
		ui16Carrier1Magnitude += value;

		AAD_RdFL(carrier1_fm_quality0);
		AAD_Rd01(carrier1_fm_quality0, cr1ql, value);
		ui16Carrier1Quality  = value << 8;

		AAD_RdFL(carrier1_fm_quality1);
		AAD_Rd01(carrier1_fm_quality1, cr1ql, value);
		ui16Carrier1Quality += value;

		//Read a high byte first
		OS_LockMutex(&g_AAD_Sema);
		AAD_RdFL(carrier2_average_mag0);
		AAD_Rd01(carrier2_average_mag0, cr2mg, value);
		ui16Carrier2Magnitude  = value << 8;
		OS_UnlockMutex(&g_AAD_Sema);

		OS_LockMutex(&g_AAD_Sema);
		AAD_RdFL(carrier2_average_mag1);
		AAD_Rd01(carrier2_average_mag1, cr2mg, value);
		ui16Carrier2Magnitude += value;
		OS_UnlockMutex(&g_AAD_Sema);

		OS_LockMutex(&g_AAD_Sema);
		AAD_RdFL(carrier2_fm_quality0);
		AAD_Rd01(carrier2_fm_quality0, cr2ql, value);
		ui16Carrier2Quality  = value << 8;
		OS_UnlockMutex(&g_AAD_Sema);

		OS_LockMutex(&g_AAD_Sema);
		AAD_RdFL(carrier2_fm_quality1);
		AAD_Rd01(carrier2_fm_quality1, cr2ql, value);
		ui16Carrier2Quality += value;
		OS_UnlockMutex(&g_AAD_Sema);

		//Read registers
		AAD_RdFL(stereo_id_magnitude);
		AAD_Rd01(stereo_id_magnitude, stmag, ui8StIdMag);

		AAD_RdFL(bilingual_id_magnitude);
		AAD_Rd01(bilingual_id_magnitude, bimag, ui8BiIdMag);

		AAD_DEBUG("Car1Mag = 0x%04x, Car1Ql = 0x%04x\n", ui16Carrier1Magnitude, ui16Carrier1Quality);
		AAD_DEBUG("Car2Mag = 0x%04x, Car2Ql = 0x%04x, StIdMag = 0x%x, BiIdMag = 0x%x\n",	\
				   ui16Carrier2Magnitude, ui16Carrier2Quality, ui8StIdMag, ui8BiIdMag);

#if 0
		AAD_RdFL(stereo_carrier_search_result);
		AAD_RdFL(scs_quality_status);
		AAD_RdFL(asd_carrier_status);

		AAD_DEBUG("SCS Rlt = 0x%04x, SCS St = 0x%04x, ASD St = 0x%04x\n",	\
				   AAD_Rd(stereo_carrier_search_result), AAD_Rd(scs_quality_status), AAD_Rd(asd_carrier_status));
#endif

#endif	//#ifndef AAD_SIGNAL_QUALITY_DEBUG
	}
	else
	{
		if((g_ui32AadPrintCount++ % DEBUG_MSG_PRINT_TIME) == 0)	//DDI calls every 30 ms in DVB mode or 20 ms in ATSC mode
		{
			AAD_DEBUG_TMP("GetAnalogMode  : Mode 0x%x(%s)(SysSel = %d, status = 0x%2X)\n",	\
						   eAudioMode, debugString[eAudioMode], ui8SysSel, AAD_Rd(status));

			if(ui16Carrier2Magnitude)
			{
				AAD_DEBUG_TMP("Car2Mag = 0x%04x, Car2Ql = 0x%04x, StIdMag = 0x%x, BiIdMag = 0x%x\n",	\
						   	   ui16Carrier2Magnitude, ui16Carrier2Quality, ui8StIdMag, ui8BiIdMag);
			}
		}
	}

	//Copy for return value
	*pAudioMode = eAudioMode;

	//Copy for global value
	g_o_eAudioMode = eAudioMode;

	AAD_PRINT("AAD_SIF_GetCurAnalogMode : Get 0x%x(%s)(SysSel = %d, status = 0x%2X)\n",	\
			   eAudioMode, debugString[eAudioMode], ui8SysSel, AAD_Rd(status));

	return RET_OK;
}


/**
 * Checks whether SIF(Sound Intermediate Frequency) carrier exists or not.
 * This function is used for checking the SIF of the current analog signal.
 * @see
*/
int AAD_SIF_GetSIFSatus ( BOOLEAN *pSIFStatus )
{
	UINT8 		ui8StdSel;
	UINT8		ui8PrmCarDet, ui8SecCarDet;
	UINT8		ui8NicamCtrlBits;

	//Check if any primary or secondary carrier is detected using status register and return as boolean value.
	AAD_RdFL(status);
	AAD_Rd01(status, prmcardet, ui8PrmCarDet);
	AAD_Rd01(status, seccardet, ui8SecCarDet);

	//Check primary or secondary carrier status
	if ((ui8PrmCarDet == 1) || (ui8SecCarDet == 1))
	{
		//Read a standard select  register
		//Read Standard Select reg.
		AAD_RdFL(standard_select);
		AAD_Rd01(standard_select, stndrd, ui8StdSel);

		if ( ui8StdSel == 0x7 )
		{
			//Just read the high byte so that low byte will be updated
			AAD_RdFL(nicam_control_bits0);
			AAD_RdFL(nicam_control_bits1);
			AAD_Rd01(nicam_control_bits1, nicctl, ui8NicamCtrlBits);

			ui8NicamCtrlBits = (ui8NicamCtrlBits & 0xE) >> 1;
			AAD_DEBUG("NICAM Control bits 0x%x\n", ui8NicamCtrlBits);
		}

		*pSIFStatus = TRUE;
	}
	else
	{
		*pSIFStatus = FALSE;
	}

	//Check carrier status for debug
	(void) AAD_ASD_CheckCarrierDetection();

	AAD_DEBUG("AAD_SIF_GetSIFSatus : %d(status = 0x%X)\n", *pSIFStatus, AAD_Rd(status) );

	return RET_OK;
}

/**
 * Read write to registers.
 * This function is used for verification.
 * @see
*/
int AAD_SIF_ReadAndWriteReg ( LX_AAD_REG_INFO_T	*pstRegInfo )
{
	switch (pstRegInfo->mode)
	{
		case LX_AAD_REG_READ:
		{
			memcpy ( &(pstRegInfo->readdata), (UINT32*)(((UINT32)gpRealRegAAD) + pstRegInfo->addr), sizeof (UINT32));
			AAD_PRINT ("<<< AAD_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_AAD_REG_WRITE:
		{
			AAD_PRINT (">>> AAD_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			memcpy ((UINT32*)(((UINT32)gpRealRegAAD) + pstRegInfo->addr), (UINT32*)&pstRegInfo->writedata, sizeof (UINT32));
		}
		break;

		case LX_AAD_REG_WRNRD:
		{
			AAD_PRINT (">>> AAD_REG_WRITE addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->writedata);
			memcpy ((UINT32*)(((UINT32)gpRealRegAAD) + pstRegInfo->addr), (UINT32*)&pstRegInfo->writedata, sizeof (UINT32));
			AAD_SIF_TIME_DELAY_MS (AAD_REG_READ_DELAY);
			memcpy ( &(pstRegInfo->readdata), (UINT32*)(((UINT32)gpRealRegAAD) + pstRegInfo->addr), sizeof (UINT32));
			AAD_PRINT ("<<< AAD_REG_READ  addr = 0x%2X, data = 0x%2X\n", pstRegInfo->addr, pstRegInfo->readdata );
		}
		break;

		case LX_AAD_REG_GET_MAX_ADDR:
		{
			pstRegInfo->readdata = sizeof (AAD_REG_T) - 4;
			AAD_PRINT ("<<< LX_AAD_REG_GET_MAX_ADDR	addr = 0x%2X\n", pstRegInfo->readdata );
		}
		break;

		case LX_AAD_REG_TASK_START:
		{
			//Send event for monitor task
			g_suiAadWaitQForMonitor = LX_AAD_TASK_START;
			wake_up_interruptible_all(&Aad_Monitor_WaitQ);

			AAD_PRINT ("<<< LX_AAD_REG_TASK_START(mode = 0x%X)\n", pstRegInfo->mode );
		}
		break;

		case LX_AAD_REG_TASK_STOP:
		{
			//Send event for monitor task
			g_suiAadWaitQForMonitor = LX_AAD_TASK_STOP;
			wake_up_interruptible_all(&Aad_Monitor_WaitQ);

			AAD_PRINT ("<<< LX_AAD_REG_TASK_STOP(mode = 0x%X)\n", pstRegInfo->mode );
		}
		break;

		case LX_AAD_REG_READ_ARRAY:
		case LX_AAD_REG_READ_ALL:
		default:
			return RET_ERROR;
			break;
	}

	return RET_OK;
}

/**
 * Start ASD function.
*/
int AAD_DoASD ( LX_AAD_SIF_SOUNDSYSTEM_T eSoundSystem )
{
	int						eRetVal = RET_ERROR;
	UINT8					ui8SysSel = 0;

	UINT8 *debugString[6] = {"SYSTEM_BG", "SYSTEM_I", "SYSTEM_DK", "SYSTEM_L", "SYSTEM_MN",	\
							 "SYSTEM_UNKNOWN" };

	//do AAOS function if DTV is DVB mode
	if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT )
	{
		//Stop AAOS(Automatic Audio Output Select)
		AAD_RdFL(audio_output_ctrl);
		AAD_Wr01(audio_output_ctrl, aouten, 0);
		AAD_WrFL(audio_output_ctrl);

		//Stop Stereo Carrier Search Control
		AAD_RdFL(stereo_carrier_search_ctrl);
		AAD_Wr01(stereo_carrier_search_ctrl, srchen, 0x0);
		AAD_Wr01(stereo_carrier_search_ctrl, chgmod, 0x0);
		AAD_Wr01(stereo_carrier_search_ctrl, vsphase_y_rnw, 0b000);
		AAD_WrFL(stereo_carrier_search_ctrl);
	}

	switch (eSoundSystem)
	{
		case LX_AAD_SIF_SYSTEM_BG:

			//For workaround, NICAM high frequency area deviation is larger than low area.
			//Set a Carrier Average Frequency Threshold High(ASD, SCS)
			//Org. threshold high is 0x18.
			AAD_RdFL(carrier_average_freq_thd);
			AAD_Wr01(carrier_average_freq_thd, asfth, 0x30);
			AAD_WrFL(carrier_average_freq_thd);

			//4.5MHz => 0x3 - PAL Chrom
			if ( RET_OK != AAD_SetAndStartASD ( AAD_ASD_4_5_MHZ, 0x3, B_G_L_ASD_TIMEOUT_DELAY ) )
			{
				//Set a current sound system.
				AAD_RdFL(system_select);
				AAD_Wr01(system_select, syssel, 0x1); //FM-Mono/NICAM
				AAD_WrFL(system_select);

				//Start Stereo Carrier Search Control(Skip A2DK1, A2DK2 and A2DK3 in BG system)
				//Syssel change automatically when a stereo carrier is detected
				AAD_RdFL(stereo_carrier_search_ctrl);
				AAD_Wr01(stereo_carrier_search_ctrl, srchen, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, chgmod, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, vsphase_y_rnw, 0b111);
				AAD_WrFL(stereo_carrier_search_ctrl);
				break;
			}

			//Start Stereo Carrier Search Control(Skip A2DK1, A2DK2 and A2DK3 in BG system)
			//Syssel change automatically when a stereo carrier is detected
			AAD_RdFL(stereo_carrier_search_ctrl);
			AAD_Wr01(stereo_carrier_search_ctrl, srchen, 0x2);
			AAD_Wr01(stereo_carrier_search_ctrl, chgmod, 0x2);
			AAD_Wr01(stereo_carrier_search_ctrl, vsphase_y_rnw, 0b111);
			AAD_WrFL(stereo_carrier_search_ctrl);

			//If any of B/G system detected, return success
			AAD_RdFL(system_select);
			AAD_Rd01(system_select, syssel, ui8SysSel);
			if ( ui8SysSel == 0x0 )
			{
				AAD_DEBUG("B/G : FM-Stereo(A2) carrier detected\n");
				eRetVal = RET_OK;
			}
			else if ( ui8SysSel == 0x1 )
			{
				AAD_DEBUG("B/G : FM-Mono/NICAM carrier detected\n");
				eRetVal = RET_OK;
			}
			break;

		case LX_AAD_SIF_SYSTEM_I:
			//4.5MHz => 0x3 - PAL Chroma
			if ( RET_OK != AAD_SetAndStartASD ( AAD_ASD_4_5_MHZ, 0x3, M_N_I_ASD_TIMEOUT_DELAY ) )
			{
				//Set a current sound system.
				AAD_RdFL(system_select);
				AAD_Wr01(system_select, syssel, 0x3); //FM-Mono/NICAM
				AAD_WrFL(system_select);

				break;
			}

			//If I system detected in system select, return success
			AAD_RdFL(system_select);
			AAD_Rd01(system_select, syssel, ui8SysSel);
			if ( ui8SysSel == 0x3 )
			{
				AAD_DEBUG("I   : FM-Mono/NICAM carrier detected\n");
				eRetVal = RET_OK;
			}
			break;

		case LX_AAD_SIF_SYSTEM_DK:

			//6.5MHz => 0x1 - D/K1, D/K2, D/K3 or D/K NICAM
			if ( RET_OK != AAD_SetAndStartASD ( AAD_ASD_6_5_MHZ, 0x1, D_K_ASD_TIMEOUT_DELAY ) )
			{
				//Set a current sound system.
				AAD_RdFL(system_select);
				AAD_Wr01(system_select, syssel, 0x7); //FM-Mono/NICAM
				AAD_WrFL(system_select);

				//Start Stereo Carrier Search Control
				//Syssel change automatically when a stereo carrier is detected
				AAD_RdFL(stereo_carrier_search_ctrl);
				AAD_Wr01(stereo_carrier_search_ctrl, srchen, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, chgmod, 0x2);
				AAD_Wr01(stereo_carrier_search_ctrl, vsphase_y_rnw, 0b000);
				AAD_WrFL(stereo_carrier_search_ctrl);
				break;
			}

			//Start Stereo Carrier Search Control
			//Syssel change automatically when a stereo carrier is detected
			AAD_RdFL(stereo_carrier_search_ctrl);
			AAD_Wr01(stereo_carrier_search_ctrl, srchen, 0x2);
			AAD_Wr01(stereo_carrier_search_ctrl, chgmod, 0x2);
			AAD_Wr01(stereo_carrier_search_ctrl, vsphase_y_rnw, 0b000);
			AAD_WrFL(stereo_carrier_search_ctrl);

			//If D/K system detected in system select, return success
			AAD_RdFL(system_select);
			AAD_Rd01(system_select, syssel, ui8SysSel);
			if (( ui8SysSel == 0x4 ) || ( ui8SysSel == 0x5 ) || ( ui8SysSel == 0x6 ))
			{
				AAD_DEBUG("D/K1, 2, 3 : FM-Stereo(A2) carrier detected(SysSel = %d)\n", ui8SysSel);
				eRetVal = RET_OK;
			}
			else if ( ui8SysSel == 0x7 )
			{
				AAD_DEBUG("D/K : FM-Mono/NICAM carrier detected\n");
				eRetVal = RET_OK;
			}
			break;

		case LX_AAD_SIF_SYSTEM_L:
			//6.5MHz => 0x0 - SECAM L NICAM
			if ( RET_OK != AAD_SetAndStartASD ( AAD_ASD_6_5_MHZ, 0x0, B_G_L_ASD_TIMEOUT_DELAY ) )
			{
				//Set a current sound system.
				AAD_RdFL(system_select);
				AAD_Wr01(system_select, syssel, 0x2); //AM-Mono/NICAM
				AAD_WrFL(system_select);

				break;
			}

			//If L system detected in system select, return success
			AAD_RdFL(system_select);
			AAD_Rd01(system_select, syssel, ui8SysSel);
			if ( ui8SysSel == 0x2 )
			{
				AAD_DEBUG("L   : FM-Mono/NICAM carrier detected\n");
				eRetVal = RET_OK;
			}
			break;

		case LX_AAD_SIF_SYSTEM_MN:
			if ( (g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)	 //Check BTSC only
			   ||(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT) )
			{
				if ( RET_OK != AAD_SetAndStartASD ( AAD_ASD_4_5_MHZ, 0x0, M_N_I_ASD_TIMEOUT_DELAY ) ) //BTSC
				{
					//Set a current sound system.
					AAD_RdFL(system_select);
					AAD_Wr01(system_select, syssel, 0xA); //FM-Stereo + SAP (BTSC)
					AAD_WrFL(system_select);

					break;
				}

				//If M/N-BTSC system detected in system select, return success
				AAD_RdFL(system_select);
				AAD_Rd01(system_select, syssel, ui8SysSel);
				if ( ui8SysSel == 0xA )
				{
					AAD_DEBUG("BTSC : FM-Stereo+SAP carrier detected\n");
					eRetVal = RET_OK;
				}
			}
			else if ( g_eAadInitType == LX_AAD_INIT_ATSC_EIAJ_SELECT )	 //Check EIA-J only
			{
				if ( RET_OK != AAD_SetAndStartASD ( AAD_ASD_4_5_MHZ, 0x1, M_N_I_ASD_TIMEOUT_DELAY ) ) //EIA-J
				{
					//Set a current sound system.
					AAD_RdFL(system_select);
					AAD_Wr01(system_select, syssel, 0x9); //FM-Stereo (EIA-J)
					AAD_WrFL(system_select);

					break;
				}

				//If M/N-EIA-J system detected in system select, return success
				AAD_RdFL(system_select);
				AAD_Rd01(system_select, syssel, ui8SysSel);
				if ( ui8SysSel == 0x9 )
				{
					AAD_DEBUG("EIA-J : FM-Stereo Carrier detected\n");
					eRetVal = RET_OK;
				}
			}
			else //Check for A2
			{
				if ( RET_OK != AAD_SetAndStartASD ( AAD_ASD_4_5_MHZ, 0x2, M_N_I_ASD_TIMEOUT_DELAY ) )	//A2
				{
					//Set a current sound system.
					AAD_RdFL(system_select);
					AAD_Wr01(system_select, syssel, 0x8); //FM-Stereo (A2)
					AAD_WrFL(system_select);

					break;
				}

				//If M/N-A2 system is detected in system select, return success
				AAD_RdFL(system_select);
				AAD_Rd01(system_select, syssel, ui8SysSel);
				if ( ui8SysSel == 0x8 )	//A2
				{
					eRetVal = RET_OK;
					AAD_DEBUG("M/N : FM-Stereo(A2) Carrier detected\n");
					break;
				}
			}
			break;

		case LX_AAD_SIF_SYSTEM_UNKNOWN:
			eRetVal = RET_INVALID_PARAMS;

			//May be we can implement after complete all system test
			break;

		default:
			break;
	}

	//do AAOS function if DTV is DVB mode
	if ( g_eAadInitType == LX_AAD_INIT_DVB_SELECT || g_eAadInitType == LX_AAD_INIT_DVB_CN_SELECT )
	{
		//Enable AAOS(Automatic Audio Output Select)
		AAD_RdFL(audio_output_ctrl);
		AAD_Wr01(audio_output_ctrl, aouten, 1);
		AAD_WrFL(audio_output_ctrl);
	}

	//Sanity check for debug print
	if( eSoundSystem > LX_AAD_SIF_SYSTEM_MN )
		eSoundSystem = LX_AAD_SIF_SYSTEM_MN + 1;

	//Check carrier status for debug
	(void) AAD_ASD_CheckCarrierDetection ();

	AAD_RdFL(system_select);
	AAD_RdFL(status);
	AAD_PRINT ("AAD_DoASD : Sound System %d(%s)(syssel = %d, status = 0x%X)\n",	\
				eSoundSystem, debugString[eSoundSystem], AAD_Rd(system_select), AAD_Rd(status) );

	return eRetVal;
}

int AAD_SetAndStartASD ( AAD_ASD_CARRIER_T eASDCar, UINT8 ui8RegValue, UINT32	ui32Timeout )
{
	int		eRetVal = RET_ERROR;

	//Clears a system detect register.
	AAD_RdFL(auto_system_detect_ctrl);
	AAD_Wr01(auto_system_detect_ctrl, frfive, 0x0);
	AAD_Wr01(auto_system_detect_ctrl, sxfive, 0);

	if ( eASDCar == AAD_ASD_4_5_MHZ )
	{
		AAD_Wr01(auto_system_detect_ctrl, frfive, ui8RegValue);
	}
	else if ( eASDCar == AAD_ASD_6_5_MHZ )
	{
		//Set a PAL Chroma Carrier for D/K and L system
		AAD_Wr01(auto_system_detect_ctrl, frfive, 0x3);		//PAL Chroma Carrier
		AAD_Wr01(auto_system_detect_ctrl, sxfive, ui8RegValue);
	}
	else
	{
		AAD_ERROR ("AAD_SetAndStartASD : Failed(eASDCar = %d)!!!\n", eASDCar);
		return RET_ERROR;
	}

	//Sets a system detect register.
	AAD_WrFL(auto_system_detect_ctrl);

	/* Reset AAD_ASD_Task_waitq_flag value */
	AAD_ASD_Task_waitq_flag = 0;

	/* Enable interrupt */
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		//Un-mask status interrupt in status register for ASD Complete
		AAD_RdFL(interrupt_mask);
		AAD_Wr(interrupt_mask, 0x7F);
		AAD_WrFL(interrupt_mask);

		//Enable AAD interrupt(Status change output enable, Polarity : Active High)
		AAD_RdFL(status_pin_control);
		AAD_Wr01(status_pin_control, staten, 1);
		AAD_WrFL(status_pin_control);
	}

	//Enable ASD(Automatic System Dection)
	AAD_RdFL(system_select);
	AAD_Wr01(system_select, asden, 1);
	AAD_WrFL(system_select);

	//Wait timeout to do ASD function
	eRetVal = AAD_ASD_WaitEvent_Intrruptible(ui32Timeout);

	//Read a system detect register.
	AAD_RdFL(auto_system_detect_ctrl);
	AAD_PRINT ("AAD_SetAndStartASD : System Detect Control reg val [0x%x]\n", AAD_Rd(auto_system_detect_ctrl));

	return eRetVal;
}

/**
 * Wait for ASD to complete.
 * @see AAD_ASD_WakeUp_Interruptible
*/
int AAD_ASD_WaitEvent_Intrruptible ( UINT32	ui32Timeout )
{
	UINT8	ui8Loop;
	UINT8	ui8AsdEn;

	UINT64	start, elapsed;

	AAD_DEBUG_TMP ("Wait %d ms for ASD to complete!!!\n", ui32Timeout);

	//Get a current mili-second time
	start = jiffies;

	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		//Change note for ui32Timeout * 2 : need more time than spec.
		wait_event_interruptible_timeout(AAD_ASD_Task_wait, AAD_ASD_Task_waitq_flag != 0, msecs_to_jiffies(ui32Timeout * 2));

		//Get a elapsed nono-second time
		elapsed = jiffies_to_msecs(jiffies - start);

		//Read a status register
		AAD_RdFL(status);

		if(AAD_ASD_Task_waitq_flag)
		{
			AAD_DEBUG("ASD is completed in %d ms.(0x%X)\n", (UINT32)elapsed, AAD_Rd(status));
		}
		else
		{
			AAD_DEBUG("ASD is timeout in %d ms.(0x%X)\n", (UINT32)elapsed, AAD_Rd(status));
		}
	}
	else
	{
		for( ui8Loop = 0; ui8Loop < (ui32Timeout / AAD_SIF_TIME_DELAY_10MS); ui8Loop++ )
		{
			AAD_SIF_TIME_DELAY_MS(AAD_SIF_TIME_DELAY_10MS);

			//Read a system select registers.
			AAD_RdFL(system_select);
			AAD_Rd01(system_select, asden, ui8AsdEn);
			AAD_DEBUG ("asden = 0x%X\n", ui8AsdEn);
			if (ui8AsdEn == 0)
			{
				AAD_PRINT ("ASD is completed\n");
				break;
			}
		}

		//Get a elapsed nono-second time
		elapsed = jiffies - start;

		AAD_DEBUG ("ASD is completed in %d ms.(0x%X)\n", (UINT32)elapsed, AAD_Rd(status));
	}

	//Read a system select registers.
	AAD_RdFL(system_select);
	AAD_Rd01(system_select, asden, ui8AsdEn);

	//Check ASD status
	if (ui8AsdEn == 0)
	{
		return RET_OK;
	}
	else
	{
		//Disable ASD function
		AAD_Wr01(system_select, asden, 0);
		AAD_WrFL(system_select);

		return RET_ERROR;
	}
}

/**
 * Signal ASD is completed.
 * @see AAD_ASD_WaitEvent_Intrruptible
*/
void AAD_ASD_WakeUp_Interruptible ( void )
{
	AAD_ASD_Task_waitq_flag = 1;

	wake_up_interruptible(&AAD_ASD_Task_wait);

	return;
}

/**
 * Check a audio carrier and status detection.
 * @see AAD_DoASD
*/
int AAD_ASD_CheckCarrierDetection ( void )
{
	UINT8		ui8PrmCarDet, ui8SecCarDet, ui8MonoSt, ui8IMono, ui8BiSap;

#ifdef AAD_SIGNAL_QUALITY_DEBUG
	UINT8		value;

	UINT16		ui16Carrier1Magnitude = 0;
	UINT16	  	ui16Carrier2Magnitude = 0;
	UINT16		ui16Carrier1Quality = 0;
	UINT16	  	ui16Carrier2Quality = 0;
#endif

	//Read status register
	AAD_RdFL(status);
	AAD_Rd01(status, mono_str, ui8PrmCarDet);
	AAD_Rd01(status, seccardet,ui8SecCarDet);
	AAD_Rd01(status, mono_str, ui8MonoSt);
	AAD_Rd01(status, imono,    ui8IMono);
	AAD_Rd01(status, bisap,	   ui8BiSap);

	if ((ui8PrmCarDet == 1) && (ui8SecCarDet == 1))
	{
		AAD_DEBUG("Primary & Secondary Carrier Detected\n");
	}
	else if (ui8PrmCarDet == 1)
	{
		AAD_DEBUG("Primary Carrier Detected\n");
	}
	else if (ui8SecCarDet == 1)
	{
		AAD_DEBUG("Secondary Carrier Detected\n");
	}

	if (ui8MonoSt == 1)
	{
		AAD_DEBUG("Stereo Bit Detected\n");
	}

	if (ui8IMono == 1)
	{
		AAD_DEBUG("NICAM Bit Detected\n");
	}

	if (ui8BiSap == 1)
	{
		AAD_DEBUG("BI-SAP Bit Detected\n");
	}

#ifdef AAD_SIGNAL_QUALITY_DEBUG

	AAD_RdFL(carrier1_average_mag0);
	AAD_Rd01(carrier1_average_mag0, cr1mg, value);
	ui16Carrier1Magnitude  = value << 8;

	AAD_RdFL(carrier1_average_mag1);
	AAD_Rd01(carrier1_average_mag1, cr1mg, value);
	ui16Carrier1Magnitude += value;
	AAD_DEBUG("Carrier1 Magnitude Current 0x%4X\n", ui16Carrier1Magnitude );

	AAD_RdFL(carrier2_average_mag0);
	AAD_Rd01(carrier2_average_mag0, cr2mg, value);
	ui16Carrier2Magnitude  = value << 8;

	AAD_RdFL(carrier2_average_mag1);
	AAD_Rd01(carrier2_average_mag1, cr2mg, value);
	ui16Carrier2Magnitude += value;
	AAD_DEBUG("Carrier2 Magnitude Current 0x%4X\n", ui16Carrier2Magnitude );

	AAD_RdFL(carrier1_fm_quality0);
	AAD_Rd01(carrier1_fm_quality0, cr1ql, value);
	ui16Carrier1Quality  = value << 8;

	AAD_RdFL(carrier1_fm_quality1);
	AAD_Rd01(carrier1_fm_quality1, cr1ql, value);
	ui16Carrier1Quality += value;
	AAD_DEBUG("Carrier1 Quality Current   0x%4X\n", ui16Carrier1Quality );

	AAD_RdFL(carrier2_fm_quality0);
	AAD_Rd01(carrier2_fm_quality0, cr2ql, value);
	ui16Carrier2Quality  = value << 8;

	AAD_RdFL(carrier2_fm_quality1);
	AAD_Rd01(carrier2_fm_quality1, cr2ql, value);
	ui16Carrier2Quality += value;
	AAD_DEBUG("Carrier2 Quality Current   0x%4X\n", ui16Carrier2Quality );

	AAD_RdFL(stereo_id_magnitude);
	AAD_RdFL(bilingual_id_magnitude);
	AAD_DEBUG("Stereo ID Mag. Current     0x%4X\n", AAD_Rd(stereo_id_magnitude));
	AAD_DEBUG("Biling ID Mag. Current     0x%4X\n", AAD_Rd(bilingual_id_magnitude));

	//Print global variables
	AAD_DEBUG("carrier2Magitude    = 0x%4X\n", g_stThresholdLevel.carrier2Magitude);
	AAD_DEBUG("carrier2QualityHigh = 0x%4X, Low = 0x%4X\n",	\
			   g_stThresholdLevel.carrier2QualityHigh, g_stThresholdLevel.carrier2QualityLow);
	AAD_DEBUG("stereoIDMagHigh     = 0x%4X, Low = 0x%4X\n",	\
			   g_stThresholdLevel.stereoIDMagHigh, g_stThresholdLevel.stereoIDMagLow);

	//Carrier 1 Quality AAOS Threshold High / Low Register
	AAD_RdFL(carrier1_quality_aaos_thd_hi);
	AAD_RdFL(carrier1_quality_aaos_thd_lo);
	AAD_DEBUG("Carrier1 Quality Threshold 0x%4X(0x%4X)\n",	\
			   AAD_Rd(carrier1_quality_aaos_thd_hi) << 3, AAD_Rd(carrier1_quality_aaos_thd_lo) << 3 );

	//Carrier 2 Quality AAOS Threshold High / Low Register
	AAD_RdFL(carrier2_quality_aaos_thd_hi);
	AAD_RdFL(carrier2_quality_aaos_thd_lo);
	AAD_DEBUG("Carrier2 Quality Threshold 0x%4X(0x%4X)\n",	\
			   AAD_Rd(carrier2_quality_aaos_thd_hi) << 3, AAD_Rd(carrier2_quality_aaos_thd_lo) << 3 );

	//ID Magnitude Threshold High / Low Register
	AAD_RdFL(id_magnitude_thd_hi);
	AAD_RdFL(id_magnitude_thd_lo);
	AAD_DEBUG("St/Bil Magnitude Threshold 0x%4X(0x%4X)\n",	\
			   AAD_Rd(id_magnitude_thd_hi), AAD_Rd(id_magnitude_thd_lo) );
#endif

	return RET_OK;
}

/**
 * Get dectected Sound System.
 * @see AAD_SIF_GetBandDetect
*/
LX_AAD_SIF_SOUNDSYSTEM_T AAD_GetSoundSystem (void)
{
	UINT8	ui8SysSel;

	LX_AAD_SIF_SOUNDSYSTEM_T	eTVSystem	= LX_AAD_SIF_SYSTEM_UNKNOWN;

	//ASD completed, use system select register to find detected sound system
	//Check a current sound sysytem is detected.
	AAD_RdFL(system_select);
	AAD_Rd01(system_select, syssel, ui8SysSel);

	switch (ui8SysSel)
	{
		case 0x0:
		case 0x1:
			eTVSystem = LX_AAD_SIF_SYSTEM_BG;
			break;

		case 0x2:
			eTVSystem = LX_AAD_SIF_SYSTEM_L;
			break;

		case 0x3:
			eTVSystem = LX_AAD_SIF_SYSTEM_I;
			break;

		case 0x4:
		case 0x5:
		case 0x6:
		case 0x7:
			eTVSystem = LX_AAD_SIF_SYSTEM_DK;
			break;

		case 0x8:
		case 0x9:
			eTVSystem = LX_AAD_SIF_SYSTEM_MN;
			break;

		case 0xA:
			if ( (g_eAadInitType == LX_AAD_INIT_ATSC_BTSC_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_ATSC_BR_SELECT)
			   ||(g_eAadInitType == LX_AAD_INIT_DVB_TW_SELECT) )
			{
				eTVSystem = LX_AAD_SIF_SYSTEM_MN;
			}
			else
			{
				eTVSystem = LX_AAD_SIF_SYSTEM_UNKNOWN; //FM-Radio
			}
			break;

		case 0xB:
		case 0xC:
		case 0xD:
		case 0xE:
		case 0xF:
		default:
			eTVSystem = LX_AAD_SIF_SYSTEM_UNKNOWN; //FM-Radio
			break;
	}

	return eTVSystem;
}

/**
 * Routine for monitor task to check mono / stereo detection
 * @see
*/
int AAD_Monitor_Task( void *i_pvParam )
{
	UINT8	ui8CurStIdMag = 0;
	UINT8	ui8CurBiIdMag = 0;
	UINT32	ui32CurStatus = 0;

	UINT8	ui8PrevStIdMag = 0xFF;
	UINT8	ui8PrevBiIdMag = 0xFF;
	UINT8	ui8PrevMonoSt = 0xFF, ui8PrevBiSap = 0xFF;

	UINT32	ui32ContinuousCountStIdmag = 0;
	UINT32	ui32ContinuousCountBiIdmag = 0;
	UINT32	ui32NonContinuousCountStIdmag = 0;
	UINT32	ui32NonContinuousCountBiIdmag = 0;

	UINT32	ui32ContinuousCountStereo = 0;
	UINT32	ui32ContinuousCountBilingual = 0;

#if 0
	UINT32	ui32Count = 0;
#endif

	UINT64	ui64TaskTimeout = AAD_MONOTOR_TASK_NO_TIMEOUT;

	AAD_PRINT( "AAD> Task is created!\n" );

	do
	{
		/* Check stop condition when device is closed. */
		if( kthread_should_stop())
		{
			AAD_PRINT( "AAD> Task - exit!\n" );
			break;
		}

		/* Reset a wait queue monitor variable. */
		g_suiAadWaitQForMonitor = LX_AAD_TASK_NONE_MSG;

		wait_event_interruptible_timeout( Aad_Monitor_WaitQ,	\
										  g_suiAadWaitQForMonitor != LX_AAD_TASK_NONE_MSG,	\
										  msecs_to_jiffies(ui64TaskTimeout) );		//50 ms

		//Set a Monitor task : Start and Stop timeout
		if(g_suiAadWaitQForMonitor == LX_AAD_TASK_START)
		{
			ui64TaskTimeout = AAD_MONOTOR_TASK_TIMEOUT;
		}
		else if(g_suiAadWaitQForMonitor == LX_AAD_TASK_STOP)
		{
			ui64TaskTimeout = AAD_MONOTOR_TASK_NO_TIMEOUT;
		}

		AAD_PRINT("AAD_Monitor_Task(msg = %d, timeout = 0x%llX)\n", g_suiAadWaitQForMonitor, ui64TaskTimeout);

		//Check a Mono/Stereo Status
		AAD_RdFL(status);
		ui32CurStatus = AAD_Rd(status);

		//Check a current and previous Stereo count
		if(ui32CurStatus == 0x13 && ui8PrevMonoSt == 0x13)
		{
			ui32ContinuousCountStereo++;
		}
		else
		{
			ui32ContinuousCountStereo = 0;
		}

		//Check a current and previous Stereo count
		if(ui32CurStatus == 0x43 && ui8PrevBiSap == 0x43)
		{
			ui32ContinuousCountBilingual++;
		}
		else
		{
			ui32ContinuousCountBilingual = 0;
		}

		//Update a current ST/BI Status
		ui8PrevMonoSt = (UINT8)ui32CurStatus;
		ui8PrevBiSap  = (UINT8)ui32CurStatus;

		//Check a Mono/Stereo ID value
		AAD_RdFL(stereo_id_magnitude);
		AAD_Rd01(stereo_id_magnitude, stmag, ui8CurStIdMag);

		//Check a current and previous ST ID Mag count
		if(ui8CurStIdMag == 0 && ui8PrevStIdMag == 0)
		{
			ui32ContinuousCountStIdmag++;
			ui32NonContinuousCountStIdmag = 0;
		}
		else if(ui32ContinuousCountStIdmag > 0)
		{
			ui32NonContinuousCountStIdmag++;
			ui32ContinuousCountStIdmag = 0;
		}

		//Check a Bilingual ID value
		AAD_RdFL(bilingual_id_magnitude);
		AAD_Rd01(bilingual_id_magnitude, bimag, ui8CurBiIdMag);

		//Check a current and previous BI ID Mag count
		if(ui8CurBiIdMag == 0 && ui8PrevBiIdMag == 0)
		{
			ui32ContinuousCountBiIdmag++;
			ui32NonContinuousCountBiIdmag = 0;
		}
		else if(ui32ContinuousCountBiIdmag > 0)
		{
			ui32NonContinuousCountBiIdmag++;
			ui32ContinuousCountBiIdmag = 0;
		}

		//Update a current ST/BI ID Mag value
		ui8PrevStIdMag = ui8CurStIdMag;
		ui8PrevBiIdMag = ui8CurBiIdMag;

		//Set a ST sound status in continus ST ID zero value
		if( (ui32ContinuousCountStIdmag   > ID_ST_MAG_ZERO_COUNT_ATSC_A2)
		  &&(ui32ContinuousCountBilingual < ID_ST_MAG_NONZERO_COUNT_ATSC_A2)
		  &&(ui32ContinuousCountStIdmag   >= ui32ContinuousCountBiIdmag) )
		{
			g_MonitorAudioMode = LX_ATV_GET_NTSC_A2_STEREO;
			ui32ContinuousCountBiIdmag = 0;

			AAD_DEBUG( "AAD> Cont.0 Mode = 0x%X, CountSt = %d, CountBi = %d\n",	\
						g_MonitorAudioMode, ui32ContinuousCountStIdmag, ui32ContinuousCountBiIdmag );
		}
		//Set a BI sound status in continus BI ID zero value
		else if( (ui32ContinuousCountBiIdmag > ID_BI_MAG_ZERO_COUNT_ATSC_A2)
		  	   &&(ui32ContinuousCountStereo  < ID_ST_MAG_NONZERO_COUNT_ATSC_A2)
			   &&(ui32ContinuousCountBiIdmag >= ui32ContinuousCountStIdmag) )
		{
			g_MonitorAudioMode = LX_ATV_GET_NTSC_A2_SAP;
			ui32ContinuousCountBilingual = 0;

			AAD_DEBUG( "AAD> Cont.1 Mode = 0x%X, CountSt = %d, CountBi = %d\n",	\
						g_MonitorAudioMode, ui32ContinuousCountStIdmag, ui32ContinuousCountBiIdmag );
		}


		//Set a Normal sound status in non-continus ST ID zero value when previous status is ST
		if( ((ui32NonContinuousCountStIdmag > ID_ST_MAG_NONZERO_COUNT_ATSC_A2)
		   ||(ui32CurStatus == 0x43) )
		//||(ui32ContinuousCountBilingual  > STATUS_BI_NZ_EXIT_COUNT_ATSC_A2) )
		
		  &&(g_MonitorAudioMode == LX_ATV_GET_NTSC_A2_STEREO) )
		{
			g_MonitorAudioMode = LX_ATV_GET_NTSC_A2_UNKNOWN;
			ui32ContinuousCountStIdmag = 0;
			ui32ContinuousCountStereo  = 0;

			AAD_DEBUG( "AAD> Non-Cont.0-0 Mode = 0x%X, CountSt = %d, CountBi = %d\n",	\
						g_MonitorAudioMode, ui32NonContinuousCountStIdmag, ui32NonContinuousCountBiIdmag );
		}

		//Set a Normal sound status in non-continus ST ID zero value when previous status is SAP
		if( ((ui32NonContinuousCountStIdmag > ID_ST_MAG_NONZERO_COUNT_ATSC_A2)
		  ||(ui32CurStatus == 0x43) )
		//||(ui32ContinuousCountBilingual  > STATUS_BI_NZ_EXIT_COUNT_ATSC_A2) )
		  &&(g_MonitorAudioMode == LX_ATV_GET_NTSC_A2_SAP) )
		{
			g_MonitorAudioMode = LX_ATV_GET_NTSC_A2_UNKNOWN;
			ui32ContinuousCountStIdmag = 0;
			ui32ContinuousCountStereo  = 0;

			AAD_DEBUG( "AAD> Non-Cont.0-1 Mode = 0x%X, CountSt = %d, CountBi = %d\n",	\
						g_MonitorAudioMode, ui32NonContinuousCountStIdmag, ui32NonContinuousCountBiIdmag );
		}

		//Set a Normal sound status in continus BI ID zero value when previous status is ST
		if( ((ui32NonContinuousCountBiIdmag > ID_BI_MAG_NONZERO_COUNT_ATSC_A2)
		   ||(ui32ContinuousCountStereo    > ID_BI_MAG_NONZERO_COUNT_ATSC_A2) )
		  &&(g_MonitorAudioMode == LX_ATV_GET_NTSC_A2_STEREO) )
		{
			g_MonitorAudioMode = LX_ATV_GET_NTSC_A2_UNKNOWN;
			ui32ContinuousCountBiIdmag   = 0;
			ui32ContinuousCountBilingual = 0;

			AAD_DEBUG( "AAD> Non-Cont.1-0 Mode = 0x%X, CountSt = %d, CountBi = %d\n",	\
						g_MonitorAudioMode, ui32NonContinuousCountStIdmag, ui32NonContinuousCountBiIdmag );
		}

		//Set a Normal sound status in continus BI ID zero value when previous status is SAP
		if( ((ui32NonContinuousCountBiIdmag > ID_BI_MAG_NONZERO_COUNT_ATSC_A2)
		   ||(ui32ContinuousCountStereo    > ID_BI_MAG_NONZERO_COUNT_ATSC_A2) )
		  &&(g_MonitorAudioMode == LX_ATV_GET_NTSC_A2_SAP) )
		{
			g_MonitorAudioMode = LX_ATV_GET_NTSC_A2_UNKNOWN;
			ui32ContinuousCountBiIdmag   = 0;
			ui32ContinuousCountBilingual = 0;

			AAD_DEBUG( "AAD> Non-Cont.1-1 Mode = 0x%X, CountSt = %d, CountBi = %d\n",	\
						g_MonitorAudioMode, ui32NonContinuousCountStIdmag, ui32NonContinuousCountBiIdmag );
		}

#if 0
		//For debug print : not interrupt value
		if((ui32Count++ % AAD_MONOTOR_TASK_PRINT_TIME) == 0)
		{
			AAD_DEBUG( "AAD> Mode = 0x%X, ST = 0x%X, BI = 0x%X, CountSt = %d(%d), CountBi = %d(%d)\n",	\
						g_MonitorAudioMode, ui32ContinuousCountStereo, ui32ContinuousCountBilingual,	\
						ui32ContinuousCountStIdmag, ui32NonContinuousCountStIdmag, ui32ContinuousCountBiIdmag, ui32NonContinuousCountBiIdmag );
		}
#endif
	} while ( TRUE );

	return 0;
}

/** @} */

