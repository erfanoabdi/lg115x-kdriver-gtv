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
 *  main driver implementation for i2c device.
 *	i2c device will teach you how to make device driver with new platform.
 *
 *  author		ingyu.yang (ingyu.yang@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_i2c
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/


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
#include <mach/platform.h>
#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif
#include "os_util.h"
#include "base_dev_cfg.h"
#include "i2c_drv.h"
#include "base_types.h"
#include "i2c_core.h"

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
 *	main control block for i2c device.
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
	LX_I2C_DEV_HANDLE		handle;

// END of device specific data
}
I2C_DEVICE_T;

#ifdef KDRV_CONFIG_PM
typedef struct
{
	// add here extra parameter
	bool	is_suspended;
}I2C_DRVDATA_T;
#endif

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	I2C_PROC_Init(void);
extern	void	I2C_PROC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		I2C_Init(void);
void	I2C_Cleanup(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_i2c_debug_fd;
int 	g_i2c_major = I2C_MAJOR;
int 	g_i2c_minor = I2C_MINOR;

int		g_i2c_num_devices;
/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      I2C_Open(struct inode *, struct file *);
static int      I2C_Close(struct inode *, struct file *);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
static int 		I2C_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#else
static long		I2C_Ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif
#ifdef SUPPORT_I2C_DEVICE_READ_WRITE_FOPS
static ssize_t  I2C_Read(struct file *, char *, size_t, loff_t *);
static ssize_t  I2C_Write(struct file *, const char *, size_t, loff_t *);
#endif

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/**
 * main control block for i2c device
*/
static I2C_DEVICE_T*		g_i2c_device;


/**
 * file I/O description for i2c device
 *
*/
static struct file_operations g_i2c_fops =
{
	.open 	= I2C_Open,
	.release= I2C_Close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
	.ioctl	= I2C_Ioctl,
#else
	.unlocked_ioctl = I2C_Ioctl,
#endif
#ifdef SUPPORT_I2C_DEVICE_READ_WRITE_FOPS
	.read 	= I2C_Read,
	.write 	= I2C_Write,
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
static int I2C_suspend(struct platform_device *pdev, pm_message_t state)
{
	I2C_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	// add here the suspend code
	I2C_DevSuspendAll();


	drv_data->is_suspended = 1;
	printk("[%s] done suspend\n", I2C_MODULE);

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
static int I2C_resume(struct platform_device *pdev)
{
	I2C_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;


	// add here the resume code
	I2C_DevResumeAll();

	drv_data->is_suspended = 0;
	printk("[%s] done resume\n", I2C_MODULE);
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
 int  I2C_probe(struct platform_device *pdev)
{

	I2C_DRVDATA_T *drv_data;

	drv_data = (I2C_DRVDATA_T *)kmalloc(sizeof(I2C_DRVDATA_T) , GFP_KERNEL);


	// add here driver registering code & allocating resource code



	I2C_PRINT("[%s] done probe\n", I2C_MODULE);
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
static int  I2C_remove(struct platform_device *pdev)
{
	I2C_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code



	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	I2C_PRINT("released\n");

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
static void  I2C_release(struct device *dev)
{


	I2C_PRINT("device released\n");
}

/*
 *	module platform driver structure
 */
static struct platform_driver i2c_driver =
{
	.probe          = I2C_probe,
	.suspend        = I2C_suspend,
	.remove         = I2C_remove,
	.resume         = I2C_resume,
	.driver         =
	{
		.name   = I2C_MODULE,
	},
};

static struct platform_device i2c_device = {
	.name = I2C_MODULE,
	.id = 0,
	.id = -1,
	.dev = {
		.release = I2C_release,
	},
};
#endif

int I2C_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for i2c device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_i2c_debug_fd = DBG_OPEN( I2C_MODULE );
	if (g_i2c_debug_fd < 0) return -1;

	OS_DEBUG_EnableModule ( g_i2c_debug_fd );

	//OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 0, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 1, DBG_COLOR_RED );
	OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 2, DBG_COLOR_YELLOW);
	//OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 3, DBG_COLOR_NONE );


	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&i2c_driver) < 0)
	{
		I2C_PRINT("[%s] platform driver register failed\n",I2C_MODULE);

	}
	else
	{
		if(platform_device_register(&i2c_device))
		{
			platform_driver_unregister(&i2c_driver);
			I2C_PRINT("[%s] platform device register failed\n",I2C_MODULE);
		}
		else
		{
			I2C_PRINT("[%s] platform register done\n", I2C_MODULE);
		}


	}
#endif

	/* TODO : initialize your module not specific minor device */
	I2C_CoreInit();
	g_i2c_num_devices = I2C_CoreGetDeviceNums();


	g_i2c_device = (I2C_DEVICE_T*)OS_KMalloc( sizeof(I2C_DEVICE_T)*g_i2c_num_devices );

	if ( NULL == g_i2c_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(I2C_DEVICE_T)* g_i2c_num_devices );
		return -ENOMEM;
	}

	memset( g_i2c_device, 0x0, sizeof(I2C_DEVICE_T)* g_i2c_num_devices );

	if (g_i2c_major)
	{
		dev = MKDEV( g_i2c_major, g_i2c_minor );
		err = register_chrdev_region(dev, g_i2c_num_devices, I2C_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_i2c_minor, g_i2c_num_devices, I2C_MODULE );
		g_i2c_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register i2c device\n" );
		return -EIO;
	}

	/* END */

	for ( i=0; i<g_i2c_num_devices; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_i2c_major, g_i2c_minor+i );
		cdev_init( &(g_i2c_device[i].cdev), &g_i2c_fops );
		g_i2c_device[i].devno		= dev;
		g_i2c_device[i].cdev.owner = THIS_MODULE;
		g_i2c_device[i].cdev.ops   = &g_i2c_fops;

		/* TODO: initialize minor device */


		/* END */

		err = cdev_add (&(g_i2c_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding i2c device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}
        OS_CreateDeviceClass ( g_i2c_device[i].devno, "%s%d", I2C_MODULE, i );
	}

	/* initialize proc system */
	I2C_PROC_Init ( );

	I2C_DEBUG("i2c device initialized\n");

	return 0;
}

void I2C_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_i2c_major, g_i2c_minor );

