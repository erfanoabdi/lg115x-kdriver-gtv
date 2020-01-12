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

/** @file
 *
 *  AAD(Analog Audio Decoder) register details. ( used only within kdriver )
 *
 *  author     JongSang Oh(jongsang.oh@lge.com)
 *  version    1.0 => 2.0 (L8-A0/A1 => B0)
 *  date       2010.05.28 => 2010.12.07
 */

#ifndef _AAD_REG_H_
#define _AAD_REG_H_

/*----------------------------------------------------------------------------------------
    Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
	0x0000 system_select ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	syssel                          : 4,	//  0: 3
	asden                           : 1;	//     4
} System_Select;

/*-----------------------------------------------------------------------------
	0x0004 standard_select ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	stndrd                          : 3;	//  0: 2
} Standard_Select;

/*-----------------------------------------------------------------------------
	0x0008 sample_rate ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	samprt                          : 3;	//  0: 2
} Sample_Rate;

/*-----------------------------------------------------------------------------
	0x000c fm_am_demod_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dmd1dv                          : 2,	//  0: 1
	dmd1md                          : 1,	//     2
	dmd1fl                          : 2,	//  3: 4
	dmd1dv2                         : 1,	//     5
	dmd1fl2                         : 1;	//     6
} FM_AM_Demod_Ctrl;

/*-----------------------------------------------------------------------------
	0x0010 demod1_carrier_freq0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dm1fq                           : 8;	//  0: 7
} Demod1_Carrier_Freq0;

/*-----------------------------------------------------------------------------
	0x0014 demod1_carrier_freq1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dm1fq                           : 8;	//  0: 7
} Demod1_Carrier_Freq1;

/*-----------------------------------------------------------------------------
	0x0018 fm_dqpsk_demod_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dmd2dv                          : 2,	//  0: 1
	                                : 1,	//     2 reserved
	dmd2fl                          : 2,	//  3: 4
	dmd2dv2                         : 1;	//     5
} FM_DQPSK_Demod_Ctrl;

/*-----------------------------------------------------------------------------
	0x001c demod2_carrier_freq0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dm2fq                           : 8;	//  0: 7
} Demod2_Carrier_Freq0;

/*-----------------------------------------------------------------------------
	0x0020 demod2_carrier_freq1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	dm2fq                           : 8;	//  0: 7
} Demod2_Carrier_Freq1;

/*-----------------------------------------------------------------------------
	0x0024 audio_output_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	rtsel                           : 2,	//  0: 1
	lfsel                           : 2,	//  2: 3
	aouten                          : 1,	//     4
	lrsum                           : 1,	//     5
	rol                             : 1,	//     6
	lor                             : 1;	//     7
} Audio_Output_Ctrl;

/*-----------------------------------------------------------------------------
	0x0028 audio_mode_detect ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	audmod                          : 2;	//  0: 1
} Audio_Mode_Detect;

/*-----------------------------------------------------------------------------
	0x0030 carrier1_average_freq0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1fq                           : 8;	//  0: 7
} Carrier1_Average_Freq0;

/*-----------------------------------------------------------------------------
	0x0034 carrier1_average_freq1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1fq                           : 8;	//  0: 7
} Carrier1_Average_Freq1;

/*-----------------------------------------------------------------------------
	0x0038 carrier1_phase_noise0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1pn                           : 8;	//  0: 7
} Carrier1_Phase_Noise0;

/*-----------------------------------------------------------------------------
	0x003c carrier1_phase_noise1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1pn                           : 8;	//  0: 7
} Carrier1_Phase_Noise1;

/*-----------------------------------------------------------------------------
	0x0040 carrier1_average_mag0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1mg                           : 8;	//  0: 7
} Carrier1_Average_Mag0;

/*-----------------------------------------------------------------------------
	0x0044 carrier1_average_mag1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1mg                           : 8;	//  0: 7
} Carrier1_Average_Mag1;

/*-----------------------------------------------------------------------------
	0x0048 carrier1_mag_noise0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1mn                           : 8;	//  0: 7
} Carrier1_Mag_Noise0;

/*-----------------------------------------------------------------------------
	0x004c carrier1_mag_noise1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1mn                           : 8;	//  0: 7
} Carrier1_Mag_Noise1;

/*-----------------------------------------------------------------------------
	0x0050 carrier1_fm_quality0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1ql                           : 8;	//  0: 7
} Carrier1_FM_Quality0;

/*-----------------------------------------------------------------------------
	0x0054 carrier1_fm_quality1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr1ql                           : 8;	//  0: 7
} Carrier1_FM_Quality1;

/*-----------------------------------------------------------------------------
	0x0058 carrier2_average_mag0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2mg                           : 8;	//  0: 7
} Carrier2_Average_Mag0;

/*-----------------------------------------------------------------------------
	0x005c carrier2_average_mag1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2mg                           : 8;	//  0: 7
} Carrier2_Average_Mag1;

/*-----------------------------------------------------------------------------
	0x0060 carrier2_mag_noise0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2mn                           : 8;	//  0: 7
} Carrier2_Mag_Noise0;

/*-----------------------------------------------------------------------------
	0x0064 carrier2_mag_noise1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2mn                           : 8;	//  0: 7
} Carrier2_Mag_Noise1;

/*-----------------------------------------------------------------------------
	0x0068 carrier2_fm_quality0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2ql                           : 8;	//  0: 7
} Carrier2_FM_Quality0;

/*-----------------------------------------------------------------------------
	0x006c carrier2_fm_quality1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2ql                           : 8;	//  0: 7
} Carrier2_FM_Quality1;

/*-----------------------------------------------------------------------------
	0x0070 pilot_magnitude ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pltmag                          : 8;	//  0: 7
} Pilot_Magnitude;

/*-----------------------------------------------------------------------------
	0x0074 pilot_pll_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	plllok                          : 1;	//     0
} Pilot_PLL_Status;

/*-----------------------------------------------------------------------------
	0x0078 fm_subcarrier_mag ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	submag                          : 8;	//  0: 7
} FM_Subcarrier_Mag;

/*-----------------------------------------------------------------------------
	0x007c fm_subcarrier_noise ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	subns                           : 8;	//  0: 7
} FM_Subcarrier_Noise;

/*-----------------------------------------------------------------------------
	0x0080 nicam_error_rate0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nicerr                          : 8;	//  0: 7
} NICAM_Error_Rate0;

/*-----------------------------------------------------------------------------
	0x0084 nicam_error_rate1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nicerr                          : 8;	//  0: 7
} NICAM_Error_Rate1;

/*-----------------------------------------------------------------------------
	0x0088 nicam_control_bits0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nicad                           : 8;	//  0: 7
} NICAM_Control_Bits0;

/*-----------------------------------------------------------------------------
	0x008c nicam_control_bits1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nicctl                          : 5,	//  0: 4
	nicad                           : 3;	//  5: 7
} NICAM_Control_Bits1;

/*-----------------------------------------------------------------------------
	0x0090 agc_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	agcdcy                          : 4,	//  0: 3
	agcatk                          : 4;	//  4: 7
} AGC_Control;

/*-----------------------------------------------------------------------------
	0x0094 agc_ctrl_for_am ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	amdcy                           : 4,	//  0: 3
	amatk                           : 4;	//  4: 7
} AGC_Ctrl_for_AM;

/*-----------------------------------------------------------------------------
	0x0098 pilot_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pilotpaln                       : 1,	//     0
	sapphs                          : 1;	//     1
} Pilot_Control;

/*-----------------------------------------------------------------------------
	0x00a8 status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	prmcardet                       : 1,	//     0
	seccardet                       : 1,	//     1
	sndstat                         : 2,	//  2: 3
	mono_str                        : 1,	//     4
	imono                           : 1,	//     5
	bisap                           : 1,	//     6
	asdcomplete                     : 1;	//     7
} Status;

/*-----------------------------------------------------------------------------
	0x00ac status_pin_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	staten                          : 1,	//     0
	statpl                          : 1,	//     1
	astat                           : 1;	//     2
} Status_Pin_Control;

/*-----------------------------------------------------------------------------
	0x00b0 headroom ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	headrm                          : 3;	//  0: 2
} Headroom;

/*-----------------------------------------------------------------------------
	0x00b4 agc_freeze_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	agcfrz                          : 1;	//     0
} AGC_Freeze_Control;

/*-----------------------------------------------------------------------------
	0x00b8 stereo_carrier_search_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	srchen                          : 2,	//  0: 1
	chgmod                          : 2,	//  2: 3
	                                : 1,	//     4 reserved
	vsphase_y_rnw                   : 3;	//  5: 7
} Stereo_Carrier_Search_Ctrl;

/*-----------------------------------------------------------------------------
	0x00bc stereo_carrier_search_result ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	srchrsl                         : 4,	//  0: 3
	srchst                          : 1,	//     4
	srchcmp                         : 1,	//     5
	srchind                         : 1;	//     6
} Stereo_Carrier_Search_Result;

/*-----------------------------------------------------------------------------
	0x00c0 carrier2_average_freq0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2fq                           : 8;	//  0: 7
} Carrier2_Average_Freq0;

/*-----------------------------------------------------------------------------
	0x00c4 carrier2_average_freq1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	cr2fq                           : 8;	//  0: 7
} Carrier2_Average_Freq1;

/*-----------------------------------------------------------------------------
	0x00c8 stereo_id_magnitude ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	stmag                           : 8;	//  0: 7
} Stereo_ID_Magnitude;

/*-----------------------------------------------------------------------------
	0x00cc bilingual_id_magnitude ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	bimag                           : 8;	//  0: 7
} Bilingual_ID_Magnitude;

/*-----------------------------------------------------------------------------
	0x0100 audio_output_result ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	rtrsl                           : 2,	//  0: 1
	lfrsl                           : 2,	//  2: 3
	                                : 1,	//     4 reserved
	lrsum                           : 1,	//     5
	rol                             : 1,	//     6
	lor                             : 1;	//     7
} Audio_Output_Result;

/*-----------------------------------------------------------------------------
	0x0104 mute_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mutect                          : 2,	//  0: 1
	muteovov                        : 1,	//     2
	mute                            : 2,	//  3: 4
	asdmut                          : 1,	//     5
	aaosmut                         : 1;	//     6
} Mute_Control;

/*-----------------------------------------------------------------------------
	0x0108 i2s_control ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	i2s_mode                        : 2,	//  0: 1
	                                : 2,	//  2: 3 reserved
	lrpol                           : 1,	//     4
	sclkpol                         : 1;	//     5
} I2S_Control;

/*-----------------------------------------------------------------------------
	0x010c interrupt_mask '' => L8 B0 added
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mask_prmcardet                  : 1,	//     0
	mask_seccardet                  : 1,	//     1
	mask_sndstat                    : 2,	//  2: 3
	mask_monostr                    : 1,	//     4
	mask_imono                      : 1,	//     5
	mask_bisap                      : 1,	//     6
	mask_asd                        : 1;	//     7
} Interrupt_Mask;

/*-----------------------------------------------------------------------------
	0x0110 manual_agc '' => L8 B0 added
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	gain                            : 6,	//  0: 5
	                                : 1,	//     6 reserved
	manualmode                      : 1;	//     7
} Manual_AGC;

/*-----------------------------------------------------------------------------
	0x0114 btsc_en_status '' => L8 B0 added
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	btsc_en                         : 1;	//     0
} BTSC_En_Status;

/*-----------------------------------------------------------------------------
	0x0128 carrier1_quality_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mn1thh                          : 8;	//  0: 7
} Carrier1_Quality_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x012c carrier1_quality_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mn1thl                          : 8;	//  0: 7
} Carrier1_Quality_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0130 carrier2_quality_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mn2thh                          : 8;	//  0: 7
} Carrier2_Quality_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x0134 carrier2_quality_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	mn2thl                          : 8;	//  0: 7
} Carrier2_Quality_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0138 carrier1_phase_noise_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pn1thh                          : 8;	//  0: 7
} Carrier1_Phase_Noise_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x013c carrier1_phase_noise_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pn1thl                          : 8;	//  0: 7
} Carrier1_Phase_Noise_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0140 fm_subcarrier_mag_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fmsmth                          : 8;	//  0: 7
} FM_Subcarrier_Mag_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x0144 fm_subcarrier_mag_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fmsmtl                          : 8;	//  0: 7
} FM_Subcarrier_Mag_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0148 fm_subcarrier_noise_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fmsnth                          : 8;	//  0: 7
} FM_Subcarrier_Noise_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x014c fm_subcarrier_noise_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	fmsntl                          : 8;	//  0: 7
} FM_Subcarrier_Noise_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0150 nicam_error_rate_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nierth                          : 8;	//  0: 7
} NICAM_Error_Rate_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x0154 nicam_error_rate_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	niertl                          : 8;	//  0: 7
} NICAM_Error_Rate_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0158 pilot_mag_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pilmth                          : 8;	//  0: 7
} Pilot_Mag_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x015c pilot_mag_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	pilmtl                          : 8;	//  0: 7
} Pilot_Mag_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0160 carrier1_high_quality_aaos_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	q1hthh                          : 8;	//  0: 7
} Carrier1_High_Quality_AAOS_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x0164 carrier1_high_quality_aaos_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	q1hthl                          : 8;	//  0: 7
} Carrier1_High_Quality_AAOS_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0168 id_magnitude_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	idthh                           : 8;	//  0: 7
} ID_Magnitude_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x016c id_magnitude_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	idthl                           : 8;	//  0: 7
} ID_Magnitude_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0200 auto_system_detect_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	frfive                          : 3,	//  0: 2
	sxfive                          : 1;	//     3
} Auto_System_Detect_Ctrl;

/*-----------------------------------------------------------------------------
	0x0204 carrier_magnitude_thd ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	asmth                           : 8;	//  0: 7
} Carrier_Magnitude_Thd;

/*-----------------------------------------------------------------------------
	0x0208 carrier_average_freq_thd ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	asfth                           : 8;	//  0: 7
} Carrier_Average_Freq_Thd;

/*-----------------------------------------------------------------------------
	0x020c carrier_fm_quality_asd_thd ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	asqth                           : 8;	//  0: 7
} Carrier_FM_Quality_ASD_Thd;

/*-----------------------------------------------------------------------------
	0x0210 carrier_am_noise_asd_thd ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	aanth                           : 8;	//  0: 7
} Carrier_AM_Noise_ASD_Thd;

/*-----------------------------------------------------------------------------
	0x0214 nicam_noise_asd_thd ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	annth                           : 8;	//  0: 7
} NICAM_Noise_ASD_Thd;

/*-----------------------------------------------------------------------------
	0x0228 nicam_noise0 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nicns                           : 8;	//  0: 7
} NICAM_Noise0;

/*-----------------------------------------------------------------------------
	0x022c nicam_noise1 ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nicns                           : 8;	//  0: 7
} NICAM_Noise1;

/*-----------------------------------------------------------------------------
	0x0230 asd_carrier_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	carrier1freqgood        : 1,	//     0, 
	carrier1maggood   		: 1,	//     1, 
	asdcarrier1qualitygood  : 1,	// 	 2, 
	asdcarrier1good      	: 1,	//	 3, ASDCarrier1Good = Carrier1MagGood && Carrier1FreqGood && ASDCarrier1QualityGood
	carrier2freqgood        : 1,	//     4
	carrier2maggood         : 1,	//     5
	asdcarrier2qualitygood  : 1,	//     6, 
	asdcarrier2good         : 1;	//     7, ASDCarrier2Good = Carrier2MagGood && Carrier2FreqGood && ASDCarrier2QualityGood
} ASD_Carrier_Status;

/*-----------------------------------------------------------------------------
	0x0234 carrier_fm_quality_scs_thd ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sqlth                           : 8;	//  0: 7
} Carrier_FM_Quality_SCS_Thd;

/*-----------------------------------------------------------------------------
	0x0238 nicam_noise_scs_thd ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	snsth                           : 8;	//  0: 7
} NICAM_Noise_SCS_Thd;

/*-----------------------------------------------------------------------------
	0x023c scs_quality_status ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	scsdqpsk              : 1,	//     0, NICAM, NICAM DK
	scsdqpskqualitygood   : 1,	//     1, (w1SCSDQPSK && w1SCSDQPSKQualityGood) || (w1SCSFM && w1SCSFMQualityGood)
	scsfm                 : 1,	// 	 2, A2, DK1,DK2, DK3
	scsfmqualitygood      : 1,	//	 3, i16Carrier2FMQuality[15:3] >= {5'b0, i8FMQualitySCSThresh}
	car2freqgood          : 1,	//     4
	car2maggood           : 1,	//     5
	scsqualitygood        : 1,	//     6, (w1SCSDQPSK && w1SCSDQPSKQualityGood) || (w1SCSFM && w1SCSFMQualityGood)
	scsgood               : 1;	//     7, w1Carrier2MagGood && w1Carrier2FreqGood && w1SCSQualityGood
} SCS_Quality_Status;

/*-----------------------------------------------------------------------------
	0x0240 auto_volume_correction ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	avcen                           : 1,	//     0
	avcdcy                          : 2,	//  1: 2
	avcatk                          : 2;	//  3: 4
} Auto_Volume_Correction;

/*-----------------------------------------------------------------------------
	0x0244 level_prescaler_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	prectl0                         : 1;	//     0
} Level_Prescaler_Ctrl;

/*-----------------------------------------------------------------------------
	0x0248 prescaler_level_left_ch ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	levlft                          : 8;	//  0: 7
} Prescaler_Level_Left_Ch;

/*-----------------------------------------------------------------------------
	0x024c prescaler_level_right_ch ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	levlft                          : 8;	//  0: 7
} Prescaler_Level_Right_Ch;

/*-----------------------------------------------------------------------------
	0x0250 manual_output_level_left_ch ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	levlft                          : 8;	//  0: 7
} Manual_Output_Level_Left_Ch;

/*-----------------------------------------------------------------------------
	0x0254 manual_output_level_right_ch ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	levlrt                          : 8;	//  0: 7
} Manual_Output_Level_Right_Ch;

/*-----------------------------------------------------------------------------
	0x0258 avc_level_thd_hi ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	avchi                           : 8;	//  0: 7
} AVC_Level_Thd_Hi;

/*-----------------------------------------------------------------------------
	0x025c avc_level_thd_lo ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	avclo                           : 8;	//  0: 7
} AVC_Level_Thd_Lo;

/*-----------------------------------------------------------------------------
	0x0260 nicam_mono_prescaler_level ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	niamopr                         : 8;	//  0: 7
} NICAM_Mono_Prescaler_Level;

/*-----------------------------------------------------------------------------
	0x0264 nicam_prescaler_level ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	nicampr                         : 8;	//  0: 7
} NICAM_Prescaler_Level;

/*-----------------------------------------------------------------------------
	0x0268 a2_prescaler_level ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	a2pr                            : 8;	//  0: 7
} A2_Prescaler_Level;

/*-----------------------------------------------------------------------------
	0x026c legacy_levels_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	lglen                           : 1;	//     0
} Legacy_Levels_Ctrl;

/*-----------------------------------------------------------------------------
	0x02a8 sif_drop_interpolation_ctrl ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32
	sfden                           : 1;	//     0
} SIF_Drop_Interpolation_Ctrl;

typedef struct {
	System_Select                   	system_select                   ;	// 0x0000 : ''
	Standard_Select                 	standard_select                 ;	// 0x0004 : ''
	Sample_Rate                     	sample_rate                     ;	// 0x0008 : ''
	FM_AM_Demod_Ctrl                	fm_am_demod_ctrl                ;	// 0x000c : ''
	Demod1_Carrier_Freq0            	demod1_carrier_freq0            ;	// 0x0010 : ''
	Demod1_Carrier_Freq1            	demod1_carrier_freq1            ;	// 0x0014 : ''
	FM_DQPSK_Demod_Ctrl             	fm_dqpsk_demod_ctrl             ;	// 0x0018 : ''
	Demod2_Carrier_Freq0            	demod2_carrier_freq0            ;	// 0x001c : ''
	Demod2_Carrier_Freq1            	demod2_carrier_freq1            ;	// 0x0020 : ''
	Audio_Output_Ctrl               	audio_output_ctrl               ;	// 0x0024 : ''
	Audio_Mode_Detect               	audio_mode_detect               ;	// 0x0028 : ''
	UINT32                          	                 __rsvd_00[   1];	// 0x002c
	Carrier1_Average_Freq0          	carrier1_average_freq0          ;	// 0x0030 : ''
	Carrier1_Average_Freq1          	carrier1_average_freq1          ;	// 0x0034 : ''
	Carrier1_Phase_Noise0           	carrier1_phase_noise0           ;	// 0x0038 : ''
	Carrier1_Phase_Noise1           	carrier1_phase_noise1           ;	// 0x003c : ''
	Carrier1_Average_Mag0           	carrier1_average_mag0           ;	// 0x0040 : ''
	Carrier1_Average_Mag1           	carrier1_average_mag1           ;	// 0x0044 : ''
	Carrier1_Mag_Noise0             	carrier1_mag_noise0             ;	// 0x0048 : ''
	Carrier1_Mag_Noise1             	carrier1_mag_noise1             ;	// 0x004c : ''
	Carrier1_FM_Quality0            	carrier1_fm_quality0            ;	// 0x0050 : ''
	Carrier1_FM_Quality1            	carrier1_fm_quality1            ;	// 0x0054 : ''
	Carrier2_Average_Mag0           	carrier2_average_mag0           ;	// 0x0058 : ''
	Carrier2_Average_Mag1           	carrier2_average_mag1           ;	// 0x005c : ''
	Carrier2_Mag_Noise0             	carrier2_mag_noise0             ;	// 0x0060 : ''
	Carrier2_Mag_Noise1             	carrier2_mag_noise1             ;	// 0x0064 : ''
	Carrier2_FM_Quality0            	carrier2_fm_quality0            ;	// 0x0068 : ''
	Carrier2_FM_Quality1            	carrier2_fm_quality1            ;	// 0x006c : ''
	Pilot_Magnitude                 	pilot_magnitude                 ;	// 0x0070 : ''
	Pilot_PLL_Status                	pilot_pll_status                ;	// 0x0074 : ''
	FM_Subcarrier_Mag               	fm_subcarrier_mag               ;	// 0x0078 : ''
	FM_Subcarrier_Noise             	fm_subcarrier_noise             ;	// 0x007c : ''
	NICAM_Error_Rate0               	nicam_error_rate0               ;	// 0x0080 : ''
	NICAM_Error_Rate1               	nicam_error_rate1               ;	// 0x0084 : ''
	NICAM_Control_Bits0             	nicam_control_bits0             ;	// 0x0088 : ''
	NICAM_Control_Bits1             	nicam_control_bits1             ;	// 0x008c : ''
	AGC_Control                     	agc_control                     ;	// 0x0090 : ''
	AGC_Ctrl_for_AM                 	agc_ctrl_for_am                 ;	// 0x0094 : ''
	Pilot_Control                   	pilot_control                   ;	// 0x0098 : ''
	UINT32                          	                 __rsvd_01[   3];	// 0x009c ~ 0x00a4
	Status                          	status                          ;	// 0x00a8 : ''
	Status_Pin_Control              	status_pin_control              ;	// 0x00ac : ''
	Headroom                        	headroom                        ;	// 0x00b0 : ''
	AGC_Freeze_Control              	agc_freeze_control              ;	// 0x00b4 : ''
	Stereo_Carrier_Search_Ctrl      	stereo_carrier_search_ctrl      ;	// 0x00b8 : ''
	Stereo_Carrier_Search_Result    	stereo_carrier_search_result    ;	// 0x00bc : ''
	Carrier2_Average_Freq0          	carrier2_average_freq0          ;	// 0x00c0 : ''
	Carrier2_Average_Freq1          	carrier2_average_freq1          ;	// 0x00c4 : ''
	Stereo_ID_Magnitude             	stereo_id_magnitude             ;	// 0x00c8 : ''
	Bilingual_ID_Magnitude          	bilingual_id_magnitude          ;	// 0x00cc : ''
	UINT32                          	                 __rsvd_02[  12];	// 0x00d0 ~ 0x00fc
	Audio_Output_Result             	audio_output_result             ;	// 0x0100 : ''
	Mute_Control                    	mute_control                    ;	// 0x0104 : ''
	I2S_Control                     	i2s_control                     ;	// 0x0108 : ''
	Interrupt_Mask                  	interrupt_mask                  ;	// 0x010c : ''
	Manual_AGC                      	manual_agc                      ;	// 0x0110 : ''
	BTSC_En_Status                  	btsc_en_status                  ;	// 0x0114 : ''
	UINT32                          	                 __rsvd_03[   4];	// 0x0118 ~ 0x0124
	Carrier1_Quality_AAOS_Thd_Hi    	carrier1_quality_aaos_thd_hi    ;	// 0x0128 : ''
	Carrier1_Quality_AAOS_Thd_Lo    	carrier1_quality_aaos_thd_lo    ;	// 0x012c : ''
	Carrier2_Quality_AAOS_Thd_Hi    	carrier2_quality_aaos_thd_hi    ;	// 0x0130 : ''
	Carrier2_Quality_AAOS_Thd_Lo    	carrier2_quality_aaos_thd_lo    ;	// 0x0134 : ''
	Carrier1_Phase_Noise_AAOS_Thd_Hi	carrier1_phase_noise_aaos_thd_hi;	// 0x0138 : ''
	Carrier1_Phase_Noise_AAOS_Thd_Lo	carrier1_phase_noise_aaos_thd_lo;	// 0x013c : ''
	FM_Subcarrier_Mag_AAOS_Thd_Hi   	fm_subcarrier_mag_aaos_thd_hi   ;	// 0x0140 : ''
	FM_Subcarrier_Mag_AAOS_Thd_Lo   	fm_subcarrier_mag_aaos_thd_lo   ;	// 0x0144 : ''
	FM_Subcarrier_Noise_AAOS_Thd_Hi 	fm_subcarrier_noise_aaos_thd_hi ;	// 0x0148 : ''
	FM_Subcarrier_Noise_AAOS_Thd_Lo 	fm_subcarrier_noise_aaos_thd_lo ;	// 0x014c : ''
	NICAM_Error_Rate_AAOS_Thd_Hi    	nicam_error_rate_aaos_thd_hi    ;	// 0x0150 : ''
	NICAM_Error_Rate_AAOS_Thd_Lo    	nicam_error_rate_aaos_thd_lo    ;	// 0x0154 : ''
	Pilot_Mag_AAOS_Thd_Hi           	pilot_mag_aaos_thd_hi           ;	// 0x0158 : ''
	Pilot_Mag_AAOS_Thd_Lo           	pilot_mag_aaos_thd_lo           ;	// 0x015c : ''
	Carrier1_High_Quality_AAOS_Thd_Hi	carrier1_high_quality_aaos_thd_h;	// 0x0160 : ''
	Carrier1_High_Quality_AAOS_Thd_Lo	carrier1_high_quality_aaos_thd_l;	// 0x0164 : ''
	ID_Magnitude_Thd_Hi             	id_magnitude_thd_hi             ;	// 0x0168 : ''
	ID_Magnitude_Thd_Lo                	id_magnitude_thd_lo             ;	// 0x016c : ''
	UINT32                          	                 __rsvd_04[  36];	// 0x0170 ~ 0x01fc
	Auto_System_Detect_Ctrl         	auto_system_detect_ctrl         ;	// 0x0200 : ''
	Carrier_Magnitude_Thd           	carrier_magnitude_thd           ;	// 0x0204 : ''
	Carrier_Average_Freq_Thd        	carrier_average_freq_thd        ;	// 0x0208 : ''
	Carrier_FM_Quality_ASD_Thd      	carrier_fm_quality_asd_thd      ;	// 0x020c : ''
	Carrier_AM_Noise_ASD_Thd        	carrier_am_noise_asd_thd        ;	// 0x0210 : ''
	NICAM_Noise_ASD_Thd             	nicam_noise_asd_thd             ;	// 0x0214 : ''
	UINT32                          	                 __rsvd_05[   4];	// 0x0218 ~ 0x0224
	NICAM_Noise0                    	nicam_noise0                    ;	// 0x0228 : ''
	NICAM_Noise1                    	nicam_noise1                    ;	// 0x022c : ''
	ASD_Carrier_Status                  asd_carrier_status				;	// 0x0230 : ''
	Carrier_FM_Quality_SCS_Thd      	carrier_fm_quality_scs_thd      ;	// 0x0234 : ''
	NICAM_Noise_SCS_Thd             	nicam_noise_scs_thd             ;	// 0x0238 : ''
	SCS_Quality_Status                  scs_quality_status				;	// 0x023c : ''
	Auto_Volume_Correction          	auto_volume_correction          ;	// 0x0240 : ''
	Level_Prescaler_Ctrl            	level_prescaler_ctrl            ;	// 0x0244 : ''
	Prescaler_Level_Left_Ch         	prescaler_level_left_ch         ;	// 0x0248 : ''
	Prescaler_Level_Right_Ch        	prescaler_level_right_ch        ;	// 0x024c : ''
	Manual_Output_Level_Left_Ch     	manual_output_level_left_ch     ;	// 0x0250 : ''
	Manual_Output_Level_Right_Ch    	manual_output_level_right_ch    ;	// 0x0254 : ''
	AVC_Level_Thd_Hi                	avc_level_thd_hi                ;	// 0x0258 : ''
	AVC_Level_Thd_Lo                	avc_level_thd_lo                ;	// 0x025c : ''
	NICAM_Mono_Prescaler_Level      	nicam_mono_prescaler_level      ;	// 0x0260 : ''
	NICAM_Prescaler_Level           	nicam_prescaler_level           ;	// 0x0264 : ''
	A2_Prescaler_Level              	a2_prescaler_level              ;	// 0x0268 : ''
	Legacy_Levels_Ctrl              	legacy_levels_ctrl              ;	// 0x026c : ''
	UINT32                          	                 __rsvd_06[  14];	// 0x0270 ~ 0x02a4
	SIF_Drop_Interpolation_Ctrl     	sif_drop_interpolation_ctrl     ;	// 0x02a8 : ''
} AAD_REG_T;
/* 95 regs, 95 types */
/* 95 regs, 95 types in Total*/

