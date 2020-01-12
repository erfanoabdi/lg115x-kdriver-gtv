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
	0x0600 mif0_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	block_req_r_mvi0_y              : 1,	//     0
	block_req_r_mvi0_c              : 1,	//     1
	block_req_r_ipc0_y              : 1,	//     2
	block_req_r_ipc_c               : 1,	//     3
	block_req_r_ipc1_y              : 1,	//     4
	block_req_r_tnr1_y              : 1,	//     5
	block_req_r_capa_y              : 1,	//     6
	block_req_r_capa_c              : 1,	//     7
	block_req_r_tnr_y               : 1,	//     8
	block_req_r_tnr_c               : 1,	//     9
	block_req_r_3df_y               : 1,	//    10
	block_req_r_3df_c               : 1,	//    11
	                                : 4,	// 12:15 reserved
	disable_mif0                    : 1,	//    16
	load_req_stv_v                  : 1,	//    17
	load_req_pri_v                  : 1,	//    18
	load_cmd_saddr_v                : 1,	//    19
	load_cmd_pend_v                 : 1,	//    20
	load_cmd_pri_v                  : 1,	//    21
	load_cmd_stride_v               : 1,	//    22
	enable_stv_cnt                  : 1;	//    23
} MIF0_CTRL0;

/*-----------------------------------------------------------------------------
	0x0604 mif0_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	set_10b_req_wd_capa_y           : 1,	//     0
	set_10b_req_wd_capa_c           : 1,	//     1
	set_10b_req_rd_capa_y           : 1,	//     2
	set_10b_req_rd_capa_c           : 1,	//     3
	set_10b_req_wd_tnr_y            : 1,	//     4
	set_10b_req_wd_tnr_c            : 1,	//     5
	set_10b_req_rd_tnr_y            : 1,	//     6
	set_10b_req_rd_tnr_c            : 1,	//     7
	set_10b_req_rd_ipc_y            : 1,	//     8
	set_10b_req_rd_ipc_c            : 1,	//     9
	set_10b_req_wd_scm_y            : 1,	//    10
	set_10b_req_wd_scm_c            : 1,	//    11
	                                : 4,	// 12:15 reserved
	scm_mrb_bank_mode               : 3,	// 16:18
	scm_mwb_bank_mode               : 3;	// 19:21
} MIF0_CTRL1;

/*-----------------------------------------------------------------------------
	0x0608 mif0_cmd_pri0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_v_mvi0_y             : 4,	//  0: 3
	cmd_rd_pri_v_mvi0_c             : 4,	//  4: 7
	cmd_rd_pri_v_ipc0_y             : 4,	//  8:11
	cmd_rd_pri_v_ipc_c              : 4,	// 12:15
	cmd_rd_pri_v_ipc1_y             : 4,	// 16:19
	cmd_rd_pri_v_capa_y             : 4,	// 20:23
	cmd_rd_pri_v_capa_c             : 4,	// 24:27
	cmd_rd_pri_v_tnr_y              : 4;	// 28:31
} MIF0_CMD_PRI0;

/*-----------------------------------------------------------------------------
	0x060c mif0_cmd_pri1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_v_tnr_c              : 4,	//  0: 3
	cmd_rd_pri_v_3df_y              : 4,	//  4: 7
	cmd_rd_pri_v_3df_c              : 4,	//  8:11
	cmd_rd_pri_v_tnr1_y             : 4;	// 12:15
} MIF0_CMD_PRI1;

/*-----------------------------------------------------------------------------
	0x0610 mif0_req_pri0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_pri_v_mvi0_y             : 4,	//  0: 3
	req_rd_pri_v_mvi0_c             : 4,	//  4: 7
	req_rd_pri_v_ipc0_y             : 4,	//  8:11
	req_rd_pri_v_ipc_c              : 4,	// 12:15
	req_rd_pri_v_ipc1_y             : 4,	// 16:19
	req_rd_pri_v_capa_y             : 4,	// 20:23
	req_rd_pri_v_capa_c             : 4,	// 24:27
	req_rd_pri_v_tnr_y              : 4;	// 28:31
} MIF0_REQ_PRI0;

/*-----------------------------------------------------------------------------
	0x0614 mif0_req_pri1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_pri_v_tnr_c              : 4,	//  0: 3
	req_rd_pri_v_3df_y              : 4,	//  4: 7
	req_rd_pri_v_3df_c              : 4,	//  8:11
	req_rd_pri_v_tnr1_y             : 4;	// 12:15
} MIF0_REQ_PRI1;

/*-----------------------------------------------------------------------------
	0x0618 mif0_req_stv0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_stv_v_mvi0_y             : 4,	//  0: 3
	req_rd_stv_v_mvi0_c             : 4,	//  4: 7
	req_rd_stv_v_ipc0_y             : 4,	//  8:11
	req_rd_stv_v_ipc_c              : 4,	// 12:15
	req_rd_stv_v_ipc1_y             : 4,	// 16:19
	req_rd_stv_v_capa_y             : 4,	// 20:23
	req_rd_stv_v_capa_c             : 4,	// 24:27
	req_rd_stv_v_tnr_y              : 4;	// 28:31
} MIF0_REQ_STV0;

/*-----------------------------------------------------------------------------
	0x061c mif0_req_stv1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_stv_v_tnr_c              : 4,	//  0: 3
	req_rd_stv_v_3df_y              : 4,	//  4: 7
	req_rd_stv_v_3df_c              : 4,	//  8:11
	req_rd_stv_v_tnr1_y             : 4;	// 12:15
} MIF0_REQ_STV1;

/*-----------------------------------------------------------------------------
	0x0620 mif0_cmd_pend0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pend_v_mvi0_y            : 4,	//  0: 3
	cmd_rd_pend_v_mvi0_c            : 4;	//  4: 7
} MIF0_CMD_PEND0;

/*-----------------------------------------------------------------------------
	0x0624 mif0_cmd_pend1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	                                : 4,	//  0: 3 reserved
	cmd_rd_pend_v_tnr1_y            : 4,	//  4: 7
	cmd_rd_pend_v_ipc0_y            : 4,	//  8:11
	cmd_rd_pend_v_ipc_c             : 4,	// 12:15
	cmd_rd_pend_v_ipc1_y            : 4,	// 16:19
	cmd_rd_pend_v_capa_y            : 4,	// 20:23
	cmd_rd_pend_v_capa_c            : 4,	// 24:27
	cmd_rd_pend_v_tnr_y             : 4;	// 28:31
} MIF0_CMD_PEND1;

/*-----------------------------------------------------------------------------
	0x0628 mif0_cmd_pend2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pend_v_tnr_c             : 4,	//  0: 3
	cmd_rd_pend_v_3df_y             : 4,	//  4: 7
	cmd_rd_pend_v_3df_c             : 4;	//  8:11
} MIF0_CMD_PEND2;

/*-----------------------------------------------------------------------------
	0x062c mif0_cmd_stride0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_stride_v_mvi0_y          : 4,	//  0: 3
	cmd_rd_stride_v_mvi0_c          : 4,	//  4: 7
	cmd_rd_stride_v_8_tnr1_y        : 4,	//  8:11
	cmd_rd_stride_v_2_tnr1_y        : 4,	// 12:15
	cmd_rd_stride_v_8_ipc0_y        : 4,	// 16:19
	cmd_rd_stride_v_2_ipc0_y        : 4,	// 20:23
	cmd_rd_stride_v_8_ipc_c         : 4,	// 24:27
	cmd_rd_stride_v_2_ipc_c         : 4;	// 28:31
} MIF0_CMD_STRIDE0;

/*-----------------------------------------------------------------------------
	0x0630 mif0_cmd_stride1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_stride_v_8_ipc1_y        : 4,	//  0: 3
	cmd_rd_stride_v_2_ipc1_y        : 4,	//  4: 7
	cmd_rd_stride_v_8_capa_y        : 4,	//  8:11
	cmd_rd_stride_v_2_capa_y        : 4,	// 12:15
	cmd_rd_stride_v_8_capa_c        : 4,	// 16:19
	cmd_rd_stride_v_2_capa_c        : 4,	// 20:23
	cmd_rd_stride_v_8_tnr_y         : 4,	// 24:27
	cmd_rd_stride_v_2_tnr_y         : 4;	// 28:31
} MIF0_CMD_STRIDE1;

/*-----------------------------------------------------------------------------
	0x0634 mif0_cmd_stride2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_stride_v_8_tnr_c         : 4,	//  0: 3
	cmd_rd_stride_v_2_tnr_c         : 4,	//  4: 7
	cmd_rd_stride_v_8_3df_y         : 4,	//  8:11
	cmd_rd_stride_v_2_3df_y         : 4,	// 12:15
	cmd_rd_stride_v_8_3df_c         : 4,	// 16:19
	cmd_rd_stride_v_2_3df_c         : 4;	// 20:23
} MIF0_CMD_STRIDE2;

/*-----------------------------------------------------------------------------
	0x0638 mif0_cmd_stride3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	man_cmd_rd_stride_v_mvi0_y      : 1,	//     0
	man_cmd_rd_stride_v_mvi0_c      : 1,	//     1
	man_cmd_rd_stride_v_ipc0_y      : 1,	//     2
	man_cmd_rd_stride_v_ipc_c       : 1,	//     3
	man_cmd_rd_stride_v_ipc1_y      : 1,	//     4
	man_cmd_rd_stride_v_tnr1_y      : 1,	//     5
	man_cmd_rd_stride_v_capa_y      : 1,	//     6
	man_cmd_rd_stride_v_capa_c      : 1,	//     7
	man_cmd_rd_stride_v_tnr_y       : 1,	//     8
	man_cmd_rd_stride_v_tnr_c       : 1,	//     9
	man_cmd_rd_stride_v_3df_y       : 1,	//    10
	man_cmd_rd_stride_v_3df_c       : 1;	//    11
} MIF0_CMD_STRIDE3;

/*-----------------------------------------------------------------------------
	0x063c saddr_rd_mvi0_yc ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_mvi0_y             :16,	//  0:15
	req_rd_saddr_mvi0_c             :16;	// 16:31
} SADDR_RD_MVI0_YC;

/*-----------------------------------------------------------------------------
	0x0640 saddr_rd_ipc0_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_ipc0_y           :16,	//  0:15
	req_rd_saddr_8_ipc0_y           :16;	// 16:31
} SADDR_RD_IPC0_Y;

/*-----------------------------------------------------------------------------
	0x0644 saddr_rd_ipc_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_ipc_c            :16,	//  0:15
	req_rd_saddr_8_ipc_c            :16;	// 16:31
} SADDR_RD_IPC_C;

/*-----------------------------------------------------------------------------
	0x0648 saddr_rd_ipc1_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_ipc1_y           :16,	//  0:15
	req_rd_saddr_8_ipc1_y           :16;	// 16:31
} SADDR_RD_IPC1_Y;

/*-----------------------------------------------------------------------------
	0x064c saddr_rd_tnr1_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_tnr1_y           :16,	//  0:15
	req_rd_saddr_8_tnr1_y           :16;	// 16:31
} SADDR_RD_TNR1_Y;

/*-----------------------------------------------------------------------------
	0x0650 saddr_rd_capa_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_capa_y           :16,	//  0:15
	req_rd_saddr_8_capa_y           :16;	// 16:31
} SADDR_RD_CAPA_Y;

/*-----------------------------------------------------------------------------
	0x0654 saddr_rd_capa_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_capa_c           :16,	//  0:15
	req_rd_saddr_8_capa_c           :16;	// 16:31
} SADDR_RD_CAPA_C;

/*-----------------------------------------------------------------------------
	0x0658 saddr_rd_tnr_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_tnr_y            :16,	//  0:15
	req_rd_saddr_8_tnr_y            :16;	// 16:31
} SADDR_RD_TNR_Y;

/*-----------------------------------------------------------------------------
	0x065c saddr_rd_tnr_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_tnr_c            :16,	//  0:15
	req_rd_saddr_8_tnr_c            :16;	// 16:31
} SADDR_RD_TNR_C;

/*-----------------------------------------------------------------------------
	0x0660 saddr_rd_3df_yc ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_3df_y              :16,	//  0:15
	req_rd_saddr_3df_c              :16;	// 16:31
} SADDR_RD_3DF_YC;

/*-----------------------------------------------------------------------------
	0x0664 saddr_rd_dl_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_dl_y             :16,	//  0:15
	req_rd_saddr_8_dl_y             :16;	// 16:31
} SADDR_RD_DL_Y;

/*-----------------------------------------------------------------------------
	0x0668 saddr_rd_dl_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_dl_c             :16,	//  0:15
	req_rd_saddr_8_dl_c             :16;	// 16:31
} SADDR_RD_DL_C;

/*-----------------------------------------------------------------------------
	0x066c saddr_rd_dr_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_dr_y             :16,	//  0:15
	req_rd_saddr_8_dr_y             :16;	// 16:31
} SADDR_RD_DR_Y;

/*-----------------------------------------------------------------------------
	0x0670 saddr_rd_dr_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_dr_c             :16,	//  0:15
	req_rd_saddr_8_dr_c             :16;	// 16:31
} SADDR_RD_DR_C;

/*-----------------------------------------------------------------------------
	0x0674 saddr_ext_0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_mvi0_y_msb4b       : 4,	//  0: 3
	req_rd_saddr_mvi0_c_msb4b       : 4,	//  4: 7
	req_rd_saddr_8_ipc0_y_msb4b     : 4,	//  8:11
	req_rd_saddr_2_ipc0_y_msb4b     : 4,	// 12:15
	req_rd_saddr_8_ipc_c_msb4b      : 4,	// 16:19
	req_rd_saddr_2_ipc_c_msb4b      : 4,	// 20:23
	req_rd_saddr_8_ipc1_y_msb4b     : 4,	// 24:27
	req_rd_saddr_2_ipc1_y_msb4b     : 4;	// 28:31
} SADDR_EXT_0;

/*-----------------------------------------------------------------------------
	0x0678 saddr_ext_1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_8_capa_y_msb4b     : 4,	//  0: 3
	req_rd_saddr_2_capa_y_msb4b     : 4,	//  4: 7
	req_rd_saddr_8_capa_c_msb4b     : 4,	//  8:11
	req_rd_saddr_2_capa_c_msb4b     : 4,	// 12:15
	req_rd_saddr_8_tnr_y_msb4b      : 4,	// 16:19
	req_rd_saddr_2_tnr_y_msb4b      : 4,	// 20:23
	req_rd_saddr_8_tnr_c_msb4b      : 4,	// 24:27
	req_rd_saddr_2_tnr_c_msb4b      : 4;	// 28:31
} SADDR_EXT_1;

/*-----------------------------------------------------------------------------
	0x067c saddr_ext_2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_3df_y_msb4b        : 4,	//  0: 3
	req_rd_saddr_3df_c_msb4b        : 4,	//  4: 7
	req_rd_saddr_8_tnr1_y_msb4b     : 4,	//  8:11
	req_rd_saddr_2_tnr1_y_msb4b     : 4;	// 12:15
} SADDR_EXT_2;

/*-----------------------------------------------------------------------------
	0x0680 saddr_ext_3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_8_dl_y_msb4b       : 4,	//  0: 3
	req_rd_saddr_2_dl_y_msb4b       : 4,	//  4: 7
	req_rd_saddr_8_dl_c_msb4b       : 4,	//  8:11
	req_rd_saddr_2_dl_c_msb4b       : 4,	// 12:15
	req_rd_saddr_8_dr_y_msb4b       : 4,	// 16:19
	req_rd_saddr_2_dr_y_msb4b       : 4,	// 20:23
	req_rd_saddr_8_dr_c_msb4b       : 4,	// 24:27
	req_rd_saddr_2_dr_c_msb4b       : 4;	// 28:31
} SADDR_EXT_3;

/*-----------------------------------------------------------------------------
	0x0684 mif0_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fr_rd_valid_mvi0_y              : 1,	//     0
	fr_rd_valid_mvi0_c              : 1,	//     1
	                                : 8,	//  2: 9 reserved
	fr_rd_valid_ipc0_y              : 1,	//    10
	fr_rd_valid_ipc_c               : 1,	//    11
	fr_rd_valid_ipc1_y              : 1,	//    12
	fr_rd_valid_capa_y              : 1,	//    13
	fr_rd_valid_capa_c              : 1,	//    14
	fr_rd_valid_tnr_y               : 1,	//    15
	fr_rd_valid_tnr_c               : 1,	//    16
	fr_wd_q_full_capa_y             : 1,	//    17
	fr_wd_q_full_capa_c             : 1,	//    18
	fr_wd_q_full_tnr_y              : 1,	//    19
	fr_wd_q_full_tnr_c              : 1,	//    20
	fr_wd_q_full_scm_y              : 1,	//    21
	fr_wd_q_full_scm_c              : 1,	//    22
	fr_rd_valid_3df_y               : 1,	//    23
	fr_rd_valid_3df_c               : 1;	//    24
} MIF0_STATUS;

/*-----------------------------------------------------------------------------
	0x0688 saddr_param_update0_m ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	load                            : 1,	//     0
	                                :10,	//  1:10 reserved
	register_update_address0_m      :21;	// 11:31
} SADDR_PARAM_UPDATE0_M;

/*-----------------------------------------------------------------------------
	0x068c saddr_param_update0_s0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	load                            : 1,	//     0
	                                :10,	//  1:10 reserved
	register_update_address0_s0     :21;	// 11:31
} SADDR_PARAM_UPDATE0_S0;

/*-----------------------------------------------------------------------------
	0x0690 saddr_param_update0_s1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	load                            : 1,	//     0
	                                :10,	//  1:10 reserved
	register_update_address0_s1     :21;	// 11:31
} SADDR_PARAM_UPDATE0_S1;

/*-----------------------------------------------------------------------------
	0x0694 mif0_bank_addr_0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_bank_addr_8_mvi_y        : 4,	//  0: 3
	req_rd_bank_addr_8_mvi_c        : 4,	//  4: 7
	req_rd_bank_addr_8_ipc0_y       : 4,	//  8:11
	req_rd_bank_addr_8_ipc_c        : 4,	// 12:15
	req_rd_bank_addr_8_ipc1_y       : 4,	// 16:19
	req_rd_bank_addr_8_tnr_1_y      : 4,	// 20:23
	req_rd_bank_addr_8_capa_y       : 4,	// 24:27
	req_rd_bank_addr_8_capa_c       : 4;	// 28:31
} MIF0_BANK_ADDR_0;

/*-----------------------------------------------------------------------------
	0x0698 mif0_bank_addr_1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_bank_addr_8_tnr0_y       : 4,	//  0: 3
	req_rd_bank_addr_8_tnr_c        : 4,	//  4: 7
	req_rd_bank_addr_8_3df_y        : 4,	//  8:11
	req_rd_bank_addr_8_3df_c        : 4;	// 12:15
} MIF0_BANK_ADDR_1;

/*-----------------------------------------------------------------------------
	0x069c mif0_bank_addr_2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_bank_addr_2_mvi_y        : 4,	//  0: 3
	req_rd_bank_addr_2_mvi_c        : 4,	//  4: 7
	req_rd_bank_addr_2_ipc0_y       : 4,	//  8:11
	req_rd_bank_addr_2_ipc_c        : 4,	// 12:15
	req_rd_bank_addr_2_ipc1_y       : 4,	// 16:19
	req_rd_bank_addr_2_tnr_1_y      : 4,	// 20:23
	req_rd_bank_addr_2_capa_y       : 4,	// 24:27
	req_rd_bank_addr_2_capa_c       : 4;	// 28:31
} MIF0_BANK_ADDR_2;

/*-----------------------------------------------------------------------------
	0x06a0 mif0_bank_addr_3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_bank_addr_2_tnr0_y       : 4,	//  0: 3
	req_rd_bank_addr_2_tnr_c        : 4,	//  4: 7
	req_rd_bank_addr_2_3df_y        : 4,	//  8:11
	req_rd_bank_addr_2_3df_c        : 4;	// 12:15
} MIF0_BANK_ADDR_3;

/*-----------------------------------------------------------------------------
	0x06a4 mif0_lsb_switch ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_lsb_ipc0_y               : 2,	//  0: 1
	req_rd_lsb_ipc_c                : 2,	//  2: 3
	req_rd_lsb_ipc1_y               : 2,	//  4: 5
	req_rd_lsb_tnr1_y               : 2,	//  6: 7
	req_rd_lsb_capa_y               : 2,	//  8: 9
	req_rd_lsb_capa_c               : 2,	// 10:11
	req_rd_lsb_tnr0_y               : 2,	// 12:13
	req_rd_lsb_tnr_c                : 2;	// 14:15
} MIF0_LSB_SWITCH;

/*-----------------------------------------------------------------------------
	0x06a8 mif0_req_pend0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_pend_v_mvi0_y            : 8,	//  0: 7
	req_rd_pend_v_mvi0_c            : 8,	//  8:15
	req_rd_pend_v_ipc0_y            : 8,	// 16:23
	req_rd_pend_v_ipc_c             : 8;	// 24:31
} MIF0_REQ_PEND0;

/*-----------------------------------------------------------------------------
	0x06ac mif0_req_pend1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_pend_v_ipc1_y            : 8,	//  0: 7
	req_rd_pend_v_tnr1_y            : 8,	//  8:15
	req_rd_pend_v_capa_y            : 8,	// 16:23
	req_rd_pend_v_capa_c            : 8;	// 24:31
} MIF0_REQ_PEND1;

/*-----------------------------------------------------------------------------
	0x06b0 mif0_req_pend2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_pend_v_tnr0_y            : 8,	//  0: 7
	req_rd_pend_v_tnr_c             : 8,	//  8:15
	req_rd_pend_v_3df_y             : 8,	// 16:23
	req_rd_pend_v_3df_c             : 8;	// 24:31
} MIF0_REQ_PEND2;

/*-----------------------------------------------------------------------------
	0x06b4 mif0_req_pend3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_pend_v_tnr_y             : 8,	//  0: 7
	req_wd_pend_v_tnr_c             : 8,	//  8:15
	req_wd_pend_v_scm_y             : 8,	// 16:23
	req_wd_pend_v_scm_c             : 8;	// 24:31
} MIF0_REQ_PEND3;

/*-----------------------------------------------------------------------------
	0x06b8 mif0_req_pend4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_pend_v_capa_y            : 8,	//  0: 7
	req_wd_pend_v_capa_c            : 8;	//  8:15
} MIF0_REQ_PEND4;

/*-----------------------------------------------------------------------------
	0x06bc mif0_cmd_pri0_s ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_s_v_mvi0_y           : 4,	//  0: 3
	cmd_rd_pri_s_v_mvi0_c           : 4,	//  4: 7
	cmd_rd_pri_s_v_ipc0_y           : 4,	//  8:11
	cmd_rd_pri_s_v_ipc_c            : 4,	// 12:15
	cmd_rd_pri_s_v_ipc1_y           : 4,	// 16:19
	cmd_rd_pri_s_v_capa_y           : 4,	// 20:23
	cmd_rd_pri_s_v_capa_c           : 4,	// 24:27
	cmd_rd_pri_s_v_tnr_y            : 4;	// 28:31
} MIF0_CMD_PRI0_S;

/*-----------------------------------------------------------------------------
	0x06c0 mif0_cmd_pri1_s ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_s_v_tnr_c            : 4,	//  0: 3
	cmd_rd_pri_s_v_3df_y            : 4,	//  4: 7
	cmd_rd_pri_s_v_3df_c            : 4,	//  8:11
	cmd_rd_pri_s_v_tnr1_y           : 4;	// 12:15
} MIF0_CMD_PRI1_S;

/*-----------------------------------------------------------------------------
	0x06c4 mif0_reset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	reset_mif_r_mvi0_y              : 1,	//     0
	reset_mif_r_mvi0_c              : 1,	//     1
	reset_mif_r_ipc0_y              : 1,	//     2
	reset_mif_r_ipc_c               : 1,	//     3
	reset_mif_r_ipc1_y              : 1,	//     4
	reset_mif_r_tnr1_y              : 1,	//     5
	reset_mif_r_capa_y              : 1,	//     6
	reset_mif_r_capa_c              : 1,	//     7
	reset_mif_r_tnr_y               : 1,	//     8
	reset_mif_r_tnr_c               : 1,	//     9
	reset_mif_r_3df_y               : 1,	//    10
	reset_mif_r_3df_c               : 1,	//    11
	reset_mif_w_capa_y              : 1,	//    12
	reset_mif_w_capa_c              : 1,	//    13
	reset_mif_w_tnr_y               : 1,	//    14
	reset_mif_w_tnr_c               : 1,	//    15
	reset_mif_w_scm_y               : 1,	//    16
	reset_mif_w_scm_c               : 1,	//    17
	                                : 2,	// 18:19 reserved
	reset_time_out_cnt              :12;	// 20:31
} MIF0_RESET;

/*-----------------------------------------------------------------------------
	0x06c8 saddr_param_update0_m1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	load                            : 1,	//     0
	                                :10,	//  1:10 reserved
	register_update_address0_m1     :21;	// 11:31
} SADDR_PARAM_UPDATE0_M1;

/*-----------------------------------------------------------------------------
	0x06cc de_zebu_dbg0 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x06d0 de_zebu_dbg1 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x06d4 de_zebu_dbg2 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x06d8 de_zebu_dbg3 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0a00 mif1_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	block_req_w_scs_y               : 1,	//     0
	block_req_w_scs_c               : 1,	//     1
	block_req_w_capb_y              : 1,	//     2
	block_req_w_capb_c              : 1,	//     3
	block_req_w_tnr_y               : 1,	//     4
	block_req_w_tnr_c               : 1,	//     5
	block_req_w_scm_y               : 1,	//     6
	block_req_w_scm_c               : 1,	//     7
	block_req_w_capa_y              : 1,	//     8
	block_req_w_capa_c              : 1,	//     9
	block_req_w_mdi_y               : 1,	//    10
	block_req_w_mdi_c               : 1,	//    11
	                                : 4,	// 12:15 reserved
	disable_mif1                    : 1,	//    16
	load_req_stv_v                  : 1,	//    17
	load_req_pri_v                  : 1,	//    18
	load_cmd_saddr_v                : 1,	//    19
	load_cmd_pend_v                 : 1,	//    20
	load_cmd_pri_v                  : 1,	//    21
	load_cmd_stride_v               : 1,	//    22
	enable_stv_cnt                  : 1,	//    23
	set_ecanvas                     : 1;	//    24
} MIF1_CTRL;

/*-----------------------------------------------------------------------------
	0x0a04 mif1_cmd_pri0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_pri_v_capa_y             : 4,	//  0: 3
	cmd_wd_pri_v_capa_c             : 4,	//  4: 7
	cmd_wd_pri_v_capb_y             : 4,	//  8:11
	cmd_wd_pri_v_capb_c             : 4,	// 12:15
	cmd_wd_pri_v_tnr_y              : 4,	// 16:19
	cmd_wd_pri_v_tnr_c              : 4,	// 20:23
	cmd_wd_pri_v_scm_y              : 4,	// 24:27
	cmd_wd_pri_v_scm_c              : 4;	// 28:31
} MIF1_CMD_PRI0;

/*-----------------------------------------------------------------------------
	0x0a08 mif1_cmd_pri1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_pri_v_scs_y              : 4,	//  0: 3
	cmd_wd_pri_v_scs_c              : 4,	//  4: 7
	cmd_wd_pri_v_mdi_y              : 4,	//  8:11
	cmd_wd_pri_v_mdi_c              : 4;	// 12:15
} MIF1_CMD_PRI1;

/*-----------------------------------------------------------------------------
	0x0a0c mif1_req_pri0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_pri_v_capa_y             : 4,	//  0: 3
	req_wd_pri_v_capa_c             : 4,	//  4: 7
	req_wd_pri_v_capb_y             : 4,	//  8:11
	req_wd_pri_v_capb_c             : 4,	// 12:15
	req_wd_pri_v_tnr_y              : 4,	// 16:19
	req_wd_pri_v_tnr_c              : 4,	// 20:23
	req_wd_pri_v_scm_y              : 4,	// 24:27
	req_wd_pri_v_scm_c              : 4;	// 28:31
} MIF1_REQ_PRI0;

/*-----------------------------------------------------------------------------
	0x0a10 mif1_req_pri1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_pri_v_scs_y              : 4,	//  0: 3
	req_wd_pri_v_scs_c              : 4,	//  4: 7
	req_wd_pri_v_mdi_y              : 4,	//  8:11
	req_wd_pri_v_mdi_c              : 4;	// 12:15
} MIF1_REQ_PRI1;

/*-----------------------------------------------------------------------------
	0x0a14 mif1_req_stv0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_stv_v_capa_y             : 4,	//  0: 3
	req_wd_stv_v_capa_c             : 4,	//  4: 7
	req_wd_stv_v_capb_y             : 4,	//  8:11
	req_wd_stv_v_capb_c             : 4,	// 12:15
	req_wd_stv_v_tnr_y              : 4,	// 16:19
	req_wd_stv_v_tnr_c              : 4,	// 20:23
	req_wd_stv_v_scm_y              : 4,	// 24:27
	req_wd_stv_v_scm_c              : 4;	// 28:31
} MIF1_REQ_STV0;

/*-----------------------------------------------------------------------------
	0x0a18 mif1_req_stv1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_stv_v_scs_y              : 4,	//  0: 3
	req_wd_stv_v_scs_c              : 4,	//  4: 7
	req_wd_stv_v_mdi_y              : 4,	//  8:11
	req_wd_stv_v_mdi_c              : 4;	// 12:15
} MIF1_REQ_STV1;

/*-----------------------------------------------------------------------------
	0x0a1c mif1_cmd_pend0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_pend_v_capa_y            : 4,	//  0: 3
	cmd_wd_pend_v_capa_c            : 4,	//  4: 7
	cmd_wd_pend_v_capb_y            : 4,	//  8:11
	cmd_wd_pend_v_capb_c            : 4,	// 12:15
	cmd_wd_pend_v_tnr_y             : 4,	// 16:19
	cmd_wd_pend_v_tnr_c             : 4,	// 20:23
	cmd_wd_pend_v_scm_y             : 4,	// 24:27
	cmd_wd_pend_v_scm_c             : 4;	// 28:31
} MIF1_CMD_PEND0;

/*-----------------------------------------------------------------------------
	0x0a20 mif1_cmd_pend1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_pend_v_scs_y             : 4,	//  0: 3
	cmd_wd_pend_v_scs_c             : 4,	//  4: 7
	cmd_wd_pend_v_mdi_y             : 4,	//  8:11
	cmd_wd_pend_v_mdi_c             : 4;	// 12:15
} MIF1_CMD_PEND1;

/*-----------------------------------------------------------------------------
	0x0a24 mif1_cmd_stride0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_stride_v_8_capa_y        : 4,	//  0: 3
	cmd_wd_stride_v_2_capa_y        : 4,	//  4: 7
	cmd_wd_stride_v_8_capa_c        : 4,	//  8:11
	cmd_wd_stride_v_2_capa_c        : 4,	// 12:15
	cmd_wd_stride_v_capb_y          : 4,	// 16:19
	cmd_wd_stride_v_capb_c          : 4,	// 20:23
	cmd_wd_stride_v_8_tnr_y         : 4,	// 24:27
	cmd_wd_stride_v_2_tnr_y         : 4;	// 28:31
} MIF1_CMD_STRIDE0;

/*-----------------------------------------------------------------------------
	0x0a28 mif1_cmd_stride1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_stride_v_8_tnr_c         : 4,	//  0: 3
	cmd_wd_stride_v_2_tnr_c         : 4,	//  4: 7
	cmd_wd_stride_v_8_scm_y         : 4,	//  8:11
	cmd_wd_stride_v_2_scm_y         : 4,	// 12:15
	cmd_wd_stride_v_8_scm_c         : 4,	// 16:19
	cmd_wd_stride_v_2_scm_c         : 4,	// 20:23
	cmd_wd_stride_v_scs_y           : 4,	// 24:27
	cmd_wd_stride_v_scs_c           : 4;	// 28:31
} MIF1_CMD_STRIDE1;

/*-----------------------------------------------------------------------------
	0x0a2c mif1_cmd_stride2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_stride_v_mdi_y           : 4,	//  0: 3
	cmd_wd_stride_v_mdi_c           : 4,	//  4: 7
	                                : 8,	//  8:15 reserved
	man_cmd_wd_stride_v_scs_y       : 1,	//    16
	man_cmd_wd_stride_v_scs_c       : 1,	//    17
	man_cmd_wd_stride_v_capb_y      : 1,	//    18
	man_cmd_wd_stride_v_capb_c      : 1,	//    19
	man_cmd_wd_stride_v_tnr_y       : 1,	//    20
	man_cmd_wd_stride_v_tnr_c       : 1,	//    21
	man_cmd_wd_stride_v_scm_y       : 1,	//    22
	man_cmd_wd_stride_v_scm_c       : 1,	//    23
	man_cmd_wd_stride_v_capa_y      : 1,	//    24
	man_cmd_wd_stride_v_capa_c      : 1,	//    25
	man_cmd_wd_stride_v_mdi_y       : 1,	//    26
	man_cmd_wd_stride_v_mdi_c       : 1;	//    27
} MIF1_CMD_STRIDE2;

/*-----------------------------------------------------------------------------
	0x0a30 saddr_wd_capa_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_capa_y           :16,	//  0:15
	req_wd_saddr_8_capa_y           :16;	// 16:31
} SADDR_WD_CAPA_Y;

/*-----------------------------------------------------------------------------
	0x0a34 saddr_wd_capa_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_capa_c           :16,	//  0:15
	req_wd_saddr_8_capa_c           :16;	// 16:31
} SADDR_WD_CAPA_C;

/*-----------------------------------------------------------------------------
	0x0a38 saddr_wd_tnr_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_tnr_y            :16,	//  0:15
	req_wd_saddr_8_tnr_y            :16;	// 16:31
} SADDR_WD_TNR_Y;

/*-----------------------------------------------------------------------------
	0x0a3c saddr_wd_tnr_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_tnr_c            :16,	//  0:15
	req_wd_saddr_8_tnr_c            :16;	// 16:31
} SADDR_WD_TNR_C;

/*-----------------------------------------------------------------------------
	0x0a40 saddr_wd_scm_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_scm_y            :16,	//  0:15
	req_wd_saddr_8_scm_y            :16;	// 16:31
} SADDR_WD_SCM_Y;

/*-----------------------------------------------------------------------------
	0x0a44 saddr_wd_scm_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_scm_c            :16,	//  0:15
	req_wd_saddr_8_scm_c            :16;	// 16:31
} SADDR_WD_SCM_C;

/*-----------------------------------------------------------------------------
	0x0a48 saddr_wd_capb_yc ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_capb_y             :16,	//  0:15
	req_wd_saddr_capb_c             :16;	// 16:31
} SADDR_WD_CAPB_YC;

/*-----------------------------------------------------------------------------
	0x0a4c saddr_wd_scs_yc ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_scs_y              :16,	//  0:15
	req_wd_saddr_scs_c              :16;	// 16:31
} SADDR_WD_SCS_YC;

/*-----------------------------------------------------------------------------
	0x0a50 saddr_wd_mdi_yc ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_mdi_y              :16,	//  0:15
	req_wd_saddr_mdi_c              :16;	// 16:31
} SADDR_WD_MDI_YC;

/*-----------------------------------------------------------------------------
	0x0a54 saddr_wd_dl_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_dl_y             :16,	//  0:15
	req_wd_saddr_8_dl_y             :16;	// 16:31
} SADDR_WD_DL_Y;

/*-----------------------------------------------------------------------------
	0x0a58 saddr_wd_dl_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_dl_c             :16,	//  0:15
	req_wd_saddr_8_dl_c             :16;	// 16:31
} SADDR_WD_DL_C;

/*-----------------------------------------------------------------------------
	0x0a5c saddr_wd_dr_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_dr_y             :16,	//  0:15
	req_wd_saddr_8_dr_y             :16;	// 16:31
} SADDR_WD_DR_Y;

/*-----------------------------------------------------------------------------
	0x0a60 saddr_wd_dr_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_2_dr_c             :16,	//  0:15
	req_wd_saddr_8_dr_c             :16;	// 16:31
} SADDR_WD_DR_C;

/*-----------------------------------------------------------------------------
	0x0a64 saddr_ext_4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_8_dl_y_msb4b       : 4,	//  0: 3
	req_wd_saddr_2_dl_y_msb4b       : 4,	//  4: 7
	req_wd_saddr_8_dl_c_msb4b       : 4,	//  8:11
	req_wd_saddr_2_dl_c_msb4b       : 4,	// 12:15
	req_wd_saddr_8_dr_y_msb4b       : 4,	// 16:19
	req_wd_saddr_2_dr_y_msb4b       : 4,	// 20:23
	req_wd_saddr_8_dr_c_msb4b       : 4,	// 24:27
	req_wd_saddr_2_dr_c_msb4b       : 4;	// 28:31
} SADDR_EXT_4;

/*-----------------------------------------------------------------------------
	0x0a68 saddr_ext_5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_8_scm_y_msb4b      : 4,	//  0: 3
	req_wd_saddr_2_scm_y_msb4b      : 4,	//  4: 7
	req_wd_saddr_8_scm_c_msb4b      : 4,	//  8:11
	req_wd_saddr_2_scm_c_msb4b      : 4,	// 12:15
	req_wd_saddr_8_tnr_y_msb4b      : 4,	// 16:19
	req_wd_saddr_2_tnr_y_msb4b      : 4,	// 20:23
	req_wd_saddr_8_tnr_c_msb4b      : 4,	// 24:27
	req_wd_saddr_2_tnr_c_msb4b      : 4;	// 28:31
} SADDR_EXT_5;

/*-----------------------------------------------------------------------------
	0x0a6c saddr_ext_6 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_8_capa_y_msb4b     : 4,	//  0: 3
	req_wd_saddr_2_capa_y_msb4b     : 4,	//  4: 7
	req_wd_saddr_8_capa_c_msb4b     : 4,	//  8:11
	req_wd_saddr_2_capa_c_msb4b     : 4,	// 12:15
	req_wd_saddr_capb_y_msb4b       : 4,	// 16:19
	req_wd_saddr_capb_c_msb4b       : 4,	// 20:23
	req_wd_saddr_scs_y_msb4b        : 4,	// 24:27
	req_wd_saddr_scs_c_msb4b        : 4;	// 28:31
} SADDR_EXT_6;

/*-----------------------------------------------------------------------------
	0x0a70 saddr_ext_7 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_saddr_mdi_y_msb4b        : 4,	//  0: 3
	req_wd_saddr_mdi_c_msb4b        : 4;	//  4: 7
} SADDR_EXT_7;

/*-----------------------------------------------------------------------------
	0x0a74 mif1_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fr_wd_q_full_capb_y             : 1,	//     0
	fr_wd_q_full_capb_c             : 1,	//     1
	fr_wd_q_full_scs_y              : 1,	//     2
	fr_wd_q_full_scs_c              : 1,	//     3
	                                : 4,	//  4: 7 reserved
	fr_wd_q_full_mdi_y              : 1,	//     8
	fr_wd_q_full_mdi_c              : 1;	//     9
} MIF1_STATUS;

/*-----------------------------------------------------------------------------
	0x0a78 mif1_bank_addr_0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_bank_addr_8_scs_y        : 4,	//  0: 3
	req_wd_bank_addr_8_scs_c        : 4,	//  4: 7
	req_wd_bank_addr_8_capb_y       : 4,	//  8:11
	req_wd_bank_addr_8_capb_c       : 4,	// 12:15
	req_wd_bank_addr_8_tnr_y        : 4,	// 16:19
	req_wd_bank_addr_8_tnr_c        : 4,	// 20:23
	req_wd_bank_addr_8_scm_y        : 4,	// 24:27
	req_wd_bank_addr_8_scm_c        : 4;	// 28:31
} MIF1_BANK_ADDR_0;

/*-----------------------------------------------------------------------------
	0x0a7c mif1_bank_addr_1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_bank_addr_8_capa_y       : 4,	//  0: 3
	req_wd_bank_addr_8_capa_c       : 4,	//  4: 7
	req_wd_bank_addr_8_mdi_y        : 4,	//  8:11
	req_wd_bank_addr_8_mdi_c        : 4;	// 12:15
} MIF1_BANK_ADDR_1;

/*-----------------------------------------------------------------------------
	0x0a80 mif1_bank_addr_2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	                                :16,	//  0:15 reserved
	req_wd_bank_addr_2_tnr_y        : 4,	// 16:19
	req_wd_bank_addr_2_tnr_c        : 4,	// 20:23
	req_wd_bank_addr_2_scm_y        : 4,	// 24:27
	req_wd_bank_addr_2_scm_c        : 4;	// 28:31
} MIF1_BANK_ADDR_2;

/*-----------------------------------------------------------------------------
	0x0a84 mif1_bank_addr_3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_bank_addr_2_capa_y       : 4,	//  0: 3
	req_wd_bank_addr_2_capa_c       : 4;	//  4: 7
} MIF1_BANK_ADDR_3;

/*-----------------------------------------------------------------------------
	0x0a88 mif1_req_pend0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_pend_v_scs_y             : 8,	//  0: 7
	req_wd_pend_v_scs_c             : 8,	//  8:15
	req_wd_pend_v_capb_y            : 8,	// 16:23
	req_wd_pend_v_capb_c            : 8;	// 24:31
} MIF1_REQ_PEND0;

/*-----------------------------------------------------------------------------
	0x0a8c mif1_req_pend1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_wd_pend_v_mdi_y             : 8,	//  0: 7
	req_wd_pend_v_mdi_c             : 8;	//  8:15
} MIF1_REQ_PEND1;

/*-----------------------------------------------------------------------------
	0x0a90 mif1_cmd_pri0_s ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_pri_s_v_capa_y           : 4,	//  0: 3
	cmd_wd_pri_s_v_capa_c           : 4,	//  4: 7
	cmd_wd_pri_s_v_capb_y           : 4,	//  8:11
	cmd_wd_pri_s_v_capb_c           : 4,	// 12:15
	cmd_wd_pri_s_v_tnr_y            : 4,	// 16:19
	cmd_wd_pri_s_v_tnr_c            : 4,	// 20:23
	cmd_wd_pri_s_v_scm_y            : 4,	// 24:27
	cmd_wd_pri_s_v_scm_c            : 4;	// 28:31
} MIF1_CMD_PRI0_S;

/*-----------------------------------------------------------------------------
	0x0a94 mif1_cmd_pri1_s ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_wd_pri_s_v_scs_y            : 4,	//  0: 3
	cmd_wd_pri_s_v_scs_c            : 4,	//  4: 7
	cmd_wd_pri_s_v_mdi_y            : 4,	//  8:11
	cmd_wd_pri_s_v_mdi_c            : 4;	// 12:15
} MIF1_CMD_PRI1_S;

/*-----------------------------------------------------------------------------
	0x0a98 mif1_reset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	reset_mif_w_scs_y               : 1,	//     0
	reset_mif_w_scs_c               : 1,	//     1
	reset_mif_w_capb_y              : 1,	//     2
	reset_mif_w_capb_c              : 1,	//     3
	reset_mif_w_mdi_y               : 1,	//     4
	reset_mif_w_mdi_c               : 1,	//     5
	                                :14,	//  6:19 reserved
	reset_time_out_cnt              :12;	// 20:31
} MIF1_RESET;

/*-----------------------------------------------------------------------------
	0x1500 mif2_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	block_req_r_scm_y               : 1,	//     0
	block_req_r_scm_c               : 1,	//     1
	block_req_r_scs_y               : 1,	//     2
	block_req_r_scs_c               : 1,	//     3
	                                : 8,	//  4:11 reserved
	set_10b_req_rd_scm_y            : 1,	//    12
	set_10b_req_rd_scm_c            : 1,	//    13
	                                : 2,	// 14:15 reserved
	disable_mif2                    : 1,	//    16
	load_req_stv_v                  : 1,	//    17
	load_req_pri_v                  : 1,	//    18
	load_cmd_saddr_v                : 1,	//    19
	load_cmd_pend_v                 : 1,	//    20
	load_cmd_pri_v                  : 1,	//    21
	load_cmd_stride_v               : 1,	//    22
	enable_stv_cnt                  : 1;	//    23
} MIF2_CTRL;

/*-----------------------------------------------------------------------------
	0x1504 mif2_cmd_pri ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_v_scm_y              : 4,	//  0: 3
	cmd_rd_pri_v_scm_c              : 4,	//  4: 7
	cmd_rd_pri_v_scs_y              : 4,	//  8:11
	cmd_rd_pri_v_scs_c              : 4;	// 12:15
} MIF2_CMD_PRI;

/*-----------------------------------------------------------------------------
	0x1508 mif2_req_pri ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_pri_v_scm_y              : 4,	//  0: 3
	req_rd_pri_v_scm_c              : 4,	//  4: 7
	req_rd_pri_v_scs_y              : 4,	//  8:11
	req_rd_pri_v_scs_c              : 4;	// 12:15
} MIF2_REQ_PRI;

/*-----------------------------------------------------------------------------
	0x150c mif2_req_stv ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_stv_v_scm_y              : 4,	//  0: 3
	req_rd_stv_v_scm_c              : 4,	//  4: 7
	req_rd_stv_v_scs_y              : 4,	//  8:11
	req_rd_stv_v_scs_c              : 4;	// 12:15
} MIF2_REQ_STV;

/*-----------------------------------------------------------------------------
	0x1510 mif2_cmd_pend ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pend_v_scm_y             : 4,	//  0: 3
	cmd_rd_pend_v_scm_c             : 4,	//  4: 7
	cmd_rd_pend_v_scs_y             : 4,	//  8:11
	cmd_rd_pend_v_scs_c             : 4;	// 12:15
} MIF2_CMD_PEND;

/*-----------------------------------------------------------------------------
	0x1514 mif2_cmd_stride ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_stride_v_8_scm_y         : 4,	//  0: 3
	cmd_rd_stride_v_2_scm_y         : 4,	//  4: 7
	cmd_rd_stride_v_8_scm_c         : 4,	//  8:11
	cmd_rd_stride_v_2_scm_c         : 4,	// 12:15
	cmd_rd_stride_v_scs_y           : 4,	// 16:19
	cmd_rd_stride_v_scs_c           : 4,	// 20:23
	man_cmd_rd_stride_v_scm_y       : 1,	//    24
	man_cmd_rd_stride_v_scm_c       : 1,	//    25
	man_cmd_rd_stride_v_scs_y       : 1,	//    26
	man_cmd_rd_stride_v_scs_c       : 1;	//    27
} MIF2_CMD_STRIDE;

/*-----------------------------------------------------------------------------
	0x1518 saddr_rd_scm_y ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_scm_y            :16,	//  0:15
	req_rd_saddr_8_scm_y            :16;	// 16:31
} SADDR_RD_SCM_Y;

/*-----------------------------------------------------------------------------
	0x151c saddr_rd_scm_c ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_2_scm_c            :16,	//  0:15
	req_rd_saddr_8_scm_c            :16;	// 16:31
} SADDR_RD_SCM_C;

/*-----------------------------------------------------------------------------
	0x1520 saddr_rd_scs_yc ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_scs_y              :16,	//  0:15
	req_rd_saddr_scs_c              :16;	// 16:31
} SADDR_RD_SCS_YC;

/*-----------------------------------------------------------------------------
	0x1524 saddr_ext_8 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_8_scm_y_msb4b      : 4,	//  0: 3
	req_rd_saddr_2_scm_y_msb4b      : 4,	//  4: 7
	req_rd_saddr_8_scm_c_msb4b      : 4,	//  8:11
	req_rd_saddr_2_scm_c_msb4b      : 4,	// 12:15
	req_rd_saddr_scs_y_msb4b        : 4,	// 16:19
	req_rd_saddr_scs_c_msb4b        : 4;	// 20:23
} SADDR_EXT_8;

/*-----------------------------------------------------------------------------
	0x1528 mif2_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fr_rd_valid_scm_y               : 1,	//     0
	fr_rd_valid_scm_c               : 1,	//     1
	fr_rd_valid_scs_y               : 1,	//     2
	fr_rd_valid_scs_c               : 1;	//     3
} MIF2_STATUS;

/*-----------------------------------------------------------------------------
	0x152c saddr_param_update1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	load                            : 1,	//     0
	                                :10,	//  1:10 reserved
	register_update_address1        :21;	// 11:31
} SADDR_PARAM_UPDATE1;

/*-----------------------------------------------------------------------------
	0x1530 mif2_bank_addr_0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_bank_addr_8_scm_y        : 4,	//  0: 3
	req_rd_bank_addr_8_scm_c        : 4,	//  4: 7
	req_rd_bank_addr_8_scs_y        : 4,	//  8:11
	req_rd_bank_addr_8_scs_c        : 4;	// 12:15
} MIF2_BANK_ADDR_0;

/*-----------------------------------------------------------------------------
	0x1534 mif2_bank_addr_1 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x1538 mif2_bank_addr_2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_bank_addr_2_scm_y        : 4,	//  0: 3
	req_rd_bank_addr_2_scm_c        : 4,	//  4: 7
	req_rd_bank_addr_2_scs_y        : 4,	//  8:11
	req_rd_bank_addr_2_scs_c        : 4;	// 12:15
} MIF2_BANK_ADDR_2;

/*-----------------------------------------------------------------------------
	0x153c mif2_bank_addr_3 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x1540 mif2_lsb_switch ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_lsb_scm_y                : 2,	//  0: 1
	req_rd_lsb_scm_c                : 2;	//  2: 3
} MIF2_LSB_SWITCH;

/*-----------------------------------------------------------------------------
	0x1544 mif2_cmd_pri_s ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_s_v_scm_y            : 4,	//  0: 3
	cmd_rd_pri_s_v_scm_c            : 4,	//  4: 7
	cmd_rd_pri_s_v_scs_y            : 4,	//  8:11
	cmd_rd_pri_s_v_scs_c            : 4;	// 12:15
} MIF2_CMD_PRI_S;

/*-----------------------------------------------------------------------------
	0x1548 mif2_reset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	reset_mif_r_scm_y               : 1,	//     0
	reset_mif_r_scm_c               : 1,	//     1
	reset_mif_r_scs_y               : 1,	//     2
	reset_mif_r_scs_c               : 1,	//     3
	                                :16,	//  4:19 reserved
	reset_time_out_cnt              :12;	// 20:31
} MIF2_RESET;

/*-----------------------------------------------------------------------------
	0x1b00 mif3_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	block_req_r_osd_0               : 1,	//     0
	block_req_r_osd_1               : 1,	//     1
	block_req_r_osd_2               : 1,	//     2
	block_req_r_osd_3               : 1,	//     3
	block_req_r_cve_y               : 1,	//     4
	block_req_r_cve_c               : 1,	//     5
	                                :10,	//  6:15 reserved
	disable_mif3                    : 1,	//    16
	load_req_stv_v                  : 1,	//    17
	load_req_pri_v                  : 1,	//    18
	load_cmd_saddr_v                : 1,	//    19
	load_cmd_pend_v                 : 1,	//    20
	load_cmd_pri_v                  : 1,	//    21
	load_cmd_stride_v               : 1,	//    22
	enable_stv_cnt                  : 1,	//    23
	mif3_mrb_bank_mode              : 3;	// 24:26
} MIF3_CTRL;

/*-----------------------------------------------------------------------------
	0x1b04 mif3_cmd_pri ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_v_osd_0              : 4,	//  0: 3
	cmd_rd_pri_v_osd_1              : 4,	//  4: 7
	cmd_rd_pri_v_osd_2              : 4,	//  8:11
	cmd_rd_pri_v_osd_3              : 4,	// 12:15
	cmd_rd_pri_v_cve_y              : 4,	// 16:19
	cmd_rd_pri_v_cve_c              : 4;	// 20:23
} MIF3_CMD_PRI;

/*-----------------------------------------------------------------------------
	0x1b08 mif3_req_pri ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_pri_v_osd_0              : 4,	//  0: 3
	req_rd_pri_v_osd_1              : 4,	//  4: 7
	req_rd_pri_v_osd_2              : 4,	//  8:11
	req_rd_pri_v_osd_3              : 4,	// 12:15
	req_rd_pri_v_cve_y              : 4,	// 16:19
	req_rd_pri_v_cve_c              : 4;	// 20:23
} MIF3_REQ_PRI;

/*-----------------------------------------------------------------------------
	0x1b0c mif3_req_stv ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_stv_v_osd_0              : 4,	//  0: 3
	req_rd_stv_v_osd_1              : 4,	//  4: 7
	req_rd_stv_v_osd_2              : 4,	//  8:11
	req_rd_stv_v_osd_3              : 4,	// 12:15
	req_rd_stv_v_cve_y              : 4,	// 16:19
	req_rd_stv_v_cve_c              : 4;	// 20:23
} MIF3_REQ_STV;

/*-----------------------------------------------------------------------------
	0x1b10 mif3_cmd_pend ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pend_v_osd_0             : 4,	//  0: 3
	cmd_rd_pend_v_osd_1             : 4,	//  4: 7
	cmd_rd_pend_v_osd_2             : 4,	//  8:11
	cmd_rd_pend_v_osd_3             : 4,	// 12:15
	cmd_rd_pend_v_cve_y             : 4,	// 16:19
	cmd_rd_pend_v_cve_c             : 4;	// 20:23
} MIF3_CMD_PEND;

/*-----------------------------------------------------------------------------
	0x1b14 mif3_cmd_stride ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_stride_v_osd_0           : 4,	//  0: 3
	cmd_rd_stride_v_osd_1           : 4,	//  4: 7
	cmd_rd_stride_v_osd_2           : 4,	//  8:11
	cmd_rd_stride_v_osd_3           : 4,	// 12:15
	cmd_rd_stride_v_cve_y           : 4,	// 16:19
	cmd_rd_stride_v_cve_c           : 4,	// 20:23
	man_cmd_rd_stride_v_osd_0       : 1,	//    24
	man_cmd_rd_stride_v_osd_1       : 1,	//    25
	man_cmd_rd_stride_v_osd_2       : 1,	//    26
	man_cmd_rd_stride_v_osd_3       : 1,	//    27
	man_cmd_rd_stride_v_cve_y       : 1,	//    28
	man_cmd_rd_stride_v_cve_c       : 1;	//    29
} MIF3_CMD_STRIDE;

/*-----------------------------------------------------------------------------
	0x1b18 saddr_rd_osd_01 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_osd_1              :16,	//  0:15
	req_rd_saddr_osd_0              :16;	// 16:31
} SADDR_RD_OSD_01;

/*-----------------------------------------------------------------------------
	0x1b1c saddr_rd_osd_23 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_osd_3              :16,	//  0:15
	req_rd_saddr_osd_2              :16;	// 16:31
} SADDR_RD_OSD_23;

/*-----------------------------------------------------------------------------
	0x1b20 saddr_rd_cve_yc ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_cve_y              :16,	//  0:15
	req_rd_saddr_cve_c              :16;	// 16:31
} SADDR_RD_CVE_YC;

/*-----------------------------------------------------------------------------
	0x1b24 saddr_ext_9 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	req_rd_saddr_osd_0_msb4b        : 4,	//  0: 3
	req_rd_saddr_osd_1_msb4b        : 4,	//  4: 7
	req_rd_saddr_osd_2_msb4b        : 4,	//  8:11
	req_rd_saddr_osd_3_msb4b        : 4,	// 12:15
	req_rd_saddr_cve_y_msb4b        : 4,	// 16:19
	req_rd_saddr_cve_c_msb4b        : 4;	// 20:23
} SADDR_EXT_9;

/*-----------------------------------------------------------------------------
	0x1b28 mif3_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fr_rd_valid_osd_0               : 1,	//     0
	fr_rd_valid_osd_1               : 1,	//     1
	fr_rd_valid_osd_2               : 1,	//     2
	fr_rd_valid_osd_3               : 1,	//     3
	fr_rd_valid_cve_y               : 1,	//     4
	fr_rd_valid_cve_c               : 1;	//     5
} MIF3_STATUS;

/*-----------------------------------------------------------------------------
	0x1b2c mif3_bank_addr_0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	                                :16,	//  0:15 reserved
	req_rd_bank_addr_8_cve_y        : 4,	// 16:19
	req_rd_bank_addr_8_cve_c        : 4;	// 20:23
} MIF3_BANK_ADDR_0;

/*-----------------------------------------------------------------------------
	0x1b30 mif3_bank_addr_1 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x1b34 mif3_bank_addr_2 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x1b38 mif3_bank_addr_3 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x1b3c mif3_cmd_pri_s ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cmd_rd_pri_s_v_osd_0            : 4,	//  0: 3
	cmd_rd_pri_s_v_osd_1            : 4,	//  4: 7
	cmd_rd_pri_s_v_osd_2            : 4,	//  8:11
	cmd_rd_pri_s_v_osd_3            : 4,	// 12:15
	cmd_rd_pri_s_v_cve_y            : 4,	// 16:19
	cmd_rd_pri_s_v_cve_c            : 4;	// 20:23
} MIF3_CMD_PRI_S;

/*-----------------------------------------------------------------------------
	0x1b40 mif3_reset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	reset_mif_r_osd_0               : 1,	//     0
	reset_mif_r_osd_1               : 1,	//     1
	reset_mif_r_osd_2               : 1,	//     2
	reset_mif_r_osd_3               : 1,	//     3
	reset_mif_r_cve_y               : 1,	//     4
	reset_mif_r_cve_c               : 1,	//     5
	                                :14,	//  6:19 reserved
	reset_time_out_cnt              :12;	// 20:31
} MIF3_RESET;

typedef struct {
	MIF0_CTRL0                      	mif0_ctrl0                      ;	// 0x0600 : ''
	MIF0_CTRL1                      	mif0_ctrl1                      ;	// 0x0604 : ''
	MIF0_CMD_PRI0                   	mif0_cmd_pri0                   ;	// 0x0608 : ''
	MIF0_CMD_PRI1                   	mif0_cmd_pri1                   ;	// 0x060c : ''
	MIF0_REQ_PRI0                   	mif0_req_pri0                   ;	// 0x0610 : ''
	MIF0_REQ_PRI1                   	mif0_req_pri1                   ;	// 0x0614 : ''
	MIF0_REQ_STV0                   	mif0_req_stv0                   ;	// 0x0618 : ''
	MIF0_REQ_STV1                   	mif0_req_stv1                   ;	// 0x061c : ''
	MIF0_CMD_PEND0                  	mif0_cmd_pend0                  ;	// 0x0620 : ''
	MIF0_CMD_PEND1                  	mif0_cmd_pend1                  ;	// 0x0624 : ''
	MIF0_CMD_PEND2                  	mif0_cmd_pend2                  ;	// 0x0628 : ''
	MIF0_CMD_STRIDE0                	mif0_cmd_stride0                ;	// 0x062c : ''
	MIF0_CMD_STRIDE1                	mif0_cmd_stride1                ;	// 0x0630 : ''
	MIF0_CMD_STRIDE2                	mif0_cmd_stride2                ;	// 0x0634 : ''
	MIF0_CMD_STRIDE3                	mif0_cmd_stride3                ;	// 0x0638 : ''
	SADDR_RD_MVI0_YC                	saddr_rd_mvi0_yc                ;	// 0x063c : ''
	SADDR_RD_IPC0_Y                 	saddr_rd_ipc0_y                 ;	// 0x0640 : ''
	SADDR_RD_IPC_C                  	saddr_rd_ipc_c                  ;	// 0x0644 : ''
	SADDR_RD_IPC1_Y                 	saddr_rd_ipc1_y                 ;	// 0x0648 : ''
	SADDR_RD_TNR1_Y                 	saddr_rd_tnr1_y                 ;	// 0x064c : ''
	SADDR_RD_CAPA_Y                 	saddr_rd_capa_y                 ;	// 0x0650 : ''
	SADDR_RD_CAPA_C                 	saddr_rd_capa_c                 ;	// 0x0654 : ''
	SADDR_RD_TNR_Y                  	saddr_rd_tnr_y                  ;	// 0x0658 : ''
	SADDR_RD_TNR_C                  	saddr_rd_tnr_c                  ;	// 0x065c : ''
	SADDR_RD_3DF_YC                 	saddr_rd_3df_yc                 ;	// 0x0660 : ''
	SADDR_RD_DL_Y                   	saddr_rd_dl_y                   ;	// 0x0664 : ''
	SADDR_RD_DL_C                   	saddr_rd_dl_c                   ;	// 0x0668 : ''
	SADDR_RD_DR_Y                   	saddr_rd_dr_y                   ;	// 0x066c : ''
	SADDR_RD_DR_C                   	saddr_rd_dr_c                   ;	// 0x0670 : ''
	SADDR_EXT_0                     	saddr_ext_0                     ;	// 0x0674 : ''
	SADDR_EXT_1                     	saddr_ext_1                     ;	// 0x0678 : ''
	SADDR_EXT_2                     	saddr_ext_2                     ;	// 0x067c : ''
	SADDR_EXT_3                     	saddr_ext_3                     ;	// 0x0680 : ''
	MIF0_STATUS                     	mif0_status                     ;	// 0x0684 : ''
	SADDR_PARAM_UPDATE0_M           	saddr_param_update0_m           ;	// 0x0688 : ''
	SADDR_PARAM_UPDATE0_S0          	saddr_param_update0_s0          ;	// 0x068c : ''
	SADDR_PARAM_UPDATE0_S1          	saddr_param_update0_s1          ;	// 0x0690 : ''
	MIF0_BANK_ADDR_0                	mif0_bank_addr_0                ;	// 0x0694 : ''
	MIF0_BANK_ADDR_1                	mif0_bank_addr_1                ;	// 0x0698 : ''
	MIF0_BANK_ADDR_2                	mif0_bank_addr_2                ;	// 0x069c : ''
	MIF0_BANK_ADDR_3                	mif0_bank_addr_3                ;	// 0x06a0 : ''
	MIF0_LSB_SWITCH                 	mif0_lsb_switch                 ;	// 0x06a4 : ''
	MIF0_REQ_PEND0                  	mif0_req_pend0                  ;	// 0x06a8 : ''
	MIF0_REQ_PEND1                  	mif0_req_pend1                  ;	// 0x06ac : ''
	MIF0_REQ_PEND2                  	mif0_req_pend2                  ;	// 0x06b0 : ''
	MIF0_REQ_PEND3                  	mif0_req_pend3                  ;	// 0x06b4 : ''
	MIF0_REQ_PEND4                  	mif0_req_pend4                  ;	// 0x06b8 : ''
	MIF0_CMD_PRI0_S                 	mif0_cmd_pri0_s                 ;	// 0x06bc : ''
	MIF0_CMD_PRI1_S                 	mif0_cmd_pri1_s                 ;	// 0x06c0 : ''
	MIF0_RESET                      	mif0_reset                      ;	// 0x06c4 : ''
	SADDR_PARAM_UPDATE0_M1          	saddr_param_update0_m1          ;	// 0x06c8 : ''
	UINT32                          	de_zebu_dbg0                    ;	// 0x06cc : ''
	UINT32                          	de_zebu_dbg1                    ;	// 0x06d0 : ''
	UINT32                          	de_zebu_dbg2                    ;	// 0x06d4 : ''
	UINT32                          	de_zebu_dbg3                    ;	// 0x06d8 : ''
} MIF0_REG_T;
/* 55 regs, 55 types */

