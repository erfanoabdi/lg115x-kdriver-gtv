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
	0x0100 top_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	scan_type						: 1,	//	   0
	field_gen_mode					: 2,	//	1: 2
	auto_field_gen					: 2,	//	3: 4
	data_bit_mode					: 1,	//	   5
	chroma_sampling					: 1,	//	   6
	aspect_ratio					: 1,	//	   7
	yc_delay_mode_r					: 2,	//	8: 9
	yc_delay_mode_b					: 2,	// 10:11
	yc_delay_mode_g					: 2,	// 12:13
									: 5,	// 14:18 reserved
	auto_sync_polarity				: 1,	//	  19
	manual_sync_polarity			: 2,	// 20:21
	adc_cut_direction				: 1,	//	  22
	adc_bit_sel						: 1,	//	  23
	br_port_swap					: 1,	//	  24
	r_port_swap						: 1,	//	  25
	b_port_swap						: 1,	//	  26
	g_port_swap						: 1,	//	  27
	bit_numbering_lsb2				: 3,	// 28:30
	data_en_mode					: 1;	//	  31
} CHA_TOP_CTRL;

/*-----------------------------------------------------------------------------
	0x0104 offset_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	offset_h						:12,	//	0:11
									: 4,	// 12:15 reserved
	offset_v						:12;	// 16:27
} CHA_OFFSET_CTRL;

/*-----------------------------------------------------------------------------
	0x0108 size_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	size_h							:12,	//	0:11
									: 4,	// 12:15 reserved
	size_v							:12;	// 16:27
} CHA_SIZE_CTRL;

/*-----------------------------------------------------------------------------
	0x010c int_sync_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	res_hsync						:10,	//	0: 9
									: 6,	// 10:15 reserved
	res_hfp							:10;	// 16:25
} CHA_INT_SYNC_CTRL0;

/*-----------------------------------------------------------------------------
	0x0110 int_sync_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	res_vfp							: 5,	//	0: 4
									: 3,	//	5: 7 reserved
	res_vsync						: 5,	//	8:12
	res_field_add_line				: 2,	// 13:14
	res_field_sel					: 1,	//	  15
	res_sync_sel					: 1;	//	  16
} CHA_INT_SYNC_CTRL1;

/*-----------------------------------------------------------------------------
	0x0114 dig_filt_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient1				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient0				:10;	// 16:25
} CHA_DIG_FILT_CTRL0;

/*-----------------------------------------------------------------------------
	0x0118 dig_filt_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient3				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient2				:10;	// 16:25
} CHA_DIG_FILT_CTRL1;

/*-----------------------------------------------------------------------------
	0x011c dig_filt_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient5				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient4				:10;	// 16:25
} CHA_DIG_FILT_CTRL2;

/*-----------------------------------------------------------------------------
	0x0120 dig_filt_ctrl3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient7				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient6				:10,	// 16:25
	fir_coeff_load					: 1,	//	  26
	fir_en							: 1,	//	  27
	fir_sample_en					: 1,	//	  28
	fir_sampling_rate				: 2,	// 29:30
	fir_sample_c					: 1;	//	  31
} CHA_DIG_FILT_CTRL3;

/*-----------------------------------------------------------------------------
	0x0124 csc_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient1				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient0				:15;	// 16:30
} CHA_CSC_CTRL0;

/*-----------------------------------------------------------------------------
	0x0128 csc_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient3				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient2				:15;	// 16:30
} CHA_CSC_CTRL1;

/*-----------------------------------------------------------------------------
	0x012c csc_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient5				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient4				:15;	// 16:30
} CHA_CSC_CTRL2;

/*-----------------------------------------------------------------------------
	0x0130 csc_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient7				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient6				:15;	// 16:30
} CHA_CSC_CTRL3;

/*-----------------------------------------------------------------------------
	0x0134 csc_ctrl4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_offset0						:11,	//	0:10
									: 5,	// 11:15 reserved
	csc_coefficient8				:15;	// 16:30
} CHA_CSC_CTRL4;

/*-----------------------------------------------------------------------------
	0x0138 csc_ctrl5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_offset2						:11,	//	0:10
									: 5,	// 11:15 reserved
	csc_offset1						:11;	// 16:26
} CHA_CSC_CTRL5;

/*-----------------------------------------------------------------------------
	0x013c csc_ctrl6 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_offset4						:11,	//	0:10
									: 5,	// 11:15 reserved
	csc_offset3						:11;	// 16:26
} CHA_CSC_CTRL6;

/*-----------------------------------------------------------------------------
	0x0140 csc_ctrl7 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coeff_load					: 1,	//	   0
	csc_en							: 1,	//	   1
									:14,	//	2:15 reserved
	csc_offset5						:11;	// 16:26
} CHA_CSC_CTRL7;

/*-----------------------------------------------------------------------------
	0x0144 misc_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pattern_type					: 3,	//	0: 2
	pattern_csc						: 1,	//	   3
	pattern_detail					: 4,	//	4: 7
	reg_update_pulse				: 1,	//	   8
	field_fix_mode					: 2,	//	9:10
	sync_extend						: 1,	//	  11
	write_inhibit					: 1,	//	  12
									:15,	// 13:27 reserved
	reg_update_position				: 4;	// 28:31
} CHA_MISC_CTRL;

/*-----------------------------------------------------------------------------
	0x0148 offset_read ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	hoffset_read					:11,	//	0:10
									: 5,	// 11:15 reserved
	voffset_read					:11;	// 16:26
} CHA_OFFSET_READ;

/*-----------------------------------------------------------------------------
	0x014c cva_intr_msk ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sd_vd_range						: 2,	//	0: 1
	sd_ext_range					: 2,	//	2: 3
	intr_mask_node					: 1,	//	   4
	intr_mask_nohsync				: 1,	//	   5
	intrl_mask_novsync				: 1,	//	   6
	intr_mask_abnormal				: 1,	//	   7
	intr_mask_invalid				: 1,	//	   8
	intr_mask_de					: 1,	//	   9
	intr_mask_hsync					: 1,	//	  10
	intr_mask_vsync					: 1,	//	  11
	intr_mask_normal				: 1,	//	  12
	intr_mask_valid					: 1,	//	  13
	sd_hsync_sel					: 1,	//	  14
	sd_en							: 1,	//	  15
	sd_frame_cnt					: 3,	// 16:18
									: 1,	//	  19 reserved
	sd_line_num						:12;	// 20:31
} CHA_INTR_MSK;

/*-----------------------------------------------------------------------------
	0x0150 size_read ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	line_num_read					:16,	//	0:15
	line_width_read					:16;	// 16:31
} CHA_SIZE_READ;

/*-----------------------------------------------------------------------------
	0x0154 pre3d_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pre3d_sync_enable				: 1,	//	   0
	pre3d_sync_mode					: 2,	//	1: 2
	pre3d_field_mode				: 1,	//	   3
	pre3d_frame_lr_mode				: 1,	//	   4
	pre3d_mid_vsync_bypass			: 1,	//	   5
									:10,	//	6:15 reserved
	pre3d_active_line_num			:12;	// 16:27
} CHA_PRE3D_CTRL0;

/*-----------------------------------------------------------------------------
	0x0158 pre3d_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pre3d_active_space1_line_num	:12,	//	0:11
									: 4,	// 12:15 reserved
	pre3d_active_space2_line_num	:12;	// 16:27
} CHA_PRE3D_CTRL1;

/*-----------------------------------------------------------------------------
	0x015c pre3d_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pre3d_front_porch_line_num		:12,	//	0:11
									: 4,	// 12:15 reserved
	pre3d_sync_line_num				:12;	// 16:27
} CHA_PRE3D_CTRL2;

/*-----------------------------------------------------------------------------
	0x0160 size_status0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	current_line_number_read		:12,	//	0:11
	de_signal_detect				: 1,	//	  12
	hsync_signal_detect				: 1,	//	  13
	vsync_signal_detect				: 1,	//	  14
	source_valid					: 1,	//	  15
	sd_hsync_fault_flag_read		: 1,	//	  16
	sd_vsync_fault_flag_read		: 1,	//	  17
									: 6,	// 18:23 reserved
	sd_hsync_novalid_size			: 3;	// 24:26
} CHA_SIZE_STATUS0;

/*-----------------------------------------------------------------------------
	0x0164 size_status1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sd_hoffset_indurance_read		: 8,	//	0: 7
									: 8,	//	8:15 reserved
	sd_voffset_indurance_read		: 8;	// 16:23
} CHA_SIZE_STATUS1;

/*-----------------------------------------------------------------------------
	0x0180 top_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	scan_type						: 1,	//	   0
	field_gen_mode					: 2,	//	1: 2
	auto_field_gen					: 2,	//	3: 4
	data_bit_mode					: 1,	//	   5
	chroma_sampling					: 1,	//	   6
	aspect_ratio					: 1,	//	   7
	yc_delay_mode_r					: 2,	//	8: 9
	yc_delay_mode_b					: 2,	// 10:11
	yc_delay_mode_g					: 2,	// 12:13
									: 5,	// 14:18 reserved
	auto_sync_polarity				: 1,	//	  19
	manual_sync_polarity			: 2,	// 20:21
	adc_cut_direction				: 1,	//	  22
	adc_bit_sel						: 1,	//	  23
	br_port_swap					: 1,	//	  24
	r_port_swap						: 1,	//	  25
	b_port_swap						: 1,	//	  26
	g_port_swap						: 1,	//	  27
	bit_numbering_lsb2				: 3,	// 28:30
	data_en_mode					: 1;	//	  31
} CHB_TOP_CTRL;

/*-----------------------------------------------------------------------------
	0x0184 offset_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	offset_h						:12,	//	0:11
									: 4,	// 12:15 reserved
	offset_v						:12;	// 16:27
} CHB_OFFSET_CTRL;

/*-----------------------------------------------------------------------------
	0x0188 size_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	size_h							:12,	//	0:11
									: 4,	// 12:15 reserved
	size_v							:12;	// 16:27
} CHB_SIZE_CTRL;

/*-----------------------------------------------------------------------------
	0x018c int_sync_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	res_hsync						:10,	//	0: 9
									: 6,	// 10:15 reserved
	res_hfp							:10;	// 16:25
} CHB_INT_SYNC_CTRL0;

/*-----------------------------------------------------------------------------
	0x0190 int_sync_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	res_vfp							: 5,	//	0: 4
									: 3,	//	5: 7 reserved
	res_vsync						: 5,	//	8:12
	res_field_add_line				: 2,	// 13:14
	res_field_sel					: 1,	//	  15
	res_sync_sel					: 1;	//	  16
} CHB_INT_SYNC_CTRL1;

/*-----------------------------------------------------------------------------
	0x0194 dig_filt_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient1				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient0				:10;	// 16:25
} CHB_DIG_FILT_CTRL0;

/*-----------------------------------------------------------------------------
	0x0198 dig_filt_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient3				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient2				:10;	// 16:25
} CHB_DIG_FILT_CTRL1;

/*-----------------------------------------------------------------------------
	0x019c dig_filt_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient5				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient4				:10;	// 16:25
} CHB_DIG_FILT_CTRL2;

/*-----------------------------------------------------------------------------
	0x01a0 dig_filt_ctrl3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fir_coefficient7				:10,	//	0: 9
									: 6,	// 10:15 reserved
	fir_coefficient6				:10,	// 16:25
	fir_coeff_load					: 1,	//	  26
	fir_en							: 1,	//	  27
	fir_sample_en					: 1,	//	  28
	fir_sampling_rate				: 2,	// 29:30
	fir_sample_c					: 1;	//	  31
} CHB_DIG_FILT_CTRL3;

/*-----------------------------------------------------------------------------
	0x01a4 csc_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient1				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient0				:15;	// 16:30
} CHB_CSC_CTRL0;

/*-----------------------------------------------------------------------------
	0x01a8 csc_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient3				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient2				:15;	// 16:30
} CHB_CSC_CTRL1;

/*-----------------------------------------------------------------------------
	0x01ac csc_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient5				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient4				:15;	// 16:30
} CHB_CSC_CTRL2;

/*-----------------------------------------------------------------------------
	0x01b0 csc_ctrl3 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coefficient7				:15,	//	0:14
									: 1,	//	  15 reserved
	csc_coefficient6				:15;	// 16:30
} CHB_CSC_CTRL3;

/*-----------------------------------------------------------------------------
	0x01b4 csc_ctrl4 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_offset0						:11,	//	0:10
									: 5,	// 11:15 reserved
	csc_coefficient8				:15;	// 16:30
} CHB_CSC_CTRL4;

/*-----------------------------------------------------------------------------
	0x01b8 csc_ctrl5 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_offset2						:11,	//	0:10
									: 5,	// 11:15 reserved
	csc_offset1						:11;	// 16:26
} CHB_CSC_CTRL5;

/*-----------------------------------------------------------------------------
	0x01bc csc_ctrl6 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_offset4						:11,	//	0:10
									: 5,	// 11:15 reserved
	csc_offset3						:11;	// 16:26
} CHB_CSC_CTRL6;

/*-----------------------------------------------------------------------------
	0x01c0 csc_ctrl7 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	csc_coeff_load					: 1,	//	   0
	csc_en							: 1,	//	   1
									:14,	//	2:15 reserved
	csc_offset5						:11;	// 16:26
} CHB_CSC_CTRL7;

/*-----------------------------------------------------------------------------
	0x01c4 misc_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pattern_type					: 3,	//	0: 2
	pattern_csc						: 1,	//	   3
	pattern_detail					: 4,	//	4: 7
	reg_update_pulse				: 1,	//	   8
	field_fix_mode					: 2,	//	9:10
	sync_extend						: 1,	//	  11
	write_inhibit					: 1,	//	  12
									:15,	// 13:27 reserved
	reg_update_position				: 4;	// 28:31
} CHB_MISC_CTRL;

/*-----------------------------------------------------------------------------
	0x01c8 offset_read ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	hoffset_read					:11,	//	0:10
									: 5,	// 11:15 reserved
	voffset_read					:11;	// 16:26
} CHB_OFFSET_READ;

/*-----------------------------------------------------------------------------
	0x01cc cvb_intr_msk ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sd_vd_range						: 2,	//	0: 1
	sd_ext_range					: 2,	//	2: 3
	intr_mask_node					: 1,	//	   4
	intr_mask_nohsync				: 1,	//	   5
	intrl_mask_novsync				: 1,	//	   6
	intr_mask_abnormal				: 1,	//	   7
	intr_mask_invalid				: 1,	//	   8
	intr_mask_de					: 1,	//	   9
	intr_mask_hsync					: 1,	//	  10
	intr_mask_vsync					: 1,	//	  11
	intr_mask_normal				: 1,	//	  12
	intr_mask_valid					: 1,	//	  13
	sd_hsync_sel					: 1,	//	  14
	sd_en							: 1,	//	  15
	sd_frame_cnt					: 3,	// 16:18
									: 1,	//	  19 reserved
	sd_line_num						:12;	// 20:31
} CHB_INTR_MSK;

/*-----------------------------------------------------------------------------
	0x01d0 size_read ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	line_num_read					:16,	//	0:15
	line_width_read					:16;	// 16:31
} CHB_SIZE_READ;

/*-----------------------------------------------------------------------------
	0x01d4 pre3d_ctrl0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pre3d_sync_enable				: 1,	//	   0
	pre3d_sync_mode					: 2,	//	1: 2
	pre3d_field_mode				: 1,	//	   3
	pre3d_frame_lr_mode				: 1,	//	   4
	pre3d_mid_vsync_bypass			: 1,	//	   5
									:10,	//	6:15 reserved
	pre3d_active_line_num			:12;	// 16:27
} CHB_PRE3D_CTRL0;

/*-----------------------------------------------------------------------------
	0x01d8 pre3d_ctrl1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pre3d_active_space1_line_num	:12,	//	0:11
									: 4,	// 12:15 reserved
	pre3d_active_space2_line_num	:12;	// 16:27
} CHB_PRE3D_CTRL1;

/*-----------------------------------------------------------------------------
	0x01dc pre3d_ctrl2 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pre3d_front_porch_line_num		:12,	//	0:11
									: 4,	// 12:15 reserved
	pre3d_sync_line_num				:12;	// 16:27
} CHB_PRE3D_CTRL2;

/*-----------------------------------------------------------------------------
	0x01e0 size_status0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	current_line_number_read		:12,	//	0:11
	de_signal_detect				: 1,	//	  12
	hsync_signal_detect				: 1,	//	  13
	vsync_signal_detect				: 1,	//	  14
	source_valid					: 1,	//	  15
	sd_hsync_fault_flag_read		: 1,	//	  16
	sd_vsync_fault_flag_read		: 1,	//	  17
									: 6,	// 18:23 reserved
	sd_hsync_novalid_size			: 3;	// 24:26
} CHB_SIZE_STATUS0;

/*-----------------------------------------------------------------------------
	0x01e4 size_status1 ''
------------------------------------------------------------------------------*/
/*	no field */

