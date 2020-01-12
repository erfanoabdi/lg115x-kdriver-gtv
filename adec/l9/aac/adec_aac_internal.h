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

#ifndef _ADEC_AAC_INTERNAL_H_
#define _ADEC_AAC_INTERNAL_H_

#ifndef _WIN32
#ifdef __XTENSA__
#else
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "os_util.h"
#endif
#else
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#endif

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

#include "adec_aac_config.h"
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

#define AAC_SAMPLE_FREQUENCY(SF_INDEX) (SF_INDEX < 0x0d ? gAacSampleFrequencyTable[SF_INDEX] : 0)
#define AAC_AUDIO_OBJECT_TYPE(AOT_INDEX) \
	((AOT_INDEX < 42 && AOT_INDEX > 0) ? gAacAudioObjectTypeNameTable[SF_INDEX] : "")

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

extern unsigned int gAacSampleFrequencyTable[];
extern char* gAacAudioObjectTypeNameTable[];

#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_INTERNAL_H_ */
