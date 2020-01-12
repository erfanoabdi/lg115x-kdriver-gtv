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
 * author	  jaemo.kim (jaemo.kim@lge.com)
 * version	  1.0
 * date		  2010.02.18
 * note		  Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */

/*----------------------------------------------------------------------------------------
 *	 Control Constants
 *---------------------------------------------------------------------------------------*/
#define USE_DE_SWAP_DDR_MEMORY

/*----------------------------------------------------------------------------------------
 *	 File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <mach/platform.h>
#include <linux/delay.h>
#include <asm/io.h>

#include "base_types.h"
#include "os_util.h"

#include "de_model.h"
#include "de_kapi.h"
#include "de_def.h"
#include "de_ver_def.h"

#include "l8/de_ipc_def_l8.h"
#include "l8/de_cfg_l8.h"
#include "de_int_def.h"
#include "de_prm_def.h"
#include "de_hal_def.h"
#include "de_cfg.h"
#include "de_drv.h"
#include "l8/de_reg_def_l8.h"
#include "l8/de_reg_l8.h"
#include "l8/de_prm_l8.h"

#ifdef USE_KDRV_CODES_FOR_L8
#include "de_pe0_reg_l8.h"		// 0x0300
#include "de_pe1_reg_l8.h"		// 0x1200
#include "fmt3d_reg.h"				// 0x0200
#include "ctop_regs.h"		        // 0xfd300000
#endif

#include "l8/de_mif_reg_l8.h"		// 0x0600, 0x0a00, 0x1500, 0x1b00
#include "l8/de_cvi_reg_l8.h"		// 0x0100
#include "l8/de_ov_reg_l8.h"		// 0x1100
#include "l8/de_top_reg_l8.h"		// 0x0000, 0x0800, 0x1000, 0x1800
#include "l8/de_msc_reg_l8.h"		// 0x0500
#include "l8/de_ssc_reg_l8.h"		// 0x0900
#include "l8/de_spp_reg_l8.h"		// 0x1f80
#include "l8/de_oif_reg_l8.h"		// 0x1c800
//#include "l8/ctop_reg_l8.h"		// 0xfd300000

//#include "../afe/vport_reg.h"      // 0xf6007000
//#include "../hdmi/hdmi_phy_reg.h"  // 0xf6007000

/*----------------------------------------------------------------------------------------
 *	 Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define  SIZE_OF_IPC_FOR_CPU 13

/*----------------------------------------------------------------------------------------
 *	 Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define OIF_WfCM(_r, _f, _c, _m)		(gpRegOIF->_r._f)		= (_c) ? ((_m)|gpRegOIF->_r._f)       : (~(_m)&gpRegOIF->_r._f)
#define CHA_CVI_WfCM(_r, _f, _c, _m)	(gpRegCHA_CVI->_r._f)	= (_c) ? ((_m)|gpRegCHA_CVI->_r._f)   : (~(_m)&gpRegCHA_CVI->_r._f)
#define CHB_CVI_WfCM(_r, _f, _c, _m)	(gpRegCHB_CVI->_r._f)	= (_c) ? ((_m)|gpRegCHB_CVI->_r._f)   : (~(_m)&gpRegCHB_CVI->_r._f)
#define MSC_WfCM(_r, _f, _c, _m)		(gpRegMSC->_r._f)	    = (_c) ? ((_m)|gpRegMSC->_r._f)       : (~(_m)&gpRegMSC->_r._f)

#ifdef USE_PQL_REG_DEFINED_IN_DE
#define PE0_WfCM(_r, _f, _c, _m)		(gpRegPE0->_r._f)		= (_c) ? ((_m)|gpRegPE0->_r._f)       : (~(_m)&gpRegPE0->_r._f)
#define PE1_WfCM(_r, _f, _c, _m)		(gpRegPE1->_r._f)		= (_c) ? ((_m)|gpRegPE1->_r._f)       : (~(_m)&gpRegPE1->_r._f)
#endif

#ifdef USE_KDRV_CODES_FOR_L8
#define CTOP_CTRL_B0_WfCM(_r, _f, _c, _m)	(CTOP_SHADOW_B0->_r._f) = (_c) ? ((_m)|CTOP_SHADOW_B0->_r._f) : (~(_m)&CTOP_SHADOW_B0->_r._f)
#define CTOP_CTRL_A0_WfCM(_r, _f, _c, _m)	(CTOP_SHADOW_A0->_r._f) = (_c) ? ((_m)|CTOP_SHADOW_A0->_r._f) : (~(_m)&CTOP_SHADOW_A0->_r._f)
#define CTOP_CTRL_WfCM		CTOP_CTRL_B0_WfCM
#endif

/*----------------------------------------------------------------------------------------
 *	 Type Definitions
 *---------------------------------------------------------------------------------------*/
#define MAX_PE_TPG_RECT 32
/*----------------------------------------------------------------------------------------
 *	 External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 External Variables
 *---------------------------------------------------------------------------------------*/
#ifdef USE_DE_FIRMWARE_DWONLOAD_IN_DRIVER
extern LX_DE_MEM_CFG_T *gpDeMem;
#endif
//extern volatile VPORT_Reg_T *gVportReg;
//extern volatile HDMI_PHY3P_Reg_T *pstHdmiPhyReg;

/*----------------------------------------------------------------------------------------
 *	 global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 global Variables
 *---------------------------------------------------------------------------------------*/
BOOLEAN g_b_InitTPG = FALSE;
UINT32  g_patchEn   = 0;


/*----------------------------------------------------------------------------------------
 *	 Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *	 Static Variables
 *---------------------------------------------------------------------------------------*/
static DE_DPLL_SET_T sDisplayPll[] = {
	{ DCLK_61_875	,{ 216 ,1980 ,4  } },
	{ DCLK_65		,{ 216 ,2080 ,4  } },
	{ DCLK_66_462	,{ 260 ,2560 ,4  } },
	{ DCLK_66_528	,{ 250 ,2464 ,4  } },
	{ DCLK_36_818	,{ 220 ,1800 ,6  } },
	{ DCLK_36_855	,{ 300 ,2457 ,6  } },
	{ DCLK_54		,{ 216 ,2592 ,6  } },
	{ DCLK_54_054	,{ 250 ,3003 ,6  } },
	{ DCLK_74_1758	,{ 273 ,3000 ,4  } },
	{ DCLK_74_25	,{ 252 ,2772 ,4  } },
	{ DCLK_80_109	,{ 273 ,3240 ,4  } },
	{ DCLK_80_190	,{ 200 ,2376 ,4  } },
	{ DCLK_81		,{ 273 ,3276 ,4  } },
	{ DCLK_27		,{ 250 ,2000 ,8  } },
	{ DCLK_13_5		,{ 216 ,1728 ,16 } },
	{ DCLK_27_027	,{ 250 ,2002 ,8  } },
	{ DCLK_13_5135	,{ 250 ,2002 ,16 } },
	{ DCLK_148_3516 ,{ 273 ,3000 ,2  } },
	{ DCLK_148_5	,{ 252 ,2772 ,2  } },
	{ DCLK_85_86	,{ 252 ,2772 ,2  } },
};

#ifdef USE_QEMU_SYSTEM
MIF0_REG_T		 *gpRealRegMIF0		 = NULL;
MIF1_REG_T		 *gpRealRegMIF1		 = NULL;
MIF2_REG_T		 *gpRealRegMIF2		 = NULL;
MIF3_REG_T		 *gpRealRegMIF3		 = NULL;

CHA_CVI_REG_T	 *gpRealRegCHA_CVI	 = NULL;
CHB_CVI_REG_T	 *gpRealRegCHB_CVI	 = NULL;
PRE3D_REG_T		 *gpRealRegPRE3D	 = NULL;

MSC_REG_T		 *gpRealRegMSC		 = NULL;
SSC_REG_T		 *gpRealRegSSC		 = NULL;

DE0A_CTRL_REG_T  *gpRealRegDE0A_CTRL = NULL;
DE0B_CTRL_REG_T  *gpRealRegDE0B_CTRL = NULL;
DE1A_CTRL_REG_T  *gpRealRegDE1A_CTRL = NULL;
DE1B_CTRL_REG_T  *gpRealRegDE1B_CTRL = NULL;

ov_top_REG_T	 *gpRealRegov_top	 = NULL;

#ifdef USE_PQL_REG_DEFINED_IN_DE
PE0_REG_T		 *gpRealRegPE0		 = NULL;
PE1_REG_T		 *gpRealRegPE1		 = NULL;
#endif
SPP_MCU_REG_T	 *gpRealRegSPP_MCU	 = NULL;
eDMA_REG_T		 *gpRealRegeDMA		 = NULL;

OIF_REG_T		 *gpRealRegOIF		 = NULL;
//CTOP_CTRL_REG_T	 *gpRealRegCTOP_CTRL = NULL;
#elif defined(USE_VIDEO_IOREMAP)
volatile MIF0_REG_T		 *gpRealRegMIF0		 = NULL;
volatile MIF1_REG_T		 *gpRealRegMIF1		 = NULL;
volatile MIF2_REG_T		 *gpRealRegMIF2		 = NULL;
volatile MIF3_REG_T		 *gpRealRegMIF3		 = NULL;

volatile CHA_CVI_REG_T	 *gpRealRegCHA_CVI	 = NULL;
volatile CHB_CVI_REG_T	 *gpRealRegCHB_CVI	 = NULL;
volatile PRE3D_REG_T	 *gpRealRegPRE3D	 = NULL;

volatile MSC_REG_T		 *gpRealRegMSC		 = NULL;
volatile SSC_REG_T		 *gpRealRegSSC		 = NULL;

volatile DE0A_CTRL_REG_T *gpRealRegDE0A_CTRL = NULL;
volatile DE0B_CTRL_REG_T *gpRealRegDE0B_CTRL = NULL;
volatile DE1A_CTRL_REG_T *gpRealRegDE1A_CTRL = NULL;
volatile DE1B_CTRL_REG_T *gpRealRegDE1B_CTRL = NULL;

volatile ov_top_REG_T	 *gpRealRegov_top	 = NULL;

#ifdef USE_PQL_REG_DEFINED_IN_DE
volatile PE0_REG_T		 *gpRealRegPE0		 = NULL;
volatile PE1_REG_T		 *gpRealRegPE1		 = NULL;
#endif
volatile SPP_MCU_REG_T	 *gpRealRegSPP_MCU	 = NULL;
volatile eDMA_REG_T		 *gpRealRegeDMA		 = NULL;

volatile OIF_REG_T		 *gpRealRegOIF		 = NULL;
//volatile CTOP_CTRL_REG_T *gpRealRegCTOP_CTRL = NULL;
#else
volatile MIF0_REG_T		 *gpRealRegMIF0		 = (volatile MIF0_REG_T		 *)(VIDEO_MIF0_REG_BASE);
volatile MIF1_REG_T		 *gpRealRegMIF1		 = (volatile MIF1_REG_T		 *)(VIDEO_MIF1_REG_BASE);
volatile MIF2_REG_T		 *gpRealRegMIF2		 = (volatile MIF2_REG_T		 *)(VIDEO_MIF2_REG_BASE);
volatile MIF3_REG_T		 *gpRealRegMIF3		 = (volatile MIF3_REG_T		 *)(VIDEO_MIF3_REG_BASE);

volatile CHA_CVI_REG_T	 *gpRealRegCHA_CVI	 = (volatile CHA_CVI_REG_T	 *)(VIDEO_CVA_TOP_REG_BASE);
volatile CHB_CVI_REG_T	 *gpRealRegCHB_CVI	 = (volatile CHB_CVI_REG_T	 *)(VIDEO_CVB_TOP_REG_BASE);
#ifdef USE_KDRV_CODES_FOR_L8
volatile PRE3D_REG_T	 *gpRealRegPRE3D	 = (volatile PRE3D_REG_T	 *)(VIDEO_DLR_REG_BASE);
#endif

volatile MSC_REG_T		 *gpRealRegMSC		 = (volatile MSC_REG_T		 *)(VIDEO_MSCR_REG_BASE);
volatile SSC_REG_T		 *gpRealRegSSC		 = (volatile SSC_REG_T		 *)(VIDEO_SSCR_REG_BASE);

volatile DE0A_CTRL_REG_T *gpRealRegDE0A_CTRL = (volatile DE0A_CTRL_REG_T *)(VIDEO_DE0A_HIF_REG_BASE);
volatile DE0B_CTRL_REG_T *gpRealRegDE0B_CTRL = (volatile DE0B_CTRL_REG_T *)(VIDEO_DE0B_HIF_REG_BASE);
volatile DE1A_CTRL_REG_T *gpRealRegDE1A_CTRL = (volatile DE1A_CTRL_REG_T *)(VIDEO_DE1A_HIF_REG_BASE);
volatile DE1B_CTRL_REG_T *gpRealRegDE1B_CTRL = (volatile DE1B_CTRL_REG_T *)(VIDEO_DE1B_HIF_REG_BASE);

volatile ov_top_REG_T	 *gpRealRegov_top	 = (volatile ov_top_REG_T	 *)(VIDEO_OV_REG_BASE);

#ifdef USE_KDRV_CODES_FOR_L8
#ifdef USE_PQL_REG_DEFINED_IN_DE
volatile PE0_REG_T		 *gpRealRegPE0		 = (volatile PE0_REG_T		 *)(VIDEO_PE0_REG_BASE);
volatile PE1_REG_T		 *gpRealRegPE1		 = (volatile PE1_REG_T		 *)(VIDEO_PE1_REG_BASE);
#endif
#endif
volatile SPP_MCU_REG_T	 *gpRealRegSPP_MCU	 = (volatile SPP_MCU_REG_T	 *)(VIDEO_IPC_REG_BASE);
volatile eDMA_REG_T		 *gpRealRegeDMA		 = (volatile eDMA_REG_T		 *)(VIDEO_DMA_REG_BASE);

volatile OIF_REG_T 		 *gpRealRegOIF		 = (volatile OIF_REG_T       *)(DTVSOC_OIF_BASE);
//volatile CTOP_CTRL_REG_T *gpRealRegCTOP_CTRL = (volatile CTOP_CTRL_REG_T *)(DTVSOC_TOP_CTRL);
#endif
MIF0_REG_T		*gpRegMIF0			= NULL;
MIF1_REG_T		*gpRegMIF1			= NULL;
MIF2_REG_T		*gpRegMIF2			= NULL;
MIF3_REG_T		*gpRegMIF3			= NULL;

