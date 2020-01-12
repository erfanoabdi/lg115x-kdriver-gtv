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
 * adec functions through IPC.
 * Communicate adec firmware through IPC instead registers.
 *
 * author     wonchang.shin (wonchang.shinu@lge.com)
 * version    1.0
 * date       2010.06.19
 *
 * @addtogroup lg1152_adec
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <linux/delay.h>

#include "adec_kapi.h"

#include "adec_ipc_func.h"

#include "adec_ipc/adec_ipc_cmd.h"
#include "adec_ipc/adec_ipc_req.h"
#include "adec_ipc/adec_ipc_list.h"

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
//#define ENABLE_IPC_DEBUG
#define IPC_ADDITIONAL_DELAY 	5

//semaphore
OS_SEM_T	g_ADEC_IPC_Sema;

#define ADEC_IPC_LOCK_INIT()	OS_InitMutex(&g_ADEC_IPC_Sema, OS_SEM_ATTR_DEFAULT)

#if 1
#define ADEC_IPC_LOCK()			msleep_interruptible(1); \
								OS_LockMutex(&g_ADEC_IPC_Sema)
#else
#define ADEC_IPC_LOCK()			OS_LockMutex(&g_ADEC_IPC_Sema)
#endif
#define ADEC_IPC_UNLOCK()		OS_UnlockMutex(&g_ADEC_IPC_Sema)


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
//define for pause and resume jiffies
UINT64 PauseJiffies  = 0;
UINT64 ResumeJiffies = 0;


//define for adec module parameters
KDRV_ADEC_DEC_PARAM_T		gKdrvAdecDec0Param;
KDRV_ADEC_DEC_PARAM_T		gKdrvAdecDec1Param;

KDRV_ADEC_SE_PARAM_T		gKdrvAdecSeParam;

KDRV_ADEC_SPK_PARAM_T		gKdrvAdecSpkParam;
KDRV_ADEC_SPDIF_PARAM_T		gKdrvAdecSpdifParam;
KDRV_ADEC_HP_PARAM_T		gKdrvAdecHpParam;
KDRV_ADEC_SCART_PARAM_T		gKdrvAdecScartParam;
KDRV_ADEC_LINEOUT_PARAM_T	gKdrvAdecLineoutParam;

KDRV_ADEC_LIPSYNC_PARAM_T 	gKdrvAdecLipsync0Param;
KDRV_ADEC_LIPSYNC_PARAM_T 	gKdrvAdecLipsync1Param;
KDRV_ADEC_LIPSYNC_PARAM_T 	gKdrvAdecLipsync2Param;
KDRV_ADEC_LIPSYNC_PARAM_T 	gKdrvAdecLipsync3Param;
KDRV_ADEC_LIPSYNC_PARAM_T 	gKdrvAdecLipsync4Param;
KDRV_ADEC_LIPSYNC_PARAM_T 	gKdrvAdecLipsync5Param;
KDRV_ADEC_LIPSYNC_PARAM_T 	gKdrvAdecLipsync6Param;

KDRV_ADEC_MIX_START_PARAM_T gKdrvAdecMP00Param;
KDRV_ADEC_MIX_START_PARAM_T gKdrvAdecMP01Param;
KDRV_ADEC_MIX_START_PARAM_T gKdrvAdecMP02Param;
KDRV_ADEC_MIX_START_PARAM_T gKdrvAdecMP03Param;
KDRV_ADEC_MIX_START_PARAM_T gKdrvAdecMP04Param;
KDRV_ADEC_MIX_START_PARAM_T gKdrvAdecMP05Param;
KDRV_ADEC_MIX_START_PARAM_T gKdrvAdecMP06Param;

KDRV_ADEC_ENC_PARAM_T 		gKdrvAdecEncParam;

ADEC_CMD_SYNC_BASE_T 		gAdecSyncBase;
ADEC_SET_TREBLE_T			gAdecSetTreble;
ADEC_SET_BASS_T				gAdecSetBass;

ADEC_SET_SPDIF_SCMS_T		gAdecSetSpdifScms;
ADEC_SET_SPDIF_DTO_T		gAdecSetSpdifDto;
ADEC_SET_SPDIF_IEC_T		gAdecSetSpdifIec;

ADEC_SET_VOLUME_T			gAdecSetVolume;

ADEC_SET_PCM_INTERRUPT_T	gAdecSetPcmRate;
ADEC_MIX_EXT_PARAM_T		gAdecMixExtParam;


/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern LX_ADEC_INIT_TYPE_T	g_eAdecInitType;


