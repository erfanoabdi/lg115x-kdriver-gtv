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
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author		wonsik.do
 *  @version	1.0
 *  @date		2010-11-16
 *  @note		Additional information.
 */

/*----------------------------------------------------------------------------------------
  Control Constants
  ----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
  File Inclusions
  ----------------------------------------------------------------------------------------*/
#include <linux/uaccess.h>
#include <linux/irq.h>
#include "cvd_hw.h"
#include "vport_reg_B0.h"
#include "afe_drv.h"
#include "linux/delay.h"

#include "ctop_regs.h"
#include "./../../afe/l9/vport_reg_l9a0.h"
#include "./../../afe/l9/vport_i2c_l9a0.h"
#include "./../../afe/l9/chbcvd_reg_l9.h"
#include "os_util.h"

/*----------------------------------------------------------------------------------------
  Constant Definitions
  ----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
  Macro Definitions
  ----------------------------------------------------------------------------------------*/
#define VPORT_REG_DIRECT_ACCESS 0
#define	HSTART_SHIFT_DUE_TO_DE_CROP_WORKAROUND	1

// CVD Memory Map ( Base Addr:0x7bb00000, End Addr:0x7befffff, Addr Size:0x00400000)
#define CVD_BASE_ADDR	0x7BB00000

//#define L9_SLOW_AGC_WORKAROUND
//#define L9_USE_SYNCTIP_ONLY_MODE
/*----------------------------------------------------------------------------------------
  Type Definitions
  ----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
  External Function Prototype Declarations
  ----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
  External Variables
  ----------------------------------------------------------------------------------------*/
// shadow register
extern volatile VPORT_L9A0_REG_T __iomem *gpRegVPORT_L9A0;
// real
extern volatile VPORT_L9A0_REG_T __iomem *gpRealRegVPORT_L9A0;

extern CVD_STATE_T	Current_State;
extern CVD_STATE_T	Next_State;
extern int main_timer_state;
extern int chb_timer_state;


/*----------------------------------------------------------------------------------------
  global Variables
  ----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
  Static Function Prototypes Declarations
  ----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
  Static Variables
  ----------------------------------------------------------------------------------------*/
extern volatile VPORT_L9A0_REG_T __iomem *gpRegVPORT_L9A0;
extern volatile VPORT_L9A0_REG_T __iomem *gpRealRegVPORT_L9A0;

extern volatile CHBCVD_L9_REG_T __iomem *gpRegCHBCVD_L9;
extern volatile CHBCVD_L9_REG_T __iomem *gpRealRegCHBCVD_L9;

/*========================================================================================
  Implementation Group
  ========================================================================================*/
int CVD_L9Ax_ClampAGC_OnOff(UINT8 on_off)
{
	//JUHEE : no more supported in L9
#if 0
	if(on_off)
	{
		VPORT_L9A0_RdFL(iris_099);
		VPORT_L9A0_Wr01(iris_099, cvd_cvd1_clampagc_on, 0x1);
		VPORT_L9A0_WrFL(iris_099);
	}
	else
	{
		VPORT_L9A0_RdFL(iris_099);
		VPORT_L9A0_Wr01(iris_099, cvd_cvd1_clampagc_on, 0x0);
		VPORT_L9A0_WrFL(iris_099);
	}
#endif
	return 0;
}

int CVD_L9Ax_Clamp_Current_Control(UINT8 value)
{
	if(value <= 0x3)
	{
		VPORT_I2C_Read((UINT32*)&gafe_cvbs_2);
		gafe_cvbs_2.cvbs_icon = value;
		VPORT_I2C_Write((UINT32*)&gafe_cvbs_2);
		return 0;
	}
	return -1;
}

int CVD_L9Ax_Force_Color_Kill(UINT8 color_kill_enable)
{
	if(color_kill_enable)
	{
		VPORT_L9A0_RdFL(iris_097);
		VPORT_L9A0_Wr01(iris_097, ckill, 0x0);
		VPORT_L9A0_WrFL(iris_097);
	}
	else
	{
		VPORT_L9A0_RdFL(iris_097);
		VPORT_L9A0_Wr01(iris_097, ckill, 0x1c00);
		VPORT_L9A0_WrFL(iris_097);
	}
	return 0;
}

/* From L8B0 below Set_Picture_Enhancement function is not used */

void CVD_L9Ax_Set_Picture_Enhancement(BOOLEAN ycrdc_lcr_on, BOOLEAN cyrdc_lcr_on)
{
	CVD_DEBUG("Picture Enhancement Register Setting\n");
	VPORT_L9A0_RdFL(iris_155);
	VPORT_L9A0_Wr06(iris_155, cvd_2dcomb_byp, 0x0, cvd_2dcomb_cband_sel, 0x0, cvd_2dcomb_ccombo_sel, 0x0 , cvd_2dcomb_ycrdc_cvsel, 0x1, cvd_2dcomb_ycrdc_chsel, 0x0, cvd_2dcomb_ycrdc_lrsel, 0x1);
	VPORT_L9A0_WrFL(iris_155);
	VPORT_L9A0_RdFL(iris_156);
	VPORT_L9A0_Wr03(iris_156, cvd_2dcomb_divider_th, 0x2, cvd_2dcomb_ycrdc_yhsymth, 0x20, cvd_2dcomb_ycrdc_yhdiffth, 0x10);
	VPORT_L9A0_WrFL(iris_156);
	VPORT_L9A0_RdFL(iris_157);
	VPORT_L9A0_Wr01(iris_157, cvd_2dcomb_ycrdc_cvdiffth, 0x40);
	VPORT_L9A0_WrFL(iris_157);
	VPORT_L9A0_RdFL(iris_160);
	VPORT_L9A0_Wr05(iris_160, cvd_2dcomb_ycrdc_yvsel, 0x2, cvd_2dcomb_ycrdc_yhsel, 0xE, cvd_2dcomb_ycrdc_hth_on, 0x1, cvd_2dcomb_ycrdc_hth_sel, 0x2, cvd_2dcomb_ycrdc_hth, 0x40);
	VPORT_L9A0_WrFL(iris_160);
	VPORT_L9A0_RdFL(iris_158);
	VPORT_L9A0_Wr03(iris_158, cvd_2dcomb_ycrdc_lrtype, 0x0, cvd_2dcomb_ycrdc_dsel, 0x0, cvd_2dcomb_ycrdc_method, 0x0);
	VPORT_L9A0_WrFL(iris_158);
	if(ycrdc_lcr_on)
	{
		VPORT_L9A0_RdFL(iris_158);
		VPORT_L9A0_Wr01(iris_158, cvd_2dcomb_ycrdc_lcr_on, 0x7);
		VPORT_L9A0_WrFL(iris_158);
	}
	else
	{
		VPORT_L9A0_RdFL(iris_158);
		VPORT_L9A0_Wr01(iris_158, cvd_2dcomb_ycrdc_lcr_on, 0x0);
		VPORT_L9A0_WrFL(iris_158);
	}
}

