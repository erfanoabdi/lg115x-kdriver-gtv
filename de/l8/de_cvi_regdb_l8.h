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
	0x0100 top_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_TOP_CTRL[] = {                         /* 0x0100 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "scan_type"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 1, "field_gen_mode"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 3, "auto_field_gen"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "data_bit_mode"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "chroma_sampling"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "aspect_ratio"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 8, "yc_delay_mode_r"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,10, "yc_delay_mode_b"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "yc_delay_mode_g"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "auto_sync_polarity"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "manual_sync_polarity"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "adc_cut_direction"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "adc_bit_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "br_port_swap"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "r_port_swap"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "b_port_swap"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "g_port_swap"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,28, "bit_numbering_lsb2"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "data_en_mode"                    },
};

/*-----------------------------------------------------------------------------
	0x0104 offset_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_OFFSET_CTRL[] = {                      /* 0x0104 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "offset_h"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "offset_v"                        },
};

/*-----------------------------------------------------------------------------
	0x0108 size_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_SIZE_CTRL[] = {                        /* 0x0108 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "size_h"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "size_v"                          },
};

/*-----------------------------------------------------------------------------
	0x010c int_sync_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_INT_SYNC_CTRL0[] = {                   /* 0x010c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "res_hsync"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "res_hfp"                         },
};

/*-----------------------------------------------------------------------------
	0x0110 int_sync_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_INT_SYNC_CTRL1[] = {                   /* 0x0110 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 0, "res_vfp"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12, 8, "res_vsync"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,13, "res_field_add_line"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "res_field_sel"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "res_sync_sel"                    },
};

/*-----------------------------------------------------------------------------
	0x0114 dig_filt_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_DIG_FILT_CTRL0[] = {                   /* 0x0114 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient1"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient0"                },
};

/*-----------------------------------------------------------------------------
	0x0118 dig_filt_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_DIG_FILT_CTRL1[] = {                   /* 0x0118 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient3"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient2"                },
};

/*-----------------------------------------------------------------------------
	0x011c dig_filt_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_DIG_FILT_CTRL2[] = {                   /* 0x011c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient5"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient4"                },
};

/*-----------------------------------------------------------------------------
	0x0120 dig_filt_ctrl3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_DIG_FILT_CTRL3[] = {                   /* 0x0120 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient7"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient6"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "fir_coeff_load"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "fir_en"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "fir_sample_en"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,29, "fir_sampling_rate"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "fir_sample_c"                    },
};

/*-----------------------------------------------------------------------------
	0x0124 csc_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL0[] = {                        /* 0x0124 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient1"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient0"                },
};

/*-----------------------------------------------------------------------------
	0x0128 csc_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL1[] = {                        /* 0x0128 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient3"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient2"                },
};

/*-----------------------------------------------------------------------------
	0x012c csc_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL2[] = {                        /* 0x012c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient5"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient4"                },
};

/*-----------------------------------------------------------------------------
	0x0130 csc_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL3[] = {                        /* 0x0130 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient7"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient6"                },
};

/*-----------------------------------------------------------------------------
	0x0134 csc_ctrl3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL4[] = {                        /* 0x0134 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "csc_offset0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient8"                },
};

/*-----------------------------------------------------------------------------
	0x0138 csc_ctrl4 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL5[] = {                        /* 0x0138 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "csc_offset2"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "csc_offset1"                     },
};

/*-----------------------------------------------------------------------------
	0x013c csc_ctrl5 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL6[] = {                        /* 0x013c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "csc_offset4"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "csc_offset3"                     },
};

/*-----------------------------------------------------------------------------
	0x0140 csc_ctrl6 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_CSC_CTRL7[] = {                        /* 0x0140 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "csc_coeff_load"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "csc_en"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "csc_offset5"                     },
};

/*-----------------------------------------------------------------------------
	0x0144 misc_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_MISC_CTRL[] = {                        /* 0x0144 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "pattern_type"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "pattern_csc"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "pattern_detail"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "reg_update_pulse"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 9, "field_fix_mode"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "sync_extend"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "write_inhibit"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "reg_update_position"             },
};

/*-----------------------------------------------------------------------------
	0x0148 offset_read ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_OFFSET_READ[] = {                      /* 0x0148 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "hoffset_read"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "voffset_read"                    },
};

/*-----------------------------------------------------------------------------
	0x014c size_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_INTR_MSK[] = {                        /* 0x014c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "sd_vd_range"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "sd_ext_range"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "intr_mask_node"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "intr_mask_nohsync"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "intrl_mask_novsync"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "intr_mask_abnormal"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "intr_mask_invalid"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "intr_mask_de"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "intr_mask_hsync"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "intr_mask_vsync"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "intr_mask_normal"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "intr_mask_valid"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "sd_hsync_sel"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "sd_en"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "sd_frame_cnt"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,20, "sd_line_num"                     },
};

/*-----------------------------------------------------------------------------
	0x0150 size_read ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_SIZE_READ[] = {                        /* 0x0150 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "line_num_read"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "line_width_read"                 },
};

/*-----------------------------------------------------------------------------
	0x0154 pre3d_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_PRE3D_CTRL0[] = {                      /* 0x0154 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "pre3d_sync_enable"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 1, "pre3d_sync_mode"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "pre3d_field_mode"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "pre3d_frame_lr_mode"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "pre3d_mid_vsync_bypass"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "pre3d_active_line_num"           },
};

/*-----------------------------------------------------------------------------
	0x0158 pre3d_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_PRE3D_CTRL1[] = {                      /* 0x0158 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "pre3d_active_space1_line_num"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "pre3d_active_space2_line_num"    },
};

/*-----------------------------------------------------------------------------
	0x015c pre3d_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_PRE3D_CTRL2[] = {                      /* 0x015c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "pre3d_front_porch_line_num"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "pre3d_sync_line_num"             },
};

/*-----------------------------------------------------------------------------
	0x0160 size_status0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_SIZE_STATUS0[] = {                     /* 0x0160 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "current_line_number_read"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "de_signal_detect"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "hsync_signal_detect"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "vsync_signal_detect"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "source_valid"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "sd_hsync_fault_flag_read"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "sd_vsync_fault_flag_read"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,24, "sd_hsync_novalid_size"           },
};

/*-----------------------------------------------------------------------------
	0x0164 size_status1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHA_SIZE_STATUS1[] = {                     /* 0x0164 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "sd_hoffset_indurance_read"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "sd_voffset_indurance_read"       },
};

/*-----------------------------------------------------------------------------
	0x0180 top_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_TOP_CTRL[] = {                         /* 0x0180 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "scan_type"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 1, "field_gen_mode"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 3, "auto_field_gen"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "data_bit_mode"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "chroma_sampling"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "aspect_ratio"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 8, "yc_delay_mode_r"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,10, "yc_delay_mode_b"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "yc_delay_mode_g"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "auto_sync_polarity"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "manual_sync_polarity"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "adc_cut_direction"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "adc_bit_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "br_port_swap"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "r_port_swap"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "b_port_swap"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "g_port_swap"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,28, "bit_numbering_lsb2"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "data_en_mode"                    },
};

/*-----------------------------------------------------------------------------
	0x0184 offset_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_OFFSET_CTRL[] = {                      /* 0x0184 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "offset_h"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "offset_v"                        },
};

/*-----------------------------------------------------------------------------
	0x0188 size_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_SIZE_CTRL[] = {                        /* 0x0188 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "size_h"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "size_v"                          },
};

/*-----------------------------------------------------------------------------
	0x018c int_sync_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_INT_SYNC_CTRL0[] = {                   /* 0x018c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "res_hsync"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "res_hfp"                         },
};

/*-----------------------------------------------------------------------------
	0x0190 int_sync_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_INT_SYNC_CTRL1[] = {                   /* 0x0190 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 0, "res_vfp"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12, 8, "res_vsync"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,13, "res_field_add_line"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "res_field_sel"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "res_sync_sel"                    },
};

/*-----------------------------------------------------------------------------
	0x0194 dig_filt_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_DIG_FILT_CTRL0[] = {                   /* 0x0194 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient1"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient0"                },
};

/*-----------------------------------------------------------------------------
	0x0198 dig_filt_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_DIG_FILT_CTRL1[] = {                   /* 0x0198 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient3"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient2"                },
};

/*-----------------------------------------------------------------------------
	0x019c dig_filt_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_DIG_FILT_CTRL2[] = {                   /* 0x019c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient5"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient4"                },
};

/*-----------------------------------------------------------------------------
	0x01a0 dig_filt_ctrl3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_DIG_FILT_CTRL3[] = {                   /* 0x01a0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "fir_coefficient7"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "fir_coefficient6"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "fir_coeff_load"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "fir_en"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "fir_sample_en"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,29, "fir_sampling_rate"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "fir_sample_c"                    },
};

/*-----------------------------------------------------------------------------
	0x01a4 csc_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL0[] = {                        /* 0x01a4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient1"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient0"                },
};

/*-----------------------------------------------------------------------------
	0x01a8 csc_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL1[] = {                        /* 0x01a8 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient3"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient2"                },
};

/*-----------------------------------------------------------------------------
	0x01ac csc_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL2[] = {                        /* 0x01ac */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient5"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient4"                },
};

