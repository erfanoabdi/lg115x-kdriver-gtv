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

/** @file pe_cmg.c
 *
 *  driver for picture enhance color management functions. ( used only within kdriver )
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
#include "pe_cmg.h"
#include "pe_cmg_coeff.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_CMG_ERROR	printk

#define PE_CMG_DBG_PRINT(fmt,args...)	if(_g_cmg_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_CMG_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_CMG_ERROR(fmt,##args);_action;}}

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
static int PE_CMG_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_CMG_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_CMG_SetCenRegionEnable(LX_PE_CMG_REGION_ENABLE_T *pstParams);
static int PE_CMG_GetCenRegionEnable(LX_PE_CMG_REGION_ENABLE_T *pstParams);
static int PE_CMG_SetCenRegion(LX_PE_CMG_REGION_T *pstParams);
static int PE_CMG_GetCenRegion(LX_PE_CMG_REGION_T *pstParams);
static int PE_CMG_SetCenRegionCtrl(LX_PE_CMG_REGION_CTRL_T *pstParams);
static int PE_CMG_GetCenRegionCtrl(LX_PE_CMG_REGION_CTRL_T *pstParams);
static int PE_CMG_SetCenGlobalCtrl(LX_PE_CMG_GLOBAL_CTRL_T *pstParams);
static int PE_CMG_GetCenGlobalCtrl(LX_PE_CMG_GLOBAL_CTRL_T *pstParams);
static int PE_CMG_SetCenColorCtrl(LX_PE_CMG_COLOR_CTRL_T *pstParams);
static int PE_CMG_GetCenColorCtrl(LX_PE_CMG_COLOR_CTRL_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_cmg_trace=0x0;	//default should be off.

/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 * init color management
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CMG_Init(void)
{
	int ret = RET_OK;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_CMG_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		else
		{
			PE_CMG_DBG_PRINT("write cmg_l_init_param_l9a0. \n");
			ret = PE_REG_SetTable(cmg_l_init_param_l9a0,0,sizeof(cmg_l_init_param_l9a0));
			PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);		
			PE_CMG_DBG_PRINT("write cmg_r_init_param_l9a0. \n");
			ret = PE_REG_SetTable(cmg_r_init_param_l9a0,0,sizeof(cmg_r_init_param_l9a0));
			PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);		
		}
	}while(0);

	return ret;
}

/**
 * standard function of cmg setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CMG_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CMG_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_ENABLE_T):
		{
			LX_PE_CMG_REGION_ENABLE_T *pRcvData;
			LX_PE_CMG_REGION_ENABLE_T stParam;
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_REGION_ENABLE_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_REGION_ENABLE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_REGION_ENABLE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMG_SetCenRegionEnable(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_SetCenRegionEnable() error.\n",__F__,__L__);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_REGION_ENABLE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_T):
		{
			LX_PE_CMG_REGION_T *pRcvData;
			LX_PE_CMG_REGION_T stParam;
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_REGION_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_REGION_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_REGION_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMG_SetCenRegion(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_SetCenRegion() error.\n",__F__,__L__);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_REGION_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_CTRL_T):
		{
			LX_PE_CMG_REGION_CTRL_T *pRcvData;
			LX_PE_CMG_REGION_CTRL_T stParam;
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_REGION_CTRL_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_REGION_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_REGION_CTRL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMG_SetCenRegionCtrl(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_SetCenRegionCtrl() error.\n",__F__,__L__);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_REGION_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_GLOBAL_CTRL_T):
		{
			LX_PE_CMG_GLOBAL_CTRL_T *pRcvData;
			LX_PE_CMG_GLOBAL_CTRL_T stParam;
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_GLOBAL_CTRL_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_GLOBAL_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_GLOBAL_CTRL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMG_SetCenGlobalCtrl(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_SetCenGlobalCtrl() error.\n",__F__,__L__);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_GLOBAL_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_COLOR_CTRL_T):
		{
			LX_PE_CMG_COLOR_CTRL_T *pRcvData;
			LX_PE_CMG_COLOR_CTRL_T stParam;
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_COLOR_CTRL_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_COLOR_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_COLOR_CTRL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMG_SetCenColorCtrl(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_SetCenColorCtrl() error.\n",__F__,__L__);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_COLOR_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_CMG_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMG_SetDefaultSettings(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_SetDefaultSettings() error.\n",__F__,__L__);
				PE_CMG_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_CMG_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMG_SetDebugSettings(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_SetDebugSettings() error.\n",__F__,__L__);
				PE_CMG_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CMG_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of cmg getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CMG_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CMG_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_ENABLE_T):
		{
			LX_PE_CMG_REGION_ENABLE_T *pRcvData;
			LX_PE_CMG_REGION_ENABLE_T stParam;
			memset0(stParam);
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_REGION_ENABLE_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_REGION_ENABLE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_REGION_ENABLE_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMG_GetCenRegionEnable(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_GetCenRegionEnable() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_REGION_ENABLE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_T):
		{
			LX_PE_CMG_REGION_T *pRcvData;
			LX_PE_CMG_REGION_T stParam;
			memset0(stParam);
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_REGION_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_REGION_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_REGION_T *)data;
				stParam.win_id = pRcvData->win_id;
				stParam.region_num= pRcvData->region_num;
				ret = PE_CMG_GetCenRegion(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_GetCenRegion() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_REGION_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_CTRL_T):
		{
			LX_PE_CMG_REGION_CTRL_T *pRcvData;
			LX_PE_CMG_REGION_CTRL_T stParam;
			memset0(stParam);
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_REGION_CTRL_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_REGION_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_REGION_CTRL_T *)data;
				stParam.win_id = pRcvData->win_id;
				stParam.region_num= pRcvData->region_num;
				ret = PE_CMG_GetCenRegionCtrl(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_GetCenRegionCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_REGION_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_GLOBAL_CTRL_T):
		{
			LX_PE_CMG_GLOBAL_CTRL_T *pRcvData;
			LX_PE_CMG_GLOBAL_CTRL_T stParam;
			memset0(stParam);
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_GLOBAL_CTRL_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_GLOBAL_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_GLOBAL_CTRL_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMG_GetCenGlobalCtrl(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_GetCenGlobalCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_GLOBAL_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMG_COLOR_CTRL_T):
		{
			LX_PE_CMG_COLOR_CTRL_T *pRcvData;
			LX_PE_CMG_COLOR_CTRL_T stParam;
			memset0(stParam);
			do{
				PE_CMG_DBG_PRINT("S LX_PE_CMG_COLOR_CTRL_T\n");
				/* data size check */
				PE_CMG_CHECK_CODE(datasize!=sizeof(LX_PE_CMG_COLOR_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMG_COLOR_CTRL_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMG_GetCenColorCtrl(&stParam);
				PE_CMG_CHECK_CODE(ret,break,"[%s,%d] PE_CMG_GetCenColorCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMG_DBG_PRINT("E LX_PE_CMG_COLOR_CTRL_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CMG_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_CMG_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMG_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);
	}while(0);
	return ret;
}
static int PE_CMG_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMG_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(CMG)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_cmg_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(CMG)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(CMG)))
				{
					PE_CMG_DBG_PRINT("cen bypass.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_00,	reg_cen_bypass,	0x0);	//CEN block OFF
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_00,	reg_cen_bypass,	0x0);	//CEN block OFF
					}
				}
				else
				{
					PE_CMG_DBG_PRINT("cen on.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_00,	reg_cen_bypass,	0x1);	//CEN block ON
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_00,	reg_cen_bypass,	0x1);	//CEN block ON
					}
				}
			}
			else
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(CMG)))
				{
					PE_CMG_DBG_PRINT("pe1_l_cen_ctrl_01 : 0x0.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P1L_L9_RdFL(pe1_l_cen_ctrl_01);
						DE_P1L_L9_Wr(pe1_l_cen_ctrl_01,0x0);
						DE_P1L_L9_WrFL(pe1_l_cen_ctrl_01);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P1R_L9_RdFL(pe1_r_cen_ctrl_01);
						DE_P1R_L9_Wr(pe1_r_cen_ctrl_01,0x0);
						DE_P1R_L9_WrFL(pe1_r_cen_ctrl_01);
					}
				}
			}
		}
	}while(0);
	return ret;
}
static int PE_CMG_SetCenRegionEnable(LX_PE_CMG_REGION_ENABLE_T *pstParams)
{
	int ret = RET_OK;
	UINT32 wdata=0;
	UINT32 count=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);

		for(count=0;count<LX_PE_CMG_REGION_NUM;count++)
		{
			if(pstParams->enable[count])
				wdata |= (0x1<<(count+LX_PE_CMG_REGION_NUM));
			else
				wdata &= ~(0x1<<(count+LX_PE_CMG_REGION_NUM));

			if(pstParams->show_region[count])
				wdata |= (0x1<<(count));
			else
				wdata &= ~(0x1<<(count));
		}

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_01);
				DE_P1L_L9B0_Wr(pe1_l_cen_ctrl_01,wdata);
				DE_P1L_L9B0_WrFL(pe1_l_cen_ctrl_01);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_01);
				DE_P1R_L9B0_Wr(pe1_r_cen_ctrl_01,wdata);
				DE_P1R_L9B0_WrFL(pe1_r_cen_ctrl_01);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ctrl_01);
				DE_P1L_L9_Wr(pe1_l_cen_ctrl_01,wdata);
				DE_P1L_L9_WrFL(pe1_l_cen_ctrl_01);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ctrl_01);
				DE_P1R_L9_Wr(pe1_r_cen_ctrl_01,wdata);
				DE_P1R_L9_WrFL(pe1_r_cen_ctrl_01);
			}
		}
	} while (0);

	return ret;
}
static int PE_CMG_GetCenRegionEnable(LX_PE_CMG_REGION_ENABLE_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 rdata=0;
	UINT32 count=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_01);
				rdata = DE_P1L_L9B0_Rd(pe1_l_cen_ctrl_01);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_01);
				rdata = DE_P1R_L9B0_Rd(pe1_r_cen_ctrl_01);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ctrl_01);
				rdata = DE_P1L_L9_Rd(pe1_l_cen_ctrl_01);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ctrl_01);
				rdata = DE_P1R_L9_Rd(pe1_r_cen_ctrl_01);
			}
		}

		for(count=0;count<LX_PE_CMG_REGION_NUM;count++)
		{
			if(rdata & (0x1<<(count+LX_PE_CMG_REGION_NUM)))
				pstParams->enable[count] = 1;
			else
				pstParams->enable[count] = 0;

			if(rdata & (0x1<<(count)))
				pstParams->show_region[count] = 1;
			else
				pstParams->show_region[count] = 0;
		}
	} while (0);

	return ret;
}
static int PE_CMG_SetCenRegion(LX_PE_CMG_REGION_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count=0;
	UINT32 startAddr=0;
	UINT32 x_wdata,y_wdata;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMG_CHECK_CODE(pstParams->region_num>LX_PE_CMG_REGION_MAX,ret=RET_ERROR;break,\
			"[%s,%d] region_num(%d) is out of range.\n",__F__,__L__,pstParams->region_num);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				startAddr = pstParams->region_num*LX_PE_CMG_TBLPOINT;
				/* H color region table : 000 */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					/* hue_x,x_wdata : 0~1024(0~720 degree) */
					x_wdata = (UINT32)GET_BITS(pstParams->hue_x[count],0,10);
					y_wdata = GET_BITS(pstParams->hue_g[count],0,7);	// 0~127

					DE_P1L_L9B0_Wr01(pe1_l_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1L_L9B0_Wr01(pe1_l_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1L_L9B0_WrFL(pe1_l_cen_ia_data);
				}
				/* S color region table : 001 */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->sat_x[count],0,7);	// 0~100
					y_wdata = GET_BITS(pstParams->sat_g[count],0,7);	// 0~127

					DE_P1L_L9B0_Wr01(pe1_l_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1L_L9B0_Wr01(pe1_l_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1L_L9B0_WrFL(pe1_l_cen_ia_data);
				}
				/* V color region table : 010 */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->val_x[count],0,8);	// 0~255
					y_wdata = GET_BITS(pstParams->val_g[count],0,7);	// 0~127

					DE_P1L_L9B0_Wr01(pe1_l_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1L_L9B0_Wr01(pe1_l_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1L_L9B0_WrFL(pe1_l_cen_ia_data);
				}
				/* show color region table : 011 */
				startAddr = pstParams->region_num;
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x3);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9B0_Wr(pe1_l_cen_ia_data,	0x0);
					DE_P1L_L9B0_WrFL(pe1_l_cen_ia_data);
				}
				/* normal operation */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				startAddr = pstParams->region_num*LX_PE_CMG_TBLPOINT;
				/* H color region table : 000 */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					/* hue_x,x_wdata : 0~1024(0~720 degree) */
					x_wdata = (UINT32)GET_BITS(pstParams->hue_x[count],0,10);
					y_wdata = GET_BITS(pstParams->hue_g[count],0,7);	// 0~127

					DE_P1R_L9B0_Wr01(pe1_r_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1R_L9B0_Wr01(pe1_r_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1R_L9B0_WrFL(pe1_r_cen_ia_data);
				}
				/* S color region table : 001 */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->sat_x[count],0,7);	// 0~100
					y_wdata = GET_BITS(pstParams->sat_g[count],0,7);	// 0~127

					DE_P1R_L9B0_Wr01(pe1_r_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1R_L9B0_Wr01(pe1_r_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1R_L9B0_WrFL(pe1_r_cen_ia_data);
				}
				/* V color region table : 010 */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->val_x[count],0,8);	// 0~255
					y_wdata = GET_BITS(pstParams->val_g[count],0,7);	// 0~127

					DE_P1R_L9B0_Wr01(pe1_r_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1R_L9B0_Wr01(pe1_r_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1R_L9B0_WrFL(pe1_r_cen_ia_data);
				}
				/* show color region table : 011 */
				startAddr = pstParams->region_num;
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x3);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9B0_Wr(pe1_r_cen_ia_data,	0x0);
					DE_P1R_L9B0_WrFL(pe1_r_cen_ia_data);
				}
				/* normal operation */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				startAddr = pstParams->region_num*LX_PE_CMG_TBLPOINT;
				/* H color region table : 000 */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					/* hue_x,x_wdata : 0~1024(0~720 degree) */
					x_wdata = (UINT32)GET_BITS(pstParams->hue_x[count],0,10);
					y_wdata = GET_BITS(pstParams->hue_g[count],0,7);	// 0~127

					DE_P1L_L9_Wr01(pe1_l_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1L_L9_Wr01(pe1_l_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1L_L9_WrFL(pe1_l_cen_ia_data);
				}
				/* S color region table : 001 */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->sat_x[count],0,7);	// 0~100
					y_wdata = GET_BITS(pstParams->sat_g[count],0,7);	// 0~127

					DE_P1L_L9_Wr01(pe1_l_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1L_L9_Wr01(pe1_l_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1L_L9_WrFL(pe1_l_cen_ia_data);
				}
				/* V color region table : 010 */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->val_x[count],0,8);	// 0~255
					y_wdata = GET_BITS(pstParams->val_g[count],0,7);	// 0~127

					DE_P1L_L9_Wr01(pe1_l_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1L_L9_Wr01(pe1_l_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1L_L9_WrFL(pe1_l_cen_ia_data);
				}
				/* show color region table : 011 */
				startAddr = pstParams->region_num;
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x3);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9_Wr(pe1_l_cen_ia_data,	0);
					DE_P1L_L9_WrFL(pe1_l_cen_ia_data);
				}
				/* normal operation */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				startAddr = pstParams->region_num*LX_PE_CMG_TBLPOINT;
				/* H color region table : 000 */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					/* hue_x,x_wdata : 0~1024(0~720 degree) */
					x_wdata = (UINT32)GET_BITS(pstParams->hue_x[count],0,10);
					y_wdata = GET_BITS(pstParams->hue_g[count],0,7);	// 0~127

					DE_P1R_L9_Wr01(pe1_r_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1R_L9_Wr01(pe1_r_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1R_L9_WrFL(pe1_r_cen_ia_data);
				}
				/* S color region table : 001 */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->sat_x[count],0,7);	// 0~100
					y_wdata = GET_BITS(pstParams->sat_g[count],0,7);	// 0~127

					DE_P1R_L9_Wr01(pe1_r_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1R_L9_Wr01(pe1_r_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1R_L9_WrFL(pe1_r_cen_ia_data);
				}
				/* V color region table : 010 */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					x_wdata = GET_BITS(pstParams->val_x[count],0,8);	// 0~255
					y_wdata = GET_BITS(pstParams->val_g[count],0,7);	// 0~127

					DE_P1R_L9_Wr01(pe1_r_cen_ia_data,	hif_cen_x_wdata, 	x_wdata);	//[25:16] x data
					DE_P1R_L9_Wr01(pe1_r_cen_ia_data,	hif_cen_y_wdata, 	y_wdata);	//[9:0] y data
					DE_P1R_L9_WrFL(pe1_r_cen_ia_data);
				}
				/* show color region table : 011 */
				startAddr = pstParams->region_num;
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x3);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9_Wr(pe1_r_cen_ia_data,	0);
					DE_P1R_L9_WrFL(pe1_r_cen_ia_data);
				}
				/* normal operation */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
	} while (0);

	return ret;
}
static int PE_CMG_GetCenRegion(LX_PE_CMG_REGION_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 count=0;
	UINT32 startAddr=0;
	UINT32 x_wdata=0,y_wdata=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMG_CHECK_CODE(pstParams->region_num>LX_PE_CMG_REGION_MAX,ret=RET_ERROR;break,\
			"[%s,%d] region_num(%d) is out of range.\n",__F__,__L__,pstParams->region_num);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		startAddr = pstParams->region_num*LX_PE_CMG_TBLPOINT;

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				/* H color region table : 000 */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
					DE_P1L_L9B0_Rd01(pe1_l_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1L_L9B0_Rd01(pe1_l_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					/* x_wdata,hue_x 0~1024 (0~720 degree) */
					pstParams->hue_x[count] = (UINT16)GET_BITS(x_wdata,0,10);
					pstParams->hue_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* S color region table : 001 */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
					DE_P1L_L9B0_Rd01(pe1_l_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1L_L9B0_Rd01(pe1_l_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->sat_x[count] = (UINT8)GET_BITS(x_wdata,0,7);	// 0~100
					pstParams->sat_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* V color region table : 010 */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
					DE_P1L_L9B0_Rd01(pe1_l_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1L_L9B0_Rd01(pe1_l_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->val_x[count] = (UINT8)GET_BITS(x_wdata,0,8);	// 0~255
					pstParams->val_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* normal operation */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				/* H color region table : 000 */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
		
				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
					DE_P1R_L9B0_Rd01(pe1_r_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1R_L9B0_Rd01(pe1_r_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					/* x_wdata,hue_x 0~1024 (0~720 degree) */
					pstParams->hue_x[count] = (UINT16)GET_BITS(x_wdata,0,10);
					pstParams->hue_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* S color region table : 001 */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
					DE_P1R_L9B0_Rd01(pe1_r_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1R_L9B0_Rd01(pe1_r_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->sat_x[count] = (UINT8)GET_BITS(x_wdata,0,7);	// 0~100
					pstParams->sat_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* V color region table : 010 */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
					DE_P1R_L9B0_Rd01(pe1_r_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1R_L9B0_Rd01(pe1_r_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->val_x[count] = (UINT8)GET_BITS(x_wdata,0,8);	// 0~255
					pstParams->val_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* normal operation */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				/* H color region table : 000 */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
					DE_P1L_L9_Rd01(pe1_l_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1L_L9_Rd01(pe1_l_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					/* x_wdata,hue_x 0~1024 (0~720 degree) */
					pstParams->hue_x[count] = (UINT16)GET_BITS(x_wdata,0,10);
					pstParams->hue_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* S color region table : 001 */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
					DE_P1L_L9_Rd01(pe1_l_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1L_L9_Rd01(pe1_l_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->sat_x[count] = (UINT8)GET_BITS(x_wdata,0,7);	// 0~100
					pstParams->sat_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* V color region table : 010 */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
					DE_P1L_L9_Rd01(pe1_l_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1L_L9_Rd01(pe1_l_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->val_x[count] = (UINT8)GET_BITS(x_wdata,0,8);	// 0~255
					pstParams->val_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* normal operation */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				/* H color region table : 000 */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
		
				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
					DE_P1R_L9_Rd01(pe1_r_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1R_L9_Rd01(pe1_r_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					/* x_wdata,hue_x 0~1024 (0~720 degree) */
					pstParams->hue_x[count] = (UINT16)GET_BITS(x_wdata,0,10);
					pstParams->hue_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* S color region table : 001 */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x1);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
					DE_P1R_L9_Rd01(pe1_r_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1R_L9_Rd01(pe1_r_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->sat_x[count] = (UINT8)GET_BITS(x_wdata,0,7);	// 0~100
					pstParams->sat_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* V color region table : 010 */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x2);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_TBLPOINT;count++)
				{
					DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
					DE_P1R_L9_Rd01(pe1_r_cen_ia_data,	hif_cen_x_wdata,	x_wdata);	//[25:16] x data
					DE_P1R_L9_Rd01(pe1_r_cen_ia_data,	hif_cen_y_wdata,	y_wdata);	//[9:0] y data

					pstParams->val_x[count] = (UINT8)GET_BITS(x_wdata,0,8);	// 0~255
					pstParams->val_g[count] = (UINT8)GET_BITS(y_wdata,0,7);	// 0~127
				}
				/* normal operation */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
	} while (0);

	return ret;
}
static int PE_CMG_SetCenRegionCtrl(LX_PE_CMG_REGION_CTRL_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count=0;
	UINT32 startAddr=0;
	UINT32 delta[3];		// -128 ~ 127, hsv or gbr
	UINT32 wdata=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMG_CHECK_CODE(pstParams->region_num>LX_PE_CMG_REGION_MAX,ret=RET_ERROR;break,\
			"[%s,%d] region_num(%d) is out of range.\n",__F__,__L__,pstParams->region_num);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;
		
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					wdata=0;
					delta[0] = GET_BITS(pstParams->region_delta[count*3],0,8);		// -128 ~ 127
					delta[1] = GET_BITS(pstParams->region_delta[count*3+1],0,8);	// -128 ~ 127
					delta[2] = GET_BITS(pstParams->region_delta[count*3+2],0,8);	// -128 ~ 127
					wdata = (delta[0]<<16)|(delta[1]<<8)|(delta[2]);
					DE_P1L_L9B0_Wr(pe1_l_cen_ia_data,	wdata);
					DE_P1L_L9B0_WrFL(pe1_l_cen_ia_data);
				}
				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);		//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				wdata = GET_BITS(pstParams->master_gain,0,8);	// 0~128~255
				DE_P1L_L9B0_Wr(pe1_l_cen_ia_data,	wdata);
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_data);
				/* normal operation */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;

				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					wdata=0;
					delta[0] = GET_BITS(pstParams->region_delta[count*3],0,8);		// -128 ~ 127
					delta[1] = GET_BITS(pstParams->region_delta[count*3+1],0,8);	// -128 ~ 127
					delta[2] = GET_BITS(pstParams->region_delta[count*3+2],0,8);	// -128 ~ 127
					wdata = (delta[0]<<16)|(delta[1]<<8)|(delta[2]);
					DE_P1R_L9B0_Wr(pe1_r_cen_ia_data,	wdata);
					DE_P1R_L9B0_WrFL(pe1_r_cen_ia_data);
				}
				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);		//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				wdata = GET_BITS(pstParams->master_gain,0,8);	// 0~128~255
				DE_P1R_L9B0_Wr(pe1_r_cen_ia_data,	wdata);
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_data);
				/* normal operation */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;
		
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					wdata=0;
					delta[0] = GET_BITS(pstParams->region_delta[count*3],0,8);		// -128 ~ 127
					delta[1] = GET_BITS(pstParams->region_delta[count*3+1],0,8);	// -128 ~ 127
					delta[2] = GET_BITS(pstParams->region_delta[count*3+2],0,8);	// -128 ~ 127
					wdata = (delta[0]<<16)|(delta[1]<<8)|(delta[2]);
					DE_P1L_L9_Wr(pe1_l_cen_ia_data,	wdata);
					DE_P1L_L9_WrFL(pe1_l_cen_ia_data);
				}
				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);		//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				wdata = GET_BITS(pstParams->master_gain,0,8);	// 0~128~255
				DE_P1L_L9_Wr(pe1_l_cen_ia_data,	wdata);
				DE_P1L_L9_WrFL(pe1_l_cen_ia_data);
				/* normal operation */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;

				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					wdata=0;
					delta[0] = GET_BITS(pstParams->region_delta[count*3],0,8);		// -128 ~ 127
					delta[1] = GET_BITS(pstParams->region_delta[count*3+1],0,8);	// -128 ~ 127
					delta[2] = GET_BITS(pstParams->region_delta[count*3+2],0,8);	// -128 ~ 127
					wdata = (delta[0]<<16)|(delta[1]<<8)|(delta[2]);
					DE_P1R_L9_Wr(pe1_r_cen_ia_data,	wdata);
					DE_P1R_L9_WrFL(pe1_r_cen_ia_data);
				}
				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);		//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				wdata = GET_BITS(pstParams->master_gain,0,8);	// 0~128~255
				DE_P1R_L9_Wr(pe1_r_cen_ia_data,	wdata);
				DE_P1R_L9_WrFL(pe1_r_cen_ia_data);
				/* normal operation */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
			}
		}		
	} while (0);

	return ret;
}
static int PE_CMG_GetCenRegionCtrl(LX_PE_CMG_REGION_CTRL_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 count=0;
	UINT32 startAddr=0;
	UINT32 reMgain=0;
	UINT32 reDgain[LX_PE_CMG_DELTA_SETNUM];	// hsv or gbr

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[4:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
				reMgain = DE_P1L_L9B0_Rd(pe1_l_cen_ia_data);
				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;

				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
					reDgain[count] = DE_P1L_L9B0_Rd(pe1_l_cen_ia_data);
				}
				pstParams->master_gain = (UINT8)GET_BITS(reMgain,0,8);	// region master gain, 0~128~255
				pstParams->region_delta[0] = (SINT8)GET_BITS(reDgain[0],16,8);	// h, -128 ~ 127
				pstParams->region_delta[1] = (SINT8)GET_BITS(reDgain[0],8,8);	// s, -128 ~ 127
				pstParams->region_delta[2] = (SINT8)GET_BITS(reDgain[0],0,8);	// v, -128 ~ 127
				pstParams->region_delta[3] = (SINT8)GET_BITS(reDgain[1],16,8);	// g, -128 ~ 127
				pstParams->region_delta[4] = (SINT8)GET_BITS(reDgain[1],8,8);	// b, -128 ~ 127
				pstParams->region_delta[5] = (SINT8)GET_BITS(reDgain[1],0,8);	// r, -128 ~ 127
				/* normal operation */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[4:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
				reMgain = DE_P1R_L9B0_Rd(pe1_r_cen_ia_data);
				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;

				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
					reDgain[count] = DE_P1R_L9B0_Rd(pe1_r_cen_ia_data);
				}
				pstParams->master_gain = (UINT8)GET_BITS(reMgain,0,8);	// region master gain, 0~128~255
				pstParams->region_delta[0] = (SINT8)GET_BITS(reDgain[0],16,8);	// h, -128 ~ 127
				pstParams->region_delta[1] = (SINT8)GET_BITS(reDgain[0],8,8);	// s, -128 ~ 127
				pstParams->region_delta[2] = (SINT8)GET_BITS(reDgain[0],0,8);	// v, -128 ~ 127
				pstParams->region_delta[3] = (SINT8)GET_BITS(reDgain[1],16,8);	// g, -128 ~ 127
				pstParams->region_delta[4] = (SINT8)GET_BITS(reDgain[1],8,8);	// b, -128 ~ 127
				pstParams->region_delta[5] = (SINT8)GET_BITS(reDgain[1],0,8);	// r, -128 ~ 127
				/* normal operation */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[4:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
				reMgain = DE_P1L_L9_Rd(pe1_l_cen_ia_data);
				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;

				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
					reDgain[count] = DE_P1L_L9_Rd(pe1_l_cen_ia_data);
				}
				pstParams->master_gain = (UINT8)GET_BITS(reMgain,0,8);	// region master gain, 0~128~255
				pstParams->region_delta[0] = (SINT8)GET_BITS(reDgain[0],16,8);	// h, -128 ~ 127
				pstParams->region_delta[1] = (SINT8)GET_BITS(reDgain[0],8,8);	// s, -128 ~ 127
				pstParams->region_delta[2] = (SINT8)GET_BITS(reDgain[0],0,8);	// v, -128 ~ 127
				pstParams->region_delta[3] = (SINT8)GET_BITS(reDgain[1],16,8);	// g, -128 ~ 127
				pstParams->region_delta[4] = (SINT8)GET_BITS(reDgain[1],8,8);	// b, -128 ~ 127
				pstParams->region_delta[5] = (SINT8)GET_BITS(reDgain[1],0,8);	// r, -128 ~ 127
				/* normal operation */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[4:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
				/* region master gain : 101 */
				startAddr = pstParams->region_num;

				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x5);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
				reMgain = DE_P1R_L9_Rd(pe1_r_cen_ia_data);
				/* region delta gain : 100 */
				startAddr = pstParams->region_num*LX_PE_CMG_DELTA_SETNUM;

				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x4);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	startAddr); //[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTA_SETNUM;count++)
				{
					DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
					reDgain[count] = DE_P1R_L9_Rd(pe1_r_cen_ia_data);
				}
				pstParams->master_gain = (UINT8)GET_BITS(reMgain,0,8);	// region master gain, 0~128~255
				pstParams->region_delta[0] = (SINT8)GET_BITS(reDgain[0],16,8);	// h, -128 ~ 127
				pstParams->region_delta[1] = (SINT8)GET_BITS(reDgain[0],8,8);	// s, -128 ~ 127
				pstParams->region_delta[2] = (SINT8)GET_BITS(reDgain[0],0,8);	// v, -128 ~ 127
				pstParams->region_delta[3] = (SINT8)GET_BITS(reDgain[1],16,8);	// g, -128 ~ 127
				pstParams->region_delta[4] = (SINT8)GET_BITS(reDgain[1],8,8);	// b, -128 ~ 127
				pstParams->region_delta[5] = (SINT8)GET_BITS(reDgain[1],0,8);	// r, -128 ~ 127
				/* normal operation */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
	} while (0);
	return ret;
}
static int PE_CMG_SetCenGlobalCtrl(LX_PE_CMG_GLOBAL_CTRL_T *pstParams)
{
	int ret = RET_OK;
	UINT32 count=0;
	UINT32 wdata=0;	// h,s,v,g,b,r

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
				/* global master gain : 110 */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					wdata = GET_BITS(pstParams->global_delta[count],0,10);		// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
					DE_P1L_L9B0_Wr(pe1_l_cen_ia_data,	wdata);
					DE_P1L_L9B0_WrFL(pe1_l_cen_ia_data);
				}
				/* normal operation */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
				/* global master gain : 110 */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					wdata = GET_BITS(pstParams->global_delta[count],0,10);		// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
					DE_P1R_L9B0_Wr(pe1_r_cen_ia_data,	wdata);
					DE_P1R_L9B0_WrFL(pe1_r_cen_ia_data);
				}
				/* normal operation */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
				/* global master gain : 110 for L9A0*/
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					wdata = GET_BITS(pstParams->global_delta[count],0,10);		// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
					DE_P1L_L9_Wr(pe1_l_cen_ia_data,	wdata);
					DE_P1L_L9_WrFL(pe1_l_cen_ia_data);
				}
				/* normal operation */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
				/* global master gain : 110 for L9A0*/
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					wdata = GET_BITS(pstParams->global_delta[count],0,10);		// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
					DE_P1R_L9_Wr(pe1_r_cen_ia_data,	wdata);
					DE_P1R_L9_WrFL(pe1_r_cen_ia_data);
				}
				/* normal operation */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
	} while (0);
	return ret;
}
static int PE_CMG_GetCenGlobalCtrl(LX_PE_CMG_GLOBAL_CTRL_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 count=0;
	UINT32 rdata=0;	// h,s,v,g,b,r

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
				/* global master gain : 110 for L9A0*/
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					DE_P1L_L9B0_RdFL(pe1_l_cen_ia_data);
					rdata = DE_P1L_L9B0_Rd(pe1_l_cen_ia_data);
					pstParams->global_delta[count] = (SINT16)PE_CONVHEX2DEC(rdata,9);// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
				}
				/* normal operation */
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9B0_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9B0_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
				/* global master gain : 110 for L9A0*/
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					DE_P1R_L9B0_RdFL(pe1_r_cen_ia_data);
					rdata = DE_P1R_L9B0_Rd(pe1_r_cen_ia_data);
					pstParams->global_delta[count] = (SINT16)PE_CONVHEX2DEC(rdata,9);// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
				}
				/* normal operation */
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9B0_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9B0_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ia_ctrl);
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
				/* global master gain : 110 for L9A0*/
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					DE_P1L_L9_RdFL(pe1_l_cen_ia_data);
					rdata = DE_P1L_L9_Rd(pe1_l_cen_ia_data);
					pstParams->global_delta[count] = (SINT16)PE_CONVHEX2DEC(rdata,9);// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
				}
				/* normal operation */
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1L_L9_Wr01(pe1_l_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1L_L9_WrFL(pe1_l_cen_ia_ctrl);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ia_ctrl);
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x0);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x1);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
				/* global master gain : 110 for L9A0*/
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x6);	//[10:8]
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);

				for(count=0;count<LX_PE_CMG_DELTANUM;count++)
				{
					DE_P1R_L9_RdFL(pe1_r_cen_ia_data);
					rdata = DE_P1R_L9_Rd(pe1_r_cen_ia_data);
					pstParams->global_delta[count] = (SINT16)PE_CONVHEX2DEC(rdata,9);// -512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
				}
				/* normal operation */
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_enable, 	0x1);	//[15] 0:host access, 1:normal mode
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai, 		0x0);	//[12] ai 0:disable, 1:enable
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_ai_sel, 	0x0);	//[10:8]
				DE_P1R_L9_Wr01(pe1_r_cen_ia_ctrl,	hif_cen_address,	0x0);	//[7:0] address
				DE_P1R_L9_WrFL(pe1_r_cen_ia_ctrl);
			}
		}
	} while (0);
	return ret;
}
static int PE_CMG_SetCenColorCtrl(LX_PE_CMG_COLOR_CTRL_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMG_DBG_PRINT("set pstParams[%d] : sat:%d\n",pstParams->win_id,pstParams->saturation);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_02);	
				DE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_02, ihsv_sgain,	GET_BITS(pstParams->saturation,0,8));	//[7:0] reg_ihsv_sgain
				DE_P1L_L9B0_WrFL(pe1_l_cen_ctrl_02);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_02);	
				DE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_02, ihsv_sgain,	GET_BITS(pstParams->saturation,0,8));	//[7:0] reg_ihsv_sgain
				DE_P1R_L9B0_WrFL(pe1_r_cen_ctrl_02);
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ctrl_02);	
				DE_P1L_L9_Wr01(pe1_l_cen_ctrl_02, ihsv_sgain,	GET_BITS(pstParams->saturation,0,8));	//[7:0] reg_ihsv_sgain
				DE_P1L_L9_WrFL(pe1_l_cen_ctrl_02);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ctrl_02);	
				DE_P1R_L9_Wr01(pe1_r_cen_ctrl_02, ihsv_sgain,	GET_BITS(pstParams->saturation,0,8));	//[7:0] reg_ihsv_sgain
				DE_P1R_L9_WrFL(pe1_r_cen_ctrl_02);
			}
		}
	} while (0);
	return ret;
}
static int PE_CMG_GetCenColorCtrl(LX_PE_CMG_COLOR_CTRL_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	UINT32 rdata=0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_02);
				DE_P1L_L9B0_Rd01(pe1_l_cen_ctrl_02, ihsv_sgain,	rdata);	//[7:0] reg_ihsv_sgain
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_02);
				DE_P1R_L9B0_Rd01(pe1_r_cen_ctrl_02, ihsv_sgain,	rdata);	//[7:0] reg_ihsv_sgain
			}
			pstParams->saturation=(UINT16)rdata;
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P1L_L9_RdFL(pe1_l_cen_ctrl_02);
				DE_P1L_L9_Rd01(pe1_l_cen_ctrl_02, ihsv_sgain,	rdata);	//[7:0] reg_ihsv_sgain
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P1R_L9_RdFL(pe1_r_cen_ctrl_02);
				DE_P1R_L9_Rd01(pe1_r_cen_ctrl_02, ihsv_sgain,	rdata);	//[7:0] reg_ihsv_sgain
			}
			pstParams->saturation=(UINT16)rdata;
		}
		PE_CMG_DBG_PRINT("get pstParams[%d] : sat:%d\n",pstParams->win_id,pstParams->saturation);
	} while (0);
	return ret;
}

