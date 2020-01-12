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

/*! \file ADC_module.c
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
#include <linux/workqueue.h>		/**< For working queue */
#include <linux/interrupt.h>	/** irqreturn_t */
#include <linux/irq.h>
#include <linux/delay.h>	/** for mdelay	*/
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/version.h>

#include "os_util.h"
#include "afe_drv.h"

#include "afe_kapi.h"

//#include "vport_reg.h"
#include "resolution.h"
#include "adc_module.h"
#include "adc_hw.h"

#include "vport_reg_B0.h"
#include "vport_reg.h"



/******************************************************************************
 *				DEFINES
 *****************************************************************************/
#if 0
#define	VPORT_REG_BASE_ADDR	0xc0007000
#define	VPORT_REG_SIZE		0x7ef0
#define	VPORT_REG_QEMU_BASE_ADDR	0x4007000
#endif

#define	ADC_TIMEOUT 1000 //1 sec
#define ADC_THREAD_TIMEOUT	10

//#define IRQ_PRINT	1
/**
 *	Global variables of the driver
 */

//spinlock_t adc_lock ;
struct semaphore adc_lock;

//DECLARE_MUTEX(adc_lock);

ST_WORK_QUEUE	*gWorkqueue;


int gResolution;

int gResolutionSearching = 0;
extern volatile VPORT_RegAx_T *gVportRegAx;
extern volatile VPORT_RegBx_T *gVportRegBx;
extern			 VPORT_RegBx_T *gpRegVPORT;		// for shadow register


LX_AFE_ADC_SET_INPUT_T gPortStatus[3];

LX_AFE_PCMODE_MODETABLE_T *gPCTable;

unsigned int gPCTableNum = 0;

//extern int ADC_InitDaughterBoard(void);

#ifdef USE_ADC_TIMER
OS_TIMER_T		adc_timer;
S_VIDEO_SIZE_INFORM previous_size_inform = {0xffffffff,};
#endif

extern LX_AFE_ADC_INPUT_SOURCE_TYPE_T	gADC_InputSrc; //added by dws
extern int gPrevious_index;
extern int gADC_IN_IFM;
#if 0
static long kernel_pid = -1;
DECLARE_WAIT_QUEUE_HEAD(wq_kernel_test);
static int wq_condition = 0;

static struct timer_list *timer;
#endif

struct task_struct	*stADC_Thread;

OS_SEM_T	g_ADC_Sema;

ADC_SYNC_MODE g_ADC_Sync_Mode = ADC_SYNC_MODE_UNKNOWN;	//Initial ADC Sync Mode Unknown..

int (*ADC_PowerOn)(void);
int (*ADC_PowerOff)(void);
int (*ADC_InitDaughterBoard)(void);
int (*ADC_GetBoundaryInform)(ST_EXTAU *inform);
int (*ADC_SetTAUInform)(S_EXTAU_VALUE *value);
int (*ADC_SetRGBPosition)(S_RGB_POSITION *pos);
int (*ADC_GetRGBResult)(S_RGB_OUT *result);
int (*ADC_GetRGBPosition)(S_RGB_POSITION *pos);
int (*ADC_SetRGBSize)(S_RGB_SIZE *size);
int (*ADC_GetSizeValue)(S_VIDEO_SIZE_INFORM *size);
int (*ADC_SetPort)(S_SEL_PORT *sel_port);
unsigned int (*ADC_GetPhaseValue)(void);
int (*ADC_SetPhaseValue)(unsigned int phase);
unsigned int (*ADC_GetScanType)(void);
int (*ADC_SetGainValue)(LX_AFE_ADC_GAIN_VALUE_T *stGain);
int (*ADC_GetGainValue)(LX_AFE_ADC_GAIN_VALUE_T *stGain);
int (*ADC_SetOffsetValue)(LX_AFE_ADC_OFFSET_VALUE_T *stOffset);
int (*ADC_GetOffsetValue)(LX_AFE_ADC_OFFSET_VALUE_T *stOffset);
int (*ADC_SetInputVideo)(int iIndex);
int (*ADC_Enable_Scart_Mode)(BOOLEAN scart_enable);
void (*ADC_Vport_ALL_Power_Down)(void);
int (*ADC_Channel_Power_Control)(int vref_pdb, int bpdb, int gpdb, int rpdb);
int (*ADC_SetType)(LX_AFE_ADC_INPUT_SOURCE_TYPE_T sel_type);
int (*ADC_Set_LLPLL)(UINT32 arg);
int (*ADC_SetClock)(int iIndex, SINT16 value);
int (*ADC_AGOCInit)(void);
int (*ADC_GetSelectedPort)(void);
int (*ADC_SetGainRGB)( ST_GAIN_INFORM *gain);
int (*ADC_GetGainRGB)( ST_GAIN_INFORM *gain);
int (*ADC_SetIRELevel)(E_AGC_IRE ire , S_GAIN_TARGET *gain);
int (*ADC_GetOffsetRGB)( S_OFFSET_INFORM *offset);
int (*ADC_SetOffsetRGB)( S_OFFSET_INFORM *offset);
int (*ADC_GetSumRGB)(S_RGB_SUM *sum);
int (*ADC_SetAgocWakeup)(void);
int (*ADC_Reset_Digital)(void);
int (*ADC_Reset_Digital_24MHZ)(void);
int (*ADC_Reset_LLPLL)(void);
int (*ADC_Check_Sync_Exist)(void);
int (*ADC_Sync_Polarity_Override)(int set);
int (*ADC_Enable_Detection_Interrupt)(int enable);
int (*ADC_Enable_AOGC_Interrupt)(int enable);
int (*ADC_Mute_Enable)(int enable);
int (*ADC_Power_OnOff)(void);
int (*ADC_Ext_Clamp)(int sel_ext);
int (*ADC_Set_Clamp)(UINT8 clamp_base, UINT8 clamp_place, UINT8 clamp_width);
int (*ADC_GetPCModeInfo)(LX_AFE_PCMODE_INFO_T *stPCModeInfo);
int (*ADC_Get_LLPLL_Filter_Status)(void);
int (*ADC_SetIRETestMode)(int sel , E_AGC_IRE mode);
int (*ADC_Set_Coast)(UINT8 precoast, UINT8 postcoast);
int (*ADC_Set_Hsync_Sel)(UINT8 select);
int (*ADC_CVI_Sync_Exist)(void);
int (*ADC_Set_SOG_Level)(UINT32 level);
int (*ADC_Get_SOG_Level)(UINT32 *level);
int (*ADC_Check_Htotal_Diff)(UINT8 count, int Kadp_Index);
int (*ADC_Read_Pixel_Value)(LX_AFE_ADC_PIXEL_VALUE_T *pixel_value_t);
int (*ADC_Green_Mid_Level_Select)(UINT8 value);
int (*ADC_Read_VSync_Width)(UINT32 *pvs_width);
int (*ADC_Read_HSync_Width)(UINT32 *pvs_width);
void (*ADC_Set_Blank)(int blank_sp); // added by won.hur(won.hur@lge.com) @2011.08.17
void (*ADC_LVDS_Control)(int enable); // added by won.hur(won.hur@lge.com) @2011.08.17
int (*ADC_Component_APA)(int iIndex);
int (*ADC_Component_APA_Test)(UINT32 ADC_APA_Delay, UINT32 ADC_APA_Enable, UINT32 ADC_APA_Same_Count, UINT32 ADC_APA_Execute, UINT32 ADC_APA_Enable_Print);
int (*ADC_Get_Vtotal_Value)(void);
int (*ADC_Set_Sync_Mode)(ADC_SYNC_MODE sync_mode);
#ifdef	KDRV_CONFIG_PM
int (*ADC_RunSuspend)(void);
int (*ADC_RunResume)(void);
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

