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


/** @file pe_drv.c
 *
 *  main driver implementation for pe device.
 *	
 *	@author		
 *	@version	0.1
 *	@note		
 *	@date		2011.06.11
 *	@see		
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
//#include "pe_qkb.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#ifdef	KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif

#include "os_util.h"
#include "base_dev_cfg.h"
#include "debug_util.h"

#include "pe_cfg.h"
#include "pe_drv.h"
#include "pe_io.h"
#include "pe_spd.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define PE_MAJOR	PQL_MAJOR
#define PE_MINOR	PQL_MINOR

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
/**
 *	main control block for pe device.
 *	each minor device has unique control block
 *
 */
typedef struct
{
	int 					dev_open_count; 	///< check if device is opened or not
	dev_t					devno;			///< device number
	struct cdev 			cdev;			///< char device structure
}
PE_DEVICE_T;

typedef struct
{
	// add here extra parameter
	bool	is_suspended;
}PE_DRVDATA_T;

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int 	g_pe_debug_fd;
int 	g_pe_major = PE_MAJOR;
int 	g_pe_minor = PE_MINOR;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int		PE_Open(struct inode *, struct file *);
static int		PE_Close(struct inode *, struct file *);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int		PE_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#else
static long		PE_Ioctl (struct file *, unsigned int, unsigned long );
#endif
static int		PE_DRV_ProcessPkt( unsigned long arg );
/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
/**
 * main control block for pe device
*/
static PE_DEVICE_T*		g_pe_device;

/**
 * file I/O description for pe device
 *
*/
static struct file_operations g_pe_fops =
{
	.open	= PE_Open,
	.release= PE_Close,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl	= PE_Ioctl,
#else
	.unlocked_ioctl = PE_Ioctl,
#endif
	.read	= NULL,
	.write	= NULL,
};

/*========================================================================================
	Implementation Group
========================================================================================*/

#ifdef	KDRV_CONFIG_PM
/**
 *
 * suspending module.
 *
 * @param	struct platform_device *pdev pm_message_t state
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int PE_suspend(struct platform_device *pdev, pm_message_t state)
{
	PE_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	drv_data->is_suspended = 1;

	PE_SPD_suspend();
	
	PE_DRV_DBG_PRINT("[%s] done suspend\n", PE_MODULE);

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
static int PE_resume(struct platform_device *pdev)
{
	PE_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;

	drv_data->is_suspended = 0;

	PE_SPD_resume();

	PE_DRV_DBG_PRINT("[%s] done resume\n", PE_MODULE);
	
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
 int  PE_probe(struct platform_device *pdev)
{

	PE_DRVDATA_T *drv_data;

	drv_data = (PE_DRVDATA_T *)kmalloc(sizeof(PE_DRVDATA_T) , GFP_KERNEL);

	PE_DRV_DBG_PRINT("[%s] done probe\n", PE_MODULE);

	drv_data->is_suspended = 0;
	
	PE_SPD_probe();
	
	platform_set_drvdata(pdev, drv_data);

	return 0;
}


/**
 *
 * module remove function. this function will be called in rmmod pe module.
 *
 * @param	struct platform_device
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static int	PE_remove(struct platform_device *pdev)
{
	PE_DRVDATA_T *drv_data;

	PE_SPD_remove();

	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	PE_DRV_DBG_PRINT("[%s] removed\n", PE_MODULE);

	return 0;
}

/**
 *
 * module release function. this function will be called in rmmod pe module.
 *
 * @param	struct device *dev
 * @return	int 0 : OK , -1 : NOT OK
 *
 */
static void PE_release(struct device *dev)
{
	PE_SPD_release();
	
	PE_DRV_DBG_PRINT("[%s] device released\n", PE_MODULE);
}

/*
 *	module platform driver structure
 */
static struct platform_driver pe_driver =
{
	.probe          = PE_probe,
	.suspend        = PE_suspend,
	.remove         = PE_remove,
	.resume         = PE_resume,
	.driver         =
	{
		.name   = PE_MODULE,
	},
};

