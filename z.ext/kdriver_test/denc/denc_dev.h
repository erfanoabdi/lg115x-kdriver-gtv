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

#ifndef  DENC_DEV_INC
#define  DENC_DEV_INC

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
int DENC_DEV_Open(void);
int DENC_DEV_Close(void);
int DENC_DEV_Init(void);
int DENC_DEV_OnOff(BOOLEAN *pstParams);
int DENC_DEV_SetOutPutVideoStd(LX_DENC_VIDEO_SYSTEM_T *pstParams);
int DENC_DEV_SetSource(LX_DENC_VIDEO_SOURCE_T *pstParams);
int DENC_DEV_TtxEnable(BOOLEAN *pstParams);
int DENC_DEV_WssEnable(BOOLEAN *pstParams);
int DENC_DEV_VpsEnable(BOOLEAN *pstParams);
int DENC_DEV_SetTtxData(LX_DENC_TTX_DATA_T *pstParams);
int DENC_DEV_SetWssData(LX_DENC_WSS_DATA_T *pstParams);
int DENC_DEV_SetVpsData(LX_DENC_VPS_DATA_T *pstParams);
int DENC_DEV_ColorBarEnable(BOOLEAN *pstParams);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef DENC_DEV_INC  ----- */
/**  @} */
