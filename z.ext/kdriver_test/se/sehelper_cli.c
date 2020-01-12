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
 * Main CLI implementation for SE device.
 *
 * author     Srinivasan Shanmugam (srinivasan.shanmugam@lge.com)
 * version    1.0
 * date       2010.05.12
 * note       Additional information.
 *
 * @addtogroup lg1150_Se
 * @{
 */


/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sehelper_cli.h"
#include "se_cfg.h"

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define CLIH_DATA_DUMP

#define SAMPLE_TS_NAME					"../demo98.trp"

#define SS_USER_INFO_SIZE				120

#define SS_KEY_INFO_SIZE				128

#define SS_MAC_LEN						32

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
void CLI_MemDump( UINT32 *i_puiBuf, UINT32 i_uiBufLen );

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/
UINT8 	*g_pcSCVirtualBaseAddr 			= NULL;

UINT8 	*g_pucSSSecBuf 					= NULL;

UINT32	g_uiSSSecInfoLen 				= 0;

UINT8	g_aucKeyInfo[ SS_KEY_INFO_SIZE ];

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/
static int	g_siSEDevId 				= -1;

static DTV_STATUS_T CLI_HLPR_SE_Mod_IOCTL( UINT32 i_uiFunID, UINT32 io_uiArg );



/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
DTV_STATUS_T CLI_HLPR_SE_Mod_IOCTL( UINT32 i_uiFunID, UINT32 io_uiArg )
{
    DTV_STATUS_T eResult = NOT_PERMITTED;

    if ( -1 == g_siSEDevId )
    {
        CLI_SE_CRITICAL( ( "CLIH SE> Dev not opened! \n" ) );
    }
	else
	{
		SINT32 iRet = -1;
		iRet = ioctl( g_siSEDevId, i_uiFunID, io_uiArg );
	    if ( !iRet )
	    {
			eResult = OK;

			CLI_SE_DEBUG_INFO( ( "CLIH SE> IOCTL S! \n" ) );
	    }
		else
		{
			CLI_SE_DEBUG_INFO( ( "CLIH SE> Fun ID - 0x%x\n", i_uiFunID ) );

			CLI_SE_CRITICAL( ( "CLIH SE> IOCTL F - %d\n", iRet ) );
		}
	}

    return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_Init( void )
{
	DTV_STATUS_T eResult = NOT_PERMITTED;

	if ( g_siSEDevId == -1 )
	{
		g_siSEDevId = open( "/dev/lg/se0", O_RDWR );
	}

	/* validate the driver open */
	if( g_siSEDevId != -1 )
	{
		g_pcSCVirtualBaseAddr = mmap( 0,
									  SE_TOTAL_PHY_BUF_SIZE,
									  PROT_WRITE|PROT_READ,
									  MAP_SHARED,
									  g_siSEDevId,
									  0 );
		if ( g_pcSCVirtualBaseAddr == ( void * )-1 )
		{
			close(g_siSEDevId);

			CLI_SE_CRITICAL( ( "CLIH SE> Mmap failed!!!!\n" ) );

			return eResult;
		}

		CLI_SE_DEBUG_INFO( ( "\n\n CLIH-SE> user space address 0x%x \n\n", (UINT32)g_pcSCVirtualBaseAddr ) );

		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_INITIALIZE, 0 );
		if ( ISDTVSUCCESS( eResult ) )
		{
			FILE *fp = NULL;

			fp = fopen( SAMPLE_TS_NAME,"rb" );
			if ( fp )
			{
				SINT32 iLen = 0;
				SINT32 iLpCnt = 0;

				iLen = fread( g_pcSCVirtualBaseAddr, 1 , SE_CRYPTO_BUF_SIZE , fp );

				CLI_SE_DEBUG_INFO( ( "CLIH-SE> Len read %d \n", iLen ) );

				for (  ; iLpCnt < 30; iLpCnt++ )
				{
					CLI_SE_DEBUG_INFO( ( " 0x%x \t", *( g_pcSCVirtualBaseAddr + iLpCnt ) ) );

					if ( !( iLpCnt % 9 ) && ( iLpCnt != 0 ) )
					{
						CLI_SE_DEBUG_INFO( ( "\n" ) );
					}
				}

				CLI_SE_DEBUG_INFO( ( "\n" ) );

				fclose( fp );
			}
			else
			{
				CLI_SE_DEBUG_INFO( ( "\n CLIH-SE> fopen failed! \n" ) );
			}
		}
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH SE> Dev not opened!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_UnInit( void )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_FINALIZE, 0 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_UnInit S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_UnInit F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SC_SW_Reset( void )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_SC_SW_RESET, 0 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_SC_SW_Reset S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_SC_SW_Reset F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_GetSCAccess( UINT32 i_uiAccessType )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( 0 == i_uiAccessType )
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_GET_MASTERSHIP_FOR_SC, 0 );
	}
	else
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_GET_SINGLE_USE_FOR_SC, 0 );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_GetSCAccess S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_GetSCAccess F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_ReleaseSCAccess( )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_RELEASE_SINGLE_USE, 0 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_ReleaseSCAccess S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_ReleaseSCAccess F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_PrintDMAState( UINT32 i_uiChannel )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	LX_SE_DMA_STATUS_T stDMAStatus;

	stDMAStatus.channel = i_uiChannel;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_READ_DMA_STATUS, ( UINT32 )&stDMAStatus );

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "Ch - %d\nRemaining cnt - %d\nBuf state - 0x%x\n",
						stDMAStatus.channel, stDMAStatus.resCount, stDMAStatus.bufState ) );

		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_PrintDMAStatus S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_PrintDMAStatus F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_PrintDMABusyStatus( UINT32 i_uiChannel )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	UINT32 uiStatus;

	if ( 0 == i_uiChannel )
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_IS_DMA_CHA_BUSY, ( UINT32 )&uiStatus );
	}
	else
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_IS_DMA_CHB_BUSY, ( UINT32 )&uiStatus );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "Ch - %d\tBusy status - %d\n", i_uiChannel, uiStatus ) );

		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_PrintDMABusyStatus S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_PrintDMABusyStatus F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_PrintJobStatus( UINT32 i_uiChannel )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	UINT32 uiStatus;

	if ( 0 == i_uiChannel )
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_ISDMA_CHA_JOB_DONE, ( UINT32 )&uiStatus );
	}
	else
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_ISDMA_CHB_JOB_DONE, ( UINT32 )&uiStatus );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "Ch - %d\tJob status - %d\n", i_uiChannel, uiStatus ) );

		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_PrintJobStatus S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_PrintJobStatus F!\n" ) );
	}

	return eResult;
}

