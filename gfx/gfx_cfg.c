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
 *  main configuration file for gfx device
 *	gfx device will teach you how to make device driver with new platform.
 *
 *  author		raxis.lim (raxis.lim@lge.com).lim (raxis.lim@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_gfx
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "gfx_impl.h"
#include "base_dev_cfg.h"
#include "fbdev_drv.h"
#include "misc_util.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	GFX_SIZE_FOR_768MB

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

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
LX_GFX_MEM_CFG_T gMemCfgGfx =
{
	.surface = { .name = "gfx_surface", .base = 0x0, .size = 0x08000000 /* 128MB by default */ },
};

LX_GFX_REG_CFG_T	g_gfx_reg_cfg[] =
{
    // for L8 A0 & B0
    {
        .chip_name          = "GFX-L8",
        .reg_base_addr      = 0xc0027000,
        .reg_size           = 0x1E0,
        .irq_num            = L8_IRQ_GFX,
    },
    // for L9 A0
    {
        .chip_name          = "GFX-L9",
        .reg_base_addr      = 0xc001d000, 	// L9_GFX_BASE,
        .reg_size           = 0x200, 		// real size is '0x1EC'
        .irq_num            = L9_IRQ_GFX,
    },
};

LX_GFX_REG_CFG_T*	gpGfxRegCfg = NULL;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/
void    GFX_InitCfg ( void )
{
	/* get chip configuration */
	memcpy( &g_gfx_cfg, g_gfx_hal.GetCfg(), sizeof(LX_GFX_CFG_T));
	/*-----------------------------------------------------------------------------------
	 * [L9] configuration
	 *----------------------------------------------------------------------------------*/
    if( lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
    {
		gpGfxRegCfg = &g_gfx_reg_cfg[1];

        switch ( lx_chip_plt() )
        {
			/* COSMO uses different memory location & size. */
            case LX_CHIP_PLATFORM_COSMO:
            {
				gMemCfgGfx.surface.base= 0x33A00000;
				gMemCfgGfx.surface.size= 0x00100000;
            }
            break;

            case LX_CHIP_PLATFORM_FPGA:
            case LX_CHIP_PLATFORM_GP:
            default:
			{
#if 0
				gMemCfgGfx.surface.size= 104<<20; 		/* 154MB */
#else
				gMemCfgGfx.surface.size= 172<<20; 		/* 154MB */
#endif
				gMemCfgGfx.surface.base= 0x0;			/* automatic allocation */
			}
			break;
        }
    }
	/*-----------------------------------------------------------------------------------
	 * [L8] configuration
	 *----------------------------------------------------------------------------------*/
    else
    {
		gpGfxRegCfg = &g_gfx_reg_cfg[0];
		/* [TODO] */
    }
}

/** @} */

