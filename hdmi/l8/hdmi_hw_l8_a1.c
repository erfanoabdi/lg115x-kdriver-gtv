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
#include "../afe/vport_reg.h"


/******************************************************************************
 *				DEFINES
 *****************************************************************************/
#define HDMI_L8A1_ADAPTIVE_LOOP	// B0 fix 예정
#define HDMI_L8A1_PACKAGE_SLOW	// B0 fix 예정
#define HDMI_L8B0				// B0 fix 예정

//#define HDMI_DEBUG_ON
//#define HDMI_DEBUG_PRINT_ON

/**
 *	Global variables of the driver
 */
extern volatile VPORT_RegAx_T *gVportRegAx;
static volatile HDMI_PHY3P_Reg_T *pstHdmiPhyReg;
extern HDMI_DATA_T *gBootData;

static UINT8 _gSCDT = 0;
static UINT32 _gcount = 0;

static LX_HDMI_AVI_COLORSPACE_T	_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
static LX_HDMI_TIMING_INFO_T _gPrevTiming = {0,};
static UINT8 _gHDMI_port_changed = 0;
static UINT8 _gHDMI_Change = 0;
static UINT8 _gHDMI_DVI_Done = 0;

//static UINT8 _gHDMI_Mute = 0;
//static UINT8 _gTimingStatus = 0;

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
 * HDMI_GetCSCStatus
 * CSS Status Information
 * @parm void
 * @return int - 1 : changed , 0 : don't change
*/
int HDMI_L8Ax_GetCSCStatus(void)
{
	int ret = 0;
	LX_HDMI_AVI_COLORSPACE_T csc = 0;	// RGB Default

	if(!HDMI_GetPortConnection()) { csc = LX_HDMI_AVI_COLORSPACE_RGB; goto func_exit; }

	csc = ((gVportRegAx->reg_pkt_avi_dat__7.reg_pkt_avi_dat__7) & 0x6000) >> 13;

	if(_gPrevPixelEncoding != csc)
	{
		HDMI_PRINT(" _gPrevPixelEncoding = [%d] -> [%d]\n", _gPrevPixelEncoding, csc);
		ret = 1;
	}

func_exit:
	_gPrevPixelEncoding = csc;

	return ret;
}

/**
 * HDMI_SetPixelRepetition
 * Pixel Repetition for 13.5MHz formats
 * @parm void
 * @return void
*/
void HDMI_L8Ax_SetPixelRepetition(void)
{
	UINT8 port  = 0;
	UINT8 repet = 0;
	UINT8 reg_div2 = 0;
	UINT8 reg_div4 = 0;

	port = (gVportRegAx->reg_scdt.reg_prt_sel < 2) ? 0 : 1;
	repet = gBootData->pixelRepet = gVportRegAx->reg_pr_man.reg_pr;
	reg_div2 = (port == 0) ? pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div2 : pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div2;
	reg_div4 = (port == 0) ? pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div4 : pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div4;
	HDMI_PRINT(" HDMI Pixel Repetition. Port(PHy)[%d] Repetition[%d] \n",port, repet);

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
		pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div2 = reg_div2;
		pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div4 = reg_div4;
	}
	else
	{
		pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div2 = reg_div2;
		pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div4 = reg_div4;
	}
}

/**
 * HDMI_SetAutoSyncPolarity
 *
 * @parm void
 * @return void
*/
void HDMI_L8Ax_SetAutoSyncPolarity(void)
{
	gVportRegAx->reg_pr_man.reg_inv_hs = gVportRegAx->reg_h_pol.reg_h_pol;	// horizontal sync inversion
	gVportRegAx->reg_pr_man.reg_inv_vs = gVportRegAx->reg_h_pol.reg_v_pol;	// vertical sync inversion
}

/**
 * HDMI_GetPortConnection
 *
 * @parm void
 * @return int - 1 : connected , 0 : disconnected
*/
int HDMI_L8Ax_GetPortConnection(void)
{
	int ret = 0;
#ifdef HDMI_DEBUG_ON   // HDMI interupt
	UINT32 int_plug = 0;
#endif

#if 0

	_gSCDT = ret = gVportRegAx->reg_scdt.reg_scdt;

	if( _gSCDT == 1)
	{
		_gcount = 0;
	}
	else 	//if(gVportRegAx->reg_scdt.reg_scdt == 0)
	{
		if(_gcount == 0)
		{
			if(gBootData->pixelRepet > 0)			//L8 B0 bug 2010.12.07
			{
				gVportRegAx->reg_swrst_linksys.reg_swrst_linktmds = 1;
				gVportRegAx->reg_swrst_linksys.reg_swrst_linktmds = 0;
				pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div2 = pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div2 = 0;
				//pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div4 = pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div4 = 0;
				gBootData->pixelRepet = 0;
			}
			_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
		}

		_gcount = 1;
	}
#else
	if(gVportRegAx->reg_scdt.reg_prt_sel < 2)		//! check port 0 , 1
	{
		if((gVportRegAx->reg_scdt.reg_scdt == 1)  && (pstHdmiPhyReg->phy0.hdmi_phy_ro03.cr_done == 1))
		{
			ret = 1;
		}
		else
		{
			gVportRegAx->reg_swrst_linksys.reg_swrst_linkpix = 1;    // A1 board 에서만 적용, should be removed in Rev. B0.
			ret = 0;
		}
	}
	else if(gVportRegAx->reg_scdt.reg_prt_sel < 4)	//! check port 2 , 3
	{
		if((gVportRegAx->reg_scdt.reg_scdt == 1) && (pstHdmiPhyReg->phy1.hdmi_phy_ro03.cr_done == 1))
		{
			ret = 1;
		}
		else
		{
			gVportRegAx->reg_swrst_linksys.reg_swrst_linkpix = 1;	// A1 board 에서만 적용, should be removed in Rev. B0.
			ret = 0;
		}
	}
	else						//! port selection error
	{
		gVportRegAx->reg_swrst_linksys.reg_swrst_linkpix = 1;
		ret = 0;
	}

#ifdef HDMI_L8B0
	if(ret == 0)
	{
		/** sh.myoung
		White Noise from HDMI format to DVI format at the Master 6100
		A1 board 에서만 적용, should be removed in Rev. B0.
		**/
		gVportRegAx->reg_swrst_linksys.reg_swrst_linkpix = 0;
		if(_gcount == 0)
		{

			if(gBootData->pixelRepet > 0)
			{
				gVportRegAx->reg_swrst_linksys.reg_swrst_linktmds = 1;
				gVportRegAx->reg_swrst_linksys.reg_swrst_linktmds = 0;
				pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div2 = 0;
				//pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div4 = 0;
				pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div2 = 0;
				//pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div4 = 0;
				gBootData->pixelRepet = 0;
			}

			gVportRegAx->reg_swrst_linksys.reg_swrst_linkhdcp = 1;
			gVportRegAx->reg_swrst_linksys.reg_swrst_linkhdcp = 0;
			_gPrevPixelEncoding = LX_HDMI_AVI_COLORSPACE_RGB;
		}
		_gSCDT = 0;
		_gcount = 1;
	}
	else
	{
		_gSCDT = 1;
		_gcount = 0;
	}
#endif
#endif  // #if 1

#ifdef HDMI_DEBUG_ON   // HDMI interupt
	memcpy((void *)&int_plug , (void *)&gVportRegAx->int_plug , sizeof(unsigned int)*1);

	if(_gPrevInt_plug != int_plug)
	{
		HDMI_ERROR(" _gPrevInt_plug = [0x%x] -> [0x%x] cr_done = [%d] \n", _gPrevInt_plug, int_plug, pstHdmiPhyReg->phy0.hdmi_phy_ro03.cr_done);
		_gPrevInt_plug = int_plug;
	}
#endif

	return ret;
}

