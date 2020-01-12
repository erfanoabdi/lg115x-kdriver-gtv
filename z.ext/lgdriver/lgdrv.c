/*
	SIC LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
	Copyright(c) 2010 by LG Electronics Inc.

	This program is free software; you can redistribute it and/or 
	modify it under the terms of the GNU General Public License
	version 2 as published by the Free Software Foundation.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of 
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
	GNU General Public License for more details.
*/ 


/*-----------------------------------------------------------------------------
	제어 상수
	(Control Constants)
------------------------------------------------------------------------------*/
#define DEBUG_LEVEL		0
#define MAP_PRINT_MODE	0

/*-----------------------------------------------------------------------------
    #include 파일들
    (File Inclusions)
------------------------------------------------------------------------------*/
#include "lgdrv.h"

/*-----------------------------------------------------------------------------
	상수 정의
	(Constant Definitions)
------------------------------------------------------------------------------*/
#define MMAP_FLG_ER		0
#define MMAP_FLG_AD		1
#define MMAP_FLG_PR		2
#define MMAP_FLG_SAME	0xbadc0de

/*-----------------------------------------------------------------------------
	매크로 함수 정의
	(Macro Definitions)
------------------------------------------------------------------------------*/
#ifndef remap_page_range
#define remap_page_range(vma, start, phys, size, prot)	io_remap_page_range(vma, start, phys, size, prot)
#endif

#ifndef		__initcall
#define		__initcall(x)
#endif

/*----------------------------------------------------------------------------
    형 정의
    (Type Definitions)
-----------------------------------------------------------------------------*/
typedef struct
{
	REGION_T		*pMaps;
	unsigned long	addr[2];
	int				size;
	int				aeFlag;
	int				count;
	int				index;
} MMAP_FND_ARG_T;

/*-----------------------------------------------------------------------------
	Extern 전역변수와 함수 prototype 선언
	(Extern Variables & Function Prototype Declarations)
------------------------------------------------------------------------------*/
int				lgdrv_open(struct inode *node, struct file *fp);
ssize_t			lgdrv_read(struct file *fp, char *buf, size_t cnt, loff_t *pos);
ssize_t			lgdrv_write(struct file *fp, const char *buf, size_t cnt, loff_t *pos);
int				lgdrv_ioctl(struct inode *inode, struct file * file, unsigned int cmd, unsigned long arg);
int				lgdrv_mmap(struct file *fp, struct vm_area_struct *vma);
int				lgdrv_release(struct inode *node, struct file *fp);

/*-----------------------------------------------------------------------------
	Static 변수와 함수 prototype 선언
	(Static Variables & Function Prototypes Declarations)
------------------------------------------------------------------------------*/
static int		lgdrv_init (void);
static void		lgdrv_exit (void);
static int		lgdrv_is_valid_app(pid_t _pid);
static int		lgdrv_mmap_info_update_locale(int input_pid, int update_mode, int use_lock_flag, unsigned long addr, unsigned long len, int ae_flag);
static int		lgdrv_find_matmap(REGION_T *pMaps, unsigned long beg, unsigned long end, int sz, int aeFlag, int *pCnt, int *pIdx);

/*-----------------------------------------------------------------------------
	전역 변수 정의
	(Define global variables)
------------------------------------------------------------------------------*/
struct mm_struct	*pSavedMM        = NULL;
EXPORT_SYMBOL(pSavedMM);

pid_t				mmap_tracing_pid = -1;
EXPORT_SYMBOL(mmap_tracing_pid);

MAPS_INFO_T			*pMapInfo        = NULL;
EXPORT_SYMBOL(pMapInfo);

struct file_operations lgdrv_fops =
{
	.owner   = THIS_MODULE,
	.read    = lgdrv_read,
	.write   = lgdrv_write,
	.ioctl   = lgdrv_ioctl,
	.mmap    = lgdrv_mmap,
	.open    = lgdrv_open,
	.release = lgdrv_release,
};

/*-----------------------------------------------------------------------------
	Global 함수 구현
	(Implementation of global functions)
------------------------------------------------------------------------------*/
int lgdrv_open(struct inode *node, struct file *fp)
{
	printk("[LG Driver Open Called]\n");
	if(!try_module_get(THIS_MODULE)) return -ENODEV;
	return 0;
}

ssize_t lgdrv_read(struct file *fp, char *buf, size_t cnt, loff_t *pos)
{
	#if 0
	ssize_t _cnt = sizeof(pid_t);

	if(copy_to_user(buf, &mmap_tracing_pid, _cnt))
		return -EFAULT;

	return _cnt;
	#else
	return 0;
	#endif
}

