/****************************************************************************************
 * SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 * Copyright(c) 2010 by LG Electronics Inc.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * ***************************************************************************************/

/** @file
 *
 *	main configuration for lg1150 driver
 *
 *  author		daeyoung lim ( raxis.lim@lge.com )
 *  version		1.0
 *  date		2009.11.04
 *
 *  @addtogroup lg1150_base
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#define MEMCFG_DEBUG 			2		// 1 : summary for each module, 2 : detailed print enabled.
#define MEMCFG_ALLOC 			1		// force allocation ignoring base address for DDR0(silkroad 0)
#define USE_REBASE				0

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/delay.h>

#include "base_types.h"
#include "base_dev_cfg.h"
#include "debug_util.h"
#include "os_util.h"
#define MEMCFG_CHECK_TEST 0
#if MEMCFG_CHECK_TEST
#include <asm/io.h>				/**< For ioremap_nocache */
#endif

#ifdef INCLUDE_KDRV_ADEC
#include "adec_cfg.h"
#endif
#ifdef INCLUDE_KDRV_DE
#include "de_cfg.h"
#endif
#ifdef INCLUDE_KDRV_FBDEV
#include "fbdev_cfg.h"
#endif
#ifdef INCLUDE_KDRV_GFX
#include "gfx_cfg.h"
#endif
#ifdef INCLUDE_KDRV_PVR
#include "pvr_cfg.h"
#endif
#ifdef INCLUDE_KDRV_TTX
#include "ttx_cfg.h"
#endif
#ifdef INCLUDE_KDRV_VDEC
#include "vdec_cfg.h"
#endif
#ifdef INCLUDE_KDRV_SDEC
#include "sdec_cfg.h"
#endif
#ifdef INCLUDE_KDRV_SE
#include "se_cfg.h"
#endif
#ifdef INCLUDE_KDRV_VENC
#include "venc_cfg.h"
#endif
#ifdef INCLUDE_KDRV_PNG
#include "png_cfg.h"
#endif
#ifdef INCLUDE_KDRV_AFE
#include "afe_cfg.h"
#endif
#ifdef INCLUDE_KDRV_MJPEG
#include "mjpeg_cfg.h"
#endif
#ifdef INCLUDE_KDRV_DE
#include "de_cfg.h"
#endif

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define LX_MEMCFG_ENTRY(_m_entry)	{ .name= _STR(_m_entry), .pM=(LX_MEMCFG_T*)&(_m_entry), .nM=sizeof(_m_entry)/sizeof(LX_MEMCFG_T) }
#define LX_MEMCFG_ENTRY_END			{ .name="Total", .pM=NULL, .nM=0}

#define BIT_ALIGN_UP(_n, _b)		( ((_n) + (1<<(_b)) -1) & ( ~((1<<(_b))-1)) )
#define SWAP(_p1,_p2)				({typeof(*_p1) _tmp = *_p2;  *_p2 = *_p1; *_p1 = _tmp; })

#define _MB_(_sz)	(( _sz)>>20)
#define _KB_(_sz)	(((_sz)>>10)&0x3ff)
#define _BB_(_sz)	(( _sz)     &0x3ff)

#define LX_MEMCFG_ENTRY_NAME(Entry)	((LX_MEMCFG_T*)&(Entry))->name
#define LX_MEMCFG_ENTRY_BASE(Entry)	((LX_MEMCFG_T*)&(Entry))->base
#define LX_MEMCFG_ENTRY_SIZE(Entry)	((LX_MEMCFG_T*)&(Entry))->size

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern char *simple_strtok(char *s, const char *delim, char **save_ptr);

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
/*
 * In order to maintain memory configuration without overlapping between kdrv_lg1150 and mali kernel driver,
 * mali kernel driver SHOULD BE INITIALIZED ONLY AFTER lg1150 kernel driver module.
 *
 * after BASE_MEMCFG_AllocDevMem() shall calculate base address for mali gpu, then mali kernel module
 * depends on below variable to initialize its memory.
 * 20100813 seokjoo.lee.
 */
unsigned long kdrv_gpu_pmem_base;
unsigned long kdrv_gpu_pmem_size;

EXPORT_SYMBOL(kdrv_gpu_pmem_base);
EXPORT_SYMBOL(kdrv_gpu_pmem_size);

/* kernel memory size affects memory configuration of kdriver.
 * so this value should be kept to variable to read at any time.
 *
 */
UINT32	kdrv_osmem_size = 0;

#ifdef MAKE_SE_MODULE
#include "../se/se_mem_cfg.c"
#endif


/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/
static void BASE_MEMCFG_PRINT			(struct seq_file *m, LX_MEMCFG_T *pM, int addr_shift, UINT32 prvEnd);
static int	BASE_MEMCFG_Compare			(LX_MEMCFG_ENTRY_T *p1, LX_MEMCFG_ENTRY_T *p2);
static void BASE_MEMCFG_Sort			(LX_MEMCFG_ENTRY_T	*pMemCfgEntry, int nEntries);

static void BASE_MEMCFG_ParseOSMemSize	(void);
static void BASE_MEMCFG_SetBase 		(UINT32 ddr0base, UINT32 ddr0size, UINT32 ddr1base, UINT32 ddr1size, UINT32 ddr2base, UINT32 ddr2size );
static void BASE_MEMCFG_PreAllocDevMem	(void);
static int  BASE_MEMCFG_AllocDevMem		(void);

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
/**
 * Total DDR0/1 size Holder.
 *
 * @note "Total" means that maximum memory to be used for the kernel driver.
 *		 Its meaning is changed since L9.
 *		 L9 supports logically continuous memory space in spite of physically separated.
 *		 So it is impossible to determine valid start address of device driver with /proc/cmdline information.
 */
static LX_MEMCFG_T	sMemCfgDDR[3]	= { { .name = "DDR0", .base = 0,	.size = 256*1024*1024},
										{ .name = "DDR1", .base = 0,	.size = 256*1024*1024},
										{ .name = "DDR2", .base = 0,	.size = 128*1024*1024} };
/**
 * Allocation Start point of physical address for DDR 0.
 *
 * @note "base" field is copied from sMemCfgDDR data
 * @note "base" address is set by BASE_MEMCFG_SetBase
 * @note "size" should always to be zero.
 *
 */
