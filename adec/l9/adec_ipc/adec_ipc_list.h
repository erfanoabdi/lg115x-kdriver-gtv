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

#ifndef _ADEC_IPC_LIST_H_
#define _ADEC_IPC_LIST_H_

#include "base_types.h"


/**
 *	Define of the ADEC_IPC_CMD. Send the command from ARM to DSP
 * 	@see IPC_A2D_COMMAND
 * 	@see IPC_A2D_SETVALUE
 */

//#define USE_IPC_ADEC_CMD

//#define USE_ADEC_RESTRUCT_IPC_CMD
#define IPC_A2D_COMMAND( X )				(0x00010000 + X)	// Arm to DSP
#define IPC_A2D_SETVALUE( X )				(0x00040000 + X)	// Arm to DSP

#define ADEC_IPC_CMD_READY_DECODING			IPC_A2D_COMMAND(0x0201)
#define ADEC_IPC_CMD_START_DECODING			IPC_A2D_COMMAND(0x0202)
#define ADEC_IPC_CMD_STOP_DECODING			IPC_A2D_COMMAND(0x0203)
#define ADEC_IPC_CMD_PAUSE_DECODING			IPC_A2D_COMMAND(0x0204)
#define ADEC_IPC_CMD_RESUME_DECODING		IPC_A2D_COMMAND(0x0205)
#define ADEC_IPC_CMD_FLUSH_DECODING			IPC_A2D_COMMAND(0x0206)


/**
 *	Define of the ADEC_IPC_SET. Send the command from ARM to DSP
 * 	@see IPC_A2D_COMMAND
 * 	@see IPC_A2D_SETVALUE
 */
#define ADEC_IPC_SET_SPDIF_OUT_TYPE			IPC_A2D_SETVALUE(0x0110)

#define ADEC_IPC_SET_AV_LIPSYNC				IPC_A2D_SETVALUE(0x0201)
#define ADEC_IPC_SET_SYNC_BASE				IPC_A2D_SETVALUE(0x0205)

#define ADEC_IPC_SET_TRICK_STATE			IPC_A2D_SETVALUE(0x0301)
#define ADEC_IPC_SET_BALANCE				IPC_A2D_SETVALUE(0x0302)
#define ADEC_IPC_SET_BASS					IPC_A2D_SETVALUE(0x0303)
#define ADEC_IPC_SET_TREBLE					IPC_A2D_SETVALUE(0x0304)
#define ADEC_IPC_SET_VOLUME					IPC_A2D_SETVALUE(0x0305)

#define ADEC_IPC_SET_SE_MODE				IPC_A2D_SETVALUE(0x0401)
#define ADEC_IPC_SET_SE_FN					IPC_A2D_SETVALUE(0x0402)
#define ADEC_IPC_SET_SE_ENABLE				IPC_A2D_SETVALUE(0x0403)

#define ADEC_IPC_SET_SPDIF_SCMS				IPC_A2D_SETVALUE(0x0410)
#define ADEC_IPC_SET_SPDIF_DTO				IPC_A2D_SETVALUE(0x0411)
#define ADEC_IPC_SET_SPDIF_IEC				IPC_A2D_SETVALUE(0x0412)
#define ADEC_IPC_SET_PCMINTRATE				IPC_A2D_SETVALUE(0x0413)


#define ADEC_IPC_CMD_CONNECT				IPC_A2D_COMMAND(0x0501)
#define ADEC_IPC_CMD_DISCONNECT				IPC_A2D_COMMAND(0x0507)
#define ADEC_IPC_CMD_SETCONFIG				IPC_A2D_COMMAND(0x0502)

#define ADEC_IPC_CMD_TEST_1					IPC_A2D_COMMAND(0x0601)
#define ADEC_IPC_CMD_TEST_2					IPC_A2D_COMMAND(0x0602)
#define ADEC_IPC_CMD_TEST_3					IPC_A2D_COMMAND(0x0603)
#define ADEC_IPC_CMD_TEST_4					IPC_A2D_COMMAND(0x0604)
#define ADEC_IPC_CMD_TEST_5					IPC_A2D_COMMAND(0x0605)
#define ADEC_IPC_CMD_TEST_6					IPC_A2D_COMMAND(0x0606)
#define ADEC_IPC_CMD_TEST_7					IPC_A2D_COMMAND(0x0607)
#define ADEC_IPC_CMD_TEST_8					IPC_A2D_COMMAND(0x0608)
#define ADEC_IPC_CMD_TEST_9					IPC_A2D_COMMAND(0x0609)
#define ADEC_IPC_CMD_TEST_10				IPC_A2D_COMMAND(0x060a)
#define ADEC_IPC_CMD_PRINT_MEMMAP			IPC_A2D_COMMAND(0x1000)