ssize_t lgdrv_write(struct file *fp, const char *buf, size_t cnt, loff_t *pos)
{
	#if 0
	ssize_t _cnt = sizeof(pid_t);

	if(copy_from_user(&mmap_tracing_pid, buf, _cnt))
		return -EFAULT;

	return _cnt;
	#else
	return 0;
	#endif
}

int lgdrv_ioctl(struct inode *inode, struct file * file, unsigned int cmd, unsigned long arg)
{
	ssize_t	size;
	int		rc = 0;
	int		_pid;

	#if 0
	if      (_IOC_DIR(cmd) & _IOC_READ)
	{
		rc = access_ok(VERIFY_READ, (void*)arg, sizeof(unsigned long));
		if(rc < 0) return -EINVAL;
	}
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
	{
		rc = access_ok(VERIFY_WRITE, (void*)arg, sizeof(unsigned long));
		if(rc < 0) return -EINVAL;
	}
	#endif
	size = _IOC_SIZE(cmd);

	switch (cmd)
	{
		case LGDRV_READ_MMAP_PID  :
			if(copy_to_user((void*)arg, &mmap_tracing_pid, size)) rc = -EFAULT;
			else                                                  rc = (int)size;
			break;
		case LGDRV_WRITE_MMAP_PID :
			if(copy_from_user(&mmap_tracing_pid, (void*)arg, size)) rc = -EFAULT;
			else                                                    rc = (int)size;
			printk("writed PID : %d\n", mmap_tracing_pid);
			break;
		case LGDRV_PASS_MMAP_ADDR :
			{
				unsigned long addr;
				if(copy_from_user(&addr, (void*)arg, size)) rc = -EFAULT;
				else                                        rc = (int)size;
				pMapInfo = (MAPS_INFO_T*)addr;

				pSavedMM = current->mm;

				printk("virt Mem:0x%08lx -> phys mem:0x%08lx\n", (unsigned long)addr, (unsigned long)pMapInfo);
				printk("pMapInfo->incCnt:0x%08x\n", pMapInfo->incCnt);
			}
			break;
		case LGDRV_MMAP_UPDATE:
			{
				if(copy_from_user(&_pid, (void*)arg, size)) rc = -EFAULT;
				else                                        rc = (int)size;

				if(mmap_tracing_pid < 0)
					return -EFAULT;
				lgdrv_mmap_update_with_ioctl(_pid, LGDRV_MMAP_UP2MEM);
			}
			break;
		case LGDRV_MMAP_UPDATE_PRNT:
			{
				if(copy_from_user(&_pid, (void*)arg, size)) rc = -EFAULT;
				else                                        rc = (int)size;

				if(mmap_tracing_pid < 0)
					return -EFAULT;
				lgdrv_mmap_update_with_ioctl(_pid, LGDRV_MMAP_UP2PRINT);
			}
			break;
		case LGDRV_READ_STKFRM    :
			{
				struct task_struct	*p;
				STK_REG_T			local;
				struct pt_regs		*r;

				if(copy_from_user(&local, (void*)arg, size)) rc = -EFAULT;
				else                                        rc = (int)size;

				if((p = find_task_by_pid_ns(local.pid, &init_pid_ns)) == NULL)
					return -EFAULT;
				if(!p->mm)
					return -EFAULT;
				//r = (struct pt_regs*)((ulong)p->stack + THREAD_SIZE -32 -sizeof(struct pt_regs));
				r = (struct pt_regs*)(task_pt_regs(p));
				#ifdef __MIPS__
				local.pc = r->cp0_epc;
				local.ra = r->regs[31];
				local.sp = r->regs[29];
				#endif
				if(copy_to_user((void*)arg, &local, size)) rc = -EFAULT;
				else                                       rc = (int)size;
			}
			break;
		default :
			printk("[LGDRV]Invalid ioctl command :%d\n", cmd);
			rc = -EFAULT;
			break;
	}

	return rc;
}

int lgdrv_mmap(struct file *fp, struct vm_area_struct *vma)
{
	return 0;
}

int lgdrv_release(struct inode *node, struct file *fp)
{
	module_put(THIS_MODULE);
	printk("LG Driver Close Called]\n");
	return 0;
}

int lgdrv_start(void)
{
	return (lgdrv_init());
}
__initcall (lgdrv_start);

