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
#include <linux/sched.h>
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


#include "vport_reg_B0.h"
//#include "vport_reg.h"
#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"
#include "afe_drv.h"

#include "../de/l8/de_cvi_reg_l8.h"
/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define INTERLACE_CHECK_COUNT 7

//#define ADC_DETECTION_INTERRUPT_MASK	0x0000068A
//#define ADC_DETECTION_INTERRUPT_MASK	0x00000E8A
#define ADC_AOGC_INTERRUPT_MASK			0x0000C000
//#define ADC_BOUNDARY_COMPENSATION

//#define BEFORE_REVISION
#define VPORT_REG_DIRECT_ACCESS 0
//#define L8_RGB_FEEDBACK_HSYNC_WORKAROUND
#define L8_RGB_FEEDBACK_HSYNC_WORKAROUND_2
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
extern CHA_CVI_REG_T	 *gpRealRegCHA_CVI;
extern CHA_CVI_REG_T	 *gpRegCHA_CVI;
extern int gResolution;
extern volatile VPORT_RegBx_T *gVportRegBx;
extern			VPORT_RegBx_T *gpRegVPORT;		// shadow register
LX_AFE_ADC_INPUT_SOURCE_TYPE_T gADC_type= LX_ADC_INPUT_SOURCE_NONE;
extern int gPrevious_index;
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

int ADC_PowerOn_Bx(void)
{

	// reg_exta_sel , reg_extb_sel
	//gVportRegBx->reg_exta_sel.reg_exta_sel = 0x2;
	//gVportRegBx->reg_exta_sel.reg_extb_sel = 0x2;

//	gVportRegBx->top_004.reg_swrst_exta = 0x0;
//	gVportRegBx->top_004.reg_swrst_extb = 0x0;


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_biaspdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


	mdelay(5);


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_biaspdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


	mdelay(5);


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_003.reg_ch3pll_pdb = 1;
#else
	VPORT_RdFL(top_003);
	VPORT_Wr01(top_003, reg_ch3pll_pdb, 1);
	VPORT_WrFL(top_003);
#endif


	//gVportRegBx->top_008.reg_swrst_ch3pix = 1;


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_bpdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_bpdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_gpdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_gpdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_rpdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_rpdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_vref_pdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_llpll, 1);
	VPORT_WrFL(top_008);
#endif

	mdelay(20);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 0;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_llpll, 0);
	VPORT_WrFL(top_008);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_ch3pix = 1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_ch3pix, 1);
	VPORT_WrFL(top_008);
#endif

	mdelay(20);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_ch3pix = 0;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_ch3pix, 0);
	VPORT_WrFL(top_008);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_015.afe3ch_volbypas = 0;
#else
	VPORT_RdFL(ch3_adc_015);
	VPORT_Wr01(ch3_adc_015, afe3ch_volbypas, 0);
	VPORT_WrFL(ch3_adc_015);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_015.afe3ch_envolclp = 1;
#else
	VPORT_RdFL(ch3_adc_015);
	VPORT_Wr01(ch3_adc_015, afe3ch_envolclp, 1);
	VPORT_WrFL(ch3_adc_015);
#endif


//Enable Interrupt
	//gVportRegBx->intr_vp2_enable.intr_vp2_enable = ADC_DETECTION_INTERRUPT_MASK | ADC_AOGC_INTERRUPT_MASK;

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->intr_vp2_enable.intr_vp2_enable = ADC_DETECTION_INTERRUPT_MASK;
#else
	VPORT_RdFL(intr_vp2_enable);
	VPORT_Wr01(intr_vp2_enable, intr_vp2_enable, ADC_DETECTION_INTERRUPT_MASK);
	VPORT_WrFL(intr_vp2_enable);
#endif

	return 0;

}

int ADC_PowerOff_Bx(void)
{
	// reg_exta_sel , reg_extb_sel
	//gVportRegBx->reg_exta_sel.reg_exta_sel = 0x0;
	//gVportRegBx->reg_exta_sel.reg_extb_sel = 0x0;


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_biaspdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_vref_pdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif




#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_bpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_bpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_gpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_gpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_rpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_rpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_003.reg_ch3pll_pdb = 0;
#else
	VPORT_RdFL(top_003);
	VPORT_Wr01(top_003, reg_ch3pll_pdb, 0);
	VPORT_WrFL(top_003);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_llpll, 1);
	VPORT_WrFL(top_008);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_ch3pix = 1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_ch3pix, 1);
	VPORT_WrFL(top_008);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_015.afe3ch_volbypas = 0;
#else
	VPORT_RdFL(ch3_adc_015);
	VPORT_Wr01(ch3_adc_015, afe3ch_volbypas, 0);
	VPORT_WrFL(ch3_adc_015);
#endif


	return 0;

}

int ADC_InitDaughterBoard_Bx(void)
{
	//ADC_PowerOn();

	//gVportRegBx->ch3_cst_002.cst_postcoast = 0x1f;	//Macro vision pseudo Sync

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_002.cst_precoast = 0xa;	//1006 by moon
#else
	VPORT_RdFL(ch3_cst_002);
	VPORT_Wr01(ch3_cst_002, cst_precoast, 0xa);
	VPORT_WrFL(ch3_cst_002);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_002.cst_postcoast = 0x1d;	//1006 by moon
#else
	VPORT_RdFL(ch3_cst_002);
	VPORT_Wr01(ch3_cst_002, cst_postcoast, 0x1d);
	VPORT_WrFL(ch3_cst_002);
#endif

//	gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x2;	// RGB shaking problem => H positon shift problem
	//gVportRegBx->intr_vp2_enable.intr_vp2_enable = 0xffffffff;
	//gVportRegBx->ch3_adc_002.afe3ch_soglvl = 0xa; // default value is 0xe 0617 by Jun
	//gVportRegBx->ch3_adc_002.afe3ch_soglvl = 0xb; // default value is 0xe 0708 by dws
	//gVportRegBx->ch3_adc_002.ch3_adc_002 = 0xe; // for IFM (correct Vtotal read) , but has problem with 480i/576i pattern
	//gVportRegBx->ch3_adc_002.ch3_adc_002 = 0x9; // 0906 bd-390 Transformer warning scene horizontal shaking.
	//gVportRegBx->ch3_adc_002.afe3ch_soglvl = 0xa;  // 0908 for Macrovision & LG BDP Warning Scene.

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_002.afe3ch_soglvl = 0xb; // Continuos Interrupt (0xa)
	gVportRegBx->ch3_adc_002.afe3ch_sog_bw = 0x3; // 1102 by powerory
	gVportRegBx->ch3_adc_002.afe3ch_sog_hys = 0x2; // 1005 by moon
#else
	VPORT_RdFL(ch3_adc_002);
//	VPORT_Wr01(ch3_adc_002, afe3ch_soglvl, 0xb);
	//VPORT_Wr01(ch3_adc_002, afe3ch_soglvl, 0x7);	//110325 for shibasoku 33% sync level
	VPORT_Wr01(ch3_adc_002, afe3ch_soglvl, 0x6);	//110414
	//VPORT_Wr01(ch3_adc_002, afe3ch_sog_bw, 0x3);
	VPORT_Wr01(ch3_adc_002, afe3ch_sog_bw, 0x2);	//110407 by powerory
	VPORT_Wr01(ch3_adc_002, afe3ch_sog_hys, 0x1); //110325 for shibasoku 33% sync level
	//VPORT_Wr01(ch3_adc_002, afe3ch_sog_sinki, 0x1); //110325 for shibasoku 33% sync level
	VPORT_Wr01(ch3_adc_002, afe3ch_sog_sinki, 0x3); //110414
	VPORT_WrFL(ch3_adc_002);
#endif


	//Added for TVX 1080P shaking problem
	//Removed (side effect on directv settop)
//	gVportRegBx->ch3_adc_002.afe3ch_sog_bw = 0x1; // default value is 0x0 0621 by Jun
	//gVportRegBx->ch3_adc_002.afe3ch_sog_bw = 0x3; // 0908 for Macrovision & LG BDP Warning Scene.
	//gVportRegBx->ch3_adc_002.afe3ch_sog_bw = 0x1; // 1005 by moon

	//gVportRegBx->ch3_adc_002.afe3ch_sog_hys = 0x2; //0903 bd-390 Transformer warning scene horizontal shaking.
	//gVportRegBx->ch3_adc_002.afe3ch_sog_hys = 0x0; // 0908 for Macrovision & LG BDP Warning Scene.


	//gVportRegBx->ch3_digital_015.ch3reg_size_detect_ctrl = 0x388D;
	//gVportRegBx->ch3_digital_015.ch3_digital_015 = 0x3F4D; // For Component/RGB Interrupt Issue(old value: 0x388d) ====>>>> Component Detection Issue
	//gVportRegBx->ch3_digital_015.ch3reg_size_detect_ctrl = 0x3FCD; // 0825 new value from powerory //SS BDP menu shaking...

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_015.ch3reg_size_detect_ctrl = 0x3DCD; // 0913 for (time 221 to 222 no interrupt issue)
#else
	VPORT_RdFL(ch3_digital_015);
	VPORT_Wr01(ch3_digital_015, ch3reg_size_detect_ctrl, 0x3DCD);
	VPORT_WrFL(ch3_digital_015);
#endif


	//gVportRegBx->ch3_cst_003.cst_hspolover = 0x1; // 0825 new value from powerory
	//gVportRegBx->ch3_cst_003.cst_vspolover = 0x1; // 0825 new value from powerory
	//gVportRegBx->ch3_cst_003.cst_hspoluser = 0x0; // 0825 new value from powerory
	//gVportRegBx->ch3_cst_003.cst_vspoluser = 0x0; // 0825 new value from powerory

	//Add From B0 Revision(Auto Calibration)

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_034.ch3reg_aogc_sample_accum = 0x0;
#else
	VPORT_RdFL(ch3_digital_034);
	VPORT_Wr01(ch3_digital_034, ch3reg_aogc_sample_accum, 0x0);
	VPORT_WrFL(ch3_digital_034);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_034.ch3reg_aogc_aoc_accum = 0x0;
#else
	VPORT_RdFL(ch3_digital_034);
	VPORT_Wr01(ch3_digital_034, ch3reg_aogc_aoc_accum, 0x0);
	VPORT_WrFL(ch3_digital_034);
#endif


	//Add From B0 Revision(for Component)

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_005.afe3ch_b_ph = 0x1;
#else
	VPORT_RdFL(ch3_adc_005);
	VPORT_Wr01(ch3_adc_005, afe3ch_b_ph, 0x1);
	VPORT_WrFL(ch3_adc_005);
#endif

	// set CST_EQ_THR to 1 (from 4)
	VPORT_RdFL(ch3_adc_005);
	VPORT_Wr01(ch3_adc_005, afe3ch_g_ph, 1);
	VPORT_WrFL(ch3_adc_005);

	//For no interrupt occurrence when resolution change between master 210(576i) to 209(480i)
	//gVportRegBx->ch3_llpll_010.llpll_lpf_clk_sel = 0x1;
	//110214

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_010.llpll_lpf_clk_sel = 0x0;
#else
	VPORT_RdFL(ch3_llpll_010);
	VPORT_Wr01(ch3_llpll_010, llpll_lpf_clk_sel, 0x0);
	VPORT_WrFL(ch3_llpll_010);
#endif


	ADC_PowerOn();

	//20110125
//	_iow(&gVportRegBx->ch3_digital_003 , 8 , 0 , 0x80);	// ch3reg_hspdeq_thr
	_iow(&gVportRegBx->ch3_digital_063 , 8 , 0 , 0xc4);	// ch3reg_hsprdeq_thr

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


int ADC_GetBoundaryInform_Bx(ST_EXTAU *inform)
{
	// get extau_up
	inform->extau_up = gVportRegBx->ch3_digital_058.ch3reg_extau_up_result;

	// get extau_down
	inform->extau_down = gVportRegBx->ch3_digital_058.ch3reg_extau_down_result;

	// get extau_left
#ifdef ADC_BOUNDARY_COMPENSATION
	if(gVportRegBx->ch3_digital_059.ch3reg_extau_left_result >= 1)
		inform->extau_left = gVportRegBx->ch3_digital_059.ch3reg_extau_left_result - 1 ;// powerory
	else
#endif
		inform->extau_left = gVportRegBx->ch3_digital_059.ch3reg_extau_left_result;// powerory

	// get extau_lest
#ifdef ADC_BOUNDARY_COMPENSATION
	if(gVportRegBx->ch3_digital_059.ch3reg_extau_right_result >= 1)
		inform->extau_right = gVportRegBx->ch3_digital_059.ch3reg_extau_right_result - 1 ;// powerory
	else
#endif
		inform->extau_right = gVportRegBx->ch3_digital_059.ch3reg_extau_right_result;// powerory
	//inform->extau_right = gVportRegBx->ch3_digital_059.ch3reg_extau_right_result;

	return 0;
}

int ADC_SetTAUInform_Bx(S_EXTAU_VALUE *value)
{
	UINT16	hsout_width_tmp;
	// Height

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_026.ch3reg_extau_down_offset = value->height_offset;
#else
	VPORT_RdFL(ch3_digital_026);
	VPORT_Wr01(ch3_digital_026, ch3reg_extau_down_offset, value->height_offset);
	VPORT_WrFL(ch3_digital_026);
#endif


	// WIDTH

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_028.ch3reg_extau_right_offset = value->width_offset;
#else
	VPORT_RdFL(ch3_digital_028);
	VPORT_Wr01(ch3_digital_028, ch3reg_extau_right_offset, value->width_offset);
	VPORT_WrFL(ch3_digital_028);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_023.ch3reg_extau_ctrl = value->control;
#else
	VPORT_RdFL(ch3_digital_023);
	VPORT_Wr01(ch3_digital_023, ch3reg_extau_ctrl, value->control);
	VPORT_WrFL(ch3_digital_023);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_024.ch3reg_extau_level = value->level;
#else
	VPORT_RdFL(ch3_digital_024);
	VPORT_Wr01(ch3_digital_024, ch3reg_extau_level, value->level);
	VPORT_WrFL(ch3_digital_024);
#endif


	// For Proper Boundary Detection in case of PC RGB Horizontal Sync Crosstalk on RGB Signal, Adjusted left_offset.
	// Enlarge Value to 10 1019 for PC geforce 8400GS
#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_027.ch3reg_extau_left_offset = gVportRegBx->ch3_digital_044.ch3reg_ro_hsout_width + 10;//5; // by powerory 1013

#else
	VPORT_RdFL(ch3_digital_044);
	VPORT_Rd01(ch3_digital_044, ch3reg_ro_hsout_width, hsout_width_tmp);

	VPORT_RdFL(ch3_digital_027);
	VPORT_Wr01(ch3_digital_027, ch3reg_extau_left_offset, hsout_width_tmp + 10);
	VPORT_WrFL(ch3_digital_027);
#endif
	return 0;

}

int ADC_SetRGBPosition_Bx(S_RGB_POSITION *pos)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_021.ch3reg_ap_xpos_b = pos->width_b;
#else
	VPORT_RdFL(ch3_digital_021);
	VPORT_Wr01(ch3_digital_021, ch3reg_ap_xpos_b, pos->width_b);
	VPORT_WrFL(ch3_digital_021);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_019.ch3reg_ap_xpos_g = pos->width_g;
#else
	VPORT_RdFL(ch3_digital_019);
	VPORT_Wr01(ch3_digital_019, ch3reg_ap_xpos_g, pos->width_g);
	VPORT_WrFL(ch3_digital_019);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_017.ch3reg_ap_xpos_r = pos->width_r;
#else
	VPORT_RdFL(ch3_digital_017);
	VPORT_Wr01(ch3_digital_017, ch3reg_ap_xpos_r, pos->width_r);
	VPORT_WrFL(ch3_digital_017);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_022.ch3reg_ap_ypos_b = pos->height_b;
#else
	VPORT_RdFL(ch3_digital_022);
	VPORT_Wr01(ch3_digital_022, ch3reg_ap_ypos_b, pos->height_b);
	VPORT_WrFL(ch3_digital_022);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_020.ch3reg_ap_ypos_g = pos->height_g;
#else
	VPORT_RdFL(ch3_digital_020);
	VPORT_Wr01(ch3_digital_020, ch3reg_ap_ypos_g, pos->height_g);
	VPORT_WrFL(ch3_digital_020);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_018.ch3reg_ap_ypos_r = pos->height_r;
#else
	VPORT_RdFL(ch3_digital_018);
	VPORT_Wr01(ch3_digital_018, ch3reg_ap_ypos_r, pos->height_r);
	VPORT_WrFL(ch3_digital_018);
#endif


	return 0;
}

