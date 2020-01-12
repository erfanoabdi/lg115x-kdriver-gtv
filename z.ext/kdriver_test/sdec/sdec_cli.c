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

#include <pthread.h>

#include "base_types.h"

//#include "de_def.h"
//#include "de_ipc_def.h"
//#include "frc_cfg.h"
#include "sdec_kapi.h"
//#include "gpio/gpio_cli_def.h"
#include "cli/cli_def.h"
#include "sdec/sdec_cli.h"
//#include "dbg/dbg_reg.h"]
#define	SDEC_NODE_NAME		"/dev/lg/sdec0"


int g_iSDECFd;//basic SDEC file handle
SINT32 bSInitflag = FALSE;//basic SDEC flag

void * SECTION_Notify_CHA ( void *TaskParam);

void CLI_SDEC_Start(void)
{
	unsigned int lCliCmdId = CLI_SDEC_CMD_BASE+1;

	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_InitialaizeModule", lCliCmdId++, CLI_SDEC_SDEC_IO_InitialaizeModule);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_GetCurrentSTCPCR",  lCliCmdId++, CLI_SDEC_SDEC_IO_GetCurrentSTCPCR);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SelInputPort",  lCliCmdId++, CLI_SDEC_SDEC_IO_SelInputPort);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SetCiperEnable",  lCliCmdId++, CLI_SDEC_SDEC_IO_SetCiperEnable);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SetCiperMode",  lCliCmdId++, CLI_SDEC_SDEC_IO_SetCiperMode);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SetCiperKey",  lCliCmdId++, CLI_SDEC_SDEC_IO_SetCiperKey);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SetPcrRecovery",  lCliCmdId++, CLI_SDEC_SDEC_IO_SetPcrRecovery);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_PIDFilterAlloc",  lCliCmdId++, CLI_SDEC_SDEC_IO_PIDFilterAlloc);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_PIDFilterFree",  lCliCmdId++, CLI_SDEC_SDEC_IO_PIDFilterFree);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_PIDFilterSetPID",  lCliCmdId++, CLI_SDEC_SDEC_IO_PIDFilterSetPID);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_PIDFilterMapSelect",  lCliCmdId++, CLI_SDEC_SDEC_IO_PIDFilterMapSelect);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_PIDFilterEnable", lCliCmdId++,  CLI_SDEC_SDEC_IO_PIDFilterEnable);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_PIDFilterGetState",  lCliCmdId++, CLI_SDEC_SDEC_IO_PIDFilterGetState);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterAlloc",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterAlloc);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterFree",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterFree);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterPattern",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterPattern);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterBufferReset",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterBufferReset);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterBufferSet",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterBufferSet);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterGetInfo",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterGetInfo);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterSetReadPtr",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterSetReadPtr);
	Add_GLI_Cmd("CLI_SDEC_SDEC_IO_SectionFilterGetState",  lCliCmdId++, CLI_SDEC_SDEC_IO_SectionFilterGetState);
	Add_GLI_Cmd("CLI_Sdec_File_Open",  lCliCmdId++, CLI_Sdec_File_Open);
	Add_GLI_Cmd("CLI_Sdec_File_Close",  lCliCmdId++, CLI_Sdec_File_Close);
	Add_GLI_Cmd("CLI_SDEC_EnableLog",  lCliCmdId++, CLI_SDEC_EnableLog);

	printf("SDEC App function register success\n");

	SDEC_OpenDriverModule();

}


int CLI_Sdec_File_Open(int argc, char **argv)
{
	SDEC_OpenDriverModule ();

	return 0;
}

int CLI_Sdec_File_Close(int argc, char **argv)
{
	if(bSInitflag)
	{
		close(g_iSDECFd);

		printf("SDEC file close success\r\n");

		bSInitflag = FALSE;
	}

    	return 0;
}


