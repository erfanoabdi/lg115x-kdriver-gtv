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
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author		wonsik.do
 *  @version	1.0
 *  @date		2010-11-16
 *  @note		Additional information.
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <mach/irqs.h>
#include <linux/kthread.h>
#include <linux/signal.h>
#include <linux/semaphore.h>

#include "afe_drv.h"

#include "os_util.h"
//#include "hdmi_phy_reg.h"

#include "linux/delay.h"
#include "afe_cfg.h"
#include "vport_reg.h"
#include "vport_reg_B0.h"
#include "l9/vport_reg_l9a0.h"
#include "l9/chbcvd_reg_l9.h"

#include "l9/vport_reg_l9b0.h" // added by won.hur for L9B0

#include "cvd_hw.h"
//#include "cvd_control.h"
#include "pe_dcm.h"

#include <asm/io.h>			/**< For ioremap_nocache */
#include <linux/slab.h>

/************** L9Bx CVD New PE Settings ******************************/
#include "cvd_pe/CVD_L_AV_NTSC_443_default.h"
#include "cvd_pe/CVD_L_AV_NTSC_M_default.h"
#include "cvd_pe/CVD_L_AV_PAL60_default.h"
#include "cvd_pe/CVD_L_AV_PALI_default.h"
#include "cvd_pe/CVD_L_AV_PALM_default.h"
#include "cvd_pe/CVD_L_AV_PALNc_default.h"
#include "cvd_pe/CVD_L_AV_SECAM_default.h"
#include "cvd_pe/CVD_L_RF_NTSC_443_default.h"
#include "cvd_pe/CVD_L_RF_NTSC_M_default.h"
#include "cvd_pe/CVD_L_RF_PAL60_default.h"
#include "cvd_pe/CVD_L_RF_PALI_default.h"
#include "cvd_pe/CVD_L_RF_PALM_default.h"
#include "cvd_pe/CVD_L_RF_PALNc_default.h"
#include "cvd_pe/CVD_L_RF_SECAM_default.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define	MAINCVD_TIMEOUT	150	// 1000 msec
#define	L9_CVD_TIMEOUT	50	// 1000 msec
#define	CHBCVD_TIMEOUT	200 // 1000 msec

#define USE_REGISTER_VALUE_FOR_SYNC_STATES

#define CVD_CH42_SKIP_WORKAROUND
//#undef CVD_CH42_SKIP_WORKAROUND

#define ENABLE_CVD_THREAD				///<< Must define this to make CVD driver work.
//#define ENABLE_CVD_TIMER				1

#define CVD_HSTATE_FIX_VCR_COUNT_THRESHOLD	10

#define CVD_STATE_TRANSITION_MAX	15

#define ENABLE_CVD_STATE_TRANSITION_LIMIT 1
#define ENABLE_CVD_INTERRUPT_CONTROL 0

#define CVD_FC_FOR_SECAM_LOW	190
#define CVD_FC_FOR_SECAM_HIGH	240
#define CVD_FC_FOR_PALCn_LOW	170
#define CVD_FC_FOR_PALCn_HIGH	220
//#define CVD_KERNEL_DEBUG
//#define CHB_CVD_KERNEL_DEBUG
#define	SYSTEM_DETECTION_BY_CORDIC_ONLY
//#define	SYSTEM_DETECTION_BY_CORDIC_CS0_CS1
#define CVD_L9B0_NOISE_MAX	0x3FF

#define DCRESTORE_ACCUM_WIDTH_DEFAULT		0x1B
#define DCRESTORE_ACCUM_WIDTH_FOR_WEAK_RF	0x22
#define DCRESTORE_ACCUM_WIDTH_INITIAL			0x25
#define NOISE_THRESHOLD_DEFAULT				0x32
#define NOISE_THRESHOLD_WEAK_RF				0x80

// Workaround for stable AGC on EBC field stream
#define L9_WA_FOR_EBS_STABLE_AGC
#define	CVD_STABLE_COUNT_1	1
#define	CVD_STABLE_COUNT_2	20
#define CVD_ADAPTIVE_BURST_GATE_COUNT_ITERATION	20	// 20 is about 1 sec.
#define CVD_ADAPTIVE_BURST_GATE_THRESHOLD_0	0x3D090		// Threshold for enter CDTO value(hex value of 250K)
#define CVD_ADAPTIVE_BURST_GATE_THRESHOLD_1	0xCE4			// Threshold for CORDIC FREQ VALUE *20 (hex value of 3.3k)
#define CVD_ADAPTIVE_BURST_GATE_THRESHOLD_3	0x27100		// Threshold for leave CDTO value(hex value of 160K)
#define CVD_STABLE_COUNT_FOR_AGC_CONTROL	20

// Workaround for chroma lost on brasil streams : 20120202
 #define L9_DONT_KILL_CHROMALOCK_ON_CLOCK_LOST
#define L9_ADAPTIVE_BURST_GATE_END_MODE
#define L9_BYPASS_AGC_ON_ADAPTIVE_BURST_GATE_END_MODE

// Workaround for DONG_GO_DONG_RAK stream :	20120208
#define L9_CHANGE_FIELD_DETECT_MODE_ON_DONG_GO_DONG_RAK

// moved to cvd_hw.h
//#define L9_FAST_3DCOMB_WORKAROUND

// Workaround for stable AGC on weak RF signal
//#define CVD_AGC_PEAK_CONTROL_WORKAROUND
//#define L9_ADAPTIVE_AGC_PEAK_NOMINAL_CONTROL
#define AGC_PEAK_NOMINAL_INITIAL_VALUE	0x30
#define AGC_PEAK_NOMINAL_DEFAULT_VALUE	0x0a

//Workaround for PERU noisy field signal(low freq cut-off, status_noise is 0x3ff), flickering and unstable sync
#define L9_AGC_BYPASS_WA_FOR_NOISY_SIGNAL

//Workaround for Field Stream(양천구 신월동 11채널 영상 Sync흔들림)
//Set hstate_fixed to '1' on NTSC RF signal
#define L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC

//Workaround for Field Stream(대전 대덕구 화면 펄럭임발생 문제 : 수직 동기 떨림 빈도 100%)
//Set vsync_cntl 0x2, vsync_cntl_noisy 0x1, vloop_tc 0x3
//#define L9_VSYNC_CNTL_FOR_STABLE_VSYNC

#define L9_ADAPTIVE_AGC_PEAK_EN_CONTROL
#define NOISE_TH_FOR_PEAK_EN_CONTROL		0x170
#define AGC_GAIN_TH_FOR_PEAK_EN_CONTROL		0xB80
/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
//	function pointer
void (*CVD_Program_Color_System_Main_Multi)(CVD_SET_SYSTEM_MAIN_T *pSet_system_t);
void (*CVD_Set_Picture_Enhancement)(BOOLEAN ycrdc_lcr_on, BOOLEAN cyrdc_lcr_on);
void (*CVD_Program_Color_System_CHB_NTSC)(void);
void (*CVD_SW_Reset)(LX_AFE_CVD_SELECT_T select_main_sub);
void (*CVD_Power_Down)(LX_AFE_CVD_SELECT_T select_main_sub, BOOLEAN PowerOnOFF);
UINT8 (*CVD_Get_FC_Flag)(LX_AFE_CVD_SELECT_T select_main_sub);
#if 1 //gogosing burst mag status check for color burst level test (color 틀어짐 대응) 2011.06.11
UINT8 (*CVD_Get_CVD_Burst_Mag_Flag)(CVD_STATE_T	color_system);
int (*CVD_Set_CVD_CAGC)(UINT8 state,CVD_SET_SYSTEM_MAIN_T *pSet_system_t, CVD_STATE_T	color_system);
#endif
UINT8 (*CVD_Get_Cordic_Freq)(LX_AFE_CVD_SELECT_T select_main_sub);
void (*CVD_Program_Color_System_Main)(CVD_SET_SYSTEM_MAIN_T set_system_t);
void (*CVD_Program_Color_System_CHB)(CVD_SET_SYSTEM_CHB_T set_system_t);

int (*CVD_Set_Source_Type)(UINT32 arg);
int (*CVD_Clamp_Current_Control)(UINT8 value);
int (*CVD_Force_Color_Kill)(UINT8 color_kill_enable);

int (*CVD_Set_Scart_Overlay)(BOOLEAN arg);
void (*CVD_Reg_Init)(LX_AFE_CVD_SELECT_T select_main_sub);
int (*CVD_Get_Scart_AR)(UINT32 arg);
void (*CVD_Print_Vport_Version)(void);
int (*CVD_Vport_Output)(UINT32 arg);
int (*CVD_Vport_Reg_Read)(UINT32 arg);
int (*CVD_Vport_Reg_Write)(UINT32 arg);
int (*CVD_Get_FB_Status)(UINT32 arg);
int (*CVD_Get_No_Signal_Flag)(void);
int (*CVD_Get_HLock_Flag)(void);
int (*CVD_Get_VLock_Flag)(void);
int (*CVD_Get_Vline_625_Flag)(void);
int	(*CVD_Get_PAL_Flag)(void);
int	(*CVD_Get_SECAM_Flag)(void);
int	(*CVD_Get_Chromalock_Flag)(void);
int	(*CVD_Get_Noise_Status)(void);
int	(*CVD_Get_NoBurst_Flag)(void);
int	(*CVD_Get_CHB_No_Signal_Flag)(void);
int	(*CVD_Get_CHB_HLock_Flag)(void);
int	(*CVD_Get_CHB_VLock_Flag)(void);
int	(*CVD_Get_CHB_Chromalock_Flag)(void);
int (*CVD_Get_CHB_PAL_Flag)(void);
int (*CVD_Get_CHB_Vline_625_Flag)(void);
int (*CVD_Get_CHB_SECAM_Flag)(void);
int (*CVD_Reset_irisyc)(int enable);
int (*CVD_Reset_hdct)(int enable);
int (*CVD_Reset_cdct)(int enable);
int	(*CVD_Get_PAL_Flag_CS0)(void);
int	(*CVD_Get_SECAM_Flag_CS0)(void);
int	(*CVD_Get_Chromalock_Flag_CS0)(void);
UINT8 (*CVD_Get_FC_Flag_CS0)(void);
UINT8 (*CVD_Get_Cordic_Freq_CS0)(void);
int	(*CVD_Get_PAL_Flag_CS1)(void);
int	(*CVD_Get_SECAM_Flag_CS1)(void);
int	(*CVD_Get_Chromalock_Flag_CS1)(void);
UINT8 (*CVD_Get_FC_Flag_CS1)(void);
UINT8 (*CVD_Get_Cordic_Freq_CS1)(void);
/*
void (*CVD_Set_AAF_Params)(CVD_SET_AAF_T *pSet_aaf_t);
void (*CVD_Set_ADC_Params)(CVD_SET_CRES54MHZ_T *pSet_cresampler_t);
void (*CVD_Set_Cresampler_Params)(CVD_SET_CRES54MHZ_T *pSet_cresampler_t);
void (*CVD_Set_Dot_Crawl_Params)(CVD_SET_DOT_CRAWL_T *pSet_dotcrawl_t);
void (*CVD_Set_Cross_Color_Reducer)(CVD_SET_CROSS_COLOR_REDUCER_T *pSet_crosscolor_t);
void (*CVD_Set_Motion_Detector)(CVD_SET_MOTION_DETECTOR_T *pSet_motiondetector_t);
void (*CVD_Set_Global_Classifier)(   CVD_SET_GLOBAL_CLASSIFIER_T *pSet_globalclassifier);
void (*CVD_Set_Output_Delay)( CVD_SET_OUPUT_DELAY_T        	*pSet_outputdelay_t);
void (*CVD_Set_Output_Range)(CVD_SET_OUPUT_RANGE_T	*pSet_outputrange_t);
void (*CVD_Set_Blending)(CVD_SET_BLENDING_T	*pSet_blending);
*/
int	(*CVD_Read_fld_cnt_value)(UINT16 *pfld_hfcnt_value, UINT16 *pfld_lfcnt_value);
int (*CVD_Set_motion_mode)(UINT8	md_mode_value, UINT8 motion_mode_value) ;
int (*CVD_Channel_Power_Control)(UINT32 on_off) ;
int (*CVD_Reset_Clampagc)(void) ;
int (*CVD_Bypass_Control)(LX_AFE_CVD_BYPASS_CONTROL_T *cvd_bypass_control_t);

int (*CVD_Get_Vline_625_Reg)(void);	// Read user forced vline 625 value
void (*CVD_Program_Color_System_PreJob)(CVD_SET_SYSTEM_MAIN_T *pSet_system_t);
int (*CVD_Set_Hstate_Max)(UINT32	hstate_max_value);
int (*CVD_OnOff_Chromalock_Ckill)(BOOLEAN bonoff);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
//extern volatile VPORT_RegAx_T *gVportRegAx;
//extern volatile VPORT_RegBx_T *gVportRegBx;
extern LX_VPORT_REG_T *pVportRegCfg;

extern PE_DCM_DCE_HISTO_T _g_dce_histo_info[LX_PE_WIN_NUM];
/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
volatile VPORT_L9A0_REG_T __iomem *gpRegVPORT_L9A0;
volatile VPORT_L9A0_REG_T __iomem *gpRealRegVPORT_L9A0;
volatile VPORT_RegAx_T *gVportRegAx;
volatile VPORT_RegBx_T *gVportRegBx;
		 VPORT_RegBx_T *gpRegVPORT;		// for shadow register



volatile VPORT_L9B0_REG_T __iomem *gpRegVPORT_L9B0;	// won.hur(11.08.23) : L9B0 register added
volatile VPORT_L9B0_REG_T __iomem *gpRealRegVPORT_L9B0; // won.hur



BOOLEAN gCVDInitStatus = FALSE;

volatile CHBCVD_L9_REG_T __iomem *gpRegCHBCVD_L9;
volatile CHBCVD_L9_REG_T __iomem *gpRealRegCHBCVD_L9;

//DECLARE_WAIT_QUEUE_HEAD(WaitQueue_MainCVD);
//DECLARE_WAIT_QUEUE_HEAD(WaitQueue_CHBCVD);

//static int gMainCVD_condition = 0;
//static int gCHBCVD_condition = 0;

#ifdef ENABLE_CVD_TIMER
OS_TIMER_T		main_timer;
OS_TIMER_T		chb_timer;

int main_timer_state = 0;
int chb_timer_state = 0;
#endif

UINT8 gEnable_Status_Fld_Flag = 0;
UINT8 gEnable_Monitor_3DCOMB_State = 0;
UINT32 curr_g3DCOMB_State = 0;
UINT32 prev_g3DCOMB_State = 0;
UINT32	curr_g3DCOMB_Control = 0;
UINT32	prev_g3DCOMB_Control = 0;

spinlock_t 	cvd_lock;
ULONG		cvd_irq_flags = 0;

static int gCVD_State_Transition_Count = 0;
static UINT8 MainCVD_Same_State_Count=0;
#ifdef ENABLE_CVD_THREAD
struct task_struct	*stCVD_Thread;
//long	gPid_cvd_thread = -1;

int		gForce_thread_sleep = 0;
int		gCVD_thread_running = 0;
int		gWait_return = -1;
int		gAutoProgramming	= 0;
//wait_queue_head_t	WaitQueue_CVD;
DECLARE_WAIT_QUEUE_HEAD(WaitQueue_CVD);
#endif
#ifdef	KDRV_CONFIG_PM
typedef	struct{
	BOOLEAN CVDInitStatus;
	LX_AFE_CVD_SELECT_T SelectMainSub;
}S_AFE_PM_DATA;

S_AFE_PM_DATA *gAfePmData;
#endif

OS_SEM_T	g_CVD_Sema;

int gStable_AGC_WA_for_EBS_Enabled = 0;

BOOLEAN g_CVD_RF_Input_Mode = FALSE;
CVD_AGC_PEAK_CONTROL_T	g_CVD_AGC_Peak_Control;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static irqreturn_t CVD_L8_IRQ_Handler(int irq, void *dev_id, struct pt_regs *regs);
static irqreturn_t CVD_L9_IRQ_Handler(int irq, void *dev_id, struct pt_regs *regs);

#ifdef CVD_AGC_PEAK_CONTROL_WORKAROUND		//disabled
static int CVD_Adaptive_AGC_Peak_Control(CVD_AGC_PEAK_CONTROL_T *pCVD_AGC_Peak_Control );
#endif
//static int CVD_Adaptive_AGC_Peak_Control_using_motion(CVD_ADAPTIVE_PEAK_NOMINAL_CONTROL_T *pCVD_AGC_Peak_Nominal_Control );
#ifdef ENABLE_CVD_TIMER
static void MainCVD_Timer(ULONG data);
#endif
static int MainCVD_Thread(void *data);

#ifdef ENABLE_CVD_TIMER
static void CHBCVD_Timer(ULONG data);
#endif
static int	MainCVD_Work(void);
static int	CVD_Task_3cdetect(void);
#ifdef ENABLE_CVD_TIMER
static int CHBCVD_Work_NTSC(void);
#endif
static int CVD_AGC_Peak_En_Control(int force_peak_enable, int avg_noise, int avg_agc_gain);
static int CVD_NoBurst_CKill_Control(CVD_STATUS_3CS_T *pCVD_Status);
static int CVD_Pattern_Detection(CVD_PATTERN_DETECTION_T *pCVD_Pattern_Detection_t );
/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static BOOLEAN	ENABLE_ANALOGTTX = FALSE;
static BOOLEAN	ENABLE_ANALOGCC = FALSE;
static UINT8 gEnable_SW3DCOMBControl = 1;
static UINT32	gLf_thr_value = 0x280; //0x220;
static UINT32	gHf_thr_value= 0x300; //0x290;
static UINT32	gDiff_thr_value = 0x100;
static UINT32	gRegion_30_mode = 0;

static CVD_STATUS_3CS_T	gCVD_Status_3CS ;
#ifdef CVD_AGC_PEAK_CONTROL_WORKAROUND		//disabled
static UINT32	g_AGC_Peak_Nominal_Value = AGC_PEAK_NOMINAL_DEFAULT_VALUE;
#endif
static BOOLEAN gPortChangeDetected = FALSE;

CVD_STATE_T	gMainCVD_State = CVD_STATE_VideoNotReady;
CVD_STATE_T	gPrevious_Main_System = CVD_STATE_VideoNotReady;
CVD_STATE_T gCHBCVD_State = CVD_STATE_VideoNotReady;
BOOLEAN		gNTSC_ONLY_Main = FALSE;
BOOLEAN	 gNTSC_ONLY_CHB		= FALSE;
//LX_AFE_VIDEO_SYSTEM_T	g_Set_CVD_Color_System = LX_MULTI;
LX_AFE_CVD_SUPPORT_COLOR_SYSTEM_T	g_CVD_Color_System_Support = LX_COLOR_SYSTEM_MULTI;
CVD_STATE_T	Current_State = CVD_STATE_VideoNotReady;
CVD_STATE_T	Next_State = CVD_STATE_VideoNotReady;
CVD_STATE_T	Set_State;
int gExitFromForcedState = 0;

CVD_ADAPTIVE_PEAK_NOMINAL_CONTROL_T g_CVD_AGC_Peak_Nominal_Control ;
CVD_PATTERN_DETECTION_T g_CVD_Pattern_Detection_t ;

static CVD_SET_SYSTEM_MAIN_T gSetColorSystemMain[]=
{ // from L8_A0_Set_Color_Standard_Reg_100617.xls
//comb3_buffer_size			fld2_init_rd_pel			    /* new L9Bx 3DComb Buffer Value 				*/		cvd1_clampagc_on             		colour_mode    ped  	   hagc_en		  dagc_en	mv_hagc_mode   adaptive_mode	  adc_updn_swap	 	   	cdto_inc	hdtc_inc  	hactive_start	  vactive_height   burst_gate_end  	    noise_th   lowfreq_vdiff_gain  y_noise_th_gain	   motion_mode	  fb_sync		  vactive_md_start		  motion_config	  	chroma_level	 	       2dcomb_ccombo_sel   2dcomb_divider_th		  2dcomb_ycrdc_yvsel  	   2dcomb_ycrdc_lrtype 2dcomb_cyrdc_crvalid		 			 oadj_y_coeff	   oadj_y_offo		 hactive_width_54M			cstate			hrs_ha_start	vactive_start_L9B0	burst_get_start_L9B0  fb_vstart_even cagc_gate   cordic_gate burst_gate		sync_mid	  fb_height_half	  cagc_av
//																																     														<<<===  Not Used  ===>>  chroma_bw_lo		hagc			cagc_gate_start											    	 cagc		 secam_ybw																adaptive_chroma_mode				  hactive_md_start
// default
	{955500, 	716625, 	477750, 	238875, 	0x0, 	955500, 	716625, 	477750, 	238875, 	0x0, 	0x0, 0x08,	0x20, 0x1f, 0x3c, 0x6e, 0x0, 0x0, 0x0, 0x1, 0x3c0, 0x0, 0x1, 0x1, 0x0, 0x2, 0x1, 0x0, 0x0, 0x0, 0x1, 0, 0xDD, 0x0, 0x0, 0x32, 0x50, 0x21F07C1F, 0x20000000, 0x82, 0x50, 0x22, 0x61, 0x0, 0x8A, 0x46, 0x0, 0x0, 0x3, 0x02, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x0, 0x0, 0x0, 0x23, 0x0, 0x24, 0x5F, 0x6E, 0x50, 0x00, 0x00, 0x00, 0x0D, 0x0, 0x05, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x2, 0x020,0x000, 0x0, 0x0, 0x1000,0x3, 0x1b, 	0, 	  0x84,	 0x50,	0x06, 0x0, 	0x0, 	0x5, 0x0, 0x0,	0x82,	0x50,	0x23,	0x3c, 0x6e,	0x32, 0x46,		0x13, 0x14,	0x82	,0x32, 0x50, 0x3c, 0x6e, 0x32, 0x46,	0xFE,	0x80 , 0x60, 0x80,	0x80, 0x8a},	//default
   //NTSC-M																																																												//110418       //for NTSC burst shift(from 0x46)
   {0xE946A, 	0xAF4A7, 	0x75153, 	0x3AA71, 	0x71D, 	0xE946A, 	0xAF83B, 	0x74DC5, 	0x3AE05, 	0x38F, 	0x1, 0x08,	0x80, 0x18, 0x32, 0x46, 0x0, 0x0, 0x0, 0x1, 0x3c0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x0, 0x1, 0, 0xDD, 0x1, 0x0, 0x32, 0x50, 0x21F07C1F, 0x20000000, 0x82, 0x50, 0x22, 0x61, 0x0, 0x80, 0x4b, 0x0, 0x0, 0x3, 0x02, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x0, 0x0, 0x23, 0x0, 0x24, 0x5F, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x7, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x1, 0xDC0, 0x5, 0x1b,	0x40, 0x86,	 0x50,	0x18, 0x10,	0x1,	0x7, 0x0, 0x0,	0x7f,	0x50,	0x27,	0x32, 0x46,	0x32, 0x4b,		0x08, 0x09,	0x8a 	,0x34, 0x47, 0x34, 0x47, 0x34, 0x47,	0xF7,	0x82 , 0x60, 0x80,	0x80, 0x8a},	//NTSC M

   //NTSC-J,
   {0xE946A, 	0xAF4A7, 	0x75153, 	0x3AA71, 	0x71D, 	0xE946A, 	0xAE83B, 	0x74DC5, 	0x3AE05, 	0x71D, 	0x1, 0x08,	0x80, 0x18, 0x32, 0x46, 0x0, 0x0, 0x0, 0x0, 0x3c0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x0, 0x1, 0, 0xCD, 0x1, 0x0, 0x32, 0x50, 0x21F07C1F, 0x20000000, 0x82, 0x50, 0x22, 0x61, 0x0, 0x80, 0x46, 0x0, 0x0, 0x3, 0x02, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x0, 0x0, 0x23, 0x0, 0x24, 0x5F, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x7, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x1, 0xDC0, 0x5, 0x25,	0x40, 0x86,	 0x50,	0x18, 0x10,	0x1,	0x7, 0x0, 0x0,	0x7f,	0x50,	0x27,	0x32, 0x46,	0x32, 0x46,		0x08, 0x09,	0x8a 	,0x34, 0x47, 0x34, 0x47, 0x34, 0x47,	0xFE,	0x82 , 0x60, 0x80,	0x80, 0x8a},	//NTSC J

   //NTSC-443,
   {0 , 		0 , 		0 , 		0 , 		0 , 	0 , 		0 , 		0 , 		0 , 		0 , 	0x1, 0x08,	0x80, 0x18, 0x32, 0x46, 0x0, 0x0, 0x0, 0x1, 0x3c0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x0, 0x0, 0, 0xDD, 0x1, 0x0, 0x32, 0x50, 0x2A098ACB, 0x20000000, 0x82, 0x50, 0x22, 0x61, 0x1, 0x80, 0x46, 0x0, 0x0, 0x3, 0x02, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x1, 0x0, 0x23, 0x0, 0x24, 0x5F, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x0, 0xDC0, 0x5, 0x25,	0x40, 0x7c,	 0x50,	0x18, 0x10,	0x1,	0x7, 0x0, 0x0,	0x6c,	0x50,	0x27,	0x32, 0x46,	0x32, 0x46,		0x08, 0x09,	0x8a 	,0x34, 0x42, 0x34, 0x42, 0x34, 0x42,	0xFE,	0x82 , 0x60, 0x80,	0x80, 0x8a },	//NTSC 443

   //PAL-I,B,G,H,D,
   {0x15A604, 	0x103EBD, 	0xAD773, 	0x56BBA, 	0x1, 	0x2B4C0A, 	0x207D79, 	0x15AA76,	0xAD773, 	0x470, 	0x1, 0x08,	0x80, 0x18, 0x32, 0x5e, 0x1, 0x1, 0x1, 0x0, 0x3f0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x2, 0x1, 0, 0xDC, 0x1, 0x3, 0x32, 0x50, 0x2A098ACB, 0x20000000, 0x84, 0x50, 0x2A, 0xC1, 0x0, 0x80, 0x6E, 0x0, 0x0, 0x3, 0x0A, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x0, 0x0, 0x21, 0x0, 0x2c, 0xbf, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x7, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x1, 0xDC0, 0x5, 0x25,	0x40, 0x88,	 0x50,	0x18, 0x10, 0x1,	0x7, 0x0, 0x0,	0x7b,	0x50,	0x2f,	0x32, 0x5e,	0x32, 0x6E,		0x08, 0x08,	0x93 	,0x34, 0x4a, 0x34, 0x4a, 0x34, 0x4a,	0xFE,	0x7B , 0x6e, 0x80,	0x80, 0x8a },	//PAL i
   																						//0x6e

   //PAL_N,
   {0x15A604, 	0x103EBD, 	0xAD773, 	0x56BBA, 	0x1, 	0x2B4C0A, 	0x207D79, 	0x15AA76,	0xAD773, 	0x470, 	0x1, 0x08,	0x80, 0x18, 0x32, 0x6e, 0x1, 0x1, 0x1, 0x1, 0x3c0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x2, 0x1, 0, 0xDD, 0x1, 0x3, 0x32, 0x50, 0x2A098ACB, 0x20000000, 0x8A, 0x50, 0x2A, 0xC1, 0x0, 0x80, 0x46, 0x0, 0x0, 0x3, 0x06, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x0, 0x0, 0x21, 0x0, 0x2c, 0xbf, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x7, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x1, 0xDC0, 0x5, 0x25,	0x40, 0x88,	 0x50,	0x18, 0x10, 0x1,	0x7, 0x0, 0x0,	0x7b,	0x50,	0x2f,	0x32, 0x6e,	0x32, 0x46,		0x08, 0x08,	0x93 	,0x34, 0x4a, 0x34, 0x4a, 0x34, 0x4a,	0xFE,	0x7B , 0x6e, 0x80,	0x80, 0x8a },	//PAL N

   //PAL-M,
   {0xE9050, 	0xAEE05, 	0x74BB7, 	0x3A5DC, 	0x1, 	0x1D20A2,	0x15DC09,	0xE93E0, 	0x74BB7, 	0x38E, 	0x1, 0x08,	0x80, 0x18, 0x32, 0x46, 0x2, 0x0, 0x0, 0x1, 0x3c0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x2, 0x1, 0, 0xDD, 0x1, 0x3, 0x32, 0x50, 0x21E6EFA4, 0x20000000, 0x82, 0x50, 0x22, 0x61, 0x0, 0x80, 0x46, 0x0, 0x0, 0x3, 0x06, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x0, 0x0, 0x21, 0x0, 0x24, 0x5F, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x7, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x1, 0xDC0, 0x5, 0x25,	0x40, 0x88,	 0x50,	0x30, 0x10, 0x1,	0x7, 0x0, 0x0,	0x82,	0x50,	0x27,	0x32, 0x46,	0x32, 0x46,		0x08, 0x09,	0x8a 	,0x34, 0x51, 0x34, 0x51, 0x34, 0x51,	0xFE,	0x82 , 0x60, 0x80,	0x80, 0x8a  },	// PAL M

   																	       //gate value//
   //PAL-CN,
   {0x117D90, 	0xD1FF9, 	0x8C25F, 	0x46130, 	0x1, 	0x22FB22, 	0x1A3FF1,	0x118128,	0x8C25F, 	0x396, 	0x1, 0x08,	0x80, 0x18, 0x32, 0x6e, 0x3, 0x1, 0x1, 0x0, 0x3c0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x2, 0x1, 0, 0xDC, 0x1, 0x3, 0x32, 0x50, 0x21f69446, 0x20000000, 0x8A, 0x50, 0x2A, 0xC1, 0x0, 0x80, 0x46, 0x0, 0x0, 0x3, 0x06, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x0, 0x0, 0x21, 0x0, 0x2c, 0xbf, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x7, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x1, 0xDC0, 0x5, 0x25,	0x40, 0x92,	 0x4a,	0x18, 0x10, 0x1,	0x7, 0x0, 0x0,	0x8c,	0x50,	0x2f,	0x32, 0x6e,	0x32, 0x46,		0x08, 0x08,	0x93 	,0x34, 0x4f, 0x34, 0x4f, 0x34, 0x4f,	0xFE,	0x7B , 0x6e, 0x80,	0x80, 0x8a  },	// PAL CN

   //PAL-60,
   {0 , 		0 ,	 		0 , 		0 , 		0 , 	0 , 		0 ,	 		0 , 		0 , 		0 , 	0x1, 0x08,	0x80, 0x18, 0x32, 0x46, 0x1, 0x0, 0x0, 0x0, 0x3f0, 0x1, 0x1, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x2, 0x0, 1, 0xDD, 0x1, 0x3, 0x32, 0x50, 0x2A098ACB, 0x20000000, 0x82, 0x50, 0x22, 0x61, 0x0, 0x80, 0x46, 0x0, 0x0, 0x3, 0x02, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x1, 0x0, 0x21, 0x0, 0x24, 0x5F, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x0, 0xDC0, 0x5, 0x25,	0x40, 0x7D,	 0x50,	0x18, 0x10,	0x1,	0x7, 0x0, 0x0,	0x70,	0x50,	0x27,	0x32, 0x46,	0x32, 0x46,		0x08, 0x09,	0x8a 	,0x34, 0x4a, 0x34, 0x4a, 0x34, 0x4a,	0xFE,	0x7B , 0x60, 0x80,	0x80, 0x8a  },	// PAL 60

   //SECAM,                                                                                                                                                                                                                                                                  //110809
   {0 , 		0 , 		0 , 		0 , 		0 , 	0 , 		0 , 		0 , 		0 , 		0 , 	0x1, 0x20,	0x80, 0x18, 0x32, 0x60, 0x4, 0x1, 0x1, 0x0, 0x3f0, 0x1, 0x0, 0x0, 0x0, 0x2, 0x1, 0x0, 0x0, 0x0, 0x0, 0, 0xDC, 0x1, 0x5, 0x32, 0x50, 0x28A33BB2, 0x20000000, 0x84, 0x50, 0x2A, 0xC1, 0x0, 0x80, 0x6E, 0x1, 0x1, 0x3, 0x02, 0x0, 0x2, 0x2, 0x3, 0x0, 0x3, 0x0, 0x3, 0x3, 0x0, 0x1, 0x1, 0x0, 0x23, 0x0, 0x24, 0x5F, 0x6E, 0x50, 0x0a,  0x2, 0xff, 0x18, 0x5, 0x13, 0x0, 0x0, 0x0, 0x1, 0x0, 0x1, 0x02, 0x020, 0x010, 0x040, 0x2, 0xE, 0x1, 0x2, 0x040, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0xB0, 0x000, 0x0, 0x0, 0xDC0, 0x5, 0x25,	0x40, 0x8D,	 0x50,	0x18, 0x10,	0x0,	0x2, 0x0, 0x0,	0x99,	0x30,	0x29,	0x32, 0x60,	0x32, 0x6E,		0x08, 0x08,	0x93 	,0x32, 0x50, 0x32, 0x60, 0x32, 0x6e,	0xFE,	0x80 , 0x6e, 0x78,	0x80, 0x8a  },	// SECAM
																//0x1a				0x32, 0x6e																																																																																																																																																								/*0x98*/
};																		//gogosing 110610 (for russia ATV field stream color system issue),	0x32 -> 0x3b																																																																																																																	//gogosing 110610 (for russia ATV field stream color system issue), 0x5 -> 0x2


