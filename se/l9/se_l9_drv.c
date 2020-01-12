/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 ***************************************************************************************/

/** @file
 *
 *  This file has the necessary security core driver functions.
 *
 *  author		Stan.Kim
 *  version		1.0
 *  date		2012.02
 *  note		Additional information.
 *
 *  @addtogroup l9_se
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define TZ_CONFIG_USE_TIMER_SCHED	1

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <asm/io.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include "os_util.h"
#include "base_dev_cfg.h"
#include "se_drv.h"
#include "se_l9_drv.h"
#include "tz_if.h"

#if	(TZ_CONFIG_USE_TIMER_SCHED)
#include <linux/sched.h>
#endif

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define SE_MEM_BASE_GP4			0x7FD00000
#define	SE_MEM_BASE_COSMO		0x2FD00000

#define SE_MEM_SIZE				0x00200000
#define SE_MEM_PARAM_SIZE		PAGE_SIZE*4
#define SE_MEM_TZ_BUFFER_SIZE	PAGE_SIZE*4

#define SE_DMA_WAIT				1000	// 1000usec
#define SE_DMA_WAIT_RETRY		1000	// Max : SE_DMA_WAIT_RETRY * SE_DMA_WAIT = 1sec

#define SE_TZ_THREAD_PRIORITY	92

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define SE_RUN_TZ_OPERATION(cmd)		{\
									if (_runTzOperation(cmd, NULL)!=SE_SUCCESS) {\
										SE_ERROR("TZ_CMD_%s:%s\n", _get_tz_cmd_name(cmd), _pFailedMsg);\
										return SE_FAIL;\
									}\
								}

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct _MEM_tag {
	/* Whole Physical Memory */
	UINT32 base;
	UINT32 size;

	/* Non Cached Area */
	UINT32 bufferAddr;
	UINT32 bufferSize;
	UINT32 paramAddr;
	UINT32 paramSize;
	UINT32 dmaInfo;

	/* Cached Area */
	UINT32 tzBufferAddr;
	UINT32 tzBufferSize;

	/* Virtual */
	UINT8 *pBuffer;
	UINT8 *pParam;
	UINT32 *pDMAInfo;
} MEM_T;

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static MEM_T _gMemory;
static char *_pFailedMsg = "failed.";

static DECLARE_WAIT_QUEUE_HEAD(_wq_se_dma);

static struct task_struct *_gpTzOperationThread = NULL;
static OS_SEM_T	_gMutexTzOperationStart;
static OS_SEM_T	_gMutexTzOperationEnd;
static struct _TzOperation_tag {
	TZ_CMD_T tzCmd;
	TZ_ARG_T *pTzArg;
	SE_RETURN_T result;
} _gTzOperation;

/*========================================================================================
	Implementation Group
========================================================================================*/
static const char *_get_tz_cmd_name(TZ_CMD_T tzCmd)
{
	switch (tzCmd) {
		case TZ_CMD_L2CC :
			return "L2CC";
		case TZ_CMD_INIT_IF :
			return "INIT_IF";
		case TZ_CMD_EXIT_IF :
			return "EXIT_IF";
		case TZ_CMD_LOCK_DEBUG :
			return "LOCK_DEBUG";
		case TZ_CMD_UNLOCK_DEBUG :
			return "UNLOCK_DEBUG";

		case TZ_CMD_RESET :
			return "RESET";
		case TZ_CMD_SET_KEY :
			return "SET_KEY";
		case TZ_CMD_SET_IV :
			return "SET_IV";

		case TZ_CMD_GET_RANDOM_NUMBER :
			return "GET_RANDOM_NUMBER";

		case TZ_CMD_GET_VERIFICATION :
			return "GET_VERIFICATION";
		case TZ_CMD_SET_VERIFICATION :
			return "SET_VERIFICATION";
		case TZ_CMD_GET_ADC :
			return "GET_ADC";
		case TZ_CMD_SET_ADC :
			return "SET_ADC";
		case TZ_CMD_SET_ASC :
			return "SET_ASC";

		case TZ_CMD_RUN_BLOCK_OP :
			return "RUN_BLOCK_OP";
		case TZ_CMD_RUN_HASH_OP :
			return "RUN_HASH_OP";
		case TZ_CMD_RUN_RSA_OP :
			return "RUN_RSA_OP";

		case TZ_CMD_SET_SECURE_DATA :
			return "SET_SECURE_DATA";
		case TZ_CMD_GET_SECURE_DATA :
			return "GET_SECURE_DATA";
		case TZ_CMD_TRF_SECURE_DATA :
			return "TRF_SECURE_DATA";

		case TZ_CMD_NF_DRMAGENT :
			return "NF_DRMAGENT";

		case TZ_CMD_TRUST_OPERATION :
			return "TRUST_OPERATION";

		case TZ_CMD_PM_SUSPEND :
			return "PM_SUSPEND";
		case TZ_CMD_PM_RESUME :
			return "PM_RESUME";

		/* Debugging CMDs */
		case TZ_CMD_INTR_TEST :
			return "INTR_TEST";
		case TZ_CMD_TZTIMER_TICK_SCHED_TEST :
			return "TZTIMER_TICK_SCHED_TEST";

		default :
			break;
	}
	return "UNKNOWN";
}

