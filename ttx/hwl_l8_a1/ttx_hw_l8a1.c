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
 *	TTX module control Drivers linux module
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version		1.0
 *  date
 *  note		Additional information.
 *
 *  @addtogroup lg1150_TTX
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
#include "ttx_data.h"
#include "ttx_hw.h"
#include "ttx_drv.h"

#include "../afe/vport_reg.h"

#ifdef	_WITHOUT_AFE_
volatile VPORT_Reg_T *gVportReg;	// <- prototype from AFE. because the
					//	registers for ADC, CVD, HDMI are in AFE modules
#else	//_WITHOUT_ADC_
extern volatile VPORT_RegAx_T *gVportRegAx;
#endif

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define	VBI_OFF	0
#define	VBI_ON	1
#define	BIT_REVERSE	0
/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
#define	VPS_DATA_LINE	16
#define	WSS_DATA_LINE	23
#define	DATA_OFFSET		2
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
static int guiPmIntStatus = 0;
static eVBIDataType guiPmTtxModeStatus = eTTX625B;
#endif
/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static volatile unsigned long gTTX_addr;

static unsigned char *ucPacket;

#if	BIT_REVERSE
static const unsigned char _aBitOrderConvTab[256] = {
0x00, /* 00h */	0x80, /* 01h */	0x40, /* 02h */	0xc0, /* 03h */
0x20, /* 04h */	0xa0, /* 05h */	0x60, /* 06h */	0xe0, /* 07h */
0x10, /* 08h */	0x90, /* 09h */	0x50, /* 0ah */	0xd0, /* 0bh */
0x30, /* 0ch */	0xb0, /* 0dh */	0x70, /* 0eh */	0xf0, /* 0fh */
0x08, /* 10h */	0x88, /* 11h */	0x48, /* 12h */	0xc8, /* 13h */
0x28, /* 14h */	0xa8, /* 15h */	0x68, /* 16h */	0xe8, /* 17h */
0x18, /* 18h */	0x98, /* 19h */	0x58, /* 1ah */	0xd8, /* 1bh */
0x38, /* 1ch */	0xb8, /* 1dh */	0x78, /* 1eh */	0xf8, /* 1fh */
0x04, /* 20h */	0x84, /* 21h */	0x44, /* 22h */	0xc4, /* 23h */
0x24, /* 24h */	0xa4, /* 25h */	0x64, /* 26h */	0xe4, /* 27h */
0x14, /* 28h */	0x94, /* 29h */	0x54, /* 2ah */	0xd4, /* 2bh */
0x34, /* 2ch */	0xb4, /* 2dh */	0x74, /* 2eh */	0xf4, /* 2fh */
0x0c, /* 30h */	0x8c, /* 31h */	0x4c, /* 32h */	0xcc, /* 33h */
0x2c, /* 34h */	0xac, /* 35h */	0x6c, /* 36h */	0xec, /* 37h */
0x1c, /* 38h */	0x9c, /* 39h */	0x5c, /* 3ah */	0xdc, /* 3bh */
0x3c, /* 3ch */	0xbc, /* 3dh */	0x7c, /* 3eh */	0xfc, /* 3fh */
0x02, /* 40h */	0x82, /* 41h */	0x42, /* 42h */	0xc2, /* 43h */
0x22, /* 44h */	0xa2, /* 45h */	0x62, /* 46h */	0xe2, /* 47h */
0x12, /* 48h */	0x92, /* 49h */	0x52, /* 4ah */	0xd2, /* 4bh */
0x32, /* 4ch */	0xb2, /* 4dh */	0x72, /* 4eh */	0xf2, /* 4fh */
0x0a, /* 50h */	0x8a, /* 51h */	0x4a, /* 52h */	0xca, /* 53h */
0x2a, /* 54h */	0xaa, /* 55h */	0x6a, /* 56h */	0xea, /* 57h */
0x1a, /* 58h */	0x9a, /* 59h */	0x5a, /* 5ah */	0xda, /* 5bh */
0x3a, /* 5ch */	0xba, /* 5dh */	0x7a, /* 5eh */	0xfa, /* 5fh */
0x06, /* 60h */	0x86, /* 61h */	0x46, /* 62h */	0xc6, /* 63h */
0x26, /* 64h */	0xa6, /* 65h */	0x66, /* 66h */	0xe6, /* 67h */
0x16, /* 68h */	0x96, /* 69h */	0x56, /* 6ah */	0xd6, /* 6bh */
0x36, /* 6ch */	0xb6, /* 6dh */	0x76, /* 6eh */	0xf6, /* 6fh */
0x0e, /* 70h */	0x8e, /* 71h */	0x4e, /* 72h */	0xce, /* 73h */
0x2e, /* 74h */	0xae, /* 75h */	0x6e, /* 76h */	0xee, /* 77h */
0x1e, /* 78h */	0x9e, /* 79h */	0x5e, /* 7ah */	0xde, /* 7bh */
0x3e, /* 7ch */	0xbe, /* 7dh */	0x7e, /* 7eh */	0xfe, /* 7fh */
0x01, /* 80h */	0x81, /* 81h */	0x41, /* 82h */	0xc1, /* 83h */
0x21, /* 84h */	0xa1, /* 85h */	0x61, /* 86h */	0xe1, /* 87h */
0x11, /* 88h */	0x91, /* 89h */	0x51, /* 8ah */	0xd1, /* 8bh */
0x31, /* 8ch */	0xb1, /* 8dh */	0x71, /* 8eh */	0xf1, /* 8fh */
0x09, /* 90h */	0x89, /* 91h */	0x49, /* 92h */	0xc9, /* 93h */
0x29, /* 94h */	0xa9, /* 95h */	0x69, /* 96h */	0xe9, /* 97h */
0x19, /* 98h */	0x99, /* 99h */	0x59, /* 9ah */	0xd9, /* 9bh */
0x39, /* 9ch */	0xb9, /* 9dh */	0x79, /* 9eh */	0xf9, /* 9fh */
0x05, /* a0h */	0x85, /* a1h */	0x45, /* a2h */	0xc5, /* a3h */
0x25, /* a4h */	0xa5, /* a5h */	0x65, /* a6h */	0xe5, /* a7h */
0x15, /* a8h */	0x95, /* a9h */	0x55, /* aah */	0xd5, /* abh */
0x35, /* ach */	0xb5, /* adh */	0x75, /* aeh */	0xf5, /* afh */
0x0d, /* b0h */	0x8d, /* b1h */	0x4d, /* b2h */	0xcd, /* b3h */
0x2d, /* b4h */	0xad, /* b5h */	0x6d, /* b6h */	0xed, /* b7h */
0x1d, /* b8h */	0x9d, /* b9h */	0x5d, /* bah */	0xdd, /* bbh */
0x3d, /* bch */	0xbd, /* bdh */	0x7d, /* beh */	0xfd, /* bfh */
0x03, /* c0h */	0x83, /* c1h */	0x43, /* c2h */	0xc3, /* c3h */
0x23, /* c4h */	0xa3, /* c5h */	0x63, /* c6h */	0xe3, /* c7h */
0x13, /* c8h */	0x93, /* c9h */	0x53, /* cah */	0xd3, /* cbh */
0x33, /* cch */	0xb3, /* cdh */	0x73, /* ceh */	0xf3, /* cfh */
0x0b, /* d0h */	0x8b, /* d1h */	0x4b, /* d2h */	0xcb, /* d3h */
0x2b, /* d4h */	0xab, /* d5h */	0x6b, /* d6h */	0xeb, /* d7h */
0x1b, /* d8h */	0x9b, /* d9h */	0x5b, /* dah */	0xdb, /* dbh */
0x3b, /* dch */	0xbb, /* ddh */	0x7b, /* deh */	0xfb, /* dfh */
0x07, /* e0h */	0x87, /* e1h */	0x47, /* e2h */	0xc7, /* e3h */
0x27, /* e4h */	0xa7, /* e5h */	0x67, /* e6h */	0xe7, /* e7h */
0x17, /* e8h */	0x97, /* e9h */	0x57, /* eah */	0xd7, /* ebh */
0x37, /* ech */	0xb7, /* edh */	0x77, /* eeh */	0xf7, /* efh */
0x0f, /* f0h */	0x8f, /* f1h */	0x4f, /* f2h */	0xcf, /* f3h */
0x2f, /* f4h */	0xaf, /* f5h */	0x6f, /* f6h */	0xef, /* f7h */
0x1f, /* f8h */	0x9f, /* f9h */	0x5f, /* fah */	0xdf, /* fbh */
0x3f, /* fch */	0xbf, /* fdh */	0x7f, /* feh */	0xff, /* ffh */
};
#else
static const unsigned char _aBitOrderConvTab[256] = {
 0x00  , 0x01  , 	0x02  	, 0x03  ,
 0x04  , 0x05  , 	0x06  	, 0x07  ,
 0x08  , 0x09  , 	0x0a  	, 0x0b  ,
 0x0c  , 0x0d  , 	0x0e  	, 0x0f  ,
 0x10  , 0x11  , 	0x12  	, 0x13  ,
 0x14  , 0x15  , 	0x16  	, 0x17  ,
 0x18  , 0x19  , 	0x1a  	, 0x1b  ,
 0x1c  , 0x1d  , 	0x1e  	, 0x1f  ,
 0x20  , 0x21  , 	0x22  	, 0x23  ,
 0x24  , 0x25  , 	0x26  	, 0x27  ,
 0x28  , 0x29  , 	0x2a  	, 0x2b  ,
 0x2c  , 0x2d  , 	0x2e  	, 0x2f  ,
 0x30  , 0x31  , 	0x32  	, 0x33  ,
 0x34  , 0x35  , 	0x36  	, 0x37  ,
 0x38  , 0x39  , 	0x3a  	, 0x3b  ,
 0x3c  , 0x3d  , 	0x3e  	, 0x3f  ,
 0x40  , 0x41  , 	0x42  	, 0x43  ,
 0x44  , 0x45  , 	0x46  	, 0x47  ,
 0x48  , 0x49  , 	0x4a  	, 0x4b  ,
 0x4c  , 0x4d  , 	0x4e  	, 0x4f  ,
 0x50  , 0x51  , 	0x52  	, 0x53  ,
 0x54  , 0x55  , 	0x56  	, 0x57  ,
 0x58  , 0x59  , 	0x5a  	, 0x5b  ,
 0x5c  , 0x5d  , 	0x5e  	, 0x5f  ,
 0x60  , 0x61  , 	0x62  	, 0x63  ,
 0x64  , 0x65  , 	0x66  	, 0x67  ,
 0x68  , 0x69  , 	0x6a  	, 0x6b  ,
 0x6c  , 0x6d  , 	0x6e  	, 0x6f  ,
 0x70  , 0x71  , 	0x72  	, 0x73  ,
 0x74  , 0x75  , 	0x76  	, 0x77  ,
 0x78  , 0x79  , 	0x7a  	, 0x7b  ,
 0x7c  , 0x7d  , 	0x7e  	, 0x7f  ,
 0x80  , 0x81  , 	0x82  	, 0x83  ,
 0x84  , 0x85  , 	0x86  	, 0x87  ,
 0x88  , 0x89  , 	0x8a  	, 0x8b  ,
 0x8c  , 0x8d  , 	0x8e  	, 0x8f  ,
 0x90  , 0x91  , 	0x92  	, 0x93  ,
 0x94  , 0x95  , 	0x96  	, 0x97  ,
 0x98  , 0x99  , 	0x9a  	, 0x9b  ,
 0x9c  , 0x9d  , 	0x9e  	, 0x9f  ,
 0xa0  , 0xa1  , 	0xa2  	, 0xa3  ,
 0xa4  , 0xa5  , 	0xa6  	, 0xa7  ,
 0xa8  , 0xa9  , 	0xaa  	, 0xab  ,
 0xac  , 0xad  , 	0xae  	, 0xaf  ,
 0xb0  , 0xb1  , 	0xb2  	, 0xb3  ,
 0xb4  , 0xb5  , 	0xb6  	, 0xb7  ,
 0xb8  , 0xb9  , 	0xba  	, 0xbb  ,
 0xbc  , 0xbd  , 	0xbe  	, 0xbf  ,
 0xc0  , 0xc1  , 	0xc2  	, 0xc3  ,
 0xc4  , 0xc5  , 	0xc6  	, 0xc7  ,
 0xc8  , 0xc9  , 	0xca  	, 0xcb  ,
 0xcc  , 0xcd  , 	0xce  	, 0xcf  ,
 0xd0  , 0xd1  , 	0xd2  	, 0xd3  ,
 0xd4  , 0xd5  , 	0xd6  	, 0xd7  ,
 0xd8  , 0xd9  , 	0xda  	, 0xdb  ,
 0xdc  , 0xdd  , 	0xde  	, 0xdf  ,
 0xe0  , 0xe1  , 	0xe2  	, 0xe3  ,
 0xe4  , 0xe5  , 	0xe6  	, 0xe7  ,
 0xe8  , 0xe9  , 	0xea  	, 0xeb  ,
 0xec  , 0xed  , 	0xee  	, 0xef  ,
 0xf0  , 0xf1  , 	0xf2  	, 0xf3  ,
 0xf4  , 0xf5  , 	0xf6  	, 0xf7  ,
 0xf8  , 0xf9  , 	0xfa  	, 0xfb  ,
 0xfc  , 0xfd  , 	0xfe  	, 0xff
};
#endif
/*========================================================================================
	Implementation Group
========================================================================================*/

