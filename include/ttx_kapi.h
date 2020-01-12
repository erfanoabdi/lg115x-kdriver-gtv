
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

/** @file ttx_kapi.h
 *
 *  application interface header for ttx device
 *
 *  @author		wonsik.do (wonsik.do@lge.com)
 *  @version		1.0
 *  @date		2010.01.06
 *
 *  @addtogroup lg1150_ttx
 *	@{
 */

#ifndef	_TTX_KAPI_H_
#define	_TTX_KAPI_H_

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
#define	TTX_IOC_MAGIC		't'
/**
@name TTX IOCTL List
ioctl list for ttx device.

@{

@def TTX_IO_INIT
Creates TTX VBI buffer on kernel memory, and initialize VBI parameters.

@def TTX_IOW_ONOFF_INTERRUPT
Sets ON/OFF the TTX interrupt
TRUE : ON the TTX interrupt, FALSE : Off the TTX Interrrupt

@def TTX_IOW_ONOFF_SLICER
Sets ON/OFF the TTX SLICER
TRUE : ON the TTX interrupt, FALSE : Off the TTX Interrrupt

@def TTX_IO_RESET_TTX_BUFFER
Resets TTX buffer

@def TTX_IO_RESET_VPS_BUFFER
Resets VPS buffer

@def TTX_IO_RESET_WSS_BUFFER
Resets WSS buffer

@def TTX_IOR_GET_PACKET_COUNT
Read Received TTX Packet Number in kernel buffer.

@def TTX_IOR_GET_STATUS
Read received TTX status(received data type) from kernel driver.

@def TTX_IOR_GET_VPS_PACKET
Read VPS packets(15bytes) from kernel driver

@def TTX_IOR_GET_WSS_PACKET
Read WSS packets(4bytes) from kernel driver.

@def TTX_IOR_GET_TTX_PACKET
Read TTX packets and size from kernel driver.

@def TTX_IOR_GET_BUFFER_INFO
Read TTX buffer information(TTX size, vps offset, wss offset, putaddr offset, getaddr offset, etc)

@def TTX_IOR_GET_PARAMETER_OFFSET
Read Offset address (from MMAP address)of TTX Parameter structure.

@def TTX_IOR_GET_MMAP_SIZE
Read MMAP size of TTX driver.

@def TTX_IO_RESET_TTX
Reset TTX parameters

@def TTX_IO_RELEASE
Release TTX device(Free VBI buffer)

@def TTX_IOW_ENABLE_VPS_SLICER
Enable/Disable VPS Slicing on VBI Line 16. If disabled(default condition) TTX slicing is executed on Line 16.

@def TTX_CC_IO_INIT
Creates CC VBI buffer on kernel memory, and initializes VBI parameters.

@def TTX_CC_IOW_ONOFF_INTERRUPT
Sets ON/OFF the CC interrupt
TRUE : On the CC interrup, FALSE : OFF the CC interrupt

@def TTX_CC_IOW_ONOFF_SLICER
Sets ON/OFF the CC Slicer
TRUE : On the CC slicer, FALSE : OFF the CC slicer

@def TTX_CC_IOR_GET_PACKET_COUNT
Read Received CC Packet Number in kernel buffer.

@def TTX_CC_IOR_GET_PARAMETER_OFFSET
Read Offset address (from MMAP address)of CC Parameter structure.

@def TTX_CC_IOR_GET_MMAP_SIZE
Read MMAP size of CC driver.

@def TTX_CC_IO_RESET_CC
Reset CC parameters

@def TTX_CC_IO_RELEASE
Release CC device(Free VBI buffer & stop interrupt)

@def TTX_CC_IO_RESET_CC_BUFFER
Resets Kernel CC buffer

@def TTX_CC_IOWR_GET_CC_PACKET
Read CC data1/2 and field indication from kernel driver

#def TTX_CC_IOW_SET_DEBUGMODE_CC_PRINT
Print incoming CC data to shell, which is only used for debugging

*/
/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
//#define	TTX_IO_RESET				_IO(TTX_IOC_MAGIC,  0 )
//#define TTX_IOR_CHIP_REV_INFO		_IOR(TTX_IOC_MAGIC, 1, CHIP_REV_INFO_T )
//#define TTX_IOW_WRITE_UINT32		_IOW(TTX_IOC_MAGIC, 2, UINT32 )
//#define TTX_IOW_WRITE_STRUCT		_IOW(TTX_IOC_MAGIC, 3, LX_TTX_IOCTL_PARAM_T)
#define	TTX_IO_INIT					_IO(TTX_IOC_MAGIC, 4)
#define TTX_IOW_ONOFF_INTERRUPT		_IOW(TTX_IOC_MAGIC, 5, UINT8)
#define TTX_IOW_ONOFF_SLICER			_IOW(TTX_IOC_MAGIC, 6, UINT8)
#define TTX_IO_RESET_TTX_BUFFER		_IO(TTX_IOC_MAGIC, 7)
#define TTX_IO_RESET_VPS_BUFFER		_IO(TTX_IOC_MAGIC, 8)
#define TTX_IO_RESET_WSS_BUFFER		_IO(TTX_IOC_MAGIC, 9)
//#define	TTX_IOR_GET_PACKET_COUNT		_IOR(TTX_IOC_MAGIC, 10, UINT32)
//#define TTX_IOR_GET_STATUS			_IOR(TTX_IOC_MAGIC, 11, UINT32)
//#define TTX_IOR_GET_VPS_PACKET		_IOR(TTX_IOC_MAGIC, 12, UINT32)
//#define TTX_IOR_GET_WSS_PACKET		_IOR(TTX_IOC_MAGIC, 12, UINT32)
//#define TTX_IOR_GET_TTX_PACKET		_IOR(TTX_IOC_MAGIC, 13, LX_TTX_PACKET_TYPE_T)
//#define TTX_IOR_GET_BUFFER_INFO		_IOR(TTX_IOC_MAGIC, 14, LX_TTX_INFO_T)
#define TTX_IOR_GET_PARAMETER_OFFSET	_IOR(TTX_IOC_MAGIC, 15, UINT32)
#define TTX_IOR_GET_MMAP_SIZE		_IOR(TTX_IOC_MAGIC, 16, UINT32)
#define TTX_IO_RESET_TTX			_IO(TTX_IOC_MAGIC, 17)
#define TTX_IO_RELEASE				_IO(TTX_IOC_MAGIC, 18)
#define TTX_IOW_ENABLE_VPS_SLICER		_IOW(TTX_IOC_MAGIC, 19, BOOLEAN)

