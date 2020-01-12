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

#ifndef  DIMM_DEV_INC
#define  DIMM_DEV_INC

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
int DIMM_DEV_Init(LX_DIMM_INFO_T *pstParams);
int DIMM_DEV_LocaldimmingControl(BOOLEAN *pstParams);
int DIMM_DEV_StoreDemo(BOOLEAN *pstParams);
int DIMM_DEV_LineDemo(LX_DIMM_LINE_CTRL_T *pstParams);
int DIMM_DEV_ControlCompensation(BOOLEAN *pstParams);
int DIMM_DEV_LutControl(LX_DIMM_LUT_CTRL_T *pstParams);
int DIMM_DEV_StoreControl(BOOLEAN *pstParams);
int DIMM_DEV_RegRd(UINT32 *pstParams);
int DIMM_DEV_RegWr(LX_DIMM_REG_WRITE_T *pstParams);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef DIMM_DEV_INC  ----- */
/**  @} */