/**
 * HDMI_GetMode
 *
 * @parm LX_HDMI_MODE_T
 * @return int
*/
int HDMI_L8Ax_GetMode(LX_HDMI_MODE_T *mode)
{
	int 	ret = 0;
	BOOLEAN bHDMI = 0;   					///< 0 : DVI mode, 1 : HDMI Mode
	UINT8	u8port = 0;						///< HDMI port

	u8port = gVportRegAx->reg_scdt.reg_prt_sel;
	if(!HDMI_GetPortConnection()) { goto func_exit; }

	// HDMI/DVI mode status
	bHDMI = gVportRegAx->reg_scdt.reg_hdmi_mode;

func_exit:
	HDMI_PRINT(" HDMI_GetMode = DDIPort[%d] CurPort[%d] mode[%d] \n", mode->u8port, u8port, bHDMI);
	mode->srcIdx 	= LX_HDMI_MAIN_WINDOW;
	mode->bHDMI 	= gBootData->mode = bHDMI;
	mode->u8port 	= u8port;

	return ret;
}

/**
 * HDMI_GetAspectRatio
 *
 * @parm LX_HDMI_ASPECTRATIO_T
 * @return int
*/
int HDMI_L8Ax_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio)
{
	int ret = 0;
	LX_HDMI_AVI_PICTURE_ARC_T AspectRatio = 0; // 0 : ARC_NO_DATA

	if(!HDMI_GetPortConnection()) { memset((void *)ratio , 0 , sizeof(LX_HDMI_ASPECTRATIO_T)); goto func_exit; }

	// aspect ratio for AVI
	AspectRatio = ((gVportRegAx->reg_pkt_avi_dat__7.reg_pkt_avi_dat__7)&0x300000 )>>20;

func_exit:
	ratio->srcIdx = LX_HDMI_MAIN_WINDOW;
	ratio->eAspectRatio = AspectRatio;

	return ret;
}

/**
 * HDMI_GetTimingInfo
 *
 * @parm HDMI_IOR_GET_TIMING_INFO
 * @return int
*/
int HDMI_L8Ax_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info)
{
	int ret = 0;
	BOOLEAN mode = 0;
	BOOLEAN hdcpAuth = 0;
//	UINT32	tmdsClock = 0;
	LX_HDMI_TIMING_INFO_T bufTiming = {0,};

#ifdef HDMI_DEBUG_ON
	LX_HDMI_AVI_COLORSPACE_T csc = 0;	// RGB Default
	UINT32 AVI = 0;
	UINT32 RGB = 0;
#endif

	memset((void *)info , 0 , sizeof(LX_HDMI_TIMING_INFO_T));

	if(!HDMI_GetPortConnection())
	{
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
		_gHDMI_DVI_Done = _gHDMI_Change = 0;
		goto func_exit;
	}

	if(_gHDMI_port_changed > 0)
	{
		_gHDMI_port_changed --;
		goto func_exit;
	}

	if(_gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch])
	{
		HDMI_SetPixelRepetition();			// for specific 13.5MHz input(480i, 576i)
		HDMI_SetAutoSyncPolarity();			// sync inversion
	}

#if 1	// L8A1 과 BD 와의 호환성 오류시 non standard 처리함.
	if(	(gVportRegAx->reg_h_av.reg_v_av < 240)  || (gVportRegAx->reg_h_av.reg_h_av < 320) ||
		(gVportRegAx->reg_h_av.reg_v_av > 2970) || (gVportRegAx->reg_h_av.reg_h_av > 3840) )
	{
		//memset((void *)info , 0 , sizeof(LX_HDMI_TIMING_INFO_T));
		//memset(&bufTiming , 0 , sizeof(LX_HDMI_TIMING_INFO_T));
		goto func_exit;
	}
#endif

	bufTiming.hFreq		= 0; 				///< Horizontal frequency(100 Hz unit) = Dot Freq / hTotal
	bufTiming.vFreq		= 0; 				///< Veritical frequency(1/10 Hz unit) = hFreq / vTotal
	bufTiming.hTotal	= gVportRegAx->reg_h_tot.reg_h_tot; 		///< Horizontal total pixels
	bufTiming.vTotal 	= gVportRegAx->reg_h_tot.reg_v_tot; 		///< Vertical total lines
	bufTiming.hStart 	= gVportRegAx->reg_h_bp.reg_h_bp; 			///< Horizontal start pixel (Back Porch)
	bufTiming.vStart 	= gVportRegAx->reg_h_bp.reg_v_bp;			///< Vertical start lines (Back Porch)
	//bufTiming.hStart 	= gVportRegAx->reg_h_fp.reg_h_fp; 			///< Horizontal start pixel (Front Porch)
	//bufTiming.vStart 	= gVportRegAx->reg_h_fp.reg_v_fp;			///< Vertical start lines (Front Porch)
	bufTiming.hActive 	= gVportRegAx->reg_h_av.reg_h_av;			///< Horizontal active pixel
	bufTiming.vActive 	= gVportRegAx->reg_h_av.reg_v_av; 			///< Vertical active lines
	bufTiming.scanType 	= gVportRegAx->reg_h_pol.reg_intrl; 		///< Scan type (0 : interlace, 1 : progressive)
	bufTiming.scanType 	^= 1;

	mode 		= gVportRegAx->reg_scdt.reg_hdmi_mode;
	hdcpAuth 	= gVportRegAx->int_plug.int_auth_done;
	//tmdsClock	= ;

	if(_gHDMI_DVI_Done < gBootData->timingCn[gBootData->hdmi_switch])
	{
		if(mode == 1 && _gHDMI_DVI_Done < gBootData->hdmiCn[gBootData->hdmi_switch]) // HDMI Mode
		{
			//HDMI_ERROR("------_gHDMI_DVI_Done ===== %d[%d]---------\n",_gHDMI_DVI_Done,_gHDMI_Change);
			if(hdcpAuth == 0)// || _gHDMI_DVI_Done == 20)
			{
				bufTiming.hActive = bufTiming.hActive - 5*_gHDMI_DVI_Done;			///< Horizontal active pixel
				bufTiming.vActive = bufTiming.vActive + 5*_gHDMI_DVI_Done; 			///< Vertical active lines
				_gHDMI_Change = _gHDMI_DVI_Done;
			}
			else
			{
				if(_gHDMI_DVI_Done < _gHDMI_Change+3)
				{
					//HDMI_ERROR("------_gHDMI_DVI_Done ===== %d[%d]---------\n",_gHDMI_DVI_Done,_gHDMI_Change);
					bufTiming.hActive = bufTiming.hActive - 10*_gHDMI_DVI_Done;			///< Horizontal active pixel
					bufTiming.vActive = bufTiming.vActive + 10*_gHDMI_DVI_Done; 		///< Vertical active lines
				}
			}
		}
		else //if(mode == 0) // DVI Mode
		{
			if(_gHDMI_DVI_Done < gBootData->dviCn[gBootData->hdmi_switch])
			{
				//HDMI_ERROR("------_gHDMI_DVI_Done ===== %d[%d]---------\n",_gHDMI_DVI_Done,_gHDMI_Change);
				bufTiming.hActive = bufTiming.hActive - 5*_gHDMI_DVI_Done;			///< Horizontal active pixel
				bufTiming.vActive = bufTiming.vActive + 5*_gHDMI_DVI_Done; 			///< Vertical active lines
			}
		}

		_gHDMI_DVI_Done++;
	}

	/* Format 1080i 에서 vstart value 무조건 even 으로 ~*/
	if( bufTiming.scanType == 0)
		bufTiming.vStart = ((bufTiming.vStart >> 1) << 1);