#define ADEC_IPC_SET_MIX_EXT_CNF			IPC_A2D_SETVALUE(0x1000)
#define ADEC_IPC_CMD_REAL_ENABLE			IPC_A2D_COMMAND(0x0700)
#define ADEC_IPC_SET_REAL_TH				IPC_A2D_COMMAND(0x1100)
#define ADEC_IPC_SET_REAL_FREQ				IPC_A2D_COMMAND(0x1101)

#define ADEC_IPC_CMD_ASK_ALIVE				IPC_A2D_COMMAND(0xFFFF)

/**
 *	Define of the ADEC_IPC_REQ. Send the command from DSP to ARM
 * 	@see IPC_D2A_REQUEST
 * 	@see IPC_D2A_RESPONSE
 */

#define IPC_D2A_REQUEST( X )				(0x01000000 + X)	// DSP to ARM
#define IPC_D2A_RESPONSE( X )				(0x04000000 + X)	// DSP to ARM

#define ADEC_IPC_REQ_UNDERFLOW				IPC_D2A_REQUEST(0x1000)
#define ADEC_IPC_REQ_PRESENT_END			IPC_D2A_REQUEST(0x1001)
#define ADEC_IPC_REQ_DSP0NOTRSP				IPC_D2A_REQUEST(0x8000)

#define ADEC_IPC_RSP_ALIVE					IPC_D2A_RESPONSE(0xFFFF)
#define ADEC_IPC_RSP_DEC_RESULT				IPC_D2A_RESPONSE(0x1001)



// define Audio Decoder Parameter Number
#define NUM_OF_PARAM_NONE_INFO					0
#define NUM_OF_PARAM_WMAPRO_INFO				8
#define NUM_OF_PARAM_AAC_INFO					6
#define NUM_OF_PARAM_ADPCM_INFO					2
#define NUM_OF_PARAM_AC3_INFO					2
#define NUM_OF_PARAM_RA_INFO					5

// define CV(Clear Voice) Parameter Number
#define KDRV_SE_SET_MODE_NUM		4
#define KDRV_ADEC_SET_DECODE_NUM	10
typedef struct {
	UINT32				mixPort;
	UINT32				waitLength;		// Sample base 1024->23ms
	UINT32				initialMuteLength;	// Sample base 1024->23ms
	UINT32				autoFadeOutEnable;	// 1: enable. 0: disable
	UINT32				autoFadeInEnable;	// 1: enable. 0: disable
	UINT32				fadeLength;		// (0~31) Power of 2. if 9, fade lenght = 512sample (12ms)
	UINT32				initDelay;		// samples. if 48 1ms
}ADEC_MIX_EXT_PARAM_T;


/**
 *	Define of the ADEC_IPC_SET structures
 */
typedef enum {
	KDRV_ADEC_TPI_0,		//0
	KDRV_ADEC_TPI_1,		//1
	KDRV_ADEC_SAI,			//2
	KDRV_ADEC_EMP_0,		//3
	KDRV_ADEC_EMP_1,		//4
	KDRV_ADEC_MIXIN_0,		//5
	KDRV_ADEC_MIXIN_1,		//6
	KDRV_ADEC_MIXIN_2,		//7
	KDRV_ADEC_MIXIN_3,		//8
	KDRV_ADEC_MIXIN_4,		//9
	KDRV_ADEC_MIXIN_5,		//10
	KDRV_ADEC_DEC_0,		//11
	KDRV_ADEC_DEC_1,		//12
	KDRV_ADEC_ENC,			//13
	KDRV_ADEC_SRC_0,		//14
	KDRV_ADEC_SRC_1,		//15
	KDRV_ADEC_SRC_2,		//16
	KDRV_ADEC_MP_00,		//17
	KDRV_ADEC_MP_01,		//18
	KDRV_ADEC_MP_02,		//19
	KDRV_ADEC_MP_03,		//20
	KDRV_ADEC_MP_04,		//21
	KDRV_ADEC_MP_05,		//22
	KDRV_ADEC_MP_06,		//23
	KDRV_ADEC_MP_07,		//24
	KDRV_ADEC_MIXER_0,		//25
	KDRV_ADEC_MIXER_1,		//26
	KDRV_ADEC_SE,			//27
	KDRV_ADEC_SPDIF,		//28
	KDRV_ADEC_SPK,			//29
	KDRV_ADEC_HP,			//30
	KDRV_ADEC_SCART,		//31
	KDRV_ADEC_LINEOUT,		//32
} KDRV_ADEC_MOD_T;

