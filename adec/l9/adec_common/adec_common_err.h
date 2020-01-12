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

#ifndef _ADEC_ERR_H_
#define _ADEC_ERR_H_


/// No Error
#define ADEC_ERR_NONE			0
/// Some parameter, should not be null, is null.
#define ADEC_ERR_NULL_PARAM		-1
/// Not specified. But error.
#define ADEC_ERR_GENERIC		-2
/// Not implemented yet.
#define ADEC_ERR_NOT_IMPLMENTED	-3
/// Data is broken. Check if some module overwrite data area or current module has bug.
#define ADEC_ERR_DATA_CORRUPTED	-4
/// Argument range is not acceptible
#define ADEC_ERR_ARG_RANGE		-5
/// Fail to acquire or releas mutex
#define ADEC_ERR_MUTEX			-6
/// Not enough memory
#define ADEC_ERR_NOT_ENOUGH_MEM -7
/// Not supported action
#define ADEC_ERR_NOT_SUPPROTED	-8
/// Duplicated data.
#define ADEC_ERR_DUPPLICATED	-9
/// Can not find item.
#define ADEC_ERR_NOT_FOUND		-10
/// No data remain to process. Buffer underflow.
#define ADEC_ERR_UNDERFLOW		-200
/// Not enough buffer space to store data. Buffer overflow.
#define ADEC_ERR_OVERFLOW		-201
/// Has no permission for current operation.
#define ADEC_ERR_PERMISSION		-300
/// During repeat, write operation is denied.
#define ADEC_ERR_REPEAT			-400
/// Exeeded max number of client of the multi sink buffer
#define ADEC_ERR_MAX_CLIENT		-401
/// This client is not registered.
#define ADEC_ERR_NOT_REGISTERED	-402
/// This client is already registered.
#define ADEC_ERR_REGISTERED_ALREADY	-403

#define ADEC_ERR_WAITING	-800

#endif
