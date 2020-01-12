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
 *  main driver implementation for gpio device.
 *	gpio device will teach you how to make device driver with new platform.
 *
 *  author		ingyu.yang (ingyu.yang@lge.com)
 *  version		1.0
 *  date		2009.12.30
 *  note		Additional information.
 *
 *  @addtogroup lg1150_gpio
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_GPIO_DEVICE_READ_WRITE_FOPS
#undef	GPIO_DRV_PRINT_ENABLE
//#define GPIO_DRV_PRINT_ENABLE
//static void Debug_GPIO_Print(void);

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif
#include "mach/platform.h"
#include <asm/uaccess.h>
#include "os_util.h"
#include "base_dev_cfg.h"
#include "gpio_drv.h"
#include "gpio_reg.h"
#include "ctop_regs.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define TOTAL_GPIO8_NUM 12

#define GPIO_MUTEX_INIT(h)				OS_InitMutex(&h->mutex, OS_SEM_ATTR_DEFAULT)
#define GPIO_MUTEX_LOCK(h)				OS_LockMutex(&h->mutex)
#define GPIO_MUTEX_UNLOCK(h)			OS_UnlockMutex(&h->mutex)

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
#if 1
/**
 *	main control block for gpio device.
 *	each minor device has unique control block
 *
 */
typedef struct GPIO_DEVICE_t
{
// BEGIN of common device
	int						dev_open_count;		///< check if device is opened or not
	dev_t					devno;			///< device number
	struct cdev				cdev;			///< char device structure
// END of command device

// BEGIN of device specific data
	OS_SEM_T				mutex;
// END of device specific data
}
GPIO_DEVICE_T;
#endif

#ifdef KDRV_CONFIG_PM
typedef struct
{
	// add here extra parameter
	bool			is_suspended;
	unsigned int	top_control_24;
	unsigned int	top_control_25;
	unsigned char	gpio_direction[TOTAL_GPIO8_NUM];
	unsigned char	gpio_data[TOTAL_GPIO8_NUM];
}GPIO_DRVDATA_T;
#endif

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	GPIO_PROC_Init(void);
extern	void	GPIO_PROC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		GPIO_Init(void);
void	GPIO_Cleanup(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_gpio_debug_fd;
int 	g_gpio_major = GPIO_MAJOR;
int 	g_gpio_minor = GPIO_MINOR;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      GPIO_Open(struct inode *, struct file *);
static int      GPIO_Close(struct inode *, struct file *);
static int 		GPIO_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#ifdef SUPPORT_GPIO_DEVICE_READ_WRITE_FOPS
static ssize_t  GPIO_Read(struct file *, char *, size_t, loff_t *);
static ssize_t  GPIO_Write(struct file *, const char *, size_t, loff_t *);
#endif

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/**
 * main control block for gpio device
*/
static GPIO_DEVICE_T*		g_gpio_device;

/**
 * file I/O description for gpio device
 *
*/
static struct file_operations g_gpio_fops =
{
	.open 	= GPIO_Open,
	.release= GPIO_Close,
	.ioctl	= GPIO_Ioctl,
#ifdef SUPPORT_GPIO_DEVICE_READ_WRITE_FOPS
	.read 	= GPIO_Read,
	.write 	= GPIO_Write,
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
 * @return	int 0 : OK, -1 : NOT OK
 *
 */
static int GPIO_suspend(struct platform_device *pdev, pm_message_t state)
{
	GPIO_DRVDATA_T	*drv_data;
	int				i;

	drv_data = platform_get_drvdata(pdev);

	//Debug_GPIO_Print();

	// add here the suspend code
	CTOP_CTRL_RdFL(ctr24);
	drv_data->top_control_24 = CTOP_CTRL_Rd(ctr24);
	CTOP_CTRL_RdFL(ctr25);
	drv_data->top_control_25 = CTOP_CTRL_Rd(ctr25);

	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		drv_data->gpio_direction[i] = GPIONDIR(i);
	}

	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		drv_data->gpio_data[i] = GPIONDATA(i);
	}

#if 0
	// for test start
	CTOP_CTRL_Wr(ctr24, 0x0);
	CTOP_CTRL_WrFL(ctr24);
	CTOP_CTRL_Wr(ctr25, 0x0);
	CTOP_CTRL_WrFL(ctr25);

	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		GPIONDIR(i) = 0x0;
	}

	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		GPIONDATA(i) = 0x0;
	}
	// for test end
