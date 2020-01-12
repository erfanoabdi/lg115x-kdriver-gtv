/****************************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   COPYRIGHT(c) 1998-2010 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work covered by this copyright hereon
 *   may be reproduced, stored in a retrieval system, in any form
 *   or by any means, electronic, mechanical, photocopying, recording
 *   or otherwise, without the prior written  permission of LG Electronics.
 ***************************************************************************************/

/** @file
 *
 *  Header for Secure Engine kdriver.
 *
 *  @author     stan.kim@lge.com
 *  @version    1.0
 *  @date       2011.6
 *  @note       Additional information.
 */

#ifndef	_SE_KAPI_H_
#define	_SE_KAPI_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "base_types.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	SE_IOC_MAGIC		'S'
#define LX_SE_ADC_SIZE 12

/*----------------------------------------------------------------------------------------
	IO comand naming rule  : MODULE_IO[R][W]_COMMAND
----------------------------------------------------------------------------------------*/
/**
@name Secure Engine IOCTL List
ioctl list for Secure Engine
@{
*/

/**
@def SE_IO_RESET
	reset SE devices
*/
#define SE_IO_RESET					_IO(SE_IOC_MAGIC, 0)

/**
@def SE_IOR_MEM_INFO
	get allocated memory info for se
	@param	seMemInfo	[out] pointer to LX_SE_MEM_INFO_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOR_GET_MEM_INFO			_IOR(SE_IOC_MAGIC, 1, LX_SE_MEM_INFO_T)

/**
@def SE_IORW_GET_RANDOM_NUMBER
	get h/w random number
	@param	randomData	[in/out] pointer to LX_SE_DATA_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define	SE_IORW_GET_RANDOM_NUMBER	_IOWR(SE_IOC_MAGIC, 2, LX_SE_DATA_T)

/**
@def SE_IOW_SET_KEY
	set clear key
	@param	keyData	[in] pointer to LX_SE_DATA_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOW_SET_KEY				_IOW(SE_IOC_MAGIC, 3, LX_SE_DATA_T)

/**
@def SE_IOW_SET_IV
	set clear iv
	@param	ivData	[in] pointer to LX_SE_DATA_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOW_SET_IV				_IOW(SE_IOC_MAGIC, 4, LX_SE_DATA_T)

/**
@def SE_IORW_RUN_BLOCK_OP
	Run Block Crypto Operation
	@param	operation	[in]	pointer to LX_SE_BLOCK_OP_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_RUN_BLOCK_OP		_IOWR(SE_IOC_MAGIC, 5, LX_SE_BLOCK_OP_T)

/**
@def SE_IORW_RUN_HASH_OP
	Run Hash Crypto Operation
	@param	operation	[in]	pointer to LX_SE_HASH_OP_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_RUN_HASH_OP			_IOWR(SE_IOC_MAGIC, 6, LX_SE_HASH_OP_T)

/**
@def SE_IORW_RUN_RSA_OP
	Run RSA Crypto Operation
	@param	operation	[in]	pointer to LX_SE_RSA_OP_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_RUN_RSA_OP			_IOWR(SE_IOC_MAGIC, 7, LX_SE_RSA_OP_T)

/**
@def SE_IORW_GET_SECURE_DATA
	Generate and set Random Key. Return encrypted random key by OTP key
	@param	secureData	[in/out]	pointer to LX_SE_SECURE_DATA_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_GET_SECURE_DATA		_IOWR(SE_IOC_MAGIC, 8, LX_SE_SECURE_DATA_T)

/**
@def SE_IORW_SET_SECURE_DATA
	Set Secure Key or IV following setType
	@param	secureData	[in]	pointer to LX_SE_SECURE_DATA_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_SET_SECURE_DATA		_IOWR(SE_IOC_MAGIC, 9, LX_SE_SECURE_DATA_T)

/**
@def SE_IORW_TRANSFORM_SECURE_DATA
	Generate Random Key. Return encrypted random key by OTP key and encrypted secure data by random key
	@param	secureData[2]	[in/out]	pointer to LX_SE_SECURE_DATA_T(1st:Secure Key, 2nd:Secure Data)
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_TRANSFORM_SECURE_DATA		_IOWR(SE_IOC_MAGIC, 10, LX_SE_SECURE_DATA_T)

/**
@def SE_IOW_LOCK_DEBUG
	Lock Debug H/W
	@param
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOW_LOCK_DEBUG		_IOW(SE_IOC_MAGIC, 11, LX_SE_DEBUG_T)

/**
@def SE_IORW_TRANSFORM_SECURE_DATA
	Unlock Debug H/W
	@param
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOW_UNLOCK_DEBUG		_IOW(SE_IOC_MAGIC, 12, LX_SE_DEBUG_T)

/**
@def SE_IO_SET_VERIFICATION
	Unlock Debug H/W
	@param
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IO_SET_VERIFICATION	_IO(SE_IOC_MAGIC, 13)

/**
@def SE_IOR_GET_VERIFICATION
	Unlock Debug H/W
	@param
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOR_GET_VERIFICATION	_IOR(SE_IOC_MAGIC, 14, unsigned int)

/**
@def SE_IOW_SET_ADC
	Set ADC Value
	@param
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOW_SET_ADC			_IOW(SE_IOC_MAGIC, 15, unsigned char*)

/**
@def SE_IOR_GET_ADC
	Get ADC Value
	@param
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IOR_GET_ADC			_IOR(SE_IOC_MAGIC, 16, unsigned char*)

/**
@def SE_IORW_NF_DRMAGENT
	Netflix DRM Agent for PlayReady
	@param	operation	[in/out]	pointer to LX_SE_NF_OP_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_NF_DRMAGENT		_IOWR(SE_IOC_MAGIC, 17, LX_SE_NF_OP_T)

/**
@def SE_IORW_TRUST_OPERATION
	Trust Operation which runs in Secure World
	@param	operation	[in/out]	pointer to LX_SE_TRUST_OP_T
	@return	RET_OK if success, RET_ERROR otherwise.
*/
#define SE_IORW_TRUST_OPERATION	_IOWR(SE_IOC_MAGIC, 18, LX_SE_TRUST_OP_T)

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/

