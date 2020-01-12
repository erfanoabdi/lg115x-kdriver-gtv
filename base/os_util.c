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
 *  OS(Linux) kernal API wrapper
 *
 *  author      daeyoung lim ( raxis.lim@lge.com )
 *  @version	1.0
 *  @date		2009.11.18
 *
 *  @addtogroup lg1150_base
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/
#undef SUPPORT_CACHE_MAP

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/hardirq.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/version.h>

#include "tlsf.h"
#include "debug_util.h"
#include "os_util.h"
#include "debug_util.h"
#include "base_dev_cfg.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
/* if you want to see some debug message from os_util, define it */
#undef	KDRV_OS_DEBUG

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
static struct task_struct*	g_csOwner = NULL;
static struct class* kdrv_class = NULL;

#define SET_CRITICAL() 		do { g_csOwner = current; } while (0)
#define CLEAR_CRITICAL() 	do { g_csOwner = NULL; } while (0)
#define CHECK_CRITICAL() 	( g_csOwner == current || in_interrupt() )

#define ASSERT_CRITICAL() do \
{\
    if ( !CHECK_CRITICAL() )\
    {\
        DBG_PRINT_ERROR("Error, must be in critical section to call %s\n", __FUNCTION__);\
        BKNI_Fail();\
    }\
} while (0)

#define ASSERT_NOT_CRITICAL() do \
{\
    if ( CHECK_CRITICAL() )\
    {\
        DBG_PRINT_ERROR("Error, must not be in critical section to call %s\n", __FUNCTION__);\
        OS_FAILURE();\
    }\
} while (0)

/*
 * critical resource lock/unlock
 */
//#define	OS_GLOBAL_LOCK()		do { if( CHECK_CRITICAL() )	spin_lock_irqsave(&_g_res_lock, flags);	 } while(0)
//#define OS_GLOBAL_UNLOCK()		do { if( CHECK_CRITICAL() )	spin_unlock_irqrestore(&_g_res_lock, flags); } while(0)

#define	OS_GLOBAL_LOCK()			do { spin_lock_irqsave(&_g_res_lock, flags);	  } while(0)
#define OS_GLOBAL_UNLOCK()			do { spin_unlock_irqrestore(&_g_res_lock, flags); } while(0)

#define	MSEC2TICKS(msec)			( ( (msec) * HZ) / 1000 )

/*
 * OS internal debug macroes
*/
#define	OS_PRINT(format, args...)		DBG_PRINT(0,DEBUG_IDX_OSUTIL_PRINT,format, ##args)
#define OS_TRACE(format, args...)		DBG_PRINTX(0,DEBUG_IDX_OSUTIL_PRINT,format, ##args)

/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/

/**
 * spinlock for resource protection
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
static	spinlock_t		_g_res_lock = SPIN_LOCK_UNLOCKED;
#else
static DEFINE_SPINLOCK(_g_res_lock);
#endif

/*========================================================================================
	Implementation Group
========================================================================================*/
UINT32   lx_chip_rev	( void )
{
	return g_base_dev_cfg.chip_rev;
}
EXPORT_SYMBOL(lx_chip_rev);


UINT32	lx_ace_rev		( void )
{
	return g_base_dev_cfg.ace_rev;
}

UINT32	lx_chip_plt	( void )
{
	return g_base_dev_cfg.chip_plt;
}
EXPORT_SYMBOL(lx_chip_plt);

/**
 *	Initialize OS core
 *
 */
void	OS_CORE_Init	( void )
{
	/* create device class */
	kdrv_class = class_create(THIS_MODULE, "lg" );
}

/**
 *	cleanup OS core
 *
 */
void	OS_CORE_Cleanup	( void )
{
	/* destroy device class */
}

/**
 *	Initialize OS debug
 *
 */
void	OS_CORE_InitDebug	( void )
{
	/* enable OS debug */
#ifdef	KDRV_OS_DEBUG
    OS_DEBUG_EnableModuleByIndex( g_global_debug_fd, DEBUG_IDX_OSUTIL_PRINT, DBG_COLOR_GREEN );
#endif
}

/**
 *	system failure handler
 *
 *	When system failure or system crash is detected by system, this function is called.
 *
 *	TODO : more analysis & implementation needed
 */

void	OS_FAILURE_Tag	( const char* szFile, const char* szFunc, const int nLine )
{
	OS_DEBUG_PrintError_Tag( DBG_LEVEL_ERR, szFile, szFunc, nLine, "OS_FAILURE Issued...\n");
}

