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
*														   INCLUDES
*******************************************************************************
*/
#define CLI_GLOBALS
//#include <includes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include "base_types.h"
#include "dbg/dbg_def.h"
#include "cli/cli_main.h"
#include "cli/cli_list.h"

/*
*******************************************************************************
*														   CONSTANTS
*******************************************************************************
*/


#ifdef EXCEPT_XDP
//#define	CL_XDP_GetStatus() 1
#endif

#define ADEC_MENU_CMDS

#define CLI_STACKSIZE	SZ_16K /* SZ_8K */
#define MAX_LENGTH_CLI_CMD 50

#define MAX_CLI_CMD_LENGTH	2048

#define RegWrite32(reg, data) (*((volatile unsigned int *)(reg)) = (data))

/*
*******************************************************************************
*														   MACROS
*******************************************************************************
*/

/*
*******************************************************************************
*														   LOCAL VARIABLES
*******************************************************************************
*/
//LOCAL unsigned int CLIStack[CLI_STACKSIZE];
/*
*******************************************************************************
*														   TABLES
*******************************************************************************
*/
/*
 * cliCommandTable is where the commands are looked up, Any new commands
 * to be supported must have their entries updated here. All command
 * functions implemented must accept variable arguments
 */
struct lookup cliCommandTable [MAX_CLI_CMDS];

/*
*******************************************************************************
*														   LOCAL FUNCTIONS PROTOTYPES
*******************************************************************************
*/
static void cli_parser (void *tmp);
static void cli_banner (void);
int Script_DataSet (int argc, char ** argv);
int CLI_runScriptFile(int argc, char **argv);
unsigned int GetBaseX(char *pString);

/*
*******************************************************************************
*														   GLOBAL FUNCTIONS
*******************************************************************************
*/

//BOOLEAN fOSProfile = FALSE;
static	pthread_t	stCLI_thread;
static LIST_T *spFirstScript;

/*
*******************************************************************************
*									 ADD COMMANDS TO CLI TABLE
* FUNCTION		: Register_CLI_CmdTable
* DESCRIPTION	: This function can be called from any file to add a group of
*				  CLI commands to the global CLI comand table.
*
* Example		: In your file define a table of the commands you would like to
*				  have as shown below
*
*				  CLI_COMMAND_TABLE MyCmdTable [] =
*							{
*							  //Name od command, address of function to be called
*						  {"CMD1", function_pointer_1},
*							  {"CMD2", function_pointer_2},
*							  {"CMD3", function_pointer_3},
*							  {0x0	 , 0x0}  //This is required to define end of list
*							};
*
*				 Now call the function in any of the initialization function
*				 of the respective module as shown below to register the commands
*
*				 Register_CLI_CmdTable (MyCmdTable);
*
*							   (OR)
*
*				 You can use Add_CLI_Cmd (<cmd name>, <function address>) to add
*				 one command at a time.
*
* ARGUMENTS		: CLI command name, function name
* RETURNS		: RET_OK on Success, RET_ERROR on error
*******************************************************************************
*/


int Register_CLI_CmdTable (CLI_COMMAND_TABLE * pstrCLICommandTable)
{

	unsigned int u32TableIndex   = 0;
	unsigned int u32CmdIndex	   = 0;
	int    cmdAdded		   = 0x0;
	unsigned int u32CmdTableSize = 0x0;

	for (u32CmdIndex=0;u32CmdIndex < u32CmdTableSize; u32CmdIndex++)
	{
		for (u32TableIndex = 0; u32TableIndex < (MAX_CLI_CMDS-1); u32TableIndex++)
		{
			if ((cliCommandTable[u32TableIndex].func == pstrCLICommandTable[u32CmdIndex].CmdFuncPtr) &&
					(!(strcmp(cliCommandTable[u32TableIndex].CmdStr, pstrCLICommandTable[u32CmdIndex].CmdString))))
			{
				ffprintf ("Command <<<	%s	>>> Already exists\n", pstrCLICommandTable[u32CmdIndex].CmdString);
				cmdAdded = 0x1;
				break;
			}

			if ((cliCommandTable[u32TableIndex].func) == 0x0)
			{
				strcpy (cliCommandTable[u32TableIndex].CmdStr, pstrCLICommandTable[u32CmdIndex].CmdString);
				cliCommandTable[u32TableIndex].func = pstrCLICommandTable[u32CmdIndex].CmdFuncPtr;
				cmdAdded = 0x1;
				break;
			}
		}
		if (!cmdAdded)
		{
			ffprintf ("Command <<<	%s	>>> could NOT be added, CLI table is Full\n", pstrCLICommandTable[u32CmdIndex].CmdString);
		}
		cmdAdded = 0x0;
	}
	return RET_OK;
}


