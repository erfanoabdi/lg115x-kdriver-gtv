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

 /*! \file HDMI_control.c
 * \brief HDMI H/W control Drivers linux module
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
#include <linux/delay.h>
#include <asm/div64.h> //do_div

#include "os_util.h"

#include "hdmi_drv.h"
#include "hdmi_kapi.h"
#include "hdmi_module.h"
#include "ctop_regs.h"

/**< DE WinBlank Function */
#if 0
#include "de_kapi.h"
#include "../de/de_ipc_def.h"
#include "../de/de_def.h"
#include "../de/de_int_def.h"
#include "../de/de_hal_def.h"
#include "../de/l8/de_reg_def_l8.h"
#include "../de/l8/de_reg_l8.h"
#endif

/******************************************************************************
 *				DEFINES
 *****************************************************************************/
#define HDMI_L9B0

//#define HDMI_DEBUG_ON
//#define HDMI_DEBUG_PRINT_ON
//#define ENABLE_HDMI_AUDIO_DEBUG

// L9A A0 Issue
#define L9A_A0_ARC_PDB

/**
 *	Global variables of the driver
 */
extern HDMI_DATA_T *gBootData;

static UINT8 _gSCDT = 0;
static UINT32 _gcount = 0;
static UINT8 _gMuteCount = 0;
static UINT8 _gSourceMute = 0;
static UINT8 _gHDMI_CSC_Mute = 0;
static UINT8 _gTimingStatus = 0;

static LX_HDMI_AVI_COLORSPACE_T	_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
static LX_HDMI_AVI_COLORSPACE_T	_gCurPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
static LX_HDMI_TIMING_INFO_T _gPrevTiming = {0,};
static UINT8 _gHDMI_port_changed = 0;
static UINT8 _gHDMI_Change = 0;
static UINT8 _gHDMI_DVI_Done = 0;

//HDMI Audio Variables
static LX_HDMI_AUDIO_INFO_T _gHDMIAudioInfo = {LX_HDMI_MAIN_WINDOW, LX_HDMI_AUDIO_DEFAULT,	\
										   	   LX_HDMI_SAMPLING_FREQ_NONE, FALSE, FALSE};

//static UINT32 _gCurHDMIPort = 0;

//static UINT8 _gHDMI_Mute = 0;

#ifdef HDMI_DEBUG_ON
static UINT32 _gPrevRGB = 0;
static UINT32 _gPrevInt_plug = 0;
static UINT32 _gPrevAVI = 0;
#endif

#ifdef	KDRV_CONFIG_PM
static unsigned int guiPmInterruptStatus;
static unsigned int guiPmHpdStatus;
static LX_HDMI_HPD_T gstPmPortSel;
#endif

typedef enum
{
	HDMI_L9AX_PORT_0,
	HDMI_L9AX_PORT_1,
	HDMI_L9AX_PORT_MAX,
} HDMI_L9AX_PORT_T;

static UINT32 _gui32CurPort = 0;


static int _HDMI_L9Ax_Set3DFullTimingInfo(LX_HDMI_TIMING_INFO_T *info);
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
static void	_HDMI_L9Ax_ClearSWResetAll(void)
{
	VPORT_I2C_Read((UINT32*)&gsoft_reset_1);
	VPORT_I2C_Read((UINT32*)&gsoft_reset_2);
	VPORT_I2C_Read((UINT32*)&gsoft_reset_3);
	VPORT_I2C_Read((UINT32*)&gsoft_reset_5);
	VPORT_I2C_Read((UINT32*)&gsoft_reset_8);	//HDMI Audio
	VPORT_I2C_Read((UINT32*)&gsoft_reset_6);	//for UD & 3D format

	gsoft_reset_1.swrst_link_prt1_tmds = 0;
	gsoft_reset_1.swrst_link_prt0_tmds = 0;
	gsoft_reset_1.swrst_hdcp1 = 0;
	gsoft_reset_1.swrst_hdcp0 = 0;
	gsoft_reset_1.swrst_f3m = 0;

	gsoft_reset_2.swrst_link_pip0_pix = 0;
	gsoft_reset_2.swrst_aw1 = 0;
	gsoft_reset_2.swrst_vw1 = 0;
	gsoft_reset_2.swrst_link_pip1_tmds = 0;
	gsoft_reset_2.swrst_aw0	= 0;
	gsoft_reset_2.swrst_vw0	= 0;
    gsoft_reset_2.swrst_link_pip0_tmds = 0;

	gsoft_reset_3.swrst_ar1 = 0;
	gsoft_reset_3.swrst_link_pip1_aud = 0;
	gsoft_reset_3.swrst_ar0 = 0;
	gsoft_reset_3.swrst_link_pip0_aud = 0;
	gsoft_reset_3.swrst_vr1 = 0;
	gsoft_reset_3.swrst_link_pip1_pix = 0;
    gsoft_reset_3.swrst_vr0 = 0;

	gsoft_reset_5.swrst_afe3ch_cvbs = 0;
	gsoft_reset_5.swrst_chb = 0;
	gsoft_reset_5.swrst_cvbs = 0;
	gsoft_reset_5.swrst_f54m = 0;
	gsoft_reset_5.swrst_vdac = 0;

	gsoft_reset_8.swrst_hdmi_dto0 = 0;
	gsoft_reset_8.swrst_hdmi_dto1 = 0;
	gsoft_reset_8.swrst_adto = 0;

	gsoft_reset_6.swrst_ultra = 0;
	gsoft_reset_6.swrst_half_ultra = 0;

	VPORT_I2C_Write((UINT32*)&gsoft_reset_1);
	VPORT_I2C_Write((UINT32*)&gsoft_reset_2);
	VPORT_I2C_Write((UINT32*)&gsoft_reset_3);
	VPORT_I2C_Write((UINT32*)&gsoft_reset_5);
	VPORT_I2C_Write((UINT32*)&gsoft_reset_8);
	VPORT_I2C_Write((UINT32*)&gsoft_reset_6);
}

static void	_HDMI_L9Ax_ClearResetLink(BOOLEAN reset)
{
	/* Link sys reset */
	VPORT_I2C_Read((UINT32*)&gsoft_reset_1);

	gsoft_reset_1.swrst_f3m = reset;

	VPORT_I2C_Write((UINT32*)&gsoft_reset_1);
}


/**
* HDMI_L9Ax_Get3DScanType
* Get 3D format Scan Type
* @parm void
* @return int		1 : 3D Timing Info - Scan Type is interlace
*/
int HDMI_L9Ax_Get3DScanType(void)
{
	int		ret	= 0;
	UINT8	vic = 0;

	LX_HDMI_VSI_PACKET_T  pVSIPacket = {0, };

	HDMI_GetVsiPacket(&pVSIPacket);

	//if(!HDMI_GetPortConnection())
	if (!_gSCDT || pVSIPacket.eVsiPacket.eVideoFormat != 2 || pVSIPacket.eVsiPacket.e3DStructure > 1)
	{
		ret = 0;
		goto func_exit;
	}

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		VPORT_I2C_Read((UINT32*)&glink_116);
		//VIC6~VIC0
		vic = glink_116.reg_pkt_avi_dat_pip0_4;
		vic = (vic & 0x7F);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&glink_520);
		//VIC6~VIC0
		vic = glink_520.reg_pkt_avi_dat_pip1_4;
		vic = (vic & 0x7F);
	}

	//HDMI_ERROR("VideoFormat: [%d]\n", pVSIPacket.eVsiPacket.eVideoFormat);
	//HDMI_ERROR("3DStructure: [%d]\n", pVSIPacket.eVsiPacket.e3DStructure);
	//HDMI_ERROR("vic			: [%d]\n", vic);

	if(pVSIPacket.eVsiPacket.e3DStructure < 2)
	{
	 	if(vic == 5 || vic == 20)		// 1080!@60Hz & 1080!@50Hz
			ret = 1;
		else
			ret = 0;
	}

func_exit:

	return ret;
}

/**
* HDMI_L9Ax_GetCSCStatus
* CSS Status Information
* @parm void
* @return int - 1 : changed , 0 : don't change
*/
int HDMI_L9Ax_GetCSCStatus(void)
{
	UINT8 tmp8 = 0;

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* Port 0 */
		//VPORT_I2C_Read((UINT32*)&glink_1);
		//if(glink_1.reg_hdmi_mode_pip0)
		if(gBootData->mode)
		{
			//Y1Y0
			VPORT_I2C_Read((UINT32*)&glink_119);
			tmp8 = glink_119.reg_pkt_avi_dat_pip0_1;
			_gCurPixelEncoding = (tmp8 & 0x60)>>5;
		}
		else
		{
			_gCurPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
		}
	}
	else
	{
		/* Port 1 */
		//VPORT_I2C_Read((UINT32*)&glink_405);
		//if(glink_405.reg_hdmi_mode_pip1)

		if(gBootData->mode)
		{
			//Y1Y0
			VPORT_I2C_Read((UINT32*)&glink_523);
			tmp8 = glink_523.reg_pkt_avi_dat_pip1_1;
			_gCurPixelEncoding = (tmp8 & 0x60)>>5;
		}
		else
		{
			_gCurPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
		}
	}

	HDMI_PRINT(" CSC _gCurPixelEncoding = [%d] \n", _gCurPixelEncoding);
	return 0;
}

/**
* HDMI_L9Ax_SetPixelRepetition
* Pixel Repetition for 13.5MHz formats - support Pixel Repetition 2
* @parm void
* @return void
*/
void HDMI_L9Ax_SetPixelRepetition(void)
{
	UINT8 repet = 0;
	UINT8 repet_1 = 0;

	/* L9A-L9D LVDS Connection(Over : 25MHz, No Phy Pixel PLL Dividing)
	   L9 B0 에서서는 H/W 적으로 처리함. support x2, x4 */

	/* Port0 &Port1 - phy0 로 입력 소스임. 그래서 LVDS 25MHz 이하 대응을 위해서 적용 20110725 */
	//if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		VPORT_I2C_Read((UINT32*)&glink_339);
		repet = glink_339.reg_pr_tx_pip0;		// From Source Device

		//if(gBootData->pixelRepet == repet)
			//return ;

		VPORT_I2C_Read((UINT32*)&glink_2);
		VPORT_I2C_Read((UINT32*)&glink_338);
		VPORT_I2C_Read((UINT32*)&glink_341);

		if(repet != 0)
		{
			glink_338.reg_pr_rx_pip0 = 0;		// Manual Pixel Repetition
			glink_2.reg_avc_en_pip0 = 0;		// Manual Video Configuration

			if( _gCurPixelEncoding == LX_HDMI_AVI_COLORSPACE_YCBCR422)
			{
				glink_341.reg_align422_rx_pip0 = 1;
				VPORT_I2C_Write((UINT32*)&glink_341);
			}
			else
			{
				if(glink_341.reg_align422_rx_pip0)
				{
					glink_341.reg_align422_rx_pip0 = 0;
					VPORT_I2C_Write((UINT32*)&glink_341);
				}
			}

			HDMI_PRINT(" Port(PHy)[%d]  Pixel Source(Tx) [%d] \n",_gui32CurPort,  glink_339.reg_pr_tx_pip0);
			HDMI_PRINT(" Rx Pixel[%d] Auto Video Conf [%d]\n",glink_338.reg_pr_rx_pip0, glink_2.reg_avc_en_pip0);
		}
		else
		{
			glink_2.reg_avc_en_pip0 = 1;		// Auto Video Configuration
			if(glink_341.reg_align422_rx_pip0)
			{
				glink_341.reg_align422_rx_pip0 = 0;
				VPORT_I2C_Write((UINT32*)&glink_341);
			}

		}
		VPORT_I2C_Write((UINT32*)&glink_2);
		VPORT_I2C_Write((UINT32*)&glink_338);

	}
	//else
	{
		VPORT_I2C_Read((UINT32*)&glink_743);
		repet_1 = glink_743.reg_pr_tx_pip1;		// From Source Device

		VPORT_I2C_Read((UINT32*)&glink_406);
		VPORT_I2C_Read((UINT32*)&glink_742);
		VPORT_I2C_Read((UINT32*)&glink_745);

		if(repet_1 != 0)
		{
			glink_742.reg_pr_rx_pip1 = 0;		// Manual Pixel Repetition
			glink_406.reg_avc_en_pip1 = 0;		// Manual Video Configuration

			if( _gCurPixelEncoding == LX_HDMI_AVI_COLORSPACE_YCBCR422)
			{
				glink_745.reg_align422_rx_pip1 = 1;
				VPORT_I2C_Write((UINT32*)&glink_745);
			}
			else
			{
				if(glink_745.reg_align422_rx_pip1)
				{
					glink_745.reg_align422_rx_pip1 = 0;
					VPORT_I2C_Write((UINT32*)&glink_745);
				}
			}


			HDMI_PRINT(" Port(PHy)[%d]  Pixel Source(Tx) [%d] \n",_gui32CurPort,  glink_743.reg_pr_tx_pip1);
			HDMI_PRINT(" Rx Pixel[%d] Auto Video Conf [%d]\n",glink_742.reg_pr_rx_pip1, glink_406.reg_avc_en_pip1);
		}
		else
		{
			glink_406.reg_avc_en_pip1 = 1;		// Auto Video Configuration
			if(glink_745.reg_align422_rx_pip1)
			{
				glink_745.reg_align422_rx_pip1 = 0;
				VPORT_I2C_Write((UINT32*)&glink_745);
			}
		}
		VPORT_I2C_Write((UINT32*)&glink_406);
		VPORT_I2C_Write((UINT32*)&glink_742);
	}
	gBootData->pixelRepet = repet;
}

/**
* HDMI_L9Ax_SetAutoSyncPolarity
*
* @parm void
* @return void
*/
void HDMI_L9Ax_SetAutoSyncPolarity(void)
{
	/* Auto Negative Polarity Setting from L9 */
	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		VPORT_I2C_Read((UINT32*)&glink_363);
		VPORT_I2C_Read((UINT32*)&glink_345);

		if(glink_363.reg_h_pol_pip0 == 0) // polarity Low
			glink_345.reg_inv_hs_pip0 = 1;
		else
			glink_345.reg_inv_hs_pip0 = 0;

		if(glink_363.reg_v_pol_pip0 == 0) // polarity Low
			glink_345.reg_inv_vs_pip0 = 1;
		else
			glink_345.reg_inv_vs_pip0 = 0;

		VPORT_I2C_Write((UINT32*)&glink_345);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&glink_767);
		VPORT_I2C_Read((UINT32*)&glink_749);

		if(glink_767.reg_h_pol_pip1 == 0) // polarity Low
			glink_749.reg_inv_hs_pip1 = 1;
		else
			glink_749.reg_inv_hs_pip1 = 0;

		if(glink_767.reg_v_pol_pip1 == 0) // polarity Low
			glink_749.reg_inv_vs_pip1 = 1;
		else
			glink_749.reg_inv_vs_pip1 = 0;

		VPORT_I2C_Write((UINT32*)&glink_749);
	}
}

/**
* HDMI_L9Ax_GetPortConnection
*
* @parm void
* @return int - 1 : connected , 0 : disconnected
*/
int HDMI_L9Ax_GetPortConnection(void)
{
	LX_HDMI_MUTE_CTRL_T		hdmiMute = {LX_HDMI_MAIN_WINDOW, 0, 0, 2};  // Default - A/V Clear

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* Port 0 */
		VPORT_I2C_Read((UINT32*)&glink_0);
		_gSCDT = glink_0.reg_scdt_prt0;

		if( _gSCDT == 1)
		{
			_gcount = 0;

			VPORT_I2C_Read((UINT32*)&glink_151);

			if( glink_151.reg_pkt_gcp_smute_pip0 == 1)
			{
				_gSourceMute = 1;
				_gMuteCount = 0;
			}
			else if( (glink_151.reg_pkt_gcp_cmute_pip0 == 1) ||  ( glink_151.reg_pkt_gcp_smute_pip0 == 0))
			{
				if( (_gSourceMute == 1) && (_gMuteCount == 0) )
				{
					_gMuteCount = 1;
					_gSourceMute = 1;
				}
				else
				{
					_gMuteCount = 0;
					_gSourceMute = 0;
				}
			}
		}
		else
		{
			gBootData->mode = 0;
			gBootData->pixelRepet = 0;

			if(_gcount == 0)
			{
#if 0
				if(gBootData->pixelRepet > 0)	//Juhee FIXME : check if L9 has the same bug?
				{
					//VPORT_I2C_Read((UINT32*)&gsoft_reset_1);
					//gsoft_reset_1.swrst_link_prt0_tmds = 0;
					//VPORT_I2C_Write((UINT32*)&gsoft_reset_1);

					//VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_33);
					//ghdmi_phy_n_33.pix_pclk_div2 = 0;
					//VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_33);

					gBootData->pixelRepet = 0;
				}

				//* Sony BD Player(BD-S1E) Format Detection Issue */
				//* Llinksys & linktmds reset NG : linktmds -> Llinksys Sequence
				_HDMI_L9Ax_ClearResetLink(0);
#endif
				VPORT_I2C_Read((UINT32*)&glink_341);

				if(glink_341.reg_align422_rx_pip0 == 1)
				{
					glink_341.reg_align422_rx_pip0 = 0;
					VPORT_I2C_Write((UINT32*)&glink_341);
				}
			}
			else if((_gSourceMute > 0 || _gHDMI_CSC_Mute == 1) && _gcount == 2 )
			{
				HDMI_SetMute(&hdmiMute);
				//HDMI_ERROR("No SCDT - Mute Clear\n");
				_gSourceMute = _gHDMI_CSC_Mute = 0;
				_gcount = 1;
			}
		}
	}
	else
	{
		/* Port 1 */
		VPORT_I2C_Read((UINT32*)&glink_404);
		_gSCDT = glink_404.reg_scdt_prt1;

		if( _gSCDT == 1)
		{
			_gcount = 0;
		}
		else
		{
			gBootData->mode = 0;
			gBootData->pixelRepet = 0;
			if(_gcount == 0)
			{
#if 0
				if(gBootData->pixelRepet > 0)	//Juhee FIXME : check if L9 has the same bug?
				{
					//VPORT_I2C_Read((UINT32*)&gsoft_reset_1);
					//gsoft_reset_1.swrst_link_prt0_tmds = 0;
					//VPORT_I2C_Write((UINT32*)&gsoft_reset_1);

					/* Auto Pixel repetition from L9 B0 H/W */
					//VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_33);
					//ghdmi_phy_n_33.pix_pclk_div2 = 0;
					//VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_33);

					gBootData->pixelRepet = 0;
				}

				//* Sony BD Player(BD-S1E) Format Detection Issue L8 B0*/
				//* Llinksys & linktmds reset NG : linktmds -> Llinksys Sequence
				_HDMI_L9Ax_ClearResetLink(0);
#endif
				_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;

				_gcount = 2;
			}
			else if((_gSourceMute > 0 || _gHDMI_CSC_Mute == 1) && _gcount == 2 )
			{
				HDMI_SetMute(&hdmiMute);
				//HDMI_ERROR("No SCDT - Mute Clear\n");
				_gSourceMute = _gHDMI_CSC_Mute = 0;
				_gcount = 1;
			}
		}
	}
	HDMI_PRINT("HDMI_L9Ax_GetPortConnection PORT[%d] - SCDT[%d]\n",_gui32CurPort, _gSCDT);
	return _gSCDT;
}

