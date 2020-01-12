/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/*
/@file name : reg.h
/@description : register map for GFX
**/

typedef struct
{
	unsigned int bwCmdStatus	: 1;
	unsigned int 			: 15;
	unsigned int abwProcessingLine	: 13;
	unsigned int 			: 3;
} GFX_REG_COM_STATUS0_T;

typedef struct
{
    UINT32
    bwCmdQueueStatus                : 1,    //     0
                                    :15,    //  1:15 reserved
    abwNumOfCmd                     :10,    // 16:25
                                    : 4,    // 26:29 reserved
    CmdQueueState                   : 2;    // 30:31
} GFX_REG_COM_STATUS1_T;

typedef struct
{
	unsigned int bwBatchRunStatus	: 1;
	unsigned int 			: 15;
	unsigned int abwNumOfParam	: 10;
	unsigned int 			: 6;
} GFX_REG_COM_STATUS2_T;

typedef struct
{
	unsigned int bwBatchRunStatus	: 1;
	unsigned int 			: 31;
} GFX_REG_COM_INTR_STATUS_T;

typedef struct
{
	unsigned int bwIntEnable	: 1;
	unsigned int 			: 3;
	unsigned int bwQueueFullInt	: 1;
	unsigned int bwResetInt	: 1;
	unsigned int 			: 2;
	
	unsigned int abwIntrMode	: 2;
	unsigned int 			: 6;
	unsigned int abwNumOfCmd	: 8;
	unsigned int 			: 8;
} GFX_REG_COM_INTR_CTRL_T;

typedef struct
{
	unsigned int bwIntrClear	: 1;
	unsigned int 			: 31;
	
} GFX_REG_COM_INTR_CLEAR_T;

typedef struct
{

	unsigned int abwOpMode		: 3;		// GFX_REG_COM_OP_MODE_T
	unsigned int 			: 1;
	unsigned int bwEnableBusrtMode	: 1;
	unsigned int bwEnableCLUTUpdate	: 1;
	unsigned int 			: 6;
	unsigned int bwEnChromaFilter 	: 1;
	unsigned int abwChromaFilterMode	: 2;
	unsigned int 			: 17;

} GFX_REG_COM_OP_MODE_T;

typedef struct
{
	unsigned int abwCfltCoef1	: 12;
	unsigned int 			: 4;
	unsigned int abwCfltCoef0	: 12;
	unsigned int 			: 4;
} GFX_REG_COM_CFLT_COEF0;

typedef struct
{
	unsigned int abwCfltCoef3	: 12;
	unsigned int 			: 4;
	unsigned int abwCfltCoef2	: 12;
	unsigned int 			: 4;
} GFX_REG_COM_CFLT_COEF1;

typedef struct
{
	unsigned int abwCmdDelay	: 10;
	unsigned int 			: 22;
} GFX_REG_CMD_DELAY;

typedef struct
{
	unsigned int bwRun	: 1;
	unsigned int 			: 31;
} GFX_REG_COM_BATCH_RUN;

typedef struct
{
	unsigned int bwStart	: 1;
	unsigned int 			: 31;
} GFX_REG_COM_START;

typedef struct
{
	unsigned int bwPause	: 2;
	unsigned int 			: 30;
} GFX_REG_COM_PAUSE;

typedef struct
{
	unsigned int bwHalt	: 1;
	unsigned int 			: 31;
} GFX_REG_COM_HALT;

typedef struct
{
	unsigned int abwAddr	: 32;
} GFX_REG_PORT_BASE_ADDR;


typedef struct
{
	unsigned int abwStride	: 15;
	unsigned int 			: 17;
} GFX_REG_PORT_STRIDE;

typedef struct
{
	unsigned int abwPft	: 5;
	unsigned int 	: 3;
	unsigned int bwEndian	: 1;
	unsigned int 	: 23;
} GFX_REG_PORT_PFORMAT;

typedef struct
{
/*
	unsigned int abwAlpha0	: 8;
	unsigned int abwAlpha1	: 8;
	unsigned int abwAlpha2	: 8;
	unsigned int abwAlpha3	: 8;
*/
	unsigned int abwAlpha	: 32;
} GFX_REG_PORT_GALPHA;

typedef struct
{
	unsigned int abwAddr	: 8;
	unsigned int bwRW	: 1;
	unsigned int bwAutoUp	: 1;
	unsigned int 	: 22;
} GFX_REG_PORT_CLUT_CTRL;

