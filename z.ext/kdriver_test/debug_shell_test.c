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
 *  @date		2010-02-02
 *  @note		Additional information.
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "simple_lib.h"
#include <stdlib.h>

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
static void command_version ( int argc, char* argv[] )
{
	PRINTF("kernel : 2.6.28\n");
	PRINTF("DTVSOC : lg1150\n");
}

static void command_diagnostics	( int argc, char* argv[] )
{

}

static void command_rect	( int argc, char* argv[] )
{
	if ( argc == 1 )
	{
		PRINTF("[usage] %s x y width height color\n", argv[0] );
		PRINTF("           x : x offset from canvas origin\n"
			   "           y : y offset from canvas origin\n"
			   "           y : y offset from canvas origin\n"
			   "       width : width\n"
			   "      height : height\n"
			   "      color  : 32bit 0xAARRGGBB color\n");

		return;
	}

}

static void command_quit ( int argc, char* argv[] )
{
	exit(0);
}

static DENTRY_T my_dtab[] =
{
	{"version", "display version information", command_version },
	{"diag",	"execute diagnostics command", command_diagnostics },
	{"rect",	"draw rectangle", 			   command_rect },
	{"quit",    "quit program",				   command_quit },
	{NULL, NULL, NULL },
};

main()
{
	PRINTF("-------------------------------------------------------------------------------\n");
	PRINTF("simple debug shell (raxis.lim@lge.com)\n");
	PRINTF("-------------------------------------------------------------------------------\n");

	for ( ; ; )
	{
		DebugShell ( my_dtab, "> ");
	}
}