/*========================================================================================
	DEVICE REGISTER
========================================================================================*/
void     OS_CreateDeviceClass    ( dev_t dev, const char* fmt, ... )
{
	va_list args;
	char dev_name[LX_MAX_DEVICE_NAME];

	va_start(args, fmt);
	vsnprintf( dev_name, LX_MAX_DEVICE_NAME, fmt, args );	
	va_end(args);

	device_create( kdrv_class, NULL, dev, NULL, dev_name );
}
EXPORT_SYMBOL(OS_CreateDeviceClass);

void     OS_DestroyDeviceClass   ( dev_t dev )
{
	device_destroy( kdrv_class, dev );
}

/*========================================================================================
	MUTEX OS API IMPLEMENTATION
========================================================================================*/
void	OS_InitMutex_Tag	( OS_SEM_T* pSem , UINT32 attr )
{
	PARAM_UNUSED(attr);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
	init_MUTEX(&(pSem->sem));
#else
	sema_init(&(pSem->sem),1);
#endif
	init_waitqueue_head(&(pSem->wq));
}

int		OS_LockMutex_Tag	( OS_SEM_T* pSem, UINT32 timeout,
								const char* szFile, const char* szFunc, const int nLine )
{
	ULONG	flags;
	int		ret = RET_OK;

	/* interrupt handler should not lock mutex with wait option */
	if ( timeout !=0 && CHECK_CRITICAL() )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"Ooops. OS_LockMutex called at ISR. force timeout from %d to 0.\n", timeout );

		timeout = 0;
	}

	/* normally, ISR should call OS_LockMutex with zero timeout */
	if ( timeout == 0x0 )
	{
		ret = down_trylock(&(pSem->sem));

		if ( RET_OK != ret )
		{
			OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
				"OS_LockMutex timeout. ret=%d\n", ret );
		}
	}
	/* normal process seems to call OS_LockMutex with timoeut option */
	else if ( timeout == OS_INFINITE_WAIT )
	{
		ret = down_interruptible(&(pSem->sem));

		if ( RET_OK != ret )
		{
			OS_DEBUG_PrintError_Tag( DBG_LEVEL_ERR, szFile, szFunc, nLine,
				"OS_LockMutex failed. ret=%d\n", ret );
		}
	}
	else
	/* if timeout option is enabled, */
	{
		ULONG			ticks;
		wait_queue_t	wqe;

		ticks = MSEC2TICKS(timeout);
		pSem->wait_loop = TRUE;

		/* make wait_queue */
		init_waitqueue_entry(&wqe, current);
		add_wait_queue(&(pSem->wq), &wqe);

		for ( ; ; )
		{
			/* normally, current task goes to sleep before checking condition.
			 * when event is sent by another task or ISR, current task will escape from schedule_timeout().
			 * this is what I want.
			 */
			set_current_state( TASK_INTERRUPTIBLE );

			ret = down_trylock(&(pSem->sem));

			/* good, I've got semaphore */
			if ( RET_OK == ret )
			{
				ret = RET_OK; break;
			}
			/* event not yet arrived, check timeout */
			else if (!ticks)
			{
				OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
					"OS_LockMutex timeout. ret=%d\n", ret );
				ret = RET_TIMEOUT; break;
			}
			else
			{
				ticks = schedule_timeout(ticks);

				OS_PRINT("OS_LockMutex : timeout interrupt (ticks:%d)\n", (int)ticks );

				if ( signal_pending(current) )
				{
					OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
						"OS_LockMutex signal interrrupt.\n" );
					ret = RET_INTR_CALL; break;
				}
			}
		}

	   	set_current_state(TASK_RUNNING);
	    remove_wait_queue(&(pSem->wq), &wqe);
	}

	if ( RET_OK == ret )
	{
		OS_GLOBAL_LOCK();
		pSem->wait_loop = FALSE;
		OS_GLOBAL_UNLOCK();

		OS_PRINT("OS_LockMutex   (%p): done (tm:%d)\n", pSem, timeout);
	}

	return ret;
}

/**
 * unlock all processes blocked by mutex
 *
 * @param pSem [IN] pointer to semaphore
 */
