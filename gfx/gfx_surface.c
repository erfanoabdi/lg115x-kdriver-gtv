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
 * GFX module control Drivers linux module
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version     1.0
 *  date
 *  note        Additional information.
 *
 *  @addtogroup lg1150_gfx
 *  @{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>		 /**< printk() */
#include <linux/slab.h>			 /**< kmalloc() */
#include <linux/fs.h>			 /**< everything\ldots{} */
#include <linux/types.h>		 /**< size_t */
#include <linux/fcntl.h>		 /**< O_ACCMODE */
#include <asm/uaccess.h>
#include <linux/ioport.h>		 /**< For request_region, check_region etc */
#include <asm/io.h>				 /**< For ioremap_nocache */
#include <linux/workqueue.h>	 /**< For working queue */
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>

#include "gfx_impl.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define ROUND32(width) ((((width) + 31) / 32) * 32 )

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
GFX_SURFACE_OBJ_T *g_stSurface;

int GFX_AllocMemory(UINT32 surface_fd);
int GFX_FreeMemory(UINT32 surface_fd);

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT32					g_cr_idx;
static OS_RGN_T					g_rgn;
static LX_GFX_PHYS_MEM_INFO_T	g_gfx_mem_info;

/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 *
 * initialize GFX memory for surface
 *
 * @param	void
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_InitSurfaceMemory(void)
{
	int ret = 0;

	g_cr_idx = 0;

	/* partition gfx memory into two part. first part is non-cached attributed and second part is cached attributed */
	g_gfx_mem_info.phys_addr 	= (void*)gMemCfgGfx.surface.base;
	g_gfx_mem_info.offset	 	= 0;
	g_gfx_mem_info.length	 	= gMemCfgGfx.surface.size;

	GFX_TRACE_BEGIN();
	g_stSurface = (GFX_SURFACE_OBJ_T *)OS_Malloc(sizeof(GFX_SURFACE_OBJ_T)*GFX_MAX_SURFACE);
	memset((void *)g_stSurface, 0x0 , sizeof(GFX_SURFACE_OBJ_T)*GFX_MAX_SURFACE);
//	ret = OS_InitRegion( &g_rgn , (void *)g_gfx_mem_info.phys_addr, g_gfx_mem_info.length );

	GFX_CHECK_CODE( ret<0 , /* nop */, "%s:%d -- can't initialize surface region..\n", __F__, __L__ );
	GFX_TRACE_END();

	return ret;
}

/**
 *
 * allocate surface memory as imcoming param-LX_GFX_SURFACE_SETTING_PARAM_T
 *
 * @param	LX_GFX_SURFACE_SETTING_PARAM_T
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_AllocSurface(LX_GFX_SURFACE_SETTING_PARAM_T *surface)
{
	int i;
	int slot = -1;
	int ret  = RET_ERROR;

	GFX_TRACE_BEGIN();

	/* I would like to give the special fd to the external memory based surface */
	if( (ULONG)surface->surface_setting.phys_addr ) { i = 0; }
	else 											{ i = 0x80; }

	/* serach empty slot */
	for ( /* nop */ ; i < GFX_MAX_SURFACE; i++ )
	{
		if ( !g_stSurface[i].bAlloc ) { slot = i; break; }
	}

	GFX_CHECK_CODE( slot < 0, goto func_exit, "surface pool(max %d) is full\n", GFX_MAX_SURFACE );
	GFX_PRINT("%s : empty slot = %d\n", __F__, slot );

	surface->surface_fd 				= slot;
	g_stSurface[slot].bAlloc 			= 1;
	g_stSurface[slot].surf.type 		= surface->surface_setting.type;
	g_stSurface[slot].surf.pixel_format = surface->surface_setting.pixel_format;
	g_stSurface[slot].surf.width 		= surface->surface_setting.width;
	g_stSurface[slot].surf.height 		= surface->surface_setting.height;
	g_stSurface[slot].surf.alignment 	= surface->surface_setting.alignment;

	g_stSurface[slot].cIdx				= g_cr_idx++;
	g_stSurface[slot].cTick				= OS_GetMsecTicks();

	GFX_TRACE("%s:%d -- fd %d, dim %dx%d\n", __F__, __L__, slot, g_stSurface[slot].surf.width, g_stSurface[slot].surf.height );

	/* if application doesn't know stride, caculate it here */
	if(surface->surface_setting.stride == 0)
	{
		switch(surface->surface_setting.pixel_format)
		{
			case LX_GFX_PIXEL_FORMAT_INDEX_0:
				g_stSurface[slot].surf.stride = 0;
				break;

			case LX_GFX_PIXEL_FORMAT_INDEX_1:
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*1)/8;
				break;
			case LX_GFX_PIXEL_FORMAT_INDEX_2 :
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*2)/8;
				break;
			case LX_GFX_PIXEL_FORMAT_INDEX_4 :
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*4)/8;
				break;
			case LX_GFX_PIXEL_FORMAT_INDEX_8 :
			case LX_GFX_PIXEL_FORMAT_ALPHA_8 :
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*8)/8;
				break;

			case LX_GFX_PIXEL_FORMAT_Y8__Cb8_444__Cr8_444 :
			case LX_GFX_PIXEL_FORMAT_Cb8_420__Cr8_420:
			case LX_GFX_PIXEL_FORMAT_Cb8_422__Cr8_422:
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*8)/8;
				break;

			case LX_GFX_PIXEL_FORMAT_CbCr_420:		/* 16 */
			case LX_GFX_PIXEL_FORMAT_CbCr_422:		/* 16 */
			case LX_GFX_PIXEL_FORMAT_CbCr_444:		/* 16 */
			case LX_GFX_PIXEL_FORMAT_Y0Cb0Y1Cr0_422:/* 16 */
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*16)/8;
				break;

			case LX_GFX_PIXEL_FORMAT_YCbCr655:
			case LX_GFX_PIXEL_FORMAT_AYCbCr2644:
			case LX_GFX_PIXEL_FORMAT_AYCbCr4633:
			case LX_GFX_PIXEL_FORMAT_AYCbCr6433:
			case LX_GFX_PIXEL_FORMAT_RGB565 :
			case LX_GFX_PIXEL_FORMAT_ARGB1555:
			case LX_GFX_PIXEL_FORMAT_ARGB4444 :
			case LX_GFX_PIXEL_FORMAT_ARGB6343 :
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*16)/8;
				break;

			case LX_GFX_PIXEL_FORMAT_AYCbCr8888 :
			case LX_GFX_PIXEL_FORMAT_ARGB8888 :
				g_stSurface[slot].surf.stride = ROUND32(surface->surface_setting.width*32)/8;
				break;
		}
	}
	else
	{
		g_stSurface[slot].surf.stride = surface->surface_setting.stride;
	}

	/*	raxis.lim (2010/05/13)
	 *	-- 	we need fill surface memory info even though it is from external memory
	 *		since application can query surfaec memory regardless of surface type.
	 *		but it is important that valid information may be only physical address or length.
	 */
	if( surface->surface_setting.phys_addr )
	{
		g_stSurface[slot].surf.phys_addr= surface->surface_setting.phys_addr;
		g_stSurface[slot].mem.phys_addr = surface->surface_setting.phys_addr;
		g_stSurface[slot].mem.length    = surface->surface_setting.stride * surface->surface_setting.height;
		g_stSurface[slot].mem.offset    = (UINT32)surface->surface_setting.phys_addr - (UINT32)g_gfx_mem_info.phys_addr;

		ret = RET_OK;
	}
	else
	{
		ret = GFX_AllocMemory(surface->surface_fd);
	}

	GFX_CHECK_CODE( ret != RET_OK, g_stSurface[slot].bAlloc = 0; goto func_exit, "fail to alloc surface (%d)\n", surface->surface_fd );

	g_stSurface[slot].bPalette = 0;

	GFX_GetAllocSurface(surface);
	//GFX_TRACE("%s:%d -- surface fd = %d, phys addr = %p\n", __F__, __L__,
	//surface->surface_fd, surface->surface_setting.phys_addr );
	//GFX_TRACE("SURFACE BASE %p IDX %d SZ %d\n", g_stSurface, slot, sizeof(GFX_SURFACE_OBJ_T) );
	//OS_HexDump( (UINT32)&(g_stSurface[slot]), (UINT8*)&(g_stSurface[slot]), sizeof(GFX_SURFACE_OBJ_T) );

	ret = RET_OK;
