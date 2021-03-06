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

#ifndef _REG_IPC_L9A0_h
#define _REG_IPC_L9A0_h

typedef struct { // 0x4e00
	UINT32 run_stall                   : 1;   //     0
	UINT32 start_vector_sel            : 1;   //     1
	UINT32 p_debug_enable              : 1;   //     2
	UINT32                             : 21;  // 23: 3
	UINT32 sw_reset_fdone              : 1;   //    24
	UINT32                             : 3;   // 27:25
	UINT32 sw_reset                    : 1;   //    28
} L9A0_SET_CTRL0_T;

typedef struct { // 0x4e04
	UINT32 prid                        : 16;  // 15: 0
} L9A0_SET_CTRL1_T;

typedef struct { // 0x4e08
	UINT32 ipc_interrupt_event_arm     : 30;  // 29: 0
	UINT32 edma_interrupt_event_arm    : 1;   //    30
	UINT32 wdg_interrupt_event         : 1;   //    31
} L9A0_EXT_INTR_EVENT_T;

typedef struct { // 0x4e0c
	UINT32 ipc_interrupt_enable_arm    : 30;  // 29: 0
	UINT32 edma_interrupt_enable_arm   : 1;   //    30
	UINT32 wdg_interrupt_enable        : 1;   //    31
} L9A0_EXT_INTR_ENABLE_T;

typedef struct { // 0x4e10
	UINT32 ipc_interrupt_status_arm    : 30;  // 29: 0
	UINT32 edma_interrupt_status_arm   : 1;   //    30
	UINT32 wdg_interrupt_status        : 1;   //    31
} L9A0_EXT_INTR_STATUS_T;

typedef struct { // 0x4e14
	UINT32 ipc_interrupt_clear_arm     : 30;  // 29: 0
	UINT32 edma_interrupt_clear_arm    : 1;   //    30
	UINT32 wdg_interrupt_clear         : 1;   //    31
} L9A0_EXT_INTR_CLEAR_T;

typedef struct { // 0x4e18
	UINT32 ipc_interrupt_event_mcu     : 30;  // 29: 0
	UINT32 edma_interrupt_event_mcu    : 1;   //    30
} L9A0_INT_INTR_EVENT_T;

typedef struct { // 0x4e1c
	UINT32 ipc_interrupt_enable_mcu    : 30;  // 29: 0
	UINT32 edma_interrupt_enable_mcu   : 1;   //    30
} L9A0_INT_INTR_ENABLE_T;

typedef struct { // 0x4e20
	UINT32 ipc_interrupt_status_mcu    : 30;  // 29: 0
	UINT32 edma_interrupt_status_mcu   : 1;   //    30
} L9A0_INT_INTR_STATUS_T;

typedef struct { // 0x4e24
	UINT32 ipc_interrupt_clear_mcu     : 30;  // 29: 0
	UINT32 edma_interrupt_clear_mcu    : 1;   //    30
} L9A0_INT_INTR_CLEAR_T;

typedef struct { // 0x4e28
	UINT32 boot_img_offset             : 32;  // 31: 0
} L9A0_SROM_BOOT_OFFSET_T;

typedef struct { // 0x4e2c
	UINT32 sram2_atlas_port_sel        : 3;   //  2: 0
	UINT32                             : 1;   //     3
	UINT32 sram1_atlas_port_sel        : 3;   //  6: 4
	UINT32                             : 1;   //     7
	UINT32 sram0_atlas_port_sel        : 3;   // 10: 8
	UINT32                             : 1;   //    11
	UINT32 edma_atlas_port_sel         : 3;   // 14:12
} L9A0_ATLAS_PORT_SEL_T;

typedef struct { // 0x4e30
	UINT32 pdebug_status               : 8;   //  7: 0
} L9A0_PDEBUG_STATUS_T;

typedef struct { // 0x4e34
	UINT32 pdebug_data                 : 32;  // 31: 0
} L9A0_PDEBUG_DATA_T;

typedef struct { // 0x4e38
	UINT32 pdebug_pc                   : 32;  // 31: 0
} L9A0_PDEBUG_PC_T;

typedef struct { // 0x4e3c
	UINT32 axi_user_id                 : 3;   //  2: 0
} L9A0_AXI_USER_ID_T;

typedef struct { // 0x4e40
	UINT32 sram0_atlas_offset          : 32;  // 31: 0
} L9A0_SRAM0_ATLAS_OFFSET_T;

typedef struct { // 0x4e44
	UINT32 sram1_atlas_offset          : 32;  // 31: 0
} L9A0_SRAM1_ATLAS_OFFSET_T;

typedef struct { // 0x4e48
	UINT32 sram2_atlas_offset          : 32;  // 31: 0
} L9A0_SRAM2_ATLAS_OFFSET_T;

