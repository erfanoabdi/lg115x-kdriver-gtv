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

#include "./../../afe/l9/vport_reg_l9a0.h"
#include "./../../afe/l9/vport_i2c_l9a0.h"

#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"
#include "afe_drv.h"
#include "os_util.h"

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
#define L9_RGB_WA_FOR_CLOCK_ADJUST
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
extern LX_AFE_ADC_INPUT_SOURCE_TYPE_T gADC_type;
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

int ADC_L9Ax_PowerOn(void)
{

	//FIXME juhee - check if the following sequence is right or not.
	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 0;
	gllpll_0.llpll_pdb = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	OS_MsecSleep(20);

	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 1;
	gllpll_0.llpll_pdb = 1;
	VPORT_I2C_Write((UINT32*)&gllpll_0);


	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_biaspdb = 0;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	OS_MsecSleep(5);

	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_biaspdb = 1;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	OS_MsecSleep(5);

	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_bpdb = 1;
	gafe_3ch_0.afe3ch_gpdb = 1;
	gafe_3ch_0.afe3ch_rpdb = 1;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	//FIXME juhee - check if removed is ok not to set.
#if 0
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, 1);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif

	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	gsoft_reset_0.swrst_pix = 1;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);

	OS_MsecSleep(20);

	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	gsoft_reset_0.swrst_pix = 0;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);

	//FIXME juhee - check if removed is ok not to set.
#if 0
	VPORT_RdFL(ch3_adc_015);
	VPORT_Wr01(ch3_adc_015, afe3ch_volbypas, 0);
	VPORT_WrFL(ch3_adc_015);

	VPORT_RdFL(ch3_adc_015);
	VPORT_Wr01(ch3_adc_015, afe3ch_envolclp, 1);
	VPORT_WrFL(ch3_adc_015);
#endif


//Enable Interrupt
	//gVportRegBx->intr_vp2_enable.intr_vp2_enable = ADC_DETECTION_INTERRUPT_MASK | ADC_AOGC_INTERRUPT_MASK;
	//FIXME juhee - no match
#if 0
	VPORT_RdFL(intr_vp2_enable);
	VPORT_Wr01(intr_vp2_enable, intr_vp2_enable, ADC_DETECTION_INTERRUPT_MASK);
	VPORT_WrFL(intr_vp2_enable);
#endif
	return 0;

}

int ADC_L9Ax_PowerOff(void)
{
	// reg_exta_sel , reg_extb_sel
	//gVportRegBx->reg_exta_sel.reg_exta_sel = 0x0;
	//gVportRegBx->reg_exta_sel.reg_extb_sel = 0x0;

	//FIXME juhee - check if removed is ok not to set.
#if 0
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_vref_pdb, 0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif
	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_biaspdb = 0;
	gafe_3ch_0.afe3ch_bpdb = 0;
	gafe_3ch_0.afe3ch_gpdb = 0;
	gafe_3ch_0.afe3ch_rpdb = 0;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	//FIXME juhee - what will be the right value??
#if 0
	VPORT_RdFL(top_003);
	VPORT_Wr01(top_003, reg_ch3pll_pdb, 0);
	VPORT_WrFL(top_003);
#endif

	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 0;
	gllpll_0.llpll_pdb = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	gsoft_reset_0.swrst_pix = 1;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);

	return 0;

}

int ADC_L9Ax_InitDaughterBoard(void)
{
	S_OFFSET_INFORM offset;
	//ADC_PowerOn();

	//gVportRegBx->ch3_cst_002.cst_postcoast = 0x1f;	//Macro vision pseudo Sync
	/* release swrst_f24m is move to i2c_coreinit
	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	//gsoft_reset_0.swrst_pix = 0;
	gsoft_reset_0.swrst_f24m = 0;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
	*/

	//powerory 110425
	/*
	VPORT_I2C_Read((UINT32*)&gadc_dig_22);
	VPORT_I2C_Read((UINT32*)&gadc_dig_23);
	gadc_dig_22.precoast = 0x0A;
	gadc_dig_23.postcoast = 0x1D;
	VPORT_I2C_Write((UINT32*)&gadc_dig_22);
	VPORT_I2C_Write((UINT32*)&gadc_dig_23);
	*/


	VPORT_I2C_Read((UINT32*)&gafe_3ch_5);
	VPORT_I2C_Read((UINT32*)&gafe_3ch_6);

//	gafe_3ch_5.afe3ch_soglvl = 0x7;
//	gafe_3ch_5.afe3ch_sog_hys = 0x3;

	//by wy.choi  for shibasoku test
	gafe_3ch_5.afe3ch_soglvl = 0x7;
	gafe_3ch_5.afe3ch_sog_hys = 0x2;

	gafe_3ch_6.afe3ch_sog_bw = 0x2;

	VPORT_I2C_Write((UINT32*)&gafe_3ch_5);
	VPORT_I2C_Write((UINT32*)&gafe_3ch_6);

	//FIXME : Juhee 3DCD is not possible
	VPORT_I2C_Read((UINT32*)&gadc_dig_112);
	VPORT_I2C_Read((UINT32*)&gadc_dig_113);
	gadc_dig_112.size_detect_ctrl_1 = 0x3D;
	gadc_dig_113.size_detect_ctrl_0 = 0xCD;
	VPORT_I2C_Write((UINT32*)&gadc_dig_112);
	VPORT_I2C_Write((UINT32*)&gadc_dig_113);

	//gVportRegBx->ch3_cst_003.cst_hspolover = 0x1; // 0825 new value from powerory
	//gVportRegBx->ch3_cst_003.cst_vspolover = 0x1; // 0825 new value from powerory
	//gVportRegBx->ch3_cst_003.cst_hspoluser = 0x0; // 0825 new value from powerory
	//gVportRegBx->ch3_cst_003.cst_vspoluser = 0x0; // 0825 new value from powerory

	//For no interrupt occurrence when resolution change between master 210(576i) to 209(480i)
	//gVportRegBx->ch3_llpll_010.llpll_lpf_clk_sel = 0x1;
	//110214

	VPORT_I2C_Read((UINT32*)&gllpll_18);
	gllpll_18.llpll_lpf_clk_sel = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_18);

	ADC_PowerOn();

	//20110125
//	_iow(&gVportRegBx->ch3_digital_003 , 8 , 0 , 0x80);	// ch3reg_hspdeq_thr
	gadc_dig_54.hsprdeq_thr = 0xC4;
	VPORT_I2C_Write((UINT32*)&gadc_dig_54);

	//20110425
	//L9A0 Initial Resettings by powerory

	offset.offset_R = ADC_L9_R_PRE_OFFSET_VALUE;
	offset.offset_G = ADC_L9_G_PRE_OFFSET_VALUE;
	offset.offset_B = ADC_L9_B_PRE_OFFSET_VALUE;

	ADC_L9Ax_SetOffsetRGB( &offset);

	gadc_dig_52.hsdiff_thr_1 = 0x0;
	gadc_dig_53.hsdiff_thr_0 = 0x0;
	VPORT_I2C_Write((UINT32*)&gadc_dig_52);
	VPORT_I2C_Write((UINT32*)&gadc_dig_53);

	//110426 by powerory
	gadc_dig_222.clamp_init = 0x0F;
	VPORT_I2C_Write((UINT32*)&gadc_dig_222);

	//110426 by powerory	(Clamp position adjust)
	//gadc_dig_203.blank_sp_0 = 0x80;	// default 0x04;

	//110513 by min
	gadc_dig_203.blank_sp_0 = 0x60;	// default 0x04;
	VPORT_I2C_Write((UINT32*)&gadc_dig_203);

	VPORT_I2C_Read((UINT32*)&gadc_dig_204);
	VPORT_I2C_Read((UINT32*)&gadc_dig_209);
	VPORT_I2C_Read((UINT32*)&gadc_dig_215);
	gadc_dig_204.r_blank_mode = 0x1;
	gadc_dig_209.g_blank_mode = 0x1;
	gadc_dig_215.b_blank_mode = 0x1;
	VPORT_I2C_Write((UINT32*)&gadc_dig_204);
	VPORT_I2C_Write((UINT32*)&gadc_dig_209);
	VPORT_I2C_Write((UINT32*)&gadc_dig_215);

	gadc_dig_220.clamp_width = 0x14;
	VPORT_I2C_Write((UINT32*)&gadc_dig_220);

	//110720 : changed cst_diff_thr value to 1 for valid coast on 1080i component input of BH-200.
	//Default value '3' is for abnormal vsync pulse of Divico component signal.
	gadc_dig_26.cst_diff_thr_1 = 0x0;
	gadc_dig_27.cst_diff_thr_0 = 0x1;
	VPORT_I2C_Write((UINT32*)&gadc_dig_26);
	VPORT_I2C_Write((UINT32*)&gadc_dig_27);

#ifdef	KDRV_CONFIG_PM
	if(gAdcPmInitial < 0)
	{
		gAdcPMData = (S_ADC_PM_DATA *)kmalloc(sizeof(S_ADC_PM_DATA) , GFP_KERNEL);
		gAdcPMData->giPmIndexValue = -1;
		gAdcPmInitial = 0;
	}
#endif
//	OS_MsecSleep(100);

	return 0;
}


int ADC_L9Ax_GetBoundaryInform(ST_EXTAU *inform)
{
	UINT16 out16 = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_182);
	VPORT_I2C_Read((UINT32*)&gadc_dig_183);
	VPORT_I2C_Read((UINT32*)&gadc_dig_184);
	VPORT_I2C_Read((UINT32*)&gadc_dig_185);
	VPORT_I2C_Read((UINT32*)&gadc_dig_186);
	VPORT_I2C_Read((UINT32*)&gadc_dig_187);
	VPORT_I2C_Read((UINT32*)&gadc_dig_188);
	VPORT_I2C_Read((UINT32*)&gadc_dig_189);

	// get extau_up
	out16 = 0;
	inform->extau_up = 0;
	out16 =  (gadc_dig_182.extau_up_result_1)&0x0F;
	inform->extau_up |= out16 << 8;
	out16 =  gadc_dig_183.extau_up_result_0;
	inform->extau_up |= out16;

	// get extau_down
	out16 = 0;
	inform->extau_down = 0;
	out16 =  (gadc_dig_184.extau_down_result_1)&0x0F;
	inform->extau_down |= out16 << 8;
	out16 =  gadc_dig_185.extau_down_result_0;
	inform->extau_down |= out16;

	// get extau_left
	out16 = 0;
	inform->extau_left = 0;
	out16 =  (gadc_dig_186.extau_left_result_1)&0x0F;
	inform->extau_left |= out16 << 8;
	out16 =  gadc_dig_187.extau_left_result_0;
	inform->extau_left |= out16;
#ifdef ADC_BOUNDARY_COMPENSATION
	if(inform->extau_left >= 1)
	{
		inform->extau_left -= 1;
	}
#endif

	// get extau_lest
	out16 = 0;
	inform->extau_right = 0;
	out16 =  (gadc_dig_188.extau_right_result_1)&0x0F;
	inform->extau_right |= out16 << 8;
	out16 =  gadc_dig_189.extau_right_result_0;
	inform->extau_right |= out16;
#ifdef ADC_BOUNDARY_COMPENSATION
	if(inform->extau_right >= 1)
	{
		inform->extau_right -= 1;
	}
#endif

	return 0;
}

