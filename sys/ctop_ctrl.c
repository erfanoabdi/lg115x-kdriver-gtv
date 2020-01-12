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
 *  main driver implementation for sys device.
 *	sys device will teach you how to make device driver with new platform.
 *
 *  author		ks.hyun (ks.hyun@lge.com)
 *  version		1.0
 *  date		2010.12.13
 *  note		Additional information.
 *
 *  @addtogroup lg1150_sys
 *	@{
 */


/*----------------------------------------------------------------------------------------
 *	 Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <mach/platform.h>
#include <linux/delay.h>
#include <asm/io.h>

#include "base_types.h"
#include "os_util.h"
#include "sys_drv.h"
#include "ctop_ctrl.h"

#include "ctop_regs.h"


/*----------------------------------------------------------------------------------------
 *	 Constant Definitions
 *---------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
 *	 Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 External Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 global Variables
 *---------------------------------------------------------------------------------------*/
CTOP_CTRL_T gCTOP_CTRL;				// CTOP for L8
CTOP_CTRL_REG_L9_T gCTOP_CTRL_L9;	// CTOP for L9 A0
CTOP_CTRL_REG_L9B_T gCTOP_CTRL_L9B;	// CTOP for L9 B0 

/*----------------------------------------------------------------------------------------
 *	 Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 Static Variables
 *---------------------------------------------------------------------------------------*/


int CTOP_CTRL_Init(void)
{
	int regs_size;

	if (lx_chip_rev() >= LX_CHIP_REV(L9,B0))
	{
		regs_size = sizeof(CTOP_CTRL_REG_L9B0_T);

		gCTOP_CTRL_L9B.phys.addr = (volatile UINT32 *)ioremap(L9_TOPCTRL_BASE, regs_size);
		gCTOP_CTRL_L9B.shdw.addr = (UINT32 *)OS_KMalloc(regs_size);
	}
	
	/* A1 & B0 ctop are used at same time */
	
	if (lx_chip_rev() >= LX_CHIP_REV(L9,A0))
	{
		regs_size = sizeof(CTOP_CTRL_REG_L9A0_T);

		gCTOP_CTRL_L9.phys.addr = (volatile UINT32 *)ioremap(L9_TOPCTRL_BASE, regs_size);
		gCTOP_CTRL_L9.shdw.addr = (UINT32 *)OS_KMalloc(regs_size);
	}
 	else if (lx_chip_rev() >= LX_CHIP_REV(L8,B0))
	{
		regs_size = sizeof(CTOP_CTRL_REG_B0_T);

		gCTOP_CTRL.phys.addr = (volatile UINT32 *)ioremap(L8_TOP_CTRL, regs_size);
		gCTOP_CTRL.shadow.addr = (UINT32 *)OS_KMalloc(regs_size);
	}
	else
	{
		regs_size = sizeof(CTOP_CTRL_REG_A0_T);

		gCTOP_CTRL.phys.addr = (volatile UINT32 *)ioremap(L8_TOP_CTRL, regs_size);
		gCTOP_CTRL.shadow.addr = (UINT32 *)OS_KMalloc(regs_size);
	}

	return 0;
}

int CTOP_CTRL_Free(void)
{
	if (lx_chip_rev() >= LX_CHIP_REV(L9,B0))
	{
		OS_Free((void *)gCTOP_CTRL_L9B.shdw.addr);
		if(gCTOP_CTRL_L9B.phys.addr) iounmap((void *)gCTOP_CTRL_L9B.phys.addr);
	}
	
	/* A1 & B0 ctop are used at same time */
	
	if(lx_chip_rev() >= LX_CHIP_REV(L9,A0))
	{
		OS_Free((void *)gCTOP_CTRL_L9.shdw.addr);
		if (gCTOP_CTRL_L9.phys.addr) iounmap((void *)gCTOP_CTRL_L9.phys.addr);
	}
	else
	{
		OS_Free((void *)gCTOP_CTRL.shadow.addr);
		if (gCTOP_CTRL.phys.addr) iounmap((void *)gCTOP_CTRL.phys.addr);
	}

	return 0;
}



