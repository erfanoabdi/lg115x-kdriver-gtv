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
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <asm/io.h>			/**< For ioremap_nocache */
#include <linux/workqueue.h>		/**< For working queue */
#include <linux/interrupt.h>
#include <linux/delay.h>	/** for mdelay	*/

#include "os_util.h"
#include "afe_drv.h"

#include "afe_kapi.h"
#include "vport_reg.h"

//#include "vport_reg.h"
#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"
#include "cvd_hw.h"

/******************************************************************************
 *				DEFINES
 *****************************************************************************/
#define HTOTAL_DETECTION_LOOP 1

//120102 for PC38374 640*480 : auto size result wrong(+10)
// Need reexamination
#define L9_PC_640_480_WA 		
			
#define SUPPORT_RGB_COMPOSITE_SYNC
//at 1124 Workaround for video horizontal line artifact when 1080P 24Hz to 1080i 50Hz conversion on divx
#define ADC_PERIODIC_HTOTAL_READ
/**
 *	Global variables of the driver
 */

int gEnablePeriodicRead = 0;
int gEnableAPC = 0;

int gEnableScartFB = 0;

LX_AFE_ADC_GAIN_VALUE_T user_gain_value;

extern int gResolution;
extern int gResolutionSearching;

//extern volatile VPORT_Reg_T *gVportReg;

extern LX_AFE_ADC_SET_INPUT_T gPortStatus[3];

extern LX_AFE_PCMODE_MODETABLE_T *gPCTable;

extern ST_WORK_QUEUE	*gWorkqueue;

extern int gIn_SOG_Workaround;

extern int gPrevious_index;
#ifdef ADC_PERIODIC_SIGNAL_INFO_READ
	S_VIDEO_SIZE_INFORM previous_size_inform = {0xffffffff,};
extern int gADC_IN_IFM;
extern S_VIDEO_SIZE_INFORM g_SignalInform;
extern LX_AFE_ADC_INPUT_SOURCE_TYPE_T gADC_type;
#endif
#ifdef L8_PHASE_NOISE_WORKAROUND
extern int	gNew_Component_Input_Detected;
#define HIGH_DEV_THRESHOLD 100000
#define MAX_DEV_COUNT 3
#endif
extern ADC_SYNC_MODE g_ADC_Sync_Mode ;

#define SYNC_LEVEL_100	0xC0
#define SYNC_LEVEL_108	0xBC

//#define	L9_ADC_LVDS_ON_OFF_CONTROL
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

LX_AFE_ADC_INPUT_SOURCE_TYPE_T	gADC_InputSrc = LX_ADC_INPUT_SOURCE_NONE; //added by dws

int ADC_Phase_Convert_to_Reg_Value(int ui_phase_value, int gResolution_index)
{
	unsigned int max, llpll_shift_10bit;

	if(gResolution_index <0)
		return -1;

	if(TB_FS[ConvertTable[gResolution_index]] == 0)
		return -1;
	else
		//max = (float)11538.46 / (float)TB_FS[ConvertTable[gResolution_index]]; // (1/FS) *(1,000,000 /130) *1.5
		//120103 wonsik.do : removed floating point in kernel driver
		//max = (float)7694.31 / (float)TB_FS[ConvertTable[gResolution_index]]; // (1/FS) *(1,000,000 /130) + 2 //0927 by powerory
		max = 7694308 / TB_FS[ConvertTable[gResolution_index]]; // (1/FS) *(1,000,000 /130) + 2 //0927 by powerory

	//llpll_shift_10bit = (((float)max / (float)63.0) * (float)ui_phase_value) +0.5; // (MAX-MIN)/64 * LLPLL_SHIFT_6bit
	llpll_shift_10bit = (max * ui_phase_value / 63) ; // (MAX-MIN)/64 * LLPLL_SHIFT_6bit

//	AFE_PRINT("PC Adjust:Phase [%d]\n", ui_phase_value);
//	AFE_PRINT("llpll_shift_10bit = [%d]\n", (int)(llpll_shift_10bit));

	return llpll_shift_10bit;
}

