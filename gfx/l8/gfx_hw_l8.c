/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/** @file
 *
 *	GFX hw control
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version     1.0
 *  date
 *  note        Additional information.
 *
 *  @addtogroup lg1150_gfx
 *  @{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define	GFX_PALETTE_AUTO_INC

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>	/**< printk() */
#include <linux/slab.h> 	/**< kmalloc() */
#include <linux/fs.h> 		/**< everything\ldots{} */
#include <linux/types.h> 	/**< size_t */
#include <linux/fcntl.h>	/**< O_ACCMODE */
#include <asm/uaccess.h>
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <asm/io.h>			/**< For ioremap_nocache */
#include <linux/interrupt.h>	/** interrupt */
#include <linux/platform_device.h>
#include <linux/delay.h>

#include "gfx_impl.h"
#include "gfx_reg_l8.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	_DIRECT_ACCESS_		0

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
extern volatile GFX_REG_MAP_T 	*g_gfx_reg_l8;
extern GFX_REG_MAP_T 			*g_gfx_reg_l8_cache;

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
volatile UINT32 guiAckMode = 0;

#ifdef	KDRV_CONFIG_PM
unsigned int gPmCmdDelay = 0;
unsigned int gPmIntrCtrl = 0;
#endif

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/
/** generate CQ BATCH RUN command after single or multiple CQ operation group.
 *	-- single CQ operation group is consist of single or multi normal CQs and CQ START
 *
 */
int GFX_L8_RunFlushCommand(void)
{
	GFX_TRACE_BEGIN();
	GFX_WARN("%s:%d -- CQ BATCH RUN issue !!!\n", __F__, __L__ );

	g_gfx_reg_l8->pause.bwPause	= FALSE;
	g_gfx_reg_l8->halt.bwHalt	= FALSE;
	g_gfx_reg_l8->start.bwStart	= FALSE;
	g_gfx_reg_l8->run.bwRun		= TRUE;

	GFX_TRACE_END();
	return RET_OK;
}

/** generate CQ START command after every GFX operation such write, blit etc.
 *
 *
 */
int GFX_L8_RunStartCommand(void)
{
	GFX_TRACE_BEGIN();
	GFX_WARN("%s:%d -- CQ START issue !!!\n", __F__, __L__ );
	g_gfx_reg_l8->pause.bwPause = FALSE;
	g_gfx_reg_l8->halt.bwHalt	= FALSE;
	g_gfx_reg_l8->run.bwRun = FALSE;
	g_gfx_reg_l8->start.bwStart = TRUE;
	GFX_TRACE_END();
	return RET_OK;
}

int GFX_L8_PauseCommand(void)
{
	GFX_TRACE_BEGIN();
	g_gfx_reg_l8->halt.bwHalt = FALSE;
	g_gfx_reg_l8->run.bwRun = FALSE;
	g_gfx_reg_l8->start.bwStart = FALSE;
	g_gfx_reg_l8->pause.bwPause = 1;
	GFX_TRACE_END();

	return 0;
}

int GFX_L8_ResumeCommand(void)
{
	GFX_TRACE_BEGIN();
	g_gfx_reg_l8->halt.bwHalt = FALSE;
	g_gfx_reg_l8->run.bwRun = FALSE;
	g_gfx_reg_l8->start.bwStart = FALSE;
	g_gfx_reg_l8->pause.bwPause = 2;
	GFX_TRACE_END();

	return 0;
}

int GFX_L8_StopCommand(void)
{
	GFX_TRACE_BEGIN();
	g_gfx_reg_l8->pause.bwPause = FALSE;
	g_gfx_reg_l8->run.bwRun = FALSE;
	g_gfx_reg_l8->start.bwStart = FALSE;
	g_gfx_reg_l8->halt.bwHalt = TRUE;
	GFX_TRACE_END();
	return 0;
}

int GFX_L8_SwResetCommand(void)
{
	/* do nothing */ return RET_OK;
}

void GFX_L8_GetComQueueStatus(GFX_CMD_QUEUE_CTRL_T *queue)
{
	GFX_TRACE_BEGIN();
	queue->usLine 		= g_gfx_reg_l8->status0.abwProcessingLine;
	queue->bStatus 		= g_gfx_reg_l8->status0.bwCmdStatus;
	queue->usRemainSpace= g_gfx_reg_l8->status1.abwNumOfCmd;
	queue->bFull 		= g_gfx_reg_l8->status1.bwCmdQueueStatus;
	queue->usRemainParam= g_gfx_reg_l8->status2.abwNumOfParam;
	queue->bBatchStatus = g_gfx_reg_l8->status2.bwBatchRunStatus;
	queue->bCmdQStatus	= g_gfx_reg_l8->status1.CmdQueueState;

	memcpy( (void*)&queue->status[0], (void*)&(g_gfx_reg_l8->status0), sizeof(UINT32) );
	memcpy( (void*)&queue->status[1], (void*)&(g_gfx_reg_l8->status1), sizeof(UINT32) );
	memcpy( (void*)&queue->status[2], (void*)&(g_gfx_reg_l8->status2), sizeof(UINT32) );

	GFX_TRACE_END();
}

/**	check if GFX engine is idle or busy state.
 *	this function is used for detecting graphic sync.
 *
 *	@param	none
 *	@return TRUE if GFX is idle, FALSE otherwise
 *	@note check code seems tbe be somewhat hureustric.
 *
 *	@see	GFX_IOW_WAIT_FOR_SYNC
 */
BOOLEAN GFX_L8_IsGfxIdle(void)
{
	BOOLEAN	ret;

	ret = ( g_gfx_reg_l8->status0.bwCmdStatus == 0 && 
			g_gfx_reg_l8->status1.CmdQueueState == 0 )? TRUE : FALSE ;

//	printk("[gfx] state = %s %d-%d\n", (ret)? "<IDLE>":"<BUSY>", 
//				g_gfx_reg_l8->status0.bwCmdStatus, g_gfx_reg_l8->status1.CmdQueueState );

	return ret;
}

