
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
#	FILE NAME	:	makefile                                                   #
#	VERSION		:	1.0                                                        #
#	AUTHOR		:	wonsik.do (wonsik.do@lge.com)
#	DATE        :	2010.01.06
#	DESCRIPTION	:	Makefile for building denc module                      #
#******************************************************************************#
include $(KDRV_TOP_DIR)/modules.mk

denc_MODULE			:= denc

#-------------------------------------------------------------------------------
# TODO: define your driver source file
#-------------------------------------------------------------------------------
kdrv_denc-objs		:= denc_cfg.o
kdrv_denc-objs		+= denc_drv.o
kdrv_denc-objs		+= denc_proc.o
kdrv_denc-objs		+=
kdrv_denc-objs		+= denc_io.o
kdrv_denc-objs		+= denc_hal.o
kdrv_denc-objs		+= mcu/io_denc.o
ifeq ($(USE_VIDEO_UART2_FOR_MCU), YES)
ifneq ($(KDRV_GLOBAL_LINK), YES)
kdrv_denc-objs		:= denc_hal.o
endif
endif
kdrv_denc-objs		+= l9/denc_reg_l9.o
kdrv_denc-objs		+= l8/denc_reg_l8.o

#-------------------------------------------------------------------------------
# TODO: define your driver specific CFLAGS
#-------------------------------------------------------------------------------
kdrv_denc-CFLAGS	+=
kdrv_denc-CFLAGS	+=
kdrv_denc-CFLAGS	+=

#-------------------------------------------------------------------------------
# DO NOT change the below part
#-------------------------------------------------------------------------------
EXTRA_CFLAGS		+= $(kdrv_denc-CFLAGS)

ifeq ($(KDRV_GLOBAL_LINK), YES)
# KDRV_MODULE_OBJS is used for KDRV_GLOBAL_LINK mode
KDRV_MODULE_OBJS	+= $(addprefix $(denc_MODULE)/,$(kdrv_denc-objs))
else
obj-m				:= kdrv_denc.o
endif
