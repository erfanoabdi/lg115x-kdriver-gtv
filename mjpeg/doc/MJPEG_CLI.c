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
 *  test program implementation for MJPEG device.
 *
 *  author		youngwoo.jin (youngwoo.jin@lge.com)
 *  version		1.0
 *  date		2011.04.11
 *  note		Additional information.
 *
 *  @addtogroup lg1152_MJPEG
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>			// for errno
#include <string.h>			// for strerror
#include <linux/ioctl.h>	// for _IOC macro
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <malloc.h>

#include "base_types.h"
#include <Common/MOD_DTV.h>

#include <CLI/cli_main.h>
#include "MJPEG_kapi.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define MJPEG_DEVICE_PATH	"/dev/lg/png0"//ICOD_NODE_NAME
#define MJPEG_BUFFER_SIZE	0x00200000
#define MJPEG_FILE_BUFFER_SIZE 	7 * 1024 *1024

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define MJPEG_CHECK_CODE(__checker,__if_action,__fmt,__args...)	\
			__CHECK_IF_ERROR(__checker,printf,__if_action,__fmt,##__args)
#define FILE_WRITE01( _dataPtr, _filePtr )	{ fwrite( (void *)_dataPtr, 1, 1, _filePtr ); }
#define FILE_WRITE02( _dataPtr, _filePtr )	{ fwrite( ((void *)_dataPtr)+1, 1, 1, _filePtr ); \
											  fwrite( ((void *)_dataPtr)+0, 1, 1, _filePtr ); }


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
	global Functions
----------------------------------------------------------------------------------------*/
void MJPEG_CLI_Call(void);

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static void		MJPEG_CLI_AddCommand(void);
static SINT32	MJPEG_CLI_Init(int argc, char **argv);
static SINT32	MJPEG_CLI_Encode(int argc, char **argv);
static SINT32	MJPEG_CLI_Decode(int argc, char **argv);
static SINT32	MJPEG_OpenDriverModule(void);
static void		MJPEG_CloseDriverModule(void);
static SINT32	MJPEG_DriverIOCTL(UINT32 i_uiIOCTLNumb, UINT32 i_uiArg);
static UINT32	_GetFileSize(char *i_pcFileName);
static UINT32	_LoadFile(char *i_pcFileName, void *o_pBuffer);
static UINT32	_LoadInputFile(char *i_pcFileName, LX_MEM_BUF_T *pstInputStream);
static UINT32	_YUVParser(void *i_pBuffer, UINT32 i_ui32Size, MJPEG_IMAGE_INFO_T *o_pstImageInfo);
static UINT32	_CreateJPEG(char *i_pcFileName, LX_MEM_STREAM_T *i_pstData,
							    MJPEG_HEADER_T *i_pstHeader, UINT32 i_ui32Size);
static UINT32	_CreateYUV(char *i_pcFileName, LX_MEM_STREAM_T *i_pstInput,
							  MJPEG_IMAGE_INFO_T *i_pstInfo, UINT32 i_ui32Size);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
static int g_iMJPEGDevID = 0;

/*========================================================================================
	Implementation Group
========================================================================================*/
/**
 * Be called by CLI
 *
 */
void MJPEG_CLI_Call(void)
{
	MJPEG_CLI_AddCommand();
}

/**
 *
 *
 */
static void
MJPEG_CLI_AddCommand(void)
{
	Add_GLI_Cmd("MJPEG Encode YUV image file", 280, MJPEG_CLI_Encode);
	Add_GLI_Cmd("MJPEG Decode JPG image file", 281, MJPEG_CLI_Decode);
	Add_GLI_Cmd("MJPEG Initialize", 282, MJPEG_CLI_Init);

	printf("MJPEG App function register success\n");
}

/**
 *
 *
 */
static SINT32
MJPEG_CLI_Init(int argc, char **argv)
{
	if ( MJPEG_OpenDriverModule() != RET_OK )
	{
		printf( "App] Device open error\n" );
		return 0;
	}

	MJPEG_DriverIOCTL(MJPEG_IO_SW_RESET, 0);

	MJPEG_CloseDriverModule();
	return 0;
}

/**
 *
 *
 */
