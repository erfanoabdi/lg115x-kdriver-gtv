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
 *  @addtogroup lg1150_template
 *
 *  driver interface header for template device. ( used only within kdriver )
 *	template device will teach you how to make device driver with new platform.
 *
 *  @author		dae-young lim (raxis@lge.com)
 *  @version	1.0 
 *  @date		2009.11.15 
 *
 */

#ifndef	_QEMU_DRV_H_
#define	_QEMU_DRV_H_

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

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define	QEMU_PRINT(format, args...)		DBG_PRINT(  g_qemu_debug_fd, 0, format, ##args)
#define	QEMU_TRACE(format, args...)		DBG_PRINTX( g_qemu_debug_fd, 0, format, ##args)

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern	int		QEMU_Init ( void );
extern	void	QEMU_Cleanup ( void );

extern	void	QEMU_StartTick  ( void );
extern	void	QEMU_StopTick   ( void );

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int		g_qemu_debug_fd;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _QEMU_DRV_H_ */

