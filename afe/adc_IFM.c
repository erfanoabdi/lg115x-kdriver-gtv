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

/*! \file ADC_IFMfunc.c
 * \brief ADC module control Drivers linux module
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

#include "os_util.h"
#include "afe_drv.h"
#include <linux/interrupt.h>
#include "afe_kapi.h"
#include "vport_reg.h"

//#include "vport_reg.h"
#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"
#include "cvd_hw.h"

#include <linux/delay.h>	/** for mdelay	*/
/******************************************************************************
 *				DEFINES
 *****************************************************************************/
//#define ADC_1080I_WORKAROUND
//#define ADC_POLARITY_OVERRIDE
//#define ADC_SIZE_DETECTION_WORKAROUND
#define ADC_EXT_CLAMP_WORKAROUND
#define L8_B0_SCART_FB_EN_WORKAROUND

//#define ADC_MSPG_114_TIMING_WORKAROUND
//#define ADC_COMP_NO_SYNC_WORKAOURND
//extern volatile VPORT_Reg_T *gVportReg;
/**
 *	Global variables of the driver
 */
extern LX_AFE_PCMODE_MODETABLE_T *gPCTable;

extern unsigned int gPCTableNum;

extern LX_AFE_ADC_INPUT_SOURCE_TYPE_T	gADC_InputSrc; //added by dws
extern ST_WORK_QUEUE	*gWorkqueue;
extern int gResolution;

int gIn_SOG_Workaround = 0;

int gPrevious_index = 0xffff;
int gADC_IN_IFM = 0;
#ifdef USE_ADC_TIMER
extern S_VIDEO_SIZE_INFORM previous_size_inform;
#endif

#ifdef L8_PHASE_NOISE_WORKAROUND
int	gNew_Component_Input_Detected = -1;
#endif
extern UINT32	gADC_APA_Enable;
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
S_VIDEO_SIZE_INFORM g_SignalInform = {0xffffffff,};

static LX_AFE_RESOLUTION_TYPE_T	g_User_Resolution = LX_RES_1024X768;
/**
 *
 * Input format measurement
 * vsize, hsize, ext_vsize deteting
 *
 * @param	S_VIDEO_SIZE_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/

int ADC_GetInputSignalInform(S_VIDEO_SIZE_INFORM *inform)
{
	S_VIDEO_SIZE_INFORM inform_proc, *inform_temp;

//	AFE_PRINT("%s entered\n" , __func__);
	// memory clear
	memset((void *)&inform_proc , 0 , sizeof(S_VIDEO_SIZE_INFORM));
	inform_temp = (S_VIDEO_SIZE_INFORM *)kmalloc(sizeof(S_VIDEO_SIZE_INFORM),GFP_KERNEL);

#ifndef USE_ADC_TIMER
	//add by dws
	//OS_MsecSleep(100); //0731 dws
///
#endif

#if 0
	for(i = 0 ; i < INPUT_SIGNAL_INFORM_REPEAT ; i++)	// getting the inform for INPUT_SIGNAL_INFORM_REPEAT times
	{
		ADC_GetSizeValue(inform_temp);

		inform_proc.ext_vsize += inform_temp->ext_vsize;
		inform_proc.vd_vsize += inform_temp->vd_vsize;
		inform_proc.vd_hsize += inform_temp->vd_hsize;
/*
		AFE_PRINT("temp [%d] ext_vsize : %d , vd_vsize : %d , vd_hsize : %d\n", i ,
		inform_temp->ext_vsize , inform_temp->vd_vsize , inform_temp->vd_hsize);
		AFE_PRINT("proc [%d] ext_vsize : %d , vd_vsize : %d , vd_hsize : %d\n", i ,
		inform_proc.ext_vsize , inform_proc.vd_vsize , inform_proc.vd_hsize);
*/

	//	udelay(0);	// delay should be used?
	}

	// calculate average vlaue
	inform_proc.ext_vsize = (inform_proc.ext_vsize + (INPUT_SIGNAL_INFORM_REPEAT>>1))>>INPUT_SIGNAL_INFORM_REPEAT_POW;
	inform_proc.vd_vsize = (inform_proc.vd_vsize + (INPUT_SIGNAL_INFORM_REPEAT>>1))>>INPUT_SIGNAL_INFORM_REPEAT_POW;
	inform_proc.vd_hsize = (inform_proc.vd_hsize + (INPUT_SIGNAL_INFORM_REPEAT>>1))>>INPUT_SIGNAL_INFORM_REPEAT_POW;

#else
	ADC_GetSizeValue(inform_temp);

	inform_proc.ext_vsize += inform_temp->ext_vsize;
	inform_proc.vd_vsize += inform_temp->vd_vsize;
	inform_proc.vd_hsize += inform_temp->vd_hsize;
#endif
	inform->vd_vsize = inform_proc.vd_vsize;
	inform->vd_hsize = inform_proc.vd_hsize;
	inform->ext_vsize = inform_proc.ext_vsize;

	inform->polarity = inform_temp->polarity;


//	AFE_PRINT("ext_v:%ld,vd_v:%ld,vd_h:%ld\n", inform->ext_vsize , inform->vd_vsize , inform->vd_hsize);

	kfree(inform_temp);

	return 0;

}


