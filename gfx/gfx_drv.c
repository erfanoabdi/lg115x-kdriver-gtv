/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/** @file
 *
 *  main driver implementation for gfx device.
 *	gfx device will teach you how to make device driver with new platform.
 *
 *  author		raxis.lim (raxis.lim@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_gfx
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_GFX_DEVICE_READ_WRITE_FOPS
#define SUPPORT_GFX_UNLOCKED_IOCTL

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#include <linux/delay.h>
#endif
#include "os_util.h"
#include "base_dev_cfg.h"
#include "gfx_impl.h"
#include "gfx_drv.h"

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
 *	main control block for gfx device.
 *	each minor device has unique control block
 *
 */
typedef struct
{
// BEGIN of common device
	int						dev_open_count;		///< check if device is opened or not
	dev_t					devno;				///< device number
	struct cdev				cdev;				///< char device structure
// END of command device

// BEGIN of device specific data


// END of device specific data
}
GFX_DEVICE_T;

#ifdef KDRV_CONFIG_PM
typedef struct
{
	// add here extra parameter
	bool	is_suspended;
}GFX_DRVDATA_T;
#endif

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	GFX_L8_InitHAL	( GFX_HAL_T*  hal );	/* implemented at l8/gfx_drv_l8.c */
extern	void	GFX_L9_InitHAL	( GFX_HAL_T*  hal );	/* implemented at l8/gfx_drv_l9.c */

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int 			g_gfx_major = GFX_MAJOR;
int 			g_gfx_minor = GFX_MINOR;
int				g_gfx_debug_fd;
int    			g_gfx_trace_depth;

LX_GFX_CFG_T	g_gfx_cfg;
GFX_HAL_T		g_gfx_hal;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      GFX_Open	(struct inode *, struct file *);
static int      GFX_Close	(struct inode *, struct file *);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_GFX_UNLOCKED_IOCTL)
static int 		GFX_Ioctl 	(struct inode*, struct file *, UINT32, ULONG );
#else
static long		GFX_Ioctl 	(struct file *, UINT32, ULONG );
#endif
static int		GFX_mmap	(struct file *, struct vm_area_struct *);
#ifdef SUPPORT_GFX_DEVICE_READ_WRITE_FOPS
static ssize_t  GFX_Read	(struct file *, char *, size_t, loff_t *);
static ssize_t  GFX_Write	(struct file *, const char *, size_t, loff_t *);
#endif

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static OS_SEM_T	gfx_dev_mtx;

/**
 * main control block for gfx device
*/
static GFX_DEVICE_T*		g_gfx_device;

/**
 * file I/O description for gfx device
 *
*/
static struct file_operations g_gfx_fops =
{
	.open 	= GFX_Open,
	.release= GFX_Close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_GFX_UNLOCKED_IOCTL)
	.ioctl	= GFX_Ioctl,
#else
	.unlocked_ioctl	= GFX_Ioctl,
#endif
	.mmap	= GFX_mmap,
#ifdef SUPPORT_GFX_DEVICE_READ_WRITE_FOPS
	.read 	= GFX_Read,
	.write 	= GFX_Write,
#else
	.read	= NULL,
	.write	= NULL,
#endif
};

