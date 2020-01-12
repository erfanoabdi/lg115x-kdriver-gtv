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
	0x0010 ctr00 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR00[] = {                            /* 0x0010 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "core_clk_div_sel"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "peri_apb1_clk_5_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "peri_apb1_clk_4_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "peri_apb1_clk_3_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "peri_apb1_clk_2_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "peri_apb1_clk_1_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "peri_apb1_clk_0_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "peri_apb0_clk_7_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "peri_apb0_clk_6_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "peri_apb0_clk_5_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "peri_apb0_clk_4_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "peri_apb0_clk_3_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "peri_apb0_clk_2_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "peri_apb0_clk_1_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "peri_apb0_clk_0_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "peri_ahb_clk_30_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "peri_ahb_clk_28_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "peri_ahb_clk_26_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "peri_ahb_clk_25_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "peri_ahb_clk_24_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "peri_ahb_clk_22_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "peri_ahb_clk_19_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "peri_ahb_clk_14_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "peri_ahb_clk_9_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "peri_ahb_clk_8_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "peri_ahb_clk_7_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "peri_ahb_clk_6_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "peri_ahb_clk_5_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "peri_ahb_clk_4_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "peri_ahb_clk_3_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "peri_ahb_clk_2_off"              },
};

/*-----------------------------------------------------------------------------
	0x0014 ctr01 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR01[] = {                            /* 0x0014 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "peri_ahb_clk_1_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "peri_ahb_clk_0_off"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "peri_apb1_clk_14_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "peri_apb1_clk_13_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "peri_apb1_clk_12_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "peri_apb1_clk_11_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "peri_apb1_clk_10_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "peri_apb1_clk_9_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "peri_apb1_clk_8_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "peri_apb1_clk_7_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "peri_apb1_clk_6_off"             },
};

/*-----------------------------------------------------------------------------
	0x0018 ctr02 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR02[] = {                            /* 0x0018 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "peri_ahb_clk_29_sel"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "psel_l2_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,16, "bus_clk_4_sel"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,18, "pclken_sel"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "bus_clk_1_sel"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,22, "peri_ahb_clk_27_sel"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "peri_ahb_clk_23_off"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,26, "peri_ahb_clk_21_sel"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "peri_ahb_clk_20_sel"             },
};

/*-----------------------------------------------------------------------------
	0x001c ctr03 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0020 ctr04 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0024 ctr05 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0028 ctr06 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR06[] = {                            /* 0x0028 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "peri_apb0_rstn_1"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "peri_apb0_rstn_0"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "bus_rstn_8"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "peri_ahb_rstn_27"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "peri_ahb_rstn_26"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "peri_ahb_rstn_25"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "peri_ahb_rstn_24"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "peri_ahb_rstn_23"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "peri_ahb_rstn_22"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "peri_ahb_rstn_21"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "peri_ahb_rstn_20"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "peri_ahb_rstn_19"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "peri_ahb_rstn_18"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "peri_ahb_rstn_17"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "peri_ahb_rstn_16"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "peri_ahb_rstn_15"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "peri_ahb_rstn_14"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "peri_ahb_rstn_13"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "peri_ahb_rstn_12"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "peri_ahb_rstn_11"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "peri_ahb_rstn_10"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "peri_ahb_rstn_9"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "peri_ahb_rstn_8"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "peri_ahb_rstn_7"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "peri_ahb_rstn_6"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "peri_ahb_rstn_5"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "peri_ahb_rstn_4"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "peri_ahb_rstn_3"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "peri_ahb_rstn_2"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "bus_rstn_7"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "bus_rstn_6"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "bus_rstn_5"                      },
};

/*-----------------------------------------------------------------------------
	0x002c ctr07 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR07[] = {                            /* 0x002c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "bus_rstn_4"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "bus_rstn_3"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "bus_rstn_2"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "bus_rstn_1"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "bus_rstn_0"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "peri_ahb_rstn_1"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "peri_ahb_rstn_0"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "peri_apb1_rstn_14"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "peri_apb1_rstn_13"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "peri_apb1_rstn_12"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "peri_apb1_rstn_11"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "peri_apb1_rstn_10"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "peri_apb1_rstn_9"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "peri_apb1_rstn_8"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "peri_apb1_rstn_7"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "peri_apb1_rstn_6"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "peri_apb1_rstn_5"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "peri_apb1_rstn_4"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "peri_apb1_rstn_3"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "peri_apb1_rstn_2"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "peri_apb1_rstn_1"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "peri_apb1_rstn_0"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "bus_rstn_9"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "peri_apb0_rstn_7"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "peri_apb0_rstn_6"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "peri_apb0_rstn_5"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "peri_apb0_rstn_4"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "peri_apb0_rstn_3"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "peri_apb0_rstn_2"                },
};

/*-----------------------------------------------------------------------------
	0x0030 ctr08 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR08[] = {                            /* 0x0030 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "bt_usb_por_ctr"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "usb_por_ctr"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "peri_ahb_rstn_37"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "peri_ahb_rstn_36"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "peri_ahb_rstn_35"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "peri_ahb_rstn_34"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "peri_ahb_rstn_33"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "peri_ahb_rstn_32"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "peri_ahb_rstn_31"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "peri_ahb_rstn_30"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "peri_ahb_rstn_29"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "peri_ahb_rstn_28"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "wdreset_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "poreset_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "cpureset_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "reset_l2_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "scurst_n"                        },
};

/*-----------------------------------------------------------------------------
	0x0034 ctr09 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR09[] = {                            /* 0x0034 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "frc_tcon_apb_sel"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "led_cpuclk_off"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 5, "led_cpu_clk"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "vdclk_sel"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "vdclk_off"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 9, "gpuclk_sel"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "gpuclk_off"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "tcon_aclk_off"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "vp_aclk_off"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "frc_aclk_off"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "de_aclk_off"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "aud_dsp1aclk_off"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "aud_dsp0aclk_off"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "aud_aclk_off"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "vd_aclk_off"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "ve_aclk_off"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "te_aclk_off"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "gfx_aclk_off"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "gpu_aclk_off"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,24, "coreclock_sel"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,26, "frc_mclk_sel"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,28, "m_mclk_sel"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "frcclk_sel"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "pll_sel"                         },
};

/*-----------------------------------------------------------------------------
	0x0038 ctr10 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR10[] = {                            /* 0x0038 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "vpclk_off"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "gfxclk_off"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "teclk_off"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "frcclk_off"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "de270clk_off"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "ve270clk_off"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "te_ah270clk_off"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "declk_off"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "veclk_off"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "te_ahclk_off"                    },
};

/*-----------------------------------------------------------------------------
	0x003c ctr11 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR11[] = {                            /* 0x003c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "rst_frc_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "rst_de_vd_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "rst_de_de_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "rst_gfx_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "rst_ve_n"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "rst_teah_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "rst_te_te_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "rst_te_ve_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "rst_te_vd_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "rst_vd_n"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "rst_tcona_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "rst_vpa_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "rst_frca_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "rst_dea_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "rst_vda_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "rst_gpu_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "rst_vea_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "rst_tea_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "rst_gfxa_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "rst_gpua_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "rst_apb_frc_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "rst_apb_gfx_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "rst_apb_gpu_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "rst_apb_de_n"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "rst_apb_te_n"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "rst_apb_ve_n"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "rst_apb_vd_n"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "rst_apb_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "rst_axi_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "rst_frc_m_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "rst_mm_n"                        },
};

/*-----------------------------------------------------------------------------
	0x0040 ctr12 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR12[] = {                            /* 0x0040 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "rst_frc_m_half_n"                },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "rst_mm_half_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "rst_axi2_n_10"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "rst_axi2_n_9"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "rst_axi2_n_8"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "rst_axi2_n_7"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "rst_axi2_n_6"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "rst_axi2_n_5"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "rst_axi2_n_4"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "rst_axi2_n_3"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "rst_axi2_n_2"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "rst_axi2_n_1"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "rst_axi1_n_10"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "rst_axi1_n_9"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "rst_axi1_n_8"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "rst_axi1_n_7"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "rst_axi1_n_6"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "rst_axi1_n_5"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "rst_axi1_n_4"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "rst_axi1_n_3"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "rst_axi1_n_2"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "rst_axi1_n_1"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "rst_axi0_n_10"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "rst_axi0_n_9"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "rst_axi0_n_8"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "rst_axi0_n_7"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "rst_axi0_n_6"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "rst_axi0_n_5"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "rst_axi0_n_4"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "rst_axi0_n_3"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "rst_axi0_n_2"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "rst_axi0_n_1"                    },
};

/*-----------------------------------------------------------------------------
	0x0044 ctr13 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR13[] = {                            /* 0x0044 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "rst_vp_n"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "rst_apb_tcon_n"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "rst_apb_oif_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "rst_apb_led_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "rst_led_cpu_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "rst_tcon_pix_n"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "rst_frc_pix_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "rst_led_pix_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "rst_oif_pix_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "lvds_pix_clk"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "tcon_pix_clk_off"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "tcon_pix_clk_sel"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "frc_pix_clk_off"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "led_pix_clk_off"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "oif_pix_clk_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "dispclk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "rst_dea_dp_n"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "rst_disp_dp_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "rst_de_dp_n"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "rst_se_n"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "rst_s_te_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "rst_s_de_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "rst_disp_frc_n"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "rst_disp_de_n"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "rst_chb_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "rst_cha_n"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,27, "chbclk_sel"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,29, "chaclk_sel"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "dco_sel"                         },
};

/*-----------------------------------------------------------------------------
	0x0048 ctr14 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR14[] = {                            /* 0x0048 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "adto_loop_val"                   },
};

/*-----------------------------------------------------------------------------
	0x004c ctr15 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR15[] = {                            /* 0x004c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 28, 0, "adto_add_val"                    },
};

/*-----------------------------------------------------------------------------
	0x0050 ctr16 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR16[] = {                            /* 0x0050 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 0, "adto_err_val"                    },
};

/*-----------------------------------------------------------------------------
	0x0054 ctr17 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR17[] = {                            /* 0x0054 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,14, "audio_sel8"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,16, "audio_sel7"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,18, "audio_sel6"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "audio_sel5"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,22, "audio_sel4"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,24, "audio_sel3"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,26, "audio_sel2"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,28, "audio_sel1"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,30, "audio_sel0"                      },
};

/*-----------------------------------------------------------------------------
	0x0058 ctr18 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR18[] = {                            /* 0x0058 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "fs23clk_lsb_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "ext_en"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 5, "fs23clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 7, "fs22clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,10, "fs21clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,13, "fs20clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,16, "fs12clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,19, "fs11clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,22, "fs10clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,26, "fs04clk_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "fs03clk_sel"                     },
};

/*-----------------------------------------------------------------------------
	0x005c ctr19 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR19[] = {                            /* 0x005c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "de_mon"                          },
};

/*-----------------------------------------------------------------------------
	0x0060 ctr20 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR20[] = {                            /* 0x0060 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "cpu_gpu_mon"                     },
};

/*-----------------------------------------------------------------------------
	0x0064 ctr21 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR21[] = {                            /* 0x0064 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "frc_mon_0"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "gfx_mon"                         },
};

/*-----------------------------------------------------------------------------
	0x0068 ctr22 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR22[] = {                            /* 0x0068 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "aud_mon"                         },
};

/*-----------------------------------------------------------------------------
	0x006c ctr23 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR23[] = {                            /* 0x006c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 21, 0, "aad_mon"                         },
};

/*-----------------------------------------------------------------------------
	0x0070 ctr24 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR24[] = {                            /* 0x0070 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "gpio7_en"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "gpio6_en"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "gpio5_en"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "gpio4_en"                        },
};

/*-----------------------------------------------------------------------------
	0x0074 ctr25 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR25[] = {                            /* 0x0074 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 1, "gpio11_en"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 8, "gpio10_en"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,16, "gpio9_en"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "gpio8_en"                        },
};

/*-----------------------------------------------------------------------------
	0x0078 ctr26 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR26[] = {                            /* 0x0078 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 0, "cvbsout_en"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "sdhc_pull_ctr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "i2c_sel"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "frc_mon_1"                       },
};

/*-----------------------------------------------------------------------------
	0x007c ctr27 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR27[] = {                            /* 0x007c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "ddc_ck_pull_ctr"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "ddc_da_pull_ctr"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "tun_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "led_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "frame_lr_pull_ctr"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "lcd_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "cam_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "eb_data_pull_ctr1"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "eb_data_pull_ctr0"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "eb_cs_pull_ctr"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "nand_pull_ctr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "rmii_pull_ctr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "ext_intr_pull_ctr"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "pwm_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "i2c_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "uart_pull_ctr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "smc_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "audio_pull_ctr2"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "audio_pull_ctr1"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "audio_pull_ctr0"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "spi1_pull_ctr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "spi0_pull_ctr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "tpo_data_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "tpo_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "tpi_data_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "tpi_pull_ctr"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "spto_pull_ctr"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "stpi_pull_ctr"                   },
};

/*-----------------------------------------------------------------------------
	0x0080 ctr28 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0084 ctr29 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0088 ctr30 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x008c ctr31 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0090 ctr32 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR32[] = {                            /* 0x0090 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 23, 0, "en"                              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "sw_4sd_0_135_l_0_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "rmlvds_en"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "mini_flip_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "mini_en"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "sw_4sd_0_135_l_1_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "sw_4sd_0_135_l_2_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "rf"                              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "pdb"                             },
};

/*-----------------------------------------------------------------------------
	0x0094 ctr33 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR33[] = {                            /* 0x0094 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "enpemp_h"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 8, "rs"                              },
};

/*-----------------------------------------------------------------------------
	0x0098 ctr34 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR34[] = {                            /* 0x0098 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "ovdrv"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,16, "enpemp_l"                        },
};

/*-----------------------------------------------------------------------------
	0x009c ctr35 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR35[] = {                            /* 0x009c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "vsb_inck_sel"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 4, "aad_dco_lpfon"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27, 6, "aad_dco_fcw"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "aad_dco_resetb"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "aad_dco_rtest"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "aad_adc_sifsel"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "aad_adc_pdb"                     },
};

/*-----------------------------------------------------------------------------
	0x00a0 ctr36 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR36[] = {                            /* 0x00a0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "aad_btscclk_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 7, "aad_fs02clk_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 9, "aad_fs01clk_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "aad_fs00clk_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "aad_adcclk_test"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "aad_spllclk_test"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "aad_spll_pdb"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "aad_spll_sel"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,16, "aad_spll_cvs"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,18, "aad_spll_cvl"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "aad_spll_ci"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,22, "aad_spll_od"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,24, "aad_spll_n"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,30, "aad_spll_m"                      },
};

/*-----------------------------------------------------------------------------
	0x00a4 ctr37 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR37[] = {                            /* 0x00a4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 0, "hp_test1"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 2, "hp_slop_dig"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 5, "auda_bci"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "aud_lin_drv_ch_mute"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "aud_pdb_ldrv_in"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "auad_rec_ch_mute"                },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "auad_pdb_in"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "aud_lindrv0_ch_sel"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,15, "aud_lindrv1_ch_sel"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "auad_dc_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,19, "auad_ch_sel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,22, "auad_gcon"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,26, "auad_ccon"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "auda_pdb"                        },
};

/*-----------------------------------------------------------------------------
	0x00a8 ctr38 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR38[] = {                            /* 0x00a8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "ovdrv"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "hp_pdb"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16, 9, "hp_ssp_n"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,17, "hp_ssp_m"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,19, "hp_ssp_od"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,21, "hp_ssp_ms"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "hp_ssp_sr"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,28, "hp_ssp_cih"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "hp_ssp_cv"                       },
};

/*-----------------------------------------------------------------------------
	0x00ac ctr39 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR39[] = {                            /* 0x00ac */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "sw_4sd_90_45_r_0_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "sw_4sd_90_45_r_1_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "sw_4sd_90_45_r_2_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "sw_4sd_90_45_l_0_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "sw_4sd_90_45_l_1_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "sw_4sd_90_45_l_2_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "sw_4sd_0_135_r_0_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "sw_4sd_0_135_r_1_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "sw_4sd_0_135_r_2_off"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "sel_90_45_r"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "sel_90_45_l"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "sel_0_135_r"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "sel_0_135_l"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "mini_en135d"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "mini_en90d"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "mini_en45d"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "mini_en0d"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "aumi_pop"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "aumi_pdb_in"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,20, "aumi_ccon"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,23, "aumi_cont"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,27, "aumi_cnt"                        },
};

