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
 *  note		Additional information.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "os_util.h" //for alloc functions

#include "adec_drv.h"
#include "adec_coredrv_l8.h"
#include "adec_kapi.h"
#include "debug_util.h"
#include "mixedIP_coredrv_l8.h"


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
//Current Mixed IP Power Status
static LX_ADEC_POWER_STATUS_T gADECPowerStatus[LX_ADEC_POWER_MAX] = {	\
							  {LX_ADEC_POWER_AAD, 	LX_ADEC_POWER_UP},
							  {LX_ADEC_POWER_ADC, 	LX_ADEC_POWER_UP},
							  {LX_ADEC_POWER_HPDRV, LX_ADEC_POWER_UP},
							  {LX_ADEC_POWER_DAC, 	LX_ADEC_POWER_UP},
							  {LX_ADEC_POWER_MIC, 	LX_ADEC_POWER_UP} };


/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * Sets the CTOP CTRL clock module register value for setting clock source.
 * @see ADEC_Init
*/
int MIXED_IP_ADEC_L8_SetClockSource( LX_ADEC_CLOCK_SRC_T ui8AdecClockSrc )
{
	//Setting Audio Clock Mux
	ADEC_MIP_DEBUG("CTOP RD R: 0x54(CTR17) : 0x%X\n", CTOP_CTRL_READ(0x54));
	ADEC_MIP_DEBUG("CTOP RD R: 0x58(CTR18) : 0x%X\n", CTOP_CTRL_READ(0x58));

	if(lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		ADEC_MIP_DEBUG("CTOP RD R: 0x88(CTR30) : 0x%X\n", CTOP_CTRL_READ(0x88));
		ADEC_MIP_DEBUG("CTOP RD R: 0x8C(CTR31) : 0x%X\n", CTOP_CTRL_READ(0x8C));

#ifdef ENABLE_DSP_SW_SRC
			/* Set Clock Path CTR17, CTR18 */
			CTOP_CTRL_WRITE(0x54, 0x00040000);
			//CTOP_CTRL_WRITE(0x54, 0x00040044);	//For SCART bypss test
			CTOP_CTRL_WRITE(0x58, 0x00000000);

			/* Set Clock Division CTR30, CTR31 */
			CTOP_CTRL_WRITE(0x88, 0x00000000);
			CTOP_CTRL_WRITE(0x8C, 0x142A8000);

			//Setting AAD PLL for AAD audio module
			ADEC_MIP_DEBUG("CTOP RD R: 0x9C(CTR35) : 0x%X\n", CTOP_CTRL_READ(0x9C));
			ADEC_MIP_DEBUG("CTOP RD R: 0xA0(CTR36) : 0x%X\n", CTOP_CTRL_READ(0xA0));

			CTOP_CTRL_WRITE(0x9C, 0xB2FA99D0);
			CTOP_CTRL_WRITE(0xA0, 0x476B4000);

			ADEC_MIP_DEBUG("CTOP RD W: 0x9C(CTR35) : 0x%X\n", CTOP_CTRL_READ(0x9C));
			ADEC_MIP_DEBUG("CTOP RD W: 0xA0(CTR36) : 0x%X\n", CTOP_CTRL_READ(0xA0));
#else
		if( ui8AdecClockSrc == LX_ADEC_CLOCK_DTV )	//DTV to SPK
		{
			ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetClockSource : LX_ADEC_CLOCK_DTV\n");

			/* Set Clock Path CTR17, CTR18 */
			CTOP_CTRL_WRITE(0x54, 0x00040000);
			CTOP_CTRL_WRITE(0x58, 0x00000000);

			/* Set Clock Division CTR30, CTR31 */
			CTOP_CTRL_WRITE(0x88, 0x00000000);
			CTOP_CTRL_WRITE(0x8C, 0x142A8000);
		}
		else if( ui8AdecClockSrc == LX_ADEC_CLOCK_ATV )	//Analog TV to SPK(default)
		{
			ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetClockSource : LX_ADEC_CLOCK_ATV\n");

			/* Set Clock Path CTR17, CTR18 */
			CTOP_CTRL_WRITE(0x54, 0x04404444);
			CTOP_CTRL_WRITE(0x58, 0x40000000);

			/* Set Clock Division CTR30, CTR31 */
			CTOP_CTRL_WRITE(0x88, 0x20000000);
			CTOP_CTRL_WRITE(0x8C, 0x142A8000);

			//Setting AAD PLL for AAD audio module
			ADEC_MIP_DEBUG("CTOP RD R: 0x9C(CTR35) : 0x%X\n", CTOP_CTRL_READ(0x9C));
			ADEC_MIP_DEBUG("CTOP RD R: 0xA0(CTR36) : 0x%X\n", CTOP_CTRL_READ(0xA0));

			CTOP_CTRL_WRITE(0x9C, 0xB2FA99D0);
			CTOP_CTRL_WRITE(0xA0, 0x476B4000);

			ADEC_MIP_DEBUG("CTOP RD W: 0x9C(CTR35) : 0x%X\n", CTOP_CTRL_READ(0x9C));
			ADEC_MIP_DEBUG("CTOP RD W: 0xA0(CTR36) : 0x%X\n", CTOP_CTRL_READ(0xA0));
		}
		else if( ui8AdecClockSrc == LX_ADEC_CLOCK_HDMI )	//HDMI to SPK
		{
			ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetClockSource : LX_ADEC_CLOCK_HDMI\n");

			/* Set Clock Path CTR17, CTR18 */
			CTOP_CTRL_WRITE(0x54, 0x088C8888);
			CTOP_CTRL_WRITE(0x58, 0x80000000);

			/* Set Clock Division CTR30, CTR31 */
			CTOP_CTRL_WRITE(0x88, 0x00000000);
			CTOP_CTRL_WRITE(0x8C, 0x142A8000);
		}
		else if( ui8AdecClockSrc == LX_ADEC_CLOCK_ADC )	//ADC to SPK
		{
			ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetClockSource : LX_ADEC_CLOCK_ADC\n");

			/* Set Clock Path CTR17, CTR18 */
			CTOP_CTRL_WRITE(0x54, 0x00040000);
			CTOP_CTRL_WRITE(0x58, 0x00000000);

			/* Set Clock Division CTR30, CTR31 */
			CTOP_CTRL_WRITE(0x88, 0x00000000);
			CTOP_CTRL_WRITE(0x8C, 0x142A8000);
		}
		else
		{
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetClockSource : 0x%X(Not Supported!!!)\n", ui8AdecClockSrc);
			return RET_ERROR;
		}
#endif

		ADEC_MIP_DEBUG("CTOP RD R: 0x88(CTR30) : 0x%X\n", CTOP_CTRL_READ(0x88));
		ADEC_MIP_DEBUG("CTOP RD R: 0x8C(CTR31) : 0x%X\n", CTOP_CTRL_READ(0x8C));
	}
	else if (lx_chip_rev() == LX_CHIP_REV(L8, A1))
	{
		if( ui8AdecClockSrc == LX_ADEC_CLOCK_DTV )	//DTV to SPK
		{
			CTOP_CTRL_WRITE(0x54, 0x55154000);
			CTOP_CTRL_WRITE(0x58, 0x4A524908);
		}
		else if( ui8AdecClockSrc == LX_ADEC_CLOCK_ATV )	//Analog TV to SPK(default)
		{
			CTOP_CTRL_WRITE(0x54, 0x00400000);
			CTOP_CTRL_WRITE(0x58, 0x00000000);

			//Setting AAD PLL for AAD audio module
			ADEC_MIP_DEBUG("CTOP RD R: 0x9C(CTR35) : 0x%X\n", CTOP_CTRL_READ(0x9C));
			ADEC_MIP_DEBUG("CTOP RD R: 0xA0(CTR36) : 0x%X\n", CTOP_CTRL_READ(0xA0));

			CTOP_CTRL_WRITE(0x9C, 0xB2FA99D0);
			CTOP_CTRL_WRITE(0xA0, 0x476B4000);

			ADEC_MIP_DEBUG("CTOP RD W: 0x9C(CTR35) : 0x%X\n", CTOP_CTRL_READ(0x9C));
			ADEC_MIP_DEBUG("CTOP RD W: 0xA0(CTR36) : 0x%X\n", CTOP_CTRL_READ(0xA0));
		}
		else if( ui8AdecClockSrc == LX_ADEC_CLOCK_HDMI )	//HDMI to SPK
		{
			CTOP_CTRL_WRITE(0x54, 0x55154000);
			CTOP_CTRL_WRITE(0x58, 0x4A524908);
		}
		else if( ui8AdecClockSrc == LX_ADEC_CLOCK_ADC )	//ADC to SPK
		{
			CTOP_CTRL_WRITE(0x54, 0x55154000);
			CTOP_CTRL_WRITE(0x58, 0x4A524908);
		}
		else
		{
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetClockSource : 0x%X(Not Supported!!!)\n", ui8AdecClockSrc);
			return RET_ERROR;
		}
	}
	else
	{
		ADEC_ERROR("MIXED_IP_ADEC_DRV_SetClockSource : LX_CHIP_REV => Unknown(0x%X)\n", lx_chip_rev());
		return RET_ERROR;
	}

	ADEC_MIP_DEBUG("CTOP RD W: 0x54(CTR17) : 0x%X\n", CTOP_CTRL_READ(0x54));
	ADEC_MIP_DEBUG("CTOP RD W: 0x58(CTR18) : 0x%X\n", CTOP_CTRL_READ(0x58));

	ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetClockSource : %d\n", ui8AdecClockSrc);
	return RET_OK;
}