void	OS_UnlockMutex_Tag ( OS_SEM_T* pSem,
								const char* szFile, const char* szFunc, const int nLine )
{
	ULONG	flags;
	BOOLEAN	is_wait_loop = FALSE;

	OS_GLOBAL_LOCK();
	is_wait_loop 	= pSem->wait_loop;
	pSem->wait_loop = FALSE;
	OS_GLOBAL_UNLOCK();

	/* unlock mutex first and wake up the sleeping task */
	OS_PRINT("OS_UnlockMutex (%p): done (loop:%d)\n", pSem, is_wait_loop);
	up(&(pSem->sem));
	if(is_wait_loop) wake_up_interruptible(&(pSem->wq));
}

/*========================================================================================
	EVENT OS API IMPLEMENTATION
========================================================================================*/
/*
 * init event structure
 */
void	OS_InitEvent_Tag		( OS_EVENT_T* pEvnt )
{
	/* initialize event structure */
	pEvnt->ev = 0x0;
	init_waitqueue_head(&(pEvnt->wq));
}

/*
 * fetch event
*/
static BOOLEAN	_OS_FetchEvent	( OS_EVENT_T* pEvnt, UINT32 ev, UINT32* rEv, UINT32 option )
{
	ULONG	flags;
	BOOLEAN	ret;

	OS_GLOBAL_LOCK();

	if ( pEvnt->ev )
	{
		/* check if any required event is received */
		if ( ( option == OS_EVENT_RECEIVE_ANY ) && ( pEvnt->ev & ev) )
		{
			*rEv = ( pEvnt->ev & ev );
			pEvnt->ev &= ~( pEvnt->ev & ev );
			ret = TRUE;
		}
		/* check if all required event is received */
		else if ( ( option == OS_EVENT_RECEIVE_ALL ) && ( ( pEvnt->ev & ev ) == ev ) )
		{
			*rEv = ev;
			pEvnt->ev &= ~ev;
			ret = TRUE;
		}
		/* event is found but requested event not found, return RET_TIMEOUT (timeout error) */
		else
		{
			ret = FALSE;
		}
	}
	/* event is not found, return RET_TIMEOUT (timeout error) */
	else
	{
		ret = FALSE;
	}

	OS_GLOBAL_UNLOCK();

	return ret;
}

/**
 * receive event structure
 */
int      OS_RecvEvent_Tag		( OS_EVENT_T* pEvnt, UINT32 ev, UINT32* rEv, UINT32 option, UINT32 timeout,
									const char* szFile, const char* szFunc, const int nLine )
{
	int				ret;
	int				rc;

	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	*rEv = 0x0;
	ret	 = 0;

	/* if ISR handler, timeout should be 0 */
	if ( timeout )
	{
		ASSERT_NOT_CRITICAL();
	}
	else if ( CHECK_CRITICAL() )
	{
		BOOLEAN rc = _OS_FetchEvent( pEvnt, ev, rEv, option );

		return (rc)? RET_OK: RET_TIMEOUT;
	}

	if ( signal_pending(current) )
	{
		return RET_INTR_CALL;
	}

	if (timeout<10)	timeout = 5;

	rc = wait_event_interruptible_timeout( pEvnt->wq, _OS_FetchEvent(pEvnt,ev,rEv,option), msecs_to_jiffies(timeout) );

//	if ( rc>0 )	printk("^g^[evnt] ok %d/%d(%d) (%d)\n", rc, msecs_to_jiffies(timeout), timeout, msecs_to_jiffies(timeout) - rc );
//	else 		printk("^g^[evnt] Event timeout..\n");
	if ( rc<=0 ) { OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine, "Event timeout. req %d ms\n", timeout ); }

	return (rc>0)? RET_OK: RET_TIMEOUT;
}

/*
 * send event
 */
void      OS_SendEvent_Tag		( OS_EVENT_T* pEvnt, UINT32 ev,
									const char* szFile, const char* szFunc, const int nLine )
{
	ULONG	flags;

	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	OS_GLOBAL_LOCK();
	pEvnt->ev |= ev;
	OS_GLOBAL_UNLOCK();

    wake_up_interruptible(&pEvnt->wq);
}

/**
 * clear event
 */
void	OS_ClearEvent_Tag		( OS_EVENT_T* pEvnt, const char* szFile, const char* szFunc, const int nLine )
{
	ULONG	flags;

	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	OS_GLOBAL_LOCK();
	pEvnt->ev = 0x0;
	OS_GLOBAL_UNLOCK();
}

/*========================================================================================
	TIME/TICK OS API IMPLEMENTATION
========================================================================================*/
int		OS_MsecSleep_Tag ( UINT32 msec, const char* szFile, const char* szFunc, const int nLine )
{
	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);
	msleep(msec);

	return RET_OK;
}

