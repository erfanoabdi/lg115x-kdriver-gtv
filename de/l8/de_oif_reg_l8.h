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
	0x0000 out_video_info ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	out_ch_num						: 2,	//	0: 1
	out_sel							: 2;	//	2: 3
} OUT_VIDEO_INFO;

/*-----------------------------------------------------------------------------
	0x0004 data_color_from ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	b_data_from						: 2,	//	0: 1
	g_data_from						: 2,	//	2: 3
	r_data_from						: 2;	//	4: 5
} DATA_COLOR_FROM;

/*-----------------------------------------------------------------------------
	0x0008 data_color ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	data_color_b					:10,	//	0:9
	data_color_g					:10,	// 10:19
	data_color_r					:10;	// 20:29
} DATA_COLOR;

/*-----------------------------------------------------------------------------
	0x000c sync_color ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sync_color_b					:10,	//	0:9
	sync_color_g					:10,	// 10:19
	sync_color_r					:10;	// 20:29
} SYNC_COLOR;

/*-----------------------------------------------------------------------------
	0x0010 delay_data ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	b_delay							: 3,	//	0: 2
	g_delay							: 3,	//	3: 5
	r_delay							: 3,	//	6: 8
	de_delay						: 4;	//	9:12
} DELAY_DATA;

/*-----------------------------------------------------------------------------
	0x0014 sync_de_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	de_pol							: 1,	//	   0
	de_disable						: 1,	//	   1
	hsync_pol						: 1,	//	   2
	hsync_disable					: 1,	//	   3
	vsync_pol						: 1,	//	   4
	vsync_disable					: 1,	//	   5
	out_sdq							: 2;	//	6: 7
} SYNC_DE_CONTROL;

/*-----------------------------------------------------------------------------
	0x0018 pwm_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
//	pwm_sel							: 6,	//	   0
	pwm_sel							: 1,	//	   0
	pwm0_pol						: 1,	//	   1
	pwm1_pol						: 1,	//	   2
	pwm2_pol						: 1,	//	   3
	pwm3_pol						: 1,	//	   4
	pwm4_pol						: 1;	//	   5
} PWM_CONTROL;

/*-----------------------------------------------------------------------------
	0x001c led_in_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	led_in_control					: 1;	//	   0
} LED_IN_CONTROL;

/*-----------------------------------------------------------------------------
	0x0020 lvds_if_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	lvds_empty_data					: 2,	//	0:1
	lvds_8bit_mode					: 1,	//	2
	lvds_lsb_first					: 1,	//	3
	                   				: 1,	//	4 resered
	lvds_sync_order					: 3,	//	5: 7
	lvds_ch3_from					: 2,	//	8: 9
	lvds_ch2_from					: 2,	// 10:11
	lvds_ch1_from					: 2,	// 12:13
	lvds_ch0_from					: 2,	// 14:15
	lvds_out_sel					: 1;	//	  16
} LVDS_IF_CONTROL;

/*-----------------------------------------------------------------------------
	0x0024 lr_if_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	lr_if_control					: 1;	//	   0
} LR_IF_CONTROL;

/*-----------------------------------------------------------------------------
	0x0030 reserved0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	reserved0						: 8;	//	0: 7
} RESERVED0;

/*-----------------------------------------------------------------------------
	0x0034 lvds_en ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	lvds_en							:24;	//	0:23
} LVDS_EN;

/*-----------------------------------------------------------------------------
	0x0038 lvds_force_data_en ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	lvds_ch3_force_en				: 4,	//	0: 3
	lvds_ch2_force_en				: 4,	//	4: 7
	lvds_ch1_force_en				: 4,	//	8:11
	lvds_ch0_force_en				: 4;	// 12:15
} LVDS_FORCE_DATA_EN;

/*-----------------------------------------------------------------------------
	0x003c lvds_force_data ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	lvds_ch3_data					: 4,	//	0: 3
	lvds_ch2_data					: 4,	//	4: 7
	lvds_ch1_data					: 4,	//	8:11
	lvds_ch0_data					: 4;	// 12:15
} LVDS_FORCE_DATA;

typedef struct {
	OUT_VIDEO_INFO						out_video_info					;	// 0x0000 : ''
	DATA_COLOR_FROM						data_color_from					;	// 0x0004 : ''
	DATA_COLOR							data_color						;	// 0x0008 : ''
	SYNC_COLOR							sync_color						;	// 0x000c : ''
	DELAY_DATA							delay_data						;	// 0x0010 : ''
	SYNC_DE_CONTROL						sync_de_control					;	// 0x0014 : ''
	PWM_CONTROL							pwm_control						;	// 0x0018 : ''
	LED_IN_CONTROL						led_in_control					;	// 0x001c : ''
	LVDS_IF_CONTROL						lvds_if_control					;	// 0x0020 : ''
	LR_IF_CONTROL						lr_if_control					;	// 0x0024 : ''
	UINT32												 __rsvd_00[   2];	// 0x0028 ~ 0x002c
	RESERVED0							reserved0						;	// 0x0030 : ''
	LVDS_EN								lvds_en							;	// 0x0034 : ''
	LVDS_FORCE_DATA_EN					lvds_force_data_en				;	// 0x0038 : ''
	LVDS_FORCE_DATA						lvds_force_data					;	// 0x003c : ''
} OIF_REG_T;
/* 14 regs, 14 types */

