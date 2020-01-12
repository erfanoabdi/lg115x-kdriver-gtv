/*
SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
Copyright(c) 2010 by LG Electronics Inc.
 
This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details.
*/ 


/** @file
 *
 *  main driver implementation for sci device.
 *	sci device will teach you how to make device driver with new platform.
 *
 *  author		Juhee Park
 *  version		1.0
 *  date		2011.06.09
 *  note		Additional information.
 *
 *  @addtogroup lg_sci
 *	@{
 */

/*----------------------------------------------------------------------------------------
    Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rmap.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>

#include "os_util.h"
#include "base_types.h"

#include "sci_err.h"
#include "sci_cfg.h"
#include "sci_kapi.h"

#include "l9/sci_l9_reg.h"
#include "l9/sci_l9_isr.h"


/*----------------------------------------------------------------------------------------
    Constant Definitions
----------------------------------------------------------------------------------------*/
#define MODULE_NAME 	    "sci"

/*----------------------------------------------------------------------------------------
    Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Extern Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    global Variables
----------------------------------------------------------------------------------------*/
int	g_sci_debug_fd	= -1;
int	g_sci_trace_depth = 0;

/* Module param for Debug print */
#define SCI_DEBUG_OFF		0x00
#define SCI_DEBUG_PRINT		0x01
#define SCI_DEBUG_TRACE		0x02
#define SCI_DEBUG_WARN		0x04
#define SCI_DEBUG_ERROR		0x08
#define SCI_DEBUG_PROC		0x10
#define SCI_DEBUG_ASSERT	0x20
#define SCI_DEBUG_SYSFS		0x40
#define SCI_DEBUG_DEVFS		0x80

static int gSCI_DbgLvl = 0x40;//SCI_DEBUG_DEVFS||SCI_DEBUG_ERROR;
module_param(gSCI_DbgLvl, int, 0);
MODULE_PARM_DESC(gSCI_DbgLvl, "Device Driver operating mode = SCI_DEBUG_SYSFS or SCI_DEBUG_DEVFS");

/*========================================================================================
    Implementation Group : SCI Common
========================================================================================*/
#define R_BLOCK							0x80

#define S_BLOCK							0xC0
#define S_BLOCK_RESPONSE		0x20

#define S_BLOCK_ERROR_VPP		(S_BLOCK|0x24)

#define IFS_REQUEST	(S_BLOCK|0x01)
#define IFS_RESPONSE	(IFS_REQUEST|S_BLOCK_RESPONSE)

#define WTX_REQUEST (S_BLOCK|0x03)
#define WTX_RESPONSE (WTX_REQUEST|S_BLOCK_RESPONSE)

#define RESYNTH_REQUEST (S_BLOCK|0x0)
#define RESYNTH_RESPONSE (RESYNTH_REQUEST|S_BLOCK_RESPONSE)



/**
@fn static SCI_RETURN_T _respondSBlock(struct SCI_T* pSCI, LX_SCI_BLOCK *rx)
@brief
@param ui32INFS
@return 0 : success
@return -1 : failure
@return -2 : non-supported request
*/
static SCI_RETURN_T _respondSBlock(struct SCI_T* pSCI, LX_SCI_BLOCK *rx)
{
	LX_SCI_BLOCK tx;
	UINT8 data = 0;

	//printk("res S B\n");
	tx.buf = &data;
	if (rx->pcb == IFS_REQUEST)
	{
		tx.nad = rx->nad;
		tx.pcb = IFS_RESPONSE;
		tx.len = 1;
		tx.buf[0] = pSCI->infs;
	}
	else if (rx->pcb == WTX_REQUEST)
	{
		tx.nad = rx->nad;
		tx.pcb = WTX_RESPONSE;
		tx.len = 1;
		tx.buf[0] = pSCI->wtx;
	}
	else if (rx->pcb == RESYNTH_REQUEST)
	{
		tx.nad = rx->nad;
		tx.pcb = RESYNTH_RESPONSE;
		tx.len = 0;
	}
	else
	{

	}

	pSCI->writeBlock(&tx);

	return SCI_SUCCESS;
}

/**
@fn static SCI_RETURN_T _respondRBlock(struct SCI_T* pSCI, LX_SCI_BLOCK *rx)
@brief
@param ui32INFS
@return 0 : success
@return -1 : failure
@return -2 : non-supported request
*/
static SCI_RETURN_T _respondRBlock(struct SCI_T* pSCI, LX_SCI_BLOCK *rx, UINT8 ns)
{
	LX_SCI_BLOCK tx;

	//printk("res R B\n");

	tx.nad = rx->nad;
	tx.pcb = IFS_RESPONSE & ns;
	tx.len = 0;

	pSCI->writeBlock(&tx);

	return SCI_SUCCESS;
}


