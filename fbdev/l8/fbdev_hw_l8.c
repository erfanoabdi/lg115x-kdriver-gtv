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

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "fbdev_impl.h"
#include "fbdev_hw.h"
#include "fbdev_reg_l8.h"

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
volatile OSD_REG_MAP_T*				g_fbdev_reg_l8;
OSD_REG_MAP_T						debug_regs;

static volatile unsigned long		gOsdIrqReg;

DECLARE_MUTEX(fbdev_sem_l8);
/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/
/** initialize OSD IP HW and register
 *
 * @param
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_InitHW		(void)
{
	ULONG	addr;
	int		ret = RET_OK;

	/* access to register */
	g_fbdev_reg_l8 =(OSD_REG_MAP_T *)ioremap( gpFbdevRegCfg->reg_base_addr , gpFbdevRegCfg->reg_size );

	if(g_fbdev_reg_l8 == NULL)
	{
		FBDEV_PRINT("ERROR : can't allocate for register\n");
		return -1;
	}

	/* raxis.lim (2010/06/05) -- set OSD scale factor */
	g_fbdev_reg_l8->osd0_sc_cfg.osd0_sc_bc_sa = 6;
	g_fbdev_reg_l8->osd1_sc_cfg.osd1_sc_bc_sa = 6;
	g_fbdev_reg_l8->osd2_sc_cfg.osd2_sc_bc_sa = 6;

	/* raxis.lim (2010/11/20) -- interrupt register can be accessed anytime */
	gOsdIrqReg = (unsigned long )ioremap(0xc0028030, 0x4);

	/* add extra job */
#if 1 /* MCU */
    if (!request_mem_region(0xc0017000, 0x810, MODULE_NAME))
    {
        FBDEV_ERROR( ": cannot reserve MMIO region\n");
		goto func_exit;
    }

    addr = (unsigned long)ioremap(0xc0017000, 0x810);

    __raw_writel(0xa041f, addr + 0x8);
    __raw_writel(0x10000, addr + 0x800);

    iounmap((void *)addr);
    release_mem_region(0xc0017000, 0x810);
#endif
func_exit:
	return ret;
}

/** de-initial OSD IP HW and register
 *
 * @param
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_ShutdownHW	(void)
{
	int ret = RET_OK;

	iounmap((void *)g_fbdev_reg_l8);
	iounmap((void *)gOsdIrqReg);

	return ret;
}

/** Update OSD ZList
 *
 * @note layer id definition is 00: video, 01: OSD0, 02: OSD1, 03: OSD2
 */
int FBDEV_L8_SetZList ( LX_FBDEV_ZLIST_T* zList )
{
	OSD_LAYER_MUX mux = g_fbdev_reg_l8->osd_layer_mux;

//	printk("<BEFORE> mux3 - %d, mux2 - %d, mux1 - %d, mux0 - %d\n", mux.layer_mux_3, mux.layer_mux_2, mux.layer_mux_1, mux.layer_mux_0 );

#if 1
	mux.layer_mux_0 = zList->order[3] +1;
	mux.layer_mux_1 = zList->order[0] +1;
	mux.layer_mux_2 = zList->order[1] +1;
	mux.layer_mux_3 = zList->order[2] +1;
#else
	mux.layer_mux_3 = zList->order[0] +1;
	mux.layer_mux_2 = zList->order[1] +1;
	mux.layer_mux_1 = zList->order[2] +1;
	mux.layer_mux_0 = zList->order[3] +1;
#endif

//	printk("<AFTER> mux3 - %d, mux2 - %d, mux1 - %d, mux0 - %d\n", mux.layer_mux_3, mux.layer_mux_2, mux.layer_mux_1, mux.layer_mux_0 );

	g_fbdev_reg_l8->osd_layer_mux = mux;
	
	return RET_OK;
}

/** Get OSD ZList
 *
 * @note layer id definition is 00: video, 01: OSD0, 02: OSD1, 03: OSD2
 */
