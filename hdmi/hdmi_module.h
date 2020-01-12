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

/*! \file HDMI_module.h
 * \brief VPORT ADC module header
 */

#ifndef LX_HDMI_MODULE
#define LX_HDMI_MODULE

#include <linux/interrupt.h>

 /*
 	define
 */
#define HDMI_VIDEO_INIT_STATE						0	///<  Video ISR Initialize or No SDCT State.
#define HDMI_VIDEO_INTERRUPT_STATE					1	///<  Video ISR Assert State.
#define HDMI_VIDEO_STABLE_STATE					    2	///<  Video ISR Stable State.

#define HDMI_AVI_INIT_STATE							0	///<  AVI ISR Initialize or No SDCT State.
#define HDMI_AVI_INTERRUPT_STATE					1	///<  AVI ISR Assert State.
#define HDMI_AVI_STABLE_STATE					    3	///<  AVI ISR Stable State.

#define HDMI_VSI_INIT_STATE							0	///<  VSI ISR Initialize or No SDCT State.
#define HDMI_VSI_INTERRUPT_STATE					1	///<  VSI ISR Assert State.
#define HDMI_VSI_STABLE_STATE					    3	///<  VSI ISR Stable State.

#define HDMI_AVI_CSC_CHANGE_STATE					0	///<  AVI CSC Change State.
#define HDMI_AVI_CSC_TEMP_STATE						2	///<  AVI CSC Temp State.
#define HDMI_AVI_CSC_STABLE_STATE					3	///<  AVI CSC Stable State.

#define HDMI_VIDEO_READ_STATE					    4	///<  Video Read Stable State.

#define HDMI_SOURCE_MUTE_CLEAR_STATE				0	///<  Source Clear State.
#define HDMI_SOURCE_MUTE_STATE						1	///<  Source Mute State.

#define HDMI_AUDIO_INIT_STATE						0	///<  Audio ISR Initialize or No SCDT State.
#define HDMI_AUDIO_INTERRUPT_STATE					1	///<  Audio ISR Assert State.
#define HDMI_AUDIO_GET_INFO_STATE					4	///<  Audio ISR and Get Info. State.(count value)
#define HDMI_AUDIO_STABLE_STATE					    5	///<  Audio ISR and sampling frequency Stable State.(count value)
#define HDMI_AUDIO_RECHECK_TIME			   		   10	///<  500 mili-second, DDI calls a every 50 ms..
#define HDMI_AUDIO_FREQ_ERROR_TIME				   10	///<  500 mili-second, DDI calls a every 50 ms.
#define HDMI_AUDIO_PORT_STABLE_COUNT	   		   50	///<  2.5 seconds, DDI calls a every 50 ms..

#define DEBUG_HDMI_AUDIO_MSG_PRINT_TIME			  500	///<  1 second, DDI calls a every 20 ms..
//#define DEBUG_HDMI_AUDIO_MSG_PRINT_TIME		 	 5000	///<  10 seconds, DDI calls a every 20 ms..

// define HDMI Audio Sample Word PCM & non-PCM sample
#define HDMI_AUDIO_SAMPLE_PCM_MASK				 0x02	///< 0 : linear PCM, 1 : other purpose

// define HDMI Audio Sample Frequency according to IEC60958 channel status BYTE3(IEC60958-3 Third edition, 2006-05 spec.)
#define HDMI_AUDIO_SAMPLE_BIT_MASK				 0x0F	///< Bit 0 ~ 3
#define HDMI_AUDIO_SAMPLE_22_05KHZ				    4	///< 22.05 Kbps
#define HDMI_AUDIO_SAMPLE_24_KHZ					6	///< 24 Kbps
#define HDMI_AUDIO_SAMPLE_32_KHZ					3	///< 32 Kbps
#define HDMI_AUDIO_SAMPLE_44_1KHZ					0	///< 44.1 Kbps
#define HDMI_AUDIO_SAMPLE_48_KHZ					2	///< 48 Kbps
#define HDMI_AUDIO_SAMPLE_88_2KHZ					8	///< 88.2 Kbps
#define HDMI_AUDIO_SAMPLE_96_KHZ				   10	///< 96 Kbps
#define HDMI_AUDIO_SAMPLE_176_4KHZ				   12	///< 176.4 Kbps
#define HDMI_AUDIO_SAMPLE_192_KHZ				   14	///< 192 Kbps
#define HDMI_AUDIO_SAMPLE_768_KHZ					9	///< 768 Kbps


