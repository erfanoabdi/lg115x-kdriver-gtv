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

#include "os_util.h"

#include "hdmi_drv.h"
#include "hdmi_kapi.h"
#include "hdmi_module.h"
#include "hdmi_phy_reg_l8.h"
#include "../../afe/vport_reg_B0.h"
#include "../sys/ctop_regs.h"	//for CTOP CTRL Reg. map

/**< DE WinBlank Function */
#include "de_kapi.h"
#include "../de/de_ipc_def.h"
#include "../de/de_def.h"
#include "../de/de_int_def.h"
#include "../de/de_hal_def.h"
#include "../de/l8/de_reg_def_l8.h"
#include "../de/l8/de_reg_l8.h"
/******************************************************************************
 *				DEFINES
 *****************************************************************************/
#define HDMI_L8B0_SWITCH_ADAPTIVE_LOOP
//#define HDMI_L8C0				// B0 fix

#define HDMI_DEBUG_ON

//#define ENABLE_HDMI_AUDIO_DEBUG
#define USE_HDMI_AUDIO_PLL

/**
 *	Global variables of the driver
 */
extern volatile VPORT_RegBx_T *gVportRegBx;
extern			VPORT_RegBx_T *gpRegVPORT;		// shadow register

static volatile HDMI_PHY3P_Reg_T *pstHdmiPhyReg;
static 			HDMI_PHY3P_Reg_T *pstHdmiPhyShadowReg;

extern LX_HDMI_REG_T *pHdmiRegCfg;
extern HDMI_DATA_T *gBootData;

static UINT8 _gSCDT = 0;
static UINT8 _gMuteCtrl = 0;
static UINT8 _gcount = 0;
static UINT8 _gSourceMute = 0;
static UINT8 _gHDMI_CSC_Mute = 0;
static UINT8 _gTimingStatus = 0;

static LX_HDMI_AVI_COLORSPACE_T	_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
static LX_HDMI_TIMING_INFO_T _gPrevTiming = {0,};
static UINT8 _gHDMI_port_changed = 0;
static UINT8 _gHDMI_Change = 0;
static UINT8 _gHDMI_DVI_Done = 0;

//HDMI Audio Variables
static UINT32	_gHDMIAudioPrevNValue		= 0;	//HDMI previous N Value in source device
static UINT32	_gHDMIAudioPrevTMDSFreq 	= 0;	//HDMI previous measured frequency value of TMDS clock
static LX_HDMI_AUDIO_INFO_T _gHDMIAudioInfo = {LX_HDMI_MAIN_WINDOW, LX_HDMI_AUDIO_DEFAULT,	\
										   LX_HDMI_SAMPLING_FREQ_NONE, FALSE, FALSE};

#ifdef HDMI_DEBUG_ON
static UINT32 _gPrevInt_plug = 0;
//static UINT32 _gPrevAVI = 0;
#endif

#ifdef	KDRV_CONFIG_PM
static unsigned int guiPmInterruptStatus;
static unsigned int guiPmHpdStatus;
static LX_HDMI_HPD_T gstPmPortSel;
#endif

static void HDMI_L8Bx_SetColorDepth(BOOLEAN bOnOff);
static int HDMI_L8Bx_Get3DScanType(void);

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
 * HDMI_L8Bx_Get3DScanType
 * Get 3D format Scan Type
 * @parm void
 * @return int		1 : 3D Timing Info - Scan Type is interlace
*/
int HDMI_L8Bx_Get3DScanType(void)
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

	VPORT_RdFL(hdmi_link_034);
	VPORT_Rd01(hdmi_link_034, reg_pkt_avi_dat__6, vic);
	vic = (vic & 0x7F);
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
 * HDMI_L8Bx_SetColorDepth
 * Set Color Depth(Deep Color) Information from Link to PHy for NoSigal & Invalid Format Issue
 * @parm BOOLEAN		ON : 1 (SCDT ==0), OFF : 0 (SCDT == 0)
 * @return void
*/
void HDMI_L8Bx_SetColorDepth(BOOLEAN bOnOff)
{
	UINT8	port = 0;
	UINT8	linkColorDepth = 0, phyColorDepth = 0;

	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_prt_sel, port);
	port = (port < 2) ? 0 : 1;

	VPORT_RdFL(hdmi_link_085);
	VPORT_Rd01(hdmi_link_085, reg_cdepth, linkColorDepth);		// Deep Color

	if(port)	// Phy 1
	{
		HDMIPHY_RdFL(phy1.hdmi_phy_pll04);
		HDMIPHY_Rd01(phy1.hdmi_phy_pll04, pi_div, phyColorDepth);
	}
	else		// Phy 0
	{
		HDMIPHY_RdFL(phy0.hdmi_phy_pll04);
		HDMIPHY_Rd01(phy0.hdmi_phy_pll04, pi_div, phyColorDepth);
	}

	if(linkColorDepth == phyColorDepth)
		return ;

	if(bOnOff)		// SCDT == 1
	{
		if(port)	// Phy 1
		{
			HDMIPHY_RdFL(phy1.hdmi_phy_pll04);
			HDMIPHY_Wr01(phy1.hdmi_phy_pll04, pi_pll_dat_sel, 0x1);
			HDMIPHY_Wr01(phy1.hdmi_phy_pll04, pi_div, linkColorDepth);
			HDMIPHY_WrFL(phy1.hdmi_phy_pll04);
		}
		else		// Phy 0
		{
			HDMIPHY_RdFL(phy0.hdmi_phy_pll04);
			HDMIPHY_Wr01(phy0.hdmi_phy_pll04, pi_pll_dat_sel, 0x1);
			HDMIPHY_Wr01(phy0.hdmi_phy_pll04, pi_div, linkColorDepth);
			HDMIPHY_WrFL(phy0.hdmi_phy_pll04);
		}
	}
	else			// SCDT == 0
	{
		HDMIPHY_RdFL(phy1.hdmi_phy_pll04);
		HDMIPHY_Wr01(phy1.hdmi_phy_pll04, pi_pll_dat_sel, 0x0);
		HDMIPHY_Wr01(phy1.hdmi_phy_pll04, pi_div, 0x00);
		HDMIPHY_WrFL(phy1.hdmi_phy_pll04);

		HDMIPHY_RdFL(phy0.hdmi_phy_pll04);
		HDMIPHY_Wr01(phy0.hdmi_phy_pll04, pi_pll_dat_sel, 0x0);
		HDMIPHY_Wr01(phy0.hdmi_phy_pll04, pi_div, 0x00);
		HDMIPHY_WrFL(phy0.hdmi_phy_pll04);
	}
}

/**
 * HDMI_L8Bx_GetCSCStatus
 * CSS Status Information
 * @parm void
 * @return int - 1 : changed , 0 : don't change
*/
int HDMI_L8Bx_GetCSCStatus(void)
{
	UINT8 hdmiMode = 0;
	UINT32 aviData = 0;

	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_hdmi_mode, hdmiMode);	// HDMI mode


	if(hdmiMode)
	{
		VPORT_RdFL(hdmi_link_035);
		VPORT_Rd01(hdmi_link_035, reg_pkt_avi_dat__7, aviData);
		_gPrevPixelEncoding = (aviData & 0x6000) >> 13;
	}
	else
		_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;

	HDMI_PRINT(" CSC _gPrevPixelEncoding = [%d] \n", _gPrevPixelEncoding);
	return 0;
}

/**
 * HDMI_L8Bx_SetPixelRepetition
 * Pixel Repetition for 13.5MHz formats - support Pixel Repetition 2
 * @parm void
 * @return void
*/
void HDMI_L8Bx_SetPixelRepetition(void)
{
	UINT8 port  = 0;
	UINT8 repet = 0;
	UINT8 reg_div2 = 0;
	UINT8 reg_div4 = 0;
	UINT8 prt_sel = 0;

	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_prt_sel, prt_sel);
	port = (prt_sel < 2) ? 0 : 1;

	VPORT_RdFL(hdmi_link_079);
	VPORT_Rd01(hdmi_link_079, reg_pr, repet);

	if (port == 0)
	{
		HDMIPHY_RdFL(phy0.hdmi_phy_pll03);
		HDMIPHY_Rd01(phy0.hdmi_phy_pll03,pi_pclk_div2,reg_div2);
		HDMIPHY_Rd01(phy0.hdmi_phy_pll03,pi_pclk_div4,reg_div4);
	}
	else
	{
		HDMIPHY_RdFL(phy1.hdmi_phy_pll03);
		HDMIPHY_Rd01(phy1.hdmi_phy_pll03,pi_pclk_div2,reg_div2);
		HDMIPHY_Rd01(phy1.hdmi_phy_pll03,pi_pclk_div4,reg_div4);
	}

	HDMI_PRINT(" HDMI Pixel Repetition_Bx. Port(PHy)[%d] Repetition[%d] \n",port, repet);

	switch(repet)
	{
		case 0:
			if((reg_div2==0) && (reg_div4==0)) return;
			else 	reg_div2 = reg_div4 = 0;
			break;

		case 1:
			if((reg_div2==1) && (reg_div4==0)) return;
			else	{ reg_div2 = 1; reg_div4 = 0; }
			break;

		default:
			reg_div2 = reg_div4 = 0;
			break;
	}

	if(port)
	{
		HDMIPHY_RdFL(phy1.hdmi_phy_pll03);
		HDMIPHY_Wr01(phy1.hdmi_phy_pll03, pi_pclk_div2, reg_div2);
		HDMIPHY_Wr01(phy1.hdmi_phy_pll03, pi_pclk_div4, reg_div4);
		HDMIPHY_WrFL(phy1.hdmi_phy_pll03);
	}
	else
	{
		HDMIPHY_RdFL(phy0.hdmi_phy_pll03);
		HDMIPHY_Wr01(phy0.hdmi_phy_pll03, pi_pclk_div2, reg_div2);
		HDMIPHY_Wr01(phy0.hdmi_phy_pll03, pi_pclk_div4, reg_div4);
		HDMIPHY_WrFL(phy0.hdmi_phy_pll03);
	}

	gBootData->pixelRepet = repet;
}

/**
 * HDMI_L8Bx_SetAutoSyncPolarity
 *
 * @parm void
 * @return void
*/
void HDMI_L8Bx_SetAutoSyncPolarity(void)
{
	/* Auto Negative Polarity Setting from L8 B0 */
	//gVportRegBx->hdmi_link_079.reg_inv_hs = gVportRegBx->hdmi_link_085.reg_h_pol;	// horizontal sync inversion
	//gVportRegBx->hdmi_link_079.reg_inv_vs = gVportRegBx->hdmi_link_085.reg_v_pol;	// vertical sync inversion
}

/**
 * HDMI_L8Bx_GetPortConnection
 *
 * @parm void
 * @return int - 1 : connected , 0 : disconnected
*/
int HDMI_L8Bx_GetPortConnection(void)
{
	LX_HDMI_MUTE_CTRL_T		hdmiMute = {LX_HDMI_MAIN_WINDOW, 0, 0, 2};  // Default - A/V Clear

#ifdef HDMI_DEBUG_ON   // HDMI interupt
	UINT32 	int_plug = 0;
#endif

	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_scdt, _gSCDT);

	if( _gSCDT == 1)
	{
		HDMI_L8Bx_SetColorDepth(_gSCDT);
		_gcount = 0;
	}
	else
	{
		if(_gcount == 0)
		{
			if(gBootData->pixelRepet > 0)			//L8 B0 bug 2010.12.07 - HDMI(repet Mode) -> DVI Format Change
			{
				VPORT_RdFL(top_010);
				VPORT_Wr01(top_010, reg_swrst_linktmds, 1);
				VPORT_WrFL(top_010);

				HDMIPHY_RdFL(phy0.hdmi_phy_pll03);
				HDMIPHY_Wr01(phy0.hdmi_phy_pll03, pi_pclk_div2, 0);
				//HDMIPHY_Wr01(phy0.hdmi_phy_pll03, pi_pclk_div4, 0);
				HDMIPHY_WrFL(phy0.hdmi_phy_pll03);

				HDMIPHY_RdFL(phy1.hdmi_phy_pll03);
				HDMIPHY_Wr01(phy1.hdmi_phy_pll03, pi_pclk_div2, 0);
				//HDMIPHY_Wr01(phy1.hdmi_phy_pll03, pi_pclk_div4, 0);
				HDMIPHY_WrFL(phy1.hdmi_phy_pll03);

				gBootData->pixelRepet = 0;
			}

			//* Sony BD Player(BD-S1E) Format Detection Issue */
			//* Llinksys & linktmds reset NG : linktmds -> Llinksys Sequence
			VPORT_RdFL(top_010);
			VPORT_Wr01(top_010, reg_swrst_linksys, 1);
			VPORT_WrFL(top_010);

			_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;

			VPORT_RdFL(top_010);
			VPORT_Wr(top_010, 0x00000000);  	// All Link Reset Clear
			VPORT_WrFL(top_010);

			HDMI_L8Bx_SetColorDepth(_gSCDT);

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

#ifdef HDMI_DEBUG_ON   // HDMI interupt
	memcpy((void *)&int_plug , (void *)&gVportRegBx->hdmi_link_001 , sizeof(unsigned int)*1);

	if(_gPrevInt_plug != int_plug)
	{
		HDMI_ERROR(" Bx_gPrevInt_plug = [0x%x] -> [0x%x]\n", _gPrevInt_plug, int_plug);
		HDMI_ERROR(" HDMI[%d] Vsize[%d]  Pixel Repeat[%d] \n", gVportRegBx->hdmi_link_008.reg_hdmi_mode, gVportRegBx->hdmi_link_081.reg_v_av, gVportRegBx->hdmi_link_079.reg_pr );
		_gPrevInt_plug = int_plug;
	}
#endif

	return _gSCDT;
}

/**
 * HDMI_L8Bx_GetMode
 *
 * @parm LX_HDMI_MODE_T
 * @return int
*/
int HDMI_L8Bx_GetMode(LX_HDMI_MODE_T *mode)
{
	int 	ret = 0;
	BOOLEAN bHDMI = 0;   					///< 0 : DVI mode, 1 : HDMI Mode
	UINT8	u8port = 0;						///< HDMI port

	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_prt_sel, u8port);

	//if(!HDMI_GetPortConnection()) { goto func_exit; }
	if (!_gSCDT) { goto func_exit; }

	// HDMI/DVI mode status
	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_hdmi_mode, bHDMI);

