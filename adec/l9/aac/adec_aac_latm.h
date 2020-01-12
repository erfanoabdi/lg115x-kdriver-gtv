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

/** @file adec_aac_internal.h
 *
 *  This file contains defitions, the AAC brobe module use internaly.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       AudioSpecificConfig parser.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

#ifndef _ADEC_AAC_LATM_H_
#define _ADEC_AAC_LATM_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

#include "adec_aac_config.h"
#include "adec_aac_generic.h"
#include "adec_aac_asc.h"

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

typedef struct _ADEC_AAC_LATM_FRMAE_INDEX_T
{
	unsigned char		progIndex : 4;
	unsigned char		layIndex : 3;
}ADEC_AAC_LATM_FRAME_INDEX_T;

typedef struct _ADEC_AAC_LATM_STREAM_INFO_T
{
	unsigned int	frameLength				: 9;
	unsigned int	latmBufferFullness		: 8;
	unsigned int	muxSlotLengthByte		: 8;
	unsigned int	coreFrameOffset			: 6;
	unsigned int	frameLengthType			: 3;
	unsigned short	CECLPframeLengthTableIndex: 6;
	unsigned short	muxSlotLengthCoded		: 2;
	unsigned short	HVXCframeLengthTableIndex: 1;
	unsigned short	AuEndFlag				: 1;
	ADEC_AAC_LATM_FRAME_INDEX_T		sIndex;
}ADEC_AAC_LATM_STREAM_INFO_T;
typedef struct _ADEC_AAC_LATM_LAYER_INFO_T
{
	ADEC_AAC_ASC_T	audioSpecificConfig;
	unsigned char	useSameConfig			: 1;
	unsigned char	streamID				: 6;
}ADEC_AAC_LATM_LAYER_INFO_T;

typedef struct _ADEC_AAC_LATM_PROGRAM_INFO_T
{
	ADEC_AAC_LATM_LAYER_INFO_T layerInfo[8];
	unsigned char		numLayer;
}ADEC_AAC_LATM_PROGRAM_INFO_T;

typedef struct _ADEC_AAC_AUDIO_MUX_ELEMENT_T
{
	ADEC_AAC_LATM_PROGRAM_INFO_T prgramInfo[16];
	ADEC_AAC_LATM_STREAM_INFO_T	 streamInfo[64];
	ADEC_AAC_LATM_FRAME_INDEX_T	 cunkIndex[16];
	unsigned int	taraBufferFullness;
	unsigned int	otherDataLenBits;
	unsigned int	crcCheckSume			: 8;
	unsigned int	numSubFrames			: 6;
	unsigned int	numProgram				: 4;
	unsigned int	audioMuxVersion			: 1;
	unsigned int	audioMuxVersionA		: 1;
	unsigned int	allStreamsSameTimeFraming: 1;
	unsigned int	useSameStreamMux		: 1;
	unsigned int	otherDataBit			: 1;
	unsigned int	muxConfigPresent		: 1;
	unsigned int	otherDataPresent		: 1;
	unsigned int	creCheckPresent			: 1;
}ADEC_AAC_AUDIO_MUX_ELEMENT_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern int ADEC_AAC_ParseAudioMuxElement(unsigned char* _pBuffer, 
	unsigned int _length, 
	unsigned int _muxConfigPresent,
	ADEC_AAC_AUDIO_MUX_ELEMENT_T* _pMuxElement);

extern int ADEC_AAC_ParseAudioMuxElement2(ADEC_AAC_BITSTREAM_T* _pBitStream,
	unsigned int _muxConfigPresent,
	ADEC_AAC_AUDIO_MUX_ELEMENT_T* _pMuxElement);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/


#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_LATM_H_ */