/*-----------------------------------------------------------------------------
	0x01b0 csc_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL3[] = {                        /* 0x01b0 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14, 0, "csc_coefficient7"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient6"                },
};

/*-----------------------------------------------------------------------------
	0x01b4 csc_ctrl3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL4[] = {                        /* 0x01b4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "csc_offset0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,16, "csc_coefficient8"                },
};

/*-----------------------------------------------------------------------------
	0x01b8 csc_ctrl4 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL5[] = {                        /* 0x01b8 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "csc_offset2"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "csc_offset1"                     },
};

/*-----------------------------------------------------------------------------
	0x01bc csc_ctrl5 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL6[] = {                        /* 0x01bc */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "csc_offset4"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "csc_offset3"                     },
};

/*-----------------------------------------------------------------------------
	0x01c0 csc_ctrl6 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_CSC_CTRL7[] = {                        /* 0x01c0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "csc_coeff_load"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "csc_en"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "csc_offset5"                     },
};

/*-----------------------------------------------------------------------------
	0x01c4 misc_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_MISC_CTRL[] = {                        /* 0x01c4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "pattern_type"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "pattern_csc"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "pattern_detail"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "reg_update_pulse"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 9, "field_fix_mode"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "sync_extend"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "write_inhibit"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "reg_update_position"             },
};

/*-----------------------------------------------------------------------------
	0x01c8 offset_read ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_OFFSET_READ[] = {                      /* 0x01c8 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "hoffset_read"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "voffset_read"                    },
};

/*-----------------------------------------------------------------------------
	0x01cc size_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_INTR_MSK[] = {                        /* 0x01cc */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "sd_vd_range"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "sd_ext_range"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "intr_mask_node"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "intr_mask_nohsync"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "intrl_mask_novsync"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "intr_mask_abnormal"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "intr_mask_invalid"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "intr_mask_de"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "intr_mask_hsync"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "intr_mask_vsync"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "intr_mask_normal"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "intr_mask_valid"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "sd_hsync_sel"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "sd_en"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "sd_frame_cnt"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,20, "sd_line_num"                     },
};