int ADC_L9Ax_SetTAUInform(S_EXTAU_VALUE *value)
{
	UINT32 hsout_tmp;
	// Height
	VPORT_I2C_Read((UINT32*)&gadc_dig_176);
	VPORT_I2C_Read((UINT32*)&gadc_dig_177);
	gadc_dig_176.extau_down_offset_1 = (value->height_offset&0xFF00)>>8;
	gadc_dig_177.extau_down_offset_0 = value->height_offset&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_176);
	VPORT_I2C_Write((UINT32*)&gadc_dig_177);

	// WIDTH
	VPORT_I2C_Read((UINT32*)&gadc_dig_180);
	VPORT_I2C_Read((UINT32*)&gadc_dig_181);
	gadc_dig_180.extau_right_offset_1 = (value->width_offset&0xFF00)>>8;
	gadc_dig_181.extau_right_offset_0 = value->width_offset&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_180);
	VPORT_I2C_Write((UINT32*)&gadc_dig_181);

	VPORT_I2C_Read((UINT32*)&gadc_dig_170);
	VPORT_I2C_Read((UINT32*)&gadc_dig_171);
	gadc_dig_170.extau_ctrl_1 = (value->control&0xFF00)>>8;
	gadc_dig_171.extau_ctrl_0 = value->control&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_170);
	VPORT_I2C_Write((UINT32*)&gadc_dig_171);

	VPORT_I2C_Read((UINT32*)&gadc_dig_172);
	VPORT_I2C_Read((UINT32*)&gadc_dig_173);
	gadc_dig_172.extau_level_1 = (value->level&0xFF00)>>8;
	gadc_dig_173.extau_level_0 = value->level&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_172);
	VPORT_I2C_Write((UINT32*)&gadc_dig_173);

	// For Proper Boundary Detection in case of PC RGB Horizontal Sync Crosstalk on RGB Signal, Adjusted left_offset.
	// Enlarge Value to 10 1019 for PC geforce 8400GS
	VPORT_I2C_Read((UINT32*)&gadc_dig_75);
	VPORT_I2C_Read((UINT32*)&gadc_dig_76);
	hsout_tmp = ((gadc_dig_75.ro_hsout_width_1 << 8) & 0xF00) + gadc_dig_76.ro_hsout_width_0;
	hsout_tmp += 10;
	gadc_dig_178.extau_left_offset_1 = (hsout_tmp >> 8) & 0x00F;
	gadc_dig_179.extau_left_offset_0 = hsout_tmp & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_178);
	VPORT_I2C_Write((UINT32*)&gadc_dig_179);

	return 0;
}

int ADC_L9Ax_SetRGBPosition(S_RGB_POSITION *pos)
{
	VPORT_I2C_Read((UINT32*)&gadc_dig_130);
	VPORT_I2C_Read((UINT32*)&gadc_dig_131);
	gadc_dig_130.ap_xpos_b_1 = (pos->width_b&0xFF00)>>8;
	gadc_dig_131.ap_xpos_b_0 = pos->width_b&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_130);
	VPORT_I2C_Write((UINT32*)&gadc_dig_131);

	VPORT_I2C_Read((UINT32*)&gadc_dig_122);
	VPORT_I2C_Read((UINT32*)&gadc_dig_123);
	gadc_dig_122.ap_xpos_g_1 = (pos->width_g&0xFF00)>>8;
	gadc_dig_123.ap_xpos_g_0 = pos->width_g&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_122);
	VPORT_I2C_Write((UINT32*)&gadc_dig_123);

	VPORT_I2C_Read((UINT32*)&gadc_dig_138);
	VPORT_I2C_Read((UINT32*)&gadc_dig_139);
	gadc_dig_138.ap_xpos_r_1 = (pos->width_r&0xFF00)>>8;
	gadc_dig_139.ap_xpos_r_0 = pos->width_r&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_138);
	VPORT_I2C_Write((UINT32*)&gadc_dig_139);

	VPORT_I2C_Read((UINT32*)&gadc_dig_134);
	VPORT_I2C_Read((UINT32*)&gadc_dig_135);
	gadc_dig_134.ap_ypos_b_1 = (pos->height_b&0xFF00)>>8;
	gadc_dig_135.ap_ypos_b_0 = pos->height_b&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_134);
	VPORT_I2C_Write((UINT32*)&gadc_dig_135);

	VPORT_I2C_Read((UINT32*)&gadc_dig_126);
	VPORT_I2C_Read((UINT32*)&gadc_dig_127);
	gadc_dig_126.ap_ypos_g_1 = (pos->height_g&0xFF00)>>8;
	gadc_dig_127.ap_ypos_g_0 = pos->height_g&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_126);
	VPORT_I2C_Write((UINT32*)&gadc_dig_127);

	VPORT_I2C_Read((UINT32*)&gadc_dig_142);
	VPORT_I2C_Read((UINT32*)&gadc_dig_143);
	gadc_dig_142.ap_ypos_r_1 = (pos->height_r&0xFF00)>>8;
	gadc_dig_143.ap_ypos_r_0 = pos->height_r&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_142);
	VPORT_I2C_Write((UINT32*)&gadc_dig_143);

	return 0;
}

int ADC_L9Ax_GetRGBResult(S_RGB_OUT *result)
{
	UINT32 out32 = 0;

	result->out_b = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_146);
	VPORT_I2C_Read((UINT32*)&gadc_dig_147);
	VPORT_I2C_Read((UINT32*)&gadc_dig_148);
	VPORT_I2C_Read((UINT32*)&gadc_dig_149);
	out32 =  gadc_dig_146.ap_out_b_3;
	result->out_b |= out32 << 24;
	out32 =  gadc_dig_147.ap_out_b_2;
	result->out_b |= out32 << 16;
	out32 =  gadc_dig_148.ap_out_b_1;
	result->out_b |= out32 << 8;
	out32 =  gadc_dig_149.ap_out_b_0;
	result->out_b |= out32;

	out32 = 0;
	result->out_g =	0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_150);
	VPORT_I2C_Read((UINT32*)&gadc_dig_151);
	VPORT_I2C_Read((UINT32*)&gadc_dig_152);
	VPORT_I2C_Read((UINT32*)&gadc_dig_153);
	out32 =  gadc_dig_150.ap_out_g_3;
	result->out_g |= out32 << 24;
	out32 =  gadc_dig_151.ap_out_g_2;
	result->out_g |= out32 << 16;
	out32 =  gadc_dig_152.ap_out_g_1;
	result->out_g |= out32 << 8;
	out32 =  gadc_dig_153.ap_out_g_0;
	result->out_g |= out32;

	out32 = 0;
	result->out_r =	0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_154);
	VPORT_I2C_Read((UINT32*)&gadc_dig_155);
	VPORT_I2C_Read((UINT32*)&gadc_dig_156);
	VPORT_I2C_Read((UINT32*)&gadc_dig_157);
	out32 =  gadc_dig_154.ap_out_r_3;
	result->out_r |= out32 << 24;
	out32 =  gadc_dig_155.ap_out_r_2;
	result->out_r |= out32 << 16;
	out32 =  gadc_dig_156.ap_out_r_1;
	result->out_r |= out32 << 8;
	out32 =  gadc_dig_157.ap_out_r_0;
	result->out_r |= out32;

	return 0;
}

int ADC_L9Ax_GetRGBPosition(S_RGB_POSITION *pos)
{
	UINT16 out16 = 0;

	pos->width_b = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_130);
	VPORT_I2C_Read((UINT32*)&gadc_dig_131);
	out16 =  (gadc_dig_130.ap_xpos_b_1)&0x0F;
	pos->width_b |= out16 << 8;
	out16 =  gadc_dig_131.ap_xpos_b_0;
	pos->width_b |= out16;

	out16 = 0;
	pos->width_g =	0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_122);
	VPORT_I2C_Read((UINT32*)&gadc_dig_123);
	out16 =  (gadc_dig_122.ap_xpos_g_1)&0x0F;
	pos->width_g |= out16 << 8;
	out16 =  gadc_dig_123.ap_xpos_g_0;
	pos->width_g |= out16;

	out16 = 0;
	pos->width_r =	0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_138);
	VPORT_I2C_Read((UINT32*)&gadc_dig_139);
	out16 =  (gadc_dig_138.ap_xpos_r_1)&0x0F;
	pos->width_r |= out16 << 8;
	out16 =  gadc_dig_139.ap_xpos_r_0;
	pos->width_r |= out16;

	out16 = 0;
	pos->height_b = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_134);
	VPORT_I2C_Read((UINT32*)&gadc_dig_135);
	out16 =  (gadc_dig_134.ap_ypos_b_1)&0x0F;
	pos->height_b |= out16 << 8;
	out16 =  gadc_dig_135.ap_ypos_b_0;
	pos->height_b |= out16;

	out16 = 0;
	pos->height_g =	0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_126);
	VPORT_I2C_Read((UINT32*)&gadc_dig_127);
	out16 =  (gadc_dig_126.ap_ypos_g_1)&0x0F;
	pos->height_g |= out16 << 8;
	out16 =  gadc_dig_127.ap_ypos_g_0;
	pos->height_g |= out16;

	out16 = 0;
	pos->height_r =	0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_142);
	VPORT_I2C_Read((UINT32*)&gadc_dig_143);
	out16 =  (gadc_dig_142.ap_ypos_r_1)&0x0F;
	pos->height_r |= out16 << 8;
	out16 =  gadc_dig_143.ap_ypos_r_0;
	pos->height_r |= out16;

	return 0;
}

int ADC_L9Ax_SetRGBSize(S_RGB_SIZE *size)
{
	VPORT_I2C_Read((UINT32*)&gadc_dig_132);
	VPORT_I2C_Read((UINT32*)&gadc_dig_133);
	gadc_dig_132.ap_xsize_b_1 = (size->width_b&0xFF00)>>8;
	gadc_dig_133.ap_xsize_b_0 = size->width_b&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_132);
	VPORT_I2C_Write((UINT32*)&gadc_dig_133);

	VPORT_I2C_Read((UINT32*)&gadc_dig_124);
	VPORT_I2C_Read((UINT32*)&gadc_dig_125);
	gadc_dig_124.ap_xsize_g_1 = (size->width_g&0xFF00)>>8;
	gadc_dig_125.ap_xsize_g_0 = size->width_g&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_124);
	VPORT_I2C_Write((UINT32*)&gadc_dig_125);

	VPORT_I2C_Read((UINT32*)&gadc_dig_140);
	VPORT_I2C_Read((UINT32*)&gadc_dig_141);
	gadc_dig_140.ap_xsize_r_1 = (size->width_r&0xFF00)>>8;
	gadc_dig_141.ap_xsize_r_0 = size->width_r&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_140);
	VPORT_I2C_Write((UINT32*)&gadc_dig_141);

	VPORT_I2C_Read((UINT32*)&gadc_dig_136);
	VPORT_I2C_Read((UINT32*)&gadc_dig_137);
	gadc_dig_136.ap_ysize_b_1 = (size->height_b&0xFF00)>>8;
	gadc_dig_137.ap_ysize_b_0 = size->height_b&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_136);
	VPORT_I2C_Write((UINT32*)&gadc_dig_137);

	VPORT_I2C_Read((UINT32*)&gadc_dig_128);
	VPORT_I2C_Read((UINT32*)&gadc_dig_129);
	gadc_dig_128.ap_ysize_g_1 = (size->height_g&0xFF00)>>8;
	gadc_dig_129.ap_ysize_g_0 = size->height_g&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_128);
	VPORT_I2C_Write((UINT32*)&gadc_dig_129);

	VPORT_I2C_Read((UINT32*)&gadc_dig_144);
	VPORT_I2C_Read((UINT32*)&gadc_dig_145);
	gadc_dig_144.ap_ysize_r_1 = (size->height_r&0xFF00)>>8;
	gadc_dig_145.ap_ysize_r_0 = size->height_r&0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_144);
	VPORT_I2C_Write((UINT32*)&gadc_dig_145);

	return 0;
}