/**
 *
 * search correct setting value from the table
 *
 * @param	S_VIDEO_SIZE_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_IFM_SearchTable(S_VIDEO_SIZE_INFORM *inform)
{
	int step , iSearchedIndex = -1;
	int iMinGapValue = 0xffffff;
	int iHorGap , iVerGap , iVTotalGap, iHSGap;
	S_VIDEO_SIZE_INFORM *par;
	int iLoopNum;
	//unsigned int iInterlace;
	UINT32 vs_width, hs_width;

	// translate the gotten parameter
	par = (S_VIDEO_SIZE_INFORM *)kmalloc(sizeof(S_VIDEO_SIZE_INFORM) , GFP_KERNEL);

	par->ext_vsize = inform->ext_vsize;
	if((inform->vd_hsize+2) != 0)
	{
		par->vd_hsize = (FREQ_CONV_HZ + (inform->vd_hsize>>1) )/(inform->vd_hsize + 2);
	}
	else
	{
		 par->vd_hsize = 0;
	}
	if(inform->vd_vsize != 0)
		par->vd_vsize = (FREQ_CONV_HZ*1000 + (inform->vd_vsize>>1))/(inform->vd_vsize);
	else
		par->vd_vsize = 0;

	par->polarity = (0x08)>>(inform->polarity); // convert register value to AFE_PCMODE_FLAGs

	//iInterlace = (ADC_GetScanType()^1);
	//iLoopNum = PARAM_NUM;
	iLoopNum = gPCTableNum; //0622 dws

	ADC_DEBUG("calculated ext_vsize : %ld , vd_vsize : %ld , vd_hsize : %ld , pol : %ld , src : %d\n",
		par->ext_vsize , par->vd_vsize , par->vd_hsize , par->polarity, gADC_InputSrc);

	//AFE_PRINT("interace mode is %d\n", iInterlace);
#define	HOR_TOLERANCE	3
#define	VER_TOLERANCE	3
#define	TOTAL_TOLERANCE	0xfff
#define VSYNC_WIDTH_TOLERANCE	1
#define HSYNC_WIDTH_TOLERANCE	5
#define VTOTAL_TOLERANCE	50

	for(step = 0 ; step < iLoopNum ; step++)
	{
		if((gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR)||(gADC_InputSrc == LX_ADC_INPUT_SOURCE_RGB_SCART)) // YPbPr Source
		{
		   if(!((gPCTable+step)->u8_Status_Flag & AFE_PCMODE_FLAG_YPBPR_BIT))
			  continue;
		}
		else	// RGB PC mode
		{
		   if((gPCTable+step)->u8_Status_Flag & AFE_PCMODE_FLAG_YPBPR_BIT)
			  continue;
		   else if(!(par->polarity & ((gPCTable+step)->u8_Status_Flag & 0x0F))) continue;	//0622 dws
		}


	// L8's interlace determine is not working
	//	if(iInterlace != (((gPCTable+step)->u8_Status_Flag&0x10) >> 4) ) continue;
		//if(par->polarity != ((gPCTable+step)->u8_Status_Flag&0x3) ) continue;	//0622 dws
//		AFE_PRINT("step = [%d], Hfreq = [%d], Vfreq = [%d], flag = [0x%x], pol = [0x%x] \n",step, (gPCTable+step)->u16_HFreq, (gPCTable+step)->u16_VFreq ,(gPCTable+step)->u8_Status_Flag, par->polarity);

		// check horizontal freq
		iHorGap = abs((gPCTable+step)->u16_HFreq - par->vd_hsize);

		if( iHorGap <= HOR_TOLERANCE)
		{
			// check vertical freq
			iVerGap = abs((gPCTable+step)->u16_VFreq - par->vd_vsize);
			if( iVerGap <= VER_TOLERANCE)
			{
				if((gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR)||(gADC_InputSrc == LX_ADC_INPUT_SOURCE_RGB_SCART)) // YPbPr Source
				{
					// check vertical total
					iVTotalGap = ((gPCTable+step)->u16_VTotal - par->ext_vsize);

					//added 110622 : for abnormal component signal to be invalide signal, Vtotal must be measured
					//					: measured vototal can be increased due to serration and macrovision pulses.
					if ( iVTotalGap >= (-(VTOTAL_TOLERANCE)) && (iVTotalGap <= (gPCTable+step)->u8_VTotal_Tolerance) )
					{
						if(iMinGapValue > (iHorGap + iVerGap))
						{
							iMinGapValue = (iHorGap + iVerGap);
							iSearchedIndex = step;
							AFE_PRINT("1st Match Table : step=[%d], flag=[0x%x], pol=[0x%ld], HorGap=[%d], VerGap=[%d], VtotalGap=[%d], MinGap=[%d] \n",step ,(gPCTable+step)->u8_Status_Flag, par->polarity, iHorGap, iVerGap, iVTotalGap, iMinGapValue);
						}
					}
				}
				else	// RGB PC mode
				{

					// check vertical total
					iVTotalGap = abs((gPCTable+step)->u16_VTotal - par->ext_vsize);
					if(iVTotalGap <= (gPCTable+step)->u8_VTotal_Tolerance)
					{
						// add code  here for extra option

						//If Vsync Width is not zero
						if((gPCTable+step)->u8_VSync_Width > 0)
						{
							ADC_Read_VSync_Width(&vs_width);

							AFE_PRINT("Step [%d], Table Vsync Width = [%d], Read Vsync Width = [%d]\n ", step, (gPCTable+step)->u8_VSync_Width, vs_width);

							if((abs((gPCTable+step)->u8_VSync_Width - vs_width)) <= VSYNC_WIDTH_TOLERANCE)
							{
								if(iMinGapValue > (iHorGap + iVerGap + iVTotalGap))
								{
									iMinGapValue = (iHorGap + iVerGap + iVTotalGap);
									iSearchedIndex = step;
									AFE_PRINT("1st Match Table : step=[%d], flag=[0x%x], pol=[0x%ld], HorGap=[%d], VerGap=[%d], VtotalGap=[%d], MinGap=[%d] \n",step ,(gPCTable+step)->u8_Status_Flag, par->polarity, iHorGap, iVerGap, iVTotalGap, iMinGapValue);
								}
							}
						}

						//If Vsync Width is zero, do not compare vsync width value
						else
						{
							if((gPCTable+step)->u8_HSync_Width > 0)
							{
								ADC_Read_HSync_Width(&hs_width);

								AFE_PRINT("Step [%d], Table Hsync Width = [%d], Read Hsync Width = [%d]\n ", step, (gPCTable+step)->u8_HSync_Width, hs_width);

								iHSGap = abs((gPCTable+step)->u8_HSync_Width - hs_width);
							}
							else
								iHSGap = 0;


							if(iMinGapValue > (iHorGap + iVerGap + iVTotalGap + iHSGap))
							{
								iMinGapValue = (iHorGap + iVerGap + iVTotalGap + iHSGap);
								iSearchedIndex = step;
								AFE_PRINT("1st Match Table : step=[%d], flag=[0x%x], pol=[0x%ld], HorGap=[%d], VerGap=[%d], VtotalGap=[%d], HSGap=[%d], MinGap=[%d] \n",step ,(gPCTable+step)->u8_Status_Flag, par->polarity, iHorGap, iVerGap, iVTotalGap, iHSGap, iMinGapValue);
							}

						}

					}
				}
			}
		}
	}

	if(iSearchedIndex < 0)
	{
	ADC_DEBUG("! 2ND Search Loop\n");

		for(step = 0; step < iLoopNum ; step++)
		{


			if((gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR)||(gADC_InputSrc == LX_ADC_INPUT_SOURCE_RGB_SCART)) // YPbPr Source
			{
				if(!((gPCTable+step)->u8_Status_Flag & AFE_PCMODE_FLAG_YPBPR_BIT))
					continue;
			}
			else	// RGB PC mode
			{
				if((gPCTable+step)->u8_Status_Flag & AFE_PCMODE_FLAG_YPBPR_BIT)
					continue;
			}
			//	if(iInterlace != ((LX_Default_ModeTable[step].u8_Status_Flag&0x10) >> 4) ) continue;
			//	if(par->polarity != (par->u8_Status_Flag&0x3) ) continue;

			//			if(par->ext_vsize < (gPCTable+step)->u16_VTotal)
			if(par->ext_vsize > LX_Default_Resolution[(gPCTable+step)->u8_Res_Idx].u16_Height)
			{
				iHorGap = abs((gPCTable+step)->u16_HFreq - par->vd_hsize);
				iVerGap = abs((gPCTable+step)->u16_VFreq - par->vd_vsize);
				iVTotalGap = abs((gPCTable+step)->u16_VTotal - par->ext_vsize);

				if(iHorGap + iVerGap > 10) continue;

				if(iMinGapValue > (iHorGap + iVerGap))
				{

					iMinGapValue = (iHorGap + iVerGap);
					if(iMinGapValue < TOTAL_TOLERANCE)
					{

						iSearchedIndex = step;
						AFE_PRINT("2nd Match Table : step=[%d], flag=[0x%x], pol=[0x%ld], HorGap=[%d], VerGap=[%d], VtotalGap=[%d], MinGap=[%d] \n",step ,(gPCTable+step)->u8_Status_Flag, par->polarity, iHorGap, iVerGap, iVTotalGap, iMinGapValue);
					}
				}

			}
		}
	}
	else if( (abs((gPCTable+iSearchedIndex)->u16_VFreq - 600) <= VER_TOLERANCE) && ( ((gPCTable+iSearchedIndex)->u8_Res_Idx == LX_RES_1024X768) || ((gPCTable+iSearchedIndex)->u8_Res_Idx == LX_RES_1280X768) || ((gPCTable+iSearchedIndex)->u8_Res_Idx == LX_RES_1360X768) ) )// User PCMode Resolution Setting
	{
		if((iSearchedIndex != 24) && (iSearchedIndex != 98) /*&&(iSearchedIndex != 36)*/) // 1280x768 59Hz or 1360x768 60Hz(VESA)
		{
			if (g_User_Resolution == LX_RES_1024X768)
			{
				if(iSearchedIndex != 16) //1024x768 65MHz
					iSearchedIndex = 15; //1024X768 60Hz (VESA)
			}
			else if (g_User_Resolution == LX_RES_1280X768)
				iSearchedIndex = 25; //1280X768 60Hz (VESA-GTF)
			else if (g_User_Resolution == LX_RES_1360X768)
				if(iSearchedIndex != 36)
					iSearchedIndex = 35; //1360X768 60Hz (VESA)
			AFE_PRINT("3rd User Resolution : Change SearchdeIndex to = [%d]\n",iSearchedIndex );
		}
	}