/**
@fn static SCI_RETURN_T _requestResynch(SCI_L9_S_CTRL_T ctrl)
@brief
@param ui32INFS
@return 0 : success
@return -1 : failure
*/
static SCI_RETURN_T _requestResynch(struct SCI_T* pSCI)
{
	LX_SCI_BLOCK tx;
	LX_SCI_BLOCK rx;

	//printk("resync\n");

	tx.nad = 0;
	tx.pcb = RESYNTH_REQUEST;
	tx.len = 0;

	pSCI->writeBlock(&tx);


	pSCI->readBlock(&rx);

	if (rx.nad != tx.nad)
	{
		return SCI_FAIL;
	}
	if (rx.pcb != IFS_RESPONSE)
	{
		return SCI_FAIL;
	}
	if (rx.len != 1)
	{
		return SCI_FAIL;
	}

	return SCI_SUCCESS;
}

/**
@fn static SCI_RETURN_T _requestINFS(SCI_L9_S_CTRL_T ctrl)
@brief
@param ui32INFS
@return 0 : success
@return -1 : failure
*/
static SCI_RETURN_T _requestINFS(struct SCI_T* pSCI, UINT8* infs)
{
	LX_SCI_BLOCK tx, rx;
	UINT8 dataTx = 0, dataRx = 0;

	//printk("req INFS\n");

	tx.buf = &dataTx;
	rx.buf = &dataRx;

	tx.nad = 0;
	tx.pcb = IFS_REQUEST;
	tx.len = 1;
	tx.buf[0] = pSCI->infs;

	pSCI->writeBlock(&tx);

	pSCI->readBlock(&rx);

	if (rx.nad != tx.nad)
	{
		return SCI_FAIL;
	}
	if (rx.pcb != IFS_RESPONSE)
	{
		return SCI_FAIL;
	}
	if (rx.len != 1)
	{
		return SCI_FAIL;
	}

	*infs = rx.buf[0];

	return SCI_SUCCESS;
}

#if 0 //This code may be needed later
/**
@fn static SCI_RETURN_T _requestWTX(SCI_L9_S_CTRL_T ctrl)
@brief
@param ui32INFS
@return 0 : success
@return -1 : failure
*/
static SCI_RETURN_T _requestWTX(struct SCI_T* pSCI, UINT8 *wtx)
{
	//SINT32 ret = 0;

	LX_SCI_BLOCK tx, rx;
	UINT8 dataTx = 0, dataRx = 0;

	//printk("req WTX\n");

	tx.buf = &dataTx;
	rx.buf = &dataRx;

	tx.nad = 0;
	tx.pcb = WTX_REQUEST;
	tx.len = 1;
	tx.buf[0] = pSCI->wtx;

	pSCI->writeBlock(&tx);

	pSCI->readBlock(&rx);

	if (rx.nad != tx.nad)
	{
		return SCI_FAIL;
	}
	if (rx.pcb != WTX_RESPONSE)
	{
		return SCI_FAIL;
	}
	if (rx.len != 1)
	{
		return SCI_FAIL;
	}

	*wtx = rx.buf[0];

	return SCI_SUCCESS;
}
#endif

