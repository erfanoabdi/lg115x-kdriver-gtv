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

#if	!defined(CLI_MAIN_H)
#define CLI_MAIN_H

//#include "Common/DDI_DTV.h"
/*
*******************************************************************************
*                                                          CONSTANTS
*******************************************************************************
*/

/*
*******************************************************************************
*                                                         GLOBAL  MACROS
*******************************************************************************
*/
#ifdef CLI_GLOBALS
#define EXT
#else
#define EXT extern
#endif

#define DEFALUT_DIGITAL_X 16
#define MAX_CLI_CMDS 1000
/*
*******************************************************************************
*                                                          GLOBAL DATA TYPES
*******************************************************************************
*/
typedef int CLI_FN (int argc, char **argv);

struct lookup /* Lookup table structure */
{
	char * CmdStr; /* command string */
	unsigned int cmdId;
    /* pointer to command  coressponding function  */
	int (* func) (int argc, char **argv);
};

typedef int (*funcPtr)(int argc, char **argv);

typedef struct
{
 	char CmdString[20]; /* command string */
    /* pointer to command  coressponding function  */
	funcPtr CmdFuncPtr;
} CLI_COMMAND_TABLE;

/*
*******************************************************************************
*                                                          GLOBAL VARIABLES
*******************************************************************************
*/



/*
*******************************************************************************
*                                                          EXTERNALS
*******************************************************************************
*/

/*
*******************************************************************************
*                                                          GLOBAL FUNCTION PROTOTYPES
*******************************************************************************
*/
int Add_CLI_Cmd (char * pCmdName, int (*fn) (int argc, char **argv));
int Register_CLI_CmdTable (CLI_COMMAND_TABLE * pstrCLICommandTable);
EXT int  parse_args  (char *, char **);
void cli_start (void);
void cli_join (void);
void cli_parser_spawn (void);
void UpperStr       (char *);
int  his_append     (char *);
int  his_set        (char *, int);
int  gets_his       (char *s);
int  help    	    (int , char **);
int  memdump 	    (int , char **);
int  memdumpbyte    (int , char **);
int  memdumpword    (int , char **);
int  mem_wrt_long	(int , char **);
int  mem_wrt_word	(int , char **);
int  mem_wrt_char	(int , char **);
int	 Read_Memory    (int , char **);
int	 Write_Memory   (int , char **);
int  memory_fill 	(int argc, char **argv);
int	Read_Register (int argc, char **argv);
int	Write_Register (int argc, char **argv);
int MemoryWriteContinous32(int argc, char **argv);
int	Write_MemoryFromFile   (int , char **);
int memdump_phys (int argc, char **argv);	// added by SC Jung
int TT_REGTest(int argc, char **argv);
//int  TT_OSStat(int argc, char **argv);
int  memwr   		(int , char **);
int CLI_OSDelay (int argc, char **argv);
int 	Soft_Reset( void );
void CONSOLE_PrintTitle (const char *title, char	cUnderlineChar );
int Add_GLI_Cmd (char * pCmdName, unsigned int cmdId, int (*fn) (int argc, char **argv));
int runScriptFile(char *runFileName);
unsigned int GetDigitX(char *pString);
//int UartGetChar ( FILE *fp );
int convGUI2CLI (char *s);

int MemoryWrite(UINT32 i_uiPhysicalAdd, UINT32 i_uiValue);
int MemoryRead(UINT32 i_uiPhyAdd);

#endif /* End of CLI_MAIN_H */

