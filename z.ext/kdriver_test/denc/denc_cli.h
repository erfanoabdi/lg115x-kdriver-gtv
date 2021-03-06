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

#ifndef  DENC_CLI_INC
#define  DENC_CLI_INC

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
int CLI_DENC_Init(int argc ,char **argv);
int CLI_DENC_OnOff(int argc ,char **argv);
int CLI_DENC_SetOutPutVideoStd(int argc ,char **argv);
int CLI_DENC_SetSource(int argc ,char **argv);
int CLI_DENC_TtxEnable(int argc ,char **argv);
int CLI_DENC_WssEnable(int argc ,char **argv);
int CLI_DENC_VpsEnable(int argc ,char **argv);
int CLI_DENC_SetTtxData(int argc ,char **argv);
int CLI_DENC_SetWssData(int argc ,char **argv);
int CLI_DENC_SetVpsData(int argc ,char **argv);
int CLI_DENC_ColorBarEnable(int argc ,char **argv);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef DENC_CLI_INC  ----- */
/**  @} */
