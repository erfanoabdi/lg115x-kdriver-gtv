
/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/** @file hdmi_kapi.h
 *
 *  application interface header for hdmi device
 *
 *  @author		sh.myoung (sh.myoung@lge.com)
 *  @version		0.9
 *  @date		2010.02.21
 *
 *  @addtogroup lg1150_hdmi
 *	@{
 */

#ifndef	_HDMI_KAPI_H_
#define	_HDMI_KAPI_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	HDMI_IOC_MAGIC		'a'
/**
@name HDMI IOCTL List
ioctl list for hdmi device.
@{

@def HDMI_IO_INIT
Initialize HDMI module.
Set init Rx HDCP production key.

@def HDMI_IOR_GET_MODE
Get HDMI/DVI mode Information.
Checks whether HDMI format or DVI mode in HDMI port

@def HDMI_IOR_GET_ASPECTRATIO
Get the aspect ratio of the HDMI.
This information is generated from the 2 bits aspect ratio information(M1,M0)
of the HDMI AVI(Auxiliary Video Information) InfoFrame packet.

@def HDMI_IOR_GET_TIMING_INFO
Get the Timing Info of HDMI.

@def HDMI_IOR_GET_HDMI_STATUS
Get the information of HDMI.
Those are HDMI mode, HDCP, colordonmain, colorimetry and hotplug information.

@def HDMI_IOR_GET_HDMI_VSI
Get HDMI VSI(Vendor Specific Information) inforFrame data.

@def HDMI_IOR_GET_HDMI_AVI
Get HDMI AVI(Auxiliary Video Information) inforFrame data.

@def HDMI_IOW_SET_HPD
Control HDMI HOT Plug Detect.
Enable hotplug GPIO/PIN output and set output value

@def HDMI_IOW_SET_PORT
Set the Input port of HDMI.

@def HDMI_IOR_GET_INFO_FOR_AUDIO
Gets a HDMI audio info related to AUDIO module.
This info. has a No audio, PCM or AC3 info to DDI function from HDMI port input	\n
And a interlaced info. in current video.

@def HDMI_IOW_SET_MUTE_CONTROL
Set HDMI Video and Audio Mute.

@def HDMI_IOR_GET_HDMI_SPD
Get HDMI SPD(Source Product Description) InfoFrame data.

@def HDMI_IOW_SET_ARC
Set HDMI ARC(Audio Return Channel) Control.

@def HDMI_IOW_POWER_CONTROL
This ioctl control power on/off of HDMI block to reduce power consumption.

@def HDMI_IOW_SET_3D_N_UD
Set HDMI Data Control for support UD and 3D(SS Full, Frame Packing 1080p@60).

@def HDMI_IOR_PHY_REG_RD
Read HDMI PHY Register Value.

@def HDMI_IOW_PHY_REG_WR
Write HDMI PHY Register Value.

@def HDMI_IOW_HDCP_ENABLE
Set HDCP Enable : 1, disable : 0.

@def HDMI_IOW_SET_HDCP
Set HDCP key info after reading HDCP key value from Tx eeprom.
*/


/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
#define HDMI_IOW_INIT						_IOW(HDMI_IOC_MAGIC,  0, LX_HDMI_INIT_T)
#define HDMI_IOR_GET_MODE					_IOR(HDMI_IOC_MAGIC,  1, LX_HDMI_MODE_T )
#define HDMI_IOR_GET_ASPECTRATIO			_IOR(HDMI_IOC_MAGIC,  2, LX_HDMI_ASPECTRATIO_T)
#define HDMI_IOR_GET_TIMING_INFO			_IOR(HDMI_IOC_MAGIC,  3, LX_HDMI_TIMING_INFO_T)
#define HDMI_IOR_GET_HDMI_STATUS			_IOR(HDMI_IOC_MAGIC,  4, LX_HDMI_STATUS_T)
#define HDMI_IOR_GET_HDMI_VSI				_IOR(HDMI_IOC_MAGIC,  5, LX_HDMI_VSI_PACKET_T)
#define HDMI_IOR_GET_HDMI_AVI				_IOR(HDMI_IOC_MAGIC,  6, LX_HDMI_AVI_PACKET_T)
#define HDMI_IOW_SET_HPD					_IOW(HDMI_IOC_MAGIC,  7, LX_HDMI_HPD_T)
#define HDMI_IOW_SET_PORT					_IOW(HDMI_IOC_MAGIC,  8, LX_HDMI_HPD_T)
#define HDMI_IOR_GET_INFO_FOR_AUDIO			_IOR(HDMI_IOC_MAGIC,  9, LX_HDMI_AUDIO_INFO_T)
#define HDMI_IOW_SET_MUTE_CONTROL			_IOW(HDMI_IOC_MAGIC,  10, LX_HDMI_MUTE_CTRL_T)
#define HDMI_IOW_SET_RGB_YCBCR				_IOW(HDMI_IOC_MAGIC,  11, LX_HDMI_AVI_COLORSPACE_T)
#define HDMI_IOR_GET_HDMI_SPD				_IOR(HDMI_IOC_MAGIC,  12, LX_HDMI_SPD_PACKET_T)
#define HDMI_IOW_SET_ARC					_IOW(HDMI_IOC_MAGIC,  13, LX_HDMI_ARC_CTRL_T)
#define HDMI_IOW_POWER_CONTROL				_IOW(HDMI_IOC_MAGIC,  14, LX_HDMI_POWER_CTRL_T)
#define HDMI_IOW_SET_UD_3D_MODE				_IOW(HDMI_IOC_MAGIC,  15, LX_HDMI_UD_3D_MODE_T)

