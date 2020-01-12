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
 *  main driver implementation for png device.
 *	png device will teach you how to make device driver with new platform.
 *
 *  author		raxis.lim (raxis.lim@lge.com).lim (raxis.lim@lge.com)
 *  version		1.0
 *  date		2010.10.05
 *  note		Additional information.
 *
 *  @addtogroup lg1150_png
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef	SUPPORT_PNG_DEVICE_READ_WRITE_FOPS

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
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <asm/irq.h>                    /**< For isr */
#include <asm/io.h>                    /**< For isr */
#include <linux/version.h>

#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
#include <linux/platform_device.h>
#endif

#include "os_util.h"
#include "base_dev_cfg.h"
#include "misc_util.h"
#include "png_drv.h"

#include "png_reg_api.h"
#include "venc_cfg.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#undef	PNG_DEBUG

#define SLKRD_MAX_ADDRESS	0x20000000		/* 2**29. high 3bits are port address in silkroad */

/* Youngwoo.Jin (2011/04/28)
 * L9 PNG decoder needs extra buffer for window and filter
 */
#define PNG_WINDOW_BUFF_SIZE		0x8000
#define PNG_FILTER_BUFF_SIZE		0x4000

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
enum
{
	PNG_EVENT_DECODE_DONE	= 0x0001,
};

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/**
 *	main control block for png device.
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
	LX_PNG_DECODE_METHOD_T	png_decode_method;		///< png decode method ( full or partial )
	OS_EVENT_T				png_event;				///< evnet to catch decode_done
	LX_PNG_HDR_T			png_img_hdr;			///< header information of current image
	LX_MEM_BUF_T			png_src_buf;			///< working buffer information
#ifdef PNG_DEBUG
	LX_MEM_BUF_T			png_src_buf_virt;
#endif
	LX_MEM_STREAM_T			png_out_stream;			///< output stream information
	int						png_img_chunk_seqno;	///< seqeunce number of PNG chunk

	UINT64					png_decode_start_usec;	///< micro second
	UINT64					png_decode_end_usec;	///< micro second

// END of device specific data
}
PNG_DEVICE_T;

#ifdef KDRV_CONFIG_PM	// added by SC Jung for quick booting
typedef struct
{
	// add here extra parameter
	bool	is_suspended;
} PNG_DRVDATA_T;
#endif

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern	void	PNG_PROC_Init(void);
extern	void	PNG_PROC_Cleanup(void);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/
extern	LX_PNG_DECODE_CAPS_T	g_png_decode_caps;

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
int		PNG_Init(void);
void	PNG_Cleanup(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int		g_png_debug_fd;
int     g_png_trace_depth;
int 	g_png_major = PNG_MAJOR;
int 	g_png_minor = PNG_MINOR;

irqreturn_t (*PNG_ISRCallback)(int irq, void *dev_id, struct pt_regs *regs) = NULL;
irqreturn_t PNG_ISRHandler(int irq, void *dev_id, struct pt_regs *regs);

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int      PNG_Open(struct inode *, struct file *);
static int      PNG_Close(struct inode *, struct file *);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int 		PNG_Ioctl (struct inode *, struct file *, unsigned int, unsigned long );
#else
static long		PNG_Ioctl (struct file *, unsigned int, unsigned long );
#endif
static int      PNG_mmap(struct file *, struct vm_area_struct *);
#ifdef SUPPORT_PNG_DEVICE_READ_WRITE_FOPS
static ssize_t  PNG_Read(struct file *, char *, size_t, loff_t *);
static ssize_t  PNG_Write(struct file *, const char *, size_t, loff_t *);
#endif


static	void	PNG_InitReg 	( PNG_DEVICE_T* png_device );
static	void	PNG_InitHW	 	( PNG_DEVICE_T* png_device );
static	void	PNG_ShutdownHW	( PNG_DEVICE_T* png_device );
static	void	PNG_SwReset		( PNG_DEVICE_T* png_device );
static	void	PNG_EnableISR	( PNG_DEVICE_T* png_device, BOOLEAN fOn );
static	int		PNG_DecodeStart	( PNG_DEVICE_T* png_device, LX_PNG_DECODE_PARAM_T* decode_param );
static	int		PNG_DecodeStop	( PNG_DEVICE_T* png_device );

static	int		PNG_SyncDecode	( PNG_DEVICE_T* png_device );

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static	BOOLEAN	g_host_init_done;
static	BOOLEAN g_png_init_done;

/**
 * main control block for png device
*/
static PNG_DEVICE_T*		g_png_device;

