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
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author     raxis.lim
 *  @version    1.0
 *  @date       2011-04-03
 *  @note       Additional information.
 */

#ifndef	_FBDEV_HAL_H_
#define	_FBDEV_HAL_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "fbdev_hw.h"

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef	struct
{
	LX_FBDEV_CFG_T*	(*GetCfg)		(void);

	int		(*InitHW)				(void);
	int		(*ShutdownHW)			(void);
	int		(*RunSuspend)			(void);
	int		(*RunResume)			(void);

	int 	(*SetOSDEnable)         (int layer , int enable);
	int 	(*GetOSDEnable)         (int layer);

	int		(*InitOSDLayer)			(int layer);
	int 	(*WriteOSDHeader)       (struct scanvas  *pstCanvas);

	int 	(*GetZList)             (LX_FBDEV_ZLIST_T* zList );
	int 	(*SetZList)             (LX_FBDEV_ZLIST_T* zList );

	int 	(*WriteOSDEndian)       (int layer , ENDIAN_INFORM_T *endian);
	int 	(*WriteOSDCSC)          (int layer , CSC_INFORM_T *csc);

	int 	(*DownloadPalette)      (unsigned int addr , unsigned int size , void *data);

	int 	(*GetInterruptStatus)   (INTERRUPT_INFORM_T intr);
	int 	(*SetInterruptClear)    (INTERRUPT_INFORM_T intr);
	int 	(*SetInterruptEnable)   (INTERRUPT_INFORM_T intr , int enable);

	int     (*SetConnCtrl)			(LX_FBDEV_CONN_CTRL_T* pConnCtrl);
	int		(*SetTridCtrl)			(int layer, LX_FBDEV_TRID_CTRL_T* ctrl);
	int		(*SetOutPathCtrl)		(int layer, LX_FBDEV_OUT_PATH_CTRL_T* ctrl);

	int		(*SetVideoMixerCtrl)	(LX_FBDEV_VIDEO_MIXER_CTRL_T* ctrl);

	void	(*ISRTickHandler)		(UINT32);
}
FBDEV_HAL_T;

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern	FBDEV_HAL_T	g_fbdev_hal;

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _FBDEV_HAL_H_ */

