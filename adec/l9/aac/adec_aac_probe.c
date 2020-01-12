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
#include "adec_aac_probe.h"
#include "adec_aac_debug.h"
#include "adec_aac_asc.h"
#include "adec_aac_adts.h"
#include "adec_aac_loas.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef int (*PFN_ADEC_TEST_ES_T)(unsigned char* pBuffer, unsigned int bufferLength, void* pHeader);

typedef struct _ADEC_AAC_FORMAT_ELEMENT_T
{
	void* pHeader;
	void* pAudioSpecificConfig;
	PFN_ADEC_TEST_ES_T pfnTestFunc;
	int acceptableCnt;
	int isAscParsed;
	ADEC_AAC_FORMAT_TYPE_T type;
}ADEC_AAC_FORMAT_ELEMENT_T;

typedef struct _ADEC_AAC_PROBE_T
{
	ADEC_AAC_ASC_T audioSpecificConfig;
	ADEC_AAC_FORMAT_ELEMENT_T formatElement[AAC_FORMAT_TYPE_MAX];
	int testCnt;
	int isDsiSet;
	int isAscParsed;
	ADEC_AAC_FORMAT_TYPE_T bestFormat;
}ADEC_AAC_PROBE_T;


/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
const char* gAacFormatTypeString[AAC_FORMAT_TYPE_MAX] = 
{
	"AAC Raw",
	"AAC ADTS",
	"AAC ADIF",
	"AAC LOAS/LATM",
};

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static ADEC_AAC_FORMAT_ELEMENT_T* ADEC_AAC_Probe_GetBestFormat(void* pProbe);
static ADEC_AAC_ASC_T* ADEC_AAC_Probe_FindAscInFormatElement(void* pProbe);
static ADEC_AAC_CODEC_TYPE_T ADEC_AAC_Probe_GetCodecTypeFromAsc(ADEC_AAC_ASC_T* pAsc);
static int ADEC_AAC_Probe_CheckAdtsWrapper(unsigned char* pBuffer, unsigned int bufferLength, void* pHeader);
static int ADEC_AAC_Probe_CheckLoasWrapper(unsigned char* pBuffer, unsigned int bufferLength, void* pHeader);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 * This wrapper function helps the AAC Probe to parse the ADTS header.
 *
 * @param 	pBuffer - The pointer of the memory, the AAC es stored.
 * @param	length - Total length of the data.
 * @param	pHeader - The pointer of the structure, the parse result will be stored. 
 *				This structure should be allocated by the caller.
 * @return	greater than or equlas to 0 - First byte of the adts header from pBuffer.
 * @return	otherwise - Error occures during parsing.
*/

static int ADEC_AAC_Probe_CheckAdtsWrapper(unsigned char* pBuffer, unsigned int bufferLength, void* pHeader)
{
	int acceptableCnt;
	int result;
	unsigned int curLength;
	unsigned int used;
	unsigned char* curBuf;
	curBuf = pBuffer;
	curLength = bufferLength;
	acceptableCnt = -1;
	used = 0;
	do
	{
		result = ADEC_AAC_ParseADTS(curBuf,curLength,&used,(ADEC_AAC_ADTS_HEADER_T*)pHeader);
		curBuf += used;
		curLength -= used;
		acceptableCnt ++;
	}while(result >= 0);

	if(result != -AAC_PARSE_ERROR_NEED_MORE_DATA && acceptableCnt == 0)
		return result;

	return acceptableCnt;
}

static int ADEC_AAC_Probe_CheckLoasWrapper(unsigned char* pBuffer, unsigned int bufferLength, void* pHeader)
{
	int acceptableCnt;
	int result;
	unsigned int curLength;
	unsigned int used;
	unsigned char* curBuf;
	curBuf = pBuffer;
	curLength = bufferLength;
	acceptableCnt = -1;
	used = 0;
	do
	{
		result = ADEC_AAC_ParseLoas(curBuf,curLength,&used, AAC_SYNC_STREAM_TYPE_ASS, (ADEC_AAC_LOAS_T*)pHeader);
		curBuf += used;
		curLength -= used;
		acceptableCnt ++;
	}while(result >= 0);

	if(result != -AAC_PARSE_ERROR_NEED_MORE_DATA && acceptableCnt == 0)
		return result;

	return acceptableCnt;
}