/*
 * @{
 * Naming for register pointer.
 * gpRealRegAAD : real register of AAD.
 * gpRegAAD     : shadow register.
 *
 * @def AAD_RdFL: Read  FLushing : Shadow <- Real.
 * @def AAD_WrFL: Write FLushing : Shadow -> Real.
 * @def AAD_Rd  : Read  whole register(UINT32) from Shadow register.
 * @def AAD_Wr  : Write whole register(UINT32) from Shadow register.
 * @def AAD_Rd01 ~ AAD_Rdnn: Read  given '01~nn' fields from Shadow register.
 * @def AAD_Wr01 ~ AAD_Wrnn: Write given '01~nn' fields to   Shadow register.
 * */
#define AAD_RdFL(_r)			((gpRegAAD->_r)=(gpRealRegAAD->_r))
#define AAD_WrFL(_r)			((gpRealRegAAD->_r)=(gpRegAAD->_r))

#define AAD_Rd(_r)			*((UINT32*)(&(gpRegAAD->_r)))
#define AAD_Wr(_r,_v)			((AAD_Rd(_r))=((UINT32)(_v)))

#define AAD_Rd01(_r,_f01,_v01)													\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
								} while(0)

#define AAD_Rd02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
								} while(0)

#define AAD_Rd03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
								} while(0)

