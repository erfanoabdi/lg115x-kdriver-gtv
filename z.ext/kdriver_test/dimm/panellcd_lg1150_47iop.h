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


/** @file panellcd_lg1150_47iop.h
 *
 * This source file defines .....
 *
 *  @author
 *  @version	0.1
 *  @date		2010.05.19
 *  @note
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	_PANELLCD_LG1150_47IOP_H_
#define	_PANELLCD_LG1150_47IOP_H_
/******************************************************************************
 #include 파일들 (File Inclusions)
******************************************************************************/

/******************************************************************************
    매크로 함수 정의 (Macro Definitions)
******************************************************************************/

/******************************************************************************
   로컬 상수 정의 (Local Constant Definitions)
******************************************************************************/

/******************************************************************************
    로컬 형 정의 (Local Type Definitions)
******************************************************************************/

/******************************************************************************
    전역 형 정의 (Global Type Definitions)
******************************************************************************/


/******************************************************************************
    Extern 전역변수와 함수 prototype 선언
    (Extern Variables & Function Prototype Declarations)
******************************************************************************/

/******************************************************************************
    Static 변수와 함수 prototype 선언
    (Static Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    로컬 변수와 함수 prototype 선언
    (Local Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
    전역 변수와 함수 prototype 선언
    (Global Variables & Function Prototypes Declarations)
******************************************************************************/

