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

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "base_types.h"

//#include "de_def.h"
//#include "de_ipc_def.h"
//#include "frc_cfg.h"
#include "frc_kapi.h"
//#include "gpio/gpio_cli_def.h"
#include "cli/cli_arg.h"
//#include "de/de_cli.h"
//#include "dbg/dbg_reg.h"

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
int DDI_FRC_Ioctl(unsigned int cmd, UINT32 i_uiArg)
{
	int iResult;
	int iFd;

	iFd = open("/dev/lg/frc0", O_RDWR);
	if( iFd == -1)
	{
		printf("ERROR(%s)- Could not open device: frc\n", __FUNCTION__);
		return RET_ERROR;
	}
	iResult = ioctl(iFd, cmd, i_uiArg);
	close(iFd);

	return RET_OK;
}

int CLI_FRC_IO_INIT(int argc ,char **argv)
{
	int ret_val = RET_ERROR;

	ret_val = DDI_FRC_Ioctl(FRC_IO_INIT, 0); 	

	return ret_val;
}

int CLI_FRC_IO_RESET_SW(int argc ,char **argv)
{
	int ret_val = RET_ERROR;

	ret_val = DDI_FRC_Ioctl(FRC_IO_RESET_SW, 0); 	

	return ret_val;
}

int CLI_FRC_IOR_IS_STABLE(int argc ,char **argv)
{
	UINT32					nArg = START_ARGS;
	LX_FRC_STABLE_T			stable_status;
	int						ret_val = RET_ERROR;

	ret_val = DDI_FRC_Ioctl(FRC_IOR_IS_STABLE, (UINT32)&stable_status); 	

	printf("stable_status = %d\n", stable_status);

	return ret_val;
}

