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
 * date       2010.04.21
 * note       Additional information.
 *
 * @addtogroup lg1150_dimm
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

#include "dimm_kapi.h"
#include "dimm_dev.h"
#include "dimm_cli.h"
#include "panellcd_lg1150_47iop.h"

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
int CLI_DIMM_Start(void)
{
    unsigned int lCliCmdId = CLI_DIMM_CMD_BASE+1;

	CLI2CMD(CLI_DIMM_Init);
	CLI2CMD(CLI_DIMM_LocaldimmingControl);
	CLI2CMD(CLI_DIMM_StoreDemo);
	CLI2CMD(CLI_DIMM_LineDemo);
	CLI2CMD(CLI_DIMM_ControlCompensation);
	CLI2CMD(CLI_DIMM_LutControl);
	CLI2CMD(CLI_DIMM_StoreControl);
	CLI2CMD(CLI_DIMM_RegRd);
	CLI2CMD(CLI_DIMM_RegWr);
}

int CLI_DIMM_Init(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DIMM_INFO_T stParams;

	do {
		if (argc != (nArg+4) || !strcmp(argv[1], "-help")) {
			printf("%s <panel_type bPCenable u32ScanTh u32SpiCmd>\n", argv[0]);
			break;
		}
		stParams.blType    = A2U;
		stParams.bPCenable = A2U;
		stParams.u32ScanTh = A2U;
		stParams.u32SpiCmd = A2U;
		stParams.InitData  = gLocalDimming_47Iop_AAL;
		stParams.u32length = ARRAY_SIZE(gLocalDimming_47Iop_AAL);
		ret = DIMM_DEV_Init( &stParams );
	} while (0);

	return ret;
}

int CLI_DIMM_LocaldimmingControl(int argc ,char **argv)
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
		ret = DIMM_DEV_LocaldimmingControl( &stParams );
	} while (0);

	return ret;
}

int CLI_DIMM_StoreDemo(int argc ,char **argv)
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
		ret = DIMM_DEV_StoreDemo( &stParams );
	} while (0);

	return ret;
}

int CLI_DIMM_LineDemo(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DIMM_LINE_CTRL_T stParams;

	do {
		if (argc != (nArg+4) || !strcmp(argv[1], "-help")) {
			printf("%s <bEnable ePatEn u8Vppf u8Hppf>\n", argv[0]);
			break;
		}
		stParams.bEnable = A2U;
		stParams.ePatEn  = A2U;
		stParams.u8Vppf  = A2U;
		stParams.u8Hppf  = A2U;
		ret = DIMM_DEV_LineDemo( &stParams );
	} while (0);

	return ret;
}

int CLI_DIMM_ControlCompensation(int argc ,char **argv)
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
		ret = DIMM_DEV_ControlCompensation( &stParams );
	} while (0);

	return ret;
}

int CLI_DIMM_LutControl(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DIMM_LUT_CTRL_T stParams;
	UINT32 whichLut;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <whichLut(0 - 2)>\n", argv[0]);
			break;
		}
		whichLut = A2U;
		if (whichLut > 2) {
			printf("%s <whichLut(0 - 2)>\n", argv[0]);
			break;
		}
		stParams.pLut	= gLocalDimming_47Iop_OFF[whichLut];
		stParams.u32Num = 289;
		ret = DIMM_DEV_LutControl( &stParams );
	} while (0);

	return ret;
}

int CLI_DIMM_StoreControl(int argc ,char **argv)
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
		ret = DIMM_DEV_StoreControl( &stParams );
	} while (0);

	return ret;
}

int CLI_DIMM_RegRd(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	UINT32 stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <addr>\n", argv[0]);
			break;
		}
		stParams = A2U;
ffprintf("aaaaa addr = 0x%08x\n", (UINT32)stParams);
		ret = DIMM_DEV_RegRd( &stParams );
ffprintf("aaaaa data = 0x%08x\n", (UINT32)stParams);
	} while (0);

	return ret;
}

int CLI_DIMM_RegWr(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_DIMM_REG_WRITE_T stParams;

	do {
		if (argc != (nArg+2) || !strcmp(argv[1], "-help")) {
			printf("%s <addr data>\n", argv[0]);
			break;
		}
		stParams.wr_addr = A2U;
		stParams.wr_data = A2U;
ffprintf("aaaaa stParams.wr_addr = 0x%08x\n", (UINT32)stParams.wr_addr);
ffprintf("aaaaa stParams.wr_data = 0x%08x\n", (UINT32)stParams.wr_data);
		ret = DIMM_DEV_RegWr( &stParams );
	} while (0);

	return ret;
}

/**  @} */
