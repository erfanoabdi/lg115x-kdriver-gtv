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

/** @file adec_aac_bitstream.h
 *
 *  Define the structure and macros to help processing bitstream.
 *
 *  @author     Kyungsoon Jung(kyngsoon.jung@lge.com)
 *  @version    0.6
 *  @date       2011.04.25
 *  @note       Bitstream helper
 *
 *  @addtogroup lg1150_adec
 *	@{
 */



#ifndef _ADEC_AAC_BITSTREAM_H_
#define _ADEC_AAC_BITSTREAM_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/



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

typedef struct _ADEC_AAC_BITSTREAM_t
{
	unsigned char* byteBuffer;
	unsigned int bitBuffer;
	int bitRemain;
	unsigned int bitRead;
	unsigned int byteUsed;
	int byteRemain;
}ADEC_AAC_BITSTREAM_T;

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////////
// Caution
///////////////////////////////////////////////////////////////////////////////////
// Use carefully bellow macroes.
// For the efficiency, this macroes have minimized error tolerence.

// Init bitstream
// X: input buffer pointer
// Y: buffer length
// Z: bitstream structure
#define BITSTREAM_INIT(X,Y,Z) \
	{\
		Z->byteBuffer = X;\
		Z->byteRemain = Y;\
		Z->byteUsed = 0;\
		Z->bitRemain = 0;\
		Z->bitBuffer = 0;\
		Z->bitRead = 0;\
	}


// Read and flush bits
// X : output. unsigned int type.
// T : type of the outparam. 
// Y : bits to read.
// Z : bitstream structure
#define BITSTREAM_READ_BITS(X,Y,Z) \
	{\
		while(Y>Z->bitRemain)\
		{\
			if(Z->byteRemain <= 0)\
			{\
				X = 0;\
				break;\
			}\
			Z->bitBuffer = (Z->bitBuffer << 8) | *(Z->byteBuffer);\
			Z->byteBuffer++;\
			Z->byteRemain--;\
			Z->byteUsed++;\
			Z->bitRemain+=8;\
		}\
		X = ((~(0xFFFFFFFF << Z->bitRemain)) &Z->bitBuffer) >> (Z->bitRemain - Y);\
		Z->bitRemain -= Y;\
		Z->bitRead += Y;\
	}

#define BITSTREAM_READ_1BIT(X,Z) \
	{\
		if(Z->bitRemain == 0)\
		{\
			if(Z->byteRemain <= 0)\
			{\
				X = 0;\
			}\
			Z->bitBuffer = (Z->bitBuffer << 8) | *(Z->byteBuffer);\
			Z->byteBuffer++;\
			Z->byteRemain--;\
			Z->byteUsed++;\
			Z->bitRemain+=8;\
		}\
		X = (Z->bitBuffer >> (Z->bitRemain - 1)) & 0x01;\
		Z->bitRemain --;\
		Z->bitRead ++;\
	}

// Skip bits
// Y : bits to skip.
// Z : bitstream structure.
#define BITSTREAM_SKIP_BITS(Y,Z) \
	{\
		while( Y > Z->bitRemain )\
		{\
			if(Z->byteRemain <= 0)\
			{\
				break;\
			}\
			Z->bitBuffer = (Z->bitBuffer << 8) | *(Z->byteBuffer);\
			Z->byteBuffer++;\
			Z->byteRemain--;\
			Z->byteUsed++;\
			Z->bitRemain+=8;\
		}\
		Z->bitRemain -= Y;\
		Z->bitRead += Y;\
	}

#define BITSTREAM_SKIP_1BIT(Z) \
	{\
		if( Z->bitRemain == 0)\
		{\
			if(Z->byteRemain <= 0)\
			{\
				break;\
			}\
			Z->bitBuffer = (Z->bitBuffer << 8) | *(Z->byteBuffer);\
			Z->byteBuffer++;\
			Z->byteRemain--;\
			Z->byteUsed++;\
			Z->bitRemain+=8;\
		}\
		Z->bitRemain --;\
		Z->bitRead ++;\
	}


// Check bits, never flush bitstream.
// X : output.
// T : type of the outparam.
// Y : bits to read
// Z : bitstream structure.
#define BITSTREAM_CHECK_BITS(X,Y,Z) \
	{\
		while( Y>Z->bitRemain )\
		{\
			if( Z->byteRemain <= 0 )\
			{\
				X=0;\
				break;\
			}\
			Z->bitBuffer = (Z->bitBuffer << 8) | *(Z->byteBuffer);\
			Z->byteBuffer++;\
			Z->byteRemain--;\
			Z->byteUsed++;\
			Z->bitRemain+=8;\
		}\
		X = ((~(0xFFFFFFFF << Z->bitRemain)) &Z->bitBuffer) >> (Z->bitRemain - Y);\
	}

// Get the length of the available bits to read.
// This macro does not need a explicit output variable.
// This macro retrieves the result directly.
// Z : bitstream structure.
#define BITSTREAM_BITS_TO_DECODE(Z) ((Z->byteRemain << 3)+Z->bitRemain)

#define BITSTREAM_BYTE_ALIGN(Z)\
	{\
		Z->bitRemain &= 0xFFFFFFF8;\
	}

#define READ_16BIT_BIG_ENDIAN(X) ((((unsigned char*)X)[0] << 8) | ((unsigned char*)X)[1])

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */


#endif /* _ADEC_AAC_BITSTREAM_H_ */

