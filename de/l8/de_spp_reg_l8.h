typedef struct { // 0x1f80
	UINT32 runstall                    : 1;   // 0       0
	UINT32 startvectorsel              : 1;   // 1       1
	UINT32 pdebugenable                : 1;   // 2       2
	UINT32                             : 29;  // 31:3    3
} IPC_108M_SET_REG_T;

typedef struct { // 0x1f84
	UINT32 prid                        : 16;  // 15:0    0
	UINT32                             : 16;  // 31:16   16
} IPC_108M_PRID_T;

typedef struct { // 0x1f88
	UINT32                             : 28;  // 27:0    0
	UINT32 ack_interrupt_event_arm     : 1;   // 28      28
	UINT32 ipc_interrupt_event_arm     : 1;   // 29      29
	UINT32 edma_intterupt_event_arm    : 1;   // 30      30
	UINT32 wdg_interrupt_event         : 1;   // 31      31
} EXT_INTERRUPT_EVENT_T;

typedef struct { // 0x1f8c
	UINT32                             : 28;  // 27:0    0
	UINT32 ack_interrupt_enable_arm    : 1;   // 28      28
	UINT32 ipc_interrupt_enable_arm    : 1;   // 29      29
	UINT32 edma_intterupt_enable_arm   : 1;   // 30      30
	UINT32 wdg_interrupt_enable        : 1;   // 31      31
} EXT_INTERRUPT_ENABLE_T;

typedef struct { // 0x1f90
	UINT32                             : 28;  // 27:0    0
	UINT32 ack_interrupt_status_arm    : 1;   // 28      28
	UINT32 ipc_interrupt_status_arm    : 1;   // 29      29
	UINT32 edma_intterupt_status_arm   : 1;   // 30      30
	UINT32 wdg_interrupt_status        : 1;   // 31      31
} EXT_INTERRUPT_STATUS_T;

typedef struct { // 0x1f94
	UINT32                             : 28;  // 27:0    0
	UINT32 ack_interrupt_clear_arm     : 1;   // 28      28
	UINT32 ipc_interrupt_clear_arm     : 1;   // 29      29
	UINT32 edma_intterupt_clear_arm    : 1;   // 30      30
	UINT32 wdg_interrupt_clear         : 1;   // 31      31
} EXT_INTERRUPT_CLEAR_T;

typedef struct { // 0x1f98
	UINT32                             : 29;  // 28:0    0
	UINT32 ipc_interrupt_event_108     : 1;   // 29      29
	UINT32                             : 1;   // 30      30
	UINT32 edma_interrupt_event_108    : 1;   // 31      31
} INT_INTERRUPT_EVENT_T;

typedef struct { // 0x1f9c
	UINT32                             : 29;  // 28:0    0
	UINT32 ipc_intterupt_enalbe_108    : 1;   // 29      29
	UINT32                             : 1;   // 30      30
	UINT32 edma_interrupt_enable_108   : 1;   // 31      31
} INT_INTERRUPT_ENABLE_T;

typedef struct { // 0x1fa0
	UINT32                             : 29;  // 28:0    0
	UINT32 ipc_interrupt_status_108    : 1;   // 29      29
	UINT32                             : 1;   // 30      30
	UINT32 edma_interrupt_status_108   : 1;   // 31      31
} INT_INTERRUPT_STATUS_T;

typedef struct { // 0x1fa4
	UINT32                             : 29;  // 28:0    0
	UINT32 ipc_interrupt_clear_108     : 1;   // 29      29
	UINT32                             : 1;   // 30      30
	UINT32 edma_interrupt_clear_108    : 1;   // 31      31
} INT_INTERRUPT_CLEAR_T;

typedef struct { // 0x1fa8
	UINT32 boot_img_offset             : 32;  // 31:0    0
} SROM_BOOT_OFFSET_T;