static SINT32
MJPEG_CLI_Encode(int argc, char **argv)
{
	SINT32 ret;
	char cOutputFileName[100];
	char *pcFileName = cOutputFileName;
	char *pcInputFileName;
	UINT32 ui32ImageSize, ui32BufferSize;
	LX_MEM_STREAM_T stFileStream;
	LX_MEM_BUF_T stInputBuffer, stOutputBuffer[2];
	UINT8 ui8BufferIndex;
	MJPEG_ENC_INIT_PARAM_T stEncInitParam;
	MJPEG_ENC_START_PARAM_T stEncParam;
	MJPEG_ENC_STOP_PARAM_T stStopParam;

	// Initialize variable
	memset( &stFileStream, 0, sizeof(LX_MEM_STREAM_T) );
	memset( stOutputBuffer, 0, sizeof(LX_MEM_BUF_T)*2 );

	MJPEG_CHECK_CODE( argc < 2, return 0, "App] Usage : %s <YUV file name>\n", argv[0] );

	pcInputFileName = argv[1];
	while ( ((*pcFileName++ = *pcInputFileName++) != '.')
			 && (*pcInputFileName != '\0'));

	*pcFileName++ = 'o';
	*pcFileName++ = 'u';
	*pcFileName++ = 't';
	*pcFileName++ = '.';
	*pcFileName++ = 'j';
	*pcFileName++ = 'p';
	*pcFileName++ = 'g';
	*pcFileName++ = '\0';

	ret = MJPEG_OpenDriverModule();
	MJPEG_CHECK_CODE( ret != RET_OK, return 0, "App] Device open error\n");

	// Reset device
	ret = MJPEG_DriverIOCTL(MJPEG_IO_SW_RESET, 0);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Device reset error\n" );

	// Get buffer size
	ret = MJPEG_DriverIOCTL(MJPEG_IOR_BUFFER_SIZE, (UINT32)&ui32BufferSize);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Device buffer error\n" );
	MJPEG_CHECK_CODE( ui32BufferSize == 0, goto FUNCTION_EXIT, "App] Device buffer size error\n" );

	// For input JPEG file
	stFileStream.mem_buf.addr = (UINT32)malloc( MJPEG_FILE_BUFFER_SIZE );
	MJPEG_CHECK_CODE( stFileStream.mem_buf.addr == 0, goto FUNCTION_EXIT, "App] Memory allocation error\n" );

	// For output JPEG file
	stOutputBuffer[0].addr = (UINT32)mmap( 0, ui32BufferSize, PROT_READ|PROT_WRITE, MAP_SHARED, g_iMJPEGDevID, 0 );
	MJPEG_CHECK_CODE( (void *)stOutputBuffer[0].addr == MAP_FAILED, goto FUNCTION_EXIT, "App] Device mmap error\n" );
	stOutputBuffer[0].length = LX_CALC_ALIGNED_VALUE( (MJPEG_BUFFER_SIZE>>1)-3, 2);	// ui32BufferSize
	stOutputBuffer[1].addr = stOutputBuffer[0].addr + stOutputBuffer[0].length;
	stOutputBuffer[1].length = stOutputBuffer[0].length;

	// For temporary input frame buffer
	stInputBuffer.addr = stOutputBuffer[0].addr + MJPEG_BUFFER_SIZE;
	stInputBuffer.length = ui32BufferSize - MJPEG_BUFFER_SIZE;

	// Read YUV image file
	ui32ImageSize = _LoadInputFile( argv[1], &stInputBuffer );
	MJPEG_CHECK_CODE( ui32ImageSize == 0, goto FUNCTION_EXIT, "App] YUV image file error\n" );

	ui32ImageSize = _YUVParser( (void *)stInputBuffer.addr, ui32ImageSize, &stEncInitParam.stImageInfo );
	MJPEG_CHECK_CODE( ui32ImageSize == 0, goto FUNCTION_EXIT, "App] YUV image info error\n" );

	stInputBuffer.addr = 0x77B10000 + MJPEG_BUFFER_SIZE;		// Physical address of input buffer
	memcpy( (void *)&stEncInitParam.stBuffer, (void *)&stInputBuffer, sizeof(LX_MEM_BUF_T) );
	stEncInitParam.stOption.eRotation = 0;//MJPEG_ROTATE_90;
	stEncInitParam.stOption.eMirroring = 0;//MJPEG_MIRROR_HORIZONTAL|MJPEG_MIRROR_VERTICAL;

	ret = MJPEG_DriverIOCTL(MJPEG_IOW_ENCODE_INIT, (UINT32)&stEncInitParam);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Encoder init error\n" );

	ui8BufferIndex = 0;

	stEncParam.ui32OutputOffset = (ui8BufferIndex==0)? 0 : stOutputBuffer[0].length;
	stEncParam.ui32OutputLength = stOutputBuffer[ui8BufferIndex].length;

	MJPEG_DriverIOCTL(MJPEG_IOW_ENCODE_START, (UINT32)&stEncParam);

	ui8BufferIndex = 1;

	do
	{
		stEncParam.ui32OutputOffset = (ui8BufferIndex==0)? 0 : stOutputBuffer[0].length;
		stEncParam.ui32OutputLength = stOutputBuffer[ui8BufferIndex].length;

		ui8BufferIndex = (~ui8BufferIndex)&0x01;

		ret = MJPEG_DriverIOCTL(MJPEG_IOW_ENCODE_START, (UINT32)&stEncParam);

		if ( ret == RET_OK )
		{
			memcpy( (void *)stFileStream.mem_buf.addr+stFileStream.curr_offset, (void *)stOutputBuffer[ui8BufferIndex].addr,
					stOutputBuffer[ui8BufferIndex].length );
			stFileStream.curr_offset += stOutputBuffer[ui8BufferIndex].length;
		}
	} while( ret == RET_OK );

	ret = MJPEG_DriverIOCTL(MJPEG_IOR_ENCODE_STOP, (UINT32)&stStopParam);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Encoding error\n" );

	memcpy( (void *)stFileStream.mem_buf.addr+stFileStream.curr_offset, (void *)stOutputBuffer[ui8BufferIndex].addr,
			stOutputBuffer[ui8BufferIndex].length );
	stFileStream.curr_offset = 0;

	// Display Image info
	printf("App] Input image information\n");

	switch ( stEncInitParam.stImageInfo.eFormat )
	{
		case MJPEG_FORMAT_420: { printf("App] Format - 4:2:0\n"); break; }
		case MJPEG_FORMAT_422: { printf("App] Format - 4:2:2 Horizontal\n"); break; }
		case MJPEG_FORMAT_224: { printf("App] Format - 4:2:2 Vertical\n"); break; }
		case MJPEG_FORMAT_444: { printf("App] Format - 4:4:4\n"); break; }
		case MJPEG_FORMAT_400: { printf("App] Format - 4:0:0\n"); break; }
		default : { printf("App] Unknown format\n"); break; }
	}
	printf("App] Image Size : %d * %d\n", stEncInitParam.stImageInfo.ui16Width, stEncInitParam.stImageInfo.ui16Height);

	ui32ImageSize = _CreateJPEG( cOutputFileName, &stFileStream, &stStopParam.stHeader, stStopParam.ui32Length );
	printf("App] Output image information\n");
	printf("App] Image Size : %d * %d\n", stStopParam.stHeader.stFrameHeader.ui16Width, stStopParam.stHeader.stFrameHeader.ui16Height);
	printf("App] File size : %d\n", ui32ImageSize);

FUNCTION_EXIT:
	if( stFileStream.mem_buf.addr != 0 ) { free((void *)stFileStream.mem_buf.addr); }
	if( stOutputBuffer[0].addr != 0 ) { munmap((void *)stOutputBuffer[0].addr, 0x00400000); }
	MJPEG_CloseDriverModule();
	return 0;
}

