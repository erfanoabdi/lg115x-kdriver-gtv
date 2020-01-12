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

/** @file
 *
 *  driver interface header for fbdev device. ( used only within kdriver )
 *
 *  @author		raxis
 *  @version	1.0
 *  @date		2009.11.15
 *
 *  @addtogroup lg1150_fbdev
 *	@{
 */

#ifndef	_FBDEV_GFX_UTIL_H_
#define	_FBDEV_GFX_UTIL_H_

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

#include "gfx_kapi.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

#define	FBDEV_GFX_CreateSurface(w,h,phys_addr)	FBDEV_GFX_CreateSurfaceEx(w,h,phys_addr,(phys_addr)?(w)*4:0x0)
extern int      FBDEV_GFX_CreateSurfaceEx	( int w, int h, UINT32 phys_addr, UINT32 stride );
extern void		FBDEV_GFX_FreeSurface		( int fd );
extern int		FBDEV_GFX_GetSurfaceInfo	( int fd, LX_GFX_SURFACE_SETTING_T* surface_info );
extern void     FBDEV_GFX_BlitSurface    	( int src0, int src1, int dst, int sx, int sy, int sw, int wh, int dx, int dy, BOOLEAN fBlend );
extern void		FBDEV_GFX_StretchSurface 	( int src, int dst, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh );
extern void		FBDEV_GFX_ClearSurface		( int dst, int w, int h );
extern void		FBDEV_GFX_FillSurface		( int fd, int x, int y, int w, int h, UINT32 color );

extern void		FBDEV_GFX_FadeSurface		( int src, int dst, int w, int h, UINT32 alpha );

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _FBDEV_DRV_H_ */

/** @} */
