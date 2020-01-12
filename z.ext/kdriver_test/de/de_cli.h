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
 * date       2010.02.27
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
int DE_CLI_RD(UINT32 addr);
int DE_CLI_WD(UINT32 addr, UINT32 data);
int CLI_DE_MCU(int argc ,char **argv);

int CLI_DE_Init(int argc ,char **argv);
int CLI_DE_GetFirmwareInfo(int argc ,char **argv);
int CLI_DE_SetBgColor(int argc ,char **argv);
int CLI_DE_GetOutFrRate(int argc ,char **argv);
int CLI_DE_SetWinBlank(int argc ,char **argv);
int CLI_DE_GetInputWin(int argc ,char **argv);
int CLI_DE_GetOutWin(int argc ,char **argv);
int CLI_DE_SetInputWin(int argc ,char **argv);
int CLI_DE_SetOutWin(int argc ,char **argv);
int CLI_DE_SetFreeze(int argc ,char **argv);
int CLI_DE_SetInputSrc(int argc ,char **argv);
int CLI_DE_SetDisOut(int argc ,char **argv);
int CLI_DE_SetDisFmt(int argc ,char **argv);
int CLI_DE_SetFrRate(int argc ,char **argv);
int CLI_DE_SetIrePattern(int argc ,char **argv);
int CLI_DE_SetMultiWin(int argc ,char **argv);
int CLI_DE_MultiWinEnable(int argc ,char **argv);
int CLI_DE_GrabPixel(int argc ,char **argv);
int CLI_DE_GetMviTimingInfo(int argc ,char **argv);
int CLI_DE_SetSpread(int argc ,char **argv);
int CLI_DE_SetCviSrcType(int argc ,char **argv);
int CLI_DE_SetCviCsc(int argc ,char **argv);
int CLI_DE_SetPostCsc(int argc ,char **argv);
int CLI_DE_GetMviColormetry(int argc ,char **argv);
int CLI_DE_ZList(int argc ,char **argv);
int CLI_DE_SetDeinterlace(int argc ,char **argv);
//int CLI_DE_Set3DSrcMux(int argc ,char **argv);
int CLI_DE_SetPre3DInMode(int argc ,char **argv);
//int CLI_DE_SetPre3DOutMode(int argc ,char **argv);
//int CLI_DE_SetPre3DInCH(int argc ,char **argv);
//int CLI_DE_SetPre3DEnable(int argc ,char **argv);
int CLI_DE_SetPre3DOprMode(int argc ,char **argv);
int CLI_DE_SetPwm(int argc ,char **argv);
int CLI_DE_SetCve(int argc ,char **argv);
int CLI_DE_SetVcs(int argc ,char **argv);
int CLI_DE_RegRd(int argc ,char **argv);
int CLI_DE_RegWr(int argc ,char **argv);
int CLI_DE_GetCaptureWin(int argc ,char **argv);
int CLI_DE_SetCaptureWin(int argc ,char **argv);

int CLI_DE_OIF_Init(int argc ,char **argv);
int CLI_DE_OIF_SetLvdsOutput(int argc ,char **argv);
int CLI_DE_OIF_SetVesaJeida(int argc ,char **argv);
int CLI_DE_OIF_SetPanelIf(int argc ,char **argv);
int CLI_DE_OIF_SetPixel(int argc ,char **argv);
int CLI_DE_OIF_SetBitDepth(int argc ,char **argv);
int CLI_DE_OIF_SelectOutPath(int argc ,char **argv);
int CLI_DE_OIF_SelectLvdsPath(int argc ,char **argv);
int CLI_DE_OIF_SelectOutCh(int argc ,char **argv);
int CLI_DE_OIF_SelectLedPath(int argc ,char **argv);
int CLI_DE_OIF_SelectPwmPath(int argc ,char **argv);
int CLI_DE_OIF_GetInfo(int argc ,char **argv);
int CLI_DE_OIF_GetLvdsIfCtrl(int argc ,char **argv);
int CLI_DE_OIF_GetLvdsTxCtrl(int argc ,char **argv);
int CLI_DE_OIF_SetLvdsIfCtrl(int argc ,char **argv);
int CLI_DE_OIF_SetLvdsTxCtrl(int argc ,char **argv);

#ifdef USE_DE_FIRMWARE_DWONLOAD_IN_DRIVER
int CLI_DE_FW_Download(int argc ,char **argv);
int CL_DE_FW_Download(int inx, char *pFile);
#endif
/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