typedef struct { // 0x1fac
	UINT32 sram2_silk_port_sel         : 3;   // 2:0     0
	UINT32                             : 1;   // 3       3
	UINT32 sram1_silk_port_sel         : 3;   // 6:4     4
	UINT32                             : 1;   // 7       7
	UINT32 sram0_silk_port_sel         : 3;   // 10:8    8
	UINT32                             : 1;   // 11      11
	UINT32 edma_silk_port_sel          : 3;   // 14:12   12
	UINT32                             : 17;  // 31:15   15
} SILK_PORT_SEL_T;

typedef struct { // 0x1fb0
	UINT32 pdebugstatus                : 8;   // 7:0     0
	UINT32                             : 24;  // 31:8    8
} PDEBUGSTATUS_T;

typedef struct { // 0x1fb4
	UINT32 pdebugdata                  : 32;  // 31:0    0
} PDEBUGDATA_T;

typedef struct { // 0x1fb8
	UINT32 pdebugpc                    : 32;  // 31:0    0
} PDEBUGPC_T;

typedef struct { // 0x1fbc
	UINT32 axi_user_id                 : 3;   // 2:0     0
	UINT32                             : 29;  // 31:3    3
} AXI_USER_ID_T;

typedef struct { // 0x1fdc
	UINT32 frc_mcu_ver                 : 32;  // 31:0    0
} SPP_MCU_VERSION_T;

typedef struct { // 0x1fe0
	UINT32 txlockedid                  : 32;  // 31:0    0
} TXLOCKEDID_T;

typedef struct { // 0x1fe4
	UINT32 txsrcaddr                   : 32;  // 31:0    0
} TXSRCADDR_T;

typedef struct { // 0x1fe8
	UINT32 txdstaddr                   : 32;  // 31:0    0
} TXDSTADDR_T;

typedef struct { // 0x1fec
	UINT32 txnxtaddr                   : 32;  // 31:0    0
} TXNXTADDR_T;

typedef struct { // 0x1ff0
	UINT32                             : 32;  // 31:0    0
} TXLENGTH_T;

typedef struct { // 0x1ff4
	UINT32 txstart                     : 1;   // 0       0
	UINT32 txdir                       : 3;   // 3:1     1
	UINT32 txnxtdir                    : 1;   // 4       4
	UINT32                             : 3;   // 7:5     5
	UINT32 txdone                      : 1;   // 8       8
	UINT32 txlocked                    : 1;   // 9       9
	UINT32 txswitchen                  : 1;   // 10      10
	UINT32                             : 1;   // 11      11
	UINT32 txatomicsize                : 2;   // 13:12   12
	UINT32                             : 18;  // 31:14   14
} EDMA_CONTROL_REGISTER_T;

typedef struct { // 0x1ff8
	UINT32 patternaddr                 : 32;  // 31:0    0
} PATTERNADDR_T;

typedef struct { // 0x1ffc
	UINT32                             : 32;  // 31:0    0
} PATTERNDATA_T;

typedef struct { // 0x8000
	UINT32 command_status              : 1;   // 0       0
	UINT32                             : 15;  // 15:1    1
	UINT32 processing_line             : 13;  // 28:16   16
	UINT32                             : 3;   // 31:29   29
} GFX_STATUS0_T;

typedef struct { // 0x8010
	UINT32 interrupt_generation_mode   : 2;   // 1:0     0
	UINT32                             : 14;  // 15:2    2
	UINT32 command_number              : 8;   // 23:16   16
	UINT32                             : 8;   // 31:24   24
} GFX_INTR_CTRL_T;

typedef struct { // 0x8014
	UINT32 interrupt_clear             : 1;   // 0       0
	UINT32                             : 31;  // 31:1    1
} GFX_INTR_CLEAR_T;

