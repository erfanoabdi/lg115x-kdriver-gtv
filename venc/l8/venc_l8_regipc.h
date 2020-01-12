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


#ifndef _REG_VENC_L8_IPC_h
 #define _REG_VENC_L8_IPC_h


/*-----------------------------------------------------------------------------
	0x0000 VENC_REG0 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		ddr_jump                        : 1;	//    31
		UINT32		rom_jump                        : 1;	//    30
		UINT32		_rsvd_00                        : 6;	// 29:24
		UINT32		jump_offset                     :24;	// 23: 0
	}be;
	struct {
		UINT32		jump_offset                     :24;	// 23: 0
		UINT32		_rsvd_00                        : 6;	// 29:24
		UINT32		rom_jump                        : 1;	//    30
		UINT32		ddr_jump                        : 1;	//    31
	}le;
} VENC_REG0;

/*-----------------------------------------------------------------------------
	0x0004 VENC_REG1 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        :30;	// 31: 2
		UINT32		rw_mode                         : 2;	//  1: 0
	}be;
	struct {
		UINT32		rw_mode                         : 2;	//  1: 0
		UINT32		_rsvd_00                        :30;	// 31: 2
	}le;
} VENC_REG1;

/*-----------------------------------------------------------------------------
	0x0008 VENC_REG2 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		rw_address                      ;   	// 31: 0
	}be;
	struct {
		UINT32		rw_address                      ;   	// 31: 0
	}le;
} VENC_REG2;

/*-----------------------------------------------------------------------------
	0x000c VENC_REG3 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		w_data                          ;   	// 31: 0
	}be;
	struct {
		UINT32		w_data                          ;   	// 31: 0
	}le;
} VENC_REG3;

/*-----------------------------------------------------------------------------
	0x0010 VENC_REG4 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        :28;	// 31: 4
		UINT32		input_mode                      : 4;	//  3: 0
	}be;
	struct {
		UINT32		input_mode                      : 4;	//  3: 0
		UINT32		_rsvd_00                        :28;	// 31: 4
	}le;
} VENC_REG4;

/*-----------------------------------------------------------------------------
	0x0014 VENC_REG5 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        :22;	// 31: 10
		UINT32		yuyv                            : 2;	//  9: 8
		UINT32		color_format                    : 4;	//  7: 4
		UINT32		_rsvd_01                        : 2;	//  3: 2
		UINT32		source_type                     : 2;	//  1: 0
	}be;
	struct {
		UINT32		source_type                     : 2;	//  1: 0
		UINT32		_rsvd_01                        : 2;	//  3: 2
		UINT32		color_format                    : 4;	//  7: 4
		UINT32		yuyv                            : 2;	//  8: 9
		UINT32		_rsvd_00                        :22;	// 31: 10
	}le;
} VENC_REG5;

/*-----------------------------------------------------------------------------
	0x0018 VENC_REG6 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	debug                           ;   	// 31: 0
} VENC_REG6;

/*-----------------------------------------------------------------------------
	0x001c VENC_REG7 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	debug                           ;   	// 31: 0
} VENC_REG7;

#if 0
/*-----------------------------------------------------------------------------
	0x0020 VENC_REG8 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        :31;	// 31: 1
		UINT32		sw_reset                        : 1;	//     0
	}be;
	struct {
		UINT32		sw_reset                        : 1;	//     0
		UINT32		_rsvd_00                        :31;	// 31: 1
	}le;
} VENC_REG8;
#endif
/*-----------------------------------------------------------------------------
	0x0020 VENC_REG8 ''
------------------------------------------------------------------------------*/
typedef union  {
	struct {
		UINT32		scaler_base_addr                    ;   	// 31: 0
	}be;
	struct {
		UINT32		scaler_base_addr                    ;   	// 31: 0
	}le;
} VENC_REG8;