static SE_RETURN_T _run_tz_cmd(TZ_CMD_T tzCmd, TZ_ARG_T *pTzArg)
{
	TZ_ARG_T tzArg = {0};

	if (pTzArg==NULL) {
		tzArg.cmd_r0 = tzCmd;
		pTzArg = &tzArg;
	}

#if	(TZ_CONFIG_USE_TIMER_SCHED)
	do {
		TZ_IOCTL(pTzArg);
		if ((TZ_RETURN_T)pTzArg->cmd_r0==TZ_TIMER_SCHED) {
			/*
			; Set current process state to interruptible sleep.
			*/
			set_current_state(TASK_INTERRUPTIBLE);

			/*
			; Take a nap and wake up after specified time.
			*/
#if 0
			if (schedule_timeout(msecs_to_jiffies(2))<0)	// Just Check
				SE_ERROR("Negative return in schedule_timeout()\n");
#else
			yield();
#endif
		}
	} while ((TZ_RETURN_T)pTzArg->cmd_r0==TZ_TIMER_SCHED);
#else
	TZ_IOCTL(pTzArg);
#endif

	switch ((TZ_RETURN_T)pTzArg->cmd_r0) {
		case TZ_SUCCESS :
			return SE_SUCCESS;

		case TZ_FAIL :
			SE_ERROR("TZ_FAIL\n");
			break;

		case TZ_FAIL_VERSION_MISMATCH :
			SE_ERROR("TZ_FAIL_VERSION_MISMATCH - Update sedrv and tzfw.\n");
			break;

		case TZ_FAIL_NOT_SUPPORTED_CHIP	:
			SE_ERROR("TZ_FAIL_NOT_SUPPORTED_CHIP\n");
			break;

		case TZ_FAIL_WRONG_CMD :
			SE_ERROR("TZ_FAIL_WRONG_CMD\n");
			break;

		case TZ_FAIL_NOT_IMPLEMENTED :
			SE_ERROR("TZ_FAIL_NOT_IMPLEMENTED\n");
			break;

		case TZ_FAIL_ILLEGAL_ARGUMENT :
			SE_ERROR("TZ_FAIL_ILLEGAL_ARGUMENT\n");
			break;

		case TZ_FAIL_BUSY :
			SE_ERROR("TZ_FAIL_BUSY\n");
			break;

		default :
			SE_ERROR("Unknown(%d)\n", pTzArg->cmd_r0);
			break;
	}

	return SE_FAIL;
}

