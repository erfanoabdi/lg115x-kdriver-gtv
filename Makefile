
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
#   FILE NAME   :   makefile                                                   #
#   VERSION     :   1.0                                                        #
#   AUTHOR      :   raxis@lge.com                                              #
#   DATE        :   2010/11/17                                                 #
#   DESCRIPTION :   Makefile for building LG1150 kernel driver                 #
#******************************************************************************#

KERNEL_NAME         ?= linux-2.6.35-googletv-l9
PRODUCT_NAME = $(shell echo $(PROJECT_NAME) | tr "[:upper:]" "[:lower:]")
ifeq ($(KERNEL_NAME),)
$(error KERNEL_NAME is not defined, please define this variable above )
endif
ifeq ($(PLATFORM_CHIP),)
$(error PLATFORM_CHIP is not defined, please define this variable above )
endif

#==============================================================================
#
#	BUILD FLAGS
#
#==============================================================================
# if KDRV_GLOBAL_LINK is YES, the big one kernel module is created.
# if NO, all modules creates its own kernel module.
# default value is YES
KDRV_GLOBAL_LINK	?= YES

# If you want to make your own simple test application, set KDRV_TEST_APP to YES.
# your test code should be located at kdriver/ext/kdriver_test
# need to modify kdriver/z.ext/kdriver_test/Makefile.
# default value is YES
KDRV_TEST_APP		?= NO

# If you want to make nand flash simulator, set KDRV_NANDSIM to YES.
# see z.ext/nandsim
# default value is YES
KDRV_NANDSIM		?= NO

# if you want to test a power management, set KDRV_CONFIG_PM to YES
# default value is NO
KDRV_CONFIG_PM		?= NO

# enable debug feature
DEBUG				?= y

# build kernel dirver for QEMU
USE_QEMU_SYSTEM		?= NO

#UNIVDEC				?= vdec
UNIVDEC				?= univdec

SIC_LAB_BOARD		?= NO

# TODO(daeseok.youn) : need to build for executing without dtv binary
# This value will be removed after ready to build completely DTV for v4
# default vaule is NO.
WITHOUT_DTV         ?= NO
export WITHOUT_DTV

export KDRV_GLOBAL_LINK KDRV_TEST_APP KDRV_NANDSIM KDRV_CONFIG_PM DEBUG
export USE_QEMU_SYSTEM
export UNIVDEC

ifeq ($(KERNELVERSION),)
#==============================================================================
#
#	KERNEL MODULE
#
#==============================================================================
KDRV_TOP_DIR		:= $(shell pwd)
KDRV_BUILD_DIR		:= $(KDRV_TOP_DIR)/build
TEST_APP_DIR		:= $(KDRV_TOP_DIR)/z.ext/kdriver_test
GP3_ROOT_DIR 		:= $(shell cd ../../../.. && pwd)

KO_OUT_DIR			?= $(ANDROID_ROOT_DIR)/vendor/lge/$(PRODUCT_NAME)/prebuilt/kernel/modules
MKNOD_OUT_DIR		?= $(ANDROID_ROOT_DIR)/vendor/lge/$(PRODUCT_NAME)
OUT_DIR				?= /share/gtv_platform/$(PRODUCT_NAME)/v4/$(USER)/root/system/vendor
TFTP_DIR			?= $(GP3_ROOT_DIR)/target/$(PLATFORM_CHIP)/tftp


# KERNEL_NAME should be defined outside of this makefile
# If you want to fix KERNEL_NAME for test. uncomment the below line
# KERNEL_NAME		?= linux-2.6.33-mvista
#KERNEL_DIR			:= ../../kernel/$(KERNEL_NAME)
KERNEL_DIR			:= $(ANDROID_ROOT_DIR)/kernel
INSTALL_DIR := $(ANDROID_ROOT_DIR)/vendor/lge/$(PRODUCT_NAME)/prebuilt/kernel/modules


# this flag makes kdriver to be kernel module regardless of kernel configuration
KDRV_MODULE_BUILD 	:= YES

# export core environment variable for each module Makefile
export GP3_ROOT_DIR	KDRV_TOP_DIR OUT_DIR KDRV_OUT_DIR TFTP_DIR KERNEL_DIR KERNEL_NAME
export KDRV_MODULE_BUILD

include $(KDRV_TOP_DIR)/modules.mk

export INCLUDE_L8_CHIP_KDRV
export INCLUDE_L9_CHIP_KDRV

KDIFF					=	../../tools/kdiff.sh

#------------------------------------------------------------------------------
# default target
#------------------------------------------------------------------------------
ifeq ($(USE_CITRIX_SERVER), YES)
all : build_kdriver prodgsync_kdriver copy_kdriver
else
all : build_kdriver copy_kdriver
endif
	# KDRIVER BUILD DONE...