/**
 *
 *
 */
static SINT32
MJPEG_CLI_Decode(int argc, char **argv)
{
	SINT32 ret;
	char cOutputFileName[100];
	char *pcFileName = cOutputFileName;
	char *pcInputFileName;
	UINT32 ui32BufferSize, ui32FileSize;
	LX_MEM_STREAM_T stFileStream;
	LX_MEM_BUF_T stInputBuffer[2];
	UINT8 ui8BufferIndex;
	LX_MEM_STREAM_T stOutputStream;
	MJPEG_DEC_SEQ_PARAM_T stDecSeqParam;
	MJPEG_DEC_INIT_PARAM_T stDecInitParam;
	MJPEG_DEC_START_PARAM_T stDecParam;
	MJPEG_DEC_STOP_PARAM_T stStopParam;

	// Initialize variable
	memset( &stFileStream, 0, sizeof(LX_MEM_STREAM_T) );
	memset( stInputBuffer, 0, sizeof(LX_MEM_BUF_T)*2 );

	MJPEG_CHECK_CODE(argc < 2, return 0, "App] Usage : %s <JPEG file name>\n", argv[0]);

	pcInputFileName = argv[1];
	while ( ((*pcFileName++ = *pcInputFileName++) != '.')
			 && (*pcInputFileName != '\0'));

	*pcFileName++ = 'y';
	*pcFileName++ = 'u';
	*pcFileName++ = 'v';
	*pcFileName++ = '\0';

	//Open device
	ret = MJPEG_OpenDriverModule();
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Device open error\n");

	// Reset device
	ret = MJPEG_DriverIOCTL(MJPEG_IO_SW_RESET, 0);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Device reset error\n" );

	// Get buffer size
	ret = MJPEG_DriverIOCTL(MJPEG_IOR_BUFFER_SIZE, (UINT32)&ui32BufferSize);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Device buffer error\n" );
	MJPEG_CHECK_CODE( ui32BufferSize == 0, goto FUNCTION_EXIT, "App] Device buffer size error\n" );

	// For input JPEG file
	ui32FileSize = _GetFileSize( argv[1] );
	MJPEG_CHECK_CODE( ui32FileSize == 0, goto FUNCTION_EXIT, "App] File size error\n" );

	stFileStream.mem_buf.addr = (UINT32)malloc( ui32FileSize );
	MJPEG_CHECK_CODE( stFileStream.mem_buf.addr == 0, goto FUNCTION_EXIT, "App] Memory allocation error\n" );

	stFileStream.mem_buf.length = _LoadFile( argv[1], (void *)stFileStream.mem_buf.addr );
	MJPEG_CHECK_CODE( stFileStream.mem_buf.length == 0, goto FUNCTION_EXIT, "App] File read error\n" );
	stFileStream.curr_offset = 0;

	// For input stream
	stInputBuffer[0].addr = (UINT32)mmap( 0, ui32BufferSize, PROT_READ|PROT_WRITE, MAP_SHARED, g_iMJPEGDevID, 0 );
	MJPEG_CHECK_CODE( (void *)stInputBuffer[0].addr == MAP_FAILED, goto FUNCTION_EXIT, "App] Device mmap error\n" );
	stInputBuffer[0].length = MJPEG_BUFFER_SIZE;	// ui32BufferSize

	// for temporary output frame buffer
	stOutputStream.mem_buf.addr = 0x77B10000 + MJPEG_BUFFER_SIZE;
	stOutputStream.mem_buf.length = ui32BufferSize - MJPEG_BUFFER_SIZE;
	stOutputStream.curr_offset = 0;

	memset(stInputBuffer[0].addr, 0, MJPEG_BUFFER_SIZE);

	// Decode header
	do
	{
		UINT32 ui32DataSize = stFileStream.mem_buf.length - stFileStream.curr_offset;
		ui32DataSize = ( ui32DataSize > stInputBuffer[0].length )? stInputBuffer[0].length : ui32DataSize;

		memcpy( (void *)(stInputBuffer[0].addr), (void *)(stFileStream.mem_buf.addr+stFileStream.curr_offset), ui32DataSize );

		ret = MJPEG_DriverIOCTL(MJPEG_IOR_DECODE_SEQUENCE_INIT, (UINT32)&stDecSeqParam);
		MJPEG_CHECK_CODE( stDecSeqParam.ui32Length == 0, goto FUNCTION_EXIT, "App] Header info error\n" );

		stFileStream.curr_offset += stDecSeqParam.ui32Length;

	} while( ret != RET_OK );

	// Ready to decode
	memcpy( (void *)&stDecInitParam.stHeader, (void *)&stDecSeqParam.stHeader, sizeof(MJPEG_HEADER_T) );
	memcpy( (void *)&stDecInitParam.stBuffer, (void *)&stOutputStream.mem_buf, sizeof(LX_MEM_BUF_T) );
	stDecInitParam.stOption.eScaleHorizontal = 0;//MJPEG_SCALE_1of2;
	stDecInitParam.stOption.eScaleVertical= 0;//MJPEG_SCALE_1of2;
	stDecInitParam.stOption.eRotation = 0;//MJPEG_ROTATE_90;
	stDecInitParam.stOption.eMirroring = 0;//MJPEG_MIRROR_HORIZONTAL|MJPEG_MIRROR_VERTICAL;

	ret = MJPEG_DriverIOCTL(MJPEG_IOW_DECODE_INIT, (UINT32)&stDecInitParam);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Decoding init error\n" );

	// Double buffer
	if ( stFileStream.mem_buf.length - stFileStream.curr_offset > stInputBuffer[0].length )
	{
		// initialize decode input buffers - 4bytes aligned
		stInputBuffer[0].length = LX_CALC_ALIGNED_VALUE( (stInputBuffer[0].length>>1)-3, 2 );
		stInputBuffer[1].addr = stInputBuffer[0].addr + stInputBuffer[0].length;
		stInputBuffer[1].length = stInputBuffer[0].length;
	}

	ui8BufferIndex = 0;

	do
	{
		UINT32 ui32DataSize = stFileStream.mem_buf.length - stFileStream.curr_offset;
		ui32DataSize = ( ui32DataSize > stInputBuffer[ui8BufferIndex].length )? stInputBuffer[ui8BufferIndex].length : ui32DataSize;

		memcpy( (void *)(stInputBuffer[ui8BufferIndex].addr), (void *)(stFileStream.mem_buf.addr+stFileStream.curr_offset), ui32DataSize );

		stDecParam.ui32InputOffset = (ui8BufferIndex==0)? 0 : stInputBuffer[0].length;
		stDecParam.ui32InputLength = ui32DataSize;

		ret = MJPEG_DriverIOCTL(MJPEG_IOW_DECODE_START, (UINT32)&stDecParam);
		MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Decoding start error\n" );

		stFileStream.curr_offset += ui32DataSize;
		if ( stInputBuffer[1].length != 0 ) { ui8BufferIndex = (~ui8BufferIndex)&0x01; }
	} while( stFileStream.mem_buf.length - stFileStream.curr_offset > 0 );

	ret = MJPEG_DriverIOCTL(MJPEG_IOR_DECODE_STOP, (UINT32)&stStopParam);
	MJPEG_CHECK_CODE( ret != RET_OK, goto FUNCTION_EXIT, "App] Device status error\n" );
	MJPEG_CHECK_CODE( stStopParam.ui32Length == 0, goto FUNCTION_EXIT, "App] Decoding error\n" );

	// remap output buffer
	stOutputStream.curr_offset = stStopParam.ui32Offset;
	stOutputStream.mem_buf.addr = stInputBuffer[0].addr + MJPEG_BUFFER_SIZE;

	// Display Image info
	printf("Input image information\n");

	switch ( stStopParam.stImageInfo.eFormat )
	{
		case MJPEG_FORMAT_420: { printf("App] Format - 4:2:0\n"); break; }
		case MJPEG_FORMAT_422: { printf("App] Format - 4:2:2 Horizontal\n"); break; }
		case MJPEG_FORMAT_224: { printf("App] Format - 4:2:2 Vertical\n"); break; }
		case MJPEG_FORMAT_444: { printf("App] Format - 4:4:4\n"); break; }
		case MJPEG_FORMAT_400: { printf("App] Format - 4:0:0\n"); break; }
		default : { printf("App] Unknown format\n"); break; }
	}
	printf("App] Image Size : %d * %d\n", stDecSeqParam.stHeader.stFrameHeader.ui16Width, stDecSeqParam.stHeader.stFrameHeader.ui16Height);

	printf("Output image information\n");
	printf("App] Image Size : %d * %d\n", stStopParam.stImageInfo.ui16Width, stStopParam.stImageInfo.ui16Height);

	_CreateYUV(cOutputFileName, &stOutputStream, &stStopParam.stImageInfo, stStopParam.ui32Length);

	printf("App] Image byte-size : %d\n", stDecSeqParam.ui32Length);

FUNCTION_EXIT:
	if( stFileStream.mem_buf.addr != 0 ) { free((void *)stFileStream.mem_buf.addr); }
	if( stInputBuffer[0].addr != 0 ) { munmap((void *)stInputBuffer[0].addr, 0x00400000); }
	MJPEG_CloseDriverModule();
	return 0;
}