typedef struct {
	MIF1_CTRL                       	mif1_ctrl                       ;	// 0x0a00 : ''
	MIF1_CMD_PRI0                   	mif1_cmd_pri0                   ;	// 0x0a04 : ''
	MIF1_CMD_PRI1                   	mif1_cmd_pri1                   ;	// 0x0a08 : ''
	MIF1_REQ_PRI0                   	mif1_req_pri0                   ;	// 0x0a0c : ''
	MIF1_REQ_PRI1                   	mif1_req_pri1                   ;	// 0x0a10 : ''
	MIF1_REQ_STV0                   	mif1_req_stv0                   ;	// 0x0a14 : ''
	MIF1_REQ_STV1                   	mif1_req_stv1                   ;	// 0x0a18 : ''
	MIF1_CMD_PEND0                  	mif1_cmd_pend0                  ;	// 0x0a1c : ''
	MIF1_CMD_PEND1                  	mif1_cmd_pend1                  ;	// 0x0a20 : ''
	MIF1_CMD_STRIDE0                	mif1_cmd_stride0                ;	// 0x0a24 : ''
	MIF1_CMD_STRIDE1                	mif1_cmd_stride1                ;	// 0x0a28 : ''
	MIF1_CMD_STRIDE2                	mif1_cmd_stride2                ;	// 0x0a2c : ''
	SADDR_WD_CAPA_Y                 	saddr_wd_capa_y                 ;	// 0x0a30 : ''
	SADDR_WD_CAPA_C                 	saddr_wd_capa_c                 ;	// 0x0a34 : ''
	SADDR_WD_TNR_Y                  	saddr_wd_tnr_y                  ;	// 0x0a38 : ''
	SADDR_WD_TNR_C                  	saddr_wd_tnr_c                  ;	// 0x0a3c : ''
	SADDR_WD_SCM_Y                  	saddr_wd_scm_y                  ;	// 0x0a40 : ''
	SADDR_WD_SCM_C                  	saddr_wd_scm_c                  ;	// 0x0a44 : ''
	SADDR_WD_CAPB_YC                	saddr_wd_capb_yc                ;	// 0x0a48 : ''
	SADDR_WD_SCS_YC                 	saddr_wd_scs_yc                 ;	// 0x0a4c : ''
	SADDR_WD_MDI_YC                 	saddr_wd_mdi_yc                 ;	// 0x0a50 : ''
	SADDR_WD_DL_Y                   	saddr_wd_dl_y                   ;	// 0x0a54 : ''
	SADDR_WD_DL_C                   	saddr_wd_dl_c                   ;	// 0x0a58 : ''
	SADDR_WD_DR_Y                   	saddr_wd_dr_y                   ;	// 0x0a5c : ''
	SADDR_WD_DR_C                   	saddr_wd_dr_c                   ;	// 0x0a60 : ''
	SADDR_EXT_4                     	saddr_ext_4                     ;	// 0x0a64 : ''
	SADDR_EXT_5                     	saddr_ext_5                     ;	// 0x0a68 : ''
	SADDR_EXT_6                     	saddr_ext_6                     ;	// 0x0a6c : ''
	SADDR_EXT_7                     	saddr_ext_7                     ;	// 0x0a70 : ''
	MIF1_STATUS                     	mif1_status                     ;	// 0x0a74 : ''
	MIF1_BANK_ADDR_0                	mif1_bank_addr_0                ;	// 0x0a78 : ''
	MIF1_BANK_ADDR_1                	mif1_bank_addr_1                ;	// 0x0a7c : ''
	MIF1_BANK_ADDR_2                	mif1_bank_addr_2                ;	// 0x0a80 : ''
	MIF1_BANK_ADDR_3                	mif1_bank_addr_3                ;	// 0x0a84 : ''
	MIF1_REQ_PEND0                  	mif1_req_pend0                  ;	// 0x0a88 : ''
	MIF1_REQ_PEND1                  	mif1_req_pend1                  ;	// 0x0a8c : ''
	MIF1_CMD_PRI0_S                 	mif1_cmd_pri0_s                 ;	// 0x0a90 : ''
	MIF1_CMD_PRI1_S                 	mif1_cmd_pri1_s                 ;	// 0x0a94 : ''
	MIF1_RESET                      	mif1_reset                      ;	// 0x0a98 : ''
} MIF1_REG_T;
/* 39 regs, 39 types */