int ADC_GetRGBResult_Bx(S_RGB_OUT *result)
{
	result->out_b =  gVportRegBx->ch3_digital_054.ch3reg_ap_out_b;
	result->out_g =	 gVportRegBx->ch3_digital_053.ch3reg_ap_out_g;
	result->out_r =  gVportRegBx->ch3_digital_052.ch3reg_ap_out_r;

	return 0;
}

int ADC_GetRGBPosition_Bx(S_RGB_POSITION *pos)
{
	pos->width_b =  gVportRegBx->ch3_digital_021.ch3reg_ap_xpos_b;
	pos->width_g =	gVportRegBx->ch3_digital_019.ch3reg_ap_xpos_g;
	pos->width_r =  gVportRegBx->ch3_digital_017.ch3reg_ap_xpos_r;

	pos->height_b = gVportRegBx->ch3_digital_022.ch3reg_ap_ypos_b;
	pos->height_g = gVportRegBx->ch3_digital_020.ch3reg_ap_ypos_g;
	pos->height_r = gVportRegBx->ch3_digital_018.ch3reg_ap_ypos_r;

	return 0;
}

int ADC_SetRGBSize_Bx(S_RGB_SIZE *size)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_021.ch3reg_ap_xsize_b = size->width_b;
#else
	VPORT_RdFL(ch3_digital_021);
	VPORT_Wr01(ch3_digital_021, ch3reg_ap_xsize_b, size->width_b);
	VPORT_WrFL(ch3_digital_021);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_019.ch3reg_ap_xsize_g = size->width_g;
#else
	VPORT_RdFL(ch3_digital_019);
	VPORT_Wr01(ch3_digital_019, ch3reg_ap_xsize_g, size->width_g);
	VPORT_WrFL(ch3_digital_019);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_017.ch3reg_ap_xsize_r = size->width_r;
#else
	VPORT_RdFL(ch3_digital_017);
	VPORT_Wr01(ch3_digital_017, ch3reg_ap_xsize_r, size->width_r);
	VPORT_WrFL(ch3_digital_017);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_022.ch3reg_ap_ysize_b = size->height_b;
#else
	VPORT_RdFL(ch3_digital_022);
	VPORT_Wr01(ch3_digital_022, ch3reg_ap_ysize_b, size->height_b);
	VPORT_WrFL(ch3_digital_022);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_020.ch3reg_ap_ysize_g = size->height_g;
#else
	VPORT_RdFL(ch3_digital_020);
	VPORT_Wr01(ch3_digital_020, ch3reg_ap_ysize_g, size->height_g);
	VPORT_WrFL(ch3_digital_020);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_018.ch3reg_ap_ysize_r = size->height_r;
#else
	VPORT_RdFL(ch3_digital_018);
	VPORT_Wr01(ch3_digital_018, ch3reg_ap_ysize_r, size->height_r);
	VPORT_WrFL(ch3_digital_018);
#endif


	return 0;
}

int ADC_GetSizeValue_Bx(S_VIDEO_SIZE_INFORM *size)
{
	size->ext_vsize = gVportRegBx->ch3_digital_048.ch3reg_sd_ext_vsize;
	size->vd_vsize = gVportRegBx->ch3_digital_049.ch3reg_sd_vd_vsize;
	size->vd_hsize = gVportRegBx->ch3_digital_050.ch3reg_sd_vd_hsize;
	size->polarity = (gVportRegBx->ch3_cst_006.cst_ro_hspol<<1) | (gVportRegBx->ch3_cst_007.cst_ro_vspol);

	return 0;

}

int ADC_SetPort_Bx(S_SEL_PORT *sel_port)
{

#ifdef	KDRV_CONFIG_PM
	memcpy((void *)&gAdcPMData->gstPmSelPortValue , (void *)sel_port , sizeof(S_SEL_PORT));
#endif
	//AFE_PRINT("ADC_SetPort \n");

	AFE_PRINT("ADC_SetPort : sel_port->port = [%d] \n", sel_port->port);


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_004.afe3ch_selmux = sel_port->port;
#else
	VPORT_RdFL(ch3_adc_004);
	VPORT_Wr01(ch3_adc_004, afe3ch_selmux, sel_port->port);
	VPORT_WrFL(ch3_adc_004);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_005.cst_selmux = sel_port->port;
#else
	VPORT_RdFL(ch3_cst_005);
	VPORT_Wr01(ch3_cst_005, cst_selmux, sel_port->port);
	VPORT_WrFL(ch3_cst_005);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_003.cst_hsselover = sel_port->hsselover;
#else
	VPORT_RdFL(ch3_cst_003);
	VPORT_Wr01(ch3_cst_003, cst_hsselover, sel_port->hsselover);
	VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_003.cst_hsseluser = sel_port->hsseluser;
#else
	VPORT_RdFL(ch3_cst_003);
	VPORT_Wr01(ch3_cst_003, cst_hsseluser, sel_port->hsseluser);
	VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_003.cst_vsselover = sel_port->vsselover;
#else
	VPORT_RdFL(ch3_cst_003);
	VPORT_Wr01(ch3_cst_003, cst_vsselover, sel_port->vsselover);
	VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_003.cst_vsseluser = sel_port->vsseluser;
#else
	VPORT_RdFL(ch3_cst_003);
	VPORT_Wr01(ch3_cst_003, cst_vsseluser, sel_port->vsseluser);
	VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_004.cst_compover = sel_port->compover;
#else
	VPORT_RdFL(ch3_cst_004);
	VPORT_Wr01(ch3_cst_004, cst_compover, sel_port->compover);
	VPORT_WrFL(ch3_cst_004);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_cst_004.cst_compuser = sel_port->compuser;
#else
	VPORT_RdFL(ch3_cst_004);
	VPORT_Wr01(ch3_cst_004, cst_compuser, sel_port->compuser);
	VPORT_WrFL(ch3_cst_004);
#endif


	return 0;
}

int ADC_GetSelectedPort_Bx(void)
{
	return	gVportRegBx->ch3_adc_004.afe3ch_selmux;
}

unsigned int ADC_GetPhaseValue_Bx(void)
{
	return(gVportRegBx->ch3_llpll_004.llpll_shift);

}

int ADC_SetPhaseValue_Bx(unsigned int phase)
{

//	AFE_PRINT("######### Set ADC Phase Register to [%d][0x%x]\n", phase,phase );

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_004.llpll_shift = phase;
#else
	VPORT_RdFL(ch3_llpll_004);
	VPORT_Wr01(ch3_llpll_004, llpll_shift, phase);
	VPORT_WrFL(ch3_llpll_004);
#endif


	return 0;

}

unsigned int ADC_GetScanType_Bx(void)
{
	unsigned int ret=0;
	UINT8	count=0;

	for(;count<INTERLACE_CHECK_COUNT;count++)
	   ret += gVportRegBx->ch3_cst_007.cst_ro_interlaced^1;

//	AFE_PRINT("* Interlace check count value = [%d]\n", ret);

	if(ret > (INTERLACE_CHECK_COUNT/2))
		return 1;
	else
		return 0;

}