/**
@fn SCI_RETURN_T _decodeATR (UINT8 *i_pucAtrBuffer, SINT32 i_iAtrLen, SCI_L9_ATR_T *atrp)
@brief
@param i_pucAtrBuffer
@param i_iAtrLen
@param atrp
@return 0 : success
@return -1 : Invalid Params
@return -2 : Invalid ATR
*/
static SCI_RETURN_T _decodeATR (LX_SCI_ATR_T *pATR)
{
	SINT32 iErr = 0;
	int left = 0, ofs = 0, count = 0;
	int flags = 0, i= 0;

	/* sanity check parameters */
	if ((pATR == NULL) || (pATR->data_count < 3))
	{
		iErr = -1;
		goto bad;
	}

	/* more than 33 bytes is not valid according to the spec ... so
	* we truncate what we look at in the name of sanity preservation
	*/
	if (pATR->data_count > SCI_MAX_ATR)
	{
		pATR->data_count = SCI_MAX_ATR;
	}

	ofs = 0;
	left = pATR->data_count;

	/* first byte is TS */
	pATR->TS = pATR->data[ofs++];
	left--;

	/* check the first byte - it can have only two values */
	if (pATR->TS==0x3f)
	{
		pATR->direct_convention=0;
	}
	else
	{
		pATR->direct_convention=1;
	}

	if (left>0)
	{
		/* next byte is always T0 */
		pATR->T0=pATR->data[ofs++];
		left--;

		/* Validate T0 */
		if ( pATR->T0 != 0xF0 )
		{
			iErr = -2;
			goto bad;
		}

		/* extract flags for T[A-D] and historical count */
		flags=(pATR->T0 & 0xf0) >> 4;
		pATR->historical_count=(pATR->T0 & 0x0f);

		/* now read in the interleaved additional bytes */
		while ((left>0) && (flags!=0))
		{
			/* grab the A byte */
			if (flags & 0x01)
			{
				pATR->TA[1+pATR->TA_count++]=pATR->data[ofs++];

				if ( count == 0 )
				{
					pATR->FI = ( pATR->TA[1 + pATR->TA_count - 1] & 0xf0 ) >> 4 ;
					pATR->FI = ( pATR->FI == 3 ) ? 744 : ( pATR->FI == 1 ) ? 372 : -1;

					/* Validate FI */
					if ( pATR->FI == -1 )
					{
						pATR->FI = 372;
						iErr = -3;
						goto bad;
					}

					pATR->DI = pATR->TA[1 + pATR->TA_count - 1] & 0x0F;
					pATR->DI = ( pATR->DI == 3 ) ? 0x04 : ( pATR->DI == 4 ) ? 0x08 : ( pATR->DI == 2 ) ? 0x02 : -1;

					/* Validate DI */
					if ( pATR->DI == -1 )
					{
						pATR->DI = 0x02;
						iErr = -4;
						goto bad;
					}
				}
				else if ( count == 2 )
				{
					pATR->ifsc = pATR->TA[1+pATR->TA_count-1];
				}
				else if ( count == 3 )
				{
					pATR->power = pATR->TA[1+pATR->TA_count-1] & 0x01F;

					/* Validate Power */
					if ( ( pATR->power != 0x01 ) \
							&& ( pATR->power != 0x03 ) )
					{
						iErr = -5;
						goto bad;
					}
				}

				left--;
				if (left<=0)
					break;
			}

			if (flags & 0x02)
			{
				pATR->TB[1+pATR->TB_count++]=pATR->data[ofs++];
				left--;

				if ( count == 0 )
				{
					/* Validate TB0 */
					if ( pATR->TB[1+pATR->TB_count-1] != 0x00 )
					{
						iErr = -6;
						goto bad;
					}
				}
				else if ( count == 2 )
				{
					pATR->bwi = ( pATR->TB[1+pATR->TB_count-1] & 0xF0  ) >> 4;
					pATR->cwi = pATR->TB[1+pATR->TB_count-1] & 0x0F;
				}

				if (left<=0)
					break;
			}

			if (flags & 0x04)
			{
				pATR->TC[1+pATR->TC_count++]=pATR->data[ofs++];
				left--;

				if ( count == 0 )
				{
					pATR->guard_time = pATR->TC[1+pATR->TC_count-1];
					if ( pATR->guard_time == 0xFF ) pATR->guard_time = 1;

					pATR->crc = pATR->TC[1 + pATR->TC_count - 1] & 0x01;
				}

				if (left<=0)
					break;
			}

			/* we have another TD to start the next block */
			if (flags & 0x08)
			{
				flags=pATR->data[ofs++];
				if ((flags & 0x0f) != 0xF)
					pATR->transmission_protocol=(flags & 0x0f);
				flags=(flags & 0xf0)>>4;

				left--;
				if (left<=0)
				{
					break;
				}
			}
			else
			{
				break;
			}

			++count;
		}

		/* at this point we should have any historical chars
		* and TCK left to read ... check that we have enough
		* data to cover this and abort if not
		*/
		if (left<pATR->historical_count)
		{
			iErr = -7;
			goto bad;
		}

		if ( pATR->historical_count > 0 )
		{
			/* copy historical */
			memcpy(pATR->historical,pATR->data+ofs,pATR->historical_count);
			ofs+=pATR->historical_count;
			left-=pATR->historical_count;
		}

		/* check to see if we have 9x 00 and in which case it
		* probably isn't the TCK but is the response to the
		* implied select - there doesn't appear to be a nice
		* way of determining if TCK is transmitted or not
		*
		* for the moment at least this data is considered
		* as part of the ATR ... it is transmitted always on
		* reset but isn't strictly/logically in the ATR
		* but such things we simply have to live with
		*/
		if (left>=2)
		{
			if ( ((pATR->data[ofs] & 0x90)==0x90) && (pATR->data[ofs+1]==0x00) )
			{
				ofs+=2;
				left-=2;
			}
		}

		/* anything remaining should be the TCK */
		if (left>0)
		{
			/* now we should just have TCK left */
			pATR->TCK=1;
			pATR->TCK_sent=pATR->data[ofs++];

			/* now we calculate what the TCK should be so it can be checked
			* by user logic if desired
			*/
			pATR->TCK_calc=0;
			for(i=1;i<(ofs-1);i++)
				pATR->TCK_calc ^= (pATR->data[i] & 0xff);
			left--;

			/* Validate TCK */
			if ( pATR->TCK_sent != pATR->TCK_calc )
			{
				iErr = -8;
				goto bad;
			}
		}

		/* extra data should be "deleted" from the ATR */
		if (left>0)
		{
			pATR->data_count=-left;
		}
	}

	return 0;

bad:

	SCI_TRACE("Decode ATR Failed : Err[ %d ]\n", iErr );

	if ( iErr < -2 )
	{
		iErr = -2;
	}

	return iErr;
}

