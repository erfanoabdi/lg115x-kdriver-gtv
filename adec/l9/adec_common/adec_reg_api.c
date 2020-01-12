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

#include "adec_reg_api.h"
#include "../adec_common/adec_common_err.h"

#define ADEC_REG_SimpleDelay(_LOOP_)\
{\
	int ___i = _LOOP_ << 10;\
	while ( ___i-- > 0 );\
}

#ifdef _WIN32

int ADEC_REG_CheckRegister(void* _addr)
{
#if(ADEC_OPT_USE_CHK_BIT == 0)
	return ADEC_ERR_NONE;
#else
	int retry = ADEC_CNST_REG_RETRY_CNT;

	while(retry--)
	{
		if(((*(volatile unsigned int*)(_addr) >> 28) & 0x0f) == ADEC_CNST_CHK_BIT)
		{
			return ADEC_ERR_NONE;
		}
	}
	return ADEC_ERR_DATA_CORRUPTED;
#endif
}

void ADEC_REG_WriteRegister(void* _addr, unsigned int _value)
{
	unsigned int val;
	int retry = ADEC_CNST_REG_RETRY_CNT;

#if (ADEC_OPT_USE_CHK_BIT == 1)
	val = (ADEC_CNST_CHK_BIT << 28) | (_value & 0x0fffffff);
#else
	val = _value;
#endif
	*(volatile unsigned int *)(_addr) = val;
	while((ADEC_REG_ReadRegister(_addr) != (_value & 0x0fffffff)) && ((retry--) > 0))
	{
		ADEC_REG_SimpleDelay(1);
		*(volatile unsigned int *)(_addr) = val;
	}
}

unsigned int ADEC_REG_ReadRegister(void* _addr)
{
	unsigned int val;
	int retry = ADEC_CNST_REG_RETRY_CNT;
	
	do
	{
		val = *(volatile unsigned int*)(_addr);
#if (ADEC_OPT_USE_CHK_BIT == 1)
		if( ((val>>28)&0xf) == ADEC_CNST_CHK_BIT )
			return (val & 0x0fffffff);
#else
		return val;
#endif
		ADEC_REG_SimpleDelay(1);
	}while((retry--) > 0);
	return 0;
}

#else
#ifdef __XTENSA__

// DSP0 & DSP1

int ADEC_REG_CheckRegister(void* _addr)
{
#if(ADEC_OPT_USE_CHK_BIT == 0)
	return ADEC_ERR_NONE;
#else
	int retry = ADEC_CNST_REG_RETRY_CNT;

	while(retry--)
	{
		if(((*(volatile unsigned int*)(_addr) >> 28) & 0x0f) == ADEC_CNST_CHK_BIT)
		{
			return ADEC_ERR_NONE;
		}
	}
	return ADEC_ERR_DATA_CORRUPTED;
#endif
}

void ADEC_REG_WriteRegister(void* _addr, unsigned int _value)
{
	unsigned int val;
	int retry = ADEC_CNST_REG_RETRY_CNT;

#if (ADEC_OPT_USE_CHK_BIT == 1)
	val = (ADEC_CNST_CHK_BIT << 28) | (_value & 0x0fffffff);
#else
	val = _value;
#endif
	*(volatile unsigned int *)(_addr) = val;
	while((ADEC_REG_ReadRegister(_addr) != (_value & 0x0fffffff)) && ((retry--) > 0))
	{
		ADEC_REG_SimpleDelay(1);
		*(volatile unsigned int *)(_addr) = val;
	}
}

unsigned int ADEC_REG_ReadRegister(void* _addr)
{
	unsigned int val;
	int retry = ADEC_CNST_REG_RETRY_CNT;
	
	do
	{
		val = *(volatile unsigned int*)(_addr);
#if (ADEC_OPT_USE_CHK_BIT == 1)
		if( ((val>>28)&0xf) == ADEC_CNST_CHK_BIT )
			return (val & 0x0fffffff);
#else
		return val;
#endif
		ADEC_REG_SimpleDelay(1);
	}while((retry--) > 0);
	return 0;
}

#else

// Linux

int ADEC_REG_CheckRegister(void* _addr)
{
#if(ADEC_OPT_USE_CHK_BIT == 0)
	return ADEC_ERR_NONE;
#else
	int retry = ADEC_CNST_REG_RETRY_CNT;

	while(retry--)
	{
		if(((*(volatile unsigned int*)(_addr) >> 28) & 0x0f) == ADEC_CNST_CHK_BIT)
		{
			return ADEC_ERR_NONE;
		}
	}
	return ADEC_ERR_DATA_CORRUPTED;
#endif
}

void ADEC_REG_WriteRegister(void* _addr, unsigned int _value)
{
	unsigned int val;
	int retry = ADEC_CNST_REG_RETRY_CNT;

#if (ADEC_OPT_USE_CHK_BIT == 1)
	val = (ADEC_CNST_CHK_BIT << 28) | (_value & 0x0fffffff);
#else
	val = _value;
#endif
	*(volatile unsigned int *)(_addr) = val;
	while((ADEC_REG_ReadRegister(_addr) != (_value & 0x0fffffff)) && ((retry--) > 0))
	{
		udelay(1);
		*(volatile unsigned int *)(_addr) = val;
	}
}

unsigned int ADEC_REG_ReadRegister(void* _addr)
{
	unsigned int val;
	int retry = ADEC_CNST_REG_RETRY_CNT;
	
	do
	{
		val = *(volatile unsigned int*)(_addr);
#if (ADEC_OPT_USE_CHK_BIT == 1)
		if( ((val>>28)&0xf) == ADEC_CNST_CHK_BIT )
			return (val & 0x0fffffff);
#else
		return val;
#endif
		udelay(1);
	}while((retry--) > 0);
	return 0;
}

#endif // __XTENSA__
#endif // _WIN32
