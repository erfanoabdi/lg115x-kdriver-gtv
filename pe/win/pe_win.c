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


/** @file pe_win.c
 *
 *  driver for picture enhance window control functions. ( used only within kdriver )
 *	- control window of each pe module
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
#include "pe_win.h"
#include "pe_win_coeff.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define PE_WIN_H_MAX	1920
#define PE_WIN_V_MAX	1080
#define PE_WIN_H_HALF	960
#define PE_WIN_V_HALF	540

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_WIN_ERROR	printk

#define PE_WIN_DBG_PRINT(fmt,args...)	if(_g_win_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_WIN_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_WIN_ERROR(fmt,##args);_action;}}

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
extern DE_OVR_REG_L9_T gDE_OVR_L9;
extern DE_DED_REG_L9_T gDE_DED_L9;

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int PE_WIN_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_WIN_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_WIN_SetEnhancingWindow(LX_PE_WIN_CTRL_T *pstParams);
static int PE_WIN_GetEnhancingWindow(LX_PE_WIN_CTRL_T *pstParams);
static int PE_WIN_SetAutoDetectInfo(LX_PE_WIN_D3D_INFO_T *pstParams);
static int PE_WIN_GetAutoDetectInfo(LX_PE_WIN_D3D_INFO_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_win_trace=0x0;	//default should be off.
static LX_PE_WIN_CTRL_T _g_win_ctrl[LX_PE_WIN_NUM];
static PE_WIN_SETTINGS_T _g_win_settings[LX_PE_WIN_NUM];
static LX_PE_WIN_D3D_INFO_T _g_win_detect_info;
static UINT32 _g_win_force_ov_win_sync=0x1;	/* force to use ov out win sync. */

/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * init pe window
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_WIN_Init(void)
{
	int ret = RET_OK;
	UINT8 i;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_WIN_DBG_PRINT("init global values.\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_win_ctrl[i].win_id	= i;
				_g_win_ctrl[i].out_type	= LX_PE_OUT_NUM;
				_g_win_ctrl[i].pos_x	= 0xffff;
				_g_win_ctrl[i].pos_y	= 0xffff;
				_g_win_ctrl[i].size_w	= 0xffff;
				_g_win_ctrl[i].size_h	= 0xffff;
				_g_win_ctrl[i].ofs_x	= 0xffff;
				_g_win_ctrl[i].ofs_y	= 0xffff;
				_g_win_settings[i].win_id	= i;
				_g_win_settings[i].out_type	= LX_PE_OUT_NUM;
				_g_win_settings[i].act_x0	= 0xffff;
				_g_win_settings[i].act_y0	= 0xffff;
				_g_win_settings[i].act_x1	= 0xffff;
				_g_win_settings[i].act_y1	= 0xffff;
			}
			_g_win_detect_info.top.pos_x0 = 0;
			_g_win_detect_info.top.pos_y0 = 0;
			_g_win_detect_info.top.pos_x1 = PE_WIN_H_MAX;
			_g_win_detect_info.top.pos_y1 = PE_WIN_V_HALF;
			_g_win_detect_info.bottom.pos_x0 = 0;
			_g_win_detect_info.bottom.pos_y0 = PE_WIN_V_HALF;
			_g_win_detect_info.bottom.pos_x1 = PE_WIN_H_MAX;
			_g_win_detect_info.bottom.pos_y1 = PE_WIN_V_MAX;
			_g_win_detect_info.left.pos_x0 = 0;
			_g_win_detect_info.left.pos_y0 = 0;
			_g_win_detect_info.left.pos_x1 = PE_WIN_H_HALF;
			_g_win_detect_info.left.pos_y1 = PE_WIN_V_MAX;
			_g_win_detect_info.right.pos_x0 = PE_WIN_H_HALF;
			_g_win_detect_info.right.pos_y0 = 0;
			_g_win_detect_info.right.pos_x1 = PE_WIN_H_MAX;
			_g_win_detect_info.right.pos_y1 = PE_WIN_V_MAX;
			_g_win_detect_info.tb_ratio = 1024;
			_g_win_detect_info.ss_ratio = 1024;
			PE_WIN_DBG_PRINT("pe1 win1,2 enable, inside.\n");
	        PE_P1L_L9B0_Wr02(pe1_l_win1_ctrl_00,win_en,			0x1,\
            									win_outside,	0x0);
	        PE_P1L_L9B0_Wr02(pe1_l_win2_ctrl_00,win_en,			0x1,\
            									win_outside,	0x0);
	        PE_P1R_L9B0_Wr02(pe1_r_win1_ctrl_00,win_en,			0x1,\
            									win_outside,	0x0);
	        PE_P1R_L9B0_Wr02(pe1_r_win2_ctrl_00,win_en,			0x1,\
												win_outside,	0x0);

			if(_g_win_force_ov_win_sync)
			{
				PE_P1R_L9B0_Wr01(pe1_r_top_ctrl_02,	disp_active_mux,	0x1);	//disp_va, disp_ha -> (TPG) FILM1
				DE_DED_L9B_RdFL(ded_def_disp_sync_sel);
				DE_DED_L9B_Wr01(ded_def_disp_sync_sel, ded_def_pe1_r,	0x0);	//ded to def 0
				DE_DED_L9B_WrFL(ded_def_disp_sync_sel);
				DE_DED_L9B_RdFL(ded_dly_disp_sync_params);
				DE_DED_L9B_Wr01(ded_dly_disp_sync_params, h_dly,		0xc3);	//pixel delay : 195
				DE_DED_L9B_WrFL(ded_dly_disp_sync_params);
			}
			else
			{
				PE_P1R_L9B0_Wr01(pe1_r_top_ctrl_02,	disp_active_mux,	0x0);	//pe1_va_in, pe1_ha_in-> (TPG) FILM1
				DE_DED_L9B_RdFL(ded_def_disp_sync_sel);
				DE_DED_L9B_Wr01(ded_def_disp_sync_sel, ded_def_pe1_r,	0x1);	//ded to def 1
				DE_DED_L9B_WrFL(ded_def_disp_sync_sel);
				DE_DED_L9B_RdFL(ded_dly_disp_sync_params);
				DE_DED_L9B_Wr01(ded_dly_disp_sync_params, h_dly,		0x13d);	//pixel delay : 317
				DE_DED_L9B_WrFL(ded_dly_disp_sync_params);
			}
		}
		else
		{
			PE_WIN_DBG_PRINT("init global values.\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_win_ctrl[i].win_id	= i;
				_g_win_ctrl[i].out_type	= LX_PE_OUT_NUM;
				_g_win_ctrl[i].pos_x	= 0xffff;
				_g_win_ctrl[i].pos_y	= 0xffff;
				_g_win_ctrl[i].size_w	= 0xffff;
				_g_win_ctrl[i].size_h	= 0xffff;
				_g_win_ctrl[i].ofs_x	= 0xffff;
				_g_win_ctrl[i].ofs_y	= 0xffff;
			}
			PE_WIN_DBG_PRINT("pe1 win enable, inside.\n");
	        PE_P1L_L9A0_Wr02(pe1_l_win_ctrl_00, win_en,			0x1,\
												win_outside,	0x0);
	        PE_P1R_L9A0_Wr02(pe1_r_win_ctrl_00, win_en,			0x1,\
												win_outside,	0x0);
			PE_WIN_DBG_PRINT("bbd enable & manual setting.\n");
            PE_P1L_L9A0_Wr02(pe1_l_bbd2d_ctrl_0,black_boundary_detection_enable,0x1,\
												mode_selection,					0x1);
            PE_P1R_L9A0_Wr02(pe1_r_bbd2d_ctrl_0,black_boundary_detection_enable,0x1,\
												mode_selection,					0x1);
		}
	}while(0);

	return ret;
}

