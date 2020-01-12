
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

/** @file
 *
 *  application interface header for vdec device
 *
 *  author		seokjoo.lee (seokjoo.lee@lge.com)
 *  version		0.75
 *  date		2010.07.26
 *
 *  @addtogroup lg1150_vdec
 *	@{
 */

#ifndef	_VDEC_KAPI_H_
#define	_VDEC_KAPI_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
//#define VDEC_KDRV_FP_DTS_MATCHED	1
#undef	VDEC_REDUCE_FLUSH		// 20110318 DVR-VDEC PAUSE¡Æu¡¤A Tuning
//#define VDEC_IO_RATE_DELETED

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
#define	VDEC_KDRV_IOC_MAGIC		'v'

/** maximum video decoder context */
#define	VDEC_KDRV_CH_MAXN			8


typedef enum
{
	VDEC_KDRV_DBGCMD_MOD_TOP = 0,
	VDEC_KDRV_DBGCMD_MOD_PDEC,
	VDEC_KDRV_DBGCMD_MOD_FEED,
	VDEC_KDRV_DBGCMD_MOD_MSVC,
	VDEC_KDRV_DBGCMD_MOD_SYNC,
	VDEC_KDRV_DBGCMD_MOD_MIN = VDEC_KDRV_DBGCMD_MOD_TOP,
	VDEC_KDRV_DBGCMD_MOD_MAX = VDEC_KDRV_DBGCMD_MOD_SYNC,
	VDEC_KDRV_DBGCMD_MOD_MAXN = (VDEC_KDRV_DBGCMD_MOD_MAX+1),
} VDEC_KDRV_DBGCMD_MOD_T;

typedef enum
{
	VDEC_KDRV_DBGCMD_RESET = 0,
	VDEC_KDRV_DBGCMD_MCU_RESET,
	VDEC_KDRV_DBGCMD_TOP_MIN = VDEC_KDRV_DBGCMD_RESET,
	VDEC_KDRV_DBGCMD_TOP_MAX = VDEC_KDRV_DBGCMD_MCU_RESET,
	VDEC_KDRV_DBGCMD_TOP_MAXN = (VDEC_KDRV_DBGCMD_TOP_MAX+1),
} VDEC_KDRV_DBGCMD_TOP_TYPE_T;

typedef enum
{
	VDEC_KDRV_DBGCMD_SYNC_DISPLAY_OFFSET = 0,
	VDEC_KDRV_DBGCMD_SYNC_1,
	VDEC_KDRV_DBGCMD_SYNC_MIN = VDEC_KDRV_DBGCMD_SYNC_DISPLAY_OFFSET,
	VDEC_KDRV_DBGCMD_SYNC_MAX = VDEC_KDRV_DBGCMD_SYNC_1,
	VDEC_KDRV_DBGCMD_SYNC_MAXN = (VDEC_KDRV_DBGCMD_SYNC_MAX+1),
} VDEC_KDRV_DBGCMD_SYNC_TYPE_T;

/**
Video Codec Type.
	decoding formats supported by the Video decoder.
*/
typedef enum
{
	VDEC_KDRV_VCODEC_H264_HP = 0,
	VDEC_KDRV_VCODEC_H264_MVC,
	VDEC_KDRV_VCODEC_VC1_RCV_V1,
	VDEC_KDRV_VCODEC_VC1_RCV_V2,
	VDEC_KDRV_VCODEC_VC1_ES,
	VDEC_KDRV_VCODEC_MPEG2_HP,
	VDEC_KDRV_VCODEC_MPEG4_ASP,
	VDEC_KDRV_VCODEC_XVID,
	VDEC_KDRV_VCODEC_DIVX3,
	VDEC_KDRV_VCODEC_DIVX4,
	VDEC_KDRV_VCODEC_DIVX5,
	VDEC_KDRV_VCODEC_RVX,
	VDEC_KDRV_VCODEC_AVS,
	VDEC_KDRV_VCODEC_VP8,

	VDEC_KDRV_VCODEC_MIN = VDEC_KDRV_VCODEC_H264_HP,	///< min marker for allowed codec for VDEC_KDRV_VDEC_OP_MODE_TS.
	VDEC_KDRV_VCODEC_MAX = VDEC_KDRV_VCODEC_VP8,		///< max marker for allowed codec for VDEC_KDRV_VDEC_OP_MODE_TS.
	VDEC_KDRV_VCODEC_INVALID
} VDEC_KDRV_VCODEC_T;

/**
@var VDEC_KDRV_SRC_SDEC0
Video decoder input can be selected from system decoder 0.

@var VDEC_KDRV_SRC_SDEC1
Video decoder input can be selected from system decoder 1.

@var VDEC_KDRV_SRC_BUFF
Video decoder input is manually controlled by caller.
*/
typedef enum
{
	VDEC_KDRV_SRC_SDEC0		= 0,					// Video decoder input can be selected from system decoder 0.
	VDEC_KDRV_SRC_SDEC1		= 1,					// Video decoder input can be selected from system decoder 1.
	VDEC_KDRV_SRC_SDEC2		= 2,					// Video decoder input can be selected from system decoder 1.
	VDEC_KDRV_SRC_TVP		= 3,					// Video decoder input is manually controlled by caller.
	VDEC_KDRV_SRC_BUFF0		= 4,					// Video decoder input is manually controlled by caller.
	VDEC_KDRV_SRC_BUFF1		= 5,					// Video decoder input is manually controlled by caller.
	VDEC_KDRV_SRC_GRP_BUFF0	= 6,
	VDEC_KDRV_SRC_GRP_BUFF1	= 7,
	VDEC_KDRV_SRC_MIN		= VDEC_KDRV_SRC_SDEC0,
	VDEC_KDRV_SRC_MAX		= VDEC_KDRV_SRC_GRP_BUFF1,
	VDEC_KDRV_SRC_MAXN	= (VDEC_KDRV_SRC_MAX+1),
	VDEC_KDRV_SRC_INVALID
} VDEC_KDRV_SRC_T;

/**
@var VDEC_KDRV_DST_DE0
Video decoder output shall be directed to Main Window of DE(Display Engine)
*/

/**
@var VDEC_KDRV_DST_DE1
Video decoder output shall be directed to Sub Window of DE(Display Engine)
*/

/**
@var VDEC_KDRV_DST_BUFF
Video decoder output shall be remains to DPB (Decoded Picture Buffer), not automatically passed to DE.
*/
typedef enum
{
	VDEC_KDRV_DST_DE0		= 0,					// Video decoder output shall be directed to Main Window of DE(Display Engine)
	VDEC_KDRV_DST_DE1		= 1,					// Video decoder output shall be directed to Sub Window of DE(Display Engine)
	VDEC_KDRV_DST_BUFF		= 2,
	VDEC_KDRV_DST_GRP_BUFF0	= 3,
	VDEC_KDRV_DST_GRP_BUFF1	= 4,
	VDEC_KDRV_DST_MIN		= VDEC_KDRV_DST_DE0,
	VDEC_KDRV_DST_MAX		= VDEC_KDRV_DST_GRP_BUFF1,
	VDEC_KDRV_DST_MAXN	= (VDEC_KDRV_DST_MAX+1),
	VDEC_KDRV_DST_INVALID
} VDEC_KDRV_DST_T;


/**
@var VDEC_KDRV_OPMOD_NORMAL
Video decoder will be operated on the broadcast/DVR/file play configuration
*/

/**
@var VDEC_KDRV_OPMOD_ONE_FRAME
Video decoder will be operated on the channel browser , thumbnail and drip decoding configuration
*/
typedef enum
{
	VDEC_KDRV_OPMOD_NORMAL = 0,
	VDEC_KDRV_OPMOD_ONE_FRAME,
	VDEC_KDRV_OPMOD_DUAL,
	VDEC_KDRV_OPMOD_GRAPHIC_BUFF,
	VDEC_KDRV_OPMOD_MIN = VDEC_KDRV_OPMOD_NORMAL,
	VDEC_KDRV_OPMOD_MAX = VDEC_KDRV_OPMOD_GRAPHIC_BUFF,
	VDEC_KDRV_OPMOD_MAXN = (VDEC_KDRV_OPMOD_MAX+1),
	VDEC_KDRV_OPMOD_INVALID
} VDEC_KDRV_OPMODE_T;