void	OS_NsecDelay_Tag 	( UINT32 nsec, const char* szFile, const char* szFunc, const int nLine )
{
	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	ndelay ( nsec );
}

void	OS_UsecDelay_Tag 	( UINT32 usec, const char* szFile, const char* szFunc, const int nLine )
{
	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	udelay ( usec );
}

/**
 * get mili seconds
 *
 */
UINT64	OS_GetMsecTicks_Tag ( void )
{
    struct timespec ts;
    ts = current_kernel_time();

    return (UINT64)(ts.tv_sec * 1000 + ts.tv_nsec/1000000);
}

/**
 * get micro seconds
 *
 * TOOD) get the real micro seconds
 */
UINT64	OS_GetUsecTicks_Tag ( void )
{
    struct timespec ts;
    ts = current_kernel_time();

    return (UINT64)(ts.tv_sec * 1000000 + ts.tv_nsec/1000);
}

/**
 * get current (sec, msec, usec) information
 *
 *
 */
void	OS_GetCurrentTicks_Tag ( UINT32* pSec, UINT32* pMSec, UINT32* pUSec )
{
    struct timespec ts;
    ts = current_kernel_time();

    if ( pSec )	*pSec = ts.tv_sec;
    if ( pMSec) *pMSec= ts.tv_nsec/1000000;
    if ( pUSec) *pUSec= ts.tv_nsec/1000;
}

static void	_OS_TimerCallback		( ULONG param )
{
	OS_TIMER_T*	pTimer = (OS_TIMER_T*)param;

//	DBG_TRACE(" info (object:%p, callback:%p, option:%x, ticks:%4d )\n", pTimer, pTimer->callback, pTimer->option, pTimer->ticks );

	if ( pTimer->callback )
	{
		pTimer->callback ( pTimer->data );
	}

	/* if current timer is stopped or destroyed during timer callback, we should not do anything */
	if ( pTimer->callback && ( pTimer->option & OS_TIMER_TIMETICK ) )
	{
		mod_timer( &pTimer->tmr, get_jiffies_64() + pTimer->ticks );
	}
}


/**
 * get current timer ticks(174Mhz)
 *
 */
UINT32	OS_GetHighResTicks_Tag( void )
{
	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
	{
		/* [TODO-L9] implement high res tick for L9 */
		return 0;
	}
	else
	{
		#define L8_REFCOUNTER		(L8_TIMER0_1_BASE + 0x24)
		#define L8_CLOCK_SHIFT		20
		#define NSEC_PER_CLK_MULT(clock, shift) \
			(UINT32)((((UINT64)NSEC_PER_SEC << (shift)) + (clock)/2) / (clock))
		#define L8_CLK_MULT			NSEC_PER_CLK_MULT(CLOCK_TICK_RATE, L8_CLOCK_SHIFT)

		return *(volatile UINT32*)L8_REFCOUNTER;
	}
}

/**
 * get nano seconds from oldTick time to current.
 *
 */
UINT64	OS_GetElapsedHighResTime_Tag( UINT32 oldTick )
{
	UINT64 ns;
	UINT32 value;

	if (lx_chip_rev( ) >= LX_CHIP_REV(L9,A0) )
	{
		/* [TODO-L9] implement high res tick for L9 */
		return 0;
	}
	else
	{

		/* timer tick is decrement coutner */
		value = (UINT32)(oldTick - *(volatile UINT32*)L8_REFCOUNTER);
		ns = ((UINT64)value * L8_CLK_MULT) >> L8_CLOCK_SHIFT;
	}

	return ns;
}


/*========================================================================================
	TIMER OS API IMPLEMENTATION
========================================================================================*/
void		OS_InitTimer_Tag		( OS_TIMER_T* pTimer, const char* szFile, const char* szFunc, const int nLine  )
{
	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	memset( pTimer, 0x0, sizeof(OS_TIMER_T));

    init_timer(&pTimer->tmr);
}