#ifdef USE_ADC_TIMER
static void ADC_Timer_Routine(void)
{
	S_VIDEO_SIZE_INFORM current_size_inform;

	//int unused;

	if(gADC_IN_IFM)
		return;

	ADC_GetInputSignalInform(&current_size_inform);

	if( (abs(previous_size_inform.ext_vsize - current_size_inform.ext_vsize) > 5) || (abs(previous_size_inform.vd_vsize - current_size_inform.vd_vsize) > 5) || (abs(previous_size_inform.vd_hsize - current_size_inform.vd_hsize) > 5) ) // one of size inform changed !!!
	{
		AFE_PRINT("Size Change by ADC Timer!!!\n");
		memcpy(&previous_size_inform, &current_size_inform, sizeof(S_VIDEO_SIZE_INFORM));

		queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
		//ADC_IFM_ISR((void *)&unused);

		return;
	}

	return;
}
#endif
/**
 *
 * ADC_SetPCmodeTable
 *
 * @param	struct work_struct
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_SetPCmodeTable(LX_AFE_SET_PCMODE_TABLE_T *stTableInfo)
{
 //  UINT32 *ModeTablePtr;

   //ModeTablePtr = kmalloc(stTableInfo->Table_Size, GFP_KERNEL);
   gPCTable = (LX_AFE_PCMODE_MODETABLE_T *)kmalloc(stTableInfo->Table_Size, GFP_KERNEL);

   //if(ModeTablePtr != NULL)
   if(gPCTable != NULL)
   {
   	  if( stTableInfo->Table_Size == 0) return -1;
//	  memcpy(ModeTablePtr, stTableInfo->pPCMode_Table, stTableInfo->Table_Size);
//	  memcpy(gPCTable, stTableInfo->pPCMode_Table, stTableInfo->Table_Size);
	  if( copy_from_user(gPCTable, stTableInfo->pPCMode_Table, stTableInfo->Table_Size) )
	  {
		  AFE_PRINT("ERROR During copying PCModeTable !!!!! \n");
		  return -EFAULT;
	  }
//	  gPCTable = (LX_AFE_PCMODE_MODETABLE_T *)ModeTablePtr;
	  AFE_PRINT("OK PCMode Table Copied\n");
	  gPCTableNum = stTableInfo->Table_Count;
	  AFE_PRINT("PC Mode Table Size is [%d]\n",gPCTableNum);

	  /* DO NOT Free this Memory */
//	  kfree(ModeTablePtr);
	  return 0;
   }
   else
   {

	 // kfree(ModeTablePtr);
	  return -1;
   }
}

/**
 *
 * Input format measurement
 *
 * @param	struct work_struct
 * @return	0 : OK , -1 : NOK
 *
*/
void ADC_IFM(void *unused)
{
	ST_EXTAU 	extau;

	if(gPCTable == NULL) return;

	//spin_lock(&adc_lock);
#if 0
	if(down_trylock(&adc_lock) != 0)
	{
		return;
	}
#else
	down(&adc_lock);
#endif
	//gResolutionSearching = 1;
	//down_interruptible(&adc_lock);

	ADC_IFM_main(&gResolution);

	if(gResolution >= 0)
	{

		extau.extau_up = (gPCTable+gResolution)->u16_HStart;
		extau.extau_left = (gPCTable+gResolution)->u16_VStart;

		ADC_IBD_Setting(&extau , gResolution);

#ifndef L8_COMP_HSYNC_PHASE_SEARCH
		if(gADC_InputSrc == LX_ADC_INPUT_SOURCE_YPBPR) // YPbPr Source
			ADC_SetPhaseValue((gPCTable+gResolution)->u16_ADC_Phase); // by dws
#endif
		//AFE_PRINT("# Setting ADC Phase to [0x%x]\n", (gPCTable+gResolution)->u16_ADC_Phase);
		//ADC_SetPhaseValue((gPCTable+gResolution)->u16_ADC_Phase); // by dws
	}
//	spin_unlock(&adc_lock);
	//gResolutionSearching = 0;
	up(&adc_lock);
	//return 0;
}

/**
 *
 * Input format measurement(Kernel Thread)
 *
 * @param	struct work_struct
 * @return	0 : OK , -1 : NOK
 *
*/
void ADC_IFM_Thread(void *unused)
{
	ST_EXTAU 	extau;

	if(gPCTable == NULL) return;

	//spin_lock(&adc_lock);
	/*
	if(down_trylock(&adc_lock) != 0)
	{
		return;
	}
	*/
	if(down_interruptible(&adc_lock))
		AFE_PRINT("ERROR down interruptible");

	ADC_IFM_main(&gResolution);

	if(gResolution >= 0)
	{

		extau.extau_up = (gPCTable+gResolution)->u16_HStart;
		extau.extau_left = (gPCTable+gResolution)->u16_VStart;

		ADC_IBD_Setting(&extau , gResolution);

		//AFE_PRINT("# Setting ADC Phase to [0x%x]\n", (gPCTable+gResolution)->u16_ADC_Phase);
		//ADC_SetPhaseValue((gPCTable+gResolution)->u16_ADC_Phase); // by dws
	}
//	spin_unlock(&adc_lock);
	up(&adc_lock);
	//return 0;
}

void ADC_IFM_ISR(void *unused)
{
	kernel_thread((void *)&ADC_IFM_Thread, (void*)NULL, CLONE_KERNEL);
}