/**
 * standard function of win setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_WIN_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_WIN_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_WIN_CTRL_T):
		{
			LX_PE_WIN_CTRL_T *pRcvData;
			LX_PE_WIN_CTRL_T stParam;
			do{
				PE_WIN_DBG_PRINT("S LX_PE_WIN_CTRL_T\n");
				/* data size check */
				PE_WIN_CHECK_CODE(datasize!=sizeof(LX_PE_WIN_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_WIN_CTRL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_WIN_SetEnhancingWindow(&stParam);
				PE_WIN_CHECK_CODE(ret,break,"[%s,%d] PE_WIN_SetEnhancingWindow() error.\n",__F__,__L__);
				PE_WIN_DBG_PRINT("E LX_PE_WIN_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_WIN_D3D_INFO_T):
		{
			LX_PE_WIN_D3D_INFO_T *pRcvData;
			LX_PE_WIN_D3D_INFO_T stParam;
			do{
				PE_WIN_DBG_PRINT("S LX_PE_WIN_D3D_INFO_T\n");
				/* data size check */
				PE_WIN_CHECK_CODE(datasize!=sizeof(LX_PE_WIN_D3D_INFO_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_WIN_D3D_INFO_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_WIN_SetAutoDetectInfo(&stParam);
				PE_WIN_CHECK_CODE(ret,break,"[%s,%d] PE_WIN_SetAutoDetectInfo() error.\n",__F__,__L__);
				PE_WIN_DBG_PRINT("E LX_PE_WIN_D3D_INFO_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_WIN_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_WIN_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_WIN_SetDefaultSettings(&stParam);
				PE_WIN_CHECK_CODE(ret,break,"[%s,%d] PE_WIN_SetDefaultSettings() error.\n",__F__,__L__);
				PE_WIN_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_WIN_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_WIN_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_WIN_SetDebugSettings(&stParam);
				PE_WIN_CHECK_CODE(ret,break,"[%s,%d] PE_WIN_SetDebugSettings() error.\n",__F__,__L__);
				PE_WIN_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_WIN_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of win getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_WIN_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_WIN_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_WIN_CTRL_T):
		{
			LX_PE_WIN_CTRL_T *pRcvData;
			LX_PE_WIN_CTRL_T stParam;
			memset0(stParam);
			do{
				PE_WIN_DBG_PRINT("S LX_PE_WIN_CTRL_T\n");
				/* data size check */
				PE_WIN_CHECK_CODE(datasize!=sizeof(LX_PE_WIN_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_WIN_CTRL_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_WIN_GetEnhancingWindow(&stParam);
				PE_WIN_CHECK_CODE(ret,break,"[%s,%d] PE_WIN_GetEnhancingWindow() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_WIN_DBG_PRINT("E LX_PE_WIN_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_WIN_D3D_INFO_T):
		{
			LX_PE_WIN_D3D_INFO_T *pRcvData;
			LX_PE_WIN_D3D_INFO_T stParam;
			memset0(stParam);
			do{
				PE_WIN_DBG_PRINT("S LX_PE_WIN_D3D_INFO_T\n");
				/* data size check */
				PE_WIN_CHECK_CODE(datasize!=sizeof(LX_PE_WIN_D3D_INFO_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_WIN_D3D_INFO_T *)data;
				ret = PE_WIN_GetAutoDetectInfo(&stParam);
				PE_WIN_CHECK_CODE(ret,break,"[%s,%d] PE_WIN_GetAutoDetectInfo() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_WIN_DBG_PRINT("E LX_PE_WIN_D3D_INFO_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_WIN_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_WIN_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_WIN_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);
	}while(0);
	return ret;
}
static int PE_WIN_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_WIN_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(WIN)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_win_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(WIN)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(WIN)))
				{
					PE_WIN_DBG_PRINT("pe1 win1,2 : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9B0_Wr01(pe1_l_win1_ctrl_00, win_en,	0x0);
						PE_P1L_L9B0_Wr01(pe1_l_win2_ctrl_00, win_en,	0x0);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9B0_Wr01(pe1_r_win1_ctrl_00, win_en,	0x0);
						PE_P1R_L9B0_Wr01(pe1_r_win2_ctrl_00, win_en,	0x0);
					}
				}
				else
				{
					PE_WIN_DBG_PRINT("pe1 win1,2 : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9B0_Wr01(pe1_l_win1_ctrl_00, win_en,	0x1);
						PE_P1L_L9B0_Wr01(pe1_l_win2_ctrl_00, win_en,	0x1);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9B0_Wr01(pe1_r_win1_ctrl_00, win_en,	0x1);
						PE_P1R_L9B0_Wr01(pe1_r_win2_ctrl_00, win_en,	0x1);
					}
				}
			}
			else
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(WIN)))
				{
					PE_WIN_DBG_PRINT("pe1 win, bbd en, : disable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9A0_Wr01(pe1_l_win_ctrl_00, win_en,		0x0);
						PE_P1L_L9A0_Wr01(pe1_l_bbd2d_ctrl_0, black_boundary_detection_enable,	0x0);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9A0_Wr01(pe1_r_win_ctrl_00, win_en,		0x0);
						PE_P1R_L9A0_Wr01(pe1_r_bbd2d_ctrl_0, black_boundary_detection_enable,	0x0);
					}
				}
				else
				{
					PE_WIN_DBG_PRINT("pe1 win, bbd en, : enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9A0_Wr01(pe1_l_win_ctrl_00, win_en,		0x1);
						PE_P1L_L9A0_Wr01(pe1_l_bbd2d_ctrl_0, black_boundary_detection_enable,	0x1);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9A0_Wr01(pe1_r_win_ctrl_00, win_en,		0x1);
						PE_P1R_L9A0_Wr01(pe1_r_bbd2d_ctrl_0, black_boundary_detection_enable,	0x1);
					}
				}
			}
		}
	}while(0);
	return ret;
}
static int PE_WIN_SetEnhancingWindow(LX_PE_WIN_CTRL_T *pstParams)
{
	int ret = RET_OK;
	static UINT32 prev_ov_r_out_sync=0xff;
	UINT32 ov_r_out_sync;
	UINT32 x0,y0,x1,y1;
	UINT32 act_x0[2],act_y0[2],act_x1[2],act_y1[2];
	UINT32 win0_x0,win0_x1,win0_y0,win0_y1;
	UINT32 win1_x0,win1_x1,win1_y0,win1_y1;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_WIN_CHECK_CODE(pstParams->out_type>=LX_PE_OUT_NUM,break,\
			"[%s,%d] out_type(%d) error.\n",__F__,__L__,pstParams->out_type);
		PE_WIN_CHECK_CODE((pstParams->pos_x+pstParams->size_w)>PE_WIN_H_MAX,break,\
			"[%s,%d] x(%d),w(%d) invalid.\n",__F__,__L__,pstParams->pos_x,pstParams->size_w);
		if(pstParams->out_type!=LX_PE_OUT_UD)
		{
			PE_WIN_CHECK_CODE((pstParams->pos_y+pstParams->size_h)>PE_WIN_V_MAX,break,\
				"[%s,%d][no UD] y(%d),h(%d) invalid.\n",__F__,__L__,pstParams->pos_y,pstParams->size_h);
		}
		else
		{
			PE_WIN_CHECK_CODE((pstParams->pos_y+pstParams->size_h)>(PE_WIN_V_MAX<<1),break,\
				"[%s,%d][UD] y(%d),h(%d) invalid.\n",__F__,__L__,pstParams->pos_y,pstParams->size_h);
		}
		PE_WIN_CHECK_CODE(pstParams->size_w<(pstParams->ofs_x<<1),break,\
			"[%s,%d] w(%d),ofs_x(%d) invalid.\n",__F__,__L__,pstParams->size_w,pstParams->ofs_x);
		PE_WIN_CHECK_CODE(pstParams->size_h<(pstParams->ofs_y<<1),break,\
			"[%s,%d] h(%d),ofs_y(%d) invalid.\n",__F__,__L__,pstParams->size_h,pstParams->ofs_y);
		PE_WIN_DBG_PRINT("set pstParams[%d] : o,i_type:%d,%d, x,y,w,h : %d,%d,%d,%d, ofs:%d,%d\n",\
			pstParams->win_id,pstParams->out_type,pstParams->in_type,pstParams->pos_x,pstParams->pos_y,\
			pstParams->size_w,pstParams->size_h,pstParams->ofs_x,pstParams->ofs_y);

		x0 = pstParams->pos_x;
		y0 = pstParams->pos_y;
		x1 = x0+pstParams->size_w-1;
		y1 = y0+pstParams->size_h-1;
		if(pstParams->out_type==LX_PE_OUT_TB)
		{
			act_x0[0] = x0;
			act_y0[0] = (y0>>1);
			act_x1[0] = act_x0[0]+pstParams->size_w-1;
			act_y1[0] = act_y0[0]+(pstParams->size_h>>1)-1;

			win0_x0 = act_x0[0]+pstParams->ofs_x;
			win0_y0 = act_y0[0]+pstParams->ofs_y;
			win0_x1 = act_x1[0]-pstParams->ofs_x;
			win0_y1 = act_y1[0]-pstParams->ofs_y;

			act_x0[1] = act_x0[0];
			act_y0[1] = act_y0[0]+PE_WIN_V_HALF;
			act_x1[1] = act_x1[0];
			act_y1[1] = act_y1[0]+PE_WIN_V_HALF;

			win1_x0 = win0_x0;
			win1_y0 = win0_y0+PE_WIN_V_HALF;
			win1_x1 = win0_x1;
			win1_y1 = win0_y1+PE_WIN_V_HALF;
		}
		else if(pstParams->out_type==LX_PE_OUT_SS)
		{
			act_x0[0] = (pstParams->pos_x>>1);
			act_y0[0] = y0;
			act_x1[0] = act_x0[0]+(pstParams->size_w>>1)-1;
			act_y1[0] = y1;

			win0_x0 = act_x0[0]+pstParams->ofs_x;
			win0_y0 = act_y0[0]+pstParams->ofs_y;
			win0_x1 = act_x1[0]-pstParams->ofs_x;
			win0_y1 = act_y1[0]-pstParams->ofs_y;

			act_x0[1] = act_x0[0]+PE_WIN_H_HALF;
			act_y0[1] = act_y0[0];
			act_x1[1] = act_x1[0]+PE_WIN_H_HALF;
			act_y1[1] = act_y1[0];

			win1_x0 = win0_x0+PE_WIN_H_HALF;
			win1_y0 = win0_y0;
			win1_x1 = win0_x1+PE_WIN_H_HALF;
			win1_y1 = win0_y1;
		}
		else
		{
			act_x0[0] = x0;
			act_y0[0] = y0;
			act_x1[0] = x1;
			act_y1[0] = y1;

			win0_x0 = act_x0[0]+pstParams->ofs_x;
			win0_y0 = act_y0[0]+pstParams->ofs_y;
			win0_x1 = act_x1[0]-pstParams->ofs_x;
			win0_y1 = act_y1[0]-pstParams->ofs_y;

 			act_x0[1] = act_x0[0];
			act_y0[1] = act_y0[0];
			act_x1[1] = act_x1[0];
			act_y1[1] = act_y1[0];
			win1_x0 = win1_y0 = win1_x1 = win1_y1 = 0;
		}

		PE_WIN_DBG_PRINT("[wid:%d] set x0,y0,x1,y1:%d,%d,%d,%d\n"\
			"act     : w0:%d,%d,%d,%d, w1:%d,%d,%d,%d\n"\
			"act+ofs : w0:%d,%d,%d,%d, w1:%d,%d,%d,%d\n",\
			pstParams->win_id,x0,y0,x1,y1,\
			act_x0[0],act_y0[0],act_x1[0],act_y1[0],act_x0[1],act_y0[1],act_x1[1],act_y1[1],\
			win0_x0,win0_y0,win0_x1,win0_y1,win1_x0,win1_y0,win1_x1,win1_y1);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			/**************************************************************/
			/* pe sync control for R path */
			/**************************************************************/
			switch(pstParams->out_type)
			{
				case LX_PE_OUT_2D:
					ov_r_out_sync = 0;	//win sync
					break;
				case LX_PE_OUT_TB:
				case LX_PE_OUT_SS:
				case LX_PE_OUT_FS:
				case LX_PE_OUT_DUAL:
				case LX_PE_OUT_PIP:
					ov_r_out_sync = 1;	//display sync
					break;
				default:
					ov_r_out_sync = 0;
					break;
			}
			if(_g_win_force_ov_win_sync)
			{
				ov_r_out_sync = 0;
			}
			if(prev_ov_r_out_sync != ov_r_out_sync)
			{
				DE_OVR_L9B0_RdFL(ovr_ov_ctrl0);
				DE_OVR_L9B0_Wr01(ovr_ov_ctrl0, ov_out_sync_mux, ov_r_out_sync? 0x1:0x0);
				DE_OVR_L9B0_WrFL(ovr_ov_ctrl0);
				PE_P1R_L9B0_Wr01(pe1_r_win1_ctrl_00, win_en,	ov_r_out_sync? 0x1:0x0);
				prev_ov_r_out_sync = ov_r_out_sync;
			}
			/**************************************************************/

			if(PE_CHECK_MAIN(pstParams->win_id))
			{
                PE_P1L_L9B0_Wr02(pe1_l_win1_ctrl_01,win_w0_x0,	win0_x0,\
                									win_w0_y0,	win0_y0);
                PE_P1L_L9B0_Wr02(pe1_l_win1_ctrl_02,win_w0_x1,	win0_x1,\
                									win_w0_y1,	win0_y1);
                PE_P1L_L9B0_Wr02(pe1_l_win1_ctrl_03,win_w1_x0,	win1_x0,\
                									win_w1_y0,	win1_y0);
                PE_P1L_L9B0_Wr02(pe1_l_win1_ctrl_04,win_w1_x1,	win1_x1,\
                									win_w1_y1,	win1_y1);
                PE_P1L_L9B0_Wr02(pe1_l_win2_ctrl_01,win_w0_x0,	act_x0[0],\
                									win_w0_y0,	act_y0[0]);
                PE_P1L_L9B0_Wr02(pe1_l_win2_ctrl_02,win_w0_x1,	act_x1[0],\
                									win_w0_y1,	act_y1[0]);
                PE_P1L_L9B0_Wr02(pe1_l_win2_ctrl_03,win_w1_x0,	act_x0[1],\
                									win_w1_y0,	act_y0[1]);
                PE_P1L_L9B0_Wr02(pe1_l_win2_ctrl_04,win_w1_x1,	act_x1[1],\
                									win_w1_y1,	act_y1[1]);
                PE_P1L_L9B0_Wr02(pe1_l_apl_ctrl_00,	apl_win_ctrl_x0, act_x0[0],\
                									apl_win_ctrl_y0, act_y0[0]);
                PE_P1L_L9B0_Wr02(pe1_l_apl_ctrl_01,	apl_win_ctrl_x1, act_x1[0],\
                									apl_win_ctrl_y1, act_y1[0]);
				_g_win_ctrl[LX_PE_WIN_MAIN].out_type= pstParams->out_type;
				_g_win_ctrl[LX_PE_WIN_MAIN].in_type	= pstParams->in_type;
				_g_win_ctrl[LX_PE_WIN_MAIN].pos_x	= pstParams->pos_x;
				_g_win_ctrl[LX_PE_WIN_MAIN].pos_y	= pstParams->pos_y;
				_g_win_ctrl[LX_PE_WIN_MAIN].size_w	= pstParams->size_w;
				_g_win_ctrl[LX_PE_WIN_MAIN].size_h	= pstParams->size_h;
				_g_win_ctrl[LX_PE_WIN_MAIN].ofs_x	= pstParams->ofs_x;
				_g_win_ctrl[LX_PE_WIN_MAIN].ofs_y	= pstParams->ofs_y;
				_g_win_settings[LX_PE_WIN_MAIN].out_type= pstParams->out_type;
				_g_win_settings[LX_PE_WIN_MAIN].in_type	= pstParams->in_type;
				_g_win_settings[LX_PE_WIN_MAIN].act_x0	= act_x0[0];
				_g_win_settings[LX_PE_WIN_MAIN].act_y0	= act_y0[0];
				_g_win_settings[LX_PE_WIN_MAIN].act_x1	= act_x1[0];
				_g_win_settings[LX_PE_WIN_MAIN].act_y1	= act_y1[0];
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				if(ov_r_out_sync)
				{
	                PE_P1R_L9B0_Wr02(pe1_r_win1_ctrl_01,win_w0_x0,	win0_x0,\
	                									win_w0_y0,	win0_y0);
	                PE_P1R_L9B0_Wr02(pe1_r_win1_ctrl_02,win_w0_x1,	win0_x1,\
	                									win_w0_y1,	win0_y1);
	                PE_P1R_L9B0_Wr02(pe1_r_win1_ctrl_03,win_w1_x0,	win1_x0,\
	                									win_w1_y0,	win1_y0);
	                PE_P1R_L9B0_Wr02(pe1_r_win1_ctrl_04,win_w1_x1,	win1_x1,\
	                									win_w1_y1,	win1_y1);
				}
                PE_P1R_L9B0_Wr02(pe1_r_win2_ctrl_01,win_w0_x0,	act_x0[1],\
                									win_w0_y0,	act_y0[1]);
                PE_P1R_L9B0_Wr02(pe1_r_win2_ctrl_02,win_w0_x1,	act_x1[1],\
                									win_w0_y1,	act_y1[1]);
                PE_P1R_L9B0_Wr02(pe1_r_win2_ctrl_03,win_w1_x0,	0x0,\
                									win_w1_y0,	0x0);
                PE_P1R_L9B0_Wr02(pe1_r_win2_ctrl_04,win_w1_x1,	0x0,\
                									win_w1_y1,	0x0);
                PE_P1R_L9B0_Wr02(pe1_r_apl_ctrl_00,	apl_win_ctrl_x0, act_x0[1],\
                									apl_win_ctrl_y0, act_y0[1]);
                PE_P1R_L9B0_Wr02(pe1_r_apl_ctrl_01,	apl_win_ctrl_x1, act_x1[1],\
                									apl_win_ctrl_y1, act_y1[1]);
				_g_win_ctrl[LX_PE_WIN_SUB].out_type	= pstParams->out_type;
				_g_win_ctrl[LX_PE_WIN_SUB].in_type	= pstParams->in_type;
				_g_win_ctrl[LX_PE_WIN_SUB].pos_x	= pstParams->pos_x;
				_g_win_ctrl[LX_PE_WIN_SUB].pos_y	= pstParams->pos_y;
				_g_win_ctrl[LX_PE_WIN_SUB].size_w	= pstParams->size_w;
				_g_win_ctrl[LX_PE_WIN_SUB].size_h	= pstParams->size_h;
				_g_win_ctrl[LX_PE_WIN_SUB].ofs_x	= pstParams->ofs_x;
				_g_win_ctrl[LX_PE_WIN_SUB].ofs_y	= pstParams->ofs_y;
				_g_win_settings[LX_PE_WIN_SUB].out_type	= pstParams->out_type;
				_g_win_settings[LX_PE_WIN_SUB].in_type	= pstParams->in_type;
				_g_win_settings[LX_PE_WIN_SUB].act_x0	= act_x0[1];
				_g_win_settings[LX_PE_WIN_SUB].act_y0	= act_y0[1];
				_g_win_settings[LX_PE_WIN_SUB].act_x1	= act_x1[1];
				_g_win_settings[LX_PE_WIN_SUB].act_y1	= act_y1[1];
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
                PE_P1L_L9A0_Wr02(pe1_l_win_ctrl_01, win_w0_x0,	win0_x0,\
                									win_w0_y0,	win0_y0);
                PE_P1L_L9A0_Wr02(pe1_l_win_ctrl_02, win_w0_x1,	win0_x1,\
                									win_w0_y1,	win0_y1);

                PE_P1L_L9A0_Wr02(pe1_l_win_ctrl_03, win_w1_x0,	win1_x0,\
                									win_w1_y0,	win1_y0);
                PE_P1L_L9A0_Wr02(pe1_l_win_ctrl_04, win_w1_x1,	win1_x1,\
                									win_w1_y1,	win1_y1);

                PE_P1L_L9A0_Wr02(pe1_l_apl_ctrl_00, apl_win_ctrl_x0, x0,\
                									apl_win_ctrl_y0, y0);
                PE_P1L_L9A0_Wr02(pe1_l_apl_ctrl_01, apl_win_ctrl_x1, x1,\
                									apl_win_ctrl_y1, y1);

                PE_P1L_L9A0_Wr02(pe1_l_bbd2d_ctrl_1,bbd_x0, x0,\
                									bbd_y0, y0);
                PE_P1L_L9A0_Wr02(pe1_l_bbd2d_ctrl_2,bbd_x1, x1,\
                									bbd_y1, y1);
				_g_win_ctrl[LX_PE_WIN_MAIN].out_type= pstParams->out_type;
				_g_win_ctrl[LX_PE_WIN_MAIN].in_type	= pstParams->in_type;
				_g_win_ctrl[LX_PE_WIN_MAIN].pos_x	= pstParams->pos_x;
				_g_win_ctrl[LX_PE_WIN_MAIN].pos_y	= pstParams->pos_y;
				_g_win_ctrl[LX_PE_WIN_MAIN].size_w	= pstParams->size_w;
				_g_win_ctrl[LX_PE_WIN_MAIN].size_h	= pstParams->size_h;
				_g_win_ctrl[LX_PE_WIN_MAIN].ofs_x	= pstParams->ofs_x;
				_g_win_ctrl[LX_PE_WIN_MAIN].ofs_y	= pstParams->ofs_y;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
                PE_P1R_L9A0_Wr02(pe1_r_win_ctrl_01, win_w0_x0,	win0_x0,\
                									win_w0_y0,	win0_y0);
                PE_P1R_L9A0_Wr02(pe1_r_win_ctrl_02, win_w0_x1,	win0_x1,\
                									win_w0_y1,	win0_y1);

                PE_P1R_L9A0_Wr02(pe1_r_win_ctrl_03, win_w1_x0,	win1_x0,\
                									win_w1_y0,	win1_y0);
                PE_P1R_L9A0_Wr02(pe1_r_win_ctrl_04, win_w1_x1,	win1_x1,\
                									win_w1_y1,	win1_y1);

                PE_P1R_L9A0_Wr02(pe1_r_apl_ctrl_00, apl_win_ctrl_x0, x0,\
                									apl_win_ctrl_y0, y0);
                PE_P1R_L9A0_Wr02(pe1_r_apl_ctrl_01, apl_win_ctrl_x1, x1,\
                									apl_win_ctrl_y1, y1);

                PE_P1R_L9A0_Wr02(pe1_r_bbd2d_ctrl_1,bbd_x0, x0,\
                									bbd_y0, y0);
                PE_P1R_L9A0_Wr02(pe1_r_bbd2d_ctrl_2,bbd_x1, x1,\
                									bbd_y1, y1);
				_g_win_ctrl[LX_PE_WIN_SUB].out_type	= pstParams->out_type;
				_g_win_ctrl[LX_PE_WIN_SUB].in_type	= pstParams->in_type;
				_g_win_ctrl[LX_PE_WIN_SUB].pos_x	= pstParams->pos_x;
				_g_win_ctrl[LX_PE_WIN_SUB].pos_y	= pstParams->pos_y;
				_g_win_ctrl[LX_PE_WIN_SUB].size_w	= pstParams->size_w;
				_g_win_ctrl[LX_PE_WIN_SUB].size_h	= pstParams->size_h;
				_g_win_ctrl[LX_PE_WIN_SUB].ofs_x	= pstParams->ofs_x;
				_g_win_ctrl[LX_PE_WIN_SUB].ofs_y	= pstParams->ofs_y;
			}
		}
	} while (0);

	return ret;
}
static int PE_WIN_GetEnhancingWindow(LX_PE_WIN_CTRL_T *pstParams)
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
				pstParams->out_type	= _g_win_ctrl[LX_PE_WIN_MAIN].out_type;
				pstParams->in_type	= _g_win_ctrl[LX_PE_WIN_MAIN].in_type;
				pstParams->pos_x	= _g_win_ctrl[LX_PE_WIN_MAIN].pos_x;
				pstParams->pos_y	= _g_win_ctrl[LX_PE_WIN_MAIN].pos_y;
				pstParams->size_w	= _g_win_ctrl[LX_PE_WIN_MAIN].size_w;
				pstParams->size_h	= _g_win_ctrl[LX_PE_WIN_MAIN].size_h;
				pstParams->ofs_x	= _g_win_ctrl[LX_PE_WIN_MAIN].ofs_x;
				pstParams->ofs_y	= _g_win_ctrl[LX_PE_WIN_MAIN].ofs_y;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->out_type	= _g_win_ctrl[LX_PE_WIN_SUB].out_type;
				pstParams->in_type	= _g_win_ctrl[LX_PE_WIN_SUB].in_type;
				pstParams->pos_x	= _g_win_ctrl[LX_PE_WIN_SUB].pos_x;
				pstParams->pos_y	= _g_win_ctrl[LX_PE_WIN_SUB].pos_y;
				pstParams->size_w	= _g_win_ctrl[LX_PE_WIN_SUB].size_w;
				pstParams->size_h	= _g_win_ctrl[LX_PE_WIN_SUB].size_h;
				pstParams->ofs_x	= _g_win_ctrl[LX_PE_WIN_SUB].ofs_x;
				pstParams->ofs_y	= _g_win_ctrl[LX_PE_WIN_SUB].ofs_y;
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->out_type	= _g_win_ctrl[LX_PE_WIN_MAIN].out_type;
				pstParams->in_type	= _g_win_ctrl[LX_PE_WIN_MAIN].in_type;
				pstParams->pos_x	= _g_win_ctrl[LX_PE_WIN_MAIN].pos_x;
				pstParams->pos_y	= _g_win_ctrl[LX_PE_WIN_MAIN].pos_y;
				pstParams->size_w	= _g_win_ctrl[LX_PE_WIN_MAIN].size_w;
				pstParams->size_h	= _g_win_ctrl[LX_PE_WIN_MAIN].size_h;
				pstParams->ofs_x	= _g_win_ctrl[LX_PE_WIN_MAIN].ofs_x;
				pstParams->ofs_y	= _g_win_ctrl[LX_PE_WIN_MAIN].ofs_y;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->out_type	= _g_win_ctrl[LX_PE_WIN_SUB].out_type;
				pstParams->in_type	= _g_win_ctrl[LX_PE_WIN_SUB].in_type;
				pstParams->pos_x	= _g_win_ctrl[LX_PE_WIN_SUB].pos_x;
				pstParams->pos_y	= _g_win_ctrl[LX_PE_WIN_SUB].pos_y;
				pstParams->size_w	= _g_win_ctrl[LX_PE_WIN_SUB].size_w;
				pstParams->size_h	= _g_win_ctrl[LX_PE_WIN_SUB].size_h;
				pstParams->ofs_x	= _g_win_ctrl[LX_PE_WIN_SUB].ofs_x;
				pstParams->ofs_y	= _g_win_ctrl[LX_PE_WIN_SUB].ofs_y;
			}
		}

		PE_WIN_DBG_PRINT("get pstParams[%d] : o,i_type:%d,%d, x,y,w,h : %d,%d,%d,%d, ofs:%d,%d\n",\
			pstParams->win_id,pstParams->out_type,pstParams->in_type,pstParams->pos_x,pstParams->pos_y,\
			pstParams->size_w,pstParams->size_h,pstParams->ofs_x,pstParams->ofs_y);
	} while (0);
	return ret;
}
static int PE_WIN_SetAutoDetectInfo(LX_PE_WIN_D3D_INFO_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		PE_WIN_DBG_PRINT("set : \n"\
		"top:%d,%d,%d,%d,btm:%d,%d,%d,%d,lft:%d,%d,%d,%d,rgt:%d,%d,%d,%d,ratio(1024=1.0): tb:%d,ss:%d\n",\
		pstParams->top.pos_x0,pstParams->top.pos_y0,pstParams->top.pos_x1,pstParams->top.pos_y1,\
		pstParams->bottom.pos_x0,pstParams->bottom.pos_y0,pstParams->bottom.pos_x1,pstParams->bottom.pos_y1,\
		pstParams->left.pos_x0,pstParams->left.pos_y0,pstParams->left.pos_x1,pstParams->left.pos_y1,\
		pstParams->right.pos_x0,pstParams->right.pos_y0,pstParams->right.pos_x1,pstParams->right.pos_y1,\
		pstParams->tb_ratio,pstParams->ss_ratio);

		memcpy(&_g_win_detect_info,pstParams,sizeof(LX_PE_WIN_D3D_INFO_T));
	} while (0);
	return ret;
}
static int PE_WIN_GetAutoDetectInfo(LX_PE_WIN_D3D_INFO_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		memcpy(pstParams,&_g_win_detect_info,sizeof(LX_PE_WIN_D3D_INFO_T));

		PE_WIN_DBG_PRINT("get : \n"\
		"top:%d,%d,%d,%d,btm:%d,%d,%d,%d,lft:%d,%d,%d,%d,rgt:%d,%d,%d,%d,ratio(1024=1.0): tb:%d,ss:%d\n",\
		pstParams->top.pos_x0,pstParams->top.pos_y0,pstParams->top.pos_x1,pstParams->top.pos_y1,\
		pstParams->bottom.pos_x0,pstParams->bottom.pos_y0,pstParams->bottom.pos_x1,pstParams->bottom.pos_y1,\
		pstParams->left.pos_x0,pstParams->left.pos_y0,pstParams->left.pos_x1,pstParams->left.pos_y1,\
		pstParams->right.pos_x0,pstParams->right.pos_y0,pstParams->right.pos_x1,pstParams->right.pos_y1,\
		pstParams->tb_ratio,pstParams->ss_ratio);
	} while (0);
	return ret;
}
int PE_WIN_GetCurWinSettings(PE_WIN_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	static UINT32 count = 0;

	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->out_type	= _g_win_settings[LX_PE_WIN_MAIN].out_type;
				pstParams->in_type	= _g_win_settings[LX_PE_WIN_MAIN].in_type;
				pstParams->act_x0	= _g_win_settings[LX_PE_WIN_MAIN].act_x0;
				pstParams->act_y0	= _g_win_settings[LX_PE_WIN_MAIN].act_y0;
				pstParams->act_x1	= _g_win_settings[LX_PE_WIN_MAIN].act_x1;
				pstParams->act_y1	= _g_win_settings[LX_PE_WIN_MAIN].act_y1;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->out_type	= _g_win_settings[LX_PE_WIN_SUB].out_type;
				pstParams->in_type	= _g_win_settings[LX_PE_WIN_SUB].in_type;
				pstParams->act_x0	= _g_win_settings[LX_PE_WIN_SUB].act_x0;
				pstParams->act_y0	= _g_win_settings[LX_PE_WIN_SUB].act_y0;
				pstParams->act_x1	= _g_win_settings[LX_PE_WIN_SUB].act_x1;
				pstParams->act_y1	= _g_win_settings[LX_PE_WIN_SUB].act_y1;
			}
			pstParams->tb_ratio = _g_win_detect_info.tb_ratio;
			pstParams->ss_ratio = _g_win_detect_info.ss_ratio;
			if(_g_win_trace)
			{
				if(count%240==0)
				{
					PE_WIN_DBG_PRINT("get pstParams[%d] : o,i_type:%d,%d, act_x,y:%d,%d,%d,%d, ratio:tb:%d,ss:%d\n",\
						pstParams->win_id,pstParams->out_type,pstParams->in_type,pstParams->act_x0,pstParams->act_y0,\
						pstParams->act_x1,pstParams->act_y1,pstParams->tb_ratio,pstParams->ss_ratio);
				}
				if(count >= 240)	count=0;
				else				count++;
			}
		}
		else
		{
			PE_WIN_DBG_PRINT("nothing to do.\n");
		}
	} while (0);
	return ret;
}
int PE_WIN_SetD3dDceHistoWin(void)
{
	int ret = RET_OK;
	LX_PE_OUT_TYPE out_type;
	UINT32 act_l_x0,act_l_y0,act_l_x1,act_l_y1;

	do {
		if(PE_CHIP_VER_GT(L9,B0))
		{
			out_type	= _g_win_settings[LX_PE_WIN_MAIN].out_type;
			act_l_x0	= _g_win_settings[LX_PE_WIN_MAIN].act_x0;
			act_l_y0	= _g_win_settings[LX_PE_WIN_MAIN].act_y0;
			act_l_x1	= _g_win_settings[LX_PE_WIN_MAIN].act_x1;
			act_l_y1	= _g_win_settings[LX_PE_WIN_MAIN].act_y1;
			PE_WIN_DBG_PRINT("get : out_type:%d, act_l_x0,y0,x1,y1 : %d,%d,%d,%d\n",\
				out_type,act_l_x0,act_l_y0,act_l_x1,act_l_y1);

			if(out_type==LX_PE_OUT_TB)
			{
				act_l_y0 += PE_WIN_V_HALF;
				act_l_y1 += PE_WIN_V_HALF;
			}
			else if(out_type==LX_PE_OUT_SS)
			{
				act_l_x0 += PE_WIN_H_HALF;
				act_l_x1 += PE_WIN_H_HALF;
			}
			PE_P1R_L9B0_Wr02(pe1_r_win2_ctrl_01,win_w0_x0,	act_l_x0,\
            									win_w0_y0,	act_l_y0);
			PE_P1R_L9B0_Wr02(pe1_r_win2_ctrl_02,win_w0_x1,	act_l_x1,\
            									win_w0_y1,	act_l_y1);

			PE_WIN_DBG_PRINT("set : out_type:%d, act_l_x0,y0,x1,y1 : %d,%d,%d,%d\n",\
				out_type,act_l_x0,act_l_y0,act_l_x1,act_l_y1);
		}
		else
		{
			PE_WIN_DBG_PRINT("nothing to do.\n");
		}
	} while (0);
	return ret;
}
int PE_WIN_SetLrcrInOutSel(void)
{
	int ret = RET_OK;
	LX_PE_OUT_TYPE cur_out_type;

	do {
		if(PE_CHIP_VER_GT(L9,B0))
		{
			cur_out_type = _g_win_settings[LX_PE_WIN_MAIN].out_type;
			if(cur_out_type==LX_PE_OUT_2D)
			{
				DE_P1L_L9B0_RdFL(pe1_l_lrcr_ctrl_00);
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_hist_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_hist_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_proc_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_proc_data_sel,	0x0);	//L data
				//DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	ff_mode_en,			0x0);	//ff md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_out_data_sel,		0x1);	//R data, dont care
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_out_data_sel,		0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_rd_en,			0x0);	//rd md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_mode,			0x3);	//32 bin
				DE_P1L_L9B0_WrFL(pe1_l_lrcr_ctrl_00);
			}
			else if(cur_out_type==LX_PE_OUT_TB || cur_out_type==LX_PE_OUT_SS)
			{
				DE_P1L_L9B0_RdFL(pe1_l_lrcr_ctrl_00);
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_hist_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_hist_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_proc_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_proc_data_sel,	0x0);	//L data
				//DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	ff_mode_en,			0x0);	//ff md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_out_data_sel,		0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_out_data_sel,		0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_rd_en,			0x0);	//rd md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_mode,			0x0);	//256 bin
				DE_P1L_L9B0_WrFL(pe1_l_lrcr_ctrl_00);
			}
			else if(cur_out_type==LX_PE_OUT_FS || cur_out_type==LX_PE_OUT_DUAL || cur_out_type==LX_PE_OUT_PIP)
			{
				DE_P1L_L9B0_RdFL(pe1_l_lrcr_ctrl_00);
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_hist_data_sel,	0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_hist_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_proc_data_sel,	0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_proc_data_sel,	0x0);	//L data
				//DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	ff_mode_en,			0x0);	//ff md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_out_data_sel,		0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_out_data_sel,		0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_rd_en,			0x0);	//rd md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_mode,			0x0);	//256 bin
				DE_P1L_L9B0_WrFL(pe1_l_lrcr_ctrl_00);
			}
			else	//LX_PE_OUT_3D_2D || LX_PE_OUT_UD
			{
				DE_P1L_L9B0_RdFL(pe1_l_lrcr_ctrl_00);
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_hist_data_sel,	0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_hist_data_sel,	0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_proc_data_sel,	0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_proc_data_sel,	0x0);	//L data
				//DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	ff_mode_en,			0x0);	//ff md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	r_out_data_sel,		0x1);	//R data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	l_out_data_sel,		0x0);	//L data
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_rd_en,			0x0);	//rd md off
				DE_P1L_L9B0_Wr01(pe1_l_lrcr_ctrl_00,	hist_mode,			0x0);	//256 bin
				DE_P1L_L9B0_WrFL(pe1_l_lrcr_ctrl_00);
			}
		}
		else
		{
			PE_WIN_DBG_PRINT("nothing to do.\n");
		}
	} while (0);
	return ret;
}
int PE_WIN_SetLrcrWin(LX_PE_OUT_TYPE detect_type, LX_PE_WIN_POSITION_T win_pos)
{
#ifdef PE_D3D_KANGSIK_YOON_TEST

	int ret = RET_OK;
	LX_PE_OUT_TYPE cur_out_type;
	UINT32 act_l_x0,act_l_y0,act_l_x1,act_l_y1;
	UINT32 top_act_l_x0, top_act_l_y0, top_act_l_x1, top_act_l_y1;
	UINT32 bottom_act_l_x0, bottom_act_l_y0, bottom_act_l_x1, bottom_act_l_y1;
	UINT32 left_act_l_x0, left_act_l_y0, left_act_l_x1, left_act_l_y1;
	UINT32 right_act_l_x0, right_act_l_y0, right_act_l_x1, right_act_l_y1;

	do {
		if(PE_CHIP_VER_GT(L9,B0))
		{
			cur_out_type = _g_win_settings[LX_PE_WIN_MAIN].out_type;
			act_l_x0 = _g_win_settings[LX_PE_WIN_MAIN].act_x0;
			act_l_y0 = _g_win_settings[LX_PE_WIN_MAIN].act_y0;
			act_l_x1 = _g_win_settings[LX_PE_WIN_MAIN].act_x1;
			act_l_y1 = _g_win_settings[LX_PE_WIN_MAIN].act_y1;

			top_act_l_x0 = _g_win_detect_info.top.pos_x0;
			top_act_l_y0 = _g_win_detect_info.top.pos_y0;
			top_act_l_x1 = _g_win_detect_info.top.pos_x1;
			top_act_l_y1 = _g_win_detect_info.top.pos_y1;

			bottom_act_l_x0 = _g_win_detect_info.bottom.pos_x0;
			bottom_act_l_y0 = _g_win_detect_info.bottom.pos_y0;
			bottom_act_l_x1 = _g_win_detect_info.bottom.pos_x1;
			bottom_act_l_y1 = _g_win_detect_info.bottom.pos_y1;

			left_act_l_x0 = _g_win_detect_info.left.pos_x0;
			left_act_l_y0 = _g_win_detect_info.left.pos_y0;
			left_act_l_x1 = _g_win_detect_info.left.pos_x1;
			left_act_l_y1 = _g_win_detect_info.left.pos_y1;

			right_act_l_x0 = _g_win_detect_info.right.pos_x0;
			right_act_l_y0 = _g_win_detect_info.right.pos_y0;
			right_act_l_x1 = _g_win_detect_info.right.pos_x1;
			right_act_l_y1 = _g_win_detect_info.right.pos_y1;

			
			if(act_l_x1>PE_WIN_H_MAX)	act_l_x1=PE_WIN_H_MAX;
			if(act_l_y1>PE_WIN_V_MAX)	act_l_y1=PE_WIN_V_MAX;

			if(top_act_l_x1>PE_WIN_H_MAX)			top_act_l_x1=PE_WIN_H_MAX;
			if(top_act_l_y1>(PE_WIN_V_MAX>>1) )	    top_act_l_y1=(PE_WIN_V_MAX>>1);

			if(bottom_act_l_x1>PE_WIN_H_MAX)		bottom_act_l_x1=PE_WIN_H_MAX;
			if(bottom_act_l_y1>PE_WIN_V_MAX)		bottom_act_l_y1=PE_WIN_V_MAX;

			if(left_act_l_x1>(PE_WIN_H_MAX>>1))	    left_act_l_x1=(PE_WIN_H_MAX>>1);
			if(left_act_l_y1>PE_WIN_V_MAX )	        left_act_l_y1=PE_WIN_V_MAX;

			if(right_act_l_x1>PE_WIN_H_MAX)	        right_act_l_x1=PE_WIN_H_MAX;
			if(right_act_l_y1>PE_WIN_V_MAX)			right_act_l_y1=PE_WIN_V_MAX;
		
			if(cur_out_type==LX_PE_OUT_2D)
			{
				if(detect_type == LX_PE_OUT_TB)
				{
					if(win_pos == PE_WIN_LRTB_FULL)
					{
					/* L data:(0,0)~(1920,540), R data:(0,540)~(1920,1080) */
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		top_act_l_x0,\
															y0,		top_act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		top_act_l_x1,\
															y1,		top_act_l_y1);
															
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		bottom_act_l_x0,\
															y0,		bottom_act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		bottom_act_l_x1,\
															y1,		bottom_act_l_y1);
															
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
															y0,		act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
															y1,		act_l_y1);

//				    printk(" **********(PE_WIN_LRTB_FULL/TB) top_act_l_x0 = %d, top_act_l_y0 = %d, top_act_l_x1 = %d, top_act_l_y1 = %d *********\n", top_act_l_x0, top_act_l_y0, top_act_l_x1, top_act_l_y1);
//					printk(" **********(PE_WIN_LRTB_FULL/TB) bottom_act_l_x0 = %d, bottom_act_l_y0 = %d, bottom_act_l_x1 = %d, bottom_act_l_y1 = %d *********\n", bottom_act_l_x0, bottom_act_l_y0, bottom_act_l_x1, bottom_act_l_y1);
										
				}
					else if(win_pos == PE_WIN_LRTB_HAF1)
					{
						/* L data:(0,0)~(1920,270), R data:(0,540)~(1920,810) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		top_act_l_x0,\
																y0,		top_act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		top_act_l_x1,\
																y1,		top_act_l_y0 + ((top_act_l_y1 - top_act_l_y0)>>1) );										
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		bottom_act_l_x0,\
																y0,		bottom_act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		bottom_act_l_x1,\
																y1,		bottom_act_l_y0 + ((bottom_act_l_y1 - bottom_act_l_y0)>>1) );
																
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
																y0,		act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
																y1,		act_l_y1);

//						printk(" **********(PE_WIN_LRTB_HAF1/TB) top_act_l_x0 = %d, top_act_l_y0 = %d, top_act_l_x1 = %d, top_act_l_y1 = %d *********\n", top_act_l_x0, top_act_l_y0, top_act_l_x1, top_act_l_y0 + ((top_act_l_y1 - top_act_l_y0)>>1));
//						printk(" **********(PE_WIN_LRTB_HAF1/TB) bottom_act_l_x0 = %d, bottom_act_l_y0 = %d, bottom_act_l_x1 = %d, bottom_act_l_y1 = %d *********\n", bottom_act_l_x0, bottom_act_l_y0, bottom_act_l_x1, bottom_act_l_y0 + ((bottom_act_l_y1 - bottom_act_l_y0)>>1));
					
					}
					else if(win_pos == PE_WIN_LRTB_HAF2)
					{
						/* L data:(0,270)~(1920,540), R data:(0,810)~(1920,1080) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		top_act_l_x0,\
																y0,		top_act_l_y0 + ((top_act_l_y1 - top_act_l_y0)>>1) );
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		top_act_l_x1,\
																y1,		top_act_l_y1);										
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		bottom_act_l_x0,\
																y0,		bottom_act_l_y0 + ((bottom_act_l_y1 - bottom_act_l_y0)>>1) );
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		bottom_act_l_x1,\
																y1,		bottom_act_l_y1);
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
																y0,		act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
																y1,		act_l_y1);
//						printk(" **********(PE_WIN_LRTB_HAF2/TB) top_act_l_x0 = %d, top_act_l_y0 = %d, top_act_l_x1 = %d, top_act_l_y1 = %d *********\n", top_act_l_x0, top_act_l_y0 + ((top_act_l_y1 - top_act_l_y0)>>1), top_act_l_x1, top_act_l_y1);
//						printk(" **********(PE_WIN_LRTB_HAF2/TB) bottom_act_l_x0 = %d, bottom_act_l_y0 = %d, bottom_act_l_x1 = %d, bottom_act_l_y1 = %d *********\n", bottom_act_l_x0, bottom_act_l_y0 + ((bottom_act_l_y1 - bottom_act_l_y0)>>1), bottom_act_l_x1, bottom_act_l_y1);
					}
				}
				else if(detect_type == LX_PE_OUT_SS)
				{
					if(win_pos == PE_WIN_LRTB_FULL)
					{
					/* L data:(0,0)~(960,1080), R data:(960,0)~(1920,1080) */
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		left_act_l_x0,\
															y0,		left_act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		left_act_l_x1,\
															y1,		left_act_l_y1);										
															
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		right_act_l_x0,\
															y0,		right_act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		right_act_l_x1,\
															y1,		right_act_l_y1);										
															
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
															y0,		act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
															y1,		act_l_y1);

