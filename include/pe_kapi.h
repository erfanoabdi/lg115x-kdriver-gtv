
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

/** @file pe_kapi.h
 *
 *  application interface header for picture enhance modules.
 *	
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note		
 *	@date		2011.06.11
 *	@see		
 */

#ifndef	_PE_KAPI_H_
#define	_PE_KAPI_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#ifndef USE_XTENSA
#include "base_types.h"
#endif

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	PE_IOC_MAGIC		'a'
#define PE_IOC_MAXNR		100

#define LX_PE_CMG_TBLPOINT			8
#define LX_PE_CMG_REGION_MAX		15
#define LX_PE_CMG_REGION_NUM		(LX_PE_CMG_REGION_MAX+1)
#define LX_PE_CMG_DELTANUM			6
#define LX_PE_CMG_DELTA_SETNUM		2

/* pe debug print define, just for pe drv,io,reg,spd debug */
#undef PE_USE_DBG_PRINT

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_IOWR_PKT					_IOWR(PE_IOC_MAGIC, 0, LX_PE_PKT_T)

#define PE_ITEM_PKTRW(_str)			PE_PKTRW_##_str
#define PE_ITEM_PKTMODL(_str)		PE_PKTMODL_##_str
#define PE_ITEM_PKTFUNC(_str)		PE_PKTFUNC_##_str

#define PE_PKTINFO_RWTYPE_POS		24
#define PE_PKTINFO_RWTYPE_BITS		0x000000ff
#define PE_PKTINFO_MODLTYPE_POS		16
#define PE_PKTINFO_MODLTYPE_BITS	0x000000ff
#define PE_PKTINFO_FUNCTYPE_POS		0
#define PE_PKTINFO_FUNCTYPE_BITS	0x0000ffff

#define PE_PKTINFO_RWTYPE_MAXNUM	PE_PKTINFO_RWTYPE_BITS
#define PE_PKTINFO_MODLTYPE_MAXNUM	PE_PKTINFO_MODLTYPE_BITS
#define PE_PKTINFO_FUNCTYPE_MAXNUM	PE_PKTINFO_FUNCTYPE_BITS

#define PE_LSHIFT_DATA(_val, _bits, _pos)	(((_val)&(_bits))<<(_pos))
#define PE_DATA_MASK(_bits, _pos)			((_bits)<<(_pos))
#define PE_RSHIFT_DATA(_val, _bits, _pos)	(((_val)&PE_DATA_MASK((_bits),(_pos)))>>(_pos))

#define PE_SET_PKTINFO_RWTYPE(_info,_type)	\
	_info = (( (_info) & ~(PE_DATA_MASK(PE_PKTINFO_RWTYPE_BITS,PE_PKTINFO_RWTYPE_POS)) ) \
			| (PE_LSHIFT_DATA(_type,PE_PKTINFO_RWTYPE_BITS,PE_PKTINFO_RWTYPE_POS)))
#define PE_GET_PKTINFO_RWTYPE(_info)	PE_RSHIFT_DATA(_info, PE_PKTINFO_RWTYPE_BITS, PE_PKTINFO_RWTYPE_POS)

#define PE_SET_PKTINFO_MODLTYPE(_info,_type)	\
	_info = (( (_info) & ~(PE_DATA_MASK(PE_PKTINFO_MODLTYPE_BITS,PE_PKTINFO_MODLTYPE_POS)) ) \
			| (PE_LSHIFT_DATA(_type,PE_PKTINFO_MODLTYPE_BITS,PE_PKTINFO_MODLTYPE_POS)))
#define PE_GET_PKTINFO_MODLTYPE(_info)	PE_RSHIFT_DATA(_info, PE_PKTINFO_MODLTYPE_BITS, PE_PKTINFO_MODLTYPE_POS)

#define PE_SET_PKTINFO_FUNCTYPE(_info,_type)	\
	_info = (( (_info) & ~(PE_DATA_MASK(PE_PKTINFO_FUNCTYPE_BITS,PE_PKTINFO_FUNCTYPE_POS)) ) \
			| (PE_LSHIFT_DATA(_type,PE_PKTINFO_FUNCTYPE_BITS,PE_PKTINFO_FUNCTYPE_POS)))
#define PE_GET_PKTINFO_FUNCTYPE(_info)	PE_RSHIFT_DATA(_info, PE_PKTINFO_FUNCTYPE_BITS, PE_PKTINFO_FUNCTYPE_POS)

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/**
 *	pe window index enumeration
 */
typedef enum
{
	LX_PE_WIN_MAIN = 0,				///< main window
	LX_PE_WIN_SUB,					///< sub window

	LX_PE_WIN_ALL,					///< all window
	LX_PE_WIN_NUM = LX_PE_WIN_ALL,	///< number of windows
}
LX_PE_WIN_ID;

/**
 *	pe debug type enumeration
 */
typedef enum {
	LX_PE_DBG_NONE		= 0x0,	///< set nothing, reserved
	LX_PE_DBG_LEVEL		= 0x1,	///< debug print level on, off
	LX_PE_DBG_BYPASS	= 0x2,	///< bypass mode on, off
	LX_PE_DBG_ALL		= LX_PE_DBG_LEVEL|LX_PE_DBG_BYPASS	///< set all
}
LX_PE_DBG_TYPE;

/**
 *	pe source type enumeration
 */
typedef enum {
	LX_PE_SRC_DTV = 0,	///< digital input
	LX_PE_SRC_ATV,		///< analog rf input
	LX_PE_SRC_CVBS,		///< analog cvbs, s-video input
	LX_PE_SRC_COMP,		///< externel input, component
	LX_PE_SRC_RGBPC,	///< externel input, rgb-pc
	LX_PE_SRC_HDMI,		///< externel input, hdmi
	LX_PE_SRC_NUM		///< max num
}
LX_PE_SRC_TYPE;

/**
 *	pe format type enumeration
 */
typedef enum {
	LX_PE_FMT_HD = 0,		///< hd input
	LX_PE_FMT_SD,			///< sd input
	LX_PE_FMT_NTSC,			///< analog input, ntsc
	LX_PE_FMT_PAL,			///< analog input, pal
	LX_PE_FMT_SECAM,		///< analog input, secam
	LX_PE_FMT_SCARTRGB,		///< analog input, scartrgb
	LX_PE_FMT_NUM			///< max num
}
LX_PE_FMT_TYPE;

/**
 *	pe output type enumeration
 */
typedef enum {
	LX_PE_OUT_2D = 0,	///< single normal
	LX_PE_OUT_3D_2D,	///< 3d to 2d
	LX_PE_OUT_UD,		///< ud
	LX_PE_OUT_TB,		///< top and bottom
	LX_PE_OUT_SS,		///< side by side
	LX_PE_OUT_FS,		///< frame sequential
	LX_PE_OUT_DUAL,		///< dual screen
	LX_PE_OUT_PIP,		///< pip screen
	LX_PE_OUT_NUM		///< max num
}
LX_PE_OUT_TYPE;

/**
 *	pe frame rate type enumeration
 */
typedef enum {
	LX_PE_FR_60HZ = 0,	///< frame rate 60hz
	LX_PE_FR_50HZ,		///< frame rate 50hz
	LX_PE_FR_NUM		///< max num
}
LX_PE_FR_TYPE;

/**
 *	pe scan type enumeration
 */
