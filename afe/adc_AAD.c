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

/*! \file ADC_AutoAdjustfunc.c
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
#include <linux/delay.h>	/** for mdelay	*/

#include "os_util.h"
#include "afe_drv.h"

#include <linux/interrupt.h>
#include "afe_kapi.h"
#include "vport_reg.h"
 
//#include "vport_reg.h"
#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"
//#include "vport_reg_B0.h"

/******************************************************************************
 *				DEFINES
 *****************************************************************************/


/**
 *	Global variables of the driver
 */
//extern volatile VPORT_Reg_T *gVportReg;
//extern volatile VPORT_RegBx_T *gVportRegBx;

/******************************************************************************
 *				DATA STRUCTURES
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions 
 */
extern ST_WORK_QUEUE	*gWorkqueue;

typedef struct
{
	unsigned int uiWidthMaxR;
	unsigned int uiWidthMinR;
	unsigned int uiHeightMaxR;
	unsigned int uiHeightMinR;	
	
	unsigned int uiWidthMaxG;
	unsigned int uiWidthMinG;
	unsigned int uiHeightMaxG;
	unsigned int uiHeightMinG;	
	
	unsigned int uiWidthMaxB;
	unsigned int uiWidthMinB;
	unsigned int uiHeightMaxB;
	unsigned int uiHeightMinB;	
	
}ST_SCREEN_AREA_INFO;

/******************************************************************************
 *				Local function
 *****************************************************************************/
/**
 *	Structure that declares the usual file
 *	access functions 
 */