clean : clean_kdriver
	# KDRIVER CLEAN DONE...

#------------------------------------------------------------------------------
# PHONEY target list
#------------------------------------------------------------------------------
.PHONEY: pre_build_kdriver param
.PHONEY: .check_env

build_kdriver : .check_env pre_build_kdriver

ifeq ($(KDRV_GLOBAL_LINK), YES)

	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(ECHO) "<!> LG KERNEL DRIVER : GLOBAL LINK MODE.."
	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(MAKE) -C $(KERNEL_DIR) M=$(KDRV_TOP_DIR) ARCH=arm CROSS_COMPILE=arm-eabi- modules

ifeq ($(KDRV_MODULE_BUILD), YES)
	@-$(MV) $(KDRV_TOP_DIR)/kdrv*.ko		$(KDRV_BUILD_DIR)
	@-$(MV) $(KDRV_TOP_DIR)/Module.* 		$(KDRV_BUILD_DIR)
	@-$(MV) $(KDRV_TOP_DIR)/modules.order 	$(KDRV_BUILD_DIR)
	@-$(STRIP) -d $(KDRV_BUILD_DIR)/kdrv_$(PLATFORM_CHIP).ko
endif
else
	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(ECHO) "<!> LG KERNEL DRIVER : UNIT LINK MODE.."
	@$(ECHO) "-------------------------------------------------------------------------------"

	@( $(foreach mod, $(BASE_MODULE), \
		$(MAKE) -C $(KERNEL_DIR) M=$(KDRV_TOP_DIR)/$(mod) &&) $(ECHO) -n ) || exit 1

endif

	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(ECHO) "<!> LG KERNEL DRIVER : EXTRA MODULE.."
	@$(ECHO) "-------------------------------------------------------------------------------"

	@( $(foreach mod, $(EXT_MODULE), \
		$(MAKE) -C $(KERNEL_DIR) M=$(KDRV_TOP_DIR)/$(mod) &&) $(ECHO) -n ) || exit 1

ifeq ($(KDRV_TEST_APP), YES)
	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(ECHO) "<!> LG KERNEL DRIVER : TEST APP.."
	@$(ECHO) "-------------------------------------------------------------------------------"
	@make -C $(TEST_APP_DIR)
endif

	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(ECHO) "<!> LG KERNEL DRIVER : COMPLETED.."
	@$(ECHO) "-------------------------------------------------------------------------------"

clean_kdriver :
	#
	# debug - cleaning lg1150 driver
	#
	@$(FIND) $(KDRV_TOP_DIR) -name '*.o'	\
                     -o -name .tmp_versions \
                     -o -name '*.out'       \
                     -o -name '*.cmd'       \
                     -o -name '*.symvers'   \
                     -o -name '*.markers'	\
                     -o -name '*.order'     \
                     -o -name '*.mod.c'     \
                     -o -name 'kdrv_*.ko' | xargs $(RM) -rf

ifeq ($(KDRV_TEST_APP), YES)
	@make -C $(TEST_APP_DIR) clean
endif

	@$(ECHO) "<!> job complated"

#------------------------------------------------------------------------------
# copy kernel driver to predefined directory ( USE_CITRIX_SERVER=NO )
#------------------------------------------------------------------------------
copy_kdriver :
	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(ECHO) "<!> LG KERNEL DRIVER : COPYING DRIVER.."
	@$(ECHO) "-------------------------------------------------------------------------------"
ifeq ($(KDRV_GLOBAL_LINK), YES)
#	@$(CP) -f $(KDRV_TOP_DIR)/build/kdrv_$(PLATFORM_CHIP).ko $(KO_OUT_DIR)
endif

#	@$(CP) -f $(KDRV_TOP_DIR)/build/lgmknods.sh $(MKNOD_OUT_DIR)
	@$(CD) $(KDRV_TOP_DIR)
ifeq ($(KDRV_NANDSIM), YES)
	@$(CP) -f ../z.ext/nandsim/nandsim.ko $(OUT_DIR)
	@$(CP) -f ../z.ext/nandsim/go.nandsim $(OUT_DIR)
endif

ifeq ($(KDRV_TEST_APP), YES)
	@make -C $(TEST_APP_DIR) copy
endif

#------------------------------------------------------------------------------
# copy kernel driver to predefined directory ( USE_CITRIX_SERVER=YES )
#------------------------------------------------------------------------------
prodgsync_kdriver:
	@$(ECHO) "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
	@$(ECHO) "<!> LG KERNEL DRIVER : COPYING(SYNC) DRIVER.."
	@$(ECHO) "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

	@$(RM) $(CHANGE_FILE_LIST) && touch $(CHANGE_FILE_LIST)