/* 14 regs, 14 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegOIF : real register of OIF.
 * gpRegOIF		: shadow register.
 *
 * @def OIF_RdFL: Read	FLushing : Shadow <- Real.
 * @def OIF_WrFL: Write FLushing : Shadow -> Real.
 * @def OIF_Rd	: Read	whole register(UINT32) from Shadow register.
 * @def OIF_Wr	: Write whole register(UINT32) from Shadow register.
 * @def OIF_Rd01 ~ OIF_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def OIF_Wr01 ~ OIF_Wrnn: Write given '01~nn' fields to	 Shadow register.
 * */
#define OIF_RdFL(_r)			((gpRegOIF->_r)=(gpRealRegOIF->_r))
#define OIF_WrFL(_r)			((gpRealRegOIF->_r)=(gpRegOIF->_r))

#define OIF_Rd(_r)			*((UINT32*)(&(gpRegOIF->_r)))
#define OIF_Wr(_r,_v)			((OIF_Rd(_r))=((UINT32)(_v)))

#define OIF_Rd01(_r,_f01,_v01)													\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
								} while(0)

#define OIF_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
								} while(0)

#define OIF_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
								} while(0)

#define OIF_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
								} while(0)

#define OIF_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
								} while(0)

#define OIF_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
								} while(0)

#define OIF_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
								} while(0)

#define OIF_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
								} while(0)

#define OIF_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
								} while(0)

#define OIF_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
									(_v10) = (gpRegOIF->_r._f10);				\
								} while(0)

#define OIF_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
									(_v10) = (gpRegOIF->_r._f10);				\
									(_v11) = (gpRegOIF->_r._f11);				\
								} while(0)

#define OIF_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
									(_v10) = (gpRegOIF->_r._f10);				\
									(_v11) = (gpRegOIF->_r._f11);				\
									(_v12) = (gpRegOIF->_r._f12);				\
								} while(0)

#define OIF_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
									(_v10) = (gpRegOIF->_r._f10);				\
									(_v11) = (gpRegOIF->_r._f11);				\
									(_v12) = (gpRegOIF->_r._f12);				\
									(_v13) = (gpRegOIF->_r._f13);				\
								} while(0)

#define OIF_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
									(_v10) = (gpRegOIF->_r._f10);				\
									(_v11) = (gpRegOIF->_r._f11);				\
									(_v12) = (gpRegOIF->_r._f12);				\
									(_v13) = (gpRegOIF->_r._f13);				\
									(_v14) = (gpRegOIF->_r._f14);				\
								} while(0)

#define OIF_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
									(_v10) = (gpRegOIF->_r._f10);				\
									(_v11) = (gpRegOIF->_r._f11);				\
									(_v12) = (gpRegOIF->_r._f12);				\
									(_v13) = (gpRegOIF->_r._f13);				\
									(_v14) = (gpRegOIF->_r._f14);				\
									(_v15) = (gpRegOIF->_r._f15);				\
								} while(0)