#if 0
	if(_gHDMI_port_changed > 0)
	{
		info->hActive = gVportRegAx->reg_h_av.reg_h_av - 10*_gHDMI_DVI_Done;			///< Horizontal active pixel
		info->vActive = gVportRegAx->reg_h_av.reg_v_av - 10*_gHDMI_DVI_Done; 			///< Vertical active lines

		_gHDMI_port_changed --;

		//HDMI_ERROR(" 1111111_gHDMI_port_changed End DVI/HDMI = %d   CSC= %d \n",gVportRegAx->reg_scdt.reg_hdmi_mode,((gVportRegAx->reg_pkt_avi_dat__7.reg_pkt_avi_dat__7) & 0x6000) >> 13);
		return ret;
	}
	else
	{
		//if(gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_smute == 0 && gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_cmute == 1)
		if(gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_cmute == 1 || (gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_smute == 0 && gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_cmute == 0))
		{
			if(	_gHDMI_Mute == 1)
			{
				_gHDMI_Mute = 0;
			}

			if(gVportRegAx->reg_scdt.reg_hdmi_mode == 0) // DVI Mode
			{
				if(_gHDMI_DVI_Done < 11 && _gHDMI_DVI_Done > 4)
				{
					gVportRegAx->reg_mute_pol.reg_mute_aud = 0;
					gVportRegAx->reg_mute_pol.reg_mute_vid = 0;
					HDMI_ERROR("------DVI Mute Clear---------\n");
					//_gHDMI_port_changed --;
				}
			}
			else 		// HDMI Mode
			{
				if(gVportRegAx->int_plug.int_auth_done == 1)
				{
					if(_gHDMI_DVI_Done < 14 && _gHDMI_DVI_Done > 4)
					{
						gVportRegAx->reg_mute_pol.reg_mute_aud = 0;
						gVportRegAx->reg_mute_pol.reg_mute_vid = 0;
						HDMI_ERROR("------HDMI HDCP Mute Clear---------\n");
					}
				}
				else
				{
					if(_gHDMI_DVI_Done < 14)
					{
						if(_gHDMI_DVI_Done == 13)
						{
							gVportRegAx->reg_mute_pol.reg_mute_aud = 0;
							gVportRegAx->reg_mute_pol.reg_mute_vid = 0;
							//_gHDMI_DVI_Done = 11;
							HDMI_ERROR("------HDMI --- DVI Mute Clear---------\n");
						}
						if(_gHDMI_DVI_Done <13)
						{
							gVportRegAx->reg_mute_pol.reg_mute_aud = 1;
							gVportRegAx->reg_mute_pol.reg_mute_vid = 1;
						}
					}
				}
			}

			if(_gHDMI_DVI_Done < 15)
			{
				//HDMI_ERROR("_gHDMI_DVI_Done == %d\n",_gHDMI_DVI_Done);
				_gHDMI_DVI_Done++;
			}
		}
		else if(gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_smute == 1 && gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_cmute == 1)
		{
			gVportRegAx->reg_mute_pol.reg_mute_aud = 1;
			gVportRegAx->reg_mute_pol.reg_mute_vid = 1;
			memset((void *)info , 0 , sizeof(LX_HDMI_TIMING_INFO_T));
			_gHDMI_Mute = 1;
			_gHDMI_DVI_Done = 0;
			_gHDMI_port_changed = 1;
			return ret;
		}
		else // if(gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_smute == 1 && gVportRegAx->reg_pkt_gcp_smute.reg_pkt_gcp_cmute == 0)
		{
			gVportRegAx->reg_mute_pol.reg_mute_aud = 1;
			gVportRegAx->reg_mute_pol.reg_mute_vid = 1;

			_gHDMI_Mute = 1;
			_gHDMI_DVI_Done = 0;
		}
	}
#endif


#ifdef HDMI_DEBUG_ON
	#if 0		// AVI 변경
		memcpy((void *)&AVI , (void *)&gVportRegAx->reg_pkt_avi_dat__7 , sizeof(unsigned int)*1);
		if(_gPrevAVI != AVI)
			HDMI_ERROR(" _gPrevAVI = [0x%x] -> [0x%x]\n", _gPrevAVI, AVI);
		_gPrevAVI = AVI;
	#endif

	#if  0		// CSC 변경
		csc = ((gVportRegAx->reg_pkt_avi_dat__7.reg_pkt_avi_dat__7) & 0x6000) >> 13;
		if(_gPrevPixelEncoding != csc)
			HDMI_ERROR(" _gPrevPixelEncoding = [%d] -> [%d]\n", _gPrevPixelEncoding, csc);
		_gPrevPixelEncoding = csc;
	#endif

	#if 0		// RGB
		memcpy((void *)&RGB , (void *)&gVportRegAx->reg_vid_blank_r , sizeof(unsigned int)*1);
		if(_gPrevRGB != RGB)
			HDMI_ERROR(" _gPrevRGB = [0x%x] -> [0x%x]\n", _gPrevRGB, RGB );
		_gPrevRGB = RGB;
	#endif
#endif

func_exit:
	info->srcIdx = LX_HDMI_MAIN_WINDOW;								///< window index
	info->hFreq 	= _gPrevTiming.hFreq 	= bufTiming.hFreq; 		///< Horizontal frequency(100 Hz unit) = Dot Freq / hTotal
	info->vFreq 	= _gPrevTiming.vFreq 	= bufTiming.vFreq; 		///< Veritical frequency(1/10 Hz unit) = hFreq / vTotal
	info->hTotal	= _gPrevTiming.hTotal 	= bufTiming.hTotal; 	///< Horizontal total pixels
	info->vTotal 	= _gPrevTiming.vTotal	= bufTiming.vTotal; 	///< Vertical total lines
	info->hStart	= _gPrevTiming.hStart	= bufTiming.hStart; 	///< Horizontal start pixel (Back Porch)
	info->vStart 	= _gPrevTiming.vStart	= bufTiming.hStart;		///< Vertical start lines (Back Porch)
	info->hActive 	= _gPrevTiming.hActive	= bufTiming.hActive;	///< Horizontal active pixel
	info->vActive 	= _gPrevTiming.vActive	= bufTiming.vActive; 	///< Vertical active lines
	info->scanType 	= _gPrevTiming.scanType	= bufTiming.scanType; 	///< Scan type (0 : interlace, 1 : progressive) 	info->scanType ^= 1;

	return ret;
}

