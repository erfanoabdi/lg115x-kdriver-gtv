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

//#include "vport_reg.h"
//#include "drv.h"
#include "hdmi_drv.h"
#include "hdmi_kapi.h"
#include "hdmi_module.h"
#include "hdmi_phy_reg.h"
#include "vport_reg.h"

#include "../i2c/i2c_core_api.h"
#include <linux/delay.h>	/** for mdelay	*/
/******************************************************************************
 *				DEFINES
 *****************************************************************************/
#define	I2C_PORT	1

#define	HDMI_ADDR0	0x60
#define	HDMI_ADDR1	0x68
#define	HDMI_PHY	0x6C

#define	HDMI_OTHER	0x98
/**
 *	Global variables of the driver
 */


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
static int I2C_read_buffer(unsigned char slave , unsigned int addr , unsigned int size, unsigned int *value);
static int I2C_write_buffer(unsigned char slave , unsigned int addr , unsigned int size, unsigned int *value);
static int I2C_read(unsigned char slave , unsigned int addr , unsigned char *buf);
static int I2C_write(unsigned char slave , unsigned int addr , unsigned char value);

/**
* HDMI_GetMode
*
* @parm LX_HDMI_MODE_T
* @return int
*/
int HDMI_GetMode(LX_HDMI_MODE_T *mode)
{
	int ret = 0;

	// HDMI/DVI mode status
	mode->srcIdx = LX_HDMI_MAIN_WINDOW;
	//mode->bHDMI = gVportReg->reg_scdt.reg_hdmi_mode;
	I2C_read(HDMI_ADDR0 , 0x71 , (unsigned char *)&mode->bHDMI);
	mode->bHDMI ^= 1;
	return ret;
}

/**
* HDMI_GetAspectRatio
*
* @parm LX_HDMI_ASPECTRATIO_T
* @return int
*/
int HDMI_GetAspectRatio(LX_HDMI_ASPECTRATIO_T *ratio)
{
	int ret = 0;
	unsigned char temp;

	// aspect ratio for AVI
	ratio->srcIdx = LX_HDMI_MAIN_WINDOW;
	//ratio->eAspectRatio = ((gVportReg->reg_pkt_avi_dat.reg_pkt_avi_dat)&0x300000 )>>20;
	I2C_read(HDMI_ADDR0 , 0x80 , &temp);
	ratio->eAspectRatio = (temp&0x30 >> 4) ;

	return ret;

}

/**
* HDMI_GetTimingInfo
*
* @parm HDMI_IOR_GET_TIMING_INFO
* @return int
*/
int HDMI_GetTimingInfo(LX_HDMI_TIMING_INFO_T *info)
{
	int ret = 0;

	info->srcIdx = LX_HDMI_MAIN_WINDOW;	///< window index
	info->hFreq = 0; 				///< Horizontal frequency(100 Hz unit)
	info->vFreq = 0; 				///< Veritical frequency(1/10 Hz unit)

//	info->hTotal = gVportReg->reg_h_tot.reg_h_tot; 				///< Horizontal total pixels
//	info->vTotal = gVportReg->reg_h_tot.reg_v_tot; 				///< Vertical total lines
//	info->hStart = gVportReg->reg_h_fp.reg_h_fp; 				///< Horizontal start pixel
//	info->vStart = gVportReg->reg_h_fp.reg_v_fp;				///< Vertical start lines
//	info->hActive = gVportReg->reg_h_av.reg_h_av;				///< Horizontal active pixel
//	info->vActive = gVportReg->reg_h_av.reg_v_av; 			///< Vertical active lines
//	info->scanType = gVportReg->reg_h_pol.reg_intrl; 		///< Scan type (0 : interlace, 1 : progressive)

	I2C_read_buffer(HDMI_ADDR0 , 0x10 , 2 , (unsigned int *)&info->hTotal);
	I2C_read_buffer(HDMI_ADDR0 , 0x12 , 2 , (unsigned int *)&info->vTotal);
	I2C_read_buffer(HDMI_ADDR0 , 0x18 , 1 , (unsigned int *)&info->hStart);
	I2C_read_buffer(HDMI_ADDR0 , 0x19 , 1 , (unsigned int *)&info->vStart);
	I2C_read_buffer(HDMI_ADDR0 , 0x14 , 2 , (unsigned int *)&info->hActive);
	I2C_read_buffer(HDMI_ADDR0 , 0x16 , 2 , (unsigned int *)&info->vActive);
	I2C_read_buffer(HDMI_ADDR0 , 0x21 , 1 , (unsigned int *)&info->scanType);

	info->scanType ^= 1;
	return ret;
}