static LX_MEMCFG_T	sMemCfgHeader[3]= { { .name = "DDR0 LX", .base = 0, .size = 0 },
										{ .name = "DDR1 LX", .base = 0, .size = 0 },
										{ .name = "DDR2 LX", .base = 0, .size = 0 } };
/**
 * Memory status used in device
 */
static LX_MEMCFG_STAT_T sMemCfgStat;

/**
 * GPU memory configuration.
 *
 * This shall reserve memory area for MALI 400 GPU in order to prevent overlapping by other modules.
 *
 * .size SHALL be parsed by "pmem=###M" from /proc/cmdline for Android and Linux.
 */
static LX_MEMCFG_T	sMemCfgGPU		= { .name = "GPU pmem", .base = 0x0, .size = 0x0 };

/*---------------------------------------------------------------------------------------
 * Global Memory Configuration data base (L8)
 *--------------------------------------------------------------------------------------*/
static LX_MEMCFG_ENTRY_T	sMemCfgList_L8[] =
{
	LX_MEMCFG_ENTRY(sMemCfgHeader[0]),
	LX_MEMCFG_ENTRY(sMemCfgGPU),

#ifdef INCLUDE_KDRV_GFX
	LX_MEMCFG_ENTRY(gMemCfgGfx),
#endif

/* allocation shall be passed to mali400.ko
 */
#ifdef INCLUDE_KDRV_PVR
	LX_MEMCFG_ENTRY(gMemCfgDvr[0]),
#endif
#ifdef INCLUDE_KDRV_SDEC
	LX_MEMCFG_ENTRY(gMemCfgSDECGPB[0]),
#endif
#ifdef INCLUDE_KDRV_SE
	LX_MEMCFG_ENTRY(gMemCfgSE),
	LX_MEMCFG_ENTRY(gMemCfgSE_TZFW),
#endif
#ifdef INCLUDE_KDRV_ADEC
	LX_MEMCFG_ENTRY(gstMemCfgADEC[0]),
#endif
#ifdef INCLUDE_KDRV_VDEC
	LX_MEMCFG_ENTRY(gMemCfgVdec[0]),
#endif
#ifdef INCLUDE_KDRV_VENC
	LX_MEMCFG_ENTRY(gpstVencMemConfig),
#endif

#ifdef INCLUDE_KDRV_PNG
	LX_MEMCFG_ENTRY(gMemCfgPng[0]),
#endif

#ifdef INCLUDE_KDRV_GFX
//	LX_MEMCFG_ENTRY(gMemCfgGfx),
#endif

	LX_MEMCFG_ENTRY(sMemCfgHeader[1]),

#ifdef INCLUDE_KDRV_FBDEV
	LX_MEMCFG_ENTRY(gMemCfgFBDev),
	LX_MEMCFG_ENTRY(gMemCfgFBHdr),
#endif
#ifdef INCLUDE_KDRV_TTX
	LX_MEMCFG_ENTRY(gstBufMemCfgTTXRev[0]),
#endif
#ifdef INCLUDE_KDRV_DE
	LX_MEMCFG_ENTRY(gMemCfgDe[0]),
#endif
#ifdef INCLUDE_KDRV_AFE
	LX_MEMCFG_ENTRY(gMemCfgCvd[0]),				/* CVD (3DComb H/W Memory) */
#endif
	LX_MEMCFG_ENTRY_END
};

/*---------------------------------------------------------------------------------------
 * Global Memory Configuration data base (L9/GP)
 *--------------------------------------------------------------------------------------*/
static LX_MEMCFG_ENTRY_T	sMemCfgList_L9_GP[] =
{
	LX_MEMCFG_ENTRY(sMemCfgHeader[0]),			/* DDR0 START */
	LX_MEMCFG_ENTRY(sMemCfgGPU),				/* MALI */
#ifdef INCLUDE_KDRV_VDEC
	LX_MEMCFG_ENTRY(gMemCfgVdec[0]),			/* VDEC for DDR0 */
#endif

#ifdef INCLUDE_KDRV_GFX
	LX_MEMCFG_ENTRY(gMemCfgGfx),				/* GFX */
#endif
#ifdef INCLUDE_KDRV_FBDEV
	LX_MEMCFG_ENTRY(gMemCfgFBDev[0]),			/* FBDEV OSD0 */
	LX_MEMCFG_ENTRY(gMemCfgFBDev[1]),			/* FBDEV OSD1 */
	LX_MEMCFG_ENTRY(gMemCfgFBDev[2]),			/* FBDEV OSD2 */
//	LX_MEMCFG_ENTRY(gMemCfgFBDev[3]),			/* FBDEV OSD3 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[0]),			/* FBDEV OSD0 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[1]),			/* FBDEV OSD1 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[2]),			/* FBDEV OSD2 */
//	LX_MEMCFG_ENTRY(gMemCfgFBHdr[3]),			/* FBDEV OSD3 */
#endif

	LX_MEMCFG_ENTRY(sMemCfgHeader[1]),			/* DDR1 START */
#ifdef INCLUDE_KDRV_VDEC
	LX_MEMCFG_ENTRY(gMemCfgVdec[1]),			/* VDEC for DDR1 */
#endif
#ifdef INCLUDE_KDRV_ADEC
	LX_MEMCFG_ENTRY(gstMemCfgADEC[0]),			/* ADEC */
#endif
#ifdef INCLUDE_KDRV_PNG
	LX_MEMCFG_ENTRY(gMemCfgPng[1]),				/* PNG */
#endif
#ifdef INCLUDE_KDRV_SDEC
	LX_MEMCFG_ENTRY(gMemCfgSDECGPB[1]),			/* SDEC/TE */
#endif
#ifdef INCLUDE_KDRV_PVR
	LX_MEMCFG_ENTRY(gMemCfgDvr[1]),				/* SDEC/PVR */
#endif
#ifdef INCLUDE_KDRV_TTX
	LX_MEMCFG_ENTRY(gstBufMemCfgTTXRev[1]),		/* VBI Buffer */
#endif
#ifdef INCLUDE_KDRV_AFE
	LX_MEMCFG_ENTRY(gMemCfgCvd[1]),				/* CVD (3DComb H/W Memory) */
#endif
#ifdef INCLUDE_KDRV_VENC
	LX_MEMCFG_ENTRY(_stVencMemComfigPreset[0]),	/* VENC */
#endif
#ifdef INCLUDE_KDRV_MJPEG
	LX_MEMCFG_ENTRY(g_stMemCfgMJPEG),			/* MJPEG */
#endif
#ifdef INCLUDE_KDRV_SE
	LX_MEMCFG_ENTRY(gMemCfgSE), 				/* SE */
	LX_MEMCFG_ENTRY(gMemCfgSE_TZFW),			/* SE/TZ */
#endif

    LX_MEMCFG_ENTRY(sMemCfgHeader[2]),          /* DDR2 START */
#ifdef INCLUDE_KDRV_DE
	LX_MEMCFG_ENTRY(gMemCfgDe[2]),				/* DE */
#endif
#ifdef INCLUDE_KDRV_FBDEV
	LX_MEMCFG_ENTRY(gMemCfgFBDev[3]),			/* FBDEV - OSD3 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[3]),			/* FBDEV - OSD3 */
#endif
	LX_MEMCFG_ENTRY_END
};