#if 0
static CVD_SET_SYSTEM_CHB_T gSetColorSystemCHB[]=
{
	/*Default*/		{/*0x1,*/ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, /*0x0,*/ 0x0, 0x0, 0xDD, 0x8A, 0x21F07C1F, 0x00, 0x00, 0x82, 0x22, 0x61, 0x1, 0x2, 0x3, 0x0, 0x4, 0x5/*, 0x1*/},

	/*NTSC*/		{/*0x0,*/ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, /*0x0,*/ 0x0, 0x0, 0xDD, 0xA8, 0x21F07C1F, 0x00, 0x00, 0x82, 0x22, 0x61, 0x1, 0x2, 0x3, 0x0, 0x4, 0x5/*, 0x0*/},

	/*NTSCj*/		{/*0x0,*/ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, /*0x0,*/ 0x0, 0x0, 0xCD, 0xA8, 0x21F07C1F, 0x00, 0x00, 0x82, 0x22, 0x61, 0x1, 0x2, 0x3, 0x0, 0x4, 0x5/*, 0x0*/},

	/*NTSC443*/		{/*0x0,*/ 0x0, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x1, /*0x0,*/ 0x0, 0x0, 0xDD, 0xA8, 0x2A098ACB, 0x00, 0x00, 0x82, 0x22, 0x61, 0x1, 0x2, 0x3, 0x0, 0x4, 0x5/*, 0x0*/},

	/*PALi*/		{/*0x0,*/ 0x0, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, /*0x0,*/ 0x0, 0x0, 0xDC, 0x67, 0x2A098ACB, 0x00, 0x00, 0x84, 0x2A, 0xC1, 0x1, 0x2, 0x3, 0x0, 0x0, 0x5/*, 0x0*/},

	/*PALn*/		{/*0x0,*/ 0x0, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x1, /*0x0,*/ 0x0, 0x0, 0xDD, 0x67, 0x21F69446, 0x00, 0x00, 0x8A, 0x2A, 0xC1, 0x1, 0x2, 0x3, 0x0, 0x0, 0x5/*, 0x0*/},

	/*PALm*/		{/*0x0,*/ 0x0, 0x1, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, /*0x0,*/ 0x0, 0x0, 0xDD, 0x67, 0x21E6EFA4, 0x00, 0x00, 0x82, 0x22, 0x61, 0x1, 0x2, 0x3, 0x0, 0x0, 0x5/*, 0x0*/},

	/*PALCn*/		{/*0x0,*/ 0x0, 0x1, 0x1, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, /*0x0,*/ 0x0, 0x0, 0xDC, 0x67, 0x21F69446, 0x00, 0x00, 0x8A, 0x2A, 0xC1, 0x1, 0x2, 0x3, 0x0, 0x0, 0x5/*, 0x0*/},

	/*PAL60*/		{/*0x0,*/ 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, /*0x0,*/ 0x0, 0x0, 0xDD, 0x8A, 0x21F07C1F, 0x00, 0x00, 0x82, 0x22, 0x61, 0x1, 0x2, 0x3, 0x0, 0x4, 0x5/*, 0x0*/},

	/*SECAM*/		{/*0x0,*/ 0x0, 0x1, 0x1, 0x4, 0x0, 0x0, 0x0, 0x0, 0x0, /*0x0,*/ 0x0, 0x0, 0xDC, 0x67, 0x28A33BB2, 0x41, 0xEC, 0x84, 0x2A, 0xC1, 0x1, 0x2, 0x2, 0x0, 0x0, 0x5/*, 0x0*/},
};
#endif

/**********************************************************************/
/************** L9Ax CVD PE Settings **********************************/
/**********************************************************************/

static CVD_L9Ax_SET_AAF_T gSetAAF_L9Ax[] =
{
	/*Default*/
	{0x0,	0x003F,	0x0024,	0xFCED,	0xFB2B,	0x08B1,	0x24B8,	0x3438	},

	/*NTSC*/
	{0x1,	0x0029,	0xFF75,	0xFCAA,	0xFD6F,	0x0B7B,	0x231E,	0x2F60	},
	/*NTSC_RF*/
	//110919 : band limit to 6.5MHz for all RF Inputs
	{0x1,	0xFFC6,	0xFF92,	0x0131,	0x06C7,	0x1037,	0x19A6,	0x1DA6	},	//F6p0_K5
	//{0x1,	0x0029,	0xFF75,	0xFCAA,	0xFD6F,	0x0B7B,	0x231E,	0x2F60	},
	/*PAL*/
	{0x1,	0x0029,	0xFF75,	0xFCAA,	0xFD6F,	0x0B7B,	0x231E,	0x2F60	},
	/*PAL_RF*/
	//110919 : band limit to 6.5MHz for all RF Inputs
	{0x1,	0xFFC6,	0xFF92,	0x0131,	0x06C7,	0x1037,	0x19A6,	0x1DA6	},	//F6p0_K5
	//{0x1,	0x0029,	0xFF75,	0xFCAA,	0xFD6F,	0x0B7B,	0x231E,	0x2F60	},	//(a)
	/*SECAL*/
	{0x1,	0x0029,	0xFF75,	0xFCAA,	0xFD6F,	0x0B7B,	0x231E,	0x2F60	},
	/*SECAM_RF*/
	//110919 : band limit to 6.5MHz for all RF Inputs
	{0x1,	0xFFC6,	0xFF92,	0x0131,	0x06C7,	0x1037,	0x19A6,	0x1DA6	},	//F6p0_K5
	//{0x1,	0x0029,	0xFF75,	0xFCAA,	0xFD6F,	0x0B7B,	0x231E,	0x2F60	},
	/*(a)*/
	{0x1,	0x0029,	0xFF75,	0xFCAA,	0xFD6F,	0x0B7B,	0x231E,	0x2F60	},	//(a)
	/*F6p0_K5*/
	{0x1,	0xFFC6,	0xFF92,	0x0131,	0x06C7,	0x1037,	0x19A6,	0x1DA6	},	//F6p0_K5
	/*F5p5_K5*/
	{0x1,	0xFE6A,	0xFFC2,	0x03A7,	0x09E9,	0x110C,	0x16C0,	0x18F0	},	//F5p5_K5
	/*F4p5_K5*/
	{0x1,	0x0000,	0x021D,	0x05D8,	0x0AB5,	0x0FA5,	0x1356,	0x14B6	},	//F4p5_K5
};