/*-----------------------------------------------------------------------------
	0x01d0 size_read ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_SIZE_READ[] = {                        /* 0x01d0 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "line_num_read"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "line_width_read"                 },
};

/*-----------------------------------------------------------------------------
	0x01d4 pre3d_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_PRE3D_CTRL0[] = {                      /* 0x01d4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "pre3d_sync_enable"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 1, "pre3d_sync_mode"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "pre3d_field_mode"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "pre3d_frame_lr_mode"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "pre3d_mid_vsync_bypass"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "pre3d_active_line_num"           },
};

/*-----------------------------------------------------------------------------
	0x01d8 pre3d_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_PRE3D_CTRL1[] = {                      /* 0x01d8 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "pre3d_active_space1_line_num"    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "pre3d_active_space2_line_num"    },
};

/*-----------------------------------------------------------------------------
	0x01dc pre3d_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_PRE3D_CTRL2[] = {                      /* 0x01dc */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "pre3d_front_porch_line_num"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "pre3d_sync_line_num"             },
};

/*-----------------------------------------------------------------------------
	0x01e0 size_status0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CHB_SIZE_STATUS0[] = {                     /* 0x01e0 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "current_line_number_read"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "de_signal_detect"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "hsync_signal_detect"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "vsync_signal_detect"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "source_valid"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "sd_hsync_fault_flag_read"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "sd_vsync_fault_flag_read"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,24, "sd_hsync_novalid_size"           },
};

/*-----------------------------------------------------------------------------
	0x01e4 size_status1 ''
------------------------------------------------------------------------------*/
/*	no field */