/**
 *
 * search phase
 *
 * @param	LX_AFE_ADC_SET_INPUT_T
 * @return	0 : OK , -1 : NOK
 *
*/	
int ADC_PHASE_Mesurement(int iIndex)
{

	ST_EXTAU *extau;
//	int i,j, loop, waiting_time = 20; //powerory 2010-10-05
	int i,j, loop, waiting_time = 10; //powerory 2011-03-14
	int iMax = 0;
	ST_SCREEN_AREA_INFO sScreen, sScreenConst, sScreenBackup;
	S_RGB_POSITION *pos;
	S_RGB_SIZE *size;
	S_RGB_OUT *result;

	//0325
	//S_RGB_SUM *sum;

//	unsigned int uiWidthStepR, uiWidthStepG, uiWidthStepB;
//	unsigned int uiHeightStepR, uiHeightStepG, uiHeightStepB;
	
//	unsigned int uiOutR, uiOutG, uiOutB;
	unsigned int uiOutMaxR = 0, uiOutMaxG = 0, uiOutMaxB = 0;
	
	unsigned int uiWidthMaxR = 0, uiWidthMaxG = 0,uiWidthMaxB = 0;
	unsigned int uiHeightMaxR = 0, uiHeightMaxG = 0,uiHeightMaxB = 0;
	
	
	pos = (S_RGB_POSITION *)kmalloc(sizeof(S_RGB_POSITION),GFP_KERNEL);
	size = (S_RGB_SIZE *)kmalloc(sizeof(S_RGB_SIZE),GFP_KERNEL);
	result = (S_RGB_OUT *)kmalloc(sizeof(S_RGB_OUT),GFP_KERNEL);
	
	//LLPLL_shift 일단 0으로 초기화
	ADC_SetPhaseValue(0);
	
	
	extau = kmalloc(sizeof(ST_EXTAU) , GFP_KERNEL);
	ADC_IBD_Mesurement(extau , iIndex);

	sScreen.uiWidthMinR = extau->extau_left + PHASE_METHOD2_OFFSET;
	sScreen.uiHeightMinR = extau->extau_up +  PHASE_METHOD2_OFFSET;
	sScreen.uiWidthMaxR = extau->extau_right - PHASE_METHOD2_OFFSET;
	sScreen.uiHeightMaxR = extau->extau_down - PHASE_METHOD2_OFFSET;

	sScreen.uiWidthMinG = extau->extau_left + PHASE_METHOD2_OFFSET;
	sScreen.uiHeightMinG = extau->extau_up +  PHASE_METHOD2_OFFSET;
	sScreen.uiWidthMaxG = extau->extau_right - PHASE_METHOD2_OFFSET;
	sScreen.uiHeightMaxG = extau->extau_down - PHASE_METHOD2_OFFSET;

	sScreen.uiWidthMinB = extau->extau_left + PHASE_METHOD2_OFFSET;
	sScreen.uiHeightMinB = extau->extau_up +  PHASE_METHOD2_OFFSET;
	sScreen.uiWidthMaxB = extau->extau_right - PHASE_METHOD2_OFFSET;
	sScreen.uiHeightMaxB = extau->extau_down - PHASE_METHOD2_OFFSET;

	AFE_PRINT("extau up : %d , down : %d , left : %d , right : %d\n",
		extau->extau_up , extau->extau_down , extau->extau_left , extau->extau_right);


	// initial value backup
	memcpy(&sScreenConst , &sScreen , sizeof(ST_SCREEN_AREA_INFO));
	
	// why be necessary 3 times loop
	for(loop=0;loop<1;loop++){
		size->width_r = (sScreen.uiWidthMaxR - sScreen.uiWidthMinR)/PHASE_POS_X_STEP;
		size->height_r = (sScreen.uiHeightMaxR - sScreen.uiHeightMinR)/PHASE_POS_Y_STEP;
		size->width_g = (sScreen.uiWidthMaxG - sScreen.uiWidthMinG)/PHASE_POS_X_STEP;
		size->height_g = (sScreen.uiHeightMaxG - sScreen.uiHeightMinG)/PHASE_POS_Y_STEP;
		size->width_b = (sScreen.uiWidthMaxB - sScreen.uiWidthMinB)/PHASE_POS_X_STEP;
		size->height_b = (sScreen.uiHeightMaxB - sScreen.uiHeightMinB)/PHASE_POS_Y_STEP;

//	AFE_PRINT("SIZE WIDTH - R[%d] G[%d] B[%d], HEIGHT - R[%d] G[%d] B[%d]\n", size->width_r , size->width_g , size->width_b , size->height_r , size->height_g , size->height_b);

	
		ADC_SetRGBSize(size);
	
		memcpy(&sScreenBackup , &sScreen , sizeof(ST_SCREEN_AREA_INFO));
		
		for(i=0; i<PHASE_POS_X_STEP; i++){
			
			//ap_xpos_b
			pos->width_b = sScreen.uiWidthMinB + i*size->width_b;
			
			//ap_xpos_g
			pos->width_g = sScreen.uiWidthMinG + i*size->width_g;
			
			//ap_xpos_r
			pos->width_r = sScreen.uiWidthMinR + i*size->width_r;

			for(j=0; j<PHASE_POS_Y_STEP; j++){
				//ap_ypos_b
				pos->height_b = sScreen.uiHeightMinB + j*size->height_b;
				
				//ap_ypos_g
				pos->height_g = sScreen.uiHeightMinG + j*size->height_g;
				
				//ap_ypos_r
				pos->height_r = sScreen.uiHeightMinR + j*size->height_r;
//AFE_PRINT("%d @ %s ij[%d][%d] POS WIDTH - R[%d] G[%d] B[%d], HEIGHT - R[%d] G[%d] B[%d]\n", __LINE__ , __func__ , i,j,pos->width_r , pos->width_g , pos->width_b , pos->height_r , pos->height_g , pos->height_b);
				
				ADC_SetRGBPosition(pos);



				msleep(waiting_time);

				ADC_GetRGBResult(result);

				ADC_GetRGBPosition(pos);
//AFE_PRINT("%d @ %s ij[%d][%d] POS WIDTH - R[%d] G[%d] B[%d], HEIGHT - R[%d] G[%d] B[%d]\n", __LINE__ , __func__ , i,j,pos->width_r , pos->width_g , pos->width_b , pos->height_r , pos->height_g , pos->height_b);


//AFE_PRINT("%d @ %s ij[%d][%d] RESULT R[%d] G[%d] B[%d]\n", __LINE__ , __func__ , i,j,result->out_r , result->out_g , result->out_b );


				if(uiOutMaxR < result->out_r)
				{
					uiWidthMaxR = pos->width_r;
					uiHeightMaxR = pos->height_r;
					uiOutMaxR = result->out_r;
				}
				if(uiOutMaxG < result->out_g)
				{
					uiWidthMaxG = pos->width_g;
					uiHeightMaxG = pos->height_g;
					uiOutMaxG = result->out_g;
				}
				if(uiOutMaxB < result->out_b)
				{
					uiWidthMaxB = pos->width_b;
					uiHeightMaxB = pos->height_b;
					uiOutMaxB = result->out_b;
				}		
				
				
			//	AFE_PRINT("loop[%d][%d] x:%04d-%4d, y:%04d-%04d\n", i,j,uiWidthMaxR, uiWidthMaxR+size->width_r, uiHeightMaxR, uiHeightMaxR+size->height_r);
				
			//	AFE_PRINT("loop[%d][%d] x:%04d-%4d, y:%04d-%04d\n", i,j,uiWidthMaxG, uiWidthMaxG+size->width_g, uiHeightMaxG, uiHeightMaxG+size->height_g);
				
			//	AFE_PRINT("loop[%d][%d] x:%04d-%4d, y:%04d-%04d\n", i,j,uiWidthMaxB, uiWidthMaxB+size->width_b, uiHeightMaxB, uiHeightMaxB+size->height_b);
				
				
			} 
		}
		
		
		
		sScreen.uiWidthMinR = uiWidthMaxR - (int)(size->width_r/5.0);
		sScreen.uiWidthMaxR = uiWidthMaxR + (int)(size->width_r*1.6);
		sScreen.uiWidthMinG = uiWidthMaxG - (int)(size->width_g/5.0);
		sScreen.uiWidthMaxG = uiWidthMaxG + (int)(size->width_g*1.6);
		sScreen.uiWidthMinB = uiWidthMaxB - (int)(size->width_b/5.0);
		sScreen.uiWidthMaxB = uiWidthMaxB + (int)(size->width_b*1.6);
	
		if(sScreen.uiWidthMinR < sScreenConst.uiWidthMinR) sScreen.uiWidthMinR = sScreenConst.uiWidthMinR;
		if(sScreen.uiWidthMaxR > sScreenConst.uiWidthMaxR) sScreen.uiWidthMaxR = sScreenConst.uiWidthMaxR;
		if(sScreen.uiWidthMinG < sScreenConst.uiWidthMinG) sScreen.uiWidthMinG = sScreenConst.uiWidthMinG;
		if(sScreen.uiWidthMaxG > sScreenConst.uiWidthMaxG) sScreen.uiWidthMaxG = sScreenConst.uiWidthMaxG;
		if(sScreen.uiWidthMinB < sScreenConst.uiWidthMinB) sScreen.uiWidthMinB = sScreenConst.uiWidthMinB;
		if(sScreen.uiWidthMaxB > sScreenConst.uiWidthMaxB) sScreen.uiWidthMaxB = sScreenConst.uiWidthMaxB;

		sScreen.uiHeightMinR = uiHeightMaxR - (int)(size->height_r/5.0);
		sScreen.uiHeightMaxR = uiHeightMaxR + (int)(size->height_r*1.6);
		sScreen.uiHeightMinG = uiHeightMaxG - (int)(size->height_g/5.0);
		sScreen.uiHeightMaxG = uiHeightMaxG + (int)(size->height_g*1.6);
		sScreen.uiHeightMinB = uiHeightMaxB - (int)(size->height_b/5.0);
		sScreen.uiHeightMaxB = uiHeightMaxB + (int)(size->height_b*1.6);
	
		if(sScreen.uiHeightMinR < sScreenConst.uiHeightMinR) sScreen.uiHeightMinR = sScreenConst.uiHeightMinR;
		if(sScreen.uiHeightMaxR > sScreenConst.uiHeightMaxR) sScreen.uiHeightMaxR = sScreenConst.uiHeightMaxR;
		if(sScreen.uiHeightMinG < sScreenConst.uiHeightMinG) sScreen.uiHeightMinG = sScreenConst.uiHeightMinG;
		if(sScreen.uiHeightMaxG > sScreenConst.uiHeightMaxG) sScreen.uiHeightMaxG = sScreenConst.uiHeightMaxG;
		if(sScreen.uiHeightMinB < sScreenConst.uiHeightMinB) sScreen.uiHeightMinB = sScreenConst.uiHeightMinB;
		if(sScreen.uiHeightMaxB > sScreenConst.uiHeightMaxB) sScreen.uiHeightMaxB = sScreenConst.uiHeightMaxB;


		if((size->width_r == 1 && size->height_r == 1) && (size->width_g == 1 && size->height_g == 1)  && 
			(size->width_b == 1 && size->height_b == 1) )
		{
			break;
		}


		if(memcmp(&sScreen , &sScreenBackup , sizeof(ST_SCREEN_AREA_INFO)) == 0)
		{
			break;	
		}

	} //loop

	// ap_xpos_b
	pos->width_b = uiWidthMaxB;
	// ap_xpos_g
	pos->width_g = uiWidthMaxG;
	// ap_xpos_r
	pos->width_r = uiWidthMaxR;
	
	//ap_ypos_b
	pos->height_b = uiHeightMaxB;
	//ap_ypos_g
	pos->height_g = uiHeightMaxG;
	//ap_ypos_r
	pos->height_r = uiHeightMaxR;


	AFE_PRINT("%s xpos_r : %d , xpos_g : %d , xpos_b : %d\n", __func__ , uiWidthMaxR , uiWidthMaxG ,uiWidthMaxB );
	AFE_PRINT("%s ypos_r : %d , ypos_g : %d , ypos_b : %d\n", __func__ , uiHeightMaxR , uiHeightMaxG ,uiHeightMaxB );
	
	ADC_SetRGBPosition(pos);
	


	// PHASE 돌아가면서 확정하기 Method 1 : 간단하게 dev 최고값
	{
		//120103 wonsik.do : removed floating point in kernel driver
		//float dMax = -1.0;
		UINT64 dMax = 0;

		int iPhaseMin =0 ;
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) * 1.5);
		//120103 wonsik.do : removed floating point in kernel driver
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) +2); //0918 by powerory
		int iPhaseMax = (1000000000 / (TB_FS[iIndex] * 130) +2); //0918 by powerory
		int iPhaseMaxConst = iPhaseMax;
		int iPhaseOffset = PHASE_OFFSET ;
		//120103 wonsik.do : removed floating point in kernel driver
		//float dOutAvg;
		UINT64 dOutAvg;
		//int out_g_old = 0;

		AFE_PRINT("%d @ %s phase max %d\n", __LINE__ , __func__ , iPhaseMax );
	
	
		for(loop=0;loop<1;loop++)
		{
			int iPhaseStep = (iPhaseMax-iPhaseMin) / PHASE_STEP + 1;  
			int iMaxBackup = iMax;
			for(i=iPhaseMin;i<=iPhaseMax;i+=iPhaseStep)		// risky
			{
				//LLPLL_shift
				ADC_SetPhaseValue(i);
				
				msleep(waiting_time);
	
				ADC_GetRGBResult(result);

//AFE_PRINT("%s loop[%2d] PHASE %3d RESULT R[%10d] G[%10d] B[%10d] gap[%10d]\n", __func__ , loop ,i,result->out_r , result->out_g , result->out_b, abs(result->out_g - out_g_old) );
//                out_g_old = result->out_g;

				//0325
//				sum = (S_RGB_SUM *)kmalloc(sizeof(S_RGB_SUM),GFP_KERNEL);
//				ADC_GetSumRGB(sum);

//AFE_PRINT("%d @ %s loop[%02d] PHASE %02d SUM R[%010d] G[%010d] B[%010d]\n", __LINE__ , __func__ , loop ,i,sum->sum_R , sum->sum_G , sum->sum_B );


				//120103 wonsik.do : removed floating point in kernel driver
				//dOutAvg = (float)(result->out_r + result->out_g + result->out_b ) / 3.0 ;
				dOutAvg = result->out_r + result->out_g + result->out_b;
				if(dOutAvg > dMax) {
					dMax = dOutAvg;
					iMax = i;
				}
				//AFE_PRINT("%d/%d\n", i, iPhaseMaxConst);
				
			}
			iPhaseMin = iMax - iPhaseStep * iPhaseOffset;
			iPhaseMax = iMax + iPhaseStep * (1+iPhaseOffset) ;

AFE_PRINT("Phase max %d , Phase min %d\n", iPhaseMax, iPhaseMin);
	
			if(iPhaseMin < 0) iPhaseMin =0;
			if(iPhaseMax > iPhaseMaxConst) iPhaseMax = iPhaseMaxConst;
	
			if(iPhaseStep == 1 ) break;
			if(iMax == iMaxBackup) break;
		}
	}
	

	
	kfree(extau);
	
	
	kfree(pos);
	kfree(size);
	kfree(result);
