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

#ifndef _#MOD#_REG_H_
#define _#MOD#_REG_H_

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
const DBG_REG_FIELD_T dbg_CHBCVD_001[] = {                       /* 0x0000 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_ntsc443_mode"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 4, "chbreg_colour_mode"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_vline_625"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "chbreg_hpixel"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_hv_delay"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_soft_reset"               },
};

/*-----------------------------------------------------------------------------
	0x0004 chbcvd_002 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_002[] = {                       /* 0x0004 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_hagc_half_en"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 4, "chbreg_dc_clamp_mode"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_mv_hagc"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_hagc_field_mode"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_ped"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_chroma_burst5or10"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,24, "chbreg_chroma_bw_lo"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,28, "chbreg_luma_notch_bw"            },
};

/*-----------------------------------------------------------------------------
	0x0008 chbcvd_003 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_003[] = {                       /* 0x0008 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_vf_nstd_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_lbadrgen_rst"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 8, "chbreg_hagc_mode"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_cagc_en"                  },
};

/*-----------------------------------------------------------------------------
	0x000c chbcvd_004 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_004[] = {                       /* 0x000c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_noise_thresh"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_hagc"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_adaptive_chroma_mode"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,20, "chbreg_adaptive_mode"            },
};

/*-----------------------------------------------------------------------------
	0x0010 chbcvd_005 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_005[] = {                       /* 0x0010 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_adc_input_swap"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_adc_updn_swap"            },
};

/*-----------------------------------------------------------------------------
	0x0014 chbcvd_006 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_006[] = {                       /* 0x0014 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_force_vcr"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_force_vcr_trick"          },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_force_vcr_ff"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_force_vcr_rew"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_force_vcr_en"             },
};

/*-----------------------------------------------------------------------------
	0x0018 chbcvd_007 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_007[] = {                       /* 0x0018 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_blue_mode"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_cbcr_swap"                },
};

/*-----------------------------------------------------------------------------
	0x001c chbcvd_008 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_008[] = {                       /* 0x001c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_blue_cr"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_blue_cb"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "chbreg_blue_y"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "chbreg_yc_delay"                 },
};

/*-----------------------------------------------------------------------------
	0x0020 chbcvd_009 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_009[] = {                       /* 0x0020 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_hue"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_saturation"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "chbreg_brightness"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "chbreg_contrast"                 },
};

/*-----------------------------------------------------------------------------
	0x0024 chbcvd_010 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_010[] = {                       /* 0x0024 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "chbreg_ckill"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_hlock_ckill"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_vbi_ckill"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "chbreg_user_ckill_mode"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "chbreg_cagc"                     },
};

/*-----------------------------------------------------------------------------
	0x0028 chbcvd_011 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_011[] = {                       /* 0x0028 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_nstd_hysis"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 4, "chbreg_hnon_std_threshold"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "chbreg_vnon_std_threshold"       },
};

/*-----------------------------------------------------------------------------
	0x002c chbcvd_012 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_012[] = {                       /* 0x002c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_bypass"                   },
};

/*-----------------------------------------------------------------------------
	0x0030 chbcvd_013 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_013[] = {                       /* 0x0030 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_bknwt_ckill"              },
};

/*-----------------------------------------------------------------------------
	0x0034 chbcvd_014 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_014[] = {                       /* 0x0034 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "chbreg_locked_count_noisy_max"   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 4, "chbreg_hdetect_clamp_level"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "chbreg_agc_peak_cntl"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_agc_peak_en"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,20, "chbreg_agc_peak_nominal"         },
};

/*-----------------------------------------------------------------------------
	0x0038 chbcvd_015 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_015[] = {                       /* 0x0038 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 8, "chbreg_hlock_vsync_mode"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "chbreg_locked_count_clean_max"   },
};

/*-----------------------------------------------------------------------------
	0x003c chbreg_hstate_fixed ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBREG_HSTATE_FIXED[] = {              /* 0x003c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "r0f38_5"                         },
};

/*-----------------------------------------------------------------------------
	0x0040 chbreg_disable_hfine ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBREG_DISABLE_HFINE[] = {             /* 0x0040 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "r0f38_7"                         },
};

/*-----------------------------------------------------------------------------
	0x003c chbcvd_016 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_016[] = {                       /* 0x003c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "chbreg_hstate_max"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_hstate_unlocked"          },
};

/*-----------------------------------------------------------------------------
	0x0040 chbcvd_017 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_017[] = {                       /* 0x0040 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "chbreg_cdto_inc"                 },
};

/*-----------------------------------------------------------------------------
	0x0044 chbcvd_018 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_018[] = {                       /* 0x0044 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "chbreg_hdto_inc"                 },
};

/*-----------------------------------------------------------------------------
	0x0048 chbcvd_019 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_019[] = {                       /* 0x0048 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_hsync_gate_end"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_hsync_gate_start"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "chbreg_hsync_phase_offset"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "chbreg_hsync_rising"             },
};

/*-----------------------------------------------------------------------------
	0x004c chbcvd_020 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_020[] = {                       /* 0x004c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_hlpf_clamp_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_hlpf_clamp_vbi_en"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_hlpf_clamp_noisy_en"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_hlpf_clamp_sel"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_hfine_lt_hcoarse"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_hdetect_noise_en"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "chbreg_hsync_low"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "chbreg_hsync_similar"            },
};

/*-----------------------------------------------------------------------------
	0x0050 chbcvd_021 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_021[] = {                       /* 0x0050 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_hsync_rising_end"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13, 8, "chbreg_hsync_rising_start"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,16, "chbreg_hsync_rising_auto"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,20, "chbreg_status_hsync_width"       },
};

/*-----------------------------------------------------------------------------
	0x0054 chbcvd_022 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_022[] = {                       /* 0x0054 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_db_freq"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_dr_freq"                  },
};

/*-----------------------------------------------------------------------------
	0x0058 chbcvd_023 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_023[] = {                       /* 0x0058 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_burst_gate_end"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_burst_gate_start"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "chbreg_hblank_end"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "chbreg_hblank_start"             },
};

/*-----------------------------------------------------------------------------
	0x005c chbcvd_024 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_024[] = {                       /* 0x005c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_hactive_width"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_hactive_start"            },
};

/*-----------------------------------------------------------------------------
	0x0060 chbcvd_025 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_025[] = {                       /* 0x0060 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_dual_coarse_hedge_vbi"    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_dual_fine_hedge_vbi"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_dual_hedge_auto_width"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_dual_hedge_dis"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_many_hsyncs_mode"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "chbreg_no_hsyncs_mode"           },
};

/*-----------------------------------------------------------------------------
	0x0064 chbcvd_026 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_026[] = {                       /* 0x0064 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 0, "chbreg_mv_hsync_rising_end"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_disable_hdsw_weak"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_no_hsyncs_weak"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,16, "chbreg_mv_hsync_rising_start"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "chbreg_slow_hdsw"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "chbreg_vcr_state2_long"          },
};

/*-----------------------------------------------------------------------------
	0x0068 chbcvd_027 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_027[] = {                       /* 0x0068 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_vactive_height"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_vactive_start"            },
};

/*-----------------------------------------------------------------------------
	0x006c chbcvd_028 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_028[] = {                       /* 0x006c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 0, "chbreg_vsync_agc_max"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 8, "chbreg_vsync_agc_min"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,16, "chbreg_vsync_h_max"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,24, "chbreg_vsync_h_min"              },
};

/*-----------------------------------------------------------------------------
	0x0070 chbcvd_029 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_029[] = {                       /* 0x0070 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_proscan_1field_mode"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 4, "chbreg_vsync_vbi_max"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_vlock_wide_range"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,16, "chbreg_vsync_vbi_min"            },
};

/*-----------------------------------------------------------------------------
	0x0074 chbcvd_030 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_030[] = {                       /* 0x0074 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_vsync_cntl"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_vsync_cntl_vcr"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_vsync_cntl_trick"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_vsync_cntl_ff_rew"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_vsync_cntl_noisy"         },
};

/*-----------------------------------------------------------------------------
	0x0078 chbcvd_031 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_031[] = {                       /* 0x0078 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_field_detect_mode"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_vodd_delayed"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_veven_delayed"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_flip_field"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_field_polarity"           },
};

/*-----------------------------------------------------------------------------
	0x007c chbcvd_032 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_032[] = {                       /* 0x007c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_vcrtrick_proscan"         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_vdetect_noise_en"         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_vactive_half_lines"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,12, "chbreg_vsync_no_signal_thresh"   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,20, "chbreg_vsync_signal_thresh"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,28, "chbreg_vloop_tc"                 },
};

/*-----------------------------------------------------------------------------
	0x0080 chbcvd_033 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_033[] = {                       /* 0x0080 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 0, "chbreg_vfield_hoffset"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_vfield_hoffset_fixed"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_vodd_early_delayed"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_veven_early_delayed"      },
};

/*-----------------------------------------------------------------------------
	0x0084 chbcvd_034 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_034[] = {                       /* 0x0084 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_cpump_vsync_blank_filter" },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 4, "chbreg_cpump_auto_stip_mode"     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_cpump_auto_stip_vactive"  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_cpump_auto_stip_noisy"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_cpump_auto_stip_no_signal"},
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_cpump_auto_stip_unlocked" },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "chbreg_cpump_auto_stip_nobp"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "chbreg_cpump_noisy_filter_en"    },
};

/*-----------------------------------------------------------------------------
	0x0088 chbcvd_035 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_035[] = {                       /* 0x0088 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_cpump_level_filter_gain"  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_cpump_fixed_syncmid"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_cpump_accum_mode"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "chbreg_cpump_vsync_mode"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_cpump_vsync_syncmid_filter" },
};

/*-----------------------------------------------------------------------------
	0x008c chbcvd_036 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_036[] = {                       /* 0x008c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_cpump_diff_noisy_only"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 4, "chbreg_cpump_up_diff_max"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_cpump_diff_signal_only"   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,16, "chbreg_cpump_dn_max"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,24, "chbreg_cpump_up_max"             },
};

/*-----------------------------------------------------------------------------
	0x0090 chbcvd_037 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_037[] = {                       /* 0x0090 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_cpump_y_override"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 8, "chbreg_cpump_dn_diff_max"        },
};

/*-----------------------------------------------------------------------------
	0x0094 chbcvd_038 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_038[] = {                       /* 0x0094 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_mv_vbi_detected"          },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 4, "chbreg_mv_colourstripes"         },
};

/*-----------------------------------------------------------------------------
	0x0098 chbcvd_039 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_039[] = {                       /* 0x0098 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_proscan_detected"         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_hnon_standard"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_vnon_standard"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_bknwt_detected"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_no_signal"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_hlock"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "chbreg_vlock"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "chbreg_chromalock"               },
};

/*-----------------------------------------------------------------------------
	0x009c chbcvd_040 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_040[] = {                       /* 0x009c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_pal_detected"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_secam_detected"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_vline_625_detected"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_noisy"                    },
};

/*-----------------------------------------------------------------------------
	0x00a0 chbcvd_041 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_041[] = {                       /* 0x00a0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_vcr"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_vcr_trick"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_vcr_ff"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_vcr_rew"                  },
};

/*-----------------------------------------------------------------------------
	0x00a4 chbcvd_042 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_042[] = {                       /* 0x00a4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "chbreg_status_hdto_inc"          },
};

/*-----------------------------------------------------------------------------
	0x00a8 chbcvd_043 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_043[] = {                       /* 0x00a8 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "chbreg_status_cdto_inc"          },
};

/*-----------------------------------------------------------------------------
	0x00ac chbcvd_044 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_044[] = {                       /* 0x00ac */
	{ REG_XS_N_RW,  0, 0,  0, 0, 23, 8, "chbreg_status_agc_gain"          },
};

