/***************************************************************************************
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
 *  This file.......
 *
 *  author		Juhee Park
 *  version		1.0
 *  date		2011.6.11
 *  note		Additional information.
 *
 *
 */

#ifndef  _TZ_IF_H_
#define  _TZ_IF_H_

/*----------------------------------------------------------------------------------------
 Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

#ifdef   __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
 Constant Definitions
----------------------------------------------------------------------------------------*/
#define TZ_IF_VER			20120209

#define TZ_CHIP_REV_L9_A0	0
#define TZ_CHIP_REV_L9_A1	1
#define TZ_CHIP_REV_L9_B0	2
#define TZ_CHIP_REV_L9_B1	3
#define TZ_CHIP_REV_H13_A0	4

#define TZ_ADC_SIZE			12

/*----------------------------------------------------------------------------------------
 Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum {
	TZ_SUCCESS					= 8000,
	TZ_TIMER_SCHED				= TZ_SUCCESS + 4,

	TZ_FAIL						= -8000,
	TZ_FAIL_VERSION_MISMATCH 	= TZ_FAIL - 1,
	TZ_FAIL_NOT_SUPPORTED_CHIP	= TZ_FAIL - 2,
	TZ_FAIL_WRONG_CMD			= TZ_FAIL - 3,
	TZ_FAIL_NOT_IMPLEMENTED		= TZ_FAIL - 4,
	TZ_FAIL_ILLEGAL_ARGUMENT	= TZ_FAIL - 5,
	TZ_FAIL_BUSY 				= TZ_FAIL - 6,
} TZ_RETURN_T;

/*
* NWd -> SWd command.
**/
typedef enum {
	TZ_CMD_L2CC						= 0x01,
	TZ_CMD_INIT_IF					= 0x02,
	TZ_CMD_EXIT_IF					= 0x03,
	TZ_CMD_LOCK_DEBUG				= 0x04,
	TZ_CMD_UNLOCK_DEBUG				= 0x05,

	TZ_CMD_RESET					= 0x10,
	TZ_CMD_SET_KEY					= 0x11,
	TZ_CMD_SET_IV					= 0x12,

	TZ_CMD_GET_RANDOM_NUMBER		= 0x18,

	TZ_CMD_GET_VERIFICATION			= 0x20,
	TZ_CMD_SET_VERIFICATION			= 0x21,
	TZ_CMD_GET_ADC					= 0x22,
	TZ_CMD_SET_ADC					= 0x23,
	TZ_CMD_SET_ASC					= 0x24,

	TZ_CMD_RUN_BLOCK_OP				= 0x60,
	TZ_CMD_RUN_HASH_OP 				= 0x61,
	TZ_CMD_RUN_RSA_OP				= 0x62,

	TZ_CMD_SET_SECURE_DATA			= 0x70,
	TZ_CMD_GET_SECURE_DATA			= 0x71,
	TZ_CMD_TRF_SECURE_DATA			= 0x72,

	TZ_CMD_NF_DRMAGENT				= 0x80,

	TZ_CMD_TRUST_OPERATION			= 0x90,

	TZ_CMD_PM_SUSPEND				= 0xe1,
	TZ_CMD_PM_RESUME				= 0xe2,

	/* Debugging CMDs */
	TZ_CMD_INTR_TEST				= 0xF1,
	TZ_CMD_TZTIMER_TICK_SCHED_TEST	= 0xF2,
} TZ_CMD_T;

typedef struct _TZ_MEM_tag {
	/* Non Cached Area */
	unsigned int ncBase;
	unsigned int ncSize;

	/* Cached Area */
	unsigned int cBase;
	unsigned int cSize;

	/* DMA Info */
	unsigned int *pDMAInfo;
} TZ_MEM_T;

/*
* Structure for holding values to go in registers r0-r7 before SMC issue.
**/
typedef struct _TZ_ARG_tag {
	TZ_CMD_T cmd_r0;	/*command type*/
	unsigned int arg_r1;
	unsigned int arg_r2;
	unsigned int arg_r3;
	unsigned int arg_r4;
	unsigned int arg_r5;
	unsigned int arg_r6;
	unsigned int arg_r7;
} TZ_ARG_T;

typedef struct _TZ_ASC_tag {
	unsigned int ascChannel;
	unsigned int regionNumber;
	unsigned int regionEnable;
	unsigned int permition;
	unsigned int baseAddr;
	unsigned int regionSize;
	unsigned int subregionDisable;
} TZ_ASC_T;

/*----------------------------------------------------------------------------------------
 Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern void TZ_IOCTL(TZ_ARG_T *pTzArg);

/*----------------------------------------------------------------------------------------
 Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef   __cplusplus
}
#endif /* __cplusplus */

#endif /* _TZ_IF_H_ */

/** @} */

