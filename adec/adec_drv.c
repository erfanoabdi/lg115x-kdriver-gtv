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
 *  main driver implementation for adec device.
 *	adec device will teach you how to make device driver with new platform.
 *
 *  author		jongsang.oh (jongsang.oh@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_adec
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_ADEC_DEVICE_READ_WRITE_FOPS
//#define SUPPORT_ADEC_UNLOCKED_IOCTL


/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/interrupt.h>    /**< For isr */
#include <linux/irq.h>			/**< For isr */
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <linux/rmap.h>
#include <linux/kthread.h>
#include <asm/io.h>			/**< For ioremap_nocache */
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <linux/poll.h>

#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif

#include "os_util.h"
#include "base_dev_cfg.h"
#include "adec_cfg.h"
#include "adec_drv.h"

#ifdef INCLUDE_L8_CHIP_KDRV
#include "l8/adec_coredrv_l8.h"
#else
#include "l9/adec_coredrv_l9.h"
#endif


/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/**
 *	main control block for adec device.
 *	each minor device has unique control block
 *
 */
typedef struct
{
// BEGIN of common device
	int						dev_open_count;	///< check if device is opened or not
	dev_t					devno;			///< device number
	struct cdev				cdev;			///< char device structure
// END of command device

// BEGIN of device specific data
	bool					is_initialized; //check if device is initialized

// END of device specific data
}
ADEC_DEVICE_T;

#ifdef KDRV_CONFIG_PM
typedef struct
{
	// add here extra parameter
	bool	is_suspended;
}ADEC_DRVDATA_T;
#endif

#define NUM_OF_ADEC_PRINT		10

/* ADEC Debug Print Type */
UINT8 *ui8AdecPrintType[NUM_OF_ADEC_PRINT]  =
							  { "ADEC_PRINT", 	  "ADEC_TRACE", 	"ADEC_ISR_DEBUG",	\
								"ADEC_MIP_DEBUG", "ADEC_ERROR", 	"ADEC_DEBUG",	\
								"ADEC_DEBUG_TMP", "ADEC_IPC_DEBUG", "ADEC_IPC_MSG",	\
								"ADEC_MIP_I2C", };

/* ADEC Debug Print Color */
UINT8 *ui8AdecPrintColor[8] = {
			"COLOR_BLACK",
			"COLOR_RED",
			"COLOR_GREEN",
			"COLOR_YELLOW",
			"COLOR_BLUE",
			"COLOR_PURPLE",
			"COLOR_CYAN",
			"COLOR_GRAY",
};


/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	ADEC_PROC_Init(void);
extern	void	ADEC_PROC_Cleanup(void);


/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
//For poll function
DECLARE_WAIT_QUEUE_HEAD(adec_poll_wait_queue);
extern unsigned int	g_ui32RecEventType;

extern spinlock_t 			gADEC_DSP_event_lock;
extern LX_ADEC_INIT_TYPE_T	g_eAdecInitType;

//ADEC KDRV Structure
extern LX_ADEC_KDRV_T g_adec_kdrv;


/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		ADEC_Init(void);
void	ADEC_Cleanup(void);


/*----------------------------------------------------------------------------------------
	Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int (*ADEC_DRV_InitModule) ( LX_ADEC_INIT_TYPE_T eInitType );
int (*ADEC_DRV_SetEvent) ( LX_ADEC_EVENT_TYPE_T eEventType );
int (*ADEC_DRV_ResetEvent) ( LX_ADEC_EVENT_TYPE_T eEventType );
int (*ADEC_DRV_GetEvent) ( LX_ADEC_EVENT_INFO_T *pstEventInfo );
int (*ADEC_DRV_GetBufferInfo) ( LX_ADEC_BUF_INFO_T *pstBufferInfo );
int (*ADEC_DRV_GetDecodingInfo) ( LX_ADEC_DEC_INFO_T *pstDecodingInfo );
int (*ADEC_DRV_ResetAADModule) ( void );
int (*ADEC_DRV_SetAADBitShift) ( UINT32 ui32BitShift );

int (*ADEC_DRV_SetSource) ( LX_ADEC_SET_SOURCE_T stSetSource );
int (*ADEC_DRV_StartDecoding) ( LX_ADEC_START_DEC_T stStartInfo );
int (*ADEC_DRV_StopDecoding) ( LX_ADEC_STOP_DEC_T stStopInfo );
int (*ADEC_DRV_GetESExistence) ( LX_ADEC_ES_EXIST_T	*pstEsExist );
int (*ADEC_DRV_GetESInfo) ( LX_ADEC_GET_ES_INFO_T *pstESInfo );
int (*ADEC_DRV_StartADDecoding) ( LX_ADEC_SRC_TYPE_T eSrcType );
int (*ADEC_DRV_StopADDecoding) ( void );
int (*ADEC_DRV_SetSPKOutMode) ( LX_ADEC_OUTPUT_MODE_T eSPKOutMode );
int (*ADEC_DRV_SetDecoderMode) ( LX_ADEC_DECODER_MODE_T i_eDecoderMode );

int (*ADEC_DRV_SetVolume) ( UINT32 ui32Volume );
int (*ADEC_DRV_SetADVolume) ( UINT32 ui32Volume );
int (*ADEC_DRV_SetDACOutVolume) ( LX_ADEC_DAC_VOL_T stDACOutVol );
int (*ADEC_DRV_SetOutputPortMute) ( LX_ADEC_OUTPUT_PORT_MUTE_T stOutPortMute );
int (*ADEC_DRV_SetMixVolume) ( LX_ADEC_MIX_VOL_T stMixVolume );
int (*ADEC_DRV_SetDAMPVolume) ( UINT8 ui8Volume );
int (*ADEC_DRV_SetDAMPMute) ( BOOLEAN bMuteFlag );
int (*ADEC_DRV_SetMixChannelVolume) ( LX_ADEC_MIX_CH_VOL_T stMixChVolume );

int (*ADEC_DRV_SetSPDIFOutputType) ( LX_ADEC_SPDIF_MODE_T eSPDIFMode );
int (*ADEC_DRV_SetSPDIFScmsCtrl) ( LX_ADEC_SPDIF_SCMS_T eSPDIFScmsCtrl );
int (*ADEC_DRV_SetSPDIFVolume) ( UINT32 ui32Volume );
int (*ADEC_DRV_SetSPDIFMute) ( BOOLEAN bMuteFlag );

int (*ADEC_DRV_EnableAVLipSync) ( LX_ADEC_SYNC_T i_eAVLipSync );
int (*ADEC_DRV_SetI2SOutDelayTime) ( UINT32 ui32DelayinMS );
int (*ADEC_DRV_SetDACOutDelayTime) ( UINT32 ui32DelayinMS );
int (*ADEC_DRV_SetSPDIFDelayTime) ( UINT32 ui32DelayinMS );
int (*ADEC_DRV_SetTrickState) ( LX_ADEC_TRICK_MODE_T	eTrickMode );
int (*ADEC_DRV_GetClockInfo) ( LX_ADEC_CLOCK_INFO_T *pstClockInfo );

int (*ADEC_DRV_SetBalance) ( UINT8 ui8BalanceVal );
int (*ADEC_DRV_SetBASS) ( UINT8 ui8BassVal );
int (*ADEC_DRV_SetTreble) ( UINT8 ui8TrebleVal );

int (*ADEC_DRV_SESetFunction) ( LX_ADEC_SE_FN_PARAM_T *pstSeFnParam );
int (*ADEC_DRV_SEGetFunction) ( LX_ADEC_SE_FN_PARAM_T *pstSeFnParam );

int (*ADEC_DRV_FeedAudioClip) ( LX_ADEC_CLIP_MEM_INFO_T stMemInfo );
int (*ADEC_DRV_StartAudioClip) ( LX_ADEC_CLIP_START_INFO_T stStartInfo );
int (*ADEC_DRV_StopAudioClip) ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int (*ADEC_DRV_PauseAudioClip) ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int (*ADEC_DRV_ResumeAudioClip) ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int (*ADEC_DRV_FlushAudioClip) ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int (*ADEC_DRV_ResetAudioClip) ( LX_ADEC_DEC_MODE_T i_eAdecMode );
int (*ADEC_DRV_SetAVClockControl) ( LX_ADEC_AV_CLOCK_INFO_T	i_stAVClockInfo );

int (*ADEC_DRV_GetEncBufInfo) ( LX_ADEC_ENC_BUF_T *pstEncBufInfo );
int (*ADEC_DRV_SetEncParam) ( LX_ADEC_ENC_PARAM_T stEncParam );
int (*ADEC_DRV_GetEncParam) ( LX_ADEC_ENC_PARAM_T *pstEncParam );
int (*ADEC_DRV_StartEncoding) ( LX_ADEC_ENC_START_T stStartInfo );
int (*ADEC_DRV_StopEncoding) ( LX_ADEC_ENC_STOP_T stStopInfo );
int (*ADEC_DRV_GetEncodingInfo) ( LX_ADEC_ENC_INFO_T *pstEncodingInfo );

int (*ADEC_DRV_GetPcmBufInfo) ( LX_ADEC_PCM_BUF_T *pstPcmBufInfo );
int (*ADEC_DRV_GetCapturingInfo) ( LX_ADEC_CAP_INFO_T *pstCapturingInfo );

int (*ADEC_DRV_ReadAndWriteReg) ( LX_ADEC_REG_INFO_T *pstRegInfo );
int (*ADEC_DRV_ShowStatusRegister) ( void);
int (*ADEC_DRV_CustomIpc) ( LX_ADEC_CUSTOM_IPC_T* pstCustomIpc );

int (*ADEC_Monitor_Task) ( void *i_pvParam );

irqreturn_t (*adec_interrupt0) (int irq, void *dev_id, struct pt_regs *regs);
irqreturn_t (*adec_interrupt1) (int irq, void *dev_id, struct pt_regs *regs);

#ifdef	KDRV_CONFIG_PM
int (*ADEC_RunSuspend)( void );
int (*ADEC_RunResume)( void );
#endif


/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_adec_debug_fd;
int 	g_adec_major = ADEC_MAJOR;
int 	g_adec_minor = ADEC_MINOR;

struct task_struct 	*g_pstADECMonitorTask = NULL;


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      ADEC_Open(struct inode *, struct file *);
static int      ADEC_Close(struct inode *, struct file *);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_ADEC_UNLOCKED_IOCTL)
static int 		ADEC_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#else
static long		ADEC_Ioctl (struct file *, unsigned int, unsigned long );
#endif
static int		ADEC_Mmap( struct file *filp , struct vm_area_struct *vma );
static unsigned int ADEC_Poll(struct file *filp, poll_table *wait);

#ifdef SUPPORT_ADEC_DEVICE_READ_WRITE_FOPS
static ssize_t  ADEC_Read(struct file *, char *, size_t, loff_t *);
static ssize_t  ADEC_Write(struct file *, const char *, size_t, loff_t *);
#endif

static int ADEC_InitFunction(void);


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/**
 * main control block for adec device
*/
static ADEC_DEVICE_T*		g_adec_device;

/**
 * file I/O description for adec device
 *
*/
static struct file_operations g_adec_fops =
{
	.open 	= ADEC_Open,
	.release= ADEC_Close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_ADEC_UNLOCKED_IOCTL)
	.ioctl	= ADEC_Ioctl,
#else
	.unlocked_ioctl	= ADEC_Ioctl,
#endif
	.mmap 	= ADEC_Mmap,
	.poll 	= ADEC_Poll,
#ifdef SUPPORT_ADEC_DEVICE_READ_WRITE_FOPS
	.read 	= ADEC_Read,
	.write 	= ADEC_Write,
#else
	.read	= NULL,
	.write	= NULL,
#endif
};