/*-----------------------------------------------------------------------------
	0x00b0 ctr40 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR40[] = {                            /* 0x00b0 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 8, "out_sel"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,11, "sgm_zero_sel"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,14, "test1"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,17, "test0"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,20, "dif1"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,23, "dif0"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,26, "dem0"                            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "tdi"                             },
};

/*-----------------------------------------------------------------------------
	0x00b4 ctr41 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR41[] = {                            /* 0x00b4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "pwm_sel"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,24, "dif"                             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,26, "dat_hp_vol_con"                  },
};

/*-----------------------------------------------------------------------------
	0x00b8 ctr42 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR42[] = {                            /* 0x00b8 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "testinr"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "testinl"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "testr"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "testl"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,18, "dig_sel"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,20, "vol_gain"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "bm_20"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,27, "pol_con"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "cont"                            },
};

/*-----------------------------------------------------------------------------
	0x00bc ctr43 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR43[] = {                            /* 0x00bc */
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "testin_mic"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "test_mic"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,18, "dig_sel_mic"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,20, "vol_gain_mic"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "bm_20_mic"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,27, "pol_con_mic"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "cont_mic"                        },
};

/*-----------------------------------------------------------------------------
	0x00c0 ctr44 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR44[] = {                            /* 0x00c0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "dr3p_dtrsel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 3, "dr3p_od_cvdclk"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 6, "dr3p_nsc"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,10, "dr3p_npc"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,16, "dr3p_msex"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,25, "dr3p_m"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "dr3p_lf"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "dr3p_cih"                        },
};

/*-----------------------------------------------------------------------------
	0x00c4 ctr45 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR45[] = {                            /* 0x00c4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,21, "dr3p_udex"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "dr3p_ps23c"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "dr3p_pdb"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "dr3p_od_recclk"                  },
};

/*-----------------------------------------------------------------------------
	0x00c8 ctr46 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR46[] = {                            /* 0x00c8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "dr3p_dtrsel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 3, "dr3p_od_cvdclk"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 6, "dr3p_nsc"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,10, "dr3p_npc"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,16, "dr3p_msex"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,25, "dr3p_m"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "dr3p_lf"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "dr3p_cih"                        },
};

/*-----------------------------------------------------------------------------
	0x00cc ctr47 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR47[] = {                            /* 0x00cc */
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,21, "dr3p_udex"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "dr3p_ps23c"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "dr3p_pdb"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "dr3p_od_recclk"                  },
};

