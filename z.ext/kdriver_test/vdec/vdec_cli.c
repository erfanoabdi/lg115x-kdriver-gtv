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
#include "vdec_kapi.h"
//#include "gpio/gpio_cli_def.h"
#include "cli/cli_def.h"
#include "vdec/vdec_cli.h"
//#include "dbg/dbg_reg.h"]
#define	VDEC_NODE_NAME		"/dev/lg/vdec0"


int g_iVDECFd;//basic SDEC file handle
SINT32 bSInitflag1 = FALSE;//basic SDEC flag

void CLI_VDEC_Start(void)
{
	unsigned int lCliCmdId = CLI_VDEC_CMD_BASE+1;
	
	Add_GLI_Cmd("CLI_VDEC_IO_ChInitialize", lCliCmdId++, CLI_VDEC_IO_ChInitialize);
	Add_GLI_Cmd("CLI_VDEC_IO_ChClose",  lCliCmdId++, CLI_VDEC_IO_ChClose);
	Add_GLI_Cmd("CLI_VDEC_IO_ChReset",  lCliCmdId++, CLI_VDEC_IO_ChReset);
	Add_GLI_Cmd("CLI_VDEC_IO_SetBufferLVL",  lCliCmdId++, CLI_VDEC_IO_SetBufferLVL);
	Add_GLI_Cmd("CLI_VDEC_IO_GetBufferLVL",  lCliCmdId++, CLI_VDEC_IO_GetBufferLVL);
	Add_GLI_Cmd("CLI_VDEC_IO_GetFiemwareVersion",  lCliCmdId++, CLI_VDEC_IO_GetFiemwareVersion);
	Add_GLI_Cmd("CLI_VDEC_IO_ChStart",  lCliCmdId++, CLI_VDEC_IO_ChStart);
	Add_GLI_Cmd("CLI_VDEC_IO_ChStop",  lCliCmdId++, CLI_VDEC_IO_ChStop);
	Add_GLI_Cmd("CLI_VDEC_IO_SetLipsync",  lCliCmdId++, CLI_VDEC_IO_SetLipsync);
	Add_GLI_Cmd("CLI_VDEC_IO_GetLipsync",  lCliCmdId++, CLI_VDEC_IO_GetLipsync);
	Add_GLI_Cmd("CLI_VDEC_IO_UpdateBuffer",  lCliCmdId++, CLI_VDEC_IO_UpdateBuffer);
	Add_GLI_Cmd("CLI_VDEC_IO_SetFrame", lCliCmdId++,  CLI_VDEC_IO_SetFrame);
	Add_GLI_Cmd("CLI_VDEC_IO_GetOutput",  lCliCmdId++, CLI_VDEC_IO_GetOutput);
	Add_GLI_Cmd("CLI_VDEC_IO_GetStatus",  lCliCmdId++, CLI_VDEC_IO_GetStatus);
	Add_GLI_Cmd("CLI_VDEC_IO_SetNotify",  lCliCmdId++, CLI_VDEC_IO_SetNotify);
	Add_GLI_Cmd("CLI_VDEC_IO_GetNotify",  lCliCmdId++, CLI_VDEC_IO_GetNotify);
	Add_GLI_Cmd("CLI_VDEC_IO_SetRegister",  lCliCmdId++, CLI_VDEC_IO_SetRegister);
	Add_GLI_Cmd("CLI_VDEC_IO_GetRegister",  lCliCmdId++, CLI_VDEC_IO_GetRegister);
	Add_GLI_Cmd("CLI_Vdec_File_Open",  lCliCmdId++, CLI_Vdec_File_Open);
	Add_GLI_Cmd("CLI_Vdec_File_Close",  lCliCmdId++, CLI_Vdec_File_Close);
	Add_GLI_Cmd("CLI_VDEC_EnableLog",  lCliCmdId++, CLI_VDEC_EnableLog);


	printf("VDEC App function register success\n");

	VDEC_OpenDriverModule();

}


int CLI_Vdec_File_Open(int argc, char **argv)
{
	VDEC_OpenDriverModule ();

	return 0;
}

int CLI_Vdec_File_Close(int argc, char **argv)
{
	if(bSInitflag1)
	{
		close(g_iVDECFd);

		printf("VDEC file close success\r\n");

		bSInitflag1 = FALSE;
	}

    	return 0;
}