func_exit:
	HDMI_PRINT(" HDMI_L8Bx_GetMode = DDIPort[%d] CurPort[%d] mode[%d] \n", mode->u8port, u8port, bHDMI);
	mode->srcIdx 	= LX_HDMI_MAIN_WINDOW;
	mode->bHDMI 	= gBootData->mode = bHDMI;
	mode->u8port 	= u8port;

	return ret;
}

/**
* HDMI_L8Bx_GetAspectRatio
*
* @parm LX_HDMI_ASPECTRATIO_T
* @return int
*/
int HDMI_L8Bx_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio)
{
	int ret = 0;
	UINT32 aviData = 0;

	LX_HDMI_AVI_PICTURE_ARC_T AspectRatio = 0; // 0 : ARC_NO_DATA

	//if(!HDMI_GetPortConnection()) { memset((void *)ratio , 0 , sizeof(LX_HDMI_ASPECTRATIO_T)); goto func_exit; }
	if (!_gSCDT) { memset((void *)ratio , 0 , sizeof(LX_HDMI_ASPECTRATIO_T)); goto func_exit; }

	// aspect ratio for AVI
	VPORT_RdFL(hdmi_link_035);
	VPORT_Rd01(hdmi_link_035, reg_pkt_avi_dat__7, aviData);
	AspectRatio = (aviData&0x300000) >> 20;

func_exit:
	ratio->srcIdx = LX_HDMI_MAIN_WINDOW;
	ratio->eAspectRatio = AspectRatio;

	return ret;
}

/**
 * HDMI_L8Bx_GetTimingInfo
 *
 * @parm HDMI_IOR_GET_TIMING_INFO
 * @return int
*/
int HDMI_L8Bx_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info)
{
	int ret = 0;
	BOOLEAN mode = 0,		muteClr	= 0;
	BOOLEAN pixelRepet = 0, hdcpAuth = 0;
	UINT32	tmdsClock = 0,	tmpClock = 0;
	UINT32	up_freq = 0,	down_freq = 0;
	UINT8 	phyPort = 0,	colorDepth = 0;
	UINT8 	prt_sel = 0, 	count = 0;
//	UINT32  aviInfo = 0;
	LX_HDMI_TIMING_INFO_T 	bufTiming = {0,};
	LX_HDMI_MUTE_CTRL_T		hdmiMute = {LX_HDMI_MAIN_WINDOW, 0, 0, 2};  // Default - A/V Clear

	memset((void *)info , 0 , sizeof(LX_HDMI_TIMING_INFO_T));

	if(!HDMI_GetPortConnection())
	{
		memset(&bufTiming , 0 , sizeof(LX_HDMI_TIMING_INFO_T));
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
		_gHDMI_DVI_Done = _gHDMI_Change = 0;
		//HDMI_ERROR(" HDMI_L8Bx_GetTimingInfo  PortConnection \n");
		goto func_exit;
	}

/* Sourec Mute & CSC Mute Clear */
	VPORT_RdFL(hdmi_link_001);
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

	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_hdmi_mode, mode);
	VPORT_Rd01(hdmi_link_008, reg_prt_sel, prt_sel);

	if(_gHDMI_port_changed > 0)
	{
		_gHDMI_port_changed --;

		/*  AVI & VSI Packet Null Issue Workaround W/ Switch IC */
		/*  HDMI Mode Detection NG */
		if( _gHDMI_port_changed == 0 && gBootData->hdmi_switch == 1 )
		{
			//memcpy((void *)&aviInfo , (void *)&gVportRegBx->hdmi_link_028 , sizeof(unsigned int)*1);
			//if(aviInfo == 0)
			{
				HDMIPHY_RdFL(phy0.hdmi_phy_rsv01);
				HDMIPHY_Wr01(phy0.hdmi_phy_rsv01, resetb_sel, 1);
				HDMIPHY_WrFL(phy0.hdmi_phy_rsv01);

				HDMIPHY_RdFL(phy0.hdmi_phy_dbt00);
				HDMIPHY_Wr01(phy0.hdmi_phy_dbt00, dtb_resetb, 0);
				HDMIPHY_WrFL(phy0.hdmi_phy_dbt00);
			#ifdef HDMI_DEBUG_ON
				HDMI_PRINT(" Phy0 dbt reset \n");
			#endif
				HDMIPHY_RdFL(phy0.hdmi_phy_dbt00);
				HDMIPHY_Wr01(phy0.hdmi_phy_dbt00, dtb_resetb, 1);
				HDMIPHY_WrFL(phy0.hdmi_phy_dbt00);

				HDMIPHY_RdFL(phy0.hdmi_phy_rsv01);
				HDMIPHY_Wr01(phy0.hdmi_phy_rsv01, resetb_sel, 0);
				HDMIPHY_WrFL(phy0.hdmi_phy_rsv01);
			}
		}
		HDMI_PRINT("------bx_gHDMI_port_changed ===== %d---------\n",_gHDMI_port_changed);
		goto func_exit;
	}

	if(_gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch])
	{
		HDMI_SetPixelRepetition();			// for specific 13.5MHz input(480i, 576i)
		//HDMI_SetAutoSyncPolarity();			// sync inversion
		HDMI_L8Bx_GetCSCStatus();
	}

	bufTiming.hFreq		= 0; 				///< Horizontal frequency(100 Hz unit) = Dot Freq / hTotal
	bufTiming.vFreq		= 0; 				///< Veritical frequency(1/10 Hz unit) = hFreq / vTotal

	VPORT_RdFL(hdmi_link_080);
	VPORT_Rd01(hdmi_link_080, reg_h_tot, bufTiming.hTotal);			///< Horizontal total pixels
	VPORT_Rd01(hdmi_link_080, reg_v_tot, bufTiming.vTotal);			///< Vertical total lines

	VPORT_RdFL(hdmi_link_081);
	VPORT_Rd01(hdmi_link_081, reg_h_av, bufTiming.hActive);			///< Horizontal active pixel
	VPORT_Rd01(hdmi_link_081, reg_v_av, bufTiming.vActive);			///< Vertical active lines
/*
	VPORT_RdFL(hdmi_link_082);
	VPORT_Rd01(hdmi_link_082, reg_h_fp, bufTiming.hStart);			///< Horizontal start pixel (Front Porch)
	VPORT_Rd01(hdmi_link_082, reg_v_fp, bufTiming.vStart);			///< Vertical start lines (Front Porch)
*/
	VPORT_RdFL(hdmi_link_083);
	VPORT_Rd01(hdmi_link_083, reg_h_bp, bufTiming.hStart);			///< Horizontal start pixel (Back Porch)
	VPORT_Rd01(hdmi_link_083, reg_v_bp, bufTiming.vStart);			///< Vertical start lines (Back Porch)

	VPORT_RdFL(hdmi_link_085);
	VPORT_Rd01(hdmi_link_085, reg_intrl, bufTiming.scanType);		///< Scan type (0 : interlace, 1 : progressive)
	bufTiming.scanType 	^= 1;

	VPORT_RdFL(hdmi_link_001);
	VPORT_Rd01(hdmi_link_001, int_auth_done, hdcpAuth);

	phyPort = (prt_sel < 2) ? 0 : 1;

	if (phyPort == 0)
	{
		HDMIPHY_RdFL(phy0.hdmi_phy_ro05);
		HDMIPHY_Rd01(phy0.hdmi_phy_ro05,tmds_freq_15_8,up_freq);

		HDMIPHY_RdFL(phy0.hdmi_phy_ro06);
		HDMIPHY_Rd01(phy0.hdmi_phy_ro06,tmds_freq_7_0,down_freq);
	}
	else
	{
		HDMIPHY_RdFL(phy1.hdmi_phy_ro05);
		HDMIPHY_Rd01(phy1.hdmi_phy_ro05,tmds_freq_15_8,up_freq);

		HDMIPHY_RdFL(phy1.hdmi_phy_ro06);
		HDMIPHY_Rd01(phy1.hdmi_phy_ro06,tmds_freq_7_0,down_freq);
	}

	tmdsClock = ((up_freq << 8) + down_freq);

	VPORT_RdFL(hdmi_link_085);
	VPORT_Rd01(hdmi_link_085, reg_cdepth, colorDepth);

	VPORT_RdFL(hdmi_link_079);
	VPORT_Rd01(hdmi_link_079, reg_pr, pixelRepet);

	if(_gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch])
	{
		if(mode == 1 && _gHDMI_DVI_Done < gBootData->hdmiCn[gBootData->hdmi_switch]) // HDMI Mode
		{
			//HDMI_ERROR("------bx_gHDMI_DVI_Done ===== %d[%d]---------\n",_gHDMI_DVI_Done,_gHDMI_Change);
			if(hdcpAuth == 0)
			{
				//HDMI_ERROR("------bx_gHDMI_DVI_Done HDMI  ===== %d]---------\n",_gHDMI_DVI_Done);
				bufTiming.hStart = bufTiming.hStart - _gHDMI_DVI_Done;					///< Horizontal start pixel (Back Porch)
				bufTiming.vStart = bufTiming.vStart + _gHDMI_DVI_Done; 					///< Vertical start lines (Back Porch)
				_gHDMI_Change = _gHDMI_DVI_Done;
			}
			else
			{
				//HDMI_ERROR("------bx_gHDMI_DVI_Done HDMI Auth  ===== %d]---------\n",_gHDMI_DVI_Done);
				if(_gHDMI_DVI_Done < _gHDMI_Change+3)
				{
					bufTiming.hStart = bufTiming.hStart - _gHDMI_DVI_Done;				///< Horizontal start pixel (Back Porch)
					bufTiming.vStart = bufTiming.vStart + _gHDMI_DVI_Done; 				///< Vertical start lines (Back Porch)
				}
			}
		}
		else		// DVI Mode
		{
			//HDMI_ERROR("------bx_gHDMI_DVI_Done ===== DVI %d  ---------\n",_gHDMI_DVI_Done);
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

	if(bufTiming.vActive == 1103 || bufTiming.vActive == 2228) 	// 3D - Interlace Format Setting
	{
		if( HDMI_L8Bx_Get3DScanType( ) )
			bufTiming.scanType = 0;
	}

#if 0
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
		HDMI_ERROR(" Timinginfo   pre scanType = %d //  buf scanType = %d \n", _gPrevTiming.scanType, bufTiming.scanType);
#endif

	if(_gHDMI_DVI_Done > gBootData->timingCn[gBootData->hdmi_switch] - 1)
	{
		if(_gPrevTiming.hTotal	!= bufTiming.hTotal)		count++;
		if(_gPrevTiming.vTotal	!= bufTiming.vTotal)		count++;
		if(_gPrevTiming.hActive	!= bufTiming.hActive)		count++;
		if(_gPrevTiming.vActive	!= bufTiming.vActive)		count++;

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

				_gTimingStatus = 1;
				HDMI_ERROR("== Timinginfo ERROR ==\n");
			}
		}
	}
	// InValid Format Check
	if(	(bufTiming.vActive < 240)  || (bufTiming.hActive < 320) ||
		(bufTiming.vActive > 2970) || (bufTiming.hActive > 3840) )
	{
		//memset(&bufTiming , 0 , sizeof(LX_HDMI_TIMING_INFO_T));
		goto func_exit;
	}

//////////////* Horizontal & Vertical frequency //////////////////////////
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

#ifdef HDMI_DEBUG_ON
	if(_gHDMI_DVI_Done == (gBootData->timingCn[gBootData->hdmi_switch]-1))
	{
		HDMI_PRINT(" Timinginfo [TMDS = %d] [vTotal = %d]  [hTotal = %d] \n", tmdsClock, bufTiming.vTotal, bufTiming.hTotal);
		HDMI_PRINT(" 			[ColorDepth = %d] [pixelRepet = %d] [Scan = %s]\n", colorDepth, pixelRepet,(bufTiming.scanType ? "P" : "I"));
		HDMI_PRINT(" Timinginfo V Freq = = %d //  H Freq = = %d \n", bufTiming.vFreq, bufTiming.hFreq);
	}
