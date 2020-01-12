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
	0x0010 ctr00 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	core_clk_div_sel				: 2,	//	0: 1
	peri_apb1_clk_5_off				: 1,	//	   2
	peri_apb1_clk_4_off				: 1,	//	   3
	peri_apb1_clk_3_off				: 1,	//	   4
	peri_apb1_clk_2_off				: 1,	//	   5
	peri_apb1_clk_1_off				: 1,	//	   6
	peri_apb1_clk_0_off				: 1,	//	   7
	peri_apb0_clk_7_off				: 1,	//	   8
	peri_apb0_clk_6_off				: 1,	//	   9
	peri_apb0_clk_5_off				: 1,	//	  10
	peri_apb0_clk_4_off				: 1,	//	  11
	peri_apb0_clk_3_off				: 1,	//	  12
	peri_apb0_clk_2_off				: 1,	//	  13
	peri_apb0_clk_1_off				: 1,	//	  14
	peri_apb0_clk_0_off				: 1,	//	  15
	peri_ahb_clk_30_off				: 1,	//	  16
	peri_ahb_clk_28_off				: 1,	//	  17
	peri_ahb_clk_26_off				: 1,	//	  18
	peri_ahb_clk_25_off				: 1,	//	  19
	peri_ahb_clk_24_off				: 1,	//	  20
	peri_ahb_clk_22_off				: 1,	//	  21
	peri_ahb_clk_19_off				: 1,	//	  22
	peri_ahb_clk_14_off				: 1,	//	  23
	peri_ahb_clk_9_off				: 1,	//	  24
	peri_ahb_clk_8_off				: 1,	//	  25
	peri_ahb_clk_7_off				: 1,	//	  26
	peri_ahb_clk_6_off				: 1,	//	  27
	peri_ahb_clk_5_off				: 1,	//	  28
	peri_ahb_clk_4_off				: 1,	//	  29
	peri_ahb_clk_3_off				: 1,	//	  30
	peri_ahb_clk_2_off				: 1;	//	  31
} CTR00;

/*-----------------------------------------------------------------------------
	0x0014 ctr01 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:21,	//	0:20 reserved
	peri_ahb_clk_1_off				: 1,	//	  21
	peri_ahb_clk_0_off				: 1,	//	  22
	peri_apb1_clk_14_off			: 1,	//	  23
	peri_apb1_clk_13_off			: 1,	//	  24
	peri_apb1_clk_12_off			: 1,	//	  25
	peri_apb1_clk_11_off			: 1,	//	  26
	peri_apb1_clk_10_off			: 1,	//	  27
	peri_apb1_clk_9_off				: 1,	//	  28
	peri_apb1_clk_8_off				: 1,	//	  29
	peri_apb1_clk_7_off				: 1,	//	  30
	peri_apb1_clk_6_off				: 1;	//	  31
} CTR01;

/*-----------------------------------------------------------------------------
	0x0018 ctr02 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:12,	//	0:11 reserved
	peri_ahb_clk_29_sel				: 3,	// 12:14
	psel_l2_sel						: 1,	//	  15
	bus_clk_4_sel					: 2,	// 16:17
	pclken_sel						: 2,	// 18:19
	bus_clk_1_sel					: 2,	// 20:21
	peri_ahb_clk_27_sel				: 3,	// 22:24
	peri_ahb_clk_23_off				: 1,	//	  25
	peri_ahb_clk_21_sel				: 3,	// 26:28
	peri_ahb_clk_20_sel				: 3;	// 29:31
} CTR02;

/*-----------------------------------------------------------------------------
	0x001c ctr03 ''
------------------------------------------------------------------------------*/
/*	no field */

// L8B0 add
typedef struct {
	UINT32
	uart2_sel               : 2,   //  0: 1
	uart1_sel               : 2,   //  2: 3
	uart0_sel               : 2,   //  4: 5
	                        : 1,   //     6 reserved
	rx_sel_de               : 1,   //     7
	rx_sel_led              : 1,   //     8
	rx_sel_sp               : 2,   //  9:10
	rx_sel_cpu0             : 1,   //    11
	                        : 3,   // 12:14 reserved
	rst_mm1_phy_n           : 1,   //    15
	                        : 7,   // 16:22 reserved
	rst_mm0_phy_n           : 1,   //    23
	                        : 7,   // 24:30 reserved
	rst_frc_m_phy_n         : 1;   //    31
} CTR03;

/*-----------------------------------------------------------------------------
	0x0020 ctr04 ''
------------------------------------------------------------------------------*/
/*	no field */

// L8B0 add
typedef struct {
	UINT32
	mip_mon                     ;  // 31: 0
} CTR04;

/*-----------------------------------------------------------------------------
	0x0024 ctr05 ''
------------------------------------------------------------------------------*/
/*	no field */

// L8B0 add
typedef struct {
	UINT32
	flip_en_0               : 1,   //     0
	flip_en_1               : 1,   //     1
	flip_en_2               : 1,   //     2
	flip_en_3               : 1,   //     3
	flip_en_4               : 1,   //     4
	flip_en_5               : 1,   //     5
	flip_en_6               : 1,   //     6
	flip_en_7               : 1,   //     7
	flip_en_8               : 1,   //     8
	flip_en_9               : 1,   //     9
	flip_en_10              : 1,   //    10
	flip_en_11              : 1,   //    11
	flip_en_12              : 1,   //    12
	flip_en_13              : 1,   //    13
	flip_en_14              : 1,   //    14
	flip_en_15              : 1,   //    15
	flip_en_16              : 1,   //    16
	flip_en_17              : 1,   //    17
	flip_en_18              : 1,   //    18
	flip_en_19              : 1,   //    19
	flip_en_20              : 1,   //    20
	flip_en_21              : 1,   //    21
	flip_en_22              : 1,   //    22
	flip_en_23              : 1,   //    23
	                        : 7,   // 24:30 reserved
	tun_i2c_direct_en       : 1;   //    31
} CTR05;

