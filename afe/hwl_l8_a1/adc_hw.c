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
 *	ADC hw control
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
#include <linux/sched.h>
#include <linux/kernel.h>	/**< printk() */
#include <linux/slab.h> 	/**< kmalloc() */
#include <linux/fs.h> 		/**< everything\ldots{} */
#include <linux/types.h> 	/**< size_t */
#include <linux/fcntl.h>	/**< O_ACCMODE */
#include <asm/uaccess.h>
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <asm/io.h>			/**< For ioremap_nocache */
#include <linux/interrupt.h>	/** interrupt */
#include <linux/platform_device.h>
#include <linux/wait.h>		/** wait_event_interrupt_timeout	*/
#include <linux/delay.h>	/** for mdelay	*/

#include "afe_kapi.h"


#include "vport_reg.h"
//#include "vport_reg.h"
#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"
#include "afe_drv.h"
/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define INTERLACE_CHECK_COUNT 7

#define ADC_DETECTION_INTERRUPT_MASK	0x0000068A
//#define ADC_DETECTION_INTERRUPT_MASK	0x00000E8A
#define ADC_AOGC_INTERRUPT_MASK			0x0000C000
//#define ADC_BOUNDARY_COMPENSATION
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
extern int gResolution;
extern volatile VPORT_RegAx_T *gVportRegAx;
static LX_AFE_ADC_INPUT_SOURCE_TYPE_T gADC_type= LX_ADC_INPUT_SOURCE_NONE;
#ifdef	KDRV_CONFIG_PM
typedef	struct{
	S_SEL_PORT gstPmSelPortValue;
	S_EXTAU_VALUE gstPmExtauValue;
	S_RGB_POSITION gstPmPositionValue;
	unsigned int guiPmPhaseValue;
	LX_AFE_ADC_GAIN_VALUE_T gstPmGainValue;
	LX_AFE_ADC_OFFSET_VALUE_T gstPmOffsetValue;
	int giPmIndexValue;
	SINT16 giPmClockValue;
	unsigned int guiPmInterruptStatus;
}S_ADC_PM_DATA;

static S_ADC_PM_DATA *gAdcPMData;

static int gAdcPmInitial = -1;
#endif
/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static DECLARE_WAIT_QUEUE_HEAD(agoc_done);
static int giAgocIntDone = 0;

static int giSelMuxBackup = 0;
/*========================================================================================
	Implementation Group
========================================================================================*/

int ADC_PowerOn_Ax(void)
{

	// reg_exta_sel , reg_extb_sel
	//gVportRegAx->reg_exta_sel.reg_exta_sel = 0x2;
	//gVportRegAx->reg_exta_sel.reg_extb_sel = 0x2;

//	gVportRegAx->reg_swrst_exta.reg_swrst_exta = 0x0;
//	gVportRegAx->reg_swrst_exta.reg_swrst_extb = 0x0;

	gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 0;

	mdelay(5);

	gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 1;

	mdelay(5);

	gVportRegAx->reg_cvbspll_pdb.reg_ch3pll_pdb = 1;

	//gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 1;

	gVportRegAx->afe3ch_biaspdb.afe3ch_bpdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_gpdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_rpdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_vref_pdb = 1;

	gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 1;
	mdelay(5);
	gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 0;

	gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 1;
	mdelay(5);
	gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0;

	gVportRegAx->afe3ch_envolclp.afe3ch_volbypas = 0;
	gVportRegAx->afe3ch_envolclp.afe3ch_envolclp = 1;

//Enable Interrupt
	gVportRegAx->intr_vp2_enable.intr_vp2_enable = ADC_DETECTION_INTERRUPT_MASK | ADC_AOGC_INTERRUPT_MASK;
	return 0;

}

int ADC_PowerOff_Ax(void)
{
	// reg_exta_sel , reg_extb_sel
	//gVportRegAx->reg_exta_sel.reg_exta_sel = 0x0;
	//gVportRegAx->reg_exta_sel.reg_extb_sel = 0x0;

	gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_vref_pdb = 0;


	gVportRegAx->afe3ch_biaspdb.afe3ch_bpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_gpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_rpdb = 0;

	gVportRegAx->reg_cvbspll_pdb.reg_ch3pll_pdb = 0;

	gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 1;
	gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 1;

	gVportRegAx->afe3ch_envolclp.afe3ch_volbypas = 0;

	return 0;

}

int ADC_InitDaughterBoard_Ax(void)
{
	//ADC_PowerOn();

	//gVportRegAx->cst_extcoastsel.cst_postcoast = 0x1f;	//Macro vision pseudo Sync
	gVportRegAx->cst_extcoastsel.cst_precoast = 0xa;	//1006 by moon
	gVportRegAx->cst_extcoastsel.cst_postcoast = 0x1d;	//1006 by moon
//	gVportRegAx->ch3reg_hsdiff_thr.ch3reg_hsdiff_thr = 0x2;	// RGB shaking problem => H positon shift problem
	//gVportRegAx->intr_vp2_enable.intr_vp2_enable = 0xffffffff;
	//gVportRegAx->afe3ch_soglvl.afe3ch_soglvl = 0xa; // default value is 0xe 0617 by Jun
	//gVportRegAx->afe3ch_soglvl.afe3ch_soglvl = 0xb; // default value is 0xe 0708 by dws
	//gVportRegAx->afe3ch_soglvl.afe3ch_soglvl = 0xe; // for IFM (correct Vtotal read) , but has problem with 480i/576i pattern
	//gVportRegAx->afe3ch_soglvl.afe3ch_soglvl = 0x9; // 0906 bd-390 Transformer warning scene horizontal shaking.
	//gVportRegAx->afe3ch_soglvl.afe3ch_soglvl = 0xa;  // 0908 for Macrovision & LG BDP Warning Scene.
	gVportRegAx->afe3ch_soglvl.afe3ch_soglvl = 0xb; // Continuos Interrupt (0xa)

	//Added for TVX 1080P shaking problem
	//Removed (side effect on directv settop)
//	gVportRegAx->afe3ch_soglvl.afe3ch_sog_bw = 0x1; // default value is 0x0 0621 by Jun
	//gVportRegAx->afe3ch_soglvl.afe3ch_sog_bw = 0x3; // 0908 for Macrovision & LG BDP Warning Scene.
	//gVportRegAx->afe3ch_soglvl.afe3ch_sog_bw = 0x1; // 1005 by moon
	gVportRegAx->afe3ch_soglvl.afe3ch_sog_bw = 0x3; // 1102 by powerory

	//gVportRegAx->afe3ch_soglvl.afe3ch_sog_hys = 0x2; //0903 bd-390 Transformer warning scene horizontal shaking.
	//gVportRegAx->afe3ch_soglvl.afe3ch_sog_hys = 0x0; // 0908 for Macrovision & LG BDP Warning Scene.
	gVportRegAx->afe3ch_soglvl.afe3ch_sog_hys = 0x2; // 1005 by moon

	//gVportRegAx->ch3reg_size_detect_ctrl.ch3reg_size_detect_ctrl = 0x388D;
	//gVportRegAx->ch3reg_size_detect_ctrl.ch3reg_size_detect_ctrl = 0x3F4D; // For Component/RGB Interrupt Issue(old value: 0x388d) ====>>>> Component Detection Issue
	//gVportRegAx->ch3reg_size_detect_ctrl.ch3reg_size_detect_ctrl = 0x3FCD; // 0825 new value from powerory //SS BDP menu shaking...
	gVportRegAx->ch3reg_size_detect_ctrl.ch3reg_size_detect_ctrl = 0x3DCD; // 0913 for (time 221 to 222 no interrupt issue)

	//gVportRegAx->cst_hsselover.cst_hspolover = 0x1; // 0825 new value from powerory
	//gVportRegAx->cst_hsselover.cst_vspolover = 0x1; // 0825 new value from powerory
	//gVportRegAx->cst_hsselover.cst_hspoluser = 0x0; // 0825 new value from powerory
	//gVportRegAx->cst_hsselover.cst_vspoluser = 0x0; // 0825 new value from powerory

	ADC_PowerOn();

#ifdef	KDRV_CONFIG_PM
	if(gAdcPmInitial < 0)
	{
		gAdcPMData = (S_ADC_PM_DATA *)kmalloc(sizeof(S_ADC_PM_DATA) , GFP_KERNEL);
		gAdcPMData->giPmIndexValue = -1;
		gAdcPmInitial = 0;
	}
#endif
//	msleep(100);

	return 0;
}