#endif
/*
	if ( _gPrevTiming.vFreq	!= bufTiming.vFreq )
	{
		if((bufTiming.vFreq - _gPrevTiming.vFreq) == 1)
			bufTiming.vFreq --;
		else if((_gPrevTiming.vFreq - bufTiming.vFreq ) == 1)
			bufTiming.vFreq ++;
	}

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

#ifdef HDMI_DEBUG_ON
	#if 0		// AVI
		memcpy((void *)&aviInfo , (void *)&gVportRegBx->hdmi_link_035 , sizeof(unsigned int)*1);
		if(_gPrevAVI != aviInfo)
			HDMI_ERROR(" bx _gPrevAVI = [0x%x] -> [0x%x]\n", _gPrevAVI, aviInfo);
		_gPrevAVI = aviInfo;
	#endif
#endif

	return ret;
}

/**
 * HDMI_L8Bx_GetStatus
 *
 * @parm LX_HDMI_STATUS_T
 * @return int
*/
int HDMI_L8Bx_GetStatus(LX_HDMI_STATUS_T *status)
{
	int ret = 0;

	status->srcIdx = LX_HDMI_MAIN_WINDOW;

	VPORT_RdFL(hdmi_link_008);
	VPORT_Rd01(hdmi_link_008, reg_hdmi_mode, status->bHdmiMode);	// HDMI mode
	VPORT_Rd01(hdmi_link_008, reg_prt_sel, status->u8hdmiport);		// port number

	VPORT_RdFL(hdmi_link_085);
	VPORT_Rd01(hdmi_link_085, reg_cdepth, status->eColorDepth);		// Deep Color

	switch(status->u8hdmiport)	// HPD enable status
	{
		case 0:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Rd01(hdmi_link_099, phy3p_hpd0_i, status->eHotPlug);
			break;

		case 1:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Rd01(hdmi_link_099, phy3p_hpd1_i, status->eHotPlug);
			break;

		case 2:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Rd01(hdmi_link_099, phy3p_hpd2_i, status->eHotPlug);
			break;

		case 3:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Rd01(hdmi_link_099, phy3p_hpd3_i, status->eHotPlug);
			break;

		default:
			HDMI_PRINT("WARNING : port number is out of range\n");
		break;
	}

	status->pixelRepet = 0;
	status->csc = 0;

	return ret;
}

/**
 * HDMI_GetVsiPacket
 *
 * @parm LX_HDMI_VSI_PACKET_T
 * @return int
*/
int HDMI_L8Bx_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet)
{
	int ret = 0;
	UINT32 buf[6];
	UINT32 *addr;

	// VSI packet data structure
	struct{
		UINT32	PacketType	: 8;	// data8 	- VSI Packet Header
		UINT32	Version		: 8;
		UINT32	Length		: 5;
		UINT32	resv0		: 11;

		UINT32	Meta_data17	: 8;	// data7	- VSI Packet Data_1
		UINT32	Meta_data18	: 8;
		UINT32	Meta_data19	: 8;
		UINT32	Meta_data20	: 8;

		UINT32	Meta_data13	: 8;	// data6	- VSI Packet Data_2
		UINT32	Meta_data14	: 8;
		UINT32	Meta_data15	: 8;
		UINT32	Meta_data16	: 8;

		UINT32	Meta_data9	: 8;	// data5	- VSI Packet Data_3
		UINT32	Meta_data10	: 8;
		UINT32	Meta_data11	: 8;
		UINT32	Meta_data12	: 8;

		UINT32	Meta_data5	: 8;	// data4	- VSI Packet Data_4
		UINT32	Meta_data6	: 8;
		UINT32	Meta_data7	: 8;
		UINT32	Meta_data8	: 8;

		UINT32	Meta_data1	: 8;	// data3	- VSI Packet Data_5
		UINT32	Meta_data2	: 8;
		UINT32	Meta_data3	: 8;
		UINT32	Meta_data4	: 8;

		UINT32	resv1		: 5;	// data2	- VSI Packet Data_6
		UINT32	HDMI_Video_Format	: 3;
		UINT32	resv2		: 3;
		UINT32	Meta_present	: 1;
		UINT32	Structure	: 4;
		UINT32	resv3		: 4;
		UINT32	Ext_Data	: 4;
		UINT32	Meta_length	: 5;
		UINT32	Meta_Type	: 3;

		UINT32	CheckSum	: 8;	// data1	- VSI Packet Data_7
		UINT32	IEEERegister	: 24;
	}VSIpacket;

	//if(!HDMI_GetPortConnection())
	if (!_gSCDT) { memset((void *)packet , 0 , sizeof(LX_HDMI_VSI_PACKET_T)); goto func_exit; }

#if 1
	addr = (UINT32 *)&gVportRegBx->hdmi_link_113;
	memcpy((void *)&VSIpacket , (void *)addr , sizeof(unsigned int)*8);
#else
	memcpy((void *)&VSIpacket , (void *)&gVportRegBx->hdmi_link_113 , sizeof(unsigned int)*8);
#endif
	//packet inform
	packet->eVsiPacket.packet.type = VSIpacket.PacketType;
	packet->eVsiPacket.packet.version = VSIpacket.Version;
	packet->eVsiPacket.packet.length = VSIpacket.Length;

	// packet status
	packet->eVsiPacket.packetStatus = LX_HDMI_VSI_PACKET_STATUS_UPDATED;

	// IEE
	packet->eVsiPacket.IEERegId[0] = (UINT8)(VSIpacket.IEEERegister & 0xff);
	packet->eVsiPacket.IEERegId[1] = (UINT8)((VSIpacket.IEEERegister & 0xff00)>>8);
	packet->eVsiPacket.IEERegId[2] = (UINT8)((VSIpacket.IEEERegister & 0xff0000)>>16);

	// Packet Data(from checksum(PB0)) & Payload(from PB4)
	packet->eVsiPacket.packet.dataBytes[0] = VSIpacket.CheckSum;
	packet->eVsiPacket.packet.dataBytes[1] = packet->eVsiPacket.IEERegId[0];
	packet->eVsiPacket.packet.dataBytes[2] = packet->eVsiPacket.IEERegId[1];
	packet->eVsiPacket.packet.dataBytes[3] = packet->eVsiPacket.IEERegId[2];

	VPORT_RdFL(hdmi_link_119);
	VPORT_RdFL(hdmi_link_118);
	VPORT_RdFL(hdmi_link_117);
	VPORT_RdFL(hdmi_link_116);
	VPORT_RdFL(hdmi_link_115);
	VPORT_RdFL(hdmi_link_114);

	VPORT_Rd01(hdmi_link_119, reg_pkt_vsi_dat__6, buf[0]);
	VPORT_Rd01(hdmi_link_118, reg_pkt_vsi_dat__5, buf[1]);
	VPORT_Rd01(hdmi_link_117, reg_pkt_vsi_dat__4, buf[2]);
	VPORT_Rd01(hdmi_link_116, reg_pkt_vsi_dat__3, buf[3]);
	VPORT_Rd01(hdmi_link_115, reg_pkt_vsi_dat__2, buf[4]);
	VPORT_Rd01(hdmi_link_114, reg_pkt_vsi_dat, buf[5]);

	memcpy((void *)(&packet->eVsiPacket.packet.dataBytes[4]) , (void *)(&buf) , 24);
	memcpy((void *)(&packet->eVsiPacket.PayLoad[0]) , (void *)(&buf) , 24);

	packet->eVsiPacket.eVideoFormat	= VSIpacket.HDMI_Video_Format;
	packet->eVsiPacket.e3DStructure	= VSIpacket.Structure;
	packet->eVsiPacket.e3DExtData	= VSIpacket.Ext_Data;
	packet->eVsiPacket.eVSiVIC		= packet->eVsiPacket.packet.dataBytes[5];

func_exit:
	packet->srcIdx = LX_HDMI_MAIN_WINDOW;

#if 0
	if(gVportRegBx->hdmi_link_113.reg_pkt_vsi_hdr != 0)
	{
		HDMI_DEBUG("#### VSI Packet ####\n");

		HDMI_DEBUG("Packet.type: [0x%x]\n", packet->eVsiPacket.packet.type);
		HDMI_DEBUG("Packet.version: [0x%x]\n", packet->eVsiPacket.packet.version);
		HDMI_DEBUG("Packet.length: [0x%x]\n", packet->eVsiPacket.packet.length);
		HDMI_DEBUG("VideoFormat: [0x%x]\n", packet->eVsiPacket.eVideoFormat);
		HDMI_DEBUG("3DStructure: [0x%x]\n", packet->eVsiPacket.e3DStructure);
		HDMI_DEBUG("3DExtData: [0x%x]\n", packet->eVsiPacket.e3DExtData);
		HDMI_DEBUG("\n");
	}
#endif

	return ret;
}

/**
 * HDMI_GetAviPacket
 *
 * @parm LX_HDMI_AVI_PACKET_T
 * @return int
*/
int HDMI_L8Bx_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet)
{
	int ret = 0;
	unsigned int *data;
	UINT32 *addr;

	// AVI packet data structure
	struct{
		UINT32	type		: 8;		// Packet Header
		UINT32	version		: 8;
		UINT32	length		: 8;
		UINT32	resv4		: 8;

		UINT32	data6		: 32;		// data1
		UINT32	data5		: 32;		// data2
		UINT32	data4		: 32;		// data3

		UINT32	PNSR		: 16;		// data4
		UINT32	resv3		: 16;

		UINT32	LNSB		: 16;		// data5
		UINT32	PNEL		: 16;

		UINT32	VIC06		: 7;		// data6
		UINT32	resv1		: 1;
		UINT32	PR03		: 4;
		UINT32	CN			: 2;
		UINT32	YQ			: 2;
		UINT32	LNET		: 16;

		UINT32	CheckSum	: 8;		// data7
		UINT32	S01		: 2;
		UINT32	B01		: 2;
		UINT32	A0		: 1;
		UINT32	Y01		: 2;
		UINT32	resv0		: 1;
		UINT32	R03		: 4;
		UINT32	M01		: 2;
		UINT32	C01		: 2;
		UINT32	SC01		: 2;
		UINT32	Q01		: 2;
		UINT32	EC03		: 3;
		UINT32	ITC		: 1;

	} AVIpacket;

	//if(!HDMI_GetPortConnection())
	if (!_gSCDT) { memset((void *)packet , 0 , sizeof(LX_HDMI_AVI_PACKET_T)); goto func_exit; }

#if 1
	addr = (UINT32 *)&gVportRegBx->hdmi_link_028;
	memcpy((void *)&AVIpacket , (void *)addr , sizeof(unsigned int)*8);
#else
	memcpy((void *)&AVIpacket , (void *)&gVportRegBx->hdmi_link_028 , sizeof(unsigned int)*8);
#endif



	packet->eAviPacket.ePixelEncoding = AVIpacket.Y01;
	packet->eAviPacket.eActiveInfo = AVIpacket.A0;
	packet->eAviPacket.eBarInfo = AVIpacket.B01;
	packet->eAviPacket.eScanInfo = AVIpacket.S01;
	packet->eAviPacket.eColorimetry = AVIpacket.C01;
	packet->eAviPacket.ePictureAspectRatio = AVIpacket.M01;
	packet->eAviPacket.eActiveFormatAspectRatio = AVIpacket.R03;
	packet->eAviPacket.eITContent = AVIpacket.ITC;
	packet->eAviPacket.eExtendedColorimetry = AVIpacket.EC03;
	packet->eAviPacket.eRGBQuantizationRange = AVIpacket.Q01;
	packet->eAviPacket.eScaling = AVIpacket.SC01;

	packet->eAviPacket.VideoIdCode = AVIpacket.VIC06;
	packet->eAviPacket.eYCCQuantizationRange = AVIpacket.YQ;
	packet->eAviPacket.eContentType = AVIpacket.CN;
	packet->eAviPacket.PixelRepeat = AVIpacket.PR03;

	packet->eAviPacket.TopBarEndLineNumber = AVIpacket.LNET;
	packet->eAviPacket.BottomBarStartLineNumber = AVIpacket.LNSB;
	packet->eAviPacket.LeftBarEndPixelNumber = AVIpacket.PNEL;
	packet->eAviPacket.RightBarEndPixelNumber = AVIpacket.PNSR;

	//packet->eAviPacket.packetStatus = ;
	packet->eAviPacket.packet.type = AVIpacket.type;
	packet->eAviPacket.packet.version = AVIpacket.version;
	packet->eAviPacket.packet.length = AVIpacket.length;

	data = (unsigned int *)kmalloc(sizeof(int)*7 , GFP_KERNEL);

	VPORT_RdFL(hdmi_link_029);
	VPORT_RdFL(hdmi_link_030);
	VPORT_RdFL(hdmi_link_031);
	VPORT_RdFL(hdmi_link_032);
	VPORT_RdFL(hdmi_link_033);
	VPORT_RdFL(hdmi_link_034);
	VPORT_RdFL(hdmi_link_035);

	VPORT_Rd01(hdmi_link_029, reg_pkt_avi_dat, *(data + 6));
	VPORT_Rd01(hdmi_link_030, reg_pkt_avi_dat__2, *(data + 5));
	VPORT_Rd01(hdmi_link_031, reg_pkt_avi_dat__3, *(data + 4));
	VPORT_Rd01(hdmi_link_032, reg_pkt_avi_dat__4, *(data + 3));
	VPORT_Rd01(hdmi_link_033, reg_pkt_avi_dat__5, *(data + 2));
	VPORT_Rd01(hdmi_link_034, reg_pkt_avi_dat__6, *(data + 1));
	VPORT_Rd01(hdmi_link_035, reg_pkt_avi_dat__7, *(data + 0));

	memcpy((void *)&packet->eAviPacket.packet.dataBytes , (void *)data ,  sizeof(char)*28);

	packet->eAviPacket.bHdmiMode = gBootData->mode;

	kfree(data);

func_exit:
	packet->srcIdx = LX_HDMI_MAIN_WINDOW;

	return ret;
}

