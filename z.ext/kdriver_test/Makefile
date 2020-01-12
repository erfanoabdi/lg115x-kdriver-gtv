
# SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
# Copyright(c) 2010 by LG Electronics Inc.

# This program is free software; you can redistribute it and/or 
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
# GNU General Public License for more details.
 

#	FILE NAME	:	Makefile                                                   #
#	VERSION		:	1.0                                                        #
#	AUTHOR		:	dae-young lim (raxis@lge.com)                              #
#	DATE        :	2009/11/25                                                 #
#	DESCRIPTION	:	This file is to make indivisual sub modules.               #
#******************************************************************************#

#==============================================================================
# 	How to add your test files
#
#	1. define xxx_SRCS and xxx_OBJS
#	2. add xxx_OBJS to OBJS list
#	3. add xxx_test to EXE_LIST list
#	4. define xxx_test command
#
#==============================================================================

include $(KDRV_TOP_DIR)/modules.mk

#==============================================================================
# default object directory
#==============================================================================
OBJ_DIR			:= objs

#==============================================================================
# CFLAGS
#==============================================================================
CFLAGS			+= $(EXTRA_CFLAGS)
CFLAGS			+=
CFLAGS			+= -I$(shell pwd)

#==============================================================================
# OBJS defnitions
#==============================================================================
common_SRCS = simple_uart_lib.c simple_linux_lib.c simple_misc_lib.c

# sample OBJS
sample_SRCS	= sample_test.c $(common_SRCS)
sample_OBJS	= $(foreach src, $(sample_SRCS), $(OBJ_DIR)/$(src:.c=.o))

# debug_shell_test OBJS
debug_shell_test_SRCS	= debug_shell_test.c $(common_SRCS)
debug_shell_test_OBJS	= $(foreach src, $(debug_shell_test_SRCS), $(OBJ_DIR)/$(src:.c=.o))

# top_OBJS
#CFLAGS		+= -DEXCEPT_XDP
top_SRCS	 = cli/cli_main.c
top_SRCS	+= cli/cli_xdp.c
top_SRCS	+= cli/cli_core.c
top_SRCS	+= cli/cli_list.c
top_SRCS	+= cli/cli_mem.c
top_SRCS	+= cli/cli_help.c
top_SRCS	+= cli/cli_arg.c
top_SRCS	+= dbg/dbg_reg.c
top_SRCS	+= dbg/dbg_mem.c
#top_SRCS	+= cli/cli_memhandler.c
#top_SRCS	+= dbg/dbg_reg_main.c

ifeq ($(INCLUDE_KDRV_DE), YES)
top_SRCS	+= de/de_xdp.c
top_SRCS	+= de/de_dev.c
top_SRCS	+= de/de_cli.c
top_SRCS	+= de/de_test.c
endif
ifeq ($(INCLUDE_KDRV_DIMM), YES)
top_SRCS	+= dimm/dimm_dev.c
top_SRCS	+= dimm/dimm_cli.c
endif
ifeq ($(INCLUDE_KDRV_TCON), YES)
top_SRCS	+= tcon/tcon_dev.c
top_SRCS	+= tcon/tcon_cli.c
endif
ifeq ($(INCLUDE_KDRV_FMT3D), YES)
top_SRCS	+= fmt3d/fmt3d_dev.c
top_SRCS	+= fmt3d/fmt3d_cli.c
endif
ifeq ($(INCLUDE_KDRV_DENC), YES)
top_SRCS	+= denc/denc_dev.c
top_SRCS	+= denc/denc_cli.c
endif
ifeq ($(INCLUDE_KDRV_GPIO), YES)
top_SRCS	+= gpio/gpio_cli.c
endif
ifeq ($(INCLUDE_KDRV_FRC), YES)
top_SRCS	+= frc/frc_cli.c
endif
ifeq ($(INCLUDE_KDRV_I2C), YES)
top_SRCS	+= i2c/i2c_cli.c
endif
ifeq ($(INCLUDE_KDRV_VENC), YES)
top_SRCS	+= venc/venc_cli.c
endif
ifeq ($(INCLUDE_KDRV_PQL), YES)
top_SRCS	+= pql/pql_dev.c
top_SRCS	+= pql/pql_cli.c
endif

