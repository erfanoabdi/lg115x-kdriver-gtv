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

/** @file
 *
 *  #MOD# register details. ( used only within kdriver )
 *
 *  author     user name (user_name@lge.com)
 *  version    1.0
 *  date       2010.xx.xx
 *
 */

#ifndef _CHBCVD_REG_L9_H_
#define _CHBCVD_REG_L9_H_

/*----------------------------------------------------------------------------------------
    Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
	0x0000 chbcvd_001 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_ntsc443_mode             : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_colour_mode              : 3,	//  4: 6
	                                : 1,	//     7 reserved
	chbreg_vline_625                : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_hpixel                   : 2,	// 12:13
	                                : 2,	// 14:15 reserved
	chbreg_hv_delay                 : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_soft_reset               : 1;	//    20
} CHBCVD_L9_001;

/*-----------------------------------------------------------------------------
	0x0004 chbcvd_002 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hagc_half_en             : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_dc_clamp_mode            : 2,	//  4: 5
	                                : 2,	//  6: 7 reserved
	chbreg_mv_hagc                  : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_hagc_field_mode          : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_ped                      : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_chroma_burst5or10        : 1,	//    20
	                                : 3,	// 21:23 reserved
	chbreg_chroma_bw_lo             : 2,	// 24:25
	                                : 2,	// 26:27 reserved
	chbreg_luma_notch_bw            : 2;	// 28:29
} CHBCVD_L9_002;

/*-----------------------------------------------------------------------------
	0x0008 chbcvd_003 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vf_nstd_en               : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_lbadrgen_rst             : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_hagc_mode                : 2,	//  8: 9
	                                : 2,	// 10:11 reserved
	chbreg_cagc_en                  : 1;	//    12
} CHBCVD_L9_003;

/*-----------------------------------------------------------------------------
	0x000c chbcvd_004 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_noise_thresh             : 8,	//  0: 7
	chbreg_hagc                     : 8,	//  8:15
	chbreg_adaptive_chroma_mode     : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_adaptive_mode            : 3;	// 20:22
} CHBCVD_L9_004;

/*-----------------------------------------------------------------------------
	0x0010 chbcvd_005 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_adc_input_swap           : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_adc_updn_swap            : 1;	//     4
} CHBCVD_L9_005;

/*-----------------------------------------------------------------------------
	0x0014 chbcvd_006 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_force_vcr                : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_force_vcr_trick          : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_force_vcr_ff             : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_force_vcr_rew            : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_force_vcr_en             : 1;	//    16
} CHBCVD_L9_006;

/*-----------------------------------------------------------------------------
	0x0018 chbcvd_007 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_blue_mode                : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_cbcr_swap                : 1;	//     4
} CHBCVD_L9_007;

/*-----------------------------------------------------------------------------
	0x001c chbcvd_008 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_blue_cr                  : 8,	//  0: 7
	chbreg_blue_cb                  : 8,	//  8:15
	chbreg_blue_y                   : 8,	// 16:23
	chbreg_yc_delay                 : 4;	// 24:27
} CHBCVD_L9_008;

/*-----------------------------------------------------------------------------
	0x0020 chbcvd_009 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hue                      : 8,	//  0: 7
	chbreg_saturation               : 8,	//  8:15
	chbreg_brightness               : 8,	// 16:23
	chbreg_contrast                 : 8;	// 24:31
} CHBCVD_L9_009;

/*-----------------------------------------------------------------------------
	0x0024 chbcvd_010 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_ckill                    : 4,	//  0: 3
	chbreg_hlock_ckill              : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_vbi_ckill                : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_user_ckill_mode          : 2,	// 12:13
	                                : 2,	// 14:15 reserved
	chbreg_cagc                     : 8;	// 16:23
} CHBCVD_L9_010;

/*-----------------------------------------------------------------------------
	0x0028 chbcvd_011 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_nstd_hysis               : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_hnon_std_threshold       : 6,	//  4: 9
	                                : 2,	// 10:11 reserved
	chbreg_vnon_std_threshold       : 2;	// 12:13
} CHBCVD_L9_011;

/*-----------------------------------------------------------------------------
	0x002c chbcvd_012 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_bypass                   : 1;	//     0
} CHBCVD_L9_012;

/*-----------------------------------------------------------------------------
	0x0030 chbcvd_013 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_bknwt_ckill              : 1;	//     0
} CHBCVD_L9_013;

/*-----------------------------------------------------------------------------
	0x0034 chbcvd_014 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_locked_count_noisy_max   : 4,	//  0: 3
	chbreg_hdetect_clamp_level      : 8,	//  4:11
	chbreg_agc_peak_cntl            : 3,	// 12:14
	                                : 1,	//    15 reserved
	chbreg_agc_peak_en              : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_agc_peak_nominal         : 7;	// 20:26
} CHBCVD_L9_014;

/*-----------------------------------------------------------------------------
	0x0038 chbcvd_015 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_disable_hfine			: 1,	// 0
	                                : 3,	//  1: 3 reserved
	chbreg_hstate_fixed				: 1,	//	4
	                                : 3,	//  5: 7 reserved
	chbreg_hlock_vsync_mode         : 2,	//  8: 9
	                                : 2,	// 10:11 reserved
	chbreg_locked_count_clean_max   : 4;	// 12:15
} CHBCVD_L9_015;

/*-----------------------------------------------------------------------------
	0x003c chbcvd_016 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hstate_max               : 3,	//  0: 2
	                                : 1,	//     3 reserved
	chbreg_hstate_unlocked          : 1;	//     4
} CHBCVD_L9_016;

/*-----------------------------------------------------------------------------
	0x0040 chbcvd_017 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_cdto_inc                 ;   	// 31: 0
} CHBCVD_L9_017;

/*-----------------------------------------------------------------------------
	0x0044 chbcvd_018 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hdto_inc                 ;   	// 31: 0
} CHBCVD_L9_018;

/*-----------------------------------------------------------------------------
	0x0048 chbcvd_019 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hsync_gate_end           : 8,	//  0: 7
	chbreg_hsync_gate_start         : 8,	//  8:15
	chbreg_hsync_phase_offset       : 8,	// 16:23
	chbreg_hsync_rising             : 8;	// 24:31
} CHBCVD_L9_019;

/*-----------------------------------------------------------------------------
	0x004c chbcvd_020 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hlpf_clamp_en            : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_hlpf_clamp_vbi_en        : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_hlpf_clamp_noisy_en      : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_hlpf_clamp_sel           : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_hfine_lt_hcoarse         : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_hdetect_noise_en         : 1,	//    20
	                                : 3,	// 21:23 reserved
	chbreg_hsync_low                : 1,	//    24
	                                : 3,	// 25:27 reserved
	chbreg_hsync_similar            : 1;	//    28
} CHBCVD_L9_020;

/*-----------------------------------------------------------------------------
	0x0050 chbcvd_021 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hsync_rising_end         : 8,	//  0: 7
	chbreg_hsync_rising_start       : 6,	//  8:13
	                                : 2,	// 14:15 reserved
	chbreg_hsync_rising_auto        : 2,	// 16:17
	                                : 2,	// 18:19 reserved
	chbreg_status_hsync_width       : 8;	// 20:27
} CHBCVD_L9_021;

/*-----------------------------------------------------------------------------
	0x0054 chbcvd_022 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_db_freq                  : 8,	//  0: 7
	chbreg_dr_freq                  : 8;	//  8:15
} CHBCVD_L9_022;

/*-----------------------------------------------------------------------------
	0x0058 chbcvd_023 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_burst_gate_end           : 8,	//  0: 7
	chbreg_burst_gate_start         : 8,	//  8:15
	chbreg_hblank_end               : 8,	// 16:23
	chbreg_hblank_start             : 8;	// 24:31
} CHBCVD_L9_023;

/*-----------------------------------------------------------------------------
	0x005c chbcvd_024 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hactive_width            : 8,	//  0: 7
	chbreg_hactive_start            : 8;	//  8:15
} CHBCVD_L9_024;

/*-----------------------------------------------------------------------------
	0x0060 chbcvd_025 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_dual_coarse_hedge_vbi    : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_dual_fine_hedge_vbi      : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_dual_hedge_auto_width    : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_dual_hedge_dis           : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_many_hsyncs_mode         : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_no_hsyncs_mode           : 2;	// 20:21
} CHBCVD_L9_025;

/*-----------------------------------------------------------------------------
	0x0064 chbcvd_026 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_mv_hsync_rising_end      : 6,	//  0: 5
	                                : 2,	//  6: 7 reserved
	chbreg_disable_hdsw_weak        : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_no_hsyncs_weak           : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_mv_hsync_rising_start    : 6,	// 16:21
	                                : 2,	// 22:23 reserved
	chbreg_slow_hdsw                : 1,	//    24
	                                : 3,	// 25:27 reserved
	chbreg_vcr_state2_long          : 1;	//    28
} CHBCVD_L9_026;

/*-----------------------------------------------------------------------------
	0x0068 chbcvd_027 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vactive_height           : 8,	//  0: 7
	chbreg_vactive_start            : 8;	//  8:15
} CHBCVD_L9_027;

/*-----------------------------------------------------------------------------
	0x006c chbcvd_028 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vsync_agc_max            : 6,	//  0: 5
	                                : 2,	//  6: 7 reserved
	chbreg_vsync_agc_min            : 7,	//  8:14
	                                : 1,	//    15 reserved
	chbreg_vsync_h_max              : 7,	// 16:22
	                                : 1,	//    23 reserved
	chbreg_vsync_h_min              : 7;	// 24:30
} CHBCVD_L9_028;

/*-----------------------------------------------------------------------------
	0x0070 chbcvd_029 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_proscan_1field_mode      : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_vsync_vbi_max            : 7,	//  4:10
	                                : 1,	//    11 reserved
	chbreg_vlock_wide_range         : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_vsync_vbi_min            : 7;	// 16:22
} CHBCVD_L9_029;

/*-----------------------------------------------------------------------------
	0x0074 chbcvd_030 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vsync_cntl               : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_vsync_cntl_vcr           : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_vsync_cntl_trick         : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_vsync_cntl_ff_rew        : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_vsync_cntl_noisy         : 1;	//    16
} CHBCVD_L9_030;

/*-----------------------------------------------------------------------------
	0x0078 chbcvd_031 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_field_detect_mode        : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_vodd_delayed             : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_veven_delayed            : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_flip_field               : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_field_polarity           : 1;	//    16
} CHBCVD_L9_031;

/*-----------------------------------------------------------------------------
	0x007c chbcvd_032 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vcrtrick_proscan         : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_vdetect_noise_en         : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_vactive_half_lines       : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_vsync_no_signal_thresh   : 8,	// 12:19
	chbreg_vsync_signal_thresh      : 8,	// 20:27
	chbreg_vloop_tc                 : 2;	// 28:29
} CHBCVD_L9_032;

/*-----------------------------------------------------------------------------
	0x0080 chbcvd_033 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vfield_hoffset           : 9,	//  0: 8
	                                : 3,	//  9:11 reserved
	chbreg_vfield_hoffset_fixed     : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_vodd_early_delayed       : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_veven_early_delayed      : 1;	//    20
} CHBCVD_L9_033;

/*-----------------------------------------------------------------------------
	0x0084 chbcvd_034 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_cpump_vsync_blank_filter : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_cpump_auto_stip_mode     : 2,	//  4: 5
	                                : 2,	//  6: 7 reserved
	chbreg_cpump_auto_stip_vactive  : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_cpump_auto_stip_noisy    : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_cpump_auto_stip_no_signal: 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_cpump_auto_stip_unlocked : 1,	//    20
	                                : 3,	// 21:23 reserved
	chbreg_cpump_auto_stip_nobp     : 1,	//    24
	                                : 3,	// 25:27 reserved
	chbreg_cpump_noisy_filter_en    : 1;	//    28
} CHBCVD_L9_034;

/*-----------------------------------------------------------------------------
	0x0088 chbcvd_035 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_cpump_level_filter_gain  : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_cpump_fixed_syncmid      : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_cpump_accum_mode         : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_cpump_vsync_mode         : 2,	// 12:13
	                                : 2,	// 14:15 reserved
	chbreg_cpump_vsync_syncmid_filte: 1;	//    16
} CHBCVD_L9_035;

/*-----------------------------------------------------------------------------
	0x008c chbcvd_036 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_cpump_diff_noisy_only    : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_cpump_up_diff_max        : 7,	//  4:10
	                                : 1,	//    11 reserved
	chbreg_cpump_diff_signal_only   : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_cpump_dn_max             : 7,	// 16:22
	                                : 1,	//    23 reserved
	chbreg_cpump_up_max             : 7;	// 24:30
} CHBCVD_L9_036;

/*-----------------------------------------------------------------------------
	0x0090 chbcvd_037 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_cpump_y_override         : 8,	//  0: 7
	chbreg_cpump_dn_diff_max        : 7;	//  8:14
} CHBCVD_L9_037;

/*-----------------------------------------------------------------------------
	0x0094 chbcvd_038 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_mv_vbi_detected          : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_mv_colourstripes         : 3;	//  4: 6
} CHBCVD_L9_038;

/*-----------------------------------------------------------------------------
	0x0098 chbcvd_039 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_proscan_detected         : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_hnon_standard            : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_vnon_standard            : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_bknwt_detected           : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_no_signal                : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_hlock                    : 1,	//    20
	                                : 3,	// 21:23 reserved
	chbreg_vlock                    : 1,	//    24
	                                : 3,	// 25:27 reserved
	chbreg_chromalock               : 1;	//    28
} CHBCVD_L9_039;

/*-----------------------------------------------------------------------------
	0x009c chbcvd_040 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_pal_detected             : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_secam_detected           : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_vline_625_detected       : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_noisy                    : 1;	//    12
} CHBCVD_L9_040;

/*-----------------------------------------------------------------------------
	0x00a0 chbcvd_041 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vcr                      : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_vcr_trick                : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_vcr_ff                   : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_vcr_rew                  : 1;	//    12
} CHBCVD_L9_041;

/*-----------------------------------------------------------------------------
	0x00a4 chbcvd_042 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_status_hdto_inc          :30;	//  0:29
} CHBCVD_L9_042;

/*-----------------------------------------------------------------------------
	0x00a8 chbcvd_043 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_status_cdto_inc          :30;	//  0:29
} CHBCVD_L9_043;

/*-----------------------------------------------------------------------------
	0x00ac chbcvd_044 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_status_cmag				: 8,	//  0:7
	chbreg_status_agc_gain          :16;	//  8:23
} CHBCVD_L9_044;

/*-----------------------------------------------------------------------------
	0x00b0 chbreg_status_cmag ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_status_sync_height       : 8,	//  0: 7
	chbreg_status_cordic_freq       : 8,	//  8:15
	chbreg_status_cgain             :14;	// 16:29
} CHBCVD_L9_045;

/*-----------------------------------------------------------------------------
	0x00b4 chbcvd_046 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_status_nstd              :11,	//  0:10
	                                : 1,	//    11 reserved
	chbreg_status_noise             : 8;	// 12:19
} CHBCVD_L9_046;

/*-----------------------------------------------------------------------------
	0x00b8 chbcvd_047 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_palsw_level              : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_sv_bf                    : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_auto_secam_level         : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_peak_en                  : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_peak_gain                : 3,	// 16:18
	                                : 1,	//    19 reserved
	chbreg_peak_range               : 2,	// 20:21
	                                : 2,	// 22:23 reserved
	chbreg_secam_ybw                : 1;	//    24
} CHBCVD_L9_047;

/*-----------------------------------------------------------------------------
	0x00bc chbcvd_048 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_lose_chromalock_mode     : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_lose_chromalock_ckill    : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_lose_chromalock_level    : 3,	//  8:10
	                                : 1,	//    11 reserved
	chbreg_lose_chromalock_count    : 4;	// 12:15
} CHBCVD_L9_048;

/*-----------------------------------------------------------------------------
	0x00c0 chbcvd_049 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_cstripe_detect_control   : 3;	//  0: 2
} CHBCVD_L9_049;

/*-----------------------------------------------------------------------------
	0x00c4 chbcvd_050 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hresampler_2up           : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_hfine_vcr_en             : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_hfine_vcr_trick_en       : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_chroma_activity_level    : 8,	// 12:19
	chbreg_fixed_cstate             : 1,	//    20
	                                : 3,	// 21:23 reserved
	chbreg_cstate                   : 3;	// 24:26
} CHBCVD_L9_050;

/*-----------------------------------------------------------------------------
	0x00c8 chbcvd_051 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_noise_th_en              : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_cpump_delay              : 8,	//  4:11
	chbreg_cpump_adjust             : 8,	// 12:19
	chbreg_cpump_adjust_delay       : 6,	// 20:25
	                                : 2,	// 26:27 reserved
	chbreg_cpump_adjust_polarity    : 1;	//    28
} CHBCVD_L9_051;

/*-----------------------------------------------------------------------------
	0x00cc chbcvd_052 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_noise_th                 : 7;	//  0: 6
} CHBCVD_L9_052;

/*-----------------------------------------------------------------------------
	0x00d0 chbcvd_053 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_delta_hluma_th           : 8,	//  0: 7
	chbreg_delta_vluma_th           : 8,	//  8:15
	chbreg_vchroma_th               : 8;	// 16:23
} CHBCVD_L9_053;

/*-----------------------------------------------------------------------------
	0x00d4 chbcvd_054 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_pchroma_peak             : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_pchroma_coring_en        : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_pchroma_peak_en          : 1,	//     8
	                                : 3,	//  9:11 reserved
	chbreg_schroma_peak             : 2,	// 12:13
	                                : 2,	// 14:15 reserved
	chbreg_schroma_coring_en        : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_schroma_peak_en          : 1;	//    20
} CHBCVD_L9_054;

/*-----------------------------------------------------------------------------
	0x00d8 chbcvd_055 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hsync_pulse_width        : 4,	//  0: 3
	chbreg_tcomb_gain               : 3,	//  4: 6
	                                : 1,	//     7 reserved
	chbreg_comb_gain                : 3,	//  8:10
	                                : 1,	//    11 reserved
	chbreg_notch_gain               : 3;	// 12:14
} CHBCVD_L9_055;

/*-----------------------------------------------------------------------------
	0x00dc chbcvd_056 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_cagc_coring              : 3,	//  0: 2
	                                : 1,	//     3 reserved
	chbreg_cagc_unity_gain          : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_cagc_coring_threshold    : 4,	//  8:11
	chbreg_cagc_tc_ismall           : 3,	// 12:14
	                                : 1,	//    15 reserved
	chbreg_cagc_tc_ibig             : 3,	// 16:18
	                                : 1,	//    19 reserved
	chbreg_cagc_tc_p                : 2;	// 20:21
} CHBCVD_L9_056;

/*-----------------------------------------------------------------------------
	0x00e0 chbcvd_057 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_dcrestore_bp_delay       : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_dcrestore_kill_enable    : 1,	//     4
	                                : 3,	//  5: 7 reserved
	chbreg_dcrestore_no_bad_bp      : 1;	//     8
} CHBCVD_L9_057;

/*-----------------------------------------------------------------------------
	0x00e4 chbcvd_058 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_syncmid_filter_en        : 1,	//     0
	                                : 3,	//  1: 3 reserved
	chbreg_syncmid_nobp_en          : 1;	//     4
} CHBCVD_L9_058;

/*-----------------------------------------------------------------------------
	0x00e8 chbcvd_059 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_hdmgc                    : 8,	//  0: 7
	chbreg_hamgc                    : 3,	//  8:10
	                                : 1,	//    11 reserved
	chbreg_dcrestore_accum_width    : 6,	// 12:17
	                                : 2,	// 18:19 reserved
	chbreg_dcrestore_kill_enable_noi: 1,	//    20
	                                : 3,	// 21:23 reserved
	chbreg_dcrestore_lpf_en         : 1,	//    24
	                                : 3,	// 25:27 reserved
	chbreg_dcrestore_gain           : 2;	// 28:29
} CHBCVD_L9_059;

/*-----------------------------------------------------------------------------
	0x00ec chbcvd_060 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_vactive_hdsw_mode        : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	chbreg_min_sync_height          : 7,	//  4:10
	                                : 1,	//    11 reserved
	chbreg_auto_min_sync_height     : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_dcrestore_hsync_mid      : 8,	// 16:23
	chbreg_bp_kill_thresh           : 8;	// 24:31
} CHBCVD_L9_060;

/*-----------------------------------------------------------------------------
	0x00f0 chbcvd_061 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	chbreg_big_hluma_th             : 8,	//  0: 7
	chbreg_hloop_range              : 2,	//  8: 9
	                                : 2,	// 10:11 reserved
	chbreg_no_hsyncs_noisy          : 1,	//    12
	                                : 3,	// 13:15 reserved
	chbreg_hsync_falling_filter     : 1,	//    16
	                                : 3,	// 17:19 reserved
	chbreg_disable_hdsw_mode        : 2;	// 20:21
} CHBCVD_L9_061;

typedef struct {
	CHBCVD_L9_001                      	chbcvd_001                      ;	// 0x0000 : ''
	CHBCVD_L9_002                      	chbcvd_002                      ;	// 0x0004 : ''
	CHBCVD_L9_003                      	chbcvd_003                      ;	// 0x0008 : ''
	CHBCVD_L9_004                      	chbcvd_004                      ;	// 0x000c : ''
	CHBCVD_L9_005                      	chbcvd_005                      ;	// 0x0010 : ''
	CHBCVD_L9_006                      	chbcvd_006                      ;	// 0x0014 : ''
	CHBCVD_L9_007                      	chbcvd_007                      ;	// 0x0018 : ''
	CHBCVD_L9_008                      	chbcvd_008                      ;	// 0x001c : ''
	CHBCVD_L9_009                      	chbcvd_009                      ;	// 0x0020 : ''
	CHBCVD_L9_010                      	chbcvd_010                      ;	// 0x0024 : ''
	CHBCVD_L9_011                      	chbcvd_011                      ;	// 0x0028 : ''
	CHBCVD_L9_012                      	chbcvd_012                      ;	// 0x002c : ''
	CHBCVD_L9_013                      	chbcvd_013                      ;	// 0x0030 : ''
	CHBCVD_L9_014                      	chbcvd_014                      ;	// 0x0034 : ''
	CHBCVD_L9_015                      	chbcvd_015                      ;	// 0x0038 : ''
	CHBCVD_L9_016                      	chbcvd_016                      ;	// 0x003c : ''
	CHBCVD_L9_017                      	chbcvd_017                      ;	// 0x0040 : ''
	CHBCVD_L9_018                      	chbcvd_018                      ;	// 0x0044 : ''
	CHBCVD_L9_019                      	chbcvd_019                      ;	// 0x0048 : ''
	CHBCVD_L9_020                      	chbcvd_020                      ;	// 0x004c : ''
	CHBCVD_L9_021                      	chbcvd_021                      ;	// 0x0050 : ''
	CHBCVD_L9_022                      	chbcvd_022                      ;	// 0x0054 : ''
	CHBCVD_L9_023                      	chbcvd_023                      ;	// 0x0058 : ''
	CHBCVD_L9_024                      	chbcvd_024                      ;	// 0x005c : ''
	CHBCVD_L9_025                      	chbcvd_025                      ;	// 0x0060 : ''
	CHBCVD_L9_026                      	chbcvd_026                      ;	// 0x0064 : ''
	CHBCVD_L9_027                      	chbcvd_027                      ;	// 0x0068 : ''
	CHBCVD_L9_028                      	chbcvd_028                      ;	// 0x006c : ''
	CHBCVD_L9_029                      	chbcvd_029                      ;	// 0x0070 : ''
	CHBCVD_L9_030                      	chbcvd_030                      ;	// 0x0074 : ''
	CHBCVD_L9_031                      	chbcvd_031                      ;	// 0x0078 : ''
	CHBCVD_L9_032                      	chbcvd_032                      ;	// 0x007c : ''
	CHBCVD_L9_033                      	chbcvd_033                      ;	// 0x0080 : ''
	CHBCVD_L9_034                      	chbcvd_034                      ;	// 0x0084 : ''
	CHBCVD_L9_035                      	chbcvd_035                      ;	// 0x0088 : ''
	CHBCVD_L9_036                      	chbcvd_036                      ;	// 0x008c : ''
	CHBCVD_L9_037                      	chbcvd_037                      ;	// 0x0090 : ''
	CHBCVD_L9_038                      	chbcvd_038                      ;	// 0x0094 : ''
	CHBCVD_L9_039                      	chbcvd_039                      ;	// 0x0098 : ''
	CHBCVD_L9_040                      	chbcvd_040                      ;	// 0x009c : ''
	CHBCVD_L9_041                      	chbcvd_041                      ;	// 0x00a0 : ''
	CHBCVD_L9_042                      	chbcvd_042                      ;	// 0x00a4 : ''
	CHBCVD_L9_043                      	chbcvd_043                      ;	// 0x00a8 : ''
	CHBCVD_L9_044                      	chbcvd_044                      ;	// 0x00ac : ''
	CHBCVD_L9_045 	           		  	chbcvd_045  		            ;	// 0x00b0 : ''
	CHBCVD_L9_046                      	chbcvd_046                      ;	// 0x00b4 : ''
	CHBCVD_L9_047                      	chbcvd_047                      ;	// 0x00b8 : ''
	CHBCVD_L9_048                      	chbcvd_048                      ;	// 0x00bc : ''
	CHBCVD_L9_049                      	chbcvd_049                      ;	// 0x00c0 : ''
	CHBCVD_L9_050                      	chbcvd_050                      ;	// 0x00c4 : ''
	CHBCVD_L9_051                      	chbcvd_051                      ;	// 0x00c8 : ''
	CHBCVD_L9_052                      	chbcvd_052                      ;	// 0x00cc : ''
	CHBCVD_L9_053                      	chbcvd_053                      ;	// 0x00d0 : ''
	CHBCVD_L9_054                      	chbcvd_054                      ;	// 0x00d4 : ''
	CHBCVD_L9_055                      	chbcvd_055                      ;	// 0x00d8 : ''
	CHBCVD_L9_056                      	chbcvd_056                      ;	// 0x00dc : ''
	CHBCVD_L9_057                      	chbcvd_057                      ;	// 0x00e0 : ''
	CHBCVD_L9_058                      	chbcvd_058                      ;	// 0x00e4 : ''
	CHBCVD_L9_059                      	chbcvd_059                      ;	// 0x00e8 : ''
	CHBCVD_L9_060                      	chbcvd_060                      ;	// 0x00ec : ''
	CHBCVD_L9_061                      	chbcvd_061                      ;	// 0x00f0 : ''
} CHBCVD_L9_REG_T;
/* 63 regs, 63 types */

