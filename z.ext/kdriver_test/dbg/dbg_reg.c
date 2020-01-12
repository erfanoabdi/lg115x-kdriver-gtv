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


/*-----------------------------------------------------------------------------
	전역 제어 상수
	(Control Constants)
------------------------------------------------------------------------------*/
#define USE_DE_REG_DBG

#define _REG_DBG_C_
/*-----------------------------------------------------------------------------
	#include 파일들
	(File Inclusions)
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <termios.h>
#include <unistd.h>

#include "base_types.h"

#include "cli/pc_key.h"
#include "cli/cli_main.h"
#include "dbg/dbg_reg.h"

#ifdef USE_DE_REG_DBG
#ifdef INCLUDE_KDRV_DE
#include "de_ipc_def.h"
#include "de_reg_def.h"
#include "de/de_test.h"
#include "de/de_cli.h"

#include "de_mif_regdb.h"
#include "de_cvi_regdb.h"
#include "de_ov_regdb.h"
#include "de_top_regdb.h"
#include "de_msc_regdb.h"
#include "de_ssc_regdb.h"
#include "de_oif_regdb.h"
#include "ctop_regdb.h"
#endif
#ifdef INCLUDE_KDRV_FMT3D
#include "fmt3d_regdb.h"
#endif
#ifdef INCLUDE_KDRV_PQL
#include "pe0_regdb.h"
#include "pe1_regdb.h"
#endif
#ifdef INCLUDE_KDRV_DENC
#include "denc_regdb.h"
#endif
#ifdef INCLUDE_KDRV_TCON
#include "tcon_regdb.h"
#endif
#ifdef INCLUDE_KDRV_DIMM
#include "dimm_regdb.h"
#endif
#endif

/*-----------------------------------------------------------------------------
	상수 정의
	(Constant Definitions)
------------------------------------------------------------------------------*/
/** @addtogroup reg_dbg
 * @{ */
#define REG_MOD_REG		0x80000000
#define REG_MOD_FLD		0x40000000
#define REG_MOD_VAL		0x20000000

#define VENC_REG_OFFSET 0x2000
#define DE_REG_OFFSET	0x7000
#define TE_REG_OFFSET	0x9000

#define DE_REG_SIZE		0x2000

//#define DE_REG_BASE		(DTV_SOC_REG + DE_REG_OFFSET)
//#define DE_REG_END		(DE_REG_BASE + DE_REG_SIZE)
#define DE_REG_BASE		(DTVSOC_DE_BASE)
#define DE_REG_END		(DTVSOC_DE_BASE + DE_REG_SIZE)

#define DE

/*-----------------------------------------------------------------------------
	매크로 함수 정의
	(Macro Definitions)
------------------------------------------------------------------------------*/
//#define Rd32(_a)	((UINT32)*((volatile UINT32*)_a))
//#define Rd32(_a)	MemoryRead(_a)
//#define Wr32(_a,_v) MemoryWrite(_a, _v)
#define Rd32(_a)	DE_CLI_RD(_a)
#define Wr32(_a,_v) DE_CLI_WD(_a, _v)

//#define Wr32(_a,_v)	*((volatile UINT32*)_a) = (UINT32)(_v)

/*-----------------------------------------------------------------------------
	형 정의
	(Type Definitions)
------------------------------------------------------------------------------*/

typedef struct {
	DBG_REG_T *pMdl;
	UINT32	  BAdr;
	UINT32	  nReg;
	UINT32	  iReg;
	UINT32	  iFld;
} DBG_MODULE_T;

/*-----------------------------------------------------------------------------
	외부 전역변수와 외부 함수 prototype 선언
	(Extern Variables & External Function Prototype Declarations)
------------------------------------------------------------------------------*/
//extern int UartGetChar(FILE *fp);
//int MemoryWrite(UINT32 i_uiPhysicalAdd, UINT32 i_uiValue);
//UINT32 MemoryRead(UINT32 i_uiPhyAdd);

