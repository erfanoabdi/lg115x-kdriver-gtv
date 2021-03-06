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

#ifndef _SDEC_REG_L9A0_H_
#define _SDEC_REG_L9A0_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "sdec_reg.h"

/*-----------------------------------------------------------------------------
	0x0000 vid_out_sel ''
------------------------------------------------------------------------------*/
typedef struct {
	VID_OUT_SEL                     	vid_out_sel                     ;	// 0x0000 : ''
	DL_SEL                          	dl_sel                          ;	// 0x0004 : ''
/* added at L9 A0
 * Start */
	UINT32                          	                 __rsvd_04[   2];	// 0x0008
	UINT32                          	addr_sw0                        ;	// 0x0010 : ''
	UINT32                          	addr_sw1                        ;	// 0x0014 : ''
	UINT32                          	addr_sw2                        ;	// 0x0018 : ''
/* added at L9 A0
 * End */
} SDTOP_REG_A0_T;
/* 2 regs, 2 types */

typedef struct {
	CDIP                          	cdip[ 4]                        ;	// 0x0000 : ''
	                                	                                 	// ~
	                                	                                 	// 0x000c : ''
	CDIOP                           	cdiop[ 2]                       ;	// 0x0010 : ''
	                                	                                 	// ~
	                                	                                 	// 0x0014 : ''
	CDIN_PARALLEL_SEL               	cdin_parallel_sel               ;	// 0x0018 : ''
	UINT32                          	                 __rsvd_00[   1];	// 0x001c
	CDIC                            	cdic[ 2]                        ;	// 0x0020 : ''
	                                	                                 	// ~
	                                	                                 	// 0x0024 : ''
	CDIC_DSC                        	cdic_dsc[ 2]                    ;	// 0x0028 : ''
	                                	                                 	// ~
	                                	                                 	// 0x002c : ''
	CDOC                            	cdoc[ 2]                        ;	// 0x0030 : ''	                                	                                 	// ~
	                                	                                 	// 0x0034 : ''
/* added at L8 B0
 * Start */
    CDIP3_2ND							cdip4_2nd;                          // 0x0038
    CDIP4_2ND							cdip3_2nd;							// 0x003C
    BDRC0_3								bdrc0;								// 0x0040
    BDRC_LEV0_3							bdrc_lev0;							// 0x0044
    BDRC0_3								bdrc1;								// 0x0048
    BDRC_LEV0_3							bdrc_lev1;							// 0x004C
    BDRC0_3								bdrc2;								// 0x0050
    BDRC_LEV0_3							bdrc_lev2;							// 0x0054
    BDRC0_3								bdrc3;								// 0x0058
	BDRC_LEV0_3							bdrc_lev3;							// 0x005C
	UINT32                          	                 __rsvd_01[   4];	// 0x0060 ~ 0x006c
/* added at L8 B0
 * End */
	MRQ_PRI                         	mrq_pri                         ;	// 0x0070 : ''
	UINT32                          	                 __rsvd_02[   3];	// 0x0074 ~ 0x007c
	//STCC                            	stcc[ 2]                        ;	// 0x0080 : ''
	UINT32                            	stcc[ 8]                        ;	// 0x0080 : ''
	                                	                                 	// ~
	                                	                                 	// 0x009c : ''
	SUB_STCC_RATE                   	sub_stcc_rate                   ;	// 0x00a0 : ''
	UINT32                          	                 __rsvd_03[   1];	// 0x00a4
	STCC_ERR_CTRL                   	stcc_err_ctrl[ 2]               ;	// 0x00a8 : ''
	                                	                                 	// ~
	                                	                                 	// 0x00ac : ''
	STCC_ASG                        	stcc_asg                        ;	// 0x00b0 : ''
	BYTE_CNT                        	byte_cnt[ 2]                    ;	// 0x00b4 : ''
	                                	                                 	// ~
	                                	                                 	// 0x00b8 : ''
	UINT32                          	                 __rsvd_04[   1];	// 0x00bc
	STCC_G                          	stcc_g                          ;	// 0x00c0 ~ 0x00c4
	UINT32                          	                 __rsvd_05[  14];	// 0x00c8 ~ 0x00fc
	INTR_EN                         	intr_en                         ;	// 0x0100 : ''
	UINT32                       		intr_stat                       ;	// 0x0104 : ''
	INTR_RSTAT                      	intr_rstat                      ;	// 0x0108 : ''
	UINT32                          	                 __rsvd_06[   1];	// 0x010c
	ERR_INTR_EN                     	err_intr_en                     ;	// 0x0110 : ''
	ERR_INTR_STAT                   	err_intr_stat                   ;	// 0x0114 : ''
	TEST                            	test                            ;	// 0x0118 : ''
	VERSION_                         	version                         ;	// 0x011c : ''
	GPB_BASE_ADDR                   	gpb_base_addr                   ;	// 0x0120 : ''
/* added at L8 B0
 * Start */
	CDIP0_2ND							cdip0_2nd;						;	//124h
   	CDIC								cdic2;							;	//128h
   	CDIC2_TS2PES						cdic2_ts2pes;					;	//12Ch
/* added at L8 B0
 * End */
} SDIO_REG_A0_T;
/* 37 regs, 21 types */

