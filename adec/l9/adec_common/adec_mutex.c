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

#include "adec_mutex.h"

static ADEC_MUTEX_RESULT_T AlwaysSuccessMutex_Acqire(void* _mutex, int _timeout);
static ADEC_MUTEX_RESULT_T AlwaysSuccessMutex_Release(void* _mutex);

static ADEC_MUTEX_T gAlwaysSuccessMutex = {
	AlwaysSuccessMutex_Acqire,
	AlwaysSuccessMutex_Release
};

ADEC_MUTEX_T* DEFAULT_MUTEX = &gAlwaysSuccessMutex;


static ADEC_MUTEX_RESULT_T AlwaysSuccessMutex_Acqire(void* _mutex, int _timeout)
{
	return ADEC_MUTEX_OK;
}

static ADEC_MUTEX_RESULT_T AlwaysSuccessMutex_Release(void* _mutex)
{
	return ADEC_MUTEX_OK;
}