/**
 * HDMI_GetStatus
 *
 * @parm LX_HDMI_STATUS_T
 * @return int
*/
int HDMI_L8Ax_GetStatus(LX_HDMI_STATUS_T *status)
{
	int ret = 0;

	status->srcIdx = LX_HDMI_MAIN_WINDOW;
	status->bHdmiMode = gVportRegAx->reg_scdt.reg_hdmi_mode;	// HDMI/DVI mode
	status->u8hdmiport = gVportRegAx->reg_scdt.reg_prt_sel;	// port number
	status->eColorDepth = gVportRegAx->reg_h_pol.reg_cdepth;	// port number
	status->pixelRepet = 0;
	status->csc = 0;

	switch(status->u8hdmiport)	// HPD enable status
	{
		case 0:
			status->eHotPlug = gVportRegAx->phy1p_hpd3_i.phy3p_hpd0_i;
		break;
		case 1:
			status->eHotPlug = gVportRegAx->phy1p_hpd3_i.phy3p_hpd1_i;
		break;
		case 2:
			status->eHotPlug = gVportRegAx->phy1p_hpd3_i.phy3p_hpd2_i;
		break;
		case 3:
			status->eHotPlug = gVportRegAx->phy1p_hpd3_i.phy3p_hpd3_i;
		break;
		default:
			HDMI_PRINT("WARNING : port number is out of range\n");
		break;
	}

	return ret;
}

/**
 * HDMI_GetVsiPacket
 *
 * @parm LX_HDMI_VSI_PACKET_T
 * @return int
*/
int HDMI_L8Ax_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet)
{
	int ret = 0;
	UINT32 buf[6];

	// VSI packet data structure
	struct{

		UINT32	Meta_data17	: 8;	// data7
		UINT32	Meta_data18	: 8;
		UINT32	Meta_data19	: 8;
		UINT32	Meta_data20	: 8;

		UINT32	Meta_data13	: 8;	// data6
		UINT32	Meta_data14	: 8;
		UINT32	Meta_data15	: 8;
		UINT32	Meta_data16	: 8;

		UINT32	Meta_data9	: 8;	// data5
		UINT32	Meta_data10	: 8;
		UINT32	Meta_data11	: 8;
		UINT32	Meta_data12	: 8;

		UINT32	Meta_data5	: 8;	// data4
		UINT32	Meta_data6	: 8;
		UINT32	Meta_data7	: 8;
		UINT32	Meta_data8	: 8;

		UINT32	Meta_data1	: 8;	// data5
		UINT32	Meta_data2	: 8;
		UINT32	Meta_data3	: 8;
		UINT32	Meta_data4	: 8;

		UINT32	resv2		: 3;	// data6
		UINT32	Meta_present	: 1;
		UINT32	Structure	: 4;
		UINT32	resv3		: 4;
		UINT32	Ext_Data	: 4;
		UINT32	Meta_length	: 5;
		UINT32	Meta_Type	: 3;
		UINT32	Meta_data0	: 8;

		UINT32	IEEERegister	: 24;	// data7
		UINT32	resv1		: 5;
		UINT32	HDMI_Video_Format	: 3;

		UINT32	PacketType	: 8;	// data8 header
		UINT32	Version		: 8;
		UINT32	Length		: 5;
		UINT32	resv0		: 3;
		UINT32	CheckSum	: 8;
	}VSIpacket;


#if 1
	UINT32 *addr;

	addr = (UINT32 *)&gVportRegAx->reg_pkt_unr_dat;
	memcpy((void *)&VSIpacket , (void *)addr , sizeof(unsigned int)*8);
#else
	memcpy((void *)&VSIpacket , (void *)&gVportRegAx->reg_pkt_unr_dat , sizeof(unsigned int)*8);
#endif



	if (!HDMI_GetPortConnection()) { memset((void *)packet , 0 , sizeof(LX_HDMI_VSI_PACKET_T)); goto func_exit; }

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

	// payload
	packet->eVsiPacket.PayLoad[0] = VSIpacket.resv1 | (VSIpacket.HDMI_Video_Format << 5);

	buf[0] = gVportRegAx->reg_pkt_unr_dat__6.reg_pkt_unr_dat__6;
	buf[1] = gVportRegAx->reg_pkt_unr_dat__5.reg_pkt_unr_dat__5;
	buf[2] = gVportRegAx->reg_pkt_unr_dat__4.reg_pkt_unr_dat__4;
	buf[3] = gVportRegAx->reg_pkt_unr_dat__3.reg_pkt_unr_dat__3;
	buf[4] = gVportRegAx->reg_pkt_unr_dat__2.reg_pkt_unr_dat__2;
	buf[5] = gVportRegAx->reg_pkt_unr_dat.reg_pkt_unr_dat;

	memcpy((void *)(&packet->eVsiPacket.PayLoad[1]) , (void *)(&buf) , 24);

	packet->eVsiPacket.packet.dataBytes[0] = VSIpacket.CheckSum;
	packet->eVsiPacket.packet.dataBytes[1] = packet->eVsiPacket.IEERegId[0];
	packet->eVsiPacket.packet.dataBytes[2] = packet->eVsiPacket.IEERegId[1];
	packet->eVsiPacket.packet.dataBytes[3] = packet->eVsiPacket.IEERegId[2];

	memcpy((void *)(&packet->eVsiPacket.packet.dataBytes[4]) , (void *)(&packet->eVsiPacket.PayLoad[0]) , 24);

	packet->eVsiPacket.eVideoFormat	= VSIpacket.HDMI_Video_Format;
	packet->eVsiPacket.e3DStructure	= VSIpacket.Structure;
	packet->eVsiPacket.e3DExtData	= VSIpacket.Ext_Data;
	packet->eVsiPacket.eVSiVIC		= packet->eVsiPacket.packet.dataBytes[5];

func_exit:
	packet->srcIdx = LX_HDMI_MAIN_WINDOW;

	return ret;
}