static struct platform_device pe_device = {
	.name = PE_MODULE,
	.id = 0,
	.dev =
	{
		.release = PE_release,
	},
};
#endif

/**
 * open handler for pe device
 *
 * @param   *inode [in] inode
 * @param   *filp [in] file
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int
PE_Open(struct inode *inode, struct file *filp)
{
	int 				major,minor;
	struct cdev*		cdev;
	PE_DEVICE_T*	my_dev;

	cdev	= inode->i_cdev;
	my_dev	= container_of ( cdev, PE_DEVICE_T, cdev);

	my_dev->dev_open_count++;
	filp->private_data = my_dev;

	/* some debug */
	major = imajor(inode);
	minor = iminor(inode);
	
	PE_DRV_DBG_PRINT("device opened (%d:%d)\n", major, minor );

	return 0;
}

/**
 * pe close
 *
 * @param   *inode [in] inode
 * @param   *file [in] file
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
static int
PE_Close(struct inode *inode, struct file *file)
{
	int 				major,minor;
	PE_DEVICE_T*	my_dev;
	struct cdev*		cdev;

	cdev	= inode->i_cdev;
	my_dev	= container_of ( cdev, PE_DEVICE_T, cdev);

	if ( my_dev->dev_open_count > 0 )
	{
		--my_dev->dev_open_count;
	}

	major = imajor(inode);
	minor = iminor(inode);

	PE_DRV_DBG_PRINT("device closed (%d:%d)\n", major, minor );
	return 0;
}

/**
 * ioctl handler for pe device.
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 * @note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int
PE_Ioctl ( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
#else
static long
PE_Ioctl ( struct file *filp, unsigned int cmd, unsigned long arg )
#endif
{
    int err = 0, ret = 0;

    PE_DEVICE_T*	my_dev;
    struct cdev*		cdev;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
#else
		struct inode *inode = filp->f_path.dentry->d_inode;
#endif

	/*
	 * get current pe device object
	 */
    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, PE_DEVICE_T, cdev);

    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != PE_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > PE_IOC_MAXNR)
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

	PE_DRV_DBG_PRINT("cmd = %08X (cmd_idx=%d)\n",cmd, _IOC_NR(cmd) );

	switch(cmd)
	{
		case PE_IOWR_PKT:
		{
#ifndef USE_VIDEO_IOCTL_CALLING
			PE_DRV_DBG_PRINT("PE_IOWR_PKT IOCTL\n");
			ret = 0;
#else
			ret = PE_DRV_ProcessPkt(arg);
#endif
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

/**
 * init pe drv
 *
 * @param   void
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author
 */
int PE_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for pe device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_pe_debug_fd = DBG_OPEN( PE_MODULE );
	if (g_pe_debug_fd < 0) {
		DBG_PRINT_ERROR("can't open pe module\n" );
		return -EIO;
	}
	OS_DEBUG_EnableModule ( g_pe_debug_fd );

	OS_DEBUG_EnableModuleByIndex ( g_pe_debug_fd, 0, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_pe_debug_fd, 1, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_pe_debug_fd, 2, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_pe_debug_fd, 3, DBG_COLOR_NONE );

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
#ifdef	KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&pe_driver) < 0)
	{
		PE_DRV_DBG_PRINT("[%s] platform driver register failed\n",PE_MODULE);

	}
	else
	{
		if(platform_device_register(&pe_device))
		{
			platform_driver_unregister(&pe_driver);
			PE_DRV_DBG_PRINT("[%s] platform device register failed\n",PE_MODULE);
		}
		else
		{
			PE_DRV_DBG_PRINT("[%s] platform register done\n", PE_MODULE);
		}


	}
#endif

	g_pe_device = (PE_DEVICE_T*)OS_KMalloc( sizeof(PE_DEVICE_T)*PE_MAX_DEVICE );

	if ( NULL == g_pe_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(PE_DEVICE_T)* PE_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_pe_device, 0x0, sizeof(PE_DEVICE_T)* PE_MAX_DEVICE );

	if (g_pe_major)
	{
		dev = MKDEV( g_pe_major, g_pe_minor );
		err = register_chrdev_region(dev, PE_MAX_DEVICE, PE_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_pe_minor, PE_MAX_DEVICE, PE_MODULE );
		g_pe_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register pe device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */


	/* END */

	for ( i=0; i<PE_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_pe_major, g_pe_minor+i );
		cdev_init( &(g_pe_device[i].cdev), &g_pe_fops );
		g_pe_device[i].devno		= dev;
		g_pe_device[i].cdev.owner = THIS_MODULE;
		g_pe_device[i].cdev.ops   = &g_pe_fops;

		/* TODO: initialize minor device */


		/* END */

		err = cdev_add (&(g_pe_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding pe device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}
        OS_CreateDeviceClass ( g_pe_device[i].devno, "%s%d", PE_MODULE, i );
	}

#ifndef	KDRV_CONFIG_PM
	PE_SPD_probe();
#endif

	PE_DRV_DBG_PRINT("pe device initialized\n");

	return 0;
}

/**
 * pe cleanup
 *
 * @param   void
 * @return  void
 * @see
 * @author
 */
void PE_Cleanup(void)
{
	PE_DRV_DBG_PRINT("pe device cleanup\n");
}

/**
 * ioctl handler for pe pkt.
 *
 * @param   arg [in] unsigned long
 * @return  OK if success, ERROR otherwise.
 * @see
 * @author	sj.youm@lge.co
 */
static int	PE_DRV_ProcessPkt( unsigned long arg )
{
	int ret = RET_ERROR;
	LX_PE_PKT_T		recv_pkt={0,0,NULL};
	unsigned int	*recv_data=NULL;

	do{
		PE_DRV_DBG_PRINT("S\n");
		ret = copy_from_user(&recv_pkt, (void __user *)arg, sizeof(LX_PE_PKT_T));
		PE_DRV_CHECK_CODE(ret,break,"[%s,%d]copy_from_user error. \n",__F__,__L__);

		PE_DRV_CHECK_CODE(!(recv_pkt.size),break,"[%s,%d]recv_pkt.size is zero. \n",__F__,__L__);
		PE_DRV_CHECK_CODE(!(recv_pkt.data),break,"[%s,%d]recv_pkt.data is null. \n",__F__,__L__);

		recv_data = (unsigned int *)OS_Malloc(recv_pkt.size);
		PE_DRV_CHECK_CODE(!recv_data,break,"[%s,%d]recv_data is null. \n",__F__,__L__);
		ret = copy_from_user(recv_data, (void __user *)recv_pkt.data, recv_pkt.size);
		PE_DRV_CHECK_CODE(ret,break,"[%s,%d]copy_from_user error. \n",__F__,__L__);

		/* io functon call */
		ret = PE_IO_ProcessPkt(recv_data, &recv_pkt);
		PE_DRV_CHECK_CODE(ret,break,"[%s,%d]PE_IO_ProcessPkt() error. \n",__F__,__L__);

		if(PE_GET_PKTINFO_RWTYPE(recv_pkt.info)==PE_ITEM_PKTRW(GET))
		{
			PE_DRV_DBG_PRINT("PE_ITEM_PKTRW(GET) case..\n");
			ret = copy_to_user((void __user *)recv_pkt.data, recv_data, recv_pkt.size);
			PE_DRV_CHECK_CODE(ret,break,"[%s,%d]copy_to_user error. \n",__F__,__L__);
		}

		PE_DRV_DBG_PRINT("E\n");
	}while(0);

	if(recv_data)	OS_Free(recv_data);

	return ret;	
}

#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",PE_Init);
#else
module_init(PE_Init);
#endif
module_exit(PE_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("GPL");
#endif

