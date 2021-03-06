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
 *  main driver implementation for hdmi device.
 *	hdmi device will teach you how to make device driver with new platform.
 *
 *  author		sh.myoung (sh.myoung@lge.com)
 *  version		1.0
 *  date		2010.02.21
 *  note		Additional information.
 *
 *  @addtogroup lg1150_hdmi
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_HDMI_DEVICE_READ_WRITE_FOPS

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
#include "hdmi_drv.h"
#include "hdmi_module.h"

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
 *	main control block for hdmi device.
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
	int						dev_initialized;	///< check if device is initialized or not
// END of device specific data
}
HDMI_DEVICE_T;
typedef struct
{
	unsigned int read_write;
	unsigned int sub;
	unsigned int addr;
	unsigned int size;
	unsigned int value;

}S_INTERFACE_REG;

#ifdef KDRV_CONFIG_PM
typedef struct
{
	// add here extra parameter
	bool	is_suspended;
}HDMI_DRVDATA_T;
#endif

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	HDMI_PROC_Init(void);
extern	void	HDMI_PROC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		HDMI_Init(void);
void	HDMI_Cleanup(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_hdmi_debug_fd;
int 	g_hdmi_major = HDMI_MAJOR;
int 	g_hdmi_minor = HDMI_MINOR;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      HDMI_Open(struct inode *, struct file *);
static int      HDMI_Close(struct inode *, struct file *);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int 		HDMI_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#else
static long		HDMI_Ioctl (struct file *, unsigned int, unsigned long );
#endif
#ifdef SUPPORT_HDMI_DEVICE_READ_WRITE_FOPS
static ssize_t  HDMI_Read(struct file *, char *, size_t, loff_t *);
static ssize_t  HDMI_Write(struct file *, const char *, size_t, loff_t *);
#endif

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/**
 * main control block for hdmi device
*/
static HDMI_DEVICE_T*		g_hdmi_device;

/**
 * file I/O description for hdmi device
 *
*/
static struct file_operations g_hdmi_fops =
{
	.open 	= HDMI_Open,
	.release= HDMI_Close,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl	= HDMI_Ioctl,
#else
	.unlocked_ioctl= HDMI_Ioctl,
#endif

#ifdef SUPPORT_HDMI_DEVICE_READ_WRITE_FOPS
	.read 	= HDMI_Read,
	.write 	= HDMI_Write,
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
static int HDMI_suspend(struct platform_device *pdev, pm_message_t state)
{
	HDMI_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	// add here the suspend code

#if 1
	if(HDMI_RunSuspend() >=0 )
	{
	drv_data->is_suspended = 1;
	HDMI_PRINT("[%s] done suspend\n", HDMI_MODULE);
	}
	else
	{
		drv_data->is_suspended = 0;
		HDMI_PRINT("[%s] failed suspend\n", HDMI_MODULE);
	}
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
static int HDMI_resume(struct platform_device *pdev)
{
	HDMI_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;


	// add here the resume code
	HDMI_RunResume();


	drv_data->is_suspended = 0;
	HDMI_PRINT("[%s] done resume\n", HDMI_MODULE);
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
 int  HDMI_probe(struct platform_device *pdev)
{

	HDMI_DRVDATA_T *drv_data;

	drv_data = (HDMI_DRVDATA_T *)kmalloc(sizeof(HDMI_DRVDATA_T) , GFP_KERNEL);


	// add here driver registering code & allocating resource code



	HDMI_PRINT("[%s] done probe\n", HDMI_MODULE);
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
static int  HDMI_remove(struct platform_device *pdev)
{
	HDMI_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code



	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	HDMI_PRINT("released\n");

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
static void  HDMI_release(struct device *dev)
{


	HDMI_PRINT("device released\n");
}


/*
 *	module platform driver structure
 */
static struct platform_driver hdmi_driver =
{
	.probe          = HDMI_probe,
	.suspend        = HDMI_suspend,
	.remove         = HDMI_remove,
	.resume         = HDMI_resume,
	.driver         =
	{
		.name   = HDMI_MODULE,
	},
};

static struct platform_device hdmi_device = {
	.name = HDMI_MODULE,
	.id = 0,
	.id = -1,
	.dev = {
		.release = HDMI_release,
	},
};
#endif

int HDMI_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for hdmi device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_hdmi_debug_fd = DBG_OPEN( HDMI_MODULE );
	if(g_hdmi_debug_fd < 0) return -1;
	OS_DEBUG_EnableModule ( g_hdmi_debug_fd );

#if	1	//temp
	OS_DEBUG_DisableModuleByIndex ( g_hdmi_debug_fd, 0 );					//HDMI_PRINT
	OS_DEBUG_DisableModuleByIndex ( g_hdmi_debug_fd, 1 );      				//HDMI_TRACE
	OS_DEBUG_EnableModuleByIndex  ( g_hdmi_debug_fd, 2, DBG_COLOR_NONE );   //HDMI_ERROR
	OS_DEBUG_EnableModuleByIndex  ( g_hdmi_debug_fd, 3, DBG_COLOR_NONE );   //HDMI_DEBUG
	OS_DEBUG_DisableModuleByIndex ( g_hdmi_debug_fd, 4 );    				//AUDIO_ERROR
	OS_DEBUG_DisableModuleByIndex ( g_hdmi_debug_fd, 5 );       			//AUDIO_DEBUG
#else
	OS_DEBUG_EnableModuleByIndex ( g_hdmi_debug_fd, 0, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_hdmi_debug_fd, 1, DBG_COLOR_NONE );
	OS_DEBUG_EnableModuleByIndex ( g_hdmi_debug_fd, 2, DBG_COLOR_RED );    //HDMI_ERROR
	OS_DEBUG_EnableModuleByIndex ( g_hdmi_debug_fd, 3, DBG_COLOR_BLUE );   //HDMI_DEBUG
	OS_DEBUG_EnableModuleByIndex ( g_hdmi_debug_fd, 4, DBG_COLOR_RED );    //AUDIO_ERROR
	OS_DEBUG_EnableModuleByIndex ( g_hdmi_debug_fd, 5, DBG_COLOR_BLUE );   //AUDIO_DEBUG
#endif  //#if 1

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&hdmi_driver) < 0)
	{
		HDMI_PRINT("[%s] platform driver register failed\n",HDMI_MODULE);

	}
	else
	{
		if(platform_device_register(&hdmi_device))
		{
			platform_driver_unregister(&hdmi_driver);
			HDMI_PRINT("[%s] platform device register failed\n",HDMI_MODULE);
		}
		else
		{
			HDMI_PRINT("[%s] platform register done\n", HDMI_MODULE);
		}


	}
#endif

	g_hdmi_device = (HDMI_DEVICE_T*)OS_KMalloc( sizeof(HDMI_DEVICE_T)*HDMI_MAX_DEVICE );

	if ( NULL == g_hdmi_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(HDMI_DEVICE_T)* HDMI_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_hdmi_device, 0x0, sizeof(HDMI_DEVICE_T)* HDMI_MAX_DEVICE );

	if (g_hdmi_major)
	{
		dev = MKDEV( g_hdmi_major, g_hdmi_minor );
		err = register_chrdev_region(dev, HDMI_MAX_DEVICE, HDMI_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_hdmi_minor, HDMI_MAX_DEVICE, HDMI_MODULE );
		g_hdmi_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register hdmi device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */


	/* END */

	for ( i=0; i<HDMI_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_hdmi_major, g_hdmi_minor+i );
		cdev_init( &(g_hdmi_device[i].cdev), &g_hdmi_fops );
		g_hdmi_device[i].devno		= dev;
		g_hdmi_device[i].cdev.owner = THIS_MODULE;
		g_hdmi_device[i].cdev.ops   = &g_hdmi_fops;

		/* TODO: initialize minor device */


		/* END */

		err = cdev_add (&(g_hdmi_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding hdmi device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}
        OS_CreateDeviceClass ( g_hdmi_device[i].devno, "%s%d", HDMI_MODULE, i );
	}

	/* initialize proc system */
	HDMI_PROC_Init ( );

	HDMI_PRINT("hdmi device initialized\n");

	return 0;
}

void HDMI_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_hdmi_major, g_hdmi_minor );

#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	platform_driver_unregister(&hdmi_driver);
	platform_device_unregister(&hdmi_device);
#endif

	/* cleanup proc system */
	HDMI_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<HDMI_MAX_DEVICE;i++)
	{
		/* TODO: cleanup each minor device */


		/* END */
		cdev_del( &(g_hdmi_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */

	unregister_chrdev_region(dev, HDMI_MAX_DEVICE );

	OS_Free( g_hdmi_device );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for hdmi device
 *
 */
static int
HDMI_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    HDMI_DEVICE_T*	my_dev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, HDMI_DEVICE_T, cdev);

    /* TODO : add your device specific code */
   	if( my_dev->dev_open_count == 0 )
    {
		//reset initialized value
		my_dev->dev_initialized = 0;

    	//HDMI_Initialize();
    }

	/* END */

    my_dev->dev_open_count++;
    filp->private_data = my_dev;

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);

    //HDMI_Initialize(NULL);

    HDMI_PRINT("device opened (%d:%d)\n", major, minor );

    return 0;
}

/**
 * release handler for hdmi device
 *
 */
static int
HDMI_Close(struct inode *inode, struct file *file)
{
    int					major,minor;
    HDMI_DEVICE_T*	my_dev;
    struct cdev*		cdev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, HDMI_DEVICE_T, cdev);

    if ( my_dev->dev_open_count > 0 )
    {
        --my_dev->dev_open_count;
    }

    /* TODO : add your device specific code */
    HDMI_exit();

	/* END */

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);
    HDMI_PRINT("device closed (%d:%d)\n", major, minor );
    return 0;
}

