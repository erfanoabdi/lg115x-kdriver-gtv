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

#ifndef _H_ADEC_AAC_DEBUG_H_
#define _H_ADEC_AAC_DEBUG_H_

#include "adec_aac_config.h"

#if (AAC_OPT_USE_DEBUG == 1)
#define AAC_PRINT(...)			ADEC_DEBUG_PRINT(ADEC_DEBUG_TYPE_AAC_PARSER, __VA_ARGS__)
#define AAC_PRINT_LOW(...)		ADEC_DEBUG_PRINT(ADEC_DEBUG_TYPE_AAC_PARSER_LOW, __VA_ARGS__)
//#define AAC_PRINT_LOW(fmt,args...)	KADP_PRINT("^y^"fmt,##args)
#define AAC_ERROR(...)			ADEC_DEBUG_PRINT_ERR(ADEC_DEBUG_TYPE_AAC_PARSER, __VA_ARGS__)
#define AAC_TRACE_BEGIN()		ADEC_DEBUG_PRINT(ADEC_DEBUG_TYPE_TRACE,"[adec] %s:%d -- BEGIN\n", __FUNCTION__, __LINE__)
#define AAC_TRACE_END()			ADEC_DEBUG_PRINT(ADEC_DEBUG_TYPE_TRACE,"[adec] %s:%d -- END\n", __FUNCTION__, __LINE__ )
#define AAC_TRAC_MARK()			ADEC_DEBUG_PRINT(ADEC_DEBUG_TYPE_TRACE,"[adec] %s:%d -- TRACE !!!\n", __FUNCTION__, __LINE__ )
#define ADEC_CHECK_CODE(...)
#else
#define AAC_PRINT(...)			
#define AAC_PRINT_LOW(...)		
//#define AAC_PRINT_LOW(fmt,args...)	KADP_PRINT("^y^"fmt,##args)
#define AAC_ERROR(...) 
#define AAC_TRACE_BEGIN() 
#define AAC_TRACE_END()	
#define AAC_TRAC_MARK() 
#define ADEC_CHECK_CODE(...)
#endif

#define AAC_PRINT_TYPE_HEX8 "0x%02X"
#define AAC_PRINT_TYPE_HEX16 "0x%04X"
#define AAC_PRINT_TYPE_HEX32 "0x%16X"
#define AAC_PRINT_TYPE_STR "%s"
#define AAC_PRINT_TYPE_INT "%d"

#define ADEC_PRINT_MEMBER(_TYPE_,_INS_,_NAME_) \
	AAC_PRINT_LOW( #_INS_ "->" #_NAME_ " = " AAC_PRINT_TYPE_##_TYPE_ "\n",_INS_ -> _NAME_) 

#endif//_H_ADEC_AAC_DEBUG_H_