/*-----------------------------------------------------------------------------
	0x0024 VENC_REG9 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	debug                           ;   	// 31: 0
} VENC_REG9;

/*-----------------------------------------------------------------------------
	0x0028 VENC_REG10 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		invalid		                    : 4;	// 31:28
		UINT32		venc_reg59                      : 1;	//    27
		UINT32		venc_reg58                      : 1;	//    26
		UINT32		venc_reg57                      : 1;	//    25
		UINT32		venc_reg56                      : 1;	//    24
		UINT32		venc_reg55                      : 1;	//    23
		UINT32		venc_reg54                      : 1;	//    22
		UINT32		reserved_0                     	: 5;	// 21:17
		UINT32		venc_reg48                      : 1;	//	  16
		UINT32		venc_reg47                      : 1;	//    15
		UINT32		venc_reg46                      : 1;	//    14
		UINT32		venc_reg45                      : 1;	//    13
		UINT32		venc_reg44                      : 1;	//    12
		UINT32		reserved_1                      : 2;	// 11:10
		UINT32		venc_reg41                      : 1;	//     9
		UINT32		venc_reg40                      : 1;	//     8
		UINT32		reserved_2                      :8;	//  7: 0
	}be;
	struct {
		UINT32		reserved_2                      :8;	//  7: 0
		UINT32		venc_reg40                      : 1;	//     8
		UINT32		venc_reg41                      : 1;	//     9
		UINT32		reserved_1                      : 2;	// 11:10
		UINT32		venc_reg44                      : 1;	//    12
		UINT32		venc_reg45                      : 1;	//    13
		UINT32		venc_reg46                      : 1;	//    14
		UINT32		venc_reg47                      : 1;	//    15
		UINT32		venc_reg48                      : 1;	//	  16
		UINT32		reserved_0                     	: 5;	// 21:17
		UINT32		venc_reg54                      : 1;	//    22
		UINT32		venc_reg55                      : 1;	//    23
		UINT32		venc_reg56                      : 1;	//    24
		UINT32		venc_reg57                      : 1;	//    25
		UINT32		venc_reg58                      : 1;	//    26
		UINT32		venc_reg59                      : 1;	//    27
		UINT32		invalid		                    : 4;	// 31:28
	}le;
} VENC_REG10;

/*-----------------------------------------------------------------------------
	0x002c VENC_REG11 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		invalid		                    : 4;	// 31:28
		UINT32		venc_reg59                      : 1;	//    27
		UINT32		venc_reg58                      : 1;	//    26
		UINT32		venc_reg57                      : 1;	//    25
		UINT32		venc_reg56                      : 1;	//    24
		UINT32		venc_reg55                      : 1;	//    23
		UINT32		venc_reg54                      : 1;	//    22
		UINT32		reserved_0                     	: 5;	// 21:17
		UINT32		venc_reg48                      : 1;	//	  16
		UINT32		venc_reg47                      : 1;	//    15
		UINT32		venc_reg46                      : 1;	//    14
		UINT32		venc_reg45                      : 1;	//    13
		UINT32		venc_reg44                      : 1;	//    12
		UINT32		reserved_1                      : 2;	// 11:10
		UINT32		venc_reg41                      : 1;	//     9
		UINT32		venc_reg40                      : 1;	//     8
		UINT32		reserved_2                      :8;	//  7: 0
	}be;
	struct {
		UINT32		reserved_2                      :8;	//  7: 0
		UINT32		venc_reg40                      : 1;	//     8
		UINT32		venc_reg41                      : 1;	//     9
		UINT32		reserved_1                      : 2;	// 11:10
		UINT32		venc_reg44                      : 1;	//    12
		UINT32		venc_reg45                      : 1;	//    13
		UINT32		venc_reg46                      : 1;	//    14
		UINT32		venc_reg47                      : 1;	//    15
		UINT32		venc_reg48                      : 1;	//	  16
		UINT32		reserved_0                     	: 5;	// 21:17
		UINT32		venc_reg54                      : 1;	//    22
		UINT32		venc_reg55                      : 1;	//    23
		UINT32		venc_reg56                      : 1;	//    24
		UINT32		venc_reg57                      : 1;	//    25
		UINT32		venc_reg58                      : 1;	//    26
		UINT32		venc_reg59                      : 1;	//    27
		UINT32		invalid		                    : 4;	// 31:28
	}le;
} VENC_REG11;

/*-----------------------------------------------------------------------------
	0x0894 VENC_REG37 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        : 5;	// 31:27
		UINT32		width                           :11;	// 26:16
		UINT32		_rsvd_01                        : 5;	// 15:11
		UINT32		height                          :11;	// 10: 0
	}be;
	struct {
		UINT32		height                          :11;	// 10: 0
		UINT32		_rsvd_01                        : 5;	// 15:11
		UINT32		width                           :11;	// 26:16
		UINT32		_rsvd_00                        : 5;	// 31:27
	}le;
} VENC_REG37;

/*-----------------------------------------------------------------------------
	0x0898 VENC_REG38 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	rd_base_input                   ;   	// 31: 0
} VENC_REG38;

/*-----------------------------------------------------------------------------
	0x089c VENC_REG39 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	rd_end_input                    ;   	// 31: 0
} VENC_REG39;

/*-----------------------------------------------------------------------------
	0x08a0 VENC_REG40 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	wr_idx_input                    ;   	// 31: 0
} VENC_REG40;

/*-----------------------------------------------------------------------------
	0x08a4 VENC_REG41 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	rd_idx_input                    ;   	// 31: 0
} VENC_REG41;

/*-----------------------------------------------------------------------------
	0x08a8 VENC_REG42 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	offset_picsz_input              ;   	// 31: 0
} VENC_REG42;

/*-----------------------------------------------------------------------------
	0x08ac VENC_REG43 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32		abwStrideSize						: 11;
	UINT32		abwResereved						: 5;
	UINT32		hdr_sz_pic                      	: 16;   	// 31: 0
} VENC_REG43;


/*-----------------------------------------------------------------------------
	0x00b0 VENC_REG44 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        : 4;	// 31: 28
		UINT32		scaler_stride                   :12;	// 27: 16
		UINT32		_rsvd_01                        : 7;	// 15: 9
		UINT32		thumbnail_start                 : 1;	//  8: 8
		UINT32		_rsvd_02                        : 3;	//  7: 5
		UINT32		sw_reset                 		: 1;	//  4: 4
		UINT32		lock_inactive                   : 1;	//  3: 3
		UINT32		separate_cbcr                 	: 1;	//  2: 2
		UINT32		progressive                 	: 1;	//  1: 1
		UINT32		path                 			: 1;	//  0: 0
	}be;
	struct {
		UINT32		path                 			: 1;	//  0: 0
		UINT32		progressive                 	: 1;	//  1: 1
		UINT32		separate_cbcr                 	: 1;	//  2: 2
		UINT32		lock_inactive                   : 1;	//  3: 3
		UINT32		sw_reset                 		: 1;	//  4: 4
		UINT32		_rsvd_02                        : 3;	//  7: 5
		UINT32		thumbnail_start                 : 1;	//  8: 8
		UINT32		_rsvd_01                        : 7;	// 15: 9
		UINT32		scaler_stride                   :12;	// 27: 16
		UINT32		_rsvd_00                        : 4;	// 31: 28
	}le;
} VENC_REG44;

/*-----------------------------------------------------------------------------
	0x00b4 VENC_REG45 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        : 5;	// 31:27
		UINT32		height                          :11;	// 26:16
		UINT32		_rsvd_01                        : 5;	// 15:11
		UINT32		width                           :11;	// 10: 0
	}be;
	struct {
		UINT32		width                           :11;	// 10: 0
		UINT32		_rsvd_01                        : 5;	// 15:11
		UINT32		height                          :11;	// 26:16
		UINT32		_rsvd_00                        : 5;	// 31:27
	}le;
} VENC_REG45;

/*-----------------------------------------------------------------------------
	0x00b8 VENC_REG46 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        : 5;	// 31:27
		UINT32		height                          :11;	// 26:16
		UINT32		_rsvd_01                        : 5;	// 15:11
		UINT32		width                           :11;	// 10: 0
	}be;
	struct {
		UINT32		width                           :11;	// 10: 0
		UINT32		_rsvd_01                        : 5;	// 15:11
		UINT32		height                          :11;	// 26:16
		UINT32		_rsvd_00                        : 5;	// 31:27
	}le;
} VENC_REG46;

/*-----------------------------------------------------------------------------
	0x00bc VENC_REG47 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		in_base_addr                    ;   	// 31: 0
	}be;
	struct {
		UINT32		in_base_addr                    ;   	// 31: 0
	}le;
} VENC_REG47;


/*-----------------------------------------------------------------------------
	0x00c0 VENC_REG48 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		out_base_addr                    ;   	// 31: 0
	}be;
	struct {
		UINT32		out_base_addr                    ;   	// 31: 0
	}le;
} VENC_REG48;

/*-----------------------------------------------------------------------------
	0x08c4 VENC_REG49 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		in_base_addr_c                  ;   	// 31: 0
	}be;
	struct {
		UINT32		in_base_addr_c                  ;   	// 31: 0
	}le;
} VENC_REG49;


/*-----------------------------------------------------------------------------
	0x08d8 VENC_REG54 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		slice_counter                   :16;	// 31:16
		UINT32		slice_encoding_msec             :16;	// 15: 0
	}be;
	struct {
		UINT32		slice_encoding_msec             :16;	// 15: 0
		UINT32		slice_counter                   :16;	// 31:16
	}le;
} VENC_REG54;


/*-----------------------------------------------------------------------------
	0x00dc VENC_REG55 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		ext_esw_ptr                     ;   	// 31: 0
	}be;
	struct {
		UINT32		ext_esw_ptr                     ;   	// 31: 0
	}le;
} VENC_REG55;

/*-----------------------------------------------------------------------------
	0x00e0 VENC_REG56 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		ext_esr_ptr                     ;   	// 31: 0
	}be;
	struct {
		UINT32		ext_esr_ptr                     ;   	// 31: 0
	}le;
} VENC_REG56;


/*-----------------------------------------------------------------------------
	0x00e4 VENC_REG57 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        :28;	// 31: 4
		UINT32		enc_ctrl                        : 4;	//  3: 0
	}be;
	struct {
		UINT32		enc_ctrl                        : 4;	//  3: 0
		UINT32		_rsvd_00                        :28;	// 31: 4
	}le;
} VENC_REG57;

/*-----------------------------------------------------------------------------
	0x00e8 VENC_REG58 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		es_base_addr                    ;   	// 31: 0
	}be;
	struct {
		UINT32		es_base_addr                    ;   	// 31: 0
	}le;
} VENC_REG58;


/*-----------------------------------------------------------------------------
	0x00ec VENC_REG59 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		es_end_addr                     ;   	// 31: 0
	}be;
	struct {
		UINT32		es_end_addr                     ;   	// 31: 0
	}le;
} VENC_REG59;

#if 0
/*-----------------------------------------------------------------------------
	0x00ec VENC_REG59 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		es_len                          ;   	// 31: 0
	}be;
	struct {
		UINT32		es_len                          ;   	// 31: 0
	}le;
} VENC_REG59;
#endif /* #if 0 */