/**
 * HDMI_GetAviPacket
 *
 * @parm LX_HDMI_AVI_PACKET_T
 * @return int
*/
int HDMI_L8Ax_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet)
{
	int ret = 0;
#if 0
	// AVI packet data structure
	struct{
		UINT32	type		: 8;
		UINT32	version		: 8;
		UINT32	length		: 8;
		UINT32	resv4		: 8;


		UINT32	CheckSum	: 8;
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

		UINT32	VIC06		: 7;
		UINT32	resv1		: 1;
		UINT32	PR03		: 4;
		UINT32	resv2		: 4;
		UINT32	LNET		: 16;

		UINT32	LNSB		: 16;
		UINT32	PNEL		: 16;

		UINT32	PNSR		: 16;
		UINT32	resv3		: 16;

	} AVIpacket;

	memcpy((void *)&AVIpacket , (void *)&gVportRegAx->reg_pkt_avi_hdr , sizeof(unsigned int)*5);

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
	packet->eAviPacket.PixelRepeat = AVIpacket.PR03;

	packet->eAviPacket.TopBarEndLineNumber = AVIpacket.LNET;
	packet->eAviPacket.BottomBarStartLineNumber = AVIpacket.LNSB;
	packet->eAviPacket.LeftBarEndPixelNumber = AVIpacket.PNEL;
	packet->eAviPacket.RightBarEndPixelNumber = AVIpacket.PNSR;

	//packet->eAviPacket.packetStatus = ;
	packet->eAviPacket.packet.type = AVIpacket.type;
	packet->eAviPacket.packet.version = AVIpacket.version;
	packet->eAviPacket.packet.length = AVIpacket.length;

	memcpy((void *)&packet->eAviPacket.packet.dataBytes , (void *)&gVportRegAx->reg_pkt_avi_dat ,  sizeof(char)*28);
#else

	unsigned int *data;

	// AVI packet data structure
	struct{
		UINT32	type		: 8;
		UINT32	version		: 8;
		UINT32	length		: 8;
		UINT32	resv4		: 8;

		UINT32	data6		: 32;
		UINT32	data5		: 32;
		UINT32	data4		: 32;

		UINT32	PNSR		: 16;		// data3
		UINT32	resv3		: 16;

		UINT32	LNSB		: 16;		// data2
		UINT32	PNEL		: 16;

		UINT32	VIC06		: 7;		// data1
		UINT32	resv1		: 1;
		UINT32	PR03		: 4;
		UINT32	CN			: 2;
		UINT32	YQ			: 2;
		UINT32	LNET		: 16;

		UINT32	CheckSum	: 8;		// data0
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

#if 1
	UINT32 *addr;

	addr = (UINT32 *)&gVportRegAx->reg_pkt_avi_hdr;
	memcpy((void *)&AVIpacket , (void *)addr , sizeof(unsigned int)*8);
#else
	memcpy((void *)&AVIpacket , (void *)&gVportRegAx->reg_pkt_avi_hdr , sizeof(unsigned int)*8);
#endif


	if (!HDMI_GetPortConnection()) { memset((void *)packet , 0 , sizeof(LX_HDMI_AVI_PACKET_T)); goto func_exit; }

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

	*(data + 6) = gVportRegAx->reg_pkt_avi_dat.reg_pkt_avi_dat;
	*(data + 5) = gVportRegAx->reg_pkt_avi_dat__2.reg_pkt_avi_dat__2;
	*(data + 4) = gVportRegAx->reg_pkt_avi_dat__3.reg_pkt_avi_dat__3;
	*(data + 3) = gVportRegAx->reg_pkt_avi_dat__4.reg_pkt_avi_dat__4;
	*(data + 2) = gVportRegAx->reg_pkt_avi_dat__5.reg_pkt_avi_dat__5;
	*(data + 1) = gVportRegAx->reg_pkt_avi_dat__6.reg_pkt_avi_dat__6;
	*(data + 0) = gVportRegAx->reg_pkt_avi_dat__7.reg_pkt_avi_dat__7;

	memcpy((void *)&packet->eAviPacket.packet.dataBytes , (void *)data ,  sizeof(char)*28);

	packet->eAviPacket.bHdmiMode = gBootData->mode;

	kfree(data);
#endif

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
int HDMI_L8Ax_GetSpdPacket(LX_HDMI_SPD_PACKET_T *packet)
{
	int ret = 0;

	return ret;
}

/**
 * HDMI_SetHPDEnDisable
 *
 * @parm LX_HDMI_HPD_T
 * @return int
*/
int HDMI_L8Ax_SetHPDEnDisable(LX_HDMI_HPD_T *hpd)
{
	int ret = 0;

	//_writew(&gVportRegAx->phy1p_hpd3_i, 0x1111);			// HPD PIN Disable(HPD:High) : 0

	switch(hpd->u8HDMIPort)
	{
		case 0:
			gVportRegAx->phy1p_hpd3_i.phy3p_hpd0_i = !hpd->bEnable;		// 1 : Enable(HPD:Low), 0 : Disable(HPD:High)
		break;
		case 1:
			gVportRegAx->phy1p_hpd3_i.phy3p_hpd1_i = !hpd->bEnable;
		break;
		case 2:
			gVportRegAx->phy1p_hpd3_i.phy3p_hpd2_i = !hpd->bEnable;
		break;
		case 3:
			gVportRegAx->phy1p_hpd3_i.phy3p_hpd3_i = !hpd->bEnable;
		break;
		default:
			HDMI_PRINT("WARNING : port number is out of range\n");
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
int HDMI_L8Ax_SetPort(LX_HDMI_HPD_T *hpd)
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
		gVportRegAx->reg_swrst_linksys.reg_swrst_linktmds = 1;
		gVportRegAx->reg_swrst_linksys.reg_swrst_linkhdcp = 1;
		gVportRegAx->reg_swrst_linksys.reg_swrst_linkpix = 1;

		//mdelay(100);

		/* Port	Power down control */
		HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);

		if(hpd->u8HDMIPort < 2)
			type = 	HDMI_PHY0_PORT;
		else
			type = 	HDMI_PHY1_PORT;
		// try to do power on without status check
		HDMI_SetPowerControl(type , 1);

		/* Port Select */
		gVportRegAx->reg_scdt.reg_prt_sel = hpd->u8HDMIPort;
		gBootData->pixelRepet = gBootData->mode = 0;
		_gHDMI_port_changed = gBootData->connectCn[gBootData->hdmi_switch];
		_gHDMI_DVI_Done = _gHDMI_Change = 0;

		//mdelay(50);
		gVportRegAx->reg_swrst_linksys.reg_swrst_linktmds = 0;
		gVportRegAx->reg_swrst_linksys.reg_swrst_linkpix = 0;
		gVportRegAx->reg_swrst_linksys.reg_swrst_linkhdcp = 0;
	}
	else
	{
		/* Port	Power down control */
		HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);
		//_writew(&gVportRegAx->phy1p_hpd3_i, 0x1111);			// HPD PIN Disable(HPD:High) : 0
		/* Port Select */
		gVportRegAx->reg_scdt.reg_prt_sel = hpd->u8HDMIPort;

		return ret;
	}

#ifdef HDMI_L8B0		/* L8 A1 bug용 patch */

	pstHdmiPhyReg->phy0.hdmi_phy_pll04.pi_pll_dat_sel = 0;
	pstHdmiPhyReg->phy1.hdmi_phy_pll04.pi_pll_dat_sel = 0;
	pstHdmiPhyReg->phy0.hdmi_phy_eq01.eq_dat_sel = 0;
	pstHdmiPhyReg->phy1.hdmi_phy_eq01.eq_dat_sel = 0;

	if(hpd->u8HDMIPort == 0)
	{
		// 0xc001e044 0x20
		pstHdmiPhyReg->phy0.hdmi_phy_eq01.eq_dat_sel = 1;
		pstHdmiPhyReg->phy0.hdmi_phy_eq01.eq_pt_sel = 0;
	}
	if(hpd->u8HDMIPort == 1)
	{
		//0xc001e044 0x21
		pstHdmiPhyReg->phy0.hdmi_phy_eq01.eq_dat_sel = 1;
		pstHdmiPhyReg->phy0.hdmi_phy_eq01.eq_pt_sel = 1;
	}
	if(hpd->u8HDMIPort == 2)
	{
		//0xc001e244 0x20
		pstHdmiPhyReg->phy1.hdmi_phy_eq01.eq_dat_sel = 1;
		pstHdmiPhyReg->phy1.hdmi_phy_eq01.eq_pt_sel = 0;

	  	/* Color depth Setting for port 3 color depth bug */
		//0xc001e2d0 0x12
		//mdelay(1000);			// should be removed in Rev. B0.	SC Jung
		//pstHdmiPhyReg->phy1.hdmi_phy_pll04.pi_pll_dat_sel = 1;
		//pstHdmiPhyReg->phy1.hdmi_phy_pll04.pi_div = gVportRegAx->reg_h_pol.reg_cdepth;
		//printk("HDMI >> color depth on phy = %d\n",pstHdmiPhyReg->phy1.hdmi_phy_pll04.pi_div);
	}
	if(hpd->u8HDMIPort == 3)
	{
		//0xc001e244 0x20
		pstHdmiPhyReg->phy1.hdmi_phy_eq01.eq_dat_sel = 1;
		pstHdmiPhyReg->phy1.hdmi_phy_eq01.eq_pt_sel = 1;
	}
#endif

	return ret;
}