/*-----------------------------------------------------------------------------
	0x0028 ctr06 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	peri_apb0_rstn_1				: 1,	//	   0
	peri_apb0_rstn_0				: 1,	//	   1
	bus_rstn_8						: 1,	//	   2
	peri_ahb_rstn_27				: 1,	//	   3
	peri_ahb_rstn_26				: 1,	//	   4
	peri_ahb_rstn_25				: 1,	//	   5
	peri_ahb_rstn_24				: 1,	//	   6
	peri_ahb_rstn_23				: 1,	//	   7
	peri_ahb_rstn_22				: 1,	//	   8
	peri_ahb_rstn_21				: 1,	//	   9
	peri_ahb_rstn_20				: 1,	//	  10
	peri_ahb_rstn_19				: 1,	//	  11
	peri_ahb_rstn_18				: 1,	//	  12
	peri_ahb_rstn_17				: 1,	//	  13
	peri_ahb_rstn_16				: 1,	//	  14
	peri_ahb_rstn_15				: 1,	//	  15
	peri_ahb_rstn_14				: 1,	//	  16
	peri_ahb_rstn_13				: 1,	//	  17
	peri_ahb_rstn_12				: 1,	//	  18
	peri_ahb_rstn_11				: 1,	//	  19
	peri_ahb_rstn_10				: 1,	//	  20
	peri_ahb_rstn_9					: 1,	//	  21
	peri_ahb_rstn_8					: 1,	//	  22
	peri_ahb_rstn_7					: 1,	//	  23
	peri_ahb_rstn_6					: 1,	//	  24
	peri_ahb_rstn_5					: 1,	//	  25
	peri_ahb_rstn_4					: 1,	//	  26
	peri_ahb_rstn_3					: 1,	//	  27
	peri_ahb_rstn_2					: 1,	//	  28
	bus_rstn_7						: 1,	//	  29
	bus_rstn_6						: 1,	//	  30
	bus_rstn_5						: 1;	//	  31
} CTR06;

/*-----------------------------------------------------------------------------
	0x002c ctr07 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 3,	//	0: 2 reserved
	bus_rstn_4						: 1,	//	   3
	bus_rstn_3						: 1,	//	   4
	bus_rstn_2						: 1,	//	   5
	bus_rstn_1						: 1,	//	   6
	bus_rstn_0						: 1,	//	   7
	peri_ahb_rstn_1					: 1,	//	   8
	peri_ahb_rstn_0					: 1,	//	   9
	peri_apb1_rstn_14				: 1,	//	  10
	peri_apb1_rstn_13				: 1,	//	  11
	peri_apb1_rstn_12				: 1,	//	  12
	peri_apb1_rstn_11				: 1,	//	  13
	peri_apb1_rstn_10				: 1,	//	  14
	peri_apb1_rstn_9				: 1,	//	  15
	peri_apb1_rstn_8				: 1,	//	  16
	peri_apb1_rstn_7				: 1,	//	  17
	peri_apb1_rstn_6				: 1,	//	  18
	peri_apb1_rstn_5				: 1,	//	  19
	peri_apb1_rstn_4				: 1,	//	  20
	peri_apb1_rstn_3				: 1,	//	  21
	peri_apb1_rstn_2				: 1,	//	  22
	peri_apb1_rstn_1				: 1,	//	  23
	peri_apb1_rstn_0				: 1,	//	  24
	bus_rstn_9						: 1,	//	  25
	peri_apb0_rstn_7				: 1,	//	  26
	peri_apb0_rstn_6				: 1,	//	  27
	peri_apb0_rstn_5				: 1,	//	  28
	peri_apb0_rstn_4				: 1,	//	  29
	peri_apb0_rstn_3				: 1,	//	  30
	peri_apb0_rstn_2				: 1;	//	  31
} CTR07;

/*-----------------------------------------------------------------------------
	0x0030 ctr08 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	bt_usb_por_ctr					: 1,	//	   0
	usb_por_ctr						: 1,	//	   1
	peri_ahb_rstn_37				: 1,	//	   2
	peri_ahb_rstn_36				: 1,	//	   3
	peri_ahb_rstn_35				: 1,	//	   4
	peri_ahb_rstn_34				: 1,	//	   5
	peri_ahb_rstn_33				: 1,	//	   6
	peri_ahb_rstn_32				: 1,	//	   7
	peri_ahb_rstn_31				: 1,	//	   8
	peri_ahb_rstn_30				: 1,	//	   9
									:12,	// 10:21 reserved
	peri_ahb_rstn_29				: 1,	//	  22
	peri_ahb_rstn_28				: 1,	//	  23
									: 3,	// 24:26 reserved
	wdreset_n						: 1,	//	  27
	poreset_n						: 1,	//	  28
	cpureset_n						: 1,	//	  29
	reset_l2_n						: 1,	//	  30
	scurst_n						: 1;	//	  31
} CTR08;

/*-----------------------------------------------------------------------------
	0x0034 ctr09 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 2,	//	0: 1 reserved
	frc_tcon_apb_sel				: 2,	//	2: 3
	led_cpuclk_off					: 1,	//	   4
	led_cpu_clk						: 2,	//	5: 6
	vdclk_sel						: 1,	//	   7
	vdclk_off						: 1,	//	   8
	gpuclk_sel						: 2,	//	9:10
	gpuclk_off						: 1,	//	  11
	tcon_aclk_off					: 1,	//	  12
	vp_aclk_off						: 1,	//	  13
	frc_aclk_off					: 1,	//	  14
	de_aclk_off						: 1,	//	  15
	aud_dsp1aclk_off				: 1,	//	  16
	aud_dsp0aclk_off				: 1,	//	  17
	aud_aclk_off					: 1,	//	  18
	vd_aclk_off						: 1,	//	  19
	ve_aclk_off						: 1,	//	  20
	te_aclk_off						: 1,	//	  21
	gfx_aclk_off					: 1,	//	  22
	gpu_aclk_off					: 1,	//	  23
	coreclock_sel					: 2,	// 24:25
	frc_mclk_sel					: 2,	// 26:27
	m_mclk_sel						: 2,	// 28:29
	frcclk_sel						: 1,	//	  30
	pll_sel							: 1;	//	  31
} CTR09;

/*-----------------------------------------------------------------------------
	0x0038 ctr10 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:22,	//	0:21 reserved
	vpclk_off						: 1,	//	  22
	gfxclk_off						: 1,	//	  23
	teclk_off						: 1,	//	  24
	frcclk_off						: 1,	//	  25
	de270clk_off					: 1,	//	  26
	ve270clk_off					: 1,	//	  27
	te_ah270clk_off					: 1,	//	  28
	declk_off						: 1,	//	  29
	veclk_off						: 1,	//	  30
	te_ahclk_off					: 1;	//	  31
} CTR10;

/*-----------------------------------------------------------------------------
	0x003c ctr11 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 1,	//	   0 reserved
	rst_frc_n						: 1,	//	   1
	rst_de_vd_n						: 1,	//	   2
	rst_de_de_n						: 1,	//	   3
	rst_gfx_n						: 1,	//	   4
	rst_ve_n						: 1,	//	   5
	rst_teah_n						: 1,	//	   6
	rst_te_te_n						: 1,	//	   7
	rst_te_ve_n						: 1,	//	   8
	rst_te_vd_n						: 1,	//	   9
	rst_vd_n						: 1,	//	  10
	rst_tcona_n						: 1,	//	  11
	rst_vpa_n						: 1,	//	  12
	rst_frca_n						: 1,	//	  13
	rst_dea_n						: 1,	//	  14
	rst_vda_n						: 1,	//	  15
	rst_gpu_n						: 1,	//	  16
	rst_vea_n						: 1,	//	  17
	rst_tea_n						: 1,	//	  18
	rst_gfxa_n						: 1,	//	  19
	rst_gpua_n						: 1,	//	  20
	rst_apb_frc_n					: 1,	//	  21
	rst_apb_gfx_n					: 1,	//	  22
	rst_apb_gpu_n					: 1,	//	  23
	rst_apb_de_n					: 1,	//	  24
	rst_apb_te_n					: 1,	//	  25
	rst_apb_ve_n					: 1,	//	  26
	rst_apb_vd_n					: 1,	//	  27
	rst_apb_n						: 1,	//	  28
	rst_axi_n						: 1,	//	  29
	rst_frc_m_n						: 1,	//	  30
	rst_mm_n						: 1;	//	  31
} CTR11;

/*-----------------------------------------------------------------------------
	0x0040 ctr12 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	rst_frc_m_half_n				: 1,	//	   0
	rst_mm_half_n					: 1,	//	   1
	rst_axi2_n_10					: 1,	//	   2
	rst_axi2_n_9					: 1,	//	   3
	rst_axi2_n_8					: 1,	//	   4
	rst_axi2_n_7					: 1,	//	   5
	rst_axi2_n_6					: 1,	//	   6
	rst_axi2_n_5					: 1,	//	   7
	rst_axi2_n_4					: 1,	//	   8
	rst_axi2_n_3					: 1,	//	   9
	rst_axi2_n_2					: 1,	//	  10
	rst_axi2_n_1					: 1,	//	  11
	rst_axi1_n_10					: 1,	//	  12
	rst_axi1_n_9					: 1,	//	  13
	rst_axi1_n_8					: 1,	//	  14
	rst_axi1_n_7					: 1,	//	  15
	rst_axi1_n_6					: 1,	//	  16
	rst_axi1_n_5					: 1,	//	  17
	rst_axi1_n_4					: 1,	//	  18
	rst_axi1_n_3					: 1,	//	  19
	rst_axi1_n_2					: 1,	//	  20
	rst_axi1_n_1					: 1,	//	  21
	rst_axi0_n_10					: 1,	//	  22
	rst_axi0_n_9					: 1,	//	  23
	rst_axi0_n_8					: 1,	//	  24
	rst_axi0_n_7					: 1,	//	  25
	rst_axi0_n_6					: 1,	//	  26
	rst_axi0_n_5					: 1,	//	  27
	rst_axi0_n_4					: 1,	//	  28
	rst_axi0_n_3					: 1,	//	  29
	rst_axi0_n_2					: 1,	//	  30
	rst_axi0_n_1					: 1;	//	  31
} CTR12;

/*-----------------------------------------------------------------------------
	0x0044 ctr13 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	rst_vp_n						: 1,	//	   0
	rst_apb_tcon_n					: 1,	//	   1
	rst_apb_oif_n					: 1,	//	   2
	rst_apb_led_n					: 1,	//	   3
	rst_led_cpu_n					: 1,	//	   4
	rst_tcon_pix_n					: 1,	//	   5
	rst_frc_pix_n					: 1,	//	   6
	rst_led_pix_n					: 1,	//	   7
	rst_oif_pix_n					: 1,	//	   8
	lvds_pix_clk					: 1,	//	   9
	tcon_pix_clk_off				: 1,	//	  10
	tcon_pix_clk_sel				: 1,	//	  11
	frc_pix_clk_off					: 1,	//	  12
	led_pix_clk_off					: 1,	//	  13
	oif_pix_clk_sel					: 1,	//	  14
	dispclk_sel						: 1,	//	  15
	rst_dea_dp_n					: 1,	//	  16
	rst_disp_dp_n					: 1,	//	  17
	rst_de_dp_n						: 1,	//	  18
	rst_se_n						: 1,	//	  19
	rst_s_te_n						: 1,	//	  20
	rst_s_de_n						: 1,	//	  21
									: 1,	//	  22 reserved
	rst_disp_frc_n					: 1,	//	  23
	rst_disp_de_n					: 1,	//	  24
	rst_chb_n						: 1,	//	  25
	rst_cha_n						: 1,	//	  26
	chbclk_sel						: 2,	// 27:28
	chaclk_sel						: 2,	// 29:30
	dco_sel							: 1;	//	  31
} CTR13;

/*-----------------------------------------------------------------------------
	0x0048 ctr14 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	adto_loop_val					:16;	//	0:15
} CTR14;

/*-----------------------------------------------------------------------------
	0x004c ctr15 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	adto_add_val					:29;	//	0:28
} CTR15;

/*-----------------------------------------------------------------------------
	0x0050 ctr16 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	adto_err_val					: 6;	//	0: 5
} CTR16;

/*-----------------------------------------------------------------------------
	0x0054 ctr17 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:14,	//	0:13 reserved
	audio_sel8						: 2,	// 14:15
	audio_sel7						: 2,	// 16:17
	audio_sel6						: 2,	// 18:19
	audio_sel5						: 2,	// 20:21
	audio_sel4						: 2,	// 22:23
	audio_sel3						: 2,	// 24:25
	audio_sel2						: 2,	// 26:27
	audio_sel1						: 2,	// 28:29
	audio_sel0						: 2;	// 30:31
} CTR17;

/*-----------------------------------------------------------------------------
	0x0058 ctr18 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fs23clk_lsb_sel					: 1,	//	   0
	ext_en							: 1,	//	   1
									: 3,	//	2: 4 reserved
	fs23clk_sel						: 2,	//	5: 6
	fs22clk_sel						: 3,	//	7: 9
	fs21clk_sel						: 3,	// 10:12
	fs20clk_sel						: 3,	// 13:15
	fs12clk_sel						: 3,	// 16:18
	fs11clk_sel						: 3,	// 19:21
	fs10clk_sel						: 4,	// 22:25
	fs04clk_sel						: 3,	// 26:28
	fs03clk_sel						: 3;	// 29:31
} CTR18;

/*-----------------------------------------------------------------------------
	0x005c ctr19 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	de_mon							;		// 31: 0
} CTR19;

/*-----------------------------------------------------------------------------
	0x0060 ctr20 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cpu_gpu_mon						;		// 31: 0
} CTR20;

/*-----------------------------------------------------------------------------
	0x0064 ctr21 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	frc_mon_0						:16,	//	0:15
	gfx_mon							:16;	// 16:31
} CTR21;

/*-----------------------------------------------------------------------------
	0x0068 ctr22 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	aud_mon							;		// 31: 0
} CTR22;

/*-----------------------------------------------------------------------------
	0x006c ctr23 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	aad_mon							:22;	//	0:21
} CTR23;

/*-----------------------------------------------------------------------------
	0x0070 ctr24 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	gpio7_en						: 8,	//	0: 7
	gpio6_en						: 8,	//	8:15
	gpio5_en						: 8,	// 16:23
	gpio4_en						: 8;	// 24:31
} CTR24;

/*-----------------------------------------------------------------------------
	0x0074 ctr25 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 1,	//	   0 reserved
	gpio11_en						: 7,	//	1: 7
	gpio10_en						: 8,	//	8:15
	gpio9_en						: 8,	// 16:23
	gpio8_en						: 8;	// 24:31
} CTR25;

/*-----------------------------------------------------------------------------
	0x0078 ctr26 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cvbsout_en						:10,	//	0: 9
	sdhc_pull_ctr					: 1,	//	  10
									: 4,	// 11:14 reserved
	i2c_sel							: 1,	//	  15
	frc_mon_1						:16;	// 16:31
} CTR26;

/*-----------------------------------------------------------------------------
	0x007c ctr27 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 2,	//	0: 1 reserved
	ddc_ck_pull_ctr					: 1,	//	   2
	ddc_da_pull_ctr					: 1,	//	   3
	tun_pull_ctr					: 1,	//	   4
	led_pull_ctr					: 1,	//	   5
	frame_lr_pull_ctr				: 1,	//	   6
	lcd_pull_ctr					: 1,	//	   7
									: 1,	//	   8 reserved
	cam_pull_ctr					: 1,	//	   9
	eb_data_pull_ctr1				: 1,	//	  10
	eb_data_pull_ctr0				: 1,	//	  11
	eb_cs_pull_ctr					: 1,	//	  12
	nand_pull_ctr					: 1,	//	  13
	rmii_pull_ctr					: 1,	//	  14
	ext_intr_pull_ctr				: 1,	//	  15
	pwm_pull_ctr					: 1,	//	  16
	i2c_pull_ctr					: 1,	//	  17
	uart_pull_ctr					: 1,	//	  18
	smc_pull_ctr					: 1,	//	  19
	audio_pull_ctr2					: 1,	//	  20
	audio_pull_ctr1					: 1,	//	  21
	audio_pull_ctr0					: 1,	//	  22
									: 1,	//	  23 reserved
	spi1_pull_ctr					: 1,	//	  24
	spi0_pull_ctr					: 1,	//	  25
	tpo_data_ctr					: 1,	//	  26
	tpo_pull_ctr					: 1,	//	  27
	tpi_data_ctr					: 1,	//	  28
	tpi_pull_ctr					: 1,	//	  29
	spto_pull_ctr					: 1,	//	  30
	stpi_pull_ctr					: 1;	//	  31
} CTR27;

/*-----------------------------------------------------------------------------
	0x0080 ctr28 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0084 ctr29 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0088 ctr30 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x008c ctr31 ''
------------------------------------------------------------------------------*/
/*	no field */

