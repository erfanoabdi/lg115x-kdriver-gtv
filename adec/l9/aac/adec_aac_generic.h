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

/** @file adec_aac_generic.h
 *
 *  This file contains basic structures and features for processing AAC stream.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       Basic definitions for processing AAC.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */



#ifndef _ADEC_AAC_GENERIC_H_
#define _ADEC_AAC_GENERIC_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#include "adec_aac_config.h"


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


typedef enum _ADEC_AAC_CODEC_TYPE_T
{
	AAC_CODEC_TYPE_NOT_DEF = 0x00,
	AAC_CODEC_TYPE_AAC,
	AAC_CODEC_TYPE_HE_AAC,
	AAC_CODEC_TYPE_HE_AAC_V2,

	////////////////////////////////////////////////////////////////////
	// Bellow value make the sizeof this enum type to be 4byte.
	AAC_CODEC_TYPE_BOUND = 0x7FFFFFFF
}ADEC_AAC_CODEC_TYPE_T;

typedef enum _ADEC_AAC_FORMAT_TYPE_T
{
	AAC_FORMAT_TYPE_RAW = 0x00,
	AAC_FORMAT_TYPE_ADTS,
	AAC_FORMAT_TYPE_ADIF,
	AAC_FORMAT_TYPE_LOAS,
	//AAC_FORMAT_TYPE_LATM,
	AAC_FORMAT_TYPE_MAX,

	////////////////////////////////////////////////////////////////////
	// Bellow value make the sizeof this enum type to be 4byte.
	AAC_FORMAT_TYPE_BOUND = 0x7FFFFFFF
}ADEC_AAC_FORMAT_TYPE_T;

typedef enum _ADEC_AAC_SAMPLING_FREQUENCY_T
{
	AAC_SAMPLE_FREQ_96000 = 0x0,
	AAC_SAMPLE_FREQ_88200 = 0x1,
	AAC_SAMPLE_FREQ_64000 = 0x2,
	AAC_SAMPLE_FREQ_48000 = 0x3,
	AAC_SAMPLE_FREQ_44100 = 0x4,
	AAC_SAMPLE_FREQ_32000 = 0x5,
	AAC_SAMPLE_FREQ_24000 = 0x6,
	AAC_SAMPLE_FREQ_22050 = 0x7,
	AAC_SAMPLE_FREQ_16000 = 0x8,
	AAC_SAMPLE_FREQ_12000 = 0x9,
	AAC_SAMPLE_FREQ_11025 = 0xa,
	AAC_SAMPLE_FREQ_8000 = 0xb,
	AAC_SAMPLE_FREQ_7350 = 0xc,

	////////////////////////////////////////////////////////////////////
	// Bellow value make the sizeof this enum type to be 4byte.
	AAC_SAMPLE_FREQ_BOUND = 0x7FFFFFFF
}_ADEC_AAC_SAMPLING_FREQUENCY_T;

typedef enum _ADEC_AAC_PARSE_ERROR_T
{
	AAC_PARSE_ERROR_NONE = 0,
	AAC_PARSE_ERROR_NEED_MORE_DATA,
	AAC_PARSE_ERROR_SYNTAX,
	AAC_PARSE_ERROR_SYNC_FAIL,
	AAC_PARSE_ERROR_NEXT_SYNC_FAIL,
	AAC_PARSE_ERROR_NOT_IMPLEMENTED,
	AAC_PARSE_ERROR_GENERIG,
}ADEC_AAC_PARSE_ERROR;


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

extern const char* gAacFormatTypeString[AAC_FORMAT_TYPE_MAX];


#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_GENERIC_H_ */