int ADC_SetGainValue_Bx(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_008.afe3ch_gin_r = stGain->R_Gain_Value;
#else
	VPORT_RdFL(ch3_adc_008);
	VPORT_Wr01(ch3_adc_008, afe3ch_gin_r, stGain->R_Gain_Value);
	VPORT_WrFL(ch3_adc_008);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_009.afe3ch_gin_g = stGain->G_Gain_Value;
#else
	VPORT_RdFL(ch3_adc_009);
	VPORT_Wr01(ch3_adc_009, afe3ch_gin_g, stGain->G_Gain_Value);
	VPORT_WrFL(ch3_adc_009);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_010.afe3ch_gin_b = stGain->B_Gain_Value;
#else
	VPORT_RdFL(ch3_adc_010);
	VPORT_Wr01(ch3_adc_010, afe3ch_gin_b, stGain->B_Gain_Value);
	VPORT_WrFL(ch3_adc_010);
#endif


	return ret;

}

int ADC_GetGainValue_Bx(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;

	stGain->R_Gain_Value = gVportRegBx->ch3_adc_008.afe3ch_gin_r;
	stGain->G_Gain_Value = gVportRegBx->ch3_adc_009.afe3ch_gin_g;
	stGain->B_Gain_Value = gVportRegBx->ch3_adc_010.afe3ch_gin_b;


	return ret;
}


int ADC_SetOffsetValue_Bx(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_011.ch3reg_afe3ch_oftin_r = stOffset->R_Offset_Value;
#else
	VPORT_RdFL(ch3_adc_011);
	VPORT_Wr01(ch3_adc_011, ch3reg_afe3ch_oftin_r, stOffset->R_Offset_Value);
	VPORT_WrFL(ch3_adc_011);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_012.ch3reg_afe3ch_oftin_g = stOffset->G_Offset_Value;
#else
	VPORT_RdFL(ch3_adc_012);
	VPORT_Wr01(ch3_adc_012, ch3reg_afe3ch_oftin_g, stOffset->G_Offset_Value);
	VPORT_WrFL(ch3_adc_012);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_013.ch3reg_afe3ch_oftin_b = stOffset->B_Offset_Value;
#else
	VPORT_RdFL(ch3_adc_013);
	VPORT_Wr01(ch3_adc_013, ch3reg_afe3ch_oftin_b, stOffset->B_Offset_Value);
	VPORT_WrFL(ch3_adc_013);
#endif


	return ret;

}

int ADC_GetOffsetValue_Bx(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;

	stOffset->R_Offset_Value = gVportRegBx->ch3_adc_011.ch3reg_afe3ch_oftin_r;
	stOffset->G_Offset_Value = gVportRegBx->ch3_adc_012.ch3reg_afe3ch_oftin_g;
	stOffset->B_Offset_Value = gVportRegBx->ch3_adc_013.ch3reg_afe3ch_oftin_b;

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
int ADC_SetInputVideo_Bx(int iIndex)
{

	int i;
	UINT32 hsout_width =0;
	UINT8	llpll_filter_status;

	int ret = 0;

#ifdef	KDRV_CONFIG_PM
	gAdcPMData->giPmIndexValue = iIndex;
#endif
	AFE_PRINT("%s entered with value [%d]\n", __func__, iIndex);

	//CH3_LLPLL_003

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_003.llpll_div_max = LLPLL_div_max[iIndex];
#else
	VPORT_RdFL(ch3_llpll_003);
	VPORT_Wr01(ch3_llpll_003, llpll_div_max, LLPLL_div_max[iIndex]);
	VPORT_WrFL(ch3_llpll_003);
#endif


	//LLPLL_counter_max

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_003.llpll_counter_max = LLPLL_counter_max[iIndex];
#else
	VPORT_RdFL(ch3_llpll_003);
	VPORT_Wr01(ch3_llpll_003, llpll_counter_max, LLPLL_counter_max[iIndex]);
	VPORT_WrFL(ch3_llpll_003);
#endif


	//LLPLL_shift 일단 0으로 초기화

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_004.llpll_shift = 0;		// should be set the phase later
#else
	VPORT_RdFL(ch3_llpll_004);
	VPORT_Wr01(ch3_llpll_004, llpll_shift, 0);
	VPORT_WrFL(ch3_llpll_004);
#endif


	//LLPLL_dco_max

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_006.llpll_dco_max = LLPLL_dco_max[iIndex];
#else
	VPORT_RdFL(ch3_llpll_006);
	VPORT_Wr01(ch3_llpll_006, llpll_dco_max, LLPLL_dco_max[iIndex]);
	VPORT_WrFL(ch3_llpll_006);
#endif


	//LLPLL_dco_min

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_006.llpll_dco_min = LLPLL_dco_min[iIndex];
#else
	VPORT_RdFL(ch3_llpll_006);
	VPORT_Wr01(ch3_llpll_006, llpll_dco_min, LLPLL_dco_min[iIndex]);
	VPORT_WrFL(ch3_llpll_006);
#endif


	//LLPLL_coarse_scale

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_007.llpll_coarse_scale = LLPLL_coarse_scale[iIndex];
#else
	VPORT_RdFL(ch3_llpll_007);
	VPORT_Wr01(ch3_llpll_007, llpll_coarse_scale, LLPLL_coarse_scale[iIndex]);
	VPORT_WrFL(ch3_llpll_007);
#endif


	//LLPLL_g1_nom

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_008.llpll_g1_nom = LLPLL_g1_nom[iIndex];
#else
	VPORT_RdFL(ch3_llpll_008);
	VPORT_Wr01(ch3_llpll_008, llpll_g1_nom, LLPLL_g1_nom[iIndex]);
	VPORT_WrFL(ch3_llpll_008);
#endif


	//LLPLL_g2_nom

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_008.llpll_g2_nom = LLPLL_g2_nom[iIndex];
#else
	VPORT_RdFL(ch3_llpll_008);
	VPORT_Wr01(ch3_llpll_008, llpll_g2_nom, LLPLL_g2_nom[iIndex]);
	VPORT_WrFL(ch3_llpll_008);
#endif


	//LLPLL_g3_p_nom

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_008.llpll_g3_p_nom = LLPLL_g3_p_nom[iIndex];
#else
	VPORT_RdFL(ch3_llpll_008);
	VPORT_Wr01(ch3_llpll_008, llpll_g3_p_nom, LLPLL_g3_p_nom[iIndex]);
	VPORT_WrFL(ch3_llpll_008);
#endif


	//LLPLL_g3_n_nom

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_008.llpll_g3_n_nom = LLPLL_g3_n_nom[iIndex];
#else
	VPORT_RdFL(ch3_llpll_008);
	VPORT_Wr01(ch3_llpll_008, llpll_g3_n_nom, LLPLL_g3_n_nom[iIndex]);
	VPORT_WrFL(ch3_llpll_008);
#endif


	//LLPLL_g1_fine

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_009.llpll_g1_fine = LLPLL_g1_fine[iIndex];
#else
	VPORT_RdFL(ch3_llpll_009);
	VPORT_Wr01(ch3_llpll_009, llpll_g1_fine, LLPLL_g1_fine[iIndex]);
	VPORT_WrFL(ch3_llpll_009);
#endif


	//LLPLL_g2_fine

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_009.llpll_g2_fine = LLPLL_g2_fine[iIndex];
#else
	VPORT_RdFL(ch3_llpll_009);
	VPORT_Wr01(ch3_llpll_009, llpll_g2_fine, LLPLL_g2_fine[iIndex]);
	VPORT_WrFL(ch3_llpll_009);
#endif


	//LLPLL_g3_p_fine

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_009.llpll_g3_p_fine = LLPLL_g3_p_fine[iIndex];
#else
	VPORT_RdFL(ch3_llpll_009);
	VPORT_Wr01(ch3_llpll_009, llpll_g3_p_fine, LLPLL_g3_p_fine[iIndex]);
	VPORT_WrFL(ch3_llpll_009);
#endif


	//LLPLL_g3_n_fine

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_009.llpll_g3_n_fine = LLPLL_g3_n_fine[iIndex];
#else
	VPORT_RdFL(ch3_llpll_009);
	VPORT_Wr01(ch3_llpll_009, llpll_g3_n_fine, LLPLL_g3_n_fine[iIndex]);
	VPORT_WrFL(ch3_llpll_009);
#endif


	for( i = 0 ; i < 30 ; i++)
	{
#if VPORT_REG_DIRECT_ACCESS
	llpll_filter_status = gVportRegBx->ch3_llpll_015.llpll_filter_status;
#else
	VPORT_RdFL(ch3_llpll_015);
	VPORT_Rd01(ch3_llpll_015, llpll_filter_status, llpll_filter_status);
#endif
		if(llpll_filter_status == 3) break;
		mdelay(5);
	}
	if(i==30)
	{
		AFE_PRINT("!!!!!!!!!!!! LLPLL UNLOCK !!!!!!!!!!!!!!!!!\n");
		ADC_Reset_LLPLL();
		ret = -1;
	}
#if 0
	for( i = 0 ; i < 100 ; i++)
	{
		if(gVportRegBx->ch3_llpll_015.llpll_filter_status != 3)
			AFE_PRINT("!!!!!!!!!!!! %d LLPLL UNLOCK !!!!!!!!!!!!!!!!!\n", i );
		mdelay(5);
	}
#endif
/*
	gVportRegBx->afe3ch_biaspdb.afe3ch_rpdb = 0;
	gVportRegBx->afe3ch_biaspdb.afe3ch_gpdb = 0;
	gVportRegBx->afe3ch_biaspdb.afe3ch_bpdb = 0;

	mdelay(5);

	gVportRegBx->afe3ch_biaspdb.afe3ch_rpdb = 1;
	gVportRegBx->afe3ch_biaspdb.afe3ch_gpdb = 1;
	gVportRegBx->afe3ch_biaspdb.afe3ch_bpdb = 1;

	mdelay(5);
*/
	if(iIndex == 306) // for Component 576i 50Hz field indication should be inverted
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_005.ch3reg_fld_inv = 0x1;
#else
		VPORT_RdFL(ch3_digital_005);
		VPORT_Wr01(ch3_digital_005, ch3reg_fld_inv, 0x1);
		VPORT_WrFL(ch3_digital_005);
#endif
}
	else
{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_005.ch3reg_fld_inv = 0x0;
#else
		VPORT_RdFL(ch3_digital_005);
		VPORT_Wr01(ch3_digital_005, ch3reg_fld_inv, 0x0);
		VPORT_WrFL(ch3_digital_005);
#endif
}

#if 0
	mdelay(100); //tmp for test

	//added 0716 for horizontal shaking problem
	for(i=0; i<4; i++)
	{
		hsout_width += gVportRegBx->ch3_digital_044.ch3reg_ro_hsout_width;
		//AFE_PRINT("hsout_width read = [%d]\n", hsout_width);
	}

	hsout_width = hsout_width >>2;

	AFE_PRINT("calculated hsout_width = [%d]\n", hsout_width);

	gVportRegBx->ch3_digital_004.ch3reg_hsout_width = hsout_width;
#else

	hsout_width = HS_Out_Width[iIndex];

//	AFE_PRINT("Table hsout_width = [%d]\n", hsout_width);


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_004.ch3reg_hsout_width = hsout_width;
#else
	VPORT_RdFL(ch3_digital_004);
	VPORT_Wr01(ch3_digital_004, ch3reg_hsout_width, hsout_width);
	VPORT_WrFL(ch3_digital_004);
#endif


	/* For Test Purpose 110214
	gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 1;
	gVportRegBx->ch3_digital_003.ch3reg_hsoutcnt = LLPLL_counter_max[iIndex] - 1;
	gVportRegBx->ch3_digital_065.ch3reg_vsoutover = 1;
	gVportRegBx->ch3_digital_065.ch3reg_vsoutcnt = TB_SD_EXT_VSIZE[iIndex] * LLPLL_counter_max[iIndex] - 1;
	*/
#endif

	/*
	if(i<100)
		return 0;
	else
		return -1;
		*/
	return ret;
}