/*-----------------------------------------------------------------------------
	0x00b0 chbreg_status_cmag ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBREG_STATUS_CMAG[] = {               /* 0x00b0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "r0fac_2"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_status_cordic_freq"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,16, "chbreg_status_cgain"             },
};

/*-----------------------------------------------------------------------------
	0x00b4 chbcvd_046 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_046[] = {                       /* 0x00b4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "chbreg_status_nstd"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,12, "chbreg_status_noise"             },
};

/*-----------------------------------------------------------------------------
	0x00b8 chbcvd_047 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_047[] = {                       /* 0x00b8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_palsw_level"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_sv_bf"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_auto_secam_level"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_peak_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "chbreg_peak_gain"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "chbreg_peak_range"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "chbreg_secam_ybw"                },
};

/*-----------------------------------------------------------------------------
	0x00bc chbcvd_048 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_048[] = {                       /* 0x00bc */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_lose_chromalock_mode"     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_lose_chromalock_ckill"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 8, "chbreg_lose_chromalock_level"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "chbreg_lose_chromalock_count"    },
};

/*-----------------------------------------------------------------------------
	0x00c0 chbcvd_049 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_049[] = {                       /* 0x00c0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "chbreg_cstripe_detect_control"   },
};

/*-----------------------------------------------------------------------------
	0x00c4 chbcvd_050 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_050[] = {                       /* 0x00c4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_hresampler_2up"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_hfine_vcr_en"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_hfine_vcr_trick_en"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,12, "chbreg_chroma_activity_level"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_fixed_cstate"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,24, "chbreg_cstate"                   },
};

/*-----------------------------------------------------------------------------
	0x00c8 chbcvd_051 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_051[] = {                       /* 0x00c8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_noise_th_en"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 4, "chbreg_cpump_delay"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,12, "chbreg_cpump_adjust"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,20, "chbreg_cpump_adjust_delay"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "chbreg_cpump_adjust_polarity"    },
};

/*-----------------------------------------------------------------------------
	0x00cc chbcvd_052 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_052[] = {                       /* 0x00cc */
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 0, "chbreg_noise_th"                 },
};