typedef struct {
	CHA_TOP_CTRL						   cha_top_ctrl						;	// 0x0100 : ''
	CHA_OFFSET_CTRL						   cha_offset_ctrl					;	// 0x0104 : ''
	CHA_SIZE_CTRL						   cha_size_ctrl					;	// 0x0108 : ''
	CHA_INT_SYNC_CTRL0					   cha_int_sync_ctrl0				;	// 0x010c : ''
	CHA_INT_SYNC_CTRL1					   cha_int_sync_ctrl1				;	// 0x0110 : ''
	CHA_DIG_FILT_CTRL0					   cha_dig_filt_ctrl0				;	// 0x0114 : ''
	CHA_DIG_FILT_CTRL1					   cha_dig_filt_ctrl1				;	// 0x0118 : ''
	CHA_DIG_FILT_CTRL2					   cha_dig_filt_ctrl2				;	// 0x011c : ''
	CHA_DIG_FILT_CTRL3					   cha_dig_filt_ctrl3				;	// 0x0120 : ''
	CHA_CSC_CTRL0						   cha_csc_ctrl0					;	// 0x0124 : ''
	CHA_CSC_CTRL1						   cha_csc_ctrl1					;	// 0x0128 : ''
	CHA_CSC_CTRL2						   cha_csc_ctrl2					;	// 0x012c : ''
	CHA_CSC_CTRL3						   cha_csc_ctrl3					;	// 0x0130 : ''
	CHA_CSC_CTRL4						   cha_csc_ctrl4					;	// 0x0134 : ''
	CHA_CSC_CTRL5						   cha_csc_ctrl5					;	// 0x0138 : ''
	CHA_CSC_CTRL6						   cha_csc_ctrl6					;	// 0x013c : ''
	CHA_CSC_CTRL7						   cha_csc_ctrl7					;	// 0x0140 : ''
	CHA_MISC_CTRL						   cha_misc_ctrl					;	// 0x0144 : ''
	CHA_OFFSET_READ						   cha_offset_read					;	// 0x0148 : ''
	CHA_INTR_MSK						   cha_intr_msk						;	// 0x014c : ''
	CHA_SIZE_READ						   cha_size_read					;	// 0x0150 : ''
	CHA_PRE3D_CTRL0						   cha_pre3d_ctrl0					;	// 0x0154 : ''
	CHA_PRE3D_CTRL1						   cha_pre3d_ctrl1					;	// 0x0158 : ''
	CHA_PRE3D_CTRL2						   cha_pre3d_ctrl2					;	// 0x015c : ''
	CHA_SIZE_STATUS0					   cha_size_status0					;	// 0x0160 : ''
	CHA_SIZE_STATUS1					   cha_size_status1					;	// 0x0164 : ''
} CHA_CVI_REG_T;
/* 26 regs, 26 types */

