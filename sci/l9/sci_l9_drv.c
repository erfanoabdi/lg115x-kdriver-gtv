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

#include <linux/delay.h>

#include "os_util.h"

#include "./../../sys/l9/ctop_ctrl_reg_l9.h"

#include "./../sci_err.h"
#include "./../sci_cfg.h"

#include "sci_l9_reg.h"
#include "sci_l9_isr.h"
#include "sci_kapi.h"


#define SCI_L9_MAX_RFD	0x3F
#define SCI_L9_MAX_TFD	0x1F
#define SCI_L9_MAX_INFS	0x3F
#define SCI_L9_MAX_WTX	0x3F
#define SCI_L9_MAX_T	15


#define SCI_L9_INPUT_CLK	198
#define SCI_L9_INPUT_CLK_DIV	32
#define SCI_L9_CRD_OPERATING_CLK	3375
#define SCI_L9_CRD_OPERATING_CLK_ADJ	1000	// L9 Card Operating Clock is 3.375
#define SCI_L9_CLK_ADJ_VALUE		10

#define SCI_L9_DEFAULT_FS	372
#define SCI_L9_DEFAULT_DI	1



/*========================================================================================
	Implementation Group
========================================================================================*/

/**********************************************************************
	Static Functions
**********************************************************************/

/**
@fn int _SCI_L9_CalculateBaud(int FS, int DI)
@brief
@param FS
@param DI
@return baud
*/
static UINT32 _calculateBaud(UINT32 FS, UINT32 DI)
{
		return (SCI_L9_INPUT_CLK * SCI_L9_CRD_OPERATING_CLK_ADJ * SCI_L9_CLK_ADJ_VALUE * FS / DI / SCI_L9_INPUT_CLK_DIV / SCI_L9_CRD_OPERATING_CLK / SCI_L9_CLK_ADJ_VALUE);
}

/**
@fn int _SCI_L9_CalculateBR(int baud)
@brief
@param baud
@return br
*/
static UINT32 _calculateBR(UINT32 baud)
{
	return baud / 10;
}

/**
@fn int _SCI_L9_CalculateFT(int baud)
@brief
@param baud
@return ft
*/
static UINT32 _calculateFT(UINT32 baud)
{
		return baud % 10;
}

/**
@fn SCI_RETURN_T _SCI_L9_ReadFIFO(UINT8 *pFIFO, UINT32 i32FIFOSize)
@brief
@param pFIFO
@param i32FIFOSize
@return 0 : success
*/
static SCI_RETURN_T _readFIFO(UINT8 *pFIFO, UINT32 i32FIFOSize)
{
	SCI_RETURN_T ret = 0;
	UINT32 i;

	UINT32 quotient = 0;
	UINT32 remainder = 0;

	UINT8 *rx_ptr = pFIFO;

	UINT32 rfdDepth = SCI_L9_MAX_RFD;


	//SCI_RdFL(sci_rf_depth);
	//rfdDepth = SCI_Rd(sci_rf_depth);

	//printk("rf dep:%d\n", rfdDepth);

	quotient = i32FIFOSize / rfdDepth;
	remainder = i32FIFOSize % rfdDepth;

	if(!quotient && remainder)
		SCI_L9_EnableIRQ(SCI_L9_IRQ_FRF);

	//move to ReadFIFO
	while(quotient > 0)
	{
		if(quotient == 1 && !remainder)
			ret = SCI_L9_WaitIRQ(SCI_L9_IRQ_EOR, 200);
		else
			ret = SCI_L9_WaitIRQ(SCI_L9_IRQ_FRF, 200);

		if( ret != SCI_SUCCESS)
			return SCI_FAIL;


		for(i = 0; i < rfdDepth; i++)
		{
			SCI_RdFL(sci_data);
			SCI_Rd01(sci_data, data, rx_ptr[i]);
		}

		rx_ptr += rfdDepth;
		quotient--;
	}

	SCI_L9_DisableIRQ(SCI_L9_IRQ_FRF);

	if(remainder)
	{
		ret = SCI_L9_WaitIRQ(SCI_L9_IRQ_EOR, 200);

		if( ret != SCI_SUCCESS)
			return SCI_FAIL;

		for(i = 0; i < remainder; i++)
		{
			SCI_RdFL(sci_data);
			SCI_Rd01(sci_data, data, rx_ptr[i]);
		}
	}

	return SCI_SUCCESS;
}

