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
 *  typedefs for ci device
 *
 *  author 		Srinivasan Shanmugam (srinivasan.shanmugam@lge.com) /  Hwajeong Lee (hwajeong.lee@lge.com)
 *  version		1.0
 *  date		2009.03.23
 *  note		Additional information.
 *
 *  @addtogroup lg1150_ci
 *	@{
 */

#ifndef	_CI_DEFS_H_
#define	_CI_DEFS_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "ci_cfg.h"

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


#ifdef CI_ENABLE_INFO

	#define CI_INFO( _X_ )						printk _X_

#else

	#define CI_INFO( _X_ )						/* NOP */

#endif /* CI_ENABLE_INFO */

#ifdef CI_ENABLE_DBG_INFO

	#define CI_DBG_INFO( _X_ )						printk _X_

#else

	#define CI_DBG_INFO( _X_ )						/* NOP */

#endif /* CI_ENABLE_DBG_INFO */


#ifdef CI_ENABLE_ERR

	#define CI_ERR( _X_ )						printk _X_

#else

	#define CI_ERR( _X_ )						/* NOP */

#endif /* CI_ENABLE_ERR */


#ifdef CIS_ENABLE_INFO

	#define CIS_INFO( _X_ )						printk _X_

#else

	#define CIS_INFO( _X_ )						/* NOP */

#endif /* CIS_ENABLE_INFO */

#ifdef CIS_ENABLE_TUPLE_DUMP

	#define CIS_TUPLE_DUMP( _X_ )				printk _X_

#else

	#define CIS_TUPLE_DUMP( _X_ )				/* NOP */

#endif /* CIS_ENABLE_TUPLE_DUMP */

#ifdef CIS_ENABLE_PARSE_DUMP

	#define CIS_PARSE_DUMP( _X_ )				printk _X_

#else

	#define CIS_PARSE_DUMP( _X_ )				/* NOP */

#endif /* CIS_ENABLE_PARSE_DUMP */

#define CIS_ERR( _X_ )							CI_ERR( _X_ )

#ifdef CI_IO_ENABLE_INFO

	#define CI_IO_INFO( _X_ )					printk _X_

#else

	#define CI_IO_INFO( _X_ )					/* NOP */

#endif /* CI_IO_ENABLE_INFO */

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _CI_DEFS_H_ */

/** @} */