/**
 * file I/O description for png device
 *
*/
static struct file_operations g_png_fops =
{
	.open 	= PNG_Open,
	.release= PNG_Close,
	.mmap	= PNG_mmap,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
	.ioctl	= PNG_Ioctl,
#else
	.unlocked_ioctl = PNG_Ioctl,
#endif
#ifdef SUPPORT_PNG_DEVICE_READ_WRITE_FOPS
	.read 	= PNG_Read,
	.write 	= PNG_Write,
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
static int
PNG_suspend(struct platform_device *pdev, pm_message_t state)
{
	PNG_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	// add here the suspend code

	if ( drv_data->is_suspended == 1 )
	{
		return -1;	//If already in suspend state, so ignore
	}

	drv_data->is_suspended = 1;
	PNG_PRINT("[%s] done suspend\n", PNG_MODULE);

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
static int
PNG_resume(struct platform_device *pdev)
{
	PNG_DRVDATA_T *drv_data;

	drv_data = platform_get_drvdata(pdev);

	if(drv_data->is_suspended == 0) return -1;

	// add here the resume code

	drv_data->is_suspended = 0;
	PNG_PRINT("[%s] done resume\n", PNG_MODULE);
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
 int
PNG_probe(struct platform_device *pdev)
{
	PNG_DRVDATA_T *drv_data;

	drv_data = (PNG_DRVDATA_T *)kmalloc(sizeof(PNG_DRVDATA_T) , GFP_KERNEL);

	// add here driver registering code & allocating resource code

	PNG_PRINT("[%s] done probe\n", PNG_MODULE);
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
static int
PNG_remove(struct platform_device *pdev)
{
	PNG_DRVDATA_T *drv_data;

	// add here driver unregistering code & deallocating resource code

	drv_data = platform_get_drvdata(pdev);
	kfree(drv_data);

	PNG_PRINT("released\n");

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
static void
PNG_release(struct device *dev)
{
	PNG_PRINT("device released\n");
}

/*
 *	module platform driver structure
 */
static struct platform_driver png_driver =
{
	.probe			= PNG_probe,
	.suspend		= PNG_suspend,
	.remove 		= PNG_remove,
	.resume 		= PNG_resume,
	.driver 		=
	{
		.name	= PNG_MODULE,
	},
};

static struct platform_device png_device = {
	.name	= PNG_MODULE,
	.id 	= -1,
	.dev	= {
		.release = PNG_release,
	},
};
#endif


int PNG_Init(void)
{
	int			i;
	int			err;
	dev_t		dev;

	/* Get the handle of debug output for png device.
	 *
	 * Most module should open debug handle before the real initialization of module.
	 * As you know, debug_util offers 4 independent debug outputs for your device driver.
	 * So if you want to use all the debug outputs, you should initialize each debug output
	 * using OS_DEBUG_EnableModuleByIndex() function.
	 */
	g_png_debug_fd = DBG_OPEN( PNG_MODULE );
	if ( g_png_debug_fd >= 0 )
	{
		OS_DEBUG_EnableModule ( g_png_debug_fd );
//		OS_DEBUG_EnableModuleByIndex ( g_png_debug_fd, 0, DBG_COLOR_NONE );
//		OS_DEBUG_EnableModuleByIndex ( g_png_debug_fd, 1, DBG_COLOR_GREEN );
		OS_DEBUG_EnableModuleByIndex ( g_png_debug_fd, 2, DBG_COLOR_YELLOW );
		OS_DEBUG_EnableModuleByIndex ( g_png_debug_fd, 3, DBG_COLOR_RED );
	}

	/* allocate main device handler, register current device.
	 *
	 * If devie major is predefined then register device using that number.
	 * otherwise, major number of device is automatically assigned by Linux kernel.
	 *
	 */
#ifdef KDRV_CONFIG_PM
	// added by SC Jung for quick booting
	if(platform_driver_register(&png_driver) < 0)
	{
		PNG_PRINT("[%s] platform driver register failed\n",PNG_MODULE);
	}
	else
	{
		if(platform_device_register(&png_device))
		{
			platform_driver_unregister(&png_driver);
			PNG_PRINT("[%s] platform device register failed\n",PNG_MODULE);
		}
		else
		{
			PNG_PRINT("[%s] platform register done\n", PNG_MODULE);
		}
	}
#endif

	g_png_device = (PNG_DEVICE_T*)OS_KMalloc( sizeof(PNG_DEVICE_T)*PNG_MAX_DEVICE );

	if ( NULL == g_png_device )
	{
		DBG_PRINT_ERROR("out of memory. can't allocate %d bytes\n", sizeof(PNG_DEVICE_T)* PNG_MAX_DEVICE );
		return -ENOMEM;
	}

	memset( g_png_device, 0x0, sizeof(PNG_DEVICE_T)* PNG_MAX_DEVICE );

	if (g_png_major)
	{
		dev = MKDEV( g_png_major, g_png_minor );
		err = register_chrdev_region(dev, PNG_MAX_DEVICE, PNG_MODULE );
	}
	else
	{
		err = alloc_chrdev_region(&dev, g_png_minor, PNG_MAX_DEVICE, PNG_MODULE );
		g_png_major = MAJOR(dev);
	}

	if ( err < 0 )
	{
		DBG_PRINT_ERROR("can't register png device\n" );
		return -EIO;
	}

	/* TODO : initialize your module not specific minor device */
	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )
	{
		err = request_irq( L9_IRQ_ICOD0, (irq_handler_t)PNG_ISRHandler, 0, "L9-PNG", NULL);

		if ( err )
		{
			unregister_chrdev_region(dev, PNG_MAX_DEVICE);
			return -EIO;
		}

		g_host_init_done = TRUE;
	}
	else
	{
		g_host_init_done = FALSE;
	}

	g_png_init_done	 = FALSE;

	/* END */

	for ( i=0; i<PNG_MAX_DEVICE; i++ )
	{
		/* initialize cdev structure with predefined variable */
		dev = MKDEV( g_png_major, g_png_minor+i );
		cdev_init( &(g_png_device[i].cdev), &g_png_fops );
		g_png_device[i].devno		= dev;
		g_png_device[i].cdev.owner = THIS_MODULE;
		g_png_device[i].cdev.ops   = &g_png_fops;

		/* TODO: initialize minor device */


		/* END */

		err = cdev_add (&(g_png_device[i].cdev), dev, 1 );

		if (err)
		{
			DBG_PRINT_ERROR("error (%d) while adding png device (%d.%d)\n", err, MAJOR(dev), MINOR(dev) );
			return -EIO;
		}

		OS_CreateDeviceClass ( g_png_device[i].devno, "%s%d", PNG_MODULE, i );
	}

	/* initialize proc system */
	PNG_PROC_Init ( );

#ifdef PNG_DEBUG
	PNG_PRINT("png device initialized\n");
#endif

	return 0;
}

void PNG_Cleanup(void)
{
	int i;
	dev_t dev = MKDEV( g_png_major, g_png_minor );

	/* cleanup proc system */
	PNG_PROC_Cleanup( );

	/* remove all minor devicies and unregister current device */
	for ( i=0; i<PNG_MAX_DEVICE;i++)
	{
		/* TODO: cleanup each minor device */


		/* END */
		cdev_del( &(g_png_device[i].cdev) );
	}

	/* TODO : cleanup your module not specific minor device */
	free_irq( L9_IRQ_ICOD0, NULL );

	unregister_chrdev_region(dev, PNG_MAX_DEVICE );

	OS_Free( g_png_device );
}


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * open handler for png device
 *
 */
static int
PNG_Open(struct inode *inode, struct file *filp)
{
    int					major,minor;
    struct cdev*    	cdev;
    PNG_DEVICE_T*		my_dev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, PNG_DEVICE_T, cdev);

    /* TODO : add your device specific code */
	if ( my_dev->dev_open_count == 0 )
	{
		OS_InitEvent( &my_dev->png_event );
		PNG_InitReg( my_dev );
		/* PNG_InitHW may not be execulted until host initialized. but don't worry */
		PNG_InitHW( my_dev );

	}

	/* END */
    my_dev->dev_open_count++;
    filp->private_data = my_dev;

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);

#ifdef PNG_DEBUG
    PNG_PRINT("device opened (%d:%d)\n", major, minor );
#endif

    return 0;
}

/**
 * release handler for png device
 *
 */
static int
PNG_Close(struct inode *inode, struct file *file)
{
    int				major,minor;
    PNG_DEVICE_T*	my_dev;
    struct cdev*	cdev;

    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, PNG_DEVICE_T, cdev);

    if ( my_dev->dev_open_count > 0 )
    {
        --my_dev->dev_open_count;
    }

    /* TODO : add your device specific code */
	if ( my_dev->dev_open_count <= 0 )
	{
		PNG_ShutdownHW	( my_dev );
	}

	/* END */

	/* some debug */
    major = imajor(inode);
    minor = iminor(inode);

#ifdef PNG_DEBUG
    PNG_PRINT("device closed (%d:%d)\n", major, minor );
#endif

    return 0;
}

/**
 * memory mapping to virtual region
 *
 */
static int PNG_mmap(struct file *file, struct vm_area_struct *vma)
{
    int ret;
    UINT32 start;
    UINT32 len;
    UINT32 off = ((vma->vm_pgoff) << PAGE_SHIFT);

    PNG_TRACE_BEGIN();
    start = gpMemCfgPng->memory_base & PAGE_MASK;
    len = PAGE_ALIGN((start & ~PAGE_MASK) + gpMemCfgPng->memory_size);

#ifdef PNG_DEBUG
    PNG_PRINT("MMAP : start - %08x , len - %08x , off - %08x\n" , start , len , off);
#endif

    if ((vma->vm_end - vma->vm_start + off) > len)
    {
        PNG_ERROR("ERROR : PNG_mmap leng is over\n");
        PNG_PRINT("start : %x end : %x offset : %x len : %x\n" , (UINT32)vma->vm_start , (UINT32)vma->vm_end , off , len);
        return RET_INVALID_PARAMS;
    }

    off += start;
    PNG_PRINT("%s:%d -- mmap : start - %08x , len - %08x , off - %08x\n", __F__, __L__, start , len , (UINT32)vma->vm_pgoff);

    vma->vm_pgoff = off >> PAGE_SHIFT;
    vma->vm_flags |= VM_IO;
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

    ret = io_remap_pfn_range(vma,   vma->vm_start, off >> PAGE_SHIFT,
                                    vma->vm_end - vma->vm_start,
                                    vma->vm_page_prot );

    PNG_TRACE_END();

    return ret;
}

/**
 * ioctl handler for png device.
 *
 *
 * note: if you have some critial data, you should protect them using semaphore or spin lock.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
static int
PNG_Ioctl ( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg )
{
#else
static long
PNG_Ioctl ( struct file *filp, unsigned int cmd, unsigned long arg )
{
	struct inode *inode = filp->f_path.dentry->d_inode;
#endif
    int err = 0, ret = 0;

    PNG_DEVICE_T*	my_dev;
    struct cdev*	cdev;

	/*
	 * get current png device object
	 */
    cdev	= inode->i_cdev;
    my_dev	= container_of ( cdev, PNG_DEVICE_T, cdev);

