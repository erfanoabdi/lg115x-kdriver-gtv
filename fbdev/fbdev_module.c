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
 *  main driver implementation for fbdev device.
 *
 *  author      raxis.lim (raxis.lim@lge.com)
 *  version     1.0
 *  date        2009.12.30
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
#include "fbdev_impl.h"
#include "fbdev_hw.h"
#include "fbdev_hal.h"

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

/*----------------------------------------------------------------------------------------
    global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static ENDIAN_INFORM_T	FBDEV_GetImageEndian( int bpp );

/*----------------------------------------------------------------------------------------
    Static Variables
----------------------------------------------------------------------------------------*/
/** get endian value for given bpp information
 *
 *
 */
static ENDIAN_INFORM_T	FBDEV_GetImageEndian( int bpp )
{
    ENDIAN_INFORM_T endian;

    switch(bpp)
    {
        case 8:
            endian.header   = 4;
            endian.palette  = 4;
            endian.image    = 7;
            break;
        case 16:
            endian.header   = 4;
            endian.palette  = 4;
            endian.image    = 6;
            break;

        case 32:
            endian.header   = 4;
            endian.palette  = 4;
            endian.image    = 4;
            break;

        default:
            endian.header   = 4;
            endian.palette  = 4;
            endian.image    = 4;
            break;
    }

	return endian;
}

void FBDEV_ChangeImageEndian(int layer, int bpp)
{
	ENDIAN_INFORM_T endian = FBDEV_GetImageEndian( bpp );
	FBDEV_WriteOSDEndian(layer , &endian);
}

static UINT8   FBDEV_GetPxlFmt             (int bpp)
{
	switch(bpp)
	{
		case 8:		return ARGB8888;
		case 16:	return RGB565;
		case 32:
		default:	return ARGB8888;
	}
}

static UINT8   FBDEV_GetPxlDepth           (int bpp)
{
	switch(bpp)
	{
		case 8:		return PIX_DEPTH_8_BPP;
		case 16:	return PIX_DEPTH_16_BPP;
		case 32:
		default:	return PIX_DEPTH_32_BPP;
	}
}

/**
 *
 * initialize layer's memory and palette.
 *
 * @param	struct fb_info *info
 * @return	void
 *
 */
void FBDEV_InitCanvas(struct dm_win_info *winfo)
{
	CSC_INFORM_T				csc_inform;
	ENDIAN_INFORM_T				endian_inform;

	int							fb_dev_id = winfo->fb_dev_id;
	struct fb_var_screeninfo*	vinfo	= &winfo->fbinfo.var;
	struct scanvas* 			hCanvas	= &winfo->pixel;

#ifdef _USING_PALETE_
	UINT32	*pInitPal;
#endif
	/* configure OSD.
	 * this function will inform OSD hardware to read OSD header data from the designated DDR ( canv->baddr )
 	 */
	FBDEV_InitOSDLayer( fb_dev_id );

	/* set the default CSC param
	 *
	 * [note] We don't use YCbCr color space, so any CSC is not necessary.
	 * [note] CSC is not set anymore after this point.
	 */
	memset( &csc_inform, 0x0, sizeof(CSC_INFORM_T));
	csc_inform.bypass = 1;

	/* determine OSD endian info
	 * [note] You can see FBDEV_GetImangeEndian function process only RGB color space.
	 * 		  If you need to need YCbCr color space, we should upgrade that function.
	 * [note] endian is not set anymore after this point. ( one exception is the change of pixel format )
	 */
	memset( &endian_inform, 0x0, sizeof(ENDIAN_INFORM_T) );
	endian_inform = FBDEV_GetImageEndian( vinfo->bits_per_pixel );

    FBDEV_WriteOSDEndian( fb_dev_id, &endian_inform);
    FBDEV_WriteOSDCSC	( fb_dev_id, &csc_inform);

	/* initialize image data */
	hCanvas->fb_dev_id				= fb_dev_id;
	hCanvas->uiOSDHeaderPTR		= winfo->osd_header_base;
	hCanvas->uiPalettePTR 		= (gMemCfgFBDev[fb_dev_id].mem_palette_base);
	hCanvas->uiBasePTR 			= (gMemCfgFBDev[fb_dev_id].mem_frame_base);
	hCanvas->uiBitmapDataYPTR 	= hCanvas->uiBasePTR;

	hCanvas->input_win.x		= 0;
	hCanvas->input_win.y		= 0;
	hCanvas->input_win.w		= vinfo->xres;		/* default input_win to be same as var->xres */
	hCanvas->input_win.h		= vinfo->yres;		/* default input_win to be same as var->xres */

	hCanvas->output_win.x		= 0;
	hCanvas->output_win.y		= 0;
	hCanvas->output_win.w		= vinfo->xres;		/* default output_win to be same as var->xres */
	hCanvas->output_win.h		= vinfo->yres;		/* default output_win to be same as var->xres */

	hCanvas->bColorKeyEnable	= 0;
	hCanvas->bGlobalAlphaEnable	= g_fbdev_cfg->fb[fb_dev_id].b_use_global_alpha;
	hCanvas->bPixelOrderEnable	= 0;

	hCanvas->eOSDPixFmt 		= FBDEV_GetPxlFmt( vinfo->bits_per_pixel );
	hCanvas->eOSDPixDepth		= FBDEV_GetPxlDepth ( vinfo->bits_per_pixel );
	hCanvas->usGlobalAlpha		= 0xff;

	/* calucate stride */
	hCanvas->stride				= vinfo->xres_virtual * vinfo->bits_per_pixel / 8;

	/* sync current data to OSD header register. */
	FBDEV_WriteOSDHeader( hCanvas );

	// header setting by register
#ifdef _USING_PALETE_
	pInitPal = (UINT32*)OS_Malloc(256*sizeof(UINT32));
	/* Initialize Palette Data with RGBYCBCR or Grey scale for driver test.
	 * These data will be replaced with those from ACC/DCC/Xm.
	 * */
	defaultPal(pInitPal);
	FBDEV_SetInitPalette(0,0,256,pInitPal);
	OS_Free(pInitPal);
#endif
}

