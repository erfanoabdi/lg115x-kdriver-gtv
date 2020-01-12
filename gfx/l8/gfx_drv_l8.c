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
 *  Brief description. 
 *  Detailed description starts here. 
 *
 *  @author		raxis.lim
 *  @version	1.0 
 *  @date		2011-04-03
 *  @note		Additional information. 
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>			/**< printk() */
#include <linux/slab.h>			 	/**< kmalloc() */
#include <linux/fs.h> 				/**< everything\ldots{} */
#include <linux/types.h>		 	/**< size_t */
#include <linux/fcntl.h>			/**< O_ACCMODE */
#include <asm/uaccess.h>
#include <linux/ioport.h>			/**< For request_region, check_region etc */
#include <asm/io.h>					/**< For ioremap_nocache */
#include <linux/workqueue.h>		/**< For working queue */
#include <linux/interrupt.h>
#include <linux/irq.h>

#include "gfx_impl.h"
#include "gfx_reg_l8.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	GFX_REG_BASE_ADDR 		0xc0027000
#define	GFX_REG_SIZE 			0x1DF

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern void GFX_L8_stop(void);
extern int GFX_L8_RunFlushCommand(void);
extern int GFX_L8_RunStartCommand(void);
extern int GFX_L8_PauseCommand(void);
extern int GFX_L8_ResumeCommand(void);
extern int GFX_L8_StopCommand(void);
extern int GFX_L8_SwResetCommand(void);
extern void GFX_L8_GetComQueueStatus(GFX_CMD_QUEUE_CTRL_T *queue);
extern BOOLEAN GFX_L8_IsGfxIdle(void);
extern void GFX_L8_GetOperationStatus(GFX_CMD_OP_T *operate);
extern void GFX_L8_SetOperationStatus(GFX_CMD_OP_T *operate);
extern void GFX_L8_GetInputConfigure(int iPort, GFX_PORT_CONFIGURE_T *port);
extern void GFX_L8_SetInputConfigure(GFX_PORT_CONFIGURE_T *port);
extern void GFX_L8_SetBlendingOut(GFX_ENUM_OUT_T type);
extern void GFX_L8_GetBlendConfigure( GFX_BLEND_CONFIGURE_T *blend);
extern void GFX_L8_SetBlendConfigure( GFX_BLEND_CONFIGURE_T *blend);
extern void GFX_L8_GetOutputConfigure(GFX_OUT_CONFIGURE_T *port);
extern void GFX_L8_SetOutputConfigure(GFX_OUT_CONFIGURE_T *port);
extern void GFX_L8_SetInterruptMode(UINT32 uiMode);
extern void GFX_L8_SetAckMode(UINT32 *mode);
extern void GFX_L8_GetAckMode(UINT32 *mode);
extern int GFX_L8_SetCLUT(int port, int size , UINT32 *data);
extern void GFX_L8_SetCommandDelayReg(UINT16 delay);
extern UINT16 GFX_L8_GetCommandDelayReg(void);
extern void GFX_L8_SetRegister(UINT32 addr, UINT32 val);
extern UINT32 GFX_L8_GetRegister(UINT32 addr);
extern void GFX_L8_DumpRegister(void);
extern int GFX_L8_RunSuspend(void);
extern int GFX_L8_RunResume(void);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
volatile GFX_REG_MAP_T*	g_gfx_reg_l8;
GFX_REG_MAP_T*			g_gfx_reg_l8_cache;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
const LX_GFX_CFG_T*	GFX_L8_GetCfg		(void);
int 				GFX_L8_InitHW		(void);
int 				GFX_L8_ShutdownHW	(void);
irqreturn_t		 	GFX_L8_ISRHandler	(int irq, void *dev_id, struct pt_regs *regs);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static	GFX_HAL_T		g_gfx_hal_l8 =
{
	.GetCfg					= GFX_L8_GetCfg,
	.InitHW					= GFX_L8_InitHW,
	.ShutdownHW				= GFX_L8_ShutdownHW,
	.RunSuspend				= GFX_L8_RunSuspend,
	.RunResume				= GFX_L8_RunResume,

	.RunFlushCommand		= GFX_L8_RunFlushCommand,
	.RunStartCommand		= GFX_L8_RunStartCommand,
	.RunPauseCommand		= GFX_L8_PauseCommand,
	.RunResumeCommand		= GFX_L8_ResumeCommand,
	.RunStopCommand			= GFX_L8_StopCommand,
	.RunSwResetCommand		= GFX_L8_SwResetCommand,

	.GetComQueueStatus		= GFX_L8_GetComQueueStatus,

	.GetOperationStatus		= GFX_L8_GetOperationStatus,
	.SetOperationStatus		= GFX_L8_SetOperationStatus,
	.GetInputConfigure		= GFX_L8_GetInputConfigure,
	.SetInputConfigure		= GFX_L8_SetInputConfigure,
	.GetBlendConfigure		= GFX_L8_GetBlendConfigure,
	.SetBlendConfigure		= GFX_L8_SetBlendConfigure,
	.SetBlendingOut			= GFX_L8_SetBlendingOut,
	.GetOutputConfigure		= GFX_L8_GetOutputConfigure,
	.SetOutputConfigure		= GFX_L8_SetOutputConfigure,
	.SetScalerConfigure		= NULL,
	.SetCLUT				= GFX_L8_SetCLUT,
	.SetCommandDelayReg		= GFX_L8_SetCommandDelayReg,
	.GetCommandDelayReg		= GFX_L8_GetCommandDelayReg,

	.IsGfxIdle				= GFX_L8_IsGfxIdle,

	.SetRegister			= GFX_L8_SetRegister,
	.GetRegister			= GFX_L8_GetRegister,
	.DumpRegister			= GFX_L8_DumpRegister,
};

