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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "base_types.h"
#include "dbg/dbg_def.h"
#include "cli/cli_arg.h"
#include "cli/cli_main.h"

#ifdef INCLUDE_KDRV_DE
#include "de_kapi.h"
#endif
#ifdef INCLUDE_KDRV_DIMM
#include "dimm_kapi.h"
#endif
#ifdef INCLUDE_KDRV_FMT3D
#include "fmt3d_kapi.h"
#endif
#ifdef INCLUDE_KDRV_DENC
#include "denc_kapi.h"
#endif

ENUM2STR_T geCm[] = {
	e2str(FALSE) ,
	e2str(TRUE)  ,
};

ENUM2STR_T geDE[] = {
#ifdef INCLUDE_KDRV_DE
	e2str(LX_DE_AR_4_3) ,
	e2str(LX_DE_AR_16_9) ,
	e2str(LX_DE_COLOR_601) ,
	e2str(LX_DE_COLOR_709) ,
	e2str(LX_DE_COLOR_RGB) ,
	e2str(LX_DE_COLOR_RGBEX) ,
	e2str(LX_DE_COLOR_XVYCC601) ,
	e2str(LX_DE_COLOR_XVYCC709) ,
	e2str(LX_DE_COLOR_NOT_DEFINED) ,
	e2str(LX_DE_CVE_WITH_OSD) ,
	e2str(LX_DE_CVE_WITHOUT_OSD) ,
	e2str(LX_DE_CVE_NTSC) ,
	e2str(LX_DE_CVE_PAL) ,
	e2str(LX_DE_CVE_SECAM) ,
	e2str(LX_DE_CVI_CH_A) ,
	e2str(LX_DE_CVI_CH_B) ,
	e2str(LX_CVI_A) ,
	e2str(LX_CVI_B) ,
	e2str(LX_MVI_VDEC) ,
	e2str(LX_MVI_CPU) ,
	e2str(LX_IRE_PATTERN_DISABLE) ,
	e2str(LX_IRE_PATTERN_WHITE) ,
	e2str(LX_IRE_PATTERN_RED) ,
	e2str(LX_IRE_PATTERN_GREEN) ,
	e2str(LX_IRE_PATTERN_BLUE) ,
	e2str(LX_MULTI_WIN_OFF) ,
	e2str(LX_MULTI_WIN_H_SPLIT) ,
	e2str(LX_MULTI_WIN_MOVE) ,
	e2str(LX_MULTI_WIN_ZOOM) ,
	e2str(LX_MULTI_WIN_NUMS) ,
	e2str(LX_DE_OIF_8BIT) ,
	e2str(LX_DE_OIF_10BIT) ,
	e2str(LX_OIF_LED_IN_DE) ,
	e2str(LX_OIF_LED_IN_FRC) ,
	e2str(LX_OIF_LVDS_NOT_ODC) ,
	e2str(LX_OIF_LVDS_ODC) ,
	e2str(LX_OIF_1CH) ,
	e2str(LX_OIF_2CH) ,
	e2str(LX_OIF_4CH) ,
	e2str(LX_OIF_CH_RESERVED) ,
	e2str(LX_OIF_OUTPUT_DE) ,
	e2str(LX_OIF_OUTPUT_FRC) ,
	e2str(LX_OIF_OUTPUT_LED) ,
	e2str(LX_DE_OIF_PANEL_LVDS) ,
	e2str(LX_DE_OIF_PANEL_MINI_LVDS) ,
	e2str(LX_OIF_PIXEL_SDQ) ,
	e2str(LX_OIF_PIXEL_SINGLE) ,
	e2str(LX_OIF_PIXEL_DUAL) ,
	e2str(LX_OIF_PIXEL_QUAD) ,
	e2str(LX_OIF_PWM_DE) ,
	e2str(LX_OIF_PWM_FRC) ,
	e2str(LX_DE_OIF_VESA) ,
	e2str(LX_DE_OIF_JEIDA) ,
	e2str(LX_PANEL_TYPE_1920) ,
	e2str(LX_PANEL_TYPE_1366) ,
	e2str(LX_PANEL_TYPE_1024) ,
	e2str(LX_PANEL_TYPE_1365) ,
	//e2str(LX_PRE3D_IN_CHA) ,
	//e2str(LX_PRE3D_IN_CHB) ,
	e2str(LX_PRE3D_IN_NORMAL) ,
	e2str(LX_PRE3D_IN_FRFR) ,
	//e2str(LX_PRE3D_OUT_TB) ,
	//e2str(LX_PRE3D_OUT_SS) ,
	//e2str(LX_PRE3D_OUT_FRFR) ,
	e2str(LX_DE_PWM0) ,
	e2str(LX_DE_PWM1) ,
	e2str(LX_DE_PWM2) ,
	e2str(LX_DE_PWM3) ,
	e2str(LX_DE_PWM4) ,
	e2str(LX_DE_INTERLACED) ,
	e2str(LX_DE_PROGRESSIVE) ,
	e2str(LX_SURFACE_VIDEO) ,
	e2str(LX_SURFACE_OSD1) ,
	e2str(LX_SURFACE_OSD2) ,
	e2str(LX_SURFACE_OSD3) ,
	e2str(LX_SURFACE_CURSOR) ,
	e2str(LX_DE_WIN_MAIN) ,
	e2str(LX_DE_WIN_SUB) ,
#endif
};

