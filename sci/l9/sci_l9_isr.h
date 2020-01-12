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
#ifndef	_SCI_L9_ISR_H_
#define	_SCI_L9_ISR_H_

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
/**
	Smartcard Interface IRQ Definitions
*/
typedef enum
{
	SCI_L9_IRQ_FRF		= 0x01,
	SCI_L9_IRQ_FRNE		= 0x02,
	SCI_L9_IRQ_FTF		= 0x04,
	SCI_L9_IRQ_FTE		= 0x08,
	SCI_L9_IRQ_EOR		= 0x10,
	SCI_L9_IRQ_EOT		= 0x20,
	SCI_L9_IRQ_ERR		= 0x40,
	SCI_L9_IRQ_CDPWR 	= 0x80
} SCI_IRQ_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

extern void SCI_L9_EnableIRQ(UINT8 mask);
extern void SCI_L9_DisableIRQ(UINT8 mask);
extern SCI_RETURN_T SCI_L9_WaitIRQ (SCI_IRQ_T irq, UINT32 timeout);
extern SCI_RETURN_T SCI_L9_InitISR(void);
extern void SCI_L9_ExitISR(void);

extern UINT8 isCardInserted;


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SCI_L9_ISR_H_ */

/** @} */