/**
* HDMI_L9Ax_GetMode
*
* @parm LX_HDMI_MODE_T
* @return int
*/
int HDMI_L9Ax_GetMode(LX_HDMI_MODE_T *mode)
{
	int 	ret = 0;
	BOOLEAN bHDMI = 0;

	//if(!HDMI_GetPortConnection()) { goto func_exit; }
	if (!_gSCDT) {bHDMI = 0; goto func_exit; }
	else bHDMI = gBootData->mode;
#if 0
	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* Port 0 */
		VPORT_I2C_Read((UINT32*)&glink_1);
		bHDMI = glink_1.reg_hdmi_mode_pip0;
	}
	else
	{
		/* Port 1 */
		VPORT_I2C_Read((UINT32*)&glink_405);
		bHDMI = glink_405.reg_hdmi_mode_pip1;
	}
#endif

func_exit:
	HDMI_PRINT(" HDMI_L8Bx_GetMode = DDIPort[%d] CurPort[%d] mode[%d] \n", mode->u8port, _gui32CurPort, bHDMI);
	mode->srcIdx 	= LX_HDMI_MAIN_WINDOW;
	mode->bHDMI 	= bHDMI;
	mode->u8port 	= _gui32CurPort;

	//Update global value for debug print
	if(bHDMI == 0)
	{
		_gHDMIAudioInfo.audioType = LX_HDMI_AUDIO_DVI;
	}

	return ret;
}

/**
* HDMI_L9Ax_GetAspectRatio
*
* @parm LX_HDMI_ASPECTRATIO_T
* @return int
*/
int HDMI_L9Ax_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio)
{
	int ret = 0;
	UINT8 tmp8 = 0;

	LX_HDMI_AVI_PICTURE_ARC_T aspectRatio = 0; // 0 : ARC_NO_DATA

	//if(!HDMI_GetPortConnection()) { memset((void *)ratio , 0 , sizeof(LX_HDMI_ASPECTRATIO_T)); goto func_exit; }
	if (!_gSCDT) { memset((void *)ratio , 0 , sizeof(LX_HDMI_ASPECTRATIO_T)); goto func_exit; }

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* Port 0 */
		//M1M0
		VPORT_I2C_Read((UINT32*)&glink_118);
		tmp8 = glink_118.reg_pkt_avi_dat_pip0_2;
		aspectRatio = (tmp8 & 0x30)>>4;
	}
	else
	{
		/* Port 1 */
		//M1M0
		VPORT_I2C_Read((UINT32*)&glink_522);
		tmp8 = glink_522.reg_pkt_avi_dat_pip1_2;
		aspectRatio = (tmp8 & 0x30)>>4;
	}

func_exit:
	ratio->srcIdx = LX_HDMI_MAIN_WINDOW;
	ratio->eAspectRatio = aspectRatio;

	return ret;
}

/**
* HDMI_L9Ax_GetTimingInfo
*
* @parm HDMI_IOR_GET_TIMING_INFO
* @return int
*/
int HDMI_L9Ax_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info)
{
	int ret = 0;
	BOOLEAN mode = 0,		hdcpAuth = 0;
	//BOOLEAN pixelRepet = 0, muteClr	= 0;
	UINT32	tmdsClock = 0,	tmpClock = 0;
	UINT32	up_freq = 0,	down_freq = 0;
	UINT8 	pixelRepet = 0,	colorDepth = 0;
	UINT8 	/*prt_sel = 0,*/ 	count = 0;
	//UINT32  aviInfo = 0;
	LX_HDMI_TIMING_INFO_T 	bufTiming = {0,};
	//LX_HDMI_MUTE_CTRL_T		hdmiMute = {LX_HDMI_MAIN_WINDOW, 0, 0, 2};  // Default - A/V Clear

	memset((void *)info , 0 , sizeof(LX_HDMI_TIMING_INFO_T));

	if(!HDMI_GetPortConnection())
	{
		memset(&bufTiming , 0 , sizeof(LX_HDMI_TIMING_INFO_T));
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
		_gHDMI_DVI_Done = _gHDMI_Change = 0;
		_gTimingStatus = 0;
		//HDMI_ERROR(" HDMI_L8Bx_GetTimingInfo  PortConnection \n");
		goto func_exit;
	}
#if 0
	/* Sourec Mute & CSC Mute Clear */
	VPORT_RdFL(hdmi_link_001); //FIXME : check Myung
	VPORT_Rd01(hdmi_link_001, int_clr_mute, muteClr);			///< Source Mute clear

	if(_gHDMI_CSC_Mute == 1 && _gHDMI_port_changed == 1)
	{
		_gHDMI_CSC_Mute = 0;
		HDMI_SetMute(&hdmiMute);
		//HDMI_ERROR("HDMI CSC - Mute Clear\n" );
	}

	if(muteClr == 1 && _gSourceMute > 0 )
	{
		_gSourceMute--;
		if(_gSourceMute == 0)
		{
			HDMI_SetMute(&hdmiMute);
			//HDMI_ERROR("HDMI Source - Mute Clear\n" );
		}
	}
///// < Mute Clear end.
#endif

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		VPORT_I2C_Read((UINT32*)&glink_1);
		gBootData->mode = mode = glink_1.reg_hdmi_mode_pip0;
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&glink_405);
		gBootData->mode = mode = glink_405.reg_hdmi_mode_pip1;
	}

	if(_gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch])
	{
		HDMI_SetPixelRepetition();			// for specific 13.5MHz input(480i, 576i) L9 A0 only, L9 B0 don't use
		HDMI_SetAutoSyncPolarity();			// sync inversion for UD and 3D(SS_Full) format
	}

	if(_gHDMI_port_changed > 0)
	{
		_gHDMI_port_changed --;
		HDMI_PRINT("------ HDMI Port changed ===== %d---------\n",_gHDMI_port_changed);
		goto func_exit;
	}

	if( _gSourceMute == 1)
	{
		HDMI_PRINT("------ HDMI Source Mute ---------\n");
		if(_gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch])
			_gHDMI_DVI_Done++;
		goto func_exit;
	}
//////////////* L9 A0/A1 CSC control(과도 존재함) L9 B0  //////////////////////////
//////////////* L9 B0 부터 IQR handler로 구현 예정 		 //////////////////////////
#if 1
	HDMI_L9Ax_GetCSCStatus();

	if(_gPrevPixelEncoding != _gCurPixelEncoding)
	{
		HDMI_PRINT("------CSC_changed ===== %d---------\n",_gCurPixelEncoding);
		_gPrevPixelEncoding = _gCurPixelEncoding;
		goto func_exit;
	}
#endif

	bufTiming.hFreq		= 0; 				///< Horizontal frequency(100 Hz unit) = Dot Freq / hTotal
	bufTiming.vFreq		= 0; 				///< Veritical frequency(1/10 Hz unit) = hFreq / vTotal

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		VPORT_I2C_Read((UINT32*)&glink_346);
		VPORT_I2C_Read((UINT32*)&glink_347);
		VPORT_I2C_Read((UINT32*)&glink_348);
		VPORT_I2C_Read((UINT32*)&glink_349);
		VPORT_I2C_Read((UINT32*)&glink_350);
		VPORT_I2C_Read((UINT32*)&glink_351);
		VPORT_I2C_Read((UINT32*)&glink_352);
		VPORT_I2C_Read((UINT32*)&glink_353);
		VPORT_I2C_Read((UINT32*)&glink_354);
		VPORT_I2C_Read((UINT32*)&glink_355);
		VPORT_I2C_Read((UINT32*)&glink_356);
		VPORT_I2C_Read((UINT32*)&glink_357);
		VPORT_I2C_Read((UINT32*)&glink_358);
		VPORT_I2C_Read((UINT32*)&glink_359);

		VPORT_I2C_Read((UINT32*)&glink_363);
		VPORT_I2C_Read((UINT32*)&glink_28);


		bufTiming.hTotal = glink_346.reg_h_tot_pip0_1;
		bufTiming.hTotal = bufTiming.hTotal << 8;
		bufTiming.hTotal = bufTiming.hTotal | glink_347.reg_h_tot_pip0_0;

		bufTiming.vTotal = glink_348.reg_v_tot_pip0_1;
		bufTiming.vTotal = bufTiming.vTotal << 8;
		bufTiming.vTotal = bufTiming.vTotal | glink_349.reg_v_tot_pip0_0;

		bufTiming.hActive = glink_350.reg_h_av_pip0_1;
		bufTiming.hActive = bufTiming.hActive << 8;
		bufTiming.hActive = bufTiming.hActive | glink_351.reg_h_av_pip0_0;

		bufTiming.vActive = glink_352.reg_v_av_pip0_1;
		bufTiming.vActive = bufTiming.vActive << 8;
		bufTiming.vActive = bufTiming.vActive | glink_353.reg_v_av_pip0_0;
#if 0
		/* Front Porch */
		bufTiming.hStart = glink_354.reg_h_fp_pip0_1;
		bufTiming.hStart = bufTiming.hStart << 8;
		bufTiming.hStart = bufTiming.hStart | glink_355.reg_h_fp_pip0_0;

		bufTiming.vStart = glink_356.reg_v_fp_pip0;
#else
		/* Back Porch */
		bufTiming.hStart = glink_357.reg_h_bp_pip0_1;
		bufTiming.hStart = bufTiming.hStart << 8;
		bufTiming.hStart = bufTiming.hStart | glink_358.reg_h_bp_pip0_0;

		bufTiming.vStart = glink_359.reg_v_bp_pip0;
#endif
		///< Scan type (0 : interlace, 1 : progressive)
		bufTiming.scanType = glink_363.reg_intrl_pip0;
		bufTiming.scanType 	^= 1;

		hdcpAuth = glink_28.reg_hdcp_authed_prt0;
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&glink_750);
		VPORT_I2C_Read((UINT32*)&glink_751);
		VPORT_I2C_Read((UINT32*)&glink_752);
		VPORT_I2C_Read((UINT32*)&glink_753);
		VPORT_I2C_Read((UINT32*)&glink_754);
		VPORT_I2C_Read((UINT32*)&glink_755);
		VPORT_I2C_Read((UINT32*)&glink_756);
		VPORT_I2C_Read((UINT32*)&glink_757);
		VPORT_I2C_Read((UINT32*)&glink_758);
		VPORT_I2C_Read((UINT32*)&glink_759);
		VPORT_I2C_Read((UINT32*)&glink_760);
		VPORT_I2C_Read((UINT32*)&glink_761);
		VPORT_I2C_Read((UINT32*)&glink_762);
		VPORT_I2C_Read((UINT32*)&glink_763);

		VPORT_I2C_Read((UINT32*)&glink_767);
		VPORT_I2C_Read((UINT32*)&glink_432);

		bufTiming.hTotal = glink_750.reg_h_tot_pip1_1;
		bufTiming.hTotal = bufTiming.hTotal << 8;
		bufTiming.hTotal = bufTiming.hTotal | glink_751.reg_h_tot_pip1_0;

		bufTiming.vTotal = glink_752.reg_v_tot_pip1_1;
		bufTiming.vTotal = bufTiming.vTotal << 8;
		bufTiming.vTotal = bufTiming.vTotal | glink_753.reg_v_tot_pip1_0;

		bufTiming.hActive = glink_754.reg_h_av_pip1_1;
		bufTiming.hActive = bufTiming.hActive << 8;
		bufTiming.hActive = bufTiming.hActive | glink_755.reg_h_av_pip1_0;

		bufTiming.vActive = glink_756.reg_v_av_pip1_1;
		bufTiming.vActive = bufTiming.vActive << 8;
		bufTiming.vActive = bufTiming.vActive | glink_757.reg_v_av_pip1_0;
#if 0
		/* Front Porch */
		bufTiming.hStart = glink_758.reg_h_fp_pip1_1;
		bufTiming.hStart = bufTiming.hStart << 8;
		bufTiming.hStart = bufTiming.hStart | glink_759.reg_h_fp_pip1_0;

		bufTiming.vStart = glink_760.reg_v_fp_pip1;
#else
		/* Back Porch */
		bufTiming.hStart = glink_761.reg_h_bp_pip1_1;
		bufTiming.hStart = bufTiming.hStart << 8;
		bufTiming.hStart = bufTiming.hStart | glink_762.reg_h_bp_pip1_0;

		bufTiming.vStart = glink_763.reg_v_bp_pip1;
#endif
		///< Scan type (0 : interlace, 1 : progressive)
		bufTiming.scanType = glink_767.reg_intrl_pip1;
		bufTiming.scanType 	^= 1;

		hdcpAuth = glink_432.reg_hdcp_authed_prt1;
	}

#if 1
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_23);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_24);

	up_freq = ghdmi_phy_n_23.tmds_freq;
	down_freq = ghdmi_phy_n_24.tmds_freq;

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		VPORT_I2C_Read((UINT32*)&glink_339);
		//VPORT_I2C_Read((UINT32*)&glink_340);
		VPORT_I2C_Read((UINT32*)&glink_344);
		pixelRepet = glink_339.reg_pr_tx_pip0;		// From Source Device
		//pixelRepet = glink_340.reg_pr_stat_pip0;
		colorDepth = glink_344.reg_cmode_stat_pip0;
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&glink_743);
		//VPORT_I2C_Read((UINT32*)&glink_744);
		VPORT_I2C_Read((UINT32*)&glink_748);
		pixelRepet = glink_743.reg_pr_tx_pip1;		// From Source Device
		//pixelRepet = glink_744.reg_pr_stat_pip1;
		colorDepth = glink_748.reg_cmode_stat_pip1;
	}

	tmdsClock = ((up_freq << 8) + down_freq);

	tmpClock = tmdsClock * 1000;

#endif

	if(_gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch])
	{
		if(mode == 1 && _gHDMI_DVI_Done < gBootData->hdmiCn[gBootData->hdmi_switch]) // HDMI Mode
		{
			if(gBootData->hdmi_switch)
			{
				//HDMI_ERROR("------  HDMI [5]  -> [%d]---------\n",_gHDMI_DVI_Done);
				bufTiming.hStart = bufTiming.hStart - _gHDMI_DVI_Done;					///< Horizontal start pixel (Back Porch)
				bufTiming.vStart = bufTiming.vStart + _gHDMI_DVI_Done; 					///< Vertical start lines (Back Porch)
			}
			else
			{
				if(hdcpAuth == 0)
				{
					//HDMI_ERROR("------bx_gHDMI_DVI_Done HDMI  ===== %d]---------\n",_gHDMI_DVI_Done);
					bufTiming.hStart = bufTiming.hStart - _gHDMI_DVI_Done;					///< Horizontal start pixel (Back Porch)
					bufTiming.vStart = bufTiming.vStart + _gHDMI_DVI_Done; 					///< Vertical start lines (Back Porch)
					_gHDMI_Change = _gHDMI_DVI_Done;
				}
				else
				{
					if(_gHDMI_DVI_Done < _gHDMI_Change+3)
					{
						bufTiming.hStart = bufTiming.hStart - _gHDMI_DVI_Done;				///< Horizontal start pixel (Back Porch)
						bufTiming.vStart = bufTiming.vStart + _gHDMI_DVI_Done; 				///< Vertical start lines (Back Porch)
					}
				}
			}
		}
		else		// DVI Mode
		{
			//HDMI_ERROR("------ DVI[2] -> %d  ---------\n",_gHDMI_DVI_Done);
			if(_gHDMI_DVI_Done < gBootData->dviCn[gBootData->hdmi_switch])
			{
				bufTiming.hStart = bufTiming.hStart - _gHDMI_DVI_Done;					///< Horizontal start pixel (Back Porch)
				bufTiming.vStart = bufTiming.vStart + _gHDMI_DVI_Done; 					///< Vertical start lines (Back Porch)
			}
		}
		_gHDMI_DVI_Done++;
	}

	/* All vStart Value set even for Interlace Format, 1080i Issue*/
	if( bufTiming.scanType == 0)
		bufTiming.vStart = ((bufTiming.vStart >> 1) << 1);

#if 1
	_HDMI_L9Ax_Set3DFullTimingInfo(&bufTiming);
#else
	if(bufTiming.vActive == 1103 || bufTiming.vActive == 2228)		// 3D - Interlace Format Setting
	{
		if( HDMI_L9Ax_Get3DScanType( ) )
			bufTiming.scanType = 0;
	}
#endif

#if 0
	if(_gHDMI_DVI_Done >= gBootData->timingCn[gBootData->hdmi_switch])
	{
		if(_gPrevTiming.hTotal	!= bufTiming.hTotal)
			HDMI_ERROR(" Timinginfo   pre hTotal = %d //  buf hTotal = %d \n", _gPrevTiming.hTotal, bufTiming.hTotal);
		if(_gPrevTiming.vTotal	!= bufTiming.vTotal)
			HDMI_ERROR(" Timinginfo   pre vTotal = %d //  buf vTotal = %d \n", _gPrevTiming.vTotal, bufTiming.vTotal);
		if(_gPrevTiming.hStart 	!= bufTiming.hStart)
			HDMI_ERROR(" Timinginfo   pre hStart = %d //  buf hStart = %d \n", _gPrevTiming.hStart, bufTiming.hStart);
		if(_gPrevTiming.vStart 	!= bufTiming.vStart)
			HDMI_ERROR(" Timinginfo   pre vStart = %d //  buf vStart = %d \n", _gPrevTiming.vStart, bufTiming.vStart);
		if(_gPrevTiming.hActive	!= bufTiming.hActive)
			HDMI_ERROR(" Timinginfo   pre hActive = %d //  buf hActive = %d \n", _gPrevTiming.hActive, bufTiming.hActive);
		if(_gPrevTiming.vActive	!= bufTiming.vActive)
			HDMI_ERROR(" Timinginfo   pre vActive = %d //  buf vActive = %d \n", _gPrevTiming.vActive, bufTiming.vActive);
		if(_gPrevTiming.scanType	!= bufTiming.scanType)
			HDMI_ERROR(" Timinginfo   pre scanType = %d //  buf scanType = %d \n\n", _gPrevTiming.scanType, bufTiming.scanType);
		if(_gPrevTiming.full_3d_timing	!= bufTiming.full_3d_timing)
			HDMI_ERROR(" Timinginfo   pre full_3d_timing = %d //  buf full_3d_timing = %d \n\n", _gPrevTiming.full_3d_timing, bufTiming.full_3d_timing);
	}
#endif