/**
@fn static INT32 _SCI_L9_WriteFIFO(UINT8 *pFIFO,INT32 i32FIFOSize)
@brief
@param pFIFO
@param i32FIFOSize
@return 0 : success
@return -1 : failure
*/
static SCI_RETURN_T _writeFIFO(UINT8 *pFIFO,UINT32 i32FIFOSize)
{
	SCI_RETURN_T ret = 0;

	UINT32 i = 0;

	UINT32 quotient = 0;
	UINT32 remainder = 0;

	UINT8 *tx_ptr = pFIFO;

	quotient = i32FIFOSize / SCI_L9_MAX_TFD;
	remainder = i32FIFOSize % SCI_L9_MAX_TFD;

	SCI_L9_EnableIRQ(SCI_L9_IRQ_FTE);

	/* TX on */
	SCI_Wr(sci_cmd, 0);
	SCI_Wr01(sci_cmd, tx_en, 1);
	SCI_WrFL(sci_cmd);

	while(quotient != 0)
	{
		for(i = 0; i < SCI_L9_MAX_TFD; i++)
		{
			SCI_Wr(sci_data, tx_ptr[i]);
			SCI_WrFL(sci_data);
		}

		ret = SCI_L9_WaitIRQ(SCI_L9_IRQ_FTE, 400);

		if (ret != SCI_SUCCESS)
		{
			SCI_L9_DisableIRQ(SCI_L9_IRQ_FTE);

			return SCI_FAIL;
		}

		tx_ptr += SCI_L9_MAX_TFD;
		quotient--;
	}

	SCI_L9_DisableIRQ(SCI_L9_IRQ_FTE);

	if(remainder)
	{
		for(i = 0; i < remainder; i++)
		{
			SCI_Wr(sci_data, tx_ptr[i]);
			SCI_WrFL(sci_data);
		}
	}

	return SCI_SUCCESS;
}

/**
@fn void _SCI_L9_PrepareForATR(void)
@brief set configration before read Answer to Reset(ATR)
@param void
@return void
*/
static void _prepareForATR(void)
{
	// calculate baud parameter value for setting baud rate
	UINT32 baud = _calculateBaud(SCI_L9_DEFAULT_FS, SCI_L9_DEFAULT_DI);

	UINT32 brValue = _calculateBR(baud);
	UINT32 ftValue = _calculateFT(baud);


	SCI_Wr(sci_ext, 0);
	SCI_Wr01(sci_ext, if_mod, 0x01);
	SCI_WrFL(sci_ext);

	SCI_Wr(sci_set, 0);
	SCI_WrFL(sci_set);

	SCI_Wr(sci_dly, 0);
	SCI_Wr01(sci_dly, clk_div, 0x07);
	SCI_WrFL(sci_dly);


	SCI_Wr(sci_br, 0);
	SCI_Wr01(sci_br, br, brValue);
	SCI_WrFL(sci_br);

	SCI_Wr(sci_ft, 0);
	SCI_Wr01(sci_ft, ft, ftValue);
	SCI_WrFL(sci_ft);

	SCI_Wr(sci_mode, 0);
	SCI_Wr01(sci_mode, max_rpt, 0x03);
	SCI_WrFL(sci_mode);


	SCI_Wr(sci_fc, 0);
	SCI_Wr01(sci_fc, par_retry, 3);
	SCI_Wr01(sci_fc, t_fc_en, 1);
	SCI_WrFL(sci_fc);

	SCI_Wr(sci_wi, 0);
	SCI_Wr01(sci_wi, wi, 0x3F);
	SCI_WrFL(sci_wi);

	SCI_Wr(sci_agt, 0);
	SCI_WrFL(sci_agt);


	SCI_Wr(sci_di, 0);
	SCI_Wr01(sci_di, di, 0xF);
	SCI_WrFL(sci_di);

}

