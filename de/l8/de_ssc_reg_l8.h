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
typedef struct {
	UINT32
	bnd_pel_filtering_scheme        : 1,	//     0
	fir_config                      : 1,	//     1
	filter_type_sel                 : 1,	//     2
	fir_coef_table_sel              : 1,	//     3
	size_info_manual_load           : 1,	//     4
	ddr_update_endian_change        : 1;	//     5
} SSC_CTRL;

/*-----------------------------------------------------------------------------
	0x0904 ssc_field ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	interlace_mode                  : 1,	//     0
	curr_field                      : 1,	//     1
	mpeg_stream                     : 1;	//     2
} SSC_FIELD;

/*-----------------------------------------------------------------------------
	0x0908 ssc_y_init_pos_h ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_init_pos_h                    :12;	//  0:11
} SSC_Y_INIT_POS_H;

/*-----------------------------------------------------------------------------
	0x090c ssc_y_init_pos_v ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_init_pos_v                    :12;	//  0:11
} SSC_Y_INIT_POS_V;

/*-----------------------------------------------------------------------------
	0x0910 ssc_c_init_pos_h ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_init_pos_h                    :12;	//  0:11
} SSC_C_INIT_POS_H;

/*-----------------------------------------------------------------------------
	0x0914 ssc_c_init_pos_v ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_init_pos_v                    :12;	//  0:11
} SSC_C_INIT_POS_V;

/*-----------------------------------------------------------------------------
	0x0918 ssc_scr_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	input_h_size                    :11,	//  0:10
	                                : 5,	// 11:15 reserved
	input_v_size                    :11;	// 16:26
} SSC_SCR_SIZE;

/*-----------------------------------------------------------------------------
	0x091c ssc_win_out_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	out_win_h_size                  :11,	//  0:10
	                                : 5,	// 11:15 reserved
	out_win_v_size                  :11;	// 16:26
} SSC_WIN_OUT_SIZE;

/*-----------------------------------------------------------------------------
	0x0920 ssc_win_in_offset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	in_win_h_offset                 :11,	//  0:10
	                                : 5,	// 11:15 reserved
	in_win_v_offset                 :11;	// 16:26
} SSC_WIN_IN_OFFSET;

/*-----------------------------------------------------------------------------
	0x0924 ssc_win_in_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	in_win_h_size                   :11,	//  0:10
	                                : 5,	// 11:15 reserved
	in_win_v_size                   :11;	// 16:26
} SSC_WIN_IN_SIZE;

/*-----------------------------------------------------------------------------
	0x0928 ssc_c_format ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	in_c_format                     : 3,	//  0: 2
	                                :13,	//  3:15 reserved
	out_c_format                    : 3;	// 16:18
} SSC_C_FORMAT;

/*-----------------------------------------------------------------------------
	0x0930 ssc_y_fir_coef_data0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient0                  :10,	//  0: 9
	                                : 6,	// 10:15 reserved
	y_coefficient1                  :10;	// 16:25
} SSC_Y_FIR_COEF_DATA0;

/*-----------------------------------------------------------------------------
	0x0934 ssc_y_fir_coef_data1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient2                  :10,	//  0: 9
	                                : 6,	// 10:15 reserved
	y_coefficient3                  :10;	// 16:25
} SSC_Y_FIR_COEF_DATA1;

/*-----------------------------------------------------------------------------
	0x0938 ssc_y_fir_coef_data2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient4                  :10,	//  0: 9
	                                : 6,	// 10:15 reserved
	y_coefficient5                  :10;	// 16:25
} SSC_Y_FIR_COEF_DATA2;

/*-----------------------------------------------------------------------------
	0x093c ssc_y_fir_coef_data3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient6                  :10,	//  0: 9
	                                : 6,	// 10:15 reserved
	y_coefficient7                  :10;	// 16:25
} SSC_Y_FIR_COEF_DATA3;

/*-----------------------------------------------------------------------------
	0x0940 ssc_y_fir_coef_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	addr                            : 4,	//  0: 3
	h_v_sel                         : 1,	//     4
	                                : 3,	//  5: 7 reserved
	update                          : 1,	//     8
	                                : 3,	//  9:11 reserved
	rnw                             : 1;	//    12
} SSC_Y_FIR_COEF_CTRL;

/*-----------------------------------------------------------------------------
	0x0944 ssc_update_from_ddr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ddr_stride                      :11,	//  0:10
	                                : 5,	// 11:15 reserved
	ddr_line_cnt                    : 8,	// 16:23
	                                : 7,	// 24:30 reserved
	ddr_update_on                   : 1;	//    31
} SSC_UPDATE_FROM_DDR;

/*-----------------------------------------------------------------------------
	0x0968 ssc_status_rev ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_status                        :11,	//  0:10
	                                : 5,	// 11:15 reserved
	c_status                        :11;	// 16:26
} SSC_STATUS_REV;

/*-----------------------------------------------------------------------------
	0x096c ssc_manual_load ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	update_input_frm_size           : 1,	//     0
	                                : 3,	//  1: 3 reserved
	update_wout_size                : 1,	//     4
	                                : 3,	//  5: 7 reserved
	update_win_offset               : 1,	//     8
	                                : 3,	//  9:11 reserved
	update_win_size                 : 1;	//    12
} SSC_MANUAL_LOAD;

typedef struct {
	SSC_CTRL                        	ssc_ctrl                        ;	// 0x0900 : ''
	SSC_FIELD                       	ssc_field                       ;	// 0x0904 : ''
	SSC_Y_INIT_POS_H                	ssc_y_init_pos_h                ;	// 0x0908 : ''
	SSC_Y_INIT_POS_V                	ssc_y_init_pos_v                ;	// 0x090c : ''
	SSC_C_INIT_POS_H                	ssc_c_init_pos_h                ;	// 0x0910 : ''
	SSC_C_INIT_POS_V                	ssc_c_init_pos_v                ;	// 0x0914 : ''
	SSC_SCR_SIZE                    	ssc_scr_size                    ;	// 0x0918 : ''
	SSC_WIN_OUT_SIZE                	ssc_win_out_size                ;	// 0x091c : ''
	SSC_WIN_IN_OFFSET               	ssc_win_in_offset               ;	// 0x0920 : ''
	SSC_WIN_IN_SIZE                 	ssc_win_in_size                 ;	// 0x0924 : ''
	SSC_C_FORMAT                    	ssc_c_format                    ;	// 0x0928 : ''
	UINT32                          	                 __rsvd_00[   1];	// 0x092c
	SSC_Y_FIR_COEF_DATA0            	ssc_y_fir_coef_data0            ;	// 0x0930 : ''
	SSC_Y_FIR_COEF_DATA1            	ssc_y_fir_coef_data1            ;	// 0x0934 : ''
	SSC_Y_FIR_COEF_DATA2            	ssc_y_fir_coef_data2            ;	// 0x0938 : ''
	SSC_Y_FIR_COEF_DATA3            	ssc_y_fir_coef_data3            ;	// 0x093c : ''
	SSC_Y_FIR_COEF_CTRL             	ssc_y_fir_coef_ctrl             ;	// 0x0940 : ''
	SSC_UPDATE_FROM_DDR             	ssc_update_from_ddr             ;	// 0x0944 : ''
	UINT32                          	                 __rsvd_01[   8];	// 0x0948 ~ 0x0964
	SSC_STATUS_REV                  	ssc_status_rev                  ;	// 0x0968 : ''
	SSC_MANUAL_LOAD                 	ssc_manual_load                 ;	// 0x096c : ''
} SSC_REG_T;
/* 19 regs, 19 types */