int ADC_Phase_Convert_to_UI_Value(int reg_phase_value, int gResolution_index)
{
	unsigned int max, llpll_shift_6bit;

	if(gResolution_index <0)
		return -1;

	if(TB_FS[ConvertTable[gResolution_index]] == 0)
		return -1;
	else
		//max = (float)11538.46 / (float)TB_FS[ConvertTable[gResolution_index]]; // (1/FS) *(1,000,000 /130) *1.5
		//120103 wonsik.do : removed floating point in kernel driver
		//max = (float)7694.31 / (float)TB_FS[ConvertTable[gResolution_index]]; // (1/FS) *(1,000,000 /130) + 2 //0927 by powerory
		max = 7694308 / TB_FS[ConvertTable[gResolution_index]]; // (1/FS) *(1,000,000 /130) + 2 //0927 by powerory

//	AFE_PRINT("Phase Max Value= [%d]\n", (int)max);
	llpll_shift_6bit = (63 * reg_phase_value / max) ; // 64/(MAX-MIN) * LLPLL_SHIFT_6bit

//	AFE_PRINT("Phase Reg Value = [0x%x]\n ", reg_phase_value);
//	AFE_PRINT("UI Value = [0x%x]\n", (int)(llpll_shift_6bit));

	return llpll_shift_6bit;
}
/**
 *
 * ADC_SetInputFormat
 *
 * @param	LX_AFE_ADC_SET_INPUT_T
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetInputFormat(LX_AFE_ADC_SET_INPUT_T *stInput)
{
	S_SEL_PORT	*sel_port;
//	ST_EXTAU 	extau;
//	int		unused;

	sel_port = (S_SEL_PORT *)kmalloc(sizeof(S_SEL_PORT) , GFP_KERNEL);

	switch(stInput->adc_input_source_type)
	{
		case 	LX_ADC_INPUT_SOURCE_RGB_PC:
			sel_port->port = 2;

			sel_port->hsselover = 0;
			sel_port->hsseluser = 0;
			sel_port->vsselover = 0;
			sel_port->vsseluser = 0;
			sel_port->compover = 0;
			sel_port->compuser = 0;

			gADC_InputSrc = LX_ADC_INPUT_SOURCE_RGB_PC; //dws
			break;
		case 	LX_ADC_INPUT_SOURCE_RGB_SCART:
			switch(stInput->adc_input_port )
			{
				case 	LX_ADC_YPbPr_IN1:
					sel_port->port = 0;
				break;
				case	LX_ADC_YPbPr_IN2:
					sel_port->port = 1;

				break;
				default:
					AFE_ERROR("never can come in here\n" );
				break;


			}
			//sel_port->port = 1;
			sel_port->hsselover = 0;
			sel_port->hsseluser = 0;
			sel_port->vsselover = 0;
			sel_port->vsseluser = 0;
			sel_port->compover = 0;
			sel_port->compuser = 0;
			gADC_InputSrc = LX_ADC_INPUT_SOURCE_RGB_SCART;	///< Scart RGB
		break;
		case	LX_ADC_INPUT_SOURCE_YPBPR:
			switch(stInput->adc_input_port )
			{
				case 	LX_ADC_YPbPr_IN1:
					sel_port->port = 0;
				break;
				case	LX_ADC_YPbPr_IN2:
					sel_port->port = 1;

				break;
				default:
					AFE_ERROR("never can come in here\n" );
				break;


			}

			sel_port->hsselover = 1;
			sel_port->hsseluser = 1;
			sel_port->vsselover = 1;
			sel_port->vsseluser = 1;
			sel_port->compover = 1;
			sel_port->compuser = 1;

			gADC_InputSrc = LX_ADC_INPUT_SOURCE_YPBPR; //dws
		break;
		case	LX_ADC_INPUT_SOURCE_NONE:
			gADC_InputSrc = LX_ADC_INPUT_SOURCE_NONE; //dws
			kfree(sel_port);
			return 0;
		break;

	}


	ADC_SetPort(sel_port);
	ADC_SetType(stInput->adc_input_source_type);


	// searching signal after port setting
	// This IFM is no-use. Because SetPort will trigger interrupt.
	// In the case, booting with RGB connected, interrupt is not occured
#if 0
	gResolution = -1;
	ADC_IFM_main(&gResolution);
	if(gResolution >= 0)
	{
		extau.extau_up = (gPCTable+gResolution)->u16_HStart;
		extau.extau_left = (gPCTable+gResolution)->u16_VStart;

		ADC_IBD_Setting(&extau , gResolution);

//	   AFE_PRINT("$ Setting ADC Phase to [0x%x]\n", (gPCTable+gResolution)->u16_ADC_Phase);
//	   ADC_SetPhaseValue((gPCTable+gResolution)->u16_ADC_Phase); // by dws
	}

	AFE_PRINT("%s resolution %d\n" , __func__ , gResolution);
#endif
	//gResolution = -1;

	//ADC_IFM_ISR((void *)&unused);

	//queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);

	// Invoke frequency detection task
	//ADC_Clear_Previous_Size_Info();

	kfree(sel_port);

	return 0;

}

#ifdef ADC_PERIODIC_SIGNAL_INFO_READ
/**
 *
 * ADC_Periodic_Task
 *
 * @param	void
 * @desp	periodic read size inform, and excute adc_IFM if input format change detected.
 * @return	int
 *
*/
int ADC_Periodic_Task(void)
{
	S_VIDEO_SIZE_INFORM adc_size;
	static int	prev_size_transition_flag = 0;
	//ST_GAIN_INFORM gain;
	static int	sync_level_count = 0;
	static int	prev_sync_level = 100;
	int	sync_level;
	//static int 	sync_level_avg;

	/*
	if ( ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 )) && ( ADC_Check_Sync_Exist() < 0) )
	{
		ADC_L9Bx_Mute_Control(ADC_MUTE_CTRL_MUTE);
		ADC_DEBUG("@@@ ADC Sync Loss : Mute Enable @@@ \n");
	}
	*/

	if( (gEnablePeriodicRead > 0) && (gIn_SOG_Workaround < 1))
	{
		ADC_GetSizeValue(&adc_size);

		// For SCART RGB FB_EN control
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
		{
			if( gADC_type == LX_ADC_INPUT_SOURCE_RGB_SCART)
			{
				// Enable Scart FB_En, only when RGB input (3ch ADC) is stable
				if(gEnableScartFB && (gResolution >= 0) && (gResolutionSearching == 0) && (ADC_L9Bx_Get_LLPLL_Filter_Status() > 0) && ( ADC_Check_Sync_Exist() == 0) )
					CVD_L9Bx_Set_Scart_FB_En(1);	// Disable FB_EN on llpll unlock state
				//120110 wonsik.do : not to check LLPLL filter status
				//			if((gEnableScartFB == 0) ||/* (ADC_L9Bx_Get_LLPLL_Filter_Status() == 0) ||*/ ( ADC_Check_Sync_Exist() < 0) )
				//120118 wonsik.do : not to clear scart fb_en register on set_scart_overlay function.
				/*
				   if( ADC_Check_Sync_Exist() < 0 )
				   CVD_L9Bx_Set_Scart_FB_En(0);	// Disable FB_EN on llpll unlock state
				   else if(gEnableScartFB && (gResolution >= 0) && (gResolutionSearching == 0) && (ADC_L9Bx_Get_LLPLL_Filter_Status() > 0) && ( ADC_Check_Sync_Exist() == 0) )
				   CVD_L9Bx_Set_Scart_FB_En(1);	// Disable FB_EN on llpll unlock state
				 */
			}
		}

#ifdef SUPPORT_RGB_COMPOSITE_SYNC
		//111020 for composite sync support
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
		{
			if( gADC_type == LX_ADC_INPUT_SOURCE_RGB_PC)
			{
				if (( ADC_Check_Sync_Exist() == 0) && (adc_size.vd_vsize > 0) && (adc_size.vd_hsize > 0) && (ADC_Get_Vtotal_Value() < 100))
				{
					if (g_ADC_Sync_Mode != ADC_COMPOSITE_SYNC_MODE)
						ADC_Set_Sync_Mode(ADC_COMPOSITE_SYNC_MODE);
				}
				else if(g_ADC_Sync_Mode != ADC_SEPERATED_SYNC_MODE) 
					ADC_Set_Sync_Mode(ADC_SEPERATED_SYNC_MODE);
			}
			else if ( ( gADC_type == LX_ADC_INPUT_SOURCE_RGB_SCART) && (g_ADC_Sync_Mode != ADC_COMPOSITE_SYNC_MODE) )
				ADC_Set_Sync_Mode(ADC_COMPOSITE_SYNC_MODE);
			else if ( ( gADC_type == LX_ADC_INPUT_SOURCE_YPBPR) && (g_ADC_Sync_Mode != ADC_COMPOSITE_SYNC_MODE) )
				ADC_Set_Sync_Mode(ADC_COMPOSITE_SYNC_MODE);
		}
#endif

		//For macrovision enabled 480i component signals, ext_vsize can be changed  more then 20.
		// For samsung BDP vd_vsize fluctuate more than 1000
		if( ( (gADC_type == LX_ADC_INPUT_SOURCE_RGB_PC) && (abs(previous_size_inform.ext_vsize - adc_size.ext_vsize) > 5)) ||  ( (gADC_type != LX_ADC_INPUT_SOURCE_RGB_PC) && (abs(previous_size_inform.ext_vsize - adc_size.ext_vsize) > 30 ))|| (abs(previous_size_inform.vd_vsize - adc_size.vd_vsize) > 3000) || (abs(previous_size_inform.vd_hsize - adc_size.vd_hsize) > 5) || (prev_size_transition_flag> 0))
		{
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
				ADC_L9Bx_Mute_Control(ADC_MUTE_CTRL_MUTE);
#ifdef	L9_ADC_LVDS_ON_OFF_CONTROL
				ADC_L9Bx_LVDS_Channel_Enable_Control(0);
#endif

				// 111123 wonsik.do : for post gain control for 110% white saturation test.
				if(prev_sync_level != 100)
				{
					ADC_L9Bx_Post_Gain_Control(100);
					prev_sync_level = 100;
				}
			}

			// For SCART RGB FB_EN control
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
				if( gADC_type == LX_ADC_INPUT_SOURCE_RGB_SCART)
				{
					CVD_L9Bx_Set_Scart_FB_En(0);	// Disable FB_EN on input freq. change
				}
			}

			gResolutionSearching = 1;
			ADC_DEBUG("# Size Change Detected ![%d]\n", prev_size_transition_flag);
			ADC_DEBUG("Prev ext_v[%ld], vd_v[%ld], vd_h[%ld]\n", previous_size_inform.ext_vsize, previous_size_inform.vd_vsize, previous_size_inform.vd_hsize);
			ADC_DEBUG("Curr ext_v[%ld], vd_v[%ld], vd_h[%ld]\n", adc_size.ext_vsize, adc_size.vd_vsize, adc_size.vd_hsize);
			memcpy(&previous_size_inform, &adc_size, sizeof(S_VIDEO_SIZE_INFORM));

			prev_size_transition_flag = 0;

			// Set Ctl_Add valut to 0x13
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
				if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR) // YPbPr Source
					ADC_L9Bx_Set_Ctl_Add(0x0, 0x0, 0x2, 0x3);

			// Read size value again to check the read value is stable.
			ADC_GetSizeValue(&adc_size);

			if( (abs(adc_size.ext_vsize - previous_size_inform.ext_vsize) < 3) && (abs(adc_size.vd_vsize - previous_size_inform.vd_vsize) < 3) && (abs(adc_size.vd_hsize - previous_size_inform.vd_hsize) < 3))
			{
				if ( (adc_size.ext_vsize > 250) && (adc_size.vd_vsize > 240000) && (adc_size.vd_hsize > 240) && (adc_size.vd_hsize < 1604)  /* && (ADC_Check_Sync_Exist() == 0)*/ ) 
				{
					// Table searching , Clamp setting & setting global timing values
					if( ADC_IFM_Routine(&adc_size) < 0)
					{
						//111219 wonsik.do : ???
						//	prev_size_transition_flag = 1;
					}
					else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) ) {
						ADC_L9Bx_Mute_Control(ADC_MUTE_CTRL_AUTO);
#ifdef	L9_ADC_LVDS_ON_OFF_CONTROL
						ADC_L9Bx_LVDS_Channel_Enable_Control(1);
#endif
					}

				}
				else
				{
					gResolution = -1;
					ADC_DEBUG("# Invalid Signal !\n");
				}

				// Is ADC input stable now???
				gResolutionSearching = 0;

			}
			else
			{
				ADC_DEBUG("# ADC unstable !\n");
				prev_size_transition_flag = 1;
			}

			sync_level_count = 0;
		}
		else
		{
			//111123 wonsik.do : for shibasokuk 110% white saturation test.
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
				if( gADC_type == LX_ADC_INPUT_SOURCE_YPBPR)
				{
					if((gResolution == 59) || (gResolution == 61))
					{
						sync_level_count ++;
						if(sync_level_count > 10)
						{
							sync_level_count = 0;
							sync_level = ADC_L9Bx_Read_Sync_Low_Level();
							if( ( sync_level < SYNC_LEVEL_108 ) && (prev_sync_level == 100))
							{
								ADC_DEBUG("### Sync Level 108 [0x%x] ###\n", sync_level);
								prev_sync_level = 108;
								//gain.gain_G = 0.9 * user_gain_value.G_Gain_Value;
								//gain.gain_R = user_gain_value.R_Gain_Value;
								//gain.gain_B = user_gain_value.B_Gain_Value;

								ADC_L9Bx_Post_Gain_Control(90);
								//ADC_SetGainRGB(&gain);
							}
							else if( ( sync_level > SYNC_LEVEL_100 ) && (prev_sync_level == 108))
							{
								ADC_DEBUG("### Sync Level 100 [0x%x] ###\n", sync_level);
								prev_sync_level = 100;
								//gain.gain_G = user_gain_value.G_Gain_Value;
								//gain.gain_R = user_gain_value.R_Gain_Value;
								//gain.gain_B = user_gain_value.B_Gain_Value;
								ADC_L9Bx_Post_Gain_Control(100);
								//ADC_SetGainRGB(&gain);
							}
						}
						//	ADC_DEBUG("##### Sync Level [0x%x] #####\n", (int)gain.gain_G);
					}
				}
				else
				{
					if(prev_sync_level != 100)
					{
						ADC_L9Bx_Post_Gain_Control(100);
						prev_sync_level = 100;
					}
					sync_level_count = 0;
				}
			}

		}

	}

	return 0;


}
#endif

