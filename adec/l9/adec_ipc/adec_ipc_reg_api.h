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
 * date       2011.04.30
 * note       Additional information.
 *
 */

#ifndef _ADEC_IPC_REG_API_H_
#define _ADEC_IPC_REG_API_H_


#include "base_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define		ADEC_IPC_REG_INVALID_OFFSET					0xFFFFFFFF

#define		ADEC_IPC_REG_CEILING_4BYTES( _offset )			(((_offset) + 0x3) & (~0x3))



void ADEC_IPC_REG_Init(void* _cmdReg, void* _reqReg, void* _dbgReg);

UINT32 ADEC_IPC_REG_CMD_GetWrOffset(void);
void ADEC_IPC_REG_CMD_SetWrOffset(UINT32 ui32WrOffset);
UINT32 ADEC_IPC_REG_CMD_GetRdOffset(void);
void ADEC_IPC_REG_CMD_SetRdOffset(UINT32 ui32RdOffset);

UINT32 ADEC_IPC_REG_CMD_GetBaseAddr(void);
void ADEC_IPC_REG_CMD_SetBaseAddr(UINT32 ui32Cmdptr);
UINT32 ADEC_IPC_REG_CMD_GetBufSize(void);
void ADEC_IPC_REG_CMD_SetBufSize(UINT32 ui32CmdSize);

UINT32 ADEC_IPC_REG_REQ_GetWrOffset(void);
void ADEC_IPC_REG_REQ_SetWrOffset(UINT32 ui32WrOffset);
UINT32 ADEC_IPC_REG_REQ_GetRdOffset(void);
void ADEC_IPC_REG_REQ_SetRdOffset(UINT32 ui32RdOffset);

UINT32 ADEC_IPC_REG_REQ_GetBaseAddr(void);
void ADEC_IPC_REG_REQ_SetBaseAddr(UINT32 ui32Reqptr);
UINT32 ADEC_IPC_REG_REQ_GetBufSize(void);
void ADEC_IPC_REG_REQ_SetBufSize(UINT32 ui32ReqSize);

UINT32 ADEC_IPC_REG_DBG_GetWrOffset(void);
void ADEC_IPC_REG_DBG_SetWrOffset(UINT32 ui32WrOffset);
UINT32 ADEC_IPC_REG_DBG_GetRdOffset(void);
void ADEC_IPC_REG_DBG_SetRdOffset(UINT32 ui32RdOffset);

UINT32 ADEC_IPC_REG_DBG_GetBaseAddr(void);
void ADEC_IPC_REG_DBG_SetBaseAddr(UINT32 ui32DBGptr);
UINT32 ADEC_IPC_REG_DBG_GetBufSize(void);
void ADEC_IPC_REG_DBG_SetBufSize(UINT32 ui32DBGSize);





#ifdef __cplusplus
}
#endif

#endif /* _ADEC_IPC_REG_API_H_ */