// define HDMI Audio Data-Types according to IEC61937-2 Burst Info Preamble C(Pc)(IEC61937-2 First edition, 2004-03 spec.)
#define  BURST_INFO_AUDIO_TYPE_BIT_MASK		   	 0x1F	///< Bit 4 - 0
#define  BURST_INFO_AUDIO_TYPE_NULL					0	///<Null Data
#define  BURST_INFO_AUDIO_TYPE_AC3					1	///<AC-3 data
#define  BURST_INFO_AUDIO_TYPE_SMPTE_338M			2	///<Refer to SMPTE 338M
#define  BURST_INFO_AUDIO_TYPE_PAUSE				3	///<Pause
#define  BURST_INFO_AUDIO_TYPE_MPEG1_L1				4	///<MPEG-1 layer-1 data
#define  BURST_INFO_AUDIO_TYPE_MPEG1_L23			5	///<MPEG-1 layer-2 or -3 data or MPEG-2 without extension
#define  BURST_INFO_AUDIO_TYPE_MPEG2_EXT			6	///<MPEG-2 data with extension
#define  BURST_INFO_AUDIO_TYPE_MPEG2_AAC			7	///<MPEG-2 AAC
#define  BURST_INFO_AUDIO_TYPE_MPEG2_L1				8	///<MPEG-2, layer-1 low sampling frequency
#define  BURST_INFO_AUDIO_TYPE_MPEG2_L2				9	///<MPEG-2, layer-2 low sampling frequency
#define  BURST_INFO_AUDIO_TYPE_MPEG2_L3			   10	///<MPEG-2, layer-3 low sampling frequency
#define  BURST_INFO_AUDIO_TYPE_DTS_I			   11	///<DTS type I
#define  BURST_INFO_AUDIO_TYPE_DTS_II			   12	///<DTS type II
#define  BURST_INFO_AUDIO_TYPE_DTS_III			   13	///<DTS type III
#define  BURST_INFO_AUDIO_TYPE_ATRAC			   14	///<ATRAC
#define  BURST_INFO_AUDIO_TYPE_ATRAC_23			   15	///<ATRAC 2/3
#define  BURST_INFO_AUDIO_TYPE_ATRAC_X			   16	///<ATRAC-X
#define  BURST_INFO_AUDIO_TYPE_DTS_IV			   17	///<DTS type IV
#define  BURST_INFO_AUDIO_TYPE_WMA_I_IV			   18	///<WMA type I ~ IV
#define  BURST_INFO_AUDIO_TYPE_MPEG2_AAC_LOW	   19	///<MPEG-2 AAC low sampling frequency
#define  BURST_INFO_AUDIO_TYPE_MPEG4_AAC	       20	///<MPEG-4 AAC 
#define  BURST_INFO_AUDIO_TYPE_AC3_ENHANCED		   21	///<AC-3 Enhanced
#define  BURST_INFO_AUDIO_TYPE_MAT				   22	///<MAT
														///<23-26 Reserved
														///<27-30 Refer to SMPTE 338M
														///<31 Extended data-type

#define  BURST_INFO_PAYLOAD_ERROR_BIT_MASK	   	 0x80	///<Error-flag indicationg that the burst-payload may contain errors
#define  BURST_INFO_DEPENDENT_INFO_BIT_MASK	   0x1F00	///<Data-type-dependent info.
#define  BURST_INFO_STREAM_NUMBER_BIT_MASK	   0xD000	///<Bitstream number

#define HDMI_DEEP_COLOR_8BIT						0	///<  Deep Color 8bit
#define HDMI_DEEP_COLOR_10BIT						1	///<  Deep Color 8bit
#define HDMI_DEEP_COLOR_12BIT						2	///<  Deep Color 8bit

/*
	Device inform
*/


