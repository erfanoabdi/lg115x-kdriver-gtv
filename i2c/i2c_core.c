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
 *  main driver implementation for i2c device.
 *	i2c device will teach you how to make device driver with new platform.
 *
 *  author		Murugan
 *				ks.hyun (ks.hyun@lge.com)
 *  version		1.0
 *  date		2009.4.6
 *  note		Additional information.
 *
 *  @addtogroup lg1150_i2c
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
#include <asm/arch/irqs.h>
#else
#include <mach/irqs.h>
#endif
#include <linux/interrupt.h>
#include <mach/platform.h>

#include "base_types.h"
#include "os_util.h"
#include "i2c_kapi.h"
#include "i2c_regs.h"
#include "i2c_core.h"
#include "i2c_drv.h"
#include "i2c_cfg.h"
#include "ctop_regs.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

#define USE_I2C_IRQ				1		/* use i2c interrupt */
#define USE_INTERNAL_I2C_IRQ	1		/* use i2c interrupt for internal i2c channel */

#define USE_I2C_NEW_TRANSFER			/* process the i2c xfer in irq handler */


/*
EEPROM Writing 후 Staus를 체크 하지 않을 경우 다음 operation에서 에러 발생 하여 추가.
EEPROM Write후 내부 operation이 종료되기 전에 동일한 CH의 다른 IC를 제어 할 경우 문제 발생 .
하드웨어적인 분리가 필요할 듯. User 모드에서 소프트웨어적으로 처리하려면 EEPROM Write후 delay가 필요 함
GP3 코드 수정 하여 error 발생 부분 보정 함.
*/
//#define I2C_EEPROM_CHECK_WRITE_ACK



#ifdef I2C_EEPROM_CHECK_WRITE_ACK
#define I2C_EEPROM_ADDR			0xA0
#define I2C_EEPROM_ADDR_MASK	0xFC
#endif

#define I2C_MAX_FIFO_SIZE		4		/* HW FIFO Size */

#define I2C_POLLING_INTERVAL	50		/* in usecs */
#define I2C_MIN_WAIT_TIME		100		/* 100ms */

#if 0
#define I2C_CORE_DEBUG(format, args...)		I2C_PRINT_ERROR(format, ##args)
#else
#define I2C_CORE_DEBUG(format, args...)		do{}while(0)
#endif

#define I2C_CHECK_INITED(h)												\
	do{																	\
		if(h == NULL) return -1;										\
		if (!h->inited )												\
		{																\
			DBG_PRINT_ERROR("%s is not inited \n", h->name);			\
			return -1;													\
		}																\
	} while(0)


#define I2C_CHECK_PARAM(subAddrSize, bufSize) 											\
	do{																					\
		if ((subAddrSize > 4) || (bufSize == 0) )										\
		{																				\
			DBG_PRINT_ERROR("Invalid parameter : SubAddrLen[%d], BufferLen[%d]\n",		\
				subAddrSize, bufSize );													\
			return -1;																	\
		}																				\
	} while(0)


#define I2C_MAX_NAME		15

#define I2C_MUTEX_INIT(h)		OS_InitMutex(&h->mutex, OS_SEM_ATTR_DEFAULT)
#define I2C_MUTEX_LOCK(h)							\
	do{												\
		if(h->func->lock(&h->control) < 0)			\
		{											\
			DBG_PRINT_ERROR("Mutex Lock Fail\n");	\
			return -1;								\
		}											\
	}while(0)
#define I2C_MUTEX_UNLOCK(h)							\
	do{ 											\
		if(h->func->unlock(&h->control) < 0)		\
		{											\
			DBG_PRINT_ERROR("Mutex Unlock Fail\n");	\
		}											\
	}while(0)


#define I2C_REG_WRITE(h, reg, value)		(h->reg_base->reg = value)
#define I2C_REG_READ(h, reg)				(h->reg_base->reg)


#define I2C_TYPE_HW			0
#define I2C_TYPE_SW			1


#define I2C_ACK				1
#define I2C_NO_ACK			0
#define I2C_NACK			0

#define I2C_FLAG_START			0x01
#define I2C_FLAG_STOP			0x02
#define I2C_FLAG_RESTART		0x04
#define I2C_FLAG_ACK			0x08
#define I2C_FLAG_IGNORE_ACK		0x10


#define I2C_GPIO_HIGH_TIMEOUT		(HZ/10)		// 100ms
#define I2C_GPIO_DELAY(h)			OS_UsecDelay(h->delay)


#define I2C_STATUS_RUNNING		1
#define I2C_STATUS_DONE			0
#define I2C_STATUS_ERROR		-1

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
typedef struct I2C_HW_XFER
{
	UINT32				mode;
	LX_I2C_RW_DATA_T	*data;

	UINT32				step;
	UINT32				fifoSize;
	UINT32				index;
	UINT32				started;

	UINT32				xferMode;
	int					res;
} I2C_HW_XFER_T;

typedef struct I2C_HW
{
	UINT8				ch;
	UINT8				use_irq;
	UINT16				clock_prescale;
	UINT32				clock_value;
	I2C_REG_ADDR_T		reg_base;
	OS_SEM_T			mutex;

	UINT16				poll_interval;

	char				irq_name[I2C_MAX_NAME+1];
	UINT32				irq_num;
	struct completion	irq_completion;

	spinlock_t			irq_lock;
	spinlock_t			xfer_lock;
	UINT32				xfer_started;

#ifdef USE_I2C_NEW_TRANSFER
	I2C_HW_XFER_T		xfer;
#endif
} I2C_HW_T, *I2C_HW_HANDLE;

typedef struct I2C_SW
{
	UINT8				ch;
	UINT8				gpio_inited;
	UINT32				delay;
	OS_SEM_T			mutex;

	UINT8				sda;
	UINT8				scl;

	UINT32				sda_dir_reg;
	UINT32				scl_dir_reg;
	UINT8				sda_dir_mask;
	UINT8				scl_dir_mask;

	UINT32				sda_data_reg;
	UINT32				scl_data_reg;

} I2C_SW_T, *I2C_SW_HANDLE;


typedef union I2C_CTRL
{
	I2C_HW_T		hw;
	I2C_SW_T		sw;
} I2C_CTRL_T, *I2C_CTRL_HANDLE;

typedef struct I2C_DEVICE_FUN
{
	int		(*open)(UINT8 ch, I2C_CTRL_HANDLE control, UINT8 use_irq);
	int		(*close)(I2C_CTRL_HANDLE handle);
	int		(*init)(I2C_CTRL_HANDLE handle, LX_SW_I2C_GPIO_MAP_T *map);
	int		(*setclock)(I2C_CTRL_HANDLE handle, UINT32 clock);
	int		(*write)(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data);
	int		(*read)(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data);

	int		(*lock)(I2C_CTRL_HANDLE handle);
	int		(*unlock)(I2C_CTRL_HANDLE handle);
} I2C_DEVICE_FUN_T;

typedef struct I2C_DEV
{
	UINT8				ch;
	UINT8				inited;
	char				name[I2C_MAX_NAME+1];
	UINT8				clock;

	UINT8				type;
	I2C_DEVICE_FUN_T	*func;

	I2C_CTRL_T			control;
} I2C_DEV_T;

#define hw_control		control.hw
#define sw_control		control.sw

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int _I2C_HwOpen(UINT8 ch, I2C_CTRL_HANDLE control, UINT8 use_irq);
static int _I2C_HwClose(I2C_CTRL_HANDLE handle);
static int _I2C_HwSetClock(I2C_CTRL_HANDLE handle, UINT32 clock);
static int _I2C_HwWrite(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data);
static int _I2C_HwRead(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data);
static int _I2C_HwLock(I2C_CTRL_HANDLE handle);
static int _I2C_HwUnlock(I2C_CTRL_HANDLE handle);
static int _I2C_HwInternalLock(I2C_CTRL_HANDLE handle);
static int _I2C_HwInternalUnlock(I2C_CTRL_HANDLE handle);

static int _I2C_SwOpen(UINT8 ch, I2C_CTRL_HANDLE control, UINT8 use_irq);
static int _I2C_SwClose(I2C_CTRL_HANDLE handle);
static int _I2C_SwInit(I2C_CTRL_HANDLE handle, LX_SW_I2C_GPIO_MAP_T *map);
static int _I2C_SwSetClock(I2C_CTRL_HANDLE handle, UINT32 clock);
static int _I2C_SwWrite(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data);
static int _I2C_SwRead(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data);
static int _I2C_SwLock(I2C_CTRL_HANDLE handle);
static int _I2C_SwUnlock(I2C_CTRL_HANDLE handle);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/* For quick boot implementation */
/* Below register array is for copying the register values when going to suspend mode */
static I2C_REG_T	*astI2CRegArray;

