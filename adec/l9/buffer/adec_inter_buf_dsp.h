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


#ifndef _ADEC_INTER_BUF_DSP_H_
#define _ADEC_INTER_BUF_DSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "adec_common_buf.h"

typedef struct
{
	PFN_ADEC_CACHE_CONTROL		pfnInvalidate;
	PFN_ADEC_CACHE_CONTROL		pfnWriteback;
	PFN_ADEC_ADDR_TRANSLATOR	pfnAddrTrans;
}DSP_PROCESSOR_ADAP;

void ADEC_DSP_Create_Interbuf_Writer(void);
void ADEC_DSP_Create_Interbuf_Reader(void);

extern DSP_PROCESSOR_ADAP gDspWriterAdap;

#ifdef __cplusplus
}
#endif


#endif