int FBDEV_L8_GetZList( LX_FBDEV_ZLIST_T* zList )
{
	OSD_LAYER_MUX mux = g_fbdev_reg_l8->osd_layer_mux;

#if 1
	zList->order[3] = mux.layer_mux_0 - 1;
	zList->order[0] = mux.layer_mux_1 - 1;
	zList->order[1] = mux.layer_mux_2 - 1;
	zList->order[2] = mux.layer_mux_3 - 1;
#else
	zList->order[0] = mux.layer_mux_3 - 1;
	zList->order[1] = mux.layer_mux_2 - 1;
	zList->order[2] = mux.layer_mux_1 - 1;
	zList->order[3] = mux.layer_mux_0 - 1;
#endif

	return RET_OK;
}

/** OSD HW Layer enable/disable
 *
 * @param	int layer : osd layer ,
 *		enable : 0 : display off , 1 : display on
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_SetOSDEnable(int layer , int enable)
{
	int ret = RET_OK;

	FBDEV_TRACE_BEGIN();

	FBDEV_TRACE("%s:%d -- osd %d, enable %d\n", __FUNCTION__, __LINE__, layer, enable );

	switch(layer)
	{
		case LX_FBDEV_ID_OSD0: 		g_fbdev_reg_l8->osd0_cfg.osd0_en = enable; break;
		case LX_FBDEV_ID_OSD1: 		g_fbdev_reg_l8->osd1_cfg.osd1_en = enable; break;
		case LX_FBDEV_ID_OSD2: 		g_fbdev_reg_l8->osd2_cfg.osd2_en = enable; break;
		case LX_FBDEV_ID_CSR0: 	g_fbdev_reg_l8->crsr_cfg.crsr_en = enable; break;
		default:
			FBDEV_WARN("invalid OSD ID %d\n", layer );
			ret = RET_ERROR;
		break;
	}

	FBDEV_TRACE_END();
	return ret;
}

/** OSD HW Layer status
 *
 * @param	void
 * @return	int 1 : ON , 0 : OFF
 *
 */
int FBDEV_L8_GetOSDEnable(int layer)
{
	int ret = 0;

	switch(layer)
	{
		case LX_FBDEV_ID_OSD0: 		ret = g_fbdev_reg_l8->osd0_cfg.osd0_en; break;
		case LX_FBDEV_ID_OSD1: 		ret = g_fbdev_reg_l8->osd1_cfg.osd1_en; break;
		case LX_FBDEV_ID_OSD2: 		ret = g_fbdev_reg_l8->osd2_cfg.osd2_en; break;
		case LX_FBDEV_ID_CSR0:	ret = g_fbdev_reg_l8->crsr_cfg.crsr_en; break;
		default:
			FBDEV_WARN("invalid OSD ID %d\n", layer );
			ret = -1;
		break;
	}

	return ret;
}

/** make OSD to be initial state
 *	make the basic configuration for each OSD layer.
 *
 *	@param layerId
 */