int CLI_FRC_IOW_SET_MEMC(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	BOOLEAN	bEnable;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <bEnable>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	bEnable = (BOOLEAN)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_MEMC, (UINT32)&bEnable); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_MEMC_TYPE(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	LX_FRC_MC_TYPE_T	mc_type;
	int					ret_val = RET_ERROR;
	UINT32	ui32Number;
	LX_FRC_MC_LEVEL_T	mc_level;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <mc_level>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	mc_level = (LX_FRC_MC_LEVEL_T)ui32Number;

	mc_type.blurLevel = 1;
	mc_type.judderLevel = 2;
	mc_type.eFrcMCLevel = mc_level; 

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_MEMC_TYPE, (UINT32)&mc_type); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_TRUEMOTION_DEMO(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	BOOLEAN	bEnable;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <bEnable>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	bEnable = (BOOLEAN)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_TRUEMOTION_DEMO, (UINT32)&bEnable); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_TRUEMOTION_DEMO_BAR(int argc ,char **argv)
{
	UINT32					nArg = START_ARGS;
	LX_FRC_DEMO_BAR_CTRL_T	demo_bar_ctrl;
	int						ret_val = RET_ERROR;

	demo_bar_ctrl.bEnable = 1;
	demo_bar_ctrl.rBar = 15;
	demo_bar_ctrl.gBar = 0;
	demo_bar_ctrl.bBar = 0;
	demo_bar_ctrl.thick = 30;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_TRUEMOTION_DEMO_BAR, (UINT32)&demo_bar_ctrl); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_SCAN_DIRECTION(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	BOOLEAN	bDirection;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <bDirection>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	bDirection = (BOOLEAN)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_SCAN_DIRECTION, (UINT32)&bDirection); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_FREEZE(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	BOOLEAN	bEnable;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <bEnable>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	bEnable = (BOOLEAN)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_FREEZE, (UINT32)&bEnable); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_OSD_CONTROL(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	LX_FRC_OSD_CTRL_T	osd_ctrl;
	int					ret_val = RET_ERROR;
	UINT32	ui32Number;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <test_case number>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;

	osd_ctrl.bOnOff = 1;
	osd_ctrl.eOSDRegion = 2;
	osd_ctrl.tOSDRect.u16Hsize = 300; 
	osd_ctrl.tOSDRect.u16Vsize = 400; 
	osd_ctrl.tOSDRect.u16Hstart = 1600; 
	osd_ctrl.tOSDRect.u16Vstart = 550; 

	if(ui32Number == 1)
	{
		osd_ctrl.bOnOff = 1;
		osd_ctrl.eOSDRegion = 2;
		osd_ctrl.tOSDRect.u16Hsize = 1000; 
		osd_ctrl.tOSDRect.u16Vsize = 1000; 
		osd_ctrl.tOSDRect.u16Hstart = 0; 
		osd_ctrl.tOSDRect.u16Vstart = 0; 
	}

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_OSD_CONTROL, (UINT32)&osd_ctrl); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_SPREAD_SPECTRUM(int argc ,char **argv)
{
	UINT32						nArg = START_ARGS;
	LX_FRC_SPREAD_SPECTRUM_T	spread_spectrum;
	int							ret_val = RET_ERROR;

	spread_spectrum.u16Period = 1;
	spread_spectrum.u16Percent = 2;
	spread_spectrum.bEnable = 0; 
	spread_spectrum.bMiuLVDS = 1; 

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_SPREAD_SPECTRUM, (UINT32)&spread_spectrum); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_PWM_DUTY_CYCLE(int argc ,char **argv)
{
	UINT32					nArg = START_ARGS;
	LX_FRC_PWM_PARAM_T		pwm_param;
	int						ret_val = RET_ERROR;

	pwm_param.ePWMIndex = 3;
	pwm_param.u16Duty = 2;
	pwm_param.u16StartPosition = 1; 

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_PWM_DUTY_CYCLE, (UINT32)&pwm_param); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_PWM_CONTROL(int argc ,char **argv)
{
	UINT32					nArg = START_ARGS;
	LX_FRC_PWM_CTRL_T		pwm_ctrl;
	int						ret_val = RET_ERROR;

	pwm_ctrl.ePWMIndex = 3;
	pwm_ctrl.ePWMOutput = 2;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_PWM_CONTROL, (UINT32)&pwm_ctrl); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_LOW_DELAY_MODE(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	LX_FRC_LOW_DELAY_T 	low_delay;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <low_delay>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	low_delay = (LX_FRC_LOW_DELAY_T)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_LOW_DELAY_MODE, (UINT32)&low_delay); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_DISPLAY_OUTPUT(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	BOOLEAN	bEnable;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <bEnable>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	bEnable = (BOOLEAN)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_DISPLAY_OUTPUT, (UINT32)&bEnable); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_FHD_MODE(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	LX_FRC_FHD_MODE_T 	fhd_mode;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <fhd_mode>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	fhd_mode = (LX_FRC_FHD_MODE_T)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_FHD_MODE, (UINT32)&fhd_mode); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_RGBYUV_MODE(int argc ,char **argv)
{
	UINT32	nArg = START_ARGS;
	UINT32	ui32Number;
	LX_FRC_RGBYUV_MODE_T 	rgbyuv_mode;
	int ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <rgbyuv_mode>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	rgbyuv_mode = (LX_FRC_RGBYUV_MODE_T)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_RGBYUV_MODE, (UINT32)&rgbyuv_mode); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_JUDDER_LEVEL(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	UINT32				ui32Number;
	UINT8				judder_level;
	int					ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <judder_level>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	judder_level = (UINT8)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_JUDDER_LEVEL, (UINT32)&judder_level); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_BLUR_LEVEL(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	UINT32				ui32Number;
	UINT8				blur_level;
	int					ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <blur_level>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	blur_level = (UINT8)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_BLUR_LEVEL, (UINT32)&blur_level); 	

	return ret_val;
}

int CLI_FRC_IOR_REG_RD(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	UINT32				ui32Number;
	UINT32				rd_addr_data;
	int					ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <rd_addr>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	rd_addr_data = (UINT32)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOR_REG_RD, (UINT32)&rd_addr_data); 	

	printf("rd_data = 0x%x\n", rd_addr_data);

	return ret_val;
}