static int _reset(struct SCI_T* pSCI)
{
	SCI_RETURN_T res = SCI_SUCCESS;


	if(pSCI->setVoltage == NULL || pSCI->reset == NULL || pSCI->configIF == NULL )
	{
		SCI_ERROR("FUNC ASSERT\n");
		return SCI_FAIL;
	}

	res = pSCI->setVoltage(SCI_VOLTAGE_3V);
	if (res == SCI_SUCCESS)
	{
		res = pSCI->reset(&pSCI->atr);
		if (res != SCI_SUCCESS)
		{
			res = pSCI->setVoltage(SCI_VOLTAGE_5V);
			if (res == SCI_SUCCESS)
			{
				res = pSCI->reset(&pSCI->atr);
				if (res != SCI_SUCCESS)
				{
					SCI_ERROR("error");
					return SCI_FAIL;
				}
			}
		}
	}
	res = _decodeATR(&pSCI->atr);
	if (res == SCI_SUCCESS)
	{
		if(pSCI->atr.power == SCI_VOLTAGE_5V)
			res = pSCI->setVoltage(SCI_VOLTAGE_5V);
		else
			res = pSCI->setVoltage(SCI_VOLTAGE_3V);
	}

	res = pSCI->configIF(&pSCI->atr);
	if (res != SCI_SUCCESS)
	{
		return res;
	}
	res = _requestINFS(pSCI, &pSCI->infs);
	res = pSCI->configINFS(pSCI->infs);
	//_requestWTX();
	//res = pSCI->configWTX(&pSCI->atr);

	return res;
}

static int _transmit(struct SCI_T* pSCI, LX_SCI_TRANSMIT* transmitArg)
{
	SCI_RETURN_T res = SCI_SUCCESS;
	SCI_RETURN_T resWriteBlock = SCI_SUCCESS;
	SCI_RETURN_T resReadBlock = SCI_SUCCESS;

	UINT8 ns = transmitArg->txBuf.pcb & 0x40;
	UINT8 transmitCount = 3;
	UINT8 flagResync = 0;
	UINT8 retry = 3;

	do{
		/* I Block send */
		resWriteBlock = pSCI->writeBlock(&transmitArg->txBuf);

		/* card response */
		resReadBlock = pSCI->readBlock(&transmitArg->rxBuf);

		if (resWriteBlock != SCI_SUCCESS)
		{
			if (resReadBlock != SCI_SUCCESS)
			{
				_respondRBlock(pSCI, &transmitArg->rxBuf, ns);

				/* card response */
				resReadBlock = pSCI->readBlock(&transmitArg->rxBuf);

				retry--;
				continue;
			}
			else
			{
				if(((transmitArg->rxBuf.pcb & 0xC0) == R_BLOCK) && ((ns >> 2) == (transmitArg->rxBuf.pcb & 0x10)))
				{
					retry--;
					continue;
				}
				else
				{
					_requestResynch(pSCI);

					retry--;
					continue;
				}
			}
		}
		else
		{
			if (resReadBlock != SCI_SUCCESS)
			{
				do {
					_respondRBlock(pSCI, &transmitArg->rxBuf, ns);

					resReadBlock = pSCI->readBlock(&transmitArg->rxBuf);

					if(!(transmitArg->rxBuf.pcb & 0xC0))
						break;

				} while (retry--);

				if (retry == 0)
					return SCI_FAIL;
			}
			else
			{
				// handleS Block request
			    if((transmitArg->rxBuf.pcb & 0xC0) == S_BLOCK)
			    {
					/* superblock response */
					res = _respondSBlock(pSCI, &transmitArg->rxBuf);

					// response S Block error
					if(res != SCI_SUCCESS)
					{
						goto error_handler;
					}

					// receive I Block from card
					res = pSCI->readBlock(&transmitArg->rxBuf);

					if(res != SCI_SUCCESS)
					{
						return SCI_FAIL_RESET;
					}
					else
						break;	//success

				}
				else if((transmitArg->txBuf.pcb & 0x40) == (transmitArg->rxBuf.pcb & 0x40))		// I Block handler
				{
					/* I Block response */
					break;
				}
				else	// tx pcb and rx pcb are not same (I block)
				{
					//ns = transmitArg->rxBuf.pcb & 0x10;

					res = _respondRBlock(pSCI, &transmitArg->rxBuf, ns);

					if(res != SCI_SUCCESS)
					{
						return SCI_FAIL_RESET;
					}

					goto error_handler;
				}
			}
		}

error_handler:
		if(flagResync == 1)
			return SCI_FAIL_RESET;

		/* card reset */
		transmitCount--;
		if(transmitCount < 2)
		{
			flagResync = 1;

			//resync fail
			if(_requestResynch(pSCI))
			{
				return SCI_FAIL_RESET;
			}
		}

	}while(transmitCount);

	return 0;
}

/*========================================================================================
    Implementation Group : SCI CDEV
========================================================================================*/
static struct SCI_T* gpSCI = NULL;