/*
*******************************************************************************
*									 ADD COMMANDS TO CLI TABLE
* FUNCTION		: Add_CLI_Cmd
* DESCRIPTION	: This function can be called from any file to add a CLI command
*				  Currently MAX_CLI_CMDS is set to 300.
* ARGUMENTS		: CLI command name, function name
* RETURNS		: RET_OK on Success, RET_ERROR on error
*******************************************************************************
*/

//int Add_CLI_Cmd(char * pCmdName, int (*fn) (int argc, char **argv))
int Add_CLI_Cmd(char * pCmdName, CLI_FN *fn)
{

	int cmdAdded = 0x0;
	int i= 0;

	//Mur-07.Mar.07 - sanity check for overflow
	if ( strlen(pCmdName) >= MAX_LENGTH_CLI_CMD )
	{
		ffprintf ( "> CLI Command [%s] Exceeds length [%d]!!!!!!!\n\n", pCmdName, strlen(pCmdName) );
		return RET_ERROR;
	}

	for (i = 0; i < (MAX_CLI_CMDS-1); i++)
	{
		if (((cliCommandTable[i].func) == fn)&& (!(strcmp(cliCommandTable[i].CmdStr, pCmdName))))
		{
			printf ("Command <<<  %s  >>> Already exists\n", pCmdName);
			cmdAdded = 0x1;
			break;
		}

		if (*(cliCommandTable[i].CmdStr) == 0x0)
		{
			strcpy (cliCommandTable[i].CmdStr, pCmdName);
			UpperStr(cliCommandTable[i].CmdStr);
			cliCommandTable[i].func = fn;
			cmdAdded = 0x1;
			break;
		}
	}
	if (cmdAdded)
	{
		return RET_OK;
	}
	else
	{
		printf ("Command could NOT be added\n");
		return RET_ERROR;
	}
}

//int Add_GLI_Cmd (char * pCmdName, unsigned int cmdId, int (*fn) (int argc, char **argv))
int Add_GLI_Cmd (char * pCmdName, unsigned int cmdId, CLI_FN *fn)
{

	int cmdAdded = 0x0;
	int i= 0;

	//Mur-07.Mar.07 - sanity check for overflow
	if ( strlen(pCmdName) >= MAX_LENGTH_CLI_CMD )
	{
		ffprintf("> CLI Command [%s] Exceeds length [%d]!!!!!!!\n\n", pCmdName, strlen(pCmdName) );
		return RET_ERROR;
	}

	for (i = 0; i < (MAX_CLI_CMDS-1); i++)
	{
//		  if (((cliCommandTable[i].func) == fn) || ((cliCommandTable[i].cmdId) == cmdId) || (!(strcmp(cliCommandTable[i].CmdStr, pCmdName))))
		if ( ((cliCommandTable[i].cmdId) == cmdId) || (!(strcasecmp(cliCommandTable[i].CmdStr, pCmdName))))
		{
			printf ("Command <<<  %s  >>> Already exists\n", pCmdName);
			cmdAdded = 0x1;
			break;
		}

		if (*(cliCommandTable[i].CmdStr)) continue;
		if (  cliCommandTable[i].cmdId	) continue;

		cliCommandTable[i].cmdId = cmdId;
		strcpy (cliCommandTable[i].CmdStr, pCmdName);
		cliCommandTable[i].func = fn;
		cmdAdded = 0x1;
		break;
	}
	if (cmdAdded)
	{
		return RET_OK;
	}
	else
	{
		printf ("Command could NOT be added\n");
		return RET_ERROR;
	}
}