#define	TTX_CC_IO_RESET				_IO(TTX_IOC_MAGIC,  20 )
//#define TTX_CC_IOR_CHIP_REV_INFO		_IOR(TTX_IOC_MAGIC, 21, CHIP_REV_INFO_T )
//#define TTX_CC_IOW_WRITE_UINT32		_IOW(TTX_IOC_MAGIC, 22, UINT32 )
//#define TTX_CC_IOW_WRITE_STRUCT		_IOW(TTX_IOC_MAGIC, 23, LX_TTX_IOCTL_PARAM_T)
#define	TTX_CC_IO_INIT					_IO(TTX_IOC_MAGIC, 24)
#define TTX_CC_IOW_ONOFF_INTERRUPT		_IOW(TTX_IOC_MAGIC, 25, BOOLEAN)
#define TTX_CC_IOW_ONOFF_SLICER			_IOW(TTX_IOC_MAGIC, 26, BOOLEAN)
#define	TTX_CC_IOR_GET_PACKET_COUNT		_IOR(TTX_IOC_MAGIC, 27, UINT32)
//#define TTX_CC_IOR_GET_PARAMETER_OFFSET	_IOR(TTX_IOC_MAGIC, 28, UINT32)
//#define TTX_CC_IOR_GET_MMAP_SIZE		_IOR(TTX_IOC_MAGIC, 29, UINT32)
//#define TTX_CC_IO_RESET_CC			_IO(TTX_IOC_MAGIC, 30)
//#define TTX_CC_IO_RELEASE				_IO(TTX_IOC_MAGIC, 31)
#define TTX_CC_IO_RESET_CC_BUFFER		_IO(TTX_IOC_MAGIC, 32)
#define TTX_CC_IOR_GET_CC_PACKET	_IOWR(TTX_IOC_MAGIC, 33, LX_CC_DATA_T)

