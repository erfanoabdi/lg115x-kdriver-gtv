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
typedef struct {
	UINT32
	bnd_pel_filtering_scheme        : 1,	//     0
	adaptive_fir_filter_en          : 1,	//     1
	v_scaler_config                 : 1,	//     2
	                                : 1,	//     3 reserved
	filter_type_sel_luma            : 1,	//     4
	filter_type_sel_chroma          : 1,	//     5
	yuv444_sampling_mode            : 1,	//     6
	ddr_update_endian_change        : 1,	//     7
	size_info_manual_load           : 1,	//     8
	v_chroma_pre_filter_enable      : 1;	//     9
} MSC_CTRL;

/*-----------------------------------------------------------------------------
	0x0504 msc_y_init_pos_h ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_init_pos_h                    :12;	//  0:11
} MSC_Y_INIT_POS_H;

/*-----------------------------------------------------------------------------
	0x0508 msc_y_init_pos_v ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_init_pos_v                    :12;	//  0:11
} MSC_Y_INIT_POS_V;

/*-----------------------------------------------------------------------------
	0x050c msc_c_init_pos_h ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_init_pos_h                    :12;	//  0:11
} MSC_C_INIT_POS_H;

/*-----------------------------------------------------------------------------
	0x0510 msc_c_init_pos_v ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_init_pos_v                    :12;	//  0:11
} MSC_C_INIT_POS_V;

/*-----------------------------------------------------------------------------
	0x0514 msc_adaptive_th ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	reg_th0                         : 2,	//  0: 1
	reg_th1                         : 2,	//  2: 3
	reg_th2                         : 2,	//  4: 5
	reg_th3                         : 2;	//  6: 7
} MSC_ADAPTIVE_TH;

/*-----------------------------------------------------------------------------
	0x0518 msc_scr_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	input_h_size                    :11,	//  0:10
	                                : 5,	// 11:15 reserved
	input_v_size                    :11;	// 16:26
} MSC_SCR_SIZE;

/*-----------------------------------------------------------------------------
	0x051c msc_win_out_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	out_win_h_size                  :11,	//  0:10
	                                : 5,	// 11:15 reserved
	out_win_v_size                  :11;	// 16:26
} MSC_WIN_OUT_SIZE;

/*-----------------------------------------------------------------------------
	0x0520 msc_win_in_offset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	in_win_h_offset                 :11,	//  0:10
	                                : 5,	// 11:15 reserved
	in_win_v_offset                 :11;	// 16:26
} MSC_WIN_IN_OFFSET;

/*-----------------------------------------------------------------------------
	0x0524 msc_win_in_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	in_win_h_size                   :11,	//  0:10
	                                : 5,	// 11:15 reserved
	in_win_v_size                   :11;	// 16:26
} MSC_WIN_IN_SIZE;

/*-----------------------------------------------------------------------------
	0x0528 msc_c_format ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	in_c_format                     : 3,	//  0: 2
	                                :13,	//  3:15 reserved
	out_c_format                    : 3;	// 16:18
} MSC_C_FORMAT;

/*-----------------------------------------------------------------------------
	0x052c msc_3df_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	tdf_enable                      : 1,	//     0
	tdf_mode                        : 1;	//     1
} MSC_3DF_CTRL;

/*-----------------------------------------------------------------------------
	0x0530 msc_y_fir_coef_data0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient0                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	y_coefficient1                  :12;	// 16:27
} MSC_Y_FIR_COEF_DATA0;

/*-----------------------------------------------------------------------------
	0x0534 msc_y_fir_coef_data1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient2                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	y_coefficient3                  :12;	// 16:27
} MSC_Y_FIR_COEF_DATA1;

/*-----------------------------------------------------------------------------
	0x0538 msc_y_fir_coef_data2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient4                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	y_coefficient5                  :12;	// 16:27
} MSC_Y_FIR_COEF_DATA2;

/*-----------------------------------------------------------------------------
	0x053c msc_y_fir_coef_data3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient6                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	y_coefficient7                  :12;	// 16:27
} MSC_Y_FIR_COEF_DATA3;

/*-----------------------------------------------------------------------------
	0x0540 msc_y_fir_coef_data4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient8                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	y_coefficient9                  :12;	// 16:27
} MSC_Y_FIR_COEF_DATA4;

/*-----------------------------------------------------------------------------
	0x0544 msc_y_fir_coef_data5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_coefficient10                 :12,	//  0:11
	                                : 4,	// 12:15 reserved
	y_coefficient11                 :12;	// 16:27
} MSC_Y_FIR_COEF_DATA5;

/*-----------------------------------------------------------------------------
	0x0548 msc_y_fir_coef_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	addr                            : 4,	//  0: 3
	h_v_sel                         : 1,	//     4
	                                : 3,	//  5: 7 reserved
	update                          : 1,	//     8
	                                : 3,	//  9:11 reserved
	rnw                             : 1;	//    12
} MSC_Y_FIR_COEF_CTRL;

/*-----------------------------------------------------------------------------
	0x0550 msc_c_fir_coef_data0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_coefficient0                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	c_coefficient1                  :12;	// 16:27
} MSC_C_FIR_COEF_DATA0;

/*-----------------------------------------------------------------------------
	0x0554 msc_c_fir_coef_data1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_coefficient2                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	c_coefficient3                  :12;	// 16:27
} MSC_C_FIR_COEF_DATA1;

/*-----------------------------------------------------------------------------
	0x0558 msc_c_fir_coef_data2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_coefficient4                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	c_coefficient5                  :12;	// 16:27
} MSC_C_FIR_COEF_DATA2;

/*-----------------------------------------------------------------------------
	0x055c msc_c_fir_coef_data3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	c_coefficient6                  :12,	//  0:11
	                                : 4,	// 12:15 reserved
	c_coefficient7                  :12;	// 16:27
} MSC_C_FIR_COEF_DATA3;

/*-----------------------------------------------------------------------------
	0x0560 msc_c_fir_coef_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	addr                            : 4,	//  0: 3
	h_v_sel                         : 1,	//     4
	                                : 3,	//  5: 7 reserved
	update                          : 1,	//     8
	                                : 3,	//  9:11 reserved
	rnw                             : 1;	//    12
} MSC_C_FIR_COEF_CTRL;

/*-----------------------------------------------------------------------------
	0x0564 msc_update_from_ddr ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ddr_stride                      :11,	//  0:10
	                                : 5,	// 11:15 reserved
	ddr_line_cnt                    : 8,	// 16:23
	                                : 7,	// 24:30 reserved
	ddr_update_on                   : 1;	//    31
} MSC_UPDATE_FROM_DDR;

/*-----------------------------------------------------------------------------
	0x0568 msc_status_rev ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	y_status                        :11,	//  0:10
	                                : 5,	// 11:15 reserved
	c_status                        :11;	// 16:26
} MSC_STATUS_REV;

/*-----------------------------------------------------------------------------
	0x056c msc_manual_load ''
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
} MSC_MANUAL_LOAD;

/*-----------------------------------------------------------------------------
	0x0570 msc_wout_offset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	wout_y_offset                   :11,	//  0:10
	                                : 5,	// 11:15 reserved
	wout_c_offset                   :11;	// 16:26
} MSC_WOUT_OFFSET;

typedef struct {
	MSC_CTRL                        	msc_ctrl                        ;	// 0x0500 : ''
	MSC_Y_INIT_POS_H                	msc_y_init_pos_h                ;	// 0x0504 : ''
	MSC_Y_INIT_POS_V                	msc_y_init_pos_v                ;	// 0x0508 : ''
	MSC_C_INIT_POS_H                	msc_c_init_pos_h                ;	// 0x050c : ''
	MSC_C_INIT_POS_V                	msc_c_init_pos_v                ;	// 0x0510 : ''
	MSC_ADAPTIVE_TH                 	msc_adaptive_th                 ;	// 0x0514 : ''
	MSC_SCR_SIZE                    	msc_scr_size                    ;	// 0x0518 : ''
	MSC_WIN_OUT_SIZE                	msc_win_out_size                ;	// 0x051c : ''
	MSC_WIN_IN_OFFSET               	msc_win_in_offset               ;	// 0x0520 : ''
	MSC_WIN_IN_SIZE                 	msc_win_in_size                 ;	// 0x0524 : ''
	MSC_C_FORMAT                    	msc_c_format                    ;	// 0x0528 : ''
	MSC_3DF_CTRL                    	msc_3df_ctrl                    ;	// 0x052c : ''
	MSC_Y_FIR_COEF_DATA0            	msc_y_fir_coef_data0            ;	// 0x0530 : ''
	MSC_Y_FIR_COEF_DATA1            	msc_y_fir_coef_data1            ;	// 0x0534 : ''
	MSC_Y_FIR_COEF_DATA2            	msc_y_fir_coef_data2            ;	// 0x0538 : ''
	MSC_Y_FIR_COEF_DATA3            	msc_y_fir_coef_data3            ;	// 0x053c : ''
	MSC_Y_FIR_COEF_DATA4            	msc_y_fir_coef_data4            ;	// 0x0540 : ''
	MSC_Y_FIR_COEF_DATA5            	msc_y_fir_coef_data5            ;	// 0x0544 : ''
	MSC_Y_FIR_COEF_CTRL             	msc_y_fir_coef_ctrl             ;	// 0x0548 : ''
	UINT32                          	                 __rsvd_00[   1];	// 0x054c
	MSC_C_FIR_COEF_DATA0            	msc_c_fir_coef_data0            ;	// 0x0550 : ''
	MSC_C_FIR_COEF_DATA1            	msc_c_fir_coef_data1            ;	// 0x0554 : ''
	MSC_C_FIR_COEF_DATA2            	msc_c_fir_coef_data2            ;	// 0x0558 : ''
	MSC_C_FIR_COEF_DATA3            	msc_c_fir_coef_data3            ;	// 0x055c : ''
	MSC_C_FIR_COEF_CTRL             	msc_c_fir_coef_ctrl             ;	// 0x0560 : ''
	MSC_UPDATE_FROM_DDR             	msc_update_from_ddr             ;	// 0x0564 : ''
	MSC_STATUS_REV                  	msc_status_rev                  ;	// 0x0568 : ''
	MSC_MANUAL_LOAD                 	msc_manual_load                 ;	// 0x056c : ''
	MSC_WOUT_OFFSET                 	msc_wout_offset                 ;	// 0x0570 : ''
} MSC_REG_T;
/* 28 regs, 28 types */