typedef struct {
	MIF2_CTRL                       	mif2_ctrl                       ;	// 0x1500 : ''
	MIF2_CMD_PRI                    	mif2_cmd_pri                    ;	// 0x1504 : ''
	MIF2_REQ_PRI                    	mif2_req_pri                    ;	// 0x1508 : ''
	MIF2_REQ_STV                    	mif2_req_stv                    ;	// 0x150c : ''
	MIF2_CMD_PEND                   	mif2_cmd_pend                   ;	// 0x1510 : ''
	MIF2_CMD_STRIDE                 	mif2_cmd_stride                 ;	// 0x1514 : ''
	SADDR_RD_SCM_Y                  	saddr_rd_scm_y                  ;	// 0x1518 : ''
	SADDR_RD_SCM_C                  	saddr_rd_scm_c                  ;	// 0x151c : ''
	SADDR_RD_SCS_YC                 	saddr_rd_scs_yc                 ;	// 0x1520 : ''
	SADDR_EXT_8                     	saddr_ext_8                     ;	// 0x1524 : ''
	MIF2_STATUS                     	mif2_status                     ;	// 0x1528 : ''
	SADDR_PARAM_UPDATE1             	saddr_param_update1             ;	// 0x152c : ''
	MIF2_BANK_ADDR_0                	mif2_bank_addr_0                ;	// 0x1530 : ''
	UINT32                          	mif2_bank_addr_1                ;	// 0x1534 : ''
	MIF2_BANK_ADDR_2                	mif2_bank_addr_2                ;	// 0x1538 : ''
	UINT32                          	mif2_bank_addr_3                ;	// 0x153c : ''
	MIF2_LSB_SWITCH                 	mif2_lsb_switch                 ;	// 0x1540 : ''
	MIF2_CMD_PRI_S                  	mif2_cmd_pri_s                  ;	// 0x1544 : ''
	MIF2_RESET                      	mif2_reset                      ;	// 0x1548 : ''
} MIF2_REG_T;
/* 19 regs, 19 types */