/*---------------------------------------------------------------------------------------
 * Global Memory Configuration data base (L9/COSMO)
 *--------------------------------------------------------------------------------------*/
static LX_MEMCFG_ENTRY_T	sMemCfgList_L9_COSMO[] =
{
	LX_MEMCFG_ENTRY(sMemCfgHeader[0]),			/* DDR0 START */

/*
 * moved from DDR1 to here
 */

#ifdef INCLUDE_KDRV_PVR
	LX_MEMCFG_ENTRY(gMemCfgDvr[1]),				/* SDEC/PVR */
#endif

#ifdef INCLUDE_KDRV_ADEC
	LX_MEMCFG_ENTRY(gstMemCfgADEC[0]),			/* ADEC */
#endif
#ifdef INCLUDE_KDRV_PNG
	LX_MEMCFG_ENTRY(gMemCfgPng[1]),				/* PNG */
#endif
#ifdef INCLUDE_KDRV_SDEC
	LX_MEMCFG_ENTRY(gMemCfgSDECGPB[1]),			/* SDEC/TE */
#endif

#ifdef INCLUDE_KDRV_TTX
	LX_MEMCFG_ENTRY(gstBufMemCfgTTXRev[1]),		/* VBI Buffer */
#endif

#ifdef INCLUDE_KDRV_SE
	LX_MEMCFG_ENTRY(gMemCfgSE),					/* SE */
	LX_MEMCFG_ENTRY(gMemCfgSE_TZFW),			/* SE/TZ */
#endif

#ifdef INCLUDE_KDRV_VENC
	LX_MEMCFG_ENTRY(_stVencMemComfigPreset[0]),	/* VENC */
#endif

	LX_MEMCFG_ENTRY(sMemCfgGPU),				/* MALI */
#ifdef INCLUDE_KDRV_GFX
	LX_MEMCFG_ENTRY(gMemCfgGfx),				/* GFX */
#endif

#ifdef INCLUDE_KDRV_AFE
	LX_MEMCFG_ENTRY(gMemCfgCvd[1]),				/* CVD (3DComb H/W Memory) */
#endif

#ifdef INCLUDE_KDRV_FBDEV
	LX_MEMCFG_ENTRY(gMemCfgFBDev[0]),			/* FBDEV OSD0 */
	LX_MEMCFG_ENTRY(gMemCfgFBDev[1]),			/* FBDEV OSD1 */
	LX_MEMCFG_ENTRY(gMemCfgFBDev[2]),			/* FBDEV OSD2 */
//	LX_MEMCFG_ENTRY(gMemCfgFBDev[3]),			/* FBDEV - OSD3 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[0]),			/* FBDEV OSD0 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[1]),			/* FBDEV OSD1 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[2]),			/* FBDEV OSD2 */
//	LX_MEMCFG_ENTRY(gMemCfgFBHdr[3]),			/* FBDEV OSD3 */
#endif
#ifdef INCLUDE_KDRV_MJPEG
	LX_MEMCFG_ENTRY(g_stMemCfgMJPEG),			/* MJPEG */
#endif

	LX_MEMCFG_ENTRY(sMemCfgHeader[1]),			/* DDR1 START */
#ifdef INCLUDE_KDRV_VDEC
	LX_MEMCFG_ENTRY(gMemCfgVdec[1]),			/* VDEC */
#endif

    LX_MEMCFG_ENTRY(sMemCfgHeader[2]),          /* DDR2 START */
#ifdef INCLUDE_KDRV_DE
	LX_MEMCFG_ENTRY(gMemCfgDe[2]),				/* DE */
#endif
#ifdef INCLUDE_KDRV_FBDEV
	LX_MEMCFG_ENTRY(gMemCfgFBDev[3]),			/* FBDEV OSD3 */
	LX_MEMCFG_ENTRY(gMemCfgFBHdr[3]),			/* FBDEV OSD3 */
#endif
	LX_MEMCFG_ENTRY_END
};


static LX_MEMCFG_ENTRY_T*	sMemCfgList;

/** Print format string.
 *
 * @see BASE_MEMCFG_Print BASE_MEMCFG_PRINT
 */
										// 12345678901234567890,0x12345678,+0x12345678,->,0x12345678,0x12345678,1234MB+1234kB,1234MB+1234kB,~,1234MB+1234kB
static const char *fmtMemCfgHeadLine	= "---------------------------------------------------------------------------------------------------------------\n"
										  "Name                ,  org base,   org size,..,ALLOC BASE,ALLOC  END,alloc size MB,alloc base MB,~,alloc end MB\n";
static const char *fmtEntryFormat		= "%-20.20s,0x%08x,+0x%08x,=>,0x%08x,0x%08x,%4dMB+%4dkB,%4dMB+%4dkB,~,%4dMB+%4dkB\n";
static const char *fmtEntryFormatSub	= " %-19.19s,0x%08x,+0x%08x,=>,0x%08x,0x%08x,%4dMB+%4dkB,%4dMB+%4dkB,~,%4dMB+%4dkB\n";