typedef enum {
	KDRV_ADEC_TRICK_NORMAL_SPEED,
	KDRV_ADEC_TRICK_PAUSE,
	KDRV_ADEC_TRICK_FORCED_STOP_ON,
	KDRV_ADEC_TRICK_FAST_FORWARD_1P2X,
	KDRV_ADEC_TRICK_FAST_FORWARD_1P5X,
	KDRV_ADEC_TRICK_FAST_FORWARD_2X,
	KDRV_ADEC_TRICK_SLOW_0P8X,
	KDRV_ADEC_TRICK_SLOW_0P5X
} KDRV_ADEC_TRICK_MODE_T;

/**
 * ADEC Decoder and Mix Mode for Audio Clip Play.
 */
typedef  enum
{
	KDRV_ADEC_DECODE_MAIN		= 0x01,		///< Main Audio Decoding
	KDRV_ADEC_DECODE_ASSO		= 0x02,		///< Associate Audio Decoding

	KDRV_ADEC_MIX_BUF0			= 0x10,		///< PCM Audio Mix in PCM Buffer0
	KDRV_ADEC_MIX_BUF1			= 0x20,		///< PCM Audio Mix in PCM Buffer1
	KDRV_ADEC_MIX_BUF2			= 0x40,		///< PCM Audio Mix in PCM Buffer2
	KDRV_ADEC_MIX_BUF3			= 0x80		///< PCM Audio Mix in PCM Buffer3
} KDRV_ADEC_DEC_MODE_T;

/**
 * ADEC Input Port.
 */
typedef  enum
{
	KDRV_ADEC_INPUT_TP_0		= 0x01,
	KDRV_ADEC_INPUT_TP_1		= 0x02,
	KDRV_ADEC_INPUT_SI_0		= 0x03,
	KDRV_ADEC_INPUT_SIF			= 0x04,
	KDRV_ADEC_INPUT_ADC			= 0x05,
	KDRV_ADEC_INPUT_HDMI		= 0x06,
	KDRV_ADEC_INPUT_I2S			= 0x07,
	KDRV_ADEC_INPUT_SYSTEM_0	= 0x08,
	KDRV_ADEC_INPUT_SYSTEM_1	= 0x09,
	KDRV_ADEC_INPUT_DEC0		= 0x0a,
	KDRV_ADEC_INPUT_DEC1		= 0x0b,
	KDRV_ADEC_INPUT_MIX_OUT		= 0x0c,
} KDRV_ADEC_INPUT_PORT_T;

/**
 * ADEC Output Port.
 */
typedef  enum
{
	KDRV_ADEC_OUTPUT_DPBM		= 0x01,
	KDRV_ADEC_OUTPUT_DPBA		= 0x02,
	KDRV_ADEC_OUTPUT_ENC		= 0x03,
} KDRV_ADEC_OUTPUT_PORT_T;

/**
 * ADEC Codec Type.
 */
typedef  enum
{
	KDRV_ADEC_CODEC_PCM			= 0x01,
	KDRV_ADEC_CODEC_MS10_DDC	= 0x02,
	KDRV_ADEC_CODEC_MS10_DDT	= 0x03,
	KDRV_ADEC_CODEC_MPEG12		= 0x04,
	KDRV_ADEC_CODEC_MP3			= 0x05,
	KDRV_ADEC_CODEC_WMA			= 0x06,
	KDRV_ADEC_CODEC_DTS			= 0x07,
	KDRV_ADEC_CODEC_VORBIS		= 0x08,
	KDRV_ADEC_CODEC_AMR_WB		= 0x09,
	KDRV_ADEC_CODEC_AMR_NB		= 0x0A,
	KDRV_ADEC_CODEC_ADPCM		= 0x0B,
	KDRV_ADEC_CODEC_DDCO		= 0x0C,
	KDRV_ADEC_CODEC_AAC_ENC		= 0x0D,
	KDRV_ADEC_CODEC_RA8		= 0x0E,
} KDRV_ADEC_CODEC_T;