func_exit:
	GFX_TRACE_END();
	return ret;
}

/**
 *
 * return allocated surface memory inform
 *
 * @param	LX_GFX_SURFACE_SETTING_PARAM_T
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_GetAllocSurface(LX_GFX_SURFACE_SETTING_PARAM_T *surface)
{
	int ret = RET_ERROR;
	int slot = surface->surface_fd;

	GFX_TRACE_BEGIN();

	/* check error condition */
	GFX_CHECK_CODE( slot > GFX_MAX_SURFACE, goto func_exit, "invalid fd (%d)\n", slot );
	
//	if(g_stSurface[slot].surf.width == 0 && g_stSurface[slot].surf.height == 0)
//	{
//		GFX_TRACE_END();
//		return -1;
//	}

	surface->surface_setting.type       = g_stSurface[slot].surf.type;
	surface->surface_setting.pixel_format= g_stSurface[slot].surf.pixel_format;
	surface->surface_setting.width      = g_stSurface[slot].surf.width;
	surface->surface_setting.height     = g_stSurface[slot].surf.height;
	surface->surface_setting.alignment  = g_stSurface[slot].surf.alignment;
	surface->surface_setting.stride     = g_stSurface[slot].surf.stride;
	surface->surface_setting.phys_addr  = g_stSurface[slot].surf.phys_addr;

	ret = RET_OK;