/**
* HDMI_GetStatus
*
* @parm LX_HDMI_STATUS_T
* @return int
*/
int HDMI_GetStatus(LX_HDMI_STATUS_T *status)
{
	int ret = 0;

	status->srcIdx = LX_HDMI_MAIN_WINDOW;
	//status->bHdmiMode = gVportReg->reg_scdt.reg_hdmi_mode;	// HDMI/DVI mode
	I2C_read(HDMI_ADDR0 , 0x71 , (unsigned char *)&status->bHdmiMode);

	//status->u8hdmiport = gVportReg->reg_scdt.reg_prt_sel;	// port number
	I2C_read(HDMI_ADDR0 , 0x0e , &status->u8hdmiport);

	switch(status->u8hdmiport)	// HPD enable status
	{
/*
		case 0:
			status->eHotPlug = gVportReg->phy1p_hpd3_i.phy3p_hpd0_i;
		break;
		case 1:
			status->eHotPlug = gVportReg->phy1p_hpd3_i.phy3p_hpd1_i;
		break;
		case 2:
			status->eHotPlug = gVportReg->phy1p_hpd3_i.phy3p_hpd2_i;
		break;
		case 3:
			status->eHotPlug = gVportReg->phy1p_hpd3_i.phy1p_hpd3_i;
		break;
		default:
			printk("WARNING : port number is out of range\n");
		break;
*/
		I2C_read(HDMI_ADDR0 , 0x0b , (unsigned char *)&status->eHotPlug);
	}



	return ret;
}

/**
* HDMI_GetVsiPacket
*
* @parm LX_HDMI_VSI_PACKET_T
* @return int
*/
int HDMI_GetVsiPacket(LX_HDMI_VSI_PACKET_T *packet)
{
	int ret = 0;





	return ret;
}

/**
* HDMI_GetAviPacket
*
* @parm LX_HDMI_AVI_PACKET_T
* @return int
*/
int HDMI_GetAviPacket(LX_HDMI_AVI_PACKET_T *packet)
{
	int ret = 0;
	unsigned int temp[8];

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


	packet->srcIdx = LX_HDMI_MAIN_WINDOW;


//	memcpy((void *)&AVIpacket , (void *)&gVportReg->reg_pkt_avi_hdr , sizeof(unsigned int)*5);
	I2C_read_buffer(HDMI_ADDR0 , 0x7b , 3 , &temp[0]);
	I2C_read_buffer(HDMI_ADDR0 , 0x7e , 4 , &temp[1]);
	I2C_read_buffer(HDMI_ADDR0 , 0x82 , 4 , &temp[2]);
	I2C_read_buffer(HDMI_ADDR0 , 0x86 , 4 , &temp[3]);
	I2C_read_buffer(HDMI_ADDR0 , 0x8a , 4 , &temp[4]);
	I2C_read_buffer(HDMI_ADDR0 , 0x8e , 4 , &temp[5]);
	I2C_read_buffer(HDMI_ADDR0 , 0x92 , 4 , &temp[6]);
	I2C_read_buffer(HDMI_ADDR0 , 0x96 , 4 , &temp[7]);

	memcpy((void *)&AVIpacket , (void *)&temp , sizeof(unsigned int)*5);

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

	memcpy((void *)&packet->eAviPacket.packet.dataBytes , (void *)&temp[1] ,  sizeof(char)*28);

	return ret;
}