/**
 * ADEC Source Format Type.
 */
typedef  enum
{
	KDRV_ADEC_SRC_TYPE_UNKNOWN	= 0,	///< Unknown
	KDRV_ADEC_SRC_TYPE_PCM		= 1,	///< PCM
	KDRV_ADEC_SRC_TYPE_AC3		= 2,	///< AC3(Dolby Digital)
	KDRV_ADEC_SRC_TYPE_EAC3		= 3,	///< Enhanced AC3(Dolby Digital Plus)
	KDRV_ADEC_SRC_TYPE_MPEG		= 4,	///< MPEG
	KDRV_ADEC_SRC_TYPE_AAC		= 5,	///< AAC
	KDRV_ADEC_SRC_TYPE_HEAAC	= 6,	///< HEAAC
	KDRV_ADEC_SRC_TYPE_DRA		= 7,	///< DRA
	KDRV_ADEC_SRC_TYPE_MP3		= 8,	///< MP3
	KDRV_ADEC_SRC_TYPE_DTS		= 9,	///< DTS
	KDRV_ADEC_SRC_TYPE_SIF		= 10,	///< SIF
	KDRV_ADEC_SRC_TYPE_SIF_BTSC	= 11, 	///< SIF(BTSC)
	KDRV_ADEC_SRC_TYPE_SIF_A2	= 12,	///< SIF(A2)
	KDRV_ADEC_SRC_TYPE_DEFAULT	= 13,	///< DEFAULT
	KDRV_ADEC_SRC_TYPE_NONE		= 14,	///< NONE

	KDRV_ADEC_SRC_TYPE_MS10_DDC	= 15, 	///< MS10_DDC(AC3, EAC3)
	KDRV_ADEC_SRC_TYPE_MS10_DDT	= 16, 	///< MS10_DDT(AAC, HE-AAC)

	KDRV_ADEC_SRC_TYPE_WMA		= 20, 	///< temp
	KDRV_ADEC_SRC_TYPE_WMA_PRO	= 20, 	///< WMA, WMA_PRO
	KDRV_ADEC_SRC_TYPE_ADPCM	= 21, 	///< ADPCM
	KDRV_ADEC_SRC_TYPE_VORBIS	= 22, 	///< VORBIS
	KDRV_ADEC_SRC_TYPE_AMR_NB	= 23, 	///< AMR_NB
	KDRV_ADEC_SRC_TYPE_AMR_WB	= 24, 	///< AMR_NB
	KDRV_ADEC_SRC_TYPE_AAC_LC	= 25, 	///< AAC_LC
	KDRV_ADEC_SRC_TYPE_AAC_LC_ENC	= 26, 	///< AAC_LC_ENC

} KDRV_ADEC_SRC_TYPE_T;

/**
 * ADEC Audio Sample Frequency.
 */
typedef enum
{
	KDRV_ADEC_SAMPLING_FREQ_NONE		= 0,		///< None
	KDRV_ADEC_SAMPLING_FREQ_4_KHZ		= 4,		///< 4 Kbps
	KDRV_ADEC_SAMPLING_FREQ_8_KHZ		= 8,		///< 8 Kbps
	KDRV_ADEC_SAMPLING_FREQ_11_025KHZ	= 11,		///< 11.025 Kbps
	KDRV_ADEC_SAMPLING_FREQ_12_KHZ		= 12,		///< 12 kbps
	KDRV_ADEC_SAMPLING_FREQ_16_KHZ		= 16,		///< 16 Kbps
	KDRV_ADEC_SAMPLING_FREQ_22_05KHZ	= 22,		///< 22.05 Kbps
	KDRV_ADEC_SAMPLING_FREQ_24_KHZ		= 24,		///< 24 Kbps
	KDRV_ADEC_SAMPLING_FREQ_32_KHZ		= 32,		///< 32 Kbps
	KDRV_ADEC_SAMPLING_FREQ_44_1KHZ		= 44,		///< 44.1 Kbps
	KDRV_ADEC_SAMPLING_FREQ_48_KHZ		= 48,		///< 48 Kbps
	KDRV_ADEC_SAMPLING_FREQ_64_KHZ		= 64,		///< 64 Kbps
	KDRV_ADEC_SAMPLING_FREQ_88_2KHZ		= 88,		///< 88.2 Kbps
	KDRV_ADEC_SAMPLING_FREQ_96_KHZ		= 96,		///< 96 Kbps
	KDRV_ADEC_SAMPLING_FREQ_128_KHZ 	= 128,		///< 128 Kbps
	KDRV_ADEC_SAMPLING_FREQ_176_4KHZ	= 176,		///< 176.4 Kbps
	KDRV_ADEC_SAMPLING_FREQ_192_KHZ		= 192,		///< 192 Kbps
	KDRV_ADEC_SAMPLING_FREQ_768_KHZ		= 768,		///< 768 Kbps
	KDRV_ADEC_SAMPLING_FREQ_DEFAULT		= 999		///< default
} KDRV_ADEC_SAMPLING_FREQ_T;

