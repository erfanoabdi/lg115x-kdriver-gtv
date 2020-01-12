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
	0x0000 108m_set_reg ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_108M_SET_REG[] = {                     /* 0x0000 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "runstall"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  1, 1, "startvectorsel"                  },
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 2, "pdebugenable"                    },
};

/*-----------------------------------------------------------------------------
	0x0004 108m_prid ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_108M_PRID[] = {                        /* 0x0004 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 15, 0, "prid"                            },
};

/*-----------------------------------------------------------------------------
	0x0008 ext_interrupt_event ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_EXT_INTERRUPT_EVENT[] = {              /* 0x0008 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_interrupt_event_arm"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_intterupt_event_arm"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "wdg_interrupt_event"             },
};

/*-----------------------------------------------------------------------------
	0x000c ext_interrupt_enable ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_EXT_INTERRUPT_ENABLE[] = {             /* 0x000c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_interrupt_enable_arm"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_intterupt_enable_arm"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "wdg_interrupt_enable"            },
};

/*-----------------------------------------------------------------------------
	0x0010 ext_interrupt_status ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_EXT_INTERRUPT_STATUS[] = {             /* 0x0010 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_interrupt_status_arm"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_intterupt_status_arm"       },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "wdg_interrupt_status"            },
};

/*-----------------------------------------------------------------------------
	0x0014 ext_interrupt_clear ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_EXT_INTERRUPT_CLEAR[] = {              /* 0x0014 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_interrupt_clear_arm"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_intterupt_clear_arm"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 31,31, "wdg_interrupt_clear"             },
};

/*-----------------------------------------------------------------------------
	0x0018 int_interrupt_event ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_INT_INTERRUPT_EVENT[] = {              /* 0x0018 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_interrupt_event_108"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_interrupt_event_108"        },
};

/*-----------------------------------------------------------------------------
	0x001c int_interrupt_enable ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_INT_INTERRUPT_ENABLE[] = {             /* 0x001c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_intterupt_enalbe_108"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_interrupt_enable_108"       },
};

/*-----------------------------------------------------------------------------
	0x0020 int_interrupt_status ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_INT_INTERRUPT_STATUS[] = {             /* 0x0020 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_interrupt_status_108"        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_interrupt_status_108"       },
};

/*-----------------------------------------------------------------------------
	0x0024 int_interrupt_clear ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_INT_INTERRUPT_CLEAR[] = {              /* 0x0024 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 29, 0, "ipc_interrupt_clear_108"         },
	{ REG_XS_N_RW,  0, 0,  0, 0, 30,30, "edma_interrupt_clear_108"        },
};

/*-----------------------------------------------------------------------------
	0x0028 srom boot offset ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SROM BOOT OFFSET[] = {                 /* 0x0028 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "boot_img_offset"                 },
};

/*-----------------------------------------------------------------------------
	0x002c silk_port_sel ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SILK_PORT_SEL[] = {                    /* 0x002c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "sram2_silk_port_sel"             },
	{ REG_XS_N_RW,  0, 0,  0, 0,  6, 4, "sram1_silk_port_sel"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10, 8, "sram0_silk_port_sel"             },
	{ REG_XS_N_RW,  0, 0,  0, 0, 14,12, "edma_silk_port_sel"              },
};

/*-----------------------------------------------------------------------------
	0x0030 pdebugstatus ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_PDEBUGSTATUS[] = {                     /* 0x0030 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  7, 0, "pdebugstatus"                    },
};

/*-----------------------------------------------------------------------------
	0x0034 pdebugdata ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_PDEBUGDATA[] = {                       /* 0x0034 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "pdebugdata"                      },
};

/*-----------------------------------------------------------------------------
	0x0038 pdebugpc ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_PDEBUGPC[] = {                         /* 0x0038 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "pdebugpc"                        },
};

/*-----------------------------------------------------------------------------
	0x003c axi_user_id ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_AXI_USER_ID[] = {                      /* 0x003c */
	{ REG_XS_N_RW,  0, 0,  0, 0,  2, 0, "axi_user_id"                     },
};

/*-----------------------------------------------------------------------------
	0x005c spp_mcu version ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_SPP_MCU VERSION[] = {                  /* 0x005c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "frc_mcu_ver"                     },
};

/*-----------------------------------------------------------------------------
	0x0000 txlockedid ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_TXLOCKEDID[] = {                       /* 0x0000 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "txlockedid"                      },
};

/*-----------------------------------------------------------------------------
	0x0004 txsrcaddr ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_TXSRCADDR[] = {                        /* 0x0004 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "txsrcaddr"                       },
};

/*-----------------------------------------------------------------------------
	0x0008 txdstaddr ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_TXDSTADDR[] = {                        /* 0x0008 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "txdstaddr"                       },
};

/*-----------------------------------------------------------------------------
	0x000c txnxtaddr ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_TXNXTADDR[] = {                        /* 0x000c */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "txnxtaddr"                       },
};

