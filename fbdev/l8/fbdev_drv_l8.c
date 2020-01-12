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
#include "fbdev_impl.h"
#include "fbdev_hal.h"
#include "fbdev_reg_l8.h"
#include <linux/workqueue.h>		/**< For working queue */
#include <linux/interrupt.h>
#include <linux/irq.h>

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
extern int FBDEV_L8_RunSuspend(void);
extern int FBDEV_L8_RunResume(void);

extern int FBDEV_L8_InitHW		          	(void);
extern int FBDEV_L8_ShutdownHW				(void);
extern int FBDEV_L8_SetOSDEnable			(int layer , int enable);
extern int FBDEV_L8_GetOSDEnable			(int layer);
extern int FBDEV_L8_InitOSDLayer			(int layer);
//extern int FBDEV_L8_WriteOSDCfg				(LAYER_INFORM_T *inform);
extern int FBDEV_L8_WriteOSDHeader			(struct scanvas  *pstCanvas);

extern int FBDEV_L8_GetZList              (LX_FBDEV_ZLIST_T* zList );
extern int FBDEV_L8_SetZList              (LX_FBDEV_ZLIST_T* zList );

extern int FBDEV_L8_WriteOSDEndian        (int layer , ENDIAN_INFORM_T *endian);
extern int FBDEV_L8_WriteOSDCSC           (int layer , CSC_INFORM_T *csc);
extern int FBDEV_L8_GetInterruptStatus    (INTERRUPT_INFORM_T intr);
extern int FBDEV_L8_SetInterruptClear     (INTERRUPT_INFORM_T intr);
extern int FBDEV_L8_SetInterruptEnable    (INTERRUPT_INFORM_T intr , int enable);
extern int FBDEV_L8_DownloadPalette       (unsigned int addr , unsigned int size , void *data);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
LX_FBDEV_CFG_T*	FBDEV_L8_GetCfg		(void);
irqreturn_t 	FBDEV_L8_ISRHandler	(int irq, void *dev_id, struct pt_regs *regs);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static	FBDEV_HAL_T		g_fbdev_hal_l8 =
{
	.GetCfg			= FBDEV_L8_GetCfg,
	.RunSuspend		= FBDEV_L8_RunSuspend,
	.RunResume		= FBDEV_L8_RunResume,

	.InitHW			= FBDEV_L8_InitHW,
	.ShutdownHW		= FBDEV_L8_ShutdownHW,

	.SetOSDEnable	= FBDEV_L8_SetOSDEnable,
	.GetOSDEnable	= FBDEV_L8_GetOSDEnable,

	.InitOSDLayer	= FBDEV_L8_InitOSDLayer,
	.WriteOSDHeader	= FBDEV_L8_WriteOSDHeader,

	.GetZList		= FBDEV_L8_GetZList,
	.SetZList		= FBDEV_L8_SetZList,
	.SetVideoMixerCtrl	= NULL,

	.WriteOSDEndian	= FBDEV_L8_WriteOSDEndian,
	.WriteOSDCSC	= FBDEV_L8_WriteOSDCSC,

	.DownloadPalette	= FBDEV_L8_DownloadPalette,

	.GetInterruptStatus	= FBDEV_L8_GetInterruptStatus,
	.SetInterruptClear	= FBDEV_L8_SetInterruptClear,
	.SetInterruptEnable	= FBDEV_L8_SetInterruptEnable,
};

/*========================================================================================
    Implementation Group
========================================================================================*/

/** get L9 specific configuration
 *
 *  @return LX_FBDEV_CFG_T
 */
LX_FBDEV_CFG_T*   FBDEV_L8_GetCfg(void)
{
    return NULL;
}

void	FBDEV_L8_InitHAL( FBDEV_HAL_T*	hal )
{
	memcpy( hal, &g_fbdev_hal_l8, sizeof(FBDEV_HAL_T));
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


/** @} */