typedef struct {
	CHB_TOP_CTRL						   chb_top_ctrl						;	// 0x0180 : ''
	CHB_OFFSET_CTRL						   chb_offset_ctrl					;	// 0x0184 : ''
	CHB_SIZE_CTRL						   chb_size_ctrl					;	// 0x0188 : ''
	CHB_INT_SYNC_CTRL0					   chb_int_sync_ctrl0				;	// 0x018c : ''
	CHB_INT_SYNC_CTRL1					   chb_int_sync_ctrl1				;	// 0x0190 : ''
	CHB_DIG_FILT_CTRL0					   chb_dig_filt_ctrl0				;	// 0x0194 : ''
	CHB_DIG_FILT_CTRL1					   chb_dig_filt_ctrl1				;	// 0x0198 : ''
	CHB_DIG_FILT_CTRL2					   chb_dig_filt_ctrl2				;	// 0x019c : ''
	CHB_DIG_FILT_CTRL3					   chb_dig_filt_ctrl3				;	// 0x01a0 : ''
	CHB_CSC_CTRL0						   chb_csc_ctrl0					;	// 0x01a4 : ''
	CHB_CSC_CTRL1						   chb_csc_ctrl1					;	// 0x01a8 : ''
	CHB_CSC_CTRL2						   chb_csc_ctrl2					;	// 0x01ac : ''
	CHB_CSC_CTRL3						   chb_csc_ctrl3					;	// 0x01b0 : ''
	CHB_CSC_CTRL4						   chb_csc_ctrl4					;	// 0x01b4 : ''
	CHB_CSC_CTRL5						   chb_csc_ctrl5					;	// 0x01b8 : ''
	CHB_CSC_CTRL6						   chb_csc_ctrl6					;	// 0x01bc : ''
	CHB_CSC_CTRL7						   chb_csc_ctrl7					;	// 0x01c0 : ''
	CHB_MISC_CTRL						   chb_misc_ctrl					;	// 0x01c4 : ''
	CHB_OFFSET_READ						   chb_offset_read					;	// 0x01c8 : ''
	CHB_INTR_MSK						   chb_intr_msk						;	// 0x01cc : ''
	CHB_SIZE_READ						   chb_size_read					;	// 0x01d0 : ''
	CHB_PRE3D_CTRL0						   chb_pre3d_ctrl0					;	// 0x01d4 : ''
	CHB_PRE3D_CTRL1						   chb_pre3d_ctrl1					;	// 0x01d8 : ''
	CHB_PRE3D_CTRL2						   chb_pre3d_ctrl2					;	// 0x01dc : ''
	CHB_SIZE_STATUS0					   chb_size_status0					;	// 0x01e0 : ''
	UINT32								   chb_size_status1					;	// 0x01e4 : ''
} CHB_CVI_REG_T;
/* 26 regs, 26 types */