/*-----------------------------------------------------------------------------
	0x0010 txlength ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0014 edma control register ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_EDMA CONTROL REGISTER[] = {            /* 0x0014 */
	{ REG_XS_N_RW,  0, 0,  0, 0,  0, 0, "txstart"                         },
	{ REG_XS_N_RW,  0, 0,  0, 0,  3, 1, "txdir"                           },
	{ REG_XS_N_RW,  0, 0,  0, 0,  4, 4, "txnxtdir"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0,  8, 8, "txdone"                          },
	{ REG_XS_N_RW,  0, 0,  0, 0,  9, 9, "txlocked"                        },
	{ REG_XS_N_RW,  0, 0,  0, 0, 10,10, "txswitchen"                      },
	{ REG_XS_N_RW,  0, 0,  0, 0, 13,12, "txatomicsize"                    },
};

/*-----------------------------------------------------------------------------
	0x0018 patternaddr ''
------------------------------------------------------------------------------*/
const DBG_REG_FIELD_T dbg_PATTERNADDR[] = {                      /* 0x0018 */
	{ REG_XS_N_RW,  0, 0,  0, 0, 31, 0, "patternaddr"                     },
};

/*-----------------------------------------------------------------------------
	0x001c patterndata ''
------------------------------------------------------------------------------*/
/*	no field */

const DBG_REG_T gDbgRegSPP[] = {
{0x0000,N_FLD(dbg_108M_SET_REG),                    "108M_SET_REG"                    ,dbg_108M_SET_REG                    }, //
{0x0004,N_FLD(dbg_108M_PRID),                       "108M_PRID"                       ,dbg_108M_PRID                       }, //
{0x0008,N_FLD(dbg_EXT_INTERRUPT_EVENT),             "EXT_INTERRUPT_EVENT"             ,dbg_EXT_INTERRUPT_EVENT             }, //
{0x000c,N_FLD(dbg_EXT_INTERRUPT_ENABLE),            "EXT_INTERRUPT_ENABLE"            ,dbg_EXT_INTERRUPT_ENABLE            }, //
{0x0010,N_FLD(dbg_EXT_INTERRUPT_STATUS),            "EXT_INTERRUPT_STATUS"            ,dbg_EXT_INTERRUPT_STATUS            }, //
{0x0014,N_FLD(dbg_EXT_INTERRUPT_CLEAR),             "EXT_INTERRUPT_CLEAR"             ,dbg_EXT_INTERRUPT_CLEAR             }, //
{0x0018,N_FLD(dbg_INT_INTERRUPT_EVENT),             "INT_INTERRUPT_EVENT"             ,dbg_INT_INTERRUPT_EVENT             }, //
{0x001c,N_FLD(dbg_INT_INTERRUPT_ENABLE),            "INT_INTERRUPT_ENABLE"            ,dbg_INT_INTERRUPT_ENABLE            }, //
{0x0020,N_FLD(dbg_INT_INTERRUPT_STATUS),            "INT_INTERRUPT_STATUS"            ,dbg_INT_INTERRUPT_STATUS            }, //
{0x0024,N_FLD(dbg_INT_INTERRUPT_CLEAR),             "INT_INTERRUPT_CLEAR"             ,dbg_INT_INTERRUPT_CLEAR             }, //
{0x0028,N_FLD(dbg_SROM BOOT OFFSET),                "SROM BOOT OFFSET"                ,dbg_SROM BOOT OFFSET                }, //
{0x002c,N_FLD(dbg_SILK_PORT_SEL),                   "SILK_PORT_SEL"                   ,dbg_SILK_PORT_SEL                   }, //
{0x0030,N_FLD(dbg_PDEBUGSTATUS),                    "PDEBUGSTATUS"                    ,dbg_PDEBUGSTATUS                    }, //
{0x0034,N_FLD(dbg_PDEBUGDATA),                      "PDEBUGDATA"                      ,dbg_PDEBUGDATA                      }, //
{0x0038,N_FLD(dbg_PDEBUGPC),                        "PDEBUGPC"                        ,dbg_PDEBUGPC                        }, //
{0x003c,N_FLD(dbg_AXI_USER_ID),                     "AXI_USER_ID"                     ,dbg_AXI_USER_ID                     }, //
{0x005c,N_FLD(dbg_SPP_MCU VERSION),                 "SPP_MCU VERSION"                 ,dbg_SPP_MCU VERSION                 }, //
{0x0000,N_FLD(dbg_TXLOCKEDID),                      "TXLOCKEDID"                      ,dbg_TXLOCKEDID                      }, //
{0x0004,N_FLD(dbg_TXSRCADDR),                       "TXSRCADDR"                       ,dbg_TXSRCADDR                       }, //
{0x0008,N_FLD(dbg_TXDSTADDR),                       "TXDSTADDR"                       ,dbg_TXDSTADDR                       }, //
{0x000c,N_FLD(dbg_TXNXTADDR),                       "TXNXTADDR"                       ,dbg_TXNXTADDR                       }, //
{0x0010,0,                                  "TXLENGTH"                        ,NULL                                }, //
{0x0014,N_FLD(dbg_EDMA CONTROL REGISTER),           "EDMA CONTROL REGISTER"           ,dbg_EDMA CONTROL REGISTER           }, //
{0x0018,N_FLD(dbg_PATTERNADDR),                     "PATTERNADDR"                     ,dbg_PATTERNADDR                     }, //
{0x001c,0,                                  "PATTERNDATA"                     ,NULL                                }, //
};

/* 25 regs, 25 types in Total*/

/* from 'L8-DE_SPP_MCU_RegFile_091229.csv' 20100311 00:54:16   대한민국 표준시 by getregs v2.3 */