//	kfree(sum);

	return iMax;

}

/**
 *
 * search phase for hsize detection
 *
 * @param	: iIndex
 * @return	: deviation value
 *
*/	
UINT64 ADC_PHASE_Mesurement_Simple(int iIndex, UINT16 htotal)
{

	int i, loop, waiting_time = 20;// 110819 by wonsik.do to speed-up auto config //50; //powerory 2010-10-05
	int iMax = 0;
	S_RGB_POSITION *pos;
	S_RGB_SIZE *size;
	S_RGB_OUT *result;
	//120103 wonsik.do : removed floating point in kernel driver
	//float dMax = -1.0;
	UINT64 dMax = 0;

	
	pos = (S_RGB_POSITION *)kmalloc(sizeof(S_RGB_POSITION),GFP_KERNEL);
	size = (S_RGB_SIZE *)kmalloc(sizeof(S_RGB_SIZE),GFP_KERNEL);
	result = (S_RGB_OUT *)kmalloc(sizeof(S_RGB_OUT),GFP_KERNEL);
	
	//LLPLL_shift 일단 0으로 초기화
	ADC_SetPhaseValue(0);

	//powerory
	size->width_r  = htotal;
	size->height_r = TB_SD_EXT_VSIZE[iIndex];
	size->width_g  = htotal;
	size->height_g = TB_SD_EXT_VSIZE[iIndex];
	size->width_b  = htotal;
	size->height_b = TB_SD_EXT_VSIZE[iIndex];
	
	ADC_SetRGBSize(size);
	
	pos->width_b  = 0;
	pos->width_g  = 0;
	pos->width_r  = 0;
	pos->height_b = 0;
	pos->height_g = 0;
	pos->height_r = 0;

	ADC_SetRGBPosition(pos);

	// PHASE 돌아가면서 확정하기 Method 1 : 간단하게 dev 최고값
	{

		int iPhaseMin =0 ;
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) * 1.5);
		//120103 wonsik.do : removed floating point in kernel driver
		int iPhaseMax = (1000000000/ (TB_FS[iIndex] * 130) + 2); //0918 by powerory
		int iPhaseMaxConst = iPhaseMax;
		int iPhaseOffset = PHASE_OFFSET_SIMPLE ;
		//120103 wonsik.do : removed floating point in kernel driver
		//float dOutAvg;
		UINT64 dOutAvg;
		
		//AFE_PRINT("%d @ %s phase max %d\n", __LINE__ , __func__ , iPhaseMax );
	
		for(loop=0;loop<1;loop++)
		{
			int iPhaseStep = (iPhaseMax-iPhaseMin) / PHASE_STEP_SIMPLE + 1;  
			int iMaxBackup = iMax;
			for(i=iPhaseMin;i<=iPhaseMax;i+=iPhaseStep)		// risky
			{
				//LLPLL_shift
				ADC_SetPhaseValue(i);
				
				msleep(waiting_time);
	
				ADC_GetRGBResult(result);

//AFE_PRINT("%d @ %s loop[%d] PHASE %d RESULT R[%d] G[%d] B[%d]\n", __LINE__ , __func__ , loop ,i,result->out_r , result->out_g , result->out_b );
	
				//120103 wonsik.do : removed floating point in kernel driver
				dOutAvg = result->out_r + result->out_g + result->out_b;
				if(dOutAvg > dMax) {
					dMax = dOutAvg;
					iMax = i;
				}
				//AFE_PRINT("%d/%d\n", i, iPhaseMaxConst);
			}
			iPhaseMin = iMax - iPhaseStep * iPhaseOffset;
			iPhaseMax = iMax + iPhaseStep * (1+iPhaseOffset) ;

//AFE_PRINT("Phase max %d , Phase min %d\n", iPhaseMax, iPhaseMin);
	
			if(iPhaseMin < 0) iPhaseMin =0;
			if(iPhaseMax > iPhaseMaxConst) iPhaseMax = iPhaseMaxConst;
	
			if(iPhaseStep == 1 ) break;
			if(iMax == iMaxBackup) break;
		}
	}
	
	kfree(pos);
	kfree(size);
	kfree(result);

	return dMax;

}
/**
 *
 * set phase which came from ADC_PHASE_Mesurement
 *
 * @param	LX_AFE_ADC_SET_INPUT_T
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_PHASE_Setting(int iMaxPll)
{

	//LLPLL_shift
	ADC_SetPhaseValue((unsigned int)iMaxPll);

	return 0;
}



int ADC_PHASE_main(int iIndex)
{
	int ret;
	
//	AFE_PRINT("%s entered with index[%d]\n", __func__ , iIndex);
	
	if(iIndex == -1)
	{
		AFE_PRINT("not be set resolutuin yet\n\n");
		return -1;
	}
	
	ret = ADC_PHASE_Mesurement(ConvertTable[iIndex]); //by dws 0809

	AFE_PRINT("## AutoPhase Result is [%d][0x%x]\n", ret, ret);

	if(ret < 0)
	{
		AFE_ERROR("return value is %d\n",  ret);
		return -1;
	}
	else
		ADC_PHASE_Setting(ret);
	
	return 0;
	
}


/**
 *
 * search phase for component input
 *
 * @param	: iIndex
 * @return	: deviation value
 *
*/	
int ADC_PHASE_Mesurement_for_Component(int iIndex)
{

	int i, loop, waiting_time = 10;
	int iMax = 0;
	S_RGB_OUT *result;
	//120103 wonsik.do : removed floating point in kernel driver
	//float dMax = -1.0;
	UINT64 dMax = 0;

	
	result = (S_RGB_OUT *)kmalloc(sizeof(S_RGB_OUT),GFP_KERNEL);
	
	//LLPLL_shift 일단 0으로 초기화
	ADC_SetPhaseValue(0);

	// PHASE 돌아가면서 확정하기 Method 1 : 간단하게 dev 최고값
	{

		int iPhaseMin =0 ;
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) * 1.5);
		//120103 wonsik.do : removed floating point in kernel driver
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) + 2); //0918 by powerory
		int iPhaseMax = (1000000000 / (TB_FS[iIndex] * 130) + 2); //0918 by powerory
		int iPhaseMaxConst = iPhaseMax;
		int iPhaseOffset = PHASE_OFFSET_COMPONENT ;
		//120103 wonsik.do : removed floating point in kernel driver
		//float dOutAvg;
		UINT64 dOutAvg;
		
		//AFE_PRINT("%d @ %s phase max %d\n", __LINE__ , __func__ , iPhaseMax );
	
		for(loop=0;loop<1;loop++)
		{
			int iPhaseStep = (iPhaseMax-iPhaseMin) / PHASE_STEP_COMPONENT + 1;  
			int iMaxBackup = iMax;
			for(i=iPhaseMin;i<=iPhaseMax;i+=iPhaseStep)		// risky
			{
				//LLPLL_shift
				ADC_SetPhaseValue(i);
				
				msleep(waiting_time);
	
				ADC_GetRGBResult(result);

				//120103 wonsik.do : removed floating point in kernel driver
				//dOutAvg = (float)(result->out_r + result->out_g + result->out_b ) / 3.0 ;
				dOutAvg = result->out_r + result->out_g + result->out_b;

				ADC_DEBUG("PHASE %3d RESULT R[%7d] G[%7d] B[%7d] Avg[%7d]\n", i,result->out_r , result->out_g , result->out_b, (UINT32)dOutAvg );
	
				if(dOutAvg > dMax) {
					dMax = dOutAvg;
					iMax = i;
				}
				//AFE_PRINT("dOutAvg %d , dMax %d\n", (int)dOutAvg, (int)dMax);

				//AFE_PRINT("%d/%d\n", i, iPhaseMaxConst);
			}
			iPhaseMin = iMax - iPhaseStep * iPhaseOffset;
			iPhaseMax = iMax + iPhaseStep * (1+iPhaseOffset) ;

//AFE_PRINT("Phase max %d , Phase min %d\n", iPhaseMax, iPhaseMin);
	
			if(iPhaseMin < 0) iPhaseMin =0;
			if(iPhaseMax > iPhaseMaxConst) iPhaseMax = iPhaseMaxConst;
	
			if(iPhaseStep == 1 ) break;
			if(iMax == iMaxBackup) break;
		}
	}
	
	kfree(result);

	return (int)iMax;

}