typedef struct
{
	unsigned int bwCKeyEnable	: 1;
	unsigned int bwBitMaskEnable	: 1;
	unsigned int bwCOCEnable	: 1;
	unsigned int bwCSCEnable	: 1;
	unsigned int 			: 4;
	unsigned int bwCKeyMode	: 1;
	unsigned int 			: 3;
	unsigned int abwCSCCoefSel	: 2;
	unsigned int 	: 18;
} GFX_REG_PORT_CTRL;

typedef struct
{
	unsigned int abwValue	: 32;
} GFX_REG_PORT_CKEY0;

typedef struct
{
	unsigned int abwValue	: 32;
} GFX_REG_PORT_CKEY1;

typedef struct
{
	unsigned int abwValue	: 32;
} GFX_REG_PORT_REPLACE_COLOR;

typedef struct
{
	unsigned int abwMask	: 32;
} GFX_REG_PORT_BITMASK;

typedef struct
{
	unsigned int abwOrderChange7_0	: 2;
	unsigned int abwOrderChange15_8	: 2;
	unsigned int abwOrderChange23_16: 2;
	unsigned int abwOrderChange31_24: 2;
	unsigned int 			: 24;
} GFX_REG_PORT_COC_CTRL;

typedef struct
{
	unsigned int abwOutSel		: 3;
	unsigned int 			: 29;
} GFX_REG_BLD_OUT_SEL;

typedef struct
{
	unsigned int bwPMA0Enable	: 1;
	unsigned int bwPMA1Enable	: 1;
	unsigned int bwXOR0Enable	: 1;
	unsigned int bwXOR1Enable	: 1;
	unsigned int bwDIVEnable	: 1;
	unsigned int 			: 3;
	unsigned int abwCm0		: 2;
	unsigned int 			: 2;
	unsigned int abwAlpham0		: 2;
	unsigned int 			: 2;
	unsigned int abwComSelB		: 2;
	unsigned int 			: 2;
	unsigned int abwComSelG		: 2;
	unsigned int 			: 2;
	unsigned int abwComSelR		: 2;
	unsigned int 			: 2;
	unsigned int abwComSelAlpha	: 2;
	unsigned int 			: 2;
} GFX_REG_BLD_CTRL0;

typedef struct
{
	unsigned int abwB3Sel		: 4;
	unsigned int abwA3Sel		: 4;
	unsigned int abwB2Sel		: 4;
	unsigned int abwA2Sel		: 4;
	unsigned int abwB1Sel		: 4;
	unsigned int abwA1Sel		: 4;
	unsigned int abwB0Sel		: 4;
	unsigned int abwA0Sel		: 4;
} GFX_REG_BLD_CTRL1;

typedef struct
{
	unsigned int abwConstantCtrl	: 32;
} GFX_REG_BLD_CONST_CTRL;

typedef struct
{
	unsigned int abwRasterSel	: 4;
	unsigned int 			: 28;
} GFX_REG_BLD_ROP_CTRL;


typedef struct
{
	unsigned int abwAddr	: 32;
} GFX_REG_WR_BASE_ADDR;


typedef struct
{
	unsigned int abwStride	: 15;
	unsigned int 			: 17;
} GFX_REG_WR_STRIDE;

typedef struct
{
	unsigned int abwPft	: 5;
	unsigned int 	: 3;
	unsigned int bwEndian	: 1;
	unsigned int 	: 23;
} GFX_REG_WR_PFORMAT;

typedef struct
{
	unsigned int abwHSIZE	: 13;
	unsigned int 	: 3;
	unsigned int abwVSIZE	: 13;
	unsigned int 	: 3;

} GFX_REG_WR_SIZE;

typedef struct
{
	unsigned int abwAlpha	: 32;
/*
	unsigned int abwAlpha1  : 8;
	unsigned int abwAlpha2	: 8;
	unsigned int abwAlpha3	: 8;
*/
} GFX_REG_WR_GALPHA;

typedef struct
{
	unsigned int 	: 2;
	unsigned int bwCOCEnable	: 1;
	unsigned int bwCSCEnable	: 1;
	unsigned int 	: 28;
} GFX_REG_WR_CTRL;

typedef struct
{
	unsigned int abwCSCCoef	: 32;
} GFX_REG_WR_CSC_COEF;