int cli_cmd_list(int argc, char **argv)
{
	int i;

	printf("CLI Command List", '=' );

	for(i=0; i<MAX_CLI_CMDS; i++)
	{
		if (*(cliCommandTable[i].CmdStr) == 0x0) break;
		printf("[%3d] %s\n", cliCommandTable[i].cmdId, cliCommandTable[i].CmdStr);
	}
	return 0;
}

/*
*******************************************************************************
*						INITIALISE CLI COMMAND TABLE
* FUNCTION		: Init_CLI_Commands
* DESCRIPTION	: This function intialises the CLI command table and adds the
*				  basic commands to the CLI table at startup.
* ARGUMENTS		: CLI command name, function name
* RETURNS		: RET_OK on Success, RET_ERROR on error
*******************************************************************************
*/

void Init_CLI_Commands (void)
{
	int i;

	spFirstScript = InitList();
	spFirstScript->pData = NULL;

	for (i=0;i<MAX_CLI_CMDS;i++)
	{
		memset((char*) &cliCommandTable[i], 0x0, sizeof (struct lookup));
	}

	for (i=0;i<MAX_CLI_CMDS;i++)
	{
		cliCommandTable[i].CmdStr = (char *)malloc(MAX_LENGTH_CLI_CMD);
		memset(cliCommandTable[i].CmdStr, 0x0, MAX_LENGTH_CLI_CMD);
	}

	Add_CLI_Cmd( "HELP"		,help);
	Add_CLI_Cmd( "?"		,help);
	Add_CLI_Cmd( "LS"		,help);
	Add_CLI_Cmd( "MAN"		,help);
	Add_CLI_Cmd( "DELAY"	,CLI_OSDelay);

	Add_CLI_Cmd( "CMDLIST"	,cli_cmd_list);
	Add_CLI_Cmd( "MD"		,memdump);
	Add_CLI_Cmd( "FMW"		,Write_MemoryFromFile);
	Add_CLI_Cmd( "MD8"		,memdumpbyte);
	Add_CLI_Cmd( "MD16"		,memdumpword);
	Add_CLI_Cmd( "MW8"		,mem_wrt_char);
	Add_CLI_Cmd( "MW16"		,mem_wrt_word);
	Add_CLI_Cmd( "MW32"		,mem_wrt_long);
	Add_CLI_Cmd( "MWC32"	,MemoryWriteContinous32);
	Add_CLI_Cmd( "MR"		,Read_Memory);
	Add_CLI_Cmd( "MW"		,Write_Memory);
	Add_CLI_Cmd( "MFILL"	,memory_fill);
	Add_CLI_Cmd( "RD"		,Read_Register);
	Add_CLI_Cmd( "WR"		,Write_Register);

	//Add_CLI_Cmd( "PMD"		,memdump_phys);

//	  Add_CLI_Cmd( "RESET"	,(CLI_FN)Soft_Reset);
	Add_CLI_Cmd( "D.S"		,Script_DataSet);
	Add_CLI_Cmd( "REGTEST"	,TT_REGTest);
//	 Add_CLI_Cmd( "OS_STAT" ,TT_OSStat);
	Add_CLI_Cmd("RUN"		,CLI_runScriptFile);

}
//int TT_OSStat(int argc, char **argv)
//{
//	  unsigned int ui32Arg = 0;
//	  if ( argc < 2)
//	  {
//		  printf("\r\nOS_STAT <0 or 1>");
//		  printf("\r\n0 - Disable OS Stat log");
//		  printf("\r\n1 - Enable OS Stat log");
//		  return 0;
//	  }
//	  else
//	  {
//		  ui32Arg = strtoul (argv[1], NULL, 0 );
//		  if(ui32Arg == 0 || ui32Arg == 1)
//			  fOSProfile = ui32Arg;
//		  else
//			  printf("\r\n Invalid value entered, valid values are 0 and 1");
//	  }