/**
 *
 * ADC_GetTimingInfo
 *
 * @param	LX_AFE_ADC_TIMING_INFO_T
 * @desp	return current input signal information
 * @return	void
 *
*/
void ADC_GetTimingInfo(LX_AFE_ADC_TIMING_INFO_T *stTiming)
{

	S_VIDEO_SIZE_INFORM inform;
	ST_EXTAU extau;
	UINT8 ADC_No_Sync = 0;

#ifdef L8_PHASE_NOISE_WORKAROUND
	S_RGB_OUT	dev_result;
	static int high_dev_count;
#endif
	//	AFE_TRACE("%s entered %d\n" , __func__ , gResolution);

	// Check if sync exist on selected port
	if(ADC_Check_Sync_Exist() < 0)
	{
		//AFE_PRINT("ADC_NO_SYNC!!!\n");
		ADC_No_Sync = 1;
		//		gResolution = -1;

		// why???
		//		memset((void *)&previous_size_inform, 0, sizeof(S_VIDEO_SIZE_INFORM));
	}

	if(gResolution < 0)
	{
		memset((void *)stTiming , 0 , sizeof(LX_AFE_ADC_TIMING_INFO_T));
		stTiming->u8_Table_Idx = 0xff;

		//return;
	}
	if(ADC_No_Sync)
	{
		memset((void *)stTiming , 0 , sizeof(LX_AFE_ADC_TIMING_INFO_T));
		stTiming->u8_Table_Idx = 0xff;

		stTiming->Sync_Exist = 0;
		return;
	}
	stTiming->Sync_Exist = 1;
	stTiming->Unstable = 0;
	//	AFE_TRACE("%s entered\n" , __func__);

	stTiming->llpll_status = ADC_Get_LLPLL_Filter_Status();
	//add 1202 to read Current H/V Frequency and VTotal, Not stored value.
	//ADC_GetInputSignalInform(&inform);
	ADC_GetSizeValue(&inform);

	if( inform.vd_vsize != 0)
	{
		stTiming->u16_Cur_HFreq = (FREQ_CONV_HZ + (inform.vd_hsize>>1))/(inform.vd_hsize + 2);
		stTiming->u16_Cur_VFreq = (FREQ_CONV_HZ*1000 + (inform.vd_vsize>>1))/inform.vd_vsize;
	}
	else
	{
		stTiming->u16_Cur_HFreq = 0;
		stTiming->u16_Cur_VFreq = 0;
	}
	stTiming->u16_Cur_VTotal = inform.ext_vsize;

	if( previous_size_inform.vd_vsize != 0)
	{
		stTiming->u16_Prev_HFreq = (FREQ_CONV_HZ + (previous_size_inform.vd_hsize>>1))/(previous_size_inform.vd_hsize + 2);
		stTiming->u16_Prev_VFreq = (FREQ_CONV_HZ*1000 + (previous_size_inform.vd_vsize>>1))/previous_size_inform.vd_vsize;
	}
	else
	{
		stTiming->u16_Prev_HFreq = 0;
		stTiming->u16_Prev_VFreq = 0;
	}
	stTiming->u16_Prev_VTotal = previous_size_inform.ext_vsize;

#ifdef ADC_PERIODIC_HTOTAL_READ
	if((gResolution >= 0) && (gADC_IN_IFM == 0) && (gADC_type != LX_ADC_INPUT_SOURCE_RGB_PC) && (gResolutionSearching == 0))
	{
		ADC_Check_Htotal_Diff(1, gResolution);
	}
#endif

	ADC_IFM_GetInputSignal(&inform);
	ADC_IBD_GetBackPorch(&extau);

	if(gResolution < 0)
		stTiming->u8_Table_Idx = 0xff;
	else
		stTiming->u8_Table_Idx = gResolution;

	if(inform.vd_vsize != 0)
	{
		stTiming->u16_HFreq = (FREQ_CONV_HZ + (inform.vd_hsize>>1))/(inform.vd_hsize + 2);
		stTiming->u16_VFreq = (FREQ_CONV_HZ*1000 + (inform.vd_vsize>>1))/inform.vd_vsize;
	}
	else
	{
		stTiming->u16_HFreq = 0;
		stTiming->u16_VFreq = 0;
	}
	stTiming->u16_VTotal = inform.ext_vsize;

	stTiming->u16_HStart = extau.extau_up;
	stTiming->u16_VStart = extau.extau_left;

	// no calculate data...............
	if(gResolution < 0)
	{
		stTiming->u16_HTotal = 0;
		stTiming->u16_HActive = 0;
		stTiming->u16_VActive = 0;
	}
	else
	{
		stTiming->u16_HTotal = (gPCTable+gResolution)->u16_HTotal;
		stTiming->u16_HActive = LX_Default_Resolution[(gPCTable+gResolution)->u8_Res_Idx].u16_Width;
		stTiming->u16_VActive = LX_Default_Resolution[(gPCTable+gResolution)->u8_Res_Idx].u16_Height;
	}
	// no calculate data...............

	//stTiming->u16_Phase = ADC_Phase_Convert_to_UI_Value(ADC_GetPhaseValue(), gResolution);
	stTiming->u16_Phase = 0;//by dws
	stTiming->u8_ScanType = ADC_GetScanType();
	stTiming->Unstable |= gResolutionSearching;

	ADC_Read_HSync_Width(&stTiming->hs_width);
	//stTiming->hs_width = 0;

	ADC_Read_VSync_Width(&stTiming->vs_width);

#ifdef L8_PHASE_NOISE_WORKAROUND
	if(gEnableAPC > 0)
	{
		if((gADC_type == LX_ADC_INPUT_SOURCE_YPBPR) && (stTiming->Unstable < 1) && (stTiming->Sync_Exist == 1) && (stTiming->u8_Table_Idx != 0xff) && gNew_Component_Input_Detected)
		{
			ADC_GetRGBResult(&dev_result);
			//		AFE_PRINT("@@@ G Dev Result : [%d]\n", dev_result.out_g);

			if(dev_result.out_g > HIGH_DEV_THRESHOLD)
				high_dev_count++;
			else
				high_dev_count = 0;

			if(high_dev_count > MAX_DEV_COUNT)
			{
				high_dev_count = 0;
				gNew_Component_Input_Detected = 0;
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->apc);
			}
		}
	}