/*========================================================================================
	Implementation Group
========================================================================================*/
void	BASE_MEMCFG_Init ( void )
{
	int	ret;

	BASE_MEMCFG_ParseOSMemSize( );

	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) )	/* L9 */
	{
		if ( lx_chip_plt() == LX_CHIP_PLATFORM_COSMO )
		{
			sMemCfgList = sMemCfgList_L9_COSMO;
#ifndef USE_ADDRESS_SWITCH_CASE1
			BASE_MEMCFG_SetBase( 0x28000000, 128<<20, 0x70000000, 256<<20, 0x80000000, 128<<20 );
#else
/*
 * OS kernel memory size : 736MB, GPU + SDEC + ADEC + SE : 160MB , VDEC : 128MB
 */
#ifdef INCLUDE_KDRV_PVR
			BASE_MEMCFG_SetBase( 0x2D69F000, 170<<20, 0x70000000, 128<<20, 0x80000000, 128<<20 );
#else
			BASE_MEMCFG_SetBase( 0x2E000000, 160<<20, 0x70000000, 128<<20, 0x80000000, 128<<20 );
#endif
#endif
		}
		else
		{
			sMemCfgList = sMemCfgList_L9_GP;
			BASE_MEMCFG_SetBase( 0x20000000, 256<<20, 0x70000000, 256<<20, 0x80000000, 128<<20 );
		}
	}
	else /* L8 */
	{
		sMemCfgList = sMemCfgList_L8;
		BASE_MEMCFG_SetBase( 106<<20, 0, 512<<20, 0, 0, 0 );
	}

	/* DO some hooking or preinialization before starting memory allocation */
	BASE_MEMCFG_PreAllocDevMem( );

	/* DO automatic memory layout */
	ret = BASE_MEMCFG_AllocDevMem( );
	__CHECK_IF_ERROR( ret != RET_OK, printk, /* nop */, "fatal error at memcfg creation\n" );
}

static void BASE_MEMCFG_ParseOSMemSize		(void)
{
	struct file 	*fd_cmd_line;
	mm_segment_t	old_fs;
	int				buf_sz = 1024, read_sz;		// TODO stat for /proc/cmdline should be used. T.T
	char 			*_buf;

#ifdef MODULE
	fd_cmd_line = filp_open("/proc/cmdline", O_RDONLY, 0);

	if ( IS_ERR(fd_cmd_line) ) return;

	if ( NULL == (_buf = kmalloc(buf_sz, GFP_KERNEL) )) goto on_error_opened;

	old_fs = get_fs();
	set_fs(get_ds());

	if (	( ! fd_cmd_line->f_op)
		||	( ! fd_cmd_line->f_op->read) )				goto on_error_kmalloced;

	read_sz = fd_cmd_line->f_op->read(fd_cmd_line, _buf, buf_sz-1, &fd_cmd_line->f_pos);

	set_fs(old_fs);

	if ( read_sz < 0 )									goto on_error_kmalloced;

	_buf[buf_sz-1]= '\0';
#else
	/* 	raxis.lim (2010/11/19) - when kdriver becoems built-in kernel object, proc system is not
		available at this time. instead we can use saved_command_line in linux/init/main.c
	*/
	extern char *saved_command_line;

	if ( NULL == (_buf = kmalloc(buf_sz, GFP_KERNEL) )) goto on_error_out_of_memory;
	read_sz	= snprintf( _buf, buf_sz-1, "%s", saved_command_line );
#endif
	{
		char* p;
		char* args = _buf;

		while((p=strsep(&args, " \t")) != NULL)
		{
			if (!*p) continue;

			if (!strncmp(p, "mem=",4)) kdrv_osmem_size = memparse(p+4,NULL);
		}
	}
#ifdef KDRV_MODULE_BUILD
on_error_kmalloced:
		kfree(_buf);

on_error_opened:
		filp_close(fd_cmd_line, NULL);
#else
on_error_out_of_memory:
		PARAM_UNUSED(old_fs);		// dummy code to remove compile error and warning
		PARAM_UNUSED(fd_cmd_line);	// dummy code to remove compile error and warning
#endif

	printk("^y^OS mem %d MB\n", kdrv_osmem_size>>20 );
}

/**
 * Set Base address(Lowest address of DDR0) of BASE_MEMCFG_AllocDevMem.
 *
 * if any parameter below is zero, then /proc/cmdline format for parsing default value.
 *
 * [NOTE] If you want to change memory base address of modules in LG1150 kernel driver, do following step.
 * 	1. call this function to change the start point from where allocation shall be done,
 * 	2. call BASE_MEMCFG_AllocDevMem() to change each base address of each modules.
 * 	3. [OPTIONAL] BASE_MEMCFG_Print() to check the result.
 *
 *
 * 	"mem=###M"	   : for kernel memory size.
 * 	"pmem=###M"    : for pmem size specified by Android.
 * 	'ddr0mem=###M" : for total size of DDR 0 memory.
 * 	"ddr1mem=###M" : for total size of DDR 1 memory.
 *
 * @param base		[IN] baes address to begin pre-allocation(actually reserve or just calculate base address only)
 *						 if base == 0 then, try to get XXX from "mem=XXXM" via parsing /proc/cmdline.
 *						 otherwise base shall be set as parameter passed.
 * @param pmem		[IN] for Android, manual setting shall be used.
 * @param ddr0		[IN] DDR 0 Size in byte.
 * @param ddr1		[IN] DDR 1 Size in byte.
 *
 */
static void BASE_MEMCFG_SetBase( UINT32 ddr0base, UINT32 ddr0size, UINT32 ddr1base, UINT32 ddr1size, UINT32 ddr2base, UINT32 ddr2size )
{
	sMemCfgDDR[0].base	= ddr0base;
	sMemCfgDDR[0].size	= ddr0size;
	sMemCfgDDR[1].base	= ddr1base;
	sMemCfgDDR[1].size	= ddr1size;
	sMemCfgDDR[2].base	= ddr2base;
	sMemCfgDDR[2].size	= ddr2size;

	sMemCfgHeader[0].base = sMemCfgDDR[0].base;
	sMemCfgHeader[1].base = sMemCfgDDR[1].base;
	sMemCfgHeader[2].base = sMemCfgDDR[2].base;

	if (lx_chip_rev() >= LX_CHIP_REV(L9,A0) )
	{
		sMemCfgStat.size = sMemCfgDDR[0].size + sMemCfgDDR[1].size + sMemCfgDDR[2].size;
	}
	else /* L8 */
	{
		sMemCfgStat.size = (sMemCfgDDR[0].size - sMemCfgHeader[0].base) + 128*1024*1024;
	}

	printk("<!> mem param: kernel/ddr0/ddr1/ddr2 %08x/%08x@%08x/%08x@%08x/%08x@%08x\n",
							kdrv_osmem_size,
							sMemCfgDDR[0].size, sMemCfgDDR[0].base,
							sMemCfgDDR[1].size, sMemCfgDDR[1].base,
							sMemCfgDDR[2].size, sMemCfgDDR[2].base );

	return;
}

