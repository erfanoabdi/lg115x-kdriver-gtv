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


/*
*******************************************************************************
*                                    INCLUDES
*******************************************************************************
*/
//#include <includes.h>
//#include <typedefs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "base_types.h"

#include "cli/cli_main.h"
#include "dbg/dbg_def.h"
#include "top/test_mem.h"

/*
*******************************************************************************
*                                    CONSTANTS
*******************************************************************************
*/

/*
*******************************************************************************
*                                    MACROS
*******************************************************************************
*/

/*
*******************************************************************************
*                                    LOCAL VARIABLES
*******************************************************************************
*/
static unsigned long int last_addr = 0x0;

/*
*******************************************************************************
*                                    TABLES
*******************************************************************************
*/

/*
*******************************************************************************
*                                    LOCAL FUNCTIONS PROTOTYPES
*******************************************************************************
*/

/*
*******************************************************************************
*                                    GLOBAL FUNCTIONS
*******************************************************************************
*/
//extern int gets(char *s);
int MemoryWriteWidth(unsigned int i_uiPhysicalAdd, unsigned int i_uiWidth, unsigned int i_uiValue);
int MemoryWrite(unsigned int i_uiPhysicalAdd, unsigned int i_uiValue);
int MemoryRead(unsigned int i_uiPhyAdd);
int PhysMemoryRead(unsigned int i_uiPhyAddr, unsigned int i_uiLength , unsigned int *i_pdata);

/*
*******************************************************************************
*                                    MEMORY WRITE
* FUNCTION         : memwr
* DESCRIPTION      : Writes to a memory location
* ARGUMENTS        : Address, Value
* RETURNS          : 0
*******************************************************************************
*/
int memwr (int argc, char ** argv)
{
	long int value;
	volatile unsigned int * ptr = (volatile unsigned int *) strtoul( argv[1], NULL, 0);
	value = strtoul( argv[2], NULL, 0);
	*ptr = value;
	printf ("\n\n Value at address : 0x%x  is 0x%x\n", (unsigned int)ptr, (unsigned int)*ptr);
	return RET_OK;
}