int HDMI_SetHPDEnDisable(LX_HDMI_HPD_T *hpd)
{
	int ret = 0;

	// port HPD set ??
	switch(hpd->u8HDMIPort)
	{
		case 0:
		//	gVportReg->phy1p_hpd3_i.phy3p_hpd0_i = hpd->bEnable;
		break;
		case 1:
		//	gVportReg->phy1p_hpd3_i.phy3p_hpd1_i = hpd->bEnable;
		break;
		case 2:
		//	gVportReg->phy1p_hpd3_i.phy3p_hpd2_i = hpd->bEnable;
		break;
		case 3:
		//	gVportReg->phy1p_hpd3_i.phy1p_hpd3_i = hpd->bEnable;
		break;
		default:
			printk("WARNING : port number is out of range\n");
		break;
	}


	return ret;
}


int HDMI_SetPort(LX_HDMI_HPD_T *hpd)
{
	int ret = 0;
	HDMI_PHY_TYPE_T type;

	// audio mute

	if(hpd->u8HDMIPort < 2)
	{
		type = 	HDMI_PHY0_PORT;
	}
	else
	{
		type = 	HDMI_PHY1_PORT;
	}

	// try to do power on without status check
	HDMI_SetPowerControl(type , 1);

	// port select
	//gVportReg->reg_scdt.reg_prt_sel = hpd->u8HDMIPort;
	I2C_write(HDMI_ADDR0 , 0xe , hpd->u8HDMIPort);

	// audio on

	return ret;
}

/**
* HDMI_SetPowerDown
*
* @parm HDMI_PHY_TYPE_T
* @return int
*/
int HDMI_SetPowerControl(HDMI_PHY_TYPE_T type , int power)
{
	int ret = 0;

	switch(type)
	{
		case HDMI_ALL_PHY_PORT:
			if(power)	// power up
			{
		//		pstHdmiPhyReg->phy0.hdmi_phy_rsv00.pdb_all = 1;
		//		pstHdmiPhyReg->phy1.hdmi_phy_rsv00.pdb_all = 1;
			}
			else		// power down
			{
		//		pstHdmiPhyReg->phy0.hdmi_phy_rsv00.pdb_all = 0;
		//		pstHdmiPhyReg->phy1.hdmi_phy_rsv00.pdb_all = 0;

			}
		break;
		case HDMI_PHY0_PORT:
			if(power)	// power up
			{
		//		pstHdmiPhyReg->phy0.hdmi_phy_rsv00.pdb_all = 1;
			}
			else		// power down
			{
		//		pstHdmiPhyReg->phy0.hdmi_phy_rsv00.pdb_all = 0;

			}
		break;
		case HDMI_PHY1_PORT:
			if(power)	// power up
			{
		//		pstHdmiPhyReg->phy1.hdmi_phy_rsv00.pdb_all = 1;
			}
			else		// power down
			{
		//		pstHdmiPhyReg->phy1.hdmi_phy_rsv00.pdb_all = 0;

			}
		break;

	}

	return ret;
}



/**
* HDMI_SetPowerDown
*
* @parm void
* @return int
*/
int HDMI_SetAudio(void)
{
	int ret = 0;

	return 0;

	//gVportReg->reg_i2s_shift1.reg_i2s_sck = 1;		// SCK edge
	I2C_write(HDMI_ADDR0 , 0x4f , 0x01);
	//gVportReg->reg_i2s_pcm_only.reg_i2s_sd_en = 0xF;	// IIS data output enable
	I2C_write(HDMI_ADDR0 , 0x54 , 0xf);
	//gVportReg->reg_i2s_gnd_mode.reg_i2s_gnd_mode = 1;	// IIS output enable
	I2C_write(HDMI_ADDR0 , 0x59 , 1);
	//gVportReg->reg_i2s_gnd_mode.reg_sp_en = 1;		// S/PDIF output enable
	I2C_write(HDMI_ADDR0 , 0x5a , 1);

	return ret;
}