typedef enum {
	LX_PE_SCAN_INTERLACE = 0,	///< scan type interlace
	LX_PE_SCAN_PROGRESS,		///< scan type progress
	LX_PE_SCAN_NUM				///< max num
}
LX_PE_SCAN_TYPE;

/**
 *	pe dtv play type enumeration
 */
typedef enum {
	LX_PE_DTV_NORMAL = 0,	///< dtv type normal
	LX_PE_DTV_FILEPLAY,		///< dtv type fileplay
	LX_PE_DTV_HDDPLAY,		///< dtv type hddplay
	LX_PE_DTV_TESTPIC,		///< dtv type test picture
	LX_PE_DTV_NUM			///< max num
}
LX_PE_DTV_TYPE;

/**
 * pe 3D formatter image format parameter.
 * see LX_3D_IMG_FMT_IN_T
 */
typedef enum {
	LX_PE_3D_IN_TB = 0,		///< top and bottom
	LX_PE_3D_IN_SS,			///< side by side
	LX_PE_3D_IN_QC,			///< quincunx
	LX_PE_3D_IN_CB,			///< check board
	LX_PE_3D_IN_FP,			///< frame packing
	LX_PE_3D_IN_FPI,		///< frame packing interlace
	LX_PE_3D_IN_FA,			///< field alternate
	LX_PE_3D_IN_FS,			///< frame sequence
	LX_PE_3D_IN_LA,			///< line alternate
	LX_PE_3D_IN_SSF,		///< side by side full
	LX_PE_3D_IN_DUAL,		///< dual HD
	LX_PE_3D_IN_CA,			///< column alternate
	LX_PE_3D_IN_LAH,		///< line alternate half
	LX_PE_3D_IN_NUM			///< max number
}
LX_PE_3D_IN_TYPE;

/**
 *	pe histogram bin mode enumeration
 */
typedef enum
{
	LX_PE_64_BINMODE = 0,	///< 64 bin mode
	LX_PE_32_BINMODE,		///< 32 bin mode
	LX_PE_16_BINMODE,		///< 16 bin mode
	LX_PE_BINMODE_NUM		///< max num
}
LX_PE_BINMODE;

/**
 *	pe dynamic contrast enhancement color domain enumeration
 */
typedef enum
{
	LX_PE_HSV_DOMAIN = 0,	///< scaled hsv domain
	LX_PE_YC_DOMAIN,		///< yc domain
	LX_PE_ORG_HSV_DOMAIN,	///< normal hsv domain
	LX_PE_HSL_DOMAIN,		///< hsl domain
	LX_PE_DOMAIN_NUM		///< max num
}
LX_PE_COLOR_DOMAIN;

/**
 *	scaler fir filter type enumeration
 */
typedef enum
{
	LX_PE_POLYPHASE = 0,	///< polyphase filter
	LX_PE_BILINEAR,			///< bilinear filter
	LX_PE_FIRTYPE_NUM		///< max num
}
LX_PE_SCL_FIRTYPE;

/**
 *	scaler filter tap type enumeration
 */
typedef enum
{
	LX_PE_12TAP = 0,		///< 12 tap filter
	LX_PE_06TAP_NORMAL,		///< 6 tap filter, normal case
	LX_PE_06TAP_HALF,		///< 6 tap filter, half case
	LX_PE_06TAP_DOWN_75,	///< 6 tap filter, 75% down scaling case
	LX_PE_06TAP_DOWN_50,	///< 6 tap filter, 50% down scaling case
	LX_PE_06TAP_DOWN_25,	///< 6 tap filter, 25% down scaling case
	LX_PE_FILTERTYPE_NUM	///< max num
}
LX_PE_SCL_FILTERTYPE;

/**
 *	pe resolution enhancement module control type enumeration
 */
typedef enum
{
	LX_PE_RES_NONE	= 0x0,		///< set nothing, reserved
	LX_PE_RES_CMN	= 0x1,		///< common settings
	LX_PE_RES_HOR	= 0x2,		///< horizontal settings
	LX_PE_RES_VER	= 0x4,		///< vertical settings
	LX_PE_RES_ALL	= LX_PE_RES_CMN | LX_PE_RES_HOR | LX_PE_RES_VER	///< set all.
}
LX_PE_SHP_RES_CTRL;

/**
 *	pe edge enhancement module control type enumeration
 */
typedef enum
{
	LX_PE_EDGE_NONE	= 0x0,		///< set nothing, reserved
	LX_PE_EDGE_SI	= 0x1,		///< sharp intensity control
	LX_PE_EDGE_EI	= 0x2,		///< edge intensity control
	LX_PE_EDGE_ALL	= LX_PE_EDGE_SI | LX_PE_EDGE_EI	///< set all.
}
LX_PE_SHP_EDGE_CTRL;

/**
 *	pe d3d module format type enumeration
 */
typedef enum {
	LX_PE_D3D_FMT_2D_2D = 0,
	LX_PE_D3D_FMT_3D_3D,
	LX_PE_D3D_FMT_3D_CB = LX_PE_D3D_FMT_3D_3D,
	LX_PE_D3D_FMT_3D_CC,
	LX_PE_D3D_FMT_3D_LL,
	LX_PE_D3D_FMT_3D_SS,
	LX_PE_D3D_FMT_3D_TB,
	LX_PE_D3D_FMT_UNKNOWN,
	LX_PE_D3D_FMT_MAX
} LX_PE_D3D_FORMAT_T;

/**
 *	pe d3d module debug type enumeration
 */
typedef enum {
	LX_PE_D3D_DEC_OFF,
	LX_PE_D3D_DEC_ON,
	LX_PE_D3D_DEC_PATTERN,
	LX_PE_D3D_DEC_LOG,
	LX_PE_D3D_DEC_FIX_2D,
	LX_PE_D3D_DEC_FIX_3D,
	LX_PE_D3D_DEC_FIX_FULL_WIN,	
	LX_PE_D3D_DEC_MAX
} LX_PE_D3D_DEC_T;

/**
 *	pe d3d module capture debug type enumeration
 */
typedef enum {
	LX_PE_D3D_CAP_OFF,
	LX_PE_D3D_CAP_ON,
	LX_PE_D3D_CAP_FMT,
	LX_PE_D3D_CAP_TIME,
	LX_PE_D3D_CAP_DRAW,
	LX_PE_D3D_CAP_LOG,	
	LX_PE_D3D_CAP_MAX
} LX_PE_D3D_CAP_T;

/**
 *	pe d3d dual screen format type enumeration
 */
typedef enum {
	LX_PE_D3D_DSC_2D,
	LX_PE_D3D_DSC_SS,
	LX_PE_D3D_DSC_TB,
	LX_PE_D3D_DSC_MAX
} LX_PE_D3D_DSC_T;

/**
 *  pe lrcr window position type enumeration
 */
typedef enum
{
	PE_WIN_LRTB_FULL,
	PE_WIN_LRTB_HAF1,
	PE_WIN_LRTB_HAF2,
	PE_WIN_MAX
} LX_PE_WIN_POSITION_T;

/**
 *	packet type descripter for read, write, init
 */
typedef enum
{
	PE_ITEM_PKTRW(INIT)	= 0,	///< init type
	PE_ITEM_PKTRW(SET)	= 1,	///< set type
	PE_ITEM_PKTRW(GET)	= 2,	///< get type
	PE_ITEM_PKTRW(NUM),			///< rw type number
	PE_ITEM_PKTRW(MAX)	= PE_PKTINFO_RWTYPE_MAXNUM	///< max
}
LX_PE_PKT_RWTYPE;