int ADC_GetBoundaryInform_Ax(ST_EXTAU *inform)
{
	// get extau_up
	inform->extau_up = gVportRegAx->ch3reg_extau_up_result.ch3reg_extau_up_result;

	// get extau_down
	inform->extau_down = gVportRegAx->ch3reg_extau_up_result.ch3reg_extau_down_result;

	// get extau_left
#ifdef ADC_BOUNDARY_COMPENSATION
	if(gVportRegAx->ch3reg_extau_left_result.ch3reg_extau_left_result >= 1)
		inform->extau_left = gVportRegAx->ch3reg_extau_left_result.ch3reg_extau_left_result - 1 ;// powerory
	else
#endif
		inform->extau_left = gVportRegAx->ch3reg_extau_left_result.ch3reg_extau_left_result;// powerory

	// get extau_lest
#ifdef ADC_BOUNDARY_COMPENSATION
	if(gVportRegAx->ch3reg_extau_left_result.ch3reg_extau_right_result >= 1)
		inform->extau_right = gVportRegAx->ch3reg_extau_left_result.ch3reg_extau_right_result - 1 ;// powerory
	else
#endif
		inform->extau_right = gVportRegAx->ch3reg_extau_left_result.ch3reg_extau_right_result;// powerory
	//inform->extau_right = gVportRegAx->ch3reg_extau_left_result.ch3reg_extau_right_result;

	return 0;
}

int ADC_SetTAUInform_Ax(S_EXTAU_VALUE *value)
{
	// Height
	gVportRegAx->ch3reg_extau_down_offset.ch3reg_extau_down_offset = value->height_offset;

	// WIDTH
	gVportRegAx->ch3reg_extau_right_offset.ch3reg_extau_right_offset = value->width_offset;

	gVportRegAx->ch3reg_extau_ctrl.ch3reg_extau_ctrl = value->control;

	gVportRegAx->ch3reg_extau_level.ch3reg_extau_level = value->level;

	// For Proper Boundary Detection in case of PC RGB Horizontal Sync Crosstalk on RGB Signal, Adjusted left_offset.
	// Enlarge Value to 10 1019 for PC geforce 8400GS
	gVportRegAx->ch3reg_extau_left_offset.ch3reg_extau_left_offset = gVportRegAx->ch3reg_ro_hsout_width.ch3reg_ro_hsout_width + 10;//5; // by powerory 1013

	return 0;

}

int ADC_SetRGBPosition_Ax(S_RGB_POSITION *pos)
{
	gVportRegAx->ch3reg_ap__pos_b.ch3reg_ap__pos_b = pos->width_b;
	gVportRegAx->ch3reg_ap__pos_g.ch3reg_ap__pos_g = pos->width_g;
	gVportRegAx->ch3reg_ap__pos_r.ch3reg_ap__pos_r = pos->width_r;

	gVportRegAx->ch3reg_ap_ypos_b.ch3reg_ap_ypos_b = pos->height_b;
	gVportRegAx->ch3reg_ap_ypos_g.ch3reg_ap_ypos_g = pos->height_g;
	gVportRegAx->ch3reg_ap_ypos_r.ch3reg_ap_ypos_r = pos->height_r;

	return 0;
}

int ADC_GetRGBResult_Ax(S_RGB_OUT *result)
{
	result->out_b =  gVportRegAx->ch3reg_ap_out_b.ch3reg_ap_out_b;
	result->out_g =	 gVportRegAx->ch3reg_ap_out_g.ch3reg_ap_out_g;
	result->out_r =  gVportRegAx->ch3reg_ap_out_r.ch3reg_ap_out_r;

	return 0;
}

int ADC_GetRGBPosition_Ax(S_RGB_POSITION *pos)
{
	pos->width_b =  gVportRegAx->ch3reg_ap__pos_b.ch3reg_ap__pos_b;
	pos->width_g =	gVportRegAx->ch3reg_ap__pos_g.ch3reg_ap__pos_g;
	pos->width_r =  gVportRegAx->ch3reg_ap__pos_r.ch3reg_ap__pos_r;

	pos->height_b = gVportRegAx->ch3reg_ap_ypos_b.ch3reg_ap_ypos_b;
	pos->height_g = gVportRegAx->ch3reg_ap_ypos_g.ch3reg_ap_ypos_g;
	pos->height_r = gVportRegAx->ch3reg_ap_ypos_r.ch3reg_ap_ypos_r;

	return 0;
}

int ADC_SetRGBSize_Ax(S_RGB_SIZE *size)
{
	gVportRegAx->ch3reg_ap__pos_b.ch3reg_ap__size_b = size->width_b;
	gVportRegAx->ch3reg_ap__pos_g.ch3reg_ap__size_g = size->width_g;
	gVportRegAx->ch3reg_ap__pos_r.ch3reg_ap__size_r = size->width_r;

	gVportRegAx->ch3reg_ap_ypos_b.ch3reg_ap_ysize_b = size->height_b;
	gVportRegAx->ch3reg_ap_ypos_g.ch3reg_ap_ysize_g = size->height_g;
	gVportRegAx->ch3reg_ap_ypos_r.ch3reg_ap_ysize_r = size->height_r;

	return 0;
}

