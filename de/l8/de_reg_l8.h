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
 * date       2010.03.31
 * note       Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */
#ifndef  DE_REG_INC
#define  DE_REG_INC

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

int DE_REG_InitPHY2VIRT(void);
int DE_REG_FreePHY2VIRT(void);
int VIDEO_IPC_WakeUpReg(VIDEO_IPC_TYPE_T ipcType, BOOLEAN turnOn);
int VIDEO_IPC_GetStatusReg(VIDEO_IPC_TYPE_T ipcType, UINT32 *pStatus);
int VIDEO_WDG_WakeUpReg(BOOLEAN turnOn);
int VIDEO_DMA_WakeUpReg(BOOLEAN turnOn);
UINT32 VIDEO_REG_RD(UINT32 addr);
int VIDEO_REG_WD(UINT32 addr, UINT32 value);
int DE_REG_SetBgColor(LX_DE_COLOR_T *pBackColor);
int DE_REG_SetWinBlank(LX_DE_SET_WIN_BLANK_T *pWinBlank);
int DE_REG_GrabPixel(LX_DE_GRAB_PIXEL_T *pstParams);

BOOLEAN DE_REG_CheckIrq4Vsync(void);
int DE_REG_UpdateVideoIrqStatus(VIDEO_INTR_TYPE_T intrType, UINT32 *pVideoIrqStatus);
int DE_REG_Init(LX_DE_PANEL_TYPE_T *pstParams);
int DE_REG_GetCviCsc(LX_DE_CVI_CSC_T *pstParams);
int DE_REG_SetCviCsc(LX_DE_CVI_CSC_T *pstParams);
int DE_REG_GetPostCsc(LX_DE_POST_CSC_T *pstParams);
int DE_REG_SetPostCsc(LX_DE_POST_CSC_T *pstParams);
int DE_REG_GetMviColorimetry(LX_DE_MVI_COLORIMETRY_T *pstParams);
int DE_REG_SetSpread(LX_DE_SET_SPREAD_T *pstParams);
int DE_REG_SetFrRate(DE_DPLL_CLK_T dclk);
int DE_REG_GetFrRate(LX_DE_FR_RATE_T *pstParams);
int DE_REG_SetDeinterlace(BOOLEAN bEnable);
int DE_REG_SetInputSrc(LX_DE_SET_INPUT_T *pstParams);
int DE_REG_SetCviSrcType(LX_DE_CVI_SRC_TYPE_T *pstParams);

int DE_REG_OIF_Init(LX_DE_OIF_INFO_T *pstParams);
int DE_REG_OIF_GetInfo(LX_DE_OIF_INFO_T *pstParams);
int DE_REG_OIF_SelectOutCh(LX_DE_OIF_OUTPUT_CH_T *pstParams);
int DE_REG_OIF_SetLvdsOutput(BOOLEAN *pstParams);
int DE_REG_OIF_SetLvdsPower(BOOLEAN *pstParams);
int DE_REG_OIF_SetDarkScreen(BOOLEAN *pstParams);
int DE_REG_OIF_SetVesaJeida(LX_DE_OIF_STD_T *pstParams);
int DE_REG_OIF_SetPanelIf(LX_DE_OIF_PANEL_IF_T *pstParams);
int DE_REG_OIF_SetPixel(LX_DE_OIF_PIXEL_T *pstParams);
int DE_REG_OIF_SetBitDepth(LX_DE_OIF_BIT_DEPTH_T *pstParams);
int DE_REG_OIF_SelectOutPath(LX_DE_OIF_OUTPUT_PATH_T *pstParams);
int DE_REG_OIF_SelectLvdsPath(LX_DE_OIF_LVDS_PATH_T *pstParams);
int DE_REG_OIF_SelectLedPath(LX_DE_OIF_LED_IN_PATH_T *pstParams);
int DE_REG_OIF_SelectPwmPath(LX_DE_OIF_PWM_PATH_T *pstParams);
int DE_REG_OIF_SelectLrPath(LX_DE_OIF_LR_PATH_T *pstParams);
int DE_REG_OIF_GetLvdsIfCtrl(LX_DE_OIF_LVDS_IF_CTRL_T *pstParams);
int DE_REG_OIF_GetLvdsTxCtrl(LX_DE_OIF_LVDS_TX_CTRL_T *pstParams);
int DE_REG_OIF_SetLvdsIfCtrl(LX_DE_OIF_LVDS_IF_CTRL_T *pstParams);
int DE_REG_OIF_SetLvdsTxCtrl(LX_DE_OIF_LVDS_TX_CTRL_T *pstParams);

int DE_REG_SetTable(LX_DE_SET_TABLE_T *pstParams);

int DE_REG_GPIO_Init(void);
int DE_REG_HDMI_Init(void);
int DE_REG_LVDS_Init(void);
int DE_REG_MISC_Init(void);
int DE_REG_Uart0_Switch(int whichUart);
int DE_REG_OIF_InitLvdsIfCtrl(void);

#ifdef USE_DE_FIRMWARE_DWONLOAD_IN_DRIVER
int DE_REG_FW_Download(LX_DE_FW_DWLD_T *pstParams);
#endif

int DE_REG_SetCviFir(LX_DE_CVI_FIR_T *pstParams);
int DE_REG_SetCviTpg(LX_DE_CVI_TPG_T *pstParams);
int DE_REG_SetCviCsampleMode(LX_DE_CSAMPLE_MODE_T *pstParams);
int DE_REG_SetCviSyncReshape(LX_DE_CVI_SYNC_RESHAPE_T *pstParams);
int DE_REG_SetPeTestPatternGen(LX_DE_PE_TPG_T *pstParams);
int DE_REG_SetPeBbdCtrl(LX_DE_PE_BBD_CTRL_T *pstParams);
int DE_REG_GetPeBbdStatus(LX_DE_PE_COORDINATE_T *pstParams);
int DE_REG_SetEdgeCrop(BOOLEAN *pstParams);
int DE_REG_SetVcs(LX_DE_VCS_IPC_T *pstParams);
int DE_REG_ResetDE(BOOLEAN bReset);
BOOLEAN VIDEO_REG_IPCisAlive(void);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif
/**  @} */
