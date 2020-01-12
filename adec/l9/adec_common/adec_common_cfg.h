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

#ifndef _ADEC_COMMON_CFG_H_
#define _ADEC_COMMON_CFG_H_

#if defined(_WIN32)
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#elif defined(__XTENSA__)
#include <stdlib.h>
#else
#include <linux/delay.h>
#include "../adec_coredrv_l9.h"
#endif

#ifndef NULL
#define NULL (void*)0
#endif

#define ADEC_OPT_USE_CHK_BIT		1
#define ADEC_OPT_USE_DEBUG			1
#define ADEC_OPT_USE_ASSERT			1

// Set the level of debug
// 0 : only err,  1 : err, print,  2 : err, print, low
#define ADEC_OPT_DEBUG_LEVEL		2

#define ADEC_CNST_CHK_BIT			0xA
#define ADEC_CNST_REG_RETRY_CNT		0x10


#ifdef _WIN32
#undef ADEC_OPT_USE_ASSERT
#define ADEC_OPT_USE_ASSERT			1
#define __INLINE__					__forceinline
#else
#define __INLINE__					inline
#endif

#endif