static I2C_DEVICE_FUN_T _stI2cFunc[] =
{
	[I2C_TYPE_HW] =
	{
		.open		= _I2C_HwOpen,
		.close		= _I2C_HwClose,
		.init		= NULL,
		.setclock	= _I2C_HwSetClock,
		.write		= _I2C_HwWrite,
		.read		= _I2C_HwRead,
		.lock		= _I2C_HwLock,
		.unlock		= _I2C_HwUnlock,
	},
	[I2C_TYPE_SW] =
	{
		.open		= _I2C_SwOpen,
		.close		= _I2C_SwClose,
		.init		= _I2C_SwInit,
		.setclock	= _I2C_SwSetClock,
		.write		= _I2C_SwWrite,
		.read		= _I2C_SwRead,
		.lock		= _I2C_SwLock,
		.unlock 	= _I2C_SwUnlock,
	}
};

static I2C_DEVICE_FUN_T _stI2cInternalFunc =
{
	.open		= _I2C_HwOpen,
	.close		= _I2C_HwClose,
	.init		= NULL,
	.setclock	= _I2C_HwSetClock,
	.write		= _I2C_HwWrite,
	.read		= _I2C_HwRead,
	.lock		= _I2C_HwInternalLock,
	.unlock		= _I2C_HwInternalUnlock,
};


static LX_I2C_CFG_T *_pI2cConfig;
static I2C_DEV_T *_pI2cDev;

/* i2c handler for internal register control */
static LX_I2C_DEV_HANDLE _pInternalHandle;

#ifdef USE_I2C_NEW_TRANSFER
static int _I2C_WriteProcess(I2C_HW_HANDLE h);
static int _I2C_ReadProcess(I2C_HW_HANDLE h);
#endif

/*========================================================================================
	Implementation Group
========================================================================================*/

static void _I2C_Reset(I2C_HW_HANDLE h)
{
	I2C_REG_WRITE(h, I2CMCTR1, SOFT_RESET);
	I2C_REG_WRITE(h, I2CMCTR1, CLEAR_FIFOS);
}

static void _I2C_HwRecovery(I2C_HW_HANDLE h)
{
	I2C_REG_WRITE(h, I2CMCTR1, SOFT_RESET);
	I2C_REG_WRITE(h, I2CMCTR1, CLEAR_FIFOS);

	I2C_REG_WRITE (h, I2CMPREH, MSB8(h->clock_prescale));
	I2C_REG_WRITE (h, I2CMPREL, LSB8(h->clock_prescale));
}

static inline void _I2C_WakeUpEvent(I2C_HW_HANDLE h)
{
	complete(&h->irq_completion);
}

static inline int _I2C_WaitforEvent (I2C_HW_HANDLE h, UINT32 timeout)
{
	int rc;
	rc = wait_for_completion_timeout( &h->irq_completion, timeout);
	if(rc == 0) return -ETIME;
	return rc;
}

static inline UINT32 _I2C_GetSubAddr(UINT8 subAddrSize, UINT8 *subAddr)
{
	UINT32 addr = 0;
	UINT8 index = 0;

	for(index=0; index<subAddrSize; index++)
		addr = (addr << 8) | subAddr[index];

	return addr;
}


static irqreturn_t _I2C_IrqHandler(int irq, void *dev)
{
	UINT8 ctr0;
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)dev;
	unsigned long flags;
#ifdef USE_I2C_NEW_TRANSFER
	int done;
	UINT8 status;
	I2C_HW_XFER_T	*xfer;
#endif

	if(h == NULL)
	{
		I2C_PRINT_ERROR("Undefined I2C Interrupt !!! [%d]\n", irq);
		return IRQ_HANDLED;
	}

	ctr0 = I2C_REG_READ(h, I2CMCTR0);
	I2C_REG_WRITE(h, I2CMCTR0, ctr0 | INT_ACK);

	spin_lock_irqsave(&h->xfer_lock, flags);
	if(!h->xfer_started)
	{
		spin_unlock_irqrestore(&h->xfer_lock, flags);
		return IRQ_HANDLED;
	}

#ifdef USE_I2C_NEW_TRANSFER
	done = 0;
	xfer = &h->xfer;

	status = I2C_REG_READ (h, I2CMMS);
	if(xfer->xferMode == I2C_WRITE_MODE)
	{
		if(status & MASTER_STAT_SNB)
		{
			I2C_CORE_DEBUG("No Ack. status=0x%x !!!\n", status);
			xfer->res = I2C_ERR_NOACK;
			done = 1;
		}
	}
	else
	{
		if((status & MASTER_STAT_MFS) != (xfer->fifoSize << 5))
		{
			I2C_PRINT_ERROR("Not matched received buffer. status=0x%x !!!\n", status);
			xfer->res = I2C_ERR_TIMEOUT;
			done = 1;
		}
	}

	if(!done)
	{
		int rc;

		if(xfer->mode == I2C_WRITE_MODE)
			rc = _I2C_WriteProcess(h);
		else
			rc = _I2C_ReadProcess(h);

		if(rc != I2C_STATUS_RUNNING)
		{
			if(rc == I2C_STATUS_ERROR)
				xfer->res = -1;
			else
				xfer->res = I2C_OK;

			done = 1;
		}
	}

	if(done)
	{
		_I2C_WakeUpEvent ( h );
	}
#else
	_I2C_WakeUpEvent ( h );
#endif
	spin_unlock_irqrestore(&h->xfer_lock, flags);

	return IRQ_HANDLED;
}

static void _I2C_UninitISR(I2C_HW_HANDLE h)
{
	free_irq (h->irq_num, NULL);
}

/*
 * Murugan -24.09.2010 - Added for suspend & resume support
 */
int I2C_DevSuspendAll(void)
{
	SINT16	i16Count;

	/* Memorize the clock speed for all channels and restore them back when resume  */
	for (i16Count = 0; i16Count < _pI2cConfig->num_hw_devices; i16Count++)
	{
		I2C_REG_ADDR_T reg_addr = _pI2cDev[i16Count].hw_control.reg_base;
		//Copy to the global array
		memcpy ( (char *)&astI2CRegArray[i16Count], (char *)reg_addr, sizeof(I2C_REG_T) );

		/* Nothing to do for disable i2c */
	}
	return 0;
}

int I2C_DevResumeAll(void)
{
	SINT16	i16Count;

	/* Memorize the clock speed for all channels and restore them back when resume  */
	for (i16Count = 0; i16Count < _pI2cConfig->num_hw_devices; i16Count++)
	{
		I2C_REG_ADDR_T reg_addr = _pI2cDev[i16Count].hw_control.reg_base;

		//Copy to the global array
		memcpy ( (char *)reg_addr, (char *)&astI2CRegArray[i16Count], sizeof(I2C_REG_T) );

		/* Nothing to do for enable i2c */
	}
	return 0;
}



static int _I2C_WaitForCompletion(I2C_HW_HANDLE h, UINT8 numBytes, UINT8 mode)
{
	UINT8 status;
	UINT32 timeout = (numBytes * 9)/h->clock_value + I2C_MIN_WAIT_TIME;

	if(h->use_irq)
	{
		unsigned long flags;

		int rc = _I2C_WaitforEvent(h, timeout);
		spin_lock_irqsave(&h->xfer_lock, flags);
		h->xfer_started = 0;
		spin_unlock_irqrestore(&h->xfer_lock, flags);

		if(rc < 0)
		{
			I2C_PRINT_ERROR("rc=%d, status=0x%x!!!\n", rc, I2C_REG_READ(h, I2CMMS));
			_I2C_HwRecovery(h);
			return I2C_ERR_TIMEOUT;
		}
		status = I2C_REG_READ(h, I2CMMS);
	}
	else
	{
		UINT32 count = (timeout * 1000) / h->poll_interval + 1;

		while(1)
		{
			status = I2C_REG_READ (h, I2CMMS);

			if(status & MASTER_STAT_MIS)
			{
				UINT8 ctr0 = I2C_REG_READ(h, I2CMCTR0);
				I2C_REG_WRITE(h, I2CMCTR0, ctr0 | INT_ACK);
				break;
			}

			if(count == 0)
			{
				I2C_PRINT_ERROR("Timeout. status=0x%x !!!\n", status);
				_I2C_HwRecovery(h);
				return I2C_ERR_TIMEOUT;
			}

			OS_UsecDelay(h->poll_interval);
			count--;
		}
	}

	if(mode == I2C_WRITE_MODE)
	{
		if(status & MASTER_STAT_SNB)
		{
			I2C_CORE_DEBUG("No Ack. status=0x%x !!!\n", status);
			return I2C_ERR_NOACK;
		}

		if((status & MASTER_STAT_MFS))
		{
			I2C_CORE_DEBUG("Buffer is not empty. status=0x%x !!!\n", status);
		}
	}
	else
	{
		if((status & MASTER_STAT_MFS) != (numBytes << 5))
		{
			I2C_CORE_DEBUG("Buffer is not correct. status=0x%x !!!\n", status);
		}
	}

	return 0;
}



