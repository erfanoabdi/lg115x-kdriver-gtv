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


/** @file pe_etc.c
 *
 *  driver for picture enhance etc functions. ( used only within kdriver )
 *	- functional functions regardless of modules
 *	
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note		
 *	@date		2011.07.16
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

#include "de_ctr_reg_l9.h"

#include "pe_cfg.h"
#include "pe_def.h"
#include "pe_etc.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_ETC_ERROR	printk

#define PE_ETC_DBG_PRINT(fmt,args...)	if(_g_etc_trace) printk("[%x][%s,%d] "fmt,PE_CHIP_VER,__F__,__L__,##args)
#define PE_ETC_CHECK_CODE(_checker,_action,fmt,args...)	{if(_checker){PE_ETC_ERROR(fmt,##args);_action;}}

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
extern DE_DEE_REG_L9_T gDE_DEE_L9;

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

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static UINT32 _g_etc_trace=0x0;	//default should be off.
static UINT32 _g_fwi_ctrl_enable = 0xff;
static UINT32 _g_pe_etc_mode_block = PE_SIMUL_MODE_NONE;
static UINT32 _g_fwi_win_id = 0;

/*========================================================================================
	Implementation Group
========================================================================================*/
void PE_ETC_SetDbgPrintOnOff(UINT32 on_off)
{
	_g_etc_trace = on_off;
	return;
}
UINT32 PE_ETC_GetDbgPrintOnOff(void)
{
	return _g_etc_trace;
}
/**
 * start all win simultaneous reg. setting mode
 *	- should call end mode after start mode, for used blocks.
 *
 * @param   mode_block [in] UINT32, see PE_SIMUL_MODE_PE0,SM,PE1
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_StartSimulSetMode(UINT32 mode_block)
{
	int ret = RET_OK;

	do{
		PE_ETC_DBG_PRINT(" block:0x%x, pre block:0x%x\n",mode_block,_g_pe_etc_mode_block);
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_ETC_DBG_PRINT("under construction.\n");	ret = RET_OK;
		}
		else
		{
			if(mode_block & PE_SIMUL_MODE_PE0)
			{
				DE_DEE_L9_RdFL(dee_pe0_hif_offset);
				DE_DEE_L9_Wr01(dee_pe0_hif_offset, pe0_hif_offset_en,	0x1);
				DE_DEE_L9_WrFL(dee_pe0_hif_offset);
			}
			if(mode_block & PE_SIMUL_MODE_SM)
			{
				DE_DEE_L9_RdFL(dee_sm_hif_offset);
				DE_DEE_L9_Wr01(dee_sm_hif_offset, sm_hif_offset_en,		0x1);
				DE_DEE_L9_WrFL(dee_sm_hif_offset);
			}
			if(mode_block & PE_SIMUL_MODE_PE1)
			{
				DE_DEE_L9_RdFL(dee_pe1_hif_offset);
				DE_DEE_L9_Wr01(dee_pe1_hif_offset, pe1_hif_offset_en,	0x1);
				DE_DEE_L9_WrFL(dee_pe1_hif_offset);
			}
			_g_pe_etc_mode_block = mode_block;
		}
	}while(0);

	return ret;
}

/**
 * end all win simultaneous reg. setting mode
 *	- should call end mode after start mode, for used blocks.
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_EndSimulSetMode(void)
{
	int ret = RET_OK;

	do{
		PE_ETC_DBG_PRINT("pre block:0x%x\n",_g_pe_etc_mode_block);
		if(PE_CHIP_VER_GT(L9,B0))
		{
			PE_ETC_DBG_PRINT("under construction.\n");	ret = RET_OK;
		}
		else
		{
			if(_g_pe_etc_mode_block & PE_SIMUL_MODE_PE0)
			{
				DE_DEE_L9_RdFL(dee_pe0_hif_offset);
				DE_DEE_L9_Wr01(dee_pe0_hif_offset, pe0_hif_offset_en,	0x0);
				DE_DEE_L9_WrFL(dee_pe0_hif_offset);
			}
			if(_g_pe_etc_mode_block & PE_SIMUL_MODE_SM)
			{
				DE_DEE_L9_RdFL(dee_sm_hif_offset);
				DE_DEE_L9_Wr01(dee_sm_hif_offset, sm_hif_offset_en,		0x0);
				DE_DEE_L9_WrFL(dee_sm_hif_offset);
			}
			if(_g_pe_etc_mode_block & PE_SIMUL_MODE_PE1)
			{
				DE_DEE_L9_RdFL(dee_pe1_hif_offset);
				DE_DEE_L9_Wr01(dee_pe1_hif_offset, pe1_hif_offset_en,	0x0);
				DE_DEE_L9_WrFL(dee_pe1_hif_offset);
			}
			_g_pe_etc_mode_block = PE_SIMUL_MODE_NONE;
		}
	}while(0);

	return ret;
}
/**
 * set pq f/w control enable
 *
 * @param   on_off [in] UINT32
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_SetPqFwCtrlEnable(UINT32 win_id)
{
	int ret = RET_OK;
	PE_FWI_PQ_TABLE_HEAD_T header;
	PE_FWI_L9_PQ_IPC_CTRL table;
	UINT32 tbl_size;
	UINT8 *pTable = NULL;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(_g_fwi_ctrl_enable!=win_id)
			{
			pTable = (UINT8 *)&table;
			tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = LX_PE_WIN_MAIN;	//PE_ETC_GetPqFwWindow();
			header.offset = PE_FWI_OFFSET(PE_FWI_L9_FW_CTRL);
			header.length = sizeof(PE_FWI_L9_FW_CTRL);
				table.fw_ctrl.enable = win_id;	//0,1,2:on(0:L, 1:R, 2:All), 0xff:off

				PE_ETC_DBG_PRINT(" wId:%d,ofs:%d,len:%d, tbl_size:%d, fw_en:%d->%d\n",header.win_id,\
					header.offset,header.length,tbl_size,_g_fwi_ctrl_enable,table.fw_ctrl.enable);

				_g_fwi_ctrl_enable = win_id;
			ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
			PE_ETC_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);
		}
		}
		else
		{
			PE_ETC_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	}while(0);

	return ret;
}
/**
 * get pq f/w control enable
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
UINT32 PE_ETC_GetPqFwCtrlEnable(void)
{
	return _g_fwi_ctrl_enable;
}
/**
 * set pq f/w win setting
 *
 * @param   win_id [in] UINT32
 * @return  void
 * @see
 * @author
 */