/**
 *	packet type descripter for pe modules
 */
typedef enum
{
	PE_ITEM_PKTMODL(INIT)		= 0,	///< using init module
	PE_ITEM_PKTMODL(DEFAULT)	= 1,	///< set default settings on each module
	PE_ITEM_PKTMODL(DBG)		= 2,	///< set debug settings on each module
	PE_ITEM_PKTMODL(CMN)		= 3,	///< using common module
	PE_ITEM_PKTMODL(CSC)		= 4,	///< using csc module
	PE_ITEM_PKTMODL(CMG)		= 5,	///< using color manege module
	PE_ITEM_PKTMODL(NRD)		= 6,	///< using noise reduction module
	PE_ITEM_PKTMODL(DNT)		= 7,	///< using deinterlacer module
	PE_ITEM_PKTMODL(SHP)		= 8,	///< using sharpness module
	PE_ITEM_PKTMODL(CCM)		= 9,	///< using color correction module
	PE_ITEM_PKTMODL(DCM)		= 10,	///< using dynamic contrast module
	PE_ITEM_PKTMODL(WIN)		= 11,	///< using window control module
	PE_ITEM_PKTMODL(D3D)		= 12,	///< using detect 3d control module
	PE_ITEM_PKTMODL(NUM),				///< module type number
	PE_ITEM_PKTMODL(MAX)		= PE_PKTINFO_MODLTYPE_MAXNUM	///< max
}
LX_PE_PKT_MODLTYPE;

/**
 *	packet type descripter for functions
 */
typedef enum
{
	PE_ITEM_PKTFUNC(LX_PE_INIT_SETTINS_T)		= 0,	///< using struct LX_PE_INIT_SETTINS_T
	PE_ITEM_PKTFUNC(LX_PE_DEFAULT_SETTINGS_T)	= 1,	///< using struct LX_PE_DEFAULT_SETTINGS_T
	PE_ITEM_PKTFUNC(LX_PE_DBG_SETTINGS_T)		= 2,	///< using struct LX_PE_DBG_SETTINGS_T
	PE_ITEM_PKTFUNC(LX_PE_D3D_SETTINGS_T)		= 3,	///< using struct LX_PE_D3D_SETTINGS_T
	PE_ITEM_PKTFUNC(LX_PE_D3D_INFO_T)			= 4,	///< using struct LX_PE_D3D_INFO_T
	PE_ITEM_PKTFUNC(LX_PE_D3D_THR_T)			= 5,	///< using struct LX_PE_D3D_THR_T	
	PE_ITEM_PKTFUNC(LX_PE_D3D_DSC_INFO_T)       = 6,	///< using struct LX_PE_D3D_DSC_INFO_T	

	PE_ITEM_PKTFUNC(LX_PE_CMN_CONTRAST_T)		= 1000,	///< using struct LX_PE_CMN_CONTRAST_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_BRIGHTNESS_T)		= 1001,	///< using struct LX_PE_CMN_BRIGHTNESS_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_SATURATION_T)		= 1002,	///< using struct LX_PE_CMN_SATURATION_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_HUE_T)			= 1003,	///< using struct LX_PE_CMN_HUE_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_SIMUL_MODE_T)		= 1004,	///< using struct LX_PE_CMN_SIMUL_MODE_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_LEVEL_CTRL_T)		= 1005,	///< using struct LX_PE_CMN_LEVEL_CTRL_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_SUB_CONTRAST_T)	= 1006,	///< using struct LX_PE_CMN_SUB_CONTRAST_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_FWI_CTRL_T)		= 1007,	///< using struct LX_PE_CMN_FWI_CTRL_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_MUTE_STATE_T)		= 1008,	///< using struct LX_PE_CMN_MUTE_STATE_T
	PE_ITEM_PKTFUNC(LX_PE_CMN_BYPASS_T)			= 1009,	///< using struct LX_PE_CMN_BYPASS_T

	PE_ITEM_PKTFUNC(LX_PE_CSC_XVYCC_T)			= 2000,	///< using struct LX_PE_CSC_XVYCC_T
	PE_ITEM_PKTFUNC(LX_PE_CSC_GAMUT_T)			= 2001,	///< using struct LX_PE_CSC_GAMUT_T

	PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_ENABLE_T)	= 3000,	///< using struct LX_PE_CMG_REGION_ENABLE_T
	PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_T)			= 3001,	///< using struct LX_PE_CMG_REGION_T
	PE_ITEM_PKTFUNC(LX_PE_CMG_REGION_CTRL_T)	= 3002,	///< using struct LX_PE_CMG_REGION_CTRL_T
	PE_ITEM_PKTFUNC(LX_PE_CMG_GLOBAL_CTRL_T)	= 3003,	///< using struct LX_PE_CMG_GLOBAL_CTRL_T
	PE_ITEM_PKTFUNC(LX_PE_CMG_COLOR_CTRL_T)		= 3004,	///< using struct LX_PE_CMG_COLOR_CTRL_T

	PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_T)			= 4000,	///< using struct LX_PE_NRD_DNR_T
	PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_T)			= 4001,	///< using struct LX_PE_NRD_TNR_T
	PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_CMN_T)		= 4002,	///< using struct LX_PE_NRD_DNR_CMN_T
	PE_ITEM_PKTFUNC(LX_PE_NRD_DNR_DETAIL_T)		= 4003,	///< using struct LX_PE_NRD_DNR_DETAIL_T
	PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_CMN_T)		= 4004,	///< using struct LX_PE_NRD_TNR_CMN_T
	PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_DETAIL_T)		= 4005,	///< using struct LX_PE_NRD_TNR_DETAIL_T
	PE_ITEM_PKTFUNC(LX_PE_NRD_TNR_CRGN_T)		= 4006,	///< using struct LX_PE_NRD_TNR_CRGN_T	

	PE_ITEM_PKTFUNC(LX_PE_DNT_FILMMODE_T)		= 5000,	///< using struct LX_PE_DNT_FILMMODE_T

	PE_ITEM_PKTFUNC(LX_PE_SHP_RES_T)			= 6000,	///< using struct LX_PE_SHP_RES_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_EDGE_T)			= 6001,	///< using struct LX_PE_SHP_EDGE_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_RES_ENABLE_T)		= 6002,	///< using struct LX_PE_SHP_RES_ENABLE_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_SCLFILTER_T)		= 6003,	///< using struct LX_PE_SHP_SCLFILTER_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_RE_CMN_T)			= 6004,	///< using struct LX_PE_SHP_RE_CMN_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_RE_HOR_T)			= 6005,	///< using struct LX_PE_SHP_RE_HOR_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_RE_VER_T)			= 6006,	///< using struct LX_PE_SHP_RE_VER_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_RE_MISC_T)		= 6007,	///< using struct LX_PE_SHP_RE_VER_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_EE_T)				= 6008,	///< using struct LX_PE_SHP_EE_T
	PE_ITEM_PKTFUNC(LX_PE_SHP_DE_T)				= 6009,	///< using struct LX_PE_SHP_DE_T

	PE_ITEM_PKTFUNC(LX_PE_CCM_GAMMALUT_T)		= 7000,	///< using struct LX_PE_CCM_GAMMALUT_T
	PE_ITEM_PKTFUNC(LX_PE_CCM_PIXEL_REPLACE_T)	= 7001,	///< using struct LX_PE_CCM_PIXEL_REPLACE_T
	PE_ITEM_PKTFUNC(LX_PE_CCM_WB_T)				= 7002,	///< using struct LX_PE_CCM_WB_T
	PE_ITEM_PKTFUNC(LX_PE_CCM_AUTO_CR_T)		= 7003,	///< using struct LX_PE_CCM_AUTO_CR_T

	PE_ITEM_PKTFUNC(LX_PE_DCM_HISTO_INFO_T)		= 8000,	///< using struct LX_PE_DCM_HISTO_INFO_T
	PE_ITEM_PKTFUNC(LX_PE_DCM_DCE_CONF_T)		= 8001,	///< using struct LX_PE_DCM_DCE_CONF_T
	PE_ITEM_PKTFUNC(LX_PE_DCM_DCE_LUT_T)		= 8002,	///< using struct LX_PE_DCM_DCE_LUT_T
	PE_ITEM_PKTFUNC(LX_PE_DCM_DSE_LUT_T)		= 8003,	///< using struct LX_PE_DCM_DSE_LUT_T
	PE_ITEM_PKTFUNC(LX_PE_DCM_BLENDING_T)		= 8004,	///< using struct LX_PE_DCM_BLENDING_T

	PE_ITEM_PKTFUNC(LX_PE_WIN_CTRL_T)			= 9000,	///< using struct LX_PE_WIN_CTRL_T
	PE_ITEM_PKTFUNC(LX_PE_WIN_D3D_INFO_T)		= 9001,	///< using struct LX_PE_WIN_D3D_INFO_T

	PE_ITEM_PKTFUNC(MAX)						= PE_PKTINFO_FUNCTYPE_MAXNUM	///< max
}
LX_PE_PKT_FUNCTYPE;

