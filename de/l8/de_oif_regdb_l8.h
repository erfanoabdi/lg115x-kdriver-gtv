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
	0x0000 out_video_info ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_OUT_VIDEO_INFO[] = {                   /* 0x0000 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 0, "out_video_info"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "out_sel"                         },
};

/*-----------------------------------------------------------------------------
	0x0004 data_color_from ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_DATA_COLOR_FROM[] = {                  /* 0x0004 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 0, "data_color_from"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 2, "g_data_from"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 4, "r_data_from"                     },
};

/*-----------------------------------------------------------------------------
	0x0008 data_color ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_DATA_COLOR[] = {                       /* 0x0008 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "data_color"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,10, "data_color_g"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,20, "data_color_r"                    },
};

/*-----------------------------------------------------------------------------
	0x000c sync_color ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SYNC_COLOR[] = {                       /* 0x000c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "sync_color"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 19,10, "sync_color_g"                    },
	{ REG_XS_N_RW,  0, 0,  0, 0, 29,20, "sync_color_r"                    },
};

/*-----------------------------------------------------------------------------
	0x0010 delay_data ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_DELAY_DATA[] = {                       /* 0x0010 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 12, 0, "delay_data"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 3, "g_delay"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 6, "r_delay"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 12, 9, "de_delay"                        },
};

/*-----------------------------------------------------------------------------
	0x0014 sync_de_control ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SYNC_DE_CONTROL[] = {                  /* 0x0014 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "sync_de_control"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "de_disable"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "hsync_pol"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "hsync_disable"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "vsync_pol"                       },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "vsync_disable"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 6, "out_sdq"                         },
};

/*-----------------------------------------------------------------------------
	0x0018 pwm_control ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_PWM_CONTROL[] = {                      /* 0x0018 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 0, "pwm_control"                     },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "pwm0_pol"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "pwm1_pol"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 3, "pwm2_pol"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "pwm3_pol"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  5, 5, "pwm4_pol"                        },
};

/*-----------------------------------------------------------------------------
	0x001c led_in_control ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_LED_IN_CONTROL[] = {                   /* 0x001c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "led_in_control"                  },
};

/*-----------------------------------------------------------------------------
	0x0020 lvds_if_control ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_LVDS_IF_CONTROL[] = {                  /* 0x0020 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 16, 0, "lvds_if_control"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 5, "lvds_sync_order"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 8, "lvds_ch3_from"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11,10, "lvds_ch2_from"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "lvds_ch1_from"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,14, "lvds_ch0_from"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 16,16, "lvds_out_sel"                    },
};

/*-----------------------------------------------------------------------------
	0x0024 lr_if_control ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_LR_IF_CONTROL[] = {                    /* 0x0024 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "lr_if_control"                   },
};

/*-----------------------------------------------------------------------------
	0x0030 reserved0 ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_RESERVED0[] = {                        /* 0x0030 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "reserved0"                       },
};

/*-----------------------------------------------------------------------------
	0x0034 lvds_en ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_LVDS_EN[] = {                          /* 0x0034 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 23, 0, "lvds_en"                         },
};

/*-----------------------------------------------------------------------------
	0x0038 lvds_force_data_en ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_LVDS_FORCE_DATA_EN[] = {               /* 0x0038 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "lvds_force_data_en"              },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "lvds_ch2_force_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "lvds_ch1_force_en"               },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "lvds_ch0_force_en"               },
};

/*-----------------------------------------------------------------------------
	0x003c lvds_force_data ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_LVDS_FORCE_DATA[] = {                  /* 0x003c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "lvds_force_data"                 },
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 4, "lvds_ch2_data"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 11, 8, "lvds_ch1_data"                   },
	{ REG_XS_N_RW,  0, 0,  0, 0, 15,12, "lvds_ch0_data"                   },
};

DBG_REG_T gDbgRegOIF[] = {
	{0x0000,N_FLD(dbg_OUT_VIDEO_INFO),                  "OUT_VIDEO_INFO"                  ,dbg_OUT_VIDEO_INFO                  }, //
	{0x0004,N_FLD(dbg_DATA_COLOR_FROM),                 "DATA_COLOR_FROM"                 ,dbg_DATA_COLOR_FROM                 }, //
	{0x0008,N_FLD(dbg_DATA_COLOR),                      "DATA_COLOR"                      ,dbg_DATA_COLOR                      }, //
	{0x000c,N_FLD(dbg_SYNC_COLOR),                      "SYNC_COLOR"                      ,dbg_SYNC_COLOR                      }, //
	{0x0010,N_FLD(dbg_DELAY_DATA),                      "DELAY_DATA"                      ,dbg_DELAY_DATA                      }, //
	{0x0014,N_FLD(dbg_SYNC_DE_CONTROL),                 "SYNC_DE_CONTROL"                 ,dbg_SYNC_DE_CONTROL                 }, //
	{0x0018,N_FLD(dbg_PWM_CONTROL),                     "PWM_CONTROL"                     ,dbg_PWM_CONTROL                     }, //
	{0x001c,N_FLD(dbg_LED_IN_CONTROL),                  "LED_IN_CONTROL"                  ,dbg_LED_IN_CONTROL                  }, //
	{0x0020,N_FLD(dbg_LVDS_IF_CONTROL),                 "LVDS_IF_CONTROL"                 ,dbg_LVDS_IF_CONTROL                 }, //
	{0x0024,N_FLD(dbg_LR_IF_CONTROL),                   "LR_IF_CONTROL"                   ,dbg_LR_IF_CONTROL                   }, //
	{0x0030,N_FLD(dbg_RESERVED0),                       "RESERVED0"                       ,dbg_RESERVED0                       }, //
	{0x0034,N_FLD(dbg_LVDS_EN),                         "LVDS_EN"                         ,dbg_LVDS_EN                         }, //
	{0x0038,N_FLD(dbg_LVDS_FORCE_DATA_EN),              "LVDS_FORCE_DATA_EN"              ,dbg_LVDS_FORCE_DATA_EN              }, //
	{0x003c,N_FLD(dbg_LVDS_FORCE_DATA),                 "LVDS_FORCE_DATA"                 ,dbg_LVDS_FORCE_DATA                 }, //
};

/* 14 regs, 14 types in Total*/

/* from 'LG1150-FRC-MAN-03 v0.9(OIF-20100309).csv' 20100311 00:54:18   대한민국 표준시 by getregs v2.3 */