func_exit:
	GFX_TRACE_END();
	return ret;
}

/** free allocated surface memory which got from GFX_AllocSurface
 *
 * @param	int surface_fd
 * @return	int	0 : OK , -1 : NOT OK
 */
int GFX_FreeSurface(UINT32 surface_fd)
{
	UINT32 phys_addr;
	int ret = RET_ERROR;
	GFX_TRACE_BEGIN();

	/* check error condition */
	GFX_CHECK_CODE( surface_fd > GFX_MAX_SURFACE, goto func_exit, "invalid fd (%d)\n", surface_fd );
	
	phys_addr = (UINT32)g_stSurface[surface_fd].surf.phys_addr;

	/* since surface is allocated with the external memory, we should check phys_addr range */
	if ( g_stSurface[surface_fd].surf.type == LX_GFX_SURFACE_TYPE_MEM_BUFFER 
		&& ( phys_addr >= (UINT32)g_gfx_mem_info.phys_addr && phys_addr < (UINT32)g_gfx_mem_info.phys_addr+g_gfx_mem_info.length ) )
	{
       GFX_FreeMemory(surface_fd);
	}

	/*	raxis.lim (2010/07/01)
	 *	-- palettte memory should be free when paleltte data is available.
	 *	-- bPalette variable seems not enough to free palette data.
	 */
	if( g_stSurface[surface_fd].pal )
	{
		OS_Free( g_stSurface[surface_fd].pal );
	}

	memset( &g_stSurface[surface_fd], 0x0, sizeof(GFX_SURFACE_OBJ_T) );

	ret = RET_OK;
func_exit:
	GFX_TRACE_END();
	return ret;
}


