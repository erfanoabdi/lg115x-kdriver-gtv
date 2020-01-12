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

#include "venc_kapi.h"
#include "cli/cli_def.h"
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

int g_iVencFd = 0;
 
int DDI_VENC_Ioctl(unsigned int cmd, UINT32 i_uiArg)
{
	int iResult = 0;

	if ( g_iVencFd )
	{
		iResult = ioctl(g_iVencFd , cmd, i_uiArg);
	}

	return iResult;
}

int CLI_VENC_OPEN(int argc ,char **argv)
{
	int ret_val = RET_OK;

	if ( g_iVencFd == 0 )
	{
		g_iVencFd = open("/dev/lg/venc0", O_RDWR);
		if( g_iVencFd == -1)
		{
			printf("ERROR(%s)- Could not open device: venc\n", __FUNCTION__);
			return RET_ERROR;
		}
		printf ( "VENC device open success!!\n");
	}
	else
	{
		printf ( "VENC device already opened [0x%08X]\n", g_iVencFd );
	}

	return ret_val;
}

int CLI_VENC_CLOSE(int argc ,char **argv)
{
	int iResult;

	if ( (g_iVencFd != 0 ) && (g_iVencFd != -1) )
	{
		close ( g_iVencFd );
		g_iVencFd = 0;
	}
	return RET_OK;
}



/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/

/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/


int CLI_VENC_IOW_INIT(int argc ,char **argv)
{
	int ret_val = RET_OK;
	
	ret_val = DDI_VENC_Ioctl(LX_VENC_IO_INIT, 0); 	

	return ret_val;
}

int CLI_VENC_ScalerStart(int argc ,char **argv)
{
	int ret_val = RET_OK;
	UINT32 nArg = START_ARGS;
	LX_VENC_IO_ThumbNailGen_T	stThumb = { 0 };

	/* 
	 * The input YUV frame has to be loaded in the below address 
	 * before giving the start command. The scalar works only if the 
	 * Codec is loaded and the VENC is initialized
	 */
	stThumb.ui32InputFrameBaseAddr	= 0x0FC91000;	
	stThumb.ui32ChrominanceBaseAddr = 0x0FEB1000;
	stThumb.ui32InputWidth			= 1920;	//1920 X 1080 is HD resolution
	stThumb.ui32InputHeight			= 1088;	
	stThumb.ui16InputStride	   		= 2048;	//2048 is standard stride size for input from video decoder
	stThumb.ui16ChbOpControl		= 3;	//Value 3 for digital thumbnail start
	stThumb.ui32ScaledWidth			= 320;
	stThumb.ui32ScaledHeight		= 240;
	
	ret_val = DDI_VENC_Ioctl(LX_VENC_IO_START_SCALAR, (UINT32)&stThumb); 	

	printf ( "I/P Luma Base[0x%08X] C_Base[0x%08X] Width[0x%X] Height[0x%X] Stride[0x%X] OpControl[0x%X]\n", 
		stThumb.ui32InputFrameBaseAddr,
		stThumb.ui32ChrominanceBaseAddr,
		stThumb.ui32InputWidth,
		stThumb.ui32InputHeight,
		stThumb.ui16InputStride,
		stThumb.ui16ChbOpControl );

	return ret_val;
}

int CLI_VENC_GetScaledImage(int argc ,char **argv)
{
	int ret_val = RET_OK;
	UINT32 nArg = START_ARGS;
	LX_VENC_IO_ThumbNailOut_T	stThumb = { 0 };

	ret_val = DDI_VENC_Ioctl(LX_VENC_IO_GET_SCALED_IMAGE, (UINT32)&stThumb); 	

	printf ( "ScaleImagePhyAddr[0x%08X] Size[0x%08X]\n", 
		stThumb.ui32ScaledImagePhysicalBase,
		stThumb.ui32ScaledImageSize );

	return ret_val;
}

