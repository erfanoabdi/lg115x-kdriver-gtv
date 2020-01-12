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
 *  main driver implementation for ttx device.
 *	ttx device will teach you how to make device driver with new platform.
 *
 *  author		wonsik.do (wonsik.do@lge.com)
 *  version		1.0
 *  date		2010.01.06
 *  note		Additional information.
 *
 *  @addtogroup lg1150_ttx
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_TTX_DEVICE_READ_WRITE_FOPS

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <asm/io.h>			/**< For ioremap_nocache */
#include <linux/dma-mapping.h>
#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif
#include "os_util.h"
#include "base_dev_cfg.h"

#include "ttx_module.h"
#include "ttx_hw.h"
#include "ttx_drv.h"
#include "ttx_data.h"


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
 *	main control block for ttx device.
 *	each minor device has unique control block
 *
 */
typedef struct
{
// BEGIN of common device
	int						dev_open_count;		///< check if device is opened or not
	dev_t					devno;			///< device number
	struct cdev				cdev;			///< char device structure
	int						dev_initialized;	///< check if device is initialized or not
// END of command device

// BEGIN of device specific data


// END of device specific data
}
TTX_DEVICE_T;

#ifdef KDRV_CONFIG_PM
typedef struct
{
	// add here extra parameter
	bool	is_suspended;
}TTX_DRVDATA_T;
#endif

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	TTX_PROC_Init(void);
extern	void	TTX_PROC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		TTX_Init(void);
void	TTX_Cleanup(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_ttx_debug_fd;
int 	g_ttx_major = TTX_MAJOR;
int 	g_ttx_minor = TTX_MINOR;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      TTX_Open(struct inode *, struct file *);
static int      TTX_Close(struct inode *, struct file *);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int 		TTX_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#else
static long     TTX_Ioctl (struct file * filp, unsigned int cmd, unsigned long arg);
#endif
static int TTX_MMap(struct file *, struct vm_area_struct *);
#ifdef SUPPORT_TTX_DEVICE_READ_WRITE_FOPS
static ssize_t  TTX_Read(struct file *, char *, size_t, loff_t *);
static ssize_t  TTX_Write(struct file *, const char *, size_t, loff_t *);
#endif

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/**
 * main control block for ttx device
*/
static TTX_DEVICE_T*		g_ttx_device;

/**
 * file I/O description for ttx device
 *
*/
static struct file_operations g_ttx_fops =
{
	.open 	= TTX_Open,
	.release= TTX_Close,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl	= TTX_Ioctl,
#else
	.unlocked_ioctl	= TTX_Ioctl,
#endif
	.mmap	= TTX_MMap,
#ifdef SUPPORT_TTX_DEVICE_READ_WRITE_FOPS
	.read 	= TTX_Read,
	.write 	= TTX_Write,
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
static int TTX_suspend(struct platform_device *pdev, pm_message_t state)
{
	TTX_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	// add here the suspend code
#if 1
printk("%s %d\n" , __func__ , __LINE__);

	TTX_CC_RunSuspend(); // won.hur
//	TTX_RunSuspend();

	drv_data->is_suspended = 1;
	TTX_PRINT("[%s] done suspend\n", TTX_MODULE);
#endif
	// for testing
//	TTX_SetInputFormat(0);
//	TTX_ClearAllMemory();
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
static int TTX_resume(struct platform_device *pdev)
{
	TTX_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;


	// add here the resume code
	//	TTX_RunResume();

	TTX_CC_RunResume();

	drv_data->is_suspended = 0;
	TTX_PRINT("[%s] done resume\n", TTX_MODULE);
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
 int  TTX_probe(struct platform_device *pdev)
{

	TTX_DRVDATA_T *drv_data;

	drv_data = (TTX_DRVDATA_T *)kmalloc(sizeof(TTX_DRVDATA_T) , GFP_KERNEL);


	// add here driver registering code & allocating resource code



	TTX_PRINT("[%s] done probe\n", TTX_MODULE);
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
static int  TTX_remove(struct platform_device *pdev)
{
	TTX_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code



	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	TTX_PRINT("released\n");

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
static void  TTX_release(struct device *dev)
{


	TTX_PRINT("device released\n");
}

/*
 *	module platform driver structure
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
static struct platform_driver ttx_driver =
#else
static struct platform_driver ttx_driver __refdata =
#endif
{
	.probe          = TTX_probe,
	.suspend        = TTX_suspend,
	.remove         = TTX_remove,
	.resume         = TTX_resume,
	.driver         =
	{
		.name   = TTX_MODULE,
	},
};

static struct platform_device ttx_device = {
	.name = TTX_MODULE,
	.id = 0,
	.id = -1,
	.dev = {
		.release = TTX_release,
	},
};
#endif

int TTX_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	TTX_PRINT("%s entered \n" , __func__);
	/* Get the handle of debug output for ttx device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_ttx_debug_fd = DBG_OPEN( TTX_MODULE );
	if( g_ttx_debug_fd < 0) return -1;
	OS_DEBUG_EnableModule ( g_ttx_debug_fd );

	OS_DEBUG_EnableModuleByIndex ( g_ttx_debug_fd, 0, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_ttx_debug_fd, 1, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_ttx_debug_fd, 2, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_ttx_debug_fd, 3, DBG_COLOR_NONE );

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&ttx_driver) < 0)
	{
		TTX_PRINT("[%s] platform driver register failed\n",TTX_MODULE);

	}
	else
	{
		if(platform_device_register(&ttx_device))
		{
			platform_driver_unregister(&ttx_driver);
			TTX_PRINT("[%s] platform device register failed\n",TTX_MODULE);
		}
		else
		{
			TTX_PRINT("[%s] platform register done\n", TTX_MODULE);
		}


	}
#endif

	g_ttx_device = (TTX_DEVICE_T*)OS_KMalloc( sizeof(TTX_DEVICE_T)*TTX_MAX_DEVICE );

	if ( NULL == g_ttx_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(TTX_DEVICE_T)* TTX_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_ttx_device, 0x0, sizeof(TTX_DEVICE_T)* TTX_MAX_DEVICE );

	if (g_ttx_major)
	{
		dev = MKDEV( g_ttx_major, g_ttx_minor );
		err = register_chrdev_region(dev, TTX_MAX_DEVICE, TTX_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_ttx_minor, TTX_MAX_DEVICE, TTX_MODULE );
		g_ttx_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register ttx device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */


	/* END */

	for ( i=0; i<TTX_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_ttx_major, g_ttx_minor+i );
		cdev_init( &(g_ttx_device[i].cdev), &g_ttx_fops );
		g_ttx_device[i].devno		= dev;
		g_ttx_device[i].cdev.owner = THIS_MODULE;
		g_ttx_device[i].cdev.ops   = &g_ttx_fops;

		/* TODO: initialize minor device */


		/* END */

		err = cdev_add (&(g_ttx_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding ttx device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}
        OS_CreateDeviceClass ( g_ttx_device[i].devno, "%s%d", TTX_MODULE, i );
	}

	/* initialize proc system */
	TTX_PROC_Init ( );

	TTX_PRINT("ttx device initialized\n");

	//dws
	//TTX_Initialize();

	return 0;
}

void TTX_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_ttx_major, g_ttx_minor );

#ifdef KDRV_CONFIG_PM

	platform_driver_unregister(&ttx_driver);
	platform_device_unregister(&ttx_device);
#endif

	/* cleanup proc system */
	TTX_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<TTX_MAX_DEVICE;i++)
	{
		/* TODO: cleanup each minor device */


		/* END */
		cdev_del( &(g_ttx_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */

	unregister_chrdev_region(dev, TTX_MAX_DEVICE );

	OS_Free( g_ttx_device );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for ttx device
 *
 */
static int
TTX_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    TTX_DEVICE_T*	my_dev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, TTX_DEVICE_T, cdev);

	TTX_PRINT("%s entered \n" , __func__);

    /* TODO : add your device specific code */


	/* END */


    my_dev->dev_open_count++;
    filp->private_data = my_dev;

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    TTX_PRINT("device opened (%d:%d)\n", major, minor );

    return 0;
}

/**
 * release handler for ttx device
 *
 */
static int
TTX_Close(struct inode *inode, struct file *file)
{
    int					major,minor;
    TTX_DEVICE_T*	my_dev;
    struct cdev*		cdev;

	TTX_PRINT("%s entered \n" , __func__);
    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, TTX_DEVICE_T, cdev);

    if ( my_dev->dev_open_count > 0 )
    {
        --my_dev->dev_open_count;
    }

    /* TODO : add your device specific code */

	/* END */

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    TTX_PRINT("device closed (%d:%d)\n", major, minor );
    return 0;
}

/**
 * memory mapping to virtual region
 *
 */
static int TTX_MMap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long start;
	unsigned long len;
	unsigned long off = ((vma->vm_pgoff) << PAGE_SHIFT);

	TTX_PRINT("%s entered \n" , __func__);

	start = gstMemCfgTTX->buf_memory_base & PAGE_MASK; // or 0
	len = PAGE_ALIGN((start & ~PAGE_MASK) + gstMemCfgTTX->buf_memory_size);

	TTX_PRINT("MMAP : start - %08lx , len - %08lx , off - %08lx " , start , len , off);

	if ((vma->vm_end - vma->vm_start + off) > len)
	{
	 	TTX_PRINT("ERROR : GFX_SurfaceMmap leng is over\n");
	 	TTX_PRINT("start : %lx end : %lx offset : %lx len : %lx\n" ,
	 		vma->vm_start , vma->vm_end , off , len);

        return -EINVAL;
	}

	off += start;

	TTX_PRINT("MMAP : start - %08lx , len - %08lx , off - %08lx " , start , len , vma->vm_pgoff);

	vma->vm_pgoff = off >> PAGE_SHIFT;

	vma->vm_flags |= VM_IO;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	return io_remap_pfn_range(vma, vma->vm_start, off >> PAGE_SHIFT,
	                           vma->vm_end - vma->vm_start,
	                           vma->vm_page_prot);
	return 0;
}

/**
 * ioctl handler for ttx device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int
TTX_Ioctl ( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
#else
static long
TTX_Ioctl (struct file * filp, unsigned int cmd, unsigned long arg)
#endif
{
    int err = 0, ret = 0;

    TTX_DEVICE_T*	my_dev;
    struct cdev*		cdev;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
#else
	struct inode *inode = filp->f_path.dentry->d_inode;
#endif
    unsigned int temp;

	//TTX_PRINT("%s entered \n" , __func__);
	/*
	 * get current ttx device object
	 */
    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, TTX_DEVICE_T, cdev);

    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != TTX_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > TTX_IOC_MAXNR)
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

	//TTX_TRACE("cmd = %08X (cmd_idx=%d)\n", cmd, _IOC_NR(cmd) );

	switch(cmd)
	{
#if 0
		case TTX_IO_RESET:
		{
			TTX_PRINT("reset ok\n");
			ret = 0;
		}
		break;

		case TTX_IOR_CHIP_REV_INFO:
		{
			CHIP_REV_INFO_T	rev_info;

			rev_info.version = 0x100;
			rev_info.date[0] =  9;	/* 2009/11/24 */
			rev_info.date[0] = 11;
			rev_info.date[0] = 24;

			TTX_PRINT("rev_info (%0X, %d:%d:%d)\n", rev_info.version,
														rev_info.date[0], rev_info.date[1], rev_info.date[2] );

	        if ( copy_to_user((void __user *)arg, &rev_info, sizeof(CHIP_REV_INFO_T)) )
    	        return -EFAULT;

			ret = 0;
		}
		break;

		case TTX_IOW_WRITE_UINT32:
		{
			UINT32	data;

			ret = __get_user( data, (UINT32 __user *)arg );

			TTX_PRINT("data = 0x%X\n", data );

			ret = 0;
		}
		break;
#endif
		case TTX_IO_INIT:
			TTX_PRINT("TTX init\n");
			if(my_dev->dev_initialized == 0)
			{
				if(TTX_Initialize() < 0)
				{
					ret = -1;
				}
				else
				{
					ret = 0;
					my_dev->dev_initialized = 1;
				}
			}
			break;

		case TTX_IOW_ONOFF_INTERRUPT:
			if((BOOLEAN)arg)
				ret = TTX_SetInterrupt(1);
			else
				ret = TTX_SetInterrupt(0);
			break;

		case TTX_IOW_ONOFF_SLICER:
			if((BOOLEAN)arg)
				TTX_PRINT("TTX Slicer ON\n");
			else
				TTX_PRINT("TTX Interrupt OFF\n");
			ret=0;
			break;

		case TTX_IO_RESET_TTX_BUFFER:
	//		TTX_PRINT("reset TTX buffer\n");

			ret = TTX_ClearTTXMemory();

			break;

		case TTX_IO_RESET_VPS_BUFFER:
	//		TTX_PRINT("reset VPS buffer\n");

			ret = TTX_ClearVPSMemory();

			break;

		case TTX_IO_RESET_WSS_BUFFER:
	//		TTX_PRINT("reset WSS buffer\n");

			ret = TTX_ClearWSSMemory();

			break;

		case TTX_IOR_GET_PARAMETER_OFFSET:
			TTX_PRINT("TTX Get Parameter offset\n");
			ret=0;
			break;

		case TTX_IOR_GET_MMAP_SIZE:
			TTX_PRINT("TTX Get Memory Map Size\n");

			temp = (unsigned int)TTX_GetMemorySize();
			ret = copy_to_user((void *)arg, (void *)&temp, sizeof(unsigned int));
			//memcpy((void *)arg , (void *)&temp , sizeof(int) );
			//TTX_GetMemorySize();

			//ret=0;
			break;

		case TTX_IO_RESET_TTX:
	//		TTX_PRINT("TTX Reset Parameters\n");
			ret=0;
			break;

		case TTX_IO_RELEASE:
			TTX_PRINT("TTX Release\n");
			ret=0;
			break;

		case TTX_IOW_ENABLE_VPS_SLICER:
			{
				BOOLEAN bEnable;
				TTX_PRINT("ENABLE_VPS_SLICER\n");

				if( copy_from_user((void *)&bEnable, (void *)arg, sizeof(BOOLEAN)) )
					return -EFAULT;

				if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
					ret = TTX_Enable_VPS_Slicer_L9Bx(bEnable);
			}
			break;

		case TTX_CC_IO_RESET:
		{
			CC_PRINT("reset ok\n");
			ret = 0;
		}
		break;

		case TTX_CC_IO_INIT:
			CC_PRINT("CC init\n");

			if(my_dev->dev_initialized == 0)
			{
				ret = TTX_CC_Initialize();
				my_dev->dev_initialized = 1;
			}
			break;

		case TTX_CC_IOW_ONOFF_INTERRUPT:
			if((BOOLEAN)arg)
				CC_PRINT("CC interrupt ON\n");
			else
				CC_PRINT("CC Interrupt OFF\n");
			ret = TTX_CC_InterruptControl(arg);
			break;

		case TTX_CC_IOW_ONOFF_SLICER:
			if((BOOLEAN)arg)
				CC_PRINT("CC Slicer ON\n");
			else
				CC_PRINT("CC Slicer OFF\n");

			ret=0;
			break;

		case TTX_CC_IOR_GET_PACKET_COUNT:
			//CC_PRINT("CC Get packet count\n");

			temp = TTX_CC_GetVBIBufferInform();

			//memcpy((void *)arg , (void *)&temp , sizeof(int));
			ret = copy_to_user((void *)arg , (void *)&temp , sizeof(int));

			break;

		case TTX_CC_IO_RESET_CC_BUFFER:
			CC_PRINT("CC Reset CC Buffer \n");
			ret = TTX_CC_SetVBIBufferClear();
			break;

		case TTX_CC_IOR_GET_CC_PACKET:
		//	CC_PRINT("CC Get CC Packet \n");

			ret = TTX_CC_GetVBIData((void *)&temp , 1);
			//memcpy((void *)arg , (void *)&temp , sizeof(int));
			ret |= copy_to_user((void *)arg , (void *)&temp , sizeof(int));

			break;

		case TTX_IOW_VBI_SET_OPTION_LPF:
			ret = TTX_SetVBILPFOption(arg);
			break;

		case TTX_IOW_SET_TYPE_LINE318:
			ret = TTX_SetTypeLine318(arg);
			break;

		case TTX_IOW_CHECK_LINE318:
			ret = TTX_CheckOnLine318(arg);
			break;

		case TTX_CC_IOW_SET_DEBUGMODE_CC_PRINT:
			if(arg == 2) ret = TTX_CC_SetPrintResultOfLineDetect(1);
			else if(arg == 1) ret = TTX_CC_DebugModeOnOffCCPrint(1);
			else
			{
				ret = TTX_CC_SetPrintResultOfLineDetect(0);
				ret = TTX_CC_DebugModeOnOffCCPrint(0);
			}
			break;

#if 0
		case TTX_IOR_GET_PACKET_COUNT:
			TTX_PRINT("get TTX packet count\n");
			ret=0;
			break;
#endif
		case TTX_IOW_SET_TT_SLICER_MODE:
			{
				unsigned int slicer_mode;

				if( copy_from_user((void *)&slicer_mode, (void *)arg, sizeof(unsigned int)) )
					return -EFAULT;

				if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
					ret = TTX_Set_TT_Slicer_Mode_L9Bx(slicer_mode);
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
user_initcall_grp("kdrv",TTX_Init);
#else
module_init(TTX_Init);
#endif
module_exit(TTX_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

