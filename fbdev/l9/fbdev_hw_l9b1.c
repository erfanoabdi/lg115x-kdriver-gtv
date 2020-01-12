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

/* @file name : fbdev_hw.c
 * @description : fbdev hardware register control functions
 */
/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define PRINTK(fmt,args...)						//printk(##fmt,args)

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "fbdev_impl.h"
#include "fbdev_drv.h"
#include "fbdev_hw.h"
#include "fbdev_hw_l9.h"
#include "fbdev_util_l9.h"
#include "fbdev_reg_l9a0.h"

#include "../sys/ctop_regs.h"

#include "gfx_drv.h"
#include "fbdev_gfx_util.h"
#include <linux/version.h>

#undef	FBDEV_USE_UD_TEST_MODE					/* support UD test mode in FHD environment. test only */
#define FBDEV_USE_UD_BOUNDARY_WORKAROUND		/* support workaround code for fixing OSD boundary between L picture and R picture.
												 * should be enabled in L9 soc */

#undef	FBDEV_UD_CRSR_ASYNC_DRAW				/* test only */
#define	FBDEV_L9B1_VCOM_TEST_SUPPORT			/* VCOM 1920x1080 is supported */
#define	FBDEV_USE_SW_OSD_EMULATION				/* support global alpha moduatioin & down scaler etc by GFX H/W */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define FRC_OSD_EMUL_DEVID			0			/* OSD0 */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct	
{
	BOOLEAN             b_hdr_ready;        /* set to TRUE when FRC header is written */

	UINT32              hdr_phys_addr;		/* N/A */
	UINT32              hdr_virt_addr;      /* N/A */

	UINT32              frc_hdr[6];         /* FRC header data */
	UINT32              frc_hdr_remap[6];   /* FRC header data (after remap) */
}
FBDEV_FRC_HDR_EMUL_DATA_T;

typedef struct
{
	int                 osd_idx;
	int                 fb_dev_id;				/* OSD Id used for OSD merger */

	int					posd0_hd_surface;
	int					posd1_hd_surface;

	BOOLEAN				b_crsr_draw_req;		/* indicate asynchrnous cursor render requested or not */

	int					merge_osd_hd_surface[2];/* surface ID for merge HD content */
	UINT32				merge_osd_hd_addr[2];	/* two physical address for merge POSDs (1920*1080) only */

	int					my_vsync;

#define	OSD_EMUL_VIEW_NONE			0x0
#define	OSD_EMUL_VIEW_POSD0			0x0001
#define	OSD_EMUL_VIEW_POSD1			0x0002
#define	OSD_EMUL_VIEW_CRSR			0x0004
#define OSD_EMUL_VIEW_BACK_IMG		0x0010
#define OSD_EMUL_VIEW_POSD1_RESIZE	0x0020
	UINT32				osd_view_flag;
	BOOLEAN             osd_view_status[LX_FBDEV_ID_MAX];

	FBDEV_OSD_HDR_T		osd_hdr_sav[LX_FBDEV_ID_MAX];	
}
FBDEV_FRC_OSD_EMUL_DATA_T;

#ifdef	FBDEV_USE_SW_OSD_EMULATION
typedef enum
{
	FBDEV_SW_OSD_EMUL_MASK_NONE			= 0x0,
	FBDEV_SW_OSD_EMUL_MASK_GLOBAL_ALPHA	= 0x1,
	FBDEV_SW_OSD_EMUL_MASK_DOWN_SCALE	= 0x2,
}
FBDEV_SW_OSD_EMUL_TYPE_MASK_T;

/** this structure is needed to support global alpha & down scaler which is not fully supported by OSD H/W 
 *
 */
typedef struct
{
	struct
	{
		FBDEV_SW_OSD_EMUL_TYPE_MASK_T	mask;
		int				osd_idx;
		int				my_vsync;

		int				osd_surf[2];
		UINT32			bitmap_ptr[2];

		LX_RECT_T		src_rect;
		LX_RECT_T		dst_rect;
	}
	fb[LX_FBDEV_ID_MAX];
}
FBDEV_SW_OSD_EMUL_DATA_T;
#endif

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern void	FBDEV_L9_InitOSDZOrder	( void );

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
extern volatile OSD_L9A0_REG_T*		g_fbdev_l9_reg;
extern OSD_L9A0_REG_T*				g_fbdev_l9_reg_cached;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static void FBDEV_L9B1_WriteFRCHeader	( void );
static void FBDEV_L9B1_InitHDOSDMerge	( void );
static void FBDEV_L9B1_ExitHDOSDMerge	( void );
static void	FBDEV_L9B1_MergePOSD 		( int layer, BOOLEAN b_flip );
static void	FBDEV_L9B1_MergePOSD_1080p	( int layer, BOOLEAN b_flip );

static void FBDEV_L9B1_InitSwOSD		( int layer, FBDEV_SW_OSD_EMUL_TYPE_MASK_T mask );
static void FBDEV_L9B1_ExitSwOSD		( int layer, FBDEV_SW_OSD_EMUL_TYPE_MASK_T mask );

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static const int 					pixel_depth_2_bpp[PIX_DEPTH_MAX] = { 1, 2, 4, 8, 16, 24, 32  };
static FBDEV_FRC_HDR_EMUL_DATA_T	g_frc_hdr_emul;
static FBDEV_FRC_OSD_EMUL_DATA_T	g_frc_osd_emul;
static OS_SEM_T  		     		g_frc_osd_emul_mtx;
static LX_FBDEV_TRID_CTRL_T			g_frc_trid_ctrl[LX_FBDEV_ID_MAX];

#ifdef	FBDEV_USE_SW_OSD_EMULATION
static FBDEV_SW_OSD_EMUL_DATA_T		g_sw_osd_emul;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
static  spinlock_t      			g_osd_hdr_spinlock = SPIN_LOCK_UNLOCKED;
#else
static DEFINE_SPINLOCK(g_osd_hdr_spinlock);
#endif

#ifdef FBDEV_UD_CRSR_ASYNC_DRAW
static OS_TIMER_T			g_crsr_draw_noti_timer;
static struct work_struct 	g_crsr_draw_task;
static void					FBDEV_L9B1_CrsrDrawNotiTimerCallback( ULONG data );
static void 				FBDEV_L9B1_CrsrDrawTask( struct work_struct *work );
#endif

/*========================================================================================
	Implementation Group
========================================================================================*/

/** initialize L9A0(A1) HAL, Cfg and all chip dependent variables.
 * 
 */
void FBDEV_L9B1_InitHAL ( void )
{
	int	i;

	PRINTK("^g^[fb] L9B1 HAL....\n");

	OS_InitMutex ( &g_frc_osd_emul_mtx, OS_SEM_ATTR_DEFAULT );	/* create simple mutex */
#ifdef FBDEV_UD_CRSR_ASYNC_DRAW
	OS_InitTimer ( &g_crsr_draw_noti_timer );					/* initialize timer to notify cursor rendering */
	INIT_WORK(&g_crsr_draw_task, FBDEV_L9B1_CrsrDrawTask );		/* initialize work struct to support asynchronous cursor rendering in UD mode */
#endif

	g_fbdev_frc_ctrl.b_frc_sep_enable	= FALSE;	/* Blending Mode by default */
	g_fbdev_frc_ctrl.b_frc_UD_enable	= FALSE;	/* Standard Mode by default */
	g_fbdev_frc_ctrl.b_frc_hdr_emul		= FALSE;	/* Disable FRC HDR emulation */
	g_fbdev_frc_ctrl.b_frc_osd_emul		= TRUE;
	g_fbdev_frc_ctrl.frc_hdr_emul_dev	= -1;

	/* initialize OSD merge variable */
	g_frc_osd_emul.merge_osd_hd_addr[0] 	= 0x0;//gMemCfgFBDevFRCEmul.osd_emul_mem_1080p.base;
	g_frc_osd_emul.merge_osd_hd_addr[1] 	= 0x0;//gMemCfgFBDevFRCEmul.osd_emul_mem_1080p.base;
	g_frc_osd_emul.merge_osd_hd_surface[0]	= -1;
	g_frc_osd_emul.merge_osd_hd_surface[1]	= -1;

	g_frc_osd_emul.posd0_hd_surface			= -1;
	g_frc_osd_emul.posd1_hd_surface			= -1;
#if 0
	g_frc_osd_emul.crsr_bkup_surface		= -1;
#endif
	g_frc_osd_emul.osd_idx	= 0;
	g_frc_osd_emul.my_vsync	= -1;

#ifdef	FBDEV_USE_SW_OSD_EMULATION
	/* initialize SW OSD */
	for( i=0; i<LX_FBDEV_ID_MAX; i++ ) 
	{
		//g_sw_osd_emul.fb[0].osd_surf[0]	= -1;
		//g_sw_osd_emul.fb[0].osd_surf[1]	= -1;
		g_sw_osd_emul.fb[i].mask			= FBDEV_SW_OSD_EMUL_MASK_NONE;
		g_sw_osd_emul.fb[i].osd_idx			= 0;
		g_sw_osd_emul.fb[i].my_vsync		= -1;
	}
#endif

	for( i=0; i<LX_FBDEV_ID_MAX; i++ ) 
	{
		g_frc_trid_ctrl[i].mode 	= LX_FBDEV_TRID_MODE_NONE;
		g_frc_trid_ctrl[i].disparity= 0;
	}
}

/** OSD HW Layer enable/disable
 *
 * @param	int layer : osd layer ,
 *		enable : 0 : display off , 1 : display on
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L9B1_SetOSDEnable(int layer , int enable)
{
	int ret = RET_ERROR;

	FBDEV_TRACE_BEGIN();
	FBDEV_CHECK_CODE( layer<0 || layer>=LX_FBDEV_ID_MAX, goto func_exit, "invalid layer %d\n", layer );

	/* save POSD view status for FRC emulation */
	g_frc_osd_emul.osd_view_status[layer] = enable;

	/* if FRC emulation mode, ignore show/hide request for OSD2 */
    if ( g_fbdev_frc_ctrl.b_frc_UD_enable )
	{
		/* merge POSDs to the single POSD */
		FBDEV_L9B1_MergePOSD( layer, FALSE );
	}
	else
	{
		switch(layer)
		{
			case LX_FBDEV_ID_OSD0: 	g_fbdev_l9_reg->osd0_cfg.osd0_en = enable; break;
			case LX_FBDEV_ID_OSD1: 	g_fbdev_l9_reg->osd1_cfg.osd1_en = enable; break;
			case LX_FBDEV_ID_OSD2: 	g_fbdev_l9_reg->osd2_cfg.osd2_en = enable; break;
			case LX_FBDEV_ID_CSR0:	g_fbdev_l9_reg->crsr_cfg.crsr_en = enable; break;
			default: 				FBDEV_WARN("invalid OSD ID %d\n", layer ); ret = RET_ERROR; break;
		}
	}

	ret = RET_OK; /* all work done */
func_exit:
	FBDEV_TRACE_END();
	return ret;
}

/** OSD HW Layer status
 *
 * @param	void
 * @return	int 1 : ON , 0 : OFF
 *
 */
int FBDEV_L9B1_GetOSDEnable(int layer)
{
	int ret = 0;

	/* if FRC OSD enabled, just return the pseudo OSD status */
    if ( g_fbdev_frc_ctrl.b_frc_UD_enable )
	{
		return g_frc_osd_emul.osd_view_status[layer];
	}

	switch(layer)
	{
		case LX_FBDEV_ID_OSD0:	ret = g_fbdev_l9_reg->osd0_cfg.osd0_en; break;
		case LX_FBDEV_ID_OSD1:	ret = g_fbdev_l9_reg->osd1_cfg.osd1_en; break;
		case LX_FBDEV_ID_OSD2:	ret = g_fbdev_l9_reg->osd2_cfg.osd2_en; break;
		case LX_FBDEV_ID_CSR0:	ret = g_fbdev_l9_reg->crsr_cfg.crsr_en; break;
		default: 				FBDEV_WARN("invalid OSD ID %d\n", layer ); ret = -1; break;
	}

	return ret;
}

/** Set Header information
 *
 * @param	SCanvas : pointer to structure bearing osd header information
 * @return	void
 *
 * @ingroup osd_ddi_func
 */
