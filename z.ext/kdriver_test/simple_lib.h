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
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author     raxis
 *  @version    1.0
 *  @date       2010-01-21
 *  @note       Additional information.
 */

#ifndef	_SIMPLE_LIB_H_
#define	_SIMPLE_LIB_H_

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
#include "base_types.h"

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct
{
    char* commandStr;
	char* helpStr;
    void (*pfunc)(int argc, char* argv[]);
}
DENTRY_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

void	PRINTF( const char *format, ...);

/** get single character from console
 *
 * @param NONE
 * @return character
 */
extern	int		GetChar( );

/** print comment and get hexa decimal number from console
 *
 * @param comment [IN] comment string
 * @return integer
 */
extern	UINT32 	ReadHexKeyInput ( char* comment );

/** print comment and get decimal number from console
 *
 * @param comment [IN] comment string
 * @return integer
 */
extern	UINT32 	ReadDecimalKeyInput ( char* comment );

/** print comment and pause execution until key input.
 *
 * @param comment [IN] comment string
 * @return NONE
 */
extern	void 	PauseKeyInput ( char* comment );

/** parse command string and calls registered callback.
 *
 * @param dtab [IN] pointer to DENTRY_T array.
 * @param prompt [IN] prompt string
 * @return NONE
 */
extern	void	DebugShell ( DENTRY_T* dtab, char* prompt );

/** sleep process for msec time
 *
 * @param msec [IN] milisecond time
 * @return NONE
 */
extern	void	MsecSleep ( UINT32 msec );

/** dump text, binary data to human readable form
 *
 * @param address [IN] start point of address indicator.
 * @param data [IN] pointer to data buffer
 * @param len [IN] length of data buffer
 * @return NONE
*/
extern	void	HexDump	( UINT32 address, UINT8* data, UINT32 len );

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PROTOS_H_ */

