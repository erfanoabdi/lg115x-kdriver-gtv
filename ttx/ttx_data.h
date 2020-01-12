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
 * TTX module
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version     1.0
 *  date
 *  note        Additional information.
 *
 *  @addtogroup lg1150_ttx
 *  @{
 */

#ifndef _TTX_DATA_H_
#define _TTX_DATA_H_

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
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define	TTX_DATA_SIZE		48
/*
#define	TTX_ALL_MEMORY_START	0x25fb0000
#define	TTX_ALL_MEMORY_SIZE	0x4B0000
*/


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct{
	unsigned int header;
	unsigned int tailer;
	unsigned int count;
	unsigned int VPS_offset;
	unsigned int WSS_offset;
	unsigned int status;
} sBufInform;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

int TTX_InitDataMemory(void);
int TTX_UninitDataMemory(void);
int TTX_GetMemorySize(void);
int TTX_GetDataAddr(sHeaderData *data);
unsigned int TTX_GetVBIDataLineCount(unsigned int *data);
unsigned int TTX_CheckDataParity(unsigned int data);

int TTX_ClearTTXMemory(void);
int TTX_ClearVPSMemory(void);
int TTX_ClearWSSMemory(void);
int TTX_ClearAllMemory(void);

int TTX_CC_PutVBIData(unsigned int *data , unsigned int size);
int TTX_CC_GetVBIData(LX_CC_DATA_T *cc_data , unsigned int size);
int TTX_CC_InitDataMemory(void);
int TTX_CC_UninitDataMemory(void);
int TTX_CC_GetVBIBufferInform(void);
int TTX_CC_SetVBIBufferClear(void);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _TTX_DATA_H_ */

/** @} */

