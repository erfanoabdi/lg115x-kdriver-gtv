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


#ifndef	_REG_DBG_H_
#define	_REG_DBG_H_

/*-----------------------------------------------------------------------------
	제어 상수
	(Control Constants)
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
    #include 파일들
    (File Inclusions)
------------------------------------------------------------------------------*/

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*-----------------------------------------------------------------------------
	상수 정의
	(Constant Definitions)
------------------------------------------------------------------------------*/

/** @addtogroup reg_dbg
 * @{ */

/** Register XS(Access) type definition */
/* DO NOT USE Directly */
#define REG_XS_R_			0x01					///< Read       accessible
#define REG_XS__W			0x02					///< Write      accessible
#define REG_XS_RW			(REG_XS_R_|REG_XS__W)	///< Read/Write accessible
#define REG_XS_MSK_RW		REG_XS_RW				///< Accessibility mask

#define REG_XS_WR_PSTV		0x04					///< Write Bit Polarity  Mask
#define REG_XS_HAS_WR		0x08					///< Write Bit Existence Mask
#define REG_XS_WR__			0x00					///< just dummy
#define REG_XS_WR_P			(REG_XS_HAS_WR|REG_XS_WR_PSTV)		///< write bit 1 when write
#define REG_XS_WR_N			(REG_XS_HAS_WR )		///< Write bit 0 when write

#define REG_XS_NORM			0x00					///< NORmal  access
#define REG_XS_IDX			0x10					///< Indexed access
#define REG_XS_MSK_TYPE		REG_XS_IDX				///< Access type mask

/*
 * ONLY Use below definition.
 *
 * REG_XS_N... : Normal  Access Register
 * REG_XS_I... : Indexed Access Register
 * REG_XS_._.. : Without Write bit.
 * REG_XS_.P.. : with Positive Write bit. '1' to write
 * REG_XS_.N.. : with Negative Write bit. '0' to write
 * REG_XS_..R. :     Readable Register
 * REG_XS_.._. : NOT Readable Register
 * REG_XS_...W :     Writable Register
 * REG_XS_..._ : NOT Writable Register
 */
#define REG_XS_N_R_		(REG_XS_NORM | REG_XS_WR_P | REG_XS_R_)
#define REG_XS_N__W		(REG_XS_NORM | REG_XS_WR__ | REG_XS__W)
#define REG_XS_N_RW		(REG_XS_NORM | REG_XS_WR__ | REG_XS_RW)
#define REG_XS_NPR_		(REG_XS_NORM | REG_XS_WR_P | REG_XS_R_)
#define REG_XS_NP_W		(REG_XS_NORM | REG_XS_WR_P | REG_XS__W)
#define REG_XS_NPRW		(REG_XS_NORM | REG_XS_WR_P | REG_XS_RW)
#define REG_XS_NNR_		(REG_XS_NORM | REG_XS_WR_N | REG_XS_R_)
#define REG_XS_NN_W		(REG_XS_NORM | REG_XS_WR_N | REG_XS__W)
#define REG_XS_NNRW		(REG_XS_NORM | REG_XS_WR_N | REG_XS_RW)
#define REG_XS_I_R_		(REG_XS_IDX  | REG_XS_WR__ | REG_XS_R_)
#define REG_XS_I__W		(REG_XS_IDX  | REG_XS_WR__ | REG_XS__W)
#define REG_XS_I_RW		(REG_XS_IDX  | REG_XS_WR__ | REG_XS_RW)
#define REG_XS_IPR_		(REG_XS_IDX  | REG_XS_WR_P | REG_XS_R_)
#define REG_XS_IP_W		(REG_XS_IDX  | REG_XS_WR_P | REG_XS__W)
#define REG_XS_IPRW		(REG_XS_IDX  | REG_XS_WR_P | REG_XS_RW)
#define REG_XS_INR_		(REG_XS_IDX  | REG_XS_WR_N | REG_XS_R_)
#define REG_XS_IN_W		(REG_XS_IDX  | REG_XS_WR_N | REG_XS__W)
#define REG_XS_INRW		(REG_XS_IDX  | REG_XS_WR_N | REG_XS_RW)


/*-----------------------------------------------------------------------------
	매크로 함수 정의
	(Macro Definitions)
------------------------------------------------------------------------------*/
#define N_FLD(_FldArray)	(sizeof((_FldArray))/sizeof((_FldArray[0])))


/*----------------------------------------------------------------------------
    형 정의
    (Type Definitions)
------------------------------------------------------------------------------*/
/** General Debug Field Type.
 *
 * 각 Field의 access type,
 * 해당 Field의 LSB 위치(32비트 레지스터의 LSB에서부터)
 * 해당 Field의 MSB 위치
 * Index Bit의 위치, 비트 width,
 * Read/Write Bit의 위치 및 Polarity
 * 등을 저장할 수 있음.
 *
 */
typedef struct {
	unsigned int
			type: 5,	///> access type. REG_XS_[NI][NP_][_R][_W]
			wP  : 5,	///> Read/Write  bit position from LSB
			_rsv: 2,	///> reserved.
			iP  : 5,	///> Index field bit position from LSB
			iW  : 5,	///> Index field bit width
			bMsb: 5,	///> field bit position from LSB
			bLsb: 5;	///> field bit width
				  		//   0x00

	char	*fName;		///> 0x04 : Field name
						//   0x08
}	DBG_REG_FIELD_T;

/** General Debug Register Type.
 *
 * @li 각 레지스터의 offset address (16bit from BASE ADDRESS),
 * @li 레지스터의 type,
 * @li Field의 array,
 * @li 레지스터의 이름
 * 들을 저장하는 형식.
 *
 * @see DBG_REG_FIELD_T
 * @ingroup reg_dbg
 */
typedef struct {
	UINT16					rAddr;	///> 0x00 : address
	UINT16					nFld;	///> 0x02 : address
	char					*rName;	///> 0x04 : name
	const DBG_REG_FIELD_T	*pFld;	///> 0x08 : 속한 Field들의 array
									//   0x0c :
}	DBG_REG_T;

/** @} */	// reg_dbg

/*-----------------------------------------------------------------------------
	Extern 전역변수와 함수 prototype 선언
	(Extern Variables & Function Prototype Declarations)
------------------------------------------------------------------------------*/
extern DBG_REG_T*	FindGenReg(char *pTarget, DBG_REG_T* pFindStart);
extern int			PrGenReg(DBG_REG_T	*pReg, int mode);
extern unsigned int		RdGenReg(DBG_REG_T *pReg, DBG_REG_FIELD_T *pFld);
extern unsigned int		WrGenReg(DBG_REG_T *pReg, DBG_REG_FIELD_T * pFld, unsigned int RegValue);
extern void			DumpGenReg(char *startAddr, char *sizeStr, unsigned int option);
int		debug_RegDebug(void);
int		CLI_DE_RegDebug(int argc ,char **argv);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _REG_DBG_H_ */