static ADEC_AAC_FORMAT_ELEMENT_T* ADEC_AAC_Probe_GetBestFormat(void* pProbe)
{
	ADEC_AAC_PROBE_T* pAacProbe = NULL;
	ADEC_AAC_FORMAT_ELEMENT_T* curBestElement = NULL;
	int bestAcceptable = 0;
	int i;

	if(pProbe == NULL)
	{
		return NULL;
	}

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;

	for(i = 1; i < AAC_FORMAT_TYPE_MAX ; i++)
	{
		if(pAacProbe->formatElement[i].acceptableCnt > bestAcceptable)
		{
			curBestElement = &pAacProbe->formatElement[i];
			bestAcceptable = curBestElement->acceptableCnt;
		}
	}

	return curBestElement;
}

static ADEC_AAC_ASC_T* ADEC_AAC_Probe_FindAscInFormatElement(void* pProbe)
{
	ADEC_AAC_PROBE_T* pAacProbe = NULL;
	ADEC_AAC_ASC_T* pAsc = NULL;
	int bestAcceptable = 0;
	int i;

	if(pProbe == NULL)
	{
		return NULL;
	}

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;

	for(i = 1; i < AAC_FORMAT_TYPE_MAX ; i++)
	{
		if(	pAacProbe->formatElement[i].acceptableCnt > bestAcceptable && 
			pAacProbe->formatElement[i].isAscParsed)
		{
			pAsc = (ADEC_AAC_ASC_T*)pAacProbe->formatElement[i].pAudioSpecificConfig;
			bestAcceptable = pAacProbe->formatElement[i].acceptableCnt;
		}
	}

	return pAsc;
}

static ADEC_AAC_CODEC_TYPE_T ADEC_AAC_Probe_GetCodecTypeFromAsc(ADEC_AAC_ASC_T* pAsc)
{
	if(pAsc == NULL)
	{
		AAC_ERROR("Null parameter error\n");
		return AAC_CODEC_TYPE_NOT_DEF;
	}

	AAC_PRINT("ASC] psPresentFlag = %d, sbrPresentFlag = %d\n", 
		pAsc->psPresentFlag, pAsc->sbrPresentFlag);
	if(pAsc->psPresentFlag && pAsc->sbrPresentFlag)
	{
		AAC_PRINT("ASC] It's a HE-AAC v2.\n");
		return AAC_CODEC_TYPE_HE_AAC_V2;
	}
	
	if(pAsc->sbrPresentFlag)
	{
		AAC_PRINT("ASC] It's a HE-AAC.\n");
		return AAC_CODEC_TYPE_HE_AAC;
	}

	AAC_PRINT("ASC] It's a AAC.\n");
	
	return AAC_CODEC_TYPE_AAC;
}


void* ADEC_AAC_Probe_Create(void)
{
	int i;
	ADEC_AAC_PROBE_T* newProbe = NULL;

	AAC_TRACE_BEGIN();
	newProbe = (ADEC_AAC_PROBE_T*)AAC_MALLOC(sizeof(ADEC_AAC_PROBE_T));
	
	if(newProbe == NULL)
	{
		AAC_ERROR("Fail to allocate memory\n");
		AAC_TRACE_END();
		return NULL;
	}

	// create adts probe
	memset(newProbe, 0, sizeof(ADEC_AAC_PROBE_T));
	newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].acceptableCnt = 0;
	newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].pfnTestFunc = &ADEC_AAC_Probe_CheckAdtsWrapper;
	newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].pAudioSpecificConfig = NULL;
	newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].isAscParsed = 0;
	newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].type = AAC_FORMAT_TYPE_ADTS;
	newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].pHeader = AAC_MALLOC(sizeof(ADEC_AAC_ADTS_HEADER_T));

	if(newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].pHeader == NULL)
	{
		AAC_ERROR("Fail to allocate memory\n");
		AAC_TRACE_END();
		goto aacProbeError;
	}

	newProbe->formatElement[AAC_FORMAT_TYPE_LOAS].acceptableCnt = 0;
	newProbe->formatElement[AAC_FORMAT_TYPE_LOAS].pfnTestFunc = &ADEC_AAC_Probe_CheckLoasWrapper;
	newProbe->formatElement[AAC_FORMAT_TYPE_LOAS].pAudioSpecificConfig = NULL;
	newProbe->formatElement[AAC_FORMAT_TYPE_LOAS].isAscParsed = 0;
	newProbe->formatElement[AAC_FORMAT_TYPE_LOAS].type = AAC_FORMAT_TYPE_LOAS;
	newProbe->formatElement[AAC_FORMAT_TYPE_LOAS].pHeader = AAC_MALLOC(sizeof(ADEC_AAC_LOAS_T));

	if(newProbe->formatElement[AAC_FORMAT_TYPE_ADTS].pHeader == NULL)
	{
		AAC_ERROR("Fail to allocate memory\n");
		AAC_TRACE_END();
		goto aacProbeError;
	}

	AAC_TRACE_END();
	return newProbe;
