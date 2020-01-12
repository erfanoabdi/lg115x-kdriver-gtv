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
 *	ADC hw control
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version     1.0
 *  date
 *  note        Additional information.
 *
 *  @addtogroup lg1150_gfx
 *  @{
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
#include <linux/interrupt.h>	/** interrupt */
#include <linux/platform_device.h>
#include <linux/delay.h>	/** for mdelay	*/
#include <linux/wait.h>		/** wait_event_interrupt_timeout	*/

#include "afe_drv.h"

#include "afe_kapi.h"
#include "vport_reg.h"
 
//#include "vport_reg.h"
#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"

#include "../i2c/i2c_core_api.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define	I2C_PORT	1

#define	ADC_ANALOG	0xAA
#define	ADC_DIGITAL	0xAC
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
//extern volatile VPORT_Reg_T *gVportReg;
/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
#if 0
static DECLARE_WAIT_QUEUE_HEAD(agoc_done);
static int giAgocIntDone = 0;
#endif
/*========================================================================================
	Implementation Group
========================================================================================*/
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
			*value = (unsigned int)(buff[0]<<8 | buff[1]<<0);
		break;
		case 3:
			*value = (unsigned int)(buff[0]<<16 | buff[1]<<8 | buff[2]<<0);
		break;
		case 4:
			*value = (unsigned int)(buff[0]<<24 | buff[1]<<16 | buff[2]<<8 | buff[3]<<0);
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
	
	
	for(i=size ; i >= 1 ; i--)
	{
		buff[i - 1] = (unsigned char)(temp&0xff);
		temp >>= 8;
		
	}
	
	ret = i2c_write(I2C_PORT,slave,addr,1,buff,size,0);
	
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
	
	return ret;
}

int ADC_PowerOn(void)
{
	
	
}

int ADC_PowerOff(void)
{
	
	
}


int ADC_InitDaughterBoard(void)
{
	
	// i2c intialize
	I2C_DEV_DriverInit();
	I2C_DEV_Install ( I2C_PORT, STANDARD_SPEED, MASTER_MODE );
	//I2C_DEV_Enable(I2C_PORT);
	
	// hdmi initialize
	I2C_write(0x3b*2 , 0x08 , 0x35);
	I2C_write(ADC_ANALOG , 0x31 , 0x7);
	I2C_write(ADC_ANALOG , 0x32 , 0x01);
	I2C_write(ADC_ANALOG , 0x56 , 0x0c);
	
	I2C_write(ADC_ANALOG , 0x55 , 0x0);	// change buffer
//	I2C_write(ADC_ANALOG , 0x56 , 0x0e);	// clock change

	I2C_write(ADC_ANALOG , 0x41 , 0x01);	// clamp on
	I2C_write(ADC_ANALOG , 0x44 , 0x0);	// IRE test off
	
	I2C_write(ADC_ANALOG , 0xaf , 0x11);	// CSC 
	
	msleep(100);
	
	return 0;
}


int ADC_GetBoundaryInform(ST_EXTAU *inform)
{
	
	// get extau_up
	//inform->extau_up = gVportReg->ch3reg_e_tau_up_result.ch3reg_e_tau_up_result;
	I2C_read_buffer(ADC_DIGITAL , 0xc1 , 2 , &inform->extau_up);
	
	// get extau_down
	//inform->extau_down = gVportReg->ch3reg_e_tau_up_result.ch3reg_e_tau_down_result;
	I2C_read_buffer(ADC_DIGITAL , 0xc3 , 2 , &inform->extau_down);
	
	// get extau_left
	//inform->extau_left = gVportReg->ch3reg_e_tau_left_result.ch3reg_e_tau_left_result;
	I2C_read_buffer(ADC_DIGITAL , 0xc5 , 2 , &inform->extau_left);
	
	// get extau_lest
	//inform->extau_right = gVportReg->ch3reg_e_tau_left_result.ch3reg_e_tau_right_result;
	I2C_read_buffer(ADC_DIGITAL , 0xc7 , 2 , &inform->extau_right);
	
	return 0;
}