static CVD_L9Ax_SET_CRES54MHZ_T gSetCresampler_L9Ax[] =
{
	/*Default*/																																																								//added 20110126
	{0x0,	0x0,	0x600,	0x0,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x2, 	0x0,	0x0},
	/*NTSC*/
	{0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
	/*NTSC_RF*/
	{0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
	/*PAL*/
	{0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
	/*PAL_RF*/
	{0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
	/*SECAM*/
	{0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
	/*SECAM_RF*/
	{0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
};

static CVD_L9Ax_SET_DOT_CRAWL_T gSetDotCrawl_L9Ax[] =
{
	/*Default*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x1,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x080,	0x0,	0x0,	0x0,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},

	/*NTSC*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x0,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x40,	0x0,	0x0,	0x7,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
	/*NTSC_RF*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x0,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x40,	0x0,	0x0,	0x7,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
	/*PAL*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x0,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x40,	0x0,	0x0,	0x7,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
	/*PAL_RF*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x0,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x40,	0x0,	0x0,	0x7,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
	/*SECAM*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x1,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x080,	0x0,	0x0,	0x0,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
	/*SECAM_RF*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x1,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x080,	0x0,	0x0,	0x0,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
};

static CVD_L9Ax_SET_CROSS_COLOR_REDUCER_T gSetCrossColorReducer_L9Ax[] =
{
	/*Default*/
	{0x0,	0x0,	0x2,	0x0,	0x9,	0x0,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x00,	0x2,	0x0,	0x1,	0x0,	0x1,	0x85,	0x0,	0x084},
	/*NTSC*/																																												//modified 110126
	{0x8,	0x6,	0x2,	0x0,	0x9,	0x1,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x7,	0x2,	0x1,	0x1,	0x0,	0x1,	0x88,	0x0,	0x084},
	/*NTSC_RF*/																																												//modified 110126
	{0x8,	0x6,	0x2,	0x0,	0x9,	0x1,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x7,	0x2,	0x1,	0x1,	0x0,	0x1,	0x88,	0x0,	0x084},
	/*PAL*/																																												//modified 110126
	{0x8,	0x6,	0x2,	0x0,	0x9,	0x1,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x7,	0x2,	0x1,	0x1,	0x0,	0x1,	0x88,	0x0,	0x084},
	/*PAL_RF*/																																												//modified 110126
	{0x8,	0x6,	0x2,	0x0,	0x9,	0x1,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x7,	0x2,	0x1,	0x1,	0x0,	0x1,	0x88,	0x0,	0x084},
	/*SECAM*/
	{0x0,	0x0,	0x2,	0x0,	0x9,	0x0,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x00,	0x2,	0x0,	0x1,	0x0,	0x1,	0x85,	0x0,	0x084},
	/*SECAM_RF*/
	{0x0,	0x0,	0x2,	0x0,	0x9,	0x0,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x00,	0x2,	0x0,	0x1,	0x0,	0x1,	0x85,	0x0,	0x084},
};

static CVD_L9Ax_SET_MOTION_DETECTOR_T gSetMotionDetector_L9Ax[] =
{
	/*Default*/
	{0x0,	0x1,	0x0A,	0x2,	0x0,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x20,	0x05,	0xF,	0x0,	0x00},
	/*NTSC*/
	{0x0,	0x1,	0x0A,	0x2,	0x1,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x18,	0x13,	0xF,	0x0,	0x22},
	/*NTSC_RF*/
	{0x0,	0x1,	0x0A,	0x2,	0x1,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x18,	0x13,	0xF,	0x0,	0x22},
	/*PAL*/
	{0x0,	0x1,	0x08,	0x2,	0x0,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x18,	0x10,	0xF,	0x0,	0x00},
	/*PAL_RF*/
	{0x0,	0x1,	0x08,	0x2,	0x0,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x18,	0x10,	0xF,	0x0,	0x00},
	/*SECAM*/
	{0x0,	0x1,	0x0A,	0x2,	0x0,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x20,	0x05,	0xF,	0x0,	0x00},
	/*SECAM_RF*/
	{0x0,	0x1,	0x0A,	0x2,	0x0,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x20,	0x05,	0xF,	0x0,	0x00},
};

static CVD_L9Ax_SET_GLOBAL_CLASSIFIER_T gSetGlobalClassifier_L9Ax[] =
{
	/*Default*/
	{0x0,	0x00,	0x0,	0x0,	0x03,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0000,	0x40,	0x40,	0xC0C0C0,	0xC0C0C0C0,	0xC0C0C0,	0xC0C0C0C0,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002,	0x0,	0x09,	0x0A,	0x08},
	/*NTSC*/
	{0x1,	0x08,	0x0,	0x0,	0x05,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x003,	0x003,	0xf00,	0xF00,	0x0,	0x003,	0x003,	0xf00,	0xF00,	0x0000,	0x40,	0x40,	0x727476,	0x787A7C7E,	0x727476,	0x787A7C7E,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002,	0x1,	0x09,	0x0A,	0x1},
	/*NTSC_RF*/
	{0x1,	0x08,	0x0,	0x0,	0x05,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x003,	0x003,	0xf00,	0xF00,	0x0,	0x003,	0x003,	0xf00,	0xF00,	0x0000,	0x40,	0x40,	0x727476,	0x787A7C7E,	0x727476,	0x787A7C7E,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002,	0x1,	0x09,	0x0A,	0x1},
	/*PAL*/
	{0x1,	0x08,	0x0,	0x0,	0x05,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x010,	0x180,	0xf00,	0xF00,	0x0,	0x010,	0x180,	0xf00,	0xF00,	0x0000,	0x40,	0x40,	0x727476,	0x787A7C7E,	0x727476,	0x787A7C7E,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x0,	0x0,	0x000000,	0x1,	0x09,	0x0A,	0x1},
	/*PAL_RF*/
	{0x1,	0x08,	0x0,	0x0,	0x05,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x010,	0x180,	0xf00,	0xF00,	0x0,	0x010,	0x180,	0xf00,	0xF00,	0x0000,	0x40,	0x40,	0x727476,	0x787A7C7E,	0x727476,	0x787A7C7E,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x0,	0x0,	0x000000,	0x1,	0x09,	0x0A,	0x1},
	/*SECAM*/
	{0x0,	0x00,	0x0,	0x0,	0x03,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0000,	0x40,	0x40,	0xC0C0C0,	0xC0C0C0C0,	0xC0C0C0,	0xC0C0C0C0,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002,	0x0,	0x09,	0x0A,	0x08},
	/*SECAM_RF*/
	{0x0,	0x00,	0x0,	0x0,	0x03,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0000,	0x40,	0x40,	0xC0C0C0,	0xC0C0C0C0,	0xC0C0C0,	0xC0C0C0C0,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002,	0x0,	0x09,	0x0A,	0x08},
};

static CVD_L9Ax_SET_BLENDING_T gSetBlending_L9Ax[] =
{
	/*Default*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*NTSC*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*NTSC_RF*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*PAL*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*PAL_RF*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*SECAM*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*SECAM_RF*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
};

static CVD_L9Ax_SET_CHROMA_DEMODE_T gSetChromaDemode_L9Ax[] =
{
	/*Default*/
	{	0x1,	0x0	},
	/*NTSC*/
	{	0x1,	0x0	},
	/*NTSC_RF*/
	{	0x0,	0x0	},
	/*PAL*/
	{	0x1,	0x0	},
	/*PAL_RF*/
	{	0x0,	0x0	},
	/*SECAM*/
	{	0x0,	0x2	},
	/*SECAM_RF*/
	{	0x0,	0x2	},
};


static CVD_L9Ax_SET_OUPUT_DELAY_T gSetOutputDelay_L9Ax[] =
{
//							yc_de..	cb_delay
	/*Default*/
	{0x0,	0x0,	0x0,	0x0,	0x0},
	/*NTSC*/
//	{0x0,	0x80,	0x0,	0x1,	0x1},
	{0x0,	0x0,	0x0,	0x1,	0x0},
	/*NTSC_RF*/
	{0x0,	0x0,	0x0,	0x1,	0x0},
	/*PAL*/
	{0x0,	0x0,	0x0,	0x3,	0x0},
	/*PAL_RF*/
	{0x0,	0x0,	0x0,	0x3,	0x0},
	/*SECAM*/
	{0x0,	0x0,	0x0,	0x3,	0x0},
	/*SECAM_RF*/
	{0x0,	0x0,	0x0,	0x3,	0x0},
};

static CVD_L9Ax_SET_OUPUT_RANGE_T gSetOutputRange_L9Ax[] =
{
	/*Default*/
	{0x000,	0x040,	0x0DC0, 0x000, 0x040, 0x0E10},
	/*NTSC*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
	/*NTSC_RF*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
	/*PAL*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
	/*PAL_RF*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
	/*SECAM*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
	/*SECAM_RF*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
};

static CVD_L9Ax_SET_LUMA_PEAKING_T gSetLumaPeaking_L9Ax[] =
{
	/*Default*/
	{0x0,	0x2,	0x0,	0x0},
	/*NTSC*/
	{0x0,	0x2,	0x0,	0x0},
	/*NTSC_RF*/
	{0x0,	0x2,	0x0,	0x0},
	/*PAL*/
	{0x0,	0x2,	0x0,	0x0},
	/*PAL_RF*/
	{0x0,	0x2,	0x0,	0x0},
	/*SECAM*/
	{0x0,	0x2,	0x0,	0x1},
	/*SECAM_RF*/
	{0x0,	0x2,	0x0,	0x1},
};

static CVD_L9Ax_SET_SECAM_FREQ_T	gSetSecamFreq_L9Ax[] =
{
	/*Default*/
	{0x016,	0xfc1},
	/*NTSC*/
	{0x016,	0xfc1},
	/*NTSC_RF*/
	{0x016,	0xfc1},
	/*PAL*/
	{0x016,	0xfc1},
	/*PAL_RF*/
	{0x016,	0xfc1},
	/*SECAM*/
	{0x016,	0x0},
	/*SECAM_RF*/
	{0x016,	0x0},
};

/**********************************************************************/
/************** L8B0 CVD PE Settings **********************************/
/**********************************************************************/

static CVD_L8Bx_SET_AAF_T gSetAAF_L8Bx[] =
{
	/*Default*/
	{0x0,	0x003F,	0x0024,	0xFCED,	0xFB2B,	0x08B1,	0x24B8,	0x3438,	0x24B8,	0x08B1,	0xFB2B,	0xFCED,	0x0024,	0x003F},

	/*NTSC*/
	{0x1,	0x0516,	0x0102,	0xF33E,	0xF1D4,	0x1470,	0x4c48,	0x6838,	0x4c48,	0x1470,	0xF1D4,	0xF33E,	0x102,	0x516},
	/*NTSC_RF*/
	{0x1,	0x0516,	0x0102,	0xF33E,	0xF1D4,	0x1470,	0x4c48,	0x6838,	0x4c48,	0x1470,	0xF1D4,	0xF33E,	0x102,	0x516},
};

static CVD_L8Bx_SET_CRES54MHZ_T gSetCresampler_L8Bx[] =
{
	/*Default*/																																																								//added 20110126
	{0x0,	0x06,	0x3,	0x3,	0x5,	0x0,	0x2,	0x0,	0x0,	0x0,	0x0,	0x0,	0x600,	0x0,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x2, 	0x0,	0x0},

	/*NTSC*/
	{0x1,	0x06,	0x3,	0x3,	0x5,	0x0,	0x2,	0x0,	0x1,	0x0,	0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
	/*NTSC_RF*/
	{0x1,	0x06,	0x3,	0x3,	0x5,	0x0,	0x2,	0x0,	0x1,	0x0,	0x1,	0x0,	0x000,	0x1,	0x1,	0x038A,	0x0103,	0x1F8F,	0x1FD0,	0x0010,	0x0003,	0x1,	0x0380,	0x00EE,	0x1FA9,	0x1FE6,	0x0003,	0x0,	0x3, 	0x7,	0x0},
};

static CVD_L8Bx_SET_DOT_CRAWL_T gSetDotCrawl_L8Bx[] =
{
	/*Default*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x1,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x080,	0x0,	0x0,	0x0,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},

	/*NTSC*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x0,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x40,	0x0,	0x0,	0x7,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
	/*NTSC_RF*/
	{0x0,	0x0,	0x0,	0x1,	0x0,	0x0,	0x020,	0x010,	0x040,	0x2,	0xE,	0x1,	0x2,	0x40,	0x0,	0x0,	0x7,	0x0,	0x020,	0x020,	0x021,	0x021,	0x022,	0x022,	0x023},
};

static CVD_L8Bx_SET_CROSS_COLOR_REDUCER_T gSetCrossColorReducer_L8Bx[] =
{
	/*Default*/
	{0x0,	0x0,	0x2,	0x0,	0x9,	0x0,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x00,	0x2,	0x0,	0x1,	0x0,	0x1,	0x85,	0x0,	0x084},
	/*NTSC*/																																												//modified 110126
	{0x8,	0x6,	0x2,	0x0,	0x9,	0x1,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x7,	0x2,	0x1,	0x1,	0x0,	0x1,	0x88,	0x0,	0x084},
	/*NTSC_RF*/																																												//modified 110126
	{0x8,	0x6,	0x2,	0x0,	0x9,	0x1,	0x0C0,	0x040,	0x74,	0x2,	0x0C4,	0x0C8,	0x0CC,	0x0D0,	0x0D8,	0x0E0,	0x0F0,	0x010,	0x7,	0x2,	0x1,	0x1,	0x0,	0x1,	0x88,	0x0,	0x084},
};

static CVD_L8Bx_SET_MOTION_DETECTOR_T gSetMotionDetector_L8Bx[] =
{
	/*Default*/
	{0x0,	0x1,	0x1,	0x0A,	0x2,	0x0,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x20,	0x05,	0x00,	0x0,	0x09,	0x0A,	0x08},
	/*NTSC*/
	{0x0,	0x1,	0x1,	0x0A,	0x2,	0x1,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x18,	0x13,	0x08,	0x1,	0x09,	0x0A,	0x1},
	/*NTSC_RF*/
	{0x0,	0x1,	0x1,	0x0A,	0x2,	0x1,	0x0,	0x00,	0x00,	0x00FF,	0xFFFF,	0x18,	0x13,	0x08,	0x1,	0x09,	0x0A,	0x1},
};

static CVD_L8Bx_SET_GLOBAL_CLASSIFIER_T gSetGlobalClassifier_L8Bx[] =
{
	/*Default*/
	{0x0,	0xF,	0x0,	0x0,	0x0,	0x03,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0000,	0x40,	0x40,	0xC0C0C0,	0xC0C0C0C0,	0xC0C0C0,	0xC0C0C0C0,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002},
	/*NTSC*/
	{0x1,	0xF,	0x0,	0x0,	0x0,	0x05,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x003,	0x003,	0xf00,	0xF00,	0x0,	0x003,	0xF00,	0x300,	0xF00,	0x0000,	0x40,	0x40,	0x727476,	0x787A7C7E,	0x727476,	0x787A7C7E,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002},
	/*NTSC_RF*/
	{0x0,	0xF,	0x0,	0x0,	0x0,	0x03,	0x03,	0x20,	0x20,	0x4,	0x2,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0,	0x005,	0x100,	0x300,	0xF00,	0x0000,	0x40,	0x40,	0xC0C0C0,	0xC0C0C0C0,	0xC0C0C0,	0xC0C0C0C0,	/*	0x0,	0x0004,	0x01030202,	0x03010400,*/	0x1,	0x1,	0x000002},
};

static CVD_L8Bx_SET_BLENDING_T gSetBlending_L8Bx[] =
{
	/*Default*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*NTSC*/
	//{	0x4,	0x00000040,	0x10302020,	0x340c4000	},
	//New value 110222 by kim.min
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
	/*NTSC_RF*/
	{	0x0,	0x00000004,	0x01030202,	0x03010400	},
};

static CVD_L8Bx_SET_OUPUT_DELAY_T gSetOutputDelay_L8Bx[] =
{
//							yc_de..	cb_delay
	/*Default*/
	{0x0,	0x0,	0x0,	0x0,	0x0},
	/*NTSC*/
//	{0x0,	0x80,	0x0,	0x1,	0x1},
	{0x0,	0x0,	0x0,	0x0,	0x0},
	/*NTSC_RF*/
	{0x0,	0x0,	0x0,	0x0,	0x0},
};

static CVD_L8Bx_SET_OUPUT_RANGE_T gSetOutputRange_L8Bx[] =
{
	/*Default*/
	{0x000,	0x040,	0x0DC0, 0x000, 0x040, 0x0E10},
	/*NTSC*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
	/*NTSC_RF*/
	{0x000,	0x000,	0x1000, 0x000, 0x000, 0x1000},
};
/*========================================================================================
	Implementation Group
========================================================================================*/


int CVD_InitChipRev(void)
{

	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))	// won.hur @2011.08.24
	{
		CVD_Program_Color_System_Main_Multi	=	CVD_L9Bx_Program_Color_System_Main_Multi;
		CVD_Set_Picture_Enhancement	=    	CVD_L9Bx_Set_Picture_Enhancement;
		CVD_Program_Color_System_CHB_NTSC	=  	CVD_L9Bx_Program_Color_System_CHB_NTSC;
		CVD_SW_Reset		=                   	CVD_L9Bx_SW_Reset;
		CVD_Power_Down		=                 	CVD_L9Bx_Power_Down;
		CVD_Get_FC_Flag		=                	CVD_L9Bx_Get_FC_Flag;
		CVD_Get_CVD_Burst_Mag_Flag		=	CVD_L9Bx_Get_CVD_Burst_Mag_Flag;
		CVD_Set_CVD_CAGC			=	CVD_L9Bx_Set_CVD_CAGC;
		CVD_Get_Cordic_Freq		=                	CVD_L9Bx_Get_Cordic_Freq;
//		CVD_Program_Color_System_Main		=      	CVD_L9Bx_Program_Color_System_Main;
//		CVD_Program_Color_System_CHB		=       	CVD_L9Bx_Program_Color_System_CHB;
		CVD_Set_Source_Type		=            	CVD_L9Bx_Set_Source_Type;
		CVD_Clamp_Current_Control		=      	CVD_L9Bx_Clamp_Current_Control;
		CVD_Force_Color_Kill		=           	CVD_L9Bx_Force_Color_Kill;

		CVD_Set_Scart_Overlay		=          	CVD_L9Bx_Set_Scart_Overlay;
		CVD_Reg_Init		=                  	CVD_L9Bx_Reg_Init;
		CVD_Get_Scart_AR		=              	CVD_L9Bx_Get_Scart_AR;
		CVD_Print_Vport_Version		=        	CVD_L9Bx_Print_Vport_Version;
		CVD_Vport_Output		=              	CVD_L9Bx_Vport_Output;
		CVD_Vport_Reg_Read		=             	CVD_L9Bx_Vport_Reg_Read;
		CVD_Vport_Reg_Write		=            	CVD_L9Bx_Vport_Reg_Write;
		CVD_Get_FB_Status		=              	CVD_L9Bx_Get_FB_Status;
		CVD_Get_No_Signal_Flag	=				CVD_L9Bx_Get_No_Signal_Flag;
		CVD_Get_HLock_Flag		=				CVD_L9Bx_Get_HLock_Flag;
		CVD_Get_VLock_Flag		=				CVD_L9Bx_Get_VLock_Flag;
		CVD_Get_Vline_625_Flag	=				CVD_L9Bx_Get_Vline_625_Flag;
		CVD_Get_PAL_Flag		=				CVD_L9Bx_Get_PAL_Flag;
		CVD_Get_SECAM_Flag		=				CVD_L9Bx_Get_SECAM_Flag;
		CVD_Get_Chromalock_Flag	=				CVD_L9Bx_Get_Chromalock_Flag;
		CVD_Get_Noise_Status	=				CVD_L9Bx_Get_Noise_Status;
		CVD_Get_NoBurst_Flag	=				CVD_L9Bx_Get_NoBurst_Flag;
		CVD_Get_CHB_No_Signal_Flag	=			CVD_L9Bx_Get_CHB_No_Signal_Flag;
		CVD_Get_CHB_HLock_Flag		=			CVD_L9Bx_Get_CHB_HLock_Flag;
		CVD_Get_CHB_VLock_Flag		=			CVD_L9Bx_Get_CHB_VLock_Flag;
		CVD_Get_CHB_Chromalock_Flag	=			CVD_L9Bx_Get_CHB_Chromalock_Flag;
		CVD_Get_CHB_PAL_Flag 		=			CVD_L9Bx_Get_CHB_PAL_Flag;
		CVD_Get_CHB_Vline_625_Flag		=			CVD_L9Bx_Get_CHB_Vline_625_Flag;
		CVD_Get_CHB_SECAM_Flag			=			CVD_L9Bx_Get_CHB_SECAM_Flag;
		CVD_Reset_irisyc			=			CVD_L9Bx_Reset_irisyc;
		CVD_Reset_hdct				=			CVD_L9Bx_Reset_hdct;
		CVD_Reset_cdct				=			CVD_L9Bx_Reset_cdct;
		/*
		CVD_Set_AAF_Params			=			CVD_L9Bx_Set_AAF_Params;
		CVD_Set_ADC_Params			=			CVD_L9Bx_Set_ADC_Params;
    	CVD_Set_Cresampler_Params	=			CVD_L9Bx_Set_Cresampler_Params;
    	CVD_Set_Dot_Crawl_Params	=			CVD_L9Bx_Set_Dot_Crawl_Params;
		CVD_Set_Cross_Color_Reducer	=			CVD_L9Bx_Set_Cross_Color_Reducer;
		CVD_Set_Motion_Detector		=			CVD_L9Bx_Set_Motion_Detector;
		CVD_Set_Global_Classifier	=			CVD_L9Bx_Set_Global_Classifier;
		CVD_Set_Output_Delay		=			CVD_L9Bx_Set_Output_Delay;
		CVD_Set_Output_Range		=			CVD_L9Bx_Set_Output_Range;
		CVD_Set_Blending			=			CVD_L9Bx_Set_Blending;
		*/
		CVD_Read_fld_cnt_value		=			CVD_L9Bx_Read_fld_cnt_value;
		CVD_Set_motion_mode			=			CVD_L9Bx_Set_motion_mode;
		CVD_Channel_Power_Control	=			CVD_L9Bx_Channel_Power_Control;
		CVD_Reset_Clampagc	=			CVD_L9Bx_Reset_Clampagc;
		CVD_Get_PAL_Flag_CS0		=			CVD_L9Bx_Get_PAL_Flag_CS0;
		CVD_Get_SECAM_Flag_CS0		=			CVD_L9Bx_Get_SECAM_Flag_CS0;
		CVD_Get_Chromalock_Flag_CS0		=			CVD_L9Bx_Get_Chromalock_Flag_CS0;
		CVD_Get_FC_Flag_CS0			=			CVD_L9Bx_Get_FC_Flag_CS0;
		CVD_Get_Cordic_Freq_CS0		=			CVD_L9Bx_Get_Cordic_Freq_CS0;
		CVD_Get_PAL_Flag_CS1		=			CVD_L9Bx_Get_PAL_Flag_CS1;
		CVD_Get_SECAM_Flag_CS1		=			CVD_L9Bx_Get_SECAM_Flag_CS1;
		CVD_Get_Chromalock_Flag_CS1		=			CVD_L9Bx_Get_Chromalock_Flag_CS1;
		CVD_Get_FC_Flag_CS1			=			CVD_L9Bx_Get_FC_Flag_CS1;
		CVD_Get_Cordic_Freq_CS1		=			CVD_L9Bx_Get_Cordic_Freq_CS1;
		CVD_Bypass_Control			=			CVD_L9Bx_Bypass_Control;
		CVD_Get_Vline_625_Reg	=				CVD_L9Bx_Get_Vline_625_Reg;
		CVD_Program_Color_System_PreJob	=	CVD_L9Bx_Program_Color_System_PreJob;
		CVD_Set_Hstate_Max			=			CVD_L9Bx_Set_Hstate_Max;
		CVD_OnOff_Chromalock_Ckill	=			CVD_L9Bx_OnOff_Chromalock_Ckill;
	}
	else if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{
		CVD_Program_Color_System_Main_Multi	=	CVD_L9Ax_Program_Color_System_Main_Multi;
		CVD_Set_Picture_Enhancement	=    	CVD_L9Ax_Set_Picture_Enhancement;
		CVD_Program_Color_System_CHB_NTSC	=  	CVD_L9Ax_Program_Color_System_CHB_NTSC;
		CVD_SW_Reset		=                   	CVD_L9Ax_SW_Reset;
		CVD_Power_Down		=                 	CVD_L9Ax_Power_Down;
		CVD_Get_FC_Flag		=                	CVD_L9Ax_Get_FC_Flag;
#if 1 //gogosing burst mag status check for color burst level test (color 틀어짐 대응) 2011.06.11
		CVD_Get_CVD_Burst_Mag_Flag		=	CVD_L9Ax_Get_CVD_Burst_Mag_Flag;
		CVD_Set_CVD_CAGC			=	CVD_L9Ax_Set_CVD_CAGC;
#endif
		CVD_Get_Cordic_Freq		=                	CVD_L9Ax_Get_Cordic_Freq;
//		CVD_Program_Color_System_Main		=      	CVD_L9Ax_Program_Color_System_Main;
//		CVD_Program_Color_System_CHB		=       	CVD_L9Ax_Program_Color_System_CHB;
		CVD_Set_Source_Type		=            	CVD_L9Ax_Set_Source_Type;
		CVD_Clamp_Current_Control		=      	CVD_L9Ax_Clamp_Current_Control;
		CVD_Force_Color_Kill		=           	CVD_L9Ax_Force_Color_Kill;

		CVD_Set_Scart_Overlay		=          	CVD_L9Ax_Set_Scart_Overlay;
		CVD_Reg_Init		=                  	CVD_L9Ax_Reg_Init;
		CVD_Get_Scart_AR		=              	CVD_L9Ax_Get_Scart_AR;
		CVD_Print_Vport_Version		=        	CVD_L9Ax_Print_Vport_Version;
		CVD_Vport_Output		=              	CVD_L9Ax_Vport_Output;
		CVD_Vport_Reg_Read		=             	CVD_L9Ax_Vport_Reg_Read;
		CVD_Vport_Reg_Write		=            	CVD_L9Ax_Vport_Reg_Write;
		CVD_Get_FB_Status		=              	CVD_L9Ax_Get_FB_Status;
		CVD_Get_No_Signal_Flag	=				CVD_L9Ax_Get_No_Signal_Flag;
		CVD_Get_HLock_Flag		=				CVD_L9Ax_Get_HLock_Flag;
		CVD_Get_VLock_Flag		=				CVD_L9Ax_Get_VLock_Flag;
		CVD_Get_Vline_625_Flag	=				CVD_L9Ax_Get_Vline_625_Flag;
		CVD_Get_PAL_Flag		=				CVD_L9Ax_Get_PAL_Flag;
		CVD_Get_SECAM_Flag		=				CVD_L9Ax_Get_SECAM_Flag;
		CVD_Get_Chromalock_Flag	=				CVD_L9Ax_Get_Chromalock_Flag;
		CVD_Get_Noise_Status	=				CVD_L9Ax_Get_Noise_Status;
		CVD_Get_NoBurst_Flag	=				CVD_L9Ax_Get_NoBurst_Flag;
		CVD_Get_CHB_No_Signal_Flag	=			CVD_L9Ax_Get_CHB_No_Signal_Flag;
		CVD_Get_CHB_HLock_Flag		=			CVD_L9Ax_Get_CHB_HLock_Flag;
		CVD_Get_CHB_VLock_Flag		=			CVD_L9Ax_Get_CHB_VLock_Flag;
		CVD_Get_CHB_Chromalock_Flag	=			CVD_L9Ax_Get_CHB_Chromalock_Flag;
		CVD_Get_CHB_PAL_Flag 		=			CVD_L9Ax_Get_CHB_PAL_Flag;
		CVD_Get_CHB_Vline_625_Flag		=			CVD_L9Ax_Get_CHB_Vline_625_Flag;
		CVD_Get_CHB_SECAM_Flag			=			CVD_L9Ax_Get_CHB_SECAM_Flag;
		CVD_Reset_irisyc			=			CVD_L9Ax_Reset_irisyc;
		CVD_Reset_hdct				=			CVD_L9Ax_Reset_hdct;
		CVD_Reset_cdct				=			CVD_L9Ax_Reset_cdct;
		/*
		CVD_Set_AAF_Params			=			CVD_L9Ax_Set_AAF_Params;
		CVD_Set_ADC_Params			=			CVD_L9Ax_Set_ADC_Params;
    	CVD_Set_Cresampler_Params	=			CVD_L9Ax_Set_Cresampler_Params;
    	CVD_Set_Dot_Crawl_Params	=			CVD_L9Ax_Set_Dot_Crawl_Params;
		CVD_Set_Cross_Color_Reducer	=			CVD_L9Ax_Set_Cross_Color_Reducer;
		CVD_Set_Motion_Detector		=			CVD_L9Ax_Set_Motion_Detector;
		CVD_Set_Global_Classifier	=			CVD_L9Ax_Set_Global_Classifier;
		CVD_Set_Output_Delay		=			CVD_L9Ax_Set_Output_Delay;
		CVD_Set_Output_Range		=			CVD_L9Ax_Set_Output_Range;
		CVD_Set_Blending			=			CVD_L9Ax_Set_Blending;
		*/
		CVD_Read_fld_cnt_value		=			CVD_L9Ax_Read_fld_cnt_value;
		CVD_Set_motion_mode			=			CVD_L9Ax_Set_motion_mode;
		CVD_Channel_Power_Control	=			CVD_L9Ax_Channel_Power_Control;
		CVD_Reset_Clampagc	=			CVD_L9Ax_Reset_Clampagc;
		CVD_Get_PAL_Flag_CS0		=			CVD_L9Ax_Get_PAL_Flag_CS0;
		CVD_Get_SECAM_Flag_CS0		=			CVD_L9Ax_Get_SECAM_Flag_CS0;
		CVD_Get_Chromalock_Flag_CS0		=			CVD_L9Ax_Get_Chromalock_Flag_CS0;
		CVD_Get_FC_Flag_CS0			=			CVD_L9Ax_Get_FC_Flag_CS0;
		CVD_Get_Cordic_Freq_CS0		=			CVD_L9Ax_Get_Cordic_Freq_CS0;
		CVD_Get_PAL_Flag_CS1		=			CVD_L9Ax_Get_PAL_Flag_CS1;
		CVD_Get_SECAM_Flag_CS1		=			CVD_L9Ax_Get_SECAM_Flag_CS1;
		CVD_Get_Chromalock_Flag_CS1		=			CVD_L9Ax_Get_Chromalock_Flag_CS1;
		CVD_Get_FC_Flag_CS1			=			CVD_L9Ax_Get_FC_Flag_CS1;
		CVD_Get_Cordic_Freq_CS1		=			CVD_L9Ax_Get_Cordic_Freq_CS1;
		CVD_Bypass_Control			=			CVD_L9Ax_Bypass_Control;
		CVD_Get_Vline_625_Reg	=				CVD_L9Ax_Get_Vline_625_Reg;
		CVD_Program_Color_System_PreJob	=		CVD_L9Ax_Program_Color_System_PreJob;
		CVD_Set_Hstate_Max			=			CVD_L9Ax_Set_Hstate_Max;
		CVD_OnOff_Chromalock_Ckill	=			CVD_L9Ax_OnOff_Chromalock_Ckill;
	}
#ifdef L8_KDRIVER_SUPPORT
	else if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
	{
		CVD_Program_Color_System_Main_Multi	=	CVD_L8Bx_Program_Color_System_Main_Multi;
		CVD_Set_Picture_Enhancement	=    	CVD_L8Bx_Set_Picture_Enhancement;
		CVD_Program_Color_System_CHB_NTSC	=  	CVD_L8Bx_Program_Color_System_CHB_NTSC;
		CVD_SW_Reset		=                   	CVD_L8Bx_SW_Reset;
		CVD_Power_Down		=                 	CVD_L8Bx_Power_Down;
		CVD_Get_FC_Flag		=                	CVD_L8Bx_Get_FC_Flag;
		CVD_Get_Cordic_Freq		=                	CVD_L8Bx_Get_Cordic_Freq;
//		CVD_Program_Color_System_Main		=      	CVD_L8Bx_Program_Color_System_Main;
//		CVD_Program_Color_System_CHB		=       	CVD_L8Bx_Program_Color_System_CHB;
		CVD_Set_Source_Type		=            	CVD_L8Bx_Set_Source_Type;
		CVD_Clamp_Current_Control		=      	CVD_L8Bx_Clamp_Current_Control;
		CVD_Force_Color_Kill		=           	CVD_L8Bx_Force_Color_Kill;

		CVD_Set_Scart_Overlay		=          	CVD_L8Bx_Set_Scart_Overlay;
		CVD_Reg_Init		=                  	CVD_L8Bx_Reg_Init;
		CVD_Get_Scart_AR		=              	CVD_L8Bx_Get_Scart_AR;
		CVD_Print_Vport_Version		=        	CVD_L8Bx_Print_Vport_Version;
		CVD_Vport_Output		=              	CVD_L8Bx_Vport_Output;
		CVD_Vport_Reg_Read		=             	CVD_L8Bx_Vport_Reg_Read;
		CVD_Vport_Reg_Write		=            	CVD_L8Bx_Vport_Reg_Write;
		CVD_Get_FB_Status		=              	CVD_L8Bx_Get_FB_Status;
		CVD_Get_No_Signal_Flag	=				CVD_L8Bx_Get_No_Signal_Flag;
		CVD_Get_HLock_Flag		=				CVD_L8Bx_Get_HLock_Flag;
		CVD_Get_VLock_Flag		=				CVD_L8Bx_Get_VLock_Flag;
		CVD_Get_Vline_625_Flag	=				CVD_L8Bx_Get_Vline_625_Flag;
		CVD_Get_PAL_Flag		=				CVD_L8Bx_Get_PAL_Flag;
		CVD_Get_SECAM_Flag		=				CVD_L8Bx_Get_SECAM_Flag;
		CVD_Get_Chromalock_Flag	=				CVD_L8Bx_Get_Chromalock_Flag;
		CVD_Get_Noise_Status	=				CVD_L8Bx_Get_Noise_Status;
		CVD_Get_NoBurst_Flag	=				CVD_L8Bx_Get_NoBurst_Flag;
		CVD_Get_CHB_No_Signal_Flag	=			CVD_L8Bx_Get_CHB_No_Signal_Flag;
		CVD_Get_CHB_HLock_Flag		=			CVD_L8Bx_Get_CHB_HLock_Flag;
		CVD_Get_CHB_VLock_Flag		=			CVD_L8Bx_Get_CHB_VLock_Flag;
		CVD_Get_CHB_Chromalock_Flag	=			CVD_L8Bx_Get_CHB_Chromalock_Flag;
		CVD_Get_CHB_PAL_Flag 		=			CVD_L8Bx_Get_CHB_PAL_Flag;
		CVD_Get_CHB_Vline_625_Flag		=			CVD_L8Bx_Get_CHB_Vline_625_Flag;
		CVD_Get_CHB_SECAM_Flag			=			CVD_L8Bx_Get_CHB_SECAM_Flag;
		CVD_Reset_irisyc			=			CVD_L8Bx_Reset_irisyc;
		CVD_Reset_hdct				=			CVD_L8Bx_Reset_hdct;
		CVD_Reset_cdct				=			CVD_L8Bx_Reset_cdct;
		/*
		CVD_Set_AAF_Params			=			CVD_L8Bx_Set_AAF_Params;
		CVD_Set_ADC_Params			=			CVD_L8Bx_Set_ADC_Params;
    	CVD_Set_Cresampler_Params	=			CVD_L8Bx_Set_Cresampler_Params;
    	CVD_Set_Dot_Crawl_Params	=			CVD_L8Bx_Set_Dot_Crawl_Params;
		CVD_Set_Cross_Color_Reducer	=			CVD_L8Bx_Set_Cross_Color_Reducer;
		CVD_Set_Motion_Detector		=			CVD_L8Bx_Set_Motion_Detector;
		CVD_Set_Global_Classifier	=			CVD_L8Bx_Set_Global_Classifier;
		CVD_Set_Output_Delay		=			CVD_L8Bx_Set_Output_Delay;
		CVD_Set_Output_Range		=			CVD_L8Bx_Set_Output_Range;
		CVD_Set_Blending			=			CVD_L8Bx_Set_Blending;
		*/
		CVD_Read_fld_cnt_value		=			CVD_L8Bx_Read_fld_cnt_value;
		CVD_Set_motion_mode			=			CVD_L8Bx_Set_motion_mode;
		CVD_Channel_Power_Control	=			CVD_L8Bx_Channel_Power_Control;
		CVD_OnOff_Chromalock_Ckill	=			CVD_L8Bx_OnOff_Chromalock_Ckill;
	}
	else
	{
		CVD_Program_Color_System_Main_Multi	=	CVD_L8Ax_Program_Color_System_Main_Multi;
		CVD_Set_Picture_Enhancement	=    	CVD_L8Ax_Set_Picture_Enhancement;
		CVD_Program_Color_System_CHB_NTSC	=  	CVD_L8Ax_Program_Color_System_CHB_NTSC;
		CVD_SW_Reset		=                   	CVD_L8Ax_SW_Reset;
		CVD_Power_Down		=                 	CVD_L8Ax_Power_Down;
		CVD_Get_FC_Flag		=                	CVD_L8Ax_Get_FC_Flag;
		CVD_Get_Cordic_Freq		=                	CVD_L8Ax_Get_Cordic_Freq;
//		CVD_Program_Color_System_Main		=      	CVD_L8Ax_Program_Color_System_Main;
//		CVD_Program_Color_System_CHB		=       	CVD_L8Ax_Program_Color_System_CHB;
		CVD_Set_Source_Type		=            	CVD_L8Ax_Set_Source_Type;
		CVD_Clamp_Current_Control		=      	CVD_L8Ax_Clamp_Current_Control;
		CVD_Force_Color_Kill		=           	CVD_L8Ax_Force_Color_Kill;

		CVD_Set_Scart_Overlay		=          	CVD_L8Ax_Set_Scart_Overlay;
		CVD_Reg_Init		=                   	CVD_L8Ax_Reg_Init;
		CVD_Get_Scart_AR		=               	CVD_L8Ax_Get_Scart_AR;
		CVD_Print_Vport_Version		=        	CVD_L8Ax_Print_Vport_Version;
		CVD_Vport_Output		=               	CVD_L8Ax_Vport_Output;
		CVD_Vport_Reg_Read		=             	CVD_L8Ax_Vport_Reg_Read;
		CVD_Vport_Reg_Write		=            	CVD_L8Ax_Vport_Reg_Write;
		CVD_Get_FB_Status		=               	CVD_L8Ax_Get_FB_Status;
		CVD_Get_No_Signal_Flag	=				CVD_L8Ax_Get_No_Signal_Flag;
		CVD_Get_HLock_Flag		=				CVD_L8Ax_Get_HLock_Flag;
		CVD_Get_VLock_Flag		=				CVD_L8Ax_Get_VLock_Flag;
		CVD_Get_PAL_Flag		=				CVD_L8Ax_Get_PAL_Flag;
		CVD_Get_SECAM_Flag		=				CVD_L8Ax_Get_SECAM_Flag;
		CVD_Get_Chromalock_Flag	=				CVD_L8Ax_Get_Chromalock_Flag;
		CVD_Get_Noise_Status	=				CVD_L8Ax_Get_Noise_Status;
		CVD_Get_NoBurst_Flag	=				CVD_L8Ax_Get_NoBurst_Flag;
		CVD_Get_CHB_No_Signal_Flag	=			CVD_L8Ax_Get_CHB_No_Signal_Flag;
		CVD_Get_CHB_HLock_Flag		=			CVD_L8Ax_Get_CHB_HLock_Flag;
		CVD_Get_CHB_VLock_Flag		=			CVD_L8Ax_Get_CHB_VLock_Flag;
		CVD_Get_CHB_Chromalock_Flag	=			CVD_L8Ax_Get_CHB_Chromalock_Flag;
		CVD_Get_CHB_PAL_Flag 		=			CVD_L8Ax_Get_CHB_PAL_Flag;
		CVD_Get_Vline_625_Flag		=			CVD_L8Ax_Get_Vline_625_Flag;
		CVD_Get_SECAM_Flag			=			CVD_L8Ax_Get_SECAM_Flag;
		CVD_Reset_irisyc			=			CVD_L8Ax_Reset_irisyc;

		CVD_Read_fld_cnt_value		=			CVD_L8Ax_Read_fld_cnt_value;
		CVD_Set_motion_mode			=			CVD_L8Ax_Set_motion_mode;
		CVD_Channel_Power_Control	=			CVD_L8Ax_Channel_Power_Control;
		CVD_OnOff_Chromalock_Ckill	=			CVD_L8Ax_OnOff_Chromalock_Ckill;
	}
#else //#ifdef L8_KDRIVER_SUPPORT
	else
	{
		return -1;
	}

#endif //#ifdef L8_KDRIVER_SUPPORT

	return 0;

}

/* CVD_Init 	: Initialize CVD registers to default value
					: Enables Kenel_threads(CVD main & CHB)
					: Enable CVD Interrupt 						*/
int CVD_Init(LX_AFE_CVD_SELECT_T select_main_sub)
{
	int result;

	result	= 0;


#ifdef	KDRV_CONFIG_PM
	gAfePmData = (S_AFE_PM_DATA *)kmalloc(sizeof(S_AFE_PM_DATA) , GFP_KERNEL);
	gAfePmData->CVDInitStatus = FALSE;
	gAfePmData->SelectMainSub = select_main_sub;
#endif

	if(select_main_sub == LX_CVD_MAIN)
	{
		AFE_PRINT("Main CVD Initialize Start... DTVSoC Chip Verstion [0x%x]\n", lx_chip_rev() );

		if (lx_chip_rev() == LX_CHIP_REV_L8_A1)		    	pVportRegCfg = &gRegCfgVport[0];	// 0 : L8 A1 & B0 for Real.
		else if (lx_chip_rev() == LX_CHIP_REV_L8_B0)		pVportRegCfg = &gRegCfgVport[0];	// 0 : L8 A1 & B0 for Real.
		else if (lx_chip_rev() >= LX_CHIP_REV_L9_A0)		pVportRegCfg = &gRegCfgVport[1];	// 1 : L9 Ax
		else return -1;	// Unkown chip revision

		CVD_DEBUG("gRegCfg [0x%x]\n", pVportRegCfg->vport_reg_base_addr);



		if (lx_chip_rev() >= LX_CHIP_REV( L9, B0)) // won.hur : added of L9B0 migration @2011.08.24
		{
				gpRealRegVPORT_L9B0 = (volatile VPORT_L9B0_REG_T __iomem *)ioremap(pVportRegCfg->vport_reg_base_addr , pVportRegCfg->vport_reg_size);
			if(gpRealRegVPORT_L9B0 == NULL)
			{
				AFE_ERROR("ERROR : can't allocate for register\n");
				return -1;
			}
			gpRegVPORT_L9B0 = (volatile VPORT_L9B0_REG_T __iomem *)kmalloc(sizeof(VPORT_L9B0_REG_T), GFP_ATOMIC);
			if(gpRegVPORT_L9B0 == NULL)
			{
				AFE_ERROR("ERROR : can't allocate for shadow register\n");
				return -1;
			}

		}
		else if ( lx_chip_rev() >= LX_CHIP_REV( L9, A0 ))
		{
			gpRealRegVPORT_L9A0 = (volatile VPORT_L9A0_REG_T __iomem *)ioremap(pVportRegCfg->vport_reg_base_addr , pVportRegCfg->vport_reg_size);
			if(gpRealRegVPORT_L9A0 == NULL)
			{
				AFE_ERROR("ERROR : can't allocate for register\n");
				return -1;
			}
			gpRegVPORT_L9A0 = (volatile VPORT_L9A0_REG_T __iomem *)kmalloc(sizeof(VPORT_L9A0_REG_T), GFP_ATOMIC);
			if(gpRegVPORT_L9A0 == NULL)
			{
				AFE_ERROR("ERROR : can't allocate for shadow register\n");
				return -1;
			}

		}
		else if ( lx_chip_rev( ) == LX_CHIP_REV( L8, B0 ) )
		{
			gVportRegBx = (volatile VPORT_RegBx_T *)ioremap(pVportRegCfg->vport_reg_base_addr , pVportRegCfg->vport_reg_size);

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
		}
		else
		{
			gVportRegAx = (volatile VPORT_RegAx_T *)ioremap(pVportRegCfg->vport_reg_base_addr , pVportRegCfg->vport_reg_size);

			if(gVportRegAx == NULL)
			{
				AFE_ERROR("ERROR : can't allocate for register\n");
				return -1;
			}
		}

		//spin_lock_init(&cvd_lock);

		CVD_InitChipRev();

		CVD_Power_Down(LX_CVD_MAIN, FALSE);
		CVD_Reg_Init(LX_CVD_MAIN);
		//Init Register to NTSC Mode
		CVD_Program_Color_System_Main_Multi(&gSetColorSystemMain[CVD_STATE_NTSC]);

#ifdef CVD_CH42_SKIP_WORKAROUND
		CVD_Clamp_Current_Control(0x3);
#endif
#ifdef CVD_AGC_PEAK_CONTROL_WORKAROUND		//disabled
		g_CVD_AGC_Peak_Control.bEnable = TRUE;
		g_CVD_AGC_Peak_Control.white_ratio_th = 60;
		g_CVD_AGC_Peak_Control.white_frame_max_th = 30;
		g_CVD_AGC_Peak_Control.white_frame_on = 8;
		g_CVD_AGC_Peak_Control.white_frame_off = 8;
		g_CVD_AGC_Peak_Control.print_en = 0;
#endif
#if 0
		g_CVD_AGC_Peak_Nominal_Control.bEnable = TRUE ;		//120109
		//g_CVD_AGC_Peak_Nominal_Control.tnr_x_avg_t_th = 0x18 ;
		g_CVD_AGC_Peak_Nominal_Control.tnr_x_avg_t_th = 0x100 ;
		g_CVD_AGC_Peak_Nominal_Control.tnr_x_avg_s_th = 0x20 ;
		g_CVD_AGC_Peak_Nominal_Control.tpd_s_status_th = 0xc0 ;
		g_CVD_AGC_Peak_Nominal_Control.pattern_frame_max_th = 0x8 ;
		g_CVD_AGC_Peak_Nominal_Control.pattern_frame_on = 0x4 ;
		g_CVD_AGC_Peak_Nominal_Control.pattern_frame_off = 0x4 ;
		g_CVD_AGC_Peak_Nominal_Control.pattern_update = 0 ;
		g_CVD_AGC_Peak_Nominal_Control.pattern_frame_cnt = 0 ;
		g_CVD_AGC_Peak_Nominal_Control.reg_agc_peak_nominal = 0xa0 ;
		g_CVD_AGC_Peak_Nominal_Control.reg_contrast = 0x82 ;
		g_CVD_AGC_Peak_Nominal_Control.reg_brightness = 0x20 ;
		g_CVD_AGC_Peak_Nominal_Control.pattern_found = 0x0 ;
		g_CVD_AGC_Peak_Nominal_Control.print_en = 0 ;
		g_CVD_AGC_Peak_Nominal_Control.noise_th = 0xa0 ;
#endif
		//for Pattern Detection Using Global Motion
		g_CVD_Pattern_Detection_t.bEnable = TRUE ;
		g_CVD_Pattern_Detection_t.prev_motion_value = 0 ;
	//	g_CVD_Pattern_Detection_t.motion_diff_th = 3 ;
		g_CVD_Pattern_Detection_t.motion_diff_th = 1 ;
		g_CVD_Pattern_Detection_t.motion_count_th = 40 ;
		g_CVD_Pattern_Detection_t.motion_count_max = 50 ;
		g_CVD_Pattern_Detection_t.global_motion_count = 0 ;
		g_CVD_Pattern_Detection_t.pattern_found = 0 ;
//		g_CVD_Pattern_Detection_t.tnr_x_avg_t_th = 0x100 ;
		g_CVD_Pattern_Detection_t.tnr_x_avg_t_th = 0x30 ;
		g_CVD_Pattern_Detection_t.tnr_x_avg_s_th = 0x30 ;
	//g_CVD_Pattern_Detection_t.tnr_x_avg_s_th = 0x40 ;
		g_CVD_Pattern_Detection_t.tpd_s_status_th = 0x80 ;

		gCVD_Status_3CS.noisy_signal_detected = 0;
		gCVD_Status_3CS.no_color_detected = 0;

		if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0 ))  //won.hur : added for L9B0 migration
		{
			CVD_L9Bx_Program_Color_System_CS(CVD_SEL_CS_CS0, &gSetColorSystemMain[CVD_STATE_PAL60]);
			CVD_L9Bx_Program_Color_System_CS(CVD_SEL_CS_CS1, &gSetColorSystemMain[CVD_STATE_PALm]);
		}
		else if ( lx_chip_rev() >= LX_CHIP_REV( L9, A0 ))  //won.hur : changed! @2011.08.24
		{
			CVD_L9Ax_Program_Color_System_CS(CVD_SEL_CS_CS0, &gSetColorSystemMain[CVD_STATE_PAL60]);
			CVD_L9Ax_Program_Color_System_CS(CVD_SEL_CS_CS1, &gSetColorSystemMain[CVD_STATE_PALm]);
		}

		CVD_SW_Reset(LX_CVD_MAIN);

		if ( lx_chip_rev( ) < LX_CHIP_REV( L8, B0 ) )
			CVD_Set_Picture_Enhancement(FALSE, FALSE);
		else if ( lx_chip_rev( ) == LX_CHIP_REV( L8, B0 ) )
			CVD_Set_PQ_Mode(LX_CVD_PQ_NTSC_M);
		// From L9 CVD PE Setting is moved to CVD_Task_3cdtect for PAL/SECAM support
		/*
		else
		{
			CVD_L9Ax_Set_AAF_Params(&gSetAAF_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_ADC_Params(&gSetCresampler_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Cresampler_Params(&gSetCresampler_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Dot_Crawl_Params(&gSetDotCrawl_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Cross_Color_Reducer(&gSetCrossColorReducer_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Motion_Detector(&gSetMotionDetector_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Global_Classifier(&gSetGlobalClassifier_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Output_Delay( &gSetOutputDelay_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Output_Range( &gSetOutputRange_L9Ax[CVD_PE_NTSC]);
			CVD_L9Ax_Set_Blending( &gSetBlending_L9Ax[CVD_PE_NTSC]);
		}
		*/

	//Initialize CVD semaphore
	OS_InitMutex(&g_CVD_Sema, OS_SEM_ATTR_DEFAULT);


#ifdef ENABLE_CVD_TIMER
			spin_lock(&cvd_lock);

			if(main_timer_state < 1)
			{
				OS_InitTimer( &main_timer);
				main_timer_state = 1;
				OS_StartTimer( &main_timer, MainCVD_Timer, OS_TIMER_TIMETICK, MAINCVD_TIMEOUT, 0 );
				spin_unlock(&cvd_lock);
				AFE_PRINT("Creating Main CVD Timer...\n");
			}
			else
			{
				spin_unlock(&cvd_lock);
				AFE_PRINT("Main CVD Timer Already Created\n");
			}

#endif
#ifdef ENABLE_CVD_THREAD

//			spin_lock(&cvd_lock);

#if 0
			if(gPid_cvd_thread < 0)
			{
				gPid_cvd_thread = kernel_thread( MainCVD_Thread, (void*)NULL, CLONE_KERNEL);
				gCVD_thread_running = 1;
			}
#endif
			stCVD_Thread = kthread_create( MainCVD_Thread, (void*)NULL, "cvd_thread");

			if(stCVD_Thread)
			{
				wake_up_process(stCVD_Thread);
				gCVD_thread_running = 1;
				AFE_PRINT("CVD Thread [%d]\n", stCVD_Thread->pid);
			}


			else
			{
				AFE_ERROR("CVD Thread Already Created\n");
			}

#endif

	}

	else
	{
		AFE_PRINT("CHB CVD Initialize Start...\n");

		if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0 ))
		{
			AFE_PRINT("L9B0 CHB Init bypass\n");
		}
		else if ( lx_chip_rev() >= LX_CHIP_REV( L9, A0 ))
		{
			pVportRegCfg = &gRegCfgVport[1];	// 1 : L9 Ax

			CVD_DEBUG("CVD gRegCfg [0x%x]\n", pVportRegCfg->chbcvd_reg_base_addr);
			CVD_DEBUG("CVD chbcvd_reg_size [0x%x]\n", pVportRegCfg->chbcvd_reg_base_addr);

			gpRealRegCHBCVD_L9 = (volatile CHBCVD_L9_REG_T __iomem *)ioremap(pVportRegCfg->chbcvd_reg_base_addr , pVportRegCfg->chbcvd_reg_size);
			if(gpRealRegCHBCVD_L9 == NULL)
			{
				AFE_ERROR("ERROR : can't allocate for register\n");
				return -1;
			}
			gpRegCHBCVD_L9 = (volatile CHBCVD_L9_REG_T __iomem *)kmalloc(sizeof(CHBCVD_L9_REG_T), GFP_ATOMIC);
			if(gpRegCHBCVD_L9 == NULL)
			{
				AFE_ERROR("ERROR : can't allocate for shadow register\n");
				return -1;
			}

		}

		CVD_Power_Down(LX_CVD_SUB, FALSE);

		if ( lx_chip_rev() < LX_CHIP_REV( L9, B0 ))
		{
			CVD_Reg_Init(LX_CVD_SUB);
		//reg init for CHB CVD
			CVD_Program_Color_System_CHB_NTSC();
		}
		//CVD_SW_Reset(LX_CVD_SUB); //Reg reset for CHB is moved to CVD_Reg_Init

#if 0	//Disable CHB Timer
#ifdef ENABLE_CVD_TIMER
		if(chb_timer_state < 1)
		{
			AFE_PRINT("Creating CHB CVD Timer...\n");
			OS_InitTimer( &chb_timer);
			OS_StartTimer( &chb_timer, CHBCVD_Timer, OS_TIMER_TIMETICK, CHBCVD_TIMEOUT, 0 );
			chb_timer_state = 1;
		}
		else
			AFE_PRINT("CHB CVD Timer Already Created\n");
#endif
#endif
	}

	if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0 ) )	// won.hur : added for L9B0 migration @2011.08.24
	{
		VPORT_L9B0_Wr(cvd_intr1_enable0, L9_CVD_INTERRUPT_MASK);
		VPORT_L9B0_WrFL(cvd_intr1_enable0);
	}
	//Enable CVD Interrupt (No Signal, VLock, HLock)
	//gVportReg->intr_vp1_enable.intr_vp1_enable |= L8_MAIN_CVD_INTERRUPT_MASK;
	// TODO : dws
	// L9A0 has two cvd interrupt. One is for CVD main & the other is for VBI.
	else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
	{
		//VPORT_L9A0_RdFL(cvd_intr1_enable0);
		VPORT_L9A0_Wr(cvd_intr1_enable0, L9_CVD_INTERRUPT_MASK);
		VPORT_L9A0_WrFL(cvd_intr1_enable0);
	}
	else if ( lx_chip_rev( ) == LX_CHIP_REV( L8, B0 ) )
		gVportRegBx->intr_vp1_enable.intr_vp1_enable |= L8_MAIN_CVD_INTERRUPT_MASK;
	else
		gVportRegAx->intr_vp1_enable.intr_vp1_enable |= L8_MAIN_CVD_INTERRUPT_MASK;

	if (!gCVDInitStatus)
	{
		if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))
			result = request_irq(L9_IRQ_CVD_BCPU, (irq_handler_t)CVD_L9_IRQ_Handler, 0, "CVD", NULL);
		else
			result = request_irq(L8_IRQ_VP_IRIS, (irq_handler_t)CVD_L8_IRQ_Handler, 0, "CVD", NULL);

		if(result)
		{
			AFE_PRINT("!!! CVD Request IRQ Failed : return[%d]\n", result);
			return -1;
		}
		gCVDInitStatus = TRUE;
#ifdef	KDRV_CONFIG_PM
		gAfePmData->CVDInitStatus = TRUE;
#endif
	}


	CVD_Print_Vport_Version();

	return 0;
}

void CVD_UnInit(void)
{
	if(gCVDInitStatus)
	{
		if (lx_chip_rev() >= LX_CHIP_REV(L9, A0))
			free_irq(L9_IRQ_CVD_BCPU, NULL);
		else
			free_irq(L8_IRQ_VP_IRIS, NULL);
		gCVDInitStatus = FALSE;
#ifdef	KDRV_PM_CONFIG
		kfree(gAfePmData);
#endif
	}

	/* kill_proc is Not defined !!!!
	   How can I stop the kernel threads???? */
#if 0
#ifdef ENABLE_CVD_TIMER
	OS_StopTimer( &main_timer );
	OS_StopTimer( &chb_timer );
#endif
#endif
}

static irqreturn_t CVD_L9_IRQ_Handler(int irq, void *dev_id, struct pt_regs *regs)
{
	UINT32 cvd_int_status = 0;
	UINT32 vbi_int_status = 0;
	UINT32	int_mask;

	// won.hur : chip selection added for L9B0 migration
	// Originally there wasn't any chip selection
	// But, since the orignal code used "VPORT_L9A0_" function,
	// there was no choice but to add them...
	// @2011.08.24
	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		VPORT_L9B0_RdFL(cvd_intr1_intr0);
		cvd_int_status = VPORT_L9B0_Rd(cvd_intr1_intr0);

		VPORT_L9B0_RdFL(cvd_intr1_intr1);
		vbi_int_status = VPORT_L9B0_Rd(cvd_intr1_intr1);

	}
	else
	{
		VPORT_L9A0_RdFL(cvd_intr1_intr0);
		cvd_int_status = VPORT_L9A0_Rd(cvd_intr1_intr0);

		VPORT_L9A0_RdFL(cvd_intr1_intr1);
		vbi_int_status = VPORT_L9A0_Rd(cvd_intr1_intr1);
	}
	//	CVD_DEBUG("ISR:VBI Int. [0x%x]\n", vbi_int_status);