/**
 *	pe packet
 */
typedef struct
{
	UINT32 info;	///< [31:24]LX_PE_PKT_RWTYPE | [23:16]LX_PE_PKT_MODLTYPE | [15:0]LX_PE_PKT_FUNCTYPE
	UINT32 size;	///< size of data
	void *data;		///< packet data
}
LX_PE_PKT_T;

/**
 *	pe packet io
 */
typedef struct
{
	LX_PE_PKT_T iopkt;	///< packet to use ioctl
	void *input;		///< input of iopkt.data
	UINT32 rwtype;		///< input of iopkt.info, see LX_PE_PKT_RWTYPE
	UINT32 moduletype;	///< input of iopkt.info, see LX_PE_PKT_MODLTYPE
	UINT32 functype;	///< input of iopkt.info, see LX_PE_PKT_FUNCTYPE
	UINT32 size;		///< input of iopkt.size
}
LX_PE_PKT_IO_T;

/**
 *	input struct for calling kadp functions
 */
typedef struct
{
	void *data;			///< input param to use kadp functions
	UINT32 functype;	///< function type, see LX_PE_PKT_FUNCTYPE
	UINT32 size;		///< size of data
}
LX_PE_CTRL_PARAM_T;

/**
 *	pe debug settings control parameter type (only for debug)
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	LX_PE_DBG_TYPE	type;	///< debug type
	UINT32 print_lvl;		///< pe debug print level
	UINT32 bypass;			///< bypass each module
}
LX_PE_DBG_SETTINGS_T;

/**
 *	pe default settings control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID	win_id;		///< window id
	LX_PE_SRC_TYPE	src_type;	///< pe source type
	LX_PE_FMT_TYPE	fmt_type;	///< pe format type
	LX_PE_OUT_TYPE	out_type;	///< pe out type
	LX_PE_FR_TYPE	fr_type;	///< pe frame rate type
	LX_PE_SCAN_TYPE	scan_type;	///< pe scan type
	LX_PE_DTV_TYPE	dtv_type;	///< pe dtv play type
}
LX_PE_DEFAULT_SETTINGS_T;

/**
 *	pe initial settings control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	UINT32 suspend_mode;		///< suspend mode, 0:off(default), 1:on
}
LX_PE_INIT_SETTINS_T;

/***************************************************************************/
/* CMN : Common */
/***************************************************************************/
/**
 *	pe contrast control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT16 contrast;		///< contrast value
}
LX_PE_CMN_CONTRAST_T;

/**
 *	pe brightness control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
    UINT16 brightness;		///< brightness value
}
LX_PE_CMN_BRIGHTNESS_T;

/**
 *	pe saturation control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
    UINT16 saturation;		///< saturation value
}
LX_PE_CMN_SATURATION_T;

/**
 *	pe hue control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
    UINT16 hue;				///< hue value
}
LX_PE_CMN_HUE_T;

/**
 *	pe simultaneous mode on off
 */
typedef struct
{
    UINT32 on_off;	///< 0x1:on, 0x0:off
}
LX_PE_CMN_SIMUL_MODE_T;

/**
 *	pe level control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
    UINT16 offset;			///< display input y offset, 0~512~1023
    UINT16 center;			///< center position for y gain control, 0~128~255
    UINT16 gain;			///< display input y gain, 0~128~255
}
LX_PE_CMN_LEVEL_CTRL_T;

/**
 *	pe sub contrast control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
    UINT16 contrast;		///< sub contrast, 0~512~1023
}
LX_PE_CMN_SUB_CONTRAST_T;

/**
 *	pe firmware control parameter type
 */
typedef struct
{
	UINT32 use_fw_en;		///< use fw or not
	UINT32 fw_ctrl_en;		///< fw control enable or not
	UINT32 dbg_print_en;	///< dbg print enable or not
}
LX_PE_CMN_FWI_CTRL_T;

/**
 *	pe mute state parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 on_off;			///< to inform mute on, off to pe drv
}
LX_PE_CMN_MUTE_STATE_T;

/**
 *	pe block bypass control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 front_bypass_en;	///< to bypass front pe, on,off
}
LX_PE_CMN_BYPASS_T;

/***************************************************************************/
/* CCM : Color Correctin Module */
/***************************************************************************/
/**
 *	pe gamma lut control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 lut_size;		///< size of lut_data
	UINT32 *lut_data;		///< [29:20]R | [19:10]B | [9:0]G
}
LX_PE_CCM_GAMMALUT_T;

/**
 *	pe gamma pixel replacement control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT8 pxl_rep_r;		///< R value for pixel replacement, 1:on, 0:off, default:0
	UINT8 pxl_rep_g;		///< G value for pixel replacement, 1:on, 0:off, default:0
	UINT8 pxl_rep_b;		///< B value for pixel replacement, 1:on, 0:off, default:0
}
LX_PE_CCM_PIXEL_REPLACE_T;

/**
 *	pe white balance control parameter type
 */
typedef struct {
	LX_PE_WIN_ID win_id;	///< window id
	UINT8 r_gain;			///< red gain, 		0~255(192= 1.0 gain)
	UINT8 g_gain;			///< green gain, 	0~255(192= 1.0 gain)
	UINT8 b_gain;			///< blue gain, 	0~255(192= 1.0 gain)
	UINT8 r_offset; 		///< red offset,	0~255(128= zero offset)
	UINT8 g_offset; 		///< green offset,	0~255(128= zero offset)
	UINT8 b_offset; 		///< blue offset,	0~255(128= zero offset)
}
LX_PE_CCM_WB_T;

/**
 *	pe left right mismatch correction control parameter type
 */