int ADC_GetSizeValue_Ax(S_VIDEO_SIZE_INFORM *size)
{
	size->ext_vsize = gVportRegAx->ch3reg_sd_e_t_vsize.ch3reg_sd_e_t_vsize;
	size->vd_vsize = gVportRegAx->ch3reg_sd_vd_vsize.ch3reg_sd_vd_vsize;
	size->vd_hsize = gVportRegAx->ch3reg_sd_vd_hsize.ch3reg_sd_vd_hsize;
	size->polarity = (gVportRegAx->cst_ro_hsdet.cst_ro_hspol<<1) | (gVportRegAx->cst_ro_vspol.cst_ro_vspol);

	return 0;

}

int ADC_SetPort_Ax(S_SEL_PORT *sel_port)
{

#ifdef	KDRV_CONFIG_PM
	memcpy((void *)&gAdcPMData->gstPmSelPortValue , (void *)sel_port , sizeof(S_SEL_PORT));
#endif
	//AFE_PRINT("ADC_SetPort \n");

	AFE_PRINT("ADC_SetPort : sel_port->port = [%d] \n", sel_port->port);

	gVportRegAx->afe3ch_selmux.afe3ch_selmux = sel_port->port;
	gVportRegAx->cst_selmux.cst_selmux = sel_port->port;

	gVportRegAx->cst_hsselover.cst_hsselover = sel_port->hsselover;
	gVportRegAx->cst_hsselover.cst_hsseluser = sel_port->hsseluser;
	gVportRegAx->cst_hsselover.cst_vsselover = sel_port->vsselover;
	gVportRegAx->cst_hsselover.cst_vsseluser = sel_port->vsseluser;
	gVportRegAx->cst_compover.cst_compover = sel_port->compover;
	gVportRegAx->cst_compover.cst_compuser = sel_port->compuser;

	return 0;
}

int ADC_GetSelectedPort_Ax(void)
{
	return	gVportRegAx->afe3ch_selmux.afe3ch_selmux;
}

unsigned int ADC_GetPhaseValue_Ax(void)
{
	return(gVportRegAx->llpll_shift.llpll_shift);

}

int ADC_SetPhaseValue_Ax(unsigned int phase)
{

//	AFE_PRINT("######### Set ADC Phase Register to [%d][0x%x]\n", phase,phase );
	gVportRegAx->llpll_shift.llpll_shift = phase;

	return 0;

}

unsigned int ADC_GetScanType_Ax(void)
{
	unsigned int ret=0;
	UINT8	count=0;

	for(;count<INTERLACE_CHECK_COUNT;count++)
	   ret += gVportRegAx->cst_ro_vspol.cst_ro_interlaced^1;

//	AFE_PRINT("* Interlace check count value = [%d]\n", ret);

	if(ret > (INTERLACE_CHECK_COUNT/2))
		return 1;
	else
		return 0;

}


int ADC_SetGainValue_Ax(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;

	gVportRegAx->afe3ch_gin_r.afe3ch_gin_r = stGain->R_Gain_Value;
	gVportRegAx->afe3ch_gin_g.afe3ch_gin_g = stGain->G_Gain_Value;
	gVportRegAx->afe3ch_gin_b.afe3ch_gin_b = stGain->B_Gain_Value;

	return ret;

}

int ADC_GetGainValue_Ax(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;

	stGain->R_Gain_Value = gVportRegAx->afe3ch_gin_r.afe3ch_gin_r;
	stGain->G_Gain_Value = gVportRegAx->afe3ch_gin_g.afe3ch_gin_g;
	stGain->B_Gain_Value = gVportRegAx->afe3ch_gin_b.afe3ch_gin_b;


	return ret;
}


int ADC_SetOffsetValue_Ax(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;

	gVportRegAx->afe3ch_oftin_r.afe3ch_oftin_r = stOffset->R_Offset_Value;
	gVportRegAx->afe3ch_oftin_g.afe3ch_oftin_g = stOffset->G_Offset_Value;
	gVportRegAx->afe3ch_oftin_b.afe3ch_oftin_b = stOffset->B_Offset_Value;

	return ret;

}

int ADC_GetOffsetValue_Ax(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;

	stOffset->R_Offset_Value = gVportRegAx->afe3ch_oftin_r.afe3ch_oftin_r;
	stOffset->G_Offset_Value = gVportRegAx->afe3ch_oftin_g.afe3ch_oftin_g;
	stOffset->B_Offset_Value = gVportRegAx->afe3ch_oftin_b.afe3ch_oftin_b;

	return ret;
}


/**
 *
 * setting the LLPLL by index value which came from ADC_IFM_SearchTable
 *
 * @param	S_VIDEO_SIZE_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetInputVideo_Ax(int iIndex)
{

	int i;
	UINT32 hsout_width =0;


#ifdef	KDRV_CONFIG_PM
	gAdcPMData->giPmIndexValue = iIndex;
#endif

	//LLPLL_div_max
	gVportRegAx->llpll_div_max.llpll_div_max = LLPLL_div_max[iIndex];

	//LLPLL_counter_max
	gVportRegAx->llpll_div_max.llpll_counter_max = LLPLL_counter_max[iIndex];

	//LLPLL_shift 일단 0으로 초기화
	gVportRegAx->llpll_shift.llpll_shift = 0;		// should be set the phase later

	//LLPLL_dco_max
	gVportRegAx->llpll_dco_max.llpll_dco_max = LLPLL_dco_max[iIndex];

	//LLPLL_dco_min
	gVportRegAx->llpll_dco_max.llpll_dco_min = LLPLL_dco_min[iIndex];

	//LLPLL_coarse_scale
	gVportRegAx->llpll_coarse_scale.llpll_coarse_scale = LLPLL_coarse_scale[iIndex];

	//LLPLL_g1_nom
	gVportRegAx->llpll_g1_nom.llpll_g1_nom = LLPLL_g1_nom[iIndex];

	//LLPLL_g2_nom
	gVportRegAx->llpll_g1_nom.llpll_g2_nom = LLPLL_g2_nom[iIndex];

	//LLPLL_g3_p_nom
	gVportRegAx->llpll_g1_nom.llpll_g3_p_nom = LLPLL_g3_p_nom[iIndex];

	//LLPLL_g3_n_nom
	gVportRegAx->llpll_g1_nom.llpll_g3_n_nom = LLPLL_g3_n_nom[iIndex];

	//LLPLL_g1_fine
	gVportRegAx->llpll_g1_fine.llpll_g1_fine = LLPLL_g1_fine[iIndex];

	//LLPLL_g2_fine
	gVportRegAx->llpll_g1_fine.llpll_g2_fine = LLPLL_g2_fine[iIndex];

	//LLPLL_g3_p_fine
	gVportRegAx->llpll_g1_fine.llpll_g3_p_fine = LLPLL_g3_p_fine[iIndex];

	//LLPLL_g3_n_fine
	gVportRegAx->llpll_g1_fine.llpll_g3_n_fine = LLPLL_g3_n_fine[iIndex];

	for( i = 0 ; i < 100 ; i++)
	{
		if(gVportRegAx->llpll_filter_status.llpll_filter_status == 3) break;
		mdelay(5);
	}
	if(i==100)
	{
		AFE_PRINT("!!!!!!!!!!!! LLPLL UNLOCK !!!!!!!!!!!!!!!!!\n");
	//	ADC_Reset_LLPLL();
	}
#if 0
	for( i = 0 ; i < 100 ; i++)
	{
		if(gVportRegAx->llpll_filter_status.llpll_filter_status != 3)
			AFE_PRINT("!!!!!!!!!!!! %d LLPLL UNLOCK !!!!!!!!!!!!!!!!!\n", i );
		mdelay(5);
	}
#endif
/*
	gVportRegAx->afe3ch_biaspdb.afe3ch_rpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_gpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_bpdb = 0;

	mdelay(5);

	gVportRegAx->afe3ch_biaspdb.afe3ch_rpdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_gpdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_bpdb = 1;

	mdelay(5);
*/
	if(iIndex == 306) // for Component 576i 50Hz field indication should be inverted
		gVportRegAx->ch3reg_fld_inv.ch3reg_fld_inv = 0x1;
	else
		gVportRegAx->ch3reg_fld_inv.ch3reg_fld_inv = 0x0;