//For HDMI HDCP debug
#define HDMI_IOR_REG_RD						_IOR(HDMI_IOC_MAGIC,  20, UINT32)
#define HDMI_IOW_REG_WR						_IOW(HDMI_IOC_MAGIC,  21, LX_HDMI_PHY_REG_WRITE_T)
#define HDMI_IOW_HDCP_ENABLE				_IOW(HDMI_IOC_MAGIC,  22, BOOLEAN)
#define HDMI_IOW_SET_HDCP					_IOW(HDMI_IOC_MAGIC,  23, LX_HDMI_HDCP_T )
#define	HDMI_IOW_INTERFACE					_IOW(HDMI_IOC_MAGIC,  24, S_INTERFACE_REG)

#define HDMI_IOC_MAXNR				25


/** @} */
#define LX_HDMI_PACKET_DATA_LENGTH	28							///< LX_HDMI_PACKET_DATA_LENGTH
#define LX_HDMI_VSI_PACKET_DATA_LENGTH 28						///< HDMI VSI Packet Data length
#define LX_HDMI_VSI_INFOFRAME_PACKET_LEN 28						///< HDMI VSI Inforframe packet length
#define LX_HDMI_VSI_VENDOR_SPECIFIC_REGID_LEN 3					///< HDMI VSI Vendor specific Regid length
#define LX_HDMI_VSI_VENDOR_SPECIFIC_PAYLOAD_LEN \
	(LX_HDMI_VSI_INFOFRAME_PACKET_LEN - LX_HDMI_VSI_VENDOR_SPECIFIC_REGID_LEN)		///< HDMI VSI Vendor specific payload length
#define LX_HDCP_KEY_SIZE	289			///< HDCP key size
#define LX_HDMI_SPD_VENDOR_NAME_LEN  	8					///< Vendor Name Data Length of SPD
#define LX_HDMI_SPD_PRODUCT_DESC_LEN    16					///< Product Description Data Length of SPD


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
 * HDMI source window index.
 */
typedef enum
{
	LX_HDMI_MAIN_WINDOW = 0,		///< main window
	LX_HDMI_SUB_WINDOW				///< sub window
} LX_HDMI_WINDOW_IDX_T;

/**
 * HDMI VSI Packet Status.
 * VSI Packet Status TYPE is defined.
 */
typedef enum
{
	LX_HDMI_VSI_PACKET_STATUS_NOT_RECEIVED = 0,		///< initial state
	LX_HDMI_VSI_PACKET_STATUS_STOPPED,				///< Stoped
	LX_HDMI_VSI_PACKET_STATUS_UPDATED,				///< Updated
	LX_HDMI_VSI_PACKET_STATUS_MAX					///< Max
} LX_HDMI_PACKET_STATUS_T;

/**
 * HDMI AVI Color Space.
 * AVI Color Space TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_COLORSPACE_RGB = 0,			///< RGB
	LX_HDMI_AVI_COLORSPACE_YCBCR422,		///< YCbCr422
	LX_HDMI_AVI_COLORSPACE_YCBCR444,		///< YCbCr444
	LX_HDMI_AVI_COLORSPACE_FUTURE			///< Future
} LX_HDMI_AVI_COLORSPACE_T;

/**
 * HDMI AVI ACTIVE FORMAT INFO.
 * TVI ACTIVE FORMAT DATA is defined.
 *
 */
typedef enum
{
	LX_HDMI_AVI_ACTIVE_INFO_NO_DATA = 0,   	///< ACTIVE FORMAT No data
	LX_HDMI_AVI_ACTIVE_INFO_VALID			///< Valid data
} LX_HDMI_AVI_ACTIVE_FORMAT_INFO_T;

/**
 * HDMI AVI BAR INFO.
 * AVI BAR DATA is defined.
 */
typedef enum
{
	LX_HDMI_AVI_BAR_INFO_INVALID,					///< Invalid
	LX_HDMI_AVI_BAR_INFO_VERTICALVALID,				///< V Valid
	LX_HDMI_AVI_BAR_INFO_HORIZVALID,				///< H Valid
	LX_HDMI_AVI_BAR_INFO_VERTHORIZVALID				///< V and H Valid
} LX_HDMI_AVI_BAR_INFO_T;

