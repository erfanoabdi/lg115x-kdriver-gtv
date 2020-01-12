
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
#	FILE NAME	:	incs.mk                                                    #
#	VERSION		:	1.0                                                        #
#	AUTHOR		:   raxis.lim@lge.com                                          #
#	DATE        :	                                                           #
#	DESCRIPTION	:	Basic header files for kernel driver                       #
#                   see tools.mk and incs.mk in TV src directory               #
#******************************************************************************#

#==============================================================================
# Chip Configuration
#
# by default, all chip driver shoule be enabled.
# if the specific driver is built for the mass production, only one driver should
# be enabled
#
# (2011/04/11) current kernel kdriver doesn't support this feature yet.
#
#==============================================================================
INCLUDE_L8_CHIP_KDRV ?= NO
INCLUDE_L9_CHIP_KDRV ?= YES

CONFIG_MACH_L8_ANDROID ?= y

#==============================================================================
# Module Configuration
#==============================================================================
INCLUDE_KDRV_SAMPLE	= NO
INCLUDE_KDRV_AAD	= YES
INCLUDE_KDRV_ADEC	= YES
INCLUDE_KDRV_AFE	= YES
INCLUDE_KDRV_CI		= YES
INCLUDE_KDRV_DE		= YES
INCLUDE_KDRV_DEMOD	= YES
INCLUDE_KDRV_DENC	= YES
INCLUDE_KDRV_DIMM	= YES
INCLUDE_KDRV_FBDEV	= YES
INCLUDE_KDRV_FMT3D	= YES
INCLUDE_KDRV_FRC	= YES
INCLUDE_KDRV_GFX	= YES
INCLUDE_KDRV_GPU	= NO
INCLUDE_KDRV_GPIO	= YES
INCLUDE_KDRV_HDMI	= YES
INCLUDE_KDRV_I2C	= YES
INCLUDE_KDRV_PE		= YES
INCLUDE_KDRV_PVR	= NO    #NO in cosmo : daeseok.youn
INCLUDE_KDRV_SDEC	= YES
INCLUDE_KDRV_TTX	= YES
INCLUDE_KDRV_VDEC	= YES
INCLUDE_KDRV_VENC	= NO    #NO in cosmo : daeseok.youn
INCLUDE_KDRV_PNG	= NO    #No in cosmo : daeseok.youn
INCLUDE_KDRV_SE		= YES
INCLUDE_KDRV_SYS	= YES
INCLUDE_KDRV_SCI	= YES
INCLUDE_KDRV_CLK	= YES
INCLUDE_KDRV_MONITOR= YES
INCLUDE_KDRV_MJPEG	= YES   #jaeseop.so (for jpeg encoding)

ifeq ($(PROJECT_NAME), EDEN_KR)
INCLUDE_KDRV_PVR	= YES
INCLUDE_KDRV_DUALDPB=YES
endif

ifeq ($(PROJECT_NAME), EDEN)
INCLUDE_KDRV_PVR	= NO
INCLUDE_KDRV_DUALDPB=YES
endif

ifneq ($(INCLUDE_KDRV_DE), YES)
INCLUDE_KDRV_PE		= NO
endif

#==============================================================================
# Extra Configuration
#==============================================================================
#
# INCLUDE_GPU_MALI:
# YES -- if kdriver should know the existence of MALI and reserve its runtime memory
# NO  -- otherwise ( MALI is not supported nor kdriver doesn't know the exitence of MALI )
#        when MALI is configured to allocated its memory in OS memory, set this value to NO
#
INCLUDE_GPU_MALI	= NO

#==============================================================================
# Global macro functions to define compiler directives
#------------------------------------------------------------------------------
#     1. Macro function cond_define(param1,param2)
#       This macro wll be expanded to
#           -Dparam2    <== If 'param2' has a same value with param1
#     2. Macro function cond_assign(param1,param2,param3,param4)
#       This macro wll be expanded to
#           param3      <== if 'param2' has a same value with param1
#           param4      <== otherwise
#
# cond_define = $(strip $(if $(subst $(strip $1)A,,$($(strip $2))A),,-D$(strip $2)))
# cond_assign = $(strip $(if $(subst $(strip $1)A,,$($(strip $2))A),$4,$3))
#==============================================================================
_cnd_define  = $(if $(subst $(1)A,,$(strip $($2))A),,-D$2)
_cnd_assign  = $(if $(subst $(1)A,,$(strip $($2))A),$4,$3)
cond_define  = $(call _cnd_define,$(strip $1),$(strip $2))
cond_assign  = $(call _cnd_assign,$(strip $1),$(strip $2),$(strip $3),$(strip $4))

#==============================================================================
# HOST TOOLS
#==============================================================================
CC			 =  $(CROSS_COMPILE)gcc
STRIP		 =  $(CROSS_COMPILE)strip
CP           =  cp
RM           =  rm -f
MV           =  mv -f
CD           =  cd
LN           =  ln -s
TAR          =  tar
CAT          =  cat
SED          =  sed
AWK          =  awk
GZIP         =  gzip
ECHO         =  echo
MAKE         =  make
FIND         =  find
FIND_L       =  find -L
CHMOD        =  chmod
TOUCH        =  touch
MKDIR        =  mkdir -p
RMDIR        =  rm -rf
DOS2UNIX     =  dos2unix
CPIO_CMD     =  LC_ALL=C cpio -pudm --quiet
CCDV		 =  ccdv -v0 -p -x 0

#==============================================================================
#   Add on tools to transfer output files to transfer server.
#==============================================================================
IS_NEWER     		=  is_newer.sh
IS_DIFFER    		=  is_differ.sh
TRANS_IMAGES 		=  trans_images.sh
PROD_SYNC           =   /prod/bin/prodsync
PROD_GSYNC          =   /prod/bin/prodgsync