typedef struct {
	UINT8 enable;			///< enable LRCR, 1:on, 0:off
	UINT8 th_max_hist;		///< if(max_hist > th_max_hist), auto cr does not apply
	UINT8 th_valid_bins;	///< if(n_valid_bins > th_valid_bins), auto cr does not apply
	UINT8 adj_th0;			///< adjustment threshold 0
	UINT8 adj_th1;			///< adjustment threshold 1
}
LX_PE_CCM_AUTO_CR_T;

/***************************************************************************/
/* DCM : Dynamic Contrast Module */
/***************************************************************************/
/**
 *	pe histogram information parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	LX_PE_COLOR_DOMAIN domain;	///< color domain
	UINT32 bin_num;				///< bin number 16 or 32 or 64
	UINT32 histo_ready;			///< histogram is valid, if 1, invalid otherwise.
	UINT32 average[3];			///< the average of all luma in the specified region Y or RGB
    UINT32 min;					///< the min of all luma in the specified region
	UINT32 max;					///< the max of all luma in the specified region
    UINT32 histogram[64];		///< histogram data of luma
    UINT32 sat_status;			///< saturation status
    UINT32 status[5];			///< histogram status
    UINT32 motion;				///< motion status(from tnr block)
}
LX_PE_DCM_HISTO_INFO_T;

/**
 *	pe dynamic contrast enhancement configuration parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;			///< window id
	LX_PE_COLOR_DOMAIN domain;		///< dce color domain.
	LX_PE_BINMODE bin_mode;			///< histogram bin number mode
	UINT32 lut_step;				///< lut step
    UINT32 min_pos;					///< the min position, default "26"(0~255(0%~100%))
	UINT32 max_pos;					///< the max position, default "220"(0~255(0%~100%))
}
LX_PE_DCM_DCE_CONF_T;

/**
 *	pe dynamic contrast lut parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	UINT32 step;				///< lut step, size of data, sync with the lut step on dce config
	UINT32 *data;				///< [9:0] y data, [25:16] x data
}
LX_PE_DCM_DCE_LUT_T;

/**
 *	pe dynamic saturation lut parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	UINT32 step;				///< lut step, size of data, sync with the lut step on dce config
	UINT32 *data;				///< [9:0] y data, [25:16] x data
}
LX_PE_DCM_DSE_LUT_T;

/**
 *	pe dynamic contrast bypass blending control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 color_out_gain;	///< dce out gain, 1(0%)~128(200%)~255(400%) (forbidden 0)
	UINT32 region_sel;		///< LSB : select region 0
}
LX_PE_DCM_BLENDING_T;

/***************************************************************************/
/* WIN : Window control */
/***************************************************************************/

/**
 *	pe window position type
 */
typedef struct
{
	UINT16 pos_x0;				///< start position of window, x position
	UINT16 pos_y0;				///< start position of window, y position
	UINT16 pos_x1;				///< end position of window, x position
	UINT16 pos_y1;				///< end position of window, y position
}
LX_PE_WIN_POS_T;

/**
 *	pe window parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	LX_PE_OUT_TYPE out_type;	///< output type
	LX_PE_3D_IN_TYPE in_type;	///< 3d input type
	UINT16 pos_x;				///< start position of window, x position
	UINT16 pos_y;				///< start position of window, y position
	UINT16 size_w;				///< width of window
	UINT16 size_h;				///< height of window
	UINT16 ofs_x;				///< x offset
	UINT16 ofs_y;				///< y offset
}
LX_PE_WIN_CTRL_T;

/**
 *	pe auto detection info. parameter type
 */
typedef struct
{
	LX_PE_WIN_POS_T top;		///< valid position of TB top
	LX_PE_WIN_POS_T bottom;		///< valid position of TB bottom
	LX_PE_WIN_POS_T left;		///< valid position of SS left
	LX_PE_WIN_POS_T right;		///< valid position of SS right
	UINT16 tb_ratio;			///< detection ratio for tb (1024=100%)
	UINT16 ss_ratio;			///< detection ratio for ss (1024=100%)
}
LX_PE_WIN_D3D_INFO_T;

/***************************************************************************/
/* CSC : Color Space Conversion */
/***************************************************************************/

/**
 *	pe xvYCC control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT8 enable;			///< enable xvYCC, 1:on, 0:off
	UINT8 scaler;			///< scaling factor, 0~255(255 = x1.0)
}
LX_PE_CSC_XVYCC_T;

/**
 *	pe color gamut control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT16 matrix[9];		///< 3x3 matrix, control primary color correction
}
LX_PE_CSC_GAMUT_T;

/***************************************************************************/
/* CMG : Color Management */
/***************************************************************************/

/**
 *	pe color enhancement region enable parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;						///< window id
	UINT8 enable[LX_PE_CMG_REGION_NUM];			///< enable region 0 ~ 15
	UINT8 show_region[LX_PE_CMG_REGION_NUM];	///< show region 0 ~ 15, for debug
}
LX_PE_CMG_REGION_ENABLE_T;

/**
 *	pe color enhancement region parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;					///< window id
	UINT32 region_num;						///< region number, 0~15
	UINT16 hue_x[LX_PE_CMG_TBLPOINT];		///< hue input, 0~512~1023(0~360~720 degree)
	UINT8 hue_g[LX_PE_CMG_TBLPOINT];		///< hue gain, 0~127
	UINT8 sat_x[LX_PE_CMG_TBLPOINT];		///< saturation input, 0~100
	UINT8 sat_g[LX_PE_CMG_TBLPOINT];		///< saturation gain, 0~127
	UINT8 val_x[LX_PE_CMG_TBLPOINT];		///< value input, 0~255
	UINT8 val_g[LX_PE_CMG_TBLPOINT];		///< value gain, 0~127
}
LX_PE_CMG_REGION_T;

/**
 *	pe color enhancement region control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;						///< window id
	UINT32 region_num;							///< region number, 0~15
	SINT8 region_delta[LX_PE_CMG_DELTANUM];		///< region delta(offset), -128 ~ 127, [0]h [1]s [2]v [3]g [4]b [5]r
	UINT8 master_gain;							///< region master gain, 0~128~255
}
LX_PE_CMG_REGION_CTRL_T;

/**
 *	pe color enhancement global control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;						///< window id
	SINT16 global_delta[LX_PE_CMG_DELTANUM];	///< global gain,-512 ~ 511, [0]h [1]s [2]v [3]g [4]b [5]r
}
LX_PE_CMG_GLOBAL_CTRL_T;

/**
 *	pe color enhancement color control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT16 saturation;		///< saturation gain
}
LX_PE_CMG_COLOR_CTRL_T;

/***************************************************************************/
/* NRD : Noise Reduction*/
/***************************************************************************/

