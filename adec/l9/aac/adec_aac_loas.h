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

/** @file adec_aac_loas.h
 *
 *  This is a header file for adec_aac_loas.c 
 *  Structres and functions , needed to parse the AAC LOAS, is defined here.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       AudioSpecificConfig parser.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

#ifndef _ADEC_AAC_LOAS_H_
#define _ADEC_AAC_LOAS_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "adec_aac_generic.h"
#include "adec_aac_latm.h"

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

typedef struct _ADEC_AAC_AUDIO_SYNC_STREAM_T
{
	ADEC_AAC_AUDIO_MUX_ELEMENT_T audioMuxElement;
	unsigned short		audioMuxLengthBytes;
	unsigned char*		muxElementStartPosition;
}ADEC_AAC_AUDIO_SYNC_STREAM_T;

typedef struct _ADEC_AAC_EP_ADUIO_SYNC_STREAM_T
{
	unsigned int		headerParity;
	unsigned short		audioMuxLengthBytes;
	unsigned char		futureUse;
	unsigned char		frameCounter;
}ADEC_AAC_EP_AUDIO_SYNC_STREAM_T;

typedef enum _ADEC_AAC_SYNC_STREAM_TYPE_T
{
	AAC_SYNC_STREAM_TYPE_NOT_DEF = 0,
	AAC_SYNC_STREAM_TYPE_ASS,	// AudioSyncStream
	AAC_SYNC_STREAM_TYPE_EP,	// EPAudioSyncStream
	AAC_SYNC_STREAM_TYPE_APS,	// AudioPointerStream
	AAC_SYNC_STREAM_TYPE_BOUND = 0x7FFFFFFF
}ADEC_AAC_SYNC_STREAM_TYPE_T;

typedef struct _ADEC_AAC_LOAS_T
{
	ADEC_AAC_SYNC_STREAM_TYPE_T syncType;
	union 
	{
		ADEC_AAC_AUDIO_SYNC_STREAM_T ass;
		ADEC_AAC_EP_AUDIO_SYNC_STREAM_T ep;
	} syncStream;
}ADEC_AAC_LOAS_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern int ADEC_AAC_ParseLoas(
	unsigned char* _pBuffer, 
	unsigned int _length, 
	unsigned int* _used,
	ADEC_AAC_SYNC_STREAM_TYPE_T _syncType,
	ADEC_AAC_LOAS_T* _pLoas);


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_PROBE_H_ */