typedef struct
{
	// common control
	GFX_REG_COM_INTR_STATUS_T	intr;
	GFX_REG_COM_STATUS0_T		status0;	
	GFX_REG_COM_STATUS1_T		status1;	
	GFX_REG_COM_STATUS2_T		status2;	
	GFX_REG_COM_INTR_CTRL_T		intr_ctrl;
	GFX_REG_COM_INTR_CLEAR_T	intr_clear;
	unsigned int reserved_00[2];
	
	GFX_REG_COM_OP_MODE_T	op_mode;
	GFX_REG_COM_CFLT_COEF0	cflt_coef0;
	GFX_REG_COM_CFLT_COEF1	cflt_coef1;
	GFX_REG_CMD_DELAY	cmd_delay;
	GFX_REG_COM_BATCH_RUN	run;
	GFX_REG_COM_START	start;
	GFX_REG_COM_PAUSE	pause;
	GFX_REG_COM_HALT	halt;
	
	unsigned int reserved_02[16];
	
	// read port 0
	unsigned int		in0_base_addr;
	GFX_REG_PORT_STRIDE	in0_stride;
	GFX_REG_PORT_PFORMAT	in0_format;
	unsigned int reserved_03[1];
	GFX_REG_PORT_GALPHA	in0_galpha;
	GFX_REG_PORT_CLUT_CTRL	in0_clut_ctrl;
	unsigned int		in0_data_clut;
	GFX_REG_PORT_CTRL	in0_ctrl;
	unsigned int 		in0_uiCKeyLowThr;
	unsigned int 		in0_uiCKeyHighThr;
	unsigned int 		in0_uiReplaceColor;
	unsigned int 		in0_uiBitMask;
	unsigned int	in0_coc_ctrl;
	unsigned int reserved_04[3];
	
	// read port 1
	unsigned int		in1_base_addr;
	GFX_REG_PORT_STRIDE	in1_stride;
	GFX_REG_PORT_PFORMAT	in1_format;
	unsigned int reserved_05[1];
	GFX_REG_PORT_GALPHA	in1_galpha;
	GFX_REG_PORT_CLUT_CTRL	in1_clut_ctrl;
	unsigned int		in1_data_clut;
	GFX_REG_PORT_CTRL	in1_ctrl;
	unsigned int 		in1_uiCKeyLowThr;
	unsigned int 		in1_uiCKeyHighThr;
	unsigned int 		in1_uiReplaceColor;
	unsigned int 		in1_uiBitMask;
	unsigned int	in1_coc_ctrl;
	unsigned int reserved_06[3];
	
	// read port 2
	unsigned int		in2_base_addr;
	GFX_REG_PORT_STRIDE	in2_stride;
	GFX_REG_PORT_PFORMAT	in2_format;
	unsigned int reserved_07[1];
	GFX_REG_PORT_GALPHA	in2_galpha;
	unsigned int reserved_08[11];
	
	GFX_REG_BLD_OUT_SEL	out_sel;
	GFX_REG_BLD_CTRL0	out_ctrl0;
	GFX_REG_BLD_CTRL1	out_ctrl1;
	GFX_REG_BLD_CONST_CTRL	out_const_ctrl;
	GFX_REG_BLD_ROP_CTRL	out_rop_ctrl;
	unsigned int reserved_09[11];
	
	// write port
	unsigned int		out_base_addr;
	GFX_REG_WR_STRIDE	out_stride;
	GFX_REG_WR_PFORMAT	out_format;
	GFX_REG_WR_SIZE		out_size;
	GFX_REG_WR_GALPHA	out_galpha;
	GFX_REG_WR_CTRL		out_ctrl;
	unsigned int		out_coc_ctrl;
	GFX_REG_WR_CSC_COEF	out_coef0;
	GFX_REG_WR_CSC_COEF	out_coef1;
	GFX_REG_WR_CSC_COEF	out_coef2;
	GFX_REG_WR_CSC_COEF	out_coef3;
	GFX_REG_WR_CSC_COEF	out_coef4;
	GFX_REG_WR_CSC_COEF	out_coef5;
	GFX_REG_WR_CSC_COEF	out_coef6;
	GFX_REG_WR_CSC_COEF	out_coef7;
	
	unsigned int reserved_10[1];
} GFX_REG_MAP_T;

irqreturn_t GFX_irq_handler(int irq, void *dev_id, struct pt_regs *regs);