int ADC_SetTAUInform(S_EXTAU_VALUE *value)
{
	// Height
	//gVportReg->ch3reg_e_tau_down_offset.ch3reg_e_tau_down_offset = value->height_offset;
	I2C_write_buffer(ADC_DIGITAL , 0x22 , 2 , &value->height_offset);

	// WIDTH
	//gVportReg->ch3reg_e_tau_right_offset.ch3reg_e_tau_right_offset = value->width_offset;
	I2C_write_buffer(ADC_DIGITAL , 0x26 , 2 , &value->width_offset);	
	
	//gVportReg->ch3reg_e_tau_ctrl.ch3reg_e_tau_ctrl = value->control;
	I2C_write(ADC_DIGITAL , 0x1c , (value->control&0xff00)>>8);
	I2C_write(ADC_DIGITAL , 0x1d , value->control & 0xff);
	
//	temp = 0x10000000;
//	I2C_write_buffer(ADC_DIGITAL , 0x1e , 2 , &temp);
	
	return 0;
	
}


int ADC_GetI2CData(unsigned int sub , unsigned int addr , unsigned int size )
{
	unsigned int value;
	
	if(size > 4) return -1;
	
	if(sub)
	{
		I2C_read_buffer(ADC_DIGITAL , addr , size , &value);
		AFE_PRINT(" DIGITAL , %x : %x[%d]\n" , addr , value , size);
	}
	else{
		
		I2C_read_buffer(ADC_ANALOG , addr , size , &value);
		AFE_PRINT(" ANALOG , %x : %x[%d]\n" , addr , value , size);
	}
	
	return 0;
	
}

int ADC_SetI2CData(unsigned int sub , unsigned int addr , unsigned int size , unsigned int value)
{
	if(size > 4) return -1;
	
	if(sub)
	{
		I2C_write_buffer(ADC_DIGITAL , addr , size , &value);
		
	}
	else{
		
		I2C_write_buffer(ADC_ANALOG , addr , size , &value);
		
	}

	ADC_GetI2CData(sub , addr , size);
	
	return 0;
	
}



int ADC_GetRGBResult(S_RGB_OUT *result)
{
	//result->out_b =  gVportReg->ch3reg_ap_out_b.ch3reg_ap_out_b;
	I2C_read_buffer(ADC_DIGITAL , 0xad , 4 , &result->out_b);
	//result->out_g = gVportReg->ch3reg_ap_out_g.ch3reg_ap_out_g;
	I2C_read_buffer(ADC_DIGITAL , 0xb1 , 4 , &result->out_g);
	//result->out_r =  gVportReg->ch3reg_ap_out_r.ch3reg_ap_out_r;
	I2C_read_buffer(ADC_DIGITAL , 0xa9 , 4 , &result->out_r);
	       
	return 0;	
}


int ADC_SetRGBPosition(S_RGB_POSITION *pos)
{
	//gVportReg->ch3reg_ap__pos_b.ch3reg_ap__pos_b = pos->width_b;
	I2C_write_buffer(ADC_DIGITAL , 0x4 , 2 , &pos->width_b);
	//gVportReg->ch3reg_ap__pos_g.ch3reg_ap__pos_g = pos->width_g;
	I2C_write_buffer(ADC_DIGITAL , 0xc , 2 , &pos->width_g);
	//gVportReg->ch3reg_ap__pos_r.ch3reg_ap__pos_r = pos->width_r;
	I2C_write_buffer(ADC_DIGITAL , 0x14 , 2 , &pos->width_r);
	
	//gVportReg->ch3reg_ap_ypos_b.ch3reg_ap_ypos_b = pos->height_b;
	I2C_write_buffer(ADC_DIGITAL , 0x8 , 2 , &pos->height_b);
	//gVportReg->ch3reg_ap_ypos_g.ch3reg_ap_ypos_g = pos->height_g;
	I2C_write_buffer(ADC_DIGITAL , 0x10 , 2 , &pos->height_g);
	//gVportReg->ch3reg_ap_ypos_r.ch3reg_ap_ypos_r = pos->height_r;
	I2C_write_buffer(ADC_DIGITAL , 0x18 , 2 , &pos->height_r);



	return 0;	
}