static inline char*         GFX_IOC2STR ( UINT32 ioc_cmd )
{
    switch (ioc_cmd)
    {
        case GFX_IO_RESET:                  return "GFX_IO_RESET";
        case GFX_IOR_CHIP_REV_INFO:         return "GFX_IOR_CHIP_REV_INFO";
        case GFX_IOR_GET_CFG: 		        return "GFX_IOR_GET_CFG";
        case GFX_IORW_ALLOC_SURFACE:        return "GFX_IORW_ALLOC_SURFACE";
        case GFX_IOW_FREE_SURFACE:          return "GFX_IOW_FREE_SURFACE";
        case GFX_IORW_GET_SURFACE_SETTING:  return "GFX_IORW_GET_SURFACE_SETTING";
        case GFX_IOW_SET_SURFACE_PALETTE:   return "GFX_IOW_SET_SURFACE_PALETTE";
        case GFX_IOW_SYNC_SURFACE:          return "GFX_IOW_SYNC_SURFACE";
        case GFX_IORW_GET_SURFACE_MEM_INFO: return "GFX_IORW_GET_SURFACE_MEM_INFO";
        case GFX_IOW_BLEND:                 return "GFX_IOW_BLEND";
        case GFX_IOW_MANUAL_BLEND:          return "GFX_IOW_MANUAL_BLEND";
        case GFX_IOW_SET_ALPHA_CONV_TBL:    return "GFX_IOW_SET_ALPHA_CONV_TBL";
        case GFX_IOW_SET_CSC_CONV_TBL:      return "GFX_IOW_SET_CSC_CONV_TBL";
        case GFX_IOR_GET_CMD_DELAY:         return "GFX_IOR_GET_CMD_DELAY";
        case GFX_IOW_SET_CMD_DELAY:         return "GFX_IOW_SET_CMD_DELAY";
        case GFX_IOW_SET_BATCH_RUN_MODE:    return "GFX_IOW_SET_BATCH_RUN_MODE";
        case GFX_IOR_GET_BATCH_RUN_MODE:    return "GFX_IOR_GET_BATCH_RUN_MODE";
		case GFX_IOW_SET_GRAPHIC_SYNC_MODE:	return "GFX_IOW_SET_GRAPHIC_SYNC_MODE";
        case GFX_IOW_WAIT_FOR_SYNC:         return "GFX_IOW_WAIT_FOR_SYNC";
        case GFX_IOR_GET_MEM_STAT:  		return "GFX_IOR_GET_MEM_STAT";
		case GFX_IOW_SW_SCALE:				return "GFX_IOW_SCALE";
		case GFX_IOW_SW_BLIT:				return "GFX_IOW_SW_BLIT";
        default:                            return "Unknown IOCTL";
    }
}

/** lock GFX device */
void            GFX_LockDevice              (void)
{
	OS_LockMutex( &gfx_dev_mtx );
}

/** unlock GFX device */
void            GFX_UnlockDevice            (void)
{
	OS_UnlockMutex( &gfx_dev_mtx );
}


/*========================================================================================
	Implementation Group
========================================================================================*/
/** initialize HAL function list
 *
 */
