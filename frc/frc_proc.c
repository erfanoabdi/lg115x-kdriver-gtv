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
 *  Linux proc interface for frc device.
 *	frc device will teach you how to make device driver with new platform.
 *
 *  author		sh.myoung (sh.myoung@lge.com)
 *  version		1.0 
 *  date		2010.01.06
 *  note		Additional information. 
 *
 *  @addtogroup lg1150_frc 
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
#include <asm/uaccess.h>
#include "frc_drv.h"
#include "proc_util.h"
#include "debug_util.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
enum {
	PROC_ID_AUTHOR	= 0,
	PROC_ID_COMMAND,
	PROC_ID_MAX,
};

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static OS_PROC_DESC_TABLE_T	_g_frc_device_proc_table[] =
{
	{ "author",		PROC_ID_AUTHOR  , OS_PROC_FLAG_READ },
	{ "command",	PROC_ID_COMMAND , OS_PROC_FLAG_WRITE },
	{ NULL, 		PROC_ID_MAX		, 0 }
};

/*========================================================================================
	Implementation Group
========================================================================================*/

/*
 * read_proc implementation of frc device 
 *
*/
static int	_FRC_ReadProcFunction(	UINT32 procId, char* buffer )
{
	int		ret;

	/* TODO: add your proc_write implementation */
	switch( procId )
	{
		case PROC_ID_AUTHOR:
		{
			ret = sprintf( buffer, "%s\n", "sh.myoung (sh.myoung@lge.com)" );
		}
		break;	
		
		default:
		{
			ret = sprintf( buffer, "%s(%d)\n", "unimplemented read proc", procId );
		}
	}
		
	return ret;
}

/*
 * write_proc implementation of frc device 
 *
*/
static int _FRC_WriteProcFunction( UINT32 procId, char* command )
{
	/* TODO: add your proc_write implementation */	
	switch( procId )
	{
		case PROC_ID_COMMAND:
		{
			printk("command string : %s\n", command );
		}
		break;
				
		default:
		{
			/* do nothing */
		}
		break;	
	}
	
	return strlen(command);
}

/**
 * initialize proc utility for frc device
 *
 * @see FRC_Init
*/
void	FRC_PROC_Init (void)
{
	OS_PROC_CreateEntryEx ( FRC_MODULE, _g_frc_device_proc_table, 
											_FRC_ReadProcFunction,
											_FRC_WriteProcFunction );
}

/**
 * cleanup proc utility for frc device
 *
 * @see FRC_Cleanup
*/
void	FRC_PROC_Cleanup (void)
{
	OS_PROC_RemoveEntry( FRC_MODULE );	
}

/** @} */