/* 52 regs, 52 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegCHA_CVI : real register of CHA_CVI.
 * gpRegCHA_CVI		: shadow register.
 *
 * @def CHA_CVI_RdFL: Read	FLushing : Shadow <- Real.
 * @def CHA_CVI_WrFL: Write FLushing : Shadow -> Real.
 * @def CHA_CVI_Rd	: Read	whole register(UINT32) from Shadow register.
 * @def CHA_CVI_Wr	: Write whole register(UINT32) from Shadow register.
 * @def CHA_CVI_Rd01 ~ CHA_CVI_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def CHA_CVI_Wr01 ~ CHA_CVI_Wrnn: Write given '01~nn' fields to	 Shadow register.
 * */
#define CHA_CVI_RdFL(_r)			((gpRegCHA_CVI->_r)=(gpRealRegCHA_CVI->_r))
#define CHA_CVI_WrFL(_r)			((gpRealRegCHA_CVI->_r)=(gpRegCHA_CVI->_r))

#define CHA_CVI_Rd(_r)			*((UINT32*)(&(gpRegCHA_CVI->_r)))
#define CHA_CVI_Wr(_r,_v)			((CHA_CVI_Rd(_r))=((UINT32)(_v)))

#define CHA_CVI_Rd01(_r,_f01,_v01)													\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
								} while(0)