/**
@fn SCI_RETURN_T _SCI_L9_ReadATR(LX_SCI_ATR_T* pATR)
@brief read Answer to Reset(ATR)
@param pATR : ATR pointer
@return SCI_INVAL_PARAM
@return SCI_SUCCESS
*/
static SCI_RETURN_T _readATR(LX_SCI_ATR_T* pATR)
{
	UINT32 iCurDepth, depth = 0;
	UINT32 historicCnt = 0;
	SCI_RETURN_T waitFull = SCI_SUCCESS;
	UINT32 i;

	/* Prepare for ATR */
	_prepareForATR();

	 //Clear FIFO
	SCI_Wr(sci_cmd, 0);
	SCI_Wr01(sci_cmd, clr_ft, 1);
	SCI_Wr01(sci_cmd, clr_fr, 1);
	SCI_WrFL(sci_cmd);


	/* ATR Reception using burst mode for TS & TA0 */
	iCurDepth = 2;
	SCI_Wr(sci_rf_depth, 0);
	SCI_Wr01(sci_rf_depth, rf_depth, iCurDepth);
	SCI_WrFL(sci_rf_depth);

	SCI_Wr(sci_dds, 0);
	SCI_Wr01(sci_dds, reset_low, 1);
	SCI_Wr01(sci_dds, p_mon_hi, 1);
	SCI_WrFL(sci_dds);


	/* Mask IRQs */
	SCI_L9_EnableIRQ(SCI_L9_IRQ_FRF);


	//cold reset
	SCI_Wr(sci_cmd, 0x08);
	SCI_WrFL(sci_cmd);

	//Receive enable
	SCI_Wr(sci_cmd, 0x01);
	SCI_WrFL(sci_cmd);



	/* wait for full */
	waitFull = SCI_L9_WaitIRQ(SCI_L9_IRQ_FRF, 200);


	if (waitFull != SCI_SUCCESS)
	{
		/* sci pwr off */
		SCI_Wr(sci_cmd, 0);
		SCI_Wr01(sci_cmd, pwr_off, 1);
		SCI_WrFL(sci_cmd);
		SCI_ERROR("There was error during BCAS read.\n");
		return SCI_FAIL;
	}


	/* read TS, T0 */
	for(i = 0; i < iCurDepth; i++)
	{
		SCI_RdFL(sci_data);
		SCI_Rd01(sci_data, data, pATR->data[i]);
	}

	historicCnt = (UINT32)(pATR->data[1] & 0x0f);


	do
	{
		/* Count TAi~TDi */
		depth = (UINT32)(((pATR->data[iCurDepth-1]&0x10)>>4)+
					((pATR->data[iCurDepth-1]&0x20)>>5) +
					((pATR->data[iCurDepth-1]&0x40)>>6) +
					((pATR->data[iCurDepth-1]&0x80)>>7));

		if (depth == 0)
		{
			break;
		}

		if (iCurDepth + depth <= SCI_MAX_ATR)
		{
			SCI_Wr01(sci_rf_depth, rf_depth, depth);
			SCI_WrFL(sci_rf_depth);

			/* wait for full */
			waitFull = SCI_L9_WaitIRQ(SCI_L9_IRQ_FRF, 200);


			if (waitFull != SCI_SUCCESS)
			{
				/* sci off */
				SCI_Wr(sci_cmd, 0);
				SCI_Wr01(sci_cmd, pwr_off, 1);
				SCI_WrFL(sci_cmd);

				SCI_ERROR("There was error during BCAS read.\n");
				return SCI_FAIL;
			}

			/* read TAi~TDi if it is exists */
			//_readFIFO((pATR->data + iCurDepth), depth);

			for(i = 0; i < depth; i++)
			{
				SCI_RdFL(sci_data);
				SCI_Rd01(sci_data, data, (pATR->data + iCurDepth)[i]);
			}


			iCurDepth += depth;

			if(!isCardInserted)
			{
				/* sci off */
				SCI_Wr(sci_cmd, 0);
				SCI_Wr01(sci_cmd, pwr_off, 1);
				SCI_WrFL(sci_cmd);

				SCI_ERROR("There was error during BCAS read.\n");
				return SCI_FAIL;
			}
		}
		else
		{
			return SCI_FAIL;
		}

		if(!isCardInserted)
		{
			/* sci off */
			SCI_Wr(sci_cmd, 0);
			SCI_Wr01(sci_cmd, pwr_off, 1);
			SCI_WrFL(sci_cmd);

			SCI_ERROR("There was error during BCAS read.\n");
			return SCI_FAIL;
		}

	}while (iCurDepth < SCI_MAX_ATR);

	if (historicCnt != 0)
	{
		/* read historic bytes & clk*/
		depth = historicCnt + 1;
	}
	else
	{
		/* read clk */
		depth = 1;
	}


	//iCurDepth += depth;
	if(iCurDepth + depth <= SCI_MAX_ATR)
	{
		/* read operation */
		SCI_Wr01(sci_rf_depth, rf_depth, depth);
		SCI_WrFL(sci_rf_depth);


		/* wait for full */
		waitFull = SCI_L9_WaitIRQ(SCI_L9_IRQ_FRF, 200);
		if (waitFull != SCI_SUCCESS)
		{
			/* sci off */
			SCI_Wr(sci_cmd, 0);
			SCI_Wr01(sci_cmd, pwr_off, 1);
			SCI_WrFL(sci_cmd);
			SCI_ERROR("There was error during BCAS read.\n");
			return SCI_FAIL;
		}
		//_readFIFO((pATR->data + iCurDepth), depth);
		for(i = 0; i < depth; i++)
		{
			SCI_RdFL(sci_data);
			SCI_Rd01(sci_data, data, (pATR->data + iCurDepth)[i]);
		}

		iCurDepth += depth;

		if(!isCardInserted)
		{
			/* sci off */
			SCI_Wr(sci_cmd, 0);
			SCI_Wr01(sci_cmd, pwr_off, 1);
			SCI_WrFL(sci_cmd);

			SCI_ERROR("There was error during BCAS read.\n");
			return SCI_FAIL;
		}
	}
	else
	{
		return -1;
	}

	SCI_L9_DisableIRQ(SCI_L9_IRQ_FRF);

 	/* send a Null command */
	SCI_Wr(sci_cmd, 0);
	SCI_WrFL(sci_cmd);

	/* DisplayATR */
	{
		UINT32 i = 0;
		for( i = 0; i < iCurDepth; i++)
			SCI_PRINT("ATR[%02d]  =  0x%02X \n", i, pATR->data[i]);
	}
	pATR->data_count = iCurDepth;


	return SCI_SUCCESS;
}