/*-----------------------------------------------------------------------------
	0x0090 ctr32 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	en								:24,	//	0:23
	sw_4sd_0_135_l_0_off			: 1,	//	  24
	rmlvds_en						: 1,	//	  25
	mini_flip_en					: 1,	//	  26
	mini_en							: 1,	//	  27
	sw_4sd_0_135_l_1_off			: 1,	//	  28
	sw_4sd_0_135_l_2_off			: 1,	//	  29
	rf								: 1,	//	  30
	pdb								: 1;	//	  31
} CTR32;

/*-----------------------------------------------------------------------------
	0x0094 ctr33 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	enpemp_h						: 8,	//	0: 7
	rs								:24;	//	8:31
} CTR33;

/*-----------------------------------------------------------------------------
	0x0098 ctr34 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ovdrv							:16,	//	0:15
	enpemp_l						:16;	// 16:31
} CTR34;

/*-----------------------------------------------------------------------------
	0x009c ctr35 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	vsb_inck_sel					: 1,	//	   0
									: 3,	//	1: 3 reserved
	aad_dco_lpfon					: 2,	//	4: 5
	aad_dco_fcw						:22,	//	6:27
	aad_dco_resetb					: 1,	//	  28
	aad_dco_rtest					: 1,	//	  29
	aad_adc_sifsel					: 1,	//	  30
	aad_adc_pdb						: 1;	//	  31
} CTR35;

/*-----------------------------------------------------------------------------
	0x00a0 ctr36 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 6,	//	0: 5 reserved
	aad_btscclk_sel					: 1,	//	   6
	aad_fs02clk_sel					: 2,	//	7: 8
	aad_fs01clk_sel					: 2,	//	9:10
	aad_fs00clk_sel					: 1,	//	  11
	aad_adcclk_test					: 1,	//	  12
	aad_spllclk_test				: 1,	//	  13
	aad_spll_pdb					: 1,	//	  14
	aad_spll_sel					: 1,	//	  15
	aad_spll_cvs					: 2,	// 16:17
	aad_spll_cvl					: 2,	// 18:19
	aad_spll_ci						: 2,	// 20:21
	aad_spll_od						: 2,	// 22:23
	aad_spll_n						: 6,	// 24:29
	aad_spll_m						: 2;	// 30:31
} CTR36;

/*-----------------------------------------------------------------------------
	0x00a4 ctr37 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	hp_test1						: 2,	//	0: 1
	hp_slop_dig						: 3,	//	2: 4
	auda_bci						: 3,	//	5: 7
	aud_lin_drv_ch_mute				: 1,	//	   8
	aud_pdb_ldrv_in					: 1,	//	   9
	auad_rec_ch_mute				: 1,	//	  10
	auad_pdb_in						: 1,	//	  11
	aud_lindrv0_ch_sel				: 3,	// 12:14
	aud_lindrv1_ch_sel				: 3,	// 15:17
	auad_dc_sel						: 1,	//	  18
	auad_ch_sel						: 3,	// 19:21
	auad_gcon						: 4,	// 22:25
	auad_ccon						: 3,	// 26:28
	auda_pdb						: 3;	// 29:31
} CTR37;

/*-----------------------------------------------------------------------------
	0x00a8 ctr38 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	ovdrv							: 8,	//	0: 7
	hp_pdb							: 1,	//	   8
	hp_ssp_n						: 8,	//	9:16
	hp_ssp_m						: 2,	// 17:18
	hp_ssp_od						: 2,	// 19:20
	hp_ssp_ms						: 3,	// 21:23
	hp_ssp_sr						: 4,	// 24:27
	hp_ssp_cih						: 3,	// 28:30
	hp_ssp_cv						: 1;	//	  31
} CTR38;

/*-----------------------------------------------------------------------------
	0x00ac ctr39 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sw_4sd_90_45_r_0_off			: 1,	//	   0
	sw_4sd_90_45_r_1_off			: 1,	//	   1
	sw_4sd_90_45_r_2_off			: 1,	//	   2
	sw_4sd_90_45_l_0_off			: 1,	//	   3
	sw_4sd_90_45_l_1_off			: 1,	//	   4
	sw_4sd_90_45_l_2_off			: 1,	//	   5
	sw_4sd_0_135_r_0_off			: 1,	//	   6
	sw_4sd_0_135_r_1_off			: 1,	//	   7
	sw_4sd_0_135_r_2_off			: 1,	//	   8
	sel_90_45_r						: 1,	//	   9
	sel_90_45_l						: 1,	//	  10
	sel_0_135_r						: 1,	//	  11
	sel_0_135_l						: 1,	//	  12
	mini_en135d						: 1,	//	  13
	mini_en90d						: 1,	//	  14
	mini_en45d						: 1,	//	  15
	mini_en0d						: 1,	//	  16
									: 1,	//	  17 reserved
	aumi_pop						: 1,	//	  18
	aumi_pdb_in						: 1,	//	  19
	aumi_ccon						: 3,	// 20:22
	aumi_cont						: 4,	// 23:26
	aumi_cnt						: 5;	// 27:31
} CTR39;

/*-----------------------------------------------------------------------------
	0x00b0 ctr40 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 8,	//	0: 7 reserved
	out_sel							: 3,	//	8:10
	sgm_zero_sel					: 3,	// 11:13
	test1							: 3,	// 14:16
	test0							: 3,	// 17:19
	dif1							: 3,	// 20:22
	dif0							: 3,	// 23:25
	dem0							: 3,	// 26:28
	tdi								: 3;	// 29:31
} CTR40;

/*-----------------------------------------------------------------------------
	0x00b4 ctr41 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:23,	//	0:22 reserved
	pwm_sel							: 1,	//	  23
	dif								: 2,	// 24:25
	dat_hp_vol_con					: 6;	// 26:31
} CTR41;

/*-----------------------------------------------------------------------------
	0x00b8 ctr42 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:14,	//	0:13 reserved
	testinr							: 1,	//	  14
	testinl							: 1,	//	  15
	testr							: 1,	//	  16
	testl							: 1,	//	  17
	dig_sel							: 2,	// 18:19
	vol_gain						: 6,	// 20:25
	bm_20							: 1,	//	  26
	pol_con							: 2,	// 27:28
	cont							: 3;	// 29:31
} CTR42;

/*-----------------------------------------------------------------------------
	0x00bc ctr43 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:16,	//	0:15 reserved
	testin_mic						: 1,	//	  16
	test_mic						: 1,	//	  17
	dig_sel_mic						: 2,	// 18:19
	vol_gain_mic					: 6,	// 20:25
	bm_20_mic						: 1,	//	  26
	pol_con_mic						: 2,	// 27:28
	cont_mic						: 3;	// 29:31
} CTR43;

/*-----------------------------------------------------------------------------
	0x00c0 ctr44 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dr3p_dtrsel						: 3,	//	0: 2
	dr3p_od_cvdclk					: 3,	//	3: 5
	dr3p_nsc						: 4,	//	6: 9
	dr3p_npc						: 6,	// 10:15
	dr3p_msex						: 9,	// 16:24
	dr3p_m							: 3,	// 25:27
	dr3p_lf							: 1,	//	  28
	dr3p_cih						: 3;	// 29:31
} CTR44;

/*-----------------------------------------------------------------------------
	0x00c4 ctr45 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:21,	//	0:20 reserved
	dr3p_udex						: 5,	// 21:25
	dr3p_ps23c						: 1,	//	  26
	dr3p_pdb						: 1,	//	  27
	dr3p_od_recclk					: 4;	// 28:31
} CTR45;

/*-----------------------------------------------------------------------------
	0x00c8 ctr46 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dr3p_dtrsel						: 3,	//	0: 2
	dr3p_od_cvdclk					: 3,	//	3: 5
	dr3p_nsc						: 4,	//	6: 9
	dr3p_npc						: 6,	// 10:15
	dr3p_msex						: 9,	// 16:24
	dr3p_m							: 3,	// 25:27
	dr3p_lf							: 1,	//	  28
	dr3p_cih						: 3;	// 29:31
} CTR46;

/*-----------------------------------------------------------------------------
	0x00cc ctr47 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:21,	//	0:20 reserved
	dr3p_udex						: 5,	// 21:25
	dr3p_ps23c						: 1,	//	  26
	dr3p_pdb						: 1,	//	  27
	dr3p_od_recclk					: 4;	// 28:31
} CTR47;

/*-----------------------------------------------------------------------------
	0x00d0 ctr48 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dr3p_dtrsel						: 3,	//	0: 2
	dr3p_od_cvdclk					: 3,	//	3: 5
	dr3p_nsc						: 4,	//	6: 9
	dr3p_npc						: 6,	// 10:15
	dr3p_msex						: 9,	// 16:24
	dr3p_m							: 3,	// 25:27
	dr3p_lf							: 1,	//	  28
	dr3p_cih						: 3;	// 29:31
} CTR48;

/*-----------------------------------------------------------------------------
	0x00d4 ctr49 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:21,	//	0:20 reserved
	dr3p_udex						: 5,	// 21:25
	dr3p_ps23c						: 1,	//	  26
	dr3p_pdb						: 1,	//	  27
	dr3p_od_recclk					: 4;	// 28:31
} CTR49;

/*-----------------------------------------------------------------------------
	0x00d8 ctr50 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 4,	//	0: 3 reserved
	dpll_n							:13,	//	4:16
	dpll_m							: 9,	// 17:25
	dpll_mode						: 4,	// 26:29
	dpll_en_edge					: 1,	//	  30
	dpll_mode_en					: 1;	//	  31
} CTR50;

/*-----------------------------------------------------------------------------
	0x00dc ctr51 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:18,	//	0:17 reserved
	dpll_cvs						: 2,	// 18:19
	dpll_cvl						: 2,	// 20:21
	dpll_ci							: 2,	// 22:23
	dpll_pdb						: 1,	//	  24
	dpll_sel						: 1,	//	  25
	dpll_od							: 6;	// 26:31
} CTR51;

/*-----------------------------------------------------------------------------
	0x00e0 ctr52 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 4,	//	0: 3 reserved
	dpll_n							:13,	//	4:16
	dpll_m							: 9,	// 17:25
	dpll_mode						: 4,	// 26:29
	dpll_en_edge					: 1,	//	  30
	dpll_mode_en					: 1;	//	  31
} CTR52;

/*-----------------------------------------------------------------------------
	0x00e4 ctr53 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									:18,	//	0:17 reserved
	dpll_cvs						: 2,	// 18:19
	dpll_cvl						: 2,	// 20:21
	dpll_ci							: 2,	// 22:23
	dpll_pdb						: 1,	//	  24
	dpll_sel						: 1,	//	  25
	dpll_od							: 6;	// 26:31
} CTR53;

/*-----------------------------------------------------------------------------
	0x00e8 ctr54 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 9,	// 0: 8 reserved
	disp_bypass 						: 1,	// 9 : disp_bypass
	disp_pdb							: 1,	// 10 : disp_pdb
	disp_ms							: 3,	// 11:13
	disp_sr							: 4,	// 14:17
	disp_od							: 3,	// 18:20
	disp_n							: 8,	// 21:28
	disp_m							: 3;	// 29:31
} CTR54;

/*-----------------------------------------------------------------------------
	0x00ec ctr55 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dco_fcw							:22,	//	0:21
	dco_rtest						: 1,	//	  22
	dco_resetb						: 1,	//	  23
	dco_lpfon						: 2;	// 24:25
} CTR55;

/*-----------------------------------------------------------------------------
	0x00f0 ctr56 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dco_rtest						: 1,	//	   0
	dco_resetb						: 1,	//	   1
	dco_lpfon						: 2;	//	2: 3
} CTR56;

/*-----------------------------------------------------------------------------
	0x00f4 ctr57 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	testdataoutsel0					: 1,	//	   0
	testclk0						: 1,	//	   1
	txpreemphasistune0				: 1,	//	   2
	txrisetune0						: 1,	//	   3
	txvreftune0						: 4,	//	4: 7
	txfslstune0						: 4,	//	8:11
	vregtune0						: 1,	//	  12
	txhsxvtune0						: 2,	// 13:14
	sqrxtune0						: 3,	// 15:17
	compdistune0					: 3,	// 18:20
	loopbackenb0					: 1,	//	  21
	otgdisable0						: 1,	//	  22
	portreset0						: 1,	//	  23
	commononn						: 1,	//	  24
	refclksel						: 2,	// 25:26
	refclkdiv						: 2,	// 27:28
	testen							: 1,	//	  29
	vatestenb						: 1,	//	  30
	siddq							: 1;	//	  31
} CTR57;

/*-----------------------------------------------------------------------------
	0x00f8 ctr58 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
									: 1,	//	   0 reserved
	testdataout1_0_en				: 1,	//	   1
	testdataout1_1_en				: 1,	//	   2
	testdataout1_2_en				: 1,	//	   3
	testdataout1_3_en				: 1,	//	   4
	testdataout0_0_en				: 1,	//	   5
	testdataout0_1_en				: 1,	//	   6
	testdataout0_2_en				: 1,	//	   7
	testdataout0_3_en				: 1,	//	   8
	vregtune1						: 1,	//	   9
	txhsxvtune1						: 2,	// 10:11
	sqrxtune1						: 3,	// 12:14
	compdistune1					: 3,	// 15:17
	loopbackenb1					: 1,	//	  18
	portreset1						: 1,	//	  19
	testaddr0						: 4,	// 20:23
	testdatain0						: 8;	// 24:31
} CTR58;

/*-----------------------------------------------------------------------------
	0x00fc ctr59 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	bt_testdataout1_0_en			: 1,	//	   0
	bt_testdataout1_1_en			: 1,	//	   1
	bt_testdataout1_2_en			: 1,	//	   2
	bt_testdataout1_3_en			: 1,	//	   3
	bt_testdataout0_0_en			: 1,	//	   4
	bt_testdataout0_1_en			: 1,	//	   5
	bt_testdataout0_2_en			: 1,	//	   6
	bt_testdataout0_3_en			: 1,	//	   7
	testaddr1						: 4,	//	8:11
	testdatain1						: 8,	// 12:19
	testdataoutsel1					: 1,	//	  20
	testclk1						: 1,	//	  21
	txpreemphasistune1				: 1,	//	  22
	txrisetune1						: 1,	//	  23
	txvreftune1						: 4,	// 24:27
	txfslstune1						: 4;	// 28:31
} CTR59;

/*-----------------------------------------------------------------------------
	0x0100 ctr60 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	testdataoutsel0					: 1,	//	   0
	testclk0						: 1,	//	   1
	txpreemphasistune0				: 1,	//	   2
	txrisetune0						: 1,	//	   3
	txvreftune0						: 4,	//	4: 7
	txfslstune0						: 4,	//	8:11
	vregtune0						: 1,	//	  12
	txhsxvtune0						: 2,	// 13:14
	sqrxtune0						: 3,	// 15:17
	compdistune0					: 3,	// 18:20
	loopbackenb0					: 1,	//	  21
	otgdisable0						: 1,	//	  22
	portreset0						: 1,	//	  23
	commononn						: 1,	//	  24
	refclksel						: 2,	// 25:26
	refclkdiv						: 2,	// 27:28
	testen							: 1,	//	  29
	vatestenb						: 1,	//	  30
	tsiddq							: 1;	//	  31
} CTR60;

/*-----------------------------------------------------------------------------
	0x0104 ctr61 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	demod_reset_sel					: 1,	//	   0
	bbmode_sel						: 2,	//	1: 2
	uart0_sel						: 2,	//	3: 4
	uart_rx_sel						: 1,	//	   5
									: 1,	//	   6 reserved
	cvbs_input_mux					: 1,	//	   7
									: 1,	//	   8 reserved
	vregtune1						: 1,	//	   9
	txhsxvtune1						: 2,	// 10:11
	sqrxtune1						: 3,	// 12:14
	compdistune1					: 3,	// 15:17
	loopbackenb1					: 1,	//	  18
	portreset1						: 1,	//	  19
	testaddr0						: 4,	// 20:23
	testdatain0						: 8;	// 24:31
} CTR61;

/*-----------------------------------------------------------------------------
	0x0108 ctr62 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	jtag_sel						: 3,	//	0: 2
	uart2_sel						: 2,	//	3: 4
	uart1_sel						: 1,	//	   5
									: 2,	//	6: 7 reserved
	testaddr1						: 4,	//	8:11
	testdatain1						: 8,	// 12:19
	testdataoutsel1					: 1,	//	  20
	testclk1						: 1,	//	  21
	txpreemphasistune1				: 1,	//	  22
	txrisetune1						: 1,	//	  23
	txvreftune1						: 4,	// 24:27
	txfslstune1						: 4;	// 28:31
} CTR62;

/*-----------------------------------------------------------------------------
	0x010c ctr63 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	gpo_probe_0_en					: 1,	//	   0
	gpo_probe_1_en					: 1,	//	   1
	gpo_probe_2_en					: 1,	//	   2
	gpo_probe_3_en					: 1,	//	   3
	gpo_probe_4_en					: 1,	//	   4
	gpo_probe_5_en					: 1,	//	   5
	gpo_probe_6_en					: 1,	//	   6
	gpo_probe_7_en					: 1,	//	   7
	gpo_probe_8_en					: 1,	//	   8
	gpo_probe_9_en					: 1,	//	   9
	gpo_probe_10_en					: 1,	//	  10
	gpo_probe_11_en					: 1,	//	  11
	q_probe_0_en					: 1,	//	  12
	q_probe_1_en					: 1,	//	  13
	q_probe_2_en					: 1,	//	  14
	q_probe_3_en					: 1,	//	  15
	q_probe_4_en					: 1,	//	  16
	q_probe_5_en					: 1,	//	  17
	q_probe_6_en					: 1,	//	  18
	q_probe_7_en					: 1,	//	  19
	q_probe_8_en					: 1,	//	  20
	q_probe_9_en					: 1,	//	  21
	i_probe_0_en					: 1,	//	  22
	i_probe_1_en					: 1,	//	  23
	i_probe_2_en					: 1,	//	  24
	i_probe_3_en					: 1,	//	  25
	i_probe_4_en					: 1,	//	  26
	i_probe_5_en					: 1,	//	  27
	i_probe_6_en					: 1,	//	  28
	i_probe_7_en					: 1,	//	  29
	i_probe_8_en					: 1,	//	  30
	i_probe_9_en					: 1;	//	  31
} CTR63;

typedef struct {
	UINT32								resrvd[4]						;	// 0x0000 : ''
	CTR00								ctr00							;	// 0x0010 : ''
	CTR01								ctr01							;	// 0x0014 : ''
	CTR02								ctr02							;	// 0x0018 : ''
	CTR03								ctr03_B0						;	// 0x001c : ''
	CTR04								ctr04_B0						;	// 0x0020 : ''
	CTR05								ctr05_B0						;	// 0x0024 : ''
	CTR06								ctr06							;	// 0x0028 : ''
	CTR07								ctr07							;	// 0x002c : ''
	CTR08								ctr08							;	// 0x0030 : ''
	CTR09								ctr09							;	// 0x0034 : ''
	CTR10								ctr10							;	// 0x0038 : ''
	CTR11								ctr11							;	// 0x003c : ''
	CTR12								ctr12							;	// 0x0040 : ''
	CTR13								ctr13							;	// 0x0044 : ''
	CTR14								ctr14							;	// 0x0048 : ''
	CTR15								ctr15							;	// 0x004c : ''
	CTR16								ctr16							;	// 0x0050 : ''
	CTR17								ctr17							;	// 0x0054 : ''
	CTR18								ctr18							;	// 0x0058 : ''
	CTR19								ctr19							;	// 0x005c : ''
	CTR20								ctr20							;	// 0x0060 : ''
	CTR21								ctr21							;	// 0x0064 : ''
	CTR22								ctr22							;	// 0x0068 : ''
	CTR23								ctr23							;	// 0x006c : ''
	CTR24								ctr24							;	// 0x0070 : ''
	CTR25								ctr25							;	// 0x0074 : ''
	CTR26								ctr26							;	// 0x0078 : ''
	CTR27								ctr27							;	// 0x007c : ''
	UINT32								ctr28							;	// 0x0080 : ''
	UINT32								ctr29							;	// 0x0084 : ''
	UINT32								ctr30							;	// 0x0088 : ''
	UINT32								ctr31							;	// 0x008c : ''
	CTR32								ctr32							;	// 0x0090 : ''
	CTR33								ctr33							;	// 0x0094 : ''
	CTR34								ctr34							;	// 0x0098 : ''
	CTR35								ctr35							;	// 0x009c : ''
	CTR36								ctr36							;	// 0x00a0 : ''
	CTR37								ctr37							;	// 0x00a4 : ''
	CTR38								ctr38							;	// 0x00a8 : ''
	CTR39								ctr39							;	// 0x00ac : ''
	CTR40								ctr40							;	// 0x00b0 : ''
	CTR41								ctr41							;	// 0x00b4 : ''
	CTR42								ctr42							;	// 0x00b8 : ''
	CTR43								ctr43							;	// 0x00bc : ''
	CTR44								ctr44							;	// 0x00c0 : ''
	CTR45								ctr45							;	// 0x00c4 : ''
	CTR46								ctr46							;	// 0x00c8 : ''
	CTR47								ctr47							;	// 0x00cc : ''
	CTR48								ctr48							;	// 0x00d0 : ''
	CTR49								ctr49							;	// 0x00d4 : ''
	CTR50								ctr50							;	// 0x00d8 : ''
	CTR51								ctr51							;	// 0x00dc : ''
	CTR52								ctr52							;	// 0x00e0 : ''
	CTR53								ctr53							;	// 0x00e4 : ''
	CTR54								ctr54							;	// 0x00e8 : ''
	CTR55								ctr55							;	// 0x00ec : ''
	CTR56								ctr56							;	// 0x00f0 : ''
	CTR57								ctr57							;	// 0x00f4 : ''
	CTR58								ctr58							;	// 0x00f8 : ''
	CTR59								ctr59							;	// 0x00fc : ''
	CTR60								ctr60							;	// 0x0100 : ''
	CTR61								ctr61							;	// 0x0104 : ''
	CTR62								ctr62							;	// 0x0108 : ''
	CTR63								ctr63							;	// 0x010c : ''
} CTOP_CTRL_REG_T;
/* 64 regs, 64 types */

