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
 ***************************************************************************************/

/** @file
 *
 *  main configuration file for se device
 *	se device will teach you how to make device driver with new platform.
 *
 *  author		Juhee Park
 *  version		1.0
 *  date		2011.06.02
 *  note		Additional information.
 *
 *  @addtogroup lg_se
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/slab.h>

#include <asm/io.h>

#include "os_util.h"
#include "base_dev_cfg.h"
#include "se_cfg.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

#ifndef MAKE_SE_MODULE
#include "se_mem_cfg.c"
#endif

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
void SE_PreInit(void)
{

	if (lx_chip_rev() >= LX_CHIP_REV(L9,A0)) {
        switch ( lx_chip_plt() ) {
            case LX_CHIP_PLATFORM_COSMO:
				gMemCfgSE.base  = 0x2fd00000;     /*INCLUDE_GTV*/
				gMemCfgSE_TZFW.base = 0x2ff00000; /*INCLUDE_GTV*/
	            break;

            case LX_CHIP_PLATFORM_GP:
            default:
                break;
        }
    }
	else { /* L8 */
		/* not necessary */
	}
}

/** @} */

