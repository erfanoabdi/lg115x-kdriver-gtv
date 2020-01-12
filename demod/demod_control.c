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

/*! \file ADC_control.c
 * \brief ADC control Drivers linux module
 */


/**
 *	Necessary includes for device drivers
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>	/**< printk() */
#include <linux/slab.h> 	/**< kmalloc() */
#include <linux/fs.h> 		/**< everything\ldots{} */
#include <linux/types.h> 	/**< size_t */
#include <linux/fcntl.h>	/**< O_ACCMODE */
#include <asm/uaccess.h>

#include <linux/delay.h>	/**< mdelay */

#include "demod_drv.h"
#include "os_util.h"
#include "demod_drv.h"

#include "demod_kapi.h"
#include "ctop_regs.h"
#include "demod_module.h"

#include "i2c_core.h"

/******************************************************************************
 *				DEFINES
 *****************************************************************************/
#define L9_CTOP_DEMOD_I2C_ADDR		0x12
#define L9_CTOP_DEMOD_I2C_RETRY		3

#define DEMOD_L9_A0

/**
 *	Global variables of the driver
 */

#ifdef KDRV_CONFIG_PM
unsigned int guiPmClk;
unsigned int guiPmBbmode;
unsigned int guiPmI2C;
#endif


/******************************************************************************
 *				DATA STRUCTURES
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions
 */


/******************************************************************************
 *				Local function
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions
 */



/**
 *
 * Hardware reset
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/


int DEMOD_ResetHW(void)
{
	if(lx_chip_rev() < LX_CHIP_REV(L8, B0))

	{

		 CTOP_CTRL_A0_RdFL(ctr61);

		 CTOP_CTRL_A0_Wr01(ctr61, demod_reset_sel, 1);

		 CTOP_CTRL_A0_WrFL(ctr61);

		 OS_MsecSleep(10);

		CTOP_CTRL_A0_Wr01(ctr61, demod_reset_sel, 0);

		CTOP_CTRL_A0_WrFL(ctr61);

		 OS_MsecSleep(20);

		CTOP_CTRL_A0_Wr01(ctr61, demod_reset_sel, 1);

		CTOP_CTRL_A0_WrFL(ctr61);

	}

	else if(lx_chip_rev() == LX_CHIP_REV(L8, B0))

	{

		 CTOP_CTRL_B0_RdFL(ctr61);

		 CTOP_CTRL_B0_Wr01(ctr61, demod_reset_sel, 1);

		 CTOP_CTRL_B0_WrFL(ctr61);

		 OS_MsecSleep(10);

		CTOP_CTRL_B0_Wr01(ctr61, demod_reset_sel, 0);

		CTOP_CTRL_B0_WrFL(ctr61);

		 OS_MsecSleep(20);

		CTOP_CTRL_B0_Wr01(ctr61, demod_reset_sel, 1);

		CTOP_CTRL_B0_WrFL(ctr61);

	}

#ifdef DEMOD_L9_A0
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
	{

		/* do nothing ... */
	}
#endif
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{

		ACTRL_RdFL(ggbb_0);

		ACTRL_Wr01(ggbb_0, reset, 1);

		ACTRL_WrFL(ggbb_0);

		OS_MsecSleep(10);


		ACTRL_Wr01(ggbb_0, reset, 0);

		ACTRL_WrFL(ggbb_0);


		 OS_MsecSleep(20);


		 ACTRL_Wr01(ggbb_0, reset, 1);

		 ACTRL_WrFL(ggbb_0);

	}

	else if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{

		ACTRL_RdFL(ggbb_0_bx);

		ACTRL_Wr01(ggbb_0_bx, reset, 1);

		ACTRL_WrFL(ggbb_0_bx);

		OS_MsecSleep(10);


		ACTRL_Wr01(ggbb_0_bx, reset, 0);

		ACTRL_WrFL(ggbb_0_bx);


		 OS_MsecSleep(20);


		 ACTRL_Wr01(ggbb_0_bx, reset, 1);

		 ACTRL_WrFL(ggbb_0_bx);

	}
	return 0;
}


/**
 *
 * select normal mode
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/
int DEMOD_SetI2C(void)
{
	if(lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{
		CTOP_CTRL_RdFL(ctr26);

		CTOP_CTRL_Wr01(ctr26, i2c_sel, 0);

		CTOP_CTRL_WrFL(ctr26);
	}
#ifdef DEMOD_L9_A0
		else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
		{

			/* do nothing ... */
		}