void SDEC_OpenDriverModule ( void )
{
    /* open the vdec module */
    g_iSDECFd = open ( SDEC_NODE_NAME, O_RDWR );

    /* validate the driver open */
    if( g_iSDECFd == -1 )
    {
        printf("ERROR(SDEC_OpenDriverModule) Could not open device, Err[ %d ]\n", errno );
        //ffprintf("ERROR(%s) Could not open device, Err[ %d ]\n", __FUNCTION__, errno );
        return ;
    }

    bSInitflag = TRUE;

    printf("SDEC open success\n");
}


static int SDEC_DriverIOCTL(SINT32 i_uiIOCTLNumb, UINT32 i_uiArg)
{
    SINT32 iResult;

    int iFd = g_iSDECFd;
    if( iFd == -1 )
    {
        printf("ERROR(SDEC_DriverIOCTL) Could not open device :%s \n",SDEC_NODE_NAME);
        return;
    }

    iResult = ioctl(iFd, i_uiIOCTLNumb, i_uiArg);

#if 1
    if(iResult < 0)
		printf("%s ioctl failed %d", SDEC_NODE_NAME, iResult);

    return iResult;
#endif /* #if 0 */

}

int CLI_SDEC_SDEC_IO_InitialaizeModule(int argc ,char **argv)
{
	int ret;
	LX_SDEC_CAP_T stLX_SDEC_CAP_T;
	stLX_SDEC_CAP_T.eCh = LX_SDEC_CH_A;

	ret = SDEC_DriverIOCTL(SDEC_IOR_INIT,(UINT32 )&stLX_SDEC_CAP_T);

	if(ret == 0x0)
	{
		printf("ucPidFltNr[%d]\n", stLX_SDEC_CAP_T.ucPidFltNr);
		printf("ucSecFltNr[%d]\n", stLX_SDEC_CAP_T.ucSecFltNr);
		printf("ucFltDepth[%d]\n", stLX_SDEC_CAP_T.ucFltDepth);
	}

	return 0;
}

int CLI_SDEC_SDEC_IO_GetCurrentSTCPCR(int argc ,char **argv)
{
	int ret;
	LX_SDEC_GET_STC_PCR_T stLX_SDEC_GET_STC_PCR_T;
	stLX_SDEC_GET_STC_PCR_T.eCh = LX_SDEC_CH_A;

	ret = SDEC_DriverIOCTL(SDEC_IOR_GET_STC_PCR,(UINT32 )&stLX_SDEC_GET_STC_PCR_T);

	if(ret == 0x0)
	{
		printf("uiStc_hi[%d]\n", stLX_SDEC_GET_STC_PCR_T.uiStc_hi);
		printf("uiStc_lo[%d]\n", stLX_SDEC_GET_STC_PCR_T.uiStc_lo);
		printf("uiPcr_hi[%d]\n", stLX_SDEC_GET_STC_PCR_T.uiPcr_hi);
		printf("uiPcr_lo[%d]\n", stLX_SDEC_GET_STC_PCR_T.uiPcr_lo);
	}

	return 0;
}

