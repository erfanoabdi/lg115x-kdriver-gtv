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

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "base_types.h"

//#include "de_def.h"
//#include "de_ipc_def.h"
#include "gpio_cfg.h"
#include "gpio_kapi.h"
#include "gpio/gpio_cli_def.h"
#include "cli/cli_arg.h"
//#include "de/de_cli.h"
//#include "dbg/dbg_reg.h"

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
int DDI_GPIO_Ioctl(unsigned int cmd, UINT32 i_uiArg)
{
	int iResult;
	int iFd;

	iFd = open("/dev/lg/gpio0", O_RDWR);
	if( iFd == -1)
	{
		printf("ERROR(%s)- Could not open device: GPIO\n", __FUNCTION__);
		return RET_ERROR;
	}
	iResult = ioctl(iFd, cmd, i_uiArg);
	close(iFd);

	return RET_OK;
}

int CLI_GPIO_READ(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Pin;
	LX_Gpio_Pinset_Param_t  gpio_param;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <Pin>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Pin = A2U;
	gpio_param.u8Pinnumber = (unsigned char)ui32Pin;

	ret_val = DDI_GPIO_Ioctl(LXDRV_GPIO_READ, (UINT32)&gpio_param); 	
//	ret_val = DDI_GPIO_Ioctl(0, (UINT32)&gpio_param); 	
	printf("Pin Read data = 0x%x\n", gpio_param.u8Flag);

	return ret_val;
}

int CLI_GPIO_PULL_HIGHLOW(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Pin;
	UINT32	ui32Flag;
	LX_Gpio_Pinset_Param_t  gpio_param;
	int ret_val = RET_ERROR;

	if (argc < 3 || !strcmp(argv[1], "-help"))
	{
		printf("%s <Pin> <Flag>\n", argv[0]);
		return RET_ERROR;
	}
	ui32Pin = A2U;
	ui32Flag = A2U;

	gpio_param.u8Pinnumber = (unsigned char)ui32Pin;
	gpio_param.u8Flag = (unsigned char)ui32Flag;

	ret_val = DDI_GPIO_Ioctl(LXDRV_GPIO_PULL_HIGHLOW, (UINT32)&gpio_param); 	

	return ret_val;
}

int CLI_GPIO_INOUT(int argc ,char **argv)
{
	UINT32 nArg = START_ARGS;
	UINT32	ui32Pin;
	UINT32	ui32Flag;
	LX_Gpio_Pinset_Param_t  gpio_param;
	int ret_val = RET_ERROR;

	if (argc < 3 || !strcmp(argv[1], "-help"))
	{
		printf("%s <Pin> <Flag>\n", argv[0]);
		return RET_ERROR;
	}
	ui32Pin = A2U;
	ui32Flag = A2U;

	gpio_param.u8Pinnumber = (unsigned char)ui32Pin;
	gpio_param.u8Flag = (unsigned char)ui32Flag;

	ret_val = DDI_GPIO_Ioctl(LXDRV_GPIO_INOUT, (UINT32)&gpio_param); 	

	return ret_val;
}

int CLI_GPIO_SET(int argc ,char **argv)
{
	UINT32 nArg = START_ARGS;
	UINT32	ui32Pin;
	LX_Gpio_Pinset_Param_t  gpio_param;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <Pin>\n", argv[0]);
		return RET_ERROR;
	}
	ui32Pin = A2U;
	gpio_param.u8Pinnumber = (unsigned char)ui32Pin;

	ret_val = DDI_GPIO_Ioctl(LXDRV_GPIO_SET, (UINT32)&gpio_param); 	

	return ret_val;
}

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int CLI_GPIO_Start(void)
{
    unsigned int lCliCmdId = CLI_GPIO_CMD_BASE;

	Add_CLI_Cmd("gpio_read", CLI_GPIO_READ);
	Add_CLI_Cmd("gpio_inout", CLI_GPIO_INOUT);
	Add_CLI_Cmd("gpio_highlow", CLI_GPIO_PULL_HIGHLOW);
	Add_CLI_Cmd("gpio_set", CLI_GPIO_SET);

	return RET_OK;
}


/**  @} */