void lgdrv_end(void)
{
	lgdrv_exit();
}

int lgdrv_mmap_info_update(int pid, int upMode, int lockFlag)
{
	return lgdrv_mmap_info_update_locale(pid, upMode, lockFlag, 0, 0, -1);
}

void lgdrv_mmap_update (struct task_struct *pTask, unsigned long addr, unsigned long len, int ae_flag)
{
	if(pSavedMM == NULL || pTask->mm == NULL)
		return;

	if( !lgdrv_is_valid_app(mmap_tracing_pid) )
	{
		printk("^r^[mmap call]process %d is dead!\n", mmap_tracing_pid);
		mmap_tracing_pid = -1;
		pSavedMM         = NULL;
		return;
	}
	if(pTask->mm == pSavedMM)
		lgdrv_mmap_info_update_locale(mmap_tracing_pid, LGDRV_MMAP_UP2MEM, 0/*use lock disable*/, addr, len, ae_flag);
}

void lgdrv_mmap_update_with_ioctl(int pid, int update_mode)
{
	read_lock(&tasklist_lock);
	if( !lgdrv_is_valid_app(mmap_tracing_pid) )
	{
		printk("^r^[ioctl call]process %d is dead!\n", mmap_tracing_pid);
		mmap_tracing_pid = -1;
		pSavedMM         = NULL;
	}
	else
	{
		lgdrv_mmap_info_update(pid, update_mode, 1/*use lock enable*/);
	}
	read_unlock(&tasklist_lock);
}

/*-----------------------------------------------------------------------------
	Static 함수 구현
	(Implementation of static functions)
------------------------------------------------------------------------------*/
static struct class *lgdrv_class;
static struct cdev lgdrv_dev;

static int __init lgdrv_class_init(void)
{
	lgdrv_class = class_create(THIS_MODULE, "lgdrv");
	if (IS_ERR(lgdrv_class))
		return PTR_ERR(lgdrv_class);
	return 0;
}

postcore_initcall(lgdrv_class_init);

static int lgdrv_init(void)
{
	int		res;

	printk("^Y^LG Driver Init!\n");

	cdev_init(&lgdrv_dev, &lgdrv_fops);
	if (cdev_add(&lgdrv_dev, MKDEV(LGDRV_MAJOR, 0), 0) ||
		register_chrdev_region(MKDEV(LGDRV_MAJOR, 0), 0, "/dev/lgdrv") < 0)
		panic("Coudn't register /dev/lgdrv driver\n");
	device_create(lgdrv_class, NULL, MKDEV(LGDRV_MAJOR, 0), NULL, "lgdrv");

	res = register_chrdev(LGDRV_MAJOR, LGDRV_NAME, &lgdrv_fops);

	if ( res < 0)
		return res;

	read_lock(&tasklist_lock);
	pfnMmapUpdate = lgdrv_mmap_update;
	read_unlock(&tasklist_lock);

	return 0;
}

static void lgdrv_exit(void)
{
	read_lock(&tasklist_lock);
	pfnMmapUpdate = NULL;
	read_unlock(&tasklist_lock);

	unregister_chrdev(LGDRV_MAJOR, LGDRV_NAME);

	device_destroy(lgdrv_class, MKDEV(LGDRV_MAJOR, 0));
	class_destroy(lgdrv_class);
	cdev_del(&lgdrv_dev);
	unregister_chrdev_region(MKDEV(LGDRV_MAJOR, 0), 0);
	printk("^y^Bye bye LG Driver!\n");
}

