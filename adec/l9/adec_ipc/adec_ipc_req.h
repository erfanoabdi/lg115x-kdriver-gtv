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

/** @file
 *
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     youngki.lyu@lge.com
 * version    1.0
 * date       2011.05.08
 * note       Additional information.
 *
 */

#ifndef _ADEC_IPC_REQ_H_
#define _ADEC_IPC_REQ_H_


#include "base_types.h"


#ifdef __cplusplus
extern "C" {
#endif



typedef void (*ADEC_IPC_REQ_RECEIVER_CALLBACK_T)(unsigned int, unsigned int, void *);



void ADEC_IPC_REQ_Init(void);

#ifdef __XTENSA__
BOOLEAN ADEC_IPC_REQ_Send(UINT32 ui32HdrId, UINT32 ui32BodySize, void *pIpcBody);
#else
BOOLEAN ADEC_IPC_REQ_Register_ReceivingCallback(UINT32 ui32HdrId, ADEC_IPC_REQ_RECEIVER_CALLBACK_T fpCallback);
UINT32 ADEC_IPC_REQ_Receive(void);
#endif



#ifdef __cplusplus
}
#endif

#endif /* _ADEC_IPC_REQ_H_ */

