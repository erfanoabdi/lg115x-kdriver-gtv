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
 ***************************************************************************************/

/** @file
 *
 *  main driver implementation for se device.
 *	se device will teach you how to make device driver with new platform.
 *
 *  author		Stan.Kim
 *  version		1.0
 *  date		2012.02
 *  note		Additional information.
 *
 *  @addtogroup lg_se
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define SE_USE_UNLOCKED_IOCTL

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/rmap.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>

#include "os_util.h"
#include "base_dev_cfg.h"

#include "se_drv.h"
#include "l9/se_l9_drv.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define SE_MODULE	"se"

/* Module param for Debug print */
#define SE_DEBUG_OFF	0x00
#define SE_DEBUG_PRINT	0x01
#define SE_DEBUG_TRACE	0x02
#define SE_DEBUG_WARN	0x04
#define SE_DEBUG_ERROR	0x08
#define SE_DEBUG_PROC	0x10
#define SE_DEBUG_ASSERT	0x20
#define SE_DEBUG_SYSFS	0x40
#define SE_DEBUG_DEVFS	0x80

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int g_se_debug_fd	= -1;
int g_se_trace_depth = 0;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static int _SE_Open(struct inode *inode, struct file *filp);
static int _SE_Close(struct inode *inode, struct file *filp);
static int _SE_MMAP(struct file *file, struct vm_area_struct *vma);
static int _SE_IOCTL(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static long _SE_UNLOCKED_IOCTL(struct file *file, unsigned int cmd, unsigned long arg);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static int gSE_DbgLvl = SE_DEBUG_DEVFS|SE_DEBUG_ERROR;

module_param(gSE_DbgLvl, int, S_IWUSR);
MODULE_PARM_DESC(gSE_DbgLvl, "Debugging Level");

static SE_T _gSE;

static char *_error_failed_in_getting_data = "Failed in getting data";
static char *_error_failed_in_setting_data = "Failed in setting data";
static char *_error_failed_in_operation = "Failed in operation";

static dev_t _gDevNum;
static struct cdev _gCDev;
static struct file_operations _gFileOperations = {
	.open 	= _SE_Open,
	.release= _SE_Close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SE_USE_UNLOCKED_IOCTL)
	.ioctl	= _SE_IOCTL,
#else
	.unlocked_ioctl = _SE_UNLOCKED_IOCTL,
#endif
	.mmap	= _SE_MMAP,
	.read	= NULL,
	.write	= NULL,
};

static OS_SEM_T	_gMutexOperation;