/**
@name VDEC IOCTL List
ioctl list for vdec device.

@{
@def VDEC_KDRV_IO_OPEN_CH
Initialize vdec channel with codec/data type
VDEC module shall initialize itself with pre-defined configuration.
@li register device, interrupts, buffer memory.

@param VDEC_KDRV_IO_OPEN_CH* [IN] per-channel initialization parameter.

@def VDEC_KDRV_IO_CLOSE_CH
closes given VDEC Channel.
reduce CPU burden when given channel shall not be used by closing vdec channel.
TBD: what to do if decoding is not completed or given channel is not responding
@param UINT8	[IN] channel id to close. must be "0<= param < VDEC_KDRV_CH_MAXN"

@def VDEC_KDRV_IO_RESET
Reset/Flush vdec module for all channel or per-channel.
When vdec device receives above IOCTL, then reset itself.

@param UITN32	[IN]	one of @ref ESoftReset

@def VDEC_KDRV_IO_SET_BUFFER_LVL
sets buffer overflow/underflow level.

@param VDEC_KDRV_IO_BUFFER_LVL_T* [IN]

@def VDEC_KDRV_IO_GET_BUFFER_LVL
reads buffer almost overflow(aFull)/underflow(aEmpty) level(in bytes).

@param VDEC_KDRV_IO_BUFFER_LVL_T* [IN] aFull

@def VDEC_KDRV_IO_PLAY
Starts Decoding with given VDEC CH.
with ioctl paramter, @ref VDEC_KDRV_IO_PLAY_T.
When Start decoding, initially Lip Sync mode shall be Off.
@param VDEC_KDRV_IO_PLAY* [IN] see typical value below.

Usual VDEC_KDRV_IO_PLAY_T value.

for normal transport stream case.
- @ref VDEC_KDRV_IO_PLAY_T.syncOn = 1 (automatically Lip Sync On after first valid PTS/DTS found.)
- @ref VDEC_KDRV_IO_PLAY_T.picScanMode = VDEC_KDRV_PIC_SCAN_MSK_ALL
- @ref VDEC_KDRV_IO_PLAY_T.step = 0
- @ref VDEC_KDRV_IO_PLAY_T.userDataEn = 1
- @ref VDEC_KDRV_IO_PLAY_T.resetStatus shall be: \n
	1 : reset saved information) when channel/source change.\n
	0 : when error recovery if required. within same channel.

for File play or streaming play mode.
[NOTE] File play shall use GSTC for lipsync.
- @ref VDEC_KDRV_IO_PLAY_T.syncOn = 0
- @ref VDEC_KDRV_IO_PLAY_T.picScanMode = VDEC_KDRV_PIC_SCAN_MSK_ALL
- @ref VDEC_KDRV_IO_PLAY_T.step = 0
- @ref VDEC_KDRV_IO_PLAY_T.userDataEn = 1 or 0
- @ref VDEC_KDRV_IO_PLAY_T.resetStatus shall be: \n
	1 : reset saved information) when channel/source change.\n
	0 : when error recovery if required or pause/resume case.

for JPEG or MPEG I Frame decoding.
- @ref VDEC_KDRV_IO_PLAY_T.syncOn = 0
- @ref VDEC_KDRV_IO_PLAY_T.picScanMode = VDEC_KDRV_PIC_SCAN_MSK_I
- @ref VDEC_KDRV_IO_PLAY_T.step = 1
- @ref VDEC_KDRV_IO_PLAY_T.userDataEn = 1 or 0
- @ref VDEC_KDRV_IO_PLAY_T.resetStatus = 1

@def VDEC_KDRV_IO_STOP
Stops Decoding with given VDEC CH.
@param UINT32 channel to stop.
this will cause all operation of VDEC stopped for given channel.
TODO : what is difference between Stop & pause
TODO : what is difference between SET_SPEED(0)

@def VDEC_KDRV_IO_RATE	OBSOLETE 20110416 seokjoo.lee moved VDEC_KDRV_IO_PLAY_T
Set/Get Decoding Rate to nominal rate.

Stops Decoding with given VDEC CH.
@param VDEC_KDRV_IO_RATE_T
this will cause all operation of VDEC stopped for given channel.
TODO : what is difference between Stop & pause
TODO : what is difference between SET_SPEED(0)

@def VDEC_KDRV_IO_SET_LIP_SYNC
enable/disable Lip sync mode.
controls lip sync mode of given VDEC Channel.
only allowed for channel 0 and 1.
@param UINT32	[IN] 0 for free run, 1 for synchronized(PTS check).

@def VDEC_KDRV_IO_GET_LIP_SYNC
get current enable/disable state of Lip sync mode.
controls lip sync mode of given VDEC Channel.
only allowed for channel 0 and 1.

@def VDEC_KDRV_IO_UPDATE_BUFFER
trigers VDEC input buffer read pointer to be updated.
shall be used only when buffered/file play  mode.
when Updating Video Input buffer, some decoding micro command SHOULD Be attached.
[NOTE] User application SHOULD check command queue availibility via VDEC_KDRV_IO_GET_BUFFER_LVL,
before calling VDEC_KDRV_IO_UPDATE_BUFFER with decoding command to command queue.


@def VDEC_KDRV_IO_SET_FRAME
set number of frames for given channel.
@param UINT32 number of frames.

@def VDEC_KDRV_IO_GET_OUTPUT
TBD: get decoded output frame information.
TBD: SIC code only, not implemented.\n
- TBD: return value read from register: ch[0~3]_frame_stride,ch[0~3]_frame_width_height \n
- TBD: also used to FILEPLAY_JPEGINFO FILEPLAY_JPEGGETINFO ioctl in Verification source code.

@def VDEC_KDRV_IO_GET_TIME
returns GSTC (Global STC) value(28bit) which runs monotonic increasing 28 bit unsigned value @ 90KHz.
if negative, some error happened.

@def VDEC_KDRV_IO_GET_STATUS
Read status of VDEC channel.
control codec_type, speed, and additional parameter.\n
TBD: if a status structure is allocated in kernel driver and mmap'ed to user space,\n
then this IOR ioctl can be implemented as reading member variable from mmap'ed status structure.

@def VDEC_KDRV_IO_SET_NOTIFY
enable/disable notify events.
such as Sequence header, Picture header, User data, AFD/Bar data.

@def VDEC_KDRV_IO_GET_NOTIFY
enable/disable notify events.
such as Sequence header, Picture header, User data, AFD/Bar data.

@def VDEC_KDRV_IO_GET_VERSION
Read version.
version of device driver, firmware version, H/W revision info.
- Kernel level device driver read this information from H/W register when opening device driver and user space application only read this.
- TBD: if status structure is mmap'ed, then simple read version info from mmap'ed address of status structure.

@def VDEC_KDRV_IO_SET_REG
debugging : write each register.
TBD:

@def VDEC_KDRV_IO_GET_REG
debugging : read each register.
TBD:

*/
/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
// when opening device driver.
//#define VDEC_KDRV_IOR_CFG				_IOR(VDEC_IOC_MAGIC, 'Z', CHIP_REV_INFO_T* )
//#define VDEC_KDRV_IOW_CFG				_IOW(VDEC_IOC_MAGIC, 'Z', CHIP_REV_INFO_T* )

// per channel operation.
//           +- common prefix. -> shall be removed when dump.
//      |<---+--->|
//		0123456789012345678901234
#define VDEC_KDRV_IO_OPEN_CH			_IOWR(VDEC_KDRV_IOC_MAGIC,  0, VDEC_KDRV_IO_OPEN_CH_T)
#define VDEC_KDRV_IO_CLOSE_CH			_IO(VDEC_KDRV_IOC_MAGIC,  1)

// decoding control
#define VDEC_KDRV_IO_PLAY				_IOWR(VDEC_KDRV_IOC_MAGIC,  2, VDEC_KDRV_IO_PLAY_T)
#define VDEC_KDRV_IO_PLAY_SET			_IOWR(VDEC_KDRV_IOC_MAGIC,  3, VDEC_KDRV_IO_PLAY_SET_T)
#define VDEC_KDRV_IO_PLAY_GET			_IOWR(VDEC_KDRV_IOC_MAGIC,  4, VDEC_KDRV_IO_PLAY_GET_T)

#define VDEC_KDRV_IO_FLUSH				_IO(VDEC_KDRV_IOC_MAGIC,  5)

#define VDEC_KDRV_IO_SET_BUFFER_LVL		_IOWR(VDEC_KDRV_IOC_MAGIC,  6, VDEC_KDRV_IO_BUFFER_LVL_T)
#define VDEC_KDRV_IO_GET_BUFFER_STATUS		_IOWR(VDEC_KDRV_IOC_MAGIC,  7, VDEC_KDRV_IO_BUFFER_STATUS_T)

// for buffered/file play only
#define VDEC_KDRV_IO_UPDATE_BUFFER		_IOWR(VDEC_KDRV_IOC_MAGIC,  8, VDEC_KDRV_IO_UPDATE_BUFFER_T)

// aux data gathering.
#define VDEC_KDRV_IO_GET_OUTPUT			_IOWR(VDEC_KDRV_IOC_MAGIC,  9, VDEC_KDRV_OUTPUT_T)

// misc.
#define VDEC_KDRV_IO_GET_VERSION		_IOWR(VDEC_KDRV_IOC_MAGIC,  10, VDEC_KDRV_IO_VERSION_T)

