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

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#include "base_types.h"

#include "de_kapi.h"
#include "de_def.h"

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

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
static UINT32 de_fd = 0;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int DE_DEV_Open(void)
{
	de_fd = open( "/dev/lg/de0", O_RDONLY|O_RDWR );

	return RET_OK;
}

int DE_DEV_Close(void)
{
 	close(de_fd);

	return RET_OK;
}

int DE_DEV_MCU(LX_DE_IPC_CLI_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_IPC_CLI, pstParams);
}

int DE_DEV_RegRd(UINT32 *pstParams)
{
	return ioctl( de_fd, DE_IOR_REG_RD, pstParams );

}

int DE_DEV_RegWr(LX_DE_REG_WR_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_REG_WR, pstParams );

}

int DE_DEV_GetCaptureWin(LX_DE_REG_WR_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_GET_CAPTURE_WIN, pstParams );

}

int DE_DEV_SetCaptureWin(LX_DE_REG_WR_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_CAPTURE_WIN, pstParams );

}

int DE_DEV_Init(LX_DE_PANEL_TYPE_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_DE_INIT, pstParams );
}

int DE_DEV_GetFirmwareInfo(LX_DE_FIRMWARE_INFO_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_GET_FIRMWARE_INFO, pstParams );
}

int DE_DEV_SetBgColor(LX_DE_COLOR_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_BG_COLOR, pstParams );
}

int DE_DEV_GetOutFrRate(LX_DE_FR_RATE_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_GET_OUT_FR_RATE, pstParams );
}

int DE_DEV_SetWinBlank(LX_DE_SET_WIN_BLANK_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_WIN_BLANK, pstParams );
}

int DE_DEV_GetInputWin(LX_DE_WIN_DIMENSION_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_GET_INPUT_WIN, pstParams );
}

int DE_DEV_GetOutWin(LX_DE_WIN_DIMENSION_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_GET_OUT_WIN, pstParams );
}

int DE_DEV_SetInputWin(LX_DE_WIN_DIMENSION_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_INPUT_WIN, pstParams );
}

int DE_DEV_SetOutWin(LX_DE_WIN_DIMENSION_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_OUT_WIN, pstParams );
}

int DE_DEV_SetFreeze(LX_DE_SET_FREEZE_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_FREEZE, pstParams );
}

int DE_DEV_SetInputSrc(LX_DE_SET_INPUT_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_INPUT_SRC, pstParams );
}

int DE_DEV_SetDisOut(BOOLEAN *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_DIS_OUT, pstParams );
}

int DE_DEV_SetDisFmt(LX_DE_DIS_FMT_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_DIS_FMT, pstParams );
}

int DE_DEV_SetFrRate(LX_DE_FR_RATE_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_FR_RATE, pstParams );
}

int DE_DEV_SetIrePattern(LX_DE_SET_IRE_PATTERN_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_IRE_PATTERN, pstParams );
}

int DE_DEV_SetMultiWin(LX_DE_MULTI_WIN_TYPE_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_MULTI_WIN, pstParams );
}

int DE_DEV_MultiWinEnable(BOOLEAN *pstParams)
{
	return ioctl( de_fd, DE_IOW_MULTI_WIN_ENABLE, pstParams );
}

int DE_DEV_GrabPixel(LX_DE_GRAB_PIXEL_T *pstParams)
{
	return ioctl( de_fd, DE_IOWR_GRAB_PIXEL, pstParams );
}

int DE_DEV_GetMviTimingInfo(LX_DE_MVI_TIMING_INFO_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_GET_MVI_TIMING_INFO, pstParams );
}

int DE_DEV_SetSpread(LX_DE_SET_SPREAD_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_SPREAD, pstParams );
}

int DE_DEV_SetCviSrcType(LX_DE_CVI_SRC_TYPE_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_CVI_SRC_TYPE, pstParams );
}

int DE_DEV_SetCviCsc(LX_DE_CVI_CSC_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_CVI_CSC, pstParams );
}

int DE_DEV_SetPostCsc(LX_DE_POST_CSC_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_POST_CSC, pstParams );
}

int DE_DEV_GetMviColormetry(LX_DE_MVI_COLORIMETRY_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_GET_MVI_COLORIMETRY, pstParams );
}

int DE_DEV_ZList(LX_DE_ZLIST_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_ZLIST, pstParams );
}

int DE_DEV_SetDeinterlace(BOOLEAN *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_DEINTERLACE, pstParams );
}