#if 0
	mdelay(100); //tmp for test

	//added 0716 for horizontal shaking problem
	for(i=0; i<4; i++)
	{
		hsout_width += gVportRegAx->ch3reg_ro_hsout_width.ch3reg_ro_hsout_width;
		//AFE_PRINT("hsout_width read = [%d]\n", hsout_width);
	}

	hsout_width = hsout_width >>2;

	AFE_PRINT("calculated hsout_width = [%d]\n", hsout_width);

	gVportRegAx->ch3reg_hsout_width_sel.ch3reg_hsout_width = hsout_width;
#else

	hsout_width = HS_Out_Width[iIndex];

//	AFE_PRINT("Table hsout_width = [%d]\n", hsout_width);

	gVportRegAx->ch3reg_hsout_width_sel.ch3reg_hsout_width = hsout_width;
#endif

	if(i<100)
		return 0;
	else
		return -1;
}



int ADC_AGOCInit_Ax(void)
{

	// clamp off
	gVportRegAx->afe3ch_envolclp.afe3ch_envolclp = 0;

	// aogc conter set as max
	gVportRegAx->ch3reg_aogc_th.ch3reg_aogc_counter_ma = 0xFFFE;
	gVportRegAx->ch3reg_aogc_th.ch3reg_aogc_sample_count = 6;	// 0 : 16, 1 : 32

	// aoc, agc sel register
	gVportRegAx->ch3reg_agc_ire.ch3reg_agc_ire = 3;
	gVportRegAx->ch3reg_agc_ire.ch3reg_aoc_ire = 4;


	return 0;
}

/**
 *
 * setting ADC IRE test enable/disable
 *
 * @param	0 : inactive , 1 : active
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetIRETestMode_Ax(int sel , E_AGC_IRE mode)
{
	int ret = 0;


	if(sel)	// IRE Test mode inactive
	{
		ADC_AGOCInit();
/* powerory : before revision */
#ifdef BEFORE_REVISION
		if(mode == IRE0_RGB)
			gVportRegAx->afe3ch_r_ph.afe3ch_r_ph = 0x7;//0x6;//0x3;	// modified parameter by jun's mail(08.02)
		else
			gVportRegAx->afe3ch_r_ph.afe3ch_r_ph = 0x7;//0x4;//0x3;

		mdelay(100);
		gVportRegAx->afe3ch_r_ph.afe3ch_r_ph = 0x3;//0x4;//0x3;
		giSelMuxBackup = gVportRegAx->afe3ch_selmux.afe3ch_selmux;
		gVportRegAx->afe3ch_selmux.afe3ch_selmux = 0x3;
//~powerory
#else
// powerory : After Revision
		giSelMuxBackup = gVportRegAx->afe3ch_selmux.afe3ch_selmux;
		gVportRegAx->afe3ch_r_ph.afe3ch_r_ph = 0x04 | giSelMuxBackup;//0x3;
		mdelay(100);
		gVportRegAx->afe3ch_r_ph.afe3ch_r_ph = giSelMuxBackup;//0x4;//0x3;
		//gVportRegAx->afe3ch_selmux.afe3ch_selmux = 0x3;
//~powerory
#endif
		// aogc mode set as sampling only
		gVportRegAx->ch3reg_aogc_mode.ch3reg_aogc_mode = 0x0;
		// aogc clk
		gVportRegAx->reg_clken_aoc.reg_clken_aoc = 1;

		// aogc test start
		gVportRegAx->aogc_enable.aogc_enable = 1;

		mdelay(20);

	}
	else	// IRE Test mode active
	{
		gVportRegAx->afe3ch_selmux.afe3ch_selmux = giSelMuxBackup;

		gVportRegAx->afe3ch_r_ph.afe3ch_r_ph = 0x0;

		// clamp on
		gVportRegAx->afe3ch_envolclp.afe3ch_envolclp = 1;

		// aogc clk
		gVportRegAx->reg_clken_aoc.reg_clken_aoc = 0;
		// aogc mode set as sampling only
		gVportRegAx->ch3reg_aogc_mode.ch3reg_aogc_mode = 0x0;

		// aogc test stop
		gVportRegAx->aogc_enable.aogc_enable = 0;
	}
	return ret;
}