void VDEC_OpenDriverModule ( void )
{
    /* open the vdec module */
    g_iVDECFd = open (VDEC_NODE_NAME, O_RDWR );

    /* validate the driver open */
    if( g_iVDECFd == -1 )
    {
        printf("ERROR(VDEC_OpenDriverModule) Could not open device, Err[ %d ]\n", errno );
        //ffprintf("ERROR(%s) Could not open device, Err[ %d ]\n", __FUNCTION__, errno );
        return ;
    }

    bSInitflag1 = TRUE;

    printf("VDEC open success\n");
#if 0
    /* create the logging task */
    if ( ( enError = DEBUG_CreateFileDumpTask (
                                                    (int8*)"Sdeclog.txt",
                                                    ( 5 * 1024 ),
                                                    g_iSDECFd,
                                                    SdecLog_BufferContents,
                                                    &g_uiFdHandle ) ) != FD_NO_ERROR )
    {
        printf (
            "\nERROR(SDEC_OpenDriverModule) Log Task creation Failed [ 0x%08X ]",
            enError );
    }
#endif
}


static int VDEC_DriverIOCTL(SINT32 i_uiIOCTLNumb, UINT32 i_uiArg)
{
    SINT32 iResult;

    int iFd = g_iVDECFd;
    if( iFd == -1 )
    {
        printf("ERROR(VDEC_DriverIOCTL) Could not open device :%s \n",VDEC_NODE_NAME);
        return;
    }

    iResult = ioctl(iFd, i_uiIOCTLNumb, i_uiArg);

#if 1
    if(iResult < 0)
		printf("%s ioctl failed %d", VDEC_NODE_NAME, iResult);

    return iResult;
#endif /* #if 0 */

}


int CLI_VDEC_IO_ChInitialize(int argc, char **argv)
{
	int ret;
	LX_VDEC_IO_INIT_CH_T stLX_VDEC_IO_INIT_CH_T;

	stLX_VDEC_IO_INIT_CH_T.ch = 0x0;
	stLX_VDEC_IO_INIT_CH_T.vcodec = LX_VCODEC_MPEG2_HP;
	

//	ui32Val = strtoul ( argv[1], NULL, 0 );

	ret = VDEC_DriverIOCTL(LX_VDEC_IO_INIT_CH,  (UINT32 )&stLX_VDEC_IO_INIT_CH_T);	

	return 0;	
}

int CLI_VDEC_IO_ChClose(int argc, char **argv)
{
	UINT32 ui32Val;

	ui32Val = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_CLOSE_CH, ui32Val);	

	return 0;	
}	

int CLI_VDEC_IO_ChReset(int argc, char **argv)
{
	UINT32 ui32Val;

	ui32Val = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_CLOSE_CH, ui32Val);	

	return 0;	
}

int CLI_VDEC_IO_SetBufferLVL(int argc, char **argv)
{
	LX_VDEC_IO_BUFFER_LVL_T stLX_VDEC_IO_BUFFER_LVL_T;



	VDEC_DriverIOCTL(LX_VDEC_IO_SET_BUFFER_LVL, (UINT32 )&stLX_VDEC_IO_BUFFER_LVL_T);	

	return 0;	
}

int CLI_VDEC_IO_GetBufferLVL(int argc, char **argv)
{
	LX_VDEC_IO_BUFFER_LVL_T stLX_VDEC_IO_BUFFER_LVL_T;



	VDEC_DriverIOCTL(LX_VDEC_IO_GET_BUFFER_LVL, (UINT32 )&stLX_VDEC_IO_BUFFER_LVL_T);	

	return 0;	
}
//misc
int CLI_VDEC_IO_GetFiemwareVersion(int argc, char **argv)
{
	LX_VDEC_IO_VERSION_T stLX_VDEC_IO_VERSION_T;



	VDEC_DriverIOCTL(LX_VDEC_IO_GET_VERSION, (UINT32 )&stLX_VDEC_IO_VERSION_T);	

	return 0;	
}
//decoding control
int CLI_VDEC_IO_ChStart(int argc, char **argv)
{
	LX_VDEC_IO_START_T stLX_VDEC_IO_START_T;

	stLX_VDEC_IO_START_T.ch = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_START,(UINT32 )&stLX_VDEC_IO_START_T);

	return 0;	
}

int CLI_VDEC_IO_ChStop(int argc, char **argv)
{
	UINT32 ui32Val;

	ui32Val = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_STOP, ui32Val);	

	return 0;	
}

int CLI_VDEC_IO_SetLipsync(int argc, char **argv)
{
	LX_VDEC_LIP_SYNC_T stLX_VDEC_LIP_SYNC_T;

	VDEC_DriverIOCTL(LX_VDEC_IO_SET_LIP_SYNC,(UINT32 )&stLX_VDEC_LIP_SYNC_T);

	return 0;	
}

int CLI_VDEC_IO_GetLipsync(int argc, char **argv)
{
	LX_VDEC_LIP_SYNC_T stLX_VDEC_LIP_SYNC_T;

	VDEC_DriverIOCTL(LX_VDEC_IO_GET_LIP_SYNC,(UINT32 )&stLX_VDEC_LIP_SYNC_T);

	return 0;	
}

//for buffered/file play only
int CLI_VDEC_IO_UpdateBuffer(int argc, char **argv)
{
	UINT32 ui32Val;

	ui32Val = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_UPDATE_BUFFER, ui32Val);	

	return 0;	
}