/*========================================================================================
	Implementation Group
========================================================================================*/
#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
/**
 *
 * suspending module.
 *
 * @param	struct platform_device *pdev pm_message_t state
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int ADEC_suspend(struct platform_device *pdev, pm_message_t state)
{
	ADEC_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	// add here the suspend code
	if(drv_data->is_suspended == 1) return -1;

	// run to suspend mode
	ADEC_RunSuspend();

	drv_data->is_suspended = 1;
	ADEC_DEBUG("[%s] done suspend\n", ADEC_MODULE);

	return 0;
}


/**
 *
 * resuming module.
 *
 * @param	struct platform_device *
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int ADEC_resume(struct platform_device *pdev)
{
	ADEC_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;

	// run to resume mode
	ADEC_RunResume();

	drv_data->is_suspended = 0;
	ADEC_DEBUG("[%s] done resume!!!\n", ADEC_MODULE);
	return 0;
}

/**
 *
 * probing module.
 *
 * @param	struct platform_device *pdev
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
 int ADEC_probe(struct platform_device *pdev)
{

	ADEC_DRVDATA_T *drv_data;

	drv_data = (ADEC_DRVDATA_T *)kmalloc(sizeof(ADEC_DRVDATA_T), GFP_KERNEL);

	// add here driver registering code & allocating resource code

	ADEC_PRINT("[%s] done probe\n", ADEC_MODULE);
	drv_data->is_suspended = 0;
	platform_set_drvdata(pdev, drv_data);

	return 0;
}


/**
 *
 * module remove function. this function will be called in rmmod fbdev module.
 *
 * @param	struct platform_device
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int  ADEC_remove(struct platform_device *pdev)
{
	ADEC_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code

	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	ADEC_PRINT("[%s] removed\n", ADEC_MODULE );
	return 0;
}

/**
 *
 * module release function. this function will be called in rmmod module.
 *
 * @param	struct device *dev
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static void  ADEC_release(struct device *dev)
{
	ADEC_PRINT("[%s] device released\n", ADEC_MODULE);
}

/*
 *	module platform driver structure
 */
static struct platform_driver adec_driver =
{
	.probe          = ADEC_probe,
	.suspend        = ADEC_suspend,
	.remove         = ADEC_remove,
	.resume         = ADEC_resume,
	.driver         =
	{
		.name   = ADEC_MODULE,
	},
};

static struct platform_device adec_device = {
	.name = ADEC_MODULE,
	.id = 0,
	.id = -1,
	.dev = {
		.release = ADEC_release,
	},
};
#endif

int ADEC_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for adec device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_adec_debug_fd = DBG_OPEN( ADEC_MODULE );

	if ( g_adec_debug_fd < 0 )
	{
		DBG_PRINT_ERROR("debug system shutdowned.\n" );
		return -EIO;
	}

	OS_DEBUG_EnableModule ( g_adec_debug_fd );

#if	1	//temp
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 0);					//ADEC_PRINT
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 1);					//ADEC_TRACE
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 2);					//ADEC_ISR_DEBUG
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 3);					//ADEC_MIP_DEBUG
	OS_DEBUG_EnableModuleByIndex (g_adec_debug_fd, 4, DBG_COLOR_NONE);	//ADEC_ERROR
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 5);					//ADEC_DEBUG
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 6);					//ADEC_DEBUG_TMP
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 7);					//ADEC_IPC_DEBUG
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 8);					//ADEC_IPC_MSG
	OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, 9);					//ADEC_MIP_I2C
#else	//all enable for debug
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 0, DBG_COLOR_BLUE );	//ADEC_PRINT
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 1, DBG_COLOR_BLUE );	//ADEC_TRACE
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 2, DBG_COLOR_BLUE );	//ADEC_ISR_DEBUG
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 3, DBG_COLOR_BLUE );	//ADEC_MIP_DEBUG
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 4, DBG_COLOR_RED );	//ADEC_ERROR
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 5, DBG_COLOR_BLUE );	//ADEC_DEBUG
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 6, DBG_COLOR_BLUE );	//ADEC_DEBUG_TMP
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 7, DBG_COLOR_BLUE );	//ADEC_IPC_DEBUG
	OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, 8, DBG_COLOR_CYAN);	//ADEC_IPC_MSG
#endif	//#if	1

#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&adec_driver) < 0)
	{
		ADEC_DEBUG("[%s] platform driver register failed\n",ADEC_MODULE);

	}
	else
	{
		if(platform_device_register(&adec_device))
		{
			platform_driver_unregister(&adec_driver);
			ADEC_DEBUG("[%s] platform device register failed\n",ADEC_MODULE);
		}
		else
		{
			ADEC_DEBUG("[%s] platform register done\n", ADEC_MODULE);
		}
	}
