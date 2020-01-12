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
#include "adec_inter_buf_writer.h"
#include "adec_buf.h"
#include "adec_buf_dbg.h"
#include "adec_buf_reg_api.h"
#include "../adec_common/adec_common_err.h"

///////////////////////////////////////////////////////////////////////////////
// Macro Definition
///////////////////////////////////////////////////////////////////////////////



#if (ADEC_OPT_BUF_USE_REG == 1)
#define UPDATE_STRUCT()		intBuf->adap->pfnInvalidate(intBuf->comBuf, sizeof(ADEC_COMMON_BUF_T), intBuf->adap)
#define WRITE_STRUCT()		intBuf->adap->pfnWriteback(intBuf->comBuf, sizeof(ADEC_COMMON_BUF_T), intBuf->adap)
#define STATUS_REG (&intBuf->status)
#else
#define UPDATE_STRUCT()
#define WRITE_STRUCT()
#define STATUS_REG (&intBuf->comBuf->status)
#endif // (ADEC_OPT_BUF_USE_REG == 1)

///////////////////////////////////////////////////////////////////////////////
// Type Definition
///////////////////////////////////////////////////////////////////////////////

typedef struct _ADEC_INTER_BUF_WRITER_T
{
	ADEC_BUF_T					super;
	ADEC_COMMON_BUF_STATUS_T	status;
	void*						regBase;
	ADEC_COMMON_BUF_T*			comBuf;
	ADEC_PROCESSOR_ADAP_IF_T*	adap;
	ADEC_MUTEX_T*				mutex;
}ADEC_INTER_BUF_WRITER_T;


///////////////////////////////////////////////////////////////////////////////
// Static variables
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Local Function Declaration
///////////////////////////////////////////////////////////////////////////////
static int InterBufWriter_Init(void* _buf);
	
static int InterBufWriter_GetFreeSize(void* _buf);

static int InterBufWriter_GetUsedSize(void* _buf);

static int InterBufWriter_GetAuCnt(void* _buf);

static int InterBufWriter_GetMaxSize(void* _buf);

static int InterBufWriter_GetMaxAu(void* _buf);

static int InterBufWriter_GetMaxAuLength(void* _buf);

static int InterBufWriter_CreateNewAu(
	ADEC_AU_INFO_T* _info, 
	void* _buf);

static int InterBufWriter_WriteData(
	unsigned char* _src, 
	int _length, 
	void* _buf);

static int InterBufWriter_InsertAu(
	unsigned char* _src, 
	int _length, 
	ADEC_AU_INFO_T* _info, 
	void* _buf);

static int InterBufWriter_UpdateAu(
	ADEC_AU_INFO_T* _info,
	void* _buf);

static int InterBufWriter_GetAuInfo(
	int index, 
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf);

static int InterBufWriter_GetCurrentInfo(
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf);

static int InterBufWriter_ReadData(
	_OUT_ unsigned char** _dst, 
	_IN_OUT_ int* _size,
	_OUT_ int* _au_in_data,
	void* _buf);

static int InterBufWriter_FlushData(
	int _size, 
	void* _buf);

static int InterBufWriter_ReadAu(
	_OUT_ unsigned char** _dst,
	_OUT_ int* _size,
	_IN_OUT_ int* _au_cnt,
	void* _buf);

static int InterBufWriter_SetRepeat(int _cnt,
	void* _buf);


///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////////////////////
int InterBufWriter_GetStructSize(void)
{
	return sizeof(ADEC_INTER_BUF_WRITER_T);
}

int InterBufWriter_CreateFromCommonBuffer(
	ADEC_MUTEX_T* _mutex,
	ADEC_PROCESSOR_ADAP_IF_T* _adapIf,
	void* _registerAddr,
	void* _commonBufferStruct,
	void* _struct)
{
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_mutex != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"Mutex should not be null.\n");
	ADEC_BUF_ASSERT(_adapIf != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"adap should not be null.\n");
	ADEC_BUF_ASSERT(_commonBufferStruct != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"common buffer should not be null.\n");
	ADEC_BUF_ASSERT(_struct != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"struct should not be null.\n");

#if (ADEC_OPT_BUF_USE_REG == 1)
	ADEC_BUF_ASSERT(_registerAddr != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"_registerAddr should not be null when you using register for inter communication\n");
#endif
	
	intBuf = (ADEC_INTER_BUF_WRITER_T*) _struct;

	intBuf->super.create_new_au = InterBufWriter_CreateNewAu;
	intBuf->super.flush_data = InterBufWriter_FlushData;
	intBuf->super.get_au_cnt = InterBufWriter_GetAuCnt;
	intBuf->super.get_au_info = InterBufWriter_GetAuInfo;
	intBuf->super.get_current_info = InterBufWriter_GetCurrentInfo;
	intBuf->super.get_free_size = InterBufWriter_GetFreeSize;
	intBuf->super.get_max_au = InterBufWriter_GetMaxAu;
	intBuf->super.get_max_au_length = InterBufWriter_GetMaxAuLength;
	intBuf->super.get_max_size = InterBufWriter_GetMaxSize;
	intBuf->super.get_used_size = InterBufWriter_GetUsedSize;
	intBuf->super.init = InterBufWriter_Init;
	intBuf->super.insert_au = InterBufWriter_InsertAu;
	intBuf->super.read_au = InterBufWriter_ReadAu;
	intBuf->super.read_data = InterBufWriter_ReadData;
	intBuf->super.update_au = InterBufWriter_UpdateAu;
	intBuf->super.write_data = InterBufWriter_WriteData;