/*-----------------------------------------------------------------------------
	0x00d0 chbcvd_053 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_053[] = {                       /* 0x00d0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_delta_hluma_th"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "chbreg_delta_vluma_th"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "chbreg_vchroma_th"               },
};

/*-----------------------------------------------------------------------------
	0x00d4 chbcvd_054 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_054[] = {                       /* 0x00d4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_pchroma_peak"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_pchroma_coring_en"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_pchroma_peak_en"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "chbreg_schroma_peak"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_schroma_coring_en"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_schroma_peak_en"          },
};

/*-----------------------------------------------------------------------------
	0x00d8 chbcvd_055 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_055[] = {                       /* 0x00d8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "chbreg_hsync_pulse_width"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 4, "chbreg_tcomb_gain"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 8, "chbreg_comb_gain"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "chbreg_notch_gain"               },
};

/*-----------------------------------------------------------------------------
	0x00dc chbcvd_056 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_056[] = {                       /* 0x00dc */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "chbreg_cagc_coring"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_cagc_unity_gain"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "chbreg_cagc_coring_threshold"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "chbreg_cagc_tc_ismall"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "chbreg_cagc_tc_ibig"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "chbreg_cagc_tc_p"                },
};

/*-----------------------------------------------------------------------------
	0x00e0 chbcvd_057 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_057[] = {                       /* 0x00e0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_dcrestore_bp_delay"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_dcrestore_kill_enable"    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "chbreg_dcrestore_no_bad_bp"      },
};

/*-----------------------------------------------------------------------------
	0x00e4 chbcvd_058 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_058[] = {                       /* 0x00e4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "chbreg_syncmid_filter_en"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "chbreg_syncmid_nobp_en"          },
};

/*-----------------------------------------------------------------------------
	0x00e8 chbcvd_059 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_059[] = {                       /* 0x00e8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_hdmgc"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 8, "chbreg_hamgc"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,12, "chbreg_dcrestore_accum_width"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "chbreg_dcrestore_kill_enable_noisy"  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "chbreg_dcrestore_lpf_en"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,28, "chbreg_dcrestore_gain"           },
};

/*-----------------------------------------------------------------------------
	0x00ec chbcvd_060 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_060[] = {                       /* 0x00ec */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "chbreg_vactive_hdsw_mode"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 4, "chbreg_min_sync_height"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_auto_min_sync_height"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "chbreg_dcrestore_hsync_mid"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "chbreg_bp_kill_thresh"           },
};

