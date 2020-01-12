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
	0x0900 ssc_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_CTRL[] = {                         /* 0x0900 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "bnd_pel_filtering_scheme"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "fir_config"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "filter_type_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "fir_coef_table_sel"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "size_info_manual_load"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "ddr_update_endian_change"        },
};

/*-----------------------------------------------------------------------------
	0x0904 ssc_field ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_FIELD[] = {                        /* 0x0904 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "interlace_mode"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "curr_field"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "mpeg_stream"                     },
};

/*-----------------------------------------------------------------------------
	0x0908 ssc_y_init_pos_h ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_Y_INIT_POS_H[] = {                 /* 0x0908 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_init_pos_h"                    },
};

/*-----------------------------------------------------------------------------
	0x090c ssc_y_init_pos_v ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_Y_INIT_POS_V[] = {                 /* 0x090c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_init_pos_v"                    },
};

/*-----------------------------------------------------------------------------
	0x0910 ssc_c_init_pos_h ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_C_INIT_POS_H[] = {                 /* 0x0910 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_init_pos_h"                    },
};

/*-----------------------------------------------------------------------------
	0x0914 ssc_c_init_pos_v ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_C_INIT_POS_V[] = {                 /* 0x0914 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_init_pos_v"                    },
};

/*-----------------------------------------------------------------------------
	0x0918 ssc_scr_size ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_SCR_SIZE[] = {                     /* 0x0918 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "input_h_size"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "input_v_size"                    },
};

/*-----------------------------------------------------------------------------
	0x091c ssc_win_out_size ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_WIN_OUT_SIZE[] = {                 /* 0x091c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "out_win_h_size"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "out_win_v_size"                  },
};

/*-----------------------------------------------------------------------------
	0x0920 ssc_win_in_offset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_WIN_IN_OFFSET[] = {                /* 0x0920 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "in_win_h_offset"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "in_win_v_offset"                 },
};

/*-----------------------------------------------------------------------------
	0x0924 ssc_win_in_size ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_WIN_IN_SIZE[] = {                  /* 0x0924 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "in_win_h_size"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "in_win_v_size"                   },
};

/*-----------------------------------------------------------------------------
	0x0928 ssc_c_format ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_C_FORMAT[] = {                     /* 0x0928 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "in_c_format"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "out_c_format"                    },
};

/*-----------------------------------------------------------------------------
	0x0930 ssc_y_fir_coef_data0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_Y_FIR_COEF_DATA0[] = {             /* 0x0930 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "y_coefficient0"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "y_coefficient1"                  },
};

/*-----------------------------------------------------------------------------
	0x0934 ssc_y_fir_coef_data1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_Y_FIR_COEF_DATA1[] = {             /* 0x0934 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "y_coefficient2"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "y_coefficient3"                  },
};

/*-----------------------------------------------------------------------------
	0x0938 ssc_y_fir_coef_data2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_Y_FIR_COEF_DATA2[] = {             /* 0x0938 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "y_coefficient4"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "y_coefficient5"                  },
};

/*-----------------------------------------------------------------------------
	0x093c ssc_y_fir_coef_data3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_Y_FIR_COEF_DATA3[] = {             /* 0x093c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "y_coefficient6"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,16, "y_coefficient7"                  },
};

/*-----------------------------------------------------------------------------
	0x0940 ssc_y_fir_coef_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_Y_FIR_COEF_CTRL[] = {              /* 0x0940 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "addr"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "h_v_sel"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "update"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "rnw"                             },
};

/*-----------------------------------------------------------------------------
	0x0944 ssc_update_from_ddr ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_UPDATE_FROM_DDR[] = {              /* 0x0944 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "ddr_stride"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "ddr_line_cnt"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "ddr_update_on"                   },
};

/*-----------------------------------------------------------------------------
	0x0968 ssc_status_rev ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_STATUS_REV[] = {                   /* 0x0968 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "y_status"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "c_status"                        },
};

/*-----------------------------------------------------------------------------
	0x096c ssc_manual_load ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SSC_MANUAL_LOAD[] = {                  /* 0x096c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "update_input_frm_size"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "update_wout_size"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "update_win_offset"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "update_win_size"                 },
};

DBG_REG_T gDbgRegSSC[] = {
	{0x0900,N_FLD(dbg_SSC_CTRL),                        "SSC_CTRL"                        ,dbg_SSC_CTRL                        }, //
	{0x0904,N_FLD(dbg_SSC_FIELD),                       "SSC_FIELD"                       ,dbg_SSC_FIELD                       }, //
	{0x0908,N_FLD(dbg_SSC_Y_INIT_POS_H),                "SSC_Y_INIT_POS_H"                ,dbg_SSC_Y_INIT_POS_H                }, //
	{0x090c,N_FLD(dbg_SSC_Y_INIT_POS_V),                "SSC_Y_INIT_POS_V"                ,dbg_SSC_Y_INIT_POS_V                }, //
	{0x0910,N_FLD(dbg_SSC_C_INIT_POS_H),                "SSC_C_INIT_POS_H"                ,dbg_SSC_C_INIT_POS_H                }, //
	{0x0914,N_FLD(dbg_SSC_C_INIT_POS_V),                "SSC_C_INIT_POS_V"                ,dbg_SSC_C_INIT_POS_V                }, //
	{0x0918,N_FLD(dbg_SSC_SCR_SIZE),                    "SSC_SCR_SIZE"                    ,dbg_SSC_SCR_SIZE                    }, //
	{0x091c,N_FLD(dbg_SSC_WIN_OUT_SIZE),                "SSC_WIN_OUT_SIZE"                ,dbg_SSC_WIN_OUT_SIZE                }, //
	{0x0920,N_FLD(dbg_SSC_WIN_IN_OFFSET),               "SSC_WIN_IN_OFFSET"               ,dbg_SSC_WIN_IN_OFFSET               }, //
	{0x0924,N_FLD(dbg_SSC_WIN_IN_SIZE),                 "SSC_WIN_IN_SIZE"                 ,dbg_SSC_WIN_IN_SIZE                 }, //
	{0x0928,N_FLD(dbg_SSC_C_FORMAT),                    "SSC_C_FORMAT"                    ,dbg_SSC_C_FORMAT                    }, //
	{0x0930,N_FLD(dbg_SSC_Y_FIR_COEF_DATA0),            "SSC_Y_FIR_COEF_DATA0"            ,dbg_SSC_Y_FIR_COEF_DATA0            }, //
	{0x0934,N_FLD(dbg_SSC_Y_FIR_COEF_DATA1),            "SSC_Y_FIR_COEF_DATA1"            ,dbg_SSC_Y_FIR_COEF_DATA1            }, //
	{0x0938,N_FLD(dbg_SSC_Y_FIR_COEF_DATA2),            "SSC_Y_FIR_COEF_DATA2"            ,dbg_SSC_Y_FIR_COEF_DATA2            }, //
	{0x093c,N_FLD(dbg_SSC_Y_FIR_COEF_DATA3),            "SSC_Y_FIR_COEF_DATA3"            ,dbg_SSC_Y_FIR_COEF_DATA3            }, //
	{0x0940,N_FLD(dbg_SSC_Y_FIR_COEF_CTRL),             "SSC_Y_FIR_COEF_CTRL"             ,dbg_SSC_Y_FIR_COEF_CTRL             }, //
	{0x0944,N_FLD(dbg_SSC_UPDATE_FROM_DDR),             "SSC_UPDATE_FROM_DDR"             ,dbg_SSC_UPDATE_FROM_DDR             }, //
	{0x0968,N_FLD(dbg_SSC_STATUS_REV),                  "SSC_STATUS_REV"                  ,dbg_SSC_STATUS_REV                  }, //
	{0x096c,N_FLD(dbg_SSC_MANUAL_LOAD),                 "SSC_MANUAL_LOAD"                 ,dbg_SSC_MANUAL_LOAD                 }, //
};

/* 19 regs, 19 types in Total*/

/* from 'L8-DE-SSC_RegFile_091215.csv' 20100311 00:54:15   대한민국 표준시 by getregs v2.3 */
