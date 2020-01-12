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
 * TTX module
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version     1.0
 *  date
 *  note        Additional information.
 *
 *  @addtogroup lg1150_ttx
 *  @{
 */

#ifndef _TTX_HW_H_
#define _TTX_HW_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/interrupt.h>

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
//#define	TTX_REG_MEMORY_START	0xC001D000 	// packet data read memory
//#define	TTX_REG_MEMORY_SIZE		0x1000
//
//#define	TTX_L9A0_REG_MEMORY_START	0xC0024100
//#define	TTX_L9A0_REG_MEMORY_SIZE	0x800




typedef struct{
	unsigned short wss_data;
	unsigned int wss_int_time;
} sWSSBufInform;
/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
// function pointer
extern int (*TTX_HWInitialize)(void);
extern int (*TTX_SetInputFormat)(eVBIDataType type);
extern int (*TTX_SetInterrupt)(int on_off);
extern int (*TTX_ParsingVBIData)(sHeaderData *data);
extern int (*TTX_PutVBIData)(sHeaderData *data);
extern int (*TTX_SetInterruptClear)(void);
extern int (*TTX_SetVBILPF)(int onoff);
extern int (*TTX_CheckOnLine318)(int mode);
extern int (*TTX_SetTypeLine318)(int onoff);
#ifdef	KDRV_CONFIG_PM
extern int (*TTX_RunSuspend)(void);
extern int (*TTX_RunResume)(void);
#endif

// L8 Ax function prototype
int TTX_HWInitialize_L8Ax(void);
int TTX_SetInputFormat_L8Ax(eVBIDataType type);
int TTX_SetInterrupt_L8Ax(int on_off);
int TTX_ParsingVBIData_L8Ax(sHeaderData *data);
int TTX_PutVBIData_L8Ax(sHeaderData *data);
int TTX_SetInterruptClear_L8Ax(void);
int TTX_SetVBILPF_L8Ax(int onoff);
int TTX_CheckOnLine318_L8Ax(int mode);
int TTX_SetTypeLine318_L8Ax(int onoff);
#ifdef	KDRV_CONFIG_PM
int TTX_RunSuspend_L8Ax(void);
int TTX_RunResume_L8Ax(void);
#endif


// L8 Bx function prototype
int TTX_HWInitialize_L8Bx(void);
int TTX_SetInputFormat_L8Bx(eVBIDataType type);
int TTX_SetInterrupt_L8Bx(int on_off);
int TTX_ParsingVBIData_L8Bx(sHeaderData *data);
int TTX_PutVBIData_L8Bx(sHeaderData *data);
int TTX_SetInterruptClear_L8Bx(void);
int TTX_SetVBILPF_L8Bx(int onoff);
int TTX_CheckOnLine318_L8Bx(int mode);
int TTX_SetTypeLine318_L8Bx(int onoff);
#ifdef	KDRV_CONFIG_PM
int TTX_RunSuspend_L8Bx(void);
int TTX_RunResume_L8Bx(void);
#endif


// L9 Ax function prototype
int TTX_HWInitialize_L9Ax(void);
int TTX_SetInputFormat_L9Ax(eVBIDataType type);
int TTX_SetInterrupt_L9Ax(int on_off);
int TTX_ParsingVBIData_L9Ax(sHeaderData *data);
int TTX_PutVBIData_L9Ax(sHeaderData *data);
int TTX_SetInterruptClear_L9Ax(void);
int TTX_SetVBILPF_L9Ax(int onoff);
int TTX_CheckOnLine318_L9Ax(int mode);
int TTX_SetTypeLine318_L9Ax(int onoff);
#ifdef	KDRV_CONFIG_PM
int TTX_RunSuspend_L9Ax(void);
int TTX_RunResume_L9Ax(void);
#endif


// L9 Bx function prototype
int TTX_HWInitialize_L9Bx(void);
int TTX_SetInputFormat_L9Bx(eVBIDataType type);
int TTX_SetInterrupt_L9Bx(int on_off);
int TTX_ParsingVBIData_L9Bx(sHeaderData *data);
int TTX_PutVBIData_L9Bx(sHeaderData *data);
int TTX_SetInterruptClear_L9Bx(void);
int TTX_Enable_VPS_Slicer_L9Bx(BOOLEAN Enable);
int TTX_SetVBILPF_L9Bx(int onoff);
int TTX_CheckOnLine318_L9Bx(int mode);
int TTX_SetTypeLine318_L9Bx(int onoff);
int TTX_Set_TT_Slicer_Mode_L9Bx(unsigned int slicer_mode);
#ifdef	KDRV_CONFIG_PM
int TTX_RunSuspend_L9Bx(void);
int TTX_RunResume_L9Bx(void);
#endif

// For CC
extern int (*TTX_CC_HWInitialize)(void);
extern int (*TTX_CC_SetInputFormat)(eVBIDataType type);
extern int (*TTX_CC_SetInterrupt)(int on_off);
extern int (*TTX_CC_ParsingVBIData)(unsigned int *packet);
extern int (*TTX_CC_SetInterruptClear)(void);
extern int (*TTX_CC_DebugModeOnOffCCPrint)(int mode);
#ifdef	KDRV_CONFIG_PM
extern int (*TTX_CC_RunSuspend)(void);
extern int (*TTX_CC_RunResume)(void);
#endif

// L8 A0/A1 functions hwl_l8_a1/
int CC_L8Ax_HWInitialize(void);
int CC_L8Ax_SetInputFormat(eVBIDataType type);
int CC_L8Ax_SetInterrupt(int on_off);
int CC_L8Ax_ParsingVBIData(unsigned int *packet);
int CC_L8Ax_SetInterruptClear(void);
int CC_L8Ax_DebugModeOnOffCCPrint(int mode);
#ifdef	KDRV_CONFIG_PM
int CC_L8Ax_RunSuspend(void);
int CC_L8Ax_RunResume(void);
#endif

// L8 L8B0 functions hwl_l8_b0/
int CC_L8B0_HWInitialize(void);
int CC_L8B0_SetInputFormat(eVBIDataType type);
int CC_L8B0_SetInterrupt(int on_off);
int CC_L8B0_ParsingVBIData(unsigned int *packet);
int CC_L8B0_SetInterruptClear(void);
int CC_L8B0_DebugModeOnOffCCPrint(int mode);
#ifdef	KDRV_CONFIG_PM
int CC_L8B0_RunSuspend(void);
int CC_L8B0_RunResume(void);
#endif


// L8 L9A0 functions hwl_l8_b0/
int CC_L9A0_HWInitialize(void);
int CC_L9A0_SetInputFormat(eVBIDataType type);
int CC_L9A0_SetInterrupt(int on_off);
int CC_L9A0_ParsingVBIData(unsigned int *packet);
int CC_L9A0_SetInterruptClear(void);
int CC_L9A0_DebugModeOnOffCCPrint(int mode);
#ifdef	KDRV_CONFIG_PM
int CC_L9A0_RunSuspend(void);
int CC_L9A0_RunResume(void);
#endif

// won.hur : added for L9 L9B0 functions hwl_l9_b0/
// @2011.08.24
int CC_L9B0_HWInitialize(void);
int CC_L9B0_SetInputFormat(eVBIDataType type);
int CC_L9B0_SetInterrupt(int on_off);
int CC_L9B0_ParsingVBIData(unsigned int *packet);
int CC_L9B0_SetInterruptClear(void);
int CC_L9B0_DebugModeOnOffCCPrint(int mode);
#ifdef	KDRV_CONFIG_PM
int CC_L9B0_RunSuspend(void);
int CC_L9B0_RunResume(void);
#endif



/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/


#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _TTX_HW_H_ */

/** @} */