/******************************************************************************
	함수 정의
	(Function Definitions)
******************************************************************************/
#define LOCALDIMMING_47IOP_DATA_LENGTH	3600
#define LOCALDIMMING_47IOP_LUT_LENGTH	289
const UINT32 gLocalDimming_47Iop_AAL[LOCALDIMMING_47IOP_DATA_LENGTH] =
{
	//*********** IOP 47Inch ***********//

	// The vertical & horizontal number of blocks : Add = 0x104,  VBN, HBN
	 9,		// VBN
	24,		// HBN

	// The vertical & horizontal size of block : Add = 0x100,  VSB(1080/VBN) = 120, HSB(1920/HBN) = 80
	120,	// VSB
	 80,	// HSB

	// Horizontal SBI coefficient : Add = 0x220, The number of coefficients - Direct : HSB * 5 => 80 * 5 =400
	// Edge (Top/bottom) = 1080, Edge (Left/Right) = 1920
    0,    76,   441,   433,    74,    0,    78,   447,   427,    72,    1,    80,   453,   420,    70,
    1,    82,   460,   413,    68,    2,    85,   464,   406,    67,    2,    87,   471,   399,    65,
    2,    89,   478,   392,    63,    3,    91,   484,   385,    61,    3,    93,   490,   378,    60,
    4,    96,   495,   371,    58,    4,    98,   501,   365,    56,    5,   101,   505,   358,    55,
    5,   103,   512,   351,    53,    5,   106,   517,   344,    52,    6,   108,   522,   338,    50,
    6,   111,   527,   331,    49,    7,   113,   532,   325,    47,    7,   116,   537,   318,    46,
    8,   119,   540,   312,    45,    8,   122,   546,   305,    43,    9,   125,   549,   299,    42,
   10,   128,   552,   293,    41,   10,   131,   557,   287,    39,   11,   134,   560,   281,    38,
   11,   137,   564,   275,    37,   12,   140,   567,   269,    36,   12,   144,   569,   264,    35,
   13,   147,   572,   258,    34,   14,   150,   575,   252,    33,   14,   154,   577,   247,    32,
   15,   157,   579,   242,    31,   16,   161,   581,   236,    30,   16,   165,   583,   231,    29,
   17,   169,   584,   226,    28,   18,   173,   585,   221,    27,   18,   177,   587,   216,    26,
   19,   181,   587,   212,    25,   20,   185,   588,   207,    24,   21,   189,   589,   202,    23,
   22,   193,   589,   198,    22,   22,   198,   589,   193,    22,   23,   202,   589,   189,    21,
   24,   207,   588,   185,    20,   25,   212,   587,   181,    19,   26,   216,   587,   177,    18,
   27,   221,   585,   173,    18,   28,   226,   584,   169,    17,   29,   231,   583,   165,    16,
   30,   236,   581,   161,    16,   31,   242,   579,   157,    15,   32,   247,   577,   154,    14,
   33,   252,   575,   150,    14,   34,   258,   572,   147,    13,   35,   264,   569,   144,    12,
   36,   269,   567,   140,    12,   37,   275,   564,   137,    11,   38,   281,   560,   134,    11,
   39,   287,   557,   131,    10,   41,   293,   552,   128,    10,   42,   299,   549,   125,     9,
   43,   305,   546,   122,     8,   45,   312,   540,   119,     8,   46,   318,   537,   116,     7,
   47,   325,   532,   113,     7,   49,   331,   527,   111,     6,   50,   338,   522,   108,     6,
   52,   344,   517,   106,     5,   53,   351,   512,   103,     5,   55,   358,   505,   101,     5,
   56,   365,   501,    98,     4,   58,   371,   495,    96,     4,   60,   378,   490,    93,     3,
   61,   385,   484,    91,     3,   63,   392,   478,    89,     2,   65,   399,   471,    87,     2,
   67,   406,   464,    85,     2,   68,   413,   460,    82,     1,   70,   420,   453,    80,     1,
   72,   427,   447,    78,     0,   74,   433,   441,    76,     0,

	// Vertical SBI coefficient : Add = 0x224, The number of coefficients - Direct : VSB * 5 => 120 * 5 = 600
	// Edge (Top/bottom) = 1080, Edge (Left/Right) = 1920
	0,    20,   500,   485,    19,    0,    20,   515,   470,    19,    0,    21,   530,   455,    18,
    0,    21,   545,   440,    18,    0,    22,   558,   426,    18,    0,    22,   574,   411,    17,
    0,    22,   588,   397,    17,    0,    23,   603,   382,    16,    0,    23,   616,   369,    16,
    0,    24,   630,   355,    15,    0,    24,   644,   341,    15,    1,    24,   657,   328,    14,
    1,    25,   669,   315,    14,    1,    25,   682,   303,    13,    1,    25,   694,   291,    13,
    1,    26,   706,   279,    12,    1,    26,   718,   267,    12,    1,    26,   729,   256,    12,
    1,    26,   741,   245,    11,    1,    27,   750,   235,    11,    1,    27,   761,   225,    10,
    1,    27,   771,   215,    10,    1,    28,   779,   206,    10,    1,    28,   789,   197,     9,
    1,    28,   797,   189,     9,    1,    28,   806,   180,     9,    1,    29,   814,   172,     8,
    1,    29,   821,   165,     8,    1,    29,   828,   158,     8,    1,    29,   836,   151,     7,
    1,    30,   842,   144,     7,    1,    30,   848,   138,     7,    1,    30,   854,   132,     7,
    1,    31,   859,   127,     6,    1,    31,   865,   121,     6,    1,    31,   870,   116,     6,
    1,    32,   874,   111,     6,    2,    32,   878,   107,     5,    2,    32,   883,   102,     5,
    2,    33,   886,    98,     5,    2,    33,   890,    94,     5,    2,    34,   893,    90,     5,
    2,    34,   896,    87,     5,    2,    35,   900,    83,     4,    2,    35,   903,    80,     4,
    2,    36,   905,    77,     4,    2,    36,   908,    74,     4,    2,    37,   909,    72,     4,
    2,    37,   912,    69,     4,    2,    38,   913,    67,     4,    2,    39,   915,    65,     3,
    2,    40,   917,    62,     3,    2,    40,   919,    60,     3,    2,    41,   919,    59,     3,
    2,    42,   920,    57,     3,    2,    43,   921,    55,     3,    2,    44,   922,    53,     3,
    2,    45,   922,    52,     3,    3,    47,   921,    50,     3,    3,    48,   921,    49,     3,
    3,    49,   921,    48,     3,    3,    50,   921,    47,     3,    3,    52,   922,    45,     2,
    3,    53,   922,    44,     2,    3,    55,   921,    43,     2,    3,    57,   920,    42,     2,
    3,    59,   919,    41,     2,    3,    60,   919,    40,     2,    3,    62,   917,    40,     2,
    3,    65,   915,    39,     2,    4,    67,   913,    38,     2,    4,    69,   912,    37,     2,
    4,    72,   909,    37,     2,    4,    74,   908,    36,     2,    4,    77,   905,    36,     2,
    4,    80,   903,    35,     2,    4,    83,   900,    35,     2,    5,    87,   896,    34,     2,
    5,    90,   893,    34,     2,    5,    94,   890,    33,     2,    5,    98,   886,    33,     2,
    5,   102,   883,    32,     2,    5,   107,   878,    32,     2,    6,   111,   874,    32,     1,
    6,   116,   870,    31,     1,    6,   121,   865,    31,     1,    6,   127,   859,    31,     1,
    7,   132,   854,    30,     1,    7,   138,   848,    30,     1,    7,   144,   842,    30,     1,
    7,   151,   836,    29,     1,    8,   158,   828,    29,     1,    8,   165,   821,    29,     1,
    8,   172,   814,    29,     1,    9,   180,   806,    28,     1,    9,   189,   797,    28,     1,
    9,   197,   789,    28,     1,   10,   206,   779,    28,     1,   10,   215,   771,    27,     1,
   10,   225,   761,    27,     1,   11,   235,   750,    27,     1,   11,   245,   741,    26,     1,
   12,   256,   729,    26,     1,   12,   267,   718,    26,     1,   12,   279,   706,    26,     1,
   13,   291,   694,    25,     1,   13,   303,   682,    25,     1,   14,   315,   669,    25,     1,
   14,   328,   657,    24,     1,   15,   341,   644,    24,     0,   15,   355,   630,    24,     0,
   16,   369,   616,    23,     0,   16,   382,   603,    23,     0,   17,   397,   588,    22,     0,
   17,   411,   574,    22,     0,   18,   426,   558,    22,     0,   18,   440,   545,    21,     0,
   18,   455,   530,    21,     0,   19,   470,   515,    20,     0,   19,   485,   500,    20,     0,

	// Pixel Compensation LUT A ~ D: Add = 0x228(A), 0x22C(B), 0x230(C), 0x234(D)
	// The number of coefficients(289) : A = 81, B = 72, C = 72, D = 64
	460,	945,	1023,	982,	869,	705,	501,	245,	0,		292,	600,	650,	624,	552,	448,	318,	156,	0,
	166,	341,	370,	355,	314,	255,	181,	88,		0,		102,	209,	227,	217,	192,	156,	111,	54,		0,
	67,		138,	150,	144,	127,	103,	73,		36,		0,		43,		89,		97,		93,		82,		66,		47,		23,		0,
	25,		52,		57,		54,		48,		39,		27,		13,		0,		11,		23,		25,		24,		21,		17,		12,		6,		0,
	0,		0,		0,		0,		0,		0,		0,		0,		0,

	787,	1023,	1012,	930,	787,	603,	378,	112,	500,	650,	643,	591,	500,	383,	240,	71,
	284,	370,	366,	336,	284,	218,	136,	40,		174,	227,	224,	206,	174,	133,	83,		24,
	115,	150,	148,	136,	115,	88,		55,		16,		74,		97,		96,		88,		74,		57,		35,		10,
	43,		57,		56,		51,		43,		33,		21,		6,		19,		25,		24,		22,		19,		14,		9,		2,
	0,		0,		0,		0,		0,		0,		0,		0,

	460,	945,	1023,	982,	869,	705,  	501,	245,	0,		220,	452,	490,	470,	416,	338,	240,	117,	0,
	126,	260,	282,	270,	239,	194,	138,	67,		0,		82,		170,	184,	176,	156,	126,	90,		44,		0,
	54,		111,	121,	116,	102,	83,		59,		29,		0,		33,		69,		75,		72,		63,		51,		36,		18,		0,
	18,		36,		40,		38,		34,		27,		19,		9,		0,		5,		11,		12,		11,		10,		8,		5,		2,		0,

	787,	1023,	1012,	930,	787,	603,	378,	112,	377,	490,	485,	445,	377,	289,	181,	53,
	217,	282,	279,	256,	217,	166,	104,	31,		141,	184,	182,	167,	141,	108,	68,		20,
	93,		121,	119,	110,	93,		71,		44,		13,		57,		75,		74,		68,		57,		44,		27,		8,
	30,		40,		39,		36,		30,		23,		14,		4,		9,		12,		11,		10,		9,		7,		4,		1,

	// Dimming Curve(Min) BV_UPPER_LUT : Add = 0x238  The number of coefficients : 1024
	0,		0,		0,		0,		1,		1,		1,		1,		1,		1,		1,		1,
	2,		2,		2,		2,		2,		2,		2,		3,		3,		3,		3,		3,
	4,		4,		4,		4,		4,		4,		5,		5,		5,		5,		5,		5,
	6,		6,		6,		6,		6,		6,		6,		6,		7,		7,		7,		7,
	7,		8,		8,		9,		9,		10,		10,		11,		11,		12,		12,		13,
	13,		14,		14,		15,		15,		16,		17,		17,		18,		19,		20,		21,
	22,		22,		23,		24,		25,		26,		26,		27,		28,		29,		30, 	31,
	32,		33,		34,		35,		36,		36,		37,		38,		39,		40,		41,		42,
	43,		44,		45,		46,		47,		48,		49,		50,		52,		53,		54,		55,
	56,		57,		58,		59,		60,		61,		63,		64,		65,		66,		68,		69,
	70,		71,		73,		74,		75,		76,		78,		79,		80,		82,		83,		85,
	86,		88,		89,		91,		93,		94,		96,		97,		99, 	100,	102,	103,
	105,	107,	108,	110,	111,	113,	114,	116,	118,	119,	121,	122,
	124,	125,	127,	128,	130,	132,	135,	137,	140,	142,	144,	147,
	149,	152,	154,	156,	159,	161,	164,	166,	169,	171,	173,	176,
	178,	181,	183,	185,	188,	190,	193,	195,	197,	200,	202,	205,
	207,	210,	213,	216,	219,	222,	226,	229,	232,	235,	238,	241,
	244,	247,	250,	253,	257,	260,	263,	266,	269,	272,	275,	278,
	281,	284,	287,	291,	294,	297,	300,	303,	306,	309,	313,	316,
	320,	323,	327,	330,	334,	337,	340,	344,	347,	351,	354,	358,
	361,	364,	368,	371,	375,	378,	382,	385,	389,	392,	395,	399,
	402,	406,	409,	413,	416,	420,	424,	428,	431,	435,	439,	443,
	447,	451,	454,	458,	462,	466,	470,	474,	478,	481,	485,	489,
	493,	497,	501,	504,	508,	512,	516,	520,	524,	527,	531,	535,
	539,	543,	547,	551,	555,	559,	563,	567,	571,	575,	579,	583,
	587,	591,	595,	599,	603,	606,	610,	614,	618,	622,	626,	630,
	634,	638,	642,	646,	650,	654,	658,	662,	666,	669,	672,	676,
	679,	682,	685,	688,	692,	695,	698,	701,	704,	707,	711,	714,
	717,	720,	723,	727,	730,	733,	736,	739,	743,	746,	749,	752,
	755,	758,	762,	765,	768,	771,	774,	777,	780,	783,	786,	789,
	792,	795,	798,	801,	804,	807,	810,	813,	817,	820,	823,	826,
	829,	832,	835,	838,	841,	844,	847,	850,	853,	856,	859,	862,
	865,	867,	869,	871,	874,	876,	878,	880,	882,	884,	887,	889,
	891,	893,	895,	897,	900,	902,	904,	906,	908,	910,	912,	915,
	917,	919,	921,	923,	925,	928,	930,	932,	934,	935,	936,	937,
	939,	940,	941,	942,	943,	944,	945,	946,	948,	949,	950,	951,
	952,	953,	954,	955,	957,	958,	959,	960,	961,	962,	963,	964,
	966,	967,	968,	969,	970,	971,	972,	973,	975,	976,	977,	978,
	979,	980,	981,	982,	984,	985,	986,	987,	988,	989,	990,	991,
	993,	994,	995,	996,	997,	998,	999,	1000,	1002,	1003,	1004,	1005,
	1006,	1007,	1007,	1008,	1008,	1009,	1009,	1010,	1010,	1011,	1011,	1012,
	1012,	1013,	1013,	1014,	1015,	1015,	1016,	1016,	1017,	1017,	1018,	1018,
	1019,	1019,	1020,	1020,	1021,	1021,	1022,	1022,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,

	// Dimming Curve(Max) BV_LOWER_LUT : Add = 0x23C  The number of coefficients : 1024
	0, 		1, 		2, 		3, 		4, 		5, 		6, 		7, 		8, 		8, 		9, 		10,
	11, 	12, 	13, 	14, 	15,		16, 	18, 	19, 	20, 	22, 	23, 	24,
	26,		27,		28,		29,		31,		32,		33,	 	35,	 	36,	 	38,	 	39,	 	41,
	42,	 	44,	 	45,	 	47,	 	48,	 	50,	 	51,	 	53,	 	54,	 	56,	 	57,	 	59,
	60,	 	62,	 	65,	 	67,	 	69,	 	72,	 	74,	 	76,	 	79,	 	81,	 	83,	 	85,
	88,	 	90,	 	92,	  	95,	 	97,	 	100, 	103, 	106, 	109, 	111, 	114, 	117,
	120,	123,	126,	129,	132,	134,	137,	140,	143,	147,	151,	155,
	159,	163,	167,	171,	175,	179,	183,	187,	191,	195,	199,	203,
	207,	212,	218,	223,	229,	234,	240,	245,	251,	256,	261,	267,
	272,	278,	283,	289,	294,	301,	308,	315,	322,	329,	336,	343,
	350,	357,	364,	371,	378,	385,	392,	399,	406,	413,	421,	428,
	435,	442,	450,	457,	464,	471,	479,	486,	493,	500,	508,	515,
	522,	529,	537,	544,	551,	558,	566,	573,	580,	587,	595,	602,
	609,	616,	624,	631,	638,	644,	 649,	655,	660,	666,	671,	677,
	682,	688,	693,	699,	704,	710,	715,	721,	726,	732,	737,	743,
	748,	754,	759,	765,	770,	776,	781,	787,	792,	798,	803,	809,
	814,	818,	821,	825,	828,	832,	836,	839,	843,	846,	850,	854,
	857,	861,	864,	868,	872,	875,	879,	882,	886,	889,	893,	897,
	900,	904,	907,	911,	915,	918,	922,	925,	929,	931,	933,	935,
	937,	939,	940,	942,	944,	946,	948,	950,	952,	954,	956,	958,
	960,	961,	963,	965,	967,	969,	971,	973,	975,	977,	979,	980,
	982,	984,	986,	988,	990,	991,	992,	993,	994,	995,	996,	997,
	998,	999,	1000, 	1001, 	1002, 	1003,	1004,	1005,	1007,	1008,	1009,	1010,
	1011,	1012,	1013,	1014,	1015,	1016,	1017,	1018,	1019,	1020,	1021,	1022,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,	1023,
	1023,	1023,	1023,	1023,

	// Horizontal weighted average coefficient : Add = 0x218  The number of coefficients : HSB(80)
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,

	// Vertical weighted average coefficient : Add = 0x21C  The number of coefficients : VSB(120)
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,
	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,	255,

	// Umask coefficient : Add = 0x0E0  The number of coefficients : 25
	3,		3,		3,		3,		3,
	6,		10,		16,		10,		6,
	36,		150,	461,	151,	36,
	11,		25,		33,		24,		11,
	4,		5,		5,		5,		4,

	// SPI I/F Timing Setting (SCLK T1 ~ T5) : Add = 0x10C, 0x110, 0x114, 0x118, 0x138
	249, 	20,		20,		249,	31,

	// BLU Block Configuraion
	// Start point of BV segment for SPI(BLK SP0 ~ SP3) : Add = 0x144, 0x148, 0x14C, 0x150
	16,		8,		0,		0,
	// Horizontal BV number of BV segment for SPI(BNH SEG0 ~ SEG3) : Add = 0x154, 0x158, 0x15C, 0x160
	8,		8,		8,		0,
	// Vertical BV number of BV segment for SPI(BNV SEG0 ~ SEG3) : Add = 0x164, 0x168, 0x16C, 0x170
	9,		9,		9,		0,

	0,		// BLU VS Mode : Add = 0x1B0

	// MAX_OPT (MAX OPT, MAX OPT MODE, MAX OPT 0_1, LEVEL TH, NUMBER TH) : Add = 0x024, 0x084, 0x088, 0x08C, 0x090
	20,		1,	16639,	  800,		86,

	1,		// BLK MAX Mode : Add = 0x028
	1023,	// NLF_TH(Threshold value for the Non-Linear Filter)  : Add = 0x02C
	64,		// A_PARA(Weighting factor of the temporal dilter))  : Add = 0x030
	0,		// Weighting average enable : Add = 0x020
	1,		// LUT Mode : Add = 0x0D0
	0,		// UMASK_EN : Add = 0x0E4
	0,		// SCD_EN : : Add = 0x040
	4,		// PC GAIN Precision : Add = 0x09C
	0,		// PIXEL GAIN P5 : Add = 0x058
	1,		// SPI Channel mask : Add = 0x174
	0, 		// Run Stall for DSP : Add = 0x210
};

