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


/** @file adec_aac_asc.c
 *
 * Implementation of AAC AudioSpecificConfig.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       AAC AudioSpecificConfig parser.
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
#include "adec_aac_debug.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
unsigned int gAacSampleFrequencyTable[0xd] = 
{
	96000, //AAC_SAMPLE_FREQ_96000 = 0x0,
	88200, //AAC_SAMPLE_FREQ_88200 = 0x1,
	64000, //AAC_SAMPLE_FREQ_64000 = 0x2,
	48000, //AAC_SAMPLE_FREQ_48000 = 0x3,
	44100, //AAC_SAMPLE_FREQ_44100 = 0x4,
	32000, //AAC_SAMPLE_FREQ_32000 = 0x5,
	24000, //AAC_SAMPLE_FREQ_24000 = 0x6,
	22050, //AAC_SAMPLE_FREQ_22050 = 0x7,
	16000, //AAC_SAMPLE_FREQ_16000 = 0x8,
	12000, //AAC_SAMPLE_FREQ_12000 = 0x9,
	11025, //AAC_SAMPLE_FREQ_11025 = 0xa,
	8000, //AAC_SAMPLE_FREQ_8000 = 0xb,
	7350, //AAC_SAMPLE_FREQ_7350 = 0xc,
};

char* gAacAudioObjectTypeNameTable[42] = 
{
	NULL,
	"AAC MAIN", //1
	"AAC LC", //2
	"AAC SSR", //3
	"AAC LTP", //4
	"SBR", //5
	"AAC scalable", //6
	"TwinVQ", // 7
	"CELP", //8
	"HVXC", //9
	"(reserved)", //10
	"(reserved)", //11
	"TTSI", //12
	"Main synthetic", //13
	"Wavetable synthesis", //14
	"General MIDI ", //15
	"Algorithmic Synthesis", //16
	"ER AAC LC", //17
	"(reserved)", //18
	"ER AAC LTP", //19
	"ER AAC scalable", //20
	"ER Twin VQ", //21
	"ER BSAC", //22
	"ER AAC LD", //23
	"ER CELP", //24
	"ER HVXC", //25
	"ER HILN", //26
	"ER Parametric", //27
	"SSC", //28
	"PS", //29
	"MPEG Surround", //30
	"(escape)", //31
	"Layer-1", //32
	"Layer-2", //33
	"Layer-3", //34
	"DST", //35
	"ALS", //36
	"SLS", //37
	"SLS non-core", //38
	"ER AAC ELD", //39
	"SMR Simple", //40
	"SMR Main" //41
};


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

static int program_config_element(ADEC_AAC_BITSTREAM_T *, ADEC_AAC_PROGRAM_CONFIG_ELEMENT_T *);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/
static int program_config_element(ADEC_AAC_BITSTREAM_T *pBitStream, ADEC_AAC_PROGRAM_CONFIG_ELEMENT_T* pConfig)
{
	unsigned char* pCurBuffer;
	unsigned int bufferLen;
	unsigned int i;
	BITSTREAM_READ_BITS(pConfig->element_instance_tag, 4, pBitStream);
	BITSTREAM_READ_BITS(pConfig->object_type, 2, pBitStream);
	BITSTREAM_READ_BITS(pConfig->sampling_frequency_index, 4, pBitStream);
	BITSTREAM_READ_BITS(pConfig->num_front_channel_elements, 4, pBitStream);
	BITSTREAM_READ_BITS(pConfig->num_side_channel_elements, 4, pBitStream);
	BITSTREAM_READ_BITS(pConfig->num_back_channel_elements, 4, pBitStream);
	BITSTREAM_READ_BITS(pConfig->num_lfe_channel_elements, 2, pBitStream);
	BITSTREAM_READ_BITS(pConfig->num_assoc_data_elements, 3, pBitStream);
	BITSTREAM_READ_BITS(pConfig->num_valid_cc_elements, 4, pBitStream);
	BITSTREAM_READ_1BIT(pConfig->mono_mixdown_present, pBitStream);
	if( 1 == pConfig->mono_mixdown_present )
	{
		BITSTREAM_READ_BITS(pConfig->mono_mixdown_element_number, 4, pBitStream);
	}
	BITSTREAM_READ_1BIT(pConfig->stereo_mixdown_present, pBitStream);
	if( 1 == pConfig->stereo_mixdown_element_number )
	{
		BITSTREAM_READ_BITS(pConfig->stereo_mixdown_element_number, 4, pBitStream);
	}
	BITSTREAM_READ_1BIT(pConfig->matrix_mixdown_idx_present, pBitStream);
	if( 1 == pConfig->matrix_mixdown_idx_present )
	{
		BITSTREAM_READ_BITS(pConfig->matrix_mixdown_idx, 2, pBitStream);
		BITSTREAM_READ_1BIT(pConfig->psedon_surround_enable, pBitStream);
	}
	for(i = 0; i < pConfig->num_front_channel_elements; i++)
	{
		BITSTREAM_READ_1BIT(pConfig->front[i].element_is_cpe, pBitStream);
		BITSTREAM_READ_BITS(pConfig->front[i].element_tag_select, 4, pBitStream);
	}
	for(i = 0; i < pConfig->num_side_channel_elements; i++)
	{
		BITSTREAM_READ_1BIT(pConfig->side[i].element_is_cpe, pBitStream);
		BITSTREAM_READ_BITS(pConfig->side[i].element_tag_select, 4, pBitStream);
	}
	for(i = 0; i < pConfig->num_back_channel_elements; i++)
	{
		BITSTREAM_READ_1BIT(pConfig->back[i].element_is_cpe, pBitStream);
		BITSTREAM_READ_BITS(pConfig->back[i].element_tag_select, 4, pBitStream);
	}
	for(i = 0; i < pConfig->num_lfe_channel_elements; i++)
	{
		BITSTREAM_READ_1BIT(pConfig->lfe[i].element_is_cpe, pBitStream);
		BITSTREAM_READ_BITS(pConfig->lfe[i].element_tag_select, 4, pBitStream);
	}
	for(i = 0; i < pConfig->num_assoc_data_elements; i++)
	{
		BITSTREAM_READ_1BIT(pConfig->assoc[i].element_is_cpe, pBitStream);
		BITSTREAM_READ_BITS(pConfig->assoc[i].element_tag_select, 4, pBitStream);
	}
	for(i = 0; i < pConfig->num_valid_cc_elements; i++)
	{
		BITSTREAM_READ_1BIT(pConfig->valid_cc[i].element_is_cpe, pBitStream);
		BITSTREAM_READ_BITS(pConfig->valid_cc[i].element_tag_select, 4, pBitStream);
	}

	pCurBuffer = pBitStream->byteBuffer;
	bufferLen = pBitStream->byteRemain;

	AAC_PRINT("Buffer position = 0x%08X (len = %d)\n",pCurBuffer, bufferLen);

	pConfig->comment_field_bytes = pCurBuffer[0];

	if(pConfig->comment_field_bytes != 0)
	{
		memcpy(pConfig->comment_field_data, pCurBuffer + 1, pConfig->comment_field_bytes);
	}

	pCurBuffer += 1 + pConfig->comment_field_bytes;
	bufferLen -= 1 + pConfig->comment_field_bytes;

	BITSTREAM_INIT(pCurBuffer,bufferLen,pBitStream);

	return 0;
	
}

/**
 * Parse the GASpecificConfig.
 *
 * @param	pBitStream - pointer of the structure containing information about the current bitstream.
 * @param	pConfig - The pointer of the structure, the parse result will be stored. 
 *				This structure should be allocated by the caller.
 * @return	greater than or equlas to 0 - Total bits, consumed, during parsing.
 * @return	otherwise - Error occures during parsing.
*/