static void    GFX_InitHAL ( void )
{
    memset( &g_gfx_hal, 0x0, sizeof(GFX_HAL_T));

    /* initialize HAL (Hardware Abstraction Layer) */
    if( lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
    {
        GFX_L9_InitHAL( &g_gfx_hal );
    }
    else
    {
        GFX_L8_InitHAL( &g_gfx_hal );
    }
}

/** initialize GFX environment before the real initialization
 *
 *	@note GFX_InitHAL should be called before calling any other functions.
 *	@note GFX_InitCFG should be called before calling any other functions.
 */
void	GFX_PreInit(void)
{
	GFX_InitHAL( );
	GFX_InitCfg( );
}

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
static int GFX_suspend(struct platform_device *pdev, pm_message_t state)
{
	GFX_DRVDATA_T *drv_data;
	int count = 0;

	drv_data = platform_get_drvdata(pdev);

	// add here the suspend code
/*
	while(1)
	{
		if(GFX_IsGfxIdle() == TRUE)
			break;
		if(count++ > 10)
			return -1;
		else
		{
			mdelay(10);
		}

	}

*/
#if 1
	GFX_RunSuspend();
	drv_data->is_suspended = 1;
	GFX_PRINT("[%s] done suspend\n", GFX_MODULE);
#endif
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
static int GFX_resume(struct platform_device *pdev)
{
	GFX_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;

	// add here the resume code
	GFX_RunResume();

	drv_data->is_suspended = 0;
	GFX_PRINT("[%s] done resume\n", GFX_MODULE);
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
 int /*__init*/ GFX_probe(struct platform_device *pdev)
{
	GFX_DRVDATA_T *drv_data;

	drv_data = (GFX_DRVDATA_T *)kmalloc(sizeof(GFX_DRVDATA_T) , GFP_KERNEL);

	// add here driver registering code & allocating resource code

	GFX_PRINT("[%s] done probe\n", GFX_MODULE);
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
static int  GFX_remove(struct platform_device *pdev)
{
	GFX_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code

	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	GFX_PRINT("released\n");

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
static void  GFX_release(struct device *dev)
{
	GFX_PRINT("device released\n");
}

/*
 *	module platform driver structure
 */
static struct platform_driver gfx_driver =
{
	.probe          = GFX_probe,
	.suspend        = GFX_suspend,
	.remove         = GFX_remove,
	.resume         = GFX_resume,
	.driver         =
	{
		.name   = GFX_MODULE,
	},
};

static struct platform_device gfx_device = {
	.name = GFX_MODULE,
	.id = 0,
	.id = -1,
	.dev = {
		.release = GFX_release,
	},
};
#endif

/** initialize GFX deivce. this function is called from base_dev_cfg.c for the large single kernel driver
 *
 *
 */
int		GFX_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for gfx device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_gfx_trace_depth	= 0;
	g_gfx_debug_fd		= DBG_OPEN( GFX_MODULE );

	if ( g_gfx_debug_fd < 0 )
	{
		printk("[GFX] can't get debug handle\n");
	}
	else
	{
		OS_DEBUG_EnableModule ( g_gfx_debug_fd );
//		OS_DEBUG_EnableModuleByIndex ( g_gfx_debug_fd, 0, DBG_COLOR_NONE );		// print
//		OS_DEBUG_EnableModuleByIndex ( g_gfx_debug_fd, 1, DBG_COLOR_GREEN );	// trace
//		OS_DEBUG_EnableModuleByIndex ( g_gfx_debug_fd, 2, DBG_COLOR_YELLOW );	// warn
		OS_DEBUG_EnableModuleByIndex ( g_gfx_debug_fd, 3, DBG_COLOR_RED );		// error
	}

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 */
#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&gfx_driver) < 0)
	{
		GFX_PRINT("[%s] platform driver register failed\n",GFX_MODULE);
	}
	else
	{
		if(platform_device_register(&gfx_device))
		{
			platform_driver_unregister(&gfx_driver);
			GFX_PRINT("[%s] platform device register failed\n",GFX_MODULE);
		}
		else
		{
			GFX_PRINT("[%s] platform register done\n", GFX_MODULE);
		}
	}
#endif
	g_gfx_device = (GFX_DEVICE_T*)OS_Malloc( sizeof(GFX_DEVICE_T)*GFX_MAX_DEVICE );

	if ( NULL == g_gfx_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(GFX_DEVICE_T)* GFX_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_gfx_device, 0x0, sizeof(GFX_DEVICE_T)* GFX_MAX_DEVICE );

	if (g_gfx_major)
	{
		dev = MKDEV( g_gfx_major, g_gfx_minor );
		err = register_chrdev_region(dev, GFX_MAX_DEVICE, GFX_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_gfx_minor, GFX_MAX_DEVICE, GFX_MODULE );
		g_gfx_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register gfx device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */
	OS_InitMutex ( &gfx_dev_mtx, OS_SEM_ATTR_DEFAULT );

	GFX_InitHW();				/* initialize H/W for GFX to be used from kdriver initialization */

/* defense code not to call malloc_ex - COSMO - 2012.04.04 */
/* reviewer: sinchul.jung, gyoseok.chu and seokgan.jung */
#if 1
    GFX_InitSurfaceMemory();   /* initialize dynamic surface memory */
#endif

    GFX_SetCommandDelay(0x0);	/* set default cmd delay. the value came from verification test */

	/* END */

	for ( i=0; i<GFX_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_gfx_major, g_gfx_minor+i );
		cdev_init( &(g_gfx_device[i].cdev), &g_gfx_fops );
		g_gfx_device[i].devno		= dev;
		g_gfx_device[i].cdev.owner	= THIS_MODULE;
		g_gfx_device[i].cdev.ops	= &g_gfx_fops;

		/* TODO: initialize minor device */

		/* END */
		err = cdev_add (&(g_gfx_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding gfx device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}

		OS_CreateDeviceClass ( g_gfx_device[i].devno, "%s%d", GFX_MODULE, i );
	}

	/* initialize proc system */
	GFX_PROC_Init ( );

	GFX_PRINT("gfx device initialized\n");

	return 0;
}

void GFX_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_gfx_major, g_gfx_minor );

#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	platform_driver_unregister(&gfx_driver);
	platform_device_unregister(&gfx_device);
#endif

	/* cleanup proc system */
	GFX_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<GFX_MAX_DEVICE;i++)
	{
		/* TODO: cleanup each minor device */


		/* END */
		cdev_del( &(g_gfx_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */

	unregister_chrdev_region(dev, GFX_MAX_DEVICE );

	OS_Free( g_gfx_device );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for gfx device
 *
 */
static int
GFX_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    GFX_DEVICE_T*		my_dev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, GFX_DEVICE_T, cdev);

    /* TODO : add your device specific code */
	if ( my_dev->dev_open_count == 0 )
	{
		/* do nothing */
	}

	/* END */
    my_dev->dev_open_count++;
    filp->private_data = my_dev;

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);

    GFX_PRINT("device opened (%d:%d)\n", major, minor );

    return 0;
}

/**
 * release handler for gfx device
 *
 */
static int
GFX_Close(struct inode *inode, struct file *file)
{
    int					major,minor;
    GFX_DEVICE_T*	my_dev;
    struct cdev*		cdev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, GFX_DEVICE_T, cdev);

    if ( my_dev->dev_open_count > 0 )
    {
        --my_dev->dev_open_count;
    }

    /* TODO : add your device specific code */

	/* END */

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    GFX_PRINT("device closed (%d:%d)\n", major, minor );
    return 0;
}

/**
 * memory mapping to virtual region
 *
 */
static int GFX_mmap(struct file *file, struct vm_area_struct *vma)
{
	int	ret;
	GFX_PRINT("%s : BEGIN\n", __F__ );
	ret = GFX_SurfaceMmap(vma);
	GFX_PRINT("%s : END\n", __F__ );

	return ret;
}

/**
 * ioctl handler for gfx device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_GFX_UNLOCKED_IOCTL)
static int GFX_Ioctl ( struct inode* inode, struct file *filp, UINT32 cmd, ULONG arg )
#else
static long GFX_Ioctl ( struct file* filp, UINT32 cmd, ULONG arg )
#endif
{
    int err = 0, ret = 0;

    GFX_DEVICE_T*	my_dev;
    struct cdev*	cdev;

	LX_GFX_SURFACE_SETTING_PARAM_T*		surface;

	/*
	 * get current gfx device object
	 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SUPPORT_GFX_UNLOCKED_IOCTL)
	cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, GFX_DEVICE_T, cdev);
#else
	my_dev	= (GFX_DEVICE_T*)filp->private_data;
	cdev	= &my_dev->cdev;
#endif

	GFX_TRACE_BEGIN();

    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != GFX_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
    	GFX_TRACE_END();
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > GFX_IOC_MAXNR)
    {
    	DBG_PRINT_WARNING("out of ioctl command. cmd_idx=%d\n", _IOC_NR(cmd) );
    	GFX_TRACE_END();
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
    	GFX_TRACE_END();
        return -EFAULT;
	}

	GFX_PRINT("%s : IOC 0x%08x (%s:%d)\n", __F__, cmd, GFX_IOC2STR(cmd), _IOC_NR(cmd));

	switch(cmd)
	{
		case GFX_IO_RESET:
		{
			GFX_PRINT("reset ok\n");
			ret = 0;
		}
		break;

		case GFX_IOR_CHIP_REV_INFO:
		{
			LX_CHIP_REV_INFO_T	rev_info;

			rev_info.version = 0x100;
			rev_info.date[0] =  9;	/* 2009/11/24 */
			rev_info.date[0] = 11;
			rev_info.date[0] = 24;

	        if ( copy_to_user((void __user *)arg, &rev_info, sizeof(CHIP_REV_INFO_T)) )
	        {
	           	GFX_TRACE_END();
    	        return -EFAULT;
			}

			ret = 0;
		}
		break;

		case GFX_IOR_GET_CFG:
		{
			err = copy_to_user((void __user *)arg, (void *)&g_gfx_cfg , sizeof(LX_GFX_CFG_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");

			ret = RET_OK; /* all work done */
		}
		break;

		case GFX_IORW_ALLOC_SURFACE :
		{
			surface = (LX_GFX_SURFACE_SETTING_PARAM_T *)kmalloc(sizeof(LX_GFX_SURFACE_SETTING_PARAM_T),GFP_KERNEL);
			ret = copy_from_user((void *)surface, (void __user *)arg, sizeof(LX_GFX_SURFACE_SETTING_PARAM_T));

			GFX_LockDevice();
			ret = GFX_AllocSurface(surface);

			if(ret >= 0)
			{
				ret = copy_to_user((void __user *)arg, (void *)surface , sizeof(LX_GFX_SURFACE_SETTING_PARAM_T));
			}
			else
			{
				memset((void*)surface , 0 , sizeof(LX_GFX_SURFACE_SETTING_PARAM_T));
			}
			GFX_UnlockDevice();

			kfree(surface);
		}
		break;

		case GFX_IOW_FREE_SURFACE :
		{
			UINT32	surface_fd;

			ret = copy_from_user((void *)&surface_fd, (void __user *)arg, sizeof(UINT32));
			GFX_LockDevice();
			ret = GFX_FreeSurface(surface_fd);
			GFX_UnlockDevice();
		}
		break;

		case GFX_IORW_GET_SURFACE_SETTING :
		{
			surface = (LX_GFX_SURFACE_SETTING_PARAM_T *)kmalloc(sizeof(LX_GFX_SURFACE_SETTING_PARAM_T),GFP_KERNEL);
			ret = copy_from_user((void *)surface, (void __user *)arg, sizeof(LX_GFX_SURFACE_SETTING_PARAM_T));
			GFX_LockDevice();
			GFX_GetAllocSurface(surface);
			GFX_UnlockDevice();
			ret = copy_to_user((void __user *)arg, (void *)surface, sizeof(LX_GFX_SURFACE_SETTING_PARAM_T));
			kfree(surface);
		}
		break;

		case GFX_IOW_SET_SURFACE_PALETTE :
		{
			LX_GFX_SURFACE_PALETTE_PARAM_T		param;
			LX_GFX_SURFACE_PALETTE_PARAM_T*		input_param = (LX_GFX_SURFACE_PALETTE_PARAM_T*)arg;

			/*	raxis.lim (2010/06/05)
			 *	--	palette data is passed as pointer, so we should copy it from user space
		 	 */
			param.palette.palette_data 	= (UINT32*)OS_Malloc(sizeof(UINT32)*256);

			if ( __get_user( param.surface_fd, &(input_param->surface_fd) ) )
			{
				ret = RET_ERROR; break;
			}

			if ( __get_user( param.palette.palette_num, &(input_param->palette.palette_num) ) )
			{
				ret = RET_ERROR; break;
			}

			ret = copy_from_user((void*)param.palette.palette_data,
								 (void __user*)input_param->palette.palette_data,
								sizeof(UINT32)*input_param->palette.palette_num );

			GFX_LockDevice();
			GFX_SetSurfacePalette( param.surface_fd, param.palette.palette_num , param.palette.palette_data );
			GFX_UnlockDevice();

			OS_Free(param.palette.palette_data);
		}
		break;

		case GFX_IOW_SYNC_SURFACE :

		break;

		case GFX_IOW_BLEND :
		case GFX_IOW_MANUAL_BLEND :
		{
			int	retry;
			LX_GFX_MANUAL_BLEND_CTRL_PARAM_T manual_blend;

			memset( &manual_blend, 0x0, sizeof(LX_GFX_MANUAL_BLEND_CTRL_PARAM_T));
			err = copy_from_user((void *)&manual_blend, (void __user *)arg, sizeof(LX_GFX_MANUAL_BLEND_CTRL_PARAM_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");

			GFX_LockDevice();

			for ( retry=g_gfx_cfg.sync_fail_retry_count ; retry>0 ; retry-- )
			{
				ret = GFX_RunCustomBlitDraw(&manual_blend);

			    /* when batch mode is auto, driver automatically flush queue (start batch run) */
			    if(GFX_GetRunCommand() == LX_GFX_BATCH_RUN_MODE_AUTO)
				{
					GFX_RunFlushCommand();
				}

				/* raxis.lim (2011/05/21)
				 * since there is some bugs in L9A0 scaler read/write timing problem, I should add s/w workaround
				 * to do "soft reset" for GFX block every scaler request.
				 * [NOTE] check if L9B0 fixes this bug
				 */
				if ( GFX_GetGraphicSyncMode() == LX_GFX_GRAPHIC_SYNC_MODE_AUTO )
				{
					UINT32	timeout = GFX_CalcSyncTimeout( &manual_blend, g_gfx_cfg.sync_wait_timeout );
					ret = GFX_WaitSyncCommand( timeout );

					if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )
					{
						if ( manual_blend.scaler.mode ) GFX_SwResetCommand();
					}
				}

				/* if GFX operation failed, retry it */
				if ( ret == RET_OK ) break;
#ifdef GFX_USE_NOISY_ERR_DEBUG
				/* if ERROR, report the requet info */
				GFX_DumpBlitParam ( &manual_blend );
#endif
			}
			GFX_UnlockDevice();
		}
		break;

		case GFX_IOW_SW_SCALE:
		{
			LX_GFX_SW_SCALE_CTRL_PARAM_T scale_param;
			ret = copy_from_user((void *)&scale_param, (void __user *)arg, sizeof(LX_GFX_SW_SCALE_CTRL_PARAM_T));
			ret = GFX_RunSwScale( &scale_param );
		}
		break;

		case GFX_IOW_SW_BLIT:
		{
			LX_GFX_SW_BLIT_CTRL_PARAM_T blit_param;
			ret = copy_from_user((void *)&blit_param, (void __user *)arg, sizeof(LX_GFX_SW_BLIT_CTRL_PARAM_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");

			ret = GFX_RunSwBlit( &blit_param );
		}
		break;

		case GFX_IOW_SET_ALPHA_CONV_TBL :
		{
			LX_GFX_ALPHA_CONV_TBL_T alpha_conv;
			ret = copy_from_user((void *)&alpha_conv, (void __user *)arg, sizeof(LX_GFX_ALPHA_CONV_TBL_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");

			ret = GFX_SetGlobalAlpha(alpha_conv.alpha);
		}
		break;

		case GFX_IOW_SET_CSC_CONV_TBL:
		{
			LX_GFX_CSC_TBL_T csc_conv;
			ret = copy_from_user((void *)&csc_conv, (void __user *)arg, sizeof(LX_GFX_CSC_TBL_T));

			ret = GFX_SetColorSpace(csc_conv.coef);
		}
		break;

		case GFX_IOR_GET_CMD_DELAY :
		{
			UINT32	cmd_delay;

			GFX_GetCommandDelay(&cmd_delay);

			ret = copy_to_user((void __user *)arg, (void *)&cmd_delay, sizeof(UINT32));
		}
		break;

		case GFX_IOW_SET_CMD_DELAY :
		{
			UINT32	cmd_delay;

			ret = copy_from_user((void *)&cmd_delay, (void __user *)arg, sizeof(UINT32));

			GFX_SetCommandDelay(cmd_delay);
		}
		break;

		case GFX_IORW_GET_SURFACE_MEM_INFO:
		{
			LX_GFX_SURFACE_MEM_INFO_PARAM_T mem_info;

			ret = copy_from_user((void *)&mem_info, (void __user *)arg, sizeof(LX_GFX_SURFACE_MEM_INFO_PARAM_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");

			GFX_LockDevice();
			(void)GFX_GetSurfaceMemory(&mem_info);
			GFX_UnlockDevice();

			ret = copy_to_user((void __user *)arg, (void *)&mem_info, sizeof(LX_GFX_SURFACE_MEM_INFO_PARAM_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");
		}
		break;

		case GFX_IOW_SET_BATCH_RUN_MODE:
		{
		    LX_GFX_BATCH_RUN_MODE_T	run_mode;

			ret = copy_from_user((void *)&run_mode, (void __user *)arg, sizeof(LX_GFX_BATCH_RUN_MODE_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");

			GFX_SetRunCommand(&run_mode);
		}
		break;

		case GFX_IOR_GET_BATCH_RUN_MODE:
		{
		    LX_GFX_BATCH_RUN_MODE_T	run_mode;

			run_mode = GFX_GetRunCommand();
			ret = copy_to_user((void __user *)arg, (void *)&run_mode, sizeof(LX_GFX_BATCH_RUN_MODE_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");
		}
		break;

		case GFX_IO_START_BATCH_RUN:
		{
			GFX_RunFlushCommand();
		}
		break;

		case GFX_IOW_SET_GRAPHIC_SYNC_MODE:
		{
			LX_GFX_GRAPHIC_SYNC_MODE_T	mode;

			ret = copy_from_user((void *)&mode, (void __user *)arg, sizeof(LX_GFX_GRAPHIC_SYNC_MODE_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");
			GFX_SetGraphicSyncMode(mode);
		}
		break;

		case GFX_IOW_WAIT_FOR_SYNC:
		{
			UINT32	msec_tm;

			ret = copy_from_user((void *)&msec_tm, (void __user *)arg, sizeof(UINT32));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");
			GFX_WaitSyncCommand(msec_tm);
		}
		break;

		case GFX_IOR_GET_MEM_STAT:
		{
			LX_GFX_MEM_STAT_T	mem_stat;
			(void)GFX_GetSurfaceMemoryStat( &mem_stat );

			ret = copy_to_user((void __user *)arg, (void *)&mem_stat, sizeof(LX_GFX_MEM_STAT_T));
            GFX_CHECK_CODE( err > 0, break, "copy error\n");
		}
		break;

	    default:
	    {
			/* redundant check but it seems more readable */
    	    ret = -ENOTTY;
		}
    }

	GFX_PRINT("%s : IOC 0x%08x (%s:%d) -- ret %d\n", __F__, cmd, GFX_IOC2STR(cmd), _IOC_NR(cmd), ret );
	GFX_TRACE_END();

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",GFX_Init);
#else
module_init(GFX_Init);
#endif
module_exit(GFX_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

