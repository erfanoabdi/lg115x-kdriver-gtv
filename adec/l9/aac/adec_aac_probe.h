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

/** @file adec_aac_probe.h
 *
 *  This is a header file for adec_aac_probe.h
 *  The major functionality of the AAC probe is to detect the AAC profile and format.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.26
 *  @note       AAC Probe.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */



#ifndef _ADEC_AAC_PROBE_H_
#define _ADEC_AAC_PROBE_H_

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
	typedef struct _ADEC_AAC_MEDIA_INFO_T
	{
		unsigned int samplingFrequency;
		unsigned int extensionSamplingFrequency;
		unsigned int channelConfiguration;
		unsigned int extensionChannelConfiguration;
		unsigned int psPresentFlag;
		unsigned int sbrPresentFlag;
		unsigned int objectType;
		unsigned int extensionObjectType;
	}ADEC_AAC_MEDIA_INFO_T;
/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
extern void* ADEC_AAC_Probe_Create(void);
extern int ADEC_AAC_Probe_CheckEs(unsigned char* pBuffer, unsigned int length, void* pProbe);
extern int ADEC_AAC_Probe_SetDsi(unsigned char* pBuffer, unsigned int length, void* pProbe);
extern ADEC_AAC_CODEC_TYPE_T ADEC_AAC_Probe_GetCodecType(void* pProbe);
extern ADEC_AAC_FORMAT_TYPE_T ADEC_AAC_Probe_GetFormatType(void* pProbe);
extern int ADEC_AAC_Probe_GetMediaInfo(ADEC_AAC_MEDIA_INFO_T* mediaInfo, void* pProbe);
extern void ADEC_AAC_Probe_Terminate(void* pProbe);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_PROBE_H_ */

