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

/** @file pe_nrd.c
 *
 *  driver for picture enhance noise reduction functions. ( used only within kdriver )
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
#include "pe_nrd.h"
#include "pe_nrd_coeff.h"

#include "pe_etc.h"
#include "pe_spd.h"
#include "pe_fwi.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_NRD_ERROR	printk

#define PE_NRD_DBG_PRINT(fmt,args...)	if(_g_nrd_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_NRD_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_NRD_ERROR(fmt,##args);_action;}}

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
static int PE_NRD_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_NRD_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_NRD_SetDnrCtrl(LX_PE_NRD_DNR_T *pstParams);
static int PE_NRD_GetDnrCtrl(LX_PE_NRD_DNR_T *pstParams);
static int PE_NRD_SetTnrCtrl(LX_PE_NRD_TNR_T *pstParams);
static int PE_NRD_GetTnrCtrl(LX_PE_NRD_TNR_T *pstParams);
static int PE_NRD_SetDnrCmnCtrl(LX_PE_NRD_DNR_CMN_T *pstParams);
static int PE_NRD_GetDnrCmnCtrl(LX_PE_NRD_DNR_CMN_T *pstParams);
static int PE_NRD_SetDnrDetailCtrl(LX_PE_NRD_DNR_DETAIL_T *pstParams);
static int PE_NRD_GetDnrDetailCtrl(LX_PE_NRD_DNR_DETAIL_T *pstParams);
static int PE_NRD_SetTnrCmnCtrl(LX_PE_NRD_TNR_CMN_T *pstParams);
static int PE_NRD_GetTnrCmnCtrl(LX_PE_NRD_TNR_CMN_T *pstParams);
static int PE_NRD_SetTnrDetailCtrl(LX_PE_NRD_TNR_DETAIL_T *pstParams);
static int PE_NRD_GetTnrDetailCtrl(LX_PE_NRD_TNR_DETAIL_T *pstParams);
static int PE_NRD_SetTnrColorRegionCtrl(LX_PE_NRD_TNR_CRGN_T *pstParams);
static int PE_NRD_GetTnrColorRegionCtrl(LX_PE_NRD_TNR_CRGN_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_nrd_trace=0x0;	//default should be off.

static UINT8 _g_nrd_auto_mode[LX_PE_WIN_NUM];

/*----------------------------------------------------------------------------------------
	Implementation Group
----------------------------------------------------------------------------------------*/

