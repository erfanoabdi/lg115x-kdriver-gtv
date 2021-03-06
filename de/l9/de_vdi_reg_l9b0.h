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

#ifndef _REG_VDI_L9B0_h
#define _REG_VDI_L9B0_h

/*-----------------------------------------------------------------------------
	0x4d00 vdi_frame_inx ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 frame_index         : 8;   //  0: 7
} L9B0_VDI_FRAME_INX_T;

/*-----------------------------------------------------------------------------
	0x4d04 vdi_pic_info ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 display_type        : 2;   //  0: 1
} L9B0_VDI_PIC_INFO_T;

/*-----------------------------------------------------------------------------
	0x4d08 vdi_frame_rate ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 framerateresidual   : 16;  //  0:15
	UINT32 frameratedivminus1  : 16;  // 16:31
} L9B0_VDI_FRAME_RATE_T;

/*-----------------------------------------------------------------------------
	0x4d0c vdi_aspect_ratio ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 aspect_ratio        : 4;   //  0: 3
} L9B0_VDI_ASPECT_RATIO_T;

/*-----------------------------------------------------------------------------
	0x4d10 vdi_picture_size ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 height              : 16;  //  0:15
	UINT32 width               : 16;  // 16:31
} L9B0_VDI_PICTURE_SIZE_T;

/*-----------------------------------------------------------------------------
	0x4d14 vdi_h_offset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 right_offset        : 16;  //  0:15
	UINT32 left_offset         : 16;  // 16:31
} L9B0_VDI_H_OFFSET_T;

/*-----------------------------------------------------------------------------
	0x4d18 vdi_v_offset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 bottom_offset       : 16;  //  0:15
	UINT32 top_offset          : 16;  // 16:31
} L9B0_VDI_V_OFFSET_T;

/*-----------------------------------------------------------------------------
	0x4d1c vdi_frameupdated ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 updated             : 1;   //     0
} L9B0_VDI_FRAMEUPDATED_T;

/*-----------------------------------------------------------------------------
	0x4d20 vdi_y_frame_base_address ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 start_addr          : 12;  //  0:11
	UINT32 base_addr           : 20;  // 12:31
} L9B0_VDI_Y_FRAME_BASE_ADDRESS_T;

/*-----------------------------------------------------------------------------
	0x4d24 vdi_c_frame_base_address ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 start_addr          : 12;  //  0:11
	UINT32 base_addr           : 20;  // 12:31
} L9B0_VDI_C_FRAME_BASE_ADDRESS_T;

/*-----------------------------------------------------------------------------
	0x4d28 vdi_y_frame_offset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 frame_offset        : 32;  //  0:31
} L9B0_VDI_Y_FRAME_OFFSET_T;

/*-----------------------------------------------------------------------------
	0x4d2c vdi_c_frame_offset ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 frame_offset        : 32;  //  0:31
} L9B0_VDI_C_FRAME_OFFSET_T;

/*-----------------------------------------------------------------------------
	0x4d30 vdi_stride ''
------------------------------------------------------------------------------*/
typedef struct {
	UINT32 stride              : 16;  //  0:15
} L9B0_VDI_STRIDE_T;

/*-----------------------------------------------------------------------------
	0x4d34 pixel_aspect_ratio ''
------------------------------------------------------------------------------*/
typedef struct { // 0x4d34
	UINT32 par_height          : 16;  // 15: 0
	UINT32 par_width           : 16;  // 31:16
} PIXEL_ASPECT_RATIO_T;

/*-----------------------------------------------------------------------------
	0x4d38 vdi_framepackarrange ''
------------------------------------------------------------------------------*/
typedef struct { // 0x4d38
	UINT32 vdi_3d_sei          : 8;  // 7: 0
	UINT32 vdi_lr_order        : 2;  // 9: 8
} VDI_FRAMEPACKARRANGE_T;

/*-----------------------------------------------------------------------------
	0x4d3c vdi_ptsinfo ''
------------------------------------------------------------------------------*/
typedef struct { // 0x4d3c
	UINT32 pts_info        : 32;  // 31: 0
} VDI_PTSINFO_T;

typedef struct {
	L9B0_VDI_FRAME_INX_T                vdi_frame_inx;                       // 0x4d00
	L9B0_VDI_PIC_INFO_T                 vdi_pic_info;                        // 0x4d04
	L9B0_VDI_FRAME_RATE_T               vdi_frame_rate;                      // 0x4d08
	L9B0_VDI_ASPECT_RATIO_T             vdi_aspect_ratio;                    // 0x4d0c
	L9B0_VDI_PICTURE_SIZE_T             vdi_picture_size;                    // 0x4d10
	L9B0_VDI_H_OFFSET_T                 vdi_h_offset;                        // 0x4d14
	L9B0_VDI_V_OFFSET_T                 vdi_v_offset;                        // 0x4d18
	L9B0_VDI_FRAMEUPDATED_T             vdi_frameupdated;                    // 0x4d1c
	L9B0_VDI_Y_FRAME_BASE_ADDRESS_T     vdi_y_frame_base_address;            // 0x4d20
	L9B0_VDI_C_FRAME_BASE_ADDRESS_T     vdi_c_frame_base_address;            // 0x4d24
	L9B0_VDI_Y_FRAME_OFFSET_T           vdi_y_frame_offset;                  // 0x4d28
	L9B0_VDI_C_FRAME_OFFSET_T           vdi_c_frame_offset;                  // 0x4d2c
	L9B0_VDI_STRIDE_T                   vdi_stride;                          // 0x4d30
	PIXEL_ASPECT_RATIO_T            	pixel_aspect_ratio;              	 // 0x4d34
	VDI_FRAMEPACKARRANGE_T          	vdi_framepackarrange;            	 // 0x4d38
	VDI_PTSINFO_T          				vdi_ptsinfo;            		 	 // 0x4d3c
} DE_VDI_REG_L9B0_T;

#endif
