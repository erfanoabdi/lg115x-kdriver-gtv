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


/*-----------------------------------------------------------------------------
	0x1a00 osd0_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_en                         : 1,	//     0
	                                : 3,	//  1: 3 reserved
	osd0_round                      : 3,	//  4: 6
	osd0_pure_ck                    : 1,	//     7
	osd0_hdr_ptr_bmp_sel            : 1,	//     8
	osd0_hdr_ptr_plte_sel           : 1,	//     9
	osd0_hdr_color_key_sel          : 1,	//    10
	osd0_hdr_wpl_sel                : 1,	//    11
	osd0_hdr_global_alpha_sel       : 1,	//    12
	osd0_hdr_format_sel             : 1,	//    13
	osd0_hdr_depth_sel              : 1,	//    14
	osd0_hdr_h_out_sel              : 1,	//    15
	osd0_hdr_w_out_sel              : 1,	//    16
	osd0_hdr_global_alpha_en_sel    : 1,	//    17
	osd0_hdr_pixel_order_sel        : 1,	//    18
	osd0_hdr_h_mem_sel              : 1,	//    19
	osd0_hdr_w_mem_sel              : 1,	//    20
	osd0_hdr_ypos_sel               : 1,	//    21
	osd0_hdr_xpos_sel               : 1,	//    22
	osd0_hdr_color_key_en_sel       : 1,	//    23
	osd0_ch_blue                    : 2,	// 24:25
	osd0_ch_green                   : 2,	// 26:27
	osd0_ch_red                     : 2,	// 28:29
	osd0_ch_alpha                   : 2;	// 30:31
} OSD0_CFG;

/*-----------------------------------------------------------------------------
	0x1a04 osd1_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_en                         : 1,	//     0
	                                : 3,	//  1: 3 reserved
	osd1_round                      : 3,	//  4: 6
	osd1_pure_ck                    : 1,	//     7
	osd1_hdr_ptr_bmp_sel            : 1,	//     8
	osd1_hdr_ptr_plte_sel           : 1,	//     9
	osd1_hdr_color_key_sel          : 1,	//    10
	osd1_hdr_wpl_sel                : 1,	//    11
	osd1_hdr_global_alpha_sel       : 1,	//    12
	osd1_hdr_format_sel             : 1,	//    13
	osd1_hdr_depth_sel              : 1,	//    14
	osd1_hdr_h_out_sel              : 1,	//    15
	osd1_hdr_w_out_sel              : 1,	//    16
	osd1_hdr_global_alpha_en_sel    : 1,	//    17
	osd1_hdr_pixel_order_sel        : 1,	//    18
	osd1_hdr_h_mem_sel              : 1,	//    19
	osd1_hdr_w_mem_sel              : 1,	//    20
	osd1_hdr_ypos_sel               : 1,	//    21
	osd1_hdr_xpos_sel               : 1,	//    22
	osd1_hdr_color_key_en_sel       : 1,	//    23
	osd1_ch_blue                    : 2,	// 24:25
	osd1_ch_green                   : 2,	// 26:27
	osd1_ch_red                     : 2,	// 28:29
	osd1_ch_alpha                   : 2;	// 30:31
} OSD1_CFG;

/*-----------------------------------------------------------------------------
	0x1a08 osd2_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_en                         : 1,	//     0
	                                : 3,	//  1: 3 reserved
	osd2_round                      : 3,	//  4: 6
	osd2_pure_ck                    : 1,	//     7
	osd2_hdr_ptr_bmp_sel            : 1,	//     8
	osd2_hdr_ptr_plte_sel           : 1,	//     9
	osd2_hdr_color_key_sel          : 1,	//    10
	osd2_hdr_wpl_sel                : 1,	//    11
	osd2_hdr_global_alpha_sel       : 1,	//    12
	osd2_hdr_format_sel             : 1,	//    13
	osd2_hdr_depth_sel              : 1,	//    14
	osd2_hdr_h_out_sel              : 1,	//    15
	osd2_hdr_w_out_sel              : 1,	//    16
	osd2_hdr_global_alpha_en_sel    : 1,	//    17
	osd2_hdr_pixel_order_sel        : 1,	//    18
	osd2_hdr_h_mem_sel              : 1,	//    19
	osd2_hdr_w_mem_sel              : 1,	//    20
	osd2_hdr_ypos_sel               : 1,	//    21
	osd2_hdr_xpos_sel               : 1,	//    22
	osd2_hdr_color_key_en_sel       : 1,	//    23
	osd2_ch_blue                    : 2,	// 24:25
	osd2_ch_green                   : 2,	// 26:27
	osd2_ch_red                     : 2,	// 28:29
	osd2_ch_alpha                   : 2;	// 30:31
} OSD2_CFG;

/*-----------------------------------------------------------------------------
	0x1a0c osd_awb ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_awb_blue                   : 1,	//     0
	crsr_awb_green                  : 1,	//     1
	crsr_awb_red                    : 1,	//     2
	                                : 5,	//  3: 7 reserved
	osd2_awb_blue                   : 1,	//     8
	osd2_awb_green                  : 1,	//     9
	osd2_awb_red                    : 1,	//    10
	                                : 5,	// 11:15 reserved
	osd1_awb_blue                   : 1,	//    16
	osd1_awb_green                  : 1,	//    17
	osd1_awb_red                    : 1,	//    18
	                                : 5,	// 19:23 reserved
	osd0_awb_blue                   : 1,	//    24
	osd0_awb_green                  : 1,	//    25
	osd0_awb_red                    : 1;	//    26
} OSD_AWB;

/*-----------------------------------------------------------------------------
	0x1a10 osd_disp_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	disp_h                          :12,	//  0:11
	                                : 4,	// 12:15 reserved
	disp_w                          :11;	// 16:26
} OSD_DISP_SIZE;

/*-----------------------------------------------------------------------------
	0x1a14 osd_irq ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_irq                        : 1,	//     0
	osd2_irq                        : 1,	//     1
	osd1_irq                        : 1,	//     2
	osd0_irq                        : 1,	//     3
	mixer_irq                       : 1,	//     4
	                                :11,	//  5:15 reserved
	crsr_irq_en                     : 1,	//    16
	osd2_irq_en                     : 1,	//    17
	osd1_irq_en                     : 1,	//    18
	osd0_irq_en                     : 1,	//    19
	mixer_irq_en                    : 1;	//    20
} OSD_IRQ;

/*-----------------------------------------------------------------------------
	0x1a18 osd0_base_addr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_base_addr                  ;   	// 31: 0
} OSD0_BASE_ADDR;

/*-----------------------------------------------------------------------------
	0x1a1c osd0_hdr0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_hdr_color_key_en           :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd0_hdr_xpos                   :11,	// 16:26
	                                : 4,	// 27:30 reserved
	osd0_hdr_ypos                   : 1;	//    31
} OSD0_HDR0;

/*-----------------------------------------------------------------------------
	0x1a20 osd0_hdr1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_hdr_h_mem                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd0_hdr_w_mem                  :11;	// 16:26
} OSD0_HDR1;

/*-----------------------------------------------------------------------------
	0x1a24 osd0_hdr2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_hdr_h_out                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd0_hdr_w_out                  :11,	// 16:26
	                                : 4,	// 27:30 reserved
	osd0_hdr_pixel_order            : 1;	//    31
} OSD0_HDR2;

/*-----------------------------------------------------------------------------
	0x1a28 osd0_hdr3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_hdr_wpl                    :16,	//  0:15
	osd0_hdr_global_alpha           : 8,	// 16:23
	osd0_hdr_format                 : 4,	// 24:27
	osd0_hdr_depth                  : 3,	// 28:30
	osd0_hdr_global_alpha_en        : 1;	//    31
} OSD0_HDR3;

/*-----------------------------------------------------------------------------
	0x1a2c osd0_hdr4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_hdr_color_key              ;   	// 31: 0
} OSD0_HDR4;

/*-----------------------------------------------------------------------------
	0x1a30 osd0_hdr5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_hdr_ptr_plte               ;   	// 31: 0
} OSD0_HDR5;

/*-----------------------------------------------------------------------------
	0x1a34 osd0_hdr6 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_hdr_ptr_bmp                ;   	// 31: 0
} OSD0_HDR6;

/*-----------------------------------------------------------------------------
	0x1a38 osd0_lfsr_seed ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_lfsr_seed                  :19;	//  0:18
} OSD0_LFSR_SEED;

/*-----------------------------------------------------------------------------
	0x1a3c osd1_base_addr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_base_addr                  ;   	// 31: 0
} OSD1_BASE_ADDR;

/*-----------------------------------------------------------------------------
	0x1a40 osd1_hdr0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_hdr_color_key_en           :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd1_hdr_xpos                   :11,	// 16:26
	                                : 4,	// 27:30 reserved
	osd1_hdr_ypos                   : 1;	//    31
} OSD1_HDR0;

/*-----------------------------------------------------------------------------
	0x1a44 osd1_hdr1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_hdr_h_mem                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd1_hdr_w_mem                  :11;	// 16:26
} OSD1_HDR1;

/*-----------------------------------------------------------------------------
	0x1a48 osd1_hdr2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_hdr_h_out                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd1_hdr_w_out                  :11,	// 16:26
	                                : 4,	// 27:30 reserved
	osd1_hdr_pixel_order            : 1;	//    31
} OSD1_HDR2;

/*-----------------------------------------------------------------------------
	0x1a4c osd1_hdr3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_hdr_wpl                    :16,	//  0:15
	osd1_hdr_global_alpha           : 8,	// 16:23
	osd1_hdr_format                 : 4,	// 24:27
	osd1_hdr_depth                  : 3,	// 28:30
	osd1_hdr_global_alpha_en        : 1;	//    31
} OSD1_HDR3;

/*-----------------------------------------------------------------------------
	0x1a50 osd1_hdr4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_hdr_color_key              ;   	// 31: 0
} OSD1_HDR4;

/*-----------------------------------------------------------------------------
	0x1a54 osd1_hdr5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_hdr_ptr_plte               ;   	// 31: 0
} OSD1_HDR5;

/*-----------------------------------------------------------------------------
	0x1a58 osd1_hdr6 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_hdr_ptr_bmp                ;   	// 31: 0
} OSD1_HDR6;

/*-----------------------------------------------------------------------------
	0x1a5c osd1_lfsr_seed ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_lfsr_seed                  :19;	//  0:18
} OSD1_LFSR_SEED;

/*-----------------------------------------------------------------------------
	0x1a60 osd2_base_addr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_base_addr                  ;   	// 31: 0
} OSD2_BASE_ADDR;

/*-----------------------------------------------------------------------------
	0x1a64 osd2_hdr0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_hdr_color_key_en           :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd2_hdr_xpos                   :11,	// 16:26
	                                : 4,	// 27:30 reserved
	osd2_hdr_ypos                   : 1;	//    31
} OSD2_HDR0;

/*-----------------------------------------------------------------------------
	0x1a68 osd2_hdr1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_hdr_h_mem                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd2_hdr_w_mem                  :11;	// 16:26
} OSD2_HDR1;

/*-----------------------------------------------------------------------------
	0x1a6c osd2_hdr2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_hdr_h_out                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd2_hdr_w_out                  :11,	// 16:26
	                                : 4,	// 27:30 reserved
	osd2_hdr_pixel_order            : 1;	//    31
} OSD2_HDR2;

/*-----------------------------------------------------------------------------
	0x1a70 osd2_hdr3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_hdr_wpl                    :16,	//  0:15
	osd2_hdr_global_alpha           : 8,	// 16:23
	osd2_hdr_format                 : 4,	// 24:27
	osd2_hdr_depth                  : 3,	// 28:30
	osd2_hdr_global_alpha_en        : 1;	//    31
} OSD2_HDR3;

/*-----------------------------------------------------------------------------
	0x1a74 osd2_hdr4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_hdr_color_key              ;   	// 31: 0
} OSD2_HDR4;

/*-----------------------------------------------------------------------------
	0x1a78 osd2_hdr5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_hdr_ptr_plte               ;   	// 31: 0
} OSD2_HDR5;

/*-----------------------------------------------------------------------------
	0x1a7c osd2_hdr6 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_hdr_ptr_bmp                ;   	// 31: 0
} OSD2_HDR6;

/*-----------------------------------------------------------------------------
	0x1a80 osd2_lfsr_seed ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_lfsr_seed                  :19;	//  0:18
} OSD2_LFSR_SEED;

/*-----------------------------------------------------------------------------
	0x1a84 osd0_sc_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_sc_vs_en                   : 1,	//     0
	osd0_sc_hs_en                   : 1,	//     1
	                                : 2,	//  2: 3 reserved
	osd0_sc_bc_sa                   : 3,	//  4: 6
	                                : 1,	//     7 reserved
	osd0_sc_bnd_mode                : 1,	//     8
	                                : 3,	//  9:11 reserved
	osd0_sc_h2v_path                : 1,	//    12
	                                : 3,	// 13:15 reserved
	osd0_sc_h_phase_off             :12,	// 16:27
	osd0_sc_buf_mode_en             : 1;	//    28
} OSD0_SC_CFG;

/*-----------------------------------------------------------------------------
	0x1a88 osd1_sc_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_sc_vs_en                   : 1,	//     0
	osd1_sc_hs_en                   : 1,	//     1
	                                : 2,	//  2: 3 reserved
	osd1_sc_bc_sa                   : 3,	//  4: 6
	                                : 1,	//     7 reserved
	osd1_sc_bnd_mode                : 1,	//     8
	                                : 3,	//  9:11 reserved
	osd1_sc_h2v_path                : 1,	//    12
	                                : 3,	// 13:15 reserved
	osd1_sc_h_phase_off             :12,	// 16:27
	osd1_sc_buf_mode_en             : 1;	//    28
} OSD1_SC_CFG;

/*-----------------------------------------------------------------------------
	0x1a8c osd2_sc_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_sc_vs_en                   : 1,	//     0
	osd2_sc_hs_en                   : 1,	//     1
	                                : 2,	//  2: 3 reserved
	osd2_sc_bc_sa                   : 3,	//  4: 6
	                                : 1,	//     7 reserved
	osd2_sc_bnd_mode                : 1,	//     8
	                                : 3,	//  9:11 reserved
	osd2_sc_h2v_path                : 1,	//    12
	                                : 3,	// 13:15 reserved
	osd2_sc_h_phase_off             :12,	// 16:27
	osd2_sc_buf_mode_en             : 1;	//    28
} OSD2_SC_CFG;

/*-----------------------------------------------------------------------------
	0x1a90 osd_sc_v_phase_off ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd_sc_v_phase_off              :13,	//  0:12
	                                : 3,	// 13:15 reserved
	osd_sc_v_phase_off_index        : 2,	// 16:17
	                                :13,	// 18:30 reserved
	osd_sc_v_phase_off_rw           : 1;	//    31
} OSD_SC_V_PHASE_OFF;

/*-----------------------------------------------------------------------------
	0x1a94 osd0_csc_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_csc_data                   :11,	//  0:10
	osd0_csc_rw_n                   : 1,	//    11
	                                : 4,	// 12:15 reserved
	osd0_csc_bypass                 : 1,	//    16
	                                : 3,	// 17:19 reserved
	osd0_csc_sel                    : 1,	//    20
	                                : 3,	// 21:23 reserved
	osd0_csc_out_ctrl               : 3,	// 24:26
	                                : 1,	//    27 reserved
	osd0_csc_in_ctrl                : 3;	// 28:30
} OSD0_CSC_CTRL;

/*-----------------------------------------------------------------------------
	0x1a98 osd1_csc_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_csc_data                   :11,	//  0:10
	osd1_csc_rw_n                   : 1,	//    11
	                                : 4,	// 12:15 reserved
	osd1_csc_bypass                 : 1,	//    16
	                                : 3,	// 17:19 reserved
	osd1_csc_sel                    : 1,	//    20
	                                : 3,	// 21:23 reserved
	osd1_csc_out_ctrl               : 3,	// 24:26
	                                : 1,	//    27 reserved
	osd1_csc_in_ctrl                : 3;	// 28:30
} OSD1_CSC_CTRL;

/*-----------------------------------------------------------------------------
	0x1a9c osd2_csc_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_csc_data                   :11,	//  0:10
	osd2_csc_rw_n                   : 1,	//    11
	                                : 4,	// 12:15 reserved
	osd2_csc_bypass                 : 1,	//    16
	                                : 3,	// 17:19 reserved
	osd2_csc_sel                    : 1,	//    20
	                                : 3,	// 21:23 reserved
	osd2_csc_out_ctrl               : 3,	// 24:26
	                                : 1,	//    27 reserved
	osd2_csc_in_ctrl                : 3;	// 28:30
} OSD2_CSC_CTRL;

/*-----------------------------------------------------------------------------
	0x1aa4 osd0_end_conv ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_bbs                        : 1,	//     0
	osd0_bqws                       : 1,	//     1
	osd0_bhws                       : 1,	//     2
	                                : 1,	//     3 reserved
	osd0_pbs                        : 1,	//     4
	osd0_pqws                       : 1,	//     5
	osd0_phws                       : 1,	//     6
	                                : 1,	//     7 reserved
	osd0_hbs                        : 1,	//     8
	osd0_hqws                       : 1,	//     9
	osd0_hhws                       : 1;	//    10
} OSD0_END_CONV;

/*-----------------------------------------------------------------------------
	0x1aa8 osd1_end_conv ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_bbs                        : 1,	//     0
	osd1_bqws                       : 1,	//     1
	osd1_bhws                       : 1,	//     2
	                                : 1,	//     3 reserved
	osd1_pbs                        : 1,	//     4
	osd1_pqws                       : 1,	//     5
	osd1_phws                       : 1,	//     6
	                                : 1,	//     7 reserved
	osd1_hbs                        : 1,	//     8
	osd1_hqws                       : 1,	//     9
	osd1_hhws                       : 1;	//    10
} OSD1_END_CONV;

/*-----------------------------------------------------------------------------
	0x1aac osd2_end_conv ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_bbs                        : 1,	//     0
	osd2_bqws                       : 1,	//     1
	osd2_bhws                       : 1,	//     2
	                                : 1,	//     3 reserved
	osd2_pbs                        : 1,	//     4
	osd2_pqws                       : 1,	//     5
	osd2_phws                       : 1,	//     6
	                                : 1,	//     7 reserved
	osd2_hbs                        : 1,	//     8
	osd2_hqws                       : 1,	//     9
	osd2_hhws                       : 1;	//    10
} OSD2_END_CONV;

/*-----------------------------------------------------------------------------
	0x1ab4 crsr_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_en                         : 1,	//     0
	crsr_double                     : 1,	//     1
	crsr_manual_address             : 1,	//     2
	crsr_manual_stride              : 1,	//     3
	crsr_round                      : 3,	//  4: 6
	crsr_pure_color_key             : 1,	//     7
	crsr_hdr_ptr_bmp_sel            : 1,	//     8
	crsr_hdr_ptr_plte_sel           : 1,	//     9
	crsr_hdr_color_key_sel          : 1,	//    10
	crsr_hdr_wpl_sel                : 1,	//    11
	crsr_hdr_global_alpha_sel       : 1,	//    12
	crsr_hdr_format_sel             : 1,	//    13
	crsr_hdr_depth_sel              : 1,	//    14
	                                : 2,	// 15:16 reserved
	crsr_hdr_global_alpha_en_sel    : 1,	//    17
	crsr_hdr_pixel_order_sel        : 1,	//    18
	crsr_hdr_h_mem_sel              : 1,	//    19
	crsr_hdr_w_mem_sel              : 1,	//    20
	crsr_hdr_ypos_sel               : 1,	//    21
	crsr_hdr_xpos_sel               : 1,	//    22
	crsr_hdr_color_key_en_sel       : 1,	//    23
	crsr_ch_blue                    : 2,	// 24:25
	crsr_ch_green                   : 2,	// 26:27
	crsr_ch_red                     : 2,	// 28:29
	crsr_ch_alpha                   : 2;	// 30:31
} CRSR_CFG;

/*-----------------------------------------------------------------------------
	0x1ab8 crsr_base_addr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_base_addr                  ;   	// 31: 0
} CRSR_BASE_ADDR;

/*-----------------------------------------------------------------------------
	0x1abc crsr_hdr0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_hdr_ypos                   :12,	//  0:11
	                                : 4,	// 12:15 reserved
	crsr_hdr_xpos                   :11,	// 16:26
	                                : 4,	// 27:30 reserved
	crsr_hdr_pos_load               : 1;	//    31
} CRSR_HDR0;

/*-----------------------------------------------------------------------------
	0x1ac0 crsr_hdr1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_hdr_h_mem                  :12,	//  0:11
	                                : 3,	// 12:14 reserved
	crsr_hdr_pixel_order            : 1,	//    15
	crsr_hdr_w_mem                  :11,	// 16:26
	                                : 4,	// 27:30 reserved
	crsr_hdr_color_key_en           : 1;	//    31
} CRSR_HDR1;

/*-----------------------------------------------------------------------------
	0x1ac4 crsr_hdr2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_hdr_wpl                    :16,	//  0:15
	crsr_hdr_global_alpha           : 8,	// 16:23
	crsr_hdr_format                 : 4,	// 24:27
	crsr_hdr_depth                  : 3,	// 28:30
	crsr_hdr_global_alpha_en        : 1;	//    31
} CRSR_HDR2;

/*-----------------------------------------------------------------------------
	0x1ac8 crsr_hdr3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_hdr_color_key              ;   	// 31: 0
} CRSR_HDR3;

/*-----------------------------------------------------------------------------
	0x1acc crsr_hdr4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_hdr_ptr_plte               ;   	// 31: 0
} CRSR_HDR4;

/*-----------------------------------------------------------------------------
	0x1ad0 crsr_hdr5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_hdr_ptr_bmp                ;   	// 31: 0
} CRSR_HDR5;

/*-----------------------------------------------------------------------------
	0x1ad4 crsr_end_conv ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_bbs                        : 1,	//     0
	crsr_bqws                       : 1,	//     1
	crsr_bhws                       : 1,	//     2
	                                : 1,	//     3 reserved
	crsr_pbs                        : 1,	//     4
	crsr_pqws                       : 1,	//     5
	crsr_phws                       : 1,	//     6
	                                : 1,	//     7 reserved
	crsr_hbs                        : 1,	//     8
	crsr_hqws                       : 1,	//     9
	crsr_hhws                       : 1;	//    10
} CRSR_END_CONV;

/*-----------------------------------------------------------------------------
	0x1ad8 crsr_lfsr_seed ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_lfsr_seed                  :19;	//  0:18
} CRSR_LFSR_SEED;

/*-----------------------------------------------------------------------------
	0x1adc osd_ddr_addressing ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd_layer_sel                   : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	op_mode                         : 1,	//     4
	                                : 4,	//  5: 8 reserved
	osd_ddr_sel_bmp                 : 3,	//  9:11
	                                : 5,	// 12:16 reserved
	osd_ddr_sel_plt                 : 3,	// 17:19
	                                : 5,	// 20:24 reserved
	osd_ddr_sel_hdr                 : 3;	// 25:27
} OSD_DDR_ADDRESSING;

/*-----------------------------------------------------------------------------
	0x1ae0 osd_layer_mux ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	layer_mux_3                     : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	layer_mux_2                     : 2,	//  4: 5
	                                : 2,	//  6: 7 reserved
	layer_mux_1                     : 2,	//  8: 9
	                                : 2,	// 10:11 reserved
	layer_mux_0                     : 2,	// 12:13
	                                : 2,	// 14:15 reserved
	video_layer_alpha               : 8;	// 16:23
} OSD_LAYER_MUX;

/*-----------------------------------------------------------------------------
	0x1ae4 osd_mixer_out ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mixer_out_blue                  :10,	//  0: 9
	mixer_out_green                 :10,	// 10:19
	mixer_out_red                   :10,	// 20:29
	mixer_mask_en                   : 1,	//    30
	mixer_out_en                    : 1;	//    31
} OSD_MIXER_OUT;

/*-----------------------------------------------------------------------------
	0x1ae8 osd_mixer_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mixer_process_line              :12;	//  0:11
} OSD_MIXER_STATUS;

/*-----------------------------------------------------------------------------
	0x1aec osd_status_cfg ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd_status_cfg                  : 4;	//  0: 3
} OSD_STATUS_CFG;

/*-----------------------------------------------------------------------------
	0x1af0 osd_status_0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd0_process_state              :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd0_process_line               : 3;	// 16:18
} OSD_STATUS_0;

/*-----------------------------------------------------------------------------
	0x1af4 osd_status_1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd1_process_state              :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd1_process_line               : 3;	// 16:18
} OSD_STATUS_1;

/*-----------------------------------------------------------------------------
	0x1af8 osd_status_2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	osd2_process_state              :12,	//  0:11
	                                : 4,	// 12:15 reserved
	osd2_process_line               : 3;	// 16:18
} OSD_STATUS_2;

/*-----------------------------------------------------------------------------
	0x1afc osd_status_3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	crsr_process_state              :12,	//  0:11
	                                : 4,	// 12:15 reserved
	crsr_process_line               : 3;	// 16:18
} OSD_STATUS_3;

typedef struct {
	OSD0_CFG                        	osd0_cfg                        ;	// 0x1a00 : ''
	OSD1_CFG                        	osd1_cfg                        ;	// 0x1a04 : ''
	OSD2_CFG                        	osd2_cfg                        ;	// 0x1a08 : ''
	OSD_AWB                         	osd_awb                         ;	// 0x1a0c : ''
	OSD_DISP_SIZE                   	osd_disp_size                   ;	// 0x1a10 : ''
	OSD_IRQ                         	osd_irq                         ;	// 0x1a14 : ''
	OSD0_BASE_ADDR                  	osd0_base_addr                  ;	// 0x1a18 : ''
	OSD0_HDR0                       	osd0_hdr0                       ;	// 0x1a1c : ''
	OSD0_HDR1                       	osd0_hdr1                       ;	// 0x1a20 : ''
	OSD0_HDR2                       	osd0_hdr2                       ;	// 0x1a24 : ''
	OSD0_HDR3                       	osd0_hdr3                       ;	// 0x1a28 : ''
	OSD0_HDR4                       	osd0_hdr4                       ;	// 0x1a2c : ''
	OSD0_HDR5                       	osd0_hdr5                       ;	// 0x1a30 : ''
	OSD0_HDR6                       	osd0_hdr6                       ;	// 0x1a34 : ''
	OSD0_LFSR_SEED                  	osd0_lfsr_seed                  ;	// 0x1a38 : ''
	OSD1_BASE_ADDR                  	osd1_base_addr                  ;	// 0x1a3c : ''
	OSD1_HDR0                       	osd1_hdr0                       ;	// 0x1a40 : ''
	OSD1_HDR1                       	osd1_hdr1                       ;	// 0x1a44 : ''
	OSD1_HDR2                       	osd1_hdr2                       ;	// 0x1a48 : ''
	OSD1_HDR3                       	osd1_hdr3                       ;	// 0x1a4c : ''
	OSD1_HDR4                       	osd1_hdr4                       ;	// 0x1a50 : ''
	OSD1_HDR5                       	osd1_hdr5                       ;	// 0x1a54 : ''
	OSD1_HDR6                       	osd1_hdr6                       ;	// 0x1a58 : ''
	OSD1_LFSR_SEED                  	osd1_lfsr_seed                  ;	// 0x1a5c : ''
	OSD2_BASE_ADDR                  	osd2_base_addr                  ;	// 0x1a60 : ''
	OSD2_HDR0                       	osd2_hdr0                       ;	// 0x1a64 : ''
	OSD2_HDR1                       	osd2_hdr1                       ;	// 0x1a68 : ''
	OSD2_HDR2                       	osd2_hdr2                       ;	// 0x1a6c : ''
	OSD2_HDR3                       	osd2_hdr3                       ;	// 0x1a70 : ''
	OSD2_HDR4                       	osd2_hdr4                       ;	// 0x1a74 : ''
	OSD2_HDR5                       	osd2_hdr5                       ;	// 0x1a78 : ''
	OSD2_HDR6                       	osd2_hdr6                       ;	// 0x1a7c : ''
	OSD2_LFSR_SEED                  	osd2_lfsr_seed                  ;	// 0x1a80 : ''
	OSD0_SC_CFG                     	osd0_sc_cfg                     ;	// 0x1a84 : ''
	OSD1_SC_CFG                     	osd1_sc_cfg                     ;	// 0x1a88 : ''
	OSD2_SC_CFG                     	osd2_sc_cfg                     ;	// 0x1a8c : ''
	OSD_SC_V_PHASE_OFF              	osd_sc_v_phase_off              ;	// 0x1a90 : ''
	OSD0_CSC_CTRL                   	osd0_csc_ctrl                   ;	// 0x1a94 : ''
	OSD1_CSC_CTRL                   	osd1_csc_ctrl                   ;	// 0x1a98 : ''
	OSD2_CSC_CTRL                   	osd2_csc_ctrl                   ;	// 0x1a9c : ''
	UINT32                          	                 __rsvd_00[   1];	// 0x1aa0
	OSD0_END_CONV                   	osd0_end_conv                   ;	// 0x1aa4 : ''
	OSD1_END_CONV                   	osd1_end_conv                   ;	// 0x1aa8 : ''
	OSD2_END_CONV                   	osd2_end_conv                   ;	// 0x1aac : ''
	UINT32                          	                 __rsvd_01[   1];	// 0x1ab0
	CRSR_CFG                        	crsr_cfg                        ;	// 0x1ab4 : ''
	CRSR_BASE_ADDR                  	crsr_base_addr                  ;	// 0x1ab8 : ''
	CRSR_HDR0                       	crsr_hdr0                       ;	// 0x1abc : ''
	CRSR_HDR1                       	crsr_hdr1                       ;	// 0x1ac0 : ''
	CRSR_HDR2                       	crsr_hdr2                       ;	// 0x1ac4 : ''
	CRSR_HDR3                       	crsr_hdr3                       ;	// 0x1ac8 : ''
	CRSR_HDR4                       	crsr_hdr4                       ;	// 0x1acc : ''
	CRSR_HDR5                       	crsr_hdr5                       ;	// 0x1ad0 : ''
	CRSR_END_CONV                   	crsr_end_conv                   ;	// 0x1ad4 : ''
	CRSR_LFSR_SEED                  	crsr_lfsr_seed                  ;	// 0x1ad8 : ''
	OSD_DDR_ADDRESSING              	osd_ddr_addressing              ;	// 0x1adc : ''
	OSD_LAYER_MUX                   	osd_layer_mux                   ;	// 0x1ae0 : ''
	OSD_MIXER_OUT                   	osd_mixer_out                   ;	// 0x1ae4 : ''
	OSD_MIXER_STATUS                	osd_mixer_status                ;	// 0x1ae8 : ''
	OSD_STATUS_CFG                  	osd_status_cfg                  ;	// 0x1aec : ''
	OSD_STATUS_0                    	osd_status_0                    ;	// 0x1af0 : ''
	OSD_STATUS_1                    	osd_status_1                    ;	// 0x1af4 : ''
	OSD_STATUS_2                    	osd_status_2                    ;	// 0x1af8 : ''
	OSD_STATUS_3                    	osd_status_3                    ;	// 0x1afc : ''
} OSD_REG_MAP_T;
/* 62 regs, 62 types */

