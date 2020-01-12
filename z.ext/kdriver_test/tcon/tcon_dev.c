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

#include "tcon_kapi.h"

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
static UINT32 tcon_fd = 0;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int TCON_DEV_Open(void)
{
	tcon_fd = open( "/dev/lg/tcon0", O_RDONLY|O_RDWR );

	return RET_OK;
}

int TCON_DEV_Close(void)
{
 	close(tcon_fd);

	return RET_OK;
}

int TCON_DEV_Init(LX_TCON_INIT_INFO_T  *pstParams)
{
	return ioctl( tcon_fd, TCON_IOW_INIT, pstParams );
}

int TCON_DEV_SetOdcTable(LX_TCON_ODC_TABLE_T *pstParams)
{
	return ioctl( tcon_fd, TCON_IOW_SET_ODC_TABLE, pstParams );
}

int TCON_DEV_SetDga(LX_TCON_DGA_LUT_T *pstParams)
{
	return ioctl( tcon_fd, TCON_IOW_SET_DGA, pstParams );
}

int TCON_DEV_Control(LX_TCON_CTRL_T *pstParams)
{
	return ioctl( tcon_fd, TCON_IOW_CONTROL, pstParams );
}

/**  @} */