// Frame buffer (DPB)
#define VDEC_KDRV_IO_UPDATE_GRAPBUF		_IOWR(VDEC_KDRV_IOC_MAGIC,  11, VDEC_KDRV_IO_UPDATE_GRAPHIC_BUFFER_T)

// debugging.
#define VDEC_KDRV_IO_SET_REG			_IOWR(VDEC_KDRV_IOC_MAGIC,  12, VDEC_KDRV_SET_REG_T)
#define VDEC_KDRV_IO_GET_REG			_IOWR(VDEC_KDRV_IOC_MAGIC,  13, VDEC_KDRV_GET_REG_T)

#define VDEC_KDRV_IO_ENABLELOG			_IOWR(VDEC_KDRV_IOC_MAGIC,  14, UINT32)
#define VDEC_KDRV_IO_DBG_CMD			_IOWR(VDEC_KDRV_IOC_MAGIC,  15, VDEC_KDRV_DBG_CMD_T)

#define VDEC_KDRV_IO_GET_MEM_LOG		_IOWR(VDEC_KDRV_IOC_MAGIC,  16, VDEC_KDRV_GET_MEM_LOG_T)

#define VDEC_KDRV_IO_OPEN_PARAM			_IOWR(VDEC_KDRV_IOC_MAGIC,  17, VDEC_KDRV_IO_OPEN_PARAM_T)
#define VDEC_KDRV_IO_CLOSE_PARAM		_IOWR(VDEC_KDRV_IOC_MAGIC,  18, VDEC_KDRV_IO_NULL_PARAM_T)
#define VDEC_KDRV_IO_PLAY_PARAM			_IOWR(VDEC_KDRV_IOC_MAGIC,  19, VDEC_KDRV_IO_PLAY_PARAM_T)
#define VDEC_KDRV_IO_FLUSH_BUFFER		_IOWR(VDEC_KDRV_IOC_MAGIC,  20, VDEC_KDRV_IO_NULL_PARAM_T)

#define VDEC_KDRV_IO_ENABLE_CALLBACK	_IOWR(VDEC_KDRV_IOC_MAGIC,  21, VDEC_KDRV_ENABLE_CALLBACK_T)
#define VDEC_KDRV_IO_CALLBACK_NOTI		_IOWR(VDEC_KDRV_IOC_MAGIC,  22, VDEC_KDRV_IO_CALLBACK_INFO_T)
#define VDEC_KDRV_IO_GET_DEC_SEQH		_IOWR(VDEC_KDRV_IOC_MAGIC,  23, VDEC_KDRV_IO_DEC_SEQH_OUTPUT_T)
#define VDEC_KDRV_IO_GET_DEC_PICD		_IOWR(VDEC_KDRV_IOC_MAGIC,  24, VDEC_KDRV_IO_DEC_PICD_OUTPUT_T)
#define VDEC_KDRV_IO_GET_DISPLAY		_IOWR(VDEC_KDRV_IOC_MAGIC,  25, VDEC_KDRV_IO_DISP_OUTPUT_T)
#define VDEC_KDRV_IO_GET_RUNNING_FB		_IOWR(VDEC_KDRV_IOC_MAGIC,  26, VDEC_KDRV_IO_FB_INFO_T)
#define VDEC_KDRV_IO_GET_LATEST_DEC_UID		_IOWR(VDEC_KDRV_IOC_MAGIC,  27, VDEC_KDRV_IO_GET_LATEST_UID_T)
#define VDEC_KDRV_IO_GET_LATEST_DISP_UID	_IOWR(VDEC_KDRV_IOC_MAGIC,  28, VDEC_KDRV_IO_GET_LATEST_UID_T)
#define VDEC_KDRV_IO_SET_BASETIME		_IOWR(VDEC_KDRV_IOC_MAGIC,  29, VDEC_KDRV_IO_BASETIME_T)
#define VDEC_KDRV_IO_GET_BASETIME		_IOWR(VDEC_KDRV_IOC_MAGIC,  30, VDEC_KDRV_IO_BASETIME_T)
#define VDEC_KDRV_IO_GET_DISPLAY_FPS		_IOWR(VDEC_KDRV_IOC_MAGIC,  31, VDEC_KDRV_IO_FRAMERATE_T)
#define VDEC_KDRV_IO_GET_DROP_FPS		_IOWR(VDEC_KDRV_IOC_MAGIC,  32, VDEC_KDRV_IO_FRAMERATE_T)
#define VDEC_KDRV_IO_CTRL_USRD			_IOWR(VDEC_KDRV_IOC_MAGIC,  33, VDEC_KDRV_IO_CTRL_USRD_T)
#define VDEC_KDRV_IO_SET_FREE_DPB			_IOWR(VDEC_KDRV_IOC_MAGIC,  34, VDEC_KDRV_IO_FREE_DPB_T)
#define VDEC_KDRV_IO_RECONFIG_DPB			_IOWR(VDEC_KDRV_IOC_MAGIC,  35, VDEC_KDRV_IO_NULL_PARAM_T)

#define VDEC_KDRV_IOC_MAXNR				(_IOC_NR(VDEC_KDRV_IO_SET_FREE_DPB) +1 )


/** @} */


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#if defined(_VDEC_KADP_C_) || defined( _VDEC_NOTI_C_)
#define VDEC_EXTERN
#else
#define VDEC_EXTERN	extern
#endif

