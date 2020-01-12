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
 * date       2010.03.22
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <mach/platform.h>

#include "base_types.h"
#include "os_util.h"

#include "de_ver_def.h"

#include "de_def.h"
#include "de_cfg.h"
#include "l8/de_ipc_def_l8.h"
#include "l8/de_reg_def_l8.h"

#include "denc_kapi.h"
#include "l8/denc_regp_l8.h"
#include "l8/denc_reg_l8.h"   // 0x1900
#ifdef USE_KDRV_CODES_FOR_L8
#include "../afe/vport_reg.h"   // 0xc0007000
#include "../afe/vport_reg_B0.h"   // 0xc0007000
#endif

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define ATOP_WfCM(_r, _f, _c, _m)   (gpRegATOP->_r._f) = (_c)?((_m)|gpRegATOP->_r._f):(~(_m)&gpRegATOP->_r._f)

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/
extern int VIDEO_REG_WD(UINT32 addr, UINT32 value);
extern UINT32 VIDEO_REG_RD(UINT32 addr);
int DE_REG_HDMI_Init(void);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/
#ifdef USE_KDRV_CODES_FOR_L8
extern volatile VPORT_RegAx_T *gVportRegAx;
extern volatile VPORT_RegBx_T *gVportRegBx;
#endif
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
#ifdef USE_QEMU_SYSTEM
static volatile ATOP_REG_T *gpRealRegATOP = NULL;
#elif defined(USE_VIDEO_IOREMAP)
static ATOP_REG_T *gpRealRegATOP = NULL;
#else
static volatile ATOP_REG_T *gpRealRegATOP = (volatile ATOP_REG_T *)(VIDEO_ATOP_REG_BASE);
#endif
static ATOP_REG_T *gpRegATOP = NULL;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/

/**
 * @callgraph
 * @callergraph
 *
 * @brief Initialize DENC module.
 *
 * @return RET_OK(0)
 */