void GFX_L8_GetOperationStatus(GFX_CMD_OP_T *operate)
{
	GFX_TRACE_BEGIN();
	operate->sPort = g_gfx_reg_l8->op_mode.abwOpMode;	// pixel format

	operate->bBurstModeEnable = g_gfx_reg_l8->op_mode.bwEnableBusrtMode;		// burst mode enable
	operate->bUpdateCLUT = g_gfx_reg_l8->op_mode.bwEnableCLUTUpdate;
	operate->bChromaFilterEnable = g_gfx_reg_l8->op_mode.bwEnChromaFilter;

	operate->sFmode = g_gfx_reg_l8->op_mode.abwChromaFilterMode;

	operate->usCoef[0] = g_gfx_reg_l8->cflt_coef0.abwCfltCoef0;
	operate->usCoef[1] = g_gfx_reg_l8->cflt_coef0.abwCfltCoef1;
	operate->usCoef[2] = g_gfx_reg_l8->cflt_coef1.abwCfltCoef2;
	operate->usCoef[3] = g_gfx_reg_l8->cflt_coef1.abwCfltCoef3;
	GFX_TRACE_END();
}

void GFX_L8_SetOperationStatus(GFX_CMD_OP_T *operate)
{
	GFX_TRACE_BEGIN();

#if _DIRECT_ACCESS_
	g_gfx_reg_l8->op_mode.abwOpMode = operate->sPort;	// pixel format

	g_gfx_reg_l8->op_mode.bwEnableBusrtMode = operate->bBurstModeEnable;		// burst mode enable
	g_gfx_reg_l8->op_mode.bwEnableCLUTUpdate = operate->bUpdateCLUT;
	g_gfx_reg_l8->op_mode.bwEnChromaFilter = operate->bChromaFilterEnable;

	g_gfx_reg_l8->op_mode.abwChromaFilterMode = operate->sFmode;

	g_gfx_reg_l8->cflt_coef0.abwCfltCoef0 = operate->usCoef[0];
	g_gfx_reg_l8->cflt_coef0.abwCfltCoef1 = operate->usCoef[1];
	g_gfx_reg_l8->cflt_coef1.abwCfltCoef2 = operate->usCoef[2];
	g_gfx_reg_l8->cflt_coef1.abwCfltCoef3 = operate->usCoef[3];
#else
	g_gfx_reg_l8_cache->op_mode.abwOpMode = operate->sPort;	// pixel format
	g_gfx_reg_l8_cache->op_mode.bwEnableBusrtMode = operate->bBurstModeEnable;		// burst mode enable
	g_gfx_reg_l8_cache->op_mode.bwEnableCLUTUpdate = operate->bUpdateCLUT;
	g_gfx_reg_l8_cache->op_mode.bwEnChromaFilter = operate->bChromaFilterEnable;
	g_gfx_reg_l8_cache->op_mode.abwChromaFilterMode = operate->sFmode;

	g_gfx_reg_l8_cache->cflt_coef0.abwCfltCoef0 = operate->usCoef[0];
	g_gfx_reg_l8_cache->cflt_coef0.abwCfltCoef1 = operate->usCoef[1];
	g_gfx_reg_l8_cache->cflt_coef1.abwCfltCoef2 = operate->usCoef[2];
	g_gfx_reg_l8_cache->cflt_coef1.abwCfltCoef3 = operate->usCoef[3];

	GFX_REG_MEMCPY( &g_gfx_reg_l8->op_mode, &g_gfx_reg_l8_cache->op_mode, 3 /* reg count = 3 */ );
#endif
	GFX_TRACE_END();
}

