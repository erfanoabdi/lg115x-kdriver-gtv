#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/mman.h>

#include "SampleDrv.h"

#define	UINT32	unsigned int
#define	NOT_OK	-1

/**
 *	Application function to demostrate dumping of
 *	memory
 */
int PhysMemoryRead(UINT32 i_uiPhyAddr, UINT32 i_uiLength , UINT32 *i_pdata)
{
	SPhysReadParams *pParam;
	
	int iResult = 0;
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return 0;
	}
	
	pParam = (SPhysReadParams *)malloc(sizeof(SPhysReadParams));
	
	
	pParam->uiMemOffset = i_uiPhyAddr;
	pParam->uiMemSize = i_uiLength;
	pParam->pData = (UINT32 *)i_pdata;
	
	iResult = ioctl(iFd, SAMPLE_IOC_READ_PHYS_MEMORY, pParam);


	close(iFd);
	
	free(pParam);
	
	return iResult;
}

/**
 *	\struct SPhysReadParams
 *	Structure to pass RegisterRead() API parameters
 */


/**
 *	<shripad 22-june-06>Function to read memory
 */
int MemoryRead(UINT32 i_uiPhyAdd)
{
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	SRegisterReadParams sRegisterReadParams;

	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return 0xDEADCAFE;
	}

	sRegisterReadParams.uiRegOffset = i_uiPhyAdd;
	ioctl(iFd,  SAMPLE_IOC_READ_MEMORY,&sRegisterReadParams);
	close(iFd);
	return sRegisterReadParams.uiReadValue;
}

/*
  *	Function to write to any location in the memory
  *	It is required  for writing ROM files to memory ( 32 bit )
  */
int MemoryWrite(UINT32 i_uiPhysicalAdd, UINT32 i_uiValue)
{

	/* I am using SRegisterWriteParams struct here. Since members are same */
	int iResult = 0;
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return NOT_OK;
	}

	SRegisterWriteParams sRegisterWriteParams;
	sRegisterWriteParams.uiRegOffset = i_uiPhysicalAdd;
	sRegisterWriteParams.uiWidth = 4;
	sRegisterWriteParams.uiValue = i_uiValue;
	iResult = ioctl(iFd,  SAMPLE_IOC_WRITE_MEMORY, &sRegisterWriteParams);
	close(iFd);
	return iResult;
}

/*
  *	Function to write to any location in the memory
  *	It is required  for writing ROM files to memory ( char/short/int )
  */
int MemoryWriteWidth(UINT32 i_uiPhysicalAdd, UINT32 i_uiWidth, UINT32 i_uiValue)
{

	/* I am using SRegisterWriteParams struct here. Since members are same */
	int iResult = 0;
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return NOT_OK;
	}

	SRegisterWriteParams sRegisterWriteParams;
	sRegisterWriteParams.uiRegOffset = i_uiPhysicalAdd;
	sRegisterWriteParams.uiWidth = i_uiWidth;
	sRegisterWriteParams.uiValue = i_uiValue;
	iResult = ioctl(iFd,  SAMPLE_IOC_WRITE_MEMORY, &sRegisterWriteParams);
	close(iFd);
	return iResult;
}

/**
 *	Balaji - 07092006 - Function to read group of memory locations
 */
int MemoryReadGroup(
				UINT32	*i_puiAddress,
				UINT32	*o_puiValue,
				UINT32	i_uiNumberOfReads )
{
	int iFd;
	SRegisterGRWParams sGrpReadParams;

	iFd = open ( SAMPLE_DEV_NAME, O_RDWR );
	if( iFd == -1 )
	{
		printf ( "ERROR- Could not open the device:%s\n", SAMPLE_DEV_NAME );
		return 0;
	}

	sGrpReadParams.puiAddress = i_puiAddress;
	sGrpReadParams.puiValue	  = o_puiValue;
	sGrpReadParams.uiSizeToRW = i_uiNumberOfReads;
	ioctl ( iFd, SAMPLE_IOC_READG_MEMORY, &sGrpReadParams );

	close ( iFd );

	return i_uiNumberOfReads;
}

/**
 *	Balaji - 07092006 - Function to write group of memory locations
 */
int MemoryWriteGroup(
				UINT32	*i_puiAddress,
				UINT32	*i_puiValue,
				UINT32	i_uiNumberOfReads )
{
	int iFd;
	SRegisterGRWParams sGrpReadParams;

	iFd = open ( SAMPLE_DEV_NAME, O_RDWR );
	if( iFd == -1 )
	{
		printf ( "ERROR- Could not open the device:%s\n", SAMPLE_DEV_NAME );
		return 0;
	}

	sGrpReadParams.puiAddress = i_puiAddress;
	sGrpReadParams.puiValue	  = i_puiValue;
	sGrpReadParams.uiSizeToRW = i_uiNumberOfReads;
	ioctl ( iFd, SAMPLE_IOC_WRITEG_MEMORY, &sGrpReadParams );

	close ( iFd );

	return i_uiNumberOfReads;
}

