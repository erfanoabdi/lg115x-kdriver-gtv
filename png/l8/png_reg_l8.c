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
 *  register interface implementation for png device of lg1150. ( used only within kdriver )
 *	png device will teach you how to make device driver with new platform.
 *
 *  @author		Youngwoo Jin (youngwoo.jin@lge.com)
 *  @version	1.0 
 *  @date		2011.04.21
 *
 *  @addtogroup lg1150_png
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
#include "png_reg_l8.h"
#include "png_reg_api_type.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define PNG_REG_BASE		( L8_VENC_BASE + 0xE00 )
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

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static void		PNG_REG_Initialize(void);
static void		PNG_REG_Reset(void);
static void		PNG_REG_SetInterruptMask(UINT32 i_ui32Interrupts, BOOLEAN i_bEnable);
static UINT32	PNG_REG_GetInterruptStatus(void);
static void		PNG_REG_ClearInterrupt(UINT32 i_ui32Interrupts);
static void		PNG_REG_Start(void);
static void		PNG_REG_SetReadBaseAddress(UINT32 i_ui32Address);
static void		PNG_REG_SetReadByteSzie(UINT32 i_ui32Size);
static void		PNG_REG_SetWriteBaseAddress(UINT32 i_ui32Address);
static void		PNG_REG_SetImageInfo(UINT16 i_ui16Width, UINT16 i_ui16Height, UINT8 i_ui8BitDepth, UINT8 ui8ColorType);
static void		PNG_REG_SetDMAReadValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);
static void		PNG_REG_SetDMAWriteValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2);

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
PNG_REG_API_T g_stRegAPI_l8 = {
	PNG_REG_Initialize,
	PNG_REG_Reset,
	PNG_REG_SetInterruptMask,
	PNG_REG_GetInterruptStatus,
	PNG_REG_ClearInterrupt,
	PNG_REG_Start,
	NULL,//PNG_REG_MidStart,
	PNG_REG_SetReadBaseAddress,
	PNG_REG_SetReadByteSzie,
	PNG_REG_SetWriteBaseAddress,
	PNG_REG_SetImageInfo,
	PNG_REG_SetDMAReadValue,
	PNG_REG_SetDMAWriteValue,
	NULL, //PNG_REG_SetReadByteDrop,
	NULL, //PNG_REG_SetReadBaseAddressWin,
	NULL, //PNG_REG_SetWriteBaseAddressWin,
	NULL, //PNG_REG_SetDMAReadValueWin,
	NULL, //PNG_REG_SetDMAWriteValueWin,
	NULL, //PNG_REG_SetReadBaseAddressFilter,
	NULL, //PNG_REG_SetWriteBaseAddressFilter,
	NULL, //PNG_REG_SetDMAReadValueFilter,
	NULL //PNG_REG_SetDMAWriteValueFilter
};

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static volatile PNG_REG_T* _g_pRegPNG = NULL;		/** PNG register */

/*========================================================================================
	Implementation Group
========================================================================================*/
static void
PNG_REG_Initialize(void)
{
	_g_pRegPNG = (volatile PNG_REG_T*)PNG_REG_BASE;
}

static void
PNG_REG_Reset(void)
{
	_g_pRegPNG->cmd.sw_rst = 1;
}

static void
PNG_REG_SetInterruptMask(UINT32 i_ui32Interrupts, BOOLEAN i_bEnable)
{
	if( i_bEnable ) { *((volatile UINT32*)(&(_g_pRegPNG->intr_enable))) = *((UINT32*)(&(_g_pRegPNG->intr_enable))) |  i_ui32Interrupts; }
	else			{ *((volatile UINT32*)(&(_g_pRegPNG->intr_enable))) = *((UINT32*)(&(_g_pRegPNG->intr_enable))) & ~i_ui32Interrupts; }
}

static UINT32
PNG_REG_GetInterruptStatus(void)
{
	return *((UINT32*)(&(_g_pRegPNG->intr_status)));
}

static void
PNG_REG_ClearInterrupt(UINT32 i_ui32Interrupts)
{
	*((volatile UINT32*)(&(_g_pRegPNG->intr_status))) = i_ui32Interrupts;
}

static void
PNG_REG_Start(void)
{
	_g_pRegPNG->cmd.start = 1;
}

static void
PNG_REG_SetReadBaseAddress(UINT32 i_ui32Address)
{
	*((volatile UINT32*)(&(_g_pRegPNG->read_base_addr))) = i_ui32Address;
}

static void
PNG_REG_SetReadByteSzie(UINT32 i_ui32Size)
{
	*((volatile UINT32*)(&(_g_pRegPNG->read_byte_size))) = i_ui32Size;
}

static void
PNG_REG_SetWriteBaseAddress(UINT32 i_ui32Address)
{
	*((volatile UINT32*)(&(_g_pRegPNG->write_base_addr))) = i_ui32Address;
}

static void
PNG_REG_SetImageInfo(UINT16 i_ui16Width, UINT16 i_ui16Height, UINT8 i_ui8BitDepth, UINT8 ui8ColorType)
{
	*((volatile UINT32*)(&(_g_pRegPNG->image_info1))) = (i_ui16Width<<16) | i_ui16Height;
	*((volatile UINT32*)(&(_g_pRegPNG->image_info2))) = ((ui8ColorType&0x07)<<8) | (i_ui8BitDepth&0x1F);
}

static void
PNG_REG_SetDMAReadValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	*((volatile UINT32*)(&(_g_pRegPNG->dma_read_value))) = ((i_ui8TxID&0x01)<<24) | (i_ui16Delay<<8) | ((i_ui8Priority1&0x0F)<<4) | (i_ui8Priority1&0x0F);
}

static void
PNG_REG_SetDMAWriteValue(UINT8 i_ui8TxID, UINT16 i_ui16Delay, UINT8 i_ui8Priority1, UINT8 i_ui8Priority2)
{
	*((volatile UINT32*)(&(_g_pRegPNG->dma_write_value))) = ((i_ui8TxID&0x01)<<24) | (i_ui16Delay<<8) | ((i_ui8Priority1&0x0F)<<4) | (i_ui8Priority1&0x0F);
}

