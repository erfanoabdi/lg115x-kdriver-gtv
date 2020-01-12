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
 * @addtogroup lg1150_dimm
 * @{
 */

#ifndef  DIMM_CLI_INC
#define  DIMM_CLI_INC

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
int CLI_DIMM_Init(int argc ,char **argv);
int CLI_DIMM_LocaldimmingControl(int argc ,char **argv);
int CLI_DIMM_StoreDemo(int argc ,char **argv);
int CLI_DIMM_LineDemo(int argc ,char **argv);
int CLI_DIMM_ControlCompensation(int argc ,char **argv);
int CLI_DIMM_LutControl(int argc ,char **argv);
int CLI_DIMM_StoreControl(int argc ,char **argv);
int CLI_DIMM_RegRd(int argc ,char **argv);
int CLI_DIMM_RegWr(int argc ,char **argv);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef DIMM_CLI_INC  ----- */

/**  @} */