SINT32 ADEC_IPC_InitFunc(void)
{
	//Initialize ADEC IPC semaphore
	ADEC_IPC_LOCK_INIT();

	//Initialized ADEC IPC global variables
	// Lipsync0 Paramter(DEC0)
	gKdrvAdecLipsync0Param.bOnOff = 0x0;
	gKdrvAdecLipsync0Param.ubound = 0x20;
	gKdrvAdecLipsync0Param.lbound = 0x20;
	gKdrvAdecLipsync0Param.offset = 0x100;
	gKdrvAdecLipsync0Param.freerunubound = 0x1388;	// 5 sec
	gKdrvAdecLipsync0Param.freerunlbound = 0x1388;	// 5 sec

	// Lipsync1 Paramter(DEC1)
	gKdrvAdecLipsync1Param.bOnOff = 0x0;
	gKdrvAdecLipsync1Param.ubound = 0x20;
	gKdrvAdecLipsync1Param.lbound = 0x20;
	gKdrvAdecLipsync1Param.offset = 0x100;
	gKdrvAdecLipsync1Param.freerunubound = 0x1388;	// 5 sec
	gKdrvAdecLipsync1Param.freerunlbound = 0x1388;	// 5 sec

	// Lipsync2 Paramter(MP02)
	gKdrvAdecLipsync2Param.bOnOff = 0x0;
	gKdrvAdecLipsync2Param.ubound = 0x64;
	gKdrvAdecLipsync2Param.lbound = 0x64;
	gKdrvAdecLipsync2Param.offset = 0x384;
	gKdrvAdecLipsync2Param.freerunubound = 0x1388;	// 5 sec
	gKdrvAdecLipsync2Param.freerunlbound = 0x1D4C0;	// 120 sec

	// Lipsync3 Paramter(MP03)
	gKdrvAdecLipsync3Param.bOnOff = 0x0;
	gKdrvAdecLipsync3Param.ubound = 0x64;
	gKdrvAdecLipsync3Param.lbound = 0x64;
	gKdrvAdecLipsync3Param.offset = 0x384;
	gKdrvAdecLipsync3Param.freerunubound = 0x1388;	// 5 sec
	gKdrvAdecLipsync3Param.freerunlbound = 0x1D4C0;	// 120 sec

	// Lipsync4 Paramter(MP04)
	gKdrvAdecLipsync4Param.bOnOff = 0x0;
	gKdrvAdecLipsync4Param.ubound = 0x64;
	gKdrvAdecLipsync4Param.lbound = 0x64;
	gKdrvAdecLipsync4Param.offset = 0x384;
	gKdrvAdecLipsync4Param.freerunubound = 0x1388;	// 5 sec
	gKdrvAdecLipsync4Param.freerunlbound = 0x1D4C0;	// 120 sec

	// Lipsync5 Paramter(MP05)
	gKdrvAdecLipsync5Param.bOnOff = 0x0;
	gKdrvAdecLipsync5Param.ubound = 0x64;
	gKdrvAdecLipsync5Param.lbound = 0x64;
	gKdrvAdecLipsync5Param.offset = 0x384;
	gKdrvAdecLipsync5Param.freerunubound = 0x1388;	// 5 sec
	gKdrvAdecLipsync5Param.freerunlbound = 0x1D4C0;	// 120 sec

	// Lipsync6 Paramter(MP06)
	gKdrvAdecLipsync6Param.bOnOff = 0x0;
	gKdrvAdecLipsync6Param.ubound = 0x64;
	gKdrvAdecLipsync6Param.lbound = 0x64;
	gKdrvAdecLipsync6Param.offset = 0x384;
	gKdrvAdecLipsync6Param.freerunubound = 0x1388;	// 5 sec
	gKdrvAdecLipsync6Param.freerunlbound = 0x1D4C0;	// 120 sec

	// MP00 Paramter
	gKdrvAdecMP00Param.ui32PortNum = 1;
	gKdrvAdecMP00Param.ui32NumOfCH = 2;
	gKdrvAdecMP00Param.ui32BitPerSample = 32;
	gKdrvAdecMP00Param.ui32SampleRate= 48;
	gKdrvAdecMP00Param.ui32GainL = 1024;
	gKdrvAdecMP00Param.ui32GainR = 1024;

	// MP01 Paramter
	gKdrvAdecMP01Param.ui32PortNum = 1;
	gKdrvAdecMP01Param.ui32NumOfCH = 2;
	gKdrvAdecMP01Param.ui32BitPerSample = 32;
	gKdrvAdecMP01Param.ui32SampleRate= 48;
	gKdrvAdecMP01Param.ui32GainL = 1024;
	gKdrvAdecMP01Param.ui32GainR = 1024;

	// MP02 Paramter
	gKdrvAdecMP02Param.ui32PortNum = 1;
	gKdrvAdecMP02Param.ui32NumOfCH = 2;
	gKdrvAdecMP02Param.ui32BitPerSample = 16;
	gKdrvAdecMP02Param.ui32SampleRate = 44;
	gKdrvAdecMP02Param.ui32GainL = 1024;
	gKdrvAdecMP02Param.ui32GainR = 1024;

	// MP03 Paramter
	gKdrvAdecMP03Param.ui32PortNum = 1;
	gKdrvAdecMP03Param.ui32NumOfCH = 2;
	gKdrvAdecMP03Param.ui32BitPerSample = 16;
	gKdrvAdecMP03Param.ui32SampleRate = 44;
	gKdrvAdecMP03Param.ui32GainL = 1024;
	gKdrvAdecMP03Param.ui32GainR = 1024;

	// MP04 Paramter
	gKdrvAdecMP04Param.ui32PortNum = 1;
	gKdrvAdecMP04Param.ui32NumOfCH = 2;
	gKdrvAdecMP04Param.ui32BitPerSample = 16;
	gKdrvAdecMP04Param.ui32SampleRate = 44;
	gKdrvAdecMP04Param.ui32GainL = 1024;
	gKdrvAdecMP04Param.ui32GainR = 1024;

	// MP05 Paramter
	gKdrvAdecMP05Param.ui32PortNum = 1;
	gKdrvAdecMP05Param.ui32NumOfCH = 2;
	gKdrvAdecMP05Param.ui32BitPerSample = 16;
	gKdrvAdecMP05Param.ui32SampleRate = 44;
	gKdrvAdecMP05Param.ui32GainL = 1024;
	gKdrvAdecMP05Param.ui32GainR = 1024;

	// MP06 Paramter
	gKdrvAdecMP06Param.ui32PortNum = 1;
	gKdrvAdecMP06Param.ui32NumOfCH = 2;
	gKdrvAdecMP06Param.ui32BitPerSample = 16;
	gKdrvAdecMP06Param.ui32SampleRate = 44;
	gKdrvAdecMP06Param.ui32GainL = 1024;
	gKdrvAdecMP06Param.ui32GainR = 1024;

	// SPK Paramter
	gKdrvAdecSpkParam.outmode = 0;
	gKdrvAdecSpkParam.mute = 0;
	gKdrvAdecSpkParam.volctrl = 0;
	gKdrvAdecSpkParam.volume = 0x800000;
	gKdrvAdecSpkParam.delayMS = 0;
	gKdrvAdecSpkParam.balance = 0;

	// SPDIF Paramter
	gKdrvAdecSpdifParam.type = 0;
	gKdrvAdecSpdifParam.outmode = 0;
	gKdrvAdecSpdifParam.mute = 0;
	gKdrvAdecSpdifParam.volctrl = 0;
	gKdrvAdecSpdifParam.volume = 0x800000;
	gKdrvAdecSpdifParam.delayMS = 0;

	// SPDIF SCMS Paramter
	gAdecSetSpdifScms.ui32Scms = 0;

	// SPDIF DTO Paramter
	gAdecSetSpdifDto.ui32SampleFreq = KDRV_ADEC_SAMPLING_FREQ_NONE;
	gAdecSetSpdifDto.ui32DtoRate = 0;	// UNKNOWN
	gAdecSetSpdifDto.ui32ForceCntClear = 0;

	// SPDIF IEC Paramter
	gAdecSetSpdifIec.ui32SetIecSrc = KDRV_ADEC_DEC_0;
	gAdecSetSpdifIec.ui32LipsyncOnOff = 0x1;	//Lipsunc ON

	// HP Paramter
	gKdrvAdecSpkParam.outmode = 0;
	gKdrvAdecSpkParam.mute = 0;
	gKdrvAdecSpkParam.volctrl = 0;
	gKdrvAdecSpkParam.volume = 0x800000;
	gKdrvAdecSpkParam.delayMS = 0;

	// SCART Paramter
	gKdrvAdecScartParam.outmode = 0;
	gKdrvAdecScartParam.mute = 0;
	gKdrvAdecScartParam.volctrl = 0;
	gKdrvAdecScartParam.volume = 0x800000;
	gKdrvAdecScartParam.delayMS = 0;

	// PCM Interrupt Rate Paramter
	gAdecSetPcmRate.ui32InFs = KDRV_ADEC_SAMPLING_FREQ_NONE;
	gAdecSetPcmRate.ui32PCMclkRate = 0;	// UNKNOWN
	gAdecSetPcmRate.ui32ForceCntClear = 0;

	// Encoder Paramter
	gKdrvAdecEncParam.adecInputMode    = KDRV_ADEC_INPUT_DEC0;
	gKdrvAdecEncParam.aencCodecType	   = KDRV_ADEC_CODEC_AAC_ENC;
	gKdrvAdecEncParam.aencOutMode	   = 0;
	gKdrvAdecEncParam.channelNumber	   = 2;
	gKdrvAdecEncParam.bitPerSample	   = 16;
	gKdrvAdecEncParam.samplingFrequency= 480;
	gKdrvAdecEncParam.bitRate		   = 224000;
//	gKdrvAdecEncParam.bitRate = stEncParam.duration;

	//Mix Ext Param
	gAdecMixExtParam.mixPort			= 0;
	gAdecMixExtParam.waitLength			= 1024;
	gAdecMixExtParam.initialMuteLength	= 0;
	gAdecMixExtParam.autoFadeOutEnable	= 1;
	gAdecMixExtParam.autoFadeInEnable	= 1;
	gAdecMixExtParam.fadeLength			= 9;
	gAdecMixExtParam.initDelay			= 0;

	return RET_OK;
}

