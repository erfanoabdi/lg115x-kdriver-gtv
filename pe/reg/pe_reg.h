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

/** @file pe_reg.h
 *
 *  driver header for picture enhance register read, write functions. ( used only within kdriver )
 *	
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note		
 *	@date		2011.06.11
 *	@see		
 */

#ifndef	_PE_REG_H_
#define	_PE_REG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "de_pe0_reg_l9.h"
#include "de_pe1_reg_l9.h"
#include "de_pe0_reg_l9b0.h"
#include "de_pe1_reg_l9b0.h"
#include "de_wcp_reg_l9.h"
#include "de_ctr_reg_l9.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define PE_SET_REG_TABLE(_mdl_,_tbl,_ofs)	\
	do{\
		PE_##_mdl_##_DBG_PRINT("write %s.\n",#_tbl);\
		ret = PE_REG_SetTable((_tbl),(_ofs),sizeof(_tbl));\
		PE_##_mdl_##_CHECK_CODE(ret,break,"[%s,%d] PE_REG_SetTable() error.\n",__F__,__L__);\
	}while(0)

/* l9b0 pe0 l */
#define PE_P0L_L9B0_Wr01(_a,_f,_d)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Wr01(_a,_f,_d);DE_P0L_L9B0_WrFL(_a);	}while(0)

#define PE_P0L_L9B0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Wr01(_a,_f1,_d1);DE_P0L_L9B0_Wr01(_a,_f2,_d2);DE_P0L_L9B0_WrFL(_a); }while(0)

#define PE_P0L_L9B0_Wr03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Wr01(_a,_f1,_d1);DE_P0L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0L_L9B0_Wr01(_a,_f3,_d3);DE_P0L_L9B0_WrFL(_a); }while(0)

#define PE_P0L_L9B0_Wr04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Wr01(_a,_f1,_d1);DE_P0L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0L_L9B0_Wr01(_a,_f3,_d3);DE_P0L_L9B0_Wr01(_a,_f4,_d4);DE_P0L_L9B0_WrFL(_a); }while(0)

#define PE_P0L_L9B0_Wr05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Wr01(_a,_f1,_d1);DE_P0L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0L_L9B0_Wr01(_a,_f3,_d3);DE_P0L_L9B0_Wr01(_a,_f4,_d4);DE_P0L_L9B0_Wr01(_a,_f5,_d5);\
		DE_P0L_L9B0_WrFL(_a); }while(0)

#define PE_P0L_L9B0_Wr06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Wr01(_a,_f1,_d1);DE_P0L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0L_L9B0_Wr01(_a,_f3,_d3);DE_P0L_L9B0_Wr01(_a,_f4,_d4);DE_P0L_L9B0_Wr01(_a,_f5,_d5);\
		DE_P0L_L9B0_Wr01(_a,_f6,_d6);DE_P0L_L9B0_WrFL(_a); }while(0)

#define PE_P0L_L9B0_Wr07(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6,_f7,_d7)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Wr01(_a,_f1,_d1);DE_P0L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0L_L9B0_Wr01(_a,_f3,_d3);DE_P0L_L9B0_Wr01(_a,_f4,_d4);DE_P0L_L9B0_Wr01(_a,_f5,_d5);\
		DE_P0L_L9B0_Wr01(_a,_f6,_d6);DE_P0L_L9B0_Wr01(_a,_f7,_d7);DE_P0L_L9B0_WrFL(_a); }while(0)

#define PE_P0L_L9B0_Rd01(_a,_f,_d)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Rd01(_a,_f,_d);	}while(0)

#define PE_P0L_L9B0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Rd01(_a,_f1,_d1);DE_P0L_L9B0_Rd01(_a,_f2,_d2);	}while(0)

#define PE_P0L_L9B0_Rd03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Rd01(_a,_f1,_d1);DE_P0L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0L_L9B0_Rd01(_a,_f3,_d3); }while(0)

#define PE_P0L_L9B0_Rd04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Rd01(_a,_f1,_d1);DE_P0L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0L_L9B0_Rd01(_a,_f3,_d3);DE_P0L_L9B0_Rd01(_a,_f4,_d4); }while(0)