void GFX_L8_GetInputConfigure(int iPort, GFX_PORT_CONFIGURE_T *port)
{
	GFX_TRACE_BEGIN();

	switch(iPort)
	{
		case ePort0:
			port->ulAddr = g_gfx_reg_l8->in0_base_addr;
			port->usStride = g_gfx_reg_l8->in0_stride.abwStride;
			port->sEndian = g_gfx_reg_l8->in0_format.bwEndian;
			port->sFmt = g_gfx_reg_l8->in0_format.abwPft;
			port->uiGlobalAlpha = g_gfx_reg_l8->in0_galpha.abwAlpha;

			port->usAddrCLUT = g_gfx_reg_l8->in0_clut_ctrl.abwAddr;
			port->bRW_CLUT = g_gfx_reg_l8->in0_clut_ctrl.bwRW;
			port->bAuto_Inc_CLUT = g_gfx_reg_l8->in0_clut_ctrl.bwAutoUp;
			port->ulDataCLUT = g_gfx_reg_l8->in0_data_clut;
			port->bColorKeyEnable = g_gfx_reg_l8->in0_ctrl.bwCKeyEnable;
			port->bBitMaskEnable = g_gfx_reg_l8->in0_ctrl.bwBitMaskEnable;
			port->bCOCEnable = g_gfx_reg_l8->in0_ctrl.bwCOCEnable;
			port->bCSCEnable = g_gfx_reg_l8->in0_ctrl.bwCSCEnable;
			port->bColorKeyMode = g_gfx_reg_l8->in0_ctrl.bwCKeyMode;
			port->sCSCSel = g_gfx_reg_l8->in0_ctrl.abwCSCCoefSel;
			port->ulKeyLow = g_gfx_reg_l8->in0_uiCKeyLowThr;
			port->ulKeyHigh = g_gfx_reg_l8->in0_uiCKeyHighThr;
			port->ulReplaceColor = g_gfx_reg_l8->in0_uiReplaceColor;
			port->ulBitMask = g_gfx_reg_l8->in0_uiBitMask;
			port->ulCOCCtrl = g_gfx_reg_l8->in0_coc_ctrl;

		break;

		case ePort1:
			port->ulAddr = g_gfx_reg_l8->in1_base_addr;
			port->usStride = g_gfx_reg_l8->in1_stride.abwStride;
			port->sEndian = g_gfx_reg_l8->in1_format.bwEndian;
			port->sFmt = g_gfx_reg_l8->in1_format.abwPft;
			port->uiGlobalAlpha = g_gfx_reg_l8->in1_galpha.abwAlpha;
			port->usAddrCLUT = g_gfx_reg_l8->in1_clut_ctrl.abwAddr;
			port->bRW_CLUT = g_gfx_reg_l8->in1_clut_ctrl.bwRW;
			port->bAuto_Inc_CLUT = g_gfx_reg_l8->in1_clut_ctrl.bwAutoUp;
			port->ulDataCLUT = g_gfx_reg_l8->in1_data_clut;
			port->bColorKeyEnable = g_gfx_reg_l8->in1_ctrl.bwCKeyEnable;
			port->bBitMaskEnable = g_gfx_reg_l8->in1_ctrl.bwBitMaskEnable;
			port->bCOCEnable = g_gfx_reg_l8->in1_ctrl.bwCOCEnable;
			port->bCSCEnable = g_gfx_reg_l8->in1_ctrl.bwCSCEnable;
			port->bColorKeyMode = g_gfx_reg_l8->in1_ctrl.bwCKeyMode;
			port->sCSCSel = g_gfx_reg_l8->in1_ctrl.abwCSCCoefSel;
			port->ulKeyLow = g_gfx_reg_l8->in1_uiCKeyLowThr;
			port->ulKeyHigh = g_gfx_reg_l8->in1_uiCKeyHighThr;
			port->ulReplaceColor = g_gfx_reg_l8->in1_uiReplaceColor;
			port->ulBitMask = g_gfx_reg_l8->in1_uiBitMask;
			port->ulCOCCtrl = g_gfx_reg_l8->in1_coc_ctrl;
		break;
		case ePort2:
			port->ulAddr = g_gfx_reg_l8->in2_base_addr;
			port->usStride = g_gfx_reg_l8->in2_stride.abwStride;
			port->sEndian = g_gfx_reg_l8->in2_format.bwEndian;
			port->sFmt = g_gfx_reg_l8->in2_format.abwPft;
			port->uiGlobalAlpha = g_gfx_reg_l8->in2_galpha.abwAlpha;

		break;
		default:
			GFX_WARN(" Invalid port number #%d in %s\n", port->sPort, __func__);
		break;
	}

	GFX_TRACE_END();
}