/**
 * init nrd init
 * 	- l9 tnr alpha lut loading sequence
 *		: load time on -> tnr off -> loading alpha lut -> tnr on -> load time off
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_NRD_Init(void)
{
	int ret = RET_OK;
	UINT32 fw_msg;
	UINT8 i;
	
	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_P0L_L9B0_Wr01(pe0_load,	load_time,	0x1);
			PE_P0R_L9B0_Wr01(pe0_load,	load_time,	0x1);
			fw_msg = FALSE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);

			PE_SET_REG_TABLE(NRD,nrd_l_init_param_l9b0,0);
			PE_SET_REG_TABLE(NRD,nrd_r_init_param_l9b0,0);
			PE_SET_REG_TABLE(NRD,nrd_l_tnr_alpha_lut_init_param_l9b0,0);
			PE_SET_REG_TABLE(NRD,nrd_r_tnr_alpha_lut_init_param_l9b0,0);
			PE_SET_REG_TABLE(NRD,nrd_l_tnr_crg_lut_init_param_l9b0,0);
			PE_SET_REG_TABLE(NRD,nrd_r_tnr_crg_lut_init_param_l9b0,0);

			fw_msg = TRUE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);
			PE_P0L_L9B0_Wr01(pe0_load,	load_time,	0x0);
			PE_P0R_L9B0_Wr01(pe0_load,	load_time,	0x0);
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_nrd_auto_mode[i] = 0;
			}
		}
		else
		{
			fw_msg = FALSE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);

			PE_SET_REG_TABLE(NRD,nrd_l_init_param_l9a0,0);
			PE_SET_REG_TABLE(NRD,nrd_r_init_param_l9a0,0);

			fw_msg = TRUE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);
		}
	}while(0);
	
	return ret;
}

/**
 * standard function of nrd setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_NRD_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_NRD_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_T):
		{
			LX_PE_NRD_DNR_T *pRcvData;
			LX_PE_NRD_DNR_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_DNR_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_DNR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_DNR_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetDnrCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetDnrCtrl() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_DNR_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_T):
		{
			LX_PE_NRD_TNR_T *pRcvData;
			LX_PE_NRD_TNR_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetTnrCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetTnrCtrl() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_CMN_T):
		{
			LX_PE_NRD_DNR_CMN_T *pRcvData;
			LX_PE_NRD_DNR_CMN_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_DNR_CMN_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_DNR_CMN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_DNR_CMN_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetDnrCmnCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetDnrCmnCtrl() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_DNR_CMN_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_DETAIL_T):
		{
			LX_PE_NRD_DNR_DETAIL_T *pRcvData;
			LX_PE_NRD_DNR_DETAIL_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_DNR_DETAIL_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_DNR_DETAIL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_DNR_DETAIL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetDnrDetailCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetDnrDetailCtrl() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_DNR_DETAIL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_CMN_T):
		{
			LX_PE_NRD_TNR_CMN_T *pRcvData;
			LX_PE_NRD_TNR_CMN_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_CMN_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_CMN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_CMN_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetTnrCmnCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetTnrCmnCtrl() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_CMN_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_DETAIL_T):
		{
			LX_PE_NRD_TNR_DETAIL_T *pRcvData;
			LX_PE_NRD_TNR_DETAIL_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_DETAIL_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_DETAIL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_DETAIL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetTnrDetailCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetTnrDetailCtrl() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_DETAIL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_CRGN_T):
		{
			LX_PE_NRD_TNR_CRGN_T *pRcvData;
			LX_PE_NRD_TNR_CRGN_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_CRGN_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_CRGN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_CRGN_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetTnrColorRegionCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetTnrColorRegionCtrl() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_CRGN_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetDefaultSettings(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetDefaultSettings() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_NRD_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_NRD_SetDebugSettings(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_SetDebugSettings() error.\n",__F__,__L__);
				PE_NRD_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_NRD_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of nrd getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_NRD_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_NRD_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_T):
		{
			LX_PE_NRD_DNR_T *pRcvData;
			LX_PE_NRD_DNR_T stParam;
			memset0(stParam);
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_DNR_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_DNR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_DNR_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_NRD_GetDnrCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_GetDnrCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_DNR_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_T):
		{
			LX_PE_NRD_TNR_T *pRcvData;
			LX_PE_NRD_TNR_T stParam;
			memset0(stParam);
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_NRD_GetTnrCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_GetTnrCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_CMN_T):
		{
			LX_PE_NRD_DNR_CMN_T *pRcvData;
			LX_PE_NRD_DNR_CMN_T stParam;
			memset0(stParam);
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_DNR_CMN_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_DNR_CMN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_DNR_CMN_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_NRD_GetDnrCmnCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_GetDnrCmnCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_DNR_CMN_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_DETAIL_T):
		{
			LX_PE_NRD_DNR_DETAIL_T *pRcvData;
			LX_PE_NRD_DNR_DETAIL_T stParam;
			memset0(stParam);
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_DNR_DETAIL_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_DNR_DETAIL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_DNR_DETAIL_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_NRD_GetDnrDetailCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_GetDnrDetailCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_DNR_DETAIL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_CMN_T):
		{
			LX_PE_NRD_TNR_CMN_T *pRcvData;
			LX_PE_NRD_TNR_CMN_T stParam;
			memset0(stParam);
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_CMN_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_CMN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_CMN_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_NRD_GetTnrCmnCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_GetTnrCmnCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_CMN_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_DETAIL_T):
		{
			LX_PE_NRD_TNR_DETAIL_T *pRcvData;
			LX_PE_NRD_TNR_DETAIL_T stParam;
			memset0(stParam);
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_DETAIL_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_DETAIL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_DETAIL_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_NRD_GetTnrDetailCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_GetTnrDetailCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_DETAIL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_CRGN_T):
		{
			LX_PE_NRD_TNR_CRGN_T *pRcvData;
			LX_PE_NRD_TNR_CRGN_T stParam;
			memset0(stParam);
			do{
				PE_NRD_DBG_PRINT("S LX_PE_NRD_TNR_CRGN_T\n");
				/* data size check */
				PE_NRD_CHECK_CODE(datasize!=sizeof(LX_PE_NRD_TNR_CRGN_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_NRD_TNR_CRGN_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_NRD_GetTnrColorRegionCtrl(&stParam);
				PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_NRD_GetTnrColorRegionCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_NRD_DBG_PRINT("E LX_PE_NRD_TNR_CRGN_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_NRD_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_NRD_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	UINT32 fw_msg;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_CHECK_CODE(pstParams->src_type>=LX_PE_SRC_NUM,ret=RET_ERROR;break,\
			"[%s,%d] src_type(%d) is over.\n",__F__,__L__,pstParams->src_type);
		PE_NRD_CHECK_CODE(pstParams->fmt_type>=LX_PE_FMT_NUM,ret=RET_ERROR;break,\
			"[%s,%d] fmt_type(%d) is over.\n",__F__,__L__,pstParams->fmt_type);
		PE_NRD_CHECK_CODE(pstParams->out_type>=LX_PE_OUT_NUM,ret=RET_ERROR;break,\
			"[%s,%d] out_type(%d) is over.\n",__F__,__L__,pstParams->out_type);
		PE_NRD_CHECK_CODE(pstParams->fr_type>=LX_PE_FR_NUM,ret=RET_ERROR;break,\
			"[%s,%d] fr_type(%d) is over.\n",__F__,__L__,pstParams->fr_type);
		PE_NRD_CHECK_CODE(pstParams->scan_type>=LX_PE_SCAN_NUM,ret=RET_ERROR;break,\
			"[%s,%d] scan_type(%d) is over.\n",__F__,__L__,pstParams->scan_type);
		PE_NRD_CHECK_CODE(pstParams->dtv_type>=LX_PE_DTV_NUM,ret=RET_ERROR;break,\
			"[%s,%d] dtv_type(%d) is over.\n",__F__,__L__,pstParams->dtv_type);
		PE_NRD_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			/* set tnr default */
			fw_msg = FALSE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
					if(pstParams->fmt_type==LX_PE_FMT_SECAM)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_atv_secam_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_atv_secam_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_atv_nt_pal_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_atv_nt_pal_sd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_SECAM)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_av_secam_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_av_secam_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_av_nt_pal_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_av_nt_pal_sd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_RGBPC:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_pc_all_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_pc_all_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ || \
							pstParams->dtv_type==LX_PE_DTV_FILEPLAY || \
							pstParams->dtv_type==LX_PE_DTV_HDDPLAY)
						{
						if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_dtv_480_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_dtv_480_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_dtv_576_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_dtv_576_sd_param_l9b0,0);
						}
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_dtv_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_dtv_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_COMP:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_comp_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_comp_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_comp_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_comp_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_hdmi_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_hdmi_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_default_hdmi_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_default_hdmi_hd_param_l9b0,0);
					}
					break;
			}
			fw_msg = TRUE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);

			/* set tnr color region default */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
				case LX_PE_SRC_CVBS:
				case LX_PE_SRC_RGBPC:
				case LX_PE_SRC_DTV:
				case LX_PE_SRC_COMP:
				case LX_PE_SRC_HDMI:
				default:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_tnr_crg_default_all_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_tnr_crg_default_all_param_l9b0,0);
					}
					break;
			}

			/* set dnr default */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
				case LX_PE_SRC_CVBS:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_all_sd_i_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_all_sd_i_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_RGBPC:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_all_hd_p_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_all_hd_p_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_COMP:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->scan_type==LX_PE_SCAN_INTERLACE)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_comp_sd_i_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_comp_sd_i_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_comp_sd_p_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_comp_sd_p_param_l9b0,0);
						}
					}
					else
					{
						if(pstParams->scan_type==LX_PE_SCAN_INTERLACE)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_comp_hd_i_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_comp_hd_i_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_comp_hd_p_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_comp_hd_p_param_l9b0,0);
						}
					}
					break;
				case LX_PE_SRC_DTV:
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->scan_type==LX_PE_SCAN_INTERLACE)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_all_sd_i_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_all_sd_i_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_all_sd_p_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_all_sd_p_param_l9b0,0);
						}
					}
					else
					{
						if(pstParams->scan_type==LX_PE_SCAN_INTERLACE)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_all_hd_i_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_all_hd_i_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_dnr_default_all_hd_p_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_dnr_default_all_hd_p_param_l9b0,0);
						}
					}
					break;
			}

			/* set clc default */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_atv_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_atv_sd_param_l9b0,0);
					}
				break;
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_NTSC)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_av_ntsc_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_av_ntsc_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_av_pal_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_av_pal_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_dtv_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_dtv_sd_param_l9b0,0);
					}
					else
					{
						if(pstParams->dtv_type==LX_PE_DTV_TESTPIC)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_clc_default_pc_all_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_clc_default_pc_all_param_l9b0,0);
						}
						else
						{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_dtv_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_dtv_hd_param_l9b0,0);
					}
					}
					break;
				case LX_PE_SRC_RGBPC:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_pc_all_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_pc_all_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_COMP:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_comp_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_comp_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_comp_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_comp_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_hdmi_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_hdmi_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_clc_default_hdmi_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_clc_default_hdmi_hd_param_l9b0,0);
					}
					break;
			}

			/* set v filter y default */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
				case LX_PE_SRC_CVBS:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_vflt_y_default_all_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_vflt_y_default_all_sd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_RGBPC:
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_vflt_y_default_pc_all_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_vflt_y_default_pc_all_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_DTV:
				case LX_PE_SRC_COMP:
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_vflt_y_default_all_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_vflt_y_default_all_sd_param_l9b0,0);
					}
					else
					{
						if(pstParams->scan_type==LX_PE_SCAN_INTERLACE)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_vflt_y_default_all_hd_i_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_vflt_y_default_all_hd_i_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_l_vflt_y_default_all_hd_p_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(NRD,nrd_r_vflt_y_default_all_hd_p_param_l9b0,0);
						}
					}
					break;
			}
		}
		else
		{
			fw_msg = FALSE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_default_sd_dtv_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_default_sd_dtv_param_l9a0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_default_hd_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_default_hd_param_l9a0,0);
					}
					break;
				case LX_PE_SRC_ATV:
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_SECAM)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_default_sd_ana_secam_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_default_sd_ana_secam_param_l9a0,0);
					}
					else if(pstParams->src_type==LX_PE_SRC_ATV)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_default_sd_ana_rf_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_default_sd_ana_rf_param_l9a0,0);
					}
					else	//CVBS
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_default_sd_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_default_sd_param_l9a0,0);
					}
					break;
				case LX_PE_SRC_COMP:
				case LX_PE_SRC_RGBPC:
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_default_sd_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_default_sd_param_l9a0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_l_default_hd_param_l9a0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(NRD,nrd_r_default_hd_param_l9a0,0);
					}
					break;
			}

			/* tnr motion select */
			if((pstParams->src_type==LX_PE_SRC_ATV||pstParams->src_type==LX_PE_SRC_CVBS)&&
				(pstParams->fmt_type==LX_PE_FMT_SECAM))
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					PE_P0L_L9A0_Wr02(p0l_tnr_ctrl_00,	calc_motion_max_yc, 	0x0,\
														hist_motion_max_yc, 	0x0);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					PE_P0R_L9A0_Wr02(p0r_tnr_ctrl_00,	calc_motion_max_yc, 	0x0,\
														hist_motion_max_yc, 	0x0);
				}
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					PE_P0L_L9A0_Wr02(p0l_tnr_ctrl_00,	calc_motion_max_yc, 	0x1,\
														hist_motion_max_yc, 	0x1);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					PE_P0R_L9A0_Wr02(p0r_tnr_ctrl_00,	calc_motion_max_yc, 	0x1,\
														hist_motion_max_yc, 	0x1);
				}
			}

			fw_msg = TRUE;
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR_ENABLE, &fw_msg, sizeof(fw_msg));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);
		}
	}while(0);
	
	return ret;
}
static int PE_NRD_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(NRD)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_nrd_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(NRD)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(NRD)))
				{
					PE_NRD_DBG_PRINT("tnr,dnr : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P0L_L9B0_Wr01(tnr_ctrl_00,	tnr_enable, 	0x0);
						PE_P0L_L9B0_Wr04(dnr_max_ctrl,	enable_snr, 	0x0,\
														enable_mnr, 	0x0,\
														enable_dc_bnr, 	0x0,\
														enable_ac_bnr, 	0x0);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P0R_L9B0_Wr01(tnr_ctrl_00,	tnr_enable, 	0x0);
						PE_P0R_L9B0_Wr04(dnr_max_ctrl,	enable_snr, 	0x0,\
														enable_mnr, 	0x0,\
														enable_dc_bnr, 	0x0,\
														enable_ac_bnr, 	0x0);
					}
				}
				else
				{
					PE_NRD_DBG_PRINT("tnr,dnr : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P0L_L9B0_Wr01(tnr_ctrl_00,	tnr_enable, 	0x1);
						PE_P0L_L9B0_Wr04(dnr_max_ctrl,	enable_snr, 	0x1,\
														enable_mnr, 	0x1,\
														enable_dc_bnr, 	0x1,\
														enable_ac_bnr, 	0x1);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P0R_L9B0_Wr01(tnr_ctrl_00,	tnr_enable, 	0x1);
						PE_P0R_L9B0_Wr04(dnr_max_ctrl,	enable_snr, 	0x1,\
														enable_mnr, 	0x1,\
														enable_dc_bnr, 	0x1,\
														enable_ac_bnr, 	0x1);
					}
				}
			}
			else
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(NRD)))
				{
					PE_NRD_DBG_PRINT("tnr,bnr(h,v),mnr,dc bnr,snr : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P0L_L9A0_Wr01(p0l_tnr_ctrl_00,		tnr_enable, 	0x0);
						PE_P0L_L9A0_Wr02(p0l_bnr_ac_ctrl_00,	bnr_h_en, 		0x0,\
																bnr_v_en, 		0x0);
						PE_P0L_L9A0_Wr01(p0l_mnr_ctrl_00,		mnr_enable, 	0x0);
						PE_P0L_L9A0_Wr01(p0l_bnr_dc_ctrl_00,	dc_bnr_enable, 	0x0);
						PE_P0L_L9A0_Wr01(p0l_snr_ctrl_00,		snr_enable, 	0x0);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P0R_L9A0_Wr01(p0r_tnr_ctrl_00,		tnr_enable, 	0x0);
						PE_P0R_L9A0_Wr02(p0r_bnr_ac_ctrl_00,	bnr_h_en, 		0x0,\
																bnr_v_en, 		0x0);
						PE_P0R_L9A0_Wr01(p0r_mnr_ctrl_00,		mnr_enable, 	0x0);
						PE_P0R_L9A0_Wr01(p0r_bnr_dc_ctrl_00,	dc_bnr_enable, 	0x0);
						PE_P0R_L9A0_Wr01(p0r_snr_ctrl_00,		snr_enable, 	0x0);
					}
				}
				else
				{
					PE_NRD_DBG_PRINT("tnr,bnr(h,v),mnr : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P0L_L9A0_Wr01(p0l_tnr_ctrl_00,		tnr_enable, 	0x1);
						PE_P0L_L9A0_Wr02(p0l_bnr_ac_ctrl_00,	bnr_h_en, 		0x1,\
																bnr_v_en, 		0x1);
						PE_P0L_L9A0_Wr01(p0l_mnr_ctrl_00,		mnr_enable, 	0x1);
						PE_P0L_L9A0_Wr01(p0l_bnr_dc_ctrl_00,	dc_bnr_enable, 	0x0);
						PE_P0L_L9A0_Wr01(p0l_snr_ctrl_00,		snr_enable, 	0x0);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P0R_L9A0_Wr01(p0r_tnr_ctrl_00,		tnr_enable, 	0x1);
						PE_P0R_L9A0_Wr02(p0r_bnr_ac_ctrl_00,	bnr_h_en, 		0x1,\
																bnr_v_en, 		0x1);
						PE_P0R_L9A0_Wr01(p0r_mnr_ctrl_00,		mnr_enable, 	0x1);
						PE_P0R_L9A0_Wr01(p0r_bnr_dc_ctrl_00,	dc_bnr_enable, 	0x0);
						PE_P0R_L9A0_Wr01(p0r_snr_ctrl_00,		snr_enable, 	0x0);
					}
				}
			}
		}
	}while(0);
	return ret;
}
static int PE_NRD_SetDnrCtrl(LX_PE_NRD_DNR_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_DBG_PRINT("set pstParams[%d] : %d,%d,%d,%d,%d,%d\n",pstParams->win_id,\
			pstParams->mnr_enable,pstParams->ac_bnr_h_en,pstParams->ac_bnr_v_en,\
			pstParams->ac_bnr_gain,pstParams->dc_bnr_gain,pstParams->mnr_gain);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P0L_L9A0_Wr01(p0l_mnr_ctrl_00,		mnr_enable, 		(pstParams->mnr_enable)? 0x1:0x0);
				PE_P0L_L9A0_Wr02(p0l_bnr_ac_ctrl_00,	bnr_h_en, 			(pstParams->ac_bnr_h_en)? 0x1:0x0,\
														bnr_v_en, 			(pstParams->ac_bnr_v_en)? 0x1:0x0);
				PE_P0L_L9A0_Wr01(p0l_bnr_ac_ctrl_01, 	strength_h_max, 	GET_BITS(pstParams->ac_bnr_gain,0,8));
				PE_P0L_L9A0_Wr01(p0l_bnr_dc_ctrl_00, 	gain, 				GET_BITS(pstParams->dc_bnr_gain,0,8));
				PE_P0L_L9A0_Wr01(p0l_mnr_ctrl_05, 		filter_threshold,	GET_BITS(pstParams->mnr_gain,0,8));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9A0_Wr01(p0r_mnr_ctrl_00,		mnr_enable, 		(pstParams->mnr_enable)? 0x1:0x0);
				PE_P0R_L9A0_Wr02(p0r_bnr_ac_ctrl_00,	bnr_h_en, 			(pstParams->ac_bnr_h_en)? 0x1:0x0,\
														bnr_v_en, 			(pstParams->ac_bnr_v_en)? 0x1:0x0);
				PE_P0R_L9A0_Wr01(p0r_bnr_ac_ctrl_01, 	strength_h_max, 	GET_BITS(pstParams->ac_bnr_gain,0,8));
				PE_P0R_L9A0_Wr01(p0r_bnr_dc_ctrl_00, 	gain, 				GET_BITS(pstParams->dc_bnr_gain,0,8));
				PE_P0R_L9A0_Wr01(p0r_mnr_ctrl_05, 		filter_threshold,	GET_BITS(pstParams->mnr_gain,0,8));
			}
		}
	} while (0);
	return ret;
}
static int PE_NRD_GetDnrCtrl(LX_PE_NRD_DNR_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				PE_P0L_L9A0_Rd01(p0l_mnr_ctrl_00,		mnr_enable, 		pstParams->mnr_enable);
				PE_P0L_L9A0_Rd02(p0l_bnr_ac_ctrl_00,	bnr_h_en, 			pstParams->ac_bnr_h_en,\
														bnr_v_en, 			pstParams->ac_bnr_v_en);
				PE_P0L_L9A0_Rd01(p0l_bnr_ac_ctrl_01, 	strength_h_max, 	pstParams->ac_bnr_gain);
				PE_P0L_L9A0_Rd01(p0l_bnr_dc_ctrl_00, 	gain, 				pstParams->dc_bnr_gain);
				PE_P0L_L9A0_Rd01(p0l_mnr_ctrl_05, 		filter_threshold,	pstParams->mnr_gain);
			}
			if(PE_CHECK_SUB(win_id))
			{
				PE_P0R_L9A0_Rd01(p0r_mnr_ctrl_00,		mnr_enable, 		pstParams->mnr_enable);
				PE_P0R_L9A0_Rd02(p0r_bnr_ac_ctrl_00,	bnr_h_en, 			pstParams->ac_bnr_h_en,\
														bnr_v_en, 			pstParams->ac_bnr_v_en);
				PE_P0R_L9A0_Rd01(p0r_bnr_ac_ctrl_01, 	strength_h_max, 	pstParams->ac_bnr_gain);
				PE_P0R_L9A0_Rd01(p0r_bnr_dc_ctrl_00, 	gain, 				pstParams->dc_bnr_gain);
				PE_P0R_L9A0_Rd01(p0r_mnr_ctrl_05, 		filter_threshold,	pstParams->mnr_gain);
			}
		}

		PE_NRD_DBG_PRINT("get pstParams[%d] : %d,%d,%d,%d,%d,%d\n",pstParams->win_id,\
			pstParams->mnr_enable,pstParams->ac_bnr_h_en,pstParams->ac_bnr_v_en,\
			pstParams->ac_bnr_gain,pstParams->dc_bnr_gain,pstParams->mnr_gain);
	} while (0);
	return ret;
}
static int PE_NRD_SetTnrCtrl(LX_PE_NRD_TNR_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_DBG_PRINT("set pstParams[%d] : en:%d,nt_lvl:%d\n",\
			pstParams->win_id,pstParams->tnr_enable,pstParams->adj_nt_lvl_val);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		else
		{
			ret = PE_FWI_SETDATA(PQL_IPC_SET_TNR, pstParams, sizeof(LX_PE_NRD_TNR_T));
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P0L_L9A0_Wr02(p0l_tnr_ctrl_13,	adjust_nt_lvl,		(pstParams->tnr_enable)? 0x1:0x0,\
													adjust_nt_lvl_val,	GET_BITS(pstParams->adj_nt_lvl_val,0,8));
				PE_P0L_L9A0_Wr02(p0l_tnr_ctrl_18,	y_gain,				(pstParams->tnr_enable)? 0x20:0xff,\
													c_gain,				(pstParams->tnr_enable)? 0x20:0xff);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9A0_Wr02(p0r_tnr_ctrl_13,	adjust_nt_lvl,		(pstParams->tnr_enable)? 0x1:0x0,\
													adjust_nt_lvl_val,	GET_BITS(pstParams->adj_nt_lvl_val,0,8));
				PE_P0R_L9A0_Wr02(p0r_tnr_ctrl_18,	y_gain,				(pstParams->tnr_enable)? 0x20:0xff,\
													c_gain,				(pstParams->tnr_enable)? 0x20:0xff);
			}
		}
	} while (0);
	return ret;
}
static int PE_NRD_GetTnrCtrl(LX_PE_NRD_TNR_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				PE_P0L_L9A0_Rd02(p0l_tnr_ctrl_13,	adjust_nt_lvl,		pstParams->tnr_enable,\
													adjust_nt_lvl_val,	pstParams->adj_nt_lvl_val);
			}
			if(PE_CHECK_SUB(win_id))
			{
				PE_P0R_L9A0_Rd02(p0r_tnr_ctrl_13,	adjust_nt_lvl,		pstParams->tnr_enable,\
													adjust_nt_lvl_val,	pstParams->adj_nt_lvl_val);
			}
		}
		PE_NRD_DBG_PRINT("get pstParams[%d] : en:%d,nt_lvl:%d\n",\
			pstParams->win_id,pstParams->tnr_enable,pstParams->adj_nt_lvl_val);
	} while (0);
	return ret;
}
static int PE_NRD_SetDnrCmnCtrl(LX_PE_NRD_DNR_CMN_T *pstParams)
{
	int ret = RET_OK;
	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_DBG_PRINT("set pstParams[%d] : mnr : en:%d, flt_th:%d\n"\
			"ac_bnr: h,v_en:%d,%d,  mo_max,min:%d,%d,  h,v_bmax:%d,%d,  str_h,v_max:%d,%d\n"\
			"dc_bnr: en:%d,  mo_min,max:%d,%d, ac_blki_multi:%d\n",\
			pstParams->win_id,pstParams->mnr_enable,pstParams->filter_threshold,\
			pstParams->bnr_h_en,pstParams->bnr_v_en,pstParams->ac_bnr_protect_motion_max,\
			pstParams->ac_bnr_protect_motion_min,pstParams->hbmax_gain,pstParams->vbmax_gain,\
			pstParams->strength_h_max,pstParams->strength_v_max,\
			pstParams->dc_bnr_enable,pstParams->motion_min,pstParams->motion_max,\
			pstParams->ac_blockiness_multifly);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P0L_L9B0_Wr01(mnr_ctrl_00,	mnr_enable,			GET_BITS(pstParams->mnr_enable,0,1));
				PE_P0L_L9B0_Wr01(mnr_ctrl_05,	filter_threshold,	GET_BITS(pstParams->filter_threshold,0,8));
				PE_P0L_L9B0_Wr04(bnr_ac_ctrl_00,bnr_h_en,			GET_BITS(pstParams->bnr_h_en,0,1),\
												bnr_v_en,			GET_BITS(pstParams->bnr_v_en,0,1),\
												hbmax_gain,			GET_BITS(pstParams->hbmax_gain,0,4),\
												vbmax_gain,			GET_BITS(pstParams->vbmax_gain,0,4));
				PE_P0L_L9B0_Wr01(bnr_ac_ctrl_01,strength_h_max,		GET_BITS(pstParams->strength_h_max,0,8));
				PE_P0L_L9B0_Wr01(bnr_ac_ctrl_02,strength_v_max,		GET_BITS(pstParams->strength_v_max,0,8));
				PE_P0L_L9B0_Wr02(bnr_ac_ctrl_06,ac_bnr_protect_motion_max,	GET_BITS(pstParams->ac_bnr_protect_motion_max,0,8),\
												ac_bnr_protect_motion_min,	GET_BITS(pstParams->ac_bnr_protect_motion_min,0,8));
				PE_P0L_L9B0_Wr03(bnr_dc_ctrl_00,dc_bnr_enable,		GET_BITS(pstParams->dc_bnr_enable,0,1),\
												motion_min,			GET_BITS(pstParams->motion_min,0,4),\
												motion_max,			GET_BITS(pstParams->motion_max,0,4));
				PE_P0L_L9B0_Wr01(bnr_dc_ctrl_01,ac_blockiness_multifly_x_n,GET_BITS(pstParams->ac_blockiness_multifly,0,4));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9B0_Wr01(mnr_ctrl_00,	mnr_enable,			GET_BITS(pstParams->mnr_enable,0,1));
				PE_P0R_L9B0_Wr01(mnr_ctrl_05,	filter_threshold,	GET_BITS(pstParams->filter_threshold,0,8));
				PE_P0R_L9B0_Wr04(bnr_ac_ctrl_00,bnr_h_en,			GET_BITS(pstParams->bnr_h_en,0,1),\
												bnr_v_en,			GET_BITS(pstParams->bnr_v_en,0,1),\
												hbmax_gain,			GET_BITS(pstParams->hbmax_gain,0,4),\
												vbmax_gain,			GET_BITS(pstParams->vbmax_gain,0,4));
				PE_P0R_L9B0_Wr01(bnr_ac_ctrl_01,strength_h_max,		GET_BITS(pstParams->strength_h_max,0,8));
				PE_P0R_L9B0_Wr01(bnr_ac_ctrl_02,strength_v_max,		GET_BITS(pstParams->strength_v_max,0,8));
				PE_P0R_L9B0_Wr02(bnr_ac_ctrl_06,ac_bnr_protect_motion_max,	GET_BITS(pstParams->ac_bnr_protect_motion_max,0,8),\
												ac_bnr_protect_motion_min,	GET_BITS(pstParams->ac_bnr_protect_motion_min,0,8));
				PE_P0R_L9B0_Wr03(bnr_dc_ctrl_00,dc_bnr_enable,		GET_BITS(pstParams->dc_bnr_enable,0,1),\
												motion_min,			GET_BITS(pstParams->motion_min,0,4),\
												motion_max,			GET_BITS(pstParams->motion_max,0,4));
				PE_P0R_L9B0_Wr01(bnr_dc_ctrl_01,ac_blockiness_multifly_x_n,GET_BITS(pstParams->ac_blockiness_multifly,0,4));
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_NRD_GetDnrCmnCtrl(LX_PE_NRD_DNR_CMN_T *pstParams)
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
				PE_P0L_L9B0_Rd01(mnr_ctrl_00,	mnr_enable,			pstParams->mnr_enable);
				PE_P0L_L9B0_Rd01(mnr_ctrl_05,	filter_threshold,	pstParams->filter_threshold);
				PE_P0L_L9B0_Rd04(bnr_ac_ctrl_00,bnr_h_en,			pstParams->bnr_h_en,\
												bnr_v_en,			pstParams->bnr_v_en,\
												hbmax_gain,			pstParams->hbmax_gain,\
												vbmax_gain,			pstParams->vbmax_gain);
				PE_P0L_L9B0_Rd01(bnr_ac_ctrl_01,strength_h_max,		pstParams->strength_h_max);
				PE_P0L_L9B0_Rd01(bnr_ac_ctrl_02,strength_v_max,		pstParams->strength_v_max);
				PE_P0L_L9B0_Rd02(bnr_ac_ctrl_06,ac_bnr_protect_motion_max,	pstParams->ac_bnr_protect_motion_max,\
												ac_bnr_protect_motion_min,	pstParams->ac_bnr_protect_motion_min);
				PE_P0L_L9B0_Rd03(bnr_dc_ctrl_00,dc_bnr_enable,		pstParams->dc_bnr_enable,\
												motion_min,			pstParams->motion_min,\
												motion_max,			pstParams->motion_max);
				PE_P0L_L9B0_Rd01(bnr_dc_ctrl_01,ac_blockiness_multifly_x_n,pstParams->ac_blockiness_multifly);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9B0_Rd01(mnr_ctrl_00,	mnr_enable,			pstParams->mnr_enable);
				PE_P0R_L9B0_Rd01(mnr_ctrl_05,	filter_threshold,	pstParams->filter_threshold);
				PE_P0R_L9B0_Rd04(bnr_ac_ctrl_00,bnr_h_en,			pstParams->bnr_h_en,\
												bnr_v_en,			pstParams->bnr_v_en,\
												hbmax_gain,			pstParams->hbmax_gain,\
												vbmax_gain,			pstParams->vbmax_gain);
				PE_P0R_L9B0_Rd01(bnr_ac_ctrl_01,strength_h_max,		pstParams->strength_h_max);
				PE_P0R_L9B0_Rd01(bnr_ac_ctrl_02,strength_v_max,		pstParams->strength_v_max);
				PE_P0R_L9B0_Rd02(bnr_ac_ctrl_06,ac_bnr_protect_motion_max,	pstParams->ac_bnr_protect_motion_max,\
												ac_bnr_protect_motion_min,	pstParams->ac_bnr_protect_motion_min);
				PE_P0R_L9B0_Rd03(bnr_dc_ctrl_00,dc_bnr_enable,		pstParams->dc_bnr_enable,\
												motion_min,			pstParams->motion_min,\
												motion_max,			pstParams->motion_max);
				PE_P0R_L9B0_Rd01(bnr_dc_ctrl_01,ac_blockiness_multifly_x_n,pstParams->ac_blockiness_multifly);
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_NRD_DBG_PRINT("get pstParams[%d] : mnr : en:%d, flt_th:%d\n"\
			"ac_bnr: h,v_en:%d,%d,  mo_max,min:%d,%d,  h,v_bmax:%d,%d,  str_h,v_max:%d,%d\n"\
			"dc_bnr: en:%d,  mo_min,max:%d,%d, ac_blki_multi:%d\n",\
			pstParams->win_id,pstParams->mnr_enable,pstParams->filter_threshold,\
			pstParams->bnr_h_en,pstParams->bnr_v_en,pstParams->ac_bnr_protect_motion_max,\
			pstParams->ac_bnr_protect_motion_min,pstParams->hbmax_gain,pstParams->vbmax_gain,\
			pstParams->strength_h_max,pstParams->strength_v_max,\
			pstParams->dc_bnr_enable,pstParams->motion_min,pstParams->motion_max,\
			pstParams->ac_blockiness_multifly);
	} while (0);
	return ret;
}
static int PE_NRD_SetDnrDetailCtrl(LX_PE_NRD_DNR_DETAIL_T *pstParams)
{
	int ret = RET_OK;		

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_DBG_PRINT("set pstParams[%d] \n"\
			"mnr   : g_map_en:%d,  sd_hd:%d,  [x]:%d,%d,%d,%d,  [y]:%d,%d,%d,%d\n"\
			"ac_bnr: mo_p_en:%d,  src_type:%d,  flt_type:%d\n"\
			"ac_bnr: [str_h_x]:%d,%d,  det_min_th:%d,  [str_v_x]:%d,%d\n"\
			"dc_bnr: blki_adp:%d,  mo_g_en:%d,  sd_hd:%d\n"\
			"dc_bnr: [x,y]:%d,%d, %d,%d, %d,%d, %d,%d\n",\
			pstParams->win_id,\
			/* mnr */
			pstParams->edge_gain_mapping_enable,pstParams->sel_sd_hd,\
			pstParams->x1_position,pstParams->x2_position,pstParams->x3_position,pstParams->x4_position,\
			pstParams->y1_position,pstParams->y2_position,pstParams->y3_position,pstParams->y4_position,\
			/* ac_bnr */
			pstParams->motion_protect_enable,pstParams->source_type,pstParams->fiter_type,\
			pstParams->strength_h_x0,pstParams->strength_h_x1,pstParams->detect_min_th,\
			pstParams->strength_v_x0,pstParams->strength_v_x1,\
			/* dc_bnr */
			pstParams->ac_blockiness_adaptive_mode,pstParams->motion_gain_enable,pstParams->sd_hd_sel,\
			pstParams->dc_bnr_gain_ctrl_x0,pstParams->dc_bnr_gain_ctrl_y0,\
			pstParams->dc_bnr_gain_ctrl_x1,pstParams->dc_bnr_gain_ctrl_y1,\
			pstParams->dc_bnr_gain_ctrl_x2,pstParams->dc_bnr_gain_ctrl_y2,\
			pstParams->dc_bnr_gain_ctrl_x3,pstParams->dc_bnr_gain_ctrl_y3);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				/* mnr */
				PE_P0L_L9B0_Wr02(mnr_ctrl_00,	edge_gain_mapping_enable,	GET_BITS(pstParams->edge_gain_mapping_enable,0,1),\
												sel_sd_hd,					GET_BITS(pstParams->sel_sd_hd,0,1));
				PE_P0L_L9B0_Wr01(mnr_ctrl_03,	x1_position,	GET_BITS(pstParams->x1_position,0,8));
				PE_P0L_L9B0_Wr04(mnr_ctrl_04,	x2_position,	GET_BITS(pstParams->x2_position,0,8),\
												x3_position,	GET_BITS(pstParams->x3_position,0,8),\
												x4_position,	GET_BITS(pstParams->x4_position,0,8),\
												y1_position,	GET_BITS(pstParams->y1_position,0,8));
				PE_P0L_L9B0_Wr03(mnr_ctrl_05,	y2_position,	GET_BITS(pstParams->y2_position,0,8),\
												y3_position,	GET_BITS(pstParams->y3_position,0,8),\
												y4_position,	GET_BITS(pstParams->y4_position,0,8));
				/* ac_bnr */
				PE_P0L_L9B0_Wr03(bnr_ac_ctrl_00,motion_protect_enable,		GET_BITS(pstParams->motion_protect_enable,0,1),\
												source_type,	GET_BITS(pstParams->source_type,0,1),\
												fiter_type,		GET_BITS(pstParams->fiter_type,0,1));
				PE_P0L_L9B0_Wr03(bnr_ac_ctrl_01,strength_h_x0,	GET_BITS(pstParams->strength_h_x0,0,8),\
												strength_h_x1,	GET_BITS(pstParams->strength_h_x1,0,8),\
												detect_min_th,	GET_BITS(pstParams->detect_min_th,0,8));
				PE_P0L_L9B0_Wr02(bnr_ac_ctrl_02,strength_v_x0,	GET_BITS(pstParams->strength_v_x0,0,8),\
												strength_v_x1,	GET_BITS(pstParams->strength_v_x1,0,8));
				/* dc_bnr */
				PE_P0L_L9B0_Wr03(bnr_dc_ctrl_00,ac_blockiness_adaptive_mode,GET_BITS(pstParams->ac_blockiness_adaptive_mode,0,1),\
												motion_gain_enable,			GET_BITS(pstParams->motion_gain_enable,0,1),\
												sd_hd_sel,					GET_BITS(pstParams->sd_hd_sel,0,1));
				PE_P0L_L9B0_Wr04(bnr_dc_ctrl_02,dc_bnr_gain_ctrl_y2,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y2,0,8),\
												dc_bnr_gain_ctrl_x2,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x2,0,8),\
												dc_bnr_gain_ctrl_y3,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y3,0,8),\
												dc_bnr_gain_ctrl_x3,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x3,0,8));
				PE_P0L_L9B0_Wr04(bnr_dc_ctrl_03,dc_bnr_gain_ctrl_y0,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y0,0,8),\
												dc_bnr_gain_ctrl_x0,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x0,0,8),\
												dc_bnr_gain_ctrl_y1,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y1,0,8),\
												dc_bnr_gain_ctrl_x1,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x1,0,8));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				/* mnr */
				PE_P0R_L9B0_Wr02(mnr_ctrl_00,	edge_gain_mapping_enable,	GET_BITS(pstParams->edge_gain_mapping_enable,0,1),\
												sel_sd_hd,					GET_BITS(pstParams->sel_sd_hd,0,1));
				PE_P0R_L9B0_Wr01(mnr_ctrl_03,	x1_position,	GET_BITS(pstParams->x1_position,0,8));
				PE_P0R_L9B0_Wr04(mnr_ctrl_04,	x2_position,	GET_BITS(pstParams->x2_position,0,8),\
												x3_position,	GET_BITS(pstParams->x3_position,0,8),\
												x4_position,	GET_BITS(pstParams->x4_position,0,8),\
												y1_position,	GET_BITS(pstParams->y1_position,0,8));
				PE_P0R_L9B0_Wr03(mnr_ctrl_05,	y2_position,	GET_BITS(pstParams->y2_position,0,8),\
												y3_position,	GET_BITS(pstParams->y3_position,0,8),\
												y4_position,	GET_BITS(pstParams->y4_position,0,8));
				/* ac_bnr */
				PE_P0R_L9B0_Wr03(bnr_ac_ctrl_00,motion_protect_enable,		GET_BITS(pstParams->motion_protect_enable,0,1),\
												source_type,	GET_BITS(pstParams->source_type,0,1),\
												fiter_type,		GET_BITS(pstParams->fiter_type,0,1));
				PE_P0R_L9B0_Wr03(bnr_ac_ctrl_01,strength_h_x0,	GET_BITS(pstParams->strength_h_x0,0,8),\
												strength_h_x1,	GET_BITS(pstParams->strength_h_x1,0,8),\
												detect_min_th,	GET_BITS(pstParams->detect_min_th,0,8));
				PE_P0R_L9B0_Wr02(bnr_ac_ctrl_02,strength_v_x0,	GET_BITS(pstParams->strength_v_x0,0,8),\
												strength_v_x1,	GET_BITS(pstParams->strength_v_x1,0,8));
				/* dc_bnr */
				PE_P0R_L9B0_Wr03(bnr_dc_ctrl_00,ac_blockiness_adaptive_mode,GET_BITS(pstParams->ac_blockiness_adaptive_mode,0,1),\
												motion_gain_enable,			GET_BITS(pstParams->motion_gain_enable,0,1),\
												sd_hd_sel,					GET_BITS(pstParams->sd_hd_sel,0,1));
				PE_P0R_L9B0_Wr04(bnr_dc_ctrl_02,dc_bnr_gain_ctrl_y2,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y2,0,8),\
												dc_bnr_gain_ctrl_x2,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x2,0,8),\
												dc_bnr_gain_ctrl_y3,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y3,0,8),\
												dc_bnr_gain_ctrl_x3,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x3,0,8));
				PE_P0R_L9B0_Wr04(bnr_dc_ctrl_03,dc_bnr_gain_ctrl_y0,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y0,0,8),\
												dc_bnr_gain_ctrl_x0,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x0,0,8),\
												dc_bnr_gain_ctrl_y1,	GET_BITS(pstParams->dc_bnr_gain_ctrl_y1,0,8),\
												dc_bnr_gain_ctrl_x1,	GET_BITS(pstParams->dc_bnr_gain_ctrl_x1,0,8));
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_NRD_GetDnrDetailCtrl(LX_PE_NRD_DNR_DETAIL_T *pstParams)
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
				/* mnr */
				PE_P0L_L9B0_Rd02(mnr_ctrl_00,	edge_gain_mapping_enable,	pstParams->edge_gain_mapping_enable,\
												sel_sd_hd,					pstParams->sel_sd_hd);
				PE_P0L_L9B0_Rd01(mnr_ctrl_03,	x1_position,				pstParams->x1_position);
				PE_P0L_L9B0_Rd04(mnr_ctrl_04,	x2_position,				pstParams->x2_position,\
												x3_position,				pstParams->x3_position,\
												x4_position,				pstParams->x4_position,\
												y1_position,				pstParams->y1_position);
				PE_P0L_L9B0_Rd03(mnr_ctrl_05,	y2_position,				pstParams->y2_position,\
												y3_position,				pstParams->y3_position,\
												y4_position,				pstParams->y4_position);
				/* ac_bnr */
				PE_P0L_L9B0_Rd03(bnr_ac_ctrl_00,motion_protect_enable,		pstParams->motion_protect_enable,\
												source_type,				pstParams->source_type,\
												fiter_type,					pstParams->fiter_type);
				PE_P0L_L9B0_Rd03(bnr_ac_ctrl_01,strength_h_x0,				pstParams->strength_h_x0,\
												strength_h_x1,				pstParams->strength_h_x1,\
												detect_min_th,				pstParams->detect_min_th);
				PE_P0L_L9B0_Rd02(bnr_ac_ctrl_02,strength_v_x0,				pstParams->strength_v_x0,\
												strength_v_x1,				pstParams->strength_v_x1);
				/* dc_bnr */
				PE_P0L_L9B0_Rd03(bnr_dc_ctrl_00,ac_blockiness_adaptive_mode,pstParams->ac_blockiness_adaptive_mode,\
												motion_gain_enable,			pstParams->motion_gain_enable,\
												sd_hd_sel,					pstParams->sd_hd_sel);
				PE_P0L_L9B0_Rd04(bnr_dc_ctrl_02,dc_bnr_gain_ctrl_y2,		pstParams->dc_bnr_gain_ctrl_y2,\
												dc_bnr_gain_ctrl_x2,		pstParams->dc_bnr_gain_ctrl_x2,\
												dc_bnr_gain_ctrl_y3,		pstParams->dc_bnr_gain_ctrl_y3,\
												dc_bnr_gain_ctrl_x3,		pstParams->dc_bnr_gain_ctrl_x3);
				PE_P0L_L9B0_Rd04(bnr_dc_ctrl_03,dc_bnr_gain_ctrl_y0,		pstParams->dc_bnr_gain_ctrl_y0,\
												dc_bnr_gain_ctrl_x0,		pstParams->dc_bnr_gain_ctrl_x0,\
												dc_bnr_gain_ctrl_y1,		pstParams->dc_bnr_gain_ctrl_y1,\
												dc_bnr_gain_ctrl_x1,		pstParams->dc_bnr_gain_ctrl_x1);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				/* mnr */
				PE_P0R_L9B0_Rd02(mnr_ctrl_00,	edge_gain_mapping_enable,	pstParams->edge_gain_mapping_enable,\
												sel_sd_hd,					pstParams->sel_sd_hd);
				PE_P0R_L9B0_Rd01(mnr_ctrl_03,	x1_position,				pstParams->x1_position);
				PE_P0R_L9B0_Rd04(mnr_ctrl_04,	x2_position,				pstParams->x2_position,\
												x3_position,				pstParams->x3_position,\
												x4_position,				pstParams->x4_position,\
												y1_position,				pstParams->y1_position);
				PE_P0R_L9B0_Rd03(mnr_ctrl_05,	y2_position,				pstParams->y2_position,\
												y3_position,				pstParams->y3_position,\
												y4_position,				pstParams->y4_position);
				/* ac_bnr */
				PE_P0R_L9B0_Rd03(bnr_ac_ctrl_00,motion_protect_enable,		pstParams->motion_protect_enable,\
												source_type,				pstParams->source_type,\
												fiter_type,					pstParams->fiter_type);
				PE_P0R_L9B0_Rd03(bnr_ac_ctrl_01,strength_h_x0,				pstParams->strength_h_x0,\
												strength_h_x1,				pstParams->strength_h_x1,\
												detect_min_th,				pstParams->detect_min_th);
				PE_P0R_L9B0_Rd02(bnr_ac_ctrl_02,strength_v_x0,				pstParams->strength_v_x0,\
												strength_v_x1,				pstParams->strength_v_x1);
				/* dc_bnr */
				PE_P0R_L9B0_Rd03(bnr_dc_ctrl_00,ac_blockiness_adaptive_mode,pstParams->ac_blockiness_adaptive_mode,\
												motion_gain_enable,			pstParams->motion_gain_enable,\
												sd_hd_sel,					pstParams->sd_hd_sel);
				PE_P0R_L9B0_Rd04(bnr_dc_ctrl_02,dc_bnr_gain_ctrl_y2,		pstParams->dc_bnr_gain_ctrl_y2,\
												dc_bnr_gain_ctrl_x2,		pstParams->dc_bnr_gain_ctrl_x2,\
												dc_bnr_gain_ctrl_y3,		pstParams->dc_bnr_gain_ctrl_y3,\
												dc_bnr_gain_ctrl_x3,		pstParams->dc_bnr_gain_ctrl_x3);
				PE_P0R_L9B0_Rd04(bnr_dc_ctrl_03,dc_bnr_gain_ctrl_y0,		pstParams->dc_bnr_gain_ctrl_y0,\
												dc_bnr_gain_ctrl_x0,		pstParams->dc_bnr_gain_ctrl_x0,\
												dc_bnr_gain_ctrl_y1,		pstParams->dc_bnr_gain_ctrl_y1,\
												dc_bnr_gain_ctrl_x1,		pstParams->dc_bnr_gain_ctrl_x1);
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_NRD_DBG_PRINT("get pstParams[%d] \n"\
			"mnr   : g_map_en:%d,  sd_hd:%d,  [x]:%d,%d,%d,%d,  [y]:%d,%d,%d,%d\n"\
			"ac_bnr: mo_p_en:%d,  src_type:%d,  flt_type:%d\n"\
			"ac_bnr: [str_h_x]:%d,%d,  det_min_th:%d,  [str_v_x]:%d,%d\n"\
			"dc_bnr: blki_adp:%d,  mo_g_en:%d,  sd_hd:%d\n"\
			"dc_bnr: [x,y]:%d,%d, %d,%d, %d,%d, %d,%d\n",\
			pstParams->win_id,\
			/* mnr */
			pstParams->edge_gain_mapping_enable,pstParams->sel_sd_hd,\
			pstParams->x1_position,pstParams->x2_position,pstParams->x3_position,pstParams->x4_position,\
			pstParams->y1_position,pstParams->y2_position,pstParams->y3_position,pstParams->y4_position,\
			/* ac_bnr */
			pstParams->motion_protect_enable,pstParams->source_type,pstParams->fiter_type,\
			pstParams->strength_h_x0,pstParams->strength_h_x1,pstParams->detect_min_th,\
			pstParams->strength_v_x0,pstParams->strength_v_x1,\
			/* dc_bnr */
			pstParams->ac_blockiness_adaptive_mode,pstParams->motion_gain_enable,pstParams->sd_hd_sel,\
			pstParams->dc_bnr_gain_ctrl_x0,pstParams->dc_bnr_gain_ctrl_y0,\
			pstParams->dc_bnr_gain_ctrl_x1,pstParams->dc_bnr_gain_ctrl_y1,\
			pstParams->dc_bnr_gain_ctrl_x2,pstParams->dc_bnr_gain_ctrl_y2,\
			pstParams->dc_bnr_gain_ctrl_x3,pstParams->dc_bnr_gain_ctrl_y3);
	} while (0);
	return ret;
}
static int PE_NRD_SetTnrCmnCtrl(LX_PE_NRD_TNR_CMN_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_DBG_PRINT("set pstParams[%d] : auto:%d, y,c_g:%d,%d, adj_nt_en,val:%d,%d\n",\
			pstParams->win_id,pstParams->auto_mode,pstParams->y_gain,pstParams->c_gain,\
			pstParams->adjust_nt_lvl,pstParams->adjust_nt_lvl_val);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P0L_L9B0_Wr02(tnr_ctrl_13,	adjust_nt_lvl,		GET_BITS(pstParams->adjust_nt_lvl,0,1),\
												adjust_nt_lvl_val,	GET_BITS(pstParams->adjust_nt_lvl_val,0,8));
				PE_P0L_L9B0_Wr02(tnr_ctrl_18,	y_gain,				GET_BITS(pstParams->y_gain,0,8),\
												c_gain,				GET_BITS(pstParams->c_gain,0,8));
				_g_nrd_auto_mode[LX_PE_WIN_MAIN] = pstParams->auto_mode;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9B0_Wr02(tnr_ctrl_13,	adjust_nt_lvl,		GET_BITS(pstParams->adjust_nt_lvl,0,1),\
												adjust_nt_lvl_val,	GET_BITS(pstParams->adjust_nt_lvl_val,0,8));
				PE_P0R_L9B0_Wr02(tnr_ctrl_18,	y_gain,				GET_BITS(pstParams->y_gain,0,8),\
												c_gain,				GET_BITS(pstParams->c_gain,0,8));
				_g_nrd_auto_mode[LX_PE_WIN_SUB] = pstParams->auto_mode;
			}
			ret = PE_ETC_InformTnrAutoMode(pstParams);
			PE_NRD_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_InformTnrAutoMode() error.\n",__F__,__L__);
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_NRD_GetTnrCmnCtrl(LX_PE_NRD_TNR_CMN_T *pstParams)
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
				PE_P0L_L9B0_Rd02(tnr_ctrl_13,	adjust_nt_lvl,		pstParams->adjust_nt_lvl,\
												adjust_nt_lvl_val,	pstParams->adjust_nt_lvl_val);
				PE_P0L_L9B0_Rd02(tnr_ctrl_18,	y_gain,				pstParams->y_gain,\
												c_gain,				pstParams->c_gain);
				pstParams->auto_mode = _g_nrd_auto_mode[LX_PE_WIN_MAIN];
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9B0_Rd02(tnr_ctrl_13,	adjust_nt_lvl,		pstParams->adjust_nt_lvl,\
												adjust_nt_lvl_val,	pstParams->adjust_nt_lvl_val);
				PE_P0R_L9B0_Rd02(tnr_ctrl_18,	y_gain,				pstParams->y_gain,\
												c_gain,				pstParams->c_gain);
				pstParams->auto_mode = _g_nrd_auto_mode[LX_PE_WIN_SUB];
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_NRD_DBG_PRINT("get pstParams[%d] : auto:%d, y,c_g:%d,%d, adj_nt_en,val:%d,%d\n",\
			pstParams->win_id,pstParams->auto_mode,pstParams->y_gain,pstParams->c_gain,\
			pstParams->adjust_nt_lvl,pstParams->adjust_nt_lvl_val);
	} while (0);
	return ret;
}
static int PE_NRD_SetTnrDetailCtrl(LX_PE_NRD_TNR_DETAIL_T *pstParams)
{
	int ret = RET_OK;
	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_DBG_PRINT("set pstParams[%d] \n"\
			"motion histo                : %d, %d, %d, %d\n"\
			"alpha gain - gm gain        : %d,  [y,x]:%d,%d, %d,%d, %d,%d, %d,%d\n"\
			"alpha gain - avg st gain    : %d,  [y,x]:%d,%d, %d,%d, %d,%d, %d,%d\n"\
			"alpha lut                   : %d, %d, %d, %d, %d, %d, %d, %d\n"\
			"adjust alpha                : %d, %d, %d, %d\n"\
			"adjust alpha - s. var to wa : %d,  [x]:%d,%d,%d,%d,  [y]:%d,%d,%d,%d\n"\
			"etc(lpct_en,lum_gain_en)    : %d,%d\n",\
			pstParams->win_id,\
			/* motion histo(n-lvl estimate) */
			pstParams->calc_motion_flt_enable,pstParams->calc_motion_flt_type,\
			pstParams->y_th_lo,pstParams->y_th_hi,\
			/* alpha gain(n-lvl gaining), gm gain */
			pstParams->nt_lvl_adjust_gm_enable,pstParams->nt_lvl_gain_gm_y0,pstParams->nt_lvl_gain_gm_x0,\
			pstParams->nt_lvl_gain_gm_y1,pstParams->nt_lvl_gain_gm_x1,pstParams->nt_lvl_gain_gm_y2,\
			pstParams->nt_lvl_gain_gm_x2,pstParams->nt_lvl_gain_gm_y3,pstParams->nt_lvl_gain_gm_x3,\
			/* alpha gain(n-lvl gaining), avg st gain */
			pstParams->nt_lvl_adjust_avg_ts_enable,pstParams->nt_lvl_gain_st_y0,\
			pstParams->nt_lvl_gain_st_x0,pstParams->nt_lvl_gain_st_y1,pstParams->nt_lvl_gain_st_x1,\
			pstParams->nt_lvl_gain_st_y2,pstParams->nt_lvl_gain_st_x2,pstParams->nt_lvl_gain_st_y3,\
			pstParams->nt_lvl_gain_st_x3,\
			/* alpha lut(n-lvl apply) */
			pstParams->alpha_lut_ind0,pstParams->alpha_lut_ind1,pstParams->alpha_lut_ind2,\
			pstParams->alpha_lut_ind3,pstParams->alpha_lut_ind4,pstParams->alpha_lut_ind5,\
			pstParams->alpha_lut_ind6,pstParams->alpha_lut_ind7,\
			/* adjust alpha(alpha gaining), adjust alpha */
			pstParams->adj_alpha_nt_lvl_th0,pstParams->adj_alpha_nt_lvl_th1,\
			pstParams->adj_alpha_k0,pstParams->adj_alpha_k1,\
			/* adjust alpha(alpha gaining), s. var --> WA */
			pstParams->wa_enable,pstParams->wa_x0,pstParams->wa_x1,pstParams->wa_x2,pstParams->wa_x3,\
			pstParams->wa_y0,pstParams->wa_y1,pstParams->wa_y2,pstParams->wa_y3,
			/* etc */
			pstParams->nt_lvl_adjust_lpct_enable,pstParams->luma_gain_enable);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				/* top */
				PE_P0L_L9B0_Wr05(tnr_ctrl_00,	wa_enable,					GET_BITS(pstParams->wa_enable,0,1),\
												calc_motion_flt_enable,		GET_BITS(pstParams->calc_motion_flt_enable,0,1),\
												calc_motion_flt_type,		GET_BITS(pstParams->calc_motion_flt_type,0,1),\
												nt_lvl_adjust_gm_enable,	GET_BITS(pstParams->nt_lvl_adjust_gm_enable,0,1),\
												nt_lvl_adjust_avg_ts_enable,GET_BITS(pstParams->nt_lvl_adjust_avg_ts_enable,0,1));
				/* motion histo(n-lvl estimate) */
				PE_P0L_L9B0_Wr02(tnr_ctrl_01,	y_th_lo,			GET_BITS(pstParams->y_th_lo,0,10),\
												y_th_hi,			GET_BITS(pstParams->y_th_hi,0,10));
				/* alpha gain(n-lvl gaining), gm gain */
				PE_P0L_L9B0_Wr04(tnr_ctrl_04,	nt_lvl_gain_gm_y0,	GET_BITS(pstParams->nt_lvl_gain_gm_y0,0,8),\
												nt_lvl_gain_gm_x0,	GET_BITS(pstParams->nt_lvl_gain_gm_x0,0,8),\
												nt_lvl_gain_gm_y1,	GET_BITS(pstParams->nt_lvl_gain_gm_y1,0,8),\
												nt_lvl_gain_gm_x1,	GET_BITS(pstParams->nt_lvl_gain_gm_x1,0,8));
				PE_P0L_L9B0_Wr04(tnr_ctrl_12,	nt_lvl_gain_gm_y2,	GET_BITS(pstParams->nt_lvl_gain_gm_y2,0,8),\
												nt_lvl_gain_gm_x2,	GET_BITS(pstParams->nt_lvl_gain_gm_x2,0,8),\
												nt_lvl_gain_gm_y3,	GET_BITS(pstParams->nt_lvl_gain_gm_y3,0,8),\
												nt_lvl_gain_gm_x3,	GET_BITS(pstParams->nt_lvl_gain_gm_x3,0,8));
				/* alpha gain(n-lvl gaining), avg st gain */
				PE_P0L_L9B0_Wr04(tnr_ctrl_28,	nt_lvl_gain_st_y0,	GET_BITS(pstParams->nt_lvl_gain_st_y0,0,8),\
												nt_lvl_gain_st_x0,	GET_BITS(pstParams->nt_lvl_gain_st_x0,0,8),\
												nt_lvl_gain_st_y1,	GET_BITS(pstParams->nt_lvl_gain_st_y1,0,8),\
												nt_lvl_gain_st_x1,	GET_BITS(pstParams->nt_lvl_gain_st_x1,0,8));
				PE_P0L_L9B0_Wr04(tnr_ctrl_30,	nt_lvl_gain_st_y2,	GET_BITS(pstParams->nt_lvl_gain_st_y2,0,8),\
												nt_lvl_gain_st_x2,	GET_BITS(pstParams->nt_lvl_gain_st_x2,0,8),\
												nt_lvl_gain_st_y3,	GET_BITS(pstParams->nt_lvl_gain_st_y3,0,8),\
												nt_lvl_gain_st_x3,	GET_BITS(pstParams->nt_lvl_gain_st_x3,0,8));
				/* alpha lut(n-lvl apply) */
				PE_P0L_L9B0_Wr04(tnr_ctrl_07,	alpha_lut_ind0,		GET_BITS(pstParams->alpha_lut_ind0,0,8),\
												alpha_lut_ind1,		GET_BITS(pstParams->alpha_lut_ind1,0,8),\
												alpha_lut_ind2,		GET_BITS(pstParams->alpha_lut_ind2,0,8),\
												alpha_lut_ind3,		GET_BITS(pstParams->alpha_lut_ind3,0,8));
				PE_P0L_L9B0_Wr03(tnr_ctrl_08,	alpha_lut_ind4,		GET_BITS(pstParams->alpha_lut_ind4,0,8),\
												alpha_lut_ind5,		GET_BITS(pstParams->alpha_lut_ind5,0,8),\
												alpha_lut_ind6,		GET_BITS(pstParams->alpha_lut_ind6,0,8));
				PE_P0L_L9B0_Wr01(tnr_ctrl_02,	alpha_lut_ind7,		GET_BITS(pstParams->alpha_lut_ind7,0,8));
				/* adjust alpha(alpha gaining), adjust alpha */
				PE_P0L_L9B0_Wr01(tnr_ctrl_05,	adj_alpha_nt_lvl_th0,	GET_BITS(pstParams->adj_alpha_nt_lvl_th0,0,8));
				PE_P0L_L9B0_Wr01(tnr_ctrl_08,	adj_alpha_nt_lvl_th1,	GET_BITS(pstParams->adj_alpha_nt_lvl_th1,0,8));
				PE_P0L_L9B0_Wr02(tnr_ctrl_06,	adj_alpha_k0,			GET_BITS(pstParams->adj_alpha_k0,0,8),\
												adj_alpha_k1,			GET_BITS(pstParams->adj_alpha_k1,0,8));
				/* adjust alpha(alpha gaining), s. var --> WA */
				PE_P0L_L9B0_Wr04(tnr_ctrl_03,	wa_y0,					GET_BITS(pstParams->wa_y0,0,8),\
												wa_y1,					GET_BITS(pstParams->wa_y1,0,8),\
												wa_x0,					GET_BITS(pstParams->wa_x0,0,8),\
												wa_x1,					GET_BITS(pstParams->wa_x1,0,8));
				PE_P0L_L9B0_Wr04(tnr_ctrl_29,	wa_y2,					GET_BITS(pstParams->wa_y2,0,8),\
												wa_y3,					GET_BITS(pstParams->wa_y3,0,8),\
												wa_x2,					GET_BITS(pstParams->wa_x2,0,8),\
												wa_x3,					GET_BITS(pstParams->wa_x3,0,8));
				/* etc */
				PE_P0L_L9B0_Wr01(tnr_ctrl_00,	nt_lvl_adjust_lpct_enable,	GET_BITS(pstParams->nt_lvl_adjust_lpct_enable,0,1));
				PE_P0L_L9B0_Wr01(tnr_ctrl_18,	luma_gain_enable,			GET_BITS(pstParams->luma_gain_enable,0,1));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				/* top */
				PE_P0R_L9B0_Wr05(tnr_ctrl_00,	wa_enable,					GET_BITS(pstParams->wa_enable,0,1),\
												calc_motion_flt_enable,		GET_BITS(pstParams->calc_motion_flt_enable,0,1),\
												calc_motion_flt_type,		GET_BITS(pstParams->calc_motion_flt_type,0,1),\
												nt_lvl_adjust_gm_enable,	GET_BITS(pstParams->nt_lvl_adjust_gm_enable,0,1),\
												nt_lvl_adjust_avg_ts_enable,GET_BITS(pstParams->nt_lvl_adjust_avg_ts_enable,0,1));
				/* motion histo(n-lvl estimate) */
				PE_P0R_L9B0_Wr02(tnr_ctrl_01,	y_th_lo,			GET_BITS(pstParams->y_th_lo,0,10),\
												y_th_hi,			GET_BITS(pstParams->y_th_hi,0,10));
				/* alpha gain(n-lvl gaining), gm gain */
				PE_P0R_L9B0_Wr04(tnr_ctrl_04,	nt_lvl_gain_gm_y0,	GET_BITS(pstParams->nt_lvl_gain_gm_y0,0,8),\
												nt_lvl_gain_gm_x0,	GET_BITS(pstParams->nt_lvl_gain_gm_x0,0,8),\
												nt_lvl_gain_gm_y1,	GET_BITS(pstParams->nt_lvl_gain_gm_y1,0,8),\
												nt_lvl_gain_gm_x1,	GET_BITS(pstParams->nt_lvl_gain_gm_x1,0,8));
				PE_P0R_L9B0_Wr04(tnr_ctrl_12,	nt_lvl_gain_gm_y2,	GET_BITS(pstParams->nt_lvl_gain_gm_y2,0,8),\
												nt_lvl_gain_gm_x2,	GET_BITS(pstParams->nt_lvl_gain_gm_x2,0,8),\
												nt_lvl_gain_gm_y3,	GET_BITS(pstParams->nt_lvl_gain_gm_y3,0,8),\
												nt_lvl_gain_gm_x3,	GET_BITS(pstParams->nt_lvl_gain_gm_x3,0,8));
				/* alpha gain(n-lvl gaining), avg st gain */
				PE_P0R_L9B0_Wr04(tnr_ctrl_28,	nt_lvl_gain_st_y0,	GET_BITS(pstParams->nt_lvl_gain_st_y0,0,8),\
												nt_lvl_gain_st_x0,	GET_BITS(pstParams->nt_lvl_gain_st_x0,0,8),\
												nt_lvl_gain_st_y1,	GET_BITS(pstParams->nt_lvl_gain_st_y1,0,8),\
												nt_lvl_gain_st_x1,	GET_BITS(pstParams->nt_lvl_gain_st_x1,0,8));
				PE_P0R_L9B0_Wr04(tnr_ctrl_30,	nt_lvl_gain_st_y2,	GET_BITS(pstParams->nt_lvl_gain_st_y2,0,8),\
												nt_lvl_gain_st_x2,	GET_BITS(pstParams->nt_lvl_gain_st_x2,0,8),\
												nt_lvl_gain_st_y3,	GET_BITS(pstParams->nt_lvl_gain_st_y3,0,8),\
												nt_lvl_gain_st_x3,	GET_BITS(pstParams->nt_lvl_gain_st_x3,0,8));
				/* alpha lut(n-lvl apply) */
				PE_P0R_L9B0_Wr04(tnr_ctrl_07,	alpha_lut_ind0,		GET_BITS(pstParams->alpha_lut_ind0,0,8),\
												alpha_lut_ind1,		GET_BITS(pstParams->alpha_lut_ind1,0,8),\
												alpha_lut_ind2,		GET_BITS(pstParams->alpha_lut_ind2,0,8),\
												alpha_lut_ind3,		GET_BITS(pstParams->alpha_lut_ind3,0,8));
				PE_P0R_L9B0_Wr03(tnr_ctrl_08,	alpha_lut_ind4,		GET_BITS(pstParams->alpha_lut_ind4,0,8),\
												alpha_lut_ind5,		GET_BITS(pstParams->alpha_lut_ind5,0,8),\
												alpha_lut_ind6,		GET_BITS(pstParams->alpha_lut_ind6,0,8));
				PE_P0R_L9B0_Wr01(tnr_ctrl_02,	alpha_lut_ind7,		GET_BITS(pstParams->alpha_lut_ind7,0,8));
				/* adjust alpha(alpha gaining), adjust alpha */
				PE_P0R_L9B0_Wr01(tnr_ctrl_05,	adj_alpha_nt_lvl_th0,	GET_BITS(pstParams->adj_alpha_nt_lvl_th0,0,8));
				PE_P0R_L9B0_Wr01(tnr_ctrl_08,	adj_alpha_nt_lvl_th1,	GET_BITS(pstParams->adj_alpha_nt_lvl_th1,0,8));
				PE_P0R_L9B0_Wr02(tnr_ctrl_06,	adj_alpha_k0,			GET_BITS(pstParams->adj_alpha_k0,0,8),\
												adj_alpha_k1,			GET_BITS(pstParams->adj_alpha_k1,0,8));
				/* adjust alpha(alpha gaining), s. var --> WA */
				PE_P0R_L9B0_Wr04(tnr_ctrl_03,	wa_y0,					GET_BITS(pstParams->wa_y0,0,8),\
												wa_y1,					GET_BITS(pstParams->wa_y1,0,8),\
												wa_x0,					GET_BITS(pstParams->wa_x0,0,8),\
												wa_x1,					GET_BITS(pstParams->wa_x1,0,8));
				PE_P0R_L9B0_Wr04(tnr_ctrl_29,	wa_y2,					GET_BITS(pstParams->wa_y2,0,8),\
												wa_y3,					GET_BITS(pstParams->wa_y3,0,8),\
												wa_x2,					GET_BITS(pstParams->wa_x2,0,8),\
												wa_x3,					GET_BITS(pstParams->wa_x3,0,8));
				/* etc */
				PE_P0R_L9B0_Wr01(tnr_ctrl_00,	nt_lvl_adjust_lpct_enable,	GET_BITS(pstParams->nt_lvl_adjust_lpct_enable,0,1));
				PE_P0R_L9B0_Wr01(tnr_ctrl_18,	luma_gain_enable,			GET_BITS(pstParams->luma_gain_enable,0,1));
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_NRD_GetTnrDetailCtrl(LX_PE_NRD_TNR_DETAIL_T *pstParams)
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
				/* top */
				PE_P0L_L9B0_Rd05(tnr_ctrl_00,	wa_enable,					pstParams->wa_enable,\
												calc_motion_flt_enable,		pstParams->calc_motion_flt_enable,\
												calc_motion_flt_type,		pstParams->calc_motion_flt_type,\
												nt_lvl_adjust_gm_enable,	pstParams->nt_lvl_adjust_gm_enable,\
												nt_lvl_adjust_avg_ts_enable,pstParams->nt_lvl_adjust_avg_ts_enable);
				/* motion histo(n-lvl estimate) */
				PE_P0L_L9B0_Rd02(tnr_ctrl_01,	y_th_lo,			pstParams->y_th_lo,\
												y_th_hi,			pstParams->y_th_hi);
				/* alpha gain(n-lvl gaining), gm gain */
				PE_P0L_L9B0_Rd04(tnr_ctrl_04,	nt_lvl_gain_gm_y0,	pstParams->nt_lvl_gain_gm_y0,\
												nt_lvl_gain_gm_x0,	pstParams->nt_lvl_gain_gm_x0,\
												nt_lvl_gain_gm_y1,	pstParams->nt_lvl_gain_gm_y1,\
												nt_lvl_gain_gm_x1,	pstParams->nt_lvl_gain_gm_x1);
				PE_P0L_L9B0_Rd04(tnr_ctrl_12,	nt_lvl_gain_gm_y2,	pstParams->nt_lvl_gain_gm_y2,\
												nt_lvl_gain_gm_x2,	pstParams->nt_lvl_gain_gm_x2,\
												nt_lvl_gain_gm_y3,	pstParams->nt_lvl_gain_gm_y3,\
												nt_lvl_gain_gm_x3,	pstParams->nt_lvl_gain_gm_x3);
				/* alpha gain(n-lvl gaining), avg st gain */
				PE_P0L_L9B0_Rd04(tnr_ctrl_28,	nt_lvl_gain_st_y0,	pstParams->nt_lvl_gain_st_y0,\
												nt_lvl_gain_st_x0,	pstParams->nt_lvl_gain_st_x0,\
												nt_lvl_gain_st_y1,	pstParams->nt_lvl_gain_st_y1,\
												nt_lvl_gain_st_x1,	pstParams->nt_lvl_gain_st_x1);
				PE_P0L_L9B0_Rd04(tnr_ctrl_30,	nt_lvl_gain_st_y2,	pstParams->nt_lvl_gain_st_y2,\
												nt_lvl_gain_st_x2,	pstParams->nt_lvl_gain_st_x2,\
												nt_lvl_gain_st_y3,	pstParams->nt_lvl_gain_st_y3,\
												nt_lvl_gain_st_x3,	pstParams->nt_lvl_gain_st_x3);
				/* alpha lut(n-lvl apply) */
				PE_P0L_L9B0_Rd04(tnr_ctrl_07,	alpha_lut_ind0,		pstParams->alpha_lut_ind0,\
												alpha_lut_ind1,		pstParams->alpha_lut_ind1,\
												alpha_lut_ind2,		pstParams->alpha_lut_ind2,\
												alpha_lut_ind3,		pstParams->alpha_lut_ind3);
				PE_P0L_L9B0_Rd03(tnr_ctrl_08,	alpha_lut_ind4,		pstParams->alpha_lut_ind4,\
												alpha_lut_ind5,		pstParams->alpha_lut_ind5,\
												alpha_lut_ind6,		pstParams->alpha_lut_ind6);
				PE_P0L_L9B0_Rd01(tnr_ctrl_02,	alpha_lut_ind7,		pstParams->alpha_lut_ind7);
				/* adjust alpha(alpha gaining), adjust alpha */
				PE_P0L_L9B0_Rd01(tnr_ctrl_05,	adj_alpha_nt_lvl_th0,	pstParams->adj_alpha_nt_lvl_th0);
				PE_P0L_L9B0_Rd01(tnr_ctrl_08,	adj_alpha_nt_lvl_th1,	pstParams->adj_alpha_nt_lvl_th1);
				PE_P0L_L9B0_Rd02(tnr_ctrl_06,	adj_alpha_k0,			pstParams->adj_alpha_k0,\
												adj_alpha_k1,			pstParams->adj_alpha_k1);
				/* adjust alpha(alpha gaining), s. var --> WA */
				PE_P0L_L9B0_Rd04(tnr_ctrl_03,	wa_y0,					pstParams->wa_y0,\
												wa_y1,					pstParams->wa_y1,\
												wa_x0,					pstParams->wa_x0,\
												wa_x1,					pstParams->wa_x1);
				PE_P0L_L9B0_Rd04(tnr_ctrl_29,	wa_y2,					pstParams->wa_y2,\
												wa_y3,					pstParams->wa_y3,\
												wa_x2,					pstParams->wa_x2,\
												wa_x3,					pstParams->wa_x3);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				/* top */
				PE_P0R_L9B0_Rd05(tnr_ctrl_00,	wa_enable,					pstParams->wa_enable,\
												calc_motion_flt_enable,		pstParams->calc_motion_flt_enable,\
												calc_motion_flt_type,		pstParams->calc_motion_flt_type,\
												nt_lvl_adjust_gm_enable,	pstParams->nt_lvl_adjust_gm_enable,\
												nt_lvl_adjust_avg_ts_enable,pstParams->nt_lvl_adjust_avg_ts_enable);
				/* motion histo(n-lvl estimate) */
				PE_P0R_L9B0_Rd02(tnr_ctrl_01,	y_th_lo,			pstParams->y_th_lo,\
												y_th_hi,			pstParams->y_th_hi);
				/* alpha gain(n-lvl gaining), gm gain */
				PE_P0R_L9B0_Rd04(tnr_ctrl_04,	nt_lvl_gain_gm_y0,	pstParams->nt_lvl_gain_gm_y0,\
												nt_lvl_gain_gm_x0,	pstParams->nt_lvl_gain_gm_x0,\
												nt_lvl_gain_gm_y1,	pstParams->nt_lvl_gain_gm_y1,\
												nt_lvl_gain_gm_x1,	pstParams->nt_lvl_gain_gm_x1);
				PE_P0R_L9B0_Rd04(tnr_ctrl_12,	nt_lvl_gain_gm_y2,	pstParams->nt_lvl_gain_gm_y2,\
												nt_lvl_gain_gm_x2,	pstParams->nt_lvl_gain_gm_x2,\
												nt_lvl_gain_gm_y3,	pstParams->nt_lvl_gain_gm_y3,\
												nt_lvl_gain_gm_x3,	pstParams->nt_lvl_gain_gm_x3);
				/* alpha gain(n-lvl gaining), avg st gain */
				PE_P0R_L9B0_Rd04(tnr_ctrl_28,	nt_lvl_gain_st_y0,	pstParams->nt_lvl_gain_st_y0,\
												nt_lvl_gain_st_x0,	pstParams->nt_lvl_gain_st_x0,\
												nt_lvl_gain_st_y1,	pstParams->nt_lvl_gain_st_y1,\
												nt_lvl_gain_st_x1,	pstParams->nt_lvl_gain_st_x1);
				PE_P0R_L9B0_Rd04(tnr_ctrl_30,	nt_lvl_gain_st_y2,	pstParams->nt_lvl_gain_st_y2,\
												nt_lvl_gain_st_x2,	pstParams->nt_lvl_gain_st_x2,\
												nt_lvl_gain_st_y3,	pstParams->nt_lvl_gain_st_y3,\
												nt_lvl_gain_st_x3,	pstParams->nt_lvl_gain_st_x3);
				/* alpha lut(n-lvl apply) */
				PE_P0R_L9B0_Rd04(tnr_ctrl_07,	alpha_lut_ind0,		pstParams->alpha_lut_ind0,\
												alpha_lut_ind1,		pstParams->alpha_lut_ind1,\
												alpha_lut_ind2,		pstParams->alpha_lut_ind2,\
												alpha_lut_ind3,		pstParams->alpha_lut_ind3);
				PE_P0R_L9B0_Rd03(tnr_ctrl_08,	alpha_lut_ind4,		pstParams->alpha_lut_ind4,\
												alpha_lut_ind5,		pstParams->alpha_lut_ind5,\
												alpha_lut_ind6,		pstParams->alpha_lut_ind6);
				PE_P0R_L9B0_Rd01(tnr_ctrl_02,	alpha_lut_ind7,		pstParams->alpha_lut_ind7);
				/* adjust alpha(alpha gaining), adjust alpha */
				PE_P0R_L9B0_Rd01(tnr_ctrl_05,	adj_alpha_nt_lvl_th0,	pstParams->adj_alpha_nt_lvl_th0);
				PE_P0R_L9B0_Rd01(tnr_ctrl_08,	adj_alpha_nt_lvl_th1,	pstParams->adj_alpha_nt_lvl_th1);
				PE_P0R_L9B0_Rd02(tnr_ctrl_06,	adj_alpha_k0,			pstParams->adj_alpha_k0,\
												adj_alpha_k1,			pstParams->adj_alpha_k1);
				/* adjust alpha(alpha gaining), s. var --> WA */
				PE_P0R_L9B0_Rd04(tnr_ctrl_03,	wa_y0,					pstParams->wa_y0,\
												wa_y1,					pstParams->wa_y1,\
												wa_x0,					pstParams->wa_x0,\
												wa_x1,					pstParams->wa_x1);
				PE_P0R_L9B0_Rd04(tnr_ctrl_29,	wa_y2,					pstParams->wa_y2,\
												wa_y3,					pstParams->wa_y3,\
												wa_x2,					pstParams->wa_x2,\
												wa_x3,					pstParams->wa_x3);
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_NRD_DBG_PRINT("get pstParams[%d] \n"\
			"motion histo                : %d, %d, %d, %d\n"\
			"alpha gain - gm gain        : %d,  [y,x]:%d,%d, %d,%d, %d,%d, %d,%d\n"\
			"alpha gain - avg st gain    : %d,  [y,x]:%d,%d, %d,%d, %d,%d, %d,%d\n"\
			"alpha lut                   : %d, %d, %d, %d, %d, %d, %d, %d\n"\
			"adjust alpha                : %d, %d, %d, %d\n"\
			"adjust alpha - s. var to wa : %d,  [x]:%d,%d,%d,%d,  [y]:%d,%d,%d,%d\n",\
			pstParams->win_id,\
			/* motion histo(n-lvl estimate) */
			pstParams->calc_motion_flt_enable,pstParams->calc_motion_flt_type,\
			pstParams->y_th_lo,pstParams->y_th_hi,\
			/* alpha gain(n-lvl gaining), gm gain */
			pstParams->nt_lvl_adjust_gm_enable,pstParams->nt_lvl_gain_gm_y0,pstParams->nt_lvl_gain_gm_x0,\
			pstParams->nt_lvl_gain_gm_y1,pstParams->nt_lvl_gain_gm_x1,pstParams->nt_lvl_gain_gm_y2,\
			pstParams->nt_lvl_gain_gm_x2,pstParams->nt_lvl_gain_gm_y3,pstParams->nt_lvl_gain_gm_x3,\
			/* alpha gain(n-lvl gaining), avg st gain */
			pstParams->nt_lvl_adjust_avg_ts_enable,pstParams->nt_lvl_gain_st_y0,\
			pstParams->nt_lvl_gain_st_x0,pstParams->nt_lvl_gain_st_y1,pstParams->nt_lvl_gain_st_x1,\
			pstParams->nt_lvl_gain_st_y2,pstParams->nt_lvl_gain_st_x2,pstParams->nt_lvl_gain_st_y3,\
			pstParams->nt_lvl_gain_st_x3,\
			/* alpha lut(n-lvl apply) */
			pstParams->alpha_lut_ind0,pstParams->alpha_lut_ind1,pstParams->alpha_lut_ind2,\
			pstParams->alpha_lut_ind3,pstParams->alpha_lut_ind4,pstParams->alpha_lut_ind5,\
			pstParams->alpha_lut_ind6,pstParams->alpha_lut_ind7,\
			/* adjust alpha(alpha gaining), adjust alpha */
			pstParams->adj_alpha_nt_lvl_th0,pstParams->adj_alpha_nt_lvl_th1,\
			pstParams->adj_alpha_k0,pstParams->adj_alpha_k1,\
			/* adjust alpha(alpha gaining), s. var --> WA */
			pstParams->wa_enable,pstParams->wa_x0,pstParams->wa_x1,pstParams->wa_x2,pstParams->wa_x3,\
			pstParams->wa_y0,pstParams->wa_y1,pstParams->wa_y2,pstParams->wa_y3);
	} while (0);
	return ret;
}
static int PE_NRD_SetTnrColorRegionCtrl(LX_PE_NRD_TNR_CRGN_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_NRD_CHECK_CODE(pstParams->gain_rgn[0]>255,break,\
			"[%s,%d] gain_rgn[0]:%d is over.\n",__F__,__L__,pstParams->gain_rgn[0]);
		PE_NRD_CHECK_CODE(pstParams->gain_rgn[1]>255,break,\
			"[%s,%d] gain_rgn[1]:%d is over.\n",__F__,__L__,pstParams->gain_rgn[1]);
		PE_NRD_CHECK_CODE(pstParams->gain_rgn[2]>255,break,\
			"[%s,%d] gain_rgn[2]:%d is over.\n",__F__,__L__,pstParams->gain_rgn[2]);
		PE_NRD_CHECK_CODE(pstParams->gain_rgn[3]>255,break,\
			"[%s,%d] gain_rgn[3]:%d is over.\n",__F__,__L__,pstParams->gain_rgn[3]);
		PE_NRD_DBG_PRINT("set pstParams[%d] : gain:%d,%d,%d,%d\n",pstParams->win_id,\
			pstParams->gain_rgn[0],pstParams->gain_rgn[1],pstParams->gain_rgn[2],pstParams->gain_rgn[3]);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P0L_L9B0_Wr04(tnr_ctrl_27,	crgn_gain_rgn0,	GET_BITS(pstParams->gain_rgn[0],0,8),\
												crgn_gain_rgn1,	GET_BITS(pstParams->gain_rgn[1],0,8),\
												crgn_gain_rgn2,	GET_BITS(pstParams->gain_rgn[2],0,8),\
												crgn_gain_rgn3,	GET_BITS(pstParams->gain_rgn[3],0,8));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9B0_Wr04(tnr_ctrl_27,	crgn_gain_rgn0,	GET_BITS(pstParams->gain_rgn[0],0,8),\
												crgn_gain_rgn1,	GET_BITS(pstParams->gain_rgn[1],0,8),\
												crgn_gain_rgn2,	GET_BITS(pstParams->gain_rgn[2],0,8),\
												crgn_gain_rgn3,	GET_BITS(pstParams->gain_rgn[3],0,8));
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	} while (0);
	return ret;
}
static int PE_NRD_GetTnrColorRegionCtrl(LX_PE_NRD_TNR_CRGN_T *pstParams)
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
				PE_P0L_L9B0_Rd04(tnr_ctrl_27,	crgn_gain_rgn0,	pstParams->gain_rgn[0],\
												crgn_gain_rgn1,	pstParams->gain_rgn[1],\
												crgn_gain_rgn2,	pstParams->gain_rgn[2],\
												crgn_gain_rgn3,	pstParams->gain_rgn[3]);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P0R_L9B0_Rd04(tnr_ctrl_27,	crgn_gain_rgn0,	pstParams->gain_rgn[0],\
												crgn_gain_rgn1,	pstParams->gain_rgn[1],\
												crgn_gain_rgn2,	pstParams->gain_rgn[2],\
												crgn_gain_rgn3,	pstParams->gain_rgn[3]);
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
		PE_NRD_DBG_PRINT("get pstParams[%d] : gain:%d,%d,%d,%d\n",pstParams->win_id,\
			pstParams->gain_rgn[0],pstParams->gain_rgn[1],pstParams->gain_rgn[2],pstParams->gain_rgn[3]);
	} while (0);
	return ret;
}
int PE_NRD_SetMeasureWinCtrl(void)
{
	int ret = RET_OK;
	UINT32 h_size[2],v_size[2];
	static UINT32 h_ofst[2]={0,0};
	static UINT32 v_ofst[2]={0,0};
	static UINT32 cur_x0[2]={0,0};
	static UINT32 cur_y0[2]={0,0};
	static UINT32 cur_x1[2]={0,0};
	static UINT32 cur_y1[2]={0,0};
	static UINT32 pre_h_size[2]={0xffff,0xffff};
	static UINT32 pre_v_size[2]={0xffff,0xffff};
	static UINT32 count = 0;
	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_P0L_L9B0_Rd02(pe0_src_size,	hsize,	h_size[0],\
											vsize,	v_size[0]);
			PE_P0R_L9B0_Rd02(pe0_src_size,	hsize,	h_size[1],\
											vsize,	v_size[1]);
			if(pre_h_size[0]!=h_size[0] || pre_v_size[0]!=v_size[0])
			{
				h_ofst[0]=h_size[0]>>4;	// 1/16=6.25%
				v_ofst[0]=v_size[0]>>4;	// 1/16=6.25%
				cur_x0[0]=h_ofst[0];
				cur_y0[0]=v_ofst[0];
				cur_x1[0]=h_size[0]-h_ofst[0];
				cur_y1[0]=v_size[0]-v_ofst[0];
				PE_P0L_L9B0_Wr02(tnr_ctrl_14,	x0,	cur_x0[0],\
												y0,	cur_y0[0]);
				PE_P0L_L9B0_Wr02(tnr_ctrl_15,	x1,	cur_x1[0],\
												y1,	cur_y1[0]);
				PE_NRD_DBG_PRINT("[L][size:%4d,%4d -> %4d,%4d][ofst:%4d,%4d][x,y:%4d,%4d,%4d,%4d]\n\n",\
					pre_h_size[0],pre_v_size[0],h_size[0],v_size[0],h_ofst[0],v_ofst[0],\
					cur_x0[0],cur_y0[0],cur_x1[0],cur_y1[0]);
				pre_h_size[0]=h_size[0];
				pre_v_size[0]=v_size[0];
			}
			if(pre_h_size[1]!=h_size[1] || pre_v_size[1]!=v_size[1])
			{
				h_ofst[1]=h_size[1]>>4;	// 1/16=6.25%
				v_ofst[1]=v_size[1]>>4;	// 1/16=6.25%
				cur_x0[1]=h_ofst[1];
				cur_y0[1]=v_ofst[1];
				cur_x1[1]=h_size[1]-h_ofst[1];
				cur_y1[1]=v_size[1]-v_ofst[1];
				PE_P0R_L9B0_Wr02(tnr_ctrl_14,	x0,	cur_x0[1],\
												y0,	cur_y0[1]);
				PE_P0R_L9B0_Wr02(tnr_ctrl_15,	x1,	cur_x1[1],\
												y1,	cur_y1[1]);
				PE_NRD_DBG_PRINT("[R][size:%4d,%4d -> %4d,%4d][ofst:%4d,%4d][x,y:%4d,%4d,%4d,%4d]\n\n",\
					pre_h_size[1],pre_v_size[1],h_size[1],v_size[1],h_ofst[1],v_ofst[1],\
					cur_x0[1],cur_y0[1],cur_x1[1],cur_y1[1]);
				pre_h_size[1]=h_size[1];
				pre_v_size[1]=v_size[1];
			}
			if(_g_nrd_trace)
			{
				if(count>240)
				{
					PE_NRD_DBG_PRINT("[state][L][size:%4d,%4d][ofst:%4d,%4d][x,y:%4d,%4d,%4d,%4d]\n",\
						h_size[0],v_size[0],h_ofst[0],v_ofst[0],cur_x0[0],cur_y0[0],cur_x1[0],cur_y1[0]);
					PE_NRD_DBG_PRINT("[state][R][size:%4d,%4d][ofst:%4d,%4d][x,y:%4d,%4d,%4d,%4d]\n\n",\
						h_size[1],v_size[1],h_ofst[1],v_ofst[1],cur_x0[1],cur_y0[1],cur_x1[1],cur_y1[1]);
					count=1;
				}
				count++;
			}
		}
		else
		{
			PE_NRD_DBG_PRINT("nothing to do.\n");
		}
	}while(0);

	return ret;
}

