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

#include "de_model.h"
#include "de_kapi.h"
#include "de_def.h"
#include "de_ipc_def.h"

#include "cli/cli_def.h"
#include "cli/cli_arg.h"

#include "dbg/dbg_reg.h"
#include "de/de_dev.h"
#include "de/de_cli.h"
#include "de/de_test.h"

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
//#define CLI2DBI(cmd, id) Add_GLI_Cmd(#cmd, DBI_XDE_CMD_BASE+id, )
//#define CLI2CMD(cmd)     Add_GLI_Cmd(#cmd, lCliCmdId++, cmd)
#define CLI2MCU(cmd, id) Add_GLI_Cmd(#cmd, MCU_XDE_CMD_BASE+id, CLI_DE_MCU)

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
char sFileFW[][100] = {
	"all",
	"/home/bringup/firmware/led_l8_ddr.bin",
	"/home/bringup/firmware/video_asc.bin",
	"/home/bringup/firmware/video_ddr.bin"
};

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
static char saCliSendBuff[VIDEO_MSG_STR_MAX];
static UINT16 coef_YCBCR_Bypass[9] = {		0x1000, 0x0000, 0x0000,		// 1.000, 0.000, 0.000
											0x0000, 0x1000, 0x0000,   	// 0.000, 1.000, 0.000
											0x0000, 0x0000, 0x1000};  	// 0.000, 0.000, 1.000
static UINT16 coef_GBR220toYCbCr709[9] = {	0x0B70, 0x0126, 0x0368,		// 0.715, 0.072, 0.213
											0xF9B3, 0x082D, 0xFE21, 	//-0.394, 0.511,-0.117
											0xF894, 0xFF40, 0x082D};	//-0.464,-0.047, 0.511
static UINT16 coef_GBR256toYCbCr709[9] = {	0x09D2, 0x00FD, 0x02ED,		// 0.614, 0.062, 0.183
											0xFA98, 0x0706, 0xFE63,   	//-0.338, 0.439,-0.101
											0xF99E, 0xFF5D, 0x0706};  	//-0.399,-0.040, 0.439
static UINT16 coef_YCbCr601toGBR256[9] = {	0x129F, 0xF9BF, 0xF2FE,		// 1.164,-0.391,-0.813
											0x129F, 0x2049, 0x0000,  	// 1.164, 2.018, 0.000
											0x129F, 0x0000, 0x1989};  	// 1.164, 0.000, 1.596
static UINT16 coef_YCbCr709toGBR256[9] = {	0x129F, 0xFC98, 0xF775,		// 1.164,-0.213,-0.534
											0x129F, 0x21D7, 0x0000,   	// 1.164, 2.115, 0.000
											0x129F, 0x0000, 0x1CB0};  	// 1.164, 0.000, 1.793
static UINT16 offset_YCbCr_toYCbCr[6]  = {  0xFFC0, 0xFE00, 0xFE00, 0x0040, 0x0200, 0x0200 };
static UINT16 offset_GBR220toYCbCr[6]  = {  0x0000, 0x0000, 0x0000, 0x0000, 0x0200, 0x0200 };
static UINT16 offset_GBR256toYCbCr[6]  = {  0x0000, 0x0000, 0x0000, 0x0040, 0x0200, 0x0200 };
static UINT16 offset_YCbCR_toGBR256[6] = {  0xFFC0, 0xFE00, 0xFE00, 0x0000, 0x0000, 0x0000 };