/**********************************************************************
	Public Functions
**********************************************************************/
/**
@fn SCI_RETURN_T setVoltage(LX_SCI_VOLTAGE voltage)
@brief set voltage of smart card 3v or 5v
@param LX_SCI_VOLTAGE :
@return SCI_INVAL_PARAM
@return SCI_SUCCESS
*/
static SCI_RETURN_T _setVoltage(LX_SCI_VOLTAGE voltage)
{
	SCI_Wr(sci_set, 0x20);

	if (SCI_VOLTAGE_5V == voltage)
	{
		SCI_Wr01(sci_set, vcc_sel, 1);
	}
	else if (SCI_VOLTAGE_3V == voltage)
	{
		SCI_Wr01(sci_set, vcc_sel, 0);
	}
	else
	{
		SCI_Wr01(sci_set, vcc_sel, 0);
		SCI_ERROR("This voltage[%d] is not supported!!!\n", voltage);
		return SCI_INVAL_PARAM;
	}

	SCI_WrFL(sci_set);

	return SCI_SUCCESS;
}

/**
@fn LX_SCI_VOLTAGE getVoltage(void)
@brief get information of voltage
@param void
@return SCI_VOLTAGE_3V
@return SCI_VOLTAGE_5V
*/
static LX_SCI_VOLTAGE _getVoltage(void)
{
	UINT32 voltage = 0;
	LX_SCI_VOLTAGE ret = SCI_VOLTAGE_3V;

	SCI_RdFL(sci_set);
	SCI_Rd01(sci_set, vcc_sel, voltage);

	if (voltage == 0)
	{
		ret = SCI_VOLTAGE_3V;
	}
	else// if (voltage == 1)
	{
		ret = SCI_VOLTAGE_5V;
	}

	return ret;
}
/**
@fn SCI_L9_CARD_STATUS_T SCI_L9_GetCardStatus(void)
@brief get status of smart card
@param void
@return SCI_L9_CARD_READY :
@return SCI_L9_CARD_DETECTED :
@return SCI_L9_CARD_ACTIVATED :
@return SCI_L9_CARD_FAILED :
*/
static LX_SCI_STATUS _getStatus(void)
{
	UINT32 stat = 0;
	LX_SCI_STATUS ret = SCI_STATUS_FAIL;

	SCI_RdFL(sci_stat);
	SCI_Rd01(sci_stat, card, stat);

	switch(stat)
	{
		case 0:
			ret = SCI_STATUS_READY;
			break;
		case 1:
			ret = SCI_STATUS_DETECTED;
			break;
		case 2:
			ret = SCI_STATUS_ACTIVE;
			break;
		case 3:
			ret = SCI_STATUS_FAIL;
			break;
		default:
			ret = SCI_STATUS_FAIL;
			SCI_ERROR("Card status[%d] is not supported!\n",stat);
			break;
	}

	return ret;
}