typedef struct {
	MIF3_CTRL                       	mif3_ctrl                       ;	// 0x1b00 : ''
	MIF3_CMD_PRI                    	mif3_cmd_pri                    ;	// 0x1b04 : ''
	MIF3_REQ_PRI                    	mif3_req_pri                    ;	// 0x1b08 : ''
	MIF3_REQ_STV                    	mif3_req_stv                    ;	// 0x1b0c : ''
	MIF3_CMD_PEND                   	mif3_cmd_pend                   ;	// 0x1b10 : ''
	MIF3_CMD_STRIDE                 	mif3_cmd_stride                 ;	// 0x1b14 : ''
	SADDR_RD_OSD_01                 	saddr_rd_osd_01                 ;	// 0x1b18 : ''
	SADDR_RD_OSD_23                 	saddr_rd_osd_23                 ;	// 0x1b1c : ''
	SADDR_RD_CVE_YC                 	saddr_rd_cve_yc                 ;	// 0x1b20 : ''
	SADDR_EXT_9                     	saddr_ext_9                     ;	// 0x1b24 : ''
	MIF3_STATUS                     	mif3_status                     ;	// 0x1b28 : ''
	MIF3_BANK_ADDR_0                	mif3_bank_addr_0                ;	// 0x1b2c : ''
	UINT32                          	mif3_bank_addr_1                ;	// 0x1b30 : ''
	UINT32                          	mif3_bank_addr_2                ;	// 0x1b34 : ''
	UINT32                          	mif3_bank_addr_3                ;	// 0x1b38 : ''
	MIF3_CMD_PRI_S                  	mif3_cmd_pri_s                  ;	// 0x1b3c : ''
	MIF3_RESET                      	mif3_reset                      ;	// 0x1b40 : ''
} MIF3_REG_T;
/* 17 regs, 17 types */