/*-----------------------------------------------------------------------------
	전역 변수 정의와 전역 함수 Prototype 선언.
	(Define global variables)
------------------------------------------------------------------------------*/
#ifdef USE_DE_REG_DBG
#ifdef INCLUDE_KDRV_DE
static int 		  snDbgRegVdp = N_FLD(gDbgRegTOP) - 1;	// exclude end marker
static DBG_REG_T *gDbgRegInfo = gDbgRegTOP;
static UINT32 	  saBaseAddr  = DTVSOC_DE_BASE;

static DBG_MODULE_T gDebugModule[] = {
	{ gDbgRegTOP	,DTVSOC_DE_BASE			,N_FLD(gDbgRegTOP)-1	,0 ,0 }, // 0x0000
	{ gDbgRegCVI	,DTVSOC_DE_BASE			,N_FLD(gDbgRegCVI)-1	,0 ,0 }, // 0x0100
	{ gDbgRegMSC	,DTVSOC_DE_BASE			,N_FLD(gDbgRegMSC)-1	,0 ,0 }, // 0x0500
	{ gDbgRegMIF	,DTVSOC_DE_BASE			,N_FLD(gDbgRegMIF)-1	,0 ,0 }, // 0x0600
	{ gDbgRegSSC	,DTVSOC_DE_BASE			,N_FLD(gDbgRegSSC)-1	,0 ,0 }, // 0x0900
	{ gDbgRegOV		,DTVSOC_DE_BASE			,N_FLD(gDbgRegOV)-1		,0 ,0 }, // 0x1100
#ifdef INCLUDE_KDRV_FMT3D
	{ gDbgRegFMT3D	,DTVSOC_DE_BASE			,N_FLD(gDbgRegFMT3D)-1	,0 ,0 }, // 0x0200
#endif
#ifdef INCLUDE_KDRV_PQL
	{ gDbgRegPE0	,DTVSOC_DE_BASE			,N_FLD(gDbgRegPE0)-1	,0 ,0 }, // 0x0300
	{ gDbgRegPE1	,DTVSOC_DE_BASE			,N_FLD(gDbgRegPE1)-1	,0 ,0 }, // 0x1200
#endif
#ifdef INCLUDE_KDRV_DENC
	{ gDbgRegDENC	,DTVSOC_DE_BASE			,N_FLD(gDbgRegDENC)-1	,0 ,0 }, // 0x1900
#endif

#ifdef INCLUDE_KDRV_TCON
	{ gDbgRegTCON	,DTVSOC_TCON_BASE     	,N_FLD(gDbgRegTCON)-1	,0 ,0 }, // 0x1a000
#endif
#ifdef INCLUDE_KDRV_DIMM
	{ gDbgRegDIMM	,DTVSOC_LED_BASE     	,N_FLD(gDbgRegDIMM)-1	,0 ,0 }, // 0x1c000
#endif
	{ gDbgRegOIF	,DTVSOC_OIF_BASE		,N_FLD(gDbgRegOIF)-1	,0 ,0 }, // 0x1c800
	{ gDbgRegCTOP	,DTVSOC_TOP_CTRL		,N_FLD(gDbgRegCTOP)-1	,0 ,0 }, // 0xfd300000
};

const UINT32	BF_MSK[] = {
	0x00000000,
	0x00000001, 0x00000003, 0x00000007, 0x0000000f,
	0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
	0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
	0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
	0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
	0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
	0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
	0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};

/*-----------------------------------------------------------------------------
	Static 변수와 Static 함수 prototype 선언
	(Static Variables & Function Prototypes Declarations)
------------------------------------------------------------------------------*/
static const char REG_DBG_HELP_TO_EXIT[]=	"press 'q' to exit";
#endif
#endif

/*-----------------------------------------------------------------------------
	Static 함수와 Global 함수 구현
	(Implementation of static and global functions)
------------------------------------------------------------------------------*/
SINT32		gnEditMemory	= 0;