/*-----------------------------------------------------------------------------
	0x00f0 VENC_REG60 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		int_esw_ptr                         ;   	// 31: 0
	}be;
	struct {
		UINT32		int_esw_ptr                         ;   	// 31: 0
	}le;
} VENC_REG60;

/*-----------------------------------------------------------------------------
	0x1cf4 VENC_REG61 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		int_esr_ptr                         ;   	// 31: 0
	}be;
	struct {
		UINT32		int_esr_ptr                         ;   	// 31: 0
	}le;
} VENC_REG61;

/*-----------------------------------------------------------------------------
	0x00f8 VENC_REG62 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        :30;	// 31: 2
		UINT32		host_int_mode                   : 2;	//  1: 0
	}be;
	struct {
		UINT32		host_int_mode                   : 2;	//  1: 0
		UINT32		_rsvd_00                        :30;	// 31: 2
	}le;
} VENC_REG62;

/*-----------------------------------------------------------------------------
	0x00fc VENC_REG63 ''
------------------------------------------------------------------------------*/
typedef union  {
	UINT32	ui32Val;
	struct {
		UINT32		_rsvd_00                        :31;	// 31: 1
		UINT32		ptr_update                      : 1;	//     0
	}be;
	struct {
		UINT32		ptr_update                      : 1;	//     0
		UINT32		_rsvd_00                        :31;	// 31: 1
	}le;
} VENC_REG63;


