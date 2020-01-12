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

/** @file
 *
 *  LG Sound Engine default parameter data header for adec device.
 *
 *  @author		Jongsang Oh (jongsang.oh@lge.com)
 *  @version	1.0
 *  @date		2011.03.08
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

#ifndef	_ADEC_SE_PARAM_H_
#define	_ADEC_SE_PARAM_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "debug_util.h"
#include "adec_cfg.h"
#include "adec_kapi.h"

#include <mach/hardware.h> // For Register base address

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
//This LG Sound Engine's parameter is used in GP3 model.
/**
 * Audio ClearVoice III - AutoVolume Parameter Definition
 *
 */
const	UINT32	gADEC_CVAVLParams[LX_CV_AUTOVOLUME_PARAM_NUM] =
{
	/*	========  AVL parameters  ========
		PARAM1, 	PARAM2, 	PARAM3, 	PARAM4, 	PARAM5, 	PARAM6, 	PARAM7, 	PARAM8, 	PARAM9, 	PARAM10,
		PARAM11,	PARAM12,	PARAM13,	PARAM14,	PARAM15,	PARAM16,	PARAM17,	PARAM18,	PARAM19,	PARAM20,
		PARAM21,	PARAM22,	PARAM23
	*/
	//	CV3 Auto Volume 0FF//
		0x00000000, 0x0007ae14, 0x0003d4b6, 0xffe18add, 0x00a00000, 0x00a00000, 0x0050f44d, 0x00653161, 0x0075b37e, 0x0075b37e,
		0x001d6cdf, 0x0074b1f6, 0x0069d6fc, 0x00025a1c, 0xffc00000, 0xffd9999a, 0x00222222, 0x00200000, 0x0050f44d, 0x00000005,
		0x00000025, 0x000022f3, 0x00111111
};

/**
* Audio ClearVoice II parameter Definition
*
*/
const	UINT32	gADEC_CVParams[LX_CV_CV2_PARAM_NUM] =
{
	/*	========  Clear Voice 2 parameters	========
	PARAM1, 	PARAM2,
	PARAM3, 	PARAM4, 	PARAM5, 	PARAM6, 	PARAM7, 	PARAM8, 	PARAM9, 	PARAM10,
	PARAM11,	PARAM12,	PARAM13,	PARAM14,	PARAM15,	PARAM16,	PARAM17,	PARAM18,	PARAM19,	PARAM20,
	PARAM21,	PARAM22,	PARAM23,	PARAM24,	PARAM25
	*/
	//CV2 Level 0 : UI Lev. 0
	0x00100000, 0x00100000,
	0x00044444, 0x001e0000, 0xffe20000, 0x004b0000, 0xffb50000, 0x00000001, 0x002aaaaa, 0x00000000,
	0x00000014, 0x00000000, 0x0000000f, 0x001b4e81, 0x00e4b17e, 0x001b4e81, 0x00e4b17e, 0x00400000, 0x00400000, 0x00015d86,
	0x0000aec3, 0x00015d86, 0x00333333, 0x00666666, 0x00888889
};

/**
 * Audio ClearVoice III - UEQ Parameter Definition
 *
 */
const	UINT32	gADEC_CVUEQParams[LX_CV_UEQ_PARAM_NUM] =
{
	/*	========  User EQ parameters  ========
		PARAM1, 	PARAM2, 	PARAM3, 	PARAM4, 	PARAM5, 	PARAM6, 	PARAM7, 	PARAM8, 	PARAM9, 	PARAM10,
		PARAM11,	PARAM12,	PARAM13,	PARAM14,	PARAM15,	PARAM16,	PARAM17,	PARAM18,	PARAM19,	PARAM20,
	*/
		/* STANDARD */
		0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000,
		0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000
};

/**
 * Audio ClearVoice III - UEQ2 Parameter Definition
 *
 */
const	UINT32	gADEC_CVUEQ2Params[LX_CV_UEQ_PARAM_NUM] =
{
	/*	========  User EQ2 parameters  ========
		PARAM1, 	PARAM2, 	PARAM3, 	PARAM4, 	PARAM5, 	PARAM6, 	PARAM7, 	PARAM8, 	PARAM9, 	PARAM10,
		PARAM11,	PARAM12,	PARAM13,	PARAM14,	PARAM15,	PARAM16,	PARAM17,	PARAM18,	PARAM19,	PARAM20,
	*/
		/* STANDARD */
		0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000,
		0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000, 0x00080000
};

/**
 * Audio ClearVoice III - Bass EQ Parameter Definition(DEQ for GP3 target)
 */