/**
 *
 * get RGB average value from register
 *
 * @param	ST_GAIN_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/

static int ADC_GetIRELevel( S_GAIN_TARGET *gain)
{
	int ret = 0;
	unsigned int temp_r, temp_g,temp_b;

	temp_r = gVportRegAx->ch3reg_ro_avg_r.ch3reg_ro_avg_r;
	temp_g = gVportRegAx->ch3reg_ro_avg_g.ch3reg_ro_avg_g;
	temp_b = gVportRegAx->ch3reg_ro_avg_b.ch3reg_ro_avg_b;


	//120103 wonsik.do : removed floating point in kernel driver
//	gain->gain_R = (float)( (temp_r&0xffff00) >> 8) + (float)(temp_r&0xff)/256.0;
//	gain->gain_G = (float)( (temp_g&0xffff00) >> 8) + (float)(temp_g&0xff)/256.0;
//	gain->gain_B = (float)( (temp_b&0xffff00) >> 8) + (float)(temp_b&0xff)/256.0;
	gain->gain_R = temp_r;
	gain->gain_G = temp_g;
	gain->gain_B = temp_b;

	AFE_PRINT("IRE level 1 : R[%d] , G[%d] , B[%d] \n",(UINT32)gain->gain_R, (UINT32)gain->gain_G , (UINT32)gain->gain_B );

	return ret;

}


/**
 *
 * set IRE value for R,G,B
 *
 * @param	S_VIDEO_SIZE_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetIRELevel_Ax(E_AGC_IRE ire , S_GAIN_TARGET *gain)
{
	int ret = 0;
	unsigned short target_ire;

	AFE_PRINT("IRE level set %d\n",ire);

	// set IRE
	gVportRegAx->ch3reg_adc_ire_test.ch3reg_adc_ire_sel_man = ire;


	switch(ire)
	{
		case 	IRE0:
		case	IRE0_RGB:
		case	IRE0_COMP:
			target_ire = 0;
		break;
		case 	IRE73:
			target_ire = 73;
		break;
		case 	IRE950:
			target_ire = 950;
		break;
		case 	IRE1023:
			target_ire = 1023;
		break;

		default:
			target_ire = 0;
			AFE_PRINT("WARNING : Invalid E_AGC_IRE type\n");
		break;

	}


	// set target value

	if(ire == IRE0_COMP){
		gVportRegAx->ch3reg_aoc_r_compare.ch3reg_aoc_r_compare = 512;
		gVportRegAx->ch3reg_aoc_g_compare.ch3reg_aoc_g_compare = target_ire;
		gVportRegAx->ch3reg_aoc_b_compare.ch3reg_aoc_b_compare = 512;
	}
	else
	{
		gVportRegAx->ch3reg_aoc_r_compare.ch3reg_aoc_r_compare = target_ire;
		gVportRegAx->ch3reg_aoc_g_compare.ch3reg_aoc_g_compare = target_ire;
		gVportRegAx->ch3reg_aoc_b_compare.ch3reg_aoc_b_compare = target_ire;
	}

	giAgocIntDone = 0;		// prevent invoking the interrupt before on time
	ADC_SetIRETestMode(1 , ire);
//	msleep(500);

#if 1
	wait_event_interruptible_timeout(agoc_done , giAgocIntDone != 0 , msecs_to_jiffies(300) );

	giAgocIntDone = 0;
#endif

	ADC_GetIRELevel(gain);
	ADC_SetIRETestMode(0 , ire);
	return ret;
}

int ADC_SetAgocWakeup_Ax(void)
{

	giAgocIntDone = 1;
	wake_up_interruptible(&agoc_done);

	return 0;
}

int ADC_GetGainRGB_Ax( ST_GAIN_INFORM *gain)
{
	gain->gain_R = gVportRegAx->afe3ch_gin_r.afe3ch_gin_r;
	gain->gain_G = gVportRegAx->afe3ch_gin_g.afe3ch_gin_g;
	gain->gain_B = gVportRegAx->afe3ch_gin_b.afe3ch_gin_b;

	return 0;

}

int ADC_SetGainRGB_Ax( ST_GAIN_INFORM *gain)
{
	gVportRegAx->afe3ch_gin_r.afe3ch_gin_r = gain->gain_R;
	gVportRegAx->afe3ch_gin_g.afe3ch_gin_g = gain->gain_G;
	gVportRegAx->afe3ch_gin_b.afe3ch_gin_b = gain->gain_B;

	return 0;
}


int ADC_GetOffsetRGB_Ax( S_OFFSET_INFORM *offset)
{
	offset->offset_R = gVportRegAx->afe3ch_oftin_r.afe3ch_oftin_r;
	offset->offset_G = gVportRegAx->afe3ch_oftin_g.afe3ch_oftin_g;
	offset->offset_B = gVportRegAx->afe3ch_oftin_b.afe3ch_oftin_b;

	return 0;

}

int ADC_SetOffsetRGB_Ax( S_OFFSET_INFORM *offset)
{
	gVportRegAx->afe3ch_oftin_r.afe3ch_oftin_r = offset->offset_R;
	gVportRegAx->afe3ch_oftin_g.afe3ch_oftin_g = offset->offset_G;
	gVportRegAx->afe3ch_oftin_b.afe3ch_oftin_b = offset->offset_B;

	return 0;
}


int ADC_GetSumRGB_Ax(S_RGB_SUM *sum)
{

	sum->sum_R = gVportRegAx->ch3reg_ap_sum_r.ch3reg_ap_sum_r;

	sum->sum_G = gVportRegAx->ch3reg_ap_sum_g.ch3reg_ap_sum_g;

	sum->sum_B = gVportRegAx->ch3reg_ap_sum_b.ch3reg_ap_sum_b;

	return 0;
}

/**
 *
 * Enable SCART RGB input on component 1 port for EU model
 *
 * @param	BOOLEAN scart_enable(TRUE:Enable SCART RGB, FALSE:component mode)
 *
*/
int ADC_Enable_Scart_Mode_Ax(BOOLEAN scart_enable)
{
	UINT32	VP2_Intr_Enable_Mask = 0xFFFFFBF3; // Disable Resolution change & SOY0 detection interrupt.
	static UINT32 	VP2_Intr_Enable_Save = 0;

	AFE_PRINT("%s entered with value [%d]\n", __func__, scart_enable);

	if (scart_enable == TRUE)
	{
		//Interrupt Disable for component1
		VP2_Intr_Enable_Save = gVportRegAx->intr_vp2_enable.intr_vp2_enable;
		gVportRegAx->intr_vp2_enable.intr_vp2_enable = VP2_Intr_Enable_Save & VP2_Intr_Enable_Mask;

		gVportRegAx->afe3ch_selmux.afe3ch_selmux = 0x0; // select component 1 port for ADC
		gVportRegAx->cst_selmux.cst_selmux = 0x0; // select component 1 port for ADC
		gVportRegAx->afe3ch_sel_ck.afe3ch_sel_ck = 0x1; // select clock from CVD

		ADC_SetType(LX_ADC_INPUT_SOURCE_RGB_SCART); // for SCART RGB set port type to RGB
	}
	else
	{
		// No need to set interrupt enable for the scart input ???
		// Re-Enable Resolution change interrupt & SOY0 interrupt
		if(VP2_Intr_Enable_Save)
			gVportRegAx->intr_vp2_enable.intr_vp2_enable = VP2_Intr_Enable_Save;

		// Port selection should be called from DDI
		//gVportRegAx->afe3ch_selmux.afe3ch_selmux = 0x0; // select component 1 port for ADC
		gVportRegAx->afe3ch_sel_ck.afe3ch_sel_ck = 0x0; // select clock from LLPLL
	}
	return 0;
}