/**
 *	pe digital noise reduction control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 mnr_enable;		///< mosquito nr enable
	UINT32 ac_bnr_h_en;		///< horizontal ac block nr enable
	UINT32 ac_bnr_v_en;		///< vertical ac block nr enable
	UINT32 ac_bnr_gain;		///< ac block nr gain, strength h max
	UINT32 dc_bnr_gain;		///< dc block nr gain
	UINT32 mnr_gain;		///< mosquito nr gain, filter threshold
}
LX_PE_NRD_DNR_T;

/**
 *	pe temporal noise reduction control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 tnr_enable;		///< tnr enable
	UINT32 adj_nt_lvl_val;	///< adjust noise temporal level value
}
LX_PE_NRD_TNR_T;

/**
 *	pe digital noise reduction common control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;				///< window id
	/* mnr */
	UINT8 mnr_enable;					///< mnr_enable
	UINT8 filter_threshold;				///< Bi-lateral filter threshold
	/* ac_bnr */
	UINT8 bnr_h_en;						///< bnr_h en
	UINT8 bnr_v_en;						///< bnr_v_en
	UINT8 ac_bnr_protect_motion_max;	///< AC_BNR_PROTECT_motion_max
	UINT8 ac_bnr_protect_motion_min;	///< AC_BNR_PROTECT_motion_min
	UINT8 hbmax_gain;					///< HBmax gain
	UINT8 vbmax_gain;					///< VBmax gain
	UINT8 strength_h_max;				///< strength_h_max
	UINT8 strength_v_max;				///< strength_v_max
	/* dc_bnr */
	UINT8 dc_bnr_enable;				///< DC BNR enable
	UINT8 motion_min;					///< Motion min
	UINT8 motion_max;					///< Motion max
	UINT8 ac_blockiness_multifly;		///< AC Blockiness Multifly
}
LX_PE_NRD_DNR_CMN_T;

/**
 *	pe digital noise reduction detailed control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;				///< window id
	/* mnr */
	UINT8 edge_gain_mapping_enable;		///< edge gain mapping enable
	UINT8 sel_sd_hd;					///< Sel_SD_H
	UINT8 x1_position;					///< X1 position
	UINT8 x2_position;					///< X2 position
	UINT8 x3_position;					///< X3 position
	UINT8 x4_position;					///< X4 position
	UINT8 y1_position;					///< Y1 position
	UINT8 y2_position;					///< Y2 position
	UINT8 y3_position;					///< Y3 position
	UINT8 y4_position;					///< Y4 position
	/* ac_bnr */
	UINT8 motion_protect_enable;		///< motion protect enable
	UINT8 source_type;					///< source type
	UINT8 fiter_type;					///< fiter type
	UINT8 strength_h_x0;				///< strength_h_x0
	UINT8 strength_h_x1;				///< strength_h_x1
	UINT8 detect_min_th;				///< Block Line Detection Minimun Threshold
	UINT8 strength_v_x0;				///< strength_v_x0
	UINT8 strength_v_x1;				///< strength_v_x1
	/* dc_bnr */
	UINT8 ac_blockiness_adaptive_mode;	///< AC Blockiness Adaptive Mode
	UINT8 motion_gain_enable;			///< motion gain enable
	UINT8 sd_hd_sel;					///< sd_hd se
	UINT8 dc_bnr_gain_ctrl_y2;			///< dc_bnr_gain_ctrl_Y2
	UINT8 dc_bnr_gain_ctrl_x2;			///< dc_bnr_gain_ctrl_X2
	UINT8 dc_bnr_gain_ctrl_y3;			///< dc_bnr_gain_ctrl_Y3
	UINT8 dc_bnr_gain_ctrl_x3;			///< dc_bnr_gain_ctrl_X3
	UINT8 dc_bnr_gain_ctrl_y0;			///< dc_bnr_gain_ctrl_Y0
	UINT8 dc_bnr_gain_ctrl_x0;			///< dc_bnr_gain_ctrl_X0
	UINT8 dc_bnr_gain_ctrl_y1;			///< dc_bnr_gain_ctrl_Y1
	UINT8 dc_bnr_gain_ctrl_x1;			///< dc_bnr_gain_ctrl_X1
}
LX_PE_NRD_DNR_DETAIL_T;

/**
 *	pe temporal noise reduction common control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	/* auto mode */
	UINT8 auto_mode;			///< auto nr mode, on(1),off(0)
	/* noise level gaining */
	UINT8 adjust_nt_lvl;		///< Adjust NT_Lvl enable
	UINT8 adjust_nt_lvl_val;	///< Adjust NT_Lvl value (8u, "this-128" will be added to Calculated NT_Lvl)
	/* alpha gain */
	UINT8 y_gain;				///< Y_TNR_Gain (3.5u)
	UINT8 c_gain;				///< C_TNR_Gain (3.5u)
}
LX_PE_NRD_TNR_CMN_T;

/**
 *	pe temporal noise reduction detailed control parameter type
 *	motion -> noise level(n-lvl) estimate -> n-lvl gaining -> n-lvl apply -> alpha gaining
 *         -> motion filter -> n-lvl apply
 *         -> scene change -> alpha gaining
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	/* adjust alpha(alpha gaining) */
	UINT8 wa_enable;					///< TNR strength adjust enable using spatial variance
	UINT8 wa_y0;						///< 0.8u,wa_Y0
	UINT8 wa_y1;						///< 0.8u,wa_Y1
	UINT8 wa_x0;						///< spatial variance threshold 0
	UINT8 wa_x1;						///< spatial variance threshold 1
	UINT8 adj_alpha_nt_lvl_th0;			///< adj_alpha_nt_lvl_th (LUTAlpha => finalAlpha)
	UINT8 adj_alpha_k0;					///< adj_alpha_k0 (LUTAlpha => finalAlpha) (4.4u, 6.0d=>0x20)
	UINT8 adj_alpha_k1;					///< adj_alpha_k1 (LUTAlpha => finalAlpha) (4.4u, 6.0d=>0x50)
	UINT8 adj_alpha_nt_lvl_th1;			///< adj_alpha_nt_lvl_th (LUTAlpha => finalAlpha)
	UINT8 wa_y2;						///< 0.8u,wa_Y2
	UINT8 wa_y3;						///< 0.8u,wa_Y3
	UINT8 wa_x2;						///< spatial variance threshold 2
	UINT8 wa_x3;						///< spatial variance threshold 3
	/* motion histo(n-lvl estimate) */
	UINT8 calc_motion_flt_enable;		///< motion filtering enable
	UINT8 calc_motion_flt_type;			///< motion filtering type
	UINT16 y_th_lo;						///< Y-Valid Range. tnr_y_th_lo (64d=>0x040)
	UINT16 y_th_hi;						///< Y-Valid Range. tnr_y_th_hi (1000d=>0x3E8)
	/* alpha gain(n-lvl gaining), gm,st gain */
	UINT8 nt_lvl_adjust_gm_enable;		///< noise level adjust (by the global motion amount) enable
	UINT8 nt_lvl_adjust_avg_ts_enable;	///< noise level adjust (by the (avg_t - avg_s))  enable
	UINT8 nt_lvl_gain_gm_y0;			///< 1.7u,nt_lvl_gain_gm_y0
	UINT8 nt_lvl_gain_gm_x0;			///< nt_lvl_gain_gm_x0
	UINT8 nt_lvl_gain_gm_y1;			///< 1.7u, nt_lvl_gain_gm_y1
	UINT8 nt_lvl_gain_gm_x1;			///< nt_lvl_gain_gm_x1
	UINT8 nt_lvl_gain_gm_y2;			///< 1.7u,nt_lvl_gain_gm_y2
	UINT8 nt_lvl_gain_gm_x2;			///< nt_lvl_gain_gm_x2
	UINT8 nt_lvl_gain_gm_y3;			///< 1.7u,nt_lvl_gain_gm_y3
	UINT8 nt_lvl_gain_gm_x3;			///< nt_lvl_gain_gm_x3
	UINT8 nt_lvl_gain_st_y0;			///< 1.7u,nt_lvl_gain_st_y0
	UINT8 nt_lvl_gain_st_x0;			///< 1.7u * avg_s,nt_lvl_gain_st_x0
	UINT8 nt_lvl_gain_st_y1;			///< 1.7u,nt_lvl_gain_st_y1
	UINT8 nt_lvl_gain_st_x1;			///< 1.7u * avg_s,nt_lvl_gain_st_x1
	UINT8 nt_lvl_gain_st_y2;			///< 1.7u,nt_lvl_gain_st_y2
	UINT8 nt_lvl_gain_st_x2;			///< 1.7u * avg_s,nt_lvl_gain_st_x2
	UINT8 nt_lvl_gain_st_y3;			///< 1.7u,nt_lvl_gain_st_y3
	UINT8 nt_lvl_gain_st_x3;			///< 1.7u * avg_s,nt_lvl_gain_st_x3
	/* alpha lut(n-lvl apply) */
	UINT8 alpha_lut_ind7;				///< alpha_lut_ind7 (NTLvl => Alpha LUT Curve) (52d=>0x34)
	UINT8 alpha_lut_ind0;				///< alpha_lut_ind0 (NTLvl => Alpha LUT Curve) (9d=>0x09)
	UINT8 alpha_lut_ind1;				///< alpha_lut_ind1 (NTLvl => Alpha LUT Curve) (17d=>0x11)
	UINT8 alpha_lut_ind2;				///< alpha_lut_ind2 (NTLvl => Alpha LUT Curve) (24d=>0x18)
	UINT8 alpha_lut_ind3;				///< alpha_lut_ind3 (NTLvl => Alpha LUT Curve) (30d=>0x1E) 
	UINT8 alpha_lut_ind4;				///< alpha_lut_ind4 (NTLvl => Alpha LUT Curve) (40d=>0x28)
	UINT8 alpha_lut_ind5;				///< alpha_lut_ind5 (NTLvl => Alpha LUT Curve) (46d=>0x2E)
	UINT8 alpha_lut_ind6;				///< alpha_lut_ind6 (NTLvl => Alpha LUT Curve) (52d=>0x34)
	/* etc */
	UINT8 nt_lvl_adjust_lpct_enable;	///< noise level adjust (by the percent of smooth area) enable
	UINT8 luma_gain_enable;				///< luma gain control enable
}
LX_PE_NRD_TNR_DETAIL_T;

