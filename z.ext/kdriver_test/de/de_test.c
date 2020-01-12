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
 * date       2010.02.22
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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#include "base_types.h"

#include "cli/cli_arg.h"
#include "de_kapi.h"
#include "de_def.h"

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
//static	pthread_t	stVideoCpuSendThread;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int CLI_DE_Test(int argc ,char **argv)
{
	int	ret;

	{
		LX_DE_REG_WR_T lReg;
		UINT32 io_data;

		lReg.wr_addr = 0x14;
		ffprintf("aaaaa %s\n", __FUNCTION__);
		io_data = lReg.wr_addr;
		ret = DE_DEV_RegRd( &io_data );
		lReg.wr_data = io_data;
ffprintf("aaaaa lReg.wr_addr = 0x%08x\n", (UINT32)lReg.wr_addr);
ffprintf("aaaaa lReg.wr_data = 0x%08x\n", (UINT32)lReg.wr_data);

		ffprintf("aaaaa %s\n", __FUNCTION__);
		lReg.wr_addr = 0x14;
		ret = DE_DEV_RegWr( &lReg );
ffprintf("aaaaa lReg.wr_addr = 0x%08x\n", (UINT32)lReg.wr_addr);
ffprintf("aaaaa lReg.wr_data = 0x%08x\n", (UINT32)lReg.wr_data);
	}

	{
		LX_DE_PANEL_TYPE_T panel_type;
		panel_type = LX_PANEL_TYPE_1920;
		ffprintf("aaaaa %s\n", __FUNCTION__);
		ret = DE_DEV_Init( &panel_type );
		ffprintf("aaaaa %s\n", __FUNCTION__);
	}

	{
		LX_DE_FIRMWARE_INFO_T rev_info = {0x12345678, {0x1234, 0x5678, 0x9abc}};
		ffprintf("aaaaa %s\n", __FUNCTION__);
		ret = DE_DEV_GetFirmwareInfo( &rev_info );
		ffprintf("aaaaa de firmware info (%0X, %d:%d:%d)\n", rev_info.version, rev_info.date.year, rev_info.date.month, rev_info.date.day );
	}

	{
		LX_DE_PANEL_TYPE_T panel_type;
		panel_type = LX_PANEL_TYPE_1920;
		ffprintf("aaaaa %s\n", __FUNCTION__);
		ret = DE_DEV_Init( &panel_type );
		ffprintf("aaaaa %s\n", __FUNCTION__);
	}

	{
		LX_DE_FIRMWARE_INFO_T rev_info = {0x12345678, {0x1234, 0x5678, 0x9abc}};
		ffprintf("aaaaa %s\n", __FUNCTION__);
		ret = DE_DEV_GetFirmwareInfo( &rev_info );
		ffprintf("aaaaa de firmware info (%0X, %d:%d:%d)\n", rev_info.version, rev_info.date.year, rev_info.date.month, rev_info.date.day );
	}

	{
		LX_DE_PANEL_TYPE_T panel_type;
		panel_type = LX_PANEL_TYPE_1920;
		ffprintf("aaaaa %s\n", __FUNCTION__);
		ret = DE_DEV_Init( &panel_type );
		ffprintf("aaaaa %s\n", __FUNCTION__);
	}

	{
		LX_DE_PANEL_TYPE_T panel_type;
		panel_type = LX_PANEL_TYPE_1920;
		ffprintf("aaaaa %s\n", __FUNCTION__);
		ret = DE_DEV_Init( &panel_type );
		ffprintf("aaaaa %s\n", __FUNCTION__);
	}

	{
		LX_DE_FIRMWARE_INFO_T rev_info = {0x12345678, {0x1234, 0x5678, 0x9abc}};
		ffprintf("aaaaa %s\n", __FUNCTION__);
		ret = DE_DEV_GetFirmwareInfo( &rev_info );
		ffprintf("aaaaa de firmware info (%0X, %d:%d:%d)\n", rev_info.version, rev_info.date.year, rev_info.date.month, rev_info.date.day );
	}

	//{
	//    LX_DE_FR_RATE_T stParams;
	//    ffprintf("aaaaa %s\n", __FUNCTION__);
	//    ret = ioctl( de_fd, DE_IOR_GET_OUT_FR_RATE, &stParams );
	//}

	//{
	//    LX_DE_COLOR_T bgColor = {0xab, 0xbc, 0xde};
	//    ffprintf("aaaaa %s\n", __FUNCTION__);
	//    ret = ioctl( de_fd, DE_IOW_SET_BG_COLOR, &bgColor );
	//}

	//{
	//    LX_DE_SET_INPUT_T stParams;
	//    stParams.win_id   = LX_DE_WIN_SUB;
	//    stParams.inputSrc = LX_MVI_VDEC;
	//    ffprintf("aaaaa %s\n", __FUNCTION__);
	//    ret = ioctl(de_fd, DE_IOW_SET_INPUT_SRC, &stParams);
	//}

	//{
	//    LX_DE_CVI_SRC_TYPE_T stParams;
	//    stParams.cvi_channel		 = LX_DE_CVI_CH_A;
	//    stParams.size_offset.hsize	 = 1920;
	//    stParams.size_offset.vsize	 = 1080;
	//    stParams.size_offset.hoffset = 0;
	//    stParams.size_offset.voffset = 0;
	//    stParams.color				 = LX_DE_COLOR_709;
	//    stParams.aspect				 = LX_DE_AR_16_9;
	//    stParams.scan				 = LX_DE_INTERLACED;
	//    ffprintf("aaaaa size = %d\n", sizeof(stParams));
	//    ret = ioctl(de_fd, DE_IOW_SET_CVI_SRC_TYPE, &stParams);
	//}

	//{
	//    LX_DE_GRAB_PIXEL_T stParams;
	//    stParams.region.win_id = LX_DE_WIN_MAIN;

	//    stParams.region.pixelGrabX = 0;
	//    stParams.region.pixelGrabY = 0;
	//    stParams.region.pixelGrabW = 10;
	//    stParams.region.pixelGrabH = 10;
	//    stParams.region.realPixelGrabW = 2048;
	//    stParams.region.realPixelGrabH = 1200;
	//    stParams.region.colorSpace = 0;

	//    ffprintf("aaaaa %s\n", __FUNCTION__);
	//    ret = ioctl(de_fd, DE_IOWR_GRAB_PIXEL, &stParams);
	//}

	return 0;
}

int CLI_DE_SetDispWin(int argc ,char **argv)
{
	int				ret;

	LX_DE_WIN_DIMENSION_T stParams;
	UINT16 w, h, x, y;

	stParams.win_id   = LX_DE_WIN_MAIN;

	w = 1920;
	do {
		h   = (w * 1080 / 1920);
		h >>= 2;
		h <<= 2;

		x = 1920 - w;
		x >>= 3;
		x <<= 2;

		y = 1080 - h;
		y >>= 3;
		y <<= 2;

		stParams.rect.w = w;
		stParams.rect.h = h;
		stParams.rect.x = x;
		stParams.rect.y = y;
		ffprintf("w(%4d), h(%4d), x(%4d), y(%4d)\n", w, h, x, y);
		ret = DE_DEV_SetOutWin( &stParams );
		sleep(2);
		w -= 16;
	} while (w>320);

	return 0;
}

/**  @} */
