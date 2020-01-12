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
 *  main driver implementation for monitor device.
 *	monitor device will teach you how to make device driver with new platform.
 *
 *  author		raxis.lim (raxis.lim@lge.com).lim (raxis.lim@lge.com)
 *  version		1.0
 *  date		2011.03.18
 *  note		Additional information.
 *
 *  @addtogroup lg1150_monitor
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_MONITOR_DEVICE_READ_WRITE_FOPS

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "os_util.h"
#include "base_dev_cfg.h"
#include "monitor_drv.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define GET_BM(value, mask) 		((value) & (mask))
#define GET_CM(value, mask) 		(((value) & (mask)) == (mask))
#define GET_BITS(val,nd,wd)         (((val)>>(nd))&(0xFFFFFFFF>>(32-(wd))))
#define GET_PVAL(val,nd,wd)         (GET_BITS(val,nd,wd)<<(nd))
#define GET_PMSK(    nd,wd)         ((((0xFFFFFFFF)>>(nd))&(0xFFFFFFFF>>(32-(wd)))) << (nd))
#define GET_RMSK(    nd,wd)         (~GET_PMSK(nd,wd))
#define GET_MASK(val,mk)            ((val) & (mk))

#define REG_WD( addr, value )       ( *( volatile UINT32 * )( addr )) = ( UINT32 )( value )
#define REG_RD( addr )              ( *( volatile UINT32 * )( addr ))
#define DDR_WD( addr, value )       ( *( UINT32 * )( addr ) ) = ( UINT32 )( value )
#define DDR_RD( addr )              ( *( UINT32 * )( addr ) )


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
int	MONITOR_WriteReg	( UINT32 addr, UINT32 value )
{
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 currValue;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;

	do {
		recvAddr = (UINT32)ioremap(addr,4);
		currAddr = recvAddr;
		currAddr >>= 2;
		currAddr <<= 2;


		if (currAddr == recvAddr) {
			REG_WD(recvAddr, value);
			break;
		}
		currValue = REG_RD(currAddr);
		nextValue = REG_RD((currAddr+4));

		dBit = (recvAddr - currAddr)<<3;
		wBit = (32 - dBit);

		currValue  = GET_BITS(currValue ,0	  ,dBit);
		currValue += GET_PVAL(value		,dBit ,wBit);

		nextValue  = GET_PVAL(nextValue ,dBit ,wBit);
		nextValue += GET_BITS(value		,0	  ,dBit);
		REG_WD(currAddr		,currValue);
		REG_WD((currAddr+4) ,nextValue);
		iounmap((void *)recvAddr);
	} while (0);
	return RET_OK;
}

/* i2c handler for internal register control */
int	MONITOR_ReadReg		( UINT32 addr, UINT32 *pValue )
{
	UINT32 value;
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;

	do {
		recvAddr = (UINT32)ioremap(addr,4);
		currAddr = recvAddr;
		currAddr >>= 2;
		currAddr <<= 2;
		value = REG_RD(currAddr);

		if (currAddr == recvAddr) break;

		nextValue = REG_RD(currAddr+4);
		dBit = (recvAddr - currAddr)<<3;
		wBit = (32 - dBit);
		value  = GET_BITS(value, dBit, wBit);
		value += GET_PVAL(nextValue, wBit, dBit);

		iounmap((void *)recvAddr);
	} while (0);

	*pValue = value;
	return RET_OK;
}

