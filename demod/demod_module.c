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

/*! \file DEMOD_module.c
 * \brief DEMOD module control Drivers linux module
 */


/**
 *	Necessary includes for device drivers
 */
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
#include <linux/workqueue.h>		/**< For working queue */
#include <linux/interrupt.h>

#include "demod_drv.h"
#include "demod_kapi.h"
#include "demod_module.h"


/******************************************************************************
 *				DEFINES
 *****************************************************************************/


/**
 *	Global variables of the driver
 */

/******************************************************************************
 *				DATA STRUCTURES
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions
 */


/******************************************************************************
 *				Local function
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions
 */



/**
* ADC Module exit
*
* @parm void
* @return int
*/
int DEMOD_exit(void)
{
	int ret = 0;


	return ret;
}


/**
* ADC Module initialize
*
* @parm void
* @return int
*/
int DEMOD_Initialize(void)
{
	int ret = 0;
	
	
	return ret;
}