int FBDEV_L8_InitOSDLayer ( int layer )
{
	int rc;
	int	ret = RET_ERROR;

	/* please refer to OSD manual OSD?_CFG field */
	UINT32	osd_cfg;
	UINT32	osd_hdr_addr;

    /* raxis.lim (note) - 0x1b000000 means that basic configuration on color selection (a,r,g,b)
     *
     *  {osd0_en = 0, osd0_round = 0, osd0_pure_ck = 0, osd0_hdr_ptr_bmp_sel = 0, osd0_hdr_ptr_plte_sel = 0,
     *  osd0_hdr_color_key_sel = 0, osd0_hdr_wpl_sel = 0, osd0_hdr_global_alpha_sel = 0, osd0_hdr_format_sel = 0,
     *  osd0_hdr_depth_sel = 0, osd0_hdr_h_out_sel = 0, osd0_hdr_w_out_sel = 0, osd0_hdr_global_alpha_en_sel = 0,
     *  osd0_hdr_pixel_order_sel = 0, osd0_hdr_h_mem_sel = 0, osd0_hdr_w_mem_sel = 0, osd0_hdr_ypos_sel = 0, osd0_hdr_xpos_sel = 0,
     *  osd0_hdr_color_key_en_sel = 0, osd0_ch_blue = 3, osd0_ch_green = 2, osd0_ch_red = 1, osd0_ch_alpha = 0
     *
     *  [note] OSD will be hidden osdx_en bit cleared
     */
	osd_cfg = 0x1b000000;	

    /* raxis.lim (2011/04/20) -- L9A0 bug fix */
    if ( lx_chip_rev() == LX_CHIP_REV(L9,A0) )
    {
        osd_cfg |= 0x8;
    }

	/* set DDR address which contains OSD header information */
    if ( gpFbdevRegCfg->slkrd_prefix_mask )
    {
        osd_hdr_addr = (( (WORD_TO_BYTE(gMemCfgFBHdr[layer].mem_header_base))&0x0FFFFFFF)| gpFbdevRegCfg->slkrd_prefix_mask );
    }
    else
    {
        osd_hdr_addr = WORD_TO_BYTE(gMemCfgFBHdr[layer].mem_header_base);
    }

	rc = down_interruptible(&fbdev_sem_l8);
	FBDEV_CHECK_CODE( rc, goto func_exit, "can't lock\n" );
	
	switch( layer )
	{
		case LX_FBDEV_ID_OSD0:
			memcpy( (void *)&g_fbdev_reg_l8->osd0_cfg, &osd_cfg, sizeof(unsigned int) );
			g_fbdev_reg_l8->osd0_base_addr.osd0_base_addr = osd_hdr_addr;	/* set header data address */
		break;

		case LX_FBDEV_ID_OSD1:
			memcpy( (void *)&g_fbdev_reg_l8->osd1_cfg, &osd_cfg, sizeof(unsigned int) );
			g_fbdev_reg_l8->osd1_base_addr.osd1_base_addr = osd_hdr_addr;	/* set header data address */
		break;

		case LX_FBDEV_ID_OSD2:
			memcpy( (void *)&g_fbdev_reg_l8->osd2_cfg, &osd_cfg, sizeof(unsigned int) );
			g_fbdev_reg_l8->osd2_base_addr.osd2_base_addr = osd_hdr_addr;	/* set header data address */
		break;

		case LX_FBDEV_ID_CSR0:
			memcpy( (void *)&g_fbdev_reg_l8->crsr_cfg, &osd_cfg, sizeof(unsigned int) );
			g_fbdev_reg_l8->crsr_base_addr.crsr_base_addr = osd_hdr_addr;	/* set header data address */
		break;

		default:
			FBDEV_ERROR("invalid osd %d\n", layer ); goto func_exit;
		break;
	}

	ret = RET_OK;	/* all work done */
func_exit:
	up(&fbdev_sem_l8);

	return RET_OK;
}