/* 130 regs, 130 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegMIF0 : real register of MIF0.
 * gpRegMIF0     : shadow register.
 *
 * @def MIF0_RdFL: Read  FLushing : Shadow <- Real.
 * @def MIF0_WrFL: Write FLushing : Shadow -> Real.
 * @def MIF0_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def MIF0_Wr  : Write whole register(UINT32) from Shadow register.
 * @def MIF0_Rd01 ~ MIF0_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def MIF0_Wr01 ~ MIF0_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define MIF0_RdFL(_r)			((gpRegMIF0->_r)=(gpRealRegMIF0->_r))
#define MIF0_WrFL(_r)			((gpRealRegMIF0->_r)=(gpRegMIF0->_r))

#define MIF0_Rd(_r)			*((UINT32*)(&(gpRegMIF0->_r)))
#define MIF0_Wr(_r,_v)			((MIF0_Rd(_r))=((UINT32)(_v)))

#define MIF0_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
								} while(0)

#define MIF0_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
								} while(0)

#define MIF0_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
								} while(0)

#define MIF0_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
								} while(0)

#define MIF0_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
								} while(0)

#define MIF0_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
								} while(0)

#define MIF0_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
								} while(0)

#define MIF0_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
								} while(0)

#define MIF0_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
								} while(0)

#define MIF0_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
									(_v10) = (gpRegMIF0->_r._f10);				\
								} while(0)

#define MIF0_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
									(_v10) = (gpRegMIF0->_r._f10);				\
									(_v11) = (gpRegMIF0->_r._f11);				\
								} while(0)

#define MIF0_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
									(_v10) = (gpRegMIF0->_r._f10);				\
									(_v11) = (gpRegMIF0->_r._f11);				\
									(_v12) = (gpRegMIF0->_r._f12);				\
								} while(0)

#define MIF0_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
									(_v10) = (gpRegMIF0->_r._f10);				\
									(_v11) = (gpRegMIF0->_r._f11);				\
									(_v12) = (gpRegMIF0->_r._f12);				\
									(_v13) = (gpRegMIF0->_r._f13);				\
								} while(0)

#define MIF0_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
									(_v10) = (gpRegMIF0->_r._f10);				\
									(_v11) = (gpRegMIF0->_r._f11);				\
									(_v12) = (gpRegMIF0->_r._f12);				\
									(_v13) = (gpRegMIF0->_r._f13);				\
									(_v14) = (gpRegMIF0->_r._f14);				\
								} while(0)

#define MIF0_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
									(_v10) = (gpRegMIF0->_r._f10);				\
									(_v11) = (gpRegMIF0->_r._f11);				\
									(_v12) = (gpRegMIF0->_r._f12);				\
									(_v13) = (gpRegMIF0->_r._f13);				\
									(_v14) = (gpRegMIF0->_r._f14);				\
									(_v15) = (gpRegMIF0->_r._f15);				\
								} while(0)

#define MIF0_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegMIF0->_r._f01);				\
									(_v02) = (gpRegMIF0->_r._f02);				\
									(_v03) = (gpRegMIF0->_r._f03);				\
									(_v04) = (gpRegMIF0->_r._f04);				\
									(_v05) = (gpRegMIF0->_r._f05);				\
									(_v06) = (gpRegMIF0->_r._f06);				\
									(_v07) = (gpRegMIF0->_r._f07);				\
									(_v08) = (gpRegMIF0->_r._f08);				\
									(_v09) = (gpRegMIF0->_r._f09);				\
									(_v10) = (gpRegMIF0->_r._f10);				\
									(_v11) = (gpRegMIF0->_r._f11);				\
									(_v12) = (gpRegMIF0->_r._f12);				\
									(_v13) = (gpRegMIF0->_r._f13);				\
									(_v14) = (gpRegMIF0->_r._f14);				\
									(_v15) = (gpRegMIF0->_r._f15);				\
									(_v16) = (gpRegMIF0->_r._f16);				\
								} while(0)


#define MIF0_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
								} while(0)

#define MIF0_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
								} while(0)

#define MIF0_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
								} while(0)

#define MIF0_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
								} while(0)

#define MIF0_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
								} while(0)

#define MIF0_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
								} while(0)

#define MIF0_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
								} while(0)

#define MIF0_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
								} while(0)

#define MIF0_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
								} while(0)

#define MIF0_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
									(gpRegMIF0->_r._f10) = (_v10);				\
								} while(0)

#define MIF0_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
									(gpRegMIF0->_r._f10) = (_v10);				\
									(gpRegMIF0->_r._f11) = (_v11);				\
								} while(0)

#define MIF0_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
									(gpRegMIF0->_r._f10) = (_v10);				\
									(gpRegMIF0->_r._f11) = (_v11);				\
									(gpRegMIF0->_r._f12) = (_v12);				\
								} while(0)

#define MIF0_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
									(gpRegMIF0->_r._f10) = (_v10);				\
									(gpRegMIF0->_r._f11) = (_v11);				\
									(gpRegMIF0->_r._f12) = (_v12);				\
									(gpRegMIF0->_r._f13) = (_v13);				\
								} while(0)

#define MIF0_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
									(gpRegMIF0->_r._f10) = (_v10);				\
									(gpRegMIF0->_r._f11) = (_v11);				\
									(gpRegMIF0->_r._f12) = (_v12);				\
									(gpRegMIF0->_r._f13) = (_v13);				\
									(gpRegMIF0->_r._f14) = (_v14);				\
								} while(0)

#define MIF0_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
									(gpRegMIF0->_r._f10) = (_v10);				\
									(gpRegMIF0->_r._f11) = (_v11);				\
									(gpRegMIF0->_r._f12) = (_v12);				\
									(gpRegMIF0->_r._f13) = (_v13);				\
									(gpRegMIF0->_r._f14) = (_v14);				\
									(gpRegMIF0->_r._f15) = (_v15);				\
								} while(0)

#define MIF0_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegMIF0->_r._f01) = (_v01);				\
									(gpRegMIF0->_r._f02) = (_v02);				\
									(gpRegMIF0->_r._f03) = (_v03);				\
									(gpRegMIF0->_r._f04) = (_v04);				\
									(gpRegMIF0->_r._f05) = (_v05);				\
									(gpRegMIF0->_r._f06) = (_v06);				\
									(gpRegMIF0->_r._f07) = (_v07);				\
									(gpRegMIF0->_r._f08) = (_v08);				\
									(gpRegMIF0->_r._f09) = (_v09);				\
									(gpRegMIF0->_r._f10) = (_v10);				\
									(gpRegMIF0->_r._f11) = (_v11);				\
									(gpRegMIF0->_r._f12) = (_v12);				\
									(gpRegMIF0->_r._f13) = (_v13);				\
									(gpRegMIF0->_r._f14) = (_v14);				\
									(gpRegMIF0->_r._f15) = (_v15);				\
									(gpRegMIF0->_r._f16) = (_v16);				\
								} while(0)

/* Indexed Register Access.
 * 
 * There is in-direct field specified by 'index' field within a register.
 * Normally a register has only one meaning for a 'field_name', but indexed register
 * can hold several data for a 'field_name' specifed by 'index' field of indexed register.
 * When writing an 3rd data for given 'field_name' register, you need to set 'rw' = 0, 'index' = 2,
 * and 'load' = 0.
 * 
 * ASSUMPTION
 * For Writing indexed register load bit 
 * 
 * parameter list
 * _r     : name of register
 * _lname : name of load  bit field	: shall be 0 after macro executed.
 * _rwname: name of rw    bit field : shall be 0 after MIF0_Wind(), 1 for MIF0_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * MIF0_Rind : General indexed register Read.(
 * MIF0_Wind : General indexed register Read.
 *
 * MIF0_Ridx : For 'index', 'rw', 'load' field name
 * MIF0_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define MIF0_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								MIF0_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								MIF0_WrFL(_r);									\
								MIF0_RdFL(_r);									\
								MIF0_Rd01(_r,_fname,_fval);						\
							} while (0)

#define MIF0_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				MIF0_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define MIF0_Ridx(_r, _ival, _fname, _fval)	MIF0_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define MIF0_Widx(_r, _ival, _fname, _fval)	MIF0_Wind(_r,load,rw,index,_ival,_fname,_fval)

/*
 * @{
 * Naming for register pointer.
 * gpRealRegMIF1 : real register of MIF1.
 * gpRegMIF1     : shadow register.
 *
 * @def MIF1_RdFL: Read  FLushing : Shadow <- Real.
 * @def MIF1_WrFL: Write FLushing : Shadow -> Real.
 * @def MIF1_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def MIF1_Wr  : Write whole register(UINT32) from Shadow register.
 * @def MIF1_Rd01 ~ MIF1_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def MIF1_Wr01 ~ MIF1_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define MIF1_RdFL(_r)			((gpRegMIF1->_r)=(gpRealRegMIF1->_r))
#define MIF1_WrFL(_r)			((gpRealRegMIF1->_r)=(gpRegMIF1->_r))

#define MIF1_Rd(_r)			*((UINT32*)(&(gpRegMIF1->_r)))
#define MIF1_Wr(_r,_v)			((MIF1_Rd(_r))=((UINT32)(_v)))

#define MIF1_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
								} while(0)

#define MIF1_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
								} while(0)

#define MIF1_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
								} while(0)

#define MIF1_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
								} while(0)

#define MIF1_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
								} while(0)

#define MIF1_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
								} while(0)

#define MIF1_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
								} while(0)

#define MIF1_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
								} while(0)

#define MIF1_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
								} while(0)

#define MIF1_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
									(_v10) = (gpRegMIF1->_r._f10);				\
								} while(0)

#define MIF1_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
									(_v10) = (gpRegMIF1->_r._f10);				\
									(_v11) = (gpRegMIF1->_r._f11);				\
								} while(0)

#define MIF1_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
									(_v10) = (gpRegMIF1->_r._f10);				\
									(_v11) = (gpRegMIF1->_r._f11);				\
									(_v12) = (gpRegMIF1->_r._f12);				\
								} while(0)

#define MIF1_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
									(_v10) = (gpRegMIF1->_r._f10);				\
									(_v11) = (gpRegMIF1->_r._f11);				\
									(_v12) = (gpRegMIF1->_r._f12);				\
									(_v13) = (gpRegMIF1->_r._f13);				\
								} while(0)

#define MIF1_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
									(_v10) = (gpRegMIF1->_r._f10);				\
									(_v11) = (gpRegMIF1->_r._f11);				\
									(_v12) = (gpRegMIF1->_r._f12);				\
									(_v13) = (gpRegMIF1->_r._f13);				\
									(_v14) = (gpRegMIF1->_r._f14);				\
								} while(0)

#define MIF1_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
									(_v10) = (gpRegMIF1->_r._f10);				\
									(_v11) = (gpRegMIF1->_r._f11);				\
									(_v12) = (gpRegMIF1->_r._f12);				\
									(_v13) = (gpRegMIF1->_r._f13);				\
									(_v14) = (gpRegMIF1->_r._f14);				\
									(_v15) = (gpRegMIF1->_r._f15);				\
								} while(0)

#define MIF1_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegMIF1->_r._f01);				\
									(_v02) = (gpRegMIF1->_r._f02);				\
									(_v03) = (gpRegMIF1->_r._f03);				\
									(_v04) = (gpRegMIF1->_r._f04);				\
									(_v05) = (gpRegMIF1->_r._f05);				\
									(_v06) = (gpRegMIF1->_r._f06);				\
									(_v07) = (gpRegMIF1->_r._f07);				\
									(_v08) = (gpRegMIF1->_r._f08);				\
									(_v09) = (gpRegMIF1->_r._f09);				\
									(_v10) = (gpRegMIF1->_r._f10);				\
									(_v11) = (gpRegMIF1->_r._f11);				\
									(_v12) = (gpRegMIF1->_r._f12);				\
									(_v13) = (gpRegMIF1->_r._f13);				\
									(_v14) = (gpRegMIF1->_r._f14);				\
									(_v15) = (gpRegMIF1->_r._f15);				\
									(_v16) = (gpRegMIF1->_r._f16);				\
								} while(0)


#define MIF1_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
								} while(0)

#define MIF1_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
								} while(0)

#define MIF1_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
								} while(0)

#define MIF1_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
								} while(0)

#define MIF1_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
								} while(0)

#define MIF1_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
								} while(0)

#define MIF1_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
								} while(0)

#define MIF1_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
								} while(0)

#define MIF1_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
								} while(0)

#define MIF1_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
									(gpRegMIF1->_r._f10) = (_v10);				\
								} while(0)

#define MIF1_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
									(gpRegMIF1->_r._f10) = (_v10);				\
									(gpRegMIF1->_r._f11) = (_v11);				\
								} while(0)

#define MIF1_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
									(gpRegMIF1->_r._f10) = (_v10);				\
									(gpRegMIF1->_r._f11) = (_v11);				\
									(gpRegMIF1->_r._f12) = (_v12);				\
								} while(0)

#define MIF1_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
									(gpRegMIF1->_r._f10) = (_v10);				\
									(gpRegMIF1->_r._f11) = (_v11);				\
									(gpRegMIF1->_r._f12) = (_v12);				\
									(gpRegMIF1->_r._f13) = (_v13);				\
								} while(0)

#define MIF1_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
									(gpRegMIF1->_r._f10) = (_v10);				\
									(gpRegMIF1->_r._f11) = (_v11);				\
									(gpRegMIF1->_r._f12) = (_v12);				\
									(gpRegMIF1->_r._f13) = (_v13);				\
									(gpRegMIF1->_r._f14) = (_v14);				\
								} while(0)

#define MIF1_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
									(gpRegMIF1->_r._f10) = (_v10);				\
									(gpRegMIF1->_r._f11) = (_v11);				\
									(gpRegMIF1->_r._f12) = (_v12);				\
									(gpRegMIF1->_r._f13) = (_v13);				\
									(gpRegMIF1->_r._f14) = (_v14);				\
									(gpRegMIF1->_r._f15) = (_v15);				\
								} while(0)

#define MIF1_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegMIF1->_r._f01) = (_v01);				\
									(gpRegMIF1->_r._f02) = (_v02);				\
									(gpRegMIF1->_r._f03) = (_v03);				\
									(gpRegMIF1->_r._f04) = (_v04);				\
									(gpRegMIF1->_r._f05) = (_v05);				\
									(gpRegMIF1->_r._f06) = (_v06);				\
									(gpRegMIF1->_r._f07) = (_v07);				\
									(gpRegMIF1->_r._f08) = (_v08);				\
									(gpRegMIF1->_r._f09) = (_v09);				\
									(gpRegMIF1->_r._f10) = (_v10);				\
									(gpRegMIF1->_r._f11) = (_v11);				\
									(gpRegMIF1->_r._f12) = (_v12);				\
									(gpRegMIF1->_r._f13) = (_v13);				\
									(gpRegMIF1->_r._f14) = (_v14);				\
									(gpRegMIF1->_r._f15) = (_v15);				\
									(gpRegMIF1->_r._f16) = (_v16);				\
								} while(0)

/* Indexed Register Access.
 * 
 * There is in-direct field specified by 'index' field within a register.
 * Normally a register has only one meaning for a 'field_name', but indexed register
 * can hold several data for a 'field_name' specifed by 'index' field of indexed register.
 * When writing an 3rd data for given 'field_name' register, you need to set 'rw' = 0, 'index' = 2,
 * and 'load' = 0.
 * 
 * ASSUMPTION
 * For Writing indexed register load bit 
 * 
 * parameter list
 * _r     : name of register
 * _lname : name of load  bit field	: shall be 0 after macro executed.
 * _rwname: name of rw    bit field : shall be 0 after MIF1_Wind(), 1 for MIF1_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * MIF1_Rind : General indexed register Read.(
 * MIF1_Wind : General indexed register Read.
 *
 * MIF1_Ridx : For 'index', 'rw', 'load' field name
 * MIF1_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define MIF1_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								MIF1_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								MIF1_WrFL(_r);									\
								MIF1_RdFL(_r);									\
								MIF1_Rd01(_r,_fname,_fval);						\
							} while (0)

#define MIF1_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				MIF1_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define MIF1_Ridx(_r, _ival, _fname, _fval)	MIF1_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define MIF1_Widx(_r, _ival, _fname, _fval)	MIF1_Wind(_r,load,rw,index,_ival,_fname,_fval)

/*
 * @{
 * Naming for register pointer.
 * gpRealRegMIF2 : real register of MIF2.
 * gpRegMIF2     : shadow register.
 *
 * @def MIF2_RdFL: Read  FLushing : Shadow <- Real.
 * @def MIF2_WrFL: Write FLushing : Shadow -> Real.
 * @def MIF2_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def MIF2_Wr  : Write whole register(UINT32) from Shadow register.
 * @def MIF2_Rd01 ~ MIF2_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def MIF2_Wr01 ~ MIF2_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define MIF2_RdFL(_r)			((gpRegMIF2->_r)=(gpRealRegMIF2->_r))
#define MIF2_WrFL(_r)			((gpRealRegMIF2->_r)=(gpRegMIF2->_r))

#define MIF2_Rd(_r)			*((UINT32*)(&(gpRegMIF2->_r)))
#define MIF2_Wr(_r,_v)			((MIF2_Rd(_r))=((UINT32)(_v)))

#define MIF2_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
								} while(0)

#define MIF2_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
								} while(0)

#define MIF2_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
								} while(0)

#define MIF2_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
								} while(0)

#define MIF2_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
								} while(0)

#define MIF2_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
								} while(0)

#define MIF2_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
								} while(0)

#define MIF2_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
								} while(0)

#define MIF2_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
								} while(0)

#define MIF2_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
									(_v10) = (gpRegMIF2->_r._f10);				\
								} while(0)

#define MIF2_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
									(_v10) = (gpRegMIF2->_r._f10);				\
									(_v11) = (gpRegMIF2->_r._f11);				\
								} while(0)

#define MIF2_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
									(_v10) = (gpRegMIF2->_r._f10);				\
									(_v11) = (gpRegMIF2->_r._f11);				\
									(_v12) = (gpRegMIF2->_r._f12);				\
								} while(0)

#define MIF2_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
									(_v10) = (gpRegMIF2->_r._f10);				\
									(_v11) = (gpRegMIF2->_r._f11);				\
									(_v12) = (gpRegMIF2->_r._f12);				\
									(_v13) = (gpRegMIF2->_r._f13);				\
								} while(0)

#define MIF2_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
									(_v10) = (gpRegMIF2->_r._f10);				\
									(_v11) = (gpRegMIF2->_r._f11);				\
									(_v12) = (gpRegMIF2->_r._f12);				\
									(_v13) = (gpRegMIF2->_r._f13);				\
									(_v14) = (gpRegMIF2->_r._f14);				\
								} while(0)

#define MIF2_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
									(_v10) = (gpRegMIF2->_r._f10);				\
									(_v11) = (gpRegMIF2->_r._f11);				\
									(_v12) = (gpRegMIF2->_r._f12);				\
									(_v13) = (gpRegMIF2->_r._f13);				\
									(_v14) = (gpRegMIF2->_r._f14);				\
									(_v15) = (gpRegMIF2->_r._f15);				\
								} while(0)

#define MIF2_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegMIF2->_r._f01);				\
									(_v02) = (gpRegMIF2->_r._f02);				\
									(_v03) = (gpRegMIF2->_r._f03);				\
									(_v04) = (gpRegMIF2->_r._f04);				\
									(_v05) = (gpRegMIF2->_r._f05);				\
									(_v06) = (gpRegMIF2->_r._f06);				\
									(_v07) = (gpRegMIF2->_r._f07);				\
									(_v08) = (gpRegMIF2->_r._f08);				\
									(_v09) = (gpRegMIF2->_r._f09);				\
									(_v10) = (gpRegMIF2->_r._f10);				\
									(_v11) = (gpRegMIF2->_r._f11);				\
									(_v12) = (gpRegMIF2->_r._f12);				\
									(_v13) = (gpRegMIF2->_r._f13);				\
									(_v14) = (gpRegMIF2->_r._f14);				\
									(_v15) = (gpRegMIF2->_r._f15);				\
									(_v16) = (gpRegMIF2->_r._f16);				\
								} while(0)


#define MIF2_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
								} while(0)

#define MIF2_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
								} while(0)

#define MIF2_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
								} while(0)

#define MIF2_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
								} while(0)

#define MIF2_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
								} while(0)

#define MIF2_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
								} while(0)

#define MIF2_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
								} while(0)

#define MIF2_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
								} while(0)

#define MIF2_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
								} while(0)

#define MIF2_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
									(gpRegMIF2->_r._f10) = (_v10);				\
								} while(0)

#define MIF2_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
									(gpRegMIF2->_r._f10) = (_v10);				\
									(gpRegMIF2->_r._f11) = (_v11);				\
								} while(0)

#define MIF2_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
									(gpRegMIF2->_r._f10) = (_v10);				\
									(gpRegMIF2->_r._f11) = (_v11);				\
									(gpRegMIF2->_r._f12) = (_v12);				\
								} while(0)

#define MIF2_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
									(gpRegMIF2->_r._f10) = (_v10);				\
									(gpRegMIF2->_r._f11) = (_v11);				\
									(gpRegMIF2->_r._f12) = (_v12);				\
									(gpRegMIF2->_r._f13) = (_v13);				\
								} while(0)

#define MIF2_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
									(gpRegMIF2->_r._f10) = (_v10);				\
									(gpRegMIF2->_r._f11) = (_v11);				\
									(gpRegMIF2->_r._f12) = (_v12);				\
									(gpRegMIF2->_r._f13) = (_v13);				\
									(gpRegMIF2->_r._f14) = (_v14);				\
								} while(0)

#define MIF2_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
									(gpRegMIF2->_r._f10) = (_v10);				\
									(gpRegMIF2->_r._f11) = (_v11);				\
									(gpRegMIF2->_r._f12) = (_v12);				\
									(gpRegMIF2->_r._f13) = (_v13);				\
									(gpRegMIF2->_r._f14) = (_v14);				\
									(gpRegMIF2->_r._f15) = (_v15);				\
								} while(0)

#define MIF2_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegMIF2->_r._f01) = (_v01);				\
									(gpRegMIF2->_r._f02) = (_v02);				\
									(gpRegMIF2->_r._f03) = (_v03);				\
									(gpRegMIF2->_r._f04) = (_v04);				\
									(gpRegMIF2->_r._f05) = (_v05);				\
									(gpRegMIF2->_r._f06) = (_v06);				\
									(gpRegMIF2->_r._f07) = (_v07);				\
									(gpRegMIF2->_r._f08) = (_v08);				\
									(gpRegMIF2->_r._f09) = (_v09);				\
									(gpRegMIF2->_r._f10) = (_v10);				\
									(gpRegMIF2->_r._f11) = (_v11);				\
									(gpRegMIF2->_r._f12) = (_v12);				\
									(gpRegMIF2->_r._f13) = (_v13);				\
									(gpRegMIF2->_r._f14) = (_v14);				\
									(gpRegMIF2->_r._f15) = (_v15);				\
									(gpRegMIF2->_r._f16) = (_v16);				\
								} while(0)

/* Indexed Register Access.
 * 
 * There is in-direct field specified by 'index' field within a register.
 * Normally a register has only one meaning for a 'field_name', but indexed register
 * can hold several data for a 'field_name' specifed by 'index' field of indexed register.
 * When writing an 3rd data for given 'field_name' register, you need to set 'rw' = 0, 'index' = 2,
 * and 'load' = 0.
 * 
 * ASSUMPTION
 * For Writing indexed register load bit 
 * 
 * parameter list
 * _r     : name of register
 * _lname : name of load  bit field	: shall be 0 after macro executed.
 * _rwname: name of rw    bit field : shall be 0 after MIF2_Wind(), 1 for MIF2_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * MIF2_Rind : General indexed register Read.(
 * MIF2_Wind : General indexed register Read.
 *
 * MIF2_Ridx : For 'index', 'rw', 'load' field name
 * MIF2_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define MIF2_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								MIF2_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								MIF2_WrFL(_r);									\
								MIF2_RdFL(_r);									\
								MIF2_Rd01(_r,_fname,_fval);						\
							} while (0)

#define MIF2_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				MIF2_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define MIF2_Ridx(_r, _ival, _fname, _fval)	MIF2_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define MIF2_Widx(_r, _ival, _fname, _fval)	MIF2_Wind(_r,load,rw,index,_ival,_fname,_fval)

/*
 * @{
 * Naming for register pointer.
 * gpRealRegMIF3 : real register of MIF3.
 * gpRegMIF3     : shadow register.
 *
 * @def MIF3_RdFL: Read  FLushing : Shadow <- Real.
 * @def MIF3_WrFL: Write FLushing : Shadow -> Real.
 * @def MIF3_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def MIF3_Wr  : Write whole register(UINT32) from Shadow register.
 * @def MIF3_Rd01 ~ MIF3_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def MIF3_Wr01 ~ MIF3_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define MIF3_RdFL(_r)			((gpRegMIF3->_r)=(gpRealRegMIF3->_r))
#define MIF3_WrFL(_r)			((gpRealRegMIF3->_r)=(gpRegMIF3->_r))

#define MIF3_Rd(_r)			*((UINT32*)(&(gpRegMIF3->_r)))
#define MIF3_Wr(_r,_v)			((MIF3_Rd(_r))=((UINT32)(_v)))

#define MIF3_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
								} while(0)

#define MIF3_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
								} while(0)

#define MIF3_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
								} while(0)

#define MIF3_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
								} while(0)

#define MIF3_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
								} while(0)

#define MIF3_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
								} while(0)

#define MIF3_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
								} while(0)

#define MIF3_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
								} while(0)

#define MIF3_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
								} while(0)

#define MIF3_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
									(_v10) = (gpRegMIF3->_r._f10);				\
								} while(0)

#define MIF3_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
									(_v10) = (gpRegMIF3->_r._f10);				\
									(_v11) = (gpRegMIF3->_r._f11);				\
								} while(0)

#define MIF3_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
									(_v10) = (gpRegMIF3->_r._f10);				\
									(_v11) = (gpRegMIF3->_r._f11);				\
									(_v12) = (gpRegMIF3->_r._f12);				\
								} while(0)

#define MIF3_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
									(_v10) = (gpRegMIF3->_r._f10);				\
									(_v11) = (gpRegMIF3->_r._f11);				\
									(_v12) = (gpRegMIF3->_r._f12);				\
									(_v13) = (gpRegMIF3->_r._f13);				\
								} while(0)

#define MIF3_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
									(_v10) = (gpRegMIF3->_r._f10);				\
									(_v11) = (gpRegMIF3->_r._f11);				\
									(_v12) = (gpRegMIF3->_r._f12);				\
									(_v13) = (gpRegMIF3->_r._f13);				\
									(_v14) = (gpRegMIF3->_r._f14);				\
								} while(0)

#define MIF3_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
									(_v10) = (gpRegMIF3->_r._f10);				\
									(_v11) = (gpRegMIF3->_r._f11);				\
									(_v12) = (gpRegMIF3->_r._f12);				\
									(_v13) = (gpRegMIF3->_r._f13);				\
									(_v14) = (gpRegMIF3->_r._f14);				\
									(_v15) = (gpRegMIF3->_r._f15);				\
								} while(0)

#define MIF3_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegMIF3->_r._f01);				\
									(_v02) = (gpRegMIF3->_r._f02);				\
									(_v03) = (gpRegMIF3->_r._f03);				\
									(_v04) = (gpRegMIF3->_r._f04);				\
									(_v05) = (gpRegMIF3->_r._f05);				\
									(_v06) = (gpRegMIF3->_r._f06);				\
									(_v07) = (gpRegMIF3->_r._f07);				\
									(_v08) = (gpRegMIF3->_r._f08);				\
									(_v09) = (gpRegMIF3->_r._f09);				\
									(_v10) = (gpRegMIF3->_r._f10);				\
									(_v11) = (gpRegMIF3->_r._f11);				\
									(_v12) = (gpRegMIF3->_r._f12);				\
									(_v13) = (gpRegMIF3->_r._f13);				\
									(_v14) = (gpRegMIF3->_r._f14);				\
									(_v15) = (gpRegMIF3->_r._f15);				\
									(_v16) = (gpRegMIF3->_r._f16);				\
								} while(0)


#define MIF3_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
								} while(0)

#define MIF3_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
								} while(0)

#define MIF3_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
								} while(0)

#define MIF3_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
								} while(0)

#define MIF3_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
								} while(0)

#define MIF3_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
								} while(0)

#define MIF3_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
								} while(0)

#define MIF3_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
								} while(0)

#define MIF3_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
								} while(0)

#define MIF3_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
									(gpRegMIF3->_r._f10) = (_v10);				\
								} while(0)

#define MIF3_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
									(gpRegMIF3->_r._f10) = (_v10);				\
									(gpRegMIF3->_r._f11) = (_v11);				\
								} while(0)

#define MIF3_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
									(gpRegMIF3->_r._f10) = (_v10);				\
									(gpRegMIF3->_r._f11) = (_v11);				\
									(gpRegMIF3->_r._f12) = (_v12);				\
								} while(0)

#define MIF3_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
									(gpRegMIF3->_r._f10) = (_v10);				\
									(gpRegMIF3->_r._f11) = (_v11);				\
									(gpRegMIF3->_r._f12) = (_v12);				\
									(gpRegMIF3->_r._f13) = (_v13);				\
								} while(0)

#define MIF3_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
									(gpRegMIF3->_r._f10) = (_v10);				\
									(gpRegMIF3->_r._f11) = (_v11);				\
									(gpRegMIF3->_r._f12) = (_v12);				\
									(gpRegMIF3->_r._f13) = (_v13);				\
									(gpRegMIF3->_r._f14) = (_v14);				\
								} while(0)

#define MIF3_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
									(gpRegMIF3->_r._f10) = (_v10);				\
									(gpRegMIF3->_r._f11) = (_v11);				\
									(gpRegMIF3->_r._f12) = (_v12);				\
									(gpRegMIF3->_r._f13) = (_v13);				\
									(gpRegMIF3->_r._f14) = (_v14);				\
									(gpRegMIF3->_r._f15) = (_v15);				\
								} while(0)

#define MIF3_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegMIF3->_r._f01) = (_v01);				\
									(gpRegMIF3->_r._f02) = (_v02);				\
									(gpRegMIF3->_r._f03) = (_v03);				\
									(gpRegMIF3->_r._f04) = (_v04);				\
									(gpRegMIF3->_r._f05) = (_v05);				\
									(gpRegMIF3->_r._f06) = (_v06);				\
									(gpRegMIF3->_r._f07) = (_v07);				\
									(gpRegMIF3->_r._f08) = (_v08);				\
									(gpRegMIF3->_r._f09) = (_v09);				\
									(gpRegMIF3->_r._f10) = (_v10);				\
									(gpRegMIF3->_r._f11) = (_v11);				\
									(gpRegMIF3->_r._f12) = (_v12);				\
									(gpRegMIF3->_r._f13) = (_v13);				\
									(gpRegMIF3->_r._f14) = (_v14);				\
									(gpRegMIF3->_r._f15) = (_v15);				\
									(gpRegMIF3->_r._f16) = (_v16);				\
								} while(0)

/* Indexed Register Access.
 * 
 * There is in-direct field specified by 'index' field within a register.
 * Normally a register has only one meaning for a 'field_name', but indexed register
 * can hold several data for a 'field_name' specifed by 'index' field of indexed register.
 * When writing an 3rd data for given 'field_name' register, you need to set 'rw' = 0, 'index' = 2,
 * and 'load' = 0.
 * 
 * ASSUMPTION
 * For Writing indexed register load bit 
 * 
 * parameter list
 * _r     : name of register
 * _lname : name of load  bit field	: shall be 0 after macro executed.
 * _rwname: name of rw    bit field : shall be 0 after MIF3_Wind(), 1 for MIF3_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * MIF3_Rind : General indexed register Read.(
 * MIF3_Wind : General indexed register Read.
 *
 * MIF3_Ridx : For 'index', 'rw', 'load' field name
 * MIF3_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define MIF3_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								MIF3_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								MIF3_WrFL(_r);									\
								MIF3_RdFL(_r);									\
								MIF3_Rd01(_r,_fname,_fval);						\
							} while (0)

#define MIF3_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				MIF3_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define MIF3_Ridx(_r, _ival, _fname, _fval)	MIF3_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define MIF3_Widx(_r, _ival, _fname, _fval)	MIF3_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8_DE_MIF_RegFile_100127b.csv' 20100316 14:34:48   대한민국 표준시 by getregs v2.3 */
