/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/** @file
 *
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     jasonlee.lee (jasonlee.lee@lge.com)
 * version    0.1
 * date       2010.03.11
 * note       Additional information.
 *
 * @addtogroup lg1150_sdec
 * @{
 */

#ifndef _SDEC_MM_H
#define _SDEC_MM_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/

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
   API
----------------------------------------------------------------------------------------*/
int 			SDEC_MM_Init(UINT32 ui32Arg);
int 			SDEC_MM_Final(void);
DTV_STATUS_T 	SDEC_MM_Alloc(UINT32 ui32Arg);
DTV_STATUS_T	SDEC_MM_Free(UINT32 ui32Arg);
DTV_STATUS_T	SDEC_MM_GetMemoryStat (UINT32 ui32Arg);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _SDEC_MM_H */

