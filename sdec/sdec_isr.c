/* ****************************************************************************************
 * DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * COPYRIGHT(c) 1998-2010 by LG Electronics Inc.
 *
 * All rights reserved. No part of this work covered by this copyright hereon
 * may be reproduced, stored in a retrieval system, in any form
 * or by any means, electronic, mechanical, photocopying, recording
 * or otherwise, without the prior written  permission of LG Electronics.
 * *****************************************************************************************/

/** @file
 *
 * main driver implementation for de device.
 * de device will teach you how to make device driver with new platform.
 *
 * author     jasonlee.lee (jasonlee.lee@lge.com)
 * version    0.1
 * date       2010.03.11
 * note       Additional information.
 *
 * @addtogroup lg1150_sdec
 * @{
 */

/*----------------------------------------------------------------------------------------
 *   Control Constants
 *---------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
 *   File Inclusions
 *---------------------------------------------------------------------------------------*/
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/uaccess.h>


#include <mach/platform.h>

#include "os_util.h"


#include "sdec_kapi.h"
#include "sdec_reg.h"
#include "sdec_drv.h"
#include "sdec_io.h"
#include "sdec_isr.h"
#include "sdec_pes.h"
#include "sdec_hal.h"

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

/*----------------------------------------------------------------------------------------
 *   Static Variables
 *---------------------------------------------------------------------------------------*/

/**
********************************************************************************
* @brief
*   initialize Mutex.
* @remarks
*  DETAIL INFORMATION
* @par requirements:
*
* @param
*   ui32Arg :arguments from userspace
*   stpSdecParam :SDEC parameter
* @return
*  DTV_STATUS_T
********************************************************************************
*/

