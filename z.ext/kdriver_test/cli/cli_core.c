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
 *                                      INCLUDES
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cli_core.h"
#include "pc_key.h"
#include "../de/de_def.h"


/*
 *******************************************************************************
 *                                      CONSTANTS
 *******************************************************************************
 */

/*
 *******************************************************************************
 *                                      MACROS
 *******************************************************************************
 */
#ifdef EXCEPT_XDP
//#define	CL_XDP_GetStatus() 1
#endif
/*
 *******************************************************************************
 *                                      LOCAL VARIABLES
 *******************************************************************************
 */

/*
 *******************************************************************************
 *                                      TABLES
 *******************************************************************************
 */

/*
 *******************************************************************************
 *                                      LOCAL FUNCTIONS PROTOTYPES
 *******************************************************************************
 */

int his_append (char *s);
int his_set (char *s, int index );
int UartGetChar ( FILE *fp );
#ifdef USE_DE_SWITCH_UART
int DE_DEV_SetPwmControl(void);
#endif

/*
 *******************************************************************************
 *                                      GLOBAL FUNCTIONS
 *******************************************************************************
 */
//extern int uHALr_getchar(void);
/*
 *******************************************************************************
 *                                    PARSE THE COMMAND BUFFER
 * FUNCTION         : parse_args
 * DESCRIPTION      : This function parses the command buffer and extracts the tokens into argv
 *                    array and calculates the argument count and returns the same.
 * ARGUMENTS        : The command tbd
 * RETURNS          : Argument count
 *******************************************************************************
 */
int parse_args(char *cmdline, char **argv)
{
	const char *delim = " \f\n\r\t\v";
	char *tok;
	int argc = 0;

	argv[argc] = NULL;

	for (tok = (char *) strtok(cmdline, delim); tok; tok = (char *) strtok(NULL, delim))
	{
		argv[argc++] = tok;
	}
	return argc;
}

/*
 *******************************************************************************
 *                       CONVERT COMMAND STRING TO UPPERCASE
 * FUNCTION         : UpperStr
 * DESCRIPTION      : This functions the command inputs into uppercase
 * ARGUMENTS        : The command itself
 * RETURNS          : None
 *******************************************************************************
 */
void UpperStr (char *Str)
{
	while ( *Str )
	{
		*Str = toupper( *Str ); Str++;
	}
}

/*
 *******************************************************************************
 *                       STORE COMMAND STRING TO BUFFER tbd
 * FUNCTION         : gets_his
 * DESCRIPTION      : This function stores the characters recieved from the terminal into the
 *                    command buffer. It handles a few special characters like backspace,
 *                    CTRL+Z, CTRL+X.
 * ARGUMENTS        : tbd
 * RETURNS          : tbd
 *******************************************************************************
 */
int gets_his (char *s)
{
	int cnt = 0;
	char  *fp;
	char  c;
	int key;

	fp = s;

	while ( 1 )
	{
//        c = getchar();
		key = UartGetChar(stdin);
//        printf("0x%08x\n", key);
		if (key >= 0x100) {
			switch (key)
			{
				case PC_KEY_UP:
					key = 0x10;
					break;
				case PC_KEY_DOWN:
					key = 0x0e;
					break;
#ifdef USE_DE_SWITCH_UART
				case PC_KEY_F01:
					DE_DEV_SetPwmControl();
					continue;
#endif
				default :
					continue;
			}
		}
		c = (char)key;

        if ( c == '\r' || c == '\n' )
		{
			*s = 0;
			while ((*fp == ' ') || (*fp == '\t') ) fp++;
			if ( strlen( fp ) ) his_append( fp );
			break;
		}

		switch ( c )
		{
            case 0x10  : // ^P
				if ( HisIndex >= 0 )
				{
					his_set( fp, HisIndex );
					if ( HisIndex ) HisIndex--;
					if ( HisIndex >= HisCount ) HisIndex = HisCount -1;
					cnt = strlen( fp );
					s = fp + cnt;
				}
				break;
			case 0x0e  : // ^N
				if ( HisIndex < HisCount )
				{
					his_set( fp, HisIndex );
					HisIndex++;
					cnt = strlen( fp );
					s = fp + cnt;
				}
				break;
			case 0x1b: // ESC
				{
					while( cnt > 0 )
					{
						cnt--; *s-- = ' ';
						if (CL_XDP_GetStatus()) printf("\b \b");
					}
				}
				break;

			case 0x8 :
			case 0x7f : // DEL
					   if ( cnt > 0 )
					   {
						   cnt--; *s-- = ' ';
						   if (CL_XDP_GetStatus()) printf("\b \b");
					   }
					   break;
			default  : cnt++;
					   *s++ = c;
                       if (CL_XDP_GetStatus()) printf("%c",c );
					   break;
		}
	}

    return(cnt);

}

/*
 *******************************************************************************
 *                                      LOCAL FUNCTIONS
 *******************************************************************************
 */

/*
 *******************************************************************************
 *                             APPEND COMMAND TO HISTORY BUFFER
 * FUNCTION         : his_append
 * DESCRIPTION      : tbd
 * ARGUMENTS        : tbd
 * RETURNS          : tbd
 *******************************************************************************
 */
int his_append (char *s)
{
	int loop;

	for ( loop = 0; loop < HIS_MAX; loop++ )
	{
		if ( strcmp( s, HisBuff[loop] ) == 0 )
		{
			HisIndex = 0;
			if ( HisCount ) HisIndex = HisCount-1;
			return HisCount;
		}
	}

	if ( HisCount < HIS_MAX )
	{
		strcpy( HisBuff[HisCount], s );
		HisCount++;
	} else {
		for ( loop = 1; loop < HIS_MAX ; loop++ )
		{
			strcpy( HisBuff[loop-1], HisBuff[loop] );
		}
		strcpy( HisBuff[HIS_MAX-1], s );
		HisIndex = HisCount-1;
	}

	HisIndex = 0;
	if ( HisCount ) HisIndex = HisCount-1;

	return HisCount;
}

/*
 *******************************************************************************
 *
 * FUNCTION         : his_set
 * DESCRIPTION      : tbd
 * ARGUMENTS        : tbd
 * RETURNS          : tbd
 *******************************************************************************
 */
int his_set (char *s, int index )
{
	int loop;
	int len;

	len = strlen( s );

	for ( loop = 0; loop < len; loop++ )
	{
		printf("\b \b");
	}

	strcpy( s, HisBuff[index] );
	printf( "%s", s );

	return index;
}