#endif

	drv_data->is_suspended = 1;
	GPIO_PRINT("[%s] done suspend\n", GPIO_MODULE);

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
static int GPIO_resume(struct platform_device *pdev)
{
	GPIO_DRVDATA_T	*drv_data;
	int				i;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;

	//Debug_GPIO_Print();

	// add here the resume code
	CTOP_CTRL_Wr(ctr24, drv_data->top_control_24);
	CTOP_CTRL_WrFL(ctr24);
	CTOP_CTRL_Wr(ctr25, drv_data->top_control_25);
	CTOP_CTRL_WrFL(ctr25);

	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		GPIONDIR(i) = drv_data->gpio_direction[i];
	}

	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		GPIONDATA(i) = drv_data->gpio_data[i];
	}

	drv_data->is_suspended = 0;
	GPIO_PRINT("[%s] done resume\n", GPIO_MODULE);

	//Debug_GPIO_Print();

	return 0;
}
/**
 *
 * probing module.
 *
 * @param	struct platform_device *pdev
 * @return	int 0 : OK, -1 : NOT OK
 *
 */
 int  GPIO_probe(struct platform_device *pdev)
{

	GPIO_DRVDATA_T *drv_data;

	drv_data = (GPIO_DRVDATA_T *)kmalloc(sizeof(GPIO_DRVDATA_T) , GFP_KERNEL);


	// add here driver registering code & allocating resource code



	GPIO_PRINT("[%s] done probe\n", GPIO_MODULE);
	drv_data->is_suspended = 0;
	platform_set_drvdata(pdev, drv_data);

	return 0;
}


/**
 *
 * module remove function, this function will be called in rmmod gpio module
 *
 * @param	struct platform_device
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int  GPIO_remove(struct platform_device *pdev)
{
	GPIO_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code



	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	GPIO_PRINT("released\n");

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
static void  GPIO_release(struct device *dev)
{


	GPIO_PRINT("device released\n");
}

/*
 *	module platform driver structure
 */
static struct platform_driver gpio_driver =
{
	.probe		= GPIO_probe,
	.suspend	= GPIO_suspend,
	.remove		= GPIO_remove,
	.resume		= GPIO_resume,
	.driver		=
	{
		.name	= GPIO_MODULE,
	},
};

static struct platform_device gpio_device = {
	.name = GPIO_MODULE,
	.id = 0,
	.id = -1,
	.dev = {
		.release = GPIO_release,
	},
};
#endif

int GPIO_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for gpio device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_gpio_debug_fd = DBG_OPEN( GPIO_MODULE );
	if(g_gpio_debug_fd < 0) return -1;
#if 0
	OS_DEBUG_EnableModule ( g_gpio_debug_fd );

	OS_DEBUG_EnableModuleByIndex ( g_gpio_debug_fd, 0, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_gpio_debug_fd, 1, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_gpio_debug_fd, 2, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_gpio_debug_fd, 3, DBG_COLOR_NONE );
#endif

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&gpio_driver) < 0)
	{
		GPIO_PRINT("[%s] platform driver register failed\n",GPIO_MODULE);

	}
	else
	{
		if(platform_device_register(&gpio_device))
		{
			platform_driver_unregister(&gpio_driver);
			GPIO_PRINT("[%s] platform device register failed\n",GPIO_MODULE);
		}
		else
		{
			GPIO_PRINT("[%s] platform register done\n", GPIO_MODULE);
		}


	}
#endif

	g_gpio_device = (GPIO_DEVICE_T*)OS_KMalloc( sizeof(GPIO_DEVICE_T)*GPIO_MAX_DEVICE );

	if ( NULL == g_gpio_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(GPIO_DEVICE_T)* GPIO_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_gpio_device, 0x0, sizeof(GPIO_DEVICE_T)* GPIO_MAX_DEVICE );

	if (g_gpio_major)
	{
		dev = MKDEV( g_gpio_major, g_gpio_minor );
		err = register_chrdev_region(dev, GPIO_MAX_DEVICE, GPIO_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_gpio_minor, GPIO_MAX_DEVICE, GPIO_MODULE );
		g_gpio_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register gpio device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */


	/* END */

	for ( i=0; i<GPIO_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_gpio_major, g_gpio_minor+i );
		cdev_init( &(g_gpio_device[i].cdev), &g_gpio_fops );
		g_gpio_device[i].devno		= dev;
		g_gpio_device[i].cdev.owner = THIS_MODULE;
		g_gpio_device[i].cdev.ops   = &g_gpio_fops;

		/* TODO: initialize minor device */


		/* END */

		err = cdev_add (&(g_gpio_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding gpio device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}
        OS_CreateDeviceClass ( g_gpio_device[i].devno, "%s%d", GPIO_MODULE, i );
	}

	/* initialize proc system */
	GPIO_PROC_Init ( );

	GPIO_PRINT("gpio device initialized\n");

	return 0;
}

