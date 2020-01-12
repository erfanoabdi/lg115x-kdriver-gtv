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
 *	kdriver main
 *
 *  author      daeyoung lim ( raxis.lim@lge.com )
 *  version		1.0
 *  date		2009.11.04
 *
 *  @addtogroup lg1150_base
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/mm.h>		// si_meminfo()
#include "base_drv.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
//semaphore
OS_SEM_T	g_Base_AVLipsync_Sema;

#define BASE_AVLIPSYNC_LOCK_INIT()	OS_InitMutex(&g_Base_AVLipsync_Sema, OS_SEM_ATTR_DEFAULT)


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern void	BASE_DEV_InitCfg(void);
extern void	BASE_DEV_Init(void);
extern void BASE_DEV_Cleanup(void);
extern void	BASE_PROC_Init(void);
extern void BASE_MEMCFG_Init(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 * initialize base device.
 * if global link mode, this device calls all init function of each module.
 *
*/
int
BASE_Init(void)
{
	/* initialize base system */
	OS_CORE_Init( );
	DBG_MSG("<!> %s : BEGIN\n", __FUNCTION__);
	BASE_DEV_InitCfg( );			// init config
	BASE_MEMCFG_Init( );			// init memcfg resource
	BASE_MEMCFG_Print(NULL, NULL, 0);

	OS_DEBUG_Init( );		//DBG_MSG("<!> debug  system initialized..\n");
	OS_PROC_Init( );		//DBG_MSG("<!> proc   system initialized..\n");
	BASE_PROC_Init( );
	OS_CORE_InitDebug( );

#ifdef USE_QEMU_SYSTEM
	/* initialize QEMU system for board simulation */
	QEMU_Init( );			DBG_MSG("<!> qemu   system initialized..\n");
#endif

	/* initialize custom board */
	BASE_DEV_Init( );

	/* AV lipsync Lock Init */
	BASE_AVLIPSYNC_LOCK_INIT();

	DBG_MSG("<!> %s : END\n", __FUNCTION__);

	return 0;
}

/**
 * cleanup base device.
 * if global link mode, this function calls all cleanup function of each module.
*/
static void
BASE_Cleanup(void)
{
	DBG_MSG("<!> %s : BEGIN\n", __FUNCTION__);

	/* cleanup custom board */
	BASE_DEV_Cleanup( );

	DBG_MSG("<!> custom board  cleanup..\n");

#ifdef USE_QEMU_SYSTEM
	/* cleanup QEMU emulation */
	QEMU_Cleanup( );		DBG_MSG("<!> qemu  system cleanup..\n");
#endif

	/* cleanup all the base system */
	OS_PROC_Cleanup( );		DBG_MSG("<!> proc  system cleanup..\n");
	OS_DEBUG_Cleanup( );	DBG_MSG("<!> debug system cleanup..\n");
	OS_CORE_Cleanup( );

	DBG_MSG("<!> %s : END\n", __FUNCTION__);
}

/////////////////////////////////////////////////////////////////////////////////////////
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",BASE_Init);
#else
module_init(BASE_Init);
#endif
module_exit(BASE_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("lg1150 base driver");
//MODULE_LICENSE("LGE");
MODULE_LICENSE("GPL");

/** @} */

