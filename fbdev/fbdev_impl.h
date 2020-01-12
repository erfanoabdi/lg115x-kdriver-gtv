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

#ifndef	_FBDEV_IMPL_H_
#define	_FBDEV_IMPL_H_

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
#include <linux/fb.h>

#include "base_types.h"
#include "os_util.h"
#include "debug_util.h"
#include "misc_util.h"

#include "fbdev_kapi.h"
#include "fbdev_cfg.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define MODULE_NAME         	"l8fb"
#define OSD0_FBNAME				"osd0_fb"
#define OSD1_FBNAME				"osd1_fb"
#define OSD2_FBNAME				"osd2_fb"
#define OSD3_FBNAME				"crsr_fb"
#define OSD_CURSOR_FBNAME   	OSD3_FBNAME

#define is_win(name, x) 		((strcmp(name, x ## _FBNAME) == 0) ? 1 : 0)

#define	BYTES_PER_WORD			8		/* 1 bitmap word =  8 byte */
#define BITS_PER_WORD			64		/* 1 bitmap word = 64 bit  */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define FBDEV_PRINT(fmt, args...)   DBG_PRINT( g_fbdev_debug_fd, 0, fmt, ##args)
#define FBDEV_TRACE(fmt, args...)   DBG_PRINT( g_fbdev_debug_fd, 1, fmt, ##args)
#define FBDEV_WARN(fmt, args...)    DBG_PRINT( g_fbdev_debug_fd, 2, "^y^[FBDEV-WRN] %s:%d -- " fmt, __F__, __L__, ##args)
#define FBDEV_ERROR(fmt, args...)   DBG_PRINT( g_fbdev_debug_fd, 3, "^y^[FBDEV-ERR] %s:%d -- " fmt, __F__, __L__, ##args)

#define FBDEV_TRACE_BEGIN()         FBDEV_TRACE("[FBDEV:%d] BEGIN -- %s:%d\n", g_fbdev_trace_depth++, __F__, __L__ )
#define FBDEV_TRACE_END()           FBDEV_TRACE("FBDEV:%d] END    -- %s:%d\n", --g_fbdev_trace_depth, __F__, __L__ )
#define FBDEV_TRACE_MARK()          FBDEV_TRACE("[FBDEV] LOGGING  -- %s:%d\n", __F__, __L__ )