/*
*******************************************************************************
*                                    MEMORY DUMP
* FUNCTION         :  memdump
* DESCRIPTION      : This function dumps the contents of the memory location specified for the
*                    no of bytes specified. It also displays the ascii equivalent values.
* ARGUMENTS        : Address, Value
* RETURNS          : 0 on Success, -1 on error
*******************************************************************************
*/
#if 00 //> Modified by kimjm in 7/8/2009 3:52:22 PM <<<{b}>>>
//> int memdump (int argc, char **argv)
//> {
//> 	int index, index1;
//> 	volatile unsigned int *ptr;
//> 	volatile unsigned int ascptr[4];
//> 	unsigned int ui32asc;
//> 	unsigned char asc[4];
//> 	long int noDWords = 0;
//> 	int i;
//> 	unsigned int value;
//>
//> 	do {
//> 		if (argc < 2)
//> 		{
//> 			printf ("USAGE: md <Address> <Number of DWords>\n");
//> 			return RET_ERROR;
//> 		}
//>
//> 		last_addr = strtoul( argv[1], NULL, 0);
//> 		ptr = (volatile unsigned int *)last_addr;
//>
//> 		if (argc == 2)
//> 		{
//> 			noDWords  = 20;
//> 		}
//> 		else
//> 			noDWords  = strtoul( argv[2], NULL, 0);
//>
//> 		if (noDWords == 0)
//> 		{
//> 			printf ("USAGE: md <Address> <Number of DWords>\n");
//> 			return RET_ERROR;
//> 		}
//>
//> 		printf ("\n   ");
//> 		noDWords = ((noDWords+3)/4)*4;
//> 		for (index = 0; index <noDWords; index++)
//> 		{
//> 			if((index %4) == 0) printf (" 0x%X: ", (unsigned int)ptr);
//> 			value = MemoryRead((unsigned int)ptr++);
//> 			ascptr[index%4] = value;
//> 			printf (" 0x%08X", value);
//> 			if ((index %4) == 3)
//> 			{
//> 				printf ("  |");
//> 				for (index1 = 0; index1 < 4; index1++)
//> 				{
//> 					ui32asc = ascptr[index1];
//> 					memcpy((char *)asc, (char *)&ui32asc, 4);
//> 					for (i=0;i<4;i++)
//> 					{
//> 						if (asc[i] > 0x1f && asc[i] < 0x7f)
//> 							printf ("%c", asc[i]);
//> 						else
//> 							printf (".");
//> 					}
//> 				}
//> 				printf ("|\n   ");
//> 			}
//>
//> 		}
//> 		printf ("\n");
//> 	} while (0);
//> 	return RET_OK;
//> }
#endif //> Modified by kimjm in 7/8/2009 3:52:22 PM <<<{e}>>>
int memdump (int argc, char **argv)
{
	int ret = RET_OK;
	int index, index1;
	volatile unsigned int *ptr;
	volatile unsigned int ascptr[4];
	unsigned int ui32asc;
	unsigned char asc[4];
	long int noDWords = 0;
	int i;
	unsigned int value;
	char *fname;
	FILE *wFile = NULL;

	do {
		if (argc < 2)
		{
			ret = RET_ERROR;
			break;
		}

		last_addr = GetDigitX(argv[1]);
		ptr = (volatile unsigned int *)last_addr;

		noDWords = (argc>2)?GetDigitX(argv[2]):20;

		if (noDWords == 0) {
			ret = RET_ERROR;
			break;
		}

		if (argc == 4) {
			fname = argv[3];
			wFile = fopen((char *)fname,"wb");
			if( wFile == NULL ) {
				ffprintf("Can't open the file %s\n", fname);
				break;
			}
		}

		noDWords = ((noDWords+3)/4)*4;

#if 1	//Yet to complete simple dumping to improve performance
		//Murugan - 16.Sep.09 - Simplified to write in one shot
		if (argc == 4) {
			unsigned int MapPhysicalMemory(unsigned int i_uiPhyAddr, unsigned int i_uiLength, unsigned int **o_uiUserSpaceAddr);
			unsigned int	*pvusermem;

			printf ( "Dumping %d words, PhysAddr[0x%x]...", (int) noDWords, (unsigned int) ptr );
			/* Get a user space virtual address for the given physical address */
			MapPhysicalMemory((unsigned int)ptr, noDWords * 4, &pvusermem );
			fwrite((unsigned int *) pvusermem, 4, noDWords, wFile);
			printf ( "Complete !!\n");
		}
		else
#endif	/* #if 0 */
		{
			for (index = 0; index <noDWords; index++,ptr++)
			{
				value = MemoryRead((unsigned int)ptr);
				if (argc == 4) {
					fwrite(&value, sizeof(value), 1, wFile);
					continue;
				}
				ascptr[index%4] = value;
				if((index %4) == 0) printf ("\n    0x%X: ", (unsigned int)ptr);
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
		if (argc == 3) printf ("\n");
		if (argc == 4) fclose(wFile);
	} while (0);
	printf ("\n");

	if (ret) {
		printf ("USAGE:\n");
		printf ("   md <Address>\n");
		printf ("or md <Address> <Number of DWords>\n");
		printf ("or md <Address> <Number of DWords> <filename to be dumped>\n");
	}
	return RET_OK;
}

/*
*******************************************************************************
*                            PHYSICAL MEMORY DUMP BYTE MODE
* FUNCTION       : memdump_phys
* DESCRIPTION    : This function dumps the contents of the physical memory for the
*                  no of bytes specified.
* ARGUMENTS      : Address, Value
* RETURNS        : 0 on success, -1 on error
*******************************************************************************
*/
int memdump_phys (int argc, char **argv)
{
	int ret = RET_OK;
	int index;
//    int index1;
	volatile unsigned int *ptr;
//    volatile unsigned int ascptr[4];
//    unsigned int ui32asc;
//    unsigned char asc[4];
	long int noDWords = 0;
	int i;
	unsigned int *value = NULL;
//    char *fname;
//    FILE *wFile = NULL;

	do {
		if (argc < 2)
		{
			ret = RET_ERROR;
			break;
		}

		last_addr = GetDigitX(argv[1]);
		ptr = (volatile unsigned int *)last_addr;

		noDWords = (argc>2)?GetDigitX(argv[2]):20;

		if (noDWords == 0) {
			ret = RET_ERROR;
			break;
		}


		noDWords = ((noDWords+3)/4)*4;

		value = (unsigned int *)malloc(sizeof(unsigned int)*noDWords);
		PhysMemoryRead((unsigned int)ptr , noDWords , value );


		for(index = 0 ; index < noDWords ; index += 4 , ptr += 4)
		{

			printf ("\n    0x%X: ", (unsigned int)ptr);
			for(i=0 ; i < 4 ; i++){
				printf (" 0x%08X", *value++);
			}
		}

		//free((void *)value);


		printf ("\n");
	}while(0);



	if (ret) {
		printf ("USAGE:\n");
		printf ("   mdp <Address>\n");
		printf ("or mdp <Address> <Number of DWords>\n");
	}
	return RET_OK;
}


/*
*******************************************************************************
*                                    MEMORY DUMP BYTE MODE
* FUNCTION       : memdumpbyte
* DESCRIPTION    : This function dumps the contents of the memory in byte mode for the
*                  no of bytes specified.
* ARGUMENTS      : Address, Value
* RETURNS        : 0 on success, -1 on error
*******************************************************************************
*/
int memdumpbyte (int argc, char **argv)
{
	unsigned int nobytes = 0;
	volatile unsigned int * ptr;
	unsigned int ui32mem;
	unsigned char ui8mem[4];
	int index, index1, bmode;

	if (argc != 3)
	{
		printf ("\n\n USAGE: md8 <Address> <Number of bytes>\n");
		return RET_ERROR;
	}

	last_addr = strtoul( argv[1], NULL, 0);
	nobytes  = strtoul( argv[2], NULL, 0);

	if (nobytes == 0)
	{
		printf ("\n\n USAGE: md8 <Address> <Number of bytes>\n");
		return RET_ERROR;
	}

	ptr = (volatile unsigned int *)last_addr;

	printf (" \n   ");

	nobytes = ((nobytes+3)/4);
	for (index = 0; index <nobytes; index++)
	{
		printf (" 0x%X: ", (unsigned int)ptr);
		ui32mem = MemoryRead((unsigned int)ptr++);
		memcpy((char *)ui8mem, (char *)&ui32mem, 4);
		for (bmode = 0; bmode <4; bmode++)
			printf (" 0x%02X", (ui8mem[bmode]));

		printf ("  |");
		for (index1=0;index1<4;index1++)
		{
			if (ui8mem[index1] > 0x1f && ui8mem[index1] < 0x7f)
				printf ("%c", ui8mem[index1]);
			else
				printf (".");
		}
		printf ("|\n   ");
//        printf (" 0x%X: ", (unsigned int)ptr);
	}
	return RET_OK;
}

/*
*******************************************************************************
*                                    MEMORY DUMP WORD MODE
* FUNCTION       : memdumpword
* DESCRIPTION    : This function dumps the contents of the memory in word mode for the
*                  no of bytes specified.
* ARGUMENTS      : Address, Value
* RETURNS        : 0 on success, -1 on error
*******************************************************************************
*/
int memdumpword (int argc, char **argv)
{
	unsigned int noWords = 0;
	volatile unsigned int * ptr;
	unsigned int ui32mem;
	unsigned short ui16mem[2];
	unsigned char ui8mem[4];
	int index, index1, bmode;

	if (argc != 3)
	{
		printf ("\n\n USAGE: md8 <Address> <Number of bytes>\n");
		return RET_ERROR;
	}

	last_addr = strtoul( argv[1], NULL, 0);
	noWords  = strtoul( argv[2], NULL, 0);

	if (noWords == 0)
	{
		printf ("\n\n USAGE: md16 <Address> <Number of Words>\n");
		return RET_ERROR;
	}

	ptr = (volatile unsigned int *)last_addr;

	printf (" \n   ");
//    printf (" 0x%X: ", (unsigned int)ptr);

	noWords = ((noWords+3)/4)*2;
	for (index = 0; index < noWords; index++)
	{
		if((index%2) == 0) printf (" 0x%X: ", (unsigned int)ptr);
		ui32mem = MemoryRead((unsigned int)ptr++);
		memcpy((char *)ui16mem, (char *)&ui32mem, 4);
		for (bmode = 0; bmode < 2; bmode++)
			printf (" 0x%04X", (ui16mem[bmode]));
		if ((index %2) == 1)
		{
			printf ("  |");
			for (bmode = 0; bmode < 2; bmode++)
			{
//                ui32mem = ui16mem[bmode];
				memcpy((char *)ui8mem, (char *)ui16mem, 4);

				for (index1=0;index1<4;index1++)
				{
					if (ui8mem[index1] > 0x1f && ui8mem[index1] < 0x7f)
						printf ("%c", ui8mem[index1]);
					else
						printf (".");
				}
			}
			printf ("|\n   ");
//            printf (" 0x%X: ", (unsigned int)ptr);
		}
	}
	return RET_OK;
}

/*
*******************************************************************************
*                                    MEMORY WRITE BYTE MODE
* FUNCTION        : mem_wrt_char
* DESCRIPTION     : This function is used for byte writes into memory locations
* ARGUMENTS       : Address, Value
* RETURNS         : 0 on sucess, -1 on error
*******************************************************************************
*/
int mem_wrt_char(int argc, char **argv)
{
    unsigned int addr;
    unsigned int val;
	int          lp;

	if( argc < 3 )
	{
		printf( "Argument Count Error!\n");
		return RET_ERROR;
	}

    addr = GetDigitX( argv[1] );

	for( lp = 2; lp < argc; lp++ ) {
        val = GetDigitX( argv[lp] );
        MemoryWriteWidth(addr, 1, val);
        addr += 1;
    }

	return RET_OK;
}

/*
*******************************************************************************
*                                    MEMORY WRITE WORD MODE
* FUNCTION       : mem_wrt_word
* DESCRIPTION    : This function is used to perform word writes into memory locations
* ARGUMENTS      : Address, Value
* RETURNS        : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
int mem_wrt_word(int argc, char **argv)
{
	unsigned int addr;
    unsigned int val;
	int            lp;

	if( argc < 3 )
	{
		printf( "Argument Count Error!\n");
		return RET_ERROR;
	}

    addr = GetDigitX( argv[1] );

	for( lp = 2; lp < argc; lp++ ) {
        val = GetDigitX( argv[lp] );
        MemoryWriteWidth(addr, 2, val);
        addr += 2;
    }

	return RET_OK;
}

/*
*******************************************************************************
*                                    MEMORY WRITE LONG MODE
* FUNCTION        : mem_wrt_long
* DESCRIPTION     : This function is used to perform 32bit  writes into memory locations
* ARGUMENTS       : Address, Value
* RETURNS         : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
int mem_wrt_long(int argc, char **argv)
{
    unsigned int addr;
    unsigned int val;
	int          lp;

	if( argc < 3 )
	{
		printf( "Argument Count Error!\n");
		return RET_ERROR;
	}

    addr = GetDigitX( argv[1] );

	for( lp = 2; lp < argc; lp++ )
    {
        val = GetDigitX( argv[lp] );
        MemoryWriteWidth(addr, 4, val);
        addr += 4;
    }

	return RET_OK;
}

/*
*******************************************************************************
*                                    READ MEMORY
* FUNCTION         : Read_Memory
* DESCRIPTION      : This routine is a general read routine that returns the content of a memory location
* ARGUMENTS        : Address, Value
* RETURNS          : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
int	Read_Memory (int argc, char **argv)
{
	unsigned int addr, reg;
	unsigned int size, i;

	if( argc < 2 )
	{
		printf( "Argument Count Error!\n");
		return RET_ERROR;
	}

	addr = GetDigitX( argv[1] );
	if(argc == 2) {
		reg = MemoryRead(addr);
		printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr, reg);
	}
	else if (argc == 3)
	{
		size = GetDigitX( argv[2] );
		for(i=0; i<=size; i+=4) {
            reg = MemoryRead(addr+i);
			printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr+i, reg);
		}
	}

	return RET_OK;
}

/*
*******************************************************************************
*                                    WRITE TO MEMORY
* FUNCTION        : Write_Memory
* DESCRIPTION     : This routine is a general write routine that writes to the address specified
* ARGUMENTS       : Address, Value
* RETURNS         : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
int	Write_Memory (int argc, char **argv)
{
    unsigned int addr, val;

	if( argc < 3 )
	{
		printf( "mw <addr> <data>\n");
		return RET_ERROR;
	}

    addr = (unsigned int)GetDigitX( argv[1] );
    val  = (unsigned int)GetDigitX( argv[2] );

    if (MemoryWrite(addr, val) == RET_OK)
		printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr, val);

    return RET_OK;
}

/*
*******************************************************************************
*                                    MEMORY FILL
* FUNCTION      : memory_fill
* DESCRIPTION   : This routine is used to fill the memory region with the value given. The memory is
*                 filled from the start address for the number of bytes mentioned with the value
*                 specified
* ARGUMENTS     : Address, Value, No of Words
* RETURNS       : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
int memory_fill (int argc, char **argv)
{

    unsigned int   addr;
    unsigned int   noDWords;
	unsigned int   value;
	unsigned int   index;

    if ( argc < 4 )
    {
		printf ("\n\n USAGE: mfill <Address> <Value> <Number of DWords>\n");
        return RET_ERROR;
    }

    addr 	 = GetDigitX( argv[1] );
	value 	 = GetDigitX( argv[2] );
	noDWords = GetDigitX( argv[3] );

	if (noDWords == 0)
	{
		printf ("\n\n USAGE: mf <Address> <Value> <Number of DWords>\n");
		return RET_ERROR;
	}

	/*memcpy (ptrTo, value, nobytes);*/
	for (index = 0; index < noDWords; index++)
	{
		MemoryWrite(addr, value);
		addr += 4;
	}

	return RET_OK;
}

/*
*******************************************************************************
*                   INTERACTIVE MEMORY WRITE
* FUNCTION         : MemoryWriteContinous32
* DESCRIPTION      : tbd
* ARGUMENTS        : tbd
* RETURNS          : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
int MemoryWriteContinous32(int argc, char **argv)
{
	unsigned int	*ptrTo;
	unsigned char	aucBuffer [ 80 ];

	if( argc < 2 )
	{
		printf( "Argument Count Error!\n");
		return RET_ERROR;
	}

	ptrTo = (unsigned int *)strtoul( (const char * ) argv[1], NULL, 0);

	while ( 1 )
	{
		// Print current data
		printf( "%04X-%04X :",  (((unsigned int)ptrTo)>>16)&0xFFFF,
                                 ((unsigned int)ptrTo)     &0xFFFF  );
		printf( " %04X-%04X - ",  ((*ptrTo)>>16)&0xFFFF,
                                 ((unsigned int)(*ptrTo)     &0xFFFF)  );

		scanf ( ( char * ) aucBuffer , "%s");

		printf ( "\n" );

		/* if a '.' is entered, terminate the loop */
		if ( aucBuffer [0] == '.' )
		{
			break;
		}

		/* Skip the current address if left blank */
		if ( aucBuffer [0] != '\0' )
		{
			/* Assign the modified data */
			*ptrTo = strtoul( (const char * ) aucBuffer, NULL, 0 );
		}

		/* Move to next address and continue editing */
		++ptrTo;
	}

	return RET_OK;
}

