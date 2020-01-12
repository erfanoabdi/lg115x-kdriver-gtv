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

/** @file pe_cmn.c
 *
 *  driver for picture enhance common functions. ( used only within kdriver )
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
#include "pe_cmn.h"
#include "pe_cmn_coeff.h"

#include "pe_etc.h"
#include "pe_spd.h"
#include "pe_fwi.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_CMN_ERROR	printk

#define PE_CMN_DBG_PRINT(fmt,args...)	if(_g_cmn_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_CMN_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_CMN_ERROR(fmt,##args);_action;}}

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
UINT8 g_pe_use_fw=0x0;	// 0:by register, 1:by fw, initialized in PE_CMN_Init()

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int PE_CMN_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_CMN_GetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams);
static int PE_CMN_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_CMN_GetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams);
static int PE_CMN_SetContrast(LX_PE_CMN_CONTRAST_T *pstParams);
static int PE_CMN_GetContrast(LX_PE_CMN_CONTRAST_T *pstParams);
static int PE_CMN_SetBrightness(LX_PE_CMN_BRIGHTNESS_T *pstParams);
static int PE_CMN_GetBrightness(LX_PE_CMN_BRIGHTNESS_T *pstParams);
static int PE_CMN_SetSaturation(LX_PE_CMN_SATURATION_T *pstParams);
static int PE_CMN_GetSaturation(LX_PE_CMN_SATURATION_T *pstParams);
static int PE_CMN_SetHue(LX_PE_CMN_HUE_T *pstParams);
static int PE_CMN_GetHue(LX_PE_CMN_HUE_T *pstParams);
static int PE_CMN_SetSimultaneousMode(LX_PE_CMN_SIMUL_MODE_T *pstParams);
static int PE_CMN_GetSimultaneousMode(LX_PE_CMN_SIMUL_MODE_T *pstParams);
static int PE_CMN_SetLevelCtrl(LX_PE_CMN_LEVEL_CTRL_T *pstParams);
static int PE_CMN_GetLevelCtrl(LX_PE_CMN_LEVEL_CTRL_T *pstParams);
static int PE_CMN_SetSubContrast(LX_PE_CMN_SUB_CONTRAST_T *pstParams);
static int PE_CMN_GetSubContrast(LX_PE_CMN_SUB_CONTRAST_T *pstParams);
static int PE_CMN_SetFwiCtrl(LX_PE_CMN_FWI_CTRL_T *pstParams);
static int PE_CMN_GetFwiCtrl(LX_PE_CMN_FWI_CTRL_T *pstParams);
static int PE_CMN_SetMuteState(LX_PE_CMN_MUTE_STATE_T *pstParams);
static int PE_CMN_GetMuteState(LX_PE_CMN_MUTE_STATE_T *pstParams);
static int PE_CMN_SetPicEnhanceBypass(LX_PE_CMN_BYPASS_T *pstParams);
static int PE_CMN_GetPicEnhanceBypass(LX_PE_CMN_BYPASS_T *pstParams);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT32 _g_cmn_trace=0x0;	//default should be off.
static UINT8 _g_cmn_simul_mode=0;
static LX_PE_DBG_SETTINGS_T _g_cmn_dbg[LX_PE_WIN_NUM];
static LX_PE_DEFAULT_SETTINGS_T _g_cmn_default[LX_PE_WIN_NUM];
static LX_PE_CMN_MUTE_STATE_T _g_cmn_mute_state[LX_PE_WIN_NUM];
static PE_CMN_SETTINGS_T _g_cmn_cur_settings[LX_PE_WIN_NUM];
static LX_PE_CMN_BYPASS_T _g_cmn_bypass_settings[LX_PE_WIN_NUM];
static UINT16 _g_cmn_contrast[LX_PE_WIN_NUM];
static UINT16 _g_cmn_brightness[LX_PE_WIN_NUM];
static UINT16 _g_cmn_saturation[LX_PE_WIN_NUM];
static UINT16 _g_cmn_hue[LX_PE_WIN_NUM];

/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * init common module
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CMN_Init(void)
{
	int ret = RET_OK;
	UINT8 i;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_CMN_DBG_PRINT("init global values.\n");
			g_pe_use_fw = 0x1;	// 0:by register, 1:by fw
			if(g_pe_use_fw)
				PE_ETC_SetPqFwCtrlEnable(LX_PE_WIN_MAIN);
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_cmn_dbg[i].win_id		= i;
				_g_cmn_default[i].win_id	= i;
				_g_cmn_default[i].src_type	= LX_PE_SRC_NUM;
				_g_cmn_default[i].fmt_type	= LX_PE_FMT_NUM;
				_g_cmn_default[i].out_type	= LX_PE_OUT_NUM;
				_g_cmn_default[i].fr_type	= LX_PE_FR_NUM;
				_g_cmn_default[i].scan_type	= LX_PE_SCAN_NUM;
				_g_cmn_default[i].dtv_type	= LX_PE_DTV_NUM;
				_g_cmn_contrast[i]			= 0xffff;
				_g_cmn_brightness[i]		= 0xffff;
				_g_cmn_saturation[i]		= 0xffff;
				_g_cmn_hue[i]				= 0xffff;
				_g_cmn_mute_state[i].win_id = i;
				_g_cmn_mute_state[i].on_off = 0;
				_g_cmn_cur_settings[i].win_id = i;
				_g_cmn_cur_settings[i].mute_en = 0;
				_g_cmn_bypass_settings[i].win_id = i;
				_g_cmn_bypass_settings[i].front_bypass_en = 0;	//default should be off.
				
			}
			PE_CMN_DBG_PRINT("vsp-yc,rgb : enable.\n");
			PE_P1L_L9B0_Wr04(pe1_l_level_ctrl_00,	enable,			0x1,\
													y_offset,		0x200,\
													center_position,0x80,\
													y_gain,			0x80);
			#ifdef PE_FIX_DECISION_LOW_GRAY_LVLS
			PE_P1L_L9B0_Wr03(pe1_l_vspyc_ctrl_00,	enable,			0x1,\
													center_position,0x10,\
													contrast,		0x200);
			#else
			PE_P1L_L9B0_Wr03(pe1_l_vspyc_ctrl_00,	enable,			0x1,\
													center_position,0x0,\
													contrast,		0x200);
			#endif
			PE_P1L_L9B0_Wr02(pe1_l_vspyc_ctrl_01,	saturation,		0x80,\
													brightness,		0x200);
			PE_P1L_L9B0_Wr04(pe1_l_vsprgb_ctrl_00,	enable,			0x1,\
													contrast,		0x200,\
													center_position,0x0,\
													brightness,		0x200);
			PE_P1R_L9B0_Wr04(pe1_r_level_ctrl_00,	enable,			0x1,\
													y_offset,		0x200,\
													center_position,0x80,\
													y_gain,			0x80);
			#ifdef PE_FIX_DECISION_LOW_GRAY_LVLS
			PE_P1R_L9B0_Wr03(pe1_r_vspyc_ctrl_00,	enable,			0x1,\
													center_position,0x10,\
													contrast,		0x200);
			#else
			PE_P1R_L9B0_Wr03(pe1_r_vspyc_ctrl_00,	enable,			0x1,\
													center_position,0x0,\
													contrast,		0x200);
			#endif
			PE_P1R_L9B0_Wr02(pe1_r_vspyc_ctrl_01,	saturation,		0x80,\
													brightness,		0x200);
			PE_P1R_L9B0_Wr04(pe1_r_vsprgb_ctrl_00,	enable,			0x1,\
													contrast,		0x200,\
													center_position,0x0,\
													brightness,		0x200);
		}
		else
		{
			PE_CMN_DBG_PRINT("init global values.\n");
			for(i=0;i<LX_PE_WIN_NUM;i++)
			{
				_g_cmn_dbg[i].win_id		= i;
				_g_cmn_default[i].win_id	= i;
				_g_cmn_default[i].src_type	= LX_PE_SRC_NUM;
				_g_cmn_default[i].fmt_type	= LX_PE_FMT_NUM;
				_g_cmn_default[i].out_type	= LX_PE_OUT_NUM;
				_g_cmn_default[i].fr_type	= LX_PE_FR_NUM;
				_g_cmn_default[i].scan_type	= LX_PE_SCAN_NUM;
				_g_cmn_default[i].dtv_type	= LX_PE_DTV_NUM;
				_g_cmn_contrast[i]			= 0xffff;
				_g_cmn_brightness[i]		= 0xffff;
				_g_cmn_saturation[i]		= 0xffff;
				_g_cmn_hue[i]				= 0xffff;
			}
		}
	}while(0);

	return ret;
}

/**
 * standard function of cmn setting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CMN_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CMN_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CMN_CONTRAST_T):
		{
			LX_PE_CMN_CONTRAST_T *pRcvData;
			LX_PE_CMN_CONTRAST_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_CONTRAST_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_CONTRAST_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_CONTRAST_T *)data;
				//memcpy(&stParam,pRcvData,datasize);
				ret = PE_COPY_FROM_USER(PE_SUSPEND_CMN_SETCONTRAST, (void *)&stParam, (void *)pRcvData, datasize);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_SUSPEND_CMN_SETCONTRAST error.\n",__F__,__L__);

				ret = PE_CMN_SetContrast(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetContrast() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_CONTRAST_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_BRIGHTNESS_T):
		{
			LX_PE_CMN_BRIGHTNESS_T *pRcvData;
			LX_PE_CMN_BRIGHTNESS_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_BRIGHTNESS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_BRIGHTNESS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_BRIGHTNESS_T *)data;
				//memcpy(&stParam,pRcvData,datasize);
				ret = PE_COPY_FROM_USER(PE_SUSPEND_CMN_SETBRIGHTNESS, (void *)&stParam, (void *)pRcvData, datasize);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_SUSPEND_CMN_SETBRIGHTNESS error.\n",__F__,__L__);

				ret = PE_CMN_SetBrightness(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetBrightness() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_BRIGHTNESS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_SATURATION_T):
		{
			LX_PE_CMN_SATURATION_T *pRcvData;
			LX_PE_CMN_SATURATION_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_SATURATION_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_SATURATION_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_SATURATION_T *)data;
				//memcpy(&stParam,pRcvData,datasize);
				ret = PE_COPY_FROM_USER(PE_SUSPEND_CMN_SETSATURATION, (void *)&stParam, (void *)pRcvData, datasize);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_SUSPEND_CMN_SETSATURATION error.\n",__F__,__L__);

				ret = PE_CMN_SetSaturation(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetSaturation() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_SATURATION_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_HUE_T):
		{
			LX_PE_CMN_HUE_T *pRcvData;
			LX_PE_CMN_HUE_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_HUE_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_HUE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_HUE_T *)data;
				//memcpy(&stParam,pRcvData,datasize);
				ret = PE_COPY_FROM_USER(PE_SUSPEND_CMN_SETHUE, (void *)&stParam, (void *)pRcvData, datasize);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_SUSPEND_CMN_SETHUE error.\n",__F__,__L__);

				ret = PE_CMN_SetHue(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetHue() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_HUE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_SIMUL_MODE_T):
		{
			LX_PE_CMN_SIMUL_MODE_T *pRcvData;
			LX_PE_CMN_SIMUL_MODE_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_SIMUL_MODE_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_SIMUL_MODE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_SIMUL_MODE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMN_SetSimultaneousMode(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetSimultaneousMode() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_SIMUL_MODE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_LEVEL_CTRL_T):
		{
			LX_PE_CMN_LEVEL_CTRL_T *pRcvData;
			LX_PE_CMN_LEVEL_CTRL_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_LEVEL_CTRL_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_LEVEL_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_LEVEL_CTRL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMN_SetLevelCtrl(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetLevelCtrl() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_LEVEL_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_SUB_CONTRAST_T):
		{
			LX_PE_CMN_SUB_CONTRAST_T *pRcvData;
			LX_PE_CMN_SUB_CONTRAST_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_SUB_CONTRAST_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_SUB_CONTRAST_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_SUB_CONTRAST_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMN_SetSubContrast(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetSubContrast() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_SUB_CONTRAST_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_FWI_CTRL_T):
		{
			LX_PE_CMN_FWI_CTRL_T *pRcvData;
			LX_PE_CMN_FWI_CTRL_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_FWI_CTRL_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_FWI_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_FWI_CTRL_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMN_SetFwiCtrl(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetFwiCtrl() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_FWI_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_MUTE_STATE_T):
		{
			LX_PE_CMN_MUTE_STATE_T *pRcvData;
			LX_PE_CMN_MUTE_STATE_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_MUTE_STATE_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_MUTE_STATE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_MUTE_STATE_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMN_SetMuteState(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetMuteState() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_MUTE_STATE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_BYPASS_T):
		{
			LX_PE_CMN_BYPASS_T *pRcvData;
			LX_PE_CMN_BYPASS_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_BYPASS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_BYPASS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_BYPASS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMN_SetPicEnhanceBypass(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetPicEnhanceBypass() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_BYPASS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				//memcpy(&stParam,pRcvData,datasize);
				ret = PE_COPY_FROM_USER(PE_SUSPEND_CMN_SETDEFAULT, (void *)&stParam, (void *)pRcvData, datasize);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_SUSPEND_CMN_SETDEFAULT error.\n",__F__,__L__);

				ret = PE_CMN_SetDefaultSettings(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetDefaultSettings() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			do{
				PE_CMN_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				memcpy(&stParam,pRcvData,datasize);
				ret = PE_CMN_SetDebugSettings(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_SetDebugSettings() error.\n",__F__,__L__);
				PE_CMN_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CMN_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}

/**
 * standard function of cmn getting
 *
 * @param   data [in/out] unsigned int
 * @param   functype [in] unsigned int
 * @param   datasize [in] unsigned int
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_CMN_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize)
{
	int ret = RET_ERROR;

	/* data null check */
	PE_CMN_CHECK_CODE(!data,return RET_ERROR,"[%s,%d] data is null.\n",__F__,__L__);

	switch(functype)
	{
		case PE_ITEM_PKTFUNC(LX_PE_CMN_CONTRAST_T):
		{
			LX_PE_CMN_CONTRAST_T *pRcvData;
			LX_PE_CMN_CONTRAST_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_CONTRAST_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_CONTRAST_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_CONTRAST_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetContrast(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetContrast() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_CONTRAST_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_BRIGHTNESS_T):
		{
			LX_PE_CMN_BRIGHTNESS_T *pRcvData;
			LX_PE_CMN_BRIGHTNESS_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_BRIGHTNESS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_BRIGHTNESS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_BRIGHTNESS_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetBrightness(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetBrightness() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_BRIGHTNESS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_SATURATION_T):
		{
			LX_PE_CMN_SATURATION_T *pRcvData;
			LX_PE_CMN_SATURATION_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_SATURATION_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_SATURATION_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_SATURATION_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetSaturation(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetSaturation() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_SATURATION_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_HUE_T):
		{
			LX_PE_CMN_HUE_T *pRcvData;
			LX_PE_CMN_HUE_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_HUE_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_HUE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_HUE_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetHue(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetHue() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_HUE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_SIMUL_MODE_T):
		{
			LX_PE_CMN_SIMUL_MODE_T *pRcvData;
			LX_PE_CMN_SIMUL_MODE_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_SIMUL_MODE_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_SIMUL_MODE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_SIMUL_MODE_T *)data;
				ret = PE_CMN_GetSimultaneousMode(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetSimultaneousMode() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_SIMUL_MODE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_LEVEL_CTRL_T):
		{
			LX_PE_CMN_LEVEL_CTRL_T *pRcvData;
			LX_PE_CMN_LEVEL_CTRL_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_LEVEL_CTRL_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_LEVEL_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_LEVEL_CTRL_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetLevelCtrl(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetLevelCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_LEVEL_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_SUB_CONTRAST_T):
		{
			LX_PE_CMN_SUB_CONTRAST_T *pRcvData;
			LX_PE_CMN_SUB_CONTRAST_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_SUB_CONTRAST_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_SUB_CONTRAST_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_SUB_CONTRAST_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetSubContrast(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetSubContrast() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_SUB_CONTRAST_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_FWI_CTRL_T):
		{
			LX_PE_CMN_FWI_CTRL_T *pRcvData;
			LX_PE_CMN_FWI_CTRL_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_FWI_CTRL_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_FWI_CTRL_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_FWI_CTRL_T *)data;
				ret = PE_CMN_GetFwiCtrl(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetFwiCtrl() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_FWI_CTRL_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_MUTE_STATE_T):
		{
			LX_PE_CMN_MUTE_STATE_T *pRcvData;
			LX_PE_CMN_MUTE_STATE_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_MUTE_STATE_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_MUTE_STATE_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_MUTE_STATE_T *)data;
				ret = PE_CMN_GetMuteState(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetMuteState() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_MUTE_STATE_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_CMN_BYPASS_T):
		{
			LX_PE_CMN_BYPASS_T *pRcvData;
			LX_PE_CMN_BYPASS_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_CMN_BYPASS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_CMN_BYPASS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_CMN_BYPASS_T *)data;
				ret = PE_CMN_GetPicEnhanceBypass(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetPicEnhanceBypass() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_CMN_BYPASS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T):
		{
			LX_PE_DEFAULT_SETTINGS_T *pRcvData;
			LX_PE_DEFAULT_SETTINGS_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_DEFAULT_SETTINGS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_DEFAULT_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DEFAULT_SETTINGS_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetDefaultSettings(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetDefaultSettings() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_DEFAULT_SETTINGS_T\n");
			}while(0);
		}
		break;
		case PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T):
		{
			LX_PE_DBG_SETTINGS_T *pRcvData;
			LX_PE_DBG_SETTINGS_T stParam;
			memset0(stParam);
			do{
				PE_CMN_DBG_PRINT("S LX_PE_DBG_SETTINGS_T\n");
				/* data size check */
				PE_CMN_CHECK_CODE(datasize!=sizeof(LX_PE_DBG_SETTINGS_T),break,"[%s,%d] size mismatch.\n",__F__,__L__);

				pRcvData = (LX_PE_DBG_SETTINGS_T *)data;
				stParam.win_id = pRcvData->win_id;
				ret = PE_CMN_GetDebugSettings(&stParam);
				PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_CMN_GetDebugSettings() error.\n",__F__,__L__);
				memcpy(pRcvData,&stParam,datasize);
				PE_CMN_DBG_PRINT("E LX_PE_DBG_SETTINGS_T\n");
			}while(0);
		}
		break;
		default:
		{
			PE_CMN_ERROR("[%s,%d] functype default case.\n",__F__,__L__);
			ret = RET_ERROR;
		}
		break;
	}

	return ret;
}
static int PE_CMN_SetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);
		
		/* inform src info to fw */
		ret = PE_ETC_InformSrcInfo(pstParams);
		PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_InformSrcInfo() error.\n",__F__,__L__);

		if(PE_CHECK_MAIN(pstParams->win_id))
		{
			if(_g_cmn_default[LX_PE_WIN_MAIN].src_type != pstParams->src_type)
				_g_cmn_default[LX_PE_WIN_MAIN].src_type = pstParams->src_type;
			if(_g_cmn_default[LX_PE_WIN_MAIN].fmt_type != pstParams->fmt_type)
				_g_cmn_default[LX_PE_WIN_MAIN].fmt_type = pstParams->fmt_type;
			if(_g_cmn_default[LX_PE_WIN_MAIN].out_type != pstParams->out_type)
				_g_cmn_default[LX_PE_WIN_MAIN].out_type = pstParams->out_type;
			if(_g_cmn_default[LX_PE_WIN_MAIN].fr_type != pstParams->fr_type)
				_g_cmn_default[LX_PE_WIN_MAIN].fr_type = pstParams->fr_type;
			if(_g_cmn_default[LX_PE_WIN_MAIN].scan_type != pstParams->scan_type)
				_g_cmn_default[LX_PE_WIN_MAIN].scan_type = pstParams->scan_type;
			if(_g_cmn_default[LX_PE_WIN_MAIN].dtv_type != pstParams->dtv_type)
				_g_cmn_default[LX_PE_WIN_MAIN].dtv_type = pstParams->dtv_type;
		}
		if(PE_CHECK_SUB(pstParams->win_id))
		{
			if(_g_cmn_default[LX_PE_WIN_SUB].src_type != pstParams->src_type)
				_g_cmn_default[LX_PE_WIN_SUB].src_type = pstParams->src_type;
			if(_g_cmn_default[LX_PE_WIN_SUB].fmt_type != pstParams->fmt_type)
				_g_cmn_default[LX_PE_WIN_SUB].fmt_type = pstParams->fmt_type;
			if(_g_cmn_default[LX_PE_WIN_SUB].out_type != pstParams->out_type)
				_g_cmn_default[LX_PE_WIN_SUB].out_type = pstParams->out_type;
			if(_g_cmn_default[LX_PE_WIN_SUB].fr_type != pstParams->fr_type)
				_g_cmn_default[LX_PE_WIN_SUB].fr_type = pstParams->fr_type;
			if(_g_cmn_default[LX_PE_WIN_SUB].scan_type != pstParams->scan_type)
				_g_cmn_default[LX_PE_WIN_SUB].scan_type = pstParams->scan_type;
			if(_g_cmn_default[LX_PE_WIN_SUB].dtv_type != pstParams->dtv_type)
				_g_cmn_default[LX_PE_WIN_SUB].dtv_type = pstParams->dtv_type;
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetDefaultSettings(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);
		if(PE_CHECK_MAIN(win_id))
		{
			pstParams->src_type = _g_cmn_default[LX_PE_WIN_MAIN].src_type;
			pstParams->fmt_type = _g_cmn_default[LX_PE_WIN_MAIN].fmt_type;
			pstParams->out_type = _g_cmn_default[LX_PE_WIN_MAIN].out_type;
			pstParams->fr_type = _g_cmn_default[LX_PE_WIN_MAIN].fr_type;
			pstParams->scan_type = _g_cmn_default[LX_PE_WIN_MAIN].scan_type;
			pstParams->dtv_type = _g_cmn_default[LX_PE_WIN_MAIN].dtv_type;
		}
		if(PE_CHECK_SUB(win_id))
		{
			pstParams->src_type = _g_cmn_default[LX_PE_WIN_SUB].src_type;
			pstParams->fmt_type = _g_cmn_default[LX_PE_WIN_SUB].fmt_type;
			pstParams->out_type = _g_cmn_default[LX_PE_WIN_SUB].out_type;
			pstParams->fr_type = _g_cmn_default[LX_PE_WIN_SUB].fr_type;
			pstParams->scan_type = _g_cmn_default[LX_PE_WIN_SUB].scan_type;
			pstParams->dtv_type = _g_cmn_default[LX_PE_WIN_SUB].dtv_type;
		}
		PE_CMN_DBG_PRINT("[win id:%d] src:%d, fmt:%d, out:%d, fr:%d, scan:%d, dtv:%d\n",\
			pstParams->win_id,pstParams->src_type,pstParams->fmt_type,pstParams->out_type,\
			pstParams->fr_type,pstParams->scan_type,pstParams->dtv_type);
	}while(0);
	return ret;
}
static int PE_CMN_SetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("win id : %d, type:0x%x, [0x%08x] print level : 0x%08x, bypass : 0x%08x\n",\
			pstParams->win_id,pstParams->type,(0x1<<PE_ITEM_PKTMODL(CMN)),pstParams->print_lvl,pstParams->bypass);

		/* set print level */
		if(pstParams->type&LX_PE_DBG_LEVEL)
			_g_cmn_trace = (pstParams->print_lvl & (0x1<<PE_ITEM_PKTMODL(CMN)))? 0x1:0x0;

		/* set bypass */
		if(pstParams->type&LX_PE_DBG_BYPASS)
		{
			if(PE_CHIP_VER_GT(L9,B0))
			{
				if(pstParams->bypass & (0x1<<PE_ITEM_PKTMODL(CMN)))
				{
					PE_CMN_DBG_PRINT("cont,bri,sat disable, hue bypass.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9B0_Wr01(pe1_l_level_ctrl_00,	enable,			0x0);
						PE_P1L_L9B0_Wr01(pe1_l_vspyc_ctrl_00,	enable,			0x0);
						PE_P1L_L9B0_Wr01(pe1_l_vsprgb_ctrl_00,	enable,			0x0);
						PE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_03,		ihsv_hoffset,	0x80);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9B0_Wr01(pe1_r_level_ctrl_00,	enable,			0x0);
						PE_P1R_L9B0_Wr01(pe1_r_vspyc_ctrl_00,	enable,			0x0);
						PE_P1R_L9B0_Wr01(pe1_r_vsprgb_ctrl_00,	enable,			0x0);
						PE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_03,		ihsv_hoffset,	0x80);
					}
				}
				else
				{
					PE_CMN_DBG_PRINT("cont,bri,sat enable.\n");
					if(PE_CHECK_MAIN(pstParams->win_id))
					{
						PE_P1L_L9B0_Wr01(pe1_l_level_ctrl_00,	enable,			0x1);
						PE_P1L_L9B0_Wr01(pe1_l_vspyc_ctrl_00,	enable,			0x1);
						PE_P1L_L9B0_Wr01(pe1_l_vsprgb_ctrl_00,	enable,			0x1);
					}
					if(PE_CHECK_SUB(pstParams->win_id))
					{
						PE_P1R_L9B0_Wr01(pe1_r_level_ctrl_00,	enable,			0x1);
						PE_P1R_L9B0_Wr01(pe1_r_vspyc_ctrl_00,	enable,			0x1);
						PE_P1R_L9B0_Wr01(pe1_r_vsprgb_ctrl_00,	enable,			0x1);
					}
				}
			}
			else
			{
				PE_CMN_DBG_PRINT("do nothing.\n");	ret = RET_OK;
			}
		}

		if(PE_CHECK_MAIN(pstParams->win_id))
		{
			_g_cmn_dbg[LX_PE_WIN_MAIN].type = pstParams->type;
			_g_cmn_dbg[LX_PE_WIN_MAIN].print_lvl = pstParams->print_lvl;
			_g_cmn_dbg[LX_PE_WIN_MAIN].bypass = pstParams->bypass;
		}
		if(PE_CHECK_SUB(pstParams->win_id))
		{
			_g_cmn_dbg[LX_PE_WIN_SUB].type = pstParams->type;
			_g_cmn_dbg[LX_PE_WIN_SUB].print_lvl = pstParams->print_lvl;
			_g_cmn_dbg[LX_PE_WIN_SUB].bypass = pstParams->bypass;
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetDebugSettings(LX_PE_DBG_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);
		if(PE_CHECK_MAIN(win_id))
		{
			pstParams->type = _g_cmn_dbg[LX_PE_WIN_MAIN].type;
			pstParams->print_lvl = _g_cmn_dbg[LX_PE_WIN_MAIN].print_lvl;
			pstParams->bypass = _g_cmn_dbg[LX_PE_WIN_MAIN].bypass;
		}
		if(PE_CHECK_SUB(win_id))
		{
			pstParams->type = _g_cmn_dbg[LX_PE_WIN_SUB].type;
			pstParams->print_lvl = _g_cmn_dbg[LX_PE_WIN_SUB].print_lvl;
			pstParams->bypass = _g_cmn_dbg[LX_PE_WIN_SUB].bypass;
		}
		PE_CMN_DBG_PRINT("win id : %d, type : 0x%x, print level : 0x%x, bypass : 0x%x\n",\
			pstParams->win_id,pstParams->type,pstParams->print_lvl,pstParams->bypass);
	}while(0);
	return ret;
}
/**
 * set contrast
 *
 * @param   *pstParams [in] LX_PE_CMN_CONTRAST_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int PE_CMN_SetContrast(LX_PE_CMN_CONTRAST_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : %d\n",pstParams->win_id,pstParams->contrast);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Wr01(pe1_l_vspyc_ctrl_00,	contrast,	GET_BITS(pstParams->contrast,0,10));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Wr01(pe1_r_vspyc_ctrl_00,	contrast,	GET_BITS(pstParams->contrast,0,10));
			}
		}
		else
		{
			ret = PE_FWI_SETDATA(PQL_IPC_SET_CONTRAST, pstParams, sizeof(LX_PE_CMN_CONTRAST_T));
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PQL_IPC_SET_CONTRAST error.\n",__F__,__L__);
			PE_CMN_DBG_PRINT("win[%d] set f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				_g_cmn_contrast[LX_PE_WIN_MAIN]=pstParams->contrast;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				_g_cmn_contrast[LX_PE_WIN_SUB]=pstParams->contrast;
			}
		}
	}while(0);
	return ret;
}

/**
 * get contrast
 *
 * @param   *pstParams [in/out] LX_PE_CMN_CONTRAST_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int PE_CMN_GetContrast(LX_PE_CMN_CONTRAST_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Rd01(pe1_l_vspyc_ctrl_00, contrast,	pstParams->contrast);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Rd01(pe1_r_vspyc_ctrl_00, contrast,	pstParams->contrast);
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("win[%d] set f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->contrast=_g_cmn_contrast[LX_PE_WIN_MAIN];
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->contrast=_g_cmn_contrast[LX_PE_WIN_SUB];
			}
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : %d\n",pstParams->win_id,pstParams->contrast);
	}while(0);
	return ret;
}
static int PE_CMN_SetBrightness(LX_PE_CMN_BRIGHTNESS_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : %d\n",pstParams->win_id,pstParams->brightness);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_vspyc_ctrl_01);
				DE_P1L_L9B0_Wr01(pe1_l_vspyc_ctrl_01, brightness,	GET_BITS(pstParams->brightness,0,10));
				DE_P1L_L9B0_WrFL(pe1_l_vspyc_ctrl_01);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_vspyc_ctrl_01);
				DE_P1R_L9B0_Wr01(pe1_r_vspyc_ctrl_01, brightness,	GET_BITS(pstParams->brightness,0,10));
				DE_P1R_L9B0_WrFL(pe1_r_vspyc_ctrl_01);
			}
		}
		else
		{
			ret = PE_FWI_SETDATA(PQL_IPC_SET_BRIGHTNESS, pstParams, sizeof(LX_PE_CMN_BRIGHTNESS_T));
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PQL_IPC_SET_BRIGHTNESS error.\n",__F__,__L__);
			PE_CMN_DBG_PRINT("win[%d] set f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				_g_cmn_brightness[LX_PE_WIN_MAIN]=pstParams->brightness;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				_g_cmn_brightness[LX_PE_WIN_SUB]=pstParams->brightness;
			}
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetBrightness(LX_PE_CMN_BRIGHTNESS_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_vspyc_ctrl_01);
				DE_P1L_L9B0_Rd01(pe1_l_vspyc_ctrl_01, brightness,	pstParams->brightness);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_vspyc_ctrl_01);
				DE_P1R_L9B0_Rd01(pe1_r_vspyc_ctrl_01, brightness,	pstParams->brightness);
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("win[%d] get f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->brightness=_g_cmn_brightness[LX_PE_WIN_MAIN];
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->brightness=_g_cmn_brightness[LX_PE_WIN_SUB];
			}
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : %d\n",pstParams->win_id,pstParams->brightness);
	}while(0);

	return ret;
}
static int PE_CMN_SetSaturation(LX_PE_CMN_SATURATION_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : %d\n",pstParams->win_id,pstParams->saturation);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_vspyc_ctrl_01);
				DE_P1L_L9B0_Wr01(pe1_l_vspyc_ctrl_01, saturation,	GET_BITS(pstParams->saturation,0,8));
				DE_P1L_L9B0_WrFL(pe1_l_vspyc_ctrl_01);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_vspyc_ctrl_01);
				DE_P1R_L9B0_Wr01(pe1_r_vspyc_ctrl_01, saturation,	GET_BITS(pstParams->saturation,0,8));
				DE_P1R_L9B0_WrFL(pe1_r_vspyc_ctrl_01);
			}
		}
		else
		{
			ret = PE_FWI_SETDATA(PQL_IPC_SET_SATURATION, pstParams, sizeof(LX_PE_CMN_SATURATION_T));
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PQL_IPC_SET_SATURATION error.\n",__F__,__L__);
			PE_CMN_DBG_PRINT("win[%d] set f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				_g_cmn_saturation[LX_PE_WIN_MAIN]=pstParams->saturation;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				_g_cmn_saturation[LX_PE_WIN_SUB]=pstParams->saturation;
			}
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetSaturation(LX_PE_CMN_SATURATION_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_vspyc_ctrl_01);
				DE_P1L_L9B0_Rd01(pe1_l_vspyc_ctrl_01, saturation,	pstParams->saturation);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_vspyc_ctrl_01);
				DE_P1R_L9B0_Rd01(pe1_r_vspyc_ctrl_01, saturation,	pstParams->saturation);
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("win[%d] get f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->saturation=_g_cmn_saturation[LX_PE_WIN_MAIN];
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->saturation=_g_cmn_saturation[LX_PE_WIN_SUB];
			}
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : %d\n",pstParams->win_id,pstParams->saturation);
	}while(0);

	return ret;
}
static int PE_CMN_SetHue(LX_PE_CMN_HUE_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : %d\n",pstParams->win_id,pstParams->hue);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_03);
				DE_P1L_L9B0_Wr01(pe1_l_cen_ctrl_03, ihsv_hoffset,	GET_BITS(pstParams->hue,0,8));
				DE_P1L_L9B0_WrFL(pe1_l_cen_ctrl_03);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_03);
				DE_P1R_L9B0_Wr01(pe1_r_cen_ctrl_03, ihsv_hoffset,	GET_BITS(pstParams->hue,0,8));
				DE_P1R_L9B0_WrFL(pe1_r_cen_ctrl_03);
			}
		}
		else
		{
			ret = PE_FWI_SETDATA(PQL_IPC_SET_HUE, pstParams, sizeof(LX_PE_CMN_HUE_T));
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PQL_IPC_SET_HUE error.\n",__F__,__L__);
			PE_CMN_DBG_PRINT("win[%d] set f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				_g_cmn_hue[LX_PE_WIN_MAIN]=pstParams->hue;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				_g_cmn_hue[LX_PE_WIN_SUB]=pstParams->hue;
			}
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetHue(LX_PE_CMN_HUE_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_cen_ctrl_03);
				DE_P1L_L9B0_Rd01(pe1_l_cen_ctrl_03, ihsv_hoffset,	pstParams->hue);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_cen_ctrl_03);
				DE_P1R_L9B0_Rd01(pe1_r_cen_ctrl_03, ihsv_hoffset,	pstParams->hue);
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("win[%d] get f/w, do nothing here.\n",pstParams->win_id);
			if(PE_CHECK_MAIN(win_id))
			{
				pstParams->hue=_g_cmn_hue[LX_PE_WIN_MAIN];
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->hue=_g_cmn_hue[LX_PE_WIN_SUB];
			}
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : %d\n",pstParams->win_id,pstParams->hue);
	}while(0);

	return ret;
}
static int PE_CMN_SetSimultaneousMode(LX_PE_CMN_SIMUL_MODE_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		PE_CMN_DBG_PRINT("set pstParams : %d\n",pstParams->on_off);
		if(pstParams->on_off)
		{
			ret = PE_ETC_StartSimulSetMode(PE_SIMUL_MODE_ALL);
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_StartSimulSetMode() error.\n",__F__,__L__);
			_g_cmn_simul_mode = 1;
		}
		else
		{
			ret = PE_ETC_EndSimulSetMode();
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_EndSimulSetMode() error.\n",__F__,__L__);
			_g_cmn_simul_mode = 0;
		}
	}while(0);

	return ret;
}
static int PE_CMN_GetSimultaneousMode(LX_PE_CMN_SIMUL_MODE_T *pstParams)
{
	int ret = RET_OK;
	do{
		CHECK_KNULL(pstParams);
		pstParams->on_off = _g_cmn_simul_mode;
		PE_CMN_DBG_PRINT("get pstParams : %d\n",pstParams->on_off);
	}while(0);

	return ret;
}
static int PE_CMN_SetLevelCtrl(LX_PE_CMN_LEVEL_CTRL_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : ofs:%d, center:%d, gain:%d\n",\
			pstParams->win_id,pstParams->offset,pstParams->center,pstParams->gain);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_level_ctrl_00);
				DE_P1L_L9B0_Wr01(pe1_l_level_ctrl_00, y_offset,			GET_BITS(pstParams->offset,0,10));
				DE_P1L_L9B0_Wr01(pe1_l_level_ctrl_00, center_position,	GET_BITS(pstParams->center,0,8));
				DE_P1L_L9B0_Wr01(pe1_l_level_ctrl_00, y_gain,			GET_BITS(pstParams->gain,0,8));
				DE_P1L_L9B0_WrFL(pe1_l_level_ctrl_00);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_level_ctrl_00);
				DE_P1R_L9B0_Wr01(pe1_r_level_ctrl_00, y_offset,			GET_BITS(pstParams->offset,0,10));
				DE_P1R_L9B0_Wr01(pe1_r_level_ctrl_00, center_position,	GET_BITS(pstParams->center,0,8));
				DE_P1R_L9B0_Wr01(pe1_r_level_ctrl_00, y_gain,			GET_BITS(pstParams->gain,0,8));
				DE_P1R_L9B0_WrFL(pe1_r_level_ctrl_00);
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetLevelCtrl(LX_PE_CMN_LEVEL_CTRL_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				DE_P1L_L9B0_RdFL(pe1_l_level_ctrl_00);
				DE_P1L_L9B0_Rd01(pe1_l_level_ctrl_00, y_offset,			pstParams->offset);
				DE_P1L_L9B0_Rd01(pe1_l_level_ctrl_00, center_position,	pstParams->center);
				DE_P1L_L9B0_Rd01(pe1_l_level_ctrl_00, y_gain,			pstParams->gain);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				DE_P1R_L9B0_RdFL(pe1_r_level_ctrl_00);
				DE_P1R_L9B0_Rd01(pe1_r_level_ctrl_00, y_offset,			pstParams->offset);
				DE_P1R_L9B0_Rd01(pe1_r_level_ctrl_00, center_position,	pstParams->center);
				DE_P1R_L9B0_Rd01(pe1_r_level_ctrl_00, y_gain,			pstParams->gain);
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : ofs:%d, center:%d, gain:%d\n",\
			pstParams->win_id,pstParams->offset,pstParams->center,pstParams->gain);
	}while(0);

	return ret;
}
static int PE_CMN_SetSubContrast(LX_PE_CMN_SUB_CONTRAST_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : cont:%d\n",pstParams->win_id,pstParams->contrast);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Wr01(pe1_l_vsprgb_ctrl_00,	contrast,	GET_BITS(pstParams->contrast,0,10));
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Wr01(pe1_r_vsprgb_ctrl_00,	contrast,	GET_BITS(pstParams->contrast,0,10));
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetSubContrast(LX_PE_CMN_SUB_CONTRAST_T *pstParams)
{
	int ret = RET_OK;
	LX_PE_WIN_ID win_id;
	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		win_id = PE_GET_CHECKED_WINID(pstParams->win_id);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				PE_P1L_L9B0_Rd01(pe1_l_vsprgb_ctrl_00,	contrast,	pstParams->contrast);
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				PE_P1R_L9B0_Rd01(pe1_r_vsprgb_ctrl_00,	contrast,	pstParams->contrast);
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : cont:%d\n",pstParams->win_id,pstParams->contrast);
	}while(0);

	return ret;
}
static int PE_CMN_SetFwiCtrl(LX_PE_CMN_FWI_CTRL_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CMN_DBG_PRINT("set pstParams : use_fw_en:%d, fw_ctrl_en:%d, dbg_print_en:%d\n",\
			pstParams->use_fw_en,pstParams->fw_ctrl_en,pstParams->dbg_print_en);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			g_pe_use_fw = pstParams->use_fw_en;
			ret = PE_ETC_SetPqFwCtrlEnable(pstParams->fw_ctrl_en);
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwCtrlEnable() error.\n",__F__,__L__);
			PE_ETC_SetDbgPrintOnOff((pstParams->dbg_print_en)? 0x1:0x0);
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetFwiCtrl(LX_PE_CMN_FWI_CTRL_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			pstParams->use_fw_en = g_pe_use_fw;
			pstParams->fw_ctrl_en = PE_ETC_GetPqFwCtrlEnable();
			pstParams->dbg_print_en = PE_ETC_GetDbgPrintOnOff();
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}

		PE_CMN_DBG_PRINT("get pstParams : use_fw_en:%d, fw_ctrl_en:%d, dbg_print_en:%d\n",\
			pstParams->use_fw_en,pstParams->fw_ctrl_en,pstParams->dbg_print_en);
	}while(0);

	return ret;
}
static int PE_CMN_SetMuteState(LX_PE_CMN_MUTE_STATE_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : on_off:%d\n",pstParams->win_id,pstParams->on_off);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			ret = PE_ETC_InformMuteState(pstParams);
			PE_CMN_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_InformMuteState() error.\n",__F__,__L__);
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				_g_cmn_mute_state[LX_PE_WIN_MAIN].on_off = pstParams->on_off;
				_g_cmn_cur_settings[LX_PE_WIN_MAIN].mute_en = pstParams->on_off;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				_g_cmn_mute_state[LX_PE_WIN_SUB].on_off = pstParams->on_off;
				_g_cmn_cur_settings[LX_PE_WIN_SUB].mute_en = pstParams->on_off;
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetMuteState(LX_PE_CMN_MUTE_STATE_T *pstParams)
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
				pstParams->on_off = _g_cmn_mute_state[LX_PE_WIN_MAIN].on_off;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->on_off = _g_cmn_mute_state[LX_PE_WIN_SUB].on_off;
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : on_off:%d\n",pstParams->win_id,pstParams->on_off);
	}while(0);

	return ret;
}
static int PE_CMN_SetPicEnhanceBypass(LX_PE_CMN_BYPASS_T *pstParams)
{
	int ret = RET_OK;

	do{
		CHECK_KNULL(pstParams);
		PE_CHECK_WINID(pstParams->win_id);
		PE_CMN_DBG_PRINT("set pstParams[%d] : front_bypass_en:%d\n",pstParams->win_id,pstParams->front_bypass_en);

		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(PE_CHECK_MAIN(pstParams->win_id))
			{
				_g_cmn_bypass_settings[LX_PE_WIN_MAIN].front_bypass_en = (pstParams->front_bypass_en)? 0x1:0x0;
			}
			if(PE_CHECK_SUB(pstParams->win_id))
			{
				_g_cmn_bypass_settings[LX_PE_WIN_SUB].front_bypass_en = (pstParams->front_bypass_en)? 0x1:0x0;
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
static int PE_CMN_GetPicEnhanceBypass(LX_PE_CMN_BYPASS_T *pstParams)
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
				pstParams->front_bypass_en = _g_cmn_bypass_settings[LX_PE_WIN_MAIN].front_bypass_en;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->front_bypass_en = _g_cmn_bypass_settings[LX_PE_WIN_SUB].front_bypass_en;
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}

		PE_CMN_DBG_PRINT("get pstParams[%d] : front_bypass_en:%d\n",pstParams->win_id,pstParams->front_bypass_en);
	}while(0);

	return ret;
}
int PE_CMN_GetCurCmnSettings(PE_CMN_SETTINGS_T *pstParams)
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
				pstParams->mute_en	= _g_cmn_cur_settings[LX_PE_WIN_MAIN].mute_en;
			}
			if(PE_CHECK_SUB(win_id))
			{
				pstParams->mute_en	= _g_cmn_cur_settings[LX_PE_WIN_SUB].mute_en;
			}
			if(_g_cmn_trace)
			{
				if(count%240==0)
				{
					PE_CMN_DBG_PRINT("get pstParams[%d] : mute_en:%d\n",\
						pstParams->win_id,pstParams->mute_en);
				}
				if(count >= 240)	count=0;
				else				count++;
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do.\n");
		}
	} while (0);
	return ret;
}
int PE_CMN_SetFrontPeDetour(void)
{
	int ret = RET_OK;
	UINT32 l_data,r_data,detour_l_onoff,detour_r_onoff;
	static UINT32 count = 0;
	static UINT32 pre_l_data = 0xff;
	static UINT32 pre_r_data = 0xff;
	static UINT32 pre_detour_l_onoff = 0xff;
	static UINT32 pre_detour_r_onoff = 0xff;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			DE_P0L_L9B0_RdFL(pe0_opmode);
			DE_P0R_L9B0_RdFL(pe0_opmode);
			DE_P0L_L9B0_Rd01(pe0_opmode,	tnr_y_en,	l_data);
			DE_P0R_L9B0_Rd01(pe0_opmode,	tnr_y_en,	r_data);
			if(pre_l_data!=l_data)
			{
				if(l_data)	//use tnr memory
				{
					detour_l_onoff = 0x0;
				}
				else		//not use tnr memory
				{
					detour_l_onoff = 0x1;
				}
				_g_cmn_bypass_settings[LX_PE_WIN_MAIN].front_bypass_en = detour_l_onoff;	//reset
				PE_CMN_DBG_PRINT("[CH L][new tnr_y_en]tnr_y_en:%d->%d, by_set:%d, detour_l_onoff:%d->%d\n\n",\
					pre_l_data,l_data,_g_cmn_bypass_settings[LX_PE_WIN_MAIN].front_bypass_en,\
					pre_detour_l_onoff,detour_l_onoff);
				pre_l_data=l_data;
			}
			else
			{
				if(l_data)	//use tnr memory
				{
					detour_l_onoff = 0x0;
				}
				else		//not use tnr memory
				{
					detour_l_onoff = (_g_cmn_bypass_settings[LX_PE_WIN_MAIN].front_bypass_en)? 0x1:0x0;
				}
			}
			if(pre_detour_l_onoff != detour_l_onoff)
			{
				DE_P0L_L9B0_Wr01(pe0_opmode,	detour_enable,	detour_l_onoff);
				DE_P0L_L9B0_WrFL(pe0_opmode);
				PE_CMN_DBG_PRINT("[CH L][set detour]tnr_y_en:%d->%d, by_set:%d, detour_l_onoff:%d->%d\n\n",\
					pre_l_data,l_data,_g_cmn_bypass_settings[LX_PE_WIN_MAIN].front_bypass_en,\
					pre_detour_l_onoff,detour_l_onoff);
				pre_detour_l_onoff = detour_l_onoff;
			}
			if(pre_r_data!=r_data)
			{
				if(r_data)	//use tnr memory
				{
					detour_r_onoff = 0x0;
				}
				else		//not use tnr memory
				{
					detour_r_onoff = 0x1;
				}
				_g_cmn_bypass_settings[LX_PE_WIN_SUB].front_bypass_en = detour_r_onoff;	//reset
				PE_CMN_DBG_PRINT("[CH R][new tnr_y_en]tnr_y_en:%d->%d, by_set:%d, detour_r_onoff:%d->%d\n\n",\
					pre_r_data,r_data,_g_cmn_bypass_settings[LX_PE_WIN_SUB].front_bypass_en,\
					pre_detour_r_onoff,detour_r_onoff);
				pre_r_data=r_data;
			}
			else
			{
				if(r_data)	//use tnr memory
				{
					detour_r_onoff = 0x0;
				}
				else		//not use tnr memory
				{
					detour_r_onoff = (_g_cmn_bypass_settings[LX_PE_WIN_SUB].front_bypass_en)? 0x1:0x0;
				}
			}
			if(pre_detour_r_onoff != detour_r_onoff)
			{
				DE_P0R_L9B0_Wr01(pe0_opmode,	detour_enable,	detour_r_onoff);
				DE_P0R_L9B0_WrFL(pe0_opmode);
				PE_CMN_DBG_PRINT("[CH R][set detour]tnr_y_en:%d->%d, by_set:%d, detour_r_onoff:%d->%d\n\n",\
					pre_r_data,r_data,_g_cmn_bypass_settings[LX_PE_WIN_SUB].front_bypass_en,\
					pre_detour_r_onoff,detour_r_onoff);
				pre_detour_r_onoff = detour_r_onoff;
			}
			if(_g_cmn_trace)
			{
				if(count%240==0)
				{
					PE_CMN_DBG_PRINT("[CH L][monitoring]tnr_y_en:%d->%d, by_set:%d, detour_l_onoff:%d->%d\n",\
						pre_l_data,l_data,_g_cmn_bypass_settings[LX_PE_WIN_MAIN].front_bypass_en,\
						pre_detour_l_onoff,detour_l_onoff);
					PE_CMN_DBG_PRINT("[CH R][monitoring]tnr_y_en:%d->%d, by_set:%d, detour_r_onoff:%d->%d\n\n",\
						pre_r_data,r_data,_g_cmn_bypass_settings[LX_PE_WIN_SUB].front_bypass_en,\
						pre_detour_r_onoff,detour_r_onoff);
				}
				if(count >= 240)	count=0;
				else				count++;
			}
		}
		else
		{
			PE_CMN_DBG_PRINT("nothing to do\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}