int ADC_PHASE_for_Component(int Kadp_Index)
{
	int ret;
	
	//AFE_PRINT("%s entered with index[%d]\n", __func__ , Kadp_Index);
	
	if(Kadp_Index == -1)
	{
		AFE_PRINT("not be set resolutuin yet\n\n");
		return -1;
	}
	
	ret = ADC_PHASE_Mesurement_for_Component(ConvertTable[Kadp_Index]);
	//ret = ADC_PHASE_Mesurement(ConvertTable[iIndex]);

	AFE_PRINT("## AutoPhase for component Result is [%d][0x%x]\n", ret, ret);

	if(ret < 0)
	{
		AFE_ERROR("return value is %d\n",  ret);
		return -1;
	}
	else
		ADC_PHASE_Setting(ret);
	
	return 0;
	
}
#if 0
/**
 *
 * search phase for component input
 *
 * @param	: iIndex
 * @return	: deviation value
 *
*/	
int ADC_PHASE_Mesurement_for_Component_HSync_Fast(int iIndex)
{

	int iMax = 0;
	S_RGB_OUT *result;
	int phase_value;
	int aogc_inc;
	UINT32 avg_temp, avg_prev;
	int avg_diff;
	UINT32 aogc_counter_max;
	UINT32 aogc_counter_max_selected =0;
	int count;

	UINT32 avg_max;
	int avg_diff_max = 0;
	int aogc_stable_count = 0;
	UINT32 aogc_stable_value = 0;
	int same_value_found = 0;
	int edge_point_found = 0;
	int phase_value_found = -1;

	AFE_PRINT("%s entered with index[%d]\n", __func__ , iIndex);

	result = (S_RGB_OUT *)kmalloc(sizeof(S_RGB_OUT),GFP_KERNEL);

	ADC_Green_Mid_Level_Select(1);
	ADC_SetPhaseValue(0);

	gVportRegBx->top_012.reg_clken_aoc = 1;
	gVportRegBx->ch3_digital_029.ch3reg_aogc_mode = 0;
	//gVportRegBx->ch3_digital_030.ch3reg_aogc_counter_max = 0;
	gVportRegBx->ch3_digital_030.ch3reg_aogc_counter_max =LLPLL_counter_max[iIndex] - HS_Out_Width[iIndex] ;
	gVportRegBx->ch3_digital_030.ch3reg_aogc_sample_count = 0;
	gVportRegBx->ch3_digital_034.ch3reg_aogc_enable_sel = 2;
	gVportRegBx->ch3_digital_034.ch3reg_aogc_src_sel = 1;

	avg_prev = 0;

	for(aogc_inc = 0; aogc_inc < 50;aogc_inc++)
	{
		mdelay(5);
		avg_temp = gVportRegBx->ch3_digital_042.ch3reg_ro_avg_g >> 14;
		aogc_counter_max = gVportRegBx->ch3_digital_030.ch3reg_aogc_counter_max;

		if(avg_prev > 0)
			avg_diff = avg_temp - avg_prev;
		else
			avg_diff = 0;

		AFE_PRINT("aogc_counter_max [%d], aogc_inc [%d], avg_temp[%d], avg_prev[%d], avg_diff[%d]\n", aogc_counter_max, aogc_inc, avg_temp, avg_prev, avg_diff);

		if(abs(avg_diff)< 10)
			aogc_stable_count ++;

		if(aogc_stable_count >3 )
			aogc_stable_value = (avg_temp + avg_prev) /2;

		/*
		if( avg_diff > avg_diff_max )
		{
			avg_diff_max = avg_diff;
			aogc_counter_max_selected = aogc_counter_max;
			AFE_PRINT("sync raising point : aogc_counter_max [%d]\n", aogc_counter_max);
		}
		*/
		if((aogc_stable_value > 0) && (avg_diff < -20) && (aogc_counter_max_selected == 0) )
		{
			aogc_counter_max_selected = aogc_counter_max;
			AFE_PRINT("sync falling point : aogc_counter_max [%d]\n", aogc_counter_max);
			break;
		}

		avg_prev = avg_temp;
		gVportRegBx->ch3_digital_030.ch3reg_aogc_counter_max = aogc_counter_max + 1;
	}

	int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) + 10); //0918 by powerory

	aogc_counter_max_selected--;

	AFE_PRINT("start search from aogc_counter_max [%d]\n", aogc_counter_max_selected);

	aogc_stable_value = aogc_stable_value * 4;
	AFE_PRINT("stable value is [%d]\n", aogc_stable_value);

	for(aogc_inc = 0; aogc_inc < 2;aogc_inc++)
	{
		AFE_PRINT("Set aogc_counter_max to [%d]\n", aogc_counter_max_selected);

		gVportRegBx->ch3_digital_030.ch3reg_aogc_counter_max = aogc_counter_max_selected ;

		int iPhaseStep = (iPhaseMax-0) / PHASE_STEP_COMPONENT + 1;  

		for(phase_value = 0; phase_value < iPhaseMax; phase_value += iPhaseStep)
		{
			ADC_SetPhaseValue(phase_value);
			//	ADC_SetPhaseValue(0);
			msleep(10);

			avg_temp = 0;
			for (count=0; count<4;count++)
			{
				avg_temp += gVportRegBx->ch3_digital_042.ch3reg_ro_avg_g >> 14;
				mdelay(5);
			}

			AFE_PRINT("aogc_counter_max[%d],phase[%3d][0x%3x], avg_temp[%d], avg_prev[%d], diff[%d] \n",aogc_counter_max_selected,  phase_value, phase_value, avg_temp, avg_prev, avg_temp-avg_prev);

			if( (same_value_found == 0) && ((aogc_stable_value - avg_temp) < 8) )
			{
				AFE_PRINT("Same value found, aogc value[%d]\n", avg_temp);
				same_value_found = 1;
			}
			if(( same_value_found > 0) && (aogc_stable_value - avg_temp) > 10)
			{
				AFE_PRINT("Edge point found, aogc value [%d], phase[%d][0x%x]\n", avg_temp, phase_value, phase_value);
				edge_point_found = 1;
				phase_value_found = phase_value;

				break;
			}

			avg_prev = avg_temp;
		}

		aogc_counter_max_selected++;
	}

	if(phase_value_found >= 0)
	{
		AFE_PRINT("Set Phase to [%d][0x%x]\n", phase_value_found, phase_value_found);
		ADC_SetPhaseValue(phase_value_found);
	}


	kfree(result);
	ADC_Green_Mid_Level_Select(0);

	gVportRegBx->top_012.reg_clken_aoc = 0;
	gVportRegBx->ch3_digital_029.ch3reg_aogc_mode = 1;
	gVportRegBx->ch3_digital_030.ch3reg_aogc_counter_max = 0;
	gVportRegBx->ch3_digital_030.ch3reg_aogc_sample_count = 6;
	gVportRegBx->ch3_digital_034.ch3reg_aogc_enable_sel = 0;
	gVportRegBx->ch3_digital_034.ch3reg_aogc_src_sel = 0;

	return (int)iMax;

}