BOOLEAN SDEC_Section_Proc(S_SDEC_PARAM_T *stpSdecParam, UINT8 ui8Ch, UINT8 ui8IsHigh)
{
	UINT32 ui32GpbDataIdx = 0x0;
	UINT8 ui8PidFltIdx = 0x0;
	UINT8 ui8SecFltIdx = 0x0;
	UINT8 ui8Count = 0x0;
	UINT32 ui32GpbWrPtr = 0x0;
	UINT32 ui32GpbRdPtr = 0x0;
	UINT32 ui32UsrRdPtr = 0x0;
	UINT32 ui32GpbBase = 0x0;
	UINT32 ui32GpbEnd = 0x0;
	BOOLEAN isFound = FALSE;
	unsigned long flags = 0;

	LX_SDEC_NOTIFY_PARAM_T notiParam;

	//ui32GpbDataIdx = stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_d_istat[ui8IsHigh].gpb_d_istat;
	ui32GpbDataIdx = SDEC_HAL_GPBGetDataIntrStat(ui8Ch, ui8IsHigh);

	if(ui32GpbDataIdx)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "GPB_DATA_CH%c_GPB%c:[0x%08x]",(ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L', ui32GpbDataIdx);

		/* 2011.03.18 - gaius.lee
		 * 32 bit를 모두 도는 것보다 필요한 bit만 돌고, 없으면 바로 끝나도록 수정.
		 */
		for (ui8Count = 0; ui8Count < 32 && ui32GpbDataIdx; ui8Count++)
		{
			if(ui8IsHigh)	ui8SecFltIdx = ui8Count + 32;
			else			ui8SecFltIdx = ui8Count;

			if ( check_one_bit(ui32GpbDataIdx, ui8Count) && (stpSdecParam->stSecMap[ui8Ch][ui8SecFltIdx].flag == TRUE))
			{
				SDEC_GPB_LOCK(stpSdecParam, ui8Ch, ui8SecFltIdx);
				//ui32GpbWrPtr	= stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_w_ptr[ui8SecFltIdx].gpb_w_ptr;
				//ui32GpbRdPtr	= stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_r_ptr[ui8SecFltIdx].gpb_r_ptr;
				ui32GpbWrPtr	= SDEC_HAL_GPBGetWritePtr(ui8Ch, ui8SecFltIdx);
				/* use s/w read pointer to implement h/w overflow */
				ui32GpbRdPtr	= stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui32Readptr;
				//ui32GpbRdPtr	= SDEC_HAL_GPBGetReadPtr(ui8Ch, ui8SecFltIdx);
				/* get user readpointer from h/w */
				//ui32UsrRdPtr	= SDEC_HAL_GPBGetReadPtr(ui8Ch, ui8SecFltIdx);
				ui32UsrRdPtr	= stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui32UsrReadptr;
				ui32GpbBase		= stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui32Baseptr;
				ui32GpbEnd		= stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui32Endptr;

				if( !ui32GpbBase || !ui32GpbEnd || !ui32GpbWrPtr || !ui32GpbRdPtr )
				{
					SDEC_DEBUG_Print("in ISR, address is not valid : B[%08x] E[%08x] W[%08x] R[%08x]", ui32GpbBase, ui32GpbEnd, ui32GpbWrPtr, ui32GpbRdPtr);
					SDEC_GPB_UNLOCK(stpSdecParam, ui8Ch, ui8SecFltIdx);
					continue;
				}
				// 20110331 kibeom.kim : add the mask to apply the gpb_base_addr (logical address in L9) value
				// ui32GpbRdPtr & ui32GpbWrPtr is restricted to 28 bit.
				ui32GpbWrPtr	|= (ui32GpbBase & GPB_BASE_ADDR_MASK);
				ui32GpbRdPtr 	|= (ui32GpbBase & GPB_BASE_ADDR_MASK);
				ui32UsrRdPtr 	|= (ui32GpbBase & GPB_BASE_ADDR_MASK);

				/* in some case write pointer jump to wrong place, so resetting write pointer as base address */
				if( ui32GpbWrPtr < ui32GpbBase || ui32GpbEnd < ui32GpbWrPtr )
				{
					SDEC_DTV_SOC_Message( SDEC_ERROR, "CH%c GPB_Data_%c GPB BND ERROR Idx[%d] b[%08x] w[%08x] e[%08x]",
										(ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L', ui8SecFltIdx
										,ui32GpbBase, ui32GpbWrPtr, ui32GpbEnd );

					/* disable section filter */
					SDEC_HAL_SECFClearEnableBit(ui8Ch, ui8SecFltIdx);
					SDEC_HAL_SECFClearBufValidBit(ui8Ch, ui8SecFltIdx);

					/* resetting write pointer and bound register */
					SDEC_HAL_GPBSetWritePtr(ui8Ch, ui8SecFltIdx, ui32GpbRdPtr);
					SDEC_HAL_GPBSetBnd(ui8Ch, ui8SecFltIdx, stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.l_bnd, stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.u_bnd);

					/* enable section filter */
					SDEC_HAL_SECFSetBufValidBit(ui8Ch, ui8SecFltIdx);
					SDEC_HAL_SECFSetEnableBit(ui8Ch, ui8SecFltIdx);

					/* continue to next */
					SDEC_GPB_UNLOCK(stpSdecParam, ui8Ch, ui8SecFltIdx);
					continue;
				}
				stpSdecParam->stSecMap[ui8Ch][ui8SecFltIdx].stStatusInfo.f.SDEC_FLTSTATE_DATAREADY = 0x1;

				SDEC_DTV_SOC_Message( SDEC_ISR, "CH%c GPB_Data_%c GPB Idx[%d] rp[%d] wp[%d]",
					(ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L', ui8SecFltIdx
					,stpSdecParam->ui8GpbInfoRIdx, stpSdecParam->ui8GpbInfoWIdx
					);

				notiParam.channel	= ui8Ch;
				notiParam.index 	= ui8SecFltIdx;
				notiParam.readPtr	= ui32GpbRdPtr;
				notiParam.writePtr	= ui32GpbWrPtr;
				notiParam.status	= LX_SDEC_FLTSTATE_DATAREADY;
				notiParam.msec		= OS_GetMsecTicks();

			#if ( LX_SDEC_USE_GPB_OW == 1 )
				/* check gpb overflow */
				if( ui32GpbRdPtr < ui32GpbWrPtr )
				{
					/* case 1.
					 *          ui32GpbRdPtr   ui32GpbWrPtr
					 *              V               V
					 * |---------------------------------------------------------|
					 *                  ^
					 *	           ui32UsrRdPtr
					 */
					if( ui32GpbRdPtr < ui32UsrRdPtr &&
						ui32UsrRdPtr < ui32GpbWrPtr )
					{
						SDEC_DTV_SOC_Message( SDEC_ERROR, "1 CH%c GPB_Data_%c GPB Idx[%d] rp[%08x] wp[%08x] OVERFLOW",
											(ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L', ui8SecFltIdx
											,ui32GpbRdPtr, ui32GpbWrPtr
											);
						/* check previous queue */
						SDEC_CheckNotifyOvf(stpSdecParam, &notiParam);
					}

				}
				else
				{
					/* case 2.
					 *      ui32GpbWrPtr                             ui32GpbRdPtr
					 *          V                                          V
					 * |---------------------------------------------------------|
					 *                                                         ^
					 *	                                                 ui32UsrRdPtr
					 *
					 *     ui32GpbWrPtr                              ui32GpbRdPtr
					 *          V                                          V
					 * |---------------------------------------------------------|
					 *      ^
					 *  ui32UsrRdPtr
					 */
					if( ui32GpbRdPtr < ui32UsrRdPtr ||
						ui32UsrRdPtr < ui32GpbWrPtr )
					{
						SDEC_DTV_SOC_Message( SDEC_ERROR, "2 CH%c GPB_Data_%c GPB Idx[%d] rp[%08x] wp[%08x] OVERFLOW",
											(ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L', ui8SecFltIdx
											,ui32GpbRdPtr, ui32GpbWrPtr
											);
						/* check previous queue */
						SDEC_CheckNotifyOvf(stpSdecParam, &notiParam);
					}

				}

			#endif
				isFound = SDEC_SetNotifyParam( stpSdecParam, notiParam);

				SDEC_DTV_SOC_Message( SDEC_ISR, "[%d] rp[0x%08x] wp[0x%08x] b[0x%08x]] e[0x%08x]" //bnd[0x%04x%04x]"
														, ui8SecFltIdx
														, ui32GpbRdPtr
														, ui32GpbWrPtr
														//, stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_bnd[ui8SecFltIdx].l_bnd
														//, stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_bnd[ui8SecFltIdx].u_bnd
														//, SDEC_HAL_GPBGetLowerBnd(ui8Ch, ui8SecFltIdx)
														//, SDEC_HAL_GPBGetUpperBnd(ui8Ch, ui8SecFltIdx)
														,ui32GpbBase
														,ui32GpbEnd
														);

				/* if write pointer arrive end, turn around to base */
				if( ui32GpbWrPtr == ui32GpbEnd )	ui32GpbWrPtr = ui32GpbBase;

				/* next read pointer update */
				//stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_r_ptr[ui8SecFltIdx].gpb_r_ptr = ui32GpbWrPtr;
				/* use s/w read pointer to implement h/w overflow */
				stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui32Readptr = ui32GpbWrPtr;
				//SDEC_HAL_GPBSetReadPtr(ui8Ch, ui8SecFltIdx, ui32GpbWrPtr);

				/* gaius.lee 2011.05.13
				 * gpb_bnd register error sometimes happens.
				 * When gpb_w_ptr is close to upper bound, value of gpb_bnd register is changed.
				 * below is to prevent this.
				 */

//				if(stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.l_bnd != stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_bnd[ui8SecFltIdx].l_bnd
//				|| stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.u_bnd != stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_bnd[ui8SecFltIdx].u_bnd
//				)
#if 0
				if(stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.l_bnd != SDEC_HAL_GPBGetLowerBnd(ui8Ch, ui8SecFltIdx)
				|| stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.u_bnd != SDEC_HAL_GPBGetUpperBnd(ui8Ch, ui8SecFltIdx)
					)
				{
/*					SDEC_DTV_SOC_Message( NORMAL5, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  bnd[0x%04x%04x]"
						, stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_bnd[ui8SecFltIdx].l_bnd
						, stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_bnd[ui8SecFltIdx].u_bnd
						);
						*/
					/* if gpb_bnd is changed by outside cause, resetting original value */
					//stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_bnd[ui8SecFltIdx]= stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd;
					SDEC_HAL_GPBSetBnd(ui8Ch, ui8SecFltIdx, stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.l_bnd, stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].stGpbBnd.u_bnd);
				}
#endif
				/* if oneshot mode is true */
				if (check_one_bit(stpSdecParam->stSecMap[ui8Ch][ui8SecFltIdx].mode, 1))
				{
					SDEC_DTV_SOC_Message( SDEC_ISR, "CH%c GPB_Data_%c GPB Idx[%d] one shot  mode", (ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L', ui8SecFltIdx);

					/* Get PID Filter index */
					ui8PidFltIdx = stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui8PidFltIdx;

					/* Clear secion filter map register */
					//clear__bit(stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->secf_map[ui8PidFltIdx * 2 + ui8IsHigh].secf_map, (ui8SecFltIdx % 32));
					SDEC_HAL_SECFClearMapBit(ui8Ch, ui8PidFltIdx, ui8SecFltIdx);

					/* disable pid filter if there is no section filter which is linked this pidfilter */
					//if(stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->secf_map[ui8PidFltIdx * 2 + ui8IsHigh].secf_map == 0x0)
					if(SDEC_HAL_SECFGetLinkedMap(ui8Ch, ui8PidFltIdx) == 0)
					{
						SDEC_DTV_SOC_Message( SDEC_ISR, "CH%c GPB_Data_%c GPB Idx[%d] Disable pid filter %d", (ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L', ui8SecFltIdx, ui8PidFltIdx);

						//eRet = SDEC_DisablePIDF(stpSdecParam, ui8PidFltIdx, ui8SecFltIdx, ui8Ch);
#ifdef NOT_USE_PIDF_SPINLOCK
						SDEC_HAL_PIDFEnable(ui8Ch, ui8PidFltIdx, SDEC_HAL_DISABLE);
#else
						SDEC_SetPidf_Enable(stpSdecParam, ui8Ch, ui8PidFltIdx, SDEC_HAL_DISABLE);
#endif
					}
				}

				SDEC_GPB_UNLOCK(stpSdecParam, ui8Ch, ui8SecFltIdx);

				/* clear bit for reduce useless iteration */
				clear__bit(ui32GpbDataIdx, ui8Count);
			}
		}
	}

	return isFound;
}

BOOLEAN SDEC_Full_Proc(S_SDEC_PARAM_T *stpSdecParam, UINT8 ui8Ch, UINT8 ui8IsHigh)
{
	UINT32 ui32GpbDataIdx = 0x0;
	UINT8 ui8SecFltIdx = 0x0;
	UINT8 ui8Count = 0x0;
	UINT32 ui32GpbWrPtr = 0x0;
	UINT32 ui32GpbRdPtr = 0x0;
	UINT32 ui32GpbBase = 0x0;
	BOOLEAN isFound = FALSE;

	LX_SDEC_NOTIFY_PARAM_T notiParam;

	//ui32GpbDataIdx = stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_f_istat[ui8IsHigh].gpb_f_istat;
	ui32GpbDataIdx = SDEC_HAL_GPBGetFullIntrStat(ui8Ch, ui8IsHigh);

	if(ui32GpbDataIdx)
	{
		/* 2011.03.18 - gaius.lee
		 * 32 bit를 모두 도는 것보다 필요한 bit만 돌고, 없으면 바로 끝나도록 수정.
		 */
		for (ui8Count = 0; ui8Count < 32 && ui32GpbDataIdx; ui8Count++)
		{
			if(ui8IsHigh)	ui8SecFltIdx = ui8Count + 32;
			else			ui8SecFltIdx = ui8Count;

			if ( check_one_bit(ui32GpbDataIdx, ui8Count) )
			{
				//ui32GpbWrPtr = stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_w_ptr[ui8SecFltIdx].gpb_w_ptr;
				//ui32GpbRdPtr = stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->gpb_r_ptr[ui8SecFltIdx].gpb_r_ptr;
				ui32GpbWrPtr	= SDEC_HAL_GPBGetWritePtr(ui8Ch, ui8SecFltIdx);
				/* use s/w read pointer to implement h/w overflow */
				ui32GpbRdPtr	= stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui32Readptr;
				//ui32GpbRdPtr	= SDEC_HAL_GPBGetReadPtr(ui8Ch, ui8SecFltIdx);
				ui32GpbBase		= stpSdecParam->stSdecMeminfo[ui8Ch][ui8SecFltIdx].ui32Baseptr;

				// 20110331 kibeom.kim : add the mask to apply the gpb_base_addr (logical address in L9) value
				// ui32GpbRdPtr & ui32GpbWrPtr is restricted to 28 bit.
				ui32GpbWrPtr	|= (ui32GpbBase & GPB_BASE_ADDR_MASK);
				ui32GpbRdPtr 	|= (ui32GpbBase & GPB_BASE_ADDR_MASK);

				SDEC_DTV_SOC_Message( SDEC_ERROR, "GPB_FULL_CH%c_GP%c gpb[%d], wptr: [0x%08x], rptr: [0x%08x]", (ui8Ch)?'B':'A', (ui8IsHigh)?'H':'L',
										ui8SecFltIdx, ui32GpbWrPtr, ui32GpbRdPtr);

				/*
				 * overflow check
				 * see above in SDEC_Section_Proc()
				 */
				/* stored buf info	*/
				notiParam.channel	= ui8Ch;
				notiParam.index 	= ui8SecFltIdx;
				notiParam.readPtr	= ui32GpbRdPtr;
				notiParam.writePtr	= ui32GpbWrPtr;
				notiParam.status	= LX_SDEC_FLTSTATE_OVERFLOW;
				notiParam.msec		= OS_GetMsecTicks();

			#if ( LX_SDEC_USE_GPB_OW == 1 )
				/* check previous queue */
				SDEC_CheckNotifyOvf(stpSdecParam, &notiParam);
			#endif

				isFound = SDEC_SetNotifyParam( stpSdecParam, notiParam);
			}
		}

		/* clear bit for reduce useless iteration */
		clear__bit(ui32GpbDataIdx, ui8Count);
	}

	return isFound;
}

void SDEC_ISR_Handler(S_SDEC_PARAM_T *stpSdecParam)
{
	DTV_STATUS_T eRet = OK;
//	ERR_INTR_STAT stErrIntrStat;
	SE_ISTAT	stSeIntrStat;

	UINT32 ui32IntrStat = 0x0;
	UINT8 ui8MainStc = 0x0;
	BOOLEAN isFound = FALSE;

	//intr clear
	//ui32IntrStat = stpSdecParam->stSDEC_IO_Reg->intr_stat;
	//stpSdecParam->stSDEC_IO_Reg->intr_stat = ui32IntrStat;
	ui32IntrStat = SDEC_HAL_IntrStatReadAndClear();

	//PCR
	if((ui32IntrStat & PCR) == PCR)
	{
		//SDEC_DTV_SOC_Message( SDEC_ISR, "PCR:[0x%08x]", ui32IntrStat);

		//ui8MainStc = stpSdecParam->stSDEC_IO_Reg->stcc_asg.main;
		ui8MainStc = SDEC_HAL_STCCGetMain();

		stpSdecParam->stPCR_STC.ui8Channel		= ui8MainStc;
		//stpSdecParam->stPCR_STC.STC_hi_value 	= stpSdecParam->stSDEC_IO_Reg->stcc[ui8MainStc*4 +1];
		//stpSdecParam->stPCR_STC.STC_lo_value 	= extract_bits(stpSdecParam->stSDEC_IO_Reg->stcc[ui8MainStc*4 +3], 0x3FF, 16);
		stpSdecParam->stPCR_STC.STC_hi_value 	= SDEC_HAL_STCCGetSTC(ui8MainStc, 1);
		stpSdecParam->stPCR_STC.STC_lo_value 	= SDEC_HAL_STCCGetSTC(ui8MainStc, 0);

		//stpSdecParam->stPCR_STC.PCR_hi_value = stpSdecParam->stSDEC_IO_Reg->stcc[ui8MainStc*4 +2];
		//stpSdecParam->stPCR_STC.PCR_lo_value = extract_bits(stpSdecParam->stSDEC_IO_Reg->stcc[ui8MainStc*4 +3], 0x3FF, 0);
		stpSdecParam->stPCR_STC.PCR_hi_value = SDEC_HAL_STCCGetPCR(ui8MainStc, 1);
		stpSdecParam->stPCR_STC.PCR_lo_value = SDEC_HAL_STCCGetPCR(ui8MainStc, 0);

		//set__bit(stpSdecParam->stSDEC_IO_Reg->stcc[ui8MainStc*4], 0);//latch
//		SDEC_HAL_STCCEnableLatch(ui8MainStc, SDEC_HAL_ENABLE);
		SDEC_HAL_STCCEnableLatch(0, SDEC_HAL_ENABLE);
		SDEC_HAL_STCCEnableLatch(1, SDEC_HAL_ENABLE);
		//set__bit(stpSdecParam->stSDEC_IO_Reg->stcc[ui8MainStc*4], 1);//copy
		//SDEC_HAL_STCCEnableCopy(ui8MainStc);

		queue_work(stpSdecParam->workqueue, &stpSdecParam->PcrRecovery);
	}

	//TB_Dcount
	if((ui32IntrStat & TB_DCOUNT) == TB_DCOUNT)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "TB_DCOUNT:[0x%08x]", ui32IntrStat);
	}

	//BDRC_0
	if((ui32IntrStat & BDRC_0) == BDRC_0)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "BDRC_0:[0x%08x]", ui32IntrStat);
	}

	//BDRC_1
	if((ui32IntrStat & BDRC_1) == BDRC_1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "BDRC_1:[0x%08x]", ui32IntrStat);
	}

	//BDRC_2
	if((ui32IntrStat & BDRC_2) == BDRC_2)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "BDRC_2:[0x%08x]", ui32IntrStat);
	}

	//BDRC_3
	if((ui32IntrStat & BDRC_3) == BDRC_3)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "BDRC_3:[0x%08x]", ui32IntrStat);
	}

	//Err_Rpt
	if((ui32IntrStat & ERR_RPT) == ERR_RPT)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "ERR_RPT:[0x%08x]", ui32IntrStat);

		eRet = SDEC_ErrIntrCheck(stpSdecParam);

		if(LX_IS_ERR(eRet))
		{
			SDEC_DEBUG_Print("SDEC_ErrIntrCheck failed:[%d]", eRet);
		}
	}

	//CHA GPB_Full_L
	if((ui32IntrStat & GPB_FULL_CHA_GPL) == GPB_FULL_CHA_GPL)
	{
		/* SDEC_Full_Proc(stpSdecParam, channel, isHigh) */
		SDEC_Full_Proc(stpSdecParam, 0, 0);
	}

	//CHA GPB_Full_H
	if((ui32IntrStat & GPB_FULL_CHA_GPH) == GPB_FULL_CHA_GPH)
	{
		/* SDEC_Full_Proc(stpSdecParam, channel, isHigh) */
		SDEC_Full_Proc(stpSdecParam, 0, 1);
	}

	//CHB GPB_Full_L
	if((ui32IntrStat & GPB_FULL_CHB_GPL) == GPB_FULL_CHB_GPL)
	{
		/* SDEC_Full_Proc(stpSdecParam, channel, isHigh) */
		SDEC_Full_Proc(stpSdecParam, 1, 0);
	}

	//CHB GPB_Full_H
	if((ui32IntrStat & GPB_FULL_CHB_GPH) == GPB_FULL_CHB_GPH)
	{
		/* SDEC_Full_Proc(stpSdecParam, channel, isHigh) */
		SDEC_Full_Proc(stpSdecParam, 1, 1);
	}

	//CHA GPB_Data_L
	if((ui32IntrStat & GPB_DATA_CHA_GPL) == GPB_DATA_CHA_GPL)
	{
		/* SDEC_Section_Proc(stpSdecParam, channel, isHigh) */
		isFound = SDEC_Section_Proc(stpSdecParam, 0, 0);
	}

	//CHA GPB_Data_H
	if((ui32IntrStat & GPB_DATA_CHA_GPH) == GPB_DATA_CHA_GPH)
	{
		/* SDEC_Section_Proc(stpSdecParam, channel, isHigh) */
		isFound = SDEC_Section_Proc(stpSdecParam, 0, 1);
	}

	//CHB GPB_Data_L
	if((ui32IntrStat & GPB_DATA_CHB_GPL) == GPB_DATA_CHB_GPL)
	{
		/* SDEC_Section_Proc(stpSdecParam, channel, isHigh) */
		isFound = SDEC_Section_Proc(stpSdecParam, 1, 0);
	}

	//CHB GPB_Data_H
	if((ui32IntrStat & GPB_DATA_CHB_GPH) == GPB_DATA_CHB_GPH)
	{
		/* SDEC_Section_Proc(stpSdecParam, channel, isHigh) */
		isFound = SDEC_Section_Proc(stpSdecParam, 1, 1);
	}

	/* if found notifying section data, notify to appl layer */
	if(isFound)	queue_work(stpSdecParam->workqueue, &stpSdecParam->Notify);

	//CHA TP_INFO
	if((ui32IntrStat & TP_INFO_CHA) == TP_INFO_CHA)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "TP_INFO_CHA:[0x%08x]", ui32IntrStat);

		eRet = SDEC_TPInfoIntrCheck(stpSdecParam, 0);

		if(LX_IS_ERR(eRet))
		{
			SDEC_DEBUG_Print("SDEC_TPInfoIntrCheck failed:[%d]", eRet);
		}
	}

	//CHB TP_INFO
	if((ui32IntrStat & TP_INFO_CHB) == TP_INFO_CHB)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR, "TP_INFO_CHB:[0x%08x]", ui32IntrStat);

		eRet = SDEC_TPInfoIntrCheck(stpSdecParam, 1);

		if(LX_IS_ERR(eRet))
		{
			SDEC_DEBUG_Print("SDEC_TPInfoIntrCheck failed:[%d]", eRet);
		}
	}

	//CHA Sec ERR
	if((ui32IntrStat & SEC_ERR_CHA) == SEC_ERR_CHA)
	{
		stSeIntrStat = SDEC_HAL_SEGetIntrStat(0);
		SDEC_DTV_SOC_Message( SDEC_ISR, "SEC_ERR_CHA:[0x%08x]", ui32IntrStat);

		SDEC_DTV_SOC_Message( SDEC_ISR, "CHA valid:[0x%08x], err_type:[0x%08x], filter_idx:[0x%08x]",
								stSeIntrStat.valid, \
								stSeIntrStat.err_type, \
								stSeIntrStat.filter_idx);
	}

	//CHB Sec ERR
	if((ui32IntrStat & SEC_ERR_CHB) == SEC_ERR_CHB)
	{
		stSeIntrStat = SDEC_HAL_SEGetIntrStat(1);
		SDEC_DTV_SOC_Message( SDEC_ISR, "SEC_ERR_CHB:[0x%08x]", ui32IntrStat);

		SDEC_DTV_SOC_Message( SDEC_ISR, "CHA valid:[0x%08x], err_type:[0x%08x], filter_idx:[0x%08x]",
								stSeIntrStat.valid, \
								stSeIntrStat.err_type, \
								stSeIntrStat.filter_idx);
	}

	/* if found notifying section data, notify to appl layer */
	SDEC_Notify(&stpSdecParam->Notify);
}

