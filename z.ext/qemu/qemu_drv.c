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
 *  qemu emulation
 *
 *  author		dae-young lim (raxis@lge.com)
 *  version	1.0
 *  @date		2009.11.27
 *
 *  @addtogroup lg1150_qemu
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_drv.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	TIMETICK	10			/* approximately 100 msec */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
/**
 *	main control block for qemu device.
 *	each minor device has unique control block
 *
 */
typedef struct QEMU_DEVICE_t
{
// BEGIN of common device
	int						dev_open_count;		///< check if device is opened or not
	dev_t					devno;			///< device number
	struct cdev				cdev;			///< char device structure
// END of command device

// BEGIN of device specific data


	OS_SEM_T				sem;			///< test semaphore ( each minor device has unique semaphore )
// END of device specific data
}
QEMU_DEVICE_T;

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		QEMU_Init(void);
void	QEMU_Cleanup(void);
void	QEMU_StartTick(void);
void	QEMU_StopTick(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_qemu_debug_fd;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static void _QEMU_MainTimeTick(unsigned long param);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static struct timer_list _g_qemu_timer;

/*========================================================================================
	Implementation Group
========================================================================================*/
int 	QEMU_Init(void)
{
	//int			i;
	//int			err;

	g_qemu_debug_fd = DBG_OPEN( "qemu" );
	OS_DEBUG_EnableModule ( g_qemu_debug_fd );

	OS_DEBUG_EnableModuleByIndex ( g_qemu_debug_fd, 0, DBG_COLOR_CYAN );
	OS_DEBUG_EnableModuleByIndex ( g_qemu_debug_fd, 1, DBG_COLOR_CYAN );
	OS_DEBUG_EnableModuleByIndex ( g_qemu_debug_fd, 2, DBG_COLOR_CYAN );
	OS_DEBUG_EnableModuleByIndex ( g_qemu_debug_fd, 3, DBG_COLOR_CYAN );

	/* create timer */
    init_timer(&_g_qemu_timer);
    _g_qemu_timer.function 	= _QEMU_MainTimeTick;
    _g_qemu_timer.data 		= 0;

	return 0;
}

void 	QEMU_Cleanup(void)
{
	del_timer_sync (&_g_qemu_timer);
}

void	QEMU_StartTick ( void )
{
    _g_qemu_timer.expires = jiffies + TIMETICK;
    add_timer(&_g_qemu_timer);
}

void	QEMU_StopTick	( void )
{

}

////////////////////////////////////////////////////////////////////////////////////////
static void _QEMU_MainTimeTick(unsigned long param)
{
    _g_qemu_timer.expires += TIMETICK;
    add_timer(&_g_qemu_timer);
}


////////////////////////////////////////////////////////////////////////////////////////
//
//	QEMU simulation
//
////////////////////////////////////////////////////////////////////////////////////////
/* i2c driver uses get_timer_val() function */
unsigned int get_timer_val(void)
{
	return 0x0;
}
