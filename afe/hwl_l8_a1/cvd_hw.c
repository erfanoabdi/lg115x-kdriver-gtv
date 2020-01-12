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
#include "vport_reg.h"
#include "afe_drv.h"

#include "linux/delay.h"
#include "../sys/ctop_regs.h"
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
extern volatile VPORT_RegAx_T *gVportRegAx;
//extern volatile HDMI_PHY3P_Reg_T *pstHdmiPhyReg;

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

/*========================================================================================
	Implementation Group
========================================================================================*/
int CVD_L8Ax_ClampAGC_OnOff(UINT8 on_off)
{
	if(on_off)
	{
		gVportRegAx->reg_dcrestore_en.reg_cvd1_clampagc_on = 0x1;
	}
	else
	{
		gVportRegAx->reg_dcrestore_en.reg_cvd1_clampagc_on = 0x0;
	}

	return 0;
}
int CVD_L8Ax_Clamp_Current_Control(UINT8 value)
{
	if(value <= 0x3)
	{
		gVportRegAx->cvbs_lpf.cvbs_icon = value;
		return 0;
	}

	return -1;
}
int CVD_L8Ax_Force_Color_Kill(UINT8 color_kill_enable)
{
	if(color_kill_enable)
	{
		//gVportRegAx->reg_cagc_gate_start.reg_ckill = 0x0;
		_iow(&gVportRegAx->reg_cagc_gate_start, 16, 0, 0x0);
	}
	else
	{
		//gVportRegAx->reg_cagc_gate_start.reg_ckill = 0x1c00;
		_iow(&gVportRegAx->reg_cagc_gate_start, 16, 0, 0x1c00);
	}

	return 0;
}
void CVD_L8Ax_Set_Picture_Enhancement(BOOLEAN ycrdc_lcr_on, BOOLEAN cyrdc_lcr_on)
{
	AFE_PRINT("Picture Enhancement Register Setting\n");

	gVportRegAx->reg_2dcomb_byp.reg_2dcomb_byp = 0x0;
	gVportRegAx->reg_2dcomb_byp.reg_2dcomb_cband_sel = 0x0;
	gVportRegAx->reg_2dcomb_byp.reg_2dcomb_ccombo_sel = 0x0;
	gVportRegAx->reg_2dcomb_byp.reg_2dcomb_ycrdc_cvsel = 0x1;
	gVportRegAx->reg_2dcomb_byp.reg_2dcomb_ycrdc_chsel = 0x0;
	gVportRegAx->reg_2dcomb_byp.reg_2dcomb_ycrdc_lrsel = 0x1;

	// need Reg debug
	//	gVportRegAx->reg_2dcomb_divider_th.reg_2dcomb_divider_th = 0x2;
	_iow(&gVportRegAx->reg_2dcomb_divider_th, 8, 24, 0x2); //reg_2dcomb_divider_th

	gVportRegAx->reg_2dcomb_divider_th.reg_2dcomb_ycrdc_yhsymth = 0x20;
	gVportRegAx->reg_2dcomb_divider_th.reg_2dcomb_ycrdc_yhdiffth = 0x10;
	gVportRegAx->reg_2dcomb_ycrdc_cvdiffth.reg_2dcomb_ycrdc_cvdiffth = 0x40;

	gVportRegAx->reg_2dcomb_ycrdc_yvsel.reg_2dcomb_ycrdc_yvsel = 0x2;
	gVportRegAx->reg_2dcomb_ycrdc_yvsel.reg_2dcomb_ycrdc_yhsel = 0xE;
	gVportRegAx->reg_2dcomb_ycrdc_yvsel.reg_2dcomb_ycrdc_hth_on = 0x1;
	gVportRegAx->reg_2dcomb_ycrdc_yvsel.reg_2dcomb_ycrdc_hth_sel = 0x2;
	gVportRegAx->reg_2dcomb_ycrdc_yvsel.reg_2dcomb_ycrdc_hth = 0x40;

	gVportRegAx->reg_2dcomb_ycrdc_lrtype.reg_2dcomb_ycrdc_lrtype = 0x0;
	gVportRegAx->reg_2dcomb_ycrdc_lrtype.reg_2dcomb_ycrdc_dsel = 0x0;

	if(ycrdc_lcr_on)
		gVportRegAx->reg_2dcomb_ycrdc_lrtype.reg_2dcomb_ycrdc_lcr_on = 0x7; //ON/OFF : ON=7, OFF=0
	else
		gVportRegAx->reg_2dcomb_ycrdc_lrtype.reg_2dcomb_ycrdc_lcr_on = 0x0; //ON/OFF : ON=7, OFF=0

	gVportRegAx->reg_2dcomb_ycrdc_lrtype.reg_2dcomb_ycrdc_method = 0x0;

	gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_crvalid = 0x0;
	gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_lrtype = 0x1;
	gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_dsel = 0x2;

	gVportRegAx->reg_2dcomb_cyrdc_chsymth.reg_2dcomb_cyrdc_chsymth = 0xB0;
	gVportRegAx->reg_2dcomb_cyrdc_chsymth.reg_2dcomb_cyrdc_chsymth1 = 0x0;

	gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_method = 0x0;
	if(cyrdc_lcr_on)
		gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_lcr_on = 0x1; //ON/OFF : ON=1, OFF=0
	else
		gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_lcr_on = 0x0; //ON/OFF : ON=1, OFF=0
}