/** memory freeing for each header.
 *
 * @param	dm_win_info **win - osd plane information
 * @return	int 0 : OK , -1 : NOT OK
 */
int FBDEV_CleanupLayer(struct dm_win_info *winfo)
{
	if ( winfo )
	{
		if ( winfo->osd_header_base ) 		iounmap((void *)winfo->osd_header_base);
		if ( winfo->osd_header_base_phys ) 	release_mem_region(winfo->osd_header_base_phys, winfo->osd_header_size);
		if ( winfo->osd_pal_base_phys ) 	release_mem_region(winfo->osd_pal_base_phys, winfo->osd_pal_size);

		kfree(winfo);
	}
	return (0);
}

/** memory allocation for each header.
 *
 * @param dm_win_info **win - osd plane information
 * @param unsigned long fb_base_phys - framebuffer physical address
 * @param unsigned long fb_size - framebuffer size
 * @param char *fbname - osd layer name
 * @return	int 0 : OK , -1 : NOT OK
 */
int FBDEV_InitLayer(struct dm_win_info **win, unsigned long fb_base_phys, unsigned long fb_size, char *fbname)
{
	struct dm_win_info *w;

	w = OS_Malloc(sizeof(struct dm_win_info));
	FBDEV_CHECK_CODE( !w, return RET_ERROR, "out of memory. can't alloc %s\n", fbname );

	memset(w, 0, sizeof(struct dm_win_info));

	w->fb_base_phys = fb_base_phys;
	w->fb_size		= fb_size;

 	/* osd header address setting */
	if( is_win(fbname,OSD0) )
	{
		w->fb_dev_id			= 0;
		w->osd_header_base_phys = gMemCfgFBHdr[0].mem_header_base;
		w->osd_header_size 		= gMemCfgFBHdr[0].mem_header_size;
		w->osd_pal_base_phys 	= gMemCfgFBDev[0].mem_palette_base;
		w->osd_pal_size 		= gMemCfgFBDev[0].mem_palette_size ;
	}
	else if(is_win(fbname,OSD1) )
	{
		w->fb_dev_id			= 1;
		w->osd_header_base_phys = gMemCfgFBHdr[1].mem_header_base;
		w->osd_header_size 		= gMemCfgFBHdr[1].mem_header_size;
		w->osd_pal_base_phys 	= gMemCfgFBDev[1].mem_palette_base;
		w->osd_pal_size 		= gMemCfgFBDev[1].mem_palette_size;
	}
	else if( is_win(fbname,OSD2) )
	{
		w->fb_dev_id			= 2;
		w->osd_header_base_phys = gMemCfgFBHdr[2].mem_header_base;
		w->osd_header_size		= gMemCfgFBHdr[2].mem_header_size;
		w->osd_pal_base_phys	= gMemCfgFBDev[2].mem_palette_base;
		w->osd_pal_size			= gMemCfgFBDev[2].mem_palette_size;
	}
	else if(is_win(fbname,OSD_CURSOR) )
	{
		w->fb_dev_id			= 3;
		w->osd_header_base_phys = gMemCfgFBHdr[3].mem_header_base;
		w->osd_header_size		= gMemCfgFBHdr[3].mem_header_size;
		w->osd_pal_base_phys	= gMemCfgFBDev[3].mem_palette_base;
		w->osd_pal_size			= gMemCfgFBDev[3].mem_palette_size;
	}
	else
	{
		FBDEV_PRINT("%s: cannot define osd header region\n", fbname);
	}

	w->osd_header_base = (unsigned long)ioremap(w->osd_header_base_phys, w->osd_header_size);

	if (!w->osd_header_base) {
		FBDEV_PRINT("%s: cannot map framebuffer\n", fbname);
		goto release_fb;
	}

	*win = w;
	return RET_OK;

release_fb:
	if (!w->alloc_fb_mem)
  		release_mem_region(w->fb_base_phys, w->fb_size);

	OS_Free(w);
	return RET_ERROR;
}