int TTX_HWInitialize_L8Ax(void)
{
	int ret = 0;

	gTTX_addr = (volatile unsigned long)ioremap(	gstRegCfgTTX->vbi_register_base , 	gstRegCfgTTX->vbi_register_size);

	ucPacket = (unsigned char *)kmalloc( (TTX_MAX_DATA + 7) * MAX_PACKET_NUM , GFP_KERNEL);

	TTX_PRINT("%s done\n", __func__);
	return ret;
}

int TTX_HWUninitialize_L8Ax(void)
{
	int ret = 0;

	iounmap((void *)gTTX_addr);

	kfree(ucPacket);

	return ret;
}

int TTX_SetInputFormat_L8Ax(eVBIDataType type)
{
	int ret = 0;
	unsigned int value;

	TTX_PRINT("%s input : %d\n" , __func__ , type);

#ifdef	KDRV_CONFIG_PM
	guiPmTtxModeStatus = type;
#endif

	switch(type)
	{
		case eDisable:
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_OFF;

		break;
		case eClosedCaptionUS:
		case eClosedCaptionEURO:

			value = 0;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			value = 0x11;
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xE736;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54548282;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );

			value = 0x35A012DB;
			memcpy((void *)&gVportRegAx->reg_caption_dto , (void *)&value , sizeof(int) );
			value = 0x2066159a;
			memcpy((void *)&gVportRegAx->reg_wss625_dto , (void *)&value , sizeof(int) );
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x25;
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
		break;
		case eVPS:
			//gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0x2736;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54545A82;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );


			value = 0x35A012DB;
			memcpy((void *)&gVportRegAx->reg_caption_dto , (void *)&value , sizeof(int) );
			value = 0x2066159a;
			memcpy((void *)&gVportRegAx->reg_wss625_dto , (void *)&value , sizeof(int) );
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x25;
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;


		break;

		case eTTX625A:
			//gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
			value = 0x55555555;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0x2736;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54545A82;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );

			gVportRegAx->reg_caption_dto.reg_teletext_dto = 0x12DB;
			gVportRegAx->reg_wss625_dto.reg_wss625_dto = 0x2066;
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x25;
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
		break;
		case eTTX625B:
			value = 0x66666666;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			value = 0x66663366;
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			value = 0x66666666; //0x66666666
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x66cc0000;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0x2736;
			//value = 0xe436;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54825a5a;//0x54545a82;//0x54546482;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );

			value = 0x35A00F91;
			memcpy((void *)&gVportRegAx->reg_caption_dto , (void *)&value , sizeof(int) );
			value = 0x2066159a;
			memcpy((void *)&gVportRegAx->reg_wss625_dto , (void *)&value , sizeof(int) );
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_caption_frame_start.reg_teletext_frame_start = 0x3;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x25;
			gVportRegAx->reg_vbi_en.reg_vbi_mu_out = 0;
			gVportRegAx->reg_vbi_en.reg_vbi_hsyncout = 0;
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;

		break;

		case eTTX625C:

		break;
		case eTTX625D:

		break;
		case eTTX525B:
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
			value = 0x99999999;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			value = 0x99999999;
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0x2736;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54545A82;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );

			gVportRegAx->reg_caption_dto.reg_teletext_dto = 0x12DB;
			gVportRegAx->reg_wss625_dto.reg_wss625_dto = 0x2066;
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x25;
		break;

		case eTTX525C:
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
			value = 0xAAAAAAAA;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xE736;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54546482;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );

			gVportRegAx->reg_caption_dto.reg_teletext_dto = 0x12DB;
			gVportRegAx->reg_wss625_dto.reg_wss625_dto = 0x2066;
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x23;
		break;
		case eTTX525D:
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
			value = 0xBBBBBBBB;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xA736;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54545A82;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );

			gVportRegAx->reg_caption_dto.reg_teletext_dto = 0x12DB;
			gVportRegAx->reg_wss625_dto.reg_wss625_dto = 0x2066;
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x23;
		break;

		case eWSS625	:
			gVportRegAx->reg_vbi_en.reg_vbi_en = VBI_ON;
			value = 0x0;
			memcpy((void *)&gVportRegAx->reg_vbil6 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil10 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil14 , (void *)&value , sizeof(int) );
			memcpy((void *)&gVportRegAx->reg_vbil18 , (void *)&value , sizeof(int) );
			value = 0x00CC0000;;
			memcpy((void *)&gVportRegAx->reg_vbil22 , (void *)&value , sizeof(int) );

			// vbi_data_hlvl & start_code
			value = 0xE736;
			memcpy((void *)&gVportRegAx->reg_start_code , (void *)&value , sizeof(int) );
			// reg_vps_start & reg_teletext_start & reg_wss625_start & reg_caption_start
			value = 0x54828282;
			memcpy((void *)&gVportRegAx->reg_caption_start , (void *)&value , sizeof(int) );

			gVportRegAx->reg_caption_dto.reg_teletext_dto = 0x12DB;
			gVportRegAx->reg_wss625_dto.reg_wss625_dto = 0x1599;
			gVportRegAx->reg_caption_frame_start.reg_wss625_frame_start = 0x2;
			gVportRegAx->reg_vsync_vbi_min.reg_vsync_vbi_max = 0x23;
		break;
		case eWSSJ	:

		break;
		default:

		break;
	}

	TTX_PRINT("%s done\n", __func__);
	return ret;

}