#define FBDEV_CHECK_CODE(__checker,__if_action,fmt,args...)   \
             __CHECK_IF_ERROR(__checker, FBDEV_WARN, __if_action , fmt, ##args )

#define FBDEV_ASSERT(__checker)   \
            __CHECK_IF_ERROR( !(__checker), FBDEV_ERROR, /* nop */, "ASSERT FAILED" )

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct fb_var_screeninfo    FBDEV_VAR_SCREEN_T;
typedef struct fb_fix_screeninfo    FBDEV_FIX_SCREEN_T;

struct dm_win_info;

struct scanvas
{
	/*! Stores OSD Plane id to which this image is attached. It is updated in the AttachImage function */
	int				fb_dev_id;

	UINT32			uiOSDHeaderPTR;		/* ioremapped address of OSD header */
	UINT32			uiPalettePTR;		/* physical address of palette data.
										   WORD(64 bit) Pointer to Palette allocated in SDRAM  */
	UINT32			uiBasePTR;
	UINT32			uiBitmapDataYPTR;	/* physical address of current bitmap data. 
										   WORD(64 bit) Pointer to Image (Y Component) allocated in SDRAM  */

	UINT8			eOSDPixFmt;		 	/* OSD Pixel Format */
	UINT8			eOSDPixDepth; 		/* OSD Pixel Depth */

	UINT32			stride;				/* byte size of single line. it is humna reable value of usWordsPerLine */

	LX_RECT_T		input_win;			/* input window is used to determine area within OSD bitmap data.	
										 * when updating screen, driver finds the start address of real base
										 * address to be displayed with input_win.
										 * input_win shoule be area inside of the real OSD bitmap data.
										 * for example, if OSD bitmap data is 1366x768, input_win area should
										 * be located within (0,0,1366,768).
										 * kdriver also uses xoffset and yoffset for pan_display requet.
										 * simple equation to calucate the base address is :
										 *
										 * base_address = ( bitmap_addr + (input_win.y + yoffset) * bitmap_stride
										 *				    + ( input_win.x + xoffset )
										 */

	LX_RECT_T		output_win;			/* output window is used to determine area within display region.
										 * output window is mapped to the real display ( normally 1920x1080 )
										 * by customizing both input_win and output_win, we can display any area 
										 * of bitmap data onto any area of display region.
										 *
										 * output_win data is direactly written to the L8 OSD register.
										 */

	UINT8			bColorKeyEnable;	/* Color Key Enable */
	UINT8			bPixelOrderEnable;	/* Pixel order Enable */
	UINT8			bGlobalAlphaEnable; /* Global Alpha for 0 and 4 Pixel format */
	UINT8			bBackImageEnable;	/* OSD has its own back image? test only */

	UINT32			uiOSDColorKey;
	UINT32			uiLfsrSeed;
	UINT16  		usGlobalAlpha;

	LX_MEM_BUF_T	back_image_mem;		/* test only */

	LX_DIMENSION_T	csr_size;			/* valid only for cursor layer. user defined cursor size */
	LX_POSITION_T	csr_hotspot;		/* valid only for cursor layer. user defined hotspot position */

	

	struct dm_win_info*	dm;				/* link to image memory info (not used) */
};

struct dm_win_info 
{
	/* raxis.lim (2010/11/20) -- device index 
	 * this field supports the quick access to the internal structure of OSD device driver.
	 * In L8 platform, valid range is between 0 ~ 3 
	 */
	UINT16			fb_dev_id;				
	SINT16 			users; 					/* number of requests to open fb device */

	struct fb_info	fbinfo;					/* linux fb info */
	struct scanvas	pixel;					/* image description info */

	/* framebuffer area */
  	unsigned long	fb_base_phys;			/* physical address of OSD bitmap data */
	unsigned long	fb_base;				/* N/A */
  	unsigned long	fb_size;				/* byte size of OSD bitmap data */

	/*osd header info*/
	unsigned long	osd_header_base_phys;	/* physical address of indirect OSD header data */
	unsigned long	osd_header_base;		/* ioremapped address */
	unsigned long	osd_header_size;		/* byte size of OSD header (register) */

	/*palette header info*/
	unsigned long	osd_pal_base_phys;		/* physical address of palette data */
	unsigned long	osd_pal_base;			/* ioremapped address. Not used */
	unsigned long	osd_pal_size;			/* number of color in palette, fixed ad 256 */

	dma_addr_t		map_dma;				/* N/A */
	UINT8			*map_cpu;				/* N/A */
	UINT32			map_size;				/* page aligend byte size of fb_size ( bitmap data ) */

	UINT8			*screen_cpu;			/* N/A */
	dma_addr_t		screen_dma;				/* same as fb_base_phys ( physical addres of bitmap data ) */

  	UINT32			pseudo_palette[17];		/* linux fb: fake palette of 16 colors. not used ? */

 	/* flag to identify if framebuffer area is fixed already or not */
  	int				alloc_fb_mem;			/* N/A */
  	unsigned long	sdram_address;			/* N/A */

	OS_SEM_T		fb_dev_mtx;				/* mutex for each FB device */

	int				vsync_cnt_on_flip;		/* vsync count that will be used to
											   wait on flipping. <0 will not
											   care the vsync. */
	int				vsync_cnt;

	UINT32			freeze_ctrl_mask;		/* if freeze mask is non-zero value, fb kdriver should block some operation
											   accoring to freeze_mask 
											 */
	/* pointer to parent context */
	struct dm_info*	dm;
};

typedef struct dm_win_info	LX_FBDEV_WIN_INFO_T;

struct dm_info 
{
	struct dm_win_info*	winfo[FBDEV_MAX_DEVICE];

	/** memory mapped I/O access 
	 *	mmio_base_phys points the ioremapped address of OSD register.
	 *	mmio_size is the total byte size of OSD register
	 */
	unsigned long		mmio_base_phys;
	unsigned long		mmio_size;

	/** handle for the linux platform device */
	struct device		*dev;
};

typedef struct dm_info		LX_FBDEV_INFO_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern	void						FBDEV_InitCfg				(void);
extern	struct fb_var_screeninfo*	FBDEV_GetVarScreenObject 	(int idx);

void    FBDEV_GetDefaultPalData     (UINT32 *pPal);
void    OSD_SetPalette              (struct dm_win_info *w ,UINT32 size_data,void   *pPalette);
void    FBDEV_InitCanvas            (struct dm_win_info *w);
int     FBDEV_CleanupLayer          (struct dm_win_info *w);
void    FBDEV_ChangeImageEndian     (int layer, int bpp);
int		FBDEV_SetConnCtrl			(LX_FBDEV_CONN_CTRL_T* pConnCtrl);

int		FBDEV_GetRemapDevID			(int layer);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int					g_fbdev_debug_fd;
extern	int					g_fbdev_trace_depth;
extern	int					g_fbdev_osd_freeze;
extern  LX_FBDEV_CFG_T* 	g_fbdev_cfg;

extern	UINT32				g_fbdev_vsync_cnt;		/** current VSYNC count status */

extern  struct dm_info* 	g_dm_ctx;

/* FBDEV config access macro */
#define LX_FBDEV_CFG(_fld)  g_fbdev_cfg->_fld

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _FBDEV_DRV_H_ */

/** @} */