/**
 * HDMI AVI SCAN INFO.
 * AVI SCAN TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_SCAN_INFO_NO_DATA,				///< Scan info no data
	LX_HDMI_AVI_SCAN_INFO_OVER_SCANNED,			///< over scan
	LX_HDMI_AVI_SCAN_INFO_UNDER_SACNNED,		///< under scan
	LX_HDMI_AVI_SCAN_INFO_FUTURE				///< Future
} LX_HDMI_AVI_SCAN_INFO_T;

/**
 * HDMI AVI COLORIMETRY.
 * AVI COLORIMETRY TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_COLORIMETRY_NO_DATA =  0,		///< COLORIMETRY No data
	LX_HDMI_AVI_COLORIMETRY_ITU601, 			///< ITU601(SMPTE170)
	LX_HDMI_AVI_COLORIMETRY_ITU709,				///< ITU709
	LX_HDMI_AVI_COLORIMETRY_EXTEND				///< Future
} LX_HDMI_AVI_COLORIMETRY_T;

/**
 * HDMI AVI PICTURE ARC.
 * AVI PICTURE ARC TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_PICTURE_ARC_NO_DATA = 0,			///< Picture ARC No data
	LX_HDMI_AVI_PICTURE_ARC_4_3,					///< ARC_4_3
	LX_HDMI_AVI_PICTURE_ARC_16_9,					///< ARC_16_9
	LX_HDMI_AVI_PICTURE_ARC_FUTURE					///< Future
} LX_HDMI_AVI_PICTURE_ARC_T;

/**
 * HDMI AVI ACTIVE FORMAT ARC.
 * AVI ACTIVE FORMAT ARC TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_ACTIVE_FORMAT_ARC_SAME  =  8,		///< Active ARC smae
	LX_HDMI_AVI_ACTIVE_FORMAT_ARC_4_3   =  9,		///< Active ARC_4_3
	LX_HDMI_AVI_ACTIVE_FORMAT_ARC_16_9  = 10,		///< Active ARC_16_9
	LX_HDMI_AVI_ACTIVE_FORMAT_ARC_14_9  = 11,		///< Active ARC_14_9
	LX_HDMI_AVI_ACTIVE_FORMAT_ARC_OTHER =  0		///< Active ARC other
} LX_HDMI_AVI_ACTIVE_FORMAT_ARC_T;

/**
 * HDMI AVI IT CONTENT.
 * AVI IT CONTENT DATA is defined.
 */
typedef enum
{
	LX_HDMI_AVI_IT_CONTENT_NO_DATA     =  0,	///< IT Content NO data
	LX_HDMI_AVI_IT_CONTENT_ITCONTENT   =  1		///< IT Content
} LX_HDMI_AVI_IT_CONTENT_T;

/**
 * HDMI AVI EXT COLORIMETRY.
 * AVI EXT COLORIMETRY TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_EXT_COLORIMETRY_XVYCC601 = 0,		///< xvYCC601
	LX_HDMI_AVI_EXT_COLORIMETRY_XVYCC709,			///< xvYCC709
	LX_HDMI_AVI_EXT_COLORIMETRY_RESERVED			///< Reserved
}	LX_HDMI_AVI_EXT_COLORIMETRY_T;

/**
 * HDMI AVI RGB QUANTIZATION RANGE.
 * AVI RGB QUANTIZATION RANGE TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_RGB_QUANTIZATION_RANGE_DEFAULT    =  0,		///< QuanRange default
	LX_HDMI_AVI_RGB_QUANTIZATION_RANGE_LIMITED,				///< QuanRange limit 15~235(220)
	LX_HDMI_AVI_RGB_QUANTIZATION_RANGE_FULL,				///< QuanRange full  0 ~255(256)
	LX_HDMI_AVI_RGB_QUANTIZATION_RANGE_RESERVED				///< QuanRange RESERVED
} LX_HDMI_AVI_RGB_QUANTIZATION_RANGE_T;

/**
 * HDMI AVI SCALING.
 * AVI SCALING TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_SCALING_NOKNOWN = 0,		///< Scaling unknown
	LX_HDMI_AVI_SCALING_HORIZ,				///< Scaling Horizontal
	LX_HDMI_AVI_SCALING_VERT,				///< Scaling Vertical
	LX_HDMI_AVI_SCALING_HORIZVERT			///< Scaling Horizontal and Vertical
} LX_HDMI_AVI_SCALING_T;

/**
 * HDMI AVI YCC QUANTIZATION RANGE.
 * AVI QUANTIZATION RANGE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_YCC_QUANTIZATION_RANGE_LIMITED = 0,		///< YCC Limited Range
	LX_HDMI_AVI_YCC_QUANTIZATION_RANGE_FULL,			///< YCC Full Range
	LX_HDMI_AVI_YCC_QUANTIZATION_RANGE_RESERVED			///< YCC RESERVED
} LX_HDMI_AVI_YCC_QUANTIZATION_RANGE_T;

/**
 * HDMI AVI CONTENT TYPE.
 * AVI CONTENT TYPE is defined.
 */