void GPIO_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_gpio_major, g_gpio_minor );

#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	platform_driver_unregister(&gpio_driver);
	platform_device_unregister(&gpio_device);
#endif

	/* cleanup proc system */
	GPIO_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<GPIO_MAX_DEVICE;i++)
	{
		/* TODO: cleanup each minor device */


		/* END */
		cdev_del( &(g_gpio_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */

	unregister_chrdev_region(dev, GPIO_MAX_DEVICE );

	OS_Free( g_gpio_device );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for gpio device
 *
 */
static int GPIO_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    GPIO_DEVICE_T*	my_dev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, GPIO_DEVICE_T, cdev);

    /* TODO : add your device specific code */


	/* END */

	if(my_dev->dev_open_count == 0)
	{
		GPIO_MUTEX_INIT(my_dev);
	}

    my_dev->dev_open_count++;
    filp->private_data = my_dev;

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    GPIO_PRINT("device opened (%d:%d)\n", major, minor );

    return 0;
}

/**
 * release handler for gpio device
 *
 */
static int GPIO_Close(struct inode *inode, struct file *file)
{
    int					major,minor;
    GPIO_DEVICE_T*	my_dev;
    struct cdev*		cdev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, GPIO_DEVICE_T, cdev);

    if ( my_dev->dev_open_count > 0 )
    {
        --my_dev->dev_open_count;
    }

    /* TODO : add your device specific code */

	/* END */

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    GPIO_PRINT("device closed (%d:%d)\n", major, minor );
    return 0;
}

#if 0
static void Debug_GPIO_Print(void)
{
#ifdef GPIO_DRV_PRINT_ENABLE
	int i;
	unsigned int	top_control = 0;

	printk("data value =");
	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		printk(" 0x%02x", (unsigned char)GPIONDATA(i));
		if(i == 3) printk(" ||");
	}
	printk("\n");

	printk("direction value =");
	for(i = 0; i < TOTAL_GPIO8_NUM; i++)
	{
		printk(" 0x%02x", (unsigned char)GPIONDIR(i));
		if(i == 3) printk(" ||");
	}
	printk("\n");


	printk("top control =");
	printk(" 0xff 0xff 0xff 0xff ||");

	CTOP_CTRL_RdFL(ctr24);
	top_control = CTOP_CTRL_Rd(ctr24);
	printk(" 0x%02x 0x%02x 0x%02x 0x%02x",
		(top_control>>24)&0xFF, (top_control>>16)&0xFF, (top_control>>8)&0xFF, top_control&0xFF);

	CTOP_CTRL_RdFL(ctr25);
	top_control = CTOP_CTRL_Rd(ctr25);
	printk(" 0x%02x 0x%02x 0x%02x 0x%02x",
		(top_control>>24)&0xFF, (top_control>>16)&0xFF, (top_control>>8)&0xFF, top_control&0xFF);
	printk("\n");
#endif

	return;
}
#endif