int ADC_GetRGBPosition(S_RGB_POSITION *pos)
{
	/*
	pos->width_b =  gVportReg->ch3reg_ap__pos_b.ch3reg_ap__pos_b;
	pos->width_g =	gVportReg->ch3reg_ap__pos_g.ch3reg_ap__pos_g;
	pos->width_r =  gVportReg->ch3reg_ap__pos_r.ch3reg_ap__pos_r;
	                                                              
	pos->height_b = gVportReg->ch3reg_ap_ypos_b.ch3reg_ap_ypos_b;
	pos->height_g = gVportReg->ch3reg_ap_ypos_g.ch3reg_ap_ypos_g;
	pos->height_r = gVportReg->ch3reg_ap_ypos_r.ch3reg_ap_ypos_r;
	*/
	
	I2C_read_buffer(ADC_DIGITAL , 0x4 , 2 , &pos->width_b);
	I2C_read_buffer(ADC_DIGITAL , 0xc , 2 , &pos->width_g);
	I2C_read_buffer(ADC_DIGITAL , 0x14 , 2 , &pos->width_r);
	
	I2C_read_buffer(ADC_DIGITAL , 0x8 , 2 , &pos->height_b);
	I2C_read_buffer(ADC_DIGITAL , 0x10 , 2 , &pos->height_g);
	I2C_read_buffer(ADC_DIGITAL , 0x18 , 2 , &pos->height_r);	

	return 0;	
}

int ADC_SetRGBSize(S_RGB_SIZE *size)
{

	/*
	gVportReg->ch3reg_ap__pos_b.ch3reg_ap__size_b = size->width_b;
	gVportReg->ch3reg_ap__pos_g.ch3reg_ap__size_g = size->width_g;
	gVportReg->ch3reg_ap__pos_r.ch3reg_ap__size_r = size->width_r;
	
	gVportReg->ch3reg_ap_ypos_b.ch3reg_ap_ysize_b = size->height_b;
	gVportReg->ch3reg_ap_ypos_g.ch3reg_ap_ysize_g = size->height_g;
	gVportReg->ch3reg_ap_ypos_r.ch3reg_ap_ysize_r = size->height_r;
	*/
	

	
	I2C_write_buffer(ADC_DIGITAL , 0x6 , 2 , &size->width_b);
	I2C_write_buffer(ADC_DIGITAL , 0xe , 2 , &size->width_g);
	I2C_write_buffer(ADC_DIGITAL , 0x16 , 2 , &size->width_r);
	
	
	I2C_write_buffer(ADC_DIGITAL , 0xa , 2 , &size->height_b);
	I2C_write_buffer(ADC_DIGITAL , 0x12 , 2 , &size->height_g);
	I2C_write_buffer(ADC_DIGITAL , 0x1a , 2 , &size->height_r);	
	



	return 0;	
}

int ADC_GetSizeValue(S_VIDEO_SIZE_INFORM *size)
{
	
	//size->ext_vsize = gVportReg->ch3reg_sd_e_t_vsize.ch3reg_sd_e_t_vsize;
	I2C_read_buffer(ADC_DIGITAL , 0xa1 , 2 , (unsigned int *)&size->ext_vsize);	
	//size->vd_vsize = gVportReg->ch3reg_sd_vd_vsize.ch3reg_sd_vd_vsize;
	I2C_read_buffer(ADC_DIGITAL , 0xa3 , 3 , (unsigned int *)&size->vd_vsize);	
	//size->vd_hsize = gVportReg->ch3reg_sd_vd_hsize.ch3reg_sd_vd_hsize;		
	I2C_read_buffer(ADC_DIGITAL , 0xa6 , 2 , (unsigned int *)&size->vd_hsize);	
	//size->polarity = (gVportReg->cst_ro_hsdet.cst_ro_hspol<<1) | (gVportReg->cst_ro_vspol.cst_ro_vspol);
	I2C_read(ADC_ANALOG , 0xb2 , (unsigned char *)&size->polarity);	

	return 0;	
	
}