/*-----------------------------------------------------------------------------
	0x00f0 chbcvd_061 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHBCVD_061[] = {                       /* 0x00f0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "chbreg_big_hluma_th"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 8, "chbreg_hloop_range"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "chbreg_no_hsyncs_noisy"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "chbreg_hsync_falling_filter"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "chbreg_disable_hdsw_mode"        },
};

const DBG_REG_T gDbgRegCHBCVD[] = {
{0x0000,N_FLD(dbg_CHBCVD_001),                      "CHBCVD_001"                      ,dbg_CHBCVD_001                      }, // 
{0x0004,N_FLD(dbg_CHBCVD_002),                      "CHBCVD_002"                      ,dbg_CHBCVD_002                      }, // 
{0x0008,N_FLD(dbg_CHBCVD_003),                      "CHBCVD_003"                      ,dbg_CHBCVD_003                      }, // 
{0x000c,N_FLD(dbg_CHBCVD_004),                      "CHBCVD_004"                      ,dbg_CHBCVD_004                      }, // 
{0x0010,N_FLD(dbg_CHBCVD_005),                      "CHBCVD_005"                      ,dbg_CHBCVD_005                      }, // 
{0x0014,N_FLD(dbg_CHBCVD_006),                      "CHBCVD_006"                      ,dbg_CHBCVD_006                      }, // 
{0x0018,N_FLD(dbg_CHBCVD_007),                      "CHBCVD_007"                      ,dbg_CHBCVD_007                      }, // 
{0x001c,N_FLD(dbg_CHBCVD_008),                      "CHBCVD_008"                      ,dbg_CHBCVD_008                      }, // 
{0x0020,N_FLD(dbg_CHBCVD_009),                      "CHBCVD_009"                      ,dbg_CHBCVD_009                      }, // 
{0x0024,N_FLD(dbg_CHBCVD_010),                      "CHBCVD_010"                      ,dbg_CHBCVD_010                      }, // 
{0x0028,N_FLD(dbg_CHBCVD_011),                      "CHBCVD_011"                      ,dbg_CHBCVD_011                      }, // 
{0x002c,N_FLD(dbg_CHBCVD_012),                      "CHBCVD_012"                      ,dbg_CHBCVD_012                      }, // 
{0x0030,N_FLD(dbg_CHBCVD_013),                      "CHBCVD_013"                      ,dbg_CHBCVD_013                      }, // 
{0x0034,N_FLD(dbg_CHBCVD_014),                      "CHBCVD_014"                      ,dbg_CHBCVD_014                      }, // 
{0x0038,N_FLD(dbg_CHBCVD_015),                      "CHBCVD_015"                      ,dbg_CHBCVD_015                      }, // 
{0x003c,N_FLD(dbg_CHBREG_HSTATE_FIXED),             "CHBREG_HSTATE_FIXED"             ,dbg_CHBREG_HSTATE_FIXED             }, // 
{0x0040,N_FLD(dbg_CHBREG_DISABLE_HFINE),            "CHBREG_DISABLE_HFINE"            ,dbg_CHBREG_DISABLE_HFINE            }, // 
{0x003c,N_FLD(dbg_CHBCVD_016),                      "CHBCVD_016"                      ,dbg_CHBCVD_016                      }, // 
{0x0040,N_FLD(dbg_CHBCVD_017),                      "CHBCVD_017"                      ,dbg_CHBCVD_017                      }, // 
{0x0044,N_FLD(dbg_CHBCVD_018),                      "CHBCVD_018"                      ,dbg_CHBCVD_018                      }, // 
{0x0048,N_FLD(dbg_CHBCVD_019),                      "CHBCVD_019"                      ,dbg_CHBCVD_019                      }, // 
{0x004c,N_FLD(dbg_CHBCVD_020),                      "CHBCVD_020"                      ,dbg_CHBCVD_020                      }, // 
{0x0050,N_FLD(dbg_CHBCVD_021),                      "CHBCVD_021"                      ,dbg_CHBCVD_021                      }, // 
{0x0054,N_FLD(dbg_CHBCVD_022),                      "CHBCVD_022"                      ,dbg_CHBCVD_022                      }, // 
{0x0058,N_FLD(dbg_CHBCVD_023),                      "CHBCVD_023"                      ,dbg_CHBCVD_023                      }, // 
{0x005c,N_FLD(dbg_CHBCVD_024),                      "CHBCVD_024"                      ,dbg_CHBCVD_024                      }, // 
{0x0060,N_FLD(dbg_CHBCVD_025),                      "CHBCVD_025"                      ,dbg_CHBCVD_025                      }, // 
{0x0064,N_FLD(dbg_CHBCVD_026),                      "CHBCVD_026"                      ,dbg_CHBCVD_026                      }, // 
{0x0068,N_FLD(dbg_CHBCVD_027),                      "CHBCVD_027"                      ,dbg_CHBCVD_027                      }, // 
{0x006c,N_FLD(dbg_CHBCVD_028),                      "CHBCVD_028"                      ,dbg_CHBCVD_028                      }, // 
{0x0070,N_FLD(dbg_CHBCVD_029),                      "CHBCVD_029"                      ,dbg_CHBCVD_029                      }, // 
{0x0074,N_FLD(dbg_CHBCVD_030),                      "CHBCVD_030"                      ,dbg_CHBCVD_030                      }, // 
{0x0078,N_FLD(dbg_CHBCVD_031),                      "CHBCVD_031"                      ,dbg_CHBCVD_031                      }, // 
{0x007c,N_FLD(dbg_CHBCVD_032),                      "CHBCVD_032"                      ,dbg_CHBCVD_032                      }, // 
{0x0080,N_FLD(dbg_CHBCVD_033),                      "CHBCVD_033"                      ,dbg_CHBCVD_033                      }, // 
{0x0084,N_FLD(dbg_CHBCVD_034),                      "CHBCVD_034"                      ,dbg_CHBCVD_034                      }, // 
{0x0088,N_FLD(dbg_CHBCVD_035),                      "CHBCVD_035"                      ,dbg_CHBCVD_035                      }, // 
{0x008c,N_FLD(dbg_CHBCVD_036),                      "CHBCVD_036"                      ,dbg_CHBCVD_036                      }, // 
{0x0090,N_FLD(dbg_CHBCVD_037),                      "CHBCVD_037"                      ,dbg_CHBCVD_037                      }, // 
{0x0094,N_FLD(dbg_CHBCVD_038),                      "CHBCVD_038"                      ,dbg_CHBCVD_038                      }, // 
{0x0098,N_FLD(dbg_CHBCVD_039),                      "CHBCVD_039"                      ,dbg_CHBCVD_039                      }, // 
{0x009c,N_FLD(dbg_CHBCVD_040),                      "CHBCVD_040"                      ,dbg_CHBCVD_040                      }, // 
{0x00a0,N_FLD(dbg_CHBCVD_041),                      "CHBCVD_041"                      ,dbg_CHBCVD_041                      }, // 
{0x00a4,N_FLD(dbg_CHBCVD_042),                      "CHBCVD_042"                      ,dbg_CHBCVD_042                      }, // 
{0x00a8,N_FLD(dbg_CHBCVD_043),                      "CHBCVD_043"                      ,dbg_CHBCVD_043                      }, // 
{0x00ac,N_FLD(dbg_CHBCVD_044),                      "CHBCVD_044"                      ,dbg_CHBCVD_044                      }, // 
{0x00b0,N_FLD(dbg_CHBREG_STATUS_CMAG),              "CHBREG_STATUS_CMAG"              ,dbg_CHBREG_STATUS_CMAG              }, // 
{0x00b4,N_FLD(dbg_CHBCVD_046),                      "CHBCVD_046"                      ,dbg_CHBCVD_046                      }, // 
{0x00b8,N_FLD(dbg_CHBCVD_047),                      "CHBCVD_047"                      ,dbg_CHBCVD_047                      }, // 
{0x00bc,N_FLD(dbg_CHBCVD_048),                      "CHBCVD_048"                      ,dbg_CHBCVD_048                      }, // 
{0x00c0,N_FLD(dbg_CHBCVD_049),                      "CHBCVD_049"                      ,dbg_CHBCVD_049                      }, // 
{0x00c4,N_FLD(dbg_CHBCVD_050),                      "CHBCVD_050"                      ,dbg_CHBCVD_050                      }, // 
{0x00c8,N_FLD(dbg_CHBCVD_051),                      "CHBCVD_051"                      ,dbg_CHBCVD_051                      }, // 
{0x00cc,N_FLD(dbg_CHBCVD_052),                      "CHBCVD_052"                      ,dbg_CHBCVD_052                      }, // 
{0x00d0,N_FLD(dbg_CHBCVD_053),                      "CHBCVD_053"                      ,dbg_CHBCVD_053                      }, // 
{0x00d4,N_FLD(dbg_CHBCVD_054),                      "CHBCVD_054"                      ,dbg_CHBCVD_054                      }, // 
{0x00d8,N_FLD(dbg_CHBCVD_055),                      "CHBCVD_055"                      ,dbg_CHBCVD_055                      }, // 
{0x00dc,N_FLD(dbg_CHBCVD_056),                      "CHBCVD_056"                      ,dbg_CHBCVD_056                      }, // 
{0x00e0,N_FLD(dbg_CHBCVD_057),                      "CHBCVD_057"                      ,dbg_CHBCVD_057                      }, // 
{0x00e4,N_FLD(dbg_CHBCVD_058),                      "CHBCVD_058"                      ,dbg_CHBCVD_058                      }, // 
{0x00e8,N_FLD(dbg_CHBCVD_059),                      "CHBCVD_059"                      ,dbg_CHBCVD_059                      }, // 
{0x00ec,N_FLD(dbg_CHBCVD_060),                      "CHBCVD_060"                      ,dbg_CHBCVD_060                      }, // 
{0x00f0,N_FLD(dbg_CHBCVD_061),                      "CHBCVD_061"                      ,dbg_CHBCVD_061                      }, // 
};

/* 63 regs, 63 types in Total*/

#ifdef __cplusplus
}
#endif

#endif	/* _#MOD#_REG_H_ */

/* from 'LG1152-VENC-MAN-01 v1 2 (Core Register Manual-20110603).csv' 20110609 14:26:41   대한민국 표준시 by getregs v2.7 */