#define CHA_CVI_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
								} while(0)

#define CHA_CVI_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
								} while(0)

#define CHA_CVI_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
								} while(0)

#define CHA_CVI_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
								} while(0)

#define CHA_CVI_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
								} while(0)

#define CHA_CVI_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
								} while(0)

#define CHA_CVI_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
								} while(0)

#define CHA_CVI_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
								} while(0)

#define CHA_CVI_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
									(_v10) = (gpRegCHA_CVI->_r._f10);				\
								} while(0)

#define CHA_CVI_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
									(_v10) = (gpRegCHA_CVI->_r._f10);				\
									(_v11) = (gpRegCHA_CVI->_r._f11);				\
								} while(0)

#define CHA_CVI_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
									(_v10) = (gpRegCHA_CVI->_r._f10);				\
									(_v11) = (gpRegCHA_CVI->_r._f11);				\
									(_v12) = (gpRegCHA_CVI->_r._f12);				\
								} while(0)

#define CHA_CVI_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
									(_v10) = (gpRegCHA_CVI->_r._f10);				\
									(_v11) = (gpRegCHA_CVI->_r._f11);				\
									(_v12) = (gpRegCHA_CVI->_r._f12);				\
									(_v13) = (gpRegCHA_CVI->_r._f13);				\
								} while(0)

#define CHA_CVI_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
									(_v10) = (gpRegCHA_CVI->_r._f10);				\
									(_v11) = (gpRegCHA_CVI->_r._f11);				\
									(_v12) = (gpRegCHA_CVI->_r._f12);				\
									(_v13) = (gpRegCHA_CVI->_r._f13);				\
									(_v14) = (gpRegCHA_CVI->_r._f14);				\
								} while(0)