#if 1		// Divx Player(Venice sometimes Timing Error) & Master6100 DVI Port - Sync down delay for Format Change
	if(_gHDMI_DVI_Done >= gBootData->timingCn[gBootData->hdmi_switch])
	{
		if(_gPrevTiming.hTotal	!= bufTiming.hTotal)		count++;
		if(_gPrevTiming.vTotal	!= bufTiming.vTotal)		count++;
		if(_gPrevTiming.hActive	!= bufTiming.hActive)		count++;
		if(_gPrevTiming.vActive	!= bufTiming.vActive)		count++;

		if( (count > 0) &&
		 	( (bufTiming.vActive == 0) || (bufTiming.vTotal == 0) || (bufTiming.hActive == 0) || (bufTiming.vActive == 0)
//			|| (_gPrevTiming.hStart != bufTiming.hStart)
//			|| (_gPrevTiming.vStart != bufTiming.vStart)
			|| (_gPrevTiming.scanType != bufTiming.scanType) )
		)
		{
			//HDMI_ERROR("\n\n== Timinginfo Changed PORT 0 ==\n\n");
			_gHDMI_DVI_Done = _gHDMI_Change = 0;
			count = 0;
		}

		if(_gTimingStatus == 1)
		{
			if(count > 0)		_gTimingStatus = 1;
			else				_gTimingStatus = 0;
		}
		else
		{
			if(count > 0)
			{
				bufTiming.hFreq		= _gPrevTiming.hFreq;
				bufTiming.vFreq		= _gPrevTiming.vFreq;
				bufTiming.hTotal	= _gPrevTiming.hTotal;
				bufTiming.vTotal	= _gPrevTiming.vTotal;
				bufTiming.hStart	= _gPrevTiming.hStart;
				bufTiming.vStart	= _gPrevTiming.vStart;
				bufTiming.hActive	= _gPrevTiming.hActive;
				bufTiming.vActive 	= _gPrevTiming.vActive;
				bufTiming.scanType	= _gPrevTiming.scanType;
				bufTiming.full_3d_timing = _gPrevTiming.full_3d_timing;

				_gTimingStatus = 1;

				if(_gui32CurPort == HDMI_L9AX_PORT_0)
				{
					//VPORT_I2C_Read((UINT32*)&glink_2);
					VPORT_I2C_Read((UINT32*)&glink_338);
					VPORT_I2C_Read((UINT32*)&glink_339);
					VPORT_I2C_Read((UINT32*)&glink_340);
					//HDMI_ERROR("\n== Timinginfo ERROR PORT 0 count[%d]==\n",count);
					//HDMI_ERROR(" Timinginfo [Mode = %d] [TMDS = %d] [ColorDepth = %d] [pixelRepet = %d] \n", mode, tmdsClock, colorDepth, pixelRepet);
					//HDMI_ERROR(" PORT 0 Stat[%d] Repetition Tx[%d] Rx[%d] \n",  glink_340.reg_pr_stat_pip0, glink_339.reg_pr_tx_pip0, glink_338.reg_pr_rx_pip0);
					//HDMI_ERROR(" avc_en_pip0[%d]\n", glink_2.reg_avc_en_pip0);
				}
				else
				{
					//VPORT_I2C_Read((UINT32*)&glink_406);
					VPORT_I2C_Read((UINT32*)&glink_742);
					VPORT_I2C_Read((UINT32*)&glink_743);
					VPORT_I2C_Read((UINT32*)&glink_744);
					//HDMI_ERROR("\n== Timinginfo ERROR PORT 1 ==\n");
					//HDMI_ERROR(" Timinginfo [Mode = %d] [TMDS = %d] [ColorDepth = %d] [pixelRepet = %d] \n", mode, tmdsClock, colorDepth, pixelRepet);
					//HDMI_ERROR(" Stat[%d] Repetition Tx[%d] Rx[%d] \n",  glink_744.reg_pr_stat_pip1, glink_743.reg_pr_tx_pip1, glink_742.reg_pr_rx_pip1);
					//HDMI_ERROR(" avc_en_pip0[%d]\n", glink_406.reg_avc_en_pip1);
				}
			}
		}
	}
#endif

	// InValid Format Check
	if(	(bufTiming.vActive < 240)  || (bufTiming.hActive < 320) ||
		(bufTiming.vActive > 2970) || (bufTiming.hActive > 4096) )
	{
		//memset(&bufTiming , 0 , sizeof(LX_HDMI_TIMING_INFO_T));
		goto func_exit;
	}

//////////////* Horizontal & Vertical frequency //////////////////////////
#if 1
	if(colorDepth)
	{
		tmdsClock= tmdsClock * 100;
		if(colorDepth ==1 )	tmdsClock =	tmdsClock / 125;					// colorDepth = 10bit
		else				tmdsClock =	tmdsClock / 150;					// colorDepth = 12bit
	}
	tmdsClock = tmdsClock * 1000;
	//bufTiming.hFreq = tmdsClock / bufTiming.hTotal; 		///< Horizontal frequency(100 Hz unit) = Dot Freq / hTotal

	bufTiming.vFreq = tmdsClock / bufTiming.vTotal;
	tmpClock = bufTiming.vFreq * 100;
	bufTiming.vFreq = tmpClock / bufTiming.hTotal;			///< Veritical frequency(1/10 Hz unit) = hFreq / vTotal

	if(pixelRepet == 1 ) // HDMI_SetPixelRepetition
	{
		bufTiming.vFreq = bufTiming.vFreq / (pixelRepet + pixelRepet);
		//bufTiming.hFreq = bufTiming.hFreq / 20;
	}
	else
	{
		//bufTiming.hFreq = bufTiming.hFreq / 10;
	}

	if( bufTiming.scanType == 0)		// 0 : interlace
		bufTiming.vFreq = bufTiming.vFreq + bufTiming.vFreq;

	if ( _gPrevTiming.vFreq	!= bufTiming.vFreq )
	{
		if((bufTiming.vFreq - _gPrevTiming.vFreq) == 1)
			bufTiming.vFreq --;
		else if((_gPrevTiming.vFreq - bufTiming.vFreq ) == 1)
			bufTiming.vFreq ++;
	}
/*
	if ( _gPrevTiming.hFreq	!= bufTiming.hFreq )
	{
		if((bufTiming.hFreq - _gPrevTiming.hFreq) == 1)
			bufTiming.hFreq --;
		else if((_gPrevTiming.hFreq - bufTiming.hFreq ) == 1)
			bufTiming.hFreq ++;
	}
*/
	if( bufTiming.vFreq > 1000 )
		bufTiming.vFreq = 1000;
#else
	bufTiming.vFreq = 1;
#endif
//////////////* Horizontal & Vertical frequency end //////////////////////////

func_exit:
	info->srcIdx = LX_HDMI_MAIN_WINDOW;									///< window index
	info->hFreq 	= _gPrevTiming.hFreq 	= bufTiming.hFreq; 		///< Horizontal frequency(100 Hz unit) = Dot Freq / hTotal
	info->vFreq 	= _gPrevTiming.vFreq 	= bufTiming.vFreq; 		///< Veritical frequency(1/10 Hz unit) = hFreq / vTotal
	info->hTotal	= _gPrevTiming.hTotal 	= bufTiming.hTotal; 		///< Horizontal total pixels
	info->vTotal 	= _gPrevTiming.vTotal	= bufTiming.vTotal; 		///< Vertical total lines
	info->hStart	= _gPrevTiming.hStart	= bufTiming.hStart; 		///< Horizontal start pixel (Back Porch)
	info->vStart 	= _gPrevTiming.vStart	= bufTiming.vStart;		///< Vertical start lines (Back Porch)
	info->hActive 	= _gPrevTiming.hActive	= bufTiming.hActive;		///< Horizontal active pixel
	info->vActive 	= _gPrevTiming.vActive	= bufTiming.vActive; 	///< Vertical active lines
	info->scanType 	= _gPrevTiming.scanType	= bufTiming.scanType; 	///< Scan type (0 : interlace, 1 : progressive) 	info->scanType ^= 1;
	info->full_3d_timing	= _gPrevTiming.full_3d_timing	= bufTiming.full_3d_timing; 	///< Full 3D Timing
	return ret;
}

/**
* HDMI_L9Ax_GetStatus
*
* @parm LX_HDMI_STATUS_T
* @return int
*/
int HDMI_L9Ax_GetStatus(LX_HDMI_STATUS_T *status)
{
	int ret = 0;

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* PORT 0 */
		//VPORT_I2C_Read((UINT32*)&glink_1);
		VPORT_I2C_Read((UINT32*)&glink_344);
		//VPORT_I2C_Read((UINT32*)&ghpd_out_control_0);

		status->bHdmiMode = gBootData->mode;	//glink_1.reg_hdmi_mode_pip0;
		status->eColorDepth = glink_344.reg_cmode_stat_pip0;
		status->eHotPlug = _gSCDT; //ghpd_out_control_0.hpd0_oen;
		status->u8hdmiport = _gui32CurPort;
		status->pixelRepet = gBootData->pixelRepet;
		status->csc = _gPrevPixelEncoding;
	}
	else
	{
		/* PORT 1 */
		//VPORT_I2C_Read((UINT32*)&glink_405);
		VPORT_I2C_Read((UINT32*)&glink_748);
		//VPORT_I2C_Read((UINT32*)&ghpd_out_control_0);

		status->bHdmiMode = gBootData->mode; 	//glink_405.reg_hdmi_mode_pip1;
		status->eColorDepth = glink_748.reg_cmode_stat_pip1;
		status->eHotPlug = _gSCDT; //ghpd_out_control_0.hpd1_oen;
		status->u8hdmiport = _gui32CurPort;
		status->pixelRepet = gBootData->pixelRepet;
		status->csc = _gPrevPixelEncoding;
	}

	return ret;
}