/**
@fn SCI_RETURN_T getATR(LX_SCI_ATR_T* pATR)
@brief get Answer to Reset(ATR)
@param ptn ATR
@return SCI_SUCCESS
@return SCI_FAILED
@return SCI_NO_CARD
@return SCI_INVAL_PARAM
*/
static SCI_RETURN_T _reset(LX_SCI_ATR_T* pATR)
{
	SCI_RETURN_T ret = SCI_SUCCESS;
	LX_SCI_STATUS status =  _getStatus();

	/* Clear ATR buffer */
	memset(pATR, 0, sizeof(LX_SCI_ATR_T));

	if (status == SCI_STATUS_ACTIVE)
	{
		/* Reset */
		//printk("i");
		ret = _readATR(pATR);
	}
	else if (status == SCI_STATUS_DETECTED)
	{
//		WaitUntilCardisAvtive();

		/* Reset */
		ret = _readATR(pATR);
	}
	else if (status == SCI_STATUS_FAIL)
	{
		/* Failed to reset the card */
		/* Please, remove the card */
		return SCI_FAIL;
	}
	else if (status == SCI_STATUS_READY)
	{
		/* Card is not inserted */
		return SCI_NO_CARD;
	}
	else
	{
		return SCI_INVAL_PARAM;
	}

	return ret;
}


static SCI_RETURN_T _configIF(LX_SCI_ATR_T* pATR)
{
	/* Configure SCI by ATR */

	UINT32 baud = _calculateBaud(pATR->FI, pATR->DI);

	UINT32 brValue = _calculateBR(baud);
	UINT32 ftValue = _calculateFT(baud);


	SCI_Wr(sci_agt, 0);
	SCI_Wr01(sci_agt, agt, pATR->guard_time);
	SCI_WrFL(sci_agt);


	SCI_Wr(sci_tos, 0);
	SCI_Wr01(sci_tos, bwi, pATR->bwi);
	SCI_Wr01(sci_tos, cwi, pATR->cwi);
	SCI_WrFL(sci_tos);


	SCI_Wr(sci_mode, 0);
	SCI_Wr01(sci_mode, conv, pATR->direct_convention);
	SCI_Wr01(sci_mode, max_rpt, 0x03);

	SCI_Wr01(sci_mode, t, pATR->transmission_protocol);
	SCI_WrFL(sci_mode);


	SCI_Wr(sci_di, 0);
	SCI_Wr01(sci_di, di, pATR->DI);
	SCI_WrFL(sci_di);


	SCI_Wr(sci_br, 0);
	SCI_Wr01(sci_br, br, brValue);
	SCI_WrFL(sci_br);


	SCI_Wr(sci_ft, 0);
	SCI_Wr01(sci_ft, ft, ftValue);
	SCI_WrFL(sci_ft);


	SCI_Wr(sci_fc, 0);
	SCI_Wr01(sci_fc, par_retry, 3);
	SCI_Wr01(sci_fc, t_fc_en, 1);
	SCI_Wr01(sci_fc, r_fc_en, 1);
	SCI_WrFL(sci_fc);

	SCI_L9_EnableIRQ(SCI_L9_IRQ_EOT|SCI_L9_IRQ_EOR);


	return SCI_SUCCESS;
}


/**
@fn static SCI_RETURN_T _configINFS(UINT8 infs)
@brief Write a block into a smartcard
@param infs : NAD
@return PCB
@return -1 : failure
*/