int ADC_SetPort(S_SEL_PORT *sel_port)
{
	unsigned char temp;
	
	//gVportReg->afe3ch_selmux.afe3ch_selmux = sel_port->port;
	I2C_write_buffer(ADC_ANALOG , 0x33 , 1 , (unsigned int *)&sel_port->port);
	
	I2C_write_buffer(ADC_ANALOG , 0x6c , 1 , (unsigned int *)&sel_port->port);		// should be check
	
	AFE_PRINT("%s port is %d\n", __func__ , sel_port->port);
	if(sel_port->port < 2)
	{
		I2C_write(ADC_ANALOG , 0x73 , 3);
		I2C_write(ADC_ANALOG , 0x48 , 1);
		I2C_write(ADC_ANALOG , 0x4a , 1);
		I2C_write(ADC_ANALOG , 0xaf , 0x11);
		
		
	}
	else
	{
		
		I2C_write(ADC_ANALOG , 0x73 , 2);
		I2C_write(ADC_ANALOG , 0x48 , 0);
		I2C_write(ADC_ANALOG , 0x4a , 0);
		I2C_write(ADC_ANALOG , 0xaf , 0);
	}
		
/*	
	gVportReg->cst_hsselover.cst_hsselover = sel_port->hsselover;
	gVportReg->cst_hsselover.cst_hsseluser = sel_port->hsseluser;
	gVportReg->cst_hsselover.cst_vsselover = sel_port->vsselover;
	gVportReg->cst_hsselover.cst_vsseluser = sel_port->vsseluser;
	gVportReg->cst_compover.cst_compover = sel_port->compover;
	gVportReg->cst_compover.cst_compuser = sel_port->compuser;
*/	
	temp = ((sel_port->hsselover<<1) | sel_port->hsseluser);
	I2C_write(ADC_ANALOG , 0x67 , temp);
	temp = ((sel_port->vsselover<<1) | sel_port->vsseluser);
	I2C_write(ADC_ANALOG , 0x68 , temp);
	temp = ((sel_port->compover<<1) | sel_port->compuser);
	I2C_write(ADC_ANALOG , 0x6b , temp);
	
	
	return 0;
}

int ADC_GetSelectedPort(void)
{
	unsigned char temp;
	
	I2C_read(ADC_ANALOG , 0x33  , &temp);
	
	return ((int)temp);
		
	
}

unsigned int ADC_GetPhaseValue(void)
{
	unsigned int temp;
	
	I2C_read_buffer(ADC_ANALOG , 0x18 , 2 , &temp);
	
	return(temp);
	
}

int ADC_SetPhaseValue(unsigned int phase)
{

	//gVportReg->llpll_shift.llpll_shift = phase;
	
	I2C_write_buffer(ADC_ANALOG , 0x18 , 2 , &phase);

	return 0;
	
}

unsigned int ADC_GetScanType(void)
{
	unsigned int temp;
	
	I2C_read(ADC_ANALOG , 0xb3 , (unsigned char *)&temp);
	
	return( ((temp&0x2)>>1)^1 );
	
}


int ADC_SetGainValue(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;	
	
	//gVportReg->afe3ch_gin_r.afe3ch_gin_r = stGain->R_Gain_Value;
	I2C_write_buffer(ADC_ANALOG , 0x3b , 2 , (unsigned int *)&stGain->R_Gain_Value);
	//gVportReg->afe3ch_gin_g.afe3ch_gin_g = stGain->G_Gain_Value;
	I2C_write_buffer(ADC_ANALOG , 0x3d , 2 , (unsigned int *)&stGain->G_Gain_Value);
	//gVportReg->afe3ch_gin_b.afe3ch_gin_b = stGain->B_Gain_Value;
	I2C_write_buffer(ADC_ANALOG , 0x3f , 2 , (unsigned int *)&stGain->B_Gain_Value);
	
	return ret;
	
}

