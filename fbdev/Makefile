
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
#	AUTHOR		:	raxis
#	DATE        :	2010.02.01
#	DESCRIPTION	:	Makefile for building fbdev module                      #
#******************************************************************************#
include $(KDRV_TOP_DIR)/modules.mk

fbdev_MODULE			:= fbdev

#-------------------------------------------------------------------------------
# TODO: define your driver source file
#-------------------------------------------------------------------------------
kdrv_fbdev-objs		:= fbdev_cfg.o
kdrv_fbdev-objs		+= fbdev_drv.o
kdrv_fbdev-objs		+= fbdev_proc.o
kdrv_fbdev-objs     += fbdev_module.o
kdrv_fbdev-objs     += fbdev_gfx_util.o
ifeq ($(INCLUDE_L8_CHIP_DRV), YES)
kdrv_fbdev-objs     += l8/fbdev_drv_l8.o
kdrv_fbdev-objs     += l8/fbdev_hw_l8.o
else
kdrv_fbdev-objs     += l8/fbdev_drv_l8_dummy.o
endif
kdrv_fbdev-objs     += l9/fbdev_drv_l9.o
kdrv_fbdev-objs		+= l9/fbdev_hw_l9.o
kdrv_fbdev-objs		+= l9/fbdev_hw_l9a0.o
kdrv_fbdev-objs		+= l9/fbdev_hw_l9b0.o
kdrv_fbdev-objs		+= l9/fbdev_hw_l9b1.o
kdrv_fbdev-objs		+= l9/fbdev_util_l9.o

kdrv_fbdev-objs		+= fbdev_partial.o

#-------------------------------------------------------------------------------
# TODO: define your driver specific CFLAGS
#-------------------------------------------------------------------------------
ifeq ($(WITHOUT_DTV), YES)
kdrv_fbdev-CFLAGS	+= -DDE_ENABLE
endif
kdrv_fbdev-CFLAGS	+=
kdrv_fbdev-CFLAGS	+=

#-------------------------------------------------------------------------------
# DO NOT change the below part
#-------------------------------------------------------------------------------
EXTRA_CFLAGS		+= $(kdrv_fbdev-CFLAGS)

ifeq ($(KDRV_GLOBAL_LINK), YES)
# KDRV_MODULE_OBJS is used for KDRV_GLOBAL_LINK mode
KDRV_MODULE_OBJS	+= $(addprefix $(fbdev_MODULE)/,$(kdrv_fbdev-objs)) 
else
obj-m				:= kdrv_fbdev.o
endif