/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int CLI_VIDEO_Start(void)
{
    unsigned int lCliCmdId = CLI_XDE_CMD_BASE+1;

	CLI2CMD(CLI_DE_RegDebug);
	CLI2CMD(CLI_DE_Test);
	CLI2CMD(CLI_DE_SetDispWin);

	CLI2CMD(CLI_DE_Init);
	CLI2CMD(CLI_DE_GetFirmwareInfo);
	CLI2CMD(CLI_DE_SetBgColor);
	CLI2CMD(CLI_DE_GetOutFrRate);
	CLI2CMD(CLI_DE_SetWinBlank);
	CLI2CMD(CLI_DE_GetInputWin);
	CLI2CMD(CLI_DE_GetOutWin);
	CLI2CMD(CLI_DE_SetInputWin);
	CLI2CMD(CLI_DE_SetOutWin);
	CLI2CMD(CLI_DE_SetFreeze);
	CLI2CMD(CLI_DE_SetInputSrc);
	CLI2CMD(CLI_DE_SetDisOut);
	CLI2CMD(CLI_DE_SetDisFmt);
	CLI2CMD(CLI_DE_SetFrRate);
	CLI2CMD(CLI_DE_SetIrePattern);
	CLI2CMD(CLI_DE_SetMultiWin);
	CLI2CMD(CLI_DE_MultiWinEnable);
	CLI2CMD(CLI_DE_GrabPixel);
	CLI2CMD(CLI_DE_GetMviTimingInfo);
	CLI2CMD(CLI_DE_SetSpread);
	CLI2CMD(CLI_DE_SetCviSrcType);
	CLI2CMD(CLI_DE_SetCviCsc);
	CLI2CMD(CLI_DE_SetPostCsc);
	CLI2CMD(CLI_DE_GetMviColormetry);
	CLI2CMD(CLI_DE_ZList);
	CLI2CMD(CLI_DE_SetDeinterlace);
	//CLI2CMD(CLI_DE_Set3DSrcMux);
	CLI2CMD(CLI_DE_SetPre3DInMode);
	//CLI2CMD(CLI_DE_SetPre3DOutMode);
	//CLI2CMD(CLI_DE_SetPre3DInCH);
	//CLI2CMD(CLI_DE_SetPre3DEnable);
	CLI2CMD(CLI_DE_SetPre3DOprMode);
	CLI2CMD(CLI_DE_SetPwm);
	CLI2CMD(CLI_DE_SetCve);
	CLI2CMD(CLI_DE_SetVcs);
	CLI2CMD(CLI_DE_RegRd);
	CLI2CMD(CLI_DE_RegWr);
	CLI2CMD(CLI_DE_GetCaptureWin);
	CLI2CMD(CLI_DE_SetCaptureWin);
	CLI2CMD(CLI_DE_OIF_Init);
	CLI2CMD(CLI_DE_OIF_SetLvdsOutput);
	CLI2CMD(CLI_DE_OIF_SetVesaJeida);
	CLI2CMD(CLI_DE_OIF_SetPanelIf);
	CLI2CMD(CLI_DE_OIF_SetPixel);
	CLI2CMD(CLI_DE_OIF_SetBitDepth);
	CLI2CMD(CLI_DE_OIF_SelectOutPath);
	CLI2CMD(CLI_DE_OIF_SelectLvdsPath);
	CLI2CMD(CLI_DE_OIF_SelectOutCh);
	CLI2CMD(CLI_DE_OIF_SelectLedPath);
	CLI2CMD(CLI_DE_OIF_SelectPwmPath);
	CLI2CMD(CLI_DE_OIF_GetInfo);
	CLI2CMD(CLI_DE_OIF_GetLvdsIfCtrl);
	CLI2CMD(CLI_DE_OIF_GetLvdsTxCtrl);
	CLI2CMD(CLI_DE_OIF_SetLvdsIfCtrl);
	CLI2CMD(CLI_DE_OIF_SetLvdsTxCtrl);
	CLI2CMD(CLI_DE_FW_Download);

    CLI2MCU(MCU_VIDEO_SetRegister               ,1);
    CLI2MCU(MCU_VIDEO_GetRegister               ,2);
    CLI2MCU(MCU_VIDEO_SetValue                  ,3);
    CLI2MCU(MCU_VIDEO_GetValue                  ,4);
    CLI2MCU(MCU_VIDEO_SetSrc                    ,5);
    CLI2MCU(MCU_VIDEO_InitFrameMemory           ,6);
    CLI2MCU(MCU_VIDEO_PrintCurrentFrame         ,7);
    CLI2MCU(MCU_VIDEO_SetWinOprMode             ,8);
    CLI2MCU(MCU_VIDEO_GetWinOprMode             ,9);
    CLI2MCU(MCU_VIDEO_SetWinPathWithType        ,10);
    CLI2MCU(MCU_VIDEO_GetWinPathWithType        ,11);
    CLI2MCU(MCU_VIDEO_SetFormatWithPath         ,12);
    CLI2MCU(MCU_VIDEO_GetFormatWithPath         ,13);
    CLI2MCU(MCU_VIDEO_SetOswMode                ,14);
    CLI2MCU(MCU_VIDEO_GetOswMode                ,15);
#ifdef USE_TEMPERAL_DEFINITION
	CLI2MCU(MCU_VIDEO_SetCviSyncWithType        ,16);
	CLI2MCU(MCU_VIDEO_GetCviSyncWithType        ,17);
#endif
    CLI2MCU(MCU_VIDEO_SetMode3DF                ,18);
    CLI2MCU(MCU_VIDEO_GetMode3DF                ,19);
    CLI2MCU(MCU_VIDEO_SetFrameOfBlock           ,20);
    CLI2MCU(MCU_VIDEO_GetFrameOfBlock           ,21);
    CLI2MCU(MCU_VIDEO_SetModeOfECanvas          ,22);
    CLI2MCU(MCU_VIDEO_GetModeOfECanvas          ,23);
    CLI2MCU(MCU_VIDEO_SetSyncParamOfDisplay     ,24);
    CLI2MCU(MCU_VIDEO_GetSyncParamOfDisplay     ,25);
    CLI2MCU(MCU_VIDEO_SetSyncOfDisplay          ,26);
    CLI2MCU(MCU_VIDEO_SetSyncParamOfVCR         ,27);
    CLI2MCU(MCU_VIDEO_GetSyncParamOfVCR         ,28);
    CLI2MCU(MCU_VIDEO_SetSyncOfVCR              ,29);
    CLI2MCU(MCU_VIDEO_SetSyncParamOfPIP         ,30);
    CLI2MCU(MCU_VIDEO_GetSyncParamOfPIP         ,31);
    CLI2MCU(MCU_VIDEO_SetSyncOfPIP              ,32);
    CLI2MCU(MCU_VIDEO_SetFormatOfImageForDlr    ,33);
    CLI2MCU(MCU_VIDEO_SetModeOfProcessingForDlr ,34);
    CLI2MCU(MCU_VIDEO_SetSizeOfImageForDlr      ,35);
    CLI2MCU(MCU_VIDEO_SetSyncPos                ,36);
    CLI2MCU(MCU_VIDEO_SetSizeOfSsrcPath         ,37);
    CLI2MCU(MCU_VIDEO_SetSsrcCoefByPath         ,38);
    CLI2MCU(MCU_VIDEO_SetSsrcPhaseByPath        ,39);
    CLI2MCU(MCU_VIDEO_SetSsrcFmtByPath          ,40);
	CLI2MCU(MCU_VIDEO_SetSyncMuxByPath			,41);
	CLI2MCU(MCU_VIDEO_SetMviSyncPath			,42);
	CLI2MCU(MCU_VIDEO_PrintDiag			        ,43);
	CLI2MCU(MCU_VIDEO_DiagnosticsInit			,44);
	CLI2MCU(MCU_VIDEO_SetModeIPC				,45);
	CLI2MCU(MCU_VIDEO_GetModeIPC				,46);
	CLI2MCU(MCU_VIDEO_SetInfoOfWCP				,47);
	CLI2MCU(MCU_VIDEO_SetPattern				,48);
#ifdef USE_XTENSA
	CLI2MCU(MCU_VIDEO_SetUartTimeOut			,49);
	CLI2MCU(MCU_VIDEO_SetInterrupt				,50);
	CLI2MCU(MCU_VIDEO_UpdateCoef				,51);
#endif
	CLI2MCU(MCU_VIDEO_SetFrameManualMode		,52);
	CLI2MCU(MCU_VIDEO_SetScenario		        ,53);
	CLI2MCU(MCU_VIDEO_SetDebugManualMode	    ,54);
	CLI2MCU(MCU_VIDEO_SetIndexOfFrame			,55);
	CLI2MCU(MCU_VIDEO_SetTNROn					,56);
	CLI2MCU(MCU_VIDEO_SetPathOfMDI				,57);
	CLI2MCU(MCU_VIDEO_PrintAddrOfBlock			,58);
	CLI2MCU(MCU_VIDEO_SetStepOfWCP				,59);
	CLI2MCU(MCU_VIDEO_SetStepMode				,60);
	CLI2MCU(MCU_VIDEO_SetDump2DDR				,61);
	CLI2MCU(MCU_VIDEO_SetTraceAddrOfBlock		,62);
	CLI2MCU(MCU_VIDEO_SetBitsOfMSCR				,63);
	CLI2MCU(MCU_VIDEO_SetCscBypassOfCVI			,64);
	CLI2MCU(MCU_VIDEO_SetAutoOffsetOfCVI		,65);
#ifdef USE_TEMPERAL_DEFINITION
	CLI2MCU(MCU_VIDEO_SetSampleWithPath		    ,66);
#endif
	CLI2MCU(MCU_VIDEO_SetCSampleOfCVI			,67);
	CLI2MCU(MCU_VIDEO_SetCSample				,68);
	CLI2MCU(MCU_VIDEO_SetCSampleOfWin			,69);
	CLI2MCU(MCU_VIDEO_SetBypassCAP				,70);
	CLI2MCU(MCU_VIDEO_SetFrameSeqOfTNR			,71);
	CLI2MCU(MCU_VIDEO_SetDefinitionOfMSCR		,72);
	CLI2MCU(MCU_VIDEO_SetIrqShowOn				,73);
	CLI2MCU(MCU_VIDEO_SetTrigOSD				,74);
	CLI2MCU(MCU_VIDEO_FillOSDBitmap				,75);
	CLI2MCU(MCU_VIDEO_FillOSDColor				,76);
	CLI2MCU(MCU_VIDEO_SetNumberOfFrame			,77);
	CLI2MCU(MCU_VIDEO_GetNumberOfFrame			,78);
	CLI2MCU(MCU_VIDEO_PrintVdec					,79);
	CLI2MCU(MCU_VIDEO_SetInverserFieldFromMVI	,80);
	CLI2MCU(MCU_VIDEO_SetPrintTNR				,81);
	CLI2MCU(MCU_VIDEO_SetTraceAddrOfOPR			,82);
	CLI2MCU(MCU_VIDEO_SetSyncModeOfVDEC			,83);
	CLI2MCU(MCU_VIDEO_SetSceneOf3DF				,84);
	CLI2MCU(MCU_VIDEO_SetAspecOfDisplay			,85);
	CLI2MCU(MCU_VIDEO_SetZoomIn					,86);
	CLI2MCU(MCU_VIDEO_SetZoomOut				,87);
	CLI2MCU(MCU_VIDEO_SetTraceOfSSCR			,88);
	CLI2MCU(MCU_VIDEO_PrintDvr					,89);
	CLI2MCU(MCU_VIDEO_SetTraceWinSize			,90);
	CLI2MCU(MCU_VIDEO_SetTraceOfInitStart		,91);
	CLI2MCU(MCU_VIDEO_SetWinInfoOfPIP			,92);
	CLI2MCU(MCU_VIDEO_Set10BitsByOPR			,93);
	CLI2MCU(MCU_VIDEO_SetBankOfMIF				,94);
	CLI2MCU(MCU_VIDEO_SetWaitingDoneOfGFX		,95);
	CLI2MCU(MCU_VIDEO_SetFormatOf3DF			,96);
	CLI2MCU(MCU_VIDEO_SetPrintOf3DF				,97);
	CLI2MCU(MCU_VIDEO_SetPrintOfIPC				,98);
	CLI2MCU(MCU_VIDEO_SetPrintToCPU			    ,99);
	CLI2MCU(MCU_VIDEO_IPC4CPU			        ,100);
	CLI2MCU(MVRD	                            ,101);
	CLI2MCU(MVWR	                            ,102);
	CLI2MCU(MVWD	                            ,103);
	CLI2MCU(MVBW	                            ,104);
	CLI2MCU(MVRC	                            ,105);

	return RET_OK;
}