#define SDEC_ERR_INTR_PRINT(field, str) \
{\
	if(field & 0x3) {\
		SDEC_DTV_SOC_Message( SDEC_ISR, str " : %x", field); \
		if(field & 0x1) SDEC_DTV_SOC_Message( SDEC_ISR, RED_COLOR "Ch[A]: " str NORMAL_COLOR); \
		if(field & 0x2) SDEC_DTV_SOC_Message( SDEC_ISR, RED_COLOR "Ch[B]: " str NORMAL_COLOR); \
	}\
}

DTV_STATUS_T SDEC_ErrIntrCheck(S_SDEC_PARAM_T *stpSdecParam)
{
	DTV_STATUS_T eRet = OK;

	if (stpSdecParam == NULL)
	{
		SDEC_DEBUG_Print("Invalid parameters");
		eRet = NOT_OK;

		goto error;
	}
#if 0
	stErrIntrStat = stpSdecParam->stSDEC_IO_Reg->err_intr_stat;
	//stErrIntrStat = SDEC_HAL_GetErrorInterruptStat();

	SDEC_ERR_INTR_PRINT(stErrIntrStat.test_dcont,		"TEST_DCOUNT");

	SDEC_ERR_INTR_PRINT(stErrIntrStat.sync_lost, 		"SYNC_LOST");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.sb_dropped, 	"SB_DROPPED");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.cdif_ovflow,	"CDIF_OVFLOW");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.cdif_wpage, 	"CDIF_WPAGE");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.cdif_rpage, 	"CDIF_RPAGE");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.stcc_dcont, 		"STCC_DCOUNT");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.mpg_pd, 		"MPG_PD");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.mpg_ts, 		"MPG_TS");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.mpg_dup, 		"MPG_DUP");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.mpg_cc, 			"MPG_CC");
	SDEC_ERR_INTR_PRINT(stErrIntrStat.mpg_sd, 		"MPG_SD");