	PNG_TRACE_BEGIN();
    /*
     * check if IOCTL command is valid or not.
     * - if magic value doesn't match, return error (-ENOTTY)
     * - if command is out of range, return error (-ENOTTY)
     *
     * note) -ENOTTY means "Inappropriate ioctl for device.
     */
    if (_IOC_TYPE(cmd) != PNG_IOC_MAGIC)
    {
    	DBG_PRINT_WARNING("invalid magic. magic=0x%02X\n", _IOC_TYPE(cmd) );
		PNG_TRACE_END();
    	return -ENOTTY;
    }
    if (_IOC_NR(cmd) > PNG_IOC_MAXNR)
    {
    	DBG_PRINT_WARNING("out of ioctl command. cmd_idx=%d\n", _IOC_NR(cmd) );
		PNG_TRACE_END();
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
		PNG_TRACE_END();
        return -EFAULT;
	}

	PNG_TRACE("cmd = %08X (cmd_idx=%d)\n", cmd, _IOC_NR(cmd) );

	switch(cmd)
	{
		case PNG_IOR_GET_CAPS:
		{
			UINT32	caps_val = g_png_decode_caps;
			ret = copy_to_user((void __user*)arg, (void *)&caps_val, sizeof(UINT32));
			PNG_CHECK_CODE( ret, goto func_exit, "[png] %s : arg copy failed\n", __F__ );
		}
		break;

		case PNG_IOR_DECODE_BUFFER_SIZE:
		{
			UINT32	sz = gpMemCfgPng->memory_size;
			ret = copy_to_user((void __user*)arg, (void *)&sz, sizeof(UINT32));
			PNG_CHECK_CODE( ret, goto func_exit, "[png] %s : arg copy failed\n", __F__ );
		}
		break;

		case PNG_IO_SW_RESET:
		{
			PNG_InitHW( my_dev );
			PNG_CHECK_CODE( !g_png_init_done, ret=RET_ERROR; break, "png hw is not initialized" );
			PNG_SwReset( my_dev );
		}
		break;

		case PNG_IOW_DECODE_INIT:
		{
			LX_PNG_DECODE_INIT_PARAM_T	init_param;

			PNG_InitHW( my_dev );
			PNG_CHECK_CODE( !g_png_init_done, ret=RET_ERROR; break, "png hw is not initialized" );

			/* raxis.lim (2010/12/17)
			 * PNG decoder should be reset before decoding png image. otherwise, decoder will work well
			 *
			 * Youngwoo.Jin (2011/04/28)
			 * disable SW reset
		 	 */
			//PNG_SwReset( my_dev );

			ret = copy_from_user((void *)&init_param, (void __user *)arg, sizeof(LX_PNG_DECODE_INIT_PARAM_T));
			PNG_CHECK_CODE( ret, goto func_exit, "[png] %s : arg copy failed\n", __F__ );

			if ( !(g_png_decode_caps & LX_PNG_DECODE_CAPS_PARTIAL_DECODE) )
			{
				/* partial decode not supported */
				PNG_CHECK_CODE( init_param.decode_method != LX_PNG_DECODE_METHOD_FULL_DECODE,
								ret = RET_INVALID_PARAMS; goto func_exit,
								"[png] %s : not support partial decode\n", __F__ );

				/* too big size to decode image */
				PNG_CHECK_CODE( init_param.img_sz >= gpMemCfgPng->memory_size,
								ret = RET_OUT_OF_MEMORY; goto func_exit,
								"[png] %s : image size (%d) is too big to decode\n", __F__, init_param.img_sz );
			}

			memcpy( &my_dev->png_img_hdr, &init_param.img_hdr, sizeof(LX_PNG_HDR_T));
			memcpy( &my_dev->png_out_stream.mem_buf, &init_param.out_buf, sizeof(LX_MEM_BUF_T));
			my_dev->png_decode_method = init_param.decode_method;
			my_dev->png_out_stream.curr_offset 	= 0;
			my_dev->png_img_chunk_seqno 		= 0;
		}
		break;

		case PNG_IOW_DECODE_START:
		{
			LX_PNG_DECODE_PARAM_T	decode_param;

			PNG_InitHW( my_dev );
			PNG_CHECK_CODE( !g_png_init_done, ret=RET_ERROR; break, "png hw is not initialized" );

			ret = copy_from_user((void *)&decode_param, (void __user *)arg, sizeof(LX_PNG_DECODE_PARAM_T));
			PNG_CHECK_CODE( ret, goto func_exit, "[png] %s : arg copy failed\n", __F__ );

			ret = PNG_DecodeStart( my_dev, &decode_param );
			PNG_CHECK_CODE( ret, goto func_exit, "[png] %s : png decode start failed\n", __F__ );
		}
		break;

		case PNG_IO_DECODE_STOP:
		{
			PNG_InitHW( my_dev );
			PNG_CHECK_CODE( !g_png_init_done, ret=RET_ERROR; break, "png hw is not initialized" );

			ret = PNG_DecodeStop( my_dev );
		}
		break;
    }

func_exit:
	PNG_TRACE_END();