/**
 * HDMI_GetSpdPacket
 *
 * @parm LX_HDMI_SPD_PACKET_T
 * @return int
*/
int HDMI_L8Bx_GetSpdPacket(LX_HDMI_SPD_PACKET_T *packet)
{
	int ret = 0;
	UINT8 *data;

	HDMI_PRINT("%s: Get SPD Packet", __func__);

	if (!_gSCDT) { memset((void *)packet , 0 , sizeof(LX_HDMI_SPD_PACKET_T)); goto func_exit; }

	data = (UINT8 *)kmalloc(sizeof(UINT32) * 7 , GFP_KERNEL);

	memcpy((void *)(data + 0), (void *)&gVportRegBx->hdmi_link_043.reg_pkt_spd_dat__7, sizeof(UINT32));
	memcpy((void *)(data + 4), (void *)&gVportRegBx->hdmi_link_042.reg_pkt_spd_dat__6, sizeof(UINT32));
	memcpy((void *)(data + 8), (void *)&gVportRegBx->hdmi_link_041.reg_pkt_spd_dat__5, sizeof(UINT32));
	memcpy((void *)(data + 12), (void *)&gVportRegBx->hdmi_link_040.reg_pkt_spd_dat__4, sizeof(UINT32));
	memcpy((void *)(data + 16), (void *)&gVportRegBx->hdmi_link_039.reg_pkt_spd_dat__3, sizeof(UINT32));
	memcpy((void *)(data + 20), (void *)&gVportRegBx->hdmi_link_038.reg_pkt_spd_dat__2, sizeof(UINT32));
	memcpy((void *)(data + 24), (void *)&gVportRegBx->hdmi_link_037.reg_pkt_spd_dat, sizeof(UINT32));

	memcpy(packet->VendorName, (void *)(data + 1), LX_HDMI_SPD_VENDOR_NAME_LEN);
	memcpy(packet->ProductDescription, (void *)(data + LX_HDMI_SPD_VENDOR_NAME_LEN + 1), LX_HDMI_SPD_PRODUCT_DESC_LEN);
	packet->SourceDeviceInfo = (UINT8)*(data + LX_HDMI_SPD_VENDOR_NAME_LEN + LX_HDMI_SPD_PRODUCT_DESC_LEN + 1);

#if 0
	HDMI_DEBUG("VendorName: [%s]\n", packet->VendorName);
	HDMI_DEBUG("ProductDes: [%s]\n", packet->ProductDescription);
	HDMI_DEBUG("SourceInfo: [%d]\n", packet->SourceDeviceInfo);
#endif

	kfree(data);

func_exit:
	packet->srcIdx = LX_HDMI_MAIN_WINDOW;

	return ret;
}

/**
 * HDMI_SetHPDEnDisable
 *
 * @parm LX_HDMI_HPD_T
 * @return int
*/
int HDMI_L8Bx_SetHPDEnDisable(LX_HDMI_HPD_T *hpd)
{
	int ret = 0;

	//_writew(&gVportRegBx->hdmi_link_099, 0x1111);			// HPD PIN Disable(HPD:High) : 0
	switch(hpd->u8HDMIPort)
	{
		case 0:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Wr01(hdmi_link_099, phy3p_hpd0_i, !hpd->bEnable);
			VPORT_WrFL(hdmi_link_099);
			break;

		case 1:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Wr01(hdmi_link_099, phy3p_hpd1_i, !hpd->bEnable);
			VPORT_WrFL(hdmi_link_099);
			break;

		case 2:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Wr01(hdmi_link_099, phy3p_hpd2_i, !hpd->bEnable);
			VPORT_WrFL(hdmi_link_099);
			break;

		case 3:
			VPORT_RdFL(hdmi_link_099);
			VPORT_Wr01(hdmi_link_099, phy3p_hpd3_i, !hpd->bEnable);
			VPORT_WrFL(hdmi_link_099);
			break;

		default:
			HDMI_ERROR("WARNING : port number is out of range\n");
		break;
	}

	HDMI_PRINT("[HDMI HW] HDMI_SetHPDEnDisable Port[%d] En[%d] \n",hpd->u8HDMIPort, hpd->bEnable);

	return ret;
}

/**
 * HDMI_SetPort
 *
 * @parm LX_HDMI_HPD_T
 * @return int
*/
int HDMI_L8Bx_SetPort(LX_HDMI_HPD_T *hpd)
{
	int ret = 0;
	HDMI_PHY_TYPE_T type;

#ifdef	KDRV_CONFIG_PM
	memcpy( (void *)&gstPmPortSel , (void *)hpd , sizeof(LX_HDMI_HPD_T) );
#endif

	if(gBootData->hdmi_switch)
 		hpd->u8HDMIPort= gBootData->SWport;

	if(hpd->u8HDMIPort < 4)
	{
		/* HDMI Link Reset - Video Format Info.*/
		VPORT_RdFL(top_010);
		//VPORT_Wr01(top_010, reg_swrst_linksys, 1);
		VPORT_Wr01(top_010, reg_swrst_linktmds, 1);
		VPORT_Wr01(top_010, reg_swrst_linkhdcp, 1);
		VPORT_Wr01(top_010, reg_swrst_linkpix, 1);
		VPORT_WrFL(top_010);

		/* Port	Power down control */
		HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);

		if(hpd->u8HDMIPort < 2)
			type = 	HDMI_PHY0_PORT;
		else
			type = 	HDMI_PHY1_PORT;
		//mdelay(300);	// ms delay

		// try to do power on without status check
		HDMI_SetPowerControl(type , 1);

		/* Port Select */
		VPORT_RdFL(hdmi_link_008);
		VPORT_Wr01(hdmi_link_008, reg_prt_sel, hpd->u8HDMIPort);	// Port
		VPORT_WrFL(hdmi_link_008);

		gBootData->pixelRepet = gBootData->mode = 0;
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
		_gHDMI_DVI_Done = _gHDMI_Change = 0;

		//mdelay(50);
		VPORT_RdFL(top_010);
		VPORT_Wr(top_010, 0x00000000);		// All Link Reset Clear
		VPORT_WrFL(top_010);
	}
	else
	{
		/* Port	Power down control */
		HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);
		//_writew(&gVportRegBx->hdmi_link_099, 0x1111);			// HPD PIN Disable(HPD:High) : 0

		/* Port Select */
		VPORT_RdFL(hdmi_link_008);
		VPORT_Wr01(hdmi_link_008, reg_prt_sel, hpd->u8HDMIPort);
		VPORT_WrFL(hdmi_link_008);
	}
	return ret;
}

/**
 * HDMI_SetPowerDown
 *
 * @parm HDMI_PHY_TYPE_T
 * @return int
*/
int HDMI_L8Bx_SetPowerControl(HDMI_PHY_TYPE_T type , int power)
{
	int ret = 0;
	UINT8 	pdbAll[2]	= {0};
	UINT8 	resetAll[2] = {0};

	if(power == 0) // Power down
	{
		pdbAll[0] = resetAll[0] = pdbAll[1] = resetAll[1] = 0;
	}
	else	// power up
	{
		if(type > 1)
			pdbAll[0] = resetAll[0] = pdbAll[1] = resetAll[1] = 1;
		else
			pdbAll[type] = resetAll[type] = 1;
	}

	HDMIPHY_RdFL(phy0.hdmi_phy_rsv00);
	HDMIPHY_Wr01(phy0.hdmi_phy_rsv00, pdb_all, pdbAll[0]);
	HDMIPHY_Wr01(phy0.hdmi_phy_rsv00, resetb_all, resetAll[0]);
	HDMIPHY_WrFL(phy0.hdmi_phy_rsv00);

	HDMIPHY_RdFL(phy1.hdmi_phy_rsv00);
	HDMIPHY_Wr01(phy1.hdmi_phy_rsv00, pdb_all, pdbAll[1]);
	HDMIPHY_Wr01(phy1.hdmi_phy_rsv00, resetb_all, resetAll[1]);
	HDMIPHY_WrFL(phy1.hdmi_phy_rsv00);

	if(power == 0) // Power down
	{
		HDMIPHY_RdFL(phy0.hdmi_phy_pll03);
		HDMIPHY_Wr01(phy0.hdmi_phy_pll03, pi_pclk_div2, 0);
		HDMIPHY_Wr01(phy0.hdmi_phy_pll03, pi_pclk_div4, 0);
		HDMIPHY_WrFL(phy0.hdmi_phy_pll03);

		HDMIPHY_RdFL(phy1.hdmi_phy_pll03);
		HDMIPHY_Wr01(phy1.hdmi_phy_pll03, pi_pclk_div2, 0);
		HDMIPHY_Wr01(phy1.hdmi_phy_pll03, pi_pclk_div4, 0);
		HDMIPHY_WrFL(phy1.hdmi_phy_pll03);
	}

	return ret;
}

int HDMI_L8Bx_RunReset(void)
{
	HDMI_LINK_007	st_hdmi_link_007;

	VPORT_RdFL(hdmi_link_009);
	VPORT_Wr01(hdmi_link_009, reg_dbg_ef_bak, 1);			// not used hdcp key ROM
	VPORT_Wr01(hdmi_link_009, reg_dbg_acr_soft, 1);
	VPORT_WrFL(hdmi_link_009);

	// hdmi link Reset - All Clear
	VPORT_RdFL(top_010);
	VPORT_Wr(top_010, 0x00000000);
	VPORT_WrFL(top_010);

	// interrupt enable
	//gVportRegBx->intr_vp3_enable.intr_vp3_enable = 0x01fbffff;
	//gVportRegBx->intr_vp2_enable.intr_vp2_enable = 0xffffffff;

	// HDMI Current Default value 2(50uA) -> 1(60uA)- by sunghyun.yang
	HDMIPHY_RdFL(phy0.hdmi_phy_dr04);
	HDMIPHY_Wr01(phy0.hdmi_phy_dr04, dr_ictrl, 1);		// DR Bias Current
	HDMIPHY_WrFL(phy0.hdmi_phy_dr04);

	HDMIPHY_RdFL(phy0.hdmi_phy_dr03);
	HDMIPHY_Wr01(phy0.hdmi_phy_dr03, dr_mode, 1);		// DR Capacitance Selection
	HDMIPHY_WrFL(phy0.hdmi_phy_dr03);

	HDMIPHY_RdFL(phy0.hdmi_phy_cr04);
	HDMIPHY_Wr01(phy0.hdmi_phy_cr04, cr_test, 1);		// PLL_Mode_Sel Auto Reset
	HDMIPHY_WrFL(phy0.hdmi_phy_cr04);


	HDMIPHY_RdFL(phy1.hdmi_phy_dr04);
	HDMIPHY_Wr01(phy1.hdmi_phy_dr04, dr_ictrl, 1);
	HDMIPHY_WrFL(phy1.hdmi_phy_dr04);

	HDMIPHY_RdFL(phy1.hdmi_phy_dr03);
	HDMIPHY_Wr01(phy1.hdmi_phy_dr03, dr_mode, 1);
	HDMIPHY_WrFL(phy1.hdmi_phy_dr03);

	HDMIPHY_RdFL(phy1.hdmi_phy_cr04);
	HDMIPHY_Wr01(phy1.hdmi_phy_cr04, cr_test, 1);
	HDMIPHY_WrFL(phy1.hdmi_phy_cr04);

#ifdef HDMI_L8B0_SWITCH_ADAPTIVE_LOOP
	if(gBootData->hdmi_switch)
	{
		HDMIPHY_RdFL(phy0.hdmi_phy_eq02);
		HDMIPHY_Wr01(phy0.hdmi_phy_eq02, eq_cs_sel, 0);
		HDMIPHY_Wr01(phy0.hdmi_phy_eq02, eq_rs_sel, 0);
		HDMIPHY_WrFL(phy0.hdmi_phy_eq02);

		HDMIPHY_RdFL(phy0.hdmi_phy_eq04);
		HDMIPHY_Wr01(phy0.hdmi_phy_eq04, eq_rs, 3);
		HDMIPHY_WrFL(phy0.hdmi_phy_eq04);

		HDMIPHY_RdFL(phy0.hdmi_phy_eq05);
		HDMIPHY_Wr01(phy0.hdmi_phy_eq05, eq_cs, 5);
		HDMIPHY_WrFL(phy0.hdmi_phy_eq05);

		//pstHdmiPhyReg->phy0.hdmi_phy_eq13.eq_af_en = 0;

		HDMIPHY_RdFL(phy1.hdmi_phy_eq02);
		HDMIPHY_Wr01(phy1.hdmi_phy_eq02, eq_cs_sel, 0);
		HDMIPHY_Wr01(phy1.hdmi_phy_eq02, eq_rs_sel, 0);
		HDMIPHY_WrFL(phy1.hdmi_phy_eq02);

		HDMIPHY_RdFL(phy1.hdmi_phy_eq04);
		HDMIPHY_Wr01(phy1.hdmi_phy_eq04, eq_rs, 3);
		HDMIPHY_WrFL(phy1.hdmi_phy_eq04);

		HDMIPHY_RdFL(phy1.hdmi_phy_eq05);
		HDMIPHY_Wr01(phy1.hdmi_phy_eq05, eq_cs, 5);
		HDMIPHY_WrFL(phy1.hdmi_phy_eq05);

		//pstHdmiPhyReg->phy1.hdmi_phy_eq13.eq_af_en = 0;
	}
#endif

	// HDCP Problem to the Master
	HDMIPHY_RdFL(phy0.hdmi_phy_eq12);
	HDMIPHY_Wr01(phy0.hdmi_phy_eq12, eq_start_frz, 47);
	HDMIPHY_WrFL(phy0.hdmi_phy_eq12);

	HDMIPHY_RdFL(phy1.hdmi_phy_eq12);
	HDMIPHY_Wr01(phy1.hdmi_phy_eq12, eq_start_frz, 47);
	HDMIPHY_WrFL(phy1.hdmi_phy_eq12);

	HDMIPHY_RdFL(phy0.hdmi_phy_eq13);
	HDMIPHY_Wr01(phy0.hdmi_phy_eq13, eq_avg_width, 1);
	HDMIPHY_WrFL(phy0.hdmi_phy_eq13);

	HDMIPHY_RdFL(phy1.hdmi_phy_eq13);
	HDMIPHY_Wr01(phy1.hdmi_phy_eq13, eq_avg_width, 1);
	HDMIPHY_WrFL(phy1.hdmi_phy_eq13);

  	// Field select(short back Porch) for Interlace mode
	VPORT_RdFL(hdmi_link_079);
  	VPORT_Wr01(hdmi_link_079, reg_lbp_f2, 0);
  	VPORT_WrFL(hdmi_link_079);

	//Read HDMI_LINK_007 reg.
	memcpy (&st_hdmi_link_007, (UINT32*) &gVportRegBx->hdmi_link_007, sizeof (UINT32));

	//Video Path Auto Configuration Enable
	st_hdmi_link_007.reg_avc_en = 1;

	//Write HDMI_LINK_007 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_007, &st_hdmi_link_007, sizeof (UINT32));


	// HPD PIN Control Enable : add sh.myoung 2010.09.30
	_writew(&gVportRegBx->hdmi_link_098, 0x0000);			// HPD PIN Enable

	return 0;
}

