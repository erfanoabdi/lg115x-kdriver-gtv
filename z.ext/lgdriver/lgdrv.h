#ifndef __LGDRV_H__
#define __LGDRV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include	<linux/autoconf.h>
#include	<linux/init.h>
#include	<linux/module.h>
#include	<linux/kernel.h>
#include	<linux/fs.h>
#include	<linux/proc_fs.h>
#include	<linux/vermagic.h>
#include	<linux/fcntl.h>
#include	<linux/seq_file.h>
#include	<linux/sched.h>
#include	<linux/mm.h>
#include	<linux/errno.h>
#include	<linux/types.h>
#include	<linux/pid.h>
#include	<linux/device.h>
#include	<linux/cdev.h>
#include	<asm/processor.h>
#include	<asm/io.h>
#include	<asm/ioctl.h>
#include	<asm/uaccess.h>
#include	<asm/thread_info.h>

/*-----------------------------------------------------------------------------
	상수 정의
	(Constant Definitions)
------------------------------------------------------------------------------*/
#define LGDRV_MAJOR					31
#define LGDRV_NAME					"lgdriver"

#define LGDRV_MMAP_UP2MEM			0
#define LGDRV_MMAP_UP2PRINT			1

typedef void (*pFN_MMAP_FUNC_T)(struct task_struct *, unsigned long, unsigned long, int);

typedef struct
{
	unsigned int	reg_beg;
	unsigned int	reg_end;
	unsigned int	reg_opt;
} REGION_T;

typedef struct _maps_info_
{
	int			incCnt;
	int			mapNum;
	REGION_T	pMmapData[1];
} MAPS_INFO_T;

typedef struct _stk_regs_
{
	pid_t        pid;
	unsigned int pc;
	unsigned int ra;
	unsigned int sp;
} STK_REG_T;

/*-----------------------------------------------------------------------------
	매크로 함수 정의
	(Macro Definitions)
------------------------------------------------------------------------------*/
/* LG Driver IOCT command set */
#define LG_DRV_IOCTL_NUM			31

#define LGDRV_PASS_MMAP_ADDR		_IOW (LG_DRV_IOCTL_NUM, 0, unsigned long )
#define LGDRV_READ_MMAP_PID			_IOR (LG_DRV_IOCTL_NUM, 1, pid_t         )
#define LGDRV_WRITE_MMAP_PID		_IOW (LG_DRV_IOCTL_NUM, 2, pid_t         )
#define LGDRV_MMAP_UPDATE_PRNT		_IOW (LG_DRV_IOCTL_NUM, 3, pid_t         )
#define LGDRV_MMAP_UPDATE			_IOW (LG_DRV_IOCTL_NUM, 4, pid_t         )
#define LGDRV_READ_STKFRM			_IOWR(LG_DRV_IOCTL_NUM, 5, STK_REG_T     )

#define REG_OPT_READ	0x01
#define	REG_OPT_WRITE	0x02
#define	REG_OPT_EXEC	0x04
#define REG_OPT_SHARED	0x08

#define	REG_OPT_TEXT	(REG_OPT_READ|              REG_OPT_EXEC)
#define	REG_OPT_ALL		(REG_OPT_READ|REG_OPT_WRITE|REG_OPT_EXEC)

/*-----------------------------------------------------------------------------
	Extern 변수 및 함수 선언
	(Declare external global variables and functions)
------------------------------------------------------------------------------*/
extern pFN_MMAP_FUNC_T		pfnMmapUpdate;

extern struct mm_struct		*pSavedMM;
extern struct				file_operations lgdrv_fops;
extern pid_t				mmap_tracing_pid;
extern MAPS_INFO_T			*pMapInfo;


extern int					lgdrv_start					(void);
extern void					lgdrv_end					(void);
extern int					lgdrv_mmap_info_update		(int pid, int update_mode, int use_lock_flag);
extern void					lgdrv_mmap_update_with_ioctl(int pid, int update_mode);
#ifdef __cplusplus
}
#endif

#endif/*__LGDRV_H__*/