    return ret;
}

/*========================================================================================
	Implementation Group
========================================================================================*/

/** initialize PNG deivce
 *
 */
static	void	PNG_InitReg ( PNG_DEVICE_T* png_device )
{
	PNG_CFG_DeviceConfig();
	PNG_REG_Initialize();

	png_device->png_src_buf.addr 	= gpMemCfgPng->memory_base;
	png_device->png_src_buf.length	= gpMemCfgPng->memory_size - PNG_WINDOW_BUFF_SIZE - PNG_FILTER_BUFF_SIZE;

#ifdef PNG_DEBUG
	png_device->png_src_buf_virt.addr 	= ioremap( gpMemCfgPng->memory_base, gpMemCfgPng->memory_size );
	png_device->png_src_buf_virt.length	= gpMemCfgPng->memory_size;
#endif
}

static	void	PNG_InitHW ( PNG_DEVICE_T* png_device )
{
	if ( !g_host_init_done ) return;	/* check whether host deivce initialized */
	if ( g_png_init_done ) return;		/* check png is already initialized */

	/* raxis.lim (2010/10/05)
	 * since png module itself is sub part of venc module, so png doesn't have any authority
	 * to control the interrupt.
	 * Youngwoo Jin (2011/04/22)
	 * Modify from compile branch to runtime branch
	 */
#ifdef PNG_DEBUG
	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) ) { }
	else
	{
#ifdef	PNG_SUPPORT_LINUX_IRQ
		DBG_PRINT_ERROR("PNG doesn't have its own ISR\n", );
#else
		/* L8 : register callback to venc */
#endif
	}