CHA_CVI_REG_T	*gpRegCHA_CVI		= NULL;
CHB_CVI_REG_T	*gpRegCHB_CVI		= NULL;
#ifdef USE_KDRV_CODES_FOR_L8
PRE3D_REG_T		*gpRegPRE3D			= NULL;
#endif

MSC_REG_T		*gpRegMSC			= NULL;
SSC_REG_T		*gpRegSSC			= NULL;

DE0A_CTRL_REG_T *gpRegDE0A_CTRL		= NULL;
DE0B_CTRL_REG_T *gpRegDE0B_CTRL		= NULL;
DE1A_CTRL_REG_T *gpRegDE1A_CTRL		= NULL;
DE1B_CTRL_REG_T *gpRegDE1B_CTRL		= NULL;

ov_top_REG_T	*gpRegov_top		= NULL;

#ifdef USE_KDRV_CODES_FOR_L8
#ifdef USE_PQL_REG_DEFINED_IN_DE
PE0_REG_T		*gpRegPE0			= NULL;
PE1_REG_T		*gpRegPE1			= NULL;
#endif
#endif
SPP_MCU_REG_T	*gpRegSPP_MCU		= NULL;
eDMA_REG_T		*gpRegeDMA			= NULL;

OIF_REG_T	    *gpRegOIF			= NULL;
//CTOP_CTRL_REG_T *gpRegCTOP_CTRL     = NULL;
UINT32          *spVideoIPCofCPU    = NULL;

//volatile UINT32 *gpRealRegSilkLoad0 = (volatile UINT32 *)(DTVSOC_SILKROAD0_BASE);
//volatile UINT32 *gpRealRegSilkLoad1 = (volatile UINT32 *)(DTVSOC_SILKROAD1_BASE);
//volatile UINT32 *gpRealRegSilkLoad2 = (volatile UINT32 *)(DTVSOC_SILKROAD2_BASE);

/*========================================================================================
 *	 Implementation Group
 *=======================================================================================*/
/**
 * @callgraph
 * @callergraph
 *
 * @brief Initialize Reigerter Physical Address to Virtual Address and Make Shadow Register
 *
 * @return RET_OK(0)
 */
