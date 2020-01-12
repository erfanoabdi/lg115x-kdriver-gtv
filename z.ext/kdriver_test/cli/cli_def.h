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
 * date       2010.03.31
 * note       Additional information.
 *
 * @addtogroup lg1150_de
 * @{
 */

#ifndef _CLI_DEF_h
#define _CLI_DEF_h
/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/
#define CLI_ADEC_CMD_BASE		   1000
#define CLI_AENC_CMD_BASE		   2000
#define CLI_AAD_CMD_BASE		   3000
#define CLI_XDE_CMD_BASE		   5000
#define CLI_PQL_CMD_BASE		   6000
#define CLI_DVR_CMD_BASE		   7000
#define CLI_I2C_CMD_BASE		   7500
#define CLI_SDEC_CMD_BASE		   8000
#define CLI_VDEC_CMD_BASE		   9000
#define CLI_SCI_CMD_BASE		   10000
#define CLI_DVBCI_CMD_BASE		   11000
#define CLI_VENC_CMD_BASE		   12000
#define CLI_SE_CMD_BASE			   13000
#define CLI_FILEPLAY_CMD_BASE	   14000
#define TOP_CLI_CHAN_MGR		   15000
#define CLI_DIMM_CMD_BASE		   16000
#define CLI_TCON_CMD_BASE		   17000
#define CLI_FMT3D_CMD_BASE		   18000
#define CLI_DENC_CMD_BASE		   19000
#define CLI_PWM_CMD_BASE		   20000

#define CLI_ADEC_CMD_SIZE		   500
#define CLI_AENC_CMD_SIZE		   500
#define CLI_AAD_CMD_SIZE		   500
#define CLI_XDE_CMD_SIZE		   300
#define CLI_PQL_CMD_SIZE		   500
#define CLI_DVR_CMD_SIZE		   500
#define CLI_SDEC_CMD_SIZE		   500
#define CLI_VDEC_CMD_SIZE		   500
#define CLI_SCI_CMD_SIZE		   500
#define CLI_DVBCI_CMD_SIZE		   500
#define CLI_VENC_CMD_SIZE		   500
#define CLI_SE_CMD_SIZE			   500
#define CLI_FILEPLAY_CMD_SIZE	   500

#define DBI_ADEC_CMD_BASE		   (CLI_ADEC_CMD_BASE	  + CLI_ADEC_CMD_SIZE)
#define DBI_AENC_CMD_BASE		   (CLI_AENC_CMD_BASE	  + CLI_AENC_CMD_SIZE)
#define DBI_AAD_CMD_BASE		   (CLI_AAD_CMD_BASE	  + CLI_AAD_CMD_SIZE)
#define DBI_XDE_CMD_BASE		   (CLI_XDE_CMD_BASE	  + CLI_XDE_CMD_SIZE)
#define DBI_PQL_CMD_BASE		   (CLI_PQL_CMD_BASE	  + CLI_PQL_CMD_SIZE)
#define DBI_DVR_CMD_BASE		   (CLI_DVR_CMD_BASE	  + CLI_DVR_CMD_SIZE)
#define DBI_SDEC_CMD_BASE		   (CLI_SDEC_CMD_BASE	  + CLI_SDEC_CMD_SIZE)
#define DBI_VDEC_CMD_BASE		   (CLI_VDEC_CMD_BASE	  + CLI_VDEC_CMD_SIZE)
#define DBI_SCI_CMD_BASE		   (CLI_SCI_CMD_BASE	  + CLI_SCI_CMD_SIZE)
#define DBI_DVBCI_CMD_BASE		   (CLI_DVBCI_CMD_BASE	  + CLI_DVBCI_CMD_SIZE)
#define DBI_VENC_CMD_BASE		   (CLI_VENC_CMD_BASE	  + CLI_VENC_CMD_SIZE)
#define DBI_SE_CMD_BASE			   (CLI_SE_CMD_BASE		  + CLI_SE_CMD_SIZE)
#define DBI_FILEPLAY_CMD_BASE	   (CLI_FILEPLAY_CMD_BASE + CLI_FILEPLAY_CMD_SIZE)

#define DBI_XDE_CMD_SIZE  200
#define MCU_XDE_CMD_SIZE  300

#define MCU_XDE_CMD_BASE  (DBI_XDE_CMD_BASE  + DBI_XDE_CMD_SIZE)
#define MCU_XDE_CMN_BASE  (MCU_XDE_CMD_BASE  + MCU_XDE_CMD_SIZE)

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define CLI2CMD(cmd)     Add_GLI_Cmd(#cmd, lCliCmdId++, cmd)

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/


#endif
/**  @} */