#define VDEC_KDRV_CHECK_IF_INVALID (_v, _t, _expr)							\
		if ( ((_v) <  _t ## _MIN) || ( _t ## _MAX  < (_v)) ) _expr

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/**
Open Parameter for Per Channel.
	Intialization parameter for each VDEC Channel using VDEC_KDRV_IO_OPEN_CH
	following operation is done when issuing VDEC_KDRV_IO_OPEN_CH.

	* CODEC setting
	* INPUT setting.
	* OUTPUT setting.

for src and dst SHOULD BE properly set before call VDEC_KDRV_IO_OPEN_CH.
*/
typedef struct
{
	VDEC_KDRV_VCODEC_T	vcodec;
	VDEC_KDRV_SRC_T		src;
	VDEC_KDRV_DST_T		dst;
	VDEC_KDRV_OPMODE_T	opmode;

	UINT32	useGstc;
	UINT32	bufDelaySize;
	UINT32	disDelayOffset;
	UINT32	usrPA;
	UINT32	usrSz;
	BOOLEAN	noAdpStr;
} VDEC_KDRV_IO_OPEN_CH_T;

typedef struct
{
	UINT32 				ch;
	VDEC_KDRV_VCODEC_T 	vcodec;
	VDEC_KDRV_SRC_T		src;
	VDEC_KDRV_DST_T		dst;

	// KTOP Param
	BOOLEAN 			bNoAdpStr;
	BOOLEAN				bForThumbnail;
	// VES Param
	BOOLEAN 			bAlignedBuf;
	UINT32				ui32DecodeOffset_bytes;
	// VDC Param
	UINT32				ui32ErrorMBFilter;
#define VDEC_KDRV_WIDTH_INVALID	0x0
	UINT32				ui32Width;
#define VDEC_KDRV_HEIGHT_INVALID	0x0
	UINT32				ui32Height;
	// VDC & VDS Param
	BOOLEAN				bDualDecoding;
	BOOLEAN				bLowLatency;
	// VDS Param
	BOOLEAN 			bUseGstc;
	UINT32				ui32DisplayOffset_ms;
	UINT8				ui8ClkID;

	UINT8				ui8VesCh;
	UINT8				ui8VdcCh;
	UINT8				ui8VdsCh;
} VDEC_KDRV_IO_OPEN_PARAM_T;

typedef struct
{
	UINT32 				ch;
} VDEC_KDRV_IO_NULL_PARAM_T;

/**
Buffer Almost Full/Empty Notify Parameter for VDEC input buffer ( CPB )
	used VDEC_KDRV_IO_SET_BUFFER_LVL
*/
typedef struct
{
	UINT32	AemptyLvl;		///< Almost Empty level of VDEC input buffer (CPB)
	UINT32	AfullLvl;			/// < Almost Full level of VDEC input buffer (CPB)
} VDEC_KDRV_IO_BUFFER_LVL_T;

typedef struct
{
	UINT32	ch;
	UINT32	cpb_depth;
	UINT32	cpb_size;
	UINT32	auq_depth;
	UINT32	dq_depth;
} VDEC_KDRV_IO_BUFFER_STATUS_T;

/**
 Video Decoder Access Unit Type.
 For update buffer write pointer for which type of this chunk of memory contains.
 for VDEC_KDRV_IO_UPDATE_BUFFER_T::au_type

Only For VDEC_KDRV_OP_MODE_BUFF
*/
typedef enum
{
	VDEC_KDRV_VAU_SEQH = 1,	///< Sequence Header
	VDEC_KDRV_VAU_SEQE,		///< Sequence End.
	VDEC_KDRV_VAU_DATA,		///< Picture Data.
	VDEC_KDRV_VAU_EOS,
	VDEC_KDRV_VAU_UNKNOWN,
	VDEC_KDRV_VAU_UINT32 = 0x20130206,
} VDEC_KDRV_VAU_T;

/**
 For update buffer write pointer for which type of this chunk of memory contains.
[NOTE] au_size should be 512 bytes unit.

*/
typedef struct
{
	UINT32	ch;
	UINT32	ui32uID;
	UINT32	frRes;
	UINT32	frDiv;
	VDEC_KDRV_VAU_T	au_type;
	UINT32	au_ptr;
	UINT32	au_size;
#define	VDEC_UNKNOWN_TIMESTAMP			0xFFFFFFFFFFFFFFFFLL
	UINT64	ui64TimeStamp;
	BOOLEAN	is90kHzTick;
	UINT16	width;
	UINT16	height;
} VDEC_KDRV_IO_UPDATE_BUFFER_T;

/**
H/W and Firmware version information.
*/
typedef struct
{
	UINT32 ui32RTLVersion;
	UINT32 ui32FirmwareVersion;
} VDEC_KDRV_IO_VERSION_T;

/**
Init parameter for frame buffer(DPB) update (VDEC_KDRV_IO_UPDATE_FRAME_BUFFER)
*/
typedef struct
{
	UINT32	numOfFrameBuf;
	UINT32	width;
	UINT32	height;
	UINT32	h_Offset;
	UINT32	v_Offset;
} VDEC_KDRV_IO_INIT_FRAME_BUFFER_T;

/**
For update frame buffer(DPB) for which type of this chunk of memory contains.
*/
typedef struct
{
	UINT32	addrY;
	UINT32	addrC;
	UINT32	sizeY;
	UINT32	sizeC;
	UINT64	ui64TimeStamp;
} VDEC_KDRV_IO_UPDATE_FRAME_BUFFER_T;

typedef struct
{
	UINT32	ch;
	VDEC_KDRV_VAU_T	eDpbType;
	UINT32	ui32uID;
	UINT32	ui32FrameRateRes;
	UINT32	ui32FrameRateDiv;
	union
	{
		VDEC_KDRV_IO_INIT_FRAME_BUFFER_T strFrmHdr;
		VDEC_KDRV_IO_UPDATE_FRAME_BUFFER_T strFrmPic;
	} u;
} VDEC_KDRV_IO_UPDATE_FRAME_T;

typedef struct
{
	UINT32	ch;
	SINT32	si32FrameFd;
	UINT16	ui32BufStride;
	UINT16	ui32PicWidth;
	UINT16	ui32PicHeight;
	UINT64	ui64TimeStamp;
	BOOLEAN	is90kHzTick;
	UINT32	ui32FrameRateRes;
	UINT32	ui32FrameRateDiv;
	UINT32	ui32uID;
} VDEC_KDRV_IO_UPDATE_GRAPHIC_BUFFER_T;

/**
[DEBUG] Direct Register access ioctl parameter.
*/
typedef struct
{
 	UINT32 	addr;
 	UINT32 	value;
} VDEC_KDRV_SET_REG_T, VDEC_KDRV_GET_REG_T;

/**
[DEBUG] Direct memory log access ioctl parameter.
*/
typedef struct
{
	UINT32 mem_type;	//<<< VDEC_KDRV_MEM_BUFF_T
	UINT32 enable;		//<<< enable flag
	UINT32 size;		//<<< input : request size / output : actural read size
	UINT32 buff_ptr;	//<<< user buffer to contain memory log info.
} VDEC_KDRV_GET_MEM_LOG_T;

/**
[DEBUG] Debug command ioctl parameter.
*/
typedef struct
{
	UINT32 	ui32Module;
	UINT32	ui32CmdType;
	UINT32	ui32nSize;
	UINT32	ui32Data[4];
} VDEC_KDRV_DBG_CMD_T;

/**
Mask for enable/disable Notify.
used VDEC_KDRV_IO_PLAY_SET(notifyMask),

for VDEC_KDRV_OP_MODE_TS
*/
#define VDEC_KDRV_NOTIFY_ID_SEQH	 	0
#define VDEC_KDRV_NOTIFY_ID_PICD	 	1
#define VDEC_KDRV_NOTIFY_ID_USRD	 	2
#define VDEC_KDRV_NOTIFY_ID_DISP	 	3
#define VDEC_KDRV_NOTIFY_ID_ERR	 	4
#define VDEC_KDRV_NOTIFY_ID_AFULL 	5
#define VDEC_KDRV_NOTIFY_ID_AEMPTY	6
#define VDEC_KDRV_NOTIFY_ID_RTIMEOUT	7
#define VDEC_KDRV_NOTIFY_ID_MAXN		8

#define VDEC_KDRV_CB_ID_SEQH	 	VDEC_KDRV_NOTIFY_ID_SEQH
#define VDEC_KDRV_CB_ID_PICD	 	VDEC_KDRV_NOTIFY_ID_PICD
#define VDEC_KDRV_CB_ID_USRD	 	VDEC_KDRV_NOTIFY_ID_USRD
#define VDEC_KDRV_CB_ID_DISP	 	VDEC_KDRV_NOTIFY_ID_DISP
#define VDEC_KDRV_CB_ID_ERR	 	VDEC_KDRV_NOTIFY_ID_ERR
#define VDEC_KDRV_CB_ID_AFULL 	VDEC_KDRV_NOTIFY_ID_AFULL
#define VDEC_KDRV_CB_ID_AEMPTY	VDEC_KDRV_NOTIFY_ID_AEMPTY
#define VDEC_KDRV_CB_ID_MAXN		VDEC_KDRV_NOTIFY_ID_MAXN

typedef enum
{
	VDEC_KDRV_NOTIFY_MSK_NONE		= 0,
	VDEC_KDRV_NOTIFY_MSK_SEQH		= 1UL<< VDEC_KDRV_NOTIFY_ID_SEQH,		///< notify if got or changed SEQuence Header
	VDEC_KDRV_NOTIFY_MSK_PICD		= 1UL<< VDEC_KDRV_NOTIFY_ID_PICD,		///< notify if PICture  Decoded
	VDEC_KDRV_NOTIFY_MSK_USRD		= 1UL<< VDEC_KDRV_NOTIFY_ID_USRD,		///< notify if got Closed Caption & AFD
	VDEC_KDRV_NOTIFY_MSK_DISP		= 1UL<< VDEC_KDRV_NOTIFY_ID_DISP,		///< notify if PICture  Displayed
	VDEC_KDRV_NOTIFY_MSK_ERR		= 1UL<< VDEC_KDRV_NOTIFY_ID_ERR,		///< notify if got Error
	VDEC_KDRV_NOTIFY_MSK_AFULL	= 1UL<< VDEC_KDRV_NOTIFY_ID_AFULL,	///< notify if Almost Full.
	VDEC_KDRV_NOTIFY_MSK_AEMPTY	= 1UL<< VDEC_KDRV_NOTIFY_ID_AEMPTY,	///< notify if Almost Empty.
	VDEC_KDRV_NOTIFY_MSK_RTIMEOUT	= 1UL<< VDEC_KDRV_NOTIFY_ID_RTIMEOUT, ///< notify if nothing happened in timeout

	VDEC_KDRV_NOTIFY_MSK_ALL		= VDEC_KDRV_NOTIFY_MSK_SEQH
								| VDEC_KDRV_NOTIFY_MSK_PICD
								| VDEC_KDRV_NOTIFY_MSK_USRD
								| VDEC_KDRV_NOTIFY_MSK_DISP
								| VDEC_KDRV_NOTIFY_MSK_ERR
								| VDEC_KDRV_NOTIFY_MSK_AFULL
								| VDEC_KDRV_NOTIFY_MSK_AEMPTY
								| VDEC_KDRV_NOTIFY_MSK_RTIMEOUT,


	VDEC_KDRV_NOTIFY_MSK_ENDMARK	= 1UL<< VDEC_KDRV_NOTIFY_ID_MAXN,		///< notify mask End marker.
} VDEC_KDRV_NOTIFY_MSK_T;



// [debug] build a list of iocmd.
/**
 */

typedef enum
{
	VDEC_KDRV_STATUS_START = 1,
	VDEC_KDRV_STATUS_STOP
} VDEC_KDRV_STATUS_T;

/**
memory buffer type for memory log
*/
typedef enum
{
	VDEC_KDRV_MEM_BUFF_PDEC,
	VDEC_KDRV_MEM_BUFF_LQ,
	VDEC_KDRV_MEM_BUFF_MSVC_CMD,
	VDEC_KDRV_MEM_BUFF_MSVC_RSP,
	VDEC_KDRV_MEM_BUFF_DQ,
	VDEC_KDRV_MEM_BUFF_CPB,		//<<< For future use
	VDEC_KDRV_MEM_BUFF_DPB		//<<< For future use
} VDEC_KDRV_MEM_BUFF_T;

/**
Video Decoder OPeration Mode.
*/
typedef enum
{
	VDEC_KDRV_OP_MODE_TS,			///< decode PES from TE(Transport Engine)
	VDEC_KDRV_OP_MODE_BUFF,		///< decode PES from buffer or file.

	VDEC_KDRV_OP_MODE_MIN = VDEC_KDRV_OP_MODE_TS,
	VDEC_KDRV_OP_MODE_MAX = VDEC_KDRV_OP_MODE_BUFF,
} VDEC_KDRV_OP_MODE_T;

/**
scan mode of video decoder.
only specified picture type of input data, others shall be skipped.\n

@see VDEC_KDRV_IO_PLAY_SET_T
*/
typedef enum
{
	VDEC_KDRV_PIC_SCAN_ALL,			///< decode IPB frame.
	VDEC_KDRV_PIC_SCAN_I,			///< I picture only (PB skip)
	VDEC_KDRV_PIC_SCAN_IP,			///< IP picture only (B skip only)
	VDEC_KDRV_PIC_SCAN_INVALID,
	VDEC_KDRV_PIC_SCAN_MIN = VDEC_KDRV_PIC_SCAN_ALL,
	VDEC_KDRV_PIC_SCAN_MAX = VDEC_KDRV_PIC_SCAN_INVALID,
	VDEC_KDRV_PIC_SCAN_MAXN = (VDEC_KDRV_PIC_SCAN_MAX+1),
} VDEC_KDRV_PIC_SCAN_T;

/**
video decoder sync operation.

@see VDEC_KDRV_IO_PLAY_SET_T
*/
typedef enum
{
	VDEC_KDRV_SYNC_OFF,			///< PTS/DTS match off.
	VDEC_KDRV_SYNC_ON,			///< PTS/DTS match on.
	VDEC_KDRV_SYNC_INVALID,
	VDEC_KDRV_SYNC_MIN = VDEC_KDRV_SYNC_OFF,
	VDEC_KDRV_SYNC_MAX = VDEC_KDRV_SYNC_INVALID,
	VDEC_KDRV_SYNC_MAXN = (VDEC_KDRV_SYNC_MAX+1),
} VDEC_KDRV_SYNC_T;

/**
display freeze state.

@see VDEC_KDRV_IO_PLAY_SET_T
*/
typedef enum
{
	VDEC_KDRV_FREEZE_UNSET,			///< freeze unset.
	VDEC_KDRV_FREEZE_SET,			///< freeze set.
	VDEC_KDRV_FREEZE_MIN = VDEC_KDRV_FREEZE_UNSET,
	VDEC_KDRV_FREEZE_MAX = VDEC_KDRV_FREEZE_SET,
	VDEC_KDRV_FREEZE_MAXN = (VDEC_KDRV_FREEZE_MAX+1),
} VDEC_KDRV_FREEZE_T;

/**
Parameter for VDEC_KDRV_IO_PLAY_SET

 */
typedef struct
{
	UINT32	picScanMode;		///< picture scan mode.	@ref VDEC_KDRV_PIC_SCAN_T,useful when trick mode.
	UINT32	srcScanType;		///< scanned source type.	@ref VDEC_KDRV_PIC_SCAN_T,useful when trick mode.
	UINT32	syncOn;				///< PTS/DTS match on. @ref VDEC_KDRV_SYNC_T
	UINT32	freeze;				///< display the last frame but decoding is going on. @ref VDEC_KDRV_FREEZE_T
	UINT32	notifyMask;			///< @ref VDEC_KDRV_NOTIFY_MSK_T
} VDEC_KDRV_IO_PLAY_SET_T;

/**
Parameter for VDEC_KDRV_IO_PLAY_GET

 */
typedef struct
{
	UINT8	clkID;
	UINT32	stcBaseTime;
	UINT32	ptsBaseTime;
} VDEC_KDRV_IO_PLAY_GET_T;

/**
common frame types
fr_type  : output frame struct type.
fr_format: pixel format of frame. shared LX_GFX_SURFACE_TYPE_T and VDEC_KDRV_FRAME_T
index    : index of frame [0~31] within decoded frame array.
width    : width in pixel.
height   : height in pixel.
stride   : stride in bytes.
pY       : physical start address of Luminance in bytes.

struct size : 0x10
*/

#define LX_FRAME_T				\
	UINT32	fr_type		:8,		\
			fr_format	:8,		\
			index		:8,		\
			_reserved	:8;		\
	UINT16	width;				\
	UINT16	height;				\
	UINT32	stride;				\
	void	*pY;

/**
Decoded VDEC Frame structure (common Video(MPEG/AVC/...) & JPEG).
@note This structure is basically shared LX_GFX_SURFACE_SETTINS_T and VDEC_KDRV_VENC_FRAME_T.
@ref VDEC_KDRV_NOTIFY_MSK_PICD VDEC_KDRV_IO_GET_OUTPUT
*/
typedef struct
{
#if 1
								// 0x00
	UINT16	width;			///< width in pixel.
	UINT16	height;			///< height in pixel.

	UINT32	stride;			///< stride in bytes.
	void	*pY;			///< physical start address of Luminance in bytes.

							// 0x0C
#else
							// 0x00
	LX_FRAME_T
#endif
							// starts of private part of VDEC Frame.
	void	*pCb;			///< physical start address of Chrominance in bytes.(if exists, or same as pY)
							// 0x10
	void	*pCr;			///< physical start address of Chrominance in bytes.(if exists, or same as pY)
							// 0x14

} VDEC_KDRV_FRAME_T;

/**
Decoded image information for Video(MPEG/AVC/DIVX/RV).
used for MPEG2&4/RV/AVC streams.
*/
typedef struct
{
								//	0x00
	UINT32	picType;			///< picture type of decoded image one of {'I', 'P', 'B', 'N'} ('N'for invalid)
	UINT32	interlSeq;			///< interlaced sequence or not.
	UINT32	interlFrm;			///< interlaced frame or not.
	UINT32	avc3DFpaValid;		///< frame packing arrangement cancel flag (AVC SEI info) : cancel flag 0 -> TRUE, cancel flag 1 -> FALSE
	UINT32	avc3DFpaType;		///< frame packing arrangement type (AVC SEI info)
	UINT32	afd;					///< Active Format Descriptor.

								//	0x18
								// 	 FrameRate = frameRateRes/ frameRateDiv
	UINT32	frameRateRes;		///< numerator of frame rate.
	UINT32	frameRateDiv;		///< denominator of frame rate - 1. (0 means denominator is 1)

								//	0x20
	UINT32	sar;				///< Source Aspect Ratio. if [31:16] == 0 then, translated by codec type.
								//	0x24
	UINT16	par_w;				///< Pixel Aspect Ratio.: width.
	UINT16	par_h;

								//  0x28
	UINT32	crop_rect[4];		///< Crop Rectangle, 0:Top, 1:Bottom, 2:Left, 3 Right.
								//  0x38
} VDEC_KDRV_MPEG_OUTPUT_T;

typedef struct
{
	UINT32	ch;
	BOOLEAN	bInputNoti;
	BOOLEAN	bFeedNoti;
	BOOLEAN	bDecodeNoti;
	BOOLEAN	bDisplayNoti;
} VDEC_KDRV_ENABLE_CALLBACK_T;

// will be move to vdec_noti.h
typedef struct
{
 	BOOLEAN	bRet;
	UINT32	ui32AuType;
	UINT32	ui32uID;
	UINT32	ui32DTS;
	UINT32	ui32PTS;
	UINT64	ui64TimeStamp;
	UINT32	ui32AuqOccupancy;
	UINT32	ui32CpbOccupancy;
	UINT32	ui32CpbSize;

} VDEC_KDRV_NOTIFY_PARAM_ES_T;

typedef struct
{
	UINT32	ui32Profile;
	UINT32	ui32Level;
	BOOLEAN	bGMC;		///< [VCODEC_H264 only] GMC (Global Motion Compensation) of SPS
	BOOLEAN	bFailed;

	UINT32	ui32PicWidth;
	UINT32	ui32PicHeight;
	UINT32	ui32RefFrameCount;

	UINT32	ui32uID;
} VDEC_KDRV_NOTIFY_PARAM_SEQH_T;

typedef struct
{
	UINT32	ch;
	VDEC_KDRV_NOTIFY_PARAM_SEQH_T	seqh;
} VDEC_KDRV_IO_DEC_SEQH_OUTPUT_T;

// will be move to vdec_noti.h
typedef struct
{
	UINT32	picType;		///< picture type of decoded image associated with this user data.
								///< shall be one of {0 for I frame, 1 for P frame, 2 for B frame }
	UINT32	rpt_ff;			///< repeat first field.
	UINT32	top_ff;			///< top    field first.
	UINT32	tmp_ref;		///< temporal reference.
	UINT32	size;			///< size in bytes.
	UINT32	offset;				///< current received Physical Address from cpb_base
								///< for valid user level address = usrVA + (offset - usrPA)
} VDEC_KDRV_NOTIFY_PARAM_USRD_T;

// will be move to vdec_noti.h
typedef struct
{
	UINT32	bGotAnchor;
	UINT32	bFail;
	UINT32	picErr;
	UINT32	bLowDelay;

	struct
	{
		UINT32 ui32DTS;
		UINT64 ui64TimeStamp;
		UINT32 ui32uID;
	} sDTS;
	struct
	{
		UINT32 ui32PTS;
		UINT64 ui64TimeStamp;
		UINT32 ui32uID;
	} sPTS;

	UINT32	ui32DqOccupancy;

	UINT32	ui32BufStride;
	UINT32	ui32PicWidth;
	UINT32	ui32PicHeight;
	UINT32	ui32CropLeftOffset;
	UINT32	ui32CropRightOffset;
	UINT32	ui32CropTopOffset;
	UINT32	ui32CropBottomOffset;

	SINT32	si32FrameFd;
	void	*pY;
	void	*pCb;
	void	*pCr;

	VDEC_KDRV_MPEG_OUTPUT_T	m;		///< selected when vcodec != VDEC_KDRV_VCODEC_JPEG.

	struct
	{
		UINT32 	ui32PicType;
		UINT32	ui32Rpt_ff;
		UINT32	ui32Top_ff;
		UINT32	ui32BuffAddr;
		UINT32	ui32Size;
	} sUsrData;

} VDEC_KDRV_NOTIFY_PARAM_PICD_T;

typedef struct
{
	UINT32	ch;
	VDEC_KDRV_NOTIFY_PARAM_PICD_T	picd;
} VDEC_KDRV_IO_DEC_PICD_OUTPUT_T;

// will be move to vdec_noti.h
typedef struct
{
	BOOLEAN	bPtsMatched;
	BOOLEAN	bDropped;
	UINT32	ui32NumDisplayed;
	UINT32	ui32DqOccupancy;
	UINT32	ui32PicWidth;
	UINT32	ui32PicHeight;

	UINT32	ui32uID;
	UINT64	ui64TimeStamp;
	UINT32	ui32PTS;

	struct
	{
		UINT32 	ui32PicType;
		UINT32	ui32Rpt_ff;
		UINT32	ui32Top_ff;
		UINT32	ui32BuffAddr;
		UINT32	ui32Size;
	} sUsrData;

	SINT32	si32FrameFd;

} VDEC_KDRV_NOTIFY_PARAM_DISP_T;

typedef struct
{
	UINT32	ch;
	VDEC_KDRV_NOTIFY_PARAM_DISP_T	disp;
} VDEC_KDRV_IO_DISP_OUTPUT_T;

typedef VDEC_KDRV_NOTIFY_PARAM_DISP_T	VDEC_KDRV_DISP_OUTPUT_T;

typedef struct
{
	VDEC_KDRV_NOTIFY_PARAM_PICD_T	decode;
	VDEC_KDRV_NOTIFY_PARAM_DISP_T	display;
} VDEC_KDRV_OUTPUT_T;

// will be move to vdec_noti.h
typedef struct
{
	enum
	{
		VDEC_KDRV_NOTI_DEC_ID_NONE = 0,
		VDEC_KDRV_NOTI_DEC_ID_SEQH,
		VDEC_KDRV_NOTI_DEC_ID_PICD,
		VDEC_KDRV_NOTI_DEC_ID_UINT32 = 0x20120703,
	} id;
	union
	{
		VDEC_KDRV_NOTIFY_PARAM_SEQH_T	seqh;
		VDEC_KDRV_NOTIFY_PARAM_PICD_T	picd;
	} u;
} VDEC_KDRV_NOTIFY_PARAM_DEC_T;

// will be move to vdec_noti.h
typedef struct
{
	enum
	{
		VDEC_KDRV_NOTI_ID_NONE = 0,
		VDEC_KDRV_NOTI_ID_INPUT,
		VDEC_KDRV_NOTI_ID_FEED,
		VDEC_KDRV_NOTI_ID_DEC,
		VDEC_KDRV_NOTI_ID_DISP,
		VDEC_KDRV_NOTI_ID_UINT32 = 0x20120507,
	} id;
	union
	{
		VDEC_KDRV_NOTIFY_PARAM_ES_T	input;
		VDEC_KDRV_NOTIFY_PARAM_ES_T	feed;
		VDEC_KDRV_NOTIFY_PARAM_DEC_T	dec;
		VDEC_KDRV_NOTIFY_PARAM_DISP_T	disp;
	} u;
} VDEC_KDRV_NOTI_INFO_T;

typedef struct
{
	UINT32	ch;
	UINT32	ui32PicWidth;
	UINT32	ui32PicHeight;
	UINT32	ui32uID;
	UINT64	ui64TimeStamp;
	UINT32	ui32PTS;
	SINT32	si32FrameFd;
} VDEC_KDRV_IO_FB_INFO_T;


typedef struct
{
	UINT32	ch;
	VDEC_KDRV_NOTI_INFO_T	info;
} VDEC_KDRV_IO_CALLBACK_INFO_T;

typedef struct
{
	UINT32	ch;
	UINT32	uID;
} VDEC_KDRV_IO_GET_LATEST_UID_T;

typedef struct
{
	UINT32	ch;
	UINT8	clkID;
	UINT32	stcBaseTime;
	UINT32	ptsBaseTime;
} VDEC_KDRV_IO_BASETIME_T;

typedef struct
{
	UINT32	ch;
	UINT32	ui32FrameRateRes;
	UINT32	ui32FrameRateDiv;
} VDEC_KDRV_IO_FRAMERATE_T;

typedef struct
{
	UINT32	ch;
	BOOLEAN	bEnable;
	UINT32	usrPA;
	UINT32	usrSz;
} VDEC_KDRV_IO_CTRL_USRD_T;

typedef struct
{
	UINT32	ch;
	SINT32	si32FreeFrameFd;
} VDEC_KDRV_IO_FREE_DPB_T;

/**
* Video decoder play type.
*
* @enum VDEC_KDRV_CMD_PLAY
*  normal play
*
* @enum VDEC_KDRV_CMD_STEP
*  one frame display per one call
*/
typedef enum
{
	VDEC_KDRV_CMD_PLAY,
	VDEC_KDRV_CMD_PAUSE,
	VDEC_KDRV_CMD_STEP,
	VDEC_KDRV_CMD_FREEZE,
	VDEC_KDRV_CMD_INVALID,
	VDEC_KDRV_CMD_MIN = VDEC_KDRV_CMD_PLAY,
	VDEC_KDRV_CMD_MAX = VDEC_KDRV_CMD_INVALID,
	VDEC_KDRV_CMD_MAXN = (VDEC_KDRV_CMD_MAX+1),
} VDEC_KDRV_CMD_T;

/**
Start Parameter for VDEC_KDRV_IO_PLAY.

*/
typedef struct
{
	UINT32	cmd;
	SINT32	speed;		// apply for only CMD_PLAY mode
} VDEC_KDRV_IO_PLAY_T;

typedef struct
{
	UINT32	ch;
	VDEC_KDRV_CMD_T			cmd;
#define	VDEC_KDRV_SPEED_RATE_NORMAL		1000
#define	VDEC_KDRV_SPEED_RATE_HALF		(VDEC_KDRV_SPEED_RATE_NORMAL/2)
#define	VDEC_KDRV_SPEED_RATE_x2			(VDEC_KDRV_SPEED_RATE_NORMAL*2)
#define	VDEC_KDRV_SPEED_RATE_x4			(VDEC_KDRV_SPEED_RATE_NORMAL*4)
#define	VDEC_KDRV_SPEED_RATE_x8			(VDEC_KDRV_SPEED_RATE_NORMAL*8)
#define	VDEC_KDRV_SPEED_RATE_x16		(VDEC_KDRV_SPEED_RATE_NORMAL*16)
#define	VDEC_KDRV_SPEED_RATE_x32		(VDEC_KDRV_SPEED_RATE_NORMAL*32)
#define	VDEC_KDRV_SPEED_RATE_INVALID	0x7FFFFFFF
	SINT32					speed;

	VDEC_KDRV_PIC_SCAN_T	picScanMode;
	VDEC_KDRV_PIC_SCAN_T	srcScanType;
	VDEC_KDRV_SYNC_T		syncOn;
} VDEC_KDRV_IO_PLAY_PARAM_T;

/**
 * Default Paramter for open channel(VDEC_KDRV_IO_OPEN_CH).
 *
 * @see VDEC_KDRV_IO_OPEN_CH_T
 */
#define VDEC_KDRV_IO_OPEN_CH_DEFAULT_PARAM  {									\
										.vcodec=VDEC_KDRV_VCODEC_MPEG2_HP,		\
										.src=VDEC_KDRV_SRC_SDEC0,							\
										.dst=VDEC_KDRV_DST_DE0,						\
										.opmode=VDEC_KDRV_OPMOD_NORMAL, \
										.useGstc=0,			\
										.bufDelaySize=0,		\
										.disDelayOffset=0,		\
										.usrPA=0,			\
										.usrSz=0,				\
										.noAdpStr=FALSE			\
										}

/**
 * Default Start Paramter for play setting(VDEC_KDRV_IO_PLAY_SET).
 *
 * @see VDEC_KDRV_IO_PLAY_SET_T
 */
#define VDEC_KDRV_IO_PLAY_SET_DEFAULT_PARAM  {									\
										.picScanMode=VDEC_KDRV_PIC_SCAN_ALL,		\
										.srcScanType=VDEC_KDRV_PIC_SCAN_ALL,		\
										.syncOn=VDEC_KDRV_SYNC_ON,							\
										.freeze=VDEC_KDRV_FREEZE_UNSET,						\
										.notifyMask=VDEC_KDRV_NOTIFY_MSK_SEQH|VDEC_KDRV_NOTIFY_MSK_PICD|VDEC_KDRV_NOTIFY_MSK_DISP|VDEC_KDRV_NOTIFY_MSK_RTIMEOUT|VDEC_KDRV_NOTIFY_MSK_ERR, \
										}

