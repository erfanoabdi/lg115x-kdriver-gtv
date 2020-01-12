
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
 *  application interface header for demod device
 *
 *  author		jeongpil.yun (jeongpil.yun@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *
 *  @addtogroup lg1150_demod
 *	@{
 */

#ifndef	_DEMOD_KAPI_H_
#define	_DEMOD_KAPI_H_

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
#define	DEMOD_IOC_MAGIC		'd'
/**
@name DEMOD IOCTL List
ioctl list for demod device.

@{
@def DEMOD_IO_HW_RESET
Haredware Reset demod module(CTR61[0]).
When demod device receives above IOCTL, then reset itself.

@def DEMOD_IO_I2C_SEL
Set I2C sel( CTR26[15]) for demod.
Set to '0' in normal mode. If set to '1' it is in standalone mode.

@def DEMOD_IOW_INCLK_SEL
Select input clock source(CTR35[0]) for demod.
Set to '0' for ARM setting(default). If set to '1', the external oscillator is used as clock source for demod.

@def DEMOD_IOW_BBMODE_SEL (CTR61[2:1])
control demodulation mode.
if set to '00', the mode is unknown.
if set to '01', the mode is VSB/QAM.
if set to '10', the mode is DVB-T.
if set to '11', the mode is DVB-C.
*/
/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
#define	DEMOD_IO_HW_RESET			_IO(DEMOD_IOC_MAGIC,  0 )
#define DEMOD_IO_I2C_SEL			_IO(DEMOD_IOC_MAGIC,  1  )
#define DEMOD_IOW_INCLK_SEL			_IOW(DEMOD_IOC_MAGIC, 2, LX_DEMOD_INCLK_SEL_T )
#define DEMOD_IOW_BBMODE_SEL		_IOW(DEMOD_IOC_MAGIC, 3, LX_DEMOD_BBMODE_T)
#define DEMOD_IOW_TUNERI2C_SEL		_IOW(DEMOD_IOC_MAGIC, 4, LX_DEMOD_TUNER_I2C_SEL_T )

#define DEMOD_IOC_MAXNR				5
/** @} */

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
 * demod ioctl parameter.
 * useful when complex parameter should be passed to demod kernel driver.
 * @see DEMOD_IOW_WRITE_STRUCT
 */
typedef enum
{
	LX_DEMOD_VSB_QAM = 0,   ///< VSB/QAM  mode is selected
	LX_DEMOD_DVB_T,         ///< DVB-T mode is selected
	LX_DEMOD_DVB_C,        	///< DVB-C mode is selected
	LX_DEMOD_UNKOWN        	///<  UNKOWN
} LX_DEMOD_BBMODE_T;

/**
 * demod ioctl parameter.
 * useful when complex parameter should be passed to demod kernel driver.
 * @see DEMOD_IOW_WRITE_STRUCT
 */
typedef enum
{
	LX_DEMOD_ARM_SETTING = 0,   ///< Selected if internal OSC is used
	LX_DEMOD_EXTERN_PIN         ///< Selected if External OSC is used
} LX_DEMOD_INCLK_SEL_T;

/**
 * demod ioctl parameter.
 * useful when complex parameter should be passed to demod kernel driver.
 * @see DEMOD_IOW_WRITE_STRUCT
 */
typedef enum
{
	LX_DEMOD_CPU = 0,   ///< When the pins( TUN_SDA and TUN_SCL) are used, Select if GBB repeater	is not needed.
	LX_DEMOD_GBB        ///< When the pins( TUN_SDA and TUN_SCL) are used, Select if GBB repeater	is needed
} LX_DEMOD_TUNER_I2C_SEL_T;


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _DEMOD_DRV_H_ */

/** @} */
