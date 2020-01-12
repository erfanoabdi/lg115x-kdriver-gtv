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
 *  main configuration file for sci device
 *	sci device will teach you how to make device driver with new platform.
 *
 *  author		Juhee Park
 *  version		1.0
 *  date		2011.06.02
 *  note		Additional information.
 *
 *  @addtogroup lg1152_sci
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
#include <linux/semaphore.h>
#include <linux/ioport.h>

#include <asm/io.h>
#include <linux/slab.h>

#include "os_util.h"
#include "base_dev_cfg.h"

#include "sci_err.h"
#include "sci_cfg.h"
#include "l9/sci_l9_drv.h"


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
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
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
SCI_RETURN_T SCI_Config(struct SCI_T **ppSCI)
{
	SCI_RETURN_T ret = SCI_SUCCESS;
	UINT32 chip = lx_chip_rev();
	UINT32 platform = lx_chip_plt();
	struct SCI_T* pSCI = NULL;

	pSCI = kzalloc(sizeof(struct SCI_T), GFP_KERNEL);
	if (pSCI == NULL)
	{
		ret = SCI_NOMEM;
		return ret;
	}

	pSCI->chip = chip;
	pSCI->platform = platform;
	pSCI->devno = MKDEV(SCI_MAJOR, SCI_MINOR);

	switch ( chip )
	{
		case LX_CHIP_REV(L9,A0):
		case LX_CHIP_REV(L9,A1):
		case LX_CHIP_REV(L9,B0):
		case LX_CHIP_REV(L9,B1):
			SCI_L9_Init(pSCI);
			break;
		default :
			ret = SCI_INVAL_PARAM;
			goto fail;
			return ret;
	}

	*ppSCI = pSCI;

	return SCI_SUCCESS;
fail:
	kfree(pSCI);
	return ret;
}

void SCI_Unconfig(struct SCI_T *pSCI)
{
	UINT32 chip = lx_chip_rev();

	switch ( chip )
	{
		case LX_CHIP_REV(L9,A0):
		case LX_CHIP_REV(L9,A1):
		case LX_CHIP_REV(L9,B0):
		case LX_CHIP_REV(L9,B1):
			SCI_L9_Exit(pSCI);
			break;
		default :
			break;
	}
	kfree(pSCI);
}

/** @} */

