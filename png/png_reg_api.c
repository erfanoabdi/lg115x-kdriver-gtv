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
 *  register interface implementation for png device of lg1152. ( used only within kdriver )
 *	png device will teach you how to make device driver with new platform.
 *
 *  @author		Youngwoo Jin (youngwoo.jin@lge.com)
 *  @version	1.0 
 *  @date		2011.04.21
 *
 *  @addtogroup lg1152_png
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"
#include "os_util.h"
#include "png_reg_api.h"
#include "png_reg_api_type.h"

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
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern PNG_REG_API_T g_stRegAPI_l8;
extern PNG_REG_API_T g_stRegAPI_l9;

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static PNG_REG_API_T *_g_pstRegAPI;

/*========================================================================================
	Implementation Group
========================================================================================*/
void PNG_REG_Initialize(void)
{
	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )	{ _g_pstRegAPI = &g_stRegAPI_l9; }
	else										{ _g_pstRegAPI = &g_stRegAPI_l8; }

	if( _g_pstRegAPI->Initialize != NULL )	{ _g_pstRegAPI->Initialize(); }
}

void PNG_REG_Reset(void)
{
	if( _g_pstRegAPI->Reset != NULL ) { _g_pstRegAPI->Reset(); }
}

void PNG_REG_SetInterruptMask(UINT32 i_ui32Interrupts, BOOLEAN i_bEnable)
{
	if( _g_pstRegAPI->SetInterruptMask != NULL ) { _g_pstRegAPI->SetInterruptMask(i_ui32Interrupts, i_bEnable); }
}

UINT32 PNG_REG_GetInterruptStatus(void)
{
	UINT32 ui32Ret = 0;
	if( _g_pstRegAPI->GetInterruptStatus != NULL ) { ui32Ret = _g_pstRegAPI->GetInterruptStatus(); }

	return ui32Ret;
}

void PNG_REG_ClearInterrupt(UINT32 i_ui32Interrupts)
{
	if( _g_pstRegAPI->ClearInterrupt != NULL ) { _g_pstRegAPI->ClearInterrupt(i_ui32Interrupts); }
}

void PNG_REG_Start(void)
{
	if( _g_pstRegAPI->Start != NULL ) { _g_pstRegAPI->Start(); }
}

void PNG_REG_MidStart(void)
{
	if( _g_pstRegAPI->MidStart != NULL ) { _g_pstRegAPI->MidStart(); }
}

void PNG_REG_SetReadBaseAddress(UINT32 i_ui32Address)
{
	if( _g_pstRegAPI->SetReadBaseAddress != NULL ) { _g_pstRegAPI->SetReadBaseAddress(i_ui32Address); }
}

void PNG_REG_SetReadByteSzie(UINT32 i_ui32Size)
{
	if( _g_pstRegAPI->SetReadByteSzie != NULL ) { _g_pstRegAPI->SetReadByteSzie(i_ui32Size); }
}

void PNG_REG_SetWriteBaseAddress(UINT32 i_ui32Address)
{
	if( _g_pstRegAPI->SetWriteBaseAddress != NULL ) { _g_pstRegAPI->SetWriteBaseAddress(i_ui32Address); }
}

void PNG_REG_SetImageInfo(UINT16 i_ui16Width, UINT16 i_ui16Height, UINT8 i_ui8BitDepth, UINT8 ui8ColorType)
{
	if( _g_pstRegAPI->SetImageInfo != NULL ) { _g_pstRegAPI->SetImageInfo(i_ui16Width, i_ui16Height, i_ui8BitDepth, ui8ColorType); }
}

void PNG_REG_SetDMAReadValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	if( _g_pstRegAPI->SetDMAReadValue != NULL ) { _g_pstRegAPI->SetDMAReadValue(i_ui8TxID, i_ui16Delay, i_ui8Priority1, i_ui8Priority2); }
}

void PNG_REG_SetDMAWriteValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	if( _g_pstRegAPI->SetDMAWriteValue != NULL ) { _g_pstRegAPI->SetDMAWriteValue(i_ui8TxID, i_ui16Delay, i_ui8Priority1, i_ui8Priority2); }
}

void PNG_REG_SetReadByteDrop(UINT8 i_ui32Start, UINT8 i_ui32End)
{
	if( _g_pstRegAPI->SetReadByteDrop != NULL ) { _g_pstRegAPI->SetReadByteDrop(i_ui32Start, i_ui32End); }
}

void PNG_REG_SetReadBaseAddressWin(UINT32 i_ui32Address)
{
	if( _g_pstRegAPI->SetReadBaseAddressWin != NULL ) { _g_pstRegAPI->SetReadBaseAddressWin(i_ui32Address); }
}

void PNG_REG_SetWriteBaseAddressWin(UINT32 i_ui32Address)
{
	if( _g_pstRegAPI->SetWriteBaseAddressWin != NULL ) { _g_pstRegAPI->SetWriteBaseAddressWin(i_ui32Address); }
}

void PNG_REG_SetDMAReadValueWin(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	if( _g_pstRegAPI->SetDMAReadValueWin != NULL ) { _g_pstRegAPI->SetDMAReadValueWin(i_ui8TxID, i_ui16Delay, i_ui8Priority1, i_ui8Priority2); }
}

void	PNG_REG_SetDMAWriteValueWin(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	if( _g_pstRegAPI->SetDMAWriteValueWin != NULL ) { _g_pstRegAPI->SetDMAWriteValueWin(i_ui8TxID, i_ui16Delay, i_ui8Priority1, i_ui8Priority2); }
}

void	PNG_REG_SetReadBaseAddressFilter(UINT32 i_ui32Address)
{
	if( _g_pstRegAPI->SetReadBaseAddressFilter != NULL ) { _g_pstRegAPI->SetReadBaseAddressFilter(i_ui32Address); }
}

void	PNG_REG_SetWriteBaseAddressFilter(UINT32 i_ui32Address)
{
	if( _g_pstRegAPI->SetWriteBaseAddressFilter != NULL ) { _g_pstRegAPI->SetWriteBaseAddressFilter(i_ui32Address); }
}

void	PNG_REG_SetDMAReadValueFilter(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	if( _g_pstRegAPI->SetDMAReadValueFilter != NULL ) { _g_pstRegAPI->SetDMAReadValueFilter(i_ui8TxID, i_ui16Delay, i_ui8Priority1, i_ui8Priority2); }
}

void	PNG_REG_SetDMAWriteValueFilter(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	if( _g_pstRegAPI->SetDMAWriteValueFilter != NULL ) { _g_pstRegAPI->SetDMAWriteValueFilter(i_ui8TxID, i_ui16Delay, i_ui8Priority1, i_ui8Priority2); }
}

