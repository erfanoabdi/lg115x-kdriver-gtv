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
typedef struct {
	UINT32
	main_444                        : 1,	//     0
	                                : 3,	//  1: 3 reserved
	main_lsr_2b                     : 2,	//  4: 5
	main_cflt_mode                  : 2,	//  6: 7
	sub_444                         : 1,	//     8
	                                : 3,	//  9:11 reserved
	sub_lsr_2b                      : 2,	// 12:13
	sub_cflt_mode                   : 2;	// 14:15
} OV_CTRL0;

/*-----------------------------------------------------------------------------
	0x1104 ov_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	border_color_cr                 : 4,	//  0: 3
	border_color_cb                 : 4,	//  4: 7
	border_color_yy                 : 8,	//  8:15
	load_enable                     : 1,	//    16
	load_type                       : 1;	//    17
} OV_CTRL1;

/*-----------------------------------------------------------------------------
	0x1108 ov_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	back_color_cr                   : 4,	//  0: 3
	back_color_cb                   : 4,	//  4: 7
	back_color_yy                   : 8,	//  8:15
	dark_color_cr                   : 4,	// 16:19
	dark_color_cb                   : 4,	// 20:23
	dark_color_yy                   : 8;	// 24:31
} OV_CTRL2;

/*-----------------------------------------------------------------------------
	0x110c ov_ctrl3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	w0_blending_alpha               : 8,	//  0: 7
	w1_blending_alpha               : 8,	//  8:15
	w0_blend_en                     : 1,	//    16
	w1_blend_en                     : 1,	//    17
	w1_blend_with                   : 1,	//    18
	large_win                       : 1,	//    19
	w0_dark_en                      : 1,	//    20
	w1_dark_en                      : 1;	//    21
} OV_CTRL3;

/*-----------------------------------------------------------------------------
	0x1110 ov_moffset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	w0_hoffset                      :11,	//  0:10
	                                : 5,	// 11:15 reserved
	w0_voffset                      :12;	// 16:27
} OV_MOFFSET;

/*-----------------------------------------------------------------------------
	0x1114 ov_msize ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	w0_hsize                        :11,	//  0:10
	                                : 5,	// 11:15 reserved
	w0_vsize                        :12;	// 16:27
} OV_MSIZE;

/*-----------------------------------------------------------------------------
	0x1118 ov_soffset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	w1_hoffset                      :11,	//  0:10
	                                : 5,	// 11:15 reserved
	w1_voffset                      :11;	// 16:26
} OV_SOFFSET;

/*-----------------------------------------------------------------------------
	0x111c ov_ssize ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	w1_hsize                        :11,	//  0:10
	                                : 5,	// 11:15 reserved
	w1_vsize                        :11;	// 16:26
} OV_SSIZE;

/*-----------------------------------------------------------------------------
	0x1120 ov_border_width ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	w1_right_wid                    : 4,	//  0: 3
	w1_left_wid                     : 4,	//  4: 7
	w1_down_wid                     : 4,	//  8:11
	w1_up_wid                       : 4,	// 12:15
	w0_right_wid                    : 4,	// 16:19
	w0_left_wid                     : 4,	// 20:23
	w0_down_wid                     : 4,	// 24:27
	w0_up_wid                       : 4;	// 28:31
} OV_BORDER_WIDTH;

/*-----------------------------------------------------------------------------
	0x1124 cflt_coef0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cflt_coef_1                     :12,	//  0:11
	                                : 4,	// 12:15 reserved
	cflt_coef_0                     :12;	// 16:27
} CFLT_COEF0;

/*-----------------------------------------------------------------------------
	0x1128 cflt_coef1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cflt_coef_3                     :12,	//  0:11
	                                : 4,	// 12:15 reserved
	cflt_coef_2                     :12;	// 16:27
} CFLT_COEF1;

/*-----------------------------------------------------------------------------
	0x1130 ov_param_by_ddr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	                                : 9,	//  0: 8 reserved
	stride                          : 2,	//  9:10
	                                : 5,	// 11:15 reserved
	vsize                           : 8,	// 16:23
	                                : 6,	// 24:29 reserved
	endian_conversion               : 1,	//    30
	update_by_ddr                   : 1;	//    31
} OV_PARAM_BY_DDR;

/*-----------------------------------------------------------------------------
	0x1140 ov_intr_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	intr_enable                     : 1,	//     0
	intr_line_pos_enable            : 1,	//     1
	                                : 6,	//  2: 7 reserved
	intr_sbuf_c_empty_en            : 1,	//     8
	intr_sbuf_y_empty_en            : 1,	//     9
	intr_mbuf_c_empty_en            : 1,	//    10
	intr_mbuf_y_empty_en            : 1,	//    11
	intr_pdb_end_en                 : 1,	//    12
	                                : 3,	// 13:15 reserved
	line_position                   :11;	// 16:26
} OV_INTR_CTRL;

/*-----------------------------------------------------------------------------
	0x1144 ov_intr_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sbuf_c_empty                    : 1,	//     0
	sbuf_y_empty                    : 1,	//     1
	mbuf_c_empty                    : 1,	//     2
	mbuf_y_empty                    : 1,	//     3
	                                :12,	//  4:15 reserved
	line_cnt                        :11;	// 16:26
} OV_INTR_STATUS;

typedef struct {
	OV_CTRL0                        	ov_ctrl0                        ;	// 0x1100 : ''
	OV_CTRL1                        	ov_ctrl1                        ;	// 0x1104 : ''
	OV_CTRL2                        	ov_ctrl2                        ;	// 0x1108 : ''
	OV_CTRL3                        	ov_ctrl3                        ;	// 0x110c : ''
	OV_MOFFSET                      	ov_moffset                      ;	// 0x1110 : ''
	OV_MSIZE                        	ov_msize                        ;	// 0x1114 : ''
	OV_SOFFSET                      	ov_soffset                      ;	// 0x1118 : ''
	OV_SSIZE                        	ov_ssize                        ;	// 0x111c : ''
	OV_BORDER_WIDTH                 	ov_border_width                 ;	// 0x1120 : ''
	CFLT_COEF0                      	cflt_coef0                      ;	// 0x1124 : ''
	CFLT_COEF1                      	cflt_coef1                      ;	// 0x1128 : ''
	UINT32                          	                 __rsvd_00[   1];	// 0x112c
	OV_PARAM_BY_DDR                 	ov_param_by_ddr                 ;	// 0x1130 : ''
	UINT32                          	                 __rsvd_01[   3];	// 0x1134 ~ 0x113c
	OV_INTR_CTRL                    	ov_intr_ctrl                    ;	// 0x1140 : ''
	OV_INTR_STATUS                  	ov_intr_status                  ;	// 0x1144 : ''
} ov_top_REG_T;
/* 14 regs, 14 types */

