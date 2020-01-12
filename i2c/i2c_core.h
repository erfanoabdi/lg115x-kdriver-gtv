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



/*
*******************************************************************************
*          (c) Copyright 2010, LG Electronics, All rights reserved
*
* FILENAME    : i2c_core.h
* AUTHOR(S)   : Murugan
* CREATED     : 15.March.2010
* DESCRIPTION : I2C Device API layer header file
*
*******************************************************************************
*                 REVISION   HISTORY
*
*       DATE            AUTHOR(s)              DESCRIPTION
*   ------------    ------------------        -----------------
*   10.March.2010          Murugan              Created
*******************************************************************************
*/

#ifndef _I2C_CORE_H_
#define _I2C_CORE_H_

#include "base_types.h"
#include "i2c_kapi.h"

/* Read/write mode for the command register */
#define I2C_WRITE_MODE  0x0
#define I2C_READ_MODE   0x1

/*
*******************************************************************************
*                  GLOBAL  MACROS
*******************************************************************************
*/
#define I2C_OK					0
#define I2C_ERR_NOACK			-1
#define I2C_ERR_TIMEOUT			-2

enum
{
	I2C_NACK_LAST_BYTE			= 0x0010,	/* NACK last byte read from slave   */
	I2C_START_BEFORE_READ		= 0x0020,	/* During a read transaction, gen.  */
											/* a repeated START after writing   */
											/* the slave addr(just before read) */
	I2C_STOP_START_BEFORE_READ	= 0x0040,	/* During a read transaction, gen.  */
											/* a STOP & START after writing     */
											/* the slave addr(just before read) */
	I2C_CHECK_EEPROM_WRITE		= 0x0080,	/* Check its internal write cycle   */
};



typedef struct I2C_DEV *LX_I2C_DEV_HANDLE;

/*
*******************************************************************************
* API Prototypes
*******************************************************************************
*/
int I2C_CoreInit(void);
int I2C_CoreGetDeviceNums(void);

LX_I2C_DEV_HANDLE I2C_DevOpen(UINT8 ch);
int I2C_DevClose(LX_I2C_DEV_HANDLE h);
int I2C_DevInit(LX_I2C_DEV_HANDLE h, LX_SW_I2C_GPIO_MAP_T *map);

int I2C_DevSetClock(LX_I2C_DEV_HANDLE h, UINT32 clock);
int I2C_DevTransfer(LX_I2C_DEV_HANDLE h, LX_I2C_RW_DATA_T *data, UINT8 mode);
int I2C_DevSuspendAll(void);
int I2C_DevResumeAll(void);

/* to access internal registers in L9 */
int I2C_DevRegWrite(UINT8 slave, UINT8 reg, UINT8 data, UINT32 retry);
int I2C_DevRegRead(UINT8 slave, UINT8 reg, UINT8* data, UINT32 retry);

#endif