// should be removed
int ADC_SetHorTotal_Ax(unsigned short value)
{
	return 0;
}


#if 0
int ADC_SetVerPos_Ax(unsigned short pos)
{
	return 0;
}

int ADC_SetHorPos_Ax(unsigned short pos)
{
	return 0;

}

int ADC_GetI2CData_Ax(unsigned int sub , unsigned int addr , unsigned int size )
{
	return 0;
}
int ADC_SetI2CData_Ax(unsigned int sub , unsigned int addr , unsigned int size , unsigned int value)
{
	return 0;
}

int ADC_SetDE_Ax(int iIndex , unsigned int extau_up , unsigned int extau_left)
{

	return 0;
}
#endif

void ADC_Vport_ALL_Power_Down_Ax(void)
{

	/* HDMI PHY Power Down Setting */
	gVportRegAx->reg_swrst_linksys.reg_swrst_linksys = 0;
	gVportRegAx->reg_swrst_p3.reg_swrst_p3 = 0;
	gVportRegAx->reg_swrst_p3.reg_swrst_p1 = 0;
	gVportRegAx->reg_swrst_exta.reg_swrst_exta = 0;
	gVportRegAx->reg_swrst_exta.reg_swrst_extb = 0;

	/*CVBS ADC Power Down Setting */
	gVportRegAx->cvbs_pdbm.cvbs_pdbm = 0x0;
	gVportRegAx->cvbs_pdbm.cvbs1_pdb = 0x0;
	gVportRegAx->cvbs_pdbm.cvbs2_pdb = 0x0;

	/* VDAC & BUFFER Power Down Setting */
	  gVportRegAx->vdac_pdb.vdac_pdb = 0x0;
	  gVportRegAx->vdac_pdb.vdicnt = 0x3; //dws
	  gVportRegAx->vdac_pdb.buf_pdb1 = 0x0;
	  gVportRegAx->vdac_pdb.buf_pdb2 = 0x0;

	  /* AFE3CH Power Down Setting */
	//gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 0x0;
//	gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_vref_pdb = 0;


	gVportRegAx->afe3ch_biaspdb.afe3ch_bpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_gpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_rpdb = 0;

	 /* LLPLL PDB */
	 gVportRegAx->reg_cvbspll_pdb.reg_cvbspll_pdb = 0;
	 gVportRegAx->reg_cvbspll_pdb.reg_ch3pll_pdb = 0;

	 /* LLPLL/CST SWRST & 3CH_DIG SWRST */
	 gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 0x1;
	 gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0x1;
}

int ADC_Channel_Power_Control_Ax(int vref_pdb, int bpdb, int gpdb, int rpdb)
{
	return 0;
}

int ADC_SetType_Ax(LX_AFE_ADC_INPUT_SOURCE_TYPE_T sel_type)
{

	//AFE_PRINT("ADC_SetType \n");
	gADC_type = sel_type;

	if(sel_type == LX_ADC_INPUT_SOURCE_YPBPR)
	{
		AFE_PRINT("Component Input Mode\n");
		gVportRegAx->afe3ch_bmidsel.afe3ch_bmidsel = 0x1;
		gVportRegAx->afe3ch_bmidsel.afe3ch_gmidsel = 0x0;
		gVportRegAx->afe3ch_bmidsel.afe3ch_rmidsel = 0x1;

		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_base = 0x3;	// SC jung wrote
		gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_base = 0x2;	// 0908 move to backporch

		_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 0 , 0x05);	// 0908 move to backporch
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_width = 0xa;
		//_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 8 , 0x2c);	// 0908 move to backporch
		_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 8 , 0x40);	// 1102
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_place = 0x5;

		//added 0716
		gVportRegAx->ch3reg_hsout_width_sel.ch3reg_hsout_width_sel = 0x0; // Select Hsync output sync width from hsout_width register
		gVportRegAx->ch3reg_fld_inv.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block

		//0906
		gVportRegAx->ch3reg_hsdiff_thr.ch3reg_hsdiff_thr = 0x0;	// RGB shaking problem => H positon shift problem
	}
	else
	{ AFE_PRINT("RGB Input Mode\n");
		gVportRegAx->afe3ch_bmidsel.afe3ch_bmidsel = 0x0;
		gVportRegAx->afe3ch_bmidsel.afe3ch_gmidsel = 0x0;
		gVportRegAx->afe3ch_bmidsel.afe3ch_rmidsel = 0x0;

		gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_base = 0x2;	// SC jung wrote

		//_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 0 , 0x14);
		_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 0 , 0x5);	//1102
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_width = 0x14;
		//_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 8 , 0x9);
		_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 8 , 0x10); //1102
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_place = 0x9;

		//added 0716
		//gVportRegAx->ch3reg_hsout_width_sel.ch3reg_hsout_width_sel = 0x1; // Select Auto Hsync output sync width

		// for horizontal sync re-shape in ADC
		gVportRegAx->ch3reg_hsout_width_sel.ch3reg_hsout_width_sel = 0x0; // Select Hsync output sync width from hsout_width register
		gVportRegAx->ch3reg_fld_inv.ch3reg_hsync_sel = 0x0; // Select internal Hsync for digital block
//		gVportRegAx->ch3reg_fld_inv.ch3reg_hsync_sel = 0x1; // powerory  <== CAN NOT CHANGE HSIZE BY 1 STEP

		//0906
		gVportRegAx->ch3reg_hsdiff_thr.ch3reg_hsdiff_thr = 0x2;	// RGB shaking problem => H positon shift problem
	}

	return 0;
}

int ADC_Set_LLPLL_Ax(UINT32 arg)
{

	if(arg >= PARAM_NUM)
		return -1;

		AFE_PRINT("Setting LLPLL to Index Number [%d]\n", arg);

	ADC_SetInputVideo(arg);

	return 0;

}

int ADC_SetClock_Ax(int iIndex, SINT16 value)
{
	UINT16 clock_value;
//	AFE_PRINT("%s entered\n", __func__);
	//LLPLL_counter_max
	clock_value = LLPLL_counter_max[iIndex];

//	AFE_PRINT("Clock Value =[%d +(%d)], \n", clock_value, value);

	// No Need to Write Same Value
	/*
	if(gVportRegAx->llpll_div_max.llpll_counter_max == (clock_value + value))
		return 0;
		*/

#ifdef	KDRV_CONFIG_PM
	gAdcPMData->giPmClockValue = value;
#endif
	clock_value += value;
	ADC_Ext_Clamp(1);
	mdelay(10);
	gVportRegAx->llpll_div_max.llpll_counter_max = clock_value;
	mdelay(50);
	ADC_Ext_Clamp(0);

	return 0;
}