void CVD_L9Ax_Program_Color_System_Main_cdto_only(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{
	VPORT_L9A0_RdFL(iris_014);
	VPORT_L9A0_Wr03(iris_014, colour_mode, pSet_system_t->reg_colour_mode, vline_625, pSet_system_t->reg_vline_625, hpixel, pSet_system_t->reg_hpixel);
	VPORT_L9A0_WrFL(iris_014);
	VPORT_L9A0_RdFL(iris_015);
	VPORT_L9A0_Wr01(iris_015, reg_ped, pSet_system_t->reg_ped);
	VPORT_L9A0_WrFL(iris_015);
	VPORT_L9A0_RdFL(iris_097);
	VPORT_L9A0_Wr02(iris_097, cagc_gate_start, pSet_system_t->reg_cagc_gate_start, cagc_gate_end, pSet_system_t->reg_cagc_gate_end);
	VPORT_L9A0_WrFL(iris_097);
	VPORT_L9A0_RdFL(iris_030);
	VPORT_L9A0_Wr01(iris_030, cdto_inc, pSet_system_t->reg_cdto_inc);
	VPORT_L9A0_WrFL(iris_030);
	VPORT_L9A0_RdFL(iris_066);
	VPORT_L9A0_Wr01(iris_066, md_noise_th, pSet_system_t->reg_md_noise_th);
	VPORT_L9A0_WrFL(iris_066);
}

void CVD_L9Ax_Program_Color_System_Main_Multi(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{
	VPORT_L9A0_RdFL(iris_mif_gmua_001);
	VPORT_L9A0_Wr01(iris_mif_gmua_001, comb3_buffer_size, pSet_system_t->reg_comb3_buffer_size);
	VPORT_L9A0_WrFL(iris_mif_gmua_001);
	VPORT_L9A0_RdFL(iris_mif_gmua_002);
	VPORT_L9A0_Wr01(iris_mif_gmua_002, fld1_init_rd_pel, pSet_system_t->reg_fld1_init_rd_pel);
	VPORT_L9A0_WrFL(iris_mif_gmua_002);
	VPORT_L9A0_RdFL(iris_mif_gmua_003);
	VPORT_L9A0_Wr01(iris_mif_gmua_003, fld2_init_rd_pel, pSet_system_t->reg_fld2_init_rd_pel);
	VPORT_L9A0_WrFL(iris_mif_gmua_003);
	VPORT_L9A0_RdFL(iris_mif_gmua_004);
	VPORT_L9A0_Wr01(iris_mif_gmua_004, fld3_init_rd_pel, pSet_system_t->reg_fld3_init_rd_pel);
	VPORT_L9A0_WrFL(iris_mif_gmua_004);
	VPORT_L9A0_RdFL(iris_mif_gmua_005);
	VPORT_L9A0_Wr01(iris_mif_gmua_005, fld4_init_rd_pel, pSet_system_t->reg_fld4_init_rd_pel);
	VPORT_L9A0_WrFL(iris_mif_gmua_005);


	// by Kim.min 2010/06/03
	VPORT_L9A0_RdFL(iris_014);
	VPORT_L9A0_Wr03(iris_014, colour_mode, pSet_system_t->reg_colour_mode, vline_625, pSet_system_t->reg_vline_625, hpixel, pSet_system_t->reg_hpixel);
	VPORT_L9A0_WrFL(iris_014);
	VPORT_L9A0_RdFL(iris_015);
	VPORT_L9A0_Wr01(iris_015, reg_ped, pSet_system_t->reg_ped);
	VPORT_L9A0_WrFL(iris_015);
	//added 110411 by kim.min
	VPORT_L9A0_RdFL(iris_178);
	VPORT_L9A0_Wr01(iris_178, adc_blank_level, pSet_system_t->reg_adc_blank_level);
	VPORT_L9A0_WrFL(iris_178);
	VPORT_L9A0_RdFL(iris_016);
	//110817 moved agc_half_en, dagc_en, dc_clamp_mode, mv_hagc_mode, hagc_field_mode to Reg_Init Function
	VPORT_L9A0_Wr02(iris_016, hagc_en, pSet_system_t->reg_hagc_en, cagc_en, pSet_system_t->reg_cagc_en );
	//VPORT_L9A0_Wr07(iris_016, hagc_en, pSet_system_t->reg_hagc_en, cagc_en, pSet_system_t->reg_cagc_en, agc_half_en, pSet_system_t->reg_agc_half_en, dagc_en, pSet_system_t->reg_dagc_en, dc_clamp_mode, pSet_system_t->reg_dc_clamp_mode, mv_hagc_mode, pSet_system_t->reg_mv_hagc_mode, hagc_field_mode, pSet_system_t->reg_hagc_field_mode);
	VPORT_L9A0_WrFL(iris_016);
	/*
	   // moved to CVD PE
	VPORT_L9A0_RdFL(iris_015);
	VPORT_L9A0_Wr01(iris_015, chroma_bw_lo, pSet_system_t->reg_chroma_bw_lo);
	VPORT_L9A0_WrFL(iris_015);
	*/
	VPORT_L9A0_RdFL(iris_017);
	VPORT_L9A0_Wr02(iris_017, adaptive_mode, pSet_system_t->reg_adaptive_mode, adaptive_3dcomb_mode, pSet_system_t->reg_adaptive_3dcomb_mode);
	VPORT_L9A0_WrFL(iris_017);
	VPORT_L9A0_RdFL(iris_018);
	VPORT_L9A0_Wr01(iris_018, reg_hagc, pSet_system_t->reg_hagc);
	VPORT_L9A0_WrFL(iris_018);
	VPORT_L9A0_RdFL(iris_020);
	VPORT_L9A0_Wr01(iris_020, adc_updn_swap, pSet_system_t->reg_adc_updn_swap);
	VPORT_L9A0_WrFL(iris_020);
	//yc_delay setting is moved to CVD PQ
	//gVportRegBx->iris_021.reg_yc_delay = pSet_system_t->reg_yc_delay;
	//VPORT_L9A0_RdFL(iris_021);
	//VPORT_L9A0_Wr01(iris_021, yc_delay, pSet_system_t->reg_yc_delay);
	//VPORT_L9A0_WrFL(iris_021);
	// need Reg debug
	//
	VPORT_L9A0_RdFL(iris_097);
	VPORT_L9A0_Wr02(iris_097, cagc_gate_start, pSet_system_t->reg_cagc_gate_start, cagc_gate_end, pSet_system_t->reg_cagc_gate_end);
	VPORT_L9A0_WrFL(iris_097);
	VPORT_L9A0_RdFL(iris_030);
	VPORT_L9A0_Wr01(iris_030, cdto_inc, pSet_system_t->reg_cdto_inc);
	VPORT_L9A0_WrFL(iris_030);
	VPORT_L9A0_RdFL(iris_031);
	VPORT_L9A0_Wr01(iris_031, hdto_inc, pSet_system_t->reg_hdto_inc);
	VPORT_L9A0_WrFL(iris_031);


	//if(gpRealRegVPORT_L9A0->iris_147.cres_clksel > 0)	// 54MHz sample clock
	if(1)	// L9Ax use 54MHz sample clock only.
	{
		VPORT_L9A0_RdFL(iris_037);
		VPORT_L9A0_Wr01(iris_037, hactive_start, pSet_system_t->reg_hactive_start_54M);
		VPORT_L9A0_Wr01(iris_037, hactive_width, pSet_system_t->reg_hactive_width_54M);
		VPORT_L9A0_WrFL(iris_037);
	}
	else
	{
		VPORT_L9A0_RdFL(iris_037);
		VPORT_L9A0_Wr01(iris_037, hactive_width, pSet_system_t->reg_hactive_width);
		VPORT_L9A0_Wr01(iris_037, hactive_start, pSet_system_t->reg_hactive_start);
		VPORT_L9A0_WrFL(iris_037);
	}
	VPORT_L9A0_RdFL(iris_038);
	VPORT_L9A0_Wr02(iris_038, vactive_start, pSet_system_t->reg_vactive_start, vactive_height, pSet_system_t->reg_vactive_height);
	VPORT_L9A0_WrFL(iris_038);
	VPORT_L9A0_RdFL(iris_017);
	VPORT_L9A0_Wr01(iris_017, ntsc443_mode, pSet_system_t->reg_ntsc443_mode);
	VPORT_L9A0_WrFL(iris_017);
	VPORT_L9A0_RdFL(iris_023);
	VPORT_L9A0_Wr01(iris_023, reg_cagc, pSet_system_t->reg_cagc);
	VPORT_L9A0_WrFL(iris_023);
	VPORT_L9A0_RdFL(iris_036);
	VPORT_L9A0_Wr01(iris_036, burst_gate_end, pSet_system_t->reg_burst_gate_end);
	VPORT_L9A0_WrFL(iris_036);
	VPORT_L9A0_RdFL(iris_046);
	VPORT_L9A0_Wr01(iris_046, secam_ybw, pSet_system_t->reg_secam_ybw);
	VPORT_L9A0_WrFL(iris_046);
	VPORT_L9A0_RdFL(iris_047);
	VPORT_L9A0_Wr01(iris_047, auto_secam_level, pSet_system_t->reg_auto_secam_level);
	VPORT_L9A0_WrFL(iris_047);
	VPORT_L9A0_RdFL(iris_048);
	VPORT_L9A0_Wr01(iris_048, lose_chromalock_mode, pSet_system_t->reg_lose_chromalock_mode);
	VPORT_L9A0_WrFL(iris_048);
	VPORT_L9A0_RdFL(iris_062);
	VPORT_L9A0_Wr06(iris_062, noise_th, pSet_system_t->reg_noise_th, noise_th_en, pSet_system_t->reg_noise_th_en, lowfreq_vdiff_gain, pSet_system_t->reg_lowfreq_vdiff_gain, chroma_vdiff_gain, pSet_system_t->reg_chroma_vdiff_gain, horiz_diff_ygain, pSet_system_t->reg_horiz_diff_ygain, horiz_diff_cgain, pSet_system_t->reg_horiz_diff_cgain);
	VPORT_L9A0_WrFL(iris_062);
	VPORT_L9A0_RdFL(iris_063);
	VPORT_L9A0_Wr04(iris_063, y_noise_th_gain, pSet_system_t->reg_y_noise_th_gain, c_noise_th_gain, pSet_system_t->reg_c_noise_th_gain, burst_noise_th_gain, pSet_system_t->reg_burst_noise_th_gain, vadap_burst_noise_th_gain, pSet_system_t->reg_vadap_burst_noise_th_gain);
	VPORT_L9A0_WrFL(iris_063);
	VPORT_L9A0_RdFL(iris_064);
	// 110623 : by kim.min adaptive_chroma_mode will be changed according to the input condition(RF or AV).
	//VPORT_L9A0_Wr02(iris_064, motion_mode, pSet_system_t->reg_motion_mode, adaptive_chroma_mode, pSet_system_t->reg_adaptive_chroma_mode);
	VPORT_L9A0_Wr01(iris_064, motion_mode, pSet_system_t->reg_motion_mode);
	VPORT_L9A0_WrFL(iris_064);
	VPORT_L9A0_RdFL(iris_065);
	VPORT_L9A0_Wr02(iris_065, comb2d_only, pSet_system_t->reg_comb2d_only, fb_sync, pSet_system_t->reg_fb_sync);
	VPORT_L9A0_WrFL(iris_065);
	VPORT_L9A0_RdFL(iris_066);
	VPORT_L9A0_Wr02(iris_066, md_noise_th, pSet_system_t->reg_md_noise_th, md_noise_th_en, pSet_system_t->reg_md_noise_th_en);
	VPORT_L9A0_WrFL(iris_066);
	VPORT_L9A0_RdFL(iris_070);
	VPORT_L9A0_Wr02(iris_070, vactive_md_start, pSet_system_t->reg_vactive_md_start, vactive_md_height, pSet_system_t->reg_vactive_md_height);
	VPORT_L9A0_WrFL(iris_070);
	VPORT_L9A0_RdFL(iris_091);
	VPORT_L9A0_Wr03(iris_091, hactive_md_start, pSet_system_t->reg_hactive_md_start, hactive_md_width, pSet_system_t->reg_hactive_md_width, motion_config, pSet_system_t->reg_motion_config);
	VPORT_L9A0_WrFL(iris_091);
	VPORT_L9A0_RdFL(iris_092);
	VPORT_L9A0_Wr01(iris_092, status_motion_mode, pSet_system_t->reg_status_motion_mode);
	VPORT_L9A0_WrFL(iris_092);
	// need Reg debug
	//	gVportRegBx->iris_093.reg_frame_motion_th = pSet_system_t->reg_frame_motion_th;
	//110127
	//	_iow(&gVportRegBx->iris_093, 8, 16, pSet_system_t->reg_frame_motion_th);
	// need Reg debug
	//	gVportRegBx->iris_088.reg_chroma_level = pSet_system_t->reg_chroma_level;
	//	gVportRegBx->iris_088.reg_hf_luma_chroma_offset = pSet_system_t->reg_hf_luma_chroma_offset;
	//	_iow(&gVportRegBx->iris_088, 8, 24, pSet_system_t->reg_chroma_level);
	//	_iow(&gVportRegBx->iris_088, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);
	VPORT_L9A0_RdFL(iris_158);
	VPORT_L9A0_Wr01(iris_158, cvd_2dcomb_ycrdc_lcr_on, pSet_system_t->reg_2dcomb_ycrdc_lcr_on);
	VPORT_L9A0_WrFL(iris_158);
	// kim.min 0622
	VPORT_L9A0_RdFL(iris_095);
	VPORT_L9A0_Wr03(iris_095, cordic_gate_end, pSet_system_t->reg_cordic_gate_end, cordic_gate_start, pSet_system_t->reg_cordic_gate_start, phase_offset_range, pSet_system_t->reg_phase_offset_range);
	VPORT_L9A0_WrFL(iris_095);
	// kim.min 0716
	//	_iow(&gVportRegBx->iris_088, 8, 0, pSet_system_t->reg_tcomb_chroma_level);
	//	_iow(&gVportRegBx->iris_088, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);
	//	_iow(&gVportRegBx->iris_088, 8, 24, pSet_system_t->reg_chroma_level);
	//kim.min 1103
	// setting of below register is moved to CVD_L9Ax_Set_Output_Range()
	//	gVportRegBx->iris_166.reg_oadj_y_offo = pSet_system_t->reg_oadj_y_offo;
	//kim.min 0906
	// setting of below register is moved to CVD_L9Ax_Set_Output_Range()
	//	gVportRegBx->iris_167.reg_oadj_y_coeff = pSet_system_t->reg_oadj_y_coeff;
	VPORT_L9A0_RdFL(iris_017);
	VPORT_L9A0_Wr01(iris_017, pal60_mode, pSet_system_t->reg_pal60_mode);
	VPORT_L9A0_WrFL(iris_017);
	//kim.min 0920
	VPORT_L9A0_RdFL(iris_028);
	VPORT_L9A0_Wr01(iris_028, hstate_max, pSet_system_t->reg_hstate_max);
	VPORT_L9A0_WrFL(iris_028);

	//by dws : remove mdelay
	//mdelay(10); //0619
	OS_MsecSleep(5);

	//No use
//	VPORT_L9A0_RdFL(top_005);
//	VPORT_L9A0_Wr01(top_005, swrst_irisyc, 0);
//	VPORT_L9A0_WrFL(top_005);
	//added 0212 for better 3dcomb operation on RF signal.
	VPORT_L9A0_RdFL(iris_025);
	VPORT_L9A0_Wr01(iris_025, hnon_std_threshold, pSet_system_t->reg_hnon_std_threshold);
	VPORT_L9A0_WrFL(iris_025);

	//added 110415 for Jo Jo Gunpo filed stream : no signal issue
	VPORT_L9A0_RdFL(iris_079);
	VPORT_L9A0_Wr01(iris_079, vsync_signal_thresh, pSet_system_t->reg_vsync_signal_thresh);
	VPORT_L9A0_WrFL(iris_079);

   //added 110608 ( for PAL Test(Sub Carrier Pull in Range) reg_fixed_cstate : 1, reg_cstate : 7 )
	/*
	VPORT_L9A0_RdFL(iris_050);
	VPORT_L9A0_Wr02(iris_050, fixed_cstate, pSet_system_t->reg_fixed_cstate, cstate, pSet_system_t->reg_cstate);
	VPORT_L9A0_WrFL(iris_050);
	*/
	// For proper color system detection, at first set cstate value to default.

#if 0
   //gogosing added 110610 (for russia ATV field stream color system issue)	// 이곳에 두면 재현 잘 됨
   		VPORT_L9A0_RdFL(iris_298);
		VPORT_L9A0_Wr01(iris_298, cs_chroma_burst5or10, pSet_system_t->cs_chroma_burst5or10);
		VPORT_L9A0_WrFL(iris_298);

		VPORT_L9A0_RdFL(iris_309);
		VPORT_L9A0_Wr01(iris_309, cs1_chroma_burst5or10, pSet_system_t->cs1_chroma_burst5or10);
		VPORT_L9A0_WrFL(iris_309);
#endif

}

void CVD_L9Ax_Program_Color_System_CS(CVD_SELECT_CDETECT_T cs_sel, CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{

	if (cs_sel == CVD_SEL_CS_CS0) // select cs0;
	{
		VPORT_L9A0_RdFL(iris_295);
		VPORT_L9A0_Wr01(iris_295, cs_adaptive_chroma_mode, pSet_system_t->reg_adaptive_chroma_mode);
		VPORT_L9A0_Wr01(iris_295, cs_auto_secam_level, pSet_system_t->reg_auto_secam_level);
		VPORT_L9A0_WrFL(iris_295);

		VPORT_L9A0_RdFL(iris_296);
		//gogoging SECAM threshold for keeping PAL stable 20110613
		VPORT_L9A0_Wr01(iris_296, cs_issecam_th, pSet_system_t->cs_issecam_th);
		VPORT_L9A0_Wr01(iris_296, cs_phase_offset_range, pSet_system_t->reg_phase_offset_range);
		VPORT_L9A0_WrFL(iris_296);

		VPORT_L9A0_RdFL(iris_298);
		VPORT_L9A0_Wr01(iris_298, cs_secam_ybw, pSet_system_t->reg_secam_ybw);
		VPORT_L9A0_Wr01(iris_298, cs_adaptive_mode, pSet_system_t->reg_adaptive_mode);
		VPORT_L9A0_Wr01(iris_298, cs_colour_mode, pSet_system_t->reg_colour_mode);
		VPORT_L9A0_Wr01(iris_298, cs_ntsc443_mode, pSet_system_t->reg_ntsc443_mode);
		VPORT_L9A0_Wr01(iris_298, cs_pal60_mode, pSet_system_t->reg_pal60_mode);
		//gogosing PAL에서 0x5c  설정 시 FSC position  테스트 시 secam, pal transition 지속됨 --> 326e 원복 110627
		//cs0, cs1 burst gate width reg. ready
		VPORT_L9A0_Wr01(iris_298, cs_chroma_burst5or10, pSet_system_t->cs_chroma_burst5or10);
		VPORT_L9A0_Wr01(iris_298, cs_cagc_en, pSet_system_t->reg_cagc_en);
		VPORT_L9A0_WrFL(iris_298);

		VPORT_L9A0_RdFL(iris_299);
		VPORT_L9A0_Wr01(iris_299, cs_cagc, pSet_system_t->reg_cagc);
		VPORT_L9A0_WrFL(iris_299);

		VPORT_L9A0_RdFL(iris_300);
		VPORT_L9A0_Wr01(iris_300, cs_chroma_bw_lo, pSet_system_t->reg_chroma_bw_lo);
		VPORT_L9A0_WrFL(iris_300);

		VPORT_L9A0_RdFL(iris_301);
		VPORT_L9A0_Wr01(iris_301, cs_cdto_inc, pSet_system_t->reg_cdto_inc);
		VPORT_L9A0_WrFL(iris_301);

		VPORT_L9A0_RdFL(iris_302);
		VPORT_L9A0_Wr01(iris_302, cs_lose_chromalock_mode, pSet_system_t->reg_lose_chromalock_mode);
		VPORT_L9A0_WrFL(iris_302);

		//added 110608 ( for PAL Test(Sub Carrier Pull in Range) reg_fixed_cstate : 1, reg_cstate : 7 )
		/*
		VPORT_L9A0_RdFL(iris_302);
		VPORT_L9A0_Wr02(iris_302, cs_fixed_cstate, pSet_system_t->reg_fixed_cstate, cs_cstate, pSet_system_t->reg_cstate);
		VPORT_L9A0_WrFL(iris_302);
		*/
	}
	else	// cs1 selected
	{
		VPORT_L9A0_RdFL(iris_306);
		VPORT_L9A0_Wr01(iris_306, cs1_adaptive_chroma_mode, pSet_system_t->reg_adaptive_chroma_mode);
		VPORT_L9A0_Wr01(iris_306, cs1_auto_secam_level, pSet_system_t->reg_auto_secam_level);
		VPORT_L9A0_WrFL(iris_306);

		VPORT_L9A0_RdFL(iris_307);
		VPORT_L9A0_Wr01(iris_307, cs1_phase_offset_range, pSet_system_t->reg_phase_offset_range);
		VPORT_L9A0_WrFL(iris_307);

		VPORT_L9A0_RdFL(iris_309);
		VPORT_L9A0_Wr01(iris_309, cs1_secam_ybw, pSet_system_t->reg_secam_ybw);
		VPORT_L9A0_Wr01(iris_309, cs1_adaptive_mode, pSet_system_t->reg_adaptive_mode);
		VPORT_L9A0_Wr01(iris_309, cs1_colour_mode, pSet_system_t->reg_colour_mode);
		VPORT_L9A0_Wr01(iris_309, cs1_ntsc443_mode, pSet_system_t->reg_ntsc443_mode);
		VPORT_L9A0_Wr01(iris_309, cs1_pal60_mode, pSet_system_t->reg_pal60_mode);
		//gogosing PAL에서 0x5c  설정 시 FSC position  테스트 시 secam, pal transition 지속됨 --> 326e 원복 110627
		//cs0, cs1 burst gate width reg. ready
		VPORT_L9A0_Wr01(iris_309, cs1_chroma_burst5or10, pSet_system_t->cs1_chroma_burst5or10);
		VPORT_L9A0_Wr01(iris_309, cs1_cagc_en, pSet_system_t->reg_cagc_en);
		VPORT_L9A0_WrFL(iris_309);

		VPORT_L9A0_RdFL(iris_310);
		VPORT_L9A0_Wr01(iris_310, cs1_cagc, pSet_system_t->reg_cagc);
		VPORT_L9A0_WrFL(iris_310);

		VPORT_L9A0_RdFL(iris_311);
		VPORT_L9A0_Wr01(iris_311, cs1_chroma_bw_lo, pSet_system_t->reg_chroma_bw_lo);
		VPORT_L9A0_WrFL(iris_311);

		VPORT_L9A0_RdFL(iris_312);
		VPORT_L9A0_Wr01(iris_312, cs1_cdto_inc, pSet_system_t->reg_cdto_inc);
		VPORT_L9A0_WrFL(iris_312);

		VPORT_L9A0_RdFL(iris_313);
		VPORT_L9A0_Wr01(iris_313, cs1_lose_chromalock_mode, pSet_system_t->reg_lose_chromalock_mode);
		VPORT_L9A0_WrFL(iris_313);

		//added 110608 ( for PAL Test(Sub Carrier Pull in Range) reg_fixed_cstate : 1, reg_cstate : 7 )
		/*
		VPORT_L9A0_RdFL(iris_313);
		VPORT_L9A0_Wr02(iris_313, cs1_fixed_cstate, pSet_system_t->reg_fixed_cstate, cs1_cstate, pSet_system_t->reg_cstate);
		VPORT_L9A0_WrFL(iris_313);
		*/
	}

	return;
}

void CVD_L9Ax_Program_Color_System_PreJob(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{
	// For proper color system detection, at first set cstate value to default.
	VPORT_L9A0_RdFL(iris_050);
	VPORT_L9A0_Wr02(iris_050, fixed_cstate, pSet_system_t->reg_fixed_cstate, cstate, pSet_system_t->reg_cstate);
	VPORT_L9A0_WrFL(iris_050);
}

void CVD_L9Ax_Program_Color_System_PostJob(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{
   //added 110608 ( for PAL Test(Sub Carrier Pull in Range) reg_fixed_cstate : 1, reg_cstate : 7 )
	VPORT_L9A0_RdFL(iris_050);
	VPORT_L9A0_Wr02(iris_050, fixed_cstate, pSet_system_t->reg_fixed_cstate, cstate, pSet_system_t->reg_cstate);
	VPORT_L9A0_WrFL(iris_050);
}

void CVD_L9Ax_Program_Color_System_CHB_NTSC(void)
{
	// the reg. name "swrst_chbcvd" is changed to "chbreg_soft_reset"
	//VPORT_L9A0_RdFL(top_007);
	//VPORT_L9A0_Wr01(top_007, swrst_chbcvd, 1);
	//VPORT_L9A0_WrFL(top_007);
	CHBCVD_L9_RdFL(chbcvd_001);
	CHBCVD_L9_Wr01(chbcvd_001, chbreg_soft_reset, 1);
	CHBCVD_L9_WrFL(chbcvd_001);

	CHBCVD_L9_RdFL(chbcvd_001);
	CHBCVD_L9_Wr03(chbcvd_001, chbreg_colour_mode, 0x0, chbreg_vline_625, 0x0, chbreg_hpixel, 0x0);
	CHBCVD_L9_WrFL(chbcvd_001);
	CHBCVD_L9_RdFL(chbcvd_002);
	CHBCVD_L9_Wr01(chbcvd_002, chbreg_ped, 0x1);
	CHBCVD_L9_WrFL(chbcvd_002);
	//	gVportRegBx->chbcvd_003.chbreg_cagc_en = 0x1;
	//	gVportRegBx->chbcvd_002.chbreg_hagc_half_en = 0x1;		//check
	//	gVportRegBx->chbcvd_002.chbreg_dc_clamp_mode = 0x0;
	//	gVportRegBx->chbcvd_002.chbreg_hagc_field_mode = 0x0;
	//	gVportRegBx->chbcvd_002.chbreg_chroma_bw_lo = 0x0;
	//	gVportRegBx->chbcvd_004.chbreg_adaptive_mode = 0x0;
	CHBCVD_L9_RdFL(chbcvd_004);
	CHBCVD_L9_Wr01(chbcvd_004, chbreg_hagc, 0xdd);
	CHBCVD_L9_WrFL(chbcvd_004);
	CHBCVD_L9_RdFL(chbcvd_010);
	CHBCVD_L9_Wr01(chbcvd_010, chbreg_cagc, 0xa8);
	CHBCVD_L9_WrFL(chbcvd_010);
	CHBCVD_L9_RdFL(chbcvd_017);
	CHBCVD_L9_Wr01(chbcvd_017, chbreg_cdto_inc, 0x21f07c1f);
	CHBCVD_L9_WrFL(chbcvd_017);
	CHBCVD_L9_RdFL(chbcvd_022);
	CHBCVD_L9_Wr02(chbcvd_022, chbreg_dr_freq, 0x00, chbreg_db_freq, 0x00);
	CHBCVD_L9_WrFL(chbcvd_022);
	CHBCVD_L9_RdFL(chbcvd_024);
	CHBCVD_L9_Wr02(chbcvd_024, chbreg_hactive_start, 0x82, chbreg_hactive_width, 0x50);
	CHBCVD_L9_WrFL(chbcvd_024);
	CHBCVD_L9_RdFL(chbcvd_027);
	CHBCVD_L9_Wr02(chbcvd_027, chbreg_vactive_start, 0x22, chbreg_vactive_height, 0x61);
	CHBCVD_L9_WrFL(chbcvd_027);
	CHBCVD_L9_RdFL(chbcvd_048);
	CHBCVD_L9_Wr01(chbcvd_048, chbreg_lose_chromalock_mode, 0x3);
	CHBCVD_L9_WrFL(chbcvd_048);
	CHBCVD_L9_RdFL(chbcvd_056);
	CHBCVD_L9_Wr01(chbcvd_056, chbreg_cagc_tc_ibig, 0x4);
	CHBCVD_L9_WrFL(chbcvd_056);
	//add by ok. 20110317
	CHBCVD_L9_RdFL(chbcvd_057);
	CHBCVD_L9_Wr02(chbcvd_057, chbreg_dcrestore_no_bad_bp, 0x1, chbreg_dcrestore_kill_enable, 0x1);
	CHBCVD_L9_WrFL(chbcvd_057);
	CHBCVD_L9_RdFL(chbcvd_034);
	CHBCVD_L9_Wr03(chbcvd_034, chbreg_cpump_auto_stip_unlocked, 0x1, chbreg_cpump_auto_stip_no_signal, 0x1, chbreg_cpump_auto_stip_noisy, 0x1);
	CHBCVD_L9_WrFL(chbcvd_034);
	CHBCVD_L9_RdFL(chbcvd_036);
	CHBCVD_L9_Wr04(chbcvd_036, chbreg_cpump_up_max, 0x7f, chbreg_cpump_diff_signal_only, 0x1, chbreg_cpump_up_diff_max, 0x7f, chbreg_cpump_diff_noisy_only, 0x1);
	CHBCVD_L9_WrFL(chbcvd_036);
	CHBCVD_L9_RdFL(chbcvd_037);
	CHBCVD_L9_Wr01(chbcvd_037, chbreg_cpump_dn_diff_max, 0x0);
	CHBCVD_L9_WrFL(chbcvd_037);
	/*
	   gVportRegBx->chbreg_adc_updn_swap.chbreg_adc_updn_swap = 0x1;
	   gVportRegBx->chbreg_yc_delay.chbreg_yc_delay = 0x0;
	   gVportRegBx->chbreg_hdto_inc.chbreg_hdto_inc = 0x20000000;
	   gVportRegBx->chbcvd_022.chbreg_dr_freq	= 0x0;
	   gVportRegBx->chbcvd_022.chbreg_db_freq	= 0x0;
	   gVportRegBx->chbcvd_001.chbreg_ntsc443_mode = 0x0;
	   _iow(&gVportRegBx->chbcvd_023, 8, 0, 0x46); // chbreg_burst_gate_end
	   gVportRegBx->chbcvd_047.chbreg_secam_ybw = 0x0;
	   gVportRegBx->chbcvd_047.chbreg_auto_secam_level = 0x0;
	   gVportRegBx->chbcvd_052.chbreg_noise_th = 0x2;
	   gVportRegBx->chbreg_cpump_adjust_polarity.chbreg_noise_th_en = 0x0;
	   gVportRegBx->chbcvd_004.chbreg_adaptive_chroma_mode = 0x1;
	 */
	CHBCVD_L9_RdFL(chbcvd_001);
	CHBCVD_L9_Wr01(chbcvd_001, chbreg_soft_reset, 0);
	CHBCVD_L9_WrFL(chbcvd_001);
}

#if 0
void CVD_L9Ax_ALL_Power_Down(void)
{
	/*CVBS ADC Power Down Setting */
	VPORT_I2C_Read((UINT32*)&gafe_cvbs_1);
	VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
	VPORT_I2C_Read((UINT32*)&gafe_vdac_0);
	VPORT_I2C_Read((UINT32*)&gafe_vdac_1);
	VPORT_I2C_Read((UINT32*)&gafe_vbuf_0);
	VPORT_I2C_Read((UINT32*)&gafe_vbuf_1);

	gafe_cvbs_1.cvbs_pdbm = 0;
	gafe_cvbs_3.cvbs_pdb = 0;

	/* VDAC & BUFFER Power Down Setting */
	gafe_vdac_0.vdac_pdb = 0;
	gafe_vdac_1.vdicnt = 0x3;
	gafe_vbuf_1.buf_pdb1 = 0;
	gafe_vbuf_0.buf_pdb2 = 0;

	VPORT_I2C_Write((UINT32*)&gafe_cvbs_1);
	VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
	VPORT_I2C_Write((UINT32*)&gafe_vdac_0);
	VPORT_I2C_Write((UINT32*)&gafe_vdac_1);
	VPORT_I2C_Write((UINT32*)&gafe_vbuf_0);
	VPORT_I2C_Write((UINT32*)&gafe_vbuf_1);


	/* AFE3CH Power Down Setting */
	/* LLPLL PDB */
	VPORT_I2C_Read((UINT32*)&gmain_pll_4);
	VPORT_I2C_Read((UINT32*)&gllpll_0);
	gmain_pll_4.dr3p_pdb = 0;
	gllpll_0.reset_n = 0;
	gllpll_0.llpll_pdb = 0;
	VPORT_I2C_Write((UINT32*)&gmain_pll_4);
	VPORT_I2C_Write((UINT32*)&gllpll_0);

	/* LLPLL/CST SWRST & 3CH_DIG SWRST */
	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	gsoft_reset_0.swrst_pix = 1;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
}
#endif

void CVD_L9Ax_SW_Reset(LX_AFE_CVD_SELECT_T select_main_sub)
{
	if(select_main_sub == LX_CVD_MAIN)
	{
		// SET SW reset registers to '1'
		//		gVportRegBx->reg_swrst_exta.reg_swrst_exta = 1;
		//		gVportRegBx->reg_swrst_exta.reg_swrst_extb = 1;
		//from cvd_test.cmm
		VPORT_L9A0_RdFL(iris_013);
		/*
		VPORT_L9A0_Rd01(iris_013, cvd_soft_reset, temp);
		AFE_PRINT("top_001 :  %x \n",(UINT32)&gpRealRegVPORT_L9A0->top_001- (UINT32)gpRealRegVPORT_L9A0);
		AFE_PRINT("iris_mif_gmua_001 :  %x \n",(UINT32)&gpRealRegVPORT_L9A0->iris_mif_gmua_001- (UINT32)gpRealRegVPORT_L9A0);
		AFE_PRINT("iris_mif_gmua_008 :  %x \n",(UINT32)&gpRealRegVPORT_L9A0->iris_mif_gmua_008- (UINT32)gpRealRegVPORT_L9A0);
		AFE_PRINT("iris_de_ctrl_001 :  %x \n",(UINT32)&gpRealRegVPORT_L9A0->iris_de_ctrl_001 - (UINT32)gpRealRegVPORT_L9A0);
		AFE_PRINT("fast_blank_status_001 :  %x \n",(UINT32)&gpRealRegVPORT_L9A0->fast_blank_status_001 - (UINT32)gpRealRegVPORT_L9A0);
		AFE_PRINT("iris_001 :  %x \n",(UINT32)&gpRealRegVPORT_L9A0->iris_001- (UINT32)gpRealRegVPORT_L9A0);
		AFE_PRINT("iris_013 :  %x \n",(UINT32)&gpRealRegVPORT_L9A0->iris_013- (UINT32)gpRealRegVPORT_L9A0);
		AFE_PRINT("cvd_soft_reset status before set to 1 : [%x]\n", temp);
		AFE_PRINT("iris_013 value:[%x]\n", REG_RD(0xc0024230));
		REG_WD(0xc0024230		,1);
		*/
		VPORT_L9A0_Wr01(iris_013, cvd_soft_reset, 1);
		VPORT_L9A0_WrFL(iris_013);

		/* LLPLL/CST SWRST & 3CH_DIG SWRST */
		VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
		VPORT_I2C_Read((UINT32*)&gllpll_0);
		gsoft_reset_0.swrst_pix = 1;
		gllpll_0.reset_n = 0;
		gllpll_0.llpll_pdb = 0;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
		VPORT_I2C_Write((UINT32*)&gllpll_0);

		// dws added
		//by dws : remove mdelay
		//mdelay(10); //0809
		OS_MsecSleep(5);
		//gVportRegBx->iris_065.reg_lbadrgen_rst = 1;
		//Clear SW Reset Registers to '0'
		//		gVportRegBx->reg_swrst_exta.reg_swrst_exta = 0;
		//		gVportRegBx->reg_swrst_exta.reg_swrst_extb = 0;
		//from cvd_test.cmm
		VPORT_L9A0_RdFL(iris_013);
		VPORT_L9A0_Wr01(iris_013, cvd_soft_reset, 0);
		VPORT_L9A0_WrFL(iris_013);

		/* LLPLL/CST SWRST & 3CH_DIG SWRST */
		VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
		VPORT_I2C_Read((UINT32*)&gllpll_0);
		gsoft_reset_0.swrst_pix = 0;
		gllpll_0.reset_n = 1;
		gllpll_0.llpll_pdb = 1;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
		VPORT_I2C_Write((UINT32*)&gllpll_0);

		// dws added
		//gVportRegBx->iris_065.reg_lbadrgen_rst = 0;

	}
	else
	{
#if 0 //FIXME : chbcvd is moved to VDEC
		// SET SW reset registers to '1'
		VPORT_L9A0_RdFL(chbcvd_003);
		VPORT_L9A0_Wr01(chbcvd_003, chbreg_lbadrgen_rst, 1);
		VPORT_L9A0_WrFL(chbcvd_003);
		VPORT_L9A0_RdFL(top_007);
		VPORT_L9A0_Wr01(top_007, reg_swrst_chbcvd, 1);
		VPORT_L9A0_WrFL(top_007);
		//Clear SW Reset Registers to '0'
		VPORT_L9A0_RdFL(chbcvd_003);
		VPORT_L9A0_Wr01(chbcvd_003, chbreg_lbadrgen_rst, 0);
		VPORT_L9A0_WrFL(chbcvd_003);
		VPORT_L9A0_RdFL(top_007);
		VPORT_L9A0_Wr01(top_007, reg_swrst_chbcvd, 0);
		VPORT_L9A0_WrFL(top_007);
#endif
	}
}

// BOOLEAN PowerOnOff
// TRUE : Power Down
// FALSE : Power Up
void CVD_L9Ax_Power_Down(LX_AFE_CVD_SELECT_T select_main_sub, BOOLEAN PowerOnOFF)
{
	if(select_main_sub == LX_CVD_MAIN)
	{
		//if(PowerOnOFF==TRUE)
		if(PowerOnOFF==FALSE) // from cvd_test.cmm
		{
			// At First, dr3p_pdb should be turned off(Workaround code for denc latchup)
			// Move to I2C for support L9A Internel I2C 8MHz - 20110623 by sh.myoung -
			//VPORT_I2C_Read((UINT32*)&gmain_pll_4);
			//gmain_pll_4.dr3p_pdb = 0;
			//VPORT_I2C_Write((UINT32*)&gmain_pll_4);

			//VPORT_I2C_Read((UINT32*)&gmain_pll_4);
			//gmain_pll_4.dr3p_pdb = 1;
			//VPORT_I2C_Write((UINT32*)&gmain_pll_4);

			//	mdelay(10);

			//cvd on
			// CVD Power On (Default settings)
			VPORT_I2C_Read((UINT32*)&gafe_cvbs_1);
			VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
			gafe_cvbs_1.cvbs_pdbm = 0x1;
			gafe_cvbs_3.cvbs_pdb = 0x1;
			VPORT_I2C_Write((UINT32*)&gafe_cvbs_1);
			VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);

			VPORT_I2C_Read((UINT32*)&gafe_vbuf_0);
			VPORT_I2C_Read((UINT32*)&gafe_vbuf_1);

			/* VDAC & BUFFER Power Down Setting */
			gafe_vbuf_1.buf_pdb1 = 1;
			// buf2 is not used.
			gafe_vbuf_0.buf_pdb2 = 0;

			VPORT_I2C_Write((UINT32*)&gafe_vbuf_0);
			VPORT_I2C_Write((UINT32*)&gafe_vbuf_1);
		}
		else
		{
			//CVD off

			VPORT_I2C_Read((UINT32*)&gmain_pll_4);
			VPORT_I2C_Read((UINT32*)&gafe_cvbs_1);
			VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
			gafe_cvbs_1.cvbs_pdbm = 0;
			gafe_cvbs_3.cvbs_pdb = 0;
			gmain_pll_4.dr3p_pdb = 0;
			VPORT_I2C_Write((UINT32*)&gmain_pll_4);
			VPORT_I2C_Write((UINT32*)&gafe_cvbs_1);
			VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);

			VPORT_I2C_Read((UINT32*)&gafe_vbuf_0);
			VPORT_I2C_Read((UINT32*)&gafe_vbuf_1);

			/* VDAC & BUFFER Power Down Setting */
			gafe_vbuf_1.buf_pdb1 = 0;
			gafe_vbuf_0.buf_pdb2 = 0;

			VPORT_I2C_Write((UINT32*)&gafe_vbuf_0);
			VPORT_I2C_Write((UINT32*)&gafe_vbuf_1);
		}
	}
	else
	{
		//if(PowerOnOFF==TRUE)
		if(PowerOnOFF==FALSE)// from cvd_test.cmm
		{
			//L8B0 Code
			/*
			VPORT_L9A0_RdFL(cvbsafe_007);
			VPORT_L9A0_Wr08(cvbsafe_007, cb_lpf, 0x1, cb_pdb, 0x1, cb_icon, 0x2, cb_cp, 0x1, cb_selref, 0x2, cb_dcsel, 0x2, cb_cnt, 0x1, cb_cns, 0x0);
			VPORT_L9A0_WrFL(cvbsafe_007);
			*/
			// L9에 cb_cnt, cb_cns 찾을 수 없음 ???
			VPORT_I2C_Read((UINT32*)&gafe_cb_0);
			VPORT_I2C_Read((UINT32*)&gafe_cb_1);
			gafe_cb_1.cb_lpf = 1;
			gafe_cb_0.cb_pdb = 1;
			gafe_cb_1.cb_icon = 2;
			gafe_cb_0.cb_cp = 1;
			gafe_cb_1.cb_selref = 2;
			gafe_cb_1.cb_dcsel = 2;
			VPORT_I2C_Write((UINT32*)&gafe_cb_0);
			VPORT_I2C_Write((UINT32*)&gafe_cb_1);
		}
		else
		{
			//L8B0 Code
			/*
			VPORT_L9A0_RdFL(cvbsafe_007);
			VPORT_L9A0_Wr01(cvbsafe_007, cb_pdb, 0);
			VPORT_L9A0_WrFL(cvbsafe_007);
			*/
			VPORT_I2C_Read((UINT32*)&gafe_cb_0);
			gafe_cb_0.cb_pdb = 0;
			VPORT_I2C_Write((UINT32*)&gafe_cb_0);
		}
	}
}

void CVD_L9Ax_Reg_Init(LX_AFE_CVD_SELECT_T select_main_sub)
{
	VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
	VPORT_I2C_Read((UINT32*)&gdata_bridge_0);
	VPORT_I2C_Read((UINT32*)&gclock_control_2);
	gsoft_reset_0.swrst_f24m = 0;
	gdata_bridge_0.reg_cvbs_clock_rate = 1;
	gclock_control_2.sel_inv_f54m_clock = 1;
	VPORT_I2C_Write((UINT32*)&gsoft_reset_0);
	VPORT_I2C_Write((UINT32*)&gdata_bridge_0);
	VPORT_I2C_Write((UINT32*)&gclock_control_2);

	// CTop CVD Clock Settings
	CTOP_CTRL_L9_RdFL(ctr27);
	CTOP_CTRL_L9_Wr01(ctr27, swrst_cvd54, 0x0);
	CTOP_CTRL_L9_Wr01(ctr27, swrst_cvd27, 0x0);
	CTOP_CTRL_L9_Wr01(ctr27, swrst_vbi, 0x0);
	CTOP_CTRL_L9_Wr01(ctr27, swrst_scart, 0x0);
	CTOP_CTRL_L9_WrFL(ctr27);

	if(select_main_sub == LX_CVD_MAIN)
	{
		// 3D Comb memory mapping
		VPORT_L9A0_RdFL(iris_mif_gmua_007);
		VPORT_L9A0_Wr01(iris_mif_gmua_007, gmau_cmd_base, gpCvdMemCfg->memory_base);
		VPORT_L9A0_WrFL(iris_mif_gmua_007);

		VPORT_L9A0_RdFL(iris_021);
		VPORT_L9A0_Wr01(iris_021, blue_mode, 0x0);
		VPORT_L9A0_WrFL(iris_021);
		// 0909 by kim.min(same setting with FPGA)

		VPORT_I2C_Read((UINT32*)&gafe_cvbs_1);
		VPORT_I2C_Read((UINT32*)&gafe_cvbs_2);

		gafe_cvbs_1.cvbs_lpf = 1;
		gafe_cvbs_2.cvbs_bw = 0;
		gafe_cvbs_2.cvbs_byp = 1;

		VPORT_I2C_Write((UINT32*)&gafe_cvbs_1);
		VPORT_I2C_Write((UINT32*)&gafe_cvbs_2);

		// 1210 Invert Odd/Even Field on B0 Revision(H/W Change or DE Firmware Change?)
		// But This has problem on CC data slicing.
		//gVportRegBx->iris_044.reg_flip_field = 1;
		// Initial Register Setting For Scart Fast Blanking.
		VPORT_L9A0_RdFL(fastblank_001);
		// Blend Ratio 0x08 is for test only
		//VPORT_L9A0_Wr04(fastblank_001, fb_2line_delay, 0x1, fb_blend_ratio, 0x08, fb3ch_delay, 0xC, fb_latency, 0x16);
		VPORT_L9A0_Wr04(fastblank_001, fb_2line_delay, 0x1, fb_blend_ratio, 0x10, fb3ch_delay, 0xC, fb_latency, 0x16);
		VPORT_L9A0_WrFL(fastblank_001);
		//For Test, Mix CVBS & RGB signal
		//gVportRegBx->fastblank_001.reg_fb_blend_ratio = 0x10;
		// For Faster 3D Comb lock(kim.min 20110110)
		// But, This produced 3D-Comb Lock-Unlocking problem on NTSC RF Signal
		// Setting reg_hnon_std_threshold to 0x0c(0x06) solve lock-unlock problem(20110221)
		VPORT_L9A0_RdFL(iris_025);
		VPORT_L9A0_Wr01(iris_025, nstd_hysis, 0x3);
		VPORT_L9A0_WrFL(iris_025);
		VPORT_L9A0_RdFL(iris_065);
		VPORT_L9A0_Wr01(iris_065, field_latency, 0x0);
		VPORT_L9A0_WrFL(iris_065);

#if 0
		// 110506 by kim.min
		//gogosing dcrestore_accum_width value change 0x1b --> 0x25 for stable clamp 20110815
		VPORT_L9A0_RdFL(iris_075);
		VPORT_L9A0_Wr02(iris_075, syncmid_filter_en, 0x0, dcrestore_accum_width, 0x25);//gogosing
		VPORT_L9A0_WrFL(iris_075);
#else
		// 110823 by kim.min
		VPORT_L9A0_RdFL(iris_075);
		VPORT_L9A0_Wr02(iris_075, syncmid_filter_en, 0x1, dcrestore_accum_width, 0x1b);
		VPORT_L9A0_WrFL(iris_075);
#endif


		// 110929 wonsik.do : restore to default value(agc_peak), for AWC test
		// Fast AGC operation : 110729
#ifdef L9_SLOW_AGC_WORKAROUND
		VPORT_L9A0_RdFL(iris_026);
		VPORT_L9A0_Wr01(iris_026, agc_peak_nominal, 0x7F);	//Default : 0x0A
		VPORT_L9A0_Wr01(iris_026, agc_peak_cntl, 0x0);		//Default : 0x1
		VPORT_L9A0_WrFL(iris_026);
#endif

		// At first, VDAC power should be turned off to hide transient artifact : 110803
		CVD_L9Ax_OnOff_VDAC(FALSE);

#if 0
		// by gogosing
		// Use fixed syncmid point, to enhance sync stability on weak RF signal.
		VPORT_L9A0_RdFL(iris_056);
		VPORT_L9A0_Wr01(iris_056, cpump_fixed_syncmid, 0x1);
		// always reset accumulators when no-signal// gogosing 원복 as default 0x01
		//VPORT_L9A0_Wr01(iris_056, cpump_accum_mode, 0x0);
		VPORT_L9A0_WrFL(iris_056);
#else
		// by wonsik.do
		VPORT_L9A0_RdFL(iris_056);
		VPORT_L9A0_Wr01(iris_056, cpump_fixed_syncmid, 0x0);
		VPORT_L9A0_Wr01(iris_056, cpump_accum_mode, 0x0);
		VPORT_L9A0_WrFL(iris_056);
#endif

		// #### IRIS_016 Register Settings From Program_Color_System_Main_Multi ####
		VPORT_L9A0_RdFL(iris_016);
		// For faster agc speed at channel change, set agc_half_en value to '0'
		VPORT_L9A0_Wr01(iris_016, agc_half_en, 0);
		// For faster agc speed at channel change, set hagc_field_mode value to '1'
		//VPORT_L9A0_Wr01(iris_016, hagc_field_mode, 1);
		VPORT_L9A0_WrFL(iris_016);

#ifdef L9_USE_SYNCTIP_ONLY_MODE
		//gogosing reg_hmgc 0x40 --> 0x60 with sync tip mode clamp
		VPORT_L9A0_RdFL(iris_077);
		VPORT_L9A0_Wr01(iris_077, reg_hmgc, 0x60);
		VPORT_L9A0_WrFL(iris_077);

		VPORT_L9A0_RdFL(iris_016);
		// dc_clamp_mode : 0(auto), 1(backporch), 2(synctip), 3(off)
		VPORT_L9A0_Wr01(iris_016, dc_clamp_mode, 0x2);
		VPORT_L9A0_WrFL(iris_016);
#else	// dc clamp auto mode
		VPORT_L9A0_RdFL(iris_077);
		VPORT_L9A0_Wr01(iris_077, reg_hmgc, 0x60);
		VPORT_L9A0_WrFL(iris_077);

		VPORT_L9A0_RdFL(iris_016);
		// dc_clamp_mode : 0(auto), 1(backporch), 2(synctip), 3(off)
		VPORT_L9A0_Wr01(iris_016, dc_clamp_mode, 0x0);
		VPORT_L9A0_WrFL(iris_016);
#endif

		//110823 : New setting values by kim.min
		VPORT_L9A0_RdFL(iris_055);
		VPORT_L9A0_Wr01(iris_055, cpump_auto_stip_noisy, 0x1);
		VPORT_L9A0_Wr01(iris_055, cpump_auto_stip_no_signal, 0x1);
		VPORT_L9A0_Wr01(iris_055, cpump_auto_stip_unlocked, 0x1);
		VPORT_L9A0_WrFL(iris_055);
	}
	else
	{
#if 0 //FIXME : chbcvd is moved to VDEC
		VPORT_L9A0_RdFL(chbcvd_007);
		VPORT_L9A0_Wr01(chbcvd_007, chbreg_blue_mode, 0x0);
		VPORT_L9A0_WrFL(chbcvd_007);
#endif
		CHBCVD_L9_RdFL(chbcvd_007);
		CHBCVD_L9_Wr01(chbcvd_007, chbreg_blue_mode, 0x0);
		CHBCVD_L9_WrFL(chbcvd_007);
	}

}

int CVD_L9Ax_Set_Source_Type(UINT32 arg)
{
	LX_AFE_CVD_SET_INPUT_T	cvd_input_info;
//AFE_PRINT("Entered CVD_Set_Source_Type\n");
	if (copy_from_user(&cvd_input_info, (void __user *)arg, sizeof(LX_AFE_CVD_SET_INPUT_T)))
	{
		AFE_PRINT("Error return at CVD_Set_Source_Type\n");
		return -1;
	}
	if(cvd_input_info.cvd_main_sub == LX_CVD_MAIN)
	{
//		AFE_PRINT("Main CVD input\n");
		if(cvd_input_info.cvd_input_source_type == LX_CVD_INPUT_SOURCE_CVBS) // for composite
		{

			/*
			//TODO:dws (below CTOP register settings should be move to source control function.
			CTOP_CTRL_L9_RdFL(ctr26);
			//exta select to ch3afe clock
			CTOP_CTRL_L9_Wr01(ctr26, exta_sel, 0x0);
			// Clock divider selection for channel input A into DE to 1/2
			CTOP_CTRL_L9_Wr01(ctr26, msclk_sel, 0x1);
			CTOP_CTRL_L9_WrFL(ctr26);

			// lvds clock select to 1/2
			CTOP_CTRL_L9_RdFL(ctr27);
			CTOP_CTRL_L9_Wr01(ctr27, lvds_clk_sel, 0x1);
			CTOP_CTRL_L9_Wr01(ctr27, swrst_cvd54, 0x0);
			CTOP_CTRL_L9_Wr01(ctr27, swrst_cvd27, 0x0);
			CTOP_CTRL_L9_Wr01(ctr27, swrst_vbi, 0x0);
			CTOP_CTRL_L9_Wr01(ctr27, swrst_scart, 0x0);
			CTOP_CTRL_L9_WrFL(ctr27);
			*/

//			AFE_PRINT("Composite input\n");
			VPORT_L9A0_RdFL(iris_014);
			VPORT_L9A0_Wr01(iris_014, yc_src, 0);
			VPORT_L9A0_WrFL(iris_014);
			CVD_DEBUG("Input source = [%d]\n", cvd_input_info.cvbs_input_port);
			//gVportRegBx->top_002.reg_exta_sel = 0;	// from cvd_test.cmm
			//gVportRegBx->top_002.reg_extb_sel = 0;	// from cvd_test.cmm
			switch(cvd_input_info.cvbs_input_port)
			{
				case LX_AFE_CVBS_IN1:
					VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
					gafe_cvbs_3.cvbs_insel = 0x2;
					VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
					break;
				case LX_AFE_CVBS_IN2:
					VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
					gafe_cvbs_3.cvbs_insel = 0x3;
					VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
					break;
				case LX_AFE_CVBS_IN3:
					VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
					gafe_cvbs_3.cvbs_insel = 0x0;
					VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
					break;
				case LX_AFE_CVBS_IN4:
					VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
					gafe_cvbs_3.cvbs_insel = 0x1;
					VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
					break;
				case LX_AFE_CVBS_IN5:
					VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
					gafe_cvbs_3.cvbs_insel = 0x4;
					VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
					break;
				case LX_AFE_CVBS_IN6:
					VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
					gafe_cvbs_3.cvbs_insel = 0x5;
					VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
					break;
				default:
					break;
			}
		}
		else // for S-Video
		{
			AFE_PRINT("S-Video is not supported.\n");
		}
		//Current_State = CVD_STATE_VideoNotReady;
		//Next_State = CVD_STATE_VideoNotReady;
	}
	else // For CHB input
	{
		/* No source select fuction on L8 */
		/* On L8, CHB source is fixed on CVBS_CB */
	}
	return 0;
}

int CVD_L9Ax_Set_Scart_Overlay(BOOLEAN arg)
{
	if(arg == TRUE)
	{
		VPORT_L9A0_RdFL(fastblank_001);
		VPORT_L9A0_Wr01(fastblank_001, fb_en, 0x1);
		VPORT_L9A0_WrFL(fastblank_001);
		// Following Register settings were move to CVD_L9Ax_Reg_Init
		/*
		   gVportRegBx->fastblank_001.fb_2line_delay = 0x1;
		   gVportRegBx->fastblank_001.fb_blend_ratio = 0x8;
		   gVportRegBx->fastblank_001.fb3ch_delay = 0xC;
		   gVportRegBx->fastblank_001.fb_latency = 0x16;
		 */
	}
	else
	{
		VPORT_L9A0_RdFL(fastblank_001);
		VPORT_L9A0_Wr01(fastblank_001, fb_en, 0x0);
		VPORT_L9A0_WrFL(fastblank_001);
		//		gVportRegBx->fastblank_001.reg_fb_blend_ratio = 0x8;
	}
	return 0;
}

int CVD_L9Ax_Set_Scart_FB_En(void)
{
	int ret;
	VPORT_L9A0_RdFL(fastblank_001);
	VPORT_L9A0_Rd01(fastblank_001, fb_en, ret);
	return ret;
}

UINT8 CVD_L9Ax_Get_FC_Flag(LX_AFE_CVD_SELECT_T select_main_sub)
{
	UINT8 cordic_freq_status;

	if(select_main_sub == LX_CVD_MAIN)
	{
		VPORT_L9A0_RdFL(iris_008);
		VPORT_L9A0_Rd01(iris_008, status_cordic_freq, cordic_freq_status);
		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
	}
	else
	{
		/* //L8 code
		VPORT_L9A0_RdFL(chbcvd_045);
		VPORT_L9Ax_Rd01(chbcvd_045, chbreg_status_cordic_freq, cordic_freq_status);
		*/
		CHBCVD_L9_RdFL(chbcvd_045);
		CHBCVD_L9_Rd01(chbcvd_045, chbreg_status_cordic_freq, cordic_freq_status);

		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
		cordic_freq_status = 0x80;
	}
	if(cordic_freq_status > FC_MORE_THRESHOLD)
		return CVD_FC_MORE_FLAG;
	else if(cordic_freq_status < FC_LESS_THRESHOLD)
		return CVD_FC_LESS_FLAG;
	else
		return CVD_FC_SAME_FLAG;
}

#if 1
//gogosing burst mag status check for color burst level test (color 틀어짐 대응) 2011.06.11
UINT8 CVD_L9Ax_Get_CVD_Burst_Mag_Flag(CVD_STATE_T	color_system)
{
	UINT16 burst_mag_status;
	VPORT_L9A0_RdFL(iris_004);
	VPORT_L9A0_Rd01(iris_004,status_burst_mag,burst_mag_status);

	if(burst_mag_status > 0x620)
		return CVD_BURST_MAG_STATE_BIG; // big : 2
	else if(burst_mag_status < 0x420)
		return CVD_BURST_MAG_STATE_SMALL; // small : 1
	else
		return CVD_BURST_MAG_STATE_SAME; // same : 0
}

int CVD_L9Ax_Set_CVD_CAGC(UINT8 state,CVD_SET_SYSTEM_MAIN_T *pSet_system_t, CVD_STATE_T	color_system)
{
	if(state==0)
 		return 0;//not changed

	if(state>1)//big
	{
		VPORT_L9A0_RdFL(iris_023);
		VPORT_L9A0_Wr01(iris_023, reg_cagc, pSet_system_t->reg_cagc);
		VPORT_L9A0_WrFL(iris_023);
	}
	else//small
	{
		VPORT_L9A0_RdFL(iris_023);
		VPORT_L9A0_Wr01(iris_023, reg_cagc,0x2c); //0x45); color 수평 noise 대응 0x45 --> 0x2c
		VPORT_L9A0_WrFL(iris_023);
	}

	return 0;
}
#endif

UINT8 CVD_L9Ax_Get_Cordic_Freq(LX_AFE_CVD_SELECT_T select_main_sub)
{
	UINT8 cordic_freq_status;
	if(select_main_sub == LX_CVD_MAIN)
	{
		VPORT_L9A0_RdFL(iris_008);
		VPORT_L9A0_Rd01(iris_008, status_cordic_freq, cordic_freq_status);
		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
	}
	else
	{
		/* //L8 Code
		VPORT_L9A0_RdFL(chbcvd_045);
		VPORT_L9Ax_Rd01(chbcvd_045, chbreg_status_cordic_freq, cordic_freq_status);
		*/
		CHBCVD_L9_RdFL(chbcvd_045);
		CHBCVD_L9_Rd01(chbcvd_045, chbreg_status_cordic_freq, cordic_freq_status);

		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
		cordic_freq_status = 0x80;
	}
	return cordic_freq_status;
}

UINT8 CVD_L9Ax_Get_Cordic_Freq_CS0(void)
{
	UINT8 cordic_freq_status;
	VPORT_L9A0_RdFL(iris_304);
	VPORT_L9A0_Rd01(iris_304, cs_status_cordic_freq, cordic_freq_status);
	cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
	return cordic_freq_status;
}

UINT8 CVD_L9Ax_Get_FC_Flag_CS0(void)
{
	UINT8 cordic_freq_status;

	VPORT_L9A0_RdFL(iris_304);
	VPORT_L9A0_Rd01(iris_304, cs_status_cordic_freq, cordic_freq_status);

	cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);

	if(cordic_freq_status > FC_MORE_THRESHOLD)
		return CVD_FC_MORE_FLAG;
	else if(cordic_freq_status < FC_LESS_THRESHOLD)
		return CVD_FC_LESS_FLAG;
	else
		return CVD_FC_SAME_FLAG;
}

UINT8 CVD_L9Ax_Get_Cordic_Freq_CS1(void)
{
	UINT8 cordic_freq_status;
	VPORT_L9A0_RdFL(iris_315);
	VPORT_L9A0_Rd01(iris_315, cs1_status_cordic_freq, cordic_freq_status);
	cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
	return cordic_freq_status;
}

UINT8 CVD_L9Ax_Get_FC_Flag_CS1(void)
{
	UINT8 cordic_freq_status;

	VPORT_L9A0_RdFL(iris_315);
	VPORT_L9A0_Rd01(iris_315, cs1_status_cordic_freq, cordic_freq_status);

	cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);

	if(cordic_freq_status > FC_MORE_THRESHOLD)
		return CVD_FC_MORE_FLAG;
	else if(cordic_freq_status < FC_LESS_THRESHOLD)
		return CVD_FC_LESS_FLAG;
	else
		return CVD_FC_SAME_FLAG;
}

int CVD_L9Ax_Get_FB_Status(UINT32 arg)
{
	LX_AFE_SCART_MODE_T		scart_fb_mode;
	UINT8	scart_fb_state;
	VPORT_L9A0_RdFL(fast_blank_status_001);
	VPORT_L9A0_Rd01(fast_blank_status_001, fb_state, scart_fb_state);
	if(scart_fb_state > 0)
		scart_fb_mode = LX_SCART_MODE_RGB;
	else
		scart_fb_mode = LX_SCART_MODE_CVBS;
	if (copy_to_user((UINT32*)arg, (UINT32*)&scart_fb_mode, sizeof(LX_AFE_SCART_MODE_T)))
	{
		return -EFAULT;
	}
	return 0;
}

int CVD_L9Ax_Get_Scart_AR(UINT32 arg)
{
	LX_AFE_SCART_AR_INFO_T	scart_ar_param;
	UINT8	sc1_sid1, sc1_sid2;
	if (copy_from_user(&scart_ar_param, (void __user *)arg, sizeof(LX_AFE_SCART_AR_INFO_T)))
	{
		return -EFAULT;
	}
	if(scart_ar_param.Scart_Id == LX_SCART_ID_1)
	{
		VPORT_I2C_Read((UINT32*)&gafe_3ch_6);
		sc1_sid1 = gafe_3ch_6.afe3ch_sc1_sid1;
		sc1_sid2 = gafe_3ch_6.afe3ch_sc1_sid2;

		if((sc1_sid2==0)&&(sc1_sid1==0))
			scart_ar_param.Scart_AR = LX_SCART_AR_INVALID;
		else if((sc1_sid2==1)&&(sc1_sid1==1))
			scart_ar_param.Scart_AR = LX_SCART_AR_4_3;
		else
			scart_ar_param.Scart_AR = LX_SCART_AR_16_9;
	}

	if (copy_to_user((UINT32*)arg, (UINT32*)&scart_ar_param, sizeof(LX_AFE_SCART_AR_INFO_T)))
	{
		return -EFAULT;
	}
	return 0;
}

void CVD_L9Ax_Print_Vport_Version(void)
{
	UINT32 vport_version = 0;
	VPORT_L9A0_RdFL(top_001);
	VPORT_L9A0_Rd01(top_001, iris_ver, vport_version);
	AFE_PRINT("Vport Version : [%x]\n", vport_version);
}

int CVD_L9Ax_Vport_Output(UINT32 arg)
{
	// No more use

	return 0;
}

int CVD_L9Ax_Vport_Reg_Read(UINT32 arg)
{
	UINT32 value;
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;
	LX_AFE_REG_RW_T reg_addr_data_t;
	if (copy_from_user(&reg_addr_data_t, (void __user *)arg, sizeof(LX_AFE_REG_RW_T)))
	{
		return -EFAULT;
	}
	do {
		recvAddr = reg_addr_data_t.addr;
		/*
		   if		(recvAddr <= 0x2000) recvAddr += DTVSOC_DE_BASE;
		   else if (recvAddr <= 0xffff) recvAddr += DTVSOC_VIRT_PL301_BASE;
		 */
		recvAddr += (UINT32)gpRealRegVPORT_L9A0;
		currAddr = recvAddr;
		currAddr >>= 2;
		currAddr <<= 2;
		value = REG_RD(currAddr);
		if (currAddr == recvAddr) break;
		nextValue = REG_RD(currAddr+4);
		dBit = (recvAddr - currAddr)<<3;
		wBit = (32 - dBit);
		value  = GET_BITS(value, dBit, wBit);
		value += GET_PVAL(nextValue, wBit, dBit);
	} while (0);
	reg_addr_data_t.data = value;
	if (copy_to_user((UINT32*)arg, (UINT32*)&reg_addr_data_t, sizeof(LX_AFE_REG_RW_T)))
	{
		return -EFAULT;
	}
	return 0;
}

int CVD_L9Ax_Vport_Reg_Write(UINT32 arg)
{
	UINT32 value;
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 currValue;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;
	LX_AFE_REG_RW_T reg_addr_data_t;
	if (copy_from_user(&reg_addr_data_t, (void __user *)arg, sizeof(LX_AFE_REG_RW_T)))
	{
		return -EFAULT;
	}
	do {
		recvAddr = reg_addr_data_t.addr;
		value = reg_addr_data_t.data;
		/*
		   if		(recvAddr <= 0x2000) recvAddr += DTVSOC_DE_BASE;
		   else if (recvAddr <= 0xffff) recvAddr += DTVSOC_VIRT_PL301_BASE;
		 */
		recvAddr += (UINT32)gpRealRegVPORT_L9A0;
		currAddr = recvAddr;
		currAddr >>= 2;
		currAddr <<= 2;
		if (currAddr == recvAddr) {
			REG_WD(recvAddr, value);
			break;
		}
		currValue = REG_RD(currAddr);
		nextValue = REG_RD((currAddr+4));
		dBit = (recvAddr - currAddr)<<3;
		wBit = (32 - dBit);
		currValue  = GET_BITS(currValue ,0	  ,dBit);
		currValue += GET_PVAL(value		,dBit ,wBit);
		nextValue  = GET_PVAL(nextValue ,dBit ,wBit);
		nextValue += GET_BITS(value		,0	  ,dBit);
		REG_WD(currAddr		,currValue);
		REG_WD((currAddr+4) ,nextValue);
	} while (0);
	return 0;
}

int CVD_L9Ax_Get_No_Signal_Flag(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_001);
	VPORT_L9A0_Rd01(iris_001, no_signal, ret);
	return ret;
}