SINT32 ADEC_IPC_Connect(KDRV_ADEC_MOD_T module, KDRV_ADEC_MOD_T connector)
{
	ADEC_CMD_CONNECT_MODULE_T param;

	// 2. make CMD
	param.module = module;
	param.connector = connector;

	// 3. send CMD
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_CONNECT, sizeof(ADEC_CMD_CONNECT_MODULE_T), &param);
	ADEC_IPC_UNLOCK();
#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_Connect : m(%d), c(%d)\n",module, connector);
#endif

	return 0;
}
SINT32 ADEC_IPC_Disconnect(KDRV_ADEC_MOD_T module)
{
	ADEC_CMD_DISCONNECT_MODULE_T param;

	// 2. make CMD
	param.module = module;

	// 3. send CMD
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_DISCONNECT, sizeof(ADEC_CMD_DISCONNECT_MODULE_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_Disconnect : m(%d)\n",module);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetConfig(KDRV_ADEC_MOD_T module, void *param)
{
	switch(module)
	{
		case KDRV_ADEC_DEC_0 :
		case KDRV_ADEC_DEC_1 :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_DEC_PARAM_T	*decParam;

			decParam = (KDRV_ADEC_DEC_PARAM_T *)param;

			setConfig.module = module;

			memcpy(&(setConfig.param[0]), decParam, sizeof(KDRV_ADEC_DEC_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_DEC_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();

			if(module == KDRV_ADEC_DEC_0)
				memcpy(&gKdrvAdecDec0Param, decParam, sizeof(KDRV_ADEC_DEC_PARAM_T));
			else
				memcpy(&gKdrvAdecDec1Param, decParam, sizeof(KDRV_ADEC_DEC_PARAM_T));
		}
		break;

		case KDRV_ADEC_SE :
		{
			KDRV_ADEC_SE_PARAM_T seParam;

			memcpy(&seParam, (KDRV_ADEC_SE_PARAM_T *)param, sizeof(KDRV_ADEC_SE_PARAM_T));

			if(seParam.seFnParam.ui32FnMode == LX_ADEC_SE_FN_MODE_SET)
			{
				ADEC_IPC_LOCK();
				ADEC_IPC_CMD_Send(ADEC_IPC_SET_SE_MODE, sizeof(KDRV_ADEC_SE_SET_MODE_T), &(seParam.seSetMode));
				ADEC_IPC_UNLOCK();
				memcpy(&gKdrvAdecSeParam.seSetMode, &seParam.seSetMode, sizeof(KDRV_ADEC_SE_SET_MODE_T));
			}
			else
			{
				ADEC_IPC_LOCK();
				ADEC_IPC_CMD_Send(ADEC_IPC_SET_SE_FN, sizeof(KDRV_ADEC_SE_FN_PARAM_T), &(seParam.seFnParam));
				ADEC_IPC_UNLOCK();
				memcpy(&gKdrvAdecSeParam.seFnParam, &seParam.seFnParam, sizeof(KDRV_ADEC_SE_FN_PARAM_T));
			}

		}
		break;

		case KDRV_ADEC_SPK :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_SPK_PARAM_T	*spkParam;

			spkParam = (KDRV_ADEC_SPK_PARAM_T *)param;

			setConfig.module = module;
			memcpy(&(setConfig.param[0]), spkParam, sizeof(KDRV_ADEC_SPK_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_SPK_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();


			memcpy(&gKdrvAdecSpkParam, spkParam, sizeof(KDRV_ADEC_SPK_PARAM_T));
		}
		break;

		case KDRV_ADEC_SPDIF :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_SPDIF_PARAM_T	*spdifParam;
			ADEC_SET_SPDIF_OUT_TYPE_T spdifOutType;


			spdifParam = (KDRV_ADEC_SPDIF_PARAM_T *)param;

			setConfig.module = module;
			memcpy(&(setConfig.param[0]), spdifParam, sizeof(KDRV_ADEC_SPDIF_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_SPDIF_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();

			spdifOutType.ui32SpdifMode = spdifParam->type;

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_SET_SPDIF_OUT_TYPE, sizeof(ADEC_SET_SPDIF_OUT_TYPE_T), &spdifOutType);
			ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
			ADEC_ERROR("ADEC_IPC_SetConfig : m(%d), t(%d)\n", module, spdifParam->type);
#endif

			memcpy(&gKdrvAdecSpdifParam, spdifParam, sizeof(KDRV_ADEC_SPDIF_PARAM_T));
		}
		break;

		case KDRV_ADEC_HP :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_HP_PARAM_T	*hpParam;

			hpParam = (KDRV_ADEC_HP_PARAM_T *)param;

			setConfig.module = module;
			memcpy(&(setConfig.param[0]), hpParam, sizeof(KDRV_ADEC_HP_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_HP_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();

			memcpy(&gKdrvAdecHpParam, hpParam, sizeof(KDRV_ADEC_HP_PARAM_T));
		}
		break;

		case KDRV_ADEC_SCART :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_SCART_PARAM_T	*scartParam;

			scartParam = (KDRV_ADEC_SCART_PARAM_T *)param;

			setConfig.module = module;
			memcpy(&(setConfig.param[0]), scartParam, sizeof(KDRV_ADEC_SCART_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_SCART_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();

			memcpy(&gKdrvAdecScartParam, scartParam, sizeof(KDRV_ADEC_SCART_PARAM_T));
		}
		break;

		case KDRV_ADEC_LINEOUT :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_LINEOUT_PARAM_T	*lineoutParam;

			lineoutParam = (KDRV_ADEC_LINEOUT_PARAM_T *)param;

			setConfig.module = module;
			memcpy(&(setConfig.param[0]), lineoutParam, sizeof(KDRV_ADEC_LINEOUT_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_LINEOUT_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();

			memcpy(&gKdrvAdecLineoutParam, lineoutParam, sizeof(KDRV_ADEC_LINEOUT_PARAM_T));
		}
		break;

		case KDRV_ADEC_MP_00 :
		case KDRV_ADEC_MP_01 :
		case KDRV_ADEC_MP_02 :
		case KDRV_ADEC_MP_03 :
		case KDRV_ADEC_MP_04 :
		case KDRV_ADEC_MP_05 :
		case KDRV_ADEC_MP_06 :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_MIX_START_PARAM_T	*mixStartParam;

			mixStartParam = (KDRV_ADEC_MIX_START_PARAM_T *)param;

			setConfig.module = module;
			memcpy(&(setConfig.param[0]), mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_MIX_START_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();

			if(module == KDRV_ADEC_MP_00)
				memcpy(&gKdrvAdecMP00Param, mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
			else if(module == KDRV_ADEC_MP_01)
				memcpy(&gKdrvAdecMP01Param, mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
			else if(module == KDRV_ADEC_MP_02)
				memcpy(&gKdrvAdecMP02Param, mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
			else if(module == KDRV_ADEC_MP_03)
				memcpy(&gKdrvAdecMP03Param, mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
			else if(module == KDRV_ADEC_MP_04)
				memcpy(&gKdrvAdecMP04Param, mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
			else if(module == KDRV_ADEC_MP_05)
				memcpy(&gKdrvAdecMP05Param, mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
			else if(module == KDRV_ADEC_MP_06)
				memcpy(&gKdrvAdecMP06Param, mixStartParam, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
			else
				ADEC_ERROR("ADEC_IPC_SetConfig : m(%d)\n", module);
		}
		break;

		case KDRV_ADEC_ENC :
		{
			ADEC_CMD_SETCONFIG_MODULE_T setConfig;
			KDRV_ADEC_ENC_PARAM_T	*encParam;

			encParam = (KDRV_ADEC_ENC_PARAM_T *)param;

			setConfig.module = module;

			memcpy(&(setConfig.param[0]), encParam, sizeof(KDRV_ADEC_ENC_PARAM_T));

			ADEC_IPC_LOCK();
			ADEC_IPC_CMD_Send(ADEC_IPC_CMD_SETCONFIG, sizeof(KDRV_ADEC_ENC_PARAM_T)+sizeof(UINT32), &setConfig);
			ADEC_IPC_UNLOCK();

			memcpy(&gKdrvAdecEncParam, encParam, sizeof(KDRV_ADEC_ENC_PARAM_T));
		}
		break;

		default :
		break;
	}

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetConfig : %d\n", module);
#endif

	return 0;
}

SINT32 ADEC_IPC_GetConfig(KDRV_ADEC_MOD_T module, void *param)
{
	switch(module)
	{
		case KDRV_ADEC_DEC_0 :
			memcpy(param, &gKdrvAdecDec0Param, sizeof(KDRV_ADEC_DEC_PARAM_T));
		break;

		case KDRV_ADEC_DEC_1 :
			memcpy(param, &gKdrvAdecDec1Param, sizeof(KDRV_ADEC_DEC_PARAM_T));
		break;

		case KDRV_ADEC_SE :
			memcpy(param, &gKdrvAdecSeParam, sizeof(KDRV_ADEC_SE_PARAM_T));
		break;

		case KDRV_ADEC_SPK :
			memcpy(param, &gKdrvAdecSpkParam, sizeof(KDRV_ADEC_SPK_PARAM_T));
		break;

		case KDRV_ADEC_SPDIF :
			memcpy(param, &gKdrvAdecSpdifParam, sizeof(KDRV_ADEC_SPDIF_PARAM_T));
		break;

		case KDRV_ADEC_HP :
			memcpy(param, &gKdrvAdecHpParam, sizeof(KDRV_ADEC_HP_PARAM_T));
		break;

		case KDRV_ADEC_SCART :
			memcpy(param, &gKdrvAdecScartParam, sizeof(KDRV_ADEC_SCART_PARAM_T));
		break;

		case KDRV_ADEC_LINEOUT :
			memcpy(param, &gKdrvAdecLineoutParam, sizeof(KDRV_ADEC_LINEOUT_PARAM_T));
		break;

		case KDRV_ADEC_MP_00 :
			memcpy(param, &gKdrvAdecMP00Param, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
		break;

		case KDRV_ADEC_MP_01 :
			memcpy(param, &gKdrvAdecMP01Param, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
		break;

		case KDRV_ADEC_MP_02 :
			memcpy(param, &gKdrvAdecMP02Param, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
		break;

		case KDRV_ADEC_MP_03 :
			memcpy(param, &gKdrvAdecMP03Param, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
		break;

		case KDRV_ADEC_MP_04 :
			memcpy(param, &gKdrvAdecMP04Param, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
		break;

		case KDRV_ADEC_MP_05 :
			memcpy(param, &gKdrvAdecMP05Param, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
		break;

		case KDRV_ADEC_MP_06 :
			memcpy(param, &gKdrvAdecMP06Param, sizeof(KDRV_ADEC_MIX_START_PARAM_T));
		break;

		case KDRV_ADEC_ENC :
			memcpy(param, &gKdrvAdecEncParam, sizeof(KDRV_ADEC_ENC_PARAM_T));
		break;

		default :			
			ADEC_ERROR("ADEC_IPC_GetConfig : m(%d)\n", module);
		break;
	}

	return 0;
}

SINT32 ADEC_IPC_Start(KDRV_ADEC_MOD_T module)
{
	ADEC_CMD_START_DECODING_T	 	param;
	ADEC_CMD_READY_DECODING_T	 	readyParam;

	if((module < KDRV_ADEC_MP_02) || (module > KDRV_ADEC_MP_07))
	{
		// 1. get codec status from DSP
		// 2. make CMD
		readyParam.ui32ReadyData = 0;

		// 3. send CMD
		ADEC_IPC_LOCK();
		ADEC_IPC_CMD_Send(ADEC_IPC_CMD_READY_DECODING, sizeof(ADEC_CMD_READY_DECODING_T), &readyParam);
		ADEC_IPC_UNLOCK();
	}

	// 1. get codec status from DSP
	// 2. make CMD
	param.module = module;

	// 3. send CMD
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_START_DECODING, sizeof(ADEC_CMD_START_DECODING_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_Start : %d\n", module);
#endif

	return 0 ;
}

SINT32 ADEC_IPC_Stop(KDRV_ADEC_MOD_T module)
{
	ADEC_CMD_STOP_DECODING_T	 	param;

	// 1. get codec status from DSP
	// 2. make CMD
	param.module = module;

	// 3. send CMD
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_STOP_DECODING , sizeof(ADEC_CMD_STOP_DECODING_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_Stop : %d\n", module);
#endif

	return 0 ;
}


SINT32 ADEC_IPC_Pause(KDRV_ADEC_MOD_T module)
{
	UINT64	diffJiffies = 0;

	ADEC_CMD_PAUSE_DECODING_T	 	param;

	//add IPC command more time for GCD platform
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		if(module == KDRV_ADEC_DEC_0 || module == KDRV_ADEC_DEC_1)
		{
			PauseJiffies = jiffies;

			diffJiffies = abs(PauseJiffies - ResumeJiffies);

			if( diffJiffies < IPC_ADDITIONAL_DELAY)
			{
				ADEC_DEBUG("ADEC_IPC_Pause(diff = %llu, delay = %d ms)\n", diffJiffies, IPC_ADDITIONAL_DELAY);
				msleep_interruptible(IPC_ADDITIONAL_DELAY);
			}
		}
	}

	// 1. get codec status from DSP
	// 2. make CMD
	param.module = module;

	// 3. send CMD
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_PAUSE_DECODING, sizeof(ADEC_CMD_PAUSE_DECODING_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_Pause : %d\n", module);
#endif

	return 0;
}

SINT32 ADEC_IPC_Resume(KDRV_ADEC_MOD_T module)
{
	UINT64	diffJiffies = 0;

	ADEC_CMD_RESUME_DECODING_T	 	param;

	//add IPC command more time for GCD platform
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		if(module == KDRV_ADEC_DEC_0 || module == KDRV_ADEC_DEC_1)
		{
			ResumeJiffies = jiffies;

			diffJiffies = abs(ResumeJiffies - PauseJiffies);

			if( diffJiffies < IPC_ADDITIONAL_DELAY)
			{
				ADEC_DEBUG("ADEC_IPC_Resume(diff = %llu, delay = %d ms)\n", diffJiffies, IPC_ADDITIONAL_DELAY);
				msleep_interruptible(IPC_ADDITIONAL_DELAY);
			}
		}
	}

	// 1. get codec status from DSP
	// 2. make CMD
	param.module = module;

	// 3. send CMD
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_RESUME_DECODING, sizeof(ADEC_CMD_RESUME_DECODING_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_Resume : %d\n", module);
#endif

	return 0;
}

SINT32 ADEC_IPC_Flush(KDRV_ADEC_MOD_T module)
{
	ADEC_CMD_FLUSH_DECODING_T param;

	param.module = module;

	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_FLUSH_DECODING, sizeof(ADEC_CMD_FLUSH_DECODING_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_Flush : %d\n", module);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetLipsync (KDRV_ADEC_MOD_T module, KDRV_ADEC_LIPSYNC_PARAM_T *param)
{
	ADEC_CMD_SETLIPSYNC_MODULE_T setLipsync;

	setLipsync.module = module;

	memcpy(&(setLipsync.param[0]), param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));

	ADEC_IPC_LOCK();

	ADEC_IPC_CMD_Send(ADEC_IPC_SET_AV_LIPSYNC, sizeof(ADEC_CMD_SETLIPSYNC_MODULE_T), &setLipsync);

	ADEC_IPC_UNLOCK();

	if(module == KDRV_ADEC_DEC_0)
	{
		memcpy(&gKdrvAdecLipsync0Param, param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_DEC_1)
	{
		memcpy(&gKdrvAdecLipsync1Param, param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_02)
	{
		memcpy(&gKdrvAdecLipsync2Param, param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_03)
	{
		memcpy(&gKdrvAdecLipsync3Param, param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_04)
	{
		memcpy(&gKdrvAdecLipsync4Param, param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_05)
	{
		memcpy(&gKdrvAdecLipsync5Param, param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_06)
	{
		memcpy(&gKdrvAdecLipsync6Param, param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else
	{
		ADEC_ERROR("ADEC_IPC_SetLipsync : %d\n", module);
	}

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetLipsync : %d\n", module);
#endif

	return 0;
}

SINT32 ADEC_IPC_GetLipsync (KDRV_ADEC_MOD_T module, KDRV_ADEC_LIPSYNC_PARAM_T *param)
{
	if(module == KDRV_ADEC_DEC_0)
	{
		memcpy(param, &gKdrvAdecLipsync0Param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_DEC_1)
	{
		memcpy(param, &gKdrvAdecLipsync1Param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_02)
	{
		memcpy(param, &gKdrvAdecLipsync2Param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_03)
	{
		memcpy(param, &gKdrvAdecLipsync3Param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_04)
	{
		memcpy(param, &gKdrvAdecLipsync4Param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_05)
	{
		memcpy(param, &gKdrvAdecLipsync5Param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else if(module == KDRV_ADEC_MP_06)
	{
		memcpy(param, &gKdrvAdecLipsync6Param, sizeof(KDRV_ADEC_LIPSYNC_PARAM_T));
	}
	else
	{
		ADEC_ERROR("ADEC_IPC_GetLipsync : %d\n", module);
	}

	return 0;
}

SINT32 ADEC_IPC_TrickMode (KDRV_ADEC_TRICK_MODE_T mode)
{
	ADEC_SET_TRICK_STATE_T	 	param;

	// 1. get codec status from DSP
	// 2. make CMD
	param.ui32TrickState = mode;

	// 3. send CMD
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_TRICK_STATE, sizeof(ADEC_SET_TRICK_STATE_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_TrickMode : %d\n", mode);
#endif

	return 0;
}

SINT32 ADEC_IPC_StartLoadCodec(KDRV_ADEC_SRC_TYPE_T codec)
{

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_StartLoadCodec : %d\n", codec);
#endif

	return 0;
}

SINT32 ADEC_IPC_EndLoadCodec(KDRV_ADEC_SRC_TYPE_T codec)
{

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_EndLoadCodec : %d\n", codec);
#endif

	return 0;
}

SINT32 ADEC_IPC_CheckDSPRunning(void)
{
	ADEC_CMD_ALIVE_PARAM_T param;

	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_CMD_ASK_ALIVE, sizeof(ADEC_CMD_ALIVE_PARAM_T), &param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_CheckDSPRunning\n");
#endif

	return 0;
}

SINT32 ADEC_IPC_SetBaseTime(ADEC_CMD_SYNC_BASE_T syncBase)
{
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_SYNC_BASE, sizeof(ADEC_CMD_SYNC_BASE_T), &syncBase);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSyncBase, &syncBase, sizeof(ADEC_CMD_SYNC_BASE_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetBaseTime : %d %d\n", (UINT32)syncBase.clockBase, (UINT32)syncBase.streamBase);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetBass(UINT8 ui8Bass)
{
	ADEC_SET_BASS_T setBass;

	setBass.ui32Bass = ui8Bass;

	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_BASS, sizeof(ADEC_SET_BASS_T), &setBass);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSetBass, &setBass, sizeof(ADEC_SET_BASS_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetBass : %d\n", ui8Bass);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetTreble(UINT8 ui8Treble)
{
	ADEC_SET_TREBLE_T setTreble;

	setTreble.ui32Treble = ui8Treble;

	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_TREBLE, sizeof(ADEC_SET_TREBLE_T), &setTreble);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSetTreble, &setTreble, sizeof(ADEC_SET_TREBLE_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetTreble : %d\n", ui8Treble);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetVolume(ADEC_SET_VOLUME_T setVolume)
{
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_VOLUME, sizeof(ADEC_SET_VOLUME_T), &setVolume);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSetVolume, &setVolume, sizeof(ADEC_SET_VOLUME_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetVolume : %d %d\n", setVolume.module, setVolume.ui32Volume);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetPCMRate(ADEC_SET_PCM_INTERRUPT_T pcmRate)
{
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_PCMINTRATE, sizeof(ADEC_SET_PCM_INTERRUPT_T), &pcmRate);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSetPcmRate, &pcmRate, sizeof(ADEC_SET_PCM_INTERRUPT_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetPCMRate : %d, %d, %d\n", pcmRate.ui32InFs, pcmRate.ui32PCMclkRate, pcmRate.ui32ForceCntClear);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetSPDIFSCMS(ADEC_SET_SPDIF_SCMS_T spdifSCMS)
{
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_SPDIF_SCMS, sizeof(ADEC_SET_SPDIF_SCMS_T), &spdifSCMS);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSetSpdifScms, &spdifSCMS, sizeof(ADEC_SET_SPDIF_SCMS_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetSPDIFSCMS : %d\n", spdifSCMS.ui32Scms);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetSPDIFDTO(ADEC_SET_SPDIF_DTO_T spdifDTO)
{
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_SPDIF_DTO, sizeof(ADEC_SET_SPDIF_DTO_T), &spdifDTO);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSetSpdifDto, &spdifDTO, sizeof(ADEC_SET_SPDIF_DTO_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetSPDIFDTO : %d, %d\n", spdifDTO.ui32SampleFreq, spdifDTO.ui32DtoRate);
#endif

	return 0;
}

SINT32 ADEC_IPC_SetSPDIFIEC(ADEC_SET_SPDIF_IEC_T spdifIec)
{
#if 1	//GCD, GLD only support this function for SPDIF DEC0 or DEC1 output.
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_SPDIF_IEC, sizeof(ADEC_SET_SPDIF_IEC_T), &spdifIec);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecSetSpdifIec, &spdifIec, sizeof(ADEC_SET_SPDIF_IEC_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetSPDIFIEC : %d, %d\n", spdifIec.ui32SetIecSrc, spdifIec.ui32LipsyncOnOff);
#endif
#endif

	return 0;
}

SINT32 ADEC_IPC_SetMixExtParam(ADEC_MIX_EXT_PARAM_T mixExtParam)
{
	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(ADEC_IPC_SET_MIX_EXT_CNF, sizeof(ADEC_MIX_EXT_PARAM_T), &mixExtParam);
	ADEC_IPC_UNLOCK();

	memcpy(&gAdecMixExtParam, &mixExtParam, sizeof(ADEC_MIX_EXT_PARAM_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetMixExtParam : %d, %d\n", mixExtParam.mixPort, mixExtParam.initDelay);
#endif

	return 0;
}


SINT32 ADEC_IPC_GetSPDIFSCMS(ADEC_SET_SPDIF_SCMS_T *pSpdifSCMS)
{
	memcpy(pSpdifSCMS, &gAdecSetSpdifScms, sizeof(LX_ADEC_SPDIF_SCMS_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_GetSPDIFSCMS : %d\n", pSpdifSCMS->ui32Scms);
#endif

	return 0;
}

SINT32 ADEC_IPC_GetSPDIFIEC(ADEC_SET_SPDIF_IEC_T *pSpdifIec)
{
	memcpy(pSpdifIec, &gAdecSetSpdifIec, sizeof(ADEC_SET_SPDIF_IEC_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_GetSPDIFIEC : %d\n", pSpdifIec->ui32SetIecSrc);
#endif

	return 0;
}

SINT32 ADEC_IPC_GetMixExtParam(ADEC_MIX_EXT_PARAM_T *pMixExtParam)
{
	memcpy(pMixExtParam, &gAdecMixExtParam, sizeof(ADEC_MIX_EXT_PARAM_T));

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_GetMixExtParam : %d\n", pMixExtParam->mixPort);
#endif

	return 0;
}



#ifdef ENABLE_DAMP_VOLUME_CONTROL
SINT32 ADEC_IPC_SetDisableSeEngine(void)
{
	LX_ADEC_CUSTOM_IPC_T		customIpc;

	//Disable a sound engine
	customIpc.hdr = 0x40403;
	customIpc.bodySize = 1;
	customIpc.param[0] = 0;

	ADEC_IPC_LOCK();
	ADEC_IPC_CMD_Send(customIpc.hdr, customIpc.bodySize, customIpc.param);
	ADEC_IPC_UNLOCK();

#ifdef ENABLE_IPC_DEBUG
	ADEC_ERROR("ADEC_IPC_SetDisableSeEngine : %d\n", module);
#endif

	return 0;
}
#endif	//#ifdef ENABLE_DAMP_VOLUME_CONTROL


void ADEC_IPC_ShowDEC0Param(void)
{
	UINT32 i;

	ADEC_ERROR("ADEC_IPC_ShowDEC0Param\n");
	ADEC_ERROR("adecInputPort    : %d\n", gKdrvAdecDec0Param.adecInputPort);
	ADEC_ERROR("adecCodecType    : %d\n", gKdrvAdecDec0Param.adecCodecType);
	ADEC_ERROR("bitRate          : %d\n", gKdrvAdecDec0Param.bitRate);
	ADEC_ERROR("samplingFreq     : %d\n", gKdrvAdecDec0Param.samplingFreq);
	ADEC_ERROR("numberOfChannel  : %d\n", gKdrvAdecDec0Param.numberOfChannel);
	ADEC_ERROR("pcmBitsPerSample : %d\n", gKdrvAdecDec0Param.pcmBitsPerSample);
	for(i=0; i<KDRV_ADEC_SET_DECODE_NUM; i++)
	ADEC_ERROR("ui32Param[%d]     : 0x%08x\n", i, gKdrvAdecDec0Param.ui32Param[i]);

	return;
}

void ADEC_IPC_ShowDEC1Param(void)
{
	UINT32 i;

	ADEC_ERROR("ADEC_IPC_ShowDEC1Param\n");
	ADEC_ERROR("adecInputPort    : %d\n", gKdrvAdecDec1Param.adecInputPort);
	ADEC_ERROR("adecCodecType    : %d\n", gKdrvAdecDec1Param.adecCodecType);
	ADEC_ERROR("bitRate          : %d\n", gKdrvAdecDec1Param.bitRate);
	ADEC_ERROR("samplingFreq     : %d\n", gKdrvAdecDec1Param.samplingFreq);
	ADEC_ERROR("numberOfChannel  : %d\n", gKdrvAdecDec1Param.numberOfChannel);
	ADEC_ERROR("pcmBitsPerSample : %d\n", gKdrvAdecDec1Param.pcmBitsPerSample);
	for(i=0; i<KDRV_ADEC_SET_DECODE_NUM; i++)
	ADEC_ERROR("ui32Param[%d]     : 0x%08x\n", i, gKdrvAdecDec1Param.ui32Param[i]);

	return;
}

void ADEC_IPC_ShowSEParam(void)
{
	UINT32 i;

	ADEC_ERROR("ADEC_IPC_ShowSEParam\n");
	ADEC_ERROR("ui32FnMode       : %d\n", gKdrvAdecSeParam.seFnParam.ui32FnMode);
	ADEC_ERROR("ui32DataOption   : %d\n", gKdrvAdecSeParam.seFnParam.ui32DataOption);
	ADEC_ERROR("ui32VarOption    : %d\n", gKdrvAdecSeParam.seFnParam.ui32VarOption);
	ADEC_ERROR("ui32AccessMode   : %d\n", gKdrvAdecSeParam.seFnParam.ui32AccessMode);
	ADEC_ERROR("ui32NoParam      : %d\n", gKdrvAdecSeParam.seFnParam.ui32NoParam);
	for(i=0; i<KDRV_SE_SET_MODE_NUM; i++)
	ADEC_ERROR("ui32Param[%d]     : 0x%08x\n", i, gKdrvAdecSeParam.seSetMode.ui32Param[i]);

	return;
}

void ADEC_IPC_ShowSPKParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowSPKParam\n");
	ADEC_ERROR("outmode          : %d\n", gKdrvAdecSpkParam.outmode);
	ADEC_ERROR("mute             : %d\n", gKdrvAdecSpkParam.mute);
	ADEC_ERROR("volctrl          : %d\n", gKdrvAdecSpkParam.volctrl);
	ADEC_ERROR("volume           : 0x%08x\n", gKdrvAdecSpkParam.volume);
	ADEC_ERROR("delayMS          : %d\n", gKdrvAdecSpkParam.delayMS);
	ADEC_ERROR("balance          : %d\n", gKdrvAdecSpkParam.balance);

	ADEC_ERROR("InFs             : %d\n", gAdecSetPcmRate.ui32InFs);
	ADEC_ERROR("PCMclkRate       : %d\n", gAdecSetPcmRate.ui32PCMclkRate);
	ADEC_ERROR("ForceCntClear    : %d\n", gAdecSetPcmRate.ui32ForceCntClear);

	return;
}

void ADEC_IPC_ShowSPDIFParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowSPDIFParam\n");
	ADEC_ERROR("type             : %d\n", gKdrvAdecSpdifParam.type);
	ADEC_ERROR("outmode          : %d\n", gKdrvAdecSpdifParam.outmode);
	ADEC_ERROR("mute             : %d\n", gKdrvAdecSpdifParam.mute);
	ADEC_ERROR("volctrl          : %d\n", gKdrvAdecSpdifParam.volctrl);
	ADEC_ERROR("volume           : 0x%08x\n", gKdrvAdecSpdifParam.volume);
	ADEC_ERROR("delayMS          : %d\n", gKdrvAdecSpdifParam.delayMS);

	ADEC_ERROR("SCMS	         : %d\n", gAdecSetSpdifScms.ui32Scms);

	ADEC_ERROR("SampleFreq       : %d\n", gAdecSetSpdifDto.ui32SampleFreq);
	ADEC_ERROR("DtoRate          : %d\n", gAdecSetSpdifDto.ui32DtoRate);
	ADEC_ERROR("ForceCntClear    : %d\n", gAdecSetSpdifDto.ui32ForceCntClear);

	ADEC_ERROR("IEC source and sync disabled now!!!\n");
	ADEC_ERROR("IEC source       : %d\n", gAdecSetSpdifIec.ui32SetIecSrc);
	ADEC_ERROR("lip sync         : %d\n", gAdecSetSpdifIec.ui32LipsyncOnOff);

	return;
}

void ADEC_IPC_ShowHPParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowHPParam\n");
	ADEC_ERROR("outmode          : %d\n", gKdrvAdecHpParam.outmode);
	ADEC_ERROR("mute             : %d\n", gKdrvAdecHpParam.mute);
	ADEC_ERROR("volctrl          : %d\n", gKdrvAdecHpParam.volctrl);
	ADEC_ERROR("volume           : 0x%08x\n", gKdrvAdecHpParam.volume);
	ADEC_ERROR("delayMS          : %d\n", gKdrvAdecHpParam.delayMS);
	ADEC_ERROR("user volume      : 0x%08x\n", gAdecSetVolume.ui32Volume);

	return;
}

void ADEC_IPC_ShowSCARTParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowSCARTParam\n");
	ADEC_ERROR("outmode          : %d\n", gKdrvAdecScartParam.outmode);
	ADEC_ERROR("mute             : %d\n", gKdrvAdecScartParam.mute);
	ADEC_ERROR("volctrl          : %d\n", gKdrvAdecScartParam.volctrl);
	ADEC_ERROR("volume           : 0x%08x\n", gKdrvAdecScartParam.volume);
	ADEC_ERROR("delayMS          : %d\n", gKdrvAdecScartParam.delayMS);

	return;
}

void ADEC_IPC_ShowMIX0Param(void)
{
	ADEC_ERROR("ADEC_IPC_ShowMIX0Param\n");
	ADEC_ERROR("portnum          : %d\n", gKdrvAdecMP02Param.ui32PortNum);
	ADEC_ERROR("numerOfChannel   : %d\n", gKdrvAdecMP02Param.ui32NumOfCH);
	ADEC_ERROR("bitPerSample     : %d\n", gKdrvAdecMP02Param.ui32BitPerSample);
	ADEC_ERROR("sampleRate       : %d\n", gKdrvAdecMP02Param.ui32SampleRate);
	ADEC_ERROR("gainL            : %d\n", gKdrvAdecMP02Param.ui32GainL);
	ADEC_ERROR("gainR            : %d\n", gKdrvAdecMP02Param.ui32GainR);

	ADEC_ERROR("gKdrvAdecLipsync2Param\n");
	ADEC_ERROR("bOnOff           : %d\n", gKdrvAdecLipsync2Param.bOnOff);
	ADEC_ERROR("ubound           : %d\n", gKdrvAdecLipsync2Param.ubound);
	ADEC_ERROR("lbound           : %d\n", gKdrvAdecLipsync2Param.lbound);
	ADEC_ERROR("offset           : %d\n", gKdrvAdecLipsync2Param.offset);
	ADEC_ERROR("freerunubound    : %d\n", gKdrvAdecLipsync2Param.freerunubound);
	ADEC_ERROR("freerunlbound    : %d\n", gKdrvAdecLipsync2Param.freerunlbound);

	return;
}


void ADEC_IPC_ShowLIPSYNCParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowLIPSYNCParam\n");
	ADEC_ERROR("gKdrvAdecLipsync0Param\n");
	ADEC_ERROR("bOnOff           : %d\n", gKdrvAdecLipsync0Param.bOnOff);
	ADEC_ERROR("ubound           : %d\n", gKdrvAdecLipsync0Param.ubound);
	ADEC_ERROR("lbound           : %d\n", gKdrvAdecLipsync0Param.lbound);
	ADEC_ERROR("offset           : %d\n", gKdrvAdecLipsync0Param.offset);
	ADEC_ERROR("freerunubound    : %d\n", gKdrvAdecLipsync0Param.freerunubound);
	ADEC_ERROR("freerunlbound    : %d\n", gKdrvAdecLipsync0Param.freerunlbound);

	ADEC_ERROR("gKdrvAdecLipsync1Param\n");
	ADEC_ERROR("bOnOff           : %d\n", gKdrvAdecLipsync1Param.bOnOff);
	ADEC_ERROR("ubound           : %d\n", gKdrvAdecLipsync1Param.ubound);
	ADEC_ERROR("lbound           : %d\n", gKdrvAdecLipsync1Param.lbound);
	ADEC_ERROR("offset           : %d\n", gKdrvAdecLipsync1Param.offset);
	ADEC_ERROR("freerunubound    : %d\n", gKdrvAdecLipsync1Param.freerunubound);
	ADEC_ERROR("freerunlbound    : %d\n", gKdrvAdecLipsync1Param.freerunlbound);

	ADEC_ERROR("gAdecSyncBase\n");
	ADEC_ERROR("module           : %d\n", gAdecSyncBase.module);
	ADEC_ERROR("clockBase        : %llu\n", gAdecSyncBase.clockBase);
	ADEC_ERROR("streamBase       : %llu\n", gAdecSyncBase.streamBase);

	return;
}

void ADEC_IPC_ShowDEC0MIXParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowDEC0MIXParam\n");
	ADEC_ERROR("portnum          : %d\n", gKdrvAdecMP00Param.ui32PortNum);
	ADEC_ERROR("numerOfChannel   : %d\n", gKdrvAdecMP00Param.ui32NumOfCH);
	ADEC_ERROR("bitPerSample     : %d\n", gKdrvAdecMP00Param.ui32BitPerSample);
	ADEC_ERROR("sampleRate       : %d\n", gKdrvAdecMP00Param.ui32SampleRate);
	ADEC_ERROR("gainL            : %d\n", gKdrvAdecMP00Param.ui32GainL);
	ADEC_ERROR("gainR            : %d\n", gKdrvAdecMP00Param.ui32GainR);
	ADEC_ERROR("****************************************\n");

	ADEC_ERROR("gAdecMixExtParam\n");
	ADEC_ERROR("mixPort          : %d\n", gAdecMixExtParam.mixPort);
	ADEC_ERROR("waitLength       : %d\n", gAdecMixExtParam.waitLength);
	ADEC_ERROR("initialMuteLength: %d\n", gAdecMixExtParam.initialMuteLength);
	ADEC_ERROR("autoFadeOutEnable: %d\n", gAdecMixExtParam.autoFadeOutEnable);
	ADEC_ERROR("autoFadeInEnable : %d\n", gAdecMixExtParam.autoFadeInEnable);
	ADEC_ERROR("fadeLength       : %d\n", gAdecMixExtParam.fadeLength);
	ADEC_ERROR("initDelay        : %d\n", gAdecMixExtParam.initDelay);

	return;
}

void ADEC_IPC_ShowDEC1MIXParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowDEC1MIXParam\n");
	ADEC_ERROR("portnum          : %d\n", gKdrvAdecMP01Param.ui32PortNum);
	ADEC_ERROR("numerOfChannel   : %d\n", gKdrvAdecMP01Param.ui32NumOfCH);
	ADEC_ERROR("bitPerSample     : %d\n", gKdrvAdecMP01Param.ui32BitPerSample);
	ADEC_ERROR("sampleRate       : %d\n", gKdrvAdecMP01Param.ui32SampleRate);
	ADEC_ERROR("gainL            : %d\n", gKdrvAdecMP01Param.ui32GainL);
	ADEC_ERROR("gainR            : %d\n", gKdrvAdecMP01Param.ui32GainR);

	return;
}

void ADEC_IPC_ShowMIXParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowMIX1Param\n");
	ADEC_ERROR("gKdrvAdecMP03Param\n");
	ADEC_ERROR("portnum          : %d\n", gKdrvAdecMP03Param.ui32PortNum);
	ADEC_ERROR("numerOfChannel   : %d\n", gKdrvAdecMP03Param.ui32NumOfCH);
	ADEC_ERROR("bitPerSample     : %d\n", gKdrvAdecMP03Param.ui32BitPerSample);
	ADEC_ERROR("sampleRate       : %d\n", gKdrvAdecMP03Param.ui32SampleRate);
	ADEC_ERROR("gainL            : %d\n", gKdrvAdecMP03Param.ui32GainL);
	ADEC_ERROR("gainR            : %d\n", gKdrvAdecMP03Param.ui32GainR);

	ADEC_ERROR("gKdrvAdecLipsync3Param\n");
	ADEC_ERROR("bOnOff           : %d\n", gKdrvAdecLipsync3Param.bOnOff);
	ADEC_ERROR("ubound           : %d\n", gKdrvAdecLipsync3Param.ubound);
	ADEC_ERROR("lbound           : %d\n", gKdrvAdecLipsync3Param.lbound);
	ADEC_ERROR("offset           : %d\n", gKdrvAdecLipsync3Param.offset);
	ADEC_ERROR("freerunubound    : %d\n", gKdrvAdecLipsync3Param.freerunubound);
	ADEC_ERROR("freerunlbound    : %d\n", gKdrvAdecLipsync3Param.freerunlbound);
	ADEC_ERROR("****************************************\n");

	ADEC_ERROR("gKdrvAdecMP04Param\n");
	ADEC_ERROR("portnum          : %d\n", gKdrvAdecMP04Param.ui32PortNum);
	ADEC_ERROR("numerOfChannel   : %d\n", gKdrvAdecMP04Param.ui32NumOfCH);
	ADEC_ERROR("bitPerSample     : %d\n", gKdrvAdecMP04Param.ui32BitPerSample);
	ADEC_ERROR("sampleRate       : %d\n", gKdrvAdecMP04Param.ui32SampleRate);
	ADEC_ERROR("gainL            : %d\n", gKdrvAdecMP04Param.ui32GainL);
	ADEC_ERROR("gainR            : %d\n", gKdrvAdecMP04Param.ui32GainR);

	ADEC_ERROR("gKdrvAdecLipsync4Param\n");
	ADEC_ERROR("bOnOff           : %d\n", gKdrvAdecLipsync4Param.bOnOff);
	ADEC_ERROR("ubound           : %d\n", gKdrvAdecLipsync4Param.ubound);
	ADEC_ERROR("lbound           : %d\n", gKdrvAdecLipsync4Param.lbound);
	ADEC_ERROR("offset           : %d\n", gKdrvAdecLipsync4Param.offset);
	ADEC_ERROR("freerunubound    : %d\n", gKdrvAdecLipsync4Param.freerunubound);
	ADEC_ERROR("freerunlbound    : %d\n", gKdrvAdecLipsync4Param.freerunlbound);
	ADEC_ERROR("****************************************\n");

	ADEC_ERROR("gKdrvAdecMP05Param\n");
	ADEC_ERROR("portnum          : %d\n", gKdrvAdecMP05Param.ui32PortNum);
	ADEC_ERROR("numerOfChannel   : %d\n", gKdrvAdecMP05Param.ui32NumOfCH);
	ADEC_ERROR("bitPerSample     : %d\n", gKdrvAdecMP05Param.ui32BitPerSample);
	ADEC_ERROR("sampleRate       : %d\n", gKdrvAdecMP05Param.ui32SampleRate);
	ADEC_ERROR("gainL            : %d\n", gKdrvAdecMP05Param.ui32GainL);
	ADEC_ERROR("gainR            : %d\n", gKdrvAdecMP05Param.ui32GainR);

	ADEC_ERROR("gKdrvAdecLipsync5Param\n");
	ADEC_ERROR("bOnOff           : %d\n", gKdrvAdecLipsync5Param.bOnOff);
	ADEC_ERROR("ubound           : %d\n", gKdrvAdecLipsync5Param.ubound);
	ADEC_ERROR("lbound           : %d\n", gKdrvAdecLipsync5Param.lbound);
	ADEC_ERROR("offset           : %d\n", gKdrvAdecLipsync5Param.offset);
	ADEC_ERROR("freerunubound    : %d\n", gKdrvAdecLipsync5Param.freerunubound);
	ADEC_ERROR("freerunlbound    : %d\n", gKdrvAdecLipsync5Param.freerunlbound);
	ADEC_ERROR("****************************************\n");

	ADEC_ERROR("gKdrvAdecMP06Param\n");
	ADEC_ERROR("portnum          : %d\n", gKdrvAdecMP06Param.ui32PortNum);
	ADEC_ERROR("numerOfChannel   : %d\n", gKdrvAdecMP06Param.ui32NumOfCH);
	ADEC_ERROR("bitPerSample     : %d\n", gKdrvAdecMP06Param.ui32BitPerSample);
	ADEC_ERROR("sampleRate       : %d\n", gKdrvAdecMP06Param.ui32SampleRate);
	ADEC_ERROR("gainL            : %d\n", gKdrvAdecMP06Param.ui32GainL);
	ADEC_ERROR("gainR            : %d\n", gKdrvAdecMP06Param.ui32GainR);

	ADEC_ERROR("gKdrvAdecLipsync6Param\n");
	ADEC_ERROR("bOnOff           : %d\n", gKdrvAdecLipsync6Param.bOnOff);
	ADEC_ERROR("ubound           : %d\n", gKdrvAdecLipsync6Param.ubound);
	ADEC_ERROR("lbound           : %d\n", gKdrvAdecLipsync6Param.lbound);
	ADEC_ERROR("offset           : %d\n", gKdrvAdecLipsync6Param.offset);
	ADEC_ERROR("freerunubound    : %d\n", gKdrvAdecLipsync6Param.freerunubound);
	ADEC_ERROR("freerunlbound    : %d\n", gKdrvAdecLipsync6Param.freerunlbound);
	ADEC_ERROR("****************************************\n");

	return;
}

void ADEC_IPC_ShowAENCParam(void)
{
	ADEC_ERROR("ADEC_IPC_ShowAENCParam\n");
	ADEC_ERROR("adecInputMode     : %d\n", gKdrvAdecEncParam.adecInputMode);
	ADEC_ERROR("aencCodecType     : %d\n", gKdrvAdecEncParam.aencCodecType);
	ADEC_ERROR("aencOutMode       : %d\n", gKdrvAdecEncParam.aencOutMode);
	ADEC_ERROR("channelNumber     : %d\n", gKdrvAdecEncParam.channelNumber);
	ADEC_ERROR("bitPerSample      : %d\n", gKdrvAdecEncParam.bitPerSample);
	ADEC_ERROR("samplingFrequency : %d\n", gKdrvAdecEncParam.samplingFrequency);
	ADEC_ERROR("bitRate           : %d\n", gKdrvAdecEncParam.bitRate);
	//ADEC_ERROR("duration             : %d\n", gKdrvAdecEncParam.duration);

	return;
}