int CLI_VENC_PngStart(int argc ,char **argv)
{
	int ret_val = RET_OK;
	UINT32 nArg = START_ARGS;
	LX_VENC_IO_PngDec_T	stPng = { 0 };

	/* 
	 * Parameters corresponding to 90dpi.png 
	 * Load the extracted IDAT of 90dpi.png into the input physical address
	 * before calling the IOCTL
	 * The address given below is used by PVR. This is only for testing
	 */
	stPng.ui32InputBase	   = 0x9A90000;	/* Allowing input length of 0x40000-> 320KB */
	stPng.ui32InputDataLen = 0x1FE1E;
	stPng.ui32OutputBase   = 0x9AE0000;	
	stPng.ui16Width		   = 0x10E;
	stPng.ui16Height	   = 0x10E;
	stPng.ui8BitDepth	   = 0x8;
	stPng.ui8ColorType	   = 0x2;
	
	ret_val = DDI_VENC_Ioctl(LX_VENC_IO_START_PNG, (UINT32)&stPng); 	

	printf ( "I/P Base[0x%08X] Size[0x%08X] O/P Base[0x%08X] Width[0x%X] Height[0x%X] BitDepth[0x%X] ColorType[0x%X]\n", 
		stPng.ui32InputBase,
		stPng.ui32InputDataLen,
		stPng.ui32OutputBase,
		stPng.ui16Width,
		stPng.ui16Height,
		stPng.ui8BitDepth,
		stPng.ui8ColorType );

	return ret_val;
}

int CLI_VENC_GetPngData(int argc ,char **argv)
{
	int ret_val = RET_OK;
	UINT32 nArg = START_ARGS;
	LX_VENC_IO_PngOut_T	stPng = { 0 };

	ret_val = DDI_VENC_Ioctl(LX_VENC_IO_GET_PNG_OUT, (UINT32)&stPng); 	

	printf ( "PngImagePhyAddr[0x%08X] Size[0x%08X]\n", 
		stPng.ui32OutputBase,
		stPng.ui32OutputDataLen );

	return ret_val;
}

UINT32 CL_VENC_LoadCodec(int argc, char **argv)
{
	FILE *pstFile;
	UINT32	ui32Size;
	UINT8	*pui8Data;

	if ( argc < 2 )
	{
		printf("%s <Codec File> -> Will load codec file to memory !!\n", argv[0]);
		return 0;
	}
	pstFile = fopen((char *)argv[1], "rb");

	if (!pstFile)
	{
		printf("Can't open %s\n",argv[1]);
		return 0;
	}

	/* Find file size */
	fseek ( pstFile, 0, SEEK_END );
	ui32Size = ftell (pstFile);
	fseek ( pstFile, 0, SEEK_SET );

	/* Allocate memory */
	pui8Data = malloc ( ui32Size );
	if ( pui8Data == NULL )
	{
		printf ( "<MALLOC_ERROR> Size[%d] for VENC Codec[%s]\n",
			ui32Size,
			argv[1] );
		fclose (pstFile);
		return 0;
	}

	/* Read the full data from beginning */
	fread ( pui8Data, 1, ui32Size, pstFile );

	/* Load the codec using IOCTL */
	{
		LX_VENC_IO_FW_CODEC_T	stVencCodec;
		stVencCodec.pCodec = pui8Data;
		stVencCodec.size = ui32Size;
		DDI_VENC_Ioctl( LX_VENC_IO_WRITE_CODEC, (UINT32) &stVencCodec );
		DDI_VENC_Ioctl( LX_VENC_IO_WRITE_RUN, 0 );		
	}

	/* Free the memory */
	free (pui8Data);

	fclose ( pstFile );

	return 0;
}


int CLI_VENC_Start(void)
{
	unsigned int lCliCmdId = CLI_VENC_CMD_BASE;

	Add_GLI_Cmd("VENC_INIT			", lCliCmdId++, CLI_VENC_IOW_INIT		);
	Add_GLI_Cmd("CLI_VENC_OPEN		", lCliCmdId++, CLI_VENC_OPEN			);
	Add_GLI_Cmd("CLI_VENC_CLOSE		", lCliCmdId++, CLI_VENC_CLOSE			);
	Add_GLI_Cmd("CLI_VENC_ScalerStart", lCliCmdId++, CLI_VENC_ScalerStart	);
	Add_GLI_Cmd("CLI_VENC_GetScaledImage", lCliCmdId++, CLI_VENC_GetScaledImage	);
	Add_GLI_Cmd("CLI_VENC_PngStart	", lCliCmdId++, CLI_VENC_PngStart			);
	Add_GLI_Cmd("CLI_VENC_GetPngData", lCliCmdId++, CLI_VENC_GetPngData			);
	Add_GLI_Cmd("CL_VENC_LoadCodec", lCliCmdId++, CL_VENC_LoadCodec			);


	return RET_OK;
}