/* 64 regs, 64 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegCTOP_CTRL : real register of CTOP_CTRL.
 * gpRegCTOP_CTRL	  : shadow register.
 *
 * @def CTOP_CTRL_RdFL: Read  FLushing : Shadow <- Real.
 * @def CTOP_CTRL_WrFL: Write FLushing : Shadow -> Real.
 * @def CTOP_CTRL_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def CTOP_CTRL_Wr  : Write whole register(UINT32) from Shadow register.
 * @def CTOP_CTRL_Rd01 ~ CTOP_CTRL_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def CTOP_CTRL_Wr01 ~ CTOP_CTRL_Wrnn: Write given '01~nn' fields to	 Shadow register.
 * */
#define CTOP_CTRL_RdFL(_r)			((gpRegCTOP_CTRL->_r)=(gpRealRegCTOP_CTRL->_r))
#define CTOP_CTRL_WrFL(_r)			((gpRealRegCTOP_CTRL->_r)=(gpRegCTOP_CTRL->_r))

#define CTOP_CTRL_Rd(_r)			*((UINT32*)(&(gpRegCTOP_CTRL->_r)))
#define CTOP_CTRL_Wr(_r,_v)			((CTOP_CTRL_Rd(_r))=((UINT32)(_v)))

#define CTOP_CTRL_Rd01(_r,_f01,_v01)													\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
								} while(0)

