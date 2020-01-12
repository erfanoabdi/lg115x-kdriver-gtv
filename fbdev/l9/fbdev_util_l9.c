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
 *  Brief description. 
 *  Detailed description starts here. 
 *
 *  @author		raxis.lim
 *  @version	1.0 
 *  @date		2011-04-03
 *  @note		Additional information. 
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "fbdev_impl.h"
#include "fbdev_hw.h"
#include "fbdev_hal.h"
#include "fbdev_util_l9.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
    Implementation Group
========================================================================================*/
UINT32	FBDEV_L9_RemapFRCHeader ( UINT32 in )
{
	typedef struct
	{
		UINT32		blk00:8,
					__r00:2,
					blk10:6,
					blk11:2,
					__r01:2,
					blk20:4,
					blk21:4,
					__r02:2,
					__r03:2;
	}
	REMAP_BEFORE_T;

	typedef struct
	{
		UINT32		blk00:8,
					blk10:6,
					blk11:2,
					blk20:4,
					blk21:4,

					__r03:2,
					__r00:2,
					__r01:2,
					__r02:2;
	}
	REMAP_AFTER_T;

	UINT32	out;
	REMAP_BEFORE_T	remap0;
	REMAP_AFTER_T	remap1;

	memcpy(&remap0, &in, 4 );

	remap1.blk00 = remap0.blk00;
	remap1.blk10 = remap0.blk10;
	remap1.blk11 = remap0.blk11;
	remap1.blk20 = remap0.blk20;
	remap1.blk21 = remap0.blk21;
	remap1.__r00 = remap0.__r00;
	remap1.__r01 = remap0.__r01;
	remap1.__r02 = remap0.__r02;
	remap1.__r03 = remap0.__r03;

	memcpy(&out, &remap1, 4 );

	return out;
}