aacProbeError:
	if( newProbe != NULL )
	{
		for( i = 1 ; i < AAC_FORMAT_TYPE_MAX ; i++ )
		{
			if( newProbe->formatElement[i].pHeader )
			{
				AAC_FREE(newProbe->formatElement[i].pHeader);
			}
			if( newProbe->formatElement[i].pAudioSpecificConfig )
			{
				AAC_FREE(newProbe->formatElement[i].pAudioSpecificConfig);
			}
		}
		AAC_FREE(newProbe);
	}
	AAC_TRACE_END();
	return NULL;
}

int ADEC_AAC_Probe_CheckEs(unsigned char* pBuffer, unsigned int length, void* pProbe)
{
	int i;
	int result;
	int atLeastOneSuccess;
	int bestAcceptable;
	ADEC_AAC_FORMAT_TYPE_T bestFormat;
	ADEC_AAC_PROBE_T* pAacProbe;
	ADEC_AAC_FORMAT_ELEMENT_T* pCurFormat;

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;
	atLeastOneSuccess = 0;

	bestAcceptable = 0;
	bestFormat = AAC_FORMAT_TYPE_RAW;

	if(pAacProbe == NULL || pBuffer == NULL)
	{
		return -1;
	}

	for( i = 1 ; i < AAC_FORMAT_TYPE_MAX ; i++ )
	{
		pCurFormat = &(pAacProbe->formatElement[i]);
#if (AAC_OPT_SKIP_PREVIOUS_ERROR_FORMAT == 1)
		if(pCurFormat->acceptableCnt < 0)
		{
			continue;
		}
#endif //(AAC_OPT_SKIP_PREVIOUS_ERROR_FORMAT == 1)
		if(pCurFormat->pfnTestFunc != NULL)
		{
			result = pCurFormat->pfnTestFunc(pBuffer, length, pCurFormat->pHeader);
			if(result >= 0)
			{
				pCurFormat->acceptableCnt += result;
				atLeastOneSuccess ++;

				if(pCurFormat->isAscParsed != 0)
				{
					pAacProbe->isAscParsed = 1;
				}

				if(pCurFormat->acceptableCnt > bestAcceptable)
				{
					bestAcceptable = pCurFormat->acceptableCnt;
					bestFormat = (ADEC_AAC_FORMAT_TYPE_T)i;
				}
			}
			else if(result == -AAC_PARSE_ERROR_NEED_MORE_DATA)
			{
				pCurFormat->acceptableCnt ++;
				atLeastOneSuccess ++;
			}
			else
			{
				pCurFormat->acceptableCnt --;
			}
		}
	}
	pAacProbe->testCnt ++;
	
	return atLeastOneSuccess;
}

int ADEC_AAC_Probe_SetDsi(unsigned char* pBuffer, unsigned int length, void* pProbe)
{
	int result;
	ADEC_AAC_PROBE_T* pAacProbe;
	
	if(pBuffer == NULL || length == 0 || pProbe == NULL)
	{
		return -1;
	}

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;

	result = ADEC_AAC_ParseASC(pBuffer, length, &(pAacProbe->audioSpecificConfig));
	if(result > 0)
	{
		pAacProbe->isDsiSet = 1;
		pAacProbe->isAscParsed = 1;
	}
	return result;
}