/**
* HDMI_L9Ax_GetVsiPacket
*
* @parm LX_HDMI_VSI_PACKET_T
* @return int
*/
int HDMI_L9Ax_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet)
{
	int ret = 0;
	UINT8 tmp8 = 0;

	HDMI_PRINT("%s: Get ASI Packet", __func__);

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* PORT 0 */
		VPORT_I2C_Read((UINT32*)&glink_334); //VSI data 0
		VPORT_I2C_Read((UINT32*)&glink_333);
		VPORT_I2C_Read((UINT32*)&glink_332);
		VPORT_I2C_Read((UINT32*)&glink_331);
		VPORT_I2C_Read((UINT32*)&glink_330);
		VPORT_I2C_Read((UINT32*)&glink_329);
		VPORT_I2C_Read((UINT32*)&glink_328);
		VPORT_I2C_Read((UINT32*)&glink_327);
		VPORT_I2C_Read((UINT32*)&glink_326);
		VPORT_I2C_Read((UINT32*)&glink_325);
		VPORT_I2C_Read((UINT32*)&glink_324);
		VPORT_I2C_Read((UINT32*)&glink_323);
		VPORT_I2C_Read((UINT32*)&glink_322);
		VPORT_I2C_Read((UINT32*)&glink_321);
		VPORT_I2C_Read((UINT32*)&glink_320);
		VPORT_I2C_Read((UINT32*)&glink_319);
		VPORT_I2C_Read((UINT32*)&glink_318);
		VPORT_I2C_Read((UINT32*)&glink_317);
		VPORT_I2C_Read((UINT32*)&glink_316);
		VPORT_I2C_Read((UINT32*)&glink_315);
		VPORT_I2C_Read((UINT32*)&glink_314);
		VPORT_I2C_Read((UINT32*)&glink_313);
		VPORT_I2C_Read((UINT32*)&glink_312);
		VPORT_I2C_Read((UINT32*)&glink_311);
		VPORT_I2C_Read((UINT32*)&glink_310);
		VPORT_I2C_Read((UINT32*)&glink_309);
		VPORT_I2C_Read((UINT32*)&glink_308);
		VPORT_I2C_Read((UINT32*)&glink_307); //VSI data 27

		VPORT_I2C_Read((UINT32*)&glink_306); //VSI HB 1
		VPORT_I2C_Read((UINT32*)&glink_305); //VSI HB 2

		///< 24 bit IEEE Registration Identifier[3]
		packet->eVsiPacket.IEERegId[0] = glink_333.reg_pkt_vsi_dat_pip0_1;
		packet->eVsiPacket.IEERegId[1] = glink_332.reg_pkt_vsi_dat_pip0_2;
		packet->eVsiPacket.IEERegId[2] = glink_331.reg_pkt_vsi_dat_pip0_3;

		///< Payload  [24]
		packet->eVsiPacket.PayLoad[0] = glink_330.reg_pkt_vsi_dat_pip0_4;
		packet->eVsiPacket.PayLoad[1] = glink_329.reg_pkt_vsi_dat_pip0_5;
		packet->eVsiPacket.PayLoad[2] = glink_328.reg_pkt_vsi_dat_pip0_6;
		packet->eVsiPacket.PayLoad[3] = glink_327.reg_pkt_vsi_dat_pip0_7;
		packet->eVsiPacket.PayLoad[4] = glink_326.reg_pkt_vsi_dat_pip0_8;
		packet->eVsiPacket.PayLoad[5] = glink_325.reg_pkt_vsi_dat_pip0_9;
		packet->eVsiPacket.PayLoad[6] = glink_324.reg_pkt_vsi_dat_pip0_10;
		packet->eVsiPacket.PayLoad[7] = glink_323.reg_pkt_vsi_dat_pip0_11;
		packet->eVsiPacket.PayLoad[8] = glink_322.reg_pkt_vsi_dat_pip0_12;
		packet->eVsiPacket.PayLoad[9] = glink_321.reg_pkt_vsi_dat_pip0_13;
		packet->eVsiPacket.PayLoad[10] = glink_320.reg_pkt_vsi_dat_pip0_14;
		packet->eVsiPacket.PayLoad[11] = glink_319.reg_pkt_vsi_dat_pip0_15;
		packet->eVsiPacket.PayLoad[12] = glink_318.reg_pkt_vsi_dat_pip0_16;
		packet->eVsiPacket.PayLoad[13] = glink_317.reg_pkt_vsi_dat_pip0_17;
		packet->eVsiPacket.PayLoad[14] = glink_316.reg_pkt_vsi_dat_pip0_18;
		packet->eVsiPacket.PayLoad[15] = glink_315.reg_pkt_vsi_dat_pip0_19;
		packet->eVsiPacket.PayLoad[16] = glink_314.reg_pkt_vsi_dat_pip0_20;
		packet->eVsiPacket.PayLoad[17] = glink_313.reg_pkt_vsi_dat_pip0_21;
		packet->eVsiPacket.PayLoad[18] = glink_312.reg_pkt_vsi_dat_pip0_22;
		packet->eVsiPacket.PayLoad[19] = glink_311.reg_pkt_vsi_dat_pip0_23;
		packet->eVsiPacket.PayLoad[20] = glink_310.reg_pkt_vsi_dat_pip0_24;
		packet->eVsiPacket.PayLoad[21] = glink_309.reg_pkt_vsi_dat_pip0_25;
		packet->eVsiPacket.PayLoad[22] = glink_308.reg_pkt_vsi_dat_pip0_26;
		packet->eVsiPacket.PayLoad[23] = glink_307.reg_pkt_vsi_dat_pip0_27;

		/**< HDMI VSI info */
		tmp8 = glink_330.reg_pkt_vsi_dat_pip0_4;
		packet->eVsiPacket.eVideoFormat = (tmp8 & 0xE0) >> 5;

		/* if videoFormat == 0, no more data */
		/* if videoFormat == 1, extended resolution present */
		packet->eVsiPacket.eVSiVIC = glink_329.reg_pkt_vsi_dat_pip0_5;
		tmp8 = glink_328.reg_pkt_vsi_dat_pip0_6;
		packet->eVsiPacket.e3DExtData = (tmp8 & 0xF0) >> 4;
		/* if videoFormat == 2, 3D Format indication with 3d structure */
		tmp8 = glink_329.reg_pkt_vsi_dat_pip0_5;
		packet->eVsiPacket.e3DStructure = (tmp8 & 0xF0) >> 4;

		///< Packet raw data
		packet->eVsiPacket.packet.type = 0x81;
		packet->eVsiPacket.packet.version = glink_306.reg_pkt_vsi_hdr_pip0_0;
		packet->eVsiPacket.packet.length = glink_305.reg_pkt_vsi_hdr_pip0_1 & 0x1F;
		packet->eVsiPacket.packet.dataBytes[0] = glink_334.reg_pkt_vsi_dat_pip0_0;
		packet->eVsiPacket.packet.dataBytes[1] = glink_333.reg_pkt_vsi_dat_pip0_1;
		packet->eVsiPacket.packet.dataBytes[2] = glink_332.reg_pkt_vsi_dat_pip0_2;
		packet->eVsiPacket.packet.dataBytes[3] = glink_331.reg_pkt_vsi_dat_pip0_3;
		packet->eVsiPacket.packet.dataBytes[4] = glink_330.reg_pkt_vsi_dat_pip0_4;
		packet->eVsiPacket.packet.dataBytes[5] = glink_329.reg_pkt_vsi_dat_pip0_5;
		packet->eVsiPacket.packet.dataBytes[6] = glink_328.reg_pkt_vsi_dat_pip0_6;
		packet->eVsiPacket.packet.dataBytes[7] = glink_327.reg_pkt_vsi_dat_pip0_7;
		packet->eVsiPacket.packet.dataBytes[8] = glink_326.reg_pkt_vsi_dat_pip0_8;
		packet->eVsiPacket.packet.dataBytes[9] = glink_325.reg_pkt_vsi_dat_pip0_9;
		packet->eVsiPacket.packet.dataBytes[10] = glink_324.reg_pkt_vsi_dat_pip0_10;
		packet->eVsiPacket.packet.dataBytes[11] = glink_323.reg_pkt_vsi_dat_pip0_11;
		packet->eVsiPacket.packet.dataBytes[12] = glink_322.reg_pkt_vsi_dat_pip0_12;
		packet->eVsiPacket.packet.dataBytes[13] = glink_321.reg_pkt_vsi_dat_pip0_13;
		packet->eVsiPacket.packet.dataBytes[14] = glink_320.reg_pkt_vsi_dat_pip0_14;
		packet->eVsiPacket.packet.dataBytes[15] = glink_319.reg_pkt_vsi_dat_pip0_15;
		packet->eVsiPacket.packet.dataBytes[16] = glink_318.reg_pkt_vsi_dat_pip0_16;
		packet->eVsiPacket.packet.dataBytes[17] = glink_317.reg_pkt_vsi_dat_pip0_17;
		packet->eVsiPacket.packet.dataBytes[18] = glink_316.reg_pkt_vsi_dat_pip0_18;
		packet->eVsiPacket.packet.dataBytes[19] = glink_315.reg_pkt_vsi_dat_pip0_19;
		packet->eVsiPacket.packet.dataBytes[20] = glink_314.reg_pkt_vsi_dat_pip0_20;
		packet->eVsiPacket.packet.dataBytes[21] = glink_313.reg_pkt_vsi_dat_pip0_21;
		packet->eVsiPacket.packet.dataBytes[22] = glink_312.reg_pkt_vsi_dat_pip0_22;
		packet->eVsiPacket.packet.dataBytes[23] = glink_311.reg_pkt_vsi_dat_pip0_23;
		packet->eVsiPacket.packet.dataBytes[24] = glink_310.reg_pkt_vsi_dat_pip0_24;
		packet->eVsiPacket.packet.dataBytes[25] = glink_309.reg_pkt_vsi_dat_pip0_25;
		packet->eVsiPacket.packet.dataBytes[26] = glink_308.reg_pkt_vsi_dat_pip0_26;
		packet->eVsiPacket.packet.dataBytes[27] = glink_307.reg_pkt_vsi_dat_pip0_27;

		packet->eVsiPacket.packetStatus = LX_HDMI_VSI_PACKET_STATUS_UPDATED;
	}
	else
	{
		/* PORT 1 */
		VPORT_I2C_Read((UINT32*)&glink_738); //VSI PB 0
		VPORT_I2C_Read((UINT32*)&glink_737); //VSI PB 1
		VPORT_I2C_Read((UINT32*)&glink_736);
		VPORT_I2C_Read((UINT32*)&glink_735);
		VPORT_I2C_Read((UINT32*)&glink_734);
		VPORT_I2C_Read((UINT32*)&glink_733);
		VPORT_I2C_Read((UINT32*)&glink_732);
		VPORT_I2C_Read((UINT32*)&glink_731);
		VPORT_I2C_Read((UINT32*)&glink_730);
		VPORT_I2C_Read((UINT32*)&glink_729);
		VPORT_I2C_Read((UINT32*)&glink_728);
		VPORT_I2C_Read((UINT32*)&glink_727);
		VPORT_I2C_Read((UINT32*)&glink_726);
		VPORT_I2C_Read((UINT32*)&glink_725);
		VPORT_I2C_Read((UINT32*)&glink_724);
		VPORT_I2C_Read((UINT32*)&glink_723);
		VPORT_I2C_Read((UINT32*)&glink_722);
		VPORT_I2C_Read((UINT32*)&glink_721);
		VPORT_I2C_Read((UINT32*)&glink_720);
		VPORT_I2C_Read((UINT32*)&glink_719);
		VPORT_I2C_Read((UINT32*)&glink_718);
		VPORT_I2C_Read((UINT32*)&glink_717);
		VPORT_I2C_Read((UINT32*)&glink_716);
		VPORT_I2C_Read((UINT32*)&glink_715);
		VPORT_I2C_Read((UINT32*)&glink_714);
		VPORT_I2C_Read((UINT32*)&glink_713);
		VPORT_I2C_Read((UINT32*)&glink_712);
		VPORT_I2C_Read((UINT32*)&glink_711); //VSI PB 27

		VPORT_I2C_Read((UINT32*)&glink_710); //VSI HB 1
		VPORT_I2C_Read((UINT32*)&glink_709); //VSI HB 2

		///< 24 bit IEEE Registration Identifier[3]
		packet->eVsiPacket.IEERegId[0] = glink_737.reg_pkt_vsi_dat_pip1_1;
		packet->eVsiPacket.IEERegId[1] = glink_736.reg_pkt_vsi_dat_pip1_2;
		packet->eVsiPacket.IEERegId[2] = glink_735.reg_pkt_vsi_dat_pip1_3;

		///< Payload  [24]
		packet->eVsiPacket.PayLoad[0] = glink_734.reg_pkt_vsi_dat_pip1_4;
		packet->eVsiPacket.PayLoad[1] = glink_733.reg_pkt_vsi_dat_pip1_5;
		packet->eVsiPacket.PayLoad[2] = glink_732.reg_pkt_vsi_dat_pip1_6;
		packet->eVsiPacket.PayLoad[3] = glink_731.reg_pkt_vsi_dat_pip1_7;
		packet->eVsiPacket.PayLoad[4] = glink_730.reg_pkt_vsi_dat_pip1_8;
		packet->eVsiPacket.PayLoad[5] = glink_729.reg_pkt_vsi_dat_pip1_9;
		packet->eVsiPacket.PayLoad[6] = glink_728.reg_pkt_vsi_dat_pip1_10;
		packet->eVsiPacket.PayLoad[7] = glink_727.reg_pkt_vsi_dat_pip1_11;
		packet->eVsiPacket.PayLoad[8] = glink_726.reg_pkt_vsi_dat_pip1_12;
		packet->eVsiPacket.PayLoad[9] = glink_725.reg_pkt_vsi_dat_pip1_13;
		packet->eVsiPacket.PayLoad[10] = glink_724.reg_pkt_vsi_dat_pip1_14;
		packet->eVsiPacket.PayLoad[11] = glink_723.reg_pkt_vsi_dat_pip1_15;
		packet->eVsiPacket.PayLoad[12] = glink_722.reg_pkt_vsi_dat_pip1_16;
		packet->eVsiPacket.PayLoad[13] = glink_721.reg_pkt_vsi_dat_pip1_17;
		packet->eVsiPacket.PayLoad[14] = glink_720.reg_pkt_vsi_dat_pip1_18;
		packet->eVsiPacket.PayLoad[15] = glink_719.reg_pkt_vsi_dat_pip1_19;
		packet->eVsiPacket.PayLoad[16] = glink_718.reg_pkt_vsi_dat_pip1_20;
		packet->eVsiPacket.PayLoad[17] = glink_717.reg_pkt_vsi_dat_pip1_21;
		packet->eVsiPacket.PayLoad[18] = glink_716.reg_pkt_vsi_dat_pip1_22;
		packet->eVsiPacket.PayLoad[19] = glink_715.reg_pkt_vsi_dat_pip1_23;
		packet->eVsiPacket.PayLoad[20] = glink_714.reg_pkt_vsi_dat_pip1_24;
		packet->eVsiPacket.PayLoad[21] = glink_713.reg_pkt_vsi_dat_pip1_25;
		packet->eVsiPacket.PayLoad[22] = glink_712.reg_pkt_vsi_dat_pip1_26;
		packet->eVsiPacket.PayLoad[23] = glink_711.reg_pkt_vsi_dat_pip1_27;

		/**< HDMI VSI info */
		tmp8 = glink_734.reg_pkt_vsi_dat_pip1_4;
		packet->eVsiPacket.eVideoFormat = (tmp8 & 0xE0) >> 5;

		/* if videoFormat == 0, no more data */
		/* if videoFormat == 1, extended resolution present */
		packet->eVsiPacket.eVSiVIC = glink_733.reg_pkt_vsi_dat_pip1_5;
		tmp8 = glink_732.reg_pkt_vsi_dat_pip1_6;
		packet->eVsiPacket.e3DExtData = (tmp8 & 0xF0) >> 4;
		/* if videoFormat == 2, 3D Format indication with 3d structure */
		tmp8 = glink_733.reg_pkt_vsi_dat_pip1_5;
		packet->eVsiPacket.e3DStructure = (tmp8 & 0xF0) >> 4;

		///< Packet raw data
		packet->eVsiPacket.packet.type = 0x81;
		packet->eVsiPacket.packet.version = glink_710.reg_pkt_vsi_hdr_pip1_0;
		packet->eVsiPacket.packet.length = glink_709.reg_pkt_vsi_hdr_pip1_1 & 0x1F;
		packet->eVsiPacket.packet.dataBytes[0] = glink_738.reg_pkt_vsi_dat_pip1_0;
		packet->eVsiPacket.packet.dataBytes[1] = glink_737.reg_pkt_vsi_dat_pip1_1;
		packet->eVsiPacket.packet.dataBytes[2] = glink_736.reg_pkt_vsi_dat_pip1_2;
		packet->eVsiPacket.packet.dataBytes[3] = glink_735.reg_pkt_vsi_dat_pip1_3;
		packet->eVsiPacket.packet.dataBytes[4] = glink_734.reg_pkt_vsi_dat_pip1_4;
		packet->eVsiPacket.packet.dataBytes[5] = glink_733.reg_pkt_vsi_dat_pip1_5;
		packet->eVsiPacket.packet.dataBytes[6] = glink_732.reg_pkt_vsi_dat_pip1_6;
		packet->eVsiPacket.packet.dataBytes[7] = glink_731.reg_pkt_vsi_dat_pip1_7;
		packet->eVsiPacket.packet.dataBytes[8] = glink_730.reg_pkt_vsi_dat_pip1_8;
		packet->eVsiPacket.packet.dataBytes[9] = glink_729.reg_pkt_vsi_dat_pip1_9;
		packet->eVsiPacket.packet.dataBytes[10] = glink_728.reg_pkt_vsi_dat_pip1_10;
		packet->eVsiPacket.packet.dataBytes[11] = glink_727.reg_pkt_vsi_dat_pip1_11;
		packet->eVsiPacket.packet.dataBytes[12] = glink_726.reg_pkt_vsi_dat_pip1_12;
		packet->eVsiPacket.packet.dataBytes[13] = glink_725.reg_pkt_vsi_dat_pip1_13;
		packet->eVsiPacket.packet.dataBytes[14] = glink_724.reg_pkt_vsi_dat_pip1_14;
		packet->eVsiPacket.packet.dataBytes[15] = glink_723.reg_pkt_vsi_dat_pip1_15;
		packet->eVsiPacket.packet.dataBytes[16] = glink_722.reg_pkt_vsi_dat_pip1_16;
		packet->eVsiPacket.packet.dataBytes[17] = glink_721.reg_pkt_vsi_dat_pip1_17;
		packet->eVsiPacket.packet.dataBytes[18] = glink_720.reg_pkt_vsi_dat_pip1_18;
		packet->eVsiPacket.packet.dataBytes[19] = glink_719.reg_pkt_vsi_dat_pip1_19;
		packet->eVsiPacket.packet.dataBytes[20] = glink_718.reg_pkt_vsi_dat_pip1_20;
		packet->eVsiPacket.packet.dataBytes[21] = glink_717.reg_pkt_vsi_dat_pip1_21;
		packet->eVsiPacket.packet.dataBytes[22] = glink_716.reg_pkt_vsi_dat_pip1_22;
		packet->eVsiPacket.packet.dataBytes[23] = glink_715.reg_pkt_vsi_dat_pip1_23;
		packet->eVsiPacket.packet.dataBytes[24] = glink_714.reg_pkt_vsi_dat_pip1_24;
		packet->eVsiPacket.packet.dataBytes[25] = glink_713.reg_pkt_vsi_dat_pip1_25;
		packet->eVsiPacket.packet.dataBytes[26] = glink_712.reg_pkt_vsi_dat_pip1_26;
		packet->eVsiPacket.packet.dataBytes[27] = glink_711.reg_pkt_vsi_dat_pip1_27;

		packet->eVsiPacket.packetStatus = LX_HDMI_VSI_PACKET_STATUS_UPDATED;
	}

	return ret;
}