/**
 *	Crypto Operation
 */
typedef enum {
	LX_SE_ENCRYPTION =0,		/**< Encryption */
	LX_SE_DECRYPTION,			/**< Decryption */
}	LX_SE_CRYPTO_OPERATION_T;

/**
 *	Crypto Type
 */
typedef enum {
	LX_SE_BYPASS =0,		/**< bypass */
	LX_SE_DES,				/**< DES */
	LX_SE_TDES,				/**< TDES */
	LX_SE_AES,				/**< AES */
	LX_SE_MULTISWAP,		/**< Multi Swap */
	LX_SE_RC4,				/**< RC4 */
	LX_SE_SHA,				/**< SHA */
	LX_SE_HMAC_SHA,			/**< HMAC SHA */
	LX_SE_MD5,				/**< MD5 */
	LX_SE_HMAC_MD5,			/**< HMAC MD5 */

	LX_SE_TDES_BAB = 0x40,	/**< Special TDES for VUDU */
	LX_SE_XOR,				/**< XOR for HDCP2.0 */

	LX_SE_RSA_1_5 = 0x80,	/**< RSA 1.5 */
	LX_SE_RSA_OAEP,			/**< RSA-OAEP */
	LX_SE_RSA_OAEP_SHA256, 	/**< RSA-OAEP (Hash=SHA256) */
	LX_SE_RSA_EXP,			/**< RSA-Exponantation */
}	LX_SE_CRYPTO_TYPE_T;

/**
 *	Residual Mode
 */
typedef enum {
	LX_SE_CLEAR =0,			/**< Clear */
	LX_SE_CTS,				/**< CTS */
	LX_SE_OFB,				/**< OFB */
	LX_SE_SCTE,				/**< SCTE */
	LX_SE_PADDING_ENABLE,	/**< PADDING Enable */
	LX_SE_PADDING_DISABLE,	/**< PADDING Disable */
	LX_SE_NO_RES			/**< No Residual */
}	LX_SE_CRYPTO_RES_MODE_T;

