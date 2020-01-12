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
 *	board specific configuration
 *
 *  author      daeyoung lim ( raxis.lim@lge.com )
 *  version		1.0
 *  date		2009.11.14
 *
 *  @addtogroup lg1150_base
 * @{
 */

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
#include "debug_util.h"
#ifdef INCLUDE_KDRV_SAMPLE
#include "sample_drv.h"
#endif

#ifdef INCLUDE_KDRV_SYS
#include "sys_drv.h"
#endif

#ifdef INCLUDE_KDRV_DEMOD
#include "demod_drv.h"
#endif

#ifdef INCLUDE_KDRV_ADEC
#include "adec_drv.h"
#endif

#ifdef INCLUDE_KDRV_AAD
#include "aad_drv.h"
#endif

#ifdef INCLUDE_KDRV_AFE
#include "afe_drv.h"
#endif

#ifdef INCLUDE_KDRV_DE
#include "de_drv.h"
#endif
#ifdef INCLUDE_KDRV_HDMI
#include "hdmi_drv.h"
#endif
#ifdef INCLUDE_KDRV_FMT3D
#include "fmt3d_drv.h"
#endif
#ifdef INCLUDE_KDRV_DENC
#include "denc_drv.h"
#endif
#ifdef INCLUDE_KDRV_DIMM
#include "dimm_drv.h"
#endif
#ifdef INCLUDE_KDRV_PE
#include "pe_drv.h"
#endif

#ifdef INCLUDE_KDRV_GPIO
#include "gpio_drv.h"
#endif
#ifdef INCLUDE_KDRV_FRC
#include "frc_drv.h"
#endif

#ifdef INCLUDE_KDRV_I2C
#include "i2c_drv.h"
#endif /* INCLUDE_KDRV_I2C */

#ifdef INCLUDE_KDRV_SDEC
#include "sdec_drv.h"
#endif /* INCLUDE_KDRV_SDEC */

#ifdef INCLUDE_KDRV_PVR
#include "pvr_drv.h"
#endif /* INCLUDE_KDRV_PVR */

#ifdef INCLUDE_KDRV_VDEC
#include "vdec_drv.h"
#endif /* INCLUDE_KDRV_VDEC */

#ifdef INCLUDE_KDRV_CI
#include "ci_drv.h"
#endif /* INCLUDE_KDRV_CI */

#ifdef INCLUDE_KDRV_TTX
#include "ttx_drv.h"
#endif /* INCLUDE_KDRV_TTX */

#ifdef INCLUDE_KDRV_VENC
#include "venc_drv.h"
#endif /* INCLUDE_KDRV_VENC */

#ifdef INCLUDE_KDRV_PNG
#include "png_drv.h"
#endif /* INCLUDE_KDRV_PNG */

#ifdef INCLUDE_KDRV_MJPEG
#include "mjpeg_drv.h"
#endif /* INCLUDE_KDRV_MJPEG */

#ifdef INCLUDE_KDRV_GFX
#include "gfx_drv.h"
#endif
#ifdef INCLUDE_KDRV_FBDEV
#include "fbdev_drv.h"
#endif

#ifdef INCLUDE_KDRV_SE
#include "se_drv.h"
#endif

#ifdef INCLUDE_KDRV_SCI
#include "sci_drv.h"
#endif

#ifdef INCLUDE_KDRV_CLK
#include "clk_drv.h"
#endif

#ifdef INCLUDE_KDRV_MONITOR
#include "monitor_drv.h"
#endif

#include "os_util.h"
#include "base_dev_cfg.h"

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
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern char *simple_strtok(char *s, const char *delim, char **save_ptr);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
LX_BASE_DEV_CFG_T	g_base_dev_cfg;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static void	BASE_DEV_ProbeChipRevision(void);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/
void		BASE_DEV_InitCfg( void )
{
	BASE_DEV_ProbeChipRevision( );

	/* initialize configuration for each module */
#ifdef KDRV_GLOBAL_LINK

#ifdef INCLUDE_KDRV_FBDEV
	FBDEV_PreInit( );
#endif
#ifdef INCLUDE_KDRV_GFX
	GFX_PreInit( );
#endif
#ifdef INCLUDE_KDRV_AFE
    AFE_InitCfg ();
#endif
#ifdef INCLUDE_KDRV_TTX
    TTX_InitCfg ();
#endif

#ifdef INCLUDE_KDRV_SE
//SE is separted from built-in kdriver
#ifndef MAKE_SE_MODULE
	SE_PreInit();
#endif
#endif

#endif
}

void		BASE_DEV_Init	( void )
{
	/* if global link mode, be careful of the sequence of driver initialization */
#ifdef KDRV_GLOBAL_LINK

#ifdef INCLUDE_KDRV_SAMPLE
	SAMPLE_Init( );
#endif

#ifdef INCLUDE_KDRV_SYS
	SYS_Init( );
#endif

#ifdef INCLUDE_KDRV_I2C
	I2C_Init();
#endif /* INCLUDE_KDRV_I2C */

#ifdef INCLUDE_KDRV_DEMOD
	DEMOD_Init( );
#endif

#ifdef INCLUDE_KDRV_ADEC
	ADEC_Init( );
#endif

#ifdef INCLUDE_KDRV_AAD
	AAD_Init( );
#endif

#ifdef INCLUDE_KDRV_AFE
	AFE_Init( );
#endif

#ifdef INCLUDE_KDRV_DE
	DE_Init( );
#endif
#ifdef INCLUDE_KDRV_HDMI
	HDMI_Init( );
#endif
#ifdef INCLUDE_KDRV_FMT3D
	FMT3D_Init( );
#endif
#ifdef INCLUDE_KDRV_DENC
	DENC_Init( );
#endif
#ifdef INCLUDE_KDRV_DIMM
	DIMM_Init( );
#endif
#ifdef INCLUDE_KDRV_PE
	PE_Init( );
#endif

#ifdef INCLUDE_KDRV_GPIO
	GPIO_Init( );
#endif
#ifdef INCLUDE_KDRV_FRC
	FRC_Init( );
#endif

#ifdef INCLUDE_KDRV_SDEC
	SDEC_Init( );
#endif

#ifdef INCLUDE_KDRV_PVR
	PVR_Init( );
#endif

#ifdef INCLUDE_KDRV_VDEC
	VDEC_Init( );
#endif

#ifdef INCLUDE_KDRV_CI
	CI_Init( );
#endif /* INCLUDE_KDRV_CI */

#ifdef INCLUDE_KDRV_TTX
	TTX_Init();
#endif /* INCLUDE_KDRV_TTX */

#ifdef INCLUDE_KDRV_VENC
	VENC_Init( );
#endif

#ifdef INCLUDE_KDRV_PNG
	PNG_Init( );
#endif

#ifdef INCLUDE_KDRV_MJPEG
	MJPEG_Init( );
#endif

#ifdef INCLUDE_KDRV_GFX
	GFX_Init();
#endif
#ifdef INCLUDE_KDRV_FBDEV
	FBDEV_Init();
#endif

#ifdef INCLUDE_KDRV_SE
//SE is separted from built-in kdriver
#ifndef MAKE_SE_MODULE
	SE_Init();
#endif
#endif

#ifdef INCLUDE_KDRV_SCI
	SCI_Init();
#endif

#ifdef INCLUDE_KDRV_CLK
	CLK_Init();
#endif

#ifdef INCLUDE_KDRV_MONITOR
	MONITOR_Init();
#endif

#endif
}

void		BASE_DEV_Cleanup	( void )
{

#ifdef KDRV_GLOBAL_LINK

#ifdef INCLUDE_KDRV_SAMPLE
	SAMPLE_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_DEMOD
	DEMOD_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_ADEC
	ADEC_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_AAD
	AAD_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_AFE
	AFE_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_DE
	DE_Cleanup( );
#endif
#ifdef INCLUDE_KDRV_HDMI
	HDMI_Cleanup( );
#endif
#ifdef INCLUDE_KDRV_FMT3D
	FMT3D_Cleanup( );
#endif
#ifdef INCLUDE_KDRV_DENC
	DENC_Cleanup( );
#endif
#ifdef INCLUDE_KDRV_DIMM
	DIMM_Cleanup( );
#endif
#ifdef INCLUDE_KDRV_PE
	PE_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_GPIO
	GPIO_Cleanup( );
#endif
#ifdef INCLUDE_KDRV_FRC
	FRC_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_I2C
	I2C_Cleanup();
#endif /* INCLUDE_KDRV_I2C */

#ifdef INCLUDE_KDRV_SDEC
	SDEC_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_VDEC
	VDEC_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_CI
	CI_Cleanup( );
#endif /* INCLUDE_KDRV_CI */

#ifdef INCLUDE_KDRV_TTX
	TTX_Cleanup( );
#endif /* INCLUDE_KDRV_TTX */

#ifdef INCLUDE_KDRV_MJPEG
	MJPEG_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_PNG
	PNG_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_VENC
	VENC_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_GFX
	GFX_Cleanup();
#endif
#ifdef INCLUDE_KDRV_FBDEV
	FBDEV_Cleanup();
#endif
#ifdef INCLUDE_KDRV_SE
//SE is separted from built-in kdriver
#ifndef MAKE_SE_MODULE
	SE_Cleanup();
#endif
#endif

#ifdef INCLUDE_KDRV_SYS
	SYS_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_SCI
	SCI_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_CLK
	CLK_Cleanup( );
#endif

#ifdef INCLUDE_KDRV_MONITOR
	MONITOR_Cleanup( );
#endif

#endif

}

/*========================================================================================
	Implementation Group
========================================================================================*/
static void	BASE_DEV_ProbeChipRevision( )
{
	char*	cmd_buf;
	int		cmd_buf_sz = 511;

	char *tok, *sav_tok;
	char delim[] = " \t\n";

	if( NULL == ( cmd_buf = OS_Malloc( cmd_buf_sz+1 )  ) ) goto func_exit;

#ifdef	MODULE
	{
		OS_FILE_T	file;
		if( RET_OK != OS_OpenFile( &file, "/proc/cmdline", O_RDONLY|O_LARGEFILE, 0666 ) )
		{
			printk("<error> can't open /proc/cmdline\n"); goto func_exit;
		}

		if ( 0 >= OS_ReadFile( &file, cmd_buf, cmd_buf_sz ) )
		{
			OS_CloseFile(&file);
			printk("<error> can't read /proc/cmdline\n"); goto func_exit;
		}
		OS_CloseFile(&file);
	}
#else
	{
		extern char *saved_command_line;
		cmd_buf_sz = snprintf( cmd_buf, cmd_buf_sz, "%s", saved_command_line );
	}
#endif

	/* initialize data */
	g_base_dev_cfg.chip_rev = 0x0;
	g_base_dev_cfg.ace_rev  = 0x0;
	g_base_dev_cfg.chip_plt = 0x0;

	{
#define	TOKEN_DCHIP		0x1
#define TOKEN_ACHIP		0x2
#define	TOKEN_PLATFORM	0x4
		UINT8	status = TOKEN_DCHIP|TOKEN_ACHIP|TOKEN_PLATFORM;
		UINT8	chip_model;
		UINT32	chip_rev;
		char	plat_str[0x20];

		tok = simple_strtok( cmd_buf, delim, &sav_tok );
		while (tok)
		{
			chip_model  = 0x0;
			chip_rev 	= 0x0;

			/* find chip=Lxyy */
			if ( (status & TOKEN_DCHIP) && sscanf( tok, "chip=L%c %x", &chip_model, &chip_rev ) == 2 /* num of token */ )
			{
				chip_model -= '0';
				g_base_dev_cfg.chip_rev = MAKE_CHIP_REV( chip_model, chip_rev );

				status &= ~TOKEN_DCHIP;
			}
			else if ( (status & TOKEN_ACHIP) && sscanf( tok, "ace=L%c %x", &chip_model, &chip_rev ) == 2 /* num of token */ )
			{
				chip_model -= '0';
				g_base_dev_cfg.ace_rev = MAKE_CHIP_REV( chip_model, chip_rev );

				status &= ~TOKEN_ACHIP;
			}
			else if ( (status & TOKEN_PLATFORM) && sscanf( tok, "platform=%s", plat_str ) == 1 /* num of token */ )
			{
				g_base_dev_cfg.chip_plt = !strncmp( plat_str, "FPGA", 4 )? LX_CHIP_PLATFORM_FPGA :
										  !strncmp( plat_str, "COSMO",5 )? LX_CHIP_PLATFORM_COSMO: LX_CHIP_PLATFORM_GP;
				status &= ~TOKEN_PLATFORM;
			}

			/* all tokens done, break loop !! */
			if ( !status ) break;

			tok = simple_strtok( NULL, delim, &sav_tok );
		}
	}
func_exit:
	if ( cmd_buf ) OS_Free( cmd_buf );

	/* check if all parameters are parsed successfully.
	 * when valid token is not found, use the some default value.
	 * espacially, the default value of ace_rev is the same value as chip_rev
	 */
	if ( g_base_dev_cfg.chip_rev == 0x0 ) g_base_dev_cfg.chip_rev = LX_CHIP_REV(L9,A1);
	if ( g_base_dev_cfg.ace_rev  == 0x0 ) g_base_dev_cfg.ace_rev  = g_base_dev_cfg.chip_rev;
	if ( g_base_dev_cfg.chip_plt == 0x0 ) g_base_dev_cfg.chip_plt = LX_CHIP_PLATFORM_GP;

	printk("^Y^CHIP_REV=0x%04X ACE_REV=0x%04X CHIP_PLT=0x%04X\n", g_base_dev_cfg.chip_rev, g_base_dev_cfg.ace_rev, g_base_dev_cfg.chip_plt );
}

/** @} */