int FBDEV_L9B1_WriteOSDHeader( struct scanvas *hCanvas )
{
	int				ret;
	int				fb_dev_id;
	UINT32			bitmap_ptr;
	FBDEV_OSD_HDR_T osd_hdr;

	/* clear osd_hdr before writing */
	memset( (void*)&osd_hdr, 0x0, sizeof(FBDEV_OSD_HDR_T) );

	ret = 0; //down_interruptible(&fbdev_sem_l9);

	fb_dev_id	= hCanvas->fb_dev_id;
	bitmap_ptr	= hCanvas->uiBitmapDataYPTR;

	/* raxis.lim (2010/12/03)
	 * -- OSD ¿Í CRSR ÀÇ register Å©±â°¡ Æ²·Á¼­ Æ²·Á¼­ µ¿ÀÏ ÄÚµå°¡ Áßº¹µÇ¾î ÀÖ´Ù. Â÷ÈÄ Á¤¸® ¿ä¸Á.
	 */
	switch( fb_dev_id )
	{
		case LX_FBDEV_ID_CSR0:
		{
			/* if cursor csr_hotspot is active then adjust (x,y) position of cursor layer.
			 * if cursor position is negative, I should move the bitmap address ( H/W doesn't accept negative position )
		 	 *
			 * if cursor size is feeded, use that values as the viewing cursor area
			 *
			 * IMPORTATNT !! if you change the base address, its alignment should be multiplt of 16 byte ( not 8 byte )
			 * woonghyeon just said that it may be H/W bug.
			 * since we are using ARGB8888 pixel format for cursor OSD. cursor offset should be multiple of 4 !!
		 	 * If we are using ARGB4444 pixel format, cursor offset should be mulitiple of 8 !!!
			 */
			int	csr_x_off = 0;
			int	csr_y_off = 0;
			int	csr_x_pos = hCanvas->output_win.x - hCanvas->csr_hotspot.x;
			int	csr_y_pos = hCanvas->output_win.y - hCanvas->csr_hotspot.y;
			int	csr_x_size= (hCanvas->csr_size.w)? LX_CALC_ALIGNED_VALUE(hCanvas->csr_size.w,2) : hCanvas->input_win.w;
			int	csr_y_size = (hCanvas->csr_size.h)? hCanvas->csr_size.h : hCanvas->input_win.h;

			/* too small cursor with may lead to OSD corruption. According to system test, width should be greater than a half of OSD width */
//			if ( csr_x_size < 16 /* 128/2 */ ) csr_x_size = 16;	/* OSD H/W crash workaround	*/
			if ( csr_x_size < 64 /* 128/2 */ ) csr_x_size = 64;	/* OSD H/W crash workaround	*/
			if ( csr_x_pos < 0 ) { csr_x_off = -csr_x_pos; csr_x_off = LX_CALC_ALIGNED_VALUE(csr_x_off,2); csr_x_pos = 0; }
			if ( csr_y_pos < 0 ) { csr_y_off = -csr_y_pos; csr_y_pos = 0; csr_y_size -= csr_y_off; }

			/* raxis.lim@lge.com (2011/09/23) 
			 * L9 donesn't support odd numbered x position when cursor is located to the far right side of the screen 
			 */
			if ( csr_x_pos + csr_x_size > 1920 ) csr_x_pos = LX_CALC_ALIGNED_VALUE(csr_x_pos,1);

			PRINTK("[crsr] sz %d,%d, pos %d,%d hot %d,%d\n", csr_x_size, csr_y_size, csr_x_pos, csr_y_pos, hCanvas->csr_hotspot.x, hCanvas->csr_hotspot.y );

			/* write cursor header 
			 *
			 * It's good idead to shrink cursor viewing area but this policy shows some incorrect behavioir when cursor type & hot spot is changed.
			 * So, it is very safe to use as large as the full cursor area.
			 * DO NOT USE osd_hdr.type.crsr.osd_hdr_w_mem = csr_x_size; 
		 	 */
			osd_hdr.type.crsr.osd_hdr_xpos		 	= csr_x_pos;		/* output x (csr x pos) */
			osd_hdr.type.crsr.osd_hdr_ypos 			= csr_y_pos;		/* output y (csr y pos) */
			osd_hdr.type.crsr.osd_hdr_w_mem 		= csr_x_size;//128 - csr_x_off;	/* viewing cursor width */
			osd_hdr.type.crsr.osd_hdr_h_mem 		= csr_y_size;		/* viewing cursor height */

			/* when 3Dmode enabled, input window should be shrinked so OSD h/w make double-sized screen */
			if( g_fbdev_trid_ctrl.fb[fb_dev_id].mode == LX_FBDEV_TRID_MODE_SS )
			{
				osd_hdr.type.crsr.osd_hdr_w_mem >>= 1;
			}
			else if ( g_fbdev_trid_ctrl.fb[fb_dev_id].mode == LX_FBDEV_TRID_MODE_TB )
			{
				osd_hdr.type.crsr.osd_hdr_h_mem >>= 1;
			}
			else
			{
				/* do nothing */
			}

			/* if offset value is valid, move bitmap address */
			bitmap_ptr += ( csr_y_off * hCanvas->stride ) + LX_CALC_ALIGNED_VALUE((csr_x_off * (pixel_depth_2_bpp[hCanvas->eOSDPixDepth]>>3)),2);

			osd_hdr.type.crsr.osd_hdr_pixel_order	= hCanvas->bPixelOrderEnable;
			osd_hdr.osd_hdr_wpl 					= hCanvas->stride>>3;

			PRINTK("[fb] cursor off : %d %d, pos ; %d %d, sz %d %d, bm 0x%08x\n\n", 
							csr_x_off, csr_y_off, osd_hdr.type.crsr.osd_hdr_xpos, osd_hdr.type.crsr.osd_hdr_ypos,
							osd_hdr.type.crsr.osd_hdr_w_mem, osd_hdr.type.crsr.osd_hdr_h_mem, bitmap_ptr );
		}
		break;

		case LX_FBDEV_ID_OSD0:
		case LX_FBDEV_ID_OSD1:
		case LX_FBDEV_ID_OSD2:
		{
			osd_hdr.type.osd.osd_hdr_color_key_en= hCanvas->bColorKeyEnable;

			osd_hdr.type.osd.osd_hdr_w_out		= hCanvas->output_win.w;	/* output w */
			osd_hdr.type.osd.osd_hdr_h_out		= hCanvas->output_win.h;	/* output h */
			osd_hdr.type.osd.osd_hdr_xpos	 	= hCanvas->output_win.x;	/* output x */
			osd_hdr.type.osd.osd_hdr_ypos 		= hCanvas->output_win.y;	/* output y */
			osd_hdr.type.osd.osd_hdr_w_mem 		= hCanvas->input_win.w;		/* input w  */
			osd_hdr.type.osd.osd_hdr_h_mem 		= hCanvas->input_win.h;		/* input h  */

			if ( g_fbdev_frc_ctrl.b_frc_sep_enable ) 
			{
				/* when 3Dmode enabled, input window should be shrinked so OSD h/w make double-sized screen */
				if( g_fbdev_trid_ctrl.fb[fb_dev_id].mode == LX_FBDEV_TRID_MODE_SS )
				{
					osd_hdr.type.osd.osd_hdr_w_mem >>= 1;
//					PRINTK("^g^TRID MODE = SS (%d,%d)\n", osd_hdr.type.osd.osd_hdr_w_mem, osd_hdr.type.osd.osd_hdr_h_mem );
				}
				else if ( g_fbdev_trid_ctrl.fb[fb_dev_id].mode == LX_FBDEV_TRID_MODE_TB )
				{
					osd_hdr.type.osd.osd_hdr_h_mem >>= 1;
//					PRINTK("^g^TRID MODE = TB (%d,%d)\n", osd_hdr.type.osd.osd_hdr_w_mem, osd_hdr.type.osd.osd_hdr_h_mem );
				}
				else
				{
					/* do nothing */
				}
			}

			osd_hdr.type.osd.osd_hdr_pixel_order= hCanvas->bPixelOrderEnable;
			osd_hdr.osd_hdr_wpl 				= hCanvas->stride>>3;

#ifdef	FBDEV_L9B1_VCOM_TEST_SUPPORT
			/* raxis.lim (2011/11/19) -- implement V-COM image (1920x1080) for POSD1
			 *
			 * L9B1 should not use the full area of 1920x1080 to avoid FRC3 osd crash.
			 * but V-COM pattern test should support 1920x1080 regardless of FRC3 problem.
		 	 */
			if ( g_dm_ctx && g_dm_ctx->winfo[LX_FBDEV_ID_OSD1] )
			{	
				static BOOLEAN b_posd1_back_image = FALSE;
				struct scanvas* posd1_canvas = &g_dm_ctx->winfo[LX_FBDEV_ID_OSD1]->pixel;

				if ( posd1_canvas->bBackImageEnable )
				{
					if ( fb_dev_id == LX_FBDEV_ID_OSD1 )
					{
						PRINTK("[fb] POSD1 BACKIMAGE !!!\n");
						bitmap_ptr							= hCanvas->back_image_mem.addr;
						osd_hdr.type.osd.osd_hdr_w_out		= 1920;
						osd_hdr.type.osd.osd_hdr_h_out		= 1080;
						osd_hdr.type.osd.osd_hdr_xpos	 	= 0;
						osd_hdr.type.osd.osd_hdr_ypos 		= 0;
						osd_hdr.type.osd.osd_hdr_w_mem 		= 1920;
						osd_hdr.type.osd.osd_hdr_h_mem 		= 1080;
						osd_hdr.osd_hdr_wpl 				= (1920*4)>>3;
					}
					else
					{
						osd_hdr.type.osd.osd_hdr_w_out		= 1920;
					}
				}
				
				/* make rewrite FRC header */
				if ( b_posd1_back_image != posd1_canvas->bBackImageEnable )	g_frc_hdr_emul.b_hdr_ready = FALSE;

				b_posd1_back_image = posd1_canvas->bBackImageEnable;
			}
#endif

#ifdef	FBDEV_USE_SW_OSD_EMULATION
			/* raxis.lim (2012/01/10) -- support global alpha modulation for POSD */
			if ( hCanvas->usGlobalAlpha < 0xff )
			{
				int		sw_osd_surf			= -1;
				UINT32	sw_osd_bitmap_ptr	= 0x0;
				int		sw_osd_idx			= g_sw_osd_emul.fb[fb_dev_id].osd_idx;

				int posd_surf	= -1;
				int width		= g_fbdev_cfg->fb[fb_dev_id].max_xres;
				int height		= g_fbdev_cfg->fb[fb_dev_id].max_yres;

				GFX_LockDevice();
				PRINTK("[fb] <0> osd %d, mask 0x%x, idx %d, global alpha 0x%02x size %d.%d bitmap 0x%08x\n", fb_dev_id, 
						g_sw_osd_emul.fb[fb_dev_id].mask, sw_osd_idx, hCanvas->usGlobalAlpha, width, height, bitmap_ptr );

				FBDEV_L9B1_InitSwOSD( fb_dev_id, FBDEV_SW_OSD_EMUL_MASK_GLOBAL_ALPHA );

				if ( g_sw_osd_emul.fb[fb_dev_id].mask & FBDEV_SW_OSD_EMUL_MASK_GLOBAL_ALPHA )
				{
					sw_osd_surf 		= g_sw_osd_emul.fb[fb_dev_id].osd_surf[ sw_osd_idx ];
					sw_osd_bitmap_ptr 	= g_sw_osd_emul.fb[fb_dev_id].bitmap_ptr[ sw_osd_idx ];

					posd_surf = FBDEV_GFX_CreateSurface( width, height, bitmap_ptr );
					FBDEV_CHECK_CODE( posd_surf<0, goto sw_osd_global_alpha_done, "can't alloc temp surface for fade\n");

					FBDEV_GFX_FadeSurface( posd_surf, sw_osd_surf, width, height, hCanvas->usGlobalAlpha );
					FBDEV_GFX_FreeSurface( posd_surf );

					/* it's very critical to replace bitmap_ptr by SW OSD bitmap */
					bitmap_ptr = sw_osd_bitmap_ptr;

					/* requeust vsync monitor */
					g_sw_osd_emul.fb[fb_dev_id].my_vsync = g_fbdev_vsync_cnt; 
sw_osd_global_alpha_done:
					GFX_UnlockDevice();
				}

				// PRINTK("[fb] <1> osd %d, global alpha 0x%02x size %d.%d bitmap 0x%08x\n", fb_dev_id, hCanvas->usGlobalAlpha, width, height, bitmap_ptr );
			}
			else if ( hCanvas->output_win.w < hCanvas->input_win.w || hCanvas->output_win.h < hCanvas->input_win.h )
			{
				int		sw_osd_surf			= -1;
				UINT32	sw_osd_bitmap_ptr	= 0x0;
				int		sw_osd_idx			= g_sw_osd_emul.fb[fb_dev_id].osd_idx;

				int posd_surf	= -1;
				int width		= g_fbdev_cfg->fb[fb_dev_id].max_xres;
				int height		= g_fbdev_cfg->fb[fb_dev_id].max_yres;

				LX_RECT_T	src_rect = { 0, 0, hCanvas->input_win.w, hCanvas->input_win.h };
				LX_RECT_T	dst_rect = { hCanvas->output_win.x, hCanvas->output_win.y, hCanvas->output_win.w, hCanvas->output_win.h };
#if 0
				/* DO NOT process the same info */
				if ( !memcmp( &dst_rect, &g_sw_osd_emul.fb[fb_dev_id].dst_rect, sizeof(LX_RECT_T) ) ) { printk("<0> ignore multiple req..\n"); goto func_exit; }
#endif
				memcpy( &g_sw_osd_emul.fb[fb_dev_id].dst_rect, &dst_rect, sizeof(LX_RECT_T));
				memcpy( &g_sw_osd_emul.fb[fb_dev_id].src_rect, &src_rect, sizeof(LX_RECT_T));

				GFX_LockDevice();
				PRINTK("[fb] <0> osd %d, mask 0x%x, idx %d, down scale %d,%d,%d,%d\n", fb_dev_id, 
						g_sw_osd_emul.fb[fb_dev_id].mask, sw_osd_idx, hCanvas->output_win.x, hCanvas->output_win.y, hCanvas->output_win.w, hCanvas->output_win.h );

				FBDEV_L9B1_InitSwOSD( fb_dev_id, FBDEV_SW_OSD_EMUL_MASK_DOWN_SCALE );

				if ( g_sw_osd_emul.fb[fb_dev_id].mask & FBDEV_SW_OSD_EMUL_MASK_DOWN_SCALE )
				{
					sw_osd_surf 		=   g_sw_osd_emul.fb[fb_dev_id].osd_surf[ sw_osd_idx ];
					sw_osd_bitmap_ptr 	= g_sw_osd_emul.fb[fb_dev_id].bitmap_ptr[ sw_osd_idx ];

					/* shrink OSD input size not to show the non-active region. 
					 * without this codes, I should clear none-active region manually ( very foolish !! )
					 */
					osd_hdr.type.osd.osd_hdr_w_mem 	= dst_rect.w;
					osd_hdr.type.osd.osd_hdr_h_mem 	= dst_rect.h;

					posd_surf = FBDEV_GFX_CreateSurface( width, height, bitmap_ptr );
					FBDEV_CHECK_CODE( posd_surf<0, goto sw_osd_down_scale_done, "can't alloc temp surface for fade\n");
#if  0
					/* clear non active region */
					FBDEV_GFX_FillSurface( sw_osd_surf, dst_rect.w, 0, width-dst_rect.w, dst_rect.h, 0x00000000 );
					FBDEV_GFX_FillSurface( sw_osd_surf, 0, dst_rect.h, width, height-dst_rect.h, 0x00000000 );
#endif
					FBDEV_GFX_StretchSurface( posd_surf, sw_osd_surf, 0, 0, hCanvas->input_win.w, hCanvas->input_win.h, 0, 0, hCanvas->output_win.w, hCanvas->output_win.h );
					FBDEV_GFX_FreeSurface( posd_surf );

					/* it's very critical to replace bitmap_ptr by SW OSD bitmap */
					bitmap_ptr = sw_osd_bitmap_ptr;

					/* requeust vsync monitor */
					g_sw_osd_emul.fb[fb_dev_id].my_vsync = g_fbdev_vsync_cnt; 
sw_osd_down_scale_done:
					GFX_UnlockDevice();
				}
			}
			else
			{
				GFX_LockDevice();
				FBDEV_L9B1_ExitSwOSD( fb_dev_id, FBDEV_SW_OSD_EMUL_MASK_GLOBAL_ALPHA|FBDEV_SW_OSD_EMUL_MASK_DOWN_SCALE );
				GFX_UnlockDevice();
			}
#endif
		}
		break;
	}

	/* copy common attribute */
	osd_hdr.osd_hdr_format 			= hCanvas->eOSDPixFmt;
	osd_hdr.osd_hdr_depth 			= hCanvas->eOSDPixDepth;

#ifdef DE_ENABLE
	osd_hdr.osd_hdr_global_alpha_en = 1;
	osd_hdr.osd_hdr_global_alpha    = 0xff;
#else
	osd_hdr.osd_hdr_global_alpha_en	= hCanvas->bGlobalAlphaEnable;
	osd_hdr.osd_hdr_global_alpha	= (hCanvas->bGlobalAlphaEnable)? hCanvas->usGlobalAlpha : 0xff;
#endif

	osd_hdr.osd_hdr_color_key		= hCanvas->uiOSDColorKey;

	if ( gpFbdevRegCfg->slkrd_prefix_mask )
	{
		osd_hdr.osd_hdr_ptr_bmp		= ((bitmap_ptr & 0x0FFFFFFF)| gpFbdevRegCfg->slkrd_prefix_mask );
		osd_hdr.osd_hdr_ptr_plte	= ((hCanvas->uiPalettePTR&0x0FFFFFFF)| gpFbdevRegCfg->slkrd_prefix_mask );
	}
	else
	{
		osd_hdr.osd_hdr_ptr_bmp		= bitmap_ptr;
		osd_hdr.osd_hdr_ptr_plte	= hCanvas->uiPalettePTR;
	}

#ifdef FBDEV_USE_MMIO_REG
	/* if FRC OSD emulation enabled and current layer is not for FRC header emulation, then merge POSD !!! */
	if ( g_fbdev_frc_ctrl.b_frc_UD_enable )
	{
		/* save header for merging */
		memcpy( (void*)&g_frc_osd_emul.osd_hdr_sav[fb_dev_id], (void*)&osd_hdr, sizeof(FBDEV_OSD_HDR_T) );

		if ( fb_dev_id == LX_FBDEV_ID_CSR0 )
		{
			g_frc_osd_emul.b_crsr_draw_req = TRUE;
#ifdef FBDEV_UD_CRSR_ASYNC_DRAW
			PRINTK("[fb] ready for async cursor..\n");
			OS_StartTimer ( &g_crsr_draw_noti_timer, FBDEV_L9B1_CrsrDrawNotiTimerCallback, OS_TIMER_TIMEOUT, 5, 0x0 );
#else
			FBDEV_L9B1_MergePOSD( fb_dev_id, TRUE );
#endif
		}
		else
		{
			FBDEV_L9B1_MergePOSD( fb_dev_id, TRUE );
		}
	}
	else
	{
		ULONG	flags;

		spin_lock_irqsave(&g_osd_hdr_spinlock, flags);
		memcpy( (void*)hCanvas->uiOSDHeaderPTR, (void*)&osd_hdr, sizeof(FBDEV_OSD_HDR_T) );
//		FBDEV_REG_MEMCPY( hCanvas->uiOSDHeaderPTR, &osd_hdr, sizeof(FBDEV_OSD_HDR_T)>>2 );
		spin_unlock_irqrestore(&g_osd_hdr_spinlock, flags);
	}
#else
	/* TODO: write register. test more !! */
	switch( fb_dev_id )
	{
		case 0:	FBDEV_REG_MEMCPY( &g_fbdev_l9_reg->osd0_hdr0, &osd_hdr, sizeof(FBDEV_OSD_HDR_T)/4);
				break;
		case 1:	FBDEV_REG_MEMCPY( &g_fbdev_l9_reg->osd1_hdr0, &osd_hdr, sizeof(FBDEV_OSD_HDR_T)/4);
				break;
		case 2:	FBDEV_REG_MEMCPY( &g_fbdev_l9_reg->osd2_hdr0, &osd_hdr, sizeof(FBDEV_OSD_HDR_T)/4);
				break;
		case 3:	FBDEV_REG_MEMCPY( &g_fbdev_l9_reg->crsr_hdr0, &osd_hdr, sizeof(FBDEV_OSD_HDR_T)/4);
				break;
	}
#endif

//func_exit:
	FBDEV_L9B1_WriteFRCHeader( );

	/* temporary code for fixing pixel corruption in COSMO only */
	{
		UINT32 val;
		val = 0x80001163;
		memcpy( (void *)&g_fbdev_l9_reg->osd0_sc_cfg , (void *)&val , 4);
	}
//	up(&fbdev_sem_l9);
	return RET_OK;
}

