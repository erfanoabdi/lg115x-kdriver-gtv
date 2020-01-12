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
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     jaemo.kim (jaemo.kim@lge.com)
 * version    1.0
 * date       2010.07.16
 * note       Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include "os_util.h"
#include "proc_util.h"
#include "debug_util.h"

#include "de_model.h"
#include "de_kapi.h"
#include "de_def.h"
#include "de_ver_def.h"

#include "l8/de_ipc_def_l8.h"
#include "de_int_def.h"
#include "de_hal_def.h"
#include "de_fw_def.h"
#include "de_fw.h"
#include "l8/de_reg_def_l8.h"
#include "l8/de_reg_l8.h"
#undef USE_DE_FIRMWARE_DOWNLOAD_FROM_KENEL
#ifdef USE_DE_FIRMWARE_DOWNLOAD_FROM_KENEL
#include "l8/de_fw_l8a0.h"
#include "l8/de_fw_l8b0.h"
#endif
/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
#ifdef USE_DE_FIRMWARE_DWONLOAD_IN_DRIVER
#ifdef USE_DE_FIRMWARE_DOWNLOAD_FROM_FILE
static DE_FW_FILE_T sDeFirmwareFromFile[] = {
/* [L8] minjun.kim 2010-09-28 아래 값은 수정하지 마시요 */
	{ 1, "/mnt/nsu/base/res/lglib/kdrv/firmware/led_l8_ddr.bin"},
	{ 2, "/mnt/nsu/base/res/lglib/kdrv/firmware/video_asc.bin"},
	{ 3, "/mnt/nsu/base/res/lglib/kdrv/firmware/video_ddr.bin"},
};
#endif
#ifdef USE_DE_FIRMWARE_DOWNLOAD_FROM_KENEL
LX_DE_FW_DWLD_T sDeFirmwareFromHeader[] = {
	{ 1, sizeof(gLedFirmwareInDDR), (char *)gLedFirmwareInDDR },
	{ 2, sizeof(gDeFirmwareInRAM),	(char *)gDeFirmwareInRAM  },
	{ 3, sizeof(gDeFirmwareInDDR),	(char *)gDeFirmwareInDDR  }
};
LX_DE_FW_DWLD_T sDeFirmwareFromHeader_L8B0[] = {
	{ 1, sizeof(gLedFirmwareInDDR_L8B0), (char *)gLedFirmwareInDDR_L8B0 },
	{ 2, sizeof(gDeFirmwareInRAM_L8B0),	(char *)gDeFirmwareInRAM_L8B0  },
	{ 3, sizeof(gDeFirmwareInDDR_L8B0),	(char *)gDeFirmwareInDDR_L8B0  }
};
#endif
#endif

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/

int DE_FW_L8_DownloadFromBin(void)
{
	int ret;
	int i;

#ifdef USE_DE_FIRMWARE_DOWNLOAD_FROM_FILE
	for (i=0;i<ARRAY_SIZE(sDeFirmwareFromFile);i++) {
		ret = DE_FW_DownloadFromFile(&sDeFirmwareFromFile[i]);
		if (ret) break;
	}
#endif

	return ret;
}

int DE_FW_L8_DownloadFromHead(void)
{
	int ret = RET_OK;
#ifdef USE_DE_FIRMWARE_DOWNLOAD_FROM_KENEL
	int i;
	LX_DE_FW_DWLD_T *lpDeFirmwareFromHeader = NULL;

	if (lx_chip_rev() >= LX_CHIP_REV(L8, B0))
		lpDeFirmwareFromHeader = sDeFirmwareFromHeader_L8B0;
	else
		lpDeFirmwareFromHeader = sDeFirmwareFromHeader;

	for (i=0;i<ARRAY_SIZE(sDeFirmwareFromHeader);i++) {
		ret = DE_REG_FW_Download(&lpDeFirmwareFromHeader[i]);
		if (ret) break;
	}
#endif

	return ret;
}

/**  @} */
