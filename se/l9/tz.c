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
 *  This file.......
 *
 *  author		Juhee Park
 *  version		1.0
 *  date		2011.6.11
 *  note		Additional information.
 *
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <asm/io.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include "tz_if.h"

/*
* tunnel to SWd.
**/
void TZ_IOCTL(TZ_ARG_T *pTzArg)
{
	isb();
	dsb();
	dmb();

	asm volatile (
        "LDM    %1, {r0-r7}\n"
        ".word  0xe1600070 @SMC 0\n"
        "MOV    %0, r0\n"
        : "=r" (pTzArg->cmd_r0)
        : "r"  (pTzArg)
        : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r12"
    );
}