void GFX_L8_SetInputConfigure(GFX_PORT_CONFIGURE_T *port)
{
	GFX_TRACE_BEGIN();

	switch(port->sPort)
	{
		case ePort0:
#if _DIRECT_ACCESS_
			g_gfx_reg_l8->in0_base_addr 			= port->ulAddr ;
			g_gfx_reg_l8->in0_stride.abwStride	= port->usStride;
			g_gfx_reg_l8->in0_format.bwEndian 	= port->sEndian ;
			g_gfx_reg_l8->in0_format.abwPft 		= port->sFmt ;
			g_gfx_reg_l8->in0_galpha.abwAlpha 	= port->uiGlobalAlpha ;

			g_gfx_reg_l8->in0_clut_ctrl.bwRW 		= 1;		/* set to read mode. don't override palette */

			g_gfx_reg_l8->in0_ctrl.bwCKeyEnable =	port->bColorKeyEnable ;
			g_gfx_reg_l8->in0_ctrl.bwBitMaskEnable = port->bBitMaskEnable ;
			g_gfx_reg_l8->in0_ctrl.bwCOCEnable =	port->bCOCEnable ;
			g_gfx_reg_l8->in0_ctrl.bwCSCEnable =   port->bCSCEnable ;
			g_gfx_reg_l8->in0_ctrl.bwCKeyMode =	port->bColorKeyMode ;
			g_gfx_reg_l8->in0_ctrl.abwCSCCoefSel =	port->sCSCSel ;
			g_gfx_reg_l8->in0_uiCKeyLowThr =	port->ulKeyLow ;
			g_gfx_reg_l8->in0_uiCKeyHighThr =   	port->ulKeyHigh ;
			g_gfx_reg_l8->in0_uiReplaceColor =	port->ulReplaceColor;
			g_gfx_reg_l8->in0_uiBitMask =    	port->ulBitMask ;
			g_gfx_reg_l8->in0_coc_ctrl =		port->ulCOCCtrl ;
#else
			g_gfx_reg_l8_cache->in0_base_addr =		port->ulAddr ;
			g_gfx_reg_l8_cache->in0_stride.abwStride =	port->usStride;
			g_gfx_reg_l8_cache->in0_format.bwEndian =	port->sEndian ;
			g_gfx_reg_l8_cache->in0_format.abwPft =	port->sFmt ;
			g_gfx_reg_l8_cache->in0_galpha.abwAlpha = port->uiGlobalAlpha ;

			g_gfx_reg_l8_cache->in0_clut_ctrl.bwRW 		= 1;		/* set to read mode. don't override palette */

			g_gfx_reg_l8_cache->in0_ctrl.bwCKeyEnable =	port->bColorKeyEnable ;
			g_gfx_reg_l8_cache->in0_ctrl.bwBitMaskEnable = port->bBitMaskEnable ;
			g_gfx_reg_l8_cache->in0_ctrl.bwCOCEnable =	port->bCOCEnable ;
			g_gfx_reg_l8_cache->in0_ctrl.bwCSCEnable =   port->bCSCEnable ;
			g_gfx_reg_l8_cache->in0_ctrl.bwCKeyMode =	port->bColorKeyMode ;
			g_gfx_reg_l8_cache->in0_ctrl.abwCSCCoefSel =	port->sCSCSel ;
			g_gfx_reg_l8_cache->in0_uiCKeyLowThr =	port->ulKeyLow ;
			g_gfx_reg_l8_cache->in0_uiCKeyHighThr =   	port->ulKeyHigh ;
			g_gfx_reg_l8_cache->in0_uiReplaceColor =	port->ulReplaceColor;
			g_gfx_reg_l8_cache->in0_uiBitMask =    	port->ulBitMask ;
			g_gfx_reg_l8_cache->in0_coc_ctrl =		port->ulCOCCtrl ;
			GFX_REG_MEMCPY( (void*)(&g_gfx_reg_l8->in0_base_addr) , (void *)(&g_gfx_reg_l8_cache->in0_base_addr), 13);
			//volatile_memcpy((volatile UINT32 *)(&g_gfx_reg_l8->in0_base_addr) , (UINT32 *)(&g_gfx_reg_l8_cache->in0_base_addr) , sizeof(UINT32)*13);
#endif

		break;

		case ePort1:
#if _DIRECT_ACCESS_
			g_gfx_reg_l8->in1_base_addr =		port->ulAddr ;
			g_gfx_reg_l8->in1_stride.abwStride =	port->usStride;
			g_gfx_reg_l8->in1_format.bwEndian =	port->sEndian ;
			g_gfx_reg_l8->in1_format.abwPft =	port->sFmt ;
			g_gfx_reg_l8->in1_galpha.abwAlpha = port->uiGlobalAlpha ;

			g_gfx_reg_l8->in1_clut_ctrl.bwRW 		= 1;		/* set to read mode. don't override palette */

			g_gfx_reg_l8->in1_ctrl.bwCKeyEnable=	port->bColorKeyEnable ;
			g_gfx_reg_l8->in1_ctrl.bwBitMaskEnable = port->bBitMaskEnable ;
			g_gfx_reg_l8->in1_ctrl.bwCOCEnable =	port->bCOCEnable ;
			g_gfx_reg_l8->in1_ctrl.bwCSCEnable =   port->bCSCEnable ;
			g_gfx_reg_l8->in1_ctrl.bwCKeyMode =	port->bColorKeyMode ;
			g_gfx_reg_l8->in1_ctrl.abwCSCCoefSel =	port->sCSCSel ;
			g_gfx_reg_l8->in1_uiCKeyLowThr =	port->ulKeyLow ;
			g_gfx_reg_l8->in1_uiCKeyHighThr =   	port->ulKeyHigh ;
			g_gfx_reg_l8->in1_uiReplaceColor =	port->ulReplaceColor;
			g_gfx_reg_l8->in1_uiBitMask =    	port->ulBitMask ;
			g_gfx_reg_l8->in1_coc_ctrl =		port->ulCOCCtrl ;
#else
			g_gfx_reg_l8_cache->in1_base_addr =		port->ulAddr ;
			g_gfx_reg_l8_cache->in1_stride.abwStride =	port->usStride;
			g_gfx_reg_l8_cache->in1_format.bwEndian =	port->sEndian ;
			g_gfx_reg_l8_cache->in1_format.abwPft =	port->sFmt ;
			g_gfx_reg_l8_cache->in1_galpha.abwAlpha = port->uiGlobalAlpha ;

			g_gfx_reg_l8_cache->in1_clut_ctrl.bwRW 		= 1;		/* set to read mode. don't override palette */

			g_gfx_reg_l8_cache->in1_ctrl.bwCKeyEnable=	port->bColorKeyEnable ;
			g_gfx_reg_l8_cache->in1_ctrl.bwBitMaskEnable = port->bBitMaskEnable ;
			g_gfx_reg_l8_cache->in1_ctrl.bwCOCEnable =	port->bCOCEnable ;
			g_gfx_reg_l8_cache->in1_ctrl.bwCSCEnable =   port->bCSCEnable ;
			g_gfx_reg_l8_cache->in1_ctrl.bwCKeyMode =	port->bColorKeyMode ;
			g_gfx_reg_l8_cache->in1_ctrl.abwCSCCoefSel =	port->sCSCSel ;
			g_gfx_reg_l8_cache->in1_uiCKeyLowThr =	port->ulKeyLow ;
			g_gfx_reg_l8_cache->in1_uiCKeyHighThr =   	port->ulKeyHigh ;
			g_gfx_reg_l8_cache->in1_uiReplaceColor =	port->ulReplaceColor;
			g_gfx_reg_l8_cache->in1_uiBitMask =    	port->ulBitMask ;
			g_gfx_reg_l8_cache->in1_coc_ctrl =		port->ulCOCCtrl ;

			GFX_REG_MEMCPY((void *)(&g_gfx_reg_l8->in1_base_addr) , (void *)(&g_gfx_reg_l8_cache->in1_base_addr) , 13);
			//volatile_memcpy((volatile UINT32 *)(&g_gfx_reg_l8->in1_base_addr) , (UINT32 *)(&g_gfx_reg_l8_cache->in1_base_addr) , sizeof(UINT32)*13);
#endif
		break;
		case ePort2:
#if _DIRECT_ACCESS_
			g_gfx_reg_l8->in2_base_addr = port->ulAddr ;
			g_gfx_reg_l8->in2_stride.abwStride = port->usStride;
			g_gfx_reg_l8->in2_format.bwEndian = port->sEndian ;
			g_gfx_reg_l8->in2_format.abwPft = port->sFmt ;
			g_gfx_reg_l8->in2_galpha.abwAlpha = port->uiGlobalAlpha ;
#else
			g_gfx_reg_l8_cache->in2_base_addr = port->ulAddr ;
			g_gfx_reg_l8_cache->in2_stride.abwStride = port->usStride;
			g_gfx_reg_l8_cache->in2_format.bwEndian = port->sEndian ;
			g_gfx_reg_l8_cache->in2_format.abwPft = port->sFmt ;
			g_gfx_reg_l8_cache->in2_galpha.abwAlpha = port->uiGlobalAlpha ;

			GFX_REG_MEMCPY((void *)(&g_gfx_reg_l8->in2_base_addr) , (void *)(&g_gfx_reg_l8_cache->in2_base_addr), 5);
			//volatile_memcpy((volatile UINT32 *)(&g_gfx_reg_l8->in2_base_addr) , (UINT32 *)(&g_gfx_reg_l8_cache->in2_base_addr) , sizeof(UINT32)*5);
#endif
		break;
		default:
		break;
	}
	GFX_TRACE_END();
}