/**
 * ioctl handler for gpio device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
static int GPIO_Ioctl ( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
	unsigned char	value = 0;
	unsigned char	write_data;
	unsigned char	direction = 0;
	unsigned int	top_control = 0;

	unsigned int	mask_32bit;
	unsigned char	mask_8bit;

    int err = 0, ret = 0;
    struct cdev*		cdev;
    GPIO_DEVICE_T*	my_dev;
	LX_Gpio_Pinset_Param_t  GPIO_param;
	/*
	 * get current gpio device object
	 */
    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, GPIO_DEVICE_T, cdev);

    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != GPIO_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > GPIO_IOC_MAXNR)
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

	GPIO_TRACE("cmd = %08X (cmd_idx=%d)\n", cmd, _IOC_NR(cmd) );

	switch(cmd)
	{
		case LXDRV_GPIO_INIT:
		{
			GPIO_PRINT("ioctl: LXDRV_GPIO_INIT\n");
		}
		break;

		case LXDRV_GPIO_SET:
		{
			if (copy_from_user(&GPIO_param, (LX_Gpio_Pinset_Param_t *) arg, sizeof(LX_Gpio_Pinset_Param_t)))
			{
				return -EFAULT;
			}

			if(GPIO_param.u8Pinnumber < 32 || GPIO_param.u8Pinnumber >= 32*3)
			{
				break;
			}

			GPIO_MUTEX_LOCK(my_dev);

			if(lx_chip_rev() >= LX_CHIP_REV(L9,B0))
			{
				mask_32bit = 0;
				if(GPIO_param.u8Pinnumber >= 32 && GPIO_param.u8Pinnumber <= 39)
				{
					CTOP_CTRL_L9B_RdFL(ctr32_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr32_reg_gpio_mux_enable);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8) + 24);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr32_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr32_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 40 && GPIO_param.u8Pinnumber <= 47)
				{
					CTOP_CTRL_L9B_RdFL(ctr53_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr53_reg_gpio_mux_enable);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8)*4+3);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr53_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr53_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 48 && GPIO_param.u8Pinnumber <= 50)
				{
					CTOP_CTRL_L9B_RdFL(ctr35_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr35_reg_gpio_mux_enable);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8)*4+23);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr35_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr35_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 51 && GPIO_param.u8Pinnumber <= 55)
				{
					CTOP_CTRL_L9B_RdFL(ctr34_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr34_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 51)		mask_32bit=1<<0;
					else if(GPIO_param.u8Pinnumber == 52)	mask_32bit=1<<7;
					else if(GPIO_param.u8Pinnumber == 53)	mask_32bit=1<<15;
					else if(GPIO_param.u8Pinnumber == 54)	mask_32bit=1<<23;
					else if(GPIO_param.u8Pinnumber == 55)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr34_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr34_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 56 && GPIO_param.u8Pinnumber <= 60)
				{
					CTOP_CTRL_L9B_RdFL(ctr37_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr37_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 56)		mask_32bit=1<<15;
					else if(GPIO_param.u8Pinnumber == 57)	mask_32bit=1<<19;
					else if(GPIO_param.u8Pinnumber == 58)	mask_32bit=1<<23;
					else if(GPIO_param.u8Pinnumber == 59)	mask_32bit=1<<27;
					else if(GPIO_param.u8Pinnumber == 60)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr37_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr37_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 61 && GPIO_param.u8Pinnumber <= 63)
				{
					CTOP_CTRL_L9B_RdFL(ctr35_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr35_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 61)		mask_32bit=1<<11;
					else if(GPIO_param.u8Pinnumber == 62)	mask_32bit=1<<15;
					else if(GPIO_param.u8Pinnumber == 63)	mask_32bit=1<<19;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr35_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr35_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber == 64)
				{
					CTOP_CTRL_L9B_RdFL(ctr39_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr39_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 64)		mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr39_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr39_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 65 && GPIO_param.u8Pinnumber <= 68)
				{
					CTOP_CTRL_L9B_RdFL(ctr38_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr38_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 65)		mask_32bit=1<<19;
					else if(GPIO_param.u8Pinnumber == 66)	mask_32bit=1<<23;
					else if(GPIO_param.u8Pinnumber == 67)	mask_32bit=1<<27;
					else if(GPIO_param.u8Pinnumber == 68)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr38_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr38_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 69 && GPIO_param.u8Pinnumber <= 71)
				{
					CTOP_CTRL_L9B_RdFL(ctr37_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr37_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 69)		mask_32bit=1<<3;
					else if(GPIO_param.u8Pinnumber == 70)	mask_32bit=1<<7;
					else if(GPIO_param.u8Pinnumber == 71)	mask_32bit=1<<11;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr37_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr37_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 72 && GPIO_param.u8Pinnumber <= 79)
				{
					CTOP_CTRL_L9B_RdFL(ctr39_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr39_reg_gpio_mux_enable);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8) + 23);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr39_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr39_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 80 && GPIO_param.u8Pinnumber <= 81)
				{
					CTOP_CTRL_L9B_RdFL(ctr51_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr51_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 80)		mask_32bit=1<<27;
					else if(GPIO_param.u8Pinnumber == 81)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr51_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr51_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 82 && GPIO_param.u8Pinnumber <= 87)
				{
					CTOP_CTRL_L9B_RdFL(ctr39_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr39_reg_gpio_mux_enable);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8) + 15);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr39_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr39_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 88 && GPIO_param.u8Pinnumber <= 93)
				{
					CTOP_CTRL_L9B_RdFL(ctr52_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr52_reg_gpio_mux_enable);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8)*4 + 11);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr52_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr52_reg_gpio_mux_enable);
				}
				else if(GPIO_param.u8Pinnumber >= 94 && GPIO_param.u8Pinnumber <= 95)
				{
					CTOP_CTRL_L9B_RdFL(ctr52_reg_gpio_mux_enable);
					top_control = CTOP_CTRL_L9B_Rd(ctr52_reg_gpio_mux_enable);

					if(GPIO_param.u8Pinnumber == 94)		mask_32bit=1<<19;
					else if(GPIO_param.u8Pinnumber == 95)	mask_32bit=1<<23;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9B_Wr(ctr52_reg_gpio_mux_enable, top_control);
					CTOP_CTRL_L9B_WrFL(ctr52_reg_gpio_mux_enable);
				}
			}
			else if(lx_chip_rev() >= LX_CHIP_REV(L9,A0))
			{
				mask_32bit = 0;
				if(GPIO_param.u8Pinnumber >= 32 && GPIO_param.u8Pinnumber <= 39)
				{
					CTOP_CTRL_L9_RdFL(ctr32);
					top_control = CTOP_CTRL_L9_Rd(ctr32);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8) + 24);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr32, top_control);
					CTOP_CTRL_L9_WrFL(ctr32);
				}
				else if(GPIO_param.u8Pinnumber >= 40 && GPIO_param.u8Pinnumber <= 47)
				{
					CTOP_CTRL_L9_RdFL(ctr53);
					top_control = CTOP_CTRL_L9_Rd(ctr53);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8)*4+3);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr53, top_control);
					CTOP_CTRL_L9_WrFL(ctr53);
				}
				else if(GPIO_param.u8Pinnumber >= 48 && GPIO_param.u8Pinnumber <= 50)
				{
					CTOP_CTRL_L9_RdFL(ctr35);
					top_control = CTOP_CTRL_L9_Rd(ctr35);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8)*4+23);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr35, top_control);
					CTOP_CTRL_L9_WrFL(ctr35);
				}
				else if(GPIO_param.u8Pinnumber >= 51 && GPIO_param.u8Pinnumber <= 55)
				{
					CTOP_CTRL_L9_RdFL(ctr34);
					top_control = CTOP_CTRL_L9_Rd(ctr34);

					if(GPIO_param.u8Pinnumber == 51)		mask_32bit=1<<0;
					else if(GPIO_param.u8Pinnumber == 52)	mask_32bit=1<<7;
					else if(GPIO_param.u8Pinnumber == 53)	mask_32bit=1<<15;
					else if(GPIO_param.u8Pinnumber == 54)	mask_32bit=1<<23;
					else if(GPIO_param.u8Pinnumber == 55)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr34, top_control);
					CTOP_CTRL_L9_WrFL(ctr34);

				}
				else if(GPIO_param.u8Pinnumber >= 56 && GPIO_param.u8Pinnumber <= 60)
				{
					CTOP_CTRL_L9_RdFL(ctr37);
					top_control = CTOP_CTRL_L9_Rd(ctr37);

					if(GPIO_param.u8Pinnumber == 56)		mask_32bit=1<<15;
					else if(GPIO_param.u8Pinnumber == 57)	mask_32bit=1<<19;
					else if(GPIO_param.u8Pinnumber == 58)	mask_32bit=1<<23;
					else if(GPIO_param.u8Pinnumber == 59)	mask_32bit=1<<27;
					else if(GPIO_param.u8Pinnumber == 60)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr37, top_control);
					CTOP_CTRL_L9_WrFL(ctr37);

				}
				else if(GPIO_param.u8Pinnumber >= 61 && GPIO_param.u8Pinnumber <= 63)
				{
					CTOP_CTRL_L9_RdFL(ctr35);
					top_control = CTOP_CTRL_L9_Rd(ctr35);

					if(GPIO_param.u8Pinnumber == 61)		mask_32bit=1<<11;
					else if(GPIO_param.u8Pinnumber == 62)	mask_32bit=1<<15;
					else if(GPIO_param.u8Pinnumber == 63)	mask_32bit=1<<19;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr35, top_control);
					CTOP_CTRL_L9_WrFL(ctr35);
				}
				else if(GPIO_param.u8Pinnumber == 64)
				{
					CTOP_CTRL_L9_RdFL(ctr39);
					top_control = CTOP_CTRL_L9_Rd(ctr39);

					if(GPIO_param.u8Pinnumber == 64)		mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr39, top_control);
					CTOP_CTRL_L9_WrFL(ctr39);

				}
				else if(GPIO_param.u8Pinnumber >= 65 && GPIO_param.u8Pinnumber <= 68)
				{
					CTOP_CTRL_L9_RdFL(ctr38);
					top_control = CTOP_CTRL_L9_Rd(ctr38);

					if(GPIO_param.u8Pinnumber == 65)		mask_32bit=1<<19;
					else if(GPIO_param.u8Pinnumber == 66)	mask_32bit=1<<23;
					else if(GPIO_param.u8Pinnumber == 67)	mask_32bit=1<<27;
					else if(GPIO_param.u8Pinnumber == 68)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr38, top_control);
					CTOP_CTRL_L9_WrFL(ctr38);
				}
				else if(GPIO_param.u8Pinnumber >= 69 && GPIO_param.u8Pinnumber <= 71)
				{
					CTOP_CTRL_L9_RdFL(ctr37);
					top_control = CTOP_CTRL_L9_Rd(ctr37);

					if(GPIO_param.u8Pinnumber == 69)		mask_32bit=1<<3;
					else if(GPIO_param.u8Pinnumber == 70)	mask_32bit=1<<7;
					else if(GPIO_param.u8Pinnumber == 71)	mask_32bit=1<<11;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr37, top_control);
					CTOP_CTRL_L9_WrFL(ctr37);

				}
				else if(GPIO_param.u8Pinnumber >= 72 && GPIO_param.u8Pinnumber <= 79)
				{
					CTOP_CTRL_L9_RdFL(ctr39);
					top_control = CTOP_CTRL_L9_Rd(ctr39);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8) + 23);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr39, top_control);
					CTOP_CTRL_L9_WrFL(ctr39);

				}
				else if(GPIO_param.u8Pinnumber >= 80 && GPIO_param.u8Pinnumber <= 81)
				{
					CTOP_CTRL_L9_RdFL(ctr51);
					top_control = CTOP_CTRL_L9_Rd(ctr51);

					if(GPIO_param.u8Pinnumber == 80)		mask_32bit=1<<27;
					else if(GPIO_param.u8Pinnumber == 81)	mask_32bit=1<<31;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr51, top_control);
					CTOP_CTRL_L9_WrFL(ctr51);

				}
				else if(GPIO_param.u8Pinnumber >= 82 && GPIO_param.u8Pinnumber <= 87)
				{
					CTOP_CTRL_L9_RdFL(ctr39);
					top_control = CTOP_CTRL_L9_Rd(ctr39);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8) + 15);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr39, top_control);
					CTOP_CTRL_L9_WrFL(ctr39);

				}
				else if(GPIO_param.u8Pinnumber >= 88 && GPIO_param.u8Pinnumber <= 93)
				{
					CTOP_CTRL_L9_RdFL(ctr52);
					top_control = CTOP_CTRL_L9_Rd(ctr52);

					mask_32bit = 1 << ((GPIO_param.u8Pinnumber % 8)*4 + 11);

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr52, top_control);
					CTOP_CTRL_L9_WrFL(ctr52);

				}
				else if(GPIO_param.u8Pinnumber >= 94 && GPIO_param.u8Pinnumber <= 95)
				{
					CTOP_CTRL_L9_RdFL(ctr52);
					top_control = CTOP_CTRL_L9_Rd(ctr52);

					if(GPIO_param.u8Pinnumber == 94)		mask_32bit=1<<19;
					else if(GPIO_param.u8Pinnumber == 95)	mask_32bit=1<<23;

					top_control = (top_control | mask_32bit);

					CTOP_CTRL_L9_Wr(ctr52, top_control);
					CTOP_CTRL_L9_WrFL(ctr52);

				}

			}
			else
			{
				if(GPIO_param.u8Pinnumber < 32 * 2)
				{
					CTOP_CTRL_RdFL(ctr24);
					top_control = CTOP_CTRL_Rd(ctr24);
				}
				else if(GPIO_param.u8Pinnumber < 32 * 3)
				{
					CTOP_CTRL_RdFL(ctr25);
					top_control = CTOP_CTRL_Rd(ctr25);
				}

				if(lx_chip_rev() < LX_CHIP_REV(L8,B0))
				{
					// for work-around : Top control register connection error - start
					if(GPIO_param.u8Pinnumber == 81) { mask_32bit = 0x4000; }
					else if(GPIO_param.u8Pinnumber == 82) { mask_32bit = 0x400; }
					else if(GPIO_param.u8Pinnumber == 83) { mask_32bit = 0x1000; }
					else if(GPIO_param.u8Pinnumber == 84) { mask_32bit = 0x2000; }
					else if(GPIO_param.u8Pinnumber == 85) { mask_32bit = 0x800; }
					// for work-around : Top control register connection error - end
					else { mask_32bit = 1 << ( 31 - (GPIO_param.u8Pinnumber % 32)); }
				}
				else
				{
					mask_32bit = 1 << ( 31 - (GPIO_param.u8Pinnumber % 32));
				}

				top_control = (top_control | mask_32bit);

				if(GPIO_param.u8Pinnumber < 32 * 2)
				{
					CTOP_CTRL_Wr(ctr24, top_control);
					CTOP_CTRL_WrFL(ctr24);
				}
				else if(GPIO_param.u8Pinnumber < 32 * 3)
				{
					CTOP_CTRL_Wr(ctr25, top_control);
					CTOP_CTRL_WrFL(ctr25);
				}
			}

			GPIO_MUTEX_UNLOCK(my_dev);