/**
 * ioctl handler for hdmi device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int HDMI_Ioctl ( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
#else
static long HDMI_Ioctl ( struct file *filp, unsigned int cmd, unsigned long arg )
#endif
{
    int err = 0, ret = 0;

    HDMI_DEVICE_T*	my_dev;
    struct cdev*		cdev;

	LX_HDMI_INIT_T *init;
   	LX_HDMI_MODE_T *mode;
	LX_HDMI_ASPECTRATIO_T *ratio;
	LX_HDMI_TIMING_INFO_T *info;
	LX_HDMI_STATUS_T *status;
	LX_HDMI_VSI_PACKET_T *vsi_packet;
	LX_HDMI_AVI_PACKET_T *avi_packet;
	LX_HDMI_SPD_PACKET_T *spd_packet;
	LX_HDMI_HPD_T *hpd;
	LX_HDMI_MUTE_CTRL_T *mute;
	LX_HDMI_AVI_COLORSPACE_T *colorspace;
	LX_HDMI_AUDIO_INFO_T *audio;
	LX_HDMI_PHY_REG_WRITE_T *reg;
	LX_HDMI_ARC_CTRL_T *arc;
	LX_HDMI_POWER_CTRL_T *power;
	LX_HDMI_UD_3D_MODE_T *udn3d;

	UINT32 temp;
	UINT32 temp_arg;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	/*
	 * get current hdmi device object
	 */
    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, HDMI_DEVICE_T, cdev);
