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

#include "../sys/ctop_regs.h"

#include "linux/delay.h"
/*----------------------------------------------------------------------------------------
  Constant Definitions
  ----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
  Macro Definitions
  ----------------------------------------------------------------------------------------*/
#define VPORT_REG_DIRECT_ACCESS 0
#define	HSTART_SHIFT_DUE_TO_DE_CROP_WORKAROUND	1

/*----------------------------------------------------------------------------------------
  Type Definitions
  ----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
  External Function Prototype Declarations
  ----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
  External Variables
  ----------------------------------------------------------------------------------------*/
extern volatile VPORT_RegBx_T *gVportRegBx;
//extern volatile HDMI_PHY3P_Reg_T *pstHdmiPhyReg;
extern			VPORT_RegBx_T *gpRegVPORT;		// shadow register

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
int CVD_L8Bx_ClampAGC_OnOff(UINT8 on_off)
{
	if(on_off)
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_099.reg_cvd1_clampagc_on = 0x1;
#else
		VPORT_RdFL(iris_099);
		VPORT_Wr01(iris_099, reg_cvd1_clampagc_on, 0x1);
		VPORT_WrFL(iris_099);
#endif

	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_099.reg_cvd1_clampagc_on = 0x0;
#else
		VPORT_RdFL(iris_099);
		VPORT_Wr01(iris_099, reg_cvd1_clampagc_on, 0x0);
		VPORT_WrFL(iris_099);
#endif

	}

	return 0;
}
int CVD_L8Bx_Clamp_Current_Control(UINT8 value)
{
	if(value <= 0x3)
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->cvbsafe_005.cvbs_icon = value;
#else
		VPORT_RdFL(cvbsafe_005);
		VPORT_Wr01(cvbsafe_005, cvbs_icon, value);
		VPORT_WrFL(cvbsafe_005);
#endif

		return 0;
	}

	return -1;
}
int CVD_L8Bx_Force_Color_Kill(UINT8 color_kill_enable)
{
	if(color_kill_enable)
	{

#if VPORT_REG_DIRECT_ACCESS
		// gVportRegBx->iris_097.reg_ckill = 0x0;
		_iow(&gVportRegBx->iris_097, 16, 0, 0x0);
#else
		VPORT_RdFL(iris_097);
		VPORT_Wr01(iris_097, reg_ckill, 0x0);
		VPORT_WrFL(iris_097);
#endif


	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		// gVportRegBx->iris_097.reg_ckill = 0x1c00;
		_iow(&gVportRegBx->iris_097, 16, 0, 0x1c00);
#else
		VPORT_RdFL(iris_097);
		VPORT_Wr01(iris_097, reg_ckill, 0x1c00);
		VPORT_WrFL(iris_097);
#endif


	}

	return 0;
}
void CVD_L8Bx_Set_Picture_Enhancement(BOOLEAN ycrdc_lcr_on, BOOLEAN cyrdc_lcr_on)
{
	AFE_PRINT("Picture Enhancement Register Setting\n");


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_155.reg_2dcomb_byp = 0x0;
	gVportRegBx->iris_155.reg_2dcomb_cband_sel = 0x0;
	gVportRegBx->iris_155.reg_2dcomb_ccombo_sel = 0x0;
	gVportRegBx->iris_155.reg_2dcomb_ycrdc_cvsel = 0x1;
	gVportRegBx->iris_155.reg_2dcomb_ycrdc_chsel = 0x0;
	gVportRegBx->iris_155.reg_2dcomb_ycrdc_lrsel = 0x1;
#else
	VPORT_RdFL(iris_155);
	VPORT_Wr06(iris_155, reg_2dcomb_byp, 0x0, reg_2dcomb_cband_sel, 0x0, reg_2dcomb_ccombo_sel, 0x0 , reg_2dcomb_ycrdc_cvsel, 0x1, reg_2dcomb_ycrdc_chsel, 0x0, reg_2dcomb_ycrdc_lrsel, 0x1);
	VPORT_WrFL(iris_155);
#endif

#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug
	//gVportRegBx->reg_2dcomb_divider_th.reg_2dcomb_divider_th = 0x2;
	_iow(&gVportRegBx->iris_156, 8, 24, 0x2); //reg_2dcomb_divider_th
	gVportRegBx->iris_156.reg_2dcomb_ycrdc_yhsymth = 0x20;
	gVportRegBx->iris_156.reg_2dcomb_ycrdc_yhdiffth = 0x10;
#else
	VPORT_RdFL(iris_156);
	VPORT_Wr03(iris_156, reg_2dcomb_divider_th, 0x2, reg_2dcomb_ycrdc_yhsymth, 0x20, reg_2dcomb_ycrdc_yhdiffth, 0x10);
	VPORT_WrFL(iris_156);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_157.reg_2dcomb_ycrdc_cvdiffth = 0x40;
#else
	VPORT_RdFL(iris_157);
	VPORT_Wr01(iris_157, reg_2dcomb_ycrdc_cvdiffth, 0x40);
	VPORT_WrFL(iris_157);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_yvsel = 0x2;
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_yhsel = 0xE;
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_hth_on = 0x1;
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_hth_sel = 0x2;
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_hth = 0x40;
#else
	VPORT_RdFL(iris_160);
	VPORT_Wr05(iris_160, reg_2dcomb_ycrdc_yvsel, 0x2, reg_2dcomb_ycrdc_yhsel, 0xE, reg_2dcomb_ycrdc_hth_on, 0x1, reg_2dcomb_ycrdc_hth_sel, 0x2, reg_2dcomb_ycrdc_hth, 0x40);
	VPORT_WrFL(iris_160);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_lrtype = 0x0;
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_dsel = 0x0;
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_method = 0x0;
#else
	VPORT_RdFL(iris_158);
	VPORT_Wr03(iris_158, reg_2dcomb_ycrdc_lrtype, 0x0, reg_2dcomb_ycrdc_dsel, 0x0, reg_2dcomb_ycrdc_method, 0x0);
	VPORT_WrFL(iris_158);
#endif

	if(ycrdc_lcr_on)
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_158.reg_2dcomb_ycrdc_lcr_on = 0x7; //ON/OFF : ON=7, OFF=0
#else
		VPORT_RdFL(iris_158);
		VPORT_Wr01(iris_158, reg_2dcomb_ycrdc_lcr_on, 0x7);
		VPORT_WrFL(iris_158);
#endif
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_158.reg_2dcomb_ycrdc_lcr_on = 0x0; //ON/OFF : ON=7, OFF=0
#else
		VPORT_RdFL(iris_158);
		VPORT_Wr01(iris_158, reg_2dcomb_ycrdc_lcr_on, 0x0);
		VPORT_WrFL(iris_158);
#endif
	}
}


