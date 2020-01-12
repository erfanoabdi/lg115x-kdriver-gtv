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

/** @file adec_aac_adts.h
 *
 *  This is a header file for adec_aac_adts.c
 *  Structres and functions , needed to parse the AAC ADTS header, is defined here.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       ADTS parser.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */



#ifndef _ADEC_AAC_ADTS_H_
#define _ADEC_AAC_ADTS_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "adec_aac_generic.h"

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

typedef struct _ADEC_AAC_ADTS_RAW_DATA_BLOCK_T
{
	unsigned char* 	pBuffer;
	unsigned int 	bufferLength;
	unsigned int	crc;
}ADEC_AAC_ADTS_RAW_DATA_BLOCK_T;

typedef struct _ADEC_AAC_ADTS_HEADER_T
{
#if (AAC_OPT_PARSE_RAW_DATA_BLOCK == 1)
	// Maximum no of blocks in single ADTS  frame is 5.
	// If the number_of_raw_data_blocks_in_frame field set 0,
	// it means that one raw data block exist in this frame.
	// If the protection_absent is not zero the first block contains all data.
	ADEC_AAC_ADTS_RAW_DATA_BLOCK_T rawDataBlock[5];
#endif //(AAC_OPT_PARSE_RAW_DATA_BLOCK == 1)
	unsigned short 	aac_frame_length;
	unsigned short 	adts_buffer_fullness;
	unsigned short 	crc;
	unsigned char 	ID;
	unsigned char 	layer;
	unsigned char 	protection_absent;
	unsigned char 	profile_ObjectType;
	unsigned char 	sampling_frequency_index;
	unsigned char 	private_bit;
	unsigned char 	channel_configuration;
	unsigned char 	original_copy;
	unsigned char 	home;
	unsigned char 	copyright_identification_bit;
	unsigned char 	copyright_identification_start;
	unsigned char 	number_of_raw_data_blocks_in_frame; 	
}ADEC_AAC_ADTS_HEADER_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

extern int ADEC_AAC_ParseADTS(
	unsigned char* pBuffer, 
	unsigned int length, 
	unsigned int* used, 
	ADEC_AAC_ADTS_HEADER_T* pHeader);


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_ADTS_H_ */
