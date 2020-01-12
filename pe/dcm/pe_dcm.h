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

/** @file pe_dcm.h
 *
 *  driver header for picture enhance dynamic contrast module functions. ( used only within kdriver )
 *	
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note		
 *	@date		2011.06.11
 *	@see		
 */

#ifndef	_PE_DCM_H_
#define	_PE_DCM_H_

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
 *	pe histogram information parameter type
 */
typedef struct
{
	LX_PE_WIN_ID win_id;		///< window id
	UINT32 bin_num;				///< bin number 16 or 32
    UINT32 histogram[64];		///< histogram data of luma
    UINT32 status[5];			///< histogram status
    UINT32 sat_status;			///< saturation status
}
PE_DCM_DCE_HISTO_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int PE_DCM_Init(void);
int PE_DCM_SetFunction(unsigned int *data, unsigned int functype, unsigned int datasize);
int PE_DCM_GetFunction(unsigned int *data, unsigned int functype, unsigned int datasize);
int PE_DCM_GetDceAllHistogram(PE_DCM_DCE_HISTO_T *pstMainParams,PE_DCM_DCE_HISTO_T *pstSubParams,UINT32 *validL,UINT32 *validR);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PE_DCM_H_ */