typedef struct {
	IPC_108M_SET_REG_T          ipc_108m_set_reg;            // 0x1f80
	IPC_108M_PRID_T             ipc_108m_prid;               // 0x1f84
	EXT_INTERRUPT_EVENT_T       ext_interrupt_event;         // 0x1f88
	EXT_INTERRUPT_ENABLE_T      ext_interrupt_enable;        // 0x1f8c
	EXT_INTERRUPT_STATUS_T      ext_interrupt_status;        // 0x1f90
	EXT_INTERRUPT_CLEAR_T       ext_interrupt_clear;         // 0x1f94
	INT_INTERRUPT_EVENT_T       int_interrupt_event;         // 0x1f98
	INT_INTERRUPT_ENABLE_T      int_interrupt_enable;        // 0x1f9c
	INT_INTERRUPT_STATUS_T      int_interrupt_status;        // 0x1fa0
	INT_INTERRUPT_CLEAR_T       int_interrupt_clear;         // 0x1fa4
	SROM_BOOT_OFFSET_T          srom_boot_offset;            // 0x1fa8
	SILK_PORT_SEL_T             silk_port_sel;               // 0x1fac
	PDEBUGSTATUS_T              pdebugstatus;                // 0x1fb0
	PDEBUGDATA_T                pdebugdata;                  // 0x1fb4
	PDEBUGPC_T                  pdebugpc;                    // 0x1fb8
	AXI_USER_ID_T               axi_user_id;                 // 0x1fbc
	UINT32                      reserved1[7];                // 0x1fc0~0x1fd8
	SPP_MCU_VERSION_T           spp_mcu_version;             // 0x1fdc
} SPP_MCU_REG_T;
/* 17 regs, 17 types */

typedef struct {
	TXLOCKEDID_T                txlockedid;                  // 0x1fe0
	TXSRCADDR_T                 txsrcaddr;                   // 0x1fe4
	TXDSTADDR_T                 txdstaddr;                   // 0x1fe8
	TXNXTADDR_T                 txnxtaddr;                   // 0x1fec
	TXLENGTH_T                  txlength;                    // 0x1ff0
	EDMA_CONTROL_REGISTER_T     edma_control_register;       // 0x1ff4
	PATTERNADDR_T               patternaddr;                 // 0x1ff8
	PATTERNDATA_T               patterndata;                 // 0x1ffc
} eDMA_REG_T;
/* 8 regs, 8 types */