void CVD_L8Ax_Program_Color_System_Main_cdto_only(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{
#if 0
	gVportRegAx->reg_comb3_buffer_size.reg_comb3_buffer_size = pSet_system_t->reg_comb3_buffer_size;
	gVportRegAx->reg_fld1_init_rd_pel.reg_fld1_init_rd_pel = pSet_system_t->reg_fld1_init_rd_pel;
	gVportRegAx->reg_fld2_init_rd_pel.reg_fld2_init_rd_pel = pSet_system_t->reg_fld2_init_rd_pel;
	gVportRegAx->reg_fld3_init_rd_pel.reg_fld3_init_rd_pel = pSet_system_t->reg_fld3_init_rd_pel;
	gVportRegAx->reg_fld4_init_rd_pel.reg_fld4_init_rd_pel = pSet_system_t->reg_fld4_init_rd_pel;
#endif
#if 0

	gVportRegAx->reg_dcrestore_en.reg_cvd1_clampagc_on = pSet_system_t->reg_cvd1_clampagc_on; // from KIM.MIN
	// by Kim.min 2010/06/03

#endif
	gVportRegAx->reg_yc_src.reg_colour_mode	= pSet_system_t->reg_colour_mode;
	gVportRegAx->reg_yc_src.reg_vline_625		= pSet_system_t->reg_vline_625;
	gVportRegAx->reg_yc_src.reg_hpixel		= pSet_system_t->reg_hpixel;

	gVportRegAx->reg_ped.reg_ped				= pSet_system_t->reg_ped;

#if 0
	gVportRegAx->reg_hagc_en.reg_hagc_en= pSet_system_t->reg_hagc_en; // kim.min 0615
	gVportRegAx->reg_hagc_en.reg_cagc_en = pSet_system_t->reg_cagc_en;
	gVportRegAx->reg_hagc_en.reg_agc_half_en = pSet_system_t->reg_agc_half_en;
	gVportRegAx->reg_hagc_en.reg_dagc_en = pSet_system_t->reg_dagc_en;
	gVportRegAx->reg_hagc_en.reg_dc_clamp_mode = pSet_system_t->reg_dc_clamp_mode;
	gVportRegAx->reg_hagc_en.reg_mv_hagc_mode = pSet_system_t->reg_mv_hagc_mode;
	gVportRegAx->reg_hagc_en.reg_hagc_field_mode = pSet_system_t->reg_hagc_field_mode;
	gVportRegAx->reg_ped.reg_chroma_bw_lo = pSet_system_t->reg_chroma_bw_lo;
	gVportRegAx->reg_adaptive_mode.reg_adaptive_mode = pSet_system_t->reg_adaptive_mode;
	gVportRegAx->reg_adaptive_mode.reg_adaptive_3dcomb_mode = pSet_system_t->reg_adaptive_3dcomb_mode;

// need Reg debug
//	gVportRegAx->reg_hagc.reg_hagc = pSet_system_t->reg_hagc;
	_iow(&gVportRegAx->reg_hagc, 8, 8, pSet_system_t->reg_hagc); //reg_hagc

	gVportRegAx->reg_adc_input_swap.reg_adc_updn_swap = pSet_system_t->reg_adc_updn_swap;

	gVportRegAx->reg_yc_delay.reg_yc_delay = pSet_system_t->reg_yc_delay;

#endif
// need Reg debug
//	gVportRegAx->reg_cagc_gate_start.reg_cagc_gate_start = pSet_system_t->reg_cagc_gate_start;
//	gVportRegAx->reg_cagc_gate_start.reg_cagc_gate_end = pSet_system_t->reg_cagc_gate_end;
	_iow(&gVportRegAx->reg_cagc_gate_start, 8, 24, pSet_system_t->reg_cagc_gate_start);	//reg_cagc_gate_start
	_iow(&gVportRegAx->reg_cagc_gate_start, 8, 16, pSet_system_t->reg_cagc_gate_end);		//reg_cagc_gate_end

	gVportRegAx->reg_cdto_inc.reg_cdto_inc = pSet_system_t->reg_cdto_inc;
#if 0
	gVportRegAx->reg_hdto_inc.reg_hdto_inc = pSet_system_t->reg_hdto_inc;

// need Reg debug
//	gVportRegAx->reg_hactive_start.reg_hactive_start = pSet_system_t->reg_hactive_start;
//	gVportRegAx->reg_hactive_start.reg_hactive_width = pSet_system_t->reg_hactive_width;
	_iow(&gVportRegAx->reg_hactive_start, 8, 8, pSet_system_t->reg_hactive_start);	//reg_hactive_start
	_iow(&gVportRegAx->reg_hactive_start, 8, 0, pSet_system_t->reg_hactive_width);	//reg_hactive_width

// need Reg debug(ok)
//	gVportRegAx->reg_vactive_start.reg_vactive_start = pSet_system_t->reg_vactive_start;
//	gVportRegAx->reg_vactive_start.reg_vactive_height = pSet_system_t->reg_vactive_height;
	_iow(&gVportRegAx->reg_vactive_start, 8, 8, pSet_system_t->reg_vactive_start);
	_iow(&gVportRegAx->reg_vactive_start, 8, 0, pSet_system_t->reg_vactive_height);

	gVportRegAx->reg_adaptive_mode.reg_ntsc443_mode = pSet_system_t->reg_ntsc443_mode;
	gVportRegAx->reg_cagc.reg_cagc = pSet_system_t->reg_cagc;

// need Reg debug(ok)
//	gVportRegAx->reg_burst_gate_start.reg_burst_gate_end = pSet_system_t->reg_burst_gate_end;
	_iow(&gVportRegAx->reg_burst_gate_start, 8, 0, pSet_system_t->reg_burst_gate_end);

	gVportRegAx->reg_peak_en.reg_secam_ybw = pSet_system_t->reg_secam_ybw;

	gVportRegAx->reg_palsw_level.reg_auto_secam_level = pSet_system_t->reg_auto_secam_level;
	gVportRegAx->reg_lose_chromalock_ckill.reg_lose_chromalock_mode = pSet_system_t->reg_lose_chromalock_mode;
	gVportRegAx->reg_noise_th.reg_noise_th = pSet_system_t->reg_noise_th;
	gVportRegAx->reg_noise_th.reg_noise_th_en = pSet_system_t->reg_noise_th_en;
	gVportRegAx->reg_noise_th.reg_lowfreq_vdiff_gain = pSet_system_t->reg_lowfreq_vdiff_gain;
	gVportRegAx->reg_noise_th.reg_chroma_vdiff_gain = pSet_system_t->reg_chroma_vdiff_gain;
	gVportRegAx->reg_noise_th.reg_horiz_diff_ygain = pSet_system_t->reg_horiz_diff_ygain;
	gVportRegAx->reg_noise_th.reg_horiz_diff_cgain = pSet_system_t->reg_horiz_diff_cgain;

	gVportRegAx->reg_y_noise_th_gain.reg_y_noise_th_gain = pSet_system_t->reg_y_noise_th_gain;
	gVportRegAx->reg_y_noise_th_gain.reg_c_noise_th_gain = pSet_system_t->reg_c_noise_th_gain;
	gVportRegAx->reg_y_noise_th_gain.reg_burst_noise_th_gain = pSet_system_t->reg_burst_noise_th_gain;
	gVportRegAx->reg_y_noise_th_gain.reg_vadap_burst_noise_th_gain = pSet_system_t->reg_vadap_burst_noise_th_gain;

	gVportRegAx->reg_motion_mode.reg_motion_mode = pSet_system_t->reg_motion_mode;
	gVportRegAx->reg_motion_mode.reg_adaptive_chroma_mode = pSet_system_t->reg_adaptive_chroma_mode;

	gVportRegAx->reg_comb2d_only.reg_comb2d_only = pSet_system_t->reg_comb2d_only;
	gVportRegAx->reg_comb2d_only.reg_fb_sync = pSet_system_t->reg_fb_sync;
#endif
	gVportRegAx->reg_md_noise_th.reg_md_noise_th = pSet_system_t->reg_md_noise_th;
#if 0
	gVportRegAx->reg_md_noise_th.reg_md_noise_th_en = pSet_system_t->reg_md_noise_th_en;

// need Reg debug(OK)
//	gVportRegAx->reg_vactive_md_start.reg_vactive_md_start = pSet_system_t->reg_vactive_md_start;
//	gVportRegAx->reg_vactive_md_start.reg_vactive_md_height = pSet_system_t->reg_vactive_md_height;
	_iow(&gVportRegAx->reg_vactive_md_start, 8, 8, pSet_system_t->reg_vactive_md_start);
	_iow(&gVportRegAx->reg_vactive_md_start, 8, 0, pSet_system_t->reg_vactive_md_height);

// need Reg debug
//	gVportRegAx->reg_hactive_md_start.reg_hactive_md_start = pSet_system_t->reg_hactive_md_start;
//	gVportRegAx->reg_hactive_md_start.reg_hactive_md_width = pSet_system_t->reg_hactive_md_width;
//	gVportRegAx->reg_hactive_md_start.reg_motion_config = pSet_system_t->reg_motion_config;
	_iow(&gVportRegAx->reg_hactive_md_start, 8, 24, pSet_system_t->reg_hactive_md_start);
	_iow(&gVportRegAx->reg_hactive_md_start, 8, 16, pSet_system_t->reg_hactive_md_width);
	_iow(&gVportRegAx->reg_hactive_md_start, 8, 8, pSet_system_t->reg_motion_config);

	gVportRegAx->reg_chroma_bw_motion.reg_status_motion_mode = pSet_system_t->reg_status_motion_mode;

// need Reg debug
//	gVportRegAx->reg_flat_luma_shift.reg_frame_motion_th = pSet_system_t->reg_frame_motion_th;
	_iow(&gVportRegAx->reg_flat_luma_shift, 8, 16, pSet_system_t->reg_frame_motion_th);

// need Reg debug
//	gVportRegAx->reg_chroma_level.reg_chroma_level = pSet_system_t->reg_chroma_level;
//	gVportRegAx->reg_chroma_level.reg_hf_luma_chroma_offset = pSet_system_t->reg_hf_luma_chroma_offset;
	_iow(&gVportRegAx->reg_chroma_level, 8, 24, pSet_system_t->reg_chroma_level);
	_iow(&gVportRegAx->reg_chroma_level, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);

	gVportRegAx->reg_2dcomb_ycrdc_lrtype.reg_2dcomb_ycrdc_lcr_on = pSet_system_t->reg_2dcomb_ycrdc_lcr_on;
	gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_lcr_on = pSet_system_t->reg_2dcomb_cyrdc_lcr_on;

// kim.min 0622
	_iow(&gVportRegAx->reg_phase_offset_range, 8, 0, pSet_system_t->reg_cordic_gate_end);
	_iow(&gVportRegAx->reg_phase_offset_range, 8, 8, pSet_system_t->reg_cordic_gate_start);
	_iow(&gVportRegAx->reg_phase_offset_range, 8, 24, pSet_system_t->reg_phase_offset_range);

// kim.min 0716
	_iow(&gVportRegAx->reg_chroma_level, 8, 0, pSet_system_t->reg_tcomb_chroma_level);
	_iow(&gVportRegAx->reg_chroma_level, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);
	_iow(&gVportRegAx->reg_chroma_level, 8, 24, pSet_system_t->reg_chroma_level);


	//kim.min 0906
	gVportRegAx->reg_oadj_y_coeff.reg_oadj_y_coeff = pSet_system_t->reg_oadj_y_coeff;
	gVportRegAx->reg_adaptive_mode.reg_pal60_mode = pSet_system_t->reg_pal60_mode;

	//kim.min 0920
	gVportRegAx->reg_hdetect_clamp_level.reg_hstate_max = pSet_system_t->reg_hstate_max;
	gVportRegAx->reg_dcrestore_gain.reg_dcrestore_accum_width = pSet_system_t->reg_dcrestore_accum_width;

// From Ok : SW workaround for 3D comb operation 2010/06/04
	mdelay(30);
	 gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 1;
	mdelay(10); //0619
	 gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 0;
#endif
}
void CVD_L8Ax_Program_Color_System_Main_Multi(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{
	gVportRegAx->reg_comb3_buffer_size.reg_comb3_buffer_size = pSet_system_t->reg_comb3_buffer_size;
	gVportRegAx->reg_fld1_init_rd_pel.reg_fld1_init_rd_pel = pSet_system_t->reg_fld1_init_rd_pel;
	gVportRegAx->reg_fld2_init_rd_pel.reg_fld2_init_rd_pel = pSet_system_t->reg_fld2_init_rd_pel;
	gVportRegAx->reg_fld3_init_rd_pel.reg_fld3_init_rd_pel = pSet_system_t->reg_fld3_init_rd_pel;
	gVportRegAx->reg_fld4_init_rd_pel.reg_fld4_init_rd_pel = pSet_system_t->reg_fld4_init_rd_pel;

	gVportRegAx->reg_dcrestore_en.reg_cvd1_clampagc_on = pSet_system_t->reg_cvd1_clampagc_on; // from KIM.MIN
	// by Kim.min 2010/06/03

	gVportRegAx->reg_yc_src.reg_colour_mode	= pSet_system_t->reg_colour_mode;
	gVportRegAx->reg_yc_src.reg_vline_625		= pSet_system_t->reg_vline_625;
	gVportRegAx->reg_yc_src.reg_hpixel		= pSet_system_t->reg_hpixel;

	gVportRegAx->reg_ped.reg_ped				= pSet_system_t->reg_ped;

	gVportRegAx->reg_hagc_en.reg_hagc_en= pSet_system_t->reg_hagc_en; // kim.min 0615
	gVportRegAx->reg_hagc_en.reg_cagc_en = pSet_system_t->reg_cagc_en;
	gVportRegAx->reg_hagc_en.reg_agc_half_en = pSet_system_t->reg_agc_half_en;
	gVportRegAx->reg_hagc_en.reg_dagc_en = pSet_system_t->reg_dagc_en;
	gVportRegAx->reg_hagc_en.reg_dc_clamp_mode = pSet_system_t->reg_dc_clamp_mode;
	gVportRegAx->reg_hagc_en.reg_mv_hagc_mode = pSet_system_t->reg_mv_hagc_mode;
	gVportRegAx->reg_hagc_en.reg_hagc_field_mode = pSet_system_t->reg_hagc_field_mode;
	gVportRegAx->reg_ped.reg_chroma_bw_lo = pSet_system_t->reg_chroma_bw_lo;
	gVportRegAx->reg_adaptive_mode.reg_adaptive_mode = pSet_system_t->reg_adaptive_mode;
	gVportRegAx->reg_adaptive_mode.reg_adaptive_3dcomb_mode = pSet_system_t->reg_adaptive_3dcomb_mode;

// need Reg debug
//	gVportRegAx->reg_hagc.reg_hagc = pSet_system_t->reg_hagc;
	_iow(&gVportRegAx->reg_hagc, 8, 8, pSet_system_t->reg_hagc); //reg_hagc

	gVportRegAx->reg_adc_input_swap.reg_adc_updn_swap = pSet_system_t->reg_adc_updn_swap;

	gVportRegAx->reg_yc_delay.reg_yc_delay = pSet_system_t->reg_yc_delay;

// need Reg debug
//	gVportRegAx->reg_cagc_gate_start.reg_cagc_gate_start = pSet_system_t->reg_cagc_gate_start;
//	gVportRegAx->reg_cagc_gate_start.reg_cagc_gate_end = pSet_system_t->reg_cagc_gate_end;
	_iow(&gVportRegAx->reg_cagc_gate_start, 8, 24, pSet_system_t->reg_cagc_gate_start);	//reg_cagc_gate_start
	_iow(&gVportRegAx->reg_cagc_gate_start, 8, 16, pSet_system_t->reg_cagc_gate_end);		//reg_cagc_gate_end

	gVportRegAx->reg_cdto_inc.reg_cdto_inc = pSet_system_t->reg_cdto_inc;
	gVportRegAx->reg_hdto_inc.reg_hdto_inc = pSet_system_t->reg_hdto_inc;

// need Reg debug
//	gVportRegAx->reg_hactive_start.reg_hactive_start = pSet_system_t->reg_hactive_start;
//	gVportRegAx->reg_hactive_start.reg_hactive_width = pSet_system_t->reg_hactive_width;
	_iow(&gVportRegAx->reg_hactive_start, 8, 8, pSet_system_t->reg_hactive_start);	//reg_hactive_start
	_iow(&gVportRegAx->reg_hactive_start, 8, 0, pSet_system_t->reg_hactive_width);	//reg_hactive_width

// need Reg debug(ok)
//	gVportRegAx->reg_vactive_start.reg_vactive_start = pSet_system_t->reg_vactive_start;
//	gVportRegAx->reg_vactive_start.reg_vactive_height = pSet_system_t->reg_vactive_height;
	_iow(&gVportRegAx->reg_vactive_start, 8, 8, pSet_system_t->reg_vactive_start);
	_iow(&gVportRegAx->reg_vactive_start, 8, 0, pSet_system_t->reg_vactive_height);

	gVportRegAx->reg_adaptive_mode.reg_ntsc443_mode = pSet_system_t->reg_ntsc443_mode;
	gVportRegAx->reg_cagc.reg_cagc = pSet_system_t->reg_cagc;

// need Reg debug(ok)
//	gVportRegAx->reg_burst_gate_start.reg_burst_gate_end = pSet_system_t->reg_burst_gate_end;
	_iow(&gVportRegAx->reg_burst_gate_start, 8, 0, pSet_system_t->reg_burst_gate_end);

	gVportRegAx->reg_peak_en.reg_secam_ybw = pSet_system_t->reg_secam_ybw;

	gVportRegAx->reg_palsw_level.reg_auto_secam_level = pSet_system_t->reg_auto_secam_level;
	gVportRegAx->reg_lose_chromalock_ckill.reg_lose_chromalock_mode = pSet_system_t->reg_lose_chromalock_mode;
	gVportRegAx->reg_noise_th.reg_noise_th = pSet_system_t->reg_noise_th;
	gVportRegAx->reg_noise_th.reg_noise_th_en = pSet_system_t->reg_noise_th_en;
	gVportRegAx->reg_noise_th.reg_lowfreq_vdiff_gain = pSet_system_t->reg_lowfreq_vdiff_gain;
	gVportRegAx->reg_noise_th.reg_chroma_vdiff_gain = pSet_system_t->reg_chroma_vdiff_gain;
	gVportRegAx->reg_noise_th.reg_horiz_diff_ygain = pSet_system_t->reg_horiz_diff_ygain;
	gVportRegAx->reg_noise_th.reg_horiz_diff_cgain = pSet_system_t->reg_horiz_diff_cgain;

	gVportRegAx->reg_y_noise_th_gain.reg_y_noise_th_gain = pSet_system_t->reg_y_noise_th_gain;
	gVportRegAx->reg_y_noise_th_gain.reg_c_noise_th_gain = pSet_system_t->reg_c_noise_th_gain;
	gVportRegAx->reg_y_noise_th_gain.reg_burst_noise_th_gain = pSet_system_t->reg_burst_noise_th_gain;
	gVportRegAx->reg_y_noise_th_gain.reg_vadap_burst_noise_th_gain = pSet_system_t->reg_vadap_burst_noise_th_gain;

	gVportRegAx->reg_motion_mode.reg_motion_mode = pSet_system_t->reg_motion_mode;
	gVportRegAx->reg_motion_mode.reg_adaptive_chroma_mode = pSet_system_t->reg_adaptive_chroma_mode;

	gVportRegAx->reg_comb2d_only.reg_comb2d_only = pSet_system_t->reg_comb2d_only;
	gVportRegAx->reg_comb2d_only.reg_fb_sync = pSet_system_t->reg_fb_sync;
	gVportRegAx->reg_md_noise_th.reg_md_noise_th = pSet_system_t->reg_md_noise_th;
	gVportRegAx->reg_md_noise_th.reg_md_noise_th_en = pSet_system_t->reg_md_noise_th_en;

// need Reg debug(OK)
//	gVportRegAx->reg_vactive_md_start.reg_vactive_md_start = pSet_system_t->reg_vactive_md_start;
//	gVportRegAx->reg_vactive_md_start.reg_vactive_md_height = pSet_system_t->reg_vactive_md_height;
	_iow(&gVportRegAx->reg_vactive_md_start, 8, 8, pSet_system_t->reg_vactive_md_start);
	_iow(&gVportRegAx->reg_vactive_md_start, 8, 0, pSet_system_t->reg_vactive_md_height);

// need Reg debug
//	gVportRegAx->reg_hactive_md_start.reg_hactive_md_start = pSet_system_t->reg_hactive_md_start;
//	gVportRegAx->reg_hactive_md_start.reg_hactive_md_width = pSet_system_t->reg_hactive_md_width;
//	gVportRegAx->reg_hactive_md_start.reg_motion_config = pSet_system_t->reg_motion_config;
	_iow(&gVportRegAx->reg_hactive_md_start, 8, 24, pSet_system_t->reg_hactive_md_start);
	_iow(&gVportRegAx->reg_hactive_md_start, 8, 16, pSet_system_t->reg_hactive_md_width);
	_iow(&gVportRegAx->reg_hactive_md_start, 8, 8, pSet_system_t->reg_motion_config);

	gVportRegAx->reg_chroma_bw_motion.reg_status_motion_mode = pSet_system_t->reg_status_motion_mode;

// need Reg debug
//	gVportRegAx->reg_flat_luma_shift.reg_frame_motion_th = pSet_system_t->reg_frame_motion_th;
	_iow(&gVportRegAx->reg_flat_luma_shift, 8, 16, pSet_system_t->reg_frame_motion_th);

// need Reg debug
//	gVportRegAx->reg_chroma_level.reg_chroma_level = pSet_system_t->reg_chroma_level;
//	gVportRegAx->reg_chroma_level.reg_hf_luma_chroma_offset = pSet_system_t->reg_hf_luma_chroma_offset;
	_iow(&gVportRegAx->reg_chroma_level, 8, 24, pSet_system_t->reg_chroma_level);
	_iow(&gVportRegAx->reg_chroma_level, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);

	gVportRegAx->reg_2dcomb_ycrdc_lrtype.reg_2dcomb_ycrdc_lcr_on = pSet_system_t->reg_2dcomb_ycrdc_lcr_on;
	gVportRegAx->reg_2dcomb_cyrdc_method.reg_2dcomb_cyrdc_lcr_on = pSet_system_t->reg_2dcomb_cyrdc_lcr_on;

// kim.min 0622
	_iow(&gVportRegAx->reg_phase_offset_range, 8, 0, pSet_system_t->reg_cordic_gate_end);
	_iow(&gVportRegAx->reg_phase_offset_range, 8, 8, pSet_system_t->reg_cordic_gate_start);
	_iow(&gVportRegAx->reg_phase_offset_range, 8, 24, pSet_system_t->reg_phase_offset_range);

// kim.min 0716
	_iow(&gVportRegAx->reg_chroma_level, 8, 0, pSet_system_t->reg_tcomb_chroma_level);
	_iow(&gVportRegAx->reg_chroma_level, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);
	_iow(&gVportRegAx->reg_chroma_level, 8, 24, pSet_system_t->reg_chroma_level);


	//kim.min 1103
	gVportRegAx->reg_oadj_y_offi.reg_oadj_y_offo = pSet_system_t->reg_oadj_y_offo;
	//kim.min 0906
	gVportRegAx->reg_oadj_y_coeff.reg_oadj_y_coeff = pSet_system_t->reg_oadj_y_coeff;
	gVportRegAx->reg_adaptive_mode.reg_pal60_mode = pSet_system_t->reg_pal60_mode;

	//kim.min 0920
	gVportRegAx->reg_hdetect_clamp_level.reg_hstate_max = pSet_system_t->reg_hstate_max;
	gVportRegAx->reg_dcrestore_gain.reg_dcrestore_accum_width = pSet_system_t->reg_dcrestore_accum_width;

// From Ok : SW workaround for 3D comb operation 2010/06/04
	mdelay(30);
	 gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 1;
	mdelay(10); //0619
	 gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 0;
}

void CVD_L8Ax_Program_Color_System_CHB_NTSC(void)
{
		gVportRegAx->reg_swrst_chbcvd.reg_swrst_chbcvd = 1;

	gVportRegAx->chbreg_hv_delay.chbreg_colour_mode = 0x0;
	gVportRegAx->chbreg_hv_delay.chbreg_vline_625 = 0x0;
	gVportRegAx->chbreg_hv_delay.chbreg_hpixel = 0x0;
	gVportRegAx->chbreg_luma_notch_bw.chbreg_ped = 0x1;

//	gVportRegAx->chbreg_cagc_en.chbreg_cagc_en = 0x1;

//	gVportRegAx->chbreg_luma_notch_bw.chbreg_hagc_half_en = 0x1;		//check
//	gVportRegAx->chbreg_luma_notch_bw.chbreg_dc_clamp_mode = 0x0;
//	gVportRegAx->chbreg_luma_notch_bw.chbreg_hagc_field_mode = 0x0;
//	gVportRegAx->chbreg_luma_notch_bw.chbreg_chroma_bw_lo = 0x0;

//	gVportRegAx->chbreg_adaptive_mode.chbreg_adaptive_mode = 0x0;

	_iow(&gVportRegAx->chbreg_adaptive_mode, 8, 8, 0xdd); //chbreg_hagc

	_iow(&gVportRegAx->chbreg_cagc, 8, 16, 0xa8); // chbreg_cagc

	gVportRegAx->chbreg_cdto_inc.chbreg_cdto_inc = 0x21f07c1f;

	_iow(&gVportRegAx->chbreg_dr_freq, 8, 8, 0x00); // chbreg_dr_freq
	_iow(&gVportRegAx->chbreg_dr_freq, 8, 0, 0x00); // chbreg_db_freq

	_iow(&gVportRegAx->chbreg_hactive_start, 8, 8, 0x82); // chbreg_hactive_start
	_iow(&gVportRegAx->chbreg_hactive_start, 8, 0, 0x50); // chbreg_hactive_width

	_iow(&gVportRegAx->chbreg_vactive_start, 8, 8, 0x22); // chbreg_vactive_start
	_iow(&gVportRegAx->chbreg_vactive_start, 8, 0, 0x61); // chbreg_vactive_height

	gVportRegAx->chbreg_lose_chromalock_count.chbreg_lose_chromalock_mode = 0x3;

	gVportRegAx->chbreg_cagc_tc_p.chbreg_cagc_tc_ibig = 0x4;

	/*
	gVportRegAx->chbreg_adc_updn_swap.chbreg_adc_updn_swap = 0x1;
	gVportRegAx->chbreg_yc_delay.chbreg_yc_delay = 0x0;
	gVportRegAx->chbreg_hdto_inc.chbreg_hdto_inc = 0x20000000;

	gVportRegAx->chbreg_dr_freq.chbreg_dr_freq	= 0x0;
	gVportRegAx->chbreg_dr_freq.chbreg_db_freq	= 0x0;

	gVportRegAx->chbreg_hv_delay.chbreg_ntsc443_mode = 0x0;

	_iow(&gVportRegAx->chbreg_hblank_start, 8, 0, 0x46); // chbreg_burst_gate_end

	gVportRegAx->chbreg_secam_ybw.chbreg_secam_ybw = 0x0;
	gVportRegAx->chbreg_secam_ybw.chbreg_auto_secam_level = 0x0;


	gVportRegAx->chbreg_noise_th.chbreg_noise_th = 0x2;
	gVportRegAx->chbreg_cpump_adjust_polarity.chbreg_noise_th_en = 0x0;

	gVportRegAx->chbreg_adaptive_mode.chbreg_adaptive_chroma_mode = 0x1;
	*/
		gVportRegAx->reg_swrst_chbcvd.reg_swrst_chbcvd = 0;

}
void CVD_L8Ax_ALL_Power_Down(void)
{

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
	 gVportRegAx->afe3ch_biaspdb.afe3ch_biaspdb = 0x0;

	 /* LLPLL PDB */
	 gVportRegAx->reg_cvbspll_pdb.reg_cvbspll_pdb = 0;
	 gVportRegAx->reg_cvbspll_pdb.reg_ch3pll_pdb = 0;

	 /* LLPLL/CST SWRST & 3CH_DIG SWRST */
	 gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 0x1;
	 gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0x1;
}

void CVD_L8Ax_SW_Reset(LX_AFE_CVD_SELECT_T select_main_sub)
{
	if(select_main_sub == LX_CVD_MAIN)
	{
		// SET SW reset registers to '1'
//		gVportRegAx->reg_swrst_exta.reg_swrst_exta = 1;
//		gVportRegAx->reg_swrst_exta.reg_swrst_extb = 1;

		gVportRegAx->reg_swrst_f81.reg_swrst_f81 = 1;
		gVportRegAx->reg_swrst_f81.reg_swrst_irisin = 1;
		gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 1;

		gVportRegAx->reg_swrst_irisrm.reg_swrst_irisrm = 1;
		gVportRegAx->reg_swrst_irisrm.reg_swrst_iriswm = 1;

		//from cvd_test.cmm
		gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 1;
		gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 1;

		// dws added
		mdelay(10); //0809
		//gVportRegAx->reg_comb2d_only.reg_lbadrgen_rst = 1;

		//Clear SW Reset Registers to '0'
//		gVportRegAx->reg_swrst_exta.reg_swrst_exta = 0;
//		gVportRegAx->reg_swrst_exta.reg_swrst_extb = 0;

		gVportRegAx->reg_swrst_f81.reg_swrst_f81 = 0;
		gVportRegAx->reg_swrst_f81.reg_swrst_irisin = 0;
		gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 0;

		gVportRegAx->reg_swrst_irisrm.reg_swrst_irisrm = 0;
		gVportRegAx->reg_swrst_irisrm.reg_swrst_iriswm = 0;

		//from cvd_test.cmm
		gVportRegAx->reg_swrst_llpll.reg_swrst_llpll = 0;
		gVportRegAx->reg_swrst_llpll.reg_swrst_ch3pix = 0;

		// dws added
		//gVportRegAx->reg_comb2d_only.reg_lbadrgen_rst = 0;
	}
	else
	{
		// SET SW reset registers to '1'
		gVportRegAx->chbreg_cagc_en.chbreg_lbadrgen_rst	= 1;
		gVportRegAx->reg_swrst_chbcvd.reg_swrst_chbcvd = 1;

		//Clear SW Reset Registers to '0'
		gVportRegAx->chbreg_cagc_en.chbreg_lbadrgen_rst	= 0;
		gVportRegAx->reg_swrst_chbcvd.reg_swrst_chbcvd = 0;
	}
}



// BOOLEAN PowerOnOff
// TRUE : Power Down
// FALSE : Power Up
void CVD_L8Ax_Power_Down(LX_AFE_CVD_SELECT_T select_main_sub, BOOLEAN PowerOnOFF)
{
	if(select_main_sub == LX_CVD_MAIN)
	{
		//if(PowerOnOFF==TRUE)
		if(PowerOnOFF==FALSE) // from cvd_test.cmm
		{
			// from cvd_test.cmm
			gVportRegAx->dr3p_m.dr3p_m = 0x0;
			gVportRegAx->dr3p_m.dr3p_npc = 0x6;
			gVportRegAx->dr3p_m.dr3p_nsc = 0x3;
			gVportRegAx->dr3p_m.dr3p_od_cvdclk = 0x3;
			gVportRegAx->dr3p_m.dr3p_od_recclk = 0x5;
			gVportRegAx->dr3p_m.dr3p_ps23c = 0x0;
			gVportRegAx->dr3p_m.dr3p_cih = 0x2;


			gVportRegAx->reg_cvbspll_pdb.reg_cvbspll_pdb = 0x1;
			gVportRegAx->cvbs_pdbm.cvbs_pdbm = 0x1;
			gVportRegAx->cvbs_pdbm.cvbs1_pdb = 0x1;
			gVportRegAx->cvbs_pdbm.cvbs2_pdb = 0x0;	// CVBS block for Chroma
		}
		else
		{
			gVportRegAx->reg_cvbspll_pdb.reg_cvbspll_pdb = 0;
			gVportRegAx->cvbs_pdbm.cvbs_pdbm = 0;
			gVportRegAx->cvbs_pdbm.cvbs1_pdb = 0;
			gVportRegAx->cvbs_pdbm.cvbs2_pdb = 0;
		}
	}
	else
	{
		//if(PowerOnOFF==TRUE)
		if(PowerOnOFF==FALSE)// from cvd_test.cmm
		{
			gVportRegAx->cb_lpf.cb_lpf = 0x1;
			gVportRegAx->cb_lpf.cb_pdb = 0x1;
			gVportRegAx->cb_lpf.cb_icon = 0x2;
			gVportRegAx->cb_lpf.cb_cp = 0x1;
			gVportRegAx->cb_lpf.cb_selref = 0x2;
			gVportRegAx->cb_lpf.cb_dcsel = 0x2;
			gVportRegAx->cb_lpf.cb_cnt = 0x1;
			gVportRegAx->cb_lpf.cb_cns = 0x0;
		}
		else
			gVportRegAx->cb_lpf.cb_pdb = 0;
	}
}

void CVD_L8Ax_Reg_Init(LX_AFE_CVD_SELECT_T select_main_sub)
{
	if(select_main_sub == LX_CVD_MAIN)
	{
		gVportRegAx->reg_yc_delay.reg_blue_mode = 0x0;	// Disable Blue Mode

		// 0909 by kim.min(same setting with FPGA)
		gVportRegAx->cvbs1_cp.cvbs_bw = 0x0;
		gVportRegAx->cvbs_lpf.cvbs_lpf = 0x1;
		gVportRegAx->cvbs_lpf.cvbs_byp = 0x1;

	}
	else
	{
		gVportRegAx->chbreg_cbcr_swap.chbreg_blue_mode = 0x0;	// Disable Blue Mode
	}

	//reg init for memory. by SC Jung
	gVportRegAx->reg_gmau_cmd_base.reg_gmau_cmd_base = gpCvdMemCfg->memory_base;

}


int CVD_L8Ax_Set_Source_Type(UINT32 arg)
{

	LX_AFE_CVD_SET_INPUT_T	cvd_input_info;

	AFE_PRINT("Entered CVD_Set_Source_Type\n");
	if (copy_from_user(&cvd_input_info, (void __user *)arg, sizeof(LX_AFE_CVD_SET_INPUT_T)))
	{
		AFE_PRINT("Error return at CVD_Set_Source_Type\n");
		return -1;
	}

	if(cvd_input_info.cvd_main_sub == LX_CVD_MAIN)
	{
		AFE_PRINT("Main CVD input\n");
		if(cvd_input_info.cvd_input_source_type == LX_CVD_INPUT_SOURCE_CVBS) // for composite
		{
		AFE_PRINT("Composite input\n");
			gVportRegAx->reg_yc_src.reg_yc_src = 0;	// select composite

			AFE_PRINT("Input source = [%d]\n", cvd_input_info.cvbs_input_port);

			//gVportRegAx->reg_exta_sel.reg_exta_sel = 0;	// from cvd_test.cmm
			//gVportRegAx->reg_exta_sel.reg_extb_sel = 0;	// from cvd_test.cmm

			switch(cvd_input_info.cvbs_input_port)
			{
				case LX_AFE_CVBS_IN1:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x2;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN2:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x3;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN3:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x0;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN4:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x1;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN5:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x2;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 1);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN6:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x3;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 1);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN7:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x0;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 1);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN8:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x1;
					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 1);
					CTOP_CTRL_WrFL(ctr61);
					break;

				default:
					break;
			}
		}
		else // for S-Video
		{
			gVportRegAx->reg_yc_src.reg_yc_src = 1;	// select y/c
			CTOP_CTRL_RdFL(ctr61);
			CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
			CTOP_CTRL_WrFL(ctr61);

			switch(cvd_input_info.cvbs_input_port)
			{
				case LX_AFE_CVBS_IN1:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x2;
					break;

				case LX_AFE_CVBS_IN2:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x3;
					break;

				case LX_AFE_CVBS_IN3:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x0;
					break;

				case LX_AFE_CVBS_IN4:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x1;
					break;

				default:
					break;
			}

			switch(cvd_input_info.chroma_input_port)
			{
				case LX_AFE_CVBS_IN5:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x3;
					break;

				case LX_AFE_CVBS_IN6:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x0;
					break;

				case LX_AFE_CVBS_IN7:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x2;
					break;

				case LX_AFE_CVBS_IN8:
					gVportRegAx->cvbs1_insel.cvbs1_insel = 0x1;
					break;

				default:
					break;
			}
		}

	  Current_State = CVD_STATE_VideoNotReady;
	  Next_State = CVD_STATE_VideoNotReady;

	}
	else // For CHB input
	{
		/* No source select fuction on L8 */
		/* On L8, CHB source is fixed on CVBS_CB */
	}
	return 0;

}

