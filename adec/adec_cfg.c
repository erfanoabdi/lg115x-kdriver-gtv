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
 *  main configuration file for adec device
 *	adec device will teach you how to make device driver with new platform.
 *
 *  author		jongsang.oh (jongsang.oh@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */


/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "adec_kapi.h"
#include "adec_cfg.h"

#ifdef INCLUDE_L8_CHIP_KDRV
#include "l8/adec_reg_l8.h"
#else
#include "l9/adec_reg_l9.h"
#endif


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
/**
ADEC Register configuration.
*/
LX_ADEC_REG_CFG_T gstAdecRegCfg[] =
{
	// for LG1150(L8) A0 & B0
	{
		.chip_name			= "LG1150(L8)",
		.reg_base_addr 		= 0xC6006000,
		.reg_size			= 0x00000600,
		.irq0_num			= 0x47,
		.irq1_num			= 0x48,
	},
	// for LG1152(L9) A0 & A1
	{
		.chip_name			= "LG1152(L9 A0)",
		.reg_base_addr 		= 0xC0008000,
		.reg_size			= 0x00000500,
		.irq0_num			= 0x5F,
		.irq1_num			= 0x60,
	},
	// for LG1152(L9) B0
	{
		.chip_name			= "LG1152(L9 B0)",
		.reg_base_addr 		= 0xC0008000,
		.reg_size			= 0x00000700,
		.irq0_num			= 0x5F,
		.irq1_num			= 0x60,
	},
};

/**
ADEC Register definition.
*/
LX_ADEC_REG_CFG_T *pstAdecRegCfg = &gstAdecRegCfg[0];	// for Real. Chip


/**
ADEC Memory Map configuration.
*/
LX_ADECDEV_MEM_CFG_S_T gstMemCfgADEC[] =
{
#ifdef INCLUDE_L8_CHIP_KDRV
	// for LG1150(L8) A0 & B0
	{
		//DSP 0
		.fw0_memory_name		= "adec_fw0",		/* dsp0 firmware download memory */
		.fw0_memory_base 		= 0x0A420000,
#ifndef ENABLE_DTS_CERTI
		.fw0_memory_size 		= 0x00100000,		/* 1 MB */
#else
		.fw0_memory_size 		= 0x00180000,		/* 1.5 MB for DTS Certification */
#endif

		.dsp0_memory_name 		= "adec_dsp0",		/* dsp0 running memory */
		.dsp0_memory_base 		= 0x0A520000,
		.dsp0_memory_size 		= 0x00200000,		/* 2 MB */

		.cpbm_memory_name 		= "adec_cpbm",		/* dsp0 coded PCM buffer main memory */
		.cpbm_memory_base 		= 0x0A720000,
#if !defined(ENABLE_MS10_DDC_CERTI) && !defined(ENABLE_DTS_CERTI)
		.cpbm_memory_size 		= 0x00180000,		/* 1.5 MB */
#else
#ifdef ENABLE_MS10_DDC_CERTI
		.cpbm_memory_size		= 0x03700000,		/* 55 MB for MS10 Certification */
#endif	//#ifdef ENABLE_MS10_DDC_CERTI
#ifdef ENABLE_DTS_CERTI
		.cpbm_memory_size		= 0x05000000,		/* 80 MB for DTS Certification */
#endif	//#ifdef ENABLE_DTS_CERTI
#endif	//#if (defined !ENABLE_DTS_CERTI && defined !ENABLE_MS10_DDC_CERTI)

		.cpba_memory_name 		= "adec_cpba",		/* dsp0 coded PCM buffer associate memory */
		.cpba_memory_base 		= 0x0A8A0000,
#ifndef ENABLE_MS10_DDC_CERTI
		.cpba_memory_size 		= 0x00080000,		/* 0.5 MB */
#else
		.cpba_memory_size 		= 0x03700000,		/* 55 MB for MS10 Certification */
#endif	//#ifndef ENABLE_MS10_DDC_CERTI

		//DSP 1
		.fw1_memory_name		= "adec_fw1",		/* dsp1 firmware download memory */
		.fw1_memory_base 		= 0x0A920000,
		.fw1_memory_size 		= 0x00080000,		/* 0.5 MB */

		.dsp1_memory_name 		= "adec_dsp1",		/* dsp1 running memory */
		.dsp1_memory_base 		= 0x0A9A0000,
		.dsp1_memory_size 		= 0x00100000,		/* 1 MB */

		.dpbm_memory_name 		= "adec_dpbm",		/* dsp1 decoded PCM buffer main memory */
		.dpbm_memory_base 		= 0x0AAA0000,
		.dpbm_memory_size 		= 0x00080000,		/* 0.5 MB :  upper 2KB used LG sound engine parameters */

		.dpba_memory_name		= "adec_dpba",		/* dsp1 decoded PCM buffer associate memory */
		.dpba_memory_base		= 0x0AB20000,
		.dpba_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb0_memory_name 		= "adec_mpb0",		/* dsp1 mixed PCM buffer 0 memory with SRC */
		.mpb0_memory_base 		= 0x0ABA0000,
		.mpb0_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb1_memory_name 		= "adec_mpb1",		/* dsp1 mixed PCM buffer 1 memory with SRC */
		.mpb1_memory_base 		= 0x0ABE0000,
		.mpb2_memory_size		= 0x00000000,		/* 0 MB */

		.mpb2_memory_name 		= "adec_mpb2",		/* dsp1 mixed PCM buffer 2 memory */
		.mpb2_memory_base 		= 0x0ABE0000,
		.mpb2_memory_size 		= 0x00000000,		/* 0 MB */

		.mpb3_memory_name		= "adec_mpb3",		/* dsp1 mixed PCM buffer 3 memory */
		.mpb3_memory_base		= 0x0ABE0000,
		.mpb3_memory_size		= 0x00000000,		/* 0 MB */

		.inter_memory_name 		= "adec_inter",		/* dsp1 inter buffer memory */
		.inter_memory_base 		= 0x0AC20000,
		.inter_memory_size 		= 0x00200000,		/* 2 MB */
	},
#else	//#ifdef INCLUDE_L8_CHIP_KDRV
	// for LG1152(L9) A0 /1, B0
	{
//To use dynamic allocation
#if 1
#if (KDRV_PLATFORM != KDRV_COSMO_PLATFORM)	//NetCast 3.0
//#if 0	//DDCO test for GCD model
		//DSP 0(10 MB)
		.fw0_memory_name		= "adec_fw0",		/* dsp0 firmware download memory */
		.fw0_memory_base 		= 0x00000000,
		.fw0_memory_size 		= 0x00200000,		/* 2.0 MB */

		.dsp0_memory_name 		= "adec_dsp0",		/* dsp0 running memory */
		.dsp0_memory_base 		= 0x00000000,
		.dsp0_memory_size 		= 0x00500000,		/* 5.0 MB */

		.cpbm_memory_name 		= "adec_cpbm",		/* dsp0 coded PCM buffer main memory */
		.cpbm_memory_base 		= 0x00000000,
		.cpbm_memory_size 		= 0x00180000,		/* 1.5 MB / 3 : PES =>0.5MB, ES = 0.5MB, Lip Sync = 0.5MB */

		.cpba_memory_name 		= "adec_cpba",		/* dsp0 coded PCM buffer associate memory */
		.cpba_memory_base 		= 0x00000000,
		.cpba_memory_size 		= 0x00180000,		/* 1.5 MB / 3 : PES =>0.5MB, ES = 0.5MB, Lip Sync = 0.5MB */

		.enc_intra_memory_name	= "adec_enci",		/* dsp0 encoder intra memory */
		.enc_intra_memory_base	= 0x00000000,
		.enc_intra_memory_size	= 0x00080000,		/* 0.5 MB */

		//DSP 1(10 MB)
		.fw1_memory_name		= "adec_fw1",		/* dsp1 firmware download memory */
		.fw1_memory_base 		= 0x00000000,
		.fw1_memory_size 		= 0x00080000,		/* 0.5 MB */

		.dsp1_memory_name 		= "adec_dsp1",		/* dsp1 running memory */
		.dsp1_memory_base 		= 0x00000000,
		.dsp1_memory_size 		= 0x00300000,		/* 3.0 MB */

		.mpb0_memory_name 		= "adec_mpb0",		/* dsp1 mixed PCM buffer 0 memory */
		.mpb0_memory_base 		= 0x00000000,
		.mpb0_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb1_memory_name 		= "adec_mpb1",		/* dsp1 mixed PCM buffer 1 memory */
		.mpb1_memory_base 		= 0x00000000,
		.mpb1_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb2_memory_name 		= "adec_mpb2",		/* dsp1 mixed PCM buffer 2 memory */
		.mpb2_memory_base 		= 0x00000000,
		.mpb2_memory_size 		= 0x00080000,		/* 0.5 MB */

		.mpb3_memory_name		= "adec_mpb3",		/* dsp1 mixed PCM buffer 3 memory */
		.mpb3_memory_base		= 0x00000000,
		.mpb3_memory_size		= 0x00080000,		/* 0.5 MB */

		.llb_memory_name		= "adec_llb",		/* dsp1 LLB mixed PCM buffer memory */
		.llb_memory_base		= 0x00000000,
		.llb_memory_size		= 0x00080000,		/* 0.5 MB */

		.dpbm_memory_name 		= "adec_dpbm",		/* dsp1 decoded PCM buffer main memory */
		.dpbm_memory_base 		= 0x00000000,
		.dpbm_memory_size 		= 0x00080000,		/* 0.5 MB  */

		.dpba_memory_name		= "adec_dpba",		/* dsp1 decoded PCM buffer associate memory */
		.dpba_memory_base		= 0x00000000,
		.dpba_memory_size		= 0x00080000,		/* 0.5 MB */

		.iec_memory_name 		= "adec_iec",		/* IEC(SPDIF) buffer memory */
		.iec_memory_base 		= 0x00000000,
		.iec_memory_size 		= 0x00100000,		/* 1.0 MB */

		.ddco_memory_name 		= "adec_ddco",		/* DDCO(SPDIF) buffer memory */
		.ddco_memory_base 		= 0x00000000,
		.ddco_memory_size 		= 0x00080000,		/* 0.5 MB */

		.acr_memory_name 		= "adec_acr",		/* ACR buffer memory */
		.acr_memory_base 		= 0x00000000,
		.acr_memory_size 		= 0x00080000,		/* 0.5 MB */

		.enc_memory_name 		= "adec_enc",		/* encoder buffer memory */
		.enc_memory_base 		= 0x00000000,
		.enc_memory_size 		= 0x00080000,		/* 0.5 MB */

		.se_memory_name 		= "adec_se",		/*adec sound engine parameter memory */
		.se_memory_base 		= 0x00000000,
		.se_memory_size 		= 0x00002000,		/* 8 KB */

		.debug_memory_name 		= "adec_debug",		/*adec debug buffer memory */
		.debug_memory_base 		= 0x00000000,
		.debug_memory_size 		= 0x00003000,		/* 4 * 3 = 12 KB */

		.ipc_memory_name 		= "adec_ipc",		/*adec IPC memory */
		.ipc_memory_base 		= 0x00000000,
		.ipc_memory_size		= 0x0007B000,		/* 492 KB(Min. 256 KB)  */
#else
		//DSP 0(10 MB)
		.fw0_memory_name		= "adec_fw0",		/* dsp0 firmware download memory */
		.fw0_memory_base 		= 0x00000000,
		.fw0_memory_size 		= 0x00200000,		/* 2.0 MB */

		.dsp0_memory_name 		= "adec_dsp0",		/* dsp0 running memory */
		.dsp0_memory_base 		= 0x00000000,
		.dsp0_memory_size 		= 0x00500000,		/* 5.0 MB */

		.cpbm_memory_name 		= "adec_cpbm",		/* dsp0 coded PCM buffer main memory */
		.cpbm_memory_base 		= 0x00000000,
		.cpbm_memory_size 		= 0x00180000,		/* 1.5 MB / 3 : PES =>0.5MB, ES = 0.5MB, Lip Sync = 0.5MB */

		.cpba_memory_name 		= "adec_cpba",		/* dsp0 coded PCM buffer associate memory */
		.cpba_memory_base 		= 0x00000000,
		.cpba_memory_size 		= 0x00180000,		/* 1.5 MB / 3 : PES =>0.5MB, ES = 0.5MB, Lip Sync = 0.5MB */

		.enc_intra_memory_name	= "adec_enci",		/* dsp0 encoder intra memory */
		.enc_intra_memory_base	= 0x00000000,
		.enc_intra_memory_size	= 0x00000000,		/* 0.0 MB => 2nd TV Spec. for NC */

		//DSP 1(10 MB)
		.fw1_memory_name		= "adec_fw1",		/* dsp1 firmware download memory */
		.fw1_memory_base 		= 0x00000000,
		.fw1_memory_size 		= 0x00080000,		/* 0.5 MB */

		.dsp1_memory_name 		= "adec_dsp1",		/* dsp1 running memory */
		.dsp1_memory_base 		= 0x00000000,
		.dsp1_memory_size 		= 0x00300000,		/* 3.0 MB */

		.mpb0_memory_name 		= "adec_mpb0",		/* dsp1 mixed PCM buffer 0 memory */
		.mpb0_memory_base 		= 0x00000000,
		.mpb0_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb1_memory_name 		= "adec_mpb1",		/* dsp1 mixed PCM buffer 1 memory */
		.mpb1_memory_base 		= 0x00000000,
		.mpb1_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb2_memory_name 		= "adec_mpb2",		/* dsp1 mixed PCM buffer 2 memory */
		.mpb2_memory_base 		= 0x00000000,
		.mpb2_memory_size 		= 0x00080000,		/* 0.5 MB */

		.mpb3_memory_name		= "adec_mpb3",		/* dsp1 mixed PCM buffer 3 memory */
		.mpb3_memory_base		= 0x00000000,
		.mpb3_memory_size		= 0x00080000,		/* 0.5 MB */

		.llb_memory_name		= "adec_llb",		/* dsp1 LLB mixed PCM buffer memory */
		.llb_memory_base		= 0x00000000,
		.llb_memory_size		= 0x00080000,		/* 0.5 MB */

		.dpbm_memory_name 		= "adec_dpbm",		/* dsp1 decoded PCM buffer main memory */
		.dpbm_memory_base 		= 0x00000000,
		.dpbm_memory_size 		= 0x00080000,		/* 0.5 MB  */

		.dpba_memory_name		= "adec_dpba",		/* dsp1 decoded PCM buffer associate memory */
		.dpba_memory_base		= 0x00000000,
		.dpba_memory_size		= 0x00080000,		/* 0.5 MB */

		.iec_memory_name 		= "adec_iec",		/* IEC(SPDIF) buffer memory */
		.iec_memory_base 		= 0x00000000,
		.iec_memory_size 		= 0x00100000,		/* 1.0 MB */

		.ddco_memory_name 		= "adec_ddco",		/* DDCO(SPDIF) buffer memory */
		.ddco_memory_base 		= 0x00000000,
		.ddco_memory_size 		= 0x00080000,		/* 0.5 MB */

		.acr_memory_name 		= "adec_acr",		/* ACR buffer memory */
		.acr_memory_base 		= 0x00000000,
		.acr_memory_size 		= 0x00080000,		/* 0.5 MB */

		.enc_memory_name 		= "adec_enc",		/* encoder buffer memory */
		.enc_memory_base 		= 0x00000000,
		.enc_memory_size 		= 0x00080000,		/* 0.5 MB */

		.se_memory_name 		= "adec_se",		/*adec sound engine parameter memory */
		.se_memory_base 		= 0x00000000,
		.se_memory_size 		= 0x00002000,		/* 8 KB */

		.debug_memory_name 		= "adec_debug",		/*adec debug buffer memory */
		.debug_memory_base 		= 0x00000000,
		.debug_memory_size 		= 0x00003000,		/* 4 * 3 = 12 KB */

		.ipc_memory_name 		= "adec_ipc",		/*adec IPC memory */
		.ipc_memory_base 		= 0x00000000,
		.ipc_memory_size 		= 0x0007B000,		/* 492 KB */
#endif
	},
#else
		//DSP 0(10 MB)
		.fw0_memory_name		= "adec_fw0",		/* dsp0 firmware download memory */
		.fw0_memory_base 		= 0x78000000,
		.fw0_memory_size 		= 0x00200000,		/* 2.0 MB */

		.dsp0_memory_name 		= "adec_dsp0",		/* dsp0 running memory */
		.dsp0_memory_base 		= 0x78200000,
		.dsp0_memory_size 		= 0x00500000,		/* 5.0 MB */

		.cpbm_memory_name 		= "adec_cpbm",		/* dsp0 coded PCM buffer main memory */
		.cpbm_memory_base 		= 0x78700000,
		.cpbm_memory_size 		= 0x00180000,		/* 1.5 MB */

		.cpba_memory_name 		= "adec_cpba",		/* dsp0 coded PCM buffer associate memory */
		.cpba_memory_base 		= 0x78880000,
		.cpba_memory_size 		= 0x00180000,		/* 1.5 MB */

		.enc_intra_memory_name	= "adec_enci",		/* dsp0 encoder intra memory */
		.enc_intra_memory_base	= 0x00000000,
		.enc_intra_memory_size	= 0x00080000,		/* 0.5 MB */

		//DSP 1(10 MB)
		.fw1_memory_name		= "adec_fw1",		/* dsp1 firmware download memory */
		.fw1_memory_base 		= 0x78A00000,
		.fw1_memory_size 		= 0x00080000,		/* 0.5 MB */

		.dsp1_memory_name 		= "adec_dsp1",		/* dsp1 running memory */
		.dsp1_memory_base 		= 0x78A80000,
		.dsp1_memory_size 		= 0x00300000,		/* 3.0 MB */

		.mpb0_memory_name 		= "adec_mpb0",		/* dsp1 mixed PCM buffer 0 memory */
		.mpb0_memory_base 		= 0x78D80000,
		.mpb0_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb1_memory_name 		= "adec_mpb1",		/* dsp1 mixed PCM buffer 1 memory */
		.mpb1_memory_base 		= 0x78E00000,
		.mpb1_memory_size		= 0x00080000,		/* 0.5 MB */

		.mpb2_memory_name 		= "adec_mpb2",		/* dsp1 mixed PCM buffer 2 memory */
		.mpb2_memory_base 		= 0x78E80000,
		.mpb2_memory_size 		= 0x00080000,		/* 0.5 MB */

		.mpb3_memory_name		= "adec_mpb3",		/* dsp1 mixed PCM buffer 3 memory */
		.mpb3_memory_base		= 0x78F00000,
		.mpb3_memory_size		= 0x00080000,		/* 0.5 MB */

		.dpbm_memory_name 		= "adec_dpbm",		/* dsp1 decoded PCM buffer main memory */
		.dpbm_memory_base 		= 0x78F80000,
		.dpbm_memory_size 		= 0x00100000,		/* 1.0 MB  */

		.dpba_memory_name		= "adec_dpba",		/* dsp1 decoded PCM buffer associate memory */
		.dpba_memory_base		= 0x79080000,
		.dpba_memory_size		= 0x00080000,		/* 0.5 MB */

		.iec_memory_name 		= "adec_iec",		/* IEC(SPDIF) buffer memory */
		.iec_memory_base 		= 0x79100000,
		.iec_memory_size 		= 0x00100000,		/* 1.0 MB */

		.enc_memory_name 		= "adec_enc",		/* encoder buffer memory */
		.enc_memory_base 		= 0x79200000,
		.enc_memory_size 		= 0x00080000,		/* 0.5 MB */

		.se_memory_name 		= "adec_se",		/*adec sound engine parameter memory */
		.se_memory_base 		= 0x79280000,
		.se_memory_size 		= 0x00002000,		/* 8 KB */

		.debug_memory_name 		= "adec_debug",		/*adec debug buffer memory */
		.debug_memory_base 		= 0x79282000,
		.debug_memory_size 		= 0x00003000,		/* 4 * 3 = 12 KB */

		.ipc_memory_name 		= "adec_ipc",		/*adec IPC memory */
		.ipc_memory_base 		= 0x79285000,
		.ipc_memory_size 		= 0x0017B000,		/* 1.5 MB */
	},
#endif	//To use dynamic allocation
#endif	//#ifdef INCLUDE_L8_CHIP_KDRV
};

/**
ADEC Memory Map definition.
*/
LX_ADECDEV_MEM_CFG_S_T *pstMemCfgADEC = &gstMemCfgADEC[0];	// for Real. Chip


/* Related to ADEC event processing */
UINT32	g_ui32CurEventType = 0;
UINT32	g_ui32RecEventType = 0;
LX_ADEC_DEC_MODE_T	g_ui32RecEventMode = LX_ADEC_DEC_MODE_NONE;

DEFINE_SPINLOCK(gADEC_DSP_event_lock);
DEFINE_SPINLOCK(gADEC_DoneNoti_lock);

/* Related to ADEC Interrupt monitor */
DECLARE_WAIT_QUEUE_HEAD(ADEC_wait_dsp0_ready);

int 					ADEC_dsp0_ready_waitq_flag;

DECLARE_WAIT_QUEUE_HEAD(ADEC_wait_dsp1_ready);

int 					ADEC_dsp1_ready_waitq_flag;


/* Related to ADEC monitor task */
LX_ADEC_TASK_MSG_T		g_suiWaitQForMonitor;
wait_queue_head_t 		Monitor_WaitQ;
DECLARE_WAIT_QUEUE_HEAD( Monitor_WaitQ );


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/



/** @} */