void		OS_StartTimer_Tag		( OS_TIMER_T* pTimer, OS_TIMER_CALLBACK_T timer_callback, UINT32 option, UINT32 timeout, UINT32 data, const char* szFile, const char* szFunc, const int nLine  )
{
	ULONG	ticks;
	ULONG	flags;

	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

//	DBG_TRACE("timer - msec = %d, tick = %d, HZ = %d\n", timeout, ticks, HZ );

	ticks = MSEC2TICKS(timeout);
	if ( ticks == 0 )	ticks = 1;


	/* fill the basic information.
	 * remember that timer_callback and data is stored to pTimer object, its pointer is passed to
	 * add_timer() function.
	 */
	OS_GLOBAL_LOCK();
	pTimer->ticks 	= ticks;
	pTimer->data	= data;
	pTimer->option	= option;
	pTimer->callback= timer_callback;

    pTimer->tmr.function= _OS_TimerCallback;
    pTimer->tmr.data 	= (ULONG)pTimer;
	pTimer->tmr.expires = get_jiffies_64() + pTimer->ticks;
	OS_GLOBAL_UNLOCK();

	mod_timer(&pTimer->tmr, get_jiffies_64()+pTimer->ticks);
}

void		OS_StopTimer_Tag		( OS_TIMER_T* pTimer, const char* szFile, const char* szFunc, const int nLine  )
{
	ULONG	flags;

	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	/* clear some critical field to stop timer if running */
	OS_GLOBAL_LOCK();
	pTimer->option	= 0x0;
	pTimer->callback= 0x0;
	OS_GLOBAL_UNLOCK();

	del_timer_sync (&pTimer->tmr);
}

/*========================================================================================
	FILE OS API IMPLEMENTATION
========================================================================================*/

int      OS_OpenFile_Tag         ( OS_FILE_T* pFile, char* fname, int flags, mode_t mode,
                                            const char* szFile, const char* szFunc, const int nLine )
{
	pFile->fd = filp_open(fname, flags, mode );

	if ( IS_ERR(pFile->fd) )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"Can't open file - name %s, flags %x, mode %x\n", fname, flags, mode );

		pFile->fd = NULL;
		return RET_ERROR;
	}

	return RET_OK;
}

int      OS_SeekFile_Tag         ( OS_FILE_T* pFile, off_t offset, int whence,
                                            const char* szFile, const char* szFunc, const int nLine )
{
	off_t offval;
	mm_segment_t old_fs;

	if ( !pFile->fd || !pFile->fd->f_op || !pFile->fd->f_op->llseek )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"File is NULL or operation is NULL\n" );
		return RET_ERROR;
	}

	old_fs = get_fs();
	set_fs(get_ds());
	offval = pFile->fd->f_op->llseek( pFile->fd, offset, whence );
	set_fs(old_fs);

	return offval;
}

int      OS_SizeOfFile_Tag       ( OS_FILE_T* pFile,
                                            const char* szFile, const char* szFunc, const int nLine )
{
	off_t file_size;
	off_t old_offset;
	mm_segment_t old_fs;

	if ( !pFile->fd || !pFile->fd->f_op || !pFile->fd->f_op->llseek )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"File is NULL or operation is NULL\n" );
		return RET_ERROR;
	}

	old_fs = get_fs();
	set_fs(get_ds());

	old_offset  = pFile->fd->f_op->llseek( pFile->fd, 0L, SEEK_CUR );
	file_size   = pFile->fd->f_op->llseek( pFile->fd, 0L, SEEK_END );
				  pFile->fd->f_op->llseek( pFile->fd, old_offset, SEEK_SET );
	set_fs(old_fs);

	return file_size;
}

int      OS_WriteFile_Tag        ( OS_FILE_T* pFile, char* buffer, int count,
                                            const char* szFile, const char* szFunc, const int nLine )
{
	int tx;
	mm_segment_t old_fs;

	if ( !pFile->fd || !pFile->fd->f_op || !pFile->fd->f_op->write )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"File is NULL or operation is NULL\n" );
		return RET_ERROR;
	}

	old_fs = get_fs();
	set_fs(get_ds());
	tx = pFile->fd->f_op->write( pFile->fd, buffer, count, &pFile->fd->f_pos );
	set_fs(old_fs);

	return tx;
}

int      OS_ReadFile_Tag         ( OS_FILE_T* pFile, char* buffer, int count,
                                            const char* szFile, const char* szFunc, const int nLine )
{
	int rx;
	mm_segment_t old_fs;

	if ( !pFile->fd || !pFile->fd->f_op || !pFile->fd->f_op->read )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"File is NULL or operation is NULL\n" );
		return RET_ERROR;
	}

	old_fs = get_fs();
	set_fs(get_ds());
	rx = pFile->fd->f_op->read( pFile->fd, buffer, count, &pFile->fd->f_pos );
	set_fs(old_fs);

	return rx;
}