int ADC_L9Ax_GetSizeValue(S_VIDEO_SIZE_INFORM *size)
{
	UINT16 out16 = 0;
	UINT32 out32 = 0;

	size->ext_vsize = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_114);
	VPORT_I2C_Read((UINT32*)&gadc_dig_115);
	out16 =  (gadc_dig_114.sd_ext_vsize_1)&0x0F;
	size->ext_vsize |= out16 << 8;
	out16 =  gadc_dig_115.sd_ext_vsize_0;
	size->ext_vsize |= out16;

	out32 = 0;
	size->vd_vsize= 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_116);
	VPORT_I2C_Read((UINT32*)&gadc_dig_117);
	VPORT_I2C_Read((UINT32*)&gadc_dig_118);
	out32 =  (gadc_dig_116.sd_vd_vsize_2)&0x7F;
	size->vd_vsize |= out32 << 16;
	out32 =  gadc_dig_117.sd_vd_vsize_1;
	size->vd_vsize |= out32 << 8;
	out32 =  gadc_dig_118.sd_vd_vsize_0;
	size->vd_vsize |= out32;

	out16 = 0;
	size->vd_hsize= 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_119);
	VPORT_I2C_Read((UINT32*)&gadc_dig_120);
	out16 =  (gadc_dig_119.sd_vd_hsize_1)&0x1F;
	size->vd_hsize |= out16 << 8;
	out16 =  gadc_dig_120.sd_vd_hsize_0;
	size->vd_hsize |= out16;

	VPORT_I2C_Read((UINT32*)&gadc_dig_29);

	size->polarity = (gadc_dig_29.ro_hspol<<1) | (gadc_dig_29.ro_vspol);

	return 0;
}

int ADC_L9Ax_SetPort(S_SEL_PORT *sel_port)
{

#ifdef	KDRV_CONFIG_PM
	memcpy((void *)&gAdcPMData->gstPmSelPortValue , (void *)sel_port , sizeof(S_SEL_PORT));
#endif
	//AFE_PRINT("ADC_SetPort \n");

	AFE_PRINT("ADC_SetPort : sel_port->port = [%d] \n", sel_port->port);

	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_selmux = sel_port->port;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	/*
	VPORT_I2C_Read((UINT32*)&gadc_dig_22);
	gadc_dig_22.compover = sel_port->compover;
	gadc_dig_22.compover = sel_port->compuser;
	VPORT_I2C_Write((UINT32*)&gadc_dig_22);
	*/

	return 0;
}

int ADC_L9Ax_GetSelectedPort(void)
{
	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);

	return	gafe_3ch_0.afe3ch_selmux;
}

unsigned int ADC_L9Ax_GetPhaseValue(void)
{
	UINT32 ret = 0;

	VPORT_I2C_Read((UINT32*)&gllpll_4);
	ret = gllpll_4.llpll_shift_1;
	ret = ret << 8;
	VPORT_I2C_Read((UINT32*)&gllpll_5);
	ret |= gllpll_5.llpll_shift_0;

	return ret;
}

int ADC_L9Ax_SetPhaseValue(unsigned int phase)
{
	UINT16 tmp16 = 0;
//	AFE_PRINT("######### Set ADC Phase Register to [%d][0x%x]\n", phase,phase );

	tmp16 = phase;
	tmp16 = tmp16 >> 8;
	gllpll_4.llpll_shift_1 = tmp16 & 0x3;
	tmp16 = phase & 0xFF;

	gllpll_5.llpll_shift_0 = tmp16;

	VPORT_I2C_Write((UINT32*)&gllpll_4);
	VPORT_I2C_Write((UINT32*)&gllpll_5);

	return 0;
}

unsigned int ADC_L9Ax_GetScanType(void)
{
	unsigned int ret = 0, tmp = 0;
	UINT8	count=0;

	for(;count<INTERLACE_CHECK_COUNT;count++)
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_29);

		tmp = gadc_dig_29.ro_interlaced;
		ret += tmp^1;
	}

//	AFE_PRINT("* Interlace check count value = [%d]\n", ret);

	if(ret > (INTERLACE_CHECK_COUNT/2))
		return 1;
	else
		return 0;

}


int ADC_L9Ax_SetGainValue(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;

	gadc_dig_0.r_pre_gain_1 = (stGain->R_Gain_Value >> 8) & 0x3F;
	gadc_dig_1.r_pre_gain_0 = (stGain->R_Gain_Value ) & 0xFF;
	gadc_dig_2.g_pre_gain_1 = (stGain->G_Gain_Value >> 8) & 0x3F;
	gadc_dig_3.g_pre_gain_0 = (stGain->G_Gain_Value ) & 0xFF;
	gadc_dig_4.b_pre_gain_1 = (stGain->B_Gain_Value >> 8) & 0x3F;
	gadc_dig_5.b_pre_gain_0 = (stGain->B_Gain_Value ) & 0xFF;

	VPORT_I2C_Write((UINT32*)&gadc_dig_0);
	VPORT_I2C_Write((UINT32*)&gadc_dig_1);
	VPORT_I2C_Write((UINT32*)&gadc_dig_2);
	VPORT_I2C_Write((UINT32*)&gadc_dig_3);
	VPORT_I2C_Write((UINT32*)&gadc_dig_4);
	VPORT_I2C_Write((UINT32*)&gadc_dig_5);
	return ret;
}

int ADC_L9Ax_GetGainValue(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;

	VPORT_I2C_Read((UINT32*)&gadc_dig_0);
	VPORT_I2C_Read((UINT32*)&gadc_dig_1);
	VPORT_I2C_Read((UINT32*)&gadc_dig_2);
	VPORT_I2C_Read((UINT32*)&gadc_dig_3);
	VPORT_I2C_Read((UINT32*)&gadc_dig_4);
	VPORT_I2C_Read((UINT32*)&gadc_dig_5);
	stGain->R_Gain_Value = gadc_dig_0.r_pre_gain_1 << 8 | gadc_dig_1.r_pre_gain_0;
	stGain->G_Gain_Value = gadc_dig_2.g_pre_gain_1 << 8 | gadc_dig_3.g_pre_gain_0;
	stGain->B_Gain_Value = gadc_dig_4.b_pre_gain_1 << 8 | gadc_dig_5.b_pre_gain_0;

	return ret;
}


int ADC_L9Ax_SetOffsetValue(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;

	gadc_dig_224.r_pre_offset_1 = (stOffset->R_Offset_Value >> 8) & 0x1F;
	gadc_dig_225.r_pre_offset_0 = stOffset->R_Offset_Value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_224);
	VPORT_I2C_Write((UINT32*)&gadc_dig_225);

	gadc_dig_226.g_pre_offset_1 = (stOffset->G_Offset_Value >> 8) & 0x1F;
	gadc_dig_227.g_pre_offset_0 = stOffset->G_Offset_Value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_226);
	VPORT_I2C_Write((UINT32*)&gadc_dig_227);

	gadc_dig_228.b_pre_offset_1 = (stOffset->B_Offset_Value >> 8) & 0x1F;
	gadc_dig_229.b_pre_offset_0 = stOffset->B_Offset_Value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_228);
	VPORT_I2C_Write((UINT32*)&gadc_dig_229);


	return ret;
}

