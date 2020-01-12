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
const DBG_REG_FIELD_T dbg_MIF0_CTRL0[] = {                       /* 0x0600 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "block_req_r_mvi0_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "block_req_r_mvi0_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "block_req_r_ipc0_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "block_req_r_ipc_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "block_req_r_ipc1_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "block_req_r_tnr1_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "block_req_r_capa_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "block_req_r_capa_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "block_req_r_tnr_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "block_req_r_tnr_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "block_req_r_3df_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "block_req_r_3df_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "disable_mif0"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "load_req_stv_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "load_req_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "load_cmd_saddr_v"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "load_cmd_pend_v"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "load_cmd_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "load_cmd_stride_v"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "enable_stv_cnt"                  },
};

/*-----------------------------------------------------------------------------
	0x0604 mif0_ctrl1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CTRL1[] = {                       /* 0x0604 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "set_10b_req_wd_capa_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "set_10b_req_wd_capa_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "set_10b_req_rd_capa_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "set_10b_req_rd_capa_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "set_10b_req_wd_tnr_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "set_10b_req_wd_tnr_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "set_10b_req_rd_tnr_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "set_10b_req_rd_tnr_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "set_10b_req_rd_ipc_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "set_10b_req_rd_ipc_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "set_10b_req_wd_scm_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "set_10b_req_wd_scm_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "scm_mrb_bank_mode"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,19, "scm_mwb_bank_mode"               },
};

/*-----------------------------------------------------------------------------
	0x0608 mif0_cmd_pri0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_PRI0[] = {                    /* 0x0608 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_v_mvi0_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_v_mvi0_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_v_ipc0_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_v_ipc_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_pri_v_ipc1_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_pri_v_capa_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_rd_pri_v_capa_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_rd_pri_v_tnr_y"              },
};

/*-----------------------------------------------------------------------------
	0x060c mif0_cmd_pri1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_PRI1[] = {                    /* 0x060c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_v_tnr_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_v_3df_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_v_3df_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_v_tnr1_y"             },
};

/*-----------------------------------------------------------------------------
	0x0610 mif0_req_pri0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_PRI0[] = {                    /* 0x0610 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_pri_v_mvi0_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_pri_v_mvi0_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_pri_v_ipc0_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_pri_v_ipc_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_pri_v_ipc1_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_pri_v_capa_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_rd_pri_v_capa_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_rd_pri_v_tnr_y"              },
};

/*-----------------------------------------------------------------------------
	0x0614 mif0_req_pri1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_PRI1[] = {                    /* 0x0614 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_pri_v_tnr_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_pri_v_3df_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_pri_v_3df_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_pri_v_tnr1_y"             },
};

/*-----------------------------------------------------------------------------
	0x0618 mif0_req_stv0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_STV0[] = {                    /* 0x0618 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_stv_v_mvi0_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_stv_v_mvi0_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_stv_v_ipc0_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_stv_v_ipc_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_stv_v_ipc1_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_stv_v_capa_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_rd_stv_v_capa_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_rd_stv_v_tnr_y"              },
};

/*-----------------------------------------------------------------------------
	0x061c mif0_req_stv1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_STV1[] = {                    /* 0x061c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_stv_v_tnr_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_stv_v_3df_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_stv_v_3df_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_stv_v_tnr1_y"             },
};

/*-----------------------------------------------------------------------------
	0x0620 mif0_cmd_pend0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_PEND0[] = {                   /* 0x0620 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pend_v_mvi0_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pend_v_mvi0_c"            },
};

/*-----------------------------------------------------------------------------
	0x0624 mif0_cmd_pend1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_PEND1[] = {                   /* 0x0624 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pend_v_tnr1_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pend_v_ipc0_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pend_v_ipc_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_pend_v_ipc1_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_pend_v_capa_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_rd_pend_v_capa_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_rd_pend_v_tnr_y"             },
};

/*-----------------------------------------------------------------------------
	0x0628 mif0_cmd_pend2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_PEND2[] = {                   /* 0x0628 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pend_v_tnr_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pend_v_3df_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pend_v_3df_c"             },
};

/*-----------------------------------------------------------------------------
	0x062c mif0_cmd_stride0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_STRIDE0[] = {                 /* 0x062c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_stride_v_mvi0_y"          },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_stride_v_mvi0_c"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_stride_v_8_tnr1_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_stride_v_2_tnr1_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_stride_v_8_ipc0_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_stride_v_2_ipc0_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_rd_stride_v_8_ipc_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_rd_stride_v_2_ipc_c"         },
};

/*-----------------------------------------------------------------------------
	0x0630 mif0_cmd_stride1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_STRIDE1[] = {                 /* 0x0630 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_stride_v_8_ipc1_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_stride_v_2_ipc1_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_stride_v_8_capa_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_stride_v_2_capa_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_stride_v_8_capa_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_stride_v_2_capa_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_rd_stride_v_8_tnr_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_rd_stride_v_2_tnr_y"         },
};

/*-----------------------------------------------------------------------------
	0x0634 mif0_cmd_stride2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_STRIDE2[] = {                 /* 0x0634 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_stride_v_8_tnr_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_stride_v_2_tnr_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_stride_v_8_3df_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_stride_v_2_3df_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_stride_v_8_3df_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_stride_v_2_3df_c"         },
};

/*-----------------------------------------------------------------------------
	0x0638 mif0_cmd_stride3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_STRIDE3[] = {                 /* 0x0638 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "man_cmd_rd_stride_v_mvi0_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "man_cmd_rd_stride_v_mvi0_c"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "man_cmd_rd_stride_v_ipc0_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "man_cmd_rd_stride_v_ipc_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "man_cmd_rd_stride_v_ipc1_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "man_cmd_rd_stride_v_tnr1_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "man_cmd_rd_stride_v_capa_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "man_cmd_rd_stride_v_capa_c"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "man_cmd_rd_stride_v_tnr_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "man_cmd_rd_stride_v_tnr_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "man_cmd_rd_stride_v_3df_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "man_cmd_rd_stride_v_3df_c"       },
};

/*-----------------------------------------------------------------------------
	0x063c saddr_rd_mvi0_yc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_MVI0_YC[] = {                 /* 0x063c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_mvi0_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_mvi0_c"             },
};

/*-----------------------------------------------------------------------------
	0x0640 saddr_rd_ipc0_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_IPC0_Y[] = {                  /* 0x0640 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_ipc0_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_ipc0_y"           },
};

/*-----------------------------------------------------------------------------
	0x0644 saddr_rd_ipc_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_IPC_C[] = {                   /* 0x0644 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_ipc_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_ipc_c"            },
};

/*-----------------------------------------------------------------------------
	0x0648 saddr_rd_ipc1_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_IPC1_Y[] = {                  /* 0x0648 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_ipc1_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_ipc1_y"           },
};

/*-----------------------------------------------------------------------------
	0x064c saddr_rd_tnr1_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_TNR1_Y[] = {                  /* 0x064c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_tnr1_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_tnr1_y"           },
};

/*-----------------------------------------------------------------------------
	0x0650 saddr_rd_capa_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_CAPA_Y[] = {                  /* 0x0650 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_capa_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_capa_y"           },
};

/*-----------------------------------------------------------------------------
	0x0654 saddr_rd_capa_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_CAPA_C[] = {                  /* 0x0654 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_capa_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_capa_c"           },
};

/*-----------------------------------------------------------------------------
	0x0658 saddr_rd_tnr_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_TNR_Y[] = {                   /* 0x0658 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_tnr_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_tnr_y"            },
};

/*-----------------------------------------------------------------------------
	0x065c saddr_rd_tnr_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_TNR_C[] = {                   /* 0x065c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_tnr_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_tnr_c"            },
};

/*-----------------------------------------------------------------------------
	0x0660 saddr_rd_3df_yc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_3DF_YC[] = {                  /* 0x0660 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_3df_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_3df_c"              },
};

/*-----------------------------------------------------------------------------
	0x0664 saddr_rd_dl_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_DL_Y[] = {                    /* 0x0664 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_dl_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_dl_y"             },
};

/*-----------------------------------------------------------------------------
	0x0668 saddr_rd_dl_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_DL_C[] = {                    /* 0x0668 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_dl_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_dl_c"             },
};

/*-----------------------------------------------------------------------------
	0x066c saddr_rd_dr_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_DR_Y[] = {                    /* 0x066c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_dr_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_dr_y"             },
};

/*-----------------------------------------------------------------------------
	0x0670 saddr_rd_dr_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_DR_C[] = {                    /* 0x0670 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_dr_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_dr_c"             },
};

/*-----------------------------------------------------------------------------
	0x0674 saddr_ext_0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_0[] = {                      /* 0x0674 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_saddr_mvi0_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_saddr_mvi0_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_saddr_8_ipc0_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_saddr_2_ipc0_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_saddr_8_ipc_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_saddr_2_ipc_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_rd_saddr_8_ipc1_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_rd_saddr_2_ipc1_y_msb4b"     },
};

/*-----------------------------------------------------------------------------
	0x0678 saddr_ext_1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_1[] = {                      /* 0x0678 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_saddr_8_capa_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_saddr_2_capa_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_saddr_8_capa_c_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_saddr_2_capa_c_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_saddr_8_tnr_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_saddr_2_tnr_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_rd_saddr_8_tnr_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_rd_saddr_2_tnr_c_msb4b"      },
};

/*-----------------------------------------------------------------------------
	0x067c saddr_ext_2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_2[] = {                      /* 0x067c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_saddr_3df_y_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_saddr_3df_c_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_saddr_8_tnr1_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_saddr_2_tnr1_y_msb4b"     },
};

/*-----------------------------------------------------------------------------
	0x0680 saddr_ext_3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_3[] = {                      /* 0x0680 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_saddr_8_dl_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_saddr_2_dl_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_saddr_8_dl_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_saddr_2_dl_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_saddr_8_dr_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_saddr_2_dr_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_rd_saddr_8_dr_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_rd_saddr_2_dr_c_msb4b"       },
};

/*-----------------------------------------------------------------------------
	0x0684 mif0_status ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_STATUS[] = {                      /* 0x0684 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "fr_rd_valid_mvi0_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "fr_rd_valid_mvi0_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "fr_rd_valid_ipc0_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "fr_rd_valid_ipc_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "fr_rd_valid_ipc1_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "fr_rd_valid_capa_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "fr_rd_valid_capa_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "fr_rd_valid_tnr_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "fr_rd_valid_tnr_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "fr_wd_q_full_capa_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "fr_wd_q_full_capa_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "fr_wd_q_full_tnr_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "fr_wd_q_full_tnr_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "fr_wd_q_full_scm_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "fr_wd_q_full_scm_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "fr_rd_valid_3df_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "fr_rd_valid_3df_c"               },
};

/*-----------------------------------------------------------------------------
	0x0688 saddr_param_update0_m ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_PARAM_UPDATE0_M[] = {            /* 0x0688 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "load"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,11, "register_update_address0_m"      },
};

/*-----------------------------------------------------------------------------
	0x068c saddr_param_update0_s0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_PARAM_UPDATE0_S0[] = {           /* 0x068c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "load"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,11, "register_update_address0_s0"     },
};

/*-----------------------------------------------------------------------------
	0x0690 saddr_param_update0_s1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_PARAM_UPDATE0_S1[] = {           /* 0x0690 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "load"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,11, "register_update_address0_s1"     },
};

/*-----------------------------------------------------------------------------
	0x0694 mif0_bank_addr_0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_BANK_ADDR_0[] = {                 /* 0x0694 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_bank_addr_8_mvi_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_bank_addr_8_mvi_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_bank_addr_8_ipc0_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_bank_addr_8_ipc_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_bank_addr_8_ipc1_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_bank_addr_8_tnr_1_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_rd_bank_addr_8_capa_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_rd_bank_addr_8_capa_c"       },
};

/*-----------------------------------------------------------------------------
	0x0698 mif0_bank_addr_1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_BANK_ADDR_1[] = {                 /* 0x0698 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_bank_addr_8_tnr0_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_bank_addr_8_tnr_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_bank_addr_8_3df_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_bank_addr_8_3df_c"        },
};

/*-----------------------------------------------------------------------------
	0x069c mif0_bank_addr_2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_BANK_ADDR_2[] = {                 /* 0x069c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_bank_addr_2_mvi_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_bank_addr_2_mvi_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_bank_addr_2_ipc0_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_bank_addr_2_ipc_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_bank_addr_2_ipc1_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_bank_addr_2_tnr_1_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_rd_bank_addr_2_capa_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_rd_bank_addr_2_capa_c"       },
};

/*-----------------------------------------------------------------------------
	0x06a0 mif0_bank_addr_3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_BANK_ADDR_3[] = {                 /* 0x06a0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_bank_addr_2_tnr0_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_bank_addr_2_tnr_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_bank_addr_2_3df_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_bank_addr_2_3df_c"        },
};

/*-----------------------------------------------------------------------------
	0x06a4 mif0_lsb_switch ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_LSB_SWITCH[] = {                  /* 0x06a4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "req_rd_lsb_ipc0_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "req_rd_lsb_ipc_c"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 4, "req_rd_lsb_ipc1_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 6, "req_rd_lsb_tnr1_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 8, "req_rd_lsb_capa_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,10, "req_rd_lsb_capa_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "req_rd_lsb_tnr0_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,14, "req_rd_lsb_tnr_c"                },
};

/*-----------------------------------------------------------------------------
	0x06a8 mif0_req_pend0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_PEND0[] = {                   /* 0x06a8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "req_rd_pend_v_mvi0_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "req_rd_pend_v_mvi0_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "req_rd_pend_v_ipc0_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "req_rd_pend_v_ipc_c"             },
};

/*-----------------------------------------------------------------------------
	0x06ac mif0_req_pend1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_PEND1[] = {                   /* 0x06ac */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "req_rd_pend_v_ipc1_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "req_rd_pend_v_tnr1_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "req_rd_pend_v_capa_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "req_rd_pend_v_capa_c"            },
};

