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
 * author     youngki.lyu (youngki.lyu@lge.com)
 * version    1.0
 * date       2010.04.30
 * note       Additional information.
 *
 * @addtogroup lg1152_vdec
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "adec_ipc_reg_api.h"
#include "adec_ipc_dbg.h"

#ifdef __XTENSA__
#include "adec_mcu_hal.h"
#else
#include <linux/kernel.h>
#include <asm/io.h> //ioremap
#include "../../adec_drv.h"
#endif


/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define VDEC_ADEC_IPC_REG_BASE				(L9_VDEC_BASE + 0x830)

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/
typedef struct {
	UINT32	wr_offset;	// byte offset
	UINT32	rd_offset;	// byte offset
	UINT32	base;
	UINT32	size;
} buf_base_ptr_t;

typedef struct {
	UINT32	wr_offset;	// byte offset
	UINT32	rd_offset;	// byte offset
} buf_ptr_t;

typedef struct {
   buf_base_ptr_t		cmdb;            		// 30 ~ 3C
   buf_base_ptr_t		reqb;             		// 40 ~ 4C
   buf_base_ptr_t		dbgb;             		// 50 ~ 5C
   buf_ptr_t			dispq[4];				// 60 ~ 7C
   UINT32				disp_mark[4];			// 80 ~ 8C
   UINT32				ipc_semaphore;			// 90
   UINT32				reserved0[3];			// 94 ~ 9C
   buf_ptr_t			auib[4];				// A0 ~ BC
   buf_ptr_t			cpb[4];					// C0 ~ DC
   UINT32              	reserved[88];			// E0 ~ FC
} s_reg_ipc_t;

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/

typedef struct _ADEC_IPC_REG_T
{
	unsigned int read_offset;
	unsigned int write_offset;
	unsigned int base_addr;
	unsigned int size;
}ADEC_IPC_REG_T;

static ADEC_IPC_REG_T* gDbgReg = NULL;
static ADEC_IPC_REG_T* gCmdReg = NULL;
static ADEC_IPC_REG_T* gReqReg = NULL;

static void ADEC_IPC_REG_SetDbgRegister(void* _firstRegisterAddr)
{
	gDbgReg = _firstRegisterAddr;
}
static void ADEC_IPC_REG_SetReqRegister(void* _firstRegisterAddr)
{
	gReqReg = _firstRegisterAddr;
}
static void ADEC_IPC_REG_SetCmdRegister(void* _firstRegisterAddr)
{
	gCmdReg = _firstRegisterAddr;
}

static inline UINT32 ADEC_IPC_ReadReg(void* _addr)
{
	UINT32 tmp;
	UINT32 val;
	UINT32 cnt = 32;
	tmp = val = *((UINT32*)_addr);
	
	do{
		tmp = val;
		val = *((UINT32*)_addr);
	}while(val != tmp && cnt--);
	
	return val;
}

static inline void ADEC_IPC_WriteReg(void* _addr, UINT32 _val)
{
	UINT32 tmp;
	UINT32 cnt = 32;
	
	do
	{
		*((UINT32*)_addr) = _val;
		tmp = *((UINT32*)_addr);
	}while(tmp != _val && cnt--);
}

/**
********************************************************************************
* @brief
*
* @remarks
*  DETAIL INFORMATION
* @par requirements:
*
* @param
*
* @return
*
********************************************************************************
*/
void ADEC_IPC_REG_Init(void* _cmdReg, void* _reqReg, void* _dbgReg)
{
	ADEC_IPC_REG_SetDbgRegister(_dbgReg);
	ADEC_IPC_REG_SetReqRegister(_reqReg);
	ADEC_IPC_REG_SetCmdRegister(_cmdReg);
}

UINT32 ADEC_IPC_REG_CMD_GetWrOffset(void)
{
	return ADEC_IPC_ReadReg(&(gCmdReg->write_offset));
}

void ADEC_IPC_REG_CMD_SetWrOffset(UINT32 ui32WrOffset)
{
	ADEC_IPC_WriteReg(&(gCmdReg->write_offset),ui32WrOffset);
}