/**
 *	Block Mode
 */
typedef enum {
	LX_SE_ECB =0,			/**< ECB */
	LX_SE_CBC,				/**< CBC */
	LX_SE_CTR,				/**< CTR */
	LX_SE_OMAC1,			/**< OMAC1 */
	LX_SE_SHA1,				/**< SHA1 */
	LX_SE_SHA224,			/**< SHA224 */
	LX_SE_SHA256,			/**< SHA256 */
	LX_SE_HMAC_SHA1,		/**< HMAC SHA1 */
	LX_SE_HMAC_SHA224,		/**< HMAC SHA224 */
	LX_SE_HMAC_SHA256,		/**< HMAC SHA256 */
	LX_SE_BLK_MD5,			/**< MD5 */
	LX_SE_BLK_HMAC_MD5,		/**< HMAC MD5 */
}	LX_SE_CRYPTO_BLK_MODE_T;

/**
 *	MPEG2 TS Key Mode, use with LX_SE_MPEG2TS_188 or LX_SE_MPEG2TS_192
 */
typedef enum {
	LX_SE_EVEN_KEY=0,	/**< Even Key */
	LX_SE_ODD_KEY		/**< Odd Key */
}	LX_SE_TS_MODE_T;

/**
 *	MPEG2 TS Format
 */
typedef enum {
	LX_SE_NOT_MPEG2TS=0,	/**< Normal */
	LX_SE_MPEG2TS_188,		/**< MPEG2 TS 188 */
	LX_SE_MPEG2TS_192		/**< Time Stamp(4) + MPEG2 TS 188 */
}	LX_SE_TS_FORMAT_T;

/**
 *	Secure Type
 */
typedef enum {
	LX_SE_SECURE_CLEAR = 0,	/**< Clear Data */
	LX_SE_SECURE_COMMON, 	/**< Secure Data with HW Common Key */
	LX_SE_SECURE_UNIQUE,	/**< Secure Data with HW Unique Key */
	LX_SE_SECURE_PREV,		/**< Secure Data with Previous Key(Currently setted) */
} LX_SE_SECURE_TYPE_T;

/**
 *	Set Type
 */
typedef enum {
	LX_SE_SECURE_KEY 		= 0,	/**< Set as a key */
	LX_SE_SECURE_IV,				/**< Set as a IV */
	LX_SE_SECURE_PKCS8,				/**< Read PKCS#8 and set as a key */
	LX_SE_SECURE_BASE64,			/**< Read Base64 Encoded and set as a key */
	LX_SE_SECURE_GEN_DH,			/**< Generate Diffie-Hellman Private Key */
	LX_SE_SECURE_PKCS1_PRIVATE_KEY,	/**< Read PKCS#1 Private Key and set as a Key */
	LX_SE_SECURE_ODD_KEY	= 0x80,	/**< ODD Key : After H13 */
	LX_SE_SECURE_ODD_IV		= 0x81,	/**< ODD IV : After H13 */
} LX_SE_SECURE_SET_T;

/**
 *	Hash data type
 */
typedef enum {
	LX_SE_HASH_PARTIAL_START 	= 0,	/**< Partial : Block Start */
	LX_SE_HASH_PARTIAL_MIDDLE	= 1,	/**< Partial : Block in the middle */
	LX_SE_HASH_PARTIAL_END		= 2,	/**< Partial : Block End */
	LX_SE_HASH_WHOLE 			= 3		/**< Whole : Get hash value in a dma op */
} LX_SE_HASH_DATA_T;

/**
 *	Debug Lock type
 */
typedef enum {
	LX_SE_DEBUG_UART0		= 0x01,	/**< UART0 Lock */
	LX_SE_DEBUG_UART1		= 0x02,	/**< UART1 Lock */
	LX_SE_DEBUG_UART2		= 0x04,	/**< UART2 Lock */
	LX_SE_DEBUG_JTAG_MCU 	= 0x08,	/**< MCU JTAG Lock */
	LX_SE_DEBUG_JTAG_MAIN 	= 0x10	/**< MAIN JTAG Lock */
} LX_SE_DEBUG_T;