int      OS_CloseFile_Tag        ( OS_FILE_T* pFile,
                                            const char* szFile, const char* szFunc, const int nLine )
{
	if ( !pFile->fd )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"File is NULL\n" );
		return RET_ERROR;
	}

	filp_close( pFile->fd, NULL );

	return RET_OK;
}

/*========================================================================================
	MEMORY OS API IMPLEMENTATION
========================================================================================*/
void*	OS_KMalloc_Tag		( size_t size, const char* szFile, const char* szFunc, const int nLine )
{
	void* ptr = NULL;
	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	ASSERT_NOT_CRITICAL();

    if ( (ptr = kmalloc(size, GFP_KERNEL)) ) memset( ptr, 0x0, size );

	return ptr;
}

void	OS_KFree_Tag		( void* ptr, const char* szFile, const char* szFunc, const int nLine )
{
	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	if (ptr)  kfree(ptr);
}

typedef struct
{
	void*	addr;
	size_t	size;
}
PHYS_MEM_ALLOC_INFO_T;

#define	TLSF_HDR_SIZE			3216
#define	VIRT_BLK_SIZE			8
#define DEFAULT_PHYS_BLK_SIZE	(2<<11) /* 4096 = PAGE SIZE */
#define	MEM_CONV_P2V(pRgn,val) 	((((val)+pRgn->block_size-1)/pRgn->block_size) * VIRT_BLK_SIZE)
#define	MEM_CONV_V2P(pRgn,val)	(((val)/VIRT_BLK_SIZE) * pRgn->block_size)

int      OS_InitRegion_Tag		( OS_RGN_T* pRgn, const void* phys_addr, const int size,
									const char* szFile, const char* szFunc, const int nLine )
{
	return OS_InitRegionEx_Tag ( pRgn, phys_addr, DEFAULT_PHYS_BLK_SIZE, size/DEFAULT_PHYS_BLK_SIZE,
									szFile, szFunc, nLine );
}

int      OS_InitRegionEx_Tag	( OS_RGN_T* pRgn, const void* phys_addr, const int block_size, const int block_num,
									const char* szFile, const char* szFunc, const int nLine )
{
	void*	ptr;
	int		mem_pool_size = (TLSF_HDR_SIZE*2) + VIRT_BLK_SIZE*block_num;

//	OS_GLOBAL_LOCK();  <-- disable spinlock since it is called at the startup 

	memset( pRgn, 0x0, sizeof(OS_RGN_T) );

	pRgn->mem_pool_size	= mem_pool_size;
	pRgn->mem_manager	= vmalloc( pRgn->mem_pool_size );

	OS_PRINT("mem_manager create with %d bytes\n", pRgn->mem_pool_size );

	if ( !pRgn->mem_manager )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine, "Ooops. Out of kernel memory. Can't allocate %d byte for mem managern", mem_pool_size);
		memset( pRgn, 0x0, sizeof(OS_RGN_T) );
		//OS_GLOBAL_UNLOCK();
		return RET_ERROR;
	}

	pRgn->block_size	= block_size;
	pRgn->block_num		= block_num;
	pRgn->phys_mem_addr = (void*)phys_addr;

	pRgn->mem_alloc_cnt	= 0;
	pRgn->mem_alloc_size= 0;

	init_memory_pool( mem_pool_size, pRgn->mem_manager );

	/* get the base offset for virtual memory space */
	ptr = malloc_ex(VIRT_BLK_SIZE, pRgn->mem_manager );
	pRgn->virt_mem_addr	= ptr;
	free_ex ( ptr, pRgn->mem_manager );

//	OS_GLOBAL_UNLOCK();

	return RET_OK;
}

int      OS_CleanupRegion_Tag	( OS_RGN_T* pRgn,
									const char* szFile, const char* szFunc, const int nLine )
{
	ULONG flags;
	int	ret = RET_OK;

	OS_GLOBAL_LOCK();

	/* If some memory is not freed, report that condition */
	if ( pRgn->mem_alloc_cnt )
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"Ooops. Error detected during OS_CleanupRegion. %d memory(s) is(are) not freed\n", pRgn->mem_alloc_cnt );
	}

	destroy_memory_pool ( pRgn->mem_manager );
	vfree( pRgn->mem_manager );
	memset( pRgn, 0x0, sizeof(OS_RGN_T));

	OS_GLOBAL_UNLOCK();

	return ret;
}

