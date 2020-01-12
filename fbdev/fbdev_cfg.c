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
 *  main configuration file for fbdev device
 *	fbdev device will teach you how to make device driver with new platform.
 *
 *  author		raxis
 *  version		1.0
 *  date		2010.02.01
 *  note		Additional information.
 *
 *  @addtogroup lg1150_fbdev
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "fbdev_impl.h"
#include <linux/mm.h>
#include <linux/kernel.h>

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define LCD_PANEL_CLOCK				180000   // not be used

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
/** OSD header size
 *
 */
#define OSD_HEADER_SIZE             32		// same as sizeof(LG1150_OSDHEADER_T))

/** OSD header memory address
 *
 */
#define OSD_BASE_ADDR				(gpFbdevRegCfg->frame_base_addr)
#define OSD_HDR_SIZE				LX_CALC_ALIGNED_VALUE(32,4)
#define OSD_PAL_SIZE				(sizeof(UINT32)*256)

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
LX_FBDEV_DEV_MEM_CFG_T gMemCfgFBDev[4] = {
{
	.mem_palette_name	= "fb0_palette", 
	.mem_palette_base	= 0x0,
	.mem_palette_size	= 0x0,
	.mem_frame_name		= "fb0_frame", .mem_frame_base		= 0x0, .mem_frame_size		= 0x0,
},
{
	.mem_palette_name	= "fb1_palette", .mem_palette_base	= 0x0, .mem_palette_size	= 0x0,
	.mem_frame_name		= "fb1_frame", .mem_frame_base		= 0x0, .mem_frame_size		= 0x0,
},
{
	.mem_palette_name	= "fb2_palette", .mem_palette_base	= 0x0, .mem_palette_size	= 0x0,
	.mem_frame_name		= "fb2_frame", .mem_frame_base		= 0x0, .mem_frame_size		= 0x0,
},
{
	.mem_palette_name	= "fb3_palette", .mem_palette_base	= 0x0, .mem_palette_size	= 0x0,
	.mem_frame_name		= "fb3_frame", .mem_frame_base		= 0x0, .mem_frame_size		= 0x0,
}
};

LX_FBDEV_HDR_MEM_CFG_T gMemCfgFBHdr[4] = {
{
	.mem_header_name	= "fb0_header", .mem_header_base	= 0x0, .mem_header_size	= 0x0,
},
{
	.mem_header_name	= "fb1_header", .mem_header_base	= 0x0, .mem_header_size	= 0x0,
},
{
	.mem_header_name	= "fb2_header", .mem_header_base	= 0x0, .mem_header_size	= 0x0,
},
{
	.mem_header_name	= "fb3_header", .mem_header_base	= 0x0, .mem_header_size	= 0x0,
}
};


/** chip register configuration
 *
 */
LX_FBDEV_REG_CFG_T    g_fbdev_reg_cfg[] =
{
    // for L8 A0 & B0
    {
        .chip_name          = "FBDEV-L8",
        .reg_base_addr      = 0xC0017A00,
        .reg_size           = 0x100,
        .irq_num            = L8_IRQ_SLT,

		.slkrd_prefix_mask	= 0xA0000000,	/* L8 OSD is running on silkroad #1 */
		.slkrd_mmap_mask	= 0x20000000,	/* L8 OSD is running on silkroad #1 */
		.frame_base_addr	= 0x26498000,
		.frame_base_size	= 0x0,			/* don't care of size */

		.dev_idx_list		= { 0, 1, 2, 3 },
    },
    // for L9 A0
    {
        .chip_name          = "FBDEV-L9",
        .reg_base_addr      = 0xC0023A00,	/* A0 */
        .reg_size           = 0x300,		/* real OSD reg range + alpha */
        .irq_num            = L9_IRQ_OSD_BCPU,

		.slkrd_prefix_mask	= 0x00000000,	/* L9 OSD doesn't need to care of the bus */
		.slkrd_mmap_mask	= 0x00000000,	/* L9 OSD doesn't need to care of the bus */
#ifdef FBDEV_USE_DDR1_MEM
		.frame_base_addr	= 0x7e000000,	/* 0x7d000000 = DDR1 0x8000_0000-50MB */
#else
		.frame_base_addr	= 0x2e800000,	/* 0x30000000 - 24MB (DDR0) */
#endif
		.frame_base_size	= 0x02000000,	/* 32MB */

		.dev_idx_list		= { 0, 1, 2, 3 },
    },
    // for L9 B0
    {
        .chip_name          = "FBDEV-L9",
        .reg_base_addr      = 0xC0025100,	/* B0 */
        .reg_size           = 0x300,		/* real OSD reg range + alpha */
        .irq_num            = L9_IRQ_OSD_BCPU,

		.slkrd_prefix_mask	= 0x00000000,	/* L9 OSD doesn't need to care of the bus */
		.slkrd_mmap_mask	= 0x00000000,	/* L9 OSD doesn't need to care of the bus */
#ifdef FBDEV_USE_DDR1_MEM
		.frame_base_addr	= 0x7e000000,	/* 0x7d000000 = DDR1 0x8000_0000-50MB */
#else
		.frame_base_addr	= 0x2d000000,	/* 0x2d000000 (40MB) */	/* 0x30000000 - 32MB (DDR0) */
#endif
		.frame_base_size	= 0x03000000,	/* 48MB */

		.dev_idx_list		= { 0, 1, 2, 3 },	/* swap dev index 0 and 1 */
    },
};

