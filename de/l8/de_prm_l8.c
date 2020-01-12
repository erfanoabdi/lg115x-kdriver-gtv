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
 * date       2010.04.15
 * note       Additional information.
 *
 * @addtogroup lg1150_de
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
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include "os_util.h"

#include "base_types.h"

#include "de_def.h"
#include "de_drv.h"
#include "de_prm_def.h"

#include "l8/de_cvi_reg_l8.h"
#include "l8/de_msc_reg_l8.h"
#include "l8/de_ssc_reg_l8.h"
#include "l8/de_top_reg_l8.h"
#ifdef USE_KDRV_CODES_FOR_L8
#include "fmt3d_reg.h"
#include "pe1_reg.h"
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
extern CHA_CVI_REG_T	 *gpRealRegCHA_CVI;
extern CHB_CVI_REG_T	 *gpRealRegCHB_CVI;
extern MSC_REG_T		 *gpRealRegMSC;
extern SSC_REG_T		 *gpRealRegSSC;
extern DE0B_CTRL_REG_T   *gpRealRegDE0B_CTRL;
#ifdef USE_KDRV_CODES_FOR_L8
extern PRE3D_REG_T		 *gpRealRegPRE3D;
extern PE1_REG_T		 *gpRealRegPE1;
extern PE1_REG_T		 *gpRegPE1;
#endif

extern CHA_CVI_REG_T	 *gpRegCHA_CVI;
extern CHB_CVI_REG_T	 *gpRegCHB_CVI;

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/
static int DE_PRM_LoadCvmFirCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadCvsFirCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadCvmCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadCvsCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadMscrFirYCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadMscrFirCCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadSscrFirYCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadMscrPhaseOffset(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadSscrPhaseOffset(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadCvmHdnCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadMdmHdnCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadDlrCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadDlrPhase(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadMdmCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);
static int DE_PRM_LoadPostCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size);

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
static DE_TABLE_PARAM_T saVideoTable[] = {
    { DE_CVM_FIR_COEF      ,DE_PRM_LoadCvmFirCoef      },
    { DE_CVS_FIR_COEF      ,DE_PRM_LoadCvsFirCoef      },
    { DE_CVM_CSC_COEF      ,DE_PRM_LoadCvmCscCoef      },
    { DE_CVS_CSC_COEF      ,DE_PRM_LoadCvsCscCoef      },
    { DE_MSCR_FIRY_COEF    ,DE_PRM_LoadMscrFirYCoef    },
    { DE_MSCR_FIRC_COEF    ,DE_PRM_LoadMscrFirCCoef    },
    { DE_SSCR_FIRY_COEF    ,DE_PRM_LoadSscrFirYCoef    },
    { DE_MSCR_PHASE_OFFSET ,DE_PRM_LoadMscrPhaseOffset },
    { DE_SSCR_PHASE_OFFSET ,DE_PRM_LoadSscrPhaseOffset },
    { DE_CVM_HDN_COEF      ,DE_PRM_LoadCvmHdnCoef      },
    { DE_MDM_HDN_COEF      ,DE_PRM_LoadMdmHdnCoef      },
    { DE_DLR_QX_COEF       ,DE_PRM_LoadDlrCoef         },
    { DE_DLR_PHASE_COEF    ,DE_PRM_LoadDlrPhase        },
    { DE_MDM_CSC_COEF      ,DE_PRM_LoadMdmCscCoef      },
    { DE_POST_CSC_COEF     ,DE_PRM_LoadPostCscCoef     },
};

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int DE_PRM_LoadTable(DE_PARAM_TYPE_T tableId, DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 writeSize)
{
    int ret = RET_ERROR;

    do {
        if (tableId >= ARRAY_SIZE(saVideoTable)) {
            ffprintk("Wrong tableId(%d)\n", tableId);
            break;
        }
        ret = saVideoTable[tableId].func(bWrite, index, pTable, writeSize);
    } while (0);

    return ret;
}

#if 0
static int DE_PRM_LoadCvmFirCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
	CHA_DIG_FILT_CTRL0 cha_dig_filt_ctrl0 = {0};
	CHA_DIG_FILT_CTRL1 cha_dig_filt_ctrl1 = {0};
	CHA_DIG_FILT_CTRL2 cha_dig_filt_ctrl2 = {0};
	CHA_DIG_FILT_CTRL3 cha_dig_filt_ctrl3 = {0};

    if (bWrite) {
        cha_dig_filt_ctrl3 = gpRealRegCHA_CVI->cha_dig_filt_ctrl3;

        cha_dig_filt_ctrl0.fir_coefficient0 = pTable[0];
        cha_dig_filt_ctrl0.fir_coefficient1 = pTable[1];
        cha_dig_filt_ctrl1.fir_coefficient2 = pTable[2];
        cha_dig_filt_ctrl1.fir_coefficient3 = pTable[3];
        cha_dig_filt_ctrl2.fir_coefficient4 = pTable[4];
        cha_dig_filt_ctrl2.fir_coefficient5 = pTable[5];
        cha_dig_filt_ctrl3.fir_coefficient6 = pTable[6];
        cha_dig_filt_ctrl3.fir_coefficient7 = pTable[7];
        cha_dig_filt_ctrl3.fir_coeff_load   = TRUE;

        gpRealRegCHA_CVI->cha_dig_filt_ctrl0 = cha_dig_filt_ctrl0;
        gpRealRegCHA_CVI->cha_dig_filt_ctrl1 = cha_dig_filt_ctrl1;
        gpRealRegCHA_CVI->cha_dig_filt_ctrl2 = cha_dig_filt_ctrl2;
        gpRealRegCHA_CVI->cha_dig_filt_ctrl3 = cha_dig_filt_ctrl3;
    } else {
        cha_dig_filt_ctrl0 = gpRealRegCHA_CVI->cha_dig_filt_ctrl0;
        cha_dig_filt_ctrl1 = gpRealRegCHA_CVI->cha_dig_filt_ctrl1;
        cha_dig_filt_ctrl2 = gpRealRegCHA_CVI->cha_dig_filt_ctrl2;
        cha_dig_filt_ctrl3 = gpRealRegCHA_CVI->cha_dig_filt_ctrl3;

        pTable[0] = cha_dig_filt_ctrl0.fir_coefficient0;
        pTable[1] = cha_dig_filt_ctrl0.fir_coefficient1;
        pTable[2] = cha_dig_filt_ctrl1.fir_coefficient2;
        pTable[3] = cha_dig_filt_ctrl1.fir_coefficient3;
        pTable[4] = cha_dig_filt_ctrl2.fir_coefficient4;
        pTable[5] = cha_dig_filt_ctrl2.fir_coefficient5;
        pTable[6] = cha_dig_filt_ctrl3.fir_coefficient6;
        pTable[7] = cha_dig_filt_ctrl3.fir_coefficient7;
    }

    return RET_OK;
}

