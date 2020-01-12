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
 * date       2010.02.17
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
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>

#include "base_types.h"
#include "os_util.h"

#include "de_ver_def.h"

#include "de_kapi.h"
#include "de_def.h"

#include "de_int_def.h"
#include "de_hal_def.h"
#include "l8/de_ipc_def_l8.h"
#include "l8/de_reg_def_l8.h"
#include "l8/de_reg_l8.h"
#include "l8/de_top_reg_l8.h"
#include "de_ipc.h"
#include "de_int.h"
#ifdef USE_KDRV_CODES_FOR_L9
#include "l9/de_reg_l9.h"
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
typedef struct {
	int (*cb_func)(void);
} DE_VSYNC_CB;
DE_VSYNC_CB vsync_cb;

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
static DECLARE_TASKLET(Video_WDG_tasklet,	VIDEO_CPU_WDG_FROM_MCU,  0UL);
static DECLARE_TASKLET(Video_IPC_tasklet,	VIDEO_CPU_ISR_FROM_MCU,  0UL);
static DECLARE_TASKLET(Video_DMA_tasklet,	VIDEO_CPU_DMA_FROM_MCU,  0UL);
static DECLARE_TASKLET(Video_JPG_tasklet,	VIDEO_CPU_JPG_FROM_MCU,  0UL);
static DECLARE_TASKLET(Video_USB_tasklet,	VIDEO_CPU_USB_FROM_MCU,  0UL);
static DECLARE_TASKLET(Video_VSYNC_tasklet, VIDEO_CPU_VSYNC_Tasklet, 0UL);

static DECLARE_WAIT_QUEUE_HEAD(wqVsync);
static int de_vsync_event_wakeup;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
/**
 * @callgraph
 * @callergraph
 *
 * @brief interrupt handler which is comming from MCU
 */
void DE_IPC_Handler(void)
{
	UINT32 ipcStatus;

	VIDEO_IPC_GetStatus(VIDEO_IPC_CPU, &ipcStatus);
	if (GET_CM(ipcStatus, VIDEO_WDG_INTERRUPT_ARM_MASK)) {
		VIDEO_CPU_ClearWDG();
		tasklet_schedule(&Video_WDG_tasklet);
	}
	if (GET_CM(ipcStatus, VIDEO_IPC_INTERRUPT_ARM_MASK)) {
		VIDEO_CPU_ClearIPC();
		tasklet_schedule(&Video_IPC_tasklet);
	}
	if (GET_CM(ipcStatus, VIDEO_DMA_INTERRUPT_ARM_MASK)) {
		VIDEO_CPU_ClearDMA();
		tasklet_schedule(&Video_DMA_tasklet);
	}
	if (GET_CM(ipcStatus, VIDEO_JPG_INTERRUPT_ARM_MASK)) {
		VIDEO_CPU_ClearJPG();
		tasklet_schedule(&Video_JPG_tasklet);
	}
	if (GET_CM(ipcStatus, VIDEO_USB_INTERRUPT_ARM_MASK)) {
		VIDEO_CPU_ClearUSB();
		tasklet_schedule(&Video_USB_tasklet);
	}
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Get interrupt status for IPC
 *
 * @param ipcType [IN] interrupt type for IPC which is happend from either MCU or CPU
 * @param pStatus [OUT] interrupt status for IPC
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_IPC_GetStatus(VIDEO_IPC_TYPE_T ipcType, UINT32 *pStatus)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DE_IPC_L9_GetStatusReg(ipcType, pStatus);
	} else
#endif
	{
		ret = VIDEO_IPC_GetStatusReg(ipcType, pStatus);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Clear Inerrupt status of CPU for IPC
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_CPU_ClearIPC(void)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
	{
		ret = DE_IPC_L9_WakeUpReg(VIDEO_IPC_CPU, FALSE);
	} else
#endif
	{
		ret = VIDEO_IPC_WakeUpReg(VIDEO_IPC_CPU, FALSE);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Clear Inerrupt status of JPG for IPC
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_CPU_ClearJPG(void)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
	{
		ret = DE_IPC_L9_WakeUpReg(VIDEO_JPG_CPU, FALSE);
	}
#endif

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Clear Inerrupt status of USB for IPC
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_CPU_ClearUSB(void)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
	{
		ret = DE_IPC_L9_WakeUpReg(VIDEO_USB_CPU, FALSE);
	}
#endif

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Clear Inerrupt status of CPU for watch dog
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_CPU_ClearWDG(void)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DE_WDG_L9_WakeUpReg(FALSE);
	} else
#endif
	{
		ret = VIDEO_WDG_WakeUpReg(FALSE);
	}

	return ret;
}

/**
 * @callgraph
 * @callergraph
 *
 * @brief Clear Inerrupt status of CPU for DMA
 *
 * @return RET_OK(0) if success, none zero for otherwise
 */
int VIDEO_CPU_ClearDMA(void)
{
	int ret = RET_OK;

#ifdef USE_KDRV_CODES_FOR_L9
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
		ret = DE_DMA_L9_WakeUpReg(FALSE);
	} else
#endif
	{
		ret = VIDEO_DMA_WakeUpReg(FALSE);
	}

	return ret;
}

irqreturn_t DE_IPC_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    DE_IPC_Handler();

	return IRQ_HANDLED;
}

irqreturn_t DE_VSYNC_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    DE_VSYNC_Handler();

	return IRQ_HANDLED;
}

void DE_VSYNC_Handler(void)
{
	do {
#ifdef USE_KDRV_CODES_FOR_L9
		if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) ) {
			if (!DE_REG_L9_CheckIrq4Vsync()) break;
		} else
#endif
		{
			if (!DE_REG_CheckIrq4Vsync()) break;
		}
		tasklet_schedule(&Video_VSYNC_tasklet);
	} while (0);
}

void DE_VSYNC_Register(int (*func)(void))
{
	vsync_cb.cb_func = func;
}
EXPORT_SYMBOL(DE_VSYNC_Register);

void DE_VSYNC_Callback(void)
{
	vsync_cb.cb_func();
}

void VIDEO_CPU_VSYNC_Tasklet(unsigned long temp)
{
	DE_VSYNC_WakeUp();
	DE_VSYNC_Callback();
}

void DE_VSYNC_WakeUp(void)
{
	if (de_vsync_event_wakeup < 0) de_vsync_event_wakeup++;
	wake_up_interruptible(&wqVsync);
}

int DE_VSYNC_WaitTimeOut(int timeout)
{
	if (timeout < 0) {
		de_vsync_event_wakeup = timeout;
		wait_event_interruptible(wqVsync, !de_vsync_event_wakeup);
	} else {
		de_vsync_event_wakeup = -1;
		wait_event_interruptible_timeout(wqVsync, !de_vsync_event_wakeup, (long)timeout);
	}
	return RET_OK;
}
/** @} */
