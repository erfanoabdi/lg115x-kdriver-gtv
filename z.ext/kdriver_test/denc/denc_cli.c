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
 * @addtogroup lg1150_denc
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

#include "../de/de_def.h"
#include "../de/de_ipc_def.h"
#include "cli/cli_def.h"
#include "cli/cli_arg.h"

#include "denc_kapi.h"
#include "denc_dev.h"
#include "denc_cli.h"

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

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int CLI_DENC_Start(void)
{
    unsigned int lCliCmdId = CLI_DENC_CMD_BASE+1;

    CLI2CMD(CLI_DENC_Init);
    CLI2CMD(CLI_DENC_OnOff);
    CLI2CMD(CLI_DENC_SetOutPutVideoStd);
    CLI2CMD(CLI_DENC_SetSource);
    CLI2CMD(CLI_DENC_TtxEnable);
    CLI2CMD(CLI_DENC_WssEnable);
    CLI2CMD(CLI_DENC_VpsEnable);
    CLI2CMD(CLI_DENC_SetTtxData);
    CLI2CMD(CLI_DENC_SetWssData);
    CLI2CMD(CLI_DENC_SetVpsData);
    CLI2CMD(CLI_DENC_ColorBarEnable);
}

int CLI_DENC_Init(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;

	do {
		if (argc != (nArg) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s\n", argv[0]);
			break;
		}
		ret = DENC_DEV_Init();
	} while (0);
}

int CLI_DENC_OnOff(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DENC_DEV_OnOff( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_SetOutPutVideoStd(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DENC_VIDEO_SYSTEM_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <videoSystem>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DENC_DEV_SetOutPutVideoStd( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_SetSource(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DENC_VIDEO_SOURCE_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <videoSource>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DENC_DEV_SetSource( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_TtxEnable(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DENC_DEV_TtxEnable( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_WssEnable(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DENC_DEV_WssEnable( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_VpsEnable(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DENC_DEV_VpsEnable( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_SetTtxData(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DENC_TTX_DATA_T stParams;
	UINT8 packData[45];
	int i;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <Line_Number\n", argv[0]);
			break;
		}
		stParams.Line_Number   = A2U;
		stParams.Size          = ARRAY_SIZE(packData);
		for (i=0;i<ARRAY_SIZE(packData);i++) packData[i] = i;
		stParams.pPacket_Addr  = packData;
		ret = DENC_DEV_SetTtxData( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_SetWssData(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DENC_WSS_DATA_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <Enable WSS_DATA>\n", argv[0]);
			break;
		}
		stParams.Enable   = A2U;
		stParams.WSS_DATA = A2U;
		ret = DENC_DEV_SetWssData( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_SetVpsData(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DENC_VPS_DATA_T stParams;
	UINT8 vpsData[15];
	int i;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <Enable>\n", argv[0]);
			break;
		}
		stParams.Enable    = A2U;
		for (i=0;i<15;i++) vpsData[i] = i;
		stParams.pVPS_DATA = vpsData;
		ret = DENC_DEV_SetVpsData( &stParams );
	} while (0);

	return ret;
}

int CLI_DENC_ColorBarEnable(int argc ,char **argv)
{
	int ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	BOOLEAN stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <Enable>\n", argv[0]);
			break;
		}
		stParams = A2U;
		ret = DENC_DEV_ColorBarEnable( &stParams );
	} while (0);

	return ret;
}

/**  @} */