/** set endianess on each layer
 *
 * @param	ENDIAN_INFORM_T
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_WriteOSDEndian(int layer , ENDIAN_INFORM_T *endian)
{
	int ret = RET_OK;

	unsigned int temp;

	temp = (unsigned int)((endian->header << 8) | (endian->palette << 4) | (endian->image << 0) );

	switch(layer){
		case LX_FBDEV_ID_OSD0:
			memcpy( (void *)&g_fbdev_reg_l8->osd0_end_conv , (void *)&temp , sizeof(int) );
		break;

		case LX_FBDEV_ID_OSD1:
			memcpy( (void *)&g_fbdev_reg_l8->osd1_end_conv ,(void *) &temp , sizeof(int) );
		break;

		case LX_FBDEV_ID_OSD2:
			memcpy( (void *)&g_fbdev_reg_l8->osd2_end_conv , (void *)&temp , sizeof(int) );
		break;

		case LX_FBDEV_ID_CSR0:
			memcpy( (void *)&g_fbdev_reg_l8->crsr_end_conv , (void *) &temp , sizeof(int) );
		break;
		default:
			FBDEV_PRINT("WARNING : invalid int type %s %d\n" , __func__ , __LINE__);
			ret = RET_ERROR;
		break;
	}

	return ret;
}

/**
 *
 * set color space conversion on each layer
 *
 * @param	int layer , CSC_INFORM_T *csc
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_WriteOSDCSC(int layer , CSC_INFORM_T *csc)
{
	int ret = 0;

	switch(layer){
		case LX_FBDEV_ID_OSD0:
			g_fbdev_reg_l8->osd0_csc_ctrl.osd0_csc_data = csc->data;
			g_fbdev_reg_l8->osd0_csc_ctrl.osd0_csc_rw_n = csc->rw_n;
			g_fbdev_reg_l8->osd0_csc_ctrl.osd0_csc_bypass = csc->bypass;
			g_fbdev_reg_l8->osd0_csc_ctrl.osd0_csc_sel = csc->sel;
			g_fbdev_reg_l8->osd0_csc_ctrl.osd0_csc_out_ctrl = csc->out_ctrl;
			g_fbdev_reg_l8->osd0_csc_ctrl.osd0_csc_in_ctrl = csc->in_ctrl;

		break;

		case LX_FBDEV_ID_OSD1:
			g_fbdev_reg_l8->osd1_csc_ctrl.osd1_csc_data = csc->data;
			g_fbdev_reg_l8->osd1_csc_ctrl.osd1_csc_rw_n = csc->rw_n;
			g_fbdev_reg_l8->osd1_csc_ctrl.osd1_csc_bypass = csc->bypass;
			g_fbdev_reg_l8->osd1_csc_ctrl.osd1_csc_sel = csc->sel;
			g_fbdev_reg_l8->osd1_csc_ctrl.osd1_csc_out_ctrl = csc->out_ctrl;
			g_fbdev_reg_l8->osd1_csc_ctrl.osd1_csc_in_ctrl = csc->in_ctrl;
		break;

		case LX_FBDEV_ID_OSD2:
			g_fbdev_reg_l8->osd2_csc_ctrl.osd2_csc_data = csc->data;
			g_fbdev_reg_l8->osd2_csc_ctrl.osd2_csc_rw_n = csc->rw_n;
			g_fbdev_reg_l8->osd2_csc_ctrl.osd2_csc_bypass = csc->bypass;
			g_fbdev_reg_l8->osd2_csc_ctrl.osd2_csc_sel = csc->sel;
			g_fbdev_reg_l8->osd2_csc_ctrl.osd2_csc_out_ctrl = csc->out_ctrl;
			g_fbdev_reg_l8->osd2_csc_ctrl.osd2_csc_in_ctrl = csc->in_ctrl;

		break;

		case LX_FBDEV_ID_CSR0:

		break;
		default:
			FBDEV_PRINT("WARNING : invalid int type %s %d\n" , __func__ , __LINE__);

			return -1;
		break;
	}

	return ret;
}

/** Get interrupt status
 * [note] If you don't need any interrupt service of FB device, DO NOT call this function
 *
 * @param	INTERRUPT_INFORM_T
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_GetInterruptStatus(INTERRUPT_INFORM_T intr)
{
	int ret = RET_OK;

	unsigned long val;
	val = __raw_readl(gOsdIrqReg );
	ret = (int)val;

	return ret;
}

/** Clear interrupt status
 * [note] If you don't need any interrupt service of FB device, DO NOT call this function
 *
 * @param	INTERRUPT_INFORM_T
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_SetInterruptClear(INTERRUPT_INFORM_T intr)
{
	int ret = RET_OK;
	unsigned long val;

	val = __raw_readl(gOsdIrqReg );
	val |= (1<<8);
	__raw_writel( val , gOsdIrqReg );

	return ret;
}

/** Set interrupt enable/disable
 * [note] If you don't need any interrupt service of FB device, DO NOT call this function
 *
 * @param	INTERRUPT_INFORM_T , int : enable/disable
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_SetInterruptEnable(INTERRUPT_INFORM_T intr , int enable)
{
	int ret = 0;
	unsigned long val;

	val = __raw_readl(gOsdIrqReg );

	if(enable) 	{ val |= (1<<4); }
	else 		{ val &= ~(1<<4); }

	__raw_writel( val , gOsdIrqReg );

	return ret;
}

/** download the palette data into DDRAM which is on specific location from OSD
 *
 * @param	unsigned int addr , unsigned int size , void *data
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
int FBDEV_L8_DownloadPalette(unsigned int addr , unsigned int size , void *data)
{
	int ret = 0;
	unsigned long base;

	base = (unsigned long)ioremap( addr, size );
	memcpy ((void*)base,(void*)data, size);
	iounmap((void*)base);
	return ret;
}
/** Set Header information
 *
 * @param	SCanvas : pointer to structure bearing osd header information
 * @return	void
 *
 * @ingroup osd_ddi_func
 */