int TTX_SetInterruptClear_L8Ax(void)
{
	int ret = 0;

	gVportRegAx->intr_vp1_clear.intr_vp1_clear |= (1<<15);	// interrupt clear
	gVportRegAx->intr_vp1_clear.intr_vp1_clear &= ~(1<<15);

	return ret;

}


int TTX_SetInterrupt_L8Ax(int on_off)
{
	int ret = 0;

#ifdef	KDRV_CONFIG_PM
	guiPmIntStatus = on_off;
#endif

	if(on_off)
		gVportRegAx->intr_vp1_enable.intr_vp1_enable |= (1<<15);
	else{
		gVportRegAx->intr_vp1_enable.intr_vp1_enable &= ~(1<<15);
		TTX_SetInterruptClear();
	}

	return ret;
}


int TTX_ParsingVBIData_L8Ax(sHeaderData *data )
{
	int ret = 0;
	unsigned long middle_addr;
	int iHeaderPos;
	int i;
#if 0
	sHeaderPacket packet;
	sTTXPacket packet;
#endif



	switch(gVportRegAx->reg_vbi_buffer_number.reg_vbi_buffer_number)
	{
		case 0:
			data->start = gVportRegAx->reg_vbi_buffer_number.reg_vbi0_start_addr;
			data->end = gVportRegAx->reg_vbi0_end_addr.reg_vbi0_end_addr;
			data->count = gVportRegAx->reg_vbi_buffer_number.reg_vbi0_data_cnt;
		break;
		case 1:
			data->start = gVportRegAx->reg_vbi1_data_cnt.reg_vbi1_start_addr;
			data->end = gVportRegAx->reg_vbi1_end_addr.reg_vbi1_end_addr;
			data->count = gVportRegAx->reg_vbi1_data_cnt.reg_vbi1_data_cnt;
		break;
		case 2:
			data->start = gVportRegAx->reg_vbi2_data_cnt.reg_vbi2_start_addr;
			data->end = gVportRegAx->reg_vbi2_end_addr.reg_vbi2_end_addr;
			data->count = gVportRegAx->reg_vbi2_data_cnt.reg_vbi2_data_cnt;
		break;
		case 3:
			data->start = gVportRegAx->reg_vbi3_data_cnt.reg_vbi3_start_addr;
			data->end = gVportRegAx->reg_vbi3_end_addr.reg_vbi3_end_addr;
			data->count = gVportRegAx->reg_vbi3_data_cnt.reg_vbi3_data_cnt;
		break;
	}
//	TTX_PRINT("TTX end %d start %d \n", data->end , data->start );

	if(data->count > (TTX_MAX_DATA + 7) * MAX_PACKET_NUM)
	{
		TTX_PRINT("came too long packet [%d]\n", data->count);
		ret = -1;
	}
	else
	{
		unsigned int *uiData;
		data->data_num = 0;

		// all data copy to DDR. because our system can't access as BYTE
#if 0
		if( data->end > data->start )
			memcpy((void *)ucPacket , (void *)(gTTX_addr + data->start*4) , sizeof(int)*(data->end - data->start + 1));
		else
		{
			middle_addr = 0x400 - data->start;
			memcpy((void *)ucPacket , (void *)(gTTX_addr + data->start*4) , sizeof(int)*middle_addr );
			memcpy((void *)(ucPacket + middle_addr*4), (void *)(gTTX_addr) , sizeof(int)*(data->end + 1) );
		}
#else

//TTX_PRINT("start [%d] end [%d]\n", data->start , data->end);
		if( data->end > data->start )
		{
			for(i = 0; i < (data->end - data->start + 1) ; i++)
			{
				uiData = (unsigned int *)(gTTX_addr + (data->start + i)*4);

				*(ucPacket + i*4 + 0) = _aBitOrderConvTab[(unsigned char)( (*uiData>>24)&0xff )];
				*(ucPacket + i*4 + 1) = _aBitOrderConvTab[(unsigned char)( (*uiData>>16)&0xff )];
				*(ucPacket + i*4 + 2) = _aBitOrderConvTab[(unsigned char)( (*uiData>>8)&0xff )];
				*(ucPacket + i*4 + 3) = _aBitOrderConvTab[(unsigned char)( (*uiData)&0xff )];
			}
			//memcpy((void *)ucPacket , (void *)(gTTX_addr + data->start*4) , sizeof(int)*(data->end - data->start + 1));

		}
		else
		{
			middle_addr = 0x400 - data->start;

			for(i = 0; i < (middle_addr) ; i++)
			{
				uiData = (unsigned int *)(gTTX_addr + (data->start + i)*4);

				*(ucPacket + i*4 + 0) = _aBitOrderConvTab[(unsigned char)( (*uiData>>24)&0xff )];
				*(ucPacket + i*4 + 1) = _aBitOrderConvTab[(unsigned char)( (*uiData>>16)&0xff )];
				*(ucPacket + i*4 + 2) = _aBitOrderConvTab[(unsigned char)( (*uiData>>8)&0xff )];
				*(ucPacket + i*4 + 3) = _aBitOrderConvTab[(unsigned char)( (*uiData)&0xff )];
			}

			for(i = 0; i < (data->end + 1) ; i++)
			{
				uiData = (unsigned int *)(gTTX_addr + i*4);

				*(ucPacket + (i + middle_addr)*4 + 0) = _aBitOrderConvTab[(unsigned char)( (*uiData>>24)&0xff )];
				*(ucPacket + (i + middle_addr)*4 + 1) = _aBitOrderConvTab[(unsigned char)( (*uiData>>16)&0xff )];
				*(ucPacket + (i + middle_addr)*4 + 2) = _aBitOrderConvTab[(unsigned char)( (*uiData>>8)&0xff )];
				*(ucPacket + (i + middle_addr)*4 + 3) = _aBitOrderConvTab[(unsigned char)( (*uiData)&0xff )];
			}
		}

		iHeaderPos = 0;
//TTX_PRINT(" parsing start %p\n" ,ucPacket);
		for(i=0;i<MAX_PACKET_NUM;i++)
		{
		//	if( ( (_aBitOrderConvTab[*(ucPacket + iHeaderPos + 3)]&0xf0) == 0x20 ) && ( _aBitOrderConvTab[*(ucPacket + iHeaderPos + 1)] == 0xff) && (_aBitOrderConvTab[*(ucPacket+iHeaderPos+2)] == 0xff))

			// seraching data position. and deliver the position to PutVBIData.
			if( ( _aBitOrderConvTab[*(ucPacket + iHeaderPos + 1)] == 0xff) && (_aBitOrderConvTab[*(ucPacket+iHeaderPos+2)] == 0xff))
			{
				data->data_start[data->data_num] = iHeaderPos + 7;
				data->data_end[data->data_num] = iHeaderPos + 7 + _aBitOrderConvTab[*(ucPacket + iHeaderPos + 4)];
				data->data_num++;
			//	TTX_PRINT(" data length %x , status %x\n", _aBitOrderConvTab[*(ucPacket + iHeaderPos + 4)] , _aBitOrderConvTab[*(ucPacket + iHeaderPos + 3)]);
				iHeaderPos = iHeaderPos + 7 + _aBitOrderConvTab[*(ucPacket + iHeaderPos + 4)];

			}
			else
			{
		//		TTX_PRINT(" 1:%x , 2:%x , 3:%x \n", (*(ucPacket + iHeaderPos + 3)&0xff), *(ucPacket + iHeaderPos + 1),*(ucPacket + iHeaderPos + 2) );
				iHeaderPos = iHeaderPos + 7 + _aBitOrderConvTab[*(ucPacket + iHeaderPos + 4)];
			}
			//TTX_PRINT("data count %d , header %d\n", data->count , iHeaderPos);
			if( data->count <= iHeaderPos + 1) break;

		}

	}



#endif

	return ret;
}