//	AFE_PRINT("^^ Status Flag [0x%x]\n", (gPCTable+step)->u8_Status_Flag);

	kfree(par);
#undef	HOR_TOLERANCE
#undef	VER_TOLERANCE
#undef	TOTAL_TOLERANCE
	return iSearchedIndex;
}

int ADC_IFM_main(int *iIndex)
{
	int ret;
	//static int previous_index = 0xffff;
	int check_count =0 ;
	int Kadp_Index = 0;

	S_VIDEO_SIZE_INFORM inform, inform_prev;

#ifdef L8_PHASE_NOISE_WORKAROUND
	UINT32 hstart_ap, vstart_ap, hactive_ap, vactive_ap;
	S_RGB_POSITION pos_ap;
	S_RGB_SIZE		size_ap;
#endif
//	AFE_PRINT("%s entered\n" , __func__);

	if(work_pending(&gWorkqueue->ifm))
	{
		ADC_DEBUG("before sync check pending\n");
		return -1;
	}

	if(ADC_Check_Sync_Exist()<0)
	{
		ADC_DEBUG("ADC No Sync\n");
		return -1;
	}

	gADC_IN_IFM = 1;

#ifdef ADC_SIZE_DETECTION_WORKAROUND
	//Disable Resolution Change Interrupt
	ADC_Enable_Detection_Interrupt(0);

	ADC_Power_OnOff();
#endif

//	ADC_Reset_Digital();
//	ADC_Reset_LLPLL();

#ifdef ADC_POLARITY_OVERRIDE
	if (gVportReg->afe3ch_selmux.afe3ch_selmux < 2)
		ADC_Sync_Polarity_Override(1);
#endif

	if(work_pending(&gWorkqueue->ifm))
	{
		ADC_DEBUG("before ext clamp pending\n");
		return -1;
	}

#ifdef ADC_EXT_CLAMP_WORKAROUND
	ADC_Ext_Clamp(1);
#endif

	OS_MsecSleep(50); //0831

	// For RGB PC, or no ADC_SOG_LVL_WORKAROUND
#ifdef ADC_SOG_LVL_WORKAROUND
	if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_RGB_PC)