ifeq ($(KDRV_GLOBAL_LINK), YES)
ifeq ($(KDRV_MODULE_BUILD), YES)
	@cd $(KDRV_BUILD_DIR) && find kdrv_$(PLATFORM_CHIP).ko -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)
endif
endif
#	@cd $(KDRV_BUILD_DIR) && find lgmknods.sh	 -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)

#	@cd $(KDRV_BUILD_DIR) && find SampleModule.ko -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)

#	@cd $(KDRV_TOP_DIR)/mali/$(PLATFORM_CHIP) && find mali*.ko -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)

ifeq ($(KDRV_NANDSIM), YES)
	@cd ../z.ext/nandsim && find nandsim.ko -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)
	@cd ../z.ext/nandsim && find go.nandsim -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)
endif

	# Copying DE firmware...
#	@cd $(KDRV_TOP_DIR)/de && find firmware -exec $(IS_DIFFER) {} $(KDRV_OUT_DIR) $(OUT_DIR) \; | $(CPIO_CMD) $(KDRV_OUT_DIR)

	@$(TRANS_IMAGES) $(OUT_DIR)

	# progsync kernel driver if file is not empty...
	@if [ -s $(CHANGE_FILE_LIST) ]; then						\
		GZIP="--fast" $(PROD_GSYNC) $(OUT_DIR) | tr ":" ".";	\
	else														\
		touch $(CHANGE_FILE_FLAG);								\
	fi


check_kver:
	$(MAKE)  -C ../../kernel check_kver

install:
	@$(CP) -f $(KDRV_BUILD_DIR)/kdrv_$(PLATFORM_CHIP).ko $(INSTALL_DIR)

#------------------------------------------------------------------------------
# TODO) check if kdriver is patched successfully or not
#------------------------------------------------------------------------------
pre_build_kdriver : #check_kver
	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(ECHO) "<!> LG KERNEL DRIVER : MAKING OUT_DIR & TFTP_DIR..."
	@$(ECHO) "-------------------------------------------------------------------------------"
	@$(MKDIR) $(OUT_DIR)
	@$(MKDIR) $(TFTP_DIR)

#------------------------------------------------------------------------------
# check if all required environment variables are set or not
#------------------------------------------------------------------------------
.check_env :
ifeq ($(OUT_DIR),)
	$(error "<OUT_DIR> is not defiend...")
endif
ifeq ($(TFTP_DIR),)
	$(error "<TFTP_DIR> not defiend...")
endif

#------------------------------------------------------------------------------
# some debug
#------------------------------------------------------------------------------
param :
	@echo "-------------------------------------------------------------------------------"
	@echo "KERNEL_DIR           = $(KERNEL_DIR)"
	@echo "OUT_DIR              = $(OUT_DIR)"
	@echo "TFTP_DIR             = $(TFTP_DIR)"
	@echo "KDRV_TOP_DIR         = $(KDRV_TOP_DIR)"
	@echo "DEBUG                = $(DEBUG)"
	@echo "-------------------------------------------------------------------------------"
	@echo "INCLUDE_L8_CHIP_KDRV = $(INCLUDE_L8_CHIP_KDRV)"
	@echo "INCLUDE_L9_CHIP_KDRV = $(INCLUDE_L9_CHIP_KDRV)"
	@echo "-------------------------------------------------------------------------------"
	@echo "EXTRA_CFLGAS         = $(EXTRA_CFLAGS)"
	@echo "-------------------------------------------------------------------------------"
	@echo "BASE_MODULE    		= $(BASE_MODULE)"
	@echo "-------------------------------------------------------------------------------"
	@echo "UNIVDEC              = $(UNIVDEC)"

else # KERNELVERSION

#==============================================================================
#
#	KERNEL BUILTIN
#
#==============================================================================
# the below paramter is used when kdriver is built into kernel
KDRV_TOP_DIR        := drivers/lg_kdriver
export KDRV_TOP_DIR

include $(KDRV_TOP_DIR)/modules.mk
include $(addprefix $(KDRV_TOP_DIR)/,$(addsuffix /Makefile,$(BASE_MODULE)))

#
# raxis.lim (2010/11/17)
# if KDRV_MODULE_BUILD is defined, we will make kdrv_lg1150.ko regardless of kernel configuration
# kDRV_MODULE_BUILD is defined at build/Makefile
#
ifeq ($(KDRV_MODULE_BUILD), YES)
CONFIG_LG_BUILTIN_KDRIVER := m
endif

obj-$(CONFIG_LG_BUILTIN_KDRIVER)	+= kdrv_$(PLATFORM_CHIP).o
kdrv_$(PLATFORM_CHIP)-objs 			:= $(KDRV_MODULE_OBJS)

endif # KERNELVERSION
