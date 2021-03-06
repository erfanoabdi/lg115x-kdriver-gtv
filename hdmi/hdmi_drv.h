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
 *  driver interface header for hdmi device. ( used only within kdriver )
 *	hdmi device will teach you how to make device driver with new platform.
 *
 *  @author		sh.myoung (sh.myoung@lge.com)
 *  @version	1.0
 *  @date		2010.01.06
 *
 *  @addtogroup lg1150_hdmi
 *	@{
 */

#ifndef	_HDMI_DRV_H_
#define	_HDMI_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "hdmi_cfg.h"
#include "hdmi_kapi.h"

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
#if	1	//temp
#define	HDMI_PRINT(format, args...)		DBG_PRINT(  g_hdmi_debug_fd, 0, format, ##args)
#define	HDMI_TRACE(format, args...)		DBG_PRINTX( g_hdmi_debug_fd, 1, format, ##args)
#define	HDMI_ERROR(format, args...)		DBG_PRINT(  g_hdmi_debug_fd, 2, format, ##args)
#define	HDMI_DEBUG(format, args...)		DBG_PRINT(  g_hdmi_debug_fd, 3, format, ##args)
#define	AUDIO_ERROR(format, args...)	DBG_PRINT(  g_hdmi_debug_fd, 4, format, ##args)
#define	AUDIO_DEBUG(format, args...)	DBG_PRINT(  g_hdmi_debug_fd, 5, format, ##args)
#else
#define	HDMI_PRINT(format, args...)		DBG_PRINT(  g_hdmi_debug_fd, 0, format, ##args)
#define	HDMI_TRACE(format, args...)		DBG_PRINTX( g_hdmi_debug_fd, 0, format, ##args)
#define	HDMI_ERROR(format, args...)		DBG_PRINT(  g_hdmi_debug_fd, 0, format, ##args)
#endif
/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern	int      HDMI_Init(void);
extern	void     HDMI_Cleanup(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int		g_hdmi_debug_fd;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _HDMI_DRV_H_ */

/** @} */