#endif
	return;
}

int ADC_AutoAdjust(LX_AFE_ADJ_PC_T *stAdj)		// input format measurement?? + Phase setting
{
	ST_EXTAU *extau;

	int resolution_index, hsize_loop, i, maxi;
	//120103 wonsik.do : removed floating point in kernel driver
	UINT64 currentDev, maxDev;
	//int j;
	UINT16	HActive_read, HTotal_table, HActive_table, HTotal_new;

	//ADC_IFM_main(&gResolution);	// look into input signal's resolution and set paramter
	maxi = 0;	// mantis CID 4662

	resolution_index = gResolution; // copy current resolution index

	if(gResolution < 0){
		AFE_ERROR("Error : not selected resolution index yet \n" );
		stAdj->result = -1;
		return -1;
	}

	//ADC_IBD_main(gResolution);	// set boundary region

	switch(stAdj->adjust_mode)
	{
		case LX_AFE_PC_Auto_HSize:
			ADC_DEBUG("\n Auto HSize\n");

			HTotal_table = (gPCTable+resolution_index)->u16_HTotal;
			HTotal_new = HTotal_table;
			HActive_table = LX_Default_Resolution[(gPCTable+resolution_index)->u8_Res_Idx].u16_Width;

			extau = (ST_EXTAU *)kmalloc(sizeof(ST_EXTAU) , GFP_KERNEL);
			for(hsize_loop=0;hsize_loop<HTOTAL_DETECTION_LOOP;hsize_loop++)
			{
				ADC_IBD_main(resolution_index);	// set boundary region
				ADC_IBD_GetBackPorch(extau);
				HActive_read = extau->extau_right - extau->extau_left + 1;
				ADC_DEBUG("HActive read = [%d], HActive table = [%d], Htotal table = [%d]\n", HActive_read, HActive_table, HTotal_table);

				if(HActive_read != HActive_table)
				{
					ADC_DEBUG("H Size different !!!\n");
					HTotal_new = (HTotal_new * HActive_table) / HActive_read;		// If htotal is increased, read hactive is reduced.
			//		HTotal_new = (HTotal_new * HActive_read) / HActive_table;		// wrong equation
					ADC_DEBUG("Caculate HTotal is [%d]\n", HTotal_new);
					if( (HTotal_new > HTotal_table + 55) || (HTotal_new < HTotal_table - 55))
					{
						ADC_DEBUG("New HTotal is TOO Big or Small\n");
						HTotal_new = HTotal_table;
						stAdj->u16_PC_Clock = HTotal_new;
						break;
					}

					ADC_SetClock(ConvertTable[gResolution], HTotal_new - HTotal_table);
					stAdj->u16_PC_Clock = HTotal_new;
				}
				else
				{
					HTotal_new = HTotal_table;
					stAdj->u16_PC_Clock = HTotal_new;
					break;
				}
			}
			maxDev = 0;
			maxi = 0;
#if 0
			for(j=0;j<2;j++)
			{
				if(j==0)
					ADC_SetPhaseValue(0);
				else if(j==1)
					ADC_SetPhaseValue(ADC_Phase_Convert_to_Reg_Value(31, gResolution));
#endif
#ifdef L9_PC_640_480_WA
			//120102 for PC38374 640*480 : auto size result wrong(+10)
			if((gResolution == 4)&& (HActive_read>630)&&(HActive_read<636))
			{
				ADC_DEBUG("PC 640*480 with Hactive Read = [%d]\n", HActive_read);
				ADC_SetClock(ConvertTable[gResolution], 800 - HTotal_table);//gogosing
				stAdj->u16_PC_Clock = 800;//gogosing
				kfree(extau);
				break;
			}
#endif
				for(i = HTotal_new - SIZE_FRONT_OFFSET ; i <= HTotal_new + SIZE_BACK_OFFSET ; i++)
				{
					ADC_SetClock(ConvertTable[gResolution], i - HTotal_table);
					currentDev = ADC_PHASE_Mesurement_Simple(ConvertTable[gResolution], i);
					ADC_DEBUG("Deviation [%d] = %ud\n", i, (UINT32)currentDev);
					if( currentDev > maxDev )
					{
						maxDev = currentDev;
						maxi = i;
					}
				}
//			}
			ADC_DEBUG("## Max Dev = [%d]\n", maxi);
			ADC_SetClock(ConvertTable[gResolution], maxi - HTotal_table);
			stAdj->u16_PC_Clock = maxi;

			kfree(extau);
			break;

		case LX_AFE_PC_AUTO_PHASE:
			ADC_PHASE_main(resolution_index);	// set phase
			stAdj->u16_PC_Phase = ADC_Phase_Convert_to_UI_Value(ADC_GetPhaseValue(), gResolution);
			//Workaound for H position Shift after Auto Config
			if ( lx_chip_rev( ) < LX_CHIP_REV( L8, B0 ) )
				ADC_Reset_Digital();
			msleep(10);
			break;

		case LX_AFE_PC_AUTO_POSITION:
			ADC_IBD_main(resolution_index);	// set boundary region
			extau = (ST_EXTAU *)kmalloc(sizeof(ST_EXTAU) , GFP_KERNEL);
			ADC_IBD_GetBackPorch(extau);

			stAdj->s16_PC_HDelay = extau->extau_left;
			stAdj->s16_PC_VDelay = extau->extau_up;
			kfree(extau);
			break;

		default:
			break;

	}

	if(resolution_index != gResolution) //if resolution changed during auto adjust
	{
		//ADC_IFM_main(&gResolution);
		//ADC_IFM_ISR((void *)&resolution_index);
		ADC_Clear_Previous_Size_Info();
		stAdj->s16_PC_HDelay = 0;
		stAdj->s16_PC_VDelay = 0;
		stAdj->u16_PC_Clock = 0;
		stAdj->u16_PC_Phase = ADC_Phase_Convert_to_UI_Value((gPCTable+gResolution)->u16_ADC_Phase, gResolution);
		stAdj->result = -1;
		return -1;
	}

	stAdj->result = 0;

	return 0;
}


int ADC_PCAdjust(LX_AFE_ADJ_PCMODE_INFO_T *stAdjMode)
{
	int ret = 0, reg_value;

	if(gResolutionSearching > 0)
		return -1;

	switch(stAdjMode->mode)
	{
		case LX_AFE_PC_HPOS:
//			AFE_PRINT("PC Adjust:HPos [%d]\n", stAdjMode->value);
//			ADC_SetHorPos(stAdjMode->value);
		break;
		case LX_AFE_PC_VPOS:
//			AFE_PRINT("PC Adjust:VPos [%d]\n", stAdjMode->value);
//			ADC_SetVerPos(stAdjMode->value);
		break;
		case LX_AFE_PC_CLOCK:
//			AFE_PRINT("PC Adjust:Clock [%d]\n", stAdjMode->value);
			if(gResolution <0)
				return -1;

			ret = ADC_SetClock(ConvertTable[gResolution], stAdjMode->value);

		break;
		case LX_AFE_PC_HTOTAL:
//			AFE_PRINT("PC Adjust:HTotal [%d]\n", stAdjMode->value);
//			ADC_SetHorTotal(stAdjMode->value);
		break;
		case LX_AFE_PC_PHASE:
			reg_value = ADC_Phase_Convert_to_Reg_Value(stAdjMode->value, gResolution);
//			AFE_PRINT("PC Adjust:Phase UI value[%d] to Reg Value[%d]\n", stAdjMode->value, reg_value);
			if(reg_value>=0)
			{
				ADC_SetPhaseValue(reg_value);
				/*
				OS_MsecSleep(100);
				ADC_Channel_Power_Control(0);
				OS_MsecSleep(100);
				ADC_Channel_Power_Control(1);
				OS_MsecSleep(100);
				*/
			}
			else
				return -1;
		break;
		case LX_AFE_PCMODE_NUM:

		break;
		default:
			AFE_PRINT("Invalid LX_AFE_ADJ_PCMODE_INFO_T type\n");
		break;

	}

	return ret;
}

int ADC_Enable_Periodic_Signal_Info_Read(UINT32 enable)
{
	ADC_DEBUG("%s with value [%d]\n", __func__, enable);

	if(enable > 0)
		gEnablePeriodicRead = 1;
	else
		gEnablePeriodicRead = 0;

	return 0;
}

int ADC_Execute_Format_Detection(void)
{
	//queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
	// Invoke frequency detection task
	ADC_Clear_Previous_Size_Info();

	return 0;
}

int ADC_Enable_Component_Auto_Phase(UINT32 enable)
{
	if(enable > 0)
		gEnableAPC = 1;
	else
		gEnableAPC = 0;

	return 0;
}


int ADC_Test_Program(LX_AFE_ADC_TEST_PARAM_T *stADC_Test_Param)
{
	int ret = 0;

	switch(stADC_Test_Param -> item_to_test)
	{
		case ADC_TEST_PHASE_READ:
			stADC_Test_Param->ADC_Phase_Value = ADC_GetPhaseValue();
			break;
		case ADC_TEST_PHASE_WRITE:
			ADC_SetPhaseValue(stADC_Test_Param->ADC_Phase_Value);
			break;
		case ADC_TEST_APA:
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
				ADC_L9Bx_Component_APA_Test(stADC_Test_Param->ADC_APA_Delay, stADC_Test_Param->ADC_APA_Enable, stADC_Test_Param->ADC_APA_Same_Count, stADC_Test_Param->ADC_APA_Execute, stADC_Test_Param->ADC_APA_Enable_Print);
			break;
		case ADC_TEST_SOGLVL_READ:
			ADC_Get_SOG_Level( &stADC_Test_Param->ADC_Soglvl_Value);
			break;
		case ADC_TEST_SOGLVL_WRITE:
			ADC_Set_SOG_Level( stADC_Test_Param->ADC_Soglvl_Value);
			break;
			
		default:
			ret = -1;
			break;
	}

	return ret;
}

int ADC_Clear_Previous_Size_Info(void)
{
	AFE_PRINT("Clearing Previous Size Inform\n");
	memset((void *)&previous_size_inform, 0, sizeof(S_VIDEO_SIZE_INFORM));
	return 0;
}