int CVD_L9Ax_Get_HLock_Flag(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_001);
	VPORT_L9A0_Rd01(iris_001, hlock, ret);
	return ret;
}

int CVD_L9Ax_Get_VLock_Flag(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_001);
	VPORT_L9A0_Rd01(iris_001, vlock, ret);
	return ret;
}

int CVD_L9Ax_Get_Vline_625_Flag(void)
{
	//return 1;
	int ret;
	VPORT_L9A0_RdFL(iris_002);
	VPORT_L9A0_Rd01(iris_002, vline_625_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_PAL_Flag(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_002);
	VPORT_L9A0_Rd01(iris_002, pal_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_SECAM_Flag(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_002);
	VPORT_L9A0_Rd01(iris_002, secam_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_Chromalock_Flag(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_001);
	VPORT_L9A0_Rd01(iris_001, chromalock, ret);
	return ret;
}

int CVD_L9Ax_Get_PAL_Flag_CS0(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_305);
	VPORT_L9A0_Rd01(iris_305, cs_pal_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_SECAM_Flag_CS0(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_304);
	VPORT_L9A0_Rd01(iris_304, cs_secam_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_Chromalock_Flag_CS0(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_305);
	VPORT_L9A0_Rd01(iris_305, cs_chromalock, ret);
	return ret;
}

int CVD_L9Ax_Get_PAL_Flag_CS1(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_316);
	VPORT_L9A0_Rd01(iris_316, cs1_pal_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_SECAM_Flag_CS1(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_315);
	VPORT_L9A0_Rd01(iris_315, cs1_secam_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_Chromalock_Flag_CS1(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_316);
	VPORT_L9A0_Rd01(iris_316, cs1_chromalock, ret);
	return ret;
}

int CVD_L9Ax_Get_Noise_Status(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_009);
	VPORT_L9A0_Rd01(iris_009, status_noise, ret);
	return ret;
}

int CVD_L9Ax_Get_NoBurst_Flag(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_002);
	VPORT_L9A0_Rd01(iris_002, noburst_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_CHB_No_Signal_Flag(void)
{
	int ret;
	/* //L8 Code
	VPORT_L9A0_RdFL(chbcvd_039);
	VPORT_L9A0_Rd01(chbcvd_039, chbreg_no_signal, ret);
	*/
	CHBCVD_L9_RdFL(chbcvd_039);
	CHBCVD_L9_Rd01(chbcvd_039, chbreg_no_signal, ret);
	return ret;
}

int CVD_L9Ax_Get_CHB_HLock_Flag(void)
{
	int ret;
	/* L8 Code
	VPORT_L9A0_RdFL(chbcvd_039);
	VPORT_L9A0_Rd01(chbcvd_039, chbreg_hlock, ret);
	*/
	CHBCVD_L9_RdFL(chbcvd_039);
	CHBCVD_L9_Rd01(chbcvd_039, chbreg_hlock, ret);
	return ret;
}

int CVD_L9Ax_Get_CHB_VLock_Flag(void)
{
	int ret;
	//L8 Code
	//VPORT_L9A0_RdFL(chbcvd_039);
	//VPORT_L9A0_Rd01(chbcvd_039, chbreg_vlock, ret);
	CHBCVD_L9_RdFL(chbcvd_039);
	CHBCVD_L9_Rd01(chbcvd_039, chbreg_vlock, ret);
	return ret;
}

int CVD_L9Ax_Get_CHB_Chromalock_Flag(void)
{
	int ret;
	//L8 Code
	//VPORT_L9A0_RdFL(chbcvd_039);
	//VPORT_L9A0_Rd01(chbcvd_039, chbreg_chromalock, ret);
	CHBCVD_L9_RdFL(chbcvd_039);
	CHBCVD_L9_Rd01(chbcvd_039, chbreg_chromalock, ret);
	return ret;
}

int CVD_L9Ax_Get_CHB_PAL_Flag(void)
{
	int ret;
	//L8 Code
	//VPORT_L9A0_RdFL(chbcvd_040);
	//VPORT_L9A0_Rd01(chbcvd_040, chbreg_pal_detected, ret);
	CHBCVD_L9_RdFL(chbcvd_040);
	CHBCVD_L9_Rd01(chbcvd_040, chbreg_pal_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_CHB_Vline_625_Flag(void)
{
	int ret;
	//L8 Code
	//VPORT_L9A0_RdFL(chbcvd_040);
	//VPORT_L9A0_Rd01(chbcvd_040, chbreg_vline_625_detected, ret);
	CHBCVD_L9_RdFL(chbcvd_040);
	CHBCVD_L9_Rd01(chbcvd_040, chbreg_vline_625_detected, ret);
	return ret;
}

int CVD_L9Ax_Get_CHB_SECAM_Flag(void)
{
	int ret;
	//L8 Code
	//VPORT_L9A0_RdFL(chbcvd_040);
	//VPORT_L9A0_Rd01(chbcvd_040, chbreg_secam_detected, ret);
	CHBCVD_L9_RdFL(chbcvd_040);
	CHBCVD_L9_Rd01(chbcvd_040, chbreg_secam_detected, ret);
	return ret;
}

int CVD_L9Ax_Reset_irisyc(int enable)
{
	if(enable > 0)
	{
		VPORT_I2C_Read((UINT32*)&gsoft_reset_5);
		gsoft_reset_5.swrst_f54m = 1;
		gsoft_reset_5.swrst_cvbs = 1;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_5);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&gsoft_reset_5);
		gsoft_reset_5.swrst_f54m = 0;
		gsoft_reset_5.swrst_cvbs = 0;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_5);
	}

	return 0;
}

int CVD_L9Ax_Reset_hdct(int enable)
{
	if(enable >0)
	{
		VPORT_L9A0_RdFL(iris_278);
		VPORT_L9A0_Wr01(iris_278, swrst_hdct, 1);
		VPORT_L9A0_WrFL(iris_278);
	}
	else
	{
		VPORT_L9A0_RdFL(iris_278);
		VPORT_L9A0_Wr01(iris_278, swrst_hdct, 0);
		VPORT_L9A0_WrFL(iris_278);
	}
	return 0;
}

int CVD_L9Ax_Reset_cdct(int enable)
{
	if(enable >0)
	{
		VPORT_L9A0_RdFL(iris_278);
		VPORT_L9A0_Wr01(iris_278, swrst_cdct, 1);
		VPORT_L9A0_WrFL(iris_278);
	}
	else
	{
		VPORT_L9A0_RdFL(iris_278);
		VPORT_L9A0_Wr01(iris_278, swrst_cdct, 0);
		VPORT_L9A0_WrFL(iris_278);
	}
	return 0;
}

void CVD_L9Ax_Set_AAF_Params(CVD_L9Ax_SET_AAF_T *pSet_aaf_t)
{
	VPORT_L9A0_RdFL(iris_235);
	VPORT_L9A0_Wr01(iris_235, lpf_0_en, pSet_aaf_t->reg_lpf_0_en);
	VPORT_L9A0_Wr01(iris_235, lpf_tap_0_0, pSet_aaf_t->reg_lpf_tap_0_0);
	VPORT_L9A0_WrFL(iris_235);

	VPORT_L9A0_RdFL(iris_236);
	VPORT_L9A0_Wr01(iris_236, lpf_tap_0_1, pSet_aaf_t->reg_lpf_tap_0_1);
	VPORT_L9A0_Wr01(iris_236, lpf_tap_0_2, pSet_aaf_t->reg_lpf_tap_0_2);
	VPORT_L9A0_WrFL(iris_236);

	VPORT_L9A0_RdFL(iris_237);
	VPORT_L9A0_Wr01(iris_237, lpf_tap_0_3, pSet_aaf_t->reg_lpf_tap_0_3);
	VPORT_L9A0_Wr01(iris_237, lpf_tap_0_4, pSet_aaf_t->reg_lpf_tap_0_4);
	VPORT_L9A0_WrFL(iris_237);

	VPORT_L9A0_RdFL(iris_238);
	VPORT_L9A0_Wr01(iris_238, lpf_tap_0_5, pSet_aaf_t->reg_lpf_tap_0_5);
	VPORT_L9A0_Wr01(iris_238, lpf_tap_0_6, pSet_aaf_t->reg_lpf_tap_0_6);
	VPORT_L9A0_WrFL(iris_238);
}

void CVD_L9Ax_Set_ADC_Params(CVD_L9Ax_SET_CRES54MHZ_T *pSet_cresampler_t)
{
	/*
	VPORT_I2C_Read((UINT32*)&gmain_pll_0);
	VPORT_I2C_Read((UINT32*)&gmain_pll_1);
	VPORT_I2C_Read((UINT32*)&gmain_pll_2);
	VPORT_I2C_Read((UINT32*)&gmain_pll_3);
	VPORT_I2C_Read((UINT32*)&gmain_pll_4);

	gmain_pll_0.dr3p_nsc = pSet_cresampler_t->DR3P_NSC;
	gmain_pll_1.dr3p_npc = pSet_cresampler_t->DR3P_NPC;
	gmain_pll_2.dr3p_m = pSet_cresampler_t->DR3P_M;
	gmain_pll_3.dr3p_od_cvdclk = pSet_cresampler_t->DR3P_OD_CVDCLK;
	gmain_pll_3.dr3p_od_recclk = pSet_cresampler_t->DR3P_OD_RECCLK;
	gmain_pll_4.dr3p_ps23c = pSet_cresampler_t->DR3P_PS23C;
	gmain_pll_4.dr3p_cih = pSet_cresampler_t->DR3P_CIH;

	VPORT_I2C_Write((UINT32*)&gmain_pll_0);
	VPORT_I2C_Write((UINT32*)&gmain_pll_1);
	VPORT_I2C_Write((UINT32*)&gmain_pll_2);
	VPORT_I2C_Write((UINT32*)&gmain_pll_3);
	VPORT_I2C_Write((UINT32*)&gmain_pll_4);
	*/
}

void CVD_L9Ax_Set_Cresampler_Params(CVD_L9Ax_SET_CRES54MHZ_T *pSet_cresampler_t)
{
	VPORT_L9A0_RdFL(iris_147);
	VPORT_L9A0_Wr01(iris_147, cres_clksel, pSet_cresampler_t->reg_cres_clksel);
	VPORT_L9A0_Wr01(iris_147, cres_0px_sel, pSet_cresampler_t->reg_cres_0px_sel);
	VPORT_L9A0_Wr01(iris_147, cres_blend_bcdefault, pSet_cresampler_t->reg_cres_blend_bcdefault);
	VPORT_L9A0_WrFL(iris_147);

	VPORT_L9A0_RdFL(iris_139);
	VPORT_L9A0_Wr01(iris_139, iris_cresampler_on, pSet_cresampler_t->reg_iris_cresampler_on);
	VPORT_L9A0_Wr01(iris_139, cres_lpfa_on, pSet_cresampler_t->reg_cres_lpfa_on);
	VPORT_L9A0_Wr01(iris_139, cres_lpfa_coeff0, pSet_cresampler_t->reg_cres_lpfa_coeff0);
	VPORT_L9A0_WrFL(iris_139);

	VPORT_L9A0_RdFL(iris_140);
	VPORT_L9A0_Wr01(iris_140, cres_lpfa_coeff1, pSet_cresampler_t->reg_cres_lpfa_coeff1);
	VPORT_L9A0_Wr01(iris_140, cres_lpfa_coeff2, pSet_cresampler_t->reg_cres_lpfa_coeff2);
	VPORT_L9A0_WrFL(iris_140);

	VPORT_L9A0_RdFL(iris_141);
	VPORT_L9A0_Wr01(iris_141, cres_lpfa_coeff3, pSet_cresampler_t->reg_cres_lpfa_coeff3);
	VPORT_L9A0_Wr01(iris_141, cres_lpfa_coeff4, pSet_cresampler_t->reg_cres_lpfa_coeff4);
	VPORT_L9A0_WrFL(iris_141);

	VPORT_L9A0_RdFL(iris_142);
	VPORT_L9A0_Wr01(iris_142, cres_lpfa_coeff5, pSet_cresampler_t->reg_cres_lpfa_coeff5);
	VPORT_L9A0_WrFL(iris_142);

	VPORT_L9A0_RdFL(iris_143);
	VPORT_L9A0_Wr01(iris_143, cres_lpfb_on, pSet_cresampler_t->reg_cres_lpfb_on);
	VPORT_L9A0_Wr01(iris_143, cres_lpfb_coeff0, pSet_cresampler_t->reg_cres_lpfb_coeff0);
	VPORT_L9A0_WrFL(iris_143);

	VPORT_L9A0_RdFL(iris_144);
	VPORT_L9A0_Wr01(iris_144, cres_lpfb_coeff1, pSet_cresampler_t->reg_cres_lpfb_coeff1);
	VPORT_L9A0_Wr01(iris_144, cres_lpfb_coeff2, pSet_cresampler_t->reg_cres_lpfb_coeff2);
	VPORT_L9A0_WrFL(iris_144);

	VPORT_L9A0_RdFL(iris_145);
	VPORT_L9A0_Wr01(iris_145, cres_lpfb_coeff3, pSet_cresampler_t->reg_cres_lpfb_coeff3);
	VPORT_L9A0_Wr01(iris_145, cres_lpfb_coeff4, pSet_cresampler_t->reg_cres_lpfb_coeff4);
	VPORT_L9A0_WrFL(iris_145);

	VPORT_L9A0_RdFL(iris_154);
	VPORT_L9A0_Wr01(iris_154, dtrs_revalpha_sel, pSet_cresampler_t->reg_dtrs_revalpha_sel);
	VPORT_L9A0_Wr01(iris_154, dtrs_dlysel, pSet_cresampler_t->reg_dtrs_dlysel);
	VPORT_L9A0_WrFL(iris_154);
	//added 20110126
	VPORT_L9A0_RdFL(iris_146);
	VPORT_L9A0_Wr01(iris_146, cres_blend_method, pSet_cresampler_t->reg_cres_blend_method);
	VPORT_L9A0_Wr01(iris_146, cres_blend_sel, pSet_cresampler_t->reg_cres_blend_sel);
	VPORT_L9A0_WrFL(iris_146);
}

void CVD_L9Ax_Set_Dot_Crawl_Params(CVD_L9Ax_SET_DOT_CRAWL_T *pSet_dotcrawl_t)
{
	VPORT_L9A0_RdFL(iris_155);
	VPORT_L9A0_Wr01(iris_155, cvd_2dcomb_byp, pSet_dotcrawl_t->reg_2dcomb_byp);
	VPORT_L9A0_Wr01(iris_155, cvd_2dcomb_cband_sel, pSet_dotcrawl_t->reg_2dcomb_cband_sel);
	VPORT_L9A0_Wr01(iris_155, cvd_2dcomb_ccombo_sel, pSet_dotcrawl_t->reg_2dcomb_ccombo_sel);
	VPORT_L9A0_Wr01(iris_155, cvd_2dcomb_ycrdc_cvsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_cvsel);
	VPORT_L9A0_Wr01(iris_155, cvd_2dcomb_ycrdc_chsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_chsel);
	VPORT_L9A0_Wr01(iris_155, cvd_2dcomb_ycrdc_lrsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_lrsel);
	VPORT_L9A0_WrFL(iris_155);

	VPORT_L9A0_RdFL(iris_156);
	VPORT_L9A0_Wr01(iris_156, cvd_2dcomb_ycrdc_yhsymth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhsymth);
	VPORT_L9A0_Wr01(iris_156, cvd_2dcomb_ycrdc_yhdiffth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhdiffth);
	VPORT_L9A0_WrFL(iris_156);

	VPORT_L9A0_RdFL(iris_157);
	VPORT_L9A0_Wr01(iris_157, cvd_2dcomb_ycrdc_cvdiffth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_cvdiffth);
	VPORT_L9A0_WrFL(iris_157);

	VPORT_L9A0_RdFL(iris_160);
	VPORT_L9A0_Wr01(iris_160, cvd_2dcomb_ycrdc_yvsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvsel);
	VPORT_L9A0_Wr01(iris_160, cvd_2dcomb_ycrdc_yhsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhsel);
	VPORT_L9A0_Wr01(iris_160, cvd_2dcomb_ycrdc_hth_on, pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth_on);
	VPORT_L9A0_Wr01(iris_160, cvd_2dcomb_ycrdc_hth_sel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth_sel);
	VPORT_L9A0_Wr01(iris_160, cvd_2dcomb_ycrdc_hth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth);
	VPORT_L9A0_WrFL(iris_160);

	VPORT_L9A0_RdFL(iris_158);
	VPORT_L9A0_Wr01(iris_158, cvd_2dcomb_ycrdc_lrtype, pSet_dotcrawl_t->reg_2dcomb_ycrdc_lrtype);
	VPORT_L9A0_Wr01(iris_158, cvd_2dcomb_ycrdc_dsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_dsel);
	VPORT_L9A0_Wr01(iris_158, cvd_2dcomb_ycrdc_lcr_on, pSet_dotcrawl_t->reg_2dcomb_ycrdc_lcr_on);
	VPORT_L9A0_Wr01(iris_158, cvd_2dcomb_ycrdc_method, pSet_dotcrawl_t->reg_2dcomb_ycrdc_method);
	VPORT_L9A0_WrFL(iris_158);

	VPORT_L9A0_RdFL(iris_225);
	VPORT_L9A0_Wr01(iris_225, cvd_2dcomb_ycrdc_yvdiv_th0, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th0);
	VPORT_L9A0_WrFL(iris_225);

	VPORT_L9A0_RdFL(iris_226);
	VPORT_L9A0_Wr01(iris_226, cvd_2dcomb_ycrdc_yvdiv_th1, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th1);
	VPORT_L9A0_Wr01(iris_226, cvd_2dcomb_ycrdc_yvdiv_th2, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th2);
	VPORT_L9A0_WrFL(iris_226);

	VPORT_L9A0_RdFL(iris_227);
	VPORT_L9A0_Wr01(iris_227, cvd_2dcomb_ycrdc_yvdiv_th3, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th3);
	VPORT_L9A0_Wr01(iris_227, cvd_2dcomb_ycrdc_yvdiv_th4, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th4);
	VPORT_L9A0_WrFL(iris_227);

	VPORT_L9A0_RdFL(iris_228);
	VPORT_L9A0_Wr01(iris_228, cvd_2dcomb_ycrdc_yvdiv_th5, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th5);
	VPORT_L9A0_Wr01(iris_228, cvd_2dcomb_ycrdc_yvdiv_th6, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th6);
	VPORT_L9A0_WrFL(iris_228);
}

void CVD_L9Ax_Set_Cross_Color_Reducer(CVD_L9Ax_SET_CROSS_COLOR_REDUCER_T *pSet_crosscolor_t)
{
	VPORT_L9A0_RdFL(iris_161);
	VPORT_L9A0_Wr01(iris_161, cvd_2dcomb_cyrdc_method, pSet_crosscolor_t->reg_2dcomb_cyrdc_method);
	VPORT_L9A0_Wr01(iris_161, cvd_2dcomb_cyrdc_lcr_on, pSet_crosscolor_t->reg_2dcomb_cyrdc_lcr_on);
	VPORT_L9A0_Wr01(iris_161, cvd_2dcomb_cyrdc_crvalid, pSet_crosscolor_t->reg_2dcomb_cyrdc_crvalid);
	VPORT_L9A0_Wr01(iris_161, cvd_2dcomb_cyrdc_lrtype, pSet_crosscolor_t->reg_2dcomb_cyrdc_lrtype);
	VPORT_L9A0_Wr01(iris_161, cvd_2dcomb_cyrdc_dsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_dsel);
	VPORT_L9A0_Wr01(iris_161, cvd_2dcomb_cyrdc_dsel1, pSet_crosscolor_t->reg_2dcomb_cyrdc_dsel1);
	VPORT_L9A0_WrFL(iris_161);

	VPORT_L9A0_RdFL(iris_162);
	VPORT_L9A0_Wr01(iris_162, cvd_2dcomb_cyrdc_chsymth, pSet_crosscolor_t->reg_2dcomb_cyrdc_chsymth);
	VPORT_L9A0_Wr01(iris_162, cvd_2dcomb_cyrdc_chsymth1, pSet_crosscolor_t->reg_2dcomb_cyrdc_chsymth1);
	VPORT_L9A0_WrFL(iris_162);

	VPORT_L9A0_RdFL(iris_230);
	VPORT_L9A0_Wr01(iris_230, cvd_2dcomb_cyrdc_hvth, pSet_crosscolor_t->reg_2dcomb_cyrdc_hvth);
	VPORT_L9A0_Wr01(iris_230, cvd_2dcomb_cyrdc_cvdiv_xsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_xsel);
	VPORT_L9A0_Wr01(iris_230, cvd_2dcomb_cyrdc_cvdiv_th0, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th0);
	VPORT_L9A0_WrFL(iris_230);

	VPORT_L9A0_RdFL(iris_231);
	VPORT_L9A0_Wr01(iris_231, cvd_2dcomb_cyrdc_cvdiv_th1, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th1);
	VPORT_L9A0_Wr01(iris_231, cvd_2dcomb_cyrdc_cvdiv_th2, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th2);
	VPORT_L9A0_WrFL(iris_231);

	VPORT_L9A0_RdFL(iris_232);
	VPORT_L9A0_Wr01(iris_232, cvd_2dcomb_cyrdc_cvdiv_th3, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th3);
	VPORT_L9A0_Wr01(iris_232, cvd_2dcomb_cyrdc_cvdiv_th4, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th4);
	VPORT_L9A0_WrFL(iris_232);

	VPORT_L9A0_RdFL(iris_233);
	VPORT_L9A0_Wr01(iris_233, cvd_2dcomb_cyrdc_cvdiv_th5, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th5);
	VPORT_L9A0_Wr01(iris_233, cvd_2dcomb_cyrdc_cvdiv_th6, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th6);
	VPORT_L9A0_WrFL(iris_233);

	VPORT_L9A0_RdFL(iris_163);
	VPORT_L9A0_Wr01(iris_163, cvd_2dcomb_cyrdc_chdiffth, pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffth);
	VPORT_L9A0_WrFL(iris_163);

	VPORT_L9A0_RdFL(iris_164);
	VPORT_L9A0_Wr01(iris_164, cvd_2dcomb_cyrdc_chdiffon, pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffon);
	VPORT_L9A0_Wr01(iris_164, cvd_2dcomb_cyrdc_chdiffsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffsel);
	VPORT_L9A0_WrFL(iris_164);

	VPORT_L9A0_RdFL(iris_228);
	VPORT_L9A0_Wr01(iris_228, cvd_2dcomb_cyrdc_dir_on, pSet_crosscolor_t->reg_2dcomb_cyrdc_dir_on);
	VPORT_L9A0_Wr01(iris_228, cvd_2dcomb_cyrdc_v0b1_dinsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_v0b1_dinsel);
	VPORT_L9A0_WrFL(iris_228);

	VPORT_L9A0_RdFL(iris_229);
	VPORT_L9A0_Wr01(iris_229, cvd_2dcomb_cyrdc_v1b4_cinsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_cinsel);
	VPORT_L9A0_Wr01(iris_229, cvd_2dcomb_cyrdc_v1b4_dinsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_dinsel);
	VPORT_L9A0_Wr01(iris_229, cvd_2dcomb_cyrdc_v1b4_th, pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_th);
	VPORT_L9A0_WrFL(iris_229);

	VPORT_L9A0_RdFL(iris_165);
	VPORT_L9A0_Wr01(iris_165, cvd_2dcomb_cyrdc_hth_on, pSet_crosscolor_t->reg_2dcomb_cyrdc_hth_on);
	VPORT_L9A0_Wr01(iris_165, cvd_2dcomb_cyrdc_hth, pSet_crosscolor_t->reg_2dcomb_cyrdc_hth);
	VPORT_L9A0_WrFL(iris_165);
}

void CVD_L9Ax_Set_Motion_Detector(CVD_L9Ax_SET_MOTION_DETECTOR_T *pSet_motiondetector_t)
{
	VPORT_L9A0_RdFL(iris_064);
	VPORT_L9A0_Wr01(iris_064, motion_mode, pSet_motiondetector_t->reg_motion_mode           );
	VPORT_L9A0_Wr01(iris_064, dot_suppress_mode, pSet_motiondetector_t->reg_dot_suppress_mode     );
//	VPORT_L9A0_Wr01(iris_064, adaptive_chroma_mode, pSet_motiondetector_t->reg_adaptive_chroma_mode  );
	VPORT_L9A0_WrFL(iris_064);

	VPORT_L9A0_RdFL(iris_091);
	VPORT_L9A0_Wr01(iris_091, motion_config, pSet_motiondetector_t->reg_motion_config         );
	VPORT_L9A0_WrFL(iris_091);

	VPORT_L9A0_RdFL(iris_092);
	VPORT_L9A0_Wr01(iris_092, status_motion_mode, pSet_motiondetector_t->reg_status_motion_mode    );
	VPORT_L9A0_WrFL(iris_092);

	VPORT_L9A0_RdFL(iris_093);
	VPORT_L9A0_Wr01(iris_093, flat_luma_shift, pSet_motiondetector_t->reg_flat_luma_shift       );
	VPORT_L9A0_Wr01(iris_093, flat_chroma_shift, pSet_motiondetector_t->reg_flat_chroma_shift     );
	VPORT_L9A0_Wr01(iris_093, flat_luma_offset, pSet_motiondetector_t->reg_flat_luma_offset      );
	VPORT_L9A0_Wr01(iris_093, flat_chroma_offset, pSet_motiondetector_t->reg_flat_chroma_offset    );
	VPORT_L9A0_WrFL(iris_093);

	VPORT_L9A0_RdFL(iris_171);
	VPORT_L9A0_Wr01(iris_171, frame_motion_th0, pSet_motiondetector_t->reg_frame_motion_th0      );
	VPORT_L9A0_Wr01(iris_171, frame_motion_th1, pSet_motiondetector_t->reg_frame_motion_th1      );
	VPORT_L9A0_WrFL(iris_171);

	VPORT_L9A0_RdFL(iris_088);
	VPORT_L9A0_Wr01(iris_088, chroma_level, pSet_motiondetector_t->reg_chroma_level          );
	VPORT_L9A0_Wr01(iris_088, tcomb_chroma_level, pSet_motiondetector_t->reg_tcomb_chroma_level    );
	VPORT_L9A0_WrFL(iris_088);

	VPORT_L9A0_RdFL(iris_189);
	VPORT_L9A0_Wr01(iris_189, cvd_3dcomb_mdy_dir, pSet_motiondetector_t->reg_3dcomb_mdy_dir              );
	VPORT_L9A0_Wr01(iris_189, cvd_3dcomb_mdy_disel, pSet_motiondetector_t->reg_3dcomb_mdy_disel            );
	VPORT_L9A0_WrFL(iris_189);

	VPORT_L9A0_RdFL(iris_192);
	VPORT_L9A0_Wr01(iris_192, cvd_3dcomb_mdi_sadsel, pSet_motiondetector_t->reg_3dcomb_mdi_sadsel);
	VPORT_L9A0_WrFL(iris_192);
}

void CVD_L9Ax_Set_Global_Classifier(   CVD_L9Ax_SET_GLOBAL_CLASSIFIER_T *pSet_globalclassifier)
{
	VPORT_L9A0_RdFL(iris_189);
	VPORT_L9A0_Wr01(iris_189, cvd_3dcomb_md_mode, pSet_globalclassifier->reg_3dcomb_md_mode              );
	VPORT_L9A0_Wr01(iris_189, cvd_3dcomb_mdy_disel1, pSet_globalclassifier->reg_3dcomb_mdy_disel1     );
	VPORT_L9A0_Wr01(iris_189, cvd_3dcomb_mdy_sel, pSet_globalclassifier->reg_3dcomb_mdy_sel              );
	VPORT_L9A0_WrFL(iris_189);

	VPORT_L9A0_RdFL(iris_197);
	VPORT_L9A0_Wr01(iris_197, cvd_3dcomb_mdlf_actv_sel, pSet_globalclassifier->reg_3dcomb_mdlf_actv_sel        );
	VPORT_L9A0_Wr01(iris_197, cvd_3dcomb_mdlf_actv_th, pSet_globalclassifier->reg_3dcomb_mdlf_actv_th         );
	VPORT_L9A0_Wr01(iris_197, cvd_3dcomb_mdhf_actv_th, pSet_globalclassifier->reg_3dcomb_mdhf_actv_th         );
	VPORT_L9A0_WrFL(iris_197);

	VPORT_L9A0_RdFL(iris_198);
	VPORT_L9A0_Wr01(iris_198, cvd_3dcomb_mdlf_blk_th, pSet_globalclassifier->reg_3dcomb_mdlf_blk_th          );
	VPORT_L9A0_Wr01(iris_198, cvd_3dcomb_mdhf_blk_th, pSet_globalclassifier->reg_3dcomb_mdhf_blk_th          );
	VPORT_L9A0_WrFL(iris_198);

	VPORT_L9A0_RdFL(iris_196);
	VPORT_L9A0_Wr01(iris_196, cvd_3dcomb_mdclass_csel, pSet_globalclassifier->reg_3dcomb_mdclass_csel         );
	VPORT_L9A0_Wr01(iris_196, cvd_3dcomb_mdclass_dsel, pSet_globalclassifier->reg_3dcomb_mdclass_dsel         );
	VPORT_L9A0_Wr01(iris_196, cvd_3dcomb_mdclass_fldsel, pSet_globalclassifier->reg_3dcomb_mdclass_fldsel       );
	VPORT_L9A0_WrFL(iris_196);

	VPORT_L9A0_RdFL(iris_199);
	VPORT_L9A0_Wr01(iris_199, cvd_3dcomb_mdlf_fld_th0, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th0         );
	VPORT_L9A0_Wr01(iris_199, cvd_3dcomb_mdlf_fld_th1, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th1         );
	VPORT_L9A0_WrFL(iris_199);

	VPORT_L9A0_RdFL(iris_200);
	VPORT_L9A0_Wr01(iris_200, cvd_3dcomb_mdlf_fld_th2, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th2         );
	VPORT_L9A0_Wr01(iris_200, cvd_3dcomb_mdlf_fld_th3, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th3         );
	VPORT_L9A0_Wr01(iris_200, cvd_3dcomb_mdhf_fld_sel, pSet_globalclassifier->reg_3dcomb_mdhf_fld_sel         );
	VPORT_L9A0_WrFL(iris_200);

	VPORT_L9A0_RdFL(iris_201);
	VPORT_L9A0_Wr01(iris_201, cvd_3dcomb_mdhf_fld_th0, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th0         );
	VPORT_L9A0_Wr01(iris_201, cvd_3dcomb_mdhf_fld_th1, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th1         );
	VPORT_L9A0_WrFL(iris_201);

	VPORT_L9A0_RdFL(iris_202);
	VPORT_L9A0_Wr01(iris_202, cvd_3dcomb_mdhf_fld_th2, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th2         );
	VPORT_L9A0_Wr01(iris_202, cvd_3dcomb_mdhf_fld_th3, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th3         );
	VPORT_L9A0_WrFL(iris_202);

	VPORT_L9A0_RdFL(iris_203);
	VPORT_L9A0_Wr01(iris_203, cvd_3dcomb_mdhlf_fld_sub_th0, pSet_globalclassifier->reg_3dcomb_mdhlf_fld_sub_th0    );
	VPORT_L9A0_Wr01(iris_203, cvd_3dcomb_mdlf_fld_grd, pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd         );
	VPORT_L9A0_Wr01(iris_203, cvd_3dcomb_mdhf_fld_grd, pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd         );
	VPORT_L9A0_WrFL(iris_203);

	VPORT_L9A0_RdFL(iris_204);
	VPORT_L9A0_Wr01(iris_204, cvd_3dcomb_mdlf_fld_grd_alp, pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd_alp     );
	VPORT_L9A0_WrFL(iris_204);

	VPORT_L9A0_RdFL(iris_205);
	VPORT_L9A0_Wr01(iris_205, cvd_3dcomb_mdlf_fld_grd_alp__2, pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd_alp__2);
	VPORT_L9A0_WrFL(iris_205);

	VPORT_L9A0_RdFL(iris_206);
	VPORT_L9A0_Wr01(iris_206, cvd_3dcomb_mdhf_fld_grd_alp, pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd_alp     );
	VPORT_L9A0_WrFL(iris_206);

	VPORT_L9A0_RdFL(iris_207);
	VPORT_L9A0_Wr01(iris_207, cvd_3dcomb_mdhf_fld_grd_alp__2, pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd_alp__2);
	VPORT_L9A0_WrFL(iris_207);

	VPORT_L9A0_RdFL(iris_209);
	VPORT_L9A0_Wr01(iris_209, cvd_3dcomb_mdpd32_on, pSet_globalclassifier->reg_3dcomb_mdpd32_on            );
	VPORT_L9A0_Wr01(iris_209, cvd_3dcomb_mdpd32_sel, pSet_globalclassifier->reg_3dcomb_mdpd32_sel           );
	VPORT_L9A0_WrFL(iris_209);

	VPORT_L9A0_RdFL(iris_210);
	VPORT_L9A0_Wr01(iris_210, cvd_3dcomb_mdpd32_th, pSet_globalclassifier->reg_3dcomb_mdpd32_th            );
	VPORT_L9A0_Wr01(iris_210, cvd_3dcomb_mdglo_mode, pSet_globalclassifier->reg_3dcomb_mdglo_mode     );
	VPORT_L9A0_WrFL(iris_210);

	VPORT_L9A0_RdFL(iris_211);
	VPORT_L9A0_Wr01(iris_211, cvd_3dcomb_mdglo_m1_lth0, pSet_globalclassifier->reg_3dcomb_mdglo_m1_lth0  );
	VPORT_L9A0_Wr01(iris_211, cvd_3dcomb_mdglo_m1_uth0, pSet_globalclassifier->reg_3dcomb_mdglo_m1_uth0  );
	VPORT_L9A0_Wr01(iris_211, cvd_3dcomb_mdglo_m1_sameth, pSet_globalclassifier->reg_3dcomb_mdglo_m1_sameth);
	VPORT_L9A0_WrFL(iris_211);
}

void CVD_L9Ax_Set_Blending( CVD_L9Ax_SET_BLENDING_T	*pSet_blending)
{
	VPORT_L9A0_RdFL(iris_185);
	VPORT_L9A0_Wr01(iris_185, cvd_3dcomb_coefi_pow, pSet_blending->reg_3dcomb_coefi_pow            );
	VPORT_L9A0_WrFL(iris_185);

	VPORT_L9A0_RdFL(iris_186);
	VPORT_L9A0_Wr01(iris_186, cvd_3dcomb_coefi, pSet_blending->reg_3dcomb_coefi);
	VPORT_L9A0_WrFL(iris_186);

	VPORT_L9A0_RdFL(iris_187);
	VPORT_L9A0_Wr01(iris_187, cvd_3dcomb_coefi__2, pSet_blending->reg_3dcomb_coefi__2           );
	VPORT_L9A0_WrFL(iris_187);

	VPORT_L9A0_RdFL(iris_188);
	VPORT_L9A0_Wr01(iris_188, cvd_3dcomb_coefi__3, pSet_blending->reg_3dcomb_coefi__3           );
	VPORT_L9A0_WrFL(iris_188);
}

void CVD_L9Ax_Set_Chroma_Demode(CVD_L9Ax_SET_CHROMA_DEMODE_T	*pSet_chromademode_t)
{
	VPORT_L9A0_RdFL(iris_064);
	VPORT_L9A0_Wr01(iris_064, adaptive_chroma_mode, pSet_chromademode_t->reg_adaptive_chroma_mode  );
	VPORT_L9A0_WrFL(iris_064);

	VPORT_L9A0_RdFL(iris_015);
	VPORT_L9A0_Wr01(iris_015, chroma_bw_lo, pSet_chromademode_t->reg_chroma_bw_lo);
	VPORT_L9A0_WrFL(iris_015);
}

void CVD_L9Ax_Set_Output_Delay( CVD_L9Ax_SET_OUPUT_DELAY_T        	*pSet_outputdelay_t)
{
	VPORT_L9A0_RdFL(iris_179);
	VPORT_L9A0_Wr01(iris_179, hrs_alpha_yoff, pSet_outputdelay_t->reg_hrs_alpha_yoff 	);
	VPORT_L9A0_Wr01(iris_179, hrs_alpha_cboff, pSet_outputdelay_t->reg_hrs_alpha_cboff);
	VPORT_L9A0_Wr01(iris_179, hrs_alpha_croff, pSet_outputdelay_t->reg_hrs_alpha_croff);
	VPORT_L9A0_WrFL(iris_179);
	//added 0126
	VPORT_L9A0_RdFL(iris_021);
	VPORT_L9A0_Wr01(iris_021, yc_delay, pSet_outputdelay_t->reg_yc_delay);
	VPORT_L9A0_WrFL(iris_021);

	VPORT_L9A0_RdFL(iris_234);
	VPORT_L9A0_Wr01(iris_234, cb_delay, pSet_outputdelay_t->reg_cb_delay);
	VPORT_L9A0_WrFL(iris_234);
}

void CVD_L9Ax_Set_Output_Range(CVD_L9Ax_SET_OUPUT_RANGE_T	*pSet_outputrange_t)
{
	VPORT_L9A0_RdFL(iris_166);
	VPORT_L9A0_Wr01(iris_166, oadj_y_offi, pSet_outputrange_t->reg_oadj_y_offi);
	VPORT_L9A0_Wr01(iris_166, oadj_y_offo, pSet_outputrange_t->reg_oadj_y_offo);
	VPORT_L9A0_WrFL(iris_166);

	VPORT_L9A0_RdFL(iris_167);
	VPORT_L9A0_Wr01(iris_167, oadj_y_coeff, pSet_outputrange_t->reg_oadj_y_coeff);
	VPORT_L9A0_WrFL(iris_167);

	VPORT_L9A0_RdFL(iris_168);
	VPORT_L9A0_Wr01(iris_168, oadj_c_offi, pSet_outputrange_t->reg_oadj_c_offi);
	VPORT_L9A0_Wr01(iris_168, oadj_c_offo, pSet_outputrange_t->reg_oadj_c_offo);
	VPORT_L9A0_WrFL(iris_168);

	VPORT_L9A0_RdFL(iris_169);
	VPORT_L9A0_Wr01(iris_169, oadj_c_coeff, pSet_outputrange_t->reg_oadj_c_coeff);
	VPORT_L9A0_WrFL(iris_169);
}

void CVD_L9Ax_Set_Luma_Peaking(CVD_L9Ax_SET_LUMA_PEAKING_T	*pSet_lumapeaking_t)
{
	VPORT_L9A0_RdFL(iris_046);
	VPORT_L9A0_Wr01(iris_046, peak_en, pSet_lumapeaking_t->reg_peak_en);
	VPORT_L9A0_Wr01(iris_046, peak_gain, pSet_lumapeaking_t->reg_peak_gain);
	VPORT_L9A0_Wr01(iris_046, peak_range, pSet_lumapeaking_t->reg_peak_range);
	VPORT_L9A0_Wr01(iris_046, secam_ybw, pSet_lumapeaking_t->reg_secam_ybw);
	VPORT_L9A0_WrFL(iris_046);
}

void CVD_L9Ax_Set_Secam_Freq(CVD_L9Ax_SET_SECAM_FREQ_T	*pSet_secamfreq_t)
{
	VPORT_L9A0_RdFL(iris_060);
	VPORT_L9A0_Wr01(iris_060, dr_freq, pSet_secamfreq_t->reg_dr_freq);
	VPORT_L9A0_Wr01(iris_060, db_freq, pSet_secamfreq_t->reg_db_freq);
	VPORT_L9A0_WrFL(iris_060);
}

int	CVD_L9Ax_Read_fld_cnt_value(UINT16 *pfld_hfcnt_value, UINT16 *pfld_lfcnt_value)
{
	VPORT_L9A0_RdFL(iris_208);
	VPORT_L9A0_Rd02(iris_208, status_fld_lfcnt, *pfld_lfcnt_value, status_fld_hfcnt, *pfld_hfcnt_value);
	return 0;
}

int CVD_L9Ax_Set_motion_mode(UINT8	md_mode_value, UINT8 motion_mode_value)
{
	VPORT_L9A0_RdFL(iris_189);
	VPORT_L9A0_Wr01(iris_189, cvd_3dcomb_md_mode, md_mode_value);
	VPORT_L9A0_WrFL(iris_189);
	VPORT_L9A0_RdFL(iris_064);
	VPORT_L9A0_Wr01(iris_064, motion_mode, motion_mode_value);
	VPORT_L9A0_WrFL(iris_064);
	return 0;
}

int CVD_L9Ax_Channel_Power_Control(UINT32 on_off)
{
	if(on_off)
	{
		if( lx_chip_rev( ) == LX_CHIP_REV(L9,A0) )
		{
			//L9A Workaround CVBS through AFE3CH_G
			VPORT_I2C_Read((UINT32*)&gafe_3ch_3);
			gafe_3ch_3.afe3ch_sel_ck = 0x1;
			VPORT_I2C_Write((UINT32*)&gafe_3ch_3);

			VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
			gafe_3ch_0.afe3ch_gpdb = 0x1;
			gafe_3ch_0.afe3ch_biaspdb = 0x1;
			VPORT_I2C_Write((UINT32*)&gafe_3ch_0);
		}

		//L9A_DIE TX to L9D_DIE on
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_1);
		ghslvdstx2_0.pdb3 = 1;
		ghslvdstx2_1.ch_en3 = 0x3f;
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_1);

		// Normal condition
		VPORT_I2C_Read((UINT32*)&gmain_pll_4);
		VPORT_I2C_Read((UINT32*)&gafe_cvbs_1);
		VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);
		gafe_cvbs_1.cvbs_pdbm = 0x1;
		gafe_cvbs_3.cvbs_pdb = 0x1;
		gmain_pll_4.dr3p_pdb = 0x1;

		//added 110622 : by tommy.lee to disable clamp control on power down condition
		gafe_cvbs_3.cvbs_cp = 1;
		VPORT_I2C_Write((UINT32*)&gmain_pll_4);
		VPORT_I2C_Write((UINT32*)&gafe_cvbs_1);
		VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);

		//by dws : remove mdelay
		//mdelay(5);
		OS_MsecSleep(5);

		VPORT_L9A0_RdFL(iris_013);
		VPORT_L9A0_Wr01(iris_013, cvd_soft_reset, 1);
		VPORT_L9A0_WrFL(iris_013);

		//by dws : remove mdelay
		//mdelay(5);
		OS_MsecSleep(5);

		VPORT_L9A0_RdFL(iris_013);
		VPORT_L9A0_Wr01(iris_013, cvd_soft_reset, 0);
		VPORT_L9A0_WrFL(iris_013);

/*
		VPORT_I2C_Read((UINT32*)&gsoft_reset_5);
		gsoft_reset_5.swrst_f54m = 0;
		gsoft_reset_5.swrst_cvbs = 0;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_5);
		*/
	}
	else
	{
		if( lx_chip_rev( ) == LX_CHIP_REV(L9,A0) )
		{
			//L9A Workaround CVBS through AFE3CH_G
			VPORT_I2C_Read((UINT32*)&gafe_3ch_3);
			gafe_3ch_3.afe3ch_sel_ck = 0x0;
			VPORT_I2C_Write((UINT32*)&gafe_3ch_3);

			VPORT_I2C_Read((UINT32*)&gafe_3ch_0);
			gafe_3ch_0.afe3ch_gpdb = 0x0;
			// Bias PDB should not turned off
			//		gafe_3ch_0.afe3ch_biaspdb = 0x0;
			VPORT_I2C_Write((UINT32*)&gafe_3ch_0);
		}

		//L9A_DIE TX to L9D_DIE off
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Read((UINT32*)&ghslvdstx2_1);
		ghslvdstx2_0.pdb3 = 0;
		ghslvdstx2_1.ch_en3 = 0;
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_0);
		VPORT_I2C_Write((UINT32*)&ghslvdstx2_1);

		// Power down
		/*
		VPORT_I2C_Read((UINT32*)&gsoft_reset_5);
		gsoft_reset_5.swrst_f54m = 0x1;
		gsoft_reset_5.swrst_cvbs = 0x1;
		VPORT_I2C_Write((UINT32*)&gsoft_reset_5);
		*/

		VPORT_I2C_Read((UINT32*)&gafe_cvbs_1);
		VPORT_I2C_Read((UINT32*)&gafe_cvbs_3);

		//Do Not Turn Off CVBS_PDBM, to enable clock  for DENC
		//gafe_cvbs_1.cvbs_pdbm = 0;
		gafe_cvbs_3.cvbs_pdb = 0;

		//added 110622 : by tommy.lee to disable clamp control on power down condition
		gafe_cvbs_3.cvbs_cp = 0;
		VPORT_I2C_Write((UINT32*)&gafe_cvbs_1);
		VPORT_I2C_Write((UINT32*)&gafe_cvbs_3);
	}
	return 0;
}