static int DE_PRM_LoadCvsFirCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
	CHB_DIG_FILT_CTRL0 chb_dig_filt_ctrl0 = {0};
	CHB_DIG_FILT_CTRL1 chb_dig_filt_ctrl1 = {0};
	CHB_DIG_FILT_CTRL2 chb_dig_filt_ctrl2 = {0};
	CHB_DIG_FILT_CTRL3 chb_dig_filt_ctrl3 = {0};

    if (bWrite) {
        chb_dig_filt_ctrl3 = gpRealRegCHB_CVI->chb_dig_filt_ctrl3;

        chb_dig_filt_ctrl0.fir_coefficient0 = pTable[0];
        chb_dig_filt_ctrl0.fir_coefficient1 = pTable[1];
        chb_dig_filt_ctrl1.fir_coefficient2 = pTable[2];
        chb_dig_filt_ctrl1.fir_coefficient3 = pTable[3];
        chb_dig_filt_ctrl2.fir_coefficient4 = pTable[4];
        chb_dig_filt_ctrl2.fir_coefficient5 = pTable[5];
        chb_dig_filt_ctrl3.fir_coefficient6 = pTable[6];
        chb_dig_filt_ctrl3.fir_coefficient7 = pTable[7];
        chb_dig_filt_ctrl3.fir_coeff_load   = TRUE;

        gpRealRegCHB_CVI->chb_dig_filt_ctrl0 = chb_dig_filt_ctrl0;
        gpRealRegCHB_CVI->chb_dig_filt_ctrl1 = chb_dig_filt_ctrl1;
        gpRealRegCHB_CVI->chb_dig_filt_ctrl2 = chb_dig_filt_ctrl2;
        gpRealRegCHB_CVI->chb_dig_filt_ctrl3 = chb_dig_filt_ctrl3;
    } else {
        chb_dig_filt_ctrl0 = gpRealRegCHB_CVI->chb_dig_filt_ctrl0;
        chb_dig_filt_ctrl1 = gpRealRegCHB_CVI->chb_dig_filt_ctrl1;
        chb_dig_filt_ctrl2 = gpRealRegCHB_CVI->chb_dig_filt_ctrl2;
        chb_dig_filt_ctrl3 = gpRealRegCHB_CVI->chb_dig_filt_ctrl3;

        pTable[0] = chb_dig_filt_ctrl0.fir_coefficient0;
        pTable[1] = chb_dig_filt_ctrl0.fir_coefficient1;
        pTable[2] = chb_dig_filt_ctrl1.fir_coefficient2;
        pTable[3] = chb_dig_filt_ctrl1.fir_coefficient3;
        pTable[4] = chb_dig_filt_ctrl2.fir_coefficient4;
        pTable[5] = chb_dig_filt_ctrl2.fir_coefficient5;
        pTable[6] = chb_dig_filt_ctrl3.fir_coefficient6;
        pTable[7] = chb_dig_filt_ctrl3.fir_coefficient7;
    }

    return RET_OK;
}
#endif

static int DE_PRM_LoadCvmFirCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    if (bWrite) {
        CHA_CVI_RdFL(cha_dig_filt_ctrl3);

        CHA_CVI_Wr01(cha_dig_filt_ctrl0 ,fir_coefficient0 ,pTable[0]);
        CHA_CVI_Wr01(cha_dig_filt_ctrl0 ,fir_coefficient1 ,pTable[1]);
        CHA_CVI_Wr01(cha_dig_filt_ctrl1 ,fir_coefficient2 ,pTable[2]);
        CHA_CVI_Wr01(cha_dig_filt_ctrl1 ,fir_coefficient3 ,pTable[3]);
        CHA_CVI_Wr01(cha_dig_filt_ctrl2 ,fir_coefficient4 ,pTable[4]);
        CHA_CVI_Wr01(cha_dig_filt_ctrl2 ,fir_coefficient5 ,pTable[5]);
        CHA_CVI_Wr01(cha_dig_filt_ctrl3 ,fir_coefficient6 ,pTable[6]);
        CHA_CVI_Wr01(cha_dig_filt_ctrl3 ,fir_coefficient7 ,pTable[7]);
 		CHA_CVI_Wr01(cha_dig_filt_ctrl3 ,fir_coeff_load   ,TRUE);
 		CHA_CVI_Wr01(cha_dig_filt_ctrl3 ,fir_en           ,(index)?TRUE:FALSE);

 		CHA_CVI_WrFL(cha_dig_filt_ctrl0);
 		CHA_CVI_WrFL(cha_dig_filt_ctrl1);
 		CHA_CVI_WrFL(cha_dig_filt_ctrl2);
 		CHA_CVI_WrFL(cha_dig_filt_ctrl3);
    } else {
		CHA_CVI_RdFL(cha_dig_filt_ctrl0);
        CHA_CVI_RdFL(cha_dig_filt_ctrl1);
        CHA_CVI_RdFL(cha_dig_filt_ctrl2);
        CHA_CVI_RdFL(cha_dig_filt_ctrl3);

        CHA_CVI_Rd01(cha_dig_filt_ctrl0 ,fir_coefficient0 ,pTable[0]);
        CHA_CVI_Rd01(cha_dig_filt_ctrl0 ,fir_coefficient1 ,pTable[1]);
        CHA_CVI_Rd01(cha_dig_filt_ctrl1 ,fir_coefficient2 ,pTable[2]);
        CHA_CVI_Rd01(cha_dig_filt_ctrl1 ,fir_coefficient3 ,pTable[3]);
        CHA_CVI_Rd01(cha_dig_filt_ctrl2 ,fir_coefficient4 ,pTable[4]);
        CHA_CVI_Rd01(cha_dig_filt_ctrl2 ,fir_coefficient5 ,pTable[5]);
        CHA_CVI_Rd01(cha_dig_filt_ctrl3 ,fir_coefficient6 ,pTable[6]);
        CHA_CVI_Rd01(cha_dig_filt_ctrl3 ,fir_coefficient7 ,pTable[7]);
    }

    return RET_OK;
}

