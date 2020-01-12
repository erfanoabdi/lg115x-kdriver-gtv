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
 *  register interface header for png device. ( used only within kdriver )
 *	png device will teach you how to make device driver with new platform.
 *
 *  @author		Youngwoo Jin (youngwoo.jin@lge.com)
 *  @version	1.0 
 *  @date		2011.04.21
 *
 *  @addtogroup lg1152_png
 *	@{
 */

#ifndef	_PNG_REG_API_H_
#define	_PNG_REG_API_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "png_cfg.h"
#include "png_kapi.h"

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

/*----------------------------------------------------------------------------------------
	Generic Usage Functions
----------------------------------------------------------------------------------------*/
void	PNG_REG_Initialize(void);
void	PNG_REG_Reset(void);
void	PNG_REG_SetInterruptMask(UINT32 i_ui32Interrupts, BOOLEAN i_bEnable);
UINT32	PNG_REG_GetInterruptStatus(void);
void	PNG_REG_ClearInterrupt(UINT32 i_ui32Interrupts);
void	PNG_REG_Start(void);
void	PNG_REG_MidStart(void);
void	PNG_REG_SetReadBaseAddress(UINT32 i_ui32Address);
void	PNG_REG_SetReadByteSzie(UINT32 i_ui32Size);
void	PNG_REG_SetWriteBaseAddress(UINT32 i_ui32Address);
void	PNG_REG_SetImageInfo(UINT16 i_ui16Width, UINT16 i_ui16Height, UINT8 i_ui8BitDepth, UINT8 ui8ColorType);
void	PNG_REG_SetDMAReadValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
void	PNG_REG_SetDMAWriteValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
void	PNG_REG_SetReadByteDrop(UINT8 i_ui32Start, UINT8 i_ui32End);
void	PNG_REG_SetReadBaseAddressWin(UINT32 i_ui32Address);
void	PNG_REG_SetWriteBaseAddressWin(UINT32 i_ui32Address);
void	PNG_REG_SetDMAReadValueWin(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
void	PNG_REG_SetDMAWriteValueWin(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
void	PNG_REG_SetReadBaseAddressFilter(UINT32 i_ui32Address);
void	PNG_REG_SetWriteBaseAddressFilter(UINT32 i_ui32Address);
void	PNG_REG_SetDMAReadValueFilter(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
void	PNG_REG_SetDMAWriteValueFilter(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PNG_REG_API_H_ */

/** @} */