int HDMI_L8Bx_HWInitial(void)
{

#ifndef	USE_QEMU_SYSTEM
	pstHdmiPhyReg =(HDMI_PHY3P_Reg_T *)ioremap(pHdmiRegCfg->phy_reg_base_addr, pHdmiRegCfg->phy_reg_size);
#else
	pstHdmiPhyReg =(HDMI_PHY3P_Reg_T *)ioremap(pHdmiRegCfg->phy_qemu_base_addr, pHdmiRegCfg->phy_reg_size);
#endif

	if(pstHdmiPhyReg == NULL)
	{
		HDMI_ERROR("ERROR : can't allocate for register\n");
		return -1;
	}

	pstHdmiPhyShadowReg = (HDMI_PHY3P_Reg_T *)kmalloc(sizeof(HDMI_PHY3P_Reg_T), GFP_KERNEL);
	if (pstHdmiPhyShadowReg == NULL)
	{
		HDMI_ERROR("ERROR : can't allocate for shadow register\n");
		return -1;
	}

#ifdef	KDRV_CONFIG_PM
	gstPmPortSel.u8HDMIPort = 0xFF;
#endif

	HDMI_RunReset();

	// audio setting
	HDMI_SetAudio();

	return 0;
}

/**
 * HDMI_SetAudio
 *
 * @parm void
 * @return int
*/
int HDMI_L8Bx_SetAudio(void)
{
	TOP_011 		st_top_011;
	TOP_013 		st_top_013;

	HDMI_PHY_PLL15	st_hdmi_pll15;

	HDMI_LINK_007	st_hdmi_link_007;
	HDMI_LINK_009	st_hdmi_link_009;
	HDMI_LINK_087	st_hdmi_link_087;
	HDMI_LINK_092	st_hdmi_link_092;
	HDMI_LINK_093	st_hdmi_link_093;
	HDMI_LINK_094	st_hdmi_link_094;

#ifdef USE_HDMI_AUDIO_PLL

	/* Set Clock Division CTR28 */
	HDMI_PRINT("CTOP RD R: 0x80(CTR28) : 0x%X\n", CTOP_CTRL_READ(0x80));
	CTOP_CTRL_WRITE(0x80, CTOP_CTRL_READ(0x80) | 0x0040);	//linkapll_clk_div : 0b01
	//CTOP_CTRL_WRITE(0x80, CTOP_CTRL_READ(0x80) | 0x0078); 	//Enable HDMI I2S Monitor Signal
	HDMI_PRINT("CTOP RD W: 0x80(CTR28) : 0x%X\n", CTOP_CTRL_READ(0x80));

	//Read HDMI_PHY_PLL15 reg.
	memcpy (&st_hdmi_pll15, (UINT32*) &pstHdmiPhyReg->phy0.hdmi_phy_pll15, sizeof (UINT32));
	memcpy (&st_hdmi_pll15, (UINT32*) &pstHdmiPhyReg->phy1.hdmi_phy_pll15, sizeof (UINT32));

	// Audio PLL Parameter MP : 010  = 256*Fs
	st_hdmi_pll15.aud_mp = 2;

	//Write HDMI_PHY_PLL15 reg.
	memcpy ((UINT32*) &pstHdmiPhyReg->phy0.hdmi_phy_pll15, &st_hdmi_pll15, sizeof (UINT32));
	memcpy ((UINT32*) &pstHdmiPhyReg->phy1.hdmi_phy_pll15, &st_hdmi_pll15, sizeof (UINT32));

	//Read TOP_013 reg.
	memcpy (&st_top_013, (UINT32*) &gVportRegBx->top_013, sizeof (UINT32));

	// Enable Audio 256 * Fs setting
	st_top_013.clksel_linkapll = 0;		// Audio PLL enasble

	//Write TOP_013 reg.
	memcpy ((UINT32*) &gVportRegBx->top_013, &st_top_013, sizeof (UINT32));


	//Read HDMI_LINK_007 reg.
	memcpy (&st_hdmi_link_007, (UINT32*) &gVportRegBx->hdmi_link_007, sizeof (UINT32));

	//Audio Path Auto Configuration Disable(Configure Fs manually)
	st_hdmi_link_007.reg_aac_en = 0;

	//Write HDMI_LINK_007 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_007, &st_hdmi_link_007, sizeof (UINT32));

	//Read HDMI_LINK_009 reg.
	memcpy (&st_hdmi_link_009, (UINT32*) &gVportRegBx->hdmi_link_009, sizeof (UINT32));

	//[Test & Debug] Select Digital Divider instead of PLL
	st_hdmi_link_009.reg_dbg_acr_soft = 0;

	//Write HDMI_LINK_009 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_009, &st_hdmi_link_009, sizeof (UINT32));

	//Read HDMI_LINK_087 reg.
	memcpy (&st_hdmi_link_087, (UINT32*) &gVportRegBx->hdmi_link_087, sizeof (UINT32));

	//Audio clock generation
	st_hdmi_link_087.reg_acr_mp 	  = 0x2;	// 256 * Fs : N value in N * Fs
	st_hdmi_link_087.reg_acr_fs_sel   = 0x1;	// Configure Fs manually : Fs
	st_hdmi_link_087.reg_acr_ncts_sel = 0x0;	// Use source device value : N, CTS

	//Write HDMI_LINK_087 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_087, &st_hdmi_link_087, sizeof (UINT32));

	//Setting PCM / Bitstream I2S ouput
	//Read HDMI_LINK_092 reg.
	memcpy (&st_hdmi_link_092, (UINT32*) &gVportRegBx->hdmi_link_092, sizeof (UINT32));

	//SCK edge : 1 => falling edge for L8 B0, 0 => rising edge
	st_hdmi_link_092.reg_i2s_sck = 1;

	//WS Polarity : 0 => Left Channel in low signal, 1 => Right Channel in low signal
	st_hdmi_link_092.reg_i2s_ws = 0;

	//Write HDMI_LINK_092 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_092, &st_hdmi_link_092, sizeof (UINT32));

	//Read HDMI_LINK_093 reg.
	memcpy (&st_hdmi_link_093, (UINT32*) &gVportRegBx->hdmi_link_093, sizeof (UINT32));

	//I2S output enable
	st_hdmi_link_093.reg_i2s_sd_en     = 0xF;	//I2S SD output enable(Channel 1 ~ 8)
	st_hdmi_link_093.reg_i2s_mclk_en   = 1;		//I2S MCLK output enable
	st_hdmi_link_093.reg_i2s_mute_flat = 1;		//I2S Flat sample and Channel mute in abnormal status

	//Write HDMI_LINK_093 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_093, &st_hdmi_link_093, sizeof (UINT32));

	//Read HDMI_LINK_094 reg.
	memcpy (&st_hdmi_link_094, (UINT32*) &gVportRegBx->hdmi_link_094, sizeof (UINT32));

	//I2S output enable
	st_hdmi_link_094.reg_i2s_gnd_mode = 1;

	//S/PDIF output enable for debug output only
	//st_hdmi_link_094.reg_sp_en = 1;

	//Write HDMI_LINK_092 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_094, &st_hdmi_link_094, sizeof (UINT32));


	//Read TOP_011 reg.
	memcpy (&st_top_011, (UINT32*) &gVportRegBx->top_011, sizeof (UINT32));

	// Audio sw reset
	st_top_011.reg_swrst_adto 	= 0;
	st_top_011.reg_swrst_apll 	= 0;
	st_top_011.reg_swrst_linkaw = 0;
	st_top_011.reg_swrst_linkar = 0;

	//Write TOP_011 reg.
	memcpy ((UINT32*) &gVportRegBx->top_011, &st_top_011, sizeof (UINT32));

#else	//#ifdef USE_HDMI_AUDIO_PLL

	/* Set Clock Division CTR28 */
	HDMI_PRINT("CTOP RD R: 0x80(CTR28) : 0x%X\n", CTOP_CTRL_READ(0x80));
	CTOP_CTRL_WRITE(0x80, CTOP_CTRL_READ(0x80) | 0x4040);		//stat_acr_n : 0b01, linkapll_clk_div : 0b01
	//CTOP_CTRL_WRITE(0x80, CTOP_CTRL_READ(0x80) | 0x4078); //Enable HDMI I2S Monitor Signal
	HDMI_PRINT("CTOP RD W: 0x80(CTR28) : 0x%X\n", CTOP_CTRL_READ(0x80));

	//Read HDMI_PHY_PLL15 reg.
	memcpy (&st_hdmi_pll15, (UINT32*) &pstHdmiPhyReg->phy0.hdmi_phy_pll15, sizeof (UINT32));
	memcpy (&st_hdmi_pll15, (UINT32*) &pstHdmiPhyReg->phy1.hdmi_phy_pll15, sizeof (UINT32));

	//Read TOP_013 reg.
	memcpy (&st_top_013, (UINT32*) &gVportRegBx->top_013, sizeof (UINT32));
	st_top_013.clksel_linkapll = 1;		// Audio PLL disble

	//Write TOP_013 reg.
	memcpy ((UINT32*) &gVportRegBx->top_013, &st_top_013, sizeof (UINT32));


	//Read HDMI_LINK_007 reg.
	memcpy (&st_hdmi_link_007, (UINT32*) &gVportRegBx->hdmi_link_007, sizeof (UINT32));

	//Audio Path Auto Configuration Enable
	st_hdmi_link_007.reg_aac_en = 1;

	//Write HDMI_LINK_007 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_007, &st_hdmi_link_007, sizeof (UINT32));

	//Read HDMI_LINK_009 reg.
	memcpy (&st_hdmi_link_009, (UINT32*) &gVportRegBx->hdmi_link_009, sizeof (UINT32));

	//[Test & Debug] Select Digital Divider instead of PLL
	st_hdmi_link_009.reg_dbg_acr_soft = 1;

	//Write HDMI_LINK_009 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_009, &st_hdmi_link_009, sizeof (UINT32));

	//Read HDMI_LINK_087 reg.
	memcpy (&st_hdmi_link_087, (UINT32*) &gVportRegBx->hdmi_link_087, sizeof (UINT32));

	//Audio clock generation
	st_hdmi_link_087.reg_acr_mp 	  = 0x2;	// 256 * Fs : N value in N * Fs
	st_hdmi_link_087.reg_acr_fs_sel   = 0x0;	// Use source device value : Fs
	st_hdmi_link_087.reg_acr_ncts_sel = 0x0;	// Use source device value : N, CTS

	//Write HDMI_LINK_087 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_087, &st_hdmi_link_087, sizeof (UINT32));

	//Setting PCM / Bitstream I2S ouput
	//Read HDMI_LINK_092 reg.
	memcpy (&st_hdmi_link_092, (UINT32*) &gVportRegBx->hdmi_link_092, sizeof (UINT32));

	//SCK edge : 1 => falling edge for L8 B0, 0 => rising edge
	st_hdmi_link_092.reg_i2s_sck = 1;

	//WS Polarity : 0 => Left Channel in low signal, 1 => Right Channel in low signal
	st_hdmi_link_092.reg_i2s_ws = 0;

	//Write HDMI_LINK_092 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_092, &st_hdmi_link_092, sizeof (UINT32));

	//Read HDMI_LINK_093 reg.
	memcpy (&st_hdmi_link_093, (UINT32*) &gVportRegBx->hdmi_link_093, sizeof (UINT32));

	//I2S output enable
	st_hdmi_link_093.reg_i2s_sd_en     = 0xF;	//I2S SD output enable(Channel 1 ~ 8)
	st_hdmi_link_093.reg_i2s_mclk_en   = 1;		//I2S MCLK output enable
	st_hdmi_link_093.reg_i2s_mute_flat = 1;		//I2S Flat sample and Channel mute in abnormal status

	//Write HDMI_LINK_093 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_093, &st_hdmi_link_093, sizeof (UINT32));

	//Read HDMI_LINK_094 reg.
	memcpy (&st_hdmi_link_094, (UINT32*) &gVportRegBx->hdmi_link_094, sizeof (UINT32));

	//I2S output enable
	st_hdmi_link_094.reg_i2s_gnd_mode = 1;

	//S/PDIF output enable for debug output only
	//st_hdmi_link_094.reg_sp_en = 1;

	//Write HDMI_LINK_092 reg.
	memcpy ((UINT32*) &gVportRegBx->hdmi_link_094, &st_hdmi_link_094, sizeof (UINT32));


	//Read TOP_011 reg.
	memcpy (&st_top_011, (UINT32*) &gVportRegBx->top_011, sizeof (UINT32));

	// Audio sw reset
	st_top_011.reg_swrst_adto 	= 0;
	st_top_011.reg_swrst_apll 	= 0;
	st_top_011.reg_swrst_linkaw = 0;
	st_top_011.reg_swrst_linkar = 0;

	//Write TOP_011 reg.
	memcpy ((UINT32*) &gVportRegBx->top_011, &st_top_011, sizeof (UINT32));

#endif	//#ifdef USE_HDMI_AUDIO_PLL

	return 0;
}