static int ADC_Thread(void *data)
{
	while(1)
	{
		ADC_Periodic_Task();
		OS_MsecSleep(ADC_THREAD_TIMEOUT);
	}

	return 0;
}
/**
 *
 * Input boundary detect
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/
void ADC_IBD(void *unused)
{


	ADC_IBD_main(gResolution);

	//return 0;
}

/**
 *
 * Auto adjust
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/
void ADC_AAD(void *unused)
{

	ADC_PHASE_main(gResolution);

	//return 0;
}

/**
 *
 * Gain/Offset calibration
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_GOC(LX_AFE_ADC_CALIB_INFO_T *calib_param)
{

	int ret = 0;
	//void *unused;
	//gResolution = -1;
	//unused = (void *)kmalloc(sizeof(4),GFP_KERNEL);
	ADC_Enable_AOGC_Interrupt(1);

	ADC_Reset_LLPLL();

	ret = ADC_AutoGainControl_main(calib_param, gResolution);
#ifdef L8_KDRIVER_SUPPORT
	if(ret >= 0)
	{
		if ( lx_chip_rev( ) <= LX_CHIP_REV( L8, B0 ) )
			ret =ADC_AutoOffsetControl_main(gResolution);  	// if auto gain is success
	}
#endif

	//ADC_Reset_LLPLL();
	//ADC_Reset_Digital();
	ADC_Enable_AOGC_Interrupt(0);

	gPrevious_index = 0xffff;

//	ADC_IFM_ISR((void *)&unused);
	ADC_Clear_Previous_Size_Info();

	return ret;

}


/**
 *
 * Auto Phase for component
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/
void ADC_APC(void *unused)
{
	ADC_PHASE_for_Component(gResolution);
}
/**
 *
 * Work queue initialize
 *
 * @param	void
 * @return	0 : OK , -1 : NOK
 *
*/
int ADC_Init_Workqueue(void)
{

	gWorkqueue = (ST_WORK_QUEUE *)kmalloc(sizeof(ST_WORK_QUEUE), GFP_KERNEL);

	gWorkqueue->WorkQueue = create_workqueue("ADC_WORK");

	if(gWorkqueue->WorkQueue == NULL)
	{

		printk("qorkqueue allocation failed\n");
		return -1;
	}

	INIT_WORK(&gWorkqueue->ifm , (void *)&ADC_IFM);
	INIT_WORK(&gWorkqueue->ibd , (void *)&ADC_IBD);
	INIT_WORK(&gWorkqueue->aad , (void *)&ADC_AAD);
	INIT_WORK(&gWorkqueue->goc , (void *)&ADC_GOC);
	INIT_WORK(&gWorkqueue->apc , (void *)&ADC_APC);

//	queue_work(&gWorkQueue->WorkQueue , &gWorkQueue->ifm);
//	spin_lock_init(&adc_lock);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
		init_MUTEX(&adc_lock);
#else
		sema_init((&adc_lock),1);
#endif


	return 0;
}

int ADC_Exit_Workqueue(void)
{

	destroy_workqueue(gWorkqueue->WorkQueue);

	return 0;
}