/*========================================================================================
    Implementation Group : SE CDEV
========================================================================================*/
static int _ioctl_LockDebug(unsigned long arg)
{
	if (_gSE.LockDebug((LX_SE_DEBUG_T)arg)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_UnlockDebug(unsigned long arg)
{
	if (_gSE.UnlockDebug((LX_SE_DEBUG_T)arg)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_Reset(unsigned long arg)
{
	if (_gSE.Reset()!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_GetMemInfo(unsigned long arg)
{
	LX_SE_MEM_INFO_T mem = {0};

	mem.physicalBase = (UINT32)_gSE.bufferAddr;
	mem.size = (UINT32)_gSE.bufferSize;

	if (copy_to_user((void __user *)arg , (void*)&mem , sizeof(LX_SE_MEM_INFO_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	return 0;
}

static int _ioctl_GetRandomNR(unsigned long arg)
{
	LX_SE_DATA_T randomNR = {0};

	if (copy_from_user((void *)&randomNR, (void __user *)arg, sizeof(LX_SE_DATA_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.GetRandomNR(&randomNR)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_SetKey(unsigned long arg)
{
	LX_SE_DATA_T key = {0};

	if (copy_from_user((void *)&key , (void __user *)arg , sizeof(LX_SE_DATA_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.SetKey(&key)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_SetIV(unsigned long arg)
{
	LX_SE_DATA_T iv = {0};

	if (copy_from_user((void *)&iv, (void __user *)arg , sizeof(LX_SE_DATA_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.SetIV(&iv)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_BlockOperation(unsigned long arg)
{
	LX_SE_BLOCK_OP_T request = {0};
	SE_RETURN_T res = SE_SUCCESS;

	if (copy_from_user((void *)&request, (void __user *)arg , sizeof(LX_SE_BLOCK_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.RunBlockOp(&request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	if (res != SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_HashOperation(unsigned long arg)
{
	LX_SE_HASH_OP_T request = {0};

	if (copy_from_user((void *)&request, (void __user *)arg , sizeof(LX_SE_HASH_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.RunHashOp(&request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_RsaOperation(unsigned long arg)
{
	LX_SE_RSA_OP_T request = {0};

	if (copy_from_user((void *)&request, (void __user *)arg, sizeof(LX_SE_RSA_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.RunRSAOp(&request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	if (copy_to_user((void __user *)arg, (void*)&request, sizeof(LX_SE_RSA_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_setting_data);
		return -EIO;
	}

	return 0;
}

static int _ioctl_SetSecureData(unsigned long arg)
{
	LX_SE_SECURE_DATA_T request = {0};

	if (copy_from_user((void *)&request, (void __user *)arg, sizeof(LX_SE_SECURE_DATA_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.SetSecureData(&request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	if (copy_to_user((void __user *)arg, (void*)&request, sizeof(LX_SE_SECURE_DATA_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_setting_data);
		return -EIO;
	}

	return 0;
}

static int _ioctl_GetSecureData(unsigned long arg)
{
	LX_SE_SECURE_DATA_T request = {0};

	if (copy_from_user((void *)&request , (void __user *)arg , sizeof(LX_SE_SECURE_DATA_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.GetSecureData(&request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_TransformSecureData(unsigned long arg)
{
	LX_SE_SECURE_DATA_T request[2] = {{0},};

	if (copy_from_user((void *)request , (void __user *)arg , sizeof(LX_SE_SECURE_DATA_T)*2)!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.TransformSecureData(request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_SetVerification(unsigned long arg)
{
	if (_gSE.SetVerification()!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_GetVerification(unsigned long arg)
{
	UINT32 verificationInfo = 0;

	if (_gSE.GetVerification(&verificationInfo)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	if (copy_to_user((void __user *)arg , (void*)&verificationInfo, sizeof(unsigned int))!=0) {
		SE_ERROR("%s\n", _error_failed_in_setting_data);
		return -EIO;
	}

	return 0;
}

static int _ioctl_SetADC(unsigned long arg)
{
 	UINT8 request[LX_SE_ADC_SIZE] = {0,};

	if (copy_from_user((void *)request, (void __user *)arg, LX_SE_ADC_SIZE)!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.SetADC(request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	return 0;
}

static int _ioctl_GetADC(unsigned long arg)
{
	UINT8 adc[LX_SE_ADC_SIZE] = {0,};

	if (_gSE.GetADC(adc)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	if (copy_to_user((void __user *)arg, (void*)&adc, LX_SE_ADC_SIZE)!=0) {
		SE_ERROR("%s\n", _error_failed_in_setting_data);
		return -EIO;
	}

	return 0;
}

static int _ioctl_NF_DrmAgent(unsigned long arg)
{
	LX_SE_NF_OP_T request = {0};

	if (copy_from_user((void *)&request, (void __user *)arg, sizeof(LX_SE_NF_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.NF_DrmAgent(&request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	if (copy_to_user((void __user *)arg , (void*)&request, sizeof(LX_SE_NF_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_setting_data);
		return -EIO;
	}

	return 0;
}

static int _ioctl_TrustOperation(unsigned long arg)
{
	LX_SE_TRUST_OP_T request = {0};

	if (copy_from_user((void *)&request, (void __user *)arg, sizeof(LX_SE_TRUST_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_getting_data);
		return -EIO;
	}

	if (_gSE.TrustOperation(&request)!=SE_SUCCESS) {
		SE_ERROR("%s\n", _error_failed_in_operation);
		return -EIO;
	}

	if (copy_to_user((void __user *)arg , (void*)&request, sizeof(LX_SE_TRUST_OP_T))!=0) {
		SE_ERROR("%s\n", _error_failed_in_setting_data);
		return -EIO;
	}

	return 0;
}

static long _SE_UNLOCKED_IOCTL(struct file *file, unsigned int cmd, unsigned long arg)
{
	static int (*pfnIOCTLs[])(unsigned long arg) = {
	/* SE_IO_RESET						0x00 */  _ioctl_Reset,
	/* SE_IOR_GET_MEM_INFO				0x01 */  _ioctl_GetMemInfo,
	/* SE_IORW_GET_RANDOM_NUMBER		0x02 */  _ioctl_GetRandomNR,
	/* SE_IOW_SET_KEY					0x03 */  _ioctl_SetKey,
	/* SE_IOW_SET_IV					0x04 */  _ioctl_SetIV,
	/* SE_IORW_RUN_BLOCK_OP 			0x05 */  _ioctl_BlockOperation,
	/* SE_IORW_RUN_HASH_OP				0x06 */  _ioctl_HashOperation,
	/* SE_IORW_RUN_RSA_OP				0x07 */  _ioctl_RsaOperation,
	/* SE_IORW_GET_SECURE_DATA			0x08 */  _ioctl_GetSecureData,
	/* SE_IOW_SET_SECURE_DATA			0x09 */  _ioctl_SetSecureData,
	/* SE_IORW_TRANSFORM_SECURE_DATA	0x0A */  _ioctl_TransformSecureData,
	/* SE_IOW_LOCK_DEBUG				0x0B */  _ioctl_LockDebug,
	/* SE_IOW_UNLOCK_DEBUG				0x0C */  _ioctl_UnlockDebug,
	/* SE_IO_SET_VERIFICATION			0x0D */  _ioctl_SetVerification,
	/* SE_IOR_GET_VERIFICATION			0x0E */  _ioctl_GetVerification,
	/* SE_IOW_SET_ADC					0x0F */  _ioctl_SetADC,
	/* SE_IOR_GET_ADC					0x10 */  _ioctl_GetADC,
	/* SE_IORW_NF_DRMAGENT				0x11 */  _ioctl_NF_DrmAgent,
	/* SE_IORW_TRUST_OPERATION			0x12 */  _ioctl_TrustOperation,
	};
	int result;

	cmd = _IOC_NR(cmd) - _IOC_NR(SE_IO_RESET);
	if (cmd<sizeof(pfnIOCTLs)/sizeof(int*)) {
		if (pfnIOCTLs[cmd]!=NULL) {
			OS_LockMutex(&_gMutexOperation);
			result = pfnIOCTLs[cmd](arg);
			OS_UnlockMutex(&_gMutexOperation);
			return result;
		}

		SE_ERROR("Not implemented(%d)\n", cmd);
		return -ENOTTY;
	}

	SE_ERROR("Not defined(%d)\n", cmd);
	return -ENOTTY;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)) || !defined(SE_USE_UNLOCKED_IOCTL)
static int _SE_IOCTL(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	return (int)_SE_UNLOCKED_IOCTL(file, cmd, arg);
}
#endif

static int _SE_MMAP(struct file *file, struct vm_area_struct *vma)
{
	vma->vm_flags |= VM_IO;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	return io_remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
		vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static int _SE_Open(struct inode *inode, struct file *filp)
{
	if (_gSE.bInit==0) {
		if (_gSE.InitChip(_gSE.chip)!=SE_SUCCESS) {
			SE_ERROR("Not initialized!\n");
			return -EIO;
		}
		_gSE.bInit = 1;
	}

	return 0;
}

static int _SE_Close(struct inode *inode, struct file *filp)
{
	return 0;
}

/*========================================================================================
    Implementation Group : SE Module
========================================================================================*/

int SE_Init(void)
{
	/* Get the handle of debug output for se device.
	*
	* Most module should open debug handle before the real initialization of module.
	* As you know, debug_util offers 4 independent debug outputs for your device driver.
	* So if you want to use all the debug outputs, you should initialize each debug output
	* using OS_DEBUG_EnableModuleByIndex() function.
	*/
	g_se_debug_fd = DBG_OPEN(SE_MODULE);

	if ( g_se_debug_fd < 0 ) {
		SE_ERROR("can't register se device\n");
		return -1;
	}
	OS_DEBUG_EnableModule (g_se_debug_fd);

	if (gSE_DbgLvl & SE_DEBUG_PRINT)
		OS_DEBUG_EnableModuleByIndex(g_se_debug_fd, 0, DBG_COLOR_NONE);

	if (gSE_DbgLvl & SE_DEBUG_TRACE)
		OS_DEBUG_EnableModuleByIndex(g_se_debug_fd, 1, DBG_COLOR_CYAN);

	if (gSE_DbgLvl & SE_DEBUG_WARN)
		OS_DEBUG_EnableModuleByIndex(g_se_debug_fd, 2, DBG_COLOR_YELLOW);

	if (gSE_DbgLvl & SE_DEBUG_ERROR)
		OS_DEBUG_EnableModuleByIndex(g_se_debug_fd, 3, DBG_COLOR_RED);

	_gSE.chip = lx_chip_rev();
	_gSE.platform = lx_chip_plt();

	switch (_gSE.chip) {
		case LX_CHIP_REV(L9,A0):
		case LX_CHIP_REV(L9,A1):
		case LX_CHIP_REV(L9,B0):
		case LX_CHIP_REV(L9,B1):
			SE_L9_Init(&_gSE);
			break;
		default :
			return -EIO;	// Invalid
	}

	_gDevNum = MKDEV(SE_MAJOR, SE_MINOR);
	register_chrdev_region(_gDevNum, 1, SE_MODULE);
	cdev_init(&_gCDev, &_gFileOperations);
	if (cdev_add(&_gCDev, _gDevNum, 1)) {
		SE_ERROR("while adding se device (%d.%d)\n", MAJOR(_gDevNum), MINOR(_gDevNum));
		return -EIO;
	}
	OS_CreateDeviceClass(_gDevNum, "%s%d", SE_MODULE, 0);
	OS_InitMutex(&_gMutexOperation, OS_SEM_ATTR_DEFAULT);

	return 0;
}

void SE_Cleanup(void)
{
	switch (lx_chip_rev()) {
		case LX_CHIP_REV(L9,A0):
		case LX_CHIP_REV(L9,A1):
		case LX_CHIP_REV(L9,B0):
		case LX_CHIP_REV(L9,B1):
			SE_L9_Exit(&_gSE);
			break;
		default :
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(KDRV_GLOBAL_LINK) || defined(MAKE_SE_MODULE)
#if defined(CONFIG_LG_BUILTIN_KDRIVER) && defined(CONFIG_LGSNAP) && !defined(MAKE_SE_MODULE)
user_initcall_grp("kdrv",SE_Init);
#else
module_init(SE_Init);
#endif
module_exit(SE_Cleanup);

MODULE_AUTHOR("LGE");
MODULE_DESCRIPTION("SE driver");
MODULE_LICENSE("proprietary");
#endif
/** @} */