/*========================================================================================
	Implementation Group (ISR)
========================================================================================*/
void FBDEV_L9B1_ISRTickHandler ( void )
{
	/* if new vsync arrived, swap osd idx */
	if ( g_fbdev_frc_ctrl.b_frc_UD_enable
		&& g_frc_osd_emul.my_vsync >= 0 && g_frc_osd_emul.my_vsync != g_fbdev_vsync_cnt )
	{
		PRINTK("OSD Vsync...[%d]\n", g_frc_osd_emul.osd_idx );
		g_frc_osd_emul.osd_idx ^= 1;
		g_frc_osd_emul.my_vsync = -1;
	}
#ifdef	FBDEV_USE_SW_OSD_EMULATION
	{
		int	i;

		for( i=0; i<LX_FBDEV_ID_MAX; i++ ) 
		{
			if ( g_sw_osd_emul.fb[i].mask && g_sw_osd_emul.fb[i].my_vsync >= 0 && g_sw_osd_emul.fb[i].my_vsync != g_fbdev_vsync_cnt )
			{
				PRINTK("[fb] osd %d: sw osd Tick...\n", i );
#if 1
				g_sw_osd_emul.fb[i].osd_idx ^= 1;
#endif
				g_sw_osd_emul.fb[i].my_vsync = -1;
			}
		}
	}
#endif
}

/*========================================================================================
	Implementation Group (CAPS)
========================================================================================*/
BOOLEAN	FBDEV_L9B1_CheckCaps ( FBDEV_L9_HW_CAPS_T caps, UINT32 args )
{
	BOOLEAN	result = FALSE;

	switch(caps)
	{
		case FBDEV_L9_HW_CAPS_TRID_CTRL:
		{
#if 0
#ifdef FBDEV_USE_POSD1_1920x1080
			int	fb_dev_id = (int)args;
		
			/* raxis.lim (2011/11/23) 
			 * blend mode should support 3D OSD.
			 * POSD1 should support detect 3D OSD request. (refer to FBDEV_L9B1_SetTridCtrl )
			 */
			if(!g_fbdev_frc_ctrl.b_frc_sep_enable) result = TRUE;
			if(fb_dev_id == LX_FBDEV_ID_OSD1) result = TRUE;

			PRINTK("[fb] OSD %d -- TRID CTRL (%s)\n", fb_dev_id, (result)? "OK":"N/A" );
#else
			if(!g_fbdev_frc_ctrl.b_frc_sep_enable) result = TRUE;
#endif
#endif
		}
		break;

		default:
		{
			FBDEV_ERROR("unknown or not supported caps %d\n", caps ); 
		}
		break;
	}	

	return result;
}

/*========================================================================================
	Implementation Group (GPU SUPPORT)
========================================================================================*/
/** set CLK
 *
 *
 */