void CVD_L8Bx_Program_Color_System_Main_cdto_only(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_014.reg_colour_mode	= pSet_system_t->reg_colour_mode;
	gVportRegBx->iris_014.reg_vline_625		= pSet_system_t->reg_vline_625;
	gVportRegBx->iris_014.reg_hpixel		= pSet_system_t->reg_hpixel;
#else
	VPORT_RdFL(iris_014);
	VPORT_Wr03(iris_014, reg_colour_mode, pSet_system_t->reg_colour_mode, reg_vline_625, pSet_system_t->reg_vline_625, reg_hpixel, pSet_system_t->reg_hpixel);
	VPORT_WrFL(iris_014);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_015.reg_ped				= pSet_system_t->reg_ped;
#else
	VPORT_RdFL(iris_015);
	VPORT_Wr01(iris_015, reg_ped, pSet_system_t->reg_ped);
	VPORT_WrFL(iris_015);
#endif

#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug
	//gVportRegBx->iris_097.reg_cagc_gate_start = pSet_system_t->reg_cagc_gate_start;
	_iow(&gVportRegBx->iris_097, 8, 24, pSet_system_t->reg_cagc_gate_start);	//reg_cagc_gate_start
	//gVportRegBx->iris_097.reg_cagc_gate_end = pSet_system_t->reg_cagc_gate_end;
	_iow(&gVportRegBx->iris_097, 8, 16, pSet_system_t->reg_cagc_gate_end);		//reg_cagc_gate_end
#else
	VPORT_RdFL(iris_097);
	VPORT_Wr02(iris_097, reg_cagc_gate_start, pSet_system_t->reg_cagc_gate_start, reg_cagc_gate_end, pSet_system_t->reg_cagc_gate_end);
	VPORT_WrFL(iris_097);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_030.reg_cdto_inc = pSet_system_t->reg_cdto_inc;
#else
	VPORT_RdFL(iris_030);
	VPORT_Wr01(iris_030, reg_cdto_inc, pSet_system_t->reg_cdto_inc);
	VPORT_WrFL(iris_030);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_066.reg_md_noise_th = pSet_system_t->reg_md_noise_th;
#else
	VPORT_RdFL(iris_066);
	VPORT_Wr01(iris_066, reg_md_noise_th, pSet_system_t->reg_md_noise_th);
	VPORT_WrFL(iris_066);
#endif

}
void CVD_L8Bx_Program_Color_System_Main_Multi(CVD_SET_SYSTEM_MAIN_T *pSet_system_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_mif_gmua_001.reg_comb3_buffer_size = pSet_system_t->reg_comb3_buffer_size;
#else
	VPORT_RdFL(iris_mif_gmua_001);
	VPORT_Wr01(iris_mif_gmua_001, reg_comb3_buffer_size, pSet_system_t->reg_comb3_buffer_size);
	VPORT_WrFL(iris_mif_gmua_001);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_mif_gmua_002.reg_fld1_init_rd_pel = pSet_system_t->reg_fld1_init_rd_pel;
#else
	VPORT_RdFL(iris_mif_gmua_002);
	VPORT_Wr01(iris_mif_gmua_002, reg_fld1_init_rd_pel, pSet_system_t->reg_fld1_init_rd_pel);
	VPORT_WrFL(iris_mif_gmua_002);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_mif_gmua_003.reg_fld2_init_rd_pel = pSet_system_t->reg_fld2_init_rd_pel;
#else
	VPORT_RdFL(iris_mif_gmua_003);
	VPORT_Wr01(iris_mif_gmua_003, reg_fld2_init_rd_pel, pSet_system_t->reg_fld2_init_rd_pel);
	VPORT_WrFL(iris_mif_gmua_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_mif_gmua_004.reg_fld3_init_rd_pel = pSet_system_t->reg_fld3_init_rd_pel;
#else
	VPORT_RdFL(iris_mif_gmua_004);
	VPORT_Wr01(iris_mif_gmua_004, reg_fld3_init_rd_pel, pSet_system_t->reg_fld3_init_rd_pel);
	VPORT_WrFL(iris_mif_gmua_004);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_mif_gmua_005.reg_fld4_init_rd_pel = pSet_system_t->reg_fld4_init_rd_pel;
#else
	VPORT_RdFL(iris_mif_gmua_005);
	VPORT_Wr01(iris_mif_gmua_005, reg_fld4_init_rd_pel, pSet_system_t->reg_fld4_init_rd_pel);
	VPORT_WrFL(iris_mif_gmua_005);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_099.reg_cvd1_clampagc_on = pSet_system_t->reg_cvd1_clampagc_on; // from KIM.MIN
#else
	VPORT_RdFL(iris_099);
	VPORT_Wr01(iris_099, reg_cvd1_clampagc_on, pSet_system_t->reg_cvd1_clampagc_on);
	VPORT_WrFL(iris_099);
#endif

	// by Kim.min 2010/06/03


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_014.reg_colour_mode	= pSet_system_t->reg_colour_mode;
	gVportRegBx->iris_014.reg_vline_625		= pSet_system_t->reg_vline_625;
	gVportRegBx->iris_014.reg_hpixel		= pSet_system_t->reg_hpixel;
#else
	VPORT_RdFL(iris_014);
	VPORT_Wr03(iris_014, reg_colour_mode, pSet_system_t->reg_colour_mode, reg_vline_625, pSet_system_t->reg_vline_625, reg_hpixel, pSet_system_t->reg_hpixel);
	VPORT_WrFL(iris_014);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_015.reg_ped				= pSet_system_t->reg_ped;
#else
	VPORT_RdFL(iris_015);
	VPORT_Wr01(iris_015, reg_ped, pSet_system_t->reg_ped);
	VPORT_WrFL(iris_015);
#endif

	//added 110411 by kim.min
	VPORT_RdFL(iris_178);
	VPORT_Wr01(iris_178, reg_adc_blank_level, pSet_system_t->reg_adc_blank_level);
	VPORT_WrFL(iris_178);


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_016.reg_hagc_en= pSet_system_t->reg_hagc_en; // kim.min 0615
	gVportRegBx->iris_016.reg_cagc_en = pSet_system_t->reg_cagc_en;
	gVportRegBx->iris_016.reg_agc_half_en = pSet_system_t->reg_agc_half_en;	//Modified Value to '0' for fast agc
	gVportRegBx->iris_016.reg_dagc_en = pSet_system_t->reg_dagc_en;
	gVportRegBx->iris_016.reg_dc_clamp_mode = pSet_system_t->reg_dc_clamp_mode;
	gVportRegBx->iris_016.reg_mv_hagc_mode = pSet_system_t->reg_mv_hagc_mode;
	gVportRegBx->iris_016.reg_hagc_field_mode = pSet_system_t->reg_hagc_field_mode;
#else
	VPORT_RdFL(iris_016);
	VPORT_Wr07(iris_016, reg_hagc_en, pSet_system_t->reg_hagc_en, reg_cagc_en, pSet_system_t->reg_cagc_en, reg_agc_half_en, pSet_system_t->reg_agc_half_en, reg_dagc_en, pSet_system_t->reg_dagc_en, reg_dc_clamp_mode, pSet_system_t->reg_dc_clamp_mode, reg_mv_hagc_mode, pSet_system_t->reg_mv_hagc_mode, reg_hagc_field_mode, pSet_system_t->reg_hagc_field_mode);
	VPORT_WrFL(iris_016);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_015.reg_chroma_bw_lo = pSet_system_t->reg_chroma_bw_lo;
#else
	VPORT_RdFL(iris_015);
	VPORT_Wr01(iris_015, reg_chroma_bw_lo, pSet_system_t->reg_chroma_bw_lo);
	VPORT_WrFL(iris_015);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_017.reg_adaptive_mode = pSet_system_t->reg_adaptive_mode;
	gVportRegBx->iris_017.reg_adaptive_3dcomb_mode = pSet_system_t->reg_adaptive_3dcomb_mode;
#else
	VPORT_RdFL(iris_017);
	VPORT_Wr02(iris_017, reg_adaptive_mode, pSet_system_t->reg_adaptive_mode, reg_adaptive_3dcomb_mode, pSet_system_t->reg_adaptive_3dcomb_mode);
	VPORT_WrFL(iris_017);
#endif

#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug
	//gVportRegBx->iris_018.reg_hagc = pSet_system_t->reg_hagc;
	_iow(&gVportRegBx->iris_018, 8, 8, pSet_system_t->reg_hagc); //reg_hagc
#else
	VPORT_RdFL(iris_018);
	VPORT_Wr01(iris_018, reg_hagc, pSet_system_t->reg_hagc);
	VPORT_WrFL(iris_018);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_020.reg_adc_updn_swap = pSet_system_t->reg_adc_updn_swap;
#else
	VPORT_RdFL(iris_020);
	VPORT_Wr01(iris_020, reg_adc_updn_swap, pSet_system_t->reg_adc_updn_swap);
	VPORT_WrFL(iris_020);
#endif


	//yc_delay setting is moved to CVD PQ
	//gVportRegBx->iris_021.reg_yc_delay = pSet_system_t->reg_yc_delay;

	// need Reg debug
	//
#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug
	//gVportRegBx->iris_097.reg_cagc_gate_start = pSet_system_t->reg_cagc_gate_start;
	_iow(&gVportRegBx->iris_097, 8, 24, pSet_system_t->reg_cagc_gate_start);	//reg_cagc_gate_start
	//gVportRegBx->iris_097.reg_cagc_gate_end = pSet_system_t->reg_cagc_gate_end;
	_iow(&gVportRegBx->iris_097, 8, 16, pSet_system_t->reg_cagc_gate_end);		//reg_cagc_gate_end
#else
	VPORT_RdFL(iris_097);
	VPORT_Wr02(iris_097, reg_cagc_gate_start, pSet_system_t->reg_cagc_gate_start, reg_cagc_gate_end, pSet_system_t->reg_cagc_gate_end);
	VPORT_WrFL(iris_097);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_030.reg_cdto_inc = pSet_system_t->reg_cdto_inc;
#else
	VPORT_RdFL(iris_030);
	VPORT_Wr01(iris_030, reg_cdto_inc, pSet_system_t->reg_cdto_inc);
	VPORT_WrFL(iris_030);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_031.reg_hdto_inc = pSet_system_t->reg_hdto_inc;
#else
	VPORT_RdFL(iris_031);
	VPORT_Wr01(iris_031, reg_hdto_inc, pSet_system_t->reg_hdto_inc);
	VPORT_WrFL(iris_031);
#endif


	// need Reg debug
	//	gVportRegBx->chbcvd_024.reg_hactive_start = pSet_system_t->reg_hactive_start;

#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->chbcvd_024.reg_hactive_width = pSet_system_t->reg_hactive_width;
	_iow(&gVportRegBx->iris_037, 8, 0, pSet_system_t->reg_hactive_width);	//reg_hactive_width
#else
	VPORT_RdFL(iris_037);
	VPORT_Wr01(iris_037, reg_hactive_width, pSet_system_t->reg_hactive_width);
	VPORT_WrFL(iris_037);
#endif

	if(gVportRegBx->iris_147.reg_cres_clksel > 0)	// 54MHz sample clock
	{
#if VPORT_REG_DIRECT_ACCESS
		_iow(&gVportRegBx->iris_037, 8, 8, (pSet_system_t->reg_hactive_start_54M));	//reg_hactive_start
#else
		VPORT_RdFL(iris_037);
#if	HSTART_SHIFT_DUE_TO_DE_CROP_WORKAROUND
		VPORT_Wr01(iris_037, reg_hactive_start, (pSet_system_t->reg_hactive_start_54M +2) );
#else
		VPORT_Wr01(iris_037, reg_hactive_start, pSet_system_t->reg_hactive_start_54M);
#endif
		VPORT_WrFL(iris_037);
#endif
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		_iow(&gVportRegBx->iris_037, 8, 8, pSet_system_t->reg_hactive_start);	//reg_hactive_start
#else
		VPORT_RdFL(iris_037);
#if	HSTART_SHIFT_DUE_TO_DE_CROP_WORKAROUND
		VPORT_Wr01(iris_037, reg_hactive_start, (pSet_system_t->reg_hactive_start +2));
#else
		VPORT_Wr01(iris_037, reg_hactive_start, pSet_system_t->reg_hactive_start);
#endif
		VPORT_WrFL(iris_037);
#endif
	}

#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug(ok)
	//gVportRegBx->iris_038.reg_vactive_start = pSet_system_t->reg_vactive_start;
	_iow(&gVportRegBx->iris_038, 8, 8, pSet_system_t->reg_vactive_start);
	//gVportRegBx->iris_038.reg_vactive_height = pSet_system_t->reg_vactive_height;
	_iow(&gVportRegBx->iris_038, 8, 0, pSet_system_t->reg_vactive_height);
#else
	VPORT_RdFL(iris_038);
	VPORT_Wr02(iris_038, reg_vactive_start, pSet_system_t->reg_vactive_start, reg_vactive_height, pSet_system_t->reg_vactive_height);
	VPORT_WrFL(iris_038);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_017.reg_ntsc443_mode = pSet_system_t->reg_ntsc443_mode;
#else
	VPORT_RdFL(iris_017);
	VPORT_Wr01(iris_017, reg_ntsc443_mode, pSet_system_t->reg_ntsc443_mode);
	VPORT_WrFL(iris_017);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_023.reg_cagc = pSet_system_t->reg_cagc;
#else
	VPORT_RdFL(iris_023);
	VPORT_Wr01(iris_023, reg_cagc, pSet_system_t->reg_cagc);
	VPORT_WrFL(iris_023);
#endif

#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug(ok)
	//gVportRegBx->iris_036.reg_burst_gate_end = pSet_system_t->reg_burst_gate_end;
	_iow(&gVportRegBx->iris_036, 8, 0, pSet_system_t->reg_burst_gate_end);
#else
	VPORT_RdFL(iris_036);
	VPORT_Wr01(iris_036, reg_burst_gate_end, pSet_system_t->reg_burst_gate_end);
	VPORT_WrFL(iris_036);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_046.reg_secam_ybw = pSet_system_t->reg_secam_ybw;
#else
	VPORT_RdFL(iris_046);
	VPORT_Wr01(iris_046, reg_secam_ybw, pSet_system_t->reg_secam_ybw);
	VPORT_WrFL(iris_046);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_047.reg_auto_secam_level = pSet_system_t->reg_auto_secam_level;
#else
	VPORT_RdFL(iris_047);
	VPORT_Wr01(iris_047, reg_auto_secam_level, pSet_system_t->reg_auto_secam_level);
	VPORT_WrFL(iris_047);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_048.reg_lose_chromalock_mode = pSet_system_t->reg_lose_chromalock_mode;
#else
	VPORT_RdFL(iris_048);
	VPORT_Wr01(iris_048, reg_lose_chromalock_mode, pSet_system_t->reg_lose_chromalock_mode);
	VPORT_WrFL(iris_048);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_062.reg_noise_th = pSet_system_t->reg_noise_th;
	gVportRegBx->iris_062.reg_noise_th_en = pSet_system_t->reg_noise_th_en;
	gVportRegBx->iris_062.reg_lowfreq_vdiff_gain = pSet_system_t->reg_lowfreq_vdiff_gain;
	gVportRegBx->iris_062.reg_chroma_vdiff_gain = pSet_system_t->reg_chroma_vdiff_gain;
	gVportRegBx->iris_062.reg_horiz_diff_ygain = pSet_system_t->reg_horiz_diff_ygain;
	gVportRegBx->iris_062.reg_horiz_diff_cgain = pSet_system_t->reg_horiz_diff_cgain;
#else
	VPORT_RdFL(iris_062);
	VPORT_Wr06(iris_062, reg_noise_th, pSet_system_t->reg_noise_th, reg_noise_th_en, pSet_system_t->reg_noise_th_en, reg_lowfreq_vdiff_gain, pSet_system_t->reg_lowfreq_vdiff_gain, reg_chroma_vdiff_gain, pSet_system_t->reg_chroma_vdiff_gain, reg_horiz_diff_ygain, pSet_system_t->reg_horiz_diff_ygain, reg_horiz_diff_cgain, pSet_system_t->reg_horiz_diff_cgain);
	VPORT_WrFL(iris_062);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_063.reg_y_noise_th_gain = pSet_system_t->reg_y_noise_th_gain;
	gVportRegBx->iris_063.reg_c_noise_th_gain = pSet_system_t->reg_c_noise_th_gain;
	gVportRegBx->iris_063.reg_burst_noise_th_gain = pSet_system_t->reg_burst_noise_th_gain;
	gVportRegBx->iris_063.reg_vadap_burst_noise_th_gain = pSet_system_t->reg_vadap_burst_noise_th_gain;
#else
	VPORT_RdFL(iris_063);
	VPORT_Wr04(iris_063, reg_y_noise_th_gain, pSet_system_t->reg_y_noise_th_gain, reg_c_noise_th_gain, pSet_system_t->reg_c_noise_th_gain, reg_burst_noise_th_gain, pSet_system_t->reg_burst_noise_th_gain, reg_vadap_burst_noise_th_gain, pSet_system_t->reg_vadap_burst_noise_th_gain);
	VPORT_WrFL(iris_063);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_064.reg_motion_mode = pSet_system_t->reg_motion_mode;
	gVportRegBx->iris_064.reg_adaptive_chroma_mode = pSet_system_t->reg_adaptive_chroma_mode;
#else
	VPORT_RdFL(iris_064);
	VPORT_Wr02(iris_064, reg_motion_mode, pSet_system_t->reg_motion_mode, reg_adaptive_chroma_mode, pSet_system_t->reg_adaptive_chroma_mode);
	VPORT_WrFL(iris_064);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_065.reg_comb2d_only = pSet_system_t->reg_comb2d_only;
	gVportRegBx->iris_065.reg_fb_sync = pSet_system_t->reg_fb_sync;
#else
	VPORT_RdFL(iris_065);
	VPORT_Wr02(iris_065, reg_comb2d_only, pSet_system_t->reg_comb2d_only, reg_fb_sync, pSet_system_t->reg_fb_sync);
	VPORT_WrFL(iris_065);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_066.reg_md_noise_th = pSet_system_t->reg_md_noise_th;
	gVportRegBx->iris_066.reg_md_noise_th_en = pSet_system_t->reg_md_noise_th_en;
#else
	VPORT_RdFL(iris_066);
	VPORT_Wr02(iris_066, reg_md_noise_th, pSet_system_t->reg_md_noise_th, reg_md_noise_th_en, pSet_system_t->reg_md_noise_th_en);
	VPORT_WrFL(iris_066);
#endif

#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug(OK)
	//gVportRegBx->iris_070.reg_vactive_md_start = pSet_system_t->reg_vactive_md_start;
	_iow(&gVportRegBx->iris_070, 8, 8, pSet_system_t->reg_vactive_md_start);
	//gVportRegBx->iris_070.reg_vactive_md_height = pSet_system_t->reg_vactive_md_height;
	_iow(&gVportRegBx->iris_070, 8, 0, pSet_system_t->reg_vactive_md_height);
#else
	VPORT_RdFL(iris_070);
	VPORT_Wr02(iris_070, reg_vactive_md_start, pSet_system_t->reg_vactive_md_start, reg_vactive_md_height, pSet_system_t->reg_vactive_md_height);
	VPORT_WrFL(iris_070);
#endif

#if VPORT_REG_DIRECT_ACCESS
	// need Reg debug
	//gVportRegBx->iris_091.reg_hactive_md_start = pSet_system_t->reg_hactive_md_start;
	_iow(&gVportRegBx->iris_091, 8, 24, pSet_system_t->reg_hactive_md_start);
	//gVportRegBx->iris_091.reg_hactive_md_width = pSet_system_t->reg_hactive_md_width;
	_iow(&gVportRegBx->iris_091, 8, 16, pSet_system_t->reg_hactive_md_width);
	//gVportRegBx->iris_091.reg_motion_config = pSet_system_t->reg_motion_config;
	_iow(&gVportRegBx->iris_091, 8, 8, pSet_system_t->reg_motion_config);
#else
	VPORT_RdFL(iris_091);
	VPORT_Wr03(iris_091, reg_hactive_md_start, pSet_system_t->reg_hactive_md_start, reg_hactive_md_width, pSet_system_t->reg_hactive_md_width, reg_motion_config, pSet_system_t->reg_motion_config);
	VPORT_WrFL(iris_091);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_092.reg_status_motion_mode = pSet_system_t->reg_status_motion_mode;
#else
	VPORT_RdFL(iris_092);
	VPORT_Wr01(iris_092, reg_status_motion_mode, pSet_system_t->reg_status_motion_mode);
	VPORT_WrFL(iris_092);
#endif


	// need Reg debug
	//	gVportRegBx->iris_093.reg_frame_motion_th = pSet_system_t->reg_frame_motion_th;
	//110127
	//	_iow(&gVportRegBx->iris_093, 8, 16, pSet_system_t->reg_frame_motion_th);

	// need Reg debug
	//	gVportRegBx->iris_088.reg_chroma_level = pSet_system_t->reg_chroma_level;
	//	gVportRegBx->iris_088.reg_hf_luma_chroma_offset = pSet_system_t->reg_hf_luma_chroma_offset;
	//	_iow(&gVportRegBx->iris_088, 8, 24, pSet_system_t->reg_chroma_level);
	//	_iow(&gVportRegBx->iris_088, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_lcr_on = pSet_system_t->reg_2dcomb_ycrdc_lcr_on;
#else
	VPORT_RdFL(iris_158);
	VPORT_Wr01(iris_158, reg_2dcomb_ycrdc_lcr_on, pSet_system_t->reg_2dcomb_ycrdc_lcr_on);
	VPORT_WrFL(iris_158);
#endif

#if 0	// A1 Only
	gVportRegBx->iris_161.reg_2dcomb_cyrdc_lcr_on = pSet_system_t->reg_2dcomb_cyrdc_lcr_on;
#endif

	// kim.min 0622
#if VPORT_REG_DIRECT_ACCESS
	_iow(&gVportRegBx->iris_095, 8, 0, pSet_system_t->reg_cordic_gate_end);
	_iow(&gVportRegBx->iris_095, 8, 8, pSet_system_t->reg_cordic_gate_start);
	_iow(&gVportRegBx->iris_095, 8, 24, pSet_system_t->reg_phase_offset_range);
#else
	VPORT_RdFL(iris_095);
	VPORT_Wr03(iris_095, reg_cordic_gate_end, pSet_system_t->reg_cordic_gate_end, reg_cordic_gate_start, pSet_system_t->reg_cordic_gate_start, reg_phase_offset_range, pSet_system_t->reg_phase_offset_range);
	VPORT_WrFL(iris_095);
#endif

	// kim.min 0716
	//	_iow(&gVportRegBx->iris_088, 8, 0, pSet_system_t->reg_tcomb_chroma_level);
	//	_iow(&gVportRegBx->iris_088, 8, 8, pSet_system_t->reg_hf_luma_chroma_offset);
	//	_iow(&gVportRegBx->iris_088, 8, 24, pSet_system_t->reg_chroma_level);


	//kim.min 1103
	// setting of below register is moved to CVD_L8Bx_Set_Output_Range()
	//	gVportRegBx->iris_166.reg_oadj_y_offo = pSet_system_t->reg_oadj_y_offo;
	//kim.min 0906
	// setting of below register is moved to CVD_L8Bx_Set_Output_Range()
	//	gVportRegBx->iris_167.reg_oadj_y_coeff = pSet_system_t->reg_oadj_y_coeff;

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_017.reg_pal60_mode = pSet_system_t->reg_pal60_mode;
#else
	VPORT_RdFL(iris_017);
	VPORT_Wr01(iris_017, reg_pal60_mode, pSet_system_t->reg_pal60_mode);
	VPORT_WrFL(iris_017);
#endif


	//kim.min 0920

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_028.reg_hstate_max = pSet_system_t->reg_hstate_max;	//Set to '5' for jitter issue(from default value '3')
#else
	VPORT_RdFL(iris_028);
	VPORT_Wr01(iris_028, reg_hstate_max, pSet_system_t->reg_hstate_max);
	VPORT_WrFL(iris_028);
#endif

	//gVportRegBx->iris_075.reg_dcrestore_accum_width = pSet_system_t->reg_dcrestore_accum_width;	//A1 Only

	// From Ok : SW workaround for 3D comb operation 2010/06/04
	// Should be removed from B0 Revision
#if 0
	mdelay(30);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_005.reg_swrst_irisyc = 1;
#else
	VPORT_RdFL(top_005);
	VPORT_Wr01(top_005, reg_swrst_irisyc, 1);
	VPORT_WrFL(top_005);
#endif

	mdelay(10); //0619

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_005.reg_swrst_irisyc = 0;
#else
	VPORT_RdFL(top_005);
	VPORT_Wr01(top_005, reg_swrst_irisyc, 0);
	VPORT_WrFL(top_005);
#endif

#endif

	//added 0212 for better 3dcomb operation on RF signal.

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_025.reg_hnon_std_threshold = pSet_system_t->reg_hnon_std_threshold;
#else
	VPORT_RdFL(iris_025);
	VPORT_Wr01(iris_025, reg_hnon_std_threshold, pSet_system_t->reg_hnon_std_threshold);
	VPORT_WrFL(iris_025);
#endif

	//added 110415 for Jo Jo Gunpo filed stream : no signal issue
	VPORT_RdFL(iris_079);
	VPORT_Wr01(iris_079, reg_vsync_signal_thresh, pSet_system_t->reg_vsync_signal_thresh);
	VPORT_WrFL(iris_079);
}

void CVD_L8Bx_Program_Color_System_CHB_NTSC(void)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_007.reg_swrst_chbcvd = 1;
#else
	VPORT_RdFL(top_007);
	VPORT_Wr01(top_007, reg_swrst_chbcvd, 1);
	VPORT_WrFL(top_007);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->chbcvd_001.chbreg_colour_mode = 0x0;
	gVportRegBx->chbcvd_001.chbreg_vline_625 = 0x0;
	gVportRegBx->chbcvd_001.chbreg_hpixel = 0x0;
#else
	VPORT_RdFL(chbcvd_001);
	VPORT_Wr03(chbcvd_001, chbreg_colour_mode, 0x0, chbreg_vline_625, 0x0, chbreg_hpixel, 0x0);
	VPORT_WrFL(chbcvd_001);
#endif

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->chbcvd_002.chbreg_ped = 0x1;
#else
	VPORT_RdFL(chbcvd_002);
	VPORT_Wr01(chbcvd_002, chbreg_ped, 0x1);
	VPORT_WrFL(chbcvd_002);
#endif


	//	gVportRegBx->chbcvd_003.chbreg_cagc_en = 0x1;

	//	gVportRegBx->chbcvd_002.chbreg_hagc_half_en = 0x1;		//check
	//	gVportRegBx->chbcvd_002.chbreg_dc_clamp_mode = 0x0;
	//	gVportRegBx->chbcvd_002.chbreg_hagc_field_mode = 0x0;
	//	gVportRegBx->chbcvd_002.chbreg_chroma_bw_lo = 0x0;

	//	gVportRegBx->chbcvd_004.chbreg_adaptive_mode = 0x0;

#if VPORT_REG_DIRECT_ACCESS
	_iow(&gVportRegBx->chbcvd_004, 8, 8, 0xdd); //chbreg_hagc
#else
	VPORT_RdFL(chbcvd_004);
	VPORT_Wr01(chbcvd_004, chbreg_hagc, 0xdd);
	VPORT_WrFL(chbcvd_004);
#endif

#if VPORT_REG_DIRECT_ACCESS
	_iow(&gVportRegBx->chbcvd_010, 8, 16, 0xa8); // chbreg_cagc
#else
	VPORT_RdFL(chbcvd_010);
	VPORT_Wr01(chbcvd_010, chbreg_cagc, 0xa8);
	VPORT_WrFL(chbcvd_010);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->chbcvd_017.chbreg_cdto_inc = 0x21f07c1f;
#else
	VPORT_RdFL(chbcvd_017);
	VPORT_Wr01(chbcvd_017, chbreg_cdto_inc, 0x21f07c1f);
	VPORT_WrFL(chbcvd_017);
#endif

#if VPORT_REG_DIRECT_ACCESS
	_iow(&gVportRegBx->chbcvd_022, 8, 8, 0x00); // chbreg_dr_freq
	_iow(&gVportRegBx->chbcvd_022, 8, 0, 0x00); // chbreg_db_freq
#else
	VPORT_RdFL(chbcvd_022);
	VPORT_Wr02(chbcvd_022, chbreg_dr_freq, 0x00, chbreg_db_freq, 0x00);
	VPORT_WrFL(chbcvd_022);
#endif

#if VPORT_REG_DIRECT_ACCESS
	_iow(&gVportRegBx->chbcvd_024, 8, 8, 0x82); // chbreg_hactive_start
	_iow(&gVportRegBx->chbcvd_024, 8, 0, 0x50); // chbreg_hactive_width
#else
	VPORT_RdFL(chbcvd_024);
	VPORT_Wr02(chbcvd_024, chbreg_hactive_start, 0x82, chbreg_hactive_width, 0x50);
	VPORT_WrFL(chbcvd_024);
#endif

#if VPORT_REG_DIRECT_ACCESS
	_iow(&gVportRegBx->chbcvd_027, 8, 8, 0x22); // chbreg_vactive_start
	_iow(&gVportRegBx->chbcvd_027, 8, 0, 0x61); // chbreg_vactive_height
#else
	VPORT_RdFL(chbcvd_027);
	VPORT_Wr02(chbcvd_027, chbreg_vactive_start, 0x22, chbreg_vactive_height, 0x61);
	VPORT_WrFL(chbcvd_027);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->chbcvd_048.chbreg_lose_chromalock_mode = 0x3;
#else
	VPORT_RdFL(chbcvd_048);
	VPORT_Wr01(chbcvd_048, chbreg_lose_chromalock_mode, 0x3);
	VPORT_WrFL(chbcvd_048);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->chbcvd_056.chbreg_cagc_tc_ibig = 0x4;
#else
	VPORT_RdFL(chbcvd_056);
	VPORT_Wr01(chbcvd_056, chbreg_cagc_tc_ibig, 0x4);
	VPORT_WrFL(chbcvd_056);
#endif


	//add by ok. 20110317
	VPORT_RdFL(chbcvd_057);
	VPORT_Wr02(chbcvd_057, chbreg_dcrestore_no_bad_bp, 0x1, chbreg_dcrestore_kill_enable, 0x1);
	VPORT_WrFL(chbcvd_057);

	VPORT_RdFL(chbcvd_034);
	VPORT_Wr03(chbcvd_034, chbreg_cpump_auto_stip_unlocked, 0x1, chbreg_cpump_auto_stip_no_signal, 0x1, chbreg_cpump_auto_stip_noisy, 0x1);
	VPORT_WrFL(chbcvd_034);

	VPORT_RdFL(chbcvd_036);
	VPORT_Wr04(chbcvd_036, chbreg_cpump_up_max, 0x7f, chbreg_cpump_diff_signal_only, 0x1, chbreg_cpump_up_diff_max, 0x7f, chbreg_cpump_diff_noisy_only, 0x1);
	VPORT_WrFL(chbcvd_036);

	VPORT_RdFL(chbcvd_037);
	VPORT_Wr01(chbcvd_037, chbreg_cpump_dn_diff_max, 0x0);
	VPORT_WrFL(chbcvd_037);

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

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_007.reg_swrst_chbcvd = 0;
#else
	VPORT_RdFL(top_007);
	VPORT_Wr01(top_007, reg_swrst_chbcvd, 0);
	VPORT_WrFL(top_007);
#endif

}
void CVD_L8Bx_ALL_Power_Down(void)
{

	/*CVBS ADC Power Down Setting */

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_002.cvbs_pdbm = 0x0;
	gVportRegBx->cvbsafe_002.cvbs1_pdb = 0x0;
	gVportRegBx->cvbsafe_002.cvbs2_pdb = 0x0;
#else
	VPORT_RdFL(cvbsafe_002);
	VPORT_Wr03(cvbsafe_002, cvbs_pdbm, 0x0, cvbs1_pdb, 0x0, cvbs2_pdb, 0x0);
	VPORT_WrFL(cvbsafe_002);
#endif

	/* VDAC & BUFFER Power Down Setting */

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_001.vdac_pdb = 0x0;
	gVportRegBx->cvbsafe_001.vdicnt = 0x3; //dws
	gVportRegBx->cvbsafe_001.buf_pdb1 = 0x0;
	gVportRegBx->cvbsafe_001.buf_pdb2 = 0x0;
#else
	VPORT_RdFL(cvbsafe_001);
	VPORT_Wr04(cvbsafe_001, vdac_pdb, 0x0, vdicnt, 0x3, buf_pdb1, 0x0, buf_pdb2, 0x0);
	VPORT_WrFL(cvbsafe_001);
#endif

	/* AFE3CH Power Down Setting */

#if 0
#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_pdb_001.afe3ch_biaspdb = 0x0;
#else
	VPORT_RdFL(cvbsafe_pdb_001);
	VPORT_Wr01(cvbsafe_pdb_001, afe3ch_biaspdb, 0x0);
	VPORT_WrFL(cvbsafe_pdb_001);
#endif


	/* LLPLL PDB */

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_003.reg_cvbspll_pdb = 0;
	gVportRegBx->top_003.reg_ch3pll_pdb = 0;
#else
	VPORT_RdFL(top_003);
	VPORT_Wr02(top_003, reg_cvbspll_pdb, 0, reg_ch3pll_pdb, 0);
	VPORT_WrFL(top_003);
#endif

	/* LLPLL/CST SWRST & 3CH_DIG SWRST */

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_008.reg_swrst_llpll = 0x1;
	gVportRegBx->top_008.reg_swrst_ch3pix = 0x1;
#else
	VPORT_RdFL(top_008);
	VPORT_Wr02(top_008, reg_swrst_llpll, 0x1, reg_swrst_ch3pix, 0x1);
	VPORT_WrFL(top_008);
#endif
#endif

}