int DE_DEV_SetUart4MCU(BOOLEAN *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_UART_FOR_MCU, pstParams );
}

//int DE_DEV_Set3DSrcMux(LX_DE_PRE3D_SRC_MUX_T *pstParams)
//{
//    return ioctl( de_fd, DE_IOW_SET_3D_SRC_MUX, pstParams );
//}

int DE_DEV_SetPre3DInMode(LX_DE_PRE3D_IN_MODE_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_PRE3D_IN_MODE, pstParams );
}

//int DE_DEV_SetPre3DOutMode(LX_DE_PRE3D_OUT_MODE_T *pstParams)
//{
//    return ioctl( de_fd, DE_IOW_SET_PRE3D_OUT_MODE, pstParams );
//}

//int DE_DEV_SetPre3DInCH(LX_DE_PRE3D_IN_CH_T *pstParams)
//{
//    return ioctl( de_fd, DE_IOW_SET_PRE3D_IN_CH, pstParams );
//}

//int DE_DEV_SetPre3DEnable(BOOLEAN *pstParams)
//{
//    return ioctl( de_fd, DE_IOW_SET_PRE3D_ENABLE, pstParams );
//}

int DE_DEV_SetPre3DOprMode(LX_DE_3D_OPR_MODE_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_3D_OPR_MODE, pstParams );
}

int DE_DEV_SetPwm(LX_DE_PWM_PARAM_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_PWM, pstParams );
}

int DE_DEV_SetCve(LX_DE_CVE_PARAM_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_CVE, pstParams );
}

int DE_DEV_SetVcs(LX_DE_VCS_IPC_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_SET_VCS, pstParams );
}

int DE_DEV_OIF_Init(LX_DE_OIF_INFO_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_INIT, pstParams );
}

int DE_DEV_OIF_SetLvdsOutput(BOOLEAN *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SET_LVDS_OUTPUT, pstParams );
}

int DE_DEV_OIF_SetVesaJeida(LX_DE_OIF_STD_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SET_VESA_JEIDA, pstParams );
}

int DE_DEV_OIF_SetPanelIf(LX_DE_OIF_PANEL_IF_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SET_PANEL_IF, pstParams );
}

int DE_DEV_OIF_SetPixel(LX_DE_OIF_PIXEL_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SET_PIXEL, pstParams );
}

int DE_DEV_OIF_SetBitDepth(LX_DE_OIF_BIT_DEPTH_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SET_BIT_DEPTH, pstParams );
}

int DE_DEV_OIF_SelectOutPath(LX_DE_OIF_OUTPUT_PATH_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SELECT_OUT_PATH, pstParams );
}

int DE_DEV_OIF_SelectLvdsPath(LX_DE_OIF_LVDS_PATH_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SELECT_LVDS_PATH, pstParams );
}

int DE_DEV_OIF_SelectOutCh(LX_DE_OIF_OUTPUT_CH_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SELECT_OUT_CH, pstParams );
}

int DE_DEV_OIF_SelectLedPath(LX_DE_OIF_LED_IN_PATH_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SELECT_LED_PATH, pstParams );
}

int DE_DEV_OIF_SelectPwmPath(LX_DE_OIF_PWM_PATH_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_OIF_SELECT_PWM_PATH, pstParams );
}

int DE_DEV_OIF_GetInfo(LX_DE_OIF_INFO_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_OIF_GET_INFO, pstParams );
}

int DE_DEV_OIF_GetLvdsIfCtrl(LX_DE_OIF_LVDS_IF_CTRL_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_OIF_GET_LVDS_IF_CTRL, pstParams );
}

int DE_DEV_OIF_GetLvdsTxCtrl(LX_DE_OIF_LVDS_TX_CTRL_T *pstParams)
{
	return ioctl( de_fd, DE_IOR_OIF_GET_LVDS_TX_CTRL, pstParams );
}

int DE_DEV_FW_Download(LX_DE_FW_DWLD_T *pstParams)
{
	return ioctl( de_fd, DE_IOW_FW_DOWNLOAD, pstParams );
}

#ifdef USE_DE_SWITCH_UART
int DE_DEV_SetPwmControl(void)
{
	LX_DE_PWM_CTRL_T stParams;
ffprintf("aaaaa\n");
	return ioctl( de_fd, DE_IOW_SET_PWM_CONTROL, &stParams );
}
#endif

/**  @} */