/* 63 regs, 63 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegCHBCVD_L9 : real register of CHBCVD.
 * gpRegCHBCVD_L9     : shadow register.
 *
 * @def CHBCVD_L9_RdFL: Read  FLushing : Shadow <- Real.
 * @def CHBCVD_L9_WrFL: Write FLushing : Shadow -> Real.
 * @def CHBCVD_L9_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def CHBCVD_L9_Wr  : Write whole register(UINT32) from Shadow register.
 * @def CHBCVD_L9_Rd01 ~ CHBCVD_L9_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def CHBCVD_L9_Wr01 ~ CHBCVD_L9_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define CHBCVD_L9_RdFL(_r)			((gpRegCHBCVD_L9->_r)=(gpRealRegCHBCVD_L9->_r))
#define CHBCVD_L9_WrFL(_r)			((gpRealRegCHBCVD_L9->_r)=(gpRegCHBCVD_L9->_r))

#define CHBCVD_L9_Rd(_r)			*((UINT32*)(&(gpRegCHBCVD_L9->_r)))
#define CHBCVD_L9_Wr(_r,_v)			((CHBCVD_L9_Rd(_r))=((UINT32)(_v)))

#define CHBCVD_L9_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
								} while(0)

#define CHBCVD_L9_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
								} while(0)

#define CHBCVD_L9_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
								} while(0)

#define CHBCVD_L9_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
								} while(0)

#define CHBCVD_L9_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
								} while(0)

#define CHBCVD_L9_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
								} while(0)

#define CHBCVD_L9_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
								} while(0)

#define CHBCVD_L9_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
								} while(0)

#define CHBCVD_L9_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
								} while(0)

#define CHBCVD_L9_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
									(_v10) = (gpRegCHBCVD_L9->_r._f10);				\
								} while(0)

#define CHBCVD_L9_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
									(_v10) = (gpRegCHBCVD_L9->_r._f10);				\
									(_v11) = (gpRegCHBCVD_L9->_r._f11);				\
								} while(0)

#define CHBCVD_L9_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
									(_v10) = (gpRegCHBCVD_L9->_r._f10);				\
									(_v11) = (gpRegCHBCVD_L9->_r._f11);				\
									(_v12) = (gpRegCHBCVD_L9->_r._f12);				\
								} while(0)

#define CHBCVD_L9_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
									(_v10) = (gpRegCHBCVD_L9->_r._f10);				\
									(_v11) = (gpRegCHBCVD_L9->_r._f11);				\
									(_v12) = (gpRegCHBCVD_L9->_r._f12);				\
									(_v13) = (gpRegCHBCVD_L9->_r._f13);				\
								} while(0)

#define CHBCVD_L9_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
									(_v10) = (gpRegCHBCVD_L9->_r._f10);				\
									(_v11) = (gpRegCHBCVD_L9->_r._f11);				\
									(_v12) = (gpRegCHBCVD_L9->_r._f12);				\
									(_v13) = (gpRegCHBCVD_L9->_r._f13);				\
									(_v14) = (gpRegCHBCVD_L9->_r._f14);				\
								} while(0)

#define CHBCVD_L9_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
									(_v10) = (gpRegCHBCVD_L9->_r._f10);				\
									(_v11) = (gpRegCHBCVD_L9->_r._f11);				\
									(_v12) = (gpRegCHBCVD_L9->_r._f12);				\
									(_v13) = (gpRegCHBCVD_L9->_r._f13);				\
									(_v14) = (gpRegCHBCVD_L9->_r._f14);				\
									(_v15) = (gpRegCHBCVD_L9->_r._f15);				\
								} while(0)

#define CHBCVD_L9_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegCHBCVD_L9->_r._f01);				\
									(_v02) = (gpRegCHBCVD_L9->_r._f02);				\
									(_v03) = (gpRegCHBCVD_L9->_r._f03);				\
									(_v04) = (gpRegCHBCVD_L9->_r._f04);				\
									(_v05) = (gpRegCHBCVD_L9->_r._f05);				\
									(_v06) = (gpRegCHBCVD_L9->_r._f06);				\
									(_v07) = (gpRegCHBCVD_L9->_r._f07);				\
									(_v08) = (gpRegCHBCVD_L9->_r._f08);				\
									(_v09) = (gpRegCHBCVD_L9->_r._f09);				\
									(_v10) = (gpRegCHBCVD_L9->_r._f10);				\
									(_v11) = (gpRegCHBCVD_L9->_r._f11);				\
									(_v12) = (gpRegCHBCVD_L9->_r._f12);				\
									(_v13) = (gpRegCHBCVD_L9->_r._f13);				\
									(_v14) = (gpRegCHBCVD_L9->_r._f14);				\
									(_v15) = (gpRegCHBCVD_L9->_r._f15);				\
									(_v16) = (gpRegCHBCVD_L9->_r._f16);				\
								} while(0)


#define CHBCVD_L9_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
								} while(0)

#define CHBCVD_L9_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
								} while(0)

#define CHBCVD_L9_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
								} while(0)

#define CHBCVD_L9_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
								} while(0)

#define CHBCVD_L9_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
								} while(0)

#define CHBCVD_L9_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
								} while(0)

#define CHBCVD_L9_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
								} while(0)

#define CHBCVD_L9_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
								} while(0)

#define CHBCVD_L9_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
								} while(0)

#define CHBCVD_L9_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
									(gpRegCHBCVD_L9->_r._f10) = (_v10);				\
								} while(0)

#define CHBCVD_L9_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
									(gpRegCHBCVD_L9->_r._f10) = (_v10);				\
									(gpRegCHBCVD_L9->_r._f11) = (_v11);				\
								} while(0)

#define CHBCVD_L9_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
									(gpRegCHBCVD_L9->_r._f10) = (_v10);				\
									(gpRegCHBCVD_L9->_r._f11) = (_v11);				\
									(gpRegCHBCVD_L9->_r._f12) = (_v12);				\
								} while(0)

#define CHBCVD_L9_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
									(gpRegCHBCVD_L9->_r._f10) = (_v10);				\
									(gpRegCHBCVD_L9->_r._f11) = (_v11);				\
									(gpRegCHBCVD_L9->_r._f12) = (_v12);				\
									(gpRegCHBCVD_L9->_r._f13) = (_v13);				\
								} while(0)

#define CHBCVD_L9_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
									(gpRegCHBCVD_L9->_r._f10) = (_v10);				\
									(gpRegCHBCVD_L9->_r._f11) = (_v11);				\
									(gpRegCHBCVD_L9->_r._f12) = (_v12);				\
									(gpRegCHBCVD_L9->_r._f13) = (_v13);				\
									(gpRegCHBCVD_L9->_r._f14) = (_v14);				\
								} while(0)

#define CHBCVD_L9_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
									(gpRegCHBCVD_L9->_r._f10) = (_v10);				\
									(gpRegCHBCVD_L9->_r._f11) = (_v11);				\
									(gpRegCHBCVD_L9->_r._f12) = (_v12);				\
									(gpRegCHBCVD_L9->_r._f13) = (_v13);				\
									(gpRegCHBCVD_L9->_r._f14) = (_v14);				\
									(gpRegCHBCVD_L9->_r._f15) = (_v15);				\
								} while(0)

#define CHBCVD_L9_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegCHBCVD_L9->_r._f01) = (_v01);				\
									(gpRegCHBCVD_L9->_r._f02) = (_v02);				\
									(gpRegCHBCVD_L9->_r._f03) = (_v03);				\
									(gpRegCHBCVD_L9->_r._f04) = (_v04);				\
									(gpRegCHBCVD_L9->_r._f05) = (_v05);				\
									(gpRegCHBCVD_L9->_r._f06) = (_v06);				\
									(gpRegCHBCVD_L9->_r._f07) = (_v07);				\
									(gpRegCHBCVD_L9->_r._f08) = (_v08);				\
									(gpRegCHBCVD_L9->_r._f09) = (_v09);				\
									(gpRegCHBCVD_L9->_r._f10) = (_v10);				\
									(gpRegCHBCVD_L9->_r._f11) = (_v11);				\
									(gpRegCHBCVD_L9->_r._f12) = (_v12);				\
									(gpRegCHBCVD_L9->_r._f13) = (_v13);				\
									(gpRegCHBCVD_L9->_r._f14) = (_v14);				\
									(gpRegCHBCVD_L9->_r._f15) = (_v15);				\
									(gpRegCHBCVD_L9->_r._f16) = (_v16);				\
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
 * _rwname: name of rw    bit field : shall be 0 after CHBCVD_L9_Wind(), 1 for CHBCVD_L9_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * CHBCVD_L9_Rind : General indexed register Read.(
 * CHBCVD_L9_Wind : General indexed register Read.
 *
 * CHBCVD_L9_Ridx : For 'index', 'rw', 'load' field name
 * CHBCVD_L9_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define CHBCVD_L9_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								CHBCVD_L9_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								CHBCVD_L9_WrFL(_r);									\
								CHBCVD_L9_RdFL(_r);									\
								CHBCVD_L9_Rd01(_r,_fname,_fval);						\
							} while (0)

#define CHBCVD_L9_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				CHBCVD_L9_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define CHBCVD_L9_Ridx(_r, _ival, _fname, _fval)	CHBCVD_L9_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define CHBCVD_L9_Widx(_r, _ival, _fname, _fval)	CHBCVD_L9_Wind(_r,load,rw,index,_ival,_fname,_fval)

/** @} *//* end of macro documentation */

#ifdef __cplusplus
}
#endif

#endif	/* _#MOD#_REG_H_ */

/* from 'LG1152-VENC-MAN-01 v1 2 (Core Register Manual-20110603).csv' 20110609 14:26:14   대한민국 표준시 by getregs v2.7 */