#define PE_P0L_L9B0_Rd05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Rd01(_a,_f1,_d1);DE_P0L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0L_L9B0_Rd01(_a,_f3,_d3);DE_P0L_L9B0_Rd01(_a,_f4,_d4);DE_P0L_L9B0_Rd01(_a,_f5,_d5); }while(0)

#define PE_P0L_L9B0_Rd06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P0L_L9B0_RdFL(_a);DE_P0L_L9B0_Rd01(_a,_f1,_d1);DE_P0L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0L_L9B0_Rd01(_a,_f3,_d3);DE_P0L_L9B0_Rd01(_a,_f4,_d4);DE_P0L_L9B0_Rd01(_a,_f5,_d5);\
		DE_P0L_L9B0_Rd01(_a,_f6,_d6); }while(0)

/* l9b0 pe0 r */
#define PE_P0R_L9B0_Wr01(_a,_f,_d)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Wr01(_a,_f,_d);DE_P0R_L9B0_WrFL(_a);	}while(0)

#define PE_P0R_L9B0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Wr01(_a,_f1,_d1);DE_P0R_L9B0_Wr01(_a,_f2,_d2);DE_P0R_L9B0_WrFL(_a); }while(0)

#define PE_P0R_L9B0_Wr03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Wr01(_a,_f1,_d1);DE_P0R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0R_L9B0_Wr01(_a,_f3,_d3);DE_P0R_L9B0_WrFL(_a); }while(0)

#define PE_P0R_L9B0_Wr04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Wr01(_a,_f1,_d1);DE_P0R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0R_L9B0_Wr01(_a,_f3,_d3);DE_P0R_L9B0_Wr01(_a,_f4,_d4);DE_P0R_L9B0_WrFL(_a); }while(0)

#define PE_P0R_L9B0_Wr05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Wr01(_a,_f1,_d1);DE_P0R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0R_L9B0_Wr01(_a,_f3,_d3);DE_P0R_L9B0_Wr01(_a,_f4,_d4);DE_P0R_L9B0_Wr01(_a,_f5,_d5);\
		DE_P0R_L9B0_WrFL(_a); }while(0)

#define PE_P0R_L9B0_Wr06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Wr01(_a,_f1,_d1);DE_P0R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0R_L9B0_Wr01(_a,_f3,_d3);DE_P0R_L9B0_Wr01(_a,_f4,_d4);DE_P0R_L9B0_Wr01(_a,_f5,_d5);\
		DE_P0R_L9B0_Wr01(_a,_f6,_d6);DE_P0R_L9B0_WrFL(_a); }while(0)

#define PE_P0R_L9B0_Wr07(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6,_f7,_d7)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Wr01(_a,_f1,_d1);DE_P0R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P0R_L9B0_Wr01(_a,_f3,_d3);DE_P0R_L9B0_Wr01(_a,_f4,_d4);DE_P0R_L9B0_Wr01(_a,_f5,_d5);\
		DE_P0R_L9B0_Wr01(_a,_f6,_d6);DE_P0R_L9B0_Wr01(_a,_f7,_d7);DE_P0R_L9B0_WrFL(_a); }while(0)

#define PE_P0R_L9B0_Rd01(_a,_f,_d)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Rd01(_a,_f,_d);	}while(0)

#define PE_P0R_L9B0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Rd01(_a,_f1,_d1);DE_P0R_L9B0_Rd01(_a,_f2,_d2);	}while(0)

#define PE_P0R_L9B0_Rd03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Rd01(_a,_f1,_d1);DE_P0R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0R_L9B0_Rd01(_a,_f3,_d3); }while(0)

#define PE_P0R_L9B0_Rd04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Rd01(_a,_f1,_d1);DE_P0R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0R_L9B0_Rd01(_a,_f3,_d3);DE_P0R_L9B0_Rd01(_a,_f4,_d4); }while(0)

#define PE_P0R_L9B0_Rd05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Rd01(_a,_f1,_d1);DE_P0R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0R_L9B0_Rd01(_a,_f3,_d3);DE_P0R_L9B0_Rd01(_a,_f4,_d4);DE_P0R_L9B0_Rd01(_a,_f5,_d5); }while(0)