static int DE_PRM_LoadCvsFirCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    if (bWrite) {
        CHB_CVI_RdFL(chb_dig_filt_ctrl3);

        CHB_CVI_Wr01(chb_dig_filt_ctrl0 ,fir_coefficient0 ,pTable[0]);
        CHB_CVI_Wr01(chb_dig_filt_ctrl0 ,fir_coefficient1 ,pTable[1]);
        CHB_CVI_Wr01(chb_dig_filt_ctrl1 ,fir_coefficient2 ,pTable[2]);
        CHB_CVI_Wr01(chb_dig_filt_ctrl1 ,fir_coefficient3 ,pTable[3]);
        CHB_CVI_Wr01(chb_dig_filt_ctrl2 ,fir_coefficient4 ,pTable[4]);
        CHB_CVI_Wr01(chb_dig_filt_ctrl2 ,fir_coefficient5 ,pTable[5]);
        CHB_CVI_Wr01(chb_dig_filt_ctrl3 ,fir_coefficient6 ,pTable[6]);
        CHB_CVI_Wr01(chb_dig_filt_ctrl3 ,fir_coefficient7 ,pTable[7]);
 		CHB_CVI_Wr01(chb_dig_filt_ctrl3 ,fir_coeff_load   ,TRUE);
 		CHB_CVI_Wr01(chb_dig_filt_ctrl3 ,fir_en           ,(index)?TRUE:FALSE);

 		CHB_CVI_WrFL(chb_dig_filt_ctrl0);
 		CHB_CVI_WrFL(chb_dig_filt_ctrl1);
 		CHB_CVI_WrFL(chb_dig_filt_ctrl2);
 		CHB_CVI_WrFL(chb_dig_filt_ctrl3);
     } else {
		CHB_CVI_RdFL(chb_dig_filt_ctrl0);
        CHB_CVI_RdFL(chb_dig_filt_ctrl1);
        CHB_CVI_RdFL(chb_dig_filt_ctrl2);
        CHB_CVI_RdFL(chb_dig_filt_ctrl3);

        CHB_CVI_Rd01(chb_dig_filt_ctrl0 ,fir_coefficient0 ,pTable[0]);
        CHB_CVI_Rd01(chb_dig_filt_ctrl0 ,fir_coefficient1 ,pTable[1]);
        CHB_CVI_Rd01(chb_dig_filt_ctrl1 ,fir_coefficient2 ,pTable[2]);
        CHB_CVI_Rd01(chb_dig_filt_ctrl1 ,fir_coefficient3 ,pTable[3]);
        CHB_CVI_Rd01(chb_dig_filt_ctrl2 ,fir_coefficient4 ,pTable[4]);
        CHB_CVI_Rd01(chb_dig_filt_ctrl2 ,fir_coefficient5 ,pTable[5]);
        CHB_CVI_Rd01(chb_dig_filt_ctrl3 ,fir_coefficient6 ,pTable[6]);
        CHB_CVI_Rd01(chb_dig_filt_ctrl3 ,fir_coefficient7 ,pTable[7]);
    }

    return RET_OK;
}

static int DE_PRM_LoadCvmCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    if (bWrite) {
        CHA_CVI_RdFL(cha_csc_ctrl7);

        CHA_CVI_Wr01(cha_csc_ctrl0 ,csc_coefficient0 ,pTable[0]);
        CHA_CVI_Wr01(cha_csc_ctrl0 ,csc_coefficient1 ,pTable[1]);
        CHA_CVI_Wr01(cha_csc_ctrl1 ,csc_coefficient2 ,pTable[2]);
        CHA_CVI_Wr01(cha_csc_ctrl1 ,csc_coefficient3 ,pTable[3]);
        CHA_CVI_Wr01(cha_csc_ctrl2 ,csc_coefficient4 ,pTable[4]);
        CHA_CVI_Wr01(cha_csc_ctrl2 ,csc_coefficient5 ,pTable[5]);
        CHA_CVI_Wr01(cha_csc_ctrl3 ,csc_coefficient6 ,pTable[6]);
        CHA_CVI_Wr01(cha_csc_ctrl3 ,csc_coefficient7 ,pTable[7]);
        CHA_CVI_Wr01(cha_csc_ctrl4 ,csc_coefficient8 ,pTable[8]);
        CHA_CVI_Wr01(cha_csc_ctrl4 ,csc_offset0      ,pTable[9]);
        CHA_CVI_Wr01(cha_csc_ctrl5 ,csc_offset1      ,pTable[10]);
        CHA_CVI_Wr01(cha_csc_ctrl5 ,csc_offset2      ,pTable[11]);
        CHA_CVI_Wr01(cha_csc_ctrl6 ,csc_offset3      ,pTable[12]);
        CHA_CVI_Wr01(cha_csc_ctrl6 ,csc_offset4      ,pTable[13]);
        CHA_CVI_Wr01(cha_csc_ctrl7 ,csc_offset5      ,pTable[14]);
        CHA_CVI_Wr01(cha_csc_ctrl7 ,csc_coeff_load   ,TRUE);
        CHA_CVI_Wr01(cha_csc_ctrl7 ,csc_en           ,(index)?TRUE:FALSE);

        CHA_CVI_WrFL(cha_csc_ctrl0);
        CHA_CVI_WrFL(cha_csc_ctrl1);
        CHA_CVI_WrFL(cha_csc_ctrl2);
        CHA_CVI_WrFL(cha_csc_ctrl3);
        CHA_CVI_WrFL(cha_csc_ctrl4);
        CHA_CVI_WrFL(cha_csc_ctrl5);
        CHA_CVI_WrFL(cha_csc_ctrl6);
        CHA_CVI_WrFL(cha_csc_ctrl7);
    } else {
        CHA_CVI_RdFL(cha_csc_ctrl0);
        CHA_CVI_RdFL(cha_csc_ctrl1);
        CHA_CVI_RdFL(cha_csc_ctrl2);
        CHA_CVI_RdFL(cha_csc_ctrl3);
        CHA_CVI_RdFL(cha_csc_ctrl4);
        CHA_CVI_RdFL(cha_csc_ctrl5);
        CHA_CVI_RdFL(cha_csc_ctrl6);
        CHA_CVI_RdFL(cha_csc_ctrl7);

        CHA_CVI_Rd01(cha_csc_ctrl0 ,csc_coefficient0 ,pTable[0]);
        CHA_CVI_Rd01(cha_csc_ctrl0 ,csc_coefficient1 ,pTable[1]);
        CHA_CVI_Rd01(cha_csc_ctrl1 ,csc_coefficient2 ,pTable[2]);
        CHA_CVI_Rd01(cha_csc_ctrl1 ,csc_coefficient3 ,pTable[3]);
        CHA_CVI_Rd01(cha_csc_ctrl2 ,csc_coefficient4 ,pTable[4]);
        CHA_CVI_Rd01(cha_csc_ctrl2 ,csc_coefficient5 ,pTable[5]);
        CHA_CVI_Rd01(cha_csc_ctrl3 ,csc_coefficient6 ,pTable[6]);
        CHA_CVI_Rd01(cha_csc_ctrl3 ,csc_coefficient7 ,pTable[7]);
        CHA_CVI_Rd01(cha_csc_ctrl4 ,csc_coefficient8 ,pTable[8]);
        CHA_CVI_Rd01(cha_csc_ctrl4 ,csc_offset0      ,pTable[9]);
        CHA_CVI_Rd01(cha_csc_ctrl5 ,csc_offset1      ,pTable[10]);
        CHA_CVI_Rd01(cha_csc_ctrl5 ,csc_offset2      ,pTable[11]);
        CHA_CVI_Rd01(cha_csc_ctrl6 ,csc_offset3      ,pTable[12]);
        CHA_CVI_Rd01(cha_csc_ctrl6 ,csc_offset4      ,pTable[13]);
        CHA_CVI_Rd01(cha_csc_ctrl7 ,csc_offset5      ,pTable[14]);
    }

    return RET_OK;
}