int CVD_L8Ax_Set_Scart_Overlay(BOOLEAN arg)
{
	if(arg == TRUE)
		gVportRegAx->reg_fb_blend_ratio.reg_fb_blend_ratio = 0x10;
	else
		gVportRegAx->reg_fb_blend_ratio.reg_fb_blend_ratio = 0x0;

	return 0;
}

UINT8 CVD_L8Ax_Get_FC_Flag(LX_AFE_CVD_SELECT_T select_main_sub)
{
	UINT8 cordic_freq_status;

	if(select_main_sub == LX_CVD_MAIN)
		cordic_freq_status = (UINT8)((SINT8)gVportRegAx->reg_status_cgain.reg_status_cordic_freq + 0x80);
	else
		cordic_freq_status = (UINT8)((SINT8)gVportRegAx->chbreg_status_cgain.chbreg_status_cordic_freq + 0x80);

#ifdef CVD_KERNEL_DEBUG
	AFE_PRINT("cordic_freq = [%d],", cordic_freq_status);
#endif

	if(cordic_freq_status > FC_MORE_THRESHOLD)
		return CVD_FC_MORE_FLAG;
	else if(cordic_freq_status < FC_LESS_THRESHOLD)
		return CVD_FC_LESS_FLAG;
	else
		return CVD_FC_SAME_FLAG;
}