/**
 *
 * VPORT_ADC_irq_handler irq handler
 *
 * @param	irq , device id , regs
 * @return	0 : OK , -1 : NOK
 *
*/
irqreturn_t VPORT_ADC_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	//int unused;
	int adc_irq_handled = 0;

	if ( lx_chip_rev( ) < LX_CHIP_REV( L8, B0 ) )
	{
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_soy1detU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy1detU\n");
	#endif
			if(gVportRegAx->cst_ro_hsdet.cst_ro_soy1det == 0)		// disappear comp 2 signal
			{
				gPortStatus[1].adc_input_port = 0;
				gPortStatus[1].adc_input_source_type = 0;
			}
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = 1<<0;
		}

		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_soy1detL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy1detL\n");
	#endif

			if((adc_irq_handled == 0) && (gVportRegAx->afe3ch_selmux.afe3ch_selmux == 0x1))
			{
				//ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}

			if(gVportRegAx->cst_ro_hsdet.cst_ro_soy1det == 1)		// appear comp 2 signal
			{
				gPortStatus[1].adc_input_port = LX_ADC_YPbPr_IN2;
				gPortStatus[1].adc_input_source_type = LX_ADC_INPUT_SOURCE_YPBPR;
			}



			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<1);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_soy0detU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy0detU\n");
	#endif

			if(gVportRegAx->cst_ro_hsdet.cst_ro_soy0det == 0)		// disappear comp 1 signal
			{
				gPortStatus[0].adc_input_port = 0;
				gPortStatus[0].adc_input_source_type = 0;
			}
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<2);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_soy0detL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy0detL\n");
	#endif

			if((adc_irq_handled == 0) && (gVportRegAx->afe3ch_selmux.afe3ch_selmux == 0x0))
			{
				//ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}

			if(gVportRegAx->cst_ro_hsdet.cst_ro_soy0det == 1)		// appear comp 1 signal
			{
				gPortStatus[0].adc_input_port = LX_ADC_YPbPr_IN1;
				gPortStatus[0].adc_input_source_type = LX_ADC_INPUT_SOURCE_YPBPR;
			}
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<3);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_sogdetU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("sogdetU\n");
	#endif

			if(gVportRegAx->cst_ro_hsdet.cst_ro_sogdet == 0)		// disappear RGB signal
			{
				gPortStatus[2].adc_input_port = 0;
				gPortStatus[2].adc_input_source_type = 0;
			}
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<4);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_sogdetL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("sogdetL\n");
	#endif

				// PC Screen Saver Interrupt Issue
				/*
			if((adc_irq_handled == 0) && ((gVportRegAx->afe3ch_selmux.afe3ch_selmux & 0x2) == 0x2))
			{
				// PC Screen Saver Interrupt Issue
				ADC_IFM((void *)&unused);
				adc_irq_handled = 1;
			}
			*/

			if(gVportRegAx->cst_ro_hsdet.cst_ro_sogdet == 1)		// appear RGB signal
			{
				gPortStatus[2].adc_input_port = LX_ADC_RGB_IN;
				gPortStatus[2].adc_input_source_type = LX_ADC_INPUT_SOURCE_RGB_PC;
			}
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<5);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_vsdetU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("vsdetU\n");
	#endif

			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<6);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_vsdetL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("vsdetL\n");
	#endif

			if((adc_irq_handled == 0) && ((gVportRegAx->afe3ch_selmux.afe3ch_selmux & 0x2) == 0x2))
			{
				//ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}


			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<7);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_hsdetU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("hsdetU\n");
	#endif

			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<8);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_cst_ro_hsdetL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("hsdetL\n");
	#endif

			if((adc_irq_handled == 0) && ((gVportRegAx->afe3ch_selmux.afe3ch_selmux & 0x2) == 0x2))
			{
	//			ADC_IFM_ISR((void *)&unused);

				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}


			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<9);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_resolution_change)		//
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("res_chg\n");
	#endif

			if(adc_irq_handled == 0)
			{
			//	ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}

	//		ADC_IFM((void *)&unused);

			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<10);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_no_signalU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("No_SigU\n");
	#endif
			if(adc_irq_handled == 0)
			{
			//	ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<11);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_no_signalL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("No_SigL\n");
	#endif
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<12);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_dpms_state_change)
		{
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<13);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_aogc_done)
		{
			ADC_SetAgocWakeup();
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<14);
		}
		if(gVportRegAx->vport_intr_status.adc3ch_aogc_err)
		{
			ADC_SetAgocWakeup();
			gVportRegAx->intr_vp2_clear.intr_vp2_clear = (1<<15);
		}
	}
	else	///////////////////////////////////////////////
	{

		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_soy1detU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy1detU\n");
	#endif
			if(gVportRegBx->ch3_cst_006.cst_ro_soy1det == 0)		// disappear comp 2 signal
			{
				gPortStatus[1].adc_input_port = 0;
				gPortStatus[1].adc_input_source_type = 0;
			}
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = 1<<0;
		}

		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_soy1detL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy1detL\n");
	#endif

			if((adc_irq_handled == 0) && (gVportRegBx->ch3_adc_004.afe3ch_selmux == 0x1))
			{
				//ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}

			if(gVportRegBx->ch3_cst_006.cst_ro_soy1det == 1)		// appear comp 2 signal
			{
				gPortStatus[1].adc_input_port = LX_ADC_YPbPr_IN2;
				gPortStatus[1].adc_input_source_type = LX_ADC_INPUT_SOURCE_YPBPR;
			}



			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<1);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_soy0detU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy0detU\n");
	#endif

			if(gVportRegBx->ch3_cst_006.cst_ro_soy0det == 0)		// disappear comp 1 signal
			{
				gPortStatus[0].adc_input_port = 0;
				gPortStatus[0].adc_input_source_type = 0;
			}
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<2);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_soy0detL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("soy0detL\n");
	#endif

			if((adc_irq_handled == 0) && (gVportRegBx->ch3_adc_004.afe3ch_selmux == 0x0))
			{
				//ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}

			if(gVportRegBx->ch3_cst_006.cst_ro_soy0det == 1)		// appear comp 1 signal
			{
				gPortStatus[0].adc_input_port = LX_ADC_YPbPr_IN1;
				gPortStatus[0].adc_input_source_type = LX_ADC_INPUT_SOURCE_YPBPR;
			}
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<3);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_sogdetU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("sogdetU\n");
	#endif

			if(gVportRegBx->ch3_cst_006.cst_ro_sogdet == 0)		// disappear RGB signal
			{
				gPortStatus[2].adc_input_port = 0;
				gPortStatus[2].adc_input_source_type = 0;
			}
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<4);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_sogdetL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("sogdetL\n");
	#endif

				// PC Screen Saver Interrupt Issue
				/*
			if((adc_irq_handled == 0) && ((gVportRegBx->ch3_adc_004.afe3ch_selmux & 0x2) == 0x2))
			{
				// PC Screen Saver Interrupt Issue
				ADC_IFM((void *)&unused);
				adc_irq_handled = 1;
			}
			*/

			if(gVportRegBx->ch3_cst_006.cst_ro_sogdet == 1)		// appear RGB signal
			{
				gPortStatus[2].adc_input_port = LX_ADC_RGB_IN;
				gPortStatus[2].adc_input_source_type = LX_ADC_INPUT_SOURCE_RGB_PC;
			}
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<5);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_vsdetU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("vsdetU\n");
	#endif

			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<6);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_vsdetL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("vsdetL\n");
	#endif

			if((adc_irq_handled == 0) && ((gVportRegBx->ch3_adc_004.afe3ch_selmux & 0x2) == 0x2))
			{
				//ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}


			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<7);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_hsdetU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("hsdetU\n");
	#endif

			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<8);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_cst_ro_hsdetL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("hsdetL\n");
	#endif

			if((adc_irq_handled == 0) && ((gVportRegBx->ch3_adc_004.afe3ch_selmux & 0x2) == 0x2))
			{
	//			ADC_IFM_ISR((void *)&unused);

				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}


			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<9);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_resolution_change)		//
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("res_chg\n");
	#endif

			if(adc_irq_handled == 0)
			{
			//	ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}

	//		ADC_IFM((void *)&unused);

			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<10);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_no_signalU)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("No_SigU\n");
	#endif
			if(adc_irq_handled == 0)
			{
			//	ADC_IFM_ISR((void *)&unused);
				queue_work(gWorkqueue->WorkQueue , &gWorkqueue->ifm);
				adc_irq_handled = 1;
			}
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<11);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_no_signalL)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("No_SigL\n");
	#endif
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<12);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_dpms_state_change)
		{
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<13);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_aogc_done)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("AOGC DONE\n");
	#endif
			ADC_SetAgocWakeup();
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<14);
		}
		if(gVportRegBx->intr_vp2_intr.adc3ch_aogc_err)
		{
	#ifdef IRQ_PRINT
			AFE_PRINT("AOGC ERROR\n");
	#endif
			ADC_SetAgocWakeup();
			gVportRegBx->intr_vp2_clear.intr_vp2_clear = (1<<15);
		}
	}


	return IRQ_HANDLED;
}

#if 0
void ADC_KernelRun(void)
{
	wq_condition = 1;
	wake_up_interruptible(&wq_kernel_test);

}

void ADC_KernelThreadTest(void)
{

	printk("start kernel thread\n");

	while(1)
	{
	//	wait_event_interruptible(wq_kernel_test , wq_condition);

		interruptible_sleep_on(&wq_kernel_test);
		wq_condition = 0;
		printk("kernel run one time\n");
	}

}


void ADC_RunTimer(unsigned long arg)
{

	printk("run timer \n");

	timer->expires += msecs_to_jiffies(1000);
	add_timer(timer);


}
#endif

/**
* ADC Module initialize
*
* @parm void
* @return int
*/


