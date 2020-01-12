/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/** @file
 *
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author     raxis.lim
 *  @version    1.0
 *  @date       2010-12-09
 *  @note       Additional information.
 */

#ifndef	_FBDEV_HW_H_
#define	_FBDEV_HW_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
/** write OSD header to memory */
#define FBDEV_REG_MEMCPY(dst,src,cnt)                           \
{                                                               \
    register int i;                                             \
    volatile UINT32* dst_ptr = (volatile UINT32*)dst;           \
             UINT32* src_ptr = (UINT32*)src;                    \
    for ( i=cnt; i>0; i-- )  *dst_ptr++ = *src_ptr++;           \
}

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
* pixel format enumeration
*/
typedef enum
{
    YCbCr655    = 0x00,//'0000b',
    AYCbCr2644  ,//'0001b',
    AYCbCr4633  ,//= '0010b',
    AYCbCr6433  ,//= '0011b',
    YCbCr888    ,//= '0100b',
    AYCbCr8655  ,//= '0101b',
    AYCbCr8888  ,//= '0110b',
    AYCbCr8888d ,//= '0111b',
    RGB565      ,//= '1000b',
    ARGB1555    ,//= '1001b',
    ARGB4444    ,//= '1010b',
    ARGB6343    ,//= '1011b',
    RGB888      ,//= '1100b',
    ARGB8565    ,//= '1101b',
    ARGB8888    //= '1110b',
}
OSD_PixFormat;

typedef enum E_PIXEL_DEPTH
{
	PIX_DEPTH_1_BPP = 0,    /* 2 indexed colors */
	PIX_DEPTH_2_BPP,        /* 4 indexed colors */
	PIX_DEPTH_4_BPP,        /* 16 indexed colors */
	PIX_DEPTH_8_BPP,        /* 256 indexed colors */
	PIX_DEPTH_16_BPP,       /* 65536 real/true colors */
	PIX_DEPTH_24_BPP,       /* 2^24 real/true colors */
	PIX_DEPTH_32_BPP,       /* 2^32 real/true colors */
	PIX_DEPTH_MAX,
}
EPixelDepth;

/**
	enum definition
*/
typedef enum
{
	CRSR_INTR	= 0,
	OSD_2_INTR	= 1,
	OSD_1_INTR	= 2,
	OSD_0_INTR	= 3,
	MIXER_INTR	= 4,
	ALL_INTR	= 5
} 
INTERRUPT_INFORM_T;

/**
	structure
*/
typedef struct
{
	unsigned short	header;	// (1<<0) : byte , (1<<1) : quarter word , (1<<2) : half word
	unsigned short	palette; // (1<<0) : byte , (1<<1) : quarter word , (1<<2) : half word
	unsigned short	image; // (1<<0) : byte , (1<<1) : quarter word , (1<<2) : half word
} 
ENDIAN_INFORM_T;

typedef struct{
	unsigned short 	data;
	bool 			rw_n;
	bool 			bypass;
	bool 			sel;
	unsigned short	out_ctrl;
	unsigned short	in_ctrl;
} 
CSC_INFORM_T;