void FBDEV_L9B1_SetGPUClk ( FBDEV_L9_HW_GPU_CLK_T clk )
{
	CTOP_CTRL_L9B_RdFL(ctr01_reg_clk_div_sel);
    CTOP_CTRL_L9B_RdFL(ctr29_reg_swrst);


	printk("[fb] GPU CLOCK [old ] 0014: 0x%08x 0084: 0x%08x\n", CTOP_CTRL_L9B_Rd(ctr01_reg_clk_div_sel), CTOP_CTRL_L9B_Rd(ctr29_reg_swrst) );

	switch( clk )
	{
		case FBDEV_L9_HW_GPU_CLK_528MHZ:
			CTOP_CTRL_L9B_Wr01(ctr01_reg_clk_div_sel, 	gpuclk_sel,		0x4 /* default value */ );
			CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, 		gpu_high_speed, 0x1 );
			printk("[fb] GPU CLOCK [528M] 0014:0x%08x 0084:0x%08x\n", CTOP_CTRL_L9B_Rd(ctr01_reg_clk_div_sel), CTOP_CTRL_L9B_Rd(ctr29_reg_swrst) );
		break;

		case FBDEV_L9_HW_GPU_CLK_400MHZ:
#if 1
			CTOP_CTRL_L9B_Wr01(ctr01_reg_clk_div_sel, 	gpuclk_sel, 	0x3 /* 400 MHz value */ );
			CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, 		gpu_high_speed, 0x0 ); 
#else
			CTOP_CTRL_L9B_Wr(ctr01_reg_clk_div_sel, 	0x00600000 );
			CTOP_CTRL_L9B_Wr(ctr29_reg_swrst, 			0x00000000 );
#endif
			printk("[fb] GPU CLOCK [400M] 0014:0x%08x 0084:0x%08x\n", CTOP_CTRL_L9B_Rd(ctr01_reg_clk_div_sel), CTOP_CTRL_L9B_Rd(ctr29_reg_swrst) );
		break;

		default: /* 320M */
			CTOP_CTRL_L9B_Wr01(ctr01_reg_clk_div_sel, 	gpuclk_sel, 	0x4 /* default */ );
			CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, 		gpu_high_speed, 0x0 );
			printk("[fb] GPU CLOCK [320M] 0014:0x%08x 0084:0x%08x\n", CTOP_CTRL_L9B_Rd(ctr01_reg_clk_div_sel), CTOP_CTRL_L9B_Rd(ctr29_reg_swrst) );
		return;
	}

	CTOP_CTRL_L9B_WrFL(ctr01_reg_clk_div_sel);
    CTOP_CTRL_L9B_WrFL(ctr29_reg_swrst);
}

/*========================================================================================
	Implementation Group (SW OSD)
========================================================================================*/
static void FBDEV_L9B1_InitSwOSD	( int fb_dev_id, FBDEV_SW_OSD_EMUL_TYPE_MASK_T mask )
{
	if ( g_sw_osd_emul.fb[fb_dev_id].mask == FBDEV_SW_OSD_EMUL_MASK_NONE )
	{
		LX_GFX_SURFACE_SETTING_T surface_info;

		int width		= g_fbdev_cfg->fb[fb_dev_id].max_xres;
		int height		= g_fbdev_cfg->fb[fb_dev_id].max_yres;

		printk("[fb] %s : OSD(%d) init SW OSD function !!!\n", __F__, fb_dev_id );
		g_sw_osd_emul.fb[fb_dev_id].osd_surf[0] = FBDEV_GFX_CreateSurface( width, height, 0x0 );
		FBDEV_CHECK_CODE( g_sw_osd_emul.fb[fb_dev_id].osd_surf[0]<0, return, "can't alloc SW OSD surface\n");
		g_sw_osd_emul.fb[fb_dev_id].osd_surf[1] = FBDEV_GFX_CreateSurface( width, height, 0x0 );
		FBDEV_CHECK_CODE( g_sw_osd_emul.fb[fb_dev_id].osd_surf[1]<0, return, "can't alloc SW OSD surface\n");

		FBDEV_GFX_GetSurfaceInfo ( g_sw_osd_emul.fb[fb_dev_id].osd_surf[0], &surface_info );
		g_sw_osd_emul.fb[fb_dev_id].bitmap_ptr[0] = (UINT32)surface_info.phys_addr;

		FBDEV_GFX_GetSurfaceInfo ( g_sw_osd_emul.fb[fb_dev_id].osd_surf[1], &surface_info );
		g_sw_osd_emul.fb[fb_dev_id].bitmap_ptr[1] = (UINT32)surface_info.phys_addr;

		g_sw_osd_emul.fb[fb_dev_id].osd_idx	 = 0;

		memset( &g_sw_osd_emul.fb[fb_dev_id].src_rect, 0x0, sizeof(LX_RECT_T));
		memset( &g_sw_osd_emul.fb[fb_dev_id].dst_rect, 0x0, sizeof(LX_RECT_T));
	}

	g_sw_osd_emul.fb[fb_dev_id].mask |= mask;
}

static void FBDEV_L9B1_ExitSwOSD	( int fb_dev_id, FBDEV_SW_OSD_EMUL_TYPE_MASK_T mask )
{
	if ( g_sw_osd_emul.fb[fb_dev_id].mask & mask )
	{
		g_sw_osd_emul.fb[fb_dev_id].mask &= ~mask;

		if ( g_sw_osd_emul.fb[fb_dev_id].mask == FBDEV_SW_OSD_EMUL_MASK_NONE )
		{
			printk("[fb] %s : OSD(%d) exit SW OSD function !!!\n", __F__, fb_dev_id );

			if ( g_sw_osd_emul.fb[fb_dev_id].osd_surf[0] >= 0 ) FBDEV_GFX_FreeSurface( g_sw_osd_emul.fb[fb_dev_id].osd_surf[0] );
			if ( g_sw_osd_emul.fb[fb_dev_id].osd_surf[1] >= 0 ) FBDEV_GFX_FreeSurface( g_sw_osd_emul.fb[fb_dev_id].osd_surf[1] );
		}
	}
}

/*========================================================================================
	Implementation Group (FRC)
========================================================================================*/
/** set 3D ctrl 
 *
 *	almost 3D configuration is done in fbdev_hw_l9.c.
 *	this function will do something related to the chip revison
 *	this function is called before writing 3D related register ( refer FBDEV_L9_SetTridCtrl )
 *	you should ovrride some default 3D related register if you need.
 *
 *	@param pConnCtrl [IN] connection control data
 *	@return RET_OK if success, RET_ERROR otherwise
 *
 */
int FBDEV_L9B1_SetTridCtrl( int layer, LX_FBDEV_TRID_CTRL_T* ctrl )
{
	/* save 3D mode. this value is used inside FRC separate/blend change */
	g_frc_trid_ctrl[layer] = *ctrl;

	return RET_OK;
}

/** set OSD output ctrl
 *
 *	@param pConnCtrl [IN] connection control data
 *	@return RET_OK if success, RET_ERROR otherwise
 *
 */
int     FBDEV_L9B1_SetConnCtrl           (LX_FBDEV_CONN_CTRL_T* pConnCtrl)
{
	int ret = RET_OK;

	/* standard connection with LDVS */
	if ( pConnCtrl->conn_type == LX_FBDEV_CONN_STD_LVDS )
	{
        g_fbdev_frc_ctrl.b_frc_UD_enable	= FALSE;	/* clear UD flag */
		g_fbdev_frc_ctrl.b_frc_sep_enable	= FALSE;	/* clear SEP flag */
		FBDEV_L9B1_ExitHDOSDMerge( );					/* shutdown OSD merge */

		g_fbdev_l9_reg->osd0_cfg.osd0_double_w = 0x0;	/* disable OSD horizontal double */
		g_fbdev_l9_reg->osd0_cfg.osd0_double_h = 0x0;	/* disable OSD vertical   double */

        /* reinialize the OSD order. this function will make OSD_MUX L/R to be default
         * TODO: FRC control function should be H/W dependent part.
         */
        FBDEV_L9_InitOSDZOrder ( );

		/* configure the POSD layer for Bi-Cubic scaler */
		OSD_L9A0_Wr01( osd_scaler_mux, reg_osd_scaler_mux, 0x0 );	/* enhance POSD0 */
		OSD_L9A0_WrFL( osd_scaler_mux );

        /* make path mode to be default ( OSD out to both R/L path ) */
        OSD_L9A0_RdFL( osd0_3d_path_mode );
        OSD_L9A0_RdFL( osd1_3d_path_mode );
        OSD_L9A0_RdFL( osd2_3d_path_mode );
        OSD_L9A0_RdFL( crsr_3d_path_mode );

        OSD_L9A0_Wr02(osd0_3d_path_mode, osd0_path_sel,     0x0,    /* LR */
                                         osd0_path_buf_half,0x0 );  /* disable path buf half (checked by ±è¿õÇö) */
        OSD_L9A0_Wr01(osd1_3d_path_mode, osd1_path_sel,     0x0 );  /* LR */
        OSD_L9A0_Wr01(osd2_3d_path_mode, osd2_path_sel,     0x0 );  /* LR */
        OSD_L9A0_Wr01(crsr_3d_path_mode, crsr_path_sel,     0x0 );  /* LR */

        OSD_L9A0_WrFL( osd0_3d_path_mode );
        OSD_L9A0_WrFL( osd1_3d_path_mode );
        OSD_L9A0_WrFL( osd2_3d_path_mode );
        OSD_L9A0_WrFL( crsr_3d_path_mode );

		/* make OSD to be FHD size */
		OSD_L9A0_RdFL( osd_disp_size );
		OSD_L9A0_Wr02( osd_disp_size, 	disp_w,		1920,	/* 1920 */
										disp_h,		1080 );	/* 1080 */
		OSD_L9A0_WrFL( osd_disp_size );

        FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[0]->pixel );
        FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[1]->pixel );
		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[2]->pixel );
        FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[3]->pixel );

        /* TODO: add more code */
#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
		OSD_L9A0_RdFL( osd1_sc_cfg );
		OSD_L9A0_Wr01( osd1_sc_cfg, osd1_sc_h_phase_off, 0x0 );			/* make default value */
		OSD_L9A0_WrFL( osd1_sc_cfg );