//	  return 0;
//}

/*
*******************************************************************************
*					   START CLI PARSER
* FUNCTION			  : cli_start
* DESCRIPTION	   : The cli parser task is created and spawned, the function cli_parser is
*									 the function that parses the commands from the CLI
* ARGUMENTS			: None
* RETURNS				: None
*******************************************************************************
*/
void cli_start (void)
{
//	  UINT8  err;
	/* Register basic CLI commands for memory access, etc */
	Init_CLI_Commands();

	/* create the CLI task */
	if( pthread_create(&stCLI_thread, NULL, (void *)cli_parser, NULL) )
	{
		ffprintf("ERROR(%s)- pthread_create failed \n", __FUNCTION__);
	}
}

void cli_join (void)
{
	void **status = NULL;

	pthread_join(stCLI_thread, (void **)status);
	ffprintf("Thread End %d\n", (unsigned int)status);
}

/*
*******************************************************************************
*														   LOCAL FUNCTIONS
*******************************************************************************
*/

/*
*******************************************************************************
*									 CLI BANNER
* FUNCTION			  : cli_banner
* DESCRIPTION	   : Prints the CLI banner before initialising the CLI interface
* ARGUMENTS			: None
* RETURNS				: None
*******************************************************************************
*/
static void cli_banner (void)
{
	printf ("\n ------------------------------------\n");
	printf ("	Command Line Interface (CLI)\n");
	printf ("	Version : 1.0\n");
	printf ("	Created : 18, Sep 2005\n");
	printf ("	Latest Modified  : %s %s\n",__DATE__, __TIME__);
	printf ("	Type ? for list of commands\n");
	printf (" ------------------------------------\n\n");
}

int cli_run(int argc, char **argv)
{
	int   cmdlp;
	unsigned int cmdchk = 0;
	BOOLEAN isNum;
	BOOLEAN isMatched;

	if (argc <= 0) {
		ffprintf("wrong Operation : no argument");
		return RET_ERROR;
	}

	if (!strcasecmp(argv[0], "quit")) return RET_ERROR;

	cmdlp  = 0;
	cmdchk = 0;
	isNum = isXDigit(argv[0]);
	/* Lookup the CLI command table for a match of command */
	while (cliCommandTable[cmdlp].CmdStr)
	{
		if ( !(*cliCommandTable[cmdlp].CmdStr) ) break;
		if (!isNum)
			isMatched = strcasecmp(argv[0], cliCommandTable[cmdlp].CmdStr)?FALSE:TRUE;
		else
			isMatched = ((cliCommandTable[cmdlp].cmdId > 0) && (strtoul(argv[0] ,NULL ,0) == cliCommandTable[cmdlp].cmdId ))?TRUE:FALSE;

		if (isMatched)
		{
			/* If there is a command match call the corresponding function */
			cmdchk++;
			cliCommandTable[cmdlp].func( argc, argv );
			//printf( "\n");
			break;
		}
		cmdlp++;
		if(cmdlp>=MAX_CLI_CMDS)
			break;
	}


	if (cmdchk == 0 || cmdlp>=MAX_CLI_CMDS)
		if (CL_XDP_GetStatus()) ffprintf("%s : wrong command..\n", argv[0]);

	return RET_OK;
}