ENUM2STR_T geDENC[] = {
#ifdef INCLUDE_KDRV_DENC
	e2str(LX_DENC_SOURCE_CVBS1_WITH_CLAMPING) ,
	e2str(LX_DENC_SOURCE_CVBS1_WITHOUT_CLAMPING) ,
	e2str(LX_DENC_SOURCE_CVBS3_WITH_CLAMPING) ,
	e2str(LX_DENC_SOURCE_CVBS3_WITHOUT_CLAMPING) ,
	e2str(LX_DENC_SOURCE_MONITOR) ,
	e2str(LX_DENC_SOURCE_NUM) ,
	e2str(LX_DENC_NTSC_M) ,
	e2str(LX_DENC_PAL_BG) ,
	e2str(LX_DENC_PAL_M) ,
	e2str(LX_DENC_PAL_N) ,
	e2str(LX_DENC_SECAM) ,
	e2str(LX_DENC_NTSC_J) ,
	e2str(LX_DENC_VIDEO_SYS_NUM) ,
#endif
};

ENUM2STR_T geDIMM[] = {
#ifdef INCLUDE_KDRV_DIMM
	e2str(LX_BL_CCFL) ,
	e2str(LX_BL_NOR_LED) ,
	e2str(LX_BL_EDGE_LED) ,
	e2str(LX_BL_IOP_LED) ,
	e2str(LX_BL_ALEF_LED) ,
	e2str(LX_BL_IOL_LED) ,
	e2str(LX_DIMM_DEMO_PATTERN_NORMAL) ,
	e2str(LX_DIMM_DEMO_PATTERN_SQUARE) ,
	e2str(LX_DIMM_DEMO_PATTERN_GRID) ,
	e2str(LX_DIMM_DEMO_PATTERN_GRADATION) ,
	e2str(LX_DIMM_DEMO_PATTERN_RESERVED) ,
#endif
};

