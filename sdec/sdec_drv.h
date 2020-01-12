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
 *  driver interface header for sdec device. ( used only within kdriver )
 *	sdec device will teach you how to make device driver with new platform.
 *
 *  @author		kyoungbin.park (kyoungbin.park@lge.com)
 *  @version	1.0
 *  @date		2009.12.30
 *			20010.3.11 DD implement
 			jasonlee.lee(jasonlee.lee@lge.com)
 *
 *  @addtogroup lg1150_sdec
 *	@{
 */

#ifndef	_SDEC_DRV_H_
#define	_SDEC_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "sdec_cfg.h"
#include "sdec_kapi.h"

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

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
#define GPB_BASE_ADDR_MASK		0xF0000000

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern	int      SDEC_Init(void);
extern	void     SDEC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	int		g_sdec_debug_fd;
//This enumeration describes the return type type of the DDI function.

/*----------------------------------------------------------------------------------------
   enum
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
   structre
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
   API
----------------------------------------------------------------------------------------*/



#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SDEC_DRV_H_ */

/** @} */