UINT32 ADEC_IPC_REG_CMD_GetRdOffset(void)
{
	return ADEC_IPC_ReadReg(&(gCmdReg->read_offset));
}

void ADEC_IPC_REG_CMD_SetRdOffset(UINT32 ui32RdOffset)
{
	ADEC_IPC_WriteReg(&(gCmdReg->read_offset),ui32RdOffset);
}

UINT32 ADEC_IPC_REG_CMD_GetBaseAddr(void)
{
	return ADEC_IPC_ReadReg(&(gCmdReg->base_addr));
}

void ADEC_IPC_REG_CMD_SetBaseAddr(UINT32 ui32Cmdptr)
{
	ADEC_IPC_WriteReg(&(gCmdReg->base_addr),ui32Cmdptr);
}

UINT32 ADEC_IPC_REG_CMD_GetBufSize(void)
{
	return ADEC_IPC_ReadReg(&(gCmdReg->size));
}

void ADEC_IPC_REG_CMD_SetBufSize(UINT32 ui32CmdSize)
{
	ADEC_IPC_WriteReg(&(gCmdReg->size),ui32CmdSize);
}





UINT32 ADEC_IPC_REG_REQ_GetWrOffset(void)
{
	return ADEC_IPC_ReadReg(&(gReqReg->write_offset));
}

void ADEC_IPC_REG_REQ_SetWrOffset(UINT32 ui32WrOffset)
{
	ADEC_IPC_WriteReg(&(gReqReg->write_offset),ui32WrOffset);
}

UINT32 ADEC_IPC_REG_REQ_GetRdOffset(void)
{
	return ADEC_IPC_ReadReg(&(gReqReg->read_offset));
}

void ADEC_IPC_REG_REQ_SetRdOffset(UINT32 ui32RdOffset)
{
	ADEC_IPC_WriteReg(&(gReqReg->read_offset),ui32RdOffset);
}

UINT32 ADEC_IPC_REG_REQ_GetBaseAddr(void)
{
	return ADEC_IPC_ReadReg(&(gReqReg->base_addr));
}

void ADEC_IPC_REG_REQ_SetBaseAddr(UINT32 ui32Reqptr)
{
	ADEC_IPC_WriteReg(&(gReqReg->base_addr),ui32Reqptr);
}

UINT32 ADEC_IPC_REG_REQ_GetBufSize(void)
{
	return ADEC_IPC_ReadReg(&(gReqReg->size));
}

void ADEC_IPC_REG_REQ_SetBufSize(UINT32 ui32ReqSize)
{
	ADEC_IPC_WriteReg(&(gReqReg->size),ui32ReqSize);
}



UINT32 ADEC_IPC_REG_DBG_GetWrOffset(void)
{
	return ADEC_IPC_ReadReg(&(gDbgReg->write_offset));
}

void ADEC_IPC_REG_DBG_SetWrOffset(UINT32 ui32WrOffset)
{
	ADEC_IPC_WriteReg(&(gDbgReg->write_offset),ui32WrOffset);
}

UINT32 ADEC_IPC_REG_DBG_GetRdOffset(void)
{
	return ADEC_IPC_ReadReg(&(gDbgReg->read_offset));
}

void ADEC_IPC_REG_DBG_SetRdOffset(UINT32 ui32RdOffset)
{
	ADEC_IPC_WriteReg(&(gDbgReg->read_offset),ui32RdOffset);
}

UINT32 ADEC_IPC_REG_DBG_GetBaseAddr(void)
{
	return ADEC_IPC_ReadReg(&(gDbgReg->base_addr));
}

void ADEC_IPC_REG_DBG_SetBaseAddr(UINT32 ui32Dbgptr)
{
	ADEC_IPC_WriteReg(&(gDbgReg->base_addr),ui32Dbgptr);
}

UINT32 ADEC_IPC_REG_DBG_GetBufSize(void)
{
	return ADEC_IPC_ReadReg(&(gDbgReg->size));
}

void ADEC_IPC_REG_DBG_SetBufSize(UINT32 ui32DbgSize)
{
	ADEC_IPC_WriteReg(&(gDbgReg->size),ui32DbgSize);
}



/** @} */