static int DE_PRM_LoadCvsCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    if (bWrite) {
        CHB_CVI_RdFL(chb_csc_ctrl7);

        CHB_CVI_Wr01(chb_csc_ctrl0 ,csc_coefficient0 ,pTable[0]);
        CHB_CVI_Wr01(chb_csc_ctrl0 ,csc_coefficient1 ,pTable[1]);
        CHB_CVI_Wr01(chb_csc_ctrl1 ,csc_coefficient2 ,pTable[2]);
        CHB_CVI_Wr01(chb_csc_ctrl1 ,csc_coefficient3 ,pTable[3]);
        CHB_CVI_Wr01(chb_csc_ctrl2 ,csc_coefficient4 ,pTable[4]);
        CHB_CVI_Wr01(chb_csc_ctrl2 ,csc_coefficient5 ,pTable[5]);
        CHB_CVI_Wr01(chb_csc_ctrl3 ,csc_coefficient6 ,pTable[6]);
        CHB_CVI_Wr01(chb_csc_ctrl3 ,csc_coefficient7 ,pTable[7]);
        CHB_CVI_Wr01(chb_csc_ctrl4 ,csc_coefficient8 ,pTable[8]);
        CHB_CVI_Wr01(chb_csc_ctrl4 ,csc_offset0      ,pTable[9]);
        CHB_CVI_Wr01(chb_csc_ctrl5 ,csc_offset1      ,pTable[10]);
        CHB_CVI_Wr01(chb_csc_ctrl5 ,csc_offset2      ,pTable[11]);
        CHB_CVI_Wr01(chb_csc_ctrl6 ,csc_offset3      ,pTable[12]);
        CHB_CVI_Wr01(chb_csc_ctrl6 ,csc_offset4      ,pTable[13]);
        CHB_CVI_Wr01(chb_csc_ctrl7 ,csc_offset5      ,pTable[14]);
        CHB_CVI_Wr01(chb_csc_ctrl7 ,csc_coeff_load   ,TRUE);
        CHB_CVI_Wr01(chb_csc_ctrl7 ,csc_en           ,(index)?TRUE:FALSE);

        CHB_CVI_WrFL(chb_csc_ctrl0);
        CHB_CVI_WrFL(chb_csc_ctrl1);
        CHB_CVI_WrFL(chb_csc_ctrl2);
        CHB_CVI_WrFL(chb_csc_ctrl3);
        CHB_CVI_WrFL(chb_csc_ctrl4);
        CHB_CVI_WrFL(chb_csc_ctrl5);
        CHB_CVI_WrFL(chb_csc_ctrl6);
        CHB_CVI_WrFL(chb_csc_ctrl7);
    } else {
        CHB_CVI_RdFL(chb_csc_ctrl0);
        CHB_CVI_RdFL(chb_csc_ctrl1);
        CHB_CVI_RdFL(chb_csc_ctrl2);
        CHB_CVI_RdFL(chb_csc_ctrl3);
        CHB_CVI_RdFL(chb_csc_ctrl4);
        CHB_CVI_RdFL(chb_csc_ctrl5);
        CHB_CVI_RdFL(chb_csc_ctrl6);
        CHB_CVI_RdFL(chb_csc_ctrl7);

        CHB_CVI_Rd01(chb_csc_ctrl0 ,csc_coefficient0 ,pTable[0]);
        CHB_CVI_Rd01(chb_csc_ctrl0 ,csc_coefficient1 ,pTable[1]);
        CHB_CVI_Rd01(chb_csc_ctrl1 ,csc_coefficient2 ,pTable[2]);
        CHB_CVI_Rd01(chb_csc_ctrl1 ,csc_coefficient3 ,pTable[3]);
        CHB_CVI_Rd01(chb_csc_ctrl2 ,csc_coefficient4 ,pTable[4]);
        CHB_CVI_Rd01(chb_csc_ctrl2 ,csc_coefficient5 ,pTable[5]);
        CHB_CVI_Rd01(chb_csc_ctrl3 ,csc_coefficient6 ,pTable[6]);
        CHB_CVI_Rd01(chb_csc_ctrl3 ,csc_coefficient7 ,pTable[7]);
        CHB_CVI_Rd01(chb_csc_ctrl4 ,csc_coefficient8 ,pTable[8]);
        CHB_CVI_Rd01(chb_csc_ctrl4 ,csc_offset0      ,pTable[9]);
        CHB_CVI_Rd01(chb_csc_ctrl5 ,csc_offset1      ,pTable[10]);
        CHB_CVI_Rd01(chb_csc_ctrl5 ,csc_offset2      ,pTable[11]);
        CHB_CVI_Rd01(chb_csc_ctrl6 ,csc_offset3      ,pTable[12]);
        CHB_CVI_Rd01(chb_csc_ctrl6 ,csc_offset4      ,pTable[13]);
        CHB_CVI_Rd01(chb_csc_ctrl7 ,csc_offset5      ,pTable[14]);
    }

    return RET_OK;
}