/**
 * ADEC ADPCM CODEC TYPE.
 */
typedef enum
{
	KDRV_ADPCM_CODEC_TYPE_NONE			= 0,	///< Not defined codec
	KDRV_ADPCM_CODEC_TYPE_IMA_QT		= 1,	///< ADPCM IMA_QT codec
	KDRV_ADPCM_CODEC_TYPE_IMA_WAV		= 2,	///< ADPCM IMA_WAV codec
	KDRV_ADPCM_CODEC_TYPE_IMA_DK3		= 3,	///< ADPCM IMA_DK3 codec
	KDRV_ADPCM_CODEC_TYPE_IMA_DK4		= 4,	///< ADPCM IMA_DK4 codec
	KDRV_ADPCM_CODEC_TYPE_IMA_WS		= 5,	///< ADPCM IMA_WS codec
	KDRV_ADPCM_CODEC_TYPE_IMA_SMJPEG	= 6,	///< ADPCM IMA_SMJPEG codec
	KDRV_ADPCM_CODEC_TYPE_MS			= 7,	///< ADPCM MS codec
	KDRV_ADPCM_CODEC_TYPE_4XM			= 8,	///< ADPCM 4XM codec
	KDRV_ADPCM_CODEC_TYPE_XA			= 9,	///< ADPCM XA codec
	KDRV_ADPCM_CODEC_TYPE_ADX			= 10,	///< ADPCM ADX codec
	KDRV_ADPCM_CODEC_TYPE_EA			= 11,	///< ADPCM EA codec
	KDRV_ADPCM_CODEC_TYPE_G726			= 12,	///< ADPCM G726 codec
	KDRV_ADPCM_CODEC_TYPE_CT			= 13,	///< ADPCM CT codec
	KDRV_ADPCM_CODEC_TYPE_SWF			= 14,	///< ADPCM SWF codec
	KDRV_ADPCM_CODEC_TYPE_YAMAHA		= 15,	///< ADPCM YAMAHA codec
	KDRV_ADPCM_CODEC_TYPE_SBPRO_4		= 16,	///< ADPCM SBPRO_4 codec
	KDRV_ADPCM_CODEC_TYPE_SBPRO_3		= 17,	///< ADPCM SBPRO_3 codec
	KDRV_ADPCM_CODEC_TYPE_SBPRO_2		= 18,	///< ADPCM SBPRO_2 codec
	KDRV_ADPCM_CODEC_TYPE_THP			= 19,	///< ADPCM THP codec
	KDRV_ADPCM_CODEC_TYPE_IMA_AMV		= 20,	///< ADPCM IMA_AMV codec
	KDRV_ADPCM_CODEC_TYPE_EA_R1			= 21,	///< ADPCM EA_R1 codec
	KDRV_ADPCM_CODEC_TYPE_EA_R3			= 22,	///< ADPCM EA_R3 codec
	KDRV_ADPCM_CODEC_TYPE_EA_R2			= 23,	///< ADPCM EA_R2 codec
	KDRV_ADPCM_CODEC_TYPE_IMA_EA_SEAD	= 24,	///< ADPCM IMA_EA_SEAD codec
	KDRV_ADPCM_CODEC_TYPE_IMA_EA_EACS	= 25,	///< ADPCM IMA_EA_EACS codec
	KDRV_ADPCM_CODEC_TYPE_EA_XAS		= 26,	///< ADPCM EA_XAS codec
	KDRV_ADPCM_CODEC_TYPE_EA_MAXIS_XA	= 27,	///< ADPCM EA_MAXIS_XA codec
	KDRV_ADPCM_CODEC_TYPE_IMA_ISS		= 28,	///< ADPCM IMA_ISS codec
	KDRV_ADPCM_CODEC_TYPE_G722			= 29,	///< ADPCM G722 codec
} KDRV_ADEC_ADPCM_TYPE_T;