int DENC_REG_Create(void)
{
    int ret = RET_OK;

#ifdef USE_QEMU_SYSTEM
    gpRealRegATOP = (         ATOP_REG_T *)OS_KMalloc(sizeof(ATOP_REG_T));
#elif defined(USE_VIDEO_IOREMAP)
    gpRealRegATOP = (volatile ATOP_REG_T *)ioremap(VIDEO_ATOP_REG_BASE, sizeof(ATOP_REG_T));
#endif
    gpRegATOP     = (         ATOP_REG_T *)OS_KMalloc(sizeof(ATOP_REG_T));

    return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Destroy DENC module.
 *
 * @return RET_OK(0)
 */
int DENC_REG_Destory(void)
{
    int ret = RET_OK;

#ifdef USE_QEMU_SYSTEM
    OS_Free((void*)gpRealRegATOP);
#elif defined(USE_VIDEO_IOREMAP)
    iounmap((void*)gpRealRegATOP);
#endif
    OS_Free((void*)gpRegATOP);

    return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Initialize DENC module.
 *
 * @return RET_OK(0)
 */
int DENC_REG_Init(void)
{
#ifdef USE_KDRV_CODES_FOR_L8
    if ( lx_chip_rev() >= LX_CHIP_REV(L8,B0) )
    {
        REG_WD(&gVportRegBx->cvbsafe_010, 0x00633502);                        // CVBS PLL Setting Value
        REG_WD(&gVportRegBx->top_003, REG_RD(&gVportRegBx->top_003) | 0x10);  // CVBS PLL Power Down 0:Power Down, 1:Power On
        REG_WD(&gVportRegBx->top_007, REG_RD(&gVportRegBx->top_007) & ~0x1);  // CVE Soft Reset 0:Normal Operation, 1:Reset
        REG_WD(&gVportRegBx->cvbsafe_001 ,0x01311000);                        // VDAC On
    }
    else
    {
        REG_WD(&gVportRegAx->dr3p_m, 0x00633502);                                               // CVBS PLL Setting Value
        REG_WD(&gVportRegAx->reg_cvbspll_pdb,  REG_RD(&gVportRegAx->reg_cvbspll_pdb)  | 0x10);  // CVBS PLL Power Down 0:Power Down, 1:Power On
        REG_WD(&gVportRegAx->reg_swrst_chbcvd, REG_RD(&gVportRegAx->reg_swrst_chbcvd) & ~0x1);  // CVE Soft Reset 0:Normal Operation, 1:Reset
        REG_WD(&gVportRegAx->vdac_pdb ,0x01311000);                                             // VDAC On
    }
#endif

    return DENC_REG_NTSC_Init();
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief NTSC/PAL/SECAM encoder enable(TRUE : enable, FALSE : disable)
 *
 * @param OnOff
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_REG_OnOff ( BOOLEAN *pOnOff )
{
    int ret = RET_OK;
    //int combAddr;

    ATOP_RdFL(cve_config);
    ATOP_WfCM(cve_config, enc_en, *pOnOff, 0x1);
    ATOP_WrFL(cve_config);
    //combAddr = (*pOnOff)?(gpDeMem->frame_base + gpDeMem->frame_size):0x25fb0000;
    //REG_WD(&gVportReg->reg_gmau_cmd_base ,combAddr);

    return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Select the color standard of CVE.
 *
 * @param OnOff
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_REG_SetOutPutVideoStd(LX_DENC_VIDEO_SYSTEM_T *pstParams)
{
    int ret = RET_OK;

    ATOP_RdFL(cve_config);
    ATOP_Wr01(cve_config, standard , *pstParams);
    ATOP_WrFL(cve_config);

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
int DENC_REG_SetSource(LX_DENC_VIDEO_SOURCE_T *pstParams)
{
    int ret = RET_OK;

    return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Teletext insertion enable (TRUE : enable, FALSE : disable)
 *
 * @param OnOff
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_REG_TtxEnable(BOOLEAN *pstParams)
{
    int ret = RET_OK;

    ATOP_RdFL(denc_vbi_ctrl);
    ATOP_WfCM(denc_vbi_ctrl, ttx_en, *pstParams, 0x1);
    ATOP_WrFL(denc_vbi_ctrl);

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
int DENC_REG_WssEnable(BOOLEAN *pstParams)
{
    int ret = RET_OK;

    ATOP_RdFL(denc_vbi_ctrl);
    ATOP_WfCM(denc_vbi_ctrl, wss_en, *pstParams, 0x1);
    ATOP_WrFL(denc_vbi_ctrl);
    ATOP_RdFL(vps_ctrl);
    ATOP_WfCM(vps_ctrl, wss_odd,     *pstParams, 0x1);
    ATOP_WfCM(vps_ctrl, wss_even,    *pstParams, 0x1);
    ATOP_WrFL(vps_ctrl);

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
int DENC_REG_VpsEnable(BOOLEAN *pstParams)
{
    int ret = RET_OK;

    ATOP_RdFL(vps_ctrl);
    ATOP_WfCM(vps_ctrl, vps_enable, *pstParams, 0x1);
    ATOP_WfCM(vps_ctrl, vbi_odd,    *pstParams, 0x1);
    ATOP_Wr01(vps_ctrl, vbi_even,               0x0);
    ATOP_Wr01(vps_ctrl, lpf_bypass,             0x1);
    ATOP_WrFL(vps_ctrl);

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
int DENC_REG_SetTtxData(LX_DENC_TTX_DATA_T *pstParams)
{
    int ret = RET_OK;
    UINT32 *pTtxData = NULL;
    UINT32 ttxSize;
    int i;

    do {
        CHECK_KNULL(pstParams->pPacket_Addr);
        ttxSize =  pstParams->Size/sizeof(UINT32);
        if (!ttxSize) break;
        pTtxData = (UINT32 *)pstParams->pPacket_Addr;
        CHECK_KNULL(pTtxData);

        ATOP_RdFL(ttx_ver_ctrl1);
        ATOP_Wr01(ttx_ver_ctrl1, ttx_line, pstParams->Line_Number );
        ATOP_WrFL(ttx_ver_ctrl1);

        ATOP_RdFL(ttx_load);
        ATOP_Wr01(ttx_load, ttx_load, 1);
        for ( i=0; i<ttxSize;i++ ) {
            ATOP_Wr01(ttx_data, ttx_data, pTtxData[i]);
            ATOP_WrFL(ttx_data);
            ATOP_Wr01(ttx_load, ttx_write_addr, i);
            ATOP_WrFL(ttx_load);
        }
        ATOP_Wr01(ttx_load, ttx_load, 0);
        ATOP_WrFL(ttx_load);
    } while (0);

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
int DENC_REG_SetWssData(LX_DENC_WSS_DATA_T *pstParams)
{
    int ret = RET_OK;

    ATOP_RdFL(denc_vbi_ctrl);
    ATOP_WfCM(denc_vbi_ctrl, wss_en, pstParams->Enable, 0x1);
    ATOP_WrFL(denc_vbi_ctrl);
    ATOP_RdFL(vps_ctrl);
    ATOP_WfCM(vps_ctrl, wss_odd,     pstParams->Enable, 0x1);
    ATOP_WfCM(vps_ctrl, wss_even,    pstParams->Enable, 0x1);
    ATOP_WrFL(vps_ctrl);

    ATOP_RdFL(wss_data);
    ATOP_Wr01(wss_data, wss_data, pstParams->WSS_DATA);
    ATOP_WrFL(wss_data);

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
int DENC_REG_SetVpsData(LX_DENC_VPS_DATA_T *pstParams)
{
    int ret = RET_OK;
    UINT16 vpsData;
    int i;

    ATOP_RdFL(vps_ctrl);
    ATOP_WfCM(vps_ctrl, vps_enable, pstParams->Enable, 0x1);
    ATOP_WfCM(vps_ctrl, vbi_odd,    pstParams->Enable, 0x1);
    ATOP_Wr01(vps_ctrl, vbi_even,                      0x0);
    ATOP_Wr01(vps_ctrl, lpf_bypass,                    0x1);
    ATOP_Wr01(vps_ctrl, vps_wr_done,                   0x0);
    vpsData = 0;
    for (i=0;i<15;i++) {
        vpsData <<= 8;
        vpsData  |= pstParams->pVPS_DATA[i];
        if ( !(i%2) ) continue;
        ATOP_Wr01(vps_ctrl, vps_data,    vpsData);
        ATOP_WrFL(vps_ctrl);
        vpsData = 0;
    }
    ATOP_Wr01(vps_ctrl, vps_wr_done, 0x1);
    ATOP_Wr01(vps_ctrl, vps_data,    vpsData);
    ATOP_WrFL(vps_ctrl);

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
int DENC_REG_ColorBarEnable(BOOLEAN *pstParams)
{
    int ret = RET_OK;

    ATOP_RdFL(cve_config);
    ATOP_WfCM(cve_config, color_bar_en, *pstParams, 0x1);
    ATOP_WrFL(cve_config);

    return ret;
}

int DENC_REG_NTSC_Init(void)
{
    ATOP_FLWr(ttx_ver_ctrl0, 0x17061606);
    ATOP_FLWr(ttx_ver_ctrl1, 0x0003c4c0);
    ATOP_FLWr(wss_data,      0x6a5a9919);
    ATOP_FLWr(cgms_data,     0x0B800000);
    ATOP_FLWr(cve_config,    0x0B381389);
    ATOP_FLWr(phase_incr,    0x2a098acb);
    ATOP_FLWr(denc_vbi_ctrl, 0x4ee00800);
    ATOP_FLWr(cc_data,       0x80808080);

    ATOP_FLWr(mv_n0_3,       0x21171500);
    ATOP_FLWr(mv_n4_9,       0x1BB61B15);
    ATOP_FLWr(mv_n10_11,     0x07F80024);
    ATOP_FLWr(mv_n12_14,     0x0F0F0000);
    ATOP_FLWr(mv_n15_20,     0xA05C0060);

	ATOP_FLWr(vps_ctrl, 0x123400b0);
	ATOP_FLWr(vps_ctrl, 0x567800b1);
	ATOP_FLWr(vps_ctrl, 0x9abc00b2);
	ATOP_FLWr(vps_ctrl, 0xdef100b3);
	ATOP_FLWr(vps_ctrl, 0x234500b4);
	ATOP_FLWr(vps_ctrl, 0x678900b5);
	ATOP_FLWr(vps_ctrl, 0xabcd01b6);

	//ATOP_FLWr(ttx_data, 0xAAAAE400); ATOP_FLWr(ttx_load, 0x00000001);
	//ATOP_FLWr(ttx_data, 0x12345678); ATOP_FLWr(ttx_load, 0x00000101);
	//ATOP_FLWr(ttx_data, 0x9ABCDEF1); ATOP_FLWr(ttx_load, 0x00000201);
	//ATOP_FLWr(ttx_data, 0x23456789); ATOP_FLWr(ttx_load, 0x00000301);
	//ATOP_FLWr(ttx_data, 0xABCDEF12); ATOP_FLWr(ttx_load, 0x00000401);
	//ATOP_FLWr(ttx_data, 0x3456789A); ATOP_FLWr(ttx_load, 0x00000501);
	//ATOP_FLWr(ttx_data, 0xBCDEF123); ATOP_FLWr(ttx_load, 0x00000601);
	//ATOP_FLWr(ttx_data, 0x456789AB); ATOP_FLWr(ttx_load, 0x00000701);
	//ATOP_FLWr(ttx_data, 0xCDEF1234); ATOP_FLWr(ttx_load, 0x00000801);
	//ATOP_FLWr(ttx_data, 0x56789ABC); ATOP_FLWr(ttx_load, 0x00000901);
	//ATOP_FLWr(ttx_data, 0xDEF12345); ATOP_FLWr(ttx_load, 0x00000A01);
	//ATOP_FLWr(ttx_data, 0x6789ABCD); ATOP_FLWr(ttx_load, 0x00000B01);

    return 0;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Power On/Off VDAC and Buffer Power on DENC Block (0:off, 1:on)
 *
 * @param on_off [IN] on_off (UINT32)
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DENC_REG_VdacPowerControl(BOOLEAN *pstParam)
{
    int ret = RET_OK;
#ifdef USE_KDRV_CODES_FOR_L8
    BOOLEAN on_off;

    on_off = *pstParam;
    if (on_off)  //Power On
    {
        REG_WD(&gVportRegBx->cvbsafe_001, 0x01311000);  // VDAC On
        /*
         VPORT_RdFL(cvbsafe_001);
         VPORT_Wr01(cvbsafe_001, vdac_pdb, 1);
         VPORT_Wr01(cvbsafe_001, buf_pdb1, 1);
         VPORT_Wr01(cvbsafe_001, buf_pdb2, 1);
         VPORT_Wr01(cvbsafe_001, buf_sel1, 1);
         VPORT_Wr01(cvbsafe_001, buf_sel2, 1);
         VPORT_WrFL(cvbsafe_001);
         */
    }
    else
    {
        REG_WD(&gVportRegBx->cvbsafe_001, 0x00300000);  // VDAC Off
        /*
         VPORT_RdFL(cvbsafe_001);
         VPORT_Wr01(cvbsafe_001, vdac_pdb, 0);
         VPORT_Wr01(cvbsafe_001, buf_pdb1, 0);
         VPORT_Wr01(cvbsafe_001, buf_pdb2, 0);
         VPORT_Wr01(cvbsafe_001, buf_sel1, 0);
         VPORT_Wr01(cvbsafe_001, buf_sel2, 0);
         VPORT_WrFL(cvbsafe_001);
         */
    }
#endif

    return ret;
}

/**  @} */