#define CTOP_CTRL_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
								} while(0)

#define CTOP_CTRL_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
								} while(0)

#define CTOP_CTRL_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
								} while(0)

#define CTOP_CTRL_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
								} while(0)

#define CTOP_CTRL_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
								} while(0)

#define CTOP_CTRL_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
								} while(0)

#define CTOP_CTRL_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
								} while(0)

#define CTOP_CTRL_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
								} while(0)

#define CTOP_CTRL_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
									(_v10) = (gpRegCTOP_CTRL->_r._f10);				\
								} while(0)

#define CTOP_CTRL_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
									(_v10) = (gpRegCTOP_CTRL->_r._f10);				\
									(_v11) = (gpRegCTOP_CTRL->_r._f11);				\
								} while(0)

#define CTOP_CTRL_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
									(_v10) = (gpRegCTOP_CTRL->_r._f10);				\
									(_v11) = (gpRegCTOP_CTRL->_r._f11);				\
									(_v12) = (gpRegCTOP_CTRL->_r._f12);				\
								} while(0)

#define CTOP_CTRL_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
									(_v10) = (gpRegCTOP_CTRL->_r._f10);				\
									(_v11) = (gpRegCTOP_CTRL->_r._f11);				\
									(_v12) = (gpRegCTOP_CTRL->_r._f12);				\
									(_v13) = (gpRegCTOP_CTRL->_r._f13);				\
								} while(0)

