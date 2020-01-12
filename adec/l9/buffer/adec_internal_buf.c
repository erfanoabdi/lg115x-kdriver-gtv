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

#include "adec_buf_cfg.h"
#include "adec_internal_buf.h"
#include "adec_common_buf.h"
#include "adec_buf.h"
#include "adec_buf_dbg.h"
#include "../adec_common/adec_common_err.h"


///////////////////////////////////////////////////////////////////////////////
// Macro Definition
///////////////////////////////////////////////////////////////////////////////

#ifndef NULL
#define NULL 0
#endif

///////////////////////////////////////////////////////////////////////////////
// Type Definition
///////////////////////////////////////////////////////////////////////////////
typedef struct _ADEC_INTERNAL_BUFFER_T
{
	ADEC_BUF_T					super;
	ADEC_COMMON_BUF_STATUS_T	status;
	ADEC_COMMON_BUF_T			commonBuf;
}ADEC_INTERNAL_BUFFER_T;




///////////////////////////////////////////////////////////////////////////////
// Static variables
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Local Function Declaration
///////////////////////////////////////////////////////////////////////////////

static int InternalBuffer_Init(void* _buf);
	
static int InternalBuffer_GetFreeSize(void* _buf);

static int InternalBuffer_GetUsedSize(void* _buf);

static int InternalBuffer_GetAuCnt(void* _buf);

static int InternalBuffer_GetMaxSize(void* _buf);

static int InternalBuffer_GetMaxAu(void* _buf);

static int InternalBuffer_GetMaxAuLength(void* _buf);

static int InternalBuffer_CreateNewAu(
	ADEC_AU_INFO_T* _info, 
	void* _buf);

static int InternalBuffer_WriteData(
	unsigned char* _src, 
	int _length, 
	void* _buf);

static int InternalBuffer_InsertAu(
	unsigned char* _src, 
	int _length, 
	ADEC_AU_INFO_T* info, 
	void* _buf);

static int InternalBuffer_UpdateAu(
	ADEC_AU_INFO_T* info,
	void* _buf);

static int InternalBuffer_GetAuInfo(
	int index, 
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf);

static int InternalBuffer_GetCurrentInfo(
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf);

static int InternalBuffer_ReadData(
	_OUT_ unsigned char** _dst, 
	_IN_OUT_ int* _size,
	_OUT_ int* _au_in_data,
	void* _buf);

static int InternalBuffer_FlushData(
	int _size, 
	void* _buf);

static int InternalBuffer_ReadAu(
	_OUT_ unsigned char** _dst,
	_OUT_ int* _size,
	_IN_OUT_ int* _au_cnt,
	void* _buf);

static int InternalBuffer_SetRepeat(int _cnt,
	void* _buf);

///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////
int InternalBuffer_GetStructSize(int _indexCount)
{
	// ADEC_INTERNAL_BUFFER_T already contains 1-length index table.
	return (sizeof(ADEC_INTERNAL_BUFFER_T) + CommonBuffer_GetStructSize(_indexCount));
}