/**
 *
 * search phase for component input
 *
 * @param	: iIndex
 * @return	: deviation value
 *
*/	
int ADC_PHASE_Mesurement_for_Component_HSync(int iIndex)
{

	int iMax = 0;
	S_RGB_OUT *result;
	int x_pos_count;
	int x_pos_max = 20;
	int y_size_g = 10;
	int sum_g_result;
	int sum_g_prev = 0;
	int sum_g_diff = -1;
	int sum_g_diff_prev = -1;
	int x_pos_found = -1;
	int phase_value;
	int max_diff_value = -1;
	int max_phase_value = -1;
	int max_sum_found = -1;
	int sum_g_store = -1;

	S_RGB_POSITION pos_ap;
	S_RGB_SIZE		size_ap;
	S_RGB_SUM	sum;
	
	AFE_PRINT("%s entered with index[%d]\n", __func__ , iIndex);

	result = (S_RGB_OUT *)kmalloc(sizeof(S_RGB_OUT),GFP_KERNEL);

	ADC_Green_Mid_Level_Select(1);

	size_ap.width_r = 0;
	size_ap.width_g = 1;
	size_ap.width_b = 0;

	size_ap.height_r = 0;
	size_ap.height_g = y_size_g;
	size_ap.height_b = 0;

	pos_ap.width_r = 0;
	pos_ap.height_r = 0;
	pos_ap.width_g = LLPLL_counter_max[iIndex] - HS_Out_Width[iIndex] - 12;
	pos_ap.height_g = 0;
	pos_ap.width_b = 0;
	pos_ap.height_b = 0;

	ADC_SetRGBSize(&size_ap);
	ADC_SetRGBPosition(&pos_ap);
	
	AFE_PRINT("HTotal [%d], Hsync Width [%d], Initial Xpos [%d]\n", LLPLL_counter_max[iIndex], HS_Out_Width[iIndex], pos_ap.width_g);

	for(x_pos_count = 0; x_pos_count < x_pos_max; x_pos_count ++)
	{
		//by dws : remove mdelay 
		//mdelay(200);
		OS_MsecSleep(200);
		ADC_GetSumRGB(&sum);
		sum_g_result = sum.sum_G ;
		sum_g_diff = abs(sum_g_result - sum_g_prev);

		AFE_PRINT("X_pos[%d], SUM G[%d], prev sum[%d], sum diff[%d]\n", pos_ap.width_g, sum_g_result, sum_g_prev, sum_g_diff );

		if( (sum_g_result > 0) && (x_pos_found <0))
		{
			if(/* ( (sum_g_result < 4000) && (sum_g_diff >= 0))  ||*/ (/* (sum_g_diff_prev < 10) && (sum_g_diff_prev >= 0) &&*/ (sum_g_diff > 20) && (sum_g_prev >0) ))
			{
				x_pos_found = pos_ap.width_g;
				sum_g_store = sum_g_prev;
				AFE_PRINT("X_pos Found [%d], sum_g_store [%d]\n", x_pos_found, sum_g_store );
				break;
			}

			sum_g_prev = sum_g_result;
			sum_g_diff_prev = sum_g_diff;

			if(work_pending(&gWorkqueue->ifm))
			{
				AFE_PRINT("In Clock Searching pending\n");
				return -1;
			}
		}

		pos_ap.width_g ++;
		ADC_SetRGBPosition(&pos_ap);

	}

	if(x_pos_found <0)
	{
		AFE_PRINT("##### Could Not Find HSync Falling Edge ##### \n");
		return -1;
	}

	//120103 wonsik.do : removed floating point in kernel driver
	int iPhaseMax = (1000000000 / (TB_FS[iIndex] * 130) +2); //0918 by powerory
	sum_g_prev = 0;

	for(x_pos_count = x_pos_found -1;x_pos_count <= (x_pos_found + 1); x_pos_count++)
	{
		max_diff_value = -1;
		sum_g_diff	= -1;

		pos_ap.width_g = x_pos_count;
		ADC_SetRGBPosition(&pos_ap);
		//by dws : remove mdelay 
		//mdelay(100);
		OS_MsecSleep(100);

		max_sum_found = 0;

		for(phase_value = 0; phase_value < iPhaseMax; phase_value ++)
		{
			ADC_SetPhaseValue(phase_value);
		//	ADC_SetPhaseValue(0);
			msleep(100);
			ADC_GetSumRGB(&sum);

			sum_g_result = sum.sum_G;
			sum_g_diff = abs(sum_g_prev - sum_g_result);

			AFE_PRINT("X_pos[%d], Phase [%3d], SUM G[%5d], diff[%5d] \n", x_pos_count, phase_value, sum_g_result, sum_g_prev- sum_g_result);

			if(max_sum_found > 0)
			{
				if( (sum_g_store - sum_g_result) > 20 )
				{
					max_diff_value = sum_g_diff;
					max_phase_value = phase_value;
					AFE_PRINT("Phase Result[0x%3x], stored sum result [%d] \n", max_phase_value, sum_g_store);
					break;
				}
			}

			if((max_sum_found <1) && (sum_g_store - sum_g_result) < 10)
			{
				AFE_PRINT("Same SUM value found\n");
				max_sum_found = 1;
			}

			/*
			if( (sum_g_diff > max_diff_value) && (phase_value > 0))
			{
				max_diff_value = sum_g_diff;
				max_phase_value = phase_value;
			}
			*/

			sum_g_prev = sum_g_result;

			if(work_pending(&gWorkqueue->ifm))
			{
				AFE_PRINT("In Phase Searching pending\n");
				return -1;
			}
		}
		if(max_phase_value > -1)
		{
			AFE_PRINT("Set Phase Value[0x%3x] \n", max_phase_value);
			ADC_SetPhaseValue(max_phase_value);
			break;
		}

	}

#if 0
	//LLPLL_shift 일단 0으로 초기화
	ADC_SetPhaseValue(0);

	// PHASE 돌아가면서 확정하기 Method 1 : 간단하게 dev 최고값
	{

		int iPhaseMin =0 ;
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) * 1.5);
		int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) + 2); //0918 by powerory
		int iPhaseMaxConst = iPhaseMax;
		int iPhaseOffset = PHASE_OFFSET_COMPONENT ;
		float dOutAvg;
		
		//AFE_PRINT("%d @ %s phase max %d\n", __LINE__ , __func__ , iPhaseMax );
	
		for(loop=0;loop<1;loop++)
		{
			int iPhaseStep = (iPhaseMax-iPhaseMin) / PHASE_STEP_COMPONENT + 1;  
			int iMaxBackup = iMax;
			for(i=iPhaseMin;i<=iPhaseMax;i+=iPhaseStep)		// risky
			{
				//LLPLL_shift
				ADC_SetPhaseValue(i);
				
				msleep(waiting_time);
	
				ADC_GetRGBResult(result);

//AFE_PRINT("%d @ %s loop[%d] PHASE %d RESULT R[%d] G[%d] B[%d]\n", __LINE__ , __func__ , loop ,i,result->out_r , result->out_g , result->out_b );
	
				dOutAvg = (float)(result->out_r + result->out_g + result->out_b ) / 3.0 ;
				if(dOutAvg > dMax) {
					dMax = dOutAvg;
					iMax = i;
				}
				//AFE_PRINT("dOutAvg %d , dMax %d\n", (int)dOutAvg, (int)dMax);

				//AFE_PRINT("%d/%d\n", i, iPhaseMaxConst);
			}
			iPhaseMin = iMax - iPhaseStep * iPhaseOffset;
			iPhaseMax = iMax + iPhaseStep * (1+iPhaseOffset) ;

//AFE_PRINT("Phase max %d , Phase min %d\n", iPhaseMax, iPhaseMin);
	
			if(iPhaseMin < 0) iPhaseMin =0;
			if(iPhaseMax > iPhaseMaxConst) iPhaseMax = iPhaseMaxConst;
	
			if(iPhaseStep == 1 ) break;
			if(iMax == iMaxBackup) break;
		}
	}
	

