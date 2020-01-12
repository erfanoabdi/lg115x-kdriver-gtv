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
 *	CC module control Drivers linux module
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version		1.0
 *  date
 *  note		Additional information.
 *
 *  @addtogroup lg1150_CC
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
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

#include "ttx_kapi.h"

#include "ttx_module.h"
#include "ttx_hw.h"
#include "ttx_drv.h"

#include "../../afe/vport_reg_B0.h"


#ifdef	_WITHOUT_AFE_
volatile VPORT_Reg_T *gVportReg;	// <- prototype from AFE. because the
					//	registers for ADC, CVD, HDMI are in AFE modules
#else	//_WITHOUT_ADC_
extern volatile VPORT_RegBx_T *gVportRegBx;		// real register for VPORT modules
extern			VPORT_RegBx_T *gpRegVPORT;		// shadow register
#endif

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define	VBI_OFF	0
#define	VBI_ON	1

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
#ifdef	KDRV_CONFIG_PM
typedef struct{
	int uiPmCcIntStatus;
	eVBIDataType ePmCcModeStatus;
}S_CC_PM_DATA;

static S_CC_PM_DATA	*gCcPmData;
#endif
/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static volatile unsigned long gCC_addr;
/*========================================================================================
	Implementation Group
========================================================================================*/

int CC_L8B0_HWInitialize(void)
{
	int ret = 0;

	gCC_addr = (volatile unsigned long)ioremap(gstRegCfgTTX->vbi_register_base , gstRegCfgTTX->vbi_register_size);

#ifdef	KDRV_CONFIG_PM
	gCcPmData = (S_CC_PM_DATA	*)kmalloc(sizeof(S_CC_PM_DATA) , GFP_KERNEL);
	gCcPmData->uiPmCcIntStatus = 0;
	gCcPmData->ePmCcModeStatus = 0;
#endif

	// Set to Big Endian Mode (From B0 Revision, default endian is little endian)
	// Need to modify kdriver to use little endian.
#if 0
	gVportRegBx->vbi_ctrl_001.reg_vbi_little_endian = 0;
#else
	// memory allocation for local shadow register
	// gpRegVPORT = (VPORT_RegBx_T *)kmalloc(sizeof(VPORT_RegBx_T), GFP_KERNEL);

	VPORT_RdFL(vbi_ctrl_001);
	VPORT_Wr01(vbi_ctrl_001, reg_vbi_little_endian, 0);
	VPORT_WrFL(vbi_ctrl_001);
#endif

	CC_PRINT("%s done\n", __func__);
	return ret;
}

int CC_L8B0_HWUninitialize(void)
{
	int ret = 0;

	iounmap((void *)gCC_addr);

	return ret;
}