#if 0
	if(cvd_int_status & (L9_CVD_INTERRUPT_MASK))
	{
		CVD_DEBUG("ISR:CVD Int. [0x%x]\n", cvd_int_status);
		//If vbi interrupt occurred, clear pending bit must be set.
		if(vbi_int_status > 0)
			int_mask = L9_CVD_INTERRUPT_PENDING;
		else
			int_mask = 0;

		if(cvd_int_status & L9_CVD_INTERRUPT_MASK)
		{

			if(((CVD_Get_HLock_Flag())||(CVD_Get_VLock_Flag())) )
			{
				if ((Current_State == CVD_STATE_NTSC_FORCED) || (Current_State == CVD_STATE_PALi_FORCED))
				{
					CVD_DEBUG("ISR:CVD Exist From Force state[%d] \n", Current_State);
					//gExitFromForcedState = 1;
					Next_State = CVD_STATE_VideoReadyToDetect;
					Current_State = CVD_STATE_VideoReadyToDetect;
				}
			}
			else
			{
				//		spin_unlock(&cvd_lock);
				CVD_DEBUG("ISR:CVD Main : Int. but Not Lock\n");
			}

		}

//		VPORT_L9A0_RdFL(cvd_intr1_clear0);

//		int_mask = VPORT_L9A0_Rd(cvd_intr1_clear0);
//		int_mask |= L9_CVD_INTERRUPT_MASK;
		/*
		VPORT_L9A0_RdFL(cvd_intr1_clear0);

		int_mask = VPORT_L9A0_Rd(cvd_intr1_clear0);
		printk("int_mask[0x%x]\n", int_mask);
		int_mask &= ~L9_CVD_INTERRUPT_MASK;
		printk("int_mask[0x%x]\n", int_mask);

		VPORT_L9A0_Wr(cvd_intr1_clear0, int_mask);
		VPORT_L9A0_WrFL(cvd_intr1_clear0);
		*/

		// Do not start timer on chroma lock
#if	ENABLE_CVD_INTERRUPT_CONTROL
		if(cvd_int_status & L9_CVD_INTERRUPT_MASK)
		{

#ifdef ENABLE_CVD_THREAD

			//spin_lock(&cvd_lock);

			if(gCVD_thread_running < 1)	// Thread is sleeping
			{
				if(((CVD_Get_HLock_Flag())||(CVD_Get_VLock_Flag())) )
				{
					if(gForce_thread_sleep < 1)
					{
						wake_up_interruptible(&WaitQueue_CVD);
//						CVD_DEBUG("ISR:CVD wake_up thread \n");
					}
					else
					{
//						CVD_DEBUG("ISR:CVD thread forced sleep \n");
					}
			//		spin_unlock(&cvd_lock);
				}
				else
				{
			//		spin_unlock(&cvd_lock);
//					CVD_DEBUG("ISR:CVD Main : Int. but Not Lock\n");
#ifdef CVD_CH42_SKIP_WORKAROUND
					//CVD_ClampAGC_OnOff(0); // agc off on No signal for ch42 no signal workaround
					//TODO:dws : remove I2C Access (clamp_current_control )
					//CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
#endif
				}
			}
			else
			{
				if(MainCVD_Same_State_Count > 0)
					MainCVD_Same_State_Count --;
		//	spin_unlock(&cvd_lock);
//			CVD_DEBUG("ISR: same state[%d]\n", MainCVD_Same_State_Count);
			}

#endif
		}
#endif

		int_mask |= L9_CVD_INTERRUPT_MASK;

		//won.hur : added chip selection for L9B0 migration
		//@2011.08.24
		if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0))
		{
			VPORT_L9B0_Wr(cvd_intr1_clear0, int_mask);
			VPORT_L9B0_WrFL(cvd_intr1_clear0);

		}
		else
		{
			VPORT_L9A0_Wr(cvd_intr1_clear0, int_mask);
			VPORT_L9A0_WrFL(cvd_intr1_clear0);
		}
	}
#endif

	//VBI Data In
	if(vbi_int_status & L9_VBI_INTERRUPT_MASK)
	{
		if( (ENABLE_ANALOGCC) && (gAutoProgramming == FALSE) )
		{
			extern int TTX_CC_InterruptFunc(unsigned int status);
			TTX_CC_InterruptFunc(vbi_int_status);
		}
		if(ENABLE_ANALOGTTX)
		{
			extern int TTX_InterruptFunc(unsigned int status);
			TTX_InterruptFunc(vbi_int_status);
		}

//		VPORT_L9A0_RdFL(cvd_intr1_clear1);

//		int_mask = VPORT_L9A0_Rd(cvd_intr1_clear1);
//		int_mask |= L9_VBI_INTERRUPT_MASK;
		int_mask = L9_VBI_INTERRUPT_MASK;

		if( lx_chip_rev() >= LX_CHIP_REV( L9, B0))
		{
			VPORT_L9B0_Wr(cvd_intr1_clear1, int_mask);
			VPORT_L9B0_WrFL(cvd_intr1_clear1);
		}
		else
		{
			VPORT_L9A0_Wr(cvd_intr1_clear1, int_mask);
			VPORT_L9A0_WrFL(cvd_intr1_clear1);
		}
//		VPORT_L9A0_RdFL(cvd_intr1_clear1);

//		int_mask = VPORT_L9A0_Rd(cvd_intr1_clear1);
//		int_mask &= ~L9_VBI_INTERRUPT_MASK;

//		VPORT_L9A0_Wr(cvd_intr1_clear1, int_mask);
//		VPORT_L9A0_WrFL(cvd_intr1_clear1);
	}

	return IRQ_HANDLED;
}

static irqreturn_t CVD_L8_IRQ_Handler(int irq, void *dev_id, struct pt_regs *regs)
{
#if 1
	UINT32 cvd_int_status = 0;

	if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
		cvd_int_status = *(UINT32*)&(gVportRegBx->intr_vp1_intr);
	else
		cvd_int_status = *(UINT32*)&(gVportRegAx->chb_cvd_pal_detected);

	//VBI Data In
	if(cvd_int_status & L8_VBI_INTERRUPT_MASK)
	{

		if(ENABLE_ANALOGCC)
		{
			extern int TTX_CC_InterruptFunc(unsigned int status);
			TTX_CC_InterruptFunc(cvd_int_status);
		}
		if(ENABLE_ANALOGTTX)
		{
			extern int TTX_InterruptFunc(unsigned int status);
			TTX_InterruptFunc(cvd_int_status);
		}
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
		{
			gVportRegBx->intr_vp1_clear.intr_vp1_clear |= L8_VBI_INTERRUPT_MASK;	// interrupt clear
			gVportRegBx->intr_vp1_clear.intr_vp1_clear &= ~L8_VBI_INTERRUPT_MASK;
		}
		else
		{
			gVportRegAx->intr_vp1_clear.intr_vp1_clear |= L8_VBI_INTERRUPT_MASK;	// interrupt clear
			gVportRegAx->intr_vp1_clear.intr_vp1_clear &= ~L8_VBI_INTERRUPT_MASK;
		}

	}
#if 1
	if(cvd_int_status & (L8_MAIN_CVD_INTERRUPT_MASK))
	{

		if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
		{
			gVportRegBx->intr_vp1_clear.intr_vp1_clear |= (L8_MAIN_CVD_INTERRUPT_MASK);	// interrupt clear
			gVportRegBx->intr_vp1_clear.intr_vp1_clear &= ~(L8_MAIN_CVD_INTERRUPT_MASK);
		}
		else
		{
			gVportRegAx->intr_vp1_clear.intr_vp1_clear |= (L8_MAIN_CVD_INTERRUPT_MASK);	// interrupt clear
			gVportRegAx->intr_vp1_clear.intr_vp1_clear &= ~(L8_MAIN_CVD_INTERRUPT_MASK);
		}

		CVD_DEBUG("CVD Int. [0x%x]\n", cvd_int_status);

		if( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
		{
			// Do not start timer on chroma lock
#if	ENABLE_CVD_INTERRUPT_CONTROL
			if(cvd_int_status & L8_MAIN_CVD_INTERRUPT_MASK)
			{

#ifdef ENABLE_CVD_THREAD

	//			spin_lock(&cvd_lock);

				if(gCVD_thread_running < 1)
				{
					if(((CVD_Get_HLock_Flag())||(CVD_Get_VLock_Flag())) )
					{
						if(gForce_thread_sleep < 1)
						{
							wake_up_interruptible(&WaitQueue_CVD);
							CVD_DEBUG("ISR:CVD wake_up thread \n");
						}
						else
							CVD_DEBUG("ISR:CVD thread forced sleep \n");
	//					spin_unlock(&cvd_lock);
					}
					else
					{
						//					spin_unlock(&cvd_lock);
						CVD_DEBUG("CVD Main : Int. but Not Lock\n");
#ifdef CVD_CH42_SKIP_WORKAROUND
						//CVD_ClampAGC_OnOff(0); // agc off on No signal for ch42 no signal workaround
						CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
#endif
					}
				}
				else
				{
					if(MainCVD_Same_State_Count > 0)
						MainCVD_Same_State_Count --;
	//				spin_unlock(&cvd_lock);
					CVD_DEBUG("CVD Main : Int. same state[%d]\n", MainCVD_Same_State_Count);
				}

#endif
#if 0	// timer
				spin_lock(&cvd_lock);

				if((main_timer_state < 1))
				{
					//110414 No_Signal_Flag is not working well
					//if(((CVD_Get_HLock_Flag())||(CVD_Get_VLock_Flag())) && ~CVD_Get_No_Signal_Flag())
					if(((CVD_Get_HLock_Flag())||(CVD_Get_VLock_Flag())) )
					{
						main_timer_state = 1;
						OS_StartTimer( &main_timer, MainCVD_Timer, OS_TIMER_TIMETICK,MAINCVD_TIMEOUT , 0 );
#ifdef CVD_KERNEL_DEBUG
						AFE_PRINT("CVD Main : Starting Timer\n");
#endif
					}
					else
					{
#ifdef CVD_KERNEL_DEBUG
						AFE_PRINT("CVD Main : Int. but Not Lock\n");
#endif
#ifdef CVD_CH42_SKIP_WORKAROUND
						//CVD_ClampAGC_OnOff(0); // agc off on No signal for ch42 no signal workaround
						CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
#endif
					}
				}
				else
				{
					if(MainCVD_Same_State_Count > 0)
						MainCVD_Same_State_Count --;
#ifdef CVD_KERNEL_DEBUG
					AFE_PRINT("timer_state [%d], same state[%d]\n", main_timer_state, MainCVD_Same_State_Count);
#endif
				}
				spin_unlock(&cvd_lock);
#endif
			}
#endif
		}
	}
#endif

#endif
	return IRQ_HANDLED;
}

/* On Brazil (PAL-M) weak RF Signal, AGC gain sometimes became high value result in white picture.
   Disabling AGC is needed on this weak RF signal( No h-lock, high noise level )
   */
int CVD_AGC_Bypass_Function(UINT32 noise_status, UINT32 hlock_flag, UINT32 no_signal_flag )
{
	int ret = 0;
	static int bypass_enable = 0;

	OS_LockMutex(&g_CVD_Sema);
	//CVD_DEBUG("%s entered :  %d,%d,%d, %d \n",__func__, noise_status, hlock_flag, no_signal_flag, bypass_enable);

	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0)) {
		if( ( noise_status == CVD_L9B0_NOISE_MAX) && ( hlock_flag == 0 )&& ( no_signal_flag == 0)) {
			if(bypass_enable == 0) {
				bypass_enable = 1;
				CVD_L9Bx_AGC_Bypass(bypass_enable);
				//CVD_L9Bx_Set_dcrestore_accum_width(0x1b);
				//111109 by kim.min for weak cable RF signals.
				CVD_L9Bx_Set_dcrestore_accum_width(DCRESTORE_ACCUM_WIDTH_FOR_WEAK_RF);
				CVD_DEBUG("## Enable AGC Bypass\n");
			}
		}
		else if(bypass_enable) {
				bypass_enable = 0;
				CVD_L9Bx_AGC_Bypass(bypass_enable);
				if(gPrevious_Main_System < CVD_STATE_VideoNotReady)
					CVD_L9Bx_Set_dcrestore_accum_width(gSetColorSystemMain[gPrevious_Main_System].reg_dcrestore_accum_width);
				CVD_DEBUG("## Disable AGC Bypass\n");
			}
		/*
		if(noise_status == CVD_L9B0_NOISE_MAX)
			CVD_L9Bx_Set_dcrestore_accum_width(0x28);
		else if(noise_status < 0x100)
			CVD_L9Bx_Set_dcrestore_accum_width(0x22);
			*/
	}
	OS_UnlockMutex(&g_CVD_Sema);
	return ret;
}


int CVD_Set_NTSC_Only(UINT32 arg)
{
	LX_AFE_VIDEO_SYSTEM_INFO_T system_info;

	if (copy_from_user(&system_info, (void __user *)arg, sizeof(LX_AFE_VIDEO_SYSTEM_INFO_T)))
	{
		return -1;
	}

	if(system_info.cvd_main_sub == LX_CVD_MAIN)
	{
		if(system_info.cvd_video_system == LX_COLOR_SYSTEM_NTSC_M)
		{
			AFE_PRINT("############ Setting NTSC ONLY MODE ##################\n");
			gNTSC_ONLY_Main = TRUE;
			}
		else
			gNTSC_ONLY_Main = FALSE;
	}
	else if(system_info.cvd_video_system == LX_COLOR_SYSTEM_NTSC_M)
		gNTSC_ONLY_CHB = TRUE;
	else
		gNTSC_ONLY_CHB = FALSE;

	return 0;
}

int CVD_Set_Color_System(UINT32 arg)
{
	LX_AFE_VIDEO_SYSTEM_INFO_T system_info;

	if (copy_from_user(&system_info, (void __user *)arg, sizeof(LX_AFE_VIDEO_SYSTEM_INFO_T)))
	{
		return -1;
	}

	if(system_info.cvd_main_sub == LX_CVD_MAIN)
	{
		if(system_info.cvd_video_system <= LX_COLOR_SYSTEM_MULTI)
		{
			g_CVD_Color_System_Support = system_info.cvd_video_system;
			AFE_PRINT("### Setting Color System to [0x%x] ###\n", g_CVD_Color_System_Support);
		}
		else
		{
			AFE_PRINT("### Invalid Color System [%d] ###\n", system_info.cvd_video_system);
			return -1;
		}
	}
	else if(system_info.cvd_video_system == LX_COLOR_SYSTEM_NTSC_M)
		gNTSC_ONLY_CHB = TRUE;
	else
		gNTSC_ONLY_CHB = FALSE;

	return 0;
}

void CVD_Get_Timing_Info(UINT32 arg)
{
	LX_AFE_CVD_TIMING_INFO_T	timing_info;
	CVD_STATE_T	cvd_state;
	UINT16 fld_lfcnt, fld_hfcnt;
	UINT32 diff;
	//UINT32 curr_3dcom_state;

	if (copy_from_user(&timing_info, (void __user *)arg, sizeof(LX_AFE_CVD_TIMING_INFO_T)))
	{
		return;
	}
	if(timing_info.cvd_main_sub == LX_CVD_MAIN)
		cvd_state = gMainCVD_State;
	else
		cvd_state = gCHBCVD_State;

	switch(cvd_state)
	{
		case CVD_STATE_NTSC:
			timing_info.cvd_standard = LX_NTSC_M;
			break;
		case CVD_STATE_NTSC443:
			timing_info.cvd_standard = LX_NTSC_443;
			break;
		case CVD_STATE_PALi:
			timing_info.cvd_standard = LX_PAL_BG;
			break;
		case CVD_STATE_PALm:
			timing_info.cvd_standard = LX_PAL_M;
			break;
		case CVD_STATE_PALCn:
			timing_info.cvd_standard = LX_PAL_CN;
			break;
		case CVD_STATE_PAL60:
			timing_info.cvd_standard = LX_PAL_60;
			break;
		case CVD_STATE_SECAM:
			timing_info.cvd_standard = LX_SECAM;
			break;
		default:
			timing_info.cvd_standard = LX_NON_STANDARD;//gogosing
			break;
	}

	//Instead of reading input VFreq., read forced VFreq. of CVD to determine CVD's running VFreq.
	//if (CVD_Get_Vline_625_Flag())	// 625 line : 50Hz System
	if (CVD_Get_Vline_625_Reg())	// 625 line : 50Hz System
			timing_info.u16_VFreq = 500;
	else
			timing_info.u16_VFreq = 599;

	// For test only
	if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
	{
		// No SW 3DCOMB Control on L9B0
	}
	else if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
	{
		if(1)
		{
			CVD_Read_fld_cnt_value(&fld_hfcnt, &fld_lfcnt);

			diff = abs(fld_lfcnt - fld_hfcnt);
			if((fld_hfcnt >= gHf_thr_value) && (fld_lfcnt < gLf_thr_value))
				curr_g3DCOMB_State = CVD_3DCOMB_STATE_10;
			else if((fld_hfcnt < gHf_thr_value) && (fld_lfcnt >= gLf_thr_value))
				curr_g3DCOMB_State = CVD_3DCOMB_STATE_30;
			else if((fld_hfcnt >= gHf_thr_value) && (fld_lfcnt >= gLf_thr_value))
			{
				if(diff <= gDiff_thr_value)
					curr_g3DCOMB_State = CVD_3DCOMB_STATE_22;
				else if(fld_lfcnt > fld_hfcnt)
					curr_g3DCOMB_State = CVD_3DCOMB_STATE_23;
				else
					curr_g3DCOMB_State = CVD_3DCOMB_STATE_21;
			}
			else
				curr_g3DCOMB_State = CVD_3DCOMB_STATE_00;

			if(gEnable_Status_Fld_Flag)
				AFE_PRINT("# lfcnt[0x%x], hfcnt[0x%x], diff[0x%x], curr_state[0x%x]\n", fld_lfcnt, fld_hfcnt, diff, curr_g3DCOMB_State );
		}

		if(gEnable_SW3DCOMBControl > 0)
		{
			if(curr_g3DCOMB_State != prev_g3DCOMB_State)
			{
				if( (curr_g3DCOMB_State == CVD_3DCOMB_STATE_22) || (curr_g3DCOMB_State == CVD_3DCOMB_STATE_10))
				{
					if(gEnable_Monitor_3DCOMB_State)
						AFE_PRINT("# Set 3DCOMB to LF: lfcnt[0x%x], hfcnt[0x%x], diff[0x%x], c_st[0x%x]\n", fld_lfcnt, fld_hfcnt, diff, curr_g3DCOMB_State);
					CVD_Set_motion_mode(0x0, 0x1);
				}
				else if (curr_g3DCOMB_State == CVD_3DCOMB_STATE_30)
				{
					if(gEnable_Monitor_3DCOMB_State)
						AFE_PRINT("# Set 3DCOMB to [%d][H/W(0), LF(1), HF(2)]: lfcnt[0x%x], hfcnt[0x%x], diff[0x%x], c_st[0x%x]\n", gRegion_30_mode, fld_lfcnt, fld_hfcnt, diff, curr_g3DCOMB_State);
					CVD_Set_motion_mode(0x0, gRegion_30_mode);
				}
				else
				{
					if(gEnable_Monitor_3DCOMB_State)
						AFE_PRINT("# Set 3DCOMB to H/W: lfcnt[0x%x], hfcnt[0x%x], diff[0x%x], c_st[0x%x]\n", fld_lfcnt, fld_hfcnt, diff, curr_g3DCOMB_State);
					CVD_Set_motion_mode(0x1, 0x0);
				}
				prev_g3DCOMB_State = curr_g3DCOMB_State;
			}

		}
		else
		{
		//CVD_Set_motion_mode(0x1, 0x0);
		}

	}

	if (copy_to_user((UINT32*)arg, (UINT32*)&timing_info, sizeof(LX_AFE_CVD_TIMING_INFO_T)))
	{
		return;
	}
}

int CVD_Get_States(UINT32 arg)
{
	LX_AFE_CVD_STATES_INFO_T	cvd_states_info;
	//UINT32 status_reg;

	if (copy_from_user(&cvd_states_info, (void __user *)arg, sizeof(LX_AFE_CVD_STATES_INFO_T)))
	{
		return -1;
	}

#ifdef USE_REGISTER_VALUE_FOR_SYNC_STATES
/* Returns Current Sync Register values(NOT the status of CVD thread) */

	cvd_states_info.cvd_status =0;

	if(cvd_states_info.cvd_main_sub == LX_CVD_MAIN)
	{
		if(CVD_Get_No_Signal_Flag())
		{
			//110414 No_Signal_Flag is not working well
			cvd_states_info.cvd_status |= LX_NO_SIGNAL;
	//		CVD_ClampAGC_OnOff(0); // agc off on No signal for ch42 no signal workaround
		}
		if(CVD_Get_HLock_Flag())
			cvd_states_info.cvd_status |= LX_HLOCK;
		if(CVD_Get_VLock_Flag())
			cvd_states_info.cvd_status |= LX_VLOCK;
		if(CVD_Get_Chromalock_Flag())
			cvd_states_info.cvd_status |= LX_CHROMALOCK;
		if(CVD_Get_Vline_625_Flag())
			cvd_states_info.cvd_status |= LX_625LINES_DETECTED;
	}
	else
	{
		if(CVD_Get_CHB_No_Signal_Flag())
			cvd_states_info.cvd_status |= LX_NO_SIGNAL;
		if(CVD_Get_CHB_HLock_Flag())
			cvd_states_info.cvd_status |= LX_HLOCK;
		if(CVD_Get_CHB_VLock_Flag())
			cvd_states_info.cvd_status |= LX_VLOCK;
		if(CVD_Get_CHB_Chromalock_Flag())
			cvd_states_info.cvd_status |= LX_CHROMALOCK;
		if(CVD_Get_CHB_Vline_625_Flag())
			cvd_states_info.cvd_status |= LX_625LINES_DETECTED;
	}

#else
/* Returns Sync Status of CVD thread */
	if(cvd_states_info.cvd_main_sub == LX_CVD_MAIN)
	{
		if(gMainCVD_State >= CVD_STATE_VideoNotReady)
			cvd_states_info.cvd_status = LX_HLOCK|LX_VLOCK;
		else
			cvd_states_info.cvd_status = LX_NO_SIGNAL;
	}
	else {
		if(gCHBCVD_State >= CVD_STATE_VideoNotReady)
			cvd_states_info.cvd_status = LX_HLOCK|LX_VLOCK;
		else
			cvd_states_info.cvd_status = LX_NO_SIGNAL;
	}

#endif
	if (copy_to_user((UINT32*)arg, (UINT32*)&cvd_states_info, sizeof(LX_AFE_CVD_STATES_INFO_T)))
	{
		return -1;
	}

	return 0;
}

#ifdef ENABLE_CVD_TIMER
static void MainCVD_Timer(ULONG data)
{
	MainCVD_Work();
}
#endif

static int MainCVD_Thread(void *data)
{
	while(1)
	{
		if ( lx_chip_rev( ) < LX_CHIP_REV( L9, A0 ) )
		{
			MainCVD_Work();
		OS_MsecSleep(MAINCVD_TIMEOUT);
		}
		else
		{
			CVD_Task_3cdetect();
			OS_MsecSleep(L9_CVD_TIMEOUT);
		}

		if(gForce_thread_sleep > 0)
		{
			AFE_PRINT("User force sleep CVD Thread\n");
			gForce_thread_sleep = 0;
			gCVD_thread_running = 0;
			interruptible_sleep_on(&WaitQueue_CVD);
			gCVD_thread_running = 1;
			//gWait_return = wait_event_interruptible(WaitQueue_CVD, gForce_thread_sleep>0);
		}

	}

	return 0;
}


int CVD_Enable_Status_Fld_Print(BOOLEAN  bEnable)
{
	CVD_DEBUG("%s Entered with [%d]\n",__func__, bEnable);

	if(bEnable)
		gEnable_Status_Fld_Flag = 1;
	else
		gEnable_Status_Fld_Flag = 0;

	return 0;
}

int CVD_Enable_3DCOMB_State_Print(BOOLEAN bEnable)
{
	CVD_DEBUG("%s Entered with [%d]\n",__func__, bEnable);
	if(bEnable)
		gEnable_Monitor_3DCOMB_State = 1;
	else
		gEnable_Monitor_3DCOMB_State = 0;

	return 0;
}

