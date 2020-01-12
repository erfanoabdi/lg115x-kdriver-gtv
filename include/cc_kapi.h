/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file cc_kapi.h
 *
 *  application interface header for cc device
 *
 *  @author		wonsik.do (wonsik.do@lge.com)
 *  @version		1.0 
 *  @date		2010.01.06
 *
 *  @addtogroup lg1150_cc 
 *	@{
 */

#ifndef	_CC_KAPI_H_
#define	_CC_KAPI_H_

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
#define	CC_IOC_MAGIC		'c'
/**
@name CC IOCTL List
ioctl list for cc device.

@{
@def CC_IO_INIT
Creates CC VBI buffer on kernel memory, and initializes VBI parameters.

@def CC_IOW_ONOFF_INTERRUPT
Sets ON/OFF the CC interrupt
TRUE : On the CC interrup, FALSE : OFF the CC interrupt

@def CC_IOW_ONOFF_SLICER
Sets ON/OFF the CC Slicer
TRUE : On the CC slicer, FALSE : OFF the CC slicer

@def CC_IOR_GET_PACKET_COUNT
Read Received CC Packet Number in kernel buffer.

@def CC_IOR_GET_PARAMETER_OFFSET
Read Offset address (from MMAP address)of CC Parameter structure. 

@def CC_IOR_GET_MMAP_SIZE
Read MMAP size of CC driver.

@def CC_IO_RESET_CC
Reset CC parameters

@def CC_IO_RELEASE
Release CC device(Free VBI buffer & stop interrupt)

@def CC_IO_RESET_CC_BUFFER
Resets Kernel CC buffer

@def CC_IOWR_GET_CC_PACKET
Read CC data1/2 and field indication from kernel driver
*/
/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
#define	CC_IO_RESET				_IO(CC_IOC_MAGIC,  0 )	
//#define CC_IOR_CHIP_REV_INFO		_IOR(CC_IOC_MAGIC, 1, CHIP_REV_INFO_T )
//#define CC_IOW_WRITE_UINT32		_IOW(CC_IOC_MAGIC, 2, UINT32 )
//#define CC_IOW_WRITE_STRUCT		_IOW(CC_IOC_MAGIC, 3, LX_CC_IOCTL_PARAM_T)
#define	CC_IO_INIT					_IO(CC_IOC_MAGIC, 4)
#define CC_IOW_ONOFF_INTERRUPT		_IOW(CC_IOC_MAGIC, 5, BOOLEAN)
#define CC_IOW_ONOFF_SLICER			_IOW(CC_IOC_MAGIC, 6, BOOLEAN)
#define	CC_IOR_GET_PACKET_COUNT		_IOR(CC_IOC_MAGIC, 7, UINT32)
//#define CC_IOR_GET_PARAMETER_OFFSET	_IOR(CC_IOC_MAGIC, 8, UINT32)
//#define CC_IOR_GET_MMAP_SIZE		_IOR(CC_IOC_MAGIC, 9, UINT32)
//#define CC_IO_RESET_CC			_IO(CC_IOC_MAGIC, 10)
//#define CC_IO_RELEASE				_IO(CC_IOC_MAGIC, 11)
#define CC_IO_RESET_CC_BUFFER		_IO(CC_IOC_MAGIC, 12)
#define CC_IOR_GET_CC_PACKET	_IOWR(CC_IOC_MAGIC, 13, LX_CC_DATA_T)

#define CC_IOC_MAXNR			14	
/** @} */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
//#define LX_CC_PACKET_UNIT	4

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/**
  * Structure of CC read data from kernel driver
  * @see 
  */
typedef struct {
	UINT8	data1;	///< CC data1
	UINT8	data2;	///< CC data2
	UINT8	field;	///< odd/even field indication(0:Even field, 1:Odd filed)
	UINT8	status; ///< status(reserved)
} LX_CC_DATA_T;

#if 0
/**
  * Structure of CC read data from kernel driver
  * @see CC_IOWR_GET_CC_PACKET
  */
typedef struct {
	LX_CC_DATA_T cc_data;
	UINT32		number;		///< Number of CC line to read from kernel driver
} LX_CC_DATA_INFO_T;

/**
  * Structure of CC Parameters 
  * @see CC_IOR_GET_PARAMETER_OFFSET
  */

  typedef struct {
	  UINT32	CC_Data_Buffer_Size;	///< Size of CC Data Buffer
	  UINT32	PutAddr;			///< offset address of put address
	  UINT32	GetAddr;			///< offset address of get address
	  UINT32	Overflow;				///< overflow indication ( set to 0x01 when buffer overflow occur)
  }	LX_CC_PARAMETER_T;

/**
  * Structure to read CC data & field
  * @see CC_IOR_GET_CC_PACKET
  */
typedef struct {
	UINT8	*pCC_Packet_Buffer;	///< CC packet buffer 
	UINT32	CC_Packet_Unit;		///< CC packet unit(number of received cc lines, one line is 4bytes long(2 data bytes + filed indication bit + 1 reserved bit + 10 vbi data line number bits))
}	LX_CC_PACKET_TYPE_T;
#endif 
/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _CC_DRV_H_ */

/** @} */