/*
*******************************************************************************
*									 CLI PARSER
* FUNCTION			  : cli_parser
* DESCRIPTION	   : This function is the main function that accepts commands from the
*									 terminal and processes them. Initially the command 'help' can be issued
*									 to display a list of commands available.
* ARGUMENTS			: dummy pointer, this pointer is not used with in cli_parser
* RETURNS				: None
*******************************************************************************
*/
static void cli_parser (void *dummy)
{
	char ReadBuffer[MAX_CLI_CMD_LENGTH]; /* This buffer is used to store the user command */
	int   argc;
	char *argv[128] = {0,0};
	int ret;

	cli_banner ();

	while (1)
	{
		if (CL_XDP_GetStatus()) printf("DTV SoC > ");

		/* cleanup command buffer */
		memset( ReadBuffer, 0 , sizeof(ReadBuffer));

		/* Store the characters typed on the terminal into ReadBuffer until carraige return is entered */
		gets_his( ReadBuffer );
		convGUI2CLI( ReadBuffer );
		if (CL_XDP_GetStatus()) printf("\n");
		/* Parse the ReadBuffer and store the tokens into argv array, argc holds the argument count */
		argc = parse_args( ReadBuffer, argv );
		if (argc <= 0) continue;
		ret = cli_run(argc, argv);
		if (ret) break;
	}
} /* cli */


/*
*******************************************************************************
*									  CLI PARSER
* FUNCTION		: cli_parser
* DESCRIPTION	: This function is the main function that accepts commands from the
*				  terminal and processes them. Initially the command 'help' can be issued
*				  to display a list of commands available.
* ARGUMENTS		: dummy pointer, this pointer is not used with in cli_parser
* RETURNS		: None
*******************************************************************************
*/


void CLI_ExecuteCommand ( UINT8 *CommandBuffer )
{
	int argc;
	char *argv[128] = {0,0};
	int   cmdlp;
	unsigned int cmdchk = 0;

	argc = parse_args( (char *) CommandBuffer, argv );

	if (argc)
	{
		UpperStr( argv[0] );

		cmdlp  = 0;
		cmdchk = 0;
		/* Lookup the CLI command table for a match of command */
		while (cliCommandTable[cmdlp].CmdStr)
		{
			if ( strcmp(argv[0], cliCommandTable[cmdlp].CmdStr ) == 0)
			{
				/* If there is a command match call the corresponding function */
				cmdchk++;
				cliCommandTable[cmdlp].func( argc, argv );
//				printf( "\n");
				break;
			}
			cmdlp++;
		}

		if (cmdchk == 0)
		{
			printf("wrong command..\n");
		}
	}

}


/*
*******************************************************************************
*									CLI COMMAND FOR OS DELAY
* FUNCTION		: CLI_OSDelay
* DESCRIPTION	: This function is the used when the command DELAY <ticks> is
*				  written in a script file. This function is associated
*				  with the command DELAY mainly to be used in script files
*				  SCRIPT FILE USAGE: DELAY <ticks>
* ARGUMENTS		: Delay in ticks
* RETURNS		: RET_OK on Success, RET_ERROR on error
*******************************************************************************
*/
int CLI_OSDelay (int argc, char **argv)
{
	unsigned int delay;
	delay = (unsigned int)strtoul( argv[1], NULL, 0);
	usleep (delay);
	return RET_OK;
}


/*
*******************************************************************************
*									TRACE32 D.S COMMAND EQUIVALENT
* FUNCTION		: Script_DataSet
* DESCRIPTION	: This function is the used when the command D.S is
*				  written in a script file. This function is associated
*				  with the command D.S that is used in cmm files in TRACE32.
*				  It helps in running data set commands in cmm files on the target directly
* ARGUMENTS		: D.S SD:<Address> %BE %LONG <Value>
* RETURNS		: RET_OK on Success, RET_ERROR on error
*******************************************************************************
*/
int Script_DataSet (int argc, char ** argv)
{
	unsigned char u8Addr [15];
	unsigned char * pu8Addr;
	unsigned int u32Addr;
	unsigned int u32Value;
	if (argc < 5 || argc > 5)
	{
		printf ("Error: Incorrect Number of arguments\n");
		return RET_ERROR;
	}
	else
	{
		strcpy ((char *)u8Addr, argv[1]);
		pu8Addr  = &u8Addr[3];
		u32Addr  =	(unsigned int) strtoul( (char *) pu8Addr, NULL, 0);
		u32Value =	(unsigned int) strtoul( argv[4], NULL, 0);
		RegWrite32 (u32Addr,u32Value);
		return RET_OK;
	}
}