#define AAD_Rd04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
								} while(0)

#define AAD_Rd05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
								} while(0)

#define AAD_Rd06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
								} while(0)

#define AAD_Rd07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
								} while(0)

#define AAD_Rd08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
								} while(0)

#define AAD_Rd09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
								} while(0)

#define AAD_Rd10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
									(_v10) = (gpRegAAD->_r._f10);				\
								} while(0)

#define AAD_Rd11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
									(_v10) = (gpRegAAD->_r._f10);				\
									(_v11) = (gpRegAAD->_r._f11);				\
								} while(0)

#define AAD_Rd12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
									(_v10) = (gpRegAAD->_r._f10);				\
									(_v11) = (gpRegAAD->_r._f11);				\
									(_v12) = (gpRegAAD->_r._f12);				\
								} while(0)

#define AAD_Rd13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
									(_v10) = (gpRegAAD->_r._f10);				\
									(_v11) = (gpRegAAD->_r._f11);				\
									(_v12) = (gpRegAAD->_r._f12);				\
									(_v13) = (gpRegAAD->_r._f13);				\
								} while(0)

#define AAD_Rd14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
									(_v10) = (gpRegAAD->_r._f10);				\
									(_v11) = (gpRegAAD->_r._f11);				\
									(_v12) = (gpRegAAD->_r._f12);				\
									(_v13) = (gpRegAAD->_r._f13);				\
									(_v14) = (gpRegAAD->_r._f14);				\
								} while(0)

