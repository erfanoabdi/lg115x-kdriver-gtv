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

#ifndef _ADEC_MUTEX_H_
#define _ADEC_MUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif
#define ADEC_MUTEX_WAIT_INFINITE	0x7FFFFFFF
///////////////////////////////////////////////////////////////////////////////
// Type Definition
///////////////////////////////////////////////////////////////////////////////
	typedef enum _ADEC_MUTEX_RESULT_T
	{
		ADEC_MUTEX_OK,
		ADEC_MUTEX_ERR,
		ADEC_MUTEX_TIMEOUT,
	}ADEC_MUTEX_RESULT_T;

	typedef struct _ADEC_MUTEX_T
	{
		ADEC_MUTEX_RESULT_T (*acqire)(void* _mutex, int _timeout);
		ADEC_MUTEX_RESULT_T (*release)(void* _mutex);
	}ADEC_MUTEX_T;

///////////////////////////////////////////////////////////////////////////////
// Extern variables
///////////////////////////////////////////////////////////////////////////////
	extern ADEC_MUTEX_T* DEFAULT_MUTEX;

#ifdef __cplusplus
}
#endif

#endif