#endif
	return (eRet);

error:
	return (eRet);

}


DTV_STATUS_T SDEC_TPInfoIntrCheck(S_SDEC_PARAM_T *stpSdecParam, UINT8 ui8Ch)
{
	DTV_STATUS_T eRet = OK;
	TPI_ISTAT stTPIInfo, stTPIInfo_remain, *stpTPIInfo;

	UINT8 ui8PIDIdx = 0x0;
//	UINT32 ui32TPIfoIntrCh[2] = {0x1, 0x2};
	UINT8 i = 0;

	if (stpSdecParam == NULL)
	{
		SDEC_DEBUG_Print("Invalid parameters");
		eRet = NOT_OK;

		goto error;
	}

	//stTPIInfo = stpSdecParam->stSDEC_MPG_Reg[ui8Ch]->tpi_istat;
	stTPIInfo = SDEC_HAL_TPIGetIntrStat(ui8Ch);
	stpTPIInfo = &stTPIInfo;
	for(i=0;i<TPI_ITR_BUF_SIZE-1;i++)
	{
		// to be empty of TPI Interrupt Buffer, process last one only 
		stTPIInfo_remain = SDEC_HAL_TPIGetIntrStat(ui8Ch);
		if( 0 != stTPIInfo_remain.pusi)
		{
			SDEC_DTV_SOC_Message( SDEC_ISR,"RTPI[%d][%d]",stTPIInfo_remain.pidf_idx, stTPIInfo_remain.pusi);
			stpTPIInfo = &stTPIInfo_remain;
		}
	}

	ui8PIDIdx  = stpTPIInfo->pidf_idx;

	if(stpTPIInfo->afef == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:afef", ui8PIDIdx);
	}
	if(stpTPIInfo->tpdf == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:tpdf", ui8PIDIdx);
	}
	if(stpTPIInfo->spf == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:SPF", ui8PIDIdx);
	}
	if(stpTPIInfo->espi == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:ESPI", ui8PIDIdx);
	}
	if(stpTPIInfo->rai == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:RAI", ui8PIDIdx);
	}
	if(stpTPIInfo->di == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:DI", ui8PIDIdx);
	}
	if(stpTPIInfo->tpri == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:TPRI", ui8PIDIdx);
	}
	if(stpTPIInfo->pusi == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:PUSI", ui8PIDIdx);

		stpSdecParam->stPIDMap[ui8Ch][ui8PIDIdx].stStatusInfo.f.SDEC_FLTSTATE_SCRMREADY = 0x1;

		if(stpTPIInfo->tsc == 0x2 || stpTPIInfo->tsc == 0x3)
		{
			stpSdecParam->stPIDMap[ui8Ch][ui8PIDIdx].stStatusInfo.f.SDEC_FLTSTATE_SCRAMBLED = 0x1;
		}

		/* check once at a time */
		// jinhwan.bae for No Audio Issue, 2012.04.26 
		// replaced following spin lock I/F
#if 0 // gaius original
		SDEC_HAL_PIDFScrambleCheck(ui8Ch, ui8PIDIdx, SDEC_HAL_DISABLE);
		SDEC_HAL_TPISetIntrPayloadUnitStartIndicator(ui8Ch, SDEC_HAL_DISABLE);
#else
		SDEC_SetPidf_TPI_IEN_Enable(stpSdecParam, ui8Ch, ui8PIDIdx, SDEC_HAL_DISABLE);
		SDEC_HAL_TPISetIntrPayloadUnitStartIndicator(ui8Ch, SDEC_HAL_DISABLE);
#endif
	}
	if(stpTPIInfo->tei == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:TEI", ui8PIDIdx);
	}
	if(stpTPIInfo->tsc_chg == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:TSC_CHG", ui8PIDIdx);
	}
	if(stpTPIInfo->auto_sc_chk == 0x1)
	{
		SDEC_DTV_SOC_Message( SDEC_ISR,"idx[%d]:AUTO_SC_CHK", ui8PIDIdx);
	}

	return (eRet);

error:
	return (eRet);
}


