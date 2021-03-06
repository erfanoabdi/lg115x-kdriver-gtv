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
 *  core driver interface header for ci device. ( used only within kdriver )
 *
 *  @author		Srinivasan Shanmugam	(srinivasan.shanmugam@lge.com) /  Hwajeong Lee (hwajeong.lee@lge.com)
 *  @version	1.0
 *  @date		2010.2.22
 *
 *  @addtogroup lg1150_ci
 *	@{
 */

#ifndef	_CI_COREDRV_H_
#define	_CI_COREDRV_H_

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

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

SINT32 CI_Initialize( void );

SINT32 CI_UnInitialize( void );

SINT32 CI_ResetCI( void );

SINT32 CI_CAMInit( void );

SINT32 CI_CAMPowerOff( void );

SINT32 CI_CAMPowerOnCompleted( void );

SINT32 CI_CheckCIS( void );

SINT32 CI_WriteCOR( void );

SINT32 CI_CAMDetectStatus( UINT32 *o_puwIsCardDetected );

SINT32 CI_ReadData ( UINT8 *o_pbyData, UINT16 *io_pwDataBufSize );

SINT32 CI_NegoBuff( UINT32 *o_puwBufSize );

SINT32 CI_ReadDAStatus( UINT32 *o_puwIsDataAvailable );

SINT32 CI_WriteData ( UINT8 *i_pbyData, UINT32 i_wDataBufSize );

SINT32 CI_ResetPhysicalIntrf( void );

SINT32 CI_ReadIIRStatus( UINT32 *o_puwIIRStatus );

SINT32 CI_CheckCAMType( SINT8 *o_pRtnValue, UINT8 *o_puwCheckCAMType );


SINT32 CI_RegPrint( void );

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _CI_COREDRV_H_ */

/** @} */