/**
* HDMI_L9Ax_GetAviPacket
*
* @parm LX_HDMI_AVI_PACKET_T
* @return int
*/
int HDMI_L9Ax_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet)
{
	int ret = 0;
	UINT8 tmp8 = 0;
	UINT8 tmp16 = 0;

	HDMI_PRINT("%s: Get AVI Packet", __func__);

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* PORT 0 */
		VPORT_I2C_Read((UINT32*)&glink_120); //AVI data 0
		VPORT_I2C_Read((UINT32*)&glink_119);
		VPORT_I2C_Read((UINT32*)&glink_118);
		VPORT_I2C_Read((UINT32*)&glink_117);
		VPORT_I2C_Read((UINT32*)&glink_116);
		VPORT_I2C_Read((UINT32*)&glink_115);
		VPORT_I2C_Read((UINT32*)&glink_114);
		VPORT_I2C_Read((UINT32*)&glink_113);
		VPORT_I2C_Read((UINT32*)&glink_112);
		VPORT_I2C_Read((UINT32*)&glink_111);
		VPORT_I2C_Read((UINT32*)&glink_110);
		VPORT_I2C_Read((UINT32*)&glink_109);
		VPORT_I2C_Read((UINT32*)&glink_108);
		VPORT_I2C_Read((UINT32*)&glink_107);
		VPORT_I2C_Read((UINT32*)&glink_106);
		VPORT_I2C_Read((UINT32*)&glink_105);
		VPORT_I2C_Read((UINT32*)&glink_104);
		VPORT_I2C_Read((UINT32*)&glink_103);
		VPORT_I2C_Read((UINT32*)&glink_102);
		VPORT_I2C_Read((UINT32*)&glink_101);
		VPORT_I2C_Read((UINT32*)&glink_100);
		VPORT_I2C_Read((UINT32*)&glink_99);
		VPORT_I2C_Read((UINT32*)&glink_98);
		VPORT_I2C_Read((UINT32*)&glink_97);
		VPORT_I2C_Read((UINT32*)&glink_96);
		VPORT_I2C_Read((UINT32*)&glink_95);
		VPORT_I2C_Read((UINT32*)&glink_94);
		VPORT_I2C_Read((UINT32*)&glink_93); //AVI data 27

		VPORT_I2C_Read((UINT32*)&glink_92); //AVI HB 1
		VPORT_I2C_Read((UINT32*)&glink_91); //AVI HB 2

		//Y1Y0
		tmp8 = glink_119.reg_pkt_avi_dat_pip0_1;
		packet->eAviPacket.ePixelEncoding = (tmp8& 0x60)>>5;

		//A0
		tmp8 = glink_119.reg_pkt_avi_dat_pip0_1;
		packet->eAviPacket.eActiveInfo = (tmp8& 0x40)>>4;

		//B1B0
		tmp8 = glink_119.reg_pkt_avi_dat_pip0_1;
		packet->eAviPacket.eBarInfo = (tmp8& 0x0C)>>2;

		//S1S0
		tmp8 = glink_119.reg_pkt_avi_dat_pip0_1;
		packet->eAviPacket.eScanInfo = (tmp8& 0x03);

		//C1C0
		tmp8 = glink_118.reg_pkt_avi_dat_pip0_2;
		packet->eAviPacket.eColorimetry = (tmp8& 0xC0)>>6;

		//M1M0
		tmp8 = glink_118.reg_pkt_avi_dat_pip0_2;
		packet->eAviPacket.ePictureAspectRatio = (tmp8& 0x30)>>4;

		//R3R2R1R0
		tmp8 = glink_118.reg_pkt_avi_dat_pip0_2;
		packet->eAviPacket.eActiveFormatAspectRatio = (tmp8& 0x0F);

		//ITC
		tmp8 = glink_117.reg_pkt_avi_dat_pip0_3;
		packet->eAviPacket.eITContent = (tmp8& 0x80)>>7;

		//EC2EC1EC0
		tmp8 = glink_117.reg_pkt_avi_dat_pip0_3;
		packet->eAviPacket.eExtendedColorimetry = (tmp8& 0x70)>>4;

		//Q1Q0
		tmp8 = glink_117.reg_pkt_avi_dat_pip0_3;
		packet->eAviPacket.eRGBQuantizationRange = (tmp8& 0x0C)>>2;

		//SC1SC0
		tmp8 = glink_117.reg_pkt_avi_dat_pip0_3;
		packet->eAviPacket.eScaling = (tmp8& 0x03);

		//VIC6~VIC0
		tmp8 = glink_116.reg_pkt_avi_dat_pip0_4;
		packet->eAviPacket.VideoIdCode = (tmp8& 0x7F);

		//YQ1YQ0
		tmp8 = glink_115.reg_pkt_avi_dat_pip0_5;
		packet->eAviPacket.eYCCQuantizationRange = (tmp8& 0xC0)>>6;

		//CN1CN0
		tmp8 = glink_115.reg_pkt_avi_dat_pip0_5;
		packet->eAviPacket.eContentType = (tmp8& 0x30)>>4;

		//PR3PR2PR1PR0
		tmp8 = glink_115.reg_pkt_avi_dat_pip0_5;
		packet->eAviPacket.PixelRepeat = (tmp8& 0x0F);

		//Line Number of End of Top Bar
		tmp16 = glink_113.reg_pkt_avi_dat_pip0_7;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_114.reg_pkt_avi_dat_pip0_6;
		packet->eAviPacket.TopBarEndLineNumber = tmp16;

		//Line Number of Start of Bottom Bar
		tmp16 = glink_111.reg_pkt_avi_dat_pip0_9;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_112.reg_pkt_avi_dat_pip0_8;
		packet->eAviPacket.BottomBarStartLineNumber = tmp16;

		//Pixel Number of End of Left Bar
		tmp16 = glink_109.reg_pkt_avi_dat_pip0_11;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_110.reg_pkt_avi_dat_pip0_10;
		packet->eAviPacket.LeftBarEndPixelNumber = tmp16;

		//Pixel Number of Start of Right Bar
		tmp16 = glink_107.reg_pkt_avi_dat_pip0_13;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_108.reg_pkt_avi_dat_pip0_12;
		packet->eAviPacket.RightBarEndPixelNumber = tmp16;

		packet->eAviPacket.packet.type = 0x82;
		packet->eAviPacket.packet.version = glink_92.reg_pkt_avi_hdr_pip0_0;
		packet->eAviPacket.packet.length = glink_91.reg_pkt_avi_hdr_pip0_1 & 0x1F;

		packet->eAviPacket.packet.dataBytes[0] = glink_120.reg_pkt_avi_dat_pip0_0;
		packet->eAviPacket.packet.dataBytes[1] = glink_119.reg_pkt_avi_dat_pip0_1;
		packet->eAviPacket.packet.dataBytes[2] = glink_118.reg_pkt_avi_dat_pip0_2;
		packet->eAviPacket.packet.dataBytes[3] = glink_117.reg_pkt_avi_dat_pip0_3;
		packet->eAviPacket.packet.dataBytes[4] = glink_116.reg_pkt_avi_dat_pip0_4;
		packet->eAviPacket.packet.dataBytes[5] = glink_115.reg_pkt_avi_dat_pip0_5;
		packet->eAviPacket.packet.dataBytes[6] = glink_114.reg_pkt_avi_dat_pip0_6;
		packet->eAviPacket.packet.dataBytes[7] = glink_113.reg_pkt_avi_dat_pip0_7;
		packet->eAviPacket.packet.dataBytes[8] = glink_112.reg_pkt_avi_dat_pip0_8;
		packet->eAviPacket.packet.dataBytes[9] = glink_111.reg_pkt_avi_dat_pip0_9;
		packet->eAviPacket.packet.dataBytes[10] = glink_110.reg_pkt_avi_dat_pip0_10;
		packet->eAviPacket.packet.dataBytes[11] = glink_109.reg_pkt_avi_dat_pip0_11;
		packet->eAviPacket.packet.dataBytes[12] = glink_108.reg_pkt_avi_dat_pip0_12;
		packet->eAviPacket.packet.dataBytes[13] = glink_107.reg_pkt_avi_dat_pip0_13;
		packet->eAviPacket.packet.dataBytes[14] = glink_106.reg_pkt_avi_dat_pip0_14;
		packet->eAviPacket.packet.dataBytes[15] = glink_105.reg_pkt_avi_dat_pip0_15;
		packet->eAviPacket.packet.dataBytes[16] = glink_104.reg_pkt_avi_dat_pip0_16;
		packet->eAviPacket.packet.dataBytes[17] = glink_103.reg_pkt_avi_dat_pip0_17;
		packet->eAviPacket.packet.dataBytes[18] = glink_102.reg_pkt_avi_dat_pip0_18;
		packet->eAviPacket.packet.dataBytes[19] = glink_101.reg_pkt_avi_dat_pip0_19;
		packet->eAviPacket.packet.dataBytes[20] = glink_100.reg_pkt_avi_dat_pip0_20;
		packet->eAviPacket.packet.dataBytes[21] = glink_99.reg_pkt_avi_dat_pip0_21;
		packet->eAviPacket.packet.dataBytes[22] = glink_98.reg_pkt_avi_dat_pip0_22;
		packet->eAviPacket.packet.dataBytes[23] = glink_97.reg_pkt_avi_dat_pip0_23;
		packet->eAviPacket.packet.dataBytes[24] = glink_96.reg_pkt_avi_dat_pip0_24;
		packet->eAviPacket.packet.dataBytes[25] = glink_95.reg_pkt_avi_dat_pip0_25;
		packet->eAviPacket.packet.dataBytes[26] = glink_94.reg_pkt_avi_dat_pip0_26;
		packet->eAviPacket.packet.dataBytes[27] = glink_93.reg_pkt_avi_dat_pip0_27;

		packet->eAviPacket.bHdmiMode = gBootData->mode;
	}
	else
	{
		/* PORT 1 */
		VPORT_I2C_Read((UINT32*)&glink_524); //AVI PB 0
		VPORT_I2C_Read((UINT32*)&glink_523); //AVI PB 1
		VPORT_I2C_Read((UINT32*)&glink_522);
		VPORT_I2C_Read((UINT32*)&glink_521);
		VPORT_I2C_Read((UINT32*)&glink_520);
		VPORT_I2C_Read((UINT32*)&glink_519);
		VPORT_I2C_Read((UINT32*)&glink_518);
		VPORT_I2C_Read((UINT32*)&glink_517);
		VPORT_I2C_Read((UINT32*)&glink_516);
		VPORT_I2C_Read((UINT32*)&glink_515);
		VPORT_I2C_Read((UINT32*)&glink_514);
		VPORT_I2C_Read((UINT32*)&glink_513);
		VPORT_I2C_Read((UINT32*)&glink_512);
		VPORT_I2C_Read((UINT32*)&glink_511);
		VPORT_I2C_Read((UINT32*)&glink_510);
		VPORT_I2C_Read((UINT32*)&glink_509);
		VPORT_I2C_Read((UINT32*)&glink_508);
		VPORT_I2C_Read((UINT32*)&glink_507);
		VPORT_I2C_Read((UINT32*)&glink_506);
		VPORT_I2C_Read((UINT32*)&glink_505);
		VPORT_I2C_Read((UINT32*)&glink_504);
		VPORT_I2C_Read((UINT32*)&glink_503);
		VPORT_I2C_Read((UINT32*)&glink_502);
		VPORT_I2C_Read((UINT32*)&glink_501);
		VPORT_I2C_Read((UINT32*)&glink_500);
		VPORT_I2C_Read((UINT32*)&glink_499);
		VPORT_I2C_Read((UINT32*)&glink_498);
		VPORT_I2C_Read((UINT32*)&glink_497); //AVI PB 27

		VPORT_I2C_Read((UINT32*)&glink_496); //AVI HB 1
		VPORT_I2C_Read((UINT32*)&glink_495); //AVI HB 2

		//Y1Y0
		tmp8 = glink_523.reg_pkt_avi_dat_pip1_1;
		packet->eAviPacket.ePixelEncoding = (tmp8& 0x60)>>5;

		//A0
		tmp8 = glink_523.reg_pkt_avi_dat_pip1_1;
		packet->eAviPacket.eActiveInfo = (tmp8& 0x40)>>4;

		//B1B0
		tmp8 = glink_523.reg_pkt_avi_dat_pip1_1;
		packet->eAviPacket.eBarInfo = (tmp8& 0x0C)>>2;

		//S1S0
		tmp8 = glink_523.reg_pkt_avi_dat_pip1_1;
		packet->eAviPacket.eScanInfo = (tmp8& 0x03);

		//C1C0
		tmp8 = glink_522.reg_pkt_avi_dat_pip1_2;
		packet->eAviPacket.eColorimetry = (tmp8& 0xC0)>>6;

		//M1M0
		tmp8 = glink_522.reg_pkt_avi_dat_pip1_2;
		packet->eAviPacket.ePictureAspectRatio = (tmp8& 0x30)>>4;

		//R3R2R1R0
		tmp8 = glink_522.reg_pkt_avi_dat_pip1_2;
		packet->eAviPacket.eActiveFormatAspectRatio = (tmp8& 0x0F);

		//ITC
		tmp8 = glink_521.reg_pkt_avi_dat_pip1_3;
		packet->eAviPacket.eITContent = (tmp8& 0x80)>>7;

		//EC2EC1EC0
		tmp8 = glink_521.reg_pkt_avi_dat_pip1_3;
		packet->eAviPacket.eExtendedColorimetry = (tmp8& 0x70)>>4;

		//Q1Q0
		tmp8 = glink_521.reg_pkt_avi_dat_pip1_3;
		packet->eAviPacket.eRGBQuantizationRange = (tmp8& 0x0C)>>2;

		//SC1SC0
		tmp8 = glink_521.reg_pkt_avi_dat_pip1_3;
		packet->eAviPacket.eScaling = (tmp8& 0x03);

		//VIC6~VIC0
		tmp8 = glink_520.reg_pkt_avi_dat_pip1_4;
		packet->eAviPacket.VideoIdCode = (tmp8& 0x7F);

		//YQ1YQ0
		tmp8 = glink_519.reg_pkt_avi_dat_pip1_5;
		packet->eAviPacket.eYCCQuantizationRange = (tmp8& 0xC0)>>6;

		//CN1CN0
		tmp8 = glink_519.reg_pkt_avi_dat_pip1_5;
		packet->eAviPacket.eContentType = (tmp8& 0x30)>>4;

		//PR3PR2PR1PR0
		tmp8 = glink_519.reg_pkt_avi_dat_pip1_5;
		packet->eAviPacket.PixelRepeat = (tmp8& 0x0F);

		//Line Number of End of Top Bar
		tmp16 = glink_517.reg_pkt_avi_dat_pip1_7;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_518.reg_pkt_avi_dat_pip1_6;
		packet->eAviPacket.TopBarEndLineNumber = tmp16;

		//Line Number of Start of Bottom Bar
		tmp16 = glink_515.reg_pkt_avi_dat_pip1_9;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_516.reg_pkt_avi_dat_pip1_8;
		packet->eAviPacket.BottomBarStartLineNumber = tmp16;

		//Pixel Number of End of Left Bar
		tmp16 = glink_513.reg_pkt_avi_dat_pip1_11;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_514.reg_pkt_avi_dat_pip1_10;
		packet->eAviPacket.LeftBarEndPixelNumber = tmp16;

		//Pixel Number of Start of Right Bar
		tmp16 = glink_511.reg_pkt_avi_dat_pip1_13;
		tmp16 = tmp16 << 8;
		tmp16 = tmp16 | glink_512.reg_pkt_avi_dat_pip1_12;
		packet->eAviPacket.RightBarEndPixelNumber = tmp16;

		packet->eAviPacket.packet.type = 0x82;
		packet->eAviPacket.packet.version = glink_496.reg_pkt_avi_hdr_pip1_0;
		packet->eAviPacket.packet.length = glink_495.reg_pkt_avi_hdr_pip1_1 & 0x1F;

		packet->eAviPacket.packet.dataBytes[0] = glink_524.reg_pkt_avi_dat_pip1_0;
		packet->eAviPacket.packet.dataBytes[1] = glink_523.reg_pkt_avi_dat_pip1_1;
		packet->eAviPacket.packet.dataBytes[2] = glink_522.reg_pkt_avi_dat_pip1_2;
		packet->eAviPacket.packet.dataBytes[3] = glink_521.reg_pkt_avi_dat_pip1_3;
		packet->eAviPacket.packet.dataBytes[4] = glink_520.reg_pkt_avi_dat_pip1_4;
		packet->eAviPacket.packet.dataBytes[5] = glink_519.reg_pkt_avi_dat_pip1_5;
		packet->eAviPacket.packet.dataBytes[6] = glink_518.reg_pkt_avi_dat_pip1_6;
		packet->eAviPacket.packet.dataBytes[7] = glink_517.reg_pkt_avi_dat_pip1_7;
		packet->eAviPacket.packet.dataBytes[8] = glink_516.reg_pkt_avi_dat_pip1_8;
		packet->eAviPacket.packet.dataBytes[9] = glink_515.reg_pkt_avi_dat_pip1_9;
		packet->eAviPacket.packet.dataBytes[10] = glink_514.reg_pkt_avi_dat_pip1_10;
		packet->eAviPacket.packet.dataBytes[11] = glink_513.reg_pkt_avi_dat_pip1_11;
		packet->eAviPacket.packet.dataBytes[12] = glink_512.reg_pkt_avi_dat_pip1_12;
		packet->eAviPacket.packet.dataBytes[13] = glink_511.reg_pkt_avi_dat_pip1_13;
		packet->eAviPacket.packet.dataBytes[14] = glink_510.reg_pkt_avi_dat_pip1_14;
		packet->eAviPacket.packet.dataBytes[15] = glink_509.reg_pkt_avi_dat_pip1_15;
		packet->eAviPacket.packet.dataBytes[16] = glink_508.reg_pkt_avi_dat_pip1_16;
		packet->eAviPacket.packet.dataBytes[17] = glink_507.reg_pkt_avi_dat_pip1_17;
		packet->eAviPacket.packet.dataBytes[18] = glink_506.reg_pkt_avi_dat_pip1_18;
		packet->eAviPacket.packet.dataBytes[19] = glink_505.reg_pkt_avi_dat_pip1_19;
		packet->eAviPacket.packet.dataBytes[20] = glink_504.reg_pkt_avi_dat_pip1_20;
		packet->eAviPacket.packet.dataBytes[21] = glink_503.reg_pkt_avi_dat_pip1_21;
		packet->eAviPacket.packet.dataBytes[22] = glink_502.reg_pkt_avi_dat_pip1_22;
		packet->eAviPacket.packet.dataBytes[23] = glink_501.reg_pkt_avi_dat_pip1_23;
		packet->eAviPacket.packet.dataBytes[24] = glink_500.reg_pkt_avi_dat_pip1_24;
		packet->eAviPacket.packet.dataBytes[25] = glink_499.reg_pkt_avi_dat_pip1_25;
		packet->eAviPacket.packet.dataBytes[26] = glink_498.reg_pkt_avi_dat_pip1_26;
		packet->eAviPacket.packet.dataBytes[27] = glink_497.reg_pkt_avi_dat_pip1_27;

		packet->eAviPacket.bHdmiMode = gBootData->mode;
	}

	return ret;
}

/**
* HDMI_L9Ax_GetSpdPacket
*
* @parm LX_HDMI_SPD_PACKET_T
* @return int
*/
int HDMI_L9Ax_GetSpdPacket(LX_HDMI_SPD_PACKET_T *packet)
{
	int ret = 0;

	HDMI_PRINT("%s: Get SPD Packet", __func__);

	if (!_gSCDT) { memset((void *)packet , 0 , sizeof(LX_HDMI_SPD_PACKET_T)); goto func_exit; }

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		/* PORT 0 */
		//VPORT_I2C_Read((UINT32*)&glink_244);  /* HDR_1 - Length of Source Product Description InfoFrame */
		//VPORT_I2C_Read((UINT32*)&glink_245);  /* HDR_0 - InfoFrame Version */
		//VPORT_I2C_Read((UINT32*)&glink_273);  /* Checksum */
		VPORT_I2C_Read((UINT32*)&glink_272);
		VPORT_I2C_Read((UINT32*)&glink_271);
		VPORT_I2C_Read((UINT32*)&glink_270);
		VPORT_I2C_Read((UINT32*)&glink_269);
		VPORT_I2C_Read((UINT32*)&glink_268);
		VPORT_I2C_Read((UINT32*)&glink_267);
		VPORT_I2C_Read((UINT32*)&glink_266);
		VPORT_I2C_Read((UINT32*)&glink_265);
		VPORT_I2C_Read((UINT32*)&glink_264);
		VPORT_I2C_Read((UINT32*)&glink_263);
		VPORT_I2C_Read((UINT32*)&glink_262);
		VPORT_I2C_Read((UINT32*)&glink_261);
		VPORT_I2C_Read((UINT32*)&glink_260);
		VPORT_I2C_Read((UINT32*)&glink_259);
		VPORT_I2C_Read((UINT32*)&glink_258);
		VPORT_I2C_Read((UINT32*)&glink_257);
		VPORT_I2C_Read((UINT32*)&glink_256);
		VPORT_I2C_Read((UINT32*)&glink_255);
		VPORT_I2C_Read((UINT32*)&glink_254);
		VPORT_I2C_Read((UINT32*)&glink_253);
		VPORT_I2C_Read((UINT32*)&glink_252);
		VPORT_I2C_Read((UINT32*)&glink_251);
		VPORT_I2C_Read((UINT32*)&glink_250);
		VPORT_I2C_Read((UINT32*)&glink_249);
		VPORT_I2C_Read((UINT32*)&glink_248);

		packet->VendorName[0] = glink_272.reg_pkt_spd_dat_pip0_1;
		packet->VendorName[1] = glink_271.reg_pkt_spd_dat_pip0_2;
		packet->VendorName[2] = glink_270.reg_pkt_spd_dat_pip0_3;
		packet->VendorName[3] = glink_269.reg_pkt_spd_dat_pip0_4;
		packet->VendorName[4] = glink_268.reg_pkt_spd_dat_pip0_5;
		packet->VendorName[5] = glink_267.reg_pkt_spd_dat_pip0_6;
		packet->VendorName[6] = glink_266.reg_pkt_spd_dat_pip0_7;
		packet->VendorName[7] = glink_265.reg_pkt_spd_dat_pip0_8;

		packet->ProductDescription[0] = glink_264.reg_pkt_spd_dat_pip0_9;
		packet->ProductDescription[1] = glink_263.reg_pkt_spd_dat_pip0_10;
		packet->ProductDescription[2] = glink_262.reg_pkt_spd_dat_pip0_11;
		packet->ProductDescription[3] = glink_261.reg_pkt_spd_dat_pip0_12;
		packet->ProductDescription[4] = glink_260.reg_pkt_spd_dat_pip0_13;
		packet->ProductDescription[5] = glink_259.reg_pkt_spd_dat_pip0_14;
		packet->ProductDescription[6] = glink_258.reg_pkt_spd_dat_pip0_15;
		packet->ProductDescription[7] = glink_257.reg_pkt_spd_dat_pip0_16;
		packet->ProductDescription[8] = glink_256.reg_pkt_spd_dat_pip0_17;
		packet->ProductDescription[9] = glink_255.reg_pkt_spd_dat_pip0_18;
		packet->ProductDescription[10] = glink_254.reg_pkt_spd_dat_pip0_19;
		packet->ProductDescription[11] = glink_253.reg_pkt_spd_dat_pip0_20;
		packet->ProductDescription[12] = glink_252.reg_pkt_spd_dat_pip0_21;
		packet->ProductDescription[13] = glink_251.reg_pkt_spd_dat_pip0_22;
		packet->ProductDescription[14] = glink_250.reg_pkt_spd_dat_pip0_23;
		packet->ProductDescription[15] = glink_249.reg_pkt_spd_dat_pip0_24;

		packet->SourceDeviceInfo = glink_248.reg_pkt_spd_dat_pip0_25;
	}
	else
	{
		/* PORT 1 */
		VPORT_I2C_Read((UINT32*)&glink_676);
		VPORT_I2C_Read((UINT32*)&glink_675);
		VPORT_I2C_Read((UINT32*)&glink_674);
		VPORT_I2C_Read((UINT32*)&glink_673);
		VPORT_I2C_Read((UINT32*)&glink_672);
		VPORT_I2C_Read((UINT32*)&glink_671);
		VPORT_I2C_Read((UINT32*)&glink_670);
		VPORT_I2C_Read((UINT32*)&glink_669);
		VPORT_I2C_Read((UINT32*)&glink_668);
		VPORT_I2C_Read((UINT32*)&glink_667);
		VPORT_I2C_Read((UINT32*)&glink_666);
		VPORT_I2C_Read((UINT32*)&glink_665);
		VPORT_I2C_Read((UINT32*)&glink_664);
		VPORT_I2C_Read((UINT32*)&glink_663);
		VPORT_I2C_Read((UINT32*)&glink_662);
		VPORT_I2C_Read((UINT32*)&glink_661);
		VPORT_I2C_Read((UINT32*)&glink_660);
		VPORT_I2C_Read((UINT32*)&glink_659);
		VPORT_I2C_Read((UINT32*)&glink_658);
		VPORT_I2C_Read((UINT32*)&glink_657);
		VPORT_I2C_Read((UINT32*)&glink_656);
		VPORT_I2C_Read((UINT32*)&glink_655);
		VPORT_I2C_Read((UINT32*)&glink_654);
		VPORT_I2C_Read((UINT32*)&glink_653);
		VPORT_I2C_Read((UINT32*)&glink_652);

		packet->VendorName[0] = glink_676.reg_pkt_spd_dat_pip1_1;
		packet->VendorName[1] = glink_675.reg_pkt_spd_dat_pip1_2;
		packet->VendorName[2] = glink_674.reg_pkt_spd_dat_pip1_3;
		packet->VendorName[3] = glink_673.reg_pkt_spd_dat_pip1_4;
		packet->VendorName[4] = glink_672.reg_pkt_spd_dat_pip1_5;
		packet->VendorName[5] = glink_671.reg_pkt_spd_dat_pip1_6;
		packet->VendorName[6] = glink_670.reg_pkt_spd_dat_pip1_7;
		packet->VendorName[7] = glink_669.reg_pkt_spd_dat_pip1_8;

		packet->ProductDescription[0] = glink_668.reg_pkt_spd_dat_pip1_9;
		packet->ProductDescription[1] = glink_667.reg_pkt_spd_dat_pip1_10;
		packet->ProductDescription[2] = glink_666.reg_pkt_spd_dat_pip1_11;
		packet->ProductDescription[3] = glink_665.reg_pkt_spd_dat_pip1_12;
		packet->ProductDescription[4] = glink_664.reg_pkt_spd_dat_pip1_13;
		packet->ProductDescription[5] = glink_663.reg_pkt_spd_dat_pip1_14;
		packet->ProductDescription[6] = glink_662.reg_pkt_spd_dat_pip1_15;
		packet->ProductDescription[7] = glink_661.reg_pkt_spd_dat_pip1_16;
		packet->ProductDescription[8] = glink_660.reg_pkt_spd_dat_pip1_17;
		packet->ProductDescription[9] = glink_659.reg_pkt_spd_dat_pip1_18;
		packet->ProductDescription[10] = glink_658.reg_pkt_spd_dat_pip1_19;
		packet->ProductDescription[11] = glink_657.reg_pkt_spd_dat_pip1_20;
		packet->ProductDescription[12] = glink_656.reg_pkt_spd_dat_pip1_21;
		packet->ProductDescription[13] = glink_655.reg_pkt_spd_dat_pip1_22;
		packet->ProductDescription[14] = glink_654.reg_pkt_spd_dat_pip1_23;
		packet->ProductDescription[15] = glink_653.reg_pkt_spd_dat_pip1_24;

		packet->SourceDeviceInfo = glink_652.reg_pkt_spd_dat_pip1_25;
	}