/*
	structure define
*/
typedef struct
{
	/* for HDMI Switch */
	BOOLEAN			hdmi_switch;		// 0: None Switch, 1: Switch
	UINT8			SWport;
	LX_HDMI_HPD_T 	hpd;

	/* Global Type */
	UINT8			port;
	BOOLEAN			mode;
	UINT8			pixelRepet;			// Pixel Repetition
	UINT8			connectCn[2];		// [0] None Switch, [1] Switch
	UINT8			timingCn[2];		// [0] None Switch, [1] Switch
	UINT8			hdmiCn[2];			// [0] None Switch, [1] Switch
	UINT8			dviCn[2];			// [0] None Switch, [1] Switch
} HDMI_DATA_T;



/*
	Enum define
*/
typedef enum{
	HDMI_PHY0_PORT		= 0,
	HDMI_PHY1_PORT		= 1,
	HDMI_ALL_PHY_PORT	= 2,
} HDMI_PHY_TYPE_T;

/*
	global variable
*/


/*
	function prototype
*/



// from hdmi_hw.c
// function pointer
extern int (*HDMI_GetMode)(LX_HDMI_MODE_T *mode);
extern int (*HDMI_GetAspectRatio)(LX_HDMI_ASPECTRATIO_T *ratio);
extern int (*HDMI_GetTimingInfo)(LX_HDMI_TIMING_INFO_T *info);
extern int (*HDMI_GetStatus)(LX_HDMI_STATUS_T *status);
extern int (*HDMI_GetVsiPacket)(LX_HDMI_VSI_PACKET_T *packet);
extern int (*HDMI_GetAviPacket)(LX_HDMI_AVI_PACKET_T *packet);
extern int (*HDMI_GetSpdPacket)(LX_HDMI_SPD_PACKET_T *packet);
extern int (*HDMI_SetPort)(LX_HDMI_HPD_T *hpd);
extern int (*HDMI_SetPowerControl)(HDMI_PHY_TYPE_T type , int power);
extern int (*HDMI_SetHPDEnDisable)(LX_HDMI_HPD_T *hpd);
extern int (*HDMI_SetAudio)(void);
extern int (*HDMI_GetAudioInfo)(LX_HDMI_AUDIO_INFO_T *audio_info);
extern int (*HDMI_SetMute)(LX_HDMI_MUTE_CTRL_T *mute);
extern int (*HDMI_SetRGBYCbCr)(LX_HDMI_AVI_COLORSPACE_T *space);
extern int (*HDMI_SetArc)(LX_HDMI_ARC_CTRL_T *arc);
extern int (*HDMI_GetRegister)(UINT32 addr , UINT32 *value);
extern int (*HDMI_SetRegister)(UINT32 addr , UINT32 value);
extern void (*HDMI_SetPixelRepetition)(void);
extern void (*HDMI_SetAutoSyncPolarity)(void);
extern int (*HDMI_RunReset)(void);
extern int (*HDMI_HWInitial)(void);
extern irqreturn_t (*HDMI_IRQHandler)(int irq, void *dev);
extern void (*HDMI_EnableInterrupt)(void);
extern int (*HDMI_GetPortConnection)(void);
extern int (*HDMI_PowerConsumption)(LX_HDMI_POWER_CTRL_T *power);
extern int (*HDMI_SetUDn3DControl)(LX_HDMI_UD_3D_MODE_T *udn3d);

#ifdef	KDRV_CONFIG_PM
extern int (*HDMI_RunSuspend)(void);
extern int (*HDMI_RunResume)(void);
#endif