int DE_CLI_RD(UINT32 addr)
{
	UINT32 stParams;

	stParams = addr;

	DE_DEV_RegRd( &stParams );
	//ffprintf("Reg[0x%08x] = 0x%08x\n", addr, stParams);

	return stParams;
}

int DE_CLI_WD(UINT32 addr, UINT32 data)
{
	LX_DE_REG_WR_T stParams;

	stParams.wr_addr = addr;
	stParams.wr_data = data;

	return DE_DEV_RegWr( &stParams );
}

int CLI_DE_MCU(int argc ,char **argv)
{
	int ret = RET_ERROR;
	LX_DE_IPC_CLI_T stParams;
	int i;

	do {
		if (!argc) break;
		strcpy(saCliSendBuff, argv[0]);
		for (i=1;i<argc;i++) sprintf(saCliSendBuff, "%s %s", saCliSendBuff, argv[i]);
		stParams.cnt  = strlen(saCliSendBuff);
		stParams.pStr = saCliSendBuff;
		ret = DE_DEV_MCU( &stParams);
	} while (0);

	return ret;
}

int CLI_DE_RegRd(int argc ,char **argv)
{
	int ret = RET_OK;
	UINT32	nArg = START_ARGS;
	UINT32 addr;
	UINT32 data;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <addr>\n", argv[0]);
			break;
		}
		addr = A2U;
		data = DE_CLI_RD(addr);
		ffprintf("Reg ([0x%0X] = 0x%08x)\n", addr, data );
	} while (0);

	return ret;
}