#else
	if(1)
#endif
	{
		ADC_GetInputSignalInform(&inform_prev);

		//by dws : remove mdelay
		//mdelay(10);
		OS_MsecSleep(10);

		if(work_pending(&gWorkqueue->ifm))
		{
			ADC_DEBUG("before loop pending\n");
			return -1;
		}

		do {

			//		ADC_Reset_Digital();
			ADC_GetInputSignalInform(&inform);

			check_count ++;

			if( (abs(inform.ext_vsize - inform_prev.ext_vsize) < 3) && (abs(inform.vd_vsize - inform_prev.vd_vsize) < 3) && (abs(inform.vd_hsize - inform_prev.vd_hsize) < 3) && (inform.ext_vsize > 250) && (inform.vd_vsize > 240000) && (inform.vd_hsize > 240))
				break;

			//mdelay(50);
			//by dws : remove mdelay
			//mdelay(10);
			OS_MsecSleep(10);

			memcpy((void *)&inform_prev, (void *)&inform, sizeof(S_VIDEO_SIZE_INFORM));

			if(work_pending(&gWorkqueue->ifm))
			{
				ADC_DEBUG("in loop pending \n");
				return -1;
			}
		} while (check_count < 20);

		ret = ADC_IFM_SearchTable(&inform);

		Kadp_Index = ret;
	}
	else		//for Component Input
	{
#if 0
		int valid_count,  sog_lvl_test;
		UINT8 sog_lvl_backup;
		int sog_lvl_prev = 0;
		int valid_size = 0;
		int valid_index_count = 0;
		int search_result;
		int search_result_prev = 0;

		S_VIDEO_SIZE_INFORM *size_inform1, *size_inform2, *size_inform3, *freq_inform;

		ADC_Get_SOG_Level(&sog_lvl_backup);

		size_inform1 = (S_VIDEO_SIZE_INFORM *)kmalloc(sizeof(S_VIDEO_SIZE_INFORM) , GFP_KERNEL);
		size_inform2 = (S_VIDEO_SIZE_INFORM *)kmalloc(sizeof(S_VIDEO_SIZE_INFORM) , GFP_KERNEL);
		size_inform3 = (S_VIDEO_SIZE_INFORM *)kmalloc(sizeof(S_VIDEO_SIZE_INFORM) , GFP_KERNEL);
		freq_inform  = (S_VIDEO_SIZE_INFORM *)kmalloc(sizeof(S_VIDEO_SIZE_INFORM) , GFP_KERNEL);
		gIn_SOG_Workaround = 1;

		for(sog_lvl_test = 5; sog_lvl_test < 16; sog_lvl_test++)
		{
			valid_size = 0;

			ADC_Set_SOG_Level(sog_lvl_test);

			mdelay(50);

			for(valid_count = 0; valid_count < 1; valid_count ++)
			{
				ADC_GetSizeValue(size_inform1);

#if 1
				freq_inform->ext_vsize = size_inform1->ext_vsize;

				if((size_inform1->vd_hsize+2) != 0)
				{
					freq_inform->vd_hsize = (FREQ_CONV_HZ + (size_inform1->vd_hsize>>1) )/(size_inform1->vd_hsize + 2);
				}
				else
				{
					freq_inform->vd_hsize = 0;
				}
				if(size_inform1->vd_vsize != 0)
					freq_inform->vd_vsize = (FREQ_CONV_HZ*1000 + (size_inform1->vd_vsize>>1))/(size_inform1->vd_vsize);
				else
					freq_inform->vd_vsize = 0;

				ADC_DEBUG("SOG LVL[%d], EXT_VSIZE[%ld], VD_VSIZE[%ld], VD_HSIZE[%ld], V_Freq[%ld], H_Freq[%ld]\n", sog_lvl_test, freq_inform->ext_vsize, size_inform1->vd_vsize, size_inform1->vd_hsize, freq_inform->vd_vsize, freq_inform->vd_hsize);
#endif

				if((size_inform1->ext_vsize > 250) && (size_inform1->vd_vsize > 240000) && (size_inform1->vd_hsize > 240))
				{
					mdelay(10);
					ADC_GetSizeValue(size_inform2);
					if( (abs(size_inform1->ext_vsize - size_inform2->ext_vsize) < 3) && (abs(size_inform1->vd_vsize - size_inform2->vd_vsize) < 3) && (abs(size_inform1->vd_hsize - size_inform2->vd_hsize) < 3) )
					{
						mdelay(10);
						ADC_GetSizeValue(size_inform3);

						if( (abs(size_inform3->ext_vsize - size_inform2->ext_vsize) < 3) && (abs(size_inform3->vd_vsize - size_inform2->vd_vsize) < 3) && (abs(size_inform3->vd_hsize - size_inform2->vd_hsize) < 3) )
						{
							valid_size = 1;
							break;
						}
					}
				}
			}

			if(valid_size> 0)
			{
				search_result = ADC_IFM_SearchTable(size_inform3);
				Kadp_Index = search_result;

				if(search_result > -1)
				{
					if(valid_index_count == 0)	//first index found
					{
						sog_lvl_prev = sog_lvl_test;
						search_result_prev = search_result;
						valid_index_count ++;
					}
					else	//
					{
						if( ((sog_lvl_prev + 1) == sog_lvl_test) && (search_result_prev == search_result)) // Same index in succession
						{
//							Kadp_Index = search_result;
							valid_index_count ++;
						}
						else	// New index found
						{
							search_result_prev = search_result;
							sog_lvl_prev = sog_lvl_test;
						}
					}
				}
				else
				{
					valid_index_count = 0;
					search_result_prev = -1;
				}

			}

			if(valid_index_count > 1)
			{
				AFE_PRINT("### Selected SOG LEVEL = [%d]!!! ###\n", sog_lvl_test);
				ADC_Set_SOG_Level(sog_lvl_test);
				break;
			}

		}
		//ADC_Set_SOG_Level(0xb);
		//ADC_Set_SOG_Level(sog_lvl_backup);

		if(valid_index_count < 2)
		{
			AFE_PRINT("### Failed to select SOG Level, restore previous value  [%d]!!! ###\n", sog_lvl_backup);
			ADC_Set_SOG_Level(sog_lvl_backup);
		}

		kfree(size_inform1);
		kfree(size_inform2);
		kfree(size_inform3);
		kfree(freq_inform);

		OS_MsecSleep(300);

		gIn_SOG_Workaround = 0;

		ret = search_result;
#endif
	}

