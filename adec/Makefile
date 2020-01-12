
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
#	AUTHOR		:	jongsang.oh (jongsang.oh@lge.com)
#	DATE        :	2009.12.30
#	DESCRIPTION	:	Makefile for building adec module                      #
#******************************************************************************#
include $(KDRV_TOP_DIR)/modules.mk

adec_MODULE			:= adec

#-------------------------------------------------------------------------------
# TODO: define your driver source file
#-------------------------------------------------------------------------------
kdrv_adec-objs		:= adec_cfg.o
kdrv_adec-objs		+= adec_drv.o
kdrv_adec-objs		+= adec_proc.o
# TODO(daeseok.youn) : adec_drv_damp file need to compile
# if gtv execute without dtv
ifeq ($(WITHOUT_DTV), YES)
kdrv_adec-objs		+= adec_drv_damp.o
endif

ifeq ($(INCLUDE_L8_CHIP_KDRV), YES)
kdrv_adec-objs		+= l8/adec_coredrv_l8.o
kdrv_adec-objs		+= l8/adec_isr_l8.o
kdrv_adec-objs		+= l8/mixedIP_coredrv_l8.o

else
kdrv_adec-objs		+= l9/adec_coredrv_l9.o
kdrv_adec-objs		+= l9/adec_isr_l9.o
kdrv_adec-objs		+= l9/adec_ipc_func.o
kdrv_adec-objs		+= l9/mixedIP_coredrv_l9_a0.o
kdrv_adec-objs		+= l9/mixedIP_coredrv_l9_b0.o

kdrv_adec-objs		+= l9/adec_ipc/adec_ipc_cmd.o
kdrv_adec-objs		+= l9/adec_ipc/adec_ipc_mem.o
kdrv_adec-objs		+= l9/adec_ipc/adec_ipc_req.o
kdrv_adec-objs		+= l9/adec_ipc/adec_ipc_dbg.o
kdrv_adec-objs		+= l9/adec_ipc/adec_ipc_reg_api.o

kdrv_adec-objs		+= l9/buffer/adec_buf_dbg.o
kdrv_adec-objs		+= l9/buffer/adec_common_buf.o
kdrv_adec-objs		+= l9/buffer/adec_inter_buf_arm.o
kdrv_adec-objs		+= l9/buffer/adec_inter_buf_reader.o
kdrv_adec-objs		+= l9/buffer/adec_inter_buf_writer.o
kdrv_adec-objs		+= l9/buffer/adec_buf_helper.o

kdrv_adec-objs		+= l9/adec_common/adec_mutex.o
kdrv_adec-objs		+= l9/adec_common/adec_processor_adap.o
kdrv_adec-objs		+= l9/adec_common/adec_reg_api.o

#kdrv_adec-objs		+= l9/aac/adec_aac_adts.o
#kdrv_adec-objs		+= l9/aac/adec_aac_asc.o
#kdrv_adec-objs		+= l9/aac/adec_aac_latm.o
#kdrv_adec-objs		+= l9/aac/adec_aac_loas.o
#kdrv_adec-objs		+= l9/aac/adec_aac_probe.o
endif

kdrv_adec-objs		+=

#-------------------------------------------------------------------------------
# TODO: define your driver specific CFLAGS
#-------------------------------------------------------------------------------
ifeq ($(WITHOUT_DTV), YES)
kdrv_adec-CFLAGS	+= -DENABLE_DAMP_VOLUME_CONTROL
endif

kdrv_adec-CFLAGS	+=
kdrv_adec-CFLAGS	+=

#-------------------------------------------------------------------------------
# DO NOT change the below part
#-------------------------------------------------------------------------------
EXTRA_CFLAGS		+= $(kdrv_adec-CFLAGS)

ifeq ($(KDRV_GLOBAL_LINK), YES)
# KDRV_MODULE_OBJS is used for KDRV_GLOBAL_LINK mode
KDRV_MODULE_OBJS	+= $(addprefix $(adec_MODULE)/,$(kdrv_adec-objs))
else
obj-m				:= kdrv_adec.o
endif