/*-----------------------------------------------------------------------------
	0x00d0 ctr48 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR48[] = {                            /* 0x00d0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "dr3p_dtrsel"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 3, "dr3p_od_cvdclk"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 6, "dr3p_nsc"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,10, "dr3p_npc"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,16, "dr3p_msex"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,25, "dr3p_m"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "dr3p_lf"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "dr3p_cih"                        },
};

/*-----------------------------------------------------------------------------
	0x00d4 ctr49 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR49[] = {                            /* 0x00d4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,21, "dr3p_udex"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "dr3p_ps23c"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "dr3p_pdb"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "dr3p_od_recclk"                  },
};

/*-----------------------------------------------------------------------------
	0x00d8 ctr50 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR50[] = {                            /* 0x00d8 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 16, 4, "dpll_n"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,17, "dpll_m"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,26, "dpll_mode"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "dpll_en_edge"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "dpll_mode_en"                    },
};

/*-----------------------------------------------------------------------------
	0x00dc ctr51 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR51[] = {                            /* 0x00dc */
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,18, "dpll_cvs"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "dpll_cvl"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,22, "dpll_ci"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "dpll_pdb"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "dpll_sel"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,26, "dpll_od"                         },
};

/*-----------------------------------------------------------------------------
	0x00e0 ctr52 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR52[] = {                            /* 0x00e0 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 16, 4, "dpll_n"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,17, "dpll_m"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,26, "dpll_mode"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "dpll_en_edge"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "dpll_mode_en"                    },
};

/*-----------------------------------------------------------------------------
	0x00e4 ctr53 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR53[] = {                            /* 0x00e4 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,18, "dpll_cvs"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,20, "dpll_cvl"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,22, "dpll_ci"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "dpll_pdb"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "dpll_sel"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,26, "dpll_od"                         },
};

/*-----------------------------------------------------------------------------
	0x00e8 ctr54 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR54[] = {                            /* 0x00e8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "disp_bypass"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "disp_pdb"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,11, "disp_ms"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,14, "disp_sr"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,18, "disp_od"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,21, "disp_n"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,29, "disp_m"                          },
};

/*-----------------------------------------------------------------------------
	0x00ec ctr55 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR55[] = {                            /* 0x00ec */
	{ REG_XS_N_RW,  0, 0,  0, 0, 21, 0, "dco_fcw"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "dco_rtest"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "dco_resetb"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,24, "dco_lpfon"                       },
};

