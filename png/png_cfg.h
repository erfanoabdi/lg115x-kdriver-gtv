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
 *  main configuration file for png device
 *	png device will teach you how to make device driver with new platform.
 *
 *  author		raxis.lim (raxis.lim@lge.com).lim (raxis.lim@lge.com)
 *  version		1.0
 *  date		2010.10.05
 *  note		Additional information.
 *
 *  @addtogroup lg1150_png
 *	@{
 */

#ifndef	_PNG_CFG_H_
#define	_PNG_CFG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

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
#define	PNG_MODULE			"png"
#define PNG_MAX_DEVICE		1

/* raxis.lim (2010/12/17)
 * 200 msec. test result shows that 1280x720 24bpp image should be decoded within 56 msec.
 * so timeout value 200 msec seems very reasonable
 */
#define	PNG_SYNC_TIMEOUT	50

/*	raxis.lim (2010/10/05)
 *	png doen't not have its own interrupt in L8.
 *	insteand png registers its interrupt handler to the venc module.
 */
#undef	PNG_SUPPORT_LINUX_IRQ

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct
{
    char*       memory_name;
    UINT32      memory_base;
    UINT32      memory_size;
} LX_PNG_MEM_CFG_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern LX_PNG_MEM_CFG_T 		gMemCfgPng[];
extern LX_PNG_MEM_CFG_T* 		gpMemCfgPng;

/*----------------------------------------------------------------------------------------
	Generic Usage Functions
----------------------------------------------------------------------------------------*/
void PNG_CFG_DeviceConfig(void);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PNG_CFG_H_ */

/** @} */