#if (ADEC_OPT_BUF_USE_REPEAT == 1)
	intBuf->super.set_repeat = InterBufWriter_SetRepeat;
#endif

	intBuf->mutex = _mutex;
	intBuf->comBuf = (ADEC_COMMON_BUF_T*)_commonBufferStruct;
	intBuf->adap = _adapIf;
	intBuf->regBase = _registerAddr;

	InterBufWriter_Init(intBuf);

	return ADEC_ERR_NONE;
}


static int InterBufWriter_Init(void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	ADEC_BUF_ASSERT(intBuf->mutex != NULL && intBuf->mutex->acqire != NULL && intBuf->mutex->release != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"mutex and its members should not be null.\n");

	result = intBuf->mutex->acqire(intBuf->mutex, ADEC_MUTEX_WAIT_INFINITE);

	ADEC_BUF_ASSERT(result == ADEC_MUTEX_OK,
		NULL,
		NULL,
		ADEC_ERR_MUTEX,
		"fail to acquire or release mutex.\n");

	UPDATE_STRUCT();

	result = CommonBuffer_Init(
		intBuf->adap,
		intBuf->comBuf,
		STATUS_REG);

	WRITE_WRITE_INDEX();
	WRITE_READ_INDEX();
	WRITE_WRITE_OFFSET();
	WRITE_READ_OFFSET();
	WRITE_IS_ALLOCATED();

	WRITE_STRUCT();

	intBuf->mutex->release(intBuf->mutex);

	return result;
}
	
static int InterBufWriter_GetFreeSize(void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	READ_WRITE_OFFSET();
	READ_READ_OFFSET();

	UPDATE_STRUCT();

	result = CommonBuffer_GetFreeSize(
		intBuf->adap,  
		intBuf->comBuf,
		STATUS_REG);

	return result;
}

static int InterBufWriter_GetUsedSize(void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	READ_WRITE_OFFSET();
	READ_READ_OFFSET();

	UPDATE_STRUCT();

	result = CommonBuffer_GetUsedSize(
		intBuf->adap,  
		intBuf->comBuf,
		STATUS_REG);

	return result;
}

static int InterBufWriter_GetAuCnt(void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	READ_WRITE_INDEX();
	READ_READ_INDEX();

	UPDATE_STRUCT();

	result = CommonBuffer_GetAuCnt(
		intBuf->adap,  
		intBuf->comBuf,
		STATUS_REG);

	return result;
}

static int InterBufWriter_GetMaxSize(void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	UPDATE_STRUCT();

	result = CommonBuffer_GetMaxSize(
		intBuf->adap,  
		intBuf->comBuf,
		STATUS_REG);

	return result;
}

static int InterBufWriter_GetMaxAu(void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	UPDATE_STRUCT();

	result = CommonBuffer_GetMaxAu(
		intBuf->adap,  
		intBuf->comBuf,
		STATUS_REG);

	return result;
}

static int InterBufWriter_GetMaxAuLength(void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	UPDATE_STRUCT();

	result = CommonBuffer_GetMaxAuLength(
		intBuf->adap,  
		intBuf->comBuf,
		STATUS_REG);

	return result;
}

static int InterBufWriter_CreateNewAu(
	ADEC_AU_INFO_T* _info, 
	void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	ADEC_BUF_ASSERT(intBuf->mutex != NULL && intBuf->mutex->acqire != NULL && intBuf->mutex->release != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"mutex and its members should not be null.\n");

	result = intBuf->mutex->acqire(intBuf->mutex, ADEC_MUTEX_WAIT_INFINITE);

	ADEC_BUF_ASSERT(result == ADEC_MUTEX_OK,
		NULL,
		NULL,
		ADEC_ERR_MUTEX,
		"fail to acquire or release mutex.\n");

	READ_WRITE_INDEX();
	READ_READ_INDEX();
	READ_WRITE_OFFSET();
	READ_IS_ALLOCATED();

	UPDATE_STRUCT();

	result = CommonBuffer_CreateNewAu(
		_info,
		intBuf->adap,
		intBuf->comBuf,
		STATUS_REG);

	WRITE_WRITE_INDEX();
	WRITE_IS_ALLOCATED();

	intBuf->mutex->release(intBuf->mutex);

	return result;
}