#define	TTX_IOW_VBI_SET_OPTION_LPF	_IOW(TTX_IOC_MAGIC, 34, BOOLEAN)
#define TTX_IOW_CHECK_LINE318		_IOW(TTX_IOC_MAGIC, 35, BOOLEAN)
#define TTX_IOW_SET_TYPE_LINE318	_IOW(TTX_IOC_MAGIC, 36, BOOLEAN)
#define TTX_IOW_SET_TT_SLICER_MODE	_IOW(TTX_IOC_MAGIC, 37, UINT32)
#define TTX_CC_IOW_SET_DEBUGMODE_CC_PRINT	_IOW(TTX_IOC_MAGIC, 38, UINT32)

#define TTX_IOC_MAXNR				39
/** @} */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define LX_VPS_BUFFER_SIZE	15
#define LX_WSS_BUFFER_SIZE	4
#define LX_TTX_PACKET_UNIT	48
#define LX_TTX_NUM_LINES	35 * 1 /* 6 ~ 22 & 318 ~ 335 */
#define LX_TTX_DATA_BUFFER_SIZE	(LX_TTX_PACKET_UNIT*LX_TTX_NUM_LINES*5) ///< 5 Frames
#define LX_VPS_OFFSET
/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
  * Received data type
  * Set if the TTX_TYPE is sliced, clear if ther TTX_TYPE isn't sliced
  * @see TTX_IOR_GET_STATUS, LX_TTX_PARAMETER_T
  */
typedef enum {
	LX_TTX_TYPE_TTX=1,	///< ttx type ttx
	LX_TTX_TYPE_VPS=2,	///< ttx type vps
	LX_TTX_TYPE_WSS=4,	///< ttx type wss
	LX_TTX_TYPE_ALL=7, ///< ttx type all
} LX_TTX_TYPE_T;

/**
  * Structure to read TTX data & size
  * @see TTX_IOR_GET_TTX_PACKET
  */
typedef struct {
	UINT8	*pTTX_Packet_Buffer;	///< TTX packet buffer
	UINT32	TTX_Packet_Unit;		///< TTX packet unit(number of received ttx lines, one line is 48bytes long)
}	LX_TTX_PACKET_TYPE_T;

/**
  * Structure of TTX Parameters
  * @see TTX_IOR_GET_PARAMETER_OFFSET
  */

  typedef struct {
//	  UINT32	TTX_Data_Buffer_Size;	///< Size of TTX Data Buffer(unit of 48bytes)
	  UINT32	GetAddr;			///< offset address of get address (unit of 48bytes)
	  UINT32	PutAddr;			///< offset address of put address (unit of 48bytes)
	  UINT32	Count;
	  UINT32	VPS_Offset;				///< offset address of VPS data buffer
	  UINT32	WSS_Offset;				///< offset address of WSS data buffer
//	  UINT32	Overflow;				///< overflow indication ( set to 0x01 when buffer overflow occur)
	  UINT32	Status;					///< TTX buffer information( @see TTX_IOR_GET_STATUS)
  }	LX_TTX_PARAMETER_T;

typedef struct {
	UINT8	data1;	///< CC data1
	UINT8	data2;	///< CC data2
	UINT8	field;	///< odd/even field indication(0:Even field, 1:Odd filed)
	UINT8	status; ///< status(reserved)
	UINT16	line_number; ///< sliced vbi line number
} LX_CC_DATA_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _TTX_DRV_H_ */

/** @} */