void*    OS_MallocRegion_Tag	( OS_RGN_T* pRgn, size_t size,
									const char* szFile, const char* szFunc, const int nLine )
{
	ULONG	flags;
	size_t	virt_size;
	UINT32	virt_offset;
	UINT32	phys_offset;
	void*	virt_memptr;
	UINT32	phys_memptr = 0x0;

	OS_GLOBAL_LOCK();

	/* convert physical size to virtual size */
	virt_size	= MEM_CONV_P2V(pRgn,size);
	size		= MEM_CONV_V2P(pRgn,virt_size);

	/* get the virtual memory from memory manager, calculate virtual offset and convert it to physical offset */
	virt_memptr = malloc_ex( virt_size, pRgn->mem_manager );

	if ( virt_memptr )
	{
		PHYS_MEM_ALLOC_INFO_T	mem_info;

		virt_offset = (UINT32)virt_memptr - (UINT32)pRgn->virt_mem_addr;
		phys_offset = MEM_CONV_V2P(pRgn,virt_offset);
		phys_memptr = (UINT32)pRgn->phys_mem_addr + phys_offset;

		/* record allocation history */
		pRgn->mem_alloc_size += size;
		pRgn->mem_alloc_cnt++;

		/* record physical address information. this data is used to confirm physical address when FREE request */
		mem_info.addr = (void*)phys_memptr;
		mem_info.size = size;
		memcpy( (void*)virt_memptr, &mem_info, sizeof(PHYS_MEM_ALLOC_INFO_T));

		/* clear physical memory region */
#if 0
		{
			void* map_ptr = ioremap( (ulong)phys_memptr, size );
			memset( map_ptr, 0x0, size );
			iounmap( map_ptr );
		}
#endif
		OS_PRINT("%s : virt_memptr %p, virt_size %d, virt_offset 0x%x, phys_offset 0x%x\n", __F__, (UINT32*)virt_memptr, virt_size, virt_offset, phys_offset  );
	}
	else
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine, "Ooops. OS_MallocRegion failed. Can't allocate %d byte\n", size );
		phys_memptr = 0x0;
	}

	OS_GLOBAL_UNLOCK();

	return (void*)phys_memptr;
}

int     OS_FreeRegion_Tag	( OS_RGN_T* pRgn, void* phys_addr,
									const char* szFile, const char* szFunc, const int nLine )
{
	ULONG	flags;
	UINT32	virt_offset;
	UINT32	virt_memptr;
	UINT32	phys_offset;
	UINT32	phys_memptr = (UINT32)phys_addr;

	int		ret = RET_OK;

	if ( phys_memptr )
	{
		PHYS_MEM_ALLOC_INFO_T	mem_info;

		OS_GLOBAL_LOCK();

		/* calculate offset */
		phys_offset = phys_memptr - (UINT32)pRgn->phys_mem_addr;
		virt_offset = MEM_CONV_P2V(pRgn,phys_offset);

		OS_PRINT("%s : virt_offset %d, phys_offset %d\n", __F__, virt_offset, phys_offset );

		virt_memptr = (UINT32)pRgn->virt_mem_addr + virt_offset;

		/* check if physical address is the same as the recorded one */
		memcpy( &mem_info, (void*)virt_memptr, sizeof(PHYS_MEM_ALLOC_INFO_T));

		OS_PRINT("%s : virt_memptr %p, phys_addr %p, size %d\n", __F__, (UINT32*)virt_memptr, (UINT32*)mem_info.addr, mem_info.size );
		OS_PRINT("%s : virt_offset 0x%x, phys_offset 0x%x\n", __F__, virt_offset, phys_offset  );

		if ( mem_info.addr == phys_addr )
		{
			pRgn->mem_alloc_cnt--;
			pRgn->mem_alloc_size -= mem_info.size;

			free_ex ( (void*)virt_memptr, pRgn->mem_manager );
		}
		else
		{
			OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
				"Ooops. OS_FreeRegion failed. Invalid phys address %p. Actual address seems %p.\n", phys_addr, mem_info.addr );
			ret = RET_ERROR;
		}

		OS_GLOBAL_UNLOCK();
	}
	else
	{
		OS_DEBUG_PrintError_Tag( DBG_LEVEL_WRN, szFile, szFunc, nLine,
			"Ooops. OS_FreeRegion failed. Null pointer\n");
		ret = RET_ERROR;
	}

	return ret;
}