static  LX_GFX_CFG_T    g_gfx_cfg_l8 =
{
    .b_hw_scaler = FALSE,
	.surface_blit_cmd_delay = 0x40,	//0x40  clk
	.screen_blit_cmd_delay	= 0x100,//0x100 clk
    .sync_wait_timeout      = 200,	//200 ms
    .sync_fail_retry_count  = 2,
};

/*========================================================================================
    Implementation Group
========================================================================================*/

/** get L9 specific configuration
 *
 *  @return LX_GFX_CFG_T
 */
const LX_GFX_CFG_T*   GFX_L8_GetCfg(void)
{
    return &g_gfx_cfg_l8;
}

void	GFX_L8_InitHAL( GFX_HAL_T*	hal )
{
	memcpy( hal, &g_gfx_hal_l8, sizeof(GFX_HAL_T));
}

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/

/** GFX interrupt handler
 *
 */
irqreturn_t GFX_L8_ISRHandler(int irq, void *dev_id, struct pt_regs *regs)
{
	if ( GFX_IsGfxIdle() )
	{
		GFX_PRINT("#### GFX ISR - IDLE OK ####\n");
		GFX_SetSyncWakeup();
	}

	g_gfx_reg_l8->intr_clear.bwIntrClear = 0x1;
	GFX_PRINT("#### GFX ISR ####\n");

	return IRQ_HANDLED;
}

/** initialize L8 hardware
*
* @return RET_OK when success, RET_ERROR otherwise
*/

int GFX_L8_InitHW(void)
{
    g_gfx_reg_l8 =(GFX_REG_MAP_T*)ioremap( gpGfxRegCfg->reg_base_addr, GFX_REG_SIZE);
    GFX_CHECK_CODE( g_gfx_reg_l8 == NULL, return RET_ERROR, "can't alloc memory for regs\n");

    g_gfx_reg_l8_cache = (GFX_REG_MAP_T *)OS_Malloc( gpGfxRegCfg->reg_size );
    GFX_CHECK_CODE( g_gfx_reg_l8_cache == NULL, return RET_ERROR, "out of memory\n");

    memset( g_gfx_reg_l8_cache, 0x0, gpGfxRegCfg->reg_size );

    /* register GFX interrupt handler */
    GFX_CHECK_CODE( request_irq( gpGfxRegCfg->irq_num, (irq_handler_t)GFX_L8_ISRHandler, 0, "gfx_irq", NULL), /* nop */, "request irq failed\n" );

	g_gfx_reg_l8->intr_ctrl.abwIntrMode = 0;	// batch command finish
	g_gfx_reg_l8->intr_ctrl.bwIntEnable = 1;	// interrupt enable

	return 0;
}

/** shutdown L8 hardware 
 *
 *
 * @return RET_OK when success, RET_ERROR otherwise
 */
int 			GFX_L8_ShutdownHW	(void)
{
	g_gfx_reg_l8->intr_ctrl.bwIntEnable = 0;	// interrupt disable

	/* [TODO] more cleanup !!! */

    iounmap((void *)g_gfx_reg_l8);

	return 0;
}

/** @} */