/* 14 regs, 14 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegov_top : real register of ov_top.
 * gpRegov_top     : shadow register.
 *
 * @def ov_top_RdFL: Read  FLushing : Shadow <- Real.
 * @def ov_top_WrFL: Write FLushing : Shadow -> Real.
 * @def ov_top_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def ov_top_Wr  : Write whole register(UINT32) from Shadow register.
 * @def ov_top_Rd01 ~ ov_top_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def ov_top_Wr01 ~ ov_top_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define ov_top_RdFL(_r)			((gpRegov_top->_r)=(gpRealRegov_top->_r))
#define ov_top_WrFL(_r)			((gpRealRegov_top->_r)=(gpRegov_top->_r))

#define ov_top_Rd(_r)			*((UINT32*)(&(gpRegov_top->_r)))
#define ov_top_Wr(_r,_v)			((ov_top_Rd(_r))=((UINT32)(_v)))

#define ov_top_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
								} while(0)

#define ov_top_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
								} while(0)

#define ov_top_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
								} while(0)

#define ov_top_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
								} while(0)

#define ov_top_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
								} while(0)

#define ov_top_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
								} while(0)

#define ov_top_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
								} while(0)

#define ov_top_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
								} while(0)

#define ov_top_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
								} while(0)

#define ov_top_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
									(_v10) = (gpRegov_top->_r._f10);				\
								} while(0)

#define ov_top_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
									(_v10) = (gpRegov_top->_r._f10);				\
									(_v11) = (gpRegov_top->_r._f11);				\
								} while(0)

#define ov_top_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
									(_v10) = (gpRegov_top->_r._f10);				\
									(_v11) = (gpRegov_top->_r._f11);				\
									(_v12) = (gpRegov_top->_r._f12);				\
								} while(0)

#define ov_top_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
									(_v10) = (gpRegov_top->_r._f10);				\
									(_v11) = (gpRegov_top->_r._f11);				\
									(_v12) = (gpRegov_top->_r._f12);				\
									(_v13) = (gpRegov_top->_r._f13);				\
								} while(0)

#define ov_top_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
									(_v10) = (gpRegov_top->_r._f10);				\
									(_v11) = (gpRegov_top->_r._f11);				\
									(_v12) = (gpRegov_top->_r._f12);				\
									(_v13) = (gpRegov_top->_r._f13);				\
									(_v14) = (gpRegov_top->_r._f14);				\
								} while(0)

#define ov_top_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
									(_v10) = (gpRegov_top->_r._f10);				\
									(_v11) = (gpRegov_top->_r._f11);				\
									(_v12) = (gpRegov_top->_r._f12);				\
									(_v13) = (gpRegov_top->_r._f13);				\
									(_v14) = (gpRegov_top->_r._f14);				\
									(_v15) = (gpRegov_top->_r._f15);				\
								} while(0)

#define ov_top_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegov_top->_r._f01);				\
									(_v02) = (gpRegov_top->_r._f02);				\
									(_v03) = (gpRegov_top->_r._f03);				\
									(_v04) = (gpRegov_top->_r._f04);				\
									(_v05) = (gpRegov_top->_r._f05);				\
									(_v06) = (gpRegov_top->_r._f06);				\
									(_v07) = (gpRegov_top->_r._f07);				\
									(_v08) = (gpRegov_top->_r._f08);				\
									(_v09) = (gpRegov_top->_r._f09);				\
									(_v10) = (gpRegov_top->_r._f10);				\
									(_v11) = (gpRegov_top->_r._f11);				\
									(_v12) = (gpRegov_top->_r._f12);				\
									(_v13) = (gpRegov_top->_r._f13);				\
									(_v14) = (gpRegov_top->_r._f14);				\
									(_v15) = (gpRegov_top->_r._f15);				\
									(_v16) = (gpRegov_top->_r._f16);				\
								} while(0)


#define ov_top_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
								} while(0)

#define ov_top_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
								} while(0)

#define ov_top_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
								} while(0)

#define ov_top_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
								} while(0)

#define ov_top_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
								} while(0)

#define ov_top_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
								} while(0)

#define ov_top_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
								} while(0)

#define ov_top_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
								} while(0)

#define ov_top_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
								} while(0)

#define ov_top_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
									(gpRegov_top->_r._f10) = (_v10);				\
								} while(0)

#define ov_top_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
									(gpRegov_top->_r._f10) = (_v10);				\
									(gpRegov_top->_r._f11) = (_v11);				\
								} while(0)

#define ov_top_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
									(gpRegov_top->_r._f10) = (_v10);				\
									(gpRegov_top->_r._f11) = (_v11);				\
									(gpRegov_top->_r._f12) = (_v12);				\
								} while(0)

#define ov_top_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
									(gpRegov_top->_r._f10) = (_v10);				\
									(gpRegov_top->_r._f11) = (_v11);				\
									(gpRegov_top->_r._f12) = (_v12);				\
									(gpRegov_top->_r._f13) = (_v13);				\
								} while(0)

#define ov_top_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
									(gpRegov_top->_r._f10) = (_v10);				\
									(gpRegov_top->_r._f11) = (_v11);				\
									(gpRegov_top->_r._f12) = (_v12);				\
									(gpRegov_top->_r._f13) = (_v13);				\
									(gpRegov_top->_r._f14) = (_v14);				\
								} while(0)

#define ov_top_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
									(gpRegov_top->_r._f10) = (_v10);				\
									(gpRegov_top->_r._f11) = (_v11);				\
									(gpRegov_top->_r._f12) = (_v12);				\
									(gpRegov_top->_r._f13) = (_v13);				\
									(gpRegov_top->_r._f14) = (_v14);				\
									(gpRegov_top->_r._f15) = (_v15);				\
								} while(0)

#define ov_top_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegov_top->_r._f01) = (_v01);				\
									(gpRegov_top->_r._f02) = (_v02);				\
									(gpRegov_top->_r._f03) = (_v03);				\
									(gpRegov_top->_r._f04) = (_v04);				\
									(gpRegov_top->_r._f05) = (_v05);				\
									(gpRegov_top->_r._f06) = (_v06);				\
									(gpRegov_top->_r._f07) = (_v07);				\
									(gpRegov_top->_r._f08) = (_v08);				\
									(gpRegov_top->_r._f09) = (_v09);				\
									(gpRegov_top->_r._f10) = (_v10);				\
									(gpRegov_top->_r._f11) = (_v11);				\
									(gpRegov_top->_r._f12) = (_v12);				\
									(gpRegov_top->_r._f13) = (_v13);				\
									(gpRegov_top->_r._f14) = (_v14);				\
									(gpRegov_top->_r._f15) = (_v15);				\
									(gpRegov_top->_r._f16) = (_v16);				\
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
 * _rwname: name of rw    bit field : shall be 0 after ov_top_Wind(), 1 for ov_top_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * ov_top_Rind : General indexed register Read.(
 * ov_top_Wind : General indexed register Read.
 *
 * ov_top_Ridx : For 'index', 'rw', 'load' field name
 * ov_top_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define ov_top_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								ov_top_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								ov_top_WrFL(_r);									\
								ov_top_RdFL(_r);									\
								ov_top_Rd01(_r,_fname,_fval);						\
							} while (0)

#define ov_top_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				ov_top_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define ov_top_Ridx(_r, _ival, _fname, _fval)	ov_top_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define ov_top_Widx(_r, _ival, _fname, _fval)	ov_top_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'LG1150-DE-MAN-06 v0.5 (Window Composer Register Description-100107).csv' 20100311 00:54:18   대한민국 표준시 by getregs v2.3 */
