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
*                                                          INCLUDES
*******************************************************************************
*/
#include <stdio.h>
#include <string.h>

#include "base_types.h"
#include "cli/cli_main.h"

/*
*******************************************************************************
*                                                          CONSTANTS
*******************************************************************************
*/

/*
*******************************************************************************
*                                                          MACROS
*******************************************************************************
*/

/*
*******************************************************************************
*                                                          LOCAL VARIABLES
*******************************************************************************
*/

/*
*******************************************************************************
*                                                          TABLES
*******************************************************************************
*/

/*
*******************************************************************************
*                                                          LOCAL FUNCTIONS PROTOTYPES
*******************************************************************************
*/
char * My_strstr (const char * str1, const char * str2);
extern void UpperStr (char *Str);
/*
*******************************************************************************
*                                                          GLOBAL FUNCTIONS
*******************************************************************************
*/
extern struct lookup cliCommandTable [];

/*
*******************************************************************************
*                                    HELP COMMAND
* FUNCTION            : help
* DESCRIPTION      : This function displays the commands supported. Any new commands
*                                    supported other than those present must have an entry here.
* ARGUMENTS         : None
* RETURNS               : None
*******************************************************************************
*/
int help (int argc, char **argv)
{
	unsigned int ui32Count = 0;
	unsigned int ui32Ctr = 0;
	char  acTemp[20];
	char  acTemp1[20];
	char *psResult ;
	BOOLEAN fFlag = FALSE;
	unsigned int ui32CmdCnt = 0;

	if(argc == 2)
	{
		printf("\n\n Help for '%s'\n", argv[1]);
		memset(acTemp,0,20);
		memset(acTemp1,0,20);
		for(ui32Ctr = 0;ui32Ctr < strlen(argv[1]);ui32Ctr++)
		{
			if(argv[1][ui32Ctr] != '*')
				acTemp[ui32Ctr] = argv[1][ui32Ctr];
			else if(argv[1][ui32Ctr] == '*')
			{
				fFlag = TRUE;
				break;
			}
		}

		UpperStr(acTemp);
		while (ui32Count < MAX_CLI_CMDS)
		{
			if(*(cliCommandTable[ui32Count].CmdStr) != 0x0 )
			{
				if(fFlag)
				{
					strcpy(acTemp1, cliCommandTable[ui32Count].CmdStr);
					psResult = My_strstr(acTemp1, acTemp);
					if(psResult != 0)
					{
						ui32CmdCnt++;
						printf( "\r\n %s", cliCommandTable[ui32Count].CmdStr);
					}
				}
				else
				{
					if(strcmp(acTemp, cliCommandTable[ui32Count].CmdStr) == 0)
					{
						ui32CmdCnt++;
						printf( "\r\n %s", cliCommandTable[ui32Count].CmdStr);
					}
				}

				ui32Count++;
			}
			else
				break;
		}

		printf( "\r\n\n No of Commands found = ");
		if(ui32CmdCnt > 0)
			printf( "%d\n", ui32CmdCnt);
		else
			printf( "%d\n", ui32CmdCnt);

	}
	else if(argc == 1)
	{
		printf("\n\n---------------------------------------------------- \n");
		printf(
			"\n\n"
			"  Memory dump 32bit mode  : md   <Addr> <No of Dwords>\n"
			"  Memory dump 16bit mode  : md16 <Addr> <No of Words>\n"
			"  Memory dump 8bit  mode  : md8  <Addr> <No of bytes>\n"
			"  Memory Write Byte(8bit) : mw8  <Addr> <Value> Write to Memory, Byte-Size\n"
			"                            ex) mw8 0xA1200000 0x12\n"
			"  Memory Write Word(16bit): mw16 <Addr> <Value> Write to Memory, Word-Size\n"
			"                            ex) mw16 0xA1200000 0x1234\n"
			"  Memory Write Long(32bit): mw32 <Addr> <Value> Write to Memory/Register,\n"
			"                            DWord-Size\n"
			"                            ex) mw32 0xA1200000 0x12345678\n"
			"  Physical Memory dump 32bit mode : pmd <Addr> <No of Dwords>\n"
			"  Read Memory location    : rd  <Addr> Reads Memory or Register\n"
			"  Write Memory location   : wr  <Addr> <Value> Writes to Memory/Register\n"
			"  Memory Fill             : mfill  <Addr> <Value> <No of bytes> \n"
			"  Memory pattern test     : mpt <Addr> <No of DWords>\n"
			"  Memory Bit test         : mbt <Addr> <No of Dwords>\n"
			"  Memory Compare          : mcmp      <Source Addr> <Dest Addr> <No of Dwords>\n"
			"  Memory Word Mode Test   : memtst    <Start address> <No of DWords>\n"
			"  Memory Bulk Mode Test   : memtstblk <Start address> <No of DWords>\n"
			"  Memory Random Mode Test : memrand   <Start address> <No of DWords>\n"
			"  Total Memory Test       : memtstfull <DDRStartAddr> <DDR End> <SRAM Start> <SRAM Bound>\n"
			"  Memory Random Bulk Mode Test : memblkrand <Start address> <No of DWords>\n"
			"  Command List            : cmdlist - Displays all commands\n"
			"  Help Command            : help - Displays basic memory operation commands\n"
			"                            with description\n"
			"  Find Command String     : help <some chars from command string>* - Displays all\n"
			"                            commands having provided chars as a subset\n"
			"  Find Command            : help <Command string> - Displays command string\n"
			);

		printf("\n\n----------------------------------------------------\n");
	}

	return 0;
}


char * My_strstr (const char * str1, const char * str2)
{
        char *cp = (char *) str1;
        char *s1, *s2;

        if ( !*str2 )
            return((char *)str1);

        while (*cp)
        {
                s1 = cp;
                s2 = (char *) str2;

                while ( *s1 && *s2 && !(*s1-*s2) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return(NULL);

}

