
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

/** @file ci_kapi.h
 *
 *  application interface header for ci device
 *
 *  @author		eunjoo.kim (eunjoo.kim@lge.com)
 *  @version		1.0
 *  @date		2009.12.30
 *
 *  @addtogroup lg1150_ci
 *	@{
 */

#ifndef	_CI_KAPI_H_
#define	_CI_KAPI_H_

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
#define	CI_IOC_MAGIC		'a'
/**
@name CI IOCTL List
ioctl list for ci device. Also the list includes IOCTLs for ci plus

@{
@def CI_IO_RESET
Reset ci module.
When ci device receives above IOCTL, then reset itself.

@def CI_IO_CHECK_CIS
Reads CIS(Card Information Struncture) from CI module

@def CI_IO_WRITE_COR
write COR(Configuration Option Register) data in the attribution memory of ci module.

@def CI_IOR_DETECT_CARD
Get the status of insertion or removal of CI module.

@def CI_IOR_READ_DATA
Read data from CI.

@def CI_IOR_NEGOTIATE_BUF_SIZE
Get the negotiated buffer size.
Host reads memory size supported by CI module from its CIS. Host compares its memory size with memory size read from CI module. And Host decides the size of which memory will be used to read and write between Hose and moduel
- Host choices the smaller one between the sizes supported by Host & CI module.

@def CI_IOR_READ_DA_STATUS
Read from stauts register if Data Abailable is set.

@def CI_IOW_SWITCH_BYPASS_MODE
Set or Clear the Bypass mode.

@def CI_IOW_WRITE_DATA
Write data to CI module

@def CI_IO_SET_PHY_RESET
Reset Physical Interface for PCMCIA module.
This is for CI plus

@def CI_IOR_READ_IIR_STATUS
Read IIR(Initialze Interface Request) from status register.
This is for CI plus.
CICAM can request Physical Interface Reset by setting IIR. If CICAM set IIR, it shall not send APDU.

@def CI_IOR_GET_DATA_RATE
Get the maxmimum data rate supported by Host.
This is for CI plus
Host must support 72Mbps, and support for 96Mbps is optional.

@def CI_IOR_CHECK_CAPABILITY
Get the type of CI CAM from CIS data.
If CAM is for CI, it returns 0
If CAM is for CI Plus, it returens 1.

@def CI_IO_CAM_INIT
CAM initialize (manual hot swap)

@def CI_IO_CAM_POWEROFF
CAM Power off

@def CI_IO_CAM_POWERONCOMPLETED
CAM Power on


*/
/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
#define	CI_IO_RESET					_IO(CI_IOC_MAGIC, 0 )
#define CI_IO_CHECK_CIS				_IO(CI_IOC_MAGIC, 1 )
#define CI_IO_WRITE_COR				_IO(CI_IOC_MAGIC, 2 )
#define CI_IOR_DETECT_CARD			_IOR(CI_IOC_MAGIC, 3, UINT32 )
#define	CI_IOR_READ_DATA			_IOR(CI_IOC_MAGIC, 4, LX_CI_IOCTL_PARAM_T )
#define CI_IOR_NEGOTIATE_BUF_SIZE	_IOR(CI_IOC_MAGIC, 5, UINT32 )
#define CI_IOR_READ_DA_STATUS		_IOR(CI_IOC_MAGIC, 6, UINT32 )
//#define CI_IOW_SWITCH_BYPASS_MODE	_IOW(CI_IOC_MAGIC, 7, UINT32 )
#define CI_IOW_WRITE_DATA			_IOW(CI_IOC_MAGIC, 8, LX_CI_IOCTL_PARAM_T)
#define	CI_IO_SET_PHY_RESET			_IO(CI_IOC_MAGIC, 9 )
#define	CI_IOR_READ_IIR_STATUS		_IOR(CI_IOC_MAGIC, 10, UINT32)
//#define CI_IOR_GET_DATA_RATE		_IOR(CI_IOC_MAGIC, 11, UINT32)
#define	CI_IOR_CHECK_CAPABILITY		_IOR(CI_IOC_MAGIC, 12, LX_CI_IOCTL_PARAM_CAMTYPE)
//#define	CI_IOR_CAM_INIT				_IOR(CI_IOC_MAGIC, 13, UINT32)
//#define	CI_IOR_PRINT_REG			_IOR(CI_IOC_MAGIC, 14, UINT32)
//#define	CI_IOR_CAM_POWEROFF			_IOR(CI_IOC_MAGIC, 15, UINT32)
//#define	CI_IOR_CAM_POWERONCOMPLETED	_IOR(CI_IOC_MAGIC, 16, UINT32)
#define	CI_IO_CAM_INIT				_IO(CI_IOC_MAGIC, 13)
#define	CI_IO_PRINT_REG				_IO(CI_IOC_MAGIC, 14)
#define	CI_IO_CAM_POWEROFF			_IO(CI_IOC_MAGIC, 15)
#define	CI_IO_CAM_POWERONCOMPLETED	_IO(CI_IOC_MAGIC, 16)

#define CI_IOC_MAXNR				16
/** @} */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
 * ci ioctl parameter.
 * useful when complex parameter should be passed to ci kernel driver.
 * @see CI_IOW_WRITE_STRUCT
 */
typedef struct
{
	UINT8*	pBuf;				///< type of codec
	UINT16	sLength;		///< optional parameter.
}
LX_CI_IOCTL_PARAM_T;


/**
 * ci ioctl parameter.
 * useful when complex parameter should be passed to ci kernel driver.
 * @see CI_IOR_CHECK_CAPABILITY
 */
typedef struct LX_CI_IOCTL_PARAM_CAMTYPE_T
{
	SINT8 uwRtnValue;			///< return value
	UINT8 uwCheckCAMType;		///< CAM type.
}
LX_CI_IOCTL_PARAM_CAMTYPE;


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _CI_DRV_H_ */

/** @} */