/**
 *	DMA Memory Info
 */
typedef struct LX_SE_MEM_INFO_tag {
	UINT32 physicalBase;	/**< SE DMA Memory Physical Base */
	UINT32 size;			/**< SE DMA Memory Size */
} LX_SE_MEM_INFO_T;

/**
 *	Data
 */
typedef struct LX_SE_DATA_tag {
	UINT8 *pData;				/**< Data Address */
	UINT32 length;				/**< Length */
} LX_SE_DATA_T;

/**
 *	Block Operation
 */
typedef struct LX_SE_BLOCK_OP_tag {
	LX_SE_CRYPTO_OPERATION_T operation;	/**< Operation : LX_SE_ENCRYPTION, LX_SE_DECRYPTION, LX_SE_PASSTHRU */
	LX_SE_CRYPTO_RES_MODE_T resMode;	/**< Residual Mode : LX_SE_CLEAR, LX_SE_CTS, LX_SE_OFB, LX_SE_SCTE, LX_SE_PADDING_ENABLE, LX_SE_PADDING_DISABLE, LX_SE_NO_RES */
	LX_SE_CRYPTO_BLK_MODE_T blkMode;	/**< Block Mode : LX_SE_ECB, LX_SE_CBC, LX_SE_CTR */
	LX_SE_CRYPTO_TYPE_T cryptoType; 	/**< Cyrpto Type : LX_SE_BYPASS, LX_SE_DES, LX_SE_TDES, LX_SE_AES, LX_SE_XOR */
	LX_SE_TS_MODE_T tsMode;				/**< MPEG2 TS Mode : LX_SE_EVEN_KEY, LX_SE_ODD_KEY */
	LX_SE_TS_FORMAT_T tsFormat;			/**< Source format : LX_SE_NOT_MPEG2TS, LX_SE_MPEG2TS_188, LX_SE_MPEG2TS_192 */
	UINT32 keyLength;					/**< Key Byte Length(Currently setted) */

	UINT32 srcAddr; 					/**< Source Physical Address */
	UINT32 dstAddr; 					/**< Destination Physical Address */
	UINT32 dataLength;					/**< Source Length */

	/* process unit set : input data stream has pattern, ex) TS 188: 4byte (need to skip) + 184 byte (need to process) */
	UINT32 processLoadLength;			/**< total process unit = processLoadLength + processSkipLength */
	UINT32 processSkipLength;			/**< total process unit should be aligned 32bit */
} LX_SE_BLOCK_OP_T;

/**
 *	Hash Operation
 */
typedef struct LX_SE_HASH_OP_tag {
	LX_SE_CRYPTO_RES_MODE_T resMode;	/**< Residual Mode : LX_SE_CLEAR, LX_SE_CTS, LX_SE_OFB, LX_SE_SCTE, LX_SE_PADDING_ENABLE, LX_SE_PADDING_DISABLE, LX_SE_NO_RES */
	LX_SE_CRYPTO_BLK_MODE_T blkMode;	/**< Block Mode : LX_SE_OMAC_1, LX_SE_SHA_1, LX_SE_SHA_256, LX_SE_HMAC_SHA_1, LX_SE_HMAC_SHA_256 */
	LX_SE_CRYPTO_TYPE_T cryptoType; 	/**< Cyrpto Type : LX_SE_MULTISWAP, LX_SE_RC4, LX_SE_SHA, LX_SE_HMAC_SHA */
	LX_SE_HASH_DATA_T seqSrcState;  	/**< the input data length is different from the output data length */
	UINT32 keyLength;					/**< Key Byte Length(Currently setted) */

	UINT32 srcAddr;						/**< Source Physical Address */
	UINT32 dstAddr;						/**< Destination Physical Address */
	UINT32 dataLength;					/**< Source Length */

	UINT32 processLoadLength;			/**< total src length */
} LX_SE_HASH_OP_T;

/**
 *	RSA Operation
 */