/**
 * ADEC AAC Format Type.
 */
typedef  enum
{
	KDRV_ADEC_AAC_FORMAT_TYPE_RAW		= 0,	///< RAW format
	KDRV_ADEC_AAC_FORMAT_TYPE_ADTS		= 1,	///< ADTS format
	KDRV_ADEC_AAC_FORMAT_TYPE_ADIF		= 2,	///< ADIF format
	KDRV_ADEC_AAC_FORMAT_TYPE_LOAS		= 3,	///< LOAS format
	KDRV_ADEC_AAC_FORMAT_TYPE_LATM		= 4	///< LATM format
} KDRV_ADEC_AAC_FORMAT_TYPE_T;

/**
 * ADEC AC3 Mode Type.
 */
typedef  enum
{
	KDRV_ADEC_DRC_MODE_LINE		= 0,	///< LINE MODE
	KDRV_ADEC_DRC_MODE_RF		= 1,	///< RF MODE
} KDRV_ADEC_DRC_MODE_T;


typedef  enum
{
	KDRV_ADEC_DOWNMIX_MODE_LORO	= 0,	///< Lo/Ro
	KDRV_ADEC_DOWNMIX_MODE_LTRT	= 1,	///< Lt/Rt
} KDRV_ADEC_DOWNMIX_MODE_T;

typedef struct
{
	UINT32		dec_num;
	UINT32		action_type;
	UINT32		result; 			///< IF 0 success, otherwise failure.
}ADEC_RSP_DEC_PARAM_T;



/**
 *	Define of the ADEC_IPC_CMD structures
 */
typedef struct
{
	int 				aliveIdentifier;
}ADEC_CMD_ALIVE_PARAM_T;

typedef struct
{
	UINT32				ui32ReadyData;
}ADEC_CMD_READY_DECODING_T;

typedef struct
{
	UINT32				module;
}ADEC_CMD_START_DECODING_T;

typedef struct
{
	UINT32				module;
}ADEC_CMD_STOP_DECODING_T;

typedef struct
{
	UINT32				module;
}ADEC_CMD_PAUSE_DECODING_T;

typedef struct
{
	UINT32				module;
}ADEC_CMD_RESUME_DECODING_T;

typedef struct
{
	UINT32				module;
}ADEC_CMD_FLUSH_DECODING_T;

typedef struct
{
	UINT32				module;
	UINT32				connector;
}ADEC_CMD_CONNECT_MODULE_T;

typedef struct
{
	UINT32				module;
}ADEC_CMD_DISCONNECT_MODULE_T;

typedef struct
{
	UINT32				module;
	UINT32				param[30];
}ADEC_CMD_SETCONFIG_MODULE_T;

typedef struct
{
	UINT32				module;
	UINT32				param[6];				///< Lipsync Param KDRV_ADEC_LIPSYNC_PARAM_T
}ADEC_CMD_SETLIPSYNC_MODULE_T;

typedef struct {
	UINT32				adecInputPort;				///< ADEC decoder KDRV_ADEC_INPUT_PORT_T
	UINT32				adecCodecType;				///< Audio Source Type KDRV_ADEC_CODEC_T
	UINT32				bitRate; 					///< Bit Rate
	UINT32				samplingFreq;				///< Sampling Frequency KDRV_ADEC_SAMPLING_FREQ_T
	UINT32				numberOfChannel;			///< The number of channel of codec or PCM data
	UINT32				pcmBitsPerSample;			///< The bit per sample of PCM data
	UINT32				ui32Param[KDRV_ADEC_SET_DECODE_NUM];	///< The decoder parameters of adec type
} KDRV_ADEC_DEC_PARAM_T;

