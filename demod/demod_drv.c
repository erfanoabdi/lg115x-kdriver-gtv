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
 *  main driver implementation for demod device.
 *	demod device will teach you how to make device driver with new platform.
 *
 *  author		jeongpil.yun (jeongpil.yun@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_demod
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_DEMOD_DEVICE_READ_WRITE_FOPS

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif
#include "os_util.h"
#include "base_dev_cfg.h"
#include "demod_drv.h"

#include "demod_module.h"

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
 *	main control block for demod device.
 *	each minor device has unique control block
 *
 */
typedef struct
{
// BEGIN of common device
	int						dev_open_count;		///< check if device is opened or not
	dev_t					devno;			///< device number
	struct cdev				cdev;			///< char device structure
// END of command device

// BEGIN of device specific data


// END of device specific data
}
DEMOD_DEVICE_T;

#ifdef KDRV_CONFIG_PM
typedef struct
{
	// add here extra parameter
	bool	is_suspended;
}DEMOD_DRVDATA_T;

#endif
/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	DEMOD_PROC_Init(void);
extern	void	DEMOD_PROC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		DEMOD_Init(void);
void	DEMOD_Cleanup(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_demod_debug_fd;
int 	g_demod_major = DEMOD_MAJOR;
int 	g_demod_minor = DEMOD_MINOR;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      DEMOD_Open(struct inode *, struct file *);
static int      DEMOD_Close(struct inode *, struct file *);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int 		DEMOD_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#else
static long		DEMOD_Ioctl (struct file *, unsigned int, unsigned long );
#endif


#ifdef SUPPORT_DEMOD_DEVICE_READ_WRITE_FOPS
static ssize_t  DEMOD_Read(struct file *, char *, size_t, loff_t *);
static ssize_t  DEMOD_Write(struct file *, const char *, size_t, loff_t *);
#endif

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/**
 * main control block for demod device
*/
static DEMOD_DEVICE_T*		g_demod_device;

/**
 * file I/O description for demod device
 *
*/
static struct file_operations g_demod_fops =
{
	.open 	= DEMOD_Open,
	.release= DEMOD_Close,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl	= DEMOD_Ioctl,
#else
	.unlocked_ioctl= DEMOD_Ioctl,
#endif
#ifdef SUPPORT_DEMOD_DEVICE_READ_WRITE_FOPS
	.read 	= DEMOD_Read,
	.write 	= DEMOD_Write,
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
static int DEMOD_suspend(struct platform_device *pdev, pm_message_t state)
{
	DEMOD_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	// add here the suspend code

	DEMOD_HW_suspend();

	drv_data->is_suspended = 1;
	DEMOD_PRINT("[%s] done suspend\n", DEMOD_MODULE);

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
static int DEMOD_resume(struct platform_device *pdev)
{
	DEMOD_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;


	// add here the resume code

	DEMOD_ResetHW();

	DEMOD_HW_resume();

	drv_data->is_suspended = 0;
	DEMOD_PRINT("[%s] done resume\n", DEMOD_MODULE);
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
static int __init DEMOD_probe(struct platform_device *pdev)
{

	DEMOD_DRVDATA_T *drv_data;

	drv_data = (DEMOD_DRVDATA_T *)kmalloc(sizeof(DEMOD_DRVDATA_T) , GFP_KERNEL);


	// add here driver registering code & allocating resource code



	DEMOD_PRINT("[%s] done probe\n", DEMOD_MODULE);
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
static int  DEMOD_remove(struct platform_device *pdev)
{
	DEMOD_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code



	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	DEMOD_PRINT("released\n");

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
static void  DEMOD_release(struct device *dev)
{


	DEMOD_PRINT("device released\n");
}

/*
 *	module platform driver structure
 */
static struct platform_driver demod_driver =
{
	.probe          = DEMOD_probe,
	.suspend        = DEMOD_suspend,
	.remove         = DEMOD_remove,
	.resume         = DEMOD_resume,
	.driver         =
	{
		.name   = DEMOD_MODULE,
	},
};

static struct platform_device demod_device = {
	.name = DEMOD_MODULE,
	.id = 0,
	.id = -1,
	.dev = {
		.release = DEMOD_release,
	},
};
#endif
int DEMOD_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for demod device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_demod_debug_fd = DBG_OPEN( DEMOD_MODULE );
	if(g_demod_debug_fd < 0) return -1;
	//OS_DEBUG_EnableModule ( g_demod_debug_fd );

	OS_DEBUG_EnableModuleByIndex ( g_demod_debug_fd, 0, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_demod_debug_fd, 1, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_demod_debug_fd, 2, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_demod_debug_fd, 3, DBG_COLOR_NONE );

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&demod_driver) < 0)
	{
		DEMOD_PRINT("[%s] platform driver register failed\n",DEMOD_MODULE);

	}
	else
	{
		if(platform_device_register(&demod_device))
		{
			platform_driver_unregister(&demod_driver);
			DEMOD_PRINT("[%s] platform device register failed\n",DEMOD_MODULE);
		}
		else
		{
			DEMOD_PRINT("[%s] platform register done\n", DEMOD_MODULE);
		}


	}
#endif

	g_demod_device = (DEMOD_DEVICE_T*)OS_KMalloc( sizeof(DEMOD_DEVICE_T)*DEMOD_MAX_DEVICE );

	if ( NULL == g_demod_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(DEMOD_DEVICE_T)* DEMOD_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_demod_device, 0x0, sizeof(DEMOD_DEVICE_T)* DEMOD_MAX_DEVICE );

	if (g_demod_major)
	{
		dev = MKDEV( g_demod_major, g_demod_minor );
		err = register_chrdev_region(dev, DEMOD_MAX_DEVICE, DEMOD_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_demod_minor, DEMOD_MAX_DEVICE, DEMOD_MODULE );
		g_demod_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register demod device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */
	DEMOD_Initialize();

	/* END */

	for ( i=0; i<DEMOD_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_demod_major, g_demod_minor+i );
		cdev_init( &(g_demod_device[i].cdev), &g_demod_fops );
		g_demod_device[i].devno		= dev;
		g_demod_device[i].cdev.owner = THIS_MODULE;
		g_demod_device[i].cdev.ops   = &g_demod_fops;

		/* TODO: initialize minor device */


		/* END */

		err = cdev_add (&(g_demod_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding demod device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}
        OS_CreateDeviceClass ( g_demod_device[i].devno, "%s%d", DEMOD_MODULE, i );
	}

	/* initialize proc system */
	DEMOD_PROC_Init ( );

	DEMOD_PRINT("demod device initialized\n");

	return 0;
}

void DEMOD_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_demod_major, g_demod_minor );

#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	platform_driver_unregister(&demod_driver);
	platform_device_unregister(&demod_device);
#endif

	/* cleanup proc system */
	DEMOD_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<DEMOD_MAX_DEVICE;i++)
	{
		/* TODO: cleanup each minor device */


		/* END */
		cdev_del( &(g_demod_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */

	unregister_chrdev_region(dev, DEMOD_MAX_DEVICE );

	OS_Free( g_demod_device );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for demod device
 *
 */
static int
DEMOD_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    DEMOD_DEVICE_T*	my_dev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, DEMOD_DEVICE_T, cdev);

    /* TODO : add your device specific code */
	if( my_dev->dev_open_count == 0 )
	{

		 /* init H/W */
	}


	/* END */

    my_dev->dev_open_count++;
    filp->private_data = my_dev;



	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    DEMOD_PRINT("device opened (%d:%d)\n", major, minor );

    return 0;
}

/**
 * release handler for demod device
 *
 */
static int
DEMOD_Close(struct inode *inode, struct file *file)
{
    int					major,minor;
    DEMOD_DEVICE_T*	my_dev;
    struct cdev*		cdev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, DEMOD_DEVICE_T, cdev);

    if ( my_dev->dev_open_count > 0 )
    {
        --my_dev->dev_open_count;
    }

    /* TODO : add your device specific code */

	if( my_dev->dev_open_count <= 0 )

	{
		  /* shutdown H/W */
	}

	/* END */

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    DEMOD_PRINT("device closed (%d:%d)\n", major, minor );
    return 0;
}

/**
 * ioctl handler for demod device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int DEMOD_Ioctl ( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
#else
static long DEMOD_Ioctl ( struct file *filp, unsigned int cmd, unsigned long arg )
#endif
{
    int err = 0, ret = 0;

    DEMOD_DEVICE_T*	my_dev;
    struct cdev*		cdev;

	LX_DEMOD_INCLK_SEL_T *inclk;
	LX_DEMOD_BBMODE_T *bbmode;
	LX_DEMOD_TUNER_I2C_SEL_T *tunerI2C;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
#else
	struct inode *inode = filp->f_path.dentry->d_inode;
#endif

	/*
	 * get current demod device object
	 */
	cdev	= inode->i_cdev;
	my_dev	= container_of ( cdev, DEMOD_DEVICE_T, cdev);

    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != DEMOD_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > DEMOD_IOC_MAXNR)
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

	DEMOD_TRACE("cmd = %08X (cmd_idx=%d)\n", cmd, _IOC_NR(cmd) );

	switch(cmd)
	{
		case DEMOD_IO_HW_RESET:
		{
			// CTR61[0] active low
			// Sequence : '1' (GBB ON) -> wait 10ms -> '0'(GBB OFF) -> wait 30ms -> '1'(GBB ON) --> wait 20 ms

			DEMOD_PRINT("DEMOD_IO_HW_RESET ok\n");

			ret = DEMOD_ResetHW();

		}
		break;

		case DEMOD_IO_I2C_SEL:
		{
			// CTR26[15] :
			// set to '0' : normal mode

			DEMOD_PRINT("DEMOD_IO_I2C_SEL ok\n");
    	        	ret = DEMOD_SetI2C();

		}
		break;

		case DEMOD_IOW_INCLK_SEL:
		{
			//CTR35[0]
			//if LX_DEMOD_ARM_SETTING is set,  set to '0',
			//if LX_DEMOD_EXTERN_PIN is set,   set to '1'.

			DEMOD_PRINT("DEMOD_IOW_INCLK_SEL ok\n");

			inclk = (LX_DEMOD_INCLK_SEL_T *)kmalloc(sizeof(LX_DEMOD_INCLK_SEL_T),GFP_KERNEL);
			if ( inclk )
			{

			ret = copy_from_user((void *)inclk, (void *)arg, sizeof(LX_DEMOD_INCLK_SEL_T));

			ret = DEMOD_SetInclk(inclk);

			kfree(inclk);
			}


		}
		break;

		case DEMOD_IOW_BBMODE_SEL:
		{
			// CTOP CTR61[2:1]
			// if the mode is LX_DEMOD_UNKOWN,  set to '00'.
			// if the mode is LX_DEMOD_VSB_QAM, set to '01'.
			// if the mode is LX_DEMOD_DVB_T,   set to '10'
			// if the mode is LX_DEMOD_DVB_C,   set to '11'

			DEMOD_PRINT("DEMOD_IOW_BBMODE_SEL ok\n");

			bbmode = (LX_DEMOD_BBMODE_T *)kmalloc(sizeof(LX_DEMOD_BBMODE_T),GFP_KERNEL);

			if ( bbmode )
			{
				ret = copy_from_user((void *)bbmode, (void *)arg, sizeof(LX_DEMOD_BBMODE_T));

				ret = DEMOD_SetBbmod(bbmode);

				kfree(bbmode);
			}

		}
		break;

		case DEMOD_IOW_TUNERI2C_SEL:
		{
			//CTR35[0]
			//if LX_DEMOD_ARM_SETTING is set,  set to '0',
			//if LX_DEMOD_EXTERN_PIN is set,   set to '1'.

			DEMOD_PRINT("DEMOD_IOW_TUNERI2C_SEL ok\n");

			tunerI2C = (LX_DEMOD_TUNER_I2C_SEL_T *)kmalloc(sizeof(LX_DEMOD_TUNER_I2C_SEL_T),GFP_KERNEL);

			if ( tunerI2C )
			{
				ret = copy_from_user((void *)tunerI2C, (void *)arg, sizeof(LX_DEMOD_TUNER_I2C_SEL_T));

				ret = DEMOD_SetTunerI2C(tunerI2C);

				kfree(tunerI2C);
			}
		}
		break;

	    default:
	    {
			/* redundant check but it seems more readable */
    	    ret = -ENOTTY;
		}
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",DEMOD_Init);
#else
module_init(DEMOD_Init);
#endif
module_exit(DEMOD_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