static SCI_RETURN_T _configINFS(UINT8 infs)
{
	SCI_Wr01(sci_rf_depth, rf_depth, infs);
	SCI_WrFL(sci_rf_depth);

	SCI_Wr01(sci_tf_depth, tf_depth, infs);
	SCI_WrFL(sci_tf_depth);

	return SCI_SUCCESS;
}



/**
@fn static SCI_RETURN_T _configWTX(UINT8 wtx)
@brief Write a block into a smartcard
@param infs : NAD
@return PCB
@return -1 : failure
*/

static SCI_RETURN_T _configWTX(UINT8 wtx)
{
	SCI_Wr01(sci_rf_depth, rf_depth, wtx);
	SCI_WrFL(sci_rf_depth);

	SCI_Wr01(sci_tf_depth, tf_depth, wtx);
	SCI_WrFL(sci_tf_depth);

	return SCI_SUCCESS;
}



/**
@fn INT32 SCI_L9_WriteBlock(UINT8 *uptx)
@brief Write a block into a smartcard
@param uptx[0] : NAD
@param uptx[1] : PCB
@param uptx[2] : LEN
@param uptx[3~] : INF data
@return PCB
@return -1 : failure
*/
static SCI_RETURN_T _writeBlock(LX_SCI_BLOCK *uptx)
{
	SCI_RETURN_T ret = 0;
/*
	int i;
	printk(KERN_ERR "TX nad : %x\n", uptx->nad);
	printk(KERN_ERR "TX pcb : %x\n", uptx->pcb);
	printk(KERN_ERR "TX len : %x\n", uptx->len);


	printk(KERN_ERR "TX : ");
	for(i = 0; i < uptx->len; i++)
		printk(KERN_ERR "%02x ", uptx->buf[i]);
	printk(KERN_ERR "\n");
*/

	/* Rx, Tx Fifo Clear */
	SCI_Wr(sci_cmd, 0);
	SCI_Wr01(sci_cmd, clr_ft, 1);
	SCI_Wr01(sci_cmd, clr_fr, 1);
	SCI_WrFL(sci_cmd);

	SCI_Wr(sci_nad, uptx->nad);
	SCI_WrFL(sci_nad);

	SCI_Wr(sci_pcb, uptx->pcb);
	SCI_WrFL(sci_pcb);

	SCI_Wr(sci_len, uptx->len);
	SCI_WrFL(sci_len);


	/* Write data */
	ret = _writeFIFO(uptx->buf, uptx->len);
	if(ret != SCI_SUCCESS)
		return SCI_TRANSMIT_FAIL;

	ret = SCI_L9_WaitIRQ(SCI_L9_IRQ_EOT, 300);

	if(ret != SCI_SUCCESS)
		return SCI_TRANSMIT_FAIL;

	return SCI_SUCCESS;
}

/**
@fn INT32 SCI_L9_ReadBlock(UINT8 *uprx)
@brief Read a INF data from a smartcard
@param uptx : INF data
@return 0 : success
@return -1 : card reset is needed
@return -2 : Retransmission is needed
*/
static SCI_RETURN_T _readBlock(LX_SCI_BLOCK *uprx)
{
	SCI_RETURN_T ret;

	SCI_Wr(sci_cmd, 0);
	SCI_Wr01(sci_cmd, rx_en, 1);
	SCI_WrFL(sci_cmd);



	SCI_L9_EnableIRQ(SCI_L9_IRQ_FRNE);

	ret = SCI_L9_WaitIRQ(SCI_L9_IRQ_FRNE, 200);

	if( ret != SCI_SUCCESS)
		return SCI_FAIL;

	SCI_L9_DisableIRQ(SCI_L9_IRQ_FRNE);


	SCI_RdFL(sci_nad);
	uprx->nad = SCI_Rd(sci_nad);

	SCI_RdFL(sci_pcb);
	uprx->pcb = SCI_Rd(sci_pcb);

	SCI_RdFL(sci_len);
	uprx->len = SCI_Rd(sci_len);


	/* Read data */
	_readFIFO(uprx->buf, uprx->len);

/*
	{
		int i;
		printk(KERN_ERR "RX nad : %x\n", uprx->nad);
		printk(KERN_ERR "RX pcb : %x\n", uprx->pcb);
		printk(KERN_ERR "RX len : %x\n", uprx->len);


		printk(KERN_ERR "RX : ");
		for(i = 0; i < uprx->len; i++)
			printk(KERN_ERR "%02x ", uprx->buf[i]);
		printk(KERN_ERR "\n");
	}
*/

	return SCI_SUCCESS;
}

