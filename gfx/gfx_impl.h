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
 *  main internal header for gfx device. 
 *
 *  @author		raxis.lim (raxis.lim@lge.com)
 *  @version	1.0 
 *  @date		2011.04.03
 *
 *  @addtogroup lg1150_gfx
 *	@{
 */

#ifndef	_GFX_DRV_H_
#define	_GFX_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#include "base_types.h"
#include "os_util.h"
#include "debug_util.h"
#include "misc_util.h"

#include "gfx_cfg.h"
#include "gfx_kapi.h"
#include "gfx_hal.h"

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
#define	GFX_PRINT(format, args...)		DBG_PRINT( g_gfx_debug_fd, 0, format, ##args)
#define	GFX_TRACE(format, args...)		DBG_PRINT( g_gfx_debug_fd, 1, format, ##args)
#define GFX_WARN(format, args...)		DBG_PRINT( g_gfx_debug_fd, 2, format, ##args)
#define	GFX_ERROR(format, args...)		DBG_PRINT( g_gfx_debug_fd, 3, format, ##args)

#define	GFX_TRACE_BEGIN()				GFX_TRACE("[GFXDEV:%d] BEGIN   -- %s:%d\n", g_gfx_trace_depth++, __FUNCTION__, __LINE__ )
#define	GFX_TRACE_END()					GFX_TRACE("[GFXDEV:%d] END     -- %s:%d\n", --g_gfx_trace_depth, __FUNCTION__, __LINE__ )
#define	GFX_TRACE_MARK()				GFX_TRACE("[GFXDEV] LOGGING -- %s:%d\n", __FUNCTION__, __LINE__ )

#define GFX_CHECK_CODE(__checker,__if_action,fmt,args...)   \
			 __CHECK_IF_ERROR(__checker, GFX_PRINT, __if_action , fmt, ##args )

#define GFX_ASSERT(__checker)	\
			__CHECK_IF_ERROR( !(__checker), GFX_WARN, /* nop */, "[GFXDEV] ASSERT FAILED -- %s:%d\n", __FUNCTION__, __LINE__ )

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct
{
    UINT32                      bAlloc:1,       // surface is allocated or not
                                bPalette:1,     // palette can be used or not
                                bPalDownload:1, // download to GFX IP
                                :1,
                                PalSize:12;     // palette size ( max value 256 )

    UINT32                      *pal;           // palette data max is 256*4 byte
    LX_GFX_SURFACE_SETTING_T    surf;
    LX_GFX_PHYS_MEM_INFO_T      mem;

	UINT32						cIdx;			// creation idex 
	UINT32						cTick;			// createion tick
}
GFX_SURFACE_OBJ_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern void					GFX_InitCfg					(void);

extern int					GFX_InitHW					(void);
extern int					GFX_ShutdownHW				(void);

extern int					GFX_AllocSurface			(LX_GFX_SURFACE_SETTING_PARAM_T *surface);
extern int					GFX_SetSurfacePalette		(int port_num, int size , UINT32 *data);
extern int					GFX_InitSurfaceMemory		(void);
extern int					GFX_AllocSurface			(LX_GFX_SURFACE_SETTING_PARAM_T *surface);
extern int					GFX_GetAllocSurface			(LX_GFX_SURFACE_SETTING_PARAM_T *surface);
extern int					GFX_FreeSurface				(UINT32 surface_fd);
extern int					GFX_FreeUnmap      		  	(UINT32 surface_fd);
extern int					GFX_GetSurfaceMemory		(LX_GFX_SURFACE_MEM_INFO_PARAM_T *mem);
extern int					GFX_GetSurfaceMemoryStat	(LX_GFX_MEM_STAT_T* pMemStat );
extern int					GFX_SurfaceMmap				(struct vm_area_struct *vma) ;

extern int					GFX_RunCustomBlitDraw		(LX_GFX_MANUAL_BLEND_CTRL_PARAM_T *param);
extern int					GFX_RunSwScale				(LX_GFX_SW_SCALE_CTRL_PARAM_T* param);
extern int					GFX_RunSwBlit				(LX_GFX_SW_BLIT_CTRL_PARAM_T* param);
extern int					GFX_SetGlobalAlpha			(UINT8 *alpha);
extern int					GFX_SetColorSpace			(UINT32 *coef);
extern void					GFX_DumpBlitParam			(LX_GFX_MANUAL_BLEND_CTRL_PARAM_T *param);

extern int					GFX_GetCommandDelay			(UINT32 *cmd_delay);
extern int					GFX_SetCommandDelay			(UINT32 cmd_delay);

extern LX_GFX_BATCH_RUN_MODE_T     GFX_GetRunCommand 	(void);
extern int                 GFX_SetRunCommand			(LX_GFX_BATCH_RUN_MODE_T *cmd);

extern int                 GFX_SetGraphicSyncMode		(LX_GFX_GRAPHIC_SYNC_MODE_T mode);
extern LX_GFX_GRAPHIC_SYNC_MODE_T  GFX_GetGraphicSyncMode(void);

extern UINT32   			GFX_PxlFmt2Bpp				( LX_GFX_PIXEL_FORMAT_T pxl_fmt );
extern UINT32				GFX_GetPortBaseAddr			( GFX_SURFACE_OBJ_T* surface, LX_RECT_T* rect );

extern UINT32				GFX_CalcSyncTimeout			( LX_GFX_MANUAL_BLEND_CTRL_PARAM_T *param, UINT32 default_msec_tmout );
extern int					GFX_WaitSyncCommand			( UINT32 msec_tmout );
extern int					GFX_SetSyncWakeup			(void);

extern  void				GFX_PROC_Init				(void);
extern  void				GFX_PROC_Cleanup			(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int					g_gfx_debug_fd;
extern	int					g_gfx_trace_depth;
extern	LX_GFX_CFG_T		g_gfx_cfg;
extern	GFX_HAL_T			g_gfx_hal;
extern	GFX_SURFACE_OBJ_T*	g_stSurface;

//extern	ULONG			g_gfx_dma_virt_addr[2];

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _GFX_DRV_H_ */

/** @} */
