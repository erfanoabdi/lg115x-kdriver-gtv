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

/*! \file HDMI_module.c
 * \brief HDMI module control Drivers linux module
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
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <asm/io.h>			/**< For ioremap_nocache */
#include <linux/workqueue.h>		/**< For working queue */
#include <linux/interrupt.h>

#include "os_util.h"

#include "hdmi_drv.h"
#include "hdmi_kapi.h"
#include "hdmi_module.h"
//#include "hdmi_phy_reg.h"
#include "vport_reg.h"

/******************************************************************************
 *				DEFINES
 *****************************************************************************/


/**
 *	Global variables of the driver
 */
extern LX_HDMI_REG_T *pHdmiRegCfg;
/******************************************************************************
 *				DATA STRUCTURES
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions
 */

/* for HDMI Switch */
HDMI_DATA_T gCfgBootData[] =
{
	// for LG1150(L8) A0 & B0
	{
	    1,			// hdmi_switch
	    0,			// SWport
	    {0, 0, 1},	// LX_HDMI_HPD_T

	    0,			// mode
	    0,			// port
	    0,			// Pixel Repetition
		{4,	3},		// connectCn[2]
	    {21, 7},	// timingCn[2]
	    {20, 4},	// hdmiCn[2]
	    {3,	1}		// dviCn[2]
	 },
	 // for LG1151(L9) A0/1 & B0
	 {
	 	1,			// hdmi_switch
	    0,			// SWport
	    {0, 0, 1},	// LX_HDMI_HPD_T

	    0,			// mode
	    0,			// port
	    0,			// Pixel Repetition
		{4,	4},		// connectCn[2]
	    {21, 8},	// timingCn[2]
	    {20, 4},	// hdmiCn[2]
	    {3,	2}		// dviCn[2]
	  }
};

HDMI_DATA_T *gBootData = &gCfgBootData[0];	// for Real.

/******************************************************************************
 *				Local function
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions
 */

int (*HDMI_GetMode)(LX_HDMI_MODE_T *mode);
int (*HDMI_GetAspectRatio)(LX_HDMI_ASPECTRATIO_T *ratio);
int (*HDMI_GetTimingInfo)(LX_HDMI_TIMING_INFO_T *info);
int (*HDMI_GetStatus)(LX_HDMI_STATUS_T *status);
int (*HDMI_GetVsiPacket)(LX_HDMI_VSI_PACKET_T *packet);
int (*HDMI_GetAviPacket)(LX_HDMI_AVI_PACKET_T *packet);
int (*HDMI_GetSpdPacket)(LX_HDMI_SPD_PACKET_T *packet);
int (*HDMI_SetPort)(LX_HDMI_HPD_T *hpd);
int (*HDMI_SetPowerControl)(HDMI_PHY_TYPE_T type , int power);
int (*HDMI_SetHPDEnDisable)(LX_HDMI_HPD_T *hpd);
int (*HDMI_SetAudio)(void);
int (*HDMI_GetAudioInfo)(LX_HDMI_AUDIO_INFO_T *audio_info);
int (*HDMI_SetMute)(LX_HDMI_MUTE_CTRL_T *mute);
int (*HDMI_SetRGBYCbCr)(LX_HDMI_AVI_COLORSPACE_T *space);
int (*HDMI_SetArc)(LX_HDMI_ARC_CTRL_T *arc);
int (*HDMI_GetRegister)(UINT32 addr , UINT32 *value);
int (*HDMI_SetRegister)(UINT32 addr , UINT32 value);
void (*HDMI_SetPixelRepetition)(void);
void (*HDMI_SetAutoSyncPolarity)(void);
int (*HDMI_RunReset)(void);
int (*HDMI_HWInitial)(void);
irqreturn_t (*HDMI_IRQHandler)(int irq, void *dev);
void (*HDMI_EnableInterrupt)(void);
int (*HDMI_GetPortConnection)(void);
int (*HDMI_PowerConsumption)(LX_HDMI_POWER_CTRL_T *power);
int (*HDMI_SetUDn3DControl)(LX_HDMI_UD_3D_MODE_T *udn3d);

#ifdef	KDRV_CONFIG_PM
int (*HDMI_RunSuspend)(void);
int (*HDMI_RunResume)(void);
#endif


/**
* ADC Module exit
*
* @parm void
* @return int
*/
int HDMI_exit(void)
{
	int ret = 0;

	// power down
	HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);

//	iounmap(pstHdmiPhyReg);
	return ret;
}