int CLI_VDEC_IO_SetFrame(int argc, char **argv)
{
	UINT32 ui32Val;

	ui32Val = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_SET_FRAME, ui32Val);	

	return 0;	
}
//aux data gathering
int CLI_VDEC_IO_GetOutput(int argc, char **argv)
{
	int ret;
	LX_VDEC_OUTPUT_T stLX_VDEC_OUTPUT_T;

	ret = VDEC_DriverIOCTL(LX_VDEC_IO_GET_OUTPUT,(UINT32 )&stLX_VDEC_OUTPUT_T);

	if(ret == 0x0)
	{
		printf("pCb[%d]\n", stLX_VDEC_OUTPUT_T.decode.frm.pCb);
		printf("pCr[%d]\n", stLX_VDEC_OUTPUT_T.decode.frm.pCr);
		printf("pts[%d]\n", stLX_VDEC_OUTPUT_T.decode.pts);
		printf("frameRateDiv[%d]\n", stLX_VDEC_OUTPUT_T.decode.m.frameRateDiv);
		printf("frameRateRes[%d]\n", stLX_VDEC_OUTPUT_T.decode.m.frameRateRes);
		printf("iFrame[%d]\n", stLX_VDEC_OUTPUT_T.decode.m.interlFrm);
		printf("pSeq[%d]\n", !stLX_VDEC_OUTPUT_T.decode.m.interlSeq);
		printf("picType[%d]\n", stLX_VDEC_OUTPUT_T.decode.m.picType);
		printf("sar[%d]\n", stLX_VDEC_OUTPUT_T.decode.m.sar);
	}

	return 0;	
}
//for decoding status monitoring/notifying
int CLI_VDEC_IO_GetStatus(int argc, char **argv)
{
	UINT32 ui32Val;

	ui32Val = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_GET_STATUS, ui32Val);	

	return 0;	
}

int CLI_VDEC_IO_SetNotify(int argc, char **argv)
{
	LX_VDEC_NOTIFY_T stLX_VDEC_NOTIFY_T;
	stLX_VDEC_NOTIFY_T.ch = 0x0;
	stLX_VDEC_NOTIFY_T.mskNotify = LX_VDEC_NOTIFY_MSK_SEQH;

	VDEC_DriverIOCTL(LX_VDEC_IO_SET_NOTIFY,(UINT32 )&stLX_VDEC_NOTIFY_T);

	return 0;	
}

int CLI_VDEC_IO_GetNotify(int argc, char **argv)
{
	LX_VDEC_NOTIFY_T stLX_VDEC_NOTIFY_T;

	VDEC_DriverIOCTL(LX_VDEC_IO_GET_NOTIFY,(UINT32 )&stLX_VDEC_NOTIFY_T);

	return 0;	
}

typedef struct S_Sdec_Reg_Read
{
 	UINT32 		uiReAddr;
 	UINT32 		uiReValue;
}SSdecRegRead;

typedef struct S_Sdec_Reg_Write
{
 	UINT32 		uiWrAddr;
 	UINT32 		uiWrValue;
}SSdecRegWrite;

//vdec debugging
int CLI_VDEC_IO_SetRegister(int argc, char **argv)
{
    SSdecRegWrite   stParam ;

    stParam.uiWrAddr = strtoul(argv[1], NULL, 0);
    stParam.uiWrValue = strtoul(argv[2], NULL, 0);

    VDEC_DriverIOCTL(LX_VDEC_IO_SET_REG,(UINT32 )&stParam);

    return 0;
}

int CLI_VDEC_IO_GetRegister(int argc, char **argv)
{
    UINT32 iResult;
    SSdecRegRead   stParam ;

    stParam.uiReAddr = strtoul(argv[1], NULL, 0);

    VDEC_DriverIOCTL(LX_VDEC_IO_GET_REG,(UINT32 )&stParam);

    iResult = stParam.uiReValue;

    printf("Vdec Reg Read Value: [0x%08X]  ",iResult);

//    Byte2Bit(iResult);
    printf("\r\n");

    return 0;
}

#if 0
int CLI_VDEC_IO_EnableLog(int argc, char **argv)
{
	LX_SDEC_SECFLT_STATE_T stLX_SDEC_SECFLT_STATE_T;

	SDEC_DriverIOCTL(VDEC_IOR_SECFLT_GET_STATE,(UINT32 )&stLX_SDEC_SECFLT_STATE_T);

	return 0;	
}
#endif /* #if 0 */

int CLI_VDEC_EnableLog(int argc, char **argv)
{
	UINT32 uiLogmaskval;

	uiLogmaskval = strtoul ( argv[1], NULL, 0 );

	VDEC_DriverIOCTL(LX_VDEC_IO_ENABLELOG, uiLogmaskval);	

	return 0;
}

/**  @} */