//				    printk(" **********(PE_WIN_LRTB_FULL/SS) left_act_l_x0 = %d, left_act_l_y0 = %d, left_act_l_x1 = %d, left_act_l_y1 = %d *********\n", left_act_l_x0, left_act_l_y0, left_act_l_x1, left_act_l_y1);
//					printk(" **********(PE_WIN_LRTB_FULL/SS) right_act_l_x0 = %d, right_act_l_y0 = %d, right_act_l_x1 = %d, right_act_l_y1 = %d *********\n", right_act_l_x0, right_act_l_y0, right_act_l_x1, right_act_l_y1);
															
				}
					else if(win_pos == PE_WIN_LRTB_HAF1)
					{
						/* L data:(0,0)~(960,540), R data:(960,0)~(1920,540) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		left_act_l_x0,\
																y0,		left_act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		left_act_l_x1,\
																y1,		left_act_l_y0 + ((left_act_l_y1-left_act_l_y0) >>1) );										
																
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		right_act_l_x0,\
																y0,		right_act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		right_act_l_x1,\
																y1,		right_act_l_y0 + ((right_act_l_y1-right_act_l_y0) >>1) );																				
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
																y0,		act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
																y1,		act_l_y1);					

//						printk(" **********(PE_WIN_LRTB_HAF1/SS) left_act_l_x0 = %d, left_act_l_y0 = %d, left_act_l_x1 = %d, left_act_l_y1 = %d *********\n", left_act_l_x0, left_act_l_y0, left_act_l_x1, left_act_l_y0 + ((left_act_l_y1-left_act_l_y0) >>1));
//						printk(" **********(PE_WIN_LRTB_HAF1/SS) right_act_l_x0 = %d, right_act_l_y0 = %d, right_act_l_x1 = %d, right_act_l_y1 = %d *********\n", right_act_l_x0, right_act_l_y0, right_act_l_x1, right_act_l_y0 + ((right_act_l_y1-right_act_l_y0) >>1));
					}
					else if(win_pos == PE_WIN_LRTB_HAF2)
					{
						/* L data:(0,540)~(960,1080), R data:(960,540)~(1920,1080) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		left_act_l_x0,\
																y0,		left_act_l_y0 + ((left_act_l_y1 - left_act_l_y0) >>1) );
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		left_act_l_x1,\
																y1,		left_act_l_y1);										
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		right_act_l_x0,\
																y0,		right_act_l_y0 + ((right_act_l_y1 - right_act_l_y0) >>1) );
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		right_act_l_x1,\
																y1,		right_act_l_y1);												
																
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
																y0,		act_l_y0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
																y1,		act_l_y1);					
//						printk(" **********(PE_WIN_LRTB_HAF2/SS) left_act_l_x0 = %d, left_act_l_y0 = %d, left_act_l_x1 = %d, left_act_l_y1 = %d *********\n", left_act_l_x0, left_act_l_y0 + ((left_act_l_y1 - left_act_l_y0) >>1), left_act_l_x1, left_act_l_y1);
//						printk(" **********(PE_WIN_LRTB_HAF2/SS) right_act_l_x0 = %d, right_act_l_y0 = %d, right_act_l_x1 = %d, right_act_l_y1 = %d *********\n", right_act_l_x0, right_act_l_y0 + ((right_act_l_y1 - right_act_l_y0) >>1), right_act_l_x1, right_act_l_y1);
												

					}
				}
				else
				{
					/* L:(0,0)~(1920,1080), R:(0,0)~(1920,1080) */
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		act_l_x0,\
															y0,		act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		act_l_x1,\
															y1,		act_l_y1);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		act_l_x0,\
															y0,		act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		act_l_x1,\
															y1,		act_l_y1);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
															y0,		act_l_y0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
															y1,		act_l_y1);
				}
			
			}
			else if(cur_out_type==LX_PE_OUT_TB)
			{
				/* L:(0,0)~(1920,540), R:(0,540)~(1920,1080) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_HALF);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0,\
														y0,		(PE_WIN_V_HALF));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
														y0,		(PE_WIN_V_HALF));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
			}
			else if(cur_out_type==LX_PE_OUT_SS)
			{
				/* L:(0,0)~(960,1080), R:(960,0)~(1920,1080) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_HALF,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		(PE_WIN_H_HALF),\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		(PE_WIN_H_HALF),\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
			}
			else if(cur_out_type==LX_PE_OUT_UD)
			{
				/* L:(0,0)~(1920,2160), R:(0,0)~(1920,2160) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX,\
														y1,		(PE_WIN_V_MAX<<1));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		(PE_WIN_V_MAX<<1));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		(PE_WIN_V_MAX<<1));
			}
			else if(cur_out_type==LX_PE_OUT_FS || cur_out_type==LX_PE_OUT_DUAL)
			{
				/* L,R: according to output window */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		act_l_x0,\
														y0,		act_l_y0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		act_l_x1,\
														y1,		act_l_y1);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		act_l_x0,\
														y0,		act_l_y0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		act_l_x1,\
														y1,		act_l_y1);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
														y0,		act_l_y0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
														y1,		act_l_y1);
			}
			else	// LX_PE_OUT_3D_2D || LX_PE_OUT_PIP
			{
				/* L:(0,0)~(1920,1080), R:(0,0)~(1920,1080) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
			}
		}
		else
		{
			PE_WIN_DBG_PRINT("nothing to do.\n");
		}
	} while (0);
	return ret;

#else

	int ret = RET_OK;
	LX_PE_OUT_TYPE cur_out_type;
	UINT32 act_l_x0,act_l_y0,act_l_x1,act_l_y1;

	do {
		if(PE_CHIP_VER_GT(L9,B0))
		{
			cur_out_type = _g_win_settings[LX_PE_WIN_MAIN].out_type;
			act_l_x0 = _g_win_settings[LX_PE_WIN_MAIN].act_x0;
			act_l_y0 = _g_win_settings[LX_PE_WIN_MAIN].act_y0;
			act_l_x1 = _g_win_settings[LX_PE_WIN_MAIN].act_x1;
			act_l_y1 = _g_win_settings[LX_PE_WIN_MAIN].act_y1;
			act_l_x1 += 1;
			act_l_y1 += 1;

			if(act_l_x1>PE_WIN_H_MAX)	act_l_x1=PE_WIN_H_MAX;
			if(act_l_y1>PE_WIN_V_MAX)	act_l_y1=PE_WIN_V_MAX;

		
			if(cur_out_type==LX_PE_OUT_2D)
			{
				if(detect_type == LX_PE_OUT_TB)
				{
					if(win_pos == PE_WIN_LRTB_FULL)
					{
					/* L data:(0,0)~(1920,540), R data:(0,540)~(1920,1080) */
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX - 64,\
															y1,		PE_WIN_V_HALF);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0 + 64,\
															y0,		PE_WIN_V_HALF);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
															y1,		PE_WIN_V_MAX);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
															y1,		PE_WIN_V_MAX);
				}
					else if(win_pos == PE_WIN_LRTB_HAF1)
					{
						/* L data:(0,0)~(1920,270), R data:(0,540)~(1920,810) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
																y0,		0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		1920 - 64,\
																y1,		270);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0 + 64,\
																y0,		540);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		1920,\
																y1,		810);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
																y0,		0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
																y1,		PE_WIN_V_MAX);
					}
					else if(win_pos == PE_WIN_LRTB_HAF2)
					{
						/* L data:(0,270)~(1920,540), R data:(0,810)~(1920,1080) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
																y0,		270);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		1920 - 64,\
																y1,		540);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		960 + 64,\
																y0,		810);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		1920,\
																y1,		1080);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
																y0,		0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
																y1,		PE_WIN_V_MAX);
					}
				}
				else if(detect_type == LX_PE_OUT_SS)
				{
					if(win_pos == PE_WIN_LRTB_FULL)
					{
					/* L data:(0,0)~(960,1080), R data:(960,0)~(1920,1080) */
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_HALF - 64,\
															y1,		PE_WIN_V_MAX);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		(PE_WIN_H_HALF + 64),\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
															y1,		PE_WIN_V_MAX);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
															y1,		PE_WIN_V_MAX);
				}
					else if(win_pos == PE_WIN_LRTB_HAF1)
					{
						/* L data:(0,0)~(960,540), R data:(960,0)~(1920,540) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
																y0,		0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		960 - 64,\
																y1,		540);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		960 + 64,\
																y0,		0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		1920,\
																y1,		540);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
																y0,		0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
																y1,		PE_WIN_V_MAX);					

					}
					else if(win_pos == PE_WIN_LRTB_HAF2)
					{
						/* L data:(0,540)~(960,1080), R data:(960,540)~(1920,1080) */
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
																y0,		540);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		960 - 64,\
																y1,		1080);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		960 + 64,\
																y0,		540);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		1920,\
																y1,		1080);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
																y0,		0);
						PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
																y1,		PE_WIN_V_MAX);					

					}
				}
				else
				{
					/* L:(0,0)~(1920,1080), R:(0,0)~(1920,1080) */
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX,\
															y1,		PE_WIN_V_MAX);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0,\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
															y1,		PE_WIN_V_MAX);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
															y0,		0);
					PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
															y1,		PE_WIN_V_MAX);
				}
			}
			else if(cur_out_type==LX_PE_OUT_TB)
			{
				/* L:(0,0)~(1920,540), R:(0,540)~(1920,1080) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_HALF);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0,\
														y0,		(PE_WIN_V_HALF));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
														y0,		(PE_WIN_V_HALF));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
			}
			else if(cur_out_type==LX_PE_OUT_SS)
			{
				/* L:(0,0)~(960,1080), R:(960,0)~(1920,1080) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_HALF,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		(PE_WIN_H_HALF),\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		(PE_WIN_H_HALF),\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
			}
			else if(cur_out_type==LX_PE_OUT_UD)
			{
				/* L:(0,0)~(1920,2160), R:(0,0)~(1920,2160) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX,\
														y1,		(PE_WIN_V_MAX<<1));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		(PE_WIN_V_MAX<<1));
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		(PE_WIN_V_MAX<<1));
			}
			else if(cur_out_type==LX_PE_OUT_FS || cur_out_type==LX_PE_OUT_DUAL)
			{
				/* L,R: according to output window */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		act_l_x0,\
														y0,		act_l_y0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		act_l_x1,\
														y1,		act_l_y1);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		act_l_x0,\
														y0,		act_l_y0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		act_l_x1,\
														y1,		act_l_y1);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		act_l_x0,\
														y0,		act_l_y0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		act_l_x1,\
														y1,		act_l_y1);
			}
			else	// LX_PE_OUT_3D_2D || LX_PE_OUT_PIP
			{
				/* L:(0,0)~(1920,1080), R:(0,0)~(1920,1080) */
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_02,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_03,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_04,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_05,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_06,	x0,		0,\
														y0,		0);
				PE_P1L_L9B0_Wr02(pe1_l_lrcr_ctrl_07,	x1,		PE_WIN_H_MAX,\
														y1,		PE_WIN_V_MAX);
			}
		}
		else
		{
			PE_WIN_DBG_PRINT("nothing to do.\n");
		}
	} while (0);
	return ret;

#endif
}