/**
* HDMI Module Chip revision converter
*
* @parm void
* @return int
*/

int HDMI_InitChipRev(void)
{
	if ( lx_chip_rev() >= LX_CHIP_REV(L9, B0) )
	{
		HDMI_GetMode =         	HDMI_L9Bx_GetMode;
		HDMI_GetAspectRatio=   	HDMI_L9Bx_GetAspectRatio;
		HDMI_GetTimingInfo=    	HDMI_L9Bx_GetTimingInfo;
		HDMI_GetStatus=        	HDMI_L9Bx_GetStatus;
		HDMI_GetVsiPacket=     	HDMI_L9Bx_GetVsiPacket;
		HDMI_GetAviPacket=     	HDMI_L9Bx_GetAviPacket;
		HDMI_GetSpdPacket=     	HDMI_L9Bx_GetSpdPacket;
		HDMI_SetPort=          	HDMI_L9Bx_SetPort;
		HDMI_SetPowerControl=  	HDMI_L9Bx_SetPowerControl;
		HDMI_SetHPDEnDisable=  	HDMI_L9Bx_SetHPDEnDisable;
		HDMI_SetAudio=         	HDMI_L9Bx_SetAudio;
		HDMI_GetAudioInfo=     	HDMI_L9Bx_GetAudioInfo;
		HDMI_SetMute=         	HDMI_L9Bx_SetMute;
		HDMI_SetArc=         	HDMI_L9Bx_SetArc;
		HDMI_SetRGBYCbCr=     	HDMI_L9Bx_SetRGBYCbCr;
		HDMI_GetRegister=      	HDMI_L9Bx_GetRegister;
		HDMI_SetRegister=      	HDMI_L9Bx_SetRegister;
		HDMI_SetPixelRepetition= 	HDMI_L9Bx_SetPixelRepetition;
		HDMI_SetAutoSyncPolarity=	HDMI_L9Bx_SetAutoSyncPolarity;
		HDMI_RunReset=			HDMI_L9Bx_RunReset;
		HDMI_HWInitial=			HDMI_L9Bx_HWInitial;
		HDMI_IRQHandler = 		HDMI_L9Bx_IRQHandler;
		HDMI_EnableInterrupt= 	HDMI_L9Bx_EnableInterrupt;
		HDMI_GetPortConnection= HDMI_L9Bx_GetPortConnection;
		HDMI_PowerConsumption=  HDMI_L9Bx_PowerConsumption;
		HDMI_SetUDn3DControl=	HDMI_L9Bx_SetUDn3DControl;

	#ifdef	KDRV_CONFIG_PM
		HDMI_RunSuspend=      	HDMI_L9Bx_RunSuspend;
		HDMI_RunResume=       	HDMI_L9Bx_RunResume;
	#endif
	}
	else if ( lx_chip_rev() == LX_CHIP_REV(L9, A1) || lx_chip_rev() == LX_CHIP_REV(L9, A0) )
	{
		HDMI_GetMode =         	HDMI_L9Ax_GetMode;
		HDMI_GetAspectRatio=   	HDMI_L9Ax_GetAspectRatio;
		HDMI_GetTimingInfo=    	HDMI_L9Ax_GetTimingInfo;
		HDMI_GetStatus=        	HDMI_L9Ax_GetStatus;
		HDMI_GetVsiPacket=     	HDMI_L9Ax_GetVsiPacket;
		HDMI_GetAviPacket=     	HDMI_L9Ax_GetAviPacket;
		HDMI_GetSpdPacket=     	HDMI_L9Ax_GetSpdPacket;
		HDMI_SetPort=          	HDMI_L9Ax_SetPort;
		HDMI_SetPowerControl=  	HDMI_L9Ax_SetPowerControl;
		HDMI_SetHPDEnDisable=  	HDMI_L9Ax_SetHPDEnDisable;
		HDMI_SetAudio=         	HDMI_L9Ax_SetAudio;
		HDMI_GetAudioInfo=     	HDMI_L9Ax_GetAudioInfo;
		HDMI_SetMute=         	HDMI_L9Ax_SetMute;
		HDMI_SetArc=         	HDMI_L9Ax_SetArc;
		HDMI_SetRGBYCbCr=     	HDMI_L9Ax_SetRGBYCbCr;
		HDMI_GetRegister=      	HDMI_L9Ax_GetRegister;
		HDMI_SetRegister=      	HDMI_L9Ax_SetRegister;
		HDMI_SetPixelRepetition= 	HDMI_L9Ax_SetPixelRepetition;
		HDMI_SetAutoSyncPolarity=	HDMI_L9Ax_SetAutoSyncPolarity;
		HDMI_RunReset=			HDMI_L9Ax_RunReset;
		HDMI_HWInitial=			HDMI_L9Ax_HWInitial;
		HDMI_IRQHandler = 		HDMI_L9Ax_IRQHandler;
		HDMI_EnableInterrupt= 	HDMI_L9Ax_EnableInterrupt;
		HDMI_GetPortConnection= HDMI_L9Ax_GetPortConnection;
		HDMI_PowerConsumption=  HDMI_L9Ax_PowerConsumption;
		HDMI_SetUDn3DControl=	HDMI_L9Ax_SetUDn3DControl;

		#ifdef	KDRV_CONFIG_PM
		HDMI_RunSuspend=      	HDMI_L9Ax_RunSuspend;
		HDMI_RunResume=       	HDMI_L9Ax_RunResume;
		#endif
	}
#ifdef INCLUDE_L8_CHIP_KDRV
	else if ( lx_chip_rev() == LX_CHIP_REV(L8, A1) || lx_chip_rev() == LX_CHIP_REV(L8, A0))
	{
		HDMI_GetMode =         	HDMI_L8Ax_GetMode;
		HDMI_GetAspectRatio=   	HDMI_L8Ax_GetAspectRatio;
		HDMI_GetTimingInfo=    	HDMI_L8Ax_GetTimingInfo;
		HDMI_GetStatus=        	HDMI_L8Ax_GetStatus;
		HDMI_GetVsiPacket=     	HDMI_L8Ax_GetVsiPacket;
		HDMI_GetAviPacket=     	HDMI_L8Ax_GetAviPacket;
		HDMI_GetSpdPacket=     	HDMI_L8Ax_GetSpdPacket;
		HDMI_SetPort=          	HDMI_L8Ax_SetPort;
		HDMI_SetPowerControl=  	HDMI_L8Ax_SetPowerControl;
		HDMI_SetHPDEnDisable=  	HDMI_L8Ax_SetHPDEnDisable;
		HDMI_SetAudio=         	HDMI_L8Ax_SetAudio;
		HDMI_GetAudioInfo=     	HDMI_L8Ax_GetAudioInfo;
		HDMI_SetMute=         	HDMI_L8Ax_SetMute;
		HDMI_SetArc=         	HDMI_L8Ax_SetArc;
		HDMI_SetRGBYCbCr=     	HDMI_L8Ax_SetRGBYCbCr;
		HDMI_GetRegister=      	HDMI_L8Ax_GetRegister;
		HDMI_SetRegister=      	HDMI_L8Ax_SetRegister;
		HDMI_SetPixelRepetition= 	HDMI_L8Ax_SetPixelRepetition;
		HDMI_SetAutoSyncPolarity=	HDMI_L8Ax_SetAutoSyncPolarity;
		HDMI_RunReset=			HDMI_L8Ax_RunReset;
		HDMI_HWInitial=			HDMI_L8Ax_HWInitial;
		HDMI_IRQHandler = 		HDMI_L8Ax_IRQHandler;
		HDMI_EnableInterrupt= 	HDMI_L8Ax_EnableInterrupt;
		HDMI_GetPortConnection= HDMI_L8Ax_GetPortConnection;
		HDMI_PowerConsumption=  HDMI_L8Ax_PowerConsumption;
		HDMI_SetUDn3DControl=	NULL;

		#ifdef	KDRV_CONFIG_PM

		HDMI_RunSuspend=      	HDMI_L8Ax_RunSuspend;
		HDMI_RunResume=       	HDMI_L8Ax_RunResume;
		#endif
	}
	else if ( lx_chip_rev( ) == LX_CHIP_REV(L8, B0) )
	{
		HDMI_GetMode =         	HDMI_L8Bx_GetMode;
		HDMI_GetAspectRatio=   	HDMI_L8Bx_GetAspectRatio;
		HDMI_GetTimingInfo=    	HDMI_L8Bx_GetTimingInfo;
		HDMI_GetStatus=        	HDMI_L8Bx_GetStatus;
		HDMI_GetVsiPacket=     	HDMI_L8Bx_GetVsiPacket;
		HDMI_GetAviPacket=     	HDMI_L8Bx_GetAviPacket;
		HDMI_GetSpdPacket=     	HDMI_L8Bx_GetSpdPacket;
		HDMI_SetPort=          	HDMI_L8Bx_SetPort;
		HDMI_SetPowerControl=  	HDMI_L8Bx_SetPowerControl;
		HDMI_SetHPDEnDisable=  	HDMI_L8Bx_SetHPDEnDisable;
		HDMI_SetAudio=         	HDMI_L8Bx_SetAudio;
		HDMI_GetAudioInfo=     	HDMI_L8Bx_GetAudioInfo;
		HDMI_SetMute=         	HDMI_L8Bx_SetMute;
		HDMI_SetArc=         	HDMI_L8Bx_SetArc;
		HDMI_SetRGBYCbCr=     	HDMI_L8Bx_SetRGBYCbCr;
		HDMI_GetRegister=      	HDMI_L8Bx_GetRegister;
		HDMI_SetRegister=      	HDMI_L8Bx_SetRegister;
		HDMI_SetPixelRepetition= 	HDMI_L8Bx_SetPixelRepetition;
		HDMI_SetAutoSyncPolarity=	HDMI_L8Bx_SetAutoSyncPolarity;
		HDMI_RunReset=			HDMI_L8Bx_RunReset;
		HDMI_HWInitial=			HDMI_L8Bx_HWInitial;
		HDMI_IRQHandler = 		HDMI_L8Bx_IRQHandler;
		HDMI_EnableInterrupt= 	HDMI_L8Bx_EnableInterrupt;
		HDMI_GetPortConnection= HDMI_L8Bx_GetPortConnection;
		HDMI_PowerConsumption=  HDMI_L8Bx_PowerConsumption;
		HDMI_SetUDn3DControl=	NULL;

		#ifdef	KDRV_CONFIG_PM

		HDMI_RunSuspend=      	HDMI_L8Bx_RunSuspend;
		HDMI_RunResume=       	HDMI_L8Bx_RunResume;
		#endif
	}
#endif 	//#ifdef INCLUDE_L8_CHIP_KDRV
	else
	{
		return -1;
	}

	return 0;
}
/**
* HDMI Module initialize
*
* @parm void
* @return int
*/
int HDMI_Initialize(LX_HDMI_INIT_T *param)
{
	int ret = 0;

	//HDMI_ERROR("HDMI initializ = %d \n", param->bHdmiSW);
	if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))			gBootData = &gCfgBootData[1];	// 1 : L9 Ax & Bx for Real
	else if (lx_chip_rev() >= LX_CHIP_REV(L8, A0))		gBootData = &gCfgBootData[0];	// 0 : L8 A1 & B0 for Real.
	else return -1;	// Unkown chip revision