/**
 * Void parameter for desired play setting(VDEC_KDRV_IO_PLAY_SET).
 *
 * @see VDEC_KDRV_IO_PLAY_SET_T
 */
#define VDEC_KDRV_IO_PLAY_SET_VOID_PARAM  {									\
										.picScanMode=VDEC_KDRV_PIC_SCAN_MAXN,		\
										.srcScanType=VDEC_KDRV_PIC_SCAN_MAXN,		\
										.syncOn=VDEC_KDRV_SYNC_MAXN,							\
										.freeze=VDEC_KDRV_FREEZE_MAXN,						\
										.notifyMask=VDEC_KDRV_NOTIFY_MSK_ENDMARK,		\
										}
/**
 * Void parameter for desired play setting(VDEC_KDRV_IO_PLAY_SET).
 *
 * @see VDEC_KDRV_IO_PLAY_SET_T
 */
#define VDEC_KDRV_IO_PLAY_VOID_PARAM  {									\
										.ch=0xFF,		\
										.cmd=VDEC_KDRV_CMD_INVALID,		\
										.speed=VDEC_KDRV_SPEED_RATE_INVALID,							\
										.picScanMode=VDEC_KDRV_PIC_SCAN_INVALID,						\
										.srcScanType=VDEC_KDRV_PIC_SCAN_INVALID,						\
										.syncOn=VDEC_KDRV_SYNC_INVALID,		\
										}

