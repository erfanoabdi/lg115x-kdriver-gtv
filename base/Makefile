
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
#	AUTHOR		:				                                               #
#	DATE        :	                                                           #
#	DESCRIPTION	:	Makefile for building base module                          #
#******************************************************************************#
include $(KDRV_TOP_DIR)/modules.mk

base_MODULE			:= base

#-------------------------------------------------------------------------------
# TODO: define your driver source file
#-------------------------------------------------------------------------------
kdrv_base-objs		:= base_mem_cfg.o
kdrv_base-objs		+= base_dev_cfg.o
kdrv_base-objs		+= base_drv.o
kdrv_base-objs		+= base_proc.o
kdrv_base-objs		+= proc_util.o
kdrv_base-objs		+= l9/base_addr_sw_l9.o
kdrv_base-objs		+= $(call cond_assign, y, DEBUG, debug_util.o, )

# support for QEMU emulation
ifeq ($(USE_QEMU_SYSTEM), YES)
kdrv_base-objs		+= ../z.ext/qemu/qemu_drv.o
endif

kdrv_base-objs		+= lib/tlsf-2.4.6/src/tlsf.o

kdrv_base-objs		+= os_util.o
kdrv_base-objs		+= misc_util.o
kdrv_base-objs		+= noti_util.o

#-------------------------------------------------------------------------------
# TODO: define your driver specific CFLAGS
#-------------------------------------------------------------------------------
kdrv_base-CFLAGS	+= -I$(KDRV_TOP_DIR)/$(base_MODULE)/lib/tlsf-2.4.6/src
kdrv_base-CFLAGS	+= -DTLSF_USE_LOCKS=0
kdrv_base-CFLAGS	+= -DTLSF_USE_MMAP=0
kdrv_base-CFLAGS	+= -DTLSF_USE_SBRK=0
kdrv_base-CFLAGS	+= -DTLSF_STATISTIC=1

#-------------------------------------------------------------------------------
# DO NOT change the below part
#-------------------------------------------------------------------------------
EXTRA_CFLAGS		+= $(kdrv_base-CFLAGS)

ifeq ($(KDRV_GLOBAL_LINK), YES)
KDRV_MODULE_OBJS	+= $(addprefix $(base_MODULE)/,$(kdrv_base-objs))
else
obj-m				:= kdrv_base.o
endif