/*=============================================================================
	General Register Debugging Packages
==============================================================================*/
int mygetch(void)
{
	struct termios oldt,
				   newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

/*
**	An Extended strtoul() to support binary input
*/
unsigned long
strtoul2(char *s, char **ptr, int base)
{
	unsigned long res;
	char *t;

	if (s == NULL)
	{
		if (ptr != NULL) *ptr = NULL;
		return 0;
	}

	/*	Remove heading spaces	*/
	while ((*s == '\t') || (*s == ' '))
	{
		if (*s == '\0') break;
		s++;
	}

	if ((base == 0) && (*s == 'b'))
	{
		base = 2;
		s++;
	}

	if (base == 2)
	{
		for (t=s; (*t == '0') || (*t == '1'); t++)
			;
	}
	else
	{
		for (t=s; isxdigit(*t) || (*t == 'x') || (*t == 'X'); t++)
			;
	}

	res = strtoul(s, ptr, base);

	switch (tolower(*t))
	{
		case 'r' : res = (res << 13); break;
		case 'k' : res = (res << 10); break;
		case 'm' : res = (res << 20); break;
		case 'g' : res = (res << 30); break;
	}

	return(res);
}

static int ProcessSerialInput( unsigned char inChar )
{
	static unsigned int inCode = 0;
	static signed	int index  = 0;
	unsigned long	key		   = 0;

	index++;

	if (inChar == 0x1B) inCode = 0;
	else				inCode = (inCode << 8) | inChar;

	if		((index == 1) && (inChar != 0x1B)) key = inChar;
	else if ((index == 2) && (inChar != 0x5B)) key = inChar;
	else if ((index == 3) && (inChar  > 0x40)) key = inCode;
	else if ((index == 4) && (inChar == 0x7E)) key = inCode;
	else if ((index == 5) && (inChar == 0x7E)) key = inCode;
	else if ( index >  5)			index = inCode = 0;

	if (key)
	{
		index  = 0;
		inCode = 0;
	}
	return(key);
}

int UartGetChar ( FILE *fp )
{
	int inKey = 0;
	int ch;

	while ((ch = mygetch()) != 0)
	{
		inKey = ProcessSerialInput(ch);
		if (inKey != 0) break;
	}

	return inKey;
}

/*====================================================================
Function Name	:	UartGetString()
Version			:	0.1
Create Date		:	2002,09,03
Author			:	Jackee
Description		:	Get string from file pointer.
Input			:
Output			:
Remarks			:	Extension of fgets with key code filter.
====================================================================*/
char *UartGetString(char *buf, int n, FILE *fp)
{
	int		key = 0, i;
//	  int j;
//	  int		oldLevel;

	if ((buf == NULL) || (n < 1)) return NULL;


	/*	Turn off canonical/echo mode to process escape sequence */
//	  setTerminalMode(~ECHO);

	for (i=0; (key != '\n') && (i < (n-1)); )
	{
		key = UartGetChar(fp);

		if (key >= 0x100)
		{
			/* Process Cursor up/down key when editing memory	*/
			if (gnEditMemory)
			{
				buf[0] = '\0';
				if (key == PC_KEY_UP  ) strcpy(buf, "k\n");
				if (key == PC_KEY_DOWN) strcpy(buf, "j\n");

				if (buf[0] != '\0')
				{
					return(buf);
				}
			}

			/* Process Escape Sequence							*/
			//			  (void)CheckSpecialFunctions(key);
		}
		else if ( (gnEditMemory && ((key == 'j') || (key == 'k'))) || (key == 0x04) )
		{
			sprintf(buf, "%c\n", key);
			return(buf);
		}
		else
		{
			/*
			 *	While in interrupt mode, key press counter is not updated.
			 *	so, we must add it manually
			 */
			//			  setTerminalMode(ECHO);		/*	Turn on echo mode	*/

			if (key == '\b')
			{
				if (i > 0)
				{
					i--;
					printf("\b \b");
				}
			}
			else if (key == '\n')
			{
				buf[i++] = key;
				break;
			}
			else
			{
				buf[i++] = key;
				putchar(key);

				//if ( i >= n-1 ) putchar('\n');
			}
			//				  fflush(stdout);

			//setTerminalMode(ICANON);	/* Turn on canonical mode */
			//			  setTerminalMode(~ECHO);
		}
	}
	buf[i] = '\0';
	return(buf);
}

UINT32	ReadHexKeyInput( char * comment )
{
	char   inputStr[20]= "0x";

	printf("%s: 0x", comment);

	(void)UartGetString(&inputStr[2], 17, stdin);

	return( strtoul( inputStr, (char **) NULL, 16 ) );
}

#ifdef USE_DE_REG_DBG
#ifdef INCLUDE_KDRV_DE
/** Reads general A/V register.
 *
 * @param pReg	Register Pointer to print.
 * @param pFld	Field Pointer to print. : if NULL, whole 32bit value returned.
 * @return whole 32bit value read.
 */
UINT32 RdGenReg(DBG_REG_T *pReg, DBG_REG_FIELD_T *pFld)
{
	UINT32	rval = 0;
	UINT32	mskWr= 0;
	UINT32	rAddr= saBaseAddr + pReg->rAddr;
	UINT32	type;

	if (pFld == NULL ) return Rd32(rAddr);

	type = pFld->type;

	//TODO : support Ready Only, Write Only.
	{
		UINT32 r4rd = rval;
		rval = Rd32(rAddr);
//printf("Reg[0x%08x] = 0x%08x\n", rAddr, rval);

		if ( type & REG_XS_HAS_WR)
		{
			mskWr =    (1 << pFld->wP);   // wr bit 0 when read

			if ( type & REG_XS_WR_PSTV) r4rd &= (~mskWr); // wr bit 0 when read
			else						r4rd |=   mskWr;  // wr bit 1 when read

			Wr32(rAddr, r4rd);

			// now read real value
			r4rd = Rd32(rAddr);

			rval = (rval & mskWr ) | (r4rd & (~mskWr));

			Wr32(rAddr, rval);
		}
	}

	return rval;
}

/** Writes general A/V register.
 *
 * @param pReg		Register Pointer to print.
 * @param pFld		Field Pointer to print.
 * @param RegValue	value to write.
 * @return 32bit value written or 0 on error.
 */
UINT32 WrGenReg(DBG_REG_T *pReg, DBG_REG_FIELD_T * pFld, UINT32 RegValue)
{
	UINT32	r4wr = RegValue;
	UINT32	rAddr;

	if ( !pReg ) return 0;

	rAddr= saBaseAddr + pReg->rAddr;

	if ( pFld )
	{
		UINT32	mskWr = 0;
		UINT32	type = pFld->type;

		if ( type & REG_XS_HAS_WR)
		{
			mskWr= (1<< pFld->wP);
			if ( type & REG_XS_WR_PSTV) r4wr |=   mskWr;  // wr bit 1, when write
			else						r4wr &= (~mskWr); // wr bit 0, when write

			Wr32(rAddr, r4wr);

		}
	}
	Wr32(rAddr, RegValue);

	// for PA Register.
	if ( (DE_REG_BASE <= rAddr) && (rAddr <= DE_REG_END) )
	{
//		  extern void vdp_UpdatePaRegister( int updMode );
//		  vdp_UpdatePaRegister( 1 );
	}

	return RegValue;
}

/** Prints general A/V Register.
 * @param pReg		pointer to Register to print.
 * @param option	0 full, else... TODO
 */
int PrGenReg(DBG_REG_T	*pReg, int option)
{
	int		fidx;
	DBG_REG_FIELD_T *fldList;
	UINT32	value = 0x12345678;
	int		printed = 0;

	if ( !pReg ) return 0;

	value = RdGenReg(pReg, NULL);

	printf("%04x:0x%08x : %s", pReg->rAddr, value, pReg->rName);

	// TODO : support long/short format, uncomment below line
	// if ( !(option & DBG_DUMP_REG_LONG ) ) continue;

	fldList = (DBG_REG_FIELD_T*)(pReg->pFld);

	for ( fidx = 0; fidx < pReg->nFld ; fidx ++ )
	{
		UINT32			bLsb, bMsb;
		UINT32			fld_value;
		DBG_REG_FIELD_T *pFld;

		pFld = (DBG_REG_FIELD_T*)(&fldList[fidx]);
		bMsb = pFld->bMsb;
		bLsb = pFld->bLsb;
		fld_value = (value&(BF_MSK[bMsb+1])&(~BF_MSK[bLsb]))>>bLsb;

		printf(" %2d:%2d,%-8x(%5d):%s",
				bMsb, bLsb,fld_value, fld_value, pFld->fName);

		printed = 1;

		if ( bLsb == 0 )
			break;
	}

	return printed;
}

/** finds general A/V register.
 *
 * @param pTarget	[IN] Pointer to find.
 *					[1,0xffff] : assume address of register.
 *					[D2A_BASE_ADDR+1, D2A_BASE_ADDR+0xFFFF] : same above.
 *					"0x......" : try to get address value from string.
 *					"vdp_..."  : try to find matched register name from gDbgRegInfo[]
 * @param pFindStart [IN] Pointer to Previously Found Pointer, if NULL, search from gDbgRegInfo[0]
 * @return pointer to General A/V register when succeeded, NULL when error,
 * @see gDbgRegInfo
 */
DBG_REG_T* FindGenReg(char *pTarget, DBG_REG_T* pFindStart)
{
	int			findMode= -1;	// -1 : err, 0 : addr, or addr string, 1 : regName string
	int			found	= 0;
	int			nReg	= snDbgRegVdp;
	UINT32		addrMin = gDbgRegInfo[0		].rAddr;	// short address.
	UINT32		addrMax = gDbgRegInfo[nReg-1].rAddr;	// short address.
	UINT32		tAddr	= 0xffffffff;					// search sub address key
	DBG_REG_T	*pReg	= (DBG_REG_T*)&gDbgRegInfo[0];;

	const UINT32	baseAddrMask = 0xffff0000;
	const UINT32	subAddrMask  = 0x0000fffc;

	if ( !pTarget ) return NULL;

	// pFindStart always widthin gDbgRegInfo array.
	if ( pFindStart )
	{
		if ( (&gDbgRegInfo[0] <= pFindStart) && (pFindStart <= &gDbgRegInfo[nReg-1]) )
			pReg = pFindStart;
		else
			return NULL;
	}

	// if D2A Base addr matched, Trim MS 16 bit & LS 2 bit
	tAddr = (UINT32)pTarget;
	if ( (tAddr & baseAddrMask) == saBaseAddr )
		tAddr &= subAddrMask;

	// if valid address range.
	if ( (addrMin <= tAddr) && (tAddr <= addrMax ))
	{
		findMode = 0;
	}
	else if ( (tAddr = strtoul2(pTarget, NULL, 0)))
	{
		UINT32	base = tAddr & baseAddrMask;

		// if valid base address && valid address range.
		if ( !base || (base == saBaseAddr))
		{
			tAddr &= subAddrMask;

			if ( (addrMin <= tAddr) && (tAddr <= addrMax ))
			{
				findMode = 0;
			}
		}
	}
	else
	{
		findMode = 1;
	}

	// search in gDbgRegInfo[]
	if ( findMode == 0 )
	{
		// TODO need binary search?
		while (nReg-- > 0 )
		{
			if ( pReg->rAddr == 0 ) break;	// check end marker

			if ( pReg->rAddr == tAddr)
			{
				found = 1;
				break;
			}
			pReg++;
		}
	}
	else if ( findMode == 1 )
	{
		char	tRegName[32+1] = "";
		char	*pRegName = &tRegName[0];
		int		nLen =0;

		while ( nLen < 32 )
		{
			if ( *pTarget == 0 ) break;
			if ( *pTarget == '*' || *pTarget == '?') break;

			*pRegName++ = toupper(*pTarget++);
			nLen++;
		}

		while (nLen)
		{
			if ( pReg->rAddr == 0 ) break;	// check end marker

			if ( !strncmp(tRegName, pReg->rName, nLen))
			{
				found = 1;
				break;
			}
			pReg++;
		}
	}

	if ( found )
	{
		return pReg;
	}
	else
		return NULL;
}

/**
 * Register Debugging용.
 * Serial Port를 통해 PC Keyboard의 화살표 키를 이용하여,\n
 * 레지스터, 각 필드, 필드의 값을 수정할 수 있도록 함.\n
 * 항목(레지스터,필드,값)의 수정은 아래(increment), 위(decrement)로,\n
 * 항목(레지스터,필드,값)의 전환은 좌, 우 화살표로 하게 되어 있음.\n
 * TODO : 현재는 1 단위 수정이나, 10 단위 수정 필요.
 *
 * @param	key		serial input key.
 * @param	option	bit 1 : restart from REG_MOD_REG.
 */
void ModifyGenReg( UINT32 key, UINT32 option)
{
	DBG_REG_T		*pReg;
	DBG_REG_FIELD_T *pFld;
	static int		siReg=0, siFld=0;	// always should be valid
	static int		step = 1;			// increment/decrement Step
	static int		siMdl=0;
	int				inc=0;
	int				opMode = 0;			// 0 step cursor 1 direct cursor 2 direct value
	int				nReg, nFld;
	UINT32			mskF = 0xFFFFFFFF;	// field mask
	int				bMsb = 31;
	int				bLsb = 0;
	UINT32			rval = 0;			// register value
	UINT32			fval = 0;			// field value
	static UINT32	stat = REG_MOD_REG;				// one of possible cursor position : [REG,FLD,VALUE]

	// to avoid to un-wanted setting for value.
	if ( option & 1 ) stat = REG_MOD_REG;

#define N_STEP	(10)

	switch ( key )
	{
		case PC_KEY_PGUP:
		case PC_KEY_PGDN:
			{
				gDebugModule[siMdl].iReg = siReg;
				gDebugModule[siMdl].iFld = siFld;

				siMdl = ((key==PC_KEY_PGDN)?(siMdl+1):(N_FLD(gDebugModule) + siMdl -1)) % N_FLD(gDebugModule);

				gDbgRegInfo = gDebugModule[siMdl].pMdl;
				snDbgRegVdp = gDebugModule[siMdl].nReg;
				saBaseAddr	= gDebugModule[siMdl].BAdr;// + DTV_SOC_REG;
				siReg		= gDebugModule[siMdl].iReg;
				siFld		= gDebugModule[siMdl].iFld;
			}
		break;

		case 's':
		{
			char	inputStr[20]= "";
			printf("\ninput Step (curr: %08x : %8d):", step, step);
			(void)UartGetString(&inputStr[0], 19, stdin);
			step = strtoul( inputStr, (char **) NULL, 0 );
			printf("\nStep : %08x : %8d", step, step);
		}
		break;

		case '+':
		case '-':
		{
			if ( key == '+' )	step <<=1;
			else				step >>=1;
			printf("\nStep : %08x : %8d", step, step);
		}
		break;
		case PC_KEY_UP:		inc = -1;											break;
		case PC_KEY_DOWN:	inc = +1;											break;
		case PC_KEY_LEFT:	stat = ((stat==REG_MOD_REG)?REG_MOD_VAL:stat<<1);	break;
		case PC_KEY_RIGHT:	stat = ((stat==REG_MOD_VAL)?REG_MOD_REG:stat>>1);	break;
		case '\n':			opMode = 1;											break;
		case '=':			opMode = 2;											break;
		default : if (option == 0 )		return;									break;
	}

	nReg = snDbgRegVdp;
	pReg = (DBG_REG_T*)(&gDbgRegInfo[siReg]);
	pFld = (DBG_REG_FIELD_T*)(&pReg->pFld[siFld]);

	// opMode : current cursor by step && real modify
	//			0 : incremental 현재 값에서 1<<step 만큼씩 변경.
	//			1 : direct 입력 for 현재 field.
	//			2 : direct 입력 for 현재 Register (32bit)
	if ( stat &  REG_MOD_VAL )
	{
		rval = RdGenReg(pReg, pFld);

		if ( pFld )
		{
			bMsb = pFld->bMsb;
			bLsb = pFld->bLsb;
			mskF = (BF_MSK[bMsb+1] & (~BF_MSK[bLsb]));
		}
		// else case done from initial value

		fval = (rval & mskF) >> bLsb;

		if ( (opMode == 0) && inc )
		{
			// modify. by step
			fval = ((fval + (step) * inc) & BF_MSK[bMsb-bLsb+1]);

			rval = ((rval & (~mskF)) | (fval << bLsb));

			(void)WrGenReg(pReg, pFld, rval);
		}
		else if ( (opMode==1)||(opMode==2) )
		{
			// modify by direct value
			char	inputStr[20]= "";
			int		idx=0;

			// echoed '=' deleting \x08 (back space) inserted.
			printf("\n\x08%04x,%2d:%2d> %-20.20s  %-20.20s [%08x] <- ",
					pReg->rAddr,
					pFld?pFld->bMsb:32, pFld?pFld->bLsb:0,
					pReg->rName, pFld?pFld->fName:"...",
					fval);
			(void)UartGetString(&inputStr[0], 19, stdin);
//			  setTerminalMode(~ECHO);	// Turn Off Echo Mode

			for ( idx = 0 ; idx < 20; idx++)
			{
				char c = inputStr[idx];

				if ( c == '\n' ) break;

				if (!( ('0' <= c && c <= '9')
					|| ('a' <= c && c <= 'f')
					|| ('A' <= c && c <= 'F') ))
				{
					if ( inputStr[0] == '0' && inputStr[1] == 'x' )
					{
						continue;
					}
					else
					{
						idx = 0; // truncate invalid input to no input.
						break;
					}
				}
			}

			if (idx)
			{
				UINT32	rval2Wr;

				fval = strtoul( inputStr, (char **) NULL, 0 );
				fval = ((fval ) & BF_MSK[bMsb-bLsb+1]);

				rval2Wr = ((rval & (~mskF)) | (fval << bLsb));

				WrGenReg(pReg,pFld,rval2Wr);
			}
		}
	}
	else if ( stat & REG_MOD_REG )
	{
		if ( opMode == 0 && inc )
		{
			if ( inc > 0 )	siReg = (siReg +	  inc) %nReg;
			else			siReg = (siReg + nReg+inc) %nReg;

			pReg = (DBG_REG_T*)(&gDbgRegInfo[siReg]);

			siFld = 0;
			pFld = (DBG_REG_FIELD_T*)(&pReg->pFld[siFld]);
		}
		else if ( opMode == 1 )
		{
			int found = 0;
			UINT32 rAddr  = ReadHexKeyInput("\nReg Address (LSB 16Bit)");
//			  setTerminalMode(~ECHO);	// Turn Off Echo Mode

			rAddr &= 0xfffffffc; // 4 bytes aligned address.

			if ( rAddr != 0 )
			{
				for ( found = 0; found < nReg ; found ++ )
				{
					if ( gDbgRegInfo[found].rAddr == rAddr) break;
				}

				if ( found != nReg ) { siReg = found; siFld = 0; }
				else				 { printf("\ninvalid Addr: 0x%04x", rAddr); }
			}
		}
		else if ( opMode == 2 )
		{
			char	inputStr[20]= "0x";
			UINT32	rval2Wr;
			int		idx;

			rval = RdGenReg(pReg, pFld);

			printf("\n\x08%04x,31: 0>[%-20.20s] 0x%08x <- 0x", pReg->rAddr, pReg->rName, rval);
			(void)UartGetString(&inputStr[2], 17, stdin);
//			  setTerminalMode(~ECHO);	// Turn Off Echo Mode

			for ( idx = 2 ; idx < 20; idx++)
			{
				char c = inputStr[idx];

				if ( c == '\n' ) break;

				if (!( ('0' <= c && c <= '9')
					|| ('a' <= c && c <= 'f')
					|| ('A' <= c && c <= 'F') ))
				{
					idx = 2; // truncate invaled input to no input.
					break;
				}
			}

			if ( idx > 2 )
			{
				rval2Wr = strtoul( inputStr, (char **) NULL, 16 );
				WrGenReg(pReg,NULL,rval2Wr);
			}
		}
	}
	else if ( stat & REG_MOD_FLD )
	{
		if ( inc && pFld )
		{
			nFld = pReg->nFld;

			if ( inc > 0 )	siFld = (siFld +	  inc) %nFld;
			else			siFld = (siFld + nFld+inc) %nFld;

			pFld = (DBG_REG_FIELD_T*)(&(pReg->pFld[siFld]));
		}
	}

	if ( !inc || !(stat & REG_MOD_VAL ) )
	{
		pReg = (DBG_REG_T*)(&gDbgRegInfo[siReg]);
		pFld = (DBG_REG_FIELD_T*)(&pReg->pFld[siFld]);
		nReg = snDbgRegVdp;
		rval = RdGenReg(pReg, pFld);

		if ( pFld )
		{
			bMsb = pFld->bMsb;
			bLsb = pFld->bLsb;
			mskF = (BF_MSK[bMsb+1] & (~BF_MSK[bLsb]));
		}
		fval = (rval & mskF) >> bLsb;
	}

	printf("\n\x08%04x,%2d:%2d>%c%-20.20s%c%c%-20.20s%c%c%08x%c (%08x)",
			pReg->rAddr,
			pFld?pFld->bMsb:32, pFld?pFld->bLsb:0,
			stat&REG_MOD_REG?'[':' ',
			pReg->rName,
			stat&REG_MOD_REG?']':' ',
			stat&REG_MOD_FLD?'[':' ',
			pFld?pFld->fName:"...",
			stat&REG_MOD_FLD?']':' ',
			stat&REG_MOD_VAL?'[':' ',
			fval,
			stat&REG_MOD_VAL?']':' ',
			rval
			);

	return;
}

/**
 * A/V Register Dump Utility.
 * 시작 주소에서 끝 주소까지 debug용 Serial로 Register 내용을 Dump함.
 *
 * @param	startAddr	Dump 시작 주소. (4byte aligned)
 * @param	sizeStr		Dump size in bytes / size String pointer
 * @param	option		Dump Mode -> TODO
 * @return	N/A
 * @see		FindGenReg PrGenReg
 */
void DumpGenReg(char *startAddr, char *sizeStr, UINT32 option)
{
	int				dumpMode = 0;	// 0 : addr range, 1 : name search.
	char			printed = 0;
	UINT32			endAddr = 0;
	DBG_REG_T		*pReg;

	// null check shall be done below.
	pReg	= FindGenReg(startAddr, NULL);

	if ( !pReg ) return;

	if (startAddr[strlen(startAddr)-1] == '*') dumpMode = 1;

	if ( dumpMode == 0 )
	{
		if ( ((UINT32)sizeStr) < 0x2000 ) endAddr = (UINT32)sizeStr;
		else					endAddr = strtoul2(sizeStr, NULL, 0);

		endAddr += pReg->rAddr;
		for (		  ; pReg->rAddr <= endAddr ; pReg++ )
		{
			if ( !pReg->rAddr ) break;
			printed |= PrGenReg(pReg, option);
		}
	}
	else if ( dumpMode == 1 )
	{
		while ( 1 )
		{
			printed |= PrGenReg(pReg, option);
			pReg = FindGenReg(startAddr, pReg+1);
			if ( !pReg || !pReg->rAddr) break;
		}
	}

	return;
}
/** @} */	// reg_dbg

int debug_RegDebug(void)
{
	UINT32	key = 0;

	ModifyGenReg(key, 1);	// reset start.

	while (1)
	{
		key = UartGetChar(stdin);

		switch (key)
		{
			case PC_KEY_ESC:
			case PC_KEY_F03:
			case PC_KEY_F10:
			case 'q':
			case 'Q':
				printf("\n");
				return RET_OK;

			case '/':
			case '?':
			case 'h':
				printf("\n%s",REG_DBG_HELP_TO_EXIT);
				continue;
		}	/* end switch key */

		ModifyGenReg(key, 0);
	}

	return RET_OK;
}

#endif
#endif

int CLI_DE_RegDebug(int argc ,char **argv)
{
#ifdef USE_DE_REG_DBG
#ifdef INCLUDE_KDRV_DE
	return debug_RegDebug();
#endif
#else
	return RET_OK;
#endif
}