static void BASE_MEMCFG_PreAllocDevMem(void)
{
	if ( lx_chip_rev() >= LX_CHIP_REV(L9,A0) && lx_chip_plt() != LX_CHIP_PLATFORM_COSMO )
	{
		/* raxis.lim (2011/11/09)
		 * L9 GFX memory should be located right before FBDEV(OSD) memory.
	 	 * this assumption is very important since many application is running based on this assumption.
		 */
		LX_MEMCFG_ENTRY_BASE(gMemCfgGfx.surface) = LX_MEMCFG_ENTRY_BASE(gMemCfgFBDev[0]) - LX_MEMCFG_ENTRY_SIZE(gMemCfgGfx.surface);

		/* In L9 platform, mali can use the reserved memory in DDR0.
		 * mali is located right before the GFX memory
		 */
		LX_MEMCFG_ENTRY_BASE(sMemCfgGPU) = LX_MEMCFG_ENTRY_BASE(sMemCfgHeader[0]);
		LX_MEMCFG_ENTRY_SIZE(sMemCfgGPU) = LX_MEMCFG_ENTRY_BASE(gMemCfgGfx.surface) - LX_MEMCFG_ENTRY_BASE(sMemCfgHeader[0]);
	}
	else
	{
		if ( lx_chip_plt() == LX_CHIP_PLATFORM_COSMO )
		{

			/* sinchul.jung
			 * COSMO mali memory and size is not sync with mali kernel driver now.
			 * So JUST set the constant at here
			 */
#ifdef INCLUDE_KDRV_PVR
			LX_MEMCFG_ENTRY_BASE(sMemCfgGPU) = 0x30000000;
#else
			LX_MEMCFG_ENTRY_BASE(sMemCfgGPU) = LX_MEMCFG_ENTRY_BASE(sMemCfgHeader[0]) + 32*1024*1024;
#endif
			LX_MEMCFG_ENTRY_SIZE(sMemCfgGPU) = 58*1024*1024;
		}
	}


}

/**
 * Alloc(Reserve) Media memory.
 *
 * Actually, modify base addresses of each module's MemCfg.*base listed before sMemCfgHeader[1] in sMemCfgList array.
 *
 * [NOTE] Currently works only if MEMCFG_ALLOC is not zero,
 *
 * @param pMemCfgEntry	[IN] if NULL, use global memory configuration array @ref sMemCfgList.
 *
 * @return 0 if success, negative for error(allocation failed)
 */
static int BASE_MEMCFG_AllocDevMem(void)
{
	int					nEntries = 0;
	UINT32				prvEnd = 0;
	LX_MEMCFG_ENTRY_T	*pEntry;
	UINT32				addr_min[3]={0xffffffff,0xffffffff,0xffffffff}, addr_max[3]={0,0,0}, alloc_size[3]={0,0,0};
	int					ddr = 0; // for ddr 0
#if (USE_REBASE>0)
	SINT32				ddr0_rebase = 0x0;
#endif

#if (MEMCFG_ALLOC == 0 )
	return 0;
#endif

	pEntry = sMemCfgList;

	/*---------------------------------------------------------------------------
		Allocate (Reserve) memory for each memcfg listed entries in pMemCfgEntry
	---------------------------------------------------------------------------*/
	while ( pEntry && pEntry->pM )
	{
		LX_MEMCFG_T	*pM = pEntry->pM;
		int			nM  = pEntry->nM;
		UINT32		addr_start=0xffffffff, addr_end   = 0;

		if ( pM == &sMemCfgHeader[1] ) { // found DDR 1 Marker
			ddr = 1;
			prvEnd = sMemCfgHeader[1].base;
		}
		else if ( pM == &sMemCfgHeader[2] )
		{
			ddr = 2;
			prvEnd = sMemCfgHeader[2].base;
		}

	#if (MEMCFG_ALLOC > 0)

		/* raxis.lim (2011/09/15) -- I want to use automatic allocation by default */
		#if (MEMCFG_DEBUG > 3)
		printk("%s: %d: ddr%d pM->name '%20.20s', %08x -> prvEnd %08x\n", __FUNCTION__, __LINE__, ddr, pM->name, pM->base, prvEnd);
		#endif

	#endif	/* MEMCFG_ALLOC */

		// select allocation chunk in a module.
		for ( ; nM > 0; nM--,pM++)
		{
			#if (MEMCFG_DEBUG > 3)
			printed += BASE_MEMCFG_PRINT(NULL, pM, 0, prvEnd);
			#endif

			// makeup base address if forceAlloc or base is 0 (use previous chunk end)
			// keep track of end address of allocation chunk.
			if ( !pM->base )
			{
				pM->base = BIT_ALIGN_UP(prvEnd, 12);	// make start address to be aligned
				prvEnd += BIT_ALIGN_UP(pM->size, 12 );  // 4KB alignment
			}
			else
			{
				prvEnd = pM->base + pM->size;
				prvEnd = BIT_ALIGN_UP(prvEnd, 12 );		// make end address to be aligned
			}

			alloc_size[ddr] += BIT_ALIGN_UP(pM->size, 4 + 10 );
			addr_end   = max(addr_end  , prvEnd );
			addr_start = min(addr_start, prvEnd - pM->size);
		}

		// min/max range for each ddr.
		addr_min[ddr] = min(addr_min[ddr], addr_start);
		addr_max[ddr] = max(addr_max[ddr], addr_end);

		#if (MEMCFG_DEBUG > 3)
		{
			UINT32	module_mem_size;
			module_mem_size = addr_end - addr_start;

			printk(fmtEntryFormat,
				pEntry->name, addr_start, module_mem_size, addr_start, addr_end,
				_MB_(module_mem_size), _KB_(module_mem_size), //_BB_(module_mem_size),
				_MB_(addr_start     ), _KB_(addr_start     ), //_BB_(addr_start     ),
				_MB_(addr_end       ), _KB_(addr_end       )  //_BB_(addr_end       ),
				);
		}
		#endif	/* MEMCFG_DEBUG */

		pEntry++;
		nEntries ++;
	}

	{
		int i = 0;

		//printk(" ddr alloc %08x %08x ->%08x, %d :\n", addr_max[0], addr_min[0], (SINT32)addr_max[0] - (SINT32)addr_min[0],(SINT32)addr_max[0] - (SINT32)addr_min[0]);
		for ( i = 0; i< 3; i++)
		{
			printk("DDR%d Alloc Range : %08x~%08x (%5d MB free)\n", i, addr_min[i], addr_max[i], _MB_( sMemCfgDDR[i].size - alloc_size[i] ) );

			if ( addr_min[i] < sMemCfgHeader[i].base )
			{
				printk("^r^DDR[%d] CHECK ERROR. start addres 0x%08x is less than base 0x%08x\n", ddr, addr_min[i], sMemCfgHeader[i].base );
			}
			if ( addr_max[i] > (sMemCfgHeader[i].base+sMemCfgDDR[i].size) )
			{
				printk("^r^DDR[%d] CHECK ERROR. end addres 0x%08x is greater than end 0x%08x\n", ddr, addr_max[i], sMemCfgHeader[i].base+sMemCfgDDR[i].size );
			}
		}

		/* not used !! */
		sMemCfgStat.alloc = 0xffffffff;

#if (USE_REBASE>0)
		ddr0_rebase = (SINT32)sMemCfgDDR[0].size - (SINT32)BIT_ALIGN_UP(addr_max[0], 20);		// MB byte align.
		printk("DDR0 Alloc Range : %08x~%08x -> rebase %c%08x\n", addr_min[0], addr_max[0], ddr0_rebase<0?'-':'+', (unsigned int)abs(ddr0_rebase));
#endif
	}

#if (USE_REBASE>0)
	/*--------------------------------------------------------------
		RE-BASE from highest memory for DDR 0.
		applies for entries before DDR1 Marker ( sMemCfgHeader[1] ).
	--------------------------------------------------------------*/
	pEntry = pMemCfgEntry ? pMemCfgEntry : &sMemCfgList[0];

	while ( pEntry && pEntry->pM )
	{
		LX_MEMCFG_T	*pM = pEntry->pM;
		int			nM;

		if ( pEntry->pM == &sMemCfgHeader[1] ) break;	// ^^;; before DDR1 Marker.

		for ( nM = pEntry->nM; nM > 0; nM--,pM++) pM->base += ddr0_rebase;

		pEntry++;
	}
#endif

	BASE_MEMCFG_Sort(sMemCfgList, nEntries);

	/* save mali information to export to other ko */
	kdrv_gpu_pmem_base = sMemCfgGPU.base;
	kdrv_gpu_pmem_size = sMemCfgGPU.size;

	return 0;
}