/**
 *
 *
 */
static SINT32
MJPEG_OpenDriverModule(void)
{
	/* open the MJPEG module */

	g_iMJPEGDevID = open(MJPEG_DEVICE_PATH, O_RDWR);

	/* validate the driver open */
	if( g_iMJPEGDevID == -1 )
	{
		printf("App] ERROR Could not open MJPEG device :[ %d ]\n", errno);
		return RET_ERROR;
	}
	else
	{
		printf("App] Device Open success devid:[ %d ]\n", g_iMJPEGDevID);
	}
	return RET_OK;
}

/**
 *
 *
 */
static void
MJPEG_CloseDriverModule(void)
{
	if(g_iMJPEGDevID)
	{
		close(g_iMJPEGDevID);
		g_iMJPEGDevID = 0;
	}
	return;
}

/**
 *
 *
 */
static SINT32
MJPEG_DriverIOCTL(UINT32 i_uiIOCTLNumb, UINT32 i_uiArg)
{
	if( ioctl(g_iMJPEGDevID, i_uiIOCTLNumb, i_uiArg) == 0 )
		return RET_OK;
	return RET_ERROR;
}

/**
 *
 *
 */
static UINT32
_GetFileSize(char *i_pcFileName)
{
	FILE *pfInput;
	UINT32 ui32ImageSize;

	//Open and read the input file to the input buffer
	pfInput = fopen( i_pcFileName, "rb" );
	if ( pfInput == NULL )
	{
		printf( "App] File doesn't exist : %s\n", i_pcFileName );
		return 0;
	}
	fseek( pfInput, 0, SEEK_END );
	ui32ImageSize = ftell( pfInput );
	fclose( pfInput );

	return ui32ImageSize;
}

