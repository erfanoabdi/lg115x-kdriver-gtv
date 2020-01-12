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

#ifndef _ADEC_BUF_CFG_H_
#define _ADEC_BUF_CFG_H_

#include "../adec_common/adec_common_cfg.h"


// Use Repeat
// This option effects on the size of structure
// All platforms, share memory using inter buffer, use same option.
#define ADEC_OPT_BUF_USE_REPEAT			1

// Use Rewind
// This option effects on the size of structure
// All platforms, share memory using inter buffer, use same option.
#define ADEC_OPT_BUF_USE_REWIND			0

// Use Debug Message
#define ADEC_OPT_BUF_USE_DEBUG          1

// Use Assert
#define ADEC_OPT_BUF_USE_ASSERT			1

// If platform is windows, force assertion to be turned on.
#ifdef _WIN32
#undef ADEC_OPT_BUF_USE_ASSERT
#define ADEC_OPT_BUF_USE_ASSERT			1
#endif

// Clear memory with specified value when init
//#define ADEC_OPT_BUF_CLR_MEM_INIT		1
#define ADEC_CNST_BUF_CLR_MEM_VAL		0xAD

#define ADEC_CNST_BUF_ALIGN_GUARD		4

// Use register for buffer status value
#define ADEC_OPT_BUF_USE_REG			1

// Use 4 registers for 1 buffer
#define ADEC_OPT_BUF_USE_4REG_PER_BUF	1

// Use total read and total write register for debugging
#define ADEC_OPT_BUF_USE_DEBUG_REG		0

// Max number of client of the multi sink buffer
#define ADEC_CNST_BUF_MAX_CLIENT		8

// Update multi sink buffer status when flush.
// If the read operation is performed during ISR, this option is not recommended.
#define ADEC_OPT_BUF_MULTI_UPDATE_FLUSH	0

// Update multi sink buffer stuatus when write.
// If the write operation is performed during ISR, this option is not recommended.
#define ADEC_OPT_BUF_MULTI_UPDATE_WRITE	1

#if (ADEC_OPT_BUF_MULTI_UPDATE_FLUSH == 1)
#undef ADEC_OPT_BUF_MULTI_UPDATE_WRITE
#define ADEC_OPT_BUF_MULTI_UPDATE_WRITE 0
#endif

#if (ADEC_OPT_BUF_MULTI_UPDATE_FLUSH == 0 && ADEC_OPT_BUF_MULTI_UPDATE_WRITE == 0)
#error At least one of the MULTI_UPDATE_FLUSH and MULT_UPDATE_WRITE should be cativated
#endif

#endif
