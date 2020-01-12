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
	0x1100 ov_ctrl0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_CTRL0[] = {                         /* 0x1100 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "main_444"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 4, "main_lsr_2b"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 6, "main_cflt_mode"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "sub_444"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "sub_lsr_2b"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,14, "sub_cflt_mode"                   },
};

/*-----------------------------------------------------------------------------
	0x1104 ov_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_CTRL1[] = {                         /* 0x1104 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "border_color_cr"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "border_color_cb"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "border_color_yy"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "load_enable"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "load_type"                       },
};

/*-----------------------------------------------------------------------------
	0x1108 ov_ctrl2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_CTRL2[] = {                         /* 0x1108 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "back_color_cr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "back_color_cb"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "back_color_yy"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "dark_color_cr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "dark_color_cb"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "dark_color_yy"                   },
};

/*-----------------------------------------------------------------------------
	0x110c ov_ctrl3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_CTRL3[] = {                         /* 0x110c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "w0_blending_alpha"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "w1_blending_alpha"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "w0_blend_en"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "w1_blend_en"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "w1_blend_with"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "large_win"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "w0_dark_en"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "w1_dark_en"                      },
};

/*-----------------------------------------------------------------------------
	0x1110 ov_moffset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_MOFFSET[] = {                       /* 0x1110 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "w0_hoffset"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "w0_voffset"                      },
};

/*-----------------------------------------------------------------------------
	0x1114 ov_msize ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_MSIZE[] = {                         /* 0x1114 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "w0_hsize"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "w0_vsize"                        },
};

/*-----------------------------------------------------------------------------
	0x1118 ov_soffset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_SOFFSET[] = {                       /* 0x1118 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "w1_hoffset"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "w1_voffset"                      },
};

/*-----------------------------------------------------------------------------
	0x111c ov_ssize ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_SSIZE[] = {                         /* 0x111c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "w1_hsize"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "w1_vsize"                        },
};

/*-----------------------------------------------------------------------------
	0x1120 ov_border_width ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_BORDER_WIDTH[] = {                  /* 0x1120 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "w1_right_wid"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "w1_left_wid"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "w1_down_wid"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "w1_up_wid"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "w0_right_wid"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "w0_left_wid"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "w0_down_wid"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "w0_up_wid"                       },
};

/*-----------------------------------------------------------------------------
	0x1124 cflt_coef0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CFLT_COEF0[] = {                       /* 0x1124 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "cflt_coef_1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "cflt_coef_0"                     },
};

/*-----------------------------------------------------------------------------
	0x1128 cflt_coef1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CFLT_COEF1[] = {                       /* 0x1128 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "cflt_coef_3"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "cflt_coef_2"                     },
};

/*-----------------------------------------------------------------------------
	0x1130 ov_param_by_ddr ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_PARAM_BY_DDR[] = {                  /* 0x1130 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 9, "stride"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "vsize"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "endian_conversion"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "update_by_ddr"                   },
};

/*-----------------------------------------------------------------------------
	0x1140 ov_intr_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_INTR_CTRL[] = {                     /* 0x1140 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "intr_enable"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "intr_line_pos_enable"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "intr_sbuf_c_empty_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "intr_sbuf_y_empty_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "intr_mbuf_c_empty_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "intr_mbuf_y_empty_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "intr_pdb_end_en"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "line_position"                   },
};

/*-----------------------------------------------------------------------------
	0x1144 ov_intr_status ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OV_INTR_STATUS[] = {                   /* 0x1144 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "sbuf_c_empty"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "sbuf_y_empty"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "mbuf_c_empty"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "mbuf_y_empty"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "line_cnt"                        },
};

DBG_REG_T gDbgRegOV[] = {
	{0x1100,N_FLD(dbg_OV_CTRL0),                        "OV_CTRL0"                        ,dbg_OV_CTRL0                        }, //
	{0x1104,N_FLD(dbg_OV_CTRL1),                        "OV_CTRL1"                        ,dbg_OV_CTRL1                        }, //
	{0x1108,N_FLD(dbg_OV_CTRL2),                        "OV_CTRL2"                        ,dbg_OV_CTRL2                        }, //
	{0x110c,N_FLD(dbg_OV_CTRL3),                        "OV_CTRL3"                        ,dbg_OV_CTRL3                        }, //
	{0x1110,N_FLD(dbg_OV_MOFFSET),                      "OV_MOFFSET"                      ,dbg_OV_MOFFSET                      }, //
	{0x1114,N_FLD(dbg_OV_MSIZE),                        "OV_MSIZE"                        ,dbg_OV_MSIZE                        }, //
	{0x1118,N_FLD(dbg_OV_SOFFSET),                      "OV_SOFFSET"                      ,dbg_OV_SOFFSET                      }, //
	{0x111c,N_FLD(dbg_OV_SSIZE),                        "OV_SSIZE"                        ,dbg_OV_SSIZE                        }, //
	{0x1120,N_FLD(dbg_OV_BORDER_WIDTH),                 "OV_BORDER_WIDTH"                 ,dbg_OV_BORDER_WIDTH                 }, //
	{0x1124,N_FLD(dbg_CFLT_COEF0),                      "CFLT_COEF0"                      ,dbg_CFLT_COEF0                      }, //
	{0x1128,N_FLD(dbg_CFLT_COEF1),                      "CFLT_COEF1"                      ,dbg_CFLT_COEF1                      }, //
	{0x1130,N_FLD(dbg_OV_PARAM_BY_DDR),                 "OV_PARAM_BY_DDR"                 ,dbg_OV_PARAM_BY_DDR                 }, //
	{0x1140,N_FLD(dbg_OV_INTR_CTRL),                    "OV_INTR_CTRL"                    ,dbg_OV_INTR_CTRL                    }, //
	{0x1144,N_FLD(dbg_OV_INTR_STATUS),                  "OV_INTR_STATUS"                  ,dbg_OV_INTR_STATUS                  }, //
};

/* 14 regs, 14 types in Total*/

/* from 'LG1150-DE-MAN-06 v0.5 (Window Composer Register Description-100107).csv' 20100311 00:54:18   대한민국 표준시 by getregs v2.3 */
