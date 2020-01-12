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

#ifndef _CLI_XDP_h
#define _CLI_XDP_h

#if 00 //> Modified by kimjm in 3/5/2010 10:44:47 AM <<<{b}>>>
//> #define XDP_SET_ID "DE"
//>
//> #if (USE_LINUX_KERNEL > 0)
//> int CLI_XDP_Start(void);
//> #endif
//> int CLI_XDP_DebugOn(int argc, char **argv);
//> int CL_XDP_DebugOn(char *pSetId, char *pOn);
//> int CLI_XDP_ReadRegister(int argc, char **argv);
//> int CL_XDP_ReadRegister(char *pSetId, UINT32 addr);
//> int CLI_XDP_WriteRegister(int argc, char **argv);
//> int CL_XDP_WriteRegister(char *pSetId, UINT32 addr, UINT32 value);
#endif //> Modified by kimjm in 3/5/2010 10:44:47 AM <<<{e}>>>

BOOLEAN CL_XDP_SetStatus(BOOLEAN bValue);
BOOLEAN CL_XDP_GetStatus(void);

#endif