static SE_RETURN_T _wait_SC_DMA_Operation(void)
{
	int retry;

#if 0
	for (retry=0; retry<SE_DMA_WAIT_RETRY; retry++) {
		if (*(_gMemory.pDMAInfo)==0)
			return SE_SUCCESS;
		wait_event_interruptible_timeout(_wq_se_dma, (*(_gMemory.pDMAInfo)==0), usecs_to_jiffies(SE_DMA_WAIT));
	}
#else
	for (retry=0; retry<SE_MEM_SIZE; retry++) {
		if (*(_gMemory.pDMAInfo)==0)
			return SE_SUCCESS;
		yield();
	}
#endif
	SE_ERROR("DMA Timeout!\n");

	return SE_FAIL;
}

static int _tzOperationThread(void *pArg)
{
	while (!kthread_should_stop()) {
		OS_LockMutex(&_gMutexTzOperationStart);

		_gTzOperation.result = _run_tz_cmd(_gTzOperation.tzCmd, _gTzOperation.pTzArg);

		OS_UnlockMutex(&_gMutexTzOperationEnd);
	}
	SE_ERROR("Stop thread\n");

	return 0;
}

static int _runTzOperation(TZ_CMD_T tzCmd, TZ_ARG_T *pTzArg)
{
	_gTzOperation.tzCmd = tzCmd;
	_gTzOperation.pTzArg = pTzArg;
	OS_UnlockMutex(&_gMutexTzOperationStart);
	OS_LockMutex(&_gMutexTzOperationEnd);

	return _gTzOperation.result;
}

static SE_RETURN_T _Reset(void)
{
	SE_RUN_TZ_OPERATION(TZ_CMD_RESET);

    return SE_SUCCESS;
}

static SE_RETURN_T _GetRandomNR(LX_SE_DATA_T *pRequest)
{
	UINT32 *pLength;

	pLength = (int *)_gMemory.pParam;
	*pLength = pRequest->length;

	SE_RUN_TZ_OPERATION(TZ_CMD_GET_RANDOM_NUMBER);

	if ((pRequest->pData != NULL)&&(pRequest->length > 0)) {
		if (copy_to_user((void __user *)pRequest->pData, (void *)(pLength+1), pRequest->length)!=0)
			return SE_FAIL;
	}

    return SE_SUCCESS;
}

static SE_RETURN_T _SetKey(LX_SE_DATA_T *pRequest)
{
	UINT8 *pData;

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_DATA_T));
	pData = _gMemory.pParam + sizeof(LX_SE_DATA_T);
	if ((pRequest->pData != NULL)&&(pRequest->length > 0)) {
		if (copy_from_user((void *)pData, (void __user *)pRequest->pData, pRequest->length)!=0)
			return SE_FAIL;
	}

	SE_RUN_TZ_OPERATION(TZ_CMD_SET_KEY);

	return SE_SUCCESS;
}

static SE_RETURN_T _SetIV(LX_SE_DATA_T *pRequest)
{
	UINT8 *pData;

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_DATA_T));
	pData = _gMemory.pParam + sizeof(LX_SE_DATA_T);
	if ((pRequest->pData != NULL)&&(pRequest->length > 0)) {
		if (copy_from_user((void *)pData, (void __user *)pRequest->pData, pRequest->length)!=0)
			return SE_FAIL;
	}

	SE_RUN_TZ_OPERATION(TZ_CMD_SET_IV);

	return SE_SUCCESS;
}

static SE_RETURN_T _SetSecureData(LX_SE_SECURE_DATA_T *pRequest)
{
	UINT8 *pData;
	LX_SE_SECURE_DATA_T *pSeucreData;

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_SECURE_DATA_T));
	pSeucreData = (LX_SE_SECURE_DATA_T *)_gMemory.pParam;
	pData = _gMemory.pParam + sizeof(LX_SE_SECURE_DATA_T);
	if ((pRequest->pData != NULL)&&(pRequest->length > 0)) {
		if (copy_from_user((void *)pData, (void __user *)pRequest->pData, pRequest->length)!=0)
			goto _ErrorReturn;
	}

	SE_RUN_TZ_OPERATION(TZ_CMD_SET_SECURE_DATA);

	pRequest->setLength = pSeucreData->setLength;	// Can be different
	if ((pRequest->pReturnData != NULL)&&(pRequest->returnLength > 0)) {
		if (copy_to_user((void __user *)pRequest->pReturnData, (void *)pData, pRequest->returnLength)!=0)
			goto _ErrorReturn;
	}

	return SE_SUCCESS;