int TT_REGTest(int argc, char **argv)
{
	unsigned int *base_reg;
	unsigned int reg_size, length, i;
	unsigned int *test_reg[2];

	if( argc < 2 )
	{
		printf( "reg_test [base_addr] [size]!\n");
		return RET_ERROR;
	}

	base_reg = (unsigned int *)strtoul( (const char * ) argv[1], NULL, 0);
	reg_size = (unsigned int)strtoul( (const char * ) argv[2], NULL, 0);

	if((reg_size%4)!=0)
	{
		reg_size=(reg_size+4)&0xFFFFFFFC;
	}

	if(reg_size==0)
		printf( "you have to set size more than 0!\n");

	length = (unsigned int) (reg_size/4);

	test_reg[0] = (unsigned int *)malloc(length*4);
	memset((char *)test_reg[0], 0x0, length*4);

	test_reg[1] = (unsigned int *)malloc(length*4);
	memset((char *)test_reg[1], 0x0, length*4);

	memcpy((char *)test_reg[0], (char *)base_reg, length*4);
	memset((char *)base_reg, 0xFF, length*4);
	memcpy((char *)test_reg[1], (char *)base_reg, length*4);

	printf("\n\nRegister R/W Test (%08x:%d)\n============================>>>\n", (unsigned int)base_reg, (unsigned int)length);
	for(i=0; i<length; i++)
	{
		printf("%08x : %08x - %08x\n", (unsigned int)(base_reg+i), (unsigned int)test_reg[0][i], (unsigned int)test_reg[1][i]);
	}
	printf("<<<============================\n");

	free(test_reg[0]);
	free(test_reg[1]);

	return 0;
}