int ADC_GetGainValue(LX_AFE_ADC_GAIN_VALUE_T *stGain)
{
	int ret = 0;	

/*
	stGain->R_Gain_Value = gVportReg->afe3ch_gin_r.afe3ch_gin_r;
	stGain->G_Gain_Value = gVportReg->afe3ch_gin_g.afe3ch_gin_g;
	stGain->B_Gain_Value = gVportReg->afe3ch_gin_b.afe3ch_gin_b;
*/

	
	I2C_read_buffer(ADC_ANALOG , 0x3b , 2 , (unsigned int *)&stGain->R_Gain_Value);
	I2C_read_buffer(ADC_ANALOG , 0x3d , 2 , (unsigned int *)&stGain->G_Gain_Value);
	I2C_read_buffer(ADC_ANALOG , 0x3f , 2 , (unsigned int *)&stGain->B_Gain_Value);	
	
	return ret;	
}


int ADC_SetOffsetValue(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;	
	/*
	gVportReg->afe3ch_oftin_r.afe3ch_oftin_r = stOffset->R_Offset_Value;
	gVportReg->afe3ch_oftin_g.afe3ch_oftin_g = stOffset->G_Offset_Value;
	gVportReg->afe3ch_oftin_b.afe3ch_oftin_b = stOffset->B_Offset_Value;
	*/
	I2C_write_buffer(ADC_ANALOG , 0x35 , 2 ,(unsigned int *) &stOffset->R_Offset_Value);
	I2C_write_buffer(ADC_ANALOG , 0x37 , 2 , (unsigned int *)&stOffset->G_Offset_Value);
	I2C_write_buffer(ADC_ANALOG , 0x39 , 2 , (unsigned int *)&stOffset->B_Offset_Value);
	return ret;
	
}

int ADC_GetOffsetValue(LX_AFE_ADC_OFFSET_VALUE_T *stOffset)
{
	int ret = 0;	
/*
	stOffset->R_Offset_Value = gVportReg->afe3ch_oftin_r.afe3ch_oftin_r;	
	stOffset->G_Offset_Value = gVportReg->afe3ch_oftin_g.afe3ch_oftin_g;
	stOffset->B_Offset_Value = gVportReg->afe3ch_oftin_b.afe3ch_oftin_b;	
*/	
	I2C_read_buffer(ADC_ANALOG , 0x35 , 2 , (unsigned int *)&stOffset->R_Offset_Value);
	I2C_read_buffer(ADC_ANALOG , 0x37 , 2 , (unsigned int *)&stOffset->G_Offset_Value);
	I2C_read_buffer(ADC_ANALOG , 0x39 , 2 , (unsigned int *)&stOffset->B_Offset_Value);
	
	return ret;	
}


