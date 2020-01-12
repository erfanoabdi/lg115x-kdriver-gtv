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


/** @file pe_ccm.c
 *
 *  driver for picture enhance color correction module functions. ( used only within kdriver )
 *	- white balance, gamma control.
 *
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note
 *	@date		2011.06.11
 *	@see
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <mach/platform.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#include "base_types.h"
#include "os_util.h"

#include "de_mif_reg_l9.h"		// for PE_CCM_SetGammaLut by DDR
#include "de_wcp_reg_l9.h"		// for PE_CCM_SetGammaLut by DDR
#include "de_cfg_def.h"
#include "de_cfg_l9.h"
#include "de_cfg.h"

#include "pe_reg.h"
#include "pe_cfg.h"
#include "pe_def.h"
#include "pe_ccm.h"
#include "pe_ccm_coeff.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define PE1_L_TOP_CTRL_00_OFFSET	0x1300
#define PE1_L_GMC_CTRL_02_OFFSET	0x16f8
#define PE1_L_GMC_IA_CTRL_OFFSET	0x16fc
#define PE1_L_GMC_IA_DATA_OFFSET	0x1700

#define PE1_R_TOP_CTRL_00_OFFSET	0x3300
#define PE1_R_GMC_CTRL_02_OFFSET	0x36f8
#define PE1_R_GMC_IA_CTRL_OFFSET	0x36fc
#define PE1_R_GMC_IA_DATA_OFFSET	0x3700

#define PE_CCM_SAT_STATUS_TH	200
#define PE_CCM_LRCR_GAIN		0xa

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_CCM_ERROR	printk

#define PE_CCM_DBG_PRINT(fmt,args...)	if(_g_ccm_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_CCM_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_CCM_ERROR(fmt,##args);_action;}}

#define memset0(_var)	memset( &(_var), 0x00, sizeof(_var))

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	data_cnt                        :14,	//  0:13
	                                : 2,	// 14:15 reserved
	block_mode                      : 2,	// 16:17
	                                : 2,	// 18:19 reserved
	auto_inc                        : 1,	//    20
	                                :10,	// 21:30 reserved
	last_block                      : 1;	//    31
} VIDEO_DDR_BLOCK_HEAD_T;

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern DE_P0L_REG_L9_T gDE_P0L_L9;
extern DE_P0R_REG_L9_T gDE_P0R_L9;
extern DE_P1L_REG_L9_T gDE_P1L_L9;
extern DE_P1R_REG_L9_T gDE_P1R_L9;
extern DE_P0L_REG_L9B_T gDE_P0L_L9B;
extern DE_P0R_REG_L9B_T gDE_P0R_L9B;
extern DE_P1L_REG_L9B_T gDE_P1L_L9B;
extern DE_P1R_REG_L9B_T gDE_P1R_L9B;

extern DE_MIB_REG_L9_T gDE_MIB_L9;		// for PE_CCM_SetGammaLut (L) by DDR
extern DE_MID_REG_L9_T gDE_MID_L9;		// for PE_CCM_SetGammaLut (R) by DDR
extern DE_OVL_REG_L9_T gDE_OVL_L9;		// for PE_CCM_SetGammaLut (L) by DDR
extern DE_OVR_REG_L9_T gDE_OVR_L9;		// for PE_CCM_SetGammaLut (R) by DDR

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int PE_CCM_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_CCM_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_CCM_SetGammaLut(LX_PE_CCM_GAMMALUT_T *pstParams);
static int PE_CCM_GetGammaLut(LX_PE_CCM_GAMMALUT_T *pstParams);
static int PE_CCM_SetWhiteBalance(LX_PE_CCM_WB_T *pstParams);
static int PE_CCM_GetWhiteBalance(LX_PE_CCM_WB_T *pstParams);
static int PE_CCM_SetAutoCorrection(LX_PE_CCM_AUTO_CR_T *pstParams);
static int PE_CCM_GetAutoCorrection(LX_PE_CCM_AUTO_CR_T *pstParams);
static int PE_CCM_SetPixelReplacement(LX_PE_CCM_PIXEL_REPLACE_T *pstParams);
static int PE_CCM_GetPixelReplacement(LX_PE_CCM_PIXEL_REPLACE_T *pstParams);
static int PE_CCM_SetGammaLutByDdr(LX_PE_CCM_GAMMALUT_T *pstParams);
static int PE_CCM_SetGammaLutByReg(LX_PE_CCM_GAMMALUT_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_ccm_trace=0x0;		//default should be off.
static UINT8 _g_ccm_lut_use_ddr=0x0;	// 0:by register, 1:by ddr, initialized in PE_CCM_Init()
static UINT32 _g_ccm_auto_cr_en=0x0;	// auto correction enable

/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * init color correctin module
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CCM_Init(void)
{
	int ret = RET_OK;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_CCM_DBG_PRINT("init global values.\n");
			_g_ccm_lut_use_ddr = 0x1;	// set gamma lut by DDR
			PE_CCM_DBG_PRINT("init wb.\n");
			PE_P1L_L9B0_Wr03(pe1_l_wb_ctrl_00,	wb_en, 				0x1,\
												degamma_en, 		0x1,\
												gamma_en, 			0x1);
			PE_P1L_L9B0_Wr03(pe1_l_wb_ctrl_01,	user_ctrl_g_gain,	0xc0,\
												user_ctrl_b_gain,	0xc0,\
												user_ctrl_r_gain,	0xc0);
			PE_P1L_L9B0_Wr03(pe1_l_wb_ctrl_02,	user_ctrl_g_offset,	0x80,\
												user_ctrl_b_offset,	0x80,\
												user_ctrl_r_offset,	0x80);

			PE_P1R_L9B0_Wr03(pe1_r_wb_ctrl_00,	wb_en, 				0x1,\
												degamma_en, 		0x1,\
												gamma_en, 			0x1);
			PE_P1R_L9B0_Wr03(pe1_r_wb_ctrl_01,	user_ctrl_g_gain,	0xc0,\
												user_ctrl_b_gain,	0xc0,\
												user_ctrl_r_gain,	0xc0);
			PE_P1R_L9B0_Wr03(pe1_r_wb_ctrl_02,	user_ctrl_g_offset,	0x80,\
												user_ctrl_b_offset,	0x80,\
												user_ctrl_r_offset,	0x80);

			PE_CCM_DBG_PRINT("init gmc.\n");
			PE_P1L_L9B0_Wr07(pe1_l_gmc_ctrl_00, pxl_rep_enable,		0x0,\
												pxl_rep_area,		0x0,\
												pxl_rep_disable_r,	0x1,\
												pxl_rep_disable_g,	0x1,\
												pxl_rep_disable_b,	0x1,\
												pxl_rep_xpos,		0x0,\
												pxl_rep_ypos,		0x0);
			PE_P1L_L9B0_Wr02(pe1_l_gmc_ctrl_01, pxl_rep_width,		0x780,\
												pxl_rep_height,		0x438);
			PE_P1L_L9B0_Wr03(pe1_l_gmc_ctrl_02, pxl_rep_value_g,	0x0,\
												pxl_rep_value_b,	0x0,\
												pxl_rep_value_r,	0x0);

			PE_P1R_L9B0_Wr07(pe1_r_gmc_ctrl_00, pxl_rep_enable,		0x0,\
												pxl_rep_area,		0x0,\
												pxl_rep_disable_r,	0x1,\
												pxl_rep_disable_g,	0x1,\
												pxl_rep_disable_b,	0x1,\
												pxl_rep_xpos,		0x0,\
												pxl_rep_ypos,		0x0);
			PE_P1R_L9B0_Wr02(pe1_r_gmc_ctrl_01, pxl_rep_width,		0x780,\
												pxl_rep_height,		0x438);
			PE_P1R_L9B0_Wr03(pe1_r_gmc_ctrl_02, pxl_rep_value_g,	0x0,\
												pxl_rep_value_b,	0x0,\
												pxl_rep_value_r,	0x0);
	
			PE_CCM_DBG_PRINT("init lrcr config.\n");
			DE_P1L_L9B0_RdFL(pe1_l_lrcr_ctrl_00);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,enable,				0x0);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,adjust_en,			0x1);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,protection_en,		0x1);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,adjust_en2,			0x1);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,r_hist_data_sel,	0x1);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,l_hist_data_sel,	0x0);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,r_proc_data_sel,	0x1);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,l_proc_data_sel,	0x0);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,ff_mode_en,			0x0);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,ff_mode_parity,		0x0);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,r_out_data_sel,		0x1);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,l_out_data_sel,		0x0);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,reg_lrcr_gain,		PE_CCM_LRCR_GAIN);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,hist_rd_en,			0x0);
			DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,hist_mode,			0x0);	//256 bin
			DE_P1L_L9B0_WrFL(pe1_l_lrcr_ctrl_00);

			PE_P1L_L9B0_Wr04(pe1_l_lrcr_ctrl_01,th_max_hist,		0xe0,\
												th_valid_bins,		0x40,\
												th1,				0xe0,\
												th0,				0x4);
			PE_P1L_L9B0_Wr04(pe1_l_lrcr_ctrl_02,hist_l_window_h_bbd,0x0,\
												hist_l_window_h_en,	0x1,\
												hist_l_window_v_bbd,0x0,\
												hist_l_window_v_en,	0x1);
			PE_P1L_L9B0_Wr04(pe1_l_lrcr_ctrl_04,hist_r_window_h_bbd,0x0,\
												hist_r_window_h_en,	0x1,\
												hist_r_window_v_bbd,0x0,\
												hist_r_window_v_en,	0x1);
			PE_P1L_L9B0_Wr04(pe1_l_lrcr_ctrl_06,active_window_h_bbd,0x0,\
												active_window_h_en,	0x1,\
												active_window_v_bbd,0x0,\
												active_window_v_en,	0x1);
		}
		else
		{
			PE_CCM_DBG_PRINT("init global values.\n");
			_g_ccm_lut_use_ddr = 0x1;	// set gamma lut by DDR
			PE_CCM_DBG_PRINT("init wb.\n");
			DE_P1L_L9_RdFL(pe1_l_wb_ctrl_00);
			DE_P1L_L9_RdFL(pe1_l_wb_ctrl_02);
			DE_P1L_L9_RdFL(pe1_l_wb_ctrl_03);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, wb_en, 				0x1);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, degamma_en, 			0x1);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, gamma_en, 				0x1);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_02, user_ctrl_g_gain,		0xc0);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_02, user_ctrl_b_gain,		0xc0);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_02, user_ctrl_r_gain,		0xc0);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_03, user_ctrl_g_offset,	0x80);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_03, user_ctrl_b_offset,	0x80);
			DE_P1L_L9_Wr01(pe1_l_wb_ctrl_03, user_ctrl_r_offset,	0x80);
			DE_P1L_L9_WrFL(pe1_l_wb_ctrl_00);
			DE_P1L_L9_WrFL(pe1_l_wb_ctrl_02);
			DE_P1L_L9_WrFL(pe1_l_wb_ctrl_03);

			DE_P1R_L9_RdFL(pe1_r_wb_ctrl_00);
			DE_P1R_L9_RdFL(pe1_r_wb_ctrl_02);
			DE_P1R_L9_RdFL(pe1_r_wb_ctrl_03);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, wb_en, 				0x1);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, degamma_en, 			0x1);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, gamma_en, 				0x1);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_02, user_ctrl_g_gain,		0xc0);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_02, user_ctrl_b_gain,		0xc0);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_02, user_ctrl_r_gain,		0xc0);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_03, user_ctrl_g_offset,	0x80);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_03, user_ctrl_b_offset,	0x80);
			DE_P1R_L9_Wr01(pe1_r_wb_ctrl_03, user_ctrl_r_offset,	0x80);
			DE_P1R_L9_WrFL(pe1_r_wb_ctrl_00);
			DE_P1R_L9_WrFL(pe1_r_wb_ctrl_02);
			DE_P1R_L9_WrFL(pe1_r_wb_ctrl_03);

			PE_CCM_DBG_PRINT("init gmc.\n");
			DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_00);
			DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_01);
			DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_02);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_enable,		0x0);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_area,			0x0); // 0:inside,1: outside
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_r,	0x1);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_g,	0x1);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_b,	0x1);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_xpos,			0x0);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_ypos,			0x0);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_01, pxl_rep_width,		0x780);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_01, pxl_rep_height,		0x438);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02, pxl_rep_value_g,		0x0);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02, pxl_rep_value_b,		0x0);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02, pxl_rep_value_r,		0x0);
			DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_00);
			DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_01);
			DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_02);

			DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_00);
			DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_01);
			DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_02);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_enable,		0x0);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_area,			0x0); // 0:inside,1: outside
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_r,	0x1);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_g,	0x1);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_b,	0x1);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_xpos,			0x0);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_ypos,			0x0);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_01, pxl_rep_width,		0x780);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_01, pxl_rep_height,		0x438);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02, pxl_rep_value_g,		0x0);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02, pxl_rep_value_b,		0x0);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02, pxl_rep_value_r,		0x0);
			DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_00);
			DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_01);
			DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_02);
		}
	}while(0);

	return ret;
}

/**
 * standard function of ccm setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CCM_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CCM_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CCM_GAMMALUT_T):
		{
			LX_PE_CCM_GAMMALUT_T *pRcvData;
			LX_PE_CCM_GAMMALUT_T stParam;
			UINT32 *pLut = NULL;
			UINT32 bufsize;
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_GAMMALUT_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_GAMMALUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_GAMMALUT_T *)data;
				bufsize = GET_BITS(pRcvData->lut_size,0,16) * sizeof(UINT32);
				PE_CCM_CHECK_CODE(!bufsize,break,"[%s,%d] bufsize zero.\n",__F__,__L__);

				pLut = OS_Malloc(bufsize);
				PE_CCM_CHECK_CODE(!pLut,break,"[%s,%d] pLut is null.\n",__F__,__L__);

				ret = copy_from_user(pLut, (void __user *)pRcvData->lut_data, bufsize);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] copy_from_user error.\n",__F__,__L__);

				stParam.win_id = pRcvData->win_id;
				stParam.lut_size = pRcvData->lut_size;
				stParam.lut_data = pLut;
				ret = PE_CCM_SetGammaLut(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetGammaLut() error.\n",__F__,__L__);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_GAMMALUT_T\n");
			}while(0);
			if(pLut)	OS_Free(pLut);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CCM_PIXEL_REPLACE_T):
		{
			LX_PE_CCM_PIXEL_REPLACE_T *pRcvData;
			LX_PE_CCM_PIXEL_REPLACE_T stParam;
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_PIXEL_REPLACE_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_PIXEL_REPLACE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_PIXEL_REPLACE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CCM_SetPixelReplacement(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetPixelReplacement() error.\n",__F__,__L__);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_PIXEL_REPLACE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CCM_WB_T):
		{
			LX_PE_CCM_WB_T *pRcvData;
			LX_PE_CCM_WB_T stParam;
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_WB_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_WB_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_WB_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CCM_SetWhiteBalance(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetWhiteBalance() error.\n",__F__,__L__);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_WB_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CCM_AUTO_CR_T):
		{
			LX_PE_CCM_AUTO_CR_T *pRcvData;
			LX_PE_CCM_AUTO_CR_T stParam;
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_AUTO_CR_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_AUTO_CR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_AUTO_CR_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CCM_SetAutoCorrection(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetAutoCorrection() error.\n",__F__,__L__);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_AUTO_CR_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_CCM_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CCM_SetDefaultSettings(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetDefaultSettings() error.\n",__F__,__L__);
				PE_CCM_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_CCM_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CCM_SetDebugSettings(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetDebugSettings() error.\n",__F__,__L__);
				PE_CCM_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CCM_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of ccm getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CCM_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CCM_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CCM_GAMMALUT_T):
		{
			LX_PE_CCM_GAMMALUT_T *pRcvData;
			LX_PE_CCM_GAMMALUT_T stParam;
			UINT32 *pLut = NULL;
			UINT32 bufsize;
			memset0(stParam);
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_GAMMALUT_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_GAMMALUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_GAMMALUT_T *)data;
				bufsize = GET_BITS(pRcvData->lut_size,0,16) * sizeof(UINT32);
				PE_CCM_CHECK_CODE(!bufsize,break,"[%s,%d] bufsize zero.\n",__F__,__L__);

				pLut = OS_Malloc(bufsize);
				PE_CCM_CHECK_CODE(!pLut,break,"[%s,%d] pLut is null.\n",__F__,__L__);

				stParam.win_id = pRcvData->win_id;
				stParam.lut_size = pRcvData->lut_size;
				stParam.lut_data = pLut;
				ret = PE_CCM_GetGammaLut(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_GetGammaLut() error.\n",__F__,__L__);

				ret = copy_to_user((void __user *)pRcvData->lut_data, pLut, bufsize);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] copy_to_user error.\n",__F__,__L__);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_GAMMALUT_T\n");
			}while(0);
			if(pLut)	OS_Free(pLut);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CCM_PIXEL_REPLACE_T):
		{
			LX_PE_CCM_PIXEL_REPLACE_T *pRcvData;
			LX_PE_CCM_PIXEL_REPLACE_T stParam;
			memset0(stParam);
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_PIXEL_REPLACE_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_PIXEL_REPLACE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_PIXEL_REPLACE_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CCM_GetPixelReplacement(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_GetPixelReplacement() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_PIXEL_REPLACE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CCM_WB_T):
		{
			LX_PE_CCM_WB_T *pRcvData;
			LX_PE_CCM_WB_T stParam;
			memset0(stParam);
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_WB_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_WB_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_WB_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CCM_GetWhiteBalance(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_GetWhiteBalance() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_WB_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CCM_AUTO_CR_T):
		{
			LX_PE_CCM_AUTO_CR_T *pRcvData;
			LX_PE_CCM_AUTO_CR_T stParam;
			memset0(stParam);
			do{
				PE_CCM_DBG_PRINT("S LX_PE_CCM_AUTO_CR_T\n");
				/* data size check */
				PE_CCM_CHECK_CODE(datasize!=sizeof(LX_PE_CCM_AUTO_CR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CCM_AUTO_CR_T *)data;
				ret = PE_CCM_GetAutoCorrection(&stParam);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_GetAutoCorrection() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CCM_DBG_PRINT("E LX_PE_CCM_AUTO_CR_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CCM_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_CCM_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CCM_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);
	}while(0);
	return ret;
}
static int PE_CCM_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CCM_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(CCM)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_ccm_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(CCM)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(CCM)))
				{
					PE_CCM_DBG_PRINT("wb,degamma,gamma,pxl_rep_en : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9B0_RdFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_00, wb_en, 			0x0);
						DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_00, degamma_en, 		0x0);
						DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_00, gamma_en, 		0x0);
						DE_P1L_L9B0_WrFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_enable,	0x0);
						DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_02);
						DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02, gmc_mode,		0x0);//read mode
						DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_02);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9B0_RdFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_00, wb_en, 			0x0);
						DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_00, degamma_en, 		0x0);
						DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_00, gamma_en, 		0x0);
						DE_P1R_L9B0_WrFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_enable,	0x0);
						DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_02);
						DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02, gmc_mode,		0x0);//read mode
						DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_02);
					}
				}
				else
				{
					PE_CCM_DBG_PRINT("wb,degamma,gamma,pxl_rep_en,pxl_rep_dis : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9B0_RdFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_00, wb_en, 			0x1);
						DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_00, degamma_en, 		0x1);
						DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_00, gamma_en, 		0x1);
						DE_P1L_L9B0_WrFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_enable,	0x1);
						DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_r,0x1);
						DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_g,0x1);
						DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_b,0x1);
						DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_02);
						DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02, gmc_mode,		0x2);//normal mode
						DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_02);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9B0_RdFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_00, wb_en, 			0x1);
						DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_00, degamma_en, 		0x1);
						DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_00, gamma_en, 		0x1);
						DE_P1R_L9B0_WrFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_enable,	0x1);
						DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_r,0x1);
						DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_g,0x1);
						DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_b,0x1);
						DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_02);
						DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02, gmc_mode,		0x2);//normal mode
						DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_02);
					}
				}
			}
			else
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(CCM)))
				{
					PE_CCM_DBG_PRINT("wb,degamma,gamma,pxl_rep_en : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9_RdFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, wb_en, 			0x0);
						DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, degamma_en, 		0x0);
						DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, gamma_en, 			0x0);
						DE_P1L_L9_WrFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_enable,	0x0);
						DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_02);
						DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02, gmc_mode,			0x0);//read mode
						DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_02);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9_RdFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, wb_en, 			0x0);
						DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, degamma_en, 		0x0);
						DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, gamma_en, 			0x0);
						DE_P1R_L9_WrFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_enable,	0x0);
						DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_02);
						DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02, gmc_mode,			0x0);//read mode
						DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_02);
					}
				}
				else
				{
					PE_CCM_DBG_PRINT("wb,degamma,gamma,pxl_rep_en,pxl_rep_dis : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9_RdFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, wb_en, 			0x1);
						DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, degamma_en, 		0x1);
						DE_P1L_L9_Wr01(pe1_l_wb_ctrl_00, gamma_en, 			0x1);
						DE_P1L_L9_WrFL(pe1_l_wb_ctrl_00);
						DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_enable,	0x1);
						DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_r,0x1);
						DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_g,0x1);
						DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_b,0x1);
						DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_00);
						DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_02);
						DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02, gmc_mode,			0x2);//normal mode
						DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_02);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9_RdFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, wb_en, 			0x1);
						DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, degamma_en, 		0x1);
						DE_P1R_L9_Wr01(pe1_r_wb_ctrl_00, gamma_en, 			0x1);
						DE_P1R_L9_WrFL(pe1_r_wb_ctrl_00);
						DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_enable,	0x1);
						DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_r,0x1);
						DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_g,0x1);
						DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_b,0x1);
						DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_00);
						DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_02);
						DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02, gmc_mode,			0x2);//normal mode
						DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_02);
					}
				}
			}
		}
	}while(0);
	return ret;
}
/**
 * set gamma correction lut
 *
 * @param   *pstParams [in] LX_PE_CCM_GAMMALUT_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int PE_CCM_SetGammaLut(LX_PE_CCM_GAMMALUT_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CCM_CHECK_CODE(!(pstParams->lut_size),return RET_ERROR,"[%s,%d] lut_size is zero.\n",__F__,__L__);
		PE_CCM_CHECK_CODE(!(pstParams->lut_data),return RET_ERROR,"[%s,%d] lut_data is null.\n",__F__,__L__);

		PE_CCM_DBG_PRINT("_g_ccm_lut_use_ddr : %s.\n",_g_ccm_lut_use_ddr? "yes":"no");
		if(_g_ccm_lut_use_ddr)
		{
			win_id=pstParams->win_id;
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->win_id=LX_PE_WIN_MAIN;
				ret = PE_CCM_SetGammaLutByDdr(pstParams);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetGammaLutByDdr() error.\n",__F__,__L__);
			}
			if(PE_CHECK_SUB(win_id))
			{
				if(win_id==LX_PE_WIN_ALL)
				{
					OS_MsecSleep(30);	// 20111125, sj.youm, 30m : experimental value for 1080p 24hz SBS
				}
				pstParams->win_id=LX_PE_WIN_SUB;
				ret = PE_CCM_SetGammaLutByDdr(pstParams);
				PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetGammaLutByDdr() error.\n",__F__,__L__);
			}
		}
		else
		{
			ret = PE_CCM_SetGammaLutByReg(pstParams);
			PE_CCM_CHECK_CODE(ret,break,"[%s,%d] PE_CCM_SetGammaLutByReg() error.\n",__F__,__L__);
		}
	}while(0);
	return ret;
}

/**
 * get gamma correction lut
 *
 * @param   *pstParams [in/out] LX_PE_CCM_GAMMALUT_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int PE_CCM_GetGammaLut(LX_PE_CCM_GAMMALUT_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count = 0;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CCM_CHECK_CODE(!(pstParams->lut_size),return RET_ERROR,"[%s,%d] lut_size is zero.\n",__F__,__L__);
		PE_CCM_CHECK_CODE(!(pstParams->lut_data),return RET_ERROR,"[%s,%d] lut_data is null.\n",__F__,__L__);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_02);
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x0);//read mode
				DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_02);
				DE_P1L_L9B0_RdFL(pe1_l_gmc_ia_ctrl);
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode off
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_ctrl,	capture_addr,	0x0);
				DE_P1L_L9B0_WrFL(pe1_l_gmc_ia_ctrl);

				OS_MsecSleep(20);	// 20110530, sj.youm, 20m : experimental value
				for(count=0;count<pstParams->lut_size;count++)
				{
					DE_P1L_L9B0_RdFL(pe1_l_gmc_ia_data);
					pstParams->lut_data[count] = DE_P1L_L9B0_Rd(pe1_l_gmc_ia_data);
				}
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x2);
				DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_02);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_02);
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x0);//read mode
				DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_02);
				DE_P1R_L9B0_RdFL(pe1_r_gmc_ia_ctrl);
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode off
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_ctrl,	capture_addr,	0x0);
				DE_P1R_L9B0_WrFL(pe1_r_gmc_ia_ctrl);

				OS_MsecSleep(20);	// 20110530, sj.youm, 20m : experimental value
				for(count=0;count<pstParams->lut_size;count++)
				{
					DE_P1R_L9B0_RdFL(pe1_r_gmc_ia_data);
					pstParams->lut_data[count] = DE_P1R_L9B0_Rd(pe1_r_gmc_ia_data);
				}
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x2);
				DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_02);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_02);
				DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x0);//read mode
				DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_02);
				DE_P1L_L9_RdFL(pe1_l_gmc_ia_ctrl);
				DE_P1L_L9_Wr01(pe1_l_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode off
				DE_P1L_L9_Wr01(pe1_l_gmc_ia_ctrl,	capture_addr,	0x0);
				DE_P1L_L9_WrFL(pe1_l_gmc_ia_ctrl);

				OS_MsecSleep(20);	// 20110530, sj.youm, 20m : experimental value
				for(count=0;count<pstParams->lut_size;count++)
				{
					DE_P1L_L9_RdFL(pe1_l_gmc_ia_data);
					pstParams->lut_data[count] = DE_P1L_L9_Rd(pe1_l_gmc_ia_data);
				}
				DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x2);
				DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_02);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_02);
				DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x0);//read mode
				DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_02);
				DE_P1R_L9_RdFL(pe1_r_gmc_ia_ctrl);
				DE_P1R_L9_Wr01(pe1_r_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode off
				DE_P1R_L9_Wr01(pe1_r_gmc_ia_ctrl,	capture_addr,	0x0);
				DE_P1R_L9_WrFL(pe1_r_gmc_ia_ctrl);

				OS_MsecSleep(20);	// 20110530, sj.youm, 20m : experimental value
				for(count=0;count<pstParams->lut_size;count++)
				{
					DE_P1R_L9_RdFL(pe1_r_gmc_ia_data);
					pstParams->lut_data[count] = DE_P1R_L9_Rd(pe1_r_gmc_ia_data);
				}
				DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x2);
				DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_02);
			}
		}
	} while (0);
	return ret;
}

static int PE_CCM_SetPixelReplacement(LX_PE_CCM_PIXEL_REPLACE_T *pstParams)
{
	int ret = RET_OK;
	UINT32 pxl_en,r_rep_off,g_rep_off,b_rep_off;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CCM_DBG_PRINT("set pstParams[%d] : r:%d,g:%d,b:%d\n",pstParams->win_id,\
			pstParams->pxl_rep_r,pstParams->pxl_rep_g,pstParams->pxl_rep_b);

		r_rep_off = (pstParams->pxl_rep_r)? 0x0:0x1;
		g_rep_off = (pstParams->pxl_rep_g)? 0x0:0x1;
		b_rep_off = (pstParams->pxl_rep_b)? 0x0:0x1;
		pxl_en = (pstParams->pxl_rep_r | pstParams->pxl_rep_g | pstParams->pxl_rep_b)? 0x1:0x0;

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_00);
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_enable,		pxl_en);
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
				DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_00);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_00);
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_enable,		pxl_en);
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
				DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_00);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_00);
				DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_enable,		pxl_en);
				DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
				DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_00);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_00);
				DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_enable,		pxl_en);
				DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
				DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_00);
			}
		}
	}while(0);

	return ret;
}
static int PE_CCM_GetPixelReplacement(LX_PE_CCM_PIXEL_REPLACE_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 r_rep_off=1,g_rep_off=1,b_rep_off=1;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_00);
				DE_P1L_L9B0_Rd01(pe1_l_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1L_L9B0_Rd01(pe1_l_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1L_L9B0_Rd01(pe1_l_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_00);
				DE_P1R_L9B0_Rd01(pe1_r_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1R_L9B0_Rd01(pe1_r_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1R_L9B0_Rd01(pe1_r_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_00);
				DE_P1L_L9_Rd01(pe1_l_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1L_L9_Rd01(pe1_l_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1L_L9_Rd01(pe1_l_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_00);
				DE_P1R_L9_Rd01(pe1_r_gmc_ctrl_00, pxl_rep_disable_r,	r_rep_off);
				DE_P1R_L9_Rd01(pe1_r_gmc_ctrl_00, pxl_rep_disable_g,	g_rep_off);
				DE_P1R_L9_Rd01(pe1_r_gmc_ctrl_00, pxl_rep_disable_b,	b_rep_off);
			}
		}
		pstParams->pxl_rep_r = (r_rep_off)? 0x0:0x1;
		pstParams->pxl_rep_g = (g_rep_off)? 0x0:0x1;
		pstParams->pxl_rep_b = (b_rep_off)? 0x0:0x1;

		PE_CCM_DBG_PRINT("get pstParams[%d] : r:%d,g:%d,b:%d\n",pstParams->win_id,\
			pstParams->pxl_rep_r,pstParams->pxl_rep_g,pstParams->pxl_rep_b);		
	}while(0);


	return ret;
}

static int PE_CCM_SetWhiteBalance(LX_PE_CCM_WB_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CCM_DBG_PRINT("set pstParams[%d] : gain:%d,%d,%d, offset:%d,%d,%d\n",\
			pstParams->win_id,pstParams->r_gain,pstParams->g_gain,pstParams->b_gain,\
			pstParams->r_offset,pstParams->g_offset,pstParams->b_offset);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_wb_ctrl_01);
				DE_P1L_L9B0_RdFL(pe1_l_wb_ctrl_02);
				DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_01, user_ctrl_g_gain,	GET_BITS(pstParams->g_gain,0,8));
				DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_01, user_ctrl_b_gain,	GET_BITS(pstParams->b_gain,0,8));
				DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_01, user_ctrl_r_gain,	GET_BITS(pstParams->r_gain,0,8));
				DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_02, user_ctrl_g_offset,	GET_BITS(pstParams->g_offset,0,8));
				DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_02, user_ctrl_b_offset,	GET_BITS(pstParams->b_offset,0,8));
				DE_P1L_L9B0_Wr01(pe1_l_wb_ctrl_02, user_ctrl_r_offset,	GET_BITS(pstParams->r_offset,0,8));
				DE_P1L_L9B0_WrFL(pe1_l_wb_ctrl_01);
				DE_P1L_L9B0_WrFL(pe1_l_wb_ctrl_02);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_wb_ctrl_01);
				DE_P1R_L9B0_RdFL(pe1_r_wb_ctrl_02);
				DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_01, user_ctrl_g_gain,	GET_BITS(pstParams->g_gain,0,8));
				DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_01, user_ctrl_b_gain,	GET_BITS(pstParams->b_gain,0,8));
				DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_01, user_ctrl_r_gain,	GET_BITS(pstParams->r_gain,0,8));
				DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_02, user_ctrl_g_offset,	GET_BITS(pstParams->g_offset,0,8));
				DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_02, user_ctrl_b_offset,	GET_BITS(pstParams->b_offset,0,8));
				DE_P1R_L9B0_Wr01(pe1_r_wb_ctrl_02, user_ctrl_r_offset,	GET_BITS(pstParams->r_offset,0,8));
				DE_P1R_L9B0_WrFL(pe1_r_wb_ctrl_01);
				DE_P1R_L9B0_WrFL(pe1_r_wb_ctrl_02);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_wb_ctrl_02);
				DE_P1L_L9_RdFL(pe1_l_wb_ctrl_03);
				DE_P1L_L9_Wr01(pe1_l_wb_ctrl_02, user_ctrl_g_gain,		GET_BITS(pstParams->g_gain,0,8));
				DE_P1L_L9_Wr01(pe1_l_wb_ctrl_02, user_ctrl_b_gain,		GET_BITS(pstParams->b_gain,0,8));
				DE_P1L_L9_Wr01(pe1_l_wb_ctrl_02, user_ctrl_r_gain,		GET_BITS(pstParams->r_gain,0,8));
				DE_P1L_L9_Wr01(pe1_l_wb_ctrl_03, user_ctrl_g_offset,	GET_BITS(pstParams->g_offset,0,8));
				DE_P1L_L9_Wr01(pe1_l_wb_ctrl_03, user_ctrl_b_offset,	GET_BITS(pstParams->b_offset,0,8));
				DE_P1L_L9_Wr01(pe1_l_wb_ctrl_03, user_ctrl_r_offset,	GET_BITS(pstParams->r_offset,0,8));
				DE_P1L_L9_WrFL(pe1_l_wb_ctrl_02);
				DE_P1L_L9_WrFL(pe1_l_wb_ctrl_03);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_wb_ctrl_02);
				DE_P1R_L9_RdFL(pe1_r_wb_ctrl_03);
				DE_P1R_L9_Wr01(pe1_r_wb_ctrl_02, user_ctrl_g_gain,		GET_BITS(pstParams->g_gain,0,8));
				DE_P1R_L9_Wr01(pe1_r_wb_ctrl_02, user_ctrl_b_gain,		GET_BITS(pstParams->b_gain,0,8));
				DE_P1R_L9_Wr01(pe1_r_wb_ctrl_02, user_ctrl_r_gain,		GET_BITS(pstParams->r_gain,0,8));
				DE_P1R_L9_Wr01(pe1_r_wb_ctrl_03, user_ctrl_g_offset,	GET_BITS(pstParams->g_offset,0,8));
				DE_P1R_L9_Wr01(pe1_r_wb_ctrl_03, user_ctrl_b_offset,	GET_BITS(pstParams->b_offset,0,8));
				DE_P1R_L9_Wr01(pe1_r_wb_ctrl_03, user_ctrl_r_offset,	GET_BITS(pstParams->r_offset,0,8));
				DE_P1R_L9_WrFL(pe1_r_wb_ctrl_02);
				DE_P1R_L9_WrFL(pe1_r_wb_ctrl_03);
			}
		}
	}while(0);

	return ret;
}

static int PE_CCM_GetWhiteBalance(LX_PE_CCM_WB_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_wb_ctrl_01);
				DE_P1L_L9B0_RdFL(pe1_l_wb_ctrl_02);
				DE_P1L_L9B0_Rd01(pe1_l_wb_ctrl_01, user_ctrl_g_gain,	pstParams->g_gain);
				DE_P1L_L9B0_Rd01(pe1_l_wb_ctrl_01, user_ctrl_b_gain,	pstParams->b_gain);
				DE_P1L_L9B0_Rd01(pe1_l_wb_ctrl_01, user_ctrl_r_gain,	pstParams->r_gain);
				DE_P1L_L9B0_Rd01(pe1_l_wb_ctrl_02, user_ctrl_g_offset,	pstParams->g_offset);
				DE_P1L_L9B0_Rd01(pe1_l_wb_ctrl_02, user_ctrl_b_offset,	pstParams->b_offset);
				DE_P1L_L9B0_Rd01(pe1_l_wb_ctrl_02, user_ctrl_r_offset,	pstParams->r_offset);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_wb_ctrl_01);
				DE_P1R_L9B0_RdFL(pe1_r_wb_ctrl_02);
				DE_P1R_L9B0_Rd01(pe1_r_wb_ctrl_01, user_ctrl_g_gain,	pstParams->g_gain);
				DE_P1R_L9B0_Rd01(pe1_r_wb_ctrl_01, user_ctrl_b_gain,	pstParams->b_gain);
				DE_P1R_L9B0_Rd01(pe1_r_wb_ctrl_01, user_ctrl_r_gain,	pstParams->r_gain);
				DE_P1R_L9B0_Rd01(pe1_r_wb_ctrl_02, user_ctrl_g_offset,	pstParams->g_offset);
				DE_P1R_L9B0_Rd01(pe1_r_wb_ctrl_02, user_ctrl_b_offset,	pstParams->b_offset);
				DE_P1R_L9B0_Rd01(pe1_r_wb_ctrl_02, user_ctrl_r_offset,	pstParams->r_offset);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_wb_ctrl_02);
				DE_P1L_L9_RdFL(pe1_l_wb_ctrl_03);
				DE_P1L_L9_Rd01(pe1_l_wb_ctrl_02, user_ctrl_g_gain,		pstParams->g_gain);
				DE_P1L_L9_Rd01(pe1_l_wb_ctrl_02, user_ctrl_b_gain,		pstParams->b_gain);
				DE_P1L_L9_Rd01(pe1_l_wb_ctrl_02, user_ctrl_r_gain,		pstParams->r_gain);
				DE_P1L_L9_Rd01(pe1_l_wb_ctrl_03, user_ctrl_g_offset,	pstParams->g_offset);
				DE_P1L_L9_Rd01(pe1_l_wb_ctrl_03, user_ctrl_b_offset,	pstParams->b_offset);
				DE_P1L_L9_Rd01(pe1_l_wb_ctrl_03, user_ctrl_r_offset,	pstParams->r_offset);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_wb_ctrl_02);
				DE_P1R_L9_RdFL(pe1_r_wb_ctrl_03);
				DE_P1R_L9_Rd01(pe1_r_wb_ctrl_02, user_ctrl_g_gain,		pstParams->g_gain);
				DE_P1R_L9_Rd01(pe1_r_wb_ctrl_02, user_ctrl_b_gain,		pstParams->b_gain);
				DE_P1R_L9_Rd01(pe1_r_wb_ctrl_02, user_ctrl_r_gain,		pstParams->r_gain);
				DE_P1R_L9_Rd01(pe1_r_wb_ctrl_03, user_ctrl_g_offset,	pstParams->g_offset);
				DE_P1R_L9_Rd01(pe1_r_wb_ctrl_03, user_ctrl_b_offset,	pstParams->b_offset);
				DE_P1R_L9_Rd01(pe1_r_wb_ctrl_03, user_ctrl_r_offset,	pstParams->r_offset);
			}
		}
		PE_CCM_DBG_PRINT("get pstParams[%d] : gain:%d,%d,%d, offset:%d,%d,%d\n",\
			pstParams->win_id,pstParams->r_gain,pstParams->g_gain,pstParams->b_gain,\
			pstParams->r_offset,pstParams->g_offset,pstParams->b_offset);
	}while(0);

	return ret;
}
static int PE_CCM_SetAutoCorrection(LX_PE_CCM_AUTO_CR_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CCM_DBG_PRINT("set pstParams : en:%d, max_hist:%d,val_bins:%d, th0,1:%d,%d\n",\
			pstParams->enable,pstParams->th_max_hist,pstParams->th_valid_bins,\
			pstParams->adj_th0,pstParams->adj_th1);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_P1L_L9B0_Wr04(pe1_l_lrcr_ctrl_01,th_max_hist,	GET_BITS(pstParams->th_max_hist,0,8),\
												th_valid_bins,	GET_BITS(pstParams->th_valid_bins,0,8),\
												th1,			GET_BITS(pstParams->adj_th1,0,8),\
												th0,			GET_BITS(pstParams->adj_th0,0,8));
			PE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,enable,			pstParams->enable? 0x1:0x0);
			_g_ccm_auto_cr_en = pstParams->enable? 0x1:0x0;
		}
		else
		{
			PE_CCM_DBG_PRINT("nothing to do.\n");
		}
	}while(0);

	return ret;
}
static int PE_CCM_GetAutoCorrection(LX_PE_CCM_AUTO_CR_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_P1L_L9B0_Rd04(pe1_l_lrcr_ctrl_01,th_max_hist,	pstParams->th_max_hist,\
												th_valid_bins,	pstParams->th_valid_bins,\
												th1,			pstParams->adj_th1,\
												th0,			pstParams->adj_th0);
			PE_P1L_L9B0_Rd01(pe1_l_lrcr_ctrl_00,enable,			pstParams->enable);
		}
		else
		{
			PE_CCM_DBG_PRINT("nothing to do.\n");
		}
		PE_CCM_DBG_PRINT("get pstParams : en:%d, max_hist:%d,val_bins:%d, th0,1:%d,%d\n",\
			pstParams->enable,pstParams->th_max_hist,pstParams->th_valid_bins,\
			pstParams->adj_th0,pstParams->adj_th1);
	}while(0);

	return ret;
}
int PE_CCM_SetAutoCrGain(PE_CCM_AC_GAIN_INFO_T *pstParams)
{
	int ret = RET_OK;
	static UINT32 count = 0;
	UINT32 lrcr_gain = 0xff;
	static UINT32 prev_lrcr_gain = 0xff;
	do{
		CHECK_KNULL(pstParams);
		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(_g_ccm_auto_cr_en && \
				(pstParams->sat_status>PE_CCM_SAT_STATUS_TH || \
				pstParams->d3d_chk))
			{
				lrcr_gain = 0x0;
			}
			else
			{
				lrcr_gain = PE_CCM_LRCR_GAIN;
			}
			if(prev_lrcr_gain!=lrcr_gain)
			{
				PE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,reg_lrcr_gain,		lrcr_gain);
				PE_CCM_DBG_PRINT("set gain : pre:0x%x -> cur:0x%x, en:%d,s_st:%d,chk:%d\n\n",\
					prev_lrcr_gain,lrcr_gain,_g_ccm_auto_cr_en,\
					pstParams->sat_status,pstParams->d3d_chk);
				prev_lrcr_gain=lrcr_gain;
			}
			if(_g_ccm_trace)
			{
				if(count>240)
				{
					PE_CCM_DBG_PRINT("state : cur_gain:0x%x, en:%d,s_st:%d,chk:%d\n\n",\
						lrcr_gain,_g_ccm_auto_cr_en,pstParams->sat_status,pstParams->d3d_chk);
					count=0;
				}
				count++;
			}
		}
		else
		{
			PE_CCM_DBG_PRINT("nothing to do.\n");
		}
	}while(0);

	return ret;
}
/**
 * set gamma correction lut by ddr
 *
 * @param   *pstParams [in] LX_PE_CCM_GAMMALUT_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int PE_CCM_SetGammaLutByDdr(LX_PE_CCM_GAMMALUT_T *pstParams)
{
	int ret = RET_OK;
	VIDEO_DDR_BLOCK_HEAD_T gammaLutHead = {0};
	LX_DE_CH_MEM_T	*pFwMem      = NULL;
	UINT32          *lpDdrUpAddr = NULL;
	UINT32 			ddrUpAddr;
	UINT32 			regUpAddr;
	UINT32 			ddrStride = 512;
	int i;
	int ddrUpCount;

	PE_CCM_DBG_PRINT("set pstParams[%d] : lut_size:%d\n",pstParams->win_id, pstParams->lut_size);

	do {

		if(PE_CHIP_VER_GT(L9,B0))
		{
			ddrUpCount = 0;
			CHECK_KNULL(pstParams);
			CHECK_KNULL(pstParams->lut_data);
			if(!pstParams->lut_size) break;
	        pFwMem = &gpDeMem->fw[2];
	        CHECK_KNULL(pFwMem);
			ddrUpAddr = pFwMem->fw_base + CONV_ROW2BYTE(VIDEO_L9_FIRMWARE_ROW_OFST_DDR_UP);
	        lpDdrUpAddr = (UINT32 *)ioremap(ddrUpAddr, CONV_ROW2BYTE(VIDEO_L9_FIRMWARE_ROW_SIZE_DDR_UP));
			ddrUpAddr &= 0x0fffffff;
			ddrUpAddr |= 0x80000000;
	        CHECK_KNULL(lpDdrUpAddr);
			switch(pstParams->win_id)
			{
				case LX_PE_WIN_MAIN :
					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 6;

					DE_P1L_L9B0_RdFL(pe1_l_top_ctrl_00);
					DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_02);
					DE_P1L_L9B0_RdFL(pe1_l_gmc_ia_ctrl);
					DE_P1L_L9B0_Wr01(pe1_l_top_ctrl_00,	load_time,	  0x1);
					DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,	  0x1);
					DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_ctrl,	capture_ai,	  0x1);
					DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_ctrl,	capture_addr, 0x0);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9B.shdw.b0->pe1_l_top_ctrl_00;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9B.shdw.b0->pe1_l_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_IA_CTRL_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9B.shdw.b0->pe1_l_gmc_ia_ctrl;

					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 1;
					gammaLutHead.data_cnt   = pstParams->lut_size + 1;

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_IA_DATA_OFFSET;
					for (i=0;i<pstParams->lut_size;i++) lpDdrUpAddr[ddrUpCount++] = pstParams->lut_data[i];

					gammaLutHead.last_block = 1;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 4;

					DE_P1L_L9B0_Wr01(pe1_l_top_ctrl_00,	load_time,	0x0);
					DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,	0x2);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9B.shdw.b0->pe1_l_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9B.shdw.b0->pe1_l_top_ctrl_00;

					regUpAddr = (0x4 << 29) | GET_PVAL(ddrUpAddr, 11, 18);
					DE_MIB_L9B0_RdFL(mifb_saddr_param_update0_m);
					DE_MIB_L9B0_Wr01(mifb_saddr_param_update0_m, register_update_address0_m, GET_BITS(regUpAddr, 11, 21));
					DE_MIB_L9B0_Wr01(mifb_saddr_param_update0_m, load, 					 0x1);
					DE_MIB_L9B0_WrFL(mifb_saddr_param_update0_m);

					DE_OVL_L9B0_RdFL(ovl_ov_param_by_ddr);
					DE_OVL_L9B0_Wr01(ovl_ov_param_by_ddr, update_by_ddr,     1);
					DE_OVL_L9B0_Wr01(ovl_ov_param_by_ddr, endian_conversion, 0x0);
					DE_OVL_L9B0_Wr01(ovl_ov_param_by_ddr, vsize,             (ddrUpCount * sizeof(UINT32) + ddrStride - 1)/ddrStride);
					DE_OVL_L9B0_Wr01(ovl_ov_param_by_ddr, stride,            GET_BITS(ddrStride, 9, 2));
					DE_OVL_L9B0_WrFL(ovl_ov_param_by_ddr);
					break;

				case LX_PE_WIN_SUB :
					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 6;

					DE_P1R_L9B0_RdFL(pe1_r_top_ctrl_00);
					DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_02);
					DE_P1R_L9B0_RdFL(pe1_r_gmc_ia_ctrl);
					DE_P1R_L9B0_Wr01(pe1_r_top_ctrl_00,	load_time,	  0x1);
					DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,	  0x1);
					DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_ctrl,	capture_ai,	  0x1);
					DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_ctrl,	capture_addr, 0x0);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9B.shdw.b0->pe1_r_top_ctrl_00;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9B.shdw.b0->pe1_r_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_IA_CTRL_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9B.shdw.b0->pe1_r_gmc_ia_ctrl;

					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 1;
					gammaLutHead.data_cnt   = pstParams->lut_size + 1;

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_IA_DATA_OFFSET;
					for (i=0;i<pstParams->lut_size;i++) lpDdrUpAddr[ddrUpCount++] = pstParams->lut_data[i];

					gammaLutHead.last_block = 1;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 4;

					DE_P1R_L9B0_Wr01(pe1_r_top_ctrl_00,	load_time,	0x0);
					DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,	0x2);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9B.shdw.b0->pe1_r_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9B.shdw.b0->pe1_r_top_ctrl_00;

					regUpAddr = (0x4 << 29) | GET_PVAL(ddrUpAddr, 11, 18);
					DE_MID_L9B0_RdFL(mifd_saddr_param_update0_m);
					DE_MID_L9B0_Wr01(mifd_saddr_param_update0_m, register_update_address0_m, GET_BITS(regUpAddr, 11, 21));
					DE_MID_L9B0_Wr01(mifd_saddr_param_update0_m, load, 					 0x1);
					DE_MID_L9B0_WrFL(mifd_saddr_param_update0_m);

					DE_OVR_L9B0_RdFL(ovr_ov_param_by_ddr);
					DE_OVR_L9B0_Wr01(ovr_ov_param_by_ddr, update_by_ddr,     1);
					DE_OVR_L9B0_Wr01(ovr_ov_param_by_ddr, endian_conversion, 0x0);
					DE_OVR_L9B0_Wr01(ovr_ov_param_by_ddr, vsize,             (ddrUpCount * sizeof(UINT32) + ddrStride - 1)/ddrStride);
					DE_OVR_L9B0_Wr01(ovr_ov_param_by_ddr, stride,            GET_BITS(ddrStride, 9, 2));
					DE_OVR_L9B0_WrFL(ovr_ov_param_by_ddr);
					break;

				default :
					BREAK_WRONG(pstParams->win_id);
			}
			if(lpDdrUpAddr) iounmap((void*)lpDdrUpAddr);
		}
		else
		{
			ddrUpCount = 0;
			CHECK_KNULL(pstParams);
			CHECK_KNULL(pstParams->lut_data);
			if(!pstParams->lut_size) break;
	        pFwMem = &gpDeMem->fw[2];
	        CHECK_KNULL(pFwMem);
			ddrUpAddr = pFwMem->fw_base + CONV_ROW2BYTE(VIDEO_L9_FIRMWARE_ROW_OFST_DDR_UP);
	        lpDdrUpAddr = (UINT32 *)ioremap(ddrUpAddr, CONV_ROW2BYTE(VIDEO_L9_FIRMWARE_ROW_SIZE_DDR_UP));
			ddrUpAddr &= 0x0fffffff;
			ddrUpAddr |= 0x80000000;
	        CHECK_KNULL(lpDdrUpAddr);
			switch(pstParams->win_id)
			{
				case LX_PE_WIN_MAIN :
					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 6;

					DE_P1L_L9_RdFL(pe1_l_top_ctrl_00);
					DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_02);
					DE_P1L_L9_RdFL(pe1_l_gmc_ia_ctrl);
					DE_P1L_L9_Wr01(pe1_l_top_ctrl_00,	load_time,	  0x1);
					DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,	  0x1);
					DE_P1L_L9_Wr01(pe1_l_gmc_ia_ctrl,	capture_ai,	  0x1);
					DE_P1L_L9_Wr01(pe1_l_gmc_ia_ctrl,	capture_addr, 0x0);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9.shdw.a0->pe1_l_top_ctrl_00;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9.shdw.a0->pe1_l_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_IA_CTRL_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9.shdw.a0->pe1_l_gmc_ia_ctrl;

					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 1;
					gammaLutHead.data_cnt   = pstParams->lut_size + 1;

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_IA_DATA_OFFSET;
					for (i=0;i<pstParams->lut_size;i++) lpDdrUpAddr[ddrUpCount++] = pstParams->lut_data[i];

					gammaLutHead.last_block = 1;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 4;

					DE_P1L_L9_Wr01(pe1_l_top_ctrl_00,	load_time,	0x0);
					DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,	0x2);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9.shdw.a0->pe1_l_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_L_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1L_L9.shdw.a0->pe1_l_top_ctrl_00;

					regUpAddr = (0x4 << 29) | GET_PVAL(ddrUpAddr, 11, 18);
					DE_MIB_L9_RdFL(mifb_saddr_param_update0_m);
					DE_MIB_L9_Wr01(mifb_saddr_param_update0_m, register_update_address0_m, GET_BITS(regUpAddr, 11, 21));
					DE_MIB_L9_Wr01(mifb_saddr_param_update0_m, load, 					 0x1);
					DE_MIB_L9_WrFL(mifb_saddr_param_update0_m);

					DE_OVL_L9_RdFL(ovl_ov_param_by_ddr);
					DE_OVL_L9_Wr01(ovl_ov_param_by_ddr, update_by_ddr,     1);
					DE_OVL_L9_Wr01(ovl_ov_param_by_ddr, endian_conversion, 0x0);
					DE_OVL_L9_Wr01(ovl_ov_param_by_ddr, vsize,             (ddrUpCount * sizeof(UINT32) + ddrStride - 1)/ddrStride);
					DE_OVL_L9_Wr01(ovl_ov_param_by_ddr, stride,            GET_BITS(ddrStride, 9, 2));
					DE_OVL_L9_WrFL(ovl_ov_param_by_ddr);
					break;

				case LX_PE_WIN_SUB :
					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 6;

					DE_P1R_L9_RdFL(pe1_r_top_ctrl_00);
					DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_02);
					DE_P1R_L9_RdFL(pe1_r_gmc_ia_ctrl);
					DE_P1R_L9_Wr01(pe1_r_top_ctrl_00,	load_time,	  0x1);
					DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,	  0x1);
					DE_P1R_L9_Wr01(pe1_r_gmc_ia_ctrl,	capture_ai,	  0x1);
					DE_P1R_L9_Wr01(pe1_r_gmc_ia_ctrl,	capture_addr, 0x0);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9.shdw.a0->pe1_r_top_ctrl_00;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9.shdw.a0->pe1_r_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_IA_CTRL_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9.shdw.a0->pe1_r_gmc_ia_ctrl;

					gammaLutHead.last_block = 0;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 1;
					gammaLutHead.data_cnt   = pstParams->lut_size + 1;

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_IA_DATA_OFFSET;
					for (i=0;i<pstParams->lut_size;i++) lpDdrUpAddr[ddrUpCount++] = pstParams->lut_data[i];

					gammaLutHead.last_block = 1;
					gammaLutHead.auto_inc   = 0;
					gammaLutHead.block_mode = 0;
					gammaLutHead.data_cnt   = 4;

					DE_P1R_L9_Wr01(pe1_r_top_ctrl_00,	load_time,	0x0);
					DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,	0x2);

					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gammaLutHead;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_GMC_CTRL_02_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9.shdw.a0->pe1_r_gmc_ctrl_02;
					lpDdrUpAddr[ddrUpCount++] =  PE1_R_TOP_CTRL_00_OFFSET;
					lpDdrUpAddr[ddrUpCount++] = *(UINT32 *)&gDE_P1R_L9.shdw.a0->pe1_r_top_ctrl_00;

					regUpAddr = (0x4 << 29) | GET_PVAL(ddrUpAddr, 11, 18);
					DE_MID_L9_RdFL(mifd_saddr_param_update0_m);
					DE_MID_L9_Wr01(mifd_saddr_param_update0_m, register_update_address0_m, GET_BITS(regUpAddr, 11, 21));
					DE_MID_L9_Wr01(mifd_saddr_param_update0_m, load, 					 0x1);
					DE_MID_L9_WrFL(mifd_saddr_param_update0_m);

					DE_OVR_L9_RdFL(ovr_ov_param_by_ddr);
					DE_OVR_L9_Wr01(ovr_ov_param_by_ddr, update_by_ddr,     1);
					DE_OVR_L9_Wr01(ovr_ov_param_by_ddr, endian_conversion, 0x0);
					DE_OVR_L9_Wr01(ovr_ov_param_by_ddr, vsize,             (ddrUpCount * sizeof(UINT32) + ddrStride - 1)/ddrStride);
					DE_OVR_L9_Wr01(ovr_ov_param_by_ddr, stride,            GET_BITS(ddrStride, 9, 2));
					DE_OVR_L9_WrFL(ovr_ov_param_by_ddr);
					break;

				default :
					BREAK_WRONG(pstParams->win_id);
			}
			if(lpDdrUpAddr) iounmap((void*)lpDdrUpAddr);
		}
	} while (0);

	return ret;
}
/**
 * set gamma correction lut by register
 *
 * @param   *pstParams [in] LX_PE_CCM_GAMMALUT_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int PE_CCM_SetGammaLutByReg(LX_PE_CCM_GAMMALUT_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count = 0;
	UINT32 lutdata;

	PE_CCM_DBG_PRINT("set pstParams[%d] : lut_size:%d\n",pstParams->win_id, pstParams->lut_size);

	if(PE_CHIP_VER_GT(L9,B0))
	{
		if(PE_CHECK_MAIN(pstParams->win_id))
		{
			DE_P1L_L9B0_RdFL(pe1_l_gmc_ctrl_02);
			DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x1);//write mode
			DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_02);
			DE_P1L_L9B0_RdFL(pe1_l_gmc_ia_data);
			DE_P1L_L9B0_RdFL(pe1_l_gmc_ia_ctrl);
			DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode
			DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_ctrl,	capture_addr,	0x0);
			DE_P1L_L9B0_WrFL(pe1_l_gmc_ia_ctrl);
			//msleep(20);
			DE_P1L_L9B0_RdFL(pe1_l_top_ctrl_00);
			DE_P1L_L9B0_Wr01(pe1_l_top_ctrl_00,	load_time,		0x1);
			DE_P1L_L9B0_WrFL(pe1_l_top_ctrl_00);
			for(count=0;count<pstParams->lut_size;count++)
			{
				lutdata = pstParams->lut_data[count];
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_data,	lut_data_g, GET_BITS(lutdata,  0, 10));
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_data,	lut_data_b, GET_BITS(lutdata, 10, 10));
				DE_P1L_L9B0_Wr01(pe1_l_gmc_ia_data,	lut_data_r, GET_BITS(lutdata, 20, 10));
				DE_P1L_L9B0_WrFL(pe1_l_gmc_ia_data);
			}
			DE_P1L_L9B0_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x2);//enable mode
			DE_P1L_L9B0_WrFL(pe1_l_gmc_ctrl_02);
			DE_P1L_L9B0_RdFL(pe1_l_top_ctrl_00);
			DE_P1L_L9B0_Wr01(pe1_l_top_ctrl_00,	load_time,		0x0);
			DE_P1L_L9B0_WrFL(pe1_l_top_ctrl_00);
		}
		if(PE_CHECK_SUB(pstParams->win_id))
		{
			DE_P1R_L9B0_RdFL(pe1_r_gmc_ctrl_02);
			DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x1);//write mode
			DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_02);
			DE_P1R_L9B0_RdFL(pe1_r_gmc_ia_data);
			DE_P1R_L9B0_RdFL(pe1_r_gmc_ia_ctrl);
			DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode
			DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_ctrl,	capture_addr,	0x0);
			DE_P1R_L9B0_WrFL(pe1_r_gmc_ia_ctrl);
			//msleep(20);
			DE_P1R_L9B0_RdFL(pe1_r_top_ctrl_00);
			DE_P1R_L9B0_Wr01(pe1_r_top_ctrl_00,	load_time,		0x1);
			DE_P1R_L9B0_WrFL(pe1_r_top_ctrl_00);
			for(count=0;count<pstParams->lut_size;count++)
			{
				lutdata = pstParams->lut_data[count];
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_data,	lut_data_g, GET_BITS(lutdata,  0, 10));
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_data,	lut_data_b, GET_BITS(lutdata, 10, 10));
				DE_P1R_L9B0_Wr01(pe1_r_gmc_ia_data,	lut_data_r, GET_BITS(lutdata, 20, 10));
				DE_P1R_L9B0_WrFL(pe1_r_gmc_ia_data);
			}
			DE_P1R_L9B0_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x2);//enable mode
			DE_P1R_L9B0_WrFL(pe1_r_gmc_ctrl_02);
			DE_P1R_L9B0_RdFL(pe1_r_top_ctrl_00);
			DE_P1R_L9B0_Wr01(pe1_r_top_ctrl_00,	load_time,		0x0);
			DE_P1R_L9B0_WrFL(pe1_r_top_ctrl_00);
		}
	}
	else
	{
		if(PE_CHECK_MAIN(pstParams->win_id))
		{
			DE_P1L_L9_RdFL(pe1_l_gmc_ctrl_02);
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x1);//write mode
			DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_02);
			DE_P1L_L9_RdFL(pe1_l_gmc_ia_data);
			DE_P1L_L9_RdFL(pe1_l_gmc_ia_ctrl);
			DE_P1L_L9_Wr01(pe1_l_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode
			DE_P1L_L9_Wr01(pe1_l_gmc_ia_ctrl,	capture_addr,	0x0);
			DE_P1L_L9_WrFL(pe1_l_gmc_ia_ctrl);
			//msleep(20);
			DE_P1L_L9_RdFL(pe1_l_top_ctrl_00);
			DE_P1L_L9_Wr01(pe1_l_top_ctrl_00,	load_time,		0x1);
			DE_P1L_L9_WrFL(pe1_l_top_ctrl_00);
			for(count=0;count<pstParams->lut_size;count++)
			{
				lutdata = pstParams->lut_data[count];
				DE_P1L_L9_Wr01(pe1_l_gmc_ia_data,	lut_data_g, GET_BITS(lutdata,  0, 10));
				DE_P1L_L9_Wr01(pe1_l_gmc_ia_data,	lut_data_b, GET_BITS(lutdata, 10, 10));
				DE_P1L_L9_Wr01(pe1_l_gmc_ia_data,	lut_data_r, GET_BITS(lutdata, 20, 10));
				DE_P1L_L9_WrFL(pe1_l_gmc_ia_data);
			}
			DE_P1L_L9_Wr01(pe1_l_gmc_ctrl_02,	gmc_mode,		0x2);//enable mode
			DE_P1L_L9_WrFL(pe1_l_gmc_ctrl_02);
			DE_P1L_L9_RdFL(pe1_l_top_ctrl_00);
			DE_P1L_L9_Wr01(pe1_l_top_ctrl_00,	load_time,		0x0);
			DE_P1L_L9_WrFL(pe1_l_top_ctrl_00);
		}
		if(PE_CHECK_SUB(pstParams->win_id))
		{
			DE_P1R_L9_RdFL(pe1_r_gmc_ctrl_02);
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x1);//write mode
			DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_02);
			DE_P1R_L9_RdFL(pe1_r_gmc_ia_data);
			DE_P1R_L9_RdFL(pe1_r_gmc_ia_ctrl);
			DE_P1R_L9_Wr01(pe1_r_gmc_ia_ctrl,	capture_ai,		0x1);//ai mode
			DE_P1R_L9_Wr01(pe1_r_gmc_ia_ctrl,	capture_addr,	0x0);
			DE_P1R_L9_WrFL(pe1_r_gmc_ia_ctrl);
			//msleep(20);
			DE_P1R_L9_RdFL(pe1_r_top_ctrl_00);
			DE_P1R_L9_Wr01(pe1_r_top_ctrl_00,	load_time,		0x1);
			DE_P1R_L9_WrFL(pe1_r_top_ctrl_00);
			for(count=0;count<pstParams->lut_size;count++)
			{
				lutdata = pstParams->lut_data[count];
				DE_P1R_L9_Wr01(pe1_r_gmc_ia_data,	lut_data_g, GET_BITS(lutdata,  0, 10));
				DE_P1R_L9_Wr01(pe1_r_gmc_ia_data,	lut_data_b, GET_BITS(lutdata, 10, 10));
				DE_P1R_L9_Wr01(pe1_r_gmc_ia_data,	lut_data_r, GET_BITS(lutdata, 20, 10));
				DE_P1R_L9_WrFL(pe1_r_gmc_ia_data);
			}
			DE_P1R_L9_Wr01(pe1_r_gmc_ctrl_02,	gmc_mode,		0x2);//enable mode
			DE_P1R_L9_WrFL(pe1_r_gmc_ctrl_02);
			DE_P1R_L9_RdFL(pe1_r_top_ctrl_00);
			DE_P1R_L9_Wr01(pe1_r_top_ctrl_00,	load_time,		0x0);
			DE_P1R_L9_WrFL(pe1_r_top_ctrl_00);
		}
	}

	return ret;
}