#define PE_P0R_L9B0_Rd06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P0R_L9B0_RdFL(_a);DE_P0R_L9B0_Rd01(_a,_f1,_d1);DE_P0R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P0R_L9B0_Rd01(_a,_f3,_d3);DE_P0R_L9B0_Rd01(_a,_f4,_d4);DE_P0R_L9B0_Rd01(_a,_f5,_d5);\
		DE_P0R_L9B0_Rd01(_a,_f6,_d6); }while(0)

/* l9b0 pe1 l */
#define PE_P1L_L9B0_Wr01(_a,_f,_d)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Wr01(_a,_f,_d);DE_P1L_L9B0_WrFL(_a);	}while(0)

#define PE_P1L_L9B0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Wr01(_a,_f1,_d1);DE_P1L_L9B0_Wr01(_a,_f2,_d2);DE_P1L_L9B0_WrFL(_a); }while(0)

#define PE_P1L_L9B0_Wr03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Wr01(_a,_f1,_d1);DE_P1L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1L_L9B0_Wr01(_a,_f3,_d3);DE_P1L_L9B0_WrFL(_a); }while(0)

#define PE_P1L_L9B0_Wr04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Wr01(_a,_f1,_d1);DE_P1L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1L_L9B0_Wr01(_a,_f3,_d3);DE_P1L_L9B0_Wr01(_a,_f4,_d4);DE_P1L_L9B0_WrFL(_a); }while(0)

#define PE_P1L_L9B0_Wr05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Wr01(_a,_f1,_d1);DE_P1L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1L_L9B0_Wr01(_a,_f3,_d3);DE_P1L_L9B0_Wr01(_a,_f4,_d4);DE_P1L_L9B0_Wr01(_a,_f5,_d5);\
		DE_P1L_L9B0_WrFL(_a); }while(0)

#define PE_P1L_L9B0_Wr06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Wr01(_a,_f1,_d1);DE_P1L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1L_L9B0_Wr01(_a,_f3,_d3);DE_P1L_L9B0_Wr01(_a,_f4,_d4);DE_P1L_L9B0_Wr01(_a,_f5,_d5);\
		DE_P1L_L9B0_Wr01(_a,_f6,_d6);DE_P1L_L9B0_WrFL(_a); }while(0)

#define PE_P1L_L9B0_Wr07(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6,_f7,_d7)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Wr01(_a,_f1,_d1);DE_P1L_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1L_L9B0_Wr01(_a,_f3,_d3);DE_P1L_L9B0_Wr01(_a,_f4,_d4);DE_P1L_L9B0_Wr01(_a,_f5,_d5);\
		DE_P1L_L9B0_Wr01(_a,_f6,_d6);DE_P1L_L9B0_Wr01(_a,_f7,_d7);DE_P1L_L9B0_WrFL(_a); }while(0)

#define PE_P1L_L9B0_Rd01(_a,_f,_d)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Rd01(_a,_f,_d);	}while(0)

#define PE_P1L_L9B0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Rd01(_a,_f1,_d1);DE_P1L_L9B0_Rd01(_a,_f2,_d2);	}while(0)

#define PE_P1L_L9B0_Rd03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Rd01(_a,_f1,_d1);DE_P1L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1L_L9B0_Rd01(_a,_f3,_d3); }while(0)

#define PE_P1L_L9B0_Rd04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Rd01(_a,_f1,_d1);DE_P1L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1L_L9B0_Rd01(_a,_f3,_d3);DE_P1L_L9B0_Rd01(_a,_f4,_d4); }while(0)

#define PE_P1L_L9B0_Rd05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Rd01(_a,_f1,_d1);DE_P1L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1L_L9B0_Rd01(_a,_f3,_d3);DE_P1L_L9B0_Rd01(_a,_f4,_d4);DE_P1L_L9B0_Rd01(_a,_f5,_d5); }while(0)

#define PE_P1L_L9B0_Rd06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P1L_L9B0_RdFL(_a);DE_P1L_L9B0_Rd01(_a,_f1,_d1);DE_P1L_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1L_L9B0_Rd01(_a,_f3,_d3);DE_P1L_L9B0_Rd01(_a,_f4,_d4);DE_P1L_L9B0_Rd01(_a,_f5,_d5);\
		DE_P1L_L9B0_Rd01(_a,_f6,_d6); }while(0)