void GFX_L8_SetBlendingOut(GFX_ENUM_OUT_T type)
{
	GFX_TRACE_BEGIN();
	g_gfx_reg_l8->out_sel.abwOutSel = type;
	GFX_TRACE_END();
}

void GFX_L8_GetBlendConfigure( GFX_BLEND_CONFIGURE_T *blend)
{
	GFX_TRACE_BEGIN();
	blend->sOut 		= g_gfx_reg_l8->out_sel.abwOutSel;
	blend->sAlphaBlend	= g_gfx_reg_l8->out_ctrl0.abwComSelAlpha	;
	blend->sRBlend		= g_gfx_reg_l8->out_ctrl0.abwComSelR;
	blend->sGBlend		= g_gfx_reg_l8->out_ctrl0.abwComSelG;
	blend->sBBlend		= g_gfx_reg_l8->out_ctrl0.abwComSelB;

	blend->usAlpha_m0	= g_gfx_reg_l8->out_ctrl0.abwAlpham0;
	blend->usC_m0		= g_gfx_reg_l8->out_ctrl0.abwCm0;

	blend->bDIVEnable	= g_gfx_reg_l8->out_ctrl0.bwDIVEnable;
	blend->bXOR0Enable	= g_gfx_reg_l8->out_ctrl0.bwXOR0Enable;
	blend->bXOR1Enable	= g_gfx_reg_l8->out_ctrl0.bwXOR1Enable;
	blend->bPMA0Enable	= g_gfx_reg_l8->out_ctrl0.bwPMA0Enable;
	blend->bPMA1Enable	= g_gfx_reg_l8->out_ctrl0.bwPMA1Enable;

	blend->usA0_Alpha	= g_gfx_reg_l8->out_ctrl1.abwA0Sel;
	blend->usB0_Alpha	= g_gfx_reg_l8->out_ctrl1.abwB0Sel;
	blend->usA1_R		= g_gfx_reg_l8->out_ctrl1.abwA1Sel;
	blend->usB1_R		= g_gfx_reg_l8->out_ctrl1.abwB1Sel;
	blend->usA2_G		= g_gfx_reg_l8->out_ctrl1.abwA2Sel;
	blend->usB2_G		= g_gfx_reg_l8->out_ctrl1.abwB2Sel;
	blend->usA3_B		= g_gfx_reg_l8->out_ctrl1.abwA3Sel;
	blend->usB3_B		= g_gfx_reg_l8->out_ctrl1.abwB3Sel;

	blend->ulBlendConstant	= g_gfx_reg_l8->out_const_ctrl.abwConstantCtrl;
	blend->sRaster			= g_gfx_reg_l8->out_rop_ctrl.abwRasterSel;
	GFX_TRACE_END();
}


