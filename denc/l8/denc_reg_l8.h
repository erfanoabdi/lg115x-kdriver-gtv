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
	0x1900 ttx_ver_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ttx_ovs                         : 8,	//  0: 7
	ttx_ove                         : 8,	//  8:15
	ttx_evs                         : 8,	// 16:23
	ttx_eve                         : 8;	// 24:31
} TTX_VER_CTRL0;

/*-----------------------------------------------------------------------------
	0x1904 ttx_ver_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ttx_ovs8                        : 1,	//     0
	ttx_ove8                        : 1,	//     1
	ttx_evs8                        : 1,	//     2
	ttx_eve8                        : 1,	//     3
	ttx_iter_cnt                    : 3,	//  4: 6
	ttx_adj                         : 1,	//     7
	ttx_err                         : 4,	//  8:11
	                                : 2,	// 12:13 reserved
	ttx_line                        :18;	// 14:31
} TTX_VER_CTRL1;

/*-----------------------------------------------------------------------------
	0x1908 wss_data ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ttx_hs                          :11,	//  0:10
	comp_c_delay                    : 5,	// 11:15
	wss_data                        :14,	// 16:29
	comp_y_lpf                      : 1;	//    30
} WSS_DATA;

/*-----------------------------------------------------------------------------
	0x190c cgms_data ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cgms_enc_data                   :20,	//  0:19
	secam_burst_blank_end           : 8,	// 20:27
	secam_preem_scaling             : 3;	// 28:30
} CGMS_DATA;

/*-----------------------------------------------------------------------------
	0x1910 cve_config ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	standard                        : 3,	//  0: 2
	enc_en                          : 1,	//     3
	color_bar_en                    : 1,	//     4
	c_filter                        : 2,	//  5: 6
	color_on                        : 1,	//     7
	y_filter                        : 2,	//  8: 9
	y_delay                         : 2,	// 10:11
	ttx_sys                         : 3,	// 12:14
	video_interface_sel             : 1,	//    15
	burst_amp                       : 8,	// 16:23
	secam_y_delay                   : 5,	// 24:28
	offset                          : 2,	// 29:30
	invert                          : 1;	//    31
} CVE_CONFIG;

/*-----------------------------------------------------------------------------
	0x1914 phase_incr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	phase_incr                      ;   	// 31: 0
} PHASE_INCR;

/*-----------------------------------------------------------------------------
	0x1918 denc_vbi_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	phase_offset                    : 8,	//  0: 7
	secam_subcarrier_scaling        : 6,	//  8:13
	secam_c_gain                    : 2,	// 14:15
	cc1_en                          : 1,	//    16
	cc2_en                          : 1,	//    17
	cc1_full                        : 1,	//    18
	cc2_full                        : 1,	//    19
	cgms_en                         : 1,	//    20
	wss_en                          : 1,	//    21
	ttx_en                          : 1,	//    22
	fm_reset_on                     : 1,	//    23
	fm_reset_initial                : 1,	//    24
	burst_phase_reset_strt          : 5,	// 25:29
	comp_c_gain                     : 2;	// 30:31
} DENC_VBI_CTRL;

/*-----------------------------------------------------------------------------
	0x191c cc_data ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cc_data_f1                      :16,	//  0:15
	cc_data_f2                      :16;	// 16:31
} CC_DATA;

/*-----------------------------------------------------------------------------
	0x1920 mv_n0_3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mv_n0                           : 6,	//  0: 5
	                                : 2,	//  6: 7 reserved
	mv_n2                           : 6,	//  8:13
	                                : 2,	// 14:15 reserved
	mv_n1                           : 6,	// 16:21
	                                : 2,	// 22:23 reserved
	mv_n3                           : 6;	// 24:29
} MV_N0_3;

/*-----------------------------------------------------------------------------
	0x1924 mv_n4_9 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mv_n4                           : 6,	//  0: 5
	mv_n22                          : 1,	//     6
	                                : 1,	//     7 reserved
	mv_n8                           : 6,	//  8:13
	                                : 2,	// 14:15 reserved
	mv_n7                           : 2,	// 16:17
	mv_n6                           : 3,	// 18:20
	mv_n5                           : 3,	// 21:23
	mv_n9                           : 6;	// 24:29
} MV_N4_9;

/*-----------------------------------------------------------------------------
	0x1928 mv_n10_11 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mv_n10                          : 6,	//  0: 5
	mv_n21                          :10,	//  6:15
	mv_n11                          :15;	// 16:30
} MV_N10_11;

/*-----------------------------------------------------------------------------
	0x192c mv_n12_14 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mv_n12                          :15,	//  0:14
	                                : 1,	//    15 reserved
	mv_n14                          : 8,	// 16:23
	mv_n13                          : 8;	// 24:31
} MV_N12_14;

/*-----------------------------------------------------------------------------
	0x1930 mv_n15_20 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mv_n15                          : 8,	//  0: 7
	                                : 8,	//  8:15 reserved
	mv_n20                          : 3,	// 16:18
	mv_n16                          : 1,	//    19
	mv_n19                          : 4,	// 20:23
	mv_n18                          : 4,	// 24:27
	mv_n17                          : 4;	// 28:31
} MV_N15_20;

/*-----------------------------------------------------------------------------
	0x1934 vps_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	index                           : 3,	//  0: 2
	                                : 1,	//     3 reserved
	lpf_bypass                      : 1,	//     4
	vbi_odd                         : 1,	//     5
	vbi_even                        : 1,	//     6
	vps_enable                      : 1,	//     7
	vps_wr_done                     : 1,	//     8
	                                : 3,	//  9:11 reserved
	wss_odd                         : 1,	//     12
	wss_even                        : 1,	//     13
	                                : 2,	// 14:15 reserved
	vps_data                        :16;	// 16:31
} VPS_CTRL;

/*-----------------------------------------------------------------------------
	0x1938 ttx_data ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ttx_data                        ;   	// 31: 0
} TTX_DATA;

/*-----------------------------------------------------------------------------
	0x193c ttx_load ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ttx_load                        : 1,	//     0
	                                : 3,	//  1: 3 reserved
	mv_sel                          : 1,	//     4
	                                : 3,	//  5: 7 reserved
	ttx_write_addr                  : 8;	//  8:15
} TTX_LOAD;

/*-----------------------------------------------------------------------------
	0x1980 field_mode ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	interlaced_mode                 : 1,	//     0
	                                : 3,	//  1: 3 reserved
	top_field                       : 1;	//     4
} FIELD_MODE;

/*-----------------------------------------------------------------------------
	0x1984 a_top_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_status                        :10,	//  0: 9
	                                : 6,	// 10:15 reserved
	c_status                        :10;	// 16:25
} A_TOP_STATUS;

typedef struct {
	TTX_VER_CTRL0                   	ttx_ver_ctrl0                   ;	// 0x1900 : ''
	TTX_VER_CTRL1                   	ttx_ver_ctrl1                   ;	// 0x1904 : ''
	WSS_DATA                        	wss_data                        ;	// 0x1908 : ''
	CGMS_DATA                       	cgms_data                       ;	// 0x190c : ''
	CVE_CONFIG                      	cve_config                      ;	// 0x1910 : ''
	PHASE_INCR                      	phase_incr                      ;	// 0x1914 : ''
	DENC_VBI_CTRL                   	denc_vbi_ctrl                   ;	// 0x1918 : ''
	CC_DATA                         	cc_data                         ;	// 0x191c : ''
	MV_N0_3                         	mv_n0_3                         ;	// 0x1920 : ''
	MV_N4_9                         	mv_n4_9                         ;	// 0x1924 : ''
	MV_N10_11                       	mv_n10_11                       ;	// 0x1928 : ''
	MV_N12_14                       	mv_n12_14                       ;	// 0x192c : ''
	MV_N15_20                       	mv_n15_20                       ;	// 0x1930 : ''
	VPS_CTRL                        	vps_ctrl                        ;	// 0x1934 : ''
	TTX_DATA                        	ttx_data                        ;	// 0x1938 : ''
	TTX_LOAD                        	ttx_load                        ;	// 0x193c : ''
	UINT32                          	                 __rsvd_00[  16];	// 0x1940 ~ 0x197c
	FIELD_MODE                      	field_mode                      ;	// 0x1980 : ''
	A_TOP_STATUS                    	a_top_status                    ;	// 0x1984 : ''
} ATOP_REG_T;
/* 18 regs, 18 types */