/**
 * Default Start Paramter for play setting(VDEC_KDRV_IO_PLAY_GET).
 *
 * @see VDEC_KDRV_IO_PLAY_SET_T
 */
#define VDEC_KDRV_IO_PLAY_GET_DEFAULT_PARAM  {									\
										.clkID = 0xFF,	\
										.stcBaseTime = 0x0, \
										.ptsBaseTime = 0x0 \
										}


#ifndef VDEC_IO_RATE_DELETED

#endif	/* VDEC_IO_RATE_DELETED */


/*----------------------------------------------------------------------------------------
	Notify Parameters
----------------------------------------------------------------------------------------*/
/**
 * Sequence Header Notify Structure.
 *
 * @note
 * profile, level, bGMC : information of Sequence Header or Sequence Parameter Set
 *
 * Appllication can use this information for waning "this contents is not played properly".
 *
 * [L8] : H.264 GMC option is not supported.
 *
 */
/**
 * Error notification.
 */
typedef struct
{
	UINT32 dummy;
} VDEC_KDRV_NOTIFY_PARAM_ERR_T;

/**
 * Almost Full notification.
 */
typedef struct
{
	UINT32 dummy;
} VDEC_KDRV_NOTIFY_PARAM_AFULL_T;

/**
 * Almost Empty notification.
 */
