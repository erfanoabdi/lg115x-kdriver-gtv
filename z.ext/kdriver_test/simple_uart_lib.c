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

/** @file
 *
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author		raxis
 *  @version	1.0
 *  @date		2010-01-21
 *  @note		Additional information.
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "simple_lib.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/*========================================================================================
	Implementation Group
========================================================================================*/

#include <stdio.h>
#include <fcntl.h>
#include <asm/types.h>
#include <linux/fs.h>
#include <termios.h>
#include <stdint.h>

struct termios	EnterDebugTerminalConfiguration	( struct termios* pTerm  )
{
	struct termios	prev_terms, new_terms;

	tcgetattr ( fileno(stdin), &prev_terms );	// save terminal setting

	new_terms = prev_terms;
	new_terms.c_lflag &= ~(ECHO | ICANON | ISIG);
	new_terms.c_cc[VMIN] = 1;
	new_terms.c_cc[VTIME] = 1;

	tcsetattr ( fileno(stdin), TCSAFLUSH, &new_terms );	// setting terminal

	*pTerm = prev_terms;
}

static	void	ExitDebugTerminalConfiguration ( struct termios* pTerm )
{
	tcsetattr ( fileno(stdin), TCSAFLUSH, pTerm );	// restore terminal setting
}

static char* GetString ( UINT8 *str_buf, int length )
{
	int		l;
	unsigned char	ch, *sp;
	struct termios	prev_terms;

	EnterDebugTerminalConfiguration( &prev_terms );

	for ( l = 0, sp = str_buf; l < length - 2;  )
	{
		ch = getchar();

		if ( ch < ' ' )  // Special Key
		{
			switch ( ch )
			{
				case 0x08 :		// Back Space
					if (l>0) {
						putchar(0x08);
						putchar(' ');
						putchar(0x08);
						l--, sp--;
					}
					break;

				case 0x0a :		// Enter
					putchar(0x0a);
					goto	GET_STRING_END;
			}
			continue;
		}
		putchar(ch);
		*sp = ch, l++, sp++;
	}

GET_STRING_END:
	*sp = '\0';
	ExitDebugTerminalConfiguration( &prev_terms );
	return	str_buf;
}

int    GetChar( )
{
	unsigned char	ch = 0xff;
	struct termios	prev_terms;

	EnterDebugTerminalConfiguration ( &prev_terms );
	ch		= getchar();
	ExitDebugTerminalConfiguration( &prev_terms );

	return	ch;
}

/**
 * read hex integer input
 */
UINT32	ReadHexKeyInput	( char* comment )
{
	UINT8   inputStr[32] = "0x";

	while(1)
	{
		PRINTF("%s: 0x", comment);
		GetString ( inputStr, 32 );

		if ( strlen(inputStr) == 0 ) continue;

		return ( strtoul( inputStr, (char **) NULL, 16 ) );
	}
}

/**
 * read decimal integer input
 */
UINT32	 ReadDecimalKeyInput	( char*	comment	)
{
	UINT8   inputStr[32];

	while(1)
	{
		PRINTF("%s: ", comment);
		GetString ( inputStr, 32 );

		if ( strlen(inputStr) == 0 ) continue;

		return ( strtol( inputStr, (char **) NULL, 10 ) );
	}
}

/**
 * pause input with some helpful message..
 */
void 	PauseKeyInput	( char* msg )
{
	PRINTF	("%s", msg );
	GetChar	( );
}

void	PRINTF( const char *format, ...)
{
    va_list             arg;

	va_start (arg, format);
	vprintf( format, arg );
    va_end(arg);
}


static char delim[] = " \t\n";
static char *simple_strtok(char *s, const char *delim, char **save_ptr)
{
    char *token;

    token = 0;                  /* Initialize to no token. */

    if (s == 0) {               /* If not first time called... */
        s = *save_ptr;          /* restart from where we left off. */
    }

    if (s != 0) {               /* If not finished... */
        *save_ptr = 0;

        s += strspn(s, delim);  /* Skip past any leading delimiters. */
        if (*s != '\0') {       /* We have a token. */
            token = s;
            *save_ptr = strpbrk(token, delim); /* Find token's end. */
            if (*save_ptr != 0) {
                /* Terminate the token and make SAVE_PTR point past it.  */
                *(*save_ptr)++ = '\0';
            }
        }
    }

    return token;
}

void	DebugShell ( DENTRY_T* dtab, char* prompt )
{
    DENTRY_T *p;
	char	*token, *save;
	int		len;
	int		argc;
	char*	argv[32];
	char	arglist[128];
	char	command[256];

    PRINTF("%s", prompt );
    memset( command, 0x0, 256 );
	GetString ( command, 256 );
    
	/* 단순히 ENTER만 입력한 경우에는 아무것도 안한다 */
	if ( strlen(command) == 0 )
	{
		return;
	}

	/* command 가 ?, help, ls 이면 도움말을 출력하고 중지한다 */
	if ( !strncmp(command,"?",	 1 ) ||
		 !strncmp(command,"ls",	 2 ) ||
		 !strncmp(command,"help",4 ) )
	{
		DENTRY_T* tab = dtab;
	
		while ( tab && tab->commandStr )
		{
			PRINTF("%10s - %s\n", tab->commandStr, tab->helpStr );
			tab++;
		}
		PRINTF("%10s - %s\n", "?",    "show help text");
		PRINTF("%10s - %s\n", "help", "show help text");

		return;
	}

	/* command 를 parsing 한다. */
    for(p = dtab; p->commandStr != NULL; ++p)
    {
        if(strncmp(p->commandStr, command, strlen(p->commandStr)) == 0)
        {
            break;
        }
    }

    if ( p->commandStr == NULL )
    {
		PRINTF("<!> unknown command. enter help or ? to view menu list\n");
		return;
    }

	/* 문자열을 token 으로 나누어서 argc, argv 형태로 만들어, 실제 callback 을 호출한다 */
	argc = 0; len = 0;
	memset( argv, 0x0, 32 * sizeof(char*) );

	token = simple_strtok( command, delim, &save );

	while( token )
	{
		argv[argc] = arglist + len;
		len += sprintf( arglist + len, "%s%c", token, '\0' );
		
		/* 최대 32개의 인수를 넘지 않게 한다 */
		if ( (argc+1) < 32 ) argc++; else break;

		token = simple_strtok( NULL, delim, &save );
	}

    p->pfunc( argc, argv );
}