#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	platform_driver_unregister(&i2c_driver);
	platform_device_unregister(&i2c_device);
#endif

	/* cleanup proc system */
	I2C_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<g_i2c_num_devices;i++)
	{
		/* TODO: cleanup each minor device */


		/* END */
		cdev_del( &(g_i2c_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */

	unregister_chrdev_region(dev, g_i2c_num_devices );

	I2C_DEBUG("i2c device cleanup\n");

	OS_Free( g_i2c_device );
	g_i2c_num_devices = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for i2c device
 *
 */
static int
I2C_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    I2C_DEVICE_T*		i2c_dev;

    cdev	= inode->i_cdev;
    i2c_dev	= container_of ( cdev, I2C_DEVICE_T, cdev);

    major = imajor(inode);
    minor = iminor(inode);

	if ( minor >= g_i2c_num_devices )
	{
		I2C_PRINT_ERROR( "Invalid device num[%d] for Open!!\n", minor );
		return -1;
	}

	if(i2c_dev->dev_open_count == 0)
	{
		LX_I2C_DEV_HANDLE handle = I2C_DevOpen(minor);

		if(handle == NULL)
		{
			I2C_PRINT_ERROR( "I2C device error...\n");
			return -EIO;
		}
		i2c_dev->handle = handle;
	}

    i2c_dev->dev_open_count++;
    filp->private_data = i2c_dev;

    I2C_PRINT("device opened (%d:%d)\n", major, minor );

    return 0;
}

/**
 * release handler for i2c device
 *
 */
static int
I2C_Close(struct inode *inode, struct file *file)
{
    int					major,minor;
    I2C_DEVICE_T*		i2c_dev;
    struct cdev*		cdev;

    cdev	= inode->i_cdev;
    i2c_dev	= container_of ( cdev, I2C_DEVICE_T, cdev);

    if ( i2c_dev->dev_open_count > 0 )
    {
        --i2c_dev->dev_open_count;

		if(i2c_dev->dev_open_count == 0)
		{
			/* UnInstall the device */
			//I2C_DevClose(i2c_dev->handle);
		}
    }
    major = imajor(inode);
    minor = iminor(inode);

	/* END */

	/* some debug */
    I2C_DEBUG("device closed (%d:%d)\n", major, minor );

    return 0;
}

/**
 * ioctl handler for i2c device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
static int I2C_Ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static long I2C_Ioctl (struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
	I2C_DEVICE_T*	i2c_dev = (I2C_DEVICE_T*)file->private_data;

	switch(cmd)
	{
		case LXDRV_IIC_INIT:
		{
			LX_SW_I2C_GPIO_MAP_T map;
			if (copy_from_user((void*)&map, (void __user *)arg, sizeof(LX_SW_I2C_GPIO_MAP_T)))
			{
				I2C_PRINT("ioctl: copy_from_user error !!! \n");
				return -EFAULT;
			}
			return I2C_DevInit(i2c_dev->handle, &map);
		}
		case LXDRV_IIC_ENABLE:
			break;

		case LXDRV_IIC_CLOCK:
			return I2C_DevSetClock(i2c_dev->handle, (UINT8)arg);

		case LXDRV_IIC_WRITE:
		case LXDRV_IIC_READ:
		{
			int res;
			LX_I2C_RW_DATA_T	rw_data;
			UINT8	sbuf[32];
			UINT8*	mbuf = NULL;
			UINT8*	kbuf;
			UINT8*	ubuf;

			if (copy_from_user((void*)&rw_data, (void __user *)arg, sizeof(LX_I2C_RW_DATA_T)))
			{
				I2C_PRINT("ioctl: copy_from_user error !!! \n");
				return -EFAULT;
			}

			/* To prevent the defect(TAINTED_SCALAR) */
			if(rw_data.bufSize > (32*1024))
				return -EFAULT;

			ubuf = rw_data.buf;

			if(rw_data.bufSize > 32)
			{
				mbuf = (UINT8*)OS_Malloc(rw_data.bufSize);
				kbuf = mbuf;
			}
			else
			{
				kbuf = sbuf;
			}

			if(kbuf == NULL) return -ENOMEM;

			if(cmd == LXDRV_IIC_WRITE)
			{
				if(copy_from_user(kbuf, (void __user *)ubuf, rw_data.bufSize))
				{
					if(mbuf != NULL) OS_Free(mbuf);
					return -EFAULT;
				}
			}

			rw_data.buf = kbuf;
			res = I2C_DevTransfer(i2c_dev->handle, &rw_data,
								(cmd == LXDRV_IIC_WRITE) ? I2C_WRITE_MODE : I2C_READ_MODE);

			if(cmd == LXDRV_IIC_READ && res >= 0)
			{
				if(copy_to_user((void __user *)ubuf, (void*)kbuf, rw_data.bufSize) != 0)
				{
					if(mbuf != NULL) OS_Free(mbuf);
					return -EFAULT;
				}
			}
			if(mbuf != NULL) OS_Free(mbuf);

			return res;
		}

		case LXDRV_IIC_ENABLE_PRINT:
			if ( arg )
			{
				//Enable print
				printk ( "Enable I2C Kernel driver print\n" );
				OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 0, DBG_COLOR_NONE );
				OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 1, DBG_COLOR_NONE );
				OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 2, DBG_COLOR_NONE );
				OS_DEBUG_EnableModuleByIndex ( g_i2c_debug_fd, 3, DBG_COLOR_NONE );
			}
			else
			{
				//Disable print
				printk ( "Disable I2C Kernel driver print\n" );
				OS_DEBUG_DisableModuleByIndex ( g_i2c_debug_fd, 0 );
				OS_DEBUG_DisableModuleByIndex ( g_i2c_debug_fd, 1 );
				OS_DEBUG_DisableModuleByIndex ( g_i2c_debug_fd, 2 );
				OS_DEBUG_DisableModuleByIndex ( g_i2c_debug_fd, 3 );
			}
			break;

		default:
			I2C_DEBUG("ioctl: default\n");
			/* redundant check but it seems more readable */
			return -ENOTTY;
	}

	return 0;
}

/* R/W functions are used with ioctl */
#if 0
static ssize_t I2C_Read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	I2C_DEVICE_T*	i2c_dev = (I2C_DEVICE_T*)filp->private_data;
	LX_I2C_RW_DATA_T  rw_data;

	if( copy_from_user((void*)&rw_data, (void __user *)buf, sizeof(LX_I2C_RW_DATA_T)) )
	{
		return -1;
	}

	return I2C_DevTransfer(i2c_dev->handle, &rw_data, I2C_READ_MODE, 0);
}

static ssize_t I2C_Write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	I2C_DEVICE_T*	i2c_dev = (I2C_DEVICE_T*)filp->private_data;
	LX_I2C_RW_DATA_T  rw_data;

	if( copy_from_user((void*)&rw_data, (void __user *)buf, sizeof(LX_I2C_RW_DATA_T)) )
	{
		return -1;
	}

	/* Write to the I2C device */
	return I2C_DevTransfer(i2c_dev->handle, &rw_data, I2C_WRITE_MODE, 0);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",I2C_Init);
#else
module_init(I2C_Init);
#endif
module_exit(I2C_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