void CVD_L8Bx_SW_Reset(LX_AFE_CVD_SELECT_T select_main_sub)
{
	if(select_main_sub == LX_CVD_MAIN)
	{
		// SET SW reset registers to '1'
		//		gVportRegBx->reg_swrst_exta.reg_swrst_exta = 1;
		//		gVportRegBx->reg_swrst_exta.reg_swrst_extb = 1;


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_005.reg_swrst_f81 = 1;
		gVportRegBx->top_005.reg_swrst_irisin = 1;
		gVportRegBx->top_005.reg_swrst_irisyc = 1;
#else
		VPORT_RdFL(top_005);
		VPORT_Wr03(top_005, reg_swrst_f81, 1, reg_swrst_irisin, 1, reg_swrst_irisyc, 1);
		VPORT_WrFL(top_005);
#endif

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_006.reg_swrst_irisrm = 1;
		gVportRegBx->top_006.reg_swrst_iriswm = 1;
#else
		VPORT_RdFL(top_006);
		VPORT_Wr02(top_006, reg_swrst_irisrm, 1, reg_swrst_iriswm, 1);
		VPORT_WrFL(top_006);
#endif

		//from cvd_test.cmm

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_008.reg_swrst_llpll = 1;
		gVportRegBx->top_008.reg_swrst_ch3pix = 1;
#else
		VPORT_RdFL(top_008);
		VPORT_Wr02(top_008, reg_swrst_llpll, 1, reg_swrst_ch3pix, 1);
		VPORT_WrFL(top_008);
#endif

		// dws added
		mdelay(10); //0809
		//gVportRegBx->iris_065.reg_lbadrgen_rst = 1;

		//Clear SW Reset Registers to '0'
		//		gVportRegBx->reg_swrst_exta.reg_swrst_exta = 0;
		//		gVportRegBx->reg_swrst_exta.reg_swrst_extb = 0;


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_005.reg_swrst_f81 = 0;
		gVportRegBx->top_005.reg_swrst_irisin = 0;
		gVportRegBx->top_005.reg_swrst_irisyc = 0;
#else
		VPORT_RdFL(top_005);
		VPORT_Wr03(top_005, reg_swrst_f81, 0, reg_swrst_irisin, 0, reg_swrst_irisyc, 0);
		VPORT_WrFL(top_005);
#endif

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_006.reg_swrst_irisrm = 0;
		gVportRegBx->top_006.reg_swrst_iriswm = 0;
#else
		VPORT_RdFL(top_006);
		VPORT_Wr02(top_006, reg_swrst_irisrm, 0, reg_swrst_iriswm, 0);
		VPORT_WrFL(top_006);
#endif

		//from cvd_test.cmm

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_008.reg_swrst_llpll = 0;
		gVportRegBx->top_008.reg_swrst_ch3pix = 0;
#else
		VPORT_RdFL(top_008);
		VPORT_Wr02(top_008, reg_swrst_llpll, 0, reg_swrst_ch3pix, 0);
		VPORT_WrFL(top_008);
#endif

		// dws added
		//gVportRegBx->iris_065.reg_lbadrgen_rst = 0;
	}
	else
	{
		// SET SW reset registers to '1'

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->chbcvd_003.chbreg_lbadrgen_rst	= 1;
#else
		VPORT_RdFL(chbcvd_003);
		VPORT_Wr01(chbcvd_003, chbreg_lbadrgen_rst, 1);
		VPORT_WrFL(chbcvd_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_007.reg_swrst_chbcvd = 1;
#else
		VPORT_RdFL(top_007);
		VPORT_Wr01(top_007, reg_swrst_chbcvd, 1);
		VPORT_WrFL(top_007);
#endif


		//Clear SW Reset Registers to '0'

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->chbcvd_003.chbreg_lbadrgen_rst	= 0;
#else
		VPORT_RdFL(chbcvd_003);
		VPORT_Wr01(chbcvd_003, chbreg_lbadrgen_rst, 0);
		VPORT_WrFL(chbcvd_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_007.reg_swrst_chbcvd = 0;
#else
		VPORT_RdFL(top_007);
		VPORT_Wr01(top_007, reg_swrst_chbcvd, 0);
		VPORT_WrFL(top_007);
#endif

	}
}



// BOOLEAN PowerOnOff
// TRUE : Power Down
// FALSE : Power Up
void CVD_L8Bx_Power_Down(LX_AFE_CVD_SELECT_T select_main_sub, BOOLEAN PowerOnOFF)
{
	if(select_main_sub == LX_CVD_MAIN)
	{
		//if(PowerOnOFF==TRUE)
		if(PowerOnOFF==FALSE) // from cvd_test.cmm
		{
			// from cvd_test.cmm

#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->cvbsafe_010.dr3p_m = 0x0;
			gVportRegBx->cvbsafe_010.dr3p_npc = 0x6;
			gVportRegBx->cvbsafe_010.dr3p_nsc = 0x3;
			gVportRegBx->cvbsafe_010.dr3p_od_cvdclk = 0x3;
			gVportRegBx->cvbsafe_010.dr3p_od_recclk = 0x5;
			gVportRegBx->cvbsafe_010.dr3p_ps23c = 0x0;
			gVportRegBx->cvbsafe_010.dr3p_cih = 0x2;
#else
			VPORT_RdFL(cvbsafe_010);
			VPORT_Wr07(cvbsafe_010, dr3p_m, 0x0, dr3p_npc, 0x6, dr3p_nsc, 0x3, dr3p_od_cvdclk, 0x3, dr3p_od_recclk, 0x5, dr3p_ps23c, 0x0, dr3p_cih, 0x2);
			VPORT_WrFL(cvbsafe_010);
#endif

#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->top_003.reg_cvbspll_pdb = 0x1;
#else
			VPORT_RdFL(top_003);
			VPORT_Wr01(top_003, reg_cvbspll_pdb, 0x1);
			VPORT_WrFL(top_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->cvbsafe_002.cvbs_pdbm = 0x1;
			gVportRegBx->cvbsafe_002.cvbs1_pdb = 0x1;
			gVportRegBx->cvbsafe_002.cvbs2_pdb = 0x0;	// CVBS block for Chroma
#else
			VPORT_RdFL(cvbsafe_002);
			VPORT_Wr03(cvbsafe_002, cvbs_pdbm, 0x1, cvbs1_pdb, 0x1, cvbs2_pdb, 0x0);
			VPORT_WrFL(cvbsafe_002);
#endif

		}
		else
		{

#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->top_003.reg_cvbspll_pdb = 0;
#else
			VPORT_RdFL(top_003);
			VPORT_Wr01(top_003, reg_cvbspll_pdb, 0);
			VPORT_WrFL(top_003);
#endif


#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->cvbsafe_002.cvbs_pdbm = 0;
			gVportRegBx->cvbsafe_002.cvbs1_pdb = 0;
			gVportRegBx->cvbsafe_002.cvbs2_pdb = 0;
#else
			VPORT_RdFL(cvbsafe_002);
			VPORT_Wr03(cvbsafe_002, cvbs_pdbm, 0, cvbs1_pdb, 0, cvbs2_pdb, 0);
			VPORT_WrFL(cvbsafe_002);
#endif

		}
	}
	else
	{
		//if(PowerOnOFF==TRUE)
		if(PowerOnOFF==FALSE)// from cvd_test.cmm
		{

#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->cvbsafe_007.cb_lpf = 0x1;
			gVportRegBx->cvbsafe_007.cb_pdb = 0x1;
			gVportRegBx->cvbsafe_007.cb_icon = 0x2;
			gVportRegBx->cvbsafe_007.cb_cp = 0x1;
			gVportRegBx->cvbsafe_007.cb_selref = 0x2;
			gVportRegBx->cvbsafe_007.cb_dcsel = 0x2;
			gVportRegBx->cvbsafe_007.cb_cnt = 0x1;
			gVportRegBx->cvbsafe_007.cb_cns = 0x0;
#else
			VPORT_RdFL(cvbsafe_007);
			VPORT_Wr08(cvbsafe_007, cb_lpf, 0x1, cb_pdb, 0x1, cb_icon, 0x2, cb_cp, 0x1, cb_selref, 0x2, cb_dcsel, 0x2, cb_cnt, 0x1, cb_cns, 0x0);
			VPORT_WrFL(cvbsafe_007);
#endif
		}
		else
		{
#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->cvbsafe_007.cb_pdb = 0;
#else
			VPORT_RdFL(cvbsafe_007);
			VPORT_Wr01(cvbsafe_007, cb_pdb, 0);
			VPORT_WrFL(cvbsafe_007);
#endif
		}
	}
}

void CVD_L8Bx_Reg_Init(LX_AFE_CVD_SELECT_T select_main_sub)
{
	if(select_main_sub == LX_CVD_MAIN)
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_021.reg_blue_mode = 0x0;	// Disable Blue Mode
#else
		VPORT_RdFL(iris_021);
		VPORT_Wr01(iris_021, reg_blue_mode, 0x0);
		VPORT_WrFL(iris_021);
#endif


		// 0909 by kim.min(same setting with FPGA)

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->cvbsafe_004.cvbs_bw = 0x0;
#else
		VPORT_RdFL(cvbsafe_004);
		VPORT_Wr01(cvbsafe_004, cvbs_bw, 0x0);
		VPORT_WrFL(cvbsafe_004);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->cvbsafe_005.cvbs_lpf = 0x1;
		gVportRegBx->cvbsafe_005.cvbs_byp = 0x1;
#else
		VPORT_RdFL(cvbsafe_005);
		VPORT_Wr02(cvbsafe_005, cvbs_lpf, 0x1, cvbs_byp, 0x1);
		VPORT_WrFL(cvbsafe_005);
#endif

		// 1210 Invert Odd/Even Field on B0 Revision(H/W Change or DE Firmware Change?)
		// But This has problem on CC data slicing.
		//gVportRegBx->iris_044.reg_flip_field = 1;

		// Initial Register Setting For Scart Fast Blanking.

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->fastblank_001.reg_fb_2line_delay = 0x1;
		gVportRegBx->fastblank_001.reg_fb_blend_ratio = 0x08;
		gVportRegBx->fastblank_001.reg_fb3ch_delay = 0xC;
		gVportRegBx->fastblank_001.reg_fb_latency = 0x16;
#else
		VPORT_RdFL(fastblank_001);
		// Blend Ratio 0x08 is for test only
		//VPORT_Wr04(fastblank_001, reg_fb_2line_delay, 0x1, reg_fb_blend_ratio, 0x08, reg_fb3ch_delay, 0xC, reg_fb_latency, 0x16);
		VPORT_Wr04(fastblank_001, reg_fb_2line_delay, 0x1, reg_fb_blend_ratio, 0x10, reg_fb3ch_delay, 0xC, reg_fb_latency, 0x16);
		VPORT_WrFL(fastblank_001);
#endif

		//For Test, Mix CVBS & RGB signal
		//gVportRegBx->fastblank_001.reg_fb_blend_ratio = 0x10;

		// For Faster 3D Comb lock(kim.min 20110110)
		// But, This produced 3D-Comb Lock-Unlocking problem on NTSC RF Signal
		// Setting reg_hnon_std_threshold to 0x0c(0x06) solve lock-unlock problem(20110221)

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_025.reg_nstd_hysis = 0x3;
#else
		VPORT_RdFL(iris_025);
		VPORT_Wr01(iris_025, reg_nstd_hysis, 0x3);
		VPORT_WrFL(iris_025);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_065.reg_field_latency = 0x0;
#else
		VPORT_RdFL(iris_065);
		VPORT_Wr01(iris_065, reg_field_latency, 0x0);
		VPORT_WrFL(iris_065);
#endif

	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->chbcvd_007.chbreg_blue_mode = 0x0;	// Disable Blue Mode
#else
		VPORT_RdFL(chbcvd_007);
		VPORT_Wr01(chbcvd_007, chbreg_blue_mode, 0x0);
		VPORT_WrFL(chbcvd_007);
#endif

	}

	// reg init for memory. by SC Jung
	// added switching register control
	if(  gpCvdMemCfg->memory_base < 0x20000000 )
	{

		gVportRegBx->iris_mif_gmua_007.reg_gmau_cmd_base &= 0x0fffffff;

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_mif_gmua_007.reg_gmau_cmd_base = (0x0fffffff & gpCvdMemCfg->memory_base);
#else
		VPORT_RdFL(iris_mif_gmua_007);
		VPORT_Wr01(iris_mif_gmua_007, reg_gmau_cmd_base, (0x0fffffff & gpCvdMemCfg->memory_base));
		VPORT_WrFL(iris_mif_gmua_007);
#endif


	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_mif_gmua_007.reg_gmau_cmd_base = gpCvdMemCfg->memory_base;
#else
		VPORT_RdFL(iris_mif_gmua_007);
		VPORT_Wr01(iris_mif_gmua_007, reg_gmau_cmd_base, gpCvdMemCfg->memory_base);
		VPORT_WrFL(iris_mif_gmua_007);
#endif
	}

}


int CVD_L8Bx_Set_Source_Type(UINT32 arg)
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
//			AFE_PRINT("Composite input\n");

#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->iris_014.reg_yc_src = 0;	// select composite
#else
			VPORT_RdFL(iris_014);
			VPORT_Wr01(iris_014, reg_yc_src, 0);
			VPORT_WrFL(iris_014);
#endif


			AFE_PRINT("Input source = [%d]\n", cvd_input_info.cvbs_input_port);

			//gVportRegBx->top_002.reg_exta_sel = 0;	// from cvd_test.cmm
			//gVportRegBx->top_002.reg_extb_sel = 0;	// from cvd_test.cmm

			switch(cvd_input_info.cvbs_input_port)
			{
				case LX_AFE_CVBS_IN1:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x2;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x2);
					VPORT_WrFL(cvbsafe_003);
#endif

					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN2:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x3;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x3);
					VPORT_WrFL(cvbsafe_003);
#endif

					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN3:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x0;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x0);
					VPORT_WrFL(cvbsafe_003);
