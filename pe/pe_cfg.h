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

/** @file pe_cfg.h
 *
 *  configuration header for pe device. ( used only within kdriver )
 *	
 *	@author		
 *	@version	0.1
 *	@note		
 *	@date		2011.06.11
 *	@see		
 */

#ifndef	_PE_CFG_H_
#define	_PE_CFG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/


#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	PE_MODULE			"pe"
#define PE_MAX_DEVICE		1
#define PE_D3D_KANGSIK_YOON_TEST
#define PE_D3D_WON_HUR_DUMP_TO_EXCEL
#define PE_D3D_USE_LINE_DIFF_BY_SW

/* 20120917, sj.youm, use protection scaler 12tap setting on hd input for GTV */
#define PE_PROTECTION_SCL_12TAP_ON_HD_INPUT
/* 20121123, sj.youm, use strong ipc motion gain for strong bnr for GTV without FRC func. */
#define PE_USE_STRONG_MOTION_GAIN
/* 20130108, sj.youm, fix poor decision of low gray levels(vspyc center pos : 0x0 -> 0x10) */
#define PE_FIX_DECISION_LOW_GRAY_LVLS
/* 20130326, sj.youm, fix(1st) ipc flicker on dtv hd new channel, ipc_ctrl_06:0x00700573->0x00402073 */
/* 20130328, sj.youm, fix(2nd) ipc flicker on dtv hd new channel, ipc_ctrl_04[15:8]:0x20,ipc_ctrl_06:0x00402073->0x0020A073 */
#define PE_FIX_IPC_FLICKER_ON_DTV_HD

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_CHIP_VER_GT(_chp,_ver)	lx_chip_rev( ) >= LX_CHIP_REV(_chp,_ver)
#define PE_CHIP_VER_LT(_chp,_ver)	lx_chip_rev( ) < LX_CHIP_REV(_chp,_ver)
#define PE_CHIP_VER					lx_chip_rev( )

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PE_CFG_H_ */
