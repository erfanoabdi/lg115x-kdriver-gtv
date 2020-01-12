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
 * @addtogroup lg1150_fmt3d
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

#include "fmt3d_kapi.h"
#include "fmt3d_dev.h"
#include "fmt3d_cli.h"

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
int CLI_FMT3D_Start(void)
{
    unsigned int lCliCmdId = CLI_FMT3D_CMD_BASE+1;

	CLI2CMD(CLI_FMT3D_Init);
	CLI2CMD(CLI_FMT3D_SetConvergence);

}

int CLI_FMT3D_Init(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;

	do {
		if (argc != (nArg) || ((argc > 1) && !strcmp(argv[1], "-help")) ) {
			printf("%s\n", argv[0]);
			break;
		}
		ret = FMT3D_DEV_Init();
	} while (0);

	return ret;
}

int CLI_FMT3D_SetConvergence(int argc ,char **argv)
{
	int	ret = RET_ERROR;
	UINT32	nArg = START_ARGS;
	UINT16 stParams;

	do {
		if (argc != (nArg+1) || !strcmp(argv[1], "-help")) {
			printf("%s <convergence>\n", argv[0]);
			break;
		}
		stParams = A2U;

		ret = FMT3D_DEV_SetConvergence( &stParams );
	} while (0);

	return ret;
}

/**  @} */