typedef struct {
	UINT32				numOfParam;					///< number of ui32Param
	UINT32				avgBytesPerSec; 			///< The average bytes per seconds for WMA codec
	UINT32				blockAlign; 				///< The block alignment for WMA codec
	UINT32				encoderOption;				///< The encoder option for WMA codec
	UINT32				wmaFormatTag;				///< The format tag for WMA codec
	UINT32				channelMask; 				///< The channel mask for WMA codec
	UINT32				advEncoderOption; 			///< The advanced encoder option for WMA codec
	UINT32				advEncoderOption2Hi;		///< The advanced encoder option 2 for WMA codec
	UINT32				advEncoderOption2Lo;		///< The advanced encoder option 2 for WMA codec
} KDRV_ADEC_WMAPRO_INFO_T;

typedef struct {
	UINT32				numOfParam;					///< number of ui32Param
	UINT32				aacFormat;					///< AAC Format Type KDRV_ADEC_AAC_FORMAT_TYPE_T
	UINT32				drcMode; 					///< drc mode KDRV_ADEC_DRC_MODE_T
	UINT32				downmixMode;				///< dwonmix mode KDRV_ADEC_DOWNMIX_MODE_T
	UINT32				encoderOption0; 			///< The Upper AAC Encoder Option of 8bytes
	UINT32				encoderOption1; 			///< The lower AAC Encoder Option of 8bytes
	UINT32				sizeOfEncOpt; 			///< The size of Encoder Option
} KDRV_ADEC_AAC_INFO_T;

typedef struct {
	UINT32				numOfParam;					///< number of ui32Param
	UINT32				adpcmType; 					///< ADPCM type KDRV_ADEC_ADPCM_TYPE_T
	UINT32				blockAlign; 				///< The block alignment for ADPCM codec
} KDRV_ADEC_ADPCM_INFO_T;

typedef struct {
	UINT32				numOfParam;					///< number of ui32Param
	UINT32				drcMode; 					///< drc mode KDRV_ADEC_DRC_MODE_T
	UINT32				downmixMode;				///< dwonmix mode KDRV_ADEC_DOWNMIX_MODE_T
} KDRV_ADEC_AC3_INFO_T;

typedef struct {
	UINT32				numOfParam;					///< number of ui32Param
	UINT32				samplesPerFrame;				///< number of samples per frame
	UINT32				freqResponse;					///< determines frequency response
	UINT32				leafSize; 					///< leaf size
	UINT32				cplStartRegion;					///< related to joint stereo encoding
	UINT32				cplQuantBits;					///< related to joint stereo encoding
} KDRV_ADEC_RA_INFO_T;

typedef struct {
	UINT32 outmode;		// output mode LR, LL, RR, L+R
	UINT32 mute;		// mute control ON, OFF
	UINT32 volctrl;		// volume control enable, bypass
	UINT32 volume;
	UINT32 delayMS;		// delay time unit ms
	SINT32 balance; 	// left -50 ~ 50 Right
} KDRV_ADEC_SPK_PARAM_T;

typedef struct {
	UINT32 type;		// spdif output type PCM, bitstream
	UINT32 outmode;		// output mode LR, LL, RR, L+R
	UINT32 mute;		// mute control ON, OFF
	UINT32 volctrl;		// volume control enable, bypass
	UINT32 volume;
	UINT32 delayMS;		// delay time unit ms
} KDRV_ADEC_SPDIF_PARAM_T;

typedef struct {
	UINT32 outmode;		// output mode LR, LL, RR, L+R
	UINT32 mute;		// mute control ON, OFF
	UINT32 volctrl;		// volume control enable , bypass
	UINT32 volume;
	UINT32 delayMS;		// delay time unit ms
} KDRV_ADEC_HP_PARAM_T, KDRV_ADEC_SCART_PARAM_T, KDRV_ADEC_LINEOUT_PARAM_T, KDRV_ADEC_WOOFER_PARAM_T;

typedef struct {
	UINT32 bOnOff;		// lipsync ON, OFF
	UINT32 ubound;		// upper bound
	UINT32 lbound;		// lower bound
	UINT32 offset;
	UINT32 freerunubound;	// freerun upper bound
	UINT32 freerunlbound;	// freerun lower bound
} KDRV_ADEC_LIPSYNC_PARAM_T;

typedef struct {
	UINT32 module;			// KDRV_ADEC_MOD_T DEC0, DEC1
	UINT64 clockBase;
	UINT64 streamBase;
} ADEC_CMD_SYNC_BASE_T;

typedef struct
{
	UINT32				ui32SpdifMode;
}ADEC_SET_SPDIF_OUT_TYPE_T;

typedef struct
{
	UINT32				ui32TrickState;
}ADEC_SET_TRICK_STATE_T;

