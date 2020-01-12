/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/** @file 
 *
 *  Brief description. 
 *  Detailed description starts here. 
 *
 *  @author		raxis.lim
 *  @version	1.0 
 *  @date		2011-04-03
 *  @note		Additional information. 
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "gfx_impl.h"

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
int GFX_InitHW  (void)
{
    return g_gfx_hal.InitHW( );
}

int GFX_RunFlushCommand(void)
{
	return g_gfx_hal.RunFlushCommand( );
}

int GFX_RunStartCommand(void)
{
	return g_gfx_hal.RunStartCommand( );
}

int GFX_PauseCommand(void)
{
	return g_gfx_hal.RunPauseCommand( );
}

int GFX_ResumeCommand(void)
{
	return g_gfx_hal.RunResumeCommand( );
}

int GFX_StopCommand(void)
{
	return g_gfx_hal.RunStopCommand( );
}

int GFX_SwResetCommand(void)
{
	return g_gfx_hal.RunSwResetCommand();
}

void GFX_GetComQueueStatus(GFX_CMD_QUEUE_CTRL_T *queue)
{
	g_gfx_hal.GetComQueueStatus(queue);
}

BOOLEAN GFX_IsGfxIdle(void)
{
	return g_gfx_hal.IsGfxIdle( );
}

void GFX_GetOperationStatus(GFX_CMD_OP_T *operate)
{
	g_gfx_hal.GetOperationStatus(operate);
}

void GFX_SetOperationStatus(GFX_CMD_OP_T *operate)
{
	g_gfx_hal.SetOperationStatus(operate); 
}

void GFX_GetInputConfigure(int iPort, GFX_PORT_CONFIGURE_T *port)
{
	g_gfx_hal.GetInputConfigure( iPort, port );
}

void GFX_SetInputConfigure(GFX_PORT_CONFIGURE_T *port)
{
	g_gfx_hal.SetInputConfigure(port);
}

void GFX_SetBlendingOut(GFX_ENUM_OUT_T type)
{
	g_gfx_hal.SetBlendingOut(type);
}

void GFX_GetBlendConfigure( GFX_BLEND_CONFIGURE_T *blend)
{
	g_gfx_hal.GetBlendConfigure(blend);
}

void GFX_SetBlendConfigure( GFX_BLEND_CONFIGURE_T *blend)
{
	g_gfx_hal.SetBlendConfigure(blend);
}

void GFX_GetOutputConfigure( GFX_OUT_CONFIGURE_T *port)
{
	g_gfx_hal.GetOutputConfigure(port);
}

void GFX_SetOutputConfigure(GFX_OUT_CONFIGURE_T *port)
{
	g_gfx_hal.SetOutputConfigure(port);
}

void GFX_SetScalerConfigure(GFX_SCALER_CONFIGURE_T* scaler)
{
	if ( g_gfx_hal.SetScalerConfigure ) g_gfx_hal.SetScalerConfigure(scaler);
}

int GFX_SetCLUT(int port, int size , UINT32 *data)
{
	return g_gfx_hal.SetCLUT(port,size,data);
}

void GFX_SetCommandDelayReg(UINT16 delay)
{
	g_gfx_hal.SetCommandDelayReg(delay);
}

UINT16 GFX_GetCommandDelayReg(void)
{
	return g_gfx_hal.GetCommandDelayReg( );
}

void GFX_SetRegister(UINT32 addr, UINT32 val)
{
	g_gfx_hal.SetRegister(addr, val);
}

UINT32 GFX_GetRegister(UINT32 addr)
{
	return g_gfx_hal.GetRegister(addr);
}

void GFX_DumpRegister(void)
{
	return g_gfx_hal.DumpRegister( );
}

int GFX_RunSuspend(void)
{
	return g_gfx_hal.RunSuspend( );
}

int GFX_RunResume(void)
{
	return g_gfx_hal.RunResume( );
}

/** @} */

