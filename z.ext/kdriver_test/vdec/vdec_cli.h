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
static int VDEC_DriverIOCTL(SINT32 i_uiIOCTLNumb, UINT32 i_uiArg);
void VDEC_OpenDriverModule (void);
 
int CLI_VDEC_IO_ChInitialize(int argc, char **argv);
int CLI_VDEC_IO_ChClose(int argc, char **argv);
int CLI_VDEC_IO_ChReset(int argc, char **argv);

int CLI_VDEC_IO_SetBufferLVL(int argc, char **argv);
int CLI_VDEC_IO_GetBufferLVL(int argc, char **argv);

//misc
int CLI_VDEC_IO_GetFiemwareVersion(int argc, char **argv);

//decoding control
int CLI_VDEC_IO_ChStart(int argc, char **argv);
int CLI_VDEC_IO_ChStop(int argc, char **argv);
int CLI_VDEC_IO_SetLipsync(int argc, char **argv);
int CLI_VDEC_IO_GetLipsync(int argc, char **argv);

//for buffered/file play only
int CLI_VDEC_IO_UpdateBuffer(int argc, char **argv);
int CLI_VDEC_IO_SetFrame(int argc, char **argv);

//aux data gathering
int CLI_VDEC_IO_GetOutput(int argc, char **argv);

//for decoding status monitoring/notifying
int CLI_VDEC_IO_GetStatus(int argc, char **argv);
int CLI_VDEC_IO_SetNotify(int argc, char **argv);
int CLI_VDEC_IO_GetNotify(int argc, char **argv);

//vdec debugging
int CLI_VDEC_IO_SetRegister(int argc, char **argv);
int CLI_VDEC_IO_GetRegister(int argc, char **argv);
int CLI_VDEC_IO_EnableLog(int argc, char **argv);


void CLI_VDEC_Start(void);
int CLI_Vdec_File_Open(int argc, char **argv);
int CLI_Vdec_File_Close(int argc, char **argv);
int CLI_VDEC_EnableLog(int argc, char **argv);

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