/* 25 regs, 25 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegSPP_MCU : real register of SPP_MCU.
 * gpRegSPP_MCU     : shadow register.
 *
 * @def SPP_MCU_RdFL: Read  FLushing : Shadow <- Real.
 * @def SPP_MCU_WrFL: Write FLushing : Shadow -> Real.
 * @def SPP_MCU_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def SPP_MCU_Wr  : Write whole register(UINT32) from Shadow register.
 * @def SPP_MCU_Rd01 ~ SPP_MCU_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def SPP_MCU_Wr01 ~ SPP_MCU_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define SPP_MCU_RdFL(_r)			((gpRegSPP_MCU->_r)=(gpRealRegSPP_MCU->_r))
#define SPP_MCU_WrFL(_r)			((gpRealRegSPP_MCU->_r)=(gpRegSPP_MCU->_r))

#define SPP_MCU_Rd(_r)			*((UINT32*)(&(gpRegSPP_MCU->_r)))
#define SPP_MCU_Wr(_r,_v)			((SPP_MCU_Rd(_r))=((UINT32)(_v)))

#define SPP_MCU_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
								} while(0)

#define SPP_MCU_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
								} while(0)

#define SPP_MCU_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
								} while(0)

#define SPP_MCU_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
								} while(0)

#define SPP_MCU_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
								} while(0)

#define SPP_MCU_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
								} while(0)

#define SPP_MCU_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
								} while(0)

#define SPP_MCU_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
								} while(0)

#define SPP_MCU_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
								} while(0)

#define SPP_MCU_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
									(_v10) = (gpRegSPP_MCU->_r._f10);				\
								} while(0)

#define SPP_MCU_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
									(_v10) = (gpRegSPP_MCU->_r._f10);				\
									(_v11) = (gpRegSPP_MCU->_r._f11);				\
								} while(0)

#define SPP_MCU_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
									(_v10) = (gpRegSPP_MCU->_r._f10);				\
									(_v11) = (gpRegSPP_MCU->_r._f11);				\
									(_v12) = (gpRegSPP_MCU->_r._f12);				\
								} while(0)

#define SPP_MCU_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
									(_v10) = (gpRegSPP_MCU->_r._f10);				\
									(_v11) = (gpRegSPP_MCU->_r._f11);				\
									(_v12) = (gpRegSPP_MCU->_r._f12);				\
									(_v13) = (gpRegSPP_MCU->_r._f13);				\
								} while(0)

#define SPP_MCU_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
									(_v10) = (gpRegSPP_MCU->_r._f10);				\
									(_v11) = (gpRegSPP_MCU->_r._f11);				\
									(_v12) = (gpRegSPP_MCU->_r._f12);				\
									(_v13) = (gpRegSPP_MCU->_r._f13);				\
									(_v14) = (gpRegSPP_MCU->_r._f14);				\
								} while(0)

#define SPP_MCU_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
									(_v10) = (gpRegSPP_MCU->_r._f10);				\
									(_v11) = (gpRegSPP_MCU->_r._f11);				\
									(_v12) = (gpRegSPP_MCU->_r._f12);				\
									(_v13) = (gpRegSPP_MCU->_r._f13);				\
									(_v14) = (gpRegSPP_MCU->_r._f14);				\
									(_v15) = (gpRegSPP_MCU->_r._f15);				\
								} while(0)

#define SPP_MCU_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegSPP_MCU->_r._f01);				\
									(_v02) = (gpRegSPP_MCU->_r._f02);				\
									(_v03) = (gpRegSPP_MCU->_r._f03);				\
									(_v04) = (gpRegSPP_MCU->_r._f04);				\
									(_v05) = (gpRegSPP_MCU->_r._f05);				\
									(_v06) = (gpRegSPP_MCU->_r._f06);				\
									(_v07) = (gpRegSPP_MCU->_r._f07);				\
									(_v08) = (gpRegSPP_MCU->_r._f08);				\
									(_v09) = (gpRegSPP_MCU->_r._f09);				\
									(_v10) = (gpRegSPP_MCU->_r._f10);				\
									(_v11) = (gpRegSPP_MCU->_r._f11);				\
									(_v12) = (gpRegSPP_MCU->_r._f12);				\
									(_v13) = (gpRegSPP_MCU->_r._f13);				\
									(_v14) = (gpRegSPP_MCU->_r._f14);				\
									(_v15) = (gpRegSPP_MCU->_r._f15);				\
									(_v16) = (gpRegSPP_MCU->_r._f16);				\
								} while(0)


#define SPP_MCU_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
								} while(0)

#define SPP_MCU_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
								} while(0)

#define SPP_MCU_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
								} while(0)

#define SPP_MCU_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
								} while(0)

#define SPP_MCU_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
								} while(0)

#define SPP_MCU_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
								} while(0)

#define SPP_MCU_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
								} while(0)

#define SPP_MCU_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
								} while(0)

#define SPP_MCU_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
								} while(0)

#define SPP_MCU_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
									(gpRegSPP_MCU->_r._f10) = (_v10);				\
								} while(0)

#define SPP_MCU_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
									(gpRegSPP_MCU->_r._f10) = (_v10);				\
									(gpRegSPP_MCU->_r._f11) = (_v11);				\
								} while(0)

#define SPP_MCU_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
									(gpRegSPP_MCU->_r._f10) = (_v10);				\
									(gpRegSPP_MCU->_r._f11) = (_v11);				\
									(gpRegSPP_MCU->_r._f12) = (_v12);				\
								} while(0)

#define SPP_MCU_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
									(gpRegSPP_MCU->_r._f10) = (_v10);				\
									(gpRegSPP_MCU->_r._f11) = (_v11);				\
									(gpRegSPP_MCU->_r._f12) = (_v12);				\
									(gpRegSPP_MCU->_r._f13) = (_v13);				\
								} while(0)

#define SPP_MCU_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
									(gpRegSPP_MCU->_r._f10) = (_v10);				\
									(gpRegSPP_MCU->_r._f11) = (_v11);				\
									(gpRegSPP_MCU->_r._f12) = (_v12);				\
									(gpRegSPP_MCU->_r._f13) = (_v13);				\
									(gpRegSPP_MCU->_r._f14) = (_v14);				\
								} while(0)

#define SPP_MCU_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
									(gpRegSPP_MCU->_r._f10) = (_v10);				\
									(gpRegSPP_MCU->_r._f11) = (_v11);				\
									(gpRegSPP_MCU->_r._f12) = (_v12);				\
									(gpRegSPP_MCU->_r._f13) = (_v13);				\
									(gpRegSPP_MCU->_r._f14) = (_v14);				\
									(gpRegSPP_MCU->_r._f15) = (_v15);				\
								} while(0)

#define SPP_MCU_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegSPP_MCU->_r._f01) = (_v01);				\
									(gpRegSPP_MCU->_r._f02) = (_v02);				\
									(gpRegSPP_MCU->_r._f03) = (_v03);				\
									(gpRegSPP_MCU->_r._f04) = (_v04);				\
									(gpRegSPP_MCU->_r._f05) = (_v05);				\
									(gpRegSPP_MCU->_r._f06) = (_v06);				\
									(gpRegSPP_MCU->_r._f07) = (_v07);				\
									(gpRegSPP_MCU->_r._f08) = (_v08);				\
									(gpRegSPP_MCU->_r._f09) = (_v09);				\
									(gpRegSPP_MCU->_r._f10) = (_v10);				\
									(gpRegSPP_MCU->_r._f11) = (_v11);				\
									(gpRegSPP_MCU->_r._f12) = (_v12);				\
									(gpRegSPP_MCU->_r._f13) = (_v13);				\
									(gpRegSPP_MCU->_r._f14) = (_v14);				\
									(gpRegSPP_MCU->_r._f15) = (_v15);				\
									(gpRegSPP_MCU->_r._f16) = (_v16);				\
								} while(0)

/* Indexed Register Access.
 *
 * There is in-direct field specified by 'index' field within a register.
 * Normally a register has only one meaning for a 'field_name', but indexed register
 * can hold several data for a 'field_name' specifed by 'index' field of indexed register.
 * When writing an 3rd data for given 'field_name' register, you need to set 'rw' = 0, 'index' = 2,
 * and 'load' = 0.
 *
 * ASSUMPTION
 * For Writing indexed register load bit
 *
 * parameter list
 * _r     : name of register
 * _lname : name of load  bit field	: shall be 0 after macro executed.
 * _rwname: name of rw    bit field : shall be 0 after SPP_MCU_Wind(), 1 for SPP_MCU_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * SPP_MCU_Rind : General indexed register Read.(
 * SPP_MCU_Wind : General indexed register Read.
 *
 * SPP_MCU_Ridx : For 'index', 'rw', 'load' field name
 * SPP_MCU_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define SPP_MCU_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								SPP_MCU_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								SPP_MCU_WrFL(_r);									\
								SPP_MCU_RdFL(_r);									\
								SPP_MCU_Rd01(_r,_fname,_fval);						\
							} while (0)

#define SPP_MCU_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				SPP_MCU_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define SPP_MCU_Ridx(_r, _ival, _fname, _fval)	SPP_MCU_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define SPP_MCU_Widx(_r, _ival, _fname, _fval)	SPP_MCU_Wind(_r,load,rw,index,_ival,_fname,_fval)

/*
 * @{
 * Naming for register pointer.
 * gpRealRegeDMA : real register of eDMA.
 * gpRegeDMA     : shadow register.
 *
 * @def eDMA_RdFL: Read  FLushing : Shadow <- Real.
 * @def eDMA_WrFL: Write FLushing : Shadow -> Real.
 * @def eDMA_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def eDMA_Wr  : Write whole register(UINT32) from Shadow register.
 * @def eDMA_Rd01 ~ eDMA_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def eDMA_Wr01 ~ eDMA_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define eDMA_RdFL(_r)			((gpRegeDMA->_r)=(gpRealRegeDMA->_r))
#define eDMA_WrFL(_r)			((gpRealRegeDMA->_r)=(gpRegeDMA->_r))

#define eDMA_Rd(_r)			*((UINT32*)(&(gpRegeDMA->_r)))
#define eDMA_Wr(_r,_v)			((eDMA_Rd(_r))=((UINT32)(_v)))

#define eDMA_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
								} while(0)

#define eDMA_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
								} while(0)

#define eDMA_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
								} while(0)

#define eDMA_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
								} while(0)

#define eDMA_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
								} while(0)

#define eDMA_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
								} while(0)

#define eDMA_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
								} while(0)

#define eDMA_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
								} while(0)

#define eDMA_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
								} while(0)

#define eDMA_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
									(_v10) = (gpRegeDMA->_r._f10);				\
								} while(0)

#define eDMA_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
									(_v10) = (gpRegeDMA->_r._f10);				\
									(_v11) = (gpRegeDMA->_r._f11);				\
								} while(0)

#define eDMA_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
									(_v10) = (gpRegeDMA->_r._f10);				\
									(_v11) = (gpRegeDMA->_r._f11);				\
									(_v12) = (gpRegeDMA->_r._f12);				\
								} while(0)

#define eDMA_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
									(_v10) = (gpRegeDMA->_r._f10);				\
									(_v11) = (gpRegeDMA->_r._f11);				\
									(_v12) = (gpRegeDMA->_r._f12);				\
									(_v13) = (gpRegeDMA->_r._f13);				\
								} while(0)

#define eDMA_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
									(_v10) = (gpRegeDMA->_r._f10);				\
									(_v11) = (gpRegeDMA->_r._f11);				\
									(_v12) = (gpRegeDMA->_r._f12);				\
									(_v13) = (gpRegeDMA->_r._f13);				\
									(_v14) = (gpRegeDMA->_r._f14);				\
								} while(0)

#define eDMA_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
									(_v10) = (gpRegeDMA->_r._f10);				\
									(_v11) = (gpRegeDMA->_r._f11);				\
									(_v12) = (gpRegeDMA->_r._f12);				\
									(_v13) = (gpRegeDMA->_r._f13);				\
									(_v14) = (gpRegeDMA->_r._f14);				\
									(_v15) = (gpRegeDMA->_r._f15);				\
								} while(0)

#define eDMA_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegeDMA->_r._f01);				\
									(_v02) = (gpRegeDMA->_r._f02);				\
									(_v03) = (gpRegeDMA->_r._f03);				\
									(_v04) = (gpRegeDMA->_r._f04);				\
									(_v05) = (gpRegeDMA->_r._f05);				\
									(_v06) = (gpRegeDMA->_r._f06);				\
									(_v07) = (gpRegeDMA->_r._f07);				\
									(_v08) = (gpRegeDMA->_r._f08);				\
									(_v09) = (gpRegeDMA->_r._f09);				\
									(_v10) = (gpRegeDMA->_r._f10);				\
									(_v11) = (gpRegeDMA->_r._f11);				\
									(_v12) = (gpRegeDMA->_r._f12);				\
									(_v13) = (gpRegeDMA->_r._f13);				\
									(_v14) = (gpRegeDMA->_r._f14);				\
									(_v15) = (gpRegeDMA->_r._f15);				\
									(_v16) = (gpRegeDMA->_r._f16);				\
								} while(0)


#define eDMA_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
								} while(0)

#define eDMA_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
								} while(0)

#define eDMA_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
								} while(0)

#define eDMA_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
								} while(0)

#define eDMA_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
								} while(0)

#define eDMA_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
								} while(0)

#define eDMA_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
								} while(0)

#define eDMA_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
								} while(0)

#define eDMA_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
								} while(0)

#define eDMA_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
									(gpRegeDMA->_r._f10) = (_v10);				\
								} while(0)

#define eDMA_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
									(gpRegeDMA->_r._f10) = (_v10);				\
									(gpRegeDMA->_r._f11) = (_v11);				\
								} while(0)

#define eDMA_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
									(gpRegeDMA->_r._f10) = (_v10);				\
									(gpRegeDMA->_r._f11) = (_v11);				\
									(gpRegeDMA->_r._f12) = (_v12);				\
								} while(0)

#define eDMA_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
									(gpRegeDMA->_r._f10) = (_v10);				\
									(gpRegeDMA->_r._f11) = (_v11);				\
									(gpRegeDMA->_r._f12) = (_v12);				\
									(gpRegeDMA->_r._f13) = (_v13);				\
								} while(0)

#define eDMA_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
									(gpRegeDMA->_r._f10) = (_v10);				\
									(gpRegeDMA->_r._f11) = (_v11);				\
									(gpRegeDMA->_r._f12) = (_v12);				\
									(gpRegeDMA->_r._f13) = (_v13);				\
									(gpRegeDMA->_r._f14) = (_v14);				\
								} while(0)

#define eDMA_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
									(gpRegeDMA->_r._f10) = (_v10);				\
									(gpRegeDMA->_r._f11) = (_v11);				\
									(gpRegeDMA->_r._f12) = (_v12);				\
									(gpRegeDMA->_r._f13) = (_v13);				\
									(gpRegeDMA->_r._f14) = (_v14);				\
									(gpRegeDMA->_r._f15) = (_v15);				\
								} while(0)

#define eDMA_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegeDMA->_r._f01) = (_v01);				\
									(gpRegeDMA->_r._f02) = (_v02);				\
									(gpRegeDMA->_r._f03) = (_v03);				\
									(gpRegeDMA->_r._f04) = (_v04);				\
									(gpRegeDMA->_r._f05) = (_v05);				\
									(gpRegeDMA->_r._f06) = (_v06);				\
									(gpRegeDMA->_r._f07) = (_v07);				\
									(gpRegeDMA->_r._f08) = (_v08);				\
									(gpRegeDMA->_r._f09) = (_v09);				\
									(gpRegeDMA->_r._f10) = (_v10);				\
									(gpRegeDMA->_r._f11) = (_v11);				\
									(gpRegeDMA->_r._f12) = (_v12);				\
									(gpRegeDMA->_r._f13) = (_v13);				\
									(gpRegeDMA->_r._f14) = (_v14);				\
									(gpRegeDMA->_r._f15) = (_v15);				\
									(gpRegeDMA->_r._f16) = (_v16);				\
								} while(0)

/* Indexed Register Access.
 *
 * There is in-direct field specified by 'index' field within a register.
 * Normally a register has only one meaning for a 'field_name', but indexed register
 * can hold several data for a 'field_name' specifed by 'index' field of indexed register.
 * When writing an 3rd data for given 'field_name' register, you need to set 'rw' = 0, 'index' = 2,
 * and 'load' = 0.
 *
 * ASSUMPTION
 * For Writing indexed register load bit
 *
 * parameter list
 * _r     : name of register
 * _lname : name of load  bit field	: shall be 0 after macro executed.
 * _rwname: name of rw    bit field : shall be 0 after eDMA_Wind(), 1 for eDMA_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * eDMA_Rind : General indexed register Read.(
 * eDMA_Wind : General indexed register Read.
 *
 * eDMA_Ridx : For 'index', 'rw', 'load' field name
 * eDMA_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define eDMA_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								eDMA_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								eDMA_WrFL(_r);									\
								eDMA_RdFL(_r);									\
								eDMA_Rd01(_r,_fname,_fval);						\
							} while (0)

#define eDMA_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				eDMA_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define eDMA_Ridx(_r, _ival, _fname, _fval)	eDMA_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define eDMA_Widx(_r, _ival, _fname, _fval)	eDMA_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8-DE_SPP_MCU_RegFile_091229.csv' 20100311 00:54:16   대한민국 표준시 by getregs v2.3 */
