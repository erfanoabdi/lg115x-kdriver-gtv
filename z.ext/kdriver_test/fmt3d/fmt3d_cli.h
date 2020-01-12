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

#ifndef  FMT3D_CLI_INC
#define  FMT3D_CLI_INC

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
int CLI_FMT3D_Init(int argc ,char **argv);
int CLI_FMT3D_SetProcessMode(int argc ,char **argv);
int CLI_FMT3D_GetProcessMode(int argc ,char **argv);
int CLI_FMT3D_SetInImgInfo(int argc ,char **argv);
int CLI_FMT3D_GetInImgInfo(int argc ,char **argv);
int CLI_FMT3D_GetOutImgInfo(int argc ,char **argv);
int CLI_FMT3D_SetRunmode(int argc ,char **argv);
int CLI_FMT3D_ToggleLrInOrder(int argc ,char **argv);
int CLI_FMT3D_ToggleLrOutOrder(int argc ,char **argv);
int CLI_FMT3D_SetInImgFmt(int argc ,char **argv);
int CLI_FMT3D_SetDisFmt(int argc ,char **argv);
int CLI_FMT3D_SetInFrRate(int argc ,char **argv);
int CLI_FMT3D_SetOutFrRate(int argc ,char **argv);
int CLI_FMT3D_SetConvergence(int argc ,char **argv);
int CLI_FMT3D_SetImgDoublePic(int argc ,char **argv);
int CLI_FMT3D_SetSampleMode(int argc ,char **argv);
int CLI_FMT3D_SetSrcMux(int argc ,char **argv);
int CLI_FMT3D_SetImgScanType(int argc ,char **argv);
int CLI_FMT3D_SetFrcOutType(int argc ,char **argv);
int CLI_FMT3D_SetFrcOutOrder(int argc ,char **argv);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef FMT3D_CLI_INC  ----- */

/**  @} */