#endif
    }
    /* UD connection with FRCIII */
    else if ( pConnCtrl->conn_type == LX_FBDEV_CONN_UD_FRCIII_BLEND )
	{
		printk("^g^[fb] FRC UD BLEND mode..\n");

		if ( g_fbdev_frc_ctrl.b_frc_UD_enable ) { /* ignore multiple requetst */ goto func_exit; }

		/* disable FRC path */
        g_fbdev_frc_ctrl.b_frc_UD_enable	= TRUE;			/* set   UD flag */
        g_fbdev_frc_ctrl.b_frc_sep_enable	= FALSE;		/* clear SEP flag */

		FBDEV_L9B1_InitHDOSDMerge( );						/* init OSD merge */

		g_fbdev_l9_reg->osd_frc_if_cfg.osd_frc_if_en = 0x0; /* disable FRC sep */

		g_fbdev_l9_reg->osd0_cfg.osd0_en = 0x1;			/* on  POSD0 for merging (Left  FHD path) */
		g_fbdev_l9_reg->osd1_cfg.osd1_en = 0x1;			/* on  POSD1 for merging (Right FHD path) */
		g_fbdev_l9_reg->osd2_cfg.osd2_en = 0x0;			/* off POSD2 for merging */
		g_fbdev_l9_reg->crsr_cfg.crsr_en = 0x0;			/* off POSD3 for merging */

        /* reinialize the OSD order. this function will make OSD_MUX L/R to be default
         * TODO: FRC control function should be H/W dependent part.
         */
		FBDEV_L9_InitOSDZOrder ( );

		/* configure the POSD layer for Bi-Cubic scaler */
		OSD_L9A0_Wr01( osd_scaler_mux, reg_osd_scaler_mux, 0x2 );	/* DO NOT enhance the scaler for POSD0 */
		OSD_L9A0_WrFL( osd_scaler_mux );

		/* configure L/R mux path */
		{
			UINT32 val;
#ifdef FBDEV_USE_UD_TEST_MODE
			val =  0x3012;	/* VIDEO - POSD0 for UD L path */
#else
			val =  0x3202;	/* VIDEO - POSD0 for UD L path */
#endif
			memcpy((void *)&g_fbdev_l9_reg->osd_layer_mux_left, (void*)&val, 4);
			val =  0x3212;	/* VIDEO - POSD1 for UD R path*/
			memcpy((void *)&g_fbdev_l9_reg->osd_layer_mux_right,(void*)&val, 4);
		}

		/* make path mode to be default ( OSD out to both R/L path ) */
		OSD_L9A0_RdFL( osd0_3d_path_mode );
		OSD_L9A0_RdFL( osd1_3d_path_mode );
		OSD_L9A0_RdFL( osd2_3d_path_mode );
		OSD_L9A0_RdFL( crsr_3d_path_mode );

		OSD_L9A0_Wr02(osd0_3d_path_mode, osd0_path_sel,		0x1,	/* POSD0 is passed to Left */
										 osd0_path_buf_half,0x0 );	/* disable path buf half (checked by ±è¿õÇö) */
#ifdef FBDEV_USE_UD_TEST_MODE
		OSD_L9A0_Wr02(osd1_3d_path_mode, osd1_path_sel,		0x1,	/* POSD1 is passed to Right */
										 osd1_path_buf_half,0x0 );	/* disable path buf half (checked by ±è¿õÇö) */
#else
		OSD_L9A0_Wr02(osd1_3d_path_mode, osd1_path_sel,		0x2,	/* POSD1 is passed to Right */
										 osd1_path_buf_half,0x0 );	/* disable path buf half (checked by ±è¿õÇö) */
#endif
		OSD_L9A0_Wr01(osd2_3d_path_mode, osd2_path_sel,		0x0	); 	/* LR */
		OSD_L9A0_Wr01(crsr_3d_path_mode, crsr_path_sel,		0x0	); 	/* LR */

		OSD_L9A0_WrFL( osd0_3d_path_mode );
		OSD_L9A0_WrFL( osd1_3d_path_mode );
		OSD_L9A0_WrFL( osd2_3d_path_mode );
		OSD_L9A0_WrFL( crsr_3d_path_mode );

		/* make OSD to be UI size */
		OSD_L9A0_RdFL( osd_disp_size );
#ifdef FBDEV_USE_UD_TEST_MODE
		OSD_L9A0_Wr02( osd_disp_size, 	disp_w,		1920,	/* output size is FHD for viewing UD emulated OSD */
										disp_h,		1080 );	/* output size is FHD for viewing UD emulated OSD */
#else
		OSD_L9A0_Wr02( osd_disp_size, 	disp_w,		1920,	/* not 1920<<1. confirmed by ±è¿õÇö  */
										disp_h,		2160 );	/* 1080<<1 . 	confirmed by ±è¿õÇö */
#endif
		OSD_L9A0_WrFL( osd_disp_size );

		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[0]->pixel );
		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[1]->pixel );
		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[2]->pixel );
		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[3]->pixel );

        /* TODO: add more code */

#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
		/* turn on the OSD boundary mode workaround for fixing the boundary split problem between L picture and R picture. 
		 * the below value is selected based on test with ±è¿õÇö. */
		OSD_L9A0_RdFL( osd1_sc_cfg );
		OSD_L9A0_Wr01( osd1_sc_cfg, osd1_sc_h_phase_off, 0x700 );	/* default 0x0 --> 0x700(shift the R picture to the left by approximately 2.x pixel) ÀÔ·Â°ª ºÐ¼®ÇÏÁö ¸»°Í */
		OSD_L9A0_WrFL( osd1_sc_cfg );
#endif
	}
    /* standard connection with FRCIII */
    else if ( pConnCtrl->conn_type == LX_FBDEV_CONN_STD_FRCIII_BLEND )
    {
		printk("^g^[fb] FRC STD BLEND mode..\n");

        g_fbdev_frc_ctrl.b_frc_sep_enable	= FALSE;		/* clear SEP flag */
        g_fbdev_frc_ctrl.b_frc_UD_enable	= FALSE;		/* clear UD flag */
		FBDEV_L9B1_ExitHDOSDMerge( );						/* shutdown OSD merge */

		g_fbdev_l9_reg->osd0_cfg.osd0_double_w = 0x0;		/* disable OSD horizontal double */
		g_fbdev_l9_reg->osd0_cfg.osd0_double_h = 0x0;		/* disable OSD vertical   double */
		g_fbdev_l9_reg->osd_frc_if_cfg.osd_frc_if_en = 0x0; /* disable FRC sep */

		/* rewrite OSD header to update current change */
        FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[0]->pixel );
        FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[1]->pixel );
        FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[2]->pixel );
        FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[3]->pixel );

        /* reinialize the OSD order. this function will make OSD_MUX L/R to be default
         * TODO: FRC control function should be H/W dependent part.
         */
		FBDEV_L9_InitOSDZOrder ( );

		/* configure the POSD layer for Bi-Cubic scaler */
		OSD_L9A0_Wr01( osd_scaler_mux, reg_osd_scaler_mux, 0x0 );	/* enhance POSD0 */
		OSD_L9A0_WrFL( osd_scaler_mux );

		/* make path mode to be default ( OSD out to both R/L path ) */
		OSD_L9A0_RdFL( osd0_3d_path_mode );
		OSD_L9A0_RdFL( osd1_3d_path_mode );
		OSD_L9A0_RdFL( osd2_3d_path_mode );
		OSD_L9A0_RdFL( crsr_3d_path_mode );

		OSD_L9A0_Wr02(osd0_3d_path_mode, osd0_path_sel,		0x1,   /* LR -> L only */
										 osd0_path_buf_half,0x0 ); /* disable path buf half (checked by ±è¿õÇö) */
		OSD_L9A0_Wr01(osd1_3d_path_mode, osd1_path_sel,		0x1	); /* LR -> L only */
		OSD_L9A0_Wr01(osd2_3d_path_mode, osd2_path_sel,		0x1	); /* LR -> L only */
		OSD_L9A0_Wr01(crsr_3d_path_mode, crsr_path_sel,		0x1	); /* LR -> L only */

		OSD_L9A0_WrFL( osd0_3d_path_mode );
		OSD_L9A0_WrFL( osd1_3d_path_mode );
		OSD_L9A0_WrFL( osd2_3d_path_mode );
		OSD_L9A0_WrFL( crsr_3d_path_mode );

		/* make OSD to be FHD size */
		OSD_L9A0_RdFL( osd_disp_size );
		OSD_L9A0_Wr02( osd_disp_size, 	disp_w,		1920,	/* 1920 */
										disp_h,		1080 );	/* 1080 */
		OSD_L9A0_WrFL( osd_disp_size );

        /* TODO: add more code */
#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
		OSD_L9A0_RdFL( osd1_sc_cfg );
		OSD_L9A0_Wr01( osd1_sc_cfg, osd1_sc_h_phase_off, 0x0 );			/* make default value */
		OSD_L9A0_WrFL( osd1_sc_cfg );
#endif
    }
    /* standard connection with FRCIII (OSD/VIDEO separated mode) */
    else if ( pConnCtrl->conn_type == LX_FBDEV_CONN_STD_FRCIII_SEPARATE )
    {
		printk("^g^[fb] FRC STD SEP mode..\n");

        if ( g_fbdev_frc_ctrl.b_frc_sep_enable ) { /* ignore multiple requetst */ goto func_exit; }

        g_fbdev_frc_ctrl.b_frc_UD_enable	= FALSE;	/* clear UD flag */
        g_fbdev_frc_ctrl.b_frc_sep_enable	= TRUE;		/* set FRC sep flag */
		g_frc_hdr_emul.b_hdr_ready			= FALSE;	/* I want to make header again */
		FBDEV_L9B1_ExitHDOSDMerge( );					/* Exit OSD merge */	/* TEST */

//		g_fbdev_l9_reg->osd0_cfg.osd0_en = 0x1;			/* on  PoSD0 for merging */
//		g_fbdev_l9_reg->osd1_cfg.osd1_en = 0x0;			/* off POSD1 for merging */
//		g_fbdev_l9_reg->osd2_cfg.osd2_en = 0x0;			/* off POSD2 for merging */
//		g_fbdev_l9_reg->crsr_cfg.crsr_en = 0x0;			/* off POSD3 for merging */

		g_fbdev_l9_reg->osd0_cfg.osd0_double_w = 0x0;	/* disable OSD horizontal double */
		g_fbdev_l9_reg->osd0_cfg.osd0_double_h = 0x0;	/* disable OSD vertical   double */

        {
			/* TODO: below codes are made for test purpose. make the release version */
			UINT32 val;
#if 0
	        /* reinialize the OSD order. this function will make OSD_MUX L/R to be default
		 	 * TODO: FRC control function should be H/W dependent part.
			 */
			FBDEV_L9_InitOSDZOrder ( );
#endif
			/* configure the POSD layer for Bi-Cubic scaler */
			OSD_L9A0_Wr01( osd_scaler_mux, reg_osd_scaler_mux, 0x0 );	/* enhance POSD0 */
			OSD_L9A0_WrFL( osd_scaler_mux );

			/* make path mode to be default ( OSD out to both R/L path ) */
			OSD_L9A0_RdFL( osd0_3d_path_mode );
			OSD_L9A0_RdFL( osd1_3d_path_mode );
			OSD_L9A0_RdFL( osd2_3d_path_mode );
			OSD_L9A0_RdFL( crsr_3d_path_mode );

			OSD_L9A0_Wr02(osd0_3d_path_mode, osd0_path_sel,		0x2,	/* R only */
											 osd0_path_buf_half,0x0 );	/* disable path buf half (checked by ±è¿õÇö) */
#ifdef FBDEV_USE_POSD1_1920x1080
			/* POSD1 uses the blend path for the normal 2D mode.
			 * when 3D mode enabled, POSD1 uses separated mode.
			 */
			if( g_frc_trid_ctrl[LX_FBDEV_ID_OSD1].mode == LX_FBDEV_TRID_MODE_NONE /* 3D off */ )
			{ 
				OSD_L9A0_Wr01(osd1_3d_path_mode, osd1_path_sel,	0x1	);	/* L only */
			}
			else
#else
			{
				OSD_L9A0_Wr01(osd1_3d_path_mode, osd1_path_sel,	0x2	);	/* R only */
			}
#endif
			OSD_L9A0_Wr01(osd2_3d_path_mode, osd2_path_sel,		0x2	);	/* R only */
			OSD_L9A0_Wr01(crsr_3d_path_mode, crsr_path_sel,		0x2	);	/* R only */

			OSD_L9A0_WrFL( osd0_3d_path_mode );
			OSD_L9A0_WrFL( osd1_3d_path_mode );
			OSD_L9A0_WrFL( osd2_3d_path_mode );
			OSD_L9A0_WrFL( crsr_3d_path_mode );

			/* make OSD to be FHD size */
			OSD_L9A0_RdFL( osd_disp_size );
			OSD_L9A0_Wr02( osd_disp_size, 	disp_w,		1920,	/* 1920 (0x780) */
											disp_h,		1080 );	/* 1080 (0x438) */
			OSD_L9A0_WrFL( osd_disp_size );

			// 3. OSD scaler buffer mode enable
			val = 0x80001163;
			memcpy((void *)&g_fbdev_l9_reg->osd0_sc_cfg , (void *)&val , 4);
			memcpy((void *)&g_fbdev_l9_reg->osd1_sc_cfg , (void *)&val , 4);
			memcpy((void *)&g_fbdev_l9_reg->osd2_sc_cfg , (void *)&val , 4);

            /* 0x2210 -> 0x2102 changed. request by ±è¿õÇö
             * DO NOT change layer mux value. this if fixed at L9B1 metal revisin.
             * For more information, please contact to ±è¿õÇö
			 *
			 * support both L9B0 and L9B1 to simulate L9B1 function on L9B0 board.
             */
			if ( lx_chip_rev() >= LX_CHIP_REV(L9,B1) )	val = 0x2102;	/* POSD1 - POSD0 */
			else										val = 0x2210;	/* POSD1 - POSD0 */
            memcpy((void *)&g_fbdev_l9_reg->osd_layer_mux_right , (void *)&val , 4);
#ifdef FBDEV_USE_POSD1_1920x1080
			/* TEST : MEMC bug fix */
			val = 0x3012;	/* VIDEO - POSD1 - POSD0 */
            memcpy((void *)&g_fbdev_l9_reg->osd_layer_mux_left , (void *)&val , 4);
#endif
			/* 0x21 for B0, 0x01 for B1 ( checked by ±è¿õÇö )
			 * L9B1 has the different concept for osd_frc_if_cfg.
			 * for more information, please contact to ±è¿õÇö and read the latest OSD manual.
			 *
			 * support both L9B0 and L9B1 to simulate L9B1 function on L9B0 board.
	 	 	 */
			if ( lx_chip_rev() >= LX_CHIP_REV(L9,B1) )	val = 0x01;
			else										val = 0x21;
			memcpy((void *)&g_fbdev_l9_reg->osd_frc_if_cfg , (void *)&val , 4);
		}

		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[0]->pixel );
		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[1]->pixel );
		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[2]->pixel );
		FBDEV_WriteOSDHeader( &g_dm_ctx->winfo[3]->pixel );