static int	MainCVD_Work(void)
{
//	static UINT8	cvd_fc_flag;
	static UINT32	cvd_cordic_freq;
	static UINT8	No_Signal_Flag, HV_Lock_Flag, Vline_625_Flag, Pal_Flag, Chromalock_Flag, Secam_Flag, status_noise, NoBurst_Flag;
	static UINT8	No_Signal_Count = 0;

	static UINT8	 MainCVD_No_Signal = 0;
	static UINT8	 MainCVD_Prev_Written_Color_System = 0;
//	static UINT8 MainCVD_State_Transition_Count=0;
	//AFE_PRINT("In MainCVD Work\n");
	//CVD_DEBUG("MainCVD State = [%d]\n ", Current_State);

	/* Read Status Registers */

	//110414 No_Signal_Flag is not working well
	No_Signal_Flag = CVD_Get_No_Signal_Flag();
	HV_Lock_Flag = (CVD_Get_HLock_Flag())||(CVD_Get_VLock_Flag());
	Vline_625_Flag = CVD_Get_Vline_625_Flag();
	Pal_Flag = CVD_Get_PAL_Flag();
	Secam_Flag = CVD_Get_SECAM_Flag();
	Chromalock_Flag = CVD_Get_Chromalock_Flag();
	status_noise = CVD_Get_Noise_Status();
	NoBurst_Flag = CVD_Get_NoBurst_Flag();
//	cvd_fc_flag = CVD_Get_FC_Flag(LX_CVD_MAIN);
	cvd_cordic_freq = CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	cvd_cordic_freq = cvd_cordic_freq /5;

	//CVD_DEBUG("NO[%d],HL[%d],VL[%d],V625[%d],PAL[%d],SECAM[%d],ChromaL[%d]\n cordic_freq[%d], noise[%d], , noburst[%d], c_state[%d], g_State[%d]\n ",No_Signal_Flag, CVD_Get_HLock_Flag(), CVD_Get_VLock_Flag(), Vline_625_Flag, Pal_Flag, Secam_Flag, Chromalock_Flag, cvd_cordic_freq, status_noise, NoBurst_Flag, Current_State, gMainCVD_State);

	//110414 No_Signal_Flag is not working well
	No_Signal_Flag = 0;

	switch(Current_State)
	{
		case CVD_STATE_VideoNotReady:
#ifdef CVD_CH42_SKIP_WORKAROUND
			//CVD_ClampAGC_OnOff(0); // agc off on No signal for ch42 no signal workaround
				if ( lx_chip_rev( ) < LX_CHIP_REV( L9, A0 ) )
					CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
				else
				{
					//Disable Clampagc reset workaround (Clamp AGC problem is fixed by disabling x2 LPF setting at Set_AAF_Params)
					//CVD_Reset_Clampagc();
					//CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
				}
#endif
			// dws 0809 for 42ch no signal problem
			if(No_Signal_Count > 3) // Remain No Signal for 2 seconds
			{
			// dws 0906 for color kill on no signal
				CVD_Force_Color_Kill(1);
				No_Signal_Count = 0;
			}
			else
				No_Signal_Count++;
				MainCVD_No_Signal = 1;
#if 0
			if ( HV_Lock_Flag /* || !No_Signal_Flag */)	// HV sync detected
				Next_State = CVD_STATE_VideoReadyToDetect;
			else
				Next_State = CVD_STATE_VideoNotReady;

			break;
#endif

		case CVD_STATE_VideoReadyToDetect:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if(gNTSC_ONLY_Main) // for CVD_Set_NTSC_Only
				Next_State = CVD_STATE_NTSC;

			else if ( Vline_625_Flag ) //Vertical 625 Line
				Next_State = CVD_STATE_PALi;
			else
				Next_State = CVD_STATE_NTSC;

			break;

		case CVD_STATE_NTSC:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if(gNTSC_ONLY_Main) // for CVD_Set_NTSC_Only
				Next_State = CVD_STATE_NTSC;

			else if(!Vline_625_Flag && NoBurst_Flag)
				Next_State = CVD_STATE_NTSC;

			else if(!Vline_625_Flag && !Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD)) // Remain in NTSC
				Next_State = CVD_STATE_NTSC;

			else if(!Vline_625_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD)) // Transition to PALm
				Next_State = CVD_STATE_PALm;

			else if(!Vline_625_Flag && (cvd_cordic_freq < FC_LESS_THRESHOLD)) // Transition to NTSC443
				Next_State = CVD_STATE_NTSC443;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;
			break;

		case CVD_STATE_NTSC_FORCED:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if(gNTSC_ONLY_Main) // for CVD_Set_NTSC_Only
				Next_State = CVD_STATE_NTSC_FORCED;

			else if(Vline_625_Flag)
				Next_State = CVD_STATE_VideoReadyToDetect;

			else if(gExitFromForcedState > 1)
			{
				gExitFromForcedState = 0;
				Next_State = CVD_STATE_VideoReadyToDetect;
			}

			else if(!Vline_625_Flag && NoBurst_Flag)
				Next_State = CVD_STATE_NTSC_FORCED;

			else if(!Vline_625_Flag && !Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD)) // Remain in NTSC
				Next_State = CVD_STATE_NTSC_FORCED;

			else if(!Vline_625_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD)) // Transition to PALm
				Next_State = CVD_STATE_NTSC_FORCED;

			else if(!Vline_625_Flag && (cvd_cordic_freq < FC_LESS_THRESHOLD)) // Transition to NTSC443
				Next_State = CVD_STATE_NTSC_FORCED;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;
			break;

		case CVD_STATE_NTSC443:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if((!Vline_625_Flag && !Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Remain in NTSC443
				Next_State = CVD_STATE_NTSC443;

			else if(!Vline_625_Flag && Pal_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD)) // Transition to PAL60
				Next_State = CVD_STATE_PAL60;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;

			break;

		case CVD_STATE_PALi:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if(Vline_625_Flag && NoBurst_Flag)
				Next_State = CVD_STATE_PALi;

			else if((Vline_625_Flag && Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Remain in PALi
				Next_State = CVD_STATE_PALi;

			else if((Vline_625_Flag && (cvd_cordic_freq > CVD_FC_FOR_SECAM_LOW) && (cvd_cordic_freq < CVD_FC_FOR_SECAM_HIGH))) // Transition to SECAM
				Next_State = CVD_STATE_SECAM;

			else if((Vline_625_Flag && (cvd_cordic_freq > CVD_FC_FOR_PALCn_LOW))) // Transition to PALCn
				Next_State = CVD_STATE_PALCn;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;

			break;

		case CVD_STATE_PALi_FORCED:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if(!Vline_625_Flag)
				Next_State = CVD_STATE_VideoReadyToDetect;

			else if(gExitFromForcedState > 1)
			{
				gExitFromForcedState = 0;
				Next_State = CVD_STATE_VideoReadyToDetect;
			}

			else if(Vline_625_Flag && NoBurst_Flag)
				Next_State = CVD_STATE_PALi_FORCED;

			else if((Vline_625_Flag && Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Remain in PALi
				Next_State = CVD_STATE_PALi_FORCED;

			else if((Vline_625_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Transition to SECAM
				Next_State = CVD_STATE_PALi_FORCED;

			else if((Vline_625_Flag && (cvd_cordic_freq > FC_MORE_THRESHOLD))) // Transition to PALCn
				Next_State = CVD_STATE_PALi_FORCED;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;

			break;

		case CVD_STATE_PALm:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if((!Vline_625_Flag && Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Remain in PALm
				Next_State = CVD_STATE_PALm;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;

			break;

		case CVD_STATE_PALCn:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if((Vline_625_Flag && Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Remain in PALCn
				Next_State = CVD_STATE_PALCn;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;

			break;
		case CVD_STATE_PAL60:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if((!Vline_625_Flag && Pal_Flag && Chromalock_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Remain in PAL60
				Next_State = CVD_STATE_PAL60;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;

			break;

		case CVD_STATE_SECAM:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			else if((Vline_625_Flag && Secam_Flag && (cvd_cordic_freq <= FC_MORE_THRESHOLD) && (cvd_cordic_freq >= FC_LESS_THRESHOLD))) // Remain in SECAM
				Next_State = CVD_STATE_SECAM;

			else if((Vline_625_Flag && (cvd_cordic_freq < FC_LESS_THRESHOLD))) // Transition to PALi
				Next_State = CVD_STATE_PALi;
			else
				Next_State = CVD_STATE_VideoReadyToDetect;

			break;

		default:
			break;
	}

	if((Next_State != CVD_STATE_VideoNotReady) && (Next_State != CVD_STATE_VideoReadyToDetect))
	{		// Sync Lock Detected
		No_Signal_Count = 0;
		// Continuous Write on ckill register when signal locked state.
//		CVD_Force_Color_Kill(0);
	}

	if(Next_State != Current_State) //State Change !!!!!
	{
		//Current_State = Next_State;

		CVD_DEBUG("Main CVD State Change to [%d]\n", Next_State);
		CVD_DEBUG("NO[%d],HL[%d],VL[%d],V625[%d],PAL[%d],SECAM[%d],ChromaL[%d]\n cordic_freq[%d], noise[%d], , noburst[%d], c_state[%d], g_State[%d]\n ",No_Signal_Flag, CVD_Get_HLock_Flag(), CVD_Get_VLock_Flag(), Vline_625_Flag, Pal_Flag, Secam_Flag, Chromalock_Flag, cvd_cordic_freq, status_noise, NoBurst_Flag, Current_State, gMainCVD_State);

		//if((Next_State == CVD_STATE_NTSC) ||(Next_State == CVD_STATE_PALi))
		// Sync Detected !!!
		if(Next_State < CVD_STATE_VideoNotReady)
		{

			// If any color system is detected, disable color kill.
			CVD_Force_Color_Kill(0);
//#ifdef ENABLE_CVD_STATE_TRANSITION_LIMIT
			if( ENABLE_CVD_STATE_TRANSITION_LIMIT )
			{
				gCVD_State_Transition_Count++;
				if(gCVD_State_Transition_Count > CVD_STATE_TRANSITION_MAX)
				{
					if(CVD_Get_Vline_625_Flag() > 0) // At 625 line system , Force system to PALi
					{
						Next_State = CVD_STATE_PALi_FORCED;
						Set_State = CVD_STATE_PALi;
					}
					else
					{
						Next_State = CVD_STATE_NTSC_FORCED;// At 525 line system , Force system to NTSC
						Set_State = CVD_STATE_NTSC;
					}

					AFE_PRINT("FORCE CVD Register to system [%d]\n", Set_State);

					CVD_Program_Color_System_Main_Multi(&gSetColorSystemMain[Set_State]);
					MainCVD_Prev_Written_Color_System = Next_State;

					//CVD_Force_Color_Kill(0);
#ifdef CVD_CH42_SKIP_WORKAROUND
					CVD_Clamp_Current_Control(0);	// 1118 change clamp current to zero
#endif
					gPrevious_Main_System = Next_State;
					Current_State = Next_State;

					//#ifdef ENABLE_CVD_INTERRUPT_CONTROL
					if( ENABLE_CVD_INTERRUPT_CONTROL )
					{
#ifdef ENABLE_CVD_THREAD
						//spin_lock(&cvd_lock);

						if(gCVD_thread_running > 0)		// Thread Running.
						{
							//spin_unlock(&cvd_lock);
							CVD_DEBUG("CVD Main : FORCE Stop Thread[%d]\n", Next_State);
							gCVD_thread_running = 0;
							interruptible_sleep_on(&WaitQueue_CVD);
						//	gWait_return = wait_event_interruptible(WaitQueue_CVD, 0);
							CVD_DEBUG("CVD Main : Wakeup Thread[%d]\n", gWait_return);
							gCVD_thread_running = 1;

							//This is for CVD can be Reseted after Interrupt Occur
							//Current_State = CVD_STATE_VideoNotReady;
							MainCVD_Same_State_Count = 0;
						}
						else
						//	spin_unlock(&cvd_lock);

						MainCVD_No_Signal = 0;
#endif
#ifdef ENABLE_CVD_TIMER
						spin_lock(&cvd_lock);

						if((main_timer_state == 1))
						{
							main_timer_state = 0 ;
							OS_StopTimer( &main_timer );
							spin_unlock(&cvd_lock);
							AFE_PRINT("CVD Main : FORCE Stop Timer[%d]\n", Next_State);

							//This is for CVD can be Reseted after Interrupt Occur
							//Current_State = CVD_STATE_VideoNotReady;
							MainCVD_Same_State_Count = 0;
						}
						else
							spin_unlock(&cvd_lock);
						MainCVD_No_Signal = 0;
						//#endif
#endif
					}
					gCVD_State_Transition_Count = 0;
					return 0;
				}
			}
//#endif
			//SW reset is needed here???
			//		if(gPrevious_Main_System != Next_State) // IF from no signal, do not re-setting cvd register to the same value
			if((gPrevious_Main_System != Next_State)|(Current_State==CVD_STATE_VideoNotReady)) // IF from VideoReadyToDetect, do not re-setting cvd register to the same value
			{
				if( MainCVD_Prev_Written_Color_System != Next_State)
				{
					AFE_PRINT("Setting CVD Register to system [%d]\n", Next_State);

					if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
					{
						//AFE_PRINT("HDCT/CDCT Reset 1\n");
						CVD_Reset_hdct(1);
						CVD_Reset_cdct(1);

						//by dws : remove mdelay
						//mdelay(10);
						OS_MsecSleep(10);
					}

					CVD_Program_Color_System_Main_Multi(&gSetColorSystemMain[Next_State]);
					MainCVD_Prev_Written_Color_System = Next_State;

					if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
					{
						CVD_Reset_hdct(0);
						CVD_Reset_cdct(0);
					}
				}
				else
					CVD_DEBUG("Same system [%d] again\n", Next_State);

			//	CVD_Force_Color_Kill(0);
#ifdef CVD_CH42_SKIP_WORKAROUND
					CVD_Clamp_Current_Control(0);	// 1118 change clamp current to zero
#endif
			}
			//Program_Color_System_Main_only_system_set(gSetColorSystemMain[Next_State]);
			//SW reset is needed here???
			//CVD_SW_Reset(LX_CVD_MAIN);
			gPrevious_Main_System = Next_State;
			//gMainCVD_State = Next_State;
			Current_State = Next_State;
		}
		else if(Next_State != CVD_STATE_VideoReadyToDetect)  // No Signal or color system change
		{
			Current_State = CVD_STATE_VideoNotReady;
#ifdef CVD_CH42_SKIP_WORKAROUND
				if ( lx_chip_rev( ) < LX_CHIP_REV( L9, A0 ) )
					CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
				else
				{
					//Disable Clampagc reset workaround (Clamp AGC problem is fixed by disabling x2 LPF setting at Set_AAF_Params)
					//CVD_Reset_Clampagc();
					CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
				}
#endif
		}
		else
		{
			// From Ok : SW workaround for 3D comb operation 2010/06/04
			Current_State = CVD_STATE_VideoReadyToDetect;
		}
		MainCVD_Same_State_Count = 0;

		//1126 for fast standard detection
		gMainCVD_State = Next_State;
	}
	else // same state again
	{
//#ifdef ENABLE_CVD_STATE_TRANSITION_LIMIT
		if( ENABLE_CVD_STATE_TRANSITION_LIMIT )
		{
			gCVD_State_Transition_Count = 0;
			//#endif
		}

		gMainCVD_State = Next_State;

		if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
		{
#if ENABLE_CVD_INTERRUPT_CONTROL
			//spin_lock(&cvd_lock);

			if(MainCVD_Same_State_Count > 4)
			{

#ifdef ENABLE_CVD_THREAD
				CVD_DEBUG("MainCVD_Same_State_Count=[%d], gCVD_thread_running = [%d]\n", MainCVD_Same_State_Count, gCVD_thread_running);
				if(gCVD_thread_running > 0 )
				{
					CVD_DEBUG("CVD Main : Stop Thread[%d]\n", Next_State);
					gCVD_thread_running = 0;
					interruptible_sleep_on(&WaitQueue_CVD);
				//	gWait_return = wait_event_interruptible(WaitQueue_CVD, 0);
					CVD_DEBUG("CVD Main : Wakeup Thread[%d]\n", gWait_return);
					gCVD_thread_running = 1;

					MainCVD_Same_State_Count = 0;
				}
#endif
#ifdef ENABLE_CVD_TIMER
				if((main_timer_state == 1))
				{
					main_timer_state = 0 ;
					OS_StopTimer( &main_timer );
			//		AFE_PRINT("CVD Main : Stop Timer[%d]\n", Next_State);

					//This is for CVD can be Reseted after Interrupt Occur
					//Current_State = CVD_STATE_VideoNotReady;
					MainCVD_Same_State_Count = 0;
				}
#endif

				MainCVD_No_Signal = 0;
//#ifdef ENABLE_CVD_STATE_TRANSITION_LIMIT
				if( ENABLE_CVD_STATE_TRANSITION_LIMIT )
					gCVD_State_Transition_Count = 0;
//#endif
			}
			else
				MainCVD_Same_State_Count++;
			//spin_unlock(&cvd_lock);
#endif
		}

	}

	return 0;
}

static int CVD_Set_Color_System_3CS(CVD_STATE_T cvd_next_state)
{
	switch(cvd_next_state)
	{
		case CVD_STATE_NTSC:
			gCVD_Status_3CS.cvd_color_system = CVD_STATE_NTSC;
			gCVD_Status_3CS.cs0_color_system = CVD_STATE_PAL60;
			gCVD_Status_3CS.cs1_color_system = CVD_STATE_PALm;
			break;

		case CVD_STATE_NTSC443:
			gCVD_Status_3CS.cvd_color_system = CVD_STATE_NTSC443;
			gCVD_Status_3CS.cs0_color_system = CVD_STATE_NTSC;
			gCVD_Status_3CS.cs1_color_system = CVD_STATE_PALm;
			break;

		case CVD_STATE_PALm:
			gCVD_Status_3CS.cvd_color_system = CVD_STATE_PALm;
			gCVD_Status_3CS.cs0_color_system = CVD_STATE_PAL60;
			gCVD_Status_3CS.cs1_color_system = CVD_STATE_NTSC;
			break;

		case CVD_STATE_PAL60:
			gCVD_Status_3CS.cvd_color_system = CVD_STATE_PAL60;
			gCVD_Status_3CS.cs0_color_system = CVD_STATE_NTSC;
			gCVD_Status_3CS.cs1_color_system = CVD_STATE_PALm;
			break;

		case CVD_STATE_PALi:
			gCVD_Status_3CS.cvd_color_system = CVD_STATE_PALi;
			gCVD_Status_3CS.cs0_color_system = CVD_STATE_SECAM;
			gCVD_Status_3CS.cs1_color_system = CVD_STATE_PALCn;
			break;

		case CVD_STATE_PALCn:
			gCVD_Status_3CS.cvd_color_system = CVD_STATE_PALCn;
			gCVD_Status_3CS.cs0_color_system = CVD_STATE_SECAM;
			gCVD_Status_3CS.cs1_color_system = CVD_STATE_PALi;
			break;

		case CVD_STATE_SECAM:
			gCVD_Status_3CS.cvd_color_system = CVD_STATE_SECAM;
			gCVD_Status_3CS.cs0_color_system = CVD_STATE_PALi;
			gCVD_Status_3CS.cs1_color_system = CVD_STATE_PALCn;
			break;

		default:
			AFE_PRINT("ERROR INVALID COLOR SYSTEM!!!\n");
			break;
	}

	CVD_DEBUG("Set Color System : CVD[%d], CS0[%d], CS1[%d]\n", gCVD_Status_3CS.cvd_color_system, gCVD_Status_3CS.cs0_color_system, gCVD_Status_3CS.cs1_color_system);
	CVD_Program_Color_System_Main_Multi(&gSetColorSystemMain[gCVD_Status_3CS.cvd_color_system]);


	//won.hur : added chip selection for L9B0 migration
	if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0 ))
	{
		CVD_L9Bx_Program_Color_System_CS(CVD_SEL_CS_CS0, &gSetColorSystemMain[gCVD_Status_3CS.cs0_color_system]);
		CVD_L9Bx_Program_Color_System_CS(CVD_SEL_CS_CS1, &gSetColorSystemMain[gCVD_Status_3CS.cs1_color_system]);

		//111221 by kim.min for better color standard detection performance
		CVD_L9Bx_swrst_CS( CVD_SEL_CS_CS0);
		CVD_L9Bx_swrst_CS( CVD_SEL_CS_CS1);

	}
	else if ( lx_chip_rev() >= LX_CHIP_REV( L9, A0 ))
	{
		CVD_L9Ax_Program_Color_System_CS(CVD_SEL_CS_CS0, &gSetColorSystemMain[gCVD_Status_3CS.cs0_color_system]);
		CVD_L9Ax_Program_Color_System_CS(CVD_SEL_CS_CS1, &gSetColorSystemMain[gCVD_Status_3CS.cs1_color_system]);
	}

	return 0;
}

static CVD_STATE_T CVD_Check_Color_System_Support(LX_AFE_CVD_SUPPORT_COLOR_SYSTEM_T	forced_color_system)
{
	CVD_STATE_T	system_ret = CVD_STATE_Default;

	// For user force color system
	if(forced_color_system == LX_COLOR_SYSTEM_NTSC_M)		// For Kor/US
		system_ret = CVD_STATE_NTSC;
	else if(forced_color_system == LX_COLOR_SYSTEM_PAL_G)
		system_ret = CVD_STATE_PALi;
	else if(forced_color_system == LX_COLOR_SYSTEM_PAL_NC)
		system_ret = CVD_STATE_PALCn;
	else if(forced_color_system == LX_COLOR_SYSTEM_PAL_M)
		system_ret = CVD_STATE_PALm;
	else if(forced_color_system == LX_COLOR_SYSTEM_SECAM)
		system_ret = CVD_STATE_SECAM;
	else if(forced_color_system == LX_COLOR_SYSTEM_NTSC_443)
		system_ret = CVD_STATE_NTSC443;
	else if(forced_color_system == LX_COLOR_SYSTEM_PAL_60)
		system_ret = CVD_STATE_PAL60;

	return system_ret;
}

static int	CVD_Task_3cdetect(void)
{
#ifdef L9_ADAPTIVE_BURST_GATE_END_MODE
	static UINT32 gVariance_Status_Cdto_Inc = 0;
	static int	  gAdaptive_Burst_Gate_Counter = 0;
	int	Adaptive_Burst_Gate_Enable = 0;
	static int Prev_Adaptive_Burst_Gate_Enable = 0;
	static UINT32 gAccumulated_Cordic_Freq_Value = 0;
#endif
	static UINT8	No_Signal_Flag, HV_Lock_Flag;
	static UINT8	No_Signal_Count = 0;

	static UINT8	 MainCVD_No_Signal = 0;
	static UINT8	 MainCVD_Prev_Written_Color_System = 0;
	static BOOLEAN	 MainCVD_Is_Prev_RF_Input = FALSE;

	//	static UINT8 MainCVD_State_Transition_Count=0;
	//AFE_PRINT("In MainCVD Work\n");
	//CVD_DEBUG("MainCVD State = [%d]\n ", Current_State);
	static	UINT32 Same_State_Count = 0;
	static UINT32	cvd_noise_sum	= 0;
	unsigned int avg_noise_level;
	static int stable_state2_count = 0;

	static UINT8 burst_mag_prev = 0, burst_mag_next = 0;
	static UINT32 burst_mag_array[10];
	UINT32 burst_mag_sum, burst_mag_avg;
	static UINT32 prev_burst_mag_avg ;
	static UINT32 cagc_target ;
	static UINT32 cagc_written ;

	static UINT16	status_noise_array[8] = {0,};
	static UINT16	status_agc_gain_array[8] = {0,};

	//LX_AFE_CVD_PQ_MODE_T	cvd_pe_color_system;
	int loop, avg_tmp = 0;
	int	agc_gain_tmp = 0;
#ifdef L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC
	static UINT8	vcr_detect_count;
	static int 		gHstateMaxCounter = 0;
#endif
	CVD_STATE_T	check_supporting_color_system;

	/* Read Status CVD Registers */
	gCVD_Status_3CS.no_signal_flag = CVD_Get_No_Signal_Flag();
	gCVD_Status_3CS.h_lock_flag = CVD_Get_HLock_Flag();
	gCVD_Status_3CS.v_lock_flag = CVD_Get_VLock_Flag();
	// ORing hlock & vlock makes frequent state transition to NO_SIGNAL_STATE on noisy signal.
	HV_Lock_Flag = gCVD_Status_3CS.h_lock_flag && gCVD_Status_3CS.v_lock_flag;
	// So ANDing hlock & vlock will be used for cheking cvd lock state.
	//HV_Lock_Flag = gCVD_Status_3CS.h_lock_flag || gCVD_Status_3CS.v_lock_flag;
	gCVD_Status_3CS.vline_625_flag = CVD_Get_Vline_625_Flag();
	gCVD_Status_3CS.cvd_pal_flag = CVD_Get_PAL_Flag();
	gCVD_Status_3CS.cvd_secam_flag = CVD_Get_SECAM_Flag();//gogosing
	gCVD_Status_3CS.cvd_chromalock_flag = CVD_Get_Chromalock_Flag();
	gCVD_Status_3CS.status_noise = CVD_Get_Noise_Status();
	gCVD_Status_3CS.no_burst_flag = CVD_Get_NoBurst_Flag();
	gCVD_Status_3CS.cvd_fc_flag = CVD_Get_FC_Flag(LX_CVD_MAIN);
	gCVD_Status_3CS.cvd_cordic_freq = CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	gCVD_Status_3CS.cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	gCVD_Status_3CS.cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	gCVD_Status_3CS.cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	gCVD_Status_3CS.cvd_cordic_freq = (gCVD_Status_3CS.cvd_cordic_freq>>2);
	//gCVD_Status_3CS.cvd_cordic_freq += CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	//gCVD_Status_3CS.cvd_cordic_freq = gCVD_Status_3CS.cvd_cordic_freq /5;
	if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0 )) {
		 gCVD_Status_3CS.status_agc_gain = CVD_L9Bx_Get_Status_AGC_Gain();
		 gCVD_Status_3CS.hnon_standard_flag = CVD_L9Bx_Get_HNon_Standard_Flag();
		 gCVD_Status_3CS.vnon_standard_flag = CVD_L9Bx_Get_VNon_Standard_Flag();
	}
	else	{
		 gCVD_Status_3CS.status_agc_gain = 0;
		 gCVD_Status_3CS.hnon_standard_flag = 0;
		 gCVD_Status_3CS.vnon_standard_flag = 0;
	}
	/*
	if(gAutoProgramming == FALSE)
		CVD_AGC_Bypass_Function((UINT32)gCVD_Status_3CS.status_noise, (UINT32)gCVD_Status_3CS.h_lock_flag, (UINT32)gCVD_Status_3CS.no_signal_flag );
		*/

	// test only 120217
/*
	if( ( !gCVD_Status_3CS.v_lock_flag ) && (!gCVD_Status_3CS.h_lock_flag ) && (gCVD_Status_3CS.no_signal_flag) ) {
		CVD_DEBUG("H/V Unlock & No signal !!! \n");
#ifdef CVD_CH42_SKIP_WORKAROUND
//				CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
#endif
	}
	*/

	for(loop = 7;loop > 0;loop--) {
		status_noise_array[loop] = status_noise_array[loop-1] ;
		avg_tmp += status_noise_array[loop];
	}
	status_noise_array[0] = gCVD_Status_3CS.status_noise;
	avg_tmp += status_noise_array[0];

	gCVD_Status_3CS.avg_status_noise = avg_tmp >> 3;

	for(loop = 7;loop > 0;loop--) {
		status_agc_gain_array[loop] = status_agc_gain_array[loop-1] ;
		agc_gain_tmp += status_agc_gain_array[loop];
	}
	status_agc_gain_array[0] = gCVD_Status_3CS.status_agc_gain;
	agc_gain_tmp += status_agc_gain_array[0];

	gCVD_Status_3CS.avg_status_agc_gain = agc_gain_tmp >> 3;

	if ( lx_chip_rev() >= LX_CHIP_REV( L9, A0 ))
	{
		gCVD_Status_3CS.cs0_pal_flag = CVD_Get_PAL_Flag_CS0();
		gCVD_Status_3CS.cs0_secam_flag = CVD_Get_SECAM_Flag_CS0();//gogosing
		gCVD_Status_3CS.cs0_chromalock_flag = CVD_Get_Chromalock_Flag_CS0();
		gCVD_Status_3CS.cs0_fc_flag = CVD_Get_FC_Flag_CS0();
		gCVD_Status_3CS.cs0_cordic_freq = CVD_Get_Cordic_Freq_CS0();
		gCVD_Status_3CS.cs0_cordic_freq += CVD_Get_Cordic_Freq_CS0();
		gCVD_Status_3CS.cs0_cordic_freq += CVD_Get_Cordic_Freq_CS0();
		gCVD_Status_3CS.cs0_cordic_freq += CVD_Get_Cordic_Freq_CS0();
		gCVD_Status_3CS.cs0_cordic_freq += CVD_Get_Cordic_Freq_CS0();
		gCVD_Status_3CS.cs0_cordic_freq = gCVD_Status_3CS.cs0_cordic_freq /5;

		gCVD_Status_3CS.cs1_pal_flag = CVD_Get_PAL_Flag_CS1();
		gCVD_Status_3CS.cs1_secam_flag = CVD_Get_SECAM_Flag_CS1();//gogosing
		gCVD_Status_3CS.cs1_chromalock_flag = CVD_Get_Chromalock_Flag_CS1();
		gCVD_Status_3CS.cs1_fc_flag = CVD_Get_FC_Flag_CS1();
		gCVD_Status_3CS.cs1_cordic_freq = CVD_Get_Cordic_Freq_CS1();
		gCVD_Status_3CS.cs1_cordic_freq += CVD_Get_Cordic_Freq_CS1();
		gCVD_Status_3CS.cs1_cordic_freq += CVD_Get_Cordic_Freq_CS1();
		gCVD_Status_3CS.cs1_cordic_freq += CVD_Get_Cordic_Freq_CS1();
		//gCVD_Status_3CS.cs1_cordic_freq += CVD_Get_Cordic_Freq_CS1();
		gCVD_Status_3CS.cs1_cordic_freq = (gCVD_Status_3CS.cs1_cordic_freq>>2);
		//gCVD_Status_3CS.cs1_cordic_freq = gCVD_Status_3CS.cs1_cordic_freq /5;

	}
	//CVD_DEBUG("NO[%d],HL[%d],VL[%d],V625[%d],PAL[%d],SECAM[%d],ChromaL[%d]\n cordic_freq[%d], noise[%d], , noburst[%d], c_state[%d], g_State[%d]\n ",No_Signal_Flag, CVD_Get_HLock_Flag(), CVD_Get_VLock_Flag(), Vline_625_Flag, Pal_Flag, Secam_Flag, Chromalock_Flag, cvd_cordic_freq, status_noise, NoBurst_Flag, Current_State, gMainCVD_State);

	//110414 No_Signal_Flag is not working well
	No_Signal_Flag = 0;

	switch(Current_State)
	{
		case CVD_STATE_VideoNotReady:
#ifdef CVD_CH42_SKIP_WORKAROUND
			//CVD_ClampAGC_OnOff(0); // agc off on No signal for ch42 no signal workaround
			if ( lx_chip_rev( ) < LX_CHIP_REV( L9, A0 ) )
				CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
			else
			{
				//Disable Clampagc reset workaround (Clamp AGC problem is fixed by disabling x2 LPF setting at Set_AAF_Params)
				//CVD_Reset_Clampagc();
				//CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
			}
#endif
			// dws 0809 for 42ch no signal problem
			if(No_Signal_Count > 3) // Remain No Signal for 2 seconds
			{
				// dws 0906 for color kill on no signal
				CVD_Force_Color_Kill(1);
				No_Signal_Count = 0;
			}
			else
				No_Signal_Count++;
			MainCVD_No_Signal = 1;
#if 0
			if ( HV_Lock_Flag /* || !No_Signal_Flag */)	// HV sync detected
				Next_State = CVD_STATE_VideoReadyToDetect;
			else
				Next_State = CVD_STATE_VideoNotReady;

			break;
#endif

		case CVD_STATE_VideoReadyToDetect:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;
			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;
			else if(g_CVD_Color_System_Support == (LX_COLOR_SYSTEM_PAL_M | LX_COLOR_SYSTEM_NTSC_M | LX_COLOR_SYSTEM_PAL_NC) )	// For Brazil
			{
				if ( gCVD_Status_3CS.vline_625_flag ) //Vertical 625 Line
					Next_State = CVD_STATE_PALCn;
				else
					Next_State = CVD_STATE_PALm;
			}
			else // for Multi System
			{
				if ( gCVD_Status_3CS.vline_625_flag ) //Vertical 625 Line
				{
					if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)
						Next_State = CVD_STATE_PALi;
					else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)
						Next_State = CVD_STATE_PALCn;
					else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)
						Next_State = CVD_STATE_SECAM;
				}
				else
				{
					if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)
						Next_State = CVD_STATE_NTSC;
					else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)
						Next_State = CVD_STATE_PALm;
					else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)
						Next_State = CVD_STATE_NTSC443;
					else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)
						Next_State = CVD_STATE_PAL60;
				}
			}

			break;

		case CVD_STATE_NTSC:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;
#ifdef CVD_USE_NO_BURST_FLAG
			else if(!gCVD_Status_3CS.vline_625_flag && gCVD_Status_3CS.no_burst_flag)	// For No burst signal, with 525 line system remain in NTSC
				Next_State = CVD_STATE_NTSC;
#endif
			else if(gCVD_Status_3CS.vline_625_flag)
			{
				if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)
					Next_State = CVD_STATE_PALi;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)
					Next_State = CVD_STATE_PALCn;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)
					Next_State = CVD_STATE_SECAM;
			}
			// 111028 wonsik.do : Check PAL-M before NTSC-M to fix slow color system transition to PAL-M (both NTSC-M & PAL-M locked for long time)
			else if(gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs1_chromalock_flag && (gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)) // Transition to PALm
				Next_State = CVD_STATE_PALm;

			else if(!gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)) // Remain in NTSC
				Next_State = CVD_STATE_NTSC;

			else if(gCVD_Status_3CS.cs0_pal_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)) // Transition to PAL60
				Next_State = CVD_STATE_PAL60;

			else if(!gCVD_Status_3CS.cvd_pal_flag && !gCVD_Status_3CS.cs0_pal_flag && !gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq < FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq < FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)) // Transition to NTSC443
				Next_State = CVD_STATE_NTSC443;

			else
				Next_State = CVD_STATE_NTSC;
			break;

		case CVD_STATE_NTSC443:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;

#ifdef CVD_USE_NO_BURST_FLAG
			else if(!gCVD_Status_3CS.vline_625_flag && gCVD_Status_3CS.no_burst_flag)	// For no burst signal
				Next_State = CVD_STATE_NTSC;
#endif

			else if(gCVD_Status_3CS.vline_625_flag)
			{
				if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)
					Next_State = CVD_STATE_PALi;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)
					Next_State = CVD_STATE_PALCn;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)
					Next_State = CVD_STATE_SECAM;
			}

			else if(!gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)) // Remain in NTSC443
				Next_State = CVD_STATE_NTSC443;

			else if(!gCVD_Status_3CS.cs0_pal_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)) // Transition to NTSC
				Next_State = CVD_STATE_PAL60;

			else if(gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs1_chromalock_flag && (gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)) // Transition to PALm
				Next_State = CVD_STATE_PALm;

			else if(gCVD_Status_3CS.cvd_pal_flag && !gCVD_Status_3CS.cs0_pal_flag && !gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq < FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq < FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)) // Transition to PAL60
				Next_State = CVD_STATE_PAL60;

			else
				Next_State = CVD_STATE_NTSC443;
			break;

		case CVD_STATE_PALi:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;

#ifdef CVD_USE_NO_BURST_FLAG
			else if(gCVD_Status_3CS.vline_625_flag && gCVD_Status_3CS.no_burst_flag)	// no burst 625 line signal
			{
				Next_State = CVD_STATE_PALi;
			}
#endif

			else if(!gCVD_Status_3CS.vline_625_flag)	// Transition to 525 line system
			{
				if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)
					Next_State = CVD_STATE_NTSC;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)
					Next_State = CVD_STATE_PALm;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)
					Next_State = CVD_STATE_NTSC443;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)
					Next_State = CVD_STATE_PAL60;
			}

			else if(gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)) // Remain in PALi
				Next_State = CVD_STATE_PALi;

			//111028 added cvd choromalock flag to AND with cvd pal flag to determine SECAM detection
			//110809 added cvd pal flag
			else if(!(gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag) && !gCVD_Status_3CS.cs0_pal_flag && gCVD_Status_3CS.cs0_secam_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)) // Transition to SECAM
				Next_State = CVD_STATE_SECAM;

			else if(gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs1_chromalock_flag && (gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)) // Transition to PALCn
				Next_State = CVD_STATE_PALCn;

#ifdef	SYSTEM_DETECTION_BY_CORDIC_ONLY
			else if((gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)) // Remain in PALi
				Next_State = CVD_STATE_PALi;
#ifdef SYSTEM_DETECTION_BY_CORDIC_CS0_CS1
			else if((gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)) // Transition to SECAM
				Next_State = CVD_STATE_SECAM;
			else if((gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)) // Transition to PALCn
				Next_State = CVD_STATE_PALCn;
#endif
#endif
			else
			{
				Next_State = CVD_STATE_PALi;
			}

			break;

		case CVD_STATE_PALm:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;

#ifdef CVD_USE_NO_BURST_FLAG
			else if(!gCVD_Status_3CS.vline_625_flag && gCVD_Status_3CS.no_burst_flag)	// no burst 525 line
			{
				if(g_CVD_Color_System_Support == (LX_COLOR_SYSTEM_PAL_M | LX_COLOR_SYSTEM_NTSC_M | LX_COLOR_SYSTEM_PAL_NC) )	// For Brazil
					Next_State = CVD_STATE_PALm;
				else
					Next_State = CVD_STATE_NTSC;
			}
#endif

			else if(gCVD_Status_3CS.vline_625_flag)
			{
				if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)
					Next_State = CVD_STATE_PALi;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)
					Next_State = CVD_STATE_PALCn;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)
					Next_State = CVD_STATE_SECAM;
			}

			else if(gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= ( FC_MORE_THRESHOLD + 0x40) ) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)) // Remain in PALm
				Next_State = CVD_STATE_PALm;

			else if(gCVD_Status_3CS.cs0_pal_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)) // Transition to PAL60
				Next_State = CVD_STATE_PAL60;

			else if(!gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs1_chromalock_flag && (gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)) // Transition to NTSC
				Next_State = CVD_STATE_NTSC;

			else if(!gCVD_Status_3CS.cvd_pal_flag && !gCVD_Status_3CS.cs0_pal_flag && !gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq < FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq < FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)) // Transition to NTSC443
				Next_State = CVD_STATE_NTSC443;

			else
				Next_State = CVD_STATE_PALm;

			break;

		case CVD_STATE_PALCn:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;

#ifdef CVD_USE_NO_BURST_FLAG
			else if(gCVD_Status_3CS.vline_625_flag && gCVD_Status_3CS.no_burst_flag)	// no burst 625 line
				Next_State = CVD_STATE_PALi;
#endif

			else if(!gCVD_Status_3CS.vline_625_flag)		// Transition to 525 line systems
			{
				if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)
					Next_State = CVD_STATE_NTSC;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)
					Next_State = CVD_STATE_PALm;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)
					Next_State = CVD_STATE_NTSC443;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)
					Next_State = CVD_STATE_PAL60;
			}

			else if(gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)) // Remain in PALCn
				Next_State = CVD_STATE_PALCn;

			else if(!gCVD_Status_3CS.cs0_pal_flag && gCVD_Status_3CS.cs0_secam_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)) // Transition to SECAM
				Next_State = CVD_STATE_SECAM;

			else if(gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs1_chromalock_flag && (gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)) // Transition to PALi
				Next_State = CVD_STATE_PALi;

#ifdef	SYSTEM_DETECTION_BY_CORDIC_ONLY
			else if((gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)) // Remain in PALCn
				Next_State = CVD_STATE_PALCn;

#ifdef SYSTEM_DETECTION_BY_CORDIC_CS0_CS1
			else if((gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)) // Transition to SECAM
				Next_State = CVD_STATE_SECAM;

			else if((gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)) // Transition to PALi
				Next_State = CVD_STATE_PALi;
#endif
#endif

			else
				Next_State = CVD_STATE_PALCn;

			break;
		case CVD_STATE_PAL60:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;

#ifdef CVD_USE_NO_BURST_FLAG
			else if(!gCVD_Status_3CS.vline_625_flag && gCVD_Status_3CS.no_burst_flag)	// no burst 525 line
				Next_State = CVD_STATE_NTSC;
#endif

			else if(gCVD_Status_3CS.vline_625_flag)
			{
				if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)
					Next_State = CVD_STATE_PALi;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)
					Next_State = CVD_STATE_PALCn;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)
					Next_State = CVD_STATE_SECAM;
			}

			else if(gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)) // Remain in PAL60
				Next_State = CVD_STATE_PAL60;

			else if(!gCVD_Status_3CS.cs0_pal_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)) // Transition to NTSC
				Next_State = CVD_STATE_NTSC;

			else if(gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs1_chromalock_flag && (gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)) // Transition to PALm
				Next_State = CVD_STATE_PALm;

			else if(!gCVD_Status_3CS.cvd_pal_flag && !gCVD_Status_3CS.cs0_pal_flag && !gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq < FC_LESS_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq < FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)) // Transition to NTSC443
				Next_State = CVD_STATE_NTSC443;

			else
				Next_State = CVD_STATE_PAL60;

			break;

		case CVD_STATE_SECAM:

			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				Next_State = CVD_STATE_VideoNotReady;

			// For user force color system
			else if ((check_supporting_color_system = CVD_Check_Color_System_Support(g_CVD_Color_System_Support)) > 0)
				Next_State = check_supporting_color_system;

#ifdef CVD_USE_NO_BURST_FLAG
			else if(gCVD_Status_3CS.vline_625_flag && gCVD_Status_3CS.no_burst_flag)	// no burst 625line
				Next_State = CVD_STATE_PALi;
#endif

			else if(!gCVD_Status_3CS.vline_625_flag)		//Transition to 625line system
			{
				if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_M)
					Next_State = CVD_STATE_NTSC;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_M)
					Next_State = CVD_STATE_PALm;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_NTSC_443)
					Next_State = CVD_STATE_NTSC443;
				else if(g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_60)
					Next_State = CVD_STATE_PAL60;
			}

			else if(!gCVD_Status_3CS.cvd_pal_flag && gCVD_Status_3CS.cvd_secam_flag && gCVD_Status_3CS.cvd_chromalock_flag && (gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)) // Remain in SECAM
				Next_State = CVD_STATE_SECAM;

			else if(gCVD_Status_3CS.cs0_pal_flag && gCVD_Status_3CS.cs0_chromalock_flag && (gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)) // Transition to PALi
				Next_State = CVD_STATE_PALi;

			else if(gCVD_Status_3CS.cs1_pal_flag && gCVD_Status_3CS.cs1_chromalock_flag && (gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)) // Transition to PALCn
				Next_State = CVD_STATE_PALCn;

