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
 * ***************************************************************************************/


/*-----------------------------------------------------------------------------+
|    This source code has been made available to you by LGE on an AS-IS
|    basis.  Anyone receiving this source is licensed under LGE
|    copyrights to use it in any way he or she deems fit, including
|    copying it, modifying it, compiling it, and redistributing it either
|    with or without modifications.  No license under LGE patents or
|    patent applications is to be implied by the copyright license.
|
|    Any user of this software should understand that LGE cannot provide
|    technical support for this software and will not be responsible for
|    any consequences resulting from the use of this software.
|
|    Any person who transfers this source code or any derivative work
|    must include the LGE copyright notice, this paragraph, and the
|    preceding two paragraphs in the transferred software.
|
|      LGE CONFIDENTIAL
|      (C) COPYRIGHT LGE CORPORATION, 2002
+-----------------------------------------------------------------------------*/

/** @file
 *
 *  DVB-CI Command Interface Physical Layer
 *
 *  author		Srinivasan Shanmugam (srinivasan.shanmugam@lge.com) /  Hwajeong Lee (hwajeong.lee@lge.com)
 *  version	0.6
 *  date		2010.02.22
 *  note		Additional information.
 *
 *  @addtogroup lg1150_ci
 *	@{
 */

#ifndef	_CI_IO_H_
#define	_CI_IO_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "ci_regdefs.h"

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
#define HW_IO_OK			0
#define HW_IO_BUSY			1
#define HW_IO_FAIL			(-1)
#define MEM_ALLOC_FAILED	(-2)

#define	DATA_REG			0x00000000
#define	COMM_REG			0x00000002
#define	STAT_REG			0x00000002
#define	SIZE_REG_LS			0x00000004
#define	SIZE_REG_MS			0x00000006

/* status register bit definitions */
#define	CI_RE				0x01
#define	CI_WE				0x02
#define	CI_FR				0x40
#define	CI_DA				0x80
#define CI_IIR				0x10 /* for CI plus */

/* command register bit definitions	*/
#define	CI_HC				0x01
#define	CI_SW				0x02
#define	CI_SR				0x04
#define	CI_RS				0x08 // avoid collision with RS in "dtv_msgs.h"
#define	CI_FRIE				0x40
#define	CI_DAIE				0x80

/* host & module buffer size */
#define HOST_BUF_SIZE		65535
#define	MOD_BUF_SIZE_MIN	16
#define	MOD_BUF_SIZE_MAX	65535

/* maximum number of module	*/
//#define MAX_MOD_NUM		2
//#define MAX_MOD_NUM			1

/* IO register access macro */
#define BYTE(addr)				*((volatile unsigned char *) (addr))
#define CONFIG_REG(addr)		*((volatile unsigned char *) (addr))



#define STAT_RD(mId)			BYTE(CiModBaseAddr[mId]  + STAT_REG)
#define	DATA_RD(mId)			BYTE(CiModBaseAddr[mId]  + DATA_REG)
#define	SIZE_LS_RD(mId)			BYTE(CiModBaseAddr[mId]  + SIZE_REG_LS)
#define	SIZE_MS_RD(mId)			BYTE(CiModBaseAddr[mId]  + SIZE_REG_MS)
#define	COMM_WR(mId, val)		(BYTE(CiModBaseAddr[mId] + COMM_REG) = val)
#define	DATA_WR(mId, val)		(BYTE(CiModBaseAddr[mId] + DATA_REG) = val)
#define	SIZE_LS_WR(mId, val)	(BYTE(CiModBaseAddr[mId] + SIZE_REG_LS) = val)
#define	SIZE_MS_WR(mId, val)	(BYTE(CiModBaseAddr[mId] + SIZE_REG_MS) = val)



#define CHECK_DA(mId)			(STAT_RD(mId)&CI_DA)
#define CHECK_FR(mId)			(STAT_RD(mId)&CI_FR)
#define CHECK_RE(mId)			(STAT_RD(mId)&CI_RE)
#define CHECK_WE(mId)			(STAT_RD(mId)&CI_WE)

/* for CI plus */
#define CHECK_IIR(mId)			(STAT_RD(mId)&CI_IIR)


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
/* mapping MOD_ID to base address */
/* this variable is declared in "buffnego.c" */
extern unsigned long	CiModBaseAddr[MAX_MOD_NUM];

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int	HW_IO_ResetSoft(MOD_ID mId);
int HW_IO_NegoBuf( MOD_ID mId, UINT32 *o_pwBufSize );
int	HW_IO_Read( MOD_ID mId, UINT8 *o_pbyData, UINT16 *io_pwDataBufSize );
int HW_IO_Write( MOD_ID mId ,UINT8 *i_pbyData, UINT32 i_wDataBufSize );

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _CI_IO_H_ */