void GFX_L8_SetBlendConfigure( GFX_BLEND_CONFIGURE_T *blend)
{
	GFX_TRACE_BEGIN();
#if _DIRECT_ACCESS_
	 g_gfx_reg_l8->out_sel.abwOutSel			= blend->sOut ;
	 g_gfx_reg_l8->out_ctrl0.abwComSelAlpha	= blend->sAlphaBlend;
	 g_gfx_reg_l8->out_ctrl0.abwComSelR		= blend->sRBlend;
	 g_gfx_reg_l8->out_ctrl0.abwComSelG		= blend->sGBlend;
	 g_gfx_reg_l8->out_ctrl0.abwComSelB		= blend->sBBlend;

	 g_gfx_reg_l8->out_ctrl0.abwAlpham0		= blend->usAlpha_m0;
	 g_gfx_reg_l8->out_ctrl0.abwCm0			= blend->usC_m0	;

	 g_gfx_reg_l8->out_ctrl0.bwDIVEnable		= blend->bDIVEnable;
	 g_gfx_reg_l8->out_ctrl0.bwXOR0Enable		= blend->bXOR0Enable;
	 g_gfx_reg_l8->out_ctrl0.bwXOR1Enable		= blend->bXOR1Enable;
	 g_gfx_reg_l8->out_ctrl0.bwPMA0Enable		= blend->bPMA0Enable;
	 g_gfx_reg_l8->out_ctrl0.bwPMA1Enable		= blend->bPMA1Enable;

	 g_gfx_reg_l8->out_ctrl1.abwA0Sel		= blend->usA0_Alpha;
	 g_gfx_reg_l8->out_ctrl1.abwB0Sel		= blend->usB0_Alpha;
	 g_gfx_reg_l8->out_ctrl1.abwA1Sel		= blend->usA1_R	;
	 g_gfx_reg_l8->out_ctrl1.abwB1Sel		= blend->usB1_R	;
	 g_gfx_reg_l8->out_ctrl1.abwA2Sel		= blend->usA2_G	;
	 g_gfx_reg_l8->out_ctrl1.abwB2Sel		= blend->usB2_G	;
	 g_gfx_reg_l8->out_ctrl1.abwA3Sel		= blend->usA3_B	;
	 g_gfx_reg_l8->out_ctrl1.abwB3Sel		= blend->usB3_B	;

	 g_gfx_reg_l8->out_const_ctrl.abwConstantCtrl	= blend->ulBlendConstant;
	 g_gfx_reg_l8->out_rop_ctrl.abwRasterSel		= blend->sRaster;
#else
	 g_gfx_reg_l8_cache->out_sel.abwOutSel			= blend->sOut ;
	 g_gfx_reg_l8_cache->out_ctrl0.abwComSelAlpha	= blend->sAlphaBlend;
	 g_gfx_reg_l8_cache->out_ctrl0.abwComSelR		= blend->sRBlend;
	 g_gfx_reg_l8_cache->out_ctrl0.abwComSelG		= blend->sGBlend;
	 g_gfx_reg_l8_cache->out_ctrl0.abwComSelB		= blend->sBBlend;

	 g_gfx_reg_l8_cache->out_ctrl0.abwAlpham0		= blend->usAlpha_m0;
	 g_gfx_reg_l8_cache->out_ctrl0.abwCm0			= blend->usC_m0	;

	GFX_TRACE("%s:%d -- blend->usAlpha_m0 = 0x%x\n", __F__, __L__, blend->usAlpha_m0 );
	GFX_TRACE("%s:%d -- blend->usC_m0     = 0x%x\n", __F__, __L__, blend->usC_m0 );

	 g_gfx_reg_l8_cache->out_ctrl0.bwDIVEnable		= blend->bDIVEnable;
	 g_gfx_reg_l8_cache->out_ctrl0.bwXOR0Enable		= blend->bXOR0Enable;
	 g_gfx_reg_l8_cache->out_ctrl0.bwXOR1Enable		= blend->bXOR1Enable;
	 g_gfx_reg_l8_cache->out_ctrl0.bwPMA0Enable		= blend->bPMA0Enable;
	 g_gfx_reg_l8_cache->out_ctrl0.bwPMA1Enable		= blend->bPMA1Enable;

	 g_gfx_reg_l8_cache->out_ctrl1.abwA0Sel		= blend->usA0_Alpha;
	 g_gfx_reg_l8_cache->out_ctrl1.abwB0Sel		= blend->usB0_Alpha;
	 g_gfx_reg_l8_cache->out_ctrl1.abwA1Sel		= blend->usA1_R	;
	 g_gfx_reg_l8_cache->out_ctrl1.abwB1Sel		= blend->usB1_R	;
	 g_gfx_reg_l8_cache->out_ctrl1.abwA2Sel		= blend->usA2_G	;
	 g_gfx_reg_l8_cache->out_ctrl1.abwB2Sel		= blend->usB2_G	;
	 g_gfx_reg_l8_cache->out_ctrl1.abwA3Sel		= blend->usA3_B	;
	 g_gfx_reg_l8_cache->out_ctrl1.abwB3Sel		= blend->usB3_B	;

	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usA0_Alpha);
	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usB0_Alpha);
	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usA1_R);
	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usA2_G);
	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usA3_B);
	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usB1_R);
	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usB2_G);
	 GFX_TRACE("%s:%d -- %x\n", __F__, __L__, blend->usB3_B);

	g_gfx_reg_l8_cache->out_const_ctrl.abwConstantCtrl	= blend->ulBlendConstant;
	g_gfx_reg_l8_cache->out_rop_ctrl.abwRasterSel		= blend->sRaster;

	GFX_REG_MEMCPY( &g_gfx_reg_l8->out_sel, &g_gfx_reg_l8_cache->out_sel, 5 /* reg count = 5 */ );
#endif
	GFX_TRACE_END();

}

void GFX_L8_GetOutputConfigure(GFX_OUT_CONFIGURE_T *port)
{
	GFX_TRACE_BEGIN();
	port->ulAddr = g_gfx_reg_l8->out_base_addr;		// out base address
	port->usStride = g_gfx_reg_l8->out_stride.abwStride;	// out stride. bytes per line
	port->sEndian = g_gfx_reg_l8->out_format.bwEndian;	// endian
	port->sFmt = g_gfx_reg_l8->out_format.abwPft;	// pixel format
	port->usVSize = g_gfx_reg_l8->out_size.abwVSIZE;
	port->usHSize = g_gfx_reg_l8->out_size.abwHSIZE;

	/*
	port->usVSize = (UINT16)(g_gfx_reg_l8->out_size.abwSIZE&0xffff0000)>>16;
	port->usHSize = (UINT16)(g_gfx_reg_l8->out_size.abwSIZE&0xffff)>>0;
	*/
	port->ulGAlpha = (g_gfx_reg_l8->out_galpha.abwAlpha);
	port->bCSCEnable = g_gfx_reg_l8->out_ctrl.bwCSCEnable;
	port->bCOCEnable = g_gfx_reg_l8->out_ctrl.bwCOCEnable;
	port->ulCOCCtrl = g_gfx_reg_l8->out_coc_ctrl;

	port->uiCSCCoef[0] = g_gfx_reg_l8->out_coef0.abwCSCCoef;
	port->uiCSCCoef[1] = g_gfx_reg_l8->out_coef1.abwCSCCoef;
	port->uiCSCCoef[2] = g_gfx_reg_l8->out_coef2.abwCSCCoef;
	port->uiCSCCoef[3] = g_gfx_reg_l8->out_coef3.abwCSCCoef;
	port->uiCSCCoef[4] = g_gfx_reg_l8->out_coef4.abwCSCCoef;
	port->uiCSCCoef[5] = g_gfx_reg_l8->out_coef5.abwCSCCoef;
	port->uiCSCCoef[6] = g_gfx_reg_l8->out_coef6.abwCSCCoef;
	port->uiCSCCoef[7] = g_gfx_reg_l8->out_coef7.abwCSCCoef;

	GFX_TRACE_END();
}