/**
 *
 * free allocated surface's image memory which got from GFX_FreeMemory
 *
 * @param	int surface_fd
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_FreeMemory(UINT32 surface_fd)
{
	int ret=0;

	GFX_TRACE_BEGIN();
	OS_FreeRegion ( &g_rgn, (void*)g_stSurface[surface_fd].surf.phys_addr );
	GFX_TRACE_END();

	return ret;
}


/**
 *
 * allocate surface's image memory which got from GFX_AllocMemory
 *
 * @param	int surface_fd , void *ptr
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_AllocMemory(UINT32 surface_fd)
{
	void *ptr ;
	int ret=RET_OK;

	GFX_TRACE_BEGIN();

#if 0
	ptr = OS_MallocRegion ( &g_rgn, g_stSurface[surface_fd].surf.height * g_stSurface[surface_fd].surf.stride );
#else
	ptr = NULL;
#endif

	if(ptr == (void *)NULL)
	{
		GFX_TRACE_END();
		return RET_ERROR;
	}

	//	g_stSurface[surface_fd].mem.surface_fd = surface_fd;
	g_stSurface[surface_fd].surf.phys_addr  = ptr;
	g_stSurface[surface_fd].mem.phys_addr   = ptr;
	g_stSurface[surface_fd].mem.length      = g_stSurface[surface_fd].surf.height*g_stSurface[surface_fd].surf.stride;

	if((UINT32)ptr >= (UINT32)g_gfx_mem_info.phys_addr)
	{
		g_stSurface[surface_fd].mem.offset = (UINT32)ptr - (UINT32)g_gfx_mem_info.phys_addr;
	}
	else
	{
		GFX_ERROR("ERROR : dynamic allocation failed %8p\n" , ptr);
		ret = RET_ERROR;
	}

	GFX_TRACE_END();
	return ret;
}

/**
 *
 * get surface's image memory inform
 *
 * @param	LX_GFX_SURFACE_MEM_INFO_PARAM_T *mem
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_GetSurfaceMemory(LX_GFX_SURFACE_MEM_INFO_PARAM_T *mem)
{
	GFX_TRACE_BEGIN();
#if 1
	memcpy( &mem->surface_mem, &(g_stSurface[mem->surface_fd].mem), sizeof(LX_GFX_PHYS_MEM_INFO_T));
#else
	mem->surface_mem.phys_addr  = g_stSurface[mem->surface_fd].mem.phys_addr;
	mem->surface_mem.offset     = g_stSurface[mem->surface_fd].mem.offset;
	mem->surface_mem.length     = g_stSurface[mem->surface_fd].mem.length;
#endif

	GFX_TRACE("%s:%d -- fd:%d, phys_addr:%p, offset:%d, length:%d\n", __F__,__L__,
					mem->surface_fd, mem->surface_mem.phys_addr, mem->surface_mem.offset, mem->surface_mem.length );
	GFX_TRACE_END();
	return 0;
}


/**
 * mmap the surface's image physical memory to virtual memory for using user level
 *
 * @param	struct vm_area_struct *vma
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_SurfaceMmap(struct vm_area_struct *vma)
{
	int	ret;
	ULONG gfx_mem_start, gfx_mem_len;
	ULONG start;
	ULONG len;
	ULONG off = ((vma->vm_pgoff) << PAGE_SHIFT);

	GFX_TRACE_BEGIN();

//	printk("[%s] vm_start %08x, vm_end %08x, vm_len %d\n", __F__, (UINT32)vma->vm_start, (UINT32)vma->vm_end,
//											 (UINT32)vma->vm_end - (UINT32)vma->vm_start );

	gfx_mem_start	= (UINT32)gMemCfgGfx.surface.base & PAGE_MASK;
	gfx_mem_len		= PAGE_ALIGN((gfx_mem_start & ~PAGE_MASK) + gMemCfgGfx.surface.size );

    if ((vma->vm_end - vma->vm_start + off) > gfx_mem_len )
	{
		GFX_TRACE_END(); return -EINVAL;
	}

	/* check if current memory range is non-cached area.
	 * if memory region is between non-cached area, do mmap with non-cached attribute.
	 * otherwise do mmap with cached attribute.
	 */
	start				= (UINT32)g_gfx_mem_info.phys_addr & PAGE_MASK;
	len					= PAGE_ALIGN((start & ~PAGE_MASK) + g_gfx_mem_info.length );
	vma->vm_page_prot	= pgprot_noncached(vma->vm_page_prot);

	off					+= gfx_mem_start;
	vma->vm_pgoff		= off >> PAGE_SHIFT;
	vma->vm_flags		|= VM_IO;

	ret = io_remap_pfn_range(vma, 	vma->vm_start, off >> PAGE_SHIFT,
									vma->vm_end - vma->vm_start,
									vma->vm_page_prot );
	GFX_TRACE_END();

	return ret;
}

