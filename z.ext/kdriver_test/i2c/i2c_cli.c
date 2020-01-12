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
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "base_types.h"

#include "i2c_kapi.h"
#include "cli/cli_def.h"
#include "cli/cli_arg.h"

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

/* I2C CH : 0 ~ 3 */
#define GLOBAL_I2C_CTRL_NO	0
#define DEVICE_SLAVE_ID		u8SlaveId
/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/

#define I2C_SUB_ADDR_LEN	1

#define I2C_MAX_CH			4
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
void Settimer(void);
float Gettimer(void);
#define BASETIME 100000L

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/


int g_iI2cInited = 0;
int g_iI2cFd[I2C_MAX_CH];

/*
 * 0x30 - AAD
 * 0x20 - Digital Audio
 */
UINT8	u8SlaveId = 0x20;

int DDI_I2C_Ioctl(unsigned int cmd, UINT32 i_uiArg)
{
	int iResult;

	iResult = ioctl(g_iI2cFd[0] , cmd, i_uiArg);

	return RET_OK;
}

int CLI_I2C_Close(int argc ,char **argv)
{
	int i;

	if ( g_iI2cInited)
	{
		for(i=0; i<I2C_MAX_CH; i++)
		{
			close(g_iI2cFd[i]);
		}
		g_iI2cInited = 0;
	}

	return RET_OK;
}


int CLI_I2C_IO_INIT(int argc ,char **argv)
{
	char fname[64];
	int i;

	if ( g_iI2cInited == 0 )
	{
		for(i=0; i<I2C_MAX_CH; i++)
		{
			sprintf(fname, "/dev/lg/i2c%d", i);
			g_iI2cFd[i] = open(fname, O_RDWR);
			if( g_iI2cFd[i] == -1)
			{
				printf("ERROR(%s)- Could not open device: i2c device [%s]\n", __FUNCTION__, fname);
				return RET_ERROR;
			}

		}

		g_iI2cInited = 1;
	}

	return RET_OK;
}

int CLI_I2C_IOW_SET_CLOCK(int argc ,char **argv)
{
	int ret_val = RET_ERROR;
	UINT32 nArg = START_ARGS;
	UINT8 clock;

	/* 0: standard, 1:fasst, 2:slow */
	switch((UINT8) A2U)
	{
		case 0: clock = I2C_CLOCK_100KHZ; break;
		case 1: clock = I2C_CLOCK_400KHZ; break;
		case 2:
		default: clock = I2C_CLOCK_50KHZ; break;
	}

	ret_val = ioctl(g_iI2cFd[GLOBAL_I2C_CTRL_NO], LXDRV_IIC_CLOCK, clock);

	return ret_val;
}

static UINT32	ui32I2CPrintControl;
int CLI_I2C_IOW_ENABLE_PRINT(int argc ,char **argv)
{
	int ret_val = RET_ERROR;

	ui32I2CPrintControl = !ui32I2CPrintControl;
	printf ( "I2C Print Enable/Disable..[%d]\n", ui32I2CPrintControl );
	ret_val = ioctl(g_iI2cFd[GLOBAL_I2C_CTRL_NO], LXDRV_IIC_ENABLE_PRINT, (UINT32)ui32I2CPrintControl);

	return ret_val;
}


#define AAD_TEST
int CLI_I2C_IOW_WRITE(int argc ,char **argv)
{
	int ret_val = RET_ERROR;
	LX_I2C_RW_DATA_T	IIC_param = { 0 };
	UINT8			aui8Data[8];
	UINT32 nArg = START_ARGS;
	UINT32		ui32Addr;
    float d;

	if (argc != 3) {
        printf("i2c_write <addr> <data>\n");
		return RET_ERROR;
	}
	ui32Addr = A2U;	//Addr
	aui8Data[0] = A2U;	//Data

	memset ( (char *)IIC_param.subAddr, 0xFF, 4 );
#if	I2C_SUB_ADDR_LEN == 1
	IIC_param.subAddrSize = 1;
	IIC_param.subAddr[0] = ui32Addr;		//register address - 0 for volume control
#else
	IIC_param.subAddrSize = 2;
	IIC_param.subAddr[0] = (ui32Addr & 0xFF00) >> 8;	//MSB
	IIC_param.subAddr[1] = ui32Addr & 0xFF;			//LSB
#endif /* #if 0 */

	IIC_param.slaveAddr = DEVICE_SLAVE_ID ; //Audio slave id //0x20 for digital audio
	IIC_param.buf = aui8Data;		//Data pointer
	IIC_param.bufSize = 1;

	//Set clock
	ret_val = ioctl(g_iI2cFd[GLOBAL_I2C_CTRL_NO], LXDRV_IIC_CLOCK, I2C_CLOCK_100KHZ);

	Settimer();
	ret_val= ioctl(g_iI2cFd[GLOBAL_I2C_CTRL_NO], LXDRV_IIC_WRITE, &IIC_param);
	d=Gettimer();             // 위의 계산 소요 시간
	printf("luma95 write time >%10.3f[sec]\n",d);

	printf ( "I2C Write return : %d\n", ret_val);
	return ret_val;
}


