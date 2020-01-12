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
#include "denc_kapi.h"

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
static UINT32 denc_fd = 0;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int DENC_DEV_Open(void)
{
	denc_fd = open( "/dev/lg/denc0", O_RDONLY|O_RDWR );

	return RET_OK;
}

int DENC_DEV_Close(void)
{
 	close(denc_fd);

	return RET_OK;
}

int DENC_DEV_Init(void)
{
	return ioctl( denc_fd, DENC_IO_INIT);
}

int DENC_DEV_OnOff(BOOLEAN *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_ONOFF, pstParams);
}

int DENC_DEV_SetOutPutVideoStd(LX_DENC_VIDEO_SYSTEM_T *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_SET_OUTPUT_VIDEO_STD, pstParams);
}

int DENC_DEV_SetSource(LX_DENC_VIDEO_SOURCE_T *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_SET_SOURCE, pstParams);
}

int DENC_DEV_TtxEnable(BOOLEAN *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_TTX_ENABLE, pstParams);
}

int DENC_DEV_WssEnable(BOOLEAN *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_WSS_ENABLE, pstParams);
}

int DENC_DEV_VpsEnable(BOOLEAN *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_VPS_ENABLE, pstParams);
}

int DENC_DEV_SetTtxData(LX_DENC_TTX_DATA_T *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_SET_TTX_DATA, pstParams);
}

int DENC_DEV_SetWssData(LX_DENC_WSS_DATA_T *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_SET_WSS_DATA, pstParams);
}

int DENC_DEV_SetVpsData(LX_DENC_VPS_DATA_T *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_SET_VPS_DATA, pstParams);
}

int DENC_DEV_ColorBarEnable(BOOLEAN *pstParams)
{
	return ioctl( denc_fd, DENC_IOW_COLORBAR_ENABLE, pstParams);
}

/**  @} */