int ADC_L9Ax_GetOffsetValue(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;

	VPORT_I2C_Read((UINT32*)&gadc_dig_224);
	VPORT_I2C_Read((UINT32*)&gadc_dig_225);
	VPORT_I2C_Read((UINT32*)&gadc_dig_226);
	VPORT_I2C_Read((UINT32*)&gadc_dig_227);
	VPORT_I2C_Read((UINT32*)&gadc_dig_228);
	VPORT_I2C_Read((UINT32*)&gadc_dig_229);
	stOffset->R_Offset_Value = gadc_dig_224.r_pre_offset_1 << 8 | gadc_dig_225.r_pre_offset_0;
	stOffset->G_Offset_Value = gadc_dig_226.g_pre_offset_1 << 8 | gadc_dig_227.g_pre_offset_0;
	stOffset->B_Offset_Value = gadc_dig_228.b_pre_offset_1 << 8 | gadc_dig_229.b_pre_offset_0;

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
int ADC_L9Ax_SetInputVideo(int iIndex)
{

	int i;
	UINT32 hsout_width =0;
	UINT8	llpll_filter_status;

	int ret = 0;

#ifdef	KDRV_CONFIG_PM
	gAdcPMData->giPmIndexValue = iIndex;
#endif
	ADC_DEBUG("%s entered with value [%d]\n", __func__, iIndex);

	VPORT_I2C_Read((UINT32*)&gadc_dig_74);
	if(iIndex == 306) // for Component 576i 50Hz field indication should be inverted
	{
		gadc_dig_74.fld_inv = 0x1;
	}
	else
	{
		gadc_dig_74.fld_inv = 0x0;
	}
	VPORT_I2C_Write((UINT32*)&gadc_dig_74);

	hsout_width = HS_Out_Width[iIndex];

//	AFE_PRINT("Table hsout_width = [%d]\n", hsout_width);

	VPORT_I2C_Read((UINT32*)&gadc_dig_59);
	VPORT_I2C_Read((UINT32*)&gadc_dig_60);

	if( (iIndex == 303) || (iIndex == 306))	// component 480i / 576i
		hsout_width *= 2;

	gadc_dig_59.hsout_width_1 = (hsout_width >> 8) & 0x0F;
	gadc_dig_60.hsout_width_0 = hsout_width & 0xFF;

	VPORT_I2C_Write((UINT32*)&gadc_dig_59);
	VPORT_I2C_Write((UINT32*)&gadc_dig_60);

	if(iIndex == 303)	// component 480i
	{
		iIndex = 304;	// Use 480P timing
		ADC_DEBUG("for Component 480i, enable subsampling\n");

		VPORT_I2C_Read((UINT32*)&gadc_dig_223);
		gadc_dig_223.sub_sampling_en = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_223);
	}
	else if(iIndex == 306)
	{
		iIndex = 307;	// Use 576P timing
		ADC_DEBUG("for Component 576i, enable subsampling\n");

		VPORT_I2C_Read((UINT32*)&gadc_dig_223);
		gadc_dig_223.sub_sampling_en = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_223);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_223);
		gadc_dig_223.sub_sampling_en = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_223);
	}

	//CH3_LLPLL_003
	VPORT_I2C_Read((UINT32*)&gllpll_1);
	gllpll_1.llpll_div_max = (LLPLL_div_max[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_1);

	//LLPLL_counter_max
	VPORT_I2C_Read((UINT32*)&gllpll_2);
	VPORT_I2C_Read((UINT32*)&gllpll_3);
	gllpll_2.llpll_counter_max_1 = ((LLPLL_counter_max[iIndex])&0xF00)>>8;
	gllpll_3.llpll_counter_max_0 = (LLPLL_counter_max[iIndex])&0xFF;
	VPORT_I2C_Write((UINT32*)&gllpll_2);
	VPORT_I2C_Write((UINT32*)&gllpll_3);

	//LLPLL_shift 일단 0으로 초기화
	VPORT_I2C_Read((UINT32*)&gllpll_4);
	VPORT_I2C_Read((UINT32*)&gllpll_5);
	gllpll_4.llpll_shift_1 = 0;
	gllpll_5.llpll_shift_0 = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_4);
	VPORT_I2C_Write((UINT32*)&gllpll_5);

	//LLPLL_dco_max
	VPORT_I2C_Read((UINT32*)&gllpll_7);
	gllpll_7.llpll_dco_max = (LLPLL_dco_max[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_7);

	//LLPLL_dco_min
	VPORT_I2C_Read((UINT32*)&gllpll_8);
	gllpll_8.llpll_dco_min = (LLPLL_dco_min[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_8);

	//LLPLL_coarse_scale
	VPORT_I2C_Read((UINT32*)&gllpll_9);
	gllpll_9.llpll_coarse_scale = (LLPLL_coarse_scale[iIndex])&0xF;
	VPORT_I2C_Write((UINT32*)&gllpll_9);

	//LLPLL_g1_nom
	VPORT_I2C_Read((UINT32*)&gllpll_10);
	gllpll_10.llpll_g1_nom = (LLPLL_g1_nom[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_10);

	//LLPLL_g2_nom
	VPORT_I2C_Read((UINT32*)&gllpll_11);
	gllpll_11.llpll_g2_nom = (LLPLL_g2_nom[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_11);

	//LLPLL_g3_p_nom
	VPORT_I2C_Read((UINT32*)&gllpll_12);
	gllpll_12.llpll_g3_p_nom = (LLPLL_g3_p_nom[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_12);

	//LLPLL_g3_n_nom
	VPORT_I2C_Read((UINT32*)&gllpll_13);
	gllpll_13.llpll_g3_n_nom = (LLPLL_g3_n_nom[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_13);

	//LLPLL_g1_fine
	VPORT_I2C_Read((UINT32*)&gllpll_14);
	gllpll_14.llpll_g1_fine = (LLPLL_g1_fine[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_14);

	//LLPLL_g2_fine
	VPORT_I2C_Read((UINT32*)&gllpll_15);
	gllpll_15.llpll_g2_fine = (LLPLL_g2_fine[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_15);

	//LLPLL_g3_p_fine
	VPORT_I2C_Read((UINT32*)&gllpll_16);
	gllpll_16.llpll_g3_p_fine = (LLPLL_g3_p_fine[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_16);

	//LLPLL_g3_n_fine
	VPORT_I2C_Read((UINT32*)&gllpll_17);
	gllpll_17.llpll_g3_n_fine = (LLPLL_g3_n_fine[iIndex])&0x1F;
	VPORT_I2C_Write((UINT32*)&gllpll_17);

	for( i = 0 ; i < 30 ; i++)
	{
		VPORT_I2C_Read((UINT32*)&gllpll_27);
		llpll_filter_status = gllpll_27.llpll_filter_status;
		if(llpll_filter_status == 3) break;
		OS_MsecSleep(5);
	}
	if(i==30)
	{
		AFE_PRINT("!!!!!!!!!!!! LLPLL UNLOCK !!!!!!!!!!!!!!!!!\n");
		ADC_Reset_LLPLL();
		ret = -1;
	}



	VPORT_I2C_Read((UINT32*)&gadc_dig_204);
	VPORT_I2C_Read((UINT32*)&gadc_dig_209);
	VPORT_I2C_Read((UINT32*)&gadc_dig_214);

	gadc_dig_204.r_blank_num_clear = 1;
	gadc_dig_209.g_blank_num_clear = 1;
	gadc_dig_214.b_blank_num_clear = 1;

	VPORT_I2C_Write((UINT32*)&gadc_dig_204);
	VPORT_I2C_Write((UINT32*)&gadc_dig_209);
	VPORT_I2C_Write((UINT32*)&gadc_dig_214);

	gadc_dig_204.r_blank_num_clear = 0;
	gadc_dig_209.g_blank_num_clear = 0;
	gadc_dig_214.b_blank_num_clear = 0;

	VPORT_I2C_Write((UINT32*)&gadc_dig_204);
	VPORT_I2C_Write((UINT32*)&gadc_dig_209);
	VPORT_I2C_Write((UINT32*)&gadc_dig_214);

	/*
	if(i<100)
		return 0;
	else
		return -1;
		*/
	return ret;
}

int ADC_L9Ax_AGOCInit(void)
{
	ST_GAIN_INFORM *gain_temp;

	ADC_DEBUG("%s entered \n", __func__);

	gain_temp = (ST_GAIN_INFORM *)kmalloc(sizeof(ST_GAIN_INFORM),GFP_KERNEL);
	// aogc conter set as max
	VPORT_I2C_Read((UINT32*)&gadc_dig_99);
	VPORT_I2C_Read((UINT32*)&gadc_dig_100);
	gadc_dig_99.aogc_counter_max_1 = 0xFF;
	gadc_dig_100.aogc_counter_max_0 = 0xFE;
	VPORT_I2C_Write((UINT32*)&gadc_dig_99);
	VPORT_I2C_Write((UINT32*)&gadc_dig_100);

	VPORT_I2C_Read((UINT32*)&gadc_dig_98);
	gadc_dig_98.aogc_sample_count = 0x6;
	VPORT_I2C_Write((UINT32*)&gadc_dig_98);

	//110504 by powerory
	VPORT_I2C_Read((UINT32*)&gadc_dig_96);
	VPORT_I2C_Read((UINT32*)&gadc_dig_97);
	gadc_dig_96.aogc_th_1 = 0x03;
	gadc_dig_97.aogc_th_0 = 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_96);
	VPORT_I2C_Write((UINT32*)&gadc_dig_97);

	//Add From B0 Revision(Auto Calibration)
	VPORT_I2C_Read((UINT32*)&gadc_dig_98);
	gadc_dig_98.aogc_sample_accum = 0;
	VPORT_I2C_Write((UINT32*)&gadc_dig_98);

#if 0
	// removed from B0
	// aoc, agc sel register
	gVportRegBx->ch3reg_agc_ire.ch3reg_agc_ire = 3;
	gVportRegBx->ch3reg_agc_ire.ch3reg_aoc_ire = 4;
#endif
//	gVportRegBx->ch3_adc_007.afe3ch_ire_sel = 4;

	//TODO :dws
	// First set gain value to default
	gain_temp->gain_R = 0x1000;
	gain_temp->gain_G = 0x1000;
	gain_temp->gain_B = 0x1000;

	ADC_SetGainRGB(gain_temp);

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
int ADC_L9Ax_SetIRETestMode(int sel , E_AGC_IRE mode)
{
	int ret = 0;


	if(sel)	// IRE Test mode inactive
	{
		//by dws 110520 : ADC_AGOCInit is already executed before.
		//ADC_AGOCInit();

		VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
		giSelMuxBackup = gafe_3ch_0.afe3ch_selmux;

		// aogc test start
		VPORT_I2C_Read((UINT32*)&gadc_dig_89);
		gadc_dig_89.aogc_enable = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_89);

		// 110503 by powerory
		ADC_L9Ax_Reset_Digital();

		OS_MsecSleep(20);
	}
	else	// IRE Test mode active
	{
		VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
		gafe_3ch_0.afe3ch_selmux = giSelMuxBackup;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

		// aogc test stop
		VPORT_I2C_Read((UINT32*)&gadc_dig_89);
		gadc_dig_89.aogc_enable = 0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_89);
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

static int ADC_L9Ax_GetIRELevel( S_GAIN_TARGET *gain)
{
	int ret = 0;
	unsigned int temp_r, temp_g,temp_b;
	UINT32 out32 = 0;

	temp_r = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_102);
	VPORT_I2C_Read((UINT32*)&gadc_dig_103);
	VPORT_I2C_Read((UINT32*)&gadc_dig_104);
	out32 =  gadc_dig_102.ro_avg_r_2;
	temp_r |= out32 << 16;
	out32 =  gadc_dig_103.ro_avg_r_1;
	temp_r |= out32 << 8;
	out32 =  gadc_dig_104.ro_avg_r_0;
	temp_r |= out32;

	out32 = 0;
	temp_g = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_105);
	VPORT_I2C_Read((UINT32*)&gadc_dig_106);
	VPORT_I2C_Read((UINT32*)&gadc_dig_107);
	out32 =  gadc_dig_105.ro_avg_g_2;
	temp_g |= out32 << 16;
	out32 =  gadc_dig_106.ro_avg_g_1;
	temp_g |= out32 << 8;
	out32 =  gadc_dig_107.ro_avg_g_0;
	temp_g |= out32;

	out32 = 0;
	temp_b = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_108);
	VPORT_I2C_Read((UINT32*)&gadc_dig_109);
	VPORT_I2C_Read((UINT32*)&gadc_dig_110);
	out32 =  gadc_dig_108.ro_avg_b_2;
	temp_b |= out32 << 16;
	out32 =  gadc_dig_109.ro_avg_b_1;
	temp_b |= out32 << 8;
	out32 =  gadc_dig_110.ro_avg_b_0;
	temp_b |= out32;


// powerory 2010-12-10
	//gain->gain_R = (float)( (temp_r&0xffff00) >> 8) + (float)(temp_r&0xff)/256.0;
	//gain->gain_G = (float)( (temp_g&0xffff00) >> 8) + (float)(temp_g&0xff)/256.0;
	//gain->gain_B = (float)( (temp_b&0xffff00) >> 8) + (float)(temp_b&0xff)/256.0;
//~powerory

	//120103 wonsik.do : removed floating point in kernel driver
	//gain->gain_R = (float)( (temp_r&0xffc000) >> 14) + (float)(temp_r&0x3fff)/16384.0;
	//gain->gain_G = (float)( (temp_g&0xffc000) >> 14) + (float)(temp_g&0x3fff)/16384.0;
	//gain->gain_B = (float)( (temp_b&0xffc000) >> 14) + (float)(temp_b&0x3fff)/16384.0;
	gain->gain_R = temp_r;
	gain->gain_G = temp_g;
	gain->gain_B = temp_b;
	//by dws
	//gain->gain_R = (float)( (temp_r&0xffe000) >> 13) + (float)(temp_r&0x1fff)/8192.0;
	//gain->gain_G = (float)( (temp_g&0xffe000) >> 13) + (float)(temp_g&0x1fff)/8192.0;
	//gain->gain_B = (float)( (temp_b&0xffe000) >> 13) + (float)(temp_b&0x1fff)/8192.0;


	//ADC_DEBUG("IRE level 1 : R[%d] , G[%d] , B[%d] \n",(int)gain->gain_R, (int)gain->gain_G , (int)gain->gain_B );

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
int ADC_L9Ax_SetIRELevel(E_AGC_IRE ire , S_GAIN_TARGET *gain)
{
	int ret = 0;
	unsigned short target_ire;


	switch(ire)
	{
		case 	IRE0:
		case	IRE0_RGB:
		case	IRE0_COMP:
			target_ire = 0;
		break;
		case 	IRE73:		//1
			target_ire = 73;
			ire = IRE73_L9;		//0
		break;
		case 	IRE950:		//2
			target_ire = 950;
			ire = IRE950_L9;	//3
		break;
		case 	IRE1023:
			target_ire = 1023;
		break;

		default:
			target_ire = 0;
			AFE_PRINT("WARNING : Invalid E_AGC_IRE type\n");
		break;
	}

	// set IRE
	//gVportRegBx->ch3_adc_007.ch3reg_adc_ire_sel_man = ire;
	VPORT_I2C_Read((UINT32*)&gafe_3ch_1);
	gafe_3ch_1.afe3ch_ire_sel = ire;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_1);

//	ADC_DEBUG("IRE level set %d\n",ire);

	// set target value
	if(ire == IRE0_COMP){
		gadc_dig_90.r_compare_1 = ((512<<14)&0x300)>>8;
		gadc_dig_91.r_compare_0 = ((512<<14)&0xFF);
		VPORT_I2C_Write((UINT32*)&gadc_dig_90);
		VPORT_I2C_Write((UINT32*)&gadc_dig_91);

		gadc_dig_92.g_compare_1 = ((target_ire<<14)&0x300)>>8;
		gadc_dig_93.g_compare_0 = ((target_ire<<14)&0xFF);
		VPORT_I2C_Write((UINT32*)&gadc_dig_92);
		VPORT_I2C_Write((UINT32*)&gadc_dig_93);

		gadc_dig_94.b_compare_1 = ((512<<14)&0x300)>>8;
		gadc_dig_95.b_compare_0 = ((512<<14)&0xFF);
		VPORT_I2C_Write((UINT32*)&gadc_dig_94);
		VPORT_I2C_Write((UINT32*)&gadc_dig_95);
	}
	else
	{
		gadc_dig_90.r_compare_1 = ((target_ire<<14)&0x300)>>8;
		gadc_dig_91.r_compare_0 = ((target_ire<<14)&0xFF);
		VPORT_I2C_Write((UINT32*)&gadc_dig_90);
		VPORT_I2C_Write((UINT32*)&gadc_dig_91);

		gadc_dig_92.g_compare_1 = ((target_ire<<14)&0x300)>>8;
		gadc_dig_93.g_compare_0 = ((target_ire<<14)&0xFF);
		VPORT_I2C_Write((UINT32*)&gadc_dig_92);
		VPORT_I2C_Write((UINT32*)&gadc_dig_93);

		gadc_dig_94.b_compare_1 = ((target_ire<<14)&0x300)>>8;
		gadc_dig_95.b_compare_0 = ((target_ire<<14)&0xFF);
		VPORT_I2C_Write((UINT32*)&gadc_dig_94);
		VPORT_I2C_Write((UINT32*)&gadc_dig_95);
	}

	giAgocIntDone = 0;		// prevent invoking the interrupt before on time
	ADC_SetIRETestMode(1 , ire);
//	OS_MsecSleep(500);

#if 1
	wait_event_interruptible_timeout(agoc_done , giAgocIntDone != 0 , msecs_to_jiffies(300) );

	giAgocIntDone = 0;
#endif

	ADC_L9Ax_GetIRELevel(gain);
	ADC_SetIRETestMode(0 , ire);
	return ret;
}

int ADC_L9Ax_SetAgocWakeup(void)
{

	giAgocIntDone = 1;
	wake_up_interruptible(&agoc_done);

	return 0;
}

int ADC_L9Ax_GetGainRGB( ST_GAIN_INFORM *gain)
{
	VPORT_I2C_Read((UINT32*)&gadc_dig_0);
	VPORT_I2C_Read((UINT32*)&gadc_dig_1);
	VPORT_I2C_Read((UINT32*)&gadc_dig_2);
	VPORT_I2C_Read((UINT32*)&gadc_dig_3);
	VPORT_I2C_Read((UINT32*)&gadc_dig_4);
	VPORT_I2C_Read((UINT32*)&gadc_dig_5);
	gain->gain_R = gadc_dig_0.r_pre_gain_1 << 8 | gadc_dig_1.r_pre_gain_0;
	gain->gain_G = gadc_dig_2.g_pre_gain_1 << 8 | gadc_dig_3.g_pre_gain_0;
	gain->gain_B = gadc_dig_4.b_pre_gain_1 << 8 | gadc_dig_5.b_pre_gain_0;

	//ADC_DEBUG("%s with value R[0x%x],G[0x%x],B[0x%x]\n", __func__, gain->gain_R, gain->gain_G, gain->gain_B);

	return 0;
}

int ADC_L9Ax_SetGainRGB( ST_GAIN_INFORM *gain)
{
	//ADC_DEBUG("%s with value R[0x%x],G[0x%x],B[0x%x]\n", __func__, gain->gain_R, gain->gain_G, gain->gain_B);

	gadc_dig_0.r_pre_gain_1 = (gain->gain_R >> 8) & 0x3F;
	gadc_dig_1.r_pre_gain_0 = (gain->gain_R ) & 0xFF;
	gadc_dig_2.g_pre_gain_1 = (gain->gain_G >> 8) & 0x3F;
	gadc_dig_3.g_pre_gain_0 = (gain->gain_G ) & 0xFF;
	gadc_dig_4.b_pre_gain_1 = (gain->gain_B >> 8) & 0x3F;
	gadc_dig_5.b_pre_gain_0 = (gain->gain_B ) & 0xFF;

	VPORT_I2C_Write((UINT32*)&gadc_dig_0);
	VPORT_I2C_Write((UINT32*)&gadc_dig_1);
	VPORT_I2C_Write((UINT32*)&gadc_dig_2);
	VPORT_I2C_Write((UINT32*)&gadc_dig_3);
	VPORT_I2C_Write((UINT32*)&gadc_dig_4);
	VPORT_I2C_Write((UINT32*)&gadc_dig_5);

	return 0;
}


int ADC_L9Ax_GetOffsetRGB( S_OFFSET_INFORM *offset)
{

	VPORT_I2C_Read((UINT32*)&gadc_dig_224);
	VPORT_I2C_Read((UINT32*)&gadc_dig_225);
	VPORT_I2C_Read((UINT32*)&gadc_dig_226);
	VPORT_I2C_Read((UINT32*)&gadc_dig_227);
	VPORT_I2C_Read((UINT32*)&gadc_dig_228);
	VPORT_I2C_Read((UINT32*)&gadc_dig_229);
	offset->offset_R = gadc_dig_224.r_pre_offset_1 << 8 | gadc_dig_225.r_pre_offset_0;
	offset->offset_G = gadc_dig_226.g_pre_offset_1 << 8 | gadc_dig_227.g_pre_offset_0;
	offset->offset_B = gadc_dig_228.b_pre_offset_1 << 8 | gadc_dig_229.b_pre_offset_0;

	return 0;
}

int ADC_L9Ax_SetOffsetRGB( S_OFFSET_INFORM *offset)
{

	ADC_DEBUG("%s with value R[0x%x],G[0x%x],B[0x%x]\n", __func__, offset->offset_R, offset->offset_G, offset->offset_B);

	gadc_dig_224.r_pre_offset_1 = (offset->offset_R >> 8) & 0x1F;
	gadc_dig_225.r_pre_offset_0 = offset->offset_R & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_224);
	VPORT_I2C_Write((UINT32*)&gadc_dig_225);

	gadc_dig_226.g_pre_offset_1 = (offset->offset_G >> 8) & 0x1F;
	gadc_dig_227.g_pre_offset_0 = offset->offset_G & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_226);
	VPORT_I2C_Write((UINT32*)&gadc_dig_227);

	gadc_dig_228.b_pre_offset_1 = (offset->offset_B >> 8) & 0x1F;
	gadc_dig_229.b_pre_offset_0 = offset->offset_B & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_228);
	VPORT_I2C_Write((UINT32*)&gadc_dig_229);


	return 0;
}


int ADC_L9Ax_GetSumRGB(S_RGB_SUM *sum)
{
	UINT32 out32 = 0;

	sum->sum_R = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_166);
	VPORT_I2C_Read((UINT32*)&gadc_dig_167);
	VPORT_I2C_Read((UINT32*)&gadc_dig_168);
	VPORT_I2C_Read((UINT32*)&gadc_dig_169);
	out32 =  gadc_dig_166.ap_sum_r_3;
	sum->sum_R |= out32 << 24;
	out32 =  gadc_dig_167.ap_sum_r_2;
	sum->sum_R |= out32 << 16;
	out32 =  gadc_dig_168.ap_sum_r_1;
	sum->sum_R |= out32 << 8;
	out32 =  gadc_dig_169.ap_sum_r_0;
	sum->sum_R |= out32;

	out32 = 0;
	sum->sum_G = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_162);
	VPORT_I2C_Read((UINT32*)&gadc_dig_163);
	VPORT_I2C_Read((UINT32*)&gadc_dig_164);
	VPORT_I2C_Read((UINT32*)&gadc_dig_165);
	out32 =  gadc_dig_162.ap_sum_g_3;
	sum->sum_G |= out32 << 24;
	out32 =  gadc_dig_163.ap_sum_g_2;
	sum->sum_G |= out32 << 16;
	out32 =  gadc_dig_164.ap_sum_g_1;
	sum->sum_G |= out32 << 8;
	out32 =  gadc_dig_165.ap_sum_g_0;
	sum->sum_G |= out32;

	out32 = 0;
	sum->sum_B = 0;
	VPORT_I2C_Read((UINT32*)&gadc_dig_158);
	VPORT_I2C_Read((UINT32*)&gadc_dig_159);
	VPORT_I2C_Read((UINT32*)&gadc_dig_160);
	VPORT_I2C_Read((UINT32*)&gadc_dig_161);
	out32 =  gadc_dig_158.ap_sum_b_3;
	sum->sum_B |= out32 << 24;
	out32 =  gadc_dig_159.ap_sum_b_2;
	sum->sum_B |= out32 << 16;
	out32 =  gadc_dig_160.ap_sum_b_1;
	sum->sum_B |= out32 << 8;
	out32 =  gadc_dig_161.ap_sum_b_0;
	sum->sum_B |= out32;

	return 0;
}