#if 0
	HDMI_DEBUG("VendorName: [%s]\n", packet->VendorName);
	HDMI_DEBUG("ProductDes: [%s]\n", packet->ProductDescription);
	HDMI_DEBUG("SourceInfo: [%d]\n", packet->SourceDeviceInfo);
#endif

func_exit:
	packet->srcIdx = LX_HDMI_MAIN_WINDOW;

	return ret;
}

/**
* HDMI_L9Ax_SetHPDEnDisable
*
* @parm LX_HDMI_HPD_T
* @return int
*/
int HDMI_L9Ax_SetHPDEnDisable(LX_HDMI_HPD_T *hpd)
{
	int ret = 0;

	VPORT_I2C_Read((UINT32*)&ghpd_out_control_0);
	if(hpd->u8HDMIPort == 1)
	{
		/* HDMI 0 */
		ghpd_out_control_0.hpd0_out = 1;
		ghpd_out_control_0.hpd0_oen = 1;
		ghpd_out_control_0.hpd1_out = 0;
		ghpd_out_control_0.hpd1_oen = 0;
	}
	else
	{
		/* HDMI 1 */
		ghpd_out_control_0.hpd0_out = 0;
		ghpd_out_control_0.hpd0_oen = 0;
		ghpd_out_control_0.hpd1_out = 1;
		ghpd_out_control_0.hpd1_oen = 1;
	}
	VPORT_I2C_Write((UINT32*)&ghpd_out_control_0);

	return ret;
}

/**
* HDMI_L9Ax_SetPort
*
* @parm LX_HDMI_HPD_T
* @return int
*/
int HDMI_L9Ax_SetPort(LX_HDMI_HPD_T *hpd)
{
	int ret = 0;

	HDMI_PHY_TYPE_T type;

#ifdef	KDRV_CONFIG_PM
	memcpy( (void *)&gstPmPortSel , (void *)hpd , sizeof(LX_HDMI_HPD_T) );
#endif

//	if(gBootData->hdmi_switch)
// 		hpd->u8HDMIPort = gBootData->SWport;

	if(hpd->u8HDMIPort == 0)
	{
		/* HDMI 0 */
		/* HDMI Link Reset - Video Format Info.*/
		_HDMI_L9Ax_ClearResetLink(1);

		/* Port	Power down control */
		HDMI_SetPowerControl(HDMI_PHY0_PORT , 0);
		type = 	HDMI_PHY0_PORT;
		//OS_MsecSleep(300);	// ms delay

		// try to do power on without status check
		HDMI_SetPowerControl(type , 1);

		gBootData->pixelRepet = gBootData->mode = 0;
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
		_gHDMI_DVI_Done = _gHDMI_Change = 0;

		//OS_MsecSleep(50);
		// All Link Reset Clear
		_HDMI_L9Ax_ClearResetLink(0);
	}
	else
	{
		/* HDMI 1 */
		/* HDMI Link Reset - Video Format Info.*/
		_HDMI_L9Ax_ClearResetLink(1);

		/* Port	Power down control */
		HDMI_SetPowerControl(HDMI_PHY1_PORT , 0);
		type = 	HDMI_PHY1_PORT;
		//OS_MsecSleep(300);	// ms delay

		// try to do power on without status check
		HDMI_SetPowerControl(type , 1);

		gBootData->pixelRepet = gBootData->mode = 0;
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
		_gHDMI_DVI_Done = _gHDMI_Change = 0;

		//OS_MsecSleep(50);
		// All Link Reset Clear
		_HDMI_L9Ax_ClearResetLink(0);
	}

	return ret;
}

/**
* HDMI_L9Ax_SetPowerDown
*
* @parm HDMI_PHY_TYPE_T
* @return int
*/
int HDMI_L9Ax_SetPowerControl(HDMI_PHY_TYPE_T type , int power)
{
	int ret = 0;
	UINT8 	pdbAll[3]	= {0};
	UINT8 	resetAll[3] = {0};

	if(power == 0) // Power down
	{
		//pdbAll[0] = resetAll[0] = pdbAll[1] = resetAll[1] = 0;
		pdbAll[type] = resetAll[type] = 0;
	}
	else	// power up
	{
		/*
		if(type > HDMI_PHY1_PORT)
			pdbAll[0] = resetAll[0] = pdbAll[1] = resetAll[1] = 1;
		else
			pdbAll[type] = resetAll[type] = 1;
		*/
		pdbAll[type] = resetAll[type] = 1;
	}

	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_01);
	ghdmi_phy_n_01.resetb_all = resetAll[type];
	ghdmi_phy_n_01.pdb_all = pdbAll[type];
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_01);

	HDMI_PRINT("HDMI_L9Ax_SetPowerControl resetAll[%d]= %d, pdb_all[%d]= %d \n", type, resetAll[type], type, pdbAll[type]);

#if 0
	VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);

	if (port == HDMI_L9AX_PORT_1)
		ghpd_out_control_1.phy_enable = 1;		// PHY_1 I2C Enable
	else
		ghpd_out_control_1.phy_enable = 0;		// PHY_0 I2C Enable

	VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);

	VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
	VPORT_I2C_Read((UINT32*)&ghpd_out_control_2);

	if(port == HDMI_L9AX_PORT_0)	// PORT0 -> PHY0 -> LINK0
	{
		ghpd_out_control_1.phy_enable = 0;		// Phy I2C Enable : 0 -> phy0, 1 -> phy 1
		ghpd_out_control_1.phy0_phy_pdb = 1;
		ghpd_out_control_1.phy0_phy_rstn = 1;

		/* Port 1 Disable */
		ghpd_out_control_1.phy1_phy_pdb = 0;
		ghpd_out_control_2.phy1_phy_rstn = 0;
	}
	else		// PORT1 -> PHY1 -> LINK1
	{
		/* PORT1 -> PHY1 -> LINK1 */
		ghpd_out_control_1.phy_enable = 1;		// Phy I2C Enable : 0 -> phy0, 1 -> phy 1
		ghpd_out_control_1.phy1_phy_pdb = 1;
		ghpd_out_control_2.phy1_phy_rstn = 1;

		/* Port 0 Disable */
		ghpd_out_control_1.phy0_phy_pdb = 0;
		ghpd_out_control_1.phy0_phy_rstn = 0;
	}

	VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);
	VPORT_I2C_Write((UINT32*)&ghpd_out_control_2);
#endif
	return ret;
}

int HDMI_L9Ax_RunReset(void)
{
	// hdmi link Reset - All Clear
	_HDMI_L9Ax_ClearSWResetAll();

	// DR Capacitance Selection
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_82);
	ghdmi_phy_n_82.dr_mode = 1;
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_82);

	// PLL_Mode_Sel Auto Reset
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_12);
	ghdmi_phy_n_12.cr_test = 1;
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_12);

	// HDMI Current Default value 2(60uA)- Use 60uA for L9 by sunghyun.yang 2011-04-29
	//VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_25);
	//ghdmi_phy_n_25.idr_adj = 2;
	//VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_25);

	//HDMI_L9A0_ADAPTIVE_LOOP
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_41);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_43);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_44);

	ghdmi_phy_n_41.eq_cs_sel = 0;
	ghdmi_phy_n_41.eq_rs_sel = 0;
	ghdmi_phy_n_43.eq_rs = 3;
	ghdmi_phy_n_44.eq_cs = 3;

	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_41);
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_43);
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_44);

	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_45);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_46);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_47);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_48);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_4b);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_4c);

	ghdmi_phy_n_45.eq_thr_h = 6;
	ghdmi_phy_n_46.eq_thr_l = 12;
	ghdmi_phy_n_47.eq_tot_h = 7;
	ghdmi_phy_n_48.eq_tot_l = 15;
	ghdmi_phy_n_4b.eq_start_frz = 20;
	ghdmi_phy_n_4c.eq_avg_width = 0;		// HDCP Problem to the Master

	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_45);
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_46);
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_47);
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_48);
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_4b);
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_4c);

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		// Field select(short back Porch) for Interlace mode
		// for UD and 3D_SS_Full format
		VPORT_I2C_Read((UINT32*)&glink_345);
		glink_345.reg_lbp_f2_pip0 = 1;
		glink_345.reg_neg_pol_en_pip0 = 0;
		VPORT_I2C_Write((UINT32*)&glink_345);

		// HPD PIN Control Enable
		VPORT_I2C_Read((UINT32*)&ghpd_out_control_0);
		ghpd_out_control_0.hpd0_oen = 0;
		ghpd_out_control_0.hpd1_oen = 1;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_0);

		// Video Path Auto Configuration Enable(default = 1)
		//VPORT_I2C_Read((UINT32*)&glink_2);
		//glink_2.reg_avc_en_pip0 = 1;
		//VPORT_I2C_Read((UINT32*)&glink_2);

		// not used hdcp key ROM
		VPORT_I2C_Read((UINT32*)&glink_818);
		glink_818.reg_dbg_hdcp_key_bak = 0;
		VPORT_I2C_Write((UINT32*)&glink_818);
	}
	else
	{
		// Field select(short back Porch) for Interlace mode
		// for UD and 3D_SS_Full format
		VPORT_I2C_Read((UINT32*)&glink_749);
		glink_749.reg_lbp_f2_pip1 = 1;
		glink_749.reg_neg_pol_en_pip1 = 0;
		VPORT_I2C_Write((UINT32*)&glink_749);

		// HPD PIN Control Enable
		VPORT_I2C_Read((UINT32*)&ghpd_out_control_0);
		ghpd_out_control_0.hpd0_oen = 1;
		ghpd_out_control_0.hpd1_oen = 0;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_0);

		// Video Path Auto Configuration Enable(default = 1)
		//VPORT_I2C_Read((UINT32*)&glink_406);
		//glink_406.reg_avc_en_pip1 = 1;
		//VPORT_I2C_Read((UINT32*)&glink_406);

		// used hdcp key ROM
		VPORT_I2C_Read((UINT32*)&glink_818);
		glink_818.reg_dbg_hdcp_key_bak = 1;
		VPORT_I2C_Write((UINT32*)&glink_818);
	}

	/* ARC pdb Issue - L9A B0 fixed : sh.myoung 20110518*/
#ifdef L9A_A0_ARC_PDB
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_26);
	ghdmi_phy_n_26.odt_sel = 0;
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_26);
#endif

	return 0;
}

/**
* _HDMI_L9Ax_EnablePhy
* L9A0 Internal Phy and I2C Enable
* @parm HDMI_L9AX_PORT_T
* @return int
*/
int _HDMI_L9Ax_EnablePhy(HDMI_L9AX_PORT_T port)
{
	/* Default value : 0 */
	//VPORT_I2C_Read((UINT32*)&glink_0);
	//glink_0.reg_prt_sel_pip0 = 0;
	//VPORT_I2C_Write((UINT32*)&glink_0);

	/* L9 A0 Audio Issue */
	if(port == HDMI_L9AX_PORT_0)
	{
		VPORT_I2C_Read((UINT32*)&glink_404);
		glink_404.reg_prt_sel_pip1 = 0;
		VPORT_I2C_Write((UINT32*)&glink_404);
	}
	else
	{
		VPORT_I2C_Read((UINT32*)&glink_404);
		glink_404.reg_prt_sel_pip1 = 1;
		VPORT_I2C_Write((UINT32*)&glink_404);
	}

	// Internal I2C support upto 8Mhz
	//VPORT_I2C_Read((UINT32*)&ghostif_control_0);
	//ghostif_control_0.reg_sel_afe3ch_ext_clock = 0;
	//ghostif_control_0.reg_sel_hostif_clock = 1;
	//ghostif_control_0.ro_hostif_is_162mhz = 1;
	//VPORT_I2C_Write((UINT32*)&ghostif_control_0);

	VPORT_I2C_Read((UINT32*)&ghpd_out_control_2);
	VPORT_I2C_Read((UINT32*)&gaudio_pll_5);

	if(port == HDMI_L9AX_PORT_0)
	{
		/* PHY0_PHY_PDB=1, PHY0_PHY_RSTN=1 을 동시에 인가시
		 * 내부 Regulator가 Final Value까지 올라가기도 전에 RSTN이 풀리므로
		 * Regulator를 전원으로 사용하는 Register들이 제대로 pre-set되지 않는 현상이 생기는 것으로 판단됨. by sunghyun.yang 2011.06.14
		 */

		/* PORT0 -> PHY0 -> LINK0 and Port 1 Disable */
		VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
		ghpd_out_control_1.phy_enable = 0;		// Phy I2C Enable : 0 -> phy0, 1 -> phy 1
		ghpd_out_control_1.phy0_phy_pdb = 1;
		ghpd_out_control_1.phy1_phy_pdb = 0;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);

		OS_MsecSleep(5);	// ms delay

		VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
		VPORT_I2C_Read((UINT32*)&ghpd_out_control_2);
		ghpd_out_control_1.phy0_phy_rstn = 1;
		ghpd_out_control_2.phy1_phy_rstn = 0;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_2);

		/* WARN!!! If hdmiphy1_sdaout_enable is set to 1
		 * with powerdown mode off, it will affect other blocks.
		 */
		gaudio_pll_5.hdmiphy1_sdaout_enable = 0;
		gaudio_pll_5.hdmiphy0_sdaout_enable = 1;
		HDMI_ERROR("HDMI_L9Ax_EnablePhy  = %d \n",  _gui32CurPort);
	}
	else
	{
		/* PORT1 -> PHY1 -> LINK1 and  Port 0 Disable */
		VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
		ghpd_out_control_1.phy_enable = 1;		// Phy I2C Enable : 0 -> phy0, 1 -> phy 1
		ghpd_out_control_1.phy1_phy_pdb = 1;
		ghpd_out_control_1.phy0_phy_pdb = 0;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);

		OS_MsecSleep(5);	// ms delay

		VPORT_I2C_Read((UINT32*)&ghpd_out_control_2);
		VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
		ghpd_out_control_2.phy1_phy_rstn = 1;
		ghpd_out_control_1.phy0_phy_rstn = 0;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_2);
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);

		/* WARN!!! If hdmiphy1_sdaout_enable is set to 1
		 * with powerdown mode off, it will affect other blocks.
		 */
		gaudio_pll_5.hdmiphy1_sdaout_enable = 1;
		gaudio_pll_5.hdmiphy0_sdaout_enable = 0;
		HDMI_ERROR("HDMI_L9Ax_EnablePhy  = %d \n",  _gui32CurPort);
	}

	VPORT_I2C_Write((UINT32*)&gaudio_pll_5);
	return 0;
}


static void _HDMI_L9Ax_EnableLVDS(void)
{
	VPORT_I2C_Read((UINT32*)&ghslvdstx0_0);
	VPORT_I2C_Read((UINT32*)&ghslvdstx0_1);
	VPORT_I2C_Read((UINT32*)&ghslvdstx1_0);
	VPORT_I2C_Read((UINT32*)&ghslvdstx1_1);
	VPORT_I2C_Read((UINT32*)&ghslvdstx1_2);
	VPORT_I2C_Read((UINT32*)&gsoft_reset_7);

	ghslvdstx0_0.pdb1 = 1;
	ghslvdstx0_1.flip_en1 = 0;
	ghslvdstx0_1.ch_en1 = 0x3f;

	ghslvdstx1_0.pdb2 = 1;
	ghslvdstx1_1.flip_en2 = 0;
	ghslvdstx1_1.ch_en2 = 0x3f;
	ghslvdstx1_2.rs2 = 0x3f;
	ghslvdstx1_2.rf2 = 0;

	gsoft_reset_7.swrst_hs1_lvds = 0;
	gsoft_reset_7.swrst_hs0_lvds = 0;

	VPORT_I2C_Write((UINT32*)&ghslvdstx0_0);
	VPORT_I2C_Write((UINT32*)&ghslvdstx0_1);
	VPORT_I2C_Write((UINT32*)&ghslvdstx1_0);
	VPORT_I2C_Write((UINT32*)&ghslvdstx1_1);
	VPORT_I2C_Write((UINT32*)&ghslvdstx1_2);
	VPORT_I2C_Write((UINT32*)&gsoft_reset_7);
}

int HDMI_L9Ax_HWInitial(void)
{
	int ret = -1;

	/* L9 A0 Internal Port Set */
	if(gBootData->hdmi_switch)
		_gui32CurPort = HDMI_L9AX_PORT_0;
	else
		_gui32CurPort = HDMI_L9AX_PORT_1;

	HDMI_ERROR("HDMI_L9Ax_HWInitial  = %d _gui32CurPort = %d\n", HDMI_L9AX_PORT_0  , _gui32CurPort);

	_HDMI_L9Ax_EnablePhy(_gui32CurPort);	// Enable HDMI Internal PHY and I2C for L9A
	_HDMI_L9Ax_EnableLVDS( );

	HDMI_RunReset();

	// audio setting
	HDMI_SetAudio();

	return ret;
}