typedef enum
{
	LX_HDMI_AVI_CONTENT_GRAPHICS = 0,		///< CONTENT GRAPHICS
	LX_HDMI_AVI_CONTENT_PHOTO,				///< CONTENT PHOTO
	LX_HDMI_AVI_CONTENT_CINEMA,				///< CONTENT CINEMA
	LX_HDMI_AVI_CONTENT_GAME				///< CONTENT GAME
} LX_HDMI_AVI_CONTENT_T;

/**
 * HDMI Audio Type.
 * HDMI Audio type is defined.
 */
typedef  enum
{
	LX_HDMI_AUDIO_DVI			= 0,		///< DVI Type
	LX_HDMI_AUDIO_NO_AUDIO		= 1,		///< No Audio Type
	LX_HDMI_AUDIO_PCM			= 2,		///< PCM Type
	LX_HDMI_AUDIO_AC3			= 3,		///< AC3 Type
	LX_HDMI_AUDIO_DTS			= 4,		///< DTS Type
	LX_HDMI_AUDIO_AAC			= 5,		///< AAC Type
	LX_HDMI_AUDIO_DEFAULT		= 6,		///< DEFAULT Type

	LX_HDMI_AUDIO_MPEG			= 10 		///< MPEG Type
} LX_HDMI_AUDIO_TYPE_T;

/**
 * HDMI Audio Sample Frequency.
 * Sample frequency values merged from HDMI Audio InfoFrame and Audio Channel Status.
 * Also, this enumeration describes the sampling rate for ADEC driver.
 * If this value is changed, LX_ADEC_SAMPLING_FREQ_T is also changed in ADEC kernel driver.
 */
typedef enum
{
	LX_HDMI_SAMPLING_FREQ_NONE		= 0,		///< None
	LX_HDMI_SAMPLING_FREQ_4_KHZ		= 4,		///< 4 Kbps
	LX_HDMI_SAMPLING_FREQ_8_KHZ		= 8,		///< 8 Kbps
	LX_HDMI_SAMPLING_FREQ_11_025KHZ	= 11,		///< 11.025 Kbps
	LX_HDMI_SAMPLING_FREQ_12_KHZ	= 12,		///< 12 kbps
	LX_HDMI_SAMPLING_FREQ_16_KHZ	= 16,		///< 16 Kbps
	LX_HDMI_SAMPLING_FREQ_22_05KHZ	= 22,		///< 22.05 Kbps
	LX_HDMI_SAMPLING_FREQ_24_KHZ	= 24,		///< 24 Kbps
	LX_HDMI_SAMPLING_FREQ_32_KHZ	= 32,		///< 32 Kbps
	LX_HDMI_SAMPLING_FREQ_44_1KHZ	= 44,		///< 44.1 Kbps
	LX_HDMI_SAMPLING_FREQ_48_KHZ	= 48,		///< 48 Kbps
	LX_HDMI_SAMPLING_FREQ_64_KHZ	= 64,		///< 64 Kbps
	LX_HDMI_SAMPLING_FREQ_88_2KHZ	= 88,		///< 88.2 Kbps
	LX_HDMI_SAMPLING_FREQ_96_KHZ	= 96,		///< 96 Kbps
	LX_HDMI_SAMPLING_FREQ_128_KHZ 	= 128,		///< 128 Kbps
	LX_HDMI_SAMPLING_FREQ_176_4KHZ	= 176,		///< 176.4 Kbps
	LX_HDMI_SAMPLING_FREQ_192_KHZ	= 192,		///< 192 Kbps
	LX_HDMI_SAMPLING_FREQ_768_KHZ	= 768,		///< 768 Kbps
	LX_HDMI_SAMPLING_FREQ_DEFAULT	= 999		///< default
} LX_HDMI_SAMPLING_FREQ_T;


/**
 * HDMI Color Depth Type.
 * HDMI Color Depth is defined.
 */
typedef  enum
{
	LX_HDMI_COLOR_DEPTH_8 = 0,		///< Color depth 24 bits
	LX_HDMI_COLOR_DEPTH_10,			///< Color depth 30 bits
	LX_HDMI_COLOR_DEPTH_12			///< Color depth 36 bits
} LX_HDMI_COLOR_DEPTH_T;

/**
 * Video Format of HDMI Vendor Specific Infoframe.
 */
typedef enum
{
	LX_FORMAT_NO_ADDITIONAL_FORMAT, 			///< No additional HDMI video format
	LX_FORMAT_EXTENDED_RESOLUTION_FORMAT,		///< Extended resolution format(4K x 2K)
	LX_FORMAT_3D_FORMAT,						///< 3D format
}LX_HDMI_VSI_VIDEO_FORMAT_T;