static int lgdrv_is_valid_app(pid_t _pid)
{
	if(_pid == -1)
	{
		printk("pid is -1 !\n");
		return 0;
	}

	if(find_task_by_pid_ns(_pid, &init_pid_ns) != NULL)
		return 1;
	printk("^r^pid %d is invalid!\n", _pid);
	return 0;
}
static int lgdrv_mmap_info_update_locale(int input_pid, int update_mode, int use_lock_flag, unsigned long addr, unsigned long len, int ae_flag)
{
	struct task_struct		*pTask     = NULL;
	struct mm_struct		*pMm       = NULL;
	struct vm_area_struct	*pVma      = NULL;
	#if (MAP_PRINT_MODE > 0)
	char					*pBuf      = NULL;
	#endif
	int						mmCount    = 0;
	pid_t					pid = (pid_t)input_pid;
	int						count, index;
	unsigned int			opt;

	if(pMapInfo == NULL)
	{
		printk("[LGDRV]Map Pointer is not allocated!\n");
		return -EINVAL;
	}

	/* PID value 가 Application과 맞지 않으면 return 한다. */
	if(mmap_tracing_pid < 0)
		return 0;

	pTask = find_task_by_pid_ns(pid, &init_pid_ns);

	if(pTask == NULL)
		return -EFAULT;

	if((pMm = get_task_mm(pTask)) == NULL)
		return -EFAULT;

	if(use_lock_flag)
		down_read(&pMm->mmap_sem);

	pVma     = pMm->mmap;

	if(use_lock_flag)
		up_read  (&pMm->mmap_sem);

	if(pVma == NULL)
	{
		mmput(pMm);
		return -EFAULT;
	}
	if(update_mode == LGDRV_MMAP_UP2PRINT)
	{
		#if (MAP_PRINT_MODE > 0)
		pMapInfo->mapNum = 0;
		pBuf = kmalloc(1024, GFP_KERNEL);
		printk("\nAddress           Mode Offset  dev_t   inode                     Mapping/Type \n");
		#endif
	}

	if(use_lock_flag)
		down_read(&pMm->mmap_sem);

	if((lgdrv_find_matmap(&pMapInfo->pMmapData[0], addr, addr+len, pMapInfo->mapNum, ae_flag, &count, &index)) >= 0)
	{
		if(index != 0)
		{
			int copy_sz;

			if(ae_flag == MMAP_FLG_ER)
			{
				copy_sz   = (pMapInfo->mapNum - (index + count+1))*sizeof(REGION_T);
				#if (DEBUG_LEVEL > 0)
				printk("er]addr[0x%08lx-0x%08lx]:index.%03d, count.%d(cp_sz.%d)\n", addr, addr+len, index, count, copy_sz);
				{
					int i;
					for(i = 0; i < count; i++)
					{
						printk("[%02d]0x%08x-0x%08x\n", i, pMapInfo->pMmapData[index+i].reg_beg, pMapInfo->pMmapData[index+i].reg_end);
					}
				}
				#endif
				memmove(&pMapInfo->pMmapData[index], &pMapInfo->pMmapData[index+count+1], copy_sz);
				pMapInfo->mapNum -= (count+1);
				goto _EndOfProc;
			}
			else
			{
				int mode = count;
				copy_sz = (pMapInfo->mapNum - index)*sizeof(REGION_T);
				#if (DEBUG_LEVEL > 0)
				printk("ad]addr[0x%08lx-0x%08lx]:index.%03d, cp_sz.%d\n", addr, addr+len, index, copy_sz);
				#endif

				while(pVma)
				{
					if(pVma->vm_start == addr)
					{
						#if (DEBUG_LEVEL > 1)
						printk("addr:0x%08x-vm:0x%08lx\n", addr, pVma->vm_start);
						#endif
						break;
					}
					pVma = pVma->vm_next;
				}

				if(pVma)
				{
					opt  = 0;
					opt |= ((pVma->vm_flags & VM_READ)     ? REG_OPT_READ   : 0);
		        	opt |= ((pVma->vm_flags & VM_WRITE)    ? REG_OPT_WRITE  : 0);
		        	opt |= ((pVma->vm_flags & VM_EXEC)     ? REG_OPT_EXEC   : 0);
		        	opt |= ((pVma->vm_flags & VM_MAYSHARE) ? REG_OPT_SHARED : 0);
					if(opt)
					{
						if(mode != MMAP_FLG_SAME)
						{
							memmove(&pMapInfo->pMmapData[index+1], &pMapInfo->pMmapData[index], copy_sz);
						}
						pMapInfo->pMmapData[index].reg_beg = addr;
						pMapInfo->pMmapData[index].reg_end = addr+len;
						pMapInfo->pMmapData[index].reg_opt = opt;
						pMapInfo->mapNum++;
					}
					goto _EndOfProc;
				}
			}
		}
	}

	pVma             = pMm->mmap;
	pMapInfo->mapNum = 0;

	for (mmCount = pMm->map_count; mmCount > 0; mmCount--)
	{
		opt  = 0;
		opt |= ((pVma->vm_flags & VM_READ)     ? REG_OPT_READ   : 0);
        opt |= ((pVma->vm_flags & VM_WRITE)    ? REG_OPT_WRITE  : 0);
        opt |= ((pVma->vm_flags & VM_EXEC)     ? REG_OPT_EXEC   : 0);
        opt |= ((pVma->vm_flags & VM_MAYSHARE) ? REG_OPT_SHARED : 0);

		if(opt)
		{
			pMapInfo->pMmapData[pMapInfo->mapNum].reg_beg  = pVma->vm_start;
			pMapInfo->pMmapData[pMapInfo->mapNum].reg_end  = pVma->vm_end;
			pMapInfo->pMmapData[pMapInfo->mapNum].reg_opt  = 0;

			pMapInfo->pMmapData[pMapInfo->mapNum].reg_opt  = opt;
			pMapInfo->mapNum++;
		}

		if(update_mode == LGDRV_MMAP_UP2PRINT)
		{
			#if (MAP_PRINT_MODE > 0)
			int			flags      = 0;
			struct file	*pFile     = NULL;

			pFile = pVma->vm_file;
			flags = pVma->vm_flags;

			printk("%08lx-%08lx ", pVma->vm_start, pVma->vm_end);
			printk("%c%c%c%c "   ,
					((flags & VM_READ)     ? 'r':'-'),
					((flags & VM_WRITE)    ? 'w':'-'),
					((flags & VM_EXEC)     ? 'x':'-'),
					((flags & VM_MAYSHARE) ? 's':'p'));
			printk("%08lx "      , pVma->vm_pgoff << PAGE_SHIFT);

			if(pFile)
			{
				struct inode	*pInod     = NULL;
				char			*pFullName = NULL;
				dev_t			dev        = 0;

				pInod = pFile->f_dentry->d_inode;
				dev   = pInod->i_sb->s_dev;

				printk("%02x:%02x ", MAJOR(dev), MINOR(dev));
				printk("%6lu"      , pInod->i_ino);

				memset(pBuf, 0, 1024);
				pFullName = d_path(pFile->f_dentry, pFile->f_vfsmnt, pBuf, 1024);
				printk("%s", pFullName);
			}
			else
			{
				if(pMm)
				{
					if     (pVma->vm_start <= pMm->start_brk && pVma->vm_end >= pMm->brk)
							printk("                                     [heap] ");
					else if(pVma->vm_start <= pMm->start_stack && pVma->vm_end >= pMm->brk)
							printk("                                     [stack] ");
				}
			}
			printk("\n");
			#endif
		}
		pVma = pVma->vm_next;
	}

_EndOfProc:
	if(use_lock_flag)
		up_read  (&pMm->mmap_sem);

	if(update_mode == LGDRV_MMAP_UP2PRINT)
	{
		#if (MAP_PRINT_MODE > 0)
		kfree(pBuf);
		#endif
	}
	mmput(pMm);
	return 0;
}