#endif

	PNG_SwReset 	( png_device );

	/* ok. png device is initialized */
	g_png_init_done = TRUE;
}

/** shutdown PNG deivce
 *
 */
static	void	PNG_ShutdownHW	( PNG_DEVICE_T* png_device )
{
	PNG_EnableISR	( png_device, FALSE );
}

/** soft reset
 *
 */
static	void	PNG_SwReset ( PNG_DEVICE_T* png_device )
{
	PNG_TRACE_BEGIN();
	PNG_EnableISR( png_device, FALSE );
	PNG_REG_Reset( );

	/* Youngwoo.Jin (11/04/27)
	 * Must enable interrupts because PNG decoder of L9 reset interrupt mask when sw reset done.
	 */
	PNG_EnableISR	( png_device, TRUE );
	PNG_TRACE_END();
}

static	void	PNG_EnableISR	( PNG_DEVICE_T* png_device, BOOLEAN fOn )
{
	PNG_TRACE_BEGIN();

	/*	register PNG_Interrupt function pointer. it is used in venc_isr.c */
	PNG_ISRCallback = (fOn)? PNG_ISRHandler: NULL;

	PNG_REG_SetInterruptMask( 0x0FFF, fOn );

#ifdef PNG_DEBUG
	PNG_PRINT("ISR %s\n", fOn? "enabled":"disabled" );
#endif

	PNG_TRACE_END();
}

