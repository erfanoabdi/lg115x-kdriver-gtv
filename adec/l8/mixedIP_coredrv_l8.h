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
 *  @author		JongSang Oh(jongsang.oh@lge.com)
 *  @version	1.0
 *  @date		2010.05.15
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

#ifndef	_MIXEDIP_COREDRV_L8_H_
#define	_MIXEDIP_COREDRV_L8_H_

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

#include "../sys/ctop_regs.h"	//for CTOP CTRL Reg. map

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
//define Power-up and down sequence delay
#define MIXEDIP_POWER_OFF_DELAY		(1+1)		// 1 us 
#define AAD_DCO_RESETB_DELAY		(5+1)		// 5 us
#define AAD_SPLL_PDB_DELAY			(200+20)	// 200 us
#define AAD_ADC_PDB_DELAY			(1+1)		// 3 cycles(36.864M(26ns * 3)
#define AUAD_PDB_IN_DELAY			(200+20)	// 200 us
#define HP_PDB_DELAY				(40+4)		// 40 us
#define AUDA_PDB_DELAY				(40+4)		// 40 us
#define AUDA_PDB_DSE_DELAY			(40+4)		// 40 us
#define AUMI_PDB_IN_DELAY			(200+20)	// 200 us




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
int MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_SRC_T ui8AdecClockSrc );
int MIXED_IP_ADEC_L8_SetSamplingFreq ( LX_ADEC_SAMPLING_FREQ_T ui32SamplingFreq );

int MIXED_IP_ADEC_L8_InitCodecRegisters ( void );
int MIXED_IP_ADEC_L8_PowerControl ( LX_ADEC_POWER_BLOCK_T block, LX_ADEC_POWER_MODE_T mode );
int MIXED_IP_ADEC_L8_StartCodec ( void );
int MIXED_IP_ADEC_L8_StopCodec ( void );

int MIXED_IP_ADEC_L8_SetADCPortNumber ( UINT8 ui8PortNum );
int MIXED_IP_ADEC_L8_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol );
int MIXED_IP_ADEC_L8_SetDACOutMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute );



#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _MIXEDIP_COREDRV_L8_H_ */

/** @} */
