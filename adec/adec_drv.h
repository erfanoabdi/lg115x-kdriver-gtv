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
 *  driver interface header for adec device. ( used only within kdriver )
 *	adec device will teach you how to make device driver with new platform.
 *
 *  @author		jongsang.oh (jongsang.oh@lge.com)
 *  @version	1.0
 *  @date		2009.12.30
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

#ifndef	_ADEC_DRV_H_
#define	_ADEC_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/interrupt.h>    /**< For isr */
#include <linux/irq.h>			/**< For isr */
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <linux/rmap.h>
#include <linux/kthread.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <linux/poll.h>

#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif

#include "os_util.h"
#include "base_dev_cfg.h"
#include "debug_util.h"
#include "adec_kapi.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define	ADEC_PRINT(format, args...)		DBG_PRINT( g_adec_debug_fd, 0, format, ##args)
#define	ADEC_TRACE(format, args...)		DBG_PRINT( g_adec_debug_fd, 1, format, ##args)
#define	ADEC_ISR_DEBUG(format, args...)	DBG_PRINT( g_adec_debug_fd, 2, format, ##args)
#define	ADEC_MIP_DEBUG(format, args...)	DBG_PRINT( g_adec_debug_fd, 3, format, ##args)
#define	ADEC_ERROR(format, args...)		DBG_PRINT( g_adec_debug_fd, 4, format, ##args)
#define	ADEC_DEBUG(format, args...)		DBG_PRINT( g_adec_debug_fd, 5, format, ##args)
#define	ADEC_DEBUG_TMP(format, args...)	DBG_PRINT( g_adec_debug_fd, 6, format, ##args)
#define	ADEC_IPC_DEBUG(format, args...)	DBG_PRINT( g_adec_debug_fd, 7, format, ##args)
#define ADEC_IPC_MSG(format, args...)	DBG_PRINT( g_adec_debug_fd, 8, format, ##args)
#define	ADEC_MIP_I2C(format, args...)	DBG_PRINT( g_adec_debug_fd, 9, format, ##args)

#define	ADEC_TRACE_BEGIN()				ADEC_TRACE("[ADEC] BEGIN -- %s:%d\n", __FUNCTION__, __LINE__ )
#define	ADEC_TRACE_END()				ADEC_TRACE("[ADEC] END   -- %s:%d\n", __FUNCTION__, __LINE__ )
#define	ADEC_TRACE_MARK()				ADEC_TRACE("[ADEC] %s:%d\n -- TRACING...", __FUNCTION__, __LINE__ )

#define	ADEC_KDRV_Message(level, format, args...)	DBG_PRINT( g_adec_debug_fd, level, format "\n", ##args)
#define NORMAL		0
#define _TRACE		1
#define NORMAL1		2
#define NORMAL2		3
#define ERROR		4
#define WARNING		5
#define NORMAL3		6
#define NORMAL4		7
#define NORMAL5		8
#define NORMAL6		9


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern	int      ADEC_Init(void);
extern	void     ADEC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int		g_adec_debug_fd;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _ADEC_DRV_H_ */

/** @} */