LX_FBDEV_REG_CFG_T*   gpFbdevRegCfg = NULL;
LX_FBDEV_CFG_T*			g_fbdev_cfg = NULL;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

static LX_FBDEV_CFG_T	g_fbdev_cfg_gp /* L9 GP4 */ =
{
	.b_use_irq				= 0x1,
	.b_gpu_clock_enable		= 0x1,		// GP platform donesn't use GPU !!
	.cursor_layer			= 0x3,
	.display_width			= 1920,		// 1920x1080 panel size
	.display_height			= 1080,		// this value is set as the read only and used througout the whole FB codes.

	.fb[0] = {
		.b_active			= 0x1,
		.b_little_endian	= 0x1,		// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,
		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 2,
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
		.max_xres			= 1280,
		.max_yres			= 720,

		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},

	.fb[1] = {
		.b_active			= 0x1,
		.b_little_endian	= 0x1,	// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,
		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 2,
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
		.max_xres			= 1280,
		.max_yres			= 720,

		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},


	/* raxis.lim (2011/11/19) -- L9 doesn't support POSD2 since it is used as alpha blender metal revison */
	.fb[2] = {
		.b_active			= 0x0,
		.b_little_endian	= 0x1,	// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,	// UI screen sometimes needs animation synced with vsync isr.

		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 2,
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
		.max_xres			= 128,
		.max_yres			= 2,

		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},

	.fb[3] = {
		.b_active			= 0x1,
		.b_little_endian	= 0x1,	// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,

		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 2,
#ifdef FBDEV_GP_USE_16BIT_CURSOR
		.bits_per_pixel		= 16,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_4444,
#else /* use 32bit color by default */
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
#endif
		.max_xres			= 128,
		.max_yres			= 128,

		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},
};

static LX_FBDEV_CFG_T	g_fbdev_cfg_cosmo =
{
	.b_use_irq				= 0x1,		// cosmo platform uses the interrupt service
	.b_gpu_clock_enable		= 0x1,		// cosmo platform uses GPU !!
	.cursor_layer			= 0x3,
	.display_width			= 1920,		// 1920x1080 panel size
	.display_height			= 1080,		// this value is set as the read only and used througout the whole FB codes.

	.fb[0] = {
		.b_active			= 0x1,
		.b_little_endian	= 0x1,	// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,		// cosmo platform uses vsync
		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 4,
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
		.max_xres			= 1920,	// cosmo platform uses 1920x1080
		.max_yres			= 1080,	// cosmo platform uses 1920x1080
		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},

	.fb[1] = {
		.b_active			= 0x1,
		.b_little_endian	= 0x1,	// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,
		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 2,
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
		.max_xres			= 1280,
		.max_yres			= 720,
		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},

	.fb[2] = {
		.b_active			= 0x0,
		.b_little_endian	= 0x1,	// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,

		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 2,
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
		.max_xres			= 128,
		.max_yres			= 2,

		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},

	.fb[3] = {
		.b_active			= 0x1,
		.b_little_endian	= 0x1,	// OSD endian is fixed to the littlen. DO NOT try big endian. it's never tested.
		.b_show_on_startup	= 0x0,
		.b_use_vsync		= 0x1,

		.b_use_global_alpha	= 0x0,
		.max_buf_num		= 2,
		.bits_per_pixel		= 32,
		.pixel_format		= LX_FBDEV_PIXEL_FORMAT_RGB_8888,
		.max_xres			= 64,
		.max_yres			= 64,
		.b_gfx_lock_req		= 0x0,
		.b_use_trid_hw		= 0x0,
	},
};

