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
	0x0500 msc_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_CTRL[] = {                         /* 0x0500 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "bnd_pel_filtering_scheme"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "adaptive_fir_filter_en"          },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "v_scaler_config"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "filter_type_sel_luma"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "filter_type_sel_chroma"          },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "yuv444_sampling_mode"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "ddr_update_endian_change"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "size_info_manual_load"           },
};

/*-----------------------------------------------------------------------------
	0x0504 msc_y_init_pos_h ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_INIT_POS_H[] = {                 /* 0x0504 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_init_pos_h"                    },
};

/*-----------------------------------------------------------------------------
	0x0508 msc_y_init_pos_v ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_INIT_POS_V[] = {                 /* 0x0508 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_init_pos_v"                    },
};

/*-----------------------------------------------------------------------------
	0x050c msc_c_init_pos_h ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_INIT_POS_H[] = {                 /* 0x050c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_init_pos_h"                    },
};

/*-----------------------------------------------------------------------------
	0x0510 msc_c_init_pos_v ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_INIT_POS_V[] = {                 /* 0x0510 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_init_pos_v"                    },
};

/*-----------------------------------------------------------------------------
	0x0514 msc_adaptive_th ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_ADAPTIVE_TH[] = {                  /* 0x0514 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "reg_th0"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "reg_th1"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 4, "reg_th2"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 6, "reg_th3"                         },
};

/*-----------------------------------------------------------------------------
	0x0518 msc_scr_size ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_SCR_SIZE[] = {                     /* 0x0518 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "input_h_size"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "input_v_size"                    },
};

/*-----------------------------------------------------------------------------
	0x051c msc_win_out_size ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_WIN_OUT_SIZE[] = {                 /* 0x051c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "out_win_h_size"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "out_win_v_size"                  },
};

/*-----------------------------------------------------------------------------
	0x0520 msc_win_in_offset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_WIN_IN_OFFSET[] = {                /* 0x0520 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "in_win_h_offset"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "in_win_v_offset"                 },
};

/*-----------------------------------------------------------------------------
	0x0524 msc_win_in_size ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_WIN_IN_SIZE[] = {                  /* 0x0524 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "in_win_h_size"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "in_win_v_size"                   },
};

/*-----------------------------------------------------------------------------
	0x0528 msc_c_format ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_FORMAT[] = {                     /* 0x0528 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "in_c_format"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "out_c_format"                    },
};

/*-----------------------------------------------------------------------------
	0x052c msc_3df_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_3DF_CTRL[] = {                     /* 0x052c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "tdf_enable"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "tdf_mode"                        },
};

/*-----------------------------------------------------------------------------
	0x0530 msc_y_fir_coef_data0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_FIR_COEF_DATA0[] = {             /* 0x0530 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_coefficient0"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "y_coefficient1"                  },
};

/*-----------------------------------------------------------------------------
	0x0534 msc_y_fir_coef_data1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_FIR_COEF_DATA1[] = {             /* 0x0534 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_coefficient2"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "y_coefficient3"                  },
};

/*-----------------------------------------------------------------------------
	0x0538 msc_y_fir_coef_data2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_FIR_COEF_DATA2[] = {             /* 0x0538 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_coefficient4"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "y_coefficient5"                  },
};

/*-----------------------------------------------------------------------------
	0x053c msc_y_fir_coef_data3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_FIR_COEF_DATA3[] = {             /* 0x053c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_coefficient6"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "y_coefficient7"                  },
};

/*-----------------------------------------------------------------------------
	0x0540 msc_y_fir_coef_data4 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_FIR_COEF_DATA4[] = {             /* 0x0540 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_coefficient8"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "y_coefficient9"                  },
};

/*-----------------------------------------------------------------------------
	0x0544 msc_y_fir_coef_data5 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_FIR_COEF_DATA5[] = {             /* 0x0544 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "y_coefficient10"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "y_coefficient11"                 },
};

/*-----------------------------------------------------------------------------
	0x0548 msc_y_fir_coef_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_Y_FIR_COEF_CTRL[] = {              /* 0x0548 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "addr"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "h_v_sel"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "update"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "rnw"                             },
};

/*-----------------------------------------------------------------------------
	0x0550 msc_c_fir_coef_data0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_FIR_COEF_DATA0[] = {             /* 0x0550 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_coefficient0"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "c_coefficient1"                  },
};

/*-----------------------------------------------------------------------------
	0x0554 msc_c_fir_coef_data1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_FIR_COEF_DATA1[] = {             /* 0x0554 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_coefficient2"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "c_coefficient3"                  },
};

/*-----------------------------------------------------------------------------
	0x0558 msc_c_fir_coef_data2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_FIR_COEF_DATA2[] = {             /* 0x0558 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_coefficient4"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "c_coefficient5"                  },
};

/*-----------------------------------------------------------------------------
	0x055c msc_c_fir_coef_data3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_FIR_COEF_DATA3[] = {             /* 0x055c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 0, "c_coefficient6"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,16, "c_coefficient7"                  },
};

/*-----------------------------------------------------------------------------
	0x0560 msc_c_fir_coef_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_C_FIR_COEF_CTRL[] = {              /* 0x0560 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "addr"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "h_v_sel"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "update"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "rnw"                             },
};

/*-----------------------------------------------------------------------------
	0x0564 msc_update_from_ddr ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_UPDATE_FROM_DDR[] = {              /* 0x0564 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "ddr_stride"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "ddr_line_cnt"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "ddr_update_on"                   },
};

/*-----------------------------------------------------------------------------
	0x0568 msc_status_rev ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_STATUS_REV[] = {                   /* 0x0568 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "y_status"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "c_status"                        },
};

/*-----------------------------------------------------------------------------
	0x056c msc_manual_load ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_MANUAL_LOAD[] = {                  /* 0x056c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "update_input_frm_size"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "update_wout_size"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "update_win_offset"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "update_win_size"                 },
};

/*-----------------------------------------------------------------------------
	0x0570 msc_wout_offset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MSC_WOUT_OFFSET[] = {                  /* 0x0570 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 0, "wout_y_offset"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,16, "wout_c_offset"                   },
};

DBG_REG_T gDbgRegMSC[] = {
	{0x0500,N_FLD(dbg_MSC_CTRL),                        "MSC_CTRL"                        ,dbg_MSC_CTRL                        }, //
	{0x0504,N_FLD(dbg_MSC_Y_INIT_POS_H),                "MSC_Y_INIT_POS_H"                ,dbg_MSC_Y_INIT_POS_H                }, //
	{0x0508,N_FLD(dbg_MSC_Y_INIT_POS_V),                "MSC_Y_INIT_POS_V"                ,dbg_MSC_Y_INIT_POS_V                }, //
	{0x050c,N_FLD(dbg_MSC_C_INIT_POS_H),                "MSC_C_INIT_POS_H"                ,dbg_MSC_C_INIT_POS_H                }, //
	{0x0510,N_FLD(dbg_MSC_C_INIT_POS_V),                "MSC_C_INIT_POS_V"                ,dbg_MSC_C_INIT_POS_V                }, //
	{0x0514,N_FLD(dbg_MSC_ADAPTIVE_TH),                 "MSC_ADAPTIVE_TH"                 ,dbg_MSC_ADAPTIVE_TH                 }, //
	{0x0518,N_FLD(dbg_MSC_SCR_SIZE),                    "MSC_SCR_SIZE"                    ,dbg_MSC_SCR_SIZE                    }, //
	{0x051c,N_FLD(dbg_MSC_WIN_OUT_SIZE),                "MSC_WIN_OUT_SIZE"                ,dbg_MSC_WIN_OUT_SIZE                }, //
	{0x0520,N_FLD(dbg_MSC_WIN_IN_OFFSET),               "MSC_WIN_IN_OFFSET"               ,dbg_MSC_WIN_IN_OFFSET               }, //
	{0x0524,N_FLD(dbg_MSC_WIN_IN_SIZE),                 "MSC_WIN_IN_SIZE"                 ,dbg_MSC_WIN_IN_SIZE                 }, //
	{0x0528,N_FLD(dbg_MSC_C_FORMAT),                    "MSC_C_FORMAT"                    ,dbg_MSC_C_FORMAT                    }, //
	{0x052c,N_FLD(dbg_MSC_3DF_CTRL),                    "MSC_3DF_CTRL"                    ,dbg_MSC_3DF_CTRL                    }, //
	{0x0530,N_FLD(dbg_MSC_Y_FIR_COEF_DATA0),            "MSC_Y_FIR_COEF_DATA0"            ,dbg_MSC_Y_FIR_COEF_DATA0            }, //
	{0x0534,N_FLD(dbg_MSC_Y_FIR_COEF_DATA1),            "MSC_Y_FIR_COEF_DATA1"            ,dbg_MSC_Y_FIR_COEF_DATA1            }, //
	{0x0538,N_FLD(dbg_MSC_Y_FIR_COEF_DATA2),            "MSC_Y_FIR_COEF_DATA2"            ,dbg_MSC_Y_FIR_COEF_DATA2            }, //
	{0x053c,N_FLD(dbg_MSC_Y_FIR_COEF_DATA3),            "MSC_Y_FIR_COEF_DATA3"            ,dbg_MSC_Y_FIR_COEF_DATA3            }, //
	{0x0540,N_FLD(dbg_MSC_Y_FIR_COEF_DATA4),            "MSC_Y_FIR_COEF_DATA4"            ,dbg_MSC_Y_FIR_COEF_DATA4            }, //
	{0x0544,N_FLD(dbg_MSC_Y_FIR_COEF_DATA5),            "MSC_Y_FIR_COEF_DATA5"            ,dbg_MSC_Y_FIR_COEF_DATA5            }, //
	{0x0548,N_FLD(dbg_MSC_Y_FIR_COEF_CTRL),             "MSC_Y_FIR_COEF_CTRL"             ,dbg_MSC_Y_FIR_COEF_CTRL             }, //
	{0x0550,N_FLD(dbg_MSC_C_FIR_COEF_DATA0),            "MSC_C_FIR_COEF_DATA0"            ,dbg_MSC_C_FIR_COEF_DATA0            }, //
	{0x0554,N_FLD(dbg_MSC_C_FIR_COEF_DATA1),            "MSC_C_FIR_COEF_DATA1"            ,dbg_MSC_C_FIR_COEF_DATA1            }, //
	{0x0558,N_FLD(dbg_MSC_C_FIR_COEF_DATA2),            "MSC_C_FIR_COEF_DATA2"            ,dbg_MSC_C_FIR_COEF_DATA2            }, //
	{0x055c,N_FLD(dbg_MSC_C_FIR_COEF_DATA3),            "MSC_C_FIR_COEF_DATA3"            ,dbg_MSC_C_FIR_COEF_DATA3            }, //
	{0x0560,N_FLD(dbg_MSC_C_FIR_COEF_CTRL),             "MSC_C_FIR_COEF_CTRL"             ,dbg_MSC_C_FIR_COEF_CTRL             }, //
	{0x0564,N_FLD(dbg_MSC_UPDATE_FROM_DDR),             "MSC_UPDATE_FROM_DDR"             ,dbg_MSC_UPDATE_FROM_DDR             }, //
	{0x0568,N_FLD(dbg_MSC_STATUS_REV),                  "MSC_STATUS_REV"                  ,dbg_MSC_STATUS_REV                  }, //
	{0x056c,N_FLD(dbg_MSC_MANUAL_LOAD),                 "MSC_MANUAL_LOAD"                 ,dbg_MSC_MANUAL_LOAD                 }, //
	{0x0570,N_FLD(dbg_MSC_WOUT_OFFSET),                 "MSC_WOUT_OFFSET"                 ,dbg_MSC_WOUT_OFFSET                 }, //
};

/* 28 regs, 28 types in Total*/

/* from 'L8-DE-MSC_RegFile_091215.csv' 20100311 00:54:15   대한민국 표준시 by getregs v2.3 */