#define CTOP_CTRL_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
									(_v10) = (gpRegCTOP_CTRL->_r._f10);				\
									(_v11) = (gpRegCTOP_CTRL->_r._f11);				\
									(_v12) = (gpRegCTOP_CTRL->_r._f12);				\
									(_v13) = (gpRegCTOP_CTRL->_r._f13);				\
									(_v14) = (gpRegCTOP_CTRL->_r._f14);				\
								} while(0)

#define CTOP_CTRL_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
									(_v10) = (gpRegCTOP_CTRL->_r._f10);				\
									(_v11) = (gpRegCTOP_CTRL->_r._f11);				\
									(_v12) = (gpRegCTOP_CTRL->_r._f12);				\
									(_v13) = (gpRegCTOP_CTRL->_r._f13);				\
									(_v14) = (gpRegCTOP_CTRL->_r._f14);				\
									(_v15) = (gpRegCTOP_CTRL->_r._f15);				\
								} while(0)

#define CTOP_CTRL_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(_v01) = (gpRegCTOP_CTRL->_r._f01);				\
									(_v02) = (gpRegCTOP_CTRL->_r._f02);				\
									(_v03) = (gpRegCTOP_CTRL->_r._f03);				\
									(_v04) = (gpRegCTOP_CTRL->_r._f04);				\
									(_v05) = (gpRegCTOP_CTRL->_r._f05);				\
									(_v06) = (gpRegCTOP_CTRL->_r._f06);				\
									(_v07) = (gpRegCTOP_CTRL->_r._f07);				\
									(_v08) = (gpRegCTOP_CTRL->_r._f08);				\
									(_v09) = (gpRegCTOP_CTRL->_r._f09);				\
									(_v10) = (gpRegCTOP_CTRL->_r._f10);				\
									(_v11) = (gpRegCTOP_CTRL->_r._f11);				\
									(_v12) = (gpRegCTOP_CTRL->_r._f12);				\
									(_v13) = (gpRegCTOP_CTRL->_r._f13);				\
									(_v14) = (gpRegCTOP_CTRL->_r._f14);				\
									(_v15) = (gpRegCTOP_CTRL->_r._f15);				\
									(_v16) = (gpRegCTOP_CTRL->_r._f16);				\
								} while(0)