static int _ioctl_reset(struct SCI_T* pSCI, unsigned long arg)
{
	unsigned long leftovers = 0;
	SCI_RETURN_T res = SCI_SUCCESS;

	if(pSCI == NULL)
	{
		SCI_ERROR("ASSERT\n");
		return SCI_FAIL;
	}

	res = _reset(pSCI);
	if (res != SCI_SUCCESS)
	{
		return res;
	}

	leftovers = copy_to_user((void *)arg, &pSCI->atr, sizeof(LX_SCI_ATR_T));

	if(leftovers != 0)
	{
		SCI_ERROR("ERR COPY TO USR\n");
		return -EFAULT;
	}

	return 0;
}

static int _ioctl_getStatus(struct SCI_T* pSCI, unsigned long arg)
{
	unsigned long leftovers = 0;
	LX_SCI_STATUS statusArg;

	if(pSCI == NULL)
	{
		SCI_ERROR("ASSERT\n");
		return SCI_FAIL;
	}

	if(pSCI->getStatus == NULL)
	{
		SCI_ERROR("FUNC ASSERT\n");
		return SCI_FAIL;
	}

	statusArg = pSCI->getStatus();

	leftovers = copy_to_user((void *)arg, &statusArg, sizeof(LX_SCI_STATUS));

	if(leftovers != 0)
	{
		SCI_ERROR("ERR COPY TO USR\n");
		return -EFAULT;
	}

	return 0;
}

static int _ioctl_getVoltage(struct SCI_T* pSCI, unsigned long arg)
{
	unsigned long leftovers = 0;
	LX_SCI_VOLTAGE voltageArg;

	if(pSCI == NULL)
	{
		SCI_ERROR("ASSERT\n");
		return SCI_FAIL;
	}

	if(pSCI->getVoltage == NULL)
	{
		SCI_ERROR("FUNC ASSERT\n");
		return SCI_FAIL;
	}

	voltageArg = pSCI->getVoltage();

	leftovers = copy_to_user((void *)arg, &voltageArg, sizeof(LX_SCI_VOLTAGE));
	if(leftovers != 0)
	{
		SCI_ERROR("Failed in setting data[%ld]\n", leftovers);
		return -EFAULT;
	}

	return 0;
}

static int _ioctl_setVoltage(struct SCI_T* pSCI, unsigned long arg)
{
	unsigned long leftovers = 0;
	LX_SCI_VOLTAGE voltageArg;
	SCI_RETURN_T res = 0;

	if(pSCI == NULL)
	{
		SCI_ERROR("ASSERT\n");
		return SCI_FAIL;
	}

	if(pSCI->setVoltage == NULL)
	{
		SCI_ERROR("FUNC ASSERT\n");
		return SCI_FAIL;
	}

	leftovers = copy_from_user(&voltageArg, (void *)arg, sizeof(LX_SCI_VOLTAGE));

	if(leftovers != 0)
	{
		SCI_ERROR("ERR COPY FROM USR\n");
		return -EFAULT;
	}


	res = pSCI->setVoltage(voltageArg);

	return 0;
}

static int _ioctl_transmit(struct SCI_T* pSCI, unsigned long arg)
{
	unsigned long leftovers = 0;
	LX_SCI_TRANSMIT transmitArg;
	SCI_RETURN_T res = 0;

	if(pSCI == NULL)
	{
		SCI_ERROR("ASSERT\n");
		return SCI_FAIL;
	}

	if(pSCI->writeBlock == NULL)
	{
		SCI_ERROR("FUNC ASSERT\n");
		return SCI_FAIL;
	}

	if(pSCI->readBlock == NULL)
	{
		SCI_ERROR("FUNC ASSERT\n");
		return SCI_FAIL;
	}

	leftovers = copy_from_user(&transmitArg, (void *)arg, sizeof(LX_SCI_TRANSMIT));

	if(leftovers != 0)
	{
		SCI_ERROR("ERR COPY FROM USR\n");
		return -EFAULT;
	}

	res = _transmit(pSCI, &transmitArg);


	leftovers = copy_to_user((void *)arg, &transmitArg, sizeof(LX_SCI_TRANSMIT));

	if(leftovers != 0)
	{
		SCI_ERROR("ERR COPY TO USR\n");
		return -EFAULT;
	}

	return 0;
}

static int (*_gpIOCTLfns[])(struct SCI_T* pSCI, unsigned long arg) =
{
	/* SCI_IOR_ACTIVATE		0x01 */	 _ioctl_reset,
	/* SCI_IOR_GET_STATUS 	0x02 */	 _ioctl_getStatus,
	/* SCI_IOW_GET_VOLTAGE	0x03 */	 _ioctl_getVoltage,
	/* SCI_IOW_SET_VOLTAGE	0x04 */	 _ioctl_setVoltage,
	/* SCI_IORW_TRANSMIT	0x05 */	 _ioctl_transmit,
};

