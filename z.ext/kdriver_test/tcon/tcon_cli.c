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
 * @addtogroup lg1150_tcon
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

#include "tcon_kapi.h"
#include "tcon_dev.h"
#include "tcon_cli.h"
#include "tcon_firmware.h"

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
int CLI_TCON_Start(void)
{
    unsigned int lCliCmdId = CLI_TCON_CMD_BASE+1;

	CLI2CMD(CLI_TCON_Init);
	CLI2CMD(CLI_TCON_SetOdcTable);
	CLI2CMD(CLI_TCON_SetDga);
	CLI2CMD(CLI_TCON_Control);
}

int CLI_TCON_Init(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_TCON_INIT_INFO_T  stParams;

	do {
		if (argc != (nArg+2) || ((argc > 1) && !strcmp(argv[1], "-help"))) {
			printf("%s <bGIP bV6>\n", argv[0], stParams.bGIP, stParams.bV6);
			break;
		}
		stParams.bGIP = A2U;
		stParams.bV6  = A2U;
		ret = TCON_DEV_Init( &stParams );
	} while (0);

	return ret;
}

int CLI_TCON_SetOdcTable(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_TCON_ODC_TABLE_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s bOdcbypass\n", argv[0]);
			break;
		}
		stParams.pODTbl     = &TCON_FIRMWARE_42_GIP[0x400/4];
		stParams.u32length	= 256;
		stParams.bOdcbypass = A2U;
		ret = TCON_DEV_SetOdcTable( &stParams );
	} while (0);

	return ret;
}

int CLI_TCON_SetDga(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_TCON_DGA_LUT_T stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s bDgaEn\n", argv[0]);
			break;
		}
		stParams.pDgaLUT   = &TCON_FIRMWARE_42_GIP[0x800/4];
		stParams.u32length = 384 * 3;
		stParams.bDgaEn	   = A2U;
		ret = TCON_DEV_SetDga( &stParams );
	} while (0);

	return ret;
}

int CLI_TCON_Control(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	LX_TCON_CTRL_T stParams;

	do {
		if (argc != (nArg+5) || !strcmp(argv[1], "-help")) {
			printf("%s bOdcbypass u8LtdCtrl bStuEn bDitherEn bDgaEn\n", argv[0]);
			break;
		}
		stParams.bOdcbypass = A2U;
		stParams.u8LtdCtrl	= A2U;
		stParams.bStuEn		= A2U;
		stParams.bDitherEn	= A2U;
		stParams.bDgaEn		= A2U;
		ret = TCON_DEV_Control( &stParams );
	} while (0);

	return ret;
}

/**  @} */