/**
 * HDMI_SetPowerDown
 *
 * @parm HDMI_PHY_TYPE_T
 * @return int
*/
int HDMI_L8Ax_SetPowerControl(HDMI_PHY_TYPE_T type , int power)
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

	pstHdmiPhyReg->phy0.hdmi_phy_rsv00.pdb_all = pstHdmiPhyReg->phy0.hdmi_phy_rsv00.resetb_all = (pdbAll[0] == 0) ?  0 : 1;
	pstHdmiPhyReg->phy1.hdmi_phy_rsv00.pdb_all = pstHdmiPhyReg->phy1.hdmi_phy_rsv00.resetb_all = (pdbAll[1] == 0) ?  0 : 1;

	if(power == 0) // Power down
	{
		pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div2 = pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div2 = 0;
		pstHdmiPhyReg->phy0.hdmi_phy_pll03.pi_pclk_div4 = pstHdmiPhyReg->phy1.hdmi_phy_pll03.pi_pclk_div4 = 0;
	}

	return ret;
}


int HDMI_L8Ax_RunReset(void)
{
	gVportRegAx->reg_dbg_lk_bypass.reg_dbg_ef_bak = 1;			// not used hdcp key ROM
	gVportRegAx->reg_dbg_lk_bypass.reg_dbg_acr_soft = 1;

	// hdmi link sys on
	gVportRegAx->reg_swrst_linksys.reg_swrst_linkvr = 0;
	gVportRegAx->reg_swrst_linksys.reg_swrst_linkvw = 0;
	gVportRegAx->reg_swrst_linksys.reg_swrst_linkpix = 0;
	gVportRegAx->reg_swrst_linksys.reg_swrst_linktmds = 0;
	gVportRegAx->reg_swrst_linksys.reg_swrst_linkhdcp = 0;
	gVportRegAx->reg_swrst_linksys.reg_swrst_linkexsr = 0;
	//gVportRegAx->reg_swrst_linksys.reg_swrst_linksys = 0;

	// interrupt enable
	//gVportRegAx->intr_vp3_enable.intr_vp3_enable = 0x01fbffff;
	//gVportRegAx->intr_vp2_enable.intr_vp2_enable = 0xffffffff;

#ifdef HDMI_L8A1_PACKAGE_SLOW  // L8 A1 PHY Issue : should be removed in Rev. B0.
	// Regulating voltage up in HDMI(100mV)
	// 0xc001e034, 0xc001e090
	pstHdmiPhyReg->phy0.hdmi_phy_cr13.cr_vbgr_ctrl = 5;
	pstHdmiPhyReg->phy1.hdmi_phy_cr13.cr_vbgr_ctrl = 5;

	pstHdmiPhyReg->phy0.hdmi_phy_dr04.dr_ictrl = 1;
	pstHdmiPhyReg->phy1.hdmi_phy_dr04.dr_ictrl = 1;

	// EQ Bandwidth Control : L8 A1 특성 때문
	// 0xc001e06c
	pstHdmiPhyReg->phy0.hdmi_phy_eq11.eq_hldiv = 1;
	pstHdmiPhyReg->phy1.hdmi_phy_eq11.eq_hldiv = 1;

	/* sh.myoung 2010-08-16
	HDP & Source Power On/Off 시 PHY 오 동작 (cr_lock =0) in SS BD-C6900
	초기 HDMI clock signal 의 Quality 문제 : watch dog timer 사용
	Register default value = 1 setting : should be removed in Rev. B0.
	*/
	pstHdmiPhyReg->phy0.hdmi_phy_cr04.cr_test = 1;
	pstHdmiPhyReg->phy1.hdmi_phy_cr04.cr_test = 1;


#ifdef HDMI_L8B0		// L8 A1 HDMI LINK Issue : should be removed in Rev. B0.
	/* sh.myoung 2010-08-17
	Register default value 0x00 -> 0x06: should be removed in Rev. B0.
	HDMI/DVI 확인 하는 기준 값
	*/
	gVportRegAx->reg_mute_pol.reg_pre_crit = 6;

#endif

#endif	// L8 A1 PHY Issue : should be removed in Rev. B0.

#ifdef HDMI_L8A1_ADAPTIVE_LOOP
	#if 0 // Snow Noise SS BD-C6900 &&  Snoy PS3 : Mamual EQ Setting
	 	pstHdmiPhyReg->phy0.hdmi_phy_cr03.cr_icp_adj = 7;
		pstHdmiPhyReg->phy0.hdmi_phy_eq06.eq_thr_h = 0x80;
		pstHdmiPhyReg->phy0.hdmi_phy_eq07.eq_thr_l = 0x80;
		pstHdmiPhyReg->phy0.hdmi_phy_eq11.eq_hldiv = 0;

		pstHdmiPhyReg->phy1.hdmi_phy_cr03.cr_icp_adj = 7;
		pstHdmiPhyReg->phy1.hdmi_phy_eq06.eq_thr_h = 0x80;
		pstHdmiPhyReg->phy1.hdmi_phy_eq07.eq_thr_l = 0x80;
		pstHdmiPhyReg->phy1.hdmi_phy_eq11.eq_hldiv = 0;
 	#else
		pstHdmiPhyReg->phy0.hdmi_phy_eq02.eq_cs_sel = 0;
		pstHdmiPhyReg->phy0.hdmi_phy_eq02.eq_rs_sel = 0;
		//pstHdmiPhyReg->phy0.hdmi_phy_eq04.eq_rs = 2;
		//pstHdmiPhyReg->phy0.hdmi_phy_eq05.eq_cs = 3;
		//pstHdmiPhyReg->phy0.hdmi_phy_eq13.eq_af_en = 0;

		pstHdmiPhyReg->phy1.hdmi_phy_eq02.eq_cs_sel = 0;
		pstHdmiPhyReg->phy1.hdmi_phy_eq02.eq_rs_sel = 0;
		//pstHdmiPhyReg->phy1.hdmi_phy_eq04.eq_rs = 2;
		//pstHdmiPhyReg->phy1.hdmi_phy_eq05.eq_cs = 3;
		//pstHdmiPhyReg->phy1.hdmi_phy_eq13.eq_af_en = 0;
	#endif // Snow Noise SS BD-C6900 &&  Snoy PS3 : Mamual EQ Setting
#endif

	// HDCP Problem to the Master
	// 0xc001e070, 0xc001e074
	pstHdmiPhyReg->phy0.hdmi_phy_eq12.eq_start_frz = 47;
	pstHdmiPhyReg->phy1.hdmi_phy_eq12.eq_start_frz = 47;

	pstHdmiPhyReg->phy0.hdmi_phy_eq13.eq_avg_width = 1;
	pstHdmiPhyReg->phy1.hdmi_phy_eq13.eq_avg_width = 1;

  	//Interlaced video 입력에서 field를 구분하는 방법
  	gVportRegAx->reg_pr_man.reg_lbp_f2 = 1;

	// HPD PIN Control Enable : add sh.myoung 2010.09.30
	_writew(&gVportRegAx->phy1p_hpd3_oen, 0x0000);			// HPD PIN Enable

	return 0;
}

