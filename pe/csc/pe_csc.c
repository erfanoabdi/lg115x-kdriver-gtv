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

/** @file pe_csc.c
 *
 *  driver for picture enhance color space conversion functions. ( used only within kdriver )
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
#include "pe_csc.h"
#include "pe_csc_coeff.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_CSC_ERROR	printk

#define PE_CSC_DBG_PRINT(fmt,args...)	if(_g_csc_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_CSC_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_CSC_ERROR(fmt,##args);_action;}}

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

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int PE_CSC_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_CSC_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_CSC_SetColorGamut(LX_PE_CSC_GAMUT_T *pstParams);
static int PE_CSC_GetColorGamut(LX_PE_CSC_GAMUT_T *pstParams);
static int PE_CSC_SetxvYCC(LX_PE_CSC_XVYCC_T *pstParams);
static int PE_CSC_GetxvYCC(LX_PE_CSC_XVYCC_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_csc_trace=0x0;	//default should be off.

/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 * init csc
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CSC_Init(void)
{
	int ret = RET_OK;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_CSC_DBG_PRINT("init xvy ctrl .\n");
			DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_00);
			DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, outputcsc_en,		0x1);
			DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, degamma_en,			0x1);
			DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, pcc_en,				0x1);
			DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, gamma_en,			0x1);
			DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, hsv_hsl_selection,	0x0);
			DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_00);
			DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_00);
			DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, outputcsc_en,		0x1);
			DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, degamma_en,			0x1);
			DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, pcc_en,				0x1);
			DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, gamma_en,			0x1);
			DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, hsv_hsl_selection,	0x0);
			DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_00);
		}
		else
		{
			PE_CSC_DBG_PRINT("init xvy ctrl .\n");
			DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_00);
			DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_00, icsc_en,			0x1);
			DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_00, degamma_en,		0x1);
			DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_00, pcc_en,			0x1);
			DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_00, gamma,			0x1);
			DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_00, hsv_hsl_selection,0x0);
			DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_00);
			DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_00);
			DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_00, icsc_en,			0x1);
			DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_00, degamma_en,		0x1);
			DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_00, pcc_en,			0x1);
			DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_00, gamma,			0x1);
			DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_00, hsv_hsl_selection,0x0);
			DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_00);
		}
	}while(0);

	return ret;
}

/**
 * standard function of csc setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CSC_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CSC_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CSC_GAMUT_T):
		{
			LX_PE_CSC_GAMUT_T *pRcvData;
			LX_PE_CSC_GAMUT_T stParam;
			do{
				PE_CSC_DBG_PRINT("S LX_PE_CSC_GAMUT_T\n");
				/* data size check */
				PE_CSC_CHECK_CODE(datasize!=sizeof(LX_PE_CSC_GAMUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CSC_GAMUT_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CSC_SetColorGamut(&stParam);
				PE_CSC_CHECK_CODE(ret,break,"[%s,%d] PE_CSC_SetColorGamut() error.\n",__F__,__L__);
				PE_CSC_DBG_PRINT("E LX_PE_CSC_GAMUT_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CSC_XVYCC_T):
		{
			LX_PE_CSC_XVYCC_T *pRcvData;
			LX_PE_CSC_XVYCC_T stParam;
			do{
				PE_CSC_DBG_PRINT("S LX_PE_CSC_XVYCC_T\n");
				/* data size check */
				PE_CSC_CHECK_CODE(datasize!=sizeof(LX_PE_CSC_XVYCC_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CSC_XVYCC_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CSC_SetxvYCC(&stParam);
				PE_CSC_CHECK_CODE(ret,break,"[%s,%d] PE_CSC_SetxvYCC() error.\n",__F__,__L__);
				PE_CSC_DBG_PRINT("E LX_PE_CSC_XVYCC_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_CSC_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_CSC_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CSC_SetDefaultSettings(&stParam);
				PE_CSC_CHECK_CODE(ret,break,"[%s,%d] PE_CSC_SetDefaultSettings() error.\n",__F__,__L__);
				PE_CSC_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_CSC_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_CSC_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CSC_SetDebugSettings(&stParam);
				PE_CSC_CHECK_CODE(ret,break,"[%s,%d] PE_CSC_SetDebugSettings() error.\n",__F__,__L__);
				PE_CSC_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CSC_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of csc getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CSC_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CSC_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CSC_GAMUT_T):
		{
			LX_PE_CSC_GAMUT_T *pRcvData;
			LX_PE_CSC_GAMUT_T stParam;
			memset0(stParam);
			do{
				PE_CSC_DBG_PRINT("S LX_PE_CSC_GAMUT_T\n");
				/* data size check */
				PE_CSC_CHECK_CODE(datasize!=sizeof(LX_PE_CSC_GAMUT_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CSC_GAMUT_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CSC_GetColorGamut(&stParam);
				PE_CSC_CHECK_CODE(ret,break,"[%s,%d] PE_CSC_GetColorGamut() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CSC_DBG_PRINT("E LX_PE_CSC_GAMUT_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CSC_XVYCC_T):
		{
			LX_PE_CSC_XVYCC_T *pRcvData;
			LX_PE_CSC_XVYCC_T stParam;
			memset0(stParam);
			do{
				PE_CSC_DBG_PRINT("S LX_PE_CSC_XVYCC_T\n");
				/* data size check */
				PE_CSC_CHECK_CODE(datasize!=sizeof(LX_PE_CSC_XVYCC_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CSC_XVYCC_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CSC_GetxvYCC(&stParam);
				PE_CSC_CHECK_CODE(ret,break,"[%s,%d] PE_CSC_GetxvYCC() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CSC_DBG_PRINT("E LX_PE_CSC_XVYCC_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CSC_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_CSC_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CSC_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);
	}while(0);
	return ret;
}
static int PE_CSC_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CSC_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(CSC)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_csc_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(CSC)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(CSC)))
				{
					PE_CSC_DBG_PRINT("degamma,pcc,gamma,s scaler : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, degamma_en,					0x0);
						DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, pcc_en,						0x0);
						DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, gamma_en,					0x0);
						DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, s_adpative_scaler_enable,	0x0);
						DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, degamma_en,					0x0);
						DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, pcc_en,						0x0);
						DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, gamma_en,					0x0);
						DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, s_adpative_scaler_enable,	0x0);
						DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_00);
					}
				}
				else
				{
					PE_CSC_DBG_PRINT("outcsc,degamma,pcc,gamma : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_00);
						DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, degamma_en,			0x1);
						DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, pcc_en,				0x1);
						DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, gamma_en,			0x1);
						DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_00);
						DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, degamma_en,			0x1);
						DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, pcc_en,				0x1);
						DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, gamma_en,			0x1);
						DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_00);
					}
				}
			}
			else
			{
				PE_CSC_DBG_PRINT("do nothing.\n");	ret = RET_OK;
			}
		}
	}while(0);
	return ret;
}
static int PE_CSC_SetColorGamut(LX_PE_CSC_GAMUT_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CSC_DBG_PRINT("set pstParams[%d] : 0x%x,0x%x,0x%x, 0x%x,0x%x,0x%x, 0x%x,0x%x,0x%x\n",\
			pstParams->win_id,pstParams->matrix[0],pstParams->matrix[1],pstParams->matrix[2],pstParams->matrix[3],\
			pstParams->matrix[4],pstParams->matrix[5],pstParams->matrix[6],pstParams->matrix[7],pstParams->matrix[8]);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_08);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_09);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0a);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0b);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0c);
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_08, r_pcc_coef0, GET_BITS(pstParams->matrix[0],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_09, r_pcc_coef1, GET_BITS(pstParams->matrix[1],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_09, r_pcc_coef2, GET_BITS(pstParams->matrix[2],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_0a, r_pcc_coef3, GET_BITS(pstParams->matrix[3],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_0a, r_pcc_coef4, GET_BITS(pstParams->matrix[4],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_0b, r_pcc_coef5, GET_BITS(pstParams->matrix[5],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_0b, r_pcc_coef6, GET_BITS(pstParams->matrix[6],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_0c, r_pcc_coef7, GET_BITS(pstParams->matrix[7],0,15));
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_0c, r_pcc_coef8, GET_BITS(pstParams->matrix[8],0,15));
				DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_0c);
				DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_0b);
				DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_0a);
				DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_09);
				DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_08);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_08);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_09);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0a);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0b);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0c);
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_08, r_pcc_coef0, GET_BITS(pstParams->matrix[0],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_09, r_pcc_coef1, GET_BITS(pstParams->matrix[1],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_09, r_pcc_coef2, GET_BITS(pstParams->matrix[2],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_0a, r_pcc_coef3, GET_BITS(pstParams->matrix[3],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_0a, r_pcc_coef4, GET_BITS(pstParams->matrix[4],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_0b, r_pcc_coef5, GET_BITS(pstParams->matrix[5],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_0b, r_pcc_coef6, GET_BITS(pstParams->matrix[6],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_0c, r_pcc_coef7, GET_BITS(pstParams->matrix[7],0,15));
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_0c, r_pcc_coef8, GET_BITS(pstParams->matrix[8],0,15));
				DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_0c);
				DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_0b);
				DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_0a);
				DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_09);
				DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_08);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_08);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_09);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0a);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0b);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0c);
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_08, r_pcc_coef0, GET_BITS(pstParams->matrix[0],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_09, r_pcc_coef1, GET_BITS(pstParams->matrix[1],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_09, r_pcc_coef2, GET_BITS(pstParams->matrix[2],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_0a, r_pcc_coef3, GET_BITS(pstParams->matrix[3],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_0a, r_pcc_coef4, GET_BITS(pstParams->matrix[4],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_0b, r_pcc_coef5, GET_BITS(pstParams->matrix[5],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_0b, r_pcc_coef6, GET_BITS(pstParams->matrix[6],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_0c, r_pcc_coef7, GET_BITS(pstParams->matrix[7],0,15));
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_0c, r_pcc_coef8, GET_BITS(pstParams->matrix[8],0,15));
				DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_0c);
				DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_0b);
				DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_0a);
				DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_09);
				DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_08);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_08);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_09);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0a);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0b);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0c);
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_08, r_pcc_coef0, GET_BITS(pstParams->matrix[0],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_09, r_pcc_coef1, GET_BITS(pstParams->matrix[1],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_09, r_pcc_coef2, GET_BITS(pstParams->matrix[2],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_0a, r_pcc_coef3, GET_BITS(pstParams->matrix[3],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_0a, r_pcc_coef4, GET_BITS(pstParams->matrix[4],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_0b, r_pcc_coef5, GET_BITS(pstParams->matrix[5],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_0b, r_pcc_coef6, GET_BITS(pstParams->matrix[6],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_0c, r_pcc_coef7, GET_BITS(pstParams->matrix[7],0,15));
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_0c, r_pcc_coef8, GET_BITS(pstParams->matrix[8],0,15));
				DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_0c);
				DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_0b);
				DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_0a);
				DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_09);
				DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_08);
			}
		}
	} while (0);

	return ret;
}
static int PE_CSC_GetColorGamut(LX_PE_CSC_GAMUT_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_08);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_09);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0a);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0b);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0c);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_08, r_pcc_coef0, pstParams->matrix[0]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_09, r_pcc_coef1, pstParams->matrix[1]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_09, r_pcc_coef2, pstParams->matrix[2]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_0a, r_pcc_coef3, pstParams->matrix[3]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_0a, r_pcc_coef4, pstParams->matrix[4]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_0b, r_pcc_coef5, pstParams->matrix[5]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_0b, r_pcc_coef6, pstParams->matrix[6]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_0c, r_pcc_coef7, pstParams->matrix[7]);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_0c, r_pcc_coef8, pstParams->matrix[8]);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_08);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_09);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0a);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0b);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0c);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_08, r_pcc_coef0, pstParams->matrix[0]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_09, r_pcc_coef1, pstParams->matrix[1]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_09, r_pcc_coef2, pstParams->matrix[2]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_0a, r_pcc_coef3, pstParams->matrix[3]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_0a, r_pcc_coef4, pstParams->matrix[4]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_0b, r_pcc_coef5, pstParams->matrix[5]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_0b, r_pcc_coef6, pstParams->matrix[6]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_0c, r_pcc_coef7, pstParams->matrix[7]);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_0c, r_pcc_coef8, pstParams->matrix[8]);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_08);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_09);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0a);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0b);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0c);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_08, r_pcc_coef0, pstParams->matrix[0]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_09, r_pcc_coef1, pstParams->matrix[1]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_09, r_pcc_coef2, pstParams->matrix[2]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_0a, r_pcc_coef3, pstParams->matrix[3]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_0a, r_pcc_coef4, pstParams->matrix[4]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_0b, r_pcc_coef5, pstParams->matrix[5]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_0b, r_pcc_coef6, pstParams->matrix[6]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_0c, r_pcc_coef7, pstParams->matrix[7]);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_0c, r_pcc_coef8, pstParams->matrix[8]);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_08);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_09);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0a);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0b);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0c);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_08, r_pcc_coef0, pstParams->matrix[0]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_09, r_pcc_coef1, pstParams->matrix[1]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_09, r_pcc_coef2, pstParams->matrix[2]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_0a, r_pcc_coef3, pstParams->matrix[3]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_0a, r_pcc_coef4, pstParams->matrix[4]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_0b, r_pcc_coef5, pstParams->matrix[5]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_0b, r_pcc_coef6, pstParams->matrix[6]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_0c, r_pcc_coef7, pstParams->matrix[7]);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_0c, r_pcc_coef8, pstParams->matrix[8]);
			}
		}

		PE_CSC_DBG_PRINT("get pstParams[%d] : 0x%x,0x%x,0x%x, 0x%x,0x%x,0x%x, 0x%x,0x%x,0x%x\n",\
			pstParams->win_id,pstParams->matrix[0],pstParams->matrix[1],pstParams->matrix[2],pstParams->matrix[3],\
			pstParams->matrix[4],pstParams->matrix[5],pstParams->matrix[6],pstParams->matrix[7],pstParams->matrix[8]);
	} while (0);
	return ret;
}
static int PE_CSC_SetxvYCC(LX_PE_CSC_XVYCC_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CSC_DBG_PRINT("set pstParams[%d] : en:%d, sclr:0x%x\n",\
			pstParams->win_id,pstParams->enable,pstParams->scaler);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_00);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0d);
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_00, scaler_en,	(pstParams->enable)? 0x1:0x0);
				DE_P1L_L9B0_Wr01(pe1_l_xvy_ctrl_0d, scaler,		GET_BITS(pstParams->scaler,0,8));
				DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_00);
				DE_P1L_L9B0_WrFL(pe1_l_xvy_ctrl_0d);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_00);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0d);
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_00, scaler_en,	(pstParams->enable)? 0x1:0x0);
				DE_P1R_L9B0_Wr01(pe1_r_xvy_ctrl_0d, scaler,		GET_BITS(pstParams->scaler,0,8));
				DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_00);
				DE_P1R_L9B0_WrFL(pe1_r_xvy_ctrl_0d);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_00);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0d);
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_00, scaler_en,	(pstParams->enable)? 0x1:0x0);
				DE_P1L_L9_Wr01(pe1_l_xvy_ctrl_0d, scaler,		GET_BITS(pstParams->scaler,0,8));
				DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_00);
				DE_P1L_L9_WrFL(pe1_l_xvy_ctrl_0d);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_00);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0d);
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_00, scaler_en,	(pstParams->enable)? 0x1:0x0);
				DE_P1R_L9_Wr01(pe1_r_xvy_ctrl_0d, scaler,		GET_BITS(pstParams->scaler,0,8));
				DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_00);
				DE_P1R_L9_WrFL(pe1_r_xvy_ctrl_0d);
			}
		}
	} while (0);

	return ret;
}
static int PE_CSC_GetxvYCC(LX_PE_CSC_XVYCC_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_00);
				DE_P1L_L9B0_RdFL(pe1_l_xvy_ctrl_0d);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_00, scaler_en,	pstParams->enable);
				DE_P1L_L9B0_Rd01(pe1_l_xvy_ctrl_0d, scaler,		pstParams->scaler);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_00);
				DE_P1R_L9B0_RdFL(pe1_r_xvy_ctrl_0d);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_00, scaler_en,	pstParams->enable);
				DE_P1R_L9B0_Rd01(pe1_r_xvy_ctrl_0d, scaler,		pstParams->scaler);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_00);
				DE_P1L_L9_RdFL(pe1_l_xvy_ctrl_0d);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_00, scaler_en,	pstParams->enable);
				DE_P1L_L9_Rd01(pe1_l_xvy_ctrl_0d, scaler,		pstParams->scaler);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_00);
				DE_P1R_L9_RdFL(pe1_r_xvy_ctrl_0d);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_00, scaler_en,	pstParams->enable);
				DE_P1R_L9_Rd01(pe1_r_xvy_ctrl_0d, scaler,		pstParams->scaler);
			}
		}

		PE_CSC_DBG_PRINT("get pstParams[%d] : en:%d, sclr:0x%x\n",\
			pstParams->win_id,pstParams->enable,pstParams->scaler);
	} while (0);

	return ret;
}

