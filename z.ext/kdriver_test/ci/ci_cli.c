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
 * CLI implementation for CI device.
 *
 * author     Srinivasan Shanmugam (srinivasan.shanmugam@lge.com)
 * version    1.0
 * date       2010.03.23
 * note       Additional information.
 *
 * @addtogroup lg1150_ci
 * @{
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
#include <stdlib.h>

#include "base_types.h"

#include "ci_cli.h"
#include "ci_cfg.h"
#include "ci_kapi.h"
#include "cli/cli_arg.h"



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
 extern int Add_GLI_Cmd (char * pCmdName, unsigned int cmdId, int (*fn) (int argc, char **argv));

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/
 void CI_AppInit ( void );

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/
 UINT8*	gpDataBuf = NULL;

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/
 static SINT32 CLI_CI_IOCTL(SINT32 i_uiIOCTLNumb, UINT32 i_uiArg);


/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
static SINT32	_gwCIDevId;
static UINT32	_gnBufSize;
static BOOLEAN 	gbCIInitflag = FALSE;

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/


static SINT32 CLI_CI_IOCTL(SINT32 i_uiIOCTLNumb, UINT32 i_uiArg)
{
    SINT32 iResult = -1;

    if( _gwCIDevId == -1 )
    {
        CI_CLI_INFO( ("CI> Could not open device \n") );

        return iResult;
    }

    if ( OK != ( iResult = ioctl(_gwCIDevId, i_uiIOCTLNumb, i_uiArg) ) )
    {
		CI_CLI_INFO( ("\n CI ioctl failed %d\n", iResult) );
    }

    gbCIInitflag = TRUE;

    return 0;
}

SINT32 CLI_CI_Reset (int argc, char **argv)
{
	return CLI_CI_IOCTL(CI_IO_RESET, 0);
}

SINT32 CLI_CI_ManualHotSwap (int argc, char **argv)
{
	return CLI_CI_IOCTL(CI_IO_CAM_INIT, 0);
}

SINT32 CLI_CI_CheckCIS (int argc, char **argv)
{
	return CLI_CI_IOCTL(CI_IO_CHECK_CIS, 0);
}

SINT32 CLI_CI_WriteCOR (int argc, char **argv)
{
	return CLI_CI_IOCTL(CI_IO_WRITE_COR, 0);
}


SINT32 CLI_CI_DetectCAM (int argc, char **argv)
{
	UINT32 uwDetectCAM = 0;

	CLI_CI_IOCTL(CI_IOR_DETECT_CARD,( UINT32 ) &uwDetectCAM);

	CI_CLI_INFO( ( "CI CLI> CAM Detect %d\n", uwDetectCAM ) );

	return 0;
}

SINT32 CLI_CI_ReadData (int argc, char **argv)
{
	if ( _gnBufSize )
	{
		if ( !gpDataBuf )
		{
			gpDataBuf = malloc( _gnBufSize );
		}

		if ( gpDataBuf )
		{
			LX_CI_IOCTL_PARAM_T stReadData;

			SINT32 wTmp = 0;

			stReadData.pBuf = gpDataBuf;
			stReadData.sLength = _gnBufSize;

			CLI_CI_IOCTL(CI_IOR_READ_DATA, ( UINT32 )&stReadData);

			CI_CLI_INFO( ( "CI CLI>Read i/o data.." ) );

			for ( ; wTmp < 15; wTmp++ )
			{
				CI_CLI_INFO( ( "0x%x  ", gpDataBuf[ wTmp ] ) );
			}

			CI_CLI_INFO( ( "\n i/o data sample over\n" ) );
		}
		else
		{
			CI_CLI_INFO( ( "CI CLI>malloc <<F>>\n" ) );
		}
	}
	else
	{
		CI_CLI_INFO( ( "CI CLI>Buf size is <<%d>>,\n", _gnBufSize ) );
	}

	return 0;
}

SINT32 CLI_CI_NegoBuf (int argc, char **argv)
{
	CLI_CI_IOCTL(CI_IOR_NEGOTIATE_BUF_SIZE, ( UINT32 )&_gnBufSize);

	CI_CLI_INFO( ( "CI CLI> Buf size %d\n", _gnBufSize ) );

	return 0;
}

SINT32 CLI_CI_ReadDAStatus (int argc, char **argv)
{
	UINT32 uwDataAvlb = 0;

	CLI_CI_IOCTL(CI_IOR_READ_DA_STATUS, ( UINT32 )&uwDataAvlb);

	CI_CLI_INFO( ( "CI CLI> Data Available %d\n", uwDataAvlb ) );

	return 0;
}

SINT32 CLI_CI_WriteData (int argc, char **argv)
{
//	if ( gpDataBuf )
	{
		LX_CI_IOCTL_PARAM_T stReadData;

		#define CMD_DATA_SIZE  5

		UINT8 abyCmdData[ CMD_DATA_SIZE ] = {0x01, 0x00, 0x82, 0x01, 0x01};

		stReadData.pBuf = abyCmdData;
		stReadData.sLength = CMD_DATA_SIZE;

		CLI_CI_IOCTL(CI_IOW_WRITE_DATA, ( UINT32 )&stReadData);
	}

	return 0;
}

SINT32 CLI_CI_SetPhyReset (int argc, char **argv)
{
	CLI_CI_IOCTL(CI_IO_SET_PHY_RESET, 0);

	return 0;
}

SINT32 CLI_CI_ReadIIRStatus (int argc, char **argv)
{
	UINT32 uwIIRStat = 0;

	CLI_CI_IOCTL(CI_IOR_READ_IIR_STATUS, ( UINT32 )&uwIIRStat);

	CI_CLI_INFO( ( "CI CLI> IIR status %d\n", uwIIRStat ) );

	return 0;
}

SINT32 CLI_CI_CheckCapability (int argc, char **argv)
{
	UINT32 uwCAM = 0;

	CLI_CI_IOCTL(CI_IOR_CHECK_CAPABILITY, ( UINT32 )&uwCAM);

	CI_CLI_INFO( ( "CI CLI> uwCAM %d\n", uwCAM ) );

	return 0;
}

SINT32 CLI_CI_RegPrint (int argc, char **argv)
{
	CLI_CI_IOCTL(CI_IO_PRINT_REG, 0);

	return 0;
}

SINT32 CI_OpenDriverModule (int argc, char **argv)
{
	/* open the sci module */
	_gwCIDevId = open("/dev/lg/ci0", O_RDWR);

	/* validate the driver open */
	if( _gwCIDevId == -1 )
	{
		CI_CLI_INFO( ("ERROR Could not open CI device :[ %d ]\n", errno) );
		return -1;
	}

	gbCIInitflag = TRUE;

	return 0;
}

SINT32 CI_CloseDriverModule (int argc, char **argv)
{
	if(gbCIInitflag)
	{
		close(_gwCIDevId);

		gbCIInitflag = FALSE;
	}

	_gnBufSize = 0;

	if ( gpDataBuf )
	{
		free( gpDataBuf );
		gpDataBuf = NULL;
	}

	return 0;
}

#if 0
SINT32 CI_ManualHS (int argc, char **argv)
{
	CLI_CI_IOCTL(CI_IOR_MAN_HS, 0);

	return 0;
}
#endif /* #if 0 */

void CI_AddCLICommands(void)
{
	unsigned int lCliCmdId = CLI_CI_CMD_BASE;

	Add_GLI_Cmd("CI_OpenMod", lCliCmdId++ ,CI_OpenDriverModule);
	Add_GLI_Cmd("CI_DetectCard", lCliCmdId++ ,CLI_CI_DetectCAM);
	Add_GLI_Cmd("CI_Reset", lCliCmdId++ ,CLI_CI_Reset);
	Add_GLI_Cmd("CI_ManualHotSwap", lCliCmdId++ ,CLI_CI_ManualHotSwap);
	Add_GLI_Cmd("CI_CheckCIS", lCliCmdId++ ,CLI_CI_CheckCIS);
	Add_GLI_Cmd("CI_WriteCOR", lCliCmdId++ ,CLI_CI_WriteCOR);
	Add_GLI_Cmd("CI_NegBufSize", lCliCmdId++ ,CLI_CI_NegoBuf);
	Add_GLI_Cmd("CI_WriteData", lCliCmdId++ ,CLI_CI_WriteData);
	Add_GLI_Cmd("CI_ReadDAStat", lCliCmdId++ ,CLI_CI_ReadDAStatus);
	Add_GLI_Cmd("CI_ReadData", lCliCmdId++ ,CLI_CI_ReadData);
	Add_GLI_Cmd("CI+_SetPhyReset", lCliCmdId++ ,CLI_CI_SetPhyReset);
	Add_GLI_Cmd("CI+_ReadIIR", lCliCmdId++ ,CLI_CI_ReadIIRStatus);
	Add_GLI_Cmd("CI_CheckCapblty", lCliCmdId++ ,CLI_CI_CheckCapability);
	Add_GLI_Cmd("CI_RegPrint", lCliCmdId++ ,CLI_CI_RegPrint);
	Add_GLI_Cmd("CI_CloseMod", lCliCmdId++ ,CI_CloseDriverModule);
#if 0
	Add_GLI_Cmd("CI_ManualHS", lCliCmdId++ ,CI_ManualHS);
#endif /* #if 0 */

	CI_CLI_INFO( ("CI app commands register success\n") );
}

void CI_AppInit ( void )
{
	CI_AddCLICommands ( );
}

/**  @} */