DBG_REG_T gDbgRegCVI[] = {
	{0x0100,N_FLD(dbg_CHA_TOP_CTRL),                        "TOP_CTRL"                        ,dbg_CHA_TOP_CTRL                        }, //
	{0x0104,N_FLD(dbg_CHA_OFFSET_CTRL),                     "OFFSET_CTRL"                     ,dbg_CHA_OFFSET_CTRL                     }, //
	{0x0108,N_FLD(dbg_CHA_INTR_MSK),                       	"INTR_MSK"                        ,dbg_CHA_INTR_MSK                        }, //
	{0x010c,N_FLD(dbg_CHA_INT_SYNC_CTRL0),                  "INT_SYNC_CTRL0"                  ,dbg_CHA_INT_SYNC_CTRL0                  }, //
	{0x0110,N_FLD(dbg_CHA_INT_SYNC_CTRL1),                  "INT_SYNC_CTRL1"                  ,dbg_CHA_INT_SYNC_CTRL1                  }, //
	{0x0114,N_FLD(dbg_CHA_DIG_FILT_CTRL0),                  "DIG_FILT_CTRL0"                  ,dbg_CHA_DIG_FILT_CTRL0                  }, //
	{0x0118,N_FLD(dbg_CHA_DIG_FILT_CTRL1),                  "DIG_FILT_CTRL1"                  ,dbg_CHA_DIG_FILT_CTRL1                  }, //
	{0x011c,N_FLD(dbg_CHA_DIG_FILT_CTRL2),                  "DIG_FILT_CTRL2"                  ,dbg_CHA_DIG_FILT_CTRL2                  }, //
	{0x0120,N_FLD(dbg_CHA_DIG_FILT_CTRL3),                  "DIG_FILT_CTRL3"                  ,dbg_CHA_DIG_FILT_CTRL3                  }, //
	{0x0124,N_FLD(dbg_CHA_CSC_CTRL0),                       "CSC_CTRL0"                       ,dbg_CHA_CSC_CTRL0                       }, //
	{0x0128,N_FLD(dbg_CHA_CSC_CTRL1),                       "CSC_CTRL1"                       ,dbg_CHA_CSC_CTRL1                       }, //
	{0x012c,N_FLD(dbg_CHA_CSC_CTRL2),                       "CSC_CTRL2"                       ,dbg_CHA_CSC_CTRL2                       }, //
	{0x0130,N_FLD(dbg_CHA_CSC_CTRL3),                       "CSC_CTRL2"                       ,dbg_CHA_CSC_CTRL3                       }, //
	{0x0134,N_FLD(dbg_CHA_CSC_CTRL4),                       "CSC_CTRL3"                       ,dbg_CHA_CSC_CTRL4                       }, //
	{0x0138,N_FLD(dbg_CHA_CSC_CTRL5),                       "CSC_CTRL4"                       ,dbg_CHA_CSC_CTRL5                       }, //
	{0x013c,N_FLD(dbg_CHA_CSC_CTRL6),                       "CSC_CTRL5"                       ,dbg_CHA_CSC_CTRL6                       }, //
	{0x0140,N_FLD(dbg_CHA_CSC_CTRL7),                       "CSC_CTRL6"                       ,dbg_CHA_CSC_CTRL7                       }, //
	{0x0144,N_FLD(dbg_CHA_MISC_CTRL),                       "MISC_CTRL"                       ,dbg_CHA_MISC_CTRL                       }, //
	{0x0148,N_FLD(dbg_CHA_OFFSET_READ),                     "OFFSET_READ"                     ,dbg_CHA_OFFSET_READ                     }, //
	{0x014c,N_FLD(dbg_CHA_SIZE_CTRL),                       "SIZE_CTRL"                       ,dbg_CHA_SIZE_CTRL                       }, //
	{0x0150,N_FLD(dbg_CHA_SIZE_READ),                       "SIZE_READ"                       ,dbg_CHA_SIZE_READ                       }, //
	{0x0154,N_FLD(dbg_CHA_PRE3D_CTRL0),                     "PRE3D_CTRL0"                     ,dbg_CHA_PRE3D_CTRL0                     }, //
	{0x0158,N_FLD(dbg_CHA_PRE3D_CTRL1),                     "PRE3D_CTRL1"                     ,dbg_CHA_PRE3D_CTRL1                     }, //
	{0x015c,N_FLD(dbg_CHA_PRE3D_CTRL2),                     "PRE3D_CTRL2"                     ,dbg_CHA_PRE3D_CTRL2                     }, //
	{0x0160,N_FLD(dbg_CHA_SIZE_STATUS0),                    "SIZE_STATUS0"                    ,dbg_CHA_SIZE_STATUS0                    }, //
	{0x0164,N_FLD(dbg_CHA_SIZE_STATUS1),                    "SIZE_STATUS1"                    ,dbg_CHA_SIZE_STATUS1                    }, //
	{0x0180,N_FLD(dbg_CHB_TOP_CTRL),                        "TOP_CTRL"                        ,dbg_CHA_TOP_CTRL                        }, //
	{0x0184,N_FLD(dbg_CHB_OFFSET_CTRL),                     "OFFSET_CTRL"                     ,dbg_CHA_OFFSET_CTRL                     }, //
	{0x0188,N_FLD(dbg_CHB_INTR_MSK),                        "INTR_MSK"                        ,dbg_CHB_INTR_MSK                        }, //
	{0x018c,N_FLD(dbg_CHB_INT_SYNC_CTRL0),                  "INT_SYNC_CTRL0"                  ,dbg_CHB_INT_SYNC_CTRL0                  }, //
	{0x0190,N_FLD(dbg_CHB_INT_SYNC_CTRL1),                  "INT_SYNC_CTRL1"                  ,dbg_CHB_INT_SYNC_CTRL1                  }, //
	{0x0194,N_FLD(dbg_CHB_DIG_FILT_CTRL0),                  "DIG_FILT_CTRL0"                  ,dbg_CHB_DIG_FILT_CTRL0                  }, //
	{0x0198,N_FLD(dbg_CHB_DIG_FILT_CTRL1),                  "DIG_FILT_CTRL1"                  ,dbg_CHB_DIG_FILT_CTRL1                  }, //
	{0x019c,N_FLD(dbg_CHB_DIG_FILT_CTRL2),                  "DIG_FILT_CTRL2"                  ,dbg_CHB_DIG_FILT_CTRL2                  }, //
	{0x01a0,N_FLD(dbg_CHB_DIG_FILT_CTRL3),                  "DIG_FILT_CTRL3"                  ,dbg_CHB_DIG_FILT_CTRL3                  }, //
	{0x01a4,N_FLD(dbg_CHB_CSC_CTRL0),                       "CSC_CTRL0"                       ,dbg_CHB_CSC_CTRL0                       }, //
	{0x01a8,N_FLD(dbg_CHB_CSC_CTRL1),                       "CSC_CTRL1"                       ,dbg_CHB_CSC_CTRL1                       }, //
	{0x01ac,N_FLD(dbg_CHB_CSC_CTRL2),                       "CSC_CTRL2"                       ,dbg_CHB_CSC_CTRL2                       }, //
	{0x01b0,N_FLD(dbg_CHB_CSC_CTRL3),                       "CSC_CTRL2"                       ,dbg_CHB_CSC_CTRL3                       }, //
	{0x01b4,N_FLD(dbg_CHB_CSC_CTRL4),                       "CSC_CTRL3"                       ,dbg_CHB_CSC_CTRL4                       }, //
	{0x01b8,N_FLD(dbg_CHB_CSC_CTRL5),                       "CSC_CTRL4"                       ,dbg_CHB_CSC_CTRL5                       }, //
	{0x01bc,N_FLD(dbg_CHB_CSC_CTRL6),                       "CSC_CTRL5"                       ,dbg_CHB_CSC_CTRL6                       }, //
	{0x01c0,N_FLD(dbg_CHB_CSC_CTRL7),                       "CSC_CTRL6"                       ,dbg_CHB_CSC_CTRL7                       }, //
	{0x01c4,N_FLD(dbg_CHB_MISC_CTRL),                       "MISC_CTRL"                       ,dbg_CHB_MISC_CTRL                       }, //
	{0x01c8,N_FLD(dbg_CHB_OFFSET_READ),                     "OFFSET_READ"                     ,dbg_CHB_OFFSET_READ                     }, //
	{0x01cc,N_FLD(dbg_CHB_SIZE_CTRL),                       "SIZE_CTRL"                       ,dbg_CHB_SIZE_CTRL                       }, //
	{0x01d0,N_FLD(dbg_CHB_SIZE_READ),                       "SIZE_READ"                       ,dbg_CHB_SIZE_READ                       }, //
	{0x01d4,N_FLD(dbg_CHB_PRE3D_CTRL0),                     "PRE3D_CTRL0"                     ,dbg_CHB_PRE3D_CTRL0                     }, //
	{0x01d8,N_FLD(dbg_CHB_PRE3D_CTRL1),                     "PRE3D_CTRL1"                     ,dbg_CHB_PRE3D_CTRL1                     }, //
	{0x01dc,N_FLD(dbg_CHB_PRE3D_CTRL2),                     "PRE3D_CTRL2"                     ,dbg_CHB_PRE3D_CTRL2                     }, //
	{0x01e0,N_FLD(dbg_CHB_SIZE_STATUS0),                    "SIZE_STATUS0"                    ,dbg_CHB_SIZE_STATUS0                    }, //
	{0x01e4,0,                                  			"SIZE_STATUS1"                    ,NULL                                    }, //
};

/* 52 regs, 52 types in Total*/

/* from 'L8-DE-CVI_RegFile_100105.csv' 20100311 00:54:15   대한민국 표준시 by getregs v2.3 */