// L8Ax function prototype
int HDMI_L8Ax_GetMode(LX_HDMI_MODE_T *mode);
int HDMI_L8Ax_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio);
int HDMI_L8Ax_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info);
int HDMI_L8Ax_GetStatus(LX_HDMI_STATUS_T *status);
int HDMI_L8Ax_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet);
int HDMI_L8Ax_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet);
int HDMI_L8Ax_GetSpdPacket(LX_HDMI_SPD_PACKET_T *packet);
int HDMI_L8Ax_SetPort(LX_HDMI_HPD_T *hpd);
int HDMI_L8Ax_SetPowerControl(HDMI_PHY_TYPE_T type , int power);
int HDMI_L8Ax_SetHPDEnDisable(LX_HDMI_HPD_T *hpd);
int HDMI_L8Ax_SetAudio(void);
int HDMI_L8Ax_GetAudioInfo(LX_HDMI_AUDIO_INFO_T *audio_info);
int HDMI_L8Ax_SetMute(LX_HDMI_MUTE_CTRL_T *mute);
int HDMI_L8Ax_SetArc(LX_HDMI_ARC_CTRL_T *arc);
int HDMI_L8Ax_SetRGBYCbCr(LX_HDMI_AVI_COLORSPACE_T *space);
int HDMI_L8Ax_GetRegister(UINT32 addr , UINT32 *value);
int HDMI_L8Ax_SetRegister(UINT32 addr , UINT32 value);
void HDMI_L8Ax_SetPixelRepetition(void);
void HDMI_L8Ax_SetAutoSyncPolarity(void);
int HDMI_L8Ax_RunReset(void);
int HDMI_L8Ax_HWInitial(void);
irqreturn_t HDMI_L8Ax_IRQHandler(int irq, void *dev);
void HDMI_L8Ax_EnableInterrupt(void);
int HDMI_L8Ax_GetPortConnection(void);
int HDMI_L8Ax_PowerConsumption(LX_HDMI_POWER_CTRL_T *power);

#ifdef	KDRV_CONFIG_PM
int HDMI_L8Ax_RunSuspend(void);
int HDMI_L8Ax_RunResume(void);
#endif

// L8Bx function prototype
int HDMI_L8Bx_GetMode(LX_HDMI_MODE_T *mode);
int HDMI_L8Bx_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio);
int HDMI_L8Bx_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info);
int HDMI_L8Bx_GetStatus(LX_HDMI_STATUS_T *status);
int HDMI_L8Bx_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet);
int HDMI_L8Bx_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet);
int HDMI_L8Bx_GetSpdPacket(LX_HDMI_SPD_PACKET_T *packet);
int HDMI_L8Bx_SetPort(LX_HDMI_HPD_T *hpd);
int HDMI_L8Bx_SetPowerControl(HDMI_PHY_TYPE_T type , int power);
int HDMI_L8Bx_SetHPDEnDisable(LX_HDMI_HPD_T *hpd);
int HDMI_L8Bx_SetAudio(void);
int HDMI_L8Bx_GetAudioInfo(LX_HDMI_AUDIO_INFO_T *audio_info);
int HDMI_L8Bx_SetMute(LX_HDMI_MUTE_CTRL_T *mute);
int HDMI_L8Bx_SetArc(LX_HDMI_ARC_CTRL_T *arc);
int HDMI_L8Bx_SetRGBYCbCr(LX_HDMI_AVI_COLORSPACE_T *space);
int HDMI_L8Bx_GetRegister(UINT32 addr , UINT32 *value);
int HDMI_L8Bx_SetRegister(UINT32 addr , UINT32 value);
void HDMI_L8Bx_SetPixelRepetition(void);
void HDMI_L8Bx_SetAutoSyncPolarity(void);
int HDMI_L8Bx_RunReset(void);
int HDMI_L8Bx_HWInitial(void);
irqreturn_t HDMI_L8Bx_IRQHandler(int irq, void *dev);
void HDMI_L8Bx_EnableInterrupt(void);
int HDMI_L8Bx_GetPortConnection(void);
int HDMI_L8Bx_PowerConsumption(LX_HDMI_POWER_CTRL_T *power);

#ifdef	KDRV_CONFIG_PM
int HDMI_L8Bx_RunSuspend(void);
int HDMI_L8Bx_RunResume(void);
#endif