/**
 *	pe tnr color region gain control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	UINT32 gain_rgn[4];			///< color region gain, 0(g0.0) ~ 32(default,g1.0) ~ 255(g8.0)
}
LX_PE_NRD_TNR_CRGN_T;

/***************************************************************************/
/* DNT : Deinterlace */
/***************************************************************************/
/**
 *	pe deinterlacer, film(3:2,2:2 pull down) mode control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 enable;			///< enable film mode, 1:on, 0:off
}
LX_PE_DNT_FILMMODE_T;

/***************************************************************************/
/* SHP : Sharpness*/
/***************************************************************************/
/**
 *	pe resolution enhancement module normal control parameter type
 */
typedef struct
{
	UINT8 peaking_w_gain;		///< peaking white gain
	UINT8 peaking_b_gain;		///< peaking black gain
	UINT8 detail_master_b_gain;	///< detail master black gain
	UINT8 detail_master_w_gain;	///< detail master white gain
	UINT8 detail_lap_gain;		///< detail laplacian gain
	UINT8 detail_sobel_gain;	///< detail sobel gain
	UINT8 detail_lap_w_gain;	///< detail laplacian white gain
	UINT8 detail_lap_b_gain;	///< detail laplacian black gain
}
LX_PE_SHP_RES_CMN_T;

/**
 *	pe resolution enhancement module horizontal control parameter type
 *	- dr  : deringing
 *	- ti  : transition improvement
 *	- cti : color transition improvement
 *	- ycm : y(luma) chroma matching
 */
typedef struct
{
	UINT8 peaking_hor_gain;		///< peaking, horiznotal gain
	UINT8 dr_csft_gain;			///< dr, center shift gain
	UINT8 dr_edgef_w_gain;		///< dr, edge filter white gain
	UINT8 dr_edgef_b_gain;		///< dr, edge filter black gain
	UINT8 ti_hpre_ygain;		///< ti, horizontal y gain(pre)
	UINT8 ti_hpre_cgain;		///< ti, horizontal c gain(pre)
	UINT8 ti_hpost_ygain;		///< ti, horizontal y gain(post)
	UINT8 ti_hpost_cgain;		///< ti, horizontal c gain(post)
	UINT8 cti_tap_size;			///< cti, tap size
	UINT8 cti_gain;				///< cti, gain
	UINT8 cti_ycm_band_sel;		///< cti, ycm band selection
	UINT8 cti_ycm_diff_th;		///< cti, ycm diff threshold
	UINT8 cti_ycm_y_gain;		///< cti, ycm y gain
	UINT8 cti_ycm_c_gain;		///< cti, ycm c gain
}
LX_PE_SHP_RES_HOR_T;

/**
 *	pe resolution enhancement module vertical control parameter type
 *	- dr  : deringing
 *	- ti  : transition improvement
 */
typedef struct
{
	UINT8 dr_vbif_th;			///< dr, vertical bilateral filter threshold
	UINT8 peaking_ver_gain;		///< peaking, vertical gain
    UINT8 ti_v_gain;			///< ti, vertical gain(pre)
}
LX_PE_SHP_RES_VER_T;

/**
 *	pe resolution enhancement module control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;			///< window id
	LX_PE_SHP_RES_CTRL type; 		///< ctrl type
	LX_PE_SHP_RES_CMN_T cmn_ctrl;	///< common setting
	LX_PE_SHP_RES_HOR_T hor_ctrl;	///< horizontal setting
	LX_PE_SHP_RES_VER_T ver_ctrl;	///< vertical setting
}
LX_PE_SHP_RES_T;

/**
 *	pe resolution enhancement module edge control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	LX_PE_SHP_EDGE_CTRL type; 	///< ctrl type
	UINT8 sharp_intensity;		///< sharp intensity, 0(low), 1(high)
	UINT8 edge_intensity;		///< edge intensity, 0(low) ~ 2(high)
}
LX_PE_SHP_EDGE_T;

/**
 *	pe resolution enhancement module on/off parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT32 enable; 			///< resolution enhancement on/off, 1:on, 0:off
}
LX_PE_SHP_RES_ENABLE_T;

/**
 *	scaler filter coefficient control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;					///< window id
	LX_PE_SCL_FILTERTYPE hor_y_tap_type;	///< scaler filter horizontal luma tap type
	LX_PE_SCL_FILTERTYPE hor_c_tap_type;	///< scaler filter horizontal chroma tap type
	LX_PE_SCL_FILTERTYPE ver_y_tap_type;	///< scaler filter vertical luma tap type
	LX_PE_SCL_FILTERTYPE ver_c_tap_type;	///< scaler filter vertical chroma tap type
	LX_PE_SCL_FIRTYPE fir_luma_type;		///< fir type, luma
	LX_PE_SCL_FIRTYPE fir_chroma_type;		///< fir type, chroma
	UINT32 hor_luma_id;						///< horizontal luma index
	UINT32 ver_luma_id;						///< vertical luma index
	UINT32 hor_chroma_id;					///< horizontal chroma index
	UINT32 ver_chroma_id;					///< vertical chroma index
}
LX_PE_SHP_SCLFILTER_T;

/**
 *	pe resolution enhancement(RE) normal control(ver.2) parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT8 mp_white_gain;	///< white gain
	UINT8 mp_black_gain;	///< black gain
	UINT8 sp_white_gain;	///< white gain
	UINT8 sp_black_gain;	///< black gain
	/* cti */
	UINT8 cti_en;			///< cti enable
	UINT8 tap_size;			///< "000" 21 ta
	UINT8 cti_gain;			///< gain (3.5u)
	UINT8 ycm_y_gain;		///< ycm_y_gain
	UINT8 ycm_c_gain;		///< ycm_c_gain
}
LX_PE_SHP_RE_CMN_T;