#define CHA_CVI_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
									(_v10) = (gpRegCHA_CVI->_r._f10);				\
									(_v11) = (gpRegCHA_CVI->_r._f11);				\
									(_v12) = (gpRegCHA_CVI->_r._f12);				\
									(_v13) = (gpRegCHA_CVI->_r._f13);				\
									(_v14) = (gpRegCHA_CVI->_r._f14);				\
									(_v15) = (gpRegCHA_CVI->_r._f15);				\
								} while(0)

#define CHA_CVI_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(_v01) = (gpRegCHA_CVI->_r._f01);				\
									(_v02) = (gpRegCHA_CVI->_r._f02);				\
									(_v03) = (gpRegCHA_CVI->_r._f03);				\
									(_v04) = (gpRegCHA_CVI->_r._f04);				\
									(_v05) = (gpRegCHA_CVI->_r._f05);				\
									(_v06) = (gpRegCHA_CVI->_r._f06);				\
									(_v07) = (gpRegCHA_CVI->_r._f07);				\
									(_v08) = (gpRegCHA_CVI->_r._f08);				\
									(_v09) = (gpRegCHA_CVI->_r._f09);				\
									(_v10) = (gpRegCHA_CVI->_r._f10);				\
									(_v11) = (gpRegCHA_CVI->_r._f11);				\
									(_v12) = (gpRegCHA_CVI->_r._f12);				\
									(_v13) = (gpRegCHA_CVI->_r._f13);				\
									(_v14) = (gpRegCHA_CVI->_r._f14);				\
									(_v15) = (gpRegCHA_CVI->_r._f15);				\
									(_v16) = (gpRegCHA_CVI->_r._f16);				\
								} while(0)


#define CHA_CVI_Wr01(_r,_f01,_v01)													\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
								} while(0)

#define CHA_CVI_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
								} while(0)

#define CHA_CVI_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
								} while(0)

#define CHA_CVI_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
								} while(0)

#define CHA_CVI_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
								} while(0)

#define CHA_CVI_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
								} while(0)

#define CHA_CVI_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
								} while(0)

#define CHA_CVI_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
								} while(0)

#define CHA_CVI_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
								} while(0)

#define CHA_CVI_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
									(gpRegCHA_CVI->_r._f10) = (_v10);				\
								} while(0)

#define CHA_CVI_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
									(gpRegCHA_CVI->_r._f10) = (_v10);				\
									(gpRegCHA_CVI->_r._f11) = (_v11);				\
								} while(0)

#define CHA_CVI_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
									(gpRegCHA_CVI->_r._f10) = (_v10);				\
									(gpRegCHA_CVI->_r._f11) = (_v11);				\
									(gpRegCHA_CVI->_r._f12) = (_v12);				\
								} while(0)

#define CHA_CVI_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
									(gpRegCHA_CVI->_r._f10) = (_v10);				\
									(gpRegCHA_CVI->_r._f11) = (_v11);				\
									(gpRegCHA_CVI->_r._f12) = (_v12);				\
									(gpRegCHA_CVI->_r._f13) = (_v13);				\
								} while(0)

#define CHA_CVI_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
									(gpRegCHA_CVI->_r._f10) = (_v10);				\
									(gpRegCHA_CVI->_r._f11) = (_v11);				\
									(gpRegCHA_CVI->_r._f12) = (_v12);				\
									(gpRegCHA_CVI->_r._f13) = (_v13);				\
									(gpRegCHA_CVI->_r._f14) = (_v14);				\
								} while(0)

#define CHA_CVI_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
									(gpRegCHA_CVI->_r._f10) = (_v10);				\
									(gpRegCHA_CVI->_r._f11) = (_v11);				\
									(gpRegCHA_CVI->_r._f12) = (_v12);				\
									(gpRegCHA_CVI->_r._f13) = (_v13);				\
									(gpRegCHA_CVI->_r._f14) = (_v14);				\
									(gpRegCHA_CVI->_r._f15) = (_v15);				\
								} while(0)

#define CHA_CVI_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(gpRegCHA_CVI->_r._f01) = (_v01);				\
									(gpRegCHA_CVI->_r._f02) = (_v02);				\
									(gpRegCHA_CVI->_r._f03) = (_v03);				\
									(gpRegCHA_CVI->_r._f04) = (_v04);				\
									(gpRegCHA_CVI->_r._f05) = (_v05);				\
									(gpRegCHA_CVI->_r._f06) = (_v06);				\
									(gpRegCHA_CVI->_r._f07) = (_v07);				\
									(gpRegCHA_CVI->_r._f08) = (_v08);				\
									(gpRegCHA_CVI->_r._f09) = (_v09);				\
									(gpRegCHA_CVI->_r._f10) = (_v10);				\
									(gpRegCHA_CVI->_r._f11) = (_v11);				\
									(gpRegCHA_CVI->_r._f12) = (_v12);				\
									(gpRegCHA_CVI->_r._f13) = (_v13);				\
									(gpRegCHA_CVI->_r._f14) = (_v14);				\
									(gpRegCHA_CVI->_r._f15) = (_v15);				\
									(gpRegCHA_CVI->_r._f16) = (_v16);				\
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
 * _r	  : name of register
 * _lname : name of load  bit field : shall be 0 after macro executed.
 * _rwname: name of rw	  bit field : shall be 0 after CHA_CVI_Wind(), 1 for CHA_CVI_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * CHA_CVI_Rind : General indexed register Read.(
 * CHA_CVI_Wind : General indexed register Read.
 *
 * CHA_CVI_Ridx : For 'index', 'rw', 'load' field name
 * CHA_CVI_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define CHA_CVI_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								CHA_CVI_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								CHA_CVI_WrFL(_r);									\
								CHA_CVI_RdFL(_r);									\
								CHA_CVI_Rd01(_r,_fname,_fval);						\
							} while (0)