#else
	my_dev	= (HDMI_DEVICE_T *)filp->private_data;
	cdev	= &my_dev->cdev;
#endif


    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != HDMI_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > HDMI_IOC_MAXNR)
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

	HDMI_TRACE("cmd = %08X (cmd_idx=%d)\n", cmd, _IOC_NR(cmd) );

	switch(cmd)
	{
		case HDMI_IOW_INIT:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IO_INIT IOCTL\n");
			if( my_dev->dev_initialized == 0 )
			{
				init = (LX_HDMI_INIT_T *)kmalloc(sizeof(LX_HDMI_INIT_T),GFP_KERNEL);
				ret = copy_from_user((void *)init, (void *)arg, sizeof(LX_HDMI_INIT_T));

				if(ret != 0)
				{
					break;
				}

				ret = HDMI_Initialize(init);

				kfree(init);

				//set my_dev->dev_initialized value
				if(ret == 0)
					my_dev->dev_initialized = 1;
			}
			else
				ret = 0;
		}
		break;

		case HDMI_IOR_GET_MODE:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_MODE IOCTL\n");
			mode = (LX_HDMI_MODE_T *)kmalloc(sizeof(LX_HDMI_MODE_T) , GFP_KERNEL);
			ret = copy_from_user((void *)mode, (void *)arg, sizeof(LX_HDMI_MODE_T));
			ret = HDMI_GetMode(mode);
			ret = copy_to_user((void *)arg, (void *)mode, sizeof(LX_HDMI_MODE_T));

			kfree(mode);
			ret = 0;
		}
		break;

		case HDMI_IOR_GET_ASPECTRATIO:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_ASPECTRATIO IOCTL\n");
			ratio = (LX_HDMI_ASPECTRATIO_T *)kmalloc(sizeof(LX_HDMI_ASPECTRATIO_T) , GFP_KERNEL);

			ret = HDMI_GetAspectRatio(ratio);
			ret = copy_to_user((void *)arg, (void *)ratio, sizeof(LX_HDMI_ASPECTRATIO_T));

			kfree(ratio);
		}
		break;

		case HDMI_IOR_GET_TIMING_INFO:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_TIMING_INFO IOCTL\n");

			info = (LX_HDMI_TIMING_INFO_T *)kmalloc(sizeof(LX_HDMI_TIMING_INFO_T) , GFP_KERNEL);

			ret = HDMI_GetTimingInfo(info);
			ret = copy_to_user((void *)arg, (void *)info, sizeof(LX_HDMI_TIMING_INFO_T));

			kfree(info);
		}
		break;

		case HDMI_IOR_GET_HDMI_STATUS:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_HDMI_STATUS IOCTL\n");

			status = (LX_HDMI_STATUS_T *)kmalloc(sizeof(LX_HDMI_STATUS_T) , GFP_KERNEL);

			ret = HDMI_GetStatus(status);
			ret |= copy_to_user((void *)arg, (void *)status, sizeof(LX_HDMI_STATUS_T));

			kfree(status);
		}
		break;

		case HDMI_IOR_GET_HDMI_VSI:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_HDMI_VSI IOCTL\n");

			vsi_packet = (LX_HDMI_VSI_PACKET_T *)kmalloc(sizeof(LX_HDMI_VSI_PACKET_T) , GFP_KERNEL);

			ret = HDMI_GetVsiPacket(vsi_packet);
			ret = copy_to_user((void *)arg, (void *)vsi_packet, sizeof(LX_HDMI_VSI_PACKET_T));

			kfree(vsi_packet);

		}
		break;

		case HDMI_IOR_GET_HDMI_AVI:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_HDMI_AVI IOCTL\n");

			avi_packet = (LX_HDMI_AVI_PACKET_T *)kmalloc(sizeof(LX_HDMI_AVI_PACKET_T) , GFP_KERNEL);

			ret = HDMI_GetAviPacket(avi_packet);
			ret = copy_to_user((void *)arg, (void *)avi_packet, sizeof(LX_HDMI_AVI_PACKET_T));

			kfree(avi_packet);

		}
		break;

		case HDMI_IOW_SET_HPD:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_SET_HPD IOCTL\n");

			hpd = (LX_HDMI_HPD_T *)kmalloc(sizeof(LX_HDMI_HPD_T) , GFP_KERNEL);

			ret = copy_from_user((void *)hpd, (void *)arg, sizeof(LX_HDMI_HPD_T));
			ret = HDMI_SetHPDEnDisable(hpd);	// set hot plug detect

			kfree(hpd);
		}
		break;

		case HDMI_IOW_SET_PORT:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_SET_PORT IOCTL\n");

			hpd = (LX_HDMI_HPD_T *)kmalloc(sizeof(LX_HDMI_HPD_T) , GFP_KERNEL);

			ret = copy_from_user((void *)hpd, (void *)arg, sizeof(LX_HDMI_HPD_T));
			ret = HDMI_SetPort(hpd);	// set hot plug detect

			kfree(hpd);

		}
		break;

		case HDMI_IOR_GET_INFO_FOR_AUDIO:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_INFO_FOR_AUDIO\n");
			audio = (LX_HDMI_AUDIO_INFO_T *)kmalloc(sizeof(LX_HDMI_AUDIO_INFO_T) , GFP_KERNEL);
			ret = copy_from_user((void *)audio, (void *)arg, sizeof(LX_HDMI_AUDIO_INFO_T));
			ret = HDMI_GetAudioInfo(audio);
			ret |= copy_to_user((void *)arg, (void *)audio, sizeof(LX_HDMI_AUDIO_INFO_T));
			kfree(audio);
		}
		break;

		case HDMI_IOW_SET_MUTE_CONTROL:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_SET_MUTE_CONTROL\n");
			mute = (LX_HDMI_MUTE_CTRL_T *)kmalloc(sizeof(LX_HDMI_MUTE_CTRL_T) , GFP_KERNEL);
			ret = copy_from_user((void *)mute, (void *)arg, sizeof(LX_HDMI_MUTE_CTRL_T));

			ret |= HDMI_SetMute(mute);
			kfree(mute);
		}
		break;

		case HDMI_IOW_SET_RGB_YCBCR:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_SET_RGB_YCBCR\n");
			colorspace = (LX_HDMI_AVI_COLORSPACE_T *)kmalloc(sizeof(LX_HDMI_AVI_COLORSPACE_T) , GFP_KERNEL);
			ret = copy_from_user((void *)colorspace, (void *)arg, sizeof(LX_HDMI_AVI_COLORSPACE_T));

			ret |= HDMI_SetRGBYCbCr(colorspace);
			kfree(colorspace);
		}
		break;

		case HDMI_IOR_GET_HDMI_SPD:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOR_GET_HDMI_SPD\n");