void CLI_HLPR_SE_StartDMAs()
{
	//CLI_HLPR_SE_Mod_IOCTL( SE_IO_START_DMA, 0 );
	return;
}

DTV_STATUS_T CLI_HLPR_SE_DoCryptoOp( LX_SE_CRYPTO_CNTL_DATA_T * i_pstCryptoInfo, BOOLEAN i_bStartSC )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( i_pstCryptoInfo )
	{
		if ( i_bStartSC	)
		{
			eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_DO_CRYPTO_OP, ( UINT32 )i_pstCryptoInfo );
		}
		else
		{
			eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_SET_CNTL_DATA, ( UINT32 )i_pstCryptoInfo );
		}

		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_DoCryptoOp S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_DoCryptoOp F!\n" ) );
		}
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_DoCryptoOp F! - inv arg!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SetKey( LX_SE_DMA_CHANNEL_TYPE_T i_eChannel, UINT8 *i_pucKeyData, UINT32 i_ucKeySize )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	LX_SE_KEY_DATA_T stKeyData;

	stKeyData.channel = i_eChannel;
	stKeyData.tsOddMode = eEVEN_KEY;

	stKeyData.key.bufAddr = i_pucKeyData;

	stKeyData.key.length = i_ucKeySize;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOW_SET_KEYDATA, ( UINT32 )&stKeyData );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_SetKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_SetKey F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SetIVData( LX_SE_DMA_CHANNEL_TYPE_T i_eChannel, UINT8 *i_pucIVData, UINT32 i_ucIVDataSize )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	LX_SE_IV_DATA_T stIVData;

	stIVData.channel = i_eChannel;

	stIVData.ivData.bufAddr = i_pucIVData;

	stIVData.ivData.length = i_ucIVDataSize;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOW_SET_IVDATA, ( UINT32 )&stIVData );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_SetIVData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_SetIVData F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_Test_PRNG_Fn( )
{
	DTV_STATUS_T eResult = NOT_OK;

	#define LEN 		32

	UINT8 auiKeyBuf[ LEN ];

	LX_SE_RANDOM_NUM_T stRandomKey;

	memset( auiKeyBuf, 0, LEN );

	stRandomKey.randomNum.bufAddr = auiKeyBuf;

	stRandomKey.randomNum.length = LEN;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_GET_RANDOM_NUMBER, ( UINT32 ) &stRandomKey );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_SS_Test_PRNG_Fn S!\n" ) );

		CLI_SE_DEBUG_INFO( ( "Random number ...\n" ) );

		CLI_MemDump( ( UINT32 * )( auiKeyBuf ), ( LEN / 4 ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_SS_Test_PRNG_Fn F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_EncryptUsrInfo( UINT8* i_pucFileName, UINT32 i_uiChannel )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( i_pucFileName && g_pcSCVirtualBaseAddr )
	{
		LX_SE_SS_CONTAINER_T stUsrInfo;

		CLI_SE_DEBUG_INFO( ( "\n\n\n\nCLIH-SE> SS Enc src addr(g_pcSCVirtualBaseAddr) 0x%x \n\n",
															(UINT32)g_pcSCVirtualBaseAddr ) );

		CLI_MemDump( ( UINT32 * )g_pcSCVirtualBaseAddr, ( SS_USER_INFO_SIZE / 4 ) );

		stUsrInfo.channel = i_uiChannel;

		stUsrInfo.decryptedUserData.offset = 0;
		stUsrInfo.decryptedUserData.length = SS_USER_INFO_SIZE;

		stUsrInfo.decryptedMAC.offset = 0;
		stUsrInfo.decryptedMAC.length = SS_USER_INFO_SIZE;

		stUsrInfo.encryptedUserData.offset = SS_USER_INFO_SIZE;
		stUsrInfo.encryptedUserData.length = SS_USER_INFO_SIZE;

		stUsrInfo.encryptedMAC.offset = 2 * SS_USER_INFO_SIZE;
		stUsrInfo.encryptedMAC.length = SS_MAC_LEN;

		stUsrInfo.keyInfo.bufAddr = g_aucKeyInfo;
		stUsrInfo.keyInfo.length = SS_KEY_INFO_SIZE;

		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_SS_ENC_USER_INFO, ( UINT32 ) &stUsrInfo );
		if ( ISDTVSUCCESS( eResult ) )
		{
			FILE *fp = NULL;

			CLI_SE_DEBUG_INFO( ( "CLIH SE> Enc usr info - s! \n" ) );

			fp = fopen( ( char * )i_pucFileName, "w+" );
			if ( fp )
			{
				SINT32 iTmp = -1;

				iTmp = fwrite( g_aucKeyInfo, 1, SS_KEY_INFO_SIZE, fp );
				if ( iTmp != SS_KEY_INFO_SIZE )
				{
					CLI_SE_DEBUG_INFO( ( "CLIH SE> CLI_HLPR_SE_SS_EncryptUsrInfo Err! \n" ) );

					eResult = NOT_OK;
				}

				fclose( fp );
			}

			CLI_SE_DEBUG_INFO( ( "Enc usr info...\n" ) );

			CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + SS_USER_INFO_SIZE ), ( SS_USER_INFO_SIZE / 4 ) );

			CLI_SE_DEBUG_INFO( ( "Enc MAC info...\n" ) );

			CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 2 * SS_USER_INFO_SIZE ) ), ( SS_MAC_LEN / 4 ) );

			CLI_SE_DEBUG_INFO( ( "Key info...\n" ) );

			CLI_MemDump( ( UINT32 * )g_aucKeyInfo, ( SS_KEY_INFO_SIZE / 4 ) );
		}
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_DecryptUsrInfo( UINT8* i_pucFileName, UINT32 i_uiChannel )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( i_pucFileName && g_pcSCVirtualBaseAddr )
	{
		FILE *fp = NULL;

		LX_SE_SS_CONTAINER_T stUsrInfo;

		CLI_SE_DEBUG_INFO( ( "\n\n\n\nCLIH-SE> SS Dec src addr(g_pcSCVirtualBaseAddr) 0x%x \n\n",
															(UINT32)g_pcSCVirtualBaseAddr ) );

		fp = fopen( ( char * )i_pucFileName, "r+" );
		if ( fp )
		{
			SINT32 iTmp = -1;

			iTmp = fread( g_aucKeyInfo, 1, SS_KEY_INFO_SIZE, fp );
			if ( iTmp != SS_KEY_INFO_SIZE )
			{
				CLI_SE_DEBUG_INFO( ( "CLIH SE> CLI_HLPR_SE_SS_DecryptUsrInfo Err! \n" ) );

				eResult = NOT_OK;
			}

			fclose( fp );
		}

		stUsrInfo.keyInfo.bufAddr = g_aucKeyInfo;
		stUsrInfo.keyInfo.length = SS_KEY_INFO_SIZE;

		stUsrInfo.channel = i_uiChannel;

		stUsrInfo.encryptedUserData.offset = SS_USER_INFO_SIZE;
		stUsrInfo.encryptedUserData.length = SS_USER_INFO_SIZE;

		stUsrInfo.encryptedMAC.offset = 2 * SS_USER_INFO_SIZE;
		stUsrInfo.encryptedMAC.length = SS_MAC_LEN;

		stUsrInfo.decryptedUserData.offset = 0;
		stUsrInfo.decryptedUserData.length = SS_USER_INFO_SIZE;

		stUsrInfo.decryptedMAC.offset = 3 * SS_USER_INFO_SIZE;
		stUsrInfo.decryptedMAC.length = SS_MAC_LEN;

		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_SS_DEC_USER_INFO, ( UINT32 ) &stUsrInfo );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_DEBUG_INFO( ( "CLIH SE> Dec usr info - s! \n" ) );

			if ( !memcmp( ( g_pcSCVirtualBaseAddr + ( 2 * SS_USER_INFO_SIZE ) ),
						  ( g_pcSCVirtualBaseAddr + ( 3 * SS_USER_INFO_SIZE ) ),
						  SS_MAC_LEN ) )
			{
				CLI_SE_INFO( ( "\n\nCLIH SE> CLI_HLPR_SE_SS_DecryptUsrInfo S! \n\n" ) );
			}
			else
			{
				eResult = NOT_OK;

				CLI_SE_CRITICAL( ( "CLIH SE> CLI_HLPR_SE_SS_DecryptUsrInfo f! \n" ) );
			}

			CLI_SE_DEBUG_INFO( ( "Dec usr info...\n" ) );

			CLI_MemDump( ( UINT32 * )g_pcSCVirtualBaseAddr, ( SS_USER_INFO_SIZE / 4 ) );

			CLI_SE_DEBUG_INFO( ( "Enc MAC info...\n" ) );

			CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 2 * SS_USER_INFO_SIZE ) ), ( SS_MAC_LEN / 4 ) );

			CLI_SE_DEBUG_INFO( ( "new Enc MAC info...\n" ) );

			CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 3 * SS_USER_INFO_SIZE ) ), ( SS_MAC_LEN / 4 ) );
		}
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_EncKey( )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	LX_SE_SS_KEYS_T stKey;

	#define KEYLEN		32

	UINT8 aucUsrKey[KEYLEN];

	UINT8 aucMACKey[KEYLEN];

	memset( aucUsrKey, 0x3, KEYLEN );

	memset( aucMACKey, 0xA, KEYLEN );

	stKey.key4UserData.bufAddr = aucUsrKey;
	stKey.key4UserData.length = KEYLEN;


	stKey.key4MACData.bufAddr = aucMACKey;
	stKey.key4MACData.length = KEYLEN;

	stKey.keyInfo.bufAddr = g_aucKeyInfo;
	stKey.keyInfo.length = SS_KEY_INFO_SIZE;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_SP_ENC_KEY, ( UINT32 ) &stKey );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_EncKey <<S>>\n" ) );

		CLI_SE_DEBUG_INFO( ( "Enc Key info...\n" ) );

		CLI_MemDump( ( UINT32* )g_aucKeyInfo, ( stKey.keyInfo.length / 4 ) );
	}
	else
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_ENCKey <<F>>\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_DecKey( )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	LX_SE_SS_KEYS_T stKey;

	#define KEYLEN1		32

	UINT8 aucUsrKey[KEYLEN1];

	UINT8 aucMACKey[KEYLEN1];

	stKey.key4UserData.bufAddr = aucUsrKey;
	stKey.key4UserData.length = KEYLEN1;

	stKey.key4MACData.bufAddr = aucMACKey;
	stKey.key4MACData.length = KEYLEN1;

	stKey.keyInfo.bufAddr = g_aucKeyInfo;
	stKey.keyInfo.length = SS_KEY_INFO_SIZE;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_SP_DEC_KEY, ( UINT32 ) &stKey );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_DecKey <<S>>\n" ) );

		CLI_SE_DEBUG_INFO( ( "usr Key ...\n" ) );

		CLI_MemDump( ( UINT32 * )aucUsrKey, ( KEYLEN1 / 4 ) );

		CLI_SE_DEBUG_INFO( ( "MAC Key ...\n" ) );

		CLI_MemDump( ( UINT32 * )aucMACKey, ( KEYLEN1 / 4 ) );
	}
	else
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_DecKey <<F>>\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_GenMAC( UINT32 i_uiChannel, BOOLEAN i_bAutoKey )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	LX_SE_MAC_T stMAC;

	UINT8 auiKey[32];

	CLI_MemDump( ( UINT32 * )g_pcSCVirtualBaseAddr, ( SS_USER_INFO_SIZE / 4 ) );

	stMAC.channel = i_uiChannel;

	stMAC.userData.offset = 0;
	stMAC.userData.length = SS_USER_INFO_SIZE;

	stMAC.mac.offset = 2 * SS_USER_INFO_SIZE;
	stMAC.mac.length = SS_MAC_LEN;

	stMAC.key.bufAddr = auiKey;
	stMAC.key.length = 32;

	if ( i_bAutoKey )
	{
		stMAC.genKey = TRUE;

		memset( auiKey, 0, 32 );
	}
	else
	{
		stMAC.genKey = FALSE;

		memset( auiKey, 0x31, 32 );
	}

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_SS_GEN_MAC, ( UINT32 ) &stMAC );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_GenMAC <<S>>\n" ) );

		CLI_SE_DEBUG_INFO( ( "MAC ...\n" ) );

		CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 2 * SS_USER_INFO_SIZE ) ), ( SS_MAC_LEN / 4 ) );

		CLI_SE_DEBUG_INFO( ( "MAC Key ...\n" ) );

		CLI_MemDump( ( UINT32 * )auiKey , 8 );
	}
	else
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_GenMAC <<F>>\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_GenKey( LX_SE_KEY_CHAIN_T *i_pstKeyChain, BOOLEAN i_bIsGenNSet )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( i_pstKeyChain )
	{
		if ( i_bIsGenNSet )
		{
			eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_SS_GENERATE_AND_SET_KEY, ( UINT32 ) i_pstKeyChain );
		}
		else
		{
			eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOR_SS_GENERATE_KEY, ( UINT32 ) i_pstKeyChain );
		}
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_GenKey <<S>>\n" ) );

		CLI_SE_DEBUG_INFO( ( "Key ...\n" ) );

		CLI_MemDump( ( UINT32 * )( i_pstKeyChain->protectedKey.data.bufAddr ),
								( i_pstKeyChain->protectedKey.data.length / 4 ) );
	}
	else
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_GenKey <<F>>\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_SetProtKey( LX_SE_KEY_CHAIN_T *i_pstKeyChain )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( i_pstKeyChain )
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOW_SET_PROTECTED_KEY, ( UINT32 ) i_pstKeyChain );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_SetProtKey <<S>>\n" ) );
	}
	else
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_SetProtKey <<F>>\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_SS_SetProtIV( LX_SE_KEY_CHAIN_T *i_pstIV )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( i_pstIV )
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IOW_SET_PROTECTED_IV, ( UINT32 ) i_pstIV );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_SetProtIV <<S>>\n" ) );
	}
	else
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_SetProtIV <<F>>\n" ) );
	}

	return eResult;
}


