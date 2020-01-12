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

/** @file pe_shp.c
 *
 *  driver for picture enhance sharpness functions. ( used only within kdriver )
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

#include "de_msc_reg_l9.h"		// base addr : 0x1100, 0x3100

#include "pe_reg.h"
#include "pe_cfg.h"
#include "pe_def.h"
#include "pe_etc.h"
#include "pe_shp.h"
#include "pe_shp_coeff.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_SHP_ERROR	printk

#define PE_SHP_DBG_PRINT(fmt,args...)	if(_g_shp_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_SHP_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_SHP_ERROR(fmt,##args);_action;}}

#define memset0(_var)	memset( &(_var), 0x00, sizeof(_var))

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
/**
*	avoid double setting scaler filter
*/
typedef struct
{
	UINT8 h_y;	///< horizontal luma
	UINT8 v_y;	///< vertical luma
	UINT8 h_c;	///< horizontal chroma
	UINT8 v_c;	///< vertical chroma
}
PE_SHP_SETFLAG_T;

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

extern DE_MSL_REG_L9_T gDE_MSL_L9;
extern DE_MSR_REG_L9_T gDE_MSR_L9;

extern UINT8 g_pe_use_fw;

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int PE_SHP_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_SHP_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_SHP_SetResolutionEnhancement(LX_PE_SHP_RES_T *pstParams);
static int PE_SHP_GetResolutionEnhancement(LX_PE_SHP_RES_T *pstParams);
static int PE_SHP_SetEdgeEnhancement(LX_PE_SHP_EDGE_T *pstParams);
static int PE_SHP_GetEdgeEnhancement(LX_PE_SHP_EDGE_T *pstParams);
static int PE_SHP_SetResEnhanceEnable(LX_PE_SHP_RES_ENABLE_T *pstParams);
static int PE_SHP_GetResEnhanceEnable(LX_PE_SHP_RES_ENABLE_T *pstParams);
static int PE_SHP_CheckValidScalerFilterValue(LX_PE_SHP_SCLFILTER_T *pstParams);
static int PE_SHP_SetScalerFilterCoeff(LX_PE_SHP_SCLFILTER_T *pstParams);
static int PE_SHP_GetScalerFilterCoeff(LX_PE_SHP_SCLFILTER_T *pstParams);
static PE_SHP_SETFLAG_T PE_SHP_GetDoubleSettingsFlag(LX_PE_WIN_ID win_id, LX_PE_SHP_SCLFILTER_T *pstParams);
static int PE_SHP_SetScalerFilterCoeffByReg(UINT32 base_addr, PE_SHP_SETFLAG_T flag, LX_PE_SHP_SCLFILTER_T *pstParams);
static int PE_SHP_SetSclSettingCnt(PE_SHP_SETFLAG_T flag, LX_PE_SHP_SCLFILTER_T *pstParams);
static int PE_SHP_SetReCommonCtrl(LX_PE_SHP_RE_CMN_T *pstParams);
static int PE_SHP_GetReCommonCtrl(LX_PE_SHP_RE_CMN_T *pstParams);
static int PE_SHP_SetReHorizontalCtrl(LX_PE_SHP_RE_HOR_T *pstParams);
static int PE_SHP_GetReHorizontalCtrl(LX_PE_SHP_RE_HOR_T *pstParams);
static int PE_SHP_SetReVerticalCtrl(LX_PE_SHP_RE_VER_T *pstParams);
static int PE_SHP_GetReVerticalCtrl(LX_PE_SHP_RE_VER_T *pstParams);
static int PE_SHP_SetReMiscCtrl(LX_PE_SHP_RE_MISC_T *pstParams);
static int PE_SHP_GetReMiscCtrl(LX_PE_SHP_RE_MISC_T *pstParams);
static int PE_SHP_SetEdgeEnhanceCtrl(LX_PE_SHP_EE_T *pstParams);
static int PE_SHP_GetEdgeEnhanceCtrl(LX_PE_SHP_EE_T *pstParams);
static int PE_SHP_SetDetailEnhanceCtrl(LX_PE_SHP_DE_T *pstParams);
static int PE_SHP_GetDetailEnhanceCtrl(LX_PE_SHP_DE_T *pstParams);
#ifdef PE_PROTECTION_SCL_12TAP_ON_HD_INPUT
static void PE_SHP_WrMscCoeff(const PE_REG_PARAM_T *reg, UINT32 base_addr, UINT32 table_size);
static int PE_SHP_SetScalerFilterCoeffOnIsr(UINT32 base_addr, PE_SHP_SETFLAG_T flag, LX_PE_SHP_SCLFILTER_T *pstParams);
#endif

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_shp_trace=0x0;	//default should be off.
static LX_PE_SHP_SCLFILTER_T _g_shp_scl_filter[LX_PE_WIN_NUM];
#ifdef PE_PROTECTION_SCL_12TAP_ON_HD_INPUT
static LX_PE_SHP_SCLFILTER_T _g_cur_scl_filter[LX_PE_WIN_NUM]={\
		{0xff,0,0,0,0,0,0,0,0,0,0},{0xff,0,0,0,0,0,0,0,0,0,0}};
#endif
static LX_PE_SHP_RES_ENABLE_T _g_shp_res_enable[LX_PE_WIN_NUM];
static PE_FWI_L9_SHP_CORING_CTRL_05 _g_shp_coring_ctrl_05[LX_PE_WIN_NUM];
static PE_FWI_L9_SHP_CORING_CTRL_06 _g_shp_coring_ctrl_06[LX_PE_WIN_NUM];
static PE_FWI_L9_SHP_CORING_CTRL_09 _g_shp_coring_ctrl_09[LX_PE_WIN_NUM];
static PE_FWI_L9_SHP_DP_CTRL_01 _g_shp_dp_ctrl_01[LX_PE_WIN_NUM];

/*----------------------------------------------------------------------------------------
	Implementation Group
----------------------------------------------------------------------------------------*/

/**
 * init shp
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_SHP_Init(void)
{
	int ret = RET_OK;
	UINT8 i;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("init global values.\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_shp_scl_filter[i].win_id			= i;
				_g_shp_scl_filter[i].hor_y_tap_type	= 0xff;
				_g_shp_scl_filter[i].hor_c_tap_type	= 0xff;
				_g_shp_scl_filter[i].ver_y_tap_type	= 0xff;
				_g_shp_scl_filter[i].ver_c_tap_type	= 0xff;
				_g_shp_scl_filter[i].fir_luma_type	= 0xff;
				_g_shp_scl_filter[i].fir_chroma_type= 0xff;
				_g_shp_scl_filter[i].hor_luma_id	= 0xff;
				_g_shp_scl_filter[i].ver_luma_id	= 0xff;
				_g_shp_scl_filter[i].hor_chroma_id	= 0xff;
				_g_shp_scl_filter[i].ver_chroma_id	= 0xff;
				_g_shp_res_enable[i].win_id			= i;
				_g_shp_res_enable[i].enable			= 0xff;
				_g_shp_coring_ctrl_05[i].gt_th0			= 0x20;	//chip default
				_g_shp_coring_ctrl_05[i].gt_th1			= 0x40;	//chip default
				_g_shp_coring_ctrl_06[i].gt_th0a		= 0x40;	//chip default
				_g_shp_coring_ctrl_06[i].gt_th0b		= 0x40;	//chip default
				_g_shp_coring_ctrl_09[i].edge_gain_b	= 0x20;	//chip default
				_g_shp_coring_ctrl_09[i].edge_gain_w	= 0x20;	//chip default
				_g_shp_coring_ctrl_09[i].texture_gain_b	= 0x20;	//chip default
				_g_shp_coring_ctrl_09[i].texture_gain_w	= 0x20;	//chip default
				_g_shp_dp_ctrl_01[i].edge_gain_b		= 0x20;	//chip default
				_g_shp_dp_ctrl_01[i].edge_gain_w		= 0x20;	//chip default
				_g_shp_dp_ctrl_01[i].texture_gain_b		= 0x20;	//chip default
				_g_shp_dp_ctrl_01[i].texture_gain_w		= 0x20;	//chip default
				#ifdef PE_PROTECTION_SCL_12TAP_ON_HD_INPUT
				_g_cur_scl_filter[i].win_id			= i;
				_g_cur_scl_filter[i].hor_y_tap_type	= 0xff;
				_g_cur_scl_filter[i].hor_c_tap_type	= 0xff;
				_g_cur_scl_filter[i].ver_y_tap_type	= 0xff;
				_g_cur_scl_filter[i].ver_c_tap_type	= 0xff;
				_g_cur_scl_filter[i].fir_luma_type	= 0xff;
				_g_cur_scl_filter[i].fir_chroma_type= 0xff;
				_g_cur_scl_filter[i].hor_luma_id	= 0xff;
				_g_cur_scl_filter[i].ver_luma_id	= 0xff;
				_g_cur_scl_filter[i].hor_chroma_id	= 0xff;
				_g_cur_scl_filter[i].ver_chroma_id	= 0xff;
				#endif
			}
			PE_SET_REG_TABLE(SHP,shp_l_init_param_l9b0,0);
			PE_SET_REG_TABLE(SHP,shp_r_init_param_l9b0,0);
			/* h,v min_max_filter enable */
	        DE_MSL_L9_RdFL(msc_min_max_ctrl_l_y);
	        DE_MSL_L9_RdFL(msc_min_max_ctrl_l_c);
	        DE_MSL_L9_Wr01(msc_min_max_ctrl_l_y, min_max_filter_en,	0x1);
	        DE_MSL_L9_Wr01(msc_min_max_ctrl_l_c, min_max_filter_en,	0x1);
	        DE_MSL_L9_Wr01(msc_min_max_ctrl_l_c, h_filter_en,		0x1);
	        DE_MSL_L9_WrFL(msc_min_max_ctrl_l_y);
	        DE_MSL_L9_WrFL(msc_min_max_ctrl_l_c);
		}
		else
		{
			PE_SHP_DBG_PRINT("init global values.\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_shp_scl_filter[i].win_id			= i;
				_g_shp_scl_filter[i].hor_y_tap_type	= 0xff;
				_g_shp_scl_filter[i].hor_c_tap_type	= 0xff;
				_g_shp_scl_filter[i].ver_y_tap_type	= 0xff;
				_g_shp_scl_filter[i].ver_c_tap_type	= 0xff;
				_g_shp_scl_filter[i].fir_luma_type	= 0xff;
				_g_shp_scl_filter[i].fir_chroma_type= 0xff;
				_g_shp_scl_filter[i].hor_luma_id	= 0xff;
				_g_shp_scl_filter[i].ver_luma_id	= 0xff;
				_g_shp_scl_filter[i].hor_chroma_id	= 0xff;
				_g_shp_scl_filter[i].ver_chroma_id	= 0xff;
				_g_shp_res_enable[i].win_id			= i;
				_g_shp_res_enable[i].enable			= 0xff;
			}

			PE_SET_REG_TABLE(SHP,shp_l_init_param_l9a0,0);
			PE_SET_REG_TABLE(SHP,shp_r_init_param_l9a0,0);
		}
	}while(0);

	return ret;
}