#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
		OSD_L9A0_RdFL( osd1_sc_cfg );
		OSD_L9A0_Wr01( osd1_sc_cfg, osd1_sc_h_phase_off, 0x0 );			/* make default value */
		OSD_L9A0_WrFL( osd1_sc_cfg );
#endif
    }
    else
    {
		g_fbdev_frc_ctrl.b_frc_sep_enable = 0;
		g_fbdev_frc_ctrl.b_frc_UD_enable  = 0;
		FBDEV_WARN("conn(0x%x) not supported\n", pConnCtrl->conn_type );
		ret = RET_ERROR;
    }

func_exit:
	return ret;
}

/** write FRC releated heaer
 *
 *
 */
static void FBDEV_L9B1_WriteFRCHeader( void )
{
	/* write OSD header only when FRC header emulation enabled */
	if ( g_fbdev_frc_ctrl.b_frc_sep_enable )
	{
		/* make the initial FRC header. this is processed only once at the first call */
		if ( g_frc_hdr_emul.b_hdr_ready == FALSE )
		{
			UINT32				val;
		    OSD_L9A0_FRC_HDR_0  osd_frc_hdr_0;   // 0x1ba4 : ''
			OSD_L9A0_FRC_HDR_1  osd_frc_hdr_1;   // 0x1ba8 : ''
			OSD_L9A0_FRC_HDR_2  osd_frc_hdr_2;   // 0x1bac : ''
			OSD_L9A0_FRC_HDR_3  osd_frc_hdr_3;   // 0x1bb0 : ''
			OSD_L9A0_FRC_HDR_4  osd_frc_hdr_4;   // 0x1bb4 : ''
			OSD_L9A0_FRC_HDR_5  osd_frc_hdr_5;   // 0x1bb8 : ''

			/* enable the 2nd routine if you want to use the cleared memory before setting frc header */
#if 1
			OSD_L9A0_RdFL(osd_frc_hdr_0); val = OSD_L9A0_Rd(osd_frc_hdr_0);	memcpy( &osd_frc_hdr_0,	&val, sizeof(UINT32));
			OSD_L9A0_RdFL(osd_frc_hdr_1); val = OSD_L9A0_Rd(osd_frc_hdr_1); memcpy( &osd_frc_hdr_1,	&val, sizeof(UINT32));
			OSD_L9A0_RdFL(osd_frc_hdr_2); val = OSD_L9A0_Rd(osd_frc_hdr_2); memcpy( &osd_frc_hdr_2,	&val, sizeof(UINT32));
			OSD_L9A0_RdFL(osd_frc_hdr_3); val = OSD_L9A0_Rd(osd_frc_hdr_3);	memcpy( &osd_frc_hdr_3,	&val, sizeof(UINT32));
			OSD_L9A0_RdFL(osd_frc_hdr_4); val = OSD_L9A0_Rd(osd_frc_hdr_4);	memcpy( &osd_frc_hdr_4,	&val, sizeof(UINT32));
			OSD_L9A0_RdFL(osd_frc_hdr_5); val = OSD_L9A0_Rd(osd_frc_hdr_5);	memcpy( &osd_frc_hdr_5,	&val, sizeof(UINT32));
#else
			memset( &osd_frc_hdr_0, 0x0, sizeof(UINT32) );
			memset( &osd_frc_hdr_1, 0x0, sizeof(UINT32) );
			memset( &osd_frc_hdr_2, 0x0, sizeof(UINT32) );
			memset( &osd_frc_hdr_3, 0x0, sizeof(UINT32) );
			memset( &osd_frc_hdr_4, 0x0, sizeof(UINT32) );
			memset( &osd_frc_hdr_5, 0x0, sizeof(UINT32) );
#endif
			/* osd_frc_hdr_0.osdf_frc_hdr_frame_number should be increased at vsync interrupt
		 	 *
			 * hdr0.format
			 *  0001 (AYUV6888), 0010 (ARGB6888), 0101 (AYUV8888), 0100 (ARGB8888)
			 *
			 * hdr0.po (pixel order)
			 *	0001 (AGBR/YUV), 0010 (BGRA/VUYA), 0001 (ABGR/AVUY, 0000: (ARGB/AYUV)
			 *
			 * hdr0.lr (Left/Right)
			 *  1 (R), 0 (L)
			 *
			 * hdr0.3d (3D bitmap indicator) - this value should be set for 2D mode or 3D mode with the same L/R image
			 *  1 (3D), 0 (2d)
			 *
			 * hdr0.header_prefix
			 *  1010 (valid OSD), 0000
			 */
			osd_frc_hdr_0.osd_frc_hdr_format		= 0x4; /* ARGB8888 */
			osd_frc_hdr_0.osd_frc_hdr_po			= 0x0; /* ARGB/AYUV */
			osd_frc_hdr_0.osd_frc_lr				= 0x0;
			osd_frc_hdr_0.osd_frc_hdr_prefix		= 0xA;
			osd_frc_hdr_0.osd_frc_3d				= 0x0; //( g_fbdev_frc_info.trid_ctrl[layer].mode )? 1:0;
			osd_frc_hdr_0.osd_frc_hdr_frame_number	= 0x0; /* fixed */

			osd_frc_hdr_1.osd_frc_hdr_bmp_width		= 1920;
			osd_frc_hdr_1.osd_frc_hdr_bmp_height	= 1080;

#ifdef	FBDEV_L9B1_VCOM_TEST_SUPPORT
			{
				struct scanvas* hCanvas = &g_dm_ctx->winfo[LX_FBDEV_ID_OSD1]->pixel;
				if ( hCanvas->bBackImageEnable )
				{
                	osd_frc_hdr_1.osd_frc_hdr_bmp_width = 1920;
				}
			}
#endif
			osd_frc_hdr_2.osd_frc_hdr_global_alpha	= 0xff;
			osd_frc_hdr_2.osd_frc_hdr_pe			= 0x1;
			osd_frc_hdr_2.osd_frc_hdr_ge			= 0x0;
			osd_frc_hdr_2.osd_frc_hdr_ce			= 0x0;
			osd_frc_hdr_2.osd_frc_hdr_ou			= 0x1;

			osd_frc_hdr_3.osd_frc_hdr_ck_r			= 0x0;
			osd_frc_hdr_3.osd_frc_hdr_ck_g			= 0x0;
			osd_frc_hdr_3.osd_frc_hdr_ck_b			= 0x0;

			osd_frc_hdr_4.osd_frc_hdr_xoffset		= 0x0;
			osd_frc_hdr_4.osd_frc_hdr_yoffset		= 0x0;

			osd_frc_hdr_5.osd_frc_hdr_disp_width	= 1920;
			osd_frc_hdr_5.osd_frc_hdr_disp_height	= 1080;

			/* save FRC header */
			g_frc_hdr_emul.frc_hdr[0] = *((UINT32*)&osd_frc_hdr_0);
			g_frc_hdr_emul.frc_hdr[1] = *((UINT32*)&osd_frc_hdr_1);
			g_frc_hdr_emul.frc_hdr[2] = *((UINT32*)&osd_frc_hdr_2);
			g_frc_hdr_emul.frc_hdr[3] = *((UINT32*)&osd_frc_hdr_3);
			g_frc_hdr_emul.frc_hdr[4] = *((UINT32*)&osd_frc_hdr_4);
			g_frc_hdr_emul.frc_hdr[5] = *((UINT32*)&osd_frc_hdr_5);

			/* save remapped header */
			g_frc_hdr_emul.frc_hdr_remap[0] = FBDEV_L9_RemapFRCHeader( *((UINT32*)&osd_frc_hdr_0) );
			g_frc_hdr_emul.frc_hdr_remap[1] = FBDEV_L9_RemapFRCHeader( *((UINT32*)&osd_frc_hdr_1) );
			g_frc_hdr_emul.frc_hdr_remap[2] = FBDEV_L9_RemapFRCHeader( *((UINT32*)&osd_frc_hdr_2) );
			g_frc_hdr_emul.frc_hdr_remap[3] = FBDEV_L9_RemapFRCHeader( *((UINT32*)&osd_frc_hdr_3) );
			g_frc_hdr_emul.frc_hdr_remap[4] = FBDEV_L9_RemapFRCHeader( *((UINT32*)&osd_frc_hdr_4) );
			g_frc_hdr_emul.frc_hdr_remap[5] = FBDEV_L9_RemapFRCHeader( *((UINT32*)&osd_frc_hdr_5) );

			PRINTK("^c^[fb] FRC HDR[0 ] = %08x\n", g_frc_hdr_emul.frc_hdr_remap[0] );	// should be 0x00a00004
			PRINTK("^c^[fb] FRC HDR[1 ] = %08x\n", g_frc_hdr_emul.frc_hdr_remap[1] );	// should be 0x00780138
			PRINTK("^c^[fb] FRC HDR[2 ] = %08x\n", g_frc_hdr_emul.frc_hdr_remap[2] );	// should be 0x000024ff
			PRINTK("^c^[fb] FRC HDR[3 ] = %08x\n", g_frc_hdr_emul.frc_hdr_remap[3] );	// should be 0x0
			PRINTK("^c^[fb] FRC HDR[4 ] = %08x\n", g_frc_hdr_emul.frc_hdr_remap[4] );	// should be 0x0
			PRINTK("^c^[fb] FRC HDR[5 ] = %08x\n", g_frc_hdr_emul.frc_hdr_remap[5] );	// should be 0x00780138

			/* remap(swap bit order) should be done before writing register */
			OSD_L9A0_Wr( osd_frc_hdr_0, g_frc_hdr_emul.frc_hdr_remap[0] );
			OSD_L9A0_Wr( osd_frc_hdr_1, g_frc_hdr_emul.frc_hdr_remap[1] );
			OSD_L9A0_Wr( osd_frc_hdr_2, g_frc_hdr_emul.frc_hdr_remap[2] );
			OSD_L9A0_Wr( osd_frc_hdr_3, g_frc_hdr_emul.frc_hdr_remap[3] );
			OSD_L9A0_Wr( osd_frc_hdr_4, g_frc_hdr_emul.frc_hdr_remap[4] );
			OSD_L9A0_Wr( osd_frc_hdr_5, g_frc_hdr_emul.frc_hdr_remap[5] );

			OSD_L9A0_WrFL( osd_frc_hdr_0 );
			OSD_L9A0_WrFL( osd_frc_hdr_1 );
			OSD_L9A0_WrFL( osd_frc_hdr_2 );
			OSD_L9A0_WrFL( osd_frc_hdr_3 );
			OSD_L9A0_WrFL( osd_frc_hdr_4 );
			OSD_L9A0_WrFL( osd_frc_hdr_5 );

			g_frc_hdr_emul.b_hdr_ready = TRUE;
		}
	}
}

/** initialize/exit OSD merge environment 
 *
 *
 */