static struct fb_var_screeninfo default_fb_var_screeninfo[LX_FBDEV_ID_MAX] = {

/* OSD0 default value */
{
	/* xres, yres, xres_virtual, yres_virtual is initialized at FBDEV_InitCfg */
	/* bits_per_pixel, red, green, blue, transp is initialized at FBDEV_InitCfg */
	/* hsync_len ( pixel format ), vsync_len ( pixel depth ) is initialized at FBDEV_InitCfg */

	.xoffset 		= 0,
	.yoffset 		= 0,
	.grayscale		= 0,

	.nonstd			= 0,
	.activate		= FB_ACTIVATE_FORCE,
	.height			= -1,
	.width			= -1,
	.accel_flags	= 0,
	.pixclock		= LCD_PANEL_CLOCK,	/* picoseconds */
	.left_margin	= 40,				/* pixclocks */
	.right_margin	= 4,				/* pixclocks */
	.upper_margin	= 8,				/* line clocks */
	.lower_margin	= 2,				/* line clocks */

	.sync 			= 0,
	.vmode 			= FB_VMODE_NONINTERLACED,
},

/* OSD1 default value */
{
	/* xres, yres, xres_virtual, yres_virtual is initialized at FBDEV_InitCfg */
	/* bits_per_pixel, red, green, blue, transp is initialized at FBDEV_InitCfg */
	/* hsync_len ( pixel format ), vsync_len ( pixel depth ) is initialized at FBDEV_InitCfg */

	.xoffset = 0,
	.yoffset = 0,
	.grayscale = 0,

	.nonstd = 0,
	.activate = FB_ACTIVATE_FORCE,
	.height = -1,
	.width = -1,
	.accel_flags = 0,
	.pixclock = LCD_PANEL_CLOCK, /* picoseconds */
	.left_margin = 40,			 /* pixclocks */
	.right_margin = 4,			 /* pixclocks */
	.upper_margin = 8,			 /* line clocks */
	.lower_margin = 2,			 /* line clocks */

	.sync = 0,
	.vmode = FB_VMODE_NONINTERLACED,
},

/* OSD2 default value */
{
	/* xres, yres, xres_virtual, yres_virtual is initialized at FBDEV_InitCfg */
	/* bits_per_pixel, red, green, blue, transp is initialized at FBDEV_InitCfg */
	/* hsync_len ( pixel format ), vsync_len ( pixel depth ) is initialized at FBDEV_InitCfg */

	.xoffset = 0,
	.yoffset = 0,
	.grayscale = 0,

	.nonstd = 0,
	.activate = FB_ACTIVATE_FORCE,
	.height = -1,
	.width = -1,
	.accel_flags = 0,
	.pixclock = LCD_PANEL_CLOCK, /* picoseconds */
	.left_margin = 40,			 /* pixclocks */
	.right_margin = 4,			 /* pixclocks */
	.upper_margin = 8,			 /* line clocks */
	.lower_margin = 2,			 /* line clocks */

	.sync = 0,
	.vmode = FB_VMODE_NONINTERLACED,
},

/* OSD3 default value */
{
	/* xres, yres, xres_virtual, yres_virtual is initialized at FBDEV_InitCfg */
	/* bits_per_pixel, red, green, blue, transp is initialized at FBDEV_InitCfg */
	/* hsync_len ( pixel format ), vsync_len ( pixel depth ) is initialized at FBDEV_InitCfg */

	.xoffset = 0,
	.yoffset = 0,
	.grayscale = 0,

	.nonstd = 0,
	.activate = FB_ACTIVATE_FORCE,
	.height = -1,
	.width = -1,
	.accel_flags = 0,
	.pixclock = LCD_PANEL_CLOCK, /* picoseconds */
	.left_margin = 40,			 /* pixclocks */
	.right_margin = 4,			 /* pixclocks */
	.upper_margin = 8,			 /* line clocks */
	.lower_margin = 2,			 /* line clocks */

	.sync = 0,
	.vmode = FB_VMODE_NONINTERLACED,
},
};

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/
/** get preinitialized fb_var_screeninfo object
 *
 *
 */