# sdec OBJS
ifeq ($(INCLUDE_KDRV_SDEC), YES)
top_SRCS	+= sdec/sdec_cli.c
endif

# vdec OBJS
ifeq ($(INCLUDE_KDRV_VDEC), YES)
top_SRCS	+= vdec/vdec_cli.c
endif

ifeq ($(INCLUDE_KDRV_CI), YES)
top_SRCS	+= ci/ci_cli.c
endif

ifeq ($(INCLUDE_KDRV_SE), YES)
top_SRCS	+= se/sehelper_cli.c
top_SRCS	+= se/se_cli.c
endif


top_SRCS	+= top/test_main.c
top_OBJS	= $(foreach src, $(top_SRCS), $(OBJ_DIR)/$(src:.c=.o))


# yyy OBJS

OBJS			=
OBJS			+= sample_OBJS
OBJS			+=
OBJS			+=

EXE_LIST		=
EXE_LIST		+= sample_test
EXE_LIST		+= debug_shell_test
EXE_LIST		+= top_test

#==============================================================================
# Build rules for this Makefile
#==============================================================================
all : $(EXE_LIST)

copy:
	# [L8] copy kdriver_test executables to $(KDRV_OUT_DIR)
	$(CP) -f $(EXE_LIST) $(KDRV_OUT_DIR)

prodgsync:
	# [L8] sync kdriver_test executables to $(KDRV_OUT_DIR)
	@$(CD) . && find $(EXE_LIST) -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)

clean :
	@$(RM) -rf $(OBJ_DIR) $(EXE_LIST)

# Build rules fro sample_test
sample_test: $(sample_OBJS)
	@$(CCDV) $(CC) $(CFLAGS:-c=) -o $@ $(sample_OBJS) -lpthread

# Build script for xxxx
debug_shell_test: $(debug_shell_test_OBJS)
	@$(CCDV) $(CC) $(CFLAGS:-c=) -o $@ $(debug_shell_test_OBJS) -lpthread

# Build rules fro top_test
top_test: $(top_OBJS)
	@$(CCDV) $(CC) $(CFLAGS:-c=) -o $@ $(top_OBJS) -lpthread
	$(CP) -f $@ $(OUT_DIR)

# Build script for xxxx

# Build script for xxxx

# Build script for xxxx

#==============================================================================
# Basic simple rules to compile
#==============================================================================
MKDEP_AWK=$(GP3_ROOT_DIR)/os/lg1150_linux/tools/mkdep.awk
CKDEP_AWK=$(GP3_ROOT_DIR)/os/lg150_linux/tools/ckdep.awk
CCDV=$(GP3_ROOT_DIR)/os/lg1150_linux/tools/ccdv
CCDV_OPTS=  -v0 -p -x 0

$(OBJ_DIR)/%.o %.o: %.c
	@test -d $(LIB_DIR) || mkdir -p $(LIB_DIR)
	@test -d $(dir  $@) || mkdir -p $(dir  $@)
	@$(CCDV) $(CC) -MMD -MF $@.P $(CFLAGS) -o $@ -c $<
	@$(ECHO) "$@ $(@:.o=.d) : $< \\"  > $(@:.o=.d)
	@$(AWK) -f $(MKDEP_AWK) $(AWK_PATH_VAR) $@.P >> $(@:.o=.d) ||                           \
	(                                                                                     	\
		$(ECHO) Inclusion error from file $<;                                               \
		$(RM) $@.P;                                                                         \
		$(CC) $(CFLAGS) -E $< | $(AWK) -f $(CKDEP_AWK) $(AWK_PATH_VAR) > $(@:.o=.i);        \
		$(CCDV) $(CC) -o $@ -c $(@:.o=.i);                                                  \
	);
	@$(RM) $@.P
	@if [ "$(UDC)" ]; then                                                                  \
		$(ECHO) "Analyzing... $<";                                                          \
		$(UDC) $(CROSS)gcc $(CFLAGS) -c $<;                                                 \
	fi
