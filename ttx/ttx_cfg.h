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
 *  main configuration file for ttx device
 *	ttx device will teach you how to make device driver with new platform.
 *
 *  author		wonsik.do (wonsik.do@lge.com)
 *  version		1.0
 *  date		2010.01.06
 *  note		Additional information.
 *
 *  @addtogroup lg1150_ttx
 *	@{
 */

#ifndef	_TTX_CFG_H_
#define	_TTX_CFG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	TTX_MODULE			"ttx"
#define TTX_MAX_DEVICE		1

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
//#define	TTX_ALL_MEMORY_START	0x25a00000
//#define	TTX_ALL_MEMORY_SIZE		0x40000

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
	/*
typedef struct
{
	char* 	chip_name;

	char* 	pcMemoryName;
	UINT32	uiMemoryBase;
	UINT32 	uiMemorySize;

	char*	pcRegName;
	UINT32	uiRegBase;
	UINT32	uiRegSize;
} LX_TTX_MEM_CFG_S_T;
*/

/* Structure for VBI H/W FIXED Buffer Memory */
typedef struct
{
	char*       		vbi_register_name;
	unsigned int        vbi_register_base;
	unsigned int        vbi_register_size;
} LX_VBI_REG_CFG_T;

/* Structure for S/W Ring Buffer Memory */
typedef struct
{
	char*       		buf_memory_name;
	unsigned int        buf_memory_base;
	unsigned int        buf_memory_size;
} LX_VBI_MEM_CFG_T;

//extern LX_TTX_MEM_CFG_S_T gstMemCfgTTXRev[];
extern LX_VBI_MEM_CFG_T* gstMemCfgTTX;
extern LX_VBI_MEM_CFG_T gstBufMemCfgTTXRev[] ;
extern LX_VBI_REG_CFG_T* gstRegCfgTTX;
extern LX_VBI_REG_CFG_T gstVbiRegCfgTTXRev[] ;
/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _TTX_CFG_H_ */

/** @} */

