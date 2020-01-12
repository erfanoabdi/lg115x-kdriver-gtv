
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

/** @file png_kapi.h
 *
 *  application interface header for png device
 *
 *  @author		raxis.lim (raxis.lim@lge.com)
 *  @version		1.0 
 *  @date		2010.10.05
 *
 *  @addtogroup lg1150_png 
 *	@{
 */

#ifndef	_PNG_KAPI_H_
#define	_PNG_KAPI_H_

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
#define	PNG_IOC_MAGIC		'p'
/**
@name PNG IOCTL List
ioctl list for png device.

@{
@def PNG_IO_RESET
Reset png module.
When png device receives above IOCTL, then reset itself.

@def PNG_IOR_CHIP_REV_INFO
Reads chip revision information.
Just get chiop revision information. To support automatic changing behavior of device driver or middleware by this ioctl.

@def PNG_IOW_WRITE_UINT32
write 32bit value to png module.
detailed...[minimum two lines of comment required. one line of comment does not generated automatically]

@def PNG_IOW_WRITE_STRUCT		
control complex parameter to png module.
control codec_type, speed, and additional parameter.
no need to use "see also", exact struct name shall be linked automatically.

*/
/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
#define PNG_IOR_GET_CAPS			_IOR(PNG_IOC_MAGIC, 0, LX_PNG_DECODE_CAPS_T )
#define	PNG_IO_SW_RESET				_IO(PNG_IOC_MAGIC,  1 )	
#define PNG_IOR_DECODE_BUFFER_SIZE	_IOR(PNG_IOC_MAGIC, 2, UINT32 )
#define	PNG_IOW_DECODE_INIT			_IOW(PNG_IOC_MAGIC, 3, LX_PNG_DECODE_INIT_PARAM_T )
#define PNG_IOW_DECODE_START		_IOR(PNG_IOC_MAGIC, 4, LX_PNG_DECODE_PARAM_T )
#define	PNG_IO_DECODE_STOP			_IO(PNG_IOC_MAGIC,  5 )

#define PNG_IOC_MAXNR				6
/** @} */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum
{
	LX_PNG_DECODE_METHOD_FULL_DECODE	= 0,
	LX_PNG_DECODE_METHOD_PARTIAL_DECODE,
}
LX_PNG_DECODE_METHOD_T;

typedef enum
{
	LX_PNG_DECODE_CAPS_NONE				= 0,
	LX_PNG_DECODE_CAPS_PARTIAL_DECODE	= (1<<0),
}
LX_PNG_DECODE_CAPS_T;

/** PNG image header information
 *	this struct has the same structure as the one described in png spec document
 */
typedef struct
{
    UINT32  width;
    UINT32  height;
    UINT8   bit_depth;
	UINT8	color_type;
    UINT8   compression_method;
    UINT8   filter_method;
    UINT8   interlace_method;
}
LX_PNG_HDR_T;

/** argument for PNG decoder initialization */
typedef struct
{
	UINT32					img_sz;			/** rough image size  */
	LX_PNG_DECODE_METHOD_T	decode_method;	/** decoding method */
	LX_PNG_HDR_T			img_hdr;		/** png header information */
	LX_MEM_BUF_T			out_buf;		/** output buffer to hold decoded data */
}
LX_PNG_DECODE_INIT_PARAM_T;

/** PNG decoder parameter 
 *
 */
typedef struct
{
	UINT32	offset;		/** offset value in mmap data of png buffer */
	UINT32	length;		/** length written in application */
}
LX_PNG_DECODE_PARAM_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PNG_DRV_H_ */

/** @} */