typedef volatile struct
{
	union
	{
		struct
		{
			UINT32						// OSD[0:3]_HDR0								
			osd_hdr_ypos		   :12,	//  0:11
                               	   : 4,	// 12:15 reserved
			osd_hdr_xpos           :11,	// 16:26
                           		   : 4,	// 27:30 reserved
			osd_hdr_color_key_en   : 1;	//    31

			UINT32					   // OSD[0:3]_HDR1
			osd_hdr_h_mem         :12, //  0:11
           			              : 4, // 12:15 reserved
			osd_hdr_w_mem         :12; // 16:27

			UINT32					   // OSD[0:3]_HDR2
			osd_hdr_h_out         :12, //  0:11
		                          : 4, // 12:15 reserved
			osd_hdr_w_out         :12, // 16:27
                                  : 3, // 28:30 reserved
			osd_hdr_pixel_order   : 1; //    31
		}
		osd;

		struct
		{
			UINT32
			osd_hdr_ypos           :12,    //  0:11
                                   : 4,    // 12:15 reserved
			osd_hdr_xpos           :11,    // 16:26
                                   : 4,    // 27:30 reserved
			osd_hdr_pos_load       : 1;    //    31

			UINT32
			osd_hdr_h_mem          :12,    //  0:11
                                   : 3,    // 12:14 reserved
			osd_hdr_pixel_order    : 1,    //    15
			osd_hdr_w_mem          :12,    // 16:27
                                   : 3,    // 28:30 reserved
			osd_hdr_color_key_en   : 1;    //    31

			UINT32
			_rsvd0                 ;
		}
		crsr;
	}
	type;

    UINT32								// OSD[0:3]_HDR3
    osd_hdr_wpl                    :16, //  0:15
    osd_hdr_global_alpha           : 8, // 16:23
    osd_hdr_format                 : 4, // 24:27
    osd_hdr_depth                  : 3, // 28:30
    osd_hdr_global_alpha_en        : 1; //    31

    UINT32								// OSD[0:3]_HDR4
    osd_hdr_color_key              ;    // 31: 0

	UINT32
	_rsvd						   ;

    UINT32								// OSD[0:3]_HDR5
    osd_hdr_ptr_plte               ;    // 31: 0

    UINT32								// OSD[0:3]_HDR6
    osd_hdr_ptr_bmp                ;	// 31: 0
}
FBDEV_OSD_HDR_T;

/** FRC connection ctrl
 *
 *	@note only valid for L9
 */
typedef struct
{
    BOOLEAN					b_frc_sep_enable;		/* OSD separated mode : TRUE when enabled, FALSE when disabled or STD LVDS/UD mode */
	BOOLEAN					b_frc_UD_enable;	
	BOOLEAN					b_frc_hdr_emul;			/* check if OSD should generate FRC3 header */
	BOOLEAN					b_frc_osd_emul;			/* ? */

	BOOLEAN					b_frc_720p_disp_enable;	/* make 1280x1080 OSD to fix FRC3 jpeg line buffer issue. valid only for L9B1+FRCA1 */

	int						frc_hdr_emul_dev;
}
FBDEV_FRC_CTRL_T;

/** 3D OSD control 
 *
 *	@note only valid for L9
 */
typedef struct
{
    LX_FBDEV_TRID_CTRL_T    fb[LX_FBDEV_ID_MAX];
}
FBDEV_TRID_CTRL_T;

extern  FBDEV_FRC_CTRL_T    g_fbdev_frc_ctrl;
extern  FBDEV_TRID_CTRL_T   g_fbdev_trid_ctrl;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/
int FBDEV_InitHardware			(void);
int FBDEV_ExitHardware			(void);
int FBDEV_SetOSDEnable			(int layer , int enable);
int FBDEV_GetOSDEnable			(int layer);

int FBDEV_InitOSDLayer			(int layer);

int FBDEV_WriteOSDHeader		(struct scanvas  *pstCanvas);

int FBDEV_GetZList				(LX_FBDEV_ZLIST_T* zList );
int FBDEV_SetZList				(LX_FBDEV_ZLIST_T* zList );
int FBDEV_SetVideoMixerCtrl		(LX_FBDEV_VIDEO_MIXER_CTRL_T* ctrl);

int FBDEV_SetTridCtrl			(int layer, LX_FBDEV_TRID_CTRL_T* ctrl);
int FBDEV_SetOutPathCtrl		(int layer, LX_FBDEV_OUT_PATH_CTRL_T* ctrl);

int FBDEV_WriteOSDEndian		(int layer , ENDIAN_INFORM_T *endian);
int FBDEV_WriteOSDCSC			(int layer , CSC_INFORM_T *csc);
int FBDEV_GetInterruptStatus	(INTERRUPT_INFORM_T intr);
int FBDEV_SetInterruptClear		(INTERRUPT_INFORM_T intr);
int FBDEV_SetInterruptEnable	(INTERRUPT_INFORM_T intr , int enable);
int FBDEV_DownloadPalette		(unsigned int addr , unsigned int size , void *data);


void FBDEV_ISRTickHandler		(UINT32 cnt);

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _PROTOS_H_ */