#define AAD_Rd15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
									(_v10) = (gpRegAAD->_r._f10);				\
									(_v11) = (gpRegAAD->_r._f11);				\
									(_v12) = (gpRegAAD->_r._f12);				\
									(_v13) = (gpRegAAD->_r._f13);				\
									(_v14) = (gpRegAAD->_r._f14);				\
									(_v15) = (gpRegAAD->_r._f15);				\
								} while(0)

#define AAD_Rd16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(_v01) = (gpRegAAD->_r._f01);				\
									(_v02) = (gpRegAAD->_r._f02);				\
									(_v03) = (gpRegAAD->_r._f03);				\
									(_v04) = (gpRegAAD->_r._f04);				\
									(_v05) = (gpRegAAD->_r._f05);				\
									(_v06) = (gpRegAAD->_r._f06);				\
									(_v07) = (gpRegAAD->_r._f07);				\
									(_v08) = (gpRegAAD->_r._f08);				\
									(_v09) = (gpRegAAD->_r._f09);				\
									(_v10) = (gpRegAAD->_r._f10);				\
									(_v11) = (gpRegAAD->_r._f11);				\
									(_v12) = (gpRegAAD->_r._f12);				\
									(_v13) = (gpRegAAD->_r._f13);				\
									(_v14) = (gpRegAAD->_r._f14);				\
									(_v15) = (gpRegAAD->_r._f15);				\
									(_v16) = (gpRegAAD->_r._f16);				\
								} while(0)


