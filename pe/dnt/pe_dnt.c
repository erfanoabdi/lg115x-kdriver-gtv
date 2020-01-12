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

/** @file pe_dnt.c
 *
 *  driver for picture enhance deinterlacer functions. ( used only within kdriver )
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
#include "pe_etc.h"
#include "pe_dnt.h"
#include "pe_dnt_coeff.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_DNT_ERROR	printk

#define PE_DNT_DBG_PRINT(fmt,args...)	if(_g_dnt_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_DNT_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_DNT_ERROR(fmt,##args);_action;}}

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
static int PE_DNT_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_DNT_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_DNT_SetFilmMode(LX_PE_DNT_FILMMODE_T *pstParams);
static int PE_DNT_GetFilmMode(LX_PE_DNT_FILMMODE_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT8 _g_dnt_trace=0x0;		//default should be off.
static UINT32 _g_dnt_op_mode=0x0;	//deinter operation mode(0x0:spatial,0x1:temporal), for debug
static UINT32 _g_dnt_fmd_en[LX_PE_WIN_NUM];

/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 * init dnt
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_DNT_Init(void)
{
	int ret = RET_OK;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_DNT_DBG_PRINT("init global values.\n");
			PE_SET_REG_TABLE(DNT,dnt_l_init_param_l9b0,0);
			PE_SET_REG_TABLE(DNT,dnt_r_init_param_l9b0,0);
			PE_SET_REG_TABLE(DNT,dnt_l_fim_init_param_l9b0,0);
		}
		else
		{
			PE_SET_REG_TABLE(DNT,dnt_l_init_param_l9a0,0);
			PE_SET_REG_TABLE(DNT,dnt_r_init_param_l9a0,0);
		}
	}while(0);

	return ret;
}

/**
 * standard function of dnt setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_DNT_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_DNT_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_DNT_FILMMODE_T):
		{
			LX_PE_DNT_FILMMODE_T *pRcvData;
			LX_PE_DNT_FILMMODE_T stParam;
			do{
				PE_DNT_DBG_PRINT("S LX_PE_DNT_FILMMODE_T\n");
				/* data size check */
				PE_DNT_CHECK_CODE(datasize!=sizeof(LX_PE_DNT_FILMMODE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DNT_FILMMODE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_DNT_SetFilmMode(&stParam);
				PE_DNT_CHECK_CODE(ret,break,"[%s,%d] PE_DNT_SetFilmMode() error.\n",__F__,__L__);
				PE_DNT_DBG_PRINT("E LX_PE_DNT_FILMMODE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_DNT_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_DNT_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_DNT_SetDefaultSettings(&stParam);
				PE_DNT_CHECK_CODE(ret,break,"[%s,%d] PE_DNT_SetDefaultSettings() error.\n",__F__,__L__);
				PE_DNT_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_DNT_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_DNT_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_DNT_SetDebugSettings(&stParam);
				PE_DNT_CHECK_CODE(ret,break,"[%s,%d] PE_DNT_SetDebugSettings() error.\n",__F__,__L__);
				PE_DNT_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_DNT_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of dnt getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in/out] unsigned int
 * @param   datasize [in/out] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_DNT_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_DNT_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_DNT_FILMMODE_T):
		{
			LX_PE_DNT_FILMMODE_T *pRcvData;
			LX_PE_DNT_FILMMODE_T stParam;
			memset0(stParam);
			do{
				PE_DNT_DBG_PRINT("S LX_PE_DNT_FILMMODE_T\n");
				/* data size check */
				PE_DNT_CHECK_CODE(datasize!=sizeof(LX_PE_DNT_FILMMODE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DNT_FILMMODE_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_DNT_GetFilmMode(&stParam);
				PE_DNT_CHECK_CODE(ret,break,"[%s,%d] PE_DNT_GetFilmMode() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_DNT_DBG_PRINT("E LX_PE_DNT_FILMMODE_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_DNT_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

static int PE_DNT_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_DNT_CHECK_CODE(pstParams->src_type>=LX_PE_SRC_NUM,ret=RET_ERROR;break,\
			"[%s,%d] src_type(%d) is over.\n",__F__,__L__,pstParams->src_type);
		PE_DNT_CHECK_CODE(pstParams->fmt_type>=LX_PE_FMT_NUM,ret=RET_ERROR;break,\
			"[%s,%d] fmt_type(%d) is over.\n",__F__,__L__,pstParams->fmt_type);
		PE_DNT_CHECK_CODE(pstParams->out_type>=LX_PE_OUT_NUM,ret=RET_ERROR;break,\
			"[%s,%d] out_type(%d) is over.\n",__F__,__L__,pstParams->out_type);
		PE_DNT_CHECK_CODE(pstParams->fr_type>=LX_PE_FR_NUM,ret=RET_ERROR;break,\
			"[%s,%d] fr_type(%d) is over.\n",__F__,__L__,pstParams->fr_type);
		PE_DNT_CHECK_CODE(pstParams->scan_type>=LX_PE_SCAN_NUM,ret=RET_ERROR;break,\
			"[%s,%d] scan_type(%d) is over.\n",__F__,__L__,pstParams->scan_type);
		PE_DNT_CHECK_CODE(pstParams->dtv_type>=LX_PE_DTV_NUM,ret=RET_ERROR;break,\
			"[%s,%d] dtv_type(%d) is over.\n",__F__,__L__,pstParams->dtv_type);
		PE_DNT_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			/* set ipc */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
					if(pstParams->fmt_type==LX_PE_FMT_NTSC)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_atv_ntsc_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_atv_ntsc_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_atv_pal_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_atv_pal_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_NTSC)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_av_ntsc_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_av_ntsc_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_av_pal_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_av_pal_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_dtv_atsc_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_dtv_atsc_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_dtv_dvb_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_dtv_dvb_sd_param_l9b0,0);
						}
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_all_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_COMP:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_av_ntsc_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_av_ntsc_param_l9b0,0);
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_all_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_RGBPC:
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_hdmi_atsc_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_hdmi_atsc_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_hdmi_pal_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_hdmi_pal_sd_param_l9b0,0);
						}
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_ipc_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_ipc_default_all_hd_param_l9b0,0);
					}
					break;
			}
			#ifdef PE_USE_STRONG_MOTION_GAIN
			if((pstParams->src_type==LX_PE_SRC_DTV || pstParams->src_type==LX_PE_SRC_COMP || \
				pstParams->src_type==LX_PE_SRC_HDMI) && (pstParams->scan_type==LX_PE_SCAN_PROGRESS))
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					DE_P0L_L9B0_RdFL(ipc_ctrl_04);
					DE_P0L_L9B0_Wr01(ipc_ctrl_04,	motion_gain,	0x60);
					DE_P0L_L9B0_WrFL(ipc_ctrl_04);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					DE_P0R_L9B0_RdFL(ipc_ctrl_04);
					DE_P0R_L9B0_Wr01(ipc_ctrl_04,	motion_gain,	0x60);
					DE_P0R_L9B0_WrFL(ipc_ctrl_04);
				}
			}
			#endif
			#ifdef PE_FIX_IPC_FLICKER_ON_DTV_HD
			if((pstParams->src_type==LX_PE_SRC_DTV)&&(pstParams->fmt_type==LX_PE_FMT_HD))
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
						DE_P0L_L9B0_RdFL(ipc_ctrl_04);
						DE_P0L_L9B0_Wr01(ipc_ctrl_04,	motion_x_tearing_gain,	0x20);
						DE_P0L_L9B0_WrFL(ipc_ctrl_04);
						DE_P0L_L9B0_RdFL(ipc_ctrl_06);
						DE_P0L_L9B0_Wr(ipc_ctrl_06, 0x0020A073);
						DE_P0L_L9B0_WrFL(ipc_ctrl_06);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P0R_L9B0_RdFL(ipc_ctrl_04);
						DE_P0R_L9B0_Wr01(ipc_ctrl_04,	motion_x_tearing_gain,	0x20);
						DE_P0R_L9B0_WrFL(ipc_ctrl_04);
						DE_P0R_L9B0_RdFL(ipc_ctrl_06);
						DE_P0R_L9B0_Wr(ipc_ctrl_06, 0x0020A073);
						DE_P0R_L9B0_WrFL(ipc_ctrl_06);
				}
			}
			else if((pstParams->src_type==LX_PE_SRC_ATV)&&(pstParams->fmt_type==LX_PE_FMT_NTSC))
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					DE_P0L_L9B0_RdFL(ipc_ctrl_06);
					DE_P0L_L9B0_Wr(ipc_ctrl_06,	0x00402073);
					DE_P0L_L9B0_WrFL(ipc_ctrl_06);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					DE_P0R_L9B0_RdFL(ipc_ctrl_06);
					DE_P0R_L9B0_Wr(ipc_ctrl_06,	0x00402073);
					DE_P0R_L9B0_WrFL(ipc_ctrl_06);
				}
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					DE_P0L_L9B0_RdFL(ipc_ctrl_06);
					DE_P0L_L9B0_Wr(ipc_ctrl_06,	0x00700573);
					DE_P0L_L9B0_WrFL(ipc_ctrl_06);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					DE_P0R_L9B0_RdFL(ipc_ctrl_06);
					DE_P0R_L9B0_Wr(ipc_ctrl_06,	0x00700573);
					DE_P0R_L9B0_WrFL(ipc_ctrl_06);
				}
			}
			#endif
			/* set hmc */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
					if(pstParams->fmt_type==LX_PE_FMT_NTSC)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_atv_ntsc_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_atv_ntsc_param_l9b0[0]);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_atv_pal_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_atv_pal_param_l9b0[0]);
					}
					break;
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_NTSC)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_av_ntsc_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_av_ntsc_param_l9b0[0]);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_av_pal_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_av_pal_param_l9b0[0]);
					}
					break;
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_dtv_atsc_sd_param_l9b0[0]);
							if(PE_CHECK_SUB(pstParams->win_id))
								P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_dtv_atsc_sd_param_l9b0[0]);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_dtv_dvb_sd_param_l9b0[0]);
							if(PE_CHECK_SUB(pstParams->win_id))
								P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_dtv_dvb_sd_param_l9b0[0]);
						}
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_all_hd_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_all_hd_param_l9b0[0]);
					}
					break;
				case LX_PE_SRC_COMP:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_av_ntsc_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_av_ntsc_param_l9b0[0]);
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_all_hd_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_all_hd_param_l9b0[0]);
					}
					break;
				case LX_PE_SRC_RGBPC:
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_hdmi_atsc_sd_param_l9b0[0]);
							if(PE_CHECK_SUB(pstParams->win_id))
								P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_hdmi_atsc_sd_param_l9b0[0]);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_hdmi_pal_sd_param_l9b0[0]);
							if(PE_CHECK_SUB(pstParams->win_id))
								P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_hdmi_pal_sd_param_l9b0[0]);
						}
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							P0L_L9B0_DNT_SET_HMC(dnt_l_ipc_hmc_default_all_hd_param_l9b0[0]);
						if(PE_CHECK_SUB(pstParams->win_id))
							P0R_L9B0_DNT_SET_HMC(dnt_r_ipc_hmc_default_all_hd_param_l9b0[0]);
					}
					break;
			}
			/* set fmd */
			switch(pstParams->src_type)
			{
				case LX_PE_SRC_ATV:
					if(pstParams->fmt_type==LX_PE_FMT_NTSC)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_atv_60hz_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_atv_60hz_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_atv_50hz_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_atv_50hz_sd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_CVBS:
					if(pstParams->fmt_type==LX_PE_FMT_NTSC)
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_av_60hz_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_av_60hz_sd_param_l9b0,0);
					}
					else
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_av_50hz_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_av_50hz_sd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_COMP:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ)
						{
						if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_av_60hz_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_av_60hz_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_av_50hz_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_av_50hz_sd_param_l9b0,0);
						}
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_all_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_DTV:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ)
						{
						if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_dtv_60hz_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_dtv_60hz_sd_param_l9b0,0);
						}
						else
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_dtv_50hz_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_dtv_50hz_sd_param_l9b0,0);
						}
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_dtv_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_dtv_hd_param_l9b0,0);
					}
					break;
				case LX_PE_SRC_RGBPC:
				case LX_PE_SRC_HDMI:
				default:
					if(pstParams->fmt_type==LX_PE_FMT_SD)
					{
						if(pstParams->fr_type==LX_PE_FR_60HZ)
						{
							if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_hdmi_60hz_sd_param_l9b0,0);
							if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_hdmi_60hz_sd_param_l9b0,0);
						}
						else
						{
						if(PE_CHECK_MAIN(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_hdmi_50hz_sd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
								PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_hdmi_50hz_sd_param_l9b0,0);
						}
					}
					else	//hd
					{
						if(PE_CHECK_MAIN(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_l_fmd_default_all_hd_param_l9b0,0);
						if(PE_CHECK_SUB(pstParams->win_id))
							PE_SET_REG_TABLE(DNT,dnt_r_fmd_default_all_hd_param_l9b0,0);
					}
					break;
			}
		}
		else
		{
			PE_DNT_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
static int PE_DNT_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_DNT_DBG_PRINT("win id : %d, type : 0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(DNT)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_dnt_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(DNT)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(DNT)))
				{
					PE_DNT_DBG_PRINT("luma,chroma_mode : %s, film_mode_enable : disable.\n",\
						_g_dnt_op_mode? "temporal":"spatial");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P0L_L9B0_RdFL(ipc_ctrl_00);
						DE_P0L_L9B0_RdFL(ipc_ctrl_23);
						DE_P0L_L9B0_Wr01(ipc_ctrl_00,	luma_mode,			_g_dnt_op_mode);
						DE_P0L_L9B0_Wr01(ipc_ctrl_00,	chroma_mode,		_g_dnt_op_mode);
						DE_P0L_L9B0_Wr01(ipc_ctrl_00,	film_mode_enable,	0x0);
						DE_P0L_L9B0_Wr01(ipc_ctrl_23,	reg_global_tbm_en,	0x0);
						DE_P0L_L9B0_WrFL(ipc_ctrl_00);
						DE_P0L_L9B0_WrFL(ipc_ctrl_23);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P0R_L9B0_RdFL(ipc_ctrl_00);
						DE_P0R_L9B0_RdFL(ipc_ctrl_23);
						DE_P0R_L9B0_Wr01(ipc_ctrl_00,	luma_mode,			_g_dnt_op_mode);
						DE_P0R_L9B0_Wr01(ipc_ctrl_00,	chroma_mode,		_g_dnt_op_mode);
						DE_P0R_L9B0_Wr01(ipc_ctrl_00,	film_mode_enable,	0x0);
						DE_P0R_L9B0_Wr01(ipc_ctrl_23,	reg_global_tbm_en,	0x0);
						DE_P0R_L9B0_WrFL(ipc_ctrl_00);
						DE_P0R_L9B0_WrFL(ipc_ctrl_23);
					}
					if(_g_dnt_op_mode)	_g_dnt_op_mode=0x0;
					else				_g_dnt_op_mode=0x1;
				}
				else
				{
					PE_DNT_DBG_PRINT("luma,chroma_mode : motion adaptive.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P0L_L9B0_RdFL(ipc_ctrl_00);
						DE_P0L_L9B0_Wr01(ipc_ctrl_00,	luma_mode,	0x2);
						DE_P0L_L9B0_Wr01(ipc_ctrl_00,	chroma_mode,0x2);
						DE_P0L_L9B0_WrFL(ipc_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P0R_L9B0_RdFL(ipc_ctrl_00);
						DE_P0R_L9B0_Wr01(ipc_ctrl_00,	luma_mode,	0x2);
						DE_P0R_L9B0_Wr01(ipc_ctrl_00,	chroma_mode,0x2);
						DE_P0R_L9B0_WrFL(ipc_ctrl_00);
					}
				}
			}
			else
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(DNT)))
				{
					PE_DNT_DBG_PRINT("luma,chroma_mode : %s, film_mode_enable : disable.\n",\
						_g_dnt_op_mode? "temporal":"spatial");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P0L_L9_RdFL(p0l_ipc_ctrl_00);
						DE_P0L_L9_Wr01(p0l_ipc_ctrl_00,	luma_mode,			_g_dnt_op_mode);
						DE_P0L_L9_Wr01(p0l_ipc_ctrl_00,	chroma_mode,		_g_dnt_op_mode);
						DE_P0L_L9_Wr01(p0l_ipc_ctrl_00,	film_mode_enable,	0x0);
						DE_P0L_L9_WrFL(p0l_ipc_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P0R_L9_RdFL(p0r_ipc_ctrl_00);
						DE_P0R_L9_Wr01(p0r_ipc_ctrl_00,	luma_mode,			_g_dnt_op_mode);
						DE_P0R_L9_Wr01(p0r_ipc_ctrl_00,	chroma_mode,		_g_dnt_op_mode);
						DE_P0R_L9_Wr01(p0r_ipc_ctrl_00,	film_mode_enable,	0x0);
						DE_P0R_L9_WrFL(p0r_ipc_ctrl_00);
					}
					if(_g_dnt_op_mode)	_g_dnt_op_mode=0x0;
					else				_g_dnt_op_mode=0x1;
				}
				else
				{
					PE_DNT_DBG_PRINT("luma,chroma_mode : motion adaptive.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						DE_P0L_L9_RdFL(p0l_ipc_ctrl_00);
						DE_P0L_L9_Wr01(p0l_ipc_ctrl_00,	luma_mode,	0x2);
						DE_P0L_L9_Wr01(p0l_ipc_ctrl_00,	chroma_mode,0x2);
						DE_P0L_L9_WrFL(p0l_ipc_ctrl_00);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						DE_P0R_L9_RdFL(p0r_ipc_ctrl_00);
						DE_P0R_L9_Wr01(p0r_ipc_ctrl_00,	luma_mode,	0x2);
						DE_P0R_L9_Wr01(p0r_ipc_ctrl_00,	chroma_mode,0x2);
						DE_P0R_L9_WrFL(p0r_ipc_ctrl_00);
					}
				}
			}
		}
	}while(0);
	return ret;
}
static int PE_DNT_SetFilmMode(LX_PE_DNT_FILMMODE_T *pstParams)
{
	int ret = RET_OK;
	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_DNT_DBG_PRINT("set pstParams[%d] : en:%d\n",pstParams->win_id,pstParams->enable);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_DNT_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				PE_FWI_PQ_TABLE_HEAD_T header;
				PE_FWI_L9_PQ_IPC_CTRL table;
				UINT32 tbl_size;
				UINT8 *pTable = NULL;

				pTable = (UINT8 *)&table;
				tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = LX_PE_WIN_MAIN;	//pstParams->win_id;
				header.offset = PE_FWI_OFFSET(PE_FWI_L9_DNT_FMD_CTRL);
				header.length = sizeof(PE_FWI_L9_DNT_FMD_CTRL);
				table.fmd_ctrl.fmd_enable = (pstParams->enable)? 1:0;

				PE_DNT_DBG_PRINT("[%s,%d]wId:%d,ofs:%d,len:%d, tbl_size:%d, fmd_en:%d\n",\
					__F__,__L__,header.win_id,header.offset,\
					header.length,tbl_size,table.fmd_ctrl.fmd_enable);
				ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
				PE_DNT_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);

				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					_g_dnt_fmd_en[LX_PE_WIN_MAIN] = pstParams->enable;
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					_g_dnt_fmd_en[LX_PE_WIN_SUB] = pstParams->enable;
				}
				PE_ETC_SetPqFwWindow((UINT32)pstParams->win_id);
			}
			else
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					DE_P0L_L9B0_RdFL(ipc_ctrl_00);
					DE_P0L_L9B0_RdFL(ipc_ctrl_12);
					DE_P0L_L9B0_Wr01(ipc_ctrl_00,	film_mode_enable,	(pstParams->enable)? 0x1:0x0);
					DE_P0L_L9B0_Wr01(ipc_ctrl_12,	fmd_32_enable,		(pstParams->enable)? 0x1:0x0);
					DE_P0L_L9B0_Wr01(ipc_ctrl_12,	fmd_22_enable,		(pstParams->enable)? 0x1:0x0);
					DE_P0L_L9B0_WrFL(ipc_ctrl_00);
					DE_P0L_L9B0_WrFL(ipc_ctrl_12);
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					DE_P0R_L9B0_RdFL(ipc_ctrl_00);
					DE_P0R_L9B0_RdFL(ipc_ctrl_12);
					DE_P0R_L9B0_Wr01(ipc_ctrl_00,	film_mode_enable,	(pstParams->enable)? 0x1:0x0);
					DE_P0R_L9B0_Wr01(ipc_ctrl_12,	fmd_32_enable,		(pstParams->enable)? 0x1:0x0);
					DE_P0R_L9B0_Wr01(ipc_ctrl_12,	fmd_22_enable,		(pstParams->enable)? 0x1:0x0);
					DE_P0R_L9B0_WrFL(ipc_ctrl_00);
					DE_P0R_L9B0_WrFL(ipc_ctrl_12);
				}
			}
		}
		else
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P0L_L9_RdFL(p0l_ipc_ctrl_00);
				DE_P0L_L9_RdFL(p0l_ipc_ctrl_12);
				DE_P0L_L9_Wr01(p0l_ipc_ctrl_00,	film_mode_enable,	(pstParams->enable)? 0x1:0x0);
				DE_P0L_L9_Wr01(p0l_ipc_ctrl_12,	fmd_32_enable,		(pstParams->enable)? 0x1:0x0);
				DE_P0L_L9_Wr01(p0l_ipc_ctrl_12,	fmd_22_enable,		(pstParams->enable)? 0x1:0x0);
				DE_P0L_L9_WrFL(p0l_ipc_ctrl_00);
				DE_P0L_L9_WrFL(p0l_ipc_ctrl_12);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P0R_L9_RdFL(p0r_ipc_ctrl_00);
				DE_P0R_L9_RdFL(p0r_ipc_ctrl_12);
				DE_P0R_L9_Wr01(p0r_ipc_ctrl_00,	film_mode_enable,	(pstParams->enable)? 0x1:0x0);
				DE_P0R_L9_Wr01(p0r_ipc_ctrl_12,	fmd_32_enable,		(pstParams->enable)? 0x1:0x0);
				DE_P0R_L9_Wr01(p0r_ipc_ctrl_12,	fmd_22_enable,		(pstParams->enable)? 0x1:0x0);
				DE_P0R_L9_WrFL(p0r_ipc_ctrl_00);
				DE_P0R_L9_WrFL(p0r_ipc_ctrl_12);
			}
		}
	} while (0);
	return ret;
}
static int PE_DNT_GetFilmMode(LX_PE_DNT_FILMMODE_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do {
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_DNT_DBG_PRINT("g_pe_use_fw : %s.\n",g_pe_use_fw? "yes":"no");
			if(g_pe_use_fw)
			{
				if(PE_CHECK_MAIN(pstParams->win_id))
				{
					pstParams->enable = _g_dnt_fmd_en[LX_PE_WIN_MAIN];
				}
				if(PE_CHECK_SUB(pstParams->win_id))
				{
					pstParams->enable = _g_dnt_fmd_en[LX_PE_WIN_SUB];
				}
			}
			else
			{
				if(PE_CHECK_MAIN(win_id))
				{
					DE_P0L_L9B0_RdFL(ipc_ctrl_00);
					DE_P0L_L9B0_Rd01(ipc_ctrl_00,	film_mode_enable,	pstParams->enable);
				}
				if(PE_CHECK_SUB(win_id))
				{
					DE_P0R_L9B0_RdFL(ipc_ctrl_00);
					DE_P0R_L9B0_Rd01(ipc_ctrl_00,	film_mode_enable,	pstParams->enable);
				}
			}
		}
		else
		{
			if(PE_CHECK_MAIN(win_id))
			{
				DE_P0L_L9_RdFL(p0l_ipc_ctrl_00);
				DE_P0L_L9_Rd01(p0l_ipc_ctrl_00,	film_mode_enable,	pstParams->enable);
			}
			if(PE_CHECK_SUB(win_id))
			{
				DE_P0R_L9_RdFL(p0r_ipc_ctrl_00);
				DE_P0R_L9_Rd01(p0r_ipc_ctrl_00,	film_mode_enable,	pstParams->enable);
			}
		}

		PE_DNT_DBG_PRINT("get pstParams[%d] : en:%d\n",pstParams->win_id,pstParams->enable);
	} while (0);
	return ret;
}