int HDMI_L8Ax_HWInitial(void)
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
int HDMI_L8Ax_SetAudio(void)
{
	// Audio sw reset
	//gVportRegAx->reg_swrst_adto.reg_swrst_adto = 0;
	//gVportRegAx->reg_swrst_adto.reg_swrst_apll = 0;
	//gVportRegAx->reg_swrst_adto.reg_swrst_linkaw = 0;
	//gVportRegAx->reg_swrst_adto.reg_swrst_linkar = 0;
	memset ((UINT32 *)&gVportRegAx->reg_swrst_adto, 0, sizeof (UINT32));

	gVportRegAx->reg_dbg_lk_bypass.reg_dbg_acr_soft = 1;
	gVportRegAx->reg_aac_en.reg_avc_en = 1;
	gVportRegAx->reg_aac_en.reg_aac_en = 1;

#if 1
	// Audio 128 * Fs setting
	gVportRegAx->clksel_irisin.clksel_linkapll = 1;
#else	//temp for L8 A1 Chip only
	// Audio 256 * Fs setting
	gVportRegAx->clksel_irisin.clksel_linkapll = 0;		// Audio PLL enable
	gVportRegAx->reg_asp_layout.reg_acr_mp = 0x2;			// Audio clock generation : N value in N * Fs
	gVportRegAx->reg_asp_layout.reg_acr_fs_sel = 0x0;		// Audio clock generation : Fs
	gVportRegAx->reg_asp_layout.reg_acr_ncts_sel = 0x0;	// Audio clock generation : N, CTS
#endif

	gVportRegAx->reg_i2s_shift1.reg_i2s_sck = 0;			// SCK edge : rising edge
	gVportRegAx->reg_i2s_pcm_only.reg_i2s_sd_en = 0xF;	// I2S data output enable
	gVportRegAx->reg_i2s_gnd_mode.reg_i2s_gnd_mode = 1;	// I2S output enable
	gVportRegAx->reg_i2s_pcm_only.reg_i2s_mclk_en = 1;	// I2S MCLK enable
	gVportRegAx->reg_i2s_gnd_mode.reg_sp_en = 1;			// S/PDIF output enable

	return 0;
}

/**
 * HDMI_GetAudioInfo
 *
 * @parm LX_HDMI_AUDIO_INFO_T *
 * @return int
*/
int HDMI_L8Ax_GetAudioInfo(LX_HDMI_AUDIO_INFO_T *audio_info)
{
	int ret = 0;
	LX_HDMI_AUDIO_TYPE_T		audioType		= 0;	///< HDMI Audio Type.
	LX_HDMI_SAMPLING_FREQ_T		samplingFreq	= 0;	///< Sampling Frequency
	BOOLEAN						bPortConnected 	= 0;	///< Port connect status (1 : connected, 0 : not connected)
	BOOLEAN						bInterlaced		= 0;	///< Scan type (1 : interlace, 0 : progressive)
	BOOLEAN						mode 			= 0;

	//Check HDMI port connection
	if( !HDMI_GetPortConnection() && _gHDMI_port_changed > 0 )
	{
		HDMI_PRINT("HDMI_GetAudioInfo : Not Connected Error!!!\n");
		goto func_exit;
	}

	//Check video active size
	if( (gVportRegAx->reg_h_av.reg_v_av <  240) || (gVportRegAx->reg_h_av.reg_h_av <  320)	\
	 || (gVportRegAx->reg_h_av.reg_v_av > 2970) || (gVportRegAx->reg_h_av.reg_h_av > 3840) )
	{
		HDMI_PRINT("HDMI_GetAudioInfo : Video Active Size Error!!!\n");
		goto func_exit;
	}

	audioType		= gVportRegAx->reg_achst_byte3.reg_achst_byte0 & 0x2;		//bit 1
	samplingFreq 	= gVportRegAx->reg_achst_byte3.reg_achst_byte3 & 0x0F;		//bit 24 ~ 27
	bInterlaced 	= gVportRegAx->reg_h_pol.reg_intrl;
	mode			= gVportRegAx->reg_scdt.reg_hdmi_mode;
	bPortConnected 	= 1;

	//set hdmi audio type
	if(mode)
	{
		// set PCM or AC-3 audio type
		if(audioType)		audioType = LX_HDMI_AUDIO_AC3;
		else				audioType = LX_HDMI_AUDIO_PCM;

		//set sampling frequency
		switch((UINT32)samplingFreq)
		{
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
				HDMI_ERROR("HDMI_GetAudioInfo : samplingFreq = 0x%X\n", samplingFreq);
				break;
		}
	}
	else
	{
		audioType = LX_HDMI_AUDIO_DVI;
		samplingFreq = LX_HDMI_SAMPLING_FREQ_48_KHZ;
		bInterlaced = 0;
	}

func_exit:
	audio_info->audioType 		= audioType;
	audio_info->samplingFreq 	= samplingFreq;
	audio_info->bPortConnected 	= bPortConnected;
	audio_info->bInterlaced 	= bInterlaced;

	HDMI_PRINT("HDMI_GetAudioInfo : type = 0x%X, freq = 0x%X, bConnected = %d, bInterlaced = %d\n", audio_info->audioType, audio_info->samplingFreq, audio_info->bPortConnected, audio_info->bInterlaced);
	//HDMI_ERROR("HDMI_GetAudioInfo : type = 0x%X, freq = 0x%X, bConnected = %d, bInterlaced = %d\n", audio_info->audioType, audio_info->samplingFreq, audio_info->bPortConnected, audio_info->bInterlaced);

	return ret;
}