typedef struct { // 0x4e5c
	UINT32 mcu_version_number          : 32;  // 31: 0
} L9A0_SPP_MCU_VERSION_T;

typedef struct { // 0x4f00
	UINT32 tx_locked_id                : 32;  // 31: 0
} L9A0_TX_LOCKED_ID_T;

typedef struct { // 0x4f04
	UINT32 tx_src_addr                 : 32;  // 31: 0
} L9A0_TX_SRC_ADDR_T;

typedef struct { // 0x4f08
	UINT32 tx_dst_addr                 : 32;  // 31: 0
} L9A0_TX_DST_ADDR_T;

typedef struct { // 0x4f0c
	UINT32 tx_nxt_addr                 : 32;  // 31: 0
} L9A0_TX_NXT_ADDR_T;

typedef struct { // 0x4f10
	UINT32 tx_length                   : 32;  // 31: 0
} L9A0_TX_LENGTH_T;

typedef struct { // 0x4f14
	UINT32 tx_start                    : 1;   //     0
	UINT32 tx_dir                      : 3;   //  3: 1
	UINT32 tx_nxt_dir                  : 1;   //     4
	UINT32                             : 3;   //  7: 5
	UINT32 tx_done                     : 1;   //     8
	UINT32 tx_locked                   : 1;   //     9
	UINT32 tx_switch_en                : 1;   //    10
	UINT32                             : 1;   //    11
	UINT32 tx_atomic_size              : 2;   // 13:12
} L9A0_EDMA_CTRL_T;

typedef struct { // 0x4f18
	UINT32 pattern_addr                : 32;  // 31: 0
} L9A0_PATTERN_ADDR_T;

typedef struct { // 0x4f1c
	UINT32 pattern_addr                : 32;  // 31: 0
} L9A0_PATTERN_DATA_T;

typedef struct {
	L9A0_SET_CTRL0_T            set_ctrl0;                   // 0x4e00
	L9A0_SET_CTRL1_T            set_ctrl1;                   // 0x4e04
	L9A0_EXT_INTR_EVENT_T       ext_intr_event;              // 0x4e08
	L9A0_EXT_INTR_ENABLE_T      ext_intr_enable;             // 0x4e0c
	L9A0_EXT_INTR_STATUS_T      ext_intr_status;             // 0x4e10
	L9A0_EXT_INTR_CLEAR_T       ext_intr_clear;              // 0x4e14
	L9A0_INT_INTR_EVENT_T       int_intr_event;              // 0x4e18
	L9A0_INT_INTR_ENABLE_T      int_intr_enable;             // 0x4e1c
	L9A0_INT_INTR_STATUS_T      int_intr_status;             // 0x4e20
	L9A0_INT_INTR_CLEAR_T       int_intr_clear;              // 0x4e24
	L9A0_SROM_BOOT_OFFSET_T     srom_boot_offset;            // 0x4e28
	L9A0_ATLAS_PORT_SEL_T       atlas_port_sel;              // 0x4e2c
	L9A0_PDEBUG_STATUS_T        pdebug_status;               // 0x4e30
	L9A0_PDEBUG_DATA_T          pdebug_data;                 // 0x4e34
	L9A0_PDEBUG_PC_T            pdebug_pc;                   // 0x4e38
	L9A0_AXI_USER_ID_T          axi_user_id;                 // 0x4e3c
	L9A0_SRAM0_ATLAS_OFFSET_T   sram0_atlas_offset;          // 0x4e40
	L9A0_SRAM1_ATLAS_OFFSET_T   sram1_atlas_offset;          // 0x4e44
	L9A0_SRAM2_ATLAS_OFFSET_T   sram2_atlas_offset;          // 0x4e48
	UINT32                      reserved0[4];                // 0x4e4c~0x4e58
	L9A0_SPP_MCU_VERSION_T      spp_mcu_version;             // 0x4e5c
	UINT32                      reserved1[40];               // 0x4e60~0x4efc
	L9A0_TX_LOCKED_ID_T         tx_locked_id;                // 0x4f00
	L9A0_TX_SRC_ADDR_T          tx_src_addr;                 // 0x4f04
	L9A0_TX_DST_ADDR_T          tx_dst_addr;                 // 0x4f08
	L9A0_TX_NXT_ADDR_T          tx_nxt_addr;                 // 0x4f0c
	L9A0_TX_LENGTH_T            tx_length;                   // 0x4f10
	L9A0_EDMA_CTRL_T            edma_ctrl;                   // 0x4f14
	L9A0_PATTERN_ADDR_T         pattern_addr;                // 0x4f18
	L9A0_PATTERN_DATA_T         pattern_data;                // 0x4f1c
} DE_IPC_REG_L9A0_T;

#endif
