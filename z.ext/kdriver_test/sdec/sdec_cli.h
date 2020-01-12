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
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     jaemo.kim (jaemo.kim@lge.com)
 * version    1.0
 * date       2010.02.27
 * note       Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/
static int SDEC_DriverIOCTL(SINT32 i_uiIOCTLNumb, UINT32 i_uiArg);
void SDEC_OpenDriverModule (void);

int CLI_SDEC_SDEC_IO_InitialaizeModule(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_GetCurrentSTCPCR(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SelInputPort(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SetCiperEnable(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SetCiperMode(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SetCiperKey(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SetPcrRecovery(int argc ,char **argv);


/// PID filter
int CLI_SDEC_SDEC_IO_PIDFilterAlloc(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_PIDFilterFree(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_PIDFilterSetPID(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_PIDFilterMapSelect(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_PIDFilterEnable(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_PIDFilterGetState(int argc ,char **argv);

/// Section filter
int CLI_SDEC_SDEC_IO_SectionFilterAlloc(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SectionFilterFree(int argc ,char **argv);
//int CLI_SDEC_ SDEC_IO_SectionFilterMode(S_SDEC_PARAM_T *stpSdecParam, UINT32 ui32Arg);
int CLI_SDEC_SDEC_IO_SectionFilterPattern(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SectionFilterBufferReset(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SectionFilterBufferSet(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SectionFilterGetInfo(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SectionFilterSetReadPtr(int argc ,char **argv);
int CLI_SDEC_SDEC_IO_SectionFilterGetState(int argc ,char **argv);

void CLI_SDEC_Start(void);
int CLI_Sdec_File_Open(int argc, char **argv);
int CLI_Sdec_File_Close(int argc, char **argv);
int CLI_SDEC_EnableLog(int argc, char **argv);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