#ifdef	SYSTEM_DETECTION_BY_CORDIC_ONLY
			else if((gCVD_Status_3CS.cvd_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cvd_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_SECAM)) // Remain in SECAM
				Next_State = CVD_STATE_SECAM;
#ifdef SYSTEM_DETECTION_BY_CORDIC_CS0_CS1
			else if((gCVD_Status_3CS.cs0_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs0_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_G)) // Transition to PALi
				Next_State = CVD_STATE_PALi;

			else if((gCVD_Status_3CS.cs1_cordic_freq <= FC_MORE_THRESHOLD) && (gCVD_Status_3CS.cs1_cordic_freq >= FC_LESS_THRESHOLD) && (g_CVD_Color_System_Support & LX_COLOR_SYSTEM_PAL_NC)) // Transition to PALCn
				Next_State = CVD_STATE_PALCn;
#endif
#endif

			else
				Next_State = CVD_STATE_SECAM;

			break;

		default:
			break;
	}

#ifdef L9_FAST_3DCOMB_WORKAROUND
	if( (gCVD_Status_3CS.hnon_standard_flag) && (gAutoProgramming == FALSE) && (Next_State != CVD_STATE_SECAM) && (Next_State != CVD_STATE_NTSC443)) {	// hnon standard detected  && not in auto tuning && color system is not in SECAM/NTSC443
		CVD_L9Bx_Set_HNon_Standard_Threshold(0);
	}
	else if(Next_State < CVD_STATE_VideoNotReady) {
		CVD_L9Bx_Set_HNon_Standard_Threshold(gSetColorSystemMain[Next_State].reg_hnon_std_threshold);
	}
	else {
//		CVD_DEBUG("HNon_Std not detected in not locked state \n");
	}

#endif
	// #1 	: sync lock detected
	//		: reset "No_Signal_Count"
	if((gCVD_Status_3CS.cvd_next_state != CVD_STATE_VideoNotReady) && (gCVD_Status_3CS.cvd_next_state != CVD_STATE_VideoReadyToDetect))
	{
		No_Signal_Count = 0;
		// Continuous Write on ckill register when signal locked state.
		//		CVD_Force_Color_Kill(0);
	}

	// #2 	: State Change Detected !!!!!
	//		:
	if( (Next_State != Current_State) || (gPortChangeDetected == TRUE))
	{
		Same_State_Count = 0;
		cvd_noise_sum = 0;	//clear noise sum value
		gPortChangeDetected = FALSE;

		//Current_State = Next_State;
		CVD_DEBUG("CVD State Change to [%d]\n", Next_State);
		CVD_DEBUG("NO[%d],HL[%d],VL[%d],V625[%d],PAL[%d],SECAM[%d],ChromaL[%d]\n cordic_freq[%d], noise[%d], , noburst[%d], c_state[%d], g_State[%d]\n ",gCVD_Status_3CS.no_signal_flag, gCVD_Status_3CS.h_lock_flag, gCVD_Status_3CS.v_lock_flag, gCVD_Status_3CS.vline_625_flag, gCVD_Status_3CS.cvd_pal_flag, gCVD_Status_3CS.cvd_secam_flag, gCVD_Status_3CS.cvd_chromalock_flag, gCVD_Status_3CS.cvd_cordic_freq, gCVD_Status_3CS.status_noise, gCVD_Status_3CS.no_burst_flag, Current_State, gMainCVD_State);
		CVD_DEBUG("CS0:PAL[%d],SECAM[%d],ChromaL[%d] cordic_freq[%d]\n ", gCVD_Status_3CS.cs0_pal_flag, gCVD_Status_3CS.cs0_secam_flag, gCVD_Status_3CS.cs0_chromalock_flag, gCVD_Status_3CS.cs0_cordic_freq);
		CVD_DEBUG("CS1:PAL[%d],SECAM[%d],ChromaL[%d] cordic_freq[%d]\n ", gCVD_Status_3CS.cs1_pal_flag, gCVD_Status_3CS.cs1_secam_flag, gCVD_Status_3CS.cs1_chromalock_flag, gCVD_Status_3CS.cs1_cordic_freq);

		// For proper color system detection, at first set cstate value to default.
		CVD_Program_Color_System_PreJob(&gSetColorSystemMain[CVD_STATE_Default]);

		CVD_NoBurst_CKill_Control(&gCVD_Status_3CS);

		for(loop = 0;loop < 10;loop++) {
			burst_mag_array[loop] = 0xFFFF ;
			burst_mag_sum += burst_mag_array[loop];
		}

		burst_mag_prev=0;
		burst_mag_next=0;
		prev_burst_mag_avg = 0xbc0;
		cagc_target = 0xbc0;

	// #2.1 	: State Change & Previously Locked State is NTSC
		if( gPrevious_Main_System == CVD_STATE_NTSC)
		{
			// Restore cagc value to default
			CVD_DEBUG("Cagc value to default \n");
			CVD_Set_CVD_CAGC(CVD_BURST_MAG_STATE_BIG,&gSetColorSystemMain[gPrevious_Main_System], gPrevious_Main_System);

			//120131
			/*
			//if saturation value is adjusted, restoring initial value is needed (for same color system detection case)
			if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0) ) {
			CVD_L9Bx_Set_CVD_Saturation_Value(gSetColorSystemMain[gPrevious_Main_System].reg_saturation);
			}
			 */

#ifdef L9_WA_FOR_EBS_STABLE_AGC
			// restore default values of reg_agc_half_en to '0', and reg_nstd_hysis to '7' for EBS field stream support.
			// restore default values of reg_agc_peak_cntl to '1', and reg_dcrestore_gain to '0' for stable AGC on NTSC RF
			if ( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (gStable_AGC_WA_for_EBS_Enabled))
			{
				gStable_AGC_WA_for_EBS_Enabled = 0;
				CVD_L9Bx_Program_Color_System_PreJob2();
			}
#endif
#ifdef CVD_AGC_PEAK_CONTROL_WORKAROUND	// disabled
			// set AGC Peak Nominal value to 0x0a (register default value whenever state change occur)
			if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0))
			{
				g_AGC_Peak_Nominal_Value = AGC_PEAK_NOMINAL_DEFAULT_VALUE;
				CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_DEFAULT_VALUE);
			}
#endif
#ifdef L9_AGC_BYPASS_WA_FOR_NOISY_SIGNAL
			// for PERU Weak RF signal
			if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) {
				if (gCVD_Status_3CS.noisy_signal_detected == 1) {
					CVD_DEBUG("CVD Normal RF Signal(default) \n");
					// changed to normal signal
					gCVD_Status_3CS.noisy_signal_detected = 0;
					CVD_L9Bx_Set_for_Normal_Signal();
				}
			}
#endif
#ifdef L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC
			if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) {
				//		CVD_DEBUG("Set hstate_fixed value to 0 (default) \n");
				CVD_L9Bx_Set_Hstate_Fixed(0);
				CVD_L9Bx_Set_Hstate_Max(5);
			}
#endif
#ifdef L9_VSYNC_CNTL_FOR_STABLE_VSYNC // disabled
			if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0))  {
				CVD_L9Bx_Set_for_Stable_Vsync(0);
			}
#endif
#ifdef L9_CHANGE_FIELD_DETECT_MODE_ON_DONG_GO_DONG_RAK

			if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0))  {
				CVD_L9Bx_Set_for_Field_Detect_Mode(2);
			}
#endif
#ifdef L9_ADAPTIVE_AGC_PEAK_EN_CONTROL
			//120222 setting agc_peak_en to initial value is moved to CVD_STABLE_COUNT_1
			// for NTSC system agc_peak_en should be '1' on no signal state.
			//CVD_AGC_Peak_En_Control(2, 0, 0);	//force on peak_en
#endif
		}
		// #2.2 	: State Change & Previously Locked State is PALm
		else if( gPrevious_Main_System == CVD_STATE_PALm)
		{
#ifdef L9_WA_FOR_EBS_STABLE_AGC
			// restore default values of reg_agc_half_en to '0', and reg_nstd_hysis to '7' for EBS field stream support.
			// restore default values of reg_agc_peak_cntl to '1', and reg_dcrestore_gain to '0' for stable AGC on NTSC RF
			if ( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (gStable_AGC_WA_for_EBS_Enabled))
			{
				gStable_AGC_WA_for_EBS_Enabled = 0;
				CVD_L9Bx_Program_Color_System_PreJob2();
			}
#endif
#ifdef L9_DONT_KILL_CHROMALOCK_ON_CLOCK_LOST
 			if( lx_chip_rev() >= LX_CHIP_REV(L9,B0))	CVD_OnOff_Chromalock_Ckill(1);
 #endif
#ifdef L9_ADAPTIVE_BURST_GATE_END_MODE
			if( lx_chip_rev() >= LX_CHIP_REV(L9,B0))
			{
				CVD_L9Bx_Set_for_Burst_Gate_End_On_Noisy(gSetColorSystemMain[CVD_STATE_PALm].reg_burst_gate_end_new ,0);
#ifdef L9_BYPASS_AGC_ON_ADAPTIVE_BURST_GATE_END_MODE
				CVD_L9Bx_AGC_Bypass(0);
#endif
			}
#endif

		}
		// #2.3 	: State Change & Previously Locked State is PALi
		else if( gPrevious_Main_System == CVD_STATE_PALi) {
#ifdef L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC
			if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) {
				//		CVD_DEBUG("Set hstate_fixed value to 0 (default) \n");
				CVD_L9Bx_Set_Hstate_Fixed(0);
			}
#endif
			//120217 : added for PAL channel change
#ifdef L9_ADAPTIVE_AGC_PEAK_EN_CONTROL
			//120222 setting agc_peak_en to initial value is moved to CVD_STABLE_COUNT_1
			// for PAL system agc_peak_en should be '0' on no signal state.
			//CVD_AGC_Peak_En_Control(0, 0, 0);	//force off peak_en
#endif
		} // #2.4 	: State Change & Previously Locked State is SECAM
		else if( gPrevious_Main_System == CVD_STATE_SECAM) {
#ifdef L9_ADAPTIVE_AGC_PEAK_EN_CONTROL
			//120222 setting agc_peak_en to initial value is moved to CVD_STABLE_COUNT_1
			// for SECAM to PAL system change agc_peak_en should be '0' on no signal state.
			//CVD_AGC_Peak_En_Control(0, 0, 0);	//force off peak_en
#endif
		} // #2.5 	: State Change & Previously Locked State is not NTSC/PALm/PALi/SECAM
		else {
#ifdef L9_ADAPTIVE_AGC_PEAK_EN_CONTROL
			//120222 setting agc_peak_en to initial value is moved to CVD_STABLE_COUNT_1
		// for PAL system agc_peak_en should be '0' on no signal state.
//		CVD_AGC_Peak_En_Control(0, 0, 0);	//force off peak_en
#endif
		}
#if 0
		// Color System Changed Restore Default Value(NTSC)
		if( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && ( g_CVD_AGC_Peak_Nominal_Control.bEnable == TRUE ) )
		{
			g_CVD_AGC_Peak_Nominal_Control.pattern_found = 0x0 ;
			g_CVD_AGC_Peak_Nominal_Control.pattern_update = 0 ;
			g_CVD_AGC_Peak_Nominal_Control.pattern_frame_cnt = 0 ;

			// Control AGC Peak Nominal, Contrast & Brightness only for NTSC system
#ifdef L9_ADAPTIVE_AGC_PEAK_NOMINAL_CONTROL		//disabled
			if( gPrevious_Main_System == CVD_STATE_NTSC) {
				CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_DEFAULT_VALUE);
				CVD_L9Bx_Set_Contrast_Brightness(0x82, 0x20);
			}
#endif
		}
#endif
		// NTSC Pattern Detection Function
		if( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && ( g_CVD_Pattern_Detection_t.bEnable == TRUE ) )
		{
			g_CVD_Pattern_Detection_t.pattern_found = 0x0 ;
			g_CVD_Pattern_Detection_t.prev_motion_value = 0x0 ;
			g_CVD_Pattern_Detection_t.global_motion_count = 0x0 ;
		}


		if(gAutoProgramming == FALSE)
			CVD_AGC_Bypass_Function(0, 0, 1 );
			//CVD_AGC_Bypass_Function((UINT32)gCVD_Status_3CS.status_noise, (UINT32)gCVD_Status_3CS.h_lock_flag, (UINT32)gCVD_Status_3CS.no_signal_flag );

		//if((Next_State == CVD_STATE_NTSC) ||(Next_State == CVD_STATE_PALi))
		// Sync Detected !!!

		// #3 	: State Change Detected && Sync Lock Detected !!!
		//		:
		if(Next_State < CVD_STATE_VideoNotReady)
		{

			// If any color system is detected, disable color kill.
			CVD_Force_Color_Kill(0);

			/*
			//120217 for test only
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
				CVD_L9Bx_Reset_mif(0);
				*/

			//		if(gPrevious_Main_System != Next_State) // IF from no signal, do not re-setting cvd register to the same value

			// #4 	: State Change from No signal
			//		:
			if((gPrevious_Main_System != Next_State)|(Current_State==CVD_STATE_VideoNotReady))
			{
				//120217 : removed comb2d_only settting
#if 0
			//120109 : reset lbadrgen_rst and comb2d_only register on sync lock to reduce comb-filter artifact on channel change.
				//120216 : com2d_only setting on NTSC RF makes 3dcomb artifact on channel change
				if ( ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) ) && (Next_State == CVD_STATE_PALi))  {
				CVD_L9Bx_Set_comb2d_only(1);
				OS_MsecSleep(5);
				CVD_L9Bx_Set_comb2d_only(0);
			}
#endif
				// #5 	: State Change from No signal, and different color system from previously locked color system
				//		:
				if( ( MainCVD_Prev_Written_Color_System != Next_State) || (MainCVD_Is_Prev_RF_Input != g_CVD_RF_Input_Mode))
				{
					CVD_DEBUG("Setting CVD [%d]\n", Next_State);

					/*
					   if((Next_State == CVD_STATE_NTSC) ||(Next_State == CVD_STATE_NTSCj)|| (Next_State == CVD_STATE_NTSC443) )
					   {
					   gEnable_SW3DCOMBControl = 1;
					   cvd_pe_color_system = LX_CVD_PQ_NTSC_M;
					   }
					   else
					   {
					   gEnable_SW3DCOMBControl = 0;
					   CVD_Set_motion_mode(0x1, 0x0) ;
					   cvd_pe_color_system = LX_CVD_PQ_PAL;
					   }

					   CVD_Set_PQ_Mode(cvd_pe_color_system);

					 */
					if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
					{
						//AFE_PRINT("HDCT/CDCT Reset 1\n");
						CVD_Reset_hdct(1);
						CVD_Reset_cdct(1);

						//by dws : remove mdelay
						//mdelay(10);
						OS_MsecSleep(10);
					}

					CVD_Set_Color_System_3CS(Next_State);
					MainCVD_Prev_Written_Color_System = Next_State;
					MainCVD_Is_Prev_RF_Input = g_CVD_RF_Input_Mode;

					if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
					{
						CVD_Reset_hdct(0);
						CVD_Reset_cdct(0);
					}
				}
				// #6 	: State Change from No signal, and but same color system with previsouly locked system.
				//		: IF from VideoReadyToDetect, do not re-setting cvd register to the same value
				else
				{
					CVD_DEBUG("Same system [%d] again\n", Next_State);

#if 0
					if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
					{
						//111122 : disable 2dcomb only mode
						if( (Next_State == CVD_STATE_PALi) || (Next_State == CVD_STATE_PALCn) || (Next_State == CVD_STATE_PALm) || (Next_State == CVD_STATE_NTSC))
							CVD_L9Bx_Set_comb2d_only(0);
					}
#endif
				}

				//	CVD_Force_Color_Kill(0);
#ifdef CVD_CH42_SKIP_WORKAROUND
				//CVD_Clamp_Current_Control(0);	// 1118 change clamp current to zero
#endif
			}
			//Program_Color_System_Main_only_system_set(gSetColorSystemMain[Next_State]);
			//SW reset is needed here???
			//CVD_SW_Reset(LX_CVD_MAIN);
			gPrevious_Main_System = Next_State;
			//gMainCVD_State = Next_State;
			Current_State = Next_State;
		}
		// #7 	: State Change to NO SIGNAL !!!!!
		//		:
		else if(Next_State != CVD_STATE_VideoReadyToDetect)  // No Signal or color system change
		{
			Current_State = CVD_STATE_VideoNotReady;
#ifdef CVD_CH42_SKIP_WORKAROUND
			if ( lx_chip_rev( ) < LX_CHIP_REV( L9, A0 ) )
				CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
			else
			{
				//Disable Clampagc reset workaround (Clamp AGC problem is fixed by disabling x2 LPF setting at Set_AAF_Params)
				//CVD_Reset_Clampagc();
				CVD_Clamp_Current_Control(0x3);	// 1118 enlarge clamp current in no sigal state
			}
#endif
#if 0
			//111122 : set to 2d comb only mode on no signal state.
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
				CVD_L9Bx_Set_comb2d_only(1);
#endif
			/*
			//120217 for test only
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
				CVD_L9Bx_Reset_mif(1);
				*/

		}
		// #8 	: State Change to videoreadytodetect !!!!!
		//		: never enter here
		else
		{
			// From Ok : SW workaround for 3D comb operation 2010/06/04
			Current_State = CVD_STATE_VideoReadyToDetect;
		}
		MainCVD_Same_State_Count = 0;

		//1126 for fast standard detection
		gMainCVD_State = Next_State;
	}
	// #8 	: Same State Again
	//		:
	else // same state again
	{

		// #9 	: Same State and Sync Lock
		//		:
		if(Next_State < CVD_STATE_VideoNotReady)
		{
			CVD_NoBurst_CKill_Control(&gCVD_Status_3CS);

			cvd_noise_sum += gCVD_Status_3CS.status_noise;


			if(Same_State_Count > CVD_STABLE_COUNT_2) {
#ifdef L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC

				if ( (lx_chip_rev() >= LX_CHIP_REV( L9, B0))&&(gMainCVD_State == CVD_STATE_NTSC)  && (g_CVD_RF_Input_Mode == TRUE) && (gAutoProgramming == FALSE)) {
					gHstateMaxCounter++;
					if(CVD_L9Bx_Read_VCR_Detected()) vcr_detect_count++;

					if(gHstateMaxCounter >= 20)
					{
						if(vcr_detect_count >=  CVD_HSTATE_FIX_VCR_COUNT_THRESHOLD)
								CVD_L9Bx_Set_Hstate_Max(3);
						else CVD_L9Bx_Set_Hstate_Max(5);

						gHstateMaxCounter = 0;
						vcr_detect_count = 0;
					}
				}
#endif

#ifdef L9_ADAPTIVE_BURST_GATE_END_MODE
				if ( (lx_chip_rev() >= LX_CHIP_REV( L9, B0))&& (gMainCVD_State == CVD_STATE_PALm) && (g_CVD_RF_Input_Mode == TRUE)  )
				{
					if(gAdaptive_Burst_Gate_Counter == 0)
					{
						if ((gMainCVD_State == CVD_STATE_PALm) && (g_CVD_RF_Input_Mode == TRUE) ) {
							gVariance_Status_Cdto_Inc = CVD_L9Bx_Differential_Status_Cdto_Inc_Value();
							gAccumulated_Cordic_Freq_Value = CVD_L9Bx_Read_Cordic_Freq_Value();
							}
						gAdaptive_Burst_Gate_Counter++;
					}
					else if(gAdaptive_Burst_Gate_Counter < CVD_ADAPTIVE_BURST_GATE_COUNT_ITERATION)
					{
						if ((gMainCVD_State == CVD_STATE_PALm) && (g_CVD_RF_Input_Mode == TRUE) ) {
							gVariance_Status_Cdto_Inc += CVD_L9Bx_Differential_Status_Cdto_Inc_Value();
							gAccumulated_Cordic_Freq_Value += CVD_L9Bx_Read_Cordic_Freq_Value();
							}
						gAdaptive_Burst_Gate_Counter++;
					}
					else if(gAdaptive_Burst_Gate_Counter >= CVD_ADAPTIVE_BURST_GATE_COUNT_ITERATION)
					{
						if((gVariance_Status_Cdto_Inc >= CVD_ADAPTIVE_BURST_GATE_THRESHOLD_0)&&(gAccumulated_Cordic_Freq_Value >= CVD_ADAPTIVE_BURST_GATE_THRESHOLD_1))
							Adaptive_Burst_Gate_Enable = 1;
						else if(gVariance_Status_Cdto_Inc <= CVD_ADAPTIVE_BURST_GATE_THRESHOLD_3)
							Adaptive_Burst_Gate_Enable = 0;
						else
							Adaptive_Burst_Gate_Enable = 2;

						if((Adaptive_Burst_Gate_Enable == Prev_Adaptive_Burst_Gate_Enable)&&(Adaptive_Burst_Gate_Enable != 2))
						{
							CVD_L9Bx_Set_for_Burst_Gate_End_On_Noisy(gSetColorSystemMain[CVD_STATE_PALm].reg_burst_gate_end_new ,Adaptive_Burst_Gate_Enable);
#ifdef L9_BYPASS_AGC_ON_ADAPTIVE_BURST_GATE_END_MODE
							CVD_L9Bx_AGC_Bypass(Adaptive_Burst_Gate_Enable);
#endif
						}
						Prev_Adaptive_Burst_Gate_Enable = Adaptive_Burst_Gate_Enable;

						//AFE_PRINT("AVERAGE CDTO_INC[%x] CORDIC_FREQ[%d] RESULT[%d]\n", gVariance_Status_Cdto_Inc, gAccumulated_Cordic_Freq_Value, Adaptive_Burst_Gate_Enable );
						gAdaptive_Burst_Gate_Counter = 0;
					}
				}
#endif
#ifdef L9_AGC_BYPASS_WA_FOR_NOISY_SIGNAL

				stable_state2_count ++;

				if(stable_state2_count >= CVD_STABLE_COUNT_FOR_AGC_CONTROL) {

					avg_noise_level = cvd_noise_sum / stable_state2_count;
					stable_state2_count = 0;
					cvd_noise_sum = 0;

					//CVD_DEBUG("# avg noise level(stable state) [0x%x]#\n", avg_noise_level);

					// for PERU Weak RF signal
					//for noisy signal , diable agc(agc_bypass) & set dcrestore gain to 1/8 for stable sync and AGC.
					if ( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) ) {
						if( avg_noise_level >=  (CVD_L9B0_NOISE_MAX - 0x10) ) {
							if (gCVD_Status_3CS.noisy_signal_detected == 0) {
								CVD_DEBUG("CVD Noisy RF Signal(stable state) \n");
								// changed to noisy signal
								gCVD_Status_3CS.noisy_signal_detected = 1;
								CVD_L9Bx_Set_for_Noisy_Signal();
							}
						}
						else if( avg_noise_level < (CVD_L9B0_NOISE_MAX - 0x30) ) {
							if (gCVD_Status_3CS.noisy_signal_detected == 1) {
								CVD_DEBUG("CVD Normal RF Signal(stable state) \n");
								// changed to normal signal
								gCVD_Status_3CS.noisy_signal_detected = 0;
								CVD_L9Bx_Set_for_Normal_Signal();
							}
						}
					}

				}
#endif
				/* On Brazil (PAL-M) weak RF Signal, AGC gain sometimes became high value result in white picture.
				   Disabling AGC is needed on this weak RF signal( No h-lock, high noise level )
				 */
				if( (gAutoProgramming == FALSE) && (g_CVD_RF_Input_Mode == TRUE) && (gMainCVD_State == CVD_STATE_PALm))
					CVD_AGC_Bypass_Function((UINT32)gCVD_Status_3CS.avg_status_noise, (UINT32)gCVD_Status_3CS.h_lock_flag, (UINT32)gCVD_Status_3CS.no_signal_flag );

			}
			else if(Same_State_Count == CVD_STABLE_COUNT_2) {

#ifdef L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC
						if ( (lx_chip_rev() >= LX_CHIP_REV( L9, B0))&&(gMainCVD_State == CVD_STATE_NTSC)&& (g_CVD_RF_Input_Mode == TRUE) && (gAutoProgramming == FALSE)) {
							if(vcr_detect_count >=  CVD_HSTATE_FIX_VCR_COUNT_THRESHOLD)
								CVD_L9Bx_Set_Hstate_Max(3);
							else CVD_L9Bx_Set_Hstate_Max(5);
							vcr_detect_count = 0;
							gHstateMaxCounter = 0;
						}
#endif

//				CVD_DEBUG("### CVD Same State Max 2 !!! ###\n");

#ifdef L9_AGC_BYPASS_WA_FOR_NOISY_SIGNAL
				avg_noise_level = cvd_noise_sum / (Same_State_Count + 1);
				cvd_noise_sum = 0;	//
				stable_state2_count = 0;
				CVD_DEBUG("#avg noise level [0x%x]#\n", avg_noise_level);

				// for PERU Weak RF signal
				//for noisy signal , diable agc(agc_bypass) & set dcrestore gain to 1/8 for stable sync and AGC.
				if ( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) ) {
					// use long-time averaged value
					if( avg_noise_level >=  (CVD_L9B0_NOISE_MAX - 0x10) ) {
						if (gCVD_Status_3CS.noisy_signal_detected == 0) {
							CVD_DEBUG("CVD Noisy RF Signal \n");
							// changed to noisy signal
							gCVD_Status_3CS.noisy_signal_detected = 1;
							CVD_L9Bx_Set_for_Noisy_Signal();
						}
					}
				}
#endif
 #ifdef L9_DONT_KILL_CHROMALOCK_ON_CLOCK_LOST
 				if(( lx_chip_rev() >= LX_CHIP_REV(L9,B0)) && (gMainCVD_State == CVD_STATE_PALm) && (g_CVD_RF_Input_Mode == TRUE) )
 						CVD_OnOff_Chromalock_Ckill(0);
 #endif
#ifdef L9_ADAPTIVE_BURST_GATE_END_MODE
					if ( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (gMainCVD_State == CVD_STATE_PALm) && (g_CVD_RF_Input_Mode == TRUE) ) {
						if((gVariance_Status_Cdto_Inc >= CVD_ADAPTIVE_BURST_GATE_THRESHOLD_0)&&(gAccumulated_Cordic_Freq_Value >= CVD_ADAPTIVE_BURST_GATE_THRESHOLD_1))
							Adaptive_Burst_Gate_Enable = 1;
						else if(gVariance_Status_Cdto_Inc <= CVD_ADAPTIVE_BURST_GATE_THRESHOLD_3)
							Adaptive_Burst_Gate_Enable = 0;
						else
							Adaptive_Burst_Gate_Enable = 2;

						if((Adaptive_Burst_Gate_Enable == Prev_Adaptive_Burst_Gate_Enable)&&(Adaptive_Burst_Gate_Enable != 2))
						{
							CVD_L9Bx_Set_for_Burst_Gate_End_On_Noisy( gSetColorSystemMain[CVD_STATE_PALm].reg_burst_gate_end_new , Adaptive_Burst_Gate_Enable);
#ifdef L9_BYPASS_AGC_ON_ADAPTIVE_BURST_GATE_END_MODE
							CVD_L9Bx_AGC_Bypass(Adaptive_Burst_Gate_Enable);
#endif
						}
						//AFE_PRINT("AVERAGE CDTO_INC[%x] CORDIC_FREQ[%d] RESULT[%d]\n", gVariance_Status_Cdto_Inc, gAccumulated_Cordic_Freq_Value, Adaptive_Burst_Gate_Enable);
						gAdaptive_Burst_Gate_Counter = 0;
					}
#endif
#if 0
#ifdef L9_WA_FOR_EBS_STABLE_AGC
					// set reg_agc_half_en to '1', and reg_nstd_hysis to '0' for EBS field stream
					// set reg_agc_peak_cntl to '0', and reg_dcrestore_gain to '3' for stable AGC on NTSC RF
					if (( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) )
					{
						gStable_AGC_WA_for_EBS_Enabled = 1;
						CVD_L9Bx_Program_Color_System_PostJob2();
					}
#endif
#endif
#ifdef L9_ADAPTIVE_AGC_PEAK_EN_CONTROL
					//Enable agc_peak_en for AWC test
					if( (gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) )
						CVD_AGC_Peak_En_Control(1, gCVD_Status_3CS.avg_status_noise, gCVD_Status_3CS.avg_status_agc_gain);
					else
						CVD_AGC_Peak_En_Control(2, 0, 0);	// force on peak_en
#endif
#ifdef L9_VSYNC_CNTL_FOR_STABLE_VSYNC // disabled
						if ((gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) ) {
							CVD_L9Bx_Set_for_Stable_Vsync(1);
						}
#endif


					Same_State_Count++;
				}
				else if(Same_State_Count == CVD_STABLE_COUNT_1)
				{
//					CVD_DEBUG("### CVD Same State Max 1 !!! ###\n");

					//won.hur : added chip selection for L9B0
					//@2011.08.24
					if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0))
					{

#ifdef L9_CHANGE_FIELD_DETECT_MODE_ON_DONG_GO_DONG_RAK
						if ((gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) )
							CVD_L9Bx_Set_for_Field_Detect_Mode(1);
#endif
#ifdef CVD_AGC_PEAK_CONTROL_WORKAROUND// disabled
						if ((gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) )
						{
							//Set AGC Peak Nominal value to 0x30, when CVD input signal is stable.
							g_AGC_Peak_Nominal_Value = AGC_PEAK_NOMINAL_INITIAL_VALUE;
							CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_INITIAL_VALUE);
						}
#endif
#ifdef L9_ADAPTIVE_AGC_PEAK_NOMINAL_CONTROL		//disabled
						if ((gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE)  && ( g_CVD_AGC_Peak_Nominal_Control.bEnable == TRUE ))
						{
							// Control AGC Peak Nominal, Contrast & Brightness only for NTSC system
						//	CVD_DEBUG("Adaptive AGC : RF NTSC Stable  adjust agc_peak_nominal!!! \n");
							CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_INITIAL_VALUE);
							CVD_L9Bx_Set_Contrast_Brightness(0x7a, 0x1f);	// Value for  new reg_agc_peak_nominal
						}
#endif
						CVD_L9Bx_Program_Color_System_PostJob(&gSetColorSystemMain[Next_State]);
#ifdef L9_WA_FOR_EBS_STABLE_AGC
						// set reg_agc_half_en to '1', and reg_nstd_hysis to '0' for EBS field stream
						// set reg_agc_peak_cntl to '0', and reg_dcrestore_gain to '3' for stable AGC on NTSC RF
						if (((gMainCVD_State == CVD_STATE_NTSC) || (gMainCVD_State == CVD_STATE_PALm)) && (g_CVD_RF_Input_Mode == TRUE) )
						{
							gStable_AGC_WA_for_EBS_Enabled = 1;
							CVD_L9Bx_Program_Color_System_PostJob2();
						}
#endif

#ifdef L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC
						if ( ((gMainCVD_State == CVD_STATE_NTSC) || (gMainCVD_State == CVD_STATE_PALi) )&& (g_CVD_RF_Input_Mode == TRUE) && (gAutoProgramming == FALSE)) {
						//	CVD_DEBUG("Set hstate_fixed value to 1 \n");
							CVD_L9Bx_Set_Hstate_Fixed(1);
							if(gMainCVD_State == CVD_STATE_NTSC)
							{	CVD_L9Bx_Set_Hstate_Max(3);
								vcr_detect_count = 0;
							}
						}
#endif
#ifdef L9_ADAPTIVE_BURST_GATE_END_MODE
						if ((gMainCVD_State == CVD_STATE_PALm) && (g_CVD_RF_Input_Mode == TRUE) ) {
							gVariance_Status_Cdto_Inc = CVD_L9Bx_Differential_Status_Cdto_Inc_Value();
							gAccumulated_Cordic_Freq_Value = CVD_L9Bx_Read_Cordic_Freq_Value();
						}
#endif
					}
					else
					{
						CVD_L9Ax_Program_Color_System_PostJob(&gSetColorSystemMain[Next_State]);
					}

					Same_State_Count++;
#ifdef CVD_CH42_SKIP_WORKAROUND
					CVD_Clamp_Current_Control(0);	// 1118 change clamp current to zero
#endif
					//120222 : setting agc_peak_en initial values
					// PALi & SECAM RF : initially off
					// NTSC & others : initially on
#ifdef L9_ADAPTIVE_AGC_PEAK_EN_CONTROL
					if (( ( gMainCVD_State == CVD_STATE_SECAM) || (gMainCVD_State == CVD_STATE_PALi) )&& (g_CVD_RF_Input_Mode == TRUE) && (gAutoProgramming == FALSE)) {
						// for PAL system agc_peak_en should be '0' on no signal state.
						CVD_AGC_Peak_En_Control(0, 0, 0);	//force off peak_en
				}
					else
						CVD_AGC_Peak_En_Control(2, 0, 0);	//force on peak_en