typedef struct LX_SE_RSA_OP_tag {
	LX_SE_CRYPTO_OPERATION_T operation;	/**< Operation : LX_SE_ENCRYPTION, LX_SE_DECRYPTION, LX_SE_PASSTHRU */
	LX_SE_CRYPTO_TYPE_T cryptoType; 	/**< Cyrpto Type : LX_SE_RSA_1_5, LX_SE_RSA_OAEP, LX_SE_RSA_EXP */
	UINT32 keyLength;					/**< Setted Key Length */

	UINT32 srcAddr;						/**< Source Physical Address */
	UINT32 dstAddr;						/**< Destination Physical Address */
	UINT32 dataLength;					/**< Source Length */

	UINT32 returnLength;				/**< Return Decrypted Length */
} LX_SE_RSA_OP_T;

/**
 *	Secure Data for Secure Key Chain
 */
typedef struct LX_SE_SECURE_DATA_tag {
	LX_SE_SECURE_TYPE_T secureType;		/**< Secure Type : LX_SE_SECURE_CLEAR, LX_SE_SECURE_COMMON, LX_SE_SECURE_UNIQUE, LX_SE_SECURE_PREV */
	LX_SE_CRYPTO_OPERATION_T operation;	/**< Operation : LX_SE_ENCRYPTION, LX_SE_DECRYPTION */
	LX_SE_CRYPTO_RES_MODE_T resMode;	/**< Residual Mode : LX_SE_CLEAR, LX_SE_CTS, LX_SE_OFB, LX_SE_SCTE, LX_SE_PADDING_ENABLE, LX_SE_PADDING_DISABLE, LX_SE_NO_RES */
	LX_SE_CRYPTO_BLK_MODE_T blkMode;	/**< Block Mode : LX_SE_ECB, LX_SE_CBC, LX_SE_CTR, LX_SE_OMAC_1, LX_SE_SHA_1, LX_SE_SHA_256, LX_SE_HMAC_SHA_1, LX_SE_HMAC_SHA_256 */
	LX_SE_CRYPTO_TYPE_T cryptoType;		/**< Cyrpto Type : LX_SE_BYPASS, LX_SE_DES, LX_SE_TDES, LX_SE_AES, LX_SE_SHA, LX_SE_TDES_BAB, LX_SE_XOR, LX_SE_RSA_1_5, LX_SE_RSA_OAEP */
	UINT32 keyLength;					/**< Setted Key Length */

	UINT32 length;						/**< Key Data Length */
	UINT8 *pData;						/**< Key Data */

	LX_SE_SECURE_SET_T setType;			/**< Secure Set Type : LX_SE_SECURE_KEY, LX_SE_SECURE_IV, LX_SE_SECURE_PKCS8, LX_SE_SECURE_BASE64, LX_SE_SECURE_GEN_DH, LX_SE_SECURE_ODD_KEY, LX_SE_SECURE_ODD_IV */
	UINT32 setOffset;					/**< Start Offset to set as a key or iv*/
	UINT32 setLength;					/**< Length to set as a key or iv */

	UINT32 returnOffset;				/**< Return Offset in the decrypted data */
	UINT32 returnLength;				/**< Return Length in the decrypted data */
	UINT8 *pReturnData;					/**< Returned decrypted data */
} LX_SE_SECURE_DATA_T;

/**
 *	NetFlix DRM Agent Operation
 */
typedef struct LX_SE_NF_OP_tag {
	UINT32 operation;					/**< Operation */
	UINT32 result;						/**< Result */
	UINT8 *pArg;						/**< Arguments */
	UINT32 argLength;					/**< Arguments Legnth */
	UINT32 srcAddr;						/**< Source Physical Address */
	UINT32 dstAddr;						/**< Destination Physical Address */
} LX_SE_NF_OP_T;

/**
 *	Trust Operation
 */
typedef struct LX_SE_TRUST_OP_tag {
	UINT32 operation;					/**< Operation */
	UINT32 result;						/**< Result */
	UINT8 *pArg;						/**< Arguments */
	UINT32 argLength;					/**< Arguments Legnth */
} LX_SE_TRUST_OP_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _SE_KAPI_H_ */

/** @} */