_ErrorReturn :
	return SE_FAIL;
}

static SE_RETURN_T _GetSecureData(LX_SE_SECURE_DATA_T *pRequest)
{
	UINT8 *pData;
	LX_SE_SECURE_DATA_T *pSeucreData;

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_SECURE_DATA_T));
	pSeucreData = (LX_SE_SECURE_DATA_T *)_gMemory.pParam;
	pData = _gMemory.pParam + sizeof(LX_SE_SECURE_DATA_T);

	SE_RUN_TZ_OPERATION(TZ_CMD_GET_SECURE_DATA);

	if ((pRequest->pData != NULL)&&(pRequest->length > 0)) {
		if (copy_to_user((void __user *)pRequest->pData, (void *)pData, pRequest->length)!=0)
			goto _ErrorReturn;
		pData += pRequest->length;
	}
	pRequest->setLength = pSeucreData->setLength;	// Can be different
	if ((pRequest->pReturnData != NULL)&&(pRequest->returnLength > 0)) {
		if (copy_to_user((void __user *)pRequest->pReturnData, (void *)pData, pRequest->returnLength)!=0)
			goto _ErrorReturn;
	}

	return SE_SUCCESS;

_ErrorReturn :
	return SE_FAIL;
}

static SE_RETURN_T _TransformSecureData(LX_SE_SECURE_DATA_T *pRequest)
{
	UINT8 *pData;
	LX_SE_SECURE_DATA_T *pSeucreData[2];

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_SECURE_DATA_T)*2);
	pSeucreData[0] = (LX_SE_SECURE_DATA_T *)_gMemory.pParam;
	pSeucreData[1] = (LX_SE_SECURE_DATA_T *)(_gMemory.pParam+sizeof(LX_SE_SECURE_DATA_T));
	pData = _gMemory.pParam + sizeof(LX_SE_SECURE_DATA_T)*2;

	SE_RUN_TZ_OPERATION(TZ_CMD_TRF_SECURE_DATA);

	// SecureData[0]
	if ((pRequest->pData != NULL)&&(pRequest->length > 0)) {
		if (copy_to_user((void __user *)pRequest->pData, (void *)pData, pRequest->length)!=0)
			goto _ErrorReturn;
		pData += pRequest->length;
	}
	pRequest->setLength = pSeucreData[0]->setLength;	// Can be different
	if ((pRequest->pReturnData != NULL)&&(pRequest->returnLength > 0)) {
		if (copy_to_user((void __user *)pRequest->pReturnData, (void *)pData, pRequest->returnLength)!=0)
			goto _ErrorReturn;
		pData += pRequest->returnLength;
	}
	// SecureData[1]
	pRequest++;
	if ((pRequest->pData != NULL)&&(pRequest->length > 0)) {
		if (copy_to_user((void __user *)pRequest->pData, (void *)pData, pRequest->length)!=0)
			goto _ErrorReturn;
		pData += pRequest->length;
	}
	pRequest->setLength = pSeucreData[1]->setLength;	// Can be different
	if ((pRequest->pReturnData != NULL)&&(pRequest->returnLength > 0)) {
		if (copy_to_user((void __user *)pRequest->pReturnData, (void *)pData, pRequest->returnLength)!=0)
			goto _ErrorReturn;
		pData += pRequest->returnLength;
	}

	return SE_SUCCESS;

_ErrorReturn :
	return SE_FAIL;
}

static SE_RETURN_T _RunBlockOp(LX_SE_BLOCK_OP_T *pRequest)
{
	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_BLOCK_OP_T));

	SE_RUN_TZ_OPERATION(TZ_CMD_RUN_BLOCK_OP);

	switch (pRequest->cryptoType) {
		case LX_SE_BYPASS :
		case LX_SE_DES :
		case LX_SE_TDES :
		case LX_SE_AES :
			return _wait_SC_DMA_Operation();

		default :
			break;
	}

	return SE_SUCCESS;
}