static void FBDEV_L9B1_InitHDOSDMerge		( void )
{
	LX_GFX_SURFACE_SETTING_T surface_info;

	if( g_frc_osd_emul.merge_osd_hd_surface[0] < 0 )
	{
		g_frc_osd_emul.merge_osd_hd_surface[0] = FBDEV_GFX_CreateSurface( 1920, 1080, 0x0 /*NULL*/ );
		FBDEV_CHECK_CODE( g_frc_osd_emul.merge_osd_hd_surface[0]<0, return, "can't alloc HD[0] surface. crtical error!!\n");

		FBDEV_GFX_GetSurfaceInfo ( g_frc_osd_emul.merge_osd_hd_surface[0], &surface_info );
		g_frc_osd_emul.merge_osd_hd_addr[0]	= (UINT32)surface_info.phys_addr;
	}
	if( g_frc_osd_emul.merge_osd_hd_surface[1] < 0 )
	{
		g_frc_osd_emul.merge_osd_hd_surface[1] = FBDEV_GFX_CreateSurface( 1920, 1080, 0x0 /*NULL*/ );
		FBDEV_CHECK_CODE( g_frc_osd_emul.merge_osd_hd_surface[1]<0, return, "can't alloc HD[1] surface. crtical error!!\n");

		FBDEV_GFX_GetSurfaceInfo ( g_frc_osd_emul.merge_osd_hd_surface[1], &surface_info );
		g_frc_osd_emul.merge_osd_hd_addr[1]	= (UINT32)surface_info.phys_addr;
	}

	if ( g_frc_osd_emul.posd0_hd_surface < 0 )
	{
		g_frc_osd_emul.posd0_hd_surface = FBDEV_GFX_CreateSurface( 1920, 1080, 0x0 /*NULL*/ );
	}
	if ( g_frc_osd_emul.posd1_hd_surface < 0 )
	{
		g_frc_osd_emul.posd1_hd_surface = FBDEV_GFX_CreateSurface( 1920, 1080, 0x0 /*NULL*/ );
	}

#if 0
	if ( g_frc_osd_emul.crsr_bkup_surface < 0 )
	{
		g_frc_osd_emul.crsr_bkup_surface = FBDEV_GFX_CreateSurface( 128, 128, 0x0 /*NULL*/ );
		g_frc_osd_emul.b_crsr_bkup		= FALSE;
		g_frc_osd_emul.crsr_bkup_cx	= 0;
		g_frc_osd_emul.crsr_bkup_cy	= 0;
	}
#endif
}

static void FBDEV_L9B1_ExitHDOSDMerge		( void )
{
	if ( g_frc_osd_emul.merge_osd_hd_surface[0] >= 0 )
	{
		FBDEV_GFX_FreeSurface( g_frc_osd_emul.merge_osd_hd_surface[0] );
		g_frc_osd_emul.merge_osd_hd_surface[0]	= -1;
		g_frc_osd_emul.merge_osd_hd_addr[0] 	= 0x0;
	}

	if ( g_frc_osd_emul.merge_osd_hd_surface[1] >= 0 )
	{
		FBDEV_GFX_FreeSurface( g_frc_osd_emul.merge_osd_hd_surface[1] );
		g_frc_osd_emul.merge_osd_hd_surface[1]	= -1;
		g_frc_osd_emul.merge_osd_hd_addr[1] 	= 0x0;
	}

	if ( g_frc_osd_emul.posd0_hd_surface >= 0 )
	{
		FBDEV_GFX_FreeSurface( g_frc_osd_emul.posd0_hd_surface );
		g_frc_osd_emul.posd0_hd_surface = -1;
	}
	if ( g_frc_osd_emul.posd1_hd_surface >= 0 )
	{
		FBDEV_GFX_FreeSurface( g_frc_osd_emul.posd1_hd_surface );
		g_frc_osd_emul.posd1_hd_surface = -1;
	}

#if 0
	if ( g_frc_osd_emul.crsr_bkup_surface >= 0 )
	{
		FBDEV_GFX_FreeSurface( g_frc_osd_emul.crsr_bkup_surface );
		g_frc_osd_emul.crsr_bkup_surface = -1;
	}
#endif
}

/** merge POSDs into the one single POSD since L9B0 chip has bugs in FRC separated OSD mode and OSD H/W
 *	can show only one POSD.
 *
 *	@param layer [IN] layer(POSD) to be updated
 *	@param b_actie [IN] if TRUE, OSD should be merged. if FALSE, separated mode will be shutdown and OSD should be cleared
 *	@param b_flip [IN] emulated POSD should be fliped for TRUE input.
 *
 *	@note 128x128 cursor is resized to 88x88 to simulate the cursor size in 1920x1080 dimension
 *	@note cursor hot spot is reimplemented
 *
 *	@note To obtain system performnace, this function is NOT general !!!
 */
static void FBDEV_L9B1_MergePOSD ( int layer, BOOLEAN b_flip )
{
	if ( g_fbdev_frc_ctrl.b_frc_UD_enable )	{ FBDEV_L9B1_MergePOSD_1080p( layer, b_flip ); }
	else									{ /* do nothing */; }
} 