#define OIF_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(_v01) = (gpRegOIF->_r._f01);				\
									(_v02) = (gpRegOIF->_r._f02);				\
									(_v03) = (gpRegOIF->_r._f03);				\
									(_v04) = (gpRegOIF->_r._f04);				\
									(_v05) = (gpRegOIF->_r._f05);				\
									(_v06) = (gpRegOIF->_r._f06);				\
									(_v07) = (gpRegOIF->_r._f07);				\
									(_v08) = (gpRegOIF->_r._f08);				\
									(_v09) = (gpRegOIF->_r._f09);				\
									(_v10) = (gpRegOIF->_r._f10);				\
									(_v11) = (gpRegOIF->_r._f11);				\
									(_v12) = (gpRegOIF->_r._f12);				\
									(_v13) = (gpRegOIF->_r._f13);				\
									(_v14) = (gpRegOIF->_r._f14);				\
									(_v15) = (gpRegOIF->_r._f15);				\
									(_v16) = (gpRegOIF->_r._f16);				\
								} while(0)


#define OIF_Wr01(_r,_f01,_v01)													\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
								} while(0)

#define OIF_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
								} while(0)

#define OIF_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
								} while(0)

#define OIF_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
								} while(0)

#define OIF_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
								} while(0)

#define OIF_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
								} while(0)

#define OIF_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
								} while(0)

#define OIF_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
								} while(0)

#define OIF_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
								} while(0)

#define OIF_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
									(gpRegOIF->_r._f10) = (_v10);				\
								} while(0)

#define OIF_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
									(gpRegOIF->_r._f10) = (_v10);				\
									(gpRegOIF->_r._f11) = (_v11);				\
								} while(0)

#define OIF_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
									(gpRegOIF->_r._f10) = (_v10);				\
									(gpRegOIF->_r._f11) = (_v11);				\
									(gpRegOIF->_r._f12) = (_v12);				\
								} while(0)

#define OIF_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
									(gpRegOIF->_r._f10) = (_v10);				\
									(gpRegOIF->_r._f11) = (_v11);				\
									(gpRegOIF->_r._f12) = (_v12);				\
									(gpRegOIF->_r._f13) = (_v13);				\
								} while(0)

#define OIF_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
									(gpRegOIF->_r._f10) = (_v10);				\
									(gpRegOIF->_r._f11) = (_v11);				\
									(gpRegOIF->_r._f12) = (_v12);				\
									(gpRegOIF->_r._f13) = (_v13);				\
									(gpRegOIF->_r._f14) = (_v14);				\
								} while(0)

#define OIF_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
									(gpRegOIF->_r._f10) = (_v10);				\
									(gpRegOIF->_r._f11) = (_v11);				\
									(gpRegOIF->_r._f12) = (_v12);				\
									(gpRegOIF->_r._f13) = (_v13);				\
									(gpRegOIF->_r._f14) = (_v14);				\
									(gpRegOIF->_r._f15) = (_v15);				\
								} while(0)

#define OIF_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do {											\
									(gpRegOIF->_r._f01) = (_v01);				\
									(gpRegOIF->_r._f02) = (_v02);				\
									(gpRegOIF->_r._f03) = (_v03);				\
									(gpRegOIF->_r._f04) = (_v04);				\
									(gpRegOIF->_r._f05) = (_v05);				\
									(gpRegOIF->_r._f06) = (_v06);				\
									(gpRegOIF->_r._f07) = (_v07);				\
									(gpRegOIF->_r._f08) = (_v08);				\
									(gpRegOIF->_r._f09) = (_v09);				\
									(gpRegOIF->_r._f10) = (_v10);				\
									(gpRegOIF->_r._f11) = (_v11);				\
									(gpRegOIF->_r._f12) = (_v12);				\
									(gpRegOIF->_r._f13) = (_v13);				\
									(gpRegOIF->_r._f14) = (_v14);				\
									(gpRegOIF->_r._f15) = (_v15);				\
									(gpRegOIF->_r._f16) = (_v16);				\
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
 * _rwname: name of rw	  bit field : shall be 0 after OIF_Wind(), 1 for OIF_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * OIF_Rind : General indexed register Read.(
 * OIF_Wind : General indexed register Read.
 *
 * OIF_Ridx : For 'index', 'rw', 'load' field name
 * OIF_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define OIF_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								OIF_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								OIF_WrFL(_r);									\
								OIF_RdFL(_r);									\
								OIF_Rd01(_r,_fname,_fval);						\
							} while (0)

#define OIF_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				OIF_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define OIF_Ridx(_r, _ival, _fname, _fval)	OIF_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define OIF_Widx(_r, _ival, _fname, _fval)	OIF_Wind(_r,load,rw,index,_ival,_fname,_fval)

/* from 'LG1150-FRC-MAN-03 v0.9(OIF-20100309).csv' 20100311 00:54:18   대한민국 표준시 by getregs v2.3 */
