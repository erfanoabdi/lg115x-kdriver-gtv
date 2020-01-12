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


#include "../adec_common/adec_mutex.h"
#include "adec_common_buf.h"
#include "adec_inter_buf_dsp.h"


static void DSP_DCache_Invalidate(void* _addr, unsigned int _length, void* _adap);
static void DSP_DCache_Writeback(void* _addr, unsigned int _length, void* _adap);
static unsigned char* DSP_Mem_Translator(unsigned char* _addr, void* _adap);


DSP_PROCESSOR_ADAP gDspWriterAdap =
{
	DSP_DCache_Invalidate,
	DSP_DCache_Writeback,
	DSP_Mem_Translator
};


static void DSP_DCache_Invalidate(void* _addr, unsigned int _length, void* _adap)
{
	xthal_dcache_region_invalidate(_addr, _length);

	return;
}

static void DSP_DCache_Writeback(void* _addr, unsigned int _length, void* _adap)
{
    xthal_dcache_region_writeback(_addr, _length);

	return;
}

static unsigned char* DSP_Mem_Translator(unsigned char* _addr, void* _adap)
{
	return _addr;
}