void SDEC_PollThread(void *param)
{
	S_SDEC_PARAM_T 	*stpSdecParam 	= (S_SDEC_PARAM_T*) param;
	LX_SDEC_CFG_T* 	pSdecConf 		= SDEC_CFG_GetConfig();

	UINT8			ui8Ch = 0, ui8cdif_full = 0, ui8cdif_ovf = 0;

	unsigned long 	flags = 0;

	/* polling PES s/w parser */
#if	( LX_SDEC_USE_KTHREAD_PES == 1)
	if(pSdecConf->noPesBug == 0)
	{
		/* check if there is PES filter */
		if(SDEC_PES_IsPESFlt())
		{
			SDEC_PES_Thread(stpSdecParam);
		}
	}
#endif

	/* check memory write fifo overflow bit */
	//chan_stat = SDEC_HAL_GetChannelStatus(ui8Ch);
	//if(chan_stat.mwf_ovf == 1)
	if(SDEC_HAL_ChanStatGetMWFOverFlow(ui8Ch))
	{
		if ( stpSdecParam->ui8SDMWCResetMode == E_SDEC_RESET_MODE_ENABLE || /* Reseting SDMWC is enabled */
			/* Reseting SDMWC is permitted only 1 time and this is first time. */
			 ( stpSdecParam->ui8SDMWCResetMode == E_SDEC_RESET_MODE_ONETIME  && stpSdecParam->ui8SDMWCResetNum == 0))
		{

			/* increase SDWMC Reset Number */
			if (stpSdecParam->ui8SDMWCResetMode == E_SDEC_RESET_MODE_ONETIME)
				  stpSdecParam->ui8SDMWCResetNum++;

			spin_lock_irqsave(&stpSdecParam->stSdecResetSpinlock, flags);

			/* print for debug */
			SDEC_DTV_SOC_Message(SDEC_RESET, YELLOW_COLOR "SDMWC RESET" NORMAL_COLOR "");

			/* 0xC000B220(SDIO-CDIC)*/
			SDEC_DTV_SOC_Message(SDEC_RESET, "CDIC       [0x%08x]", SDEC_HAL_CIDCGet(ui8Ch));

			/* 0xC000B260(SDIO-SDMWC)*/
			SDEC_DTV_SOC_Message(SDEC_RESET, "SDMWC      [0x%08x]", SDEC_HAL_SDMWCGet());

			/* 0xC000B264(SDIO-SDMWC_STAT)*/
			SDEC_DTV_SOC_Message(SDEC_RESET, "SDMWC_STAT [0x%08x]", SDEC_HAL_SDMWCGetStatus());

			/* 0xC000B418(SDCORE-CHAN_STAT)*/
			SDEC_DTV_SOC_Message(SDEC_RESET, "CHAN_STAT  [0x%08x]", SDEC_HAL_GetChannelStatus2(ui8Ch));

			/* 0xC000B420(SDCORE-CC_CHK_EN)*/
			SDEC_DTV_SOC_Message(SDEC_RESET, "CC_CHK_EN  [0x%08x%08x]", SDEC_HAL_CCCheckEnableGet(ui8Ch, 0), SDEC_HAL_CCCheckEnableGet(ui8Ch, 1));

/* gaius.lee 2011.12.05 disable for test */
#if 0
			SDEC_InputPortEnable(stpSdecParam->eInputPath[ui8Ch], SDEC_HAL_DISABLE);

			/* wait 1 ms */
			OS_UsecDelay(1000);

			/* Reset SDMWC */
			//SDEC_SDMWCReset(ui8Ch);
			SDEC_TEAH_Reset();

			/* wait 1 ms */
			OS_UsecDelay(1000);

			SDEC_InputPortEnable(stpSdecParam->eInputPath[ui8Ch], SDEC_HAL_ENABLE);
#endif
			stpSdecParam->ui8CDICResetMode = E_SDEC_RESET_MODE_DISABLE;
			stpSdecParam->ui8SDMWCResetMode = E_SDEC_RESET_MODE_DISABLE;


			spin_unlock_irqrestore(&stpSdecParam->stSdecResetSpinlock, flags);

		}
		else
		{
			switch(stpSdecParam->ui8SDMWCResetMode)
			{
				case E_SDEC_RESET_MODE_DISABLE :
					//SDEC_DTV_SOC_Message(SDEC_RESET, "SDMWC RESET is disabled\n");
					break;
				case E_SDEC_RESET_MODE_ONETIME :
					//SDEC_DTV_SOC_Message(SDEC_RESET, "SDMWC RESET is one time mode and number is %d.", stpSdecParam->ui8SDMWCResetNum);
					break;
				default:
					SDEC_DTV_SOC_Message(SDEC_RESET, "??? SDMWC RESET MODE is %d\n", stpSdecParam->ui8SDMWCResetMode);
					break;
			}
		}
	}


	/* polling SDEC stuck */
	for ( ui8Ch = 0 ; ui8Ch < 2 ; ui8Ch++ )
	{
		/* check input fifo overflow and full bit */
		//ui8cdif_full	= stpSdecParam->stSDEC_IO_Reg->cdic[ui8Ch].cdif_full;
		//ui8cdif_ovf		= stpSdecParam->stSDEC_IO_Reg->cdic[ui8Ch].cdif_ovflow;
		ui8cdif_full	= SDEC_HAL_CIDCGetCdif_Full(ui8Ch);
		ui8cdif_ovf		= SDEC_HAL_CIDCGetCdif_OverFlow(ui8Ch);

		/* this means something bad happen */
		if(ui8cdif_full || ui8cdif_ovf)
		{
			if ( stpSdecParam->ui8CDICResetMode == E_SDEC_RESET_MODE_ENABLE ||	/* Reseting SDMWC is enabled */
				/* Reseting SDMWC is permitted only 1 time and this is first time. */
				 ( stpSdecParam->ui8CDICResetMode == E_SDEC_RESET_MODE_ONETIME	&& stpSdecParam->ui8CDICResetNum == 0))
			{
				/* increase SDWMC Reset Number */
				if (stpSdecParam->ui8CDICResetMode == E_SDEC_RESET_MODE_ONETIME)
					  stpSdecParam->ui8CDICResetNum++;

				spin_lock_irqsave(&stpSdecParam->stSdecResetSpinlock, flags);

				SDEC_DTV_SOC_Message(SDEC_RESET,  RED_COLOR "SDIO RESET CH[%d]" NORMAL_COLOR "", ui8Ch );

				/* 0xC000B220(SDIO-CDIC)*/
				SDEC_DTV_SOC_Message(SDEC_RESET, "CDIC       [0x%08x]", SDEC_HAL_CIDCGet(ui8Ch));

				/* 0xC000B260(SDIO-SDMWC)*/
				SDEC_DTV_SOC_Message(SDEC_RESET, "SDMWC      [0x%08x]", SDEC_HAL_SDMWCGet());

				/* 0xC000B264(SDIO-SDMWC_STAT)*/
				SDEC_DTV_SOC_Message(SDEC_RESET, "SDMWC_STAT [0x%08x]", SDEC_HAL_SDMWCGetStatus());

				/* 0xC000B418(SDCORE-CHAN_STAT)*/
				SDEC_DTV_SOC_Message(SDEC_RESET, "CHAN_STAT  [0x%08x]", SDEC_HAL_GetChannelStatus2(ui8Ch));

				/* 0xC000B420(SDCORE-CC_CHK_EN)*/
				SDEC_DTV_SOC_Message(SDEC_RESET, "CC_CHK_EN  [0x%08x%08x]", SDEC_HAL_CCCheckEnableGet(ui8Ch, 0), SDEC_HAL_CCCheckEnableGet(ui8Ch, 1));

				/* disable channel input and wait 0.01 ms */
				SDEC_InputPortEnable(stpSdecParam->eInputPath[ui8Ch], SDEC_HAL_DISABLE);

				/* Reset Input port */
				SDEC_InputPortReset(ui8Ch);

				/* CLEAR CC_CHK_EN Register */
				SDEC_HAL_CCCheckEnableSet(ui8Ch, 0, 0);
				SDEC_HAL_CCCheckEnableSet(ui8Ch, 1, 0);

				/* enable channel input */
				SDEC_InputPortEnable(stpSdecParam->eInputPath[ui8Ch], SDEC_HAL_ENABLE);

				spin_unlock_irqrestore(&stpSdecParam->stSdecResetSpinlock, flags);
			}
			else
			{
				switch(stpSdecParam->ui8CDICResetMode)
				{
					case E_SDEC_RESET_MODE_DISABLE :
						//SDEC_DTV_SOC_Message(SDEC_RESET, "SDIO RESET is disabled\n");
						break;
					case E_SDEC_RESET_MODE_ONETIME :
						//SDEC_DTV_SOC_Message(SDEC_RESET, "SDIO RESET is one time mode and number is %d.\n", stpSdecParam->ui8CDICResetNum);
						break;
					default:
						SDEC_DTV_SOC_Message(SDEC_RESET, "??? SDIO RESET MODE is %d\n", stpSdecParam->ui8CDICResetMode);
						break;
				}
			}
		}
	}
}


