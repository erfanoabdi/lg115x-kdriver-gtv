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
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     jaemo.kim (jaemo.kim@lge.com)
 * version    1.0
 * date       2010.04.24
 * note       Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */

#ifndef  DE_DEV_INC
#define  DE_DEV_INC

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/
int DE_DEV_Open(void);
int DE_DEV_Close(void);
int DE_DEV_MCU(LX_DE_IPC_CLI_T *pstParams);
int DE_DEV_RegRd(UINT32 *pstParams);
int DE_DEV_RegWr(LX_DE_REG_WR_T *pstParams);
int DE_DEV_Init(LX_DE_PANEL_TYPE_T *pstParams);
int DE_DEV_GetFirmwareInfo(LX_DE_FIRMWARE_INFO_T *pstParams);
int DE_DEV_SetBgColor(LX_DE_COLOR_T *pstParams);
int DE_DEV_GetOutFrRate(LX_DE_FR_RATE_T *pstParams);
int DE_DEV_SetWinBlank(LX_DE_SET_WIN_BLANK_T *pstParams);
int DE_DEV_GetInputWin(LX_DE_WIN_DIMENSION_T *pstParams);
int DE_DEV_GetOutWin(LX_DE_WIN_DIMENSION_T *pstParams);
int DE_DEV_SetInputWin(LX_DE_WIN_DIMENSION_T *pstParams);
int DE_DEV_SetOutWin(LX_DE_WIN_DIMENSION_T *pstParams);
int DE_DEV_SetFreeze(LX_DE_SET_FREEZE_T *pstParams);
int DE_DEV_SetInputSrc(LX_DE_SET_INPUT_T *pstParams);
int DE_DEV_SetDisOut(BOOLEAN *pstParams);
int DE_DEV_SetDisFmt(LX_DE_DIS_FMT_T *pstParams);
int DE_DEV_SetFrRate(LX_DE_FR_RATE_T *pstParams);
int DE_DEV_SetIrePattern(LX_DE_SET_IRE_PATTERN_T *pstParams);
int DE_DEV_SetMultiWin(LX_DE_MULTI_WIN_TYPE_T *pstParams);
int DE_DEV_MultiWinEnable(BOOLEAN *pstParams);
int DE_DEV_GrabPixel(LX_DE_GRAB_PIXEL_T *pstParams);
int DE_DEV_GetMviTimingInfo(LX_DE_MVI_TIMING_INFO_T *pstParams);
int DE_DEV_SetSpread(LX_DE_SET_SPREAD_T *pstParams);
int DE_DEV_SetCviSrcType(LX_DE_CVI_SRC_TYPE_T *pstParams);
int DE_DEV_SetCviCsc(LX_DE_CVI_CSC_T *pstParams);
int DE_DEV_SetPostCsc(LX_DE_POST_CSC_T *pstParams);
int DE_DEV_GetMviColormetry(LX_DE_MVI_COLORIMETRY_T *pstParams);
int DE_DEV_ZList(LX_DE_ZLIST_T *pstParams);
int DE_DEV_SetDeinterlace(BOOLEAN *pstParams);
//int DE_DEV_Set3DSrcMux(LX_DE_PRE3D_SRC_MUX_T *pstParams);
int DE_DEV_SetPre3DInMode(LX_DE_PRE3D_IN_MODE_T *pstParams);
//int DE_DEV_SetPre3DOutMode(LX_DE_PRE3D_OUT_MODE_T *pstParams);
//int DE_DEV_SetPre3DInCH(LX_DE_PRE3D_IN_CH_T *pstParams);
//int DE_DEV_SetPre3DEnable(BOOLEAN *pstParams);
int DE_DEV_SetPwm(LX_DE_PWM_PARAM_T *pstParams);
int DE_DEV_SetCve(LX_DE_CVE_PARAM_T *pstParams);
int DE_DEV_SetVcs(LX_DE_VCS_IPC_T *pstParams);
int DE_DEV_OIF_Init(LX_DE_OIF_INFO_T *pstParams);
int DE_DEV_OIF_SetLvdsOutput(BOOLEAN *pstParams);
int DE_DEV_OIF_SetVesaJeida(LX_DE_OIF_STD_T *pstParams);
int DE_DEV_OIF_SetPanelIf(LX_DE_OIF_PANEL_IF_T *pstParams);
int DE_DEV_OIF_SetPixel(LX_DE_OIF_PIXEL_T *pstParams);
int DE_DEV_OIF_SetBitDepth(LX_DE_OIF_BIT_DEPTH_T *pstParams);
int DE_DEV_OIF_SelectOutPath(LX_DE_OIF_OUTPUT_PATH_T *pstParams);
int DE_DEV_OIF_SelectLvdsPath(LX_DE_OIF_LVDS_PATH_T *pstParams);
int DE_DEV_OIF_SelectLedPath(LX_DE_OIF_LED_IN_PATH_T *pstParams);
int DE_DEV_OIF_SelectPwmPath(LX_DE_OIF_PWM_PATH_T *pstParams);
int DE_DEV_OIF_GetInfo(LX_DE_OIF_INFO_T *pstParams);
int DE_DEV_OIF_GetLvdsIfCtrl(LX_DE_OIF_LVDS_IF_CTRL_T *pstParams);
int DE_DEV_OIF_GetLvdsTxCtrl(LX_DE_OIF_LVDS_TX_CTRL_T *pstParams);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef DE_DEV_INC  ----- */
/**  @} */