int     OS_GetRegionInfo_Tag	( OS_RGN_T* pRgn, OS_RGN_INFO_T* pMemInfo,
									const char* szFile, const char* szFunc, const int nLine )
{
	ULONG	flags;
	PARAM_UNUSED(szFile);
	PARAM_UNUSED(szFunc);
	PARAM_UNUSED(nLine);

	OS_GLOBAL_LOCK();	

	pMemInfo->phys_mem_addr = pRgn->phys_mem_addr;
	pMemInfo->block_size	= pRgn->block_size;
	pMemInfo->length		= pRgn->block_size * pRgn->block_num;

	pMemInfo->mem_alloc_cnt	= pRgn->mem_alloc_cnt;
	pMemInfo->mem_alloc_size= pRgn->mem_alloc_size;
	pMemInfo->mem_free_size = pMemInfo->length - pMemInfo->mem_alloc_size - pRgn->block_size * pRgn->mem_alloc_cnt;

	OS_GLOBAL_UNLOCK();

	return RET_OK;
}

/*========================================================================================
	IOMAP OS API IMPLEMENTATION
========================================================================================*/
int      OS_OpenCacheMap_Tag            ( OS_CACHE_MAP_T* pMap, void* phys_addr, UINT32 length )
{
    void __iomem* virt_addr;

	pMap->phys_addr = phys_addr;
	pMap->length 	= length;
#ifdef SUPPORT_CACHE_MAP
	virt_addr		= ioremap_cached ( (ULONG)phys_addr, length );
#else
	virt_addr		= ioremap ( (ULONG)phys_addr, length );
#endif

	pMap->virt_addr	= (void*)virt_addr;	

	return ( pMap->virt_addr )? RET_OK : RET_ERROR;
}

int      OS_CloseCacheMap_Tag           ( OS_CACHE_MAP_T* pMap )
{
	if ( pMap && pMap->virt_addr )
	{
		iounmap( pMap->virt_addr );
	}

	return RET_OK;
}

int      OS_InvalCacheMap_Tag           ( OS_CACHE_MAP_T* pMap, UINT32 off, UINT32 length )
{
#ifdef SUPPORT_CACHE_MAP
    void*   virt_start  = (void*)((UINT8*)pMap->virt_addr + off );
    void*   virt_end    = (void*)((UINT8*)virt_start + length );
    ULONG	phys_start  = (ULONG)((UINT8*)pMap->phys_addr + off );
    ULONG	phys_end    = (ULONG)((UINT8*)phys_start + length );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
    enum dma_data_direction dir = DMA_FROM_DEVICE;

    dmac_map_area( virt_start, length, dir );
#else
    dmac_inv_range ( virt_start, virt_end );
#endif
    outer_inv_range( phys_start, phys_end );
#endif

	return RET_OK;
}

int      OS_FlushCacheMap_Tag           ( OS_CACHE_MAP_T* pMap, UINT32 off, UINT32 length )
{
#ifdef SUPPORT_CACHE_MAP
    void*   virt_start  = (void*)((UINT8*)pMap->virt_addr + off );
    void*   virt_end    = (void*)((UINT8*)virt_start + length );
    ULONG	phys_start  = (ULONG)((UINT8*)pMap->phys_addr + off );
    ULONG	phys_end    = (ULONG)((UINT8*)phys_start + length );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
    enum dma_data_direction dir = DMA_TO_DEVICE;

    dmac_map_area( virt_start, length, dir );
#else
    dmac_clean_range ( virt_start, virt_end );
#endif
    outer_clean_range( phys_start, phys_end );
#endif
	
	return RET_OK;
}

int      OS_CleanCacheMap_Tag           ( OS_CACHE_MAP_T* pMap, UINT32 off, UINT32 length )
{
#ifdef SUPPORT_CACHED_MAP
    void*   virt_start  = (void*)((UINT8*)pMap->virt_addr + off );
    void*   virt_end    = (void*)((UINT8*)virt_start + length );
    ULONG	phys_start  = (ULONG)((UINT8*)pMap->phys_addr + off );
   	ULONG	phys_end    = (ULONG)((UINT8*)phys_start + length );

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
    enum dma_data_direction dir = DMA_TO_DEVICE;

    dmac_map_area( virt_start, length, dir );
#else
    dmac_clean_range ( virt_start, virt_end );
#endif

    outer_clean_range( phys_start, phys_end );
#endif

	return RET_OK;
}

/** @} */

