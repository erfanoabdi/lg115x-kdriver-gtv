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

#ifndef	_FBDEV_CFG_H_
#define	_FBDEV_CFG_H_

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

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	FBDEV_MODULE				"fbdev"
#define FBDEV_MAX_DEVICE			4

#undef	FBDEV_USE_DEFAULT_PALETTE 	/* if some default palette needed, define FBDEV_USE_DEFAULT_PALETTE */

#define	FBDEV_USE_AUTO_VSYNC		/* when enabled, vsync interval is calculated automatically */
#undef	FBDEV_DEBUG_VSYNC			/* when enabled, vsync timoeut is printed */
#undef	FBDEV_FLIP_LOCK				/* when enabled, flipping lock is enabled. low performance for multiple POSD update ? */
#define	FBDEV_USE_MIXER_INTR		/* when enabled, only MIXER intr is enabled, all other OSD intr is disabled */
#undef	FBDEV_USE_DDR1_MEM			/* when enabled, FB memory is allocated in DDR1, otherwise FB memory is in DDR0 */
#undef	FBDEV_GP_USE_16BIT_CURSOR	/* when enabled, GP mode will use 16bit cursor */

#define	FBDEV_USE_MMIO_REG			/* when enabled, FB device uses memory based IO. when disabled, FB device uses register based IO
									 * DO NOT modify this macro. register based IO is not working now 
									 */

#define	FBDEV_USE_POSD1_1920x1080	/* support POSD1 to be 1920x1080. DO NOT enable this macro until FBDEV is ready to support 1920x1080 OSD.
										currently, L9B1 with FRC A1 may support 1920x1080 */

#undef	FBDEV_USE_BLEND_MEMC_SW_FIX	/* when enabled, incorrect MEMC output is fixed by software. this workaround is chip specific */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct
{
	char*			mem_palette_name;
	unsigned int	mem_palette_base;
	unsigned int	mem_palette_size;		/* osd palette size */

	char*			mem_frame_name;
	unsigned int	mem_frame_base;
	unsigned int	mem_frame_size;			/* osd frame buffer size */
} 
LX_FBDEV_DEV_MEM_CFG_T;

typedef struct
{
	char*			mem_header_name;
	unsigned int	mem_header_base;
	unsigned int	mem_header_size;		/* osd header size */
}
LX_FBDEV_HDR_MEM_CFG_T;

extern LX_FBDEV_DEV_MEM_CFG_T 		gMemCfgFBDev[4];	/* memcfg for FB device memory */
extern LX_FBDEV_HDR_MEM_CFG_T		gMemCfgFBHdr[4];	/* memcfg for FB header memory */

/* FBDEV regiseter configuration 
 *
 *
 */
typedef struct
{
    char*       chip_name;
    UINT32      reg_base_addr;
    UINT32      reg_size;
    UINT32      irq_num;

	UINT32		slkrd_prefix_mask;
	UINT32		slkrd_mmap_mask;

	UINT32		frame_base_addr;		/* temp */
	UINT32		frame_base_size;		/* temp */

	UINT8		dev_idx_list[4];
}
LX_FBDEV_REG_CFG_T;


extern LX_FBDEV_REG_CFG_T*    gpFbdevRegCfg;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _FBDEV_CFG_H_ */

/** @} */

