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

#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>

static DECLARE_WAIT_QUEUE_HEAD(_wq_EOR);
static DECLARE_WAIT_QUEUE_HEAD(_wq_EOT);
static DECLARE_WAIT_QUEUE_HEAD(_wq_FRF);
static DECLARE_WAIT_QUEUE_HEAD(_wq_FTE);
static DECLARE_WAIT_QUEUE_HEAD(_wq_FRNE);

static volatile int _pSemEOR = 0;
static volatile int _pSemEOT = 0;
static volatile int _pSemFRF = 0;
static volatile int _pSemFTE = 0;
static volatile int _pSemFRNE = 0;

#include <linux/jiffies.h>

#include "l9/linux_irqs.h"
#include "sci_l9_reg.h"
#include "sci_l9_isr.h"


UINT32 intrMask = 0x3F;

UINT8 isCardInserted = 0;

/**
@fn irqreturn_t _sci_isr_handler (int irq, void *dev_id, struct pt_regs *regs)
@brief interrupt service routine
@param irq
@param dev_id
@param regs
@return IRQ_HANDLED
*/
static irqreturn_t _sci_isr_handler (int irq, void *dev_id, struct pt_regs *regs)
{
	UINT8 err = 0;
	UINT32 irqFlag = 0;
	UINT32 statFlag = 0;

	SCI_RdFL(sci_intr_stat);
	irqFlag = SCI_Rd(sci_intr_stat);

	SCI_ERROR("SCI_IRQ : %x\n",irqFlag);
	

	if(irqFlag & SCI_L9_IRQ_CDPWR)
	{
		SCI_RdFL(sci_stat);
		statFlag = SCI_Rd(sci_stat);

		if(statFlag & 0x10)
			isCardInserted = 1;
		else
			isCardInserted = 0;
	 
		_pSemEOR = 1;
		wake_up_interruptible(&_wq_EOR);
		 
		_pSemFRF = 1;
		wake_up_interruptible(&_wq_FRF);
		 
		_pSemEOT = 1;
		wake_up_interruptible(&_wq_EOT);
		 
		_pSemFTE = 1;
		wake_up_interruptible(&_wq_FTE);
		
		_pSemFRNE = 1;
		wake_up_interruptible(&_wq_FRNE);	
	}

	if(irqFlag & SCI_L9_IRQ_ERR)
	{
		SCI_RdFL(sci_err);
		err = SCI_Rd(sci_err);
		
		SCI_ERROR("SCI_IRQ_ERR : %x\n",err);
	}

	if(irqFlag & SCI_L9_IRQ_EOR)
	{
		_pSemEOR = 1;
		wake_up_interruptible(&_wq_EOR);
	}

	if(irqFlag & SCI_L9_IRQ_FRF)
	{
		_pSemFRF = 1;
		wake_up_interruptible(&_wq_FRF);
	}
	
	if(irqFlag & SCI_L9_IRQ_EOT)
	{
		_pSemEOT = 1;
		wake_up_interruptible(&_wq_EOT);
	}

	if(irqFlag & SCI_L9_IRQ_FTE)
	{
		_pSemFTE = 1;
		wake_up_interruptible(&_wq_FTE);
	}

	if(irqFlag & SCI_L9_IRQ_FRNE)
	{
		_pSemFRNE = 1;
		wake_up_interruptible(&_wq_FRNE);	
	}


	if(irqFlag & SCI_L9_IRQ_FTF)
	{
	
	}

	return IRQ_HANDLED;
}

/**
@fn void SCI_L9_MaskIRQ(UINT8 mask)
@brief mask interrupt IRQ
@param mask
@return void
*/
void SCI_L9_EnableIRQ(UINT8 mask)
{
	intrMask &= ~mask;
	SCI_Wr(sci_intr_mask, intrMask);

	SCI_WrFL(sci_intr_mask);
}


/**
@fn void SCI_L9_MaskIRQ(UINT8 mask)
@brief mask interrupt IRQ
@param mask
@return void
*/
void SCI_L9_DisableIRQ(UINT8 mask)
{
	intrMask |= mask;
	SCI_Wr(sci_intr_mask, intrMask);

	SCI_WrFL(sci_intr_mask);
}

/**
@fn SCI_RETURN_T SCI_L9_WaitIRQ (SCI_IRQ_T irq, UINT16 timeout)
@brief wait event with timeout
@param irq
@param timeout
@return SCI_SUCCESS
@return -1 : failed
*/
SCI_RETURN_T SCI_L9_WaitIRQ (SCI_IRQ_T irq, UINT32 timeout)
{
	SINT32 ret;
	
	switch(irq)
	{
		case SCI_L9_IRQ_EOR:
			_pSemEOR = 0;
			ret = wait_event_interruptible_timeout(_wq_EOR, _pSemEOR != 0, msecs_to_jiffies(timeout) );
			break;
		case SCI_L9_IRQ_FRF:
			_pSemFRF = 0;
			ret = wait_event_interruptible_timeout(_wq_FRF, _pSemFRF != 0, msecs_to_jiffies(timeout) );
			break;
		case SCI_L9_IRQ_EOT:
			_pSemEOT = 0;
			ret = wait_event_interruptible_timeout(_wq_EOT, _pSemEOT != 0, msecs_to_jiffies(timeout));
			break;
		case SCI_L9_IRQ_FTE:
			_pSemFTE = 0;
			ret = wait_event_interruptible_timeout(_wq_FTE, _pSemFTE != 0, msecs_to_jiffies(timeout));
			break;
		case SCI_L9_IRQ_FRNE:
			_pSemFRNE = 0;
			ret = wait_event_interruptible_timeout(_wq_FRNE, _pSemFRNE != 0, msecs_to_jiffies(timeout));
			break;
		case SCI_L9_IRQ_FTF:
		default:
			SCI_ERROR("irq[%x] is not supported",irq);
			return SCI_FAIL;
			break;
	}

	if(ret == 0)
		return SCI_FAIL;
	else
		return SCI_SUCCESS;
}

/**
@fn SCI_RETURN_T SCI_L9_InitISR(void)
@brief Init Interrupt setting
@param void
@return SCI_SUCCESS
*/
SCI_RETURN_T SCI_L9_InitISR(void)
{
	UINT32 ret = 0;

	/* register handler with SCI interrupt */
	ret = request_irq(L9_IRQ_SCI, (irq_handler_t)_sci_isr_handler, 0, "sci", NULL);
	if (ret !=0)
	{
		SCI_ERROR("Error[%d] in installing SCI IRQ", ret);
		return SCI_FAIL;
	}

	return SCI_SUCCESS;
}

/**
@fn void SCI_L9_ExitISR(void)
@brief release Interrupt setting
@param void
@return void
*/
void SCI_L9_ExitISR(void)
{
	free_irq(L9_IRQ_SCI, NULL);
}