/**
 * Initializes the CTOP CTRL clock module register value for setting sampling frequency.(only lower L8 A1 Chip)
 * @see ADEC_Init
*/
int MIXED_IP_ADEC_L8_SetSamplingFreq ( LX_ADEC_SAMPLING_FREQ_T ui32SamplingFreq )
{
	//Setting a sample frequency
	//Setting Digital Audio PLL
	ADEC_MIP_DEBUG("CTOP RD R: 0x48(CTR14) : 0x%X\n", CTOP_CTRL_READ(0x48));
	ADEC_MIP_DEBUG("CTOP RD R: 0x4C(CTR15) : 0x%X\n", CTOP_CTRL_READ(0x4C));
	ADEC_MIP_DEBUG("CTOP RD R: 0x50(CTR16) : 0x%X\n", CTOP_CTRL_READ(0x50));
	ADEC_MIP_DEBUG("CTOP RD R: 0x54(CTR17) : 0x%X\n", CTOP_CTRL_READ(0x54));

	//Setting Audio PLL
	ADEC_MIP_DEBUG("CTOP RD R: 0xE0(CTR52) : 0x%X\n", CTOP_CTRL_READ(0xE0));
	ADEC_MIP_DEBUG("CTOP RD R: 0xE4(CTR53) : 0x%X\n", CTOP_CTRL_READ(0xE4));

	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_NONE )
		{
			CTOP_CTRL_WRITE(0xE0, 0x80000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : ui32SamplingFreq = %d => 48KHz\n", ui32SamplingFreq);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_48_KHZ )
		{
#if 1
			CTOP_CTRL_WRITE(0xE0, 0x80000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);
#else
			//Set for minimum jitter setting
			CTOP_CTRL_WRITE(0xE0, 0x00322000);
			CTOP_CTRL_WRITE(0xE4, 0xB5340000);

			//Set for maximum jitter setting
			//CTOP_CTRL_WRITE(0xE0, 0x03858000);
			//CTOP_CTRL_WRITE(0xE4, 0x79340000);
#endif
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ )
		{
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : 44_1KHZ\n" );

			CTOP_CTRL_WRITE(0xE0, 0x84000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ )
		{
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : 32_KHZ\n" );

			CTOP_CTRL_WRITE(0xE0, 0x88000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);
		}
		else
		{
			CTOP_CTRL_WRITE(0xE0, 0x80000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : %d => 48KHz\n", ui32SamplingFreq);
		}
	}
	else if (lx_chip_rev() == LX_CHIP_REV(L8, A1))
	{
		if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_NONE )
		{
			CTOP_CTRL_WRITE(0xE0, 0x80000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : ui32SamplingFreq = %d => 48KHz\n", ui32SamplingFreq);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_48_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x80000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_44_1KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x84000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_4_KHZ )
		{
			CTOP_CTRL_WRITE(0x48, 0x00003056);
			CTOP_CTRL_WRITE(0x4C, 0x00A97782);
			CTOP_CTRL_WRITE(0x50, 0x00000016);
			CTOP_CTRL_WRITE(0x54, 0xAAAA8000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_8_KHZ )
		{
			CTOP_CTRL_WRITE(0x48, 0x00003056);
			CTOP_CTRL_WRITE(0x4C, 0x0152EF04);
			CTOP_CTRL_WRITE(0x50, 0x00000016);
			CTOP_CTRL_WRITE(0x54, 0xAAAA8000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_11_025KHZ )
		{
			CTOP_CTRL_WRITE(0x48, 0x00001ADA);
			CTOP_CTRL_WRITE(0x4C, 0x01D317CB);
			CTOP_CTRL_WRITE(0x50, 0x00000026);
			CTOP_CTRL_WRITE(0x54, 0xAAAA8000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_12_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x01C26000);
			CTOP_CTRL_WRITE(0xE4, 0xF1100000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_16_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x01C28000);
			CTOP_CTRL_WRITE(0xE4, 0xF1100000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_22_05KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x01C29300);
			CTOP_CTRL_WRITE(0xE4, 0xC9340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_24_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x01C2C000);
			CTOP_CTRL_WRITE(0xE4, 0xF1340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_32_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x88000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_64_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x94000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_88_2KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x9C000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_96_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x8C000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_128_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x98000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_176_4KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0xA0000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_192_KHZ )
		{
			CTOP_CTRL_WRITE(0xE0, 0x90000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
		}
		else if( ui32SamplingFreq == LX_ADEC_SAMPLING_FREQ_768_KHZ )
		{
			CTOP_CTRL_WRITE(0x48, 0x0000101C);
			CTOP_CTRL_WRITE(0x4C, 0x7F19A18B);
			CTOP_CTRL_WRITE(0x50, 0x00000000);

			CTOP_CTRL_WRITE(0x54, 0xAAAA8000);
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : %d => 192KHz\n", ui32SamplingFreq);
		}
		else
		{
			CTOP_CTRL_WRITE(0xE0, 0x80000000);
			CTOP_CTRL_WRITE(0xE4, 0x01340000);

			CTOP_CTRL_WRITE(0x54, 0x55154000);
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : %d => 48KHz\n", ui32SamplingFreq);
		}
	}
	else
	{
		ADEC_ERROR("MIXED_IP_ADEC_DRV_SetSamplingFreq : LX_CHIP_REV => Unknown(0x%X)\n", lx_chip_rev());
		return RET_ERROR;
	}

	ADEC_MIP_DEBUG("CTOP RD W: 0x48(CTR14) : 0x%X\n", CTOP_CTRL_READ(0x48));
	ADEC_MIP_DEBUG("CTOP RD W: 0x4C(CTR15) : 0x%X\n", CTOP_CTRL_READ(0x4C));
	ADEC_MIP_DEBUG("CTOP RD W: 0x50(CTR16) : 0x%X\n", CTOP_CTRL_READ(0x50));
	ADEC_MIP_DEBUG("CTOP RD W: 0x54(CTR17) : 0x%X\n", CTOP_CTRL_READ(0x54));

	ADEC_MIP_DEBUG("CTOP RD W: 0xE0(CTR52) : 0x%X\n", CTOP_CTRL_READ(0xE0));
	ADEC_MIP_DEBUG("CTOP RD W: 0xE4(CTR53) : 0x%X\n", CTOP_CTRL_READ(0xE4));

	ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetSamplingFreq : %dKHz\n", ui32SamplingFreq);
	ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetSamplingFreq : %dKHz\n", ui32SamplingFreq);

	return RET_OK;
}

/**
 * Initializes the Mixed IP ADC, MIC ADC, DAC and HP module register value.
 * @see ADEC_Init
*/
int MIXED_IP_ADEC_L8_InitCodecRegisters ( void )
{
	///////////////////////////////////////////////////////////////////////////
	//initial set up : all block power-up
	//COMPONENT L/R input(CH1) is input for ADEC
	//SCART 0 Output is enabled from ADEC output
	////////////////////////////////////////////////////////////////////////////

	//L8 B0 Support Function : DAC0 -> SCART, DAC1 -> Line Out
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		////////////////////////////////////////////////////////////////////////////
		/* CTR37 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr37);
		ADEC_MIP_DEBUG("\n");
		ADEC_MIP_DEBUG("RD : CTR37 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr37));

		CTOP_CTRL_B0_Wr01(ctr37, auad_ccon,				0b011); // ADC current control
		CTOP_CTRL_B0_Wr01(ctr37, auad_gcon,			   0b0011); // ADC analog gain control : 0011 is changed by performance enhancement from IP tesm.
		CTOP_CTRL_B0_Wr01(ctr37, auad_ch_sel,			0b000); // ADC channel select from input(0 : CH1, ... 5 : CH6)
																// CH1 : COMP1, CH2 : AV2, CH3 : AV1, CH4 : COMP2/SCART, CH5 : PC
		CTOP_CTRL_B0_Wr01(ctr37, auad_dc_sel,			  0b0); // ADC DC selection
		CTOP_CTRL_B0_Wr01(ctr37, aud_lindrv1_ch_sel,	0b000); // SCART1 output selection => 000 : SCART 0, 011 : DAC1(Not used in H/W)
		CTOP_CTRL_B0_Wr01(ctr37, aud_lindrv0_ch_sel,	0b011); // SCART0 output selection => 000 : SCART 0, 011 : DAC0 output mode
		CTOP_CTRL_B0_Wr01(ctr37, auad_pdb_in,			  0b1); // ADC power-up
		CTOP_CTRL_B0_Wr01(ctr37, auad_rec_ch_mute, 		  0b1); // ADC record channel un-mute
		CTOP_CTRL_B0_Wr01(ctr37, aud_lin_drv_ch_mute,	  0b1); // SCART 0, 1 output un-mute
		CTOP_CTRL_B0_Wr01(ctr37, auda_bci, 				0b100); // DAC bias current control
#ifdef ENABLE_HPDRV_PORT
		CTOP_CTRL_B0_Wr01(ctr37, hp_slop_dig,			0b000); // HP slope control => 000 : 1ns, 111 : 3ns
		CTOP_CTRL_B0_Wr01(ctr37, hp_test1, 				 0b00); // HP internal clock control
#endif

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR37 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr37));
		CTOP_CTRL_B0_WrFL(ctr37);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr37);
		ADEC_MIP_DEBUG("WR : CTR37 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr37));

#ifdef ENABLE_HPDRV_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR38 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr38);
		ADEC_MIP_DEBUG("RD : CTR38 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr38));

		CTOP_CTRL_B0_Wr01(ctr38, hp_ssp_cv,				  0b0); // HP VCO current control(0 : system , 1 : DDR clock)
		CTOP_CTRL_B0_Wr01(ctr38, hp_ssp_cih,			0b011); // HP charge pump control
		CTOP_CTRL_B0_Wr01(ctr38, hp_ssp_sr,			   0b0000); // HP spread ratio control
		CTOP_CTRL_B0_Wr01(ctr38, hp_ssp_ms,				0b000); // HP modulation speed control
		CTOP_CTRL_B0_Wr01(ctr38, hp_ssp_od,				 0b00); // HP output dividing value
		CTOP_CTRL_B0_Wr01(ctr38, hp_ssp_m, 				 0b00); // HP forward dividing value
		CTOP_CTRL_B0_Wr01(ctr38, hp_ssp_n, 		   0b00100000); // HP feedback dividing value
		CTOP_CTRL_B0_Wr01(ctr38, hp_pdb,				  0b1); // HP power-up

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR38 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr38));
		CTOP_CTRL_B0_WrFL(ctr38);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr38);
		ADEC_MIP_DEBUG("WR : CTR38 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr38));
#endif	//#ifdef ENABLE_HPDRV_PORT

#ifndef ENABLE_SCART1_MIC_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR39 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr39);
		ADEC_MIP_DEBUG("RD : CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));

		CTOP_CTRL_B0_Wr01(ctr39, aumi_pdb_in,			  0b0); // MIC ADC power-down

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));
		CTOP_CTRL_B0_WrFL(ctr39);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr39);
		ADEC_MIP_DEBUG("WR : CTR39 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr39));
#else
		////////////////////////////////////////////////////////////////////////////
		/* CTR39 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr39);
		ADEC_MIP_DEBUG("RD : CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));

		CTOP_CTRL_B0_Wr01(ctr39, aumi_cnt,			  0b00100); // MIC ADC intrnal comparison reference
		CTOP_CTRL_B0_Wr01(ctr39, aumi_cont, 		   0b1100); // MIC ADC pre-amplifier gain control : 20dB
		CTOP_CTRL_B0_Wr01(ctr39, aumi_ccon, 			0b100); // MIC ADC current control : Typical
		CTOP_CTRL_B0_Wr01(ctr39, aumi_pdb_in,			  0b1); // MIC ADC power-up
		CTOP_CTRL_B0_Wr01(ctr39, aumi_pop,				  0b1); // MIC ADC pop noise control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));
		CTOP_CTRL_B0_WrFL(ctr39);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr39);
		ADEC_MIP_DEBUG("WR : CTR39 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr39));
#endif	//#ifndef ENABLE_SCART1_MIC_PORT

		////////////////////////////////////////////////////////////////////////////
		/* CTR40 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr40);
		ADEC_MIP_DEBUG("RD : CTR40 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr40));

		CTOP_CTRL_B0_Wr01(ctr40, tdi,					 0b00); // DAC (Initial 0)
		CTOP_CTRL_B0_Wr01(ctr40, dem0, 					 0b00); // DAC demodulation control(00 : off, 11 : on)
		CTOP_CTRL_B0_Wr01(ctr40, dif0, 					 0b00); // DAC I2S format <0>(11 : Left-justified 24bit, 00 : I2S normal)
		CTOP_CTRL_B0_Wr01(ctr40, dif1, 					 0b00); // DAC I2S format <1>(00 : Left-justified 24bit, 00 : I2S normal)
		CTOP_CTRL_B0_Wr01(ctr40, test0,					 0b00); // DAC test0 : 00
		CTOP_CTRL_B0_Wr01(ctr40, test1,					 0b00); // DAC test1 : 00
		CTOP_CTRL_B0_Wr01(ctr40, sgm_zero_sel, 			 0b00); // DAC zero selection control(00 : off, 11 : on)
		CTOP_CTRL_B0_Wr01(ctr40, out_sel,				 0b00); // DAC select output bit : 00(Initial)

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR40 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr40));
		CTOP_CTRL_B0_WrFL(ctr40);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr40);
		ADEC_MIP_DEBUG("WR : CTR40 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr40));

#ifdef ENABLE_HPDRV_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR41 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr41);
		ADEC_MIP_DEBUG("RD : CTR41 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr41));

		CTOP_CTRL_B0_Wr01(ctr41, dat_hp_vol_con,	0b000000); // HP volume control
		CTOP_CTRL_B0_Wr01(ctr41, dif,				    0b00); // HP I2S format(00 : I2S 24 bit, 01 : Left-justified 24bit)
		CTOP_CTRL_B0_Wr01(ctr41, pwm_sel,				 0b1); // HP PWM format control(0 : mode1, 1 : mode2)

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR41 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr41));
		CTOP_CTRL_B0_WrFL(ctr41);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr41);
		ADEC_MIP_DEBUG("WR : CTR41 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr41));
#endif

		////////////////////////////////////////////////////////////////////////////
		/* CTR42 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr42);
		ADEC_MIP_DEBUG("RD : CTR42 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr42));

		CTOP_CTRL_B0_Wr01(ctr42, cont, 					 0b00); // ADC I2S output format(00 : I2S, 01 : Left-justified)

		//L/R channel in L8 B0 is changed to R/L output. -> Not Used
		CTOP_CTRL_B0_Wr01(ctr42, pol_con,				 0b11); // ADC I2S trigger control(11 : normal, 10 : rising edge, 00 : rising edge, LR change, 01 : LR change)
		CTOP_CTRL_B0_Wr01(ctr42, bm_20,					  0b1); // ADC I2S data mode(1 : 20 bit, 0 : 16 bit)
		CTOP_CTRL_B0_Wr01(ctr42, vol_gain, 			 0b100000); // ADC volume control(100000 : 0dB)
		CTOP_CTRL_B0_Wr01(ctr42, dig_sel,				 0b00); // ADC digital filter out selection(00 : SINC + IIR6 + IIR1)
		CTOP_CTRL_B0_Wr01(ctr42, testl,					  0b0); // ADC test1
		CTOP_CTRL_B0_Wr01(ctr42, testr,					  0b0); // ADC testr
		CTOP_CTRL_B0_Wr01(ctr42, testinl,				  0b0); // ADC testinl
		CTOP_CTRL_B0_Wr01(ctr42, testinr,				  0b0); // ADC testinr

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR42 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr42));
		CTOP_CTRL_B0_WrFL(ctr42);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr42);
		ADEC_MIP_DEBUG("WR : CTR42 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr42));

#ifdef ENABLE_SCART1_MIC_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR43 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr43);
		ADEC_MIP_DEBUG("RD : CTR43 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr43));

		CTOP_CTRL_B0_Wr01(ctr43, cont_mic, 				 0b00); // MIC ADC I2S output format(00 : I2S, 01 : Left-justified)
		CTOP_CTRL_B0_Wr01(ctr43, pol_con_mic,			 0b11); // MIC ADC I2S trigger control(11 : normal, 10 : rising edge, 00 : rising edge, LR change, 01 : LR change)
		CTOP_CTRL_B0_Wr01(ctr43, bm_20_mic,				  0b1); // MIC ADC I2S data mode(1 : 20 bit, 0 : 16 bit)
		CTOP_CTRL_B0_Wr01(ctr43, vol_gain_mic, 		 0b100000); // MIC ADC volume control(100000 : 0dB)
		CTOP_CTRL_B0_Wr01(ctr43, dig_sel_mic,			 0b00); // MIC ADC digital filter out selection(00 : SINC + IIR6 + IIR1)
		CTOP_CTRL_B0_Wr01(ctr43, test_mic, 				  0b0); // MIC ADC test_mic
		CTOP_CTRL_B0_Wr01(ctr43, testin_mic,			  0b0); // MIC ADC testin_mic

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR43 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr43));
		CTOP_CTRL_B0_WrFL(ctr43);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr43);
		ADEC_MIP_DEBUG("WR : CTR43 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr43));
#endif	//#ifdef ENABLE_SCART1_MIC_PORT

		////////////////////////////////////////////////////////////////////////////
		/* CTR64 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr64);
		ADEC_MIP_DEBUG("RD : CTR64 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr64));

		CTOP_CTRL_B0_Wr01(ctr64, dac0_vol_con, 		  0x0000); 	// 14 bit : Line DAC CH0 Volume Control
		CTOP_CTRL_B0_Wr01(ctr64, dac0_rate_cont,	   0x004); 	// 9 bit : Line DAC CH0 Volume Change Rate Control
		CTOP_CTRL_B0_Wr01(ctr64, dac0_rate_cont_mute,  0x004); 	// 9 bit : Line DAC CH0 Mute Rate Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR64 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr64));
		CTOP_CTRL_B0_WrFL(ctr64);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr64);
		ADEC_MIP_DEBUG("WR : CTR64 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr64));

		////////////////////////////////////////////////////////////////////////////
		/* CTR65 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr65);
		ADEC_MIP_DEBUG("RD : CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));

		CTOP_CTRL_B0_Wr01(ctr65, dac0_mute_ena, 		 0x1);	// 1 bit : Line DAC CH0 Mute Control
		CTOP_CTRL_B0_Wr01(ctr65, dac0_pop_dc_on_control, 0x0); 	// 1 bit : Line DAC CH0 Soft Starter Start Control
		CTOP_CTRL_B0_Wr01(ctr65, dac0_pop_con_rate,	  0x000A); 	// 14 bit : Line DAC CH0 Soft Start Rate Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));
		CTOP_CTRL_B0_WrFL(ctr65);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr65);
		ADEC_MIP_DEBUG("WR : CTR65 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr65));

#ifdef ENABLE_SCART1_MIC_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR66 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr66);
		ADEC_MIP_DEBUG("RD : CTR66 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr66));

		CTOP_CTRL_B0_Wr01(ctr66, dac1_vol_con, 		 0x0000);	// 14 bit : Line DAC CH1 Volume Control
		CTOP_CTRL_B0_Wr01(ctr66, dac1_rate_cont,	  0x004); 	// 9 bit : Line DAC CH1 Volume Change Rate Control
		CTOP_CTRL_B0_Wr01(ctr66, dac1_rate_cont_mute, 0x004); 	// 9 bit : Line DAC CH1 Mute Rate Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR66 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr66));
		CTOP_CTRL_B0_WrFL(ctr66);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr66);
		ADEC_MIP_DEBUG("WR : CTR66 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr66));

		////////////////////////////////////////////////////////////////////////////
		/* CTR67 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr67);
		ADEC_MIP_DEBUG("RD : CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));

		CTOP_CTRL_B0_Wr01(ctr67, dac1_mute_ena,			0x1);	// 1 bit : Line DAC CH1 Mute Control
		CTOP_CTRL_B0_Wr01(ctr67, dac1_pop_dc_on_control,0x0);	// 1 bit : Line DAC CH1 Soft Starter Start Control
		CTOP_CTRL_B0_Wr01(ctr67, dac1_pop_con_rate,	 0x000A);	// 14 bit : Line DAC CH1 Soft Start Rate Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));
		CTOP_CTRL_B0_WrFL(ctr67);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr67);
		ADEC_MIP_DEBUG("WR : CTR67 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr67));
#endif	//#ifdef ENABLE_SCART1_MIC_PORT

		////////////////////////////////////////////////////////////////////////////
		/* CTR68 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr68);
		ADEC_MIP_DEBUG("RD : CTR68 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr68));

		CTOP_CTRL_B0_Wr01(ctr68, dac0_dc_setup_byps,	0x1);	// 1 bit : Line DAC CH0 Soft Starter Bypass Control
		CTOP_CTRL_B0_Wr01(ctr68, dac1_dc_setup_byps,	0x1);	// 1 bit : Line DAC CH1 Soft Starter Bypass Control
		CTOP_CTRL_B0_Wr01(ctr68, dac_pop_invert,		0x1);	// 1 bit : Line DAC Soft Start Data Invert Control
#ifdef ENABLE_HPDRV_PORT
		CTOP_CTRL_B0_Wr01(ctr68, hpdrv_pwm_sel,	 	   0b11);	// 2 bit : HP Driver operation mode selection(00 : 98MHz, 11 : 398MHz)
#endif

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR68 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr68));
		CTOP_CTRL_B0_WrFL(ctr68);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr68);
		ADEC_MIP_DEBUG("WR : CTR68 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr68));

		////////////////////////////////////////////////////////////////////////////
		/* CTR69 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr69);
		ADEC_MIP_DEBUG("RD : CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));

#ifdef ENABLE_HPDRV_PORT
		CTOP_CTRL_B0_Wr01(ctr69, hpdrv_inverter_sel, 	 0b0);	// 1 bit : HP Driver Start-up Option(0 : GND, 1 : VDD)
#endif
		CTOP_CTRL_B0_Wr01(ctr69, auda_pdb,				0b01);	// 2 bit : DAC power-up : SCART 0 Power-Up
		CTOP_CTRL_B0_Wr01(ctr69, auda_pdb_dse,		    0b01);	// 2 bit : Audio DAC Diff to Single Converter Power Down
		CTOP_CTRL_B0_Wr01(ctr69, aud_pdb_ldrv_in,		 0b1);	// 1 bit : SCART0 output power down control : Power-Up

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
		CTOP_CTRL_B0_WrFL(ctr69);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr69);
		ADEC_MIP_DEBUG("WR : CTR69 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr69));

#ifdef ENABLE_HPDRV_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR70 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr70);
		ADEC_MIP_DEBUG("RD : CTR70 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr70));

		CTOP_CTRL_B0_Wr01(ctr70, hp_vol_con,		 0x0000);	// 14 bit : HP Driver Volume Control
		CTOP_CTRL_B0_Wr01(ctr70, hp_rate_cont, 	      0x004);	// 9 bit : HP Driver Volume Change Rate Control
		CTOP_CTRL_B0_Wr01(ctr70, hp_rate_cont_mute,   0x004);	// 9 bit : HP Driver Mute Rate Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR70 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr70));
		CTOP_CTRL_B0_WrFL(ctr70);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr70);
		ADEC_MIP_DEBUG("WR : CTR70 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr70));

		////////////////////////////////////////////////////////////////////////////
		/* CTR71 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr71);
		ADEC_MIP_DEBUG("RD : CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));

		CTOP_CTRL_B0_Wr01(ctr71, hp_mute_ena,			0b1);	// 1 bit : HP Driver Mute Control
		CTOP_CTRL_B0_Wr01(ctr71, hp_pop_dc_on_control, 	0b0);	// 9 bit : HP Driver Soft Starter Start Control
		CTOP_CTRL_B0_Wr01(ctr71, hp_pop_con_rate,	 0x0004);	// 14 bit : HP Driver Soft Start Rate Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));
		CTOP_CTRL_B0_WrFL(ctr71);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr71);
		ADEC_MIP_DEBUG("WR : CTR71 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr71));
#endif	//#ifdef ENABLE_HPDRV_PORT

		////////////////////////////////////////////////////////////////////////////
		/* CTR72 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr72);
		ADEC_MIP_DEBUG("RD : CTR72 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr72));

		//L/R channel in L8 B0 is changed to R/L output.
		CTOP_CTRL_B0_Wr01(ctr72, pol_con,			   0b10);	// 2 bit : ADC I2S trigger control(11 : normal, 10 : rising edge, 00 : normal, LR change, 01 : LR change)
		CTOP_CTRL_B0_Wr01(ctr72, twos_com, 				0b1);	// 1 bit : ADC two's complement data stream(0 : unsigned data, 1 : two's compliment data)
		CTOP_CTRL_B0_Wr01(ctr72, ladc_vol_con,	  	 0x0000);	// 14 bit : ADC Volume Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR72 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr72));
		CTOP_CTRL_B0_WrFL(ctr72);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr72);
		ADEC_MIP_DEBUG("WR : CTR72 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr72));

		////////////////////////////////////////////////////////////////////////////
		/* CTR73 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr73);
		ADEC_MIP_DEBUG("RD : CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

		CTOP_CTRL_B0_Wr01(ctr73, ladc_rate_cont,		0x004);	// 9 bit : Line ADC Volume Change Rate Control
		CTOP_CTRL_B0_Wr01(ctr73, ladc_rate_cont_mute,   0x004);	// 9 bit : Line ADC Mute Rate Control
		CTOP_CTRL_B0_Wr01(ctr73, ladc_mute_ena, 	      0b1);	// 1 bit : Line ADC Mute Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));
		CTOP_CTRL_B0_WrFL(ctr73);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr73);
		ADEC_MIP_DEBUG("WR : CTR73 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr73));

#ifdef ENABLE_SCART1_MIC_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR74 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr74);
		ADEC_MIP_DEBUG("RD : CTR74 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr74));

		CTOP_CTRL_B0_Wr01(ctr74, pol_con_mic,		   0b00);	// 2 bit : MIC ADC I2S trigger control(11 : normal, 10 : rising edge, 00 : rising edge, LR change, 01 : LR change)
		CTOP_CTRL_B0_Wr01(ctr74, twos_com_mic, 			0b1);	// 1 bit : MIC ADC two's complement data stream(0 : unsigned data, 1 : two's compliment data)
		CTOP_CTRL_B0_Wr01(ctr74, vol_con_mic, 		 0x0000);	// 14 bit : MIC ADC Volume Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR74 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr74));
		CTOP_CTRL_B0_WrFL(ctr74);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr74);
		ADEC_MIP_DEBUG("WR : CTR74 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr74));

		////////////////////////////////////////////////////////////////////////////
		/* CTR75 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr75);
		ADEC_MIP_DEBUG("RD : CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));

		CTOP_CTRL_B0_Wr01(ctr75, rate_cont_mic,		  0x004);	// 9 bit : MIC ADC Volume Change Rate Control
		CTOP_CTRL_B0_Wr01(ctr75, rate_cont_mute_mic,  0x004);	// 9 bit : MIC ADC Mute Rate Control
		CTOP_CTRL_B0_Wr01(ctr75, mute_ena_mic,			0b1);	// 1 bit : MIC ADC Mute Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));
		CTOP_CTRL_B0_WrFL(ctr75);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr75);
		ADEC_MIP_DEBUG("WR : CTR75 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr75));
#endif	//#ifdef ENABLE_SCART1_MIC_PORT
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////
		/* CTR37 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr37);
		ADEC_MIP_DEBUG("\n");
		ADEC_MIP_DEBUG("RD : CTR37 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr37));

		CTOP_CTRL_A0_Wr01(ctr37, auda_pdb, 				0b111); // DAC power-up
		CTOP_CTRL_A0_Wr01(ctr37, auad_ccon,				0b011); // ADC current control
		CTOP_CTRL_A0_Wr01(ctr37, auad_gcon,			   0b0011); // ADC analog gain control : 0011 is changed by performance enhancement from IP tesm.
		CTOP_CTRL_A0_Wr01(ctr37, auad_ch_sel,			0b000); // ADC channel select from input(0 : CH1, ... 5 : CH6)
																// CH1 : COMP/SCART, CH2 : CVBS, CH3 : X, CH4 : COMP, CH5 : PC
		CTOP_CTRL_A0_Wr01(ctr37, auad_dc_sel,			  0b0); // ADC DC selection
		CTOP_CTRL_A0_Wr01(ctr37, aud_lindrv1_ch_sel,	0b000); // SCART1 output selection => 000 : SCART 0, 011 : DAC1(Not used in H/W)
		CTOP_CTRL_A0_Wr01(ctr37, aud_lindrv0_ch_sel,	0b011); // SCART0 output selection => 000 : SCART 0, 011 : DAC0 output mode
		CTOP_CTRL_A0_Wr01(ctr37, auad_pdb_in,			  0b1); // ADC power-up
		CTOP_CTRL_A0_Wr01(ctr37, auad_rec_ch_mute, 		  0b1); // ADC record channel un-mute
		CTOP_CTRL_A0_Wr01(ctr37, aud_pdb_ldrv_in,		  0b1); // SCART 0, 1 output power-up
		CTOP_CTRL_A0_Wr01(ctr37, aud_lin_drv_ch_mute,	  0b1); // SCART 0, 1 output un-mute
		CTOP_CTRL_A0_Wr01(ctr37, auda_bci, 				0b100); // DAC bias current control
#ifdef ENABLE_HPDRV_PORT
		CTOP_CTRL_A0_Wr01(ctr37, hp_slop_dig,			0b000); // HP slope control => 000 : 1ns, 111 : 3ns
		CTOP_CTRL_A0_Wr01(ctr37, hp_test1, 				 0b00); // HP internal clock control
#endif

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR37 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr37));
		CTOP_CTRL_A0_WrFL(ctr37);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr37);
		ADEC_MIP_DEBUG("WR : CTR37 : 0x%8X\n\n", CTOP_CTRL_A0_Rd(ctr37));
		//ADEC_DEBUG("WR : CTR37 : 0x%X\n", CTOP_CTRL_B0_Rd(ctr37));

		////////////////////////////////////////////////////////////////////////////
		/* CTR38 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr38);
		ADEC_MIP_DEBUG("RD : CTR38 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr38));

		CTOP_CTRL_A0_Wr01(ctr38, hp_ssp_cv,				  0b0); // HP VCO current control(0 : system , 1 : DDR clock)
		CTOP_CTRL_A0_Wr01(ctr38, hp_ssp_cih,			0b001); // HP charge pump control
		CTOP_CTRL_A0_Wr01(ctr38, hp_ssp_sr,			   0b0000); // HP spread ratio control
		CTOP_CTRL_A0_Wr01(ctr38, hp_ssp_ms,				0b000); // HP modulation speed control
		CTOP_CTRL_A0_Wr01(ctr38, hp_ssp_od,				 0b00); // HP output dividing value
		CTOP_CTRL_A0_Wr01(ctr38, hp_ssp_m, 				 0b10); // HP forward dividing value
		CTOP_CTRL_A0_Wr01(ctr38, hp_ssp_n, 		   0b00100000); // HP feedback dividing value
		CTOP_CTRL_A0_Wr01(ctr38, hp_pdb,				  0b1); // HP power-up

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR38 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr38));
		CTOP_CTRL_A0_WrFL(ctr38);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr38);
		ADEC_MIP_DEBUG("WR : CTR38 : 0x%8X\n\n", CTOP_CTRL_A0_Rd(ctr38));

		////////////////////////////////////////////////////////////////////////////
		/* CTR39 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr39);
		ADEC_MIP_DEBUG("RD : CTR39 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr39));

		CTOP_CTRL_A0_Wr01(ctr39, aumi_cnt, 			  0b00100); // MIC ADC intrnal comparison reference
		CTOP_CTRL_A0_Wr01(ctr39, aumi_cont,			   0b1100); // MIC ADC pre-amplifier gain control : 20dB
		CTOP_CTRL_A0_Wr01(ctr39, aumi_ccon,				0b100); // MIC ADC current control : Typical
		CTOP_CTRL_A0_Wr01(ctr39, aumi_pdb_in,			  0b1); // MIC ADC power-up
		CTOP_CTRL_A0_Wr01(ctr39, aumi_pop, 				  0b1); // MIC ADC pop noise control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR39 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr39));
		CTOP_CTRL_A0_WrFL(ctr39);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr39);
		ADEC_MIP_DEBUG("WR : CTR39 : 0x%8X\n\n", CTOP_CTRL_A0_Rd(ctr39));

		////////////////////////////////////////////////////////////////////////////
		/* CTR40 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr40);
		ADEC_MIP_DEBUG("RD : CTR40 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr40));

		CTOP_CTRL_A0_Wr01(ctr40, tdi,					0b000); // DAC (Initial 0)
		CTOP_CTRL_A0_Wr01(ctr40, dem0, 					0b000); // DAC demodulation control(00 : off, 11 : on)
		CTOP_CTRL_A0_Wr01(ctr40, dif0, 					0b000); // DAC I2S format <0>(11 : Left-justified 24bit, 00 : I2S normal)
		CTOP_CTRL_A0_Wr01(ctr40, dif1, 					0b000); // DAC I2S format <1>(00 : Left-justified 24bit, 00 : I2S normal)
		CTOP_CTRL_A0_Wr01(ctr40, test0,					0b000); // DAC test0 : 00
		CTOP_CTRL_A0_Wr01(ctr40, test1,					0b000); // DAC test1 : 00
		CTOP_CTRL_A0_Wr01(ctr40, sgm_zero_sel, 			0b000); // DAC zero selection control(00 : off, 11 : on)
		CTOP_CTRL_A0_Wr01(ctr40, out_sel,				0b000); // DAC select output bit : 00(Initial)

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR40 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr40));
		CTOP_CTRL_A0_WrFL(ctr40);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr40);
		ADEC_MIP_DEBUG("WR : CTR40 : 0x%8X\n\n", CTOP_CTRL_A0_Rd(ctr40));

		////////////////////////////////////////////////////////////////////////////
		/* CTR41 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr41);
		ADEC_MIP_DEBUG("RD : CTR41 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr41));

		CTOP_CTRL_A0_Wr01(ctr41, dat_hp_vol_con,	 0b111001); // HP volume control
		CTOP_CTRL_A0_Wr01(ctr41, dif,				     0b00); // HP I2S format(00 : I2S 24 bit, 01 : Left-justified 24bit)
		CTOP_CTRL_A0_Wr01(ctr41, pwm_sel,				  0b1); // HP PWM format control(0 : mode1, 1 : mode2)

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR41 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr41));
		CTOP_CTRL_A0_WrFL(ctr41);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr41);
		ADEC_MIP_DEBUG("WR : CTR41 : 0x%8X\n\n", CTOP_CTRL_A0_Rd(ctr41));

		////////////////////////////////////////////////////////////////////////////
		/* CTR42 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr42);
		ADEC_MIP_DEBUG("RD : CTR42 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr42));

		CTOP_CTRL_A0_Wr01(ctr42, cont, 				    0b000); // ADC I2S output format(000 : I2S, 001 : Left-justified)

		//L/R channel in L8 A0 is changed to R/L outputl.
		CTOP_CTRL_A0_Wr01(ctr42, pol_con,				 0b01); // ADC I2S trigger control(11 : normal, 10 : rising edge, 00 : rising edge, LR change, 01 : LR change)

		CTOP_CTRL_A0_Wr01(ctr42, bm_20,					  0b1); // ADC I2S data mode(1 : 20 bit, 0 : 16 bit)
		CTOP_CTRL_A0_Wr01(ctr42, vol_gain, 			 0b100000); // ADC volume control(100000 : 0dB)
		CTOP_CTRL_A0_Wr01(ctr42, dig_sel,				 0b00); // ADC digital filter out selection(00 : SINC + IIR6 + IIR1)
		CTOP_CTRL_A0_Wr01(ctr42, testl,					  0b0); // ADC test1
		CTOP_CTRL_A0_Wr01(ctr42, testr,					  0b0); // ADC testr
		CTOP_CTRL_A0_Wr01(ctr42, testinl,				  0b0); // ADC testinl
		CTOP_CTRL_A0_Wr01(ctr42, testinr,				  0b0); // ADC testinr

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR42 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr42));
		CTOP_CTRL_A0_WrFL(ctr42);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr42);
		ADEC_MIP_DEBUG("WR : CTR42 : 0x%8X\n\n", CTOP_CTRL_A0_Rd(ctr42));

		////////////////////////////////////////////////////////////////////////////
		/* CTR43 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr43);
		ADEC_MIP_DEBUG("RD : CTR43 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr43));

		CTOP_CTRL_A0_Wr01(ctr43, cont_mic, 				0b000); // MIC ADC I2S output format(000 : I2S, 001 : Left-justified)
		CTOP_CTRL_A0_Wr01(ctr43, pol_con_mic,			 0b11); // MIC ADC I2S trigger control(11 : normal, 10 : rising edge, 00 : rising edge, LR change, 01 : LR change)
		CTOP_CTRL_A0_Wr01(ctr43, bm_20_mic,				  0b1); // MIC ADC I2S data mode(1 : 20 bit, 0 : 16 bit)
		CTOP_CTRL_A0_Wr01(ctr43, vol_gain_mic, 		 0b100000); // MIC ADC volume control(100000 : 0dB)
		CTOP_CTRL_A0_Wr01(ctr43, dig_sel_mic,	  		 0b00); // MIC ADC digital filter out selection(00 : SINC + IIR6 + IIR1)
		CTOP_CTRL_A0_Wr01(ctr43, test_mic, 				  0b0); // MIC ADC test_mic
		CTOP_CTRL_A0_Wr01(ctr43, testin_mic,			  0b0); // MIC ADC testin_mic

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("	 CTR43 : 0x%8X\n", CTOP_CTRL_A0_Rd(ctr43));
		CTOP_CTRL_A0_WrFL(ctr43);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_A0_RdFL(ctr43);
		ADEC_MIP_DEBUG("WR : CTR43 : 0x%8X\n\n", CTOP_CTRL_A0_Rd(ctr43));
	}

	ADEC_DEBUG_TMP("MIXED_IP_ADEC_DRV_SetMIPRegisters : Done!!!\n");

	return RET_OK;
}

/**
 * Control the Mixed IP AAD, ADC, MIC ADC, DAC and HPDRV module to power on and mute off.
 * @see ADEC_DRV_SetSource
*/
int MIXED_IP_ADEC_L8_PowerControl ( LX_ADEC_POWER_BLOCK_T block, LX_ADEC_POWER_MODE_T mode )
{
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		//LX_ADEC_POWER_AAD
		if(block == LX_ADEC_POWER_AAD)
		{
			if( (gADECPowerStatus[LX_ADEC_POWER_AAD].block == block)
			  &&(gADECPowerStatus[LX_ADEC_POWER_AAD].mode  == mode ) )
			{
				//update power block
				gADECPowerStatus[LX_ADEC_POWER_AAD].block = block;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(AAD) : Same mode(%d)\n", mode);
			}
			else
			{			
				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(AAD) : block = %d, mode = %d\n", block, mode);

				//update power block mode
				gADECPowerStatus[LX_ADEC_POWER_AAD].mode = mode;

				if (mode == LX_ADEC_POWER_UP)
				{	
					/* CTR35 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr35);
					ADEC_MIP_DEBUG("RD : CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					
					CTOP_CTRL_B0_Wr01(ctr35, aad_dco_resetb,		  0b1); // AAD DCO reset-on
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					CTOP_CTRL_B0_WrFL(ctr35);

					udelay(AAD_DCO_RESETB_DELAY);		//Spec. => 5 us

					/* CTR36 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr36);
					ADEC_MIP_DEBUG("RD : CTR36 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr36));
					
					CTOP_CTRL_B0_Wr01(ctr36, aad_spll_pdb,		 	 0b1); // AAD SPLL power-up
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR36 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr36));
					CTOP_CTRL_B0_WrFL(ctr36);

					udelay(AAD_SPLL_PDB_DELAY);			//Spec. => 200 us

					/* CTR35 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr35);
					ADEC_MIP_DEBUG("RD : CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					
					CTOP_CTRL_B0_Wr01(ctr35, aad_adc_pdb,			  0b1); // AAD ADC power-up
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					CTOP_CTRL_B0_WrFL(ctr35);

					udelay(AAD_ADC_PDB_DELAY); 		//Spec. => 3 cycles
				}
				else
				{
					/* CTR35 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr35);
					ADEC_MIP_DEBUG("RD : CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					
					CTOP_CTRL_B0_Wr01(ctr35, aad_adc_pdb,			  0b0); // AAD ADC power-down
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					CTOP_CTRL_B0_WrFL(ctr35);

					udelay(MIXEDIP_POWER_OFF_DELAY); 	//Spec. => 1 us

					/* CTR36 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr36);
					ADEC_MIP_DEBUG("RD : CTR36 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr36));
					
					CTOP_CTRL_B0_Wr01(ctr36, aad_spll_pdb,		 	 0b0); // AAD SPLL power-down
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR36 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr36));
					CTOP_CTRL_B0_WrFL(ctr36);

					udelay(MIXEDIP_POWER_OFF_DELAY); 	//Spec. => 1 us

					/* CTR35 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr35);
					ADEC_MIP_DEBUG("RD : CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					
					CTOP_CTRL_B0_Wr01(ctr35, aad_dco_resetb,		  0b0); // AAD DCO reset-off
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR35 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr35));
					CTOP_CTRL_B0_WrFL(ctr35);

					udelay(MIXEDIP_POWER_OFF_DELAY); 	//Spec. => 1 us
				}
			}
		}


		//LX_ADEC_POWER_ADC
		if(block == LX_ADEC_POWER_ADC)
		{
			if( (gADECPowerStatus[LX_ADEC_POWER_ADC].block == block)
			  &&(gADECPowerStatus[LX_ADEC_POWER_ADC].mode  == mode ) )
			{
				//update power block
				gADECPowerStatus[LX_ADEC_POWER_ADC].block = block;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(ADC) : Same mode(%d)\n", mode);
			}
			else
			{		
				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(ADC) : block = %d, mode = %d\n", block, mode);

				//update power block mode
				gADECPowerStatus[LX_ADEC_POWER_ADC].mode = mode;

				if (mode == LX_ADEC_POWER_UP)
				{	
					/* CTR37 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr37);
					ADEC_MIP_DEBUG("RD : CTR37 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr37));
					
					CTOP_CTRL_B0_Wr01(ctr37, auad_pdb_in,			  0b1); // ADC power-up
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR37 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr37));
					CTOP_CTRL_B0_WrFL(ctr37);

					udelay(AUAD_PDB_IN_DELAY);			//Spec. => > 200 us
				}
				else
				{
					/* CTR37 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr37);
					ADEC_MIP_DEBUG("RD : CTR37 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr37));
					
					CTOP_CTRL_B0_Wr01(ctr37, auad_pdb_in,			  0b0); // ADC power-down
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR37 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr37));
					CTOP_CTRL_B0_WrFL(ctr37);

					udelay(MIXEDIP_POWER_OFF_DELAY); 	//Spec. => 1 us
				}
			}	
		}


		//LX_ADEC_POWER_HPDRV
		if(block == LX_ADEC_POWER_HPDRV)
		{
			if( (gADECPowerStatus[LX_ADEC_POWER_HPDRV].block == block)
			  &&(gADECPowerStatus[LX_ADEC_POWER_HPDRV].mode  == mode ) )
			{
				//update power block
				gADECPowerStatus[LX_ADEC_POWER_HPDRV].block = block;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(HPD) : Same mode(%d)\n", mode);
			}
			else
			{
				//update power block mode
				gADECPowerStatus[LX_ADEC_POWER_HPDRV].mode = mode;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(HPD) : block = %d, mode = %d\n", block, mode);

				if (mode == LX_ADEC_POWER_UP)
				{	
					/* CTR38 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr38);
					ADEC_MIP_DEBUG("RD : CTR38 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr38));
					
					CTOP_CTRL_B0_Wr01(ctr38, hp_pdb,				  0b1); // HP power-up
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR38 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr38));
					CTOP_CTRL_B0_WrFL(ctr38);

					udelay(HP_PDB_DELAY);			//Spec. => 40 us		
				}
				else
				{
					udelay(HP_PDB_DELAY);			//Spec. => 40 us		

					/* CTR38 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr38);
					ADEC_MIP_DEBUG("RD : CTR38 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr38));
					
					CTOP_CTRL_B0_Wr01(ctr38, hp_pdb,				  0b0); // HP power-down
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR38 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr38));
					CTOP_CTRL_B0_WrFL(ctr38);
				}
			}	
		}


		//LX_ADEC_POWER_DAC
		if(block == LX_ADEC_POWER_DAC)
		{
			if( (gADECPowerStatus[LX_ADEC_POWER_DAC].block == block)
			  &&(gADECPowerStatus[LX_ADEC_POWER_DAC].mode  == mode ) )
			{
				//update power block
				gADECPowerStatus[LX_ADEC_POWER_DAC].block = block;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(DAC) : Same mode(%d)\n", mode);
			}
			else
			{
				//update power block mode
				gADECPowerStatus[LX_ADEC_POWER_DAC].mode = mode;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(DAC) : block = %d, mode = %d\n", block, mode);

				if (mode == LX_ADEC_POWER_UP)
				{	
					/* CTR69 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr69);
					ADEC_MIP_DEBUG("RD : CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					
					CTOP_CTRL_B0_Wr01(ctr69, auda_pdb,				0b01);	// 2 bit : DAC power-up : SCART 0 Power-Up
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					CTOP_CTRL_B0_WrFL(ctr69);

					udelay(AUDA_PDB_DELAY);			//Spec. => 40 us		

					/* CTR69 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr69);
					ADEC_MIP_DEBUG("RD : CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					
					CTOP_CTRL_B0_Wr01(ctr69, auda_pdb_dse,			0b01);	// 2 bit : Audio DAC Diff to Single Converter Power Down
					CTOP_CTRL_B0_Wr01(ctr69, aud_pdb_ldrv_in,		 0b1);	// 1 bit : SCART0 output power down control : Power-Up
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					CTOP_CTRL_B0_WrFL(ctr69);
					
					udelay(AUDA_PDB_DSE_DELAY);		//Spec. => 40 us		
				}
				else
				{
					/* CTR69 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr69);
					ADEC_MIP_DEBUG("RD : CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					
					CTOP_CTRL_B0_Wr01(ctr69, auda_pdb,				0b00);	// 2 bit : DAC power-down : SCART 0 Power-down
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					CTOP_CTRL_B0_WrFL(ctr69);

					udelay(AUDA_PDB_DELAY);			//Spec. => 40 us		

					/* CTR69 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr69);
					ADEC_MIP_DEBUG("RD : CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					
					CTOP_CTRL_B0_Wr01(ctr69, auda_pdb_dse,			0b00);	// 2 bit : Audio DAC Diff to Single Converter Power Down
					CTOP_CTRL_B0_Wr01(ctr69, aud_pdb_ldrv_in,		 0b0);	// 1 bit : SCART0 output power down control : Power-Down
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR69 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr69));
					CTOP_CTRL_B0_WrFL(ctr69);
					
					udelay(AUDA_PDB_DSE_DELAY);		//Spec. => 40 us		
				}
			}	
		}


		//LX_ADEC_POWER_MIC
		if(block == LX_ADEC_POWER_MIC)
		{
			if( (gADECPowerStatus[LX_ADEC_POWER_MIC].block == block)
			  &&(gADECPowerStatus[LX_ADEC_POWER_MIC].mode  == mode ) )
			{
				//update power block
				gADECPowerStatus[LX_ADEC_POWER_MIC].block = block;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(MIC) : Same mode(%d)\n", mode);
			}
			else
			{
				//update power block mode
				gADECPowerStatus[LX_ADEC_POWER_MIC].mode = mode;

				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_PowerControl(MIC) : block = %d, mode = %d\n", block, mode);

				if (mode == LX_ADEC_POWER_UP)
				{	
					/* CTR39 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr39);
					ADEC_MIP_DEBUG("RD : CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));
					
					CTOP_CTRL_B0_Wr01(ctr39, aumi_pdb_in,			  0b1); // MIC ADC power-up
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));
					CTOP_CTRL_B0_WrFL(ctr39);
					
					udelay(AUMI_PDB_IN_DELAY);			//Spec. => 200 us		
				}
				else
				{
					/* CTR39 ==> Read  FLushing : Shadow <- Real. */
					CTOP_CTRL_B0_RdFL(ctr39);
					ADEC_MIP_DEBUG("RD : CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));
					
					CTOP_CTRL_B0_Wr01(ctr39, aumi_pdb_in,			  0b0); // MIC ADC power-down
					
					/* Write FLushing : Shadow -> Real. */
					ADEC_MIP_DEBUG("	 CTR39 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr39));
					CTOP_CTRL_B0_WrFL(ctr39);
			
					udelay(AUMI_PDB_IN_DELAY);			//Spec. => 200 us
				}
			}
		}
	}
	else
	{
		ADEC_ERROR("MIXED_IP_ADEC_DRV_PowerControl : LX_CHIP_REV => 0x%X\n", lx_chip_rev());
	}

	return RET_OK;	
}

/**
 * Start the Mixed IP ADC, MIC ADC, DAC and HP module to mute off.(only upper L8 B0 Chip)
 * @see ADEC_Init
*/
int MIXED_IP_ADEC_L8_StartCodec ( void )
{
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		////////////////////////////////////////////////////////////////////////////
		/* CTR65 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr65);
		ADEC_MIP_DEBUG("RD : CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));

		CTOP_CTRL_B0_Wr01(ctr65, dac0_mute_ena,				0x0);	// 1 bit : Line DAC CH0 Mute Control
		CTOP_CTRL_B0_Wr01(ctr65, dac0_pop_dc_on_control,	0x1);	// 1 bit : Line DAC CH0 Soft Starter Start Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));
		CTOP_CTRL_WrFL(ctr65);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr65);
		ADEC_MIP_DEBUG("WR : CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));

#ifdef ENABLE_SCART1_MIC_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR67 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr67);
		ADEC_MIP_DEBUG("RD : CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));

		CTOP_CTRL_B0_Wr01(ctr67, dac1_mute_ena,				0x0);	// 1 bit : Line DAC CH1 Mute Control
		CTOP_CTRL_B0_Wr01(ctr67, dac1_pop_dc_on_control,	0x1);	// 1 bit : Line DAC CH1 Soft Starter Start Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));
		CTOP_CTRL_WrFL(ctr67);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr67);
		ADEC_MIP_DEBUG("WR : CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));
#endif	//#ifdef ENABLE_SCART1_MIC_PORT


#ifdef ENABLE_HPDRV_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR71 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr71);
		ADEC_MIP_DEBUG("RD : CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));

		CTOP_CTRL_B0_Wr01(ctr71, hp_mute_ena,				0b0);	// 1 bit : HP Driver Mute Control
		CTOP_CTRL_B0_Wr01(ctr71, hp_pop_dc_on_control, 		0b1);	// 9 bit : HP Driver Soft Starter Start Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));
		CTOP_CTRL_WrFL(ctr71);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr71);
		ADEC_MIP_DEBUG("WR : CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));
#endif	//#ifdef ENABLE_HPDRV_PORT

		////////////////////////////////////////////////////////////////////////////
		/* CTR73 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr73);
		ADEC_MIP_DEBUG("RD : CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

		CTOP_CTRL_B0_Wr01(ctr73, ladc_mute_ena,				0b0);	// 1 bit : Line ADC Mute Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));
		CTOP_CTRL_WrFL(ctr73);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr73);
		ADEC_MIP_DEBUG("WR : CTR72 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

#ifdef ENABLE_SCART1_MIC_PORT
		////////////////////////////////////////////////////////////////////////////
		/* CTR75 ==> Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr75);
		ADEC_MIP_DEBUG("RD : CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));

		CTOP_CTRL_B0_Wr01(ctr75, mute_ena_mic, 				0b0);	// 1 bit : MIC ADC Mute Control

		/* Write FLushing : Shadow -> Real. */
		ADEC_MIP_DEBUG("     CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));
		CTOP_CTRL_WrFL(ctr75);

		/* Read  FLushing : Shadow <- Real. */
		CTOP_CTRL_B0_RdFL(ctr75);
		ADEC_MIP_DEBUG("WR : CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));
#endif	//#ifdef ENABLE_SCART1_MIC_PORT
	}

	ADEC_DEBUG_TMP("MIXED_IP_ADEC_DRV_StartUpCodec : Done\n");

	return RET_OK;
}

/**
 * Stop the Mixed IP ADC, MIC ADC, DAC and HP module to mute off.(only upper L8 B0 Chip)
 * @see ADEC_Init
*/
int MIXED_IP_ADEC_L8_StopCodec ( void )
{
	////////////////////////////////////////////////////////////////////////////
	/* CTR65 ==> Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr65);
	ADEC_MIP_DEBUG("RD : CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));

	CTOP_CTRL_B0_Wr01(ctr65, dac0_mute_ena,				0x0);	// 1 bit : Line DAC CH0 Mute Control
	CTOP_CTRL_B0_Wr01(ctr65, dac0_pop_dc_on_control,	0x1);	// 1 bit : Line DAC CH0 Soft Starter Start Control

	/* Write FLushing : Shadow -> Real. */
	ADEC_MIP_DEBUG("     CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));
	CTOP_CTRL_WrFL(ctr65);

	/* Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr65);
	ADEC_MIP_DEBUG("WR : CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));

#ifdef ENABLE_SCART1_MIC_PORT
	////////////////////////////////////////////////////////////////////////////
	/* CTR67 ==> Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr67);
	ADEC_MIP_DEBUG("RD : CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));

	CTOP_CTRL_B0_Wr01(ctr67, dac1_mute_ena,				0x0);	// 1 bit : Line DAC CH1 Mute Control
	CTOP_CTRL_B0_Wr01(ctr67, dac1_pop_dc_on_control,	0x1);	// 1 bit : Line DAC CH1 Soft Starter Start Control

	/* Write FLushing : Shadow -> Real. */
	ADEC_MIP_DEBUG("     CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));
	CTOP_CTRL_WrFL(ctr67);

	/* Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr67);
	ADEC_MIP_DEBUG("WR : CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));
#endif	//#ifdef ENABLE_SCART1_MIC_PORT

#ifdef ENABLE_HPDRV_PORT
	////////////////////////////////////////////////////////////////////////////
	/* CTR71 ==> Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr71);
	ADEC_MIP_DEBUG("RD : CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));

	CTOP_CTRL_B0_Wr01(ctr71, hp_mute_ena,				0b0);	// 1 bit : HP Driver Mute Control
	CTOP_CTRL_B0_Wr01(ctr71, hp_pop_dc_on_control, 		0b1);	// 9 bit : HP Driver Soft Starter Start Control

	/* Write FLushing : Shadow -> Real. */
	ADEC_MIP_DEBUG("     CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));
	CTOP_CTRL_WrFL(ctr71);

	/* Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr71);
	ADEC_MIP_DEBUG("WR : CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));
#endif	//#ifdef ENABLE_HPDRV_PORT

	////////////////////////////////////////////////////////////////////////////
	/* CTR73 ==> Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr73);
	ADEC_MIP_DEBUG("RD : CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

	CTOP_CTRL_B0_Wr01(ctr73, ladc_mute_ena,				0b0);	// 1 bit : Line ADC Mute Control

	/* Write FLushing : Shadow -> Real. */
	ADEC_MIP_DEBUG("     CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));
	CTOP_CTRL_WrFL(ctr73);

	/* Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr73);
	ADEC_MIP_DEBUG("WR : CTR72 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

#ifdef ENABLE_SCART1_MIC_PORT
	////////////////////////////////////////////////////////////////////////////
	/* CTR75 ==> Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr75);
	ADEC_MIP_DEBUG("RD : CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));

	CTOP_CTRL_B0_Wr01(ctr75, mute_ena_mic, 				0b0);	// 1 bit : MIC ADC Mute Control

	/* Write FLushing : Shadow -> Real. */
	ADEC_MIP_DEBUG("     CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));
	CTOP_CTRL_WrFL(ctr75);

	/* Read  FLushing : Shadow <- Real. */
	CTOP_CTRL_B0_RdFL(ctr75);
	ADEC_MIP_DEBUG("WR : CTR75 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr75));
#endif	//#ifdef ENABLE_SCART1_MIC_PORT

	ADEC_DEBUG_TMP("MIXED_IP_ADEC_DRV_StopCodec : Done\n");

	return RET_OK;

}

/**
 * Sets a ADC port number for ADEC I2S input.
 * @see ADEC_DRV_SetSource
*/
int MIXED_IP_ADEC_L8_SetADCPortNumber ( UINT8 ui8PortNum )
{
	UINT8	value = 0;

	//L8 B0 Support Function : DAC0 -> SCART, DAC1 -> Line Out
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		if(ui8PortNum <= 5) //ui8PortNum 7 is defined for MIC input test
		{
			/* CTR73 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr73);
			ADEC_MIP_DEBUG("RD : CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

			CTOP_CTRL_B0_Wr01(ctr73, ladc_mute_ena, 0b1);	// 1 bit : Line ADC Mute Control

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));
			CTOP_CTRL_WrFL(ctr73);

			/* Wait for pop-noise supression */
			msleep_interruptible(6);	//udelay(600);

			//Reset Low : ADEC_OPTION_ADC
			ADEC_ClearReset(ADEC_OPTION_ADC);

			/* CTR37 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr37);
			ADEC_MIP_DEBUG("RD : CTR37 : 0x%X\n", CTOP_CTRL_B0_Rd(ctr37));

			CTOP_CTRL_B0_Wr01(ctr37, auad_ch_sel, ui8PortNum);	// ADC channel select from input(0 : CH1, ... 5 : CH6)
																// CH1 : SCART, CH2 : CVBS, CH3 : X, CH4 : COMP, CH5 : PC
			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_B0_WrFL(ctr37);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr37);
			ADEC_MIP_DEBUG("WR : CTR37 : 0x%X\n", CTOP_CTRL_B0_Rd(ctr37));

			//Reset High : ADEC_OPTION_ADC
			ADEC_SetReset(ADEC_OPTION_ADC);

			/* CTR73 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr73);
			ADEC_MIP_DEBUG("RD : CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

			CTOP_CTRL_B0_Wr01(ctr73, ladc_mute_ena, 0b0);	// 1 bit : Line ADC Mute Control

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));
			CTOP_CTRL_WrFL(ctr73);
		}
		else if(ui8PortNum == 6) //ui8PortNum 6 is defined for mute for HDMI /DVI is not connectted.
		{
			/* CTR73 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr73);
			ADEC_MIP_DEBUG("RD : CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));

			CTOP_CTRL_B0_Wr01(ctr73, ladc_mute_ena, 0b1);	// 1 bit : Line ADC Mute Control

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR73 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr73));
			CTOP_CTRL_WrFL(ctr73);
		}
#ifdef ENABLE_SCART1_MIC_PORT
		else if(ui8PortNum == 7)
		{
			/* CTR39 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr39);
			ADEC_MIP_DEBUG("RD : CTR39 : 0x%X\n", CTOP_CTRL_B0_Rd(ctr39));

			CTOP_CTRL_B0_Wr01(ctr39, aumi_pdb_in,	0b1); // MIC ADC power-up

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_B0_WrFL(ctr39);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr39);
			ADEC_MIP_DEBUG("WR : CTR39 : 0x%X\n", CTOP_CTRL_B0_Rd(ctr39));
		}
#endif	//#ifdef ENABLE_SCART1_MIC_PORT
		else
		{
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetADCPortNumber : ADC Port = %d(Not Supported.)\n", ui8PortNum);
			return RET_ERROR;
		}

	}
	else
	{
		if(ui8PortNum <= 5)	//ui8PortNum 7 is defined for MIC input test
		{
			/* CTR37 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("RD : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));

			CTOP_CTRL_A0_Wr01(ctr37, auad_ch_sel,    ui8PortNum);	// ADC channel select from input(0 : CH1, ... 5 : CH6)
																	// CH1 : SCART, CH2 : CVBS, CH3 : X, CH4 : COMP, CH5 : PC
			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr37);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			CTOP_CTRL_A0_Wr01(ctr37, auad_ch_sel, value);
			ADEC_MIP_DEBUG("WR : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));
		}
		else if(ui8PortNum == 7)
		{
			/* CTR39 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr39);
			ADEC_MIP_DEBUG("RD : CTR39 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr39));

			CTOP_CTRL_A0_Wr01(ctr39, aumi_pdb_in,		0b1); // MIC ADC power-up

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr39);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr39);
			ADEC_MIP_DEBUG("WR : CTR39 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr39));
		}
		else
		{
			ADEC_ERROR("MIXED_IP_ADEC_DRV_SetADCPortNumber : ADC Port = %d(Not Supported.)\n", ui8PortNum);
			return RET_ERROR;
		}
	}

	ADEC_DEBUG_TMP("MIXED_IP_ADEC_DRV_SetADCPortNumber : ADC Port = %d\n", ui8PortNum);

	return RET_OK;
}

/**
 * Sets the volume of DAC output.
 * @see ADEC_DRV_SetDACOutVolume
*/
int MIXED_IP_ADEC_L8_SetDACOutVolume ( LX_ADEC_DAC_VOL_T stDACOutVol )
{
	//L8 B0 Support Function : DAC0 -> SCART, DAC1 -> Line Out
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		//Set a SCART DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			////////////////////////////////////////////////////////////////////////////
			/* CTR64 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr64);
			ADEC_MIP_DEBUG("RD : CTR64 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr64));

			//Mute when stDACOutVol.volume is 255.
			if(stDACOutVol.hwVolume == 255)
				CTOP_CTRL_B0_Wr01(ctr64, dac0_vol_con, stDACOutVol.hwVolume + 200);	// 14 bit : Line DAC CH0 Volume Control
			else
				CTOP_CTRL_B0_Wr01(ctr64, dac0_vol_con, stDACOutVol.hwVolume);	// 14 bit : Line DAC CH0 Volume Control

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR64 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr64));
			CTOP_CTRL_B0_WrFL(ctr64);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr64);
			ADEC_MIP_DEBUG("WR : CTR64 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr64));
		}

#ifdef ENABLE_SCART1_MIC_PORT
		//Set a Line-out DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC1 )
		{
			////////////////////////////////////////////////////////////////////////////
			/* CTR66 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr66);
			ADEC_MIP_DEBUG("RD : CTR66 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr66));

			//Mute when stDACOutVol.volume is 255.
			if(stDACOutVol.hwVolume == 255)
				CTOP_CTRL_B0_Wr01(ctr66, dac1_vol_con, stDACOutVol.hwVolume + 200);	// 14 bit : Line DAC CH1 Volume Control
			else
				CTOP_CTRL_B0_Wr01(ctr66, dac1_vol_con, stDACOutVol.hwVolume);	// 14 bit : Line DAC CH1 Volume Control

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR66 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr66));
			CTOP_CTRL_B0_WrFL(ctr66);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr66);
			ADEC_MIP_DEBUG("WR : CTR66 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr66));
		}
#endif	//#ifdef ENABLE_SCART1_MIC_PORT

#ifdef ENABLE_HPDRV_PORT
		//Set a HeadPhone DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			////////////////////////////////////////////////////////////////////////////
			/* CTR70 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr70);
			ADEC_MIP_DEBUG("RD : CTR70 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr70));

			//Mute when stDACOutVol.volume is 255.
			if(stDACOutVol.hwVolume == 255)
				CTOP_CTRL_B0_Wr01(ctr70, hp_vol_con, stDACOutVol.hwVolume + 200);	// 14 bit : HP Driver Volume Control
			else
				CTOP_CTRL_B0_Wr01(ctr70, hp_vol_con, stDACOutVol.hwVolume);	// 14 bit : HP Driver Volume Control

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR70 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr70));
			CTOP_CTRL_B0_WrFL(ctr70);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr70);
			ADEC_MIP_DEBUG("WR : CTR70 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr70));
		}
#endif	//#ifdef ENABLE_HPDRV_PORT

	}
	else
	{
		//Set a SCART DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			/* CTR37 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("RD : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));

			if( stDACOutVol.hwVolume != 0 )
			{
				//do setting after L8 A1 chip
				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetDACOutVolume : Not Support!(DAC0)\n");
				CTOP_CTRL_A0_Wr01(ctr37, aud_lin_drv_ch_mute,	 0b1);		// SCART 0, 1 output un-mute
			}
			else	//mute
			{
				//do setting after L8 A1 chip
				//temp for A0 chip : Mute a SCART 0 output
				CTOP_CTRL_A0_Wr01(ctr37, aud_lin_drv_ch_mute,	 0b0);		// SCART 0, 1 output mute
			}

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr37);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("WR : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));
		}

		//Set a Line-out DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC1 )
		{
			/* CTR37 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("RD : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));

			if( stDACOutVol.hwVolume != 0 )
			{
				//do setting after L8 A1 chip
				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetDACOutVolume : Not Support!(DAC1)\n");
				CTOP_CTRL_A0_Wr01(ctr37, auda_pdb,		0b111);	// DAC1 power-up
			}
			else	//mute
			{
				//do setting after L8 A1 chip
				//temp for A0 chip : Mute a DAC1  output
				CTOP_CTRL_A0_Wr01(ctr37, auda_pdb,		0b011);	// DAC1 power-down
			}

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr37);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("WR : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));
		}

		//Set a HeadPhone DAC volume
		if ( stDACOutVol.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			/* CTR38 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr38);
			ADEC_MIP_DEBUG("RD : CTR38 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr38));
			CTOP_CTRL_A0_RdFL(ctr41);
			ADEC_MIP_DEBUG("RD : CTR41 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr41));

			if( stDACOutVol.hwVolume != 0 )
			{
				//do setting after L8 A1 chip
				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetDACOutVolume : Partial Support!(DAC2)\n");
				CTOP_CTRL_A0_Wr01(ctr38, hp_pdb,	0b1); 						// HP power-up
				//CTOP_CTRL_A0_Wr01(ctr41, dat_hp_vol_con,	i_stDACOutVol.volume);	// HP volume control : not support L8 A0 chip
			}
			else	//mute
			{
				//do setting after L8 A1 chip
				//temp for A0 chip : Mute a SCART 0 output
				CTOP_CTRL_A0_Wr01(ctr38, hp_pdb,		0b0); 		// HP power-down
			}

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr38);
			CTOP_CTRL_A0_WrFL(ctr41);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr38);
			ADEC_MIP_DEBUG("WR : CTR38 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr38));
			CTOP_CTRL_A0_RdFL(ctr41);
			ADEC_MIP_DEBUG("WR : CTR41 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr41));
		}
	}

	ADEC_DEBUG_TMP("MIXED_IP_ADEC_DRV_SetDACOutVolume : Port = %d, Volume = 0x%X\n", stDACOutVol.portNum, stDACOutVol.hwVolume);

	return RET_OK;
}

/**
 * Sets the mute of DAC output.
 * @see ADEC_DRV_SetOutputPortMute
*/
int MIXED_IP_ADEC_L8_SetDACOutMute ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute )
{
	//L8 B0 Support Function : DAC0 -> SCART, DAC1 -> Line Out
	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
	{
		//Set a SCART DAC mute
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			////////////////////////////////////////////////////////////////////////////
			/* CTR65 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr65);
			ADEC_MIP_DEBUG("RD : CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));

			if( stOutPortMute.bHwOnOff == TRUE )
			{
				CTOP_CTRL_B0_Wr01(ctr65, dac0_mute_ena, 0x1);	// 1 bit : Line DAC CH0 Mute Control
			}
			else	//un-mute
			{
				CTOP_CTRL_B0_Wr01(ctr65, dac0_mute_ena, 0x0);	// 1 bit : Line DAC CH0 Mute Control
			}

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR65 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr65));
			CTOP_CTRL_B0_WrFL(ctr65);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr65);
			ADEC_MIP_DEBUG("WR : CTR65 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr65));
		}

#ifdef ENABLE_SCART1_MIC_PORT
		//Set a Line-out DAC mute
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC1 )
		{
			////////////////////////////////////////////////////////////////////////////
			/* CTR67 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr67);
			ADEC_MIP_DEBUG("RD : CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));

			if( stOutPortMute.bHwOnOff == TRUE )
			{
				CTOP_CTRL_B0_Wr01(ctr67, dac1_mute_ena, 0x1);	// 1 bit : Line DAC CH1 Mute Control
			}
			else	//un-mute
			{
				CTOP_CTRL_B0_Wr01(ctr67, dac1_mute_ena, 0x0);	// 1 bit : Line DAC CH1 Mute Control
			}

			/* Write FLushing : Shadow -> Real. */
			ADEC_MIP_DEBUG("     CTR67 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr67));
			CTOP_CTRL_B0_WrFL(ctr67);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr67);
			ADEC_MIP_DEBUG("WR : CTR67 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr67));
		}
#endif	//#ifdef ENABLE_SCART1_MIC_PORT

#ifdef ENABLE_HPDRV_PORT
		//Set a HeadPhone DAC mute
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			if( stOutPortMute.bHwOnOff == TRUE )
			{
				////////////////////////////////////////////////////////////////////////////
				/* CTR71 ==> Read  FLushing : Shadow <- Real. */
				CTOP_CTRL_B0_RdFL(ctr71);
				ADEC_MIP_DEBUG("RD : CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));

				CTOP_CTRL_B0_Wr01(ctr71, hp_mute_ena, 			0b1);	// 1 bit : HP Driver Mute Control

				/* L8 B0 Workaround for HDMI Deep Color Detection Error : hp_pop_dc_on_control */
				//CTOP_CTRL_B0_Wr01(ctr71, hp_pop_dc_on_control, 	0b0);	// 9 bit : HP Driver Soft Starter Start Control

				/* Write FLushing : Shadow -> Real. */
				ADEC_MIP_DEBUG("     CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));
				CTOP_CTRL_B0_WrFL(ctr71);

				/* HPDRV Power-down */
				MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_HPDRV, LX_ADEC_POWER_DOWN);
			}
			else	//un-mute
			{
				/* HPDRV Power-up */
				MIXED_IP_ADEC_L8_PowerControl(LX_ADEC_POWER_HPDRV, LX_ADEC_POWER_UP);

				////////////////////////////////////////////////////////////////////////////
				/* CTR71 ==> Read  FLushing : Shadow <- Real. */
				CTOP_CTRL_B0_RdFL(ctr71);
				ADEC_MIP_DEBUG("RD : CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));

				CTOP_CTRL_B0_Wr01(ctr71, hp_mute_ena, 			0b0);	// 1 bit : HP Driver Mute Control

				/* L8 B0 Workaround for HDMI Deep Color Detection Error : hp_pop_dc_on_control */
				//CTOP_CTRL_B0_Wr01(ctr71, hp_pop_dc_on_control, 	0b1);	// 9 bit : HP Driver Soft Starter Start Control

				/* Write FLushing : Shadow -> Real. */
				ADEC_MIP_DEBUG("     CTR71 : 0x%8X\n", CTOP_CTRL_B0_Rd(ctr71));
				CTOP_CTRL_B0_WrFL(ctr71);
			}

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_B0_RdFL(ctr71);
			ADEC_MIP_DEBUG("WR : CTR71 : 0x%8X\n\n", CTOP_CTRL_B0_Rd(ctr71));
		}
#endif	//#ifdef ENABLE_HPDRV_PORT

	}
	else
	{
		//Set a SCART DAC mute
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC0 )
		{
			/* CTR37 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("RD : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));

			if( stOutPortMute.bHwOnOff != TRUE )
			{
				//do setting after L8 A1 chip
				CTOP_CTRL_A0_Wr01(ctr37, aud_lin_drv_ch_mute,	 0b1);		// SCART 0, 1 output un-mute
			}
			else	//mute
			{
				//do setting after L8 A1 chip
				//temp for A0 chip : Mute a SCART 0 output
				CTOP_CTRL_A0_Wr01(ctr37, aud_lin_drv_ch_mute,	 0b0);		// SCART 0, 1 output mute
			}

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr37);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("WR : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));
		}

		//Set a Line-out DAC mute
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC1 )
		{
			/* CTR37 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("RD : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));

			if( stOutPortMute.bHwOnOff != TRUE )
			{
				//do setting after L8 A1 chip
				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetDACOutVolume : Not Support!(DAC1)\n");
				CTOP_CTRL_A0_Wr01(ctr37, auda_pdb,		0b111);		// DAC1 power-up
			}
			else	//mute
			{
				//do setting after L8 A1 chip
				//temp for A0 chip : Mute a DAC 1 output
				CTOP_CTRL_A0_Wr01(ctr37, auda_pdb,		0b011);		// DAC1 power-down
			}

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr37);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr37);
			ADEC_MIP_DEBUG("WR : CTR37 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr37));
		}

		//Set a HeadPhone DAC mute
		if ( stOutPortMute.portNum & LX_ADEC_OUTPUT_DAC2 )
		{
			/* CTR38 ==> Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr38);
			ADEC_MIP_DEBUG("RD : CTR38 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr38));

			if( stOutPortMute.bHwOnOff != TRUE )
			{
				//do setting after L8 A1 chip
				ADEC_MIP_DEBUG("MIXED_IP_ADEC_DRV_SetDACOutVolume : Partial Support!(DAC2)\n");
				CTOP_CTRL_A0_Wr01(ctr38, hp_pdb,		0b1);		// HP power-up
			}
			else	//mute
			{
				//do setting after L8 A1 chip
				//temp for A0 chip : Mute a SCART 0 output
				CTOP_CTRL_A0_Wr01(ctr38, hp_pdb,		0b0); 		// HP power-down
			}

			/* Write FLushing : Shadow -> Real. */
			CTOP_CTRL_A0_WrFL(ctr38);

			/* Read  FLushing : Shadow <- Real. */
			CTOP_CTRL_A0_RdFL(ctr38);
			ADEC_MIP_DEBUG("WR : CTR38 : 0x%X\n", CTOP_CTRL_A0_Rd(ctr38));
		}
	}

	ADEC_DEBUG_TMP("MIXED_IP_ADEC_DRV_SetDACOutMute : Port = %d, Mute = 0x%X\n", stOutPortMute.portNum, stOutPortMute.bHwOnOff);

	return RET_OK;
}


/** @} */