int DE_REG_InitPHY2VIRT(void)
{
    int ret = RET_OK;
	LX_DE_CH_MEM_T	*pFwMem = NULL;

#ifdef USE_QEMU_SYSTEM
	gpRealRegMIF0		= (MIF0_REG_T		*)OS_KMalloc(sizeof(MIF0_REG_T));
	gpRealRegMIF1		= (MIF1_REG_T		*)OS_KMalloc(sizeof(MIF1_REG_T));
	gpRealRegMIF2		= (MIF2_REG_T		*)OS_KMalloc(sizeof(MIF2_REG_T));
	gpRealRegMIF3		= (MIF3_REG_T		*)OS_KMalloc(sizeof(MIF3_REG_T));

	gpRealRegCHA_CVI	= (CHA_CVI_REG_T	*)OS_KMalloc(sizeof(CHA_CVI_REG_T));
	gpRealRegCHB_CVI	= (CHB_CVI_REG_T	*)OS_KMalloc(sizeof(CHB_CVI_REG_T));
	gpRealRegPRE3D		= (PRE3D_REG_T		*)OS_KMalloc(sizeof(PRE3D_REG_T));

	gpRealRegMSC		= (MSC_REG_T		*)OS_KMalloc(sizeof(MSC_REG_T));
	gpRealRegSSC		= (SSC_REG_T		*)OS_KMalloc(sizeof(SSC_REG_T));

	gpRealRegDE0A_CTRL	= (DE0A_CTRL_REG_T	*)OS_KMalloc(sizeof(DE0A_CTRL_REG_T));
	gpRealRegDE0B_CTRL	= (DE0B_CTRL_REG_T	*)OS_KMalloc(sizeof(DE0B_CTRL_REG_T));
	gpRealRegDE1A_CTRL	= (DE1A_CTRL_REG_T	*)OS_KMalloc(sizeof(DE1A_CTRL_REG_T));
	gpRealRegDE1B_CTRL	= (DE1B_CTRL_REG_T	*)OS_KMalloc(sizeof(DE1B_CTRL_REG_T));

	gpRealRegov_top		= (ov_top_REG_T		*)OS_KMalloc(sizeof(ov_top_REG_T));

#ifdef USE_PQL_REG_DEFINED_IN_DE
	gpRealRegPE0		= (PE0_REG_T		*)OS_KMalloc(sizeof(PE0_REG_T));
	gpRealRegPE1		= (PE1_REG_T		*)OS_KMalloc(sizeof(PE1_REG_T));
#endif
	gpRealRegSPP_MCU	= (SPP_MCU_REG_T	*)OS_KMalloc(sizeof(SPP_MCU_REG_T));
	gpRealRegeDMA		= (eDMA_REG_T		*)OS_KMalloc(sizeof(eDMA_REG_T));

	gpRealRegOIF		= (OIF_REG_T	    *)OS_KMalloc(sizeof(OIF_REG_T));
	//gpRealRegCTOP_CTRL	= (CTOP_CTRL_REG_T	*)OS_KMalloc(sizeof(CTOP_CTRL_REG_T));
#elif defined(USE_VIDEO_IOREMAP)
	gpRealRegMIF0		= (volatile MIF0_REG_T		*)ioremap(VIDEO_MIF0_REG_BASE,	   sizeof(MIF0_REG_T));
	gpRealRegMIF1		= (volatile MIF1_REG_T		*)ioremap(VIDEO_MIF1_REG_BASE,	   sizeof(MIF1_REG_T));
	gpRealRegMIF2		= (volatile MIF2_REG_T		*)ioremap(VIDEO_MIF2_REG_BASE,	   sizeof(MIF2_REG_T));
	gpRealRegMIF3		= (volatile MIF3_REG_T		*)ioremap(VIDEO_MIF3_REG_BASE,	   sizeof(MIF3_REG_T));

	gpRealRegCHA_CVI	= (volatile CHA_CVI_REG_T	*)ioremap(VIDEO_CVA_TOP_REG_BASE,  sizeof(CHA_CVI_REG_T));
	gpRealRegCHB_CVI	= (volatile CHB_CVI_REG_T	*)ioremap(VIDEO_CVB_TOP_REG_BASE,  sizeof(CHB_CVI_REG_T));
	gpRealRegPRE3D		= (volatile PRE3D_REG_T		*)ioremap(VIDEO_DLR_REG_BASE,	   sizeof(PRE3D_REG_T));

	gpRealRegMSC		= (volatile MSC_REG_T		*)ioremap(VIDEO_MSCR_REG_BASE,	   sizeof(MSC_REG_T));
	gpRealRegSSC		= (volatile SSC_REG_T		*)ioremap(VIDEO_SSCR_REG_BASE,	   sizeof(SSC_REG_T));

	gpRealRegDE0A_CTRL	= (volatile DE0A_CTRL_REG_T *)ioremap(VIDEO_DE0A_HIF_REG_BASE, sizeof(DE0A_CTRL_REG_T));
	gpRealRegDE0B_CTRL	= (volatile DE0B_CTRL_REG_T *)ioremap(VIDEO_DE0B_HIF_REG_BASE, sizeof(DE0B_CTRL_REG_T));
	gpRealRegDE1A_CTRL	= (volatile DE1A_CTRL_REG_T *)ioremap(VIDEO_DE1A_HIF_REG_BASE, sizeof(DE1A_CTRL_REG_T));
	gpRealRegDE1B_CTRL	= (volatile DE1B_CTRL_REG_T *)ioremap(VIDEO_DE1B_HIF_REG_BASE, sizeof(DE1B_CTRL_REG_T));

	gpRealRegov_top		= (volatile ov_top_REG_T	*)ioremap(VIDEO_OV_REG_BASE,	   sizeof(ov_top_REG_T));

#ifdef USE_PQL_REG_DEFINED_IN_DE
	gpRealRegPE0		= (volatile PE0_REG_T		*)ioremap(VIDEO_PE0_REG_BASE,	   sizeof(PE0_REG_T));
	gpRealRegPE1		= (volatile PE1_REG_T		*)ioremap(VIDEO_PE1_REG_BASE,	   sizeof(PE1_REG_T));
#endif
	gpRealRegSPP_MCU	= (volatile SPP_MCU_REG_T	*)ioremap(VIDEO_IPC_REG_BASE,	   sizeof(SPP_MCU_REG_T));
	gpRealRegeDMA		= (volatile eDMA_REG_T		*)ioremap(VIDEO_IPC_REG_BASE,	   sizeof(eDMA_REG_T));

	gpRealRegOIF		= (volatile OIF_REG_T 		*)ioremap(VIDEO_DVR_REG_BASE,	   sizeof(OIF_REG_T));
	//gpRealRegCTOP_CTRL	= (volatile CTOP_CTRL_REG_T *)ioremap(DTVSOC_TOP_CTRL,	       sizeof(CTOP_CTRL_REG_T));
#endif
	gpRegMIF0		= (MIF0_REG_T		*)OS_KMalloc(sizeof(MIF0_REG_T));
	gpRegMIF1		= (MIF1_REG_T		*)OS_KMalloc(sizeof(MIF1_REG_T));
	gpRegMIF2		= (MIF2_REG_T		*)OS_KMalloc(sizeof(MIF2_REG_T));
	gpRegMIF3		= (MIF3_REG_T		*)OS_KMalloc(sizeof(MIF3_REG_T));

	gpRegCHA_CVI	= (CHA_CVI_REG_T	*)OS_KMalloc(sizeof(CHA_CVI_REG_T));
	gpRegCHB_CVI	= (CHB_CVI_REG_T	*)OS_KMalloc(sizeof(CHB_CVI_REG_T));
#ifdef USE_KDRV_CODES_FOR_L8
	gpRegPRE3D		= (PRE3D_REG_T		*)OS_KMalloc(sizeof(PRE3D_REG_T));
#endif

	gpRegMSC		= (MSC_REG_T		*)OS_KMalloc(sizeof(MSC_REG_T));
	gpRegSSC		= (SSC_REG_T		*)OS_KMalloc(sizeof(SSC_REG_T));

	gpRegDE0A_CTRL	= (DE0A_CTRL_REG_T	*)OS_KMalloc(sizeof(DE0A_CTRL_REG_T));
	gpRegDE0B_CTRL	= (DE0B_CTRL_REG_T	*)OS_KMalloc(sizeof(DE0B_CTRL_REG_T));
	gpRegDE1A_CTRL	= (DE1A_CTRL_REG_T	*)OS_KMalloc(sizeof(DE1A_CTRL_REG_T));
	gpRegDE1B_CTRL	= (DE1B_CTRL_REG_T	*)OS_KMalloc(sizeof(DE1B_CTRL_REG_T));

	gpRegov_top		= (ov_top_REG_T		*)OS_KMalloc(sizeof(ov_top_REG_T));

#ifdef USE_KDRV_CODES_FOR_L8
#ifdef USE_PQL_REG_DEFINED_IN_DE
	gpRegPE0		= (PE0_REG_T		*)OS_KMalloc(sizeof(PE0_REG_T));
	gpRegPE1		= (PE1_REG_T		*)OS_KMalloc(sizeof(PE1_REG_T));
#endif
#endif
	gpRegSPP_MCU	= (SPP_MCU_REG_T	*)OS_KMalloc(sizeof(SPP_MCU_REG_T));
	gpRegeDMA		= (eDMA_REG_T		*)OS_KMalloc(sizeof(eDMA_REG_T));

	gpRegOIF		= (OIF_REG_T		*)OS_KMalloc(sizeof(OIF_REG_T));
	//gpRegCTOP_CTRL	= (CTOP_CTRL_REG_T	*)OS_KMalloc(sizeof(CTOP_CTRL_REG_T));

	//gpRealRegSilkLoad0[404/4] = 0xf0302;
	//gpRealRegSilkLoad1[404/4] = 0xf0302;
	//gpRealRegSilkLoad2[404/4] = 0xf0302;

    do {
        pFwMem = &gpDeMem->fw[2];
        CHECK_KNULL(pFwMem);
		spVideoIPCofCPU	= ioremap(pFwMem->fw_base, SIZE_OF_IPC_FOR_CPU * sizeof(UINT32));
        CHECK_KNULL(spVideoIPCofCPU);
    } while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Free memory alocated in Shadow Register
 *
 * @return RET_OK(0)
 */
int DE_REG_FreePHY2VIRT(void)
{
#ifdef USE_QEMU_SYSTEM
	OS_Free((void*)gpRealRegMIF0);
	OS_Free((void*)gpRealRegMIF1);
	OS_Free((void*)gpRealRegMIF2);
	OS_Free((void*)gpRealRegMIF3);

	OS_Free((void*)gpRealRegCHA_CVI);
	OS_Free((void*)gpRealRegCHB_CVI);
	OS_Free((void*)gpRealRegPRE3D);

	OS_Free((void*)gpRealRegMSC);
	OS_Free((void*)gpRealRegSSC);

	OS_Free((void*)gpRealRegDE0A_CTRL);
	OS_Free((void*)gpRealRegDE0B_CTRL);
	OS_Free((void*)gpRealRegDE1A_CTRL);
	OS_Free((void*)gpRealRegDE1B_CTRL);

	OS_Free((void*)gpRealRegov_top);

#ifdef USE_PQL_REG_DEFINED_IN_DE
	OS_Free((void*)gpRealRegPE0);
	OS_Free((void*)gpRealRegPE1);
#endif
	OS_Free((void*)gpRealRegSPP_MCU);
	OS_Free((void*)gpRealRegeDMA);

	OS_Free((void*)gpRealRegOIF);
	//OS_Free((void*)gpRealRegCTOP_CTRL);
#elif defined(USE_VIDEO_IOREMAP)
	iounmap((void*)gpRealRegMIF0);
	iounmap((void*)gpRealRegMIF1);
	iounmap((void*)gpRealRegMIF2);
	iounmap((void*)gpRealRegMIF3);

	iounmap((void*)gpRealRegCHA_CVI);
	iounmap((void*)gpRealRegCHB_CVI);
	iounmap((void*)gpRealRegPRE3D);

	iounmap((void*)gpRealRegMSC);
	iounmap((void*)gpRealRegSSC);

	iounmap((void*)gpRealRegDE0A_CTRL);
	iounmap((void*)gpRealRegDE0B_CTRL);
	iounmap((void*)gpRealRegDE1A_CTRL);
	iounmap((void*)gpRealRegDE1B_CTRL);

	iounmap((void*)gpRealRegov_top);

#ifdef USE_PQL_REG_DEFINED_IN_DE
	iounmap((void*)gpRealRegPE0);
	iounmap((void*)gpRealRegPE1);
#endif
	iounmap((void*)gpRealRegSPP_MCU);
	iounmap((void*)gpRealRegeDMA);

	iounmap((void*)gpRealRegOIF);
	//iounmap((void*)gpRealRegCTOP_CTRL);
#endif

	OS_Free((void*)gpRegMIF0);
	OS_Free((void*)gpRegMIF1);
	OS_Free((void*)gpRegMIF2);
	OS_Free((void*)gpRegMIF3);

	OS_Free((void*)gpRegCHA_CVI);
	OS_Free((void*)gpRegCHB_CVI);
#ifdef USE_KDRV_CODES_FOR_L8
	OS_Free((void*)gpRegPRE3D);
#endif

	OS_Free((void*)gpRegMSC);
	OS_Free((void*)gpRegSSC);

	OS_Free((void*)gpRegDE0A_CTRL);
	OS_Free((void*)gpRegDE0B_CTRL);
	OS_Free((void*)gpRegDE1A_CTRL);
	OS_Free((void*)gpRegDE1B_CTRL);

	OS_Free((void*)gpRegov_top);

#ifdef USE_KDRV_CODES_FOR_L8
#ifdef USE_PQL_REG_DEFINED_IN_DE
	OS_Free((void*)gpRegPE0);
	OS_Free((void*)gpRegPE1);
#endif
#endif
	OS_Free((void*)gpRegSPP_MCU);
	OS_Free((void*)gpRegeDMA);

	OS_Free((void*)gpRegOIF);
	//OS_Free((void*)gpRegCTOP_CTRL);

    if (spVideoIPCofCPU) iounmap((void*)spVideoIPCofCPU);

	return RET_OK;
}

 /**
 * @callgraph
 * @callergraph
 *
 * @brief Get Status of Interrupt which is one of either MCU or CPU
 *
 * @param ipcType [IN] one of either MCU and CPU
 * @param pStatus [OUT] status pointer of Interrupt
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_IPC_GetStatusReg(VIDEO_IPC_TYPE_T ipcType, UINT32 *pStatus)
{
	int ret = RET_OK;

	switch (ipcType) {
		case VIDEO_IPC_MCU :
		case VIDEO_DMA_MCU :
			*pStatus = REG_RD(&gpRealRegSPP_MCU->int_interrupt_status);
			break;
		case VIDEO_IPC_CPU :
		case VIDEO_DMA_CPU :
			*pStatus = REG_RD(&gpRealRegSPP_MCU->ext_interrupt_status);
			break;
		default :
			BREAK_WRONG(ipcType);
	}
	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief make interrupt be happen according to ipcType which is either of MCU or CPU
 *
 * @param ipcType [IN] one of either MCU and CPU
 * @param turnOn  [IN] maket Interrupt enable or clear
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_IPC_WakeUpReg(VIDEO_IPC_TYPE_T ipcType, BOOLEAN turnOn)
{
	int ret = RET_OK;

	turnOn &= 0x1;
	switch (ipcType) {
		case VIDEO_IPC_MCU :
			if (turnOn) gpRealRegSPP_MCU->int_interrupt_event.ipc_interrupt_event_108 = 1;
			else		gpRealRegSPP_MCU->int_interrupt_clear.ipc_interrupt_clear_108 = 1;
			break;
		case VIDEO_DMA_MCU :
			if (turnOn) gpRealRegSPP_MCU->int_interrupt_event.edma_interrupt_event_108 = 1;
			else		gpRealRegSPP_MCU->int_interrupt_clear.edma_interrupt_clear_108 = 1;
			break;
		case VIDEO_IPC_CPU :
			if (turnOn) gpRealRegSPP_MCU->ext_interrupt_event.ipc_interrupt_event_arm = 1;
			else		gpRealRegSPP_MCU->ext_interrupt_clear.ipc_interrupt_clear_arm = 1;
			break;
		case VIDEO_DMA_CPU :
			if (turnOn) gpRealRegSPP_MCU->ext_interrupt_event.ack_interrupt_event_arm = 1;
			else		gpRealRegSPP_MCU->ext_interrupt_clear.ack_interrupt_clear_arm = 1;
			break;
		default :
			BREAK_WRONG(ipcType);
	}
	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief make inerrupt status for watch dog either happen or clear
 *
 * @param turnOn [IN] determine to turn On or Off
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_WDG_WakeUpReg(BOOLEAN turnOn)
{
	if (turnOn) gpRealRegSPP_MCU->ext_interrupt_event.wdg_interrupt_event = 1;
	else		gpRealRegSPP_MCU->ext_interrupt_clear.wdg_interrupt_clear = 1;

	return RET_OK;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief make inerrupt status for DMA either happen or clear
 *
 * @param turnOn [IN] determine to turn On or Off
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_DMA_WakeUpReg(BOOLEAN turnOn)
{
	if (turnOn) gpRealRegSPP_MCU->ext_interrupt_event.ack_interrupt_event_arm = 1;
	else		gpRealRegSPP_MCU->ext_interrupt_clear.ack_interrupt_clear_arm = 1;

	return RET_OK;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Write Register
 *
 * @param addr [IN] accessing for register
 * @param value [IN] acccesing for register
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_REG_WD(UINT32 addr, UINT32 value)
{
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 currValue;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;

	do {
		recvAddr = addr;
		if		(recvAddr <= 0x2000) recvAddr += DTVSOC_DE_BASE;
		else if (recvAddr <= 0xffff) recvAddr += DTVSOC_VIRT_PL301_BASE;
		currAddr = recvAddr;
		currAddr >>= 2;
		currAddr <<= 2;

		if (currAddr == recvAddr) {
			REG_WD(recvAddr, value);
			break;
		}
		currValue = REG_RD(currAddr);
		nextValue = REG_RD((currAddr+4));

		dBit = (recvAddr - currAddr)<<3;
		wBit = (32 - dBit);

		currValue  = GET_BITS(currValue ,0	  ,dBit);
		currValue += GET_PVAL(value		,dBit ,wBit);

		nextValue  = GET_PVAL(nextValue ,dBit ,wBit);
		nextValue += GET_BITS(value		,0	  ,dBit);
		REG_WD(currAddr		,currValue);
		REG_WD((currAddr+4) ,nextValue);
	} while (0);

	return RET_OK;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Write Register
 *
 * @param addr [IN] accessing for register
 *
 * @return value of register
 */
UINT32 VIDEO_REG_RD(UINT32 addr)
{
	UINT32 value;
	UINT32 recvAddr;
	UINT32 currAddr;
	UINT32 nextValue;
	UINT32 dBit;
	UINT32 wBit;

	do {
		recvAddr = addr;
		if		(recvAddr <= 0x2000) recvAddr += DTVSOC_DE_BASE;
		else if (recvAddr <= 0xffff) recvAddr += DTVSOC_VIRT_PL301_BASE;

		currAddr = recvAddr;
		currAddr >>= 2;
		currAddr <<= 2;
		value = REG_RD(currAddr);
		if (currAddr == recvAddr) break;

		nextValue = REG_RD(currAddr+4);
		dBit = (recvAddr - currAddr)<<3;
		wBit = (32 - dBit);
		value  = GET_BITS(value, dBit, wBit);
		value += GET_PVAL(nextValue, wBit, dBit);
	} while (0);
	//ffprintk("Reg[0x%08x] = 0x%08x\n", addr, value);

	return value;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set Frame rate of Display
 *
 * @param fr_rate [IN] value of Frame rate of Display
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetFrRate(DE_DPLL_CLK_T dclk)
{
	int ret = RET_OK;
#ifdef USE_KDRV_CODES_FOR_L8
	DE_DPLL_DIV_T *pDisplayPll = NULL;
	int i;

	do {
		for (i=0;i<ARRAY_SIZE(sDisplayPll);i++) {
			if (sDisplayPll[i].clk != dclk) continue;
			pDisplayPll =  &sDisplayPll[i].div;
			break;
		}
		CHECK_KNULL(pDisplayPll);
		CTOP_CTRL_RdFL(ctr50);
		CTOP_CTRL_RdFL(ctr51);

		CTOP_CTRL_Wr01(ctr50, dpll_n,  pDisplayPll->dpllN);
		CTOP_CTRL_Wr01(ctr50, dpll_m,  pDisplayPll->dpllM);
		CTOP_CTRL_Wr01(ctr51, dpll_od, pDisplayPll->dpllOd);

		CTOP_CTRL_WrFL(ctr50);
		CTOP_CTRL_WrFL(ctr51);
	} while (0);
#endif

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief get Frame rate of Display
 *
 * @param pFrRate [OUT] value of Frame rate of Display
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_GetFrRate(LX_DE_FR_RATE_T *pstParams)
{
	int ret = RET_OK;

	pstParams->isForceFreeRun = FALSE;
	pstParams->fr_rate = 30;

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set background color (non-active region color).
 *
 * @param [IN] structure pointer to carry infomation about non-active region color
 *
 * @return RET_OK(0)
 */
int DE_REG_SetBgColor(LX_DE_COLOR_T *pBackColor)
{
	int ret = RET_OK;

	ov_top_RdFL(ov_ctrl2);

	pBackColor->r >>= 4;
	pBackColor->b >>= 4;
	ov_top_Wr01(ov_ctrl2, back_color_cr, pBackColor->r);
	ov_top_Wr01(ov_ctrl2, back_color_cb, pBackColor->b);
	ov_top_Wr01(ov_ctrl2, back_color_yy, pBackColor->g);

	ov_top_WrFL(ov_ctrl2);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Set blank color of selected window
 *
 * @param pWinBlank [IN] structure pointer to carry information about window Id, whether Turn On or Off and blank color
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetWinBlank(LX_DE_SET_WIN_BLANK_T *pWinBlank)
{
	int ret = RET_OK;

	do {
		ov_top_RdFL(ov_ctrl3);
		switch (pWinBlank->win_id) {
			case LX_DE_WIN_MAIN :
				ov_top_Wr01(ov_ctrl3, w0_dark_en, pWinBlank->bEnable);
				break;
			case LX_DE_WIN_SUB :
				ov_top_Wr01(ov_ctrl3, w1_dark_en, pWinBlank->bEnable);
				break;
			default :
				BREAK_WRONG(pWinBlank->win_id);
		}
		if (ret) break;
		ov_top_WrFL(ov_ctrl3);

		pWinBlank->win_color.r >>= 4;
		pWinBlank->win_color.b >>= 4;
		ov_top_RdFL(ov_ctrl2);
		ov_top_Wr01(ov_ctrl2, dark_color_cr, pWinBlank->win_color.r);
		ov_top_Wr01(ov_ctrl2, dark_color_cb, pWinBlank->win_color.b);
		ov_top_Wr01(ov_ctrl2, dark_color_yy, pWinBlank->win_color.g);
		ov_top_WrFL(ov_ctrl2);
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief grab video pixels of certain block size and position.
 *
 * @param stParams
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_GrabPixel(LX_DE_GRAB_PIXEL_T *pstParams)
{
	int ret = RET_OK;
	UINT8 *pFrameAddrYg8 = NULL;
	UINT8 *pFrameAddrCb8 = NULL;
    UINT8 *pFrameAddrCr8 = NULL;
	UINT8 *pFrameAddrYg2 = NULL;
	UINT8 *pFrameAddrCb2 = NULL;
    UINT8 *pFrameAddrCr2 = NULL;

	UINT8 *pGrabAddrYg8 = NULL;
	UINT8 *pGrabAddrCb8 = NULL;
	UINT8 *pGrabAddrCr8 = NULL;
	UINT8 *pGrabAddrYg2 = NULL;
	UINT8 *pGrabAddrCb2 = NULL;
	UINT8 *pGrabAddrCr2 = NULL;
	UINT32 frameSize;
	UINT32 grabSize;
	UINT32 grapArea;
	UINT32 grabX;
	UINT32 grabY;
	UINT32 grabW;
	UINT32 grabH;
	UINT32 rowAddrY;
	UINT32 rowAddrC;
	UINT32 frameW;
	UINT32 frameH;
	UINT32 pixelG8;
	UINT32 pixelB8;
	UINT32 pixelR8;
	UINT32 pixelG2;
	UINT32 pixelB2;
	UINT32 pixelR2;
	UINT8  pixelYg;
	UINT8  pixelCr;
	UINT8  pixelCb;
	UINT32 x;
	UINT32 y;
	UINT32 pos;
	UINT32 div;
	UINT32 rem;

	do {

		rowAddrY = pstParams->region.realPixelGrabW;
		rowAddrC = pstParams->region.realPixelGrabH;
		frameW = 2048;
		frameH = 1200;

		grabX = pstParams->region.pixelGrabX;
		grabY = pstParams->region.pixelGrabY;
		grabW = pstParams->region.pixelGrabW;
		grabH = pstParams->region.pixelGrabH;

		frameSize = frameW * frameH;
		grabSize  = grabW  * grabH;
        if (grabSize) break;
        grapArea = grabSize;
        if (grabSize < 4) grabSize = 4;

		pFrameAddrYg8 = ioremap(VIDEO_L8_ROW2PHY(GET_BITS(rowAddrY, 16, 16)), frameSize);
		pFrameAddrCb8 = ioremap(VIDEO_L8_ROW2PHY(GET_BITS(rowAddrC, 16, 16)), frameSize);
        pFrameAddrCr8 = ioremap(VIDEO_L8_ROW2PHY(GET_BITS(rowAddrC, 16, 16)), frameSize);
		pFrameAddrYg2 = ioremap(VIDEO_L8_ROW2PHY(GET_BITS(rowAddrY,  0, 16)), frameSize/4);
		pFrameAddrCb2 = ioremap(VIDEO_L8_ROW2PHY(GET_BITS(rowAddrC,  0, 16)), frameSize/4);
        pFrameAddrCr2 = ioremap(VIDEO_L8_ROW2PHY(GET_BITS(rowAddrC,  0, 16)), frameSize/4);
		CHECK_KNULL(pFrameAddrYg8);
		CHECK_KNULL(pFrameAddrCb8);
        CHECK_KNULL(pFrameAddrCr8);
		CHECK_KNULL(pFrameAddrYg2);
		CHECK_KNULL(pFrameAddrCb2);
        CHECK_KNULL(pFrameAddrCr2);

		pGrabAddrYg8 = OS_Malloc(grabSize);
		pGrabAddrCb8 = OS_Malloc(grabSize);
		pGrabAddrCr8 = OS_Malloc(grabSize);
		pGrabAddrYg2 = OS_Malloc(grabSize/4);
		pGrabAddrCb2 = OS_Malloc(grabSize/4);
		pGrabAddrCr2 = OS_Malloc(grabSize/4);
		CHECK_KNULL(pGrabAddrYg8);
		CHECK_KNULL(pGrabAddrCb8);
		CHECK_KNULL(pGrabAddrCr8);
		CHECK_KNULL(pGrabAddrYg2);
		CHECK_KNULL(pGrabAddrCb2);
		CHECK_KNULL(pGrabAddrCr2);

		for (y=0;y<grabH;y++) {
			memcpy(&pGrabAddrYg8[(grabW * y)], &pFrameAddrYg8[(frameW * (y+grabY) + grabX)	], grabW);
			memcpy(&pGrabAddrCb8[(grabW * y)], &pFrameAddrCb8[(frameW * (y+grabY) + grabX)	], grabW);
            memcpy(&pGrabAddrCr8[(grabW * y)], &pFrameAddrCr8[(frameW * (y+grabY) + grabX)	], grabW);
			memcpy(&pGrabAddrYg2[(grabW * y)], &pFrameAddrYg2[(frameW * (y+grabY) + grabX)/4], grabW/4);
			memcpy(&pGrabAddrCb2[(grabW * y)], &pFrameAddrCb2[(frameW * (y+grabY) + grabX)/4], grabW/4);
            memcpy(&pGrabAddrCr2[(grabW * y)], &pFrameAddrCr2[(frameW * (y+grabY) + grabX)/4], grabW/4);
		}

		pixelG8 = 0;
		pixelB8 = 0;
		pixelR8 = 0;
		pixelG2 = 0;
		pixelB2 = 0;
		pixelR2 = 0;
		do {
			for (y=0;y<grabH;y++) {
				for (x=0;x<pstParams->region.pixelGrabW;x++) {
					pixelG8 += pGrabAddrYg8[(grabW * y + x)];
					pixelB8 += pGrabAddrCb8[(grabW * y + x)];
					pixelR8 += pGrabAddrCr8[(grabW * y + x)];
				}
			}
			if (10 != pstParams->region.colorDepth) break;

			for (y=0;y<grabH;y++) {
				for (x=0;x<pstParams->region.pixelGrabW;x++) {
					pos = grabW * y + x;
					div = pos/4;
					rem = pos%4;
					pixelYg  = pGrabAddrYg2[div];
					pixelCb  = pGrabAddrCb2[div];
					pixelCr  = pGrabAddrCr2[div];
					pixelG2 += GET_BITS(pixelYg, rem*2, 2);
					pixelB2 += GET_BITS(pixelCb, rem*2, 2);
					pixelR2 += GET_BITS(pixelCr, rem*2, 2);
				}
			}
		} while(0);
		pixelG8 /= grapArea;
		pixelB8 /= grapArea;
		pixelR8 /= grapArea;
		pixelG2 /= grapArea;
		pixelB2 /= grapArea;
		pixelR2 /= grapArea;
		pixelG8 <<= 2;
		pixelB8 <<= 2;
		pixelR8 <<= 2;

		pstParams->color.pixelGrabY  = pixelG8 + pixelG2;
		pstParams->color.pixelGrabCb = pixelB8 + pixelB2;
		pstParams->color.pixelGrabCr = pixelR8 + pixelR2;
	} while (0);

	if(pFrameAddrYg8) iounmap(pFrameAddrYg8);
	if(pFrameAddrCb8) iounmap(pFrameAddrCb8);
    if(pFrameAddrCr8) iounmap(pFrameAddrCr8);
	if(pFrameAddrYg2) iounmap(pFrameAddrYg2);
	if(pFrameAddrCb2) iounmap(pFrameAddrCb2);
    if(pFrameAddrCr2) iounmap(pFrameAddrCr2);

	OS_Free(pGrabAddrYg8);
	OS_Free(pGrabAddrCb8);
	OS_Free(pGrabAddrCr8);
	OS_Free(pGrabAddrYg2);
	OS_Free(pGrabAddrCb2);
	OS_Free(pGrabAddrCr2);

	return ret;
}

BOOLEAN DE_REG_CheckIrq4Vsync(void)
{
	int ret;
	DE_FUNC_INTR  de_func_intr;
	DE1A_INTR_REG de1a_intr_reg;
	UINT32 videoIntrDe;
	BOOLEAN vsyncIrq;

	do {
		vsyncIrq = 0;
		ret = DE_REG_UpdateVideoIrqStatus(VIDEO_INTR_TYPE_FUNC, &videoIntrDe);
		if (ret) break;

		de_func_intr  = *(DE_FUNC_INTR	*)&videoIntrDe;
		if (!de_func_intr.de1a_intr_cpu) break;
		ret = DE_REG_UpdateVideoIrqStatus(VIDEO_INTR_TYPE_DE1A, &videoIntrDe);
		if (ret) break;

		de1a_intr_reg = *(DE1A_INTR_REG *)&videoIntrDe;
		if (!de1a_intr_reg.disp_intr_for_cpu) break;
		vsyncIrq = 1;
	} while (0);

	return vsyncIrq;
}

int DE_REG_UpdateVideoIrqStatus(VIDEO_INTR_TYPE_T intrType, UINT32 *pVideoIrqStatus)
{
	int ret = RET_OK;
	DE0A_INTR_REG de0a_intr_reg;
	DE0B_INTR_REG de0b_intr_reg;
	DE1A_INTR_REG de1a_intr_reg;
	DE1B_INTR_REG de1b_intr_reg;
	UINT32 intrReg;
	UINT32 intrMux;

	do {
		if (!pVideoIrqStatus) {
			ffprintk("pVideoIrqStatus is Null\n");
			ret = RET_ERROR;
			break;
		}
		switch (intrType) {
			case VIDEO_INTR_TYPE_DE0A :
				de0a_intr_reg = gpRealRegDE0A_CTRL->de0a_intr_reg;
				gpRealRegDE0A_CTRL->de0a_intr_reg = de0a_intr_reg;
				*pVideoIrqStatus = *(UINT32 *)&de0a_intr_reg;
				break;
			case VIDEO_INTR_TYPE_DE0B :
				de0b_intr_reg = gpRealRegDE0B_CTRL->de0b_intr_reg;
				gpRealRegDE0B_CTRL->de0b_intr_reg = de0b_intr_reg;
				*pVideoIrqStatus = *(UINT32 *)&de0b_intr_reg;
				break;
			case VIDEO_INTR_TYPE_DE1A :
				intrReg = *(UINT32 *)&gpRealRegDE1A_CTRL->de1a_intr_reg;
				intrMux = *(UINT32 *)&gpRealRegDE1A_CTRL->de1a_intr_mux;
#ifdef USE_XTENSA
				intrReg &= ~intrMux;
#else
				intrReg &=	intrMux;
#endif
				de1a_intr_reg = *(DE1A_INTR_REG *)&intrReg;
				gpRealRegDE1A_CTRL->de1a_intr_reg = de1a_intr_reg;
				*pVideoIrqStatus = *(UINT32 *)&de1a_intr_reg;
				break;
			case VIDEO_INTR_TYPE_DE1B :
				de1b_intr_reg = gpRealRegDE1B_CTRL->de1b_intr_reg;
				gpRealRegDE1B_CTRL->de1b_intr_reg = de1b_intr_reg;
				*pVideoIrqStatus = *(UINT32 *)&de1b_intr_reg;
				break;
			case VIDEO_INTR_TYPE_FUNC :
				*pVideoIrqStatus = *(UINT32 *)&gpRealRegDE0B_CTRL->de_func_intr;
				break;
			default :
				BREAK_WRONG(intrType);
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Set Information from which source is comming
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetInputSrc(LX_DE_SET_INPUT_T *pstParams)
{
#ifdef	USE_DE_SETTING_SDEC_CLK_FROM_MVI
	static LX_DE_INPUT_SRC_T inputSrc_Old = 0xff;
	UINT32 reg;

	do {
		if (LX_DE_WIN_MAIN != pstParams->win_id) break;
		if (inputSrc_Old == pstParams->inputSrc) break;
		inputSrc_Old = pstParams->inputSrc;
		switch (pstParams->inputSrc) {
			case LX_DE_SRC_MVI :
			case LX_DE_SRC_CPU :
				reg = 1;
				break;
			case LX_DE_SRC_CVI :
			default :
				reg = 0;
				break;
		}
#ifdef USE_KDRV_CODES_FOR_L8
		CTOP_CTRL_RdFL(ctr13);
		CTOP_CTRL_Wr01(ctr13, dco_sel, reg); //setting disp clock from SDEC
		CTOP_CTRL_WrFL(ctr13);
#endif
	} while (0);
#endif
	return RET_OK;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Set information which is comming from CVI port
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetCviSrcType(LX_DE_CVI_SRC_TYPE_T *pstParams)
{
	int ret = RET_OK;
	UINT32 cviSampling = 0;

	do {
		CHECK_KNULL(pstParams);
		switch (pstParams->sampling) {
			case LX_DE_CVI_NORMAL_SAMPLING :
				cviSampling = 0;
				break;
			case LX_DE_CVI_DOUBLE_SAMPLING :
				cviSampling = 1;
				break;
			case LX_DE_CVI_QUAD_SAMPLING :
				cviSampling = 2;
				break;
			default :
				BREAK_WRONG(pstParams->sampling);
		}
		if (ret) break;

#ifdef USE_KDRV_CODES_FOR_L8
		CTOP_CTRL_RdFL(ctr13);
#endif
		switch (pstParams->cvi_channel) {
			case LX_DE_CVI_CH_A :
#ifdef USE_KDRV_CODES_FOR_L8
				CTOP_CTRL_Wr01(ctr13, chaclk_sel, cviSampling);
#endif

				CHA_CVI_RdFL(cha_dig_filt_ctrl3);
				CHA_CVI_RdFL(cha_top_ctrl);

//				CHA_CVI_WfCM(cha_dig_filt_ctrl3, fir_en,			cviSampling, 0x1);	// move to DE_PRM_LoadCvmFirCoef
				CHA_CVI_WfCM(cha_dig_filt_ctrl3, fir_sample_en,		cviSampling, 0x1);
// fir_sample_c 는 cviSampling과 관련 없음.
//				CHA_CVI_WfCM(cha_dig_filt_ctrl3, fir_sample_c,		cviSampling, 0x1);  // move to DE_REG_SetCviCsampleMode
				CHA_CVI_Wr01(cha_dig_filt_ctrl3, fir_sampling_rate, cviSampling);

				switch (pstParams->yc_delay)
				{
					case LX_DE_CVI_YC_DELAY_ATV :
						{
							//110623 : by kim.min
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_r, 0x0);
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_b, 0x3);
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_g, 0x0);
							break;
						}

					case LX_DE_CVI_YC_DELAY_OTHERS :
						{
							//110623 : by kim.min
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_r, 0x0);
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_b, 0x3);
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_g, 0x0);
							break;
						}

					case LX_DE_CVI_YC_DELAY_BYPASS :
					default :
						{
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_r, 0x0);
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_b, 0x0);
							CHA_CVI_Wr01(cha_top_ctrl, yc_delay_mode_g, 0x0);
							break;
						}
				}

				CHA_CVI_WrFL(cha_dig_filt_ctrl3);
				CHA_CVI_WrFL(cha_top_ctrl);

				break;
			case LX_DE_CVI_CH_B :
#ifdef USE_KDRV_CODES_FOR_L8
				CTOP_CTRL_Wr01(ctr13, chbclk_sel, cviSampling);
#endif
				CHB_CVI_RdFL(chb_dig_filt_ctrl3);
				CHB_CVI_RdFL(chb_top_ctrl);

//				CHB_CVI_WfCM(chb_dig_filt_ctrl3, fir_en,			cviSampling, 0x1);	// move to DE_PRM_LoadCvsFirCoef
				CHB_CVI_WfCM(chb_dig_filt_ctrl3, fir_sample_en,		cviSampling, 0x1);
// fir_sample_c 는 cviSampling과 관련 없음.
//				CHB_CVI_WfCM(chb_dig_filt_ctrl3, fir_sample_c,		cviSampling, 0x1);  // move to DE_REG_SetCviCsampleMode
				CHB_CVI_Wr01(chb_dig_filt_ctrl3, fir_sampling_rate, cviSampling);

                switch (pstParams->yc_delay)
				{
					case LX_DE_CVI_YC_DELAY_ATV :
						{
							//110623 : by kim.min
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_r, 0x0);
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_b, 0x3);
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_g, 0x0);
							break;
						}

					case LX_DE_CVI_YC_DELAY_OTHERS :
						{
							//110623 : by kim.min
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_r, 0x0);
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_b, 0x3);
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_g, 0x0);
							break;
						}

					case LX_DE_CVI_YC_DELAY_BYPASS :
					default :
						{
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_r, 0x0);
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_b, 0x0);
							CHB_CVI_Wr01(chb_top_ctrl, yc_delay_mode_g, 0x0);
							break;
						}
                }

				CHB_CVI_WrFL(chb_dig_filt_ctrl3);
				CHB_CVI_WrFL(chb_top_ctrl);
				break;
			default :
				BREAK_WRONG(pstParams->cvi_channel);
		}
		if (ret) break;