#endif

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
	g_adec_device = (ADEC_DEVICE_T*)OS_KMalloc( sizeof(ADEC_DEVICE_T)*ADEC_MAX_DEVICE );
	if ( NULL == g_adec_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(ADEC_DEVICE_T)* ADEC_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_adec_device, 0x0, sizeof(ADEC_DEVICE_T)* ADEC_MAX_DEVICE );
	if (g_adec_major)
	{
		dev = MKDEV( g_adec_major, g_adec_minor );
		err = register_chrdev_region(dev, ADEC_MAX_DEVICE, ADEC_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_adec_minor, ADEC_MAX_DEVICE, ADEC_MODULE );
		g_adec_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register adec device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */


	/* END */

	for ( i=0; i<ADEC_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_adec_major, g_adec_minor+i );
		cdev_init( &(g_adec_device[i].cdev), &g_adec_fops );
		g_adec_device[i].devno		= dev;
		g_adec_device[i].cdev.owner = THIS_MODULE;
		g_adec_device[i].cdev.ops   = &g_adec_fops;

		/* TODO: initialize minor device */


		/* END */
		err = cdev_add (&(g_adec_device[i].cdev), dev, 1 );
		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding adec device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}

		/* Support udev for GCD platform */
        OS_CreateDeviceClass ( g_adec_device[i].devno, "%s%d", ADEC_MODULE, i );
	}

	//Initialize Audio Decoder(DSP) register base
	/* Check LX Chip Revision Number */
	if(lx_chip_rev() >= LX_CHIP_REV(L9, B0))
	{
		ADEC_DEBUG_TMP ("LX_CHIP_REV(L9, BX)(0x%X)\n", lx_chip_rev());

		pstAdecRegCfg = &gstAdecRegCfg[2];
		pstMemCfgADEC = &gstMemCfgADEC[0];

		ADEC_DEBUG("reg_base_addr = 0x%X, size = 0x%X\n", pstAdecRegCfg->reg_base_addr, pstAdecRegCfg->reg_size);

		gpRealRegAUD = (volatile AUD_REG_T *)ioremap(pstAdecRegCfg->reg_base_addr, pstAdecRegCfg->reg_size);
		gpRegAUD     = (AUD_REG_T *)kmalloc(sizeof(AUD_REG_T), GFP_KERNEL);

		if(gpRealRegAUD == NULL || gpRegAUD == NULL)
		{
			DBG_PRINT_ERROR("ERROR : can't allocate for register\n");
			return -EIO;
		}
		ADEC_DEBUG("gpRealRegAUD = 0x%p, gpRegAUD = 0x%p\n", gpRealRegAUD, gpRegAUD);

		//Initializes the clock and reset module register value.
		(void)MIXED_IP_ADEC_L9_InitClockRegisters();

	}
	else if(lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{
		ADEC_DEBUG_TMP ("LX_CHIP_REV(L9, AX)(0x%X)\n", lx_chip_rev());

		pstAdecRegCfg = &gstAdecRegCfg[1];
		pstMemCfgADEC = &gstMemCfgADEC[0];

		ADEC_DEBUG("reg_base_addr = 0x%X, size = 0x%X\n", pstAdecRegCfg->reg_base_addr, pstAdecRegCfg->reg_size);

		gpRealRegAUD = (volatile AUD_REG_T *)ioremap(pstAdecRegCfg->reg_base_addr, pstAdecRegCfg->reg_size);
		gpRegAUD	 = (AUD_REG_T *)kmalloc(sizeof(AUD_REG_T), GFP_KERNEL);

		if(gpRealRegAUD == NULL || gpRegAUD == NULL)
		{
			DBG_PRINT_ERROR("ERROR : can't allocate for register\n");
			return -EIO;
		}
		ADEC_DEBUG("gpRealRegAUD = 0x%p, gpRegAUD = 0x%p\n", gpRealRegAUD, gpRegAUD);

		//Initializes the clock and reset module register value.
		(void)MIXED_IP_ADEC_L9_InitClockRegisters();

	}
#ifdef INCLUDE_L8_CHIP_KDRV
	else if(lx_chip_rev() == LX_CHIP_REV(L8, B0) || lx_chip_rev() == LX_CHIP_REV(L8, A1))
	{
		ADEC_DEBUG_TMP ("LX_CHIP_REV(L8, XX)(0x%X)\n", lx_chip_rev());

		pstAdecRegCfg = &gstAdecRegCfg[0];
		pstMemCfgADEC = &gstMemCfgADEC[0];

		ADEC_DEBUG("reg_base_addr = 0x%X, size = 0x%X\n", pstAdecRegCfg->reg_base_addr, pstAdecRegCfg->reg_size);

		g_pstADECReg_l8 = (volatile AUD_REG_L8_T *)ioremap(pstAdecRegCfg->reg_base_addr, pstAdecRegCfg->reg_size);
		gpRealRegAUD_l8 = (volatile AUD_REG_L8_T *)ioremap(pstAdecRegCfg->reg_base_addr, pstAdecRegCfg->reg_size);
		gpRegAUD_l8     = (AUD_REG_L8_T *)kmalloc(sizeof(AUD_REG_L8_T), GFP_KERNEL);
		ADEC_DEBUG("g_pstADECReg_l8  = 0x%p, gpRealRegAUD_l8 = 0x%p, gpRegAUD_l8 = 0x%p\n", g_pstADECReg_l8, gpRealRegAUD_l8, gpRegAUD_l8);

		if(gpRealRegAUD_l8 == NULL)
		{
			DBG_PRINT_ERROR("ERROR : can't allocate for register\n");
			return -EIO;
		}

		//Set D-AMP clock for 12.288Mhz
		if(lx_chip_rev() >= LX_CHIP_REV(L8, B0))
		{
			/* Set Clock Path CTR17, CTR18 */
			CTOP_CTRL_WRITE(0x54, 0x00040000);
			CTOP_CTRL_WRITE(0x58, 0x00000000);

			/* Set Clock Division CTR30, CTR31 */
			CTOP_CTRL_WRITE(0x88, 0x00000000);
			CTOP_CTRL_WRITE(0x8C, 0xD42A8000);

			ADEC_PRINT("ADEC_Init : LX_ADEC_CLOCK_DTV\n");
		}
	}
#endif	//#ifdef INCLUDE_L8_CHIP_KDRV
	else
	{
		DBG_PRINT_ERROR("LX_CHIP_REV => Unknown(0x%X) : ERROR\n", lx_chip_rev());
		return -EIO;
	}

	/* initialize adec function pointer */
	err = ADEC_InitFunction();
	if (err)
	{
		DBG_PRINT_ERROR("error (%d) while initialize adec function\n", err);
		return -EIO;
	}

	/* initialize proc system */
	ADEC_PROC_Init ( );

	ADEC_PRINT("adec device initialized\n");
	return 0;
}

void ADEC_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_adec_major, g_adec_minor );

#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	platform_driver_unregister(&adec_driver);
	platform_device_unregister(&adec_device);
#endif

	/* cleanup proc system */
	ADEC_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<ADEC_MAX_DEVICE;i++)
	{
		/* TODO: cleanup each minor device */

		/* END */
		cdev_del( &(g_adec_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */

	unregister_chrdev_region(dev, ADEC_MAX_DEVICE );

	OS_Free( g_adec_device );
	ADEC_DEBUG("adec device cleanup\n");
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for adec device
 *
 */
static int
ADEC_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    ADEC_DEVICE_T*		my_dev;
	int					iErrReturn = 0;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, ADEC_DEVICE_T, cdev);

	/* TODO : add your device specific code */
	if ( my_dev->dev_open_count == 0 )
	{
		//reset initialized value
		my_dev->is_initialized = 0;

		//Initialize IRQ0 of ADEC DSP0
		iErrReturn = request_irq(pstAdecRegCfg->irq0_num, (irq_handler_t)adec_interrupt0, 0, "ADEC0 ", NULL);
		if (iErrReturn)
		{
			ADEC_ERROR("request_irq IRQ_AUD0 in %s is failed %d\n", "ADEC0 ", iErrReturn);
			return -1;
		}
		ADEC_PRINT("adec irq0 registered :%d:\n", pstAdecRegCfg->irq0_num );

		//Initialize IRQ1 of ADEC DSP1
		iErrReturn = request_irq(pstAdecRegCfg->irq1_num, (irq_handler_t)adec_interrupt1, 0, "ADEC1 ", NULL);
		if (iErrReturn)
		{
			ADEC_ERROR("request_irq IRQ_AUD1 in %s is failed %d\n", "ADEC1 ", iErrReturn);
			free_irq(pstAdecRegCfg->irq0_num, NULL);
			return -1;
		}
		ADEC_PRINT("adec irq1 registered :%d:\n", pstAdecRegCfg->irq1_num);

		//Create thread for ADEC module
		g_pstADECMonitorTask = kthread_run( ADEC_Monitor_Task, NULL, "ADECMTASK" );

		if ( g_pstADECMonitorTask )
		{
			ADEC_PRINT("ADEC> Init - Monitor task created successfully\n");
		}
		else
		{
			ADEC_ERROR("ERROR> ADEC Monitor task creation failed!!!\n");
		}
	}
	/* END */

	/* Set device open count */
    my_dev->dev_open_count++;
    filp->private_data = my_dev;

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    ADEC_PRINT("device opened (%d:%d)\n", major, minor );
    return 0;
}

/**
 * release handler for adec device
 *
 */
static int
ADEC_Close(struct inode *inode, struct file *file)
{
    int				major,minor;
    ADEC_DEVICE_T*	my_dev;
    struct cdev*	cdev;

	unsigned long	flags = 0;
	unsigned int	ui32RecEventType = 0;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, ADEC_DEVICE_T, cdev);

	/* Decrease device open count */
    if ( my_dev->dev_open_count > 0 )
    {
        --my_dev->dev_open_count;
    }

	/* TODO : add your device specific code */
	if ( my_dev->dev_open_count <= 0 )
	{
		//Release IRQ0 of ADEC
		free_irq(pstAdecRegCfg->irq0_num, NULL);
		ADEC_PRINT("free_irq IRQ_AUD0 released :%d:\n", pstAdecRegCfg->irq0_num);

		//Release IRQ1 of ADEC
		free_irq(pstAdecRegCfg->irq1_num, NULL);
		ADEC_PRINT("free_irq IRQ_AUD1 released :%d:\n", pstAdecRegCfg->irq1_num);

		//Release monitor thread
		if ( g_pstADECMonitorTask )
		{
			kthread_stop( g_pstADECMonitorTask );
			ADEC_PRINT("kthread_stop ADEC Monitor task\n");
		}
	}
	/* END */

	//To support adec shared memory decoder close when abnormal process stops using kill command in GCD model
	if(g_eAdecInitType & LX_ADEC_INIT_TYPE_SYS_ATSC_GCD)
	{
		ui32RecEventType = LX_ADEC_EVENT_CALLED_KDRV_CLOSE;

		//spin lock for protection
		spin_lock_irqsave(&gADEC_DSP_event_lock, flags);
		g_ui32RecEventType |= ui32RecEventType;
		spin_unlock_irqrestore(&gADEC_DSP_event_lock, flags);

		//Wake up poll if any int is triggered
		if(g_ui32RecEventType)
			wake_up_interruptible_all(&adec_poll_wait_queue);

	    ADEC_DEBUG("ADEC_Close : LX_ADEC_EVENT_CALLED_KDRV_CLOSE\n");
	}

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    ADEC_PRINT("device closed (%d:%d)\n", major, minor );
    return 0;
}