/* 62 regs, 62 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegOSD : real register of OSD.
 * gpRegOSD     : shadow register.
 *
 * @def OSD_RdFL: Read  FLushing : Shadow <- Real.
 * @def OSD_WrFL: Write FLushing : Shadow -> Real.
 * @def OSD_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def OSD_Wr  : Write whole register(UINT32) from Shadow register.
 * @def OSD_Rd01 ~ OSD_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def OSD_Wr01 ~ OSD_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define OSD_RdFL(_r)			((gpRegOSD->_r)=(gpRealRegOSD->_r))
#define OSD_WrFL(_r)			((gpRealRegOSD->_r)=(gpRegOSD->_r))

#define OSD_Rd(_r)			*((UINT32*)(&(gpRegOSD->_r)))
#define OSD_Wr(_r,_v)			((OSD_Rd(_r))=((UINT32)(_v)))

#define OSD_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
								} while(0)

#define OSD_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
								} while(0)

#define OSD_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
								} while(0)

#define OSD_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
								} while(0)

#define OSD_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
								} while(0)

#define OSD_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
								} while(0)

#define OSD_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
								} while(0)

#define OSD_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
								} while(0)

#define OSD_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
								} while(0)

#define OSD_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
									(_v10) = (gpRegOSD->_r._f10);				\
								} while(0)

#define OSD_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
									(_v10) = (gpRegOSD->_r._f10);				\
									(_v11) = (gpRegOSD->_r._f11);				\
								} while(0)

#define OSD_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
									(_v10) = (gpRegOSD->_r._f10);				\
									(_v11) = (gpRegOSD->_r._f11);				\
									(_v12) = (gpRegOSD->_r._f12);				\
								} while(0)

#define OSD_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
									(_v10) = (gpRegOSD->_r._f10);				\
									(_v11) = (gpRegOSD->_r._f11);				\
									(_v12) = (gpRegOSD->_r._f12);				\
									(_v13) = (gpRegOSD->_r._f13);				\
								} while(0)

#define OSD_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
									(_v10) = (gpRegOSD->_r._f10);				\
									(_v11) = (gpRegOSD->_r._f11);				\
									(_v12) = (gpRegOSD->_r._f12);				\
									(_v13) = (gpRegOSD->_r._f13);				\
									(_v14) = (gpRegOSD->_r._f14);				\
								} while(0)

#define OSD_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
									(_v10) = (gpRegOSD->_r._f10);				\
									(_v11) = (gpRegOSD->_r._f11);				\
									(_v12) = (gpRegOSD->_r._f12);				\
									(_v13) = (gpRegOSD->_r._f13);				\
									(_v14) = (gpRegOSD->_r._f14);				\
									(_v15) = (gpRegOSD->_r._f15);				\
								} while(0)

#define OSD_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegOSD->_r._f01);				\
									(_v02) = (gpRegOSD->_r._f02);				\
									(_v03) = (gpRegOSD->_r._f03);				\
									(_v04) = (gpRegOSD->_r._f04);				\
									(_v05) = (gpRegOSD->_r._f05);				\
									(_v06) = (gpRegOSD->_r._f06);				\
									(_v07) = (gpRegOSD->_r._f07);				\
									(_v08) = (gpRegOSD->_r._f08);				\
									(_v09) = (gpRegOSD->_r._f09);				\
									(_v10) = (gpRegOSD->_r._f10);				\
									(_v11) = (gpRegOSD->_r._f11);				\
									(_v12) = (gpRegOSD->_r._f12);				\
									(_v13) = (gpRegOSD->_r._f13);				\
									(_v14) = (gpRegOSD->_r._f14);				\
									(_v15) = (gpRegOSD->_r._f15);				\
									(_v16) = (gpRegOSD->_r._f16);				\
								} while(0)


#define OSD_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
								} while(0)

#define OSD_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
								} while(0)

#define OSD_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
								} while(0)

#define OSD_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
								} while(0)

#define OSD_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
								} while(0)

#define OSD_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
								} while(0)

#define OSD_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
								} while(0)

#define OSD_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
								} while(0)

#define OSD_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
								} while(0)

#define OSD_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
									(gpRegOSD->_r._f10) = (_v10);				\
								} while(0)

#define OSD_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
									(gpRegOSD->_r._f10) = (_v10);				\
									(gpRegOSD->_r._f11) = (_v11);				\
								} while(0)

#define OSD_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
									(gpRegOSD->_r._f10) = (_v10);				\
									(gpRegOSD->_r._f11) = (_v11);				\
									(gpRegOSD->_r._f12) = (_v12);				\
								} while(0)

#define OSD_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
									(gpRegOSD->_r._f10) = (_v10);				\
									(gpRegOSD->_r._f11) = (_v11);				\
									(gpRegOSD->_r._f12) = (_v12);				\
									(gpRegOSD->_r._f13) = (_v13);				\
								} while(0)

#define OSD_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
									(gpRegOSD->_r._f10) = (_v10);				\
									(gpRegOSD->_r._f11) = (_v11);				\
									(gpRegOSD->_r._f12) = (_v12);				\
									(gpRegOSD->_r._f13) = (_v13);				\
									(gpRegOSD->_r._f14) = (_v14);				\
								} while(0)

#define OSD_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
									(gpRegOSD->_r._f10) = (_v10);				\
									(gpRegOSD->_r._f11) = (_v11);				\
									(gpRegOSD->_r._f12) = (_v12);				\
									(gpRegOSD->_r._f13) = (_v13);				\
									(gpRegOSD->_r._f14) = (_v14);				\
									(gpRegOSD->_r._f15) = (_v15);				\
								} while(0)

#define OSD_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegOSD->_r._f01) = (_v01);				\
									(gpRegOSD->_r._f02) = (_v02);				\
									(gpRegOSD->_r._f03) = (_v03);				\
									(gpRegOSD->_r._f04) = (_v04);				\
									(gpRegOSD->_r._f05) = (_v05);				\
									(gpRegOSD->_r._f06) = (_v06);				\
									(gpRegOSD->_r._f07) = (_v07);				\
									(gpRegOSD->_r._f08) = (_v08);				\
									(gpRegOSD->_r._f09) = (_v09);				\
									(gpRegOSD->_r._f10) = (_v10);				\
									(gpRegOSD->_r._f11) = (_v11);				\
									(gpRegOSD->_r._f12) = (_v12);				\
									(gpRegOSD->_r._f13) = (_v13);				\
									(gpRegOSD->_r._f14) = (_v14);				\
									(gpRegOSD->_r._f15) = (_v15);				\
									(gpRegOSD->_r._f16) = (_v16);				\
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
 * _rwname: name of rw    bit field : shall be 0 after OSD_Wind(), 1 for OSD_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * OSD_Rind : General indexed register Read.(
 * OSD_Wind : General indexed register Read.
 *
 * OSD_Ridx : For 'index', 'rw', 'load' field name
 * OSD_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define OSD_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								OSD_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								OSD_WrFL(_r);									\
								OSD_RdFL(_r);									\
								OSD_Rd01(_r,_fname,_fval);						\
							} while (0)

#define OSD_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				OSD_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define OSD_Ridx(_r, _ival, _fname, _fval)	OSD_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define OSD_Widx(_r, _ival, _fname, _fval)	OSD_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8-DE_OSD_RegFile_100106.csv' 20100311 00:54:16   대한민국 표준시 by getregs v2.3 */