#define CHA_CVI_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				CHA_CVI_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define CHA_CVI_Ridx(_r, _ival, _fname, _fval)	CHA_CVI_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define CHA_CVI_Widx(_r, _ival, _fname, _fval)	CHA_CVI_Wind(_r,load,rw,index,_ival,_fname,_fval)

/*
 * @{
 * Naming for register pointer.
 * gpRealRegCHB_CVI : real register of CHB_CVI.
 * gpRegCHB_CVI		: shadow register.
 *
 * @def CHB_CVI_RdFL: Read	FLushing : Shadow <- Real.
 * @def CHB_CVI_WrFL: Write FLushing : Shadow -> Real.
 * @def CHB_CVI_Rd	: Read	whole register(UINT32) from Shadow register.
 * @def CHB_CVI_Wr	: Write whole register(UINT32) from Shadow register.
 * @def CHB_CVI_Rd01 ~ CHB_CVI_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def CHB_CVI_Wr01 ~ CHB_CVI_Wrnn: Write given '01~nn' fields to	 Shadow register.
 * */
#define CHB_CVI_RdFL(_r)			((gpRegCHB_CVI->_r)=(gpRealRegCHB_CVI->_r))
#define CHB_CVI_WrFL(_r)			((gpRealRegCHB_CVI->_r)=(gpRegCHB_CVI->_r))

#define CHB_CVI_Rd(_r)			*((UINT32*)(&(gpRegCHB_CVI->_r)))
#define CHB_CVI_Wr(_r,_v)			((CHB_CVI_Rd(_r))=((UINT32)(_v)))

#define CHB_CVI_Rd01(_r,_f01,_v01)													\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
								} while(0)

#define CHB_CVI_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
								} while(0)

#define CHB_CVI_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
								} while(0)

#define CHB_CVI_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
								} while(0)

#define CHB_CVI_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
								} while(0)

#define CHB_CVI_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
								} while(0)

#define CHB_CVI_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
								} while(0)

#define CHB_CVI_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
								} while(0)

#define CHB_CVI_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
								} while(0)

#define CHB_CVI_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
									(_v10) = (gpRegCHB_CVI->_r._f10);				\
								} while(0)

#define CHB_CVI_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
									(_v10) = (gpRegCHB_CVI->_r._f10);				\
									(_v11) = (gpRegCHB_CVI->_r._f11);				\
								} while(0)

#define CHB_CVI_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
									(_v10) = (gpRegCHB_CVI->_r._f10);				\
									(_v11) = (gpRegCHB_CVI->_r._f11);				\
									(_v12) = (gpRegCHB_CVI->_r._f12);				\
								} while(0)

#define CHB_CVI_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
									(_v10) = (gpRegCHB_CVI->_r._f10);				\
									(_v11) = (gpRegCHB_CVI->_r._f11);				\
									(_v12) = (gpRegCHB_CVI->_r._f12);				\
									(_v13) = (gpRegCHB_CVI->_r._f13);				\
								} while(0)

#define CHB_CVI_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
									(_v10) = (gpRegCHB_CVI->_r._f10);				\
									(_v11) = (gpRegCHB_CVI->_r._f11);				\
									(_v12) = (gpRegCHB_CVI->_r._f12);				\
									(_v13) = (gpRegCHB_CVI->_r._f13);				\
									(_v14) = (gpRegCHB_CVI->_r._f14);				\
								} while(0)

#define CHB_CVI_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
									(_v10) = (gpRegCHB_CVI->_r._f10);				\
									(_v11) = (gpRegCHB_CVI->_r._f11);				\
									(_v12) = (gpRegCHB_CVI->_r._f12);				\
									(_v13) = (gpRegCHB_CVI->_r._f13);				\
									(_v14) = (gpRegCHB_CVI->_r._f14);				\
									(_v15) = (gpRegCHB_CVI->_r._f15);				\
								} while(0)