static int _I2C_FifoStart(I2C_HW_HANDLE h, UINT8 size, UINT8 ctr0)
{
	UINT8 ntb;

	switch(size)
	{
		case 1: ntb = NTB_1_BYTE; break;
		case 2: ntb = NTB_2_BYTE; break;
		case 3: ntb = NTB_3_BYTE; break;
		case 4: ntb = NTB_4_BYTE; break;
		default: return -1;
	}

	I2C_REG_WRITE (h, I2CMCTR0, ntb | ctr0);
	return 0;
}

#ifdef USE_I2C_NEW_TRANSFER
static int _I2C_ReadProcess(I2C_HW_HANDLE h)
{
	I2C_HW_XFER_T *xfer = &h->xfer;
	LX_I2C_RW_DATA_T *data = xfer->data;
	UINT32	status;
	UINT8	ctr0;
	int		res = I2C_STATUS_RUNNING;

start:
	switch(xfer->step)
	{
		case 0:
			status = I2C_REG_READ (h, I2CMMS);
			if((status & MASTER_STAT_MTB))
			{
				// If there's no interrupt in previous cmd then this status is happend,
				// but have to ignore this.
				I2C_CORE_DEBUG("master is busy... ch=%d, slave=0x%02x, status=0x%x\n",
								h->ch, data->slaveAddr, status);
			}

			//Set the mode to master
			I2C_REG_WRITE (h, I2CMCTR1, CLEAR_FIFOS |INT_ENABLE | CORE_ENABLE);

			xfer->fifoSize = 0;
			xfer->index = 0;
			xfer->started = 0;

			if(data->subAddrSize == 0)
			{
				xfer->step = 2;
				goto start;
			}

			/* Send the slave address along with the write mode bit set */
			I2C_REG_WRITE (h, I2CMMD, data->slaveAddr | I2C_WRITE_MODE);
			xfer->fifoSize++;
			xfer->step++;

		case 1:
			/* send the sub address */
			while(xfer->index < data->subAddrSize)
			{
				//Write the register address to read from
				I2C_REG_WRITE (h, I2CMMD, data->subAddr[xfer->index]);
				xfer->fifoSize++;
				xfer->index++;

				if(xfer->fifoSize == I2C_MAX_FIFO_SIZE ||
					xfer->index == data->subAddrSize)
				{
					ctr0 = INT_ACK | TRANSFER_GO;
					if(xfer->index == data->subAddrSize)
					{
						if(data->flag & I2C_STOP_START_BEFORE_READ)
							ctr0 |= SET_STOP_SEQ;
						else if(data->flag & I2C_START_BEFORE_READ)
							ctr0 |= RESTART_SEQ;
					}

					if(!xfer->started)
					{
						ctr0 |= SET_START_SEQ;
						xfer->started = 1;
					}

					xfer->xferMode = I2C_WRITE_MODE;
					if(_I2C_FifoStart(h, xfer->fifoSize, ctr0) < 0)
						goto error;

					xfer->fifoSize = 0;
					goto end;
				}
			}

			xfer->index = 0;
			xfer->step++;

		case 2:
			if(data->subAddrSize > 0)
			{
				/* Send the slave address along with the readmode bit set */
				I2C_REG_WRITE (h, I2CMMD, data->slaveAddr | I2C_READ_MODE);

				ctr0 = INT_ACK | TRANSFER_GO;

				if( data->flag & (I2C_STOP_START_BEFORE_READ) )
				{
					ctr0 |= SET_START_SEQ;
				}

			}
			else
			{
				/* Send the slave address along with the readmode bit set */
				I2C_REG_WRITE (h, I2CMMD, data->slaveAddr | I2C_READ_MODE);
				ctr0 = INT_ACK | TRANSFER_GO | SET_START_SEQ;
			}
			xfer->fifoSize = 1;
			xfer->xferMode = I2C_WRITE_MODE;
			if(_I2C_FifoStart(h, xfer->fifoSize, ctr0) < 0)
				goto error;

			xfer->fifoSize = 0;
			xfer->step++;
			break;

		case 3:
			/* Change the I2C ip to read mode to read from the slave */
			I2C_REG_WRITE (h, I2CMCTR1, TRANS_MASTER_RX | INT_ENABLE | CORE_ENABLE | CLEAR_FIFOS);
			xfer->step++;

		case 4:
			while(xfer->index < data->bufSize)
			{
				if((xfer->index+I2C_MAX_FIFO_SIZE) < data->bufSize)
				{
					xfer->fifoSize = I2C_MAX_FIFO_SIZE;
					ctr0 = INT_ACK | TRANSFER_GO;
				}
				else
				{
					xfer->fifoSize = data->bufSize - xfer->index;
					ctr0 = INT_ACK | SET_STOP_SEQ | TRANSFER_GO;
				}

				xfer->xferMode = I2C_READ_MODE;
				if(_I2C_FifoStart(h, xfer->fifoSize, ctr0) < 0)
					goto error;

				xfer->step = 5;
				goto end;
			}

		case 5:
			while(xfer->fifoSize--)
			{
				data->buf[xfer->index++] = I2C_REG_READ (h, I2CMMD);
			}

			if(xfer->index < data->bufSize)
			{
				xfer->step = 4;
				goto start;
			}
			res = I2C_STATUS_DONE;
			break;

	}
end:
	return res;

error:
	I2C_PRINT_ERROR("step[%d], slave[%02x]\n", xfer->step, data->slaveAddr);
	return I2C_STATUS_ERROR;
}


static int _I2C_WriteProcess(I2C_HW_HANDLE h)
{
	I2C_HW_XFER_T *xfer = &h->xfer;
	LX_I2C_RW_DATA_T *data = xfer->data;
	UINT32	status;
	UINT8	ctr0;
	int		res = I2C_STATUS_RUNNING;

	switch(xfer->step)
	{
		case 0:
			status = I2C_REG_READ (h, I2CMMS);
			if((status & MASTER_STAT_MTB))
			{
				// If there's no interrupt in previous cmd then this status is happend,
				// but have to ignore this.
				I2C_CORE_DEBUG("master is busy... ch=%d, slave=0x%02x, status=0x%x\n",
								h->ch, data->slaveAddr, status);
			}
			//Set the mode to master tx, clear fifos and enable int & core
			I2C_REG_WRITE (h, I2CMCTR1, CLEAR_FIFOS |INT_ENABLE | CORE_ENABLE);

			/* Send the slave address along with the write mode bit set */
			I2C_REG_WRITE (h, I2CMMD, data->slaveAddr | I2C_WRITE_MODE);
			xfer->fifoSize = 1;
			xfer->index = 0;
			xfer->started = 0;
			xfer->xferMode = I2C_WRITE_MODE;
			xfer->step++;

		case 1:
			/* Send sub address if it has. */
			while(xfer->index < data->subAddrSize)
			{
				//Write the register address to write
				I2C_REG_WRITE (h, I2CMMD, data->subAddr[xfer->index]);
				xfer->fifoSize++;
				xfer->index++;
				if(xfer->fifoSize == I2C_MAX_FIFO_SIZE)
				{
					ctr0 = INT_ACK | TRANSFER_GO;
					if(!xfer->started)
					{
						ctr0 |= SET_START_SEQ;
						xfer->started = 1;
					}

					if(_I2C_FifoStart(h, xfer->fifoSize, ctr0) < 0)
						goto error;

					xfer->fifoSize = 0;
					goto end;
				}
			}
			xfer->index = 0;
			xfer->step++;

		case 2:
			/* Send the data here */
			while(xfer->index < data->bufSize)
			{
				//Write the data
				I2C_REG_WRITE (h, I2CMMD, data->buf[xfer->index]);
				xfer->fifoSize++;
				xfer->index++;
				if(xfer->fifoSize == I2C_MAX_FIFO_SIZE ||
					xfer->index == data->bufSize)
				{
					if(xfer->index == data->bufSize)
						ctr0 = INT_ACK | TRANSFER_GO | SET_STOP_SEQ;
					else
						ctr0 = INT_ACK | TRANSFER_GO;

					if(!xfer->started)
					{
						ctr0 |= SET_START_SEQ;
						xfer->started = 1;
					}

					if(_I2C_FifoStart(h, xfer->fifoSize, ctr0) < 0)
						goto error;

					xfer->fifoSize = 0;
					goto end;
				}
			}

			res = I2C_STATUS_DONE;
			break;

		default:
			goto error;
	}
end:
	return res;

error:
	I2C_PRINT_ERROR("step[%d], slave[%02x]\n", xfer->step, data->slaveAddr);
	return I2C_STATUS_ERROR;
}