/* 18 regs, 18 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegATOP : real register of ATOP.
 * gpRegATOP     : shadow register.
 *
 * @def ATOP_RdFL: Read  FLushing : Shadow <- Real.
 * @def ATOP_WrFL: Write FLushing : Shadow -> Real.
 * @def ATOP_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def ATOP_Wr  : Write whole register(UINT32) from Shadow register.
 * @def ATOP_Rd01 ~ ATOP_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def ATOP_Wr01 ~ ATOP_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define ATOP_RdFL(_r)			((gpRegATOP->_r)=(gpRealRegATOP->_r))
#define ATOP_WrFL(_r)			((gpRealRegATOP->_r)=(gpRegATOP->_r))

#define ATOP_Rd(_r)				*((UINT32*)(&(gpRegATOP->_r)))
#define ATOP_Wr(_r,_v)			((ATOP_Rd(_r))=((UINT32)(_v)))

#define ATOP_FLWr(_r,_v)		do {ATOP_Wr(_r,_v); ATOP_WrFL(_r); } while(0)

#define ATOP_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
								} while(0)

#define ATOP_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
								} while(0)

#define ATOP_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
								} while(0)

#define ATOP_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
								} while(0)

#define ATOP_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
								} while(0)

#define ATOP_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
								} while(0)

#define ATOP_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
								} while(0)

#define ATOP_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
								} while(0)

#define ATOP_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
								} while(0)

#define ATOP_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
									(_v10) = (gpRegATOP->_r._f10);				\
								} while(0)

#define ATOP_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
									(_v10) = (gpRegATOP->_r._f10);				\
									(_v11) = (gpRegATOP->_r._f11);				\
								} while(0)

#define ATOP_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
									(_v10) = (gpRegATOP->_r._f10);				\
									(_v11) = (gpRegATOP->_r._f11);				\
									(_v12) = (gpRegATOP->_r._f12);				\
								} while(0)

#define ATOP_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
									(_v10) = (gpRegATOP->_r._f10);				\
									(_v11) = (gpRegATOP->_r._f11);				\
									(_v12) = (gpRegATOP->_r._f12);				\
									(_v13) = (gpRegATOP->_r._f13);				\
								} while(0)

#define ATOP_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
									(_v10) = (gpRegATOP->_r._f10);				\
									(_v11) = (gpRegATOP->_r._f11);				\
									(_v12) = (gpRegATOP->_r._f12);				\
									(_v13) = (gpRegATOP->_r._f13);				\
									(_v14) = (gpRegATOP->_r._f14);				\
								} while(0)

#define ATOP_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
									(_v10) = (gpRegATOP->_r._f10);				\
									(_v11) = (gpRegATOP->_r._f11);				\
									(_v12) = (gpRegATOP->_r._f12);				\
									(_v13) = (gpRegATOP->_r._f13);				\
									(_v14) = (gpRegATOP->_r._f14);				\
									(_v15) = (gpRegATOP->_r._f15);				\
								} while(0)

#define ATOP_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegATOP->_r._f01);				\
									(_v02) = (gpRegATOP->_r._f02);				\
									(_v03) = (gpRegATOP->_r._f03);				\
									(_v04) = (gpRegATOP->_r._f04);				\
									(_v05) = (gpRegATOP->_r._f05);				\
									(_v06) = (gpRegATOP->_r._f06);				\
									(_v07) = (gpRegATOP->_r._f07);				\
									(_v08) = (gpRegATOP->_r._f08);				\
									(_v09) = (gpRegATOP->_r._f09);				\
									(_v10) = (gpRegATOP->_r._f10);				\
									(_v11) = (gpRegATOP->_r._f11);				\
									(_v12) = (gpRegATOP->_r._f12);				\
									(_v13) = (gpRegATOP->_r._f13);				\
									(_v14) = (gpRegATOP->_r._f14);				\
									(_v15) = (gpRegATOP->_r._f15);				\
									(_v16) = (gpRegATOP->_r._f16);				\
								} while(0)


#define ATOP_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
								} while(0)

#define ATOP_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
								} while(0)

#define ATOP_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
								} while(0)

#define ATOP_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
								} while(0)

#define ATOP_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
								} while(0)

#define ATOP_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
								} while(0)

#define ATOP_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
								} while(0)

#define ATOP_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
								} while(0)

#define ATOP_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
								} while(0)

#define ATOP_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
									(gpRegATOP->_r._f10) = (_v10);				\
								} while(0)

#define ATOP_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
									(gpRegATOP->_r._f10) = (_v10);				\
									(gpRegATOP->_r._f11) = (_v11);				\
								} while(0)

#define ATOP_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
									(gpRegATOP->_r._f10) = (_v10);				\
									(gpRegATOP->_r._f11) = (_v11);				\
									(gpRegATOP->_r._f12) = (_v12);				\
								} while(0)

#define ATOP_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
									(gpRegATOP->_r._f10) = (_v10);				\
									(gpRegATOP->_r._f11) = (_v11);				\
									(gpRegATOP->_r._f12) = (_v12);				\
									(gpRegATOP->_r._f13) = (_v13);				\
								} while(0)

#define ATOP_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
									(gpRegATOP->_r._f10) = (_v10);				\
									(gpRegATOP->_r._f11) = (_v11);				\
									(gpRegATOP->_r._f12) = (_v12);				\
									(gpRegATOP->_r._f13) = (_v13);				\
									(gpRegATOP->_r._f14) = (_v14);				\
								} while(0)

#define ATOP_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
									(gpRegATOP->_r._f10) = (_v10);				\
									(gpRegATOP->_r._f11) = (_v11);				\
									(gpRegATOP->_r._f12) = (_v12);				\
									(gpRegATOP->_r._f13) = (_v13);				\
									(gpRegATOP->_r._f14) = (_v14);				\
									(gpRegATOP->_r._f15) = (_v15);				\
								} while(0)

#define ATOP_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegATOP->_r._f01) = (_v01);				\
									(gpRegATOP->_r._f02) = (_v02);				\
									(gpRegATOP->_r._f03) = (_v03);				\
									(gpRegATOP->_r._f04) = (_v04);				\
									(gpRegATOP->_r._f05) = (_v05);				\
									(gpRegATOP->_r._f06) = (_v06);				\
									(gpRegATOP->_r._f07) = (_v07);				\
									(gpRegATOP->_r._f08) = (_v08);				\
									(gpRegATOP->_r._f09) = (_v09);				\
									(gpRegATOP->_r._f10) = (_v10);				\
									(gpRegATOP->_r._f11) = (_v11);				\
									(gpRegATOP->_r._f12) = (_v12);				\
									(gpRegATOP->_r._f13) = (_v13);				\
									(gpRegATOP->_r._f14) = (_v14);				\
									(gpRegATOP->_r._f15) = (_v15);				\
									(gpRegATOP->_r._f16) = (_v16);				\
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
 * _rwname: name of rw    bit field : shall be 0 after ATOP_Wind(), 1 for ATOP_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * ATOP_Rind : General indexed register Read.(
 * ATOP_Wind : General indexed register Read.
 *
 * ATOP_Ridx : For 'index', 'rw', 'load' field name
 * ATOP_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define ATOP_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								ATOP_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								ATOP_WrFL(_r);									\
								ATOP_RdFL(_r);									\
								ATOP_Rd01(_r,_fname,_fval);						\
							} while (0)

#define ATOP_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				ATOP_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define ATOP_Ridx(_r, _ival, _fname, _fval)	ATOP_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define ATOP_Widx(_r, _ival, _fname, _fval)	ATOP_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8-DE_ATOP_RegFile_091215.csv' 20100311 00:54:16   대한민국 표준시 by getregs v2.3 */