/*
*******************************************************************************
*                                    READ Register
* FUNCTION         : Read_Register
* DESCRIPTION      : This routine is a general read routine that returns the content of a Register location
* ARGUMENTS        : Address, Value
* RETURNS          : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
#if 00 //> Modified by kimjm in 7/8/2009 3:21:40 PM <<<{b}>>>
//> int	Read_Register (int argc, char **argv)
//> {
//> 	unsigned int addr, reg;
//> 	unsigned int size, i;
//>
//> 	if( argc < 2 ) {
//> 		printf( "Argument Count Error!\n");
//> 		return RET_ERROR;
//> 	}
//>
//> 	addr = GetDigitX( argv[1] );
//> 	if (addr <= 0xffff) addr += DTV_SoC_VIRTUAL_BASE;
//>
//> 	if(argc == 2) {
//>         if (addr <= 0xffff) addr += DTV_SoC_VIRTUAL_BASE;
//>         reg = MemoryRead(addr);
//> 		printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr, reg);
//> 	}
//> 	else if (argc == 3)
//> 	{
//> 		size = GetDigitX( argv[2] );
//>         if (addr <= 0xffff) addr += DTV_SoC_VIRTUAL_BASE;
//> 		for(i=0; i<=size; i+=4) {
//>             reg = MemoryRead(addr+i);
//> 			printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr+i, reg);
//> 		}
//> 	}
//>
//> 	return RET_OK;
//> }
#endif //> Modified by kimjm in 7/8/2009 3:21:40 PM <<<{e}>>>

int	Read_Register (int argc, char **argv)
{
	unsigned int addr, reg;
	unsigned int size, i;
	char *fname;
	FILE *wFile = NULL;

	do {
		if( argc < 2 ) {
			printf( "usage :\n");
			printf( "RD <address> or\n");
			printf( "RD <address> <size> or\n");
			printf( "RD <address> <size> <filename>\n");
			break;
		}

		addr = GetDigitX( argv[1] );
		if (addr <= 0xffff) addr += DTV_SoC_VIRTUAL_BASE;
		if (argc == 2) {
			reg = MemoryRead(addr);
			printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr, reg);
			break;
		}
		size = (argc>2)?GetDigitX(argv[2]):1;
		if (argc == 4) {
			fname = argv[3];
			wFile = fopen((char *)fname,"wb");
			if( wFile == NULL ) {
				ffprintf("Can't open the file %s\n", fname);
				break;
			}
		}

		for(i=0; i<=size; i+=4) {
            reg = MemoryRead(addr+i);
			if (argc == 3) {
				printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr+i, reg);
			} else if (argc == 4) {
				fwrite(&reg, sizeof(reg), 1, wFile);
			}
		}
		if (argc == 4) fclose(wFile);
	} while (0);

	return RET_OK;
}

/*
*******************************************************************************
*                                    WRITE TO Register
* FUNCTION        : Write_Register
* DESCRIPTION     : This routine is a general write routine that writes to the address specified
* ARGUMENTS       : Address, Value
* RETURNS         : RET_OK on success, RET_ERROR on error
*******************************************************************************
*/
int	Write_Register (int argc, char **argv)
{
    unsigned int addr, val;

	if( argc < 3 ) {
		printf( "Argument Count Error!\n");
		return RET_ERROR;
	}

    addr = GetDigitX( argv[1] );
    val  = GetDigitX( argv[2] );

	if (addr <= 0xffff) addr += DTV_SoC_VIRTUAL_BASE;

    if ( MemoryWrite(addr, val) == RET_OK)
		printf("Address[ 0x%08x ], Vaule[ 0x%08x ]\n", addr, val);

    return RET_OK;
}

int	Write_MemoryFromFile (int argc, char **argv)
{
	int ret = RET_OK;
    unsigned int addr, val;
	int rc;
	char *fname;
	FILE *rFile = NULL;

	do {
		if( argc < 3 )
		{
			printf( "Usage : FMW <addr> <filename to dump memory>\n");
			ret = RET_ERROR;
			break;
		}

		addr  = (unsigned int)GetDigitX( argv[1] );
		fname = argv[2];
		rFile = fopen((char *)fname,"rb");
		if( rFile == NULL ) {
			ffprintf("Can't open the file %s\n", fname);
			break;
		}
		while(1) {
			rc = fread(&val,4,1, rFile);
			if (rc < 1) break;
			ret = MemoryWrite(addr, val);
			if (ret) break;
			addr+=4;
		}
	} while (0);

    return ret;
}

