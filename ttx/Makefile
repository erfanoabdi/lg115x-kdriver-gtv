
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
#	DESCRIPTION	:	Makefile for building ttx module                      #
#******************************************************************************#
include $(KDRV_TOP_DIR)/modules.mk

ttx_MODULE			:= ttx

#-------------------------------------------------------------------------------
# TODO: define your driver source file
#-------------------------------------------------------------------------------
kdrv_ttx-objs		:= ttx_cfg.o
kdrv_ttx-objs		+= ttx_drv.o
kdrv_ttx-objs		+= ttx_proc.o
kdrv_ttx-objs		+= ttx_data.o
kdrv_ttx-objs		+= ttx_module.o
kdrv_ttx-objs		+= hwl_l8_b0/ttx_hw_l8b0.o
kdrv_ttx-objs		+= hwl_l8_a1/ttx_hw_l8a1.o
kdrv_ttx-objs		+= hwl_l9_a0/ttx_hw_l9a0.o
kdrv_ttx-objs		+= hwl_l9_b0/ttx_hw_l9b0.o
kdrv_ttx-objs		+= hwl_l8_b0/cc_hw_l8b0.o
kdrv_ttx-objs		+= hwl_l8_a1/cc_hw_l8a1.o
kdrv_ttx-objs		+= hwl_l9_a0/cc_hw_l9a0.o
kdrv_ttx-objs		+= hwl_l9_b0/cc_hw_l9b0.o
kdrv_ttx-objs		+=

#-------------------------------------------------------------------------------
# TODO: define your driver specific CFLAGS
#-------------------------------------------------------------------------------
kdrv_ttx-CFLAGS	+=
kdrv_ttx-CFLAGS	+=
kdrv_ttx-CFLAGS	+=

#-------------------------------------------------------------------------------
# DO NOT change the below part
#-------------------------------------------------------------------------------
EXTRA_CFLAGS		+= $(kdrv_ttx-CFLAGS)

ifeq ($(KDRV_GLOBAL_LINK), YES)
# KDRV_MODULE_OBJS is used for KDRV_GLOBAL_LINK mode
KDRV_MODULE_OBJS	+= $(addprefix $(ttx_MODULE)/,$(kdrv_ttx-objs))
else
obj-m				:= kdrv_ttx.o
endif
