
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

/** @file gpio_kapi.h
 *
 *  application interface header for gpio device
 *
 *  @author		ingyu.yang (ingyu.yang@lge.com)
 *  @version		1.0
 *  @date		2009.01.06
 *
 *  @addtogroup lg1150_gpio
 *	@{
 */

#ifndef	_GPIO_KAPI_H_
#define	_GPIO_KAPI_H_

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
#define GPIO_IOC_MAGIC               'g'
/**
@name GPIO IOCTL List
ioctl list for gpio device.

@{
@def LXDRV_GPIO_INIT
Initialize gpio module.
When gpio device receives above IOCTL, then set the register for gpio initialization.

@def LXDRV_GPIO_SET
Set pin to gpio.
When gpio device receives above IOCTL with pin index, then set the pin to gpio.
In this IOCTL, just use the u8Pinnumber parameter in Gpio_Pinset_Param_t structure.


@def LXDRV_GPIO_INOUT
Set pin to input or output.
When gpio device receives above IOCTL with Gpio_Pinset_Param_t, then set the pin to input or output.
If u8flag is "1" then pin is set to the output port.


@def LXDRV_GPIO_READ
Get information from external chip.
When gpio device receives above IOCTL with Gpio_Pinset_Param_t, then get the information from external chip.
In this IOCTL, just use the u8Pinnumber parameter in Gpio_Pinset_Param_t structure.


@def LXDRV_GPIO_PULL_HIGHLOW
Set pin to pull high or low.
When gpio device receives above IOCTL with Gpio_Pinset_Param_t, then set the pin to pull high or low.
If u8flag is "1" then pin is set to the pull high.

*/
/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/**
 * gpio LXDRV_GPIO_INOUT, LXDRV_GPIO_PULL_HIGHLOW parameter.
 * useful when complex parameter should be passed to gpio kernel driver.
 */
 //* @see I2C_IOW_WRITE_STRUCT
 //*/
typedef struct
{
    UINT8 u8Pinnumber;      ///< pin number.
    UINT8 u8Flag;   		///< if "1" is the output, "0" is the input. Also, if "1" is the pull high, "0" is the pull low.
} LX_Gpio_Pinset_Param_t;




/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
#define LXDRV_GPIO_INIT               _IO(GPIO_IOC_MAGIC, 0)
#define LXDRV_GPIO_SET                _IOW(GPIO_IOC_MAGIC, 1, LX_Gpio_Pinset_Param_t)
#define LXDRV_GPIO_INOUT			  _IOW(GPIO_IOC_MAGIC, 2, LX_Gpio_Pinset_Param_t)
#define LXDRV_GPIO_READ               _IOWR(GPIO_IOC_MAGIC, 3, LX_Gpio_Pinset_Param_t)
#define LXDRV_GPIO_PULL_HIGHLOW		  _IOW(GPIO_IOC_MAGIC,4, LX_Gpio_Pinset_Param_t)

#define GPIO_IOC_MAXNR               5
/** @} */

/*----------------------------------------------------------------------------------------
	Macro Definitions
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

#endif /* _GPIO_DRV_H_ */

/** @} */