#if MEMCFG_CHECK_TEST
void get_mem_limit(unsigned int addr, unsigned int port_num, unsigned int *start, unsigned int *end)
{
	printk("Read Control 0x%X, : ",*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40)));

	*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40) + 0x1C) = 0x1;
	*end = *(volatile unsigned int*)(addr +  + 0x20 + (port_num * 0x40) + 0x8);
	*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40) + 0x1C) = 0x2;
	*start = *(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40) + 0x8);
}

void set_mem_limit(unsigned int addr, unsigned int port_num, unsigned int start, unsigned int end)
{
	*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40)) = 0x15111000;

	*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40) + 0x1C) = 0x1;
	*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40) + 0x4) = end;
	*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40) + 0x1C) = 0x2;
	*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40) + 0x4) = start;

//	printk("Write Control 0x%X\n",*(volatile unsigned int*)(addr + 0x20 + (port_num * 0x40)));
}

static unsigned int start_v_0[5];
static unsigned int end_v_0[5];

static unsigned int start_v_1[5];
static unsigned int end_v_1[5];

static unsigned int start_v_2[5];
static unsigned int end_v_2[5];


void name_match(char* name, unsigned int start, unsigned int end)
{
	unsigned int cmp_result;

	/* ATLAS 0 */
	cmp_result = memcmp(name,"sMemCfgGPU",11);
	if (cmp_result == 0)
	{
		start_v_0[1] = start;
		end_v_0[1] = end;
		start_v_0[2] = start;
		end_v_0[2] = end;
		return;
	}

	cmp_result = memcmp(name,"gMemCfgGfx",10);
	if (cmp_result == 0)
	{
		start_v_0[3] = 0x0;
		end_v_0[3] = 0xFFFFFFFF;
		start_v_0[4] = start;
		end_v_0[4] = end;
		return;
	}


	/* ATLAS 1 */
	cmp_result = memcmp(name,"gstMemCfgADEC",13);
	if (cmp_result == 0)
	{
		if (start < start_v_1[1])
		{
			start_v_1[1] = start;
		}

		if (end > end_v_1[1])
		{
			end_v_1[1] = end;
		}
		return;
	}


	cmp_result = memcmp(name,"gMemCfgSDECGPB",14);
	if (cmp_result == 0)
	{
		if (start < start_v_1[1])
		{
			start_v_1[1] = start;
		}

		if (end > end_v_1[1])
		{
			end_v_1[1] = end;
		}
		return;
	}

	cmp_result = memcmp(name,"gMemCfgDvr",10);
	if (cmp_result == 0)
	{
		if (start < start_v_1[1])
		{
			start_v_1[1] = start;
		}

		if (end > end_v_1[1])
		{
			end_v_1[1] = end;
		}
		return;
	}


	cmp_result = memcmp(name,"_stVencMemComfigPres",20);
	if (cmp_result == 0)
	{
		start_v_1[2] = start;
		end_v_1[2] = end;

		return;
	}


	cmp_result = memcmp(name,"gMemCfgVdec",11);
	if (cmp_result == 0)
	{
		start_v_1[3] = start;
		end_v_1[3] = end;

		return;
	}


	cmp_result = memcmp(name,"gMemCfgPng",10);
	if (cmp_result == 0)
	{
		if (start < start_v_1[4])
		{
			start_v_1[4] = start;
		}

		if (end > end_v_1[4])
		{
			end_v_1[4] = end;
		}
		return;
	}

	cmp_result = memcmp(name,"g_stMemCfgMJPEG",15);
	if (cmp_result == 0)
	{
		if (start < start_v_1[4])
		{
			start_v_1[4] = start;
		}

		if (end > end_v_1[4])
		{
			end_v_1[4] = end;
		}
		return;
	}


	/* ATLAS 2 */
	cmp_result = memcmp(name,"gMemCfgDe",9);
	if (cmp_result == 0)
	{
		start_v_2[0] = start;
		end_v_2[0] = end;

		start_v_2[1] = start;
		end_v_2[1] = end;

		start_v_2[2] = start;
		end_v_2[2] = end;

		start_v_2[4] = start;
		end_v_2[4] = end;

		if (start < start_v_2[3])
		{
			start_v_2[3] = start;
		}

		if (end > end_v_2[3])
		{
			end_v_2[3] = end;
		}
		return;
	}

	if ( !memcmp(name,"gMemCfgFBDev",12) || !memcmp(name,"gMemCfgFBHdr",12) )
	{
		if (start < start_v_2[3])
		{
			start_v_2[3] = start;
		}

		if (end > end_v_2[3])
		{
			end_v_2[3] = end;
		}
		return;
	}
}