static int _I2C_NewTransfer(I2C_HW_HANDLE h, LX_I2C_RW_DATA_T *data, UINT8 mode)
{
	int rc;
	I2C_HW_XFER_T *xfer = &h->xfer;
	unsigned long flags;

	spin_lock_irqsave(&h->xfer_lock, flags);

	memset(xfer, 0, sizeof(*xfer));
	xfer->data = data;
	xfer->mode = mode;

	INIT_COMPLETION(h->irq_completion);

//I2C_PRINT_ERROR("%s.CH[%d], Slave:%02x\n", mode == I2C_WRITE_MODE ? "write" : "read", h->ch, data->slaveAddr);

	rc = (mode == I2C_WRITE_MODE) ?	_I2C_WriteProcess(h) : _I2C_ReadProcess(h);

	if(rc == I2C_STATUS_RUNNING)
	{
		UINT32 timeout = ((data->subAddrSize + data->bufSize) * 9)/h->clock_value + I2C_MIN_WAIT_TIME;

		h->xfer_started = 1;
		spin_unlock_irqrestore(&h->xfer_lock, flags);
		/* it will wait until the end of operation */
		rc = _I2C_WaitforEvent(h, timeout);

		spin_lock_irqsave(&h->xfer_lock, flags);
		h->xfer_started = 0;
		spin_unlock_irqrestore(&h->xfer_lock, flags);

		if(rc < 0)
		{
			_I2C_HwRecovery(h);
			return -1;
		}

		rc = (xfer->res < 0) ? xfer->res : data->bufSize;
		return rc;
	}
	else
	{
		spin_unlock_irqrestore(&h->xfer_lock, flags);

		I2C_PRINT_ERROR("_I2C_NewTransfer....\n");
		return -1;
	}
}
#endif

static int _I2C_ProcessFifoData(I2C_HW_HANDLE h, UINT8 size, UINT8 ctr0, UINT8 mode)
{
	unsigned long flags;

	if(h->use_irq)
	{
		spin_lock_irqsave(&h->xfer_lock, flags);
		INIT_COMPLETION(h->irq_completion);
		h->xfer_started = 1;
		_I2C_FifoStart(h, size, ctr0);
		spin_unlock_irqrestore(&h->xfer_lock, flags);
	}
	else
	{
		_I2C_FifoStart(h, size, ctr0);
	}

	//Wait for interrupt
	if(_I2C_WaitForCompletion( h, size, mode ) < 0)
	{
		return -1;
	}

	return 0;
}


static int _I2C_ReadCmd(I2C_HW_HANDLE h, UINT8 slaveAddr, UINT8 subAddrSize,
						UINT8 *subAddr, UINT8 *buf, UINT32 bufSize, UINT32 option)
{
	int res = -1;
	UINT32 index;
	UINT8 status;
	UINT8 fifoSize = 0;
	UINT8 sta = SET_START_SEQ;
	UINT8 ctr0;

	status = I2C_REG_READ (h, I2CMMS);
	if((status & MASTER_STAT_MTB))
	{
		// If there's no interrupt in previous cmd then this status is happend,
		// but have to ignore this.
		I2C_CORE_DEBUG("master is busy... ch=%d, slave=0x%02x, status=0x%x\n",
						h->ch, slaveAddr, status);
	}

	//Set the mode to master
	I2C_REG_WRITE (h, I2CMCTR1,CLEAR_FIFOS |INT_ENABLE | CORE_ENABLE);


	if (subAddrSize > 0)
	{
		/* Send the slave address along with the write mode bit set */
		I2C_REG_WRITE (h, I2CMMD, slaveAddr | I2C_WRITE_MODE);
		fifoSize++;

		for(index = 0; index<subAddrSize; index++)
		{
			//Write the register address to read from
			I2C_REG_WRITE (h, I2CMMD, subAddr[index]);
			fifoSize++;

			if(fifoSize == 4 || (index+1) == subAddrSize)
			{
				ctr0 = INT_ACK | TRANSFER_GO;
				if((index+1) == subAddrSize)
				{
					if(option & I2C_STOP_START_BEFORE_READ)
						ctr0 |= SET_STOP_SEQ;
					else if(option & I2C_START_BEFORE_READ)
						ctr0 |= RESTART_SEQ;
				}

				if(sta)
				{
					ctr0 |= sta;
					sta = 0;
				}

				//Wait for interrupt
				if(_I2C_ProcessFifoData(h, fifoSize, ctr0, I2C_WRITE_MODE) < 0)
				{
					I2C_CORE_DEBUG("SlaveAddr=0x%02x, subAddrSize=%d, subAddr=0x%0x, bufSize=%d, index=%d\n",
						slaveAddr, subAddrSize, _I2C_GetSubAddr(subAddrSize, subAddr), bufSize, index);
					goto end;
				}
				fifoSize = 0;
			}
		}


		/* Send the slave address along with the readmode bit set */
		I2C_REG_WRITE (h, I2CMMD, slaveAddr | I2C_READ_MODE);

		ctr0 = INT_ACK | TRANSFER_GO;
		if(option & I2C_STOP_START_BEFORE_READ)
			ctr0 |= SET_START_SEQ;

	}
	else
	{
		/* Send the slave address along with the readmode bit set */
		I2C_REG_WRITE (h, I2CMMD, slaveAddr | I2C_READ_MODE);
		ctr0 = INT_ACK | TRANSFER_GO | SET_START_SEQ;
	}

	if(_I2C_ProcessFifoData(h, 1, ctr0, I2C_WRITE_MODE) < 0)
	{
		I2C_CORE_DEBUG("SlaveAddr=0x%02x, subAddrSize=%d, subAddr=0x%0x, bufSize=%d, index=%d\n",
						slaveAddr, subAddrSize, _I2C_GetSubAddr(subAddrSize, subAddr), bufSize, index);
		goto end;
	}

	/* Change the I2C ip to read mode to read from the slave */
	I2C_REG_WRITE (h, I2CMCTR1, TRANS_MASTER_RX | INT_ENABLE | CORE_ENABLE | CLEAR_FIFOS);

	index = 0;
	while(index < bufSize)
	{
		if((index+I2C_MAX_FIFO_SIZE) < bufSize)
		{
			fifoSize = I2C_MAX_FIFO_SIZE;
			ctr0 = INT_ACK | TRANSFER_GO;
		}
		else
		{
			fifoSize = bufSize - index;
			ctr0 = INT_ACK | SET_STOP_SEQ | TRANSFER_GO;
		}

		if(_I2C_ProcessFifoData(h, fifoSize, ctr0, I2C_READ_MODE) < 0)
		{
			I2C_PRINT_ERROR("SlaveAddr=0x%02x, subAddrSize=%d, subAddr=0x%0x, bufSize=%d, index=%d\n",
					slaveAddr, subAddrSize, _I2C_GetSubAddr(subAddrSize, subAddr), bufSize, index);
			goto end;
		}

		while(fifoSize--)
		{
			buf [index++] = I2C_REG_READ (h, I2CMMD);
		}
	}

	res = bufSize;

end:
	return res;
}


