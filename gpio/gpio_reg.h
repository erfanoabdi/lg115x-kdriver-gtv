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

#ifndef _GPIO_REG_h
#define _GPIO_REG_h

/********************************************************************
	GPIO0~11 (n : 0~11)
*********************************************************************/

#define GPIONDATA(n)		(*((volatile unsigned *)(GPION_Base(n) + 0x3FC)))
#define GPIONDIR(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x400)))
#define	GPIONIS(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x404)))
#define	GPIONIBE(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x408)))
#define	GPIONIEV(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x40C)))
#define	GPIONIE(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x410)))
#define	GPIONRIS(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x414)))
#define	GPIONMIS(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x418)))
#define	GPIONIC(n)			(*((volatile unsigned *)(GPION_Base(n) + 0x41C)))
#define	GPIONAFSEL(n)		(*((volatile unsigned *)(GPION_Base(n) + 0x420)))
#define	GPIONPeriphID(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFE0)))
#define	GPIONPeriphID1(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFE4)))
#define	GPIONPeriphID2(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFE8)))
#define	GPIONPeriphID3(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFEC)))
#define	GPIONPCellID0(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFF0)))
#define	GPIONPCellID1(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFF4)))
#define	GPIONPCellID2(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFF8)))
#define	GPIONPCellID3(n)	(*((volatile unsigned *)(GPION_Base(n) + 0xFFC)))

#ifdef USE_QEMU_SYSTEM
	#define GPION_Base(n)	0x40000000 + (n * 0x1000)
#else
	#define LX_GPIO0_BASE	L8_GPIO0_BASE
	#define GPION_Base(n)	LX_GPIO0_BASE + (n * 0x100000)
#endif /* USE_QEMU_SYSTEM */

#endif