/**
 *	pe resolution enhancement(RE) horizontal control(ver.2) parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;			///< window id
	UINT8 edge_y_filter_en;			///< edge_Y_filter_en
	UINT8 reg_csft_gain;			///< reg_csft_gain: center shift gain(1.5u)
	UINT8 edge_filter_white_gain;	///< edge filter white gain
	UINT8 edge_filter_black_gain;	///< edge filter black gain
	UINT8 a_gen_width;				///< a_gen_widt
	UINT8 mp_horizontal_gain;		///< horizontal gain(3.5u)
	UINT8 sp_horizontal_gain;		///< horizontal gain(3.5u)
}
LX_PE_SHP_RE_HOR_T;

/**
 *	pe resolution enhancement(RE) vertical control(ver.2) parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT8 der_v_en;			///< der-v enable
	UINT8 bif_manual_th;	///< bilateral filter threshold
	UINT8 csft_gain;		///< center-shift gain (1.5u)
	UINT8 gain_b;			///< gain(b) : 2.5u
	UINT8 gain_w;			///< gain(w): 2.5u
	UINT8 der_gain_mapping;	///< weight selection for weighted average of bif and inpu
	UINT8 mmd_sel;			///< a-gen. siz
	UINT8 mp_vertical_gain;	///< vertical gain(3.5u)
	UINT8 sp_vertical_gain;	///< vertical gain(3.5u)
}
LX_PE_SHP_RE_VER_T;

/**
 *	pe resolution enhancement(RE) misc. control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	/* e-map */
	UINT8 amap2_sel;			///< A-map generation
	UINT8 ga_max;				///< A to Edge weight control
	UINT8 ga_th0;				///< th0
	UINT8 ga_th1;				///< th1
	/* t-map */
	UINT8 amap1_sel;			///< A-map generatio for texture enahcer
	UINT8 max_sel;				///< A-map expansion for texture-enhance
	UINT8 tmap_gain;			///< T-to-texture weight
	UINT8 gt_th0;				///< th0
	UINT8 gt_th0a;				///< th0a
	UINT8 gt_th0b;				///< th0b
	UINT8 gt_th1;				///< th1
	UINT8 gt_gain0a;			///< gain(th0a) : 1.5u
	UINT8 gt_gain0b;			///< gain(th0b) : 1.5u
	UINT8 gt_max;				///< gain(th1) :1.5u
	UINT8 coring_mode1;			///< t-map(H/V) coring mode
	UINT16 var_th;				///< flat region rejection threshold,11bit
	/* d-jag */
	UINT8 center_blur_en;		///< center-pixel averaging for edge-direction calculatio
	UINT8 level_th;				///< g0: level threshold
	UINT8 protect_th;			///< G1: protect threshold
	UINT8 n_avg_gain;			///< neighborhood pixel averaing : gain
}
LX_PE_SHP_RE_MISC_T;

/**
 *	pe edge enhancement(EE) control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;	///< window id
	UINT8 mp_edge_gain_b;	///< MP: edge gain(b) : 2.5u
	UINT8 mp_edge_gain_w;	///< MP: edge gain(w)
	UINT8 sp_edge_gain_b;	///< SP: edge gain(b) : 2.5u
	UINT8 sp_edge_gain_w;	///< SP: edge gain(w)
}
LX_PE_SHP_EE_T;

/**
 *	pe detail enhancement(DE) control parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	UINT8 mp_texture_gain_b;	///< MP: texture gain(b)
	UINT8 mp_texture_gain_w;	///< MP: texture gain(w) 
	UINT8 sp_texture_gain_b;	///< SP: texture gain(b)
	UINT8 sp_texture_gain_w;	///< SP: texture gain(w) 
}
LX_PE_SHP_DE_T;

/**
 *	pe d3d threshold control parameter type
 */
typedef struct {
	BOOLEAN isDefault;       ///< Is default setting ?
	UINT32 tpd_thr;          ///< for test pattern
	UINT32 ptn_thr;          ///< for cb/cc/ll 
	UINT32 mtn_thr_2d;       ///< for motion check (2D)
	UINT32 mtn_thr_3d;       ///< for motion check	(3D)
	UINT32 fmt_th1;          ///< th1 in algorothm
	UINT32 fmt_th2;          ///< th2 in algorothm 
	UINT32 fmt_th3;	         ///< th3 in algorothm
	UINT32 fmt_th4;          ///< th4 in algorothm
	UINT32 fmt_th5;          ///< th5 in algorothm
	UINT32 fmt_th6;          ///< th6 in algorothm	
	UINT32 fmt_th7;          ///< th7 in algorothm		
	UINT32 fmt_th8;          ///< th8 in algorothm			
	UINT32 fmt_th9;          ///< th9 in algorothm			
	UINT32 fmt_th10;          ///< th10 in algorothm				
	UINT32 skp_th1;          ///< initial delay for 3D->2D detection (60Hz domain)
	UINT32 skp_th2;          ///< period(ticks) for 3D->2D detection (60Hz domain)
	UINT32 vld_thr;          ///< size of buffer for valid cahek (2D->3D, 3D->2D)
	UINT32 mbv_thr;          ///< hist_max_bin_val
	UINT32 dfs_thr_2d;       ///< hist_diff_sum (2D)
	UINT32 dfs_thr_3d;       ///< hist_diff_sum (3D)
} LX_PE_D3D_THR_T;

/**
 *	pe d3d frame capture info type
 */
typedef struct {
	LX_PE_D3D_CAP_T cap_enb;
	UINT32 h_line_num;
	UINT32 v_line_num;
	UINT32 h_line_width;
	UINT32 v_line_width;
} LX_PE_D3D_CAP_INFO_T;

/**
 *	pe d3d setting control parameter type
 */
typedef struct
{
	LX_PE_D3D_DEC_T enb;
	UINT32 sel_alg;
	LX_PE_D3D_CAP_INFO_T cap_info;
	LX_PE_D3D_FORMAT_T fmt;
	LX_PE_D3D_THR_T thr;
	UINT16	*sbs_disparity;
	UINT16	*tnb_disparity;
} LX_PE_D3D_SETTINGS_T;

/**
 *	pe d3d information control parameter type
 */
typedef struct
{
	BOOLEAN valid;
	LX_PE_D3D_FORMAT_T d3d_inf;
	LX_PE_D3D_THR_T thr;
} LX_PE_D3D_INFO_T;

/**
 *	pe dual screen information control parameter type
 */
typedef struct
{
	LX_PE_D3D_DSC_T dsc_type;
} LX_PE_D3D_DSC_INFO_T;

/*----------------------------------------------------------------------------------------
	Extern Variables
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

#endif /* _PE_KAPI_H_ */