static int DE_PRM_LoadMscrFirYCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    MSC_Y_FIR_COEF_DATA0 msc_y_fir_coef_data0;
    MSC_Y_FIR_COEF_DATA1 msc_y_fir_coef_data1;
    MSC_Y_FIR_COEF_DATA2 msc_y_fir_coef_data2;
    MSC_Y_FIR_COEF_DATA3 msc_y_fir_coef_data3;
    MSC_Y_FIR_COEF_DATA4 msc_y_fir_coef_data4;
    MSC_Y_FIR_COEF_DATA5 msc_y_fir_coef_data5;
    MSC_Y_FIR_COEF_CTRL  msc_y_fir_coef_ctrl;

    if (bWrite) {
        msc_y_fir_coef_data0 = *(MSC_Y_FIR_COEF_DATA0 *)&pTable[0];
        msc_y_fir_coef_data1 = *(MSC_Y_FIR_COEF_DATA1 *)&pTable[1];
        msc_y_fir_coef_data2 = *(MSC_Y_FIR_COEF_DATA2 *)&pTable[2];
        msc_y_fir_coef_data3 = *(MSC_Y_FIR_COEF_DATA3 *)&pTable[3];
        gpRealRegMSC->msc_y_fir_coef_data0 = msc_y_fir_coef_data0;
        gpRealRegMSC->msc_y_fir_coef_data1 = msc_y_fir_coef_data1;
        gpRealRegMSC->msc_y_fir_coef_data2 = msc_y_fir_coef_data2;
        gpRealRegMSC->msc_y_fir_coef_data3 = msc_y_fir_coef_data3;

        do {
            if (size == 4) break;
            msc_y_fir_coef_data4 = *(MSC_Y_FIR_COEF_DATA4 *)&pTable[4];
            msc_y_fir_coef_data5 = *(MSC_Y_FIR_COEF_DATA5 *)&pTable[5];
            gpRealRegMSC->msc_y_fir_coef_data4 = msc_y_fir_coef_data4;
            gpRealRegMSC->msc_y_fir_coef_data5 = msc_y_fir_coef_data5;
        } while (0);

        msc_y_fir_coef_ctrl.addr          = GET_BITS(index, 0, 4);
        msc_y_fir_coef_ctrl.h_v_sel       = GET_BITS(index, 4, 1);
        msc_y_fir_coef_ctrl.update        = 1;
        msc_y_fir_coef_ctrl.rnw           = 0;
        gpRealRegMSC->msc_y_fir_coef_ctrl = msc_y_fir_coef_ctrl;
    } else {
        msc_y_fir_coef_ctrl.addr          = GET_BITS(index, 0, 4);
        msc_y_fir_coef_ctrl.h_v_sel       = GET_BITS(index, 4, 1);
        msc_y_fir_coef_ctrl.update        = 1;
        msc_y_fir_coef_ctrl.rnw           = 1;
        gpRealRegMSC->msc_y_fir_coef_ctrl = msc_y_fir_coef_ctrl;

        msc_y_fir_coef_data0 = gpRealRegMSC->msc_y_fir_coef_data0;
        msc_y_fir_coef_data1 = gpRealRegMSC->msc_y_fir_coef_data1;
        msc_y_fir_coef_data2 = gpRealRegMSC->msc_y_fir_coef_data2;
        msc_y_fir_coef_data3 = gpRealRegMSC->msc_y_fir_coef_data3;

        pTable[0]  = *(UINT32 *)&msc_y_fir_coef_data0;
        pTable[1]  = *(UINT32 *)&msc_y_fir_coef_data1;
        pTable[2]  = *(UINT32 *)&msc_y_fir_coef_data2;
        pTable[3]  = *(UINT32 *)&msc_y_fir_coef_data3;

        do {
            if (size == 4) break;
            msc_y_fir_coef_data4 = gpRealRegMSC->msc_y_fir_coef_data4;
            msc_y_fir_coef_data5 = gpRealRegMSC->msc_y_fir_coef_data5;
            pTable[4]  = *(UINT32 *)&msc_y_fir_coef_data4;
            pTable[5]  = *(UINT32 *)&msc_y_fir_coef_data5;
        } while (0);
    }

    return RET_OK;
}

static int DE_PRM_LoadMscrFirCCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    MSC_C_FIR_COEF_DATA0 msc_c_fir_coef_data0;
    MSC_C_FIR_COEF_DATA1 msc_c_fir_coef_data1;
    MSC_C_FIR_COEF_DATA2 msc_c_fir_coef_data2;
    MSC_C_FIR_COEF_DATA3 msc_c_fir_coef_data3;
    MSC_C_FIR_COEF_CTRL  msc_c_fir_coef_ctrl;

    if (bWrite) {
        msc_c_fir_coef_data0 = *(MSC_C_FIR_COEF_DATA0 *)&pTable[0];
        msc_c_fir_coef_data1 = *(MSC_C_FIR_COEF_DATA1 *)&pTable[1];
        msc_c_fir_coef_data2 = *(MSC_C_FIR_COEF_DATA2 *)&pTable[2];
        msc_c_fir_coef_data3 = *(MSC_C_FIR_COEF_DATA3 *)&pTable[3];

        gpRealRegMSC->msc_c_fir_coef_data0 = msc_c_fir_coef_data0;
        gpRealRegMSC->msc_c_fir_coef_data1 = msc_c_fir_coef_data1;
        gpRealRegMSC->msc_c_fir_coef_data2 = msc_c_fir_coef_data2;
        gpRealRegMSC->msc_c_fir_coef_data3 = msc_c_fir_coef_data3;

        msc_c_fir_coef_ctrl.addr          = GET_BITS(index, 0, 4);
        msc_c_fir_coef_ctrl.h_v_sel       = GET_BITS(index, 4, 1);
        msc_c_fir_coef_ctrl.update        = 1;
        msc_c_fir_coef_ctrl.rnw           = 0;
        gpRealRegMSC->msc_c_fir_coef_ctrl = msc_c_fir_coef_ctrl;
    } else {
        msc_c_fir_coef_ctrl.addr          = GET_BITS(index, 0, 4);
        msc_c_fir_coef_ctrl.h_v_sel       = GET_BITS(index, 4, 1);
        msc_c_fir_coef_ctrl.update        = 1;
        msc_c_fir_coef_ctrl.rnw           = 1;
        gpRealRegMSC->msc_c_fir_coef_ctrl = msc_c_fir_coef_ctrl;

        msc_c_fir_coef_data0 = gpRealRegMSC->msc_c_fir_coef_data0;
        msc_c_fir_coef_data1 = gpRealRegMSC->msc_c_fir_coef_data1;
        msc_c_fir_coef_data2 = gpRealRegMSC->msc_c_fir_coef_data2;
        msc_c_fir_coef_data3 = gpRealRegMSC->msc_c_fir_coef_data3;

        pTable[0] = *(UINT32 *)&msc_c_fir_coef_data0;
        pTable[1] = *(UINT32 *)&msc_c_fir_coef_data1;
        pTable[2] = *(UINT32 *)&msc_c_fir_coef_data2;
        pTable[3] = *(UINT32 *)&msc_c_fir_coef_data3;
    }

    return RET_OK;
}

