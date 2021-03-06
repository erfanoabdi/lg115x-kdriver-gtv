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
 *  driver interface header for ttx device. ( used only within kdriver )
 *	ttx device will teach you how to make device driver with new platform.
 *
 *  @author		wonsik.do (wonsik.do@lge.com)
 *  @version	1.0
 *  @date		2010.01.06
 *
 *  @addtogroup lg1150_ttx
 *	@{
 */

#ifndef	_TTX_DRV_H_
#define	_TTX_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "ttx_cfg.h"
#include "ttx_kapi.h"

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
#define	TTX_PRINT(format, args...)		DBG_PRINT(  g_ttx_debug_fd, 0, format, ##args)
#define	TTX_TRACE(format, args...)		DBG_PRINTX( g_ttx_debug_fd, 0, format, ##args)
#define	CC_PRINT(format, args...)		DBG_PRINT(  g_ttx_debug_fd, 0, format, ##args)
#define	CC_TRACE(format, args...)		DBG_PRINTX( g_ttx_debug_fd, 0, format, ##args)


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern	int      TTX_Init(void);
extern	void     TTX_Cleanup(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int		g_ttx_debug_fd;

extern void    TTX_InitCfg ( void );

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _TTX_DRV_H_ */

/** @} */