ENUM2STR_T geFMT3D[] = {
#ifdef INCLUDE_KDRV_FMT3D
	e2str(LX_3D_VARIABLE_WIDTH_BLACK_BAR) ,
	e2str(LX_3D_FIXED_WIDTH_BLACK_BAR) ,
	e2str(LX_3D_DIS_FMT_LINEINTER) ,
	e2str(LX_3D_DIS_FMT_FRAMESEQ) ,
	e2str(LX_3D_DIS_FREQ_120HZ) ,
	e2str(LX_3D_DIS_FREQ_240HZ) ,
	e2str(LX_3D_IMG_8BIT) ,
	e2str(LX_3D_IMG_10BIT) ,
	e2str(LX_3D_IMG_YUV420) ,
	e2str(LX_3D_IMG_YUV422) ,
	e2str(LX_3D_IMG_YUV444) ,
	e2str(LX_3D_IMG_ONE_FRAME_PER_VSYNC) ,
	e2str(LX_3D_IMG_TWO_FRAME_PER_VSYNC) ,
	e2str(LX_3D_IMG_TOPNBOTTOM) ,
	e2str(LX_3D_IMG_SIDEBYSIDE) ,
	e2str(LX_3D_IMG_CHECKERBOARD) ,
	e2str(LX_3D_IMG_FRAMESEQ) ,
	e2str(LX_3D_IMG_FIELDSEQ) ,
	e2str(LX_3D_IMG_FREQ_60HZ) ,
	e2str(LX_3D_IMG_FREQ_120HZ) ,
	e2str(LX_3D_IMG_INTERLACE) ,
	e2str(LX_3D_IMG_PROGRASSIVE) ,
	e2str(LX_3D_LR_ORDER_R_FIRST) ,
	e2str(LX_3D_LR_ORDER_L_FIRST) ,
	e2str(LX_3D_OUT_DUAL) ,
	//e2str(LX_3D_OUT_INTERIM) ,
	e2str(LX_3D_OUT_FRAMESEQ) ,
	//e2str(LX_3D_OUT_FRAMESEQ_LRLR) ,
	//e2str(LX_3D_OUT_FRAMESEQ_LLRR) ,
	//e2str(LX_3D_OUT_FRAMESEQ_LBRB) ,
	//e2str(LX_3D_OUT_FRAMESEQ_LURU) ,
	e2str(LX_3D_OUT_LINEINTERLEAVED) ,
	e2str(LX_3D_RUNMODE_OFF) ,
	e2str(LX_3D_RUNMODE_ON) ,
	e2str(LX_3D_RUNMODE_3DTO2D) ,
	e2str(LX_3D_COMB_LORO) ,
	e2str(LX_3D_COMB_LORE) ,
	e2str(LX_3D_COMB_LERO) ,
	e2str(LX_3D_COMB_LERE) ,
	e2str(LX_3D_MODE_QUINCUNX) ,
	e2str(LX_3D_MODE_LINE_BY_LINE) ,
	e2str(LX_3D_EXT_IN) ,
	e2str(LX_3D_MEM_IN) ,
	e2str(LX_3D_BR_FRAME_FLIP) ,
	e2str(LX_3D_TL_FRAME_FLIP) ,
#endif
};

CLI2ARGS_T geCliArgs[] = {
	{geCm	 ,ARRAY_SIZE(geCm)}    ,
	{geDE	 ,ARRAY_SIZE(geDE)}    ,
	{geDENC  ,ARRAY_SIZE(geDENC)}  ,
	{geDIMM  ,ARRAY_SIZE(geDIMM)}  ,
	{geFMT3D ,ARRAY_SIZE(geFMT3D)} ,
};

SINT32 s2i(char *pString)
{
	SINT32 ret = 0;
	BOOLEAN wasMatched = FALSE;
	int i,j;

	if (isXDigit(pString)) {
		//ret = GetDigitX(pString);
		ret = strtol(pString, NULL, 0);
		wasMatched = TRUE;
	} else {
		for(i=0;i<ARRAY_SIZE(geCliArgs);i++) {
			for(j=0;j<geCliArgs[i].size;j++) {
				if (!geCliArgs[i].e2s) continue;
				if (strcmp(geCliArgs[i].e2s[j].str, pString)) continue;
				ret = (SINT32)geCliArgs[i].e2s[j].val;
				wasMatched = TRUE;
				break;
			}
			if (wasMatched) break;
		}
		if (!wasMatched) {
			ffprintf("There's not matched argument %s\n", pString);
		}
	}
	return ret;
}

unsigned int s2u(char *pString)
{
	unsigned int ret = 0;
	BOOLEAN wasMatched = FALSE;
	int i,j;

	if (isXDigit(pString)) {
		//ret = GetDigitX(pString);
		ret = strtoul(pString, NULL, 0);
		wasMatched = TRUE;
	} else {
		for(i=0;i<ARRAY_SIZE(geCliArgs);i++) {
			for(j=0;j<geCliArgs[i].size;j++) {
				if (!geCliArgs[i].e2s) continue;
				if (strcmp(geCliArgs[i].e2s[j].str, pString)) continue;
				ret = (unsigned int)geCliArgs[i].e2s[j].val;
				wasMatched = TRUE;
				break;
			}
			if (wasMatched) break;
		}
		if (!wasMatched) {
			ffprintf("There's not matched argument %s\n", pString);
		}
	}
	return ret;
}