int CLI_I2C_IOW_READ(int argc ,char **argv)
{
	int ret_val = RET_ERROR;
	LX_I2C_RW_DATA_T	IIC_param = { 0 };
	UINT8			aui8Data[8] = { 0 };
	UINT32 nArg = START_ARGS;
	UINT32			ui32Addr;
    float d;

	if (argc != 2) {
        printf("i2c_read [<addr>]\n");
//		return RET_ERROR;
		ui32Addr = 0;	//Addr
		IIC_param.subAddrSize = 0;
		IIC_param.subAddr[0] = 0;
	}
	else
	{
		ui32Addr = A2U;	//Addr

		memset ( (char *)IIC_param.subAddr, 0xFF, 4 );
#if	I2C_SUB_ADDR_LEN == 1
		IIC_param.subAddrSize = 1;
		IIC_param.subAddr[0] = ui32Addr;		//register address - 0 for volume control
#else
		IIC_param.subAddrSize = 2;
		IIC_param.subAddr[0] = (ui32Addr & 0xFF00) >> 8;	//MSB
		IIC_param.subAddr[1] = ui32Addr & 0xFF;			//LSB
#endif /* #if 0 */
	}

	IIC_param.slaveAddr = DEVICE_SLAVE_ID; //Audio slave id //0x20 for digital audio
	IIC_param.buf = aui8Data;		//Data pointer
	IIC_param.bufSize = 1;

	//Set clock
	ret_val = ioctl(g_iI2cFd[GLOBAL_I2C_CTRL_NO], LXDRV_IIC_CLOCK, I2C_CLOCK_100KHZ);

	Settimer();
	ret_val= ioctl(g_iI2cFd[GLOBAL_I2C_CTRL_NO], LXDRV_IIC_READ, &IIC_param);
	d=Gettimer();             // 위의 계산 소요 시간
	printf("luma95 read time >%10.3f[sec]\n",d);

#if 1
	printf ( "I2C Read return : %d, Reg value:[%d -> 0x%X]\n", ret_val, aui8Data[0], aui8Data[0] );
#else
	if ( IIC_param.bufSize > 1 )
	{
		while ( IIC_param.bufSize > 0 )
		{
		}
	}
#endif /* #if 0 */
	return ret_val;
}

int CLI_I2C_SET_SLAVE_ID(int argc ,char **argv)
{
	UINT32 nArg = START_ARGS;

	if (argc != 2) {
        printf("i2c_slave_id <0x-->\n");
		return 0;
	}
	u8SlaveId = (UINT8) A2U;
	return 0;
}


/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
int CLI_I2C_Start(void)
{
	unsigned int lCliCmdId = CLI_I2C_CMD_BASE;

	/* 7500 */ Add_GLI_Cmd("CLI_I2C_IO_INIT"			, lCliCmdId++, CLI_I2C_IO_INIT);
	/* 7501 */ Add_GLI_Cmd("CLI_I2C_IOW_SET_CLOCK"		, lCliCmdId++, CLI_I2C_IOW_SET_CLOCK);
	/* 7502 */ Add_GLI_Cmd("CLI_I2C_IOW_WRITE"			, lCliCmdId++, CLI_I2C_IOW_WRITE);
	/* 7503 */ Add_GLI_Cmd("CLI_I2C_IOW_READ"			, lCliCmdId++, CLI_I2C_IOW_READ);
	/* 7504 */ Add_GLI_Cmd("CLI_I2C_CLOSE"				, lCliCmdId++, CLI_I2C_Close);
	/* 7505 */ Add_GLI_Cmd("CLI_I2C_SET_SLAVE_ID"		, lCliCmdId++, CLI_I2C_SET_SLAVE_ID);
	/* 7506 */ Add_GLI_Cmd("CLI_I2C_IOW_ENABLE_PRINT"	, lCliCmdId++, CLI_I2C_IOW_ENABLE_PRINT);


	return RET_OK;
}

struct timespec tss, tse;
int				nDelay;

void Settimer(void)
{
#if 1
 	struct itimerval t;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_value.tv_sec = BASETIME; /* 1시간   */
    t.it_value.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &t, NULL) == -1){ perror("Failed to set virtual timer"); }
#else
	clock_gettime(CLOCK_MONOTONIC, &tss);

#endif /* #if 0 */
}

float Gettimer(void)
{
#if 1
    struct itimerval t;
    float  diftime;
    if (getitimer(ITIMER_REAL, &t) == -1){perror("Failed to get virtual timer");}

    diftime=(float)BASETIME-((float)(t.it_value.tv_sec)+((float)t.it_value.tv_usec/1000000.0));
    return(diftime);
#else
	clock_gettime(CLOCK_MONOTONIC, &tse);
	nDelay = (tse.tv_sec - tss.tv_sec)*1000 + tse.tv_nsec/1000000 - tss.tv_nsec/1000000;
	printf("running time is %4d msec\n",nDelay);
#endif /* #if 0 */
}

/**  @} */