/*-----------------------------------------------------------------------------
	0x06b0 mif0_req_pend2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_PEND2[] = {                   /* 0x06b0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "req_rd_pend_v_tnr0_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "req_rd_pend_v_tnr_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "req_rd_pend_v_3df_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "req_rd_pend_v_3df_c"             },
};

/*-----------------------------------------------------------------------------
	0x06b4 mif0_req_pend3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_PEND3[] = {                   /* 0x06b4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "req_wd_pend_v_tnr_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "req_wd_pend_v_tnr_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "req_wd_pend_v_scm_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "req_wd_pend_v_scm_c"             },
};

/*-----------------------------------------------------------------------------
	0x06b8 mif0_req_pend4 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_REQ_PEND4[] = {                   /* 0x06b8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "req_wd_pend_v_capa_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "req_wd_pend_v_capa_c"            },
};

/*-----------------------------------------------------------------------------
	0x06bc mif0_cmd_pri0_s ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_PRI0_S[] = {                  /* 0x06bc */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_s_v_mvi0_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_s_v_mvi0_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_s_v_ipc0_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_s_v_ipc_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_pri_s_v_ipc1_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_pri_s_v_capa_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_rd_pri_s_v_capa_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_rd_pri_s_v_tnr_y"            },
};

/*-----------------------------------------------------------------------------
	0x06c0 mif0_cmd_pri1_s ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_CMD_PRI1_S[] = {                  /* 0x06c0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_s_v_tnr_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_s_v_3df_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_s_v_3df_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_s_v_tnr1_y"           },
};

/*-----------------------------------------------------------------------------
	0x06c4 mif0_reset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF0_RESET[] = {                       /* 0x06c4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "reset_mif_r_mvi0_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "reset_mif_r_mvi0_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "reset_mif_r_ipc0_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "reset_mif_r_ipc_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "reset_mif_r_ipc1_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "reset_mif_r_tnr1_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "reset_mif_r_capa_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "reset_mif_r_capa_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "reset_mif_r_tnr_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "reset_mif_r_tnr_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "reset_mif_r_3df_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "reset_mif_r_3df_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "reset_mif_w_capa_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "reset_mif_w_capa_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "reset_mif_w_tnr_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "reset_mif_w_tnr_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "reset_mif_w_scm_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "reset_mif_w_scm_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,20, "reset_time_out_cnt"              },
};

/*-----------------------------------------------------------------------------
	0x06c8 saddr_param_update0_m1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_PARAM_UPDATE0_M1[] = {           /* 0x06c8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "load"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,11, "register_update_address0_m1"     },
};

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
const DBG_REG_FIELD_T dbg_MIF1_CTRL[] = {                        /* 0x0a00 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "block_req_w_scs_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "block_req_w_scs_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "block_req_w_capb_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "block_req_w_capb_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "block_req_w_tnr_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "block_req_w_tnr_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "block_req_w_scm_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "block_req_w_scm_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "block_req_w_capa_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "block_req_w_capa_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "block_req_w_mdi_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "block_req_w_mdi_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "disable_mif1"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "load_req_stv_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "load_req_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "load_cmd_saddr_v"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "load_cmd_pend_v"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "load_cmd_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "load_cmd_stride_v"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "enable_stv_cnt"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "set_ecanvas"                     },
};

/*-----------------------------------------------------------------------------
	0x0a04 mif1_cmd_pri0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_PRI0[] = {                    /* 0x0a04 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_pri_v_capa_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_pri_v_capa_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_pri_v_capb_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_pri_v_capb_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_wd_pri_v_tnr_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_wd_pri_v_tnr_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_wd_pri_v_scm_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_wd_pri_v_scm_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a08 mif1_cmd_pri1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_PRI1[] = {                    /* 0x0a08 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_pri_v_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_pri_v_scs_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_pri_v_mdi_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_pri_v_mdi_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a0c mif1_req_pri0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_REQ_PRI0[] = {                    /* 0x0a0c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_pri_v_capa_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_pri_v_capa_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_pri_v_capb_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_pri_v_capb_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_wd_pri_v_tnr_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_wd_pri_v_tnr_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_wd_pri_v_scm_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_wd_pri_v_scm_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a10 mif1_req_pri1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_REQ_PRI1[] = {                    /* 0x0a10 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_pri_v_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_pri_v_scs_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_pri_v_mdi_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_pri_v_mdi_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a14 mif1_req_stv0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_REQ_STV0[] = {                    /* 0x0a14 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_stv_v_capa_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_stv_v_capa_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_stv_v_capb_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_stv_v_capb_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_wd_stv_v_tnr_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_wd_stv_v_tnr_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_wd_stv_v_scm_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_wd_stv_v_scm_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a18 mif1_req_stv1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_REQ_STV1[] = {                    /* 0x0a18 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_stv_v_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_stv_v_scs_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_stv_v_mdi_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_stv_v_mdi_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a1c mif1_cmd_pend0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_PEND0[] = {                   /* 0x0a1c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_pend_v_capa_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_pend_v_capa_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_pend_v_capb_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_pend_v_capb_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_wd_pend_v_tnr_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_wd_pend_v_tnr_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_wd_pend_v_scm_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_wd_pend_v_scm_c"             },
};

/*-----------------------------------------------------------------------------
	0x0a20 mif1_cmd_pend1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_PEND1[] = {                   /* 0x0a20 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_pend_v_scs_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_pend_v_scs_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_pend_v_mdi_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_pend_v_mdi_c"             },
};

/*-----------------------------------------------------------------------------
	0x0a24 mif1_cmd_stride0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_STRIDE0[] = {                 /* 0x0a24 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_stride_v_8_capa_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_stride_v_2_capa_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_stride_v_8_capa_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_stride_v_2_capa_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_wd_stride_v_capb_y"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_wd_stride_v_capb_c"          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_wd_stride_v_8_tnr_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_wd_stride_v_2_tnr_y"         },
};

/*-----------------------------------------------------------------------------
	0x0a28 mif1_cmd_stride1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_STRIDE1[] = {                 /* 0x0a28 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_stride_v_8_tnr_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_stride_v_2_tnr_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_stride_v_8_scm_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_stride_v_2_scm_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_wd_stride_v_8_scm_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_wd_stride_v_2_scm_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_wd_stride_v_scs_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_wd_stride_v_scs_c"           },
};

/*-----------------------------------------------------------------------------
	0x0a2c mif1_cmd_stride2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_STRIDE2[] = {                 /* 0x0a2c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_stride_v_mdi_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_stride_v_mdi_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "man_cmd_wd_stride_v_scs_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "man_cmd_wd_stride_v_scs_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "man_cmd_wd_stride_v_capb_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "man_cmd_wd_stride_v_capb_c"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "man_cmd_wd_stride_v_tnr_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "man_cmd_wd_stride_v_tnr_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "man_cmd_wd_stride_v_scm_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "man_cmd_wd_stride_v_scm_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "man_cmd_wd_stride_v_capa_y"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "man_cmd_wd_stride_v_capa_c"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "man_cmd_wd_stride_v_mdi_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "man_cmd_wd_stride_v_mdi_c"       },
};

/*-----------------------------------------------------------------------------
	0x0a30 saddr_wd_capa_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_CAPA_Y[] = {                  /* 0x0a30 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_capa_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_capa_y"           },
};

/*-----------------------------------------------------------------------------
	0x0a34 saddr_wd_capa_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_CAPA_C[] = {                  /* 0x0a34 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_capa_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_capa_c"           },
};

/*-----------------------------------------------------------------------------
	0x0a38 saddr_wd_tnr_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_TNR_Y[] = {                   /* 0x0a38 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_tnr_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_tnr_y"            },
};

/*-----------------------------------------------------------------------------
	0x0a3c saddr_wd_tnr_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_TNR_C[] = {                   /* 0x0a3c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_tnr_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_tnr_c"            },
};

/*-----------------------------------------------------------------------------
	0x0a40 saddr_wd_scm_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_SCM_Y[] = {                   /* 0x0a40 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_scm_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_scm_y"            },
};

/*-----------------------------------------------------------------------------
	0x0a44 saddr_wd_scm_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_SCM_C[] = {                   /* 0x0a44 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_scm_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_scm_c"            },
};

/*-----------------------------------------------------------------------------
	0x0a48 saddr_wd_capb_yc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_CAPB_YC[] = {                 /* 0x0a48 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_capb_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_capb_c"             },
};

/*-----------------------------------------------------------------------------
	0x0a4c saddr_wd_scs_yc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_SCS_YC[] = {                  /* 0x0a4c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_scs_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a50 saddr_wd_mdi_yc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_MDI_YC[] = {                  /* 0x0a50 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_mdi_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_mdi_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a54 saddr_wd_dl_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_DL_Y[] = {                    /* 0x0a54 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_dl_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_dl_y"             },
};

/*-----------------------------------------------------------------------------
	0x0a58 saddr_wd_dl_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_DL_C[] = {                    /* 0x0a58 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_dl_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_dl_c"             },
};

/*-----------------------------------------------------------------------------
	0x0a5c saddr_wd_dr_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_DR_Y[] = {                    /* 0x0a5c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_dr_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_dr_y"             },
};

/*-----------------------------------------------------------------------------
	0x0a60 saddr_wd_dr_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_WD_DR_C[] = {                    /* 0x0a60 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_wd_saddr_2_dr_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_wd_saddr_8_dr_c"             },
};

/*-----------------------------------------------------------------------------
	0x0a64 saddr_ext_4 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_4[] = {                      /* 0x0a64 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_saddr_8_dl_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_saddr_2_dl_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_saddr_8_dl_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_saddr_2_dl_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_wd_saddr_8_dr_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_wd_saddr_2_dr_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_wd_saddr_8_dr_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_wd_saddr_2_dr_c_msb4b"       },
};

/*-----------------------------------------------------------------------------
	0x0a68 saddr_ext_5 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_5[] = {                      /* 0x0a68 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_saddr_8_scm_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_saddr_2_scm_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_saddr_8_scm_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_saddr_2_scm_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_wd_saddr_8_tnr_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_wd_saddr_2_tnr_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_wd_saddr_8_tnr_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_wd_saddr_2_tnr_c_msb4b"      },
};

/*-----------------------------------------------------------------------------
	0x0a6c saddr_ext_6 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_6[] = {                      /* 0x0a6c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_saddr_8_capa_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_saddr_2_capa_y_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_saddr_8_capa_c_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_saddr_2_capa_c_msb4b"     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_wd_saddr_capb_y_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_wd_saddr_capb_c_msb4b"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_wd_saddr_scs_y_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_wd_saddr_scs_c_msb4b"        },
};

/*-----------------------------------------------------------------------------
	0x0a70 saddr_ext_7 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_7[] = {                      /* 0x0a70 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_saddr_mdi_y_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_saddr_mdi_c_msb4b"        },
};

/*-----------------------------------------------------------------------------
	0x0a74 mif1_status ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_STATUS[] = {                      /* 0x0a74 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "fr_wd_q_full_capb_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "fr_wd_q_full_capb_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "fr_wd_q_full_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "fr_wd_q_full_scs_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "fr_wd_q_full_mdi_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "fr_wd_q_full_mdi_c"              },
};

/*-----------------------------------------------------------------------------
	0x0a78 mif1_bank_addr_0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_BANK_ADDR_0[] = {                 /* 0x0a78 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_bank_addr_8_scs_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_bank_addr_8_scs_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_bank_addr_8_capb_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_bank_addr_8_capb_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_wd_bank_addr_8_tnr_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_wd_bank_addr_8_tnr_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_wd_bank_addr_8_scm_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_wd_bank_addr_8_scm_c"        },
};

/*-----------------------------------------------------------------------------
	0x0a7c mif1_bank_addr_1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_BANK_ADDR_1[] = {                 /* 0x0a7c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_bank_addr_8_capa_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_bank_addr_8_capa_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_wd_bank_addr_8_mdi_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_wd_bank_addr_8_mdi_c"        },
};

/*-----------------------------------------------------------------------------
	0x0a80 mif1_bank_addr_2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_BANK_ADDR_2[] = {                 /* 0x0a80 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_wd_bank_addr_2_tnr_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_wd_bank_addr_2_tnr_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "req_wd_bank_addr_2_scm_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "req_wd_bank_addr_2_scm_c"        },
};

/*-----------------------------------------------------------------------------
	0x0a84 mif1_bank_addr_3 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_BANK_ADDR_3[] = {                 /* 0x0a84 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_wd_bank_addr_2_capa_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_wd_bank_addr_2_capa_c"       },
};

/*-----------------------------------------------------------------------------
	0x0a88 mif1_req_pend0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_REQ_PEND0[] = {                   /* 0x0a88 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "req_wd_pend_v_scs_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "req_wd_pend_v_scs_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "req_wd_pend_v_capb_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "req_wd_pend_v_capb_c"            },
};

/*-----------------------------------------------------------------------------
	0x0a8c mif1_req_pend1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_REQ_PEND1[] = {                   /* 0x0a8c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "req_wd_pend_v_mdi_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "req_wd_pend_v_mdi_c"             },
};

/*-----------------------------------------------------------------------------
	0x0a90 mif1_cmd_pri0_s ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_PRI0_S[] = {                  /* 0x0a90 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_pri_s_v_capa_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_pri_s_v_capa_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_pri_s_v_capb_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_pri_s_v_capb_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_wd_pri_s_v_tnr_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_wd_pri_s_v_tnr_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "cmd_wd_pri_s_v_scm_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "cmd_wd_pri_s_v_scm_c"            },
};

/*-----------------------------------------------------------------------------
	0x0a94 mif1_cmd_pri1_s ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_CMD_PRI1_S[] = {                  /* 0x0a94 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_wd_pri_s_v_scs_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_wd_pri_s_v_scs_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_wd_pri_s_v_mdi_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_wd_pri_s_v_mdi_c"            },
};

/*-----------------------------------------------------------------------------
	0x0a98 mif1_reset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF1_RESET[] = {                       /* 0x0a98 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "reset_mif_w_scs_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "reset_mif_w_scs_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "reset_mif_w_capb_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "reset_mif_w_capb_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "reset_mif_w_mdi_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "reset_mif_w_mdi_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,20, "reset_time_out_cnt"              },
};

/*-----------------------------------------------------------------------------
	0x1500 mif2_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_CTRL[] = {                        /* 0x1500 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "block_req_r_scm_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "block_req_r_scm_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "block_req_r_scs_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "block_req_r_scs_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "set_10b_req_rd_scm_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "set_10b_req_rd_scm_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "disable_mif2"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "load_req_stv_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "load_req_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "load_cmd_saddr_v"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "load_cmd_pend_v"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "load_cmd_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "load_cmd_stride_v"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "enable_stv_cnt"                  },
};

/*-----------------------------------------------------------------------------
	0x1504 mif2_cmd_pri ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_CMD_PRI[] = {                     /* 0x1504 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_v_scm_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_v_scm_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_v_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_v_scs_c"              },
};

/*-----------------------------------------------------------------------------
	0x1508 mif2_req_pri ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_REQ_PRI[] = {                     /* 0x1508 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_pri_v_scm_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_pri_v_scm_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_pri_v_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_pri_v_scs_c"              },
};

/*-----------------------------------------------------------------------------
	0x150c mif2_req_stv ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_REQ_STV[] = {                     /* 0x150c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_stv_v_scm_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_stv_v_scm_c"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_stv_v_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_stv_v_scs_c"              },
};

/*-----------------------------------------------------------------------------
	0x1510 mif2_cmd_pend ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_CMD_PEND[] = {                    /* 0x1510 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pend_v_scm_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pend_v_scm_c"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pend_v_scs_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pend_v_scs_c"             },
};

/*-----------------------------------------------------------------------------
	0x1514 mif2_cmd_stride ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_CMD_STRIDE[] = {                  /* 0x1514 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_stride_v_8_scm_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_stride_v_2_scm_y"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_stride_v_8_scm_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_stride_v_2_scm_c"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_stride_v_scs_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_stride_v_scs_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "man_cmd_rd_stride_v_scm_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "man_cmd_rd_stride_v_scm_c"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "man_cmd_rd_stride_v_scs_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "man_cmd_rd_stride_v_scs_c"       },
};

/*-----------------------------------------------------------------------------
	0x1518 saddr_rd_scm_y ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_SCM_Y[] = {                   /* 0x1518 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_scm_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_scm_y"            },
};

/*-----------------------------------------------------------------------------
	0x151c saddr_rd_scm_c ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_SCM_C[] = {                   /* 0x151c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_2_scm_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_8_scm_c"            },
};

/*-----------------------------------------------------------------------------
	0x1520 saddr_rd_scs_yc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_SCS_YC[] = {                  /* 0x1520 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_scs_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_scs_c"              },
};

/*-----------------------------------------------------------------------------
	0x1524 saddr_ext_8 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_8[] = {                      /* 0x1524 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_saddr_8_scm_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_saddr_2_scm_y_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_saddr_8_scm_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_saddr_2_scm_c_msb4b"      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_saddr_scs_y_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_saddr_scs_c_msb4b"        },
};

/*-----------------------------------------------------------------------------
	0x1528 mif2_status ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_STATUS[] = {                      /* 0x1528 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "fr_rd_valid_scm_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "fr_rd_valid_scm_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "fr_rd_valid_scs_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "fr_rd_valid_scs_c"               },
};

/*-----------------------------------------------------------------------------
	0x152c saddr_param_update1 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_PARAM_UPDATE1[] = {              /* 0x152c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "load"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,11, "register_update_address1"        },
};

/*-----------------------------------------------------------------------------
	0x1530 mif2_bank_addr_0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_BANK_ADDR_0[] = {                 /* 0x1530 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_bank_addr_8_scm_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_bank_addr_8_scm_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_bank_addr_8_scs_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_bank_addr_8_scs_c"        },
};

/*-----------------------------------------------------------------------------
	0x1534 mif2_bank_addr_1 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x1538 mif2_bank_addr_2 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_BANK_ADDR_2[] = {                 /* 0x1538 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_bank_addr_2_scm_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_bank_addr_2_scm_c"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_bank_addr_2_scs_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_bank_addr_2_scs_c"        },
};

/*-----------------------------------------------------------------------------
	0x153c mif2_bank_addr_3 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x1540 mif2_lsb_switch ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_LSB_SWITCH[] = {                  /* 0x1540 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "req_rd_lsb_scm_y"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "req_rd_lsb_scm_c"                },
};

/*-----------------------------------------------------------------------------
	0x1544 mif2_cmd_pri_s ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_CMD_PRI_S[] = {                   /* 0x1544 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_s_v_scm_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_s_v_scm_c"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_s_v_scs_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_s_v_scs_c"            },
};

/*-----------------------------------------------------------------------------
	0x1548 mif2_reset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF2_RESET[] = {                       /* 0x1548 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "reset_mif_r_scm_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "reset_mif_r_scm_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "reset_mif_r_scs_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "reset_mif_r_scs_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,20, "reset_time_out_cnt"              },
};

/*-----------------------------------------------------------------------------
	0x1b00 mif3_ctrl ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_CTRL[] = {                        /* 0x1b00 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "block_req_r_osd_0"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "block_req_r_osd_1"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "block_req_r_osd_2"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "block_req_r_osd_3"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "block_req_r_cve_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "block_req_r_cve_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "disable_mif3"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "load_req_stv_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "load_req_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "load_cmd_saddr_v"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "load_cmd_pend_v"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "load_cmd_pri_v"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "load_cmd_stride_v"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "enable_stv_cnt"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,24, "mif3_mrb_bank_mode"              },
};

/*-----------------------------------------------------------------------------
	0x1b04 mif3_cmd_pri ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_CMD_PRI[] = {                     /* 0x1b04 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_v_osd_0"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_v_osd_1"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_v_osd_2"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_v_osd_3"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_pri_v_cve_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_pri_v_cve_c"              },
};

/*-----------------------------------------------------------------------------
	0x1b08 mif3_req_pri ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_REQ_PRI[] = {                     /* 0x1b08 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_pri_v_osd_0"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_pri_v_osd_1"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_pri_v_osd_2"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_pri_v_osd_3"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_pri_v_cve_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_pri_v_cve_c"              },
};

/*-----------------------------------------------------------------------------
	0x1b0c mif3_req_stv ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_REQ_STV[] = {                     /* 0x1b0c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_stv_v_osd_0"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_stv_v_osd_1"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_stv_v_osd_2"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_stv_v_osd_3"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_stv_v_cve_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_stv_v_cve_c"              },
};

/*-----------------------------------------------------------------------------
	0x1b10 mif3_cmd_pend ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_CMD_PEND[] = {                    /* 0x1b10 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pend_v_osd_0"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pend_v_osd_1"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pend_v_osd_2"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pend_v_osd_3"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_pend_v_cve_y"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_pend_v_cve_c"             },
};

/*-----------------------------------------------------------------------------
	0x1b14 mif3_cmd_stride ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_CMD_STRIDE[] = {                  /* 0x1b14 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_stride_v_osd_0"           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_stride_v_osd_1"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_stride_v_osd_2"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_stride_v_osd_3"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_stride_v_cve_y"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_stride_v_cve_c"           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "man_cmd_rd_stride_v_osd_0"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "man_cmd_rd_stride_v_osd_1"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "man_cmd_rd_stride_v_osd_2"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "man_cmd_rd_stride_v_osd_3"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "man_cmd_rd_stride_v_cve_y"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "man_cmd_rd_stride_v_cve_c"       },
};

/*-----------------------------------------------------------------------------
	0x1b18 saddr_rd_osd_01 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_OSD_01[] = {                  /* 0x1b18 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_osd_1"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_osd_0"              },
};

/*-----------------------------------------------------------------------------
	0x1b1c saddr_rd_osd_23 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_OSD_23[] = {                  /* 0x1b1c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_osd_3"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_osd_2"              },
};

/*-----------------------------------------------------------------------------
	0x1b20 saddr_rd_cve_yc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_RD_CVE_YC[] = {                  /* 0x1b20 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "req_rd_saddr_cve_y"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "req_rd_saddr_cve_c"              },
};

/*-----------------------------------------------------------------------------
	0x1b24 saddr_ext_9 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SADDR_EXT_9[] = {                      /* 0x1b24 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "req_rd_saddr_osd_0_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "req_rd_saddr_osd_1_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "req_rd_saddr_osd_2_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "req_rd_saddr_osd_3_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_saddr_cve_y_msb4b"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_saddr_cve_c_msb4b"        },
};

/*-----------------------------------------------------------------------------
	0x1b28 mif3_status ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_STATUS[] = {                      /* 0x1b28 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "fr_rd_valid_osd_0"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "fr_rd_valid_osd_1"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "fr_rd_valid_osd_2"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "fr_rd_valid_osd_3"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "fr_rd_valid_cve_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "fr_rd_valid_cve_c"               },
};

/*-----------------------------------------------------------------------------
	0x1b2c mif3_bank_addr_0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_BANK_ADDR_0[] = {                 /* 0x1b2c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "req_rd_bank_addr_8_cve_y"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "req_rd_bank_addr_8_cve_c"        },
};

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
const DBG_REG_FIELD_T dbg_MIF3_CMD_PRI_S[] = {                   /* 0x1b3c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "cmd_rd_pri_s_v_osd_0"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "cmd_rd_pri_s_v_osd_1"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "cmd_rd_pri_s_v_osd_2"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "cmd_rd_pri_s_v_osd_3"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,16, "cmd_rd_pri_s_v_cve_y"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "cmd_rd_pri_s_v_cve_c"            },
};

/*-----------------------------------------------------------------------------
	0x1b40 mif3_reset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_MIF3_RESET[] = {                       /* 0x1b40 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "reset_mif_r_osd_0"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "reset_mif_r_osd_1"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "reset_mif_r_osd_2"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "reset_mif_r_osd_3"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "reset_mif_r_cve_y"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "reset_mif_r_cve_c"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,20, "reset_time_out_cnt"              },
};

DBG_REG_T gDbgRegMIF[] = {
	{0x0600,N_FLD(dbg_MIF0_CTRL0),                      "MIF0_CTRL0"                      ,dbg_MIF0_CTRL0                      }, //
	{0x0604,N_FLD(dbg_MIF0_CTRL1),                      "MIF0_CTRL1"                      ,dbg_MIF0_CTRL1                      }, //
	{0x0608,N_FLD(dbg_MIF0_CMD_PRI0),                   "MIF0_CMD_PRI0"                   ,dbg_MIF0_CMD_PRI0                   }, //
	{0x060c,N_FLD(dbg_MIF0_CMD_PRI1),                   "MIF0_CMD_PRI1"                   ,dbg_MIF0_CMD_PRI1                   }, //
	{0x0610,N_FLD(dbg_MIF0_REQ_PRI0),                   "MIF0_REQ_PRI0"                   ,dbg_MIF0_REQ_PRI0                   }, //
	{0x0614,N_FLD(dbg_MIF0_REQ_PRI1),                   "MIF0_REQ_PRI1"                   ,dbg_MIF0_REQ_PRI1                   }, //
	{0x0618,N_FLD(dbg_MIF0_REQ_STV0),                   "MIF0_REQ_STV0"                   ,dbg_MIF0_REQ_STV0                   }, //
	{0x061c,N_FLD(dbg_MIF0_REQ_STV1),                   "MIF0_REQ_STV1"                   ,dbg_MIF0_REQ_STV1                   }, //
	{0x0620,N_FLD(dbg_MIF0_CMD_PEND0),                  "MIF0_CMD_PEND0"                  ,dbg_MIF0_CMD_PEND0                  }, //
	{0x0624,N_FLD(dbg_MIF0_CMD_PEND1),                  "MIF0_CMD_PEND1"                  ,dbg_MIF0_CMD_PEND1                  }, //
	{0x0628,N_FLD(dbg_MIF0_CMD_PEND2),                  "MIF0_CMD_PEND2"                  ,dbg_MIF0_CMD_PEND2                  }, //
	{0x062c,N_FLD(dbg_MIF0_CMD_STRIDE0),                "MIF0_CMD_STRIDE0"                ,dbg_MIF0_CMD_STRIDE0                }, //
	{0x0630,N_FLD(dbg_MIF0_CMD_STRIDE1),                "MIF0_CMD_STRIDE1"                ,dbg_MIF0_CMD_STRIDE1                }, //
	{0x0634,N_FLD(dbg_MIF0_CMD_STRIDE2),                "MIF0_CMD_STRIDE2"                ,dbg_MIF0_CMD_STRIDE2                }, //
	{0x0638,N_FLD(dbg_MIF0_CMD_STRIDE3),                "MIF0_CMD_STRIDE3"                ,dbg_MIF0_CMD_STRIDE3                }, //
	{0x063c,N_FLD(dbg_SADDR_RD_MVI0_YC),                "SADDR_RD_MVI0_YC"                ,dbg_SADDR_RD_MVI0_YC                }, //
	{0x0640,N_FLD(dbg_SADDR_RD_IPC0_Y),                 "SADDR_RD_IPC0_Y"                 ,dbg_SADDR_RD_IPC0_Y                 }, //
	{0x0644,N_FLD(dbg_SADDR_RD_IPC_C),                  "SADDR_RD_IPC_C"                  ,dbg_SADDR_RD_IPC_C                  }, //
	{0x0648,N_FLD(dbg_SADDR_RD_IPC1_Y),                 "SADDR_RD_IPC1_Y"                 ,dbg_SADDR_RD_IPC1_Y                 }, //
	{0x064c,N_FLD(dbg_SADDR_RD_TNR1_Y),                 "SADDR_RD_TNR1_Y"                 ,dbg_SADDR_RD_TNR1_Y                 }, //
	{0x0650,N_FLD(dbg_SADDR_RD_CAPA_Y),                 "SADDR_RD_CAPA_Y"                 ,dbg_SADDR_RD_CAPA_Y                 }, //
	{0x0654,N_FLD(dbg_SADDR_RD_CAPA_C),                 "SADDR_RD_CAPA_C"                 ,dbg_SADDR_RD_CAPA_C                 }, //
	{0x0658,N_FLD(dbg_SADDR_RD_TNR_Y),                  "SADDR_RD_TNR_Y"                  ,dbg_SADDR_RD_TNR_Y                  }, //
	{0x065c,N_FLD(dbg_SADDR_RD_TNR_C),                  "SADDR_RD_TNR_C"                  ,dbg_SADDR_RD_TNR_C                  }, //
	{0x0660,N_FLD(dbg_SADDR_RD_3DF_YC),                 "SADDR_RD_3DF_YC"                 ,dbg_SADDR_RD_3DF_YC                 }, //
	{0x0664,N_FLD(dbg_SADDR_RD_DL_Y),                   "SADDR_RD_DL_Y"                   ,dbg_SADDR_RD_DL_Y                   }, //
	{0x0668,N_FLD(dbg_SADDR_RD_DL_C),                   "SADDR_RD_DL_C"                   ,dbg_SADDR_RD_DL_C                   }, //
	{0x066c,N_FLD(dbg_SADDR_RD_DR_Y),                   "SADDR_RD_DR_Y"                   ,dbg_SADDR_RD_DR_Y                   }, //
	{0x0670,N_FLD(dbg_SADDR_RD_DR_C),                   "SADDR_RD_DR_C"                   ,dbg_SADDR_RD_DR_C                   }, //
	{0x0674,N_FLD(dbg_SADDR_EXT_0),                     "SADDR_EXT_0"                     ,dbg_SADDR_EXT_0                     }, //
	{0x0678,N_FLD(dbg_SADDR_EXT_1),                     "SADDR_EXT_1"                     ,dbg_SADDR_EXT_1                     }, //
	{0x067c,N_FLD(dbg_SADDR_EXT_2),                     "SADDR_EXT_2"                     ,dbg_SADDR_EXT_2                     }, //
	{0x0680,N_FLD(dbg_SADDR_EXT_3),                     "SADDR_EXT_3"                     ,dbg_SADDR_EXT_3                     }, //
	{0x0684,N_FLD(dbg_MIF0_STATUS),                     "MIF0_STATUS"                     ,dbg_MIF0_STATUS                     }, //
	{0x0688,N_FLD(dbg_SADDR_PARAM_UPDATE0_M),           "SADDR_PARAM_UPDATE0_M"           ,dbg_SADDR_PARAM_UPDATE0_M           }, //
	{0x068c,N_FLD(dbg_SADDR_PARAM_UPDATE0_S0),          "SADDR_PARAM_UPDATE0_S0"          ,dbg_SADDR_PARAM_UPDATE0_S0          }, //
	{0x0690,N_FLD(dbg_SADDR_PARAM_UPDATE0_S1),          "SADDR_PARAM_UPDATE0_S1"          ,dbg_SADDR_PARAM_UPDATE0_S1          }, //
	{0x0694,N_FLD(dbg_MIF0_BANK_ADDR_0),                "MIF0_BANK_ADDR_0"                ,dbg_MIF0_BANK_ADDR_0                }, //
	{0x0698,N_FLD(dbg_MIF0_BANK_ADDR_1),                "MIF0_BANK_ADDR_1"                ,dbg_MIF0_BANK_ADDR_1                }, //
	{0x069c,N_FLD(dbg_MIF0_BANK_ADDR_2),                "MIF0_BANK_ADDR_2"                ,dbg_MIF0_BANK_ADDR_2                }, //
	{0x06a0,N_FLD(dbg_MIF0_BANK_ADDR_3),                "MIF0_BANK_ADDR_3"                ,dbg_MIF0_BANK_ADDR_3                }, //
	{0x06a4,N_FLD(dbg_MIF0_LSB_SWITCH),                 "MIF0_LSB_SWITCH"                 ,dbg_MIF0_LSB_SWITCH                 }, //
	{0x06a8,N_FLD(dbg_MIF0_REQ_PEND0),                  "MIF0_REQ_PEND0"                  ,dbg_MIF0_REQ_PEND0                  }, //
	{0x06ac,N_FLD(dbg_MIF0_REQ_PEND1),                  "MIF0_REQ_PEND1"                  ,dbg_MIF0_REQ_PEND1                  }, //
	{0x06b0,N_FLD(dbg_MIF0_REQ_PEND2),                  "MIF0_REQ_PEND2"                  ,dbg_MIF0_REQ_PEND2                  }, //
	{0x06b4,N_FLD(dbg_MIF0_REQ_PEND3),                  "MIF0_REQ_PEND3"                  ,dbg_MIF0_REQ_PEND3                  }, //
	{0x06b8,N_FLD(dbg_MIF0_REQ_PEND4),                  "MIF0_REQ_PEND4"                  ,dbg_MIF0_REQ_PEND4                  }, //
	{0x06bc,N_FLD(dbg_MIF0_CMD_PRI0_S),                 "MIF0_CMD_PRI0_S"                 ,dbg_MIF0_CMD_PRI0_S                 }, //
	{0x06c0,N_FLD(dbg_MIF0_CMD_PRI1_S),                 "MIF0_CMD_PRI1_S"                 ,dbg_MIF0_CMD_PRI1_S                 }, //
	{0x06c4,N_FLD(dbg_MIF0_RESET),                      "MIF0_RESET"                      ,dbg_MIF0_RESET                      }, //
	{0x06c8,N_FLD(dbg_SADDR_PARAM_UPDATE0_M1),          "SADDR_PARAM_UPDATE0_M1"          ,dbg_SADDR_PARAM_UPDATE0_M1          }, //
	{0x06cc,0,                                  "DE_ZEBU_DBG0"                    ,NULL                                }, //
	{0x06d0,0,                                  "DE_ZEBU_DBG1"                    ,NULL                                }, //
	{0x06d4,0,                                  "DE_ZEBU_DBG2"                    ,NULL                                }, //
	{0x06d8,0,                                  "DE_ZEBU_DBG3"                    ,NULL                                }, //
	{0x0a00,N_FLD(dbg_MIF1_CTRL),                       "MIF1_CTRL"                       ,dbg_MIF1_CTRL                       }, //
	{0x0a04,N_FLD(dbg_MIF1_CMD_PRI0),                   "MIF1_CMD_PRI0"                   ,dbg_MIF1_CMD_PRI0                   }, //
	{0x0a08,N_FLD(dbg_MIF1_CMD_PRI1),                   "MIF1_CMD_PRI1"                   ,dbg_MIF1_CMD_PRI1                   }, //
	{0x0a0c,N_FLD(dbg_MIF1_REQ_PRI0),                   "MIF1_REQ_PRI0"                   ,dbg_MIF1_REQ_PRI0                   }, //
	{0x0a10,N_FLD(dbg_MIF1_REQ_PRI1),                   "MIF1_REQ_PRI1"                   ,dbg_MIF1_REQ_PRI1                   }, //
	{0x0a14,N_FLD(dbg_MIF1_REQ_STV0),                   "MIF1_REQ_STV0"                   ,dbg_MIF1_REQ_STV0                   }, //
	{0x0a18,N_FLD(dbg_MIF1_REQ_STV1),                   "MIF1_REQ_STV1"                   ,dbg_MIF1_REQ_STV1                   }, //
	{0x0a1c,N_FLD(dbg_MIF1_CMD_PEND0),                  "MIF1_CMD_PEND0"                  ,dbg_MIF1_CMD_PEND0                  }, //
	{0x0a20,N_FLD(dbg_MIF1_CMD_PEND1),                  "MIF1_CMD_PEND1"                  ,dbg_MIF1_CMD_PEND1                  }, //
	{0x0a24,N_FLD(dbg_MIF1_CMD_STRIDE0),                "MIF1_CMD_STRIDE0"                ,dbg_MIF1_CMD_STRIDE0                }, //
	{0x0a28,N_FLD(dbg_MIF1_CMD_STRIDE1),                "MIF1_CMD_STRIDE1"                ,dbg_MIF1_CMD_STRIDE1                }, //
	{0x0a2c,N_FLD(dbg_MIF1_CMD_STRIDE2),                "MIF1_CMD_STRIDE2"                ,dbg_MIF1_CMD_STRIDE2                }, //
	{0x0a30,N_FLD(dbg_SADDR_WD_CAPA_Y),                 "SADDR_WD_CAPA_Y"                 ,dbg_SADDR_WD_CAPA_Y                 }, //
	{0x0a34,N_FLD(dbg_SADDR_WD_CAPA_C),                 "SADDR_WD_CAPA_C"                 ,dbg_SADDR_WD_CAPA_C                 }, //
	{0x0a38,N_FLD(dbg_SADDR_WD_TNR_Y),                  "SADDR_WD_TNR_Y"                  ,dbg_SADDR_WD_TNR_Y                  }, //
	{0x0a3c,N_FLD(dbg_SADDR_WD_TNR_C),                  "SADDR_WD_TNR_C"                  ,dbg_SADDR_WD_TNR_C                  }, //
	{0x0a40,N_FLD(dbg_SADDR_WD_SCM_Y),                  "SADDR_WD_SCM_Y"                  ,dbg_SADDR_WD_SCM_Y                  }, //
	{0x0a44,N_FLD(dbg_SADDR_WD_SCM_C),                  "SADDR_WD_SCM_C"                  ,dbg_SADDR_WD_SCM_C                  }, //
	{0x0a48,N_FLD(dbg_SADDR_WD_CAPB_YC),                "SADDR_WD_CAPB_YC"                ,dbg_SADDR_WD_CAPB_YC                }, //
	{0x0a4c,N_FLD(dbg_SADDR_WD_SCS_YC),                 "SADDR_WD_SCS_YC"                 ,dbg_SADDR_WD_SCS_YC                 }, //
	{0x0a50,N_FLD(dbg_SADDR_WD_MDI_YC),                 "SADDR_WD_MDI_YC"                 ,dbg_SADDR_WD_MDI_YC                 }, //
	{0x0a54,N_FLD(dbg_SADDR_WD_DL_Y),                   "SADDR_WD_DL_Y"                   ,dbg_SADDR_WD_DL_Y                   }, //
	{0x0a58,N_FLD(dbg_SADDR_WD_DL_C),                   "SADDR_WD_DL_C"                   ,dbg_SADDR_WD_DL_C                   }, //
	{0x0a5c,N_FLD(dbg_SADDR_WD_DR_Y),                   "SADDR_WD_DR_Y"                   ,dbg_SADDR_WD_DR_Y                   }, //
	{0x0a60,N_FLD(dbg_SADDR_WD_DR_C),                   "SADDR_WD_DR_C"                   ,dbg_SADDR_WD_DR_C                   }, //
	{0x0a64,N_FLD(dbg_SADDR_EXT_4),                     "SADDR_EXT_4"                     ,dbg_SADDR_EXT_4                     }, //
	{0x0a68,N_FLD(dbg_SADDR_EXT_5),                     "SADDR_EXT_5"                     ,dbg_SADDR_EXT_5                     }, //
	{0x0a6c,N_FLD(dbg_SADDR_EXT_6),                     "SADDR_EXT_6"                     ,dbg_SADDR_EXT_6                     }, //
	{0x0a70,N_FLD(dbg_SADDR_EXT_7),                     "SADDR_EXT_7"                     ,dbg_SADDR_EXT_7                     }, //
	{0x0a74,N_FLD(dbg_MIF1_STATUS),                     "MIF1_STATUS"                     ,dbg_MIF1_STATUS                     }, //
	{0x0a78,N_FLD(dbg_MIF1_BANK_ADDR_0),                "MIF1_BANK_ADDR_0"                ,dbg_MIF1_BANK_ADDR_0                }, //
	{0x0a7c,N_FLD(dbg_MIF1_BANK_ADDR_1),                "MIF1_BANK_ADDR_1"                ,dbg_MIF1_BANK_ADDR_1                }, //
	{0x0a80,N_FLD(dbg_MIF1_BANK_ADDR_2),                "MIF1_BANK_ADDR_2"                ,dbg_MIF1_BANK_ADDR_2                }, //
	{0x0a84,N_FLD(dbg_MIF1_BANK_ADDR_3),                "MIF1_BANK_ADDR_3"                ,dbg_MIF1_BANK_ADDR_3                }, //
	{0x0a88,N_FLD(dbg_MIF1_REQ_PEND0),                  "MIF1_REQ_PEND0"                  ,dbg_MIF1_REQ_PEND0                  }, //
	{0x0a8c,N_FLD(dbg_MIF1_REQ_PEND1),                  "MIF1_REQ_PEND1"                  ,dbg_MIF1_REQ_PEND1                  }, //
	{0x0a90,N_FLD(dbg_MIF1_CMD_PRI0_S),                 "MIF1_CMD_PRI0_S"                 ,dbg_MIF1_CMD_PRI0_S                 }, //
	{0x0a94,N_FLD(dbg_MIF1_CMD_PRI1_S),                 "MIF1_CMD_PRI1_S"                 ,dbg_MIF1_CMD_PRI1_S                 }, //
	{0x0a98,N_FLD(dbg_MIF1_RESET),                      "MIF1_RESET"                      ,dbg_MIF1_RESET                      }, //
	{0x1500,N_FLD(dbg_MIF2_CTRL),                       "MIF2_CTRL"                       ,dbg_MIF2_CTRL                       }, //
	{0x1504,N_FLD(dbg_MIF2_CMD_PRI),                    "MIF2_CMD_PRI"                    ,dbg_MIF2_CMD_PRI                    }, //
	{0x1508,N_FLD(dbg_MIF2_REQ_PRI),                    "MIF2_REQ_PRI"                    ,dbg_MIF2_REQ_PRI                    }, //
	{0x150c,N_FLD(dbg_MIF2_REQ_STV),                    "MIF2_REQ_STV"                    ,dbg_MIF2_REQ_STV                    }, //
	{0x1510,N_FLD(dbg_MIF2_CMD_PEND),                   "MIF2_CMD_PEND"                   ,dbg_MIF2_CMD_PEND                   }, //
	{0x1514,N_FLD(dbg_MIF2_CMD_STRIDE),                 "MIF2_CMD_STRIDE"                 ,dbg_MIF2_CMD_STRIDE                 }, //
	{0x1518,N_FLD(dbg_SADDR_RD_SCM_Y),                  "SADDR_RD_SCM_Y"                  ,dbg_SADDR_RD_SCM_Y                  }, //
	{0x151c,N_FLD(dbg_SADDR_RD_SCM_C),                  "SADDR_RD_SCM_C"                  ,dbg_SADDR_RD_SCM_C                  }, //
	{0x1520,N_FLD(dbg_SADDR_RD_SCS_YC),                 "SADDR_RD_SCS_YC"                 ,dbg_SADDR_RD_SCS_YC                 }, //
	{0x1524,N_FLD(dbg_SADDR_EXT_8),                     "SADDR_EXT_8"                     ,dbg_SADDR_EXT_8                     }, //
	{0x1528,N_FLD(dbg_MIF2_STATUS),                     "MIF2_STATUS"                     ,dbg_MIF2_STATUS                     }, //
	{0x152c,N_FLD(dbg_SADDR_PARAM_UPDATE1),             "SADDR_PARAM_UPDATE1"             ,dbg_SADDR_PARAM_UPDATE1             }, //
	{0x1530,N_FLD(dbg_MIF2_BANK_ADDR_0),                "MIF2_BANK_ADDR_0"                ,dbg_MIF2_BANK_ADDR_0                }, //
	{0x1534,0,                                  "MIF2_BANK_ADDR_1"                ,NULL                                }, //
	{0x1538,N_FLD(dbg_MIF2_BANK_ADDR_2),                "MIF2_BANK_ADDR_2"                ,dbg_MIF2_BANK_ADDR_2                }, //
	{0x153c,0,                                  "MIF2_BANK_ADDR_3"                ,NULL                                }, //
	{0x1540,N_FLD(dbg_MIF2_LSB_SWITCH),                 "MIF2_LSB_SWITCH"                 ,dbg_MIF2_LSB_SWITCH                 }, //
	{0x1544,N_FLD(dbg_MIF2_CMD_PRI_S),                  "MIF2_CMD_PRI_S"                  ,dbg_MIF2_CMD_PRI_S                  }, //
	{0x1548,N_FLD(dbg_MIF2_RESET),                      "MIF2_RESET"                      ,dbg_MIF2_RESET                      }, //
	{0x1b00,N_FLD(dbg_MIF3_CTRL),                       "MIF3_CTRL"                       ,dbg_MIF3_CTRL                       }, //
	{0x1b04,N_FLD(dbg_MIF3_CMD_PRI),                    "MIF3_CMD_PRI"                    ,dbg_MIF3_CMD_PRI                    }, //
	{0x1b08,N_FLD(dbg_MIF3_REQ_PRI),                    "MIF3_REQ_PRI"                    ,dbg_MIF3_REQ_PRI                    }, //
	{0x1b0c,N_FLD(dbg_MIF3_REQ_STV),                    "MIF3_REQ_STV"                    ,dbg_MIF3_REQ_STV                    }, //
	{0x1b10,N_FLD(dbg_MIF3_CMD_PEND),                   "MIF3_CMD_PEND"                   ,dbg_MIF3_CMD_PEND                   }, //
	{0x1b14,N_FLD(dbg_MIF3_CMD_STRIDE),                 "MIF3_CMD_STRIDE"                 ,dbg_MIF3_CMD_STRIDE                 }, //
	{0x1b18,N_FLD(dbg_SADDR_RD_OSD_01),                 "SADDR_RD_OSD_01"                 ,dbg_SADDR_RD_OSD_01                 }, //
	{0x1b1c,N_FLD(dbg_SADDR_RD_OSD_23),                 "SADDR_RD_OSD_23"                 ,dbg_SADDR_RD_OSD_23                 }, //
	{0x1b20,N_FLD(dbg_SADDR_RD_CVE_YC),                 "SADDR_RD_CVE_YC"                 ,dbg_SADDR_RD_CVE_YC                 }, //
	{0x1b24,N_FLD(dbg_SADDR_EXT_9),                     "SADDR_EXT_9"                     ,dbg_SADDR_EXT_9                     }, //
	{0x1b28,N_FLD(dbg_MIF3_STATUS),                     "MIF3_STATUS"                     ,dbg_MIF3_STATUS                     }, //
	{0x1b2c,N_FLD(dbg_MIF3_BANK_ADDR_0),                "MIF3_BANK_ADDR_0"                ,dbg_MIF3_BANK_ADDR_0                }, //
	{0x1b30,0,                                  "MIF3_BANK_ADDR_1"                ,NULL                                }, //
	{0x1b34,0,                                  "MIF3_BANK_ADDR_2"                ,NULL                                }, //
	{0x1b38,0,                                  "MIF3_BANK_ADDR_3"                ,NULL                                }, //
	{0x1b3c,N_FLD(dbg_MIF3_CMD_PRI_S),                  "MIF3_CMD_PRI_S"                  ,dbg_MIF3_CMD_PRI_S                  }, //
	{0x1b40,N_FLD(dbg_MIF3_RESET),                      "MIF3_RESET"                      ,dbg_MIF3_RESET                      }, //
};

/* 130 regs, 130 types in Total*/

/* from 'L8_DE_MIF_RegFile_100127b.csv' 20100316 14:34:52   대한민국 표준시 by getregs v2.3 */