int ADC_Initialize(void)
{
	int ret;

	gResolution = -1;

#if 0
	if (lx_chip_rev() == LX_CHIP_REV_L8_A1)		    	pVportRegCfg = &gRegCfgVport[0];	// 0 : L8 A1 & B0 for Real.
	else if (lx_chip_rev() == LX_CHIP_REV_L8_B0)		pVportRegCfg = &gRegCfgVport[0];	// 0 : L8 A1 & B0 for Real.
	else if (lx_chip_rev() >= LX_CHIP_REV_L9_A0)		pVportRegCfg = &gRegCfgVport[1];	// 1 : L9 Ax
    else return -1;	// Unkown chip revision
#endif

#ifndef	USE_QEMU_SYSTEM
	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0)) // added by won.hur(won.hur@lge.com) @2011.08.17
	{
		ADC_PowerOn = 			ADC_L9Bx_PowerOn;
		ADC_PowerOff =                 ADC_L9Bx_PowerOff;
		ADC_InitDaughterBoard=         ADC_L9Bx_InitDaughterBoard;
		ADC_GetBoundaryInform=         ADC_L9Bx_GetBoundaryInform;
		ADC_SetTAUInform=              ADC_L9Bx_SetTAUInform;
		ADC_SetRGBPosition=            ADC_L9Bx_SetRGBPosition;
		ADC_GetRGBResult=              ADC_L9Bx_GetRGBResult;
		ADC_GetRGBPosition=            ADC_L9Bx_GetRGBPosition;
		ADC_SetRGBSize=                ADC_L9Bx_SetRGBSize;
		ADC_GetSizeValue=              ADC_L9Bx_GetSizeValue;
		ADC_SetPort=                   ADC_L9Bx_SetPort;
		ADC_GetPhaseValue=             ADC_L9Bx_GetPhaseValue;
		ADC_SetPhaseValue=             ADC_L9Bx_SetPhaseValue;
		ADC_GetScanType=               ADC_L9Bx_GetScanType;
		ADC_SetGainValue=              ADC_L9Bx_SetGainValue;
		ADC_GetGainValue=              ADC_L9Bx_GetGainValue;
		ADC_SetOffsetValue=            ADC_L9Bx_SetOffsetValue;
		ADC_GetOffsetValue=            ADC_L9Bx_GetOffsetValue;
		ADC_SetInputVideo=             ADC_L9Bx_SetInputVideo;
		ADC_Enable_Scart_Mode=         ADC_L9Bx_Enable_Scart_Mode;
		ADC_Vport_ALL_Power_Down=      ADC_L9Bx_Vport_ALL_Power_Down;
		ADC_Channel_Power_Control=		 ADC_L9Bx_Channel_Power_Control;
		ADC_SetType=                   ADC_L9Bx_SetType;
		ADC_Set_LLPLL=                 ADC_L9Bx_Set_LLPLL;
		ADC_SetClock=                  ADC_L9Bx_SetClock;
		ADC_AGOCInit=                  ADC_L9Bx_AGOCInit;
		ADC_GetSelectedPort=           ADC_L9Bx_GetSelectedPort;
		ADC_SetGainRGB=                ADC_L9Bx_SetGainRGB;
		ADC_GetGainRGB=                ADC_L9Bx_GetGainRGB;
		ADC_SetIRELevel=               ADC_L9Bx_SetIRELevel;
		ADC_GetOffsetRGB=              ADC_L9Bx_GetOffsetRGB;
		ADC_SetOffsetRGB=              ADC_L9Bx_SetOffsetRGB;
		ADC_GetSumRGB=                 ADC_L9Bx_GetSumRGB;
		ADC_SetAgocWakeup=             ADC_L9Bx_SetAgocWakeup;
		ADC_Reset_Digital=             ADC_L9Bx_Reset_Digital;
		ADC_Reset_Digital_24MHZ=       ADC_L9Bx_Reset_Digital_24MHZ;
		ADC_Reset_LLPLL=               ADC_L9Bx_Reset_LLPLL;
		ADC_Check_Sync_Exist=          ADC_L9Bx_Check_Sync_Exist;
		ADC_Sync_Polarity_Override=    ADC_L9Bx_Sync_Polarity_Override;
		ADC_Enable_Detection_Interrupt=ADC_L9Bx_Enable_Detection_Interrupt;
		ADC_Enable_AOGC_Interrupt=		ADC_L9Bx_Enable_AOGC_Interrupt;
		ADC_Mute_Enable=               ADC_L9Bx_Mute_Enable;
		ADC_Power_OnOff=               ADC_L9Bx_Power_OnOff;
		ADC_Ext_Clamp=                 ADC_L9Bx_Ext_Clamp;
		ADC_Set_Clamp=                 ADC_L9Bx_Set_Clamp;
		ADC_GetPCModeInfo=             ADC_L9Bx_GetPCModeInfo;
		ADC_Get_LLPLL_Filter_Status=   ADC_L9Bx_Get_LLPLL_Filter_Status;
		ADC_Set_Coast	=			 ADC_L9Bx_Set_Coast;
		ADC_Set_Hsync_Sel		= 	ADC_L9Bx_Set_Hsync_Sel;
		ADC_CVI_Sync_Exist		= ADC_L9Bx_CVI_Sync_Exist;
		ADC_Set_SOG_Level = ADC_L9Bx_Set_SOG_Level;
		ADC_Get_SOG_Level = ADC_L9Bx_Get_SOG_Level;
		ADC_Check_Htotal_Diff	= ADC_L9Bx_Check_Htotal_Diff;
		ADC_SetIRETestMode=		ADC_L9Bx_SetIRETestMode;
		ADC_Read_Pixel_Value	=	ADC_L9Bx_Read_Pixel_Value;
		ADC_Green_Mid_Level_Select	=	ADC_L9Bx_Green_Mid_Level_Select;
		ADC_Read_VSync_Width	=	ADC_L9Bx_Read_VSync_Width;
		ADC_Read_HSync_Width	=	ADC_L9Bx_Read_HSync_Width;
		ADC_Set_Blank=				ADC_L9Bx_Set_Blank;
		ADC_LVDS_Control=			ADC_L9Bx_LVDS_Control;
		ADC_Component_APA	=		ADC_L9Bx_Component_APA;
		ADC_Component_APA_Test	=	ADC_L9Bx_Component_APA_Test;
		ADC_Get_Vtotal_Value	= 	ADC_L9Bx_Get_Vtotal_Value;
		ADC_Set_Sync_Mode	= 	ADC_L9Bx_Set_Sync_Mode;
#ifdef	KDRV_CONFIG_PM
		ADC_RunSuspend=                ADC_L9Bx_RunSuspend;
		ADC_RunResume=                 ADC_L9Bx_RunResume;
#endif

	}
	else if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{
		ADC_PowerOn = 			ADC_L9Ax_PowerOn;
		ADC_PowerOff =                 ADC_L9Ax_PowerOff;
		ADC_InitDaughterBoard=         ADC_L9Ax_InitDaughterBoard;
		ADC_GetBoundaryInform=         ADC_L9Ax_GetBoundaryInform;
		ADC_SetTAUInform=              ADC_L9Ax_SetTAUInform;
		ADC_SetRGBPosition=            ADC_L9Ax_SetRGBPosition;
		ADC_GetRGBResult=              ADC_L9Ax_GetRGBResult;
		ADC_GetRGBPosition=            ADC_L9Ax_GetRGBPosition;
		ADC_SetRGBSize=                ADC_L9Ax_SetRGBSize;
		ADC_GetSizeValue=              ADC_L9Ax_GetSizeValue;
		ADC_SetPort=                   ADC_L9Ax_SetPort;
		ADC_GetPhaseValue=             ADC_L9Ax_GetPhaseValue;
		ADC_SetPhaseValue=             ADC_L9Ax_SetPhaseValue;
		ADC_GetScanType=               ADC_L9Ax_GetScanType;
		ADC_SetGainValue=              ADC_L9Ax_SetGainValue;
		ADC_GetGainValue=              ADC_L9Ax_GetGainValue;
		ADC_SetOffsetValue=            ADC_L9Ax_SetOffsetValue;
		ADC_GetOffsetValue=            ADC_L9Ax_GetOffsetValue;
		ADC_SetInputVideo=             ADC_L9Ax_SetInputVideo;
		ADC_Enable_Scart_Mode=         ADC_L9Ax_Enable_Scart_Mode;
		ADC_Vport_ALL_Power_Down=      ADC_L9Ax_Vport_ALL_Power_Down;
		ADC_Channel_Power_Control=		 ADC_L9Ax_Channel_Power_Control;
		ADC_SetType=                   ADC_L9Ax_SetType;
		ADC_Set_LLPLL=                 ADC_L9Ax_Set_LLPLL;
		ADC_SetClock=                  ADC_L9Ax_SetClock;
		ADC_AGOCInit=                  ADC_L9Ax_AGOCInit;
		ADC_GetSelectedPort=           ADC_L9Ax_GetSelectedPort;
		ADC_SetGainRGB=                ADC_L9Ax_SetGainRGB;
		ADC_GetGainRGB=                ADC_L9Ax_GetGainRGB;
		ADC_SetIRELevel=               ADC_L9Ax_SetIRELevel;
		ADC_GetOffsetRGB=              ADC_L9Ax_GetOffsetRGB;
		ADC_SetOffsetRGB=              ADC_L9Ax_SetOffsetRGB;
		ADC_GetSumRGB=                 ADC_L9Ax_GetSumRGB;
		ADC_SetAgocWakeup=             ADC_L9Ax_SetAgocWakeup;
		ADC_Reset_Digital=             ADC_L9Ax_Reset_Digital;
		ADC_Reset_Digital_24MHZ=       ADC_L9Ax_Reset_Digital_24MHZ;
		ADC_Reset_LLPLL=               ADC_L9Ax_Reset_LLPLL;
		ADC_Check_Sync_Exist=          ADC_L9Ax_Check_Sync_Exist;
		ADC_Sync_Polarity_Override=    ADC_L9Ax_Sync_Polarity_Override;
		ADC_Enable_Detection_Interrupt=ADC_L9Ax_Enable_Detection_Interrupt;
		ADC_Enable_AOGC_Interrupt=		ADC_L9Ax_Enable_AOGC_Interrupt;
		ADC_Mute_Enable=               ADC_L9Ax_Mute_Enable;
		ADC_Power_OnOff=               ADC_L9Ax_Power_OnOff;
		ADC_Ext_Clamp=                 ADC_L9Ax_Ext_Clamp;
		ADC_Set_Clamp=                 ADC_L9Ax_Set_Clamp;
		ADC_GetPCModeInfo=             ADC_L9Ax_GetPCModeInfo;
		ADC_Get_LLPLL_Filter_Status=   ADC_L9Ax_Get_LLPLL_Filter_Status;
		ADC_Set_Coast	=			 ADC_L9Ax_Set_Coast;
		ADC_Set_Hsync_Sel		= 	ADC_L9Ax_Set_Hsync_Sel;
		ADC_CVI_Sync_Exist		= ADC_L9Ax_CVI_Sync_Exist;
		ADC_Set_SOG_Level = ADC_L9Ax_Set_SOG_Level;
		ADC_Get_SOG_Level = ADC_L9Ax_Get_SOG_Level;
		ADC_Check_Htotal_Diff	= ADC_L9Ax_Check_Htotal_Diff;
		ADC_SetIRETestMode=		ADC_L9Ax_SetIRETestMode;
		ADC_Read_Pixel_Value	=	ADC_L9Ax_Read_Pixel_Value;
		ADC_Green_Mid_Level_Select	=	ADC_L9Ax_Green_Mid_Level_Select;
		ADC_Read_VSync_Width	=	ADC_L9Ax_Read_VSync_Width;
		ADC_Read_HSync_Width	=	ADC_L9Ax_Read_HSync_Width;
		ADC_Set_Blank=				ADC_L9Ax_Set_Blank; // added by won.hur(won.hur@lge.com) @2011.08.17
		ADC_LVDS_Control=			ADC_L9Ax_LVDS_Control; // added by won.hur(won.hur@lge.com) @2011.08.17
#ifdef	KDRV_CONFIG_PM
		ADC_RunSuspend=                ADC_L9Ax_RunSuspend;
		ADC_RunResume=                 ADC_L9Ax_RunResume;
#endif

	}