#endif
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{

		ACTRL_RdFL(ggbb_0);

		ACTRL_Wr01(ggbb_0, i2csel, 0);

		ACTRL_WrFL(ggbb_0);
	}
	else if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{

		ACTRL_RdFL(ggbb_0_bx);

		ACTRL_Wr01(ggbb_0_bx, i2csel, 0);

		ACTRL_WrFL(ggbb_0_bx);
	}

	return 0;
}

/**
 *
 * select inclk mode
 *
 * @param	LX_DEMOD_INCLK_SEL_T
 * @return	0 : OK , -1 : NOK
 *
*/
int DEMOD_SetInclk(LX_DEMOD_INCLK_SEL_T *inclk)
{

	if(lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{

		CTOP_CTRL_RdFL(ctr35);

		switch(*inclk)
		{
			case LX_DEMOD_ARM_SETTING:
			{

				CTOP_CTRL_Wr01(ctr35, vsb_inck_sel, 0);
			}
			break;

			case LX_DEMOD_EXTERN_PIN:
				{

					CTOP_CTRL_Wr01(ctr35, vsb_inck_sel, 1);
				}
			break;

			default:
				DEMOD_PRINT("Invalid LX_DEMOD_INCLK_SEL_T type\n");
				return -1;
		}

		CTOP_CTRL_WrFL(ctr35);

	}
#ifdef DEMOD_L9_A0
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
	{

		/* do nothing ... */
	}
#endif
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{
		ACTRL_RdFL(ggbb_afe_0);

		switch(*inclk)
		{
			case LX_DEMOD_ARM_SETTING:
			{
				ACTRL_Wr01(ggbb_afe_0, inck_sel, 0);
			}
			break;

			case LX_DEMOD_EXTERN_PIN:
			{

				ACTRL_Wr01(ggbb_afe_0, inck_sel, 1);
			}
			break;

			default:
				DEMOD_PRINT("Invalid LX_DEMOD_INCLK_SEL_T type\n");
				return -1;
		}

		ACTRL_WrFL(ggbb_afe_0);


	}
	else if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		ACTRL_RdFL(ggbb_afe_0_bx);

		switch(*inclk)
		{
			case LX_DEMOD_ARM_SETTING:
			{
				ACTRL_Wr01(ggbb_afe_0, inck_sel, 0);
			}
			break;

			case LX_DEMOD_EXTERN_PIN:
			{

				ACTRL_Wr01(ggbb_afe_0_bx, inck_sel, 1);
			}
			break;

			default:
				DEMOD_PRINT("Invalid LX_DEMOD_INCLK_SEL_T type\n");
				return -1;
		}

		ACTRL_WrFL(ggbb_afe_0_bx);


	}

	return 0;
}

/**
 *
 * select bbmode
 *
 * @param	LX_DEMOD_INCLK_SEL_T
 * @return	0 : OK , -1 : NOK
 *
*/
int DEMOD_SetBbmod(LX_DEMOD_BBMODE_T *bbmode)
{
	if(lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{

		CTOP_CTRL_RdFL(ctr61);

		switch(*bbmode)
		{
			case LX_DEMOD_UNKOWN:
			{

				CTOP_CTRL_Wr01(ctr61, bbmode_sel, 0);

			}
			break;

			case LX_DEMOD_VSB_QAM:
			{

				CTOP_CTRL_Wr01(ctr61, bbmode_sel, 1);

			}
			break;
			case LX_DEMOD_DVB_T:
			{

				CTOP_CTRL_Wr01(ctr61, bbmode_sel, 2);

			}
			break;
			case LX_DEMOD_DVB_C:
			{

				CTOP_CTRL_Wr01(ctr61, bbmode_sel, 3);

			}
			break;
			default:
				DEMOD_PRINT("Invalid LX_DEMOD_BBMODE_T type\n");
				return -1;
		}

		CTOP_CTRL_WrFL(ctr61);
	}

#ifdef DEMOD_L9_A0
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
	{

		/* do nothing ... */
	}
#endif
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{

		ACTRL_RdFL(ggbb_0);

		switch(*bbmode)
		{
			case LX_DEMOD_UNKOWN:
			{

				ACTRL_Wr01(ggbb_0, bbmode, 0);

			}
			break;

			case LX_DEMOD_VSB_QAM:
			{

				ACTRL_Wr01(ggbb_0, bbmode, 1);

			}
			break;
			case LX_DEMOD_DVB_T:
			{

				ACTRL_Wr01(ggbb_0, bbmode, 2);

			}
			break;
			case LX_DEMOD_DVB_C:
			{

				ACTRL_Wr01(ggbb_0, bbmode, 3);

			}
			break;
			default:
				DEMOD_PRINT("Invalid LX_DEMOD_BBMODE_T type\n");
				return -1;
		}

		ACTRL_WrFL(ggbb_0);
	}
	else if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		/* do nothing ... */

	}
	return 0;
}