/**
 *
 * Enable SCART RGB input on component 2(from B0 revision board) port for EU model
 *
 * @param	BOOLEAN scart_enable(TRUE:Enable SCART RGB, FALSE:component mode)
 *
*/
int ADC_L9Ax_Enable_Scart_Mode(BOOLEAN scart_enable)
{
//	UINT32	VP2_Intr_Enable_Mask = 0xFFFFFBFC; // Disable Resolution change & SOY1 detection interrupt.
//	static UINT32 	VP2_Intr_Enable_Save = 0;

	AFE_PRINT("%s entered with value [%d]\n", __func__, scart_enable);

	if (scart_enable == TRUE)
	{
		//Interrupt Disable for component1
#if 0
		VP2_Intr_Enable_Save = gVportRegBx->intr_vp2_enable.intr_vp2_enable;
		gVportRegBx->intr_vp2_enable.intr_vp2_enable = VP2_Intr_Enable_Save & VP2_Intr_Enable_Mask;
#endif
		VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
		gafe_3ch_0.afe3ch_selmux = 0x1;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

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
		VPORT_I2C_Read((UINT32*)&gafe_3ch_3);
		gafe_3ch_3.afe3ch_sel_ck = 0;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_3);
	}
	return 0;
}

void ADC_L9Ax_Vport_ALL_Power_Down(void)
{
	/* AFE3CH Power Down Setting */
	//gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0x0;
//	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0;

	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_bpdb = 0;
	gafe_3ch_0.afe3ch_gpdb = 0;
	gafe_3ch_0.afe3ch_rpdb = 0;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	/* LLPLL PDB */
	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.llpll_pdb = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	/* LLPLL/CST SWRST & 3CH_DIG SWRST */
	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	gsoft_reset_0.swrst_pix = 1;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
}

int ADC_L9Ax_Channel_Power_Control(int vref_pdb, int bpdb, int gpdb, int rpdb)
{
//	AFE_PRINT("ADC Channel Power Control vfref_pdb[%d], bpdb[%d], gpdb[%d], rpdb[%d]\n", vref_pdb, bpdb, gpdb, rpdb);

	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	if(bpdb >= 0)
	{
		gafe_3ch_0.afe3ch_bpdb = bpdb;
	}
	if(gpdb >= 0)
	{
		gafe_3ch_0.afe3ch_gpdb = gpdb;
	}
	if(rpdb >= 0)
	{
		gafe_3ch_0.afe3ch_rpdb = rpdb;
	}
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	return 0;
}

