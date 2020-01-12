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
 *  main configuration file for mjpeg device
 *	mjpeg device will teach you how to make device driver with new platform.
 *
 *  author		youngwoo.jin (youngwoo.jin@lge.com)
 *  version		1.0
 *  date		2011.04.09
 *  note		Additional information.
 *
 *  @addtogroup lg1152_mjpeg
 *	@{
 */

#ifndef	_MJPEG_CFG_H_
#define	_MJPEG_CFG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define MJPEG_ENCODE_QUALITY		3	// 0: Low, 1 - Middle low, 2 - Middle high, 3 - High
/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	MJPEG_MODULE			"mjpeg"
#define MJPEG_MAX_DEVICE		1

#if 0
#define MJPEG_REG_BASE		( L9_IOCD_BASE + 0x0000 )
#define	ICOD_REG_BASE		( L9_IOCD_BASE + 0x0C00)

#define ADDR_SW_DE_SAV		0x34030210
#define ADDR_SW_CPU_GPU		0x000200D0
#define ADDR_SW_CPU_SHADOW	0x0C010200
#endif

#define	MJPEG_TIMEOUT	100

#define MJPEG_USE_LOCK		// enable lock/unlock macro

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct
{
	char*		pcMemory_name;
	UINT32		ui32Memory_base;
	UINT32		ui32Memory_size;
} MJPEG_MEM_CFG_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern MJPEG_MEM_CFG_T g_stMemCfgMJPEG;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _MJPEG_CFG_H_ */

/** @} */