void memory_barrier_init(void)
{
	unsigned int i;

	printk ("TEST MEMORY ARRAY INIT\n");

	for (i=0; i<5; i++)
	{
		start_v_0[i] = 0xFFFFFFFF;
		end_v_0[i] = 0x0;
	}

	for (i=0; i<5; i++)
	{
		start_v_1[i] = 0xFFFFFFFF;
		end_v_1[i] = 0x0;
	}

	for (i=0; i<5; i++)
	{
		start_v_2[i] = 0xFFFFFFFF;
		end_v_2[i] = 0x0;
	}

	start_v_0[0] = 0x0;
	end_v_0[0] = 0xFFFFFFFF;

	start_v_1[0] = 0x0;
	end_v_1[0] = 0xFFFFFFFF;
}

void memory_barrier_set(void)
{

	unsigned int *io_mem_0;
	unsigned int *io_mem_1;
	unsigned int *io_mem_2;
	unsigned int start, end;
	unsigned int port;

	io_mem_0 = (unsigned int*) ioremap_nocache(0xc0000000, 4096);
	io_mem_1 = (unsigned int*) ioremap_nocache(0xc0001000, 4096);
	io_mem_2 = (unsigned int*) ioremap_nocache(0xc0002000, 4096);


	/* ATLAS 0 */
//	set_mem_limit(io_mem_0, 2, 0x50000000, 0x5FFFFFFF);
	set_mem_limit(io_mem_0, 3, 0x90000000, 0x9FFFFFFF);
	set_mem_limit(io_mem_0, 4, 0xD0000000, 0xDFFFFFFF);

	/* ATLAS 1 */
	set_mem_limit(io_mem_1, 2, 0x90000000, 0x9FFFFFFF);
	set_mem_limit(io_mem_1, 3, 0xD0000000, 0xDFFFFFFF);
	set_mem_limit(io_mem_1, 4, 0x10000000, 0x1FFFFFFF);

	printk("==================================================================\n");
	printk("IP MEMORY BARRIER\n");
	for(port=0; port < 5; port++)
	{
		get_mem_limit((unsigned int)io_mem_0, port, &start, &end);
		printk("ATLAS 0 PORT %u - MIN 0x%08X : MAX 0x%08X\n",port,start,end);
	}
	printk("\n");

	for(port=0; port < 5; port++)
	{
		get_mem_limit((unsigned int)io_mem_1, port, &start, &end);
		printk("ATLAS 1 PORT %u - MIN 0x%08X : MAX 0x%08X\n",port,start,end);
	}
	printk("\n");

	for(port=0; port < 5; port++)
	{
		get_mem_limit((unsigned int)io_mem_2, port, &start, &end);
		printk("ATLAS 2 PORT %u - MIN 0x%08X : MAX 0x%08X\n",port,start,end);
	}
	printk("\n");

	printk("==================================================================\n");

}

#endif

/**
 * Prints Memory Configuration.
 *
 * @param m				[IN] if NULL, then use printk, if not NULL, then use seq_prinf for proc interface.
 * @param pMemCfgEntry	[IN] LX_MEMCFG_ENTRY_T array which terminated by LX_MEMCFG_ENTRY_END
 * @param option		[IN] if 0, then prints brief output,if mask with 0x1, prints detailed output(sub-module's detailed memory).
 */
void BASE_MEMCFG_Print(struct seq_file *m, LX_MEMCFG_ENTRY_T *pMemCfgEntry, UINT32 option)
{
	BOOLEAN detailed = option & 1;
	LX_MEMCFG_ENTRY_T	*pEntry;

	pEntry = pMemCfgEntry ? pMemCfgEntry : sMemCfgList;

	if (m)	seq_printf(m, fmtMemCfgHeadLine);
	else	printk(       fmtMemCfgHeadLine);

	// select Module
	while ( pEntry && pEntry->pM )
	{
		LX_MEMCFG_T	*pM = pEntry->pM;
		int			nM;
		UINT32		prvEnd = 0;
		UINT32		addr_start=0xffffffff, addr_end   = 0;	// per-module min/max.
		UINT32		module_mem_size;

		// Skip Header series to print.
		if (	( pM == &sMemCfgHeader[0] )
			||	( pM == &sMemCfgHeader[1] ) // found DDR 1 Marker
			||  ( pM == &sMemCfgHeader[2] ) ) // found DDR 2 Marker
		{
			if ( (pM == &sMemCfgHeader[1] || pM == &sMemCfgHeader[2] ) && (! detailed) ) {
				if (m)	seq_printf(m,fmtMemCfgHeadLine);
				else    printk(fmtMemCfgHeadLine);
			}

			prvEnd = pM->base;
			pEntry++;

			continue;
		}

		// select allocation chunk in a module & search star/end address
		for (nM = pEntry->nM ; nM > 0; nM--,pM++) {

			if (detailed)		BASE_MEMCFG_PRINT(m,	pM, 0, prvEnd);

			if ( !pM->base )	prvEnd += pM->size;
			else				prvEnd = pM->base + pM->size;

			addr_end   = max(addr_end  , prvEnd );
			addr_start = min(addr_start, prvEnd - pM->size);
		}

		module_mem_size = addr_end - addr_start;

		if (m)
		{
			seq_printf(m, fmtEntryFormat,
							pEntry->name, addr_start, module_mem_size, addr_start, addr_end,
							_MB_(module_mem_size), _KB_(module_mem_size), //_BB_(module_mem_size),
							_MB_(addr_start     ), _KB_(addr_start     ), //_BB_(addr_start     ),
							_MB_(addr_end       ), _KB_(addr_end       )  //_BB_(addr_end       ),
						);
			if (detailed) seq_putc(m,'\n');
		}
		else
		{
			printk(   fmtEntryFormat,
						pEntry->name, addr_start, module_mem_size, addr_start, addr_end,
						_MB_(module_mem_size), _KB_(module_mem_size), //_BB_(module_mem_size),
						_MB_(addr_start     ), _KB_(addr_start     ), //_BB_(addr_start     ),
						_MB_(addr_end       ), _KB_(addr_end       )  //_BB_(addr_end       ),
					);

			if (detailed) printk("\n");
		}

		pEntry++;

	}	// end of while pEntry.


#if MEMCFG_CHECK_TEST
	memory_barrier_set();
#endif
	return;
}