int ADC_Reset_Digital_Ax(void)
{
	AFE_PRINT("%s entered\n", __func__);
	gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0x1;
	mdelay(5);
	gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0x0;

	return 0;
}

int ADC_Reset_Digital_24MHZ_Ax(void)
{
	AFE_PRINT("%s entered. This function empty for this chipset[L8Ax].\n", __func__);

	return 0;
}

int ADC_Reset_LLPLL_Ax(void)
{
	AFE_PRINT("%s entered\n", __func__);
	gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 0x1;
	mdelay(5);
	gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 0x0;

	return 0;
}

int ADC_Sync_Polarity_Override_Ax(int set)
{
	AFE_PRINT("%s entered\n", __func__);
	if(set)
	{
		gVportRegAx->cst_hsselover.cst_hspolover = 0x1; // 0825 new value from powerory
		gVportRegAx->cst_hsselover.cst_vspolover = 0x1; // 0825 new value from powerory
		gVportRegAx->cst_hsselover.cst_hspoluser = 0x1; // 0825 new value from powerory
		gVportRegAx->cst_hsselover.cst_vspoluser = 0x1; // 0825 new value from powerory
	}
	else
	{
		gVportRegAx->cst_hsselover.cst_hspolover = 0x0; // 0825 new value from powerory
		gVportRegAx->cst_hsselover.cst_vspolover = 0x0; // 0825 new value from powerory
//		gVportRegAx->cst_hsselover.cst_hspoluser = 0x0; // 0825 new value from powerory
//		gVportRegAx->cst_hsselover.cst_vspoluser = 0x0; // 0825 new value from powerory
	}

	return 0;
}



int ADC_Check_Sync_Exist_Ax(void)
{

	if(((gVportRegAx->afe3ch_selmux.afe3ch_selmux == 0x0)&&(gVportRegAx->cst_ro_hsdet.cst_ro_soy0det == 0)) ||	\
		((gVportRegAx->afe3ch_selmux.afe3ch_selmux == 0x1)&&(gVportRegAx->cst_ro_hsdet.cst_ro_soy1det == 0)) ||	\
		(((gVportRegAx->afe3ch_selmux.afe3ch_selmux & 0x2) == 0x2)&&(gVportRegAx->cst_ro_hsdet.cst_ro_sogdet == 0)&&(gVportRegAx->cst_ro_hsdet.cst_ro_hsdet == 0)&&(gVportRegAx->cst_ro_hsdet.cst_ro_vsdet == 0)))
	{
		return -1;
	}
	return 0;
}

int ADC_Enable_Detection_Interrupt_Ax(int enable)
{
	AFE_PRINT("%s entered\n", __func__);
	if(enable)
		gVportRegAx->intr_vp2_enable.intr_vp2_enable = gVportRegAx->intr_vp2_enable.intr_vp2_enable | ADC_DETECTION_INTERRUPT_MASK;
	else
		gVportRegAx->intr_vp2_enable.intr_vp2_enable = gVportRegAx->intr_vp2_enable.intr_vp2_enable & (~ADC_DETECTION_INTERRUPT_MASK);

	return 0;
}

int ADC_Enable_AOGC_Interrupt_Ax(int enable)
{
	AFE_PRINT("%s entered\n", __func__);
	if(enable)
		gVportRegAx->intr_vp2_enable.intr_vp2_enable = gVportRegAx->intr_vp2_enable.intr_vp2_enable | ADC_AOGC_INTERRUPT_MASK;
	else
		gVportRegAx->intr_vp2_enable.intr_vp2_enable = gVportRegAx->intr_vp2_enable.intr_vp2_enable & (~ADC_AOGC_INTERRUPT_MASK);

	return 0;
}

int ADC_Mute_Enable_Ax(int enable)
{
	AFE_PRINT("%s entered [%d]\n", __func__, enable);
	if(enable)
	{
		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
			gVportRegAx->ch3reg_fld_inv.ch3reg_signedout = 5;
		gVportRegAx->afe3ch_r_ph.afe3ch_r_ph =0x3;
		gVportRegAx->ch3reg_adc_ire_test.ch3reg_adc_ire_test = 0x1;
	}
	else
	{
		gVportRegAx->ch3reg_fld_inv.ch3reg_signedout = 0;
		gVportRegAx->afe3ch_r_ph.afe3ch_r_ph =0x0;
		gVportRegAx->ch3reg_adc_ire_test.ch3reg_adc_ire_test = 0x0;
	}

	return 0;
}

int ADC_Power_OnOff_Ax(void)
{
	AFE_PRINT("%s entered\n", __func__);
	gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_vref_pdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_bpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_gpdb = 0;
	gVportRegAx->afe3ch_biaspdb.afe3ch_rpdb = 0;

	//gVportRegAx->ch3reg_extclampsel.ch3reg_extclampsel = 1;
	//gVportRegAx->ch3reg_extclampsel.ch3reg_clamppolover = 1;
	//gVportRegAx->ch3reg_extclampsel.ch3reg_clamppoluser = 1;

	gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 1;

	mdelay(5);

	gVportRegAx->afe3ch_biaspdb.afe3ch_vref_pdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_bpdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_gpdb = 1;
	gVportRegAx->afe3ch_biaspdb.afe3ch_rpdb = 1;

	gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 1;
	mdelay(5);
	gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 0;

	return 0;
}

int ADC_Ext_Clamp_Ax(int sel_ext)
{
	//AFE_PRINT("%s entered [%d]\n", __func__, sel_ext);
	if(sel_ext)
	{
		gVportRegAx->afe3ch_envolclp.afe3ch_envolclp = 0;
		// powerory
		//gVportRegAx->ch3reg_extclampsel.ch3reg_extclampsel = 1;
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamppolover = 1;
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamppoluser = 1;
		//~powerory

		if(gADC_type != LX_ADC_INPUT_SOURCE_YPBPR)	// 1019 Workaround for CVI Vsync Unstable on component input(VENICE 1080i)
			gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0x1;
	}
	else
	{
		// powerory
		//gVportRegAx->ch3reg_extclampsel.ch3reg_extclampsel = 0;
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamppolover = 0;
		//gVportRegAx->ch3reg_extclampsel.ch3reg_clamppoluser = 0;
		//~powerory
		gVportRegAx->afe3ch_envolclp.afe3ch_envolclp = 1;

		if(gADC_type != LX_ADC_INPUT_SOURCE_YPBPR)
		{
			gVportRegAx->ch3reg_hsoutover.ch3reg_hsoutover = 1;
			gVportRegAx->ch3reg_hsoutover.ch3reg_hsoutcnt = gVportRegAx->llpll_div_max.llpll_counter_max - 1;

			gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0x0;// 1019 Workaround for CVI Vsync Unstable on component input(VENICE 1080i)

		}
		else
			gVportRegAx->ch3reg_hsoutover.ch3reg_hsoutover = 0;

//		gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0x0;// 1019 Workaround for CVI Vsync Unstable on component input(VENICE 1080i)

	}

	return 0;
}

