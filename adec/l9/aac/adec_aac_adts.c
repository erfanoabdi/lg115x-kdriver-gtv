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
 * Implementation of AAC ADTS parser.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       AAC ADTS parser.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "adec_aac_internal.h"
#include "adec_aac_bitstream.h"
#include "adec_aac_asc.h"
#include "adec_aac_adts.h"
#include "adec_aac_debug.h"

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


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/


/**
 * Parse the ADTS header.
 *
 * @param 	pBuffer - The pointer of the memory, the AAC es stored.
 * @param	length - Total length of the data.
 * @param	pHeader - The pointer of the structure, the parse result will be stored. 
 *				This structure should be allocated by the caller.
 * @return	greater than or equlas to 0 - First byte of the adts header from pBuffer.
 * @return	otherwise - Error occures during parsing.
*/

int ADEC_AAC_ParseADTS(
	unsigned char* pBuffer, 
	unsigned int length, 
	unsigned int* used, 
	ADEC_AAC_ADTS_HEADER_T* pHeader)
{
	ADEC_AAC_BITSTREAM_T bitStream;
	ADEC_AAC_BITSTREAM_T* pBitStream;
	unsigned char *currentBuffer;
	unsigned char *endBuffer;
	unsigned char *rawDataStartPosition;
	unsigned char *tempBuffer;
	unsigned int syncWord;
	unsigned int rawDataLoop;
	unsigned int headerPosition;
	unsigned int rawDataLength;

	ADEC_CHECK_CODE(pBuffer == NULL || pHeader == NULL,return -1,"Parameter is null\n");

	currentBuffer = pBuffer;
	endBuffer = currentBuffer + length;
	pBitStream = &bitStream;
	headerPosition = 0;
	memset(pHeader, 0, sizeof(ADEC_AAC_ADTS_HEADER_T));

	syncWord = 0;
 
	// Find syncword (0xfff0)
	while(currentBuffer + 7 < endBuffer)
	{
		syncWord = (syncWord << 8) | *currentBuffer;
		if(0xfff0 == (syncWord & 0xfff6))
		{
			break;
		}
		currentBuffer++;
		headerPosition++;
	}

	if(0xfff0 != (syncWord & 0xfff6))
	{
		AAC_ERROR("Fail to find sync in ADTS frame.\n");
		if(used)
		{
			*used = headerPosition - 2;
		}
		return -AAC_PARSE_ERROR_SYNC_FAIL;
	}

	// Sync is 2bytes long. At this point currentBuffer and headerPosition indicate the 2nd byte of the sync.
	headerPosition --;

	BITSTREAM_INIT(currentBuffer, endBuffer - currentBuffer, pBitStream);

	// Skip sync
	BITSTREAM_SKIP_BITS(4, pBitStream);

	// read ID (1bit)
	BITSTREAM_READ_1BIT(pHeader->ID, pBitStream);

	// read layer (2bits)
	BITSTREAM_READ_BITS(pHeader->layer, 2, pBitStream);

	//  read protection_absent (1bit)
	BITSTREAM_READ_1BIT(pHeader->protection_absent, pBitStream);

	// read profile_ObjectType (2bits)
	BITSTREAM_READ_BITS(pHeader->profile_ObjectType, 2, pBitStream);

	// read sampling_frequency_index (4bits)
	BITSTREAM_READ_BITS(pHeader->sampling_frequency_index, 4, pBitStream);

	// read private_bit (1bit)
	BITSTREAM_READ_1BIT(pHeader->private_bit, pBitStream);
	
	// read channel_configuration (3bits)
	BITSTREAM_READ_BITS(pHeader->channel_configuration,  3, pBitStream);
	
	// read original_copy (1bit)
	BITSTREAM_READ_1BIT(pHeader->original_copy, pBitStream);
	
	// read home (1bit)
	BITSTREAM_READ_1BIT(pHeader->home, pBitStream);
	
	// read copyright_identification_bit(1bit)
	BITSTREAM_READ_1BIT(pHeader->copyright_identification_bit, pBitStream);
	
	// read copyright_identification_start(1bit)
	BITSTREAM_READ_1BIT(pHeader->copyright_identification_start, pBitStream);

	// read aac_frame_length (13bits)
	BITSTREAM_READ_BITS(pHeader->aac_frame_length, 13, pBitStream);
	
	// read adts_buffer_fullness (11bits)
	BITSTREAM_READ_BITS(pHeader->adts_buffer_fullness, 11, pBitStream);
	
	// read number_of_raw_data_blocks_in_frame (2bits)
	BITSTREAM_READ_BITS(pHeader->number_of_raw_data_blocks_in_frame, 2, pBitStream);


	// Check whether we have enough data.
	// aac_frame_length is total length of frame including header(probably)
	if(pHeader->aac_frame_length > length - headerPosition)
	{
		AAC_ERROR("Need more data to parse frame data in ADTS.\n");
		if(used)
		{
			*used = headerPosition;
		}
		return -AAC_PARSE_ERROR_NEED_MORE_DATA;
	}

	// Check next sync
	if(pHeader->aac_frame_length < length - headerPosition - 2)
	{
		syncWord = 
			(currentBuffer[pHeader->aac_frame_length - 1] << 8) | 
			(currentBuffer[pHeader->aac_frame_length]);

		if(0xfff0 != (syncWord & 0xfff6))
		{
			AAC_ERROR("Fail to find sync next ADTS frame.\n");
			if(used)
			{
				*used = headerPosition + 2;
			}
			return -AAC_PARSE_ERROR_NEXT_SYNC_FAIL;
		}
	}

	// Total ADTS header length is 56Bits(7Byte) and currentBuffer is located at the 2nd byte of header. 
	currentBuffer += 6;

	rawDataLength = pHeader->aac_frame_length - 7;
	endBuffer = pBuffer + headerPosition + pHeader->aac_frame_length;
	// Processing the raw data block.
	if (0 != pHeader->number_of_raw_data_blocks_in_frame)
	{

		// If protection exsist, parse crc. (currently never check the validity)
		// This crc is for entire ADTS frame.
		if( 0 == pHeader->protection_absent)
		{
			pHeader->crc = READ_16BIT_BIG_ENDIAN(currentBuffer);
			currentBuffer += 2;
			rawDataLength -= 2;
		}
#if (AAC_OPT_PARSE_RAW_DATA_BLOCK == 1)
		pHeader->rawDataBlock[0].pBuffer = currentBuffer;
		pHeader->rawDataBlock[0].bufferLength = rawDataLength;
#endif // (AAC_OPT_PARSE_RAW_DATA_BLOCK == 1)
	}
	else
	{
		if(0 == pHeader->protection_absent)
		{
			// [number_of_raw_data_blocks_in_frame + 1] is the actual number of raw data in this frame.
			// If the protection_absent = 0, there are information about the start positions of each blocks.
			// In this case the CRC is also exist.
			// Each information and CRC value have 2 bytes length.
			rawDataStartPosition = currentBuffer + (pHeader->number_of_raw_data_blocks_in_frame + 2) *2;
			pHeader->rawDataBlock[0].pBuffer = rawDataStartPosition;
			for(rawDataLoop = 1; rawDataLoop <= pHeader->number_of_raw_data_blocks_in_frame ; rawDataLoop++)
			{
				pHeader->rawDataBlock[rawDataLoop].pBuffer = rawDataStartPosition + READ_16BIT_BIG_ENDIAN(currentBuffer);
				if(pHeader->rawDataBlock[rawDataLoop].pBuffer > endBuffer)
				{
					AAC_ERROR("Incorrect raw data block position (cur : 0x%08X, end : 0x%08X)\n",
						pHeader->rawDataBlock[rawDataLoop].pBuffer, endBuffer);
					if(used)
					{
						*used = headerPosition + 2;
					}
					return -AAC_PARSE_ERROR_SYNTAX;
				}

				// After raw data block, 2 bytes long crc value exists.
				pHeader->rawDataBlock[rawDataLoop-1].bufferLength = 
					pHeader->rawDataBlock[rawDataLoop].pBuffer - pHeader->rawDataBlock[rawDataLoop-1].pBuffer - 2;
				currentBuffer+=2;
			}

			// Calculate the length of the last block.
			pHeader->rawDataBlock[rawDataLoop-1].bufferLength = 
				endBuffer - pHeader->rawDataBlock[rawDataLoop-1].pBuffer - 2;
			
			pHeader->crc = READ_16BIT_BIG_ENDIAN(currentBuffer);

			// Threre are CRC after each raw data blocks.
			for(rawDataLoop = 0 ; rawDataLoop <= pHeader->number_of_raw_data_blocks_in_frame ; rawDataLoop++)
			{
				tempBuffer = pHeader->rawDataBlock[rawDataLoop].pBuffer + pHeader->rawDataBlock[rawDataLoop].bufferLength;
				pHeader->rawDataBlock[rawDataLoop].crc = READ_16BIT_BIG_ENDIAN(tempBuffer);
			}
		}
		else
		{
			// If the protection_absent is not zero, there is no index of each raw blocks, and crc value.
			// In this case we will regards the following raw blocks as a single block (actually series of raw blocks).
			pHeader->rawDataBlock[0].pBuffer = currentBuffer;
			pHeader->rawDataBlock[0].bufferLength = rawDataLength;
		}
	} 

#if (AAC_OPT_PRINT_ADTS_MEMBER == 1)
	ADEC_PRINT_MEMBER(INT,pHeader,aac_frame_length);
	ADEC_PRINT_MEMBER(HEX16,pHeader,adts_buffer_fullness);
	ADEC_PRINT_MEMBER(INT,pHeader,crc);
	ADEC_PRINT_MEMBER(INT,pHeader,ID);
	ADEC_PRINT_MEMBER(INT,pHeader,layer);
	ADEC_PRINT_MEMBER(INT,pHeader,protection_absent);
	ADEC_PRINT_MEMBER(INT,pHeader,profile_ObjectType);
	ADEC_PRINT_MEMBER(INT,pHeader,sampling_frequency_index);
	ADEC_PRINT_MEMBER(INT,pHeader,private_bit);
	ADEC_PRINT_MEMBER(INT,pHeader,channel_configuration);
	ADEC_PRINT_MEMBER(INT,pHeader,original_copy);
	ADEC_PRINT_MEMBER(INT,pHeader,home);
	ADEC_PRINT_MEMBER(INT,pHeader,copyright_identification_bit);
	ADEC_PRINT_MEMBER(INT,pHeader,copyright_identification_start);
	ADEC_PRINT_MEMBER(INT,pHeader,number_of_raw_data_blocks_in_frame); 
#endif

	if(used)
	{
		*used = headerPosition + pHeader->aac_frame_length;
	}
	return headerPosition;
}




