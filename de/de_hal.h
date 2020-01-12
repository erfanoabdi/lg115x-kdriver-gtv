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
 * date       2010.03.08
 * note       Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */

#ifndef  DE_HAL_INC
#define  DE_HAL_INC

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
int DE_HAL_Init(LX_DE_PANEL_TYPE_T *pstParams);
int DE_HAL_SetFrRate(LX_DE_FR_RATE_T *pstParams);
int DE_HAL_GetFrRate(LX_DE_FR_RATE_T *pstParams);
int DE_HAL_SetBgColor(LX_DE_COLOR_T *pBackColor);
int DE_HAL_SetWinBlank(LX_DE_SET_WIN_BLANK_T *pWinBlank);
int DE_HAL_GrabPixel(LX_DE_GRAB_PIXEL_T *pstParams);
int DE_HAL_GetCviCsc(LX_DE_CVI_CSC_T *pstParams);
int DE_HAL_SetCviCsc(LX_DE_CVI_CSC_T *pstParams);
int DE_HAL_GetPostCsc(LX_DE_POST_CSC_T *pstParams);
int DE_HAL_SetPostCsc(LX_DE_POST_CSC_T *pstParams);
int DE_HAL_GetMviColorimetry(LX_DE_MVI_COLORIMETRY_T *pstParams);
int DE_HAL_SetSpread(LX_DE_SET_SPREAD_T *pstParams);
int DE_HAL_SetDeinterlace(BOOLEAN bEnable);
int DE_HAL_SetInputSrc(LX_DE_SET_INPUT_T *pstParams);
int DE_HAL_SetCviSrcType(LX_DE_CVI_SRC_TYPE_T *pstParams);
int DE_HAL_SetUart4MCU(BOOLEAN bUart4MCU);

int DE_HAL_OIF_Init(LX_DE_OIF_INFO_T *pstParams);
int DE_HAL_OIF_GetInfo(LX_DE_OIF_INFO_T *pstParams);
int DE_HAL_OIF_SetLvdsOutput(BOOLEAN *pstParams);
int DE_HAL_OIF_SetLvdsPower(BOOLEAN *pstParams);
int DE_HAL_OIF_SetDarkScreen(BOOLEAN *pstParams);
int DE_HAL_OIF_SetVesaJeida(LX_DE_OIF_STD_T *pstParams);
int DE_HAL_OIF_SetPanelIf(LX_DE_OIF_PANEL_IF_T *pstParams);
int DE_HAL_OIF_SetPixel(LX_DE_OIF_PIXEL_T *pstParams);
int DE_HAL_OIF_SetBitDepth(LX_DE_OIF_BIT_DEPTH_T *pstParams);
int DE_HAL_OIF_SelectOutPath(LX_DE_OIF_OUTPUT_PATH_T *pstParams);
int DE_HAL_OIF_SelectLvdsPath(LX_DE_OIF_LVDS_PATH_T *pstParams);
int DE_HAL_OIF_SelectOutCh(LX_DE_OIF_OUTPUT_CH_T *pstParams);
int DE_HAL_OIF_SelectLedPath(LX_DE_OIF_LED_IN_PATH_T *pstParams);
int DE_HAL_OIF_SelectPwmPath(LX_DE_OIF_PWM_PATH_T *pstParams);
int DE_HAL_OIF_SelectLrPath(LX_DE_OIF_LR_PATH_T *pstParams);
int DE_HAL_OIF_SetLvdsOutCtrl(LX_DE_OIF_LVDS_OUT_CTRL_T *pstParams);
int DE_HAL_OIF_SetLvdsPortSwap(BOOLEAN *pstParams);
int DE_HAL_OIF_SetLvdsPolarity(LX_DE_OIF_LVDS_POLARITY_T *pstParams);

int DE_HAL_SetTable(LX_DE_SET_TABLE_T *pstParams);

#ifdef USE_DE_FIRMWARE_DWONLOAD_IN_DRIVER
int DE_HAL_FW_Download(LX_DE_FW_DWLD_T *pstParams);
#endif

int DE_HAL_SetCviFir(LX_DE_CVI_FIR_T *pstParams);
int DE_HAL_SetCviTpg(LX_DE_CVI_TPG_T *pstParams);
int DE_HAL_SetCviCsampleMode(LX_DE_CSAMPLE_MODE_T *pstParams);
int DE_HAL_SetCviSyncReshape(LX_DE_CVI_SYNC_RESHAPE_T *pstParams);
int DE_HAL_SetPeTestPatternGen(LX_DE_PE_TPG_T *pstParams);
int DE_HAL_SetPeBbdCtrl(LX_DE_PE_BBD_CTRL_T *pstParams);
int DE_HAL_GetPeBbdStatus(LX_DE_PE_COORDINATE_T *pstParams);
int DE_HAL_SetEdgeCrop(BOOLEAN *pstParams);
int DE_HAL_SetVcs(LX_DE_VCS_IPC_T *pstParams);

int DE_HAL_ResetDE(BOOLEAN bReset);
int DE_HAL_InitPHY2VIRT(void);
int DE_HAL_FreePHY2VIRT(void);
int DE_HAL_GPIO_Init(void);
int DE_HAL_HDMI_Init(void);
int DE_HAL_LVDS_Init(void);
int DE_HAL_MISC_Init(void);
int DE_HAL_OSD_Init(void);
int DE_HAL_FW_DownloadFromBin(void);
int DE_HAL_FW_DownloadFromHead(void);
int DE_HAL_IPCisAlive(void);

int DE_HAL_WriteReg(UINT32 addr, UINT32 value);
int DE_HAL_ReadReg(UINT32 addr, UINT32 *value);
int DE_HAL_SetPwmControl(LX_DE_PWM_CTRL_T *pstParams);
int DE_HAL_SetPwmDutyCycle(LX_DE_PWM_PARAM_T *pstParams);
int DE_HAL_SetPwmFrequency(LX_DE_PWM_FREQ_T *pstParams);
int DE_HAL_SetPwmPulseWidth(LX_DE_PWM_PULSE_T *pstParams);
int DE_HAL_GetPwmDutyExternal(LX_DE_EXTERNAL_PWM_T *pstParams);
int DE_HAL_SetCviVideoFrameBuffer(LX_DE_CVI_RW_VIDEO_FRAME_T *pstParams);
int DE_HAL_SelectMultiWinSrc(LX_DE_MULTI_WIN_SRC_T *pstParams);
int DE_HAL_SetSCARTRGBBypassMode(int *pEnable);
int DE_HAL_GetCviInfo(LX_DE_CVI_INFO_T *pstParams);
int DE_HAL_Set3DFullMode(LX_DE_3D_FULL_MODE_T *pstParams);
int DE_HAL_SetUdMode(BOOLEAN *pstParams);
int DE_HAL_GetDebug(LX_DE_SET_DBG_T *pstParams);
int DE_HAL_SetDebug(LX_DE_SET_DBG_T *pstParams);

#endif   /* ----- #ifndef DE_HAL_INC  ----- */
/**  @} */