static int _I2C_WriteCmd(I2C_HW_HANDLE h, UINT8 slaveAddr, UINT8 subAddrSize,
							UINT8 *subAddr, UINT8 *buf, UINT32 bufSize, UINT32 option)
{
	int res = -1;
	UINT32 index;
	UINT8 status;
	UINT8 fifoSize = 0;
	UINT8 sta = SET_START_SEQ;
	UINT8 ctr0;

	status = I2C_REG_READ (h, I2CMMS);
	if((status & MASTER_STAT_MTB))
	{
		// If there's no interrupt in previous cmd then this status is happend,
		// but have to ignore this.
		I2C_CORE_DEBUG("master is busy... ch=%d, slave=0x%02x, status=0x%x\n",
						h->ch, slaveAddr, status);
	}

	//Set the mode to master tx, clear fifos and enable int & core
	I2C_REG_WRITE (h, I2CMCTR1, CLEAR_FIFOS |INT_ENABLE | CORE_ENABLE);

	/* Send the slave address along with the write mode bit set */
	I2C_REG_WRITE (h, I2CMMD, slaveAddr | I2C_WRITE_MODE);
	fifoSize++;

	//Send sub address if it has.
	if(subAddrSize > 0)
	{
		for(index = 0; index<subAddrSize; index++)
		{
			//Write the register address to write
			I2C_REG_WRITE (h, I2CMMD, subAddr[index]);
			fifoSize++;

			if(fifoSize == I2C_MAX_FIFO_SIZE)
			{
				ctr0 = INT_ACK | TRANSFER_GO;
				if(sta)
				{
					ctr0 |= sta;
					sta = 0;
				}

				if(_I2C_ProcessFifoData(h, fifoSize, ctr0, I2C_WRITE_MODE) < 0)
				{
					I2C_CORE_DEBUG("Slave=0x%02x, SubAddrSize=%d, SubAddr=0x%0x, BufSize=%d, index=%d\n",
							slaveAddr, subAddrSize, _I2C_GetSubAddr(subAddrSize, subAddr), bufSize, index);
					goto end;
				}
				fifoSize = 0;
			}
		}
	}

	/* Send the data here */
	for(index = 0; index<bufSize; index++)
	{
		//Write the data
		I2C_REG_WRITE (h, I2CMMD, buf[index]);
		fifoSize++;

		if(fifoSize == 4 || (index+1) == bufSize)
		{
			if((index+1) == bufSize)
				ctr0 = INT_ACK | TRANSFER_GO | SET_STOP_SEQ;
			else
				ctr0 = INT_ACK | TRANSFER_GO;

			if(sta)
			{
				ctr0 |= sta;
				sta = 0;
			}

			//Wait for interrupt
			if(_I2C_ProcessFifoData(h, fifoSize, ctr0, I2C_WRITE_MODE) < 0)
			{
				I2C_CORE_DEBUG("Slave=0x%02x, SubAddrSize=%d, SubAddr=0x%0x, BufSize=%d, index=%d\n",
					slaveAddr, subAddrSize, _I2C_GetSubAddr(subAddrSize, subAddr), bufSize, index);
				goto end;
			}
			fifoSize = 0;
		}
	}
	res = bufSize;

	if(option&I2C_CHECK_EEPROM_WRITE)
	{
		UINT8 dummy[1];
		UINT32 timeout = ((bufSize+1) * 4000)/h->clock_value + I2C_MIN_WAIT_TIME;
		UINT32 count = (timeout * 1000) / I2C_POLLING_INTERVAL + 1;

		while(1)
		{
			if(_I2C_ReadCmd(h, slaveAddr, subAddrSize, subAddr, dummy, 1, I2C_START_BEFORE_READ) < 0)
			{
				I2C_PRINT_ERROR("Waiting Stable...count=%d!!!\n", count);
				OS_UsecDelay(I2C_POLLING_INTERVAL);
				count--;

				if(count == 0)
				{
					I2C_DEBUG("EEPROM Write. Waiting Timeout.....\n");
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

end:

	return res;
}

static int _I2C_HwOpen(UINT8 ch, I2C_CTRL_HANDLE control, UINT8 use_irq)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)control;

	if(h == NULL)
		return -1;

	h->ch = ch;
	h->reg_base = (I2C_REG_ADDR_T)ioremap(_pI2cConfig->hw_map[ch].reg_addr, I2C_REG_SIZE);
	_I2C_Reset(h);

	I2C_MUTEX_INIT(h);

	spin_lock_init(&h->irq_lock);
	spin_lock_init(&h->xfer_lock);
	h->xfer_started = 0;

	h->use_irq = use_irq;
	if(h->use_irq)
	{
		/* CAUTION !!!.
		 * if the i2c controller was used before kernel booting like uboot,
		 * the irq handler is called during rquesting irq.
		 */
		I2C_REG_WRITE (h, I2CMCTR0, INT_ACK);

		h->irq_num = _pI2cConfig->hw_map[ch].irq;
		sprintf(h->irq_name, "I2C IRQ[%d]", ch);
		if(request_irq( h->irq_num, _I2C_IrqHandler, IRQF_DISABLED, h->irq_name, h))
		{
			I2C_PRINT_ERROR("IRQ[%d] request error\n", h->irq_num);
			return -1;
		}

		init_completion(&h->irq_completion);
	}

	return 0;
}


static int _I2C_HwClose(I2C_CTRL_HANDLE handle)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;

	if(h->use_irq)
	{
		_I2C_UninitISR(h);
	}
	_I2C_Reset(h);

	return 0;
}

static int _I2C_HwSetClock(I2C_CTRL_HANDLE handle, UINT32 clock)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	UINT16 clockPrescale;
	float fclkNanoSec;


	if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{
		/* How to calculate the prescale value ?
		 * prescale = 198MHZ(PCLK) / (clock KHZ / 100)
		 *
		 * ex) 1MHZ : 198 / (1000 / 100) = 19.8 => 0x14
		 */
		switch ( clock )
		{
			case I2C_CLOCK_50KHZ:
				clockPrescale = 0xFF; /*real i2c minimal clock is 80MHz*/
				h->clock_value = 50;
				break;
			case I2C_CLOCK_200KHZ:
				clockPrescale = 0x68;
				h->clock_value = 200;
				break;
			case I2C_CLOCK_400KHZ:
				clockPrescale = 0x37;
				h->clock_value = 400;
				break;
			case I2C_CLOCK_1MHZ:
				clockPrescale = 0x14;
				h->clock_value = 1000;
				break;
			case I2C_CLOCK_2MHZ:
				clockPrescale = 0xA;
				h->clock_value = 2000;
				break;
			case I2C_CLOCK_4MHZ:
				clockPrescale = 0x5;
				h->clock_value = 4000;
				break;
			case I2C_CLOCK_8MHZ:
				clockPrescale = 0x3;
				h->clock_value = 8000;
				break;
			case I2C_CLOCK_100KHZ:
			default:
				clockPrescale = 0xD5;
				h->clock_value = 100;
				break;
		}
	}
	else
	{
		fclkNanoSec = 1000000000.0 / I2C_CLOCK_FREQUENCY_L8;

		switch ( clock )
		{
			case I2C_CLOCK_50KHZ:
				clockPrescale = (2000/fclkNanoSec); //50KHz
				h->clock_value = 50;
				break;
			case I2C_CLOCK_200KHZ:
				clockPrescale = (500/fclkNanoSec);	//200KHz
				h->clock_value = 200;
				break;
			case I2C_CLOCK_400KHZ:
				clockPrescale = (250/fclkNanoSec);		//400KHz
				h->clock_value = 400;
				break;
			case I2C_CLOCK_100KHZ:
			default:
				clockPrescale = (1000/fclkNanoSec); //100KHz
				h->clock_value = 100;
				break;
		}
	}
	h->clock_prescale = clockPrescale;
	//h->poll_interval = 8000/h->clock_value;
	h->poll_interval = 1;

	//I2C_PRINT_ERROR("CH[%d], CLOCK[%d], PRESCALE[%d][0x%x:0x%x]\n", h->ch, h->clock_value, clockPrescale, MSB8(clockPrescale), LSB8(clockPrescale));

	I2C_REG_WRITE (h, I2CMPREH, MSB8(clockPrescale));
	I2C_REG_WRITE (h, I2CMPREL, LSB8(clockPrescale));

	return 0;
}


#define HW_START_XMIT(h,flag)			\
	do{									\
		flag = h->use_irq;				\
		if(in_interrupt() && flag){		\
			disable_irq(h->irq_num);	\
			h->use_irq = 0;				\
		}								\
	}while(0)

#define HW_END_XMIT(h,flag)				\
	do{ 								\
		if(in_interrupt() && flag){ 	\
			h->use_irq = 1; 			\
			enable_irq(h->irq_num);		\
		}								\
	}while(0)

static int _I2C_HwWrite(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	int res;
	UINT32 flag;

	HW_START_XMIT(h,flag);
#ifdef	USE_I2C_NEW_TRANSFER
	if(h->use_irq)
		res = _I2C_NewTransfer(h, data, I2C_WRITE_MODE);
	else
		res = _I2C_WriteCmd(h, data->slaveAddr, data->subAddrSize, data->subAddr,
							data->buf, data->bufSize, data->flag);
#else
	res = _I2C_WriteCmd(h, data->slaveAddr, data->subAddrSize, data->subAddr,
						data->buf, data->bufSize, data->flag);
#endif
	HW_END_XMIT(h,flag);

	return res;
}

static int _I2C_HwRead(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	int res;
	UINT32 flag;

	HW_START_XMIT(h,flag);
#ifdef	USE_I2C_NEW_TRANSFER
	if(h->use_irq)
		res = _I2C_NewTransfer(h, data, I2C_READ_MODE);
	else
		res = _I2C_ReadCmd(h, data->slaveAddr, data->subAddrSize, data->subAddr,
							data->buf, data->bufSize, data->flag);
#else
	res = _I2C_ReadCmd(h, data->slaveAddr, data->subAddrSize, data->subAddr,
						data->buf, data->bufSize, data->flag);
#endif
	HW_END_XMIT(h,flag);

	return res;
}

static int _I2C_HwLock(I2C_CTRL_HANDLE handle)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	return OS_LockMutex(&h->mutex);
}

static int _I2C_HwUnlock(I2C_CTRL_HANDLE handle)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	OS_UnlockMutex(&h->mutex);
	return 0;
}

static int _I2C_HwInternalLock(I2C_CTRL_HANDLE handle)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	int rc = 0;

	if(in_interrupt())
	{
		/* irqs be able to enter here must be disabled in normal mode */
		spin_lock(&h->irq_lock);
	}
	else
	{
		rc = OS_LockMutex(&h->mutex);

		/* disable_irq waits for any peding IRQ handlers for this interrupt to complete before returning */
		disable_irq(L9_IRQ_HDMI1);
	}

	return rc;
}

static int _I2C_HwInternalUnlock(I2C_CTRL_HANDLE handle)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;

	if(in_interrupt())
	{
		spin_unlock(&h->irq_lock);
	}
	else
	{
		enable_irq(L9_IRQ_HDMI1);
		OS_UnlockMutex(&h->mutex);
	}
	return 0;
}


