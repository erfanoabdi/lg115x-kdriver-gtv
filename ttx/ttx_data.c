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
 *	TTX module control Drivers linux module
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version		1.0
 *  date
 *  note		Additional information.
 *
 *  @addtogroup lg1150_TTX
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>	/**< printk() */
#include <linux/slab.h> 	/**< kmalloc() */
#include <linux/fs.h> 		/**< everything\ldots{} */
#include <linux/types.h> 	/**< size_t */
#include <linux/fcntl.h>	/**< O_ATTXMODE */
#include <asm/uaccess.h>
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <asm/io.h>			/**< For ioremap_nocache */
#include <base_types.h>

#include "ttx_module.h"
#include "ttx_hw.h"
#include "ttx_drv.h"
#include "ttx_data.h"
#include "ttx_cfg.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
volatile unsigned long gTTX_data_mem_addr;


unsigned int gTTXDataMemoryStart;
unsigned int gTTXDataMemorySize	;

unsigned int gTTXVpsMemoryStart;
unsigned int gTTXVpsMemorySize	;

unsigned int gTTXWssMemoryStart;
unsigned int gTTXWssMemorySize;

unsigned int gTTXHeaderMemoryStart;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static sBufInform *buf;
static sBufInform cc_buf;

/*========================================================================================
	Implementation Group
========================================================================================*/

/**
 *
 * TTX_irq_handler irq handler
 *
 * @param	irq , device id , regs
 * @return	0 : OK , -1 : NOK
 *
*/


int TTX_InitDataMemory(void)
{
	int ret = 0;

	gTTXDataMemoryStart	= 0;
	gTTXDataMemorySize	= (gstMemCfgTTX->buf_memory_size - 0x70);

	gTTXVpsMemoryStart	= gTTXDataMemoryStart + gTTXDataMemorySize;
	gTTXVpsMemorySize	= 16;

	gTTXWssMemoryStart	= gTTXVpsMemoryStart + gTTXVpsMemorySize;
	gTTXWssMemorySize	= 16;

	gTTXHeaderMemoryStart	= gTTXWssMemoryStart + gTTXWssMemorySize;

	if(gstMemCfgTTX->buf_memory_size > 0)
		gTTX_data_mem_addr = (volatile unsigned long)ioremap(gstMemCfgTTX->buf_memory_base , gstMemCfgTTX->buf_memory_size);

	// buffer header initialize
	buf = (sBufInform *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart);
	buf->header = 0;
	buf->tailer = 0;
	buf->count = 0;
	buf->VPS_offset = gTTXVpsMemoryStart;
	buf->WSS_offset = gTTXWssMemoryStart;

	TTX_PRINT("%s done VPS offset [%x] WSS offset [%x] \n", __func__ , buf->VPS_offset , buf->WSS_offset);
	return ret;
}

int TTX_UninitDataMemory(void)
{
	int ret = 0;

	// buffer header initialize
	buf->header = 0;
	buf->tailer = 0;
	buf->count = 0;
	buf->VPS_offset = 0;
	buf->WSS_offset = 0;
	buf->status = 0;

	iounmap((void *)gTTX_data_mem_addr);

	return ret;
}


int TTX_GetMemorySize(void)
{
	return gstMemCfgTTX->buf_memory_size;
}

int TTX_GetDataAddr(sHeaderData *data)
{
	int ret = 0;

	switch(data->type)
	{
		// by wonsik.do TTX data is contained both in odd and even fileds.
		case 0x6:	// teletext
		case 0x7:	// teletext

			if(buf->tailer >= (gTTXDataMemorySize	- TTX_DATA_SIZE))
			{
				buf->tailer = 0;
			}
			else
			{
				buf->tailer += TTX_DATA_SIZE;
			}

			data->addr = gTTX_data_mem_addr + buf->tailer;

			if(buf->tailer == buf->header) buf->status |= LX_TTX_TYPE_TTX; //(1<<0);

			//memcpy((void *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart) , (void *)&buf , sizeof(sBufInform));
			//memcpy((void *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart + 4) , (void *)&buf.tailer , sizeof(int));
			buf->count++;
			//TTX_PRINT("TTX came %x %x\n", buf->tailer,gTTX_data_mem_addr + gTTXHeaderMemoryStart);
		break;

		// by wonsik.do WSS and VPS are contained only in odd filed.
		//case 0xc:	// VPS
		case 0xd:	// VPS
			data->addr = gTTX_data_mem_addr + gTTXVpsMemoryStart;
			buf->count++;
			//memcpy((void *)&buf.status, (void *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart + 20) , sizeof(int));
			buf->status |= LX_TTX_TYPE_VPS; //(1<<0);
			//memcpy((void *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart + 20) , (void *)&buf.status , sizeof(int));
//			TTX_PRINT("VPS came\n");
		break;

		// by wonsik.do WSS and VPS are contained only in odd filed.
		//case 0x8:	// WSS
		case 0x9:	// WSS
			data->addr = gTTX_data_mem_addr + gTTXWssMemoryStart;
			buf->count++;
			//memcpy((void *)&buf.status, (void *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart + 20) , sizeof(int));
			buf->status |= LX_TTX_TYPE_WSS; //(1<<0);
			//memcpy((void *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart + 20) , (void *)&buf.status , sizeof(int));
//			TTX_PRINT("WSS came\n");
		break;

		default:
//			TTX_PRINT("%d came\n" , data->type);
			data->addr = 0;
			ret = -1;
		break;

	}



	return ret;
}