typedef struct {
	VENC_REG0     venc_reg0;       // 0x0000
	VENC_REG1     venc_reg1;       // 0x0004
	VENC_REG2     venc_reg2;       // 0x0008
	VENC_REG3     venc_reg3;       // 0x000c
	VENC_REG4     venc_reg4;       // 0x0010
	VENC_REG5     venc_reg5;       // 0x0014
	VENC_REG6     venc_reg6;       // 0x0018
	VENC_REG7     venc_reg7;       // 0x001c
	VENC_REG8     venc_reg8;       // 0x0020
	VENC_REG9     venc_reg9;       // 0x0024
	VENC_REG10    venc_reg10;		// 0x0028 : ''
	VENC_REG11    venc_reg11;		// 0x002c : ''
#if 0
	UINT32        _rsvd_addr_01[  32];	// 0x0030 ~ 0x00ac
#else
	UINT32        _rsvd_addr_01[  25];	// 0x0030 ~ 0x00ac
	VENC_REG37                      	venc_reg37                      ;	// 0x0894 : ''
	VENC_REG38                      	venc_reg38                      ;	// 0x0898 : ''
	VENC_REG39                      	venc_reg39                      ;	// 0x089c : ''
	VENC_REG40                      	venc_reg40                      ;	// 0x08a0 : ''
	VENC_REG41                      	venc_reg41                      ;	// 0x08a4 : ''
	VENC_REG42                      	venc_reg42                      ;	// 0x08a8 : ''
	VENC_REG43                      	venc_reg43                      ;	// 0x08ac : ''
#endif /* #if 0 */
	VENC_REG44    venc_reg44;		// 0x00b0 : ''
	VENC_REG45    venc_reg45;		// 0x00b4 : ''
	VENC_REG46    venc_reg46;		// 0x00b8 : ''
	VENC_REG47    venc_reg47;		// 0x00bc : ''
	VENC_REG48    venc_reg48;	    // 0x00c0 : ''
	VENC_REG49    venc_reg49;		// 0x08c4 : ''
	UINT32        _rsvd_addr_02[  2];	// 0x00c4 ~ 0x00e0
	UINT32		  venc_reg52;		// 0x08D0
	UINT32		  venc_reg53;		// 0x08D4
	VENC_REG54    venc_reg54;		// 0x08d8 : ''
	VENC_REG55    venc_reg55;		// 0x00dc : ''
	VENC_REG56    venc_reg56;		// 0x00e0 : ''
	VENC_REG57    venc_reg57;	    // 0x00e4 : ''
	VENC_REG58    venc_reg58;	    // 0x00e8 : ''
	VENC_REG59    venc_reg59;	    // 0x00ec : ''
	VENC_REG60    venc_reg60;	    // 0x00f0 : ''
	VENC_REG61    venc_reg61;	    // 0x1cf4 : ''
	VENC_REG62    venc_reg62;	    // 0x00f8 : ''
	VENC_REG63    venc_reg63;	    // 0x00fc : ''
} VENC_REG_IPC_T;

/******************************************************************************
 * Exported variables
 ******************************************************************************/
//extern volatile VENC_REG_IPC_T *gpVencIpcReg;


#endif	/* !_REG_VENC_IPC_h */