typedef struct
{
	UINT32 dummy;
} VDEC_KDRV_NOTIFY_PARAM_AEMPTY_T;

typedef struct
{
	UINT32	status;				// VDEC_KDRV_STATUS_T

} VDEC_KDRV_NOTIFY_PARAM_RTIMEOUT_T;

VDEC_EXTERN size_t	sSizeByNotifyID[]
#if defined (_VDEC_NOTI_C_) || defined( _VDEC_KADP_C_ )
= {
	#define VDEC_KDRV_NOTIFY_SIZE_ENTRY(_id)	[ VDEC_KDRV_NOTIFY_ID_ ## _id ] = sizeof( VDEC_KDRV_NOTIFY_PARAM_ ## _id ## _T)
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(SEQH	  ),
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(PICD	  ),
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(USRD	  ),
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(DISP	  ),
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(ERR	  ),
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(AFULL   ),
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(AEMPTY  ),
	VDEC_KDRV_NOTIFY_SIZE_ENTRY(RTIMEOUT),
}
#endif
;

typedef struct {
#define VDEC_KDRV_NOTIFY_MAGIC	0xdec1dec0
	UINT32 magic;
	UINT32 id;
	union{
		VDEC_KDRV_NOTIFY_PARAM_SEQH_T		seqh;
		VDEC_KDRV_NOTIFY_PARAM_PICD_T		picd;
		VDEC_KDRV_NOTIFY_PARAM_USRD_T		usrd;
		VDEC_KDRV_NOTIFY_PARAM_DISP_T		disp;
		VDEC_KDRV_NOTIFY_PARAM_ERR_T		err;
		VDEC_KDRV_NOTIFY_PARAM_AFULL_T	afull;
		VDEC_KDRV_NOTIFY_PARAM_AEMPTY_T	aempty;
		VDEC_KDRV_NOTIFY_PARAM_RTIMEOUT_T	rtimeout;
	} u;
} VDEC_KDRV_NOTIFY_PARAM2_T;

/*----------------------------------------------------------------------------------------
	Memory Structure of User Data Buffer.
----------------------------------------------------------------------------------------*/
/**
 * User Data Buffer structure.
 *
 * VDEC_KDRV_USRD_BUFF_T
 * VDEC_KDRV_USRD_HEAD_T
 * VDEC_KDRV_USRD_SEG_HEAD_T
 *
 * 20101121 seokjoo.lee
 * [NOTE] BODA Firmware accesses DDR memory 8 byte unit.
 * the firmware version before 0x20101022, define USRD_8BYTE_REVERSED.
 * after then, should undefine this.
 */
#undef USRD_8BYTE_REVERSED

#ifdef USRD_8BYTE_REVERSED
typedef struct
{
	UINT16	bytes;
	UINT16	type;
} VDEC_KDRV_USRD_SEG_HEAD_T;

typedef struct
{
	UINT16	bytesTotal;			///< total nmber of bytes for user data segments
	UINT16	nSegment;			///< number of user data segments for given picture decoded.
	VDEC_KDRV_USRD_SEG_HEAD_T seg[16];	///< User Data Segment header
} VDEC_KDRV_USRD_HEAD_T;

#define GET_USRD_16BIT(_u16)	(_u16)

#else

/* C A U T I O N
 * Boda firmware stores.. byte order in every 8 bytes...
 * SEG_HEAD_T.type = ( ((pHead->type & 0x00ff) << 8) | ((pHead->type & 0xff00) >> 8 ) )
 */
typedef struct
{
	UINT16	type;				///< type of user data.(sequence header or picture header)
	UINT16	bytes;				///< bytes of user data.
} VDEC_KDRV_USRD_SEG_HEAD_T;

typedef struct
{
	UINT16	nSegment;			///< number of user data segments.
	UINT16	bytesTotal;			///< total nmber of bytes for user data segments
	VDEC_KDRV_USRD_SEG_HEAD_T seg[16];	///< User Data Segment header
} VDEC_KDRV_USRD_HEAD_T;

#define GET_USRD_16BIT(_u16)	( (((_u16) & 0x00ff) << 8) | (((_u16) & 0xff00) >> 8 ) )

#endif

typedef struct
{
	VDEC_KDRV_USRD_HEAD_T	hdr;
	UINT8				pDat[0x2000 - sizeof(VDEC_KDRV_USRD_HEAD_T)];
} VDEC_KDRV_USRD_BUFF_T;