/**
 * HDMI_GetAudioInfo
 *
 * @parm LX_HDMI_AUDIO_INFO_T *
 * @return int
*/
int HDMI_L8Bx_GetAudioInfo(LX_HDMI_AUDIO_INFO_T *audio_info)
{
	int ret = 0;

	HDMI_LINK_008	st_hdmi_link_008;
	HDMI_LINK_081	st_hdmi_link_081;
	HDMI_LINK_085	st_hdmi_link_085;

#ifdef	ENABLE_HDMI_AUDIO_DEBUG
	HDMI_LINK_095	st_hdmi_link_095;
#endif

	HDMI_LINK_096	st_hdmi_link_096;
	HDMI_LINK_101	st_hdmi_link_101;

	LX_HDMI_AUDIO_TYPE_T		audioType		= LX_HDMI_AUDIO_NO_AUDIO;		///< HDMI Audio Type.
	LX_HDMI_SAMPLING_FREQ_T		samplingFreq	= LX_HDMI_SAMPLING_FREQ_48_KHZ;	///< Sampling Frequency
	BOOLEAN						bPortConnected 	= FALSE;	///< HDMI Port connect status (1 : connected, 0 : not connected)
	BOOLEAN						bInterlaced		= FALSE;	///< Scan type (1 : interlace, 0 : progressive)

#ifdef USE_HDMI_AUDIO_PLL
	HDMI_PHY_RO05	st_hdmi_phy_ro05;
	HDMI_PHY_RO05	st_hdmi_phy_ro06;

	HDMI_LINK_087	st_hdmi_link_087;
	HDMI_LINK_089	st_hdmi_link_089;
	HDMI_LINK_091	st_hdmi_link_091;

	UINT32			ui32TmdsFreq = 0;		//< HDMI measured frequency value of TMDS clock
	UINT32			ui32SamplingFreq = 0;	//< HDMI sampling frequency in source device
	BOOLEAN			bIsTmdsFreqChg = FALSE;	//< Frequency of TMDS clock is changed.

	LX_HDMI_SAMPLING_FREQ_T		tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;	///< Sampling Frequency from TMDS signal
#endif	//#ifdef USE_HDMI_AUDIO_PLL

	//Read HDMI_LINK_008 reg.
	memcpy (&st_hdmi_link_008, (UINT32*) &gVportRegBx->hdmi_link_008, sizeof (UINT32));

	//Check HDMI port connection
	if(st_hdmi_link_008.reg_scdt == 0)	//Check HDMI, DVI Sync Detect
	{
		//Reset global variable.
		_gHDMIAudioPrevNValue	= 0;	//HDMI N Value in source device
		_gHDMIAudioPrevTMDSFreq	= 0;	//HDMI previous measured frequency value of TMDS clock

		//Reset global variable.
		_gHDMIAudioInfo.srcIdx		   = LX_HDMI_MAIN_WINDOW;
		_gHDMIAudioInfo.audioType	   = LX_HDMI_AUDIO_DEFAULT;
		_gHDMIAudioInfo.samplingFreq   = LX_HDMI_SAMPLING_FREQ_NONE;
		_gHDMIAudioInfo.bPortConnected = FALSE;
		_gHDMIAudioInfo.bInterlaced    = FALSE;

		//Mute audio data
		VPORT_RdFL(hdmi_link_093);
		VPORT_Wr01(hdmi_link_093, reg_i2s_sd_en, 0x0);
		VPORT_WrFL(hdmi_link_093);

		AUDIO_ERROR("HDMI_GetAudioInfo : HDMI / DVI Not Connected!!!\n");
		goto func_exit;
	}

	//Read HDMI_LINK_081 reg.
	memcpy (&st_hdmi_link_081, (UINT32*) &gVportRegBx->hdmi_link_081, sizeof (UINT32));

	//Check video active size
	if( (st_hdmi_link_081.reg_v_av <  240) || (st_hdmi_link_081.reg_h_av <  320)	\
	 || (st_hdmi_link_081.reg_v_av > 2970) || (st_hdmi_link_081.reg_h_av > 3840) )
	{
		AUDIO_ERROR("HDMI_GetAudioInfo : Video Active Size Error(v = %d, h = %d)!!!\n",	\
					 st_hdmi_link_081.reg_v_av, st_hdmi_link_081.reg_h_av);

		//Mute audio data
		VPORT_RdFL(hdmi_link_093);
		VPORT_Wr01(hdmi_link_093, reg_i2s_sd_en, 0x0);
		VPORT_WrFL(hdmi_link_093);

		goto func_exit;
	}

	//Set HDMI audio sampling frequency
	if(st_hdmi_link_008.reg_hdmi_mode)	//0 : DVI, 1 : HDMI
	{
#ifdef USE_HDMI_AUDIO_PLL
		//Check if  HDMI audio clock is updated for source device(Monitoring HDMI audio clock)
		//Read HDMI_PHY_RO05 reg.
		memcpy (&st_hdmi_phy_ro05, (UINT32*) &pstHdmiPhyReg->phy0.hdmi_phy_ro05, sizeof (UINT32));

		//Read HDMI_PHY_RO06 reg.
		memcpy (&st_hdmi_phy_ro06, (UINT32*) &pstHdmiPhyReg->phy0.hdmi_phy_ro06, sizeof (UINT32));

		//Read HDMI_LINK_089 reg.
		memcpy (&st_hdmi_link_089, (UINT32*) &gVportRegBx->hdmi_link_089, sizeof (UINT32));

		//Read HDMI_LINK_091 reg.
		memcpy (&st_hdmi_link_091, (UINT32*) &gVportRegBx->hdmi_link_091, sizeof (UINT32));

		//Get measured frequency value of TMDS clock.
		ui32TmdsFreq = ((st_hdmi_phy_ro05.tmds_freq_15_8 << 8) | (st_hdmi_phy_ro06.tmds_freq_15_8)) * 10000;

		//Check if TMDS frquency is changed.(not optimized)
		if( (ui32TmdsFreq < ((UINT32)((float)_gHDMIAudioPrevTMDSFreq * (float)0.9)))	\
		   ||(ui32TmdsFreq > ((UINT32)((float)_gHDMIAudioPrevTMDSFreq * (float)1.1))) )
		{
			bIsTmdsFreqChg = TRUE;

#ifdef	ENABLE_HDMI_AUDIO_DEBUG
			printk("\n");
			HDMI_DEBUG("HDMI Audio TMDS Clock is changed.(ui32TmdsFreq = %d)\n", ui32TmdsFreq);
#endif
		}

		//Recalculate Fs if N or TMDS freq is changed.
		if( (st_hdmi_link_089.reg_acr_n_hw != _gHDMIAudioPrevNValue)	\
		  ||(bIsTmdsFreqChg == TRUE) )
		{
			ui32SamplingFreq = (UINT32)((float)st_hdmi_link_089.reg_acr_n_hw * (((float)ui32TmdsFreq / (float)st_hdmi_link_091.reg_acr_cts_hw) / (float)128.0));

#ifdef	ENABLE_HDMI_AUDIO_DEBUG
			HDMI_DEBUG("HDMI Audio : SamplingFreq = %d(ACR_N = %d, ACR_CTS = %d)\n",	\
						ui32SamplingFreq, st_hdmi_link_089.reg_acr_n_hw, st_hdmi_link_091.reg_acr_cts_hw);
#endif

			//Read HDMI_LINK_087 reg.
			memcpy (&st_hdmi_link_087, (UINT32*) &gVportRegBx->hdmi_link_087, sizeof (UINT32));

			//Set Fs value manually
			if(ui32SamplingFreq == 0)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x1;		//  48 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_48_KHZ\n");
			}
			else if(ui32SamplingFreq < 22983)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0xC;		//  22.05 kHz(not supported)
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_22_05KHZ(not supported)\n");
			}
			else if(ui32SamplingFreq < 30000)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0xC;		//  24 kHz(not supported)
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_24_KHZ(not supported)\n");
			}
			else if(ui32SamplingFreq < 33800)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0xC;		//  32 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_32_KHZ\n");
			}
			else if(ui32SamplingFreq < 45965)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x0;		//  44.1 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_44_1KHZ\n");
			}
			else if(ui32SamplingFreq < 67000)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x1;		//  48 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_48_KHZ\n");
			}
			else if(ui32SamplingFreq < 91935)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x3;		//  88.2 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_88_2KHZ\n");
			}
			else if(ui32SamplingFreq < 135000)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x4;		//  96 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_96_KHZ\n");
			}
			else if(ui32SamplingFreq < 183870)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x5;		//  176.4 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_176_4KHZ\n");
			}
			else if(ui32SamplingFreq < 210000)
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x7;		//  192 kHz
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_192_KHZ\n");
			}
			else
			{
				st_hdmi_link_087.reg_acr_fs_sw = 0x7;		//  768kHz(not supported)
				HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_768_KHZ(not supported)\n");
			}

			//Write HDMI_LINK_087 reg.
			memcpy ((UINT32*) &gVportRegBx->hdmi_link_087, &st_hdmi_link_087, sizeof (UINT32));

			//Update previous value for next sequence
			_gHDMIAudioPrevNValue	= st_hdmi_link_089.reg_acr_n_hw;	//HDMI N Value in source device
			_gHDMIAudioPrevTMDSFreq	= ui32TmdsFreq;						//HDMI previous measured frequency value of TMDS clock

#ifdef	ENABLE_HDMI_AUDIO_DEBUG
			//HDMI Audio Link data is not correct.
			HDMI_DEBUG("HDMI Audio Freq = %d(N = %d, TMDS Freq = %d)\n", ui32SamplingFreq, _gHDMIAudioPrevNValue, _gHDMIAudioPrevTMDSFreq);
#endif
			//The HDMI auido link layer data is not correct before setting Auido PLL value.
			goto func_exit;
		}
