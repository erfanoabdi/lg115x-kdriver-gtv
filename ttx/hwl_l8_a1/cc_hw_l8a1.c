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

#include "../../afe/vport_reg.h"
//#include "vport_regAx.h"

#ifdef	_WITHOUT_AFE_
volatile VPORT_Reg_T *gVportReg;	// <- prototype from AFE. because the 
					//	registers for ADC, CVD, HDMI are in AFE modules
#else	//_WITHOUT_ADC_
//extern volatile VPORT_Reg_T *gVportReg;
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

int CC_L8Ax_HWInitialize(void)
{
	int ret = 0;

	gCC_addr = (volatile unsigned long)ioremap(gstRegCfgTTX->vbi_register_base , gstRegCfgTTX->vbi_register_size);

#ifdef	KDRV_CONFIG_PM
	gCcPmData = (S_CC_PM_DATA	*)kmalloc(sizeof(S_CC_PM_DATA) , GFP_KERNEL);
	gCcPmData->uiPmCcIntStatus = 0;
	gCcPmData->ePmCcModeStatus = 0;
#endif

	CC_PRINT("%s done\n", __func__);
	return ret;
}

int CC_L8Ax_HWUninitialize(void)
{
	int ret = 0;

	iounmap((void *)gCC_addr);

	return ret;
}

int CC_L8Ax_SetInputFormat(eVBIDataType type)
{
	int ret = 0;
	unsigned int value;

	CC_PRINT("%s input : %d\n" , __func__ , type);
#ifdef	KDRV_CONFIG_PM
	gCcPmData->ePmCcModeStatus = type;
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

			gVportRegAx->reg_caption_dto.reg_teletext_dto = 0x12DB;
			gVportRegAx->reg_wss625_dto.reg_wss625_dto = 0x2066;
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

	CC_PRINT("%s done\n", __func__);
	return ret;

}


int CC_L8Ax_SetInterruptClear(void)
{
	int ret = 0;

	gVportRegAx->intr_vp1_clear.intr_vp1_clear |= (1<<15);	// interrupt clear
	gVportRegAx->intr_vp1_clear.intr_vp1_clear &= ~(1<<15);

	return ret;

}


int CC_L8Ax_SetInterrupt(int on_off)
{
	int ret = 0;

#ifdef	KDRV_CONFIG_PM
	gCcPmData->uiPmCcIntStatus = on_off;
#endif
	if(on_off)
		gVportRegAx->intr_vp1_enable.intr_vp1_enable |= (1<<15);
	else{
		gVportRegAx->intr_vp1_enable.intr_vp1_enable &= ~(1<<15);
		TTX_CC_SetInterruptClear();
	}

	return ret;
}


int CC_L8Ax_ParsingVBIData(unsigned int *packet)
{
	int ret = 0;
	unsigned int start, end, count;
	sCloseCaption data = {0,};
	unsigned long addr;

	start = 0;
	end = 0;
	count = 0;

	switch(gVportRegAx->reg_vbi_buffer_number.reg_vbi_buffer_number)
	{
		case 0:
			start = gVportRegAx->reg_vbi_buffer_number.reg_vbi0_start_addr;
			end = gVportRegAx->reg_vbi0_end_addr.reg_vbi0_end_addr;
			count = gVportRegAx->reg_vbi_buffer_number.reg_vbi0_data_cnt;
		break;
		case 1:
			start = gVportRegAx->reg_vbi1_data_cnt.reg_vbi1_start_addr;
			end = gVportRegAx->reg_vbi1_end_addr.reg_vbi1_end_addr;
			count = gVportRegAx->reg_vbi1_data_cnt.reg_vbi1_data_cnt;
		break;
		case 2:
			start = gVportRegAx->reg_vbi2_data_cnt.reg_vbi2_start_addr;
			end = gVportRegAx->reg_vbi2_end_addr.reg_vbi2_end_addr;
			count = gVportRegAx->reg_vbi2_data_cnt.reg_vbi2_data_cnt;
		break;
		case 3:
			start = gVportRegAx->reg_vbi3_data_cnt.reg_vbi3_start_addr;
			end = gVportRegAx->reg_vbi3_end_addr.reg_vbi3_end_addr;
			count = gVportRegAx->reg_vbi3_data_cnt.reg_vbi3_data_cnt;
		break;
		default:
			ret = -1;
			return ret;
		break;
	}

	count = ((count+3)>>2)<<2;

	if( (start < 0x1000) && (end < 0x1000) && (count <= 4*5))
	{
		if(end >= start)
			memcpy((void *)&data , (void *)(gCC_addr + start*4) , count);
		else
		{

				addr = (unsigned long)&data;
				memcpy((void *)addr , (void *)(gCC_addr + start*4) , (0x400 - start)*4 );
				memcpy((void *)(addr + (0x400 - start)*4), (void *)(gCC_addr) , (end + 1)*4 );

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

int CC_L8Ax_DebugModeOnOffCCPrint(int mode)
{
	return 0;
}

#ifdef	KDRV_CONFIG_PM

int CC_L8Ax_RunSuspend(void)
{
	// param will be saved automatically....
	return 0;
}

int CC_L8Ax_RunResume(void)
{
	TTX_CC_SetInterrupt(gCcPmData->uiPmCcIntStatus);
	CC_SetInputFormat(gCcPmData->ePmCcModeStatus);
	return 0;
}

#endif

/** @} */