static SE_RETURN_T _RunHashOp(LX_SE_HASH_OP_T *pRequest)
{
	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_HASH_OP_T));

	SE_RUN_TZ_OPERATION(TZ_CMD_RUN_HASH_OP);

	switch (pRequest->cryptoType) {
		case LX_SE_AES :
		case LX_SE_MULTISWAP :
		case LX_SE_RC4 :
		case LX_SE_SHA :
		case LX_SE_HMAC_SHA :
		case LX_SE_MD5 :
		case LX_SE_HMAC_MD5 :
			return _wait_SC_DMA_Operation();

		default :
			break;
	}

	return SE_SUCCESS;
}

static SE_RETURN_T _RunRSAOp(LX_SE_RSA_OP_T *pRequest)
{
	LX_SE_RSA_OP_T *pRSAOp;

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_RSA_OP_T));
	pRSAOp = (LX_SE_RSA_OP_T *)_gMemory.pParam;

	SE_RUN_TZ_OPERATION(TZ_CMD_RUN_RSA_OP);

	pRequest->returnLength = pRSAOp->returnLength;

	return SE_SUCCESS;
}

static SE_RETURN_T _NF_DrmAgent(LX_SE_NF_OP_T *pRequest)
{
	UINT8 *pArg;
	LX_SE_NF_OP_T *pNfOp;

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_NF_OP_T));
	pNfOp = (LX_SE_NF_OP_T *)_gMemory.pParam;
	pArg = _gMemory.pParam + sizeof(LX_SE_NF_OP_T);

	if (pRequest->argLength > 0) {
		if (copy_from_user((void*)pArg, (void __user *)pRequest->pArg, pRequest->argLength)!=0)
			goto _ErrorReturn;
	}

	SE_RUN_TZ_OPERATION(TZ_CMD_NF_DRMAGENT);

	pRequest->result = pNfOp->result;

	return SE_SUCCESS;

_ErrorReturn :
	return SE_FAIL;
}

static SE_RETURN_T _TrustOperation(LX_SE_TRUST_OP_T *pRequest)
{
	UINT8 *pArg;
	LX_SE_TRUST_OP_T *pTrustOp;

	memcpy(_gMemory.pParam, (void *)pRequest, sizeof(LX_SE_TRUST_OP_T));
	pTrustOp = (LX_SE_TRUST_OP_T *)_gMemory.pParam;
	pArg = _gMemory.pParam + sizeof(LX_SE_TRUST_OP_T);

	if (pRequest->argLength > 0) {
		if (copy_from_user((void*)pArg, (void __user *)pRequest->pArg, pRequest->argLength)!=0)
			goto _ErrorReturn;
	}

	if (pTrustOp->operation==0xffffffff)	{	// Temporal For Debug Print
		DBG_PRINT(g_se_debug_fd, 3, "[SE-KADP-ERR] %s", pArg);
		return SE_SUCCESS;
	}

	SE_RUN_TZ_OPERATION(TZ_CMD_TRUST_OPERATION);

	pRequest->result = pTrustOp->result;
	if (pRequest->argLength > 0) {
		if (copy_to_user((void __user *)pRequest->pArg, (void*)pArg, pRequest->argLength)!=0)
			goto _ErrorReturn;
	}

	return SE_SUCCESS;

_ErrorReturn :
	return SE_FAIL;
}

static SE_RETURN_T _LockDebug(LX_SE_DEBUG_T lock)
{
	LX_SE_DEBUG_T *pDebug;

	pDebug = (LX_SE_DEBUG_T *)_gMemory.pParam;
	*pDebug = lock;

	SE_RUN_TZ_OPERATION(TZ_CMD_LOCK_DEBUG);

	return SE_SUCCESS;
}

static SE_RETURN_T _UnlockDebug(LX_SE_DEBUG_T unlock)
{
	LX_SE_DEBUG_T *pDebug;

	pDebug = (LX_SE_DEBUG_T *)_gMemory.pParam;
	*pDebug = unlock;

	SE_RUN_TZ_OPERATION(TZ_CMD_UNLOCK_DEBUG);

	return SE_SUCCESS;
}