int ADC_L9Ax_SetType(LX_AFE_ADC_INPUT_SOURCE_TYPE_T sel_type)
{
	S_BLANK_INFORM blank_value;
	//AFE_PRINT("ADC_SetType \n");
	gADC_type = sel_type;

	if(sel_type == LX_ADC_INPUT_SOURCE_YPBPR)
	{
		ADC_DEBUG("Component Input Mode\n");
		VPORT_I2C_Read((UINT32*)&gafe_3ch_6);
		gafe_3ch_6.afe3ch_sog_outmux = 0;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_6);

		VPORT_I2C_Read((UINT32*)&gafe_3ch_3);
		VPORT_I2C_Read((UINT32*)&gafe_3ch_4);
		gafe_3ch_3.afe3ch_rvclpsel = 1;
		gafe_3ch_3.afe3ch_gvclpsel = 0;
		gafe_3ch_4.afe3ch_bvclpsel = 1;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_3);
		VPORT_I2C_Write((UINT32*)&gafe_3ch_4);

		VPORT_I2C_Read((UINT32*)&gadc_dig_22);
		gadc_dig_22.compover = 0x1;
		gadc_dig_22.compuser = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_22);

		blank_value.r_blank_target_value = 0x400;
		// by powerory 11.07.29 for valid pixel value read
		//blank_value.g_blank_target_value = 0x200;
		blank_value.g_blank_target_value = 0x240;
		blank_value.b_blank_target_value = 0x400;
		// by powerory 11.07.29 for valid pixel value read
		//blank_value.width = 0x4;
		blank_value.width = 0x1;
		blank_value.r_shift_value = 0x0;
		blank_value.g_shift_value = 0x0;
		blank_value.b_shift_value = 0x0;

		ADC_L9Ax_Set_Blank_Value(&blank_value);
			/*
		gadc_dig_205.r_blank_high_1 = 0x4;
		VPORT_I2C_Write((UINT32*)&gadc_dig_205);
		gadc_dig_206.r_blank_high_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_206);

		gadc_dig_207.r_blank_low_1 = 0x3;
		VPORT_I2C_Write((UINT32*)&gadc_dig_207);
		gadc_dig_208.r_blank_low_0 = 0xE0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_208);

		gadc_dig_210.g_blank_high_1 = 0x2;
		VPORT_I2C_Write((UINT32*)&gadc_dig_210);
		gadc_dig_211.g_blank_high_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_211);

		gadc_dig_212.g_blank_low_1 = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_212);
		gadc_dig_213.g_blank_low_0 = 0xF0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_213);

		gadc_dig_216.b_blank_high_1 = 0x4;
		VPORT_I2C_Write((UINT32*)&gadc_dig_216);
		gadc_dig_217.b_blank_high_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_217);

		gadc_dig_218.b_blank_low_1 = 0x3;
		VPORT_I2C_Write((UINT32*)&gadc_dig_218);
		gadc_dig_219.b_blank_low_0 = 0xE0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_219);
		*/

		//gVportRegBx->ch3_digital_002.ch3reg_clamp_base = 0x3;	// SC jung wrote
		/*
		VPORT_I2C_Read((UINT32*)&gadc_dig_50);
		gadc_dig_50.clamp_base = 0x2;
		VPORT_I2C_Write((UINT32*)&gadc_dig_50);

		VPORT_I2C_Read((UINT32*)&gadc_dig_51);
		gadc_dig_51.clamp_place = 0x5;
		VPORT_I2C_Write((UINT32*)&gadc_dig_51);

		VPORT_I2C_Read((UINT32*)&gadc_dig_220);
		gadc_dig_220.clamp_width = 0x40;
		VPORT_I2C_Write((UINT32*)&gadc_dig_220);
		*/

		//added 0716
		VPORT_I2C_Read((UINT32*)&gadc_dig_58);
		gadc_dig_58.hsout_width_sel = 0x1;		// Set to Manual mode '1'  ( For L8 '0' is manual mode )
		VPORT_I2C_Write((UINT32*)&gadc_dig_58);

		//gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block

		//0906
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x0;	// RGB shaking problem => H positon shift problem
		//1210 powerory
		VPORT_I2C_Read((UINT32*)&gadc_dig_61);
		gadc_dig_61.vsout_sel = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_61);

		//If vsout_sel is 0(vs_gen), sometimes vertical sync is not lock to video. Need more debuggint.
		//1214 powerory
		gadc_dig_62.vsdiff_thr_2 = 0x0;
		gadc_dig_63.vsdiff_thr_1 = 0x0;
		gadc_dig_64.vsdiff_thr_0 = 0x80;
		VPORT_I2C_Write((UINT32*)&gadc_dig_62);
		VPORT_I2C_Write((UINT32*)&gadc_dig_63);
		VPORT_I2C_Write((UINT32*)&gadc_dig_64);

		//gadc_dig_65.vsprdeq_thr = 0x0E;
		// reduced vsprdeq_thr value for fast locking on 24/25/30Hz signals.(110712)
		gadc_dig_65.vsprdeq_thr = 0x02;
		VPORT_I2C_Write((UINT32*)&gadc_dig_65);

		//20110114
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x2;
//		gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 0;

		/*
		//20110124 powerory (480i 입력에서 문제점해결위해 수정)
		gadc_dig_52.hsdiff_thr_1 = 0x0;
		gadc_dig_53.hsdiff_thr_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_52);
		VPORT_I2C_Write((UINT32*)&gadc_dig_53);
		*/

		//20110125
		//gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // Select feedback LLPLL Hsync for digital block
		VPORT_I2C_Read((UINT32*)&gadc_dig_88);
		gadc_dig_88.hsync_sel = 0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_88);
	}
	else
	{
		ADC_DEBUG("RGB Input Mode\n");
		VPORT_I2C_Read((UINT32*)&gafe_3ch_6);
		gafe_3ch_6.afe3ch_sog_outmux = 1;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_6);

		VPORT_I2C_Read((UINT32*)&gafe_3ch_3);
		VPORT_I2C_Read((UINT32*)&gafe_3ch_4);
		gafe_3ch_3.afe3ch_rvclpsel = 0;
		gafe_3ch_3.afe3ch_gvclpsel = 0;
		gafe_3ch_4.afe3ch_bvclpsel = 0;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_3);
		VPORT_I2C_Write((UINT32*)&gafe_3ch_4);

		VPORT_I2C_Read((UINT32*)&gadc_dig_22);
		gadc_dig_22.compover = 0x1;
		gadc_dig_22.compuser = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_22);

		blank_value.r_blank_target_value = 0x200;
		blank_value.g_blank_target_value = 0x200;
		blank_value.b_blank_target_value = 0x200;
		// by powerory 11.07.29 for valid pixel value read
		//blank_value.width = 0x4;
		blank_value.width = 0x1;
		blank_value.r_shift_value = 0x0;
		blank_value.g_shift_value = 0x0;
		blank_value.b_shift_value = 0x0;
		ADC_L9Ax_Set_Blank_Value(&blank_value);
		/*
		gadc_dig_205.r_blank_high_1 = 0x2;
		VPORT_I2C_Write((UINT32*)&gadc_dig_205);
		gadc_dig_206.r_blank_high_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_206);

		gadc_dig_207.r_blank_low_1 = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_207);
		gadc_dig_208.r_blank_low_0 = 0xF0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_208);

		gadc_dig_210.g_blank_high_1 = 0x2;
		VPORT_I2C_Write((UINT32*)&gadc_dig_210);
		gadc_dig_211.g_blank_high_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_211);

		gadc_dig_212.g_blank_low_1 = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_212);
		gadc_dig_213.g_blank_low_0 = 0xF0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_213);

		gadc_dig_216.b_blank_high_1 = 0x2;
		VPORT_I2C_Write((UINT32*)&gadc_dig_216);
		gadc_dig_217.b_blank_high_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_217);

		gadc_dig_218.b_blank_low_1 = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_218);
		gadc_dig_219.b_blank_low_0 = 0xF0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_219);
		*/

		/*
		VPORT_I2C_Read((UINT32*)&gadc_dig_50);
		gadc_dig_50.clamp_base = 0x2;
		VPORT_I2C_Write((UINT32*)&gadc_dig_50);

		VPORT_I2C_Read((UINT32*)&gadc_dig_51);
		gadc_dig_51.clamp_place = 0x5;
		VPORT_I2C_Write((UINT32*)&gadc_dig_51);

		VPORT_I2C_Read((UINT32*)&gadc_dig_220);
		gadc_dig_220.clamp_width = 0x10;
		VPORT_I2C_Write((UINT32*)&gadc_dig_220);
		*/

		//added 0716
		//gVportRegBx->ch3_digital_004.ch3reg_hsout_width_sel = 0x1; // Select Auto Hsync output sync width

		// for horizontal sync re-shape in ADC
		VPORT_I2C_Read((UINT32*)&gadc_dig_58);
		gadc_dig_58.hsout_width_sel = 0x1;		// Set to Manual mode '1' ( For L8 '0' is manual mode )
		VPORT_I2C_Write((UINT32*)&gadc_dig_58);

//		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x0; // Select internal Hsync for digital block
//		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // powerory  <== CAN NOT CHANGE HSIZE BY 1 STEP
//		gVportRegBx->ch3_digital_005.ch3reg_hsync_sel = 0x1; // B0 Revision Fixed bug(HSIZE 1 STEP)

		//0906
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x2;	// RGB shaking problem => H positon shift problem
		//1210 powerory
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_61);
		gadc_dig_61.vsout_sel = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_61);

		/*
		//20110114
		//gVportRegBx->ch3_digital_063.ch3reg_hsdiff_thr = 0x2;
		gadc_dig_52.hsdiff_thr_1 = 0x0;
		gadc_dig_53.hsdiff_thr_0 = 0x0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_52);
		VPORT_I2C_Write((UINT32*)&gadc_dig_53);
		*/

//		gVportRegBx->ch3_digital_003.ch3reg_hsoutover = 1;
//		gVportRegBx->ch3_digital_003.ch3reg_hsoutcnt = gVportRegBx->ch3_llpll_003.llpll_counter_max - 1;
		//20110125
		VPORT_I2C_Read((UINT32*)&gadc_dig_88);
		gadc_dig_88.hsync_sel = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_88);
	}

	return 0;
}

int ADC_L9Ax_Set_LLPLL(UINT32 arg)
{

	if(arg >= PARAM_NUM)
		return -1;

		AFE_PRINT("Setting LLPLL to Index Number [%d]\n", arg);

	//ADC_SetInputVideo(arg);

	if(ADC_SetInputVideo(arg) == 0)
		gPrevious_index = 0xffff;

	return 0;

}

int ADC_L9Ax_SetClock(int iIndex, SINT16 value)
{
	UINT16 clock_value;
#ifdef L9_RGB_WA_FOR_CLOCK_ADJUST
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

	VPORT_I2C_Read((UINT32*)&gllpll_2);
	VPORT_I2C_Read((UINT32*)&gllpll_3);
	gllpll_2.llpll_counter_max_1 = (clock_value&0xF00)>>8;
	gllpll_3.llpll_counter_max_0 = clock_value&0xFF;
	VPORT_I2C_Write((UINT32*)&gllpll_2);
	VPORT_I2C_Write((UINT32*)&gllpll_3);

	// 110822 : by wonsik.do, temporal workaround for 720*400, 640*480 RGB-PC clock adjust error at -50.
	// If vactive is less than 480, Do not excute L9_RGB_WA_FOR_CLOCK ADJUST
	if(TB_HEIGHT[iIndex] > 480)
	{
		//by dws : remove mdelay
		//mdelay(10);
		OS_MsecSleep(5);

#ifdef L9_RGB_WA_FOR_CLOCK_ADJUST
		ADC_L9Ax_Get_DCO_MinMax_Value(&dco_min_value, &dco_max_value);
		ADC_L9Ax_Set_DCO_MinMax_Value(0, 0);
#endif

		//by dws : remove mdelay
		//mdelay(10);
		OS_MsecSleep(5);

#ifdef L9_RGB_WA_FOR_CLOCK_ADJUST
		ADC_L9Ax_Set_DCO_MinMax_Value(dco_min_value, dco_max_value);
#endif
	}

	OS_MsecSleep(30);

	return 0;
}


int ADC_L9Ax_Reset_Digital_24MHZ(void)
{

	AFE_PRINT("%s entered. This function empty for this chipset[L9Ax].\n", __func__);

	return 0;
}

int ADC_L9Ax_Reset_Digital(void)
{
	ADC_DEBUG("%s entered\n", __func__);

#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND
	ADC_Channel_Power_Control(-1, 0, -1, 0);
#endif

	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	gsoft_reset_0.swrst_pix = 1;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);

	//by dws : remove mdelay
//	mdelay(5);
	OS_MsecSleep(5);

	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	gsoft_reset_0.swrst_pix = 0;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);

#ifdef L8_RGB_FEEDBACK_HSYNC_WORKAROUND
	ADC_Channel_Power_Control(-1, 1, -1, 1);
#endif

	return 0;
}

int ADC_L9Ax_Reset_Digital_Control(int enable_reset)
{
	ADC_DEBUG("%s entered [%d]\n", __func__, enable_reset);

	if(enable_reset)
	{
		VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
		gsoft_reset_0.swrst_pix = 1;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
		gsoft_reset_0.swrst_pix = 0;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
	}

	return 0;
}

int ADC_L9Ax_Reset_LLPLL(void)
{
	ADC_DEBUG("%s entered\n", __func__);

	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	//by dws : remove mdelay
	//mdelay(5);
	OS_MsecSleep(5);

	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 1;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	return 0;
}

int ADC_L9Ax_Reset_LLPLL_Control(int enable_reset)
{
	ADC_DEBUG("%s entered [%d]\n", __func__, enable_reset);

	if(enable_reset)
	{
		VPORT_I2C_Read((UINT32*)&gllpll_0);
		gllpll_0.reset_n = 0;
		VPORT_I2C_Write((UINT32*)&gllpll_0);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&gllpll_0);
		gllpll_0.reset_n = 1;
		VPORT_I2C_Write((UINT32*)&gllpll_0);
	}

	return 0;
}

int ADC_L9Ax_Sync_Polarity_Override(int set)
{
	ADC_DEBUG("%s entered\n", __func__);
	if(set)
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_21);
		gadc_dig_21.hspolover = 0x1;
		gadc_dig_21.vspolover = 0x1;
		gadc_dig_21.hspoluser = 0x1;
		gadc_dig_21.vspoluser = 0x1;
		VPORT_I2C_Write((UINT32*)&gadc_dig_21);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_21);
		gadc_dig_21.hspolover = 0;
		gadc_dig_21.vspolover = 0;