struct fb_var_screeninfo*   FBDEV_GetVarScreenObject ( int idx )
{
	FBDEV_CHECK_CODE( idx >= LX_FBDEV_ID_MAX, return NULL, "out of range (%d)\n", idx );

	return &default_fb_var_screeninfo[idx];
}

/** initialize FBDEV configuration
 *
 *
 */
void	FBDEV_InitCfg	( void )
{
	int		i;

	/* select fbdev configuration based on the platform type */
	switch ( lx_chip_plt() )
	{
		case LX_CHIP_PLATFORM_COSMO:
		{
			g_fbdev_cfg = &g_fbdev_cfg_cosmo;
		}
		break;

		case LX_CHIP_PLATFORM_FPGA:
		case LX_CHIP_PLATFORM_GP:
		default:
		{
			g_fbdev_cfg = &g_fbdev_cfg_gp;

			if ( lx_chip_rev() < LX_CHIP_REV(L9,A0) )
			{
				g_fbdev_cfg->fb[0].max_buf_num = 1;
			}

#ifdef FBDEV_USE_POSD1_1920x1080
			/* raxis.lim (2011/11/22)
			 * L9B1 can support 1920x1080.
			 * strictly speaking, L9 can always support 1920x1080 ( OSD post processor such as FRC3 cannot handle properly :-( )
			 */
			if ( lx_chip_rev() >= LX_CHIP_REV(L9,B1) )
			{
				/* raxis.lim (2011/11/19) -- we should support full HD UI from L9B1 */
				g_fbdev_cfg->fb[1].max_xres	= 1920;
				g_fbdev_cfg->fb[1].max_yres	= 1080;

				/* raxis.lim (2011/11/22) -- we should support downscaler for OSD to avoid FRC line buffer overflow */
				//g_fbdev_cfg->display_width= 1280;
			}
#endif

			/* raxis.lim (2011/12/15)
			 * check if cursor should use GFX H/W to do some OSD workaround.
			 * cursor is very time critical. also cursor movement with GFX workaround may produce GFX surface corruption.
			 * so we should check if current cursor implmentation needs GFX or not.
		  	 * since default value is FALSE, we should check the case which makes value TRUE
			 * - L9 A0 ~ B0 should use GFX H/W since FRC separated mode have alpha output problem.
			 * - L9B1,L9B2 can implement cursor without any usage of GFX H/W.
			 */
			if ( lx_chip_rev() >= LX_CHIP_REV(L9,B1) ) 		/* L9B1 and later */
			{
#ifndef FBDEV_USE_POSD1_1920x1080
				g_fbdev_cfg->fb[3].b_gfx_lock_req = TRUE;
#endif
			}
			else if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) ) /* L9A0 and later */
			{
				g_fbdev_cfg->fb[3].b_gfx_lock_req = TRUE;
			}

			/* raxis.lim (2011/12/22)
			 * L9 support 3DOSD specific H/W block. but TV platform dones't use it. so DO NOT enable b_use_trid_hw flag.
			 *
			 */
		}
		break;
	}

	/* select register configuration */
	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )
    {
        switch ( lx_chip_plt() )
        {
            case LX_CHIP_PLATFORM_COSMO:
            {
#ifdef USE_ADDRESS_SWITCH_CASE1
		g_fbdev_reg_cfg[2].frame_base_addr = 0x35200000;
		g_fbdev_reg_cfg[2].frame_base_size = 0x02D00000;
#endif
            }
            break;

            case LX_CHIP_PLATFORM_FPGA:
            case LX_CHIP_PLATFORM_GP:
            default:
			{
				/* do nothing */
			}
			break;
        }

		/* separete A0, B0 config */
        gpFbdevRegCfg = (lx_chip_rev() >= LX_CHIP_REV(L9,B0))? &g_fbdev_reg_cfg[2] : &g_fbdev_reg_cfg[1];

		printk("^g^[FB] REG BASE = 0x%x\n", gpFbdevRegCfg->reg_base_addr );

    }
	else /* L8 */
	{
		gpFbdevRegCfg = &g_fbdev_reg_cfg[0];
		/* [TODO] */
	}

	/* raxis.lim (2011/11/08) -- set FB memory info. this information is passed to the user application */
	g_fbdev_cfg->fb_mem_base = gpFbdevRegCfg->frame_base_addr;
	g_fbdev_cfg->fb_mem_size = gpFbdevRegCfg->frame_base_size;