int ADC_AGOCInit_Bx(void)
{

	// clamp off

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_015.afe3ch_envolclp = 0;
#else
	VPORT_RdFL(ch3_adc_015);
	VPORT_Wr01(ch3_adc_015, afe3ch_envolclp, 0);
	VPORT_WrFL(ch3_adc_015);
#endif


	// aogc conter set as max

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_030.ch3reg_aogc_counter_max = 0xFFFE;
#else
	VPORT_RdFL(ch3_digital_030);
	VPORT_Wr01(ch3_digital_030, ch3reg_aogc_counter_max, 0xFFFE);
	VPORT_WrFL(ch3_digital_030);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_030.ch3reg_aogc_sample_count = 6;	// 0 : 16, 1 : 32
#else
	VPORT_RdFL(ch3_digital_030);
	VPORT_Wr01(ch3_digital_030, ch3reg_aogc_sample_count, 6);
	VPORT_WrFL(ch3_digital_030);
#endif


#if 0
	// removed from B0
	// aoc, agc sel register
	gVportRegBx->ch3reg_agc_ire.ch3reg_agc_ire = 3;
	gVportRegBx->ch3reg_agc_ire.ch3reg_aoc_ire = 4;
#endif
//	gVportRegBx->ch3_adc_007.afe3ch_ire_sel = 4;

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
int ADC_SetIRETestMode_Bx(int sel , E_AGC_IRE mode)
{
	int ret = 0;


	if(sel)	// IRE Test mode inactive
	{
		ADC_AGOCInit();
/* powerory : before revision */
#ifdef BEFORE_REVISION
		if(mode == IRE0_RGB)
		{
#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->ch3_adc_005.afe3ch_r_ph = 0x7;//0x6;//0x3;	// modified parameter by jun's mail(08.02)
#else
			VPORT_RdFL(ch3_adc_005);
			VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x7);
			VPORT_WrFL(ch3_adc_005);
#endif
		}
		else
		{
#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->ch3_adc_005.afe3ch_r_ph = 0x7;//0x4;//0x3;
#else
			VPORT_RdFL(ch3_adc_005);
			VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x7);
			VPORT_WrFL(ch3_adc_005);
#endif
		}

		mdelay(100);

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_005.afe3ch_r_ph = 0x3;//0x4;//0x3;
#else
		VPORT_RdFL(ch3_adc_005);
		VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x3);
		VPORT_WrFL(ch3_adc_005);
#endif

#if VPORT_REG_DIRECT_ACCESS
		giSelMuxBackup = gVportRegBx->ch3_adc_004.afe3ch_selmux;
#else
		VPORT_RdFL(ch3_adc_004);
		VPORT_Rd01(ch3_adc_004, afe3ch_selmux, giSelMuxBackup);
#endif

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_004.afe3ch_selmux = 0x3;
#else
		VPORT_RdFL(ch3_adc_004);
		VPORT_Wr01(ch3_adc_004, afe3ch_selmux, 0x3);
		VPORT_WrFL(ch3_adc_004);
#endif

//~powerory
#else
// powerory : After Revision
#if VPORT_REG_DIRECT_ACCESS
		giSelMuxBackup = gVportRegBx->ch3_adc_004.afe3ch_selmux;
#else
		VPORT_RdFL(ch3_adc_004);
		VPORT_Rd01(ch3_adc_004, afe3ch_selmux, giSelMuxBackup);
#endif
		//gVportRegBx->ch3_adc_005.afe3ch_r_ph = 0x04 | giSelMuxBackup;//0x3;
		//mdelay(100);
		//gVportRegBx->ch3_adc_005.afe3ch_r_ph = giSelMuxBackup;//0x4;//0x3;

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_005.afe3ch_r_ph = 0x5;//0x3;
#else
		VPORT_RdFL(ch3_adc_005);
		VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x5);
		VPORT_WrFL(ch3_adc_005);
#endif

		mdelay(100);

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_005.afe3ch_r_ph = 0x1;//0x4;//0x3;
#else
		VPORT_RdFL(ch3_adc_005);
		VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x1);
		VPORT_WrFL(ch3_adc_005);
#endif


		//gVportRegBx->ch3_adc_004.afe3ch_selmux = 0x3;
//~powerory
#endif
		// aogc mode set as sampling only

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_029.ch3reg_aogc_mode = 0x0;
#else
		VPORT_RdFL(ch3_digital_029);
		VPORT_Wr01(ch3_digital_029, ch3reg_aogc_mode, 0x0);
		VPORT_WrFL(ch3_digital_029);
#endif

		// aogc clk

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_012.reg_clken_aoc = 1;
#else
		VPORT_RdFL(top_012);
		VPORT_Wr01(top_012, reg_clken_aoc, 1);
		VPORT_WrFL(top_012);
#endif


		// aogc test start

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_aogc_001.ch3_aogc_enable = 1;
#else
		VPORT_RdFL(ch3_aogc_001);
		VPORT_Wr01(ch3_aogc_001, ch3_aogc_enable, 1);
		VPORT_WrFL(ch3_aogc_001);
#endif


		mdelay(20);

	}
	else	// IRE Test mode active
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_004.afe3ch_selmux = giSelMuxBackup;
#else
		VPORT_RdFL(ch3_adc_004);
		VPORT_Wr01(ch3_adc_004, afe3ch_selmux, giSelMuxBackup);
		VPORT_WrFL(ch3_adc_004);
#endif



#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_005.afe3ch_r_ph = 0x0;
#else
		VPORT_RdFL(ch3_adc_005);
		VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x0);
		VPORT_WrFL(ch3_adc_005);
#endif


		// clamp on

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_015.afe3ch_envolclp = 1;
#else
		VPORT_RdFL(ch3_adc_015);
		VPORT_Wr01(ch3_adc_015, afe3ch_envolclp, 1);
		VPORT_WrFL(ch3_adc_015);
#endif


		// aogc clk

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_012.reg_clken_aoc = 0;
#else
		VPORT_RdFL(top_012);
		VPORT_Wr01(top_012, reg_clken_aoc, 0);
		VPORT_WrFL(top_012);
#endif

		// aogc mode set as sampling only

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_029.ch3reg_aogc_mode = 0x0;
#else
		VPORT_RdFL(ch3_digital_029);
		VPORT_Wr01(ch3_digital_029, ch3reg_aogc_mode, 0x0);
		VPORT_WrFL(ch3_digital_029);
#endif


		// aogc test stop

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_aogc_001.ch3_aogc_enable = 0;
#else
		VPORT_RdFL(ch3_aogc_001);
		VPORT_Wr01(ch3_aogc_001, ch3_aogc_enable, 0);
		VPORT_WrFL(ch3_aogc_001);
#endif

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

	temp_r = gVportRegBx->ch3_digital_041.ch3reg_ro_avg_r;
	temp_g = gVportRegBx->ch3_digital_042.ch3reg_ro_avg_g;
	temp_b = gVportRegBx->ch3_digital_043.ch3reg_ro_avg_b;

// powerory 2010-12-10
	//gain->gain_R = (float)( (temp_r&0xffff00) >> 8) + (float)(temp_r&0xff)/256.0;
	//gain->gain_G = (float)( (temp_g&0xffff00) >> 8) + (float)(temp_g&0xff)/256.0;
	//gain->gain_B = (float)( (temp_b&0xffff00) >> 8) + (float)(temp_b&0xff)/256.0;
//~powerory
//	gain->gain_R = (float)( (temp_r&0xffc000) >> 14) + (float)(temp_r&0x3fff)/16384.0;
//	gain->gain_G = (float)( (temp_g&0xffc000) >> 14) + (float)(temp_g&0x3fff)/16384.0;
//	gain->gain_B = (float)( (temp_b&0xffc000) >> 14) + (float)(temp_b&0x3fff)/16384.0;
	//120103 wonsik.do : removed floating point in kernel driver
//	gain->gain_R = (float)( temp_r >> 14);
//	gain->gain_G = (float)( (temp_g&0xffc000) >> 14) + (float)(temp_g&0x3fff)/16384.0;
//	gain->gain_B = (float)( (temp_b&0xffc000) >> 14) + (float)(temp_b&0x3fff)/16384.0;
	gain->gain_R = temp_r;
	gain->gain_G = temp_g;
	gain->gain_B = temp_b;


	//AFE_PRINT("IRE level 1 : R[%d] , G[%d] , B[%d] \n",gain->gain_R, gain->gain_G , gain->gain_B );

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
int ADC_SetIRELevel_Bx(E_AGC_IRE ire , S_GAIN_TARGET *gain)
{
	int ret = 0;
	unsigned short target_ire;

	ADC_DEBUG("IRE level set %d\n",ire);

	// set IRE
	//gVportRegBx->ch3_adc_007.ch3reg_adc_ire_sel_man = ire;

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_007.afe3ch_ire_sel = ire;
#else
	VPORT_RdFL(ch3_adc_007);
	VPORT_Wr01(ch3_adc_007, afe3ch_ire_sel, ire);
	VPORT_WrFL(ch3_adc_007);
#endif


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

#if 0
	// removed from B0
	if(ire == IRE0_COMP){

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3reg_aoc_r_compare.ch3reg_aoc_r_compare = 512;
#else
		VPORT_RdFL(ch3reg_aoc_r_compare);
		VPORT_Wr01(ch3reg_aoc_r_compare, ch3reg_aoc_r_compare, 512);
		VPORT_WrFL(ch3reg_aoc_r_compare);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3reg_aoc_g_compare.ch3reg_aoc_g_compare = target_ire;
#else
		VPORT_RdFL(ch3reg_aoc_g_compare);
		VPORT_Wr01(ch3reg_aoc_g_compare, ch3reg_aoc_g_compare, target_ire);
		VPORT_WrFL(ch3reg_aoc_g_compare);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3reg_aoc_b_compare.ch3reg_aoc_b_compare = 512;
#else
		VPORT_RdFL(ch3reg_aoc_b_compare);
		VPORT_Wr01(ch3reg_aoc_b_compare, ch3reg_aoc_b_compare, 512);
		VPORT_WrFL(ch3reg_aoc_b_compare);
#endif

	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3reg_aoc_r_compare.ch3reg_aoc_r_compare = target_ire;
#else
		VPORT_RdFL(ch3reg_aoc_r_compare);
		VPORT_Wr01(ch3reg_aoc_r_compare, ch3reg_aoc_r_compare, target_ire);
		VPORT_WrFL(ch3reg_aoc_r_compare);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3reg_aoc_g_compare.ch3reg_aoc_g_compare = target_ire;
#else
		VPORT_RdFL(ch3reg_aoc_g_compare);
		VPORT_Wr01(ch3reg_aoc_g_compare, ch3reg_aoc_g_compare, target_ire);
		VPORT_WrFL(ch3reg_aoc_g_compare);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3reg_aoc_b_compare.ch3reg_aoc_b_compare = target_ire;
#else
		VPORT_RdFL(ch3reg_aoc_b_compare);
		VPORT_Wr01(ch3reg_aoc_b_compare, ch3reg_aoc_b_compare, target_ire);
		VPORT_WrFL(ch3reg_aoc_b_compare);
#endif

	}
#else
	if(ire == IRE0_COMP){

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_031.ch3reg_r_compare = 512<<14;
#else
		VPORT_RdFL(ch3_digital_031);
		VPORT_Wr01(ch3_digital_031, ch3reg_r_compare, 512<<14);
		VPORT_WrFL(ch3_digital_031);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_032.ch3reg_g_compare = target_ire<<14;
#else
		VPORT_RdFL(ch3_digital_032);
		VPORT_Wr01(ch3_digital_032, ch3reg_g_compare, target_ire<<14);
		VPORT_WrFL(ch3_digital_032);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_033.ch3reg_b_compare = 512<<14;
#else
		VPORT_RdFL(ch3_digital_033);
		VPORT_Wr01(ch3_digital_033, ch3reg_b_compare, 512<<14);
		VPORT_WrFL(ch3_digital_033);
#endif

	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_031.ch3reg_r_compare = target_ire<<14;
#else
		VPORT_RdFL(ch3_digital_031);
		VPORT_Wr01(ch3_digital_031, ch3reg_r_compare, target_ire<<14);
		VPORT_WrFL(ch3_digital_031);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_032.ch3reg_g_compare = target_ire<<14;
#else
		VPORT_RdFL(ch3_digital_032);
		VPORT_Wr01(ch3_digital_032, ch3reg_g_compare, target_ire<<14);
		VPORT_WrFL(ch3_digital_032);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_033.ch3reg_b_compare = target_ire<<14;
#else
		VPORT_RdFL(ch3_digital_033);
		VPORT_Wr01(ch3_digital_033, ch3reg_b_compare, target_ire<<14);
		VPORT_WrFL(ch3_digital_033);
#endif

	}
#endif

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

int ADC_SetAgocWakeup_Bx(void)
{

	giAgocIntDone = 1;
	wake_up_interruptible(&agoc_done);

	return 0;
}

int ADC_GetGainRGB_Bx( ST_GAIN_INFORM *gain)
{
	gain->gain_R = gVportRegBx->ch3_adc_008.afe3ch_gin_r;
	gain->gain_G = gVportRegBx->ch3_adc_009.afe3ch_gin_g;
	gain->gain_B = gVportRegBx->ch3_adc_010.afe3ch_gin_b;

	return 0;

}

int ADC_SetGainRGB_Bx( ST_GAIN_INFORM *gain)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_008.afe3ch_gin_r = gain->gain_R;
#else
	VPORT_RdFL(ch3_adc_008);
	VPORT_Wr01(ch3_adc_008, afe3ch_gin_r, gain->gain_R);
	VPORT_WrFL(ch3_adc_008);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_009.afe3ch_gin_g = gain->gain_G;
#else
	VPORT_RdFL(ch3_adc_009);
	VPORT_Wr01(ch3_adc_009, afe3ch_gin_g, gain->gain_G);
	VPORT_WrFL(ch3_adc_009);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_010.afe3ch_gin_b = gain->gain_B;
#else
	VPORT_RdFL(ch3_adc_010);
	VPORT_Wr01(ch3_adc_010, afe3ch_gin_b, gain->gain_B);
	VPORT_WrFL(ch3_adc_010);
#endif


	return 0;
}