#ifdef L8_KDRIVER_SUPPORT
	else if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
	{
#if 0
		gVportRegBx = (volatile VPORT_RegBx_T *)ioremap(VPORT_REG_BASE_ADDR , VPORT_REG_SIZE);

		if(gVportRegBx == NULL)
		{
			AFE_ERROR("ERROR : can't allocate for register\n");
			return -1;
		}

		gpRegVPORT = (VPORT_RegBx_T *)kmalloc(sizeof(VPORT_RegBx_T), GFP_KERNEL);

		if(gpRegVPORT == NULL)
		{
			AFE_ERROR("ERROR : can't allocate for shadow register\n");
			return -1;
		}
#endif

		ADC_PowerOn = 			ADC_PowerOn_Bx;
		ADC_PowerOff =                 ADC_PowerOff_Bx;
		ADC_InitDaughterBoard=         ADC_InitDaughterBoard_Bx;
		ADC_GetBoundaryInform=         ADC_GetBoundaryInform_Bx;
		ADC_SetTAUInform=              ADC_SetTAUInform_Bx;
		ADC_SetRGBPosition=            ADC_SetRGBPosition_Bx;
		ADC_GetRGBResult=              ADC_GetRGBResult_Bx;
		ADC_GetRGBPosition=            ADC_GetRGBPosition_Bx;
		ADC_SetRGBSize=                ADC_SetRGBSize_Bx;
		ADC_GetSizeValue=              ADC_GetSizeValue_Bx;
		ADC_SetPort=                   ADC_SetPort_Bx;
		ADC_GetPhaseValue=             ADC_GetPhaseValue_Bx;
		ADC_SetPhaseValue=             ADC_SetPhaseValue_Bx;
		ADC_GetScanType=               ADC_GetScanType_Bx;
		ADC_SetGainValue=              ADC_SetGainValue_Bx;
		ADC_GetGainValue=              ADC_GetGainValue_Bx;
		ADC_SetOffsetValue=            ADC_SetOffsetValue_Bx;
		ADC_GetOffsetValue=            ADC_GetOffsetValue_Bx;
		ADC_SetInputVideo=             ADC_SetInputVideo_Bx;
		ADC_Enable_Scart_Mode=         ADC_Enable_Scart_Mode_Bx;
		ADC_Vport_ALL_Power_Down=      ADC_Vport_ALL_Power_Down_Bx;
		ADC_Channel_Power_Control=		 ADC_Channel_Power_Control_Bx;
		ADC_SetType=                   ADC_SetType_Bx;
		ADC_Set_LLPLL=                 ADC_Set_LLPLL_Bx;
		ADC_SetClock=                  ADC_SetClock_Bx;
		ADC_AGOCInit=                  ADC_AGOCInit_Bx;
		ADC_GetSelectedPort=           ADC_GetSelectedPort_Bx;
		ADC_SetGainRGB=                ADC_SetGainRGB_Bx;
		ADC_GetGainRGB=                ADC_GetGainRGB_Bx;
		ADC_SetIRELevel=               ADC_SetIRELevel_Bx;
		ADC_GetOffsetRGB=              ADC_GetOffsetRGB_Bx;
		ADC_SetOffsetRGB=              ADC_SetOffsetRGB_Bx;
		ADC_GetSumRGB=                 ADC_GetSumRGB_Bx;
		ADC_SetAgocWakeup=             ADC_SetAgocWakeup_Bx;
		ADC_Reset_Digital=             ADC_Reset_Digital_Bx;
		ADC_Reset_Digital_24MHZ=       ADC_Reset_Digital_24MHZ_Bx;
		ADC_Reset_LLPLL=               ADC_Reset_LLPLL_Bx;
		ADC_Check_Sync_Exist=          ADC_Check_Sync_Exist_Bx;
		ADC_Sync_Polarity_Override=    ADC_Sync_Polarity_Override_Bx;
		ADC_Enable_Detection_Interrupt=ADC_Enable_Detection_Interrupt_Bx;
		ADC_Enable_AOGC_Interrupt=		ADC_Enable_AOGC_Interrupt_Bx;
		ADC_Mute_Enable=               ADC_Mute_Enable_Bx;
		ADC_Power_OnOff=               ADC_Power_OnOff_Bx;
		ADC_Ext_Clamp=                 ADC_Ext_Clamp_Bx;
		ADC_Set_Clamp=                 ADC_Set_Clamp_Bx;
		ADC_GetPCModeInfo=             ADC_GetPCModeInfo_Bx;
		ADC_Get_LLPLL_Filter_Status=   ADC_Get_LLPLL_Filter_Status_Bx;
		ADC_Set_Coast	=			 ADC_Set_Coast_Bx;
		ADC_Set_Hsync_Sel		= 	ADC_Set_Hsync_Sel_Bx;
		ADC_CVI_Sync_Exist		= ADC_CVI_Sync_Exist_Bx;
		ADC_Set_SOG_Level = ADC_Set_SOG_Level_Bx;
		ADC_Get_SOG_Level = ADC_Get_SOG_Level_Bx;
		ADC_Check_Htotal_Diff	= ADC_Check_Htotal_Diff_Bx;
		ADC_SetIRETestMode=		ADC_SetIRETestMode_Bx;
		ADC_Read_Pixel_Value	=	ADC_Read_Pixel_Value_Bx;
		ADC_Green_Mid_Level_Select	=	ADC_Green_Mid_Level_Select_Bx;
		ADC_Read_VSync_Width	=	ADC_Read_VSync_Width_Bx;
		ADC_Read_HSync_Width	=	ADC_Read_HSync_Width_Bx;
#ifdef	KDRV_CONFIG_PM
		ADC_RunSuspend=                ADC_RunSuspend_Bx;
		ADC_RunResume=                 ADC_RunResume_Bx;
#endif
	}
	else
	{
#if 0
		gVportRegAx = (volatile VPORT_RegAx_T *)ioremap(VPORT_REG_BASE_ADDR , VPORT_REG_SIZE);
		if(gVportRegAx == NULL)
		{
			AFE_ERROR("ERROR : can't allocate for register\n");
			return -1;
		}
#endif
		ADC_PowerOn = 			ADC_PowerOn_Ax;
		ADC_PowerOff =                 ADC_PowerOff_Ax;
		ADC_InitDaughterBoard=         ADC_InitDaughterBoard_Ax;
		ADC_GetBoundaryInform=         ADC_GetBoundaryInform_Ax;
		ADC_SetTAUInform=              ADC_SetTAUInform_Ax;
		ADC_SetRGBPosition=            ADC_SetRGBPosition_Ax;
		ADC_GetRGBResult=              ADC_GetRGBResult_Ax;
		ADC_GetRGBPosition=            ADC_GetRGBPosition_Ax;
		ADC_SetRGBSize=                ADC_SetRGBSize_Ax;
		ADC_GetSizeValue=              ADC_GetSizeValue_Ax;
		ADC_SetPort=                   ADC_SetPort_Ax;
		ADC_GetPhaseValue=             ADC_GetPhaseValue_Ax;
		ADC_SetPhaseValue=             ADC_SetPhaseValue_Ax;
		ADC_GetScanType=               ADC_GetScanType_Ax;
		ADC_SetGainValue=              ADC_SetGainValue_Ax;
		ADC_GetGainValue=              ADC_GetGainValue_Ax;
		ADC_SetOffsetValue=            ADC_SetOffsetValue_Ax;
		ADC_GetOffsetValue=            ADC_GetOffsetValue_Ax;
		ADC_SetInputVideo=             ADC_SetInputVideo_Ax;
		ADC_Enable_Scart_Mode=         ADC_Enable_Scart_Mode_Ax;
		ADC_Vport_ALL_Power_Down=      ADC_Vport_ALL_Power_Down_Ax;
		ADC_Channel_Power_Control=		 ADC_Channel_Power_Control_Ax;
		ADC_SetType=                   ADC_SetType_Ax;
		ADC_Set_LLPLL=                 ADC_Set_LLPLL_Ax;
		ADC_SetClock=                  ADC_SetClock_Ax;
		ADC_AGOCInit=                  ADC_AGOCInit_Ax;
		ADC_GetSelectedPort=           ADC_GetSelectedPort_Ax;
		ADC_SetGainRGB=                ADC_SetGainRGB_Ax;
		ADC_GetGainRGB=                ADC_GetGainRGB_Ax;
		ADC_SetIRELevel=               ADC_SetIRELevel_Ax;
		ADC_GetOffsetRGB=              ADC_GetOffsetRGB_Ax;
		ADC_SetOffsetRGB=              ADC_SetOffsetRGB_Ax;
		ADC_GetSumRGB=                 ADC_GetSumRGB_Ax;
		ADC_SetAgocWakeup=             ADC_SetAgocWakeup_Ax;
		ADC_Reset_Digital=             ADC_Reset_Digital_Ax;
		ADC_Reset_Digital_24MHZ=       ADC_Reset_Digital_24MHZ_Ax;
		ADC_Reset_LLPLL=               ADC_Reset_LLPLL_Ax;
		ADC_Check_Sync_Exist=          ADC_Check_Sync_Exist_Ax;
		ADC_Sync_Polarity_Override=    ADC_Sync_Polarity_Override_Ax;
		ADC_Enable_Detection_Interrupt=ADC_Enable_Detection_Interrupt_Ax;
		ADC_Enable_AOGC_Interrupt=		ADC_Enable_AOGC_Interrupt_Ax;
		ADC_Mute_Enable=               ADC_Mute_Enable_Ax;
		ADC_Power_OnOff=               ADC_Power_OnOff_Ax;
		ADC_Ext_Clamp=                 ADC_Ext_Clamp_Ax;
		ADC_Set_Clamp=                 ADC_Set_Clamp_Ax;
		ADC_GetPCModeInfo=             ADC_GetPCModeInfo_Ax;
		ADC_Get_LLPLL_Filter_Status=   ADC_Get_LLPLL_Filter_Status_Ax;
		ADC_SetIRETestMode=		ADC_SetIRETestMode_Ax;
		ADC_Set_Coast	=			 ADC_Set_Coast_Ax;
		ADC_Set_Hsync_Sel		= 	ADC_Set_Hsync_Sel_Ax;
		ADC_CVI_Sync_Exist		= ADC_CVI_Sync_Exist_Ax;
		ADC_Set_SOG_Level = ADC_Set_SOG_Level_Ax;
		ADC_Get_SOG_Level = ADC_Get_SOG_Level_Ax;
		ADC_Check_Htotal_Diff	= ADC_Check_Htotal_Diff_Ax;
		ADC_Read_Pixel_Value	=	ADC_Read_Pixel_Value_Ax;
		ADC_Green_Mid_Level_Select	=	ADC_Green_Mid_Level_Select_Ax;
		ADC_Read_VSync_Width	=	ADC_Read_VSync_Width_Ax;
		ADC_Read_HSync_Width	=	ADC_Read_HSync_Width_Ax;
		#ifdef	KDRV_CONFIG_PM
		ADC_RunSuspend=                ADC_RunSuspend_Ax;
		ADC_RunResume=                 ADC_RunResume_Ax;
		#endif
	}