/**
 *
 *
 */
static UINT32
_LoadFile(char *i_pcFileName, void *o_pBuffer)
{
	FILE *pfInput;
	UINT32 ui32Size;

	//Open and read the input file to the input buffer
	pfInput = fopen( i_pcFileName, "rb" );
	if ( pfInput == NULL )
	{
		printf( "App] File doesn't exist : %s\n", i_pcFileName );
		return 0;
	}
	fseek( pfInput, 0, SEEK_END );
	ui32Size = ftell( pfInput );
	fseek( pfInput, 0, SEEK_SET );

	fread( o_pBuffer, 1, ui32Size, pfInput );
	fclose( pfInput );

	return ui32Size;
}

/**
 *
 *
 */
static UINT32
_LoadInputFile(char *i_pcFileName, LX_MEM_BUF_T *pstInputStream)
{
	FILE *pfInput;
	UINT32 ui32ImageSize;

	//Open and read the input file to the input buffer
	pfInput = fopen( i_pcFileName, "rb" );
	if ( pfInput == NULL )
	{
		printf( "App] File doesn't exist : %s\n", i_pcFileName );
		return 0;
	}
	fseek( pfInput, 0, SEEK_END );
	ui32ImageSize = ftell( pfInput );
	fseek( pfInput, 0, SEEK_SET );

	if ( ui32ImageSize == 0 )
	{
		printf( "App] Input data error\n" );
		fclose( pfInput);
		return 0;
	}

	fread( (UINT8*)pstInputStream->addr, 1, ui32ImageSize, pfInput );
	fclose( pfInput );

	return ui32ImageSize;
}