#define CTOP_CTRL_Wr01(_r,_f01,_v01)													\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
								} while(0)

#define CTOP_CTRL_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
								} while(0)

#define CTOP_CTRL_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
								} while(0)

#define CTOP_CTRL_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
								} while(0)

#define CTOP_CTRL_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
								} while(0)

#define CTOP_CTRL_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
								} while(0)

#define CTOP_CTRL_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
								} while(0)

#define CTOP_CTRL_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
								} while(0)

#define CTOP_CTRL_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
								} while(0)

#define CTOP_CTRL_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
									(gpRegCTOP_CTRL->_r._f10) = (_v10);				\
								} while(0)

#define CTOP_CTRL_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
									(gpRegCTOP_CTRL->_r._f10) = (_v10);				\
									(gpRegCTOP_CTRL->_r._f11) = (_v11);				\
								} while(0)

#define CTOP_CTRL_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
									(gpRegCTOP_CTRL->_r._f10) = (_v10);				\
									(gpRegCTOP_CTRL->_r._f11) = (_v11);				\
									(gpRegCTOP_CTRL->_r._f12) = (_v12);				\
								} while(0)

#define CTOP_CTRL_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
									(gpRegCTOP_CTRL->_r._f10) = (_v10);				\
									(gpRegCTOP_CTRL->_r._f11) = (_v11);				\
									(gpRegCTOP_CTRL->_r._f12) = (_v12);				\
									(gpRegCTOP_CTRL->_r._f13) = (_v13);				\
								} while(0)