void GFX_L8_SetOutputConfigure(GFX_OUT_CONFIGURE_T *port)
{
	GFX_TRACE_BEGIN();

#if _DIRECT_ACCESS_
	g_gfx_reg_l8->out_base_addr = port->ulAddr;		// out base address
	g_gfx_reg_l8->out_stride.abwStride = port->usStride;	// out stride. bytes per line
	g_gfx_reg_l8->out_format.bwEndian = port->sEndian;	// endian
	g_gfx_reg_l8->out_format.abwPft = port->sFmt;	// pixel format
	g_gfx_reg_l8->out_size.abwHSIZE = port->usHSize;
	g_gfx_reg_l8->out_size.abwVSIZE = port->usVSize;
	g_gfx_reg_l8->out_galpha.abwAlpha = port->ulGAlpha;
	g_gfx_reg_l8->out_ctrl.bwCSCEnable = port->bCSCEnable;
	g_gfx_reg_l8->out_ctrl.bwCOCEnable = port->bCOCEnable;
	g_gfx_reg_l8->out_coc_ctrl = port->ulCOCCtrl;
	g_gfx_reg_l8->out_coef0.abwCSCCoef = port->uiCSCCoef[0];
	g_gfx_reg_l8->out_coef1.abwCSCCoef = port->uiCSCCoef[1];
	g_gfx_reg_l8->out_coef2.abwCSCCoef = port->uiCSCCoef[2];
	g_gfx_reg_l8->out_coef3.abwCSCCoef = port->uiCSCCoef[3];
	g_gfx_reg_l8->out_coef4.abwCSCCoef = port->uiCSCCoef[4];
	g_gfx_reg_l8->out_coef5.abwCSCCoef = port->uiCSCCoef[5];
	g_gfx_reg_l8->out_coef6.abwCSCCoef = port->uiCSCCoef[6];
	g_gfx_reg_l8->out_coef7.abwCSCCoef = port->uiCSCCoef[7];
#else
	g_gfx_reg_l8_cache->out_base_addr = port->ulAddr;		// out base address
	g_gfx_reg_l8_cache->out_stride.abwStride= port->usStride;	// out stride. bytes per line
	g_gfx_reg_l8_cache->out_format.bwEndian = port->sEndian;	// endian
	g_gfx_reg_l8_cache->out_format.abwPft	= port->sFmt;	// pixel format
	g_gfx_reg_l8_cache->out_size.abwHSIZE	= port->usHSize;
	g_gfx_reg_l8_cache->out_size.abwVSIZE	= port->usVSize;
	g_gfx_reg_l8_cache->out_galpha.abwAlpha	= port->ulGAlpha;
	g_gfx_reg_l8_cache->out_ctrl.bwCSCEnable = port->bCSCEnable;
	g_gfx_reg_l8_cache->out_ctrl.bwCOCEnable = port->bCOCEnable;

//	if ( g_gfx_reg_l8_cache->out_ctrl.bwCSCEnable )
//	{
//		printk("<!> !!!!!!!!! OUT_CSC enable !!!!!!!!!!!\n");
//	}

	g_gfx_reg_l8_cache->out_coc_ctrl		= port->ulCOCCtrl;
	g_gfx_reg_l8_cache->out_coef0.abwCSCCoef = port->uiCSCCoef[0];
	g_gfx_reg_l8_cache->out_coef1.abwCSCCoef = port->uiCSCCoef[1];
	g_gfx_reg_l8_cache->out_coef2.abwCSCCoef = port->uiCSCCoef[2];
	g_gfx_reg_l8_cache->out_coef3.abwCSCCoef = port->uiCSCCoef[3];
	g_gfx_reg_l8_cache->out_coef4.abwCSCCoef = port->uiCSCCoef[4];
	g_gfx_reg_l8_cache->out_coef5.abwCSCCoef = port->uiCSCCoef[5];
	g_gfx_reg_l8_cache->out_coef6.abwCSCCoef = port->uiCSCCoef[6];
	g_gfx_reg_l8_cache->out_coef7.abwCSCCoef = port->uiCSCCoef[7];

	GFX_REG_MEMCPY( (void*)(&g_gfx_reg_l8->out_base_addr), (void *)(&g_gfx_reg_l8_cache->out_base_addr), 15);
#endif
	GFX_TRACE_END();
}

void GFX_L8_SetInterruptMode(UINT32 uiMode)
{
#if 0
	if(uiMode < 6)
	{
		//g_gfx_reg_l8->intr_ctrl &= 0xFFFF0000;
		//g_gfx_reg_l8->intr_ctrl |= uiMode;
		g_gfx_reg_l8->intr_ctrl.abwNumOfCmd = 0;


		switch(uiMode)
		{
			case 0:
				g_gfx_reg_l8->intr_ctrl.abwIntrMode = 0;
			break;
			case 1:
				g_gfx_reg_l8->intr_ctrl.abwIntrMode = 1;
			break;
			case 2:
				g_gfx_reg_l8->intr_ctrl.abwIntrMode = 2;
			break;
			case 3:
				g_gfx_reg_l8->intr_ctrl.abwIntrMode = 0;
			break;
			case 4:
				g_gfx_reg_l8->intr_ctrl.abwIntrMode = 1;
			break;
			case 5:
				g_gfx_reg_l8->intr_ctrl.abwIntrMode = 2;
			break;

		}
	}
#endif
}

void GFX_L8_SetAckMode(UINT32 *mode)
{
	GFX_TRACE_BEGIN();
	guiAckMode = *mode;
	guiAckMode = 1;
	GFX_TRACE_END();
}

void GFX_L8_GetAckMode(UINT32 *mode)
{
	*mode = guiAckMode;
}