static UINT32
_YUVParser(void *i_pBuffer, UINT32 i_ui32Size, MJPEG_IMAGE_INFO_T *o_pstImageInfo)
{
	UINT32 ui32DataSize = i_ui32Size - sizeof(MJPEG_IMAGE_INFO_T);

	if ( ui32DataSize < 0 )
	{
		printf( "App] Input data size error\n");
		return 0;
	}

	memcpy( (void *)o_pstImageInfo, (void *)((UINT32)i_pBuffer+ui32DataSize), sizeof(MJPEG_IMAGE_INFO_T) );

	return ui32DataSize;
}

static UINT32
_CreateJPEG(char *i_pcFileName, LX_MEM_STREAM_T *i_pstData, MJPEG_HEADER_T *i_pstHeader, UINT32 i_ui32Size)
{
	UINT32 ui32FileSize;
	FILE *pfOutput;
	UINT16 ui16Marker, ui16Length;
	UINT8 ui8Temp, ui8CodeLengths[4];
	UINT32 iCount;

	//Open and read the input file to the input buffer
	pfOutput = fopen( i_pcFileName, "wb" );
	if ( pfOutput == NULL )
	{
		printf( "App] Output file open error\n" );
		return 0;
	}

	// SOI
	ui16Marker = 0xFFD8;
	FILE_WRITE02(&ui16Marker, pfOutput );

	// App header(JFIF)
	ui16Marker = 0xFFE0;
	ui16Length = 16;
	FILE_WRITE02(&ui16Marker, pfOutput );
	FILE_WRITE02(&ui16Length, pfOutput );

	ui16Length = 0x4A46;
	FILE_WRITE02(&ui16Length, pfOutput );
	ui16Length = 0x4946;
	FILE_WRITE02(&ui16Length, pfOutput );
	ui8Temp = 0x00;
	FILE_WRITE01(&ui8Temp, pfOutput );


	ui16Length = 0x0102;
	FILE_WRITE02(&ui16Length, pfOutput );

	ui8Temp = 0x01;
	FILE_WRITE01(&ui8Temp, pfOutput );
	ui16Length = 0x0048;
	FILE_WRITE02(&ui16Length, pfOutput );
	ui16Length = 0x0048;
	FILE_WRITE02(&ui16Length, pfOutput );

	ui16Length = 0x0000;
	FILE_WRITE02(&ui16Length, pfOutput );

	// DQT
	ui16Marker = 0xFFDB;
	ui16Length = 2 + 2*(sizeof(MJPEG_QUANTIZATION_TABLE_T)-1);
	FILE_WRITE02(&ui16Marker, pfOutput );
	FILE_WRITE02(&ui16Length, pfOutput );
	ui8Temp = (i_pstHeader->stQuantiztionTables[0].ui8Pq<<4)|i_pstHeader->stQuantiztionTables[0].ui8Tq;
	FILE_WRITE01(&ui8Temp, pfOutput );
	for ( iCount = 0 ; iCount < 64 ; ++iCount )
	{
		FILE_WRITE01(&i_pstHeader->stQuantiztionTables[0].ui8Elements[iCount], pfOutput );
	}
	ui8Temp = (i_pstHeader->stQuantiztionTables[1].ui8Pq<<4)|i_pstHeader->stQuantiztionTables[1].ui8Tq;
	FILE_WRITE01(&ui8Temp, pfOutput );
	for ( iCount = 0 ; iCount < 64 ; ++iCount )
	{
		FILE_WRITE01(&i_pstHeader->stQuantiztionTables[1].ui8Elements[iCount], pfOutput );
	}

	// SOF
	ui16Marker = 0xFFC0;
	ui16Length = 2 + 6 + 3*i_pstHeader->stFrameHeader.ui8NumberOfComponents;
	FILE_WRITE02(&ui16Marker, pfOutput );
	FILE_WRITE02(&ui16Length, pfOutput );
	FILE_WRITE01(&i_pstHeader->stFrameHeader.ui8SamplingPrecision, pfOutput );
	FILE_WRITE02(&i_pstHeader->stFrameHeader.ui16Height, pfOutput );
	FILE_WRITE02(&i_pstHeader->stFrameHeader.ui16Width, pfOutput );
	FILE_WRITE01(&i_pstHeader->stFrameHeader.ui8NumberOfComponents, pfOutput );
	for ( iCount = 0 ; iCount < i_pstHeader->stFrameHeader.ui8NumberOfComponents ; ++iCount )
	{
		FILE_WRITE01(&i_pstHeader->stFrameHeader.stComponents[iCount].ui8ComponentID, pfOutput );
		ui8Temp = (i_pstHeader->stFrameHeader.stComponents[iCount].ui8HSampleFreq<<4) |
				   i_pstHeader->stFrameHeader.stComponents[iCount].ui8VSampleFreq;
		FILE_WRITE01(&ui8Temp, pfOutput );
		FILE_WRITE01(&i_pstHeader->stFrameHeader.stComponents[iCount].ui8QTableID, pfOutput );
	}

	// DRI
	ui16Marker = 0xFFDD;
	ui16Length = 2 + 2;
	FILE_WRITE02(&ui16Marker, pfOutput );
	FILE_WRITE02(&ui16Length, pfOutput );
	FILE_WRITE02(&i_pstHeader->ui16ResetInterval, pfOutput );

	// DHT
	ui16Marker = 0xFFC4;
	memset( &ui8CodeLengths, 0, 4);
	for ( iCount = 0 ; iCount < 16*4 ; ++iCount )
	{
		ui8CodeLengths[iCount/16] += i_pstHeader->stHuffmanTables[iCount/16].ui8CodeLengths[iCount%16];
	}
	ui16Length = 2 + 4*(1+16) + ui8CodeLengths[0]+ui8CodeLengths[1]+ui8CodeLengths[2]+ui8CodeLengths[3];
	FILE_WRITE02(&ui16Marker, pfOutput );
	FILE_WRITE02(&ui16Length, pfOutput );

	ui8Temp = (i_pstHeader->stHuffmanTables[0].ui8Tc<<4)|i_pstHeader->stHuffmanTables[0].ui8Th;
	FILE_WRITE01(&ui8Temp, pfOutput );
	for ( iCount = 0 ; iCount < 16 ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[0].ui8CodeLengths[iCount], pfOutput );
	for ( iCount = 0 ; iCount < ui8CodeLengths[0] ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[0].ui8CodeValues[iCount], pfOutput );

	ui8Temp = (i_pstHeader->stHuffmanTables[1].ui8Tc<<4)|i_pstHeader->stHuffmanTables[1].ui8Th;
	FILE_WRITE01(&ui8Temp, pfOutput );
	for ( iCount = 0 ; iCount < 16 ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[1].ui8CodeLengths[iCount], pfOutput );
	for ( iCount = 0 ; iCount < ui8CodeLengths[1] ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[1].ui8CodeValues[iCount], pfOutput );

	ui8Temp = (i_pstHeader->stHuffmanTables[2].ui8Tc<<4)|i_pstHeader->stHuffmanTables[2].ui8Th;
	FILE_WRITE01(&ui8Temp, pfOutput );
	for ( iCount = 0 ; iCount < 16 ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[2].ui8CodeLengths[iCount], pfOutput );
	for ( iCount = 0 ; iCount < ui8CodeLengths[2] ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[2].ui8CodeValues[iCount], pfOutput );

	ui8Temp = (i_pstHeader->stHuffmanTables[3].ui8Tc<<4)|i_pstHeader->stHuffmanTables[3].ui8Th;
	FILE_WRITE01(&ui8Temp, pfOutput );
	for ( iCount = 0 ; iCount < 16 ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[3].ui8CodeLengths[iCount], pfOutput );
	for ( iCount = 0 ; iCount < ui8CodeLengths[3] ; ++iCount )
		FILE_WRITE01(&i_pstHeader->stHuffmanTables[3].ui8CodeValues[iCount], pfOutput );
/*	Scan header was included Data
	// SOS
	ui16Marker = 0xFFDA;
	ui16Length = 2 + 1 + 2*i_pstHeader->stScanHeader.ui8NumberOfComponents + 3;
	FILE_WRITE02(&ui16Marker, pfOutput );
	FILE_WRITE02(&ui16Length, pfOutput );
	FILE_WRITE01(&i_pstHeader->stScanHeader.ui8NumberOfComponents, pfOutput );
	for ( iCount = 0 ; iCount < i_pstHeader->stScanHeader.ui8NumberOfComponents ; ++iCount )
	{
		FILE_WRITE01(&i_pstHeader->stScanHeader.stComponents[iCount].ui8ComponentID, pfOutput );
		ui8Temp = (i_pstHeader->stScanHeader.stComponents[iCount].ui8DC_HuffmanTableID<<4)
				  |i_pstHeader->stScanHeader.stComponents[iCount].ui8AC_HuffmanTableID;
		FILE_WRITE01(&ui8Temp, pfOutput );
	}
	FILE_WRITE01(&i_pstHeader->stScanHeader.ui8Ss, pfOutput );
	FILE_WRITE01(&i_pstHeader->stScanHeader.ui8Se, pfOutput );
	ui8Temp = (i_pstHeader->stScanHeader.ui8Ah<<4)|i_pstHeader->stScanHeader.ui8Al;
	FILE_WRITE01(&ui8Temp, pfOutput );
*/
	// Data
	fwrite( (void *)(i_pstData->mem_buf.addr+i_pstData->curr_offset), 1, i_ui32Size, pfOutput);

	// EOI
	ui16Marker = 0xFFD9;
	FILE_WRITE02(&ui16Marker, pfOutput );

	ui32FileSize = ftell( pfOutput );

	fclose( pfOutput );

	return ui32FileSize;
}

static UINT32
_CreateYUV(char *i_pcFileName, LX_MEM_STREAM_T *i_pstInput, MJPEG_IMAGE_INFO_T *i_pstInfo, UINT32 i_ui32Size)
{
	UINT32 ui32FileSize;
	FILE *pfOutput;

	//Open and read the input file to the input buffer
	pfOutput = fopen( i_pcFileName, "wb" );
	if ( pfOutput == NULL )
	{
		printf( "App] Output file open error\n" );
		return 0;
	}

	fwrite( (void *)(i_pstInput->mem_buf.addr+i_pstInput->curr_offset), 1, i_ui32Size, pfOutput);
	fwrite( (void *)i_pstInfo, 1, sizeof(MJPEG_IMAGE_INFO_T), pfOutput);
	ui32FileSize = ftell( pfOutput );

	fclose(pfOutput);

	return ui32FileSize;
}
