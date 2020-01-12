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

#include <asm/io.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include "sci_l9_reg.h"
#include "./../sci_cfg.h"

volatile SCI_REG_T* gpRegSCI;
volatile SCI_REG_T* gpRealRegSCI;

static struct SCI_MEM_T memL9Ax =
{
	.base = 0xFE700000,
	.size = 0x1000,
};

/**
@fn void SCI_L9_WriteReg(UINT8 addr, UINT8 data)
@brief Write a data into a register of smartcard interface device
@param addr	: address for a data to be written
@param data : data to be written
@return void
*/
SCI_RETURN_T SCI_L9_WriteRegister(LX_SCI_REG* pREG)
{
	((UINT32*)gpRealRegSCI)[pREG->regIndex] = pREG->data;

	return SCI_SUCCESS;
}

/**
@fn UINT8 SCI_L9_ReadReg(UINT8 addr)
@brief Read a data from a register of smartcard interface device
@param addr	: address for a data to be retrieven
@return retrieven data
*/
SCI_RETURN_T SCI_L9_ReadRegister(LX_SCI_REG* pREG)
{
	pREG->data = ((UINT32*)gpRealRegSCI)[pREG->regIndex];

	return SCI_SUCCESS;
}

/**
@fn void SCI_L9_WriteReg(UINT8 addr, UINT8 data)
@brief Write a data into a register of smartcard interface device
@param addr	: address for a data to be written
@param data : data to be written
@return void
*/
SCI_RETURN_T SCI_L9_InitRegister(struct SCI_T* pSCI)
{
	SCI_RETURN_T ret = SCI_SUCCESS;
	int res = 0;

	/* Init SCI Register access */
	if(pSCI->chip == LX_CHIP_REV_L9_A0 ||
		pSCI->chip == LX_CHIP_REV_L9_A1 ||
		pSCI->chip == LX_CHIP_REV_L9_B0)
	{
		pSCI->p_iomem.base = memL9Ax.base;
		pSCI->p_iomem.size = memL9Ax.size;
	}

	res = check_mem_region(pSCI->p_iomem.base, pSCI->p_iomem.size);
	if ( res != 0 )
	{
		SCI_ERROR("Error %d: Allocation for I/O memory is failed,\n", res);
		ret = SCI_NOMEM;
		return ret;
	}
	request_mem_region(pSCI->p_iomem.base, pSCI->p_iomem.size, SCI_MODULE);
	gpRealRegSCI = ioremap(pSCI->p_iomem.base, pSCI->p_iomem.size);
	if (gpRealRegSCI == NULL)
	{
		SCI_ERROR("error on %d : \n", __LINE__);
		ret = SCI_NOMEM;
		return ret;
	}
	pSCI->v_iomem.base = (UINT32)gpRealRegSCI;
	pSCI->v_iomem.size = pSCI->p_iomem.size;

	/* Init Shadow SCI Register access */
	gpRegSCI = (SCI_REG_T*)kmalloc(sizeof(SCI_REG_T), GFP_KERNEL);
	if (gpRegSCI == NULL)
	{
		SCI_ERROR("error on %d : \n", __LINE__);
		ret = SCI_NOMEM;
		release_mem_region(pSCI->p_iomem.base, pSCI->p_iomem.size);
		iounmap( (UINT32*)pSCI->v_iomem.base );
	}

	return SCI_SUCCESS;
}

/**
@fn void SCI_L9_WriteReg(UINT8 addr, UINT8 data)
@brief Write a data into a register of smartcard interface device
@param addr	: address for a data to be written
@param data : data to be written
@return void
*/
void SCI_L9_ExitRegister(struct SCI_T* pSCI)
{
	release_mem_region(pSCI->p_iomem.base, pSCI->p_iomem.size);
	pSCI->p_iomem.base = 0;
	iounmap( (UINT32*)pSCI->v_iomem.base );
	pSCI->v_iomem.base = 0;
	kfree((void *)gpRegSCI);
	gpRegSCI = NULL;
}

