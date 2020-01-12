
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
#	AUTHOR		:	sj.youm (sj.youm@lge.com)
#	DATE		:	2011.06.22
#	DESCRIPTION	:	Makefile for building pe module                      #
#******************************************************************************#
include $(KDRV_TOP_DIR)/modules.mk

pe_MODULE			:= pe

#-------------------------------------------------------------------------------
# TODO: define your driver source file
#-------------------------------------------------------------------------------
kdrv_pe-objs		:= pe_drv.o
kdrv_pe-objs		+= pe_io.o
kdrv_pe-objs		+= ccm/pe_ccm.o
kdrv_pe-objs		+= cmg/pe_cmg.o
kdrv_pe-objs		+= cmn/pe_cmn.o
kdrv_pe-objs		+= csc/pe_csc.o
kdrv_pe-objs		+= dcm/pe_dcm.o
kdrv_pe-objs		+= dnt/pe_dnt.o
kdrv_pe-objs		+= nrd/pe_nrd.o
kdrv_pe-objs		+= shp/pe_shp.o
kdrv_pe-objs		+= win/pe_win.o
kdrv_pe-objs		+= util/pe_spd.o
kdrv_pe-objs		+= util/pe_etc.o
kdrv_pe-objs		+= util/pe_d3d.o
kdrv_pe-objs		+= reg/pe_reg.o

#-------------------------------------------------------------------------------
# TODO: define your driver specific CFLAGS
#-------------------------------------------------------------------------------
kdrv_pe-CFLAGS   := -I$(KDRV_TOP_DIR)/de
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/de/l8
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/de/l9
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/util
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/reg
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/cmn
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/ccm
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/dcm
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/win
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/csc
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/cmg
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/nrd
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/dnt
kdrv_pe-CFLAGS   += -I$(KDRV_TOP_DIR)/pe/shp

#-------------------------------------------------------------------------------
# DO NOT change the below part
#-------------------------------------------------------------------------------
EXTRA_CFLAGS		+= $(kdrv_pe-CFLAGS)

ifeq ($(KDRV_GLOBAL_LINK), YES)
# KDRV_MODULE_OBJS is used for KDRV_GLOBAL_LINK mode
KDRV_MODULE_OBJS	+= $(addprefix $(pe_MODULE)/,$(kdrv_pe-objs))
else
obj-m				:= kdrv_pe.o
endif