int ADC_GetOffsetRGB_Bx( S_OFFSET_INFORM *offset)
{
	offset->offset_R = gVportRegBx->ch3_adc_011.ch3reg_afe3ch_oftin_r;
	offset->offset_G = gVportRegBx->ch3_adc_012.ch3reg_afe3ch_oftin_g;
	offset->offset_B = gVportRegBx->ch3_adc_013.ch3reg_afe3ch_oftin_b;

	return 0;

}

int ADC_SetOffsetRGB_Bx( S_OFFSET_INFORM *offset)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_011.ch3reg_afe3ch_oftin_r = offset->offset_R;
#else
	VPORT_RdFL(ch3_adc_011);
	VPORT_Wr01(ch3_adc_011, ch3reg_afe3ch_oftin_r, offset->offset_R);
	VPORT_WrFL(ch3_adc_011);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_012.ch3reg_afe3ch_oftin_g = offset->offset_G;
#else
	VPORT_RdFL(ch3_adc_012);
	VPORT_Wr01(ch3_adc_012, ch3reg_afe3ch_oftin_g, offset->offset_G);
	VPORT_WrFL(ch3_adc_012);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_adc_013.ch3reg_afe3ch_oftin_b = offset->offset_B;
#else
	VPORT_RdFL(ch3_adc_013);
	VPORT_Wr01(ch3_adc_013, ch3reg_afe3ch_oftin_b, offset->offset_B);
	VPORT_WrFL(ch3_adc_013);
#endif


	return 0;
}


int ADC_GetSumRGB_Bx(S_RGB_SUM *sum)
{

	sum->sum_R = gVportRegBx->ch3_digital_055.ch3reg_ap_sum_r;

	sum->sum_G = gVportRegBx->ch3_digital_056.ch3reg_ap_sum_g;

	sum->sum_B = gVportRegBx->ch3_digital_057.ch3reg_ap_sum_b;

	return 0;
}

/**
 *
 * Enable SCART RGB input on component 2(from B0 revision board) port for EU model
 *
 * @param	BOOLEAN scart_enable(TRUE:Enable SCART RGB, FALSE:component mode)
 *
*/
int ADC_Enable_Scart_Mode_Bx(BOOLEAN scart_enable)
{
//	UINT32	VP2_Intr_Enable_Mask = 0xFFFFFBFC; // Disable Resolution change & SOY1 detection interrupt.
//	static UINT32 	VP2_Intr_Enable_Save = 0;

	ADC_DEBUG("%s entered with value [%d]\n", __func__, scart_enable);

	if (scart_enable == TRUE)
	{
		//Interrupt Disable for component1
#if 0
		VP2_Intr_Enable_Save = gVportRegBx->intr_vp2_enable.intr_vp2_enable;
		gVportRegBx->intr_vp2_enable.intr_vp2_enable = VP2_Intr_Enable_Save & VP2_Intr_Enable_Mask;
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_004.afe3ch_selmux = 0x1; // select component 2 port for ADC
#else
		VPORT_RdFL(ch3_adc_004);
		VPORT_Wr01(ch3_adc_004, afe3ch_selmux, 0x1);
		VPORT_WrFL(ch3_adc_004);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_cst_005.cst_selmux = 0x1; // select component 2 port for ADC
#else
		VPORT_RdFL(ch3_cst_005);
		VPORT_Wr01(ch3_cst_005, cst_selmux, 0x1);
		VPORT_WrFL(ch3_cst_005);
#endif

	//	gVportRegBx->ch3_adc_003.afe3ch_sel_ck = 0x1; // select clock from CVD

		ADC_SetType(LX_ADC_INPUT_SOURCE_RGB_SCART); // for SCART RGB set port type to RGB
	}
	else
	{
		// No need to set interrupt enable for the scart input.
#if 0
		if(VP2_Intr_Enable_Save)
			gVportRegBx->intr_vp2_enable.intr_vp2_enable = VP2_Intr_Enable_Save;
#endif

		// Port selection should be called from DDI
		//gVportRegBx->ch3_adc_004.afe3ch_selmux = 0x0; // select component 1 port for ADC

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_003.afe3ch_sel_ck = 0x0; // select clock from LLPLL
#else
		VPORT_RdFL(ch3_adc_003);
		VPORT_Wr01(ch3_adc_003, afe3ch_sel_ck, 0x0);
		VPORT_WrFL(ch3_adc_003);
#endif

	}
	return 0;
}

#if 0
// should be removed
int ADC_SetHorTotal_Bx(unsigned short value)
{
	return 0;
}



int ADC_SetVerPos_Bx(unsigned short pos)
{
	return 0;
}

int ADC_SetHorPos_Bx(unsigned short pos)
{
	return 0;

}

int ADC_GetI2CData_Bx(unsigned int sub , unsigned int addr , unsigned int size )
{
	return 0;
}
int ADC_SetI2CData_Bx(unsigned int sub , unsigned int addr , unsigned int size , unsigned int value)
{
	return 0;
}