/*
*************************************************************************
* I2C GPIO - START
*************************************************************************
*/
static UINT32 _I2C_GetGpioDataReg(UINT8 num)
{
	UINT32 topAddr;
	UINT32 subAddr;

	topAddr = GPIO_DATA_REG(num/8);

	if(_pI2cConfig->chip < LX_CHIP_REV(L8,B0))
	{
		if(num < 32)		{ subAddr = 1 << (2 + (num % 8)); }
		// for work-around : Top control register connection error - start
		else if(num == 81)	{ subAddr = 0x40 << 2; }
		else if(num == 82)	{ subAddr = 0x4  << 2; }
		else if(num == 83)	{ subAddr = 0x10 << 2; }
		else if(num == 84)	{ subAddr = 0x20 << 2; }
		else if(num == 85)	{ subAddr = 0x8	<< 2; }
		// for work-around : Top control register connection error - end
		else				{ subAddr = 1 << (2 + (7-( num % 8))); }
	}
	else
	{
		if(num < 32) { subAddr = 1 << (2 + (num % 8)); }
		else		 { subAddr = 1 << (2 + (7-( num % 8))); }
	}

	return (topAddr | subAddr);
}

static UINT32 _I2C_GetGpioDirReg(UINT8 num)
{
	return GPIO_DIR_REG(num/8);
}

static UINT8 _I2C_GetGpioDirMask(UINT8 num)
{
	UINT8 mask;

	if(_pI2cConfig->chip < LX_CHIP_REV(L8,B0))
	{
		if(num < 32) 		{ mask = 1 << (num % 8); }
		// for work-around : Top control register connection error - start
		else if(num == 81)	{ mask = 0x40; }
		else if(num == 82)	{ mask = 0x4; }
		else if(num == 83)	{ mask = 0x10; }
		else if(num == 84)	{ mask = 0x20; }
		else if(num == 85)	{ mask = 0x8; }
		// for work-around : Top control register connection error - end
		else				{ mask = 1 << (7 - (num % 8)); }
	}
	else
	{
		if(num < 32) 		{ mask = 1 << (num % 8); }
		else				{ mask = 1 << (7 - (num % 8)); }
	}

	return mask;
}

static void _I2C_SetGpioTop(UINT8 num)
{
	UINT32 reg;
	UINT32 mask;

	if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{
		return;
	}
	else
	{
		if(num < 32) return;
		else if(num < 32 * 2){ reg = L8_GPIO_TOP_CTRL_32_63_REG; }
		else if(num < 32 * 3){ reg = L8_GPIO_TOP_CTRL_64_95_REG; }
		else return;
	}

	if(_pI2cConfig->chip < LX_CHIP_REV(L8,B0))
	{
		// for work-around : Top control register connection error - start
		if(num == 81)		{ mask = 0x4000; }
		else if(num == 82)	{ mask = 0x400; }
		else if(num == 83)	{ mask = 0x1000; }
		else if(num == 84)	{ mask = 0x2000; }
		else if(num == 85)	{ mask = 0x800; }
		// for work-around : Top control register connection error - end
		else				{ mask = 1 << ( 31 - (num % 32)); }
	}
	else
	{
		mask = 1 << ( 31 - (num % 32));
	}

	REG8_BIT_SET(reg, mask);

}

static inline void _I2C_SetGpioSdaToInput(I2C_SW_HANDLE h)
{
	REG8_BIT_CLEAR(h->sda_dir_reg, h->sda_dir_mask); 	// set to input
}

static inline void _I2C_SetGpioSclToInput(I2C_SW_HANDLE h)
{
	REG8_BIT_CLEAR(h->scl_dir_reg, h->scl_dir_mask); 	// set to input
}

static inline int _I2C_GetGpioSda(I2C_SW_HANDLE h)
{
	return REG8_READ(h->sda_data_reg) ? I2C_HIGH : I2C_LOW;
}

static inline int _I2C_GetGpioScl(I2C_SW_HANDLE h)
{
	return REG8_READ(h->scl_data_reg) ? I2C_HIGH : I2C_LOW;
}

static int _I2C_SetGpioScl(I2C_SW_HANDLE h, UINT8 clk)
{
	if(clk == I2C_HIGH)
	{
		unsigned long start = jiffies;

		REG8_BIT_CLEAR(h->scl_dir_reg, h->scl_dir_mask);		// set to input

		while(_I2C_GetGpioScl(h) != I2C_HIGH)
		{
			if(time_after_eq(jiffies, start + I2C_GPIO_HIGH_TIMEOUT))
			{
				I2C_PRINT_ERROR("Can't set the clock...\n");
				return -1;
			}
		}
	}
	else
	{
		/*When you chagne the in/out mode to output, the output value is the previous input value.
		  After setting the output mode, set the output value.
		*/
		REG8_BIT_SET(h->scl_dir_reg, h->scl_dir_mask);		// set to output
		REG8_WRITE(h->scl_data_reg, I2C_GPIO_LOW);
	}

	return 0;
}

static int _I2C_SetGpioSda(I2C_SW_HANDLE h, UINT8 data)
{
	if(data == I2C_HIGH)
	{
		unsigned long start = jiffies;

		REG8_BIT_CLEAR(h->sda_dir_reg, h->sda_dir_mask);		// set to input

		while(_I2C_GetGpioSda(h) != I2C_HIGH)
		{
			if(time_after_eq(jiffies, start + I2C_GPIO_HIGH_TIMEOUT))
			{
				I2C_PRINT_ERROR("Can't set the data...\n");
				return -1;
			}
		}
	}
	else
	{
		/*When you chagne the in/out mode to output, the output value is the previous input value.
		  After setting the output mode, set the output value.
		 */
		REG8_BIT_SET(h->sda_dir_reg, h->sda_dir_mask);		// set to output
		REG8_WRITE(h->sda_data_reg, I2C_GPIO_LOW);
	}

	return 0;
}

static void _I2C_GpioReset(I2C_SW_HANDLE h)
{
	_I2C_SetGpioSclToInput(h);
	_I2C_SetGpioSdaToInput(h);
}

static int _I2C_GpioStart(I2C_SW_HANDLE h)
{
	// reset clock & data
	if(_I2C_SetGpioScl(h, I2C_HIGH) < 0)
		return -1;

	if(_I2C_SetGpioSda(h, I2C_HIGH) < 0)
		return -1;

	I2C_GPIO_DELAY(h);

	_I2C_SetGpioSda(h, I2C_LOW);
	I2C_GPIO_DELAY(h);

	return 0;
}

static int _I2C_GpioRepeatStart(I2C_SW_HANDLE h)
{
	if(_I2C_SetGpioScl(h, I2C_HIGH) < 0)
		return -1;
	I2C_GPIO_DELAY(h);

	_I2C_SetGpioSda(h, I2C_LOW);
	I2C_GPIO_DELAY(h);

	return 0;
}

static int _I2C_GpioStop(I2C_SW_HANDLE h)
{
	_I2C_SetGpioSda(h, I2C_LOW);
	I2C_GPIO_DELAY(h);

	_I2C_SetGpioScl(h, I2C_HIGH);
	I2C_GPIO_DELAY(h);

	_I2C_SetGpioSda(h, I2C_HIGH);

	return 0;
}

static int _I2C_GpioByteWrite(I2C_SW_HANDLE h, UINT8 data, UINT32 flag)
{
	UINT8 i;
	UINT8 ack;

	if((flag&I2C_FLAG_START))
	{
		if(_I2C_GpioStart(h) < 0)
			return -1;
	}
	else if((flag&I2C_FLAG_RESTART))
	{
		if(_I2C_GpioRepeatStart(h) < 0)
			return -1;
	}

	for(i=0; i<8; i++)
	{
		_I2C_SetGpioScl(h, I2C_LOW);
		_I2C_SetGpioSda(h, (data&0x80) ? I2C_HIGH : I2C_LOW);

		I2C_GPIO_DELAY(h);
		_I2C_SetGpioScl(h, I2C_HIGH);

		I2C_GPIO_DELAY(h);
		data <<= 1;
	}

	_I2C_SetGpioScl(h, I2C_LOW);
	_I2C_SetGpioSdaToInput(h);

	I2C_GPIO_DELAY(h);
	_I2C_SetGpioScl(h, I2C_HIGH);
	//read ack bit
	if((flag|I2C_FLAG_IGNORE_ACK))
		ack = I2C_ACK;
	else
		ack = _I2C_GetGpioSda(h) ? I2C_NO_ACK : I2C_ACK;

	I2C_GPIO_DELAY(h);
	_I2C_SetGpioScl(h, I2C_LOW);

	if((flag&I2C_FLAG_STOP))
	{
		if(_I2C_GpioStop(h) < 0)
			return -1;
	}
	else
	{
		_I2C_SetGpioSdaToInput(h);
	}
	I2C_GPIO_DELAY(h);

	return (ack == I2C_ACK) ? 0 : -1;
}