#ifdef ADC_SIZE_DETECTION_WORKAROUND
	ADC_Enable_Detection_Interrupt(1);
#endif

#ifdef ADC_POLARITY_OVERRIDE
	ADC_Sync_Polarity_Override(0);
#endif

#ifdef ADC_EXT_CLAMP_WORKAROUND
//	ADC_Ext_Clamp(0);
#endif



	if(work_pending(&gWorkqueue->ifm))
	{
		ADC_DEBUG("before llpll pending\n");
		return -1;
	}

	if(Kadp_Index > -1)
	{

		if ( lx_chip_rev( ) <= LX_CHIP_REV( L8, B0 ) )
		{
			if(TB_PC[ConvertTable[Kadp_Index]] == 0) // Component Input
			{
				if(TB_WIDTH[ConvertTable[Kadp_Index]] < 1024) // for SD Video
				{
					ADC_Set_Clamp(0, 0x10, 0);
					ADC_Set_Coast(0x0a, 0x1d);		// For Macrovision
					//ADC_Set_Coast(0x08, 0x08);		// For Macrovision
				}
				else
				{
					ADC_Set_Clamp(0, 0x40, 0);
					ADC_Set_Coast(0x8, 0x8);
				}

			}
			else
				ADC_Set_Clamp(0, 0x10, 0);
		}
		else //For L9 clamp position
		{
			if(TB_PC[ConvertTable[Kadp_Index]] == 0) // Component Input
			{
				if(TB_WIDTH[ConvertTable[Kadp_Index]] < 1024) // for SD Video
					ADC_Set_Blank(0x20); // added by won.hur(won.hur@lge.com) @2011.08.17
					//ADC_L9Ax_Set_Blank(0x20);
				else
					ADC_Set_Blank(0x40); // added by won.hur(won.hur@lge.com) @2011.08.17
					//ADC_L9Ax_Set_Blank(0x40);
				/*
				// TODO:dws For L9A0 Clamping
				if(ConvertTable[Kadp_Index] == 303)	// component 480i
					ADC_L9Ax_Set_Blank(0x80);
				else if(ConvertTable[Kadp_Index] == 304)	// Component 480P
					ADC_L9Ax_Set_Blank(0x40);
				else if(ConvertTable[Kadp_Index] == 305)	// Component 576i
					ADC_L9Ax_Set_Blank(0x70);
				else if(ConvertTable[Kadp_Index] == 307)	// Component 576P
					ADC_L9Ax_Set_Blank(0x30);
				else
					ADC_L9Ax_Set_Blank(0x60);
					*/
			}
			else		// For RGB
				ADC_Set_Blank(0x20); // added by won.hur(won.hur@lge.com) @2011.08.17
				//ADC_L9Ax_Set_Blank(0x20);

		}

		if(gPrevious_index != Kadp_Index)
		{
#ifdef ADC_MSPG_114_TIMING_WORKAROUND
			if(Kadp_Index == 76)	// 1600*1024 59.7Hz
				ADC_Set_Hsync_Sel(0) ;
			else if (gADC_InputSrc == LX_ADC_INPUT_SOURCE_RGB_PC) // For Other RGB PC mode
				ADC_Set_Hsync_Sel(1) ; // Select feedback LLPLL Hsync for digital block
			else
				ADC_Set_Hsync_Sel(0) ; // For Component , do not use feedback hsync (20110222)
#endif
			//	ADC_Reset_LLPLL();
			if(ADC_SetInputVideo(ConvertTable[Kadp_Index]) == 0)
				gPrevious_index = Kadp_Index;
			else
				gPrevious_index = 0xffff;

			if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
			{
				//ADC_Reset_LLPLL();
		//		ADC_Reset_Digital();
			}
		}
		else
		{
			ADC_DEBUG("SAME Index again, gPrevious_index = [%d], Kadp_Index = [%d]\n", gPrevious_index, Kadp_Index);

			if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
			{
				//ADC_Reset_LLPLL();
		//		ADC_Reset_Digital();
			}

			//ADC_SetInputVideo(ConvertTable[Kadp_Index]);
		}


		AFE_PRINT("Setting done index [%d] [%d] size=[%dx%d %d Hz] P(1)/I(0)=[%d]\n", Kadp_Index , ConvertTable[Kadp_Index], TB_WIDTH[ConvertTable[Kadp_Index]], TB_HEIGHT[ConvertTable[Kadp_Index]], TB_FV[ConvertTable[Kadp_Index]]/1000, TB_PI[ConvertTable[Kadp_Index]] );


		memcpy((void *)&g_SignalInform , (void *)&inform , sizeof(S_VIDEO_SIZE_INFORM));
#ifdef USE_ADC_TIMER
		memcpy((void *)&previous_size_inform, (void *)&inform , sizeof(S_VIDEO_SIZE_INFORM));
#endif

	}
	else
	{
		memcpy((void *)&g_SignalInform , (void *)&inform , sizeof(S_VIDEO_SIZE_INFORM));
#ifdef USE_ADC_TIMER
		memcpy((void *)&previous_size_inform, (void *)&inform , sizeof(S_VIDEO_SIZE_INFORM));
#endif
		ADC_DEBUG("ext_vsize : %ld, vd_vsize : %ld, vd_hsize : %ld\n", g_SignalInform.ext_vsize, g_SignalInform.vd_vsize, g_SignalInform.vd_hsize);

		ADC_DEBUG("%s can't find index\n" , __func__);
		ret = -1;
	}