/** start decode
 *
 *
 */
int		PNG_DecodeStart	( PNG_DEVICE_T* png_device, LX_PNG_DECODE_PARAM_T* decode_param )
{
	UINT32	read_base_addr;
	UINT32	write_base_addr;
	UINT32	read_byte_size;
	int	ret = RET_ERROR;
	PNG_TRACE_BEGIN();

	/* raxis.lim(2011/10/18) clear PNG event before starting new decode.
	 * without this code, abnomal OK event will be received.
	 * ( when PNG decoding timeout occurrs, next PNG decoding will be completed abnormally )
	 */
	OS_ClearEvent( &png_device->png_event );

	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )
	{
		/* nop */
	}
	else
	{
		PNG_CHECK_CODE( decode_param->offset != 0, ret = RET_INVALID_PARAMS; goto func_exit,
						"[png] %s : offset(%d) value should be zero\n", __F__,
								decode_param->offset );
		PNG_CHECK_CODE( png_device->png_img_chunk_seqno != 0, ret = RET_ERROR; goto func_exit,
						"[png] %s : png chunk seqno(%d) shoule be zero\n", __F__,
								png_device->png_img_chunk_seqno );
		PNG_CHECK_CODE( png_device->png_out_stream.curr_offset != 0, ret = RET_ERROR; goto func_exit,
						"[png] %s : png out stream(%d) shoule be offset zero\n", __F__,
								png_device->png_out_stream.curr_offset );
	}

	if ( png_device->png_img_chunk_seqno == 0 )
	{
		/* write header information to png decoder */
		PNG_REG_SetImageInfo( png_device->png_img_hdr.width, png_device->png_img_hdr.height,
							  png_device->png_img_hdr.bit_depth, png_device->png_img_hdr.color_type );

#ifdef PNG_DEBUG
		PNG_PRINT("image w*h = %dx%d bit %d color %d\n",
									png_device->png_img_hdr.width,
									png_device->png_img_hdr.height,
									png_device->png_img_hdr.bit_depth,
									png_device->png_img_hdr.color_type );
#endif
	}
	else
	{
		/* wait for end of previous chunk decoding */
		ret = PNG_SyncDecode( png_device );
		PNG_CHECK_CODE( ret != RET_OK, goto func_exit, "[PNG] %s : decode sync error\n", __F__ );
	}

	/* set decoder memory */
	read_base_addr = png_device->png_src_buf.addr + decode_param->offset;
	write_base_addr= png_device->png_out_stream.mem_buf.addr + png_device->png_out_stream.curr_offset;
	read_byte_size = decode_param->length;