static int DE_PRM_LoadSscrFirYCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    SSC_Y_FIR_COEF_DATA0 ssc_y_fir_coef_data0;
    SSC_Y_FIR_COEF_DATA1 ssc_y_fir_coef_data1;
    SSC_Y_FIR_COEF_DATA2 ssc_y_fir_coef_data2;
    SSC_Y_FIR_COEF_DATA3 ssc_y_fir_coef_data3;
    SSC_Y_FIR_COEF_CTRL  ssc_y_fir_coef_ctrl;

    if (bWrite) {
        ssc_y_fir_coef_data0 = *(SSC_Y_FIR_COEF_DATA0 *)&pTable[0];
        ssc_y_fir_coef_data1 = *(SSC_Y_FIR_COEF_DATA1 *)&pTable[1];

        gpRealRegSSC->ssc_y_fir_coef_data0 = ssc_y_fir_coef_data0;
        gpRealRegSSC->ssc_y_fir_coef_data1 = ssc_y_fir_coef_data1;

        do {
            if (size == 2) break;
            ssc_y_fir_coef_data2 = *(SSC_Y_FIR_COEF_DATA2 *)&pTable[2];
            ssc_y_fir_coef_data3 = *(SSC_Y_FIR_COEF_DATA3 *)&pTable[3];
            gpRealRegSSC->ssc_y_fir_coef_data2 = ssc_y_fir_coef_data2;
            gpRealRegSSC->ssc_y_fir_coef_data3 = ssc_y_fir_coef_data3;
        } while (0);

        ssc_y_fir_coef_ctrl.addr          = GET_BITS(index, 0, 4);
        ssc_y_fir_coef_ctrl.h_v_sel       = GET_BITS(index, 4, 1);
        ssc_y_fir_coef_ctrl.update        = 1;
        ssc_y_fir_coef_ctrl.rnw           = 0;
        gpRealRegSSC->ssc_y_fir_coef_ctrl = ssc_y_fir_coef_ctrl;
    } else {
        ssc_y_fir_coef_ctrl.addr          = GET_BITS(index, 0, 4);
        ssc_y_fir_coef_ctrl.h_v_sel       = GET_BITS(index, 4, 1);
        ssc_y_fir_coef_ctrl.update        = 1;
        ssc_y_fir_coef_ctrl.rnw           = 1;
        gpRealRegSSC->ssc_y_fir_coef_ctrl = ssc_y_fir_coef_ctrl;

        ssc_y_fir_coef_data0 = gpRealRegSSC->ssc_y_fir_coef_data0;
        ssc_y_fir_coef_data1 = gpRealRegSSC->ssc_y_fir_coef_data1;
        pTable[0] = *(UINT32 *)&ssc_y_fir_coef_data0;
        pTable[1] = *(UINT32 *)&ssc_y_fir_coef_data1;

        do {
            if (size == 2) break;
            ssc_y_fir_coef_data2 = gpRealRegSSC->ssc_y_fir_coef_data2;
            ssc_y_fir_coef_data3 = gpRealRegSSC->ssc_y_fir_coef_data3;
            pTable[2] = *(UINT32 *)&ssc_y_fir_coef_data2;
            pTable[3] = *(UINT32 *)&ssc_y_fir_coef_data3;
        } while (0);
    }

    return RET_OK;
}

static int DE_PRM_LoadMscrPhaseOffset(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
	MSC_Y_INIT_POS_H msc_y_init_pos_h = { 0 };
	MSC_Y_INIT_POS_V msc_y_init_pos_v = { 0 };
	MSC_C_INIT_POS_H msc_c_init_pos_h = { 0 };
	MSC_C_INIT_POS_V msc_c_init_pos_v = { 0 };

    if (bWrite) {
        msc_y_init_pos_h.y_init_pos_h = pTable[0];
        msc_y_init_pos_v.y_init_pos_v = pTable[1];
        msc_c_init_pos_h.c_init_pos_h = pTable[2];
        msc_c_init_pos_v.c_init_pos_v = pTable[3];

        gpRealRegMSC->msc_y_init_pos_h = msc_y_init_pos_h;
        gpRealRegMSC->msc_y_init_pos_v = msc_y_init_pos_v;
        gpRealRegMSC->msc_c_init_pos_h = msc_c_init_pos_h;
        gpRealRegMSC->msc_c_init_pos_v = msc_c_init_pos_v;
    } else {
        msc_y_init_pos_h = gpRealRegMSC->msc_y_init_pos_h;
        msc_y_init_pos_v = gpRealRegMSC->msc_y_init_pos_v;
        msc_c_init_pos_h = gpRealRegMSC->msc_c_init_pos_h;
        msc_c_init_pos_v = gpRealRegMSC->msc_c_init_pos_v;

        pTable[0] = msc_y_init_pos_h.y_init_pos_h;
        pTable[1] = msc_y_init_pos_v.y_init_pos_v;
        pTable[2] = msc_c_init_pos_h.c_init_pos_h;
        pTable[3] = msc_c_init_pos_v.c_init_pos_v;
    }

    return RET_OK;
}

static int DE_PRM_LoadSscrPhaseOffset(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
	SSC_Y_INIT_POS_H ssc_y_init_pos_h = { 0 };
	SSC_Y_INIT_POS_V ssc_y_init_pos_v = { 0 };
	SSC_C_INIT_POS_H ssc_c_init_pos_h = { 0 };
	SSC_C_INIT_POS_V ssc_c_init_pos_v = { 0 };

    if (bWrite) {
        ssc_y_init_pos_h.y_init_pos_h = pTable[0];
        ssc_y_init_pos_v.y_init_pos_v = pTable[1];
        ssc_c_init_pos_h.c_init_pos_h = pTable[2];
        ssc_c_init_pos_v.c_init_pos_v = pTable[3];

        gpRealRegSSC->ssc_y_init_pos_h = ssc_y_init_pos_h;
        gpRealRegSSC->ssc_y_init_pos_v = ssc_y_init_pos_v;
        gpRealRegSSC->ssc_c_init_pos_h = ssc_c_init_pos_h;
        gpRealRegSSC->ssc_c_init_pos_v = ssc_c_init_pos_v;
    } else {
        ssc_y_init_pos_h = gpRealRegSSC->ssc_y_init_pos_h;
        ssc_y_init_pos_v = gpRealRegSSC->ssc_y_init_pos_v;
        ssc_c_init_pos_h = gpRealRegSSC->ssc_c_init_pos_h;
        ssc_c_init_pos_v = gpRealRegSSC->ssc_c_init_pos_v;

        pTable[0] = ssc_y_init_pos_h.y_init_pos_h;
        pTable[1] = ssc_y_init_pos_v.y_init_pos_v;
        pTable[2] = ssc_c_init_pos_h.c_init_pos_h;
        pTable[3] = ssc_c_init_pos_v.c_init_pos_v;
    }

    return RET_OK;
}