#define VDEC_KDRV_NOTIFY_BUFFER_SIZE	256

/**
Notify packet structure.
	When kernel driver has information on enabled notify mask,
	save those informatioin on per-process queue allocated when first VDEC_KDRV_IO_SET_NOTIFY.
	and then wake up waiting process.

	monitoring thread which belongs to a client process read stored notifiaction information
	from per-process queue and copies to user layer buffer.
	.
@code
	main ()
	{
		VDEC_KDRV_CTX_T *pCtx;

		VDEC_KDRV_NOTIFY_T notifyParam;

		pCtx = KADP_VDEC_Open(channel);	// automatically spawn monitoring thread.

		if ( !pCtx ) return;			// error.

		notifyParam.mskNotify = VDEC_KDRV_NOTIFY_MSK_SEQH | VDEC_KDRV_NOTIFY_MSK_PICD | VDEC_KDRV_NOTIFY_MSK_USRD;

		KADP_VDEC_Ioctl(pCtx, VDEC_KDRV_IO_SET_NOTIFY, &notifyParam);
		...
	}


	void MonitoringTask(void)
	{
		while (1)
		{
			VDEC_KDRV_NOTIFY_PARAM2_T	pNotifyBuf[256];

			bytes_read = read(pCtx->fd, (void*)pNotifyBuf, sizeof(pNotifyBuf));

			if ( bytes_read <= 0 )	continue;		// nothing received -> skip.

			nNotified = bytes_read / sizeof(VDEC_KDRV_NOTIFY_PARAM2_T);
			for iNoti = 0;

			// actual handling.
			pNoti = pNotifyBuf;
			for ( iNoti=0 ; iNoti < nNotified; iNoti++, pNoti++)
			{
				switch (pNoti->id)
				{
					case VDEC_KDRV_NOTIFY_ID_SEQH: // handle Sequence header.
					...
				}
			}
		}
	}
@endcode
*/
typedef struct
{
	/**
	which event is enabled per vdec channel.
	mask shall be set per-channel basis with VDEC_KDRV_NOTIFY_MSK_T
	*/
	UINT32	mask[VDEC_KDRV_CH_MAXN];

	/**
	notify data payload.
	which shall be filled from kernel driver only contains specified by mask field.
	*/
	UINT8	data[VDEC_KDRV_CH_MAXN][VDEC_KDRV_NOTIFY_ID_MAXN][VDEC_KDRV_NOTIFY_BUFFER_SIZE];
											// 0x80 * 4 + 0x10 = 0x210
} VDEC_KDRV_NOTIFY_PARAM_T;

// [debugging] build a list of iocmd.
VDEC_EXTERN const char*	gstrVdecIoCmd[]
#if defined(_VDEC_KADP_C_) || defined( _VDEC_NOTI_C_)
=
{															// 0123456
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_OPEN_CH)				= "OPEN_CH",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_CLOSE_CH)				= "CLOS_CH",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_PLAY)					= "PLAY",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_PLAY_SET)				= "SET",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_FLUSH)					= "FLUSH",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_SET_BUFFER_LVL)			= "S_BFLVL",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_GET_BUFFER_STATUS)			= "G_BFSTA",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_UPDATE_BUFFER)			= "UP_BUFF",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_GET_OUTPUT)				= "G_OUTP",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_GET_VERSION)			= "GET_VER",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_SET_REG)				= "S_REG",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_GET_REG)				= "G_REG",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_ENABLELOG)				= "E_LOG",
	LX_IOCMD_STRING_HOLDER(VDEC_KDRV_IO_DBG_CMD)				= "DEBUG",
	[VDEC_KDRV_IOC_MAXNR] = "unknown"
}
#endif
;

VDEC_EXTERN const char *gstrVdecVcodecName[]
#if defined(_VDEC_KADP_C_) || defined( _VDEC_NOTI_C_)
=
{
	[VDEC_KDRV_VCODEC_H264_HP]			= "h264",
	[VDEC_KDRV_VCODEC_H264_MVC]			= "h264_mvc",
	[VDEC_KDRV_VCODEC_VC1_RCV_V1]		= "vc1v1",
	[VDEC_KDRV_VCODEC_VC1_RCV_V2]		= "vc1v2",
	[VDEC_KDRV_VCODEC_VC1_ES]			= "vc1es",
	[VDEC_KDRV_VCODEC_MPEG2_HP]			= "mpg2",
	[VDEC_KDRV_VCODEC_MPEG4_ASP]		= "mpg4",
	[VDEC_KDRV_VCODEC_XVID]				= "xvid",
	[VDEC_KDRV_VCODEC_DIVX3]			= "dvx3",
	[VDEC_KDRV_VCODEC_DIVX4]			= "dvx4",
	[VDEC_KDRV_VCODEC_DIVX5]			= "dvx5",
	[VDEC_KDRV_VCODEC_RVX]				= "rvx",
	[VDEC_KDRV_VCODEC_AVS]				= "avs",
}
#endif
;

VDEC_EXTERN const char *gstrVdecSrcName[]
#if defined(_VDEC_KADP_C_) || defined( _VDEC_NOTI_C_)
=
{
	[VDEC_KDRV_SRC_SDEC0]	= "te0",
	[VDEC_KDRV_SRC_SDEC1]	= "te1",
	[VDEC_KDRV_SRC_SDEC2]	= "te2",
	[VDEC_KDRV_SRC_BUFF0 ]	= "buf0",
	[VDEC_KDRV_SRC_BUFF1 ]	= "buf1",
	[VDEC_KDRV_SRC_TVP ]	= "tvp",
	[VDEC_KDRV_SRC_GRP_BUFF0]	= "SrcGrpbuf0",
	[VDEC_KDRV_SRC_GRP_BUFF1]	= "SrcGrpbuf1",
}
#endif
;
VDEC_EXTERN const char *gstrVdecDstName[]
#if defined(_VDEC_KADP_C_) || defined( _VDEC_NOTI_C_)
=
{
	[VDEC_KDRV_DST_DE0]	= "de0",
	[VDEC_KDRV_DST_DE1]	= "de1",
	[VDEC_KDRV_DST_BUFF]	= "DstBuf",
	[VDEC_KDRV_DST_GRP_BUFF0]	= "DstGrpBuf0",
	[VDEC_KDRV_DST_GRP_BUFF1]	= "DstGrpBuf0",
}
#endif
;
VDEC_EXTERN const char *gstrVdecNotifyName[]
#if defined(_VDEC_KADP_C_) || defined( _VDEC_NOTI_C_)
=
{
	[VDEC_KDRV_NOTIFY_ID_SEQH]	= "SEQH",
	[VDEC_KDRV_NOTIFY_ID_PICD]	= "PICD",
	[VDEC_KDRV_NOTIFY_ID_USRD]	= "USRD",
	[VDEC_KDRV_NOTIFY_ID_DISP]	= "DISP",
	[VDEC_KDRV_NOTIFY_ID_ERR]		= "ERR ",
	[VDEC_KDRV_NOTIFY_ID_AFULL]	= "FULL",
	[VDEC_KDRV_NOTIFY_ID_AEMPTY]	= "EMPT",
	[VDEC_KDRV_NOTIFY_ID_RTIMEOUT]= "TOUT"
}
#endif
;

/**
@def VDEC_KDRV_IOCMD_PREFIX_SIZE
[debug] prefix byte size of IO Command.
which is used to skip common prefix in @ref VDEC_KDRV_IOCMD_NAME

@def VDEC_KDRV_IOCMD_NAME
[debug] get string from VDEC IO command value.
@param _iocmd	VDEC IO command
@return string pointer to given IO command.
if valid, returns pointer to string of given _iocmd value.
else      returns NULL
[NOTE] : prefix shall be removed.
@{
*/
#define VDEC_KDRV_IOCMD_NAME(_iocmd)		LX_IOCMD_NAME(_iocmd, gstrVdecIoCmd, VDEC_KDRV_IOC_MAXNR, 0)
#define VDEC_KDRV_VCODEC_NAME(_vcodec)			gstrVdecVcodecName[((UINT32)_vcodec)%VDEC_KDRV_VCODEC_INVALID]
#define VDEC_KDRV_SRC_NAME(_src_id)		gstrVdecSrcName   [((UINT32)_src_id)%VDEC_KDRV_SRC_MAXN]
#define VDEC_KDRV_DST_NAME(_dst_id)		gstrVdecDstName   [((UINT32)_dst_id)%VDEC_KDRV_DST_MAXN]
#define VDEC_KDRV_NOTIFY_NAME(_noti_id)	gstrVdecNotifyName[((UINT32)_noti_id)%VDEC_KDRV_NOTIFY_ID_MAXN]
/** @} */

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _VDEC_DRV_H_ */

/** @} */