typedef struct {
	CONF                            	conf                            ;	// 0x0000 : ''
	UINT32                          	                 __rsvd_07[   1];	// 0x0004
	SPLICE_CNTDN                    	splice_cntdn                    ;	// 0x0008 : ''
	UINT32                          	                 __rsvd_08[   1];	// 0x000c
	EXT_CONF                        	ext_conf                        ;	// 0x0010 : ''
	ALL_PASS_CONF                   	all_pass_conf                   ;	// 0x0014 : ''
	CHAN_STAT                       	chan_stat                       ;	// 0x0018 : ''
	UINT32                          	                 __rsvd_09[   1];	// 0x001c
	CC_CHK_EN                     	cc_chk_en[2]                     ;	// 0x0020 : ''
//	H                               	h                               ;	// 0x0024 : ''
	SEC_CRC_EN                    	sec_crc_en[2]                    ;	// 0x0028 : ''
//	H                               	h                               ;	// 0x002c : ''
	SEC_CHKSUM_EN                 	sec_chksum_en[2]                 ;	// 0x0030 : ''
//	H                               	h                               ;	// 0x0034 : ''
	PES_CRC_EN                    	pes_crc_en[2]                    ;	// 0x0038 : ''
//	H                               	h                               ;	// 0x003c : ''
	TPI_ICONF                       	tpi_iconf                       ;	// 0x0040 : ''
	TPI_ISTAT                       	tpi_istat                       ;	// 0x0044 : ''
	UINT32                          	                 __rsvd_10[   1];	// 0x0048
	SE_ISTAT                        	se_istat                        ;	// 0x004c : ''
	GPB_F_ICONF                   	gpb_f_iconf[2]                   ;	// 0x0050 : ''
//	H                               	h                               ;	// 0x0054 : ''
	GPB_F_ISTAT                   	gpb_f_istat[2]                   ;	// 0x0058 : ''
//	H                               	h                               ;	// 0x005c : ''
	GPB_D_ISTAT                   	gpb_d_istat[2]                   ;	// 0x0060 : ''
//	H                               	h                               ;	// 0x0064 : ''
	UINT32                          	                 __rsvd_11[   2];	// 0x0068 ~ 0x006c
	TPDB_EN                       	tpdb_en[2]                      ;	// 0x0070 : ''
//	H                               	h                               ;	// 0x0074 : ''
	TPDB_ID                         	tpdb_id                         ;	// 0x0078 : ''
	V_SEQ_ERR_CODE                  	v_seq_err_code                  ;	// 0x007c : ''
	AFEDB_EN                      	afedb_en[2]                      ;	// 0x0080 : ''
//	H                               	h                               ;	// 0x0084 : ''
	AFEDB_ID                        	afedb_id                        ;	// 0x0088 : ''
	PIDF_MAP                        	pidf_map                        ;	// 0x008c : ''
	PIDF_ADDR                       	pidf_addr                       ;	// 0x0090 : ''
	PIDF_DATA                       	pidf_data                       ;	// 0x0094 : ''
	KMEM_ADDR                       	kmem_addr                       ;	// 0x0098 : ''
	KMEM_DATA                       	kmem_data                       ;	// 0x009c : ''
	SECF_ADDR                       	secf_addr                       ;	// 0x00a0 : ''
	SECF_DATA                       	secf_data                       ;	// 0x00a4 : ''
//	UINT32                       	       secf_data [1]                      ;	// 0x00a4 : ''
	UINT32                          	                 __rsvd_12[   2];	// 0x00a8 ~ 0x00ac
	SECF_EN                       	secf_en[2]                       ;	// 0x00b0 : ''
//	H                               	h                               ;	// 0x00b4 : ''
	SECF_MTYPE                    	secf_mtype[2]                   ;	// 0x00b8 : ''
//	H                               	h                               ;	// 0x00bc : ''
	SECFB_VALID                   	secfb_valid[2]                   ;	// 0x00c0 : ''
//	H                               	h                               ;	// 0x00c4 : ''
	UINT32                          	                 __rsvd_13[  14];	// 0x00c8 ~ 0x00fc
	GPB_BND                         	gpb_bnd[64]                     ;	// 0x0100 : ''
	                                	                                 	// ~
	                                	                                 	// 0x01fc : ''
	GPB_W_PTR                       	gpb_w_ptr[64]                   ;	// 0x0200 : ''
	                                	                                 	// ~
	                                	                                 	// 0x02fc : ''
	GPB_R_PTR                       	gpb_r_ptr[64]                   ;	// 0x0300 : ''
	                                	                                 	// ~
	                                	                                 	// 0x03fc : ''
	SECF_MAP                        	secf_map[128]                   ;	// 0x0400 : ''
	                                	                                 	// ~
	                                	                                 	// 0x05fc : ''
} MPG_REG_A0_T;
/* 362 regs, 46 types */
/* 401 regs, 69 types in Total*/

#ifdef __cplusplus
}
#endif

#endif	/* _SDEC_REG_L9A0_H_ */

/* from 'LG1150-TE-MAN-01 v1.2 (SDEC register description-20100303).csv' 20100311 00:54:20   대한민국 표준시 by getregs v2.3 */
