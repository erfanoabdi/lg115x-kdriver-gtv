
# SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
# Copyright(c) 2010 by LG Electronics Inc.

# This program is free software; you can redistribute it and/or 
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
# GNU General Public License for more details.
 

#   ---------------------------------------------------------------------------#
#                                                                              #
#	FILE NAME	:	modules.mk                                                 #
#	VERSION		:	1.0                                                        #
#	AUTHOR		:	raxis@lge.com                                              #
#	DATE        :	2009/11/10                                                 #
#	DESCRIPTION	:	Makefile for building LG1150 kernel driver                 #
#******************************************************************************#

# MODULE_MK_INCLUDED variable prevents the recursive inclusion of module.mk
# if module.mk is included MODULE_MK_INCLUDED is set to YES.
# so if another inclusion of module.mk is safely ignored.
#
ifeq ($(MODULE_MK_INCLUDED),)

include $(KDRV_TOP_DIR)/incs.mk

#------------------------------------------------------------------------------
# define module directory
#
# TODO: add your module directory
#------------------------------------------------------------------------------
BASE_MODULE		:=
BASE_MODULE		+= base
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_SAMPLE,sample,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_AAD, 	aad,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_ADEC, 	adec,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_AFE, 	afe,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_CI, 	ci,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_DE, 	de,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_DEMOD, demod,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_DENC, 	denc,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_DIMM, 	dimm,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_FBDEV, fbdev,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_FMT3D, fmt3d,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_FRC, 	frc,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_GFX, 	gfx,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_GPU, 	gpu,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_GPIO, 	gpio,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_HDMI, 	hdmi,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_I2C, 	i2c,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_PE, 	pe,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_PVR, 	pvr,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_SDEC, 	sdec,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_TTX, 	ttx,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_VDEC, 	${UNIVDEC},)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_VENC, 	venc,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_PNG, 	png,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_SE, 	se,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_SYS, 	sys,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_SCI, 	sci,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_CLK, 	clk,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_MONITOR,monitor,)
BASE_MODULE		+= $(call cond_assign, YES, INCLUDE_KDRV_MJPEG, mjpeg,)
BASE_MODULE		+=
BASE_MODULE		+=

#------------------------------------------------------------------------------
# define extra module directory
#
# TODO: add your module directory
#------------------------------------------------------------------------------
EXT_MODULE      += $(call cond_assign, YES, KDRV_NANDSIM,  z.ext/nandsim,)
EXT_MODULE		+=
EXT_MODULE		+=


#------------------------------------------------------------------------------
# Define default CFLAGS for kernel driver
#
# TODO : add common CFLAGS
#------------------------------------------------------------------------------
EXTRA_CFLAGS	+= -D__ARM__ -DUSE_ADDRESS_SWITCH_CASE1 # NOTE : cosmo use system memory 768M
EXTRA_CFLAGS	+= -mfpu=vfp -mfloat-abi=softfp		# arm VFPv3 - Do not change this flags for COSMO
#EXTRA_CFLAGS	+= -mfpu=neon -mfloat-abi=softfp	# arm VFPv3

# 20100709 seokjoo.lee for OS_DEBUG_Print format check.
EXTRA_CFLAGS	+= -Wformat

EXTRA_CFLAGS	+= -I$(KDRV_TOP_DIR)/include
EXTRA_CFLAGS	+= $(call cond_assign, y, DEBUG, -DKDRV_DEBUG_BUILD,)
EXTRA_CFLAGS	+= -DKDRV_TOP_DIR=\"$(KDRV_TOP_DIR)\"

EXTRA_CFLAGS	+= $(addprefix -I$(KDRV_TOP_DIR)/,$(BASE_MODULE))
EXTRA_CFLAGS	+= $(addprefix -I$(KDRV_TOP_DIR)/,$(EXT_MODULE))

EXTRA_CFLAGS	+= $(call cond_assign, YES, USE_QEMU_SYSTEM, -I$(KDRV_TOP_DIR)/z.ext/qemu,)

EXTRA_CFLAGS	+= $(call cond_define, YES, KDRV_GLOBAL_LINK)
EXTRA_CFLAGS	+= $(call cond_define, YES, USE_QEMU_SYSTEM)
EXTRA_CFLAGS	+= $(call cond_define, YES, KDRV_CONFIG_PM)
EXTRA_CFLAGS	+= $(call cond_define, YES, KDRV_MODULE_BUILD)

EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_SAMPLE)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_SYS)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_AAD)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_ADEC)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_AFE)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_CI)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_DE)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_DEMOD)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_DENC)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_DIMM)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_FMT3D)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_FRC)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_FBDEV)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_GFX)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_GPIO)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_HDMI)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_I2C)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_PE)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_PVR)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_SDEC)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_TTX)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_VDEC)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_VENC)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_PNG)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_SE)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_SCI)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_CLK)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_MONITOR)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_KDRV_MJPEG)

EXTRA_CFLAGS    += $(call cond_define, YES, INCLUDE_KDRV_DUALDPB)
EXTRA_CFLAGS    += $(call cond_define, YES, INCLUDE_GPU_MALI)

EXTRA_CFLAGS	+= $(call cond_define, YES, USE_VIDEO_UART2_FOR_MCU)
#USE_VIDEO_UART2_FOR_MCU = YES
#EXTRA_CFLAGS	+= -DUSE_VIDEO_UART2_FOR_MCU

EXTRA_CFLAGS	+= -DUSE_VIDEO_IOCTL_CALLING

EXTRA_CFLAGS	+= -DKDRV_PLATFORM=$(call cond_assign, y, CONFIG_MACH_L8_ANDROID, KDRV_COSMO_PLATFORM, KDRV_GP_PLATFORM)
EXTRA_CFLAGS	+= -DKDRV_GP_PLATFORM=0x1000 -DKDRV_COSMO_PLATFORM=0x1001
EXTRA_CFLAGS	+= $(call cond_define, YES, SIC_LAB_BOARD)

EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_L8_CHIP_KDRV)
EXTRA_CFLAGS	+= $(call cond_define, YES, INCLUDE_L9_CHIP_KDRV)

#
# Below lines are orignally defined in GP2.
#
#EXTRA_CFLAGS	+= -DCHIP_LG1150=3008
#EXTRA_CFLAGS	+= -DMAIN_CHIP_TYPE=CHIP_LG1150
# [L8] minjun.kim 2010-09-30 아래 값은 수정하지 마시요
KDRV_OUT_DIR        := $(OUT_DIR)/lib/modules
CHANGE_FILE_LIST    := $(OUT_DIR)/changed_file_list.txt
CHANGE_FILE_FLAG    := $(OUT_DIR)/changed_file_flag.txt

#
# See the comment in ifeq($(MODULE_MK_INCLUDED,) part
#
MODULE_MK_INCLUDED = YES

endif