int CLI_SDEC_SDEC_IO_SelInputPort(int argc ,char **argv)
{
	LX_SDEC_SEL_INPUT_T stLX_SDEC_SEL_INPUT_T;
	stLX_SDEC_SEL_INPUT_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_SEL_INPUT_T.eInputPath = LX_SDEC_INPUT_TS_CHA;
	stLX_SDEC_SEL_INPUT_T.eInputMode = LX_SDEC_INPUTMODE_PARALLEL0;
	stLX_SDEC_SEL_INPUT_T.eStreamMode = LX_SDEC_STREAMMODE_ATSC;

	SDEC_DriverIOCTL(SDEC_IOW_SEL_INPUT_PORT,(UINT32 )&stLX_SDEC_SEL_INPUT_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_SetCiperEnable(int argc ,char **argv)
{
	LX_SDEC_CIPHER_ENABLE_T stLX_SDEC_CIPHER_ENABLE_T;

	SDEC_DriverIOCTL(SDEC_IOW_SET_CIPHER_ENABLE,(UINT32 )&stLX_SDEC_CIPHER_ENABLE_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_SetCiperMode(int argc ,char **argv)
{
	LX_SDEC_CIPHER_MODE_T stLX_SDEC_CIPHER_MODE_T;

	SDEC_DriverIOCTL(SDEC_IOW_SET_CIPHER_MODE,(UINT32 )&stLX_SDEC_CIPHER_MODE_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_SetCiperKey(int argc ,char **argv)
{
	LX_SDEC_CIPHERKEY_T stLX_SDEC_CIPHERKEY_T;

	SDEC_DriverIOCTL(SDEC_IOW_SET_CIPHER_KEY,(UINT32 )&stLX_SDEC_CIPHERKEY_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_SetPcrRecovery(int argc ,char **argv)
{
	LX_SDEC_SET_PCR_RECOVERY_T stLX_SDEC_SET_PCR_RECOVERY_T;

//	SDEC_DriverIOCTL(SDEC_IOR_SET_PCR_RECOVERY,(UINT32 )&stLX_SDEC_SET_PCR_RECOVERY_T);

	return 0;
}

/// PID filter
int CLI_SDEC_SDEC_IO_PIDFilterAlloc(int argc ,char **argv)
{
	int ret;
	LX_SDEC_PIDFLT_ALLOC_T stLX_SDEC_PIDFLT_ALLOC_T;
	stLX_SDEC_PIDFLT_ALLOC_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_PIDFLT_ALLOC_T.ePidFltMode = LX_SDEC_PIDFLT_MODE_NORMAL;

	ret = SDEC_DriverIOCTL(SDEC_IOR_PIDFLT_ALLOC,(UINT32 )&stLX_SDEC_PIDFLT_ALLOC_T);

	if(ret == 0x0)
	{
		printf("uiPidFltId[%d]\n", stLX_SDEC_PIDFLT_ALLOC_T.uiPidFltId);
	}

	stLX_SDEC_PIDFLT_ALLOC_T.eCh = LX_SDEC_CH_B;
	stLX_SDEC_PIDFLT_ALLOC_T.ePidFltMode = LX_SDEC_PIDFLT_MODE_NORMAL;

	ret = SDEC_DriverIOCTL(SDEC_IOR_PIDFLT_ALLOC,(UINT32 )&stLX_SDEC_PIDFLT_ALLOC_T);

	if(ret == 0x0)
	{
		printf("uiPidFltId[%d]\n", stLX_SDEC_PIDFLT_ALLOC_T.uiPidFltId);
	}

	return 0;
}

int CLI_SDEC_SDEC_IO_PIDFilterFree(int argc ,char **argv)
{
	LX_SDEC_PIDFLT_FREE_T stLX_SDEC_PIDFLT_FREE_T;
	stLX_SDEC_PIDFLT_FREE_T.eCh = strtoul ( argv[1], NULL, 0 );
	stLX_SDEC_PIDFLT_FREE_T.uiPidFltId = strtoul ( argv[2], NULL, 0 );

	SDEC_DriverIOCTL(SDEC_IOW_PIDFLT_FREE,(UINT32 )&stLX_SDEC_PIDFLT_FREE_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_PIDFilterSetPID(int argc ,char **argv)
{
	LX_SDEC_PIDFLT_SET_PID_T stLX_SDEC_PIDFLT_SET_PID_T;

	stLX_SDEC_PIDFLT_SET_PID_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_PIDFLT_SET_PID_T.uiDest = strtoul ( argv[1], NULL, 0 );//LX_SDEC_PIDFLT_DEST_GPB;
	stLX_SDEC_PIDFLT_SET_PID_T.uiPidFltId = 0x0;
	stLX_SDEC_PIDFLT_SET_PID_T.uiPidValue = strtoul ( argv[2], NULL, 0 );//LX_SDEC_PIDFLT_DEST_GPB;

	SDEC_DriverIOCTL(SDEC_IOW_PIDFLT_SET_PID,(UINT32 )&stLX_SDEC_PIDFLT_SET_PID_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_PIDFilterMapSelect(int argc ,char **argv)
{
	LX_SDEC_PIDFLT_SELSECFLT_T stLX_SDEC_PIDFLT_SELSECFLT_T;
	stLX_SDEC_PIDFLT_SELSECFLT_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_PIDFLT_SELSECFLT_T.uiSecFltId = 0x0;

	SDEC_DriverIOCTL(SDEC_IOW_PIDFLT_MAP_SECFLT,(UINT32 )&stLX_SDEC_PIDFLT_SELSECFLT_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_PIDFilterEnable(int argc ,char **argv)
{
	LX_SDEC_PIDFLT_ENABLE_T stLX_SDEC_PIDFLT_ENABLE_T;

	stLX_SDEC_PIDFLT_ENABLE_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_PIDFLT_ENABLE_T.uiPidFltId = 0x0;
	stLX_SDEC_PIDFLT_ENABLE_T.bPidFltEnable = strtoul ( argv[1], NULL, 0 );//TRUE;

	SDEC_DriverIOCTL(SDEC_IOW_PIDFLT_ENABLE,(UINT32 )&stLX_SDEC_PIDFLT_ENABLE_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_PIDFilterGetState(int argc ,char **argv)
{
	int ret;
	LX_SDEC_PIDFLT_STATE_T stLX_SDEC_PIDFLT_STATE_T;
	stLX_SDEC_PIDFLT_STATE_T.eCh = strtoul ( argv[1], NULL, 0 );
	stLX_SDEC_PIDFLT_STATE_T.uiPidFltId = strtoul ( argv[2], NULL, 0 );

	ret = SDEC_DriverIOCTL(SDEC_IOR_PIDFLT_GET_STATE,(UINT32 )&stLX_SDEC_PIDFLT_STATE_T);

	if(ret == 0x0)
	{
		printf("uiPidValue[0x%08x]\n", stLX_SDEC_PIDFLT_STATE_T.uiPidValue);
		printf("uiFltState[0x%08x]\n", stLX_SDEC_PIDFLT_STATE_T.uiFltState);
	}

	return 0;
}
/// Section filter
int CLI_SDEC_SDEC_IO_SectionFilterAlloc(int argc ,char **argv)
{
	int ret;
	LX_SDEC_SECFLT_ALLOC_T stLX_SDEC_SECFLT_ALLOC_T;

	stLX_SDEC_SECFLT_ALLOC_T.eCh = LX_SDEC_CH_A;

	ret = SDEC_DriverIOCTL(SDEC_IOR_SECFLT_ALLOC,(UINT32 )&stLX_SDEC_SECFLT_ALLOC_T);

	if(ret == 0x0)
	{
		printf("uiSecFltId[0x%08x]\n", stLX_SDEC_SECFLT_ALLOC_T.uiSecFltId);
	}

	stLX_SDEC_SECFLT_ALLOC_T.eCh = LX_SDEC_CH_B;

	ret = SDEC_DriverIOCTL(SDEC_IOR_SECFLT_ALLOC,(UINT32 )&stLX_SDEC_SECFLT_ALLOC_T);

	if(ret == 0x0)
	{
		printf("uiSecFltId[0x%08x]\n", stLX_SDEC_SECFLT_ALLOC_T.uiSecFltId);
	}

	return 0;
}

int CLI_SDEC_SDEC_IO_SectionFilterFree(int argc ,char **argv)
{
	LX_SDEC_SECFLT_FREE_T stLX_SDEC_SECFLT_FREE_T;

	stLX_SDEC_SECFLT_FREE_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_SECFLT_FREE_T.uiSecFltId = 0x0;

	SDEC_DriverIOCTL(SDEC_IOW_SECFLT_FREE,(UINT32 )&stLX_SDEC_SECFLT_FREE_T);

	return 0;
}
//int CLI_SDEC_ SDEC_IO_SectionFilterMode(S_SDEC_PARAM_T *stpSdecParam, UINT32 ui32Arg);
int CLI_SDEC_SDEC_IO_SectionFilterPattern(int argc ,char **argv)
{
	LX_SDEC_SECFLT_PATTERN_T stLX_SDEC_SECFLT_PATTERN_T;

	pthread_t SectionNotifyCHA;
#if 1
	//UINT8 pucPattern[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	UINT8 pucPattern[8] = {0};
	//UINT8 pucMask[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	//UINT8 pucMask[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	UINT8 pucMask[8] = {0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	//UINT8 pucNotEqual[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	UINT8 pucNotEqual[8] = {0};
#endif /* #if 0 */

	stLX_SDEC_SECFLT_PATTERN_T.eCh = LX_SDEC_CH_A;
//	stLX_SDEC_SECFLT_PATTERN_T.uiSecFltId = 0x0;
	stLX_SDEC_SECFLT_PATTERN_T.uiSecFltMode = strtoul ( argv[1], NULL, 0 );//LX_SDEC_FLTMODE_CONTI;
	stLX_SDEC_SECFLT_PATTERN_T.uiSecFltId = strtoul ( argv[2], NULL, 0 );//LX_SDEC_FLTMODE_CONTI;
#if 1
	stLX_SDEC_SECFLT_PATTERN_T.pucPattern = &pucPattern[0];
	stLX_SDEC_SECFLT_PATTERN_T.pucMask = &pucMask[0];
	stLX_SDEC_SECFLT_PATTERN_T.pucNotEqual = &pucNotEqual[0];
#endif /* #if 0 */

		if(pthread_create (
						&SectionNotifyCHA,
						NULL,
						SECTION_Notify_CHA,
						NULL) != 0){
						printf("SECTION_Notify thread error\n");
		}


		pthread_detach(SectionNotifyCHA);


	SDEC_DriverIOCTL(SDEC_IOW_SECFLT_PATTERN,(UINT32 )&stLX_SDEC_SECFLT_PATTERN_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_SectionFilterBufferReset(int argc ,char **argv)
{
	LX_SDEC_SECFLT_BUFFER_RESET stLX_SDEC_SECFLT_BUFFER_RESET;

	SDEC_DriverIOCTL(SDEC_IOW_SECFLT_BUFFER_RESET,(UINT32 )&stLX_SDEC_SECFLT_BUFFER_RESET);

	return 0;
}

int CLI_SDEC_SDEC_IO_SectionFilterBufferSet(int argc ,char **argv)
{
	LX_SDEC_SECFLT_BUFFER_SET_T stLX_SDEC_SECFLT_BUFFER_SET_T;

	stLX_SDEC_SECFLT_BUFFER_SET_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_SECFLT_BUFFER_SET_T.uiBufAddress = 0x02000000;
	stLX_SDEC_SECFLT_BUFFER_SET_T.eBufferSize = LX_SDEC_GPB_SIZE_4K;
	stLX_SDEC_SECFLT_BUFFER_SET_T.uiSecFltId = 0x0;

	SDEC_DriverIOCTL(SDEC_IOW_SECFLT_BUFFER_SET,(UINT32 )&stLX_SDEC_SECFLT_BUFFER_SET_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_SectionFilterGetInfo(int argc ,char **argv)
{
	int ret;
	int count;
	LX_SDEC_SECFLT_BUFFER_GET_INFO_T stLX_SDEC_SECFLT_BUFFER_GET_INFO_T;

	stLX_SDEC_SECFLT_BUFFER_GET_INFO_T.eCh = LX_SDEC_CH_A;
	stLX_SDEC_SECFLT_BUFFER_GET_INFO_T.uiSecFltId = 0;

	ret = SDEC_DriverIOCTL(SDEC_IOW_SECFLT_BUFFER_GET_INFO,(UINT32 )&stLX_SDEC_SECFLT_BUFFER_GET_INFO_T);

	if(ret == 0x0)
	{
		printf("usSectionNr[0x%08x]\n", stLX_SDEC_SECFLT_BUFFER_GET_INFO_T.usSectionNr);
		printf("uiReadAddr[0x%08x]\n", stLX_SDEC_SECFLT_BUFFER_GET_INFO_T.uiReadAddr);
		printf("uiWriteAddr[0x%08x]\n", stLX_SDEC_SECFLT_BUFFER_GET_INFO_T.uiWriteAddr);

		for(count = 0; count < stLX_SDEC_SECFLT_BUFFER_GET_INFO_T.usSectionNr; count++)
		{
			printf("uiSize[0x%08x]\n", stLX_SDEC_SECFLT_BUFFER_GET_INFO_T.uiSize[0]);
		}
	}

	return 0;
}

int CLI_SDEC_SDEC_IO_SectionFilterSetReadPtr(int argc ,char **argv)
{
	LX_SDEC_SECFLT_BUFFER_SET_T stLX_SDEC_SECFLT_BUFFER_SET_T;

	SDEC_DriverIOCTL(SDEC_IOW_SECFLT_SET_READPTR,(UINT32 )&stLX_SDEC_SECFLT_BUFFER_SET_T);

	return 0;
}

int CLI_SDEC_SDEC_IO_SectionFilterGetState(int argc ,char **argv)
{
	int ret;
	LX_SDEC_SECFLT_STATE_T stLX_SDEC_SECFLT_STATE_T;

	stLX_SDEC_SECFLT_STATE_T.eCh = strtoul ( argv[1], NULL, 0 );
	stLX_SDEC_SECFLT_STATE_T.uiSecFltId = strtoul ( argv[2], NULL, 0 );

	ret = SDEC_DriverIOCTL(SDEC_IOR_SECFLT_GET_STATE,(UINT32 )&stLX_SDEC_SECFLT_STATE_T);

	if(ret == 0x0)
	{
		printf("uiFltState[0x%08x]\n", stLX_SDEC_SECFLT_STATE_T.uiFltState);
	}

	return 0;
}

int CLI_SDEC_EnableLog(int argc, char **argv)
{
	UINT32 uiLogmaskval;

	uiLogmaskval = strtoul ( argv[1], NULL, 0 );

//	SDEC_DriverIOCTL(SDEC_IOR_ENABLE_LOG, uiLogmaskval);

	return 0;
}

void * SECTION_Notify_CHA ( void *TaskParam)
{
	UINT32 filesize;
	UINT32 ACKmsg[10] = {0};

	LX_SDEC_NOTIFY_PARAM_T stLX_SDEC_NOTIFY_PARAM_T;

	while(1){
		filesize = read (g_iSDECFd, &stLX_SDEC_NOTIFY_PARAM_T, sizeof(LX_SDEC_NOTIFY_PARAM_T));

		printf("channel:%d\n", stLX_SDEC_NOTIFY_PARAM_T.channel);
		printf("status:%d\n", stLX_SDEC_NOTIFY_PARAM_T.status);
		printf("index:%d\n", stLX_SDEC_NOTIFY_PARAM_T.index);

		//printf("rcv from kerenl layer CHA: size[%d] ch[%d] filteridx[%d ] section size[%d]", filesize, ACKmsg[0], ACKmsg[1], ACKmsg[2]);


		//callback
		//fnSectionCallBack(ACKmsg[0], ACKmsg[1], ACKmsg[2]);
	}

	return 0;
}

/**  @} */
