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
 *  register interface struct type header for png device. ( used only within kdriver )
 *	png device will teach you how to make device driver with new platform.
 *
 *  @author		Youngwoo Jin (youngwoo.jin@lge.com)
 *  @version	1.0 
 *  @date		2011.04.21
 *
 *  @addtogroup lg1152_png
 *	@{
 */

#ifndef	_PNG_REG_API_TYPE_H_
#define	_PNG_REG_API_TYPE_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"

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
typedef struct {
	void	(*Initialize)(void);
	void	(*Reset)(void);
	void	(*SetInterruptMask)(UINT32 i_ui32Interrupts, BOOLEAN i_bEnable);
	UINT32	(*GetInterruptStatus)(void);
	void	(*ClearInterrupt)(UINT32 i_ui32Interrupts);
	void	(*Start)(void);
	void	(*MidStart)(void);
	void	(*SetReadBaseAddress)(UINT32 i_ui32Address);
	void	(*SetReadByteSzie)(UINT32 i_ui32Size);
	void	(*SetWriteBaseAddress)(UINT32 i_ui32Address);
	void	(*SetImageInfo)(UINT16 i_ui16Width, UINT16 i_ui16Height, UINT8 i_ui8BitDepth, UINT8 ui8ColorType);
	void	(*SetDMAReadValue)(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
	void	(*SetDMAWriteValue)(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
	void	(*SetReadByteDrop)(UINT8 i_ui32Start, UINT8 i_ui32End);
	void	(*SetReadBaseAddressWin)(UINT32 i_ui32Address);
	void	(*SetWriteBaseAddressWin)(UINT32 i_ui32Address);
	void	(*SetDMAReadValueWin)(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
	void	(*SetDMAWriteValueWin)(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
	void	(*SetReadBaseAddressFilter)(UINT32 i_ui32Address);
	void	(*SetWriteBaseAddressFilter)(UINT32 i_ui32Address);
	void	(*SetDMAReadValueFilter)(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
	void	(*SetDMAWriteValueFilter)(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
} PNG_REG_API_T;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PNG_REG_API_TYPE_H_ */

/** @} */
