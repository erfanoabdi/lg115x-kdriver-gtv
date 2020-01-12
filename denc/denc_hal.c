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


/** @file
 *
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     jaemo.kim (jaemo.kim@lge.com)
 * version    1.0
 * date       2010.03.17
 * note       Additional information.
 *
 * @addtogroup lg1150_denc
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include <mach/platform.h>

#include "base_types.h"
#include "os_util.h"

#include "../de/de_model.h"
#include "../de/de_def.h"
#include "../de/de_ver_def.h"
#include "denc_kapi.h"

#include "l8/denc_regp_l8.h"
#ifdef USE_KDRV_CODES_FOR_L9
#include "l9/denc_reg_l9.h"
#endif

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
/**
 * @callgraph
 * @callergraph
 *
 * @brief Initialize DENC module.
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_Init(void)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_Init();
	} else
#endif
	{
		ret = DENC_REG_Init();
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief NTSC/PAL/SECAM encoder enable(TRUE : enable, FALSE : disable)
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_OnOff(BOOLEAN *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_OnOff(pstParams);
	} else
#endif
	{
		ret = DENC_REG_OnOff(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Select the color standard of CVE.
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_SetOutPutVideoStd(LX_DENC_VIDEO_SYSTEM_T *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_SetOutPutVideoStd(pstParams);
	} else
#endif
	{
		ret = DENC_REG_SetOutPutVideoStd(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Select the input source of CVE.
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0)
 */
int DENC_HAL_SetSource(LX_DENC_VIDEO_SOURCE_T *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_SetSource(pstParams);
	} else
#endif
	{
		ret = DENC_REG_SetSource(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Teletext insertion enable (TRUE : enable, FALSE : disable)
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_TtxEnable(BOOLEAN *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_TtxEnable(pstParams);
	} else
#endif
	{
		ret = DENC_REG_TtxEnable(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief WSS insertion enable (TRUE : enable, FALSE : disable)
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_WssEnable(BOOLEAN *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_WssEnable(pstParams);
	} else
#endif
	{
		ret = DENC_REG_WssEnable(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief VPS insertion enable (TRUE : enable, FALSE : disable)
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_VpsEnable(BOOLEAN *pstParams)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_VpsEnable(pstParams);
	} else
#endif
	{
		ret = DENC_REG_VpsEnable(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Inserts Teletext data to VBI lines.
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_SetTtxData(LX_DENC_TTX_DATA_T *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_SetTtxData(pstParams);
	} else
#endif
	{
		ret = DENC_REG_SetTtxData(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Inserts WSS data to VBI line.
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_SetWssData(LX_DENC_WSS_DATA_T *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_SetWssData(pstParams);
	} else
#endif
	{
		ret = DENC_REG_SetWssData(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Inserts VPS data to VBI lines.
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_SetVpsData(LX_DENC_VPS_DATA_T *pstParams)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_SetVpsData(pstParams);
	} else
#endif
	{
		ret = DENC_REG_SetVpsData(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Enable/disable internal colorbar pattern (TRUE : enable, FALSE : disable)
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_ColorBarEnable(BOOLEAN *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_ColorBarEnable(pstParams);
	} else
#endif
	{
		ret = DENC_REG_ColorBarEnable(pstParams);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Power On/Off VDAC and Buffer Power on DENC Block (0:off, 1:on)
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_HAL_VdacPowerControl(BOOLEAN *pstParams)
{
	int ret;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_VdacPowerControl(pstParams);
	} else
#endif
	{
		ret = DENC_REG_VdacPowerControl(pstParams);
	}

	return ret;
}

int DENC_HAL_Create(void)
{
    int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_Create();
	} else
#endif
	{
		ret = DENC_REG_Create();
	}

    return ret;
}

int DENC_HAL_Destory(void)
{
    int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DENC_REG_L9_Destory();
	} else
#endif
	{
		ret = DENC_REG_Destory();
	}

    return ret;
}
/**  @} */
