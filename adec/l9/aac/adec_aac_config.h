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

#ifndef _ADEC_AAC_CONFIG_H_
#define _ADEC_AAC_CONFIG_H_

#define AAC_OPT_KERNEL_DRVIER				1

#if (AAC_OPT_KERNEL_DRVIER == 1) && !defined(_WIN32)
	#define AAC_MALLOC	OS_KMalloc
	#define AAC_FREE	OS_KFree
#else
	#define AAC_MALLOC	malloc
	#define AAC_FREE	free
#endif

#define AAC_OPT_USE_DEBUG					0

/////////////////////////////////////////////////////////////
// ADTS OPTION
/////////////////////////////////////////////////////////////
// Parse and save information regarding raw data blocks in ADTS frames
#define AAC_OPT_PARSE_RAW_DATA_BLOCK		1

// Print whole members of parsed ADTS header 
#define AAC_OPT_PRINT_ADTS_MEMBER			1

// Check Next Frame Header of the ADTS
#define AAC_OPT_CHECK_NEXT_FRAME_HEADER		1





/////////////////////////////////////////////////////////////
// AudioSpecificConfig Option
/////////////////////////////////////////////////////////////
// Print whole members of parsed AudioSpecificConfig 
#define AAC_OPT_PRINT_ASC_MEMBER			1






/////////////////////////////////////////////////////////////
// AAC PROBE OPTION
/////////////////////////////////////////////////////////////
// Ignore format parsed, has error previously
#define AAC_OPT_SKIP_PREVIOUS_ERROR_FORMAT  1

#endif // _ADEC_AAC_CONFIG_H_