int CC_L8B0_SetInputFormat(eVBIDataType type)
{
	int ret = 0;

	CC_PRINT("%s input : %d\n" , __func__ , type);
#ifdef	KDRV_CONFIG_PM
	gCcPmData->ePmCcModeStatus = type;
#endif
	switch(type)
	{
		case eDisable:
#if 0
			gVportRegBx->vbi_004.reg_vbi_en = VBI_OFF;
#else
			VPORT_RdFL(vbi_004);
			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_OFF);
			VPORT_WrFL(vbi_004);
#endif

		break;
		case eClosedCaptionUS:
		case eClosedCaptionEURO:
#if 0
			value = 0;
			memcpy((void *)&gVportRegBx->vbi_007 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_008 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_009 , (void *)&value , sizeof(int) );
			value = 0x11;
			memcpy((void *)&gVportRegBx->vbi_010 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_011 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xE736;
			memcpy((void *)&gVportRegBx->vbi_006 , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54548282;
			memcpy((void *)&gVportRegBx->vbi_020 , (void *)&value , sizeof(int) );

			value = 0x35A012DB;
			memcpy((void *)&gVportRegBx->vbi_014 , (void *)&value , sizeof(int) );
			value = 0x2066159a;
			memcpy((void *)&gVportRegBx->vbi_015 , (void *)&value , sizeof(int) );
			gVportRegBx->vbi_016.reg_wss625_frame_start = 0x2;
			gVportRegBx->iris_041.reg_vsync_vbi_max = 0x25;
			gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
#else
			VPORT_RdFL(vbi_004);
			VPORT_RdFL(vbi_006);
			VPORT_RdFL(vbi_007);
			VPORT_RdFL(vbi_008);
			VPORT_RdFL(vbi_009);
			VPORT_RdFL(vbi_010);
			VPORT_RdFL(vbi_011);
			VPORT_RdFL(vbi_014);
			VPORT_RdFL(vbi_015);
			VPORT_RdFL(vbi_016);
			VPORT_RdFL(vbi_020);
			VPORT_RdFL(iris_041);

			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_ON);
			VPORT_Wr(vbi_006, 0xE736);
			VPORT_Wr(vbi_007, 0);
			VPORT_Wr(vbi_008, 0);
			VPORT_Wr(vbi_009, 0);
			VPORT_Wr(vbi_010, 0x11);
			VPORT_Wr(vbi_011, 0);
			VPORT_Wr(vbi_014, 0x35A012DB);
			VPORT_Wr(vbi_015, 0x2066159A);
			VPORT_Wr(vbi_020, 0x54548282);
			VPORT_Wr01(vbi_016, reg_wss625_frame_start, 0x2);
			VPORT_Wr01(iris_041, reg_vsync_vbi_max, 0x25);

			VPORT_WrFL(vbi_004);
			VPORT_WrFL(vbi_007);
			VPORT_WrFL(vbi_008);
			VPORT_WrFL(vbi_009);
			VPORT_WrFL(vbi_010);
			VPORT_WrFL(vbi_011);
			VPORT_WrFL(vbi_006);
			VPORT_WrFL(vbi_020);
			VPORT_WrFL(vbi_014);
			VPORT_WrFL(vbi_015);
			VPORT_WrFL(vbi_016);
			VPORT_WrFL(iris_041);
#endif

		break;
		case eVPS:
#if 0
			//gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_007 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_008 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_009 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_010 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_011 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0x2736;
			memcpy((void *)&gVportRegBx->vbi_006 , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & vbi_020
			value = 0x54545A82;
			memcpy((void *)&gVportRegBx->vbi_020 , (void *)&value , sizeof(int) );

			gVportRegBx->vbi_014.reg_teletext_dto = 0x12DB;
			gVportRegBx->vbi_015.reg_wss625_dto = 0x2066;
			gVportRegBx->vbi_016.reg_wss625_frame_start = 0x2;
			gVportRegBx->iris_041.reg_vsync_vbi_max = 0x25;
			gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
#else
			VPORT_RdFL(vbi_007);
			VPORT_Wr(vbi_007, 0);
			VPORT_WrFL(vbi_007);

			VPORT_RdFL(vbi_008);
			VPORT_Wr(vbi_008, 0);
			VPORT_WrFL(vbi_008);

			VPORT_RdFL(vbi_009);
			VPORT_Wr(vbi_009, 0);
			VPORT_WrFL(vbi_009);

			VPORT_RdFL(vbi_010);
			VPORT_Wr(vbi_010, 0);
			VPORT_WrFL(vbi_010);

			VPORT_RdFL(vbi_011);
			VPORT_Wr(vbi_011, 0);
			VPORT_WrFL(vbi_011);

			VPORT_RdFL(vbi_006);
			VPORT_Wr(vbi_006, 0x2736);
			VPORT_WrFL(vbi_006);

			VPORT_RdFL(vbi_020);
			VPORT_Wr(vbi_020, 0x54545A82);
			VPORT_WrFL(vbi_020);

			VPORT_RdFL(vbi_014);
			VPORT_Wr01(vbi_014, reg_teletext_dto, 0x12DB);
			VPORT_WrFL(vbi_014);

			VPORT_RdFL(vbi_015);
			VPORT_Wr01(vbi_015, reg_wss625_dto, 0x2066);
			VPORT_WrFL(vbi_015);

			VPORT_RdFL(vbi_016);
			VPORT_Wr01(vbi_016, reg_wss625_frame_start, 0x2);
			VPORT_WrFL(vbi_016);

			VPORT_RdFL(iris_041);
			VPORT_Wr01(iris_041, reg_vsync_vbi_max, 0x25);
			VPORT_WrFL(iris_041);

			VPORT_RdFL(vbi_004);
			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_ON);
			VPORT_WrFL(vbi_004);
#endif
		break;

		case eTTX625A:
#if 0
			//gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
			value = 0x55555555;
			memcpy((void *)&gVportRegBx->vbi_007 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_008 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_009 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_010 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_011 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0x2736;
			memcpy((void *)&gVportRegBx->vbi_006 , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & vbi_020
			value = 0x54545A82;
			memcpy((void *)&gVportRegBx->vbi_020 , (void *)&value , sizeof(int) );

			gVportRegBx->vbi_014.reg_teletext_dto = 0x12DB;
			gVportRegBx->vbi_015.reg_wss625_dto = 0x2066;
			gVportRegBx->vbi_016.reg_wss625_frame_start = 0x2;
			gVportRegBx->iris_041.reg_vsync_vbi_max = 0x25;
			gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
#else
			VPORT_RdFL(vbi_007);
			VPORT_Wr(vbi_007, 0x55555555);
			VPORT_WrFL(vbi_007);

			VPORT_RdFL(vbi_008);
			VPORT_Wr(vbi_008, 0x55555555);
			VPORT_WrFL(vbi_008);

			VPORT_RdFL(vbi_009);
			VPORT_Wr(vbi_009, 0x55555555);
			VPORT_WrFL(vbi_009);

			VPORT_RdFL(vbi_010);
			VPORT_Wr(vbi_010, 0x55555555);
			VPORT_WrFL(vbi_010);

			VPORT_RdFL(vbi_011);
			VPORT_Wr(vbi_011, 0);
			VPORT_WrFL(vbi_011);

			VPORT_RdFL(vbi_006);
			VPORT_Wr(vbi_006, 0x2736);
			VPORT_WrFL(vbi_006);

			VPORT_RdFL(vbi_020);
			VPORT_Wr(vbi_020, 0x54545A82);
			VPORT_WrFL(vbi_020);

			VPORT_RdFL(vbi_014);
			VPORT_Wr01(vbi_014, reg_teletext_dto, 0x12DB);
			VPORT_WrFL(vbi_014);

			VPORT_RdFL(vbi_015);
			VPORT_Wr01(vbi_015, reg_wss625_dto, 0x2066);
			VPORT_WrFL(vbi_015);

			VPORT_RdFL(vbi_016);
			VPORT_Wr01(vbi_016, reg_wss625_frame_start, 0x2);
			VPORT_WrFL(vbi_016);

			VPORT_RdFL(iris_041);
			VPORT_Wr01(iris_041, reg_vsync_vbi_max, 0x25);
			VPORT_WrFL(iris_041);

			VPORT_RdFL(vbi_004);
			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_ON);
			VPORT_WrFL(vbi_004);
#endif
		break;
		case eTTX625B:


		break;

		case eTTX625C:

		break;
		case eTTX625D:

		break;
		case eTTX525B:
#if 0
			gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
			value = 0x99999999;
			memcpy((void *)&gVportRegBx->vbi_007 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_008 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_009 , (void *)&value , sizeof(int) );
			value = 0x99999999;
			memcpy((void *)&gVportRegBx->vbi_010 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_011 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0x2736;
			memcpy((void *)&gVportRegBx->vbi_006 , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & vbi_020
			value = 0x54545A82;
			memcpy((void *)&gVportRegBx->vbi_020 , (void *)&value , sizeof(int) );

			gVportRegBx->vbi_014.reg_teletext_dto = 0x12DB;
			gVportRegBx->vbi_015.reg_wss625_dto = 0x2066;
			gVportRegBx->vbi_016.reg_wss625_frame_start = 0x2;
			gVportRegBx->iris_041.reg_vsync_vbi_max = 0x25;
#else
			VPORT_RdFL(vbi_004);
			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_ON);
			VPORT_WrFL(vbi_004);

			VPORT_RdFL(vbi_007);
			VPORT_Wr(vbi_007, 0x99999999);
			VPORT_WrFL(vbi_007);

			VPORT_RdFL(vbi_008);
			VPORT_Wr(vbi_008, 0x99999999);
			VPORT_WrFL(vbi_008);

			VPORT_RdFL(vbi_009);
			VPORT_Wr(vbi_009, 0x99999999);
			VPORT_WrFL(vbi_009);

			VPORT_RdFL(vbi_010);
			VPORT_Wr(vbi_010, 0x99999999);
			VPORT_WrFL(vbi_010);

			VPORT_RdFL(vbi_011);
			VPORT_Wr(vbi_011, 0);
			VPORT_WrFL(vbi_011);

			VPORT_RdFL(vbi_006);
			VPORT_Wr(vbi_006, 0x2736);
			VPORT_WrFL(vbi_006);

			VPORT_RdFL(vbi_020);
			VPORT_Wr(vbi_020, 0x54545A82);
			VPORT_WrFL(vbi_020);

			VPORT_RdFL(vbi_014);
			VPORT_Wr01(vbi_014, reg_teletext_dto, 0x12DB);
			VPORT_WrFL(vbi_014);

			VPORT_RdFL(vbi_015);
			VPORT_Wr01(vbi_015, reg_wss625_dto, 0x2066);
			VPORT_WrFL(vbi_015);

			VPORT_RdFL(vbi_016);
			VPORT_Wr01(vbi_016, reg_wss625_frame_start, 0x2);
			VPORT_WrFL(vbi_016);

			VPORT_RdFL(iris_041);
			VPORT_Wr01(iris_041, reg_vsync_vbi_max, 0x25);
			VPORT_WrFL(iris_041);

#endif
		break;

		case eTTX525C:
#if 0

			gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
			value = 0xAAAAAAAA;
			memcpy((void *)&gVportRegBx->vbi_007 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_008 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_009 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_010 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_011 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xE736;
			memcpy((void *)&gVportRegBx->vbi_006 , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & vbi_020
			value = 0x54546482;
			memcpy((void *)&gVportRegBx->vbi_020 , (void *)&value , sizeof(int) );

			gVportRegBx->vbi_014.reg_teletext_dto = 0x12DB;
			gVportRegBx->vbi_015.reg_wss625_dto = 0x2066;
			gVportRegBx->vbi_016.reg_wss625_frame_start = 0x2;
			gVportRegBx->iris_041.reg_vsync_vbi_max = 0x23;
#else
			VPORT_RdFL(vbi_004);
			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_ON);
			VPORT_WrFL(vbi_004);

			VPORT_RdFL(vbi_007);
			VPORT_Wr(vbi_007, 0xAAAAAAAA);
			VPORT_WrFL(vbi_007);

			VPORT_RdFL(vbi_008);
			VPORT_Wr(vbi_008, 0xAAAAAAAA);
			VPORT_WrFL(vbi_008);

			VPORT_RdFL(vbi_009);
			VPORT_Wr(vbi_009, 0xAAAAAAAA);
			VPORT_WrFL(vbi_009);

			VPORT_RdFL(vbi_010);
			VPORT_Wr(vbi_010, 0xAAAAAAAA);
			VPORT_WrFL(vbi_010);

			VPORT_RdFL(vbi_011);
			VPORT_Wr(vbi_011, 0);
			VPORT_WrFL(vbi_011);

			VPORT_RdFL(vbi_006);
			VPORT_Wr(vbi_006, 0xE736);
			VPORT_WrFL(vbi_006);

			VPORT_RdFL(vbi_020);
			VPORT_Wr(vbi_020, 0x54546482);
			VPORT_WrFL(vbi_020);

			VPORT_RdFL(vbi_014);
			VPORT_Wr01(vbi_014, reg_teletext_dto, 0x12DB);
			VPORT_WrFL(vbi_014);

			VPORT_RdFL(vbi_015);
			VPORT_Wr01(vbi_015, reg_wss625_dto, 0x2066);
			VPORT_WrFL(vbi_015);

			VPORT_RdFL(vbi_016);
			VPORT_Wr01(vbi_016, reg_wss625_frame_start, 0x2);
			VPORT_WrFL(vbi_016);

			VPORT_RdFL(iris_041);
			VPORT_Wr01(iris_041, reg_vsync_vbi_max, 0x23);
			VPORT_WrFL(iris_041);
#endif
		break;
		case eTTX525D:
#if 0
			gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
			value = 0xBBBBBBBB;
			memcpy((void *)&gVportRegBx->vbi_007 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_008 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_009 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_010 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_011 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xA736;
			memcpy((void *)&gVportRegBx->vbi_006 , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & vbi_020
			value = 0x54545A82;
			memcpy((void *)&gVportRegBx->vbi_020 , (void *)&value , sizeof(int) );

			gVportRegBx->vbi_014.reg_teletext_dto = 0x12DB;
			gVportRegBx->vbi_015.reg_wss625_dto = 0x2066;
			gVportRegBx->vbi_016.reg_wss625_frame_start = 0x2;
			gVportRegBx->iris_041.reg_vsync_vbi_max = 0x23;
#else
			VPORT_RdFL(vbi_004);
			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_ON);
			VPORT_WrFL(vbi_004);

			VPORT_RdFL(vbi_007);
			VPORT_Wr(vbi_007, 0xBBBBBBBB);
			VPORT_WrFL(vbi_007);

			VPORT_RdFL(vbi_008);
			VPORT_Wr(vbi_008, 0xBBBBBBBB);
			VPORT_WrFL(vbi_008);

			VPORT_RdFL(vbi_009);
			VPORT_Wr(vbi_009, 0xBBBBBBBB);
			VPORT_WrFL(vbi_009);

			VPORT_RdFL(vbi_010);
			VPORT_Wr(vbi_010, 0xBBBBBBBB);
			VPORT_WrFL(vbi_010);

			VPORT_RdFL(vbi_011);
			VPORT_Wr(vbi_011, 0);
			VPORT_WrFL(vbi_011);

			VPORT_RdFL(vbi_006);
			VPORT_Wr(vbi_006, 0xA736);
			VPORT_WrFL(vbi_006);

			VPORT_RdFL(vbi_020);
			VPORT_Wr(vbi_020, 0x54545A82);
			VPORT_WrFL(vbi_020);

			VPORT_RdFL(vbi_014);
			VPORT_Wr01(vbi_014, reg_teletext_dto, 0x12DB);
			VPORT_WrFL(vbi_014);

			VPORT_RdFL(vbi_015);
			VPORT_Wr01(vbi_015, reg_wss625_dto, 0x2066);
			VPORT_WrFL(vbi_015);

			VPORT_RdFL(vbi_016);
			VPORT_Wr01(vbi_016, reg_wss625_frame_start, 0x2);
			VPORT_WrFL(vbi_016);

			VPORT_RdFL(iris_041);
			VPORT_Wr01(iris_041, reg_vsync_vbi_max, 0x23);
			VPORT_WrFL(iris_041);

#endif
		break;

		case eWSS625	:
#if 0
			gVportRegBx->vbi_004.reg_vbi_en = VBI_ON;
			value = 0x0;
			memcpy((void *)&gVportRegBx->vbi_007 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_008 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_009 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegBx->vbi_010 , (void *)&value , sizeof(int) );
			value = 0x00CC0000;;
			memcpy((void *)&gVportRegBx->vbi_011 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xE736;
			memcpy((void *)&gVportRegBx->vbi_006 , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & vbi_020
			value = 0x54828282;
			memcpy((void *)&gVportRegBx->vbi_020 , (void *)&value , sizeof(int) );

			gVportRegBx->vbi_014.reg_teletext_dto = 0x12DB;
			gVportRegBx->vbi_015.reg_wss625_dto = 0x1599;
			gVportRegBx->vbi_016.reg_wss625_frame_start = 0x2;
			gVportRegBx->iris_041.reg_vsync_vbi_max = 0x23;
#else
			VPORT_RdFL(vbi_004);
			VPORT_Wr01(vbi_004, reg_vbi_en, VBI_ON);
			VPORT_WrFL(vbi_004);

			VPORT_RdFL(vbi_007);
			VPORT_Wr(vbi_007, 0);
			VPORT_WrFL(vbi_007);

			VPORT_RdFL(vbi_008);
			VPORT_Wr(vbi_008, 0);
			VPORT_WrFL(vbi_008);

			VPORT_RdFL(vbi_009);
			VPORT_Wr(vbi_009, 0);
			VPORT_WrFL(vbi_009);

			VPORT_RdFL(vbi_010);
			VPORT_Wr(vbi_010, 0);
			VPORT_WrFL(vbi_010);

			VPORT_RdFL(vbi_011);
			VPORT_Wr(vbi_011, 0x00CC0000);
			VPORT_WrFL(vbi_011);

			VPORT_RdFL(vbi_006);
			VPORT_Wr(vbi_006, 0xE736);
			VPORT_WrFL(vbi_006);

			VPORT_RdFL(vbi_020);
			VPORT_Wr(vbi_020, 0x54828282);
			VPORT_WrFL(vbi_020);

			VPORT_RdFL(vbi_014);
			VPORT_Wr01(vbi_014, reg_teletext_dto, 0x12DB);
			VPORT_WrFL(vbi_014);

			VPORT_RdFL(vbi_015);
			VPORT_Wr01(vbi_015, reg_wss625_dto, 0x1599);
			VPORT_WrFL(vbi_015);

			VPORT_RdFL(vbi_016);
			VPORT_Wr01(vbi_016, reg_wss625_frame_start, 0x2);
			VPORT_WrFL(vbi_016);

			VPORT_RdFL(iris_041);
			VPORT_Wr01(iris_041, reg_vsync_vbi_max, 0x23);
			VPORT_WrFL(iris_041);
#endif
		break;
		case eWSSJ	:

		break;
		default:

		break;
	}

	CC_PRINT("%s done\n", __func__);
	return ret;

}


//#define NO_SHADOW
int CC_L8B0_SetInterruptClear(void)
{
	int ret = 0;
	UINT32 intr_vp1_en;

#if 0
	gVportRegBx->intr_vp1_clear.intr_vp1_clear |= (1<<15);	// interrupt clear
	gVportRegBx->intr_vp1_clear.intr_vp1_clear &= ~(1<<15);
#else
	VPORT_RdFL(intr_vp1_clear);
	intr_vp1_en = VPORT_Rd(intr_vp1_clear);
	VPORT_Wr(intr_vp1_clear, (intr_vp1_en |= (1<<15)));
	VPORT_WrFL(intr_vp1_clear);

	VPORT_RdFL(intr_vp1_clear);
	intr_vp1_en = VPORT_Rd(intr_vp1_clear);
	VPORT_Wr(intr_vp1_clear, (intr_vp1_en &= ~(1<<15)));
	VPORT_WrFL(intr_vp1_clear);
#endif

	return ret;

}


int CC_L8B0_SetInterrupt(int on_off)
{
	int ret = 0;
	UINT32 intr_vp1_en;

#ifdef	KDRV_CONFIG_PM
	gCcPmData->uiPmCcIntStatus = on_off;
#endif
	if(on_off)
	{

#if 0
		gVportRegBx->intr_vp1_enable.intr_vp1_enable |= (1<<15);
#else
		VPORT_RdFL(intr_vp1_enable);
		intr_vp1_en = VPORT_Rd(intr_vp1_enable);
		VPORT_Wr(intr_vp1_enable, (intr_vp1_en |= (1<<15)));
		VPORT_WrFL(intr_vp1_enable);
#endif

	}
	else{

#if 0
		gVportRegBx->intr_vp1_enable.intr_vp1_enable &= ~(1<<15);
#else
		VPORT_RdFL(intr_vp1_enable);
		intr_vp1_en = VPORT_Rd(intr_vp1_enable);
		VPORT_Wr(intr_vp1_enable, (intr_vp1_en &= ~(1<<15)));
		VPORT_WrFL(intr_vp1_enable);
#endif

		TTX_CC_SetInterruptClear();
	}

	return ret;
}

#undef NO_SHADOW

int CC_L8B0_ParsingVBIData(unsigned int *packet)
{
	int ret = 0;
	unsigned int start, end, count;
	sCloseCaption data = {0,};
	unsigned long addr;
	int buffer_number = 0;

	start = 0;
	end = 0;
	count = 0;

#if 0
	switch(gVportRegBx->vbi_ctrl_002.reg_vbi_buffer_number)
	{
		case 0:
			start = gVportRegBx->vbi_ctrl_002.reg_vbi0_start_addr;
			end = gVportRegBx->vbi_ctrl_003.reg_vbi0_end_addr;
			count = gVportRegBx->vbi_ctrl_002.reg_vbi0_data_cnt;
			break;
		case 1:
			start = gVportRegBx->vbi_ctrl_004.reg_vbi1_start_addr;
			end = gVportRegBx->vbi_ctrl_005.reg_vbi1_end_addr;
			count = gVportRegBx->vbi_ctrl_004.reg_vbi1_data_cnt;
			break;
		case 2:
			start = gVportRegBx->vbi_ctrl_006.reg_vbi2_start_addr;
			end = gVportRegBx->vbi_ctrl_007.reg_vbi2_end_addr;
			count = gVportRegBx->vbi_ctrl_006.reg_vbi2_data_cnt;
			break;
		case 3:
			start = gVportRegBx->vbi_ctrl_008.reg_vbi3_start_addr;
			end = gVportRegBx->vbi_ctrl_009.reg_vbi3_end_addr;
			count = gVportRegBx->vbi_ctrl_008.reg_vbi3_data_cnt;
			break;
		default:
			ret = -1;
			return ret;
			break;
	}
#else
	VPORT_RdFL(vbi_ctrl_002);
	VPORT_Rd01(vbi_ctrl_002, reg_vbi_buffer_number, buffer_number);

	switch(buffer_number)
	{
		case 0:
			VPORT_RdFL(vbi_ctrl_002);
			VPORT_RdFL(vbi_ctrl_003);

			VPORT_Rd01(vbi_ctrl_002, reg_vbi0_start_addr, start);
			VPORT_Rd01(vbi_ctrl_003, reg_vbi0_end_addr, end);
			VPORT_Rd01(vbi_ctrl_002, reg_vbi0_data_cnt, count);

			break;
		case 1:
			VPORT_RdFL(vbi_ctrl_004);
			VPORT_RdFL(vbi_ctrl_005);

			VPORT_Rd01(vbi_ctrl_004, reg_vbi1_start_addr, start);
			VPORT_Rd01(vbi_ctrl_005, reg_vbi1_end_addr, end);
			VPORT_Rd01(vbi_ctrl_004, reg_vbi1_data_cnt, count);

			break;
		case 2:
			VPORT_RdFL(vbi_ctrl_006);
			VPORT_RdFL(vbi_ctrl_007);

			VPORT_Rd01(vbi_ctrl_006, reg_vbi2_start_addr, start);
			VPORT_Rd01(vbi_ctrl_007, reg_vbi2_end_addr, end);
			VPORT_Rd01(vbi_ctrl_006, reg_vbi2_data_cnt, count);

			break;
		case 3:
			VPORT_RdFL(vbi_ctrl_008);
			VPORT_RdFL(vbi_ctrl_009);

			VPORT_Rd01(vbi_ctrl_008, reg_vbi3_start_addr, start);
			VPORT_Rd01(vbi_ctrl_009, reg_vbi3_end_addr, end);
			VPORT_Rd01(vbi_ctrl_008, reg_vbi3_data_cnt, count);

			break;
		default:
			ret = -1;
			return ret;
			break;
	}

#endif

	count = ((count+3)>>2)<<2;

	if( (start < 0x1000) && (end < 0x1000) && (count <= 4*5) )
	{
		if(end >= start)
			memcpy((void *)&data , (void *)(gCC_addr + start*4) , count);
		else
		{
#if 0
			CC_PRINT("=========================================================\n");
			CC_PRINT("addr %08x start %08x end %08x count %08x\n",(unsigned int)addr , start, end, count);
			CC_PRINT("=========================================================\n");
#endif
			if((end+0x400 - start) < 5)
			{
				addr = (unsigned long)&data;
				memcpy((void *)addr , (void *)(gCC_addr + start*4) , (0x400 - start)*4 );
				memcpy((void *)(addr + (0x400 - start)*4), (void *)(gCC_addr) , (end + 1)*4 );
			}
		}
	}
	else
	{
		CC_PRINT("ERROR : start[%d] end[%d]\n", start , end);
	}

	if(data.Preamble == CC_VBI_PREAMBLE)
	{
/*
		if( (data.Status & 0x0E) == 0 ) CC_PRINT("CloseCaption came\n");
		else if( (data.Status & 0x0E) == 8 ) CC_PRINT("WSS625 came\n");
		else if( (data.Status & 0x0E) == 10 ) CC_PRINT("WSSJ came\n");
		else CC_PRINT("strange type came\n");

		CC_PRINT("VBI line number [%d]\n" , data.LineCount);
*/
		*packet = (data.Status<<24)|((data.Status & 0x1)<<16)|(data.data1 << 8)|data.data0;
//		CC_PRINT("data came [%d]\n", cont++);

	}
	else
	{
		CC_PRINT("data came. but preamble was wrong[%x], start[%x], end[%x], count[%x]\n" , data.Preamble, start, end, count);
		*packet=0;
		ret = -1;
	}


//	CC_PRINT("start [%d] end [%d] count [%d] packet [%08x]\n", start , end , count , *packet);
//	TTX_CC_SetInterruptClear();
	return ret;
}

int CC_L8B0_DebugModeOnOffCCPrint(int mode)
{
	return 0;
}

#ifdef	KDRV_CONFIG_PM

int CC_L8B0_RunSuspend(void)
{
	// param will be saved automatically....
	return 0;
}

int CC_L8B0_RunResume(void)
{
	TTX_CC_SetInterrupt(gCcPmData->uiPmCcIntStatus);
	TTX_CC_SetInputFormat(gCcPmData->ePmCcModeStatus);
	return 0;
}

#endif

/** @} */