#ifdef PNG_DEBUG
	PNG_PRINT("<0>read addr %p(%d), write addr %p(%d), length %d\n",
					read_base_addr, decode_param->offset,
					write_base_addr, png_device->png_out_stream.curr_offset, read_byte_size );
#endif


	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )
	{
		/* nop */
	}
	else
	{
		/* recaculate read/write base address based on VENC sram base
		 * raxis.lim (2010/11/2)
		 *
		 * read/write base address should be offset from the SRAM base of VENC.
		 * if read/write base address is located at the front of SRAM base, recaculated address shoule be
		 * wrapped around in the silkroad address range (2^29).
		 */
		if ( read_base_addr >= gpstVencMemConfig->uiSRAMBase )
		{
			read_base_addr  -= gpstVencMemConfig->uiSRAMBase;
		}
		else
		{
			read_base_addr += ( SLKRD_MAX_ADDRESS- gpstVencMemConfig->uiSRAMBase );
		}

		if ( write_base_addr >= gpstVencMemConfig->uiSRAMBase )
		{
			write_base_addr -= gpstVencMemConfig->uiSRAMBase;
		}
		else
		{
			write_base_addr += ( SLKRD_MAX_ADDRESS- gpstVencMemConfig->uiSRAMBase );
		}

#ifdef PNG_DEBUG
			PNG_PRINT("<1>read addr %p(%d), write addr %p(%d), length %d\n",
							read_base_addr, decode_param->offset,
							write_base_addr, png_device->png_out_stream.curr_offset, read_byte_size );
#endif
		/* 0x6000_0000(adec,gpu), 0x4000_0000(venc), 0x2000_0000(gfx) is added for MCU to access SLK0 */
		read_base_addr  |= 0x20000000;
		write_base_addr |= 0x20000000;
	}

	PNG_REG_SetReadBaseAddress( read_base_addr );
	PNG_REG_SetWriteBaseAddress( write_base_addr );
	PNG_REG_SetReadByteSzie( read_byte_size );
	PNG_REG_SetDMAReadValue( 0, 0, 0, 0);
	PNG_REG_SetDMAWriteValue( 0, 0, 0, 0);
	PNG_REG_SetDMAReadValueWin( 1, 0, 0, 0);
	PNG_REG_SetDMAWriteValueWin( 1, 0, 0, 0);
	PNG_REG_SetDMAReadValueFilter( 2, 0, 0, 0);
	PNG_REG_SetDMAWriteValueFilter( 2, 0, 0, 0);
	PNG_REG_SetReadByteDrop( 0, 0 );

	/* base address of working memory of decoder
	 * Youngwoo.Jin (2011/04/28)
	 *
	 * base address of working memory must be larger than read base memory
	 */
	PNG_REG_SetReadBaseAddressWin( png_device->png_src_buf.addr + png_device->png_src_buf.length );
	PNG_REG_SetWriteBaseAddressWin( png_device->png_src_buf.addr + png_device->png_src_buf.length );
	PNG_REG_SetReadBaseAddressFilter( png_device->png_src_buf.addr + png_device->png_src_buf.length + PNG_WINDOW_BUFF_SIZE );
	PNG_REG_SetWriteBaseAddressFilter( png_device->png_src_buf.addr + png_device->png_src_buf.length + PNG_WINDOW_BUFF_SIZE );

#ifdef PNG_DEBUG
	PNG_PRINT("read addr %p write addr %p size %d\n", read_base_addr, write_base_addr, read_byte_size );

	OS_HexDumpEx ( 0x0, png_device->png_src_buf_virt.addr, 32, NULL );
#endif

	if ( png_device->png_img_chunk_seqno == 0 )
	{
#ifdef PNG_DEBUG
		PNG_PRINT("png decode start\n");
#endif
		PNG_REG_Start();
	}
	else
	{
#ifdef PNG_DEBUG
		PNG_PRINT("png decode mid start\n");
#endif

		PNG_REG_MidStart();
	}

	/* increment seqno to accept the next chunk data */
	png_device->png_img_chunk_seqno++;

	/* record the start time of decode */
	png_device->png_decode_start_usec = OS_GetUsecTicks();

	ret = RET_OK; /* all work done */

func_exit:
	PNG_TRACE_END();
	return ret;
}

/** terminate decode
 *	this function just wait until deocode stops
 *
 */