#endif
				}

				else if(Same_State_Count < CVD_STABLE_COUNT_2)
				{
#ifdef L9_ADAPTIVE_BURST_GATE_END_MODE
					if ( ( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (gMainCVD_State == CVD_STATE_PALm) && (g_CVD_RF_Input_Mode == TRUE) ) {
						gVariance_Status_Cdto_Inc += CVD_L9Bx_Differential_Status_Cdto_Inc_Value();
						gAccumulated_Cordic_Freq_Value += CVD_L9Bx_Read_Cordic_Freq_Value();
					}
#endif
#ifdef L9_HSTATE_FIXED_CONTROL_FOR_STABLE_SYNC
						if ( (lx_chip_rev() >= LX_CHIP_REV( L9, B0))&&(gMainCVD_State == CVD_STATE_NTSC)&& (g_CVD_RF_Input_Mode == TRUE) && (gAutoProgramming == FALSE)) {
							if(CVD_L9Bx_Read_VCR_Detected()) vcr_detect_count++;
						}

						if((lx_chip_rev() >= LX_CHIP_REV( L9, B0))&&(Same_State_Count == 10))
						{
							if(vcr_detect_count+1 >= CVD_HSTATE_FIX_VCR_COUNT_THRESHOLD>>2)
								CVD_L9Bx_Set_Hstate_Max(3);
							else CVD_L9Bx_Set_Hstate_Max(5);
						}

#endif
					Same_State_Count++;
				}

				if(Same_State_Count > CVD_STABLE_COUNT_1)
				{
#ifdef CVD_AGC_PEAK_CONTROL_WORKAROUND	//disabled
					// Execute CVD_Adaptive_AGC_Peak_Control function which read histogram and set AGC Peak nominal value to default(0x0a) on white saturated signals.
					if (( lx_chip_rev() >= LX_CHIP_REV( L9, B0)) && (g_CVD_Color_System_Support == LX_COLOR_SYSTEM_NTSC_M) && (g_CVD_RF_Input_Mode == TRUE) )
						CVD_Adaptive_AGC_Peak_Control(&g_CVD_AGC_Peak_Control);
#endif
					if (( g_CVD_Pattern_Detection_t.bEnable == TRUE) && (gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) )
						CVD_Pattern_Detection(&g_CVD_Pattern_Detection_t );
					//CVD_Adaptive_AGC_Peak_Control_using_motion(&g_CVD_AGC_Peak_Nominal_Control );
#if 1
					//gogosing burst mag status check for color burst level test (color 틀어짐 대응) 2011.06.11
					//Apply cagc control for NTSC(color 틀어짐 대응) 2011.10.14
					if( (gMainCVD_State == CVD_STATE_PALi) && (g_CVD_RF_Input_Mode == TRUE) )
					{
						burst_mag_prev = burst_mag_next;
						burst_mag_next = CVD_Get_CVD_Burst_Mag_Flag(gMainCVD_State);

						if ( (burst_mag_next != burst_mag_prev)  )
						{
							//	CVD_DEBUG("burst_mag_prev = %d , burst_mag_next = %d \n",(int)burst_mag_prev,(int)burst_mag_next);
							CVD_Set_CVD_CAGC(burst_mag_next,&gSetColorSystemMain[gMainCVD_State], gMainCVD_State);
							//CVD_DEBUG("################# CAGC changed gogosing ############### \n");
						}
					}
					else if( (gMainCVD_State == CVD_STATE_NTSC) && (g_CVD_RF_Input_Mode == TRUE) )
					{
						//	burst_mag_prev = burst_mag_next;
						//	burst_mag_next = CVD_Get_CVD_Burst_Mag_Flag(gMainCVD_State);

						burst_mag_sum = 0;

						for(loop = 9;loop > 0;loop--) {
							burst_mag_array[loop] = burst_mag_array[loop-1] ;
							burst_mag_sum += burst_mag_array[loop];
						}

						if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0) )
							burst_mag_array[0] = CVD_L9Bx_Get_CVD_Burst_Mag_Value();

						burst_mag_sum += burst_mag_array[0];

						burst_mag_avg = burst_mag_sum / 10;


						//120127 : modified by kd.park
						//120131 : if pattern not found do not adjust cagc register
#if 0
						if( (burst_mag_avg > 0xbc0) )
#endif
							if( (burst_mag_avg > 0xbc0) || ( g_CVD_Pattern_Detection_t.pattern_found != 1)) {
								burst_mag_next = CVD_BURST_MAG_STATE_BIG;
								prev_burst_mag_avg = 0xbc0;
								cagc_target = gSetColorSystemMain[CVD_STATE_NTSC].reg_saturation;
							}
							else if(burst_mag_avg < 0x280) {
								burst_mag_next = CVD_BURST_MAG_STATE_VERY_SMALL;
								prev_burst_mag_avg = 0x280;
								cagc_target = 0x20;
							}
							else {
								burst_mag_next = (burst_mag_avg / 0x17) + 0x5;
								if(burst_mag_next > gSetColorSystemMain[CVD_STATE_NTSC].reg_saturation)
									burst_mag_next = gSetColorSystemMain[CVD_STATE_NTSC].reg_saturation;
								cagc_target = burst_mag_next;
							}

							//CVD_DEBUG("burst_mag = [0x%x], burst_mag_avg = [0x%x], prev_burst_mag_avg = [0x%x], burst_mag_next [0x%x], burst_mag_prev[0x%x] \n",burst_mag_array[0], burst_mag_avg, prev_burst_mag_avg, burst_mag_next, burst_mag_prev);

						if ( ( ( (burst_mag_next >= 0x5) && ( abs(prev_burst_mag_avg - burst_mag_avg) > 0x80) )  \
									||  ( ( burst_mag_next < 5) && (burst_mag_next != burst_mag_prev) ) ) \
								&& (Same_State_Count > (CVD_STABLE_COUNT_1 + 10 )) )
						{
							CVD_DEBUG("burst_mag_prev = 0x%x , burst_mag_next = 0x%x \n",burst_mag_prev,burst_mag_next);
							CVD_DEBUG("burst_mag_avg = 0x%x\n",burst_mag_avg);

							if ( lx_chip_rev() >= LX_CHIP_REV( L9, B0))
								cagc_written = CVD_L9Bx_Get_CAGC_Value();

							CVD_DEBUG("cagc_written = 0x%x , cagc_target = 0x%x \n",cagc_written,cagc_target);

							if(cagc_written > cagc_target)
								burst_mag_next = cagc_written - 1;
							else if(cagc_written < cagc_target)
								burst_mag_next = cagc_written + 1;
							else {
								CVD_DEBUG("CAGC Target Reached !!! [0x%x]\n",cagc_written);
								prev_burst_mag_avg = burst_mag_avg;
								burst_mag_prev = burst_mag_next;
								burst_mag_next = cagc_written;
							}

							CVD_Set_CVD_CAGC(burst_mag_next,&gSetColorSystemMain[gMainCVD_State], gMainCVD_State);
							//CVD_DEBUG("################# CAGC changed gogosing ############### \n");
						}

					}
					else
					{
						/*
						   burst_mag_prev=0;//pal 이외 시스템에서 초기화
						   burst_mag_next=0;
						 */
					}
#endif

				}
			}
		// #10 	: Same State , but no signal
		//		:
		else
			Same_State_Count = 0;

		gMainCVD_State = Next_State;
#if 0
	//periodic reset of CS0/CS1 is needed???
	if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
	{
		CVD_L9Bx_swrst_CS( CVD_SEL_CS_CS0);
		CVD_L9Bx_swrst_CS( CVD_SEL_CS_CS1);
	}
#endif


	}

	return 0;
}

#ifdef ENABLE_CVD_TIMER
static void CHBCVD_Timer(ULONG data)
{
	CHBCVD_Work_NTSC();
}

static int CHBCVD_Work_NTSC(void)
{
	static CVD_STATE_T	CHB_Current_State = CVD_STATE_VideoNotReady;
	static CVD_STATE_T	CHB_Next_State = CVD_STATE_VideoNotReady;
	static UINT8	cvd_fc_flag;
	static UINT8	No_Signal_Flag, HV_Lock_Flag, Vline_625_Flag, Pal_Flag, Chromalock_Flag, Secam_Flag;

#ifdef CHB_CVD_KERNEL_DEBUG
	AFE_PRINT("In CHBCVD Work\n");
	AFE_PRINT("CHBCVD State = [%d], ", CHB_Current_State);
#endif

	/* Read Status Registers */
	No_Signal_Flag = CVD_Get_CHB_No_Signal_Flag();
	HV_Lock_Flag = (CVD_Get_CHB_HLock_Flag())&&(CVD_Get_CHB_VLock_Flag());
	Vline_625_Flag = CVD_Get_CHB_Vline_625_Flag();
	Pal_Flag = CVD_Get_CHB_PAL_Flag();
	Secam_Flag = CVD_Get_CHB_SECAM_Flag();
	Chromalock_Flag = CVD_Get_CHB_Chromalock_Flag();
	cvd_fc_flag = CVD_Get_FC_Flag(LX_CVD_SUB);

#ifdef CHB_CVD_KERNEL_DEBUG
	AFE_PRINT("CHB : NO[%d] HVL[%d],V625[%d],PAL[%d],SECAM[%d],ChromaL[%d],cvd_fc[%d], c_state[%d], g_State[%d]\n ",No_Signal_Flag, HV_Lock_Flag, Vline_625_Flag, Pal_Flag, Secam_Flag, Chromalock_Flag, cvd_fc_flag, CHB_Current_State, gCHBCVD_State);
#endif

	switch(CHB_Current_State)
	{
		case CVD_STATE_VideoNotReady:
#if 0
			if ( HV_Lock_Flag /* || !No_Signal_Flag */)	// HV sync detected
				CHB_Next_State = CVD_STATE_VideoReadyToDetect;
			else
				CHB_Next_State = CVD_STATE_VideoNotReady;

			break;
#endif

		case CVD_STATE_VideoReadyToDetect:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				CHB_Next_State = CVD_STATE_VideoNotReady;

			else
				CHB_Next_State = CVD_STATE_NTSC;
			break;

		case CVD_STATE_NTSC:
			if ( !HV_Lock_Flag || No_Signal_Flag)	// No signal
				CHB_Next_State = CVD_STATE_VideoNotReady;

			else
				CHB_Next_State = CVD_STATE_NTSC;
			break;

		default:
			break;
	}

	if(CHB_Next_State != CHB_Current_State)//State Change !!!!!
	{

#ifdef CHB_CVD_KERNEL_DEBUG
		AFE_PRINT("CHB CVD State Change to [%d]\n", CHB_Next_State);
#endif

		CHB_Current_State = CHB_Next_State;

		gCHBCVD_State = CHB_Next_State;
	}

	return 0;
}
#endif

int CVD_Start_Timer(UINT32 arg)
{
#ifdef ENABLE_CVD_TIMER
	LX_AFE_CVD_TIMER_T cvd_timer_t;

	if (copy_from_user(&cvd_timer_t, (void __user *)arg, sizeof(LX_AFE_CVD_TIMER_T)))
	{
		return -EFAULT;
	}
	spin_lock(&cvd_lock);

	if((cvd_timer_t.cvd_main_sub == LX_CVD_MAIN) && (main_timer_state < 1))
	{
		main_timer_state = 1;
		OS_StartTimer( &main_timer, MainCVD_Timer, OS_TIMER_TIMETICK, cvd_timer_t.timeout, 0 );
		spin_unlock(&cvd_lock);
		AFE_PRINT("CVD Main : Starting Timer, timeout = [%d]msec\n",cvd_timer_t.timeout );
	}
	else if(chb_timer_state <1)
	{
		OS_StartTimer( &chb_timer, CHBCVD_Timer, OS_TIMER_TIMETICK, cvd_timer_t.timeout, 0 );
		chb_timer_state = 1;
		spin_unlock(&cvd_lock);
		AFE_PRINT("CVD CHB : Starting Timer, timeout = [%d]msec\n",cvd_timer_t.timeout );
	}
	else
	{
		spin_unlock(&cvd_lock);
		return -1;
	}

	return 0;
#endif
#ifdef ENABLE_CVD_THREAD
	if(gCVD_thread_running > 0)
		AFE_PRINT("CVD Thread already running !!!\n");
	else
	{
		gForce_thread_sleep = 0;
		wake_up_interruptible(&WaitQueue_CVD);
		AFE_PRINT("Starting CVD Thread\n");
	}

	return 0;
#endif

}

int CVD_Stop_Timer(UINT32 arg)
{
#ifdef ENABLE_CVD_TIMER
	LX_AFE_CVD_TIMER_T cvd_timer_t;

	if (copy_from_user(&cvd_timer_t, (void __user *)arg, sizeof(LX_AFE_CVD_TIMER_T)))
	{
		return -EFAULT;
	}

	spin_lock(&cvd_lock);
	if((cvd_timer_t.cvd_main_sub == LX_CVD_MAIN) && (main_timer_state == 1))
	{
		main_timer_state = 0 ;
		OS_StopTimer( &main_timer );
		spin_unlock(&cvd_lock);
		AFE_PRINT("CVD Main : Stop Timer\n");
	}
	else if(chb_timer_state == 1)
	{
		OS_StopTimer( &chb_timer );
		chb_timer_state = 0 ;
		spin_unlock(&cvd_lock);
		AFE_PRINT("CVD CHB : Stop Timer\n");
	}

	else
	{
		spin_unlock(&cvd_lock);
		return -1;
	}
	return 0;
#endif
#ifdef ENABLE_CVD_THREAD
		AFE_PRINT("Stopping CVD Thread\n");
		gForce_thread_sleep = 1;

	return 0;
#endif
}

int CVD_Program_Color_System(UINT32 arg)
{
	LX_AFE_CVD_PROGRAM_COLOR_SYSTEM_T program_color_system_t;

	if (copy_from_user(&program_color_system_t, (void __user *)arg, sizeof(LX_AFE_CVD_PROGRAM_COLOR_SYSTEM_T)))
	{
		return -EFAULT;
	}

	if(program_color_system_t.color_system >= LX_NON_STANDARD ) return -1;

	if(program_color_system_t.cvd_main_sub == LX_CVD_MAIN)
	{
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
			CVD_Set_Color_System_3CS(program_color_system_t.color_system);
		else
			CVD_Program_Color_System_Main_Multi(&gSetColorSystemMain[program_color_system_t.color_system]);

		gMainCVD_State = program_color_system_t.color_system;
		AFE_PRINT("CVD Main : Program Color System to [%d]\n", program_color_system_t.color_system);
	}
	else
	   AFE_PRINT("CVD CHB : Program Color System to [%d]\n", program_color_system_t.color_system);

	return 0;
}

int CVD_Set_VBI_Type(UINT32 arg)
{
	LX_AFE_CVD_VBI_MODE_T vbi_type_t;

	if (copy_from_user(&vbi_type_t, (void __user *)arg, sizeof(LX_AFE_CVD_VBI_MODE_T)))
	{
		return -EFAULT;
	}

	if(vbi_type_t == LX_VBI_MODE_TTX)
	{
		AFE_PRINT("Set to ANALOGTTX\n");
		ENABLE_ANALOGTTX = TRUE;
	}
	else if(vbi_type_t == LX_VBI_MODE_ACC)
	{
		AFE_PRINT("Set to ANALOGCC\n");
		ENABLE_ANALOGCC = TRUE;
	}

	return 0;
}

#ifdef	KDRV_CONFIG_PM
int CVD_RunSuspend(void)
{
	if (!gAfePmData->CVDInitStatus)
		return 0;

	return 0;
}

int CVD_RunResume(void)
{

	if (!gAfePmData->CVDInitStatus)
		return 0;

	if(gAfePmData->SelectMainSub == LX_CVD_MAIN)
	{
		//ALL_Power_Down();
		CVD_Power_Down(LX_CVD_MAIN, FALSE);
		CVD_Reg_Init(LX_CVD_MAIN);
		//Init Register to NTSC Mode
		CVD_Program_Color_System_Main_Multi(&gSetColorSystemMain[CVD_STATE_NTSC]);
		CVD_SW_Reset(LX_CVD_MAIN);
		//Program_Color_System_Main_tmp();
		if ( lx_chip_rev( ) < LX_CHIP_REV( L8, B0 ) )
			CVD_Set_Picture_Enhancement(FALSE, FALSE);
	}
	else
	{

		CVD_Power_Down(LX_CVD_SUB, FALSE);
		CVD_Reg_Init(LX_CVD_SUB);
		//reg init for CHB CVD
		CVD_Program_Color_System_CHB_NTSC();
	}

	return 0;
}

#endif