#if 1
			spd_packet = (LX_HDMI_SPD_PACKET_T *)kmalloc(sizeof(LX_HDMI_SPD_PACKET_T) , GFP_KERNEL);

			if (HDMI_GetSpdPacket != NULL)
			{
				ret = HDMI_GetSpdPacket(spd_packet);
				ret |= copy_to_user((void *)arg, (void *)spd_packet, sizeof(LX_HDMI_SPD_PACKET_T));

			}
			else
			{
				HDMI_ERROR("[hdmi_drv] No function HDMI_GetSpdPacket()\n");
				ret = 0;
			}

			kfree(spd_packet);
#endif
		}
		break;

		case HDMI_IOW_SET_ARC:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_SET_ARC\n");

			arc = (LX_HDMI_ARC_CTRL_T *)kmalloc(sizeof(LX_HDMI_ARC_CTRL_T) , GFP_KERNEL);
			ret = copy_from_user((void *)arc, (void *)arg, sizeof(LX_HDMI_ARC_CTRL_T));

			ret |= HDMI_SetArc(arc);
			kfree(arc);
		}
		break;

		case HDMI_IOW_POWER_CONTROL:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_POWER_CONTROL\n");

			power = (LX_HDMI_POWER_CTRL_T *)kmalloc(sizeof(LX_HDMI_POWER_CTRL_T) , GFP_KERNEL);
			ret = copy_from_user((void *)power, (void *)arg, sizeof(LX_HDMI_POWER_CTRL_T));

			ret |= HDMI_PowerConsumption(power);
			kfree(power);
		}
		break;

		case HDMI_IOW_SET_UD_3D_MODE:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_SET_3D_N_UD\n");

			udn3d = (LX_HDMI_UD_3D_MODE_T *)kmalloc(sizeof(LX_HDMI_UD_3D_MODE_T) , GFP_KERNEL);
			ret = copy_from_user((void *)udn3d, (void *)arg, sizeof(LX_HDMI_UD_3D_MODE_T));

			ret |= HDMI_SetUDn3DControl(udn3d);
			kfree(udn3d);
		}
		break;

		case HDMI_IOR_REG_RD:
		{
			HDMI_PRINT("HDMI_IOR_PHY_REG_RD IOCTL\n");
			ret = copy_from_user((void *)&temp_arg , (void *)arg , sizeof(UINT32));

			ret = HDMI_GetRegister(temp_arg, &temp);

			ret |= copy_to_user((void *)arg , (void *)&temp , sizeof(UINT32));

		}
		break;

		case HDMI_IOW_REG_WR:
		{
			HDMI_PRINT("HDMI_IOW_PHY_REG_WR IOCTL\n");
			reg = (LX_HDMI_PHY_REG_WRITE_T *)kmalloc(sizeof(LX_HDMI_PHY_REG_WRITE_T) , GFP_KERNEL);

			ret = copy_from_user((void *)reg , (void *)arg , sizeof(LX_HDMI_PHY_REG_WRITE_T));

			ret |= HDMI_SetRegister(reg->wr_addr , reg->wr_data);

			kfree(reg);

		}
		break;

		case HDMI_IOW_HDCP_ENABLE:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_HDCP_ENABLE IOCTL\n");
			ret = 0;
		}
		break;

		case HDMI_IOW_SET_HDCP:
		{
			HDMI_PRINT("[hdmi_drv]HDMI_IOW_SET_HDCP IOCTL\n");
			ret = 0;
		}
		break;

/*
		case HDMI_IOW_INTERFACE:
		{
			HDMI_PRINT("ADC interface with register\n");
			reg = (S_HDMI_INTERFACE_REG *)kmalloc(sizeof(S_HDMI_INTERFACE_REG),GFP_KERNEL);

			ret = copy_from_user((void *)reg, (void *)arg, sizeof(S_HDMI_INTERFACE_REG));

			HDMI_RunRegister(reg);

			kfree(reg);
			ret = 0;
		}
		break;
*/
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
user_initcall_grp("kdrv",HDMI_Init);
#else
module_init(HDMI_Init);
#endif
module_exit(HDMI_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