/**
 * 3D Structure field of HDMI Vendor Specific Infoframe.
 */
typedef enum
{
	LX_HDMI_VSI_3D_STRUCTURE_FRAME_PACKING,				///< Frame packing
	LX_HDMI_VSI_3D_STRUCTURE_FIELD_ALTERNATIVE,			///< Field alternative
	LX_HDMI_VSI_3D_STRUCTURE_LINE_ALTERNATIVE,			///< Line alternative
	LX_HDMI_VSI_3D_STRUCTURE_SIDEBYSIDE_FULL,			/// Side-by-Side (Full)

	LX_HDMI_VSI_3D_STRUCTURE_L_DEPTH,					///< L + Depth
	LX_HDMI_VSI_3D_STRUCTURE_L_DEPTH_GRAPHICS,			///< L + Depth + Graphics

	LX_HDMI_VSI_3D_STRUCTURE_TOP_BOTTOM,				///< Top and Bottim
	LX_HDMI_VSI_3D_STRUCTURE_SIDEBYSIDE_HALF = 0x08,	///< Side-by-Side (half)
}LX_HDMI_VSI_3D_STRUCTURE_T;

/**
 * 3D Ext Data of HDMI Vendor Specific Infoframe.
 */
typedef enum
{
	LX_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_0,		///< Horizontal Sub-sampling - Odd/left picture, Odd/right picture
	LX_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_1,		///< Horizontal Sub-sampling - Odd/left picture, Even/right picture
	LX_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_2,		///< Horizontal Sub-sampling - Even/left picture, Odd/right picture
	LX_HDMI_VSI_3D_EXT_DATA_HOR_SUB_SAMPL_3,		///< Horizontal Sub-sampling - Even/left picture, Even/right picture

	LX_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_0,		///< Quincunx Matrix - Odd/left picture, Odd/right picture
	LX_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_1,		///< Quincunx Matrix - Odd/left picture, Even/right picture
	LX_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_2,		///< Quincunx Matrix - Even/left picture, Odd/right picture
	LX_HDMI_VSI_3D_EXT_DATA_QUINCUNX_MATRIX_3,		///< Quincunx Matrix - Even/left picture, Even/right picture

}LX_HDMI_VSI_3D_EXT_DATA_T;

/**
 * HDMI Video Format Identification Code for extended resolution transmission.
 *
*/
typedef enum
{
	LX_HDMI_VSI_VIC_RESERVED,			///< Reserved
	LX_HDMI_VSI_VIC_4K2K_30HZ,			///< 4K x 2K @ 30Hz
	LX_HDMI_VSI_VIC_4K2K_25HZ,			///< 4K x 2K @ 25Hz
	LX_HDMI_VSI_VIC_4K2K_24HZ,			///< 4K x 2K @ 24Hz
	LX_HDMI_VSI_VIC_4K2K_24HZ_SMPTE,	///< 4K x 2K @ 24Hz (SMPTE)

}LX_HDMI_VSI_VIC_T;

/**
 * HDMI Data Dridge Control Parameter for UD and specific 3D(SS Full & Frame Packing) format operating.
 *
*/
typedef enum
{
	LX_HDMI_UD_3D_OFF,			///< OFF - 2D (No Data Dridge)
	LX_HDMI_UD,					///< UD Format
	LX_HDMI_3D_SS_FULL,			///< 3D Side by Side Full
	LX_HDMI_3D_FP_60HZ,			///< 3D Frame Packing 1080p@60Hz
	LX_HDMI_UD_SCALING,			///< UD Fromat Scaling for home dash mode
}LX_HDMI_UD_3D_MODE_T;


/**
 * 3D Video Full Timing Information.
 *
*/
typedef enum
{
	LX_HDMI_NORMAL_TIMING = 0,		///< 2D format
	LX_HDMI_3D_FRAMEPACK,			///< 3D frame packing format
	LX_HDMI_3D_SBSFULL,				///< 3D Side by Side Full format
	LX_HDMI_3D_FIELD_ALTERNATIVE,	///< 3D field alternative format
	LX_HDMI_3D_LINE_ALTERNATIVE, 	///< 3D line alternative format(row interleaving)
} LX_HDMI_3D_FULL_TIMING_INFO_T;

///////////////////////////////////////////////////////////////////////////////////////
/**
 * hdmi ioctl parameter.
 * HDMI Init parameter.
 * Set HDMI Chip and Switch information.
 * @see HDMI_IOW_INIT
 */
typedef struct
{
	BOOLEAN	bHdmiSW;					///< HDMI SW 0: None, 1: Switch
	UINT8	u8HWport;					///< HDMI H/W Port for HDMI Switch
	//UINT8	u8ChipRev;					///< Chip Name and Ver.
} LX_HDMI_INIT_T;