#endif

					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN4:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x1;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x1);
					VPORT_WrFL(cvbsafe_003);
#endif

					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN5:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x2;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x2);
					VPORT_WrFL(cvbsafe_003);
#endif

					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 1);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN6:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x3;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x3);
					VPORT_WrFL(cvbsafe_003);
#endif

					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 1);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN7:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x0;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x0);
					VPORT_WrFL(cvbsafe_003);
#endif

					CTOP_CTRL_RdFL(ctr61);
					CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 1);
					CTOP_CTRL_WrFL(ctr61);
					break;

				case LX_AFE_CVBS_IN8:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x1;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x1);
					VPORT_WrFL(cvbsafe_003);
#endif

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

#if VPORT_REG_DIRECT_ACCESS
			gVportRegBx->iris_014.reg_yc_src = 1;	// select y/c
#else
			VPORT_RdFL(iris_014);
			VPORT_Wr01(iris_014, reg_yc_src, 1);
			VPORT_WrFL(iris_014);
#endif

			CTOP_CTRL_RdFL(ctr61);
			CTOP_CTRL_Wr01(ctr61, cvbs_input_mux, 0);
			CTOP_CTRL_WrFL(ctr61);

			switch(cvd_input_info.cvbs_input_port)
			{
				case LX_AFE_CVBS_IN1:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x2;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x2);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				case LX_AFE_CVBS_IN2:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x3;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x3);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				case LX_AFE_CVBS_IN3:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x0;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x0);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				case LX_AFE_CVBS_IN4:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x1;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x1);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				default:
					break;
			}

			switch(cvd_input_info.chroma_input_port)
			{
				case LX_AFE_CVBS_IN5:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x3;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x3);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				case LX_AFE_CVBS_IN6:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x0;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x0);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				case LX_AFE_CVBS_IN7:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x2;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x2);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				case LX_AFE_CVBS_IN8:

#if VPORT_REG_DIRECT_ACCESS
					gVportRegBx->cvbsafe_003.cvbs1_insel = 0x1;
#else
					VPORT_RdFL(cvbsafe_003);
					VPORT_Wr01(cvbsafe_003, cvbs1_insel, 0x1);
					VPORT_WrFL(cvbsafe_003);
#endif

					break;

				default:
					break;
			}
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

int CVD_L8Bx_Set_Scart_Overlay(BOOLEAN arg)
{
	if(arg == TRUE)
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->fastblank_001.reg_fb_en = 0x1;
#else
		VPORT_RdFL(fastblank_001);
		VPORT_Wr01(fastblank_001, reg_fb_en, 0x1);
		VPORT_WrFL(fastblank_001);
#endif


		// Following Register settings were move to CVD_L8Bx_Reg_Init
		/*
		   gVportRegBx->fastblank_001.reg_fb_2line_delay = 0x1;
		   gVportRegBx->fastblank_001.reg_fb_blend_ratio = 0x8;
		   gVportRegBx->fastblank_001.reg_fb3ch_delay = 0xC;
		   gVportRegBx->fastblank_001.reg_fb_latency = 0x16;
		 */
	}
	else
	{

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->fastblank_001.reg_fb_en = 0x0;
#else
		VPORT_RdFL(fastblank_001);
		VPORT_Wr01(fastblank_001, reg_fb_en, 0x0);
		VPORT_WrFL(fastblank_001);
#endif

		//		gVportRegBx->fastblank_001.reg_fb_blend_ratio = 0x8;
	}

	return 0;
}

int CVD_L8Bx_Set_Scart_FB_En(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->fastblank_001.reg_fb_en;
#else
	VPORT_RdFL(fastblank_001);
	VPORT_Rd01(fastblank_001, reg_fb_en, ret);
	return ret;
#endif
}

UINT8 CVD_L8Bx_Get_FC_Flag(LX_AFE_CVD_SELECT_T select_main_sub)
{
	UINT8 cordic_freq_status;

	if(select_main_sub == LX_CVD_MAIN)
	{
#if VPORT_REG_DIRECT_ACCESS
		cordic_freq_status = (UINT8)((SINT8)gVportRegBx->iris_008.reg_status_cordic_freq + 0x80);
#else
		VPORT_RdFL(iris_008);
		VPORT_Rd01(iris_008, reg_status_cordic_freq, cordic_freq_status);
		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
#endif
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		cordic_freq_status = (UINT8)((SINT8)gVportRegBx->chbcvd_045.chbreg_status_cordic_freq + 0x80);
#else
		VPORT_RdFL(chbcvd_045);
		VPORT_Rd01(chbcvd_045, chbreg_status_cordic_freq, cordic_freq_status);
		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
#endif
	}

	if(cordic_freq_status > FC_MORE_THRESHOLD)
		return CVD_FC_MORE_FLAG;
	else if(cordic_freq_status < FC_LESS_THRESHOLD)
		return CVD_FC_LESS_FLAG;
	else
		return CVD_FC_SAME_FLAG;
}

UINT8 CVD_L8Bx_Get_Cordic_Freq(LX_AFE_CVD_SELECT_T select_main_sub)
{
	UINT8 cordic_freq_status;

	if(select_main_sub == LX_CVD_MAIN)
	{
#if VPORT_REG_DIRECT_ACCESS
		cordic_freq_status = (UINT8)((SINT8)gVportRegBx->iris_008.reg_status_cordic_freq + 0x80);
#else
		VPORT_RdFL(iris_008);
		VPORT_Rd01(iris_008, reg_status_cordic_freq, cordic_freq_status);
		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
#endif
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		cordic_freq_status = (UINT8)((SINT8)gVportRegBx->chbcvd_045.chbreg_status_cordic_freq + 0x80);
#else
		VPORT_RdFL(chbcvd_045);
		VPORT_Rd01(chbcvd_045, chbreg_status_cordic_freq, cordic_freq_status);
		cordic_freq_status = (UINT8)((SINT8)cordic_freq_status + 0x80);
#endif
	}

	return cordic_freq_status;
}

#if 0		// Not Used
void CVD_L8Bx_Program_Color_System_CHB(CVD_SET_SYSTEM_CHB_T set_system_t)
{
	//CHB CVD Power Down
	//gVportRegBx->cvbsafe_007.cb_pdb = set_system_t.CB_PDB;
	gVportRegBx->chbcvd_001.chbreg_hv_delay = set_system_t.chbreg_hv_delay;
	gVportRegBx->chbcvd_001.chbreg_hpixel = set_system_t.chbreg_hpixel;
	gVportRegBx->chbcvd_001.chbreg_vline_625		= set_system_t.chbreg_vline_625;
	gVportRegBx->chbcvd_001.chbreg_colour_mode	= set_system_t.chbreg_colour_mode;
	gVportRegBx->chbcvd_001.chbreg_ntsc443_mode	= set_system_t.chbreg_ntsc443_mode;

	gVportRegBx->chbcvd_002.chbreg_luma_notch_bw	= set_system_t.chbreg_luma_notch_bw;
	gVportRegBx->chbcvd_002.chbreg_chroma_bw_lo	= set_system_t.chbreg_chroma_bw_lo;
	gVportRegBx->chbcvd_002.chbreg_chroma_burst5or10	= set_system_t.chbreg_chroma_burst5or10;
	gVportRegBx->chbcvd_002.chbreg_ped				= set_system_t.chbreg_ped;

	// CHB CVD Reset
	//gVportRegBx->chbcvd_008.chbreg_lbadrgen_rst			= set_system_t.chbreg_lbadrgen_rst;

	gVportRegBx->chbcvd_004.chbreg_adaptive_mode	= set_system_t.chbreg_adaptive_mode;
	gVportRegBx->chbcvd_004.chbreg_adaptive_chroma_mode	= set_system_t.chbreg_adaptive_chroma_mode;
	gVportRegBx->chbcvd_004.chbreg_hagc	= set_system_t.chbreg_hagc;

	gVportRegBx->chbcvd_010.chbreg_cagc	= set_system_t.chbreg_cagc;

	gVportRegBx->chbcvd_017.chbreg_cdto_inc	= set_system_t.chbreg_cdto_inc;

	gVportRegBx->chbcvd_022.chbreg_dr_freq	= set_system_t.chbreg_dr_freq;
	gVportRegBx->chbcvd_022.chbreg_db_freq	= set_system_t.chbreg_db_freq;

	gVportRegBx->chbcvd_024.chbreg_hactive_start 	= set_system_t.chbreg_hactive_start;
	gVportRegBx->chbcvd_027.chbreg_vactive_start 	= set_system_t.chbreg_vactive_start;
	gVportRegBx->chbcvd_027.chbreg_vactive_height			= set_system_t.chbreg_vactive_height;
	gVportRegBx->chbcvd_047.chbreg_auto_secam_level		= set_system_t.chbreg_auto_secam_level;
	gVportRegBx->chbcvd_047.chbreg_palsw_level		= set_system_t.chbreg_palsw_level;
	gVportRegBx->chbcvd_048.chbreg_lose_chromalock_mode		= set_system_t.chbreg_lose_chromalock_mode;
	gVportRegBx->chbcvd_056.chbreg_cagc_tc_p = set_system_t.chbreg_cagc_tc_p;
	gVportRegBx->chbcvd_056.chbreg_cagc_tc_ibig = set_system_t.chbreg_cagc_tc_ibig;
	gVportRegBx->chbcvd_056.chbreg_cagc_tc_ismall = set_system_t.chbreg_cagc_tc_ismall;
	// CHB CVD Reset
	//gVportRegBx->top_007.reg_swrst_chbcvd = set_system_t.reg_swrst_chbcvd;

}
#endif