int ADC_SetDE_Bx(int iIndex , unsigned int extau_up , unsigned int extau_left)
{

	return 0;
}
#endif
void ADC_Vport_ALL_Power_Down_Bx(void)
{

	/* HDMI PHY Power Down Setting */

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_010.reg_swrst_linksys = 0;
#else
	VPORT_RdFL(top_010);
	VPORT_Wr01(top_010, reg_swrst_linksys, 0);
	VPORT_WrFL(top_010);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_009.reg_swrst_p3 = 0;
#else
	VPORT_RdFL(top_009);
	VPORT_Wr01(top_009, reg_swrst_p3, 0);
	VPORT_WrFL(top_009);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_009.reg_swrst_p1 = 0;
#else
	VPORT_RdFL(top_009);
	VPORT_Wr01(top_009, reg_swrst_p1, 0);
	VPORT_WrFL(top_009);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_004.reg_swrst_exta = 0;
#else
	VPORT_RdFL(top_004);
	VPORT_Wr01(top_004, reg_swrst_exta, 0);
	VPORT_WrFL(top_004);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_004.reg_swrst_extb = 0;
#else
	VPORT_RdFL(top_004);
	VPORT_Wr01(top_004, reg_swrst_extb, 0);
	VPORT_WrFL(top_004);
#endif


	/*CVBS ADC Power Down Setting */

	// This is CVD Power Down
#if 0
#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_002.cvbs_pdbm = 0x0;
#else
	VPORT_RdFL(cvbsafe_002);
	VPORT_Wr01(cvbsafe_002, cvbs_pdbm, 0x0);
	VPORT_WrFL(cvbsafe_002);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_002.cvbs1_pdb = 0x0;
#else
	VPORT_RdFL(cvbsafe_002);
	VPORT_Wr01(cvbsafe_002, cvbs1_pdb, 0x0);
	VPORT_WrFL(cvbsafe_002);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_002.cvbs2_pdb = 0x0;
#else
	VPORT_RdFL(cvbsafe_002);
	VPORT_Wr01(cvbsafe_002, cvbs2_pdb, 0x0);
	VPORT_WrFL(cvbsafe_002);
#endif


	/* VDAC & BUFFER Power Down Setting */

#if VPORT_REG_DIRECT_ACCESS
	  gVportRegBx->cvbsafe_001.vdac_pdb = 0x0;
#else
	  VPORT_RdFL(cvbsafe_001);
	  VPORT_Wr01(cvbsafe_001, vdac_pdb, 0x0);
	  VPORT_WrFL(cvbsafe_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	  gVportRegBx->cvbsafe_001.vdicnt = 0x3; //dws
#else
	  VPORT_RdFL(cvbsafe_001);
	  VPORT_Wr01(cvbsafe_001, vdicnt, 0x3);
	  VPORT_WrFL(cvbsafe_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	  gVportRegBx->cvbsafe_001.buf_pdb1 = 0x0;
#else
	  VPORT_RdFL(cvbsafe_001);
	  VPORT_Wr01(cvbsafe_001, buf_pdb1, 0x0);
	  VPORT_WrFL(cvbsafe_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	  gVportRegBx->cvbsafe_001.buf_pdb2 = 0x0;
#else
	  VPORT_RdFL(cvbsafe_001);
	  VPORT_Wr01(cvbsafe_001, buf_pdb2, 0x0);
	  VPORT_WrFL(cvbsafe_001);
#endif

#endif

	  /* AFE3CH Power Down Setting */
	//gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0x0;
//	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0;

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_vref_pdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif




#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_bpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_bpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_gpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_gpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_rpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_rpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


	 /* LLPLL PDB */

#if VPORT_REG_DIRECT_ACCESS
	 gVportRegBx->top_003.reg_cvbspll_pdb = 0;
#else
	 VPORT_RdFL(top_003);
	 VPORT_Wr01(top_003, reg_cvbspll_pdb, 0);
	 VPORT_WrFL(top_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
	 gVportRegBx->top_003.reg_ch3pll_pdb = 0;
#else
	 VPORT_RdFL(top_003);
	 VPORT_Wr01(top_003, reg_ch3pll_pdb, 0);
	 VPORT_WrFL(top_003);
#endif


	 /* LLPLL/CST SWRST & 3CH_DIG SWRST */

#if VPORT_REG_DIRECT_ACCESS
	 gVportRegBx->top_008.reg_swrst_llpll = 0x1;
#else
	 VPORT_RdFL(top_008);
	 VPORT_Wr01(top_008, reg_swrst_llpll, 0x1);
	 VPORT_WrFL(top_008);
#endif


#if VPORT_REG_DIRECT_ACCESS
	 gVportRegBx->top_008.reg_swrst_ch3pix = 0x1;
#else
	 VPORT_RdFL(top_008);
	 VPORT_Wr01(top_008, reg_swrst_ch3pix, 0x1);
	 VPORT_WrFL(top_008);
#endif

}

int ADC_Channel_Power_Control_Bx(int vref_pdb, int bpdb, int gpdb, int rpdb)
{
//	AFE_PRINT("ADC Channel Power Control vfref_pdb[%d], bpdb[%d], gpdb[%d], rpdb[%d]\n", vref_pdb, bpdb, gpdb, rpdb);

	VPORT_RdFL(cvbsafe_pdb_001);
	if(vref_pdb >= 0)
		VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, vref_pdb);
	if(bpdb >= 0)
		VPORT_Wr01(cvbsafe_pdb_001, afe3ch_bpdb, bpdb);
	if(gpdb >= 0)
		VPORT_Wr01(cvbsafe_pdb_001, afe3ch_gpdb, gpdb);
	if(rpdb >= 0)
		VPORT_Wr01(cvbsafe_pdb_001, afe3ch_rpdb, rpdb);
	VPORT_WrFL(cvbsafe_pdb_001);

	return 0;
}

int ADC_SetType_Bx(LX_AFE_ADC_INPUT_SOURCE_TYPE_T sel_type)
{

	//AFE_PRINT("ADC_SetType \n");
	gADC_type = sel_type;

	if(sel_type == LX_ADC_INPUT_SOURCE_YPBPR)
	{
		ADC_DEBUG("Component Input Mode\n");

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_001.afe3ch_bmidsel = 0x1;
#else
		VPORT_RdFL(ch3_adc_001);
		VPORT_Wr01(ch3_adc_001, afe3ch_bmidsel, 0x1);
		VPORT_WrFL(ch3_adc_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_001.afe3ch_gmidsel = 0x0;
#else
		VPORT_RdFL(ch3_adc_001);
		VPORT_Wr01(ch3_adc_001, afe3ch_gmidsel, 0x0);
		VPORT_WrFL(ch3_adc_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_001.afe3ch_rmidsel = 0x1;
#else
		VPORT_RdFL(ch3_adc_001);
		VPORT_Wr01(ch3_adc_001, afe3ch_rmidsel, 0x1);
		VPORT_WrFL(ch3_adc_001);
#endif


		//gVportRegBx->ch3_digital_002.ch3reg_clamp_base = 0x3;	// SC jung wrote

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_002.ch3reg_clamp_base = 0x2;	// 0908 move to backporch
#else
		VPORT_RdFL(ch3_digital_002);
		VPORT_Wr01(ch3_digital_002, ch3reg_clamp_base, 0x2);
		VPORT_WrFL(ch3_digital_002);
#endif




#if VPORT_REG_DIRECT_ACCESS
 		//gVportRegBx->ch3_digital_002.ch3reg_clamp_width = 0x5;
 _iow(&gVportRegBx->ch3_digital_002 , 8 , 0 , 0x05);	// 0908 move to backporch
#else
 VPORT_RdFL(ch3_digital_002);
 VPORT_Wr01(ch3_digital_002, ch3reg_clamp_width, 0x5);
 VPORT_WrFL(ch3_digital_002);
#endif


#if VPORT_REG_DIRECT_ACCESS
//_iow(&gVportRegBx->ch3_digital_002 , 8 , 8 , 0x2c);	// 0908 move to backporch
		_iow(&gVportRegBx->ch3_digital_002 , 8 , 8 , 0x40);	// 1102
		// gVportRegBx->ch3_digital_002.ch3reg_clamp_place = 0x40;
#else
 VPORT_RdFL(ch3_digital_002);
 VPORT_Wr01(ch3_digital_002, ch3reg_clamp_place, 0x40);
 VPORT_WrFL(ch3_digital_002);
#endif


		//added 0716

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_004.ch3reg_hsout_width_sel = 0x0; // Select Hsync output sync width from hsout_width register
#else
		VPORT_RdFL(ch3_digital_004);
		VPORT_Wr01(ch3_digital_004, ch3reg_hsout_width_sel, 0x0);
		VPORT_WrFL(ch3_digital_004);
#endif

		//gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block

		//0906
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x0;	// RGB shaking problem => H positon shift problem
		//1210 powerory

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_065.ch3reg_vsout_sel = 0x0;
#else
		VPORT_RdFL(ch3_digital_065);
		VPORT_Wr01(ch3_digital_065, ch3reg_vsout_sel, 0x0);
		VPORT_WrFL(ch3_digital_065);
#endif

		//If vsout_sel is 0(vs_gen), sometimes vertical sync is not lock to video. Need more debuggint.
		//1214 powerory

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_064.ch3reg_vsdiff_thr = 0x80;
#else
		VPORT_RdFL(ch3_digital_064);
		VPORT_Wr01(ch3_digital_064, ch3reg_vsdiff_thr, 0x80);
		VPORT_WrFL(ch3_digital_064);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_064.ch3reg_vsprdeq_thr = 0x0e;
#else
		VPORT_RdFL(ch3_digital_064);
		VPORT_Wr01(ch3_digital_064, ch3reg_vsprdeq_thr, 0x0e);
		VPORT_WrFL(ch3_digital_064);
#endif


		//20110114
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x2;
//		gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 0;

		//20110124 powerory (480i 입력에서 문제점해결위해 수정)

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x0;
#else
		VPORT_RdFL(ch3_digital_063);
		VPORT_Wr01(ch3_digital_063, ch3reg_hsdiff_thr, 0x0);
		VPORT_WrFL(ch3_digital_063);
#endif

		//20110125
		//gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x0; // Select feedback LLPLL Hsync for digital block
#else
		VPORT_RdFL(ch3_digital_005);
		VPORT_Wr01(ch3_digital_005, ch3reg_hsync_sel, 0x0);
		VPORT_WrFL(ch3_digital_005);
#endif

	}
	else
	{ ADC_DEBUG("RGB Input Mode\n");

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_001.afe3ch_bmidsel = 0x0;
#else
		VPORT_RdFL(ch3_adc_001);
		VPORT_Wr01(ch3_adc_001, afe3ch_bmidsel, 0x0);
		VPORT_WrFL(ch3_adc_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_001.afe3ch_gmidsel = 0x0;
#else
		VPORT_RdFL(ch3_adc_001);
		VPORT_Wr01(ch3_adc_001, afe3ch_gmidsel, 0x0);
		VPORT_WrFL(ch3_adc_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_001.afe3ch_rmidsel = 0x0;
#else
		VPORT_RdFL(ch3_adc_001);
		VPORT_Wr01(ch3_adc_001, afe3ch_rmidsel, 0x0);
		VPORT_WrFL(ch3_adc_001);
#endif



#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_002.ch3reg_clamp_base = 0x2;	// SC jung wrote
#else
		VPORT_RdFL(ch3_digital_002);
		VPORT_Wr01(ch3_digital_002, ch3reg_clamp_base, 0x2);
		VPORT_WrFL(ch3_digital_002);
#endif


		//_iow(&gVportRegBx->ch3_digital_002 , 8 , 0 , 0x14);
		_iow(&gVportRegBx->ch3_digital_002 , 8 , 0 , 0x5);	//1102
		//
#if VPORT_REG_DIRECT_ACCESS
//_iow(&gVportRegBx->ch3_digital_002 , 8 , 0 , 0x14);
		_iow(&gVportRegBx->ch3_digital_002 , 8 , 0 , 0x5);	//1102
		// gVportRegBx->ch3_digital_002.ch3reg_clamp_width = 0x05;
#else
 VPORT_RdFL(ch3_digital_002);
 VPORT_Wr01(ch3_digital_002, ch3reg_clamp_width, 0x05);
 VPORT_WrFL(ch3_digital_002);
#endif


#if VPORT_REG_DIRECT_ACCESS
		//_iow(&gVportRegBx->ch3_digital_002 , 8 , 8 , 0x9);
		_iow(&gVportRegBx->ch3_digital_002 , 8 , 8 , 0x10); //1102
		// gVportRegBx->ch3_digital_002.ch3reg_clamp_place = 0x10;
#else
 VPORT_RdFL(ch3_digital_002);
 VPORT_Wr01(ch3_digital_002, ch3reg_clamp_place, 0x10);
 VPORT_WrFL(ch3_digital_002);
#endif


		//added 0716
		//gVportRegBx->ch3_digital_004.ch3reg_hsout_width_sel = 0x1; // Select Auto Hsync output sync width

		// for horizontal sync re-shape in ADC

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_004.ch3reg_hsout_width_sel = 0x0; // Select Hsync output sync width from hsout_width register
#else
		VPORT_RdFL(ch3_digital_004);
		VPORT_Wr01(ch3_digital_004, ch3reg_hsout_width_sel, 0x0);
		VPORT_WrFL(ch3_digital_004);
#endif

//		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x0; // Select internal Hsync for digital block
//		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // powerory  <== CAN NOT CHANGE HSIZE BY 1 STEP
//		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // B0 Revision Fixed bug(HSIZE 1 STEP)

		//0906
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x2;	// RGB shaking problem => H positon shift problem
		//1210 powerory
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x0;

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_065.ch3reg_vsout_sel = 0x1;
#else
		VPORT_RdFL(ch3_digital_065);
		VPORT_Wr01(ch3_digital_065, ch3reg_vsout_sel, 0x1);
		VPORT_WrFL(ch3_digital_065);
#endif


		//20110114
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x2;

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x0;
#else
		VPORT_RdFL(ch3_digital_063);
		VPORT_Wr01(ch3_digital_063, ch3reg_hsdiff_thr, 0x0);
		VPORT_WrFL(ch3_digital_063);
#endif

//		gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 1;
//		gVportRegBx->ch3_digital_003.ch3reg_hsoutcnt = gVportRegBx->ch3_llpll_003.llpll_counter_max - 1;
		//20110125

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block
#else
		VPORT_RdFL(ch3_digital_005);
		VPORT_Wr01(ch3_digital_005, ch3reg_hsync_sel, 0x1);
		VPORT_WrFL(ch3_digital_005);
#endif

	}

	return 0;
}

int ADC_Set_LLPLL_Bx(UINT32 arg)
{

	if(arg >= PARAM_NUM)
		return -1;

		ADC_DEBUG("Setting LLPLL to Index Number [%d]\n", arg);

	//ADC_SetInputVideo(arg);

	if(ADC_SetInputVideo(arg) == 0)
		gPrevious_index = 0xffff;

	return 0;

}

int ADC_SetClock_Bx(int iIndex, SINT16 value)
{
	UINT16 clock_value;
#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND_2
	UINT8 dco_max_value, dco_min_value;
#endif
//	AFE_PRINT("%s entered\n", __func__);
	//LLPLL_counter_max
	clock_value = LLPLL_counter_max[iIndex];

//	AFE_PRINT("Clock Value =[%d +(%d)], \n", clock_value, value);

	// No Need to Write Same Value
	/*
	if(gVportRegBx->ch3_llpll_003.llpll_counter_max == (clock_value + value))
		return 0;
		*/

#ifdef	KDRV_CONFIG_PM
	gAdcPMData->giPmClockValue = value;
#endif
	clock_value += value;
	//ADC_Ext_Clamp(1);

//	gVportRegBx->top_008.reg_swrst_ch3pix = 0x1;

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_llpll_003.llpll_counter_max = clock_value;
#else
	VPORT_RdFL(ch3_llpll_003);
	VPORT_Wr01(ch3_llpll_003, llpll_counter_max, clock_value);
	VPORT_WrFL(ch3_llpll_003);
#endif

	mdelay(10);

#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND
	ADC_Channel_Power_Control_Bx(-1, 0, -1, 0);
#endif

#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND_2
	ADC_Get_DCO_MinMax_Value_Bx(&dco_min_value, &dco_max_value);
	ADC_Set_DCO_MinMax_Value_Bx(0, 0);
#endif

	mdelay(10);
#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND
	ADC_Channel_Power_Control_Bx(-1, 1, -1, 1);
#endif
#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND_2
	ADC_Set_DCO_MinMax_Value_Bx(dco_min_value, dco_max_value);
#endif

	mdelay(30);
//	gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 1;
//	gVportRegBx->ch3_digital_003.ch3reg_hsoutcnt = gVportRegBx->ch3_llpll_003.llpll_counter_max - 1;
//	gVportRegBx->top_008.reg_swrst_ch3pix = 0x0;
	//ADC_Ext_Clamp(0);
//	ADC_Reset_Digital_Bx();

	return 0;
}

int ADC_Reset_Digital_24MHZ_Bx(void)
{

	AFE_PRINT("%s entered. This function empty for this chipset[L8Bx].\n", __func__);

	return 0;
}

int ADC_Reset_Digital_Bx(void)
{
#if 0
#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND_2
	UINT8 dco_max_value, dco_min_value;
#endif
#endif
	ADC_DEBUG("%s entered\n", __func__);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_ch3pix = 0x1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_ch3pix, 0x1);
	VPORT_WrFL(top_008);
#endif

	mdelay(20);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_ch3pix = 0x0;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_ch3pix, 0x0);
	VPORT_WrFL(top_008);
#endif

#if 0
#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND
	ADC_Channel_Power_Control_Bx(-1, 0, -1, 0);
#endif
#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND_2
	ADC_Get_DCO_MinMax_Value_Bx(&dco_min_value, &dco_max_value);
	ADC_Set_DCO_MinMax_Value_Bx(0, 0);
#endif

	mdelay(5);

#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND
	ADC_Channel_Power_Control_Bx(-1, 1, -1, 1);
#endif
#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND_2
	ADC_Set_DCO_MinMax_Value_Bx(dco_min_value, dco_max_value);
#endif
#endif

	return 0;
}

int ADC_Reset_LLPLL_Bx(void)
{
	ADC_DEBUG("%s entered\n", __func__);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 0x1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_llpll, 0x1);
	VPORT_WrFL(top_008);
#endif

	mdelay(5);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 0x0;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_llpll, 0x0);
	VPORT_WrFL(top_008);
#endif


	return 0;
}

int ADC_Sync_Polarity_Override_Bx(int set)
{
	ADC_DEBUG("%s entered\n", __func__);
	if(set)
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_cst_003.cst_hspolover = 0x1; // 0825 new value from powerory
#else
		VPORT_RdFL(ch3_cst_003);
		VPORT_Wr01(ch3_cst_003, cst_hspolover, 0x1);
		VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_cst_003.cst_vspolover = 0x1; // 0825 new value from powerory
#else
		VPORT_RdFL(ch3_cst_003);
		VPORT_Wr01(ch3_cst_003, cst_vspolover, 0x1);
		VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_cst_003.cst_hspoluser = 0x1; // 0825 new value from powerory
#else
		VPORT_RdFL(ch3_cst_003);
		VPORT_Wr01(ch3_cst_003, cst_hspoluser, 0x1);
		VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_cst_003.cst_vspoluser = 0x1; // 0825 new value from powerory
#else
		VPORT_RdFL(ch3_cst_003);
		VPORT_Wr01(ch3_cst_003, cst_vspoluser, 0x1);
		VPORT_WrFL(ch3_cst_003);
#endif

	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_cst_003.cst_hspolover = 0x0; // 0825 new value from powerory
#else
		VPORT_RdFL(ch3_cst_003);
		VPORT_Wr01(ch3_cst_003, cst_hspolover, 0x0);
		VPORT_WrFL(ch3_cst_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_cst_003.cst_vspolover = 0x0; // 0825 new value from powerory
#else
		VPORT_RdFL(ch3_cst_003);
		VPORT_Wr01(ch3_cst_003, cst_vspolover, 0x0);
		VPORT_WrFL(ch3_cst_003);
#endif

//		gVportRegBx->ch3_cst_003.cst_hspoluser = 0x0; // 0825 new value from powerory
//		gVportRegBx->ch3_cst_003.cst_vspoluser = 0x0; // 0825 new value from powerory
	}

	return 0;
}



int ADC_Check_Sync_Exist_Bx(void)
{

	UINT8	selmux_value , soy0det_value, soy1det_value, sogdet_value, vsdet_value, hsdet_value;

#if VPORT_REG_DIRECT_ACCESS
	if(((gVportRegBx->ch3_adc_004.afe3ch_selmux == 0x0)&&(gVportRegBx->ch3_cst_006.cst_ro_soy0det == 0)) ||	\
		((gVportRegBx->ch3_adc_004.afe3ch_selmux == 0x1)&&(gVportRegBx->ch3_cst_006.cst_ro_soy1det == 0)) ||	\
		(((gVportRegBx->ch3_adc_004.afe3ch_selmux & 0x2) == 0x2)&&(gVportRegBx->ch3_cst_006.cst_ro_sogdet == 0)&&(gVportRegBx->ch3_cst_006.cst_ro_hsdet == 0)&&(gVportRegBx->ch3_cst_006.cst_ro_vsdet == 0)))
	{
		return -1;
	}
#else
	VPORT_RdFL(ch3_adc_004);
	VPORT_Rd01(ch3_adc_004,  afe3ch_selmux, selmux_value);

	VPORT_RdFL(ch3_cst_006);
	VPORT_Rd05(ch3_cst_006,  cst_ro_soy0det, soy0det_value, cst_ro_soy1det, soy1det_value, cst_ro_sogdet, sogdet_value, cst_ro_hsdet, hsdet_value, cst_ro_vsdet, vsdet_value);

	if(((selmux_value == 0x0)&&(soy0det_value == 0)) ||	\
		((selmux_value == 0x1)&&(soy1det_value == 0)) ||	\
		(((selmux_value & 0x2) == 0x2)&&(sogdet_value == 0)&&(hsdet_value == 0)&&(vsdet_value == 0)))
	{
		return -1;
	}
#endif
	return 0;
}

int ADC_Enable_Detection_Interrupt_Bx(int enable)
{
	ADC_DEBUG("%s entered\n", __func__);
	if(enable)
		gVportRegBx->intr_vp2_enable.intr_vp2_enable = gVportRegBx->intr_vp2_enable.intr_vp2_enable | ADC_DETECTION_INTERRUPT_MASK;
	else
		gVportRegBx->intr_vp2_enable.intr_vp2_enable = gVportRegBx->intr_vp2_enable.intr_vp2_enable & (~ADC_DETECTION_INTERRUPT_MASK);

	return 0;
}

int ADC_Enable_AOGC_Interrupt_Bx(int enable)
{
	ADC_DEBUG("%s entered\n", __func__);
	if(enable)
		gVportRegBx->intr_vp2_enable.intr_vp2_enable = gVportRegBx->intr_vp2_enable.intr_vp2_enable | ADC_AOGC_INTERRUPT_MASK;
	else
		gVportRegBx->intr_vp2_enable.intr_vp2_enable = gVportRegBx->intr_vp2_enable.intr_vp2_enable & (~ADC_AOGC_INTERRUPT_MASK);

	return 0;
}

int ADC_Mute_Enable_Bx(int enable)
{
	ADC_DEBUG("%s entered [%d]\n", __func__, enable);
	if(enable)
	{
		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{

#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->ch3_digital_005.ch3reg_signedout = 5;
#else
			VPORT_RdFL(ch3_digital_005);
			VPORT_Wr01(ch3_digital_005, ch3reg_signedout, 5);
			VPORT_WrFL(ch3_digital_005);
#endif

		}

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_005.afe3ch_r_ph =0x3;
#else
		VPORT_RdFL(ch3_adc_005);
		VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x3);
		VPORT_WrFL(ch3_adc_005);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_007.ch3reg_adc_ire_test = 0x1;
#else
		VPORT_RdFL(ch3_adc_007);
		VPORT_Wr01(ch3_adc_007, ch3reg_adc_ire_test, 0x1);
		VPORT_WrFL(ch3_adc_007);
#endif

	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_005.ch3reg_signedout = 0;
#else
		VPORT_RdFL(ch3_digital_005);
		VPORT_Wr01(ch3_digital_005, ch3reg_signedout, 0);
		VPORT_WrFL(ch3_digital_005);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_005.afe3ch_r_ph =0x0;
#else
		VPORT_RdFL(ch3_adc_005);
		VPORT_Wr01(ch3_adc_005, afe3ch_r_ph, 0x0);
		VPORT_WrFL(ch3_adc_005);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_007.ch3reg_adc_ire_test = 0x0;
#else
		VPORT_RdFL(ch3_adc_007);
		VPORT_Wr01(ch3_adc_007, ch3reg_adc_ire_test, 0x0);
		VPORT_WrFL(ch3_adc_007);
#endif

	}

	return 0;
}

int ADC_Power_OnOff_Bx(void)
{
	ADC_DEBUG("%s entered\n", __func__);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_biaspdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_vref_pdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_bpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_bpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_gpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_gpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_rpdb = 0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_rpdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


	//gVportRegBx->ch3_digital_002.ch3reg_extclampsel = 1;
	//gVportRegBx->ch3_digital_002.ch3reg_clamppolover = 1;
	//gVportRegBx->ch3_digital_002.ch3reg_clamppoluser = 1;


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_biaspdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


	mdelay(5);


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_vref_pdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_bpdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_bpdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_gpdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_gpdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_rpdb = 1;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_rpdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_llpll, 1);
	VPORT_WrFL(top_008);
#endif

	mdelay(5);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 0;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr01(top_008, reg_swrst_llpll, 0);
	VPORT_WrFL(top_008);
#endif


	return 0;
}

int ADC_Ext_Clamp_Bx(int sel_ext)
{
//	static int g_ph_value = 4;

	ADC_DEBUG("%s entered [%d]\n", __func__, sel_ext);
	if(sel_ext)
	{
		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{
//			ADC_Channel_Power_Control_Bx(0);
	//		gVportRegBx->top_008.reg_swrst_llpll = 0x1;
//			gVportRegBx->top_008.reg_swrst_ch3pix = 0x1;
			/*
			VPORT_RdFL(ch3_adc_005);
			VPORT_Wr01(ch3_adc_005, afe3ch_g_ph, 1);
			VPORT_WrFL(ch3_adc_005);
			AFE_PRINT("Set g_ph value to 0 \n");
			*/
		}

//		mdelay(10);


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_015.afe3ch_envolclp = 0;
#else
		VPORT_RdFL(ch3_adc_015);
		VPORT_Wr01(ch3_adc_015, afe3ch_envolclp, 0);
		VPORT_WrFL(ch3_adc_015);
#endif


		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{
//			mdelay(5);
//			ADC_Channel_Power_Control_Bx(1);
//			mdelay(5);
		}
		// powerory
		//gVportRegBx->ch3_digital_002.ch3reg_extclampsel = 1;
		//gVportRegBx->ch3_digital_002.ch3reg_clamppolover = 1;
		//gVportRegBx->ch3_digital_002.ch3reg_clamppoluser = 1;
		//~powerory
#if 0
		if(gADC_type != LX_ADC_INPUT_SOURCE_YPBPR)	// 1019 Workaround for CVI Vsync Unstable on component input(VENICE 1080i)
			gVportRegBx->top_008.reg_swrst_ch3pix = 0x1;
		//mdelay(1000);
#endif
	}
	else
	{
		// powerory
		//gVportRegBx->ch3_digital_002.ch3reg_extclampsel = 0;
		//gVportRegBx->ch3_digital_002.ch3reg_clamppolover = 0;
		//gVportRegBx->ch3_digital_002.ch3reg_clamppoluser = 0;
		//~powerory
		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{
//			mdelay(5);
	//		gVportRegBx->top_008.reg_swrst_llpll = 0x0;
//			mdelay(300);
//			gVportRegBx->top_008.reg_swrst_ch3pix = 0x0;
			/*
			VPORT_RdFL(ch3_adc_005);
			VPORT_Wr01(ch3_adc_005, afe3ch_g_ph, g_ph_value);
			VPORT_WrFL(ch3_adc_005);
			AFE_PRINT("Restore g_ph value to [%d]\n",g_ph_value);
			*/
		}
		//mdelay(30);


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_adc_015.afe3ch_envolclp = 1;
#else
		VPORT_RdFL(ch3_adc_015);
		VPORT_Wr01(ch3_adc_015, afe3ch_envolclp, 1);
		VPORT_WrFL(ch3_adc_015);
#endif


//		mdelay(10);

		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{
//			gVportRegBx->top_008.reg_swrst_ch3pix = 0x0;
		}
#if 0
		if(gADC_type != LX_ADC_INPUT_SOURCE_YPBPR)
		{
		//	gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 0;
#if 0
			gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 1;
			gVportRegBx->ch3_digital_003.ch3reg_hsoutcnt = gVportRegBx->ch3_llpll_003.llpll_counter_max - 1;

			gVportRegBx->top_008.reg_swrst_ch3pix = 0x0;// 1019 Workaround for CVI Vsync Unstable on component input(VENICE 1080i)
#endif
	//	ADC_Reset_Digital();

		}
		else
		{
			//gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 1;
			//gVportRegBx->ch3_digital_003.ch3reg_hsoutcnt = gVportRegBx->ch3_llpll_003.llpll_counter_max - 1;

		}
#endif
		/*
		else
			gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 0;
			*/

//		gVportRegBx->top_008.reg_swrst_ch3pix = 0x0;// 1019 Workaround for CVI Vsync Unstable on component input(VENICE 1080i)

	}

	return 0;
}

int ADC_Set_Clamp_Bx(UINT8 clamp_base, UINT8 clamp_place, UINT8 clamp_width)
{
	if(clamp_base)
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->ch3_digital_002.ch3reg_clamp_base = clamp_base;
#else
		VPORT_RdFL(ch3_digital_002);
		VPORT_Wr01(ch3_digital_002, ch3reg_clamp_base, clamp_base);
		VPORT_WrFL(ch3_digital_002);
#endif

}
	if(clamp_place)
	{
#if VPORT_REG_DIRECT_ACCESS
		_iow(&gVportRegBx->ch3_digital_002 , 8 , 8 , clamp_place);
#else
 VPORT_RdFL(ch3_digital_002);
 VPORT_Wr01(ch3_digital_002, ch3reg_clamp_place, clamp_place);
 VPORT_WrFL(ch3_digital_002);
#endif
}
	if(clamp_width)
	{
#if VPORT_REG_DIRECT_ACCESS
		_iow(&gVportRegBx->ch3_digital_002 , 8 , 0 , clamp_width);
#else
 VPORT_RdFL(ch3_digital_002);
 VPORT_Wr01(ch3_digital_002, ch3reg_clamp_width, clamp_width);
 VPORT_WrFL(ch3_digital_002);
#endif
}

	return 0;
}