/*-----------------------------------------------------------------------------
	0x00f0 ctr56 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR56[] = {                            /* 0x00f0 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "dco_rtest"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "dco_resetb"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "dco_lpfon"                       },
};

/*-----------------------------------------------------------------------------
	0x00f4 ctr57 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR57[] = {                            /* 0x00f4 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "testdataoutsel0"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "testclk0"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "txpreemphasistune0"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "txrisetune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "txvreftune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "txfslstune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "vregtune0"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,13, "txhsxvtune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,15, "sqrxtune0"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,18, "compdistune0"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "loopbackenb0"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "otgdisable0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "portreset0"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "commononn"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,25, "refclksel"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,27, "refclkdiv"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "testen"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "vatestenb"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "siddq"                           },
};

/*-----------------------------------------------------------------------------
	0x00f8 ctr58 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR58[] = {                            /* 0x00f8 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "testdataout1_0_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "testdataout1_1_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "testdataout1_2_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "testdataout1_3_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "testdataout0_0_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "testdataout0_1_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "testdataout0_2_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "testdataout0_3_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "vregtune1"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,10, "txhsxvtune1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "sqrxtune1"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,15, "compdistune1"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "loopbackenb1"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "portreset1"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "testaddr0"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "testdatain0"                     },
};

/*-----------------------------------------------------------------------------
	0x00fc ctr59 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR59[] = {                            /* 0x00fc */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "bt_testdataout1_0_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "bt_testdataout1_1_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "bt_testdataout1_2_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "bt_testdataout1_3_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "bt_testdataout0_0_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "bt_testdataout0_1_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "bt_testdataout0_2_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "bt_testdataout0_3_en"            },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "testaddr1"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,12, "testdatain1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "testdataoutsel1"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "testclk1"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "txpreemphasistune1"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "txrisetune1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "txvreftune1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "txfslstune1"                     },
};

