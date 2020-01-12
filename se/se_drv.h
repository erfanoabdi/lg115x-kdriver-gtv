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
 *  driver interface header for se device. ( used only within kdriver )
 *  this file lists the type, function lists exported to the other module outside SE.
 *
 *  @author		eunjoo.kim (eunjoo.kim@lge.com)
 *  @version	1.0
 *  @date		2010.03.24
 *
 *  @addtogroup lg1150_se
 *	@{
 */

#ifndef	_SE_DRV_H_
#define	_SE_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "se_kapi.h"
#include "se_cfg.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define SE_PRINT(fmt, args...)   DBG_PRINT( g_se_debug_fd, 0, fmt, ##args)
#define SE_TRACE(fmt, args...)   DBG_PRINT( g_se_debug_fd, 1, fmt, ##args)
#define SE_WARN(fmt, args...)    DBG_PRINT( g_se_debug_fd, 2, "[SE-WRN] %s(%d):" fmt, __F__, __L__, ##args)
#define SE_ERROR(fmt, args...)   DBG_PRINT( g_se_debug_fd, 3, "[SE-ERR] %s(%d):" fmt, __F__, __L__, ##args)

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum {
	SE_SUCCESS		= 0,
	SE_FAIL			= -1,
} SE_RETURN_T;

typedef struct _SE_tag {
	UINT32 bInit;
	UINT32 chip;
	UINT32 platform;
	UINT8 *pBuffer;
	UINT32 bufferAddr;
	UINT32 bufferSize;

	SE_RETURN_T (*Reset)(void);
	SE_RETURN_T (*LockDebug)(LX_SE_DEBUG_T lock);
	SE_RETURN_T (*UnlockDebug)(LX_SE_DEBUG_T unlock);
	SE_RETURN_T	(*InitChip)(UINT32 chip);
	SE_RETURN_T (*GetRandomNR)(LX_SE_DATA_T *pRequest);

	SE_RETURN_T (*SetVerification)(void);
	SE_RETURN_T (*GetVerification)(UINT32 *pVerificationInfo);
	SE_RETURN_T (*SetADC)(UINT8 *pADC);
	SE_RETURN_T (*GetADC)(UINT8 *pADC);

	SE_RETURN_T (*SetKey)(LX_SE_DATA_T *pRequest);
	SE_RETURN_T (*SetIV)(LX_SE_DATA_T *pRequest);
	SE_RETURN_T (*SetSecureData)(LX_SE_SECURE_DATA_T *pRequest);
	SE_RETURN_T (*GetSecureData)(LX_SE_SECURE_DATA_T *pRequest);
	SE_RETURN_T (*TransformSecureData)(LX_SE_SECURE_DATA_T *pRequest);

	SE_RETURN_T (*RunBlockOp)(LX_SE_BLOCK_OP_T *pRequest);
	SE_RETURN_T (*RunHashOp)(LX_SE_HASH_OP_T *pRequest);
	SE_RETURN_T (*RunRSAOp)(LX_SE_RSA_OP_T *pRequest);

	SE_RETURN_T (*NF_DrmAgent)(LX_SE_NF_OP_T *request);
	SE_RETURN_T (*TrustOperation)(LX_SE_TRUST_OP_T *request);
} SE_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern int SE_Init(void);
extern void SE_Cleanup(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern int	g_se_debug_fd;
extern int	g_se_trace_depth;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SE_DRV_H_ */

/** @} */