#define CTOP_CTRL_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
									(gpRegCTOP_CTRL->_r._f10) = (_v10);				\
									(gpRegCTOP_CTRL->_r._f11) = (_v11);				\
									(gpRegCTOP_CTRL->_r._f12) = (_v12);				\
									(gpRegCTOP_CTRL->_r._f13) = (_v13);				\
									(gpRegCTOP_CTRL->_r._f14) = (_v14);				\
								} while(0)

#define CTOP_CTRL_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
									(gpRegCTOP_CTRL->_r._f10) = (_v10);				\
									(gpRegCTOP_CTRL->_r._f11) = (_v11);				\
									(gpRegCTOP_CTRL->_r._f12) = (_v12);				\
									(gpRegCTOP_CTRL->_r._f13) = (_v13);				\
									(gpRegCTOP_CTRL->_r._f14) = (_v14);				\
									(gpRegCTOP_CTRL->_r._f15) = (_v15);				\
								} while(0)

#define CTOP_CTRL_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(gpRegCTOP_CTRL->_r._f01) = (_v01);				\
									(gpRegCTOP_CTRL->_r._f02) = (_v02);				\
									(gpRegCTOP_CTRL->_r._f03) = (_v03);				\
									(gpRegCTOP_CTRL->_r._f04) = (_v04);				\
									(gpRegCTOP_CTRL->_r._f05) = (_v05);				\
									(gpRegCTOP_CTRL->_r._f06) = (_v06);				\
									(gpRegCTOP_CTRL->_r._f07) = (_v07);				\
									(gpRegCTOP_CTRL->_r._f08) = (_v08);				\
									(gpRegCTOP_CTRL->_r._f09) = (_v09);				\
									(gpRegCTOP_CTRL->_r._f10) = (_v10);				\
									(gpRegCTOP_CTRL->_r._f11) = (_v11);				\
									(gpRegCTOP_CTRL->_r._f12) = (_v12);				\
									(gpRegCTOP_CTRL->_r._f13) = (_v13);				\
									(gpRegCTOP_CTRL->_r._f14) = (_v14);				\
									(gpRegCTOP_CTRL->_r._f15) = (_v15);				\
									(gpRegCTOP_CTRL->_r._f16) = (_v16);				\
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
 * _rwname: name of rw	  bit field : shall be 0 after CTOP_CTRL_Wind(), 1 for CTOP_CTRL_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * CTOP_CTRL_Rind : General indexed register Read.(
 * CTOP_CTRL_Wind : General indexed register Read.
 *
 * CTOP_CTRL_Ridx : For 'index', 'rw', 'load' field name
 * CTOP_CTRL_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define CTOP_CTRL_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								CTOP_CTRL_Wr03(_r,_lname,0,_rwname,1,_iname,_ival); \
								CTOP_CTRL_WrFL(_r);									\
								CTOP_CTRL_RdFL(_r);									\
								CTOP_CTRL_Rd01(_r,_fname,_fval);						\
							} while (0)

#define CTOP_CTRL_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				CTOP_CTRL_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define CTOP_CTRL_Ridx(_r, _ival, _fname, _fval)	CTOP_CTRL_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define CTOP_CTRL_Widx(_r, _ival, _fname, _fval)	CTOP_CTRL_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'L8_CTOP_CTRL_RegFile_100310.csv' 20100311 00:54:16   대한민국 표준시 by getregs v2.3 */
