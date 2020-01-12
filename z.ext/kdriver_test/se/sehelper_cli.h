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
 * main CLI implementation for SE device.
 *
 * author     Srinivasan Shanmugam (srinivasan.shanmugam@lge.com)
 * version    1.0
 * date       2010.03.23
 * note       Additional information.
 *
 * @addtogroup lg1150_Se
 * @{
 */
#ifndef	_SE_HELPER_CLI_H_
#define	_SE_HELPER_CLI_H_

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include "se_cfg.h"
#include "se_common.h"
#include "se_kapi.h"
#include "base_types.h"

/*----------------------------------------------------------------------------------------
 *   Constant Definitions
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   Macro Definitions
 *---------------------------------------------------------------------------------------*/

#define SE_TOTAL_PHY_BUF_SIZE			0x77560

#define SE_CRYPTO_BUF_SIZE				( SE_TOTAL_PHY_BUF_SIZE / 4 )

#define SE_ENC_SRC_OFF					0

#define SE_ENC_DST_OFF					( SE_ENC_SRC_OFF + SE_CRYPTO_BUF_SIZE )

#define SE_DEC_SRC_OFF					SE_ENC_DST_OFF

#define SE_DEC_DST_OFF					( SE_DEC_SRC_OFF + SE_CRYPTO_BUF_SIZE )

#define SE_DEC_SRC_OFF_2_CH_TEST		( SE_DEC_DST_OFF + SE_CRYPTO_BUF_SIZE )


#define CLI_SE_ENABLE_DEBUG_INFO
#define CLI_SE_ENABLE_CRITICAL_INFO
#define CLI_SE_ENABLE_INFO
#define CLI_SE_ENABLE_HELP

#define CLIPRINT(format, arg...) 		printf("%s:%d:" format , __FILE__ , __LINE__ , ## arg)

#ifdef CLI_SE_ENABLE_DEBUG_INFO

	#define CLI_SE_DEBUG_INFO( _X_ )	printf _X_

#else

	#define CLI_SE_DEBUG_INFO( _X_ )	/* NOP */

#endif /* CLI_SE_ENABLE_DEBUG_INFO */


#ifdef CLI_SE_ENABLE_CRITICAL_INFO

	#define CLI_SE_CRITICAL( _X_ )		printf _X_

#else

	#define CLI_SE_CRITICAL( _X_ )		/* NOP */


#endif /* CLI_SE_ENABLE_CRITICAL_INFO */


#ifdef CLI_SE_ENABLE_INFO

	#define CLI_SE_INFO( _X_ )			printf _X_

#else

	#define CLI_SE_INFO( _X_ )			/* NOP */


#endif /* CLI_SE_ENABLE_INFO */


#ifdef CLI_SE_ENABLE_HELP

	#define CLI_SE_HELP( _X_ )			printf _X_

#else

	#define CLI_SE_HELP( _X_ )			/* NOP */

#endif /* CLI_SE_ENABLE_HELP */

/*----------------------------------------------------------------------------------------
 *   Type Definitions
 *---------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
 *   External Function Prototype Declarations
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   External Variables
 *---------------------------------------------------------------------------------------*/


DTV_STATUS_T CLI_HLPR_SE_Init( void );

DTV_STATUS_T CLI_HLPR_SE_UnInit( void );

DTV_STATUS_T CLI_HLPR_SE_SC_SW_Reset( void );

DTV_STATUS_T CLI_HLPR_SE_GetSCAccess( UINT32 i_uiAccessType );

DTV_STATUS_T CLI_HLPR_SE_ReleaseSCAccess( void );


DTV_STATUS_T CLI_HLPR_SE_PrintDMAState( UINT32 i_uiChannel );

DTV_STATUS_T CLI_HLPR_SE_PrintJobStatus( UINT32 i_uiChannel );

DTV_STATUS_T CLI_HLPR_SE_PrintDMABusyStatus( UINT32 i_uiChannel );


void CLI_HLPR_SE_StartDMAs();

DTV_STATUS_T CLI_HLPR_SE_DoCryptoOp( LX_SE_CRYPTO_CNTL_DATA_T * i_pstCryptoInfo, BOOLEAN i_bStartSC );

DTV_STATUS_T CLI_HLPR_SE_SetKey( LX_SE_DMA_CHANNEL_TYPE_T i_eChannel, UINT8 *i_pucKeyData, UINT32 i_ucKeySize );

DTV_STATUS_T CLI_HLPR_SE_SetIVData( LX_SE_DMA_CHANNEL_TYPE_T i_eChannel, UINT8 *i_pucIVData, UINT32 i_ucIVDataSize );

DTV_STATUS_T CLI_HLPR_SE_SS_Test_PRNG_Fn( );


DTV_STATUS_T CLI_HLPR_SE_SS_EncryptUsrInfo( UINT8* i_pucFileName, UINT32 i_uiChannel );

DTV_STATUS_T CLI_HLPR_SE_SS_DecryptUsrInfo( UINT8* i_pucFileName, UINT32 i_uiChannel );

DTV_STATUS_T CLI_HLPR_SE_SS_EncKey( );

DTV_STATUS_T CLI_HLPR_SE_SS_DecKey( );

DTV_STATUS_T CLI_HLPR_SE_SS_GenMAC( UINT32 i_uiChannel, BOOLEAN i_bAutoKey );


DTV_STATUS_T CLI_HLPR_SE_SS_GenKey( LX_SE_KEY_CHAIN_T *i_pstKeyChain, BOOLEAN i_bIsGenNSet );

DTV_STATUS_T CLI_HLPR_SE_SS_SetProtKey( LX_SE_KEY_CHAIN_T *i_pstKeyChain );

DTV_STATUS_T CLI_HLPR_SE_SS_SetProtIV( LX_SE_KEY_CHAIN_T *i_pstIV );

DTV_STATUS_T CLI_HLPR_SE_RunCrypto( LX_SE_CONTENTS_T *i_pstContnt );


DTV_STATUS_T CLI_HLPR_SE_PrintSCRegDetails( UINT32 i_uiChannel );

DTV_STATUS_T CLI_HLPR_SE_PrintSPRegDetails( void );


DTV_STATUS_T CLI_HLPR_SE_MemCmp( UINT8 *i_ucScrBuf, UINT8 *i_ucDstBuf, UINT32 i_uiLen );


#endif /* _SE_HELPER_CLI_H_ */