#endif
	kfree(result);
	ADC_Green_Mid_Level_Select(0);

	return (int)iMax;

}

/**
 *
 * search phase for component input
 *
 * @param	: iIndex
 * @return	: deviation value
 *
*/	
int ADC_PHASE_Mesurement_for_Component_HSync_Sampling(int iIndex)
{

	int i, loop, waiting_time = 100;
	int iMax = 0;
	S_RGB_OUT *result;
	//120103 wonsik.do : removed floating point in kernel driver
	//float dMax = -1.0;
	UINT64 dMax = 0;
	S_RGB_POSITION pos_ap;
	S_RGB_SIZE		size_ap;

	ADC_Green_Mid_Level_Select(1);

	size_ap.width_r = 0;
	//size_ap.width_g = HS_Out_Width[iIndex] + 11;
	size_ap.width_g = 10;
	size_ap.width_b = 0;

	size_ap.height_r = 0;
	size_ap.height_g = 100;
	size_ap.height_b = 0;

	pos_ap.width_r = 0;
	pos_ap.height_r = 0;
	pos_ap.width_g = LLPLL_counter_max[iIndex] - HS_Out_Width[iIndex] - 11;

	pos_ap.height_g = 0;
	pos_ap.width_b = 0;
	pos_ap.height_b = 0;

	ADC_SetRGBSize(&size_ap);
	ADC_SetRGBPosition(&pos_ap);
	
	AFE_PRINT("HTotal [%d], Hsync Width [%d], x_pos[%d], x_size[%d]\n", LLPLL_counter_max[iIndex], HS_Out_Width[iIndex], pos_ap.width_g, size_ap.width_g);
	
	result = (S_RGB_OUT *)kmalloc(sizeof(S_RGB_OUT),GFP_KERNEL);
	
	//LLPLL_shift 일단 0으로 초기화
	ADC_SetPhaseValue(0);

	// PHASE 돌아가면서 확정하기 Method 1 : 간단하게 dev 최고값
	{

		int iPhaseMin =0 ;
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) * 1.5);
		//120103 wonsik.do : removed floating point in kernel driver
		//int iPhaseMax = (int) ((1.0/TB_FS[iIndex] * 1000000 / 130) + 2); //0918 by powerory
		int iPhaseMax = (1000000000 / (TB_FS[iIndex] * 130) + 2); //0918 by powerory
		int iPhaseMaxConst = iPhaseMax;
		int iPhaseOffset = PHASE_OFFSET_COMPONENT ;
		//120103 wonsik.do : removed floating point in kernel driver
		//float dOutAvg;
		UINT64 dOutAvg;
		
		AFE_PRINT("%d @ %s phase max %d\n", __LINE__ , __func__ , iPhaseMax );
	
		for(loop=0;loop<1;loop++)
		{
			int iPhaseStep = (iPhaseMax-iPhaseMin) / PHASE_STEP_COMPONENT + 1;  
			int iMaxBackup = iMax;
			for(i=iPhaseMin;i<=iPhaseMax;i+=iPhaseStep)		// risky
			{
				if(work_pending(&gWorkqueue->ifm))
				{
					AFE_PRINT("In Phase Searching pending\n");
					return -1;
				}

				//LLPLL_shift
				ADC_SetPhaseValue(i);
				
				msleep(waiting_time);
	
				ADC_GetRGBResult(result);

				AFE_PRINT("PHASE %d RESULT G[%d] \n", i, result->out_g );
	
				dOutAvg = result->out_g;
				if(dOutAvg > dMax) {
					dMax = dOutAvg;
					iMax = i;
				}
				//AFE_PRINT("dOutAvg %d , dMax %d\n", (int)dOutAvg, (int)dMax);

				//AFE_PRINT("%d/%d\n", i, iPhaseMaxConst);
			}
			iPhaseMin = iMax - iPhaseStep * iPhaseOffset;
			iPhaseMax = iMax + iPhaseStep * (1+iPhaseOffset) ;

//AFE_PRINT("Phase max %d , Phase min %d\n", iPhaseMax, iPhaseMin);
	
			if(iPhaseMin < 0) iPhaseMin =0;
			if(iPhaseMax > iPhaseMaxConst) iPhaseMax = iPhaseMaxConst;
	
			if(iPhaseStep == 1 ) break;
			if(iMax == iMaxBackup) break;
		}
	}
	
	AFE_PRINT("Phase = [%d][0x%x]\n", iMax, iMax);
	ADC_SetPhaseValue(iMax);

	kfree(result);

	ADC_Green_Mid_Level_Select(0);

	return (int)iMax;

}
#endif