//		gadc_dig_21.hspoluser = 0;
//		gadc_dig_21.vspoluser = 0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_21);
	}

	return 0;
}



int ADC_L9Ax_Check_Sync_Exist(void)
{

	UINT8	vsdet_value, hsdet_value;

	VPORT_I2C_Read((UINT32*)&gadc_dig_28);

	hsdet_value = gadc_dig_28.ro_hsdet;
	vsdet_value = gadc_dig_28.ro_vsdet;

	if ( ( ( gADC_type == LX_ADC_INPUT_SOURCE_RGB_PC) && (hsdet_value == 1) && (vsdet_value == 1)) || ( ( gADC_type != LX_ADC_INPUT_SOURCE_RGB_PC) && (hsdet_value == 1) ) )
		return 0;
#if 0
	UINT8	no_signal;

	VPORT_I2C_Read((UINT32*)&gadc_dig_121);

	no_signal = gadc_dig_121.no_signal;

	if(no_signal < 1) // signal exist
		return 0;
#endif

	return -1;

}

int ADC_L9Ax_Enable_Detection_Interrupt(int enable)
{
	ADC_DEBUG("%s entered\n", __func__);

	return 0;
}

int ADC_L9Ax_Enable_AOGC_Interrupt(int enable)
{
	ADC_DEBUG("%s entered\n", __func__);
	return 0;
}

int ADC_L9Ax_Mute_Enable(int enable)
{
	ADC_DEBUG("%s entered [%d]\n", __func__, enable);
	if(enable)
	{
		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{
			VPORT_I2C_Read((UINT32*)&gadc_dig_89);
			gadc_dig_89.signedout = 5;
			VPORT_I2C_Write((UINT32*)&gadc_dig_89);
		}
		VPORT_I2C_Read((UINT32*)&gafe_3ch_1);
		gafe_3ch_1.adc_ire_test = 0x1;
		VPORT_I2C_Write((UINT32*)&gafe_3ch_1);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_89);
		VPORT_I2C_Read((UINT32*)&gafe_3ch_1);
		gadc_dig_89.signedout = 0;
		gafe_3ch_1.adc_ire_test = 0;
		VPORT_I2C_Write((UINT32*)&gadc_dig_89);
		VPORT_I2C_Write((UINT32*)&gafe_3ch_1);
	}

	return 0;
}

int ADC_L9Ax_Power_OnOff(void)
{
	ADC_DEBUG("%s entered\n", __func__);

	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_biaspdb = 0;
	gafe_3ch_0.afe3ch_bpdb = 0;
	gafe_3ch_0.afe3ch_gpdb = 0;
	gafe_3ch_0.afe3ch_rpdb = 0;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	//gVportRegBx->ch3_digital_002.ch3reg_extclampsel = 1;
	//gVportRegBx->ch3_digital_002.ch3reg_clamppolover = 1;
	//gVportRegBx->ch3_digital_002.ch3reg_clamppoluser = 1;


	//by dws : remove mdelay
	//mdelay(5);
	OS_MsecSleep(5);

	VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
	gafe_3ch_0.afe3ch_biaspdb = 1;
	gafe_3ch_0.afe3ch_bpdb = 1;
	gafe_3ch_0.afe3ch_gpdb = 1;
	gafe_3ch_0.afe3ch_rpdb = 1;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_0);

	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 0;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	//by dws : remove mdelay
	//mdelay(5);
	OS_MsecSleep(5);

	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gllpll_0.reset_n = 1;
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	return 0;
}

int ADC_L9Ax_Ext_Clamp(int sel_ext)
{
//	static int g_ph_value = 4;

	ADC_DEBUG("%s entered [%d]\n", __func__, sel_ext);
	if(sel_ext)
	{
		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{
//			ADC_Channel_Power_Control(0);
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

		if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
		{
//			mdelay(5);
//			ADC_Channel_Power_Control(1);
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
	}

	return 0;
}

int ADC_L9Ax_Set_Clamp(UINT8 clamp_base, UINT8 clamp_place, UINT8 clamp_width)
{
	if(clamp_base)
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_50);
		gadc_dig_50.clamp_base = clamp_base & 0x3;
		VPORT_I2C_Write((UINT32*)&gadc_dig_50);
	}

	if(clamp_place)
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_51);
		gadc_dig_51.clamp_place = clamp_place;
		VPORT_I2C_Write((UINT32*)&gadc_dig_51);
	}

	if(clamp_width)
	{
		VPORT_I2C_Read((UINT32*)&gadc_dig_220);
		gadc_dig_220.clamp_width = clamp_width;
		VPORT_I2C_Write((UINT32*)&gadc_dig_220);
	}

	return 0;
}

#ifdef	KDRV_CONFIG_PM
int ADC_L9Ax_RunSuspend(void)
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

int ADC_L9Ax_RunResume(void)
{

	ADC_InitDaughterBoard();

	if(gAdcPMData->gstPmSelPortValue.port == 0xfffffff)
		return 0;

	VPORT_RdFL(ch3_digital_026);
	VPORT_Wr01(ch3_digital_026, ch3reg_extau_down_offset, gAdcPMData->gstPmExtauValue.height_offset);
	VPORT_WrFL(ch3_digital_026);

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
int ADC_L9Ax_GetPCModeInfo(LX_AFE_PCMODE_INFO_T *stPCModeInfo)
{
	UINT16	Vline, Vlowline, width16, out16;

	if(gResolution >= 0)
	{
		memset((void *)stPCModeInfo, 0, sizeof(LX_AFE_PCMODE_INFO_T));
		stPCModeInfo->u8_Mode_Idx = gResolution;

		width16 = 0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_75);
		VPORT_I2C_Read((UINT32*)&gadc_dig_76);
		width16 = (gadc_dig_75.ro_hsout_width_1)&0xF;
		width16 = width16 << 8;
		width16 |= gadc_dig_76.ro_hsout_width_0;
		stPCModeInfo->u16_HSyncWidth = width16;

		out16 = 0;
		Vline = 0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_34);
		VPORT_I2C_Read((UINT32*)&gadc_dig_35);
		out16 =  (gadc_dig_34.ro_vsprdref_1)&0x1F;
		Vline |= out16 << 8;
		out16 =  gadc_dig_35.ro_vsprdref_0;
		Vline |= out16;

		out16 = 0;
		Vlowline = 0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_36);
		VPORT_I2C_Read((UINT32*)&gadc_dig_37);
		out16 =  (gadc_dig_36.ro_vslowref_1)&0x1F;
		Vlowline |= out16 << 8;
		out16 =  gadc_dig_37.ro_vslowref_0;
		Vlowline |= out16;

		stPCModeInfo->u8_UI_Phase = ADC_Phase_Convert_to_UI_Value(ADC_GetPhaseValue(), gResolution);
		if(Vlowline > (Vline>>1))
		{
			// Active High VSync Pulse
			stPCModeInfo->u16_VSyncWidth = Vline - Vlowline;
		}
		else
		{
			stPCModeInfo->u16_VSyncWidth = Vlowline;
		}

		return 0;
	}
	else
	{
		return -1;
	}
}

int ADC_L9Ax_Get_LLPLL_Filter_Status(void)
{
	VPORT_I2C_Read((UINT32*)&gllpll_27);

	return gllpll_27.llpll_filter_status;
}

int ADC_L9Ax_Set_Coast(UINT8 precoast, UINT8 postcoast)
{
	VPORT_I2C_Read((UINT32*)&gadc_dig_22);
	VPORT_I2C_Read((UINT32*)&gadc_dig_23);
	gadc_dig_22.precoast = precoast&0x1F;
	gadc_dig_23.postcoast = postcoast&0x1F;
	VPORT_I2C_Write((UINT32*)&gadc_dig_22);
	VPORT_I2C_Write((UINT32*)&gadc_dig_23);

	return 0;
}

int ADC_L9Ax_Set_Hsync_Sel(UINT8 select)
{
	VPORT_I2C_Read((UINT32*)&gadc_dig_88);

	if(select > 0)
	{
		// Select feedback LLPLL Hsync for digital block
		gadc_dig_88.hsync_sel = 0x1;
	}
	else
	{
		gadc_dig_88.hsync_sel = 0;
	}

	VPORT_I2C_Write((UINT32*)&gadc_dig_88);

	return 0;
}

int ADC_L9Ax_CVI_Sync_Exist(void)
{
	//FIXME L8B0 bug, Check if L9 has the same problem.
#if 0
	UINT32 hsync_exist, vsync_exist, src_valid;
	CHA_CVI_RdFL(cha_size_status0);
	CHA_CVI_Rd03(cha_size_status0, hsync_signal_detect, hsync_exist, vsync_signal_detect, vsync_exist, source_valid, src_valid);

	AFE_PRINT("hsync = [%d], vsync = [%d], valid = [%d]\n", hsync_exist, vsync_exist, src_valid);

	if(hsync_exist && vsync_exist/* && src_valid*/)
		return 1;
	else
#endif
	return 0;
}

int ADC_L9Ax_Set_SOG_Level(UINT32 level)
{
	if(level > 0x1F)
		return -1;

	VPORT_I2C_Read((UINT32*)&gafe_3ch_5);
	gafe_3ch_5.afe3ch_soglvl = level&0x1F;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_5);

	ADC_DEBUG("Set SOG Level = [0x%x]\n", level);

	return 0;
}

int ADC_L9Ax_Get_SOG_Level(UINT32 *level)
{
	VPORT_I2C_Read((UINT32*)&gafe_3ch_5);
	*level = gafe_3ch_5.afe3ch_soglvl;

	ADC_DEBUG("Get SOG Level = [0x%x]\n", *level);

	return 0;
}

int ADC_L9Ax_Check_Htotal_Diff(UINT8 count, int Kadp_Index)
{
	UINT8 i;
	UINT16 maxCnt16 = 0;
	UINT16 hsoutprd16 = 0;

	for(i = 0; i < count;i++)
	{
		maxCnt16 = 0;
		hsoutprd16 = 0;

		VPORT_I2C_Read((UINT32*)&gllpll_2);
		VPORT_I2C_Read((UINT32*)&gllpll_3);
		maxCnt16 = (gllpll_2.llpll_counter_max_1)&0xF;
		maxCnt16 = maxCnt16 << 8;
		maxCnt16 |=gllpll_3.llpll_counter_max_0;

		VPORT_I2C_Read((UINT32*)&gadc_dig_81);
		VPORT_I2C_Read((UINT32*)&gadc_dig_82);
		hsoutprd16 = (gadc_dig_81.ro_hsoutprd_1)&0xF;
		hsoutprd16 = hsoutprd16 << 8;
		hsoutprd16 |= gadc_dig_82.ro_hsoutprd_0;

		if( (Kadp_Index == 59) || (Kadp_Index == 61))
		{
			//ADC_DEBUG("##### 480i/576i [%d]!!!\n", hsoutprd16);
			hsoutprd16 = hsoutprd16 >> 1;
		}

		if(abs(maxCnt16 - hsoutprd16) > 0)
		{
			AFE_PRINT("##### Htotal diff [%d] [0x%x][0x%x]!!!\n", i, maxCnt16 , hsoutprd16);
			ADC_Reset_Digital();
			OS_MsecSleep(10);
		}
		else
			return 0;

	}
	return -1;
}