int CVD_L9Ax_Reset_Clampagc(void)
{
	//ADC_DEBUG("Reset Clampagc Entered\n");
	VPORT_L9A0_RdFL(iris_278);
	VPORT_L9A0_Wr01(iris_278, iris_clampagc_v2, 0x1);
	VPORT_L9A0_WrFL(iris_278);
	//by dws : remove mdelay
	//mdelay(5);
	OS_MsecSleep(5);
	VPORT_L9A0_RdFL(iris_278);
	VPORT_L9A0_Wr01(iris_278, iris_clampagc_v2, 0x0);
	VPORT_L9A0_WrFL(iris_278);
	return 0;
}

int CVD_L9Ax_Bypass_Control(LX_AFE_CVD_BYPASS_CONTROL_T *cvd_bypass_control_t)
{
	VPORT_I2C_Read((UINT32*)&gafe_vbuf_1);
	VPORT_I2C_Read((UINT32*)&gafe_vbuf_0);

	switch(cvd_bypass_control_t->buf_out_1_sel)
	{
		case CVD_BYPASS_DAC:
			gafe_vbuf_1.buf_sel1 = 0x4;
			break;

		case CVD_BYPASS_CVBS_WITH_CLAMPING:
			gafe_vbuf_1.buf_sel1 = 0x5;
			break;

		case CVD_BYPASS_CVBS_WITHOUT_CLAMPING:
			gafe_vbuf_1.buf_sel1 = 0x7;
			break;

		default:
			break;
	}

	switch(cvd_bypass_control_t->buf_out_2_sel)
	{
		case CVD_BYPASS_DAC:
			gafe_vbuf_1.buf_sel2 = 0x4;
			break;

		case CVD_BYPASS_CVBS_WITH_CLAMPING:
			gafe_vbuf_1.buf_sel2 = 0x5;
			break;

		case CVD_BYPASS_CVBS_WITHOUT_CLAMPING:
			gafe_vbuf_1.buf_sel2 = 0x7;
			break;

		default:
			break;
	}

	if(cvd_bypass_control_t->cvbs_source_sel == CVD_BYPASS_CVBS_IN1)
		gafe_vbuf_0.buf_ycm = 0x0;
	else
		gafe_vbuf_0.buf_ycm = 0x1;

	VPORT_I2C_Write((UINT32*)&gafe_vbuf_1);
	VPORT_I2C_Write((UINT32*)&gafe_vbuf_0);

	return 0;
}