/**
 * hdmi ioctl parameter.
 * HDMI mode parameter.
 * Read HDMI / DVI mode.
 * @see HDMI_IOR_GET_MODE
 */
typedef struct
{
	LX_HDMI_WINDOW_IDX_T srcIdx;		///< HDMI Window
	BOOLEAN bHDMI;   					///< 0 : DVI mode, 1 : HDMI Mode
	UINT8	u8port;						///< HDMI port
} LX_HDMI_MODE_T;

/**
 * hdmi ioctl parameter.
 * HDMI ASPECTRATIO parameter.
 * Get HDMI ASPECTRATIO.
 * @see HDMI_IOR_GET_ASPECTRATIO
 *
 */
typedef struct
{
	LX_HDMI_WINDOW_IDX_T srcIdx;				///< HDMI Window
	LX_HDMI_AVI_PICTURE_ARC_T eAspectRatio;		///< LX_HDMI_AVI_PICTURE_ARC_T
} LX_HDMI_ASPECTRATIO_T;

/**
 * hdmi ioctl parameter.
 * HDMI TIMING INFO Parameter.
 * Get HDMI TIMING INFO.
 * @see HDMI_IOR_GET_TIMING_INFO
 */
typedef struct {
	LX_HDMI_WINDOW_IDX_T srcIdx;	///< window index
	UINT16 hFreq; 					///< Horizontal frequency(100 Hz unit)
	UINT16 vFreq; 					///< Veritical frequency(1/10 Hz unit)
	UINT16 hTotal; 					///< Horizontal total pixels
	UINT16 vTotal; 					///< Vertical total lines
	UINT16 hStart; 					///< Horizontal start pixel
	UINT16 vStart;					///< Vertical start lines
	UINT16 hActive;					///< Horizontal active pixel
	UINT16 vActive; 				///< Vertical active lines
	UINT16 scanType; 				///< Scan type (0 : interlace, 1 : progressive)
	//UINT16 HdmiLock;				///< Sampling phase
	LX_HDMI_3D_FULL_TIMING_INFO_T full_3d_timing;		///< 3D Video Full Timing Information
} LX_HDMI_TIMING_INFO_T;

/**
 * hdmi ioctl parameter.
 * HDMI STATUS.
 * Get HDMI STATUS.
 * @see HDMI_IOR_GET_HDMI_STATUS
 */
typedef struct
{
	LX_HDMI_WINDOW_IDX_T srcIdx;			///< HDMI Window
	BOOLEAN bHdmiMode;						///< 0:DVI   1:HDMI
	BOOLEAN eHotPlug;						///< 0:Enabled  1:disabled
	UINT8 u8hdmiport;						///< 0~3: port0~port3
	LX_HDMI_COLOR_DEPTH_T eColorDepth;		///< 0:8bit  1:10bit  2:12bit
	UINT8 pixelRepet;						///< PixelRepetition
	LX_HDMI_AVI_COLORSPACE_T csc;		    ///< PixelEncoding
	//BOOLEAN eHDCP;
	//BOOLEAN validHdmiStatus;
} LX_HDMI_STATUS_T;

/**
 * hdmi ioctl parameter.
 * HDMI Packet parameter.
 * @see HDMI_VSI_T, HDMI_AVI_T
 */
typedef struct
{
	UINT8 type;            ///< packet type
	UINT8 version;         ///< packet version
	UINT8 length;          ///< packet length
	UINT8 dataBytes[LX_HDMI_PACKET_DATA_LENGTH]; ///< packet data
} LX_HDMI_PACKET_T;

/**
 * hdmi ioctl parameter.
 * HDMI VSI Info.
 * Get VSI Info.
 */
typedef struct
{
	UINT8 IEERegId[LX_HDMI_VSI_VENDOR_SPECIFIC_REGID_LEN];   	///< 24 bit IEEE Registration Identifier[3]
	UINT8 PayLoad[LX_HDMI_VSI_VENDOR_SPECIFIC_PAYLOAD_LEN]; 	///< Payload  [25]

	LX_HDMI_PACKET_T packet;                               		///< Packet statue
	LX_HDMI_PACKET_STATUS_T packetStatus;						///< Raw HDMI inforFrame packet data

	LX_HDMI_VSI_VIDEO_FORMAT_T eVideoFormat;	/**< HDMI VSI info */
	LX_HDMI_VSI_3D_STRUCTURE_T e3DStructure;	/**< HDMI VSI info */
	LX_HDMI_VSI_3D_EXT_DATA_T e3DExtData;		/**< HDMI VSI info */
	LX_HDMI_VSI_VIC_T eVSiVIC;					/**< HDMI VSI info */
} LX_HDMI_VSI_T;

/**
 * hdmi ioctl parameter.
 * HDMI VSI PACKET Info.
 * Get VSI PACKET Info.
 * @see HDMI_IOR_GET_HDMI_VSI
 */