const	UINT32	gADEC_CVBassEQParams[LX_CV_DEQ_PARAM_NUM]=
{
	/*	========  Bass EQ parameters  ========
	PARAM1, 	PARAM2, 	PARAM3, 	PARAM4, 	PARAM5, 	PARAM6, 	PARAM7, 	PARAM8, 	PARAM9, 	PARAM10,
	...
	PARAM111,	PARAM112,	PARAM113,	PARAM114,	PARAM115,	PARAM116,	PARAM117,	PARAM118,
	*/
	0x00000001, 0x0000001e, 0x00200000, 0x003fb498, 0xffe04ab9, 0x003faf2e, 0xffe05008, 0x003fa9c4, 0xffe05557, 0x003fa458, 	/*	  1 */
	0xffe05aa5, 0x003f9eec, 0xffe05ff2, 0x003f997f, 0xffe0653e, 0x003f9412, 0xffe06a8a, 0x003f815d, 0xffe07d3f, 0x003f6994, 	/*	 11 */
	0xffe09508, 0x003f60d5, 0xffe09dc7, 0x003f5795, 0xffe0a708, 0x00203e0b, 0xffc04b68, 0x001f773b, 0x00203c7f, 0xffc050d2, 	/*	 21 */
	0x001f7378, 0x002034db, 0xffc0563c, 0x001f75cd, 0x00202d1b, 0xffc05ba8, 0x001f783f, 0x00201c0f, 0xffc06114, 0x001f83ff, 	/*	 31 */
	0x00200d1b, 0xffc06681, 0x001f8da6, 0x00200000, 0xffc06bee, 0x001f9576, 0x001ffc7f, 0xffc07ea3, 0x001f8642, 0x001ff7e5, 	/*	41 */
	0xffc0966c, 0x001f7313, 0x001ff37d, 0xffc09f2b, 0x001f6ebc, 0x001feed2, 0xffc0a86b, 0x001f6a26, 0x0020b636, 0xfff47db5, 	/*	51 */
	0x0020b636, 0xfff47db5, 0x001e1171, 0xfff57a2d, 0x001e1171, 0xfff57a2d, 0x001b7451, 0xfff65c79, 0x0018de4a, 0xfff7263a, 	/*	61 */
	0x00000000, 0x00000000, 0x001a76a1, 0xfff6ac5a, 0x001c0427, 0xfff62dbf, 0x001d8692, 0xfff5ab06, 0x001efda9, 0xfff524ca, 	/*	71 */
	0x0039ddf3, 0xffb4da4f, 0x001c13d3, 0x0037310a, 0xffb95bc1, 0x001a3f49, 0x0031b49f, 0xffc5af13, 0x001510b1, 0x002d651d, 	/*	81 */
	0xffccb3f2, 0x00125b53, 0x0027d534, 0xffd87d7c, 0x000ddc86, 0x00238d4e, 0xffe1d39a, 0x000a9a95, 0x00200000, 0x00000000, 	/*	91 */
	0x00000000, 0x001dc26e, 0xffe8df73, 0x00083b23, 0x001ba3ff, 0xffea84fe, 0x0007a51b, 0x0019a40c, 0xffec12d9, 0x00071782, 	/* 101 */
	0x0017c1cf, 0xffed899d, 0x00069221, 0x003f95d8, 0xffe06979, 0x001fcb18, 0xffc069d0, 0x001fcb18							/* 118 */
};

/**
 * Audio ClearVoice III - Infinite Sound Parameter Definition
 */
const	UINT32	gADEC_CVSurroundParams[LX_CV_IFNSND_PARAM_NUM] =
{
	/*	========  Infinite Sound parameters  ========
	PARAM1, 	PARAM2, 	PARAM3, 	PARAM4, 	PARAM5, 	PARAM6, 	PARAM7, 	PARAM8, 	PARAM9, 	PARAM10,
	PARAM11,	PARAM12,	PARAM13,	PARAM14,	PARAM15,	PARAM16,	PARAM17,	PARAM18,	PARAM19,	PARAM20,
	PARAM21,	PARAM22,	PARAM23

	*/
	0x00000005, 0x00066300, 0x00733800, 0x00041100, 0x0077dc00, 0x007a6500, 0x0074cc00, 0x0000ea00, 0x007e2900, 0x00317c00,
	0x00285900, 0x0036dd00, 0x003fff00, 0x00406b00, 0x00400000, 0x00266500, 0x0016da00, 0x00266500, 0x003fff00, 0x0006626b,
	0x00000008, 0x00000010, 0x0000000b
};

/**
 * Audio ClearVoice III - Input Level monitoring Parameters Definition
 */
const	UINT32	gADEC_CVINLEVParams[LX_CV_INLEV_PARAM_NUM] =
{
	/*	========  INLEV parameters	========
	PARAM1, 	PARAM2, 	PARAM3, 	PARAM4, 	PARAM5, 	PARAM6, 	PARAM7, 	PARAM8, 	PARAM9, 	PARAM10,
	*/
	0x003fbee1, 0xffe02ac8, 0x00001564, 0x00000000, 0xffffea9c, 0x003f4979, 0xffe02ac8, 0x00001564, 0x00000000, 0xffffea9c	/*	  1 */
};



/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Function Prototype Declaration
----------------------------------------------------------------------------------------*/



#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _ADEC_SE_PARAM_H_ */

/** @} */