/**
 *
 * select tuner i2c  mode
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/
int DEMOD_SetTunerI2C(LX_DEMOD_TUNER_I2C_SEL_T *tunerI2C)
{
	if(lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{
		/* nothing to do */
	}
#ifdef DEMOD_L9_A0
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
	{

		/* do nothing ... */
	}
#endif
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{

		ACTRL_RdFL(ggbb_0);

		switch( *tunerI2C)
		{
			case LX_DEMOD_CPU :

				ACTRL_Wr01(ggbb_0, tun_i2c_sel, 0);
				break;

			case LX_DEMOD_GBB :
				ACTRL_Wr01(ggbb_0, tun_i2c_sel, 1);
				break;

			default:
				DEMOD_PRINT("Invalid LX_DEMOD_TUNER_I2C_SEL_T type\n");
				return -1;
		}
		ACTRL_WrFL(ggbb_0);

	}
	else if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		/* do nothing ... */

	}

	return 0;
}



#ifdef KDRV_CONFIG_PM
int DEMOD_HW_suspend(void)
{
	if(lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{

		CTOP_CTRL_RdFL(ctr35);
		CTOP_CTRL_RdFL(ctr61);
		CTOP_CTRL_RdFL(ctr26);

		CTOP_CTRL_Rd01(ctr35, vsb_inck_sel, guiPmClk);
		CTOP_CTRL_Rd01(ctr61, bbmode_sel, guiPmBbmode);
		CTOP_CTRL_Rd01(ctr26, i2c_sel, guiPmI2C);
	}
#ifdef DEMOD_L9_A0
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
	{

		/* do nothing ... */
	}
#endif
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{
		ACTRL_RdFL(ggbb_afe_0);
		ACTRL_RdFL(ggbb_0);

		ACTRL_Rd01(ggbb_afe_0, inck_sel, guiPmClk);
		ACTRL_Rd02(ggbb_0, bbmode, guiPmBbmode,i2csel, guiPmI2C);

	}
	else if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		ACTRL_RdFL(ggbb_afe_0_bx);
		ACTRL_RdFL(ggbb_0_bx);

		ACTRL_Rd01(ggbb_afe_0_bx, inck_sel, guiPmClk);
		ACTRL_Rd01(ggbb_0_bx, i2csel, guiPmI2C);

	}
	return 0;
}

int DEMOD_HW_resume(void)
{

	if(lx_chip_rev() <= LX_CHIP_REV(L8, B0))
	{
		CTOP_CTRL_RdFL(ctr35);
		CTOP_CTRL_RdFL(ctr61);
		CTOP_CTRL_RdFL(ctr26);

		CTOP_CTRL_Wr01(ctr35, vsb_inck_sel, guiPmClk);
		CTOP_CTRL_Wr01(ctr61, bbmode_sel, guiPmBbmode);
		CTOP_CTRL_Wr01(ctr26, i2c_sel, guiPmI2C);

		CTOP_CTRL_WrFL(ctr35);
		CTOP_CTRL_WrFL(ctr61);
		CTOP_CTRL_WrFL(ctr26);
	}
#ifdef DEMOD_L9_A0
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A0))
	{

		/* do nothing ... */
	}
#endif
	else if(lx_chip_rev() == LX_CHIP_REV(L9, A1))
	{

		ACTRL_RdFL(ggbb_afe_0);
		ACTRL_RdFL(ggbb_0);


		ACTRL_Wr01(ggbb_afe_0, inck_sel, guiPmClk);
		ACTRL_Wr02(ggbb_0, bbmode, guiPmBbmode,i2csel, guiPmI2C);

		ACTRL_WrFL(ggbb_afe_0);
		ACTRL_WrFL(ggbb_0);

	}
	else if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		ACTRL_RdFL(ggbb_afe_0_bx);
		ACTRL_RdFL(ggbb_0_bx);


		ACTRL_Wr01(ggbb_afe_0_bx, inck_sel, guiPmClk);
		ACTRL_Wr01(ggbb_0_bx, i2csel, guiPmI2C);

		ACTRL_WrFL(ggbb_afe_0_bx);
		ACTRL_WrFL(ggbb_0_bx);

	}
	return 0;
}
#endif