static int InterBufWriter_WriteData(
	unsigned char* _src, 
	int _length, 
	void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	ADEC_BUF_ASSERT(intBuf->mutex != NULL && intBuf->mutex->acqire != NULL && intBuf->mutex->release != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"mutex and its members should not be null.\n");

	result = intBuf->mutex->acqire(intBuf->mutex, ADEC_MUTEX_WAIT_INFINITE);

	ADEC_BUF_ASSERT(result == ADEC_MUTEX_OK,
		NULL,
		NULL,
		ADEC_ERR_MUTEX,
		"fail to acquire or release mutex.\n");

	READ_WRITE_INDEX();
	READ_READ_INDEX();
	READ_IS_ALLOCATED();
	READ_WRITE_OFFSET();
	READ_READ_OFFSET();

	UPDATE_STRUCT();

	result = CommonBuffer_WriteData(
		_src,
		_length,
		intBuf->adap,
		intBuf->comBuf,
		STATUS_REG);

	WRITE_WRITE_INDEX();
	WRITE_IS_ALLOCATED();
	WRITE_WRITE_OFFSET();

	intBuf->mutex->release(intBuf->mutex);

	return result;
}

static int InterBufWriter_InsertAu(
	unsigned char* _src, 
	int _length, 
	ADEC_AU_INFO_T* _info, 
	void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	ADEC_BUF_ASSERT(intBuf->mutex != NULL && intBuf->mutex->acqire != NULL && intBuf->mutex->release != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"mutex and its members should not be null.\n");

	result = intBuf->mutex->acqire(intBuf->mutex, ADEC_MUTEX_WAIT_INFINITE);

	ADEC_BUF_ASSERT(result == ADEC_MUTEX_OK,
		NULL,
		NULL,
		ADEC_ERR_MUTEX,
		"fail to acquire or release mutex.\n");

	READ_WRITE_INDEX();
	READ_READ_INDEX();
	READ_IS_ALLOCATED();
	READ_WRITE_OFFSET();
	READ_READ_OFFSET();

	UPDATE_STRUCT();

	result = CommonBuffer_InsertAu(
		_src,
		_length,
		_info,
		intBuf->adap,
		intBuf->comBuf,
		STATUS_REG);

	WRITE_WRITE_INDEX();
	WRITE_IS_ALLOCATED();
	WRITE_WRITE_OFFSET();

	intBuf->mutex->release(intBuf->mutex);

	return result;
}

static int InterBufWriter_UpdateAu(
	ADEC_AU_INFO_T* _info,
	void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	ADEC_BUF_ASSERT(intBuf->mutex != NULL && intBuf->mutex->acqire != NULL && intBuf->mutex->release != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"mutex and its members should not be null.\n");

	intBuf->mutex->acqire(intBuf->mutex, ADEC_MUTEX_WAIT_INFINITE);

	READ_READ_INDEX();
	READ_IS_ALLOCATED();

	UPDATE_STRUCT();

	result = CommonBuffer_UpdateAu(
		_info,
		intBuf->adap,
		intBuf->comBuf,
		STATUS_REG);

	intBuf->mutex->release(intBuf->mutex);

	return result;
}

static int InterBufWriter_GetAuInfo(
	int _index, 
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf)
{
	return ADEC_ERR_PERMISSION;
}

static int InterBufWriter_GetCurrentInfo(
	_OUT_ int* _offset,
	_OUT_ ADEC_AU_INFO_T* _au_info, 
	void* _buf)
{
	return ADEC_ERR_PERMISSION;
}

static int InterBufWriter_ReadData(
	_OUT_ unsigned char** _dst, 
	_IN_OUT_ int* _size,
	_OUT_ int* _au_in_data,
	void* _buf)
{
	return ADEC_ERR_PERMISSION;
}

static int InterBufWriter_FlushData(
	int _size, 
	void* _buf)
{
	return ADEC_ERR_PERMISSION;
}

static int InterBufWriter_ReadAu(
	_OUT_ unsigned char** _dst,
	_OUT_ int* _size,
	_IN_OUT_ int* _au_cnt,
	void* _buf)
{
	return ADEC_ERR_PERMISSION;
}

static int InterBufWriter_SetRepeat(int _cnt,
	void* _buf)
{
	int result;
	ADEC_INTER_BUF_WRITER_T* intBuf;

	ADEC_BUF_ASSERT(_buf != NULL, 
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM, 
		"_buf should not be null.\n");

	intBuf = (ADEC_INTER_BUF_WRITER_T*) _buf;

	ADEC_BUF_ASSERT(intBuf->mutex != NULL && intBuf->mutex->acqire != NULL && intBuf->mutex->release != NULL,
		NULL,
		NULL,
		ADEC_ERR_NULL_PARAM,
		"mutex and its members should not be null.\n");

	intBuf->mutex->acqire(intBuf->mutex, ADEC_MUTEX_WAIT_INFINITE);

	READ_READ_INDEX();
	READ_WRITE_INDEX();
	READ_WRITE_OFFSET();
	READ_READ_OFFSET();

	UPDATE_STRUCT();

	result = CommonBuffer_SetRepeat(
		_cnt,
		intBuf->adap,
		intBuf->comBuf,
		STATUS_REG);

	WRITE_STRUCT();

	intBuf->mutex->release(intBuf->mutex);

	return result;
}