typedef struct
{
	LX_HDMI_WINDOW_IDX_T srcIdx;		///< HDMI Window
	LX_HDMI_VSI_T eVsiPacket;			///< HDMI VSI INFO
} LX_HDMI_VSI_PACKET_T;

/**
 * hdmi ioctl parameter.
 * HDMI AVI Info.
 * Get AVI Info.
 */
typedef struct
{
	LX_HDMI_AVI_COLORSPACE_T ePixelEncoding; 						///< LX_HDMI_AVI_COLORSPACE_T

	LX_HDMI_AVI_ACTIVE_FORMAT_INFO_T eActiveInfo; 					///< LX_HDMI_AVI_ACTIVE_FORMAT_INFO_T
	LX_HDMI_AVI_BAR_INFO_T    eBarInfo; 			 				///< LX_HDMI_AVI_BAR_INFO_T
	LX_HDMI_AVI_SCAN_INFO_T   eScanInfo;  		 					///< LX_HDMI_AVI_SCAN_INFO_T

	LX_HDMI_AVI_COLORIMETRY_T eColorimetry;							///< LX_HDMI_AVI_COLORIMETRY_T
	LX_HDMI_AVI_PICTURE_ARC_T ePictureAspectRatio; 					///< LX_HDMI_AVI_PICTURE_ARC_T
	LX_HDMI_AVI_ACTIVE_FORMAT_ARC_T eActiveFormatAspectRatio; 		///< LX_HDMI_AVI_ACTIVE_FORMAT_ARC_T

	LX_HDMI_AVI_IT_CONTENT_T eITContent;							///< LX_HDMI_AVI_IT_CONTENT_T
	LX_HDMI_AVI_EXT_COLORIMETRY_T eExtendedColorimetry;				///< LX_HDMI_AVI_EXT_COLORIMETRY_T
	LX_HDMI_AVI_RGB_QUANTIZATION_RANGE_T eRGBQuantizationRange;		///< LX_HDMI_AVI_RGB_QUANTIZATION_RANGE_T
	LX_HDMI_AVI_SCALING_T eScaling;									///< LX_HDMI_AVI_SCALING_T

	UINT8 VideoIdCode; 												///< VideoIdCode

	LX_HDMI_AVI_YCC_QUANTIZATION_RANGE_T	eYCCQuantizationRange;	///< LX_HDMI_AVI_YCC_QUANTIZATION_RANGE_T
	LX_HDMI_AVI_CONTENT_T	eContentType;							///< LX_HDMI_AVI_CONTENT_T
	UINT8 PixelRepeat;												///< PixelRepeat

	/* bar info */
	UINT16 TopBarEndLineNumber;										///< Top Bar End LineNumber
	UINT16 BottomBarStartLineNumber;      							///< Bottom Bar Start LineNumber
	UINT16 LeftBarEndPixelNumber;									///< Left Bar End Pixel Number
	UINT16 RightBarEndPixelNumber;        							///< Right Bar End Pixel Number

	LX_HDMI_PACKET_STATUS_T packetStatus;							///< LX_HDMI_PACKET_STATUS_T
	LX_HDMI_PACKET_T packet;										///< LX_HDMI_PACKET_T

	BOOLEAN bHdmiMode;												///< 0:DVI   1:HDMI
} LX_HDMI_AVI_T;

/**
 * hdmi ioctl parameter.
 * HDMI AVI PACKET Info.
 * Get AVI PACKET Info.
 * @see HDMI_IOR_GET_HDMI_AVI
 */
typedef struct
{
	LX_HDMI_WINDOW_IDX_T srcIdx;		///< HDMI Window
	LX_HDMI_AVI_T eAviPacket;			///< HDMI AVI INFO
} LX_HDMI_AVI_PACKET_T;

/**
 * hdmi ioctl parameter.
 * HDMI HOTPLUG CONTROL.
 * Set HDMI HOTPLUG.
 * @see HDMI_IOW_SET_HPD, HDMI_IOW_SET_PORT
 */
typedef struct
{
  LX_HDMI_WINDOW_IDX_T srcIdx;		///< HDMI Window
  UINT8 u8HDMIPort;					///< HDMI Port
  BOOLEAN bEnable;					///< Hotplug Enable
} LX_HDMI_HPD_T;

/**
 * hdmi ioctl parameter.
 * HDMI POWER CONTROL.
 * Set HDMI MUTE CONTROL.
 * @see HDMI_IOW_SET_MUTE_CONTROL
 */
typedef struct
{
  LX_HDMI_WINDOW_IDX_T srcIdx;		///< HDMI Window
  BOOLEAN bVideoMute;				///< Video Mute(1), Clear(0)
  BOOLEAN bAudioMute;				///< Audio Mute(1), Clear(0)
  UINT8	 	muteCtrl;				///< Mute Control -> 0 : disable, 1 : enable, 2 : internal enable
} LX_HDMI_MUTE_CTRL_T;