#else //#ifdef L8_KDRIVER_SUPPORT
	else
	{
		return -1;
	}

#endif //#ifdef L8_KDRIVER_SUPPORT

#else
	gVportRegAx = (volatile VPORT_RegAx_T *)ioremap(VPORT_REG_QEMU_BASE_ADDR , VPORT_REG_SIZE);
#endif


	//Initialize ADC semaphore
	OS_InitMutex(&g_ADC_Sema, OS_SEM_ATTR_DEFAULT);

	if (lx_chip_rev() < LX_CHIP_REV(L9, A0))
	{
	   ADC_Vport_ALL_Power_Down(); // From CMM afe All power down !!!!
	}

	if(ADC_Init_Workqueue() < 0)
	{
		ret = -1;
	}
	else
	{
		ret = 0;
	}

	if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{

	}
	else
	{
		if (request_irq(L8_IRQ_VP_3CH,(irq_handler_t)VPORT_ADC_irq_handler,0,"adc", NULL)) {
			AFE_ERROR("request_irq  is failed\n");
		}
	}

	// present input source status
	memset(&gPortStatus , 0 , sizeof(LX_AFE_ADC_SET_INPUT_T)*3);


	ADC_InitDaughterBoard();

#ifdef USE_ADC_TIMER
   OS_InitTimer( &adc_timer);
   OS_StartTimer( &adc_timer, ADC_Timer_Routine, OS_TIMER_TIMETICK, ADC_TIMEOUT, 0 );