unsigned int s2x(char *pString)
{
	unsigned int ret = 0;
	BOOLEAN wasMatched = FALSE;
	int i,j;

	if (isXDigit(pString)) {
		ret = GetDigitX(pString);
		wasMatched = TRUE;
	} else {
		for(i=0;i<ARRAY_SIZE(geCliArgs);i++) {
			for(j=0;j<geCliArgs[i].size;j++) {
				if (!geCliArgs[i].e2s) continue;
				if (strcmp(geCliArgs[i].e2s[j].str, pString)) continue;
				ret = (unsigned int)geCliArgs[i].e2s[j].val;
				wasMatched = TRUE;
				break;
			}
			if (wasMatched) break;
		}
		if (!wasMatched) {
			ffprintf("There's not matched argument %s\n", pString);
		}
	}
	return ret;
}

unsigned int s2h(char *pString)
{
	unsigned int ret = 0;

#ifdef USE_LINUX_KERNEL
	ret = simple_strtoul(pString, NULL, 16);
#else
	ret = strtoul(pString, NULL, 16);
#endif
	return ret;
}

//#include <Common/ARG_DTV.h>
BOOLEAN isXDigit(char *pString)
{
	BOOLEAN rtn		   = TRUE;
	BOOLEAN thereIsNum = FALSE;

	if ( pString ) {
		if (*pString == '-') pString++;
		if (*pString == '0') {
			pString++;
			if ((toupper(*pString) == 'X') && isxdigit(pString[1])) {
				pString++;
			} else if ((toupper(*pString) == 'U') && isdigit(pString[1])) {
				pString++;
			} else if ((toupper(*pString) == 'Z') && isxdigit(pString[1])) {
				pString++;
			}
			thereIsNum = TRUE;
		} else if ((toupper(*pString) == 'X') && isxdigit(pString[1])) {
			pString++;
			thereIsNum = TRUE;
		} else if ((toupper(*pString) == 'U') && isdigit(pString[1])) {
			pString++;
			thereIsNum = TRUE;
		} else if ((toupper(*pString) == 'Z') && isxdigit(pString[1])) {
			pString++;
			thereIsNum = TRUE;
		} else if (isdigit(*pString)) {
			pString++;
			thereIsNum = TRUE;
		}

		for ( ;*pString != '\0';++pString ) {
			if (isdigit(*pString)) {
				thereIsNum = TRUE;
			} else if (!isxdigit(*pString)) {
				rtn = FALSE;
				break;
			}
		}

//        if (thereIsNum == FALSE) rtn = FALSE;
	}

	return rtn;
}

UINT32 GetBaseX(char *pString)
{
	UINT32 base = 0;

	if ( !isXDigit(pString) ) return base;
	if ( base != 16 ) {
		while ( pString ) {
			if (*pString == '0') {
				pString++;
				base = 8;
				if        ((toupper(*pString) == 'X') && isxdigit(pString[1])) {
					base = 16;
                } else if ((toupper(*pString) == 'U') && isxdigit(pString[1])) {
					*pString = '0';
					base = 10;
                } else if ((toupper(*pString) == 'Z') && isxdigit(pString[1])) {
					*pString = '0';
					base = 2;
				}
				break;
			} else if ((toupper(*pString) == 'X') && isxdigit(pString[1])) {
				*pString = '0';
				base = 16;
			} else if ((toupper(*pString) == 'U') && isxdigit(pString[1])) {
				*pString = '0';
				base = 10;
			} else if ((toupper(*pString) == 'Z') ) {
				*pString = '0';
				base = 2;
			} else {
				base = 16;
			}
			break;
		}

		if (base == 8) base = 16;
	}

	return base;
}