#ifdef	KDRV_CONFIG_PM
int ADC_RunSuspend_Bx(void)
{
	// confirm initialization done
	if(gVportRegBx == NULL)
		return -1;

	// TAU inform
	gAdcPMData->gstPmExtauValue.height_offset = gVportRegBx->ch3_digital_026.ch3reg_extau_down_offset;
	gAdcPMData->gstPmExtauValue.width_offset = gVportRegBx->ch3_digital_028.ch3reg_extau_right_offset;
	gAdcPMData->gstPmExtauValue.width_offset = gVportRegBx->ch3_digital_023.ch3reg_extau_ctrl;

	//
	ADC_GetRGBPosition(&gAdcPMData->gstPmPositionValue);
	// phase
	gAdcPMData->guiPmPhaseValue = ADC_GetPhaseValue();

	ADC_GetGainValue(&gAdcPMData->gstPmGainValue);

	ADC_GetOffsetValue(&gAdcPMData->gstPmOffsetValue);


	gAdcPMData->guiPmInterruptStatus = gVportRegBx->intr_vp2_enable.intr_vp2_enable;

	ADC_Vport_ALL_Power_Down();

	return 0;
}

int ADC_RunResume_Bx(void)
{

	ADC_InitDaughterBoard();

	if(gAdcPMData->gstPmSelPortValue.port == 0xfffffff)
		return 0;




#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->ch3_digital_026.ch3reg_extau_down_offset = gAdcPMData->gstPmExtauValue.height_offset;
#else
	VPORT_RdFL(ch3_digital_026);
	VPORT_Wr01(ch3_digital_026, ch3reg_extau_down_offset, gAdcPMData->gstPmExtauValue.height_offset);
	VPORT_WrFL(ch3_digital_026);
#endif

	gVportRegBx->ch3_digital_028.ch3reg_extau_right_offset = gAdcPMData->gstPmExtauValue.width_offset;
	gVportRegBx->ch3_digital_023.ch3reg_extau_ctrl = gAdcPMData->gstPmExtauValue.width_offset;

	gVportRegBx->intr_vp2_enable.intr_vp2_enable = gAdcPMData->guiPmInterruptStatus;

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
int ADC_GetPCModeInfo_Bx(LX_AFE_PCMODE_INFO_T *stPCModeInfo)
{
	UINT16	Vline, Vlowline;
   if(gResolution >= 0)
   {
	   memset((void *)stPCModeInfo, 0, sizeof(LX_AFE_PCMODE_INFO_T));
	   stPCModeInfo->u8_Mode_Idx = gResolution;
	   stPCModeInfo->u16_HSyncWidth = gVportRegBx->ch3_digital_044.ch3reg_ro_hsout_width;
	   Vline = gVportRegBx->ch3_cst_010.cst_ro_vsprdref;
	   Vlowline = gVportRegBx->ch3_cst_010.cst_ro_vslowref;
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

int ADC_Get_LLPLL_Filter_Status_Bx(void)
{
	return gVportRegBx->ch3_llpll_015.llpll_filter_status;
}

int ADC_Set_Coast_Bx(UINT8 precoast, UINT8 postcoast)
{
	gVportRegBx->ch3_cst_002.cst_precoast = precoast;
	gVportRegBx->ch3_cst_002.cst_postcoast = postcoast;

	return 0;
}

int ADC_Set_Hsync_Sel_Bx(UINT8 select)
{
	if(select > 0)
		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block
	else
		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x0;

	return 0;
}

int ADC_CVI_Sync_Exist_Bx(void)
{
	UINT32 hsync_exist, vsync_exist, src_valid;

	CHA_CVI_RdFL(cha_size_status0);
	CHA_CVI_Rd03(cha_size_status0, hsync_signal_detect, hsync_exist, vsync_signal_detect, vsync_exist, source_valid, src_valid);

	AFE_PRINT("hsync = [%d], vsync = [%d], valid = [%d]\n", hsync_exist, vsync_exist, src_valid);

	if(hsync_exist && vsync_exist/* && src_valid*/)
		return 1;
	else
		return 0;
}

int ADC_Set_SOG_Level_Bx(UINT32 level)
{
	if(level > 0x1F)
		return -1;

	gVportRegBx->ch3_adc_002.afe3ch_soglvl = level;

	ADC_DEBUG("Set SOG Level = [0x%x]\n", level);

	return 0;
}

int ADC_Get_SOG_Level_Bx(UINT32 *level)
{
	*level = gVportRegBx->ch3_adc_002.afe3ch_soglvl;

	ADC_DEBUG("Get SOG Level = [0x%x]\n", *level);

	return 0;
}

int ADC_Check_Htotal_Diff_Bx(UINT8 count, int Kadp_Index)
{
	UINT8 i;

	for(i = 0; i < count;i++)
	{
		if(abs(gVportRegBx->ch3_llpll_003.llpll_counter_max - gVportRegBx->ch3_digital_036.ch3reg_ro_hsoutprd) > 0)
		{
			AFE_PRINT("##### Htotal diff [%d] [0x%x][0x%x]!!!\n", i, gVportRegBx->ch3_llpll_003.llpll_counter_max , gVportRegBx->ch3_digital_036.ch3reg_ro_hsoutprd);
			mdelay(30);
			ADC_Reset_Digital();
			mdelay(30);
		}
		else
			return 0;
	}
	return -1;
}

int ADC_Read_Pixel_Value_Bx(LX_AFE_ADC_PIXEL_VALUE_T *pixel_value_t)
{
	int ret = 0;
	S_RGB_SUM sum;

	VPORT_RdFL(ch3_digital_017);
	VPORT_Wr02(ch3_digital_017, ch3reg_ap_xpos_r, pixel_value_t->X_Pos, ch3reg_ap_xsize_r,pixel_value_t->X_Size );
	VPORT_WrFL(ch3_digital_017);

	VPORT_RdFL(ch3_digital_018);
	VPORT_Wr02(ch3_digital_018, ch3reg_ap_ypos_r, pixel_value_t->Y_Pos, ch3reg_ap_ysize_r,pixel_value_t->Y_Size );
	VPORT_WrFL(ch3_digital_018);

	VPORT_RdFL(ch3_digital_019);
	VPORT_Wr02(ch3_digital_019, ch3reg_ap_xpos_g, pixel_value_t->X_Pos, ch3reg_ap_xsize_g,pixel_value_t->X_Size );
	VPORT_WrFL(ch3_digital_019);

	VPORT_RdFL(ch3_digital_020);
	VPORT_Wr02(ch3_digital_020, ch3reg_ap_ypos_g, pixel_value_t->Y_Pos, ch3reg_ap_ysize_g,pixel_value_t->Y_Size );
	VPORT_WrFL(ch3_digital_020);

	VPORT_RdFL(ch3_digital_021);
	VPORT_Wr02(ch3_digital_021, ch3reg_ap_xpos_b, pixel_value_t->X_Pos, ch3reg_ap_xsize_b,pixel_value_t->X_Size );
	VPORT_WrFL(ch3_digital_021);

	VPORT_RdFL(ch3_digital_022);
	VPORT_Wr02(ch3_digital_022, ch3reg_ap_ypos_b, pixel_value_t->Y_Pos, ch3reg_ap_ysize_b,pixel_value_t->Y_Size );
	VPORT_WrFL(ch3_digital_022);

	mdelay(100);

	ADC_GetSumRGB_Bx(&sum);

	pixel_value_t->Sum_R_Value = sum.sum_R;
	pixel_value_t->Sum_G_Value = sum.sum_G;
	pixel_value_t->Sum_B_Value = sum.sum_B;

	return ret;
}

int ADC_Green_Mid_Level_Select_Bx(UINT8 value)
{
	VPORT_RdFL(ch3_adc_001);
	VPORT_Wr01(ch3_adc_001, afe3ch_gmidsel, value);
	VPORT_WrFL(ch3_adc_001);

	return 0;
}

int ADC_Read_VSync_Width_Bx(UINT32 *pvs_width)
{
	UINT32 v_line_count, v_low_line_count;

	VPORT_RdFL(ch3_cst_010);
	VPORT_Rd02(ch3_cst_010, cst_ro_vslowref, v_low_line_count, cst_ro_vsprdref, v_line_count);

	if((v_line_count /2) > v_low_line_count)	// Active Low VSync
		*pvs_width = v_low_line_count;
	else										// Active High VSync
		*pvs_width = v_line_count - v_low_line_count;

	return 0;
}

int ADC_Read_HSync_Width_Bx(UINT32 *phs_width)
{
	UINT32 h_line_count, h_low_line_count;

	VPORT_RdFL(ch3_cst_009);
	VPORT_Rd02(ch3_cst_009, cst_ro_hslowref, h_low_line_count, cst_ro_hsprdref, h_line_count);

	if((h_line_count /2) > h_low_line_count)	// Active Low VSync
		*phs_width = h_low_line_count;
	else										// Active High VSync
		*phs_width = h_line_count - h_low_line_count;

	return 0;
}

int ADC_Set_DCO_MinMax_Value_Bx(UINT8 dco_min, UINT8 dco_max)
{
	//LLPLL_dco_max

	VPORT_RdFL(ch3_llpll_006);
	VPORT_Wr02(ch3_llpll_006, llpll_dco_max, dco_max, llpll_dco_min, dco_min);
	VPORT_WrFL(ch3_llpll_006);

	return 0;
}

int ADC_Get_DCO_MinMax_Value_Bx(UINT8 *pdco_min, UINT8 *pdco_max)
{
	//LLPLL_dco_max

	VPORT_RdFL(ch3_llpll_006);
	VPORT_Rd02(ch3_llpll_006, llpll_dco_max, *pdco_max, llpll_dco_min, *pdco_min);
	VPORT_WrFL(ch3_llpll_006);

	return 0;
}
