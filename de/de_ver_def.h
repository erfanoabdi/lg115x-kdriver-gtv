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
 * date       2011.02.21
 * note       Additional information.
 *
 * @addtogroup lg1152_de
 * @{
 */

#ifndef  DE_VER_INC
#define  DE_VER_INC

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/
#define USE_KDRV_CODES_FOR_L9
#ifdef  KDRV_GLOBAL_LINK
#define USE_KDRV_CODES_FOR_L8
#endif

#define USE_PARM_CODES_FOR_L9
#define USE_CTOP_CODES_FOR_L9
#define USE_KDRV_CODES_FOR_CSC_n

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#ifndef LX_CHIP_REV
#define LX_CHIP_L8     0x08
#define LX_CHIP_L9     0x09
#define lx_chip_rev() 0x09A0
#define MAKE_CHIP_REV(chp, rev) (( chp << 8) | ( rev ))
#define LX_CHIP_REV(chp, rev) MAKE_CHIP_REV( LX_CHIP_##chp, 0x##rev )
#endif

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef DE_VER_INC  ----- */
/**  @} */