static int _HDMI_L9Ax_Set3DFullTimingInfo(LX_HDMI_TIMING_INFO_T *info)
{
	int ret = 0;
	UINT8 tmp8 = 0, vic = 0;

	LX_HDMI_TIMING_INFO_T 	bufTiming = {0,};
	LX_HDMI_VSI_VIDEO_FORMAT_T eVideoFormat;	/**< HDMI VSI info */
	LX_HDMI_VSI_3D_STRUCTURE_T e3DStructure;	/**< HDMI VSI info */

	info->full_3d_timing = LX_HDMI_NORMAL_TIMING;

	memcpy(&bufTiming , info , sizeof(LX_HDMI_TIMING_INFO_T));

	/**< HDMI VSI info */
	VPORT_I2C_Read((UINT32*)&glink_330);
	VPORT_I2C_Read((UINT32*)&glink_329);

	tmp8 = glink_330.reg_pkt_vsi_dat_pip0_4;
	eVideoFormat = (tmp8 & 0xE0) >> 5;

	tmp8 = glink_329.reg_pkt_vsi_dat_pip0_5;
	e3DStructure = (tmp8 & 0xF0) >> 4;

	HDMI_PRINT(" Timinginfo   eVideoFormat = %d //  buf e3DStructure = %d \n\n", eVideoFormat, e3DStructure);

	if(eVideoFormat == LX_FORMAT_NO_ADDITIONAL_FORMAT)
	{
		bufTiming.full_3d_timing = LX_HDMI_NORMAL_TIMING;		///< 2D format

		if( (bufTiming.hActive == 1280)	&& 	(bufTiming.vActive == 1470) && (bufTiming.scanType == 1))
		{
			bufTiming.vActive = 720;
			bufTiming.full_3d_timing = LX_HDMI_3D_FRAMEPACK;
		}

		if( (bufTiming.hActive == 1920)	&& 	(bufTiming.vActive == 2228) && (bufTiming.scanType == 1))
		{
			bufTiming.vActive = 1080;
			bufTiming.scanType = 0;
			bufTiming.full_3d_timing = LX_HDMI_3D_FRAMEPACK;
		}

		if( (bufTiming.hActive == 1920)	&& 	(bufTiming.vActive == 2205) && (bufTiming.scanType == 1))
		{
			bufTiming.vActive = 1080;
			bufTiming.full_3d_timing = LX_HDMI_3D_FRAMEPACK;
		}

		if( (bufTiming.hActive == 1920)	&& 	(bufTiming.vActive == 1103) && (bufTiming.scanType == 1))
		{
			bufTiming.vActive = 1080;
			bufTiming.scanType = 0;
			bufTiming.full_3d_timing = LX_HDMI_3D_FIELD_ALTERNATIVE;
		}

		if( (bufTiming.hActive == 1920)	&& 	(bufTiming.vActive == 2160) && (bufTiming.scanType == 1))
		{
			bufTiming.vActive = 1080;
			bufTiming.full_3d_timing = LX_HDMI_3D_LINE_ALTERNATIVE;
		}

		if( (bufTiming.hActive == 2560)	&& 	(bufTiming.vActive == 720) && (bufTiming.scanType == 1))
		{
			bufTiming.hActive = 1280;
			bufTiming.full_3d_timing = LX_HDMI_3D_SBSFULL;
		}

	}
	else if(eVideoFormat == LX_FORMAT_3D_FORMAT)
	{
		if(e3DStructure < 2)
		{
			VPORT_I2C_Read((UINT32*)&glink_116);
			vic = glink_116.reg_pkt_avi_dat_pip0_4;		//VIC6~VIC0
			vic = (vic & 0x7F);

	 		if(vic == 5 || vic == 6 ||vic == 7 ||vic == 10 || vic == 11 || vic == 20 || vic == 21 ||
	 			vic == 22 || vic == 25 || vic == 26 || vic == 39 )		// 1080!@60Hz & 1080!@50Hz
				bufTiming.scanType = 0;

			HDMI_PRINT(" VIC  = %d \n", vic);
		}

		switch(e3DStructure)
		{
			case LX_HDMI_VSI_3D_STRUCTURE_FRAME_PACKING:
				bufTiming.full_3d_timing = LX_HDMI_3D_FRAMEPACK;
				break;

			case LX_HDMI_VSI_3D_STRUCTURE_FIELD_ALTERNATIVE:
				bufTiming.full_3d_timing = LX_HDMI_3D_FIELD_ALTERNATIVE;
				break;

			case LX_HDMI_VSI_3D_STRUCTURE_LINE_ALTERNATIVE:
				bufTiming.full_3d_timing = LX_HDMI_3D_LINE_ALTERNATIVE;
				break;

			case LX_HDMI_VSI_3D_STRUCTURE_SIDEBYSIDE_FULL:
				bufTiming.full_3d_timing = LX_HDMI_3D_SBSFULL;
				break;

			default:
				return ret;
				break;
		}
	}
	else	//if(eVideoFormat == LX_FORMAT_EXTENDED_RESOLUTION_FORMAT)		// UD Format
		return ret;


	switch(bufTiming.full_3d_timing)
	{
		case LX_HDMI_3D_FRAMEPACK:
		{
			if(bufTiming.scanType)		//progressive
			{
				tmp8 = (bufTiming.vTotal - bufTiming.vActive);
				bufTiming.vActive = (bufTiming.vActive - tmp8) >> 1;

				//if(vic == 27 || vic == 28)

			}
			else					//interlace
			{
				if((bufTiming.hActive == 1920) && (bufTiming.vActive == 2228))
				{
					tmp8 = 68; //23+22+23
					bufTiming.vActive = (bufTiming.vActive - tmp8) >> 1;
				}
				else if((bufTiming.hActive == 1920) && (bufTiming.vActive == 1103))
				{
					tmp8 = 23; //23
					bufTiming.vActive = bufTiming.vActive - tmp8;
				}
				else
				{
					tmp8 = 23; // 1440x480i, 720x480i
					bufTiming.vActive = bufTiming.vActive - tmp8;
				}
			}
		}	break;

		case LX_HDMI_3D_SBSFULL:
		{
			bufTiming.hActive = bufTiming.hActive >> 1;
		}	break;

		case LX_HDMI_3D_FIELD_ALTERNATIVE:
		{
			bufTiming.scanType = 0;

			if(bufTiming.hActive == 1920)		tmp8 = 23; //23

			bufTiming.vActive = bufTiming.vActive - tmp8;

		}	break;

		case LX_HDMI_3D_LINE_ALTERNATIVE:
		{
			bufTiming.vActive = bufTiming.vActive >> 1;
		}	break;

		default:
			break;
	}

#if 0
	if(_gHDMI_DVI_Done < 5)
	{
		HDMI_PRINT(" Timinginfo   buf hActive = %d // 	hActive = %d \n", bufTiming.hActive, info->hActive);
		HDMI_PRINT(" Timinginfo   buf vActive = %d // 	vActive = %d \n", bufTiming.vActive, info->vActive);
		HDMI_PRINT(" Timinginfo   buf scanType = %d //  scanType = %d \n", bufTiming.scanType, info->scanType);
		HDMI_PRINT(" Timinginfo   buf full_3d = %d //  full_3d = %d \n", bufTiming.full_3d_timing, info->full_3d_timing);
		HDMI_PRINT(" Timinginfo   full_3d([0]Normal[1]FP[2]SSF[3]FA[4]LA = %d \n", bufTiming.full_3d_timing);
	}
#endif

	//info->hTotal			= bufTiming.hTotal; 			///< Horizontal total pixels
	//info->vTotal 			= bufTiming.vTotal; 			///< Vertical total lines
	//info->hStart			= bufTiming.hStart; 			///< Horizontal start pixel (Back Porch)
	//info->vStart 			= bufTiming.vStart;				///< Vertical start lines (Back Porch)
	info->hActive 			= bufTiming.hActive;			///< Horizontal active pixel
	info->vActive 			= bufTiming.vActive; 			///< Vertical active lines
	info->scanType 			= bufTiming.scanType; 			///< Scan type (0 : interlace, 1 : progressive) 	info->scanType ^= 1;
	info->full_3d_timing	= bufTiming.full_3d_timing; 	///< Full 3D Timing

	return ret;
}

/**
* HDMI_L9Ax_SetAudio
*
* @parm void
* @return int
*/
int HDMI_L9Ax_SetAudio(void)
{
	VPORT_I2C_Read((UINT32*)&glink_0);
	glink_0.reg_arc_src_prt0 = 0;
	glink_0.reg_arc_src_prt1 = 0;
	VPORT_I2C_Write((UINT32*)&glink_0);

	//Channel 0
	VPORT_I2C_Read((UINT32*)&glink_1);
	glink_1.reg_aac_en_pip0 = 1;		//Auto Audio Path Configuration Enable(N, CTS value is auto configured.)
	VPORT_I2C_Write((UINT32*)&glink_1);

	VPORT_I2C_Read((UINT32*)&glink_367);
	glink_367.reg_acr_en_pip0   = 1;		//ACR Enable(Audio Clock Generation Function Activation)
	glink_367.reg_acr_n_fs_pip0 = 1;		//0 : 128Fs, 1 : 256 Fs, 2 : 512Fs(default : 256Fs)
	glink_367.reg_acr_clk_aud_div_pip0 = 1;	//0 : 128Fs, 1 : 256 Fs, 2 : 512Fs(default : 256Fs)
	VPORT_I2C_Write((UINT32*)&glink_367);

	VPORT_I2C_Read((UINT32*)&glink_368);
	glink_368.reg_acr_adj_en_pip0 = 1;		//Clock Frequency Auto Adjustment for proper FIFO running
	VPORT_I2C_Write((UINT32*)&glink_368);

	VPORT_I2C_Read((UINT32*)&glink_383);
	glink_383.reg_i2s_sd_en_pip0   = 1;		//I2S SD Output Enable
	glink_383.reg_i2s_sd0_map_pip0 = 0;		//I2S SD 0 Output Channel Mappings
	VPORT_I2C_Write((UINT32*)&glink_383);

	VPORT_I2C_Read((UINT32*)&glink_384);
	glink_384.reg_i2s_sd1_map_pip0 = 1;		//I2S SD 1 Output Channel Mappings
	glink_384.reg_i2s_sd2_map_pip0 = 2;		//I2S SD 2 Output Channel Mappings
	glink_384.reg_i2s_sd3_map_pip0 = 3;		//I2S SD 3 Output Channel Mappings
	VPORT_I2C_Write((UINT32*)&glink_384);

	VPORT_I2C_Read((UINT32*)&glink_385);
	glink_385.reg_i2s_out_en_pip0 = 1;		//I2S Output Enable
	VPORT_I2C_Write((UINT32*)&glink_385);

	//Channel 1
	VPORT_I2C_Read((UINT32*)&glink_405);
	glink_405.reg_aac_en_pip1 = 1;
	VPORT_I2C_Write((UINT32*)&glink_405);

	VPORT_I2C_Read((UINT32*)&glink_771);
	glink_771.reg_acr_en_pip1   = 1;
	glink_771.reg_acr_n_fs_pip1 = 1;
	glink_771.reg_acr_clk_aud_div_pip1 = 0;
	VPORT_I2C_Write((UINT32*)&glink_771);

	VPORT_I2C_Read((UINT32*)&glink_772);
	glink_772.reg_acr_adj_en_pip1 = 1;
	VPORT_I2C_Write((UINT32*)&glink_772);

	VPORT_I2C_Read((UINT32*)&glink_787);
	glink_787.reg_i2s_sd_en_pip1 = 1;
	glink_787.reg_i2s_sd0_map_pip1 = 0;
	VPORT_I2C_Write((UINT32*)&glink_787);

	VPORT_I2C_Read((UINT32*)&glink_788);
	glink_788.reg_i2s_sd1_map_pip1 = 1;
	glink_788.reg_i2s_sd2_map_pip1 = 2;
	glink_788.reg_i2s_sd3_map_pip1 = 3;
	VPORT_I2C_Write((UINT32*)&glink_788);

	VPORT_I2C_Read((UINT32*)&glink_789);
	glink_789.reg_i2s_out_en_pip1 = 1;
	VPORT_I2C_Write((UINT32*)&glink_789);

	//L9 A0/A1 workaround for 128 Fs to 256 Fs
	if(lx_chip_rev() <= LX_CHIP_REV(L9, A1))
	{
		VPORT_I2C_Read((UINT32 *)&glink_382);
		glink_382.reg_i2s_sck_edge_pip0 = 1;
		VPORT_I2C_Write((UINT32*)&glink_382);
	}

	return 0;
}

/**
 *  _HDMI_L9Ax_GetAudioTypeAndFreq
 *
 *  @return int
*/
static int _HDMI_L9Ax_GetAudioTypeAndFreq(LX_HDMI_AUDIO_TYPE_T *audioType, LX_HDMI_SAMPLING_FREQ_T *samplingFreq)
{
	int ret = 0;

	//Check if audio sample word used for other purposes than liner PCM.
	VPORT_I2C_Read((UINT32*)&glink_387);

	if(glink_387.reg_achst_byte0_pip0 & HDMI_AUDIO_SAMPLE_PCM_MASK)	//bit 1, 0 : PCM, 1 : non-PCM
	{
		//Check a vaild Pc data for payload
		VPORT_I2C_Read((UINT32*)&glink_394);

		if((glink_394.reg_burst_pc_pip0_0 & BURST_INFO_PAYLOAD_ERROR_BIT_MASK) == 0) //bit 7, 0 : No Error, 1 : Error
		{
			//Set Audio Data-Types according to IEC61937-2 Burst Info Preamble C
			switch(glink_394.reg_burst_pc_pip0_0 & BURST_INFO_AUDIO_TYPE_BIT_MASK)	//bit 4 ~ 0
			{
				case BURST_INFO_AUDIO_TYPE_AC3:
				case BURST_INFO_AUDIO_TYPE_AC3_ENHANCED:
					*audioType = LX_HDMI_AUDIO_AC3;
					break;

				case BURST_INFO_AUDIO_TYPE_DTS_I:
				case BURST_INFO_AUDIO_TYPE_DTS_II:
				case BURST_INFO_AUDIO_TYPE_DTS_III:
					*audioType = LX_HDMI_AUDIO_DTS;
					break;

				case BURST_INFO_AUDIO_TYPE_MPEG1_L1:
				case BURST_INFO_AUDIO_TYPE_MPEG1_L23:
				case BURST_INFO_AUDIO_TYPE_MPEG2_EXT:
				case BURST_INFO_AUDIO_TYPE_MPEG2_L1:
				case BURST_INFO_AUDIO_TYPE_MPEG2_L2:
				case BURST_INFO_AUDIO_TYPE_MPEG2_L3:
					*audioType = LX_HDMI_AUDIO_MPEG;
					break;

				case BURST_INFO_AUDIO_TYPE_MPEG2_AAC:
					*audioType = LX_HDMI_AUDIO_AAC;
					break;

				default:
					*audioType = LX_HDMI_AUDIO_NO_AUDIO;
					break;
			}
		}	//if( (glink_394.reg_burst_pc_pip0_0 & BURST_INFO_PAYLOAD_ERROR_BIT_MASK) == 0 ) //bit 7
		else
		{
			*audioType = LX_HDMI_AUDIO_PCM;

			AUDIO_ERROR("HDMI_GetAudioInfo : Burst Info Error = %d\n", (glink_394.reg_burst_pc_pip0_0 & BURST_INFO_PAYLOAD_ERROR_BIT_MASK));
		}
	}	//if( (glink_387.reg_achst_byte0_pip0 >> 1) == 1)	//bit 1
	else
	{
		*audioType = LX_HDMI_AUDIO_PCM;
	}

	//Set Sampling frequency from IEC60958 Channel Status Byte 3
	VPORT_I2C_Read((UINT32*)&glink_390);

	switch(glink_390.reg_achst_byte3_pip0 & HDMI_AUDIO_SAMPLE_BIT_MASK) 	//bit 0 ~ 3
	{
		case HDMI_AUDIO_SAMPLE_22_05KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_22_05KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_24_KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_24_KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_32_KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_32_KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_44_1KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_44_1KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_48_KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_88_2KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_88_2KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_96_KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_96_KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_176_4KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_176_4KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_192_KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_192_KHZ;
			break;

		case HDMI_AUDIO_SAMPLE_768_KHZ:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_768_KHZ;
			break;

		default:
			*samplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;

			AUDIO_ERROR("HDMI_GetAudioInfo : samplingFreq = %d\n", *samplingFreq);
			break;
	}

	return ret;
}

/**
 *  _HDMI_L9Ax_GetAudioFreqFromTmdsClock
 *
 *  @return int
*/
static int _HDMI_L9Ax_GetAudioFreqFromTmdsClock(LX_HDMI_SAMPLING_FREQ_T *samplingFreqFromTmds)
{
	int ret = 0;

	UINT64		ui64TmdsClock = 0;			//< HDMI measured clock value of TMDS clock
	UINT64		ui64AcrN = 0;				//< HDMI ACR N value
	UINT64		ui64AcrCts = 0;				//< HDMI ACR CTS value
	UINT64		ui64TmdsSamplingFreq = 0;	//< HDMI sampling frequency in source device from TMDS clock

	//Get measured frequency value of TMDS clock.
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_23);
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_24);
	ui64TmdsClock = (UINT32)((ghdmi_phy_n_23.tmds_freq << 8) | ghdmi_phy_n_24.tmds_freq) * 10000;

	//Get ACR N H/W value.
	VPORT_I2C_Read((UINT32*)&glink_372);
	VPORT_I2C_Read((UINT32*)&glink_373);
	VPORT_I2C_Read((UINT32*)&glink_374);
	ui64AcrN = (UINT64)((glink_372.reg_acr_n_tx_pip0_2 << 16) | (glink_373.reg_acr_n_tx_pip0_1 << 8) | glink_374.reg_acr_n_tx_pip0_0);

	//Get ACR CTS H/W value.
	VPORT_I2C_Read((UINT32*)&glink_378);
	VPORT_I2C_Read((UINT32*)&glink_379);
	VPORT_I2C_Read((UINT32*)&glink_380);
	ui64AcrCts = (UINT64)((glink_378.reg_acr_cts_tx_pip0_2 << 16) | (glink_379.reg_acr_cts_tx_pip0_1 << 8) | glink_380.reg_acr_cts_tx_pip0_0);

	AUDIO_DEBUG("Get TDMS ACR  : ui64AcrN = %llu, ui64AcrCts = %llu\n", ui64AcrN, ui64AcrCts);

	//Compute a sampling frequency from TMDS clock
	ui64TmdsSamplingFreq = ui64AcrN * ui64TmdsClock;

	//Check divide by zero value.
	if( (ui64TmdsSamplingFreq > 0) && (ui64AcrCts > 0) )
	{
		do_div(ui64TmdsSamplingFreq, ui64AcrCts * 128);
	}

	//Mapping a sampling frequency from measuring from TMDS clock and ACR N & CTS H/W value
	if(ui64TmdsSamplingFreq == 0)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_NONE;
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_NONE\n");
	}
	else if(ui64TmdsSamplingFreq < 22983)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_22_05KHZ;	//  22.05 kHz(not supported)
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_22_05KHZ(not supported)\n");
	}
	else if(ui64TmdsSamplingFreq < 30000)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_24_KHZ;	//  24 kHz(not supported)
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_24_KHZ(not supported)\n");
	}
	else if(ui64TmdsSamplingFreq < 33800)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_32_KHZ;	//  32 kHz
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_32KHZ\n");
	}
	else if(ui64TmdsSamplingFreq < 45965)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_44_1KHZ;	//  44.1 kHz
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_44_1KHZ\n");
	}
	else if(ui64TmdsSamplingFreq < 67000)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_48_KHZ;	//  48 kHz
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_48_KHZ\n");
	}
	else if(ui64TmdsSamplingFreq < 91935)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_88_2KHZ;	//  88.2 kHz
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_88_2KHZ\n");
	}
	else if(ui64TmdsSamplingFreq < 135000)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_96_KHZ;	//  96 kHz
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_96_KHZ\n");
	}
	else if(ui64TmdsSamplingFreq < 183870)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_176_4KHZ;	//  176.4 kHz
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_176_4KHZ\n");
	}
	else if(ui64TmdsSamplingFreq < 210000)
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_192_KHZ;	//  192 kHz
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_192_KHZ\n");
	}
	else
	{
		*samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_768_KHZ;	//  768 kHz(not supported)
		HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_768_KHZ(not supported)\n");
	}

	HDMI_PRINT("Get Fs from TDMS clock : %llu => %d\n", ui64TmdsSamplingFreq, *samplingFreqFromTmds);
//	HDMI_ERROR("Get Fs from TDMS clock : %d => %d\n", ui32TmdsSamplingFreq, *samplingFreqFromTmds);

	return ret;
}