extern int ADEC_AAC_ParseGASoecific2(
	ADEC_AAC_BITSTREAM_T* pBitStream, 
	unsigned int samplingFrequencIndex,
	unsigned int channelConfiguration,
	unsigned int audioObjectType,
	ADEC_AAG_GA_SPECIFIC_INFO_T* pConfig)
{
	unsigned int bitRemainAtStart;
	unsigned int bitRemainAtEnd;

	AAC_TRACE_BEGIN();

	bitRemainAtStart = BITSTREAM_BITS_TO_DECODE(pBitStream);

	memset(pConfig, 0, sizeof(ADEC_AAG_GA_SPECIFIC_INFO_T));

	BITSTREAM_READ_1BIT(pConfig->frameLengthFlag, pBitStream);
	BITSTREAM_READ_1BIT(pConfig->dependsOnCoreCoder, pBitStream);

	if( 0 != pConfig->dependsOnCoreCoder )
	{
		BITSTREAM_READ_BITS(pConfig->coreCoderDelay, 14, pBitStream);
	}

	BITSTREAM_READ_1BIT(pConfig->extensionFlag, pBitStream);

	if( 0 == channelConfiguration)
	{
		program_config_element(pBitStream, &pConfig->programConfig);
	}

	if( 6 == audioObjectType || 20 == audioObjectType)
	{
		BITSTREAM_READ_BITS(pConfig->layerNr, 3, pBitStream);
	}

	if( 0 != pConfig->extensionFlag )
	{
		if( 22 == audioObjectType )
		{
			BITSTREAM_READ_BITS(pConfig->numOfSubFrame, 5, pBitStream);
			BITSTREAM_READ_BITS(pConfig->layer_length, 11, pBitStream);
		}
		if( 17 == audioObjectType ||
			19 == audioObjectType ||
			20 == audioObjectType ||
			23 == audioObjectType)
		{
			BITSTREAM_READ_1BIT(pConfig->aacSectionDataResilienceFlag, pBitStream);
			BITSTREAM_READ_1BIT(pConfig->aacScalefactorDataResilienceFlag, pBitStream);
			BITSTREAM_READ_1BIT(pConfig->aacSpectralDataResilienceFlag, pBitStream);
		}

		BITSTREAM_READ_1BIT(pConfig->extensionFlag3, pBitStream);
	}

	bitRemainAtEnd = BITSTREAM_BITS_TO_DECODE(pBitStream);

	AAC_TRACE_END();
	return bitRemainAtStart - bitRemainAtEnd;
}