#define	OSD_BUF_NUM(id)			g_fbdev_cfg->fb[(id)].max_buf_num
#define OSD0_XRES               g_fbdev_cfg->fb[0].max_xres
#define OSD0_YRES               g_fbdev_cfg->fb[0].max_yres
#define OSD0_BYTES_PER_PIXEL	( g_fbdev_cfg->fb[0].bits_per_pixel >> 3 )
#define OSD1_XRES               g_fbdev_cfg->fb[1].max_xres
#define OSD1_YRES               g_fbdev_cfg->fb[1].max_yres
#define OSD1_BYTES_PER_PIXEL	( g_fbdev_cfg->fb[1].bits_per_pixel >> 3 )
#define OSD2_XRES               g_fbdev_cfg->fb[2].max_xres
#define OSD2_YRES               g_fbdev_cfg->fb[2].max_yres
#define OSD2_BYTES_PER_PIXEL	( g_fbdev_cfg->fb[2].bits_per_pixel >> 3 )
#define OSD3_XRES               g_fbdev_cfg->fb[3].max_xres
#define OSD3_YRES               g_fbdev_cfg->fb[3].max_yres
#define OSD3_BYTES_PER_PIXEL	( g_fbdev_cfg->fb[3].bits_per_pixel >> 3 )

#define OSD0_MAX_SIZE			g_fbdev_cfg->fb[0].max_size
#define OSD1_MAX_SIZE			g_fbdev_cfg->fb[1].max_size
#define OSD2_MAX_SIZE			g_fbdev_cfg->fb[2].max_size
#define OSD3_MAX_SIZE			g_fbdev_cfg->fb[3].max_size

	/* calculate maximum buffer size of each fb device. each buffer size is bitmap word (8byte) */
	OSD0_MAX_SIZE = PAGE_ALIGN( OSD0_XRES * OSD0_YRES * OSD0_BYTES_PER_PIXEL * OSD_BUF_NUM(0) );
	OSD1_MAX_SIZE = PAGE_ALIGN( OSD1_XRES * OSD1_YRES * OSD1_BYTES_PER_PIXEL * OSD_BUF_NUM(1) );
	OSD2_MAX_SIZE = PAGE_ALIGN( OSD2_XRES * OSD2_YRES * OSD2_BYTES_PER_PIXEL * OSD_BUF_NUM(2) );
	OSD3_MAX_SIZE = PAGE_ALIGN( OSD3_XRES * OSD3_YRES * OSD3_BYTES_PER_PIXEL * OSD_BUF_NUM(3) );

	/* construct default_fb_var_screeninfo array */
	for ( i=0; i< LX_FBDEV_ID_MAX; i++ )
	{
		struct fb_var_screeninfo* fb_info	= &default_fb_var_screeninfo[i];

		fb_info->xres			= g_fbdev_cfg->fb[i].max_xres;
		fb_info->yres			= g_fbdev_cfg->fb[i].max_yres;
		fb_info->xres_virtual	= g_fbdev_cfg->fb[i].max_xres;
		fb_info->yres_virtual	= g_fbdev_cfg->fb[i].max_yres * OSD_BUF_NUM(i);

		fb_info->bits_per_pixel	= g_fbdev_cfg->fb[i].bits_per_pixel; //pixel_depth_2_bpp[ g_fbdev_cfg->fb[i].pixel_depth ];

		/* raxis.lim (2010/11/21) -- I dont't refer to pixel depth because bitfield definition is independent of pixel depth */
//		if ( g_fbdev_cfg->fb[i].pixel_format == LX_FBDEV_PIXEL_FORMAT_ARGB8888 )
		switch( g_fbdev_cfg->fb[i].bits_per_pixel )
		{
			case 8: case 32:
			{
				struct fb_bitfield a = { 24, 8, 0 };
				struct fb_bitfield r = { 16, 8, 0 };
				struct fb_bitfield g = {  8, 8, 0 };
				struct fb_bitfield b = {  0, 8, 0 };

				fb_info->transp = a, fb_info->red = r, fb_info->green = g, fb_info->blue = b;
			}
			break;
			case 16:
			{
				if ( g_fbdev_cfg->fb[i].pixel_format == LX_FBDEV_PIXEL_FORMAT_RGB_565 )
				{
					struct fb_bitfield a = {  0, 0, 0 };
					struct fb_bitfield r = { 11, 5, 0 };
					struct fb_bitfield g = {  5, 6, 0 };
					struct fb_bitfield b = {  0, 5, 0 };
					fb_info->transp = a, fb_info->red = r, fb_info->green = g, fb_info->blue = b;
				}
				else if ( g_fbdev_cfg->fb[i].pixel_format == LX_FBDEV_PIXEL_FORMAT_RGB_4444 )
				{
					struct fb_bitfield a = { 12, 4, 0 };
					struct fb_bitfield r = {  8, 4, 0 };
					struct fb_bitfield g = {  4, 4, 0 };
					struct fb_bitfield b = {  0, 4, 0 };
					fb_info->transp = a, fb_info->red = r, fb_info->green = g, fb_info->blue = b;
				}
				else
				{
					DBG_PRINT_ERROR("sorry. not ready pxlfmt %d.\n", g_fbdev_cfg->fb[i].pixel_format );
				}
			}
			break;

			default:
			{
				DBG_PRINT_ERROR("sorry. not supported or not tested bpp %d.\n", g_fbdev_cfg->fb[i].bits_per_pixel );
			}
		}
	}

	/* initialize osd header & osd palette. OSD baseaddress is fixed */
	gMemCfgFBDev[0].mem_palette_base= OSD_BASE_ADDR;
	gMemCfgFBDev[0].mem_palette_size= OSD_PAL_SIZE;
	gMemCfgFBDev[0].mem_frame_base	= 0x00000000;
	gMemCfgFBDev[0].mem_frame_size	= OSD0_MAX_SIZE;

	gMemCfgFBDev[1].mem_palette_base= 0x00000000;
	gMemCfgFBDev[1].mem_palette_size= OSD_PAL_SIZE;
	gMemCfgFBDev[1].mem_frame_base	= 0x00000000;
	gMemCfgFBDev[1].mem_frame_size	= OSD1_MAX_SIZE;

	gMemCfgFBDev[2].mem_palette_base= 0x00000000;
	gMemCfgFBDev[2].mem_palette_size= OSD_PAL_SIZE;
	gMemCfgFBDev[2].mem_frame_base	= 0x00000000;
	gMemCfgFBDev[2].mem_frame_size	= OSD2_MAX_SIZE;

	gMemCfgFBDev[3].mem_palette_base= 0x00000000;
	gMemCfgFBDev[3].mem_palette_size= OSD_PAL_SIZE;
	gMemCfgFBDev[3].mem_frame_base	= 0x00000000;
	gMemCfgFBDev[3].mem_frame_size	= OSD3_MAX_SIZE;

	gMemCfgFBHdr[0].mem_header_base	= 0x0000000;
	gMemCfgFBHdr[0].mem_header_size	= OSD_HDR_SIZE;
	gMemCfgFBHdr[1].mem_header_base	= 0x00000000;
	gMemCfgFBHdr[1].mem_header_size	= OSD_HDR_SIZE;
	gMemCfgFBHdr[2].mem_header_base	= 0x00000000;
	gMemCfgFBHdr[2].mem_header_size	= OSD_HDR_SIZE;
	gMemCfgFBHdr[3].mem_header_base	= 0x00000000;
	gMemCfgFBHdr[3].mem_header_size	= OSD_HDR_SIZE;

	/* raxis.lim (2011/09/16)
	 * POSD3(cursor) will bs DDR2 to avoid the burst band width problem.
	 * DE allowed for POSD3 to use the 1MB in DDR2
	 */
	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0 ) )
	{
		gMemCfgFBDev[3].mem_palette_base	= 0x88000000 - (1<<20);	/* static alloc */
		gMemCfgFBDev[3].mem_frame_base		= 0x00000000;			/* auto alloc */
		gMemCfgFBHdr[3].mem_header_base		= 0x00000000;			/* auto alloc */
	}
}

void    FBDEV_GetMemCfg ( UINT32* mem_base, UINT32* mem_size )
{
	*mem_base = gpFbdevRegCfg->frame_base_addr;
	*mem_size = gpFbdevRegCfg->frame_base_size;
}

/** return real HW device index from layerId
 *
 *	@param layer [IN] layerId
 *	@return remmaped index which indicates the real HW OSD index
 *
 *	@note this function is valid from L9B0
 */
int     FBDEV_GetRemapDevID         (int layer)
{
	FBDEV_CHECK_CODE( layer >= LX_FBDEV_ID_MAX, return 0, "out of range (%d)\n", layer );
	return gpFbdevRegCfg->dev_idx_list[layer];
}

/** @} */

