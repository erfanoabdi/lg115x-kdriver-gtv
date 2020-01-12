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
 * CLI implementation for SE device.
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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>
#include "cli/cli_arg.h"
#include "se_cli.h"
#include "sehelper_cli.h"
#include "se_cfg.h"


/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/
#define SET_CRYPT_MEM_1		0x1
#define SET_CRYPT_MEM_2		0x2
#define SET_CRYPT_MEM_3		0x4
#define SET_CRYPT_MEM_4		0x8
#define SET_CRYPT_MEM_5		0x10
#define SET_CRYPT_MEM_6		0x20
#define SET_CRYPT_MEM_7		0x40

#define SET_CRYPT_MEM_8		0x80
#define SET_CRYPT_MEM_9		0x100
#define SET_CRYPT_MEM_10	0x200
#define SET_CRYPT_MEM_11	0x400
#define SET_CRYPT_MEM_12	0x800
#define SET_CRYPT_MEM_13	0x1000
#define SET_CRYPT_MEM_14	0x2000
#define SET_CRYPT_MEM_15	0x4000
#define SET_CRYPT_MEM_16	0x8000
#define SET_CRYPT_MEM_17	0x10000
#define SET_CRYPT_MEM_ALL	0x1FFFF

#define SS_MAX_KEY_SIZE		32

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
extern UINT8 	*g_pcSCVirtualBaseAddr;

/*----------------------------------------------------------------------------------------
 *   global Functions
 *---------------------------------------------------------------------------------------*/
void 				CLI_SE_Start( void );

extern void CLI_MemDump( UINT32 *i_puiBuf, UINT32 i_uiBufLen );

/*----------------------------------------------------------------------------------------
 *   global Variables
 *---------------------------------------------------------------------------------------*/
UINT8 						g_saucKeyBuf[ 48 ] = { 1, 2, 3, 4, 5, 6, 7, 8 };

UINT8 						g_saucIVData[ 16 ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6 };

BOOLEAN						g_sbIsCryptDetailsSet = FALSE;

LX_SE_CRYPTO_CNTL_DATA_T 	g_stCryptInfo;

LX_SE_KEY_CHAIN_T			g_stSSKeyChain;

UINT8						g_aucSSGenKeyBuf[ SS_MAX_KEY_SIZE ];

UINT8						g_aucSSProtIVBuf[ SS_MAX_KEY_SIZE ];

UINT8						g_aucCryptArrCntr = 0;

/*----------------------------------------------------------------------------------------
 *   Static Function Prototypes Declarations
 *---------------------------------------------------------------------------------------*/
static DTV_STATUS_T CLI_SE_GenNSetKey( UINT8 i_uiCryptCntr );

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/


/*========================================================================================
 *   Implementation Group
 *=======================================================================================*/
void CLI_SE_CryptDet()
{
	CLI_SE_HELP ( ( " 1.  Channel			- eCHANNEL_A( 0 ), eCHANNEL_B( 1 ) \n" ) );
	CLI_SE_HELP ( ( " 2.  Enc or Dec			- eENCRYPTION( 0 ), eDecryption( 1 ) \n" ) );
	CLI_SE_HELP ( ( " 3.  Algrm type			- eByPass( 0 ), eDES( 1 ), eTDES( 2 ), eAES( 3 ), eMultiSwap( 4 ),"
												" eRC4( 5 ), eSHA( 6 ), eHMAC_SHA( 7 ) \n" ) );
	CLI_SE_HELP ( ( " 4.  Residue mode		- eClear( 0 ), eCTS( 1 ), eOFB( 2 ), eSCTE( 3 ), "
												"ePaddingEnable( 4 ), ePaddingDisable( 5 ), eNoRes( 6 ) \n" ) );
	CLI_SE_HELP ( ( " 5.  Block mode			- eECB( 0 ), eCBC( 1 ), eCTR( 2 ), eOMAC1( 3 ), eSHA_1( 4 ),"
												" eSHA_256( 5 ), eHMAC_SHA_1( 6 ), eHMAC_SHA_256( 7 )\n" ) );
	CLI_SE_HELP ( ( " 6.  TS Odd mode		- eEVEN_KEY( 0 ), eODD_KEY( 1 ) \n" ) );
	CLI_SE_HELP ( ( " 7.  TS format			- eNotMPEG2TS( 0 ), eMPEG2TS_188( 1 ), eMPEG2TS_192( 2 ) \n" ) );
	CLI_SE_HELP ( ( " 8.  Src type			- eBLOCK_CIPHER_TYPE( 0 ), eSHA_TYPE( 1 ) \n" ) );
	CLI_SE_HELP ( ( " 9.  Load len			- \n" ) );
	CLI_SE_HELP ( ( " 10. Skip len			- \n" ) );
	CLI_SE_HELP ( ( " 11. Src Addr			- \n" ) );
	CLI_SE_HELP ( ( " 12. Src len			- \n" ) );
	CLI_SE_HELP ( ( " 13. Dst Addr			- \n" ) );
	CLI_SE_HELP ( ( " 14. Dst len			- \n" ) );
	CLI_SE_HELP ( ( " 15. Keysize(bytes)		- 0 to 48 bytes \n" ) );
	CLI_SE_HELP ( ( " 16. Seq src len		- \n" ) );
	CLI_SE_HELP ( ( " 17. Seq src stat		- \n" ) );
}

void CLI_SE_SetDefaultCryptoInfo4BlkType( )
{
	g_stCryptInfo.channel = eCHANNEL_A;

	g_stCryptInfo.operation = eENCRYPTION;

	g_stCryptInfo.cryptoType = eAES;

	g_stCryptInfo.blkMode = eECB;

	g_stCryptInfo.resMode = eCTS;

//	g_stCryptInfo.tsOddMode = eEVEN_KEY;

	g_stCryptInfo.tsFormat = eMPEG2TS_188;

	g_stCryptInfo.srcType = eBLOCK_CIPHER_TYPE;

	g_stCryptInfo.processLoadLength = 184;

	g_stCryptInfo.processSkipLength = 4;

	g_stCryptInfo.srcBuf.offset = SE_ENC_SRC_OFF;

	g_stCryptInfo.srcBuf.length = SE_CRYPTO_BUF_SIZE;

	g_stCryptInfo.destBuf.offset = SE_ENC_DST_OFF;

	g_stCryptInfo.destBuf.length = SE_CRYPTO_BUF_SIZE;

	g_stCryptInfo.keyLength = 16;

	g_stCryptInfo.seqSrcLength = 0;

	g_stCryptInfo.seqSrcState = 0;
}

void CLI_SE_SetDefaultCryptoInfoForSHAType(  )
{
	g_stCryptInfo.channel = eCHANNEL_A;

	g_stCryptInfo.operation = eENCRYPTION;

	g_stCryptInfo.cryptoType = eSHA;

	g_stCryptInfo.blkMode = eSHA_1;

	g_stCryptInfo.resMode = ePADDING_ENABLE;

//	g_stCryptInfo.tsOddMode = eEVEN_KEY;

	g_stCryptInfo.tsFormat = eNOT_MPEG2TS;

	g_stCryptInfo.srcType = eSHA_TYPE;

	g_stCryptInfo.processSkipLength = 0;

	g_stCryptInfo.srcBuf.offset = SE_ENC_SRC_OFF;

	g_stCryptInfo.srcBuf.length = SE_CRYPTO_BUF_SIZE;

	g_stCryptInfo.processLoadLength = g_stCryptInfo.srcBuf.length;

	g_stCryptInfo.destBuf.offset = SE_ENC_DST_OFF;

	g_stCryptInfo.destBuf.length = 20;

	g_stCryptInfo.keyLength = 0;

	g_stCryptInfo.seqSrcLength = 0;

	g_stCryptInfo.seqSrcState = 0x00000101;
}