static SE_RETURN_T _SetVerification(void)
{
	SE_RUN_TZ_OPERATION(TZ_CMD_SET_VERIFICATION);

	return SE_SUCCESS;
}

static SE_RETURN_T _GetVerification(UINT32 *pVerificationInfo)
{
	SE_RUN_TZ_OPERATION(TZ_CMD_GET_VERIFICATION);

	*pVerificationInfo = *((UINT32*)_gMemory.pParam);

	return SE_SUCCESS;
}

static SE_RETURN_T _SetADC(UINT8 *pADC)
{
	memcpy((void *)_gMemory.pParam, pADC, TZ_ADC_SIZE);

	SE_RUN_TZ_OPERATION(TZ_CMD_SET_ADC);

	return SE_SUCCESS;
}

static SE_RETURN_T _GetADC(UINT8 *pADC)
{
	SE_RUN_TZ_OPERATION(TZ_CMD_GET_ADC);

	memcpy(pADC, (void *)_gMemory.pParam, TZ_ADC_SIZE);

	return SE_SUCCESS;
}

static SE_RETURN_T _InitChip(UINT32 chip)
{
	TZ_ARG_T tzArg = {0};
	TZ_MEM_T *pTzMem = NULL;

	pTzMem = (TZ_MEM_T *)_gMemory.pParam;
	pTzMem->ncBase = _gMemory.base;
	pTzMem->ncSize = SE_MEM_SIZE - SE_MEM_TZ_BUFFER_SIZE;
	pTzMem->cBase = _gMemory.tzBufferAddr;
	pTzMem->cSize = SE_MEM_TZ_BUFFER_SIZE;
	pTzMem->pDMAInfo = (unsigned int *)_gMemory.dmaInfo;

	tzArg.cmd_r0 = TZ_CMD_INIT_IF;
	tzArg.arg_r1 = TZ_IF_VER;
	tzArg.arg_r2 = _gMemory.paramAddr;

	switch (chip) {
		case LX_CHIP_REV_L9_A0 :
			tzArg.arg_r3 = TZ_CHIP_REV_L9_A0;
			break;

		case LX_CHIP_REV_L9_A1 :
			tzArg.arg_r3 = TZ_CHIP_REV_L9_A1;
			break;

		case LX_CHIP_REV_L9_B0 :
			tzArg.arg_r3 = TZ_CHIP_REV_L9_B0;
			break;

		case LX_CHIP_REV_L9_B1 :
			tzArg.arg_r3 = TZ_CHIP_REV_L9_B1;
			break;

		default :	// Last Chip Set
			tzArg.arg_r3 = TZ_CHIP_REV_L9_B1;
			break;
	}

	if (_runTzOperation(TZ_CMD_INIT_IF, &tzArg)!=SE_SUCCESS) {
		SE_ERROR("%s:%s\n", _get_tz_cmd_name(TZ_CMD_INIT_IF), _pFailedMsg);
		return SE_FAIL;
	}

	return SE_SUCCESS;
}