#if 0
	/* access to register */
	if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))			pHdmiRegCfg = &gRegCfgHdmi[1];	// 1 : L9 Ax & Bx for Real
	else if (lx_chip_rev() >= LX_CHIP_REV(L8, A0))		pHdmiRegCfg = &gRegCfgHdmi[0];	// 0 : L8 A1 & B0 for Real.
	else return -1;	// Unkown chip revision

	HDMI_PRINT("HDMI REG CFG \n");
	HDMI_PRINT("HDMI REG CFG vport base 0x%08x\n", pHdmiRegCfg->vport_reg_base_addr);
	HDMI_PRINT("HDMI REG CFG vport size 0x%08x\n", pHdmiRegCfg->vport_reg_size);
	HDMI_PRINT("HDMI REG CFG vport Qumu 0x%08x\n", pHdmiRegCfg->vport_qemu_base_addr);

	HDMI_ERROR("HDMI REG CFG PHy base 0x%08x\n", pHdmiRegCfg->phy_reg_base_addr);
	HDMI_ERROR("HDMI REG CFG Phy size 0x%08x\n", pHdmiRegCfg->phy_reg_size);
	HDMI_ERROR("HDMI REG CFG Phy Qumu 0x%08x\n", pHdmiRegCfg->phy_qemu_base_addr);
#endif

	// for HDMI Switch H/W
	gBootData->hdmi_switch = param->bHdmiSW;

	HDMI_InitChipRev();

	// initialize
	HDMI_HWInitial();

	HDMI_EnableInterrupt();

	HDMI_PRINT("HDMI initialize \n" );

	// power down
	HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);

	if(gBootData->hdmi_switch)
	{
		gBootData->SWport = param->u8HWport;
		HDMI_SetPowerControl(0 , 1);
		HDMI_SetHPDEnDisable(&gBootData->hpd);
	}

	return ret;
}



