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

#ifndef _ADEC_DBG_H_
#define _ADEC_DBG_H_


#include "adec_common_cfg.h"

#if (ADEC_OPT_USE_DEBUG == 1)
	#if defined(_WIN32)
		#if (ADEC_OPT_DEBUG_LEVEL > 0)
			#define _ADEC_DBG_PRINT		printf
			#if (ADEC_OPT_BUF_DEBUG_LEVEL > 1)
				#define _ADEC_DBG_LOW	printf
			#endif
		#endif
		#define _ADEC_DBG_ERR		printf
	#elif defined(__XTENSA__)
		#include "adec_debug.h"
		#define _ADEC_DBG_PRINT			ADEC_DBG_BUF
		#define _ADEC_DBG_LOW			ADEC_DBG_BUF
		#define _ADEC_DBG_ERR			ADEC_DBG_BUF
	#else
		#if (ADEC_OPT_DEBUG_LEVEL > 0)
			#define _ADEC_DBG_PRINT		ADEC_DEBUG_PRINT
			#if (ADEC_OPT_DEBUG_LEVEL > 1)
				#define _ADEC_DBG_LOW	ADEC_DEBUG_LOW
			#endif
		#endif
		#define _ADEC_DBG_ERR			ADEC_DEBUG_ERR
	#endif
#else
	#define _ADEC_DBG_PRINT(...)
	#define _ADEC_DBG_LOW(...)
	#define _ADEC_DBG_ERR(...)
#endif

#if (ADEC_OPT_USE_ASSERT == 1)
	#ifdef _WIN32
		#ifdef _CUNIT
			#include "CUnit.h"
			#define _ADEC_ASSERT(_COND, _RETVAL, ...)	if((_COND) ==0){CU_FAIL_FATAL(#_COND); return _RETVAL;}
		#else
			#include <assert.h>
			#define _ADEC_ASSERT(_COND, _RETVAL, ...)   assert(_COND);
		#endif
	#else
		#define _ADEC_ASSERT(_COND, _RETVAL, ...)	if((_COND) == 0){BufDbg_AssertBreak();_ADEC_DBG_ERR(__VA_ARGS__);return _RETVAL; }
	#endif
#else
	#define _ADEC_ASSERT(...)
#endif

#endif