/* l9b0 pe1 r */
#define PE_P1R_L9B0_Wr01(_a,_f,_d)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Wr01(_a,_f,_d);DE_P1R_L9B0_WrFL(_a);	}while(0)

#define PE_P1R_L9B0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Wr01(_a,_f1,_d1);DE_P1R_L9B0_Wr01(_a,_f2,_d2);DE_P1R_L9B0_WrFL(_a); }while(0)

#define PE_P1R_L9B0_Wr03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Wr01(_a,_f1,_d1);DE_P1R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1R_L9B0_Wr01(_a,_f3,_d3);DE_P1R_L9B0_WrFL(_a); }while(0)

#define PE_P1R_L9B0_Wr04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Wr01(_a,_f1,_d1);DE_P1R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1R_L9B0_Wr01(_a,_f3,_d3);DE_P1R_L9B0_Wr01(_a,_f4,_d4);DE_P1R_L9B0_WrFL(_a); }while(0)

#define PE_P1R_L9B0_Wr05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Wr01(_a,_f1,_d1);DE_P1R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1R_L9B0_Wr01(_a,_f3,_d3);DE_P1R_L9B0_Wr01(_a,_f4,_d4);DE_P1R_L9B0_Wr01(_a,_f5,_d5);\
		DE_P1R_L9B0_WrFL(_a); }while(0)

#define PE_P1R_L9B0_Wr06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Wr01(_a,_f1,_d1);DE_P1R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1R_L9B0_Wr01(_a,_f3,_d3);DE_P1R_L9B0_Wr01(_a,_f4,_d4);DE_P1R_L9B0_Wr01(_a,_f5,_d5);\
		DE_P1R_L9B0_Wr01(_a,_f6,_d6);DE_P1R_L9B0_WrFL(_a); }while(0)

#define PE_P1R_L9B0_Wr07(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6,_f7,_d7)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Wr01(_a,_f1,_d1);DE_P1R_L9B0_Wr01(_a,_f2,_d2);\
		DE_P1R_L9B0_Wr01(_a,_f3,_d3);DE_P1R_L9B0_Wr01(_a,_f4,_d4);DE_P1R_L9B0_Wr01(_a,_f5,_d5);\
		DE_P1R_L9B0_Wr01(_a,_f6,_d6);DE_P1R_L9B0_Wr01(_a,_f7,_d7);DE_P1R_L9B0_WrFL(_a); }while(0)

#define PE_P1R_L9B0_Rd01(_a,_f,_d)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Rd01(_a,_f,_d);	}while(0)

#define PE_P1R_L9B0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Rd01(_a,_f1,_d1);DE_P1R_L9B0_Rd01(_a,_f2,_d2);	}while(0)

#define PE_P1R_L9B0_Rd03(_a,_f1,_d1,_f2,_d2,_f3,_d3)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Rd01(_a,_f1,_d1);DE_P1R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1R_L9B0_Rd01(_a,_f3,_d3); }while(0)

#define PE_P1R_L9B0_Rd04(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Rd01(_a,_f1,_d1);DE_P1R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1R_L9B0_Rd01(_a,_f3,_d3);DE_P1R_L9B0_Rd01(_a,_f4,_d4); }while(0)

#define PE_P1R_L9B0_Rd05(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Rd01(_a,_f1,_d1);DE_P1R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1R_L9B0_Rd01(_a,_f3,_d3);DE_P1R_L9B0_Rd01(_a,_f4,_d4);DE_P1R_L9B0_Rd01(_a,_f5,_d5); }while(0)

#define PE_P1R_L9B0_Rd06(_a,_f1,_d1,_f2,_d2,_f3,_d3,_f4,_d4,_f5,_d5,_f6,_d6)	\
	do{	DE_P1R_L9B0_RdFL(_a);DE_P1R_L9B0_Rd01(_a,_f1,_d1);DE_P1R_L9B0_Rd01(_a,_f2,_d2);\
		DE_P1R_L9B0_Rd01(_a,_f3,_d3);DE_P1R_L9B0_Rd01(_a,_f4,_d4);DE_P1R_L9B0_Rd01(_a,_f5,_d5);\
		DE_P1R_L9B0_Rd01(_a,_f6,_d6); }while(0)