//			Debug_GPIO_Print();
		}
		break;

		case LXDRV_GPIO_INOUT:
		{
			if (copy_from_user(&GPIO_param, (LX_Gpio_Pinset_Param_t *) arg, sizeof(LX_Gpio_Pinset_Param_t)))
			{
				return -EFAULT;
			}

			if(GPIO_param.u8Pinnumber >= 32 * 3)
				return -EIO;

			GPIO_MUTEX_LOCK(my_dev);

			direction = GPIONDIR(GPIO_param.u8Pinnumber/8);

			if(lx_chip_rev() < LX_CHIP_REV(L8,B0))
			{
				if(GPIO_param.u8Pinnumber < 32)			{ mask_8bit = 1 << (GPIO_param.u8Pinnumber % 8); }
				// for work-around : Top control register connection error - start
				else if(GPIO_param.u8Pinnumber == 81)	{ mask_8bit = 0x40; }
				else if(GPIO_param.u8Pinnumber == 82)	{ mask_8bit = 0x4; }
				else if(GPIO_param.u8Pinnumber == 83)	{ mask_8bit = 0x10; }
				else if(GPIO_param.u8Pinnumber == 84)	{ mask_8bit = 0x20; }
				else if(GPIO_param.u8Pinnumber == 85)	{ mask_8bit = 0x8; }
				// for work-around : Top control register connection error - end
				else									{ mask_8bit = 1 << (7 - (GPIO_param.u8Pinnumber % 8)); }
			}
			else if(lx_chip_rev() >= LX_CHIP_REV(L9,A0))
			{
				mask_8bit = 1 << (GPIO_param.u8Pinnumber % 8);
			}
			else
			{
				if(GPIO_param.u8Pinnumber < 32) { mask_8bit = 1 << (GPIO_param.u8Pinnumber % 8); }
				else							{ mask_8bit = 1 << (7 - (GPIO_param.u8Pinnumber % 8)); }

			}
			direction = (GPIO_param.u8Flag == 1) ? (direction | mask_8bit)
								: (direction & (~mask_8bit));

			GPIONDIR(GPIO_param.u8Pinnumber/8) = direction;

			GPIO_MUTEX_UNLOCK(my_dev);

//			Debug_GPIO_Print();
		}
		break;

		case LXDRV_GPIO_READ:
		{
			if(copy_from_user(&GPIO_param, (LX_Gpio_Pinset_Param_t *) arg, sizeof(LX_Gpio_Pinset_Param_t)))
			{
				return -EFAULT;
			}

			if(GPIO_param.u8Pinnumber >= 32 * 3)
				return -EIO;


			GPIO_MUTEX_LOCK(my_dev);

			value = GPIONDATA(GPIO_param.u8Pinnumber/8);

			if(lx_chip_rev() < LX_CHIP_REV(L8,B0))
			{
				if(GPIO_param.u8Pinnumber < 32)			{ value = (value >> (GPIO_param.u8Pinnumber % 8)) & 0x1; }
				// for work-around : Top control register connection error - start
				else if(GPIO_param.u8Pinnumber == 81)	{ value = (value >> 6) & 0x1; } // 0x40
				else if(GPIO_param.u8Pinnumber == 82)	{ value = (value >> 2) & 0x1; } // 0x4
				else if(GPIO_param.u8Pinnumber == 83)	{ value = (value >> 4) & 0x1; } // 0x10
				else if(GPIO_param.u8Pinnumber == 84)	{ value = (value >> 5) & 0x1; } // 0x20
				else if(GPIO_param.u8Pinnumber == 85)	{ value = (value >> 3) & 0x1; } // 0x8
				// for work-around : Top control register connection error - end
				else									{ value = (value >> (7 - (GPIO_param.u8Pinnumber % 8))) & 0x1; }
			}
			else if(lx_chip_rev() >= LX_CHIP_REV(L9,A0))
			{
				value = (value >> (GPIO_param.u8Pinnumber % 8)) & 0x1;
			}
			else
			{
				if(GPIO_param.u8Pinnumber < 32) { value = (value >> (GPIO_param.u8Pinnumber % 8)) & 0x1; }
				else							{ value = (value >> (7 - (GPIO_param.u8Pinnumber % 8))) & 0x1; }
			}

			GPIO_param.u8Flag = value;

			GPIO_MUTEX_UNLOCK(my_dev);

			if(copy_to_user((LX_Gpio_Pinset_Param_t *) arg, &GPIO_param, sizeof(LX_Gpio_Pinset_Param_t)))
			{
				return -EFAULT;
			}

//			Debug_GPIO_Print();
		}
		break;

		case LXDRV_GPIO_PULL_HIGHLOW:
		{
			if (copy_from_user(&GPIO_param, (LX_Gpio_Pinset_Param_t *) arg, sizeof(LX_Gpio_Pinset_Param_t)))
			{
				return -EFAULT;
			}

			if(GPIO_param.u8Pinnumber >= 32 * 3)
				return -EIO;

			GPIO_MUTEX_LOCK(my_dev);

			if(lx_chip_rev() < LX_CHIP_REV(L8,B0))
			{
				if(GPIO_param.u8Pinnumber < 32)				{ mask_32bit = 1 << (2 + (GPIO_param.u8Pinnumber % 8)); }
				// for work-around : Top control register connection error - start
				else if(GPIO_param.u8Pinnumber == 81)		{ mask_32bit = 0x40 << 2; }
				else if(GPIO_param.u8Pinnumber == 82)		{ mask_32bit = 0x4  << 2; }
				else if(GPIO_param.u8Pinnumber == 83)		{ mask_32bit = 0x10 << 2; }
				else if(GPIO_param.u8Pinnumber == 84)		{ mask_32bit = 0x20 << 2; }
				else if(GPIO_param.u8Pinnumber == 85)		{ mask_32bit = 0x8  << 2; }
				// for work-around : Top control register connection error - end
				else 										{ mask_32bit = 1 << (2 + (7-(GPIO_param.u8Pinnumber % 8))); }
			}
			else if(lx_chip_rev() >= LX_CHIP_REV(L9,A0))
			{
				mask_32bit = 1 << (2 + (GPIO_param.u8Pinnumber % 8));
			}
			else
			{
				if(GPIO_param.u8Pinnumber < 32)	{ mask_32bit = 1 << (2 + (GPIO_param.u8Pinnumber % 8)); }
				else							{ mask_32bit = 1 << (2 + (7-(GPIO_param.u8Pinnumber % 8))); }
			}

			write_data = (GPIO_param.u8Flag == 1) ? 0xff : 0x0;

			*((volatile unsigned long *)(GPION_Base(GPIO_param.u8Pinnumber/8) + mask_32bit)) = write_data;

			GPIO_MUTEX_UNLOCK(my_dev);

//			Debug_GPIO_Print();
		}
		break;

	    default:
	    {
	    	GPIO_PRINT("ioctl: default\n");
			/* redundant check but it seems more readable */
    	    ret = -ENOTTY;
		}
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",GPIO_Init);
#else
module_init(GPIO_Init);
#endif
module_exit(GPIO_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