static int _I2C_GpioByteRead(I2C_SW_HANDLE h, UINT8 *data, UINT32 flag)
{
	UINT8 i;

	*data = 0;
	for(i=0; i<8; i++)
	{
		_I2C_SetGpioScl(h, I2C_LOW);
		I2C_GPIO_DELAY(h);
		_I2C_SetGpioScl(h, I2C_HIGH);
		I2C_GPIO_DELAY(h);
		*data = (*data << 1) | _I2C_GetGpioSda(h);
	}

	//Generate Ack or Nack
	_I2C_SetGpioScl(h, I2C_LOW);
	if(flag & I2C_FLAG_ACK)
		_I2C_SetGpioSda(h, I2C_LOW);
	else
		_I2C_SetGpioSda(h, I2C_HIGH);
	I2C_GPIO_DELAY(h);

	_I2C_SetGpioScl(h, I2C_HIGH);
	I2C_GPIO_DELAY(h);


	_I2C_SetGpioScl(h, I2C_LOW);
	if((flag&I2C_FLAG_STOP))
	{
		if(_I2C_GpioStop(h) < 0)
			return -1;
	}
	else
	{
		_I2C_SetGpioSdaToInput(h);
	}
	I2C_GPIO_DELAY(h);

	return 0;
}

static int _I2C_GpioWriteCmd(I2C_SW_HANDLE h, UINT8 slaveAddr, UINT8 subAddrSize,
							UINT8 *subAddr, UINT8 *buf, UINT32 bufSize, UINT32 option)
{
	UINT32 index;
	UINT32 flag;
	int res = -1;

	/* Send the slave address along with the write mode bit set */
	if(_I2C_GpioByteWrite(h, slaveAddr | I2C_WRITE_MODE, I2C_FLAG_START) < 0)
	{
		res = -1;
		goto end;
	}

	//Send sub address if it has.
	if(subAddrSize > 0)
	{
		for(index = 0; index<subAddrSize; index++)
		{
			//Write the register address to write
			if(_I2C_GpioByteWrite(h, subAddr[index], 0) < 0)
			{
				res = -2;
				goto end;
			}
		}
	}

	/* Send the data here */
	for(index = 0; index<bufSize; index++)
	{
		if(index == (bufSize-1))
			flag = I2C_FLAG_STOP;
		else
			flag = 0;

		//Write the data
		if(_I2C_GpioByteWrite (h, buf[index], flag) < 0)
		{
			res = -3;
			goto end;
		}
	}

	return bufSize;

end:
	I2C_PRINT_ERROR("_I2C_GpioWriteCmd. slave=0x%02x, res=%d\n", slaveAddr, res);
	_I2C_GpioStop(h);

	return res;
}

static int _I2C_GpioReadCmd(I2C_SW_HANDLE h, UINT8 slaveAddr, UINT8 subAddrSize,
							UINT8 *subAddr, UINT8 *buf, UINT32 bufSize, UINT32 option)
{
	UINT32 index;
	UINT32 flag;
	int res = -1;

	if (subAddrSize > 0)
	{
		/* Send the slave address along with the write mode bit set */
		if(_I2C_GpioByteWrite (h, slaveAddr | I2C_WRITE_MODE, I2C_FLAG_START) < 0)
		{
			res = -1;
			goto end;
		}

		for(index = 0; index<subAddrSize; index++)
		{
			flag = 0;
			if(index == (subAddrSize-1))
			{
				if(option & I2C_STOP_START_BEFORE_READ)
					flag = I2C_FLAG_STOP;
			}

			//Write the register address to write
			if(_I2C_GpioByteWrite(h, subAddr[index], flag) < 0)
			{
				res = -2;
				goto end;
			}
		}

		if(option&I2C_STOP_START_BEFORE_READ) flag = I2C_FLAG_START;
		else if(option&I2C_START_BEFORE_READ) flag = I2C_FLAG_RESTART;
		else								  flag = 0;
	}
	else
	{
		flag = I2C_FLAG_START;
	}

	/* Send the slave address along with the read mode bit set */
	if(_I2C_GpioByteWrite (h, slaveAddr | I2C_READ_MODE, flag) < 0)
	{
		res = -3;
		goto end;
	}

	for(index=0; index<bufSize; index++)
	{
		if(index == (bufSize-1)) flag = I2C_FLAG_STOP;
		else					 flag = I2C_FLAG_ACK;

		if(_I2C_GpioByteRead(h, &buf[index], flag) < 0)
		{
			res = -4;
			goto end;
		}
	}

	return bufSize;

end:
	I2C_PRINT_ERROR("_I2C_GpioReadCmd. slave=0x%02x, res=%d\n", slaveAddr, res);
	_I2C_GpioStop(h);

	return res;
}

static int _I2C_SwOpen(UINT8 ch, I2C_CTRL_HANDLE control, UINT8 use_irq)
{
	I2C_SW_HANDLE h = (I2C_SW_HANDLE)control;

	if(h == NULL)
		return -1;

	I2C_MUTEX_INIT(h);
	h->ch = ch;

	return 0;
}

static int _I2C_SwClose(I2C_CTRL_HANDLE handle)
{
	I2C_SW_HANDLE h = (I2C_SW_HANDLE)handle;

	_I2C_GpioReset(h);

	return 0;
}

static int _I2C_SwInit(I2C_CTRL_HANDLE handle, LX_SW_I2C_GPIO_MAP_T *map)
{
	I2C_SW_HANDLE h = (I2C_SW_HANDLE)handle;

	if(map == NULL)
	{
		I2C_PRINT_ERROR("gpio map is null\n");
		return -1;
	}

	if(h->gpio_inited)
	{
		if(map->sda == h->sda && map->scl == h->scl)
			return 0;

		I2C_PRINT_ERROR("It's already inited with a different gpio map\n");
		return -1;
	}

	h->sda = map->sda;
	h->scl = map->scl;

	h->sda_dir_reg = _I2C_GetGpioDirReg(map->sda);
	h->scl_dir_reg = _I2C_GetGpioDirReg(map->scl);

	h->sda_dir_mask = _I2C_GetGpioDirMask(map->sda);
	h->scl_dir_mask = _I2C_GetGpioDirMask(map->scl);

	h->sda_data_reg = _I2C_GetGpioDataReg(map->sda);
	h->scl_data_reg = _I2C_GetGpioDataReg(map->scl);

	_I2C_SetGpioTop(map->sda);
	_I2C_SetGpioTop(map->scl);

	_I2C_GpioReset(h);

	h->gpio_inited = TRUE;

	return 0;
}

static int _I2C_SwSetClock(I2C_CTRL_HANDLE handle, UINT32 clock)
{
	I2C_SW_HANDLE h = (I2C_SW_HANDLE)handle;

	/*****************************
		delay(uS)	freq.(Khz)
		10			47
		9			52.5
		8			57
		5			88
		4			107
		3			137
		2			185
		1			320
	******************************/
	switch ( clock )
	{
		case I2C_CLOCK_50KHZ:
			h->delay = 10;
			break;
		case I2C_CLOCK_200KHZ:
			h->delay = 2;
			break;
		case I2C_CLOCK_400KHZ:
			h->delay = 1;
			break;
		case I2C_CLOCK_100KHZ:
		default:
			h->delay = 5;
			break;
	}

	return 0;
}

static int _I2C_SwWrite(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data)
{
	I2C_SW_HANDLE h = (I2C_SW_HANDLE)handle;
	int res;

	res = _I2C_GpioWriteCmd(h, data->slaveAddr, data->subAddrSize, data->subAddr,
							data->buf, data->bufSize, data->flag);

	return res;
}

static int _I2C_SwRead(I2C_CTRL_HANDLE handle, LX_I2C_RW_DATA_T *data)
{
	I2C_SW_HANDLE h = (I2C_SW_HANDLE)handle;
	int res;

	res = _I2C_GpioReadCmd(h, data->slaveAddr, data->subAddrSize, data->subAddr,
							data->buf, data->bufSize, data->flag);

	return res;
}

static int _I2C_SwLock(I2C_CTRL_HANDLE handle)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	return OS_LockMutex(&h->mutex);
}

static int _I2C_SwUnlock(I2C_CTRL_HANDLE handle)
{
	I2C_HW_HANDLE h = (I2C_HW_HANDLE)handle;
	OS_UnlockMutex(&h->mutex);
	return 0;
}

/*
*************************************************************************
* I2C GPIO - END
*************************************************************************
*/