DTV_STATUS_T CLI_HLPR_SE_RunCrypto( LX_SE_CONTENTS_T *i_pstContnt )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( i_pstContnt )
	{
		eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IORW_RUN, ( UINT32 ) i_pstContnt );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_RunCrypto <<S>>\n" ) );
	}
	else
	{
		CLI_SE_DEBUG_INFO( ( "CLI> SS_RunCrypto <<F>>\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_PrintSCRegDetails( UINT32 i_uiChannel )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_PRINT_SC_REG_DETAILS, ( UINT32 )&i_uiChannel );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_PrintSCRegDetails S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_PrintSCRegDetails F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_PrintSPRegDetails( )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_Mod_IOCTL( SE_IO_PRINT_SP_REG_DETAILS, ( UINT32 ) 0 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLIH-SE> CLI_HLPR_SE_PrintSPRegDetails S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH-SE> CLI_HLPR_SE_PrintSPRegDetails F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_HLPR_SE_MemCmp( UINT8 *i_ucScrBuf, UINT8 *i_ucDstBuf, UINT32 i_uiLen )
{
#if 0
	if ( memcmp( i_ucScrBuf, i_ucDstBuf, i_uiLen ) )
	{
		CLI_SE_CRITICAL( ( "CLIH SE> CLI_HLPR_SE_MemCmp 0x%x src and 0x%x dst buf r DIFF! \n",
														(UINT32)i_ucScrBuf, (UINT32)i_ucDstBuf ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLIH SE> CLI_HLPR_SE_MemCmp 0x%x src and 0x%x buf r SAME! \n",
														(UINT32)i_ucScrBuf, (UINT32)i_ucDstBuf ) );
	}
#else
	SINT32 iLpCnt = 0;

	BOOLEAN bFlag = TRUE;

	for ( ; iLpCnt < i_uiLen; iLpCnt++ )
	{
		if ( i_ucScrBuf[ iLpCnt ] != i_ucDstBuf[ iLpCnt ] )
		{
		 	bFlag = FALSE;

			CLI_SE_CRITICAL( ( "CLIH SE> CLI_HLPR_SE_MemCmp 0x%x src and 0x%x dst buf r DIFF at position %d \n",
														(UINT32)i_ucScrBuf, (UINT32)i_ucDstBuf , iLpCnt ) );
			break;
		}
	}

	if ( bFlag )
	{
		CLI_SE_CRITICAL( ( "CLIH SE> CLI_HLPR_SE_MemCmp 0x%x src and 0x%x buf r SAME! \n",
													(UINT32)i_ucScrBuf, (UINT32)i_ucDstBuf ) );
	}
#endif /* #if 0 */

	return OK;
}

void CLI_MemDump( UINT32 *i_puiBuf, UINT32 i_uiBufLen )
{

#ifdef CLIH_DATA_DUMP

	SINT32 iLpCnt = 0;

	CLI_SE_DEBUG_INFO( ( "\n\nCLIH> CLI_MemDump starts \n" ) );

	for ( ; iLpCnt < i_uiBufLen; iLpCnt++ )
	{
		CLI_SE_DEBUG_INFO( ( "Buf[%d] -> 0x%x\n", iLpCnt, i_puiBuf[ iLpCnt ] ) );
	}

	CLI_SE_DEBUG_INFO( ( "CLIH> CLI_MemDump ends \n\n" ) );

#endif /* CLIH_DATA_DUMP */

}

/**  @} */