int InternalBuffer_CreateBuffer(
	unsigned char* _baseAddr, 
	int _bufferSize, 
	int _maxAuSize, 
	int _indexCount,
	void* _struct)
{
	ADEC_INTERNAL_BUFFER_T* intBuf;
	int actualBufSize;
	int structSize;
	int result;

	ADEC_BUF_ASSERT(_baseAddr != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_baseAddr should not be null.\n");

	actualBufSize = _bufferSize;

	if(_struct == NULL)
	{
		structSize = InternalBuffer_GetStructSize(_indexCount);
		actualBufSize -= structSize;
		intBuf = (ADEC_INTERNAL_BUFFER_T*) _baseAddr;
		_baseAddr += structSize;
	}
	else
	{
		intBuf = (ADEC_INTERNAL_BUFFER_T*) _struct;
	}

	if(actualBufSize < 0)
	{
		return ADEC_ERR_ARG_RANGE;
	}

	result = CommonBuffer_Create(_baseAddr, 
		actualBufSize, 
		_maxAuSize, 
		_indexCount, 
		DEFAULT_PROCESSOR_ADAP,
		&intBuf->commonBuf,
		&intBuf->status);
	if( result != ADEC_ERR_NONE)
	{
		return result;
	}

	intBuf->super.create_new_au = InternalBuffer_CreateNewAu;
	intBuf->super.flush_data = InternalBuffer_FlushData;
	intBuf->super.get_au_cnt = InternalBuffer_GetAuCnt;
	intBuf->super.get_au_info = InternalBuffer_GetAuInfo;
	intBuf->super.get_current_info = InternalBuffer_GetCurrentInfo;
	intBuf->super.get_free_size = InternalBuffer_GetFreeSize;
	intBuf->super.get_max_au = InternalBuffer_GetMaxAu;
	intBuf->super.get_max_au_length = InternalBuffer_GetMaxAuLength;
	intBuf->super.get_max_size = InternalBuffer_GetMaxSize;
	intBuf->super.get_used_size = InternalBuffer_GetUsedSize;
	intBuf->super.init = InternalBuffer_Init;
	intBuf->super.insert_au = InternalBuffer_InsertAu;
	intBuf->super.read_au = InternalBuffer_ReadAu;
	intBuf->super.read_data = InternalBuffer_ReadData;
	intBuf->super.update_au = InternalBuffer_UpdateAu;
	intBuf->super.write_data = InternalBuffer_WriteData;

#if (ADEC_OPT_BUF_USE_REPEAT == 1)
	intBuf->super.set_repeat = InternalBuffer_SetRepeat;
#endif

	return ADEC_ERR_NONE;
}

static int InternalBuffer_Init(void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_Init(
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}
	
static int InternalBuffer_GetFreeSize(void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetFreeSize(
		DEFAULT_PROCESSOR_ADAP,  
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_GetUsedSize(void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetUsedSize(
		DEFAULT_PROCESSOR_ADAP,  
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_GetAuCnt(void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetAuCnt(
		DEFAULT_PROCESSOR_ADAP,  
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_GetMaxSize(void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetMaxSize(
		DEFAULT_PROCESSOR_ADAP,  
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_GetMaxAu(void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetMaxAu(
		DEFAULT_PROCESSOR_ADAP,  
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_GetMaxAuLength(void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetMaxAuLength(
		DEFAULT_PROCESSOR_ADAP,  
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_CreateNewAu(
	ADEC_AU_INFO_T* _info, 
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_CreateNewAu(
		_info,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_WriteData(
	unsigned char* _src, 
	int _length, 
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_WriteData(
		_src,
		_length,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_InsertAu(
	unsigned char* _src, 
	int _length, 
	ADEC_AU_INFO_T* _info, 
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_InsertAu(
		_src,
		_length,
		_info,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_UpdateAu(
	ADEC_AU_INFO_T* _info,
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_UpdateAu(
		_info,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_GetAuInfo(
	int _index, 
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetAuInfo(
		_index,
		_offset,
		_au_info,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_GetCurrentInfo(
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_GetAuInfo(
		0,
		_offset,
		_au_info,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_ReadData(
	_OUT_ unsigned char** _dst, 
	_IN_OUT_ int* _size,
	_OUT_ int* _au_in_data,
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_ReadData(
		_dst,
		_size,
		_au_in_data,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_FlushData(
	int _size, 
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_FlushData(
		&_size,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_ReadAu(
	_OUT_ unsigned char** _dst,
	_OUT_ int* _size,
	_IN_OUT_ int* _au_cnt,
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	return CommonBuffer_ReadAu(
		_dst,
		_size,
		_au_cnt,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}

static int InternalBuffer_SetRepeat(int _cnt,
	void* _buf)
{
	ADEC_BUF_ASSERT(_buf != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"_buf should not be null.\n");

	return CommonBuffer_SetRepeat(
		_cnt,
		DEFAULT_PROCESSOR_ADAP,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->commonBuf,
		&((ADEC_INTERNAL_BUFFER_T*)_buf)->status);
}