#define AAD_Wr01(_r,_f01,_v01)													\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
								} while(0)

#define AAD_Wr02(_r,_f01,_v01,_f02,_v02)										\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
								} while(0)

#define AAD_Wr03(_r,_f01,_v01,_f02,_v02,_f03,_v03)								\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
								} while(0)

#define AAD_Wr04(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04)					\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
								} while(0)

#define AAD_Wr05(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05)													\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
								} while(0)

#define AAD_Wr06(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06)										\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
								} while(0)

#define AAD_Wr07(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07)								\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
								} while(0)

#define AAD_Wr08(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08)					\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
								} while(0)

#define AAD_Wr09(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09)													\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
								} while(0)

#define AAD_Wr10(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10)										\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
									(gpRegAAD->_r._f10) = (_v10);				\
								} while(0)

#define AAD_Wr11(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11)								\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
									(gpRegAAD->_r._f10) = (_v10);				\
									(gpRegAAD->_r._f11) = (_v11);				\
								} while(0)

#define AAD_Wr12(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12)					\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
									(gpRegAAD->_r._f10) = (_v10);				\
									(gpRegAAD->_r._f11) = (_v11);				\
									(gpRegAAD->_r._f12) = (_v12);				\
								} while(0)

#define AAD_Wr13(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13)													\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
									(gpRegAAD->_r._f10) = (_v10);				\
									(gpRegAAD->_r._f11) = (_v11);				\
									(gpRegAAD->_r._f12) = (_v12);				\
									(gpRegAAD->_r._f13) = (_v13);				\
								} while(0)

