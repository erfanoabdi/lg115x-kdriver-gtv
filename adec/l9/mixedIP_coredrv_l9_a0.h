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
 *  driver interface header for Mixed IP device. ( used only within kdriver )
 *	Mixed IP device will teach you how to make device driver with new platform.
 *
 *  @author		Jong-Sang Oh(jongsang.oh@lge.com)
 *  @version	1.0
 *  @date		2010.04.09
 *
 *  @addtogroup lg1152_adec
 *	@{
 */

#ifndef	_MIXEDIP_COREDRV_L9_A0_H_
#define	_MIXEDIP_COREDRV_L9_A0_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "os_util.h" //for alloc functions

#include "debug_util.h"
#include "adec_kapi.h"

#include "ctop_regs.h"	//for ACTOP CTRL Reg. map

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
#include <asm/hardware.h> // For Register base address
#else
#include <mach/hardware.h> // For Register base address
#endif

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define ENABLE_SCART_BYPASS_TEST	//Enable SCART BYPASS TEST ==> This function has sometimes mute for ADC input
                                    // And HDMI port has lip sync problem.

//#define ENABLE_HPDRV_PORT			//Enable Mixed IP HPDRV Output Port


//define Power-up and down sequence delay
#define MIXEDIP_POWER_OFF_DELAY		(1+1)		// 1 us

#define AAD_POWER_OFF_DELAY			(200+20)	// 200 ms
#define AAD_DCO_RESETB_DELAY		(5+1)		// 5 us
#define AAD_SPLL_PDB_DELAY			(200+20)	// 200 us
#define AAD_ADC_PDB_DELAY			(1+1)		// 3 cycles(36.864M(26ns * 3)
#define AAD_SW_RESET_DELAY			(200+20)	// 200 us

#define AUAD_PDB_IN_DELAY			(200+20)	// 200 ms
#define HP_PDB_DELAY				(40+4)		// 40 ms
#define AUDA_PDB_DELAY				(40+4)		// 40 ms
#define AUDA_PDB_DSE_DELAY			(40+4)		// 40 ms
#define AUMI_PDB_IN_DELAY			(200+20)	// 200 ms

//define Power-up and down sequence delay
#define MIXEDIP_RESET_DELAY			(1+1)		// 100 us -> 1ms


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int L9A0_ANALOG_ReadRegister(UINT8 slaveAddr, UINT8 regAddr, UINT8 *value);
int L9A0_ANALOG_WriteRegister(UINT8 slaveAddr, UINT8 regAddr, UINT8 value);


int L9A0_MIXED_IP_ADEC_L9_InitClockRegisters( void );
int L9A0_MIXED_IP_ADEC_L9_SetClockSource( LX_ADEC_CLOCK_SRC_T ui8AdecClockSrc );

int L9A0_MIXED_IP_ADEC_L9_SetPcmClockRate ( LX_ADEC_SAMPLING_FREQ_T ui32SrcFreq, UINT32 ui32PcmClock );
int L9A0_MIXED_IP_ADEC_L9_SetSPDIFSamplingFreq ( LX_ADEC_SAMPLING_FREQ_T ui32SamplingFreq, LX_ADEC_SPDIF_DTO_RATE_T stSpdifDtoRate );

int L9A0_MIXED_IP_ADEC_L9_InitCodecRegisters ( void );
int L9A0_MIXED_IP_ADEC_L9_PowerControl ( LX_ADEC_POWER_BLOCK_T block, LX_ADEC_POWER_MODE_T mode );
int L9A0_MIXED_IP_ADEC_L9_StartCodec ( void );
int L9A0_MIXED_IP_ADEC_L9_StopCodec ( void );

int L9A0_MIXED_IP_ADEC_L9_SetADCPortNumber ( UINT8 ui8PortNum );
int L9A0_MIXED_IP_ADEC_L9_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol );
int L9A0_MIXED_IP_ADEC_L9_SetDACOutMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute );



#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _MIXEDIP_COREDRV_L9_A0_H_ */

/** @} */