#ifdef ADC_EXT_CLAMP_WORKAROUND
	OS_MsecSleep(30); //at 1124 Workaround for video horizontal line artifact when 1080P 24Hz to 1080i 50Hz conversion on divx

	if(( Kadp_Index > -1 ) && (TB_PC[ConvertTable[Kadp_Index]] == 0)) // Component Input
	{
		ADC_Check_Htotal_Diff(5, Kadp_Index);
	}

	ADC_Ext_Clamp(0);
#endif

#ifdef L8_B0_SCART_FB_EN_WORKAROUND
		if ( lx_chip_rev( ) == LX_CHIP_REV( L8, B0 ) )
		{
			if((CVD_L8Bx_Set_Scart_FB_En()) &&(gADC_InputSrc == LX_ADC_INPUT_SOURCE_RGB_SCART)) // Scart RGB Source
			{
				CVD_Set_Scart_Overlay(FALSE);
				OS_MsecSleep(100);

				// Check if source change occured during mdelay
				if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_RGB_SCART) // Scart RGB Source
				{
					CVD_Set_Scart_Overlay(TRUE);
					AFE_PRINT("Clear and Set reg_fb_en\n");
				}
			}
		}
#endif

	OS_MsecSleep(100);

#ifdef ADC_COMP_NO_SYNC_WORKAOURND
	if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
	{
		for(check_count = 0; check_count < 10; check_count ++)
		{
			if(ADC_CVI_Sync_Exist() < 1) // H/V No sync
			{
				ADC_DEBUG("!!! CVI No Sync Detected[%d] !!!\n", check_count);
				ADC_Reset_Digital();
				OS_MsecSleep(100);
			}
			else
				break;
		}
	}