int CVD_L8Bx_Get_FB_Status(UINT32 arg)
{
	LX_AFE_SCART_MODE_T		scart_fb_mode;
	UINT8	scart_fb_state;

#if VPORT_REG_DIRECT_ACCESS
	scart_fb_state = gVportRegBx->fast_blank_status_001.reg_fb_state;
#else
	VPORT_RdFL(fast_blank_status_001);
	VPORT_Rd01(fast_blank_status_001, reg_fb_state, scart_fb_state);
#endif

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
int CVD_L8Bx_Get_Scart_AR(UINT32 arg)
{
	LX_AFE_SCART_AR_INFO_T	scart_ar_param;
	UINT8	sc1_sid1, sc1_sid2, sc2_sid1, sc2_sid2;

	if (copy_from_user(&scart_ar_param, (void __user *)arg, sizeof(LX_AFE_SCART_AR_INFO_T)))
	{
		return -EFAULT;
	}

	if(scart_ar_param.Scart_Id == LX_SCART_ID_1)
	{
#if VPORT_REG_DIRECT_ACCESS
		sc1_sid1 = gVportRegBx->scart_001.afe3ch_sc1_sid1;
		sc1_sid2 = gVportRegBx->scart_001.afe3ch_sc1_sid2;
#else
	VPORT_RdFL(scart_001);
	VPORT_Rd02(scart_001, afe3ch_sc1_sid1, sc1_sid1, afe3ch_sc1_sid2, sc1_sid2);
#endif
		if((sc1_sid2==0)&&(sc1_sid1==0))
			scart_ar_param.Scart_AR = LX_SCART_AR_INVALID;
		else if((sc1_sid2==1)&&(sc1_sid1==1))
			scart_ar_param.Scart_AR = LX_SCART_AR_4_3;
		else
			scart_ar_param.Scart_AR = LX_SCART_AR_16_9;
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		sc2_sid1 = gVportRegBx->scart_001.afe3ch_sc2_sid1;
		sc2_sid2 = gVportRegBx->scart_001.afe3ch_sc2_sid2;
#else
	VPORT_RdFL(scart_001);
	VPORT_Rd02(scart_001, afe3ch_sc2_sid1, sc2_sid1, afe3ch_sc2_sid2, sc2_sid2);
#endif
		if((sc2_sid2==0)&&(sc2_sid1==0))
			scart_ar_param.Scart_AR = LX_SCART_AR_INVALID;
		else if((sc2_sid2==1)&&(sc2_sid1==1))
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

void CVD_L8Bx_Print_Vport_Version(void)
{
	UINT32 vport_version = 0;

#if VPORT_REG_DIRECT_ACCESS
	vport_version = gVportRegBx->top_001.vport_ver;
#else
	VPORT_RdFL(top_001);
	VPORT_Rd01(top_001, vport_ver, vport_version);
#endif

	AFE_PRINT("Vport Version : [%x]\n", vport_version);

}

int CVD_L8Bx_Vport_Output(UINT32 arg)
{
	LX_AFE_VPORT_OUT_INFO_T	vport_output;

	if (copy_from_user(&vport_output, (void __user *)arg, sizeof(LX_AFE_VPORT_OUT_INFO_T)))
	{
		return -EFAULT;
	}

//	AFE_PRINT("Vport Output Select : output src=[%d], output ch=[%d]\n", vport_output.sel_out_src, vport_output.sel_out_ch);

	if(vport_output.sel_out_ch == LX_AFE_VPORT_OUT_A)
	{
		//gVportRegBx->reg_swrst_exta.reg_swrst_exta = 0;

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_002.reg_exta_sel = vport_output.sel_out_src;	// from cvd_test.cmm
#else
		VPORT_RdFL(top_002);
		VPORT_Wr01(top_002, reg_exta_sel, vport_output.sel_out_src);
		VPORT_WrFL(top_002);
#endif
	}
	else // CVI_B
	{
		//gVportRegBx->reg_swrst_exta.reg_swrst_extb = 0;

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_002.reg_extb_sel = vport_output.sel_out_src;	// from cvd_test.cmm
#else
		VPORT_RdFL(top_002);
		VPORT_Wr01(top_002, reg_extb_sel, vport_output.sel_out_src);
		VPORT_WrFL(top_002);
#endif
	}

	return 0;
}


int CVD_L8Bx_Vport_Reg_Read(UINT32 arg)
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

		recvAddr += (UINT32)gVportRegBx;

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

int CVD_L8Bx_Vport_Reg_Write(UINT32 arg)
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
		recvAddr += (UINT32)gVportRegBx;

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

int CVD_L8Bx_Get_No_Signal_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_001.reg_no_signal;
#else
	VPORT_RdFL(iris_001);
	VPORT_Rd01(iris_001, reg_no_signal, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_HLock_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_001.reg_hlock;
#else
	VPORT_RdFL(iris_001);
	VPORT_Rd01(iris_001, reg_hlock, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_VLock_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_001.reg_vlock;
#else
	VPORT_RdFL(iris_001);
	VPORT_Rd01(iris_001,reg_vlock, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_Vline_625_Flag(void)
{
	//return 1;
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_002.reg_vline_625_detected;
#else
	VPORT_RdFL(iris_002);
	VPORT_Rd01(iris_002, reg_vline_625_detected, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_PAL_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_002.reg_pal_detected;
#else
	VPORT_RdFL(iris_002);
	VPORT_Rd01(iris_002, reg_pal_detected, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_SECAM_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_002.reg_secam_detected;
#else
	VPORT_RdFL(iris_002);
	VPORT_Rd01(iris_002, reg_secam_detected, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_Chromalock_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_001.reg_chromalock;
#else
	VPORT_RdFL(iris_001);
	VPORT_Rd01(iris_001, reg_chromalock, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_Noise_Status(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_009.reg_status_noise;
#else
	VPORT_RdFL(iris_009);
	VPORT_Rd01(iris_009, reg_status_noise, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_NoBurst_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->iris_002.reg_noburst_detected;
#else
	VPORT_RdFL(iris_002);
	VPORT_Rd01(iris_002, reg_noburst_detected, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_CHB_No_Signal_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->chbcvd_039.chbreg_no_signal;
#else
	VPORT_RdFL(chbcvd_039);
	VPORT_Rd01(chbcvd_039, chbreg_no_signal, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_CHB_HLock_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->chbcvd_039.chbreg_hlock;
#else
	VPORT_RdFL(chbcvd_039);
	VPORT_Rd01(chbcvd_039, chbreg_hlock, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_CHB_VLock_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->chbcvd_039.chbreg_vlock;
#else
	VPORT_RdFL(chbcvd_039);
	VPORT_Rd01(chbcvd_039, chbreg_vlock, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_CHB_Chromalock_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->chbcvd_039.chbreg_chromalock;
#else
	VPORT_RdFL(chbcvd_039);
	VPORT_Rd01(chbcvd_039, chbreg_chromalock, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_CHB_PAL_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->chbcvd_040.chbreg_pal_detected;
#else
	VPORT_RdFL(chbcvd_040);
	VPORT_Rd01(chbcvd_040, chbreg_pal_detected, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_CHB_Vline_625_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->chbcvd_040.chbreg_vline_625_detected;
#else
	VPORT_RdFL(chbcvd_040);
	VPORT_Rd01(chbcvd_040, chbreg_vline_625_detected, ret);
	return ret;
#endif
}

int CVD_L8Bx_Get_CHB_SECAM_Flag(void)
{
	int ret;
#if VPORT_REG_DIRECT_ACCESS
	return gVportRegBx->chbcvd_040.chbreg_secam_detected;
#else
	VPORT_RdFL(chbcvd_040);
	VPORT_Rd01(chbcvd_040, chbreg_secam_detected, ret);
	return ret;
#endif
}

int CVD_L8Bx_Reset_irisyc(int enable)
{
	if(enable > 0)
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_005.reg_swrst_irisyc = 1;
#else
		VPORT_RdFL(top_005);
		VPORT_Wr01(top_005, reg_swrst_irisyc, 1);
		VPORT_WrFL(top_005);
#endif
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_005.reg_swrst_irisyc = 0;
#else
		VPORT_RdFL(top_005);
		VPORT_Wr01(top_005, reg_swrst_irisyc, 0);
		VPORT_WrFL(top_005);
#endif
	}

	return 0;
}

int CVD_L8Bx_Reset_hdct(int enable)
{
	if(enable >0)
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_027.reg_swrst_hdct = 1;
#else
		VPORT_RdFL(iris_027);
		VPORT_Wr01(iris_027, reg_swrst_hdct, 1);
		VPORT_WrFL(iris_027);
#endif
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_027.reg_swrst_hdct = 0;
#else
		VPORT_RdFL(iris_027);
		VPORT_Wr01(iris_027, reg_swrst_hdct, 0);
		VPORT_WrFL(iris_027);
#endif
	}

	return 0;
}

int CVD_L8Bx_Reset_cdct(int enable)
{
	if(enable >0)
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_027.reg_swrst_cdct = 1;
#else
		VPORT_RdFL(iris_027);
		VPORT_Wr01(iris_027, reg_swrst_cdct, 1);
		VPORT_WrFL(iris_027);
#endif
	}
	else
	{
#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->iris_027.reg_swrst_cdct = 0;
#else
		VPORT_RdFL(iris_027);
		VPORT_Wr01(iris_027, reg_swrst_cdct, 0);
		VPORT_WrFL(iris_027);
#endif
	}

	return 0;
}

void CVD_L8Bx_Set_AAF_Params(CVD_L8Bx_SET_AAF_T *pSet_aaf_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_235.reg_lpf_0_en		= pSet_aaf_t->reg_lpf_0_en;
#else
	VPORT_RdFL(iris_235);
	VPORT_Wr01(iris_235, reg_lpf_0_en, pSet_aaf_t->reg_lpf_0_en);
	VPORT_WrFL(iris_235);
#endif


#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_235.reg_lpf_tap_0_0	= pSet_aaf_t->reg_lpf_tap_0_0;
	_iow(&gVportRegBx->iris_235, 16, 0, pSet_aaf_t->reg_lpf_tap_0_0);
#else
	VPORT_RdFL(iris_235);
	VPORT_Wr01(iris_235, reg_lpf_tap_0_0, pSet_aaf_t->reg_lpf_tap_0_0);
	VPORT_WrFL(iris_235);
#endif



#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_236.reg_lpf_tap_0_1	= pSet_aaf_t->reg_lpf_tap_0_1;
	_iow(&gVportRegBx->iris_236, 16, 16, pSet_aaf_t->reg_lpf_tap_0_1);
#else
	VPORT_RdFL(iris_236);
	VPORT_Wr01(iris_236, reg_lpf_tap_0_1, pSet_aaf_t->reg_lpf_tap_0_1);
	VPORT_WrFL(iris_236);
#endif


#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_236.reg_lpf_tap_0_2	= pSet_aaf_t->reg_lpf_tap_0_2;
	_iow(&gVportRegBx->iris_236, 16, 0, pSet_aaf_t->reg_lpf_tap_0_2);
#else
	VPORT_RdFL(iris_236);
	VPORT_Wr01(iris_236, reg_lpf_tap_0_2, pSet_aaf_t->reg_lpf_tap_0_2);
	VPORT_WrFL(iris_236);
#endif


#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_237.reg_lpf_tap_0_3	= pSet_aaf_t->reg_lpf_tap_0_3;
	_iow(&gVportRegBx->iris_237, 16, 16, pSet_aaf_t->reg_lpf_tap_0_3);
#else
	VPORT_RdFL(iris_237);
	VPORT_Wr01(iris_237, reg_lpf_tap_0_3, pSet_aaf_t->reg_lpf_tap_0_3);
	VPORT_WrFL(iris_237);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_237.reg_lpf_tap_0_4	= pSet_aaf_t->reg_lpf_tap_0_4;
	_iow(&gVportRegBx->iris_237, 16, 0, pSet_aaf_t->reg_lpf_tap_0_4);
#else
	VPORT_RdFL(iris_237);
	VPORT_Wr01(iris_237, reg_lpf_tap_0_4, pSet_aaf_t->reg_lpf_tap_0_4);
	VPORT_WrFL(iris_237);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_238.reg_lpf_tap_0_5	= pSet_aaf_t->reg_lpf_tap_0_5;
	_iow(&gVportRegBx->iris_238, 16, 16, pSet_aaf_t->reg_lpf_tap_0_5);
#else
	VPORT_RdFL(iris_238);
	VPORT_Wr01(iris_238, reg_lpf_tap_0_5, pSet_aaf_t->reg_lpf_tap_0_5);
	VPORT_WrFL(iris_238);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_238.reg_lpf_tap_0_6	= pSet_aaf_t->reg_lpf_tap_0_6;
	_iow(&gVportRegBx->iris_238, 16, 0, pSet_aaf_t->reg_lpf_tap_0_6);
#else
	VPORT_RdFL(iris_238);
	VPORT_Wr01(iris_238, reg_lpf_tap_0_6, pSet_aaf_t->reg_lpf_tap_0_6);
	VPORT_WrFL(iris_238);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_239.reg_lpf_tap_0_7	= pSet_aaf_t->reg_lpf_tap_0_7;
	_iow(&gVportRegBx->iris_239, 16, 16, pSet_aaf_t->reg_lpf_tap_0_7);
#else
	VPORT_RdFL(iris_239);
	VPORT_Wr01(iris_239, reg_lpf_tap_0_7, pSet_aaf_t->reg_lpf_tap_0_7);
	VPORT_WrFL(iris_239);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_239.reg_lpf_tap_0_8	= pSet_aaf_t->reg_lpf_tap_0_8;
	_iow(&gVportRegBx->iris_239, 16, 0, pSet_aaf_t->reg_lpf_tap_0_8);
#else
	VPORT_RdFL(iris_239);
	VPORT_Wr01(iris_239, reg_lpf_tap_0_8, pSet_aaf_t->reg_lpf_tap_0_8);
	VPORT_WrFL(iris_239);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_240.reg_lpf_tap_0_9	= pSet_aaf_t->reg_lpf_tap_0_9;
	_iow(&gVportRegBx->iris_240, 16, 16, pSet_aaf_t->reg_lpf_tap_0_9);
#else
	VPORT_RdFL(iris_240);
	VPORT_Wr01(iris_240, reg_lpf_tap_0_9, pSet_aaf_t->reg_lpf_tap_0_9);
	VPORT_WrFL(iris_240);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_240.reg_lpf_tap_0_10	= pSet_aaf_t->reg_lpf_tap_0_10;
	_iow(&gVportRegBx->iris_240, 16, 0, pSet_aaf_t->reg_lpf_tap_0_10);
#else
	VPORT_RdFL(iris_240);
	VPORT_Wr01(iris_240, reg_lpf_tap_0_10, pSet_aaf_t->reg_lpf_tap_0_10);
	VPORT_WrFL(iris_240);
#endif



#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_241.reg_lpf_tap_0_11	= pSet_aaf_t->reg_lpf_tap_0_11;
	_iow(&gVportRegBx->iris_241, 16, 16, pSet_aaf_t->reg_lpf_tap_0_11);
#else
	VPORT_RdFL(iris_241);
	VPORT_Wr01(iris_241, reg_lpf_tap_0_11, pSet_aaf_t->reg_lpf_tap_0_11);
	VPORT_WrFL(iris_241);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_241.reg_lpf_tap_0_12	= pSet_aaf_t->reg_lpf_tap_0_12;
	_iow(&gVportRegBx->iris_241, 16, 0, pSet_aaf_t->reg_lpf_tap_0_12);
#else
	VPORT_RdFL(iris_241);
	VPORT_Wr01(iris_241, reg_lpf_tap_0_12, pSet_aaf_t->reg_lpf_tap_0_12);
	VPORT_WrFL(iris_241);
#endif


}

void CVD_L8Bx_Set_ADC_Params(CVD_L8Bx_SET_CRES54MHZ_T *pSet_cresampler_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_010.dr3p_m	=	pSet_cresampler_t->DR3P_M;
#else
	VPORT_RdFL(cvbsafe_010);
	VPORT_Wr01(cvbsafe_010, dr3p_m, pSet_cresampler_t->DR3P_M);
	VPORT_WrFL(cvbsafe_010);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_010.dr3p_npc	=	pSet_cresampler_t->DR3P_NPC;
#else
	VPORT_RdFL(cvbsafe_010);
	VPORT_Wr01(cvbsafe_010, dr3p_npc, pSet_cresampler_t->DR3P_NPC);
	VPORT_WrFL(cvbsafe_010);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_010.dr3p_nsc	=	pSet_cresampler_t->DR3P_NSC;
#else
	VPORT_RdFL(cvbsafe_010);
	VPORT_Wr01(cvbsafe_010, dr3p_nsc, pSet_cresampler_t->DR3P_NSC);
	VPORT_WrFL(cvbsafe_010);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_010.dr3p_od_cvdclk	=	pSet_cresampler_t->DR3P_OD_CVDCLK;
#else
	VPORT_RdFL(cvbsafe_010);
	VPORT_Wr01(cvbsafe_010, dr3p_od_cvdclk, pSet_cresampler_t->DR3P_OD_CVDCLK);
	VPORT_WrFL(cvbsafe_010);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_010.dr3p_od_recclk	=	pSet_cresampler_t->DR3P_OD_RECCLK;
#else
	VPORT_RdFL(cvbsafe_010);
	VPORT_Wr01(cvbsafe_010, dr3p_od_recclk, pSet_cresampler_t->DR3P_OD_RECCLK);
	VPORT_WrFL(cvbsafe_010);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_010.dr3p_ps23c	=	pSet_cresampler_t->DR3P_PS23C;
#else
	VPORT_RdFL(cvbsafe_010);
	VPORT_Wr01(cvbsafe_010, dr3p_ps23c, pSet_cresampler_t->DR3P_PS23C);
	VPORT_WrFL(cvbsafe_010);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->cvbsafe_010.dr3p_cih	=	pSet_cresampler_t->DR3P_CIH;
#else
	VPORT_RdFL(cvbsafe_010);
	VPORT_Wr01(cvbsafe_010, dr3p_cih, pSet_cresampler_t->DR3P_CIH);
	VPORT_WrFL(cvbsafe_010);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_013.clksel_phy	=	pSet_cresampler_t->clksel_phy;
#else
	VPORT_RdFL(top_013);
	VPORT_Wr01(top_013, clksel_phy, pSet_cresampler_t->clksel_phy);
	VPORT_WrFL(top_013);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_013.clksel_irisin	=	pSet_cresampler_t->clksel_irisin;
#else
	VPORT_RdFL(top_013);
	VPORT_Wr01(top_013, clksel_irisin, pSet_cresampler_t->clksel_irisin);
	VPORT_WrFL(top_013);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->top_013.clksel_linkapll	=	pSet_cresampler_t->clksel_linkapll;
#else
	VPORT_RdFL(top_013);
	VPORT_Wr01(top_013, clksel_linkapll, pSet_cresampler_t->clksel_linkapll);
	VPORT_WrFL(top_013);
#endif

}

void CVD_L8Bx_Set_Cresampler_Params(CVD_L8Bx_SET_CRES54MHZ_T *pSet_cresampler_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_147.reg_cres_clksel	=	pSet_cresampler_t->reg_cres_clksel;
#else
	VPORT_RdFL(iris_147);
	VPORT_Wr01(iris_147, reg_cres_clksel, pSet_cresampler_t->reg_cres_clksel);
	VPORT_WrFL(iris_147);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_147.reg_cres_0px_sel	=	pSet_cresampler_t->reg_cres_0px_sel;
#else
	VPORT_RdFL(iris_147);
	VPORT_Wr01(iris_147, reg_cres_0px_sel, pSet_cresampler_t->reg_cres_0px_sel);
	VPORT_WrFL(iris_147);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_147.reg_cres_blend_bcdefault	=	pSet_cresampler_t->reg_cres_blend_bcdefault;
#else
	VPORT_RdFL(iris_147);
	VPORT_Wr01(iris_147, reg_cres_blend_bcdefault, pSet_cresampler_t->reg_cres_blend_bcdefault);
	VPORT_WrFL(iris_147);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_139.reg_iris_cresampler_on	=	pSet_cresampler_t->reg_iris_cresampler_on;
#else
	VPORT_RdFL(iris_139);
	VPORT_Wr01(iris_139, reg_iris_cresampler_on, pSet_cresampler_t->reg_iris_cresampler_on);
	VPORT_WrFL(iris_139);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_139.reg_cres_lpfa_on	=	pSet_cresampler_t->reg_cres_lpfa_on;
#else
	VPORT_RdFL(iris_139);
	VPORT_Wr01(iris_139, reg_cres_lpfa_on, pSet_cresampler_t->reg_cres_lpfa_on);
	VPORT_WrFL(iris_139);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_139.reg_cres_lpfa_coeff0	=	pSet_cresampler_t->reg_cres_lpfa_coeff0;
#else
	VPORT_RdFL(iris_139);
	VPORT_Wr01(iris_139, reg_cres_lpfa_coeff0, pSet_cresampler_t->reg_cres_lpfa_coeff0);
	VPORT_WrFL(iris_139);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_140.reg_cres_lpfa_coeff1	=	pSet_cresampler_t->reg_cres_lpfa_coeff1;
#else
	VPORT_RdFL(iris_140);
	VPORT_Wr01(iris_140, reg_cres_lpfa_coeff1, pSet_cresampler_t->reg_cres_lpfa_coeff1);
	VPORT_WrFL(iris_140);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_140.reg_cres_lpfa_coeff2	=	pSet_cresampler_t->reg_cres_lpfa_coeff2;
#else
	VPORT_RdFL(iris_140);
	VPORT_Wr01(iris_140, reg_cres_lpfa_coeff2, pSet_cresampler_t->reg_cres_lpfa_coeff2);
	VPORT_WrFL(iris_140);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_141.reg_cres_lpfa_coeff3	=	pSet_cresampler_t->reg_cres_lpfa_coeff3;
#else
	VPORT_RdFL(iris_141);
	VPORT_Wr01(iris_141, reg_cres_lpfa_coeff3, pSet_cresampler_t->reg_cres_lpfa_coeff3);
	VPORT_WrFL(iris_141);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_141.reg_cres_lpfa_coeff4	=	pSet_cresampler_t->reg_cres_lpfa_coeff4;
#else
	VPORT_RdFL(iris_141);
	VPORT_Wr01(iris_141, reg_cres_lpfa_coeff4, pSet_cresampler_t->reg_cres_lpfa_coeff4);
	VPORT_WrFL(iris_141);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_142.reg_cres_lpfa_coeff5	=	pSet_cresampler_t->reg_cres_lpfa_coeff5;
#else
	VPORT_RdFL(iris_142);
	VPORT_Wr01(iris_142, reg_cres_lpfa_coeff5, pSet_cresampler_t->reg_cres_lpfa_coeff5);
	VPORT_WrFL(iris_142);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_143.reg_cres_lpfb_on	=	pSet_cresampler_t->reg_cres_lpfb_on;
#else
	VPORT_RdFL(iris_143);
	VPORT_Wr01(iris_143, reg_cres_lpfb_on, pSet_cresampler_t->reg_cres_lpfb_on);
	VPORT_WrFL(iris_143);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_143.reg_cres_lpfb_coeff0	=	pSet_cresampler_t->reg_cres_lpfb_coeff0;
#else
	VPORT_RdFL(iris_143);
	VPORT_Wr01(iris_143, reg_cres_lpfb_coeff0, pSet_cresampler_t->reg_cres_lpfb_coeff0);
	VPORT_WrFL(iris_143);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_144.reg_cres_lpfb_coeff1	=	pSet_cresampler_t->reg_cres_lpfb_coeff1;
#else
	VPORT_RdFL(iris_144);
	VPORT_Wr01(iris_144, reg_cres_lpfb_coeff1, pSet_cresampler_t->reg_cres_lpfb_coeff1);
	VPORT_WrFL(iris_144);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_144.reg_cres_lpfb_coeff2	=	pSet_cresampler_t->reg_cres_lpfb_coeff2;
#else
	VPORT_RdFL(iris_144);
	VPORT_Wr01(iris_144, reg_cres_lpfb_coeff2, pSet_cresampler_t->reg_cres_lpfb_coeff2);
	VPORT_WrFL(iris_144);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_145.reg_cres_lpfb_coeff3	=	pSet_cresampler_t->reg_cres_lpfb_coeff3;
#else
	VPORT_RdFL(iris_145);
	VPORT_Wr01(iris_145, reg_cres_lpfb_coeff3, pSet_cresampler_t->reg_cres_lpfb_coeff3);
	VPORT_WrFL(iris_145);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_145.reg_cres_lpfb_coeff4	=	pSet_cresampler_t->reg_cres_lpfb_coeff4;
#else
	VPORT_RdFL(iris_145);
	VPORT_Wr01(iris_145, reg_cres_lpfb_coeff4, pSet_cresampler_t->reg_cres_lpfb_coeff4);
	VPORT_WrFL(iris_145);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_154.reg_dtrs_revalpha_sel	=	pSet_cresampler_t->reg_dtrs_revalpha_sel;
#else
	VPORT_RdFL(iris_154);
	VPORT_Wr01(iris_154, reg_dtrs_revalpha_sel, pSet_cresampler_t->reg_dtrs_revalpha_sel);
	VPORT_WrFL(iris_154);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_154.reg_dtrs_dlysel	=	pSet_cresampler_t->reg_dtrs_dlysel;
#else
	VPORT_RdFL(iris_154);
	VPORT_Wr01(iris_154, reg_dtrs_dlysel, pSet_cresampler_t->reg_dtrs_dlysel);
	VPORT_WrFL(iris_154);
#endif


	//added 20110126

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_146.reg_cres_blend_method	=	pSet_cresampler_t->reg_cres_blend_method;
#else
	VPORT_RdFL(iris_146);
	VPORT_Wr01(iris_146, reg_cres_blend_method, pSet_cresampler_t->reg_cres_blend_method);
	VPORT_WrFL(iris_146);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_146.reg_cres_blend_sel	=	pSet_cresampler_t->reg_cres_blend_sel;
#else
	VPORT_RdFL(iris_146);
	VPORT_Wr01(iris_146, reg_cres_blend_sel, pSet_cresampler_t->reg_cres_blend_sel);
	VPORT_WrFL(iris_146);
#endif

}

void CVD_L8Bx_Set_Dot_Crawl_Params(CVD_L8Bx_SET_DOT_CRAWL_T *pSet_dotcrawl_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_155.reg_2dcomb_byp            	=		pSet_dotcrawl_t->reg_2dcomb_byp;
#else
	VPORT_RdFL(iris_155);
	VPORT_Wr01(iris_155, reg_2dcomb_byp, pSet_dotcrawl_t->reg_2dcomb_byp);
	VPORT_WrFL(iris_155);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_155.reg_2dcomb_cband_sel      	=		pSet_dotcrawl_t->reg_2dcomb_cband_sel;
#else
	VPORT_RdFL(iris_155);
	VPORT_Wr01(iris_155, reg_2dcomb_cband_sel, pSet_dotcrawl_t->reg_2dcomb_cband_sel);
	VPORT_WrFL(iris_155);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_155.reg_2dcomb_ccombo_sel     	=		pSet_dotcrawl_t->reg_2dcomb_ccombo_sel;
#else
	VPORT_RdFL(iris_155);
	VPORT_Wr01(iris_155, reg_2dcomb_ccombo_sel, pSet_dotcrawl_t->reg_2dcomb_ccombo_sel);
	VPORT_WrFL(iris_155);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_155.reg_2dcomb_ycrdc_cvsel    	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_cvsel;
#else
	VPORT_RdFL(iris_155);
	VPORT_Wr01(iris_155, reg_2dcomb_ycrdc_cvsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_cvsel);
	VPORT_WrFL(iris_155);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_155.reg_2dcomb_ycrdc_chsel    	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_chsel;
#else
	VPORT_RdFL(iris_155);
	VPORT_Wr01(iris_155, reg_2dcomb_ycrdc_chsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_chsel);
	VPORT_WrFL(iris_155);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_155.reg_2dcomb_ycrdc_lrsel    	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_lrsel;
#else
	VPORT_RdFL(iris_155);
	VPORT_Wr01(iris_155, reg_2dcomb_ycrdc_lrsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_lrsel);
	VPORT_WrFL(iris_155);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_156.reg_2dcomb_ycrdc_yhsymth  	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhsymth;
#else
	VPORT_RdFL(iris_156);
	VPORT_Wr01(iris_156, reg_2dcomb_ycrdc_yhsymth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhsymth);
	VPORT_WrFL(iris_156);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_156.reg_2dcomb_ycrdc_yhdiffth 	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhdiffth;
#else
	VPORT_RdFL(iris_156);
	VPORT_Wr01(iris_156, reg_2dcomb_ycrdc_yhdiffth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhdiffth);
	VPORT_WrFL(iris_156);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_157.reg_2dcomb_ycrdc_cvdiffth 	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_cvdiffth;
#else
	VPORT_RdFL(iris_157);
	VPORT_Wr01(iris_157, reg_2dcomb_ycrdc_cvdiffth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_cvdiffth);
	VPORT_WrFL(iris_157);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_yvsel    	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvsel;
#else
	VPORT_RdFL(iris_160);
	VPORT_Wr01(iris_160, reg_2dcomb_ycrdc_yvsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvsel);
	VPORT_WrFL(iris_160);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_yhsel    	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhsel;
#else
	VPORT_RdFL(iris_160);
	VPORT_Wr01(iris_160, reg_2dcomb_ycrdc_yhsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yhsel);
	VPORT_WrFL(iris_160);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_hth_on   	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth_on;
#else
	VPORT_RdFL(iris_160);
	VPORT_Wr01(iris_160, reg_2dcomb_ycrdc_hth_on, pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth_on);
	VPORT_WrFL(iris_160);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_hth_sel  	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth_sel;
#else
	VPORT_RdFL(iris_160);
	VPORT_Wr01(iris_160, reg_2dcomb_ycrdc_hth_sel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth_sel);
	VPORT_WrFL(iris_160);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_160.reg_2dcomb_ycrdc_hth      	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth;
#else
	VPORT_RdFL(iris_160);
	VPORT_Wr01(iris_160, reg_2dcomb_ycrdc_hth, pSet_dotcrawl_t->reg_2dcomb_ycrdc_hth);
	VPORT_WrFL(iris_160);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_lrtype   	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_lrtype;
#else
	VPORT_RdFL(iris_158);
	VPORT_Wr01(iris_158, reg_2dcomb_ycrdc_lrtype, pSet_dotcrawl_t->reg_2dcomb_ycrdc_lrtype);
	VPORT_WrFL(iris_158);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_dsel     	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_dsel;
#else
	VPORT_RdFL(iris_158);
	VPORT_Wr01(iris_158, reg_2dcomb_ycrdc_dsel, pSet_dotcrawl_t->reg_2dcomb_ycrdc_dsel);
	VPORT_WrFL(iris_158);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_lcr_on   	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_lcr_on;
#else
	VPORT_RdFL(iris_158);
	VPORT_Wr01(iris_158, reg_2dcomb_ycrdc_lcr_on, pSet_dotcrawl_t->reg_2dcomb_ycrdc_lcr_on);
	VPORT_WrFL(iris_158);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_158.reg_2dcomb_ycrdc_method   	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_method;
#else
	VPORT_RdFL(iris_158);
	VPORT_Wr01(iris_158, reg_2dcomb_ycrdc_method, pSet_dotcrawl_t->reg_2dcomb_ycrdc_method);
	VPORT_WrFL(iris_158);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_225.reg_2dcomb_ycrdc_yvdiv_th0	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th0;
#else
	VPORT_RdFL(iris_225);
	VPORT_Wr01(iris_225, reg_2dcomb_ycrdc_yvdiv_th0, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th0);
	VPORT_WrFL(iris_225);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_226.reg_2dcomb_ycrdc_yvdiv_th1	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th1;
#else
	VPORT_RdFL(iris_226);
	VPORT_Wr01(iris_226, reg_2dcomb_ycrdc_yvdiv_th1, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th1);
	VPORT_WrFL(iris_226);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_226.reg_2dcomb_ycrdc_yvdiv_th2	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th2;
#else
	VPORT_RdFL(iris_226);
	VPORT_Wr01(iris_226, reg_2dcomb_ycrdc_yvdiv_th2, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th2);
	VPORT_WrFL(iris_226);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_227.reg_2dcomb_ycrdc_yvdiv_th3	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th3;
#else
	VPORT_RdFL(iris_227);
	VPORT_Wr01(iris_227, reg_2dcomb_ycrdc_yvdiv_th3, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th3);
	VPORT_WrFL(iris_227);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_227.reg_2dcomb_ycrdc_yvdiv_th4	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th4;
#else
	VPORT_RdFL(iris_227);
	VPORT_Wr01(iris_227, reg_2dcomb_ycrdc_yvdiv_th4, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th4);
	VPORT_WrFL(iris_227);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_228.reg_2dcomb_ycrdc_yvdiv_th5	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th5;
#else
	VPORT_RdFL(iris_228);
	VPORT_Wr01(iris_228, reg_2dcomb_ycrdc_yvdiv_th5, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th5);
	VPORT_WrFL(iris_228);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_228.reg_2dcomb_ycrdc_yvdiv_th6	=		pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th6;
#else
	VPORT_RdFL(iris_228);
	VPORT_Wr01(iris_228, reg_2dcomb_ycrdc_yvdiv_th6, pSet_dotcrawl_t->reg_2dcomb_ycrdc_yvdiv_th6);
	VPORT_WrFL(iris_228);
#endif

}

void CVD_L8Bx_Set_Cross_Color_Reducer(CVD_L8Bx_SET_CROSS_COLOR_REDUCER_T *pSet_crosscolor_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_161.reg_2dcomb_cyrdc_method     	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_method;
#else
	VPORT_RdFL(iris_161);
	VPORT_Wr01(iris_161, reg_2dcomb_cyrdc_method, pSet_crosscolor_t->reg_2dcomb_cyrdc_method);
	VPORT_WrFL(iris_161);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_161.reg_2dcomb_cyrdc_lcr_on     	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_lcr_on;
#else
	VPORT_RdFL(iris_161);
	VPORT_Wr01(iris_161, reg_2dcomb_cyrdc_lcr_on, pSet_crosscolor_t->reg_2dcomb_cyrdc_lcr_on);
	VPORT_WrFL(iris_161);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_161.reg_2dcomb_cyrdc_crvalid    	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_crvalid;
#else
	VPORT_RdFL(iris_161);
	VPORT_Wr01(iris_161, reg_2dcomb_cyrdc_crvalid, pSet_crosscolor_t->reg_2dcomb_cyrdc_crvalid);
	VPORT_WrFL(iris_161);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_161.reg_2dcomb_cyrdc_lrtype     	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_lrtype;
#else
	VPORT_RdFL(iris_161);
	VPORT_Wr01(iris_161, reg_2dcomb_cyrdc_lrtype, pSet_crosscolor_t->reg_2dcomb_cyrdc_lrtype);
	VPORT_WrFL(iris_161);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_161.reg_2dcomb_cyrdc_dsel       	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_dsel;
#else
	VPORT_RdFL(iris_161);
	VPORT_Wr01(iris_161, reg_2dcomb_cyrdc_dsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_dsel);
	VPORT_WrFL(iris_161);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_161.reg_2dcomb_cyrdc_dsel1      	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_dsel1;
#else
	VPORT_RdFL(iris_161);
	VPORT_Wr01(iris_161, reg_2dcomb_cyrdc_dsel1, pSet_crosscolor_t->reg_2dcomb_cyrdc_dsel1);
	VPORT_WrFL(iris_161);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_162.reg_2dcomb_cyrdc_chsymth    	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_chsymth;
#else
	VPORT_RdFL(iris_162);
	VPORT_Wr01(iris_162, reg_2dcomb_cyrdc_chsymth, pSet_crosscolor_t->reg_2dcomb_cyrdc_chsymth);
	VPORT_WrFL(iris_162);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_162.reg_2dcomb_cyrdc_chsymth1   	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_chsymth1;
#else
	VPORT_RdFL(iris_162);
	VPORT_Wr01(iris_162, reg_2dcomb_cyrdc_chsymth1, pSet_crosscolor_t->reg_2dcomb_cyrdc_chsymth1);
	VPORT_WrFL(iris_162);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_230.reg_2dcomb_cyrdc_hvth       	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_hvth;
#else
	VPORT_RdFL(iris_230);
	VPORT_Wr01(iris_230, reg_2dcomb_cyrdc_hvth, pSet_crosscolor_t->reg_2dcomb_cyrdc_hvth);
	VPORT_WrFL(iris_230);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_230.reg_2dcomb_cyrdc_cvdiv_xsel 	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_xsel;
#else
	VPORT_RdFL(iris_230);
	VPORT_Wr01(iris_230, reg_2dcomb_cyrdc_cvdiv_xsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_xsel);
	VPORT_WrFL(iris_230);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_230.reg_2dcomb_cyrdc_cvdiv_th0  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th0;
#else
	VPORT_RdFL(iris_230);
	VPORT_Wr01(iris_230, reg_2dcomb_cyrdc_cvdiv_th0, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th0);
	VPORT_WrFL(iris_230);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_231.reg_2dcomb_cyrdc_cvdiv_th1  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th1;
#else
	VPORT_RdFL(iris_231);
	VPORT_Wr01(iris_231, reg_2dcomb_cyrdc_cvdiv_th1, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th1);
	VPORT_WrFL(iris_231);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_231.reg_2dcomb_cyrdc_cvdiv_th2  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th2;
#else
	VPORT_RdFL(iris_231);
	VPORT_Wr01(iris_231, reg_2dcomb_cyrdc_cvdiv_th2, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th2);
	VPORT_WrFL(iris_231);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_232.reg_2dcomb_cyrdc_cvdiv_th3  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th3;
#else
	VPORT_RdFL(iris_232);
	VPORT_Wr01(iris_232, reg_2dcomb_cyrdc_cvdiv_th3, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th3);
	VPORT_WrFL(iris_232);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_232.reg_2dcomb_cyrdc_cvdiv_th4  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th4;
#else
	VPORT_RdFL(iris_232);
	VPORT_Wr01(iris_232, reg_2dcomb_cyrdc_cvdiv_th4, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th4);
	VPORT_WrFL(iris_232);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_233.reg_2dcomb_cyrdc_cvdiv_th5  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th5;
#else
	VPORT_RdFL(iris_233);
	VPORT_Wr01(iris_233, reg_2dcomb_cyrdc_cvdiv_th5, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th5);
	VPORT_WrFL(iris_233);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_233.reg_2dcomb_cyrdc_cvdiv_th6  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th6;
#else
	VPORT_RdFL(iris_233);
	VPORT_Wr01(iris_233, reg_2dcomb_cyrdc_cvdiv_th6, pSet_crosscolor_t->reg_2dcomb_cyrdc_cvdiv_th6);
	VPORT_WrFL(iris_233);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_163.reg_2dcomb_cyrdc_chdiffth   	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffth;
#else
	VPORT_RdFL(iris_163);
	VPORT_Wr01(iris_163, reg_2dcomb_cyrdc_chdiffth, pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffth);
	VPORT_WrFL(iris_163);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_164.reg_2dcomb_cyrdc_chdiffon   	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffon;
#else
	VPORT_RdFL(iris_164);
	VPORT_Wr01(iris_164, reg_2dcomb_cyrdc_chdiffon, pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffon);
	VPORT_WrFL(iris_164);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_164.reg_2dcomb_cyrdc_chdiffsel  	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffsel;
#else
	VPORT_RdFL(iris_164);
	VPORT_Wr01(iris_164, reg_2dcomb_cyrdc_chdiffsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_chdiffsel);
	VPORT_WrFL(iris_164);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_228.reg_2dcomb_cyrdc_dir_on     	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_dir_on;
#else
	VPORT_RdFL(iris_228);
	VPORT_Wr01(iris_228, reg_2dcomb_cyrdc_dir_on, pSet_crosscolor_t->reg_2dcomb_cyrdc_dir_on);
	VPORT_WrFL(iris_228);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_228.reg_2dcomb_cyrdc_v0b1_dinsel	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_v0b1_dinsel;
#else
	VPORT_RdFL(iris_228);
	VPORT_Wr01(iris_228, reg_2dcomb_cyrdc_v0b1_dinsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_v0b1_dinsel);
	VPORT_WrFL(iris_228);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_229.reg_2dcomb_cyrdc_v1b4_cinsel	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_cinsel;
#else
	VPORT_RdFL(iris_229);
	VPORT_Wr01(iris_229, reg_2dcomb_cyrdc_v1b4_cinsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_cinsel);
	VPORT_WrFL(iris_229);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_229.reg_2dcomb_cyrdc_v1b4_dinsel	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_dinsel;
#else
	VPORT_RdFL(iris_229);
	VPORT_Wr01(iris_229, reg_2dcomb_cyrdc_v1b4_dinsel, pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_dinsel);
	VPORT_WrFL(iris_229);
#endif

	//gVportRegBx->iris_229.reg_2dcomb_cyrdc_v1b4_th    	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_th;
	_iow(&gVportRegBx->iris_229, 8, 0, pSet_crosscolor_t->reg_2dcomb_cyrdc_v1b4_th);

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_165.reg_2dcomb_cyrdc_hth_on     	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_hth_on;
#else
	VPORT_RdFL(iris_165);
	VPORT_Wr01(iris_165, reg_2dcomb_cyrdc_hth_on, pSet_crosscolor_t->reg_2dcomb_cyrdc_hth_on);
	VPORT_WrFL(iris_165);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_165.reg_2dcomb_cyrdc_hth        	=	pSet_crosscolor_t->reg_2dcomb_cyrdc_hth;
#else
	VPORT_RdFL(iris_165);
	VPORT_Wr01(iris_165, reg_2dcomb_cyrdc_hth, pSet_crosscolor_t->reg_2dcomb_cyrdc_hth);
	VPORT_WrFL(iris_165);
#endif

}

void CVD_L8Bx_Set_Motion_Detector(CVD_L8Bx_SET_MOTION_DETECTOR_T *pSet_motiondetector_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_064.reg_motion_mode           		=	pSet_motiondetector_t->reg_motion_mode           ;
#else
	VPORT_RdFL(iris_064);
	VPORT_Wr01(iris_064, reg_motion_mode, pSet_motiondetector_t->reg_motion_mode           );
	VPORT_WrFL(iris_064);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_064.reg_dot_suppress_mode     		=	pSet_motiondetector_t->reg_dot_suppress_mode     ;
#else
	VPORT_RdFL(iris_064);
	VPORT_Wr01(iris_064, reg_dot_suppress_mode, pSet_motiondetector_t->reg_dot_suppress_mode     );
	VPORT_WrFL(iris_064);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_064.reg_adaptive_chroma_mode  		=	pSet_motiondetector_t->reg_adaptive_chroma_mode  ;
#else
	VPORT_RdFL(iris_064);
	VPORT_Wr01(iris_064, reg_adaptive_chroma_mode, pSet_motiondetector_t->reg_adaptive_chroma_mode  );
	VPORT_WrFL(iris_064);
#endif


#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_091.reg_motion_config         		=	pSet_motiondetector_t->reg_motion_config         ;
	_iow(&gVportRegBx->iris_091, 8, 8, pSet_motiondetector_t->reg_motion_config);
#else
	VPORT_RdFL(iris_091);
	VPORT_Wr01(iris_091, reg_motion_config, pSet_motiondetector_t->reg_motion_config         );
	VPORT_WrFL(iris_091);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_092.reg_status_motion_mode    		=	pSet_motiondetector_t->reg_status_motion_mode    ;
#else
	VPORT_RdFL(iris_092);
	VPORT_Wr01(iris_092, reg_status_motion_mode, pSet_motiondetector_t->reg_status_motion_mode    );
	VPORT_WrFL(iris_092);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_093.reg_flat_luma_shift       		=	pSet_motiondetector_t->reg_flat_luma_shift       ;
#else
	VPORT_RdFL(iris_093);
	VPORT_Wr01(iris_093, reg_flat_luma_shift, pSet_motiondetector_t->reg_flat_luma_shift       );
	VPORT_WrFL(iris_093);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_093.reg_flat_chroma_shift     		=	pSet_motiondetector_t->reg_flat_chroma_shift     ;
#else
	VPORT_RdFL(iris_093);
	VPORT_Wr01(iris_093, reg_flat_chroma_shift, pSet_motiondetector_t->reg_flat_chroma_shift     );
	VPORT_WrFL(iris_093);
#endif


#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_093.reg_flat_luma_offset      		=	pSet_motiondetector_t->reg_flat_luma_offset      ;
	_iow(&gVportRegBx->iris_093, 8, 8, pSet_motiondetector_t->reg_flat_luma_offset);
#else
	VPORT_RdFL(iris_093);
	VPORT_Wr01(iris_093, reg_flat_luma_offset, pSet_motiondetector_t->reg_flat_luma_offset      );
	VPORT_WrFL(iris_093);
#endif



#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_093.reg_flat_chroma_offset    		=	pSet_motiondetector_t->reg_flat_chroma_offset    ;
	_iow(&gVportRegBx->iris_093, 8, 0, pSet_motiondetector_t->reg_flat_chroma_offset);
#else
	VPORT_RdFL(iris_093);
	VPORT_Wr01(iris_093, reg_flat_chroma_offset, pSet_motiondetector_t->reg_flat_chroma_offset    );
	VPORT_WrFL(iris_093);
#endif



#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_171.reg_frame_motion_th0      		=	pSet_motiondetector_t->reg_frame_motion_th0      ;
	_iow(&gVportRegBx->iris_171, 16, 16, pSet_motiondetector_t->reg_frame_motion_th0);
#else
	VPORT_RdFL(iris_171);
	VPORT_Wr01(iris_171, reg_frame_motion_th0, pSet_motiondetector_t->reg_frame_motion_th0      );
	VPORT_WrFL(iris_171);
#endif



#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_171.reg_frame_motion_th1      		=	pSet_motiondetector_t->reg_frame_motion_th1      ;
	_iow(&gVportRegBx->iris_171, 16, 0, pSet_motiondetector_t->reg_frame_motion_th1);
#else
	VPORT_RdFL(iris_171);
	VPORT_Wr01(iris_171, reg_frame_motion_th1, pSet_motiondetector_t->reg_frame_motion_th1      );
	VPORT_WrFL(iris_171);
#endif


#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_088.reg_chroma_level          		=	pSet_motiondetector_t->reg_chroma_level          ;
	_iow(&gVportRegBx->iris_088, 8, 24, pSet_motiondetector_t->reg_chroma_level);
#else
	VPORT_RdFL(iris_088);
	VPORT_Wr01(iris_088, reg_chroma_level, pSet_motiondetector_t->reg_chroma_level          );
	VPORT_WrFL(iris_088);
#endif



#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_088.reg_tcomb_chroma_level    		=	pSet_motiondetector_t->reg_tcomb_chroma_level    ;
	_iow(&gVportRegBx->iris_088, 8, 0, pSet_motiondetector_t->reg_tcomb_chroma_level);
#else
	VPORT_RdFL(iris_088);
	VPORT_Wr01(iris_088, reg_tcomb_chroma_level, pSet_motiondetector_t->reg_tcomb_chroma_level    );
	VPORT_WrFL(iris_088);
#endif




#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_189.reg_3dcomb_mdy_disel1     		=	pSet_motiondetector_t->reg_3dcomb_mdy_disel1     ;
#else
	VPORT_RdFL(iris_189);
	VPORT_Wr01(iris_189, reg_3dcomb_mdy_disel1, pSet_motiondetector_t->reg_3dcomb_mdy_disel1     );
	VPORT_WrFL(iris_189);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_210.reg_3dcomb_mdglo_mode     		=	pSet_motiondetector_t->reg_3dcomb_mdglo_mode     ;
#else
	VPORT_RdFL(iris_210);
	VPORT_Wr01(iris_210, reg_3dcomb_mdglo_mode, pSet_motiondetector_t->reg_3dcomb_mdglo_mode     );
	VPORT_WrFL(iris_210);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_211.reg_3dcomb_mdglo_m1_lth0  		=	pSet_motiondetector_t->reg_3dcomb_mdglo_m1_lth0  ;
#else
	VPORT_RdFL(iris_211);
	VPORT_Wr01(iris_211, reg_3dcomb_mdglo_m1_lth0, pSet_motiondetector_t->reg_3dcomb_mdglo_m1_lth0  );
	VPORT_WrFL(iris_211);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_211.reg_3dcomb_mdglo_m1_uth0  		=	pSet_motiondetector_t->reg_3dcomb_mdglo_m1_uth0  ;
#else
	VPORT_RdFL(iris_211);
	VPORT_Wr01(iris_211, reg_3dcomb_mdglo_m1_uth0, pSet_motiondetector_t->reg_3dcomb_mdglo_m1_uth0  );
	VPORT_WrFL(iris_211);
#endif


#if VPORT_REG_DIRECT_ACCESS
	//gVportRegBx->iris_211.reg_3dcomb_mdglo_m1_sameth		=	pSet_motiondetector_t->reg_3dcomb_mdglo_m1_sameth;
	_iow(&gVportRegBx->iris_211, 8, 8, pSet_motiondetector_t->reg_3dcomb_mdglo_m1_sameth);
#else
	VPORT_RdFL(iris_211);
	VPORT_Wr01(iris_211, reg_3dcomb_mdglo_m1_sameth, pSet_motiondetector_t->reg_3dcomb_mdglo_m1_sameth);
	VPORT_WrFL(iris_211);
#endif


}

void CVD_L8Bx_Set_Global_Classifier(   CVD_L8Bx_SET_GLOBAL_CLASSIFIER_T *pSet_globalclassifier)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_189.reg_3dcomb_md_mode              	=	pSet_globalclassifier->reg_3dcomb_md_mode              ;
#else
	VPORT_RdFL(iris_189);
	VPORT_Wr01(iris_189, reg_3dcomb_md_mode, pSet_globalclassifier->reg_3dcomb_md_mode              );
	VPORT_WrFL(iris_189);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_189.reg_3dcomb_mdy_dir              	=	pSet_globalclassifier->reg_3dcomb_mdy_dir              ;
#else
	VPORT_RdFL(iris_189);
	VPORT_Wr01(iris_189, reg_3dcomb_mdy_dir, pSet_globalclassifier->reg_3dcomb_mdy_dir              );
	VPORT_WrFL(iris_189);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_189.reg_3dcomb_mdy_disel            	=	pSet_globalclassifier->reg_3dcomb_mdy_disel            ;
#else
	VPORT_RdFL(iris_189);
	VPORT_Wr01(iris_189, reg_3dcomb_mdy_disel, pSet_globalclassifier->reg_3dcomb_mdy_disel            );
	VPORT_WrFL(iris_189);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_189.reg_3dcomb_mdy_sel              	=	pSet_globalclassifier->reg_3dcomb_mdy_sel              ;
#else
	VPORT_RdFL(iris_189);
	VPORT_Wr01(iris_189, reg_3dcomb_mdy_sel, pSet_globalclassifier->reg_3dcomb_mdy_sel              );
	VPORT_WrFL(iris_189);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_197.reg_3dcomb_mdlf_actv_sel        	=	pSet_globalclassifier->reg_3dcomb_mdlf_actv_sel        ;
#else
	VPORT_RdFL(iris_197);
	VPORT_Wr01(iris_197, reg_3dcomb_mdlf_actv_sel, pSet_globalclassifier->reg_3dcomb_mdlf_actv_sel        );
	VPORT_WrFL(iris_197);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_197.reg_3dcomb_mdlf_actv_th         	=	pSet_globalclassifier->reg_3dcomb_mdlf_actv_th         ;
#else
	VPORT_RdFL(iris_197);
	VPORT_Wr01(iris_197, reg_3dcomb_mdlf_actv_th, pSet_globalclassifier->reg_3dcomb_mdlf_actv_th         );
	VPORT_WrFL(iris_197);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_197.reg_3dcomb_mdhf_actv_th         	=	pSet_globalclassifier->reg_3dcomb_mdhf_actv_th         ;
#else
	VPORT_RdFL(iris_197);
	VPORT_Wr01(iris_197, reg_3dcomb_mdhf_actv_th, pSet_globalclassifier->reg_3dcomb_mdhf_actv_th         );
	VPORT_WrFL(iris_197);
#endif


#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_198.reg_3dcomb_mdlf_blk_th          	=	pSet_globalclassifier->reg_3dcomb_mdlf_blk_th          ;
	_iow(&gVportRegBx->iris_198, 8, 16, pSet_globalclassifier->reg_3dcomb_mdlf_blk_th);
#else
	VPORT_RdFL(iris_198);
	VPORT_Wr01(iris_198, reg_3dcomb_mdlf_blk_th, pSet_globalclassifier->reg_3dcomb_mdlf_blk_th          );
	VPORT_WrFL(iris_198);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_198.reg_3dcomb_mdhf_blk_th          	=	pSet_globalclassifier->reg_3dcomb_mdhf_blk_th          ;
	_iow(&gVportRegBx->iris_198, 8, 8, pSet_globalclassifier->reg_3dcomb_mdhf_blk_th);
#else
	VPORT_RdFL(iris_198);
	VPORT_Wr01(iris_198, reg_3dcomb_mdhf_blk_th, pSet_globalclassifier->reg_3dcomb_mdhf_blk_th          );
	VPORT_WrFL(iris_198);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_196.reg_3dcomb_mdclass_csel         	=	pSet_globalclassifier->reg_3dcomb_mdclass_csel         ;
#else
	VPORT_RdFL(iris_196);
	VPORT_Wr01(iris_196, reg_3dcomb_mdclass_csel, pSet_globalclassifier->reg_3dcomb_mdclass_csel         );
	VPORT_WrFL(iris_196);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_196.reg_3dcomb_mdclass_dsel         	=	pSet_globalclassifier->reg_3dcomb_mdclass_dsel         ;
#else
	VPORT_RdFL(iris_196);
	VPORT_Wr01(iris_196, reg_3dcomb_mdclass_dsel, pSet_globalclassifier->reg_3dcomb_mdclass_dsel         );
	VPORT_WrFL(iris_196);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_196.reg_3dcomb_mdclass_fldsel       	=	pSet_globalclassifier->reg_3dcomb_mdclass_fldsel       ;
#else
	VPORT_RdFL(iris_196);
	VPORT_Wr01(iris_196, reg_3dcomb_mdclass_fldsel, pSet_globalclassifier->reg_3dcomb_mdclass_fldsel       );
	VPORT_WrFL(iris_196);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_199.reg_3dcomb_mdlf_fld_th0         	=	pSet_globalclassifier->reg_3dcomb_mdlf_fld_th0         ;
#else
	VPORT_RdFL(iris_199);
	VPORT_Wr01(iris_199, reg_3dcomb_mdlf_fld_th0, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th0         );
	VPORT_WrFL(iris_199);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_199.reg_3dcomb_mdlf_fld_th1         	=	pSet_globalclassifier->reg_3dcomb_mdlf_fld_th1         ;
#else
	VPORT_RdFL(iris_199);
	VPORT_Wr01(iris_199, reg_3dcomb_mdlf_fld_th1, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th1         );
	VPORT_WrFL(iris_199);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_200.reg_3dcomb_mdlf_fld_th2         	=	pSet_globalclassifier->reg_3dcomb_mdlf_fld_th2         ;
#else
	VPORT_RdFL(iris_200);
	VPORT_Wr01(iris_200, reg_3dcomb_mdlf_fld_th2, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th2         );
	VPORT_WrFL(iris_200);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_200.reg_3dcomb_mdlf_fld_th3         	=	pSet_globalclassifier->reg_3dcomb_mdlf_fld_th3         ;
#else
	VPORT_RdFL(iris_200);
	VPORT_Wr01(iris_200, reg_3dcomb_mdlf_fld_th3, pSet_globalclassifier->reg_3dcomb_mdlf_fld_th3         );
	VPORT_WrFL(iris_200);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_200.reg_3dcomb_mdhf_fld_sel         	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_sel         ;
#else
	VPORT_RdFL(iris_200);
	VPORT_Wr01(iris_200, reg_3dcomb_mdhf_fld_sel, pSet_globalclassifier->reg_3dcomb_mdhf_fld_sel         );
	VPORT_WrFL(iris_200);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_201.reg_3dcomb_mdhf_fld_th0         	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_th0         ;
#else
	VPORT_RdFL(iris_201);
	VPORT_Wr01(iris_201, reg_3dcomb_mdhf_fld_th0, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th0         );
	VPORT_WrFL(iris_201);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_201.reg_3dcomb_mdhf_fld_th1         	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_th1         ;
#else
	VPORT_RdFL(iris_201);
	VPORT_Wr01(iris_201, reg_3dcomb_mdhf_fld_th1, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th1         );
	VPORT_WrFL(iris_201);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_202.reg_3dcomb_mdhf_fld_th2         	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_th2         ;
#else
	VPORT_RdFL(iris_202);
	VPORT_Wr01(iris_202, reg_3dcomb_mdhf_fld_th2, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th2         );
	VPORT_WrFL(iris_202);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_202.reg_3dcomb_mdhf_fld_th3         	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_th3         ;
#else
	VPORT_RdFL(iris_202);
	VPORT_Wr01(iris_202, reg_3dcomb_mdhf_fld_th3, pSet_globalclassifier->reg_3dcomb_mdhf_fld_th3         );
	VPORT_WrFL(iris_202);
#endif


#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_203.reg_3dcomb_mdhlf_fld_sub_th0    	=	pSet_globalclassifier->reg_3dcomb_mdhlf_fld_sub_th0    ;
	_iow(&gVportRegBx->iris_203, 8, 16, pSet_globalclassifier->reg_3dcomb_mdhlf_fld_sub_th0);
#else
	VPORT_RdFL(iris_203);
	VPORT_Wr01(iris_203, reg_3dcomb_mdhlf_fld_sub_th0, pSet_globalclassifier->reg_3dcomb_mdhlf_fld_sub_th0    );
	VPORT_WrFL(iris_203);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_203.reg_3dcomb_mdlf_fld_grd         	=	pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd         ;
	_iow(&gVportRegBx->iris_203, 8, 8, pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd);
#else
	VPORT_RdFL(iris_203);
	VPORT_Wr01(iris_203, reg_3dcomb_mdlf_fld_grd, pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd         );
	VPORT_WrFL(iris_203);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_203.reg_3dcomb_mdhf_fld_grd         	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd         ;
	_iow(&gVportRegBx->iris_203, 8, 0, pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd);
#else
	VPORT_RdFL(iris_203);
	VPORT_Wr01(iris_203, reg_3dcomb_mdhf_fld_grd, pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd         );
	VPORT_WrFL(iris_203);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_204.reg_3dcomb_mdlf_fld_grd_alp     	=	pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd_alp     ;
#else
	VPORT_RdFL(iris_204);
	VPORT_Wr01(iris_204, reg_3dcomb_mdlf_fld_grd_alp, pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd_alp     );
	VPORT_WrFL(iris_204);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_205.reg_3dcomb_mdlf_fld_grd_alp__2	=	pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd_alp__2;
#else
	VPORT_RdFL(iris_205);
	VPORT_Wr01(iris_205, reg_3dcomb_mdlf_fld_grd_alp__2, pSet_globalclassifier->reg_3dcomb_mdlf_fld_grd_alp__2);
	VPORT_WrFL(iris_205);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_206.reg_3dcomb_mdhf_fld_grd_alp     	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd_alp     ;
#else
	VPORT_RdFL(iris_206);
	VPORT_Wr01(iris_206, reg_3dcomb_mdhf_fld_grd_alp, pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd_alp     );
	VPORT_WrFL(iris_206);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_207.reg_3dcomb_mdhf_fld_grd_alp__2	=	pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd_alp__2;
#else
	VPORT_RdFL(iris_207);
	VPORT_Wr01(iris_207, reg_3dcomb_mdhf_fld_grd_alp__2, pSet_globalclassifier->reg_3dcomb_mdhf_fld_grd_alp__2);
	VPORT_WrFL(iris_207);
#endif

	/*
	   gVportRegBx->iris_185.reg_3dcomb_coefi_pow            	=	pSet_globalclassifier->reg_3dcomb_coefi_pow            ;
	//gVportRegBx->iris_186.reg_3dcomb_coefi                	=	pSet_globalclassifier->reg_3dcomb_coefi                ;
	_iow(&gVportRegBx->iris_186, 16, 0, pSet_globalclassifier->reg_3dcomb_coefi);
	gVportRegBx->iris_187.reg_3dcomb_coefi__2           	=	pSet_globalclassifier->reg_3dcomb_coefi__2           ;
	gVportRegBx->iris_188.reg_3dcomb_coefi__3           	=	pSet_globalclassifier->reg_3dcomb_coefi__3           ;
	 */

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_209.reg_3dcomb_mdpd32_on            	=	pSet_globalclassifier->reg_3dcomb_mdpd32_on            ;
#else
	VPORT_RdFL(iris_209);
	VPORT_Wr01(iris_209, reg_3dcomb_mdpd32_on, pSet_globalclassifier->reg_3dcomb_mdpd32_on            );
	VPORT_WrFL(iris_209);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_209.reg_3dcomb_mdpd32_sel           	=	pSet_globalclassifier->reg_3dcomb_mdpd32_sel           ;
#else
	VPORT_RdFL(iris_209);
	VPORT_Wr01(iris_209, reg_3dcomb_mdpd32_sel, pSet_globalclassifier->reg_3dcomb_mdpd32_sel           );
	VPORT_WrFL(iris_209);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_210.reg_3dcomb_mdpd32_th             	=	pSet_globalclassifier->reg_3dcomb_mdpd32_th            ;
#else
	VPORT_RdFL(iris_210);
	VPORT_Wr01(iris_210, reg_3dcomb_mdpd32_th, pSet_globalclassifier->reg_3dcomb_mdpd32_th            );
	VPORT_WrFL(iris_210);
#endif

}

void CVD_L8Bx_Set_Blending( CVD_L8Bx_SET_BLENDING_T	*pSet_blending)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_185.reg_3dcomb_coefi_pow            	=	pSet_blending->reg_3dcomb_coefi_pow            ;
#else
	VPORT_RdFL(iris_185);
	VPORT_Wr01(iris_185, reg_3dcomb_coefi_pow, pSet_blending->reg_3dcomb_coefi_pow            );
	VPORT_WrFL(iris_185);
#endif


#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_186.reg_3dcomb_coefi                	=	pSet_globalclassifier->reg_3dcomb_coefi                ;
	_iow(&gVportRegBx->iris_186, 16, 0, pSet_blending->reg_3dcomb_coefi);
#else
	VPORT_RdFL(iris_186);
	VPORT_Wr01(iris_186, reg_3dcomb_coefi, pSet_blending->reg_3dcomb_coefi);
	VPORT_WrFL(iris_186);
#endif



#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_187.reg_3dcomb_coefi__2           	=	pSet_blending->reg_3dcomb_coefi__2           ;
#else
	VPORT_RdFL(iris_187);
	VPORT_Wr01(iris_187, reg_3dcomb_coefi__2, pSet_blending->reg_3dcomb_coefi__2           );
	VPORT_WrFL(iris_187);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_188.reg_3dcomb_coefi__3           	=	pSet_blending->reg_3dcomb_coefi__3           ;
#else
	VPORT_RdFL(iris_188);
	VPORT_Wr01(iris_188, reg_3dcomb_coefi__3, pSet_blending->reg_3dcomb_coefi__3           );
	VPORT_WrFL(iris_188);
#endif

}

void CVD_L8Bx_Set_Output_Delay( CVD_L8Bx_SET_OUPUT_DELAY_T        	*pSet_outputdelay_t)
{

#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_179.reg_hrs_alpha_yoff 		= 	pSet_outputdelay_t->reg_hrs_alpha_yoff 	;
	_iow(&gVportRegBx->iris_179, 8, 16, pSet_outputdelay_t->reg_hrs_alpha_yoff);
#else
	VPORT_RdFL(iris_179);
	VPORT_Wr01(iris_179, reg_hrs_alpha_yoff, pSet_outputdelay_t->reg_hrs_alpha_yoff 	);
	VPORT_WrFL(iris_179);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_179.reg_hrs_alpha_cboff		= 	pSet_outputdelay_t->reg_hrs_alpha_cboff;
	_iow(&gVportRegBx->iris_179, 8, 8, pSet_outputdelay_t->reg_hrs_alpha_cboff);
#else
	VPORT_RdFL(iris_179);
	VPORT_Wr01(iris_179, reg_hrs_alpha_cboff, pSet_outputdelay_t->reg_hrs_alpha_cboff);
	VPORT_WrFL(iris_179);
#endif



#if VPORT_REG_DIRECT_ACCESS
	// gVportRegBx->iris_179.reg_hrs_alpha_croff		= 	pSet_outputdelay_t->reg_hrs_alpha_croff;
	_iow(&gVportRegBx->iris_179, 8, 0, pSet_outputdelay_t->reg_hrs_alpha_croff);
#else
	VPORT_RdFL(iris_179);
	VPORT_Wr01(iris_179, reg_hrs_alpha_croff, pSet_outputdelay_t->reg_hrs_alpha_croff);
	VPORT_WrFL(iris_179);
#endif



	//added 0126

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_021.reg_yc_delay		= 	pSet_outputdelay_t->reg_yc_delay;
#else
	VPORT_RdFL(iris_021);
	VPORT_Wr01(iris_021, reg_yc_delay, pSet_outputdelay_t->reg_yc_delay);
	VPORT_WrFL(iris_021);
#endif

	//	gVportRegBx->iris_234.reg_ycadj_cr_p		= 	pSet_outputdelay_t->reg_ycadj_cr_p;

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_234.reg_cb_delay		= 	pSet_outputdelay_t->reg_cb_delay;
#else
	VPORT_RdFL(iris_234);
	VPORT_Wr01(iris_234, reg_cb_delay, pSet_outputdelay_t->reg_cb_delay);
	VPORT_WrFL(iris_234);
#endif


}

void CVD_L8Bx_Set_Output_Range(CVD_L8Bx_SET_OUPUT_RANGE_T	*pSet_outputrange_t)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_166.reg_oadj_y_offi 	=	pSet_outputrange_t->reg_oadj_y_offi;
#else
	VPORT_RdFL(iris_166);
	VPORT_Wr01(iris_166, reg_oadj_y_offi, pSet_outputrange_t->reg_oadj_y_offi);
	VPORT_WrFL(iris_166);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_166.reg_oadj_y_offo 	=	pSet_outputrange_t->reg_oadj_y_offo;
#else
	VPORT_RdFL(iris_166);
	VPORT_Wr01(iris_166, reg_oadj_y_offo, pSet_outputrange_t->reg_oadj_y_offo);
	VPORT_WrFL(iris_166);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_167.reg_oadj_y_coeff	=	pSet_outputrange_t->reg_oadj_y_coeff;
#else
	VPORT_RdFL(iris_167);
	VPORT_Wr01(iris_167, reg_oadj_y_coeff, pSet_outputrange_t->reg_oadj_y_coeff);
	VPORT_WrFL(iris_167);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_168.reg_oadj_c_offi	=	pSet_outputrange_t->reg_oadj_c_offi;
#else
	VPORT_RdFL(iris_168);
	VPORT_Wr01(iris_168, reg_oadj_c_offi, pSet_outputrange_t->reg_oadj_c_offi);
	VPORT_WrFL(iris_168);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_168.reg_oadj_c_offo	=	pSet_outputrange_t->reg_oadj_c_offo;
#else
	VPORT_RdFL(iris_168);
	VPORT_Wr01(iris_168, reg_oadj_c_offo, pSet_outputrange_t->reg_oadj_c_offo);
	VPORT_WrFL(iris_168);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_169.reg_oadj_c_coeff	=	pSet_outputrange_t->reg_oadj_c_coeff;
#else
	VPORT_RdFL(iris_169);
	VPORT_Wr01(iris_169, reg_oadj_c_coeff, pSet_outputrange_t->reg_oadj_c_coeff);
	VPORT_WrFL(iris_169);
#endif


}

int	CVD_L8Bx_Read_fld_cnt_value(UINT16 *pfld_hfcnt_value, UINT16 *pfld_lfcnt_value)
{
#if VPORT_REG_DIRECT_ACCESS
	*pfld_lfcnt_value = gVportRegBx->iris_208.status_fld_lfcnt;
	*pfld_hfcnt_value = gVportRegBx->iris_208.status_fld_hfcnt;
#else
	VPORT_RdFL(iris_208);
	VPORT_Rd02(iris_208, status_fld_lfcnt, *pfld_lfcnt_value, status_fld_hfcnt, *pfld_hfcnt_value);
#endif


	return 0;
}

int CVD_L8Bx_Set_motion_mode(UINT8	md_mode_value, UINT8 motion_mode_value)
{

#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_189.reg_3dcomb_md_mode = md_mode_value;
#else
	VPORT_RdFL(iris_189);
	VPORT_Wr01(iris_189, reg_3dcomb_md_mode, md_mode_value);
	VPORT_WrFL(iris_189);
#endif


#if VPORT_REG_DIRECT_ACCESS
	gVportRegBx->iris_064.reg_motion_mode = motion_mode_value;
#else
	VPORT_RdFL(iris_064);
	VPORT_Wr01(iris_064, reg_motion_mode, motion_mode_value);
	VPORT_WrFL(iris_064);
#endif


	return 0;
}

int CVD_L8Bx_Channel_Power_Control(UINT32 on_off)
{
	if(on_off)
	{	// Normal condition

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->cvbsafe_002.cvbs1_pdb	=	1;
		gVportRegBx->cvbsafe_002.cvbs_pdbm	=	1;
#else
		VPORT_RdFL(cvbsafe_002);
		VPORT_Wr02(cvbsafe_002, cvbs1_pdb, 1, cvbs_pdbm, 1);
		VPORT_WrFL(cvbsafe_002);
#endif

		//		gVportRegBx->cvbsafe_002.cvbs2_pdb	=	1;	// CVBS2 in not used

		mdelay(5);

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_005.reg_swrst_f81	=	0;
		gVportRegBx->top_005.reg_swrst_irisin	=	0;
		gVportRegBx->top_005.reg_swrst_irisyc	=	0;
#else
		VPORT_RdFL(top_005);
		VPORT_Wr03(top_005, reg_swrst_f81, 0, reg_swrst_irisin, 0, reg_swrst_irisyc, 0);
		VPORT_WrFL(top_005);
#endif

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_006.reg_swrst_irisrm	=	0;
		gVportRegBx->top_006.reg_swrst_iriswm	=	0;
#else
		VPORT_RdFL(top_006);
		VPORT_Wr02(top_006, reg_swrst_irisrm, 0, reg_swrst_iriswm, 0);
		VPORT_WrFL(top_006);
#endif

	}
	else
	{	// Power down

#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_005.reg_swrst_f81	=	1;
		gVportRegBx->top_005.reg_swrst_irisin	=	1;
		gVportRegBx->top_005.reg_swrst_irisyc	=	1;
#else
		VPORT_RdFL(top_005);
		VPORT_Wr03(top_005, reg_swrst_f81, 1, reg_swrst_irisin, 1, reg_swrst_irisyc, 1);
		VPORT_WrFL(top_005);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->top_006.reg_swrst_irisrm	=	1;
		gVportRegBx->top_006.reg_swrst_iriswm	=	1;
#else
		VPORT_RdFL(top_006);
		VPORT_Wr02(top_006, reg_swrst_irisrm, 1, reg_swrst_iriswm, 1);
		VPORT_WrFL(top_006);
#endif


#if VPORT_REG_DIRECT_ACCESS
		gVportRegBx->cvbsafe_002.cvbs1_pdb	=	0;
		gVportRegBx->cvbsafe_002.cvbs_pdbm	=	0;
#else
		VPORT_RdFL(cvbsafe_002);
		VPORT_Wr02(cvbsafe_002, cvbs1_pdb, 0, cvbs_pdbm, 0);
		VPORT_WrFL(cvbsafe_002);
#endif

		//		gVportRegBx->cvbsafe_002.cvbs2_pdb	=	0;	// CVBS2 in not used

	}

	return 0;
}

int CVD_L8Bx_OnOff_Chromalock_Ckill(BOOLEAN bonoff)
{
	CVD_DEBUG("%s entered : This chip set does not support this function \n",__func__);
	return 0;
}