#endif	//#ifdef USE_HDMI_AUDIO_PLL

		//Read HDMI_LINK_096 reg.
		memcpy (&st_hdmi_link_096, (UINT32*) &gVportRegBx->hdmi_link_096, sizeof (UINT32));

		//Check if audio sample word used for other purposes than liner PCM.
		if(st_hdmi_link_096.reg_achst_byte0 & 0x2)	//bit 1
		{
			//Read HDMI_LINK_101 reg. : IEC61937 Burst Info Pc, Pd
			memcpy (&st_hdmi_link_101, (UINT32*) &gVportRegBx->hdmi_link_101, sizeof (UINT32));

			//Check a vaild Pc data for payload
			if( (st_hdmi_link_101.reg_burst_pc & 0x0080) == 0 )	//bit 7
			{
				//Set Audio Data-Types according to IEC61937-2 Burst Info Preamble C
				switch(st_hdmi_link_101.reg_burst_pc & 0x001F) 	//bit 4 ~ 0
				{
					case BURST_INFO_AUDIO_TYPE_AC3:
					case BURST_INFO_AUDIO_TYPE_AC3_ENHANCED:
						audioType = LX_HDMI_AUDIO_AC3;
						break;

					case BURST_INFO_AUDIO_TYPE_DTS_I:
					case BURST_INFO_AUDIO_TYPE_DTS_II:
					case BURST_INFO_AUDIO_TYPE_DTS_III:
						audioType = LX_HDMI_AUDIO_DTS;
						break;

					case BURST_INFO_AUDIO_TYPE_MPEG1_L1:
					case BURST_INFO_AUDIO_TYPE_MPEG1_L23:
					case BURST_INFO_AUDIO_TYPE_MPEG2_EXT:
					case BURST_INFO_AUDIO_TYPE_MPEG2_L1:
					case BURST_INFO_AUDIO_TYPE_MPEG2_L2:
					case BURST_INFO_AUDIO_TYPE_MPEG2_L3:
						audioType = LX_HDMI_AUDIO_MPEG;
						break;

					case BURST_INFO_AUDIO_TYPE_MPEG2_AAC:
						audioType = LX_HDMI_AUDIO_AAC;
						break;

					default:
						audioType = LX_HDMI_AUDIO_NO_AUDIO;
						break;
				}
			}
			else
			{
				audioType = LX_HDMI_AUDIO_PCM;
				AUDIO_ERROR("HDMI_GetAudioInfo Bx: Burst Info Error = %d\n", (st_hdmi_link_101.reg_burst_pc & BURST_INFO_PAYLOAD_ERROR_BIT_MASK));
			}
		}
		else
		{
			audioType = LX_HDMI_AUDIO_PCM;
		}

		//Set Sampling frequency from IEC60958 Channel Status Byte 3
		switch(st_hdmi_link_096.reg_achst_byte3 & 0x0F)		//bit 24 ~ 27
		{
			case HDMI_AUDIO_SAMPLE_22_05KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_22_05KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_24_KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_24_KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_32_KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_32_KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_44_1KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_44_1KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_48_KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_88_2KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_88_2KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_96_KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_96_KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_176_4KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_176_4KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_192_KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_192_KHZ;
				break;

			case HDMI_AUDIO_SAMPLE_768_KHZ:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_768_KHZ;
				break;

			default:
				samplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;
				AUDIO_ERROR("HDMI_GetAudioInfo Bx: samplingFreq = %d\n", samplingFreq);
				break;
		}


		//Compute a sampling frequency from TMDS clock
		ui32SamplingFreq = (UINT32)((float)st_hdmi_link_089.reg_acr_n_hw * (((float)ui32TmdsFreq / (float)st_hdmi_link_091.reg_acr_cts_hw) / (float)128.0));

		if(ui32SamplingFreq == 0)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_NONE;
			HDMI_PRINT("Get Fs from TDMS clock => LX_HDMI_SAMPLING_FREQ_48_KHZ\n");
		}
		else if(ui32SamplingFreq < 22983)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_22_05KHZ;	//  22.05 kHz(not supported)
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_NONE;
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_22_05KHZ(not supported)\n");
		}
		else if(ui32SamplingFreq < 30000)
		{
			st_hdmi_link_087.reg_acr_fs_sw = 0xC;
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_24_KHZ;	//  24 kHz(not supported)
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_NONE;
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_24_KHZ(not supported)\n");
		}
		else if(ui32SamplingFreq < 33800)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_32_KHZ;	//  32 kHz
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_32KHZ\n");
		}
		else if(ui32SamplingFreq < 45965)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_44_1KHZ;	//  44.1 kHz
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_44_1KHZ\n");
		}
		else if(ui32SamplingFreq < 67000)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;	//  48 kHz
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_48_KHZ\n");
		}
		else if(ui32SamplingFreq < 91935)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_88_2KHZ;	//  88.2 kHz
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_88_2KHZ\n");
		}
		else if(ui32SamplingFreq < 135000)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_96_KHZ;	//  96 kHz
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_96_KHZ\n");
		}
		else if(ui32SamplingFreq < 183870)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_176_4KHZ;	//  176.4 kHz
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_176_4KHZ\n");
		}
		else if(ui32SamplingFreq < 21000)
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_192_KHZ;	//  192 kHz
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_192_KHZ\n");
		}
		else
		{
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_768_KHZ;	//  768 kHz(not supported)
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_NONE;
			HDMI_PRINT("Set Fs => LX_HDMI_SAMPLING_FREQ_768_KHZ(not supported)\n");
		}

		//Check sampling frequency between TDMS signal and channel status
		if(samplingFreq == tmdsSamplingFreq)
		{
			//Un-mute audio data
			VPORT_RdFL(hdmi_link_093);
			VPORT_Wr01(hdmi_link_093, reg_i2s_sd_en, 0xF);
			VPORT_WrFL(hdmi_link_093);

			AUDIO_ERROR("HDMI_GetAudioInfo Bx: samplingFreq = %d\n", samplingFreq);
		}
		else
		{
			//Mute audio data
			VPORT_RdFL(hdmi_link_093);
			VPORT_Wr01(hdmi_link_093, reg_i2s_sd_en, 0x0);
			VPORT_WrFL(hdmi_link_093);

			AUDIO_ERROR("HDMI_GetAudioInfo Bx: samplingFreq = %d, tmds = %d\n", samplingFreq, tmdsSamplingFreq);
		}

		//set samplingFreq to 48Khz when TMDS clock base is zero.
		if(tmdsSamplingFreq == LX_HDMI_SAMPLING_FREQ_NONE)
			tmdsSamplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;

		//set samplingFreq to TMDS clock base
		samplingFreq = tmdsSamplingFreq;

		//Read HDMI_LINK_085 reg.
		memcpy (&st_hdmi_link_085, (UINT32*) &gVportRegBx->hdmi_link_085, sizeof (UINT32));

		//Set interlaced / progressive scan mode
		bInterlaced = st_hdmi_link_085.reg_intrl;

		//Set HDMI/DVI port connect status
		bPortConnected = TRUE;