int ADC_L9Ax_Read_Pixel_Value(LX_AFE_ADC_PIXEL_VALUE_T *pixel_value_t)
{
	int ret = 0;
	S_RGB_SUM sum;

	gadc_dig_138.ap_xpos_r_1 = (pixel_value_t->X_Pos&0x0F00)>>8;
	gadc_dig_139.ap_xpos_r_0 = (pixel_value_t->X_Pos&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_138);
	VPORT_I2C_Write((UINT32*)&gadc_dig_139);

	gadc_dig_140.ap_xsize_r_1 = (pixel_value_t->X_Size&0x0F00)>>8;
	gadc_dig_141.ap_xsize_r_0 = (pixel_value_t->X_Size&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_140);
	VPORT_I2C_Write((UINT32*)&gadc_dig_141);

	gadc_dig_142.ap_ypos_r_1 = (pixel_value_t->Y_Pos&0x0F00)>>8;
	gadc_dig_143.ap_ypos_r_0 = (pixel_value_t->Y_Pos&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_142);
	VPORT_I2C_Write((UINT32*)&gadc_dig_143);

	gadc_dig_144.ap_ysize_r_1 = (pixel_value_t->Y_Size&0x0F00)>>8;
	gadc_dig_145.ap_ysize_r_0 = (pixel_value_t->Y_Size&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_144);
	VPORT_I2C_Write((UINT32*)&gadc_dig_145);

	gadc_dig_122.ap_xpos_g_1 = (pixel_value_t->X_Pos&0x0F00)>>8;
	gadc_dig_123.ap_xpos_g_0 = (pixel_value_t->X_Pos&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_122);
	VPORT_I2C_Write((UINT32*)&gadc_dig_123);

	gadc_dig_124.ap_xsize_g_1 = (pixel_value_t->X_Size&0x0F00)>>8;
	gadc_dig_125.ap_xsize_g_0 = (pixel_value_t->X_Size&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_124);
	VPORT_I2C_Write((UINT32*)&gadc_dig_125);

	gadc_dig_126.ap_ypos_g_1 = (pixel_value_t->Y_Pos&0x0F00)>>8;
	gadc_dig_127.ap_ypos_g_0 = (pixel_value_t->Y_Pos&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_126);
	VPORT_I2C_Write((UINT32*)&gadc_dig_127);

	gadc_dig_128.ap_ysize_g_1 = (pixel_value_t->Y_Size&0x0F00)>>8;
	gadc_dig_129.ap_ysize_g_0 = (pixel_value_t->Y_Size&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_128);
	VPORT_I2C_Write((UINT32*)&gadc_dig_129);

	gadc_dig_130.ap_xpos_b_1 = (pixel_value_t->X_Pos&0x0F00)>>8;
	gadc_dig_131.ap_xpos_b_0 = (pixel_value_t->X_Pos&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_130);
	VPORT_I2C_Write((UINT32*)&gadc_dig_131);

	gadc_dig_132.ap_xsize_b_1 = (pixel_value_t->X_Size&0x0F00)>>8;
	gadc_dig_133.ap_xsize_b_0 = (pixel_value_t->X_Size&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_132);
	VPORT_I2C_Write((UINT32*)&gadc_dig_133);

	gadc_dig_134.ap_ypos_b_1 = (pixel_value_t->Y_Pos&0x0F00)>>8;
	gadc_dig_135.ap_ypos_b_0 = (pixel_value_t->Y_Pos&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_134);
	VPORT_I2C_Write((UINT32*)&gadc_dig_135);

	gadc_dig_136.ap_ysize_b_1 = (pixel_value_t->Y_Size&0x0F00)>>8;
	gadc_dig_137.ap_ysize_b_0 = (pixel_value_t->Y_Size&0xFF);
	VPORT_I2C_Write((UINT32*)&gadc_dig_136);
	VPORT_I2C_Write((UINT32*)&gadc_dig_137);

	OS_MsecSleep(100);

	ADC_L9Ax_GetSumRGB(&sum);

	pixel_value_t->Sum_R_Value = sum.sum_R;
	pixel_value_t->Sum_G_Value = sum.sum_G;
	pixel_value_t->Sum_B_Value = sum.sum_B;

	return ret;
}

int ADC_L9Ax_Green_Mid_Level_Select(UINT8 value)
{
	VPORT_I2C_Read((UINT32*)&gafe_3ch_3);
	gafe_3ch_3.afe3ch_gvclpsel = value & 0x1;
	VPORT_I2C_Write((UINT32*)&gafe_3ch_3);

	return 0;
}

int ADC_L9Ax_Read_VSync_Width(UINT32 *pvs_width)
{
	UINT32 v_line_count, v_low_line_count;
	UINT32	h_total;
	UINT16 out16 = 0;

	//For component, read vsync width counted by pixel clock
	if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
	{
		VPORT_I2C_Read((UINT32*)&gllpll_2);
		VPORT_I2C_Read((UINT32*)&gllpll_3);

		h_total = ( (gllpll_2.llpll_counter_max_1 << 8) & 0xF00) | (gllpll_3.llpll_counter_max_0 & 0xFF);

		VPORT_I2C_Read((UINT32*)&gadc_dig_77);
		VPORT_I2C_Read((UINT32*)&gadc_dig_78);
		VPORT_I2C_Read((UINT32*)&gadc_dig_79);
		//converting to line number
		*pvs_width = (((gadc_dig_77.ro_vsout_width_2 << 16) & 0xFF0000) | ((gadc_dig_78.ro_vsout_width_1 << 8) & 0xFF00) | (gadc_dig_79.ro_vsout_width_0 & 0xFF)) / h_total;

	}
	else
	{
		out16 = 0;
		v_line_count = 0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_34);
		VPORT_I2C_Read((UINT32*)&gadc_dig_35);
		out16 =  (gadc_dig_34.ro_vsprdref_1)&0x1F;
		v_line_count |= out16 << 8;
		out16 =  gadc_dig_35.ro_vsprdref_0;
		v_line_count |= out16;

		out16 = 0;
		v_low_line_count = 0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_36);
		VPORT_I2C_Read((UINT32*)&gadc_dig_37);
		out16 =  (gadc_dig_36.ro_vslowref_1)&0x1F;
		v_low_line_count |= out16 << 8;
		out16 =  gadc_dig_37.ro_vslowref_0;
		v_low_line_count |= out16;

		if((v_line_count /2) > v_low_line_count)	// Active Low VSync
			*pvs_width = v_low_line_count;
		else										// Active High VSync
			*pvs_width = v_line_count - v_low_line_count;
	}

	return 0;
}

int ADC_L9Ax_Read_HSync_Width(UINT32 *phs_width)
{
	UINT32 h_line_count, h_low_line_count;
	UINT16 out16 = 0;

	//For component, read hsync width counted by pixel clock
	if(gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
	{

		VPORT_I2C_Read((UINT32*)&gadc_dig_75);
		VPORT_I2C_Read((UINT32*)&gadc_dig_76);

		*phs_width = ((gadc_dig_75.ro_hsout_width_1 << 8) & 0xF00) | (gadc_dig_76.ro_hsout_width_0 & 0xFF);
	}
	else
	{
		out16 = 0;
		h_line_count = 0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_30);
		VPORT_I2C_Read((UINT32*)&gadc_dig_31);
		out16 =  (gadc_dig_30.ro_hsprdref_1)&0x1F;
		h_line_count |= out16 << 8;
		out16 =  gadc_dig_31.ro_hsprdref_0;
		h_line_count |= out16;

		out16 = 0;
		h_low_line_count = 0;
		VPORT_I2C_Read((UINT32*)&gadc_dig_32);
		VPORT_I2C_Read((UINT32*)&gadc_dig_33);
		out16 =  (gadc_dig_32.ro_hslowref_1)&0x1F;
		h_low_line_count |= out16 << 8;
		out16 =  gadc_dig_33.ro_hslowref_0;
		h_low_line_count |= out16;

		if((h_line_count /2) > h_low_line_count)	// Active Low VSync
			*phs_width = h_low_line_count;
		else										// Active High VSync
			*phs_width = h_line_count - h_low_line_count;

	}
	return 0;
}

void ADC_L9Ax_LVDS_Control(int enable)
{
	if(enable > 0)
	{
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_1);
		ghslvdstx2_0.pdb3 = 0x1;
		ghslvdstx2_1.ch_en3 = 0x3f;
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_1);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_1);
		ghslvdstx2_0.pdb3 = 0x0;
		ghslvdstx2_1.ch_en3 = 0x0;
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_1);
	}

	return;
}

void ADC_L9Ax_Set_Blank(int blank_sp)
{
	if(blank_sp > 0)
	{
		//for MSB 2bit
		gadc_dig_202.blank_sp_1 = (blank_sp>>8) & 0x3;
		VPORT_I2C_Write((UINT32*)&gadc_dig_202);
		//for LSB 8bit
		gadc_dig_203.blank_sp_0 = blank_sp & 0xFF;
		VPORT_I2C_Write((UINT32*)&gadc_dig_203);
	}
	return;
}


int ADC_L9Ax_Set_Blank_Value(S_BLANK_INFORM *blank_value_t)
{
	int r_blank_high_value, r_blank_low_value;
	int g_blank_high_value, g_blank_low_value;
	int b_blank_high_value, b_blank_low_value;

	r_blank_high_value = blank_value_t->r_blank_target_value + blank_value_t->width + blank_value_t->r_shift_value;
	r_blank_low_value = blank_value_t->r_blank_target_value - blank_value_t->width + blank_value_t->r_shift_value;

	g_blank_high_value = blank_value_t->g_blank_target_value + blank_value_t->width + blank_value_t->g_shift_value;
	g_blank_low_value = blank_value_t->g_blank_target_value - blank_value_t->width + blank_value_t->g_shift_value;

	b_blank_high_value = blank_value_t->b_blank_target_value + blank_value_t->width + blank_value_t->b_shift_value;
	b_blank_low_value = blank_value_t->b_blank_target_value - blank_value_t->width + blank_value_t->b_shift_value;

	gadc_dig_205.r_blank_high_1 = (r_blank_high_value >> 8) & 0x7;
	gadc_dig_206.r_blank_high_0 = r_blank_high_value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_205);
	VPORT_I2C_Write((UINT32*)&gadc_dig_206);

	gadc_dig_210.g_blank_high_1 = (g_blank_high_value >> 8) & 0x7;
	gadc_dig_211.g_blank_high_0 = g_blank_high_value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_210);
	VPORT_I2C_Write((UINT32*)&gadc_dig_211);

	gadc_dig_216.b_blank_high_1 = (b_blank_high_value >> 8) & 0x7;
	gadc_dig_217.b_blank_high_0 = b_blank_high_value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_216);
	VPORT_I2C_Write((UINT32*)&gadc_dig_217);

	gadc_dig_207.r_blank_low_1 = (r_blank_low_value >> 8) & 0x7;
	gadc_dig_208.r_blank_low_0 = r_blank_low_value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_207);
	VPORT_I2C_Write((UINT32*)&gadc_dig_208);

	gadc_dig_212.g_blank_low_1 = (g_blank_low_value >> 8) & 0x7;
	gadc_dig_213.g_blank_low_0 = g_blank_low_value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_212);
	VPORT_I2C_Write((UINT32*)&gadc_dig_213);

	gadc_dig_218.b_blank_low_1 = (b_blank_low_value >> 8) & 0x7;
	gadc_dig_219.b_blank_low_0 = b_blank_low_value & 0xFF;
	VPORT_I2C_Write((UINT32*)&gadc_dig_218);
	VPORT_I2C_Write((UINT32*)&gadc_dig_219);


	return 0;
}

int ADC_L9Ax_Set_DCO_MinMax_Value(UINT8 dco_min, UINT8 dco_max)
{
	//LLPLL_dco_max
	VPORT_I2C_Read((UINT32*)&gllpll_7);
	VPORT_I2C_Read((UINT32*)&gllpll_8);

	gllpll_7.llpll_dco_max = dco_max;
	gllpll_8.llpll_dco_min = dco_min;

	VPORT_I2C_Write((UINT32*)&gllpll_7);
	VPORT_I2C_Write((UINT32*)&gllpll_8);

	return 0;
}

int ADC_L9Ax_Get_DCO_MinMax_Value(UINT8 *pdco_min, UINT8 *pdco_max)
{
	//LLPLL_dco_max
	VPORT_I2C_Read((UINT32*)&gllpll_7);
	VPORT_I2C_Read((UINT32*)&gllpll_8);

	*pdco_max = gllpll_7.llpll_dco_max;
	*pdco_min = gllpll_8.llpll_dco_min;

	return 0;
}