static int lgdrv_find_matmap(REGION_T *pMaps, unsigned long beg, unsigned long end, int sz, int aeFlag, int *pCnt, int *pIdx)
{
	int		x, l = 0, r = sz -1;

	/**
	 * 우선 debugging이 완벽히 끝날 때 까지
	 * 정상 동작 하도록 수정
	 */
	goto _ERR_RET;

	if(pCnt == NULL || pIdx == NULL)
		goto _ERR_RET;

	*pCnt = 0;
	*pIdx = 0;

	if (sz == 0)
		goto _ERR_RET;

	if (aeFlag == MMAP_FLG_PR)
		goto _ERR_RET;
	do
	{
		x = (l + r) / 2;

		if (aeFlag == MMAP_FLG_ER)
		{
			if      ( beg < pMaps[x].reg_beg )
			{
				r = x - 1;
			}
			else if ( beg > pMaps[x].reg_beg )
			{
				l = x + 1;
			}
			else
			{
				int i;

				*pIdx = x;
				for (i = x; ; i++)
				{
					if( end <= pMaps[i].reg_end )
					{
						*pCnt= i - x;
						break;
					}
				}
				return x;
			}
		}
		else
		{
			if      ( beg  < pMaps[x].reg_beg && beg > pMaps[x-1].reg_beg )
			{
				*pIdx = x;
				return x;
			}
			else if ( beg  < pMaps[x].reg_beg )
			{
				r = x - 1;
			}
			else if ( beg  > pMaps[x].reg_beg && beg < pMaps[x+1].reg_beg )
			{
				*pIdx = x+1;
				return (x+1);
			}
			else if ( beg == pMaps[x].reg_beg )
			{
				*pIdx = x;
				*pCnt = MMAP_FLG_SAME;
				return  x;
			}
			else
			{
				l = x + 1;
			}
		}

	} while ((l <= r));

_ERR_RET:
	return -1;
}