int FBDEV_L8_WriteOSDHeader(struct scanvas *hCanvas)
{
	int		ret;
	FBDEV_OSD_HDR_T osd_hdr;
	memset( (void*)&osd_hdr, 0x0, sizeof(FBDEV_OSD_HDR_T) );

	ret = down_interruptible(&fbdev_sem_l8);

#if 1
	/* raxis.lim (2010/12/03)
	 * -- OSD 와 CRSR 의 register 크기가 틀려서 틀려서 동일 코드가 중복되어 있다. 차후 정리 요망.		
	 */
	if ( hCanvas->fb_dev_id != LX_FBDEV_ID_CSR0 )
	{
		osd_hdr.type.osd.osd_hdr_color_key_en 	= hCanvas->bColorKeyEnable;
		osd_hdr.type.osd.osd_hdr_w_out			= hCanvas->output_win.w;	/* output w */
		osd_hdr.type.osd.osd_hdr_h_out			= hCanvas->output_win.h;	/* output h */

		osd_hdr.type.osd.osd_hdr_xpos		 	= hCanvas->output_win.x;	/* output x */
		osd_hdr.type.osd.osd_hdr_ypos 			= hCanvas->output_win.y;	/* output y */

		osd_hdr.type.osd.osd_hdr_w_mem 			= hCanvas->input_win.w;		/* input w */
		osd_hdr.type.osd.osd_hdr_h_mem 			= hCanvas->input_win.h;		/* input h */
		osd_hdr.type.osd.osd_hdr_pixel_order	= hCanvas->bPixelOrderEnable;
	}
	else
	{
		osd_hdr.type.crsr.osd_hdr_xpos		 	= hCanvas->output_win.x;	/* output x (cursor x pos) */
		osd_hdr.type.crsr.osd_hdr_ypos 			= hCanvas->output_win.y;	/* output y (cursor y pos) */

		osd_hdr.type.crsr.osd_hdr_w_mem 		= hCanvas->input_win.w;		/* input w */
		osd_hdr.type.crsr.osd_hdr_h_mem 		= hCanvas->input_win.h;		/* input h */
		osd_hdr.type.crsr.osd_hdr_pixel_order	= hCanvas->bPixelOrderEnable;
	}
		
	osd_hdr.osd_hdr_wpl 			= hCanvas->stride/8;
	osd_hdr.osd_hdr_format 			= hCanvas->eOSDPixFmt;
	osd_hdr.osd_hdr_depth 			= hCanvas->eOSDPixDepth;
	osd_hdr.osd_hdr_global_alpha_en	= hCanvas->bGlobalAlphaEnable;
	osd_hdr.osd_hdr_global_alpha	= hCanvas->usGlobalAlpha;
	osd_hdr.osd_hdr_color_key		= hCanvas->uiOSDColorKey;

	if ( gpFbdevRegCfg->slkrd_prefix_mask )
	{
		osd_hdr.osd_hdr_ptr_bmp		= ((hCanvas->uiBitmapDataYPTR&0x0FFFFFFF)| gpFbdevRegCfg->slkrd_prefix_mask );
		osd_hdr.osd_hdr_ptr_plte	= ((hCanvas->uiPalettePTR&0x0FFFFFFF)| gpFbdevRegCfg->slkrd_prefix_mask );
	}
	else
	{
		osd_hdr.osd_hdr_ptr_bmp		= hCanvas->uiBitmapDataYPTR;
		osd_hdr.osd_hdr_ptr_plte	= hCanvas->uiPalettePTR;
	}

//	if ( osd_hdr.osd_hdr_wpl % 2 ) ? osd_hdr.osd_hdr_wpl += 1;

	/* write hdr data */
	memcpy ( (void*)hCanvas->uiOSDHeaderPTR, (void*)&osd_hdr, sizeof(FBDEV_OSD_HDR_T) );
	/* read data back for prevent data coherency problem */
	memcpy ( (void*)&osd_hdr, (void*)hCanvas->uiOSDHeaderPTR, sizeof(FBDEV_OSD_HDR_T) );
#else
    LG1150_OSDHEADER_T *sOsdHeaderConfig;

    sOsdHeaderConfig = kmalloc(sizeof(LG1150_OSDHEADER_T)  ,GFP_KERNEL );

    memset((void *)sOsdHeaderConfig, 0, sizeof(LG1150_OSDHEADER_T));

    /// Set osd header information
    sOsdHeaderConfig->x = hCanvas->output_win.x;
    sOsdHeaderConfig->y = hCanvas->output_win.y;

    if(hCanvas->fb_dev_id != LX_FBDEV_ID_CSR0)
    {
        if(hCanvas->bColorKeyEnable) sOsdHeaderConfig->x |= 0x8000;
    }

	sOsdHeaderConfig->in_width = hCanvas->output_win.w;
	sOsdHeaderConfig->in_height = hCanvas->output_win.h;

	sOsdHeaderConfig->out_width = hCanvas->input_win.w;
	sOsdHeaderConfig->out_height = hCanvas->input_win.h;

    if(hCanvas->fb_dev_id != LX_FBDEV_ID_CSR0)
    {
        if(hCanvas->bPixelOrderEnable) sOsdHeaderConfig->out_width |= 0x8000;
        else sOsdHeaderConfig->out_width &= ~(0x8000);
    }
    else
    {
        sOsdHeaderConfig->out_width = (hCanvas->output_win.w*hCanvas->input_win.w | 0x0000);
    }

    sOsdHeaderConfig->color_key = hCanvas->uiOSDColorKey;

    sOsdHeaderConfig->depth_format_alphaEn = (unsigned char)(hCanvas->eOSDPixDepth<<4)|(hCanvas->eOSDPixFmt<<0);

    sOsdHeaderConfig->alpha = hCanvas->usGlobalAlpha;
    sOsdHeaderConfig->stride = hCanvas->stride/8;


    if(sOsdHeaderConfig->stride%2) sOsdHeaderConfig->stride += 1;

    /*  raxis.lim (2010/05/24) -- bug fix
     *  osd global alpha should be controlled by application not pixel format.
     */
    if(hCanvas->bGlobalAlphaEnable)   sOsdHeaderConfig->depth_format_alphaEn |=  0x80;
    else                                sOsdHeaderConfig->depth_format_alphaEn &= ~0x80;

    if(hCanvas->fb_dev_id == LX_FBDEV_ID_CSR0)
    {
//      sOsdHeaderConfig->stride &= ~(0x7FF);   ????????
        if(hCanvas->bColorKeyEnable) sOsdHeaderConfig->stride |= 0x8000;
        if(hCanvas->bPixelOrderEnable) sOsdHeaderConfig->stride |= 0x4000;
    }

    sOsdHeaderConfig->bitmap_addr   = ((hCanvas->uiBitmapDataYPTR&0x0FFFFFFF)|_SILKROAD_PREFIX_);
    sOsdHeaderConfig->palette_addr  = ((hCanvas->uiPalettePTR&0x0FFFFFFF)|_SILKROAD_PREFIX_);

	OS_RegHexDumpEx( hCanvas->fb_dev_id, sOsdHeaderConfig,sizeof(LG1150_OSDHEADER_T), NULL );

    // should be check the header pointer
    memcpy ((void *)(hCanvas->uiOSDHeaderPTR),(void *)sOsdHeaderConfig,sizeof(LG1150_OSDHEADER_T));
    // for prevent data coherency problem
    memcpy ((void *)sOsdHeaderConfig,(void *)(hCanvas->uiOSDHeaderPTR),sizeof(LG1150_OSDHEADER_T));
    kfree((void *)sOsdHeaderConfig);
#endif

	up(&fbdev_sem_l8);

	return RET_OK;
}

int	FBDEV_L8_RunResume	( void )
{
    unsigned long addr;

#if 1 /* MCU */
    addr = (unsigned long)ioremap(0xc0017000, 0x810);

    __raw_writel(0xa041f, addr + 0x8);
    __raw_writel(0x10000, addr + 0x800);

    iounmap((void *)addr);
#endif
	return RET_OK;
}

int	FBDEV_L8_RunSuspend	( void )
{
	/* do nothing */
	return RET_OK;
}