static int _SCI_IOCTL(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int res = 0;

	SCI_TRACE_BEGIN();

	cmd = _IOC_NR(cmd);
	cmd -= _IOC_NR(SCI_IOR_RESET);

	if(gpSCI == NULL)
	{
		SCI_ERROR("ASSERT\n");
		return -EIO;
	}

	if (cmd < sizeof(_gpIOCTLfns)/sizeof(int*) && _gpIOCTLfns[cmd] != NULL )
	{
		res = _gpIOCTLfns[cmd](gpSCI, arg);
	}
	else
	{
		/* redundant check but it seems more readable */
   		SCI_PRINT("[SCI] IOCTL: unknown IOCTL command %08X\n", cmd );

		res = -ENOTTY;
	}

	SCI_TRACE_END();

	return res;
}

static int _SCI_Open(struct inode *inode, struct file *filp)
{

	SCI_TRACE_BEGIN();

	/* Config SCI by chip */
	/*res = SCI_Config(&gpSCI);
	if (res != SCI_SUCCESS)
	{
		SCI_Unconfig(gpSCI);
		return -EINVAL;
	}*/

	SCI_TRACE_END();

	return 0;
}

static int _SCI_Close(struct inode *inode, struct file *filp)
{
	//int	res = res_ERROR;

	SCI_TRACE_BEGIN();

	/* Unconfig the shared resource */
	//SCI_Unconfig(gpSCI);


	SCI_TRACE_END();

	return 0;
}

static struct file_operations _g_sci_fops =
{
	.open 	= _SCI_Open,
	.release= _SCI_Close,
	.ioctl	= _SCI_IOCTL,
	.read	= NULL,
	.write	= NULL,
};


static int _SCI_InitDEVFS(void)
{
	int err;

	struct sci;

	/* Config SCI by chip */
	err = SCI_Config(&gpSCI);
	if (err != SCI_SUCCESS)
	{
		SCI_Unconfig(gpSCI);
		return -EINVAL;
	}

	err = register_chrdev_region(gpSCI->devno, 1 /*SCI_MAX_DEVICE*/, SCI_MODULE );

	cdev_init(&gpSCI->cdev, &_g_sci_fops);


	gpSCI->cdev.owner 	= THIS_MODULE;
	gpSCI->cdev.ops   	= &_g_sci_fops;

	err = cdev_add (&gpSCI->cdev, gpSCI->devno, 1 );

	if (err)
	{
		SCI_PRINT("error (%d) while adding sci device (%d.%d)\n", err, MAJOR(gpSCI->devno), MINOR(gpSCI->devno) );
			return -EIO;
	}
	OS_CreateDeviceClass ( gpSCI->devno, "%s%d", SCI_MODULE, 0 );

	if (gSCI_DbgLvl & SCI_DEBUG_PROC)
	{
		//SCI_PROC_Init();
	}

	return 0;
}

static void _SCI_CleanupDEVFS(void)
{
	SCI_Unconfig(gpSCI);

	if (gSCI_DbgLvl & SCI_DEBUG_PROC)
	{
		//SCI_PROC_Cleanup();
	}

}

/*========================================================================================
    Implementation Group : SCI Platform Device-Driver
========================================================================================*/
static ssize_t _store_reset(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct SCI_T* pSCI = NULL;
	struct platform_device* pPDEV = NULL;

	SCI_RETURN_T res = 0;
	char** last = NULL;
	UINT32 input = 0;

	pPDEV = container_of(dev, struct platform_device, dev);
	pSCI = 	platform_get_drvdata(pPDEV);

	input = simple_strtoul(buf, last, 0);

	if (input == 0)
	{
		input = 0;
	}
	else if (input == 1)
	{
		res = _reset(pSCI);
		if (res != SCI_SUCCESS)
		{
			return res;
		}
	}
	else
	{
		return SCI_INVAL_PARAM;
	}

	return count;
}

static ssize_t _show_status(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct SCI_T* pSCI = NULL;
	struct platform_device* pPDEV = NULL;
	ssize_t size = 0;
	LX_SCI_STATUS status = 0;

	pPDEV = container_of(dev, struct platform_device, dev);
	pSCI = 	platform_get_drvdata(pPDEV);

	status = pSCI->getStatus();

	size = snprintf(buf, PAGE_SIZE, "%d\n", status);

	return size;
}

static ssize_t _show_voltage(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct SCI_T* pSCI = NULL;
	struct platform_device* pPDEV = NULL;
	ssize_t size = 0;
	LX_SCI_VOLTAGE voltage = 0;

	pPDEV = container_of(dev, struct platform_device, dev);
	pSCI = 	platform_get_drvdata(pPDEV);

	voltage = pSCI->getVoltage();

	size = snprintf(buf, PAGE_SIZE, "%d\n", voltage);

	return size;
}