typedef struct
{
	UINT32				ui32Bass;
}ADEC_SET_BASS_T;

typedef struct
{
	UINT32				ui32Treble;
}ADEC_SET_TREBLE_T;

/**
 * Output control in line with user's volume
 */
typedef struct
{
	UINT32				module;
	UINT32				ui32Volume;
}ADEC_SET_VOLUME_T;

typedef struct {
	UINT32 ui32Scms;		// copy free : 0, copy once 2, copy no more : 1, copy never : 3
} ADEC_SET_SPDIF_SCMS_T;

typedef struct {
	UINT32 ui32SampleFreq;		// KDRV_ADEC_SAMPLING_FREQ_T
	UINT32 ui32DtoRate;		// DTO rate :  0 UNKNOWN, 1 NORMAL, 2 fast1 , 3 fast2, 4 fast3, 5 slow1, 6 slow2 7 slow3
    UINT32 ui32ForceCntClear;          //
    UINT32 ui32isHDMI;					// HDMI : 1, NOT HDMI : 0
	UINT32 ui32HDMIFs;					// HDMI Input Frequency
} ADEC_SET_SPDIF_DTO_T;

typedef struct {
	UINT32 ui32SetIecSrc;		// KDRV_ADEC_MOD_T DEC0, DEC1
	UINT32 ui32LipsyncOnOff;	// lipsync ON, OFF
} ADEC_SET_SPDIF_IEC_T;


typedef struct {
           UINT32 ui32InFs;                      // 
           UINT32 ui32PCMclkRate;               // PCM intterupt rate :  0 UNKNOWN, 1 NORMAL, 2 fast1 , 3 fast2, 4 fast3, 5 slow1, 6 slow2 7 slow3
           UINT32 ui32ForceCntClear;          //
} ADEC_SET_PCM_INTERRUPT_T;


/**
 *	Define of the ADEC_IPC_REQ structures
 */
typedef struct {
	int		module;
	int		free;
	int		used;
	int		max;
}ADEC_REQ_PARAM_UNDERFLOW_T;

typedef struct {
	int		module;
}ADEC_REQ_PARAM_PRESENT_NOTI_T;

typedef struct {
	int		zero;
}ADEC_REQ_PARAM_DSP0NOTRSP_T;

typedef struct {
	UINT32			ui32PortNum;
	UINT32			ui32NumOfCH;
	UINT32			ui32BitPerSample;
	UINT32			ui32SampleRate;
	UINT32			ui32GainL; // 0 ~ 1024;
	UINT32          ui32GainR; // 0 ~ 1024;
} KDRV_ADEC_MIX_START_PARAM_T;

typedef struct {
	UINT32			ui32FnMode;			///< SE FN mode LX_ADEC_SE_FN_MODE_T
	UINT32			ui32DataOption;		///< SE Parameters Options LX_ADEC_SE_DATA_MODE_T
	UINT32			ui32VarOption;		///< SE Variable Options LX_ADEC_SE_VAL_MODE_T
	UINT32			ui32AccessMode;		///< SE Parameters Access Modes LX_ADEC_SE_DATA_ACCESS_T 
	UINT32			ui32NoParam;		///< The number of Parameters
} KDRV_ADEC_SE_FN_PARAM_T;

typedef struct {
	UINT32			ui32Param[KDRV_SE_SET_MODE_NUM];
} KDRV_ADEC_SE_SET_MODE_T;

typedef struct {
	UINT32			seEnable;
}KDRV_ADEC_SE_ENABLE_T;

typedef struct {
	KDRV_ADEC_SE_FN_PARAM_T seFnParam;
	KDRV_ADEC_SE_SET_MODE_T seSetMode;
} KDRV_ADEC_SE_PARAM_T;

typedef struct {
	UINT32				adecInputMode;			///< AENC input mode KDRV_ADEC_DEC_MODE_T KDRV_ADEC_DECODE_MAIN, KDRV_ADEC_DECODE_ASSO
	UINT32 				aencOutMode;			///< AENC output mode LR, LL, RR, L+R
	UINT32				aencCodecType;			///< AENC Source Type KDRV_ADEC_CODEC_T
	UINT32				channelNumber;
	UINT32				bitPerSample;
	UINT32				samplingFrequency;
	UINT32				bitRate;
} KDRV_ADEC_ENC_PARAM_T;


#endif