#ifdef	ENABLE_HDMI_AUDIO_DEBUG
		if( (audioType		!= _gHDMIAudioInfo.audioType)	   || (samplingFreq != _gHDMIAudioInfo.samplingFreq)	\
		  ||(bPortConnected != _gHDMIAudioInfo.bPortConnected) || (bInterlaced	!= _gHDMIAudioInfo.bInterlaced) )
		{
			AUDIO_ERROR("HDMI_GetAudioInfo Bx: samplingFreq = %d\n", samplingFreq);

			//Read HDMI_LINK_095 reg.
			memcpy (&st_hdmi_link_095, (UINT32*) &gVportRegBx->hdmi_link_095, sizeof (UINT32));

			//Update global value for debug print
			_gHDMIAudioInfo.audioType	   = audioType;
			_gHDMIAudioInfo.samplingFreq   = samplingFreq;
			_gHDMIAudioInfo.bPortConnected = bPortConnected;
			_gHDMIAudioInfo.bInterlaced    = bInterlaced;

			//Check IEC60958 Channel Status Byte0 ~ 3
			printk("\n");
			HDMI_DEBUG("IEC60958 Channel Status Byte0 = 0x%X, Byte1 = 0x%X, Byte2 = 0x%X, Byte3 = 0x%X\n",	\
						st_hdmi_link_096.reg_achst_byte0, st_hdmi_link_096.reg_achst_byte1, st_hdmi_link_096.reg_achst_byte2, st_hdmi_link_096.reg_achst_byte3);

			//Check IEC60958 Channel Status Byte4
			HDMI_DEBUG("^y^IEC60958 Channel Status Byte4 = 0x%X\n", st_hdmi_link_095.reg_achst_byte4);

			if(st_hdmi_link_096.reg_achst_byte0 & 0x1)
				HDMI_DEBUG("Consumer use of channel status block is error!!!\n");
			else
				HDMI_DEBUG("Consumer use of channel status block.\n");


			if(st_hdmi_link_096.reg_achst_byte0 & 0x2)
				HDMI_DEBUG("Audio sample word used for other purposes than liner PCM.\n");
			else
				HDMI_DEBUG("Audio sample word used for liner PCM.\n");


			if(st_hdmi_link_096.reg_achst_byte0 & 0x4)
				HDMI_DEBUG("Software for which no copyright is asserted.\n");
			else
				HDMI_DEBUG("Software for which copyright is asserted.\n");


			HDMI_DEBUG("Category code = 0x%X\n", st_hdmi_link_096.reg_achst_byte1);

			HDMI_DEBUG("Source number = %d\n", st_hdmi_link_096.reg_achst_byte2 & 0x0F);


			HDMI_DEBUG("Channel number = %d\n", st_hdmi_link_096.reg_achst_byte2 & 0xF0);
			if((st_hdmi_link_096.reg_achst_byte2 & 0xF0) == 0x01)
				HDMI_DEBUG("Left channel for stereo channel format.\n");

			if((st_hdmi_link_096.reg_achst_byte2 & 0xF0) == 0x02)
				HDMI_DEBUG("Right channel for stereo channel format.\n");


			if((st_hdmi_link_096.reg_achst_byte3 & 0x0F) == 0x0)
				HDMI_DEBUG("Symbol frequency = 64 X 44.1KHz = 2.8224 MHz.\n");

			if((st_hdmi_link_096.reg_achst_byte3 & 0x0F) == 0x2)
				HDMI_DEBUG("Symbol frequency = 64 X 48KHz = 3.072 MHz.\n");

			if((st_hdmi_link_096.reg_achst_byte3 & 0x0F) == 0x3)
				HDMI_DEBUG("Symbol frequency = 64 X 32KHz = 2.048 MHz.\n");


			if((st_hdmi_link_096.reg_achst_byte3 & 0x30) == 0x0)
				HDMI_DEBUG("Clock accuracy is Level II.\n");

			if((st_hdmi_link_096.reg_achst_byte3 & 0x30) == 0x1)
				HDMI_DEBUG("Clock accuracy is Level I.\n");

			if((st_hdmi_link_096.reg_achst_byte3 & 0x30) == 0x2)
				HDMI_DEBUG("Clock accuracy is Level III.\n");

			if((st_hdmi_link_096.reg_achst_byte3 & 0x30) == 0x3)
				HDMI_DEBUG("Interface frame rate not matched to sampling frequency.\n");


			if(st_hdmi_link_096.reg_achst_byte3 & 0x80)
				HDMI_DEBUG("Maximum audio sample word length is 24 bits.\n");
			else
				HDMI_DEBUG("Maximum audio sample word length is 20 bits.\n");

			//Check IEC61937 Burst Info
			if(st_hdmi_link_096.reg_achst_byte0 & 0x2)
			{
				printk("\n");
				HDMI_DEBUG("IEC61937 Burst Info Pc = 0x%X, Pd = 0x%X\n",	\
							st_hdmi_link_101.reg_burst_pc, st_hdmi_link_101.reg_burst_pd);

				if(st_hdmi_link_101.reg_burst_pc & BURST_INFO_PAYLOAD_ERROR_BIT_MASK)
					HDMI_DEBUG("Error-flag indicationg that the burst-payload may contain errors.\n");
				else
					HDMI_DEBUG("Error-flag indicationg a vaild burst-payload.\n");


				//Set Audio Data-Types according to IEC61937-2 Burst Info Preamble C
				switch(st_hdmi_link_101.reg_burst_pc & 0x001F)	//bit 4 ~ 0
				{
					case BURST_INFO_AUDIO_TYPE_AC3:
					case BURST_INFO_AUDIO_TYPE_AC3_ENHANCED:
						audioType = LX_HDMI_AUDIO_AC3;
						HDMI_DEBUG("LX_HDMI_AUDIO_AC3(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					case BURST_INFO_AUDIO_TYPE_DTS_I:
					case BURST_INFO_AUDIO_TYPE_DTS_II:
					case BURST_INFO_AUDIO_TYPE_DTS_III:
						audioType = LX_HDMI_AUDIO_DTS;
						HDMI_DEBUG("LX_HDMI_AUDIO_DTS(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					case BURST_INFO_AUDIO_TYPE_MPEG1_L1:
					case BURST_INFO_AUDIO_TYPE_MPEG1_L23:
					case BURST_INFO_AUDIO_TYPE_MPEG2_EXT:
					case BURST_INFO_AUDIO_TYPE_MPEG2_L1:
					case BURST_INFO_AUDIO_TYPE_MPEG2_L2:
					case BURST_INFO_AUDIO_TYPE_MPEG2_L3:
						audioType = LX_HDMI_AUDIO_MPEG;
						HDMI_DEBUG("LX_HDMI_AUDIO_MPEG(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					case BURST_INFO_AUDIO_TYPE_MPEG2_AAC:
						audioType = LX_HDMI_AUDIO_AAC;
						HDMI_DEBUG("LX_HDMI_AUDIO_AAC(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					case BURST_INFO_AUDIO_TYPE_ATRAC:
					case BURST_INFO_AUDIO_TYPE_ATRAC_23:
						HDMI_DEBUG("BURST_INFO_AUDIO_TYPE_ATRAC or ATRAC_23(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					case BURST_INFO_AUDIO_TYPE_NULL:
						HDMI_DEBUG("BURST_INFO_AUDIO_TYPE_NULL(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					case BURST_INFO_AUDIO_TYPE_SMPTE_338M:
						HDMI_DEBUG("BURST_INFO_AUDIO_TYPE_SMPTE_338M(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					case BURST_INFO_AUDIO_TYPE_PAUSE:
						HDMI_DEBUG("BURST_INFO_AUDIO_TYPE_PAUSE(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;

					default:
						audioType = LX_HDMI_AUDIO_NO_AUDIO;
						AUDIO_ERROR("LX_HDMI_AUDIO_NO_AUDIO(Audio type in Pc = %d)\n", st_hdmi_link_101.reg_burst_pc & 0x001F);
						break;
				}

				HDMI_DEBUG("Data-type-dependent info. = %d\n", st_hdmi_link_101.reg_burst_pc & 0x1F00);
				HDMI_DEBUG("Bitstream number = %d\n", st_hdmi_link_101.reg_burst_pc & 0xD000);

				HDMI_DEBUG("Length-code : Number of bits = %d\n", st_hdmi_link_101.reg_burst_pd);
				printk("\n");
			}
		}
#endif	//#ifdef	ENABLE_HDMI_AUDIO_DEBUG

	}
	else
	{
		audioType      = LX_HDMI_AUDIO_DVI;
		samplingFreq   = LX_HDMI_SAMPLING_FREQ_48_KHZ;
		bPortConnected = TRUE;
		bInterlaced    = FALSE;
	}

func_exit:
	audio_info->audioType 	   = audioType;
	audio_info->samplingFreq   = samplingFreq;
	audio_info->bPortConnected = bPortConnected;
	audio_info->bInterlaced    = bInterlaced;

	HDMI_PRINT("HDMI_GetAudioInfo Bx: type = 0x%X, freq = 0x%X, bConnected = %d, bInterlaced = %d\n", audio_info->audioType, audio_info->samplingFreq, audio_info->bPortConnected, audio_info->bInterlaced);
	//HDMI_ERROR("HDMI_GetAudioInfo Bx: type = 0x%X, freq = 0x%X, bConnected = %d, bInterlaced = %d\n", audio_info->audioType, audio_info->samplingFreq, audio_info->bPortConnected, audio_info->bInterlaced);

	return ret;
}

/**
 * HDMI_SetMute
 *
 * @parm UINT32 *
 * @return int
*/
int HDMI_L8Bx_SetMute(LX_HDMI_MUTE_CTRL_T *pHdmiMute)
{
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

	VPORT_RdFL(hdmi_link_006);
	//VPORT_Wr01(hdmi_link_006, reg_mute_vid, videoMute);
	//VPORT_Wr01(hdmi_link_006, reg_mute_aud, audioMute);		//This mute control disables inner clock for audio module.
	VPORT_WrFL(hdmi_link_006);

	//Mute audio data
	VPORT_RdFL(hdmi_link_093);
	VPORT_Wr01(hdmi_link_093, reg_i2s_sd_en, audioMute);
	VPORT_WrFL(hdmi_link_093);

	return 0;
}

/**
 * HDMI_SetArc
 *
 * @parm LX_HDMI_ARC_CTRL_T *
 * @return int
*/
int HDMI_L8Bx_SetArc(LX_HDMI_ARC_CTRL_T *arc)
{

	return 0;
}

/**
 * HDMI_SetRGBYCbCr
 *
 * @parm UINT32 *
 * @return int
*/
int HDMI_L8Bx_SetRGBYCbCr(LX_HDMI_AVI_COLORSPACE_T *space)
{
	UINT32 csValue = 0;

	switch(*space)
	{
			case LX_HDMI_AVI_COLORSPACE_YCBCR422:
			case LX_HDMI_AVI_COLORSPACE_YCBCR444:
				csValue = 0x800080;
			break;

			case LX_HDMI_AVI_COLORSPACE_RGB:
			default:
				csValue = 0x000000;
			break;
	}

	VPORT_RdFL(hdmi_link_086);
	VPORT_Wr(hdmi_link_086, csValue);
	VPORT_WrFL(hdmi_link_086);

	return 0;
}

/**
 *  HDMI_PowerConsumption
 *
 *  @param LX_HDMI_POWER_CTRL_T *
 *  @return int
*/
int HDMI_L8Bx_PowerConsumption(LX_HDMI_POWER_CTRL_T *power)
{
	int ret = 0;
	UINT8 	pdbAll	= 0;
	UINT8 	resetAll = 0;

	if(power->enable == 0) // Power down
		pdbAll = resetAll = 0;
	else	// power up
		pdbAll = resetAll = 1;

	if(power->srcIdx == 0)
	{
		HDMIPHY_RdFL(phy0.hdmi_phy_rsv00);
		HDMIPHY_Wr01(phy0.hdmi_phy_rsv00, pdb_all, pdbAll);
		HDMIPHY_Wr01(phy0.hdmi_phy_rsv00, resetb_all, resetAll);
		HDMIPHY_WrFL(phy0.hdmi_phy_rsv00);
	}
	else
	{
		HDMIPHY_RdFL(phy1.hdmi_phy_rsv00);
		HDMIPHY_Wr01(phy1.hdmi_phy_rsv00, pdb_all, pdbAll);
		HDMIPHY_Wr01(phy1.hdmi_phy_rsv00, resetb_all, resetAll);
		HDMIPHY_WrFL(phy1.hdmi_phy_rsv00);
	}

	//HDMI_ERROR(" HDMI_L8Bx_PowerConsumption Port[%d] Power[%s]\n", power->srcIdx, (power->enable ? "On" : "Off"));
	return ret;
}

/**
 * HDMI_GetRegister
 *
 * @parm UINT32 *
 * @return int
*/
int HDMI_L8Bx_GetRegister(UINT32 addr , UINT32 *value)
{
	unsigned long ulAddr;

	if(pstHdmiPhyReg == NULL) return -1;

	ulAddr = (unsigned long)pstHdmiPhyReg;

	memcpy((void *)value , (void *)(ulAddr + addr) , sizeof(unsigned int));

	return 0;
}

/**
 * HDMI_GetAudioInfo
 *
 * @parm LX_HDMI_AUDIO_INFO_T *
 * @return int
*/
int HDMI_L8Bx_SetRegister(UINT32 addr , UINT32 value)
{
    unsigned long ulAddr;

    if(pstHdmiPhyReg == NULL) return -1;

    ulAddr = (unsigned long)pstHdmiPhyReg;

	memcpy((void *)(ulAddr + addr) , (void *)&value , sizeof(unsigned int));

	return 0;
}

/**
 * HDMI_irq_handler irq handler
 *
 * @param	irq , device id , regs
 * @return	0 : OK , -1 : NOK
*/
irqreturn_t HDMI_L8Bx_IRQHandler(int irq, void *dev)
{
	LX_HDMI_MUTE_CTRL_T		hdmiMute = {LX_HDMI_MAIN_WINDOW, 1, 1, 2};  // Default - A/V Mute

	if(gVportRegBx->intr_vp3_intr.hdmi_clr_mute)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_CLR_MUTE]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<31);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_set_mute)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_SET_MUTE]\n");

		HDMI_SetMute(&hdmiMute);
		_gSourceMute = 3;

		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<30);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_hdmi_mode)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_HDMI_MODE]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<29);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_vs_det)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_VS_DET]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<28);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_no_scdt)
	{
		//Mute audio data
		VPORT_RdFL(hdmi_link_093);
		VPORT_Wr01(hdmi_link_093, reg_i2s_sd_en, 0x0);
		VPORT_WrFL(hdmi_link_093);

		HDMI_PRINT("intr_vp3_intr[HDMI_NO_SCDT]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<27);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_scdt)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_SCDT]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<26);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_auth_done)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_AUTH_DONE]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<25);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_auth_init)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_AUTH_INIT]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<24);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_il_chg)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_IL_CHG]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<23);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_hr_chg)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_HR_CHG]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<22);
	}

	if(gVportRegBx->intr_vp3_intr.burst_info)
	{
		HDMI_PRINT("intr_vp3_intr[BURST_INFO]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<21);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_afifo_ovrr)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_AFIFO_OVRR]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<20);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_afifo_undr)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_AFIFO_UNDR]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<19);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_spdif_err)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_SPDIF_ERR]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<18);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_fs_chg)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_FS_CHG]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<17);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_hwn_chg)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_HWN_CHG]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<16);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_hwcts_chg)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_HWCTS_CHG]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<15);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_unr)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_UNR]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<14);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_isrc2)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_ISRC2]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<13);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_isrc1)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_ISRC1]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<12);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_vsi)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_VSI]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<11);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_spd)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_SPD]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<10);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_aud)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_AUD]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<9);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_mpg)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_MPG]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<8);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_acp)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_ACP]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<7);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_gbd)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_GBD]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<6);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_acr)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_ACR]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<5);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_asp)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_ASP]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<4);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_gcp)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_GCP]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<3);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_new_avi)
	{
		UINT8 hdmiMode = 0;
		UINT32 aviData = 0, csc = 0;

		HDMI_PRINT("intr_vp3_intr[HDMI_NEW_AVI]\n");

		VPORT_RdFL(hdmi_link_008);
		VPORT_Rd01(hdmi_link_008, reg_hdmi_mode, hdmiMode);	// HDMI mode

		if (_gSCDT && hdmiMode && (_gHDMI_DVI_Done > (gBootData->timingCn[gBootData->hdmi_switch] - 1)))
		{
			VPORT_RdFL(hdmi_link_035);
			VPORT_Rd01(hdmi_link_035, reg_pkt_avi_dat__7, aviData);
			csc = (aviData & 0x6000) >> 13;

			if(_gPrevPixelEncoding != csc)
			{
				HDMI_ERROR(" interupt _gPrevPixelEncoding = [%d] -> [%d]\n", _gPrevPixelEncoding, csc);
				HDMI_SetMute(&hdmiMute);
				_gHDMI_CSC_Mute = 1;
				_gHDMI_port_changed  = 2;
				_gPrevPixelEncoding = csc;
			}
		}

		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<2);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_no_gcp)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NO_GCP]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<1);
	}

	if(gVportRegBx->intr_vp3_intr.hdmi_no_avi)
	{
		HDMI_PRINT("intr_vp3_intr[HDMI_NO_AVI]\n");
		gVportRegBx->intr_vp3_clear.intr_vp3_clear |= (1<<0);
	}

	//HDMI_PRINT("intr_vp3_intr\n");
	//gVportRegBx->intr_vp3_clear.intr_vp3_clear = 0xffffffff;

	return IRQ_HANDLED;
}

/**
 * HDMI irq handler Enable
 *
*/
void HDMI_L8Bx_EnableInterrupt(void)
{
	UINT32 intrEnable = 0x40000004;

	VPORT_RdFL(intr_vp3_enable);
	VPORT_Wr01(intr_vp3_enable, intr_vp3_enable, intrEnable);
	VPORT_WrFL(intr_vp3_enable);

	/* implement interrupt for L8 */
	if (request_irq(L8_IRQ_VP_HDMI,(irq_handler_t)HDMI_IRQHandler,0,"adc", NULL))
	{
		HDMI_ERROR("request_irq  is failed\n");
	}
}

#ifdef	KDRV_CONFIG_PM
int HDMI_L8Bx_RunSuspend(void)
{
	// confirm initialization done
	if(gVportRegBx == NULL)
		return -1;

	memcpy((void *)&guiPmHpdStatus , (void *)&gVportRegBx->hdmi_link_099 , sizeof(unsigned int));

	VPORT_RdFL(intr_vp3_enable);
	VPORT_Rd01(intr_vp3_enable, intr_vp3_enable, guiPmInterruptStatus);

	HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);	// all port off

	return 0;
}

int HDMI_L8Bx_RunResume(void)
{

	HDMI_RunReset();

	HDMI_SetAudio();

	memcpy((void *)&gVportRegBx->hdmi_link_099 , (void *)&guiPmHpdStatus , sizeof(unsigned int));

	if(gstPmPortSel.u8HDMIPort != 0xff)
		HDMI_SetPort(&gstPmPortSel);

	VPORT_RdFL(intr_vp3_enable);
	VPORT_Wr01(intr_vp3_enable, intr_vp3_enable, guiPmInterruptStatus);
	VPORT_WrFL(intr_vp3_enable);

	return 0;
}
#endif