static void FBDEV_L9B1_MergePOSD_1080p( int layer, BOOLEAN b_flip )
{
	struct scanvas *posd0_canvas, *posd1_canvas, *posd3_canvas;

	FBDEV_OSD_HDR_T	osd_L_hdr;
	FBDEV_OSD_HDR_T	osd_R_hdr;

	UINT32			merge_osd_hd_addr= 0x0;
	int				posd0_surf		= -1;
	int				posd1_surf		= -1; 
	int				posd3_surf		= -1;

	int				posd0_hd_surf	= -1;		/* POSD0 HD surface to hold stretched image */
	int				posd1_hd_surf	= -1;		/* POSD1 HD surface to hold stretched image */

	int				merge_hd_surf	= -1;
 	static int		merge_hd_surf_list[2]	= {-1,-1};

	int				cx, cy, cw, ch;			/* cursor related value */

	int				osd_idx			= g_frc_osd_emul.osd_idx;
	BOOLEAN			b_gfx_lock		= FALSE;

	/* POSD merge is only valid for UD mode. */
	if( g_fbdev_frc_ctrl.b_frc_UD_enable == FALSE ) return;

	PRINTK("^g^%s : %d,%d\n", __func__, layer, b_flip );

	/* disable multiple enterance !! */
	OS_LockMutex( &g_frc_osd_emul_mtx );

	posd0_canvas = &g_dm_ctx->winfo[0]->pixel;	/* POSD0 */
	posd1_canvas = &g_dm_ctx->winfo[1]->pixel;	/* POSD1 */
	posd3_canvas = &g_dm_ctx->winfo[3]->pixel;	/* POSD3 (CRSR) */

	/* collect OSD view status */
	g_frc_osd_emul.osd_view_flag	= 0x0;
	g_frc_osd_emul.osd_view_flag	|= (g_frc_osd_emul.osd_view_status[0])? OSD_EMUL_VIEW_POSD0: 0x0;
	g_frc_osd_emul.osd_view_flag	|= (g_frc_osd_emul.osd_view_status[1])? OSD_EMUL_VIEW_POSD1: 0x0;
	g_frc_osd_emul.osd_view_flag	|= (g_frc_osd_emul.osd_view_status[3])? OSD_EMUL_VIEW_CRSR:  0x0;
	g_frc_osd_emul.osd_view_flag	|= (posd0_canvas->bBackImageEnable)?	OSD_EMUL_VIEW_BACK_IMG: 0x0;
	g_frc_osd_emul.osd_view_flag	|= (g_frc_osd_emul.osd_view_status[1] && ( posd1_canvas->input_win.w < 1920 || posd1_canvas->input_win.h < 1080 ) )? OSD_EMUL_VIEW_POSD1_RESIZE: 0x0;

	if ( layer == 0 && !(g_frc_osd_emul.osd_view_flag & OSD_EMUL_VIEW_POSD0) )
	{
		goto func_exit;	/* abort POSD merge !! */
	}
	if ( layer == 1 && !(g_frc_osd_emul.osd_view_flag & OSD_EMUL_VIEW_POSD1) )
	{
		goto func_exit;	/* abort POSD merge !! */
	}

	if ( layer == 3 && g_frc_osd_emul.b_crsr_draw_req == FALSE )
	{
		PRINTK("[fb] %s : abort cursor draw..\n", __F__ );
		goto func_exit;	/* abort POSD merge !! */
	}

	/* clear async cursor update */
	if ( g_frc_osd_emul.osd_view_flag & OSD_EMUL_VIEW_CRSR )
	{
		g_frc_osd_emul.b_crsr_draw_req = FALSE;
	}

#if 0
	/* initialize my_vsync control value to be '-1' to avoid incorrect swap of OSD INDEX */
	if ( layer == 0 || layer == 1 ) g_frc_osd_emul.my_vsync = -1;
#endif

	/* get the saved POSD0 header to each L/R header */
	memcpy( (void*)&osd_L_hdr, (void*)&g_frc_osd_emul.osd_hdr_sav[0], sizeof(FBDEV_OSD_HDR_T) ); 
	memcpy( (void*)&osd_R_hdr, (void*)&g_frc_osd_emul.osd_hdr_sav[0], sizeof(FBDEV_OSD_HDR_T) ); 

	/* create HD POSD */
	if ( merge_hd_surf_list[0] < 0 )	merge_hd_surf_list[0]= g_frc_osd_emul.merge_osd_hd_surface[0];
	if ( merge_hd_surf_list[1] < 0 )	merge_hd_surf_list[1]= g_frc_osd_emul.merge_osd_hd_surface[1];


	/* get active OSD frame buffer and destination OSD surface */
	merge_osd_hd_addr	= g_frc_osd_emul.merge_osd_hd_addr[osd_idx];
	merge_hd_surf		= merge_hd_surf_list[osd_idx];

	GFX_LockDevice(); b_gfx_lock = TRUE;

	/* create the access surface to each POSD */
	posd0_surf	= FBDEV_GFX_CreateSurfaceEx( 1280, 720, posd0_canvas->uiBitmapDataYPTR, posd0_canvas->stride );
	posd1_surf	= FBDEV_GFX_CreateSurfaceEx( 1920,1080, posd1_canvas->uiBitmapDataYPTR, posd1_canvas->stride );

	cx			= g_frc_osd_emul.osd_hdr_sav[3].type.crsr.osd_hdr_xpos;
	cy			= g_frc_osd_emul.osd_hdr_sav[3].type.crsr.osd_hdr_ypos;
	cw			= g_frc_osd_emul.osd_hdr_sav[3].type.crsr.osd_hdr_w_mem;
	ch			= g_frc_osd_emul.osd_hdr_sav[3].type.crsr.osd_hdr_h_mem;
	posd3_surf	= FBDEV_GFX_CreateSurfaceEx( cw, ch, g_frc_osd_emul.osd_hdr_sav[3].osd_hdr_ptr_bmp, posd3_canvas->stride );

	/* if POSD0 enabled, make HD surface */
	if ( g_frc_osd_emul.osd_view_flag & OSD_EMUL_VIEW_POSD0 )
	{
		posd0_hd_surf = g_frc_osd_emul.posd0_hd_surface;
		FBDEV_GFX_StretchSurface( posd0_surf, posd0_hd_surf, 0, 0, 1280, 720, 0, 0, 1920, 1080 );
	}

	/* if POSD1 enabled, make HD surface */
	if ( g_frc_osd_emul.osd_view_flag & OSD_EMUL_VIEW_POSD1 )
	{
		if ( g_frc_osd_emul.osd_view_flag & OSD_EMUL_VIEW_POSD1_RESIZE )
		{
			posd1_hd_surf = g_frc_osd_emul.posd1_hd_surface;
			FBDEV_GFX_StretchSurface( posd1_surf, posd1_hd_surf, 0, 0, posd1_canvas->input_win.w, posd1_canvas->input_win.h, 0, 0, 1920, 1080 );
		}
		else
		{
			posd1_hd_surf = posd1_surf;
		}
	}
	
	PRINTK("[fb] UD %d-%d-%d %08x:%08x:%08x (%08x %08x (0x%x 0x%x) Idx %d flip %d layer %d\n", 
												g_frc_osd_emul.osd_view_status[0], g_frc_osd_emul.osd_view_status[1], g_frc_osd_emul.osd_view_status[3], 
												posd0_canvas->uiBitmapDataYPTR,
												posd1_canvas->uiBitmapDataYPTR,
												posd3_canvas->uiBitmapDataYPTR,

												g_frc_osd_emul.merge_osd_hd_addr[0],	
												g_frc_osd_emul.merge_osd_hd_addr[1],

												merge_hd_surf,
												merge_osd_hd_addr,
												osd_idx, b_flip, layer );

	/* check POSD0, POSD1 */
	switch( g_frc_osd_emul.osd_view_flag & (OSD_EMUL_VIEW_POSD0|OSD_EMUL_VIEW_POSD1) )
	{	
		case (OSD_EMUL_VIEW_POSD0|OSD_EMUL_VIEW_POSD1):
		{
			PRINTK("[fb] CASE1 (POSD0|POSD1)\n");
#ifdef FBDEV_USE_UD_TEST_MODE
			printk("<err> test not supported\n");
#else
	#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
			FBDEV_GFX_BlitSurface( posd0_hd_surf, posd1_hd_surf, merge_hd_surf, 0, 0, 1918, 1080, 2, 0, TRUE );
	#else
			FBDEV_GFX_BlitSurface( posd0_hd_surf, posd1_hd_surf, merge_hd_surf, 0, 0, 1920, 1080, 0, 0, TRUE );
	#endif
#endif
		}
		break;

		/* CASE2: only POSD0 is shown */
		case OSD_EMUL_VIEW_POSD0:
		{
			PRINTK("[fb] CASE2 (POSD0)\n");
#ifdef FBDEV_USE_UD_TEST_MODE
			FBDEV_GFX_StretchSurface( posd0_hd_surf, merge_hd_surf, 0, 0, 1920, 1080, 2, 0, 960, 540 );
#else
	#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
			FBDEV_GFX_BlitSurface( posd0_hd_surf, -1, merge_hd_surf, 0, 0, 1918, 1080, 2, 0, FALSE );
	#else
			FBDEV_GFX_BlitSurface( posd0_hd_surf, -1, merge_hd_surf, 0, 0, 1920, 1080, 0, 0, FALSE );
	#endif
#endif
		}
		break;

		/* CASE3: only POSD1 is shown */
		case OSD_EMUL_VIEW_POSD1:
		{
			PRINTK("[fb] CASE3 (POSD1)\n");
#ifdef FBDEV_USE_UD_TEST_MODE
			FBDEV_GFX_StretchSurface( posd1_hd_surf, merge_hd_surf, 0, 0, 1920, 1080, 2, 0, 960, 540 );
#else
	#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
			FBDEV_GFX_BlitSurface( posd1_hd_surf, -1, merge_hd_surf, 0, 0, 1918, 1080, 2, 0, FALSE );
	#else
			FBDEV_GFX_BlitSurface( posd1_hd_surf, -1, merge_hd_surf, 0, 0, 1920, 1080, 0, 0, FALSE );
	#endif
#endif
		}
		break;

		/* CASE4: any POSD is not shown. 
	 	 * In this case, clear HD surface directly and skip POSD stretch !!. this can speed up the POSD merge
		 */
		default:
		{
			PRINTK("[fb] CASE4 (empty)\n");
			FBDEV_GFX_ClearSurface( merge_hd_surf, 1920, 1080 );
		}
		break;
	}

	/* blit cursor */
	if ( g_frc_osd_emul.osd_view_flag & OSD_EMUL_VIEW_CRSR )
	{
		PRINTK("[fb] draw cursor\n");

#if 0
		/* backup cursor region for the future use */
		if ( layer == LX_FBDEV_ID_CSR0 )
		{
			if ( g_frc_osd_emul.b_crsr_bkup )
			{
				printk("[fb] restore backup cursor region\n");
				FBDEV_GFX_BlitSurface( g_frc_osd_emul.crsr_bkup_surface, -1, merge_hd_surf, 0, 0, 128, 128, g_frc_osd_emul.crsr_bkup_cx, g_frc_osd_emul.crsr_bkup_cy, FALSE );
			}

			printk("[fb] save backup cursor region\n");
			g_frc_osd_emul.b_crsr_bkup 	= TRUE;
			g_frc_osd_emul.crsr_bkup_cx	= cx;
			g_frc_osd_emul.crsr_bkup_cy	= cy;
			FBDEV_GFX_BlitSurface( merge_hd_surf, -1, g_frc_osd_emul.crsr_bkup_surface, cx, cy, 128, 128, 0, 0, FALSE );
		}
#endif
		if ( (cx+cw) > 1920 ) cw = (1920-cx);
		if ( (cy+ch) > 1080 ) ch = (1080-cy);
		FBDEV_GFX_BlitSurface( posd3_surf, merge_hd_surf, merge_hd_surf, 0, 0, cw, ch, cx, cy, TRUE );
	}

	/* FHD OSD is ready, write POSD header
	 *
	 * header configuration is very very very weired !! DO NOT understand but just accept :-(
	 * value 
	 */
	{
		struct scanvas* merge_L_canvas = &g_dm_ctx->winfo[0]->pixel;
		struct scanvas* merge_R_canvas = &g_dm_ctx->winfo[1]->pixel;
	
#ifdef FBDEV_USE_UD_TEST_MODE
		osd_L_hdr.osd_hdr_ptr_bmp 			= (UINT32)merge_osd_hd_addr;			/* make pointer to L framebuffer in 1920 width */
		osd_R_hdr.osd_hdr_ptr_bmp 			= (UINT32)merge_osd_hd_addr + (480*4);	/* make pointer to R framebuffer in 1920 width */

		osd_L_hdr.type.osd.osd_hdr_w_out	= 960;		/* confirmed by ±è¿õÇö */
		osd_L_hdr.type.osd.osd_hdr_h_out	= 1080;		/* confirmed by ±è¿õÇö */

		osd_R_hdr.type.osd.osd_hdr_w_out	= 960;		/* confirmed by ±è¿õÇö */
		osd_R_hdr.type.osd.osd_hdr_h_out	= 1080;		/* confirmed by ±è¿õÇö */

		osd_L_hdr.type.osd.osd_hdr_w_mem	= 482;		/* confirmed by ±è¿õÇö */
		osd_L_hdr.type.osd.osd_hdr_h_mem	= 540;		/* confirmed by ±è¿õÇö */
		osd_L_hdr.osd_hdr_wpl 				= (1920*4)>>3;
		osd_L_hdr.type.osd.osd_hdr_xpos 	= 1;		/* confirmed by ±è¿õÇö */

		osd_R_hdr.type.osd.osd_hdr_w_mem	= 480;		/* confirmed by ±è¿õÇö */
		osd_R_hdr.type.osd.osd_hdr_h_mem	= 540;		/* confirmed by ±è¿õÇö */
		osd_R_hdr.osd_hdr_wpl 				= (1920*4)>>3;
		osd_R_hdr.type.osd.osd_hdr_xpos 	= 960;		/* confirmed by ±è¿õÇö */
#else
		osd_L_hdr.osd_hdr_ptr_bmp 			= (UINT32)merge_osd_hd_addr;			/* make pointer to L framebuffer in 1920 width */
		osd_R_hdr.osd_hdr_ptr_bmp 			= (UINT32)merge_osd_hd_addr + (960*4);	/* make pointer to R framebuffer in 1920 width */

	#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
		osd_L_hdr.type.osd.osd_hdr_w_out	= 1920-2;	/* 1918 not 1920. confirmed by ±è¿õÇö */
#else
		osd_L_hdr.type.osd.osd_hdr_w_out	= 1920;	/* confirmed by ±è¿õÇö */
	#endif
		osd_L_hdr.type.osd.osd_hdr_h_out	= 2160;	/* confirmed by ±è¿õÇö */

		osd_R_hdr.type.osd.osd_hdr_w_out	= 1920;	/* confirmed by ±è¿õÇö */
		osd_R_hdr.type.osd.osd_hdr_h_out	= 2160;	/* confirmed by ±è¿õÇö */
	#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
		osd_L_hdr.type.osd.osd_hdr_xpos 	= 2;		/* confirmed by ±è¿õÇö */
#endif
	#ifdef FBDEV_USE_UD_BOUNDARY_WORKAROUND
		osd_L_hdr.type.osd.osd_hdr_w_mem	= 960+1;	/* 961 not 960. confirmed by ±è¿õÇö */
	#else
		osd_L_hdr.type.osd.osd_hdr_w_mem	= 960;	/* confirmed by ±è¿õÇö */
	#endif
		osd_L_hdr.type.osd.osd_hdr_h_mem	= 1080;	/* confirmed by ±è¿õÇö */
		osd_L_hdr.osd_hdr_wpl 				= (1920*4)>>3;

		osd_R_hdr.type.osd.osd_hdr_w_mem	= 960;	/* confirmed by ±è¿õÇö */
		osd_R_hdr.type.osd.osd_hdr_h_mem	= 1080;	/* confirmed by ±è¿õÇö */
		osd_R_hdr.osd_hdr_wpl 				= (1920*4)>>3;
#endif

		PRINTK("[fb] UD wr_reg : in %dx%d out %dx%d stride %d wpl %d \n", 
												osd_L_hdr.type.osd.osd_hdr_w_mem, osd_L_hdr.type.osd.osd_hdr_h_mem,
												osd_L_hdr.type.osd.osd_hdr_w_out, osd_L_hdr.type.osd.osd_hdr_h_out,
												1920*4,
												osd_L_hdr.osd_hdr_wpl );

		memcpy ( (void*)merge_L_canvas->uiOSDHeaderPTR, (void*)&osd_L_hdr, sizeof(FBDEV_OSD_HDR_T) );
		memcpy ( (void*)merge_R_canvas->uiOSDHeaderPTR, (void*)&osd_R_hdr, sizeof(FBDEV_OSD_HDR_T) );
	}
//	smp_mb();

	/* swap osd_idx */
	if ( b_flip ) 
	{
		g_frc_osd_emul.my_vsync = g_fbdev_vsync_cnt; 
	}
	
func_exit:
	if (posd3_surf>=0) FBDEV_GFX_FreeSurface ( posd3_surf );
	if (posd1_surf>=0) FBDEV_GFX_FreeSurface ( posd1_surf );
	if (posd0_surf>=0) FBDEV_GFX_FreeSurface ( posd0_surf );

	if (b_gfx_lock) GFX_UnlockDevice();
	OS_UnlockMutex( &g_frc_osd_emul_mtx );
}

#ifdef FBDEV_UD_CRSR_ASYNC_DRAW
static void FBDEV_L9B1_CrsrDrawTask(struct work_struct *work)
{
	if ( g_fbdev_frc_ctrl.b_frc_UD_enable && g_frc_osd_emul.b_crsr_draw_req )
	{
		PRINTK("[fb] drawing async cursor..\n");
		FBDEV_L9B1_MergePOSD ( LX_FBDEV_ID_CSR0, TRUE );
	}
	else
	{
		PRINTK("[fb] %s : abort cursor draw..\n", __F__ );
	}
}

static void					FBDEV_L9B1_CrsrDrawNotiTimerCallback( ULONG data )
{
	if ( g_fbdev_frc_ctrl.b_frc_UD_enable && g_frc_osd_emul.b_crsr_draw_req )
	{
		schedule_work( &g_crsr_draw_task );
	}
	else
	{
		PRINTK("[fb] %s : abort cursor draw..\n", __F__ );
	}
}
#endif

/*========================================================================================
    chip revision handler
========================================================================================*/
FBDEV_L9_HW_FUNC_T  g_fbdev_l9b1_hw_func =
{
    .L9_InitHAL         = FBDEV_L9B1_InitHAL,
    .L9_SetConnCtrl     = FBDEV_L9B1_SetConnCtrl,
    .L9_WriteOSDHeader  = FBDEV_L9B1_WriteOSDHeader,
    .L9_SetOSDEnable    = FBDEV_L9B1_SetOSDEnable,
    .L9_GetOSDEnable    = FBDEV_L9B1_GetOSDEnable,
    .L9_ISRTickHandler  = FBDEV_L9B1_ISRTickHandler,
    .L9_SetTridCtrl     = FBDEV_L9B1_SetTridCtrl,
	.L9_CheckCaps		= FBDEV_L9B1_CheckCaps,
	.L9_SetGPUClk		= FBDEV_L9B1_SetGPUClk,
};