int HDMI_HWInitial(void)
{
	// i2c intialize
	I2C_DEV_DriverInit();
	I2C_DEV_Install ( I2C_PORT, STANDARD_SPEED, MASTER_MODE );

	I2C_write(HDMI_ADDR1 , 0x61 , 1);	// not used hdcp key ROM
	I2C_write(HDMI_ADDR0 , 0x0E , 1);	// Port 2 선택]
    	I2C_write(HDMI_PHY , 0x45 ,0x01);	//PHY -> Link 방향으로 데이터 넘길 때 clock inversion 해서 안정화]
    	I2C_write(HDMI_PHY , 0x57 ,0x07);	// (L8과는 무관)    LVDS Dual Disable]
    	I2C_write(HDMI_ADDR1 , 0x5B , 0x01);	// (L8과는 무관)    LVDS ZHAN Control (Dual)]
    	I2C_write(HDMI_ADDR1 , 0x5F , 0x0d);	// (L8과는 무관)    LVDS ZHAN Control (Dual)]

    	I2C_write(HDMI_ADDR1 , 0x61 , 0x1);	// use HDCP internel key

#if 1
    	I2C_write(HDMI_ADDR0 , 0x4F , 0x01);	// (HDMI 자체 설정) I2S Audio Output에서 SCK를 negative edge로 설정함. L8에서는 default 값으로 반영됨.]
    	I2C_write(HDMI_OTHER , 0x09 , 0x04);	// (L8과는 무관)    Audio DAC을 I2S mode로 설정]
   	I2C_write(HDMI_OTHER , 0x0C , 0x80);	// (L8과는 무관)    Audio DAC의 Over Sampling을 32x로 설정]
    	I2C_write(HDMI_ADDR1 , 0x5D , 0x01);	// (HDMI 자체 설정) PHY의 Audio PLL을 사용하지 않고 RTL로 단순 분주한 audio clock을 사용함. 이 설정은 조작 편의성 때문에 사용.]


	// additional inform
	I2C_write(HDMI_PHY , 0x14 , 0x1F);      // 자체 설정) EQ RS]
	I2C_write(HDMI_PHY , 0x15 , 0x1F);      // 자체 설정) EQ CS]
	I2C_write(HDMI_PHY , 0x12 , 0x11);      // 자체 설정) EQ CONT SEL]
	I2C_write(HDMI_PHY , 0x13 , 0x10);      // 자체 설정) EQ REF SEL]
	I2C_write(HDMI_PHY , 0x16 , 0xF0);      // 자체 설정) EQ HF HYS]
	I2C_write(HDMI_PHY , 0x17 , 0xB3);      // 자체 설정) EQ LF HYS]
	I2C_write(HDMI_PHY , 0x18 , 0xFF);      // 자체 설정) EQ HF Number of Samples]
	I2C_write(HDMI_PHY , 0x19 , 0xFF);      // 자체 설정) EQ LF Number of Samples]
	I2C_write(HDMI_PHY , 0x1A , 0x30);      // 자체 설정) EQ Input Gm]
	I2C_write(HDMI_PHY , 0x1B , 0x33);      // 자체 설정) EQ Clock]
	I2C_write(HDMI_PHY , 0x1C , 0xFF);      // 자체 설정) EQ START FRZ]
	I2C_write(HDMI_PHY , 0x1E , 0x10);      // 자체 설정) EQ Offset]
#endif
	return 0;
}




static int I2C_read_buffer(unsigned char slave , unsigned int addr , unsigned int size, unsigned int *value)
{
	int ret = 0;
	unsigned char buff[4];

	//buff = (unsigned char *)kmalloc(sizeof(unsigned char)*size , GFP_KERNEL);

	memset(buff,0,4);
	ret = i2c_read(I2C_PORT,slave,addr,1,buff,size,0x20);

	switch(size)
	{
		case 1:
			*value = (unsigned int)(buff[0]<<0);
		break;
		case 2:
			*value = (unsigned int)(buff[0]<<0 | buff[1]<<8);
		break;
		case 3:
			*value = (unsigned int)(buff[0]<<0 | buff[1]<<8 | buff[2]<<16);
		break;
		case 4:
			*value = (unsigned int)(buff[0]<<0 | buff[1]<<8 | buff[2]<<16 | buff[3]<<24);
		break;
		default:
			*value = 0;
		break;

	}

	//kfree(buff);

	return ret;

}