int CLI_DE_RegWr(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_REG_WR_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <addr data>\n", argv[0]);
			break;
		}
		stParams.wr_addr = A2U;
		stParams.wr_data = A2U;
		ret = DE_DEV_RegWr( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_GetCaptureWin(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_CVI_CAPTURE_WIN_T stParams;

	do {
		if (argc != (nArg) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s\n", argv[0]);
			break;
		}
		ret = DE_DEV_GetCaptureWin( &stParams );
		ffprintf("get input cvi wid = %d whxy(%d,%d,%d,%d)\n", stParams.cvi_channel, stParams.size_offset.hsize, stParams.size_offset.vsize, stParams.size_offset.hoffset, stParams.size_offset.voffset);
	} while (0);

	return ret;
}

int CLI_DE_SetCaptureWin(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_CVI_CAPTURE_WIN_T stParams;

	do {
		if (argc != (nArg+5) || !strcmp(argv[1], "-help")) {
			printf("%s <cvi_channel hsize vsize hoffset voffset>\n", argv[0]);
			break;
		}
		stParams.cvi_channel		 = A2U;
		stParams.size_offset.hsize	 = A2U;
		stParams.size_offset.vsize	 = A2U;
		stParams.size_offset.hoffset = A2U;
		stParams.size_offset.voffset = A2U;
		ret = DE_DEV_SetCaptureWin( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_Init(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_PANEL_TYPE_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <panel_type>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DE_DEV_Init( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_GetFirmwareInfo(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_FIRMWARE_INFO_T rev_info;

	do {
		if (argc != (nArg) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s\n", argv[0]);
		}
		ret = DE_DEV_GetFirmwareInfo( &rev_info );
		ffprintf("de firmware info (%0X, %d:%d:%d)\n", rev_info.version, rev_info.date.year, rev_info.date.month, rev_info.date.day );
	} while (0);

	return ret;
}

int CLI_DE_SetBgColor(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_COLOR_T stParams;

	do {
		if (argc != (nArg+3) || !strcmp(argv[1], "-help")) {
			printf("%s <r g b>\n", argv[0]);
			break;
		}
		stParams.g = A2U;
		stParams.b = A2U;
		stParams.r = A2U;
		ret = DE_DEV_SetBgColor( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_GetOutFrRate(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_FR_RATE_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <isForceFreeRun fr_rate>\n", argv[0]);
			break;
		}
		stParams.isForceFreeRun = A2U;
		stParams.fr_rate		= A2U;
		ret = DE_DEV_GetOutFrRate( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetWinBlank(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_SET_WIN_BLANK_T stParams;

	do {
		if (argc != (nArg+5) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id bEnable r g b>\n", argv[0]);
			break;
		}
		stParams.win_id		 = A2U;
		stParams.bEnable	 = A2U;
		stParams.win_color.g = A2U;
		stParams.win_color.b = A2U;
		stParams.win_color.r = A2U;
		ret = DE_DEV_SetWinBlank( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_GetInputWin(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_WIN_DIMENSION_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id>\n", argv[0]);
			break;
		}
		stParams.win_id = A2U;
		ret = DE_DEV_GetInputWin( &stParams );

		ffprintf("get input window wid = %d whxy(%d,%d,%d,%d)\n", stParams.win_id, stParams.rect.w, stParams.rect.h, stParams.rect.x, stParams.rect.y);
	} while (0);

	return ret;
}

int CLI_DE_GetOutWin(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_WIN_DIMENSION_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id>\n", argv[0]);
			break;
		}
		stParams.win_id = A2U;
		ret = DE_DEV_GetOutWin( &stParams );
		ffprintf("get output window wid = %d whxy(%d,%d,%d,%d)\n", stParams.win_id, stParams.rect.w, stParams.rect.h, stParams.rect.x, stParams.rect.y);
	} while (0);

	return ret;
}

int CLI_DE_SetInputWin(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_WIN_DIMENSION_T stParams;

	do {
		if (argc != (nArg+5) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id w h x y>\n", argv[0]);
			break;
		}
		stParams.win_id = A2U;
		stParams.rect.w = A2U;
		stParams.rect.h = A2U;
		stParams.rect.x = A2U;
		stParams.rect.y = A2U;
		ret = DE_DEV_SetInputWin( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetOutWin(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_WIN_DIMENSION_T stParams;

	do {
		if (argc != (nArg+5) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id w h x y>\n", argv[0]);
			break;
		}
		stParams.win_id = A2U;
		stParams.rect.w = A2U;
		stParams.rect.h = A2U;
		stParams.rect.x = A2U;
		stParams.rect.y = A2U;
		ret = DE_DEV_SetOutWin( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetFreeze(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_SET_FREEZE_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id bEnable>\n", argv[0]);
			break;
		}
		stParams.win_id  = A2U;
		stParams.bEnable = A2U;
		ret = DE_DEV_SetFreeze( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetInputSrc(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_SET_INPUT_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id bEnable>\n", argv[0]);
			break;
		}
		stParams.win_id    = A2U;
		stParams.inputSrc  = A2U;
		ret = DE_DEV_SetInputSrc( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetDisOut(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DE_DEV_SetDisOut( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetDisFmt(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_DIS_FMT_T stParams;

	do {
		if (argc != (nArg+4) || !strcmp(argv[1], "-help")) {
			printf("%s <panel_type fr_rate h_total v_total>\n", argv[0]);
			break;
		}
		stParams.panel_type = A2U;
		stParams.fr_rate	= A2U;
		stParams.h_total    = A2U;
		stParams.v_total	= A2U;
		ret = DE_DEV_SetDisFmt( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetFrRate(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_FR_RATE_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <isForceFreeRun fr_rate>\n", argv[0]);
			break;
		}
		stParams.isForceFreeRun = A2U;
		stParams.fr_rate		= A2U;
		ret = DE_DEV_SetFrRate( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetIrePattern(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_SET_IRE_PATTERN_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <ire_pattern level>\n", argv[0]);
			break;
		}
		stParams.ire_pattern = A2U;
		stParams.level		 = A2U;
		ret = DE_DEV_SetIrePattern( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetMultiWin(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_MULTI_WIN_TYPE_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <MultiWinYype>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DE_DEV_SetMultiWin( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_MultiWinEnable(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DE_DEV_MultiWinEnable( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_GrabPixel(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_GRAB_PIXEL_T stParams;

	do {
		if (argc != (nArg+10) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id pixelGrabW pixelGrabH pixelGrabX pixelGrabY colorSpace colorDepth realPixelGrabW realPixelGrabH>\n", argv[0]);
			break;
		}
		stParams.region.win_id		   = A2U;
		stParams.region.pixelGrabW	   = A2U;
		stParams.region.pixelGrabH	   = A2U;
		stParams.region.pixelGrabX	   = A2U;
		stParams.region.pixelGrabY	   = A2U;
		stParams.region.colorSpace	   = A2U;
		stParams.region.colorDepth	   = A2U;
		stParams.region.realPixelGrabW = A2U;
		stParams.region.realPixelGrabH = A2U;

		ret = DE_DEV_GrabPixel( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_GetMviTimingInfo(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_MVI_TIMING_INFO_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id>\n", argv[0]);
			break;
		}
		stParams.win_id	= A2U;
		ret = DE_DEV_GetMviTimingInfo( &stParams );
		ffprintf("GetMviTimingInfo.\n         v_freq %d, h_start %d, v_start %d, h_size %d, v_size %d, isProg %d \n", stParams.mpeg_time.v_freq, stParams.mpeg_time.h_start, stParams.mpeg_time.v_start, stParams.mpeg_time.h_size, stParams.mpeg_time.v_size, stParams.mpeg_time.isProg);
	} while (0);

	return ret;
}

int CLI_DE_SetSpread(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_SET_SPREAD_T stParams;

	do {
		if (argc != (nArg+3) || !strcmp(argv[1], "-help")) {
			printf("%s <period dutyratio bEnable>\n", argv[0]);
			break;
		}
		stParams.period    = A2U;
		stParams.dutyratio = A2U;
		stParams.bEnable   = A2U;

		ret = DE_DEV_SetSpread( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetCviSrcType(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_CVI_SRC_TYPE_T stParams;

	do {
		if (argc != (nArg+13) || !strcmp(argv[1], "-help")) {
			printf("%s <cvi_channel hsize vsize hoffset voffset color aspect scan bit_res c_sample de_mode sampling sc_c_sample>\n", argv[0]);
			break;
		}
		stParams.cvi_channel		 = A2U;
		stParams.size_offset.hsize	 = A2U;
		stParams.size_offset.vsize	 = A2U;
		stParams.size_offset.hoffset = A2U;
		stParams.size_offset.voffset = A2U;
		stParams.color				 = A2U;
		stParams.aspect				 = A2U;
		stParams.scan				 = A2U;
		stParams.bit_res			 = A2U;
		stParams.c_sample			 = A2U;
		stParams.de_mode			 = A2U;
		stParams.sampling			 = A2U;
		stParams.sc_c_sample		 = A2U;

		ret = DE_DEV_SetCviSrcType( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetCviCsc(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_CVI_CSC_T stParams;
	LX_DE_COLOR_STD_T csc;
	int i;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <cvi_channel colorFormat>\n", argv[0]);
			break;
		}
		stParams.cvi_channel = A2U;
		csc                  = A2U;

		switch(csc)
		{
			case LX_DE_COLOR_601:
			case LX_DE_COLOR_709:
			case LX_DE_COLOR_XVYCC601:
			case LX_DE_COLOR_XVYCC709:
				{
					stParams.isEnable = FALSE;
					for(i=0;i<ARRAY_SIZE(coef_YCBCR_Bypass);i++)
						stParams.CSC_matrix[i] = coef_YCBCR_Bypass[i];

					for(i=0;i<ARRAY_SIZE(offset_YCbCr_toYCbCr);i++)
						stParams.CSC_offset[i] = offset_YCbCr_toYCbCr[i];
				}
				break;

			case LX_DE_COLOR_RGB:
				{
					stParams.isEnable = TRUE;

					for(i=0;i<ARRAY_SIZE(coef_GBR220toYCbCr709);i++)
						stParams.CSC_matrix[i] = coef_GBR220toYCbCr709[i];

					for(i=0;i<ARRAY_SIZE(offset_GBR220toYCbCr);i++)
						stParams.CSC_offset[i] = offset_GBR220toYCbCr[i];
				}
				break;

			case LX_DE_COLOR_RGBEX:
				{
					stParams.isEnable = TRUE;

					for(i=0;i<ARRAY_SIZE(coef_GBR256toYCbCr709);i++)
						stParams.CSC_matrix[i] = coef_GBR256toYCbCr709[i];

					for(i=0;i<ARRAY_SIZE(offset_GBR256toYCbCr);i++)
						stParams.CSC_offset[i] = offset_GBR256toYCbCr[i];
				}
				break;

			default:
				break;
		}
		ret = DE_DEV_SetCviCsc( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetPostCsc(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_POST_CSC_T stParams;
	LX_DE_COLOR_STD_T csc;
	int i;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <colorFormat>\n", argv[0]);
			break;
		}

		csc = A2U;

		switch(csc)
		{
			case LX_DE_COLOR_601:
				{
					for(i=0;i<ARRAY_SIZE(coef_YCbCr601toGBR256);i++)
						stParams.CSC_matrix[i] = coef_YCbCr601toGBR256[i];
				}
				break;

			case LX_DE_COLOR_709:
				{
					for(i=0;i<ARRAY_SIZE(coef_YCbCr709toGBR256);i++)
						stParams.CSC_matrix[i] = coef_YCbCr709toGBR256[i];
				}
				break;

			case LX_DE_COLOR_XVYCC601:
				{
					for(i=0;i<ARRAY_SIZE(coef_YCbCr601toGBR256);i++)
						stParams.CSC_matrix[i] = coef_YCbCr601toGBR256[i];	// TODO: use xvYCC601toGBR256
				}
				break;

			case LX_DE_COLOR_XVYCC709:
				{
					for(i=0;i<ARRAY_SIZE(coef_YCbCr709toGBR256);i++)
						stParams.CSC_matrix[i] = coef_YCbCr709toGBR256[i]; // TODO: use xvYCC709toGBR256
				}
				break;

			case LX_DE_COLOR_RGB:
			case LX_DE_COLOR_RGBEX:
				{
					for(i=0;i<ARRAY_SIZE(coef_YCbCr709toGBR256);i++)
						stParams.CSC_matrix[i] = coef_YCbCr709toGBR256[i];
				}
				break;

			default:
				break;
		}

		for(i=0;i<ARRAY_SIZE(offset_YCbCR_toGBR256);i++)
			stParams.CSC_offset[i] = offset_YCbCR_toGBR256[i];

		ret = DE_DEV_SetPostCsc( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_GetMviColormetry(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_MVI_COLORIMETRY_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <win_id>\n", argv[0]);
			break;
		}
		stParams.win_id		= A2U;
		ret = DE_DEV_GetMviColormetry( &stParams );
		ffprintf("colorimetry [ %d ]\n", stParams.colorimetry);
	} while (0);

	return ret;
}

int CLI_DE_ZList(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_ZLIST_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <surface_id enable>\n", argv[0]);
			break;
		}
		stParams.surface_id	= A2U;
		stParams.enable		= A2U;

		ret = DE_DEV_ZList( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetDeinterlace(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <enable>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_SetDeinterlace( &stParams );
	} while (0);

	return ret;
}

//int CLI_DE_Set3DSrcMux(int argc ,char **argv)
//{
//    int	ret = RET_ERROR;
//    UINT32	nArg = START_ARGS;
//    LX_DE_PRE3D_SRC_MUX_T stParams;

//    do {
//        if (argc != (nArg+4) || !strcmp(argv[1], "-help")) {
//            printf("%s <isOn ch in_mode out_mode>\n", argv[0]);
//            break;
//        }
//        stParams.isOn	  = A2U;
//        stParams.ch		  = A2U;
//        stParams.in_mode  = A2U;
//        stParams.out_mode = A2U;

//        ret = DE_DEV_Set3DSrcMux( &stParams );
//    } while (0);

//    return ret;
//}

int CLI_DE_SetPre3DInMode(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_PRE3D_IN_MODE_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <in_mode>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_SetPre3DInMode( &stParams );
	} while (0);

	return ret;
}

//int CLI_DE_SetPre3DOutMode(int argc ,char **argv)
//{
//    int	ret = RET_ERROR;
//    UINT32	nArg = START_ARGS;
//    LX_DE_PRE3D_OUT_MODE_T stParams;

//    do {
//        if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
//            printf("%s <out_mode>\n", argv[0]);
//            break;
//        }
//        stParams = A2U;

//        ret = DE_DEV_SetPre3DOutMode( &stParams );
//    } while (0);

//    return ret;
//}

//int CLI_DE_SetPre3DInCH(int argc ,char **argv)
//{
//    int	ret = RET_ERROR;
//    UINT32	nArg = START_ARGS;
//    LX_DE_PRE3D_IN_CH_T stParams;

//    do {
//        if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
//            printf("%s <ch>\n", argv[0]);
//            break;
//        }
//        stParams = A2U;

//        ret = DE_DEV_SetPre3DInCH( &stParams );
//    } while (0);

//    return ret;
//}

//int CLI_DE_SetPre3DEnable(int argc ,char **argv)
//{
//    int	ret = RET_ERROR;
//    UINT32	nArg = START_ARGS;
//    BOOLEAN stParams;

//    do {
//        if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
//            printf("%s <bEnable>\n", argv[0]);
//            break;
//        }
//        stParams = A2U;

//        ret = DE_DEV_SetPre3DEnable( &stParams );
//    } while (0);

//    return ret;
//}

int CLI_DE_SetPre3DOprMode(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_3D_OPR_MODE_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <pre3d_opr_mode>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_SetPre3DOprMode( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetPwm(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_PWM_PARAM_T stParams;

	do {
		if (argc != (nArg+3) || !strcmp(argv[1], "-help")) {
			printf("%s <port duty offset>\n", argv[0]);
			break;
		}
		stParams.port	= A2U;
		stParams.duty	= A2U;
		stParams.offset = A2U;

		ret = DE_DEV_SetPwm( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetCve(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_CVE_PARAM_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <fb std>\n", argv[0]);
			break;
		}
		stParams.fb  = A2U;
		stParams.std = A2U;

		ret = DE_DEV_SetCve( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_SetVcs(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_VCS_IPC_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <inx data>\n", argv[0]);
			break;
		}
		stParams.inx  = A2U;
		stParams.data = A2U;

		ret = DE_DEV_SetVcs( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_Init(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_INFO_T stParams;

	do {
		if (argc != (nArg+9) || !strcmp(argv[1], "-help")) {
			printf("%s <port duty offset>\n", argv[0]);
			break;
		}
		stParams.eVesaJeida = A2U;
		stParams.eLvdsMini	= A2U;
		stParams.ePixel		= A2U;
		stParams.eBitMode	= A2U;
		stParams.eOutVideo	= A2U;
		stParams.eOutLVDS	= A2U;
		stParams.eOutCH		= A2U;
		stParams.eLedIn		= A2U;
		stParams.ePWM		= A2U;

		ret = DE_DEV_OIF_Init( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SetLvdsOutput(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SetLvdsOutput( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SetVesaJeida(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_STD_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bVesaJeida>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SetVesaJeida( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SetPanelIf(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_PANEL_IF_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bMinLvds>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SetPanelIf( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SetPixel(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_PIXEL_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <pixelType>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SetPixel( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SetBitDepth(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_BIT_DEPTH_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bitDepth>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SetBitDepth( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SelectOutPath(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_OUTPUT_PATH_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <outputPath>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SelectOutPath( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SelectLvdsPath(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_LVDS_PATH_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <lvdsPath>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SelectLvdsPath( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SelectOutCh(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_OUTPUT_CH_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <OutCh>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SelectOutCh( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SelectLedPath(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_LED_IN_PATH_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <ledPath>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SelectLedPath( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SelectPwmPath(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_PWM_PATH_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <pwmPath>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = DE_DEV_OIF_SelectPwmPath( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_GetInfo(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_INFO_T stParams;

	do {
		if (argc != (nArg) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s\n", argv[0]);
			break;
		}

		ret = DE_DEV_OIF_GetInfo( &stParams );

		ffprintf("GetInfo.\n         eVesaJeida %d, eLvdsMini %d, ePixel %d, eBitMode %d, eOutVideo %d, eOutLVDS %d, eOutCH 0x%x, eLedIn %d, ePWM %d\n", stParams.eVesaJeida, stParams.eLvdsMini, stParams.ePixel, stParams.eBitMode, stParams.eOutVideo, stParams.eOutLVDS, stParams.eOutCH, stParams.eLedIn, stParams.ePWM);
		ret = RET_OK;
	} while (0);

	return ret;
}

int CLI_DE_OIF_GetLvdsIfCtrl(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_LVDS_IF_CTRL_T stParams;

	do {
		if (argc != (nArg) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s\n", argv[0]);
			break;
		}

		ret = DE_DEV_OIF_GetLvdsIfCtrl( &stParams );
		ffprintf("LvdsIf.\n emptyData %d, bitMode %d, lsbfirst %d, syncOrder %d, ch3LVDS %d, ch2LVDS %d, ch1LVDS %d, ch0LVDS %d, outSel %d\n", stParams.emptyData, stParams.bitMode, stParams.lsbfirst, stParams.syncOrder, stParams.ch3LVDS, stParams.ch2LVDS, stParams.ch1LVDS, stParams.ch0LVDS, stParams.outSel);
	} while (0);

	return ret;
}

int CLI_DE_OIF_GetLvdsTxCtrl(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_LVDS_TX_CTRL_T stParams;

	do {
		if (argc != (nArg) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s\n", argv[0]);
			break;
		}

		ret = DE_DEV_OIF_GetLvdsTxCtrl( &stParams );
		ffprintf("LvdsTx.\n pdb %d, rf %d, rs 0x%x, empCtrl %d, miniLVDS %d, miniFlip %d, swingLVDS %d, ovDrv %d, lvdsEn 0x%08x\n", stParams.pdb, stParams.rf, stParams.rs, stParams.empCtrl, stParams.miniLVDS, stParams.miniFlip, stParams.swingLVDS, stParams.ovDrv, stParams.lvdsEn);
	} while (0);

	return ret;
}

int CLI_DE_OIF_SetLvdsIfCtrl(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_LVDS_IF_CTRL_T stParams;

	do {
		if (argc != (nArg+9) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s <emptyData bitMode lsbfirst syncOrder ch3LVDS ch2LVDS ch1LVDS ch0LVDS outSel>\n", argv[0]);
			break;
		}

		stParams.emptyData = A2U;
		stParams.bitMode   = A2U;
		stParams.lsbfirst  = A2U;
		stParams.syncOrder = A2U;
		stParams.ch3LVDS   = A2U;
		stParams.ch2LVDS   = A2U;
		stParams.ch1LVDS   = A2U;
		stParams.ch0LVDS   = A2U;
		stParams.outSel	   = A2U;
		ret = DE_DEV_OIF_GetLvdsIfCtrl( &stParams );
	} while (0);

	return ret;
}

int CLI_DE_OIF_SetLvdsTxCtrl(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DE_OIF_LVDS_TX_CTRL_T stParams;

	do {
		if (argc != (nArg+9) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s <pdb rf rs empCtrl miniLVDS miniFlip swingLVDS ovDrv lvdsEn>\n", argv[0]);
			break;
		}

		stParams.pdb	   = A2U;
		stParams.rf		   = A2U;
		stParams.rs		   = A2U;
		stParams.empCtrl   = A2U;
		stParams.miniLVDS  = A2U;
		stParams.miniFlip  = A2U;
		stParams.swingLVDS = A2U;
		stParams.ovDrv	   = A2U;
		stParams.lvdsEn    = A2U;
		ret = DE_DEV_OIF_GetLvdsTxCtrl( &stParams );
	} while (0);

	return ret;
}

#ifdef USE_DE_FIRMWARE_DWONLOAD_IN_DRIVER
int CLI_DE_FW_Download(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	int inx;

	do {
		if (argc != (nArg+1) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s <inx>\n", argv[0]);
			break;
		}
		inx = A2U;
		if (inx >= ARRAY_SIZE(sFileFW)) break;
		ret = CL_DE_FW_Download(inx, sFileFW[inx]);
	} while (0);

	return ret;
}

int CL_DE_FW_Download(int inx, char *pFile)
{
	int ret = RET_ERROR;
	LX_DE_FW_DWLD_T stParams;
	char rFile[100];
	FILE *rpFP = NULL;
	char *pData = NULL;
	UINT32 size;

	do {
		stParams.inx = inx;
		if (inx) {
			CHECK_NULL(pFile);
			strcpy(rFile, pFile);

			rpFP = fopen(rFile, "rb");
			if (!rpFP) {
				ffprintf("%s file open error\n" , (char *)rFile);
				ret = RET_ERROR;
				break;
			}

			fseek ( rpFP, 0, SEEK_END );
			size = ftell (rpFP);
			fseek ( rpFP, 0, SEEK_SET );

			pData = malloc ( size );
			CHECK_NULL(pData);
			fread(pData, 1, size, rpFP );

			stParams.size  = size;
			stParams.pData = pData;
		}
		ret = DE_DEV_FW_Download( &stParams );
	} while (0);

	if (pData) free(pData);
	if (rpFP) fclose(rpFP);

	return ret;
}
#endif
/**  @} */