int CLI_runScriptFile(int argc, char **argv)
{
	if ((argc < 2) || ((argc > 1) && !strcmp(argv[1], "-help")) ) {
		ffprintf("run script\n");
		return RET_ERROR;
	}

	return runScriptFile(argv[1]);
}


int runScriptFile(char *runFileName)
{
	FILE *pstFile;
	char ReadBuffer[MAX_CLI_CMD_LENGTH]; /* This buffer is used to store the user command */
	char *tok;
	char *argv[128] = {0,0};
	int  argc;

	if ( NULL == runFileName )
	{
		printf("ERROR- Invalid File Name\n");
		return RET_ERROR;
	}

	pstFile = fopen(runFileName, "rt");
	if( pstFile == NULL )
	{
		printf("ERROR- Can not open file:%s\n", runFileName);
		return RET_ERROR;
	}

	if (AddScriptToList(spFirstScript, runFileName)) {
		fclose(pstFile);
		return RET_ERROR;
	}

	printf("Script %s is running\n", runFileName);
	while(fgets(ReadBuffer, sizeof(ReadBuffer), pstFile))
	{
		if ((tok = strstr(ReadBuffer, "#" )) != NULL) *tok = 0;
		if ((tok = strstr(ReadBuffer, "--")) != NULL) *tok = 0;
		printf("%s", ReadBuffer);
		argc = parse_args(ReadBuffer, argv);
		if (argc <= 0) continue;
		cli_run(argc, argv);
	}

	DeleteScriptFromList(spFirstScript, runFileName);

	fclose(pstFile);
	return RET_OK;
}

#if 00 //> Modified by kimjm in 1/13/2010 8:33:17 PM <<<{b}>>>
//> unsigned int GetBaseX(char *pString)
//> {
//> 	unsigned int base = 0;
//>
//> 	if ( !isXDigit(pString) ) return base;
//> 	if ( base != 16 ) {
//> 		while ( pString ) {
//> 			if (*pString == '0') {
//> 				pString++;
//> 				base = 8;
//> 				if ((toupper(*pString) == 'X') && isxdigit(pString[1])) {
//> 					base = 16;
//> 					break;
//> 				}
//> 			}
//>
//> 			for ( ;*pString != '\0';++pString ) {
//> 				if (isdigit(*pString)) continue;
//> 				if (!isxdigit(*pString)) break;
//> 				base = 16;
//> 				break;
//> 			}
//> 			break;
//> 		}
//>
//> 		if (base == 8) base = 16;
//> 	}
//>
//> 	return base;
//> }
#endif //> Modified by kimjm in 1/13/2010 8:33:17 PM <<<{e}>>>

unsigned int GetDigitX(char *pString)
{
	unsigned int base = GetBaseX(pString);

	return strtoul(pString, NULL, base);
}

int convGUI2CLI(char *s)
{
	char *tok, *bgn, *end, *sep;

	do {
		if ((tok = strstr(s, "$@")) == NULL) break;
		if ((bgn = strstr(s, "<" )) == NULL) break;
		if ((end = strstr(s, ">" )) == NULL) break;
		if ((sep = strstr(s, "," )) == NULL) sep = end+1;
		*tok = 0;
		*bgn = ' ';
		for (tok=end;tok<=sep;tok++) *tok = ' ';
		for (tok=sep;*tok;tok++) if (*tok == ',') *tok = ' ';
	} while(0);

	return RET_OK;

}
