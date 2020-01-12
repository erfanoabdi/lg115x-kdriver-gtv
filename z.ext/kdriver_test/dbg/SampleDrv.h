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

/******************************************************************************
 * (c)Copyright 2002. All Rights Reserved
 * LG Soft India Private Limited
 * Cherry Hills
 * Embassy Golf Link Business Park
 * Bangalore - 560017
 * India.
 * Project Name			: RUKH Verfication
 * Developed by 		: LGE ISL ( India Software Lab )
 * Security			: Confidential
 *****************************************************************************/

/******************************************************************************
 * Filename			: SampleDrv.h
 * Purpose			: Defines Sample driver related functions
 *				  / data types
 * Platform			: Linux-2.6.11.11
 * Author(s)			: Kaustubh Sarwate
 * e-mail			: kaustubh.sarwate@lgsoftindia.com
 * Creation date		: 25/05/2006	14:00:00 PM
 * Modified By			:
 * Modification description(s) :
 *****************************************************************************/

#ifndef __RUKH_SAMPLE_DRV_H__
#define __RUKH_SAMPLE_DRV_H__

#include <linux/kernel.h>

/******************************************************************************
 *	Defines
 */
#define	BUFFER_LENGTH		8
#define SAMPLE_DEV_NAME		"SampleMod"
#define	SAMPLE_MAJOR		244
#define NUM_OF_REGISTERS	50  /*<Shripad 18 June> Just for testing*/
//#define SAMPLE_DEV_NAME		"/dev/rukh0"
//#define	SAMPLE_MAJOR		254
/*Shripad(17-june-06)- 254 is used by init_mod.ko */
//#define  SAMPLE_MAJOR			255 Added in the Rukh_Major.h
#define  SAMPLE_IOC_READREG	_IOWR(SAMPLE_MAJOR, 0, void *)
#define  SAMPLE_IOC_WRITEREG	_IOWR(SAMPLE_MAJOR, 1, void *)
#define  SAMPLE_IOC_DUMPMEM	_IOWR(SAMPLE_MAJOR, 2, void *)
#define  SAMPLE_IOC_MULTITASK	_IOWR(SAMPLE_MAJOR, 3, void *)
#define  SAMPLE_IOC_WRITE_MEMORY	_IOWR(SAMPLE_MAJOR, 4, void *)
#define  SAMPLE_IOC_READ_MEMORY	_IOWR(SAMPLE_MAJOR, 5, void *)
#define  SAMPLE_IOC_READG_MEMORY		_IOWR(SAMPLE_MAJOR, 6, void *)
#define  SAMPLE_IOC_WRITEG_MEMORY	_IOWR(SAMPLE_MAJOR, 7, void *)

#define  SAMPLE_IOC_READ_PHYS_MEMORY _IOWR(SAMPLE_MAJOR, 8, void *)

/******************************************************************************
 *	Structures and Enums
 */

/**
 *	\struct SRukhSampleRegisters
 *	Structure indicating the resgiters of a device
 */
typedef struct S_RUKH_SAMPLE_REGISTERS
{
	unsigned int	uiRegister[NUM_OF_REGISTERS];	/**< Video Register Bank */
}SRukhSampleRegisters;

/**
 *	Global data to be accessed in the driver
 */
extern SRukhSampleRegisters * psRukhSampleRegisters;
extern unsigned int g_uiBaseAddress;

/******************************************************************************
 *	Function Prototypes
 */

/**
 *	\struct SRegisterReadParams
 *	Structure to pass RegisterRead() API parameters
 */
typedef struct S_REGISTER_READ_PARAMS
{
	unsigned int	uiRegOffset;	/**< Register address offset to be read */
	unsigned int  uiReadValue;	/**< Register value read in ioctl implementation */
}SRegisterReadParams;

/**
 *	\struct SPhysReadParams
 *	Structure to pass RegisterRead() API parameters
 */
typedef struct S_PHYSMEM_READ_PARAMS
{
	unsigned int	uiMemOffset;	/**< Register address offset to be read */
	unsigned int  uiMemSize;	/**< Register value read in ioctl implementation */
	unsigned int  *pData;
}SPhysReadParams;

/**
 *	\struct SRegisterGRWParams
 *	Structure to pass to group read/write () API parameters
 */
typedef struct S_REGISTER_GROUP_READ_WRITE_PARAMS
{
	unsigned int	*puiAddress;
	unsigned int 	*puiValue;
	unsigned int	uiSizeToRW;
} SRegisterGRWParams;

/**
 *	Function to demonstrate reading registers
 */
int RegisterRead(unsigned int i_uiRegOffset);

/**
 *	\struct SRegisterWriteParams
 *	Structure to pass RegisterWrite() API parameters
 */
typedef struct S_REGISTER_WRITE_PARAMS
{
	unsigned int	uiRegOffset;	/**< Register address offset to write */
	unsigned int	uiWidth;	        /**< Width to be written */
	unsigned int	uiValue;	    /**< Value to be written */
}SRegisterWriteParams;

/**
 *	Driver function to demonstrate writing of
 *	regsiters
 */
int RegisterWrite(unsigned int i_uiRegOffset, unsigned int i_uiValue);

/**
 *	\struct SDumpMemoryParams
 *	Structure to pass DumpMemory() API parameters
 */
typedef struct S_DUMP_MEMORY_PARAMS
{
	unsigned int	uiPhyAddr;	/**< Physical address to dump */
	unsigned int	uiLength;	/**< Number of bytes to be dumped */
}SDumpMemoryParams;

/**
 *	Driver function to demonstrate writing of
 *	regsiters
 */
int DumpMemory(unsigned int i_uiPhyAddr, unsigned int i_uiLength);

/**
 *	Driver function to demonstrate multitasking abilities
 */
unsigned int TestMultiTasking(void);

/**
 *	Function to write to any location in the memory
 *	It is required  for writing ROM files to memory ( 32 bit )
 */
int MemoryWrite(unsigned int i_uiPhysicalAdd, unsigned int i_uiValue);
int MemoryWriteWidth(unsigned int i_uiPhysicalAdd, unsigned int i_uiWidth, unsigned int i_uiValue);

/**
 *	Function to read memory
 */
int MemoryRead(unsigned int i_uiPhyAdd);




// added the code by SC Jung 09/11/28
int	PhysMemoryRead(unsigned int i_uiPhysicalAdd , unsigned int i_uiPhysicalSize , unsigned int *data);


/**
 *	Function to read from any location in the memory
 *	Reads a group of memory locations
 */
int MemoryReadGroup (
				unsigned int	*i_puiAddress,
				unsigned int	*o_puiValue,
				unsigned int	i_uiNumberOfReads );

/**
 *	Function to write from any location in the memory
 *	writes a group of memory locations
 */
int MemoryWriteGroup (
				unsigned int	*i_puiAddress,
				unsigned int	*i_puiValue,
				unsigned int	i_uiNumberOfWrites );


/**
 *	Function to map the given physical memory to 
 *	user virtual space. After this call, user can 
 *  directly read/write to the user virtual space address
 *  This will increase the copying/dumping performance
 */
int MapPhysicalMemory(unsigned int i_uiPhyAddr, unsigned int i_uiLength, unsigned int **o_uiUserSpaceAddr);

#endif