/**
 *  HDMI_GetAudioInfo
 *
 *  @parm LX_HDMI_AUDIO_INFO_T *
 *  @return int
*/
int HDMI_L9Ax_GetAudioInfo(LX_HDMI_AUDIO_INFO_T *audio_info)
{
	int ret = 0;

	LX_HDMI_AUDIO_TYPE_T		audioType		= LX_HDMI_AUDIO_NO_AUDIO;		///< HDMI Audio Type.
	LX_HDMI_SAMPLING_FREQ_T 	samplingFreq	= LX_HDMI_SAMPLING_FREQ_48_KHZ; ///< Sampling Frequency
	BOOLEAN 					bPortConnected	= FALSE;	///< HDMI Port connect status (1 : connected, 0 : not connected)
	BOOLEAN 					bInterlaced 	= FALSE;	///< Scan type (1 : interlace, 0 : progressive)

	UINT16						ui16VActive;			//< HDMI Vertical Active Size
	UINT16						ui16HActive;			//< HDMI horizontal Active Size

	LX_HDMI_SAMPLING_FREQ_T		samplingFreqFromTmds = LX_HDMI_SAMPLING_FREQ_48_KHZ;	//< HDMI sampling frequency from TMDS clock

	//Check HDMI port connection
	VPORT_I2C_Read((UINT32*)&glink_0);
	if(glink_0.reg_scdt_prt0 == 0)	//Check HDMI, DVI Sync Detect
	{
		AUDIO_ERROR("HDMI_GetAudioInfo : HDMI / DVI Not Connected!!!\n");

		goto func_exit;
	}

	//Get video vertical active size
	VPORT_I2C_Read((UINT32*)&glink_350);
	ui16HActive  = glink_350.reg_h_av_pip0_1 << 8;
	VPORT_I2C_Read((UINT32*)&glink_351);
	ui16HActive |= glink_351.reg_h_av_pip0_0;

	//Get video horizontal active size
	VPORT_I2C_Read((UINT32*)&glink_352);
	ui16VActive  = glink_352.reg_v_av_pip0_1 << 8;
	VPORT_I2C_Read((UINT32*)&glink_353);
	ui16VActive |= glink_353.reg_v_av_pip0_0;

	//Check video active size
	if( (ui16VActive <  240) || (ui16HActive <  320)	\
	  ||(ui16VActive > 2970) || (ui16HActive > 4096) )
	{
		AUDIO_ERROR("HDMI_GetAudioInfo : Video Active Size Error(v = %d, h = %d)!!!\n",	\
					 ui16VActive, ui16HActive);

		goto func_exit;
	}

	//Check HDMI /DVI Mode
	//VPORT_I2C_Read((UINT32*)&glink_1);
	//if(glink_1.reg_hdmi_mode_pip0 == 1)	//0 : DVI, 1 : HDMI
	if(gBootData->mode)
	{
		//Check HDMI timing count
		if( _gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch] )
		{
			AUDIO_ERROR("HDMI_GetAudioInfo : HDMI Timing Error(time = %d, table = %d)!!!\n",	\
						 _gHDMI_DVI_Done, gBootData->timingCn[gBootData->hdmi_switch]);

			goto func_exit;
		}

		//Get HDMI Audio Type and Sampling Frequency
		(void)_HDMI_L9Ax_GetAudioTypeAndFreq(&audioType, &samplingFreq);

		//Get HDMI Audio Sampling Frequency from TMDS clock when audio sample is PCM
		if(audioType == LX_HDMI_AUDIO_PCM)
		{
			//Get a sampling frequency from TMDS clock
			(void)_HDMI_L9Ax_GetAudioFreqFromTmdsClock(&samplingFreqFromTmds);

			//Set a audio output mute when TMDS Fs is zero or not support.
			if( (samplingFreqFromTmds == LX_HDMI_SAMPLING_FREQ_NONE)
			  ||(samplingFreqFromTmds == LX_HDMI_SAMPLING_FREQ_22_05KHZ)
			  ||(samplingFreqFromTmds == LX_HDMI_SAMPLING_FREQ_24_KHZ)
			  ||(samplingFreqFromTmds == LX_HDMI_SAMPLING_FREQ_768_KHZ))
			{
 				//Mute audio data
				VPORT_I2C_Read((UINT32*)&glink_383);
				glink_383.reg_i2s_sd_en_pip0 = 0;
				VPORT_I2C_Write((UINT32*)&glink_383);

				HDMI_ERROR("HDMI_GetAudioInfo : TMDS Fs Error(%d)!!!\n", samplingFreqFromTmds);
			}
			else
			{
				//Set a sampling frequency from TMDS clock
				samplingFreq = samplingFreqFromTmds;

 				//Un-mute audio data
				VPORT_I2C_Read((UINT32*)&glink_383);
				glink_383.reg_i2s_sd_en_pip0 = 1;
				VPORT_I2C_Write((UINT32*)&glink_383);

				HDMI_PRINT("HDMI_GetAudioInfo : samplingFreq = %d\n", samplingFreq);
			}
		}
		else
		{
			//Un-mute audio data
			VPORT_I2C_Read((UINT32*)&glink_383);
			glink_383.reg_i2s_sd_en_pip0 = 1;
			VPORT_I2C_Write((UINT32*)&glink_383);
		}

		//Set HDMI/DVI port connect status
		bPortConnected = TRUE;

		//Set interlaced / progressive scan mode
		VPORT_I2C_Read((UINT32*)&glink_363);
		bInterlaced = glink_363.reg_intrl_pip0;

		//For debug print
		if( (audioType		!= _gHDMIAudioInfo.audioType)	   || (samplingFreq != _gHDMIAudioInfo.samplingFreq)	\
		  ||(bPortConnected != _gHDMIAudioInfo.bPortConnected) || (bInterlaced	!= _gHDMIAudioInfo.bInterlaced) )
		{
			HDMI_DEBUG("HDMI_GetAudioInfo : type = %d, freq = %d, bConnected = %d, bInterlaced = %d\n", audioType, samplingFreq, bPortConnected, bInterlaced);

#ifdef	ENABLE_HDMI_AUDIO_DEBUG
			//Print Audio Debug Info.
			(void)_HDMI_L9Ax_DebugAudioInfo();
#endif		//#ifdef	ENABLE_HDMI_AUDIO_DEBUG
		}
	}
	else
	{
		//Check HDMI timing count
		if( _gHDMI_DVI_Done < gBootData->dviCn[gBootData->hdmi_switch] )
		{
			AUDIO_ERROR("HDMI_GetAudioInfo : DVI Timing Error(time = %d, table = %d)!!!\n",	\
						 _gHDMI_DVI_Done, gBootData->dviCn[gBootData->hdmi_switch]);

			goto func_exit;
		}

		audioType	   = LX_HDMI_AUDIO_DVI;
		samplingFreq   = LX_HDMI_SAMPLING_FREQ_48_KHZ;
		bPortConnected = TRUE;
		bInterlaced    = FALSE;

		if(audioType != _gHDMIAudioInfo.audioType)
		{
			HDMI_DEBUG("HDMI_GetAudioInfo : type = 0x%X, freq = 0x%X,\n", audioType, samplingFreq);
		}
	}

func_exit:
	//Mute audio data
	if(audioType == LX_HDMI_AUDIO_NO_AUDIO)
	{
		VPORT_I2C_Read((UINT32*)&glink_383);
		glink_383.reg_i2s_sd_en_pip0 = 0;
		VPORT_I2C_Write((UINT32*)&glink_383);
	}

	//Update HDMI Audio Info.
	audio_info->audioType	   = audioType;
	audio_info->samplingFreq   = samplingFreq;
	audio_info->bPortConnected = bPortConnected;
	audio_info->bInterlaced    = bInterlaced;

	//Update global value for debug print
	_gHDMIAudioInfo.audioType	   = audioType;
	_gHDMIAudioInfo.samplingFreq   = samplingFreq;
	_gHDMIAudioInfo.bPortConnected = bPortConnected;
	_gHDMIAudioInfo.bInterlaced    = bInterlaced;

	HDMI_PRINT("HDMI_GetAudioInfo : type = 0x%X, freq = 0x%X, bConnected = %d, bInterlaced = %d\n", audio_info->audioType, audio_info->samplingFreq, audio_info->bPortConnected, audio_info->bInterlaced);
	//HDMI_ERROR("HDMI_GetAudioInfo : type = 0x%X, freq = 0x%X, bConnected = %d, bInterlaced = %d\n", audio_info->audioType, audio_info->samplingFreq, audio_info->bPortConnected, audio_info->bInterlaced);

	return ret;
}

/**
 *  HDMI_SetMute
 *
 *  @parm LX_HDMI_MUTE_CTRL_T *
 *  @return int
*/
int HDMI_L9Ax_SetMute(LX_HDMI_MUTE_CTRL_T *mute)
{
#if 1
	if (_gSCDT && _gHDMI_port_changed == 0)
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
#else
	BOOLEAN videoMute = 0;
	BOOLEAN audioMute = 0;
	LX_DE_SET_WIN_BLANK_T winBlank = {LX_DE_WIN_MAIN, 1, {128, 0, 128}}; // Default - Mute

	videoMute = (pHdmiMute->bVideoMute == 1) ? 1 : 0;
	audioMute = (pHdmiMute->bAudioMute == 1) ? 1 : 0;

	if( pHdmiMute->muteCtrl == 0)
	{
		_gMuteCtrl = 255;
		HDMI_ERROR("HDMI_L8Bx_SetMute Mute control disable\n");
	}
	else if( pHdmiMute->muteCtrl == 1)
	{
		_gMuteCtrl = 0;
		HDMI_ERROR("HDMI_L8Bx_SetMute Mute control Enable\n");
		return 0;
	}
	else if(pHdmiMute->muteCtrl == 2)
	{
		if(_gMuteCtrl == 255)
		{
			HDMI_ERROR("HDMI_ internal Mute control disable\n");
			return 0;
		}
	}
	//else	// for Debug (pHdmiMute->muteCtrl == 3)

	winBlank.bEnable = videoMute;
	DE_REG_SetWinBlank(&winBlank);

	VPORT_I2C_Read((UINT32*)&glink_405);
	glink_405.reg_mute_aud_pip1 = 1;
	VPORT_I2C_Write((UINT32*)&glink_405);
#endif
	return 0;
}

/**
 *  HDMI_SetArc
 *
 *  @parm LX_HDMI_ARC_CTRL_T *
 *  @return int
*/
int HDMI_L9Ax_SetArc(LX_HDMI_ARC_CTRL_T *arc)
{
	int ret = 0;

	/* ARC pdb Issue - L9A B0 fixed : sh.myoung 20110518*/
#ifdef L9A_A0_ARC_PDB
	VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_26);
	ghdmi_phy_n_26.odt_sel = arc->bARCMute * 2; // Off : 0, On : 2(default)
	VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_26);
#endif

	VPORT_I2C_Read((UINT32*)&glink_0);

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		//Enable ARC in Phy0
		glink_0.reg_arc_src_prt0 = arc->path;

		VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
		ghpd_out_control_1.phy0_phy_arc_pdb = arc->bARCMute;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);
	}
	else
	{
		//Enable ARC in Phy1
		glink_0.reg_arc_src_prt1 = arc->path;

		VPORT_I2C_Read((UINT32*)&ghpd_out_control_2);
		ghpd_out_control_2.phy1_phy_arc_pdb = arc->bARCMute;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_2);
	}

	VPORT_I2C_Write((UINT32*)&glink_0);

	HDMI_PRINT("HDMI_L9Ax_SetArc [%s] Arc Path = %d,  \n", arc->bARCMute?"On":"Off", arc->path);
	return ret;
}

/**
 * HDMI_L9Ax_SetRGBYCbCr
 *
 * @parm UINT32 *
 * @return int
*/
int HDMI_L9Ax_SetRGBYCbCr(LX_HDMI_AVI_COLORSPACE_T *space)
{

#if 1
	if (_gSCDT && _gHDMI_port_changed == 0)
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
#else
	UINT8 blank_r = 0;
	UINT8 blank_g = 0;
	UINT8 blank_b = 0;

	switch(*space)
	{
			case LX_HDMI_AVI_COLORSPACE_YCBCR422:
			case LX_HDMI_AVI_COLORSPACE_YCBCR444:
				blank_r = 0x80;
				blank_g = 0x00;
				blank_b = 0x80;
				break;

			case LX_HDMI_AVI_COLORSPACE_RGB:
			default:
				blank_r = 0x00;
				blank_g = 0x00;
				blank_b = 0x00;
			break;
	}

	if(_gui32CurPort == HDMI_L9AX_PORT_0)
	{
		glink_364.reg_vid_blank_r_prt0 = blank_r;
		glink_365.reg_vid_blank_g_prt0 = blank_g;
		glink_366.reg_vid_blank_b_prt0 = blank_b;

		VPORT_I2C_Write((UINT32*)&glink_364);
		VPORT_I2C_Write((UINT32*)&glink_365);
		VPORT_I2C_Write((UINT32*)&glink_366);
	}
	else
	{
		glink_768.reg_vid_blank_r_prt1 = blank_r;
		glink_769.reg_vid_blank_g_prt1 = blank_g;
		glink_770.reg_vid_blank_b_prt1 = blank_b;
		VPORT_I2C_Write((UINT32*)&glink_768);
		VPORT_I2C_Write((UINT32*)&glink_769);
		VPORT_I2C_Write((UINT32*)&glink_770);
	}
#endif
	return 0;
}

/**
 *  HDMI_PowerConsumption
 *
 *  @param LX_HDMI_POWER_CTRL_T *
 *  @return int
*/
int HDMI_L9Ax_PowerConsumption(LX_HDMI_POWER_CTRL_T *power)
{
	int ret = 0;
	if(_gui32CurPort == HDMI_L9AX_PORT_0)	//if(ghpd_out_control_1.phy_enable == 0)
	{
#if 1
		VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_01);
		ghdmi_phy_n_01.resetb_all = power->enable;
		ghdmi_phy_n_01.pdb_all = power->enable;
		VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_01);
#else
		VPORT_I2C_Read((UINT32*)&gaudio_pll_5);
		gaudio_pll_5.hdmiphy0_sdaout_enable = 0;
		VPORT_I2C_Write((UINT32*)&gaudio_pll_5);

		VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
		ghpd_out_control_1.phy0_phy_pdb = power->enable;
		ghpd_out_control_1.phy0_phy_rstn = power->enable;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);
		if(power->enable == 1)
		{
		VPORT_I2C_Read((UINT32*)&gaudio_pll_5);
		gaudio_pll_5.hdmiphy0_sdaout_enable = 1;
		VPORT_I2C_Write((UINT32*)&gaudio_pll_5);
		}
#endif
	}
	else
	{
#if 1
		VPORT_I2C_Read((UINT32*)&ghdmi_phy_n_01);
		ghdmi_phy_n_01.resetb_all = power->enable;
		ghdmi_phy_n_01.pdb_all = power->enable;
		VPORT_I2C_Write((UINT32*)&ghdmi_phy_n_01);
#else
		VPORT_I2C_Read((UINT32*)&gaudio_pll_5);
		gaudio_pll_5.hdmiphy1_sdaout_enable = 0;
		VPORT_I2C_Write((UINT32*)&gaudio_pll_5);

		VPORT_I2C_Read((UINT32*)&ghpd_out_control_1);
		VPORT_I2C_Read((UINT32*)&ghpd_out_control_2);
		//ghpd_out_control_1.phy1_phy_pdb = power->enable;
		ghpd_out_control_1.phy1_phy_rstn = power->enable;
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_1);
		VPORT_I2C_Write((UINT32*)&ghpd_out_control_2);
		if(power->enable == 1)
		{
		VPORT_I2C_Read((UINT32*)&gaudio_pll_5);
		gaudio_pll_5.hdmiphy1_sdaout_enable = 1;
		VPORT_I2C_Write((UINT32*)&gaudio_pll_5);
		}
#endif
	}

	HDMI_ERROR(" HDMI_L9Ax_PowerConsumption Port[%d] Power[%s]\n", power->srcIdx, (power->enable ? "On" : "Off"));
	return ret;
}

int HDMI_L9Ax_SetUDn3DControl(LX_HDMI_UD_3D_MODE_T *udn3d)
{
	int ret = 0;

	VPORT_I2C_Read((UINT32*)&gdata_bridge_2);

	switch(*udn3d)
	{
		case LX_HDMI_UD:
			gdata_bridge_2.reg_half_bridge_mode_en = 1;
			gdata_bridge_2.reg_3dfr_bridge_mode = 0;
			gdata_bridge_2.reg_ud_autoset = 1;
			gdata_bridge_2.reg_3dfr_autoset = 0;
			break;

		case LX_HDMI_3D_FP_60HZ:
			gdata_bridge_2.reg_half_bridge_mode_en = 1;
			gdata_bridge_2.reg_3dfr_bridge_mode = 1;
			gdata_bridge_2.reg_ud_autoset = 0;
			gdata_bridge_2.reg_3dfr_autoset = 1;
			break;

		case LX_HDMI_3D_SS_FULL:
		case LX_HDMI_UD_3D_OFF:
		default:
			gdata_bridge_2.reg_half_bridge_mode_en = 0;
			gdata_bridge_2.reg_3dfr_bridge_mode = 0;
			gdata_bridge_2.reg_ud_autoset = 0;
			gdata_bridge_2.reg_3dfr_autoset = 0;
			break;
	}

	VPORT_I2C_Write((UINT32*)&gdata_bridge_2);
	HDMI_PRINT("HDMI_L9Ax_SetUDn3DControl[0:2D, 1:UD 2:SS 3:FP] ==>[ %d ]\n", *udn3d);
	return ret;
}

/**
 *  HDMI_GetRegister
 *
 *  @parm UINT32 *
 *  @return int
 	data:8,
	rsvd8:8,
	slaveAddr:8,
	regAddr:8;
*/
int HDMI_L9Ax_GetRegister(UINT32 addr , UINT32 *value)
{
	int ret = 0;
	VPORT_I2C_DATA_T *i2cData = (VPORT_I2C_DATA_T*)&addr;

	VPORT_I2C_Read((UINT32*)i2cData);

	//HDMI_ERROR("Read Slave 0x%x, reg 0x%x  Data = 0x%x\n", i2cData->slaveAddr, i2cData->regAddr, i2cData->data);
	*value = i2cData->data;

	return ret;
}


/**
 *  HDMI_GetAudioInfo
 *
 *  @parm LX_HDMI_AUDIO_INFO_T *
 *  @return int
*/
int HDMI_L9Ax_SetRegister(UINT32 addr , UINT32 value)
{
	int ret = 0;
	VPORT_I2C_DATA_T *i2cData = (VPORT_I2C_DATA_T*)&addr;
	i2cData->data = (UINT8)(value&0x000000ff);

	//HDMI_ERROR("Write Slave 0x%x, reg 0x%x  Data = 0x%x\n", i2cData->slaveAddr, i2cData->regAddr, i2cData->data);

	VPORT_I2C_Write((UINT32*)i2cData);

	return ret;
}


/**
 *
 * HDMI_L9Ax_irq_handler irq handler
 *
 * @param	irq , device id , regs
 * @return	0 : OK , -1 : NOK
 *
*/
irqreturn_t HDMI_L9Ax_IRQHandler(int irq, void *dev)
{


	return IRQ_HANDLED;

}

void HDMI_L9Ax_EnableInterrupt(void)
{


	/* [TODO-L9] implement interrupt for L9 */


}

#ifdef	KDRV_CONFIG_PM
int HDMI_L9Ax_RunSuspend(void)
{
	int ret = -1;

	return ret;

}

int HDMI_L9Ax_RunResume(void)
{

	int ret = -1;

	return ret;

}
#endif