/**
 * Get linux memory size which passed by mem=xx
 * @return linux memory size in bytes
 */
UINT32  BASE_MEMCFG_GetOSMemSize     (void)
{
	return kdrv_osmem_size;
}

/**
 * Get total memory configuration.
 *
 * @param stat			[OUT] stat
 */
void BASE_MEMCFG_GetStat(LX_MEMCFG_STAT_T *stat)
{
	if(!stat) return;
	memcpy(stat, &sMemCfgStat, sizeof(sMemCfgStat));
}

/**
 * Sub-module Print (for LX_MEMCFG_T)
 *
 * @param m				[IN] if NULL, use printk, if not NULL, use seq_printf.
 * @param pM			[IN] pointer to print.
 * @param addr_shift	[IN] address shift value (to support FBDEV WORD_ADDR)
 * @param prvEnd		[IN] end address of previous allocation.
 */
static void BASE_MEMCFG_PRINT(struct seq_file *m, LX_MEMCFG_T *pM, int addr_shift, UINT32 prvEnd)
{
	UINT32	size= pM->size;
	UINT32	base0= pM->base;
	UINT32	base = base0;
	UINT32	end;

	if ( !base0 && prvEnd) base = prvEnd;

	end = base + size;

	if (m) seq_printf(m, fmtEntryFormatSub,
					pM->name, base, size, base, end,
					_MB_(size), _KB_(size), //_BB_(size),
					_MB_(base), _KB_(base), //_BB_(base),
					_MB_(end ), _KB_(end )  //_BB_(end )
					);
	else	printk(		 fmtEntryFormatSub,
					pM->name, base, size, base, end,
					_MB_(size), _KB_(size), //_BB_(size),
					_MB_(base), _KB_(base), //_BB_(base),
					_MB_(end ), _KB_(end )  //_BB_(end )
					);
	return;
}

/**
 * Compare Memory Configuration Entry(used within Sort)
 * check end address first if p1 is highest end address, then return positive
 * if same end address, then check start address.
 * 	if p1 start address is lower than p2, than return positive
 * 		-> HIGHEST END address and if same end address then, BIGGER chunk returns positive.
 *
 * @return
 *  1 : p1 end > p2 end, or if same end, check p1 start > p2 start
 * -1 : p1 end < p2 end, or if same end, check p1 start < p2 start
 *  0 : p1 base == p2 base and p1 end == p2 end.
 */
static int BASE_MEMCFG_Compare( LX_MEMCFG_ENTRY_T *p1, LX_MEMCFG_ENTRY_T *p2)
{
	int			ret  = 0;
	int			nM;
	LX_MEMCFG_T	*pM;
	UINT32		start1, start2, end1= 0, end2= 0;

	if ( !p1 || !p1->pM || !p2 || !p2->pM ) return ret;

	for ( nM = p1->nM, pM=p1->pM; nM > 0; nM --, pM++)	end1 = max(end1, pM->base + pM->size);
	for ( nM = p2->nM, pM=p2->pM; nM > 0; nM --, pM++)	end2 = max(end2, pM->base + pM->size);

	start1	= p1->pM->base ;
	start2	= p2->pM->base ;

		 if ( end1 > end2 ) ret =  1;
	else if ( end1 < end2 ) ret = -1;
	else
	{
			 if ( start1 < start2 ) ret =  1;
		else if ( start1 > start2 ) ret = -1;
	}
	return ret;
}

/**
 * Sort MEMCFG_ENTRY in ascending order. (not MEMCFG_T in each modules,)
 *
 * 	HIGHEST END address and if same end address then, BIGGER chunk, moved to last.
 *
 */
static void BASE_MEMCFG_Sort(LX_MEMCFG_ENTRY_T *pMemCfgEntry, int nEntries)
{
	LX_MEMCFG_ENTRY_T	*pMC0, *pMCz;	// tmp , Last

	if (!pMemCfgEntry)
	{
		pMemCfgEntry = sMemCfgList;
		nEntries = NELEMENTS(sMemCfgList);
	}

	if (!nEntries) return;

	pMC0 = pMemCfgEntry;
	pMCz = &pMemCfgEntry[nEntries-1];

	if (!pMCz->pM) pMCz--;		// get Last && skip LX_MEMCFG_ENTRY_END marker.

	// bubble sort ^v^;;;;
	while ( pMC0 && pMC0->pM )
	{
		LX_MEMCFG_ENTRY_T	*pMC, *pMC1;

		pMC = pMC1 = pMCz;
		pMC --;
		while ( pMC1!= pMemCfgEntry )
		{
			if ( BASE_MEMCFG_Compare( pMC, pMC1) > 0) SWAP(pMC1, pMC);

			pMC1--;
			pMC--;
		}
		pMC0 ++;
	}

	return;
}

/* { proc entry for lg/bae/memcfg */
static int memcfg_proc_show(struct seq_file *m, void *v)
{
	BASE_MEMCFG_Print(m, NULL, 1);	// detailed dump
	return 0;
}

static int memcfg_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, memcfg_proc_show, NULL);
}

const struct file_operations memcfg_proc_fops = {
	.open		= memcfg_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
/* } end of proc entry for lg/memcfg */

/** @} */