#ifdef USE_KDRV_CODES_FOR_L8
		CTOP_CTRL_WrFL(ctr13);
#endif
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set Frame rate of Display
 *
 * @param fr_rate [IN] value of Frame rate of Display
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_Init(LX_DE_PANEL_TYPE_T *pstParams)
{
	g_b_InitTPG = FALSE;
	g_patchEn   = 0;
    return DE_REG_OIF_InitLvdsIfCtrl();
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief get color space conversion matrix and offset for each external source information.
 *
 * @param [IN] structure pointer to carry infomation about non-active region color
 *
 * @return RET_OK(0)
 */
int DE_REG_GetCviCsc(LX_DE_CVI_CSC_T *pstParams)
{
	int ret = RET_OK;
	DE_PARAM_TYPE_T tableId = DE_CVM_CSC_COEF;
	UINT32 *cscTable = NULL;
	UINT32 matrixArraySize, offsetArraySize, tableArraySize;
	UINT32 inx = 0;
	int i;

	do {
		CHECK_KNULL(pstParams);
		matrixArraySize = ARRAY_SIZE(pstParams->CSC_matrix);
		offsetArraySize = ARRAY_SIZE(pstParams->CSC_offset);
		tableArraySize  = matrixArraySize + offsetArraySize;
		cscTable = OS_Malloc(sizeof(UINT32)*tableArraySize);
		CHECK_KNULL(cscTable);

		memset(cscTable, 0, (sizeof(UINT32)*tableArraySize));

		switch (pstParams->cvi_channel) {
			case LX_DE_CVI_CH_A :
				tableId = DE_CVM_CSC_COEF;
				break;
			case LX_DE_CVI_CH_B :
				tableId = DE_CVS_CSC_COEF;
				break;
			default :
				BREAK_WRONG(pstParams->cvi_channel);
		}
		ret = DE_PRM_LoadTable(tableId, DE_PARAM_READ, pstParams->isEnable, cscTable, tableArraySize);
		if (ret) break;

		for (i=0;i<matrixArraySize;i++) pstParams->CSC_matrix[i] = cscTable[inx++];
		for (i=0;i<offsetArraySize;i++) pstParams->CSC_offset[i] = cscTable[inx++];
	} while (0);
	OS_Free(cscTable);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief send color space conversion matrix and offset for each external source information.
 *
 * @param [IN] structure pointer to carry infomation about non-active region color
 *
 * @return RET_OK(0)
 */
int DE_REG_SetCviCsc(LX_DE_CVI_CSC_T *pstParams)
{
	int ret = RET_OK;
	DE_PARAM_TYPE_T tableId;
	UINT32 cscTable[ARRAY_SIZE(pstParams->CSC_matrix) + ARRAY_SIZE(pstParams->CSC_offset)];
	UINT32 inx = 0;
	int i;

	do {
		CHECK_KNULL(pstParams);
		switch (pstParams->cvi_channel) {
			case LX_DE_CVI_CH_A :
				tableId = DE_CVM_CSC_COEF;
				break;
			case LX_DE_CVI_CH_B :
				tableId = DE_CVS_CSC_COEF;
				break;
			default :
				BREAK_WRONG(pstParams->cvi_channel);
		}
		if (ret) break;
		for (i=0;i<ARRAY_SIZE(pstParams->CSC_matrix);i++) cscTable[inx++] = pstParams->CSC_matrix[i];
		for (i=0;i<ARRAY_SIZE(pstParams->CSC_offset);i++) cscTable[inx++] = pstParams->CSC_offset[i];

		ret = DE_PRM_LoadTable(tableId, DE_PARAM_WRITE, pstParams->isEnable, cscTable, ARRAY_SIZE(cscTable));
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief get color space conversion matrix for post processing block
 *
 * @param [IN] structure pointer to carry infomation about non-active region color
 *
 * @return RET_OK(0)
 */
int DE_REG_GetPostCsc(LX_DE_POST_CSC_T *pstParams)
{
	int ret = RET_OK;
	UINT32 *cscTable = NULL;
	UINT32 matrixArraySize, offsetArraySize, tableArraySize;
	UINT32 inx = 0;
	int i;

	do {
		CHECK_KNULL(pstParams);
		matrixArraySize = ARRAY_SIZE(pstParams->CSC_matrix);
		offsetArraySize = ARRAY_SIZE(pstParams->CSC_offset);
		tableArraySize  = matrixArraySize + offsetArraySize;
		cscTable = OS_Malloc(sizeof(UINT32)*tableArraySize);
		CHECK_KNULL(cscTable);

		memset(cscTable, 0, (sizeof(UINT32)*tableArraySize));

		ret = DE_PRM_LoadTable(DE_POST_CSC_COEF, DE_PARAM_READ, 0, cscTable, tableArraySize);
		if (ret) break;

		for (i=0;i<matrixArraySize;i++) pstParams->CSC_matrix[i] = cscTable[inx++];
		for (i=0;i<offsetArraySize;i++) pstParams->CSC_offset[i] = cscTable[inx++];
	} while (0);
	OS_Free(cscTable);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief send color space conversion matrix for post processing block
 *
 * @param [IN] structure pointer to carry infomation about non-active region color
 *
 * @return RET_OK(0)
 */
int DE_REG_SetPostCsc(LX_DE_POST_CSC_T *pstParams)
{
	int ret = RET_OK;
	UINT32 cscTable[ARRAY_SIZE(pstParams->CSC_matrix) + ARRAY_SIZE(pstParams->CSC_offset)];
	UINT32 inx = 0;
	int i;

	do {
		CHECK_KNULL(pstParams);
		for (i=0;i<ARRAY_SIZE(pstParams->CSC_matrix);i++) cscTable[inx++] = pstParams->CSC_matrix[i];
		for (i=0;i<ARRAY_SIZE(pstParams->CSC_offset);i++) cscTable[inx++] = pstParams->CSC_offset[i];

		ret = DE_PRM_LoadTable(DE_POST_CSC_COEF, DE_PARAM_WRITE, 0, cscTable, ARRAY_SIZE(cscTable));
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief get MVI source colorimetry information.
 *
 * @param [OUT] structure pointer to carry infomation about mvi colorimetry
 *
 * @return RET_OK(0)
 */
int DE_REG_GetMviColorimetry(LX_DE_MVI_COLORIMETRY_T *pstParams)
{
	int ret = RET_OK;

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set LVDS spread spectrum for 60Hz mode (PDP).
 *
 * @param [OUT] structure pointer to carry information about spread spectrum period, duty, enable.
 *
 * @return RET_OK(0)
 */
int DE_REG_SetSpread(LX_DE_SET_SPREAD_T *pstParams)
{
#ifdef USE_KDRV_CODES_FOR_L8
	UINT16 msex[4][8] = {
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 239, 170, 132, 91, 79, 70, 62},
		{0, 59, 42, 32, 22, 19, 17, 15},
		{0, 33, 23, 18, 12, 10, 9, 8}};
	UINT8 udex[8] = {0, 2, 3, 4, 6, 7, 8, 9};
	UINT8 sr[8]   = {0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
	UINT8 ms[8]   = {0, 1, 2, 3, 4, 5, 6, 7};
#endif
	int idxPeriod;
	int idxPercent;
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);

		idxPercent = pstParams->percent;

		if (idxPercent > 7)
			idxPercent = 0;

/*  	if (pstParams->dutyratio >= 200)
			idxPercent = 7;
		else if (pstParams->dutyratio >= 175)
			idxPercent = 6;
		else if (pstParams->dutyratio >= 150)
			idxPercent = 5;
		else if (pstParams->dutyratio >= 125)
			idxPercent = 4;
		else if (pstParams->dutyratio >= 100)
			idxPercent = 3;
		else if (pstParams->dutyratio >= 75)
			idxPercent = 2;
		else if (pstParams->dutyratio >= 50)
			idxPercent = 1;
		else
			idxPercent = 0; */

		if (pstParams->percent >= 3)
			idxPeriod = 3;
		else if (pstParams->percent >= 1)
			idxPeriod = 1;
		else
			idxPeriod = 0;

		switch (pstParams->type)
		{
			case LX_DE_SPREAD_SPECTRUM_MAIN :
				{
					if (!pstParams->bEnable)
					{
						idxPeriod  = 0;
						idxPercent = 0;
					}
#ifdef USE_KDRV_CODES_FOR_L8
					CTOP_CTRL_RdFL(ctr44);
					CTOP_CTRL_RdFL(ctr45);
					CTOP_CTRL_Wr01(ctr44, dr3p_cih, 1);
					CTOP_CTRL_Wr01(ctr44, dr3p_lf, 1);
					CTOP_CTRL_Wr01(ctr44, dr3p_msex, msex[idxPeriod][idxPercent]);
					CTOP_CTRL_Wr01(ctr45, dr3p_udex, udex[idxPercent]);
					CTOP_CTRL_WrFL(ctr45);
					CTOP_CTRL_WrFL(ctr44);
#endif
					break;
				}

			case LX_DE_SPREAD_SPECTRUM_FRC :
				{
					if (!pstParams->bEnable)
					{
						idxPeriod  = 0;
						idxPercent = 0;
					}

#ifdef USE_KDRV_CODES_FOR_L8
					CTOP_CTRL_RdFL(ctr46);
					CTOP_CTRL_RdFL(ctr47);
					CTOP_CTRL_Wr01(ctr46, dr3p_cih, 1);
					CTOP_CTRL_Wr01(ctr46, dr3p_lf, 1);
					CTOP_CTRL_Wr01(ctr46, dr3p_msex, msex[idxPeriod][idxPercent]);
					CTOP_CTRL_Wr01(ctr47, dr3p_udex, udex[idxPercent]);
					CTOP_CTRL_WrFL(ctr47);
					CTOP_CTRL_WrFL(ctr46);
#endif
					break;
				}
			case LX_DE_SPREAD_SPECTRUM_CPU :
				{
#ifdef USE_KDRV_CODES_FOR_L8
					CTOP_CTRL_RdFL(ctr48);
					CTOP_CTRL_RdFL(ctr49);
					CTOP_CTRL_Wr01(ctr48, dr3p_cih, 1);
					CTOP_CTRL_Wr01(ctr48, dr3p_lf, 1);
					CTOP_CTRL_Wr01(ctr48, dr3p_msex, msex[idxPeriod][idxPercent]);
					CTOP_CTRL_Wr01(ctr49, dr3p_udex, udex[idxPercent]);
					CTOP_CTRL_WrFL(ctr49);
					CTOP_CTRL_WrFL(ctr48);
#endif
					break;
				}

			case LX_DE_SPREAD_SPECTRUM_LVDS :
				{

#ifdef USE_KDRV_CODES_FOR_L8
					idxPeriod = pstParams->period;

					if (idxPeriod > 3)
						idxPeriod = 0;

					CTOP_CTRL_RdFL(ctr54);
					CTOP_CTRL_Wr01(ctr54, disp_sr, sr[idxPercent]);
					CTOP_CTRL_Wr01(ctr54, disp_ms, ms[idxPeriod]);

					if (pstParams->bEnable)
						CTOP_CTRL_Wr01(ctr54, disp_pdb, 0); // disp_bypass로 제어하는 것이 맞으나 disp_bypass로 동작시킬 경우 disp_pdb가 동작하는 것으로 Spread Spectrum이 정상 동작하지 않을 경우 해당 PIN을 다시한번 확인해야 할 것 같습니다.
					else
						CTOP_CTRL_Wr01(ctr54, disp_pdb, 1);

					CTOP_CTRL_WrFL(ctr54);

					break;
#endif
				}

			default :
				ret = RET_ERROR;
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set deinterlace on/off control
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetDeinterlace(BOOLEAN bEnable)
{
	return RET_OK;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set de VCS parameter.
 *
 * @param arg [OUT] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetVcs(LX_DE_VCS_IPC_T *pstParams)
{
    int ret = RET_OK;

    do {
        CHECK_KNULL(pstParams);
        CHECK_KNULL(spVideoIPCofCPU);
        if (pstParams->inx >= SIZE_OF_IPC_FOR_CPU ) break;
        spVideoIPCofCPU[pstParams->inx] = pstParams->data;
    } while (0);

    return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Initialize OIF module.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_Init(LX_DE_OIF_INFO_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		ret = DE_REG_OIF_SetVesaJeida(&pstParams->eVesaJeida);
		if (ret) break;
		ret = DE_REG_OIF_SetPanelIf(&pstParams->eLvdsMini);
		if (ret) break;
		ret = DE_REG_OIF_SetPixel(&pstParams->ePixel);
		if (ret) break;
		ret = DE_REG_OIF_SetBitDepth(&pstParams->eBitMode);
		if (ret) break;
		ret = DE_REG_OIF_SelectOutPath(&pstParams->eOutVideo);
		if (ret) break;
		ret = DE_REG_OIF_SelectLvdsPath(&pstParams->eOutLVDS);
		if (ret) break;
		ret = DE_REG_OIF_SelectOutCh(&pstParams->eOutCH);
		if (ret) break;
		ret = DE_REG_OIF_SelectLedPath(&pstParams->eLedIn);
		if (ret) break;
		ret = DE_REG_OIF_SelectPwmPath(&pstParams->ePWM);
		if (ret) break;
		ret = DE_REG_OIF_SelectLrPath(&pstParams->eLR);
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Get Informatation of OIF module.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_GetInfo(LX_DE_OIF_INFO_T *pstParams)
{
	int ret = RET_OK;
	UINT32 readVal;

	do {
		CHECK_KNULL(pstParams);
		OIF_RdFL(lvds_if_control);
		OIF_Rd01(lvds_if_control, lvds_lsb_first, readVal);
		pstParams->eVesaJeida = (1 == readVal)?LX_DE_OIF_VESA:LX_DE_OIF_JEIDA; // toggle polarity

#ifdef USE_KDRV_CODES_FOR_L8
		CTOP_CTRL_RdFL(ctr32);
		CTOP_CTRL_Rd01(ctr32, mini_en, readVal);
#endif
		pstParams->eLvdsMini = (0 == readVal)?LX_DE_OIF_PANEL_LVDS:LX_DE_OIF_PANEL_MINI_LVDS;

		OIF_RdFL(sync_de_control);
		OIF_Rd01(sync_de_control, out_sdq, readVal);
		switch (readVal) {
			case 0x0 :
				pstParams->ePixel = LX_OIF_PIXEL_SINGLE;
				break;
			case 0x1 :
				pstParams->ePixel = LX_OIF_PIXEL_DUAL;
				break;
			case 0x2 :
				pstParams->ePixel = LX_OIF_PIXEL_QUAD;
				break;
			default :
				BREAK_WRONG(readVal);
		}
		if (ret) break;

		OIF_RdFL(lvds_if_control);
		OIF_Rd01(lvds_if_control, lvds_8bit_mode, readVal);
		pstParams->eBitMode = (1 == readVal)?LX_DE_OIF_8BIT:LX_DE_OIF_10BIT;	// toggle polarity

		OIF_RdFL(out_video_info);
		OIF_Rd01(out_video_info, out_sel, readVal);
		switch (readVal) {
			case LX_OIF_OUTPUT_DE :
			case LX_OIF_OUTPUT_FRC :
			case LX_OIF_OUTPUT_LED :
				pstParams->eOutVideo = readVal;
				break;
			default :
				BREAK_WRONG(readVal);
		}
		if (ret) break;

		OIF_RdFL(lvds_if_control);
		OIF_Rd01(lvds_if_control, lvds_out_sel, readVal);
		pstParams->eOutLVDS = (0 == readVal)?LX_OIF_LVDS_NOT_ODC:LX_OIF_LVDS_ODC;

		OIF_RdFL(out_video_info);
		OIF_Rd01(out_video_info, out_ch_num, readVal);
		switch (readVal) {
			case LX_OIF_1CH :
			case LX_OIF_2CH :
			case LX_OIF_4CH :
				pstParams->eOutCH = readVal;
				break;
			default :
				BREAK_WRONG(readVal);
		}
		if (ret) break;

		OIF_RdFL(led_in_control);
		OIF_Rd01(led_in_control, led_in_control, readVal);
		pstParams->eLedIn = (1 == readVal)?LX_OIF_LED_IN_DE:LX_OIF_LED_IN_FRC;	// toggle polarity

		OIF_RdFL(pwm_control);
		OIF_Rd01(pwm_control, pwm_sel, readVal);
		pstParams->ePWM = (0 == readVal)?LX_OIF_PWM_DE:LX_OIF_PWM_FRC;

		OIF_RdFL(lr_if_control);
		OIF_Rd01(lr_if_control, lr_if_control, readVal);
		pstParams->eLR = (0 == readVal)?LX_OIF_LR_DE:LX_OIF_LR_FRC;

	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief select output chanel number 1CH, 2CH, 4CH according to resolution and frame rate
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SelectOutCh(LX_DE_OIF_OUTPUT_CH_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_OIF_1CH :
			case LX_OIF_2CH :
			case LX_OIF_4CH :
				OIF_RdFL(out_video_info);
				OIF_Wr01(out_video_info, out_ch_num, *pstParams);
				OIF_WrFL(out_video_info);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Set LVDS OUTPUT Enable.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetLvdsOutput(BOOLEAN *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
//		OIF_RdFL(lvds_en);
//		OIF_WfCM(lvds_en, lvds_en, *pstParams, GET_BITS(0xFFFFFFFF, 0, 24));
//		OIF_WrFL(lvds_en);
#ifdef USE_KDRV_CODES_FOR_L8
		CTOP_CTRL_RdFL(ctr32);
		CTOP_CTRL_WfCM(ctr32, en, *pstParams, GET_BITS(0xFFFFFFFF, 0, 24));
		CTOP_CTRL_WrFL(ctr32);
#endif
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Set LVDS Power Enable.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetLvdsPower(BOOLEAN *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
#ifdef USE_KDRV_CODES_FOR_L8
		CTOP_CTRL_RdFL(ctr32);
		CTOP_CTRL_WfCM(ctr32, pdb, (TRUE == *pstParams), 1);
		CTOP_CTRL_WrFL(ctr32);
#endif
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Set LVDS to black.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetDarkScreen(BOOLEAN *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);

		if( TRUE == *pstParams )
		{
			OIF_Wr01(data_color, data_color_b, 0x0);
			OIF_Wr01(data_color, data_color_g, 0x0);
			OIF_Wr01(data_color, data_color_r, 0x0);
			OIF_WrFL(data_color);

			OIF_Wr01(sync_color, sync_color_b, 0x0);
			OIF_Wr01(sync_color, sync_color_g, 0x0);
			OIF_Wr01(sync_color, sync_color_r, 0x0);
			OIF_WrFL(sync_color);

			OIF_Wr01(data_color_from, b_data_from, 0x3);
			OIF_Wr01(data_color_from, g_data_from, 0x3);
			OIF_Wr01(data_color_from, r_data_from, 0x3);
			OIF_WrFL(data_color_from);
		}
		else
		{
			OIF_Wr01(data_color_from, b_data_from, 0x2);
			OIF_Wr01(data_color_from, g_data_from, 0x1);
			OIF_Wr01(data_color_from, r_data_from, 0x0);
			OIF_WrFL(data_color_from);
		}

	} while (0);


	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Select LVDS type(VESA/JEIDA).
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetVesaJeida(LX_DE_OIF_STD_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_DE_OIF_VESA :
			case LX_DE_OIF_JEIDA :
				OIF_RdFL(lvds_if_control);
				OIF_WfCM(lvds_if_control, lvds_lsb_first, (LX_DE_OIF_VESA == *pstParams), 1);
				OIF_WrFL(lvds_if_control);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief select LVDS inteface type(LVDS/mini-LVDS).
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetPanelIf(LX_DE_OIF_PANEL_IF_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
#ifdef USE_KDRV_CODES_FOR_L8
		switch (*pstParams) {
			case LX_DE_OIF_PANEL_LVDS :
			case LX_DE_OIF_PANEL_MINI_LVDS :
				CTOP_CTRL_RdFL(ctr32);
				CTOP_CTRL_WfCM(ctr32, mini_en, (LX_DE_OIF_PANEL_MINI_LVDS == *pstParams), 1);
				CTOP_CTRL_WrFL(ctr32);
				CTOP_CTRL_RdFL(ctr32);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
#endif
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set LVDS pixel type( single/dual/quad).
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetPixel(LX_DE_OIF_PIXEL_T *pstParams)
{
	int ret = RET_OK;
	int	oifPixel = -1;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_OIF_PIXEL_SINGLE :
			case LX_OIF_PIXEL_SDQ :
				oifPixel = 0x0;
				break;
			case LX_OIF_PIXEL_DUAL :
				oifPixel = 0x1;
				break;
			case LX_OIF_PIXEL_QUAD :
				oifPixel = 0x2;
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
		if (ret) break;

		OIF_RdFL(sync_de_control);
		OIF_Wr01(sync_de_control, out_sdq, oifPixel);
		OIF_WrFL(sync_de_control);
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set LVDS bit resolution(10bit/8bit).
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetBitDepth(LX_DE_OIF_BIT_DEPTH_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_DE_OIF_8BIT :
			case LX_DE_OIF_10BIT :
				OIF_RdFL(lvds_if_control);
				OIF_WfCM(lvds_if_control, lvds_8bit_mode, (LX_DE_OIF_8BIT == *pstParams), 1);
				OIF_WrFL(lvds_if_control);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set LVDS signal path from DE/FRC/LED/(ODC removed)
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SelectOutPath(LX_DE_OIF_OUTPUT_PATH_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_OIF_OUTPUT_DE :
			case LX_OIF_OUTPUT_FRC :
			case LX_OIF_OUTPUT_LED :
				OIF_RdFL(out_video_info);
				OIF_Wr01(out_video_info, out_sel, *pstParams);
				OIF_WrFL(out_video_info);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set LVDS signal path from DE/FRC/LED/ODC
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SelectLvdsPath(LX_DE_OIF_LVDS_PATH_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_OIF_LVDS_NOT_ODC :
			case LX_OIF_LVDS_ODC :
				OIF_RdFL(lvds_if_control);
				if (lx_chip_rev( ) >= LX_CHIP_REV(L8,B0) )
				{
					OIF_Wr01(lvds_if_control, lvds_out_sel, 0x0 );
				}
				else
				{
					OIF_WfCM(lvds_if_control, lvds_out_sel, (LX_OIF_LVDS_ODC == *pstParams), 1);
				}
				OIF_WrFL(lvds_if_control);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief select LED input path from DE/FRC.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SelectLedPath(LX_DE_OIF_LED_IN_PATH_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_OIF_LED_IN_DE :
			case LX_OIF_LED_IN_FRC :
				OIF_RdFL(led_in_control);
				OIF_WfCM(led_in_control, led_in_control, (LX_OIF_LED_IN_DE == *pstParams), 1);
				OIF_WrFL(led_in_control);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief select PWM signal path from DE/FRC.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SelectPwmPath(LX_DE_OIF_PWM_PATH_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_OIF_PWM_DE :
			case LX_OIF_PWM_FRC :
				OIF_RdFL(pwm_control);
				OIF_WfCM(pwm_control, pwm_sel, (LX_OIF_PWM_FRC == *pstParams), 1);
				OIF_WrFL(pwm_control);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while(0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief select LR signal path from DE/FRC.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SelectLrPath(LX_DE_OIF_LR_PATH_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);
		switch (*pstParams) {
			case LX_OIF_LR_DE :
			case LX_OIF_LR_FRC :
				OIF_RdFL(lr_if_control);
				OIF_WfCM(lr_if_control, lr_if_control, (LX_OIF_LR_FRC == *pstParams), 1);
				OIF_WrFL(lr_if_control);
				break;
			default :
				BREAK_WRONG(*pstParams);
		}
	} while(0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Control LVDS IF
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_GetLvdsIfCtrl(LX_DE_OIF_LVDS_IF_CTRL_T *pstParams)
{
	int ret = RET_ERROR;

	do {
		CHECK_KNULL(pstParams);
		OIF_RdFL(lvds_if_control);
		OIF_Rd01(lvds_if_control, lvds_empty_data, pstParams->emptyData);
		OIF_Rd01(lvds_if_control, lvds_8bit_mode,  pstParams->bitMode);
		OIF_Rd01(lvds_if_control, lvds_lsb_first,  pstParams->lsbfirst);
		OIF_Rd01(lvds_if_control, lvds_sync_order, pstParams->syncOrder);
		OIF_Rd01(lvds_if_control, lvds_out_sel,    pstParams->outSel);
		OIF_Rd01(lvds_if_control, lvds_ch3_from,   pstParams->ch3LVDS);
		OIF_Rd01(lvds_if_control, lvds_ch2_from,   pstParams->ch2LVDS);
		OIF_Rd01(lvds_if_control, lvds_ch1_from,   pstParams->ch1LVDS);
		OIF_Rd01(lvds_if_control, lvds_ch0_from,   pstParams->ch0LVDS);
		ret = RET_OK;
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Control LVDS TX following TCON enable.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_GetLvdsTxCtrl(LX_DE_OIF_LVDS_TX_CTRL_T *pstParams)
{
	int ret = RET_ERROR;
#ifdef USE_KDRV_CODES_FOR_L8
	UINT32 enpemp_h;
	UINT32 enpemp_l;

	do {
		CHECK_KNULL(pstParams);
		CTOP_CTRL_RdFL(ctr32);
		CTOP_CTRL_RdFL(ctr33);
		CTOP_CTRL_RdFL(ctr34);
		CTOP_CTRL_Rd01(ctr32, pdb,			pstParams->pdb);
		CTOP_CTRL_Rd01(ctr32, rf,			pstParams->rf);
		CTOP_CTRL_Rd01(ctr33, rs,			pstParams->rs);
		CTOP_CTRL_Rd01(ctr33, enpemp_h,		enpemp_h);
		CTOP_CTRL_Rd01(ctr34, enpemp_l,		enpemp_l);
		CTOP_CTRL_Rd01(ctr32, mini_en,		pstParams->miniLVDS);
		if ( lx_chip_rev( ) >= LX_CHIP_REV(L8,B0) )
		{
			// field removed
		}
		else
		{
			CTOP_CTRL_A0_Rd01(ctr32, mini_flip_en, pstParams->miniFlip);
		}
		CTOP_CTRL_Rd01(ctr32, rmlvds_en,	pstParams->swingLVDS);
		CTOP_CTRL_Rd01(ctr34, ovdrv,		pstParams->ovDrv);
		CTOP_CTRL_Rd01(ctr32, en,			pstParams->lvdsEn);
		pstParams->empCtrl = ((enpemp_h&0xffff) << 16) | (enpemp_l&0xffff);
		ret = RET_OK;
	} while (0);
#endif

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Control LVDS IF
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetLvdsIfCtrl(LX_DE_OIF_LVDS_IF_CTRL_T *pstParams)
{
	int ret = RET_ERROR;

	do {
		CHECK_KNULL(pstParams);
		OIF_RdFL(lvds_if_control);
		OIF_Wr01(lvds_if_control, lvds_empty_data, pstParams->emptyData);
		OIF_Wr01(lvds_if_control, lvds_8bit_mode,  pstParams->bitMode);
		OIF_Wr01(lvds_if_control, lvds_lsb_first,  pstParams->lsbfirst);
		OIF_Wr01(lvds_if_control, lvds_sync_order, pstParams->syncOrder);
		if (lx_chip_rev( ) >= LX_CHIP_REV(L8,B0) )
		{
			OIF_Wr01(lvds_if_control, lvds_out_sel,    0x0);
		}
		else
		{
			OIF_Wr01(lvds_if_control, lvds_out_sel,    pstParams->outSel);
		}
		OIF_Wr01(lvds_if_control, lvds_ch3_from,   pstParams->ch3LVDS);
		OIF_Wr01(lvds_if_control, lvds_ch2_from,   pstParams->ch2LVDS);
		OIF_Wr01(lvds_if_control, lvds_ch1_from,   pstParams->ch1LVDS);
		OIF_Wr01(lvds_if_control, lvds_ch0_from,   pstParams->ch0LVDS);
		OIF_WrFL(lvds_if_control);
		ret = RET_OK;
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Control LVDS TX following TCON enable.
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_OIF_SetLvdsTxCtrl(LX_DE_OIF_LVDS_TX_CTRL_T *pstParams)
{
	int ret = RET_ERROR;
	UINT32 enpemp_h;
	UINT32 enpemp_l;

	do {
		CHECK_KNULL(pstParams);
		enpemp_h = GET_BITS(pstParams->empCtrl, 16, 16);
		enpemp_l = GET_BITS(pstParams->empCtrl, 0,  16);
#ifdef USE_KDRV_CODES_FOR_L8
		CTOP_CTRL_RdFL(ctr32);
		CTOP_CTRL_RdFL(ctr33);
		CTOP_CTRL_RdFL(ctr34);
		CTOP_CTRL_Wr01(ctr32, pdb,			pstParams->pdb);
		CTOP_CTRL_Wr01(ctr32, rf,			pstParams->rf);
		CTOP_CTRL_Wr01(ctr33, rs,			pstParams->rs);
		CTOP_CTRL_Wr01(ctr33, enpemp_h,		enpemp_h);
		CTOP_CTRL_Wr01(ctr34, enpemp_l,		enpemp_l);
		CTOP_CTRL_Wr01(ctr32, mini_en,		pstParams->miniLVDS);
		if (lx_chip_rev( ) >= LX_CHIP_REV(L8,B0) )
		{
			// field removed
		}
		else
		{
			CTOP_CTRL_A0_Wr01(ctr32, mini_flip_en, pstParams->miniFlip);
		}
		CTOP_CTRL_Wr01(ctr32, rmlvds_en,	pstParams->swingLVDS);
		CTOP_CTRL_Wr01(ctr34, ovdrv,		pstParams->ovDrv);
		CTOP_CTRL_Wr01(ctr32, en,			pstParams->lvdsEn);
		CTOP_CTRL_WrFL(ctr32);
		CTOP_CTRL_WrFL(ctr33);
		CTOP_CTRL_WrFL(ctr34);
#endif
		ret = RET_OK;
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Set Table.
 *
 * @param pstParams [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetTable(LX_DE_SET_TABLE_T *pstParams)
{
	int ret = RET_OK;
	LX_DE_REG_WR_T *pTable;
	UINT32 tableLength;
	int i;

	do {
		CHECK_KNULL(pstParams);
		pTable = (LX_DE_REG_WR_T *)pstParams->pTable;
		CHECK_KNULL(pTable);
		tableLength = pstParams->size/sizeof(LX_DE_REG_WR_T);
		for (i=0;i<tableLength;i++) VIDEO_REG_WD(pTable[i].wr_addr, pTable[i].wr_data);
	} while(0);

	return ret;
}

int DE_REG_GPIO_Init(void)
{
#ifdef USE_KDRV_CODES_FOR_L8
#ifdef USE_VIDEO_UART2_FOR_MCU
	if (lx_chip_rev() >= LX_CHIP_REV(L8,B0))
	{
		// for L8B0
		CTOP_CTRL_RdFL(ctr62);
		CTOP_CTRL_Wr01(ctr62, jtag_sel,  0x3);  // setting JTAG for DE
		CTOP_CTRL_WrFL(ctr62);
		CTOP_CTRL_RdFL(ctr03);
		CTOP_CTRL_Wr01(ctr03, uart2_sel,  0x0);  // setting UART2 for DE
		CTOP_CTRL_Wr01(ctr03, rx_sel_de,  0x0);
		CTOP_CTRL_WrFL(ctr03);
	}
	else
	{
		CTOP_CTRL_A0_Wr(ctr62, 0x00000003); // setting JTAG and UART for DE
		CTOP_CTRL_A0_WrFL(ctr62);
	}
#else
	if (lx_chip_rev() >= LX_CHIP_REV(L8,B0))
	{
		// for L8B0
		CTOP_CTRL_RdFL(ctr62);
		CTOP_CTRL_Wr01(ctr62, jtag_sel,  0x0);  // setting JTAG for SP
		CTOP_CTRL_WrFL(ctr62);
		CTOP_CTRL_RdFL(ctr03);
		CTOP_CTRL_Wr01(ctr03, uart2_sel,  0x1);  // setting UART2 for CPU (Magic Remote)
		CTOP_CTRL_WrFL(ctr03);
	}
	else
	{
		CTOP_CTRL_A0_Wr(ctr62, 0x00000008); // Magic Mouse Remote
		CTOP_CTRL_A0_WrFL(ctr62);
	}
#endif
#endif

	return 0;
}

int DE_REG_HDMI_Init(void)
{
#if 0
	// setting HDMI
	VIDEO_REG_WD(0xf6007064, 0x00000000);
	VIDEO_REG_WD(0xf6007044, 0x11);
	VIDEO_REG_WD(0xf600704C, 0x0);
	VIDEO_REG_WD(0xf6007060, 0x0);
	VIDEO_REG_WD(0xf6007064, 0x00010000);
	VIDEO_REG_WD(0xf6007068, 0x0);
	VIDEO_REG_WD(0xf6007420, 0x10);
	VIDEO_REG_WD(0xf600741c, 0x00000);

	VIDEO_REG_WD(0xf601E140, 0x00000011);
	VIDEO_REG_WD(0xf601E034, 5);
	VIDEO_REG_WD(0xf601E090, 1);
	VIDEO_REG_WD(0xf601E06c, 1);
	VIDEO_REG_WD(0xf601E070, 47);
	VIDEO_REG_WD(0xf601E074, 1);

	VIDEO_REG_WD(0xf601E340, 0x00000011);
	VIDEO_REG_WD(0xf601E234, 5);
	VIDEO_REG_WD(0xf601E290, 1);
	VIDEO_REG_WD(0xf601E26c, 1);
	VIDEO_REG_WD(0xf601E270, 47);
	VIDEO_REG_WD(0xf601E274, 1);

	VIDEO_REG_WD(0xf6007064, 0x0);
	VIDEO_REG_WD(0xf6007544, 0x1);
#endif

	return 0;
}

int DE_REG_LVDS_Init(void)
{
#if 0	// disable lvds off to remain bootlog image as long as possible. dongho7.park 2010.10.19
// bootlog 에 의해 enable 된 LVDS 를 여기서 off 하여 memory dummy pattern 을 없앤다. 2010 09 14 dongho7.park
	CTOP_CTRL_Wr(ctr32, 0xf5000000);
	CTOP_CTRL_WrFL(ctr32);
#endif
#if 0
	CTOP_CTRL_Wr(ctr32, 0xb5ffffff);
	CTOP_CTRL_Wr(ctr33, 0xffffff00);
	CTOP_CTRL_Wr(ctr34, 0x00000000);
	CTOP_CTRL_Wr(ctr38, 0x00000000);
	CTOP_CTRL_Wr(ctr39, 0x007d1fff);
	CTOP_CTRL_WrFL(ctr32);
	CTOP_CTRL_WrFL(ctr33);
	CTOP_CTRL_WrFL(ctr34);
	CTOP_CTRL_WrFL(ctr38);
	CTOP_CTRL_WrFL(ctr39);
#endif
	return 0;
}

int DE_REG_MISC_Init(void)
{
#if 0
	//OIF_Wr(out_video_info, 0x00000001);  // setting oif for DE diplay
	//OIF_WrFL(out_video_info);
	//DE1A_CTRL_Wr(disp_param0, 0x000a0418);  // setting display offset
	//DE1A_CTRL_Wr(disp_param2, 0x071c000b);  // setting display offset
	//DE1A_CTRL_WrFL(disp_param0);
	//DE1A_CTRL_WrFL(disp_param2);
#endif
	return 0;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set uart for MCU or CPU
 *
 * @param arg [IN] address of buffer to be carried
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_Uart0_Switch(int whichUart)
{
#ifdef USE_KDRV_CODES_FOR_L8
#ifdef USE_DE_SWITCH_UART
	if (lx_chip_rev( ) >= LX_CHIP_REV(L8,B0) )
	{
		// for L8B0
		CTOP_CTRL_RdFL(ctr03);
		CTOP_CTRL_WfCM(ctr03, uart0_sel, !whichUart, 0x2);
		CTOP_CTRL_WfCM(ctr03, rx_sel_de,  whichUart, 0x1);
		CTOP_CTRL_WrFL(ctr03);
	}
	else
	{
		CTOP_CTRL_A0_RdFL(ctr61);
		CTOP_CTRL_A0_Wr01(ctr61, uart0_sel,   whichUart);
		CTOP_CTRL_A0_WfCM(ctr61, uart_rx_sel, whichUart, 0x1);
		CTOP_CTRL_A0_WrFL(ctr61);
	}
#endif
#endif
	return 0;
}

#ifdef USE_DE_FIRMWARE_DWONLOAD_IN_DRIVER
int DE_REG_FW_Download(LX_DE_FW_DWLD_T *pstParams)
{
	int ret = RET_OK;
	char						*lpAddr = NULL;
	LX_DE_CH_MEM_T				*pFwMem = NULL;
	SILK_PORT_SEL_T             silk_port_sel;
	IPC_108M_SET_REG_T			ipc_108m_set_reg;

	do {
		CHECK_KNULL(pstParams);
		ipc_108m_set_reg = gpRealRegSPP_MCU->ipc_108m_set_reg;
		if (!ipc_108m_set_reg.runstall) break;
		switch (pstParams->inx) {
			case 1 :
				pFwMem = (LX_DE_CH_MEM_T *)&gpDeMem->fw[1];
				DE_PRINT("Loading LED_FW\n");
				break;
			case 2 :
			case 3 :
				pFwMem = (LX_DE_CH_MEM_T *)&gpDeMem->fw[0];
				break;
			default :
				BREAK_WRONG(pstParams->inx);
				break;
		}
		if (ret) break;
		CHECK_KNULL(pFwMem);
		lpAddr	= (char *)ioremap(pFwMem->fw_base, pFwMem->fw_size);
		CHECK_KNULL(lpAddr);
		if (!pstParams->size) break;
		if (!pstParams->pData) break;
		memcpy(lpAddr, pstParams->pData, pstParams->size);
		if (lpAddr) iounmap((void*)lpAddr);
		switch (pstParams->inx) {
			case 2 :
				REG_WD(&gpRealRegSPP_MCU->silk_port_sel, 0x1016);
				REG_WD(&gpRealRegeDMA->txsrcaddr, pFwMem->fw_base);
				REG_WD(&gpRealRegeDMA->txdstaddr, 0x3ffe0000);
				REG_WD(&gpRealRegeDMA->txlength,  (pstParams->size + sizeof(UINT32) -1)/sizeof(UINT32));
			    REG_WD(&gpRealRegeDMA->edma_control_register, 0x3);
				while (1) {
					if (gpRealRegeDMA->edma_control_register.txdone) break;
					msleep(1);
				}
				DE_PRINT("Loading DE_IFW\n");
				break;
			case 3  :
				DE_PRINT("Loading DE_DFW\n");
				ipc_108m_set_reg.runstall = 0;
				gpRealRegSPP_MCU->ipc_108m_set_reg = ipc_108m_set_reg;
#ifdef USE_DE_SWAP_DDR_MEMORY
				silk_port_sel = gpRealRegSPP_MCU->silk_port_sel;
				silk_port_sel.sram0_silk_port_sel = 0x1;
				silk_port_sel.sram1_silk_port_sel = 0x0;
				gpRealRegSPP_MCU->silk_port_sel = silk_port_sel;
#endif
			default :
				break;
		}
	} while (0);

	return RET_OK;
}
#endif

/**
 * @callgraph
 * @callergraph
 *
 * @brief send color space conversion matrix and offset for each external source information.
 *
 * @param [IN] structure pointer to carry infomation about cvi FIR filter
 *
 * @return RET_OK(0)
 */
int DE_REG_SetCviFir(LX_DE_CVI_FIR_T *pstParams)
{
	int ret = RET_OK;
	DE_PARAM_TYPE_T tableId;
	UINT32 firTable[ARRAY_SIZE(pstParams->fir_coef)];
	UINT32 inx = 0;
	int i;

	do {
		CHECK_KNULL(pstParams);
		switch (pstParams->cvi_channel) {
			case LX_DE_CVI_CH_A :
				tableId = DE_CVM_FIR_COEF;
				break;
			case LX_DE_CVI_CH_B :
				tableId = DE_CVS_FIR_COEF;
				break;
			default :
				BREAK_WRONG(pstParams->cvi_channel);
		}
		if (ret) break;
		for (i=0;i<ARRAY_SIZE(pstParams->fir_coef);i++) firTable[inx++] = pstParams->fir_coef[i];

		ret = DE_PRM_LoadTable(tableId, DE_PARAM_WRITE, pstParams->isEnable, firTable, ARRAY_SIZE(firTable));
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set captured video test pattern generator to mono-tone color.
 *
 * @param [IN] structure pointer to carry infomation about captured test pattern generator parameter.
 *
 * @return RET_OK(0)
 */
int DE_REG_SetCviTpg(LX_DE_CVI_TPG_T *pstParams)
{
	int ret = RET_OK;
	unsigned char ptn_type;

	do {
		CHECK_KNULL(pstParams);
		(pstParams->isPtnOn == TRUE)?(ptn_type = 0x1):(ptn_type= 0x0); // one color pattern or bypass

		switch (pstParams->cvi_channel) {
			case LX_DE_CVI_CH_A :
				CHA_CVI_RdFL(cha_misc_ctrl);
				CHA_CVI_Wr01(cha_misc_ctrl, pattern_type, ptn_type);
				CHA_CVI_WfCM(cha_misc_ctrl, pattern_csc, (pstParams->isGBR == TRUE), 0x1);
				CHA_CVI_WfCM(cha_misc_ctrl, write_inhibit, (pstParams->isFrzOn == TRUE), 0x1);
				CHA_CVI_Wr01(cha_misc_ctrl, pattern_detail, pstParams->ptnColor);
				CHA_CVI_WrFL(cha_misc_ctrl);
				break;
			case LX_DE_CVI_CH_B :
				CHB_CVI_RdFL(chb_misc_ctrl);
				CHB_CVI_Wr01(chb_misc_ctrl, pattern_type, ptn_type);
				CHB_CVI_WfCM(chb_misc_ctrl, pattern_csc, (pstParams->isGBR == TRUE), 0x1);
				CHB_CVI_WfCM(chb_misc_ctrl, write_inhibit, (pstParams->isFrzOn == TRUE), 0x1);
				CHB_CVI_Wr01(chb_misc_ctrl, pattern_detail, pstParams->ptnColor);
				CHB_CVI_WrFL(chb_misc_ctrl);
				break;
			default :
				BREAK_WRONG(pstParams->cvi_channel);
		}
		if (ret) break;
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set captured video color sampling mode(sub sampling or 3 tap filtering).
 *
 * @param [IN] structure pointer to carry infomation about captured color sampling parameter.
 *
 * @return RET_OK(0)
 */
int DE_REG_SetCviCsampleMode(LX_DE_CSAMPLE_MODE_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);

		switch (pstParams->cvi_channel) {
			case LX_DE_CVI_CH_A :
				CHA_CVI_RdFL(cha_dig_filt_ctrl3);
				CHA_CVI_WfCM(cha_dig_filt_ctrl3, fir_sample_c, (pstParams->is3tap == TRUE), 0x1);
				CHA_CVI_WrFL(cha_dig_filt_ctrl3);
				break;
			case LX_DE_CVI_CH_B :
				CHB_CVI_RdFL(chb_dig_filt_ctrl3);
				CHB_CVI_WfCM(chb_dig_filt_ctrl3, fir_sample_c, (pstParams->is3tap == TRUE), 0x1);
				CHB_CVI_WrFL(chb_dig_filt_ctrl3);
				break;
			default :
				BREAK_WRONG(pstParams->cvi_channel);
		}
		if (ret) break;
	} while (0);

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief reshape sync of captured video for PC input.
 *
 * @param [IN] structure pointer to carry infomation about sync parameter.
 *
 * @return RET_OK(0)
 */
int DE_REG_SetCviSyncReshape(LX_DE_CVI_SYNC_RESHAPE_T *pstParams)
{
	int ret = RET_OK;

	do {
		CHECK_KNULL(pstParams);

		switch (pstParams->cvi_channel) {
			case LX_DE_CVI_CH_A :
				CHA_CVI_RdFL(cha_int_sync_ctrl0);
				CHA_CVI_RdFL(cha_int_sync_ctrl1);
				CHA_CVI_Wr01(cha_int_sync_ctrl0, res_hsync, pstParams->hsync_delay);
				CHA_CVI_Wr01(cha_int_sync_ctrl0, res_hfp,	pstParams->hfp_delay);
				CHA_CVI_Wr01(cha_int_sync_ctrl1, res_vfp,	pstParams->vfp_delay);
				CHA_CVI_Wr01(cha_int_sync_ctrl1, res_vsync, pstParams->vsync_delay);
				CHA_CVI_WfCM(cha_int_sync_ctrl1, res_sync_sel,(pstParams->isEnable == FALSE), 0x1);
				CHA_CVI_WrFL(cha_int_sync_ctrl0);
				CHA_CVI_WrFL(cha_int_sync_ctrl1);
				break;
			case LX_DE_CVI_CH_B :
				CHB_CVI_RdFL(chb_int_sync_ctrl0);
				CHB_CVI_RdFL(chb_int_sync_ctrl1);
				CHB_CVI_Wr01(chb_int_sync_ctrl0, res_hsync, pstParams->hsync_delay);
				CHB_CVI_Wr01(chb_int_sync_ctrl0, res_hfp,	pstParams->hfp_delay);
				CHB_CVI_Wr01(chb_int_sync_ctrl1, res_vfp,	pstParams->vfp_delay);
				CHB_CVI_Wr01(chb_int_sync_ctrl1, res_vsync, pstParams->vsync_delay);
				CHB_CVI_WfCM(chb_int_sync_ctrl1, res_sync_sel,(pstParams->isEnable == FALSE), 0x1);
				CHB_CVI_WrFL(chb_int_sync_ctrl0);
				CHB_CVI_WrFL(chb_int_sync_ctrl1);
				break;
			default :
				BREAK_WRONG(pstParams->cvi_channel);
		}
		if (ret) break;
	} while (0);

	return ret;
}

void DE_REG_InitPeTPG(void)
{
#ifdef USE_KDRV_CODES_FOR_L8
	LX_DE_PE_COORDINATE_T coordinate[MAX_PE_TPG_RECT] = {
		{0, 0, 1920, 1080}, // LX_DE_WINBLANK_FULL
		{600, 0, 1800, 990}, // LX_DE_WINBLANK_WIN0_VCS
		{44, 212, 548, 582}, // LX_DE_WINBLANK_WIN1_VCS
		{0, 0, 44, 1080}, {44, 990, 1800, 1080}, {1800, 0, 1920, 1080}, // LX_DE_SETWIN_VCS
		{0, 0, 239, 1080}, {1680, 0, 1920, 1080}, // LX_DE_NONACTIVE_4_3
		{0, 0, 241, 1080}, {1678, 0, 1920, 1080}, // LX_DE_NONACTIVE_4_3_WR
		{0, 0, 12, 1080}, {947, 0, 972, 1080}, {1907, 0, 1919, 1080}, // LX_DE_NONACTIVE_3D
		{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
		{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
	};
	LX_DE_COLOR_T color[MAX_PE_TPG_RECT] = {
		{0, 0, 0}, // LX_DE_WINBLANK_FULL
		{0, 0, 0}, // LX_DE_WINBLANK_WIN0_VCS
		{0, 0, 0}, // LX_DE_WINBLANK_WIN1_VCS
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, // LX_DE_SETWIN_VCS
		{0, 0, 0}, {0, 0, 0}, // LX_DE_NONACTIVE_4_3
		{0, 0, 0}, {0, 0, 0}, // LX_DE_NONACTIVE_4_3_WR
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, // LX_DE_NONACTIVE_3D
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
	};
	UINT8 index;

	PE1_RdFL(tpg_ctrl_00);
	PE1_Wr01(tpg_ctrl_00, tpg_enable, 0);
	PE1_WrFL(tpg_ctrl_00);

	PE1_RdFL(pe1_top_ctrl_02);
	PE1_Wr01(pe1_top_ctrl_02, tpg_positon, 1);
	PE1_WrFL(pe1_top_ctrl_02);

	PE1_Wr01(tpg_indir_ctrl, tpg_indir_ai_en, 1);
	PE1_Wr01(tpg_indir_ctrl, tpg_indir_addr,  0);  // Patch color
	PE1_WrFL(tpg_indir_ctrl);

	for (index = 0; index < MAX_PE_TPG_RECT ; index++)
	{
		PE1_Wr01(tpg_indir_data, tpg_indir_data, color[index].g << 22 | color[index].b << 12 | color[index].r << 2);
		PE1_WrFL(tpg_indir_data);
	}

	PE1_Wr01(tpg_indir_ctrl, tpg_indir_ai_en, 1);
	PE1_Wr01(tpg_indir_ctrl, tpg_indir_addr,  0x20);  // Patch start position
	PE1_WrFL(tpg_indir_ctrl);

	for (index = 0; index < MAX_PE_TPG_RECT ; index++)
	{
		PE1_Wr01(tpg_indir_data, tpg_indir_data, coordinate[index].x0 << 16 | coordinate[index].y0);
		PE1_WrFL(tpg_indir_data);
	}

	PE1_Wr01(tpg_indir_ctrl, tpg_indir_ai_en, 1);
	PE1_Wr01(tpg_indir_ctrl, tpg_indir_addr,  0x40);  // Patch end position
	PE1_WrFL(tpg_indir_ctrl);

	for (index = 0; index < MAX_PE_TPG_RECT ; index++)
	{
		PE1_Wr01(tpg_indir_data, tpg_indir_data, coordinate[index].x1 << 16 | coordinate[index].y1);
		PE1_WrFL(tpg_indir_data);
	}

	PE1_Wr01(tpg_ctrl_00, tpg_enable, 1);
	PE1_WrFL(tpg_ctrl_00);
#endif

	g_b_InitTPG = TRUE;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief generate Test Pattern in PE region.
 *
 * @param [IN] structure pointer to carry information about PE gamma LUT region.
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetPeTestPatternGen(LX_DE_PE_TPG_T *pstParams)
{
	int ret = RET_OK;

	if (!g_b_InitTPG)
		DE_REG_InitPeTPG();

#ifdef USE_KDRV_CODES_FOR_L8
	do {
		CHECK_KNULL(pstParams);

		switch (pstParams->eControl)
		{
			case LX_DE_WINBLANK_FULL :
				{
					g_patchEn = g_patchEn & 0xFFFFFFFE;

					if (pstParams->bOnOff)
						g_patchEn |= 0x00000001;

					break;
				}
/*
			case LX_DE_WINBLANK_WIN0_VCS :
				{
					g_patchEn = g_patchEn & 0xFFFFFFFD;

					if (pstParams->bOnOff)
						g_patchEn |= 0x00000002;

					break;
				}
*/
			case LX_DE_WINBLANK_WIN1_VCS :
				{
					g_patchEn = g_patchEn & 0xFFFFFFFB;

					if (pstParams->bOnOff)
						g_patchEn |= 0x00000004;

					break;
				}

			case LX_DE_NONACTIVE_VCS :
				{
					g_patchEn = g_patchEn & 0xFFFFFFE7;

					if (pstParams->bOnOff)
						g_patchEn |= 0x00000038;

					break;
				}

			case LX_DE_NONACTIVE_4_3 :
				{
					g_patchEn = g_patchEn & 0xFFFFFF3F;

					if (pstParams->bOnOff)
						g_patchEn |= 0x000000C0;

					break;
				}

			case LX_DE_NONACTIVE_4_3_WR :
				{
					g_patchEn = g_patchEn & 0xFFFFFCFF;

					if (pstParams->bOnOff)
						g_patchEn |= 0x00000300;

					break;
				}

			case LX_DE_NONACTIVE_3D :
				{
					g_patchEn = g_patchEn & 0xFFFFE3FF;

					if (pstParams->bOnOff)
						g_patchEn |= 0x00001C00;

					break;
				}

			case LX_DE_TPG_CLEAR_ALL :
				{
					g_patchEn = 0;

					break;
				}

			default :
				return RET_INVALID_PARAMS;
		}

		PE1_RdFL(tpg_ctrl_02);
		PE1_Wr01(tpg_ctrl_02, patch_en, g_patchEn);
		PE1_WrFL(tpg_ctrl_02);
	} while (0);
#endif

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set PE0 black boundary detection control.
 *
 * @param [IN] structure pointer to carry infomation about PE0 black boundary detection control.
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetPeBbdCtrl(LX_DE_PE_BBD_CTRL_T *pstParams)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L8
	do {
		CHECK_KNULL(pstParams);

		PE0_RdFL(bbd_ctrl_00);
		PE0_Wr01(bbd_ctrl_00, hysterisis_mode				  ,pstParams->hysterisis);
		PE0_Wr01(bbd_ctrl_00, cnt_th						  ,pstParams->cnt_th);
		PE0_Wr01(bbd_ctrl_00, diff_th						  ,pstParams->diff_th);
		PE0_Wr01(bbd_ctrl_00, op_mode						  ,0x0);	// assert auto boundary detection
#ifdef USE_PQL_REG_DEFINED_IN_DE
		PE0_WfCM(bbd_ctrl_00, black_boundary_detection_enable ,(pstParams->isOn == TRUE) ,0x1);
#endif
		PE0_WrFL(bbd_ctrl_00);
	} while (0);
#endif

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief get PE0 black boundary detection status.
 *
 * @param [IN] structure pointer to carry infomation about PE0 black boundary detection status.
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_GetPeBbdStatus(LX_DE_PE_COORDINATE_T *pstParams)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L8
	do {
		CHECK_KNULL(pstParams);

		PE0_RdFL(bbd_stat_0);
		PE0_RdFL(bbd_stat_1);

		PE0_Rd01(bbd_stat_0, x0	,pstParams->x0);
		PE0_Rd01(bbd_stat_0, y0	,pstParams->y0);
		PE0_Rd01(bbd_stat_1, x1	,pstParams->x1);
		PE0_Rd01(bbd_stat_1, y1	,pstParams->y1);
	} while (0);
#endif

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief set edge crop
 *
 * @param [IN] structure pointer to carry infomation about PE0 black boundary detection status.
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int DE_REG_SetEdgeCrop(BOOLEAN *pstParams)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L8
	do {
		;
	} while (0);
#endif

	return ret;
}

int DE_REG_ResetDE(BOOLEAN bReset)
{
	bReset = (bReset)?TRUE:FALSE;

#ifdef USE_KDRV_CODES_FOR_L8
	CTOP_CTRL_RdFL(ctr11);
	CTOP_CTRL_RdFL(ctr13);

	CTOP_CTRL_WfCM(ctr11, rst_de_de_n   ,bReset, 1);
	CTOP_CTRL_WfCM(ctr11, rst_apb_de_n  ,bReset, 1);
	CTOP_CTRL_WfCM(ctr11, rst_dea_n     ,bReset, 1);
	CTOP_CTRL_WfCM(ctr11, rst_de_vd_n   ,bReset, 1);

	CTOP_CTRL_WfCM(ctr13, rst_dea_dp_n  ,bReset, 1);
	CTOP_CTRL_WfCM(ctr13, rst_disp_dp_n ,bReset, 1);
	CTOP_CTRL_WfCM(ctr13, rst_de_dp_n   ,bReset, 1);
	CTOP_CTRL_WfCM(ctr13, rst_disp_de_n ,bReset, 1);
	CTOP_CTRL_WfCM(ctr13, rst_chb_n	    ,bReset, 1);
	CTOP_CTRL_WfCM(ctr13, rst_cha_n	    ,bReset, 1);
	CTOP_CTRL_WfCM(ctr13, rst_s_de_n    ,bReset, 1);

	CTOP_CTRL_WrFL(ctr11);
	CTOP_CTRL_WrFL(ctr13);
#endif
	return RET_OK;
}

BOOLEAN VIDEO_REG_IPCisAlive(void)
{
	return gpRealRegSPP_MCU->int_interrupt_enable.ipc_intterupt_enalbe_108;
}

int DE_REG_OIF_InitLvdsIfCtrl(void)
{
	OIF_RdFL(lvds_if_control);
	OIF_Wr01(lvds_if_control, lvds_ch3_from,   3);
	OIF_Wr01(lvds_if_control, lvds_ch2_from,   2);
	OIF_Wr01(lvds_if_control, lvds_ch1_from,   1);
	OIF_Wr01(lvds_if_control, lvds_ch0_from,   0);
	OIF_WrFL(lvds_if_control);

	return RET_OK;
}

#ifdef USE_KDRV_CODES_FOR_L8
#ifndef KDRV_GLOBAL_LINK
#ifdef USE_PQL_REG_DEFINED_IN_DE
EXPORT_SYMBOL(gpRealRegPE0);
EXPORT_SYMBOL(gpRealRegPE1);
EXPORT_SYMBOL(gpRegPE0);
EXPORT_SYMBOL(gpRegPE1);
#endif
#endif
#endif
/**  @} */