UINT8 CVD_L8Ax_Get_Cordic_Freq(LX_AFE_CVD_SELECT_T select_main_sub)
{
	UINT8 cordic_freq_status;

	if(select_main_sub == LX_CVD_MAIN)
		cordic_freq_status = (UINT8)((SINT8)gVportRegAx->reg_status_cgain.reg_status_cordic_freq + 0x80);
	else
		cordic_freq_status = (UINT8)((SINT8)gVportRegAx->chbreg_status_cgain.chbreg_status_cordic_freq + 0x80);

	return cordic_freq_status;
}

#if 0		// Not Used
void CVD_L8Ax_Program_Color_System_CHB(CVD_SET_SYSTEM_CHB_T set_system_t)
{
	//CHB CVD Power Down
	//gVportRegAx->cb_lpf.cb_pdb = set_system_t.CB_PDB;
	gVportRegAx->chbreg_hv_delay.chbreg_hv_delay = set_system_t.chbreg_hv_delay;
	gVportRegAx->chbreg_hv_delay.chbreg_hpixel = set_system_t.chbreg_hpixel;
	gVportRegAx->chbreg_hv_delay.chbreg_vline_625		= set_system_t.chbreg_vline_625;
	gVportRegAx->chbreg_hv_delay.chbreg_colour_mode	= set_system_t.chbreg_colour_mode;
	gVportRegAx->chbreg_hv_delay.chbreg_ntsc443_mode	= set_system_t.chbreg_ntsc443_mode;

	gVportRegAx->chbreg_luma_notch_bw.chbreg_luma_notch_bw	= set_system_t.chbreg_luma_notch_bw;
	gVportRegAx->chbreg_luma_notch_bw.chbreg_chroma_bw_lo	= set_system_t.chbreg_chroma_bw_lo;
	gVportRegAx->chbreg_luma_notch_bw.chbreg_chroma_burst5or10	= set_system_t.chbreg_chroma_burst5or10;
	gVportRegAx->chbreg_luma_notch_bw.chbreg_ped				= set_system_t.chbreg_ped;

	// CHB CVD Reset
	//gVportRegAx->chbreg_cagc_en.chbreg_lbadrgen_rst			= set_system_t.chbreg_lbadrgen_rst;

	gVportRegAx->chbreg_adaptive_mode.chbreg_adaptive_mode	= set_system_t.chbreg_adaptive_mode;
	gVportRegAx->chbreg_adaptive_mode.chbreg_adaptive_chroma_mode	= set_system_t.chbreg_adaptive_chroma_mode;
	gVportRegAx->chbreg_adaptive_mode.chbreg_hagc	= set_system_t.chbreg_hagc;

	gVportRegAx->chbreg_cagc.chbreg_cagc	= set_system_t.chbreg_cagc;

	gVportRegAx->chbreg_cdto_inc.chbreg_cdto_inc	= set_system_t.chbreg_cdto_inc;

	gVportRegAx->chbreg_dr_freq.chbreg_dr_freq	= set_system_t.chbreg_dr_freq;
	gVportRegAx->chbreg_dr_freq.chbreg_db_freq	= set_system_t.chbreg_db_freq;

	gVportRegAx->chbreg_hactive_start.chbreg_hactive_start 	= set_system_t.chbreg_hactive_start;
	gVportRegAx->chbreg_vactive_start.chbreg_vactive_start 	= set_system_t.chbreg_vactive_start;
	gVportRegAx->chbreg_vactive_start.chbreg_vactive_height			= set_system_t.chbreg_vactive_height;
	gVportRegAx->chbreg_secam_ybw.chbreg_auto_secam_level		= set_system_t.chbreg_auto_secam_level;
	gVportRegAx->chbreg_secam_ybw.chbreg_palsw_level		= set_system_t.chbreg_palsw_level;
	gVportRegAx->chbreg_lose_chromalock_count.chbreg_lose_chromalock_mode		= set_system_t.chbreg_lose_chromalock_mode;
	gVportRegAx->chbreg_cagc_tc_p.chbreg_cagc_tc_p = set_system_t.chbreg_cagc_tc_p;
	gVportRegAx->chbreg_cagc_tc_p.chbreg_cagc_tc_ibig = set_system_t.chbreg_cagc_tc_ibig;
	gVportRegAx->chbreg_cagc_tc_p.chbreg_cagc_tc_ismall = set_system_t.chbreg_cagc_tc_ismall;
	// CHB CVD Reset
	//gVportRegAx->reg_swrst_chbcvd.reg_swrst_chbcvd = set_system_t.reg_swrst_chbcvd;

}
#endif

