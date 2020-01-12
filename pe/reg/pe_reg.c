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

/** @file pe_reg.c
 *
 *  driver for picture enhance register read, write functions. ( used only within kdriver )
 *
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note
 *	@date		2011.06.11
 *	@see
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <mach/platform.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include "base_types.h"
#include "os_util.h"

#include "de_reg_def_l9.h"
#include "pe_cfg.h"
#include "pe_def.h"
#include "pe_reg.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_REG_ERROR	printk

#ifdef PE_USE_DBG_PRINT
#define PE_REG_DBG_PRINT(fmt,args...)	printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#else
#define PE_REG_DBG_PRINT(fmt,args...)
#endif

#define PE_REG_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_REG_ERROR(fmt,##args);_action;}}

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
static int PE_REG_WD(UINT32 addr, UINT32 value);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * write address data set
 *
 * @param   *pstParams [in] LX_PQL_SET_TABLE_T
 * @param   offset [in] UINT32
 * @param   value [in] UINT32
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_REG_SetTable(const PE_REG_PARAM_T *table, UINT32 offset, UINT32 size)
{
	int ret = RET_ERROR;
	UINT32 table_size;
	int i;

	do {
		PE_REG_CHECK_CODE(!table,break,"[%s,%d] table is null.\n",__F__,__L__);
		PE_REG_CHECK_CODE(!size,break,"[%s,%d] size is zero.\n",__F__,__L__);
		table_size = size/sizeof(PE_REG_PARAM_T);
		for (i=0;i<table_size;i++) PE_REG_WD((table[i].addr+offset), table[i].data);
		ret = RET_OK;
	} while (0);

	return ret;
}

/**
 * write address data
 *
 * @param   addr [in] UINT32
 * @param   value [in] UINT32
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int PE_REG_WD(UINT32 addr, UINT32 value)
{
	int ret = RET_OK;
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 currValue;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;
	UINT32 virtAddr = 0;

	do {
		if(PE_CHIP_VER_GT(L9,B0))
		{
	        recvAddr = addr;
	        if      (recvAddr <= 0x6000) recvAddr += DTVSOC_DE_L9_BASE;
	        else if (recvAddr <= 0xffff) recvAddr += DTVSOC_DE_L9_BASE;
	        currAddr = recvAddr;
	        currAddr >>= 2;
	        currAddr <<= 2;

			virtAddr = (UINT32)ioremap(currAddr, 0x8);
	        if (currAddr == recvAddr) {
#if (KDRV_PLATFORM != KDRV_COSMO_PLATFORM)	// daeseok.youn fixed bug.
	            REG_WD(recvAddr, value);
#else
	            REG_WD(virtAddr, value);
#endif
	            break;
	        }
	        currValue = REG_RD(virtAddr);
	        nextValue = REG_RD((virtAddr+4));

	        dBit = (recvAddr - currAddr)<<3;
	        wBit = (32 - dBit);

	        currValue  = GET_BITS(currValue ,0    ,dBit);
	        currValue += GET_PVAL(value     ,dBit ,wBit);

	        nextValue  = GET_PVAL(nextValue ,dBit ,wBit);
	        nextValue += GET_BITS(value     ,0    ,dBit);
	        REG_WD(virtAddr     ,currValue);
	        REG_WD((virtAddr+4) ,nextValue);
		}
		else
		{
			recvAddr = addr;
			if		(recvAddr <= 0x4000) recvAddr += L9_DE_BASE;
			else if (recvAddr <= 0xffff) recvAddr += L9_PL301_BASE;
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
		}
	} while (0);
	if (virtAddr) iounmap((void *)virtAddr);

	return ret;
}

/**
 * read address data
 *
 * @param   addr [in] UINT32, accessing for register
 * @return  value of register
 * @see
 * @author
 */
UINT32 PE_REG_RD(UINT32 addr)
{
	UINT32 value = 0x0;
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;
	UINT32 virtAddr = 0;

	do {
		if(PE_CHIP_VER_GT(L9,B0))
		{
	        recvAddr = addr;
	        if      (recvAddr <= 0x6000) recvAddr += DTVSOC_DE_L9_BASE;
	        else if (recvAddr <= 0xffff) recvAddr += DTVSOC_DE_L9_BASE;

	        currAddr = recvAddr;
	        currAddr >>= 2;
	        currAddr <<= 2;
			virtAddr = (UINT32)ioremap(currAddr, 0x8);

	        value = REG_RD(virtAddr);
	        if (currAddr == recvAddr) break;

	        nextValue = REG_RD(virtAddr+4);
	        dBit = (recvAddr - currAddr)<<3;
	        wBit = (32 - dBit);
	        value  = GET_BITS(value, dBit, wBit);
	        value += GET_PVAL(nextValue, wBit, dBit);
		}
		else
		{
			recvAddr = addr;
			if		(recvAddr <= 0x4000) recvAddr += L9_DE_BASE;
			else if (recvAddr <= 0xffff) recvAddr += L9_PL301_BASE;

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
		}
	} while (0);
	if (virtAddr) iounmap((void *)virtAddr);

	return value;
}