int GFX_L8_SetCLUT(int port, int size , UINT32 *data)
{
	int i; 
	int ret = 0;

	GFX_TRACE_BEGIN();

	for ( i=0; i<100; i++ )
	{
		if( GFX_L8_IsGfxIdle() ) break;
		GFX_ERROR("GFX BUSY...\n"); OS_MsecSleep(1);
	}

	switch(port)
	{
		case 0:
		{
			g_gfx_reg_l8->in0_clut_ctrl.abwAddr 	= 0;
			g_gfx_reg_l8->in0_clut_ctrl.bwRW 		= 0;
#ifdef GFX_PALETTE_AUTO_INC
			g_gfx_reg_l8->in0_clut_ctrl.bwAutoUp 	= 1;
#else
			g_gfx_reg_l8->in0_clut_ctrl.bwAutoUp 	= 0;
#endif
			for(i=0 ; i<size ; i++)
			{
#ifdef GFX_PALETTE_AUTO_INC
				/* do nothing */
#else
				g_gfx_reg_l8->in0_clut_ctrl.abwAddr = i;
#endif
				g_gfx_reg_l8->in0_data_clut			= *data++;
			}
		}
		break;

		case 1:
		{
			g_gfx_reg_l8->in1_clut_ctrl.abwAddr 	= 0;
			g_gfx_reg_l8->in1_clut_ctrl.bwRW 		= 0;
#ifdef GFX_PALETTE_AUTO_INC
			g_gfx_reg_l8->in1_clut_ctrl.bwAutoUp 	= 1;
#else
			g_gfx_reg_l8->in1_clut_ctrl.bwAutoUp 	= 0;
#endif
			for(i=0; i <size ; i++)
			{
#ifdef GFX_PALETTE_AUTO_INC
				/* do nothing */
#else
				g_gfx_reg_l8->in1_clut_ctrl.abwAddr	= i;
#endif
				g_gfx_reg_l8->in1_data_clut 		= *data++;
			}
		}
		break;

		default:
			GFX_ERROR("WARNING : Try to download CLUT at wrong port\n");
		break;
	}

	udelay(10); /* wait for gfx sync */

	GFX_TRACE_END();
	return ret;
}

/**
 *
 * command delay register setting function
 *
 * @param	delay count
 * @return	void
 *
 */
void GFX_L8_SetCommandDelayReg(UINT16 delay)
{
	GFX_TRACE_BEGIN();
	GFX_TRACE("%s:%d -- cmd delay = 0x%x\n", __F__, __L__, delay );
	g_gfx_reg_l8->cmd_delay.abwCmdDelay = delay;
	GFX_TRACE_END();
}
/**
 *
 * command delay register getting function
 *
 * @param	void
 * @return	delay count
 *
 * @ingroup osd_ddi_func
*/
UINT16 GFX_L8_GetCommandDelayReg(void)
{
	UINT16	val;
	GFX_TRACE_BEGIN();
	val = g_gfx_reg_l8->cmd_delay.abwCmdDelay;
	GFX_TRACE("%s:%d -- cmd delay = 0x%x\n", __F__, __L__, val );
	GFX_TRACE_END();
	return val;
}

/**
 *
 * register setting function
 *
 * @param	variable, address
 * @return	void
 *
 * @ingroup osd_ddi_func
*/
void GFX_L8_SetRegister(UINT32 addr, UINT32 val)
{
	GFX_TRACE_BEGIN();
	__raw_writel(val , (ULONG)(g_gfx_reg_l8) + (ULONG)addr );
	GFX_TRACE_END();
}

UINT32 GFX_L8_GetRegister(UINT32 addr)
{
	return __raw_readl( (ULONG)(g_gfx_reg_l8) + (ULONG)addr);
}

void GFX_L8_DumpRegister(void)
{
	/* do nothing */
}

int GFX_L8_RunSuspend(void)
{
#ifdef KDRV_CONFIG_PM
	gPmCmdDelay = g_gfx_reg_l8->cmd_delay.abwCmdDelay;
	gPmIntrCtrl = g_gfx_reg_l8->intr_ctrl.bwIntEnable;

	return 0;
#else
	return RET_NOT_SUPPORTED;
#endif
}

int GFX_L8_RunResume(void)
{
#ifdef KDRV_CONFIG_PM
	// restore cmd delay
	g_gfx_reg_l8->cmd_delay.abwCmdDelay = gPmCmdDelay;
	// interrupt enable
	g_gfx_reg_l8->intr_ctrl.bwIntEnable = gPmIntrCtrl;

	// restore input
	memcpy((void *)(&g_gfx_reg_l8->in0_base_addr) , (void *)(&g_gfx_reg_l8_cache->in0_base_addr) , sizeof(UINT32)*13);
	memcpy((void *)(&g_gfx_reg_l8->in1_base_addr) , (void *)(&g_gfx_reg_l8_cache->in1_base_addr) , sizeof(UINT32)*13);
	memcpy((void *)(&g_gfx_reg_l8->in2_base_addr) , (void *)(&g_gfx_reg_l8_cache->in2_base_addr) , sizeof(UINT32)*5);
	GFX_RunFlushCommand();
	// srestore output
	memcpy((void *)(&g_gfx_reg_l8->out_base_addr) , (void *)(&g_gfx_reg_l8_cache->out_base_addr) , sizeof(UINT32)*15);
	// restore blend
	memcpy((void *)(&g_gfx_reg_l8->out_sel) , (void *)(&g_gfx_reg_l8_cache->out_sel) , sizeof(UINT32)*5);
	// restore op mode
	memcpy((void *)(&g_gfx_reg_l8->op_mode) , (void *)(&g_gfx_reg_l8_cache->op_mode) , sizeof(UINT32)*3);
	GFX_RunFlushCommand();
	return 0;
#else
	return RET_NOT_SUPPORTED;
#endif
}


/** @} */

