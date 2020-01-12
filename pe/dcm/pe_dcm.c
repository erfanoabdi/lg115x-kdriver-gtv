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


/** @file pe_dcm.c
 *
 *  driver for picture enhance dynamic contrast module functions. ( used only within kdriver )
 *	- dynamic contrast enhancement, histogram information and configuration.
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

#include "pe_reg.h"
#include "pe_cfg.h"
#include "pe_def.h"
#include "pe_d3d.h"
#include "pe_win.h"
#include "pe_dcm.h"
#include "pe_dcm_coeff.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_DCM_ERROR	printk

#define PE_DCM_DBG_PRINT(fmt,args...)	if(_g_dcm_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_DCM_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_DCM_ERROR(fmt,##args);_action;}}

#define memset0(_var)	memset( &(_var), 0x00, sizeof(_var))
/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

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
extern PE_D3D_INFO_T d3d_info;

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
static unsigned int _g_dce_lut_step[LX_PE_WIN_NUM];
static UINT32 _g_dse_lut_data[LX_PE_WIN_NUM][32];
PE_DCM_DCE_HISTO_T _g_dce_histo_info[LX_PE_WIN_NUM];

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int PE_DCM_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_DCM_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_DCM_GetHistogramInfo(LX_PE_DCM_HISTO_INFO_T *pstParams);
static int PE_DCM_SetDceConfig(LX_PE_DCM_DCE_CONF_T *pstParams);
static int PE_DCM_GetDceConfig(LX_PE_DCM_DCE_CONF_T *pstParams);
static int PE_DCM_SetDceLut(LX_PE_DCM_DCE_LUT_T *pstParams);
static int PE_DCM_GetDceLut(LX_PE_DCM_DCE_LUT_T *pstParams);
static int PE_DCM_SetDseLut(LX_PE_DCM_DSE_LUT_T *pstParams);
static int PE_DCM_GetDseLut(LX_PE_DCM_DSE_LUT_T *pstParams);
static int PE_DCM_SetDceBlendingCtrl(LX_PE_DCM_BLENDING_T *pstParams);
static int PE_DCM_GetDceBlendingCtrl(LX_PE_DCM_BLENDING_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_dcm_trace=0x0;	//default should be off.

/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * init dynamic contrast module
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_DCM_Init(void)
{
	int ret = RET_OK;
	UINT8 i;
	LX_PE_DCM_DCE_CONF_T stParams;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_DCM_DBG_PRINT("init histo info\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_dce_histo_info[i].win_id			= i;
				_g_dce_histo_info[i].status[0]		= 0;
				_g_dce_histo_info[i].status[1]		= 0;
				_g_dce_histo_info[i].status[2]		= 0;
				_g_dce_histo_info[i].status[3]		= 0;
				_g_dce_histo_info[i].sat_status		= 0;
			}
			PE_DCM_DBG_PRINT("init dce config\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_dce_lut_step[i] = 32;
			}
			stParams.domain = LX_PE_HSV_DOMAIN;
			stParams.bin_mode = LX_PE_32_BINMODE;
			stParams.lut_step = 16;
			stParams.min_pos = 7;
			stParams.max_pos = 210;
			stParams.win_id = LX_PE_WIN_ALL;
			ret = PE_DCM_SetDceConfig(&stParams);
			PE_DCM_CHECK_CODE(ret,break,"[%s,%d][win all] PE_DCM_SetDceConfig() error.\n",__F__,__L__);
		}
		else
		{
			PE_DCM_DBG_PRINT("init dce config\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_dce_lut_step[i] = 32;
			}
			stParams.domain = LX_PE_HSV_DOMAIN;
			stParams.bin_mode = LX_PE_32_BINMODE;
			stParams.lut_step = 16;
			stParams.min_pos = 7;
			stParams.max_pos = 210;
			stParams.win_id = LX_PE_WIN_ALL;
			ret = PE_DCM_SetDceConfig(&stParams);
			PE_DCM_CHECK_CODE(ret,break,"[%s,%d][win all] PE_DCM_SetDceConfig() error.\n",__F__,__L__);
		}
	}while(0);

	return ret;
}

/**
 * standard function of dcm setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_DCM_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_DCM_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_DCM_DCE_CONF_T):
		{
			LX_PE_DCM_DCE_CONF_T *pRcvData;
			LX_PE_DCM_DCE_CONF_T stParam;
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_DCE_CONF_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_DCE_CONF_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_DCE_CONF_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_DCM_SetDceConfig(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_SetDceConfig() error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_DCE_CONF_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DCM_DCE_LUT_T):
		{
			LX_PE_DCM_DCE_LUT_T *pRcvData;
			LX_PE_DCM_DCE_LUT_T stParam;
			UINT32 *pLut = NULL;
			UINT32 bufsize;
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_DCE_LUT_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_DCE_LUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_DCE_LUT_T *)data;
				bufsize = GET_BITS(pRcvData->step,0,8) * sizeof(UINT32);
				PE_DCM_CHECK_CODE(!bufsize,break,"[%s,%d] bufsize zero.\n",__F__,__L__);

				pLut = OS_Malloc(bufsize);
				PE_DCM_CHECK_CODE(!pLut,break,"[%s,%d] pLut is null.\n",__F__,__L__);

				ret = copy_from_user(pLut, (void __user *)pRcvData->data, bufsize);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] copy_from_user error.\n",__F__,__L__);

				stParam.win_id = pRcvData->win_id;
				stParam.step = pRcvData->step;
				stParam.data = pLut;
				ret = PE_DCM_SetDceLut(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_SetDceLut() error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_DCE_LUT_T\n");
			}while(0);
			if(pLut)	OS_Free(pLut);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DCM_DSE_LUT_T):
		{
			LX_PE_DCM_DSE_LUT_T *pRcvData;
			LX_PE_DCM_DSE_LUT_T stParam;
			UINT32 *pLut = NULL;
			UINT32 bufsize;
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_DSE_LUT_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_DSE_LUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_DSE_LUT_T *)data;
				bufsize = GET_BITS(pRcvData->step,0,8) * sizeof(UINT32);
				PE_DCM_CHECK_CODE(!bufsize,break,"[%s,%d] bufsize zero.\n",__F__,__L__);

				pLut = OS_Malloc(bufsize);
				PE_DCM_CHECK_CODE(!pLut,break,"[%s,%d] pLut is null.\n",__F__,__L__);

				ret = copy_from_user(pLut, (void __user *)pRcvData->data, bufsize);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] copy_from_user error.\n",__F__,__L__);

				stParam.win_id = pRcvData->win_id;
				stParam.step = pRcvData->step;
				stParam.data = pLut;
				ret = PE_DCM_SetDseLut(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_SetDseLut() error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_DSE_LUT_T\n");
			}while(0);
			if(pLut)	OS_Free(pLut);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DCM_BLENDING_T):
		{
			LX_PE_DCM_BLENDING_T *pRcvData;
			LX_PE_DCM_BLENDING_T stParam;
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_BLENDING_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_BLENDING_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_BLENDING_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_DCM_SetDceBlendingCtrl(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_SetDceBlendingCtrl() error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_BLENDING_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_DCM_SetDefaultSettings(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_SetDefaultSettings() error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_DCM_SetDebugSettings(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_SetDebugSettings() error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_DCM_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of dcm getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_DCM_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_DCM_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_DCM_HISTO_INFO_T):
		{
			LX_PE_DCM_HISTO_INFO_T *pRcvData;
			LX_PE_DCM_HISTO_INFO_T stParam;
			memset0(stParam);
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_HISTO_INFO_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_HISTO_INFO_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_HISTO_INFO_T *)data;
				stParam.win_id = pRcvData->win_id;
				stParam.domain = pRcvData->domain;
				stParam.bin_num = pRcvData->bin_num;
				ret = PE_DCM_GetHistogramInfo(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_GetHistogramInfo() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_HISTO_INFO_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DCM_DCE_CONF_T):
		{
			LX_PE_DCM_DCE_CONF_T *pRcvData;
			LX_PE_DCM_DCE_CONF_T stParam;
			memset0(stParam);
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_DCE_CONF_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_DCE_CONF_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_DCE_CONF_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_DCM_GetDceConfig(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_GetDceConfig() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_DCE_CONF_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DCM_DCE_LUT_T):
		{
			LX_PE_DCM_DCE_LUT_T *pRcvData;
			LX_PE_DCM_DCE_LUT_T stParam;
			UINT32 *pLut = NULL;
			UINT32 bufsize;
			memset0(stParam);
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_DCE_LUT_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_DCE_LUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_DCE_LUT_T *)data;
				bufsize = GET_BITS(pRcvData->step,0,8) * sizeof(UINT32);
				PE_DCM_CHECK_CODE(!bufsize,break,"[%s,%d] bufsize zero.\n",__F__,__L__);

				pLut = OS_Malloc(bufsize);
				PE_DCM_CHECK_CODE(!pLut,break,"[%s,%d] pLut is null.\n",__F__,__L__);

				stParam.win_id = pRcvData->win_id;
				stParam.step = pRcvData->step;
				stParam.data = pLut;
				ret = PE_DCM_GetDceLut(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_GetDceLut() error.\n",__F__,__L__);

				ret = copy_to_user((void __user *)pRcvData->data, pLut, bufsize);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] copy_to_user error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_DCE_LUT_T\n");
			}while(0);
			if(pLut)	OS_Free(pLut);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DCM_DSE_LUT_T):
		{
			LX_PE_DCM_DSE_LUT_T *pRcvData;
			LX_PE_DCM_DSE_LUT_T stParam;
			UINT32 *pLut = NULL;
			UINT32 bufsize;
			memset0(stParam);
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_DSE_LUT_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_DSE_LUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_DSE_LUT_T *)data;
				bufsize = GET_BITS(pRcvData->step,0,8) * sizeof(UINT32);
				PE_DCM_CHECK_CODE(!bufsize,break,"[%s,%d] bufsize zero.\n",__F__,__L__);

				pLut = OS_Malloc(bufsize);
				PE_DCM_CHECK_CODE(!pLut,break,"[%s,%d] pLut is null.\n",__F__,__L__);

				stParam.win_id = pRcvData->win_id;
				stParam.step = pRcvData->step;
				stParam.data = pLut;
				ret = PE_DCM_GetDseLut(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_GetDseLut() error.\n",__F__,__L__);

				ret = copy_to_user((void __user *)pRcvData->data, pLut, bufsize);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] copy_to_user error.\n",__F__,__L__);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_DSE_LUT_T\n");
			}while(0);
			if(pLut)	OS_Free(pLut);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DCM_BLENDING_T):
		{
			LX_PE_DCM_BLENDING_T *pRcvData;
			LX_PE_DCM_BLENDING_T stParam;
			memset0(stParam);
			do{
				PE_DCM_DBG_PRINT("S LX_PE_DCM_BLENDING_T\n");
				/* data size check */
				PE_DCM_CHECK_CODE(datasize!=sizeof(LX_PE_DCM_BLENDING_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DCM_BLENDING_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_DCM_GetDceBlendingCtrl(&stParam);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_DCM_GetDceBlendingCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_DCM_DBG_PRINT("E LX_PE_DCM_BLENDING_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_DCM_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_DCM_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_DCM_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);
	}while(0);
	return ret;
}
static int PE_DCM_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_DCM_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(DCM)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_dcm_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(DCM)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(DCM)))
				{
					PE_DCM_DBG_PRINT("dynamic_contrast_en : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	dynamic_contrast_en,	0x0);
						DE_P1L_L9B0_WrFL(pe1_l_dce_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	dynamic_contrast_en,	0x0);
						DE_P1R_L9B0_WrFL(pe1_r_dce_ctrl_00);
					}
				}
				else
				{
					PE_DCM_DBG_PRINT("dynamic_contrast_en : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	dynamic_contrast_en,	0x1);
						DE_P1L_L9B0_WrFL(pe1_l_dce_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	dynamic_contrast_en,	0x1);
						DE_P1R_L9B0_WrFL(pe1_r_dce_ctrl_00);
					}
				}
			}
			else
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(DCM)))
				{
					PE_DCM_DBG_PRINT("dynamic_contrast_en : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9_RdFL(pe1_l_dce_ctrl_00);
						DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	dynamic_contrast_en,	0x0);
						DE_P1L_L9_WrFL(pe1_l_dce_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9_RdFL(pe1_r_dce_ctrl_00);
						DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	dynamic_contrast_en,	0x0);
						DE_P1R_L9_WrFL(pe1_r_dce_ctrl_00);
					}
				}
				else
				{
					PE_DCM_DBG_PRINT("dynamic_contrast_en : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9_RdFL(pe1_l_dce_ctrl_00);
						DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	dynamic_contrast_en,	0x1);
						DE_P1L_L9_WrFL(pe1_l_dce_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9_RdFL(pe1_r_dce_ctrl_00);
						DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	dynamic_contrast_en,	0x1);
						DE_P1R_L9_WrFL(pe1_r_dce_ctrl_00);
					}
				}
			}
		}
	}while(0);
	return ret;
}
int PE_DCM_GetDceAllHistogram(PE_DCM_DCE_HISTO_T *pstMainParams,PE_DCM_DCE_HISTO_T *pstSubParams,UINT32 *validL,UINT32 *validR)
{
	int ret = RET_OK;
	UINT32 count=0, r_data=0;
	UINT32 histoUpdateMaskL=0,histoUpdateMaskR=0;

	do {
		CHECK_KNULL(pstMainParams);
		CHECK_KNULL(pstSubParams);
		if(PE_CHIP_VER_GT(L9,B0))
		{
			*validL = *validR = 0;
			DE_P1L_L9B0_RdFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1R_L9B0_RdFL(pe1_r_dce_hist_ia_ctrl);
			DE_P1L_L9B0_Rd01(pe1_l_dce_hist_ia_ctrl,	hif_mask,	histoUpdateMaskL);
			DE_P1R_L9B0_Rd01(pe1_r_dce_hist_ia_ctrl,	hif_mask,	histoUpdateMaskR);
			if(histoUpdateMaskL || histoUpdateMaskR)
			{
				for(count=0;count<pstMainParams->bin_num;count++)
				{
					pstMainParams->histogram[count]	= _g_dce_histo_info[LX_PE_WIN_MAIN].histogram[count];
					pstSubParams->histogram[count]	= _g_dce_histo_info[LX_PE_WIN_SUB].histogram[count];
				}
				*validL = histoUpdateMaskL;
				*validR = histoUpdateMaskR;
				ret = RET_OK;
				break;
			}
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_enable,	0x0);	//0:read mode, 1:normal mode
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_ai,		0x1);	//ai 0:disable, 1:enable
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_address,	0x0);	//bin address
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_enable,	0x0);	//0:read mode, 1:normal mode
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_ai,		0x1);	//ai 0:disable, 1:enable
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_address,	0x0);	//bin address
			DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);
			for(count=0;count<pstMainParams->bin_num;count++)
			{
				DE_P1L_L9B0_RdFL(pe1_l_dce_hist_ia_data1);
				pstMainParams->histogram[count] = DE_P1L_L9B0_Rd(pe1_l_dce_hist_ia_data1);
				_g_dce_histo_info[LX_PE_WIN_MAIN].histogram[count] = pstMainParams->histogram[count];
				DE_P1R_L9B0_RdFL(pe1_r_dce_hist_ia_data1);
				pstSubParams->histogram[count] = DE_P1R_L9B0_Rd(pe1_r_dce_hist_ia_data1);
				_g_dce_histo_info[LX_PE_WIN_SUB].histogram[count] = pstSubParams->histogram[count];
			}
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_enable,	0x1);	//0:read mode, 1:normal mode
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_enable,	0x1);	//0:read mode, 1:normal mode
			DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);

			DE_P1L_L9B0_RdFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1R_L9B0_RdFL(pe1_r_dce_hist_ia_ctrl);
			/* Rd status 0 */
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_status_address,	0x0);
			DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1L_L9B0_RdFL(pe1_l_dce_hsit_ia_data2);
			r_data = DE_P1L_L9B0_Rd(pe1_l_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_MAIN].status[0] = pstMainParams->status[0] = r_data;
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_status_address,	0x0);
			DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);
			DE_P1R_L9B0_RdFL(pe1_r_dce_hsit_ia_data2);
			r_data = DE_P1R_L9B0_Rd(pe1_r_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_SUB].status[0] = pstSubParams->status[0] = r_data;
			/* Rd status 1 */
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_status_address,	0x1);
			DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1L_L9B0_RdFL(pe1_l_dce_hsit_ia_data2);
			r_data = DE_P1L_L9B0_Rd(pe1_l_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_MAIN].status[1] = pstMainParams->status[1] = r_data;
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_status_address,	0x1);
			DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);
			DE_P1R_L9B0_RdFL(pe1_r_dce_hsit_ia_data2);
			r_data = DE_P1R_L9B0_Rd(pe1_r_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_SUB].status[1] = pstSubParams->status[1] = r_data;
			/* Rd status 2 */
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_status_address,	0x2);
			DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1L_L9B0_RdFL(pe1_l_dce_hsit_ia_data2);
			r_data = DE_P1L_L9B0_Rd(pe1_l_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_MAIN].status[2] = pstMainParams->status[2] = r_data;
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_status_address,	0x2);
			DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);
			DE_P1R_L9B0_RdFL(pe1_r_dce_hsit_ia_data2);
			r_data = DE_P1R_L9B0_Rd(pe1_r_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_SUB].status[2] = pstSubParams->status[2] = r_data;
			/* Rd status 3 */
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_status_address,	0x3);
			DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1L_L9B0_RdFL(pe1_l_dce_hsit_ia_data2);
			r_data = DE_P1L_L9B0_Rd(pe1_l_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_MAIN].status[3] = pstMainParams->status[3] = r_data;
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_status_address,	0x3);
			DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);
			DE_P1R_L9B0_RdFL(pe1_r_dce_hsit_ia_data2);
			r_data = DE_P1R_L9B0_Rd(pe1_r_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_SUB].status[3] = pstSubParams->status[3] = r_data;
			/* Rd status 4 */
			DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_status_address,	0x4);
			DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
			DE_P1L_L9B0_RdFL(pe1_l_dce_hsit_ia_data2);
			r_data = DE_P1L_L9B0_Rd(pe1_l_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_MAIN].status[4] = pstMainParams->status[4] = r_data;
			DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_status_address,	0x4);
			DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);
			DE_P1R_L9B0_RdFL(pe1_r_dce_hsit_ia_data2);
			r_data = DE_P1R_L9B0_Rd(pe1_r_dce_hsit_ia_data2);
			_g_dce_histo_info[LX_PE_WIN_SUB].status[4] = pstSubParams->status[4] = r_data;
			/* Rd saturation status */
			PE_P0L_L9B0_Rd01(tpd_stat_00,	reg_s_status,	r_data);
			_g_dce_histo_info[LX_PE_WIN_MAIN].sat_status = pstMainParams->sat_status = r_data;
			_g_dce_histo_info[LX_PE_WIN_SUB].sat_status = pstSubParams->sat_status = r_data;
		}
		else
		{
			PE_DCM_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	} while(0);
	
	return ret;
}
static int PE_DCM_GetHistogramInfo(LX_PE_DCM_HISTO_INFO_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 count=0;
	UINT32 histo_data;
	UINT32 histoUpdateMask=0;
	PE_D3D_DTA_T *pData = &d3d_info.dta;
	PE_WIN_SETTINGS_T win_set;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				UINT32 avrg_l[3],avrg_r[3];
				win_set.win_id = LX_PE_WIN_MAIN;
				ret = PE_WIN_GetCurWinSettings(&win_set);
				PE_DCM_CHECK_CODE(ret,break,"[%s,%d] PE_WIN_GetCurWinSettings() error.\n",__F__,__L__);
				if(win_set.out_type==LX_PE_OUT_UD || \
					win_set.out_type==LX_PE_OUT_TB || \
					win_set.out_type==LX_PE_OUT_SS|| \
					win_set.out_type==LX_PE_OUT_FS || \
					win_set.out_type==LX_PE_OUT_DUAL)
				{
				if(pstParams->domain==LX_PE_YC_DOMAIN)
				{
					DE_P1L_L9B0_RdFL(pe1_l_apl_stat_00);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_00,	apl_y,		avrg_l[0]);
						DE_P1R_L9B0_RdFL(pe1_r_apl_stat_00);
						DE_P1R_L9B0_Rd01(pe1_r_apl_stat_00,	apl_y,		avrg_r[0]);
						avrg_l[2] = avrg_l[1] = avrg_l[0];
						avrg_r[2] = avrg_r[1] = avrg_r[0];
					}
					else
					{
						DE_P1L_L9B0_RdFL(pe1_l_apl_stat_01);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_01,	apl_r,		avrg_l[0]);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_01,	apl_g,		avrg_l[1]);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_01,	apl_b,		avrg_l[2]);
						DE_P1R_L9B0_RdFL(pe1_r_apl_stat_01);
						DE_P1R_L9B0_Rd01(pe1_r_apl_stat_01,	apl_r,		avrg_r[0]);
						DE_P1R_L9B0_Rd01(pe1_r_apl_stat_01,	apl_g,		avrg_r[1]);
						DE_P1R_L9B0_Rd01(pe1_r_apl_stat_01,	apl_b,		avrg_r[2]);
					}
					pstParams->average[0] = (avrg_l[0]+avrg_r[0])>>1;
					pstParams->average[1] = (avrg_l[1]+avrg_r[1])>>1;
					pstParams->average[2] = (avrg_l[2]+avrg_r[2])>>1;
				}
				else
				{
					if(pstParams->domain==LX_PE_YC_DOMAIN)
					{
						DE_P1L_L9B0_RdFL(pe1_l_apl_stat_00);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_00,	apl_y,		avrg_l[0]);
						avrg_l[2] = avrg_l[1] = avrg_l[0];
					}
					else
					{
						DE_P1L_L9B0_RdFL(pe1_l_apl_stat_01);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_01,	apl_r,		avrg_l[0]);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_01,	apl_g,		avrg_l[1]);
						DE_P1L_L9B0_Rd01(pe1_l_apl_stat_01,	apl_b,		avrg_l[2]);
					}
					pstParams->average[0] = avrg_l[0];
					pstParams->average[1] = avrg_l[1];
					pstParams->average[2] = avrg_l[2];
				}
				for(count=0;count<pstParams->bin_num;count++)
				{
					pstParams->histogram[count] = _g_dce_histo_info[LX_PE_WIN_MAIN].histogram[count];
				}
				pstParams->min = GET_BITS(_g_dce_histo_info[LX_PE_WIN_MAIN].status[0],0,10);
				pstParams->max = GET_BITS(_g_dce_histo_info[LX_PE_WIN_MAIN].status[0],16,10);
				pstParams->status[0] = _g_dce_histo_info[LX_PE_WIN_MAIN].status[0];
				pstParams->status[3] = _g_dce_histo_info[LX_PE_WIN_MAIN].status[3];
				pstParams->status[4] = _g_dce_histo_info[LX_PE_WIN_MAIN].status[4];
				pstParams->status[1] = _g_dce_histo_info[LX_PE_WIN_MAIN].status[1];
				pstParams->status[2] = _g_dce_histo_info[LX_PE_WIN_MAIN].status[2];
				pstParams->histo_ready = 1;
				pstParams->sat_status = _g_dce_histo_info[LX_PE_WIN_MAIN].sat_status;
				pstParams->motion = pData->motion_l;
			}
			if(PE_CHECK_SUB(win_id))
			{
				if(pstParams->domain==LX_PE_YC_DOMAIN)
				{
					DE_P1R_L9B0_RdFL(pe1_r_apl_stat_00);
					DE_P1R_L9B0_Rd01(pe1_r_apl_stat_00,	apl_y,		pstParams->average[0]);
				}
				else
				{
					DE_P1R_L9B0_RdFL(pe1_r_apl_stat_01);
					DE_P1R_L9B0_Rd01(pe1_r_apl_stat_01,	apl_r,		pstParams->average[0]);
					DE_P1R_L9B0_Rd01(pe1_r_apl_stat_01,	apl_g,		pstParams->average[1]);
					DE_P1R_L9B0_Rd01(pe1_r_apl_stat_01,	apl_b,		pstParams->average[2]);
				}
				pstParams->min = GET_BITS(_g_dce_histo_info[LX_PE_WIN_SUB].status[0],0,10);
				pstParams->max = GET_BITS(_g_dce_histo_info[LX_PE_WIN_SUB].status[0],16,10);
				pstParams->status[0] = _g_dce_histo_info[LX_PE_WIN_SUB].status[0];
				pstParams->status[1] = _g_dce_histo_info[LX_PE_WIN_SUB].status[1];
				pstParams->status[2] = _g_dce_histo_info[LX_PE_WIN_SUB].status[2];
				pstParams->status[3] = _g_dce_histo_info[LX_PE_WIN_SUB].status[3];
				pstParams->status[4] = _g_dce_histo_info[LX_PE_WIN_SUB].status[4];
				pstParams->histo_ready = 1;
				for(count=0;count<pstParams->bin_num;count++)
				{
					pstParams->histogram[count] = _g_dce_histo_info[LX_PE_WIN_SUB].histogram[count];
				}
				pstParams->sat_status = 0;
				pstParams->motion = pData->motion_r;
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				if(pstParams->domain==LX_PE_YC_DOMAIN)
				{
					DE_P1L_L9_RdFL(pe1_l_apl_ctrl_03);
					DE_P1L_L9_Rd01(pe1_l_apl_ctrl_03,	apl_y,		pstParams->average[0]);
				}
				else
				{
					DE_P1L_L9_RdFL(pe1_l_apl_ctrl_04);
					DE_P1L_L9_Rd01(pe1_l_apl_ctrl_04,	apl_r,		pstParams->average[0]);
					DE_P1L_L9_Rd01(pe1_l_apl_ctrl_04,	apl_g,		pstParams->average[1]);
					DE_P1L_L9_Rd01(pe1_l_apl_ctrl_04,	apl_b,		pstParams->average[2]);
				}
				DE_P1L_L9_RdFL(pe1_l_dce_hist_ia_ctrl);
				DE_P1L_L9_RdFL(pe1_l_dce_hsit_ia_data2);
				histo_data = DE_P1L_L9_Rd(pe1_l_dce_hsit_ia_data2);
				pstParams->min = GET_BITS(histo_data,0,10);
				pstParams->max = GET_BITS(histo_data,16,10);
				DE_P1L_L9_Rd01(pe1_l_dce_hist_ia_ctrl,	hif_mask,	histoUpdateMask);
				if(histoUpdateMask)	{	pstParams->histo_ready = 0;break;	}
				else				{	pstParams->histo_ready = 1;			}
				DE_P1L_L9_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_enable,	0x0);	//0:read mode, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_ai,		0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_address,	0x0);	//bin address
				DE_P1L_L9_WrFL(pe1_l_dce_hist_ia_ctrl);
				for(count=0;count<pstParams->bin_num;count++)
				{
					DE_P1L_L9_RdFL(pe1_l_dce_hist_ia_data1);
					pstParams->histogram[count] = DE_P1L_L9_Rd(pe1_l_dce_hist_ia_data1);
				}
				DE_P1L_L9_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_hist_enable,	0x1);	//0:read mode, 1:normal mode
				DE_P1L_L9_WrFL(pe1_l_dce_hist_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				if(pstParams->domain==LX_PE_YC_DOMAIN)
				{
					DE_P1R_L9_RdFL(pe1_r_apl_ctrl_03);
					DE_P1R_L9_Rd01(pe1_r_apl_ctrl_03,	apl_y,		pstParams->average[0]);
				}
				else
				{
					DE_P1R_L9_RdFL(pe1_r_apl_ctrl_04);
					DE_P1R_L9_Rd01(pe1_r_apl_ctrl_04,	apl_r,		pstParams->average[0]);
					DE_P1R_L9_Rd01(pe1_r_apl_ctrl_04,	apl_g,		pstParams->average[1]);
					DE_P1R_L9_Rd01(pe1_r_apl_ctrl_04,	apl_b,		pstParams->average[2]);
				}
				DE_P1R_L9_RdFL(pe1_r_dce_hist_ia_ctrl);
				DE_P1R_L9_RdFL(pe1_r_dce_hsit_ia_data2);				
				histo_data = DE_P1R_L9_Rd(pe1_r_dce_hsit_ia_data2);
				pstParams->min = GET_BITS(histo_data,0,10);
				pstParams->max = GET_BITS(histo_data,16,10);		
				DE_P1R_L9_Rd01(pe1_r_dce_hist_ia_ctrl,	hif_mask,	histoUpdateMask);
				if(histoUpdateMask)	{	pstParams->histo_ready = 0;break;	}
				else				{	pstParams->histo_ready = 1;			}
				DE_P1R_L9_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_enable,	0x0);	//0:read mode, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_ai,		0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_address,	0x0);	//bin address
				DE_P1R_L9_WrFL(pe1_r_dce_hist_ia_ctrl);
				for(count=0;count<pstParams->bin_num;count++)
				{
					DE_P1R_L9_RdFL(pe1_r_dce_hist_ia_data1);
					pstParams->histogram[count] = DE_P1R_L9_Rd(pe1_r_dce_hist_ia_data1);
				}
				DE_P1R_L9_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_hist_enable,	0x1);	//[15]	0:read mode, 1:normal mode
				DE_P1R_L9_WrFL(pe1_r_dce_hist_ia_ctrl);
			}
		}
	} while (0);

	return ret;
}
static int PE_DCM_SetDceConfig(LX_PE_DCM_DCE_CONF_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count, bin_mode, domain, min_pos, max_pos, lut_data, lut_step, step_unit;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);

		if(pstParams->domain==LX_PE_HSV_DOMAIN)				{domain=0;}	//scaled hsv
		else if(pstParams->domain==LX_PE_YC_DOMAIN)			{domain=1;}	//yc
		else if(pstParams->domain==LX_PE_ORG_HSV_DOMAIN)	{domain=2;}	//normal hsv
		else												{domain=3;}	//hsl
		if(pstParams->lut_step>32)		{lut_step=32;}
		else if(pstParams->lut_step==0)	{lut_step=1;}
		else							{lut_step=pstParams->lut_step;}
		min_pos = pstParams->min_pos;
		max_pos = pstParams->max_pos;
		if(lut_step==16)		{step_unit = 64;}
		else if(lut_step==32)	{step_unit = 32;}
		else					{step_unit = 1024/lut_step;}		// to make linear

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(pstParams->bin_mode==LX_PE_32_BINMODE)	{bin_mode=0x0;}
			else										{bin_mode=0x1;}
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				/* dce config */
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_01);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ia_ctrl);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ia_data);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_08);
				//DE_P1L_L9B0_RdFL(pe1_l_dce_hist_ia_ctrl);
				DE_P1L_L9B0_RdFL(pe1_l_apl_ctrl_02);
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	dynamic_contrast_en,	0x1);					//enable
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	hist_bin_mode,			bin_mode);				//32bin mode
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	window_mode_en,			0x1);					//window sync
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	window_acquire,			0x1);					//window acquire on
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	dce_domain_sel,			(domain==1)? 0x1:0x0);	//HSV or YC domain
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	win_selection,			0x0);					//from pe1 win2
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_01,	hist_bin_min_position,	min_pos);				//min position
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_01,	hist_bin_max_position,	max_pos);				//max position
				//DE_P1L_L9B0_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_status_address,	0x0);				//V value for range
				DE_P1L_L9B0_Wr01(pe1_l_apl_ctrl_02,	apl_position,			(domain==1)? 0x0:0x2);	//0x0:PE1 input(Y/C),0x2:cen input(RGB)
				DE_P1L_L9B0_WrFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9B0_WrFL(pe1_l_dce_ctrl_01);
				//DE_P1L_L9B0_WrFL(pe1_l_dce_hist_ia_ctrl);
				DE_P1L_L9B0_WrFL(pe1_l_apl_ctrl_02);
				if(domain!=1)	/* should set CEN block ON to use HSV DCE */
				{
					DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_00);
					DE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_00,	reg_cen_bypass,	0x1);					//CEN block ON
					DE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_00,	v_scaler_en,	(domain==0)? 0x1:0x0);	//v scaler en
					DE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_00,	demo_mode,		0x0);
					DE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_00,	vsp_sel,		(domain==3)? 0x0:0x1);	//0:hsl,1:hsv
					DE_P1L_L9B0_WrFL(pe1_l_cen_ctrl_00);
				}
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1L_L9B0_WrFL(pe1_l_dce_ia_ctrl);
				_g_dce_lut_step[LX_PE_WIN_MAIN] = lut_step;
				lut_data = 0;
				for(count=0;count<32;count++)
				{
					if(lut_data>0x3ff)	lut_data=0x3ff;
					DE_P1L_L9B0_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1L_L9B0_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,0,10));	//x data
					DE_P1L_L9B0_WrFL(pe1_l_dce_ia_data);
					lut_data += step_unit;
				}
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_08,	hif_dyc_wdata_y_32nd,	0x3ff);	//y data
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_08,	hif_dyc_wdata_x_32nd,	0x3ff);	//x data
				DE_P1L_L9B0_WrFL(pe1_l_dce_ctrl_08);
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_load,	 	0x1);	//0:clear, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable,		0x1);		//0:host access, 1:normal mode
				DE_P1L_L9B0_WrFL(pe1_l_dce_ia_ctrl);

				/* dse config */
				DE_P1L_L9B0_RdFL(pe1_l_dse_ctrl_00);
				DE_P1L_L9B0_Wr01(pe1_l_dse_ctrl_00,	dynamic_saturation_en,	0x1);
				DE_P1L_L9B0_Wr01(pe1_l_dse_ctrl_00,	window_mode_en,			0x1);	//window sync
				DE_P1L_L9B0_Wr01(pe1_l_dse_ctrl_00,	window_acquire,			0x1);	//window acquire on
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	win_selection,			0x0);	//from pe1 win2
				DE_P1L_L9B0_WrFL(pe1_l_dse_ctrl_00);

				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_address,	0x0);	//address
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_load,	 	0x0);		//0:clear, 1:enable
				DE_P1L_L9B0_WrFL(pe1_l_dse_ia_ctrl);
				lut_data = 0;
				for(count=0;count<32;count++)
				{
					if(lut_data>0x3ff)	lut_data=0x3ff;
					_g_dse_lut_data[LX_PE_WIN_MAIN][count]=((lut_data<<16)|(lut_data));
					DE_P1L_L9B0_Wr01(pe1_l_dse_ia_data,	hif_dse_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1L_L9B0_Wr01(pe1_l_dse_ia_data,	hif_dse_wdata_x,	GET_BITS(lut_data,0,10));	//x data
					DE_P1L_L9B0_WrFL(pe1_l_dse_ia_data);
					lut_data += step_unit;
				}
				DE_P1L_L9B0_Wr01(pe1_l_dse_ctrl_02,	hif_dse_wdata_y_32nd,	0x3ff);	//y data
				DE_P1L_L9B0_Wr01(pe1_l_dse_ctrl_02,	hif_dse_wdata_x_32nd,	0x3ff);	//x data
				DE_P1L_L9B0_WrFL(pe1_l_dse_ctrl_02);
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_load,	 	0x1);		//0:clear, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_enable,		0x1);		//0:host access, 1:normal mode
				DE_P1L_L9B0_WrFL(pe1_l_dse_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_01);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ia_ctrl);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ia_data);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_08);
				//DE_P1R_L9B0_RdFL(pe1_r_dce_hist_ia_ctrl);
				DE_P1R_L9B0_RdFL(pe1_r_apl_ctrl_02);
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	dynamic_contrast_en,	0x1);					//enable
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	hist_bin_mode,			bin_mode);				//32bin mode
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	window_mode_en,			0x1);					//window sync
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	window_acquire,			0x1);					//window acquire on
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	dce_domain_sel,			(domain==1)? 0x1:0x0);	//HSV or YC ddomain
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	win_selection,			0x0);					//from pe1 win2
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_01,	hist_bin_min_position,	min_pos);				//min position
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_01,	hist_bin_max_position,	max_pos);				//max position
				//DE_P1R_L9B0_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_status_address,	0x0);				//V value for range
				DE_P1R_L9B0_Wr01(pe1_r_apl_ctrl_02,	apl_position,			(domain==1)? 0x0:0x2);	//0x0:PE1 input(Y/C),0x2:cen input(RGB)
				DE_P1R_L9B0_WrFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9B0_WrFL(pe1_r_dce_ctrl_01);
				//DE_P1R_L9B0_WrFL(pe1_r_dce_hist_ia_ctrl);
				DE_P1R_L9B0_WrFL(pe1_r_apl_ctrl_02);
				if(domain!=1)	/* should set CEN block ON to use HSV DCE */
				{
					DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_00);
					DE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_00,	reg_cen_bypass,	0x1);					//CEN block ON
					DE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_00,	v_scaler_en,	(domain==0)? 0x1:0x0);	//v scaler en
					DE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_00,	demo_mode,		0x0);
					DE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_00,	vsp_sel,		(domain==3)? 0x0:0x1);	//0:hsl,1:hsv
					DE_P1R_L9B0_WrFL(pe1_r_cen_ctrl_00);
				}
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1R_L9B0_WrFL(pe1_r_dce_ia_ctrl);
				_g_dce_lut_step[LX_PE_WIN_SUB] = lut_step;
				lut_data = 0;
				for(count=0;count<32;count++)
				{
					if(lut_data>0x3ff)	lut_data=0x3ff;		
					DE_P1R_L9B0_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1R_L9B0_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,0,10));	//x data
					DE_P1R_L9B0_WrFL(pe1_r_dce_ia_data);
					lut_data += step_unit;
				}
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_08,	hif_dyc_wdata_y_32nd,	0x3ff);	//y data
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_08,	hif_dyc_wdata_x_32nd,	0x3ff);	//x data
				DE_P1R_L9B0_WrFL(pe1_r_dce_ctrl_08);
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable,		0x1);		//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_load,	 	0x1);	//0:clear, 1:enable
				DE_P1R_L9B0_WrFL(pe1_r_dce_ia_ctrl);

				/* dse config */
				DE_P1R_L9B0_RdFL(pe1_r_dse_ctrl_00);
				DE_P1R_L9B0_Wr01(pe1_r_dse_ctrl_00,	dynamic_saturation_en,	0x1);
				DE_P1R_L9B0_Wr01(pe1_r_dse_ctrl_00,	window_mode_en,			0x1);	//window sync
				DE_P1R_L9B0_Wr01(pe1_r_dse_ctrl_00,	window_acquire,			0x1);	//window acquire on
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	win_selection,			0x0);	//from pe1 win2
				DE_P1R_L9B0_WrFL(pe1_r_dse_ctrl_00);

				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_address,	0x0);	//address
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_load,	 	0x0);	//0:clear, 1:enable
				DE_P1R_L9B0_WrFL(pe1_r_dse_ia_ctrl);
				lut_data = 0;
				for(count=0;count<32;count++)
				{
					if(lut_data>0x3ff)	lut_data=0x3ff;
					_g_dse_lut_data[LX_PE_WIN_SUB][count]=((lut_data<<16)|(lut_data));
					DE_P1R_L9B0_Wr01(pe1_r_dse_ia_data,	hif_dse_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1R_L9B0_Wr01(pe1_r_dse_ia_data,	hif_dse_wdata_x,	GET_BITS(lut_data,0,10));	//x data
					DE_P1R_L9B0_WrFL(pe1_r_dse_ia_data);
					lut_data += step_unit;
				}
				DE_P1R_L9B0_Wr01(pe1_r_dse_ctrl_02,	hif_dse_wdata_y_32nd,	0x3ff);	//y data
				DE_P1R_L9B0_Wr01(pe1_r_dse_ctrl_02,	hif_dse_wdata_x_32nd,	0x3ff);	//x data
				DE_P1R_L9B0_WrFL(pe1_r_dse_ctrl_02);
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_enable,		0x1);		//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_load,	 	0x1);	//0:clear, 1:enable
				DE_P1R_L9B0_WrFL(pe1_r_dse_ia_ctrl);
			}
		}
		else
		{
			if(pstParams->bin_mode==LX_PE_16_BINMODE)		{bin_mode=0x2;}
			else if(pstParams->bin_mode==LX_PE_32_BINMODE)	{bin_mode=0x1;}
			else											{bin_mode=0x0;}
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9_RdFL(pe1_l_dce_ctrl_01);
				DE_P1L_L9_RdFL(pe1_l_dce_ia_ctrl);
				DE_P1L_L9_RdFL(pe1_l_dce_ia_data);
				DE_P1L_L9_RdFL(pe1_l_dce_ctrl_08);
				DE_P1L_L9_RdFL(pe1_l_dce_hist_ia_ctrl);
				DE_P1L_L9_RdFL(pe1_l_apl_ctrl_02);
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	dynamic_contrast_en,	0x1);					//enable
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	hist_bin_mode,			bin_mode);				//32bin mode
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	hist_bin_bitwidth_mode,	0x0);					//21bit full read
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	window_mode_en,			0x1);					//window sync
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	window_acquire,			0x1);					//window acquire on
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	dce_domain_sel,			(domain==1)? 0x1:0x0);	//HSV or YC domain
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_00,	win_selection,			0x1);					//from bbd
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_01,	hist_bin_min_position,	min_pos);				//min position
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_01,	hist_bin_max_position,	max_pos);				//max position
				DE_P1L_L9_Wr01(pe1_l_dce_hist_ia_ctrl,	hif_status_address,	0x0);					//Maximum V value for one frame
				DE_P1L_L9_Wr01(pe1_l_apl_ctrl_02,	apl_position,			(domain==1)? 0x0:0x2);	//0x0:PE1 input(Y/C),0x2:cen input(RGB)
				DE_P1L_L9_WrFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9_WrFL(pe1_l_dce_ctrl_01);
				DE_P1L_L9_WrFL(pe1_l_dce_hist_ia_ctrl);
				DE_P1L_L9_WrFL(pe1_l_apl_ctrl_02);
				if(domain!=1)	/* should set CEN block ON to use HSV DCE */
				{
					DE_P1L_L9_RdFL(pe1_l_cen_ctrl_00);
					DE_P1L_L9_Wr01(pe1_l_cen_ctrl_00,	cen_bypass,		0x1);					//CEN block ON
					DE_P1L_L9_Wr01(pe1_l_cen_ctrl_00,	v_scaler_en,	(domain==0)? 0x1:0x0);	//v scaler en
					DE_P1L_L9_WrFL(pe1_l_cen_ctrl_00);
				}
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1L_L9_WrFL(pe1_l_dce_ia_ctrl);
				_g_dce_lut_step[LX_PE_WIN_MAIN] = lut_step;
				lut_data = 0;
				for(count=0;count<32;count++)
				{
					if(lut_data>0x3ff)	lut_data=0x3ff;
					DE_P1L_L9_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1L_L9_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,0,10));	//x data
					DE_P1L_L9_WrFL(pe1_l_dce_ia_data);
					lut_data += step_unit;
				}
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_08,	hif_dyc_wdata_y_32nd,	0x3ff);	//y data
				DE_P1L_L9_Wr01(pe1_l_dce_ctrl_08,	hif_dyc_wdata_x_32nd,	0x3ff);	//x data
				DE_P1L_L9_WrFL(pe1_l_dce_ctrl_08);
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable,		0x1);		//0:host access, 1:normal mode
				DE_P1L_L9_WrFL(pe1_l_dce_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9_RdFL(pe1_r_dce_ctrl_01);
				DE_P1R_L9_RdFL(pe1_r_dce_ia_ctrl);
				DE_P1R_L9_RdFL(pe1_r_dce_ia_data);
				DE_P1R_L9_RdFL(pe1_r_dce_ctrl_08);
				DE_P1R_L9_RdFL(pe1_r_dce_hist_ia_ctrl);
				DE_P1R_L9_RdFL(pe1_r_apl_ctrl_02);
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	dynamic_contrast_en,	0x1);					//enable
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	hist_bin_mode,			bin_mode);				//32bin mode
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	hist_bin_bitwidth_mode,	0x0);					//21bit full read
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	window_mode_en,			0x1);					//window sync
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	window_acquire,			0x1);					//window acquire on
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	dce_domain_sel,			(domain==1)? 0x1:0x0);	//HSV or YC ddomain
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_00,	win_selection,			0x1);					//from bbd
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_01,	hist_bin_min_position,	min_pos);				//min position
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_01,	hist_bin_max_position,	max_pos);				//max position
				DE_P1R_L9_Wr01(pe1_r_dce_hist_ia_ctrl,	hif_status_address,	0x0);					//Maximum V value for one frame
				DE_P1R_L9_Wr01(pe1_r_apl_ctrl_02,	apl_position,			(domain==1)? 0x0:0x2);	//0x0:PE1 input(Y/C),0x2:cen input(RGB)
				DE_P1R_L9_WrFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9_WrFL(pe1_r_dce_ctrl_01);
				DE_P1R_L9_WrFL(pe1_r_dce_hist_ia_ctrl);
				DE_P1R_L9_WrFL(pe1_r_apl_ctrl_02);
				if(domain!=1)	/* should set CEN block ON to use HSV DCE */
				{
					DE_P1R_L9_RdFL(pe1_r_cen_ctrl_00);
					DE_P1R_L9_Wr01(pe1_r_cen_ctrl_00,	cen_bypass,		0x1);					//CEN block ON
					DE_P1R_L9_Wr01(pe1_r_cen_ctrl_00,	v_scaler_en,	(domain==0)? 0x1:0x0);	//v scaler en
					DE_P1R_L9_WrFL(pe1_r_cen_ctrl_00);
				}
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1R_L9_WrFL(pe1_r_dce_ia_ctrl);
				_g_dce_lut_step[LX_PE_WIN_SUB] = lut_step;
				lut_data = 0;
				for(count=0;count<32;count++)
				{
					if(lut_data>0x3ff)	lut_data=0x3ff;		
					DE_P1R_L9_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1R_L9_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,0,10));	//x data
					DE_P1R_L9_WrFL(pe1_r_dce_ia_data);
					lut_data += step_unit;
				}
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_08,	hif_dyc_wdata_y_32nd,	0x3ff);	//y data
				DE_P1R_L9_Wr01(pe1_r_dce_ctrl_08,	hif_dyc_wdata_x_32nd,	0x3ff);	//x data
				DE_P1R_L9_WrFL(pe1_r_dce_ctrl_08);
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable,		0x1);		//0:host access, 1:normal mode
				DE_P1R_L9_WrFL(pe1_r_dce_ia_ctrl);
			}
		}
	} while (0);
	return ret;
}
static int PE_DCM_GetDceConfig(LX_PE_DCM_DCE_CONF_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 bin_mode=0;
	UINT32 domain=0;
	UINT32 min_pos=0;
	UINT32 max_pos=0;
	UINT32 v_scl_en=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_01);
				DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_00);
				DE_P1L_L9B0_Rd01(pe1_l_dce_ctrl_00,	hist_bin_mode,			bin_mode);	//32bin mode
				DE_P1L_L9B0_Rd01(pe1_l_dce_ctrl_00,	dce_domain_sel,			domain);	//HSV or YC domain
				DE_P1L_L9B0_Rd01(pe1_l_dce_ctrl_01,	hist_bin_min_position,	min_pos);	//min position
				DE_P1L_L9B0_Rd01(pe1_l_dce_ctrl_01,	hist_bin_max_position,	max_pos);	//max position
				DE_P1L_L9B0_Rd01(pe1_l_cen_ctrl_00,	v_scaler_en,			v_scl_en);	//V scaler en
				pstParams->bin_mode = bin_mode? LX_PE_16_BINMODE:LX_PE_32_BINMODE;
				pstParams->domain = domain? LX_PE_YC_DOMAIN:(v_scl_en? LX_PE_HSV_DOMAIN:LX_PE_ORG_HSV_DOMAIN);
				pstParams->min_pos = min_pos;
				pstParams->max_pos = max_pos;
				pstParams->lut_step = _g_dce_lut_step[LX_PE_WIN_MAIN];
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_01);
				DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_00);
				DE_P1R_L9B0_Rd01(pe1_r_dce_ctrl_00,	hist_bin_mode,			bin_mode);	//32bin mode
				DE_P1R_L9B0_Rd01(pe1_r_dce_ctrl_00,	dce_domain_sel,			domain);	//HSV or YC domain
				DE_P1R_L9B0_Rd01(pe1_r_dce_ctrl_01,	hist_bin_min_position,	min_pos);	//min position
				DE_P1R_L9B0_Rd01(pe1_r_dce_ctrl_01,	hist_bin_max_position,	max_pos);	//max position
				DE_P1R_L9B0_Rd01(pe1_r_cen_ctrl_00,	v_scaler_en,			v_scl_en);	//V scaler en
				pstParams->bin_mode = bin_mode? LX_PE_16_BINMODE:LX_PE_32_BINMODE;
				pstParams->domain = domain? LX_PE_YC_DOMAIN:(v_scl_en? LX_PE_HSV_DOMAIN:LX_PE_ORG_HSV_DOMAIN);
				pstParams->min_pos = min_pos;
				pstParams->max_pos = max_pos;
				pstParams->lut_step = _g_dce_lut_step[LX_PE_WIN_SUB];
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9_RdFL(pe1_l_dce_ctrl_01);
				DE_P1L_L9_RdFL(pe1_l_cen_ctrl_00);
				DE_P1L_L9_Rd01(pe1_l_dce_ctrl_00,	hist_bin_mode,			bin_mode);	//32bin mode
				DE_P1L_L9_Rd01(pe1_l_dce_ctrl_00,	dce_domain_sel,			domain);	//HSV or YC domain
				DE_P1L_L9_Rd01(pe1_l_dce_ctrl_01,	hist_bin_min_position,	min_pos);	//min position
				DE_P1L_L9_Rd01(pe1_l_dce_ctrl_01,	hist_bin_max_position,	max_pos);	//max position
				DE_P1L_L9_Rd01(pe1_l_cen_ctrl_00,	v_scaler_en,			v_scl_en);	//V scaler en		
				pstParams->bin_mode = (LX_PE_BINMODE)bin_mode;
				pstParams->domain = domain? LX_PE_YC_DOMAIN:(v_scl_en? LX_PE_HSV_DOMAIN:LX_PE_ORG_HSV_DOMAIN);
				pstParams->min_pos = min_pos;
				pstParams->max_pos = max_pos;
				pstParams->lut_step = _g_dce_lut_step[LX_PE_WIN_MAIN];
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9_RdFL(pe1_r_dce_ctrl_01);
				DE_P1R_L9_RdFL(pe1_r_cen_ctrl_00);
				DE_P1R_L9_Rd01(pe1_r_dce_ctrl_00,	hist_bin_mode,			bin_mode);	//32bin mode
				DE_P1R_L9_Rd01(pe1_r_dce_ctrl_00,	dce_domain_sel,			domain);	//HSV or YC domain
				DE_P1R_L9_Rd01(pe1_r_dce_ctrl_01,	hist_bin_min_position,	min_pos);	//min position
				DE_P1R_L9_Rd01(pe1_r_dce_ctrl_01,	hist_bin_max_position,	max_pos);	//max position
				DE_P1R_L9_Rd01(pe1_r_cen_ctrl_00,	v_scaler_en,			v_scl_en);	//V scaler en
				pstParams->bin_mode = (LX_PE_BINMODE)bin_mode;
				pstParams->domain = domain? LX_PE_YC_DOMAIN:(v_scl_en? LX_PE_HSV_DOMAIN:LX_PE_ORG_HSV_DOMAIN);
				pstParams->min_pos = min_pos;
				pstParams->max_pos = max_pos;
				pstParams->lut_step = _g_dce_lut_step[LX_PE_WIN_SUB];
			}
		}
	} while (0);
	return ret;
}
static int PE_DCM_SetDceLut(LX_PE_DCM_DCE_LUT_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count=0;
	UINT32 lut_data=0;
	UINT32 lut_step=0;
	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);

		if(pstParams->step>32)		{lut_step=32;}
		else if(pstParams->step==0)	{lut_step=1;}
		else						{lut_step=pstParams->step;}

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_dce_ia_ctrl);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ia_data);
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1L_L9B0_WrFL(pe1_l_dce_ia_ctrl);
				for(count=0;count<lut_step;count++)
				{
					lut_data = pstParams->data[count];
					DE_P1L_L9B0_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1L_L9B0_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,16,10));	//x data
					DE_P1L_L9B0_WrFL(pe1_l_dce_ia_data);
				}
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_load,	 	0x1);	//0:clear, 1:enable
				DE_P1L_L9B0_WrFL(pe1_l_dce_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_dce_ia_ctrl);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ia_data);
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1R_L9B0_WrFL(pe1_r_dce_ia_ctrl);
				for(count=0;count<lut_step;count++)
				{
					lut_data = pstParams->data[count];		
					DE_P1R_L9B0_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1R_L9B0_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,16,10));	//x data
					DE_P1R_L9B0_WrFL(pe1_r_dce_ia_data);
				}
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_load,	 	0x1);	//0:clear, 1:enable
				DE_P1R_L9B0_WrFL(pe1_r_dce_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_dce_ia_ctrl);
				DE_P1L_L9_RdFL(pe1_l_dce_ia_data);
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1L_L9_WrFL(pe1_l_dce_ia_ctrl);
				for(count=0;count<lut_step;count++)
				{
					lut_data = pstParams->data[count];
					DE_P1L_L9_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1L_L9_Wr01(pe1_l_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,16,10));	//x data
					DE_P1L_L9_WrFL(pe1_l_dce_ia_data);
				}
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_load,	 	0x1);	//0:clear, 1:enable
				DE_P1L_L9_WrFL(pe1_l_dce_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_dce_ia_ctrl);
				DE_P1R_L9_RdFL(pe1_r_dce_ia_data);
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1R_L9_WrFL(pe1_r_dce_ia_ctrl);
				for(count=0;count<lut_step;count++)
				{
					lut_data = pstParams->data[count];		
					DE_P1R_L9_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1R_L9_Wr01(pe1_r_dce_ia_data,	hif_dyc_wdata_x,	GET_BITS(lut_data,16,10));	//x data
					DE_P1R_L9_WrFL(pe1_r_dce_ia_data);
				}
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_load,	 	0x1);	//0:clear, 1:enable
				DE_P1R_L9_WrFL(pe1_r_dce_ia_ctrl);
			}
		}
	} while (0);	
	return ret;
}
static int PE_DCM_GetDceLut(LX_PE_DCM_DCE_LUT_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 count=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_dce_ia_ctrl);
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable, 	0x0);	//0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_ai, 		0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1L_L9B0_WrFL(pe1_l_dce_ia_ctrl);
				for(count=0;count<(pstParams->step);count++)
				{
					DE_P1L_L9B0_RdFL(pe1_l_dce_ia_data);
					pstParams->data[count] = DE_P1L_L9B0_Rd(pe1_l_dce_ia_data);
				}
				DE_P1L_L9B0_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1L_L9B0_WrFL(pe1_l_dce_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_dce_ia_ctrl);
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable, 	0x0);	//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_ai, 		0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1R_L9B0_WrFL(pe1_r_dce_ia_ctrl);		
				for(count=0;count<(pstParams->step);count++)
				{
					DE_P1R_L9B0_RdFL(pe1_r_dce_ia_data);
					pstParams->data[count] = DE_P1R_L9B0_Rd(pe1_r_dce_ia_data);
				}
				DE_P1R_L9B0_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1R_L9B0_WrFL(pe1_r_dce_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_dce_ia_ctrl);
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable, 	0x0);	//0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_ai, 		0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1L_L9_WrFL(pe1_l_dce_ia_ctrl);
				for(count=0;count<(pstParams->step);count++)
				{
					DE_P1L_L9_RdFL(pe1_l_dce_ia_data);
					pstParams->data[count] = DE_P1L_L9_Rd(pe1_l_dce_ia_data);
				}
				DE_P1L_L9_Wr01(pe1_l_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1L_L9_WrFL(pe1_l_dce_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_dce_ia_ctrl);
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable, 	0x0);	//0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_ai, 		0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dyc_address,	0x0);	//address
				DE_P1R_L9_WrFL(pe1_r_dce_ia_ctrl);		
				for(count=0;count<(pstParams->step);count++)
				{
					DE_P1R_L9_RdFL(pe1_r_dce_ia_data);
					pstParams->data[count] = DE_P1R_L9_Rd(pe1_r_dce_ia_data);
				}
				DE_P1R_L9_Wr01(pe1_r_dce_ia_ctrl,	hif_dce_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1R_L9_WrFL(pe1_r_dce_ia_ctrl);
			}
		}
	} while (0);	
	return ret;
}
static int PE_DCM_SetDseLut(LX_PE_DCM_DSE_LUT_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count=0;
	UINT32 lut_data=0;
	UINT32 lut_step=0;
	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);

		if(pstParams->step>32)		{lut_step=32;}
		else if(pstParams->step==0)	{lut_step=1;}
		else						{lut_step=pstParams->step;}

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_dse_ia_ctrl);
				DE_P1L_L9B0_RdFL(pe1_l_dse_ia_data);
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_address,	0x0);	//address
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_load,	 	0x0);	//0:clear, 1:enable
				DE_P1L_L9B0_WrFL(pe1_l_dse_ia_ctrl);
				for(count=0;count<lut_step;count++)
				{
					lut_data = pstParams->data[count];
					_g_dse_lut_data[LX_PE_WIN_MAIN][count]=((lut_data<<16)|(lut_data));
					DE_P1L_L9B0_Wr01(pe1_l_dse_ia_data,	hif_dse_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1L_L9B0_Wr01(pe1_l_dse_ia_data,	hif_dse_wdata_x,	GET_BITS(lut_data,16,10));	//x data
					DE_P1L_L9B0_WrFL(pe1_l_dse_ia_data);
				}
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_dse_ia_ctrl,	hif_dse_load,	 	0x1);	//0:clear, 1:enable
				DE_P1L_L9B0_WrFL(pe1_l_dse_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_dse_ia_ctrl);
				DE_P1R_L9B0_RdFL(pe1_r_dse_ia_data);
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_enable,		0x0);	//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_ai,			0x1);	//ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_address,	0x0);	//address
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_load,	 	0x0);	//0:clear, 1:enable
				DE_P1R_L9B0_WrFL(pe1_r_dse_ia_ctrl);
				for(count=0;count<lut_step;count++)
				{
					lut_data = pstParams->data[count];
					_g_dse_lut_data[LX_PE_WIN_SUB][count]=((lut_data<<16)|(lut_data));
					DE_P1R_L9B0_Wr01(pe1_r_dse_ia_data,	hif_dse_wdata_y,	GET_BITS(lut_data,0,10));	//y data
					DE_P1R_L9B0_Wr01(pe1_r_dse_ia_data,	hif_dse_wdata_x,	GET_BITS(lut_data,16,10));	//x data
					DE_P1R_L9B0_WrFL(pe1_r_dse_ia_data);
				}
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_enable, 	0x1);	//0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_dse_ia_ctrl,	hif_dse_load,	 	0x1);	//0:clear, 1:enable
				DE_P1R_L9B0_WrFL(pe1_r_dse_ia_ctrl);
			}
		}
		else
		{
			PE_DCM_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	} while (0);	
	return ret;
}
static int PE_DCM_GetDseLut(LX_PE_DCM_DSE_LUT_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 count=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				for(count=0;count<(pstParams->step);count++)
				{
					pstParams->data[count] = _g_dse_lut_data[LX_PE_WIN_MAIN][count];
				}
			}
			if(PE_CHECK_SUB(win_id))
			{	
				for(count=0;count<(pstParams->step);count++)
				{
					pstParams->data[count] = _g_dse_lut_data[LX_PE_WIN_SUB][count];
				}
			}
		}
		else
		{
			PE_DCM_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	} while (0);	
	return ret;
}
static int PE_DCM_SetDceBlendingCtrl(LX_PE_DCM_BLENDING_T *pstParams)
{
	int ret = RET_OK;
	UINT32 color_out_gain;
	UINT32 region_sel;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		color_out_gain = pstParams->color_out_gain;
		region_sel = pstParams->region_sel;
		PE_DCM_CHECK_CODE(!color_out_gain,return RET_ERROR,"[%s,%d] color_out_gain is zero.\n",__F__,__L__);
		PE_DCM_CHECK_CODE(color_out_gain>0xff,return RET_ERROR,\
			"[%s,%d] color_out_gain(%d) is over.\n",__F__,__L__,color_out_gain);
		PE_DCM_DBG_PRINT("set pstParams[%d] : color_out_gain:%d\n",pstParams->win_id,color_out_gain);
		if(_g_dcm_trace)
		{
			UINT32 i,sel;
			for(i=0;i<16;i++)
			{
				sel = ((region_sel>>i)&0x1);
				PE_DCM_DBG_PRINT("[%2d]%d\n",i,sel);
			}
		}

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_01);
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region0_sel,	GET_BITS(region_sel,0,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region1_sel,	GET_BITS(region_sel,1,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region2_sel,	GET_BITS(region_sel,2,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region3_sel,	GET_BITS(region_sel,3,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region4_sel,	GET_BITS(region_sel,4,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region5_sel,	GET_BITS(region_sel,5,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region6_sel,	GET_BITS(region_sel,6,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region7_sel,	GET_BITS(region_sel,7,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region8_sel,	GET_BITS(region_sel,8,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region9_sel,	GET_BITS(region_sel,9,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region10_sel,	GET_BITS(region_sel,10,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region11_sel,	GET_BITS(region_sel,11,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region12_sel,	GET_BITS(region_sel,12,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region13_sel,	GET_BITS(region_sel,13,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region14_sel,	GET_BITS(region_sel,14,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_00,	color_region15_sel,	GET_BITS(region_sel,15,1));
				DE_P1L_L9B0_Wr01(pe1_l_dce_ctrl_01,	color_region_gain,	GET_BITS(color_out_gain,0,8));
				DE_P1L_L9B0_WrFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9B0_WrFL(pe1_l_dce_ctrl_01);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_01);
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region0_sel,	GET_BITS(region_sel,0,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region1_sel,	GET_BITS(region_sel,1,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region2_sel,	GET_BITS(region_sel,2,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region3_sel,	GET_BITS(region_sel,3,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region4_sel,	GET_BITS(region_sel,4,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region5_sel,	GET_BITS(region_sel,5,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region6_sel,	GET_BITS(region_sel,6,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region7_sel,	GET_BITS(region_sel,7,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region8_sel,	GET_BITS(region_sel,8,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region9_sel,	GET_BITS(region_sel,9,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region10_sel,	GET_BITS(region_sel,10,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region11_sel,	GET_BITS(region_sel,11,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region12_sel,	GET_BITS(region_sel,12,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region13_sel,	GET_BITS(region_sel,13,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region14_sel,	GET_BITS(region_sel,14,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_00,	color_region15_sel,	GET_BITS(region_sel,15,1));
				DE_P1R_L9B0_Wr01(pe1_r_dce_ctrl_01,	color_region_gain,	GET_BITS(color_out_gain,0,8));
				DE_P1R_L9B0_WrFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9B0_WrFL(pe1_r_dce_ctrl_01);
			}
		}
		else
		{
			PE_DCM_DBG_PRINT("nothing to do.\n");
		}
	} while (0);	
	return ret;
}
static int PE_DCM_GetDceBlendingCtrl(LX_PE_DCM_BLENDING_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 r_data;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_00);
				DE_P1L_L9B0_RdFL(pe1_l_dce_ctrl_01);
				DE_P1L_L9B0_Rd01(pe1_l_dce_ctrl_01,	color_region_gain,	pstParams->color_out_gain);
				r_data = DE_P1L_L9B0_Rd(pe1_l_dce_ctrl_00);
				pstParams->region_sel = GET_BITS(r_data,8,16);	//[23:8]
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_00);
				DE_P1R_L9B0_RdFL(pe1_r_dce_ctrl_01);
				DE_P1R_L9B0_Rd01(pe1_r_dce_ctrl_01,	color_region_gain,	pstParams->color_out_gain);
				r_data = DE_P1R_L9B0_Rd(pe1_r_dce_ctrl_00);
				pstParams->region_sel = GET_BITS(r_data,8,16);	//[23:8]
			}
		}
		else
		{
			PE_DCM_DBG_PRINT("nothing to do.\n");
		}
		PE_DCM_DBG_PRINT("get pstParams[%d] : color_out_gain:%d\n",pstParams->win_id,pstParams->color_out_gain);
		if(_g_dcm_trace)
		{
			UINT32 i,sel;
			for(i=0;i<16;i++)
			{
				sel = ((pstParams->region_sel>>i)&0x1);
				PE_DCM_DBG_PRINT("[%2d]%d\n",i,sel);
			}
		}
	} while (0);	
	return ret;
}

