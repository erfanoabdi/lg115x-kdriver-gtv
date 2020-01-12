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
 * main CLI implementation for CI device.
 *
 * author     Srinivasan Shanmugam (srinivasan.shanmugam@lge.com)
 * version    1.0
 * date       2010.03.23
 * note       Additional information.
 *
 * @addtogroup lg1150_ci
 * @{
 */
#ifndef	_CI_CLI_H_
#define	_CI_CLI_H_

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define CLI_CI_CMD_BASE		    8000
#define CLI_CI_CMD_SIZE		    20

#define CI_CLI_ENABLE_INFO

#ifdef CI_CLI_ENABLE_INFO

	#define CI_CLI_INFO( _X_ )				printf _X_

#else

	#define CI_CLI_INFO( _X_ )				/* NOP */

#endif /* CI_ENABLE_INFO */

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif /* _CI_CLI_H_ */