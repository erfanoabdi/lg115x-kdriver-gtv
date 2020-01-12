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

#ifndef _ADEC_REG_API_H_
#define _ADEC_REG_API_H_

#include "adec_common_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void ADEC_REG_WriteRegister(void* _addr, unsigned int _value);
	unsigned int ADEC_REG_ReadRegister(void* _addr);
	int ADEC_REG_CheckRegister(void* _addr);

#ifdef __cplusplus
}
#endif

#endif