/* l9a0 pe0 l */
#define PE_P0L_L9A0_Wr01(_a,_f,_d)	\
	do{	DE_P0L_L9_RdFL(_a);DE_P0L_L9_Wr01(_a,_f,_d);DE_P0L_L9_WrFL(_a);	}while(0)

#define PE_P0L_L9A0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0L_L9_RdFL(_a);DE_P0L_L9_Wr01(_a,_f1,_d1);DE_P0L_L9_Wr01(_a,_f2,_d2);DE_P0L_L9_WrFL(_a); }while(0)

#define PE_P0L_L9A0_Rd01(_a,_f,_d)	\
	do{	DE_P0L_L9_RdFL(_a);DE_P0L_L9_Rd01(_a,_f,_d);	}while(0)

#define PE_P0L_L9A0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0L_L9_RdFL(_a);DE_P0L_L9_Rd01(_a,_f1,_d1);DE_P0L_L9_Rd01(_a,_f2,_d2);	}while(0)

/* l9a0 pe0 r */
#define PE_P0R_L9A0_Wr01(_a,_f,_d)	\
	do{	DE_P0R_L9_RdFL(_a);DE_P0R_L9_Wr01(_a,_f,_d);DE_P0R_L9_WrFL(_a);	}while(0)

#define PE_P0R_L9A0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0R_L9_RdFL(_a);DE_P0R_L9_Wr01(_a,_f1,_d1);DE_P0R_L9_Wr01(_a,_f2,_d2);DE_P0R_L9_WrFL(_a); }while(0)

#define PE_P0R_L9A0_Rd01(_a,_f,_d)	\
	do{	DE_P0R_L9_RdFL(_a);DE_P0R_L9_Rd01(_a,_f,_d);	}while(0)

#define PE_P0R_L9A0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P0R_L9_RdFL(_a);DE_P0R_L9_Rd01(_a,_f1,_d1);DE_P0R_L9_Rd01(_a,_f2,_d2);	}while(0)

/* l9a0 pe1 l */
#define PE_P1L_L9A0_Wr01(_a,_f,_d)	\
	do{	DE_P1L_L9_RdFL(_a);DE_P1L_L9_Wr01(_a,_f,_d);DE_P1L_L9_WrFL(_a);	}while(0)

#define PE_P1L_L9A0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1L_L9_RdFL(_a);DE_P1L_L9_Wr01(_a,_f1,_d1);DE_P1L_L9_Wr01(_a,_f2,_d2);DE_P1L_L9_WrFL(_a); }while(0)

#define PE_P1L_L9A0_Rd01(_a,_f,_d)	\
	do{	DE_P1L_L9_RdFL(_a);DE_P1L_L9_Rd01(_a,_f,_d);	}while(0)

#define PE_P1L_L9A0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1L_L9_RdFL(_a);DE_P1L_L9_Rd01(_a,_f1,_d1);DE_P1L_L9_Rd01(_a,_f2,_d2);	}while(0)

/* l9a0 pe1 r */
#define PE_P1R_L9A0_Wr01(_a,_f,_d)	\
	do{	DE_P1R_L9_RdFL(_a);DE_P1R_L9_Wr01(_a,_f,_d);DE_P1R_L9_WrFL(_a);	}while(0)

#define PE_P1R_L9A0_Wr02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1R_L9_RdFL(_a);DE_P1R_L9_Wr01(_a,_f1,_d1);DE_P1R_L9_Wr01(_a,_f2,_d2);DE_P1R_L9_WrFL(_a); }while(0)

#define PE_P1R_L9A0_Rd01(_a,_f,_d)	\
	do{	DE_P1R_L9_RdFL(_a);DE_P1R_L9_Rd01(_a,_f,_d);	}while(0)

#define PE_P1R_L9A0_Rd02(_a,_f1,_d1,_f2,_d2)	\
	do{	DE_P1R_L9_RdFL(_a);DE_P1R_L9_Rd01(_a,_f1,_d1);DE_P1R_L9_Rd01(_a,_f2,_d2);	}while(0)

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
 *	pe register data setting type
 */
typedef struct {
	UINT32 addr;
	UINT32 data;
}
PE_REG_PARAM_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int PE_REG_SetTable(const PE_REG_PARAM_T *table, UINT32 offset, UINT32 size);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PE_REG_H_ */