/**
 *
 * setting the LLPLL by index value which came from ADC_IFM_SearchTable
 *
 * @param	S_VIDEO_SIZE_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetInputVideo(int iIndex)
{
	
	int i;
	unsigned int temp;

	//LLPLL_div_max
	//gVportReg->llpll_div_max.llpll_div_max = LLPLL_div_max[iIndex];
	I2C_write(ADC_ANALOG , 0x15 , LLPLL_div_max[iIndex]);
	//LLPLL_counter_max
	//gVportReg->llpll_div_max.llpll_counter_max = LLPLL_counter_max[iIndex];
	I2C_write_buffer(ADC_ANALOG , 0x16 , 2 , &LLPLL_counter_max[iIndex]);

	//LLPLL_shift 일단 0으로 초기화
	//gVportReg->llpll_shift.llpll_shift = 0;		// should be set the phase later
	ADC_SetPhaseValue(0);

	//LLPLL_dco_max
	//gVportReg->llpll_dco_max.llpll_dco_max = LLPLL_dco_max[iIndex];
	I2C_write(ADC_ANALOG , 0x1c , LLPLL_dco_max[iIndex]);
	
	//LLPLL_dco_min
	//gVportReg->llpll_dco_max.llpll_dco_min = LLPLL_dco_min[iIndex];
	I2C_write(ADC_ANALOG , 0x1d , LLPLL_dco_min[iIndex]);

	//LLPLL_coarse_scale
	//gVportReg->llpll_coarse_scale.llpll_coarse_scale = LLPLL_coarse_scale[iIndex];
	I2C_write(ADC_ANALOG , 0x1e , LLPLL_coarse_scale[iIndex]);

	//LLPLL_g1_nom
	//gVportReg->llpll_g1_nom.llpll_g1_nom = LLPLL_g1_nom[iIndex];
	I2C_write(ADC_ANALOG , 0x1f , LLPLL_g1_nom[iIndex]);

	//LLPLL_g2_nom
	//gVportReg->llpll_g1_nom.llpll_g2_nom = LLPLL_g2_nom[iIndex];
	I2C_write(ADC_ANALOG , 0x20 , LLPLL_g2_nom[iIndex]);

	//LLPLL_g3_p_nom
	//gVportReg->llpll_g1_nom.llpll_g3_p_nom = LLPLL_g3_p_nom[iIndex];
	I2C_write(ADC_ANALOG , 0x21 , LLPLL_g3_p_nom[iIndex]);

	//LLPLL_g3_n_nom
	//gVportReg->llpll_g1_nom.llpll_g3_n_nom = LLPLL_g3_n_nom[iIndex];
	I2C_write(ADC_ANALOG , 0x22 , LLPLL_g3_n_nom[iIndex]);

	//LLPLL_g1_fine
	//gVportReg->llpll_g1_fine.llpll_g1_fine = LLPLL_g1_fine[iIndex];
	I2C_write(ADC_ANALOG , 0x23 , LLPLL_g1_fine[iIndex]);
	
	//LLPLL_g2_fine
	//gVportReg->llpll_g1_fine.llpll_g2_fine = LLPLL_g2_fine[iIndex];
	I2C_write(ADC_ANALOG , 0x24 , LLPLL_g2_fine[iIndex]);
	
	//LLPLL_g3_p_fine
	//gVportReg->llpll_g1_fine.llpll_g3_p_fine = LLPLL_g3_p_fine[iIndex];
	I2C_write(ADC_ANALOG , 0x25 , LLPLL_g3_p_fine[iIndex]);

	//LLPLL_g3_n_fine
	//gVportReg->llpll_g1_fine.llpll_g3_n_fine = LLPLL_g3_n_fine[iIndex];
	//I2C_write(ADC_ANALOG , 0x26 , LLPLL_g3_n_fine[iIndex]);
	
	msleep(100);
	
	for( i = 0 ; i < 10 ; i++)
	{
		I2C_read(ADC_ANALOG , 0xef , (unsigned char *)&temp);
		
		if(temp != 3) break;
	}
	
	return 0;
}



int ADC_SetDE(int iIndex , unsigned int extau_up , unsigned int extau_left)
{
	
	static unsigned int up , left , temp;	
	
	up = 0x1000 | extau_up;
	left = 0x1000 | extau_left;
	
	I2C_write_buffer(ADC_ANALOG , 0xAA , 2 , &up);

	if(TB_PI[iIndex] == 1) {
		temp = TB_HEIGHT[iIndex];
	} else {
		temp = TB_HEIGHT[iIndex]/2;
	}

	I2C_write_buffer(ADC_ANALOG , 0xAC , 2 , &temp);
	
	I2C_write_buffer(ADC_ANALOG , 0xA6 , 2 , &left);


	I2C_write_buffer(ADC_ANALOG , 0xA8 , 2 , &TB_WIDTH[iIndex]);
	
	AFE_PRINT("up %d, left %d , heigth %d, width %d\n"
	, extau_up , extau_left , temp , TB_WIDTH[iIndex]);
	
	return 0;
}


int ADC_SetHorPos(unsigned short pos)
{
	static unsigned short temp;
	temp = 0x1000 | pos;
	
	I2C_write_buffer(ADC_ANALOG , 0xA6 , 2 , (unsigned int *)&temp);

	return 0;
	
}

int ADC_SetVerPos(unsigned short pos)
{
	static unsigned short temp;
	temp = 0x1000 | pos;
	
	I2C_write_buffer(ADC_ANALOG , 0xAA , 2 , (unsigned int *)&temp);

	return 0;
}

int ADC_SetHorTotal(unsigned short value)
{
	static unsigned short temp;
	temp = value;
	
	I2C_write_buffer(ADC_ANALOG , 0xA8 , 2 , (unsigned int *)&temp);

	return 0;
}



int ADC_AGOCInit(void)
{
	unsigned int temp;
	
	temp = 0x0;	
	I2C_write_buffer(ADC_ANALOG , 0xAF , 2 , &temp);	// CSC off
	
	temp = 0xfffe;
	I2C_write_buffer(ADC_ANALOG , 0x94 , 2 , &temp);	// aogc_counter_max
	
	temp = 0x34;
	I2C_write_buffer(ADC_ANALOG , 0xa5 , 1 , &temp);	// aogc_counter_max
	
	return 0;
	
}



/**
 *
 * setting ADC IRE test enable/disable
 *
 * @param	0 : inactive , 1 : active
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetIRETestMode(int sel)
{
	int ret = 0;
	unsigned int temp;
	

	if(sel)	// IRE Test mode inactive
	{	
		ADC_AGOCInit();
			
			
		temp = 0x10;
		
		I2C_write(ADC_ANALOG , 0x92 , 0x10);
		I2C_write(ADC_ANALOG , 0x91 , 0x1);
		
		mdelay(20);
		
	}
	else	// IRE Test mode active
	{
		I2C_write(ADC_ANALOG , 0x41 , 0x1);
		I2C_write(ADC_ANALOG , 0x92 , 0x0);
		
		I2C_write(ADC_ANALOG , 0xaf , 0x0);	//CSC
	}
	return ret;
}

/**
 *
 * get RGB average value from register
 *
 * @param	ST_GAIN_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/

int ADC_GetIRELevel( S_GAIN_TARGET *gain)
{
	int ret = 0;
	unsigned int temp_r, temp_g,temp_b;
	
	I2C_read_buffer(ADC_ANALOG , 0xdd , 3 , &temp_r);
	I2C_read_buffer(ADC_ANALOG , 0xe0 , 3 , &temp_g);
	I2C_read_buffer(ADC_ANALOG , 0xe3 , 3 , &temp_b);

	AFE_PRINT("IRE level : R[%d] , G[%d] , B[%d] \n",temp_r, temp_g , temp_b );

	//120103 wonsik.do : removed floating point in kernel driver
	//gain->gain_R = (float)( (temp_r&0xffff00) >> 8) + (float)(temp_r&0xff)/256.0; 
	//gain->gain_G = (float)( (temp_g&0xffff00) >> 8) + (float)(temp_g&0xff)/256.0;
	//gain->gain_B = (float)( (temp_b&0xffff00) >> 8) + (float)(temp_b&0xff)/256.0;
	gain->gain_R = temp_r&0xffff00;
	gain->gain_G = temp_g&0xffff00;
	gain->gain_B = temp_b&0xffff00;

	AFE_PRINT("IRE level : R[%d] , G[%d] , B[%d] \n", (int)gain->gain_R, (int)gain->gain_G, (int)gain->gain_B);
	
	return ret;
	
}


/**
 *
 * set IRE value for R,G,B
 *
 * @param	S_VIDEO_SIZE_INFORM
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetIRELevel(E_AGC_IRE ire , S_GAIN_TARGET *gain)
{
	int ret = 0;
	unsigned short target_ire;
	
	AFE_PRINT("IRE level set %d\n",ire);
		
	// set IRE
	I2C_write(ADC_ANALOG , 0x43 , ire);
	
	switch(ire)
	{
		case 	IRE0:
			target_ire = 0;
		break;
		case 	IRE73:
			target_ire = 73 ;
		break;
		case 	IRE950:
			target_ire = 950 ;
		break;
		case 	IRE1023:
			target_ire = 1023;
		break;
		default:
			target_ire = 0;
			AFE_PRINT("WARNING : Invalid E_AGC_IRE type\n");
		break;
		
	}
	
	// set target value
/*	
	I2C_write_buffer(ADC_ANALOG , 0x97 , 2 , (unsigned int *)&target_ire);
	I2C_write_buffer(ADC_ANALOG , 0x99 , 2 , (unsigned int *)&target_ire);
	I2C_write_buffer(ADC_ANALOG , 0x9b , 2 , (unsigned int *)&target_ire);
*/
	I2C_write_buffer(ADC_ANALOG , 0x9d , 2 , (unsigned int *)&target_ire);
	I2C_write_buffer(ADC_ANALOG , 0x9f , 2 , (unsigned int *)&target_ire);
	I2C_write_buffer(ADC_ANALOG , 0xa1 , 2 , (unsigned int *)&target_ire);
	