int CLI_FRC_IOW_REG_WR(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	UINT32				ui32Number;
	LX_FRC_REG_WRITE_T	write_struct;
	int					ret_val = RET_ERROR;

	if (argc < 3 || !strcmp(argv[1], "-help"))
	{
		printf("%s <write_addr> <write_data\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	write_struct.wr_addr = (UINT32)ui32Number;
	ui32Number = A2U;
	write_struct.wr_data = (UINT32)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_REG_WR, (UINT32)&write_struct); 	

	return ret_val;
}

#if 0
int CLI_FRC_IOW_SET_DISPLAY_MUTE(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	LX_FRC_MUTE_CTRL_T	mute_ctrl;
	int					ret_val = RET_ERROR;

	mute_ctrl.bEnable = 1;
	mute_ctrl.eFrcMute = 2;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_DISPLAY_MUTE, (UINT32)&mute_ctrl); 	

	return ret_val;
}

int CLI_FRC_IOW_SET_3D_MODE(int argc ,char **argv)
{
	UINT32				nArg = START_ARGS;
	UINT32				ui32Number;
	LX_FRC_3D_MODE_T	mode_3d;
	int					ret_val = RET_ERROR;

	if (argc < 2 || !strcmp(argv[1], "-help"))
	{
		printf("%s <mode_3d>\n", argv[0]);
		return RET_ERROR;
	}

	ui32Number = A2U;
	mode_3d = (LX_FRC_3D_MODE_T)ui32Number;

	ret_val = DDI_FRC_Ioctl(FRC_IOW_SET_3D_MODE, (UINT32)&mode_3d); 	

	return ret_val;
}
#endif


/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int CLI_FRC_Start(void)
{
	Add_CLI_Cmd("frc_init", CLI_FRC_IO_INIT);
	Add_CLI_Cmd("frc_reset_sw", CLI_FRC_IO_RESET_SW);
	Add_CLI_Cmd("frc_is_stable", CLI_FRC_IOR_IS_STABLE);
	Add_CLI_Cmd("frc_set_memc", CLI_FRC_IOW_SET_MEMC);
	Add_CLI_Cmd("frc_set_memc_type", CLI_FRC_IOW_SET_MEMC_TYPE);
	Add_CLI_Cmd("frc_set_truemotion_demo", CLI_FRC_IOW_SET_TRUEMOTION_DEMO);
	Add_CLI_Cmd("frc_set_truemotion_demo_bar", CLI_FRC_IOW_SET_TRUEMOTION_DEMO_BAR);
	Add_CLI_Cmd("frc_set_scan_direction", CLI_FRC_IOW_SET_SCAN_DIRECTION);
	Add_CLI_Cmd("frc_set_freeze", CLI_FRC_IOW_SET_FREEZE);
	Add_CLI_Cmd("frc_set_osd_control", CLI_FRC_IOW_SET_OSD_CONTROL);
	Add_CLI_Cmd("frc_set_spread_spectrum", CLI_FRC_IOW_SET_SPREAD_SPECTRUM);
	Add_CLI_Cmd("frc_set_pwm_duty_cycle", CLI_FRC_IOW_SET_PWM_DUTY_CYCLE);
	Add_CLI_Cmd("frc_set_pwm_control", CLI_FRC_IOW_SET_PWM_CONTROL);
	Add_CLI_Cmd("frc_set_low_delay_mode", CLI_FRC_IOW_SET_LOW_DELAY_MODE);
	Add_CLI_Cmd("frc_set_display_output", CLI_FRC_IOW_SET_DISPLAY_OUTPUT);
	Add_CLI_Cmd("frc_set_fhd_mode", CLI_FRC_IOW_SET_FHD_MODE);
	Add_CLI_Cmd("frc_set_rgbyuv_mode", CLI_FRC_IOW_SET_RGBYUV_MODE);
	Add_CLI_Cmd("frc_set_judder_level", CLI_FRC_IOW_SET_JUDDER_LEVEL);
	Add_CLI_Cmd("frc_set_blur_level", CLI_FRC_IOW_SET_BLUR_LEVEL);
	Add_CLI_Cmd("frc_reg_rd", CLI_FRC_IOR_REG_RD);
	Add_CLI_Cmd("frc_reg_wr", CLI_FRC_IOW_REG_WR);
//	Add_CLI_Cmd("frc_set_display_mute", CLI_FRC_IOW_SET_DISPLAY_MUTE);
//	Add_CLI_Cmd("frc_set_3d_mode", CLI_FRC_IOW_SET_3D_MODE);

	return RET_OK;
}

/**  @} */