void CLI_SE_SetCryptoInfo( UINT32 i_uiDispMem )
{
	UINT32	uiTmpVar = 0;

	if ( i_uiDispMem == SET_CRYPT_MEM_ALL )
	{
		printf(" Provide inputs for the following..\n" );
		printf(" ----------------------------------" );
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_1 )
	{
		printf("\n Feed Channel(0->ChanA / 1->ChanB):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 1 )
		{
			g_stCryptInfo.channel = ( LX_SE_DMA_CHANNEL_TYPE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_2 )
	{
		printf("\n Feed Enc or Dcr(0->Encryption / 1->Decryption):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 1 )
		{
			g_stCryptInfo.operation = ( LX_SE_CRYPTO_OPERATION_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_3 )
	{
		printf("\n Feed Crypto type(0->ByPass/1->DES/2->TDES/3->AES/4->MS/5->RC4/6->SHA/7->HMAC_SHA):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 7 )
		{
			g_stCryptInfo.cryptoType = ( LX_SE_CRYPTO_TYPE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_4 )
	{
		printf("\n Feed Res mode(0->Clear/1->CTS/2->OFB/3->SCTE/4->PadEnbl/5->PadDsbl/6->NoRes):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 6 )
		{
			g_stCryptInfo.resMode = ( LX_SE_CRYPTO_RES_MODE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_5 )
	{
		printf("\n Feed Blk mode(0->ECB/1->CBC/2->CTR/3->OMAC1/4->SHA_1/5->SHA_256/6->HMAC_SHA_1/7->HMAC_SHA_256/8->NoBlk):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 8 )
		{
			g_stCryptInfo.blkMode = ( LX_SE_CRYPTO_BLK_MODE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_6 )
	{
		printf("\n Feed TS Odd mode(0->EvenKeyMode/1->OddKeyMode):" );
		scanf("%d", &uiTmpVar );
#if 0
		if ( uiTmpVar <= 1 )
		{
			g_stCryptInfo.tsOddMode = ( LX_SE_TS_ODD_MODE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
#endif /* #if 0 */
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_7 )
	{
		printf("\n Feed TS format(0->NotMPEG2TS/1->MPEG2TS_188/2->MPEG2TS_192):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 2 )
		{
			g_stCryptInfo.tsFormat = ( LX_SE_TS_FORMAT_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_8 )
	{
		printf("\n Feed Src Tpye(0->DMALenIsMoreThanPULen/1->PULenIsMoreThanDMALen):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 1 )
		{
			g_stCryptInfo.srcType = ( LX_SE_SOURCE_TYPE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_9 )
	{
		printf("\n Feed Load len:");
		scanf("%d", &g_stCryptInfo.processLoadLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_10 )
	{
		printf("\n Feed skip len:");
		scanf("%d", &g_stCryptInfo.processSkipLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_11 )
	{
		printf("\n Feed Src offset:");
		scanf("%X", &g_stCryptInfo.srcBuf.offset);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_12 )
	{
		printf("\n Feed Src len:");
		scanf("%d", &g_stCryptInfo.srcBuf.length );
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_13 )
	{
		printf("\n Feed Dst offset:");
		scanf("%X", &g_stCryptInfo.destBuf.offset);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_14 )
	{
		printf("\n Feed Dst len:");
		scanf("%d", &g_stCryptInfo.destBuf.length);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_15 )
	{
		printf("\n Feed Keysize(bytes):");
		scanf("%d", &g_stCryptInfo.keyLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_16 )
	{
		printf("\n Feed seq src len:");
		scanf("%d", &g_stCryptInfo.seqSrcLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_17 )
	{
		printf("\n Feed seq src stat:");
		scanf("%d", &g_stCryptInfo.seqSrcState);
	}

	if ( i_uiDispMem == SET_CRYPT_MEM_ALL )
	{
		printf("\n ----------------------------------\n" );
	}
}

void CLI_SE_UpdateCryptoInfo()
{
	UINT32 uiTmpVar = 0;

	CLI_SE_DEBUG_INFO ( ( "------------CryptoInfo-----------------\n" ) );
	CLI_SE_CryptDet();
	CLI_SE_DEBUG_INFO ( ( " 18 ---------------------------> to quit \n" ) );

	while ( 1 )
	{
		printf( "\nEnter ur choice:" );
		scanf("%d", &uiTmpVar );

		if ( 18 == uiTmpVar )
		{
			break;
		}

		switch( uiTmpVar)
		{
			case 1:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_1 );
				break;

			case 2:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_2 );
				break;

			case 3:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_3 );
				break;

			case 4:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_4 );
				break;

			case 5:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_5 );
				break;

			case 6:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_6 );
				break;

			case 7:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_7 );
				break;

			case 8:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_8 );
				break;

			case 9:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_9 );
				break;

			case 10:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_10 );
				break;

			case 11:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_11 );
				break;

			case 12:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_12 );
				break;

			case 13:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_13 );
				break;

			case 14:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_14 );
				break;

			case 15:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_15 );
				break;

			case 16:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_16 );
				break;

			case 17:
				CLI_SE_SetCryptoInfo( SET_CRYPT_MEM_17 );
				break;

			default:
				CLI_SE_DEBUG_INFO( ( "Wrong request!\n" ) );
				break;
		}
	}
}

void CLI_SE_KeyChainDet()
{
	CLI_SE_HELP ( ( " 1.  Channel			- eCHANNEL_A( 0 ), eCHANNEL_B( 1 ) \n" ) );
	CLI_SE_HELP ( ( " 2.  Enc or Dec			- eENCRYPTION( 0 ), eDecryption( 1 ) \n" ) );
	CLI_SE_HELP ( ( " 3.  Algrm type			- eByPass( 0 ), eDES( 1 ), eTDES( 2 ), eAES( 3 ), eMultiSwap( 4 ),"
												" eRC4( 5 ), eSHA( 6 ), eHMAC_SHA( 7 ) \n" ) );
	CLI_SE_HELP ( ( " 4.  Residue mode		- eClear( 0 ), eCTS( 1 ), eOFB( 2 ), eSCTE( 3 ), "
												"ePaddingEnable( 4 ), ePaddingDisable( 5 ), eNoRes( 6 ) \n" ) );
	CLI_SE_HELP ( ( " 5.  Block mode			- eECB( 0 ), eCBC( 1 ), eCTR( 2 ), eOMAC1( 3 ), eSHA_1( 4 ),"
												" eSHA_256( 5 ), eHMAC_SHA_1( 6 ), eHMAC_SHA_256( 7 )\n" ) );
	CLI_SE_HELP ( ( " 6.  TS format			- eNotMPEG2TS( 0 ), eMPEG2TS_188( 1 ), eMPEG2TS_192( 2 ) \n" ) );
	CLI_SE_HELP ( ( " 7.  Src type			- eBLOCK_CIPHER_TYPE( 0 ), eSHA_TYPE( 1 ) \n" ) );
	CLI_SE_HELP ( ( " 8.  Load len			- \n" ) );
	CLI_SE_HELP ( ( " 9.  Skip len			- \n" ) );
	CLI_SE_HELP ( ( " 10. Keysize(bytes)		- 0 to 48 bytes \n" ) );
	CLI_SE_HELP ( ( " 11. Seq src len		- \n" ) );
	CLI_SE_HELP ( ( " 12. Seq src stat		- \n" ) );
}

void CLI_SE_SetKeyChainInfo( UINT32 i_uiDispMem )
{
	UINT32	uiTmpVar = 0;

	if ( i_uiDispMem == SET_CRYPT_MEM_ALL )
	{
		printf(" Provide inputs for the following..\n" );
		printf(" ----------------------------------" );
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_1 )
	{
		printf("\n Feed Channel(0->ChanA / 1->ChanB):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 1 )
		{
			g_stSSKeyChain.channel = ( LX_SE_DMA_CHANNEL_TYPE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_2 )
	{
		printf("\n Feed Enc or Dcr(0->Encryption / 1->Decryption):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 1 )
		{
			g_stSSKeyChain.operation = ( LX_SE_CRYPTO_OPERATION_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_3 )
	{
		printf("\n Feed Crypto type(0->ByPass/1->DES/2->TDES/3->AES/4->MS/5->RC4/6->SHA/7->HMAC_SHA):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 7 )
		{
			g_stSSKeyChain.cryptoType = ( LX_SE_CRYPTO_TYPE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_4 )
	{
		printf("\n Feed Res mode(0->Clear/1->CTS/2->OFB/3->SCTE/4->PadEnbl/5->PadDsbl/6->NoRes):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 6 )
		{
			g_stSSKeyChain.resMode = ( LX_SE_CRYPTO_RES_MODE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_5 )
	{
		printf("\n Feed Blk mode(0->ECB/1->CBC/2->CTR/3->OMAC1/4->SHA_1/5->SHA_256/6->HMAC_SHA_1/7->HMAC_SHA_256/8->NoBlk):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 8 )
		{
			g_stSSKeyChain.blkMode = ( LX_SE_CRYPTO_BLK_MODE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_6 )
	{
		printf("\n Feed TS format(0->NotMPEG2TS/1->MPEG2TS_188/2->MPEG2TS_192):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 2 )
		{
			g_stSSKeyChain.tsFormat = ( LX_SE_TS_FORMAT_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_7 )
	{
		printf("\n Feed Src Tpye(0->DMALenIsMoreThanPULen/1->PULenIsMoreThanDMALen):" );
		scanf("%d", &uiTmpVar );
		if ( uiTmpVar <= 1 )
		{
			g_stSSKeyChain.srcType = ( LX_SE_SOURCE_TYPE_T )uiTmpVar;
		}
		else
		{
			printf(" Wrong input!\n" );
			return;
		}
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_8 )
	{
		printf("\n Feed Load len:");
		scanf("%d", &g_stSSKeyChain.processLoadLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_9 )
	{
		printf("\n Feed skip len:");
		scanf("%d", &g_stSSKeyChain.processSkipLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_10 )
	{
		printf("\n Feed Keysize(bytes):");
		scanf("%d", &g_stSSKeyChain.keyLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_11 )
	{
		printf("\n Feed seq src len:");
		scanf("%d", &g_stSSKeyChain.seqSrcLength);
	}

	if ( i_uiDispMem & SET_CRYPT_MEM_12 )
	{
		printf("\n Feed seq src stat:");
		scanf("%d", &g_stSSKeyChain.seqSrcState);
	}

	if ( i_uiDispMem == SET_CRYPT_MEM_ALL )
	{
		printf("\n ----------------------------------\n" );
	}
}

void CLI_SE_UpdateKeyChainInfo()
{
	UINT32 uiTmpVar = 0;

	CLI_SE_DEBUG_INFO ( ( "------------KeyChainInfo-----------------\n" ) );
	CLI_SE_KeyChainDet();
	CLI_SE_DEBUG_INFO ( ( " 13 ---------------------------> to quit \n" ) );

	while ( 1 )
	{
		printf( "\nEnter ur choice:" );
		scanf("%d", &uiTmpVar );

		if ( 13 == uiTmpVar )
		{
			break;
		}

		switch( uiTmpVar)
		{
			case 1:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_1 );
				break;

			case 2:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_2 );
				break;

			case 3:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_3 );
				break;

			case 4:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_4 );
				break;

			case 5:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_5 );
				break;

			case 6:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_6 );
				break;

			case 7:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_7 );
				break;

			case 8:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_8 );
				break;

			case 9:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_9 );
				break;

			case 10:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_10 );
				break;

			case 11:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_11 );
				break;

			case 12:
				CLI_SE_SetKeyChainInfo( SET_CRYPT_MEM_12 );
				break;

			default:
				CLI_SE_DEBUG_INFO( ( "Wrong request!\n" ) );
				break;
		}
	}
}

DTV_STATUS_T CLI_SE_CryptoInputDetails( int argc ,char **argv )
{
	CLI_SE_HELP ( ( "|-----------------------------------------------------------------------------------------------------|\n" ) );
	CLI_SE_HELP ( ( "CLI SE> Following are the details of input should be given for encryption or decryption. \n" ) );
	CLI_SE_CryptDet();
	CLI_SE_HELP ( ( "|-----------------------------------------------------------------------------------------------------|\n" ) );

	return OK;
}

DTV_STATUS_T CLI_SE_SetCryptoDetails( int arc ,char **argv )
{
	if ( FALSE == g_sbIsCryptDetailsSet )
	{
		CLI_SE_SetDefaultCryptoInfo4BlkType( );

		g_sbIsCryptDetailsSet = TRUE;
	}
	else
	{
		CLI_SE_UpdateCryptoInfo();
	}

	return OK;
}

DTV_STATUS_T CLI_SE_ResetCryptoDetails( int arc ,char **argv )
{
	g_sbIsCryptDetailsSet = FALSE;

	CLI_SE_INFO( ( "CLI SE> ResetCryptoDetails - S! \n" ) );

	return OK;
}

DTV_STATUS_T CLI_SE_DisplayCryptoInfo( int arc ,char **argv )
{
	CLI_SE_DEBUG_INFO( (" ----------------------------------\n" ) );

	CLI_SE_DEBUG_INFO( ( " vir src add 0x%x dst src add 0x%x\n", g_pcSCVirtualBaseAddr,
									( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ) ) );

	CLI_SE_DEBUG_INFO( (" Channel(0->ChanA / 1->ChanB):\t%d\n",(UINT32)g_stCryptInfo.channel ) );

	CLI_SE_DEBUG_INFO( (" Enc or Dcr(0->Encryption / 1->Decryption):\t%d\n",(UINT32)g_stCryptInfo.operation ) );

	CLI_SE_DEBUG_INFO( (" Crypto type(0->ByPass/1->DES/2->TDES/3->AES/4->MS/5->RC4/6->SHA/"
											"7->HMAC_SHA):\t%d\n",(UINT32)g_stCryptInfo.cryptoType ) );

	CLI_SE_DEBUG_INFO( (" Res mode(0->Clear/1->CTS/2->OFB/3->SCTE/4->PadEnbl/"
										"5->PadDsbl/6->NoRes):\t%d\n",(UINT32)g_stCryptInfo.resMode ) );

	CLI_SE_DEBUG_INFO( (" Blk mode(0->ECB/1->CBC/2->CTR/3->OMAC1/4->SHA_1/5->SHA_256/6->HMAC_SHA_1/"
										"7->HMAC_SHA_256/8->NoBlk):\t%d\n",(UINT32)g_stCryptInfo.blkMode ) );

//	CLI_SE_DEBUG_INFO( (" TS Odd mode(0->EvenKeyMode/1->OddKeyMode):\t%d\n",(UINT32)g_stCryptInfo.tsOddMode ) );

	CLI_SE_DEBUG_INFO( (" TS format(0->NotMPEG2TS/1->MPEG2TS_188/2->MPEG2TS_192):\t%d\n",(UINT32)g_stCryptInfo.tsFormat ) );

	CLI_SE_DEBUG_INFO( (" Src Tpye(0->DMALenIsMoreThanPULen/1->PULenIsMoreThanDMALen):\t%d\n",(UINT32)g_stCryptInfo.srcType ) );

	CLI_SE_DEBUG_INFO( (" Load len:\t%d\n",(UINT32)g_stCryptInfo.processLoadLength ) );

	CLI_SE_DEBUG_INFO( (" skip len:\t%d\n",(UINT32)g_stCryptInfo.processSkipLength ) );

	CLI_SE_DEBUG_INFO( (" Src offset:\t0x%x\n",(UINT32)g_stCryptInfo.srcBuf.offset ) );

	CLI_SE_DEBUG_INFO( (" Src len:\t%d\n",(UINT32)g_stCryptInfo.srcBuf.length ) );

	CLI_SE_DEBUG_INFO( (" Dst offset:\t0x%x\n",(UINT32)g_stCryptInfo.destBuf.offset ) );

	CLI_SE_DEBUG_INFO( (" Dst len:\t%d\n",(UINT32)g_stCryptInfo.destBuf.length ) );

	CLI_SE_DEBUG_INFO( (" Keysize(bytes):\t%d\n",(UINT32)g_stCryptInfo.keyLength ) );

	CLI_SE_DEBUG_INFO( (" seq src len:\t%d\n",(UINT32)g_stCryptInfo.seqSrcLength ) );

	CLI_SE_DEBUG_INFO( (" seq src stat:\t%d\n",(UINT32)g_stCryptInfo.seqSrcState ) );

	CLI_SE_DEBUG_INFO( (" ----------------------------------\n" ) );

	return OK;
}

DTV_STATUS_T CLI_SE_CheckCryptoBuf( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

#if 1

	if ( 4 != arc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> Provide src, dst addr and len to be compared!\n" ) );
	}
	else
	{
		UINT32 uiScrAdd = 0;
		UINT32 uiDstAdd = 0;
		UINT32 uiLen = 0;

		uiScrAdd = strtoul( argv[ 1 ], NULL, 0 );

		uiDstAdd = strtoul( argv[ 2 ], NULL, 0 );

		uiLen = strtoul( argv[ 3 ], NULL, 0 );

		CLI_HLPR_SE_MemCmp( ( UINT8 * )uiScrAdd, ( UINT8 * )uiDstAdd, uiLen );

		CLI_SE_INFO( ( "CLI SE> CheckCryptoResult S!\n" ) );
	}

#else

#if 0
	CLI_SE_DEBUG_INFO( ( "CLI SE> Comparing Enc src and Enc dst buf !!\n" ) );

	CLI_HLPR_SE_MemCmp( (UINT8 *)0x40a48000, (UINT8 *)0x40A5A5C0, 75200 );

	CLI_SE_DEBUG_INFO( ( "CLI SE> Comparing Dec src(Enc dst) and Dec dst buf !!\n" ) );

	CLI_HLPR_SE_MemCmp( (UINT8 *)0x40A5A5C0, (UINT8 *)0x40A6CB80, 75200 );

#endif /* #if 0 */
	CLI_SE_DEBUG_INFO( ( "CLI SE> Comparing Enc src(org) and Dec dst buf(final) !!\n" ) );

	CLI_HLPR_SE_MemCmp( (UINT8 *)0xA204000, (UINT8 *)0xA243fb8, *g_stCryptInfo.destBuf.length );

#endif /* if 0 */

	return eResult;
}

DTV_STATUS_T CLI_SE_InitSecurityCore( int argc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_Init();

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_InitSecurityCore S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_InitSecurityCore F!\n" ) );
	}

	return eResult;
}


DTV_STATUS_T CLI_SE_UnInitSecurityCore( int argc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_UnInit();

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "\nCLI SE> CLI_SE_UnInitSecurityCore S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "\nCLI SE> CLI_SE_UnInitSecurityCore F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_Reset_SC( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_SC_SW_Reset();
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Reset_SC S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Reset_SC F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_GetSCAccess( int argc ,char **argv )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	if ( 2 != argc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enter -> Single use or Mastership i.e 1 or 0 !\n" ) );

		return eResult;
	}

	eResult = CLI_HLPR_SE_GetSCAccess( strtoul( argv[ 1 ], NULL, 0 ) );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_GetSCAccess S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_GetSCAccess F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_ReleaseSCAccess( int argc ,char **argv )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	eResult = CLI_HLPR_SE_ReleaseSCAccess( );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_ReleaseSCAccess S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_ReleaseSCAccess F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_DMAStatus( int arc ,char **argv )
{
	if ( 2 != arc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enter ChA or ChB!\n" ) );
	}
	else
	{
		CLI_HLPR_SE_PrintDMAState( strtoul( argv[ 1 ], NULL, 0 ) );
	}

	return OK;
}

DTV_STATUS_T CLI_SE_IsDMABusy( int arc ,char **argv )
{
	if ( 2 != arc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enter ChA or ChB!\n" ) );
	}
	else
	{
		CLI_HLPR_SE_PrintDMABusyStatus( strtoul( argv[ 1 ], NULL, 0 ) );
	}

	return OK;
}

DTV_STATUS_T CLI_SE_IsJobDone( int arc ,char **argv )
{
	if ( 2 != arc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enter ChA or ChB!\n" ) );
	}
	else
	{
		CLI_HLPR_SE_PrintJobStatus( strtoul( argv[ 1 ], NULL, 0 ) );
	}

	return OK;
}

DTV_STATUS_T CLI_SE_DoCryptoOp_Hash( int arc ,char **argv )
{
	UINT32 uiTmpVar = 0;

	DTV_STATUS_T eResult = NOT_OK;

	if ( 3 != arc )
	{
		CLI_SE_CRITICAL( ( "< cmd > < channel > < Keysize >\n" ) );

		CLI_SE_CRITICAL( ( "CLI SE> vir src add 0x%x dst src add 0x%x\n", (UINT32)g_pcSCVirtualBaseAddr,
											(UINT32)( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ) ) );

		memset( ( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ), 0, SE_CRYPTO_BUF_SIZE );

		return eResult;
	}

	CLI_SE_SetDefaultCryptoInfoForSHAType();

	uiTmpVar = strtoul( argv[ 1 ], NULL, 0 );
	if ( uiTmpVar <= 1 )
	{
		g_stCryptInfo.channel= ( LX_SE_DMA_CHANNEL_TYPE_T )uiTmpVar;
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> channel - wrong! \n" ) );

		return NOT_OK;
	}

	g_stCryptInfo.keyLength = strtoul( argv[ 2 ], NULL, 0 );

	if ( g_stCryptInfo.keyLength )
	{
		eResult = CLI_HLPR_SE_SetKey( g_stCryptInfo.channel, g_saucKeyBuf, g_stCryptInfo.keyLength );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SetKey S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetKey F!\n" ) );
		}
	}

	eResult = CLI_HLPR_SE_SetIVData( g_stCryptInfo.channel, g_saucIVData, 16 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SetIVData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetIVData F!\n" ) );
	}

	eResult = CLI_HLPR_SE_DoCryptoOp( &g_stCryptInfo, TRUE );

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_DoCryptoOp S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_DoCryptoOp F!\n" ) );
	}

	return eResult;

}

DTV_STATUS_T CLI_SE_DoCryptoOp( int arc ,char **argv )
{
	UINT32 uiTmpVar = 0;

	DTV_STATUS_T eResult = NOT_OK;

	if ( 5 != arc )
	{
		CLI_SE_CRITICAL( ( "< cmd > < Enc/dec > <keysize> < blk > <res> \n" ) );

		CLI_SE_CRITICAL( ( "CLI SE> vir src add 0x%x dst src add 0x%x\n", (UINT32)g_pcSCVirtualBaseAddr,
											(UINT32)( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ) ) );

		memset( ( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ), 0, SE_CRYPTO_BUF_SIZE );

		return eResult;
	}

	uiTmpVar = strtoul( argv[ 1 ], NULL, 0 );
	if ( uiTmpVar <= 1 )
	{
		g_stCryptInfo.operation = ( LX_SE_CRYPTO_OPERATION_T )uiTmpVar;
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> operation - wrong! \n" ) );

		return NOT_OK;
	}

	g_stCryptInfo.keyLength = strtoul( argv[ 2 ], NULL, 0 );

	uiTmpVar = strtoul( argv[ 3 ], NULL, 0 );
	if ( uiTmpVar <= 8 )
	{
		g_stCryptInfo.blkMode = ( LX_SE_CRYPTO_BLK_MODE_T )uiTmpVar;

		CLI_SE_DEBUG_INFO(( "\n Blk Mode %d \n", uiTmpVar ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Blk Mode - wrong! \n" ) );

		return NOT_OK;
	}

	uiTmpVar = strtoul( argv[ 4 ], NULL, 0 );
	if ( uiTmpVar <= 6 )
	{
		g_stCryptInfo.resMode = ( LX_SE_CRYPTO_RES_MODE_T )uiTmpVar;
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Res Mode - wrong! \n" ) );

		return NOT_OK;
	}

	if ( g_stCryptInfo.keyLength )
	{
		eResult = CLI_HLPR_SE_SetKey( g_stCryptInfo.channel, g_saucKeyBuf, g_stCryptInfo.keyLength );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SetKey S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetKey F!\n" ) );
		}
	}

	eResult = CLI_HLPR_SE_SetIVData( g_stCryptInfo.channel, g_saucIVData, 16 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SetIVData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetIVData F!\n" ) );
	}

	if ( eENCRYPTION == g_stCryptInfo.operation )
	{
		g_stCryptInfo.srcBuf.offset = SE_ENC_SRC_OFF;

		g_stCryptInfo.srcBuf.length = SE_CRYPTO_BUF_SIZE;

		g_stCryptInfo.destBuf.offset = SE_ENC_DST_OFF;

		g_stCryptInfo.destBuf.length = SE_CRYPTO_BUF_SIZE;
	}
	else
	{
		g_stCryptInfo.srcBuf.offset	= SE_DEC_SRC_OFF;

		g_stCryptInfo.srcBuf.length = SE_CRYPTO_BUF_SIZE;

		g_stCryptInfo.destBuf.offset = SE_DEC_DST_OFF;

		g_stCryptInfo.destBuf.length = SE_CRYPTO_BUF_SIZE;

	}

	eResult = CLI_HLPR_SE_DoCryptoOp( &g_stCryptInfo, TRUE );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_DoCryptoOp S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_DoCryptoOp F!\n" ) );
	}

	return eResult;

}

DTV_STATUS_T CLI_SE_SetKey( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 3 != arc )
	{
		CLI_SE_CRITICAL( ( "<cmd> <0-chA /1-ChB> <size>\n" ) );
	}
	else
	{
		SINT32 lTmp = strtoul( argv[ 1 ], NULL, 0 );

		if ( 0 == lTmp || 1 == lTmp )
		{
			eResult = CLI_HLPR_SE_SetKey( ( LX_SE_DMA_CHANNEL_TYPE_T )lTmp,
											g_saucKeyBuf,
											strtoul( argv[ 2 ], NULL, 0 ) );
		}
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SetKey S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetKey F!\n" ) );
		}
	}

	return OK;
}

DTV_STATUS_T CLI_SE_SetNullKeys( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 2 != arc )
	{
		CLI_SE_CRITICAL( ( "<cmd> <0-chA /1-ChB> \n" ) );
	}
	else
	{
		UINT8 aucKeyBuf[ 48 ];

		SINT32 lTmp = strtoul( argv[ 1 ], NULL, 0 );

		memset( aucKeyBuf, 0, 48 );

		if ( 0 == lTmp || 1 == lTmp )
		{
			eResult = CLI_HLPR_SE_SetKey( ( LX_SE_DMA_CHANNEL_TYPE_T )lTmp,
											aucKeyBuf,
											48 );
		}
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SetKey S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetKey F!\n" ) );
		}
	}

	return OK;
}


DTV_STATUS_T CLI_SE_SetIV( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 3 != arc )
	{
		CLI_SE_CRITICAL( ( "<cmd> <0-chA /1-ChB> <size>\n" ) );
	}
	else
	{
		SINT32 lTmp = strtoul( argv[ 1 ], NULL, 0 );

		if ( 0 == lTmp || 1 == lTmp )
		{
			eResult = CLI_HLPR_SE_SetIVData( ( LX_SE_DMA_CHANNEL_TYPE_T )lTmp,
											   g_saucIVData,
											   strtoul( argv[ 2 ], NULL, 0 ) );
		}

		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SetIVData S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetIVData F!\n" ) );
		}
	}

	return OK;
}

DTV_STATUS_T CLI_SE_SecStrTestPRNG( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_SS_Test_PRNG_Fn();
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SecStrTestPRNG S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SecStrTestPRNG F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_SecStrEncUsrInfo( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 3 != arc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> File name & Channel??\n" ) );
	}
	else
	{
		eResult = CLI_HLPR_SE_SS_EncryptUsrInfo( ( UINT8* )argv[ 1 ], strtoul( argv[ 2 ], NULL, 0 ) );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SecStrEncUsrInfo S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SecStrEncUsrInfo F!\n" ) );
		}
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_SecStrDecUsrInfo( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 3 != arc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> File name & Channel????\n" ) );
	}
	else
	{
		eResult = CLI_HLPR_SE_SS_DecryptUsrInfo( ( UINT8* )argv[ 1 ], strtoul( argv[ 2 ], NULL, 0 ) );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SecStrDecUsrInfo S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SecStrDecUsrInfo F!\n" ) );
		}
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_SecStrEncKey( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_SS_EncKey( );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SecStrEncKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SecStrEncKey F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_SecStrDecKey( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_SS_DecKey( );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SecStrDecKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SecStrDecKey F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_SecStrGenMAC( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 3 != arc )
	{
		CLI_SE_CRITICAL( ( "<cmd> <chan> <1 - auto/0 - fxd key>\n" ) );
	}
	else
	{
		eResult = CLI_HLPR_SE_SS_GenMAC( strtoul( argv[ 1 ], NULL, 0 ), strtoul( argv[ 2 ], NULL, 0 ) );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_SecStrGenMAC S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SecStrGenMAC F!\n" ) );
		}
	}

	return eResult;
}


DTV_STATUS_T CLI_SE_KeyChainInputDetails( int argc ,char **argv )
{
	CLI_SE_HELP ( ( "|-----------------------------------------------------------------------------------------------------|\n" ) );
	CLI_SE_HELP ( ( "CLI SE> Following are the details of input should be given for encryption or decryption. \n" ) );
	CLI_SE_KeyChainDet();
	CLI_SE_HELP ( ( "|-----------------------------------------------------------------------------------------------------|\n" ) );

	return OK;
}

DTV_STATUS_T CLI_SE_SetKeyChainParams( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 2 != arc )
	{
		CLI_SE_CRITICAL( ( "<cmd> <0-Default or 1-manual>\n" ) );
	}
	else
	{
		eResult = OK;

		if ( 0 == strtoul( argv[ 1 ], NULL, 0 ) )
		{
			g_stSSKeyChain.channel = eCHANNEL_A;

			g_stSSKeyChain.operation = eENCRYPTION;

			g_stSSKeyChain.cryptoType = eAES;

			g_stSSKeyChain.blkMode = eECB;

			g_stSSKeyChain.resMode = eClear;

			g_stSSKeyChain.tsFormat = eNOT_MPEG2TS;

			g_stSSKeyChain.srcType = eBLOCK_CIPHER_TYPE;

			g_stSSKeyChain.processLoadLength = SS_MAX_KEY_SIZE;

			g_stSSKeyChain.processSkipLength = 0;

			g_stSSKeyChain.keyLength = 16;

			g_stSSKeyChain.seqSrcLength = 0;

			g_stSSKeyChain.seqSrcState = 0;
		}
		else
		{
			CLI_SE_UpdateKeyChainInfo();
		}
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_DisplayKeyChainInfo( int arc ,char **argv )
{
	CLI_SE_DEBUG_INFO( (" ----------------------------------\n" ) );

	CLI_SE_DEBUG_INFO( ( " vir src add 0x%x dst src add 0x%x\n", g_pcSCVirtualBaseAddr,
									( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ) ) );

	CLI_SE_DEBUG_INFO( (" Channel(0->ChanA / 1->ChanB):\t%d\n",(UINT32)g_stSSKeyChain.channel ) );

	CLI_SE_DEBUG_INFO( (" Enc or Dcr(0->Encryption / 1->Decryption):\t%d\n",(UINT32)g_stSSKeyChain.operation ) );

	CLI_SE_DEBUG_INFO( (" Crypto type(0->ByPass/1->DES/2->TDES/3->AES/4->MS/5->RC4/6->SHA/"
											"7->HMAC_SHA):\t%d\n",(UINT32)g_stSSKeyChain.cryptoType ) );

	CLI_SE_DEBUG_INFO( (" Res mode(0->Clear/1->CTS/2->OFB/3->SCTE/4->PadEnbl/"
										"5->PadDsbl/6->NoRes):\t%d\n",(UINT32)g_stSSKeyChain.resMode ) );

	CLI_SE_DEBUG_INFO( (" Blk mode(0->ECB/1->CBC/2->CTR/3->OMAC1/4->SHA_1/5->SHA_256/6->HMAC_SHA_1/"
										"7->HMAC_SHA_256/8->NoBlk):\t%d\n",(UINT32)g_stSSKeyChain.blkMode ) );

	CLI_SE_DEBUG_INFO( (" TS format(0->NotMPEG2TS/1->MPEG2TS_188/2->MPEG2TS_192):\t%d\n",(UINT32)g_stSSKeyChain.tsFormat ) );

	CLI_SE_DEBUG_INFO( (" Src Tpye(0->DMALenIsMoreThanPULen/1->PULenIsMoreThanDMALen):\t%d\n",(UINT32)g_stSSKeyChain.srcType ) );

	CLI_SE_DEBUG_INFO( (" Load len:\t%d\n",(UINT32)g_stSSKeyChain.processLoadLength ) );

	CLI_SE_DEBUG_INFO( (" skip len:\t%d\n",(UINT32)g_stSSKeyChain.processSkipLength ) );

	CLI_SE_DEBUG_INFO( (" Src(key) addr:\t0x%x\n",(UINT32)g_stSSKeyChain.protectedKey.data.bufAddr ) );

	CLI_SE_DEBUG_INFO( (" Src(key) len:\t%d\n",(UINT32)g_stSSKeyChain.protectedKey.data.length ) );

	CLI_SE_DEBUG_INFO( (" RouteChain(key):\t0x%x\n",(UINT32)g_stSSKeyChain.protectedKey.bRouteChain ) );

	CLI_SE_DEBUG_INFO( (" Key Index(key):\t0x%x\n",(UINT32)g_stSSKeyChain.protectedKey.keyIndex ) );

	CLI_SE_DEBUG_INFO( (" Src(IV) addr:\t0x%x\n",(UINT32)g_stSSKeyChain.protectedIv.data.bufAddr ) );

	CLI_SE_DEBUG_INFO( (" Src(IV) len:\t%d\n",(UINT32)g_stSSKeyChain.protectedIv.data.length ) );

	CLI_SE_DEBUG_INFO( (" RouteChain(IV):\t0x%x\n",(UINT32)g_stSSKeyChain.protectedIv.bRouteChain ) );

	CLI_SE_DEBUG_INFO( (" Key Index(IV):\t0x%x\n",(UINT32)g_stSSKeyChain.protectedIv.keyIndex ) );

	CLI_SE_DEBUG_INFO( (" Keysize(bytes):\t%d\n",(UINT32)g_stSSKeyChain.keyLength ) );

	CLI_SE_DEBUG_INFO( (" seq src len:\t%d\n",(UINT32)g_stSSKeyChain.seqSrcLength ) );

	CLI_SE_DEBUG_INFO( (" seq src stat:\t%d\n",(UINT32)g_stSSKeyChain.seqSrcState ) );

	CLI_SE_DEBUG_INFO( (" ----------------------------------\n" ) );

	return OK;
}

DTV_STATUS_T CLI_SE_GenKey( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( arc < 5 )
	{
		CLI_SE_CRITICAL( ( "<cmd> <0-chA/1-chB> <0-onlykeyGen or 1-KeyGenNSet>"
					" <0-OTPkey or 1-SC key> <keylenGen> <0-Enc/1-Dec -- optional>\n" ) );
	}
	else
	{
		if ( arc > 5 )
		{
			g_stSSKeyChain.operation = ( LX_SE_CRYPTO_OPERATION_T )strtoul( argv[ 5 ], NULL, 0 );
		}
		else
		{
			g_stSSKeyChain.operation = eENCRYPTION;
		}

		g_stSSKeyChain.channel = strtoul( argv[ 1 ], NULL, 0 );

		g_stSSKeyChain.protectedKey.tsOddMode = eEVEN_KEY;

		g_stSSKeyChain.protectedKey.keyIndex = 0;

		if ( 0 == strtoul( argv[ 3 ], NULL, 0 ) )
		{
			g_stSSKeyChain.protectedKey.bRouteChain = FALSE;
		}
		else
		{
			g_stSSKeyChain.protectedKey.bRouteChain = TRUE;
		}

		g_stSSKeyChain.protectedKey.data.bufAddr = g_aucSSGenKeyBuf;

		if ( strtoul( argv[ 4 ], NULL, 0 ) <= SS_MAX_KEY_SIZE )
		{
			g_stSSKeyChain.protectedKey.data.length = strtoul( argv[ 4 ], NULL, 0 );

			g_stSSKeyChain.processLoadLength = g_stSSKeyChain.protectedKey.data.length;

			g_stSSKeyChain.protectedIv.data.length = g_stSSKeyChain.processLoadLength;

			if ( 0 == strtoul( argv[ 2 ], NULL, 0 ) )
			{
				eResult = CLI_HLPR_SE_SS_GenKey( &g_stSSKeyChain, FALSE );
			}
			else
			{
				eResult = CLI_HLPR_SE_SS_GenKey( &g_stSSKeyChain, TRUE );
			}
		}
		else
		{
			CLI_SE_CRITICAL( ( "wrong input!\n" ) );
		}
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_GenKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_GenKey F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_GenAndSetKeyUsngIndx( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 2 != arc )
	{
		CLI_SE_CRITICAL( ( "<cmd> <Crypto Comb Indx>\n" ) );
	}
	else
	{
		g_aucCryptArrCntr = strtoul( argv[ 1 ], NULL, 0 );
		eResult = CLI_SE_GenNSetKey( g_aucCryptArrCntr );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_GenAndSetKey( int arc ,char **argv )
{
	g_aucCryptArrCntr++;

	CLI_SE_GenNSetKey( g_aucCryptArrCntr );

	return OK;
}

DTV_STATUS_T CLI_SE_SetProtKey( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( arc < 3 )
	{
		CLI_SE_CRITICAL( ( "<cmd> <0-chA/1-chB> <0-OTPkey or 1-SC key> <0-Enc/1-Dec - optional>\n" ) );
	}
	else
	{
		if ( arc > 3 )
		{
			g_stSSKeyChain.operation = ( LX_SE_CRYPTO_OPERATION_T )strtoul( argv[ 3 ], NULL, 0 );
		}
		else
		{
			g_stSSKeyChain.operation = eDECRYPTION;
		}

		g_stSSKeyChain.channel = strtoul( argv[ 1 ], NULL, 0 );

		g_stSSKeyChain.protectedKey.tsOddMode = eEVEN_KEY;

		g_stSSKeyChain.protectedKey.keyIndex = 0;

		if ( 0 == strtoul( argv[ 2 ], NULL, 0 ) )
		{
			g_stSSKeyChain.protectedKey.bRouteChain = FALSE;
		}
		else
		{
			g_stSSKeyChain.protectedKey.bRouteChain = TRUE;
		}

		g_stSSKeyChain.protectedKey.data.bufAddr = g_aucSSGenKeyBuf;

		//g_stSSKeyChain.protectedKey.data.length = SS_MAX_KEY_SIZE;

		eResult = CLI_HLPR_SE_SS_SetProtKey( &g_stSSKeyChain );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SetProtKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetProtKey F!\n" ) );
	}
	return eResult;
}

DTV_STATUS_T CLI_SE_SetProtIV( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( arc < 3 )
	{
		CLI_SE_CRITICAL( ( "<cmd> <0-chA/1-chB> <0-OTPkey or 1-SC key>  <0-Enc/1-Dec - optional>\n" ) );
	}
	else
	{
		if ( arc > 3 )
		{
			g_stSSKeyChain.operation = ( LX_SE_CRYPTO_OPERATION_T )strtoul( argv[ 3 ], NULL, 0 );
		}
		else
		{
			g_stSSKeyChain.operation = eDECRYPTION;
		}

		memset( g_aucSSProtIVBuf, 0x31, SS_MAX_KEY_SIZE );

		g_stSSKeyChain.channel = strtoul( argv[ 1 ], NULL, 0 );

		g_stSSKeyChain.protectedIv.tsOddMode = eEVEN_KEY;

		g_stSSKeyChain.protectedIv.keyIndex = 0;

		if ( 0 == strtoul( argv[ 2 ], NULL, 0 ) )
		{
			g_stSSKeyChain.protectedIv.bRouteChain = FALSE;
		}
		else
		{
			g_stSSKeyChain.protectedIv.bRouteChain = TRUE;
		}

		//g_stSSKeyChain.protectedIv.data.bufAddr = g_aucSSProtIVBuf;

		//g_stSSKeyChain.protectedIv.data.length = SS_MAX_KEY_SIZE;

		g_stSSKeyChain.protectedIv.data.bufAddr = g_aucSSGenKeyBuf;

		eResult = CLI_HLPR_SE_SS_SetProtIV( &g_stSSKeyChain );
	}

	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SetProtKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetProtKey F!\n" ) );
	}
	return eResult;
}

DTV_STATUS_T CLI_SE_SetCntrlData( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	eResult = CLI_HLPR_SE_DoCryptoOp( &g_stCryptInfo, FALSE );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_SetCntrlData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_SetCntrlData F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_RunCrypto( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 6 != arc )
	{
		CLI_SE_CRITICAL( ( "<cmd> <chan> <src off> <src len> <dst off> <dst len> \n" ) );
	}
	else
	{
		LX_SE_CONTENTS_T stCntnts;

		stCntnts.channel = strtoul( argv[ 1 ], NULL, 0 );

		stCntnts.input.offset = strtoul( argv[ 2 ], NULL, 0 );

		stCntnts.input.length = strtoul( argv[ 3 ], NULL, 0 );

		stCntnts.output.offset = strtoul( argv[ 4 ], NULL, 0 );

		stCntnts.output.length = strtoul( argv[ 5 ], NULL, 0 );

		eResult = CLI_HLPR_SE_RunCrypto( &stCntnts );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> CLI_SE_RunCrypto S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_RunCrypto F!\n" ) );
		}
	}

	return eResult;
}

#define SAMPLE_BYTES_LEN 	10

DTV_STATUS_T CLI_SE_Enc2ChSimultaneously( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( !g_pcSCVirtualBaseAddr )
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enc2ch CLI_SE_SetKey F!\n" ) );

		return eResult;
	}

	memcpy( ( g_pcSCVirtualBaseAddr + ( 3 * SE_CRYPTO_BUF_SIZE ) ), g_pcSCVirtualBaseAddr, SE_CRYPTO_BUF_SIZE );

	memset( ( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ), 0, SE_CRYPTO_BUF_SIZE );

	memset( ( g_pcSCVirtualBaseAddr + ( 2 * SE_CRYPTO_BUF_SIZE ) ), 0, SE_CRYPTO_BUF_SIZE );

#if 0
	CLI_SE_INFO( ( "ch1 - Enc src \n" ) );
	CLI_MemDump( ( UINT32 * )g_pcSCVirtualBaseAddr, SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch1 - Enc dst \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Enc src \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 3 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Enc dst \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 2 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );

#endif /* #if 0 */

	g_stCryptInfo.channel = eCHANNEL_A;

	g_stCryptInfo.operation = eENCRYPTION;

	g_stCryptInfo.cryptoType = eAES;

	g_stCryptInfo.blkMode = eECB;

	g_stCryptInfo.resMode = eCTS;

//	g_stCryptInfo.tsOddMode = eEVEN_KEY;

	g_stCryptInfo.tsFormat = eMPEG2TS_188;

	g_stCryptInfo.srcType = eBLOCK_CIPHER_TYPE;

	g_stCryptInfo.processLoadLength = 184;

	g_stCryptInfo.processSkipLength = 4;

	g_stCryptInfo.srcBuf.offset = SE_ENC_SRC_OFF;

	g_stCryptInfo.srcBuf.length = SE_CRYPTO_BUF_SIZE;

	g_stCryptInfo.destBuf.offset = SE_ENC_DST_OFF;

	g_stCryptInfo.destBuf.length = SE_CRYPTO_BUF_SIZE;

	g_stCryptInfo.keyLength = 16;

	g_stCryptInfo.seqSrcLength = 0;

	g_stCryptInfo.seqSrcState = 0;

	eResult = CLI_HLPR_SE_SetKey( eCHANNEL_A, g_saucKeyBuf, g_stCryptInfo.keyLength );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Enc2ch CLI_SE_SetKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enc2ch CLI_SE_SetKey F!\n" ) );
	}

	eResult = CLI_HLPR_SE_SetIVData( eCHANNEL_A, g_saucIVData, 16 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Enc2ch CLI_SE_SetIVData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enc2ch CLI_SE_SetIVData F!\n" ) );
	}

	eResult = CLI_HLPR_SE_SetKey( eCHANNEL_B, g_saucKeyBuf, g_stCryptInfo.keyLength );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Enc2ch CLI_SE_SetKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enc2ch CLI_SE_SetKey F!\n" ) );
	}

	eResult = CLI_HLPR_SE_SetIVData( eCHANNEL_B, g_saucIVData, 16 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Enc2ch CLI_SE_SetIVData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enc2ch CLI_SE_SetIVData F!\n" ) );
	}

	eResult = CLI_HLPR_SE_DoCryptoOp( &g_stCryptInfo, TRUE );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Enc2ch CLI_SE_DoCryptoOp S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enc2ch CLI_SE_DoCryptoOp F!\n" ) );
	}

	g_stCryptInfo.channel = eCHANNEL_B;

	g_stCryptInfo.srcBuf.offset = SE_DEC_SRC_OFF_2_CH_TEST;

	g_stCryptInfo.destBuf.offset = SE_ENC_DST_OFF;

	eResult = CLI_HLPR_SE_DoCryptoOp( &g_stCryptInfo, TRUE );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Enc2ch CLI_SE_DoCryptoOp S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enc2ch CLI_SE_DoCryptoOp F!\n" ) );
	}

	CLI_HLPR_SE_StartDMAs();


#if 0
	CLI_SE_INFO( ( "Result ch1 - Enc src \n" ) );
	CLI_MemDump( ( UINT32 * )g_pcSCVirtualBaseAddr, SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch1 - Enc dst \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Enc src \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 3 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Enc dst \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 2 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );
#endif /* #if 0 */

	return eResult;
}

DTV_STATUS_T CLI_SE_Dec2ChSimultaneously( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	memset( ( g_pcSCVirtualBaseAddr + ( 3 * SE_CRYPTO_BUF_SIZE ) ), 0, SE_CRYPTO_BUF_SIZE );

	memset( g_pcSCVirtualBaseAddr, 0, SE_CRYPTO_BUF_SIZE );

#if 0
	CLI_SE_INFO( ( "ch1 - Dec dst \n" ) );
	CLI_MemDump( ( UINT32 * )g_pcSCVirtualBaseAddr, SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch1 - Dec src \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Dec dst \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 3 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Dec src \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 2 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );
#endif /* #if 0 */

	g_stCryptInfo.channel = eCHANNEL_A;

	g_stCryptInfo.operation = eDECRYPTION;

	g_stCryptInfo.cryptoType = eAES;

	g_stCryptInfo.blkMode = eECB;

	g_stCryptInfo.resMode = eCTS;

///	g_stCryptInfo.tsOddMode = eEVEN_KEY;

	g_stCryptInfo.tsFormat = eMPEG2TS_188;

	g_stCryptInfo.srcType = eBLOCK_CIPHER_TYPE;

	g_stCryptInfo.processLoadLength = 184;

	g_stCryptInfo.processSkipLength = 4;

	g_stCryptInfo.srcBuf.offset = SE_ENC_DST_OFF;

	g_stCryptInfo.srcBuf.length = SE_CRYPTO_BUF_SIZE;

	g_stCryptInfo.destBuf.offset = SE_ENC_SRC_OFF;

	g_stCryptInfo.destBuf.length = SE_CRYPTO_BUF_SIZE;

	g_stCryptInfo.keyLength = 16;

	g_stCryptInfo.seqSrcLength = 0;

	g_stCryptInfo.seqSrcState = 0;

	eResult = CLI_HLPR_SE_SetKey( eCHANNEL_A, g_saucKeyBuf, g_stCryptInfo.keyLength );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Dec2ch CLI_SE_SetKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Dec2ch CLI_SE_SetKey F!\n" ) );
	}

	eResult = CLI_HLPR_SE_SetIVData( eCHANNEL_A, g_saucIVData, 16 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Dec2ch CLI_SE_SetIVData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Dec2ch CLI_SE_SetIVData F!\n" ) );
	}

	eResult = CLI_HLPR_SE_SetKey( eCHANNEL_B, g_saucKeyBuf, g_stCryptInfo.keyLength );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Dec2ch CLI_SE_SetKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Dec2ch CLI_SE_SetKey F!\n" ) );
	}

	eResult = CLI_HLPR_SE_SetIVData( eCHANNEL_B, g_saucIVData, 16 );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Dec2ch CLI_SE_SetIVData S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Dec2ch CLI_SE_SetIVData F!\n" ) );
	}

	eResult = CLI_HLPR_SE_DoCryptoOp( &g_stCryptInfo, TRUE );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Dec2ch CLI_SE_Dec2ChSimultaneously S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Dec2ch CLI_SE_Dec2ChSimultaneously F!\n" ) );
	}

	g_stCryptInfo.channel = eCHANNEL_B;

	g_stCryptInfo.srcBuf.offset = SE_ENC_DST_OFF;

	g_stCryptInfo.destBuf.offset = SE_DEC_SRC_OFF_2_CH_TEST;

	eResult = CLI_HLPR_SE_DoCryptoOp( &g_stCryptInfo, TRUE );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> Dec2ch CLI_SE_Dec2ChSimultaneously S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> Dec2ch CLI_SE_Dec2ChSimultaneously F!\n" ) );
	}

	CLI_HLPR_SE_StartDMAs();

#if 0
	CLI_SE_INFO( ( "Result ch1 - Dec dst \n" ) );
	CLI_MemDump( ( UINT32 * )g_pcSCVirtualBaseAddr, SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch1 - Dec src \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + SE_CRYPTO_BUF_SIZE ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Dec dst \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 3 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );

	CLI_SE_INFO( ( "ch2 - Dec src \n" ) );
	CLI_MemDump( ( UINT32 * )( g_pcSCVirtualBaseAddr + ( 2 * SE_CRYPTO_BUF_SIZE ) ), SAMPLE_BYTES_LEN );
#endif /* #if 0 */

	return eResult;
}

DTV_STATUS_T CLI_SE_GenNSetKey( UINT8 i_uiCryptCntr )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( i_uiCryptCntr >= ( sizeof( g_auiCryptDet )/16 ) )
	{
		CLI_SE_CRITICAL( ( "Wrong index!\n" ) );
	}

	g_stSSKeyChain.cryptoType = ( LX_SE_CRYPTO_TYPE_T )g_auiCryptDet[ i_uiCryptCntr ][ 0 ];

	g_stSSKeyChain.keyLength = g_auiCryptDet[ i_uiCryptCntr ][ 1 ];

	g_stSSKeyChain.blkMode = ( LX_SE_CRYPTO_BLK_MODE_T )g_auiCryptDet[ i_uiCryptCntr ][ 2 ];

	g_stSSKeyChain.resMode = ( LX_SE_CRYPTO_RES_MODE_T )g_auiCryptDet[ i_uiCryptCntr ][ 3 ];

	eResult = CLI_HLPR_SE_SS_GenKey( &g_stSSKeyChain, TRUE );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_SE_GenNSetKey S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_SE_GenNSetKey F!\n" ) );
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_PrintSCRegDetails( int arc ,char **argv )
{
	DTV_STATUS_T eResult = NOT_OK;

	if ( 2 != arc )
	{
		CLI_SE_CRITICAL( ( "CLI SE> Enter ChA or ChB!\n" ) );
	}
	else
	{
		eResult = CLI_HLPR_SE_PrintSCRegDetails( strtoul( argv[ 1 ], NULL, 0 ) );
		if ( ISDTVSUCCESS( eResult ) )
		{
			CLI_SE_INFO( ( "CLI SE> Print SC Reg info S!\n" ) );
		}
		else
		{
			CLI_SE_CRITICAL( ( "CLI SE> Print SC Reg info F!\n" ) );
		}
	}

	return eResult;
}

DTV_STATUS_T CLI_SE_PrintSPRegDetails( int arc ,char **argv )
{
	DTV_STATUS_T eResult = INVALID_PARAMS;

	eResult = CLI_HLPR_SE_PrintSPRegDetails( );
	if ( ISDTVSUCCESS( eResult ) )
	{
		CLI_SE_INFO( ( "CLI SE> CLI_HLPR_SE_PrintSPRegDetails S!\n" ) );
	}
	else
	{
		CLI_SE_CRITICAL( ( "CLI SE> CLI_HLPR_SE_PrintSPRegDetails F!\n" ) );
	}

	return eResult;
}

void CLI_SE_Start(void)
{
    UINT32 uiCliCmdId = 100;

    Add_GLI_Cmd( "SE Cryto Input struct details.." ,uiCliCmdId++ , CLI_SE_CryptoInputDetails );

	Add_GLI_Cmd( "To set the crypto input details.." ,uiCliCmdId++ , CLI_SE_SetCryptoDetails );

	Add_GLI_Cmd( "Reset the crypto input details.." ,uiCliCmdId++ , CLI_SE_ResetCryptoDetails );

	Add_GLI_Cmd( "Displays latest crypto struct info.." ,uiCliCmdId++ , CLI_SE_DisplayCryptoInfo );

	Add_GLI_Cmd( "To check the compare src and dst buf.. " ,uiCliCmdId++ , CLI_SE_CheckCryptoBuf );


	Add_GLI_Cmd( "InitSE" ,uiCliCmdId++ , CLI_SE_InitSecurityCore );

	Add_GLI_Cmd( "UnInitSE" ,uiCliCmdId++ , CLI_SE_UnInitSecurityCore );

	Add_GLI_Cmd( "ResetSC" ,uiCliCmdId++ , CLI_SE_Reset_SC );

    Add_GLI_Cmd( "GetSCAccess" ,uiCliCmdId++ , CLI_SE_GetSCAccess );

    Add_GLI_Cmd( "ReleaseSC" ,uiCliCmdId++ , CLI_SE_ReleaseSCAccess );


	Add_GLI_Cmd( "DMAState" ,uiCliCmdId++ , CLI_SE_DMAStatus );

	Add_GLI_Cmd( "IsDMABusy" ,uiCliCmdId++ , CLI_SE_IsDMABusy );

	Add_GLI_Cmd( "IsJobdone" ,uiCliCmdId++ , CLI_SE_IsJobDone );


	Add_GLI_Cmd( "DoCrypto" ,uiCliCmdId++ , CLI_SE_DoCryptoOp );

	Add_GLI_Cmd( "Test Secure hash" ,uiCliCmdId++ , CLI_SE_DoCryptoOp_Hash );

	Add_GLI_Cmd( "Setkey" ,uiCliCmdId++ , CLI_SE_SetKey );

	Add_GLI_Cmd( "SetIV" ,uiCliCmdId++ , CLI_SE_SetIV );

	Add_GLI_Cmd( "PRNG" ,uiCliCmdId++ , CLI_SE_SecStrTestPRNG );


	Add_GLI_Cmd( "SSEncUsrInfo" ,uiCliCmdId++ , CLI_SE_SecStrEncUsrInfo );

	Add_GLI_Cmd( "SSDecUsrInfo" ,uiCliCmdId++ , CLI_SE_SecStrDecUsrInfo );

	Add_GLI_Cmd( "SSEncKey" ,uiCliCmdId++ , CLI_SE_SecStrEncKey );

	Add_GLI_Cmd( "SSDecKey" ,uiCliCmdId++ , CLI_SE_SecStrDecKey );

	Add_GLI_Cmd( "SSGenMAC" ,uiCliCmdId++ , CLI_SE_SecStrGenMAC );


    Add_GLI_Cmd( "KeyChainInfo" ,uiCliCmdId++ , CLI_SE_KeyChainInputDetails );

	Add_GLI_Cmd( "SetKeyChainInfo" ,uiCliCmdId++ , CLI_SE_SetKeyChainParams );

	Add_GLI_Cmd( "DisplayKeyChainInfo" ,uiCliCmdId++ , CLI_SE_DisplayKeyChainInfo );

	Add_GLI_Cmd( "GenKey" ,uiCliCmdId++ , CLI_SE_GenKey );

	Add_GLI_Cmd( "SetProtKey" ,uiCliCmdId++ , CLI_SE_SetProtKey );

	Add_GLI_Cmd( "SetProtIV" ,uiCliCmdId++ , CLI_SE_SetProtIV );

	Add_GLI_Cmd( "SetCntrlData" ,uiCliCmdId++ , CLI_SE_SetCntrlData );

	Add_GLI_Cmd( "RunCrypto" ,uiCliCmdId++ , CLI_SE_RunCrypto );


	Add_GLI_Cmd( "Encrypt two channels simultaneously.. " ,uiCliCmdId++ , CLI_SE_Enc2ChSimultaneously );

	Add_GLI_Cmd( "Decrypt two channels simultaneously.. " ,uiCliCmdId++ , CLI_SE_Dec2ChSimultaneously );


	Add_GLI_Cmd( "Prints the SC register values.." ,uiCliCmdId++ , CLI_SE_PrintSCRegDetails );

    Add_GLI_Cmd( "Print the SP IPC reg details.." ,uiCliCmdId++ , CLI_SE_PrintSPRegDetails );

	Add_GLI_Cmd( "SetNullKeys" ,uiCliCmdId++ , CLI_SE_SetNullKeys );

	Add_GLI_Cmd( "n" ,uiCliCmdId++ , CLI_SE_GenAndSetKey );

	Add_GLI_Cmd( "GenAndSetKey using crypt comb" ,uiCliCmdId++ , CLI_SE_GenAndSetKeyUsngIndx );
}

/**  @} */