//	I2C_write(ADC_ANALOG , 0x44 , 1);
//	msleep(50);
	
	
	ADC_SetIRETestMode(1);
	msleep(500);
	ADC_GetIRELevel(gain);
	
#if 0
	wait_event_interruptible_timeout(agoc_done , giAgocIntDone != 0 , 1000/10 );

	giAgocIntDone = 0;	
#endif			
	ADC_SetIRETestMode(0);
	
//	I2C_write(ADC_ANALOG , 0x44 , 0);
	return ret;
}

int ADC_SetAgocWakeup(void)
{
#if 0	
	giAgocIntDone = 1;
	wake_up_interruptible(&agoc_done);
#endif	
	return 0;
}

int ADC_GetGainRGB( ST_GAIN_INFORM *gain)
{
	
	I2C_read_buffer(ADC_ANALOG , 0x3b , 2 , (unsigned int *)&gain->gain_R);
	I2C_read_buffer(ADC_ANALOG , 0x3d , 2 , (unsigned int *)&gain->gain_G);
	I2C_read_buffer(ADC_ANALOG , 0x3f , 2 , (unsigned int *)&gain->gain_B);	
	
	gain->gain_R &= 0x3ff;
	gain->gain_G &= 0x3ff;
	gain->gain_B &= 0x3ff;
	return 0;
	
}