int CVD_L8Ax_Get_Scart_AR(UINT32 arg)
{
	LX_AFE_SCART_AR_INFO_T	scart_ar_param;

	if (copy_from_user(&scart_ar_param, (void __user *)arg, sizeof(LX_AFE_SCART_AR_INFO_T)))
	{
		return -EFAULT;
	}

	if(scart_ar_param.Scart_Id == LX_SCART_ID_1)
	{
		if((gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc1_sid2==0)&&(gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc1_sid1==0))
			scart_ar_param.Scart_AR = LX_SCART_AR_INVALID;
		else if((gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc1_sid2==1)&&(gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc1_sid1==1))
			scart_ar_param.Scart_AR = LX_SCART_AR_4_3;
		else
			scart_ar_param.Scart_AR = LX_SCART_AR_16_9;
	}
	else
	{
		if((gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc2_sid2==0)&&(gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc2_sid1==0))
			scart_ar_param.Scart_AR = LX_SCART_AR_INVALID;
		else if((gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc2_sid2==1)&&(gVportRegAx->afe3ch_sc1_sid2.afe3ch_sc2_sid1==1))
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

void CVD_L8Ax_Print_Vport_Version(void)
{
   UINT32 vport_version = 0;

   vport_version = gVportRegAx->vport_ver.vport_ver;

   AFE_PRINT("Vport Version : [%x]\n", vport_version);

}

int CVD_L8Ax_Vport_Output(UINT32 arg)
{
	LX_AFE_VPORT_OUT_INFO_T	vport_output;

	if (copy_from_user(&vport_output, (void __user *)arg, sizeof(LX_AFE_VPORT_OUT_INFO_T)))
	{
		return -EFAULT;
	}

   AFE_PRINT("Vport Output Select : output src=[%d], output ch=[%d]\n", vport_output.sel_out_src, vport_output.sel_out_ch);

	if(vport_output.sel_out_ch == LX_AFE_VPORT_OUT_A)
		//gVportRegAx->reg_swrst_exta.reg_swrst_exta = 0;
		gVportRegAx->reg_exta_sel.reg_exta_sel = vport_output.sel_out_src;	// from cvd_test.cmm
	else // CVI_B
		//gVportRegAx->reg_swrst_exta.reg_swrst_extb = 0;
		gVportRegAx->reg_exta_sel.reg_extb_sel = vport_output.sel_out_src;	// from cvd_test.cmm

	return 0;
}


int CVD_L8Ax_Vport_Reg_Read(UINT32 arg)
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

	  recvAddr += (UINT32)gVportRegAx;

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

int CVD_L8Ax_Vport_Reg_Write(UINT32 arg)
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
		 recvAddr += (UINT32)gVportRegAx;

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

int CVD_L8Ax_Get_FB_Status(UINT32 arg)
{
	LX_AFE_SCART_MODE_T		scart_fb_mode;

	scart_fb_mode = LX_SCART_MODE_CVBS;

	if (copy_to_user((UINT32*)arg, (UINT32*)&scart_fb_mode, sizeof(LX_AFE_SCART_MODE_T)))
	{
		return -EFAULT;
	}

	return 0;
}

int CVD_L8Ax_Get_No_Signal_Flag(void)
{
	return gVportRegAx->reg_no_signal.reg_no_signal;
}

int CVD_L8Ax_Get_HLock_Flag(void)
{
	return gVportRegAx->reg_no_signal.reg_hlock;
}

int CVD_L8Ax_Get_VLock_Flag(void)
{
	return gVportRegAx->reg_no_signal.reg_vlock;
}

int CVD_L8Ax_Get_Vline_625_Flag(void)
{
	return gVportRegAx->reg_hnon_standard.reg_vline_625_detected;
}

int CVD_L8Ax_Get_PAL_Flag(void)
{
	return gVportRegAx->reg_hnon_standard.reg_pal_detected;
}

int CVD_L8Ax_Get_SECAM_Flag(void)
{
	return gVportRegAx->reg_hnon_standard.reg_secam_detected;
}

int CVD_L8Ax_Get_Chromalock_Flag(void)
{
	return gVportRegAx->reg_no_signal.reg_chromalock;
}

int CVD_L8Ax_Get_Noise_Status(void)
{
	return gVportRegAx->reg_status_noise.reg_status_noise;
}

int CVD_L8Ax_Get_NoBurst_Flag(void)
{
	return gVportRegAx->reg_hnon_standard.reg_noburst_detected;
}


int CVD_L8Ax_Get_CHB_No_Signal_Flag(void)
{
	return gVportRegAx->chbreg_chromalock.chbreg_no_signal;
}

int CVD_L8Ax_Get_CHB_HLock_Flag(void)
{
	return gVportRegAx->chbreg_chromalock.chbreg_hlock;
}

int CVD_L8Ax_Get_CHB_VLock_Flag(void)
{
	return gVportRegAx->chbreg_chromalock.chbreg_vlock;
}

int CVD_L8Ax_Get_CHB_Chromalock_Flag(void)
{
	return gVportRegAx->chbreg_chromalock.chbreg_chromalock;
}

int CVD_L8Ax_Get_CHB_PAL_Flag(void)
{
	return gVportRegAx->chbreg_noisy.chbreg_pal_detected;
}

int CVD_L8Ax_Get_CHB_Vline_625_Flag(void)
{
	return gVportRegAx->chbreg_noisy.chbreg_vline_625_detected;
}

int CVD_L8Ax_Get_CHB_SECAM_Flag(void)
{
	return gVportRegAx->chbreg_noisy.chbreg_secam_detected;
}

int CVD_L8Ax_Reset_irisyc(int enable)
{
	if(enable > 0)
		gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 1;
	else
		gVportRegAx->reg_swrst_f81.reg_swrst_irisyc = 0;

	return 0;
}
int	CVD_L8Ax_Read_fld_cnt_value(UINT16 *pfld_hfcnt_value, UINT16 *pfld_lfcnt_value)
{
	return 0;
}

int CVD_L8Ax_Set_motion_mode(UINT8	md_mode_value, UINT8 motion_mode_value)
{
	return 0;
}
int CVD_L8Ax_Channel_Power_Control(UINT32 on_off)
{
	return 0;
}

int CVD_L8Ax_OnOff_Chromalock_Ckill(BOOLEAN bonoff)
{
	CVD_DEBUG("%s entered : This chip set does not support this function \n",__func__);
	return 0;
}


