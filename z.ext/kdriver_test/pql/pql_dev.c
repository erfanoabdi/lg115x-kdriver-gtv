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
 * date       2010.06.26
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

#include "../de/de_def.h"
#include "pql_kapi.h"

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
static UINT32 pql_fd = 0;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int PQL_DEV_Open(void)
{
	pql_fd = open( "/dev/lg/pql0", O_RDONLY|O_RDWR );

	return RET_OK;
}

int PQL_DEV_Close(void)
{
 	close(pql_fd);

	return RET_OK;
}

int PQL_DEV_SetTable(LX_PQL_SET_TABLE_T *pstParams)
{
	return ioctl( pql_fd, PQL_IOW_SET_TABLE, pstParams );
}

int PQL_DEV_SetEntry(PQL_PE_PARAM_T *pstParams)
{
	return ioctl( pql_fd, PQL_IOW_SET_ENTRY, pstParams );
}

int PQL_DEV_GetEntry(PQL_PE_PARAM_T *pstParams)
{
	return ioctl( pql_fd, PQL_IOR_GET_ENTRY, pstParams );
}


/**  @} */