/**
 * standard function of shp setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_SHP_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_SHP_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RES_T):
		{
			LX_PE_SHP_RES_T *pRcvData;
			LX_PE_SHP_RES_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RES_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RES_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RES_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetResolutionEnhancement(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetResolutionEnhancement() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RES_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_EDGE_T):
		{
			LX_PE_SHP_EDGE_T *pRcvData;
			LX_PE_SHP_EDGE_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_EDGE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_EDGE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_EDGE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetEdgeEnhancement(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetEdgeEnhancement() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_EDGE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RES_ENABLE_T):
		{
			LX_PE_SHP_RES_ENABLE_T *pRcvData;
			LX_PE_SHP_RES_ENABLE_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RES_ENABLE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RES_ENABLE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RES_ENABLE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetResEnhanceEnable(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetResEnhanceEnable() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RES_ENABLE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_SCLFILTER_T):
		{
			LX_PE_SHP_SCLFILTER_T *pRcvData;
			LX_PE_SHP_SCLFILTER_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_SCLFILTER_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_SCLFILTER_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_SCLFILTER_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetScalerFilterCoeff(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetScalerFilterCoeff() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_SCLFILTER_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_CMN_T):
		{
			LX_PE_SHP_RE_CMN_T *pRcvData;
			LX_PE_SHP_RE_CMN_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_CMN_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_CMN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_CMN_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetReCommonCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetReCommonCtrl() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_CMN_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_HOR_T):
		{
			LX_PE_SHP_RE_HOR_T *pRcvData;
			LX_PE_SHP_RE_HOR_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_HOR_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_HOR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_HOR_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetReHorizontalCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetReHorizontalCtrl() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_HOR_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_VER_T):
		{
			LX_PE_SHP_RE_VER_T *pRcvData;
			LX_PE_SHP_RE_VER_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_VER_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_VER_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_VER_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetReVerticalCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetReVerticalCtrl() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_VER_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_MISC_T):
		{
			LX_PE_SHP_RE_MISC_T *pRcvData;
			LX_PE_SHP_RE_MISC_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_MISC_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_MISC_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_MISC_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetReMiscCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetReMiscCtrl() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_MISC_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_EE_T):
		{
			LX_PE_SHP_EE_T *pRcvData;
			LX_PE_SHP_EE_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_EE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_EE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_EE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetEdgeEnhanceCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetEdgeEnhanceCtrl() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_EE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_DE_T):
		{
			LX_PE_SHP_DE_T *pRcvData;
			LX_PE_SHP_DE_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_DE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_DE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_DE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetDetailEnhanceCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetDetailEnhanceCtrl() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_DE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetDefaultSettings(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetDefaultSettings() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_SHP_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_SHP_SetDebugSettings(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetDebugSettings() error.\n",__F__,__L__);
				PE_SHP_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_SHP_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of shp getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_SHP_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_SHP_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RES_T):
		{
			LX_PE_SHP_RES_T *pRcvData;
			LX_PE_SHP_RES_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RES_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RES_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RES_T *)data;
				stParam.win_id = pRcvData->win_id;
				stParam.type = pRcvData->type;
				ret = PE_SHP_GetResolutionEnhancement(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetResolutionEnhancement() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RES_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_EDGE_T):
		{
			LX_PE_SHP_EDGE_T *pRcvData;
			LX_PE_SHP_EDGE_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_EDGE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_EDGE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_EDGE_T *)data;
				stParam.win_id = pRcvData->win_id;
				stParam.type = pRcvData->type;
				ret = PE_SHP_GetEdgeEnhancement(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetEdgeEnhancement() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_EDGE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RES_ENABLE_T):
		{
			LX_PE_SHP_RES_ENABLE_T *pRcvData;
			LX_PE_SHP_RES_ENABLE_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RES_ENABLE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RES_ENABLE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RES_ENABLE_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetResEnhanceEnable(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetResEnhanceEnable() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RES_ENABLE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_SCLFILTER_T):
		{
			LX_PE_SHP_SCLFILTER_T *pRcvData;
			LX_PE_SHP_SCLFILTER_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_SCLFILTER_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_SCLFILTER_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_SCLFILTER_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetScalerFilterCoeff(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetScalerFilterCoeff() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_SCLFILTER_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_CMN_T):
		{
			LX_PE_SHP_RE_CMN_T *pRcvData;
			LX_PE_SHP_RE_CMN_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_CMN_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_CMN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_CMN_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetReCommonCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetReCommonCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_CMN_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_HOR_T):
		{
			LX_PE_SHP_RE_HOR_T *pRcvData;
			LX_PE_SHP_RE_HOR_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_HOR_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_HOR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_HOR_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetReHorizontalCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetReHorizontalCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_HOR_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_VER_T):
		{
			LX_PE_SHP_RE_VER_T *pRcvData;
			LX_PE_SHP_RE_VER_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_VER_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_VER_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_VER_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetReVerticalCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetReVerticalCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_VER_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_RE_MISC_T):
		{
			LX_PE_SHP_RE_MISC_T *pRcvData;
			LX_PE_SHP_RE_MISC_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_RE_MISC_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_RE_MISC_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_RE_MISC_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetReMiscCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetReMiscCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_RE_MISC_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_EE_T):
		{
			LX_PE_SHP_EE_T *pRcvData;
			LX_PE_SHP_EE_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_EE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_EE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_EE_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetEdgeEnhanceCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetEdgeEnhanceCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_EE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_SHP_DE_T):
		{
			LX_PE_SHP_DE_T *pRcvData;
			LX_PE_SHP_DE_T stParam;
			memset0(stParam);
			do{
				PE_SHP_DBG_PRINT("S LX_PE_SHP_DE_T\n");
				/* data size check */
				PE_SHP_CHECK_CODE(datasize!=sizeof(LX_PE_SHP_DE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_SHP_DE_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_SHP_GetDetailEnhanceCtrl(&stParam);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_GetDetailEnhanceCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_SHP_DBG_PRINT("E LX_PE_SHP_DE_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_SHP_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_SHP_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_CHECK_CODE(pstParams->src_type>=LX_PE_SRC_NUM,ret=RET_ERROR;break,\
			"[%s,%d] src_type(%d) is over.\n",__F__,__L__,pstParams->src_type);
		PE_SHP_CHECK_CODE(pstParams->fmt_type>=LX_PE_FMT_NUM,ret=RET_ERROR;break,\
			"[%s,%d] fmt_type(%d) is over.\n",__F__,__L__,pstParams->fmt_type);
		PE_SHP_CHECK_CODE(pstParams->out_type>=LX_PE_OUT_NUM,ret=RET_ERROR;break,\
			"[%s,%d] out_type(%d) is over.\n",__F__,__L__,pstParams->out_type);
		PE_SHP_CHECK_CODE(pstParams->fr_type>=LX_PE_FR_NUM,ret=RET_ERROR;break,\
			"[%s,%d] fr_type(%d) is over.\n",__F__,__L__,pstParams->fr_type);
		PE_SHP_CHECK_CODE(pstParams->scan_type>=LX_PE_SCAN_NUM,ret=RET_ERROR;break,\
			"[%s,%d] scan_type(%d) is over.\n",__F__,__L__,pstParams->scan_type);
		PE_SHP_CHECK_CODE(pstParams->dtv_type>=LX_PE_DTV_NUM,ret=RET_ERROR;break,\
			"[%s,%d] dtv_type(%d) is over.\n",__F__,__L__,pstParams->dtv_type);
		PE_SHP_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if((pstParams->out_type>=LX_PE_OUT_TB) && (pstParams->out_type<=LX_PE_OUT_DUAL))
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
					PE_SET_REG_TABLE(SHP,shp_l_default_3d_param_l9b0,0);
				if(PE_CHECK_SUB(pstParams->win_id))
					PE_SET_REG_TABLE(SHP,shp_r_default_3d_param_l9b0,0);
			}
			else	//2D
			{
				switch(pstParams->src_type)
				{
					case LX_PE_SRC_ATV:
						if(pstParams->fmt_type==LX_PE_FMT_NTSC)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_atv_sd_ntsc_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_atv_sd_ntsc_param_l9b0,0);
						}
						else	//pal, secam
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_atv_sd_pal_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_atv_sd_pal_param_l9b0,0);
						}
						break;
					case LX_PE_SRC_CVBS:
						if(pstParams->fmt_type==LX_PE_FMT_NTSC)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_av_sd_ntsc_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_av_sd_ntsc_param_l9b0,0);
						}
						else if(pstParams->fmt_type==LX_PE_FMT_SCARTRGB)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_av_sd_scartrgb_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_av_sd_scartrgb_param_l9b0,0);
						}
						else	//pal, secam
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_av_sd_pal_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_av_sd_pal_param_l9b0,0);
						}
						break;
					case LX_PE_SRC_RGBPC:
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_pc_all_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_pc_all_param_l9b0,0);
						}
						break;
					case LX_PE_SRC_DTV:
						if(pstParams->fmt_type==LX_PE_FMT_SD)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_dtv_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_dtv_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_dtv_hd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_dtv_hd_param_l9b0,0);
						}
						break;
					case LX_PE_SRC_COMP:
						if(pstParams->fmt_type==LX_PE_FMT_SD)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_comp_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_comp_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_comp_hd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_comp_hd_param_l9b0,0);
						}
						break;
					case LX_PE_SRC_HDMI:
					default:
						if(pstParams->fmt_type==LX_PE_FMT_SD)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_hdmi_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_hdmi_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_hdmi_hd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_hdmi_hd_param_l9b0,0);
						}
						break;
				}
			}

			/* set v filter c default */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_rf_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_rf_sd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_SECAM)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_av_secam_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_av_secam_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_av_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_av_sd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_RGBPC:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_pc_all_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_pc_all_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_rf_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_rf_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_all_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_COMP:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_av_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_av_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_all_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_hdmi_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_hdmi_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_vflt_c_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_vflt_c_default_all_hd_param_l9b0,0);
					}
					break;
			}
			PE_SHP_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				PE_FWI_PQ_TABLE_HEAD_T header;
				PE_FWI_L9_PQ_IPC_CTRL table;
				UINT32 tbl_size;
				UINT8 *pTable = NULL;

				pTable = (UINT8 *)&table;
				tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = pstParams->win_id;
				header.offset = PE_FWI_OFFSET(PE_FWI_L9_SHP_DERH_CTRL_0B);
				header.length = sizeof(PE_FWI_L9_SHP_DERH_CTRL_0B);
				table.shp_derh_ctrl_0B.e_gain_th1 = 0x28;
				table.shp_derh_ctrl_0B.e_gain_th2 = 0x44;

				PE_SHP_DBG_PRINT("[%s,%d]wId:%d,ofs:%d,len:%d, tbl_size:%d, e_g_th1,2:%d,%d\n",\
					__F__,__L__,header.win_id,header.offset,header.length,tbl_size,\
					table.shp_derh_ctrl_0B.e_gain_th1,table.shp_derh_ctrl_0B.e_gain_th2);
				ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);
				PE_ETC_SetPqFwWindow((UINT32)pstParams->win_id);
			}
		}
		else
		{
			/* resolution default */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
				case LX_PE_SRC_CVBS:
					if(PE_CHECK_MAIN(pstParams->win_id))
						PE_SET_REG_TABLE(SHP,shp_l_default_sd_param_l9a0,0);
					if(PE_CHECK_SUB(pstParams->win_id))
						PE_SET_REG_TABLE(SHP,shp_r_default_sd_param_l9a0,0);
					break;
				case LX_PE_SRC_DTV:
				case LX_PE_SRC_COMP:
				case LX_PE_SRC_RGBPC:
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_l_default_sd_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(SHP,shp_r_default_sd_param_l9a0,0);
					}
					else	//hd
					{
						if(pstParams->out_type==LX_PE_OUT_TB)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_hd_3d_tb_param_l9a0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_hd_3d_tb_param_l9a0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_l_default_hd_param_l9a0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(SHP,shp_r_default_hd_param_l9a0,0);
						}
					}
					break;
			}
			/* chroma filter default */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
						{
					        DE_MSL_L9_RdFL(msc_chroma_tear_ctrl_l);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, vsc_chroma_pre_filter_en,	0x1);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, manual_chroma_pre_filter_on,	0x1);
					        DE_MSL_L9_WrFL(msc_chroma_tear_ctrl_l);
						}
						if(PE_CHECK_SUB(pstParams->win_id))
						{
					        DE_MSR_L9_RdFL(msc_chroma_tear_ctrl_r);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, vsc_chroma_pre_filter_en,	0x1);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, manual_chroma_pre_filter_on,	0x1);
					        DE_MSR_L9_WrFL(msc_chroma_tear_ctrl_r);
						}
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
						{
					        DE_MSL_L9_RdFL(msc_chroma_tear_ctrl_l);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, vsc_chroma_pre_filter_en,	0x0);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, manual_chroma_pre_filter_on,	0x0);
					        DE_MSL_L9_WrFL(msc_chroma_tear_ctrl_l);
						}
						if(PE_CHECK_SUB(pstParams->win_id))
						{
					        DE_MSR_L9_RdFL(msc_chroma_tear_ctrl_r);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, vsc_chroma_pre_filter_en,	0x0);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, manual_chroma_pre_filter_on,	0x0);
					        DE_MSR_L9_WrFL(msc_chroma_tear_ctrl_r);
						}
					}
					break;
				case LX_PE_SRC_ATV:
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_SECAM)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
						{
					        DE_MSL_L9_RdFL(msc_chroma_tear_ctrl_l);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, vsc_chroma_pre_filter_en,	0x1);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, manual_chroma_pre_filter_on,	0x1);
					        DE_MSL_L9_WrFL(msc_chroma_tear_ctrl_l);
						}
						if(PE_CHECK_SUB(pstParams->win_id))
						{
					        DE_MSR_L9_RdFL(msc_chroma_tear_ctrl_r);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, vsc_chroma_pre_filter_en,	0x1);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, manual_chroma_pre_filter_on,	0x1);
					        DE_MSR_L9_WrFL(msc_chroma_tear_ctrl_r);
						}
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
						{
					        DE_MSL_L9_RdFL(msc_chroma_tear_ctrl_l);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, vsc_chroma_pre_filter_en,	0x0);
					        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, manual_chroma_pre_filter_on,	0x0);
					        DE_MSL_L9_WrFL(msc_chroma_tear_ctrl_l);
						}
						if(PE_CHECK_SUB(pstParams->win_id))
						{
					        DE_MSR_L9_RdFL(msc_chroma_tear_ctrl_r);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, vsc_chroma_pre_filter_en,	0x0);
					        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, manual_chroma_pre_filter_on,	0x0);
					        DE_MSR_L9_WrFL(msc_chroma_tear_ctrl_r);
						}
					}
					break;
				case LX_PE_SRC_COMP:
				case LX_PE_SRC_RGBPC:
				case LX_PE_SRC_HDMI:
				default:
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
				        DE_MSL_L9_RdFL(msc_chroma_tear_ctrl_l);
				        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, vsc_chroma_pre_filter_en,	0x0);
				        DE_MSL_L9_Wr01(msc_chroma_tear_ctrl_l, manual_chroma_pre_filter_on,	0x0);
				        DE_MSL_L9_WrFL(msc_chroma_tear_ctrl_l);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
				        DE_MSR_L9_RdFL(msc_chroma_tear_ctrl_r);
				        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, vsc_chroma_pre_filter_en,	0x0);
				        DE_MSR_L9_Wr01(msc_chroma_tear_ctrl_r, manual_chroma_pre_filter_on,	0x0);
				        DE_MSR_L9_WrFL(msc_chroma_tear_ctrl_r);
					}
					break;
			}
		}
	}while(0);

	return ret;
}
static int PE_SHP_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(SHP)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_shp_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(SHP)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				PE_SHP_DBG_PRINT("under construction.\n");	ret = RET_OK;
			}
			else
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(SHP)))
				{
					PE_SHP_DBG_PRINT("pe0 der, pe1 der,lsr,shp,cti,ti,dj : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
				        PE_P0L_L9A0_Wr01(p0l_der_ctrl_0,		der_en,					0x0);

				        PE_P1L_L9A0_Wr01(pe1_l_der_ctrl_0,		der_v_en,				0x0);
				        PE_P1L_L9A0_Wr02(pe1_l_lsr_ctrl_06,		edge_y_filter_en,		0x0,\
				        										edge_c_filter_en,		0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_shp_edge_ctrl_00,edge_enhance_enable,	0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_cti_ctrl_00,		cti_en,					0x0);
				        PE_P1L_L9A0_Wr02(pe1_l_cti_ctrl_02,		ycm_en0,				0x0,\
				        										ycm_en1,				0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_ti_ctrl_1,		enable,					0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_ti_ctrl_2,		enable,					0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_ti_ctrl_3,		enable,					0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_dj_ctrl_00,		edf_en,					0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_lsr_dtl_ctrl_00,	detail_enhancer_enable,	0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_lsr_dtl_ee_ctrl_02,enable,				0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_shp_um_ctrl_0,	enable,					0x0);
				        PE_P1L_L9A0_Wr01(pe1_l_shp_blur_ctrl,	enable,					0x0);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
				        PE_P0R_L9A0_Wr01(p0r_der_ctrl_0,		der_en,					0x0);

				        PE_P1R_L9A0_Wr01(pe1_r_der_ctrl_0,		der_v_en,				0x0);
				        PE_P1R_L9A0_Wr02(pe1_r_lsr_ctrl_06,		edge_y_filter_en,		0x0,\
				        										edge_c_filter_en,		0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_shp_edge_ctrl_00,edge_enhance_enable,	0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_cti_ctrl_00,		cti_en,					0x0);
				        PE_P1R_L9A0_Wr02(pe1_r_cti_ctrl_02,		ycm_en0,				0x0,\
				        										ycm_en1,				0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_ti_ctrl_1,		enable,					0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_ti_ctrl_2,		enable,					0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_ti_ctrl_3,		enable,					0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_dj_ctrl_00,		edf_en,					0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_lsr_dtl_ctrl_00,	detail_enhancer_enable,	0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_lsr_dtl_ee_ctrl_02,enable,				0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_shp_um_ctrl_0,	enable,					0x0);
				        PE_P1R_L9A0_Wr01(pe1_r_shp_blur_ctrl,	enable,					0x0);
					}
				}
			}
		}
	}while(0);
	return ret;
}
static int PE_SHP_SetResolutionEnhancement(LX_PE_SHP_RES_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_CHECK_CODE(!(pstParams->type & LX_PE_RES_ALL),ret=RET_ERROR;break,\
			"[%s,%d] type(%d) is wrong.\n",__F__,__L__,pstParams->type);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("under construction.\n");	ret = RET_OK;
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				if(pstParams->type & LX_PE_RES_CMN)
				{
					PE_P1L_L9A0_Wr02(pe1_l_shp_edge_ctrl_01,	white_gain,			GET_BITS(pstParams->cmn_ctrl.peaking_w_gain,0,7),\
																black_gain,			GET_BITS(pstParams->cmn_ctrl.peaking_b_gain,0,7));
					PE_P1L_L9A0_Wr02(pe1_l_lsr_dtl_ctrl_00,		gain_w,				GET_BITS(pstParams->cmn_ctrl.detail_master_w_gain,0,8),\
																gain_b,				GET_BITS(pstParams->cmn_ctrl.detail_master_b_gain,0,8));
					PE_P1L_L9A0_Wr02(pe1_l_lsr_dtl_ee_ctrl_02,	lap_gain,			GET_BITS(pstParams->cmn_ctrl.detail_lap_gain,0,8),\
																sobel_manual_gain,	GET_BITS(pstParams->cmn_ctrl.detail_sobel_gain,0,8));
					PE_P1L_L9A0_Wr02(pe1_l_lsr_dtl_ee_ctrl_03,	white_gain, 		GET_BITS(pstParams->cmn_ctrl.detail_lap_w_gain,0,7),\
																black_gain, 		GET_BITS(pstParams->cmn_ctrl.detail_lap_b_gain,0,7));
				}

				if(pstParams->type & LX_PE_RES_HOR)
				{
					PE_P1L_L9A0_Wr01(pe1_l_shp_edge_ctrl_01, horizontal_gain,	GET_BITS(pstParams->hor_ctrl.peaking_hor_gain,0,8));
					PE_P1L_L9A0_Wr01(pe1_l_lsr_ctrl_05, csft_gain,				GET_BITS(pstParams->hor_ctrl.dr_csft_gain,0,6));
					PE_P1L_L9A0_Wr02(pe1_l_lsr_ctrl_06, edge_filter_white_gain,	GET_BITS(pstParams->hor_ctrl.dr_edgef_w_gain,0,6),\
														edge_filter_black_gain,	GET_BITS(pstParams->hor_ctrl.dr_edgef_b_gain,0,6));
					PE_P1L_L9A0_Wr02(pe1_l_ti_ctrl_2,	y_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpre_ygain,0,8),\
														c_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpre_cgain,0,8));
					PE_P1L_L9A0_Wr02(pe1_l_ti_ctrl_3,	y_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpost_ygain,0,8),\
														c_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpost_cgain,0,8));
					PE_P1L_L9A0_Wr02(pe1_l_cti_ctrl_00, tap_size,				GET_BITS(pstParams->hor_ctrl.cti_tap_size,0,3),\
														cti_gain,				GET_BITS(pstParams->hor_ctrl.cti_gain,0,8));
					PE_P1L_L9A0_Wr02(pe1_l_cti_ctrl_02, ycm_band_sel,			GET_BITS(pstParams->hor_ctrl.cti_ycm_band_sel,0,3),\
														ycm_diff_th,			GET_BITS(pstParams->hor_ctrl.cti_ycm_diff_th,0,8));
					PE_P1L_L9A0_Wr02(pe1_l_cti_ctrl_02, ycm_y_gain,				GET_BITS(pstParams->hor_ctrl.cti_ycm_y_gain,0,4),\
														ycm_c_gain,				GET_BITS(pstParams->hor_ctrl.cti_ycm_c_gain,0,4));
				}

				if(pstParams->type & LX_PE_RES_VER)
				{
					PE_P0L_L9A0_Wr01(p0l_der_ctrl_0,			bif_th,			GET_BITS(pstParams->ver_ctrl.dr_vbif_th,0,8));
					PE_P1L_L9A0_Wr01(pe1_l_shp_edge_ctrl_01,	vertical_gain,	GET_BITS(pstParams->ver_ctrl.peaking_ver_gain,0,8));
					PE_P1L_L9A0_Wr01(pe1_l_ti_ctrl_1,			ti_gain, 		GET_BITS(pstParams->ver_ctrl.ti_v_gain,0,8));
				}
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				if(pstParams->type & LX_PE_RES_CMN)
				{
					PE_P1R_L9A0_Wr02(pe1_r_shp_edge_ctrl_01,	white_gain,			GET_BITS(pstParams->cmn_ctrl.peaking_w_gain,0,7),\
																black_gain,			GET_BITS(pstParams->cmn_ctrl.peaking_b_gain,0,7));
					PE_P1R_L9A0_Wr02(pe1_r_lsr_dtl_ctrl_00,		gain_w,				GET_BITS(pstParams->cmn_ctrl.detail_master_w_gain,0,8),\
																gain_b,				GET_BITS(pstParams->cmn_ctrl.detail_master_b_gain,0,8));
					PE_P1R_L9A0_Wr02(pe1_r_lsr_dtl_ee_ctrl_02,	lap_gain,			GET_BITS(pstParams->cmn_ctrl.detail_lap_gain,0,8),\
																sobel_manual_gain,	GET_BITS(pstParams->cmn_ctrl.detail_sobel_gain,0,8));
					PE_P1R_L9A0_Wr02(pe1_r_lsr_dtl_ee_ctrl_03,	white_gain, 		GET_BITS(pstParams->cmn_ctrl.detail_lap_w_gain,0,7),\
																black_gain, 		GET_BITS(pstParams->cmn_ctrl.detail_lap_b_gain,0,7));
				}

				if(pstParams->type & LX_PE_RES_HOR)
				{
					PE_P1R_L9A0_Wr01(pe1_r_shp_edge_ctrl_01, horizontal_gain,	GET_BITS(pstParams->hor_ctrl.peaking_hor_gain,0,8));
					PE_P1R_L9A0_Wr01(pe1_r_lsr_ctrl_05, csft_gain,				GET_BITS(pstParams->hor_ctrl.dr_csft_gain,0,6));
					PE_P1R_L9A0_Wr02(pe1_r_lsr_ctrl_06, edge_filter_white_gain,	GET_BITS(pstParams->hor_ctrl.dr_edgef_w_gain,0,6),\
														edge_filter_black_gain,	GET_BITS(pstParams->hor_ctrl.dr_edgef_b_gain,0,6));
					PE_P1R_L9A0_Wr02(pe1_r_ti_ctrl_2,	y_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpre_ygain,0,8),\
														c_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpre_cgain,0,8));
					PE_P1R_L9A0_Wr02(pe1_r_ti_ctrl_3,	y_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpost_ygain,0,8),\
														c_gain,					GET_BITS(pstParams->hor_ctrl.ti_hpost_cgain,0,8));
					PE_P1R_L9A0_Wr02(pe1_r_cti_ctrl_00, tap_size,				GET_BITS(pstParams->hor_ctrl.cti_tap_size,0,3),\
														cti_gain,				GET_BITS(pstParams->hor_ctrl.cti_gain,0,8));
					PE_P1R_L9A0_Wr02(pe1_r_cti_ctrl_02, ycm_band_sel,			GET_BITS(pstParams->hor_ctrl.cti_ycm_band_sel,0,3),\
														ycm_diff_th,			GET_BITS(pstParams->hor_ctrl.cti_ycm_diff_th,0,8));
					PE_P1R_L9A0_Wr02(pe1_r_cti_ctrl_02, ycm_y_gain,				GET_BITS(pstParams->hor_ctrl.cti_ycm_y_gain,0,4),\
														ycm_c_gain,				GET_BITS(pstParams->hor_ctrl.cti_ycm_c_gain,0,4));
				}

				if(pstParams->type & LX_PE_RES_VER)
				{
					PE_P0R_L9A0_Wr01(p0r_der_ctrl_0,			bif_th,			GET_BITS(pstParams->ver_ctrl.dr_vbif_th,0,8));
					PE_P1R_L9A0_Wr01(pe1_r_shp_edge_ctrl_01,	vertical_gain,	GET_BITS(pstParams->ver_ctrl.peaking_ver_gain,0,8));
					PE_P1R_L9A0_Wr01(pe1_r_ti_ctrl_1,			ti_gain, 		GET_BITS(pstParams->ver_ctrl.ti_v_gain,0,8));
				}
			}
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetResolutionEnhancement(LX_PE_SHP_RES_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_CHECK_CODE(!(pstParams->type & LX_PE_RES_ALL),ret=RET_ERROR;break,\
			"[%s,%d] type(%d) is wrong.\n",__F__,__L__,pstParams->type);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("under construction.\n");	ret = RET_OK;
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				if(pstParams->type & LX_PE_RES_CMN)
				{
					PE_P1L_L9A0_Rd02(pe1_l_shp_edge_ctrl_01,	white_gain,			pstParams->cmn_ctrl.peaking_w_gain,\
																black_gain,			pstParams->cmn_ctrl.peaking_b_gain);
					PE_P1L_L9A0_Rd02(pe1_l_lsr_dtl_ctrl_00,		gain_w,				pstParams->cmn_ctrl.detail_master_w_gain,\
																gain_b,				pstParams->cmn_ctrl.detail_master_b_gain);
					PE_P1L_L9A0_Rd02(pe1_l_lsr_dtl_ee_ctrl_02,	lap_gain,			pstParams->cmn_ctrl.detail_lap_gain,\
																sobel_manual_gain,	pstParams->cmn_ctrl.detail_sobel_gain);
					PE_P1L_L9A0_Rd02(pe1_l_lsr_dtl_ee_ctrl_03,	white_gain, 		pstParams->cmn_ctrl.detail_lap_w_gain,\
																black_gain, 		pstParams->cmn_ctrl.detail_lap_b_gain);
				}

				if(pstParams->type & LX_PE_RES_HOR)
				{
					PE_P1L_L9A0_Rd01(pe1_l_shp_edge_ctrl_01, horizontal_gain,	pstParams->hor_ctrl.peaking_hor_gain);
					PE_P1L_L9A0_Rd01(pe1_l_lsr_ctrl_05, csft_gain,				pstParams->hor_ctrl.dr_csft_gain);
					PE_P1L_L9A0_Rd02(pe1_l_lsr_ctrl_06, edge_filter_white_gain,	pstParams->hor_ctrl.dr_edgef_w_gain,\
														edge_filter_black_gain,	pstParams->hor_ctrl.dr_edgef_b_gain);
					PE_P1L_L9A0_Rd02(pe1_l_ti_ctrl_2,	y_gain,					pstParams->hor_ctrl.ti_hpre_ygain,\
														c_gain,					pstParams->hor_ctrl.ti_hpre_cgain);
					PE_P1L_L9A0_Rd02(pe1_l_ti_ctrl_3,	y_gain,					pstParams->hor_ctrl.ti_hpost_ygain,\
														c_gain,					pstParams->hor_ctrl.ti_hpost_cgain);
					PE_P1L_L9A0_Rd02(pe1_l_cti_ctrl_00, tap_size,				pstParams->hor_ctrl.cti_tap_size,\
														cti_gain,				pstParams->hor_ctrl.cti_gain);
					PE_P1L_L9A0_Rd02(pe1_l_cti_ctrl_02, ycm_band_sel,			pstParams->hor_ctrl.cti_ycm_band_sel,\
														ycm_diff_th,			pstParams->hor_ctrl.cti_ycm_diff_th);
					PE_P1L_L9A0_Rd02(pe1_l_cti_ctrl_02, ycm_y_gain,				pstParams->hor_ctrl.cti_ycm_y_gain,\
														ycm_c_gain,				pstParams->hor_ctrl.cti_ycm_c_gain);
				}

				if(pstParams->type & LX_PE_RES_VER)
				{
					PE_P0L_L9A0_Rd01(p0l_der_ctrl_0,			bif_th,			pstParams->ver_ctrl.dr_vbif_th);
					PE_P1L_L9A0_Rd01(pe1_l_shp_edge_ctrl_01,	vertical_gain,	pstParams->ver_ctrl.peaking_ver_gain);
					PE_P1L_L9A0_Rd01(pe1_l_ti_ctrl_1,			ti_gain, 		pstParams->ver_ctrl.ti_v_gain);
				}
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				if(pstParams->type & LX_PE_RES_CMN)
				{
					PE_P1R_L9A0_Rd02(pe1_r_shp_edge_ctrl_01,	white_gain,			pstParams->cmn_ctrl.peaking_w_gain,\
																black_gain,			pstParams->cmn_ctrl.peaking_b_gain);
					PE_P1R_L9A0_Rd02(pe1_r_lsr_dtl_ctrl_00,		gain_w,				pstParams->cmn_ctrl.detail_master_w_gain,\
																gain_b,				pstParams->cmn_ctrl.detail_master_b_gain);
					PE_P1R_L9A0_Rd02(pe1_r_lsr_dtl_ee_ctrl_02,	lap_gain,			pstParams->cmn_ctrl.detail_lap_gain,\
																sobel_manual_gain,	pstParams->cmn_ctrl.detail_sobel_gain);
					PE_P1R_L9A0_Rd02(pe1_r_lsr_dtl_ee_ctrl_03,	white_gain, 		pstParams->cmn_ctrl.detail_lap_w_gain,\
																black_gain, 		pstParams->cmn_ctrl.detail_lap_b_gain);
				}

				if(pstParams->type & LX_PE_RES_HOR)
				{
					PE_P1R_L9A0_Rd01(pe1_r_shp_edge_ctrl_01, horizontal_gain,	pstParams->hor_ctrl.peaking_hor_gain);
					PE_P1R_L9A0_Rd01(pe1_r_lsr_ctrl_05, csft_gain,				pstParams->hor_ctrl.dr_csft_gain);
					PE_P1R_L9A0_Rd02(pe1_r_lsr_ctrl_06, edge_filter_white_gain,	pstParams->hor_ctrl.dr_edgef_w_gain,\
														edge_filter_black_gain,	pstParams->hor_ctrl.dr_edgef_b_gain);
					PE_P1R_L9A0_Rd02(pe1_r_ti_ctrl_2,	y_gain,					pstParams->hor_ctrl.ti_hpre_ygain,\
														c_gain,					pstParams->hor_ctrl.ti_hpre_cgain);
					PE_P1R_L9A0_Rd02(pe1_r_ti_ctrl_3,	y_gain,					pstParams->hor_ctrl.ti_hpost_ygain,\
														c_gain,					pstParams->hor_ctrl.ti_hpost_cgain);
					PE_P1R_L9A0_Rd02(pe1_r_cti_ctrl_00, tap_size,				pstParams->hor_ctrl.cti_tap_size,\
														cti_gain,				pstParams->hor_ctrl.cti_gain);
					PE_P1R_L9A0_Rd02(pe1_r_cti_ctrl_02, ycm_band_sel,			pstParams->hor_ctrl.cti_ycm_band_sel,\
														ycm_diff_th,			pstParams->hor_ctrl.cti_ycm_diff_th);
					PE_P1R_L9A0_Rd02(pe1_r_cti_ctrl_02, ycm_y_gain,				pstParams->hor_ctrl.cti_ycm_y_gain,\
														ycm_c_gain,				pstParams->hor_ctrl.cti_ycm_c_gain);
				}

				if(pstParams->type & LX_PE_RES_VER)
				{
					PE_P0R_L9A0_Rd01(p0r_der_ctrl_0,			bif_th,			pstParams->ver_ctrl.dr_vbif_th);
					PE_P1R_L9A0_Rd01(pe1_r_shp_edge_ctrl_01,	vertical_gain,	pstParams->ver_ctrl.peaking_ver_gain);
					PE_P1R_L9A0_Rd01(pe1_r_ti_ctrl_1,			ti_gain, 		pstParams->ver_ctrl.ti_v_gain);
				}
			}
		}
	} while (0);
	return ret;
}
static int PE_SHP_SetEdgeEnhancement(LX_PE_SHP_EDGE_T *pstParams)
{
	int ret = RET_OK;
	UINT8 si_data,ei_data;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_CHECK_CODE(!(pstParams->type & LX_PE_EDGE_ALL),ret=RET_ERROR;break,\
			"[%s,%d] type(%d) is wrong.\n",__F__,__L__,pstParams->type);
		PE_SHP_DBG_PRINT("set pstParams[%d] : type:0x%x, si:%d, ei:%d\n",pstParams->win_id,\
			pstParams->type,pstParams->sharp_intensity,pstParams->edge_intensity);

		si_data = pstParams->sharp_intensity;
		ei_data = pstParams->edge_intensity;

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("under construction.\n");	ret = RET_OK;
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				if(pstParams->type & LX_PE_EDGE_SI)
				{
					PE_P1L_L9A0_Wr01(pe1_l_shp_um_ctrl_0,		enable,		si_data? 0x1:0x0);
					PE_P1L_L9A0_Wr01(pe1_l_shp_um_ctrl_0,		gain_b,		si_data? 0x38:0x18);
					PE_P1L_L9A0_Wr01(pe1_l_shp_um_ctrl_0,		gain_w,		si_data? 0x30:0x10);
				}
				if(pstParams->type & LX_PE_EDGE_EI)
				{
					PE_P1L_L9A0_Wr01(pe1_l_shp_edge_ctrl_02,	laplacian_weight,	(ei_data==2)? 0x7f:(ei_data==1)? 0x60:0x40);
				}
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				if(pstParams->type & LX_PE_EDGE_SI)
				{
					PE_P1R_L9A0_Wr01(pe1_r_shp_um_ctrl_0,		enable,		si_data? 0x1:0x0);
					PE_P1R_L9A0_Wr01(pe1_r_shp_um_ctrl_0,		gain_b,		si_data? 0x38:0x18);
					PE_P1R_L9A0_Wr01(pe1_r_shp_um_ctrl_0,		gain_w,		si_data? 0x30:0x10);
				}
				if(pstParams->type & LX_PE_EDGE_EI)
				{
					PE_P1R_L9A0_Wr01(pe1_r_shp_edge_ctrl_02,	laplacian_weight,	(ei_data==2)? 0x7f:(ei_data==1)? 0x60:0x40);
				}
			}
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetEdgeEnhancement(LX_PE_SHP_EDGE_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_CHECK_CODE(!(pstParams->type & LX_PE_EDGE_ALL),ret=RET_ERROR;break,\
			"[%s,%d] type(%d) is wrong.\n",__F__,__L__,pstParams->type);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("under construction.\n");	ret = RET_OK;
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				if(pstParams->type & LX_PE_EDGE_SI)
				{
					PE_P1L_L9A0_Rd01(pe1_l_shp_um_ctrl_0,		enable,		pstParams->sharp_intensity);
				}
				if(pstParams->type & LX_PE_EDGE_EI)
				{
					PE_P1L_L9A0_Rd01(pe1_l_shp_edge_ctrl_02,	laplacian_weight,	pstParams->edge_intensity);
				}
			}
			if(PE_CHECK_SUB(win_id))
			{
				if(pstParams->type & LX_PE_EDGE_SI)
				{
					PE_P1R_L9A0_Rd01(pe1_r_shp_um_ctrl_0,		enable,		pstParams->sharp_intensity);
				}
				if(pstParams->type & LX_PE_EDGE_EI)
				{
					PE_P1R_L9A0_Rd01(pe1_r_shp_edge_ctrl_02,	laplacian_weight,	pstParams->edge_intensity);
				}
			}
		}
		PE_SHP_DBG_PRINT("get pstParams[%d] : type:0x%x, si:%d, ei:%d\n",pstParams->win_id,\
			pstParams->type,pstParams->sharp_intensity,pstParams->edge_intensity);
	} while (0);

	return ret;
}
static int PE_SHP_SetResEnhanceEnable(LX_PE_SHP_RES_ENABLE_T *pstParams)
{
	int ret = RET_OK;
	UINT32 onoff;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] : enable:%d\n",pstParams->win_id,pstParams->enable);
		onoff = (pstParams->enable)? 0x1:0x0;

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Wr01(pe1_l_sp_ctrl_00,	edge_enhance_enable,	onoff);
				PE_P1L_L9B0_Wr01(pe1_l_mp_ctrl_00,	edge_enhance_enable,	onoff);
				_g_shp_res_enable[LX_PE_WIN_MAIN].enable	= pstParams->enable;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Wr01(pe1_r_sp_ctrl_00,	edge_enhance_enable,	onoff);
				PE_P1R_L9B0_Wr01(pe1_r_mp_ctrl_00,	edge_enhance_enable,	onoff);
				_g_shp_res_enable[LX_PE_WIN_SUB].enable	= pstParams->enable;
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9A0_Wr01(pe1_l_lsr_dtl_ctrl_00,	detail_enhancer_enable,	onoff);
				PE_P1L_L9A0_Wr01(pe1_l_lsr_ctrl_06,		edge_y_filter_en,		onoff);
				PE_P1L_L9A0_Wr01(pe1_l_lsr_ctrl_06,		edge_c_filter_en,		onoff);
				PE_P1L_L9A0_Wr01(pe1_l_shp_edge_ctrl_00,	edge_enhance_enable,	onoff);
				_g_shp_res_enable[LX_PE_WIN_MAIN].enable	= pstParams->enable;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9A0_Wr01(pe1_r_lsr_dtl_ctrl_00,	detail_enhancer_enable,	onoff);
				PE_P1R_L9A0_Wr01(pe1_r_lsr_ctrl_06,		edge_y_filter_en,		onoff);
				PE_P1R_L9A0_Wr01(pe1_r_lsr_ctrl_06,		edge_c_filter_en,		onoff);
				PE_P1R_L9A0_Wr01(pe1_r_shp_edge_ctrl_00,	edge_enhance_enable,	onoff);
				_g_shp_res_enable[LX_PE_WIN_SUB].enable	= pstParams->enable;
			}
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetResEnhanceEnable(LX_PE_SHP_RES_ENABLE_T *pstParams)
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
				pstParams->enable = _g_shp_res_enable[LX_PE_WIN_MAIN].enable;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->enable = _g_shp_res_enable[LX_PE_WIN_SUB].enable;
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->enable = _g_shp_res_enable[LX_PE_WIN_MAIN].enable;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->enable = _g_shp_res_enable[LX_PE_WIN_SUB].enable;
			}
		}
		PE_SHP_DBG_PRINT("get pstParams[%d] : enable:%d\n",pstParams->win_id,pstParams->enable);
	}while(0);

	return ret;
}
static int PE_SHP_CheckValidScalerFilterValue(LX_PE_SHP_SCLFILTER_T *pstParams)
{
	int ret = RET_ERROR;
	do{
		PE_SHP_CHECK_CODE(pstParams->hor_y_tap_type>=LX_PE_FILTERTYPE_NUM,break,\
			"[%s,%d] hor_y_tap_type(%d) is over.\n",__F__,__L__,pstParams->hor_y_tap_type);
		PE_SHP_CHECK_CODE(pstParams->hor_c_tap_type>=LX_PE_FILTERTYPE_NUM,break,\
			"[%s,%d] hor_c_tap_type(%d) is over.\n",__F__,__L__,pstParams->hor_c_tap_type);
		PE_SHP_CHECK_CODE(pstParams->ver_y_tap_type>=LX_PE_FILTERTYPE_NUM,break,\
			"[%s,%d] ver_y_tap_type(%d) is over.\n",__F__,__L__,pstParams->ver_y_tap_type);
		PE_SHP_CHECK_CODE(pstParams->ver_c_tap_type>=LX_PE_FILTERTYPE_NUM,break,\
			"[%s,%d] ver_c_tap_type(%d) is over.\n",__F__,__L__,pstParams->ver_c_tap_type);
		PE_SHP_CHECK_CODE(pstParams->fir_luma_type>=LX_PE_FIRTYPE_NUM,break,\
			"[%s,%d] fir_luma_type(%d) is over.\n",__F__,__L__,pstParams->fir_luma_type);
		PE_SHP_CHECK_CODE(pstParams->fir_chroma_type>=LX_PE_FIRTYPE_NUM,break,\
			"[%s,%d] fir_chroma_type(%d) is over.\n",__F__,__L__,pstParams->fir_chroma_type);
		PE_SHP_CHECK_CODE(((pstParams->hor_y_tap_type==LX_PE_12TAP&&\
			pstParams->ver_y_tap_type!=LX_PE_12TAP)||\
			(pstParams->hor_y_tap_type!=LX_PE_12TAP&&\
			pstParams->ver_y_tap_type==LX_PE_12TAP))\
			,break,"[%s,%d] h,v_y_tap type(%d,%d) mismatch.\n",\
			__F__,__L__,pstParams->hor_y_tap_type,pstParams->ver_y_tap_type);
		PE_SHP_CHECK_CODE(((pstParams->hor_c_tap_type==LX_PE_12TAP&&\
			pstParams->ver_c_tap_type!=LX_PE_12TAP)||\
			(pstParams->hor_c_tap_type!=LX_PE_12TAP&&\
			pstParams->ver_c_tap_type==LX_PE_12TAP))\
			,break,"[%s,%d] h,v_c_tap type(%d,%d) mismatch.\n",\
			__F__,__L__,pstParams->hor_c_tap_type,pstParams->ver_c_tap_type);
		PE_SHP_CHECK_CODE(((pstParams->hor_y_tap_type==LX_PE_12TAP&&\
			pstParams->hor_c_tap_type!=LX_PE_12TAP)||\
			(pstParams->hor_y_tap_type!=LX_PE_12TAP&&\
			pstParams->hor_c_tap_type==LX_PE_12TAP))\
			,break,"[%s,%d] h_y,c_tap type(%d,%d) mismatch.\n",\
			__F__,__L__,pstParams->hor_y_tap_type,pstParams->hor_c_tap_type);
		PE_SHP_CHECK_CODE(((pstParams->ver_y_tap_type==LX_PE_12TAP&&\
			pstParams->ver_c_tap_type!=LX_PE_12TAP)||\
			(pstParams->ver_y_tap_type!=LX_PE_12TAP&&\
			pstParams->ver_c_tap_type==LX_PE_12TAP))\
			,break,"[%s,%d] v_y,c_tap type(%d,%d) mismatch.\n",\
			__F__,__L__,pstParams->ver_y_tap_type,pstParams->ver_c_tap_type);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			/* h luma id check */
			PE_SHP_CHECK_CODE((pstParams->hor_y_tap_type==LX_PE_12TAP&&\
				pstParams->hor_luma_id>=PE_SHP_SCL_COEFF_LUMA_H_12_L9B0_NUM)\
				,break,"[%s,%d] hor_y_tap_type(%d), hor_luma_id(%d) is out of range.\n",\
				__F__,__L__,pstParams->hor_y_tap_type,pstParams->hor_luma_id);
			PE_SHP_CHECK_CODE((pstParams->hor_y_tap_type==LX_PE_06TAP_NORMAL&&\
				pstParams->hor_luma_id>=PE_SHP_SCL_COEFF_LUMA_H_06_L9B0_NUM)\
				,break,"[%s,%d] hor_y_tap_type(%d), hor_luma_id(%d) is out of range.\n",\
				__F__,__L__,pstParams->hor_y_tap_type,pstParams->hor_luma_id);
			/* v luma id check */
			PE_SHP_CHECK_CODE((pstParams->ver_y_tap_type==LX_PE_12TAP&&\
				pstParams->ver_luma_id>=PE_SHP_SCL_COEFF_LUMA_V_12_L9B0_NUM)\
				,break,"[%s,%d] ver_y_tap_type(%d), ver_luma_id(%d) is out of range.\n",\
				__F__,__L__,pstParams->ver_y_tap_type,pstParams->ver_luma_id);
			PE_SHP_CHECK_CODE(((pstParams->ver_y_tap_type==LX_PE_06TAP_NORMAL||\
				pstParams->ver_y_tap_type==LX_PE_06TAP_HALF)&&\
				pstParams->ver_luma_id>=PE_SHP_SCL_COEFF_LUMA_V_06_L9B0_NUM)\
				,break,"[%s,%d] ver_y_tap_type(%d), ver_luma_id(%d) is out of range.\n",\
				__F__,__L__,pstParams->ver_y_tap_type,pstParams->ver_luma_id);
			/* h chroma id check */
			PE_SHP_CHECK_CODE(((pstParams->hor_c_tap_type==LX_PE_12TAP||\
				pstParams->hor_c_tap_type==LX_PE_06TAP_NORMAL)&&\
				pstParams->hor_chroma_id>=PE_SHP_SCL_COEFF_CHROMA_H_L9B0_NUM)\
				,break,"[%s,%d] hor_c_tap_type(%d), hor_chroma_id(%d) is out of range.\n",\
				__F__,__L__,pstParams->hor_c_tap_type,pstParams->hor_chroma_id);
			/* v chroma id check */
			PE_SHP_CHECK_CODE(((pstParams->hor_c_tap_type==LX_PE_12TAP||\
				pstParams->hor_c_tap_type==LX_PE_06TAP_NORMAL||\
				pstParams->hor_c_tap_type==LX_PE_06TAP_HALF)&&\
				pstParams->hor_chroma_id>=PE_SHP_SCL_COEFF_CHROMA_V_L9B0_NUM)\
				,break,"[%s,%d] hor_c_tap_type(%d), hor_chroma_id(%d) is out of range.\n",\
				__F__,__L__,pstParams->hor_c_tap_type,pstParams->hor_chroma_id);
			/* sub v6t check */
			PE_SHP_CHECK_CODE((pstParams->win_id>LX_PE_WIN_MAIN)&&\
				(pstParams->hor_y_tap_type==LX_PE_12TAP||pstParams->hor_c_tap_type==LX_PE_12TAP||\
				pstParams->ver_y_tap_type==LX_PE_12TAP||pstParams->ver_c_tap_type==LX_PE_12TAP),\
				break,"[%s,%d] win[%d],h_tap(y,c):%d,%d, v_tap(y,c):%d%d, It cannot use 12tap.\n",\
				__F__,__L__,pstParams->win_id,pstParams->hor_y_tap_type,\
				pstParams->hor_c_tap_type,pstParams->ver_y_tap_type,pstParams->ver_c_tap_type);

			if(	pstParams->hor_y_tap_type==LX_PE_06TAP_DOWN_75 ||\
				pstParams->hor_y_tap_type==LX_PE_06TAP_DOWN_50 ||\
				pstParams->hor_y_tap_type==LX_PE_06TAP_DOWN_25)
			{
				pstParams->hor_luma_id=0;
			}
			if( pstParams->ver_y_tap_type==LX_PE_06TAP_DOWN_75 ||\
				pstParams->ver_y_tap_type==LX_PE_06TAP_DOWN_50 ||\
				pstParams->ver_y_tap_type==LX_PE_06TAP_DOWN_25)
			{
				pstParams->ver_luma_id=0;
			}
			if( pstParams->hor_c_tap_type==LX_PE_06TAP_DOWN_75 ||\
				pstParams->hor_c_tap_type==LX_PE_06TAP_DOWN_50 ||\
				pstParams->hor_c_tap_type==LX_PE_06TAP_DOWN_25)
			{
				pstParams->hor_chroma_id=0;
			}
			if( pstParams->ver_c_tap_type==LX_PE_06TAP_DOWN_75 ||\
				pstParams->ver_c_tap_type==LX_PE_06TAP_DOWN_50 ||\
				pstParams->ver_c_tap_type==LX_PE_06TAP_DOWN_25)
			{
				pstParams->ver_chroma_id=0;
			}
		}
		else
		{
			PE_SHP_CHECK_CODE(((pstParams->hor_y_tap_type==LX_PE_12TAP||\
				pstParams->hor_y_tap_type==LX_PE_06TAP_NORMAL||\
				pstParams->hor_y_tap_type==LX_PE_06TAP_HALF)&&\
				pstParams->hor_luma_id>=PE_SHP_SCL_COEFF_L9A0_NUM)\
				,break,"[%s,%d] hor_luma_id(%d) is out of range.\n",__F__,__L__,pstParams->hor_luma_id);
			PE_SHP_CHECK_CODE(((pstParams->ver_y_tap_type==LX_PE_12TAP||\
				pstParams->ver_y_tap_type==LX_PE_06TAP_NORMAL||\
				pstParams->ver_y_tap_type==LX_PE_06TAP_HALF)&&\
				pstParams->ver_luma_id>=PE_SHP_SCL_COEFF_L9A0_NUM)\
				,break,"[%s,%d] ver_luma_id(%d) is out of range.\n",__F__,__L__,pstParams->ver_luma_id);
			pstParams->hor_luma_id = pstParams->ver_luma_id = pstParams->hor_chroma_id = pstParams->ver_chroma_id = 0;
		}
		ret = RET_OK;
	}while(0);
	return ret;
}
static int PE_SHP_SetScalerFilterCoeff(LX_PE_SHP_SCLFILTER_T *pstParams)
{
	int ret = RET_ERROR;
	UINT8 type_sel_luma,type_sel_chroma,scaler_config,adap_filter_en;
	PE_SHP_SETFLAG_T flag;
	UINT32 base_addr;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_CHECK_CODE(PE_SHP_CheckValidScalerFilterValue(pstParams)\
			,break,"[%s,%d] PE_SHP_CheckValidScalerFilterValue() error.\n",__F__,__L__);
		PE_SHP_DBG_PRINT("set[%d]:h(y,c),v(y,c)tap:(%d,%d),(%d,%d), fir(y,c):%d,%d, idx(h,v) y:%d,%d, c:%d,%d\n",\
			pstParams->win_id,pstParams->hor_y_tap_type,pstParams->hor_c_tap_type,\
			pstParams->ver_y_tap_type,pstParams->ver_c_tap_type,pstParams->fir_luma_type,\
			pstParams->fir_chroma_type,pstParams->hor_luma_id,pstParams->ver_luma_id,\
			pstParams->hor_chroma_id,pstParams->ver_chroma_id);

		scaler_config	= (pstParams->hor_y_tap_type==LX_PE_12TAP)? 0x0:0x1;
		adap_filter_en	= (pstParams->hor_y_tap_type==LX_PE_12TAP)? 0x1:0x0;
		type_sel_luma	= (pstParams->fir_luma_type==LX_PE_BILINEAR)? 0x1:0x0;
		type_sel_chroma	= (pstParams->fir_chroma_type==LX_PE_BILINEAR)? 0x1:0x0;

		if(PE_CHECK_MAIN(pstParams->win_id))
		{
			#ifdef PE_PROTECTION_SCL_12TAP_ON_HD_INPUT
			_g_cur_scl_filter[LX_PE_WIN_MAIN].win_id			= pstParams->win_id;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_y_tap_type	= pstParams->hor_y_tap_type;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_c_tap_type	= pstParams->hor_c_tap_type;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_y_tap_type	= pstParams->ver_y_tap_type;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_c_tap_type	= pstParams->ver_c_tap_type;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].fir_luma_type		= pstParams->fir_luma_type;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].fir_chroma_type	= pstParams->fir_chroma_type;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_luma_id		= pstParams->hor_luma_id;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_luma_id		= pstParams->ver_luma_id;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_chroma_id		= pstParams->hor_chroma_id;
			_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_chroma_id		= pstParams->ver_chroma_id;
			ret = PE_SHP_SetSclSettingCnt(flag,pstParams);
			PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetSclSettingCnt() error.\n",__F__,__L__);
			#else
			base_addr = 0x0;
			DE_MSL_L9_RdFL(msc_ctrl_l);
			DE_MSL_L9_WfCM(msc_ctrl_l, filter_type_sel_luma       ,type_sel_luma,	0x1);
			DE_MSL_L9_WfCM(msc_ctrl_l, filter_type_sel_chroma     ,type_sel_chroma,	0x1);
			DE_MSL_L9_WfCM(msc_ctrl_l, v_scaler_config            ,scaler_config,	0x1);
			DE_MSL_L9_WfCM(msc_ctrl_l, adaptive_fir_filter_en     ,adap_filter_en,	0x1);
			DE_MSL_L9_WrFL(msc_ctrl_l);
			flag = PE_SHP_GetDoubleSettingsFlag(LX_PE_WIN_MAIN, pstParams);
			PE_SHP_DBG_PRINT("set flag y(h,v):%d,%d, c(h,v):%d,%d\n",flag.h_y,flag.v_y,flag.h_c,flag.v_c);
			ret = PE_SHP_SetScalerFilterCoeffByReg(base_addr,flag,pstParams);
			PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetScalerFilterCoeffByReg() error.\n",__F__,__L__);
			ret = PE_SHP_SetSclSettingCnt(flag,pstParams);
			PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetSclSettingCnt() error.\n",__F__,__L__);
			_g_shp_scl_filter[LX_PE_WIN_MAIN].hor_y_tap_type	= pstParams->hor_y_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].hor_c_tap_type	= pstParams->hor_c_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].ver_y_tap_type	= pstParams->ver_y_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].ver_c_tap_type	= pstParams->ver_c_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].fir_luma_type		= pstParams->fir_luma_type;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].fir_chroma_type	= pstParams->fir_chroma_type;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].hor_luma_id		= pstParams->hor_luma_id;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].ver_luma_id		= pstParams->ver_luma_id;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].hor_chroma_id		= pstParams->hor_chroma_id;
			_g_shp_scl_filter[LX_PE_WIN_MAIN].ver_chroma_id		= pstParams->ver_chroma_id;
			#endif
		}
		if(PE_CHECK_SUB(pstParams->win_id))
		{
			base_addr = 0x2000;
			DE_MSR_L9_RdFL(msc_ctrl_r);
			DE_MSR_L9_WfCM(msc_ctrl_r,  filter_type_sel_luma       ,type_sel_luma,	0x1);
			DE_MSR_L9_WfCM(msc_ctrl_r,  filter_type_sel_chroma     ,type_sel_chroma,0x1);
			if(PE_CHIP_VER_LT(L9,B0))
			{
				DE_MSR_L9A_WfCM(msc_ctrl_r, v_scaler_config            ,scaler_config,	0x1);
			}
			DE_MSR_L9_WfCM(msc_ctrl_r,  adaptive_fir_filter_en     ,adap_filter_en,	0x1);
			DE_MSR_L9_WrFL(msc_ctrl_r);
			flag = PE_SHP_GetDoubleSettingsFlag(LX_PE_WIN_SUB, pstParams);
			PE_SHP_DBG_PRINT("set flag y(h,v):%d,%d, c(h,v):%d,%d\n",flag.h_y,flag.v_y,flag.h_c,flag.v_c);
			ret = PE_SHP_SetScalerFilterCoeffByReg(base_addr,flag,pstParams);
			PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetScalerFilterCoeffByReg() error.\n",__F__,__L__);
			_g_shp_scl_filter[LX_PE_WIN_SUB].hor_y_tap_type		= pstParams->hor_y_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_SUB].hor_c_tap_type		= pstParams->hor_c_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_SUB].ver_y_tap_type		= pstParams->ver_y_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_SUB].ver_c_tap_type		= pstParams->ver_c_tap_type;
			_g_shp_scl_filter[LX_PE_WIN_SUB].fir_luma_type		= pstParams->fir_luma_type;
			_g_shp_scl_filter[LX_PE_WIN_SUB].fir_chroma_type	= pstParams->fir_chroma_type;
			_g_shp_scl_filter[LX_PE_WIN_SUB].hor_luma_id		= pstParams->hor_luma_id;
			_g_shp_scl_filter[LX_PE_WIN_SUB].ver_luma_id		= pstParams->ver_luma_id;
			_g_shp_scl_filter[LX_PE_WIN_SUB].hor_chroma_id		= pstParams->hor_chroma_id;
			_g_shp_scl_filter[LX_PE_WIN_SUB].ver_chroma_id		= pstParams->ver_chroma_id;
		}
	} while (0);

	return ret;
}
static int PE_SHP_GetScalerFilterCoeff(LX_PE_SHP_SCLFILTER_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);
		if(PE_CHECK_MAIN(win_id))
		{
			#ifdef PE_PROTECTION_SCL_12TAP_ON_HD_INPUT
			pstParams->hor_y_tap_type	= _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_y_tap_type;
			pstParams->hor_c_tap_type	= _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_c_tap_type;
			pstParams->ver_y_tap_type	= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_y_tap_type;
			pstParams->ver_c_tap_type	= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_c_tap_type;
			pstParams->fir_luma_type	= _g_cur_scl_filter[LX_PE_WIN_MAIN].fir_luma_type;
			pstParams->fir_chroma_type	= _g_cur_scl_filter[LX_PE_WIN_MAIN].fir_chroma_type;
			pstParams->hor_luma_id		= _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_luma_id;
			pstParams->ver_luma_id		= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_luma_id;
			pstParams->hor_chroma_id	= _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_chroma_id;
			pstParams->ver_chroma_id	= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_chroma_id;
			#else
			pstParams->hor_y_tap_type	= _g_shp_scl_filter[LX_PE_WIN_MAIN].hor_y_tap_type;
			pstParams->hor_c_tap_type	= _g_shp_scl_filter[LX_PE_WIN_MAIN].hor_c_tap_type;
			pstParams->ver_y_tap_type	= _g_shp_scl_filter[LX_PE_WIN_MAIN].ver_y_tap_type;
			pstParams->ver_c_tap_type	= _g_shp_scl_filter[LX_PE_WIN_MAIN].ver_c_tap_type;
			pstParams->fir_luma_type	= _g_shp_scl_filter[LX_PE_WIN_MAIN].fir_luma_type;
			pstParams->fir_chroma_type	= _g_shp_scl_filter[LX_PE_WIN_MAIN].fir_chroma_type;
			pstParams->hor_luma_id		= _g_shp_scl_filter[LX_PE_WIN_MAIN].hor_luma_id;
			pstParams->ver_luma_id		= _g_shp_scl_filter[LX_PE_WIN_MAIN].ver_luma_id;
			pstParams->hor_chroma_id	= _g_shp_scl_filter[LX_PE_WIN_MAIN].hor_chroma_id;
			pstParams->ver_chroma_id	= _g_shp_scl_filter[LX_PE_WIN_MAIN].ver_chroma_id;
			#endif
		}
		if(PE_CHECK_SUB(win_id))
		{
			pstParams->hor_y_tap_type	= _g_shp_scl_filter[LX_PE_WIN_SUB].hor_y_tap_type;
			pstParams->hor_c_tap_type	= _g_shp_scl_filter[LX_PE_WIN_SUB].hor_c_tap_type;
			pstParams->ver_y_tap_type	= _g_shp_scl_filter[LX_PE_WIN_SUB].ver_y_tap_type;
			pstParams->ver_c_tap_type	= _g_shp_scl_filter[LX_PE_WIN_SUB].ver_c_tap_type;
			pstParams->fir_luma_type	= _g_shp_scl_filter[LX_PE_WIN_SUB].fir_luma_type;
			pstParams->fir_chroma_type	= _g_shp_scl_filter[LX_PE_WIN_SUB].fir_chroma_type;
			pstParams->hor_luma_id		= _g_shp_scl_filter[LX_PE_WIN_SUB].hor_luma_id;
			pstParams->ver_luma_id		= _g_shp_scl_filter[LX_PE_WIN_SUB].ver_luma_id;
			pstParams->hor_chroma_id	= _g_shp_scl_filter[LX_PE_WIN_SUB].hor_chroma_id;
			pstParams->ver_chroma_id	= _g_shp_scl_filter[LX_PE_WIN_SUB].ver_chroma_id;
		}
		PE_SHP_DBG_PRINT("get[%d]:h(y,c),v(y,c)tap:(%d,%d),(%d,%d), fir(y,c):%d,%d, idx(h,v) y:%d,%d, c:%d,%d\n",\
			pstParams->win_id,pstParams->hor_y_tap_type,pstParams->hor_c_tap_type,\
			pstParams->ver_y_tap_type,pstParams->ver_c_tap_type,pstParams->fir_luma_type,\
			pstParams->fir_chroma_type,pstParams->hor_luma_id,pstParams->ver_luma_id,\
			pstParams->hor_chroma_id,pstParams->ver_chroma_id);
	}while(0);

	return ret;
}
static PE_SHP_SETFLAG_T PE_SHP_GetDoubleSettingsFlag(LX_PE_WIN_ID win_id, LX_PE_SHP_SCLFILTER_T *pstParams)
{
	PE_SHP_SETFLAG_T flag = {0,0,0,0};
	UINT8 h_luma_index = pstParams->hor_luma_id;
	UINT8 v_luma_index = pstParams->ver_luma_id;
	UINT8 h_chroma_index = pstParams->hor_chroma_id;
	UINT8 v_chroma_index = pstParams->ver_chroma_id;
	LX_PE_SCL_FILTERTYPE hor_y_tap_type = pstParams->hor_y_tap_type;
	LX_PE_SCL_FILTERTYPE hor_c_tap_type = pstParams->hor_c_tap_type;
	LX_PE_SCL_FILTERTYPE ver_y_tap_type = pstParams->ver_y_tap_type;
	LX_PE_SCL_FILTERTYPE ver_c_tap_type = pstParams->ver_c_tap_type;

	if(_g_shp_scl_filter[win_id].hor_y_tap_type!= hor_y_tap_type)
	{
		flag.h_y=1;
		/* 12tap filter can not set h only.(h&v or v) */
		if(hor_y_tap_type==LX_PE_12TAP)
		{
			if(h_luma_index!=v_luma_index)	flag.v_y=1;
		}
	}
	else if((hor_y_tap_type==LX_PE_12TAP) ||
		(hor_y_tap_type==LX_PE_06TAP_NORMAL) ||
		(hor_y_tap_type==LX_PE_06TAP_HALF))
	{
		if(_g_shp_scl_filter[win_id].hor_luma_id != h_luma_index)
		{
			flag.h_y=1;
			/* 12tap filter can not set h only.(h&v or v) */
			if(hor_y_tap_type==LX_PE_12TAP)
			{
				if(h_luma_index!=v_luma_index)	flag.v_y=1;
			}
		}
	}

	if(_g_shp_scl_filter[win_id].hor_c_tap_type!= hor_c_tap_type)
	{
		flag.h_c=1;
	}
	else if((hor_c_tap_type==LX_PE_12TAP) ||
		(hor_c_tap_type==LX_PE_06TAP_NORMAL) ||
		(hor_c_tap_type==LX_PE_06TAP_HALF))
	{
		if(_g_shp_scl_filter[win_id].hor_chroma_id != h_chroma_index)	flag.h_c=1;
	}

	if(_g_shp_scl_filter[win_id].ver_y_tap_type!= ver_y_tap_type)
	{
		flag.v_y=1;
	}
	else if((ver_y_tap_type==LX_PE_12TAP) ||
		(ver_y_tap_type==LX_PE_06TAP_NORMAL) ||
		(ver_y_tap_type==LX_PE_06TAP_HALF))
	{
		if(_g_shp_scl_filter[win_id].ver_luma_id != v_luma_index)	flag.v_y=1;
	}

	if(_g_shp_scl_filter[win_id].ver_c_tap_type!= ver_c_tap_type)
	{
		flag.v_c=1;
	}
	else if((ver_c_tap_type==LX_PE_12TAP) ||
		(ver_c_tap_type==LX_PE_06TAP_NORMAL) ||
		(ver_c_tap_type==LX_PE_06TAP_HALF))
	{
		if(_g_shp_scl_filter[win_id].ver_chroma_id != v_chroma_index)	flag.v_c=1;
	}

	return flag;
}
static int PE_SHP_SetScalerFilterCoeffByReg(UINT32 base_addr, PE_SHP_SETFLAG_T flag, LX_PE_SHP_SCLFILTER_T *pstParams)
{
	int ret = RET_OK;
	UINT32 data_size;
	UINT8 h_luma_index		= pstParams->hor_luma_id;
	UINT8 v_luma_index		= pstParams->ver_luma_id;
	UINT8 h_chroma_index	= pstParams->hor_chroma_id;
	UINT8 v_chroma_index	= pstParams->ver_chroma_id;
	LX_PE_SCL_FILTERTYPE hor_y_tap_type = pstParams->hor_y_tap_type;
	LX_PE_SCL_FILTERTYPE hor_c_tap_type = pstParams->hor_c_tap_type;
	LX_PE_SCL_FILTERTYPE ver_y_tap_type = pstParams->ver_y_tap_type;
	LX_PE_SCL_FILTERTYPE ver_c_tap_type = pstParams->ver_c_tap_type;

	if(flag.h_y)
	{
		switch(hor_y_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_12tap_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_H_12_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_h_12tap_coeff_l9b0[h_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_12tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_H_12_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_h_12tap_coeff_l9a0[h_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_dn_75_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_H_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_h_dn_75_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_dn_50_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_H_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_h_dn_50_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_dn_25_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_H_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_h_dn_25_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_NORMAL:
			case LX_PE_06TAP_HALF:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_06tap_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_H_06_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_h_06tap_coeff_l9b0[h_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_06tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_H_06_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_h_06tap_coeff_l9a0[h_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
		}
	}

	if(flag.h_c)
	{
		switch(hor_c_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_H_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_h_coeff_l9b0[h_chroma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
					flag.v_c = 0;	// avoid double setting
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_def_12tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_12_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_def_12tap_coeff_l9a0,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
					flag.v_c = 0;	// avoid double setting
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_dn_75_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_H_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_h_dn_75_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_dn_50_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_H_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_h_dn_50_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_dn_25_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_H_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_h_dn_25_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_NORMAL:
			case LX_PE_06TAP_HALF:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_H_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_h_coeff_l9b0[h_chroma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_06tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_H_06_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_h_06tap_coeff_l9a0,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
		}
	}

	if(flag.v_y)
	{
		switch(ver_y_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_12tap_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_12_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_12tap_coeff_l9b0[v_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_12tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_12_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_12tap_coeff_l9a0[v_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_HALF:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_half_06tap_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_06_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_half_06tap_coeff_l9b0[v_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_half_06tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_06_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_half_06tap_coeff_l9a0[v_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_dn_75_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_dn_75_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
			break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_dn_50_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_dn_50_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
			break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_dn_25_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_dn_25_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
			break;
			case LX_PE_06TAP_NORMAL:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_06tap_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_06_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_06tap_coeff_l9b0[v_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_06tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_LUMA_V_06_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_luma_v_06tap_coeff_l9a0[v_luma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
		}
	}

	if(flag.v_c)
	{
		switch(ver_c_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_coeff_l9b0\n");
					/* v12t mode : v_c_flt share h_c_flt coeff. */
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_H_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_h_coeff_l9b0[v_chroma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_def_12tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_12_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_def_12tap_coeff_l9a0,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_dn_75_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_V_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_v_dn_75_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_dn_50_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_V_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_v_dn_50_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_dn_25_06tap_coeff_l9\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_V_06_L9_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_v_dn_25_06tap_coeff_l9,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
			case LX_PE_06TAP_NORMAL:
			case LX_PE_06TAP_HALF:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_coeff_l9b0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_V_L9B0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_v_coeff_l9b0[v_chroma_index],base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_06tap_coeff_l9a0\n");
					data_size = sizeof(PE_REG_PARAM_T)*PE_SHP_SCL_CHROMA_V_06_L9A0_NUM;
					ret = PE_REG_SetTable(shp_scl_chroma_v_06tap_coeff_l9a0,base_addr,data_size);
					PE_SHP_CHECK_CODE(ret,return ret,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);
				}
				break;
		}
	}

	return ret;
}
static int PE_SHP_SetSclSettingCnt(PE_SHP_SETFLAG_T flag, LX_PE_SHP_SCLFILTER_T *pstParams)
{
	int ret = RET_OK;
	static UINT32 cnt = 0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] : flag: hy:%d,vy:%d, tap: h_y:%d,v_y:%d\n",\
			pstParams->win_id,flag.h_y,flag.v_y,pstParams->hor_y_tap_type,pstParams->ver_y_tap_type);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				if((flag.h_y && pstParams->hor_y_tap_type==LX_PE_12TAP)||\
					(flag.v_y && pstParams->ver_y_tap_type==LX_PE_12TAP))
				{
					if(cnt>200)	cnt=0;
					else		cnt++;
					ret = PE_ETC_InformSclSettingCnt(cnt);
					PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_InformSclSettingCnt() error.\n",__F__,__L__);
					PE_SHP_DBG_PRINT("inform scl cnt : %d\n",cnt);
				}
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_SHP_SetReCommonCtrl(LX_PE_SHP_RE_CMN_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] : resolution common ctrl\n"\
			" mp w,b_gain:%d,%d,  sp w,b_gain:%d,%d\n"\
			" cti   : en:%d, size:%d, g:%d,%d,%d\n",\
			pstParams->win_id,pstParams->mp_white_gain,pstParams->mp_black_gain,\
			pstParams->sp_white_gain,pstParams->sp_black_gain,\
			/* cti */
			pstParams->cti_en,pstParams->tap_size,pstParams->cti_gain,\
			pstParams->ycm_y_gain,pstParams->ycm_c_gain);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Wr02(pe1_l_mp_ctrl_01,	white_gain,	GET_BITS(pstParams->mp_white_gain,0,7),\
													black_gain,	GET_BITS(pstParams->mp_black_gain,0,7));
				PE_P1L_L9B0_Wr02(pe1_l_sp_ctrl_01,	white_gain,	GET_BITS(pstParams->sp_white_gain,0,7),\
													black_gain,	GET_BITS(pstParams->sp_black_gain,0,7));
				/* cti */
				PE_P1L_L9B0_Wr03(pe1_l_cti_ctrl_00,	cti_en,		GET_BITS(pstParams->cti_en,0,1),\
													tap_size,	GET_BITS(pstParams->tap_size,0,3),\
													cti_gain,	GET_BITS(pstParams->cti_gain,0,8));
				PE_P1L_L9B0_Wr02(pe1_l_cti_ctrl_02,	ycm_y_gain,	GET_BITS(pstParams->ycm_y_gain,0,4),\
													ycm_c_gain,	GET_BITS(pstParams->ycm_c_gain,0,4));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Wr02(pe1_r_mp_ctrl_01,	white_gain,	GET_BITS(pstParams->mp_white_gain,0,7),\
													black_gain,	GET_BITS(pstParams->mp_black_gain,0,7));
				PE_P1R_L9B0_Wr02(pe1_r_sp_ctrl_01,	white_gain,	GET_BITS(pstParams->sp_white_gain,0,7),\
													black_gain,	GET_BITS(pstParams->sp_black_gain,0,7));
				/* cti */
				PE_P1R_L9B0_Wr03(pe1_r_cti_ctrl_00,	cti_en,		GET_BITS(pstParams->cti_en,0,1),\
													tap_size,	GET_BITS(pstParams->tap_size,0,3),\
													cti_gain,	GET_BITS(pstParams->cti_gain,0,8));
				PE_P1R_L9B0_Wr02(pe1_r_cti_ctrl_02,	ycm_y_gain,	GET_BITS(pstParams->ycm_y_gain,0,4),\
													ycm_c_gain,	GET_BITS(pstParams->ycm_c_gain,0,4));
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetReCommonCtrl(LX_PE_SHP_RE_CMN_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Rd02(pe1_l_mp_ctrl_01,	white_gain,	pstParams->mp_white_gain,\
													black_gain,	pstParams->mp_black_gain);
				PE_P1L_L9B0_Rd02(pe1_l_sp_ctrl_01,	white_gain,	pstParams->sp_white_gain,\
													black_gain,	pstParams->sp_black_gain);
				/* cti */
				PE_P1L_L9B0_Rd03(pe1_l_cti_ctrl_00,	cti_en,		pstParams->cti_en,\
													tap_size,	pstParams->tap_size,\
													cti_gain,	pstParams->cti_gain);
				PE_P1L_L9B0_Rd02(pe1_l_cti_ctrl_02,	ycm_y_gain,	pstParams->ycm_y_gain,\
													ycm_c_gain,	pstParams->ycm_c_gain);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Rd02(pe1_r_mp_ctrl_01,	white_gain,	pstParams->mp_white_gain,\
													black_gain,	pstParams->mp_black_gain);
				PE_P1R_L9B0_Rd02(pe1_r_sp_ctrl_01,	white_gain,	pstParams->sp_white_gain,\
													black_gain,	pstParams->sp_black_gain);
				/* cti */
				PE_P1R_L9B0_Rd03(pe1_r_cti_ctrl_00,	cti_en,		pstParams->cti_en,\
													tap_size,	pstParams->tap_size,\
													cti_gain,	pstParams->cti_gain);
				PE_P1R_L9B0_Rd02(pe1_r_cti_ctrl_02,	ycm_y_gain,	pstParams->ycm_y_gain,\
													ycm_c_gain,	pstParams->ycm_c_gain);
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_SHP_DBG_PRINT("get pstParams[%d] : resolution common ctrl\n"\
			" mp w,b_gain:%d,%d,  sp w,b_gain:%d,%d\n"\
			" cti   : en:%d, size:%d, g:%d,%d,%d\n",\
			pstParams->win_id,pstParams->mp_white_gain,pstParams->mp_black_gain,\
			pstParams->sp_white_gain,pstParams->sp_black_gain,\
			/* cti */
			pstParams->cti_en,pstParams->tap_size,pstParams->cti_gain,\
			pstParams->ycm_y_gain,pstParams->ycm_c_gain);
	} while (0);

	return ret;
}
static int PE_SHP_SetReHorizontalCtrl(LX_PE_SHP_RE_HOR_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] : resolution horizontal ctrl\n"\
			" en:%d, g:%d,%d,%d, a_gen_w:%d, mp,sp_gain:%d,%d\n",\
			pstParams->win_id,pstParams->edge_y_filter_en,pstParams->reg_csft_gain,\
			pstParams->edge_filter_white_gain,pstParams->edge_filter_black_gain,\
			pstParams->a_gen_width,pstParams->mp_horizontal_gain,pstParams->sp_horizontal_gain);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Wr01(pe1_l_derh_ctrl_00,a_gen_width,			GET_BITS(pstParams->a_gen_width,0,4));
				PE_P1L_L9B0_Wr01(pe1_l_derh_ctrl_05,reg_csft_gain, 			GET_BITS(pstParams->reg_csft_gain,0,6));
				PE_P1L_L9B0_Wr03(pe1_l_derh_ctrl_06,edge_y_filter_en, 		GET_BITS(pstParams->edge_y_filter_en,0,1),\
													edge_filter_white_gain,	GET_BITS(pstParams->edge_filter_white_gain,0,6),\
													edge_filter_black_gain,	GET_BITS(pstParams->edge_filter_black_gain,0,6));
				PE_P1L_L9B0_Wr01(pe1_l_mp_ctrl_01,	horizontal_gain,		GET_BITS(pstParams->mp_horizontal_gain,0,8));
				PE_P1L_L9B0_Wr01(pe1_l_sp_ctrl_01,	horizontal_gain,		GET_BITS(pstParams->sp_horizontal_gain,0,8));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Wr01(pe1_r_derh_ctrl_00,a_gen_width,			GET_BITS(pstParams->a_gen_width,0,4));
				PE_P1R_L9B0_Wr01(pe1_r_derh_ctrl_05,reg_csft_gain, 			GET_BITS(pstParams->reg_csft_gain,0,6));
				PE_P1R_L9B0_Wr03(pe1_r_derh_ctrl_06,edge_y_filter_en, 		GET_BITS(pstParams->edge_y_filter_en,0,1),\
													edge_filter_white_gain,	GET_BITS(pstParams->edge_filter_white_gain,0,6),\
													edge_filter_black_gain,	GET_BITS(pstParams->edge_filter_black_gain,0,6));
				PE_P1R_L9B0_Wr01(pe1_r_mp_ctrl_01,	horizontal_gain,		GET_BITS(pstParams->mp_horizontal_gain,0,8));
				PE_P1R_L9B0_Wr01(pe1_r_sp_ctrl_01,	horizontal_gain,		GET_BITS(pstParams->sp_horizontal_gain,0,8));
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetReHorizontalCtrl(LX_PE_SHP_RE_HOR_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Rd01(pe1_l_derh_ctrl_00,a_gen_width,			pstParams->a_gen_width);
				PE_P1L_L9B0_Rd01(pe1_l_derh_ctrl_05,reg_csft_gain, 			pstParams->reg_csft_gain);
				PE_P1L_L9B0_Rd03(pe1_l_derh_ctrl_06,edge_y_filter_en, 		pstParams->edge_y_filter_en,\
													edge_filter_white_gain,	pstParams->edge_filter_white_gain,\
													edge_filter_black_gain,	pstParams->edge_filter_black_gain);
				PE_P1L_L9B0_Rd01(pe1_l_mp_ctrl_01,	horizontal_gain,		pstParams->mp_horizontal_gain);
				PE_P1L_L9B0_Rd01(pe1_l_sp_ctrl_01,	horizontal_gain,		pstParams->sp_horizontal_gain);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Rd01(pe1_r_derh_ctrl_00,a_gen_width,			pstParams->a_gen_width);
				PE_P1R_L9B0_Rd01(pe1_r_derh_ctrl_05,reg_csft_gain, 			pstParams->reg_csft_gain);
				PE_P1R_L9B0_Rd03(pe1_r_derh_ctrl_06,edge_y_filter_en, 		pstParams->edge_y_filter_en,\
													edge_filter_white_gain,	pstParams->edge_filter_white_gain,\
													edge_filter_black_gain,	pstParams->edge_filter_black_gain);
				PE_P1R_L9B0_Rd01(pe1_r_mp_ctrl_01,	horizontal_gain,		pstParams->mp_horizontal_gain);
				PE_P1R_L9B0_Rd01(pe1_r_sp_ctrl_01,	horizontal_gain,		pstParams->sp_horizontal_gain);
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_SHP_DBG_PRINT("get pstParams[%d] : resolution horizontal ctrl\n"\
			" en:%d, g:%d,%d,%d, a_gen_w:%d, mp,sp_gain:%d,%d\n",\
			pstParams->win_id,pstParams->edge_y_filter_en,pstParams->reg_csft_gain,\
			pstParams->edge_filter_white_gain,pstParams->edge_filter_black_gain,\
			pstParams->a_gen_width,pstParams->mp_horizontal_gain,pstParams->sp_horizontal_gain);
	} while (0);

	return ret;
}
static int PE_SHP_SetReVerticalCtrl(LX_PE_SHP_RE_VER_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] : resolution vertical ctrl\n"\
			" en:%d, th:%d, g:%d,%d,%d, der_g:%d, mmd_sel:%d, mp,sp_g:%d,%d\n",\
			pstParams->win_id,pstParams->der_v_en,pstParams->bif_manual_th,pstParams->csft_gain,\
			pstParams->gain_b,pstParams->gain_w,pstParams->der_gain_mapping,pstParams->mmd_sel,\
			pstParams->mp_vertical_gain,pstParams->sp_vertical_gain);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Wr03(pe1_l_derv_ctrl_0,	der_v_en,			GET_BITS(pstParams->der_v_en,0,1),\
													bif_manual_th,		GET_BITS(pstParams->bif_manual_th,0,8),\
													der_gain_mapping,	GET_BITS(pstParams->der_gain_mapping,0,2));
				PE_P1L_L9B0_Wr01(pe1_l_derv_ctrl_1,	csft_gain,			GET_BITS(pstParams->csft_gain,0,6));
				PE_P1L_L9B0_Wr01(pe1_l_derv_ctrl_2,	mmd_sel,			GET_BITS(pstParams->mmd_sel,0,3));
				PE_P1L_L9B0_Wr02(pe1_l_derv_ctrl_3,	gain_b,				GET_BITS(pstParams->gain_b,0,7),\
													gain_w,				GET_BITS(pstParams->gain_w,0,7));
				PE_P1L_L9B0_Wr01(pe1_l_mp_ctrl_01,	vertical_gain,		GET_BITS(pstParams->mp_vertical_gain,0,8));
				PE_P1L_L9B0_Wr01(pe1_l_sp_ctrl_01,	vertical_gain,		GET_BITS(pstParams->sp_vertical_gain,0,8));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Wr03(pe1_r_derv_ctrl_0,	der_v_en,			GET_BITS(pstParams->der_v_en,0,1),\
													bif_manual_th,		GET_BITS(pstParams->bif_manual_th,0,8),\
													der_gain_mapping,	GET_BITS(pstParams->der_gain_mapping,0,2));
				PE_P1R_L9B0_Wr01(pe1_r_derv_ctrl_1,	csft_gain,			GET_BITS(pstParams->csft_gain,0,6));
				PE_P1R_L9B0_Wr01(pe1_r_derv_ctrl_2,	mmd_sel,			GET_BITS(pstParams->mmd_sel,0,3));
				PE_P1R_L9B0_Wr02(pe1_r_derv_ctrl_3,	gain_b,				GET_BITS(pstParams->gain_b,0,7),\
													gain_w,				GET_BITS(pstParams->gain_w,0,7));
				PE_P1R_L9B0_Wr01(pe1_r_mp_ctrl_01,	vertical_gain,		GET_BITS(pstParams->mp_vertical_gain,0,8));
				PE_P1R_L9B0_Wr01(pe1_r_sp_ctrl_01,	vertical_gain,		GET_BITS(pstParams->sp_vertical_gain,0,8));
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetReVerticalCtrl(LX_PE_SHP_RE_VER_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Rd03(pe1_l_derv_ctrl_0,	der_v_en,			pstParams->der_v_en,\
													bif_manual_th,		pstParams->bif_manual_th,\
													der_gain_mapping,	pstParams->der_gain_mapping);
				PE_P1L_L9B0_Rd01(pe1_l_derv_ctrl_1,	csft_gain,			pstParams->csft_gain);
				PE_P1L_L9B0_Rd01(pe1_l_derv_ctrl_2,	mmd_sel,			pstParams->mmd_sel);
				PE_P1L_L9B0_Rd02(pe1_l_derv_ctrl_3,	gain_b,				pstParams->gain_b,\
													gain_w,				pstParams->gain_w);
				PE_P1L_L9B0_Rd01(pe1_l_mp_ctrl_01,	vertical_gain,		pstParams->mp_vertical_gain);
				PE_P1L_L9B0_Rd01(pe1_l_sp_ctrl_01,	vertical_gain,		pstParams->sp_vertical_gain);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Rd03(pe1_r_derv_ctrl_0,	der_v_en,			pstParams->der_v_en,\
													bif_manual_th,		pstParams->bif_manual_th,\
													der_gain_mapping,	pstParams->der_gain_mapping);
				PE_P1R_L9B0_Rd01(pe1_r_derv_ctrl_1,	csft_gain,			pstParams->csft_gain);
				PE_P1R_L9B0_Rd01(pe1_r_derv_ctrl_2,	mmd_sel,			pstParams->mmd_sel);
				PE_P1R_L9B0_Rd02(pe1_r_derv_ctrl_3,	gain_b,				pstParams->gain_b,\
													gain_w,				pstParams->gain_w);
				PE_P1R_L9B0_Rd01(pe1_r_mp_ctrl_01,	vertical_gain,		pstParams->mp_vertical_gain);
				PE_P1R_L9B0_Rd01(pe1_r_sp_ctrl_01,	vertical_gain,		pstParams->sp_vertical_gain);
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_SHP_DBG_PRINT("get pstParams[%d] : resolution vertical ctrl\n"\
			" en:%d, th:%d, g:%d,%d,%d, der_g:%d, mmd_sel:%d, mp,sp_g:%d,%d\n",\
			pstParams->win_id,pstParams->der_v_en,pstParams->bif_manual_th,pstParams->csft_gain,\
			pstParams->gain_b,pstParams->gain_w,pstParams->der_gain_mapping,pstParams->mmd_sel,\
			pstParams->mp_vertical_gain,pstParams->sp_vertical_gain);
	} while (0);

	return ret;
}
static int PE_SHP_SetReMiscCtrl(LX_PE_SHP_RE_MISC_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] :  resolution misc ctrl\n"\
			" e-map : sel:%d, ga:%d,%d,%d\n"\
			" t-map : sel:%d,%d, g:%d, gt:%d,%d,%d,%d,%d,%d,%d, cor1:%d,var_th:%d\n"\
			" d-jag : en:%d, th:%d,%d, g:%d\n",\
			pstParams->win_id,\
			/* e-map */
			pstParams->amap2_sel,pstParams->ga_max,pstParams->ga_th0,pstParams->ga_th1,\
			/* t-map */
			pstParams->amap1_sel,pstParams->max_sel,pstParams->tmap_gain,pstParams->gt_th0,\
			pstParams->gt_th0a,pstParams->gt_th0b,pstParams->gt_th1,pstParams->gt_gain0a,\
			pstParams->gt_gain0b,pstParams->gt_max,pstParams->coring_mode1,pstParams->var_th,\
			/* d-jag */
			pstParams->center_blur_en,pstParams->level_th,pstParams->protect_th,pstParams->n_avg_gain);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				PE_FWI_PQ_TABLE_HEAD_T header;
				PE_FWI_L9_PQ_IPC_CTRL table;
				UINT32 tbl_size;
				UINT8 *pTable = NULL;

				pTable = (UINT8 *)&table;
				tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = pstParams->win_id;
				header.offset = PE_FWI_OFFSET(PE_FWI_L9_SHP_CORING_CTRL_05);
				header.length = sizeof(PE_FWI_L9_SHP_CORING_CTRL_05)+sizeof(PE_FWI_L9_SHP_CORING_CTRL_06);
				table.shp_coring_ctrl_05.gt_th0 = GET_BITS(pstParams->gt_th0,0,8);
				table.shp_coring_ctrl_05.gt_th1 = GET_BITS(pstParams->gt_th1,0,8);
				table.shp_coring_ctrl_06.gt_th0a = GET_BITS(pstParams->gt_th0a,0,8);
				table.shp_coring_ctrl_06.gt_th0b = GET_BITS(pstParams->gt_th0b,0,8);

				PE_SHP_DBG_PRINT("[%s,%d]wId:%d,ofs:%d,len:%d, tbl_size:%d\n"\
					"gt_th0,1:%d,%d, gt_th0a,b:%d,%d\n",__F__,__L__,\
					header.win_id,header.offset,header.length,tbl_size,\
					table.shp_coring_ctrl_05.gt_th0,table.shp_coring_ctrl_05.gt_th1,\
					table.shp_coring_ctrl_06.gt_th0a,table.shp_coring_ctrl_06.gt_th0b);
				ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);

				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					_g_shp_coring_ctrl_05[LX_PE_WIN_MAIN].gt_th0	= table.shp_coring_ctrl_05.gt_th0;
					_g_shp_coring_ctrl_05[LX_PE_WIN_MAIN].gt_th1	= table.shp_coring_ctrl_05.gt_th1;
					_g_shp_coring_ctrl_06[LX_PE_WIN_MAIN].gt_th0a	= table.shp_coring_ctrl_06.gt_th0a;
					_g_shp_coring_ctrl_06[LX_PE_WIN_MAIN].gt_th0b	= table.shp_coring_ctrl_06.gt_th0b;
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					_g_shp_coring_ctrl_05[LX_PE_WIN_SUB].gt_th0		= table.shp_coring_ctrl_05.gt_th0;
					_g_shp_coring_ctrl_05[LX_PE_WIN_SUB].gt_th1		= table.shp_coring_ctrl_05.gt_th1;
					_g_shp_coring_ctrl_06[LX_PE_WIN_SUB].gt_th0a	= table.shp_coring_ctrl_06.gt_th0a;
					_g_shp_coring_ctrl_06[LX_PE_WIN_SUB].gt_th0b	= table.shp_coring_ctrl_06.gt_th0b;
				}
				PE_ETC_SetPqFwWindow((UINT32)pstParams->win_id);
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					/* map */
					PE_P1L_L9B0_Wr02(pe1_l_coring_ctrl_05,	gt_th1,		GET_BITS(pstParams->gt_th1,0,8),\
															gt_th0,		GET_BITS(pstParams->gt_th0,0,8));
					PE_P1L_L9B0_Wr02(pe1_l_coring_ctrl_06,	gt_th0a,	GET_BITS(pstParams->gt_th0a,0,8),\
															gt_th0b,	GET_BITS(pstParams->gt_th0b,0,8));
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					/* map */
					PE_P1R_L9B0_Wr02(pe1_r_coring_ctrl_05,	gt_th1,		GET_BITS(pstParams->gt_th1,0,8),\
															gt_th0,		GET_BITS(pstParams->gt_th0,0,8));
					PE_P1R_L9B0_Wr02(pe1_r_coring_ctrl_06,	gt_th0a,	GET_BITS(pstParams->gt_th0a,0,8),\
															gt_th0b,	GET_BITS(pstParams->gt_th0b,0,8));
				}
			}

			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				/* map */
				PE_P1L_L9B0_Wr02(pe1_l_coring_ctrl_04,	coring_mode1,GET_BITS(pstParams->coring_mode1,0,2),\
														var_th,		GET_BITS(pstParams->var_th,0,11));
				PE_P1L_L9B0_Wr01(pe1_l_coring_ctrl_05,	gt_max,		GET_BITS(pstParams->gt_max,0,6));
				PE_P1L_L9B0_Wr02(pe1_l_coring_ctrl_06,	gt_gain0a,	GET_BITS(pstParams->gt_gain0a,0,6),\
														gt_gain0b,	GET_BITS(pstParams->gt_gain0b,0,6));
				PE_P1L_L9B0_Wr05(pe1_l_coring_ctrl_00,	amap2_sel,	GET_BITS(pstParams->amap2_sel,0,2),\
														ga_max,		GET_BITS(pstParams->ga_max,0,6),\
														ga_th0,		GET_BITS(pstParams->ga_th0,0,8),\
														ga_th1,		GET_BITS(pstParams->ga_th1,0,8),\
														amap1_sel,	GET_BITS(pstParams->amap1_sel,0,2));
				PE_P1L_L9B0_Wr01(pe1_l_coring_ctrl_01,	max_sel,	GET_BITS(pstParams->max_sel,0,3));
				PE_P1L_L9B0_Wr01(pe1_l_coring_ctrl_07,	tmap_gain,	GET_BITS(pstParams->tmap_gain,0,7));
				/* d-jag */
				PE_P1L_L9B0_Wr01(pe1_l_dj_ctrl_00,	center_blur_en,	GET_BITS(pstParams->center_blur_en,0,1));
				PE_P1L_L9B0_Wr03(pe1_l_dj_ctrl_01,	level_th,		GET_BITS(pstParams->level_th,0,8),\
													protect_th,		GET_BITS(pstParams->protect_th,0,8),\
													n_avg_gain,		GET_BITS(pstParams->n_avg_gain,0,8));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				/* map */
				PE_P1R_L9B0_Wr02(pe1_r_coring_ctrl_04,	coring_mode1,GET_BITS(pstParams->coring_mode1,0,2),\
														var_th,		GET_BITS(pstParams->var_th,0,11));
				PE_P1R_L9B0_Wr01(pe1_r_coring_ctrl_05,	gt_max,		GET_BITS(pstParams->gt_max,0,6));
				PE_P1R_L9B0_Wr02(pe1_r_coring_ctrl_06,	gt_gain0a,	GET_BITS(pstParams->gt_gain0a,0,6),\
														gt_gain0b,	GET_BITS(pstParams->gt_gain0b,0,6));
				PE_P1R_L9B0_Wr05(pe1_r_coring_ctrl_00,	amap2_sel,	GET_BITS(pstParams->amap2_sel,0,2),\
														ga_max,		GET_BITS(pstParams->ga_max,0,6),\
														ga_th0,		GET_BITS(pstParams->ga_th0,0,8),\
														ga_th1,		GET_BITS(pstParams->ga_th1,0,8),\
														amap1_sel,	GET_BITS(pstParams->amap1_sel,0,2));
				PE_P1R_L9B0_Wr01(pe1_r_coring_ctrl_01,	max_sel,	GET_BITS(pstParams->max_sel,0,3));
				PE_P1R_L9B0_Wr01(pe1_r_coring_ctrl_07,	tmap_gain,	GET_BITS(pstParams->tmap_gain,0,7));
				/* d-jag */
				PE_P1R_L9B0_Wr01(pe1_r_dj_ctrl_00,	center_blur_en,	GET_BITS(pstParams->center_blur_en,0,1));
				PE_P1R_L9B0_Wr03(pe1_r_dj_ctrl_01,	level_th,		GET_BITS(pstParams->level_th,0,8),\
													protect_th,		GET_BITS(pstParams->protect_th,0,8),\
													n_avg_gain,		GET_BITS(pstParams->n_avg_gain,0,8));
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetReMiscCtrl(LX_PE_SHP_RE_MISC_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_SHP_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
				if(g_pe_use_fw)
				{
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						pstParams->gt_th0 = _g_shp_coring_ctrl_05[LX_PE_WIN_MAIN].gt_th0;
						pstParams->gt_th1 = _g_shp_coring_ctrl_05[LX_PE_WIN_MAIN].gt_th1;
						pstParams->gt_th0a = _g_shp_coring_ctrl_06[LX_PE_WIN_MAIN].gt_th0a;
						pstParams->gt_th0b = _g_shp_coring_ctrl_06[LX_PE_WIN_MAIN].gt_th0b;
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						pstParams->gt_th0 = _g_shp_coring_ctrl_05[LX_PE_WIN_SUB].gt_th0;
						pstParams->gt_th1 = _g_shp_coring_ctrl_05[LX_PE_WIN_SUB].gt_th1;
						pstParams->gt_th0a = _g_shp_coring_ctrl_06[LX_PE_WIN_SUB].gt_th0a;
						pstParams->gt_th0b = _g_shp_coring_ctrl_06[LX_PE_WIN_SUB].gt_th0b;
					}
				}
				else
				{
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						/* map */
						PE_P1L_L9B0_Rd02(pe1_l_coring_ctrl_05,	gt_th1,		pstParams->gt_th1,\
																gt_th0,		pstParams->gt_th0);
						PE_P1L_L9B0_Rd02(pe1_l_coring_ctrl_06,	gt_th0a,	pstParams->gt_th0a,\
																gt_th0b,	pstParams->gt_th0b);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						/* map */
						PE_P1R_L9B0_Rd02(pe1_r_coring_ctrl_05,	gt_th1,		pstParams->gt_th1,\
																gt_th0,		pstParams->gt_th0);
						PE_P1R_L9B0_Rd02(pe1_r_coring_ctrl_06,	gt_th0a,	pstParams->gt_th0a,\
																gt_th0b,	pstParams->gt_th0b);
					}
				}
				/* map */
				PE_P1L_L9B0_Rd05(pe1_l_coring_ctrl_00,	amap2_sel,	pstParams->amap2_sel,\
														ga_max,		pstParams->ga_max,\
														ga_th0,		pstParams->ga_th0,\
														ga_th1,		pstParams->ga_th1,\
														amap1_sel,	pstParams->amap1_sel);
				PE_P1L_L9B0_Rd01(pe1_l_coring_ctrl_01,	max_sel,	pstParams->max_sel);
				PE_P1L_L9B0_Rd02(pe1_l_coring_ctrl_04,	coring_mode1,pstParams->coring_mode1,\
														var_th,		pstParams->var_th);
				PE_P1L_L9B0_Rd01(pe1_l_coring_ctrl_05,	gt_max,		pstParams->gt_max);
				PE_P1L_L9B0_Rd02(pe1_l_coring_ctrl_06,	gt_gain0a,	pstParams->gt_gain0a,\
														gt_gain0b,	pstParams->gt_gain0b);
				PE_P1L_L9B0_Rd01(pe1_l_coring_ctrl_07,	tmap_gain,	pstParams->tmap_gain);
				/* d-jag */
				PE_P1L_L9B0_Rd01(pe1_l_dj_ctrl_00,	center_blur_en,	pstParams->center_blur_en);
				PE_P1L_L9B0_Rd03(pe1_l_dj_ctrl_01,	level_th,		pstParams->level_th,\
													protect_th,		pstParams->protect_th,\
													n_avg_gain,		pstParams->n_avg_gain);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				/* map */
				PE_P1R_L9B0_Rd05(pe1_r_coring_ctrl_00,	amap2_sel,	pstParams->amap2_sel,\
														ga_max,		pstParams->ga_max,\
														ga_th0,		pstParams->ga_th0,\
														ga_th1,		pstParams->ga_th1,\
														amap1_sel,	pstParams->amap1_sel);
				PE_P1R_L9B0_Rd01(pe1_r_coring_ctrl_01,	max_sel,	pstParams->max_sel);
				PE_P1R_L9B0_Rd02(pe1_r_coring_ctrl_04,	coring_mode1,pstParams->coring_mode1,\
														var_th,		pstParams->var_th);
				PE_P1R_L9B0_Rd01(pe1_r_coring_ctrl_05,	gt_max,		pstParams->gt_max);
				PE_P1R_L9B0_Rd02(pe1_r_coring_ctrl_06,	gt_gain0a,	pstParams->gt_gain0a,\
														gt_gain0b,	pstParams->gt_gain0b);
				PE_P1R_L9B0_Rd01(pe1_r_coring_ctrl_07,	tmap_gain,	pstParams->tmap_gain);
				/* d-jag */
				PE_P1R_L9B0_Rd01(pe1_r_dj_ctrl_00,	center_blur_en,	pstParams->center_blur_en);
				PE_P1R_L9B0_Rd03(pe1_r_dj_ctrl_01,	level_th,		pstParams->level_th,\
													protect_th,		pstParams->protect_th,\
													n_avg_gain,		pstParams->n_avg_gain);
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_SHP_DBG_PRINT("get pstParams[%d] :  resolution misc ctrl\n"\
			" e-map : sel:%d, ga:%d,%d,%d\n"\
			" t-map : sel:%d,%d, g:%d, gt:%d,%d,%d,%d,%d,%d,%d, cor1:%d,var_th:%d\n"\
			" d-jag : en:%d, th:%d,%d, g:%d\n",\
			pstParams->win_id,\
			/* e-map */
			pstParams->amap2_sel,pstParams->ga_max,pstParams->ga_th0,pstParams->ga_th1,\
			/* t-map */
			pstParams->amap1_sel,pstParams->max_sel,pstParams->tmap_gain,pstParams->gt_th0,\
			pstParams->gt_th0a,pstParams->gt_th0b,pstParams->gt_th1,pstParams->gt_gain0a,\
			pstParams->gt_gain0b,pstParams->gt_max,pstParams->coring_mode1,pstParams->var_th,\
			/* d-jag */
			pstParams->center_blur_en,pstParams->level_th,pstParams->protect_th,pstParams->n_avg_gain);
	} while (0);

	return ret;
}
static int PE_SHP_SetEdgeEnhanceCtrl(LX_PE_SHP_EE_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] :  edge enhance ctrl\n"\
			" mp g_b,w:%d,%d,  sp g_b,w:%d,%d\n",\
			pstParams->win_id,pstParams->mp_edge_gain_b,pstParams->mp_edge_gain_w,\
			pstParams->sp_edge_gain_b,pstParams->sp_edge_gain_w);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				UINT8 wId;
				PE_FWI_PQ_TABLE_HEAD_T header;
				PE_FWI_L9_PQ_IPC_CTRL table;
				UINT32 tbl_size;
				UINT8 *pTable = NULL;

				pTable = (UINT8 *)&table;
				tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = pstParams->win_id;
				header.offset = PE_FWI_OFFSET(PE_FWI_L9_SHP_CORING_CTRL_09);
				header.length = sizeof(PE_FWI_L9_SHP_CORING_CTRL_09)+sizeof(PE_FWI_L9_SHP_DP_CTRL_01);

				wId = (pstParams->win_id==LX_PE_WIN_ALL)? 0:(UINT8)pstParams->win_id;
				table.shp_coring_ctrl_09.edge_gain_b = GET_BITS(pstParams->mp_edge_gain_b,0,7);
				table.shp_coring_ctrl_09.edge_gain_w = GET_BITS(pstParams->mp_edge_gain_w,0,7);
				table.shp_coring_ctrl_09.texture_gain_b = _g_shp_coring_ctrl_09[wId].texture_gain_b;
				table.shp_coring_ctrl_09.texture_gain_w = _g_shp_coring_ctrl_09[wId].texture_gain_w;
				table.shp_dp_ctrl_01.edge_gain_b = GET_BITS(pstParams->sp_edge_gain_b,0,7);
				table.shp_dp_ctrl_01.edge_gain_w = GET_BITS(pstParams->sp_edge_gain_w,0,7);
				table.shp_dp_ctrl_01.texture_gain_b = _g_shp_dp_ctrl_01[wId].texture_gain_b;
				table.shp_dp_ctrl_01.texture_gain_w = _g_shp_dp_ctrl_01[wId].texture_gain_w;

				PE_SHP_DBG_PRINT("[%s,%d]wId:%d,ofs:%d,len:%d, tbl_size:%d\n"\
					"coring  edge_gain_b,w:%d,%d, texture_gain_b,w:%d,%d\n"\
					"dp      edge_gain_b,w:%d,%d, texture_gain_b,w:%d,%d\n",__F__,__L__,\
					header.win_id,header.offset,header.length,tbl_size,\
					table.shp_coring_ctrl_09.edge_gain_b,table.shp_coring_ctrl_09.edge_gain_w,\
					table.shp_coring_ctrl_09.texture_gain_b,table.shp_coring_ctrl_09.texture_gain_w,\
					table.shp_dp_ctrl_01.edge_gain_b,table.shp_dp_ctrl_01.edge_gain_w,\
					table.shp_dp_ctrl_01.texture_gain_b,table.shp_dp_ctrl_01.texture_gain_w);
				ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);

				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					_g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].edge_gain_b	= table.shp_coring_ctrl_09.edge_gain_b;
					_g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].edge_gain_w	= table.shp_coring_ctrl_09.edge_gain_w;
					_g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].edge_gain_b		= table.shp_dp_ctrl_01.edge_gain_b;
					_g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].edge_gain_w		= table.shp_dp_ctrl_01.edge_gain_w;
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					_g_shp_coring_ctrl_09[LX_PE_WIN_SUB].edge_gain_b	= table.shp_coring_ctrl_09.edge_gain_b;
					_g_shp_coring_ctrl_09[LX_PE_WIN_SUB].edge_gain_w	= table.shp_coring_ctrl_09.edge_gain_w;
					_g_shp_dp_ctrl_01[LX_PE_WIN_SUB].edge_gain_b		= table.shp_dp_ctrl_01.edge_gain_b;
					_g_shp_dp_ctrl_01[LX_PE_WIN_SUB].edge_gain_w		= table.shp_dp_ctrl_01.edge_gain_w;
				}
				PE_ETC_SetPqFwWindow((UINT32)pstParams->win_id);
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					PE_P1L_L9B0_Wr02(pe1_l_coring_ctrl_09,	edge_gain_b,	GET_BITS(pstParams->mp_edge_gain_b,0,7),\
															edge_gain_w,	GET_BITS(pstParams->mp_edge_gain_w,0,7));
					PE_P1L_L9B0_Wr02(pe1_l_dp_ctrl_01,		edge_gain_b,	GET_BITS(pstParams->sp_edge_gain_b,0,7),\
															edge_gain_w,	GET_BITS(pstParams->sp_edge_gain_w,0,7));
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					PE_P1R_L9B0_Wr02(pe1_r_coring_ctrl_09,	edge_gain_b,	GET_BITS(pstParams->mp_edge_gain_b,0,7),\
															edge_gain_w,	GET_BITS(pstParams->mp_edge_gain_w,0,7));
					PE_P1R_L9B0_Wr02(pe1_r_dp_ctrl_01,		edge_gain_b,	GET_BITS(pstParams->sp_edge_gain_b,0,7),\
															edge_gain_w,	GET_BITS(pstParams->sp_edge_gain_w,0,7));
				}
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetEdgeEnhanceCtrl(LX_PE_SHP_EE_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					pstParams->mp_edge_gain_b	= _g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].edge_gain_b;
					pstParams->mp_edge_gain_w	= _g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].edge_gain_w;
					pstParams->sp_edge_gain_b	= _g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].edge_gain_b; 
					pstParams->sp_edge_gain_w	= _g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].edge_gain_w;
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					pstParams->mp_edge_gain_b	= _g_shp_coring_ctrl_09[LX_PE_WIN_SUB].edge_gain_b;
					pstParams->mp_edge_gain_w	= _g_shp_coring_ctrl_09[LX_PE_WIN_SUB].edge_gain_w;
					pstParams->sp_edge_gain_b	= _g_shp_dp_ctrl_01[LX_PE_WIN_SUB].edge_gain_b; 
					pstParams->sp_edge_gain_w	= _g_shp_dp_ctrl_01[LX_PE_WIN_SUB].edge_gain_w;
				}
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					PE_P1L_L9B0_Rd02(pe1_l_coring_ctrl_09,	edge_gain_b,	pstParams->mp_edge_gain_b,\
															edge_gain_w,	pstParams->mp_edge_gain_w);
					PE_P1L_L9B0_Rd02(pe1_l_dp_ctrl_01,		edge_gain_b,	pstParams->sp_edge_gain_b,\
															edge_gain_w,	pstParams->sp_edge_gain_w);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					PE_P1R_L9B0_Rd02(pe1_r_coring_ctrl_09,	edge_gain_b,	pstParams->mp_edge_gain_b,\
															edge_gain_w,	pstParams->mp_edge_gain_w);
					PE_P1R_L9B0_Rd02(pe1_r_dp_ctrl_01,		edge_gain_b,	pstParams->sp_edge_gain_b,\
															edge_gain_w,	pstParams->sp_edge_gain_w);
				}
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_SHP_DBG_PRINT("get pstParams[%d] :  edge enhance ctrl\n"\
			" mp g_b,w:%d,%d,  sp g_b,w:%d,%d\n",\
			pstParams->win_id,pstParams->mp_edge_gain_b,pstParams->mp_edge_gain_w,\
			pstParams->sp_edge_gain_b,pstParams->sp_edge_gain_w);
	} while (0);

	return ret;
}
static int PE_SHP_SetDetailEnhanceCtrl(LX_PE_SHP_DE_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_SHP_DBG_PRINT("set pstParams[%d] :  detail enhance ctrl\n"\
			" mp g_b,w:%d,%d,  sp g_b,w:%d,%d\n",\
			pstParams->win_id,pstParams->mp_texture_gain_b,pstParams->mp_texture_gain_w,\
			pstParams->sp_texture_gain_b,pstParams->sp_texture_gain_w);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				UINT8 wId;
				PE_FWI_PQ_TABLE_HEAD_T header;
				PE_FWI_L9_PQ_IPC_CTRL table;
				UINT32 tbl_size;
				UINT8 *pTable = NULL;

				pTable = (UINT8 *)&table;
				tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = pstParams->win_id;
				header.offset = PE_FWI_OFFSET(PE_FWI_L9_SHP_CORING_CTRL_09);
				header.length = sizeof(PE_FWI_L9_SHP_CORING_CTRL_09)+sizeof(PE_FWI_L9_SHP_DP_CTRL_01);

				wId = (pstParams->win_id==LX_PE_WIN_ALL)? 0:(UINT8)pstParams->win_id;
				table.shp_coring_ctrl_09.edge_gain_b = _g_shp_coring_ctrl_09[wId].edge_gain_b;
				table.shp_coring_ctrl_09.edge_gain_w = _g_shp_coring_ctrl_09[wId].edge_gain_w;
				table.shp_coring_ctrl_09.texture_gain_b = GET_BITS(pstParams->mp_texture_gain_b,0,7);
				table.shp_coring_ctrl_09.texture_gain_w = GET_BITS(pstParams->mp_texture_gain_w,0,7);
				table.shp_dp_ctrl_01.edge_gain_b = _g_shp_dp_ctrl_01[wId].edge_gain_b;
				table.shp_dp_ctrl_01.edge_gain_w = _g_shp_dp_ctrl_01[wId].edge_gain_w;
				table.shp_dp_ctrl_01.texture_gain_b = GET_BITS(pstParams->sp_texture_gain_b,0,7);
				table.shp_dp_ctrl_01.texture_gain_w = GET_BITS(pstParams->sp_texture_gain_w,0,7);

				PE_SHP_DBG_PRINT("[%s,%d]wId:%d,ofs:%d,len:%d, tbl_size:%d\n"\
					"coring  edge_gain_b,w:%d,%d, texture_gain_b,w:%d,%d\n"\
					"dp      edge_gain_b,w:%d,%d, texture_gain_b,w:%d,%d\n",__F__,__L__,\
					header.win_id,header.offset,header.length,tbl_size,\
					table.shp_coring_ctrl_09.edge_gain_b,table.shp_coring_ctrl_09.edge_gain_w,\
					table.shp_coring_ctrl_09.texture_gain_b,table.shp_coring_ctrl_09.texture_gain_w,\
					table.shp_dp_ctrl_01.edge_gain_b,table.shp_dp_ctrl_01.edge_gain_w,\
					table.shp_dp_ctrl_01.texture_gain_b,table.shp_dp_ctrl_01.texture_gain_w);
				ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
				PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);

				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					_g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].texture_gain_b	= table.shp_coring_ctrl_09.texture_gain_b;
					_g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].texture_gain_w	= table.shp_coring_ctrl_09.texture_gain_w;
					_g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].texture_gain_b		= table.shp_dp_ctrl_01.texture_gain_b;
					_g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].texture_gain_w		= table.shp_dp_ctrl_01.texture_gain_w;
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					_g_shp_coring_ctrl_09[LX_PE_WIN_SUB].texture_gain_b	= table.shp_coring_ctrl_09.texture_gain_b;
					_g_shp_coring_ctrl_09[LX_PE_WIN_SUB].texture_gain_w	= table.shp_coring_ctrl_09.texture_gain_w;
					_g_shp_dp_ctrl_01[LX_PE_WIN_SUB].texture_gain_b		= table.shp_dp_ctrl_01.texture_gain_b;
					_g_shp_dp_ctrl_01[LX_PE_WIN_SUB].texture_gain_w		= table.shp_dp_ctrl_01.texture_gain_w;
				}
				PE_ETC_SetPqFwWindow((UINT32)pstParams->win_id);
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					PE_P1L_L9B0_Wr02(pe1_l_coring_ctrl_09,	texture_gain_b,	GET_BITS(pstParams->mp_texture_gain_b,0,7),\
															texture_gain_w,	GET_BITS(pstParams->mp_texture_gain_w,0,7));
					PE_P1L_L9B0_Wr02(pe1_l_dp_ctrl_01,		texture_gain_b,	GET_BITS(pstParams->sp_texture_gain_b,0,7),\
															texture_gain_w,	GET_BITS(pstParams->sp_texture_gain_w,0,7));
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					PE_P1R_L9B0_Wr02(pe1_r_coring_ctrl_09,	texture_gain_b,	GET_BITS(pstParams->mp_texture_gain_b,0,7),\
															texture_gain_w,	GET_BITS(pstParams->mp_texture_gain_w,0,7));
					PE_P1R_L9B0_Wr02(pe1_r_dp_ctrl_01,		texture_gain_b,	GET_BITS(pstParams->sp_texture_gain_b,0,7),\
															texture_gain_w,	GET_BITS(pstParams->sp_texture_gain_w,0,7));
				}
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_SHP_GetDetailEnhanceCtrl(LX_PE_SHP_DE_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_SHP_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					pstParams->mp_texture_gain_b	= _g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].texture_gain_b;
					pstParams->mp_texture_gain_w	= _g_shp_coring_ctrl_09[LX_PE_WIN_MAIN].texture_gain_w;
					pstParams->sp_texture_gain_b	= _g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].texture_gain_b; 
					pstParams->sp_texture_gain_w	= _g_shp_dp_ctrl_01[LX_PE_WIN_MAIN].texture_gain_w;
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					pstParams->mp_texture_gain_b	= _g_shp_coring_ctrl_09[LX_PE_WIN_SUB].texture_gain_b;
					pstParams->mp_texture_gain_w	= _g_shp_coring_ctrl_09[LX_PE_WIN_SUB].texture_gain_w;
					pstParams->sp_texture_gain_b	= _g_shp_dp_ctrl_01[LX_PE_WIN_SUB].texture_gain_b; 
					pstParams->sp_texture_gain_w	= _g_shp_dp_ctrl_01[LX_PE_WIN_SUB].texture_gain_w;
				}
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					PE_P1L_L9B0_Rd02(pe1_l_coring_ctrl_09,	texture_gain_b,	pstParams->mp_texture_gain_b,\
															texture_gain_w,	pstParams->mp_texture_gain_w);
					PE_P1L_L9B0_Rd02(pe1_l_dp_ctrl_01,		texture_gain_b,	pstParams->sp_texture_gain_b,\
															texture_gain_w,	pstParams->sp_texture_gain_w);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					PE_P1R_L9B0_Rd02(pe1_r_coring_ctrl_09,	texture_gain_b,	pstParams->mp_texture_gain_b,\
															texture_gain_w,	pstParams->mp_texture_gain_w);
					PE_P1R_L9B0_Rd02(pe1_r_dp_ctrl_01,		texture_gain_b,	pstParams->sp_texture_gain_b,\
															texture_gain_w,	pstParams->sp_texture_gain_w);
				}
			}
		}
		else
		{
			PE_SHP_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_SHP_DBG_PRINT("set pstParams[%d] :  detail enhance ctrl\n"\
			" mp g_b,w:%d,%d,  sp g_b,w:%d,%d\n",\
			pstParams->win_id,pstParams->mp_texture_gain_b,pstParams->mp_texture_gain_w,\
			pstParams->sp_texture_gain_b,pstParams->sp_texture_gain_w);
	} while (0);

	return ret;
}

