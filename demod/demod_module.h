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

/*! \file ADC_module.h
 * \brief VPORT ADC module header
 */

#ifndef L8_ADC_MODULE
#define L8_ADC_MODULE


 /*
 	define
 */


/*
	Device inform
*/


/*
	structure define
*/


/*
	Enum define
*/


/*
	global variable
*/


/*
	function prototype
*/
int DEMOD_Initialize(void);
int DEMOD_ResetHW(void);
int DEMOD_SetI2C(void);
int DEMOD_SetInclk(LX_DEMOD_INCLK_SEL_T *inclk);
int DEMOD_SetBbmod(LX_DEMOD_BBMODE_T *bbmode);
int DEMOD_SetTunerI2C(LX_DEMOD_TUNER_I2C_SEL_T *tunerI2C);
#ifdef KDRV_CONFIG_PM
int DEMOD_HW_suspend(void);
int DEMOD_HW_resume(void);
#endif
#endif
