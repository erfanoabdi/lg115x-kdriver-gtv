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

#ifndef _TEST_MEM_h
#	define _TEST_MEM_h

#	ifdef USE_XTENSA											// Tensilica
#      define USE_XTENSA_XLMI
#	   define DTVSOC_VIRT_PL301_BASE		0xf0000000
#	   define DTVSOC_XLMI_BASE		        0x3ffc0000
#ifdef USE_XTENSA_XLMI
#	   define DTVSOC_DE_BASE				(DTVSOC_XLMI_BASE       + 0x2000)
#else
#	   define DTVSOC_DE_BASE				(DTVSOC_VIRT_PL301_BASE + 0x2000)
#endif
#	elif (USE_QEMU_KERNEL > 0 )									// Kernel Level for QEMU
#	   define DTVSOC_VIRT_PL301_BASE		0x40000000
#	   define DTVSOC_DE_BASE				0x40000000
#	elif (USE_QEMU_PROGRAM > 0 )								// Program for QEUM
#	   define DTVSOC_VIRT_PL301_BASE		0x40000000
#	   define DTVSOC_DE_BASE				0x40000000
#	elif !(USE_LINUX_KERNEL > 0)								// Board for User Level
#	   define DTVSOC_VIRT_PL301_BASE		0xf6000000
#      define DTVSOC_DE_BASE				(DTVSOC_VIRT_PL301_BASE+0x16000)
#	endif

#	if (USE_QEMU_KERNEL > 0 ) || !(USE_LINUX_KERNEL > 0)								// Board for User Level
#	   define DTVSOC_VDEC_BASE				(DTVSOC_VIRT_PL301_BASE + 0x9000)
#	   define DTVSOC_VENC_BASE				(DTVSOC_VIRT_PL301_BASE + 0x9400)
#	   define DTVSOC_GFX_BASE				(DTVSOC_VIRT_PL301_BASE + 0x8000)
#   endif

#	define DTVSOC_IPC_TOTAL_SIZE			0x100
#	define DTVSOC_IPC_FROM_CPU_BASE			(DTVSOC_DE_BASE + 0x1E00)
#	define DTVSOC_IPC_FROM_CPU_SIZE			(DTVSOC_IPC_TOTAL_SIZE/2)
#	define DTVSOC_IPC_FROM_MCU_BASE			(DTVSOC_IPC_FROM_CPU_BASE + DTVSOC_IPC_FROM_CPU_SIZE)
#	define DTVSOC_IPC_FROM_MCU_SIZE			(DTVSOC_IPC_TOTAL_SIZE/2)

#define DTV_SoC_VIRTUAL_BASE DTVSOC_VIRT_PL301_BASE
#define DTV_SoC_VIRTUAL_SIZE 0x30000

#define REG_WRITE8(  addr, value )	 ( *( volatile unsigned char  * )( addr ) ) = ( volatile unsigned char	)( value )
#define REG_WRITE16( addr, value )	 ( *( volatile unsigned short * )( addr ) ) = ( volatile unsigned short )( value )
#define REG_WRITE32( addr, value )	 ( *( volatile unsigned int * )( addr ) ) = ( volatile unsigned int )( value )
#define REG_WRITE64( addr, value )	 ( *( volatile long long * )( addr ) ) = ( volatile long long )( value )

#define REG_READ16( addr )			( *( volatile unsigned short * )( addr ) )
#define REG_READ32( addr )			( *( volatile unsigned int * )( addr ) )
#define REG_READ64( addr )			( *( volatile long long * )( addr ) )

#ifdef USE_XTENSA
//#define MemoryWrite( addr, value )	 ( *( volatile UINT32 * )( addr ) ) = ( volatile UINT32 )( value )
//#define MemoryRead( addr )			 ( *( volatile UINT32 * )( addr ) )
#endif

//#define DDR_WD( addr, value )	     ( *( UINT32 * )( addr ) ) = ( UINT32 )( value )
//#define DDR_RD( addr )			     ( *( UINT32 * )( addr ) )

#endif