/*-----------------------------------------------------------------------------
	0x0100 ctr60 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR60[] = {                            /* 0x0100 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "testdataoutsel0"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "testclk0"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "txpreemphasistune0"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "txrisetune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "txvreftune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "txfslstune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "vregtune0"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,13, "txhsxvtune0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,15, "sqrxtune0"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,18, "compdistune0"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "loopbackenb0"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "otgdisable0"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "portreset0"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "commononn"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,25, "refclksel"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,27, "refclkdiv"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "testen"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "vatestenb"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "tsiddq"                          },
};

/*-----------------------------------------------------------------------------
	0x0104 ctr61 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR61[] = {                            /* 0x0104 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "demod_reset_sel"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 1, "bbmode_sel"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 3, "uar0_sel"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "cvbs_input_mux"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "vregtune1"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,10, "txhsxvtune1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "sqrxtune1"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,15, "compdistune1"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "loopbackenb1"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "portreset1"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,20, "testaddr0"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,24, "testdatain0"                     },
};

/*-----------------------------------------------------------------------------
	0x0108 ctr62 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR62[] = {                            /* 0x0108 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "jtag_sel"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 3, "uart2_sel"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "uart1_sel"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "testaddr1"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,12, "testdatain1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "testdataoutsel1"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "testclk1"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "txpreemphasistune1"              },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "txrisetune1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,24, "txvreftune1"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,28, "txfslstune1"                     },
};

/*-----------------------------------------------------------------------------
	0x010c ctr63 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_CTR63[] = {                            /* 0x010c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "gpo_probe_0_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "gpo_probe_1_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "gpo_probe_2_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "gpo_probe_3_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "gpo_probe_4_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "gpo_probe_5_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 6, "gpo_probe_6_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 7, "gpo_probe_7_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "gpo_probe_8_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "gpo_probe_9_en"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "gpo_probe_10_en"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,11, "gpo_probe_11_en"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12,12, "q_probe_0_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,13, "q_probe_1_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,14, "q_probe_2_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,15, "q_probe_3_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "q_probe_4_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 17,17, "q_probe_5_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 18,18, "q_probe_6_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,19, "q_probe_7_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 20,20, "q_probe_8_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 21,21, "q_probe_9_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 22,22, "i_probe_0_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 23,23, "i_probe_1_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 24,24, "i_probe_2_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 25,25, "i_probe_3_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 26,26, "i_probe_4_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 27,27, "i_probe_5_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 28,28, "i_probe_6_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,29, "i_probe_7_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "i_probe_8_en"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "i_probe_9_en"                    },
};

DBG_REG_T gDbgRegCTOP[] = {
	{0x0010,N_FLD(dbg_CTR00),                           "CTR00"                           ,dbg_CTR00                           }, //
	{0x0014,N_FLD(dbg_CTR01),                           "CTR01"                           ,dbg_CTR01                           }, //
	{0x0018,N_FLD(dbg_CTR02),                           "CTR02"                           ,dbg_CTR02                           }, //
	{0x001c,0,                                  		"CTR03"                           ,NULL                                }, //
	{0x0020,0,                                  		"CTR04"                           ,NULL                                }, //
	{0x0024,0,                                  		"CTR05"                           ,NULL                                }, //
	{0x0028,N_FLD(dbg_CTR06),                           "CTR06"                           ,dbg_CTR06                           }, //
	{0x002c,N_FLD(dbg_CTR07),                           "CTR07"                           ,dbg_CTR07                           }, //
	{0x0030,N_FLD(dbg_CTR08),                           "CTR08"                           ,dbg_CTR08                           }, //
	{0x0034,N_FLD(dbg_CTR09),                           "CTR09"                           ,dbg_CTR09                           }, //
	{0x0038,N_FLD(dbg_CTR10),                           "CTR10"                           ,dbg_CTR10                           }, //
	{0x003c,N_FLD(dbg_CTR11),                           "CTR11"                           ,dbg_CTR11                           }, //
	{0x0040,N_FLD(dbg_CTR12),                           "CTR12"                           ,dbg_CTR12                           }, //
	{0x0044,N_FLD(dbg_CTR13),                           "CTR13"                           ,dbg_CTR13                           }, //
	{0x0048,N_FLD(dbg_CTR14),                           "CTR14"                           ,dbg_CTR14                           }, //
	{0x004c,N_FLD(dbg_CTR15),                           "CTR15"                           ,dbg_CTR15                           }, //
	{0x0050,N_FLD(dbg_CTR16),                           "CTR16"                           ,dbg_CTR16                           }, //
	{0x0054,N_FLD(dbg_CTR17),                           "CTR17"                           ,dbg_CTR17                           }, //
	{0x0058,N_FLD(dbg_CTR18),                           "CTR18"                           ,dbg_CTR18                           }, //
	{0x005c,N_FLD(dbg_CTR19),                           "CTR19"                           ,dbg_CTR19                           }, //
	{0x0060,N_FLD(dbg_CTR20),                           "CTR20"                           ,dbg_CTR20                           }, //
	{0x0064,N_FLD(dbg_CTR21),                           "CTR21"                           ,dbg_CTR21                           }, //
	{0x0068,N_FLD(dbg_CTR22),                           "CTR22"                           ,dbg_CTR22                           }, //
	{0x006c,N_FLD(dbg_CTR23),                           "CTR23"                           ,dbg_CTR23                           }, //
	{0x0070,N_FLD(dbg_CTR24),                           "CTR24"                           ,dbg_CTR24                           }, //
	{0x0074,N_FLD(dbg_CTR25),                           "CTR25"                           ,dbg_CTR25                           }, //
	{0x0078,N_FLD(dbg_CTR26),                           "CTR26"                           ,dbg_CTR26                           }, //
	{0x007c,N_FLD(dbg_CTR27),                           "CTR27"                           ,dbg_CTR27                           }, //
	{0x0080,0,                                  		"CTR28"                           ,NULL                                }, //
	{0x0084,0,                                  		"CTR29"                           ,NULL                                }, //
	{0x0088,0,                                  		"CTR30"                           ,NULL                                }, //
	{0x008c,0,                                  		"CTR31"                           ,NULL                                }, //
	{0x0090,N_FLD(dbg_CTR32),                           "CTR32"                           ,dbg_CTR32                           }, //
	{0x0094,N_FLD(dbg_CTR33),                           "CTR33"                           ,dbg_CTR33                           }, //
	{0x0098,N_FLD(dbg_CTR34),                           "CTR34"                           ,dbg_CTR34                           }, //
	{0x009c,N_FLD(dbg_CTR35),                           "CTR35"                           ,dbg_CTR35                           }, //
	{0x00a0,N_FLD(dbg_CTR36),                           "CTR36"                           ,dbg_CTR36                           }, //
	{0x00a4,N_FLD(dbg_CTR37),                           "CTR37"                           ,dbg_CTR37                           }, //
	{0x00a8,N_FLD(dbg_CTR38),                           "CTR38"                           ,dbg_CTR38                           }, //
	{0x00ac,N_FLD(dbg_CTR39),                           "CTR39"                           ,dbg_CTR39                           }, //
	{0x00b0,N_FLD(dbg_CTR40),                           "CTR40"                           ,dbg_CTR40                           }, //
	{0x00b4,N_FLD(dbg_CTR41),                           "CTR41"                           ,dbg_CTR41                           }, //
	{0x00b8,N_FLD(dbg_CTR42),                           "CTR42"                           ,dbg_CTR42                           }, //
	{0x00bc,N_FLD(dbg_CTR43),                           "CTR43"                           ,dbg_CTR43                           }, //
	{0x00c0,N_FLD(dbg_CTR44),                           "CTR44"                           ,dbg_CTR44                           }, //
	{0x00c4,N_FLD(dbg_CTR45),                           "CTR45"                           ,dbg_CTR45                           }, //
	{0x00c8,N_FLD(dbg_CTR46),                           "CTR46"                           ,dbg_CTR46                           }, //
	{0x00cc,N_FLD(dbg_CTR47),                           "CTR47"                           ,dbg_CTR47                           }, //
	{0x00d0,N_FLD(dbg_CTR48),                           "CTR48"                           ,dbg_CTR48                           }, //
	{0x00d4,N_FLD(dbg_CTR49),                           "CTR49"                           ,dbg_CTR49                           }, //
	{0x00d8,N_FLD(dbg_CTR50),                           "CTR50"                           ,dbg_CTR50                           }, //
	{0x00dc,N_FLD(dbg_CTR51),                           "CTR51"                           ,dbg_CTR51                           }, //
	{0x00e0,N_FLD(dbg_CTR52),                           "CTR52"                           ,dbg_CTR52                           }, //
	{0x00e4,N_FLD(dbg_CTR53),                           "CTR53"                           ,dbg_CTR53                           }, //
	{0x00e8,N_FLD(dbg_CTR54),                           "CTR54"                           ,dbg_CTR54                           }, //
	{0x00ec,N_FLD(dbg_CTR55),                           "CTR55"                           ,dbg_CTR55                           }, //
	{0x00f0,N_FLD(dbg_CTR56),                           "CTR56"                           ,dbg_CTR56                           }, //
	{0x00f4,N_FLD(dbg_CTR57),                           "CTR57"                           ,dbg_CTR57                           }, //
	{0x00f8,N_FLD(dbg_CTR58),                           "CTR58"                           ,dbg_CTR58                           }, //
	{0x00fc,N_FLD(dbg_CTR59),                           "CTR59"                           ,dbg_CTR59                           }, //
	{0x0100,N_FLD(dbg_CTR60),                           "CTR60"                           ,dbg_CTR60                           }, //
	{0x0104,N_FLD(dbg_CTR61),                           "CTR61"                           ,dbg_CTR61                           }, //
	{0x0108,N_FLD(dbg_CTR62),                           "CTR62"                           ,dbg_CTR62                           }, //
	{0x010c,N_FLD(dbg_CTR63),                           "CTR63"                           ,dbg_CTR63                           }, //
};

/* 64 regs, 64 types in Total*/

/* from 'L8_CTOP_CTRL_RegFile_100310.csv' 20100311 00:54:16   대한민국 표준시 by getregs v2.3 */