SE_RETURN_T SE_L9_Init(SE_T* pSE)
{
	MEM_T memory = {
		/* Whole Physical Memory */
		.base			= 0,	// + BASE
		.size			= SE_MEM_SIZE,

		/* Non Cached Area */
		.bufferAddr 	= 0,	// + BASE
		.bufferSize 	= SE_MEM_SIZE - SE_MEM_TZ_BUFFER_SIZE - SE_MEM_PARAM_SIZE,
		.paramAddr		= SE_MEM_SIZE - SE_MEM_TZ_BUFFER_SIZE - SE_MEM_PARAM_SIZE,	// + BASE
		.paramSize		= SE_MEM_PARAM_SIZE,
		.dmaInfo		= SE_MEM_SIZE - SE_MEM_TZ_BUFFER_SIZE - 4,	// + BASE

		/* Cached Area */
		.tzBufferAddr	= SE_MEM_SIZE - SE_MEM_TZ_BUFFER_SIZE,	// + BASE
		.tzBufferSize	= SE_MEM_TZ_BUFFER_SIZE,

		/* Virtual */
		.pBuffer		= NULL,
		.pParam 		= NULL,
		.pDMAInfo		= NULL,
	};
	struct sched_param param = { 0 };

	switch (lx_chip_plt()) {
		case LX_CHIP_PLATFORM_GP :
			memory.base = SE_MEM_BASE_GP4;
			break;

		case LX_CHIP_PLATFORM_COSMO :
			memory.base = SE_MEM_BASE_COSMO;
			break;

		default :
			SE_ERROR("unknown platform setting\n");
			memory.base = SE_MEM_BASE_GP4;
			break;
	}
	memory.bufferAddr += memory.base;
	memory.paramAddr += memory.base;
	memory.dmaInfo += memory.base;
	memory.tzBufferAddr += memory.base;

	memory.pBuffer = ioremap(memory.base, memory.size);
	memory.pParam = memory.pBuffer + memory.bufferSize;
	memory.pDMAInfo = (UINT32 *)(memory.pParam + memory.paramSize - 4);

	memcpy((void *)&_gMemory, (void *)&memory, sizeof(MEM_T));
	pSE->pBuffer = memory.pBuffer;
	pSE->bufferAddr = memory.bufferAddr;
	pSE->bufferSize = memory.bufferSize;

	/* Init Security core */
	pSE->InitChip				= _InitChip;
	pSE->Reset					= _Reset;
	pSE->LockDebug				= _LockDebug;
	pSE->UnlockDebug			= _UnlockDebug;
	pSE->GetRandomNR			= _GetRandomNR;

	pSE->SetVerification		= _SetVerification;
	pSE->GetVerification		= _GetVerification;
	pSE->SetADC					= _SetADC;
	pSE->GetADC					= _GetADC;

	pSE->SetKey					= _SetKey;
	pSE->SetIV					= _SetIV;
	pSE->SetSecureData			= _SetSecureData;
	pSE->GetSecureData			= _GetSecureData;
	pSE->TransformSecureData	= _TransformSecureData;

	pSE->RunBlockOp				= _RunBlockOp;
	pSE->RunHashOp				= _RunHashOp;
	pSE->RunRSAOp				= _RunRSAOp;

	pSE->NF_DrmAgent			= _NF_DrmAgent;
	pSE->TrustOperation			= _TrustOperation;

	// Sync Mutex for tz operation thread
	OS_InitMutex(&_gMutexTzOperationStart, OS_SEM_ATTR_DEFAULT);
	OS_InitMutex(&_gMutexTzOperationEnd, OS_SEM_ATTR_DEFAULT);
	OS_LockMutex(&_gMutexTzOperationStart);
	OS_LockMutex(&_gMutexTzOperationEnd);

	_gpTzOperationThread = kthread_create(_tzOperationThread, NULL, "setzkthread");
	if (IS_ERR(_gpTzOperationThread)) {
		SE_ERROR("while kthread_create()\n");
		_gpTzOperationThread = NULL;
		return -EIO;
	}
	// Bind se_tz_kthread to CPU0
	kthread_bind(_gpTzOperationThread, 0);	// Bind setzkthread to CPU0

	// Set Real-Time Priority
	param.sched_priority = SE_TZ_THREAD_PRIORITY;
	sched_setscheduler(_gpTzOperationThread, SCHED_FIFO, &param);

	// Start thread
	wake_up_process(_gpTzOperationThread);

    return SE_SUCCESS;
}

void SE_L9_Exit(SE_T* pSE)
{
	if (_runTzOperation(TZ_CMD_EXIT_IF, NULL)!=SE_SUCCESS) {	// Just Check
		SE_ERROR("Exit Error\n");
	}

	// Stop Thread
	if (_gpTzOperationThread!=NULL) {
		OS_UnlockMutex(&_gMutexTzOperationStart);
		kthread_stop(_gpTzOperationThread);
		_gpTzOperationThread = NULL;
	}
}