static ssize_t _store_voltage(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct SCI_T* pSCI = NULL;
	struct platform_device* pPDEV = NULL;
	LX_SCI_VOLTAGE voltage = 0;
	SCI_RETURN_T res = 0;
	char** last = NULL;
	UINT32 input = 0;

	pPDEV = container_of(dev, struct platform_device, dev);
	pSCI = 	platform_get_drvdata(pPDEV);

	input = simple_strtoul(buf, last, 0);

	if (input == 0)
	{
		voltage = SCI_VOLTAGE_3V;
	}
	else if (input == 1)
	{
		voltage = SCI_VOLTAGE_5V;
	}
	else
	{
		return SCI_INVAL_PARAM;
	}

	res = pSCI->setVoltage(voltage);
	if (res != SCI_SUCCESS)
	{
		return res;
	}

	return count;
}

static ssize_t _show_atr(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct SCI_T* pSCI = NULL;
	struct platform_device* pPDEV = NULL;
	ssize_t size = 0;
	UINT32 i = 0;

	pPDEV = container_of(dev, struct platform_device, dev);
	pSCI = 	platform_get_drvdata(pPDEV);

	for (i = 0; i < pSCI->atr.data_count; i++)
	{
		size += sprintf(buf + size, "0x%02x ", pSCI->atr.data[i]);
	}

	return size;
}

static ssize_t _show_transmit(struct device *dev, struct device_attribute *attr, char *buf)
{

	return 0;
}

static ssize_t _store_transmit(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{/*
	struct SCI_T* pSCI = NULL;
	struct platform_device* pPDEV = NULL;
	//LX_SCI_VOLTAGE voltage = 0;
	SCI_RETURN_T res = 0;
	char** last = NULL;
	UINT32 input = 0;

	UINT8 rxBuf[64] = {};

	UINT8 txBuf[64] = {0x0, 0x0, 0x3f, 0x90, 0x34, 0x0, 0x0, 0x39};
	UINT8 txBuf1[64] = {0x0, 0x0, 0x5, 0x90, 0x30, 0x0, 0x0, 0x0};
	UINT8 txBuf2[64] = {0x0, 0x40, 0x5, 0x90, 0x32, 0x0, 0x0, 0x0};

	pPDEV = container_of(dev, struct platform_device, dev);
	pSCI = 	platform_get_drvdata(pPDEV);

	input = simple_strtoul(buf, last, 0);

	if (input == 0)
	{
		res = pSCI->transmit(txBuf, rxBuf);
		if (res != SCI_SUCCESS)
		{
			return res;
		}
	}
	else if (input == 1)
	{
		res = pSCI->transmit(txBuf1, rxBuf);
		if (res != SCI_SUCCESS)
		{
			return res;
		}
	}
	else if (input == 2)
	{
		res = pSCI->transmit(txBuf2, rxBuf);
		if (res != SCI_SUCCESS)
		{
			return res;
		}
	}
	else
	{
		return SCI_INVAL_PARAM;
	}
*/
	return 0;

}

static struct device_attribute sci_device_attrs[] =
{
	__ATTR(atr,			S_IRUSR, 			_show_atr,		NULL),
	__ATTR(reset,		S_IWUSR, 			NULL,			_store_reset),
	__ATTR(status,		S_IRUSR,			_show_status,	NULL),
	__ATTR(voltage,		S_IRUSR | S_IWUSR, 	_show_voltage,	_store_voltage),
	__ATTR(transmit,	S_IRUSR | S_IWUSR, 	_show_transmit,	_store_transmit)
};

static int _SCI_InitAttrs(struct device *dev)
{
	int i= 0, error = 0;

	for (i = 0; i < ARRAY_SIZE(sci_device_attrs); i++)
	{
		error = device_create_file(dev, &sci_device_attrs[i]);
		if (error)
			break;
	}

	if (error != 0)
	{
		while (--i >= 0)
			device_remove_file(dev, &sci_device_attrs[i]);
	}

	return 0;
}

static int _SCI_Probe(struct platform_device *pPDEV)
{
	int ret = 0;
	SCI_RETURN_T res = 0;
	struct SCI_T* pSCI = NULL;

	/* sanity check */
	if (pPDEV == NULL)
	{
		//SCI_ASSERT(pPDEV);
		return -EINVAL;
	}

	/* Config SCI by chip */
	res = SCI_Config(&pSCI);
	if (res != SCI_SUCCESS)
	{
		ret = -EINVAL;
		goto fail2;
	}

	/* Probe HW info depending on the chip */
	res = _SCI_InitAttrs(&pPDEV->dev);
	if (res != 0)
	{
		ret = -EINVAL;
		goto fail1;
	}

	/* Bind sci info to the SCI platform device */
	platform_set_drvdata( pPDEV, pSCI);

	return 0;
fail1:
	SCI_Unconfig(pSCI);
fail2:
	return ret;
}

/**
 *
 * module remove function. this function will be called in insmod sci module.
 *
 * @param	struct platform_device
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int _SCI_Remove(struct platform_device *pPDEV)
{
	struct SCI_T *pSCI = NULL;

	/* sanity check */
	if (pPDEV == NULL)
	{
		return -EINVAL;
	}

	pSCI = (struct SCI_T*) platform_get_drvdata(pPDEV);

	/* Unconfig the shared resource */
	SCI_Unconfig(pSCI);

	/* remove sci info to the SCI platform device */
	platform_set_drvdata( pPDEV, NULL );

	return 0;
}