/**
 *
 * save the pallete date to surface
 *
 * @param	struct vm_area_struct *vma
 * @return	int	0 : OK , -1 : NOT OK
 *
 */
int GFX_SetSurfacePalette(int surface_fd, int size , UINT32 *data)
{
	int ret = RET_OK;

	GFX_TRACE_BEGIN();

	if( (g_stSurface[surface_fd].surf.width == 0) || (g_stSurface[surface_fd].surf.height == 0) )
	{
		GFX_ERROR("ERROR : tried to save palette data on free surface [%d]\n" , surface_fd);
		GFX_TRACE_END();
		return RET_ERROR;
	}

	/*	raxis.lim (2010/07/01) -- bug fix
	 *	-- 	the real palette memory is allocated when it is set for the first time.
	 *	-- 	We know the maximum palette size is 256 for 8bpp surface, so it is very normal 
	 *		to allocate the maximum 256 palette entries.
	 */		
	if ( !g_stSurface[surface_fd].pal )
	{
		g_stSurface[surface_fd].pal = (UINT32*)OS_Malloc( sizeof(UINT32)*256 );
	}

	GFX_CHECK_CODE( !g_stSurface[surface_fd].pal, goto func_exit, "out of resource. check memory usage\n");

	memcpy( (void *)g_stSurface[surface_fd].pal , (void *)data , sizeof(UINT32)*size);

	/* 	raxis.lim (2010/07/01) -- bug fix
	 *	-- PalSize should be number of palette entry not byte size of palette data
	 *	-- bPalDownload field should be removed ( GFX should always download palette when blit time )
	 */
	g_stSurface[surface_fd].bPalette 	= 1;
	g_stSurface[surface_fd].bPalDownload= 0;
	g_stSurface[surface_fd].PalSize 	= size;

	GFX_PRINT("%s:%d -- fd %d, palSize %d, palData %p\n", __F__, __L__, 
				surface_fd, g_stSurface[surface_fd].PalSize, g_stSurface[surface_fd].pal );
func_exit:
	GFX_TRACE_END();

	return ret;
}

/**	get memory statistics for GFX surface memory
 *	this function is used in IOCTL for debug, and in gfx proc.
 *	application can use this information to monitor memroy usage.
 *
 *	@param 	pMemStat [IN] pointer to LX_GFX_MEM_STAT_T ( surface memory info )
 *	@return RET_OK
 *
 */
int GFX_GetSurfaceMemoryStat ( LX_GFX_MEM_STAT_T* pMemStat )
{
	OS_RGN_INFO_T   mem_info;

	GFX_TRACE_BEGIN();

	pMemStat->surface_max_num	= GFX_MAX_SURFACE;

	/* copy memory partition information */
	memcpy( &pMemStat->mem_info_non_cached, &g_gfx_mem_info, sizeof(LX_GFX_PHYS_MEM_INFO_T));
//	memcpy( &pMemStat->mem_info_cached, 	&g_gfx_mem_info, sizeof(LX_GFX_PHYS_MEM_INFO_T));

	/* get memory region info */
	(void)OS_GetRegionInfo ( &g_rgn, &mem_info );

	pMemStat->surface_mem_base 		= (void*)mem_info.phys_mem_addr;
	pMemStat->surface_mem_length	= mem_info.length;
	pMemStat->surface_mem_alloc_size= mem_info.mem_alloc_size;
	pMemStat->surface_mem_free_size = mem_info.mem_free_size;
	GFX_TRACE_END();

	return RET_OK;
}

/** @} */