#endif

	gPCTable = NULL;

#ifdef ADC_PERIODIC_SIGNAL_INFO_READ
	//Disable Detection Interrupt
	ADC_Enable_Detection_Interrupt(0);
#else
	ADC_Enable_Detection_Interrupt(1);
#endif

			stADC_Thread = kthread_create( ADC_Thread, (void*)NULL, "adc_thread");

			if(stADC_Thread)
			{
				wake_up_process(stADC_Thread);
				//gCVD_thread_running = 1;
				AFE_PRINT("ADC Thread [%d]\n", stADC_Thread->pid);
			}


			else
			{
				AFE_ERROR("ADC Thread Already Created\n");
			}
	// for test
#if 0
	kernel_pid = kernel_thread((void *)&ADC_KernelThreadTest, (void*)NULL, CLONE_KERNEL);

	timer = (struct timer_list *)kmalloc(sizeof(struct timer_list), GFP_KERNEL);

	init_timer(timer);

	timer->expires = msecs_to_jiffies(1000) + jiffies;
	timer->function = ADC_RunTimer;
	timer->data = 0;

	add_timer(timer);

	printk("timer return is %d %d\n" , ret , (int)timer->expires);

	ret = 0;
#endif
	return ret;
}

#if 0
int ADC_RunRegister(S_INTERFACE_REG *reg)
{
	if(reg->read_write)	// write
	{
		ADC_SetI2CData(reg->sub , reg->addr , reg->size , reg->value);

	}
	else			// read
	{
		ADC_GetI2CData(reg->sub , reg->addr , reg->size);

	}

	return 0;
}
#endif