/**
 *
 * suspending module.
 *
 * @param	struct platform_device *pdev pm_message_t state
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int _SCI_Suspend(struct platform_device *pPDEV, pm_message_t state)
{
	return 0;
}

/**
 *
 * resuming module.
 *
 * @param	struct platform_device *
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int _SCI_Resume(struct platform_device *pPDEV)
{
	return 0;
}


/*
 *	sci platform driver structure
 */
static struct platform_driver _gSCI_PDRV =
{
	.probe		= _SCI_Probe,
	.remove     = _SCI_Remove,
	.suspend	= _SCI_Suspend,
	.resume		= _SCI_Resume,
	.driver		=
	{
		.name   = MODULE_NAME,
	},
};


/*========================================================================================
    Implementation Group : sci Platform Device
========================================================================================*/
/**
 *
 * module remove function. this function will be called in insmod sci module.
 *
 * @param	struct device
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static void _SCI_Release(struct device *pDEV)
{
	SCI_PRINT("released\n");
}

/*
 *	L8 sci platform device structure
 */
static struct platform_device _gSCI_PDEV =
{
	.name = MODULE_NAME,
	.id = 0,
	.dev =
	{
	//	.coherent_dma_mask	= 0xffffffff,
		.release 			= _SCI_Release,
	},
};

static int _SCI_InitSYSFS(void)
{
	int res = 0;

	/* Register SCI driver into the platform bus */
	res = platform_driver_register( &_gSCI_PDRV );
	if ( res == 0)
	{
		res = platform_device_register(&_gSCI_PDEV);
		if(res == 0)
		{
			SCI_PRINT("Smartcard Driver Module Inserted!!!\n");
		}
		else
		{
			platform_driver_unregister(&_gSCI_PDRV);
			return -EIO;
		}
	}
	return 0;
}
static void _SCI_CleanupSYSFS(void)
{
	platform_driver_unregister(&_gSCI_PDRV);
	platform_device_unregister(&_gSCI_PDEV);

	//SCI_Unconfig(gpSCI);
}

/*========================================================================================
    Implementation Group : sci Module
========================================================================================*/
int SCI_Init(void)
{
 	/* Get the handle of debug output for sci device.
	*
	* Most module should open debug handle before the real initialization of module.
	* As you know, debug_util offers 4 independent debug outputs for your device driver.
	* So if you want to use all the debug outputs, you should initialize each debug output
	* using OS_DEBUG_EnableModuleByIndex() function.
	*/
	g_sci_debug_fd = DBG_OPEN(SCI_MODULE);

	if(g_sci_debug_fd < 0)
		return -1;

	OS_DEBUG_EnableModule (g_sci_debug_fd);

	if (gSCI_DbgLvl & SCI_DEBUG_PRINT)
	{
		/* print */
		OS_DEBUG_EnableModuleByIndex(g_sci_debug_fd, 0, DBG_COLOR_NONE);
	}
	if (gSCI_DbgLvl & SCI_DEBUG_TRACE)
	{
		/* trace */
		OS_DEBUG_EnableModuleByIndex(g_sci_debug_fd, 1, DBG_COLOR_CYAN);
	}
	if (gSCI_DbgLvl & SCI_DEBUG_WARN)
	{
		/* Warn */
		OS_DEBUG_EnableModuleByIndex(g_sci_debug_fd, 2, DBG_COLOR_YELLOW);
	}
	if (gSCI_DbgLvl & SCI_DEBUG_ERROR)
	{
		/* Error */
		OS_DEBUG_EnableModuleByIndex(g_sci_debug_fd, 3, DBG_COLOR_RED);
	}


	//FIXME Implement the following
	//if (gSCI_DbgLvl & SCI_DEBUG_ASSERT)

	if (gSCI_DbgLvl & SCI_DEBUG_SYSFS)
	{
		_SCI_InitSYSFS();
	}
	else if (gSCI_DbgLvl & SCI_DEBUG_DEVFS)
	{
		_SCI_InitDEVFS();
	}

	return 0;
}

void SCI_Cleanup(void)
{
	if (gSCI_DbgLvl & SCI_DEBUG_SYSFS)
	{
		_SCI_CleanupSYSFS();
	}
	else if (gSCI_DbgLvl & SCI_DEBUG_DEVFS)
	{
		_SCI_CleanupDEVFS();
	}
}


#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",SCI_Init);
#else
module_init(SCI_Init);
#endif
module_exit(SCI_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("SCI driver");
MODULE_LICENSE("GPL");
#endif