static int DE_PRM_LoadCvmHdnCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    CVM_HDN_COEFF cvm_hdn_coeff = {0};

    if (bWrite) {
        for (index=0;index<size;index++){
            cvm_hdn_coeff = *(CVM_HDN_COEFF*)&pTable[index];
            cvm_hdn_coeff.rw = FALSE;
            cvm_hdn_coeff.coeff_index = index;
            gpRealRegDE0B_CTRL->cvm_hdn_coeff = cvm_hdn_coeff;
        }
    } else {
        cvm_hdn_coeff.rw = TRUE;
        for (index=0;index<size;index++){
            cvm_hdn_coeff.coeff_index = index;
            gpRealRegDE0B_CTRL->cvm_hdn_coeff = cvm_hdn_coeff;
            pTable[index] = *(UINT32*)&gpRealRegDE0B_CTRL->cvm_hdn_coeff;
        }
    }

    return RET_OK;
}

static int DE_PRM_LoadMdmHdnCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    MDM_HDN_COEFF mdm_hdn_coeff = {0};

    if (bWrite) {
        mdm_hdn_coeff.rw = FALSE;
        for (index=0;index<size;index++){
            mdm_hdn_coeff = *(MDM_HDN_COEFF*)&pTable[index];
            mdm_hdn_coeff.coeff_index = index;
            gpRealRegDE0B_CTRL->mdm_hdn_coeff = mdm_hdn_coeff;
        }
    } else {
        mdm_hdn_coeff.rw = TRUE;
        for (index=0;index<size;index++){
            mdm_hdn_coeff.coeff_index = index;
            gpRealRegDE0B_CTRL->mdm_hdn_coeff = mdm_hdn_coeff;
            pTable[index] = *(UINT32*)&gpRealRegDE0B_CTRL->mdm_hdn_coeff;
        }
    }

    return RET_OK;
}

static int DE_PRM_LoadDlrCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
#ifdef USE_KDRV_CODES_FOR_L8
    if (bWrite) {
		gpRealRegPRE3D->reg_quincunx_coeff0 = *(REG_QUINCUNX_COEFF0 *)&pTable[0];
		gpRealRegPRE3D->reg_quincunx_coeff1 = *(REG_QUINCUNX_COEFF1 *)&pTable[1];
		gpRealRegPRE3D->reg_quincunx_coeff2 = *(REG_QUINCUNX_COEFF2 *)&pTable[2];
		gpRealRegPRE3D->reg_quincunx_coeff3 = *(REG_QUINCUNX_COEFF3 *)&pTable[3];
		gpRealRegPRE3D->reg_quincunx_coeff4 = *(REG_QUINCUNX_COEFF4 *)&pTable[4];
		gpRealRegPRE3D->reg_quincunx_coeff5 = *(REG_QUINCUNX_COEFF5 *)&pTable[5];
		gpRealRegPRE3D->reg_quincunx_coeff6 = *(REG_QUINCUNX_COEFF6 *)&pTable[6];
    } else {
		pTable[0] = *(UINT32 *)&gpRealRegPRE3D->reg_quincunx_coeff0;
		pTable[1] = *(UINT32 *)&gpRealRegPRE3D->reg_quincunx_coeff1;
		pTable[2] = *(UINT32 *)&gpRealRegPRE3D->reg_quincunx_coeff2;
		pTable[3] = *(UINT32 *)&gpRealRegPRE3D->reg_quincunx_coeff3;
		pTable[4] = *(UINT32 *)&gpRealRegPRE3D->reg_quincunx_coeff4;
		pTable[5] = *(UINT32 *)&gpRealRegPRE3D->reg_quincunx_coeff5;
		pTable[6] = *(UINT32 *)&gpRealRegPRE3D->reg_quincunx_coeff6;
    }
#endif

    return RET_OK;
}

static int DE_PRM_LoadDlrPhase(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
#ifdef USE_KDRV_CODES_FOR_L8
    if (bWrite) {
		gpRealRegPRE3D->reg_phase_ctrl = *(REG_PHASE_CTRL *)&pTable[0];
		gpRealRegPRE3D->reg_phasewd0   = *(REG_PHASEWD0   *)&pTable[1];
		gpRealRegPRE3D->reg_phasewd1   = *(REG_PHASEWD1   *)&pTable[2];
		gpRealRegPRE3D->reg_phasewd2   = *(REG_PHASEWD2   *)&pTable[3];
		gpRealRegPRE3D->reg_phasewd3   = *(REG_PHASEWD3   *)&pTable[4];
		gpRealRegPRE3D->reg_phasewd4   = *(REG_PHASEWD4   *)&pTable[5];
		gpRealRegPRE3D->reg_phasewd5   = *(REG_PHASEWD5   *)&pTable[6];
    } else {
		pTable[0] = *(UINT32 *)&gpRealRegPRE3D->reg_phase_ctrl;
		pTable[1] = *(UINT32 *)&gpRealRegPRE3D->reg_phasewd0;
		pTable[2] = *(UINT32 *)&gpRealRegPRE3D->reg_phasewd1;
		pTable[3] = *(UINT32 *)&gpRealRegPRE3D->reg_phasewd2;
		pTable[4] = *(UINT32 *)&gpRealRegPRE3D->reg_phasewd3;
		pTable[5] = *(UINT32 *)&gpRealRegPRE3D->reg_phasewd4;
		pTable[6] = *(UINT32 *)&gpRealRegPRE3D->reg_phasewd5;
    }
#endif

    return RET_OK;
}