// L9Ax function prototype
int HDMI_L9Ax_GetMode(LX_HDMI_MODE_T *mode);
int HDMI_L9Ax_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio);
int HDMI_L9Ax_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info);
int HDMI_L9Ax_GetStatus(LX_HDMI_STATUS_T *status);
int HDMI_L9Ax_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet);
int HDMI_L9Ax_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet);
int HDMI_L9Ax_GetSpdPacket(LX_HDMI_SPD_PACKET_T *packet);
int HDMI_L9Ax_SetPort(LX_HDMI_HPD_T *hpd);
int HDMI_L9Ax_SetPowerControl(HDMI_PHY_TYPE_T type , int power);
int HDMI_L9Ax_SetHPDEnDisable(LX_HDMI_HPD_T *hpd);
int HDMI_L9Ax_SetAudio(void);
int HDMI_L9Ax_GetAudioInfo(LX_HDMI_AUDIO_INFO_T *audio_info);
int HDMI_L9Ax_SetMute(LX_HDMI_MUTE_CTRL_T *mute);
int HDMI_L9Ax_SetArc(LX_HDMI_ARC_CTRL_T *arc);
int HDMI_L9Ax_SetRGBYCbCr(LX_HDMI_AVI_COLORSPACE_T *space);
int HDMI_L9Ax_GetRegister(UINT32 addr , UINT32 *value);
int HDMI_L9Ax_SetRegister(UINT32 addr , UINT32 value);
void HDMI_L9Ax_SetPixelRepetition(void);
void HDMI_L9Ax_SetAutoSyncPolarity(void);
int HDMI_L9Ax_RunReset(void);
int HDMI_L9Ax_HWInitial(void);
irqreturn_t HDMI_L9Ax_IRQHandler(int irq, void *dev);
void HDMI_L9Ax_EnableInterrupt(void);
int HDMI_L9Ax_GetPortConnection(void);
int HDMI_L9Ax_PowerConsumption(LX_HDMI_POWER_CTRL_T *power);
int HDMI_L9Ax_SetUDn3DControl(LX_HDMI_UD_3D_MODE_T *udn3d);

#ifdef	KDRV_CONFIG_PM
int HDMI_L9Ax_RunSuspend(void);
int HDMI_L9Ax_RunResume(void);
#endif

// L9Bx function prototype
int HDMI_L9Bx_GetMode(LX_HDMI_MODE_T *mode);
int HDMI_L9Bx_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio);
int HDMI_L9Bx_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info);
int HDMI_L9Bx_GetStatus(LX_HDMI_STATUS_T *status);
int HDMI_L9Bx_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet);
int HDMI_L9Bx_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet);
int HDMI_L9Bx_GetSpdPacket(LX_HDMI_SPD_PACKET_T *packet);
int HDMI_L9Bx_SetPort(LX_HDMI_HPD_T *hpd);
int HDMI_L9Bx_SetPowerControl(HDMI_PHY_TYPE_T type , int power);
int HDMI_L9Bx_SetHPDEnDisable(LX_HDMI_HPD_T *hpd);
int HDMI_L9Bx_SetAudio(void);
int HDMI_L9Bx_GetAudioInfo(LX_HDMI_AUDIO_INFO_T *audio_info);
int HDMI_L9Bx_SetMute(LX_HDMI_MUTE_CTRL_T *mute);
int HDMI_L9Bx_SetArc(LX_HDMI_ARC_CTRL_T *arc);
int HDMI_L9Bx_SetRGBYCbCr(LX_HDMI_AVI_COLORSPACE_T *space);
int HDMI_L9Bx_GetRegister(UINT32 addr , UINT32 *value);
int HDMI_L9Bx_SetRegister(UINT32 addr , UINT32 value);
void HDMI_L9Bx_SetPixelRepetition(void);
void HDMI_L9Bx_SetAutoSyncPolarity(void);
int HDMI_L9Bx_RunReset(void);
int HDMI_L9Bx_HWInitial(void);
irqreturn_t HDMI_L9Bx_IRQHandler(int irq, void *dev);
void HDMI_L9Bx_EnableInterrupt(void);
int HDMI_L9Bx_GetPortConnection(void);
int HDMI_L9Bx_PowerConsumption(LX_HDMI_POWER_CTRL_T *power);
int HDMI_L9Bx_SetUDn3DControl(LX_HDMI_UD_3D_MODE_T *udn3d);

#ifdef	KDRV_CONFIG_PM
int HDMI_L9Bx_RunSuspend(void);
int HDMI_L9Bx_RunResume(void);
#endif

// from hdmi_module.c
int HDMI_Initialize(LX_HDMI_INIT_T *param);
int HDMI_exit(void);

int HDMI_GetI2CData(unsigned int sub , unsigned int addr , unsigned int size );
int HDMI_SetI2CData(unsigned int sub , unsigned int addr , unsigned int size , unsigned int value);

#endif