int ADC_SetGainRGB( ST_GAIN_INFORM *gain)
{

	
	I2C_write_buffer(ADC_ANALOG , 0x3b , 2 , (unsigned int *)&gain->gain_R);
	I2C_write_buffer(ADC_ANALOG , 0x3d , 2 , (unsigned int *)&gain->gain_G);
	I2C_write_buffer(ADC_ANALOG , 0x3f , 2 , (unsigned int *)&gain->gain_B);	
	

	return 0;
}



int ADC_GetSumRGB(S_RGB_SUM *sum)
{
	
	I2C_read_buffer(ADC_DIGITAL , 0xb5 , 4 , &sum->sum_R);
	I2C_read_buffer(ADC_DIGITAL , 0xb9 , 4 , &sum->sum_B);
	I2C_read_buffer(ADC_DIGITAL , 0xbd , 4 , &sum->sum_G);
			
	return 0;
}



int ADC_GetOffsetRGB( S_OFFSET_INFORM *offset)
{
	
	I2C_read_buffer(ADC_ANALOG , 0x35 , 2 , (unsigned int *)&offset->offset_R);
	I2C_read_buffer(ADC_ANALOG , 0x37 , 2 , (unsigned int *)&offset->offset_G);
	I2C_read_buffer(ADC_ANALOG , 0x39 , 2 , (unsigned int *)&offset->offset_B);
	
	offset->offset_R &= 0x3ff;
	offset->offset_G &= 0x3ff;
	offset->offset_B &= 0x3ff;
	
	return 0;
	
}

int ADC_SetOffsetRGB( S_OFFSET_INFORM *offset)
{
	
	offset->offset_R &= 0x3ff;
	offset->offset_G &= 0x3ff;
	offset->offset_B &= 0x3ff;
	
	I2C_write_buffer(ADC_ANALOG , 0x35 , 2 ,(unsigned int *) &offset->offset_R);
	I2C_write_buffer(ADC_ANALOG , 0x37 , 2 , (unsigned int *)&offset->offset_G);
	I2C_write_buffer(ADC_ANALOG , 0x39 , 2 , (unsigned int *)&offset->offset_B);	
	
	return 0;
}


int ADC_Enable_Scart_Mode(BOOLEAN scart_enable)
{
	return 0;
}