int TTX_ClearTTXMemory(void)
{
	int ret = 0;

	buf->header = 0;
	buf->tailer = 0;
	buf->count = 0;

	//memcpy((void *)(gTTX_data_mem_addr + gTTXHeaderMemoryStart) , (void *)&buf , sizeof(sBufInform));

	return ret;
}

int TTX_ClearVPSMemory(void)
{
	int ret = 0;

	memset((void *)(gTTX_data_mem_addr + buf->VPS_offset), 0 , gTTXVpsMemorySize);

	return ret;
}


int TTX_ClearWSSMemory(void)
{
	int ret = 0;

	memset((void *)(gTTX_data_mem_addr + buf->WSS_offset), 0 , gTTXWssMemorySize);

	return ret;
}

int TTX_ClearAllMemory(void)
{
	int ret = 0;

	TTX_ClearTTXMemory();
	TTX_ClearVPSMemory();
	TTX_ClearWSSMemory();

	return ret;
}



/* won.hur : Code for CC is as below : */

int TTX_CC_PutVBIData(unsigned int *data , unsigned int size)
{
	int ret = 0;

	if(gTTX_data_mem_addr == 0) return -1;

	memcpy((void *)(gTTX_data_mem_addr + cc_buf.tailer) , (void *)data , size*4 );

	cc_buf.tailer += size*4;
	if(cc_buf.tailer >= gstMemCfgTTX->buf_memory_size)
	{
		cc_buf.tailer = 0;
	}

	//CC_PRINT("put data [%08x]\n", *data);
	cc_buf.count++;
	return ret;
}

int TTX_CC_GetVBIData(LX_CC_DATA_T *cc_data , unsigned int size)
{
	int ret = 0;

	if(gTTX_data_mem_addr == 0) return -1;

	if(cc_buf.header != cc_buf.tailer)
	{
		//memcpy( (void *)cc_data , (void *)(gTTX_data_mem_addr + cc_buf.header), 4 );
		cc_data->data1 = *(unsigned int*)(gTTX_data_mem_addr + cc_buf.header) & 0xFF;
		cc_data->data2 = (*(unsigned int*)(gTTX_data_mem_addr + cc_buf.header) & 0xFF00) >> 8;
		cc_data->field = (*(unsigned int*)(gTTX_data_mem_addr + cc_buf.header) & 0x10000)>>16;
		cc_data->status = (*(unsigned int*)(gTTX_data_mem_addr + cc_buf.header) & 0xE0000)>>17;
		cc_data->line_number = (*(unsigned int*)(gTTX_data_mem_addr + cc_buf.header) & 0xFFC00000)>>22;
		//CC_PRINT("line number : [%d] , data : [0x%x]\n", cc_data->line_number, *(unsigned int*)(gTTX_data_mem_addr + cc_buf.header) );
		cc_buf.header += 4;
		if(cc_buf.header >= gstMemCfgTTX->buf_memory_size)
		{
			cc_buf.header = 0;
		}

		if(cc_buf.count != 0)
			cc_buf.count--;
	}
	else
	{
		cc_data->data1 = 0;
		cc_data->data2 = 0;
		cc_data->field = 0;
		cc_data->status = 0;
		cc_data->line_number = 0;
		ret = -1;
	}

	return ret;
}

int TTX_CC_InitDataMemory(void)
{
	int ret = 0;
	// buffer header initialize
	cc_buf.header = 0;
	cc_buf.tailer = 0;
	cc_buf.count = 0;

//	gTTX_data_mem_addr = (volatile unsigned long)ioremap(CC_DATA_MEMORY_START , CC_DATA_MEMORY_SIZE);
	gTTX_data_mem_addr = (volatile unsigned long)ioremap(gstMemCfgTTX->buf_memory_base , gstMemCfgTTX->buf_memory_size);

	CC_PRINT("%s done base addr [0x%x] size [0x%x] \n", __func__ , gstMemCfgTTX->buf_memory_base , gstMemCfgTTX->buf_memory_size);

	return ret;
}

int TTX_CC_UninitDataMemory(void)
{
	int ret = 0;

	// buffer header initialize
	cc_buf.header = 0;
	cc_buf.tailer = 0;
	cc_buf.count = 0;

	iounmap((void *)gTTX_data_mem_addr);

	CC_PRINT("%s done\n", __func__);

	gTTX_data_mem_addr = 0;
	return ret;
}


int TTX_CC_GetVBIBufferInform(void)
{
	return ((int)cc_buf.count);
}

int TTX_CC_SetVBIBufferClear(void)
{
	int ret = 0;

	cc_buf.header = 0;
	cc_buf.tailer = 0;
	cc_buf.count = 0;

	return ret;
}

unsigned int TTX_CheckDataParity(unsigned int data)
{
	int ret = 0;
	unsigned char data0, data1;

	int i = 0;
	UINT8 parity_check0 = 0;
	UINT8 parity_check1 = 0;
	UINT8 Mask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

	data0 = (unsigned char)((data & 0x0000FF00)>>8);
	data1 = (unsigned char)((data & 0x000000FF));

	parity_check0 = (data0 & Mask[i]);
	parity_check1 = (data1 & Mask[i]);

	for(i=1; i<8; i++)
	{
		parity_check0 ^= (data0 & Mask[i]) >> i;
		parity_check1 ^= (data1 & Mask[i]) >> i;
	}

	if((parity_check0 == 1)&&(parity_check1 == 1)) ret = 1;
	else ret = 0;

	return ret;
}

unsigned int TTX_GetVBIDataLineCount(unsigned int *data)
{
	unsigned int ret = 0;
	unsigned int data_line_count_lsb = *(data);

	ret = (data_line_count_lsb&0x000003FF);

	return ret;
}




/** @} */