/**
@fn void _SCI_L9_Init(void)
@brief Intialize a smartcard interface device
@param void
@return void
*/
static void _InitClk_L9AX(void)
{
	CTOP_CTRL_L9_RdFL(ctr27);
	CTOP_CTRL_L9_RdFL(ctr39);

	/* SCI clk from crg 8 */
	CTOP_CTRL_L9_Wr01(ctr39, sc_clk_sel, 1);
	/* 1/8 */
	CTOP_CTRL_L9_Wr01(ctr27, sc_ext_sel, 4);

	CTOP_CTRL_L9_WrFL(ctr27);
	CTOP_CTRL_L9_WrFL(ctr39);
}

/**
@fn void _SCI_L9_Init(void)
@brief Intialize a smartcard interface device
@param void
@return void
*/
static void _InitClk_L9BX(void)
{
	CTOP_CTRL_L9B_RdFL(ctr27_reg_extclk_swrst);
	CTOP_CTRL_L9B_RdFL(ctr39_reg_gpio_mux_enable);

	/* SCI clk from crg 8 */
	CTOP_CTRL_L9B_Wr01(ctr27_reg_extclk_swrst, sc_ext_sel, 4);
	/* 1/8 */
	CTOP_CTRL_L9B_Wr01(ctr27_reg_extclk_swrst, f27_18_sel, 0);

	CTOP_CTRL_L9B_Wr01(ctr39_reg_gpio_mux_enable, sc_clk_sel, 1);

	CTOP_CTRL_L9B_WrFL(ctr27_reg_extclk_swrst);
	CTOP_CTRL_L9B_WrFL(ctr39_reg_gpio_mux_enable);
}

/**
@fn SCI_RETURN_T SCI_L9_Register(SCI_T* pSCI)
@brief Intialize a smartcard interface device
@param pfnCardNotifier
@return SCI_SUCCESS : success
@return SCI_ERROR : failure
*/
SCI_RETURN_T SCI_L9_Init(struct SCI_T* pSCI)
{
	SCI_RETURN_T ret = SCI_SUCCESS;


	ret = SCI_L9_InitRegister(pSCI);
	if (ret == SCI_SUCCESS)
	{
		pSCI->infs = SCI_L9_MAX_INFS;
		pSCI->wtx = SCI_L9_MAX_WTX;

		/* Register SCI fns */
		pSCI->reset = _reset;
		pSCI->configIF = _configIF;
		pSCI->setVoltage = _setVoltage;
		pSCI->getVoltage = _getVoltage;
		pSCI->writeBlock = _writeBlock;
		pSCI->readBlock = _readBlock;
		pSCI->getStatus = _getStatus;
		pSCI->configINFS = _configINFS;
		pSCI->configWTX = _configWTX;

		/* Register SCI interrupt service */
		SCI_L9_InitISR();
		SCI_L9_EnableIRQ(SCI_L9_IRQ_CDPWR|SCI_L9_IRQ_ERR);

		if (pSCI->chip == LX_CHIP_REV_L9_A0 || pSCI->chip == LX_CHIP_REV_L9_A1)
		{
			/* Clock setting */
			_InitClk_L9AX();
		}
		else if (pSCI->chip == LX_CHIP_REV_L9_B0)
		{
			/* Clock setting */
			_InitClk_L9BX();
		}
	}
	else
	{
		SCI_ERROR("Fail to Initilaize H/W registers\n");
		return SCI_FAIL;
	}

	return SCI_SUCCESS;
}

/**
@fn SCI_RETURN_T SCI_L9_Register(SCI_T* pSCI)
@brief Intialize a smartcard interface device
@param pfnCardNotifier
@return SCI_SUCCESS : success
@return SCI_ERROR : failure
*/
void SCI_L9_Exit(struct SCI_T* pSCI)
{
	SCI_L9_ExitRegister(pSCI);

	/* Unregister SCI interrupt service */
	SCI_L9_ExitISR();
}