/**
 * ioctl handler for adec device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_ADEC_UNLOCKED_IOCTL)
static int ADEC_Ioctl ( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
#else
static long ADEC_Ioctl ( struct file* filp, unsigned int cmd, unsigned long arg )
#endif
{
    int err = 0, ret = 0;

    ADEC_DEVICE_T*		my_dev;
    struct cdev*		cdev;

	/*
	 * get current adec device object
	 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_ADEC_UNLOCKED_IOCTL)
	cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, ADEC_DEVICE_T, cdev);
#else
	my_dev	= (ADEC_DEVICE_T *)filp->private_data;
	cdev	= &my_dev->cdev;
#endif

    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != ADEC_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > ADEC_IOC_MAXNR)
    {
    	DBG_PRINT_WARNING("out of ioctl command. cmd_idx=%d\n", _IOC_NR(cmd) );
    	return -ENOTTY;
    }

	/* TODO : add some check routine for your device */

    /*
     * check if user memory is valid or not.
     * if memory can't be accessed from kernel, return error (-EFAULT)
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    if (err)
    {
    	DBG_PRINT_WARNING("memory access error. cmd_idx=%d, rw=%c%c, memptr=%p\n",
    													_IOC_NR(cmd),
    													(_IOC_DIR(cmd) & _IOC_READ)? 'r':'-',
    													(_IOC_DIR(cmd) & _IOC_WRITE)? 'w':'-',
    													(void*)arg );
        return -EFAULT;
	}

	ADEC_TRACE("cmd = %08X (cmd_idx=%d)\n", cmd, _IOC_NR(cmd) );

	switch(cmd)
	{
		case ADEC_IOW_INIT_MODULE:
		{
			LX_ADEC_INIT_TYPE_T		i_eInitType;

			ADEC_TRACE("ADEC_IOW_INIT_MODULE\n");

			if ( my_dev->is_initialized == 0 )
			{
				if ( copy_from_user(&i_eInitType, (void __user *)arg, sizeof(LX_ADEC_INIT_TYPE_T)) )
					return RET_ERROR;

				ret = ADEC_DRV_InitModule(i_eInitType);

				//set my_dev->is_initialized value
				if(ret == RET_OK)
					my_dev->is_initialized = 1;
			}
			else
			{
				ADEC_DEBUG("ADEC_IOW_INIT_MODULE is recalled.(%d)\n", my_dev->dev_open_count);
			}
		}
		break;

		case ADEC_IOW_SET_EVENT:
		{
			LX_ADEC_EVENT_TYPE_T	i_EventType;

			ADEC_TRACE("ADEC_IOW_SET_EVENT\n");

			if ( copy_from_user(&i_EventType, (void __user *)arg, sizeof(LX_ADEC_EVENT_TYPE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetEvent(i_EventType);
		}
		break;

		case ADEC_IOW_RESET_EVENT:
		{
			LX_ADEC_EVENT_TYPE_T	i_EventType;

			ADEC_TRACE("ADEC_IOW_RESET_EVENT\n");

			if ( copy_from_user(&i_EventType, (void __user *)arg, sizeof(LX_ADEC_EVENT_TYPE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_ResetEvent(i_EventType);
		}
		break;

		case ADEC_IORW_GET_EVENT:
		{
			LX_ADEC_EVENT_INFO_T	io_stEventInfo;

			ADEC_TRACE("ADEC_IOR_GET_EVENT\n");

			if ( copy_from_user(&io_stEventInfo, (void __user *)arg, sizeof(LX_ADEC_EVENT_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_GetEvent(&io_stEventInfo);

			if (copy_to_user((void *)arg, (void *)&io_stEventInfo, sizeof(LX_ADEC_EVENT_INFO_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IORW_GET_BUFFER_INFO:
		{
			LX_ADEC_BUF_INFO_T		io_stBufferInfo;

			ADEC_TRACE("LX_ADEC_BUF_INFO_T\n");

			if ( copy_from_user(&io_stBufferInfo, (void __user *)arg, sizeof(LX_ADEC_BUF_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_GetBufferInfo(&io_stBufferInfo);

			if (copy_to_user((void *)arg, (void *)&io_stBufferInfo, sizeof(LX_ADEC_BUF_INFO_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IORW_GET_DECODING_INFO:
		{
			LX_ADEC_DEC_INFO_T		io_stDecodingInfo;

			if ( copy_from_user(&io_stDecodingInfo, (void __user *)arg, sizeof(LX_ADEC_DEC_INFO_T)) )
				return RET_ERROR;

			ADEC_TRACE("ADEC_IOR_GET_DECODING_INFO\n");

			ret = ADEC_DRV_GetDecodingInfo(&io_stDecodingInfo);

			if (copy_to_user((void *)arg, (void *)&io_stDecodingInfo, sizeof(LX_ADEC_DEC_INFO_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IO_RESET_AAD_MODULE:
		{
			ADEC_TRACE("ADEC_IOW_RESET_AAD_MODULE\n");

			ret = ADEC_DRV_ResetAADModule();
		}
		break;

		case ADEC_IOW_SET_AAD_BIT_SHIFT:
		{
			UINT32	i_ui32BitShift = 0;

			ADEC_TRACE("ADEC_IOW_SET_AAD_BIT_SHIFT\n");

			if ( copy_from_user(&i_ui32BitShift, (void __user *)arg, sizeof(UINT32)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetAADBitShift(i_ui32BitShift);
		}
		break;


		case ADEC_IOW_SET_SOURCE:
		{
			LX_ADEC_SET_SOURCE_T	i_stSetSource;

			ADEC_TRACE("ADEC_IOW_SET_SOURCE\n");

			if ( copy_from_user(&i_stSetSource, (void __user *)arg, sizeof(LX_ADEC_SET_SOURCE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetSource(i_stSetSource);
		}
		break;

		case ADEC_IOW_START_DECODING:
		{
			LX_ADEC_START_DEC_T 	stStartInfo;

			ADEC_TRACE("ADEC_IOW_START_DECODING\n");

			if ( copy_from_user(&stStartInfo, (void __user *)arg, sizeof(LX_ADEC_START_DEC_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_StartDecoding(stStartInfo);
		}
		break;

		case ADEC_IOW_STOP_DECODING:
		{
			LX_ADEC_STOP_DEC_T 	stStopInfo;

			ADEC_TRACE("ADEC_IO_STOP_DECODING\n");

			if ( copy_from_user(&stStopInfo, (void __user *)arg, sizeof(LX_ADEC_STOP_DEC_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_StopDecoding(stStopInfo);
		}
		break;

		case ADEC_IORW_GET_ES_EXISTENCE:
		{
			LX_ADEC_ES_EXIST_T		io_stEsExist;

			ADEC_TRACE("ADEC_IOR_GET_ES_EXISTENCE\n");

			if ( copy_from_user(&io_stEsExist, (void __user *)arg, sizeof(LX_ADEC_ES_EXIST_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_GetESExistence(&io_stEsExist);

			if (copy_to_user((void *)arg, (void *)&io_stEsExist, sizeof(LX_ADEC_ES_EXIST_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IORW_GET_ES_INFO:
		{
			LX_ADEC_GET_ES_INFO_T	io_stESInfo	= { 0 };

			ADEC_TRACE("ADEC_IOR_GET_ES_INFO\n");

			if ( copy_from_user(&io_stESInfo, (void __user *)arg, sizeof(LX_ADEC_GET_ES_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_GetESInfo(&io_stESInfo);

			if (copy_to_user((void *)arg, (void *)&io_stESInfo, sizeof(LX_ADEC_GET_ES_INFO_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IOW_START_AD_DECODING:
		{
			LX_ADEC_SRC_TYPE_T		i_eSrcType;

			ADEC_TRACE("ADEC_IOW_START_AD_DECODING\n");

			if ( copy_from_user(&i_eSrcType, (void __user *)arg, sizeof(LX_ADEC_SRC_TYPE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_StartADDecoding(i_eSrcType);
		}
		break;

		case ADEC_IO_STOP_AD_DECODING:
		{
			ADEC_TRACE("ADEC_IO_STOP_AD_DECODING\n");

			ret = ADEC_DRV_StopADDecoding();
		}
		break;

		case ADEC_IOW_SET_SPK_OUT_MODE:
		{
			LX_ADEC_OUTPUT_MODE_T i_eSPKOutMode;

			ADEC_TRACE("ADEC_IOW_SET_SPK_OUT_MODE\n");

			if ( copy_from_user(&i_eSPKOutMode, (void __user *)arg, sizeof(LX_ADEC_OUTPUT_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetSPKOutMode(i_eSPKOutMode);
		}
		break;

		case ADEC_IOW_SET_DECODER_MODE:
		{
			LX_ADEC_DECODER_MODE_T i_eDecoderMode;

			ADEC_TRACE("ADEC_IOW_SET_DECODER_MODE\n");

			if ( copy_from_user(&i_eDecoderMode, (void __user *)arg, sizeof(LX_ADEC_DECODER_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetDecoderMode(i_eDecoderMode);
		}
		break;


		case ADEC_IOW_SET_VOLUME:
		{
			UINT32	i_ui32Volume = 0;

			ADEC_TRACE("ADEC_IOW_SET_VOLUME\n");

			if ( copy_from_user(&i_ui32Volume, (void __user *)arg, sizeof(UINT32)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetVolume(i_ui32Volume);
		}
		break;

		case ADEC_IOW_SET_AD_VOLUME:
		{
			UINT32	i_ui32Volume = 0;

			ADEC_TRACE("ADEC_IOW_SET_AD_VOLUME\n");

			if ( copy_from_user(&i_ui32Volume , (void __user *)arg, sizeof(UINT32)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetADVolume(i_ui32Volume);
		}
		break;

		case ADEC_IOW_SET_DAC_OUT_VOLUME:
		{
			LX_ADEC_DAC_VOL_T 		i_stDACOutVol;

			ADEC_TRACE("ADEC_IOW_SET_DAC_OUT_VOLUME\n");

			if ( copy_from_user(&i_stDACOutVol, (void __user *)arg, sizeof(LX_ADEC_DAC_VOL_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetDACOutVolume(i_stDACOutVol);
		}
		break;

		case ADEC_IOW_SET_OUTPUT_PORT_MUTE:
		{
			LX_ADEC_OUTPUT_PORT_MUTE_T i_stOutPortMute;

			ADEC_TRACE("ADEC_IOW_SET_OUTPUT_PORT_MUTE\n");

			if ( copy_from_user(&i_stOutPortMute, (void __user *)arg, sizeof(LX_ADEC_OUTPUT_PORT_MUTE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetOutputPortMute(i_stOutPortMute);
		}
		break;

		case ADEC_IOW_SET_MIX_VOLUME:
		{
			LX_ADEC_MIX_VOL_T	i_stMixVolume;

			ADEC_TRACE("ADEC_IOW_SET_MIX_VOLUME\n");

			if ( copy_from_user(&i_stMixVolume , (void __user *)arg, sizeof(LX_ADEC_MIX_VOL_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetMixVolume(i_stMixVolume);
		}
		break;

		case ADEC_IOW_SET_DAMP_VOLUME:
		{
			UINT32	i_ui8Volume = 0;

			ADEC_TRACE("ADEC_IOW_SET_DAMP_VOLUME\n");

			if ( copy_from_user(&i_ui8Volume , (void __user *)arg, sizeof(UINT8)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetDAMPVolume(i_ui8Volume);
		}
		break;

		case ADEC_IOW_SET_DAMP_MUTE:
		{
			BOOLEAN i_bMuteFlag;

			ADEC_TRACE("ADEC_IOW_SET_DAMP_MUTE\n");

			if ( copy_from_user(&i_bMuteFlag, (void __user *)arg, sizeof(BOOLEAN)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetDAMPMute(i_bMuteFlag);
		}
		break;

		case ADEC_IOW_SET_MIX_CH_VOLUME:
		{
			LX_ADEC_MIX_CH_VOL_T	i_stMixChVolume;

			ADEC_TRACE("ADEC_IOW_SET_MIX_CH_VOLUME\n");

			if ( copy_from_user(&i_stMixChVolume , (void __user *)arg, sizeof(LX_ADEC_MIX_CH_VOL_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetMixChannelVolume(i_stMixChVolume);
		}
		break;


		case ADEC_IOW_SET_SPDIF_OUTPUT_TYPE:
		{
			LX_ADEC_SPDIF_MODE_T	i_eSPDIFMode;

			ADEC_TRACE("ADEC_IOW_SET_SPDIF_OUTPUT_TYPE\n");

			if ( copy_from_user(&i_eSPDIFMode, (void __user *)arg, sizeof(LX_ADEC_SPDIF_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetSPDIFOutputType(i_eSPDIFMode);
		}
		break;

		case ADEC_IOW_SET_SPDIF_SCMS_CTRL:
		{
			LX_ADEC_SPDIF_SCMS_T	i_eSPDIFScmsCtrl;

			ADEC_TRACE("ADEC_IOW_SET_SPDIF_SCMS_CTRL\n");

			if ( copy_from_user(&i_eSPDIFScmsCtrl, (void __user *)arg, sizeof(LX_ADEC_SPDIF_SCMS_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetSPDIFScmsCtrl(i_eSPDIFScmsCtrl);
		}
		break;

		case ADEC_IOW_SET_SPDIF_VOLUME:
		{
			UINT32 i_ui32Volume = 0;

			ADEC_TRACE("ADEC_IOW_SET_SPDIF_VOLUME\n");

			if ( copy_from_user(&i_ui32Volume, (void __user *)arg, sizeof(UINT32)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetSPDIFVolume(i_ui32Volume);
		}
		break;

		case ADEC_IOW_SET_SPDIF_MUTE:
		{
			BOOLEAN i_bMuteFlag;

			ADEC_TRACE("ADEC_IOW_SET_SPDIF_MUTE\n");

			if ( copy_from_user(&i_bMuteFlag, (void __user *)arg, sizeof(BOOLEAN)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetSPDIFMute(i_bMuteFlag);
		}
		break;

		case ADEC_IOW_ENABLE_AV_LIPSYNC:
		{
			LX_ADEC_SYNC_T 	i_eAVLipSync;

			ADEC_TRACE("ADEC_IOW_ENABLE_AV_LIPSYNC\n");

			if ( copy_from_user(&i_eAVLipSync, (void __user *)arg, sizeof(LX_ADEC_SYNC_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_EnableAVLipSync(i_eAVLipSync);
		}
		break;

		case ADEC_IOW_SET_I2S_OUT_DELAY_TIME:
		{
			UINT32 i_ui32DelayinMS;

			ADEC_TRACE("ADEC_IOW_SET_I2S_OUT_DELAY_TIME\n");

			if ( copy_from_user(&i_ui32DelayinMS, (void __user *)arg, sizeof(UINT32)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetI2SOutDelayTime(i_ui32DelayinMS);
		}
		break;

		case ADEC_IOW_SET_DAC_OUT_DELAY_TIME:
		{
			UINT32 i_ui32DelayinMS;

			ADEC_TRACE("ADEC_IOW_SET_DAC_OUT_DELAY_TIME\n");

			if ( copy_from_user(&i_ui32DelayinMS, (void __user *)arg, sizeof(UINT32)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetDACOutDelayTime(i_ui32DelayinMS);
		}
		break;

		case ADEC_IOW_SET_SPDIF_DELAY_TIME:
		{
			UINT32 i_ui32DelayinMS;

			ADEC_TRACE("ADEC_IOW_SET_SPDIF_DELAY_TIME\n");

			if ( copy_from_user(&i_ui32DelayinMS, (void __user *)arg, sizeof(UINT32)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetSPDIFDelayTime(i_ui32DelayinMS);
		}
		break;

		case ADEC_IOW_SET_TRICK_STATE:
		{
			LX_ADEC_TRICK_MODE_T	i_eTrickMode;

			ADEC_TRACE("ADEC_IOW_SET_TRICK_STATE\n");

			if ( copy_from_user(&i_eTrickMode, (void __user *)arg, sizeof(LX_ADEC_TRICK_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetTrickState(i_eTrickMode);
		}
		break;

		case ADEC_IOR_GET_CLOCK_INFO:
		{
			LX_ADEC_CLOCK_INFO_T	io_stClockInfo;

			ADEC_TRACE("ADEC_IOR_GET_CLOCK_INFO\n");

			if ( copy_from_user(&io_stClockInfo, (void __user *)arg, sizeof(LX_ADEC_CLOCK_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_GetClockInfo(&io_stClockInfo);

			if ( copy_to_user((void *)arg, (void *)&io_stClockInfo, sizeof(LX_ADEC_CLOCK_INFO_T)) )
				return RET_ERROR;
		}
		break;


		case ADEC_IOW_SET_BALANCE:
		{
			UINT8 i_ui8BalanceVal;

			ADEC_TRACE("ADEC_IOW_SET_BALANCE\n");

			if ( copy_from_user(&i_ui8BalanceVal, (void __user *)arg, sizeof(UINT8)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetBalance(i_ui8BalanceVal);
		}
		break;

		case ADEC_IOW_SET_BASS:
		{
			UINT8 i_ui8BassVal;

			ADEC_TRACE("ADEC_IOW_SET_BASS\n");

			if ( copy_from_user(&i_ui8BassVal, (void __user *)arg, sizeof(UINT8)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetBASS(i_ui8BassVal);
		}
		break;

		case ADEC_IOW_SET_TREBLE:
		{
			UINT8 i_ui8TrebleVal;

			ADEC_TRACE("ADEC_IOW_SET_TREBLE\n");

			if ( copy_from_user(&i_ui8TrebleVal, (void __user *)arg, sizeof(UINT8)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetTreble(i_ui8TrebleVal);
		}
		break;


		case ADEC_IOW_SE_SET_FUNCTION:
		{
			LX_ADEC_SE_FN_PARAM_T i_stSeFnParam;

			memset(&i_stSeFnParam, 0x0, sizeof(LX_ADEC_SE_FN_PARAM_T));

			ADEC_TRACE("ADEC_IOW_SE_SET_FUNCTION\n");

			if ( copy_from_user(&i_stSeFnParam, (void __user *)arg, sizeof(LX_ADEC_SE_FN_PARAM_T)) )
				return RET_ERROR;

			ret = ADEC_L9_SESetFunction(&i_stSeFnParam);
		}
		break;

		case ADEC_IORW_SE_GET_FUNCTION:
		{
			LX_ADEC_SE_FN_PARAM_T	io_stSeFnParam;

			memset(&io_stSeFnParam, 0x0, sizeof(LX_ADEC_SE_FN_PARAM_T));

			ADEC_TRACE("ADEC_IORW_SE_GET_FUNCTION\n");

			if ( copy_from_user(&io_stSeFnParam, (void __user *)arg, sizeof(LX_ADEC_SE_FN_PARAM_T)) )
				return RET_ERROR;

			ret = ADEC_L9_SEGetFunction(&io_stSeFnParam);

			if (copy_to_user((void *)arg, (void *)&io_stSeFnParam, sizeof(LX_ADEC_SE_FN_PARAM_T)))
				return RET_ERROR;
		}
		break;


		case ADEC_IOW_FEED_AUDIO_CLIP:
		{
			LX_ADEC_CLIP_MEM_INFO_T	i_stClipMemInfo;

			ADEC_TRACE("ADEC_IOW_FEED_AUDIO_CLIP\n");

			if ( copy_from_user(&i_stClipMemInfo, (void __user *)arg, sizeof(LX_ADEC_CLIP_MEM_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_FeedAudioClip(i_stClipMemInfo);
		}
		break;

		case ADEC_IOW_START_AUDIO_CLIP:
		{
			LX_ADEC_CLIP_START_INFO_T i_stClipStartInfo;

			ADEC_TRACE("ADEC_IOW_START_AUDIO_CLIP\n");

			if ( copy_from_user(&i_stClipStartInfo, (void __user *)arg, sizeof(LX_ADEC_CLIP_START_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_StartAudioClip(i_stClipStartInfo);
		}
		break;

		case ADEC_IOW_STOP_AUDIO_CLIP:
		{
			LX_ADEC_DEC_MODE_T	i_eAdecMode;

			ADEC_TRACE("ADEC_IO_STOP_AUDIO_CLIP\n");

			if ( copy_from_user(&i_eAdecMode, (void __user *)arg, sizeof(LX_ADEC_DEC_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_StopAudioClip(i_eAdecMode);
		}
		break;

		case ADEC_IOW_PAUSE_AUDIO_CLIP:
		{
			LX_ADEC_DEC_MODE_T	i_eAdecMode;

			ADEC_TRACE("ADEC_IO_PAUSE_AUDIO_CLIP\n");

			if ( copy_from_user(&i_eAdecMode, (void __user *)arg, sizeof(LX_ADEC_DEC_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_PauseAudioClip(i_eAdecMode);
		}
		break;

		case ADEC_IOW_RESUME_AUDIO_CLIP:
		{
			LX_ADEC_DEC_MODE_T	i_eAdecMode;

			ADEC_TRACE("ADEC_IO_RESUME_AUDIO_CLIP\n");

			if ( copy_from_user(&i_eAdecMode, (void __user *)arg, sizeof(LX_ADEC_DEC_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_ResumeAudioClip(i_eAdecMode);
		}
		break;

		case ADEC_IOW_FLUSH_AUDIO_CLIP:
		{
			LX_ADEC_DEC_MODE_T	i_eAdecMode;

			ADEC_TRACE("ADEC_IOW_FLUSH_AUDIO_CLIP\n");

			if ( copy_from_user(&i_eAdecMode, (void __user *)arg, sizeof(LX_ADEC_DEC_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_FlushAudioClip(i_eAdecMode);
		}
		break;

		case ADEC_IOW_RESET_AUDIO_CLIP:
		{
			LX_ADEC_DEC_MODE_T	i_eAdecMode;

			ADEC_TRACE("ADEC_IOW_RESET_AUDIO_CLIP\n");

			if ( copy_from_user(&i_eAdecMode, (void __user *)arg, sizeof(LX_ADEC_DEC_MODE_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_ResetAudioClip(i_eAdecMode);
		}
		break;

		case ADEC_IOW_SET_AV_CLOCK_CTRL:
		{
			LX_ADEC_AV_CLOCK_INFO_T	i_stAVClockInfo;

			ADEC_TRACE("ADEC_IOW_SET_AV_CLOCK_CTRL\n");

			if ( copy_from_user(&i_stAVClockInfo, (void __user *)arg, sizeof(LX_ADEC_AV_CLOCK_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetAVClockControl(i_stAVClockInfo);
		}
		break;


		case ADEC_IOR_GET_ENC_BUF_INFO:
		{
			LX_ADEC_ENC_BUF_T	o_stEncBufInfo;

			ADEC_TRACE("ADEC_IOR_GET_ENC_BUF_INFO\n");

			ret = ADEC_DRV_GetEncBufInfo(&o_stEncBufInfo);

			if (copy_to_user((void *)arg, (void *)&o_stEncBufInfo, sizeof(LX_ADEC_ENC_BUF_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IOW_SET_ENC_PARAM:
		{
			LX_ADEC_ENC_PARAM_T	i_stEncParam;

			ADEC_TRACE("ADEC_IOW_SET_ENC_PARAM\n");

			if ( copy_from_user(&i_stEncParam, (void __user *)arg, sizeof(LX_ADEC_ENC_PARAM_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_SetEncParam(i_stEncParam);
		}
		break;

		case ADEC_IOR_GET_ENC_PARAM:
		{
			LX_ADEC_ENC_PARAM_T	o_stEncParam;

			ADEC_TRACE("ADEC_IOR_GET_ENC_BUF_INFO\n");

			ret = ADEC_DRV_GetEncParam(&o_stEncParam);

			if (copy_to_user((void *)arg, (void *)&o_stEncParam, sizeof(LX_ADEC_ENC_PARAM_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IOW_START_ENCODING:
		{
			LX_ADEC_ENC_START_T	i_stStartInfo;

			ADEC_TRACE("ADEC_IOW_START_ENCODING\n");

			if ( copy_from_user(&i_stStartInfo, (void __user *)arg, sizeof(LX_ADEC_ENC_START_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_StartEncoding(i_stStartInfo);
		}
		break;

		case ADEC_IOW_STOP_ENCODING:
		{
			LX_ADEC_ENC_STOP_T	i_stStopInfo;

			ADEC_TRACE("ADEC_IOW_STOP_ENCODING\n");

			if ( copy_from_user(&i_stStopInfo, (void __user *)arg, sizeof(LX_ADEC_ENC_STOP_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_StopEncoding(i_stStopInfo);
		}
		break;

		case ADEC_IORW_GET_ENCODING_INFO:
		{
			LX_ADEC_ENC_INFO_T	io_stEncParam;

			memset(&io_stEncParam, 0x0, sizeof(LX_ADEC_ENC_INFO_T));

			ADEC_TRACE("ADEC_IORW_GET_ENCODING_INFO\n");

			if ( copy_from_user(&io_stEncParam, (void __user *)arg, sizeof(LX_ADEC_ENC_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_GetEncodingInfo(&io_stEncParam);

			if (copy_to_user((void *)arg, (void *)&io_stEncParam, sizeof(LX_ADEC_ENC_INFO_T)))
				return RET_ERROR;
		}
		break;


		case ADEC_IOR_GET_PCM_BUF_INFO:
		{
			LX_ADEC_PCM_BUF_T	o_stPcmBufInfo;

			ADEC_TRACE("ADEC_IOR_GET_PCM_BUF_INFO\n");

			ret = ADEC_DRV_GetPcmBufInfo(&o_stPcmBufInfo);

			if (copy_to_user((void *)arg, (void *)&o_stPcmBufInfo, sizeof(LX_ADEC_PCM_BUF_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IORW_GET_CAPTURING_INFO:
		{
			LX_ADEC_CAP_INFO_T	io_stCapParam;

			memset(&io_stCapParam, 0x0, sizeof(LX_ADEC_CAP_INFO_T));

			ADEC_TRACE("ADEC_IORW_GET_CAPTURING_INFO\n");

			if ( copy_from_user(&io_stCapParam, (void __user *)arg, sizeof(LX_ADEC_CAP_INFO_T)) )
				return RET_ERROR;

			ret = ADEC_DRV_GetCapturingInfo(&io_stCapParam);

			if (copy_to_user((void *)arg, (void *)&io_stCapParam, sizeof(LX_ADEC_CAP_INFO_T)))
				return RET_ERROR;
		}
		break;


		case ADEC_IORW_READ_AND_WRITE_REG:
		{
			LX_ADEC_REG_INFO_T		io_stRegInfo;

			memset(&io_stRegInfo, 0x0, sizeof(LX_ADEC_REG_INFO_T));

			ADEC_TRACE("ADEC_IORW_READ_AND_WRITE_REG\n");

			if (copy_from_user( &io_stRegInfo, (void __user *)arg, sizeof(LX_ADEC_REG_INFO_T)))
				return RET_ERROR;

			ret = ADEC_DRV_ReadAndWriteReg(&io_stRegInfo);

			if (copy_to_user((void *)arg, (void *)&io_stRegInfo, sizeof(LX_ADEC_REG_INFO_T)))
				return RET_ERROR;
		}
		break;

		case ADEC_IOW_CUSTOM_IPC:
		{
			LX_ADEC_CUSTOM_IPC_T	io_stCustomIpc;

			ADEC_TRACE("ADEC_IOW_CUSTOM_IPC\n");

			if(copy_from_user( &io_stCustomIpc, (void __user *)arg, sizeof(LX_ADEC_CUSTOM_IPC_T)))
				return RET_ERROR;

			ret = ADEC_DRV_CustomIpc(&io_stCustomIpc);

			return 0;
		}
		break;

		case ADEC_IO_SHOW_STATUS_REGISTER:
		{
			ADEC_TRACE("ADEC_IO_SHOW_STATUS_REGISTER\n");

			ret = ADEC_DRV_ShowStatusRegister();
		}
		break;

 		case ADEC_IOW_DEBUG_PRINT_CTRL:
		{
			LX_ADEC_DEBUG_CTRL_T	i_stDebugCtrl;

			memset(&i_stDebugCtrl, 0x0, sizeof(LX_ADEC_DEBUG_CTRL_T));

			ADEC_TRACE("ADEC_IOW_DEBUG_PRINT_CTRL\n");

			if(copy_from_user( &i_stDebugCtrl, (void __user *)arg, sizeof(LX_ADEC_DEBUG_CTRL_T)))
					return RET_ERROR;


			if( (i_stDebugCtrl.printType >= NUM_OF_ADEC_PRINT) || (i_stDebugCtrl.printColor >= NUM_OF_ADEC_PRINT) )
			{
				ADEC_ERROR("Check Argument\n" );
				return RET_ERROR;
			}

			if ( i_stDebugCtrl.printColor == DBG_COLOR_NONE)
			{
				OS_DEBUG_DisableModuleByIndex(g_adec_debug_fd, i_stDebugCtrl.printType);
				ADEC_ERROR("ADEC_PRINT_TYPE[%s] is disabled.\n", ui8AdecPrintType[i_stDebugCtrl.printType]);
			}
			else
			{
				OS_DEBUG_EnableModuleByIndex(g_adec_debug_fd, i_stDebugCtrl.printType, i_stDebugCtrl.printColor);
				ADEC_ERROR("ADEC_PRINT_TYPE[%s] is enabled Color is [%s].\n", ui8AdecPrintType[i_stDebugCtrl.printType], ui8AdecPrintColor[i_stDebugCtrl.printColor - 1]);
			}

			return RET_OK;
 		}
		break;


	    default:
	    {
			/* redundant check but it seems more readable */
			ADEC_ERROR("Invalid IOCTL Call!!!\n");
    	    ret = RET_INVALID_IOCTL;
		}
    }

    return ret;
}