/**
 * HDMI_SetMute
 *
 * @parm UINT32 *
 * @return int
*/
int HDMI_L8Ax_SetMute(LX_HDMI_MUTE_CTRL_T *mute)
{
	BOOLEAN videoMute = 0;
	BOOLEAN audioMute = 0;

	videoMute = (mute->bVideoMute == 1) ? 1 : 0;
	audioMute = (mute->bAudioMute == 1) ? 1 : 0;

	gVportRegAx->reg_mute_pol.reg_mute_vid = videoMute;
	gVportRegAx->reg_mute_pol.reg_mute_aud = audioMute;

	return 0;
}


/**
 * HDMI_SetArc
 *
 * @parm LX_HDMI_ARC_CTRL_T *
 * @return int
*/
int HDMI_L8Ax_SetArc(LX_HDMI_ARC_CTRL_T *arc)
{

	return 0;
}

/**
 * HDMI_SetRGBYCbCr
 *
 * @parm UINT32 *
 * @return int
*/
int HDMI_L8Ax_SetRGBYCbCr(LX_HDMI_AVI_COLORSPACE_T *space)
{
	UINT32 u32CSValue =0;

	switch(*space)
	{
			case LX_HDMI_AVI_COLORSPACE_YCBCR422:
			case LX_HDMI_AVI_COLORSPACE_YCBCR444:
				u32CSValue = 0x800080;
			break;

			case LX_HDMI_AVI_COLORSPACE_RGB:
			default:
				u32CSValue = 0x000000;
			break;
	}

	_writew(&gVportRegAx->reg_vid_blank_r, u32CSValue);
	return 0;
}

/**
 *  HDMI_PowerConsumption
 *
 *  @param LX_HDMI_POWER_CTRL_T *
 *  @return int
*/
int HDMI_L8Ax_PowerConsumption(LX_HDMI_POWER_CTRL_T *power)
{

	return 0;
}

/**
 * HDMI_GetRegister
 *
 * @parm UINT32 *
 * @return int
*/
int HDMI_L8Ax_GetRegister(UINT32 addr , UINT32 *value)
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
int HDMI_L8Ax_SetRegister(UINT32 addr , UINT32 value)
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
irqreturn_t HDMI_L8Ax_IRQHandler(int irq, void *dev)
{

#if 0
	if(gVportReg->hdmi_status.hdmi_no_avi)		// interrupt type
	{

	//	printk("HDMI int0\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<0);	// clear the interrupt
	}
	if(gVportReg->hdmi_status.hdmi_no_gcp)
	{

	//	printk("HDMI int1\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<1);
	}
	if(gVportReg->hdmi_status.hdmi_new_avi)
	{

	//	printk("HDMI int2\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<2);
	}
	if(gVportReg->hdmi_status.hdmi_new_gcp)
	{

	//	printk("HDMI int3\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<3);
	}
	if(gVportReg->hdmi_status.hdmi_new_asp)
	{

	//	printk("HDMI int4\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<4);
	}
	if(gVportReg->hdmi_status.hdmi_new_acr)
	{

	//	printk("HDMI int5\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<5);
	}
	if(gVportReg->hdmi_status.hdmi_new_gbd)
	{

	//	printk("HDMI int6\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<6);
	}
	if(gVportReg->hdmi_status.hdmi_new_acp)
	{

	//	printk("HDMI int7\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<7);
	}
	if(gVportReg->hdmi_status.hdmi_new_mpg)
	{

	//	printk("HDMI int8\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<8);
	}
	if(gVportReg->hdmi_status.hdmi_new_aud)
	{

	//	printk("HDMI int9\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<9);
	}
	if(gVportReg->hdmi_status.hdmi_new_spd)
	{

	//	printk("HDMI int10\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<10);
	}
	if(gVportReg->hdmi_status.hdmi_new_unr)
	{

	//	printk("HDMI int11\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<11);
	}
	if(gVportReg->hdmi_status.hdmi_hwcts_chg)
	{

	//	printk("HDMI int12\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<12);
	}
	if(gVportReg->hdmi_status.hdmi_hwn_chg)
	{

	//	printk("HDMI int13\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<13);
	}
	if(gVportReg->hdmi_status.hdmi_fs_chg)
	{

	//	printk("HDMI int14\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<14);
	}
	if(gVportReg->hdmi_status.hdmi_spdif_err)
	{

	//	printk("HDMI int15\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<15);
	}
	if(gVportReg->hdmi_status.hdmi_afifo_undr)
	{

	//	printk("HDMI int16\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<16);
	}
	if(gVportReg->hdmi_status.hdmi_afifo_ovrr)
	{

	//	printk("HDMI int17\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<17);
	}
	if(gVportReg->hdmi_status.hdmi_vr_chg)
	{

	//	printk("HDMI int19\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<19);
	}
	if(gVportReg->hdmi_status.hdmi_hr_chg)
	{

	//	printk("HDMI int20\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<20);
	}
	if(gVportReg->hdmi_status.hdmi_po_chg)
	{

	//	printk("HDMI int21\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<21);
	}
	if(gVportReg->hdmi_status.hdmi_il_chg)
	{

	//	printk("HDMI int22\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<22);
	}
	if(gVportReg->hdmi_status.hdmi_vfifo_undr)
	{

	//	printk("HDMI int23\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<23);
	}
	if(gVportReg->hdmi_status.hdmi_vfifo_ovrr)
	{

	//	printk("HDMI int24\n");
		gVportReg->intr_vp3_clear.intr_vp3_clear |= (1<<24);
	}
#endif
	return IRQ_HANDLED;
}


void HDMI_L8Ax_EnableInterrupt(void)
{

	// ...
/*
	if (request_irq(L8_IRQ_VP_HDMI,(irq_handler_t)HDMI_IRQHandler,0,"adc", NULL))
	{
		HDMI_ERROR("request_irq  is failed\n");
	}
*/
}



#ifdef	KDRV_CONFIG_PM
int HDMI_L8Ax_RunSuspend(void)
{
	// confirm initialization done
	if(gVportRegAx == NULL)
		return -1;

	memcpy((void *)&guiPmHpdStatus , (void *)&gVportRegAx->phy1p_hpd3_i , sizeof(unsigned int));

	guiPmInterruptStatus = gVportRegAx->intr_vp3_enable.intr_vp3_enable;

	HDMI_SetPowerControl(HDMI_ALL_PHY_PORT , 0);	// all port off

	return 0;
}

int HDMI_L8Ax_RunResume(void)
{

	HDMI_RunReset();

	HDMI_SetAudio();

	memcpy((void *)&gVportRegAx->phy1p_hpd3_i , (void *)&guiPmHpdStatus , sizeof(unsigned int));

	if(gstPmPortSel.u8HDMIPort != 0xff)
		HDMI_SetPort(&gstPmPortSel);

	gVportRegAx->intr_vp3_enable.intr_vp3_enable = guiPmInterruptStatus;

	return 0;
}
#endif