int ADC_Set_Clamp_Ax(UINT8 clamp_base, UINT8 clamp_place, UINT8 clamp_width)
{
	if(clamp_base)
		gVportRegAx->ch3reg_extclampsel.ch3reg_clamp_base = clamp_base;

	if(clamp_place)
		_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 8 , clamp_place);

	if(clamp_width)
		_iow(&gVportRegAx->ch3reg_extclampsel , 8 , 0 , clamp_width);

	return 0;
}

#ifdef	KDRV_CONFIG_PM
int ADC_RunSuspend_Ax(void)
{
	// confirm initialization done
	if(gVportRegAx == NULL)
		return -1;

	// TAU inform
	gAdcPMData->gstPmExtauValue.height_offset = gVportRegAx->ch3reg_extau_down_offset.ch3reg_extau_down_offset;
	gAdcPMData->gstPmExtauValue.width_offset = gVportRegAx->ch3reg_extau_right_offset.ch3reg_extau_right_offset;
	gAdcPMData->gstPmExtauValue.width_offset = gVportRegAx->ch3reg_extau_ctrl.ch3reg_extau_ctrl;

	//
	ADC_GetRGBPosition(&gAdcPMData->gstPmPositionValue);
	// phase
	gAdcPMData->guiPmPhaseValue = ADC_GetPhaseValue();

	ADC_GetGainValue(&gAdcPMData->gstPmGainValue);

	ADC_GetOffsetValue(&gAdcPMData->gstPmOffsetValue);


	gAdcPMData->guiPmInterruptStatus = gVportRegAx->intr_vp2_enable.intr_vp2_enable;

	ADC_Vport_ALL_Power_Down();

	return 0;
}

int ADC_RunResume_Ax(void)
{

	ADC_InitDaughterBoard();

	if(gAdcPMData->gstPmSelPortValue.port == 0xfffffff)
		return 0;



	gVportRegAx->ch3reg_extau_down_offset.ch3reg_extau_down_offset = gAdcPMData->gstPmExtauValue.height_offset;
	gVportRegAx->ch3reg_extau_right_offset.ch3reg_extau_right_offset = gAdcPMData->gstPmExtauValue.width_offset;
	gVportRegAx->ch3reg_extau_ctrl.ch3reg_extau_ctrl = gAdcPMData->gstPmExtauValue.width_offset;

	gVportRegAx->intr_vp2_enable.intr_vp2_enable = gAdcPMData->guiPmInterruptStatus;

	if(gAdcPMData->giPmIndexValue < 0)
		return 0;

	ADC_SetRGBPosition(&gAdcPMData->gstPmPositionValue);

	ADC_SetPhaseValue(gAdcPMData->guiPmPhaseValue);

	ADC_SetGainValue(&gAdcPMData->gstPmGainValue);

	ADC_SetOffsetValue(&gAdcPMData->gstPmOffsetValue);

	if(gAdcPMData->giPmIndexValue >= 0)
	{
		ADC_SetInputVideo(gAdcPMData->giPmIndexValue);
		ADC_SetClock(gAdcPMData->giPmIndexValue ,gAdcPMData->giPmClockValue);
	}



	return 0;
}
#endif


/**
 *
 * ADC_GetPCmodeInfo : Return current mode index
 *
 * @param	struct work_struct
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_GetPCModeInfo_Ax(LX_AFE_PCMODE_INFO_T *stPCModeInfo)
{
	UINT16	Vline, Vlowline;
   if(gResolution >= 0)
   {
	   memset((void *)stPCModeInfo, 0, sizeof(LX_AFE_PCMODE_INFO_T));
	   stPCModeInfo->u8_Mode_Idx = gResolution;
	   stPCModeInfo->u16_HSyncWidth = gVportRegAx->ch3reg_ro_hsout_width.ch3reg_ro_hsout_width;
	   Vline = gVportRegAx->cst_ro_vsprdref.cst_ro_vsprdref;
	   Vlowline = gVportRegAx->cst_ro_vsprdref.cst_ro_vslowref;
		stPCModeInfo->u8_UI_Phase = ADC_Phase_Convert_to_UI_Value(ADC_GetPhaseValue(), gResolution);
	   if(Vlowline > (Vline>>1)) // Active High VSync Pulse
		   stPCModeInfo->u16_VSyncWidth = Vline - Vlowline;
	   else
		   stPCModeInfo->u16_VSyncWidth = Vlowline;

	   return 0;
   }
   else
	   return -1;
}

int ADC_Get_LLPLL_Filter_Status_Ax(void)
{
	return gVportRegAx->llpll_filter_status.llpll_filter_status;
}

int ADC_Set_Coast_Ax(UINT8 precoast, UINT8 postcoast)
{
	gVportRegAx->cst_extcoastsel.cst_precoast = precoast;
	gVportRegAx->cst_extcoastsel.cst_postcoast = postcoast;

	return 0;
}

int ADC_Set_Hsync_Sel_Ax(UINT8 select)
{
	if(select > 0)
		gVportRegAx->ch3reg_fld_inv.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block
	else
		gVportRegAx->ch3reg_fld_inv.ch3reg_hsync_sel = 0x0;

	return 0;
}

int ADC_CVI_Sync_Exist_Ax(void)
{
	return 0;
}

int ADC_Set_SOG_Level_Ax(UINT32 level)
{
	if(level > 0x1F)
		return -1;

	gVportRegAx->afe3ch_soglvl.afe3ch_soglvl = level;

	AFE_PRINT("Set SOG Level = [0x%x]\n", level);

	return 0;
}
int ADC_Get_SOG_Level_Ax(UINT32 *level)
{
	return 0;
}
int ADC_Check_Htotal_Diff_Ax(UINT8 count, int Kadp_Index)
{
	return 0;
}

int ADC_Read_Pixel_Value_Ax(LX_AFE_ADC_PIXEL_VALUE_T *pixel_value_t)
{
	int ret = 0;
	return ret;
}
int ADC_Green_Mid_Level_Select_Ax(UINT8 value)
{
	return 0;
}
int ADC_Read_VSync_Width_Ax(UINT32 *pvs_width)
{
	return 0;
}
int ADC_Read_HSync_Width_Ax(UINT32 *pvs_width)
{
	return 0;
}
