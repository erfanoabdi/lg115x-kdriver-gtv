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

/** @file pe_etc.h
 *
 *  driver header for picture enhance etc functions. ( used only within kdriver )
 *	
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note		
 *	@date		2011.07.16
 *	@see		
 */

#ifndef	_PE_ETC_H_
#define	_PE_ETC_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "pe_kapi.h"
#include "pe_fwi.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define PE_SIMUL_MODE_NONE	0x0		///< nothing to do operaton
#define PE_SIMUL_MODE_PE0	0x1		///< operaton on pe0 block
#define PE_SIMUL_MODE_SM	0x2		///< operaton on sm block
#define PE_SIMUL_MODE_PE1	0x4		///< operaton on pe1 block
#define PE_SIMUL_MODE_ALL	(PE_SIMUL_MODE_PE0|PE_SIMUL_MODE_SM|PE_SIMUL_MODE_PE1)

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
void PE_ETC_SetDbgPrintOnOff(UINT32 on_off);
UINT32 PE_ETC_GetDbgPrintOnOff(void);
int PE_ETC_StartSimulSetMode(UINT32 mode_block);
int PE_ETC_EndSimulSetMode(void);
int PE_ETC_SetPqFwCtrlEnable(UINT32 win_id);
UINT32 PE_ETC_GetPqFwCtrlEnable(void);
void PE_ETC_SetPqFwWindow(UINT32 win_id);
UINT32 PE_ETC_GetPqFwWindow(void);
int PE_ETC_InformMuteState(LX_PE_CMN_MUTE_STATE_T *pstParams);
int PE_ETC_InformTnrAutoMode(LX_PE_NRD_TNR_CMN_T *pstParams);
int PE_ETC_InformSclSettingCnt(UINT32 cnt);
int PE_ETC_InformSrcInfo(LX_PE_DEFAULT_SETTINGS_T *pstParams);
int PE_ETC_SetPqFwSettings(PE_FWI_PQ_TABLE_HEAD_T header, UINT8 *pTable, UINT32 tblSize);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PE_ETC_H_ */