#define AAD_Wr14(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14)										\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
									(gpRegAAD->_r._f10) = (_v10);				\
									(gpRegAAD->_r._f11) = (_v11);				\
									(gpRegAAD->_r._f12) = (_v12);				\
									(gpRegAAD->_r._f13) = (_v13);				\
									(gpRegAAD->_r._f14) = (_v14);				\
								} while(0)

#define AAD_Wr15(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15)								\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
									(gpRegAAD->_r._f10) = (_v10);				\
									(gpRegAAD->_r._f11) = (_v11);				\
									(gpRegAAD->_r._f12) = (_v12);				\
									(gpRegAAD->_r._f13) = (_v13);				\
									(gpRegAAD->_r._f14) = (_v14);				\
									(gpRegAAD->_r._f15) = (_v15);				\
								} while(0)

#define AAD_Wr16(_r,_f01,_v01,_f02,_v02,_f03,_v03,_f04,_v04,					\
					_f05,_v05,_f06,_v06,_f07,_v07,_f08,_v08,					\
					_f09,_v09,_f10,_v10,_f11,_v11,_f12,_v12,					\
					_f13,_v13,_f14,_v14,_f15,_v15,_f16,_v16)					\
								do { 											\
									(gpRegAAD->_r._f01) = (_v01);				\
									(gpRegAAD->_r._f02) = (_v02);				\
									(gpRegAAD->_r._f03) = (_v03);				\
									(gpRegAAD->_r._f04) = (_v04);				\
									(gpRegAAD->_r._f05) = (_v05);				\
									(gpRegAAD->_r._f06) = (_v06);				\
									(gpRegAAD->_r._f07) = (_v07);				\
									(gpRegAAD->_r._f08) = (_v08);				\
									(gpRegAAD->_r._f09) = (_v09);				\
									(gpRegAAD->_r._f10) = (_v10);				\
									(gpRegAAD->_r._f11) = (_v11);				\
									(gpRegAAD->_r._f12) = (_v12);				\
									(gpRegAAD->_r._f13) = (_v13);				\
									(gpRegAAD->_r._f14) = (_v14);				\
									(gpRegAAD->_r._f15) = (_v15);				\
									(gpRegAAD->_r._f16) = (_v16);				\
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
 * _rwname: name of rw    bit field : shall be 0 after AAD_Wind(), 1 for AAD_Rind()
 * _iname : name of index bit field
 * _ival  : index value
 * _fname : field name
 * _fval  : field variable that field value shall be stored.
 *
 * AAD_Rind : General indexed register Read.(
 * AAD_Wind : General indexed register Read.
 *
 * AAD_Ridx : For 'index', 'rw', 'load' field name
 * AAD_Widx : For 'index', 'rw', 'load' field name and NO_LOAD.
 */
#define AAD_Rind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
							do {												\
								AAD_Wr03(_r,_lname,0,_rwname,1,_iname,_ival);	\
								AAD_WrFL(_r);									\
								AAD_RdFL(_r);									\
								AAD_Rd01(_r,_fname,_fval);						\
							} while (0)

#define AAD_Wind(_r, _lname, _rwname, _iname, _ival, _fname, _fval)				\
				AAD_Wr04(_r, _lname,0, _rwname,0, _iname,_ival, _fname,_fval)


#define AAD_Ridx(_r, _ival, _fname, _fval)	AAD_Rind(_r,load,rw,index,_ival,_fname,_fval)

#define AAD_Widx(_r, _ival, _fname, _fval)	AAD_Wind(_r,load,rw,index,_ival,_fname,_fval)

/** @} *//* end of macro documentation */

#ifdef __cplusplus
}
#endif

#endif	/* _AAD_REG_H_ */

/* from 'LG1150-AAD-MAN-v0.61.csv' 20101207 04:12:22   KOR by getregs v2.7 */
