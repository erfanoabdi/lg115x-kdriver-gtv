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
 *  configuration header for sci device. ( used only within kdriver )
 *	sci device will teach you how to make device driver with new platform.
 *
 *  @author		juhee
 *  @version	1.0
 *  @date		2011.06.01
 *
 *  @addtogroup lg1152_sci
 *	@{
 */

#ifndef	_SCI_CFG_H_
#define	_SCI_CFG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/cdev.h>

#include "sci_err.h"
#include "sci_kapi.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define SCI_MODULE	"sci"

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
struct SCI_MEM_T
{
	UINT32 base;
	UINT32 size;
};

struct SCI_T
{
	UINT8*				module_name;
	UINT32				chip;
	UINT32				platform;
	UINT32 				devno;
	struct cdev			cdev;
	struct SCI_MEM_T 	p_iomem;
	struct SCI_MEM_T 	v_iomem;
	UINT8				infs;
	UINT8				wtx;
	LX_SCI_ATR_T 		atr;

	SCI_RETURN_T	(*reset)(LX_SCI_ATR_T* pATR);
	SCI_RETURN_T	(*configIF)(LX_SCI_ATR_T* pATR);
	SCI_RETURN_T	(*readBlock)(LX_SCI_BLOCK *uprx);
	SCI_RETURN_T	(*writeBlock)(LX_SCI_BLOCK *uptx);
	SCI_RETURN_T	(*setVoltage)(LX_SCI_VOLTAGE volt);
	LX_SCI_VOLTAGE	(*getVoltage)(void);
	LX_SCI_STATUS	(*getStatus)(void);
	SCI_RETURN_T	(*configINFS)(UINT8 infs); 
	SCI_RETURN_T	(*configWTX)(UINT8 wtx); 
};

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern SCI_RETURN_T SCI_Config(struct SCI_T **ppSCI);
extern void SCI_Unconfig(struct SCI_T *pSCI);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SCI_CFG_H_ */

/** @} */

