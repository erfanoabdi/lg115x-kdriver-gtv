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
 * main CLI implementation for SE device.
 *
 * author     Srinivasan Shanmugam (srinivasan.shanmugam@lge.com)
 * version    1.0
 * date       2010.03.23
 * note       Additional information.
 *
 * @addtogroup lg1150_se
 * @{
 */
#ifndef	_SE_CLI_H_
#define	_SE_CLI_H_

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

#define DES			1
#define TDES		2
#define AES			3
#define MS			4
#define RC4			5
#define SHA			6
#define HSHA		7

#define CLEAR 			0
#define CTS				1
#define OFB				2
#define SCTE			3
#define PADEN			4
#define PADDIS			5

#define ECB 			0
#define CBC				1
#define CTR				2
#define OMAC_1			3
#define SHA_1			4
#define SHA_256			5
#define HSHA_1			6
#define HSHA_256		7


UINT32 g_auiCryptDet[][4] = {
/* Crypto type, Key size, block mode, residual mode */
{ DES, 8, ECB, CLEAR },
{ DES, 8, ECB, CTS },
{ DES, 8, ECB, OFB },
{ DES, 8, CBC, CLEAR },
{ DES, 8, CBC, CTS },
{ DES, 8, CBC, OFB },
{ DES, 8, CBC, SCTE },

{ TDES,	24,	ECB, CLEAR },
{ TDES,	24,	ECB, CTS },
{ TDES,	24,	ECB, OFB },
{ TDES,	24,	CBC, CLEAR },
{ TDES,	24,	CBC, CTS },
{ TDES,	24,	CBC, OFB },

{ AES, 16, ECB,	CLEAR },
{ AES, 16, ECB,	CTS },
{ AES, 16, ECB,	OFB },
{ AES, 16, CBC,	CLEAR },
{ AES, 16, CBC,	CTS },
{ AES, 16, CBC,	OFB },
{ AES, 16, CTR,	0 },
{ AES, 16, OMAC_1, 0 },

{ AES, 24, ECB,	CLEAR },
{ AES, 24, ECB,	CTS },
{ AES, 24, ECB,	OFB },
{ AES, 24, CBC,	CLEAR },
{ AES, 24, CBC,	CTS },
{ AES, 24, CBC,	OFB },
{ AES, 24, CTR,	0 },
{ AES, 24, OMAC_1, 0 },

{ AES, 32, ECB,	CLEAR },
{ AES, 32, ECB,	CTS },
{ AES, 32, ECB,	OFB },
{ AES, 32, CBC,	CLEAR },
{ AES, 32, CBC,	CTS },
{ AES, 32, CBC,	OFB },
{ AES, 32, CTR,	0 },
{ AES, 32, OMAC_1, 0 },

{ RC4, 1, 0, 0 },
{ RC4, 2, 0, 0 },
{ RC4, 3, 0, 0 },
{ RC4, 13, 0, 0 },
{ RC4, 16, 0, 0 },

{ MS, 48, 0, 0 },

{ SHA, 0, SHA-1, PADEN },
{ SHA, 0, SHA-1, PADDIS },
{ SHA, 0, SHA-256, PADEN },
{ SHA, 0, SHA-256, PADDIS },

{ HSHA, 1, HSHA-1, PADEN },
{ HSHA, 1, HSHA-256, PADEN },
{ HSHA, 2, HSHA-1, PADEN },
{ HSHA, 47, HSHA-1, PADEN },
{ HSHA, 47, HSHA-256, PADEN },
{ HSHA, 48, HSHA-1, PADEN },
{ HSHA, 48, HSHA-256, PADEN }
};



/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif /* _SE_CLI_H_ */