int CVD_L9Ax_Get_Vline_625_Reg(void)
{
	int ret;
	VPORT_L9A0_RdFL(iris_014);
	VPORT_L9A0_Rd01(iris_014, vline_625, ret);
	return ret;
}

int CVD_L9Ax_OnOff_VDAC(BOOLEAN bonoff)
{

	AFE_PRINT("%s entered :  %d \n",__func__, bonoff);

	if(bonoff)
		gafe_vdac_0.vdac_pdb = 0x1;
	else
		gafe_vdac_0.vdac_pdb = 0x0;

	VPORT_I2C_Write((UINT32*)&gafe_vdac_0);

	return 0;
}

int CVD_L9Ax_Set_Hstate_Max(UINT32	hstate_max_value)
{

	CVD_DEBUG("%s entered :  %d \n",__func__, hstate_max_value);

	if(hstate_max_value > 5)
		return -1;

	VPORT_L9A0_RdFL(iris_028);
	VPORT_L9A0_Wr01(iris_028,  hstate_max, hstate_max_value);
	VPORT_L9A0_WrFL(iris_028);

	return 0;
}


int CVD_L9Ax_OnOff_Chromalock_Ckill(BOOLEAN bonoff)
{
	CVD_DEBUG("%s entered : This chip set does not support this function \n",__func__);
	return 0;
}




