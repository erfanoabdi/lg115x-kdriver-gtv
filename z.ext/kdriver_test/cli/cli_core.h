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


#if	!defined(CLI_CORE_H)
#define CLI_CORE_H

/*
*******************************************************************************
*                                                          CONSTANTS
*******************************************************************************
*/
#define HIS_MAX	16
/*
*******************************************************************************
*                                                         GLOBAL  MACROS
*******************************************************************************
*/

/*
*******************************************************************************
*                                                          GLOBAL DATA TYPES
*******************************************************************************
*/

/*
*******************************************************************************
*                                                          GLOBAL VARIABLES
*******************************************************************************
*/

char HisBuff[HIS_MAX][1024] = { {0,}, };
int  HisCount               = 0;
int  HisIndex               = 0;

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

#endif	/* End ofCLI_CORE_H */