/**
 * Parse the AudioSpecificConfig.
 *
 * @param	pBitStream - pointer of the structure containing information about the current bitstream.
 * @param	pConfig - The pointer of the structure, the parse result will be stored. 
 *				This structure should be allocated by the caller.
 * @return	greater than or equlas to 0 - Total bits, consumed, during parsing.
 * @return	otherwise - Error occures during parsing.
*/

int ADEC_AAC_ParseASC2(ADEC_AAC_BITSTREAM_T* pBitStream, ADEC_AAC_ASC_T* pConfig)
{
	unsigned int bitRemainAtStart;
	unsigned int bitRemainAtEnd;

	AAC_TRACE_BEGIN();
	bitRemainAtStart = BITSTREAM_BITS_TO_DECODE(pBitStream);

	memset(pConfig, 0, sizeof(ADEC_AAC_ASC_T));

	BITSTREAM_READ_BITS(pConfig->audioObjectType, 5,pBitStream);

	AAC_PRINT("AudioObjectType = %d(%s)\n", 
		pConfig->audioObjectType, 
		gAacAudioObjectTypeNameTable[pConfig->audioObjectType]);

	if(31 == pConfig->audioObjectType)
	{
		BITSTREAM_READ_BITS(pConfig->audioObjectType, 6,pBitStream);
		pConfig->audioObjectType += 32;
	}

	BITSTREAM_READ_BITS(pConfig->samplingFrequencyIndex, 4,pBitStream);

	
	if(0x0f == pConfig->samplingFrequencyIndex)
	{
		BITSTREAM_READ_BITS(pConfig->samplingFrequency, 24,pBitStream);
	}
	else
	{
		pConfig->samplingFrequency = AAC_SAMPLE_FREQUENCY(pConfig->samplingFrequencyIndex);
	}

	BITSTREAM_READ_BITS(pConfig->channelConfiguration, 4,pBitStream);

	AAC_PRINT("Sampling Frequency Index = %d(%dhz)\n",
		pConfig->samplingFrequencyIndex,
		pConfig->samplingFrequency);

	AAC_PRINT("Channel Configuration = %d\n",
		pConfig->channelConfiguration);

	pConfig->sbrPresentFlag = 0;
	pConfig->psPresentFlag = 0;

	if(	5 == pConfig->audioObjectType ||
		29 == pConfig->audioObjectType)
	{
		pConfig->extensionAudioObjectType = 5;
		pConfig->sbrPresentFlag = 1;
		if( 29 == pConfig->audioObjectType)
		{
			pConfig->psPresentFlag = 1;
		}
		BITSTREAM_READ_BITS(pConfig->extensionSamplingFrequencyIndex, 4,pBitStream);
		if(0x0f == pConfig->extensionSamplingFrequencyIndex)
		{
			BITSTREAM_READ_BITS(pConfig->extensionSamplingFrequency, 24,pBitStream);
		}
		else
		{
			pConfig->extensionSamplingFrequency = 
				AAC_SAMPLE_FREQUENCY(pConfig->extensionSamplingFrequencyIndex);
		}

		BITSTREAM_READ_BITS(pConfig->audioObjectType, 5,pBitStream);

		if(31 == pConfig->audioObjectType)
		{
			BITSTREAM_READ_BITS(pConfig->audioObjectType, 6,pBitStream);
			pConfig->audioObjectType += 32;
		}

		if(22 == pConfig->audioObjectType)
		{
			BITSTREAM_READ_BITS(pConfig->extensionChannelConfiguration, 4,pBitStream);
		}
	}
	else
	{
		pConfig->extensionAudioObjectType = 0;
	}

	switch(pConfig->audioObjectType)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			ADEC_AAC_ParseGASoecific2(pBitStream, 
				pConfig->samplingFrequencyIndex, 
				pConfig->channelConfiguration,
				pConfig->audioObjectType,
				&pConfig->specificInfo.gaSpecific);
			pConfig->specificInfoType = AAC_ASC_SPECIFIC_GA;
			break;
		case 8:
			//CelpSpecificConfig();
			AAC_ERROR("CelpSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 9:
			//HvxcSpecificConfig();
			AAC_ERROR("HvxcSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 12:
			//TTSSpecificConfig();
			AAC_ERROR("TTSSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 13:
		case 14:
		case 15:
		case 16:
			//StructuredAudioSpecificConfig();
			AAC_ERROR("StructuredAudioSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 24:
			//ErrorResilientCelpSpecificConfig();
			AAC_ERROR("ErrorResilientCelpSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 25:
			//ErrorResilientHvxcSpecificConfig();
			AAC_ERROR("ErrorResilientHvxcSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 26:
		case 27:
			//ParametricSpecificConfig();
			AAC_ERROR("ParametricSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 28:
			//SSCSpecificConfig();
			AAC_ERROR("SSCSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 30:
			//sacPaylodEnbedding;  //1
			AAC_ERROR("SpationSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			//SpationSpecificConfig();
			break;
		case 32:
		case 33:
		case 34:
			//MPEG_1_2_SpecificConfig();
			AAC_ERROR("MPEG_1_2_SpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 35:
			//DSTSpecificConfig();
			AAC_ERROR("DSTSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 36:
			//fillBlts; //5
			AAC_ERROR("ALSSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			//ALSSpecificConfig();
			break;
		case 37:
		case 38:
			//SLSSpecificConfig();
			AAC_ERROR("SLSSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 39:
			//ELDSpecificConfig(channelConfiguration);
			AAC_ERROR("ELDSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		case 40:
		case 41:
			//SymbolicMusicSpecificConfig();
			AAC_ERROR("SymbolicMusicSpecificConfig is not implemented yet\n");
			return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;
			break;
		default:
			break;
	}

	switch(pConfig->audioObjectType)
	{
		case 17:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 39:
			BITSTREAM_READ_BITS(pConfig->epConfig, 2,pBitStream);
			if( 2 == pConfig->epConfig || 3 == pConfig->epConfig)
			{
				// ErrorProtectionSpecificConfig();
				AAC_ERROR("ErrorProtectionSpecificConfig is not implemented yet\n");
				return -AAC_PARSE_ERROR_NOT_IMPLEMENTED;			
			}
			if( 3 == pConfig->epConfig )
			{
				BITSTREAM_READ_1BIT(pConfig->directMapping, pBitStream);
			}
			break;
		default:
			break;
	}

	if( 5 != pConfig->extensionAudioObjectType &&
		BITSTREAM_BITS_TO_DECODE(pBitStream) >= 16)
	{
		BITSTREAM_READ_BITS(pConfig->syncExtensionType, 11,pBitStream);

		if( 0x2b7 == pConfig->syncExtensionType )
		{
			BITSTREAM_READ_BITS(pConfig->extensionAudioObjectType, 5,pBitStream);

			if(31 == pConfig->extensionAudioObjectType)
			{
				BITSTREAM_READ_BITS(pConfig->extensionAudioObjectType, 6,pBitStream);
				pConfig->extensionAudioObjectType += 32;
			}
		}

		if( 5 == pConfig->extensionAudioObjectType )
		{
			BITSTREAM_READ_1BIT(pConfig->sbrPresentFlag, pBitStream);
			if( 1 == pConfig->sbrPresentFlag )
			{
				BITSTREAM_READ_BITS(pConfig->extensionSamplingFrequencyIndex, 4,pBitStream);
				if( 0x0f == pConfig->extensionSamplingFrequencyIndex )
				{
					BITSTREAM_READ_BITS(pConfig->extensionSamplingFrequency, 24,pBitStream);
				}
				else
				{
					pConfig->extensionSamplingFrequency = 
						AAC_SAMPLE_FREQUENCY(pConfig->extensionSamplingFrequencyIndex);
				}

				if( BITSTREAM_BITS_TO_DECODE(pBitStream) >= 12 )
				{
					BITSTREAM_READ_BITS(pConfig->syncExtensionType, 11,pBitStream);
					if( 0x548 == pConfig->syncExtensionType )
					{
						BITSTREAM_READ_1BIT(pConfig->psPresentFlag, pBitStream);
					}
				}
			}
		}
		else
		{
			BITSTREAM_READ_BITS(pConfig->sbrPresentFlag, 1,pBitStream);
			if( 1 == pConfig->sbrPresentFlag )
			{
				BITSTREAM_READ_BITS(pConfig->extensionSamplingFrequencyIndex, 4,pBitStream);
				if( 0x0f == pConfig->extensionSamplingFrequencyIndex )
				{
					BITSTREAM_READ_BITS(pConfig->extensionSamplingFrequency, 24,pBitStream);
				}
				else
				{
					pConfig->extensionSamplingFrequency = 
						AAC_SAMPLE_FREQUENCY(pConfig->extensionSamplingFrequencyIndex);
				}
			}
			BITSTREAM_READ_BITS(pConfig->extensionChannelConfiguration, 4, pBitStream);
		}
	}

	bitRemainAtEnd = BITSTREAM_BITS_TO_DECODE(pBitStream);

	// Print whoe result
#if (AAC_OPT_PRINT_ASC_MEMBER == 1)
	ADEC_PRINT_MEMBER(INT,pConfig,audioObjectType);
	ADEC_PRINT_MEMBER(INT,pConfig,samplingFrequencyIndex);
	ADEC_PRINT_MEMBER(INT,pConfig,samplingFrequency);
	ADEC_PRINT_MEMBER(INT,pConfig,channelConfiguration);
	ADEC_PRINT_MEMBER(INT,pConfig,sbrPresentFlag);
	ADEC_PRINT_MEMBER(INT,pConfig,psPresentFlag);
	ADEC_PRINT_MEMBER(INT,pConfig,epConfig);
	ADEC_PRINT_MEMBER(INT,pConfig,directMapping);
	ADEC_PRINT_MEMBER(INT,pConfig,syncExtensionType);
	ADEC_PRINT_MEMBER(INT,pConfig,extensionAudioObjectType);
	ADEC_PRINT_MEMBER(INT,pConfig,extensionSamplingFrequencyIndex);
	ADEC_PRINT_MEMBER(INT,pConfig,extensionSamplingFrequency);
	ADEC_PRINT_MEMBER(INT,pConfig,extensionChannelConfiguration);
#endif

	AAC_TRACE_END();
	return bitRemainAtStart - bitRemainAtEnd;
	
}


/**
 * Parse the AudioSpecificConfig. This function is a wrapper to build a bitstream.
 *
 * @param 	pBuffer - The pointer of the memory, the AudioSpecificConfig is stored.
 * @param	bufferLength - Total length of the data.
 * @param	pConfig - The pointer of the structure, the parse result will be stored. 
 *				This structure should be allocated by the caller.
 * @return	greater than or equlas to 0 - Total bits, consumed, during parsing.
 * @return	otherwise - Error occures during parsing.
*/
int ADEC_AAC_ParseASC(unsigned char* pBuffer, unsigned int bufferLength, ADEC_AAC_ASC_T* pConfig)
{
	ADEC_AAC_BITSTREAM_T bitStream;

	BITSTREAM_INIT(pBuffer, bufferLength, (&bitStream));

	return ADEC_AAC_ParseASC2(&bitStream, pConfig);
}