static int DE_PRM_LoadMdmCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
    MDM_CSC_COEF0   mdm_csc_coef0;
    MDM_CSC_COEF1   mdm_csc_coef1;
    MDM_CSC_COEF2   mdm_csc_coef2;
    MDM_CSC_COEF3   mdm_csc_coef3;
    MDM_CSC_COEF4   mdm_csc_coef4;
    MDM_CSC_OFFSET0 mdm_csc_offset0;
    MDM_CSC_OFFSET1 mdm_csc_offset1;
    MDM_CSC_OFFSET2 mdm_csc_offset2;

    if (bWrite) {
        mdm_csc_coef0   = *(MDM_CSC_COEF0   *)&pTable[0];
        mdm_csc_coef1   = *(MDM_CSC_COEF1   *)&pTable[1];
        mdm_csc_coef2   = *(MDM_CSC_COEF2   *)&pTable[2];
        mdm_csc_coef3   = *(MDM_CSC_COEF3   *)&pTable[3];
        mdm_csc_coef4   = *(MDM_CSC_COEF4   *)&pTable[4];
        mdm_csc_offset0 = *(MDM_CSC_OFFSET0 *)&pTable[5];
        mdm_csc_offset1 = *(MDM_CSC_OFFSET1 *)&pTable[6];
        mdm_csc_offset2 = *(MDM_CSC_OFFSET2 *)&pTable[7];

        gpRealRegDE0B_CTRL->mdm_csc_coef0   = mdm_csc_coef0;
        gpRealRegDE0B_CTRL->mdm_csc_coef1   = mdm_csc_coef1;
        gpRealRegDE0B_CTRL->mdm_csc_coef2   = mdm_csc_coef2;
        gpRealRegDE0B_CTRL->mdm_csc_coef3   = mdm_csc_coef3;
        gpRealRegDE0B_CTRL->mdm_csc_coef4   = mdm_csc_coef4;
        gpRealRegDE0B_CTRL->mdm_csc_offset0 = mdm_csc_offset0;
        gpRealRegDE0B_CTRL->mdm_csc_offset1 = mdm_csc_offset1;
        gpRealRegDE0B_CTRL->mdm_csc_offset2 = mdm_csc_offset2;
    } else {
        mdm_csc_coef0   = gpRealRegDE0B_CTRL->mdm_csc_coef0;
        mdm_csc_coef1   = gpRealRegDE0B_CTRL->mdm_csc_coef1;
        mdm_csc_coef2   = gpRealRegDE0B_CTRL->mdm_csc_coef2;
        mdm_csc_coef3   = gpRealRegDE0B_CTRL->mdm_csc_coef3;
        mdm_csc_coef4   = gpRealRegDE0B_CTRL->mdm_csc_coef4;
        mdm_csc_offset0 = gpRealRegDE0B_CTRL->mdm_csc_offset0;
        mdm_csc_offset1 = gpRealRegDE0B_CTRL->mdm_csc_offset1;
        mdm_csc_offset2 = gpRealRegDE0B_CTRL->mdm_csc_offset2;

        pTable[0] = *(UINT32 *)&mdm_csc_coef0;
        pTable[1] = *(UINT32 *)&mdm_csc_coef1;
        pTable[2] = *(UINT32 *)&mdm_csc_coef2;
        pTable[3] = *(UINT32 *)&mdm_csc_coef3;
        pTable[3] = *(UINT32 *)&mdm_csc_coef4;
        pTable[5] = *(UINT32 *)&mdm_csc_offset0;
        pTable[6] = *(UINT32 *)&mdm_csc_offset1;
        pTable[7] = *(UINT32 *)&mdm_csc_offset2;
    }

    return RET_OK;
}

static int DE_PRM_LoadPostCscCoef(DE_PARAM_WR_T bWrite, UINT32 index, UINT32 *pTable, UINT32 size)
{
#ifdef USE_KDRV_CODES_FOR_L8
    if (bWrite) {
        PE1_RdFL(pe1_xvy_ctrl_08);
        PE1_Wr01(pe1_xvy_ctrl_01, r_csc_coef0, pTable[0]);
        PE1_Wr01(pe1_xvy_ctrl_01, r_csc_coef1, pTable[1]);
        PE1_Wr01(pe1_xvy_ctrl_02, r_csc_coef2, pTable[2]);
        PE1_Wr01(pe1_xvy_ctrl_02, r_csc_coef3, pTable[3]);
        PE1_Wr01(pe1_xvy_ctrl_03, r_csc_coef4, pTable[4]);
        PE1_Wr01(pe1_xvy_ctrl_03, r_csc_coef5, pTable[5]);
        PE1_Wr01(pe1_xvy_ctrl_04, r_csc_coef6, pTable[6]);
        PE1_Wr01(pe1_xvy_ctrl_04, r_csc_coef7, pTable[7]);
        PE1_Wr01(pe1_xvy_ctrl_05, r_csc_coef8, pTable[8]);
        PE1_Wr01(pe1_xvy_ctrl_05, r_csc_ofst0, pTable[9]);
        PE1_Wr01(pe1_xvy_ctrl_06, r_csc_ofst1, pTable[10]);
        PE1_Wr01(pe1_xvy_ctrl_06, r_csc_ofst2, pTable[11]);
        PE1_Wr01(pe1_xvy_ctrl_07, r_csc_ofst3, pTable[12]);
        PE1_Wr01(pe1_xvy_ctrl_07, r_csc_ofst4, pTable[13]);
        PE1_Wr01(pe1_xvy_ctrl_08, r_csc_ofst5, pTable[14]);

        PE1_WrFL(pe1_xvy_ctrl_01);
        PE1_WrFL(pe1_xvy_ctrl_02);
        PE1_WrFL(pe1_xvy_ctrl_03);
        PE1_WrFL(pe1_xvy_ctrl_04);
        PE1_WrFL(pe1_xvy_ctrl_05);
        PE1_WrFL(pe1_xvy_ctrl_06);
        PE1_WrFL(pe1_xvy_ctrl_07);
        PE1_WrFL(pe1_xvy_ctrl_08);
    } else {
        PE1_RdFL(pe1_xvy_ctrl_01);
        PE1_RdFL(pe1_xvy_ctrl_02);
        PE1_RdFL(pe1_xvy_ctrl_03);
        PE1_RdFL(pe1_xvy_ctrl_04);
        PE1_RdFL(pe1_xvy_ctrl_05);
        PE1_RdFL(pe1_xvy_ctrl_06);
        PE1_RdFL(pe1_xvy_ctrl_07);
        PE1_RdFL(pe1_xvy_ctrl_08);
        PE1_Rd01(pe1_xvy_ctrl_01, r_csc_coef0, pTable[0]);
        PE1_Rd01(pe1_xvy_ctrl_01, r_csc_coef1, pTable[1]);
        PE1_Rd01(pe1_xvy_ctrl_02, r_csc_coef2, pTable[2]);
        PE1_Rd01(pe1_xvy_ctrl_02, r_csc_coef3, pTable[3]);
        PE1_Rd01(pe1_xvy_ctrl_03, r_csc_coef4, pTable[4]);
        PE1_Rd01(pe1_xvy_ctrl_03, r_csc_coef5, pTable[5]);
        PE1_Rd01(pe1_xvy_ctrl_04, r_csc_coef6, pTable[6]);
        PE1_Rd01(pe1_xvy_ctrl_04, r_csc_coef7, pTable[7]);
        PE1_Rd01(pe1_xvy_ctrl_05, r_csc_coef8, pTable[8]);
        PE1_Rd01(pe1_xvy_ctrl_05, r_csc_ofst0, pTable[9]);
        PE1_Rd01(pe1_xvy_ctrl_06, r_csc_ofst1, pTable[10]);
        PE1_Rd01(pe1_xvy_ctrl_06, r_csc_ofst2, pTable[11]);
        PE1_Rd01(pe1_xvy_ctrl_07, r_csc_ofst3, pTable[12]);
        PE1_Rd01(pe1_xvy_ctrl_07, r_csc_ofst4, pTable[13]);
        PE1_Rd01(pe1_xvy_ctrl_08, r_csc_ofst5, pTable[14]);
    }
#endif

    return RET_OK;
}

/**  @} */