int		PNG_DecodeStop	( PNG_DEVICE_T* png_device )
{
	int	ret;

	UINT32	decode_size;
	UINT32	width, height, bit_depth, color_type;
	UINT32	color_mul_factor;

	PNG_TRACE_BEGIN();
	ret = PNG_SyncDecode( png_device );
	PNG_CHECK_CODE( ret != RET_OK, goto func_exit, "[PNG] %s : decode sync error\n", __F__ );

	/* calculate decode size */
	width	 	= png_device->png_img_hdr.width;
	height		= png_device->png_img_hdr.height;
	color_type	= png_device->png_img_hdr.color_type;
	bit_depth	= png_device->png_img_hdr.bit_depth;

	switch( color_type )
	{
		case 0x0: color_mul_factor = 1; break;	/* gray scale */
		case 0x2: color_mul_factor = 3; break;	/* true color */
		case 0x3: color_mul_factor = 1; break;	/* index color */
		case 0x4: color_mul_factor = 2; break;	/* index color with alpha */
		case 0x6: color_mul_factor = 4; break;	/* true color with alpha */
		default:  color_mul_factor = 0; break;	/* ??? */
	}

	decode_size = width * height * color_mul_factor;

	switch ( bit_depth )
	{
		case 1: decode_size >>= 3; break;
		case 2: decode_size >>= 2; break;
		case 4: decode_size >>= 1; break;
		case 16:decode_size <<= 1; break;
		case 8: default: /*empty*/ break;
	}

	PNG_CHECK_CODE( decode_size == 0, goto func_exit,
					"[PNG] %s : decode_size zero error. w %d, h %d, c %d, b %d\n", __F__,
						width, height, color_type, bit_depth );

	/* proceed output stream buffer */
	png_device->png_out_stream.curr_offset += decode_size;

func_exit:
	PNG_SwReset( png_device );
	PNG_TRACE_END();
	return ret;
}

/**	wait until png decode done.
 *	this function should calculate decode size of decompression
 *
 */
static	int		PNG_SyncDecode	( PNG_DEVICE_T* png_device )
{
	UINT32	recv_event;

	int		ret = RET_ERROR;

	PNG_TRACE_BEGIN();

	/* wait event with max timeout 100 msec ( for test, I will increse timeout value to 500 msec ) */
	ret = OS_RecvEvent( &png_device->png_event, PNG_EVENT_DECODE_DONE, &recv_event, OS_EVENT_RECEIVE_ANY, PNG_SYNC_TIMEOUT /* 100msec */ );
	if ( ret != RET_OK ){ goto func_exit;}

	png_device->png_decode_end_usec = OS_GetUsecTicks();

	/* report decode time info */
#ifdef PNG_DEBUG
	PNG_PRINT("[PNG] %s : total decode time = %d\n", __F__, (int)png_device->png_decode_end_usec - (int)png_device->png_decode_start_usec );
#endif

	png_device->png_decode_start_usec 	= 0;
	png_device->png_decode_end_usec 	= 0;

	ret = RET_OK;	/* all work done */
func_exit:
	PNG_PRINT( "%s : %d * %d\n", ((ret==RET_OK)? "OK" : "ERROR"), png_device->png_img_hdr.width, png_device->png_img_hdr.height );
	PNG_TRACE_END();
	return ret;
}

/** ISR handler of PNG decoder
 *	@note it's called from ISR of venc
 */
irqreturn_t PNG_ISRHandler(int irq, void *dev_id, struct pt_regs *regs)
{
	BOOLEAN	decode_done = FALSE;
	UINT32 ui32Interrupts;

	/* read PNG interrupt status */
	ui32Interrupts = PNG_REG_GetInterruptStatus();
#ifdef PNG_DEBUG
	PNG_PRINT("png intr status 0x%x\n", ui32Interrupts );
#endif

	decode_done = ui32Interrupts& 0x0001;

	/* when png decode is done, 'decode done' event */
	if ( decode_done )
	{
		OS_SendEvent( &g_png_device[0].png_event, PNG_EVENT_DECODE_DONE );
	}

	/* clear PNG interrupt */
	PNG_REG_ClearInterrupt( ui32Interrupts );

    return IRQ_HANDLED;
}

/** this function is called from VENC
 *	since PNG is sub module in VENC hardware block, PNG should be initialized after VENC.
 *
 */
void	PNG_NotifyHostInitDone(void)
{
	g_host_init_done = TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef KDRV_GLOBAL_LINK
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP)
user_initcall_grp("kdrv",PNG_Init);
#else
module_init(PNG_Init);
#endif
module_exit(PNG_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("base driver");
MODULE_LICENSE("LGE");
#endif

/** @} */

