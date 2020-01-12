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

/*****************************************************************************
* (c)Copyright 2002. All Rights Reserved
* LG Soft India Private Limited
* Cherry Hills
* Embassy Golf Link Business Park
* Bangalore - 560017
* India.
*
* Project Name			: RUKH Verfication
* Developed by 			: Eureka
* Security			: Confidential
******************************************************************************/

/*****************************************************************************
* $Author			: Balaji H
* $Archive			: ???????
* $Revision			: 0.0a
* $Modtime			: 09/11/2005	5:30 PM
******************************************************************************/

/*****************************************************************************
* Filename			: DWLD_Parser.c
* Purpose			: To download any data through the serial Port
* Platform			: Linux-2.6.16.12
* Author(s)			: Balaji H
* e-mail			: balaji.haridass@lgsoftindia.com
* Creation date			: 09/11/2005	5:30 PM
* Modified By			:
* Modification description(s)	:
******************************************************************************/

/*****************************************************************************
*	Insight on the Download Parser module
******************************************************************************
*	This module does "Download Manager specific parsing. The
*	download manager download the data from the port. If it is
*	a test data file then the current file parses the Test Data
*	on request by the download manager
*/

/*****************************************************************************
*	Includes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "base_types.h"

#include "dbg/dbg_def.h"
#include "de/de_xdp.h"

/*****************************************************************************
*	Defines
*/



/*****************************************************************************
*	Macros
*/

/*****************************************************************************
*	Enumerations
*/


/*****************************************************************************
*	Constant Variables
*/

/*****************************************************************************
*	Structure Declrations
*/

/*****************************************************************************
*	Extern Variables
*/


/*****************************************************************************
*	Global Variables
*/


/*****************************************************************************
*	Static/Private Variables
*/

/*****************************************************************************
*	Functions Prototypes
*/
int CLI_I2C_Start(void);
int CLI_VENC_Start(void);


/*****************************************************************************
*	typedefs
*/

/*****************************************************************************
*	Extern Functions
*/

extern void cli_start (void);
extern void cli_join (void);

extern void CI_AppInit ( void );
extern void CLI_SE_Start( void );
//end fo the vdec



/*****************************************************************************
*	Functions
*/


/*****************************************************************************
 *Function Name	:main
 *Arguments			:none
 *Return Type		:int32
 *****************************************************************************/
int main ()
{

	printf ( "DTV-SoC verification software : %s", __DATE__ );

	cli_start();

	// add your create_thread code

#ifdef INCLUDE_KDRV_DE
	DE_DEV_Open();
	CLI_XDP_Start();
	CLI_VIDEO_Start();
#endif
#ifdef INCLUDE_KDRV_PQL
	PQL_DEV_Open();
	CLI_PQL_Start();
#endif
#ifdef INCLUDE_KDRV_DIMM
	DIMM_DEV_Open();
	CLI_DIMM_Start();
#endif
#ifdef INCLUDE_KDRV_TCON
	TCON_DEV_Open();
	CLI_TCON_Start();
#endif
#ifdef INCLUDE_KDRV_FMT3D
	FMT3D_DEV_Open();
	CLI_FMT3D_Start();
#endif
#ifdef INCLUDE_KDRV_DENC
	DENC_DEV_Open();
	CLI_DENC_Start();
#endif
#ifdef INCLUDE_KDRV_GPIO
	CLI_GPIO_Start();
#endif
#ifdef INCLUDE_KDRV_FRC
	CLI_FRC_Start();
#endif
#ifdef INCLUDE_KDRV_I2C
	CLI_I2C_Start();
#endif

#ifdef INCLUDE_KDRV_VENC
	CLI_VENC_Start();
#endif

#ifdef INCLUDE_KDRV_CI
	CI_AppInit();
#endif /* INCLUDE_KDRV_CI */


#ifdef INCLUDE_KDRV_SE
	CLI_SE_Start();
#endif /* INCLUDE_KDRV_SE */

	cli_join();

#ifdef INCLUDE_KDRV_DE
	DE_DEV_Close();
#endif

#ifdef INCLUDE_KDRV_DIMM
	DIMM_DEV_Close();
#endif
#ifdef INCLUDE_KDRV_PQL
	PQL_DEV_Close();
#endif
#ifdef INCLUDE_KDRV_TCON
	TCON_DEV_Close();
#endif
#ifdef INCLUDE_KDRV_FMT3D
	FMT3D_DEV_Close();
#endif
#ifdef INCLUDE_KDRV_DENC
	DENC_DEV_Close();
#endif

	return 0;
}