static int
ADEC_Mmap( struct file *filp , struct vm_area_struct *vma )
{
	ULONG 	size = vma->vm_end - vma->vm_start;
	UINT32	addr;

	/* Set default mmap variable */
	vma->vm_flags |= VM_RESERVED;
	vma->vm_flags |= VM_IO;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	/* Set physical mmap address.( addr is only one set in adec device.) */
	/* So, app driver must be manuplated other mmap address.(cpba, dpbm...) */
	addr = pstMemCfgADEC->cpbm_memory_base & PAGE_MASK;

	if ( remap_pfn_range( vma, vma->vm_start, addr >> PAGE_SHIFT, size, vma->vm_page_prot ) )
	   return -EAGAIN;

	ADEC_TRACE ( "size [%08X]\n", (UINT32)size );

	ADEC_TRACE ( "vm_pgoff     [%08X]\n",(int) vma->vm_pgoff << PAGE_SHIFT );
	ADEC_TRACE ( "vm_start     [%08X]\n",(int) vma->vm_start );
	ADEC_TRACE ( "vm_end       [%08X]\n",(int) vma->vm_end );
	ADEC_TRACE ( "vm_flags     [%08X]\n",(int) vma->vm_flags );
	return RET_OK;
}

static unsigned int
ADEC_Poll(struct file *filp, poll_table *wait)
{
    ADEC_TRACE("Poll wait!!!\n");

	poll_wait(filp, &adec_poll_wait_queue, wait);

    if(g_ui32RecEventType != 0)
        return POLLIN;
	else
		return 0;
}

