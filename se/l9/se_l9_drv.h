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
 ***************************************************************************************/

/** @file
 *
 *  This file has the necessary security core driver functions.
 *
 *  author		L9   hayrim.lee (hayrim.lee@lge.com)
 *
 *  version		1.0
 *  date			2011.04.11
 *  note			Additional information.
 *
 *  @addtogroup lg1150_se
 *	@{
 */

#ifndef	_SE_L9_DRV_H_
#define	_SE_L9_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

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

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern SE_RETURN_T SE_L9_Init(SE_T* pSE);
extern void SE_L9_Exit(SE_T* pSE);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SE_L9_DRV_H_ */

/** @} */

