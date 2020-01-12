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

/** @file adec_aac_asc.h
 *
 *  This is a header file for adec_aac_asc.h
 *  Structres and functions , needed to parse the AAC AudioSpecificConfig, is defined here.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       AudioSpecificConfig parser.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */



#ifndef _ADEC_AAC_ASC_H_
#define _ADEC_AAC_ASC_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "adec_aac_generic.h"
#include "adec_aac_bitstream.h"
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

typedef enum _ADEC_AAC_ASC_SPECIFIC_CONFIG_TYPE_T
{
	AAC_ASC_SPECIFIC_NONE = 0,
	AAC_ASC_SPECIFIC_GA,
	AAC_ASC_SPECIFIC_CELP,
	AAC_ASC_SPECIFIC_HVXC,
	AAC_ASC_SPECIFIC_TTS,
	AAC_ASC_SPECIFIC_STRUCTURED_AUDIO,
	AAC_ASC_SPECIFIC_ER_CELP,
	AAC_ASC_SPECIFIC_ER_HVXC,
	AAC_ASC_SPECIFIC_PARAMETRIC,
	AAC_ASC_SPECIFIC_SSC,
	AAC_ASC_SPECIFIC_SPATIAL,
	AAC_ASC_SPECIFIC_MPEG_1_2,
	AAC_ASC_SPECIFIC_DST,
	AAC_ASC_SPECIFIC_ALS,
	AAC_ASC_SPECIFIC_SLS,
	AAC_ASC_SPECIFIC_ELD,
	AAC_ASC_SPECIFIC_SYMBOLIC_MUSIC,

	// This value makes the size of this enumuration to be 4 bytes.
	AAC_ASC_SPECIFIC_BOUND = 0x7FFFFFFF,
}ADEC_AAC_ASC_SPECIFIC_INFO_TYPE_T;


// GA Specific Config

typedef struct _ADEC_AAC_GA_ELEMENT_T
{
	unsigned char element_is_cpe:1;
	unsigned char element_tag_select:4;
}ADEC_AAC_GA_ELEMENT_T;

typedef struct _ADEC_AAC_PROGRAM_CONFIG_ELEMENT_T
{
	ADEC_AAC_GA_ELEMENT_T front[16];
	ADEC_AAC_GA_ELEMENT_T side [16];
	ADEC_AAC_GA_ELEMENT_T back [16];
	ADEC_AAC_GA_ELEMENT_T lfe[4];
	ADEC_AAC_GA_ELEMENT_T assoc [8];
	ADEC_AAC_GA_ELEMENT_T valid_cc[16];
	unsigned int element_instance_tag:4;
	unsigned int object_type:2;
	unsigned int sampling_frequency_index:4;
	unsigned int num_front_channel_elements:4;
	unsigned int num_side_channel_elements:4;
	unsigned int num_back_channel_elements:4;
	unsigned int num_lfe_channel_elements:2;
	unsigned int num_assoc_data_elements:3;
	unsigned int num_valid_cc_elements:4;
	unsigned int mono_mixdown_present:1;
	unsigned short mono_mixdown_element_number:4;
	unsigned short stereo_mixdown_present:1;
	unsigned short stereo_mixdown_element_number:4;
	unsigned short matrix_mixdown_idx_present:1;
	unsigned short matrix_mixdown_idx:2;
	unsigned short psedon_surround_enable:1;
	unsigned char comment_field_bytes;
	unsigned char comment_field_data[256];
	
}ADEC_AAC_PROGRAM_CONFIG_ELEMENT_T;

typedef struct _ADEC_AAC_GA_SPECIFIC_INFO_T
{
	ADEC_AAC_PROGRAM_CONFIG_ELEMENT_T programConfig;
	unsigned short layer_length;
	unsigned short coreCoderDelay;
	unsigned char frameLengthFlag;
	unsigned char dependsOnCoreCoder;
	unsigned char extensionFlag;
	unsigned char layerNr;
	unsigned char numOfSubFrame;
	unsigned char aacSectionDataResilienceFlag;
	unsigned char aacScalefactorDataResilienceFlag;
	unsigned char aacSpectralDataResilienceFlag;
	unsigned char extensionFlag3;
}ADEC_AAG_GA_SPECIFIC_INFO_T;

// Audio Specific Config
typedef struct _ADEC_AAC_ASC_T
{
	// currently only the GASpecificInfo is implemented.
	union
	{
		ADEC_AAG_GA_SPECIFIC_INFO_T gaSpecific;
	}specificInfo;
	ADEC_AAC_ASC_SPECIFIC_INFO_TYPE_T specificInfoType;
	unsigned int extensionSamplingFrequency :24;
	unsigned int extensionSamplingFrequencyIndex : 4;
	unsigned int extensionChannelConfiguration : 4;
	unsigned int samplingFrequency : 24;
	unsigned int samplingFrequencyIndex : 4;
	unsigned int channelConfiguration : 4;
	unsigned short syncExtensionType : 11;
	unsigned short epConfig : 2;
	unsigned short psPresentFlag : 1;
	unsigned short sbrPresentFlag : 1;
	unsigned short directMapping :1;
	unsigned char audioObjectType;
	unsigned char extensionAudioObjectType;
}ADEC_AAC_ASC_T;


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

extern int ADEC_AAC_ParseGASoecific2(
	ADEC_AAC_BITSTREAM_T* pBitStream, 
	unsigned int samplingFrequencIndex,
	unsigned int channelConfiguration,
	unsigned int audioObjectType,
	ADEC_AAG_GA_SPECIFIC_INFO_T* pConfig);
extern int ADEC_AAC_ParseASC2(ADEC_AAC_BITSTREAM_T* pBitStream, ADEC_AAC_ASC_T* pConfig);
extern int ADEC_AAC_ParseASC(unsigned char* pBuffer, unsigned int bufferLength, ADEC_AAC_ASC_T* pConfig);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_ASC_H_ */