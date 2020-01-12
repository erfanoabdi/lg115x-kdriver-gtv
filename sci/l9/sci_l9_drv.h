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
 *  driver interface header for sci device. ( used only within kdriver )
 *	this file lists exported function, types for the external modules.
 *
 *  @author		Juhee park
 *  @version	1.0
 *  @date		2011.06.03
 *
 *  @addtogroup lg1152_sci
 *	@{
 */
#ifndef	_SCI_L9_DRV_H_
#define	_SCI_L9_DRV_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "./../sci_cfg.h"

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

extern	SCI_RETURN_T SCI_L9_Init(struct SCI_T* pSCI);
extern	void		 SCI_L9_Exit(struct SCI_T* pSCI);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SCI_L9_DRV_H_ */

/** @} */