ADEC_AAC_CODEC_TYPE_T ADEC_AAC_Probe_GetCodecType(void* pProbe)
{
	ADEC_AAC_PROBE_T* pAacProbe;
	ADEC_AAC_ASC_T* pSubAsc;

	if(pProbe == NULL)
	{
		return AAC_CODEC_TYPE_NOT_DEF;
	}

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;

	if(pAacProbe->isAscParsed == 0)
	{
		return AAC_CODEC_TYPE_NOT_DEF;
	}

	if(pAacProbe->isDsiSet == 1)
	{
		return ADEC_AAC_Probe_GetCodecTypeFromAsc(&pAacProbe->audioSpecificConfig);
	}

	if(pAacProbe->isAscParsed == 1)
	{
		pSubAsc = ADEC_AAC_Probe_FindAscInFormatElement(pProbe);
		if(pSubAsc != NULL)
		{
			return ADEC_AAC_Probe_GetCodecTypeFromAsc(pSubAsc);
		}
	}

	return AAC_CODEC_TYPE_NOT_DEF;
}

ADEC_AAC_FORMAT_TYPE_T ADEC_AAC_Probe_GetFormatType(void* pProbe)
{
	ADEC_AAC_PROBE_T* pAacProbe;
	ADEC_AAC_FORMAT_ELEMENT_T* pBest;

	if(pProbe == NULL)
	{
		AAC_PRINT("AAC Format type is %s\n", gAacFormatTypeString[AAC_FORMAT_TYPE_RAW]);
		return AAC_FORMAT_TYPE_RAW;
	}

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;

	if(pAacProbe->testCnt <= 0)
	{
		AAC_PRINT("AAC Format type is %s\n", gAacFormatTypeString[AAC_FORMAT_TYPE_RAW]);
		return AAC_FORMAT_TYPE_RAW;
	}

	pBest = ADEC_AAC_Probe_GetBestFormat(pProbe);

	if(pBest)
	{
		pAacProbe->bestFormat = pBest->type;
		AAC_PRINT("AAC Format type is %s\n", gAacFormatTypeString[pBest->type]);
		return pBest->type;
	}

	return AAC_FORMAT_TYPE_RAW;
}

int ADEC_AAC_Probe_GetMediaInfo(ADEC_AAC_MEDIA_INFO_T* mediaInfo, void* pProbe)
{
	ADEC_AAC_PROBE_T* pAacProbe;
	ADEC_AAC_ASC_T* pAsc;

	if(pProbe == NULL || mediaInfo == NULL)
	{
		return -1;
	}

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;

	if(pAacProbe->isAscParsed == 0)
	{
		return -2;
	}

	pAsc = NULL;

	if(pAacProbe->isDsiSet == 1)
	{
		pAsc = &pAacProbe->audioSpecificConfig;
	}
	else
	{
		if(pAacProbe->isAscParsed == 1)
		{
			pAsc = ADEC_AAC_Probe_FindAscInFormatElement(pProbe);
		}
	}

	if(pAsc == NULL)
	{
		return -3;
	}

	mediaInfo->channelConfiguration = pAsc->channelConfiguration;
	mediaInfo->extensionChannelConfiguration = pAsc->extensionChannelConfiguration;
	mediaInfo->objectType = pAsc->audioObjectType;
	mediaInfo->extensionObjectType = pAsc->extensionAudioObjectType;
	mediaInfo->samplingFrequency = pAsc->samplingFrequency;
	mediaInfo->extensionSamplingFrequency = pAsc->extensionSamplingFrequency;
	mediaInfo->psPresentFlag = pAsc->psPresentFlag;
	mediaInfo->sbrPresentFlag = pAsc->sbrPresentFlag;

	return 0;
}

void ADEC_AAC_Probe_Terminate(void* pProbe)
{
	ADEC_AAC_PROBE_T* pAacProbe;
	int i;

	if(pProbe == NULL)
	{
		return;
	}

	pAacProbe = (ADEC_AAC_PROBE_T*)pProbe;
	
	if( pAacProbe != NULL )
	{
		for( i = 1 ; i < AAC_FORMAT_TYPE_MAX ; i++ )
		{
			if( pAacProbe->formatElement[i].pHeader )
			{
				AAC_FREE(pAacProbe->formatElement[i].pHeader);
			}
			if( pAacProbe->formatElement[i].pAudioSpecificConfig )
			{
				AAC_FREE(pAacProbe->formatElement[i].pAudioSpecificConfig);
			}
		}
		AAC_FREE(pAacProbe);
	}
}



