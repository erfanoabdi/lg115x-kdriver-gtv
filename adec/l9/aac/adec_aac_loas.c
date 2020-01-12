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
#include "adec_aac_config.h"
#include "adec_aac_loas.h"
#include "adec_aac_debug.h"
#include "adec_aac_bitstream.h"

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
static int ADEC_AAC_ParseAudioSyncStream(
	ADEC_AAC_BITSTREAM_T *_pBitStream,
	ADEC_AAC_AUDIO_SYNC_STREAM_T* _pAss)
{
	unsigned int syncByte;
	int headerPosition;

	AAC_TRACE_BEGIN();
	BITSTREAM_CHECK_BITS(syncByte, 11, _pBitStream);
	headerPosition = 0;

	while(syncByte != 0x2B7 && _pBitStream->byteRemain > 0)
	{
		BITSTREAM_SKIP_BITS(8, _pBitStream);
		headerPosition ++;
		BITSTREAM_CHECK_BITS(syncByte, 11, _pBitStream);
	}

	if(syncByte != 0x2B7)
	{
		AAC_TRACE_END();
		AAC_ERROR("Fail to find LOAS sync.\n");
		return -AAC_PARSE_ERROR_SYNC_FAIL;
	}

	BITSTREAM_SKIP_BITS(11, _pBitStream);
	BITSTREAM_READ_BITS(_pAss->audioMuxLengthBytes, 13, _pBitStream);

	BITSTREAM_BYTE_ALIGN(_pBitStream);

	while(_pBitStream->bitRemain >= 8)
	{
		_pBitStream->bitRemain -= 8;
		_pBitStream->byteBuffer --;
	}

	_pAss->muxElementStartPosition = _pBitStream->byteBuffer;

	if(_pBitStream->byteRemain >= _pAss->audioMuxLengthBytes)
	{
		if(_pBitStream->byteRemain >= _pAss->audioMuxLengthBytes + 2)
		{
			syncByte = 
				(
					(_pAss->muxElementStartPosition[_pAss->audioMuxLengthBytes] << 8) |
					(_pAss->muxElementStartPosition[_pAss->audioMuxLengthBytes+1])
				) >> 5;

			if(syncByte != 0x2B7)
			{
				AAC_TRACE_END();
				AAC_ERROR("Fail to find sync next LOAS frame\n");
				_pBitStream->byteBuffer -= 2;
				_pBitStream->byteRemain += 2;
				return -AAC_PARSE_ERROR_NEXT_SYNC_FAIL;
			}
		}

		AAC_TRACE_END();
		return headerPosition;
	}
	else
	{
		AAC_TRACE_END();
		return -AAC_PARSE_ERROR_NEED_MORE_DATA;
	}
}

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/

int ADEC_AAC_ParseLoas(
	unsigned char* _pBuffer, 
	unsigned int _length, 
	unsigned int* _used,
	ADEC_AAC_SYNC_STREAM_TYPE_T _syncType,
	ADEC_AAC_LOAS_T* _pLoas)
{
	ADEC_AAC_BITSTREAM_T bitStream;
	ADEC_AAC_BITSTREAM_T *pBitStream;
	unsigned char* pCurBuf;
	unsigned char* pEndBuf;
	unsigned int syncByte;
	unsigned int used;
	int result;
	int remainBits;

	AAC_TRACE_BEGIN();

	if(_pLoas == NULL || _pBuffer == NULL)
	{
		AAC_ERROR("Null parameter\n");
		AAC_TRACE_END();
		return -AAC_PARSE_ERROR_GENERIG;
	}

	syncByte = 0;
	pCurBuf = _pBuffer;
	pEndBuf = _pBuffer + _length;

	if(_syncType == AAC_SYNC_STREAM_TYPE_NOT_DEF)
	{
		// Check Sync Stream Type
		while(pCurBuf <= pEndBuf)
		{
			syncByte = (syncByte << 8) | (*(pCurBuf++));

			// Check if stream is AudioSyncStream
			// 11Bits Mask = FFE0
			// 0x2B7 << 5 = 56E0
			if( (syncByte & 0xFFE0) == 0x56E0)
			{
				_syncType = AAC_SYNC_STREAM_TYPE_ASS;
				pCurBuf -= 2;
				break;
			}

			// Check if stream is EPAudioSyncStream
			if( (syncByte & 0xFFFF) == 0x4de1)
			{
				_syncType = AAC_SYNC_STREAM_TYPE_EP;
				pCurBuf -= 2;
				break;
			}
		}

		if(pCurBuf == pEndBuf)
		{
			AAC_ERROR("Fail to find sync of the LOAS\n");
			AAC_TRACE_END();
			return -AAC_PARSE_ERROR_SYNC_FAIL;
		}
	}

	used = pCurBuf - _pBuffer ;
	pBitStream = &bitStream;
	BITSTREAM_INIT(pCurBuf, (_length - used), pBitStream);

	switch(_syncType)
	{
	case AAC_SYNC_STREAM_TYPE_ASS:
		AAC_PRINT_LOW("PARSE ASS\n");
		remainBits = BITSTREAM_BITS_TO_DECODE(pBitStream);
		result = ADEC_AAC_ParseAudioSyncStream(pBitStream, &(_pLoas->syncStream.ass));
		used += (remainBits - (BITSTREAM_BITS_TO_DECODE(pBitStream)) + 7) >> 3;
		
		if(result >= 0)
		{
			used += _pLoas->syncStream.ass.audioMuxLengthBytes;
			AAC_PRINT_LOW("ASS Pass Complete. Total %d Bytes used\n",used);
			AAC_TRACE_END();
			if( used <= _length)
			{
				if(_used)
				{
					*_used = used;
				}
				AAC_TRACE_END();
				return result;
			}
			else
			{
				if(_used)
				{
					*_used = _length;
				}
				AAC_TRACE_END();
				return -AAC_PARSE_ERROR_NEED_MORE_DATA;
			}
		}
		if(_used)
		{
			*_used = used;
		}
		AAC_TRACE_END();
		return result;

	case AAC_SYNC_STREAM_TYPE_APS:
		AAC_ERROR("AudioPointerStream parser is not implemented\n");
		AAC_TRACE_END();
		return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
		
	case AAC_SYNC_STREAM_TYPE_EP:
		AAC_ERROR("EPAusioSyncStream parser is not implemented\n");
		AAC_TRACE_END();
		return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;

	default:
		AAC_ERROR("Unexpected stream type of LOAS\n");
		AAC_TRACE_END();
		return -AAC_PARSE_ERROR_SYNTAX;
	}

	AAC_TRACE_END();

	return result;
}