/** control OSD output connection
 *
 */ 
int     FBDEV_SetConnCtrl           (LX_FBDEV_CONN_CTRL_T* pConnCtrl)
{
	int	ret = RET_OK;

	if ( lx_chip_rev() < LX_CHIP_REV(L9,A0) ) return RET_NOT_SUPPORTED;

	if ( g_fbdev_hal.SetConnCtrl )
	{
		ret = g_fbdev_hal.SetConnCtrl( pConnCtrl );
	}

	return ret;
}

/**
 *	default palette data
 */
void	FBDEV_GetDefaultPalData(UINT32 *pPal)
{
	int				i;
	UINT32	r=0,g=0,b=0;
	UINT32	alpha;

	for (i=0;i<256;i++)
	{
		switch (i&0x7)
		{
			case 0: r=g=b=255; break;
			case 1: r=255; break;
			case 2: g=255; break;
			case 3: b=255; break;
			case 4: r=b=255; break;
			case 5: b=g=255; break;
			case 6: r=g=255; break;
		}
		alpha = (~i)&0xFF;
		*(pPal+i) = (alpha<<24)|(r<<16)|(g<<8)|(b);

		r^=r;
		g^=g;
		b^=b; /// clear r/g/b values
	}
#define	TR_PIXEL		0x00108080		// OSD palette pixel
	*pPal = TR_PIXEL; /// transparent color for index 0
#undef	TR_PIXEL
	return;

}

/** Set Palette
 *
 * @param	index		palette index
 * @param	pPalette	pointer to array bearing palette data
 *
 * @return	void
 */
void OSD_SetPalette( struct dm_win_info *w,	UINT32 size_data, void *pPalette)
{
	unsigned long base_phys;
	unsigned long size;

	if( size_data > 4*256)
	{
		FBDEV_PRINT("palette data is too big\n");
		return;
	}

	base_phys	= w->osd_pal_base_phys;
	size		= w->osd_pal_size;

	FBDEV_DownloadPalette( base_phys , size_data , pPalette);

	return;
}

/*========================================================================================
    Implementation Group (HAL Interface)
========================================================================================*/

/** initialize OSD IP HW and register
 *
 * @param
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_InitHardware(void)
{
	return g_fbdev_hal.InitHW();
}

/** de-initial OSD IP HW and register
 *
 * @param
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_ExitHardware(void)
{
	return g_fbdev_hal.ShutdownHW();
}

int	FBDEV_RunResume(void)
{
	return g_fbdev_hal.RunResume();
}

int FBDEV_RunSuspend(void)
{
	return g_fbdev_hal.RunSuspend();
}

/** Update OSD ZList
 *
 * @note layer id definition is 00: video, 01: OSD0, 02: OSD1, 03: OSD2
 */
int FBDEV_SetZList ( LX_FBDEV_ZLIST_T* zList )
{
	return g_fbdev_hal.SetZList( zList );
}

/** Get OSD ZList
 *
 * @note layer id definition is 00: video, 01: OSD0, 02: OSD1, 03: OSD2
 */
int FBDEV_GetZList( LX_FBDEV_ZLIST_T* zList )
{
	return g_fbdev_hal.GetZList ( zList );
}

/** OSD HW Layer enable/disable
 *
 * @param	int layer : osd layer ,
 *		enable : 0 : display off , 1 : display on
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_SetOSDEnable(int layer , int enable)
{
	return g_fbdev_hal.SetOSDEnable( layer, enable );
}

/** OSD HW Layer status
 *
 * @param	void
 * @return	int 1 : ON , 0 : OFF
 *
 */
int FBDEV_GetOSDEnable	(int layer)
{
	return g_fbdev_hal.GetOSDEnable( layer );
}

/** make the basic configuration for each OSD layer.
 *
 * [note] Do NOT use header setting in register
 *
 * @param	[IN] layer
 * @return	int 0 : OK , -1 : NOT OK
 */