#define CHB_CVI_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(_v01) = (gpRegCHB_CVI->_r._f01);				\
									(_v02) = (gpRegCHB_CVI->_r._f02);				\
									(_v03) = (gpRegCHB_CVI->_r._f03);				\
									(_v04) = (gpRegCHB_CVI->_r._f04);				\
									(_v05) = (gpRegCHB_CVI->_r._f05);				\
									(_v06) = (gpRegCHB_CVI->_r._f06);				\
									(_v07) = (gpRegCHB_CVI->_r._f07);				\
									(_v08) = (gpRegCHB_CVI->_r._f08);				\
									(_v09) = (gpRegCHB_CVI->_r._f09);				\
									(_v10) = (gpRegCHB_CVI->_r._f10);				\
									(_v11) = (gpRegCHB_CVI->_r._f11);				\
									(_v12) = (gpRegCHB_CVI->_r._f12);				\
									(_v13) = (gpRegCHB_CVI->_r._f13);				\
									(_v14) = (gpRegCHB_CVI->_r._f14);				\
									(_v15) = (gpRegCHB_CVI->_r._f15);				\
									(_v16) = (gpRegCHB_CVI->_r._f16);				\
								} while(0)


#define CHB_CVI_Wr01(_r,_f01,_v01)													\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
								} while(0)

#define CHB_CVI_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
								} while(0)

#define CHB_CVI_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
								} while(0)

#define CHB_CVI_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
								} while(0)

#define CHB_CVI_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
								} while(0)

#define CHB_CVI_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
								} while(0)

#define CHB_CVI_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
								} while(0)

#define CHB_CVI_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
								} while(0)

#define CHB_CVI_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
								} while(0)

#define CHB_CVI_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
									(gpRegCHB_CVI->_r._f10) = (_v10);				\
								} while(0)

#define CHB_CVI_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
									(gpRegCHB_CVI->_r._f10) = (_v10);				\
									(gpRegCHB_CVI->_r._f11) = (_v11);				\
								} while(0)

#define CHB_CVI_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
									(gpRegCHB_CVI->_r._f10) = (_v10);				\
									(gpRegCHB_CVI->_r._f11) = (_v11);				\
									(gpRegCHB_CVI->_r._f12) = (_v12);				\
								} while(0)

#define CHB_CVI_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
									(gpRegCHB_CVI->_r._f10) = (_v10);				\
									(gpRegCHB_CVI->_r._f11) = (_v11);				\
									(gpRegCHB_CVI->_r._f12) = (_v12);				\
									(gpRegCHB_CVI->_r._f13) = (_v13);				\
								} while(0)

#define CHB_CVI_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
									(gpRegCHB_CVI->_r._f10) = (_v10);				\
									(gpRegCHB_CVI->_r._f11) = (_v11);				\
									(gpRegCHB_CVI->_r._f12) = (_v12);				\
									(gpRegCHB_CVI->_r._f13) = (_v13);				\
									(gpRegCHB_CVI->_r._f14) = (_v14);				\
								} while(0)

#define CHB_CVI_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
									(gpRegCHB_CVI->_r._f10) = (_v10);				\
									(gpRegCHB_CVI->_r._f11) = (_v11);				\
									(gpRegCHB_CVI->_r._f12) = (_v12);				\
									(gpRegCHB_CVI->_r._f13) = (_v13);				\
									(gpRegCHB_CVI->_r._f14) = (_v14);				\
									(gpRegCHB_CVI->_r._f15) = (_v15);				\
								} while(0)

#define CHB_CVI_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(gpRegCHB_CVI->_r._f01) = (_v01);				\
									(gpRegCHB_CVI->_r._f02) = (_v02);				\
									(gpRegCHB_CVI->_r._f03) = (_v03);				\
									(gpRegCHB_CVI->_r._f04) = (_v04);				\
									(gpRegCHB_CVI->_r._f05) = (_v05);				\
									(gpRegCHB_CVI->_r._f06) = (_v06);				\
									(gpRegCHB_CVI->_r._f07) = (_v07);				\
									(gpRegCHB_CVI->_r._f08) = (_v08);				\
									(gpRegCHB_CVI->_r._f09) = (_v09);				\
									(gpRegCHB_CVI->_r._f10) = (_v10);				\
									(gpRegCHB_CVI->_r._f11) = (_v11);				\
									(gpRegCHB_CVI->_r._f12) = (_v12);				\
									(gpRegCHB_CVI->_r._f13) = (_v13);				\
									(gpRegCHB_CVI->_r._f14) = (_v14);				\
									(gpRegCHB_CVI->_r._f15) = (_v15);				\
									(gpRegCHB_CVI->_r._f16) = (_v16);				\
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
 * _r	  : name of register
 * _lname : name of load  bit field : shall be 0 after macro executed.
 * _rwname: name of rw	  bit field : shall be 0 after CHB_CVI_Wind(), 1 for CHB_CVI_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * CHB_CVI_Rind : General indexed register Read.(
 * CHB_CVI_Wind : General indexed register Read.
 *
 * CHB_CVI_Ridx : For 'index', 'rw', 'load' field name
 * CHB_CVI_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define CHB_CVI_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								CHB_CVI_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								CHB_CVI_WrFL(_r);									\
								CHB_CVI_RdFL(_r);									\
								CHB_CVI_Rd01(_r,_fname,_fval);						\
							} while (0)

#define CHB_CVI_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				CHB_CVI_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define CHB_CVI_Ridx(_r, _ival, _fname, _fval)	CHB_CVI_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define CHB_CVI_Widx(_r, _ival, _fname, _fval)	CHB_CVI_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8-DE-CVI_RegFile_100105.csv' 20100311 00:54:15   대한민국 표준시 by getregs v2.3 */