/**
* initialize function by chip revision
*
* @parm void
* @return int
*/

static int
ADEC_InitFunction(void)
{
	int ret = 0;

	/* Check LX Chip Revision Number */
	if(lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{
		ADEC_DEBUG_TMP ("ADEC_InitFunction : LX_CHIP_REV(L9, XX)(0x%X)\n", lx_chip_rev());

      	ADEC_DRV_InitModule				= ADEC_L9_InitModule;
      	ADEC_DRV_SetEvent				= ADEC_L9_SetEvent;
      	ADEC_DRV_ResetEvent				= ADEC_L9_ResetEvent;
      	ADEC_DRV_GetEvent				= ADEC_L9_GetEvent;
      	ADEC_DRV_GetBufferInfo			= ADEC_L9_GetBufferInfo;
      	ADEC_DRV_GetDecodingInfo		= ADEC_L9_GetDecodingInfo;
      	ADEC_DRV_ResetAADModule			= ADEC_L9_ResetAADModule;
      	ADEC_DRV_SetAADBitShift			= ADEC_L9_SetAADBitShift;

      	ADEC_DRV_SetSource				= ADEC_L9_SetSource;
      	ADEC_DRV_StartDecoding			= ADEC_L9_StartDecoding;
      	ADEC_DRV_StopDecoding			= ADEC_L9_StopDecoding;
      	ADEC_DRV_GetESExistence			= ADEC_L9_GetESExistence;
      	ADEC_DRV_GetESInfo				= ADEC_L9_GetESInfo;
      	ADEC_DRV_StartADDecoding		= ADEC_L9_StartADDecoding;
      	ADEC_DRV_StopADDecoding			= ADEC_L9_StopADDecoding;
      	ADEC_DRV_SetSPKOutMode			= ADEC_L9_SetSPKOutMode;
      	ADEC_DRV_SetDecoderMode			= ADEC_L9_SetDecoderMode;

      	ADEC_DRV_SetVolume				= ADEC_L9_SetVolume;
      	ADEC_DRV_SetADVolume			= ADEC_L9_SetADVolume;
     	ADEC_DRV_SetDACOutVolume		= ADEC_L9_SetDACOutVolume;
     	ADEC_DRV_SetOutputPortMute		= ADEC_L9_SetOutputPortMute;
      	ADEC_DRV_SetMixVolume			= ADEC_L9_SetMixVolume;
      	ADEC_DRV_SetDAMPVolume			= ADEC_L9_SetDAMPVolume;
      	ADEC_DRV_SetDAMPMute			= ADEC_L9_SetDAMPMute;
      	ADEC_DRV_SetMixChannelVolume	= ADEC_L9_SetMixChannelVolume;

     	ADEC_DRV_SetSPDIFOutputType		= ADEC_L9_SetSPDIFOutputType;
     	ADEC_DRV_SetSPDIFScmsCtrl		= ADEC_L9_SetSPDIFScmsCtrl;
     	ADEC_DRV_SetSPDIFVolume			= ADEC_L9_SetSPDIFVolume;
     	ADEC_DRV_SetSPDIFMute			= ADEC_L9_SetSPDIFMute;

     	ADEC_DRV_EnableAVLipSync		= ADEC_L9_EnableAVLipSync;
     	ADEC_DRV_SetI2SOutDelayTime		= ADEC_L9_SetI2SOutDelayTime;
     	ADEC_DRV_SetDACOutDelayTime		= ADEC_L9_SetDACOutDelayTime;
     	ADEC_DRV_SetSPDIFDelayTime		= ADEC_L9_SetSPDIFDelayTime;
      	ADEC_DRV_SetTrickState			= ADEC_L9_SetTrickState;
      	ADEC_DRV_GetClockInfo			= ADEC_L9_GetClockInfo;

      	ADEC_DRV_SetBalance				= ADEC_L9_SetBalance;
      	ADEC_DRV_SetBASS				= ADEC_L9_SetBASS;
      	ADEC_DRV_SetTreble				= ADEC_L9_SetTreble;

      	ADEC_DRV_SESetFunction			= ADEC_L9_SESetFunction;
      	ADEC_DRV_SEGetFunction			= ADEC_L9_SEGetFunction;

      	ADEC_DRV_FeedAudioClip			= ADEC_L9_FeedAudioClip;
      	ADEC_DRV_StartAudioClip			= ADEC_L9_StartAudioClip;
      	ADEC_DRV_StopAudioClip			= ADEC_L9_StopAudioClip;
      	ADEC_DRV_PauseAudioClip			= ADEC_L9_PauseAudioClip;
      	ADEC_DRV_ResumeAudioClip		= ADEC_L9_ResumeAudioClip;
      	ADEC_DRV_FlushAudioClip			= ADEC_L9_FlushAudioClip;
      	ADEC_DRV_ResetAudioClip			= ADEC_L9_ResetAudioClip;
      	ADEC_DRV_SetAVClockControl		= ADEC_L9_SetAVClockControl;

      	ADEC_DRV_GetEncBufInfo			= ADEC_L9_GetEncBufInfo;
      	ADEC_DRV_SetEncParam			= ADEC_L9_SetEncParam;
      	ADEC_DRV_GetEncParam			= ADEC_L9_GetEncParam;
      	ADEC_DRV_StartEncoding			= ADEC_L9_StartEncoding;
      	ADEC_DRV_StopEncoding			= ADEC_L9_StopEncoding;
      	ADEC_DRV_GetEncodingInfo		= ADEC_L9_GetEncodingInfo;

      	ADEC_DRV_GetPcmBufInfo			= ADEC_L9_GetPcmBufInfo;
      	ADEC_DRV_GetCapturingInfo		= ADEC_L9_GetCapturingInfo;

      	ADEC_DRV_ReadAndWriteReg		= ADEC_L9_ReadAndWriteReg;

		ADEC_DRV_ShowStatusRegister		= ADEC_L9_ShowStausRegister;
		ADEC_DRV_CustomIpc				= ADEC_L9_CustomIpc;

      	ADEC_Monitor_Task				= ADEC_L9_Monitor_Task;

      	adec_interrupt0					= adec_l9_interrupt0;
      	adec_interrupt1					= adec_l9_interrupt1;

#ifdef	KDRV_CONFIG_PM
		ADEC_RunSuspend		= ADEC_L9_RunSuspend;
		ADEC_RunResume		= ADEC_L9_RunResume;
#endif
	}
#ifdef INCLUDE_L8_CHIP_KDRV
	else if(lx_chip_rev() == LX_CHIP_REV(L8, B0) || lx_chip_rev() == LX_CHIP_REV(L8, A1))
	{
		ADEC_DEBUG_TMP ("ADEC_InitFunction : LX_CHIP_REV(L8)(0x%X)\n", lx_chip_rev());

      	ADEC_DRV_InitModule				= ADEC_L8_InitModule;
      	ADEC_DRV_SetEvent				= ADEC_L8_SetEvent;
      	ADEC_DRV_ResetEvent				= ADEC_L8_ResetEvent;
      	ADEC_DRV_GetEvent				= ADEC_L8_GetEvent;
      	ADEC_DRV_GetMMapInfo			= ADEC_L8_GetMMapInfo;
      	ADEC_DRV_GetBufferInfo			= ADEC_L8_GetBufferInfo;
      	ADEC_DRV_GetDecodingInfo		= ADEC_L8_GetDecodingInfo;
      	ADEC_DRV_ResetAADModule			= ADEC_L8_ResetAADModule;

      	ADEC_DRV_SetSource				= ADEC_L8_SetSource;
      	ADEC_DRV_StartDecoding			= ADEC_L8_StartDecoding;
      	ADEC_DRV_StopDecoding			= ADEC_L8_StopDecoding;
      	ADEC_DRV_GetESExistence			= ADEC_L8_GetESExistence;
      	ADEC_DRV_GetESInfo				= ADEC_L8_GetESInfo;
      	ADEC_DRV_StartADDecoding		= ADEC_L8_StartADDecoding;
      	ADEC_DRV_StopADDecoding			= ADEC_L8_StopADDecoding;
      	ADEC_DRV_SetSPKOutMode			= ADEC_L8_SetSPKOutMode;

      	ADEC_DRV_SetVolume				= ADEC_L8_SetVolume;
      	ADEC_DRV_SetADVolume			= ADEC_L8_SetADVolume;
     	ADEC_DRV_SetDACOutVolume		= ADEC_L8_SetDACOutVolume;
     	ADEC_DRV_SetOutputPortMute		= ADEC_L8_SetOutputPortMute;

     	ADEC_DRV_SetSPDIFOutputType		= ADEC_L8_SetSPDIFOutputType;
     	ADEC_DRV_SetSPDIFScmsCtrl		= ADEC_L8_SetSPDIFScmsCtrl;
     	ADEC_DRV_SetSPDIFVolume			= ADEC_L8_SetSPDIFVolume;
     	ADEC_DRV_SetSPDIFMute			= ADEC_L8_SetSPDIFMute;

     	ADEC_DRV_EnableAVLipSync		= ADEC_L8_EnableAVLipSync;
     	ADEC_DRV_SetI2SOutDelayTime		= ADEC_L8_SetI2SOutDelayTime;
     	ADEC_DRV_SetDACOutDelayTime		= ADEC_L8_SetDACOutDelayTime;
     	ADEC_DRV_SetSPDIFDelayTime		= ADEC_L8_SetSPDIFDelayTime;
      	ADEC_DRV_SetTrickState			= ADEC_L8_SetTrickState;
      	ADEC_DRV_GetClockInfo			= ADEC_L8_GetAndSetClockInfo;

      	ADEC_DRV_SetBalance				= ADEC_L8_SetBalance;
      	ADEC_DRV_SetBASS				= ADEC_L8_SetBASS;
      	ADEC_DRV_SetTreble				= ADEC_L8_SetTreble;

      	ADEC_DRV_LoadAudioClip			= ADEC_L8_LoadAudioClip;
      	ADEC_DRV_StartAudioClip			= ADEC_L8_StartAudioClip;
      	ADEC_DRV_StopAudioClip			= ADEC_L8_StopAudioClip;
      	ADEC_DRV_PauseAudioClip			= ADEC_L8_PauseAudioClip;
      	ADEC_DRV_ResumeAudioClip		= ADEC_L8_ResumeAudioClip;

      	ADEC_DRV_ReadAndWriteReg		= ADEC_L8_ReadAndWriteReg;

      	ADEC_Monitor_Task				= ADEC_L8_Monitor_Task;

      	adec_interrupt0					= adec_l8_interrupt0;
      	adec_interrupt1					= adec_l8_interrupt1;

#ifdef	KDRV_CONFIG_PM
		ADEC_RunSuspend		= ADEC_L8_RunSuspend;
		ADEC_RunResume		= ADEC_L8_RunResume;
#endif
	}
#endif	//#ifdef INCLUDE_L8_CHIP_KDRV
	else
	{
		DBG_PRINT_ERROR("LX_CHIP_REV => Unknown(0x%X) : ERROR\n", lx_chip_rev());
		return -EIO;
	}

	return ret;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",ADEC_Init);
#else
module_init(ADEC_Init);
#endif
module_exit(ADEC_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

