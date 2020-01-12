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

#ifndef	_ADEC_IPC_FUNC_H_
#define	_ADEC_IPC_FUNC_H_

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "../adec_drv.h"
#include "../adec_cfg.h"
#include "adec_kapi.h"
#include "adec_reg_l9.h"
#include "adec_coredrv_l9.h"
#include "adec_ipc/adec_ipc_list.h"

#include <mach/hardware.h> // For Register base address


SINT32 ADEC_IPC_InitFunc(void);
SINT32 ADEC_IPC_Connect(KDRV_ADEC_MOD_T module, KDRV_ADEC_MOD_T connector);
SINT32 ADEC_IPC_Disconnect(KDRV_ADEC_MOD_T module);
SINT32 ADEC_IPC_SetConfig(KDRV_ADEC_MOD_T module, void *param);
SINT32 ADEC_IPC_GetConfig(KDRV_ADEC_MOD_T module, void *param);
SINT32 ADEC_IPC_Start(KDRV_ADEC_MOD_T module);
SINT32 ADEC_IPC_Stop(KDRV_ADEC_MOD_T module);
SINT32 ADEC_IPC_Pause(KDRV_ADEC_MOD_T module);
SINT32 ADEC_IPC_Resume(KDRV_ADEC_MOD_T module);
SINT32 ADEC_IPC_Flush(KDRV_ADEC_MOD_T module);
SINT32 ADEC_IPC_SetLipsync (KDRV_ADEC_MOD_T module, KDRV_ADEC_LIPSYNC_PARAM_T *param);
SINT32 ADEC_IPC_GetLipsync (KDRV_ADEC_MOD_T module, KDRV_ADEC_LIPSYNC_PARAM_T *param);
SINT32 ADEC_IPC_TrickMode (KDRV_ADEC_TRICK_MODE_T mode);
SINT32 ADEC_IPC_StartLoadCodec(KDRV_ADEC_SRC_TYPE_T codec);
SINT32 ADEC_IPC_EndLoadCodec(KDRV_ADEC_SRC_TYPE_T codec);
SINT32 ADEC_IPC_CheckDSPRunning(void);
SINT32 ADEC_IPC_SetBaseTime(ADEC_CMD_SYNC_BASE_T syncBase);
SINT32 ADEC_IPC_SetBass(UINT8 ui8Bass);
SINT32 ADEC_IPC_SetTreble(UINT8 ui8Treble);
SINT32 ADEC_IPC_SetVolume(ADEC_SET_VOLUME_T setVolume);
SINT32 ADEC_IPC_SetPCMRate(ADEC_SET_PCM_INTERRUPT_T pcmRate);
SINT32 ADEC_IPC_SetSPDIFSCMS(ADEC_SET_SPDIF_SCMS_T spdifSCMS);
SINT32 ADEC_IPC_SetSPDIFDTO(ADEC_SET_SPDIF_DTO_T spdifDTO);
SINT32 ADEC_IPC_SetSPDIFIEC(ADEC_SET_SPDIF_IEC_T spdifIec);
SINT32 ADEC_IPC_SetMixExtParam(ADEC_MIX_EXT_PARAM_T mixExtParam);

SINT32 ADEC_IPC_GetSPDIFSCMS(ADEC_SET_SPDIF_SCMS_T *pSpdifSCMS);
SINT32 ADEC_IPC_GetSPDIFIEC(ADEC_SET_SPDIF_IEC_T *pSpdifIec);
SINT32 ADEC_IPC_GetMixExtParam(ADEC_MIX_EXT_PARAM_T *pMixExtParam);

#ifdef ENABLE_DAMP_VOLUME_CONTROL
SINT32 ADEC_IPC_SetDisableSeEngine(void);
#endif

void ADEC_IPC_ShowDEC0Param(void);
void ADEC_IPC_ShowDEC1Param(void);
void ADEC_IPC_ShowSEParam(void);
void ADEC_IPC_ShowSPKParam(void);
void ADEC_IPC_ShowSPDIFParam(void);
void ADEC_IPC_ShowHPParam(void);
void ADEC_IPC_ShowSCARTParam(void);
void ADEC_IPC_ShowMIX0Param(void);
void ADEC_IPC_ShowLIPSYNCParam(void);
void ADEC_IPC_ShowDEC0MIXParam(void);
void ADEC_IPC_ShowDEC1MIXParam(void);
void ADEC_IPC_ShowMIXParam(void);
void ADEC_IPC_ShowAENCParam(void);

#endif /* _ADEC_IPC_FUNC_H_ */
