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

/** @file
 *
 *  main configuration file for hdmi device
 *	hdmi device will teach you how to make device driver with new platform.
 *
 *  author		sh.myoung (sh.myoung@lge.com)
 *  version		1.0
 *  date		2010.01.06
 *  note		Additional information.
 *
 *  @addtogroup lg1150_hdmi
 *	@{
 */

#ifndef	_HDMI_CFG_H_
#define	_HDMI_CFG_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/

#ifdef	__cplusplus
extern "C"
{
#endif /* __cplusplus */

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/
#define	HDMI_MODULE			"hdmi"
#define HDMI_MAX_DEVICE		1

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
/**
HDMI Register configuration.
*/
typedef struct
{
	char*		chip_name;					// Chip Name
	UINT32		vport_reg_base_addr;		//	vport Register Base Address for Real Chip.
	UINT32		vport_reg_size;				//	vport Register Size  for Real Chip.
	UINT32		vport_qemu_base_addr;		//	vport Qemu Register Base Address.

	UINT32		phy_reg_base_addr;			//	Phy Register Base Address for Real Chip.
	UINT32		phy_reg_size;				//	Phy Register Size for Real Chip.
	UINT32		phy_qemu_base_addr;			//	Phy Qemu Register Base Address.
} LX_HDMI_REG_T;


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
extern LX_HDMI_REG_T *pHdmiRegCfg, gRegCfgHdmi[];

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* _HDMI_CFG_H_ */

/** @} */