/* 28 regs, 28 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegMSC : real register of MSC.
 * gpRegMSC     : shadow register.
 *
 * @def MSC_RdFL: Read  FLushing : Shadow <- Real.
 * @def MSC_WrFL: Write FLushing : Shadow -> Real.
 * @def MSC_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def MSC_Wr  : Write whole register(UINT32) from Shadow register.
 * @def MSC_Rd01 ~ MSC_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def MSC_Wr01 ~ MSC_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define MSC_RdFL(_r)			((gpRegMSC->_r)=(gpRealRegMSC->_r))
#define MSC_WrFL(_r)			((gpRealRegMSC->_r)=(gpRegMSC->_r))

#define MSC_Rd(_r)			*((UINT32*)(&(gpRegMSC->_r)))
#define MSC_Wr(_r,_v)			((MSC_Rd(_r))=((UINT32)(_v)))

#define MSC_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
								} while(0)

#define MSC_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
								} while(0)

#define MSC_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
								} while(0)

#define MSC_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
								} while(0)

#define MSC_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
								} while(0)

#define MSC_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
								} while(0)

#define MSC_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
								} while(0)

#define MSC_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
								} while(0)

#define MSC_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
								} while(0)

#define MSC_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
									(_v10) = (gpRegMSC->_r._f10);				\
								} while(0)

#define MSC_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
									(_v10) = (gpRegMSC->_r._f10);				\
									(_v11) = (gpRegMSC->_r._f11);				\
								} while(0)

#define MSC_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
									(_v10) = (gpRegMSC->_r._f10);				\
									(_v11) = (gpRegMSC->_r._f11);				\
									(_v12) = (gpRegMSC->_r._f12);				\
								} while(0)

#define MSC_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
									(_v10) = (gpRegMSC->_r._f10);				\
									(_v11) = (gpRegMSC->_r._f11);				\
									(_v12) = (gpRegMSC->_r._f12);				\
									(_v13) = (gpRegMSC->_r._f13);				\
								} while(0)

#define MSC_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
									(_v10) = (gpRegMSC->_r._f10);				\
									(_v11) = (gpRegMSC->_r._f11);				\
									(_v12) = (gpRegMSC->_r._f12);				\
									(_v13) = (gpRegMSC->_r._f13);				\
									(_v14) = (gpRegMSC->_r._f14);				\
								} while(0)

#define MSC_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
									(_v10) = (gpRegMSC->_r._f10);				\
									(_v11) = (gpRegMSC->_r._f11);				\
									(_v12) = (gpRegMSC->_r._f12);				\
									(_v13) = (gpRegMSC->_r._f13);				\
									(_v14) = (gpRegMSC->_r._f14);				\
									(_v15) = (gpRegMSC->_r._f15);				\
								} while(0)

#define MSC_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegMSC->_r._f01);				\
									(_v02) = (gpRegMSC->_r._f02);				\
									(_v03) = (gpRegMSC->_r._f03);				\
									(_v04) = (gpRegMSC->_r._f04);				\
									(_v05) = (gpRegMSC->_r._f05);				\
									(_v06) = (gpRegMSC->_r._f06);				\
									(_v07) = (gpRegMSC->_r._f07);				\
									(_v08) = (gpRegMSC->_r._f08);				\
									(_v09) = (gpRegMSC->_r._f09);				\
									(_v10) = (gpRegMSC->_r._f10);				\
									(_v11) = (gpRegMSC->_r._f11);				\
									(_v12) = (gpRegMSC->_r._f12);				\
									(_v13) = (gpRegMSC->_r._f13);				\
									(_v14) = (gpRegMSC->_r._f14);				\
									(_v15) = (gpRegMSC->_r._f15);				\
									(_v16) = (gpRegMSC->_r._f16);				\
								} while(0)


#define MSC_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
								} while(0)

#define MSC_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
								} while(0)

#define MSC_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
								} while(0)

#define MSC_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
								} while(0)

#define MSC_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
								} while(0)

#define MSC_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
								} while(0)

#define MSC_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
								} while(0)

#define MSC_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
								} while(0)

#define MSC_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
								} while(0)

#define MSC_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
									(gpRegMSC->_r._f10) = (_v10);				\
								} while(0)

#define MSC_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
									(gpRegMSC->_r._f10) = (_v10);				\
									(gpRegMSC->_r._f11) = (_v11);				\
								} while(0)

#define MSC_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
									(gpRegMSC->_r._f10) = (_v10);				\
									(gpRegMSC->_r._f11) = (_v11);				\
									(gpRegMSC->_r._f12) = (_v12);				\
								} while(0)

#define MSC_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
									(gpRegMSC->_r._f10) = (_v10);				\
									(gpRegMSC->_r._f11) = (_v11);				\
									(gpRegMSC->_r._f12) = (_v12);				\
									(gpRegMSC->_r._f13) = (_v13);				\
								} while(0)

#define MSC_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
									(gpRegMSC->_r._f10) = (_v10);				\
									(gpRegMSC->_r._f11) = (_v11);				\
									(gpRegMSC->_r._f12) = (_v12);				\
									(gpRegMSC->_r._f13) = (_v13);				\
									(gpRegMSC->_r._f14) = (_v14);				\
								} while(0)

#define MSC_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
									(gpRegMSC->_r._f10) = (_v10);				\
									(gpRegMSC->_r._f11) = (_v11);				\
									(gpRegMSC->_r._f12) = (_v12);				\
									(gpRegMSC->_r._f13) = (_v13);				\
									(gpRegMSC->_r._f14) = (_v14);				\
									(gpRegMSC->_r._f15) = (_v15);				\
								} while(0)

#define MSC_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegMSC->_r._f01) = (_v01);				\
									(gpRegMSC->_r._f02) = (_v02);				\
									(gpRegMSC->_r._f03) = (_v03);				\
									(gpRegMSC->_r._f04) = (_v04);				\
									(gpRegMSC->_r._f05) = (_v05);				\
									(gpRegMSC->_r._f06) = (_v06);				\
									(gpRegMSC->_r._f07) = (_v07);				\
									(gpRegMSC->_r._f08) = (_v08);				\
									(gpRegMSC->_r._f09) = (_v09);				\
									(gpRegMSC->_r._f10) = (_v10);				\
									(gpRegMSC->_r._f11) = (_v11);				\
									(gpRegMSC->_r._f12) = (_v12);				\
									(gpRegMSC->_r._f13) = (_v13);				\
									(gpRegMSC->_r._f14) = (_v14);				\
									(gpRegMSC->_r._f15) = (_v15);				\
									(gpRegMSC->_r._f16) = (_v16);				\
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
 * _rwname: name of rw    bit field : shall be 0 after MSC_Wind(), 1 for MSC_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * MSC_Rind : General indexed register Read.(
 * MSC_Wind : General indexed register Read.
 *
 * MSC_Ridx : For 'index', 'rw', 'load' field name
 * MSC_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define MSC_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								MSC_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								MSC_WrFL(_r);									\
								MSC_RdFL(_r);									\
								MSC_Rd01(_r,_fname,_fval);						\
							} while (0)

#define MSC_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				MSC_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define MSC_Ridx(_r, _ival, _fname, _fval)	MSC_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define MSC_Widx(_r, _ival, _fname, _fval)	MSC_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8-DE-MSC_RegFile_091215.csv' 20100311 00:54:15   대한민국 표준시 by getregs v2.3 */