/**
 * hdmi ioctl parameter.
 * HDMI Auido and Video Info.
 * Get HDMI AUDIO AND VIDEO INFORMATION.
 * @see HDMI_IOR_GET_INFO_FOR_AUDIO
 */
typedef struct
{
  LX_HDMI_WINDOW_IDX_T 			srcIdx;			///< HDMI Window
  LX_HDMI_AUDIO_TYPE_T			audioType;		///< HDMI Audio Type.
  LX_HDMI_SAMPLING_FREQ_T		samplingFreq;	///< Sampling Frequency
  BOOLEAN						bPortConnected;	///< Port connect status (1 : connected, 0 : not connected)
  BOOLEAN						bInterlaced;	///< Scan type (1 : interlace, 0 : progressive)
} LX_HDMI_AUDIO_INFO_T;

/**
 * hdmi ioctl parameter.
 * HDCP parameter.
 * Set HDCP
 * @see HDMI_IOW_SET_HDCP
 */
typedef struct
{
	LX_HDMI_WINDOW_IDX_T srcIdx;		///< HDMI Window
	UINT8 u8HdcpKey[LX_HDCP_KEY_SIZE];  ///< Tx HDCP key
} LX_HDMI_HDCP_T;

/**
 * hdmi ioctl parameter.
 * Get HDMI Color parameter.
 */
typedef enum
{
	LX_HDMI_CS_FULL_RGB = 0,				///< Full RGB (0 ~ 255)
	LX_HDMI_CS_LIMITED_RGB,					///< Limited RGB (16 ~235)

	LX_HDMI_CS_YCBCR422_601,				///< YCBCR422_601
	LX_HDMI_CS_YCBCR422_709,				///< YCBCR422_709
	LX_HDMI_CS_XVYCC422_601,				///< XVYCC422_601
	LX_HDMI_CS_XVYCC422_709,				///< XVYCC422_709

	LX_HDMI_CS_YCBCR444_601,				///< YCBCR444_601
	LX_HDMI_CS_YCBCR444_709,				///< YCBCR444_709
	LX_HDMI_CS_XVYCC444_601,				///< XVYCC444_601
	LX_HDMI_CS_XVYCC444_709,				///< XVYCC444_709

	LX_DVI_CS_FULL_RGB 						///< Full RGB (0 ~ 255)
} LX_HDMI_Color_STATUS_T;

/**
 * hdmi ioctl parameter.
 * HDMI ARC Source.
 * Set HDMI input Sourc.
 */
typedef enum{
	LX_HDMI_ARC_SPDIF_FROM_L9D	= 0,		///< L9 Digital Source
	LX_HDMI_ARC_SPDIF_FROM_PIP0	= 2,		///< L9 HDMI_0 Source
	LX_HDMI_ARC_SPDIF_FROM_PIP1	= 3			///< L9 HDMI_1 Source
} LX_HDMI_ARC_PATH_T;

/**
 * hdmi ioctl parameter.
 * HDMI ARC CONTROL.
 * @see HDMI_IOW_SET_ARC
 */
typedef struct {
	LX_HDMI_WINDOW_IDX_T 		srcIdx;			///< HDMI Window
	BOOLEAN						bARCMute;		///< ARC Mute 0 : Off, 1 : On
	LX_HDMI_ARC_PATH_T			path;			///< 0, 1 : S/PDIF from L9D, 2:S/PDIF channel 0 to L9D, 3:S/PDIF channel 1 to L9D
} LX_HDMI_ARC_CTRL_T;

/**
 * hdmi ioctl parameter.
 * HDMI SPD PACKET Info.
 * Get SPD PACKET Info.
 * @see HDMI_IOR_GET_HDMI_SPD
 */
typedef struct
{
    LX_HDMI_WINDOW_IDX_T 		srcIdx;								///< HDMI Window
    UINT8 VendorName[LX_HDMI_SPD_VENDOR_NAME_LEN+1];  				///< Vendor Name
    UINT8 ProductDescription[LX_HDMI_SPD_PRODUCT_DESC_LEN+1];		///< Product Description
    UINT8 SourceDeviceInfo;											///< Source Device Info
} LX_HDMI_SPD_PACKET_T;

/**
 * hdmi ioctl parameter.
 * HDMI POWER CONTROL.
 * @see HDMI_IOW_POWER_CONTROL
 */
typedef struct {
	LX_HDMI_WINDOW_IDX_T 		srcIdx;			///< HDMI Window
	UINT32						enable;			///< Power Control 0 : Disable, 1 : Enable
} LX_HDMI_POWER_CTRL_T;

/**
 * HDMI PHY register value write type.
 * @see HDMI_IOW_REG_WR
 */
typedef struct {
	UINT32 wr_addr;			///< register address
	UINT32 wr_data;			///< write data
} LX_HDMI_PHY_REG_WRITE_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _HDMI_DRV_H_ */

/** @} */
