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
#include "dimm_kapi.h"

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
static UINT32 dimm_fd = 0;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int DIMM_DEV_Open(void)
{
	dimm_fd = open( "/dev/lg/dimm0", O_RDONLY|O_RDWR );

	return RET_OK;
}

int DIMM_DEV_Close(void)
{
 	close(dimm_fd);

	return RET_OK;
}

int DIMM_DEV_Init(LX_DIMM_INFO_T *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_INIT, pstParams );
}

int DIMM_DEV_LocaldimmingControl(BOOLEAN *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_LOCALDIMMING_CONTROL, pstParams );
}

int DIMM_DEV_StoreDemo(BOOLEAN *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_STORE_DEMO, pstParams );
}

int DIMM_DEV_LineDemo(LX_DIMM_LINE_CTRL_T *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_LINE_DEMO, pstParams );
}

int DIMM_DEV_ControlCompensation(BOOLEAN *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_PIXELCOMPENSATION_CONTROL, pstParams );
}

int DIMM_DEV_LutControl(LX_DIMM_LUT_CTRL_T *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_LUT_CONTROL, pstParams );
}

int DIMM_DEV_StoreControl(BOOLEAN *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_STORE_CONTROL, pstParams );
}

int DIMM_DEV_RegRd(UINT32 *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOR_REG_RD, pstParams );
}

int DIMM_DEV_RegWr(LX_DIMM_REG_WRITE_T *pstParams)
{
	return ioctl( dimm_fd, DIMM_IOW_REG_WR, pstParams );
}

/**  @} */
