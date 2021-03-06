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
 * date       2011.06.27
 * note       Additional information.
 *
 * @addtogroup lg1152_de
 * @{
 */
#ifndef  CTOP_CTRL_REG_L9_INC
#define  CTOP_CTRL_REG_L9_INC
/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "l9/de_reg_mac.h"
#include "l9/ctop_ctrl_reg_l9a0.h"
#include "l9/ctop_ctrl_reg_l9b0.h"

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define CTOP_CTRL_L9_RdFL(_r)                  		FN_CHIP_RdFL(DE_CTOP, L9, _r)
#define CTOP_CTRL_L9_WrFL(_r)                  		FN_CHIP_WrFL(DE_CTOP, L9, _r)
#define CTOP_CTRL_L9_Rd(_r)                    		FN_CHIP_Rd32(DE_CTOP, L9, _r)
#define CTOP_CTRL_L9_Wr(_r, _v)                		FN_CHIP_Wr32(DE_CTOP, L9, _r, _v)
#define CTOP_CTRL_L9_Rd01(_r, _f01, _v01)      		FN_CHIP_Rd01(DE_CTOP, L9, _r, _f01, _v01)
#define CTOP_CTRL_L9_Wr01(_r, _f01, _v01)      		FN_CHIP_Wr01(DE_CTOP, L9, _r, _f01, _v01)
#define CTOP_CTRL_L9_WfCM(_r, _f, _c, _m)      		FN_CHIP_WfCM(DE_CTOP, L9, _r, _f, _c, _m)
#define CTOP_CTRL_L9_WfCV(_r, _f, _c, _v1, _v2)     FN_CHIP_WfCV(DE_CTOP, L9, _r, _f, _c, _v1, _v2)
#define CTOP_CTRL_L9_FLRd(_r, _v)              		FN_CHIP_FLRd(DE_CTOP, L9, _r, _v)
#define CTOP_CTRL_L9_FLWr(_r, _v)              		FN_CHIP_FLWr(DE_CTOP, L9, _r, _v)
#define CTOP_CTRL_L9_FLRf(_r, _f01, _v01)      		FN_CHIP_FLRf(DE_CTOP, L9, _r, _f01, _v01)
#define CTOP_CTRL_L9_FLWf(_r, _f01, _v01)      		FN_CHIP_FLWf(DE_CTOP, L9, _r, _f01, _v01)
#define CTOP_CTRL_L9_FLCM(_r, _f, _c, _m)      		FN_CHIP_FLCM(DE_CTOP, L9, _r, _f, _c, _m)
#define CTOP_CTRL_L9_FLCV(_r, _f, _c, _v1, _v2)    	FN_CHIP_FLCV(DE_CTOP, L9, _r, _f, _c, _v1, _v2)

#define CTOP_CTRL_L9A_RdFL(_r)                      REG_PHYS_VER_RdFL(g##DE_CTOP##_##L9, a0, _r)
#define CTOP_CTRL_L9A_WrFL(_r)                      REG_PHYS_VER_WrFL(g##DE_CTOP##_##L9, a0, _r)
#define CTOP_CTRL_L9A_Rd(_r)                        REG_SHDW_VER_Rd32(g##DE_CTOP##_##L9, a0, _r)
#define CTOP_CTRL_L9A_Wr(_r, _v)                    REG_SHDW_VER_Wr32(g##DE_CTOP##_##L9, a0, _r, _v)
#define CTOP_CTRL_L9A_Rd01(_r, _f01, _v01)          REG_SHDW_VER_Rd01(g##DE_CTOP##_##L9, a0, _r, _f01, _v01)
#define CTOP_CTRL_L9A_Wr01(_r, _f01, _v01)          REG_SHDW_VER_Wr01(g##DE_CTOP##_##L9, a0, _r, _f01, _v01)
#define CTOP_CTRL_L9A_FLRf(_r, _f01, _v01)          REG_PHYS_VER_FLRf(g##DE_CTOP##_##L9, a0, _r, _f01, _v01)
#define CTOP_CTRL_L9A_FLWf(_r, _f01, _v01)          REG_PHYS_VER_FLWf(g##DE_CTOP##_##L9, a0, _r, _f01, _v01)
#define CTOP_CTRL_L9A_WfCM(_r, _f, _c, _m)          REG_SHDW_VER_WfCM(g##DE_CTOP##_##L9, a0, _r, _f, _c, _m)
#define CTOP_CTRL_L9A_FLCM(_r, _f, _c, _m)          REG_PHYS_VER_FLCM(g##DE_CTOP##_##L9, a0, _r, _f, _c, _m)
#define CTOP_CTRL_L9A_FLRd(_r, _v)              	REG_PHYS_VER_FLRd(g##DE_CTOP##_##L9, a0, _r, _v)
#define CTOP_CTRL_L9A_FLWr(_r, _v)              	REG_PHYS_VER_FLWr(g##DE_CTOP##_##L9, a0, _r, _v)
#define CTOP_CTRL_L9A_FLCV(_r, _f, _c, _v1, _v2)    REG_PHYS_VER_FLCV(g##DE_CTOP##_##L9, a0, _r, _f, _c, _v1, _v2)

#define CTOP_CTRL_L9B_RdFL(_r)                      REG_PHYS_VER_RdFL(g##DE_CTOP##_##L9, b0, _r)
#define CTOP_CTRL_L9B_WrFL(_r)                      REG_PHYS_VER_WrFL(g##DE_CTOP##_##L9, b0, _r)
#define CTOP_CTRL_L9B_Rd(_r)                        REG_SHDW_VER_Rd32(g##DE_CTOP##_##L9, b0, _r)
#define CTOP_CTRL_L9B_Wr(_r, _v)                    REG_SHDW_VER_Wr32(g##DE_CTOP##_##L9, b0, _r, _v)
#define CTOP_CTRL_L9B_Rd01(_r, _f01, _v01)          REG_SHDW_VER_Rd01(g##DE_CTOP##_##L9, b0, _r, _f01, _v01)
#define CTOP_CTRL_L9B_Wr01(_r, _f01, _v01)          REG_SHDW_VER_Wr01(g##DE_CTOP##_##L9, b0, _r, _f01, _v01)
#define CTOP_CTRL_L9B_FLRf(_r, _f01, _v01)          REG_PHYS_VER_FLRf(g##DE_CTOP##_##L9, b0, _r, _f01, _v01)
#define CTOP_CTRL_L9B_FLWf(_r, _f01, _v01)          REG_PHYS_VER_FLWf(g##DE_CTOP##_##L9, b0, _r, _f01, _v01)
#define CTOP_CTRL_L9B_WfCM(_r, _f, _c, _m)          REG_SHDW_VER_WfCM(g##DE_CTOP##_##L9, b0, _r, _f, _c, _m)
#define CTOP_CTRL_L9B_FLCM(_r, _f, _c, _m)          REG_PHYS_VER_FLCM(g##DE_CTOP##_##L9, b0, _r, _f, _c, _m)
#define CTOP_CTRL_L9B_FLRd(_r, _v)              	REG_PHYS_VER_FLRd(g##DE_CTOP##_##L9, b0, _r, _v)
#define CTOP_CTRL_L9B_FLWr(_r, _v)              	REG_PHYS_VER_FLWr(g##DE_CTOP##_##L9, b0, _r, _v)
#define CTOP_CTRL_L9B_FLCV(_r, _f, _c, _v1, _v2)    REG_PHYS_VER_FLCV(g##DE_CTOP##_##L9, b0, _r, _f, _c, _v1, _v2)

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/
typedef struct {
    union {
        UINT32          *addr;
        CTOP_CTRL_REG_L9A0_T *a0;
		CTOP_CTRL_REG_L9B0_T *b0;
    } shdw;

    union {
        volatile UINT32          *addr;
        volatile CTOP_CTRL_REG_L9A0_T *a0;
		volatile CTOP_CTRL_REG_L9B0_T *b0;
    } phys;
} CTOP_CTRL_REG_L9_T;


/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

#endif   /* ----- #ifndef CTOP_CTRL_REG_L9_INC  ----- */
/**  @} */
