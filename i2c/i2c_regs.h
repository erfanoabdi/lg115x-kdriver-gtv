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
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     ks.hyun (ks.hyun@lge.com)
 * version    0.1
 * date       2010.11.05
 * note       Additional information.
 *
 */

#ifndef _I2C_REGS_H_
#define _I2C_REGS_H_

#include "base_types.h"

/* Control Register 0 bit definitions I2CMSCTR0 */
#define SET_START_SEQ	0x80
#define SET_STOP_SEQ  	0x40
#define RESTART_SEQ		0x20
#define NTB_1_BYTE		0x00
#define NTB_2_BYTE		0x08
#define NTB_3_BYTE		0x10
#define NTB_4_BYTE		0x18
#define INT_ACK			0x02
#define TRANSFER_GO		0x01

/* Control Register 1 bit definitions I2CMSCTR0 */
#define TRANS_MASTER_TX	0x00
#define TRANS_MASTER_RX	0x40
#define SLAVE_MODE_OP	0x80
#define CORE_ENABLE		0x20
#define INT_ENABLE		0x10
#define CLR_MASTER_FIFO	0x08
#define CLR_SLAVE_FIFO	0x04
#define SOFT_RESET		0x02
#define CLEAR_FIFOS		0x0C
#define CLEAR_REG       0x00

/* FIFO defines */
#define FIFO_EMPTY		0x0
#define FIFO_1BYTE_DATA	0x20 /* 0010-0000 */
#define FIFO_2BYTE_DATA	0x40 /* 0100-0000 */
#define FIFO_3BYTE_DATA	0x60 /* 0110-0000 */
#define FIFO_FULL		0x80 /* 1000-0000 */

/* Master status register */
#define MASTER_STAT_MFS	0xE0
#define MASTER_STAT_MIS	0x04
#define MASTER_STAT_SNB	0x02
#define MASTER_STAT_MTB	0x01


/* Slave status register */
#define SLAVE_STAT_STB	0x01
#define SLAVE_STAT_MNB	0x02
#define SLAVE_STAT_SIS	0x04
#define SLAVE_STAT_TRM	0x08
#define SLAVE_STAT_SAR	0x10
#define SLAVE_STAT_SFS	0xE0


#define LSB8(value) 	(value & 0x00FF)
#define MSB8(value) 	((value & 0xFF00) >> 8)


#define I2C_CLOCK_FREQUENCY_L8		174000000	/* CPU Clock(696Mhz)/4 */

// ieeum.lee --  I2C SPEED 375Khz / 97Khz / 49Khz  // 우선 임의로 수정합니다. 
#if 1
#define I2C_CLOCK_FREQUENCY_L9		222000000	/* DDR Clock(792Mhz)/4 */
#else
#define I2C_CLOCK_FREQUENCY_L9		198000000	/* DDR Clock(792Mhz)/4 */
#endif


/* I2C IRQ */
#define IRQ_I2C0_L8			L8_IRQ_I2C0	/* I2C0 */
#define IRQ_I2C1_L8			L8_IRQ_I2C1	/* I2C1 */
#define IRQ_I2C2_L8			L8_IRQ_I2C2	/* I2C2 */
#define IRQ_I2C3_L8			L8_IRQ_I2C3	/* I2C3 */
#define IRQ_I2C4_L8			L8_IRQ_I2C4	/* I2C4 */
#define IRQ_I2C5_L8			L8_IRQ_I2C5	/* I2C5 */

#define IRQ_I2C0_L9			L9_IRQ_I2C0	/* I2C0 */
#define IRQ_I2C1_L9			L9_IRQ_I2C1	/* I2C1 */
#define IRQ_I2C2_L9			L9_IRQ_I2C2 /* I2C2 */
#define IRQ_I2C3_L9			L9_IRQ_I2C3	/* I2C3 */
#define IRQ_I2C4_L9			L9_IRQ_I2C4	/* I2C4 */
#define IRQ_I2C5_L9			L9_IRQ_I2C5	/* I2C5 */
#define IRQ_I2C6_L9			L9_IRQ_I2C6	/* I2C6 */
#define IRQ_I2C7_L9			L9_IRQ_I2C7	/* I2C7 */
#define IRQ_I2C8_L9			L9_IRQ_I2C8	/* I2C8 */

/* I2C Address */
#define I2C0_REG_BASE_L8			L8_I2C0_BASE
#define I2C1_REG_BASE_L8			L8_I2C1_BASE
#define I2C2_REG_BASE_L8			L8_I2C2_BASE
#define I2C3_REG_BASE_L8			L8_I2C3_BASE
#define I2C4_REG_BASE_L8			L8_I2C4_BASE
#define I2C5_REG_BASE_L8			L8_I2C5_BASE


#define I2C0_REG_BASE_L9			L9_I2C0_BASE
#define I2C1_REG_BASE_L9			L9_I2C1_BASE
#define I2C2_REG_BASE_L9			L9_I2C2_BASE
#define I2C3_REG_BASE_L9			L9_I2C3_BASE
#define I2C4_REG_BASE_L9			L9_I2C4_BASE
#define I2C5_REG_BASE_L9			L9_I2C5_BASE
#define I2C6_REG_BASE_L9			L9_I2C6_BASE
#define I2C7_REG_BASE_L9			L9_I2C7_BASE
#define I2C8_REG_BASE_L9			L9_I2C8_BASE

#define I2C_REG_SIZE				0x30

/* GPIO I2C MACROS */
#define GPIO_BASE(n)				(L8_GPIO0_BASE + (n * 0x100000))

#define GPIO_DATA_REG(n)			(GPIO_BASE(n) + 0x000)
#define GPIO_DIR_REG(n)				(GPIO_BASE(n) + 0x400)

#define L8_GPIO_TOP_CTRL_32_63_REG	(L8_TOP_CTRL + 0x10 + 0x60 + 0)
#define L8_GPIO_TOP_CTRL_64_95_REG	(L8_TOP_CTRL + 0x10 + 0x60 + 4)


#define REG8_WRITE(reg, value)		(*(volatile UINT8*)reg = value)
#define REG8_READ(reg)				(*(volatile UINT8*)reg)

#define REG8_BIT_SET(reg, mask)		(*(volatile UINT8*)reg |= mask)
#define REG8_BIT_CLEAR(reg, mask)	(*(volatile UINT8*)reg &= ~(mask))

#define I2C_GPIO_HIGH		0xFF
#define I2C_GPIO_LOW		0x00

#define I2C_HIGH			1
#define I2C_LOW				0


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
/* I2CMS registers typedefined structure */
typedef struct I2C_REG
{
	volatile UINT32 I2CMMD;		/* Master Data Register */
	volatile UINT32 I2CMSSD;	/* Slave Data Register */
	volatile UINT32 I2CMSSA;	/* Slave Address Register */
	volatile UINT32 I2CMCTR0;	/* Control Register 0 */
	volatile UINT32 I2CMCTR1;	/* Control Register 1 */
	volatile UINT32 I2CMMS;		/* Master Status Register */
	volatile UINT32 I2CMSSS;	/* Slave Status Register */
	volatile UINT32 reserved;	/* Reserved */
	volatile UINT32 I2CMPREH;	/* Clock Pre-Scale Register High */
	volatile UINT32 I2CMPREL;	/* Clock Pre-Scale Register Low */
} I2C_REG_T, *I2C_REG_ADDR_T;


#endif