int FBDEV_InitOSDLayer	(int layer)
{
	return g_fbdev_hal.InitOSDLayer(layer);
}

/** set endianess on each layer
 *
 * @param	ENDIAN_INFORM_T
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_WriteOSDEndian(int layer , ENDIAN_INFORM_T *endian)
{
	return g_fbdev_hal.WriteOSDEndian( layer, endian );
}

/**
 *
 * set color space conversion on each layer
 *
 * @param	int layer , CSC_INFORM_T *csc
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_WriteOSDCSC(int layer , CSC_INFORM_T *csc)
{
	return g_fbdev_hal.WriteOSDCSC( layer, csc );
}

/** Get interrupt status
 * [note] If you don't need any interrupt service of FB device, DO NOT call this function
 *
 * @param	INTERRUPT_INFORM_T
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_GetInterruptStatus(INTERRUPT_INFORM_T intr)
{
	return g_fbdev_hal.GetInterruptStatus( intr );
}

/** Clear interrupt status
 * [note] If you don't need any interrupt service of FB device, DO NOT call this function
 *
 * @param	INTERRUPT_INFORM_T
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_SetInterruptClear(INTERRUPT_INFORM_T intr)
{
	return g_fbdev_hal.SetInterruptClear( intr );
}

/** Set interrupt enable/disable
 * [note] If you don't need any interrupt service of FB device, DO NOT call this function
 *
 * @param	INTERRUPT_INFORM_T , int : enable/disable
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_SetInterruptEnable(INTERRUPT_INFORM_T intr , int enable)
{
	return g_fbdev_hal.SetInterruptEnable( intr, enable );
}

/** download the palette data into DDRAM which is on specific location from OSD
 *
 * @param	unsigned int addr , unsigned int size , void *data
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_DownloadPalette(unsigned int addr , unsigned int size , void *data)
{
	return g_fbdev_hal.DownloadPalette( addr, size, data );
}
/** Set Header information
 *
 * @param	SCanvas : pointer to structure bearing osd header information
 * @return	void
 *
 * @ingroup osd_ddi_func
 */
int FBDEV_WriteOSDHeader(struct scanvas *hCanvas)
{
	if ( g_fbdev_osd_freeze )
	{
		FBDEV_ERROR("osd freezed\n");		
		return RET_OK;
	}

	return g_fbdev_hal.WriteOSDHeader( hCanvas );
}

int	FBDEV_SetTridCtrl( int layer, LX_FBDEV_TRID_CTRL_T* ctrl )
{
	int	ret = RET_OK;
	if ( g_fbdev_hal.SetTridCtrl )
	{
		ret = g_fbdev_hal.SetTridCtrl( layer, ctrl );

		if ( ret == RET_OK )
		{
			g_fbdev_trid_ctrl.fb[layer] = *ctrl;
		}
	}
	return ret;
}

/** control OSD output path
 *
 */
int FBDEV_SetOutPathCtrl( int layer, LX_FBDEV_OUT_PATH_CTRL_T* ctrl)
{
	int	ret = RET_OK;
	if ( g_fbdev_hal.SetOutPathCtrl )
	{
		ret = g_fbdev_hal.SetOutPathCtrl( layer, ctrl );
	}
	return ret;
}

/** control video mixer
 *
 */
int	FBDEV_SetVideoMixerCtrl( LX_FBDEV_VIDEO_MIXER_CTRL_T* ctrl)
{
	int	ret = RET_OK;
	if ( g_fbdev_hal.SetVideoMixerCtrl )
	{
		ret = g_fbdev_hal.SetVideoMixerCtrl( ctrl );
	}
	return ret;
}

/** ISR based tick handelr
 *
 */
void FBDEV_ISRTickHandler ( UINT32 int_cnt )
{
	if ( g_fbdev_hal.ISRTickHandler )
	{
		g_fbdev_hal.ISRTickHandler( int_cnt );
	}
}

void FBDEV_WriteReg           (UINT32 addr, UINT32 val)
{
    UINT32 vaddr = (unsigned long)ioremap( addr, 4 );

    if ( vaddr )
    {
        __raw_writel( val, vaddr );
        val = __raw_readl( vaddr );
        iounmap( (void*)vaddr );

		printk("0x%08x = 0x%08x (ok?)\n", addr, val );
    }
    else
    {
        printk("ioremap error\n");
    }
}

UINT32 FBDEV_ReadReg          (UINT32 addr )
{
    UINT32 val = 0x0;
    UINT32 vaddr = (unsigned long)ioremap( addr, 4 );

    if ( vaddr )
    {
        val = __raw_readl( vaddr );
        iounmap( (void*)vaddr );
    }
    else
    {
        printk("ioremap error\n");
    }

	return val;
}