#endif

#ifdef L8_PHASE_NOISE_WORKAROUND
	if(Kadp_Index > -1)
	{
		if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR) // For Component Input
		{
			hstart_ap = (gPCTable+Kadp_Index)->u16_HStart;
			vstart_ap = (gPCTable+Kadp_Index)->u16_VStart;

			hactive_ap = LX_Default_Resolution[(gPCTable+Kadp_Index)->u8_Res_Idx].u16_Width;
			vactive_ap = LX_Default_Resolution[(gPCTable+Kadp_Index)->u8_Res_Idx].u16_Height;

			ADC_DEBUG("HS [%d], VS [%d], HA[%d], VA[%d]\n", hstart_ap, vstart_ap, hactive_ap, vactive_ap);

			size_ap.width_r = hstart_ap;
			size_ap.width_g = hstart_ap;
			size_ap.width_b = hstart_ap;

			size_ap.height_r = vstart_ap;
			size_ap.height_g = vstart_ap;
			size_ap.height_b = vstart_ap;

			pos_ap.width_r = hactive_ap;
			pos_ap.width_g = hactive_ap;
			pos_ap.width_b = hactive_ap;

			if(TB_PI[ConvertTable[Kadp_Index]] > 0)		// Progressive
			{
				pos_ap.height_r = vactive_ap;
				pos_ap.height_g = vactive_ap;
				pos_ap.height_b = vactive_ap;
			}
			else
			{
				pos_ap.height_r = vactive_ap/2;
				pos_ap.height_g = vactive_ap/2;
				pos_ap.height_b = vactive_ap/2;
			}

			ADC_SetRGBSize(&size_ap);
			ADC_SetRGBPosition(&pos_ap);

			gNew_Component_Input_Detected = 1;
		}
	}
#endif

	gADC_IN_IFM = 0;

	*iIndex = Kadp_Index;

#ifdef L8_COMP_HSYNC_PHASE_SEARCH
	if(Kadp_Index > -1)
	{
		if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR) // For Component Input
		{
			//ADC_PHASE_Mesurement_for_Component_HSync(ConvertTable[Kadp_Index]);
			//ADC_PHASE_Mesurement_for_Component_HSync_Sampling(ConvertTable[Kadp_Index]);
			//ADC_PHASE_Mesurement_for_Component_HSync_Fast(ConvertTable[Kadp_Index]);
		}
	}
#endif


	return ret;

}

int ADC_IFM_GetInputSignal(S_VIDEO_SIZE_INFORM *inform)
{
	memcpy((void *)inform , (void *)&g_SignalInform , sizeof(S_VIDEO_SIZE_INFORM));

	return 0;
}

int ADC_Set_PCMODE_Resoultion(UINT32 arg)
{
	//int unused;

	g_User_Resolution = (LX_AFE_RESOLUTION_TYPE_T)arg;

	AFE_PRINT("Set User PCMODE Resolution to [%d]\n", g_User_Resolution);

	//ADC_IFM_ISR((void *)&unused);
	ADC_Clear_Previous_Size_Info();

	return 0;

}

