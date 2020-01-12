
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
#include <string.h>
#include <errno.h>

#include "base_types.h"
#include "de_def.h"
#include "de_ipc_def.h"
#include "cli/cli_arg.h"
#include "cli/cli_main.h"
#include "cli/cli_xdp.h"
#include "de/de_xdp.h"

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
//static BOOLEAN sbXdpDebugRunning = FALSE;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/

int CLI_XDP_Start(void)
{
	Add_CLI_Cmd("PA"    ,CLI_XDP_DebugOn);
	Add_CLI_Cmd("PR"    ,CLI_XDP_ReadRegister);
	Add_CLI_Cmd("PW"    ,CLI_XDP_WriteRegister);

    return RET_OK;
}

int CLI_XDP_DebugOn(int argc, char **argv)
{
	UINT32 nArg = START_ARGS;

	if (argc != 3) {
//        ffprintf("pa set_id on/off\n");
		return RET_ERROR;
	}

	return CL_XDP_DebugOn(uA2S(0), uA2S(1));
}

int CL_XDP_DebugOn(char *pSetId, char *pOn)
{
	int ret = RET_OK;

	do {
		if (strcasecmp(pSetId, XDP_SET_ID)) {
//            ffprintf("Not Proper setId : %s\n", pSetId);
			ret = RET_ERROR;
			break;
		}

		if      (!strcasecmp(pOn,  "ON")) CL_XDP_SetStatus(1);
		else if (!strcasecmp(pOn, "OFF")) CL_XDP_SetStatus(0);
	} while (0);

	return ret;
}

int CLI_XDP_ReadRegister(int argc, char **argv)
{
	UINT32 nArg = START_ARGS;

	if (argc != 3) {
//        ffprintf("pr set_id address\n");
		return RET_ERROR;
	}

	//if ( !sbXdpDebugRunning )
	if ( !CL_XDP_GetStatus() )
	{
//        ffprintf("XDP is not On\n");
        return RET_ERROR;
    }

	return CL_XDP_ReadRegister(uA2S(0), uA2H(1));
}

int CL_XDP_ReadRegister(char *pSetId, UINT32 addr)
{
	int ret = RET_OK;
    UINT32 value;
	UINT32 recvAddr;

	do {
		if (strcasecmp(pSetId, XDP_SET_ID)) {
//            ffprintf("Not Proper pSetId : %s\n", pSetId);
			break;
		}

		recvAddr = addr;
		if (recvAddr <= 0xffff) recvAddr += VIDEO_REG_BASE;
		if ( (recvAddr < VIDEO_REG_BASE) || (recvAddr >= (VIDEO_REG_BASE + VIDEO_REG_SIZE)) ) break;

		value = MemoryRead(recvAddr);
		if (value == 0xDEADCAFE) break;
		printf("r %s OK %08xx\n", pSetId, value);
		ret = RET_OK;
	} while (0);

    return ret;
}

int CLI_XDP_WriteRegister(int argc, char **argv)
{
	UINT32 nArg = START_ARGS;

	if (argc != 4) {
//        ffprintf("pw set_id address data\n");
		return RET_ERROR;
	}

	//if ( !sbXdpDebugRunning )
	if ( !CL_XDP_GetStatus() )
	{
//        ffprintf("XDP is not On\n");
        return RET_ERROR;
    }

	return CL_XDP_WriteRegister(uA2S(0), uA2H(1), uA2H(2));
}

int CL_XDP_WriteRegister(char *pSetId, UINT32 addr, UINT32 value)
{
	int ret = RET_ERROR;
	UINT32 recvAddr;

	do {
		if (strcasecmp(pSetId, XDP_SET_ID)) {
//            ffprintf("Not Proper pSetId : %s\n", pSetId);
			break;
		}

		recvAddr = addr;
		if (recvAddr <= 0xffff) recvAddr += VIDEO_REG_BASE;
		if ( (recvAddr < VIDEO_REG_BASE) || (recvAddr >= (VIDEO_REG_BASE + VIDEO_REG_SIZE)) ) break;

		ret = MemoryWrite(recvAddr, value);
		if (ret) break;
		printf("w %s OK %04x %08xx\n", pSetId, addr, value);
		ret = RET_OK;
	} while (0);

    return ret;
}

/**  @} */