#ifdef PE_PROTECTION_SCL_12TAP_ON_HD_INPUT
int PE_SHP_SetMainSclMode(void)
{
	int ret = RET_OK;
	static UINT32 count = 0;
	PE_SHP_SETFLAG_T flag;
	LX_PE_SHP_SCLFILTER_T cur0_scl;
	UINT32 scl_h_size = 0;
	UINT32 scaler_config,adap_filter_en,type_sel_luma,type_sel_chroma;
	LX_PE_SHP_SCLFILTER_T *pre0_scl = &_g_shp_scl_filter[LX_PE_WIN_MAIN];
	do{
		/* skip loop before init */
		cur0_scl.win_id = _g_cur_scl_filter[LX_PE_WIN_MAIN].win_id;
		if(cur0_scl.win_id==0xff)
		{
			PE_SHP_DBG_PRINT("cur0_scl.win_id:%d, count:%d. just break\n",cur0_scl.win_id,count);
			break;
		}
		/* get cur scl filter setting */
		cur0_scl.hor_y_tap_type = _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_y_tap_type;
		/* get cur scl filter mode */
		if(cur0_scl.hor_y_tap_type<LX_PE_FILTERTYPE_NUM)
		{
	/* get scaler input horizontal size */
    DE_MSL_L9_RdFL(msc_scr_size_l);
    DE_MSL_L9_Rd01(msc_scr_size_l, input_h_size,	scl_h_size);
			/* check abnormal case : if scl_h_size is over 1024, need to set 6tap */
			if((scl_h_size==0) || (scl_h_size>=1024 && cur0_scl.hor_y_tap_type==LX_PE_12TAP))
	{
				cur0_scl.hor_y_tap_type		= cur0_scl.hor_c_tap_type	= LX_PE_06TAP_NORMAL;
				cur0_scl.ver_y_tap_type		= cur0_scl.ver_c_tap_type	= LX_PE_06TAP_NORMAL;
				cur0_scl.fir_luma_type		= cur0_scl.fir_chroma_type	= LX_PE_BILINEAR;
				cur0_scl.hor_luma_id		= cur0_scl.ver_luma_id		= 15;
				cur0_scl.hor_chroma_id		= cur0_scl.ver_chroma_id	= 39;
	}
			else	/* normal case */
	{
				cur0_scl.hor_c_tap_type		= _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_c_tap_type;
				cur0_scl.ver_y_tap_type		= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_y_tap_type;
				cur0_scl.ver_c_tap_type		= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_c_tap_type;
				cur0_scl.fir_luma_type		= _g_cur_scl_filter[LX_PE_WIN_MAIN].fir_luma_type;
				cur0_scl.fir_chroma_type	= _g_cur_scl_filter[LX_PE_WIN_MAIN].fir_chroma_type;
				cur0_scl.hor_luma_id		= _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_luma_id;
				cur0_scl.ver_luma_id		= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_luma_id;
				cur0_scl.hor_chroma_id		= _g_cur_scl_filter[LX_PE_WIN_MAIN].hor_chroma_id;
				cur0_scl.ver_chroma_id		= _g_cur_scl_filter[LX_PE_WIN_MAIN].ver_chroma_id;
	}
	}
		else	/* init case */
	{
			cur0_scl.hor_y_tap_type		= cur0_scl.hor_c_tap_type	= LX_PE_06TAP_NORMAL;
			cur0_scl.ver_y_tap_type		= cur0_scl.ver_c_tap_type	= LX_PE_06TAP_NORMAL;
			cur0_scl.fir_luma_type		= cur0_scl.fir_chroma_type	= LX_PE_BILINEAR;
			cur0_scl.hor_luma_id		= cur0_scl.ver_luma_id		= 15;
			cur0_scl.hor_chroma_id		= cur0_scl.ver_chroma_id	= 39;
	}
		/* compare and settings */
		if((pre0_scl->hor_y_tap_type != cur0_scl.hor_y_tap_type) || \
			(pre0_scl->hor_c_tap_type != cur0_scl.hor_c_tap_type) || \
			(pre0_scl->ver_y_tap_type != cur0_scl.ver_y_tap_type) || \
			(pre0_scl->ver_c_tap_type != cur0_scl.ver_c_tap_type) || \
			(pre0_scl->fir_luma_type != cur0_scl.fir_luma_type) || \
			(pre0_scl->fir_chroma_type != cur0_scl.fir_chroma_type) || \
			(pre0_scl->hor_luma_id != cur0_scl.hor_luma_id) || \
			(pre0_scl->ver_luma_id != cur0_scl.ver_luma_id) || \
			(pre0_scl->hor_chroma_id != cur0_scl.hor_chroma_id) || \
			(pre0_scl->ver_chroma_id != cur0_scl.ver_chroma_id))
	{
			/* set config */
			if((pre0_scl->hor_y_tap_type != cur0_scl.hor_y_tap_type) || \
				(pre0_scl->fir_luma_type != cur0_scl.fir_luma_type) || \
				(pre0_scl->fir_chroma_type != cur0_scl.fir_chroma_type))
	{
				scaler_config	= (cur0_scl.hor_y_tap_type==LX_PE_12TAP)? 0x0:0x1;
				adap_filter_en	= (cur0_scl.hor_y_tap_type==LX_PE_12TAP)? 0x1:0x0;
				type_sel_luma	= (cur0_scl.fir_luma_type==LX_PE_BILINEAR)? 0x1:0x0;
				type_sel_chroma	= (cur0_scl.fir_chroma_type==LX_PE_BILINEAR)? 0x1:0x0;
				PE_SHP_DBG_PRINT("set[0]:scl_config:%d, adp_flt:%d, type_sel_y,c:%d,%d\n",\
					scaler_config,adap_filter_en,type_sel_luma,type_sel_chroma);
		DE_MSL_L9_RdFL(msc_ctrl_l);
				DE_MSL_L9_Wr01(msc_ctrl_l, filter_type_sel_luma       ,type_sel_luma);
				DE_MSL_L9_Wr01(msc_ctrl_l, filter_type_sel_chroma     ,type_sel_chroma);
				DE_MSL_L9_Wr01(msc_ctrl_l, v_scaler_config            ,scaler_config);
				DE_MSL_L9_Wr01(msc_ctrl_l, adaptive_fir_filter_en     ,adap_filter_en);
		DE_MSL_L9_WrFL(msc_ctrl_l);
			}
			flag = PE_SHP_GetDoubleSettingsFlag(LX_PE_WIN_MAIN,&cur0_scl);
			ret = PE_SHP_SetScalerFilterCoeffOnIsr(0x0,flag,&cur0_scl);
			PE_SHP_CHECK_CODE(ret,break,"[%s,%d] PE_SHP_SetScalerFilterCoeffOnIsr() error.\n",__F__,__L__);
			PE_SHP_DBG_PRINT("set[0]:inf : scl_h_size:%d\n"\
				"set[0]:val[wid:%d] : tap:%d,%d,%d,%d,fir:%d,%d,idx:%2d,%2d,%2d,%2d\n"\
				"set[0]:cur[wid:%d] : tap:%d,%d,%d,%d,fir:%d,%d,idx:%2d,%2d,%2d,%2d\n"\
				"set[0]:val[wid:%d] : tap:%d,%d,%d,%d,fir:%d,%d,idx:%2d,%2d,%2d,%2d\n"\
				"set[0]:flag y(h,v):%d,%d, c(h,v):%d,%d\n",\
				scl_h_size,_g_cur_scl_filter[LX_PE_WIN_MAIN].win_id,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_y_tap_type,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_c_tap_type,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_y_tap_type,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_c_tap_type,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].fir_luma_type,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].fir_chroma_type,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_luma_id,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_luma_id,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_chroma_id,\
				_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_chroma_id,\
				pre0_scl->win_id,pre0_scl->hor_y_tap_type,pre0_scl->hor_c_tap_type,\
				pre0_scl->ver_y_tap_type,pre0_scl->ver_c_tap_type,pre0_scl->fir_luma_type,\
				pre0_scl->fir_chroma_type,pre0_scl->hor_luma_id,pre0_scl->ver_luma_id,\
				pre0_scl->hor_chroma_id,pre0_scl->ver_chroma_id,cur0_scl.win_id,\
				cur0_scl.hor_y_tap_type,cur0_scl.hor_c_tap_type,cur0_scl.ver_y_tap_type,\
				cur0_scl.ver_c_tap_type,cur0_scl.fir_luma_type,cur0_scl.fir_chroma_type,\
				cur0_scl.hor_luma_id,cur0_scl.ver_luma_id,cur0_scl.hor_chroma_id,\
				cur0_scl.ver_chroma_id,flag.h_y,flag.v_y,flag.h_c,flag.v_c);
			pre0_scl->win_id = cur0_scl.win_id;
			pre0_scl->hor_y_tap_type = cur0_scl.hor_y_tap_type;
			pre0_scl->hor_c_tap_type = cur0_scl.hor_c_tap_type;
			pre0_scl->ver_y_tap_type = cur0_scl.ver_y_tap_type;
			pre0_scl->ver_c_tap_type = cur0_scl.ver_c_tap_type;
			pre0_scl->fir_luma_type = cur0_scl.fir_luma_type;
			pre0_scl->fir_chroma_type = cur0_scl.fir_chroma_type;
			pre0_scl->hor_luma_id = cur0_scl.hor_luma_id;
			pre0_scl->ver_luma_id = cur0_scl.ver_luma_id;
			pre0_scl->hor_chroma_id = cur0_scl.hor_chroma_id;
			pre0_scl->ver_chroma_id = cur0_scl.ver_chroma_id;
	}
	/* trace scaler mode */
	if(_g_shp_trace)
	{
		if(count>720)
		{
				PE_SHP_DBG_PRINT("state:inf : scl_h_size:%d\n"\
					"state:val[wid:%d] : tap:%d,%d,%d,%d,fir:%d,%d,idx:%2d,%2d,%2d,%2d\n"\
					"state:cur[wid:%d] : tap:%d,%d,%d,%d,fir:%d,%d,idx:%2d,%2d,%2d,%2d\n",\
					scl_h_size,_g_cur_scl_filter[LX_PE_WIN_MAIN].win_id,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_y_tap_type,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_c_tap_type,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_y_tap_type,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_c_tap_type,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].fir_luma_type,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].fir_chroma_type,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_luma_id,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_luma_id,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].hor_chroma_id,\
					_g_cur_scl_filter[LX_PE_WIN_MAIN].ver_chroma_id,\
					cur0_scl.win_id,cur0_scl.hor_y_tap_type,cur0_scl.hor_c_tap_type,\
					cur0_scl.ver_y_tap_type,cur0_scl.ver_c_tap_type,cur0_scl.fir_luma_type,\
					cur0_scl.fir_chroma_type,cur0_scl.hor_luma_id,cur0_scl.ver_luma_id,\
					cur0_scl.hor_chroma_id,cur0_scl.ver_chroma_id);
			count=1;
		}
		count++;
	}
	}while(0);
	return ret;
}
static void PE_SHP_WrMscCoeff(const PE_REG_PARAM_T *reg, UINT32 base_addr, UINT32 table_size)
{
	UINT32 i;

#define PE_MSL_L9_Wr(_a,_d)	\
	{	DE_MSL_L9_RdFL(_a);DE_MSL_L9_Wr(_a,_d);DE_MSL_L9_WrFL(_a);	}

	for (i=0;i<table_size;i++)
	{
		switch(reg[i].addr+base_addr)
		{
			case 0x1130:
				PE_MSL_L9_Wr(msc_y_fir_coef_data0_l,	reg[i].data);
				break;
			case 0x1134:
				PE_MSL_L9_Wr(msc_y_fir_coef_data1_l,	reg[i].data);
				break;
			case 0x1138:
				PE_MSL_L9_Wr(msc_y_fir_coef_data2_l,	reg[i].data);
				break;
			case 0x113c:
				PE_MSL_L9_Wr(msc_y_fir_coef_data3_l,	reg[i].data);
				break;
			case 0x1140:
				PE_MSL_L9_Wr(msc_y_fir_coef_data4_l,	reg[i].data);
				break;
			case 0x1144:
				PE_MSL_L9_Wr(msc_y_fir_coef_data5_l,	reg[i].data);
				break;
			case 0x1148:
				PE_MSL_L9_Wr(msc_y_fir_coef_ctrl_l,		reg[i].data);
				break;
			case 0x1150:
				PE_MSL_L9_Wr(msc_c_fir_coef_data0_l,	reg[i].data);
				break;
			case 0x1154:
				PE_MSL_L9_Wr(msc_c_fir_coef_data1_l,	reg[i].data);
				break;
			case 0x1158:
				PE_MSL_L9_Wr(msc_c_fir_coef_data2_l,	reg[i].data);
				break;
			case 0x115c:
				PE_MSL_L9_Wr(msc_c_fir_coef_data3_l,	reg[i].data);
				break;
			case 0x1160:
				PE_MSL_L9_Wr(msc_c_fir_coef_ctrl_l,		reg[i].data);
				break;
			default:
				PE_SHP_ERROR("[%s,%d] wrong case.[%03d]addr:0x%08x,data:0x%08x\n",\
					__F__,__L__,i,reg[i].addr,reg[i].data);
				break;
		}
	}
	return;
}
static int PE_SHP_SetScalerFilterCoeffOnIsr(UINT32 base_addr, PE_SHP_SETFLAG_T flag, LX_PE_SHP_SCLFILTER_T *pstParams)
{
	int ret = RET_OK;
	UINT32 table_size;
	UINT8 h_luma_index		= pstParams->hor_luma_id;
	UINT8 v_luma_index		= pstParams->ver_luma_id;
	UINT8 h_chroma_index	= pstParams->hor_chroma_id;
	UINT8 v_chroma_index	= pstParams->ver_chroma_id;
	LX_PE_SCL_FILTERTYPE hor_y_tap_type = pstParams->hor_y_tap_type;
	LX_PE_SCL_FILTERTYPE hor_c_tap_type = pstParams->hor_c_tap_type;
	LX_PE_SCL_FILTERTYPE ver_y_tap_type = pstParams->ver_y_tap_type;
	LX_PE_SCL_FILTERTYPE ver_c_tap_type = pstParams->ver_c_tap_type;

	if(flag.h_y)
	{
		switch(hor_y_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_12tap_coeff_l9b0\n");
					table_size = PE_SHP_SCL_LUMA_H_12_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_h_12tap_coeff_l9b0[h_luma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_12tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_LUMA_H_12_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_h_12tap_coeff_l9a0[h_luma_index][0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_dn_75_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_LUMA_H_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_h_dn_75_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_dn_50_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_LUMA_H_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_h_dn_50_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_dn_25_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_LUMA_H_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_h_dn_25_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_NORMAL:
			case LX_PE_06TAP_HALF:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_06tap_coeff_l9b0\n");
					table_size = PE_SHP_SCL_LUMA_H_06_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_h_06tap_coeff_l9b0[h_luma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_h_06tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_LUMA_H_06_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_h_06tap_coeff_l9a0[h_luma_index][0]),base_addr,table_size);
				}
				break;
		}
	}

	if(flag.h_c)
	{
		switch(hor_c_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_coeff_l9b0\n");
					table_size = PE_SHP_SCL_CHROMA_H_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_h_coeff_l9b0[h_chroma_index][0]),base_addr,table_size);
					flag.v_c = 0;	// avoid double setting
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_def_12tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_CHROMA_12_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_def_12tap_coeff_l9a0[0]),base_addr,table_size);
					flag.v_c = 0;	// avoid double setting
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_dn_75_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_CHROMA_H_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_h_dn_75_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_dn_50_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_CHROMA_H_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_h_dn_50_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_dn_25_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_CHROMA_H_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_h_dn_25_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_NORMAL:
			case LX_PE_06TAP_HALF:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_coeff_l9b0\n");
					table_size = PE_SHP_SCL_CHROMA_H_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_h_coeff_l9b0[h_chroma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_06tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_CHROMA_H_06_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_h_06tap_coeff_l9a0[0]),base_addr,table_size);
				}
				break;
		}
	}

	if(flag.v_y)
	{
		switch(ver_y_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_12tap_coeff_l9b0\n");
					table_size = PE_SHP_SCL_LUMA_V_12_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_12tap_coeff_l9b0[v_luma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_12tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_LUMA_V_12_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_12tap_coeff_l9a0[v_luma_index][0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_HALF:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_half_06tap_coeff_l9b0\n");
					table_size = PE_SHP_SCL_LUMA_V_06_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_half_06tap_coeff_l9b0[v_luma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_half_06tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_LUMA_V_06_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_half_06tap_coeff_l9a0[v_luma_index][0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_dn_75_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_LUMA_V_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_dn_75_06tap_coeff_l9[0]),base_addr,table_size);
				}
			break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_dn_50_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_LUMA_V_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_dn_50_06tap_coeff_l9[0]),base_addr,table_size);
				}
			break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_dn_25_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_LUMA_V_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_dn_25_06tap_coeff_l9[0]),base_addr,table_size);
				}
			break;
			case LX_PE_06TAP_NORMAL:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_06tap_coeff_l9b0\n");
					table_size = PE_SHP_SCL_LUMA_V_06_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_06tap_coeff_l9b0[v_luma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_luma_v_06tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_LUMA_V_06_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_luma_v_06tap_coeff_l9a0[v_luma_index][0]),base_addr,table_size);
				}
				break;
		}
	}

	if(flag.v_c)
	{
		switch(ver_c_tap_type)
		{
			case LX_PE_12TAP:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_h_coeff_l9b0\n");
					/* v12t mode : v_c_flt share h_c_flt coeff. */
					table_size = PE_SHP_SCL_CHROMA_H_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_h_coeff_l9b0[v_chroma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_def_12tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_CHROMA_12_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_def_12tap_coeff_l9a0[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_75:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_dn_75_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_CHROMA_V_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_v_dn_75_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_50:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_dn_50_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_CHROMA_V_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_v_dn_50_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_DOWN_25:
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_dn_25_06tap_coeff_l9\n");
					table_size = PE_SHP_SCL_CHROMA_V_06_L9_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_v_dn_25_06tap_coeff_l9[0]),base_addr,table_size);
				}
				break;
			case LX_PE_06TAP_NORMAL:
			case LX_PE_06TAP_HALF:
			default:
				if(PE_CHIP_VER_GT(L9,B0))
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_coeff_l9b0\n");
					table_size = PE_SHP_SCL_CHROMA_V_L9B0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_v_coeff_l9b0[v_chroma_index][0]),base_addr,table_size);
				}
				else
				{
					PE_SHP_DBG_PRINT("set shp_scl_chroma_v_06tap_coeff_l9a0\n");
					table_size = PE_SHP_SCL_CHROMA_V_06_L9A0_NUM;
					PE_SHP_WrMscCoeff(&(shp_scl_chroma_v_06tap_coeff_l9a0[0]),base_addr,table_size);
				}
				break;
		}
	}

	return ret;
}
#endif