int ADC_IFM_Routine(S_VIDEO_SIZE_INFORM *p_adc_size_info)
{
	int Kadp_Index = 0;
	ST_EXTAU 	extau;

	Kadp_Index = ADC_IFM_SearchTable(p_adc_size_info);

	if(Kadp_Index > -1)
	{

		if ( lx_chip_rev( ) <= LX_CHIP_REV( L8, B0 ) )
		{
			if(TB_PC[ConvertTable[Kadp_Index]] == 0) // Component Input
			{
				if(TB_WIDTH[ConvertTable[Kadp_Index]] < 1024) // for SD Video
				{
					ADC_Set_Clamp(0, 0x10, 0);
					ADC_Set_Coast(0x0a, 0x1d);		// For Macrovision
					//ADC_Set_Coast(0x08, 0x08);		// For Macrovision
				}
				else
				{
					ADC_Set_Clamp(0, 0x40, 0);
					ADC_Set_Coast(0x8, 0x8);
				}

			}
			else
				ADC_Set_Clamp(0, 0x10, 0);
		}
		else //For L9 clamp position
		{
			if(TB_PC[ConvertTable[Kadp_Index]] == 0) // Component Input
			{
				if(TB_WIDTH[ConvertTable[Kadp_Index]] < 1024) // for SD Video
					ADC_Set_Blank(0x20); // added by won.hur(won.hur@lge.com) @2011.08.17
					//ADC_L9Ax_Set_Blank(0x20);
				else
					ADC_Set_Blank(0x40); // added by won.hur(won.hur@lge.com) @2011.08.17
					//ADC_L9Ax_Set_Blank(0x40);
			}
			else		// For RGB
				ADC_Set_Blank(0x20); // added by won.hur(won.hur@lge.com) @2011.08.17
				//ADC_L9Ax_Set_Blank(0x20);

		}

		// Set Ctl_Add valut to 0x13 : 111101
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
		{
			//if(TB_PC[ConvertTable[Kadp_Index]] == 0) // Component Input
			if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR) // YPbPr Source
			{
				if(TB_WIDTH[ConvertTable[Kadp_Index]] < 1024) // for SD Video
				{
					//120207 : use internal hsync for 480/576 I/P Component
					ADC_L9Bx_Set_Hsync_Sel(0);

					if(TB_PI[ConvertTable[Kadp_Index]] == 0)	// for 480i/576i
					{
						ADC_L9Bx_Set_Ctl_Add(0x0, 0x0, 0x2, 0x4);	// by gogosing for shibasoku 33% level
						// 111123 wonsik.do : for post gain control for 110% white saturation test.
						ADC_L9Bx_Enable_Sync_Low_Level_Read(1);

						//ADC_L9Bx_Set_Ctl_Add(0x0, 0x0, 0x2, 0x5);
					}
					else										// for 480P/576P
						ADC_L9Bx_Set_Ctl_Add(0x0, 0x0, 0x2, 0x4);
				}
				else{	//for 720P, 1080i/P
						ADC_L9Bx_Set_Ctl_Add(0x0, 0x0, 0x2, 0x3);

					//120207 : use feedback hsync for 720P/1080IP Component Video
					ADC_L9Bx_Set_Hsync_Sel(1);
				}
			}
		}


		if(gPrevious_index != Kadp_Index)
		{
			//	ADC_Reset_LLPLL();
			if(ADC_SetInputVideo(ConvertTable[Kadp_Index]) == 0)
				gPrevious_index = Kadp_Index;
			else
				gPrevious_index = 0xffff;

			if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) ) {
				// for MSPG 68 timing (1152*720) : sometimes LLPLL unlock occur
				ADC_L9Bx_Check_LLPLL_status();
			}

			AFE_PRINT("Setting done index [%d] [%d] size=[%dx%d %d Hz] P(1)/I(0)=[%d]\n", Kadp_Index , ConvertTable[Kadp_Index], TB_WIDTH[ConvertTable[Kadp_Index]], TB_HEIGHT[ConvertTable[Kadp_Index]], TB_FV[ConvertTable[Kadp_Index]] /1000 , TB_PI[ConvertTable[Kadp_Index]] );
		}
		else
		{
			ADC_DEBUG("SAME Index again, gPrevious_index = [%d], Kadp_Index = [%d]\n", gPrevious_index, Kadp_Index);

			if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) ) {
				// for MSPG 68 timing (1152*720) : sometimes LLPLL unlock occur when change to same resolution 
				ADC_L9Bx_Check_LLPLL_status();
			}

		}

		// Set gResolution Index to New Index found.
		gResolution = Kadp_Index;

		memcpy((void *)&g_SignalInform , (void *)p_adc_size_info , sizeof(S_VIDEO_SIZE_INFORM));

		extau.extau_up = (gPCTable+gResolution)->u16_HStart;
		extau.extau_left = (gPCTable+gResolution)->u16_VStart;

		ADC_IBD_Setting(&extau , gResolution);

		//Component Phase setting is done at ADC_L9Bx_Component_APA()
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
		{
			ADC_L9Bx_Component_APA(ConvertTable[Kadp_Index]);
		}
		else
		{

#ifndef L8_COMP_HSYNC_PHASE_SEARCH
			if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR) // YPbPr Source
				ADC_SetPhaseValue((gPCTable+gResolution)->u16_ADC_Phase); // by dws
#endif
		}
		//at 1124 Workaround for video horizontal line artifact when 1080P 24Hz to 1080i 50Hz conversion on divx
#ifdef ADC_PERIODIC_HTOTAL_READ
		if(( Kadp_Index > -1 ) && (TB_PC[ConvertTable[Kadp_Index]] == 0)) // Component Input
		{
			OS_MsecSleep(30);
			ADC_Check_Htotal_Diff(5, Kadp_Index);
		}
#endif
		return 0;

	}
	else
	{
		// Set gResolution Index to -1 (Resolution not found).
		gResolution = Kadp_Index;

		memcpy((void *)&g_SignalInform , (void *)p_adc_size_info , sizeof(S_VIDEO_SIZE_INFORM));
		ADC_DEBUG("ext_vsize : %ld, vd_vsize : %ld, vd_hsize : %ld\n", g_SignalInform.ext_vsize, g_SignalInform.vd_vsize, g_SignalInform.vd_hsize);

		ADC_DEBUG("%s can't find index\n" , __func__);

		return -1;
	}

}
