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
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     jaemo.kim (jaemo.kim@lge.com)
 * version    1.0
 * date       2010.02.23
 * note       Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */

#ifndef  DE_REG_DEF_INC
#define  DE_REG_DEF_INC

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#ifndef DTVSOC_DE_BASE
#ifndef DTVSOC_VIRT_PL301_BASE
#ifdef USE_QEMU_SYSTEM
#define DTVSOC_VIRT_PL301_BASE		 0x40000000
#else
#define DTVSOC_VIRT_PL301_BASE		 0xF6000000
#endif
#endif
#define DTVSOC_VDEC_BASE		     (DTVSOC_VIRT_PL301_BASE+0x03000)
#define DTVSOC_VENC_BASE		     (DTVSOC_VIRT_PL301_BASE+0x08000)
#define DTVSOC_DE_BASE				 (DTVSOC_VIRT_PL301_BASE+0x16000)
#define DTVSOC_TCON_BASE		     (DTVSOC_VIRT_PL301_BASE+0x1a000)
#define DTVSOC_OIF_LED_BASE		     (DTVSOC_VIRT_PL301_BASE+0x1c000)
#endif
#define DTVSOC_LED_BASE		     	 (DTVSOC_OIF_LED_BASE)
#define DTVSOC_OIF_BASE		     	 (DTVSOC_OIF_LED_BASE + 0x800)

#ifndef DTVSOC_VIRT_COREAPB_BASE
#define DTVSOC_VIRT_COREAPB_BASE	 0xfd000000
#endif
#ifndef DTVSOC_TOP_CTRL
#define DTVSOC_TOP_CTRL				 (DTVSOC_VIRT_COREAPB_BASE+0x300000)
#endif

#define DTV_SoC_VIRTUAL_BASE         DTVSOC_VIRT_PL301_BASE
#define DTV_SoC_VIRTUAL_SIZE         0x30000

#define VIDEO_DE0A_HIF_REG_OFFSET		   0x0000
#define VIDEO_DE0B_HIF_REG_OFFSET		   0x0800
#define VIDEO_DE1A_HIF_REG_OFFSET		   0x1000
#define VIDEO_DE1B_HIF_REG_OFFSET		   0x1800

#define VIDEO_CVA_TOP_REG_OFFSET		   0x0100
#define VIDEO_CVB_TOP_REG_OFFSET		   0x0180
#define VIDEO_DLR_REG_OFFSET			   0x0200

#define VIDEO_MSCR_REG_OFFSET			   0x0500
#define VIDEO_SSCR_REG_OFFSET			   0x0900

#define VIDEO_MIF0_REG_OFFSET			   0x0600
#define VIDEO_MIF1_REG_OFFSET			   0x0a00
#define VIDEO_MIF2_REG_OFFSET			   0x1500
#define VIDEO_MIF3_REG_OFFSET			   0x1b00

#define VIDEO_OV_REG_OFFSET				   0x1100

#define VIDEO_PE0_REG_OFFSET               0x0300
#define VIDEO_PE1_REG_OFFSET			   0x1200
#define VIDEO_IPC_REG_OFFSET			   0x1F80
#define VIDEO_DMA_REG_OFFSET			   0x1FE0

#define VIDEO_ATOP_REG_OFFSET			   0x1900
#define VIDEO_OSD_REG_OFFSET			   0x1A00

#define VIDEO_DE0A_HIF_REG_BASE			   ( DTVSOC_DE_BASE + VIDEO_DE0A_HIF_REG_OFFSET )
#define VIDEO_DE0B_HIF_REG_BASE			   ( DTVSOC_DE_BASE + VIDEO_DE0B_HIF_REG_OFFSET )
#define VIDEO_DE1A_HIF_REG_BASE			   ( DTVSOC_DE_BASE + VIDEO_DE1A_HIF_REG_OFFSET )
#define VIDEO_DE1B_HIF_REG_BASE			   ( DTVSOC_DE_BASE + VIDEO_DE1B_HIF_REG_OFFSET )

#define VIDEO_CVA_TOP_REG_BASE			   ( DTVSOC_DE_BASE + VIDEO_CVA_TOP_REG_OFFSET  )
#define VIDEO_CVB_TOP_REG_BASE			   ( DTVSOC_DE_BASE + VIDEO_CVB_TOP_REG_OFFSET  )
#define VIDEO_DLR_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_DLR_REG_OFFSET	   )

#define VIDEO_MSCR_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_MSCR_REG_OFFSET	   )
#define VIDEO_SSCR_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_SSCR_REG_OFFSET	   )

#define VIDEO_MIF0_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_MIF0_REG_OFFSET	   )
#define VIDEO_MIF1_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_MIF1_REG_OFFSET	   )
#define VIDEO_MIF2_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_MIF2_REG_OFFSET	   )
#define VIDEO_MIF3_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_MIF3_REG_OFFSET	   )

#define VIDEO_OV_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_OV_REG_OFFSET	   )
#define VIDEO_ATOP_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_ATOP_REG_OFFSET	   )

#define VIDEO_PE0_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_PE0_REG_OFFSET	   )
#define VIDEO_PE1_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_PE1_REG_OFFSET	   )
#define VIDEO_IPC_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_IPC_REG_OFFSET	   )
#define VIDEO_DMA_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_DMA_REG_OFFSET	   )

#define VIDEO_OSD_REG_BASE				   ( DTVSOC_DE_BASE + VIDEO_OSD_REG_OFFSET	   )

#define VIDEO_VDI_REG_BASE                 DTVSOC_VDEC_BASE
#define VIDEO_DVR_REG_BASE				   DTVSOC_VENC_BASE
#define VIDEO_GFX_REG_BASE				   DTVSOC_GFX_BASE

/*----------------------------------------------------------------------------------------
 *	 Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif
/**  @} */