static int I2C_write_buffer(unsigned char slave , unsigned int addr , unsigned int size, unsigned int *value)
{
	int ret = 0 , i;
	unsigned char buff[4];
	unsigned int temp;

	temp = *value;


	for(i=0 ; i < size ; i++)
	{
		buff[i] = (unsigned char)(temp&0xff);
		temp >>= 8;

	}

	ret = i2c_write(I2C_PORT,slave,addr,1,buff,size,0);

	mdelay(10);
	return ret;
}

static int I2C_read(unsigned char slave , unsigned int addr , unsigned char *buf)
{
	int ret;

	ret = i2c_read(I2C_PORT,slave,addr,1,buf,1,0x20);


	return ret;

}

static int I2C_write(unsigned char slave , unsigned int addr , unsigned char value)
{
	int ret;
	static unsigned char buf;

	buf = value;


	ret = i2c_write(I2C_PORT,slave,addr,1,&buf,1,0);

	mdelay(10);
	return ret;
}



int HDMI_GetI2CData(unsigned int sub , unsigned int addr , unsigned int size )
{
	unsigned int value;

	if(size > 4) return -1;


	switch(sub)
	{
		case 0:
			I2C_read_buffer(HDMI_ADDR0 , addr , size , &value);
			HDMI_PRINT(" HDMI_ADDR0 , %x : %x[%d]\n" , addr , value , size);
		break;

		case 1:
			I2C_read_buffer(HDMI_ADDR1 , addr , size , &value);
			HDMI_PRINT(" HDMI_ADDR1 , %x : %x[%d]\n" , addr , value , size);
		break;

		case 2:
			I2C_read_buffer(HDMI_PHY , addr , size , &value);
			HDMI_PRINT(" HDMI_PHY , %x : %x[%d]\n" , addr , value , size);
		break;

		case 3:
			I2C_read_buffer(HDMI_OTHER , addr , size , &value);
			HDMI_PRINT(" HDMI_OTHER , %x : %x[%d]\n" , addr , value , size);
		break;

		default:
			I2C_read_buffer(sub , addr , size , &value);
			HDMI_PRINT(" %x , %x : %x[%d]\n" ,sub , addr , value , size);
		break;

	}


	return 0;

}

int HDMI_SetI2CData(unsigned int sub , unsigned int addr , unsigned int size , unsigned int value)
{
	if(size > 4) return -1;


	switch(sub)
	{
		case 0:
			I2C_write_buffer(HDMI_ADDR0 , addr , size , &value);
			HDMI_PRINT(" HDMI_ADDR0 , %x : %x[%d]\n" , addr , value , size);
		break;

		case 1:
			I2C_write_buffer(HDMI_ADDR1 , addr , size , &value);
			HDMI_PRINT(" HDMI_ADDR1 , %x : %x[%d]\n" , addr , value , size);
		break;

		case 2:
			I2C_write_buffer(HDMI_PHY , addr , size , &value);
			HDMI_PRINT(" HDMI_PHY , %x : %x[%d]\n" , addr , value , size);
		break;

		case 3:
			I2C_write_buffer(HDMI_OTHER , addr , size , &value);
			HDMI_PRINT(" HDMI_OTHER , %x : %x[%d]\n" , addr , value , size);
		break;

		default:
			I2C_write_buffer(sub , addr , size , &value);
			HDMI_PRINT(" %x , %x : %x[%d]\n" ,sub , addr , value , size);
		break;

	}


	HDMI_GetI2CData(sub , addr , size);

	return 0;

}