const UINT32 gLocalDimming_47Iop_OFF[3][LOCALDIMMING_47IOP_LUT_LENGTH] =
{
	{
		//47 IOP PC LUT Off - A
		951, 1012, 1023,  982,  869,  705,  501,  245,   0,
		604,  643,  650,  624,	552,  448,  318,  156,   0,
		344,  366,  370,  355,  314,  255,  181,   88,   0,
		211,  224,  227,  217,  192,  156,  111,   54,   0,
		139,  148,  150,  144,  127,  103,   73,   36,    0,
		 90,   96,   97,   93,   82,   66,   47,   23,    0,
		 53,   56,   57,   54,   48,   39,   27,   13,    0,
		 23,   24,   25,   24,   21,   17,   12,    6,    0,
		  0,    0,    0,    0,    0,    0,    0,    0,    0,
		//47 IOP PC LUT Off - B
		992, 1023, 1012,  930,  787,  603,  378,  112,
		630,  650,  643,  591,  500,  383,  240,   71,
		358,  370,  366,  336,  284,  218,  136,   40,
		220,  227,  224,  206,  174,  133,   83,   24,
		145,  150,  148,  136,  115,   88,   55,   16,
		 94,   97,   96,   88,   74,   57,   35,   10,
		 55,   57,   56,   51,   43,   33,   21,    6,
		 24,   25,   24,   22,   19,   14,    9,    2,
		  0,    0,    0,    0,    0,    0,    0,    0,
		//47 IOP PC LUT Off - C
		951, 1012, 1023,  982,  869,  705,  501,  245,    0,
		455,  485,  490,  470,  416,  338,  240,  117,    0,
		262,  279,  282,  270,  239,  194,  138,   67,    0,
		171,  182,  184,  176,  156,  126,   90,   44,    0,
		112,  119,  121,  116,  102,   83,   59,   29,    0,
		 69,   74,   75,   72,   63,   51,   36,   18,    0,
		 37,   39,   40,   38,   34,   27,   19,    9,    0,
		 11,   11,   12,   11,   10,    8,    5,    2,    0,

		//47 IOP PC LUT Off - D
		992, 1023, 1012,  930,  787,  603,  378,  112,
		475,  490,  485,  445,  377,  289,  181,   53,
		273,  282,  279,  256,  217,  166,  104,   31,
		178,  184,  182,  167,  141,  108,   68,   20,
		117,  121,  119,  110,   93,   71,   44,   13,
		 72,   75,   74,   68,   57,   44,   27,    8,
		 38,   40,   39,   36,   30,   23,   14,    4,
		 11,   12,   11,   10,    9,    7,    4,    1,
	},
	{
		//47 IOP PC LUT Off - A
		1000, 10121, 10232,  982,  869,  705,  501,  245,   0,
		604,  643,  650,  624,	552,  448,  318,  156,   0,
		344,  366,  370,  355,  314,  255,  181,   88,   0,
		211,  224,  227,  217,  192,  156,  111,   54,   0,
		139,  148,  150,  144,  127,  103,   73,   36,    0,
		 90,   96,   97,   93,   82,   66,   47,   23,    0,
		 53,   56,   57,   54,   48,   39,   27,   13,    0,
		 23,   24,   25,   24,   21,   17,   12,    6,    0,
		  0,    0,    0,    0,    0,    0,    0,    0,    0,
		//47 IOP PC LUT Off - B
		992, 1023, 1012,  930,  787,  603,  378,  112,
		630,  650,  643,  591,  500,  383,  240,   71,
		358,  370,  366,  336,  284,  218,  136,   40,
		220,  227,  224,  206,  174,  133,   83,   24,
		145,  150,  148,  136,  115,   88,   55,   16,
		 94,   97,   96,   88,   74,   57,   35,   10,
		 55,   57,   56,   51,   43,   33,   21,    6,
		 24,   25,   24,   22,   19,   14,    9,    2,
		  0,    0,    0,    0,    0,    0,    0,    0,
		//47 IOP PC LUT Off - C
		951, 1012, 1023,  982,  869,  705,  501,  245,    0,
		455,  485,  490,  470,  416,  338,  240,  117,    0,
		262,  279,  282,  270,  239,  194,  138,   67,    0,
		171,  182,  184,  176,  156,  126,   90,   44,    0,
		112,  119,  121,  116,  102,   83,   59,   29,    0,
		 69,   74,   75,   72,   63,   51,   36,   18,    0,
		 37,   39,   40,   38,   34,   27,   19,    9,    0,
		 11,   11,   12,   11,   10,    8,    5,    2,    0,

		//47 IOP PC LUT Off - D
		992, 1023, 1012,  930,  787,  603,  378,  112,
		475,  490,  485,  445,  377,  289,  181,   53,
		273,  282,  279,  256,  217,  166,  104,   31,
		178,  184,  182,  167,  141,  108,   68,   20,
		117,  121,  119,  110,   93,   71,   44,   13,
		 72,   75,   74,   68,   57,   44,   27,    8,
		 38,   40,   39,   36,   30,   23,   14,    4,
		 11,   12,   11,   10,    9,    7,    4,    1,
	},

	{
		//47 IOP PC LUT Off - A
		2000, 2002, 2003,  982,  869,  705,  501,  245,   0,
		604,  643,  650,  624,	552,  448,  318,  156,   0,
		344,  366,  370,  355,  314,  255,  181,   88,   0,
		211,  224,  227,  217,  192,  156,  111,   54,   0,
		139,  148,  150,  144,  127,  103,   73,   36,    0,
		 90,   96,   97,   93,   82,   66,   47,   23,    0,
		 53,   56,   57,   54,   48,   39,   27,   13,    0,
		 23,   24,   25,   24,   21,   17,   12,    6,    0,
		  0,    0,    0,    0,    0,    0,    0,    0,    0,
		//47 IOP PC LUT Off - B
		992, 1023, 1012,  930,  787,  603,  378,  112,
		630,  650,  643,  591,  500,  383,  240,   71,
		358,  370,  366,  336,  284,  218,  136,   40,
		220,  227,  224,  206,  174,  133,   83,   24,
		145,  150,  148,  136,  115,   88,   55,   16,
		 94,   97,   96,   88,   74,   57,   35,   10,
		 55,   57,   56,   51,   43,   33,   21,    6,
		 24,   25,   24,   22,   19,   14,    9,    2,
		  0,    0,    0,    0,    0,    0,    0,    0,
		//47 IOP PC LUT Off - C
		951, 1012, 1023,  982,  869,  705,  501,  245,    0,
		455,  485,  490,  470,  416,  338,  240,  117,    0,
		262,  279,  282,  270,  239,  194,  138,   67,    0,
		171,  182,  184,  176,  156,  126,   90,   44,    0,
		112,  119,  121,  116,  102,   83,   59,   29,    0,
		 69,   74,   75,   72,   63,   51,   36,   18,    0,
		 37,   39,   40,   38,   34,   27,   19,    9,    0,
		 11,   11,   12,   11,   10,    8,    5,    2,    0,

		//47 IOP PC LUT Off - D
		992, 1023, 1012,  930,  787,  603,  378,  112,
		475,  490,  485,  445,  377,  289,  181,   53,
		273,  282,  279,  256,  217,  166,  104,   31,
		178,  184,  182,  167,  141,  108,   68,   20,
		117,  121,  119,  110,   93,   71,   44,   13,
		 72,   75,   74,   68,   57,   44,   27,    8,
		 38,   40,   39,   36,   30,   23,   14,    4,
		 11,   12,   11,   10,    9,    7,    4,    1,
	}
};


#endif/* end of _PANELLCD_EEPROM_47IOP_TM240_H_ */