/* 19 regs, 19 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegSSC : real register of SSC.
 * gpRegSSC     : shadow register.
 *
 * @def SSC_RdFL: Read  FLushing : Shadow <- Real.
 * @def SSC_WrFL: Write FLushing : Shadow -> Real.
 * @def SSC_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def SSC_Wr  : Write whole register(UINT32) from Shadow register.
 * @def SSC_Rd01 ~ SSC_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def SSC_Wr01 ~ SSC_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define SSC_RdFL(_r)			((gpRegSSC->_r)=(gpRealRegSSC->_r))
#define SSC_WrFL(_r)			((gpRealRegSSC->_r)=(gpRegSSC->_r))

#define SSC_Rd(_r)			*((UINT32*)(&(gpRegSSC->_r)))
#define SSC_Wr(_r,_v)			((SSC_Rd(_r))=((UINT32)(_v)))

#define SSC_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
								} while(0)

#define SSC_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
								} while(0)

#define SSC_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
								} while(0)

#define SSC_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
								} while(0)

#define SSC_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
								} while(0)

#define SSC_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
								} while(0)

#define SSC_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
								} while(0)

#define SSC_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
								} while(0)

#define SSC_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
								} while(0)

#define SSC_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
									(_v10) = (gpRegSSC->_r._f10);				\
								} while(0)

#define SSC_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
									(_v10) = (gpRegSSC->_r._f10);				\
									(_v11) = (gpRegSSC->_r._f11);				\
								} while(0)

#define SSC_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
									(_v10) = (gpRegSSC->_r._f10);				\
									(_v11) = (gpRegSSC->_r._f11);				\
									(_v12) = (gpRegSSC->_r._f12);				\
								} while(0)

#define SSC_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
									(_v10) = (gpRegSSC->_r._f10);				\
									(_v11) = (gpRegSSC->_r._f11);				\
									(_v12) = (gpRegSSC->_r._f12);				\
									(_v13) = (gpRegSSC->_r._f13);				\
								} while(0)

#define SSC_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
									(_v10) = (gpRegSSC->_r._f10);				\
									(_v11) = (gpRegSSC->_r._f11);				\
									(_v12) = (gpRegSSC->_r._f12);				\
									(_v13) = (gpRegSSC->_r._f13);				\
									(_v14) = (gpRegSSC->_r._f14);				\
								} while(0)

#define SSC_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
									(_v10) = (gpRegSSC->_r._f10);				\
									(_v11) = (gpRegSSC->_r._f11);				\
									(_v12) = (gpRegSSC->_r._f12);				\
									(_v13) = (gpRegSSC->_r._f13);				\
									(_v14) = (gpRegSSC->_r._f14);				\
									(_v15) = (gpRegSSC->_r._f15);				\
								} while(0)

#define SSC_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegSSC->_r._f01);				\
									(_v02) = (gpRegSSC->_r._f02);				\
									(_v03) = (gpRegSSC->_r._f03);				\
									(_v04) = (gpRegSSC->_r._f04);				\
									(_v05) = (gpRegSSC->_r._f05);				\
									(_v06) = (gpRegSSC->_r._f06);				\
									(_v07) = (gpRegSSC->_r._f07);				\
									(_v08) = (gpRegSSC->_r._f08);				\
									(_v09) = (gpRegSSC->_r._f09);				\
									(_v10) = (gpRegSSC->_r._f10);				\
									(_v11) = (gpRegSSC->_r._f11);				\
									(_v12) = (gpRegSSC->_r._f12);				\
									(_v13) = (gpRegSSC->_r._f13);				\
									(_v14) = (gpRegSSC->_r._f14);				\
									(_v15) = (gpRegSSC->_r._f15);				\
									(_v16) = (gpRegSSC->_r._f16);				\
								} while(0)


#define SSC_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
								} while(0)

#define SSC_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
								} while(0)

#define SSC_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
								} while(0)

#define SSC_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
								} while(0)

#define SSC_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
								} while(0)

#define SSC_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
								} while(0)

#define SSC_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
								} while(0)

#define SSC_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
								} while(0)

#define SSC_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
								} while(0)

#define SSC_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
									(gpRegSSC->_r._f10) = (_v10);				\
								} while(0)

#define SSC_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
									(gpRegSSC->_r._f10) = (_v10);				\
									(gpRegSSC->_r._f11) = (_v11);				\
								} while(0)

#define SSC_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
									(gpRegSSC->_r._f10) = (_v10);				\
									(gpRegSSC->_r._f11) = (_v11);				\
									(gpRegSSC->_r._f12) = (_v12);				\
								} while(0)

#define SSC_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
									(gpRegSSC->_r._f10) = (_v10);				\
									(gpRegSSC->_r._f11) = (_v11);				\
									(gpRegSSC->_r._f12) = (_v12);				\
									(gpRegSSC->_r._f13) = (_v13);				\
								} while(0)

#define SSC_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
									(gpRegSSC->_r._f10) = (_v10);				\
									(gpRegSSC->_r._f11) = (_v11);				\
									(gpRegSSC->_r._f12) = (_v12);				\
									(gpRegSSC->_r._f13) = (_v13);				\
									(gpRegSSC->_r._f14) = (_v14);				\
								} while(0)

#define SSC_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
									(gpRegSSC->_r._f10) = (_v10);				\
									(gpRegSSC->_r._f11) = (_v11);				\
									(gpRegSSC->_r._f12) = (_v12);				\
									(gpRegSSC->_r._f13) = (_v13);				\
									(gpRegSSC->_r._f14) = (_v14);				\
									(gpRegSSC->_r._f15) = (_v15);				\
								} while(0)

#define SSC_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegSSC->_r._f01) = (_v01);				\
									(gpRegSSC->_r._f02) = (_v02);				\
									(gpRegSSC->_r._f03) = (_v03);				\
									(gpRegSSC->_r._f04) = (_v04);				\
									(gpRegSSC->_r._f05) = (_v05);				\
									(gpRegSSC->_r._f06) = (_v06);				\
									(gpRegSSC->_r._f07) = (_v07);				\
									(gpRegSSC->_r._f08) = (_v08);				\
									(gpRegSSC->_r._f09) = (_v09);				\
									(gpRegSSC->_r._f10) = (_v10);				\
									(gpRegSSC->_r._f11) = (_v11);				\
									(gpRegSSC->_r._f12) = (_v12);				\
									(gpRegSSC->_r._f13) = (_v13);				\
									(gpRegSSC->_r._f14) = (_v14);				\
									(gpRegSSC->_r._f15) = (_v15);				\
									(gpRegSSC->_r._f16) = (_v16);				\
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
 * _rwname: name of rw    bit field : shall be 0 after SSC_Wind(), 1 for SSC_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * SSC_Rind : General indexed register Read.(
 * SSC_Wind : General indexed register Read.
 *
 * SSC_Ridx : For 'index', 'rw', 'load' field name
 * SSC_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define SSC_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								SSC_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								SSC_WrFL(_r);									\
								SSC_RdFL(_r);									\
								SSC_Rd01(_r,_fname,_fval);						\
							} while (0)

#define SSC_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				SSC_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define SSC_Ridx(_r, _ival, _fname, _fval)	SSC_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define SSC_Widx(_r, _ival, _fname, _fval)	SSC_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8-DE-SSC_RegFile_091215.csv' 20100311 00:54:15   대한민국 표준시 by getregs v2.3 */
