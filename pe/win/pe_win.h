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

/** @file pe_win.h
 *
 *  driver header for picture enhance window control functions. ( used only within kdriver )
 *	
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note		
 *	@date		2011.06.11
 *	@see		
 */

#ifndef	_PE_WIN_H_
#define	_PE_WIN_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "pe_kapi.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
 *	pe window settings type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	LX_PE_OUT_TYPE out_type;	///< output type
	LX_PE_3D_IN_TYPE in_type;	///< 3d input type
	UINT32 act_x0;				///< pe active window, x0 position
	UINT32 act_y0;				///< pe active window, y0 position
	UINT32 act_x1;				///< pe active window, x1 position
	UINT32 act_y1;				///< pe active window, y1 position
	UINT16 tb_ratio;			///< detection ratio for tb (1024=100%)
	UINT16 ss_ratio;			///< detection ratio for ss (1024=100%)
}
PE_WIN_SETTINGS_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int PE_WIN_Init(void);
int PE_WIN_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize);
int PE_WIN_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize);
int PE_WIN_GetCurWinSettings(PE_WIN_SETTINGS_T *pstParams);
int PE_WIN_SetD3dDceHistoWin(void);
int PE_WIN_SetLrcrInOutSel(void);
int PE_WIN_SetLrcrWin(LX_PE_OUT_TYPE detect_type, LX_PE_WIN_POSITION_T win_pos);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PE_WIN_H_ */