/**
********************************************************************************
* @brief
*	Initialize the i2c controller for the mode of operation passed as argument.
* @remarks
*  DETAIL INFORMATION
* @par requirements:
*
* @param
*	ch : channel (0 ~ MAX)
* @return
*	-1 : error
*	others : handle
********************************************************************************
*/
static LX_I2C_DEV_HANDLE _I2C_DevOpen(UINT8 ch, UINT8 internal)
{
	LX_I2C_DEV_HANDLE h;
	I2C_DEVICE_FUN_T *func;
	UINT8 use_irq;
	UINT8 type;

	if(ch >= (_pI2cConfig->num_hw_devices + _pI2cConfig->num_sw_devices))
	{
		I2C_PRINT_ERROR("CH[%d] is not exist !!!\n", ch);
		return NULL;
	}

	h = &_pI2cDev[ch];
	if(h->inited)
	{
		return h;
	}

	type = (ch < _pI2cConfig->num_hw_devices)  ? I2C_TYPE_HW : I2C_TYPE_SW;

	func = &_stI2cFunc[type];
	if(internal && lx_chip_rev() >= LX_CHIP_REV_L9_B0)
	{
		func = &_stI2cInternalFunc;
	}

	if(internal)	use_irq = USE_INTERNAL_I2C_IRQ;
	else			use_irq = USE_I2C_IRQ;

	if(func->open(ch, &h->control, use_irq) < 0)
	{
		I2C_PRINT_ERROR("CH[%d] device open fail !!!\n", ch);
		return NULL;
	}

	snprintf(h->name, I2C_MAX_NAME, "I2C[%d]", ch);

	h->ch = ch;
	h->clock = I2C_CLOCK_INVALID;

	h->type = type;
	h->func = func;

	h->inited = TRUE;
	return h;
}

LX_I2C_DEV_HANDLE I2C_DevOpen(UINT8 ch)
{
	return _I2C_DevOpen(ch, 0);
}

int I2C_DevClose(LX_I2C_DEV_HANDLE h)
{
	int res = -1;

	I2C_CHECK_INITED(h);

	if(h->func->close)
	{
		I2C_MUTEX_LOCK(h);
		res = h->func->close(&h->control);
		I2C_MUTEX_UNLOCK(h);
	}
	h->inited = FALSE;
	return res;
}

int I2C_DevInit(LX_I2C_DEV_HANDLE h, LX_SW_I2C_GPIO_MAP_T *map)
{
	int res = -1;

	I2C_CHECK_INITED(h);

	if(h->func->init)
	{
		I2C_MUTEX_LOCK(h);
		res = h->func->init(&h->control, map);
		I2C_MUTEX_UNLOCK(h);
	}

	return res;
}

int I2C_DevSetClock(LX_I2C_DEV_HANDLE h, UINT32 clock)
{
	int res = -1;

	I2C_CHECK_INITED(h);

	if(h->func->setclock)
	{
		I2C_MUTEX_LOCK(h);
		if(h->clock != clock)
		{
			res = h->func->setclock(&h->control, clock);
			if(res == 0) h->clock = clock;
		}
		I2C_MUTEX_UNLOCK(h);
	}

	return res;
}

int I2C_DevTransfer(LX_I2C_DEV_HANDLE h, LX_I2C_RW_DATA_T *data, UINT8 mode)
{
	int res = -1;

	I2C_CHECK_INITED(h);
	I2C_CHECK_PARAM(data->subAddrSize, data->bufSize);

	I2C_MUTEX_LOCK(h);

	if(data->clock != I2C_CLOCK_INVALID && h->clock != data->clock)
	{
		res = h->func->setclock(&h->control, data->clock);
		if(res == 0) h->clock = data->clock;
	}

	if(mode == I2C_WRITE_MODE)
	{

	#ifdef I2C_EEPROM_CHECK_WRITE_ACK
		if(h->ch == _pI2cConfig->eeprom_ch &&
			(data->slaveAddr&I2C_EEPROM_ADDR_MASK) == I2C_EEPROM_ADDR)
		{
			data->flag |= I2C_CHECK_EEPROM_WRITE;
		}
	#endif

		if(h->func->write)
			res = h->func->write(&h->control, data);
	}
	else
	{
		data->flag |= I2C_START_BEFORE_READ;

		if(h->func->read)
			res = h->func->read(&h->control, data);
	}

	I2C_MUTEX_UNLOCK(h);

	return res;
}


int I2C_DevRegWrite(UINT8 slave, UINT8 reg, UINT8 data, UINT32 retry)
{
	UINT32 idx;
	int rc = -1;
	LX_I2C_RW_DATA_T param;

	if(_pInternalHandle == NULL)
	{
		I2C_PRINT_ERROR("Not supported !!!\n");
		return -1;
	}

	param.slaveAddr		= slave;
	param.subAddrSize	= 1;
	param.subAddr[0]	= reg;
	param.buf			= &data;
	param.bufSize		= 1;
	param.clock			= I2C_CLOCK_INVALID;	//use initial clock
	param.flag			= 0;

	for(idx=0; idx<retry; idx++)
	{
		rc = I2C_DevTransfer(_pInternalHandle, &param, I2C_WRITE_MODE);
		if(rc >= 0) break;
	}

	if(rc < 0)
	{
		if(lx_chip_rev() > LX_CHIP_REV_L9_A1)
		{
			I2C_PRINT_ERROR("Internal i2c write failed. slave=0x%02x, reg=0x%02x\n",
				slave, reg);
		}
	}

	return rc;
}

int I2C_DevRegRead(UINT8 slave, UINT8 reg, UINT8* data, UINT32 retry)
{
	UINT32 idx;
	int rc = -1;
	LX_I2C_RW_DATA_T param;

	if(_pInternalHandle == NULL)
	{
		I2C_PRINT_ERROR("Not supported !!!\n");
		return -1;
	}

	param.slaveAddr		= slave;
	param.subAddrSize	= 1;
	param.subAddr[0]	= reg;
	param.buf			= data;
	param.bufSize		= 1;
	param.clock			= I2C_CLOCK_INVALID;	//use initial clock
	param.flag			= 0;

	for(idx=0; idx<retry; idx++)
	{
		rc = I2C_DevTransfer(_pInternalHandle, &param, I2C_READ_MODE);
		if(rc >= 0) break;
	}

	if(rc < 0)
	{
		if(lx_chip_rev() > LX_CHIP_REV_L9_A1)
		{
			I2C_PRINT_ERROR("Internal i2c read failed. slave=0x%02x, reg=0x%02x\n",
				slave, reg);
		}
	}

	return rc;
}


int I2C_CoreInit(void)
{
	LX_I2C_CFG_T* config = I2C_GetConfig();

	if(config == NULL)
	{
		DBG_PRINT_ERROR("Not supported chip !!!\n");
		return -1;
	}

	_pI2cDev = OS_Malloc(sizeof(I2C_DEV_T) * (config->num_hw_devices + config->num_sw_devices));
	memset(_pI2cDev, 0, sizeof(I2C_DEV_T) * (config->num_hw_devices + config->num_sw_devices));
	_pI2cConfig = config;


	astI2CRegArray = OS_Malloc(sizeof(I2C_REG_T) * config->num_hw_devices);
	memset(astI2CRegArray, 0, sizeof(I2C_REG_T) * config->num_hw_devices);

	if(config->internal != NULL)
	{
		_pInternalHandle = _I2C_DevOpen(config->internal->ch, 1);
		if(_pInternalHandle != NULL)
		{
			UINT32 initial_clock = (config->internal->clock > I2C_CLOCK_2MHZ) ? I2C_CLOCK_2MHZ : config->internal->clock;

			I2C_DevSetClock(_pInternalHandle, initial_clock);

			if ( lx_chip_rev() >= LX_CHIP_REV_L9_A0)
			{
				VPORT_I2C_Read((UINT32*)&gsoft_reset_0);
				//gsoft_reset_0.swrst_pix = 0;
				gsoft_reset_0.swrst_f24m = 0;
				VPORT_I2C_Write((UINT32*)&gsoft_reset_0);

				VPORT_I2C_Read((UINT32*)&gmain_pll_4);
				gmain_pll_4.dr3p_pdb = 0;
				VPORT_I2C_Write((UINT32*)&gmain_pll_4);

				VPORT_I2C_Read((UINT32*)&gmain_pll_4);
				gmain_pll_4.dr3p_pdb = 1;
				VPORT_I2C_Write((UINT32*)&gmain_pll_4);

				if(config->internal->clock > I2C_CLOCK_2MHZ)
				{
					mdelay(1);
					// Internal I2C support upto 8Mhz
					VPORT_I2C_Read((UINT32*)&ghostif_control_0);
					ghostif_control_0.reg_sel_hostif_clock = 1;
					VPORT_I2C_Write((UINT32*)&ghostif_control_0);

/*
					// 2011 I2C nalm
					mdelay(1);
					VPORT_I2C_Read((UINT32*)&gmain_pll_0);
					gmain_pll_0.dr3p_nsc= 2;
					VPORT_I2C_Write((UINT32*)&gmain_pll_0);
*/
					mdelay(1);
					VPORT_I2C_Read((UINT32*)&ghostif_control_0);
					if(ghostif_control_0.ro_hostif_is_162mhz == 1)
						I2C_DEBUG("Set Internal I2C 162MHZ Clock !!!!\n");

					I2C_DevSetClock(_pInternalHandle, config->internal->clock);
				}

			}

		}
		else
		{
			DBG_PRINT_ERROR("Can't open internal channel[%d]\n", config->internal->ch);
		}
	}

	return 0;
}

int I2C_CoreGetDeviceNums(void)
{
	if(_pI2cConfig == NULL) return -1;

	return (_pI2cConfig->num_hw_devices + _pI2cConfig->num_sw_devices);
}