void PE_ETC_SetPqFwWindow(UINT32 win_id)
{
	int ret = RET_OK;
	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
	_g_fwi_win_id = win_id;
			ret = PE_ETC_SetPqFwCtrlEnable(win_id);
			PE_ETC_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwCtrlEnable() error.\n",__F__,__L__);
		}
		else
		{
			PE_ETC_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	}while(0);
	return;
}
/**
 * get pq f/w win setting
 *
 * @param   void
 * @return  UINT32 win id.
 * @see
 * @author
 */
UINT32 PE_ETC_GetPqFwWindow(void)
{
	return _g_fwi_win_id;
}
/**
 * inform mute state to f/w
 *
 * @param   *pstParams [in] LX_PE_CMN_MUTE_STATE_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_InformMuteState(LX_PE_CMN_MUTE_STATE_T *pstParams)
{
	int ret = RET_OK;
	PE_FWI_PQ_TABLE_HEAD_T header;
	PE_FWI_L9_PQ_IPC_CTRL table;
	UINT32 tbl_size;
	UINT8 *pTable = NULL;

	do{
		CHECK_KNULL(pstParams);
		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(pstParams->win_id!=LX_PE_WIN_MAIN || !g_pe_use_fw)
			{
				PE_ETC_DBG_PRINT("g_pe_use_fw:%d, wId:%d, en:%d, just return\n",\
					g_pe_use_fw,pstParams->win_id, pstParams->on_off);
				ret = RET_OK;
				break;
			}
			pTable = (UINT8 *)&table;
			tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
			header.win_id = LX_PE_WIN_MAIN;	//PE_ETC_GetPqFwWindow();
			header.offset = PE_FWI_OFFSET(PE_FWI_L9_MUTE_CTRL);
			header.length = sizeof(PE_FWI_L9_MUTE_CTRL);
			table.mute_ctrl.mute_enable = (pstParams->on_off)? 1:0;	// on, off

			PE_ETC_DBG_PRINT(" wId:%d,ofs:%d,len:%d, tbl_size:%d, mute_en:%d\n",\
				header.win_id,header.offset,header.length,tbl_size,table.mute_ctrl.mute_enable);
			ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
			PE_ETC_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);
		}
		else
		{
			PE_ETC_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
/**
 * inform tnr auto mode to f/w
 *
 * @param   *pstParams [in] LX_PE_NRD_TNR_CMN_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_InformTnrAutoMode(LX_PE_NRD_TNR_CMN_T *pstParams)
{
	int ret = RET_OK;
	PE_FWI_PQ_TABLE_HEAD_T header;
	PE_FWI_L9_PQ_IPC_CTRL table;
	UINT32 tbl_size;
	UINT8 *pTable = NULL;
	static UINT32 pre_auto_mode = 0xff;

	do{
		CHECK_KNULL(pstParams);
		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(!PE_CHECK_MAIN(pstParams->win_id) || !g_pe_use_fw)
			{
				PE_ETC_DBG_PRINT("g_pe_use_fw:%d, wId:%d, tnr auto en:%d, just return\n",\
					g_pe_use_fw,pstParams->win_id, pstParams->auto_mode);
				ret = RET_OK;
				break;
			}
			if(pre_auto_mode!=pstParams->auto_mode)
			{
			pTable = (UINT8 *)&table;
			tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = LX_PE_WIN_MAIN;	//PE_ETC_GetPqFwWindow();
			header.offset = PE_FWI_OFFSET(PE_FWI_L9_TNR_CTRL);
			header.length = sizeof(PE_FWI_L9_TNR_CTRL);
			table.tnr_ctrl.auto_enable = (pstParams->auto_mode)? 1:0;	// on, off

				PE_ETC_DBG_PRINT(" wId:%d,ofs:%d,len:%d, tbl_size:%d, tnr_auto:%d -> %d\n",header.win_id,\
					header.offset,header.length,tbl_size,pre_auto_mode,table.tnr_ctrl.auto_enable);

				pre_auto_mode = (pstParams->auto_mode)? 1:0;	// on, off
			ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
			PE_ETC_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);
		}
		else
		{
				PE_ETC_DBG_PRINT(" tnr_auto:%d -> %d, just return\n",\
					pre_auto_mode,pstParams->auto_mode);
			}
		}
		else
		{
			PE_ETC_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
/**
 * inform scaler setting count to f/w
 *
 * @param   cnt [in] UINT32
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_InformSclSettingCnt(UINT32 cnt)
{
	int ret = RET_OK;
	PE_FWI_PQ_TABLE_HEAD_T header;
	PE_FWI_L9_PQ_IPC_CTRL table;
	UINT32 tbl_size;
	UINT8 *pTable = NULL;

	do{
		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(!g_pe_use_fw)
			{
				PE_ETC_DBG_PRINT("g_pe_use_fw:%d, cnt:%d, just return\n",g_pe_use_fw,cnt);
				ret = RET_OK;
				break;
			}
			pTable = (UINT8 *)&table;
			tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
			header.win_id = LX_PE_WIN_MAIN;	//PE_ETC_GetPqFwWindow();
			header.offset = PE_FWI_OFFSET(PE_FWI_L9_SCL_CTRL);
			header.length = sizeof(PE_FWI_L9_SCL_CTRL);
			table.scl_ctrl.set_cnt = (cnt>0xff)? 0xff:cnt;

			PE_ETC_DBG_PRINT(" wId:%d,ofs:%d,len:%d, tbl_size:%d, set_cnt:%d\n",\
				header.win_id,header.offset,header.length,tbl_size,table.scl_ctrl.set_cnt);
			ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
			PE_ETC_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);
		}
		else
		{
			PE_ETC_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
/**
 * inform src info to f/w
 *	- dtv play type : normal, file, hdd
 *
 * @param   *pstParams [in] LX_PE_DEFAULT_SETTINGS_T
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_InformSrcInfo(LX_PE_DEFAULT_SETTINGS_T *pstParams)
{
	int ret = RET_OK;
	PE_FWI_PQ_TABLE_HEAD_T header;
	PE_FWI_L9_PQ_IPC_CTRL table;
	UINT32 tbl_size;
	UINT8 *pTable = NULL;
	static LX_PE_DTV_TYPE	pre_dtv_type = LX_PE_DTV_NUM;

	do{
		CHECK_KNULL(pstParams);
		PE_ETC_CHECK_CODE(pstParams->dtv_type>=LX_PE_DTV_NUM,ret=RET_ERROR;break,\
			"[%s,%d] dtv_type(%d) is over.\n",__F__,__L__,pstParams->dtv_type);
		if(PE_CHIP_VER_GT(L9,B0))
		{
			if(pstParams->win_id!=LX_PE_WIN_MAIN || !g_pe_use_fw)
			{
				PE_ETC_DBG_PRINT("g_pe_use_fw:%d, wId:%d, dtv_type:%d, just return\n",\
					g_pe_use_fw,pstParams->win_id,pstParams->dtv_type);
				ret = RET_OK;
				break;
			}
			if(pre_dtv_type!=pstParams->dtv_type)
			{
				pTable = (UINT8 *)&table;
				tbl_size = sizeof(PE_FWI_L9_PQ_IPC_CTRL);
				header.win_id = LX_PE_WIN_MAIN;
				header.offset = PE_FWI_OFFSET(PE_FWI_L9_SRC_INFO);
				header.length = sizeof(PE_FWI_L9_SRC_INFO);
				table.src_info.dtv_play_type = (UINT8)(pstParams->dtv_type);

				PE_ETC_DBG_PRINT(" wId:%d,ofs:%d,len:%d, tbl_size:%d, dtv_type:%d -> %d\n",header.win_id,\
					header.offset,header.length,tbl_size,pre_dtv_type,table.src_info.dtv_play_type);

				pre_dtv_type=pstParams->dtv_type;
				ret = PE_ETC_SetPqFwSettings(header, pTable, tbl_size);
				PE_ETC_CHECK_CODE(ret,break,"[%s,%d] PE_ETC_SetPqFwSettings() error.\n",__F__,__L__);
			}
			else
			{
				PE_ETC_DBG_PRINT(" dtv type:%d -> %d, just return\n",\
					pre_dtv_type,pstParams->dtv_type);
			}
		}
		else
		{
			PE_ETC_DBG_PRINT("nothing to do.\n");	ret = RET_OK;
		}
	}while(0);
	return ret;
}
/**
 * set pq f/w settings
 *
 * @param   header [in] PE_FWI_PQ_TABLE_HEAD_T
 * @param   *table [in] UINT8
 * @param   tbl_size [in] UINT32
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_ETC_SetPqFwSettings(PE_FWI_PQ_TABLE_HEAD_T header, UINT8 *pTable, UINT32 tblSize)
{
	int ret = RET_OK;
	UINT32 offset;
	UINT32 length;
	UINT32 dataSize;
	UINT8 *pData = NULL;
	DE_IPC_CMD_T ipcCmd = PQL_IPC_SET_TABLE;

	do{
		offset = header.offset;
		length = header.length;
		PE_ETC_CHECK_CODE(!length,break,"[%s,%d] length is zero.\n",__F__,__L__);
		PE_ETC_CHECK_CODE(!pTable,break,"[%s,%d] pTable is null.\n",__F__,__L__);
		PE_ETC_CHECK_CODE(!tblSize,break,"[%s,%d] tblSize is zero.\n",__F__,__L__);
		PE_ETC_CHECK_CODE(((offset+length)>tblSize),break,\
			"[%s,%d] offset(%d) and length(%d) is greater than tblSize(%d).\n",\
			__F__,__L__,offset,length,tblSize);

		PE_ETC_DBG_PRINT(" wId:%d,ofs:%d,len:%d, tbl_size:%d\n",\
			header.win_id,header.offset,header.length,tblSize);

		dataSize = sizeof(header)+length;
		pData = OS_KMalloc(dataSize);
		PE_ETC_CHECK_CODE(!pData,break,"[%s,%d] pData is null.\n",__F__,__L__);
		memcpy(pData,&header,sizeof(header));
		memcpy(&pData[sizeof(header)],&pTable[offset],length);
		ret = PE_FWI_SETDATA(ipcCmd, pData, dataSize);
		PE_ETC_CHECK_CODE(ret,break,"[%s,%d] PE_FWI_SETDATA() error.\n",__F__,__L__);
	}while(0);
	if(pData)	OS_Free(pData);

	return ret;
}