int CVD_Picture_Enhancement_Test(LX_AFE_CVD_TEST_PARAM_T *stCVD_Test_Param)
{
	LX_AFE_CVD_PQ_MODE_T 		color_system;

	if ( lx_chip_rev( ) < LX_CHIP_REV( L8, B0 ) )
		return -1;

	color_system = stCVD_Test_Param -> cvd_pe_mode;

	switch(stCVD_Test_Param -> cvd_pe_param)
	{
		case CVD_PE_AAF:	//AAF
			AFE_PRINT("Set AAF PARAMS with system [%d]\n", color_system);

			//won.hur : added additional chip selection routine for L9Bo
			//@2011.08.24
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_AAF_Params(&gSetAAF_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_AAF_Params(&gSetAAF_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_AAF_Params(&gSetAAF_L8Bx[color_system]);
			break;

		case CVD_PE_ADC:	//ADC
			AFE_PRINT("Set ADC PARAMS with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_ADC_Params(&gSetCresampler_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_ADC_Params(&gSetCresampler_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_ADC_Params(&gSetCresampler_L8Bx[color_system]);
			break;

		case CVD_PE_CRESAMPLER:	//cresampler
			AFE_PRINT("Set cresampler PARAMS with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Cresampler_Params(&gSetCresampler_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Cresampler_Params(&gSetCresampler_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Cresampler_Params(&gSetCresampler_L8Bx[color_system]);
			break;

		case CVD_PE_DOT_CRAWL:	//Dot-crawl
			AFE_PRINT("Set Dot-Crawl PARAMS with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Dot_Crawl_Params(&gSetDotCrawl_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Dot_Crawl_Params(&gSetDotCrawl_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Dot_Crawl_Params(&gSetDotCrawl_L8Bx[color_system]);
			break;

		case CVD_PE_CROSS_COLOR_RECUDER: //Cross color reducer
			AFE_PRINT("Set Cross color reducer with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Cross_Color_Reducer(&gSetCrossColorReducer_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Cross_Color_Reducer(&gSetCrossColorReducer_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Cross_Color_Reducer(&gSetCrossColorReducer_L8Bx[color_system]);
			break;

		case CVD_PE_MOTION_DETECTOR: //Motion Detector
			AFE_PRINT("Set Motion Detector with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Motion_Detector(&gSetMotionDetector_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Motion_Detector(&gSetMotionDetector_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Motion_Detector(&gSetMotionDetector_L8Bx[color_system]);
			break;

		case CVD_PE_GLOBAL_CLASSIFIER: //Global Classifier
			AFE_PRINT("Set Global Classifier with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Global_Classifier(&gSetGlobalClassifier_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Global_Classifier(&gSetGlobalClassifier_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Global_Classifier(&gSetGlobalClassifier_L8Bx[color_system]);
			break;

		case CVD_PE_OUTPUT_DELAY: //Output Delay
			AFE_PRINT("Set Output Delay with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Output_Delay( &gSetOutputDelay_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Output_Delay( &gSetOutputDelay_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Output_Delay( &gSetOutputDelay_L8Bx[color_system]);
			break;

		case CVD_PE_OUTPUT_RANGE: //Output Range
			AFE_PRINT("Set Output Range with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Output_Range( &gSetOutputRange_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Output_Range( &gSetOutputRange_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Output_Range( &gSetOutputRange_L8Bx[color_system]);
			break;

		case CVD_PE_BLENDING: //Blending
			AFE_PRINT("Set Blending with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Blending( &gSetBlending_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Blending( &gSetBlending_L9Ax[color_system]);
			else
				CVD_L8Bx_Set_Blending( &gSetBlending_L8Bx[color_system]);
			break;

		case CVD_PE_CHROMA_DEMODE: //Chroma Demode
			AFE_PRINT("Set Chroma Demode with system [%d]\n", color_system);
			if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
			{
			//	CVD_L9Bx_Set_Chroma_Demode( &gSetChromaDemode_L9Ax[color_system]);
			}
			else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
				CVD_L9Ax_Set_Chroma_Demode( &gSetChromaDemode_L9Ax[color_system]);
			break;

		default:
			break;

	}
	return 0;
}

int CVD_Set_SW_3DCOMB_Control(UINT32 arg)
{
	LX_AFE_CVD_SW_3DCOMB_CONTROL_T	control_value_t;

	if (copy_from_user(&control_value_t, (void __user *)arg, sizeof(LX_AFE_CVD_SW_3DCOMB_CONTROL_T)))
	{
		return -EFAULT;
	}

	if(control_value_t.Enable > 0)
	{
		gEnable_SW3DCOMBControl = 1;
		if(control_value_t.Hf_thr < 0xffffffff)
			gHf_thr_value= control_value_t.Hf_thr;
		if(control_value_t.Lf_thr < 0xffffffff)
			gLf_thr_value = control_value_t.Lf_thr;
		if(control_value_t.Diff_thr < 0xffffffff)
			gDiff_thr_value = control_value_t.Diff_thr;
		if(control_value_t.Region_30_mode < 0xffffffff)
			gRegion_30_mode = control_value_t.Region_30_mode;
	}
	else
	{
		gEnable_SW3DCOMBControl = 0;
		CVD_Set_motion_mode(0x1, 0x0) ;
	}

	CVD_DEBUG("SW 3DCOMB control value : Enable[%d], Hf_thr[0x%x], Lf_thr[0x%x], Diff_thr[0x%x], Region_30_mode[%d]\n", gEnable_SW3DCOMBControl, gHf_thr_value, gLf_thr_value, gDiff_thr_value, gRegion_30_mode);

	return 0;
}

int CVD_Set_Source_Type_Control(UINT32 arg)
{

	LX_AFE_CVD_SET_INPUT_T	cvd_input_info;

	//AFE_PRINT("Entered CVD_Set_Source_Type_Control\n");

	CVD_Set_Source_Type(arg);

	if (copy_from_user(&cvd_input_info, (void __user *)arg, sizeof(LX_AFE_CVD_SET_INPUT_T)))
	{
		AFE_PRINT("Error return at CVD_Set_Source_Type_Control\n");
		return -1;
	}

	if(cvd_input_info.cvd_main_sub == LX_CVD_MAIN)
	{
//		AFE_PRINT("Main CVD input\n");
		if(cvd_input_info.cvd_input_source_attribute == LX_CVD_INPUT_SOURCE_ATTRIBUTE_RF)
		{
			CVD_DEBUG("CVD : RF Input Mode\n");
			g_CVD_RF_Input_Mode = TRUE;
		}
		else
		{
			CVD_DEBUG("CVD : AV Input Mode\n");
			g_CVD_RF_Input_Mode = FALSE;
		}

		Current_State = CVD_STATE_VideoNotReady;
		Next_State = CVD_STATE_VideoNotReady;
		gPortChangeDetected = TRUE;

//#ifdef	ENABLE_CVD_INTERRUPT_CONTROL
		if( ENABLE_CVD_INTERRUPT_CONTROL )
		{
#ifdef ENABLE_CVD_THREAD
			//spin_lock(&cvd_lock);
			if(gCVD_thread_running < 1)
			{
				wake_up_interruptible(&WaitQueue_CVD);
				gCVD_thread_running = 1;
			//	spin_unlock(&cvd_lock);
			}
			else
			{
			//	spin_unlock(&cvd_lock);
				AFE_ERROR("CVD Thread Already Running\n");
			}
#endif
#if 0
			spin_lock(&cvd_lock);
			if((main_timer_state < 1))
			{
				main_timer_state = 1;
				OS_StartTimer( &main_timer, MainCVD_Timer, OS_TIMER_TIMETICK,MAINCVD_TIMEOUT , 0 );
#ifdef CVD_KERNEL_DEBUG
				AFE_PRINT("CVD Main : Force Starting Timer\n");
#endif
			}
			spin_unlock(&cvd_lock);
#endif
			//#endif
		}

	}
	else // For CHB input
	{
		/* No source select fuction on L8 */
		/* On L8, CHB source is fixed on CVBS_CB */
	}
	return 0;

}

int CVD_Enable_Detection_Interrupt(int enable)
{
	if(enable > 0)
	{
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
			gVportRegBx->intr_vp1_enable.intr_vp1_enable |= L8_MAIN_CVD_INTERRUPT_MASK;
		else
			gVportRegAx->intr_vp1_enable.intr_vp1_enable |= L8_MAIN_CVD_INTERRUPT_MASK;
	}
	else
	{
		if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
			gVportRegBx->intr_vp1_enable.intr_vp1_enable&= ~(L8_MAIN_CVD_INTERRUPT_MASK);
		else
			gVportRegAx->intr_vp1_enable.intr_vp1_enable&= ~(L8_MAIN_CVD_INTERRUPT_MASK);
	}

	return 0;
}

int CVD_Get_States_Detail(UINT32 arg)
{
	LX_AFE_CVD_STATES_DETAIL_T	cvd_states_detail_t;
	//UINT32 status_reg;

	/* Read Status Registers */
	cvd_states_detail_t.No_Signal_Flag = CVD_Get_No_Signal_Flag();
	cvd_states_detail_t.HLock_Flag = CVD_Get_HLock_Flag();
	cvd_states_detail_t.VLock_Flag = CVD_Get_VLock_Flag();
	cvd_states_detail_t.VLine_625_Flag = CVD_Get_Vline_625_Flag();
	cvd_states_detail_t.Pal_Flag = CVD_Get_PAL_Flag();
	cvd_states_detail_t.Secam_Flag = CVD_Get_SECAM_Flag();
	cvd_states_detail_t.Chromalock_Flag = CVD_Get_Chromalock_Flag();
	cvd_states_detail_t.NoBurst_Flag = CVD_Get_NoBurst_Flag();
	cvd_states_detail_t.FC_Flag = CVD_Get_FC_Flag(LX_CVD_MAIN);
	cvd_states_detail_t.cvd_cordic_freq = CVD_Get_Cordic_Freq(LX_CVD_MAIN);
	cvd_states_detail_t.status_noise = CVD_Get_Noise_Status();
	CVD_Read_fld_cnt_value(&cvd_states_detail_t.hfcnt_value, &cvd_states_detail_t.lfcnt_value);
	cvd_states_detail_t.MainCVD_State = gMainCVD_State;
	cvd_states_detail_t.Current_State = Current_State;
	cvd_states_detail_t.Next_State = Next_State;


	// won.hur : added additional chip selection routine for L9B0
	if( lx_chip_rev( ) >= LX_CHIP_REV(L9,B0) )
	{
		cvd_states_detail_t.Pal_Flag_CS0 = CVD_L9Bx_Get_PAL_Flag_CS0();
		cvd_states_detail_t.Secam_Flag_CS0 = CVD_L9Bx_Get_SECAM_Flag_CS0();
		cvd_states_detail_t.Chromalock_Flag_CS0 = CVD_L9Bx_Get_Chromalock_Flag_CS0();
		cvd_states_detail_t.FC_Flag_CS0 = CVD_L9Bx_Get_FC_Flag_CS0();
		cvd_states_detail_t.cvd_cordic_freq_CS0 = CVD_L9Bx_Get_Cordic_Freq_CS0();

		cvd_states_detail_t.Pal_Flag_CS1 = CVD_L9Bx_Get_PAL_Flag_CS1();
		cvd_states_detail_t.Secam_Flag_CS1 = CVD_L9Bx_Get_SECAM_Flag_CS1();
		cvd_states_detail_t.Chromalock_Flag_CS1 = CVD_L9Bx_Get_Chromalock_Flag_CS1();
		cvd_states_detail_t.FC_Flag_CS1 = CVD_L9Bx_Get_FC_Flag_CS1();
		cvd_states_detail_t.cvd_cordic_freq_CS1 = CVD_L9Bx_Get_Cordic_Freq_CS1();

		cvd_states_detail_t.CVD_Color_System = gCVD_Status_3CS.cvd_color_system;
		cvd_states_detail_t.CS0_Color_System = gCVD_Status_3CS.cs0_color_system;
		cvd_states_detail_t.CS1_Color_System = gCVD_Status_3CS.cs1_color_system;
		cvd_states_detail_t.color_system_support = g_CVD_Color_System_Support;

		CVD_L9Bx_Get_PE0_Motion_Value(&cvd_states_detail_t.tnr_x, &cvd_states_detail_t.tnr_s, &cvd_states_detail_t.s_status );
		cvd_states_detail_t.motion_diff = g_CVD_Pattern_Detection_t.global_motion_diff;
		cvd_states_detail_t.motion_value = CVD_L9Bx_Get_Global_Motion_Value();
		cvd_states_detail_t.pattern_found = g_CVD_Pattern_Detection_t.pattern_found;

		cvd_states_detail_t.agc_gain = CVD_L9Bx_Get_Status_AGC_Gain();
		cvd_states_detail_t.cagc = CVD_L9Bx_Get_CAGC_Value();
		//cvd_states_detail_t.saturation = CVD_L9Bx_Get_Saturation_Value();
		cvd_states_detail_t.saturation = CVD_L9Bx_Get_oadj_c_coeff_value();
		cvd_states_detail_t.hnon_standard = CVD_L9Bx_Get_HNon_Standard_Flag();
		cvd_states_detail_t.vnon_standard = CVD_L9Bx_Get_VNon_Standard_Flag();
		cvd_states_detail_t.burst_mag = CVD_L9Bx_Get_CVD_Burst_Mag_Value();
		cvd_states_detail_t.vdetect_vcount = CVD_L9Bx_Get_Vdetect_Vcount_Value();
		cvd_states_detail_t.agc_peak_en = CVD_L9Bx_Get_AGC_Peak_En_Value();
		cvd_states_detail_t.agc_bypass = CVD_L9Bx_Get_AGC_Bypass_Value();
	}
	else if( lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )  // for L9 CS0, CS1
	{
		cvd_states_detail_t.Pal_Flag_CS0 = CVD_L9Ax_Get_PAL_Flag_CS0();
		cvd_states_detail_t.Secam_Flag_CS0 = CVD_L9Ax_Get_SECAM_Flag_CS0();
		cvd_states_detail_t.Chromalock_Flag_CS0 = CVD_L9Ax_Get_Chromalock_Flag_CS0();
		cvd_states_detail_t.FC_Flag_CS0 = CVD_L9Ax_Get_FC_Flag_CS0();
		cvd_states_detail_t.cvd_cordic_freq_CS0 = CVD_L9Ax_Get_Cordic_Freq_CS0();

		cvd_states_detail_t.Pal_Flag_CS1 = CVD_L9Ax_Get_PAL_Flag_CS1();
		cvd_states_detail_t.Secam_Flag_CS1 = CVD_L9Ax_Get_SECAM_Flag_CS1();
		cvd_states_detail_t.Chromalock_Flag_CS1 = CVD_L9Ax_Get_Chromalock_Flag_CS1();
		cvd_states_detail_t.FC_Flag_CS1 = CVD_L9Ax_Get_FC_Flag_CS1();
		cvd_states_detail_t.cvd_cordic_freq_CS1 = CVD_L9Ax_Get_Cordic_Freq_CS1();

		cvd_states_detail_t.CVD_Color_System = gCVD_Status_3CS.cvd_color_system;
		cvd_states_detail_t.CS0_Color_System = gCVD_Status_3CS.cs0_color_system;
		cvd_states_detail_t.CS1_Color_System = gCVD_Status_3CS.cs1_color_system;
		cvd_states_detail_t.color_system_support = g_CVD_Color_System_Support;
	}

	if (copy_to_user((UINT32*)arg, (UINT32*)&cvd_states_detail_t, sizeof(LX_AFE_CVD_STATES_DETAIL_T)))
	{
		return -1;
	}

	return 0;
}

int CVD_Set_PQ_Mode(LX_AFE_CVD_PQ_MODE_T cvd_pq_mode)
{
	if ( lx_chip_rev( ) < LX_CHIP_REV( L8, B0 ) )
		return -1;

	CVD_DEBUG("@@@@@ CVD Set PQ Mode to [%d]@@@@@\n", cvd_pq_mode);

	//won.hur : added additional chip selection routine for L9B0
	if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
	{
		CVD_REG_PARAM_T	*pCVD_PE_Param_t;
		UINT32	size;
		static LX_AFE_CVD_PQ_MODE_T prev_cvd_pq_mode = LX_CVD_PQ_DEFAULT;

		/*
		if(prev_cvd_pq_mode ==cvd_pq_mode) {
			CVD_DEBUG("SAME CVD PE Param, bypass PE settings !\n");
			return -1;
		}
		*/

		switch(cvd_pq_mode)
		{
			case LX_CVD_PQ_NTSC_M:		// AV NTSC
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_av_ntsc_m_l_default;
				size	= sizeof(cvd_av_ntsc_m_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_NTSC_M_RF:	// RF NTSC
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_rf_ntsc_m_l_default;
				size	= sizeof(cvd_rf_ntsc_m_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL:			// AV PAL
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_av_pali_l_default;
				size	= sizeof(cvd_av_pali_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL_RF:		// RF PAL
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_rf_pali_l_default;
				size	= sizeof(cvd_rf_pali_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_SECAM:		// AV SECAM
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_av_secam_l_default;
				size	= sizeof(cvd_av_secam_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_SECAM_RF:	// RF SECAM
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_rf_secam_l_default;
				size	= sizeof(cvd_rf_secam_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_NTSC_443:		// AV NTSC 443
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_av_ntsc_443_l_default;
				size	= sizeof(cvd_av_ntsc_443_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_NTSC_443_RF:	// RF NTSC 443
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_rf_ntsc_443_l_default;
				size	= sizeof(cvd_rf_ntsc_443_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL_60:			// AV PAL 60
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_av_pal60_l_default;
				size	= sizeof(cvd_av_pal60_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL_60_RF:		// RF PAL 60
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_rf_pal60_l_default;
				size	= sizeof(cvd_rf_pal60_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL_M:		// AV PAL M
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_av_palm_l_default;
				size	= sizeof(cvd_av_palm_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL_M_RF:	// RF PAL M
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_rf_palm_l_default;
				size	= sizeof(cvd_rf_palm_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL_NC:		// AV PAL NC
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_av_palnc_l_default;
				size	= sizeof(cvd_av_palnc_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			case LX_CVD_PQ_PAL_NC_RF:	// RF PAL NC
				pCVD_PE_Param_t = (CVD_REG_PARAM_T *)cvd_rf_palnc_l_default;
				size	= sizeof(cvd_rf_palnc_l_default) / sizeof(CVD_REG_PARAM_T) ;
				break;
			default:
				AFE_PRINT("@@@ ERROR unknown color system [%d] @@@\n", cvd_pq_mode);
				return -1;
		}

		prev_cvd_pq_mode = cvd_pq_mode;

		CVD_L9Bx_Set_PE_Params(pCVD_PE_Param_t, size, cvd_pq_mode);

	}
	else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
	{
		switch(cvd_pq_mode)
		{
			case LX_CVD_PQ_NTSC_M:		// AV NTSC
			case LX_CVD_PQ_NTSC_M_RF:	// RF NTSC
			case LX_CVD_PQ_PAL:			// AV PAL
			case LX_CVD_PQ_PAL_RF:		// RF PAL
			case LX_CVD_PQ_SECAM:		// AV SECAM
			case LX_CVD_PQ_SECAM_RF:	// RF SECAM
				break;
			case LX_CVD_PQ_NTSC_443:		// AV NTSC 443
				cvd_pq_mode = LX_CVD_PQ_NTSC_M;
				break;
			case LX_CVD_PQ_NTSC_443_RF:	// RF NTSC 443
				cvd_pq_mode = LX_CVD_PQ_NTSC_M_RF;
				break;
			case LX_CVD_PQ_PAL_60:			// AV PAL 60
				cvd_pq_mode = LX_CVD_PQ_PAL;
				break;
			case LX_CVD_PQ_PAL_60_RF:		// RF PAL 60
				cvd_pq_mode = LX_CVD_PQ_PAL_RF;
				break;
			case LX_CVD_PQ_PAL_M:		// AV PAL M
				cvd_pq_mode = LX_CVD_PQ_PAL;
				break;
			case LX_CVD_PQ_PAL_M_RF:	// RF PAL M
				cvd_pq_mode = LX_CVD_PQ_PAL_RF;
				break;
			case LX_CVD_PQ_PAL_NC:		// AV PAL NC
				cvd_pq_mode = LX_CVD_PQ_PAL;
				break;
			case LX_CVD_PQ_PAL_NC_RF:	// RF PAL NC
				cvd_pq_mode = LX_CVD_PQ_PAL_RF;
				break;
			default:
				AFE_PRINT("@@@ ERROR unknown color system [%d] @@@\n", cvd_pq_mode);
				return -1;
		}
		CVD_L9Ax_Set_AAF_Params(&gSetAAF_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_ADC_Params(&gSetCresampler_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Cresampler_Params(&gSetCresampler_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Dot_Crawl_Params(&gSetDotCrawl_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Cross_Color_Reducer(&gSetCrossColorReducer_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Motion_Detector(&gSetMotionDetector_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Global_Classifier(&gSetGlobalClassifier_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Output_Delay( &gSetOutputDelay_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Output_Range( &gSetOutputRange_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Blending( &gSetBlending_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Chroma_Demode( &gSetChromaDemode_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Luma_Peaking( &gSetLumaPeaking_L9Ax[cvd_pq_mode]);
		CVD_L9Ax_Set_Secam_Freq( &gSetSecamFreq_L9Ax[cvd_pq_mode]);
	}
	else if ( lx_chip_rev( ) == LX_CHIP_REV( L8, B0 ) )
	{
		switch(cvd_pq_mode)
		{
			case LX_CVD_PQ_NTSC_M:		// AV NTSC
			case LX_CVD_PQ_NTSC_443:		// AV NTSC 443
			case LX_CVD_PQ_PAL:			// AV PAL
			case LX_CVD_PQ_SECAM:		// AV SECAM
			case LX_CVD_PQ_PAL_60:			// AV PAL 60
			case LX_CVD_PQ_PAL_M:		// AV PAL M
			case LX_CVD_PQ_PAL_NC:		// AV PAL NC
				cvd_pq_mode = LX_CVD_PQ_NTSC_M;
				break;
			case LX_CVD_PQ_NTSC_M_RF:	// RF NTSC
			case LX_CVD_PQ_PAL_RF:		// RF PAL
			case LX_CVD_PQ_SECAM_RF:	// RF SECAM
			case LX_CVD_PQ_NTSC_443_RF:	// RF NTSC 443
			case LX_CVD_PQ_PAL_60_RF:		// RF PAL 60
			case LX_CVD_PQ_PAL_M_RF:	// RF PAL M
			case LX_CVD_PQ_PAL_NC_RF:	// RF PAL NC
				cvd_pq_mode = LX_CVD_PQ_NTSC_M_RF;
				break;
			default:
				AFE_PRINT("@@@ ERROR unknown color system [%d] @@@\n", cvd_pq_mode);
				return -1;
		}
		CVD_L8Bx_Set_AAF_Params(&gSetAAF_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_ADC_Params(&gSetCresampler_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Cresampler_Params(&gSetCresampler_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Dot_Crawl_Params(&gSetDotCrawl_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Cross_Color_Reducer(&gSetCrossColorReducer_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Motion_Detector(&gSetMotionDetector_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Global_Classifier(&gSetGlobalClassifier_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Output_Delay( &gSetOutputDelay_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Output_Range( &gSetOutputRange_L8Bx[cvd_pq_mode]);
		CVD_L8Bx_Set_Blending( &gSetBlending_L8Bx[cvd_pq_mode]);
	}

	return 0;

}

int CVD_Null_Function(void)
{
	return 0;
}

int CVD_Test_Program(LX_AFE_CVD_TEST_PARAM_T *stCVD_Test_Param)
{
	int ret = 0;

	switch(stCVD_Test_Param -> item_to_test)
	{
		case CVD_TEST_PICTURE_ENHANCEMENT:
			CVD_Picture_Enhancement_Test(stCVD_Test_Param);
			break;
		case CVD_TEST_ENABLE_STATUS_FLD_PRINT:
			CVD_Enable_Status_Fld_Print(stCVD_Test_Param->bEnable_Status_Fld_Print);
			break;
		case CVD_TEST_ENABLE_3DCOMB_STATE_PRINT:
			CVD_Enable_3DCOMB_State_Print(stCVD_Test_Param->bEnable_3DCOMB_State_Print);
			break;
		case CVD_TEST_HSTATEMAX_WRITE:
			CVD_Set_Hstate_Max(stCVD_Test_Param->hstate_max_value);
			break;
		case CVD_TEST_AGC_PEAK_NOMINAL:
			/*
			g_CVD_AGC_Peak_Control.bEnable = 			stCVD_Test_Param->agc_peak_enable;
			g_CVD_AGC_Peak_Control.white_ratio_th = 	stCVD_Test_Param->agc_pean_white_ratio_th;
			g_CVD_AGC_Peak_Control.white_frame_max_th = stCVD_Test_Param->agc_pean_white_frame_max_th;
			g_CVD_AGC_Peak_Control.white_frame_on = 	stCVD_Test_Param->agc_pean_white_frame_on;
			g_CVD_AGC_Peak_Control.white_frame_off = 	stCVD_Test_Param->agc_pean_white_frame_off;
			g_CVD_AGC_Peak_Control.print_en = 			stCVD_Test_Param->agc_pean_print_en;
			g_CVD_AGC_Peak_Nominal_Control.bEnable 				= stCVD_Test_Param->agc_peak_enable ;
			g_CVD_AGC_Peak_Nominal_Control.tnr_x_avg_t_th 		= stCVD_Test_Param->agc_peak_x_avg_t_th;
			g_CVD_AGC_Peak_Nominal_Control.tnr_x_avg_s_th 		= stCVD_Test_Param->agc_peak_x_avg_s_th;
			g_CVD_AGC_Peak_Nominal_Control.tpd_s_status_th 		= stCVD_Test_Param->agc_peak_s_staus_th;
			g_CVD_AGC_Peak_Nominal_Control.pattern_frame_max_th = stCVD_Test_Param->agc_peak_white_frame_max_th ;
			g_CVD_AGC_Peak_Nominal_Control.pattern_frame_on 	= stCVD_Test_Param->agc_peak_white_frame_on ;
			g_CVD_AGC_Peak_Nominal_Control.pattern_frame_off 	= stCVD_Test_Param->agc_peak_white_frame_off ;
			g_CVD_AGC_Peak_Nominal_Control.noise_th			 	= stCVD_Test_Param->agc_peak_noise_th ;
			g_CVD_AGC_Peak_Nominal_Control.print_en 			= stCVD_Test_Param->agc_peak_print_en ;
			*/
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}


int CVD_Set_for_Tunning(BOOLEAN bEnable)
{
	int ret = 0;

	OS_LockMutex(&g_CVD_Sema);

	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		if(bEnable == TRUE)
		{
			gAutoProgramming = 1;
			CVD_L9Bx_Set_dcrestore_accum_width(DCRESTORE_ACCUM_WIDTH_DEFAULT);
			CVD_L9Bx_Set_Noise_Threshold(NOISE_THRESHOLD_DEFAULT);

			// restore default value of reg_hnon_std_threshold
#ifdef L9_FAST_3DCOMB_WORKAROUND
			CVD_L9Bx_Set_HNon_Standard_Threshold(gSetColorSystemMain[CVD_STATE_NTSC].reg_hnon_std_threshold);
#endif
		}
		else
		{
			gAutoProgramming = 0;
			if(gPrevious_Main_System < CVD_STATE_VideoNotReady)
				CVD_L9Bx_Set_dcrestore_accum_width(gSetColorSystemMain[gPrevious_Main_System].reg_dcrestore_accum_width);
			CVD_L9Bx_Set_Noise_Threshold(NOISE_THRESHOLD_WEAK_RF);
		}
	}
	OS_UnlockMutex(&g_CVD_Sema);
	return ret;
}

#if 0
#ifdef CVD_AGC_PEAK_CONTROL_WORKAROUND	//disabled
static int CVD_Adaptive_AGC_Peak_Control(CVD_AGC_PEAK_CONTROL_T *pCVD_AGC_Peak_Control )
{
	static UINT32	pe_histo_bin[4][64] = {{0,}};
	UINT32 avg_histo_bin[64];
	UINT32 sum_histo_bin = 0;
	UINT32 white_ratio;
	static UINT32	white_frame_cnt = 0;
	UINT32	agc_peak_nominal_read = 0 ;
	int	loop;

	if(pCVD_AGC_Peak_Control->bEnable == FALSE)
	{
		if(pCVD_AGC_Peak_Control->print_en)
			AFE_PRINT("CVD_Adaptive_AGC_Peak_Control Disabled\n");

		return 0;
	}

	agc_peak_nominal_read = CVD_L9Bx_Get_AGC_Peak_Nominal();

	memcpy( pe_histo_bin[3], pe_histo_bin[2], sizeof(UINT32)*32);
	memcpy( pe_histo_bin[2], pe_histo_bin[1], sizeof(UINT32)*32);
	memcpy( pe_histo_bin[1], pe_histo_bin[0], sizeof(UINT32)*32);
	memcpy( pe_histo_bin[0], &_g_dce_histo_info[LX_PE_WIN_MAIN].histogram[0], sizeof(UINT32)*64);

	for(loop = 0; loop < 32; loop++)
	{
		avg_histo_bin[loop] = ( pe_histo_bin[0][loop] + pe_histo_bin[1][loop] + pe_histo_bin[2][loop] + pe_histo_bin[3][loop] ) / 4;
		sum_histo_bin += avg_histo_bin[loop];

	}
	white_ratio = (avg_histo_bin[31] + avg_histo_bin[30] + avg_histo_bin[29] + avg_histo_bin[28] ) * 100 / sum_histo_bin;

	if(pCVD_AGC_Peak_Control->print_en)
		AFE_PRINT("AGC_Peak_Ctrl:w_r[%d], ah31[%d], ah30[%d], ah29[%d], ah28[%d], agc_peak[0x%x]\n", white_ratio, avg_histo_bin[31] ,avg_histo_bin[30] ,avg_histo_bin[29] ,avg_histo_bin[28] , agc_peak_nominal_read);

	if(white_ratio >= pCVD_AGC_Peak_Control->white_ratio_th)
	{
		if(white_frame_cnt < pCVD_AGC_Peak_Control->white_frame_max_th)
			white_frame_cnt += 2;
	}
	else
	{
		if(white_frame_cnt > 0)
			white_frame_cnt --;
	}

	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		if(white_frame_cnt >= pCVD_AGC_Peak_Control->white_frame_on)
		{
			if(g_AGC_Peak_Nominal_Value != AGC_PEAK_NOMINAL_DEFAULT_VALUE)
			{
				g_AGC_Peak_Nominal_Value = AGC_PEAK_NOMINAL_DEFAULT_VALUE;
				CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_DEFAULT_VALUE);
				if(pCVD_AGC_Peak_Control->print_en)
					AFE_PRINT("Set AGC_PEAK_NOMINAL value to [0x%x]\n", g_AGC_Peak_Nominal_Value);
			}
		}
		else if(white_frame_cnt < pCVD_AGC_Peak_Control->white_frame_off)
		{
			if(g_AGC_Peak_Nominal_Value != AGC_PEAK_NOMINAL_INITIAL_VALUE)
			{
				g_AGC_Peak_Nominal_Value = AGC_PEAK_NOMINAL_INITIAL_VALUE;
				CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_INITIAL_VALUE);
				if(pCVD_AGC_Peak_Control->print_en)
					AFE_PRINT("Set AGC_PEAK_NOMINAL value to [0x%x]\n", g_AGC_Peak_Nominal_Value);
			}
		}
	}
	return 0;
}
#endif

static int CVD_Adaptive_AGC_Peak_Control_using_motion(CVD_ADAPTIVE_PEAK_NOMINAL_CONTROL_T *pCVD_AGC_Peak_Nominal_Control )
{
	static CVD_PE0_MOTION_VALUE_T pe0_motion_value_t = {{0,}};
	/*
	static UINT32	pe_histo_bin[4][64] = {{0,}};
	UINT32 avg_histo_bin[64];
	UINT32 sum_histo_bin = 0;
	UINT32 white_ratio;
	static UINT32	white_frame_cnt = 0;
	UINT32	agc_peak_nominal_read = 0 ;
	*/
	int	loop, noise_max, noise_max_num, noise_min, noise_min_num;

	if(pCVD_AGC_Peak_Nominal_Control->bEnable == FALSE)
	{
		if(pCVD_AGC_Peak_Nominal_Control->print_en)
			AFE_PRINT("CVD_Adaptive_AGC_Peak_Control Disabled\n");

		return 0;
	}

	pe0_motion_value_t.tnr_x_avg_t[3] = pe0_motion_value_t.tnr_x_avg_t[2] ;
	pe0_motion_value_t.tnr_x_avg_t[2] = pe0_motion_value_t.tnr_x_avg_t[1] ;
	pe0_motion_value_t.tnr_x_avg_t[1] = pe0_motion_value_t.tnr_x_avg_t[0] ;
	pe0_motion_value_t.tnr_x_avg_s[3] = pe0_motion_value_t.tnr_x_avg_s[2] ;
	pe0_motion_value_t.tnr_x_avg_s[2] = pe0_motion_value_t.tnr_x_avg_s[1] ;
	pe0_motion_value_t.tnr_x_avg_s[1] = pe0_motion_value_t.tnr_x_avg_s[0] ;
	pe0_motion_value_t.tpd_s_status[3] = pe0_motion_value_t.tpd_s_status[2] ;
	pe0_motion_value_t.tpd_s_status[2] = pe0_motion_value_t.tpd_s_status[1] ;
	pe0_motion_value_t.tpd_s_status[1] = pe0_motion_value_t.tpd_s_status[0] ;
	CVD_L9Bx_Get_PE0_Motion_Value(&pe0_motion_value_t.tnr_x_avg_t[0], &pe0_motion_value_t.tnr_x_avg_s[0], &pe0_motion_value_t.tpd_s_status[0] );

	pe0_motion_value_t.avg_noise_status = 0;
	noise_max = 0;
	noise_min = 0x3FF;

	for(loop = 15; loop > 0; loop --) {
		pe0_motion_value_t.noise_status[loop] = pe0_motion_value_t.noise_status[loop-1] ;
		pe0_motion_value_t.avg_noise_status += pe0_motion_value_t.noise_status[loop];
	}
	pe0_motion_value_t.noise_status[0] = CVD_Get_Noise_Status();
	pe0_motion_value_t.avg_noise_status += pe0_motion_value_t.noise_status[0];


	for(loop = 0; loop < 16; loop ++) {
		if(pe0_motion_value_t.noise_status[loop] >= noise_max) {
			noise_max = pe0_motion_value_t.noise_status[loop];
			noise_max_num = loop;
		}
		if(pe0_motion_value_t.noise_status[loop] <= noise_min) {
			noise_min = pe0_motion_value_t.noise_status[loop];
			noise_min_num = loop;
		}
	}

	if(noise_max_num == noise_min_num) {
		pe0_motion_value_t.avg_noise_status = pe0_motion_value_t.avg_noise_status >> 4;
	}
	else {
		pe0_motion_value_t.avg_noise_status -= noise_max;
		pe0_motion_value_t.avg_noise_status -= noise_min;
		pe0_motion_value_t.avg_noise_status = pe0_motion_value_t.avg_noise_status / 14;
	}

	pe0_motion_value_t.avg_tnr_x_avg_t = (pe0_motion_value_t.tnr_x_avg_t[3] +  pe0_motion_value_t.tnr_x_avg_t[2] +  pe0_motion_value_t.tnr_x_avg_t[1] +  pe0_motion_value_t.tnr_x_avg_t[0] ) >> 2;
	pe0_motion_value_t.avg_tnr_x_avg_s = (pe0_motion_value_t.tnr_x_avg_s[3] +  pe0_motion_value_t.tnr_x_avg_s[2] +  pe0_motion_value_t.tnr_x_avg_s[1] +  pe0_motion_value_t.tnr_x_avg_s[0] ) >> 2;
	pe0_motion_value_t.avg_tpd_s_status = (pe0_motion_value_t.tpd_s_status[3] +  pe0_motion_value_t.tpd_s_status[2] +  pe0_motion_value_t.tpd_s_status[1] +  pe0_motion_value_t.tpd_s_status[0] ) >> 2;

	pCVD_AGC_Peak_Nominal_Control->reg_agc_peak_nominal = CVD_L9Bx_Get_AGC_Peak_Nominal();

	if(pCVD_AGC_Peak_Nominal_Control->print_en)
		AFE_PRINT("Adaptive_AGC_Peak : avg_t[0x%x], avg_s[0x%x], s_status[0x%x], agc_peak[0x%x], frame_cn[%d], noise[0x%x]\n", pe0_motion_value_t.avg_tnr_x_avg_t, pe0_motion_value_t.avg_tnr_x_avg_s, pe0_motion_value_t.avg_tpd_s_status, pCVD_AGC_Peak_Nominal_Control->reg_agc_peak_nominal, pCVD_AGC_Peak_Nominal_Control->pattern_frame_cnt, pe0_motion_value_t.avg_noise_status);

	if( (pe0_motion_value_t.avg_tnr_x_avg_t < pCVD_AGC_Peak_Nominal_Control->tnr_x_avg_t_th) &&
			(pe0_motion_value_t.avg_tnr_x_avg_s < pCVD_AGC_Peak_Nominal_Control->tnr_x_avg_s_th) &&
			(pe0_motion_value_t.avg_tpd_s_status >= pCVD_AGC_Peak_Nominal_Control->tpd_s_status_th) &&
			(pe0_motion_value_t.avg_noise_status <= pCVD_AGC_Peak_Nominal_Control->noise_th) )
		pCVD_AGC_Peak_Nominal_Control->pattern_update = 1;
	else
		pCVD_AGC_Peak_Nominal_Control->pattern_update = 0;

	if(pCVD_AGC_Peak_Nominal_Control->pattern_update == 1) {
		if(pCVD_AGC_Peak_Nominal_Control->pattern_frame_cnt < pCVD_AGC_Peak_Nominal_Control->pattern_frame_max_th) {
			pCVD_AGC_Peak_Nominal_Control->pattern_frame_cnt += 2;
		}
	}
	else {
		if(pCVD_AGC_Peak_Nominal_Control->pattern_frame_cnt > 0) {
			pCVD_AGC_Peak_Nominal_Control->pattern_frame_cnt --;
		}
	}

	// Test Pattern Found !!!
	if(pCVD_AGC_Peak_Nominal_Control->pattern_frame_cnt >= pCVD_AGC_Peak_Nominal_Control->pattern_frame_on) {
		if(pCVD_AGC_Peak_Nominal_Control->pattern_found < 1) {
				pCVD_AGC_Peak_Nominal_Control->pattern_found = 1;
				CVD_DEBUG("Adaptive AGC : Pattern Found !!!\n");
#ifdef L9_ADAPTIVE_AGC_PEAK_NOMINAL_CONTROL		//disabled
				if(g_CVD_Color_System_Support == LX_COLOR_SYSTEM_NTSC_M) {
				// Control AGC Peak Nominal, Contrast & Brightness only for NTSC system
					CVD_DEBUG("AGC Peak Nomial to default value\n");
					CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_DEFAULT_VALUE);
					CVD_L9Bx_Set_Contrast_Brightness(0x82, 0x20);	// NTSC Default Value
				}
#endif
		}
	}
	// Normal Condition !!!
	else if(pCVD_AGC_Peak_Nominal_Control->pattern_frame_cnt < pCVD_AGC_Peak_Nominal_Control->pattern_frame_off) {
		if(pCVD_AGC_Peak_Nominal_Control->pattern_found > 0 ) {
				pCVD_AGC_Peak_Nominal_Control->pattern_found = 0;
				CVD_DEBUG("Adaptive AGC : Pattern NOT Found !!! \n");
#ifdef L9_ADAPTIVE_AGC_PEAK_NOMINAL_CONTROL		//disabled
				if(g_CVD_Color_System_Support == LX_COLOR_SYSTEM_NTSC_M) {
				// Control AGC Peak Nominal, Contrast & Brightness only for NTSC system
					CVD_DEBUG("AGC Peak Nomial set to new NTSC value\n");
					CVD_L9Bx_Set_AGC_Peak_Nominal(AGC_PEAK_NOMINAL_INITIAL_VALUE);
					CVD_L9Bx_Set_Contrast_Brightness(0x7a, 0x1f);	// Value for  new reg_agc_peak_nominal
				}
#endif
		}
	}

	return 0;
}
#endif

static int CVD_AGC_Peak_En_Control(int force_peak_enable, int avg_noise, int avg_agc_gain)
{
	CVD_DEBUG("%s entered :  En[%d],noise[0x%x],gain[0x%x] \n",__func__, force_peak_enable, avg_noise, avg_agc_gain);
	if (lx_chip_rev() >= LX_CHIP_REV(L9, B0)) {
		if (force_peak_enable == 2) {
			CVD_L9Bx_Set_AGC_Peak_En(1);
		}
		else if (force_peak_enable == 0) {
			CVD_L9Bx_Set_AGC_Peak_En(0);
		}
		else if (force_peak_enable == 1) {
			if ( (avg_noise > NOISE_TH_FOR_PEAK_EN_CONTROL) && (avg_agc_gain > AGC_GAIN_TH_FOR_PEAK_EN_CONTROL)) {
				CVD_DEBUG("##### AGC PEAK EN Disabled !!!!! ##### \n");
				CVD_L9Bx_Set_AGC_Peak_En(0);
			}
			else {
//				CVD_DEBUG("##### AGC PEAK EN Enabled !!!!! ##### \n");
				CVD_L9Bx_Set_AGC_Peak_En(1);
			}
		}

	}
	return 0;
}

static int CVD_NoBurst_CKill_Control(CVD_STATUS_3CS_T *pCVD_Status)
{

	static int no_color_count = 0;

	/*
	CVD_DEBUG("%s entered :  chroma_lock[%d],h_lock[%d],v_lock[%d], hnon_standard[%d], vnon_standard[%d], \n",\
			__func__, pCVD_Status->cvd_chromalock_flag, pCVD_Status->h_lock_flag, pCVD_Status->v_lock_flag, pCVD_Status->hnon_standard_flag, pCVD_Status->vnon_standard_flag);
			*/

	if( (pCVD_Status->cvd_chromalock_flag == 0) /* && (pCVD_Status->cs0_chromalock_flag == 0) && (pCVD_Status->cs1_chromalock_flag == 0) */ \
			&& (pCVD_Status->h_lock_flag == 1) && (pCVD_Status->v_lock_flag == 1)\
			&& ( ( (pCVD_Status->cvd_color_system == CVD_STATE_PALi) && (pCVD_Status->cvd_secam_flag == 0) && (pCVD_Status->cvd_pal_flag == 0)) || (pCVD_Status->cvd_color_system == CVD_STATE_NTSC) ) \
	/*		&& (pCVD_Status->hnon_standard_flag == 1) */&& (pCVD_Status->vnon_standard_flag == 0)
			&& (CVD_L9Bx_Get_CVD_Burst_Mag_Value() < 0x600))
	{
		if(no_color_count > 5) {
		}
		else if(no_color_count == 5) {
			CVD_DEBUG("No Burst Detected !!!\n");

			if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
				CVD_L9Bx_Set_Noburst_Ckill(1);

			gCVD_Status_3CS.no_color_detected = 1;
			no_color_count ++;
		}
		else
			no_color_count ++;
	}
	else {
		if(gCVD_Status_3CS.no_color_detected == 1) {
			CVD_DEBUG("Burst Detected !\n");

			if (lx_chip_rev() >= LX_CHIP_REV(L9, B0))
				CVD_L9Bx_Set_Noburst_Ckill(0);
		}

		gCVD_Status_3CS.no_color_detected = 0;
		no_color_count = 0;
	}

	return 0;
}

static int CVD_Pattern_Detection(CVD_PATTERN_DETECTION_T *pCVD_Pattern_Detection_t )
{
	static CVD_PE0_MOTION_VALUE_T pe0_motion_value_t = {{0,}};

	int s_status_diff ;

	if (lx_chip_rev() < LX_CHIP_REV(L9, B0))
		return -1;

	pe0_motion_value_t.tnr_x_avg_t[3] = pe0_motion_value_t.tnr_x_avg_t[2] ;
	pe0_motion_value_t.tnr_x_avg_t[2] = pe0_motion_value_t.tnr_x_avg_t[1] ;
	pe0_motion_value_t.tnr_x_avg_t[1] = pe0_motion_value_t.tnr_x_avg_t[0] ;
	pe0_motion_value_t.tnr_x_avg_s[3] = pe0_motion_value_t.tnr_x_avg_s[2] ;
	pe0_motion_value_t.tnr_x_avg_s[2] = pe0_motion_value_t.tnr_x_avg_s[1] ;
	pe0_motion_value_t.tnr_x_avg_s[1] = pe0_motion_value_t.tnr_x_avg_s[0] ;
	pe0_motion_value_t.tpd_s_status[3] = pe0_motion_value_t.tpd_s_status[2] ;
	pe0_motion_value_t.tpd_s_status[2] = pe0_motion_value_t.tpd_s_status[1] ;
	pe0_motion_value_t.tpd_s_status[1] = pe0_motion_value_t.tpd_s_status[0] ;

	CVD_L9Bx_Get_PE0_Motion_Value(&pe0_motion_value_t.tnr_x_avg_t[0], &pe0_motion_value_t.tnr_x_avg_s[0], &pe0_motion_value_t.tpd_s_status[0] );

	pe0_motion_value_t.avg_tnr_x_avg_t = (pe0_motion_value_t.tnr_x_avg_t[3] +  pe0_motion_value_t.tnr_x_avg_t[2] +  pe0_motion_value_t.tnr_x_avg_t[1] +  pe0_motion_value_t.tnr_x_avg_t[0] ) >> 2;
	pe0_motion_value_t.avg_tnr_x_avg_s = (pe0_motion_value_t.tnr_x_avg_s[3] +  pe0_motion_value_t.tnr_x_avg_s[2] +  pe0_motion_value_t.tnr_x_avg_s[1] +  pe0_motion_value_t.tnr_x_avg_s[0] ) >> 2;
	pe0_motion_value_t.avg_tpd_s_status = (pe0_motion_value_t.tpd_s_status[3] +  pe0_motion_value_t.tpd_s_status[2] +  pe0_motion_value_t.tpd_s_status[1] +  pe0_motion_value_t.tpd_s_status[0] ) >> 2;

//	CVD_DEBUG("PE0 : avg_t[0x%x], avg_s[0x%x], s_status[0x%x] \n", pe0_motion_value_t.avg_tnr_x_avg_t, pe0_motion_value_t.avg_tnr_x_avg_s, pe0_motion_value_t.avg_tpd_s_status);

	pCVD_Pattern_Detection_t->global_motion_value = CVD_L9Bx_Get_Global_Motion_Value();

	pCVD_Pattern_Detection_t->global_motion_diff = pCVD_Pattern_Detection_t->prev_motion_value - pCVD_Pattern_Detection_t->global_motion_value;

	if( pCVD_Pattern_Detection_t->pattern_found == 1) {
		s_status_diff = 0x60;
	}
	else
		s_status_diff = 0;

	if( ( abs(pCVD_Pattern_Detection_t->global_motion_diff) <= pCVD_Pattern_Detection_t->motion_diff_th) \
			&& (pe0_motion_value_t.avg_tnr_x_avg_t < pCVD_Pattern_Detection_t->tnr_x_avg_t_th) \
			&& (pe0_motion_value_t.avg_tnr_x_avg_s < pCVD_Pattern_Detection_t->tnr_x_avg_s_th) \
			&& (pe0_motion_value_t.avg_tpd_s_status > ( pCVD_Pattern_Detection_t->tpd_s_status_th - s_status_diff) ) )
		//	&& (pe0_motion_value_t.avg_tpd_s_status > pCVD_Pattern_Detection_t->tpd_s_status_th) )
	{ 	// no global motion ??? pattern ???
		if(pCVD_Pattern_Detection_t->global_motion_count < pCVD_Pattern_Detection_t->motion_count_max)  {
			pCVD_Pattern_Detection_t->global_motion_count++;
			CVD_DEBUG("Pattern diff [%d],  avg_t[0x%x], avg_s[0x%x], s_status[0x%x], count[%d]\n", pCVD_Pattern_Detection_t->global_motion_diff, pe0_motion_value_t.avg_tnr_x_avg_t, pe0_motion_value_t.avg_tnr_x_avg_s, pe0_motion_value_t.avg_tpd_s_status, pCVD_Pattern_Detection_t->global_motion_count);
		}
	}
	else {
		if( pCVD_Pattern_Detection_t->pattern_found == 1) {
			pCVD_Pattern_Detection_t->global_motion_count --;
			CVD_DEBUG("Pattern Not Found : diff [%d],  avg_t[0x%x], avg_s[0x%x], s_status[0x%x],  count[%d]\n", pCVD_Pattern_Detection_t->global_motion_diff,  pe0_motion_value_t.avg_tnr_x_avg_t,pe0_motion_value_t.avg_tnr_x_avg_s, pe0_motion_value_t.avg_tpd_s_status, pCVD_Pattern_Detection_t->global_motion_count);
		}
		else
			pCVD_Pattern_Detection_t->global_motion_count = 0;
	}

	if( (pCVD_Pattern_Detection_t->global_motion_count > pCVD_Pattern_Detection_t->motion_count_th) && (pCVD_Pattern_Detection_t->pattern_found == 0) ) {
		pCVD_Pattern_Detection_t->pattern_found = 1;
		CVD_DEBUG("Pattern Detected (global motion)\n");
	}
	else if( (pCVD_Pattern_Detection_t->global_motion_count < pCVD_Pattern_Detection_t->motion_count_th) &&  (pCVD_Pattern_Detection_t->pattern_found == 1) ) {
		pCVD_Pattern_Detection_t->global_motion_count = 0;
		pCVD_Pattern_Detection_t->pattern_found = 0;
		CVD_DEBUG("Pattern Not Detected (global motion)\n");
	}

	pCVD_Pattern_Detection_t->prev_motion_value = pCVD_Pattern_Detection_t->global_motion_value;

	return 0;

}