int TTX_PutVBIData_L8Ax(sHeaderData *data)
{
	int ret = 0;
	int i;
	int offset = 3;
	unsigned long ptr = 0;
	unsigned int data_size = 0;

#if 0
	unsigned long middle_addr;

	if( data->end > data->start )
		memcpy((void *)data->addr , (void *)(gTTX_addr + data->start*4) , sizeof(int)*(data->end - data->start + 1));
	else
	{
		middle_addr = 0x400 - data->start;
		memcpy((void *)data->addr , (void *)(gTTX_addr + data->start*4) , middle_addr*4 );
		memcpy((void *)(data->addr + middle_addr*4), (void *)(gTTX_addr) , (data->end + 1)*4 );
	}
#else
//TTX_PRINT("save start\n");
	for(i = 0 ; i < data->data_num ; i++)
	{

		data->type = _aBitOrderConvTab[*(ucPacket+offset)] & 0x0E;

		TTX_GetDataAddr(data);
/*
		if(data->type != 6)	// debug mode
		{
			TTX_PRINT("type[%d] type[%d] end[%d] start[%d] \n", data->type , *(ucPacket+offset) ,data->data_end[i] , data->data_start[i] );

		}
*/

		data_size = data->data_end[i] - data->data_start[i];
		switch(data->type)
		{
			case 0x6: // teletext
				ptr = 6;
			break;

			case 0x8:	// WSS
				ptr = 0;
//				TTX_PRINT("WSS: [%x] [%x]  [%d] [%d]\n" , data->addr , data->data_end[i]-data->data_start[i] , *(ucPacket + data->data_start[i]) , *(ucPacket + data->data_start[i]-2));
				if(*(ucPacket +  data->data_start[i]-DATA_OFFSET) != WSS_DATA_LINE)
				{
					continue;
				}
				if(data_size > 0xd) data_size = 0xd;
			break;
			case 0xc:	// VPS
				ptr = 2;
//				TTX_PRINT("VPS: [%x] [%x]  [%d] [%d]\n" , data->addr ,  data->data_end[i]-data->data_start[i] , *(ucPacket + data->data_start[i]) , *(ucPacket + data->data_start[i]+1));
				if(*(ucPacket +  data->data_start[i]-DATA_OFFSET) != VPS_DATA_LINE)
				{
					continue;
				}
				if(data_size > 0xd) data_size = 0xd;

			break;
			default:
				ptr = 0;
			break;

		}
		if(data->addr != 0)
			memcpy( (void *)(data->addr + ptr) , (void *)(ucPacket + data->data_start[i]) , data_size);

		offset = data->data_end[i] + 3;
//		TTX_PRINT("offset [%d] end [%d]\n" , offset , data->data_end[i]);

	}
#endif
	return ret;
}

int TTX_SetVBILPF_L8Ax(int onoff)
{
	return 0;
}

int TTX_CheckOnLine318_L8Ax(int mode)
{
	return 0;
}

int TTX_SetTypeLine318_L8Ax(int onoff)
{
	return 0;
}

#ifdef	KDRV_CONFIG_PM

int TTX_RunSuspend_L8Ax(void)
{
	// param will be saved automatically....
	return 0;
}

int TTX_RunResume_L8Ax(void)
{
	TTX_SetInterrupt(guiPmIntStatus);
	TTX_SetInputFormat(guiPmTtxModeStatus);
	return 0;
}

#endif

/** @} */