/**
 *	Application function to demonstrate writing of
 *	registers using ioctl
 */
int RegisterWrite(UINT32 i_uiRegOffset, UINT32 i_uiValue)
{
	int iResult = 0;
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return 0;
	}
	SRegisterWriteParams sRegisterWriteParams;
	sRegisterWriteParams.uiRegOffset = i_uiRegOffset;
	sRegisterWriteParams.uiValue = i_uiValue;
	iResult = ioctl(iFd, SAMPLE_IOC_WRITEREG, &sRegisterWriteParams);
	close(iFd);
	return iResult;
}

/**
 *	Application function to demonstrate reading registers
 *	using ioctl calls
 */
int RegisterRead(UINT32 i_uiRegOffset)
{
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	SRegisterReadParams sRegisterReadParams;

	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return 0;
	}
	sRegisterReadParams.uiRegOffset = i_uiRegOffset;
	ioctl(iFd, SAMPLE_IOC_READREG, &sRegisterReadParams);
	close(iFd);
	return sRegisterReadParams.uiReadValue;
}

/**
 *	Application function to demostrate dumping of
 *	memory
 */
int DumpMemory(UINT32 i_uiPhyAddr, UINT32 i_uiLength)
{
	int iResult = 0;
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return 0;
	}
	SDumpMemoryParams sDumpMemoryParams;
	sDumpMemoryParams.uiPhyAddr = i_uiPhyAddr;
	sDumpMemoryParams.uiLength = i_uiLength;
	iResult = ioctl(iFd, SAMPLE_IOC_DUMPMEM, &sDumpMemoryParams);
	close(iFd);
	return iResult;
}


/**
 * Murugan-22/09/09
 * Added for faster memory dumping and copying support by directly mapping the physical memory
 * to user space. Function to map the given physical memory to 
 * user virtual space. After this call, user can 
 * directly read/write to the user virtual space address
 * This will increase the copying/dumping performance
 */
int MapPhysicalMemory(UINT32 i_uiPhyAddr, UINT32 i_uiLength, UINT32 **o_uiUserSpaceAddr)
{
	int iResult = 0;
	SDumpMemoryParams sDumpMemoryParams;
	void 	*pvUserVirtAddr;
	
	int iFd = open(SAMPLE_DEV_NAME, O_RDWR);
	if( iFd == -1 )
	{
		printf("ERROR- Could not open the device:%s\n",SAMPLE_DEV_NAME);
		return 0;
	}

	/* This command will pass the physical memory parameters to the Memory module for mapping*/
	sDumpMemoryParams.uiPhyAddr = i_uiPhyAddr;
	sDumpMemoryParams.uiLength = i_uiLength;
	iResult = ioctl(iFd, SAMPLE_IOC_DUMPMEM, &sDumpMemoryParams);

	/* This function will do the real physical to user space mapping */
	pvUserVirtAddr = mmap (
			0,	/* offset from phys memory */
			i_uiLength,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			iFd,
			0 ); 

	printf ( "Mapped virtual address [0x%08X]\n", (UINT32) pvUserVirtAddr );

	/* Pass the mapped user virtual address to the calling function */
	*o_uiUserSpaceAddr = pvUserVirtAddr;

#if 0
	/* Below code for printing the user space mapped contents */
	{
		int index, index1, i ;
		volatile unsigned int ascptr[4];
		unsigned int ui32asc, value;
		unsigned char asc[4];

		UINT32	*ptr = (UINT32 *) pvUserVirtAddr;
		
		for (index = 0; index <i_uiLength / 4; index++,ptr++)
		{
			value = *ptr;
			ascptr[index%4] = value;
			if((index %4) == 0) printf ("\n    0x%X: ", (UINT32)ptr);
			printf (" 0x%08X", value);
			if ((index %4) == 3)
			{
				printf ("  |");
				for (index1 = 0; index1 < 4; index1++)
				{
					ui32asc = ascptr[index1];
					memcpy((char *)asc, (char *)&ui32asc, 4);
					for (i=0;i<4;i++)
					{
						if (asc[i] > 0x1f && asc[i] < 0x7f)
							printf ("%c", asc[i]);
						else
							printf (".");
					}
				}
				printf ("|");
			}

		}
	}
#endif /* #if 0 */

	close(iFd);
	return iResult;
}



