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
 *  driver interface header for png device. ( used only within kdriver )
 *	png device will teach you how to make device driver with new platform.
 *
 *  @author		raxis.lim (raxis.lim@lge.com).lim (raxis.lim@lge.com)
 *  @version	1.0 
 *  @date		2010.10.05
 *
 *  @addtogroup lg1150_png
 *	@{
 */

#ifndef	_PNG_DRV_H_
#define	_PNG_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "png_cfg.h"
#include "png_kapi.h"

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
#define PNG_PRINT(format, args...)      DBG_PRINT( g_png_debug_fd, 0, format, ##args)
#define PNG_TRACE(format, args...)      DBG_PRINT( g_png_debug_fd, 1, format, ##args)
#define PNG_WARN(format, args...)       DBG_PRINT( g_png_debug_fd, 2, format, ##args)
#define PNG_ERROR(format, args...)      DBG_PRINT( g_png_debug_fd, 3, format, ##args)

#define PNG_TRACE_BEGIN()               PNG_TRACE("[PNG:%d] BEGIN   -- %s:%d\n", g_png_trace_depth++, __FUNCTION__, __LINE__ )
#define PNG_TRACE_END()                 PNG_TRACE("[PNG:%d] END     -- %s:%d\n", --g_png_trace_depth, __FUNCTION__, __LINE__ )
#define PNG_TRACE_MARK()                PNG_TRACE("[PNG] LOGGING -- %s:%d\n", __FUNCTION__, __LINE__ )

#define PNG_CHECK_CODE(__checker,__if_action,fmt,args...)   \
             __CHECK_IF_ERROR(__checker, PNG_ERROR, __if_action , fmt, ##args )

#define PNG_ASSERT(__checker)   \
            __CHECK_IF_ERROR( !(__checker), PNG_WARN, /* nop */, "[PNG] ASSERT FAILED -- %s:%d\n", __FUNCTION__, __LINE__ )


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern	int      PNG_Init(void);
extern	void     PNG_Cleanup(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int		g_png_debug_fd;
extern  int     g_png_trace_depth;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PNG_DRV_H_ */

/** @} */
