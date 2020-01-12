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
 *  Core driver implementation for ci device.
 *
 *  author		Srinivasan Shanmugam (srinivasan.shanmugam@lge.com) / Hwajeong Lee (hwajeong.lee@lge.com)
 *  version		1.0
 *  date		2010.02.19
 *  note		Additional information.
 *
 *  @addtogroup lg1150_ci
 *	@{
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/cdev.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/memory.h>
#include "os_util.h"
#include "ci_drv.h"
#include "ci_cis.h"
#include "ci_io.h"
#include "ci_regdefs.h"

//#include "l9/ctop_ctrl_reg_l9.h"
#include "../sys/ctop_regs.h"	//for CTOP CTRL Reg. map



/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define INIT_VAL			0x0000
#define CLK_128				0x0080
#define CLK_511				0X01FF
#define CLK_639 			0X027F
#define	CD1_CD2_HIGH		0x03
#define CD_POL				0x80

#define DA_STATUS			0x80

/* Control and Status1 */
#define VCC_SW0				0x0004
#define EN_PCCARD			0x0020
#define CARD_RESET			0x0080

/* Control and Status3 */
#define IO_INT_MODE			0x0004
#define POD_MODE			0x0010

/* PC Card Control1 */
#define CHIP_MODE_IO		0X001D
#define CHIP_MODE_ATTR		0X001B

/* for CI plus */
#define RESET_PHY_INT		0x10
#define IIR_STATUS			0x10


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
extern irqreturn_t CI_ISR_Handler( int irq, void *dev_id, struct pt_regs *regs );

/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Functions
----------------------------------------------------------------------------------------*/
volatile S_CI_REG_T* gpstCIReg = NULL;

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
volatile UINT32 guwDetectCard;
volatile UINT32 guwIsPowerRestart;

unsigned long	CiModBaseAddr[MAX_MOD_NUM];

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/
/**
 *	Initialize CICAM and start Irq
*/
SINT32 CI_Initialize( void )
{
	UINT16 	unRetVal = 0;
	UINT32	virtAddr = 0;


	if( lx_chip_rev() >= LX_CHIP_REV(L9, A0) )
	{

		// Set the register area
		gpstCIReg = ( S_CI_REG_T* )ioremap(L9_CI_REG_BASE, 0x3E);
		CiModBaseAddr[MOD_A]	= (unsigned long) ioremap(L9_CI_CAM_BASE, 0x8000);

		// Control for SMC
		virtAddr	= (UINT32) ioremap(0xFF400038, 0x1C);
		*(UINT32*)(virtAddr) = (UINT32)0x0;		
		*(UINT32*)(virtAddr+0x14) = (UINT32)0x45;


		if( lx_chip_rev() >= LX_CHIP_REV(L9, B0) )	
		{	
			/* release reset */
			gpstCIReg->uniCntrlStat1.unCntrlStat1 = 0x0;
			OS_MsecSleep( 5 ); 
		
			// CTOP Power control (B0)	
 			CTOP_CTRL_L9B_RdFL(ctr29_reg_swrst);
			CTOP_CTRL_L9B_Wr01(ctr29_reg_swrst, cam_vcc_pol, 0x1); 
			CTOP_CTRL_L9B_WrFL(ctr29_reg_swrst);			
		}	

	}
	else
	{
		gpstCIReg = ( S_CI_REG_T* )( L8_CI_REG_BASE);

		CiModBaseAddr[MOD_A]	= L8_CI_CAM_BASE;

		*(UINT32*)(0xFF400038) = (UINT32)0x0;
		*(UINT32*)(0xFF40004C) = (UINT32)0x45;
	}


	// Initialize global varaibles 
	guwDetectCard 			= 0;
	guwIsPowerRestart 		= 0;

	
	// Set Not-POD Mode 
	gpstCIReg->uniCntrlStat3.unCntrlStat3 = INIT_VAL;
	gpstCIReg->uniIntrCntrl.unIntrCntrl = INIT_VAL;

	// Interrupt Enable 
	gpstCIReg->uniIntrMask.unIntrMask = 0xFFFE;	
	OS_MsecSleep( 10 );	 			// for interrupt signal is stabled
	

	unRetVal = gpstCIReg->uniIntrCntrl.unIntrCntrl;

	// CI Module detect ... check if module is already inserted 
	if( 0x00 == ( gpstCIReg->uniCntrlStat2.unCntrlStat2 & CD1_CD2_HIGH ) )
	{
		// CAM Inserted 
		unRetVal |= CD_POL;
		guwDetectCard = 1;

		CI_DBG_INFO( ( "CAM Module is inserted ...\n" ) );
	}
	else
	{
		// CAM Removed 
		unRetVal &= ~CD_POL;
		guwDetectCard = 0;

		CI_DBG_INFO( ( "CAM Module is not inserted ... \n" ) );
	}

		
	gpstCIReg->uniPCCardCntrl2.unPCCardCntrl2 = 0x0f03;
	gpstCIReg->uniPCCardCntrl3.unPCCardCntrl3 = 0x0003;

	gpstCIReg->uniIntrCntrl.unIntrCntrl = unRetVal;

	gpstCIReg->uniIntrFlag.unIntrFlag = INIT_VAL;


	if(lx_chip_rev() >= LX_CHIP_REV(L9, A0))
	{
		if( request_irq ( L9_IRQ_DVBCI, ( irq_handler_t )CI_ISR_Handler , 0, "GPCI", NULL ) )
		{
			CI_ERR( ( "Request IRQ failed!\n" ) );
			return NOT_OK;
		}
	}
	else
	{
		if( request_irq ( L8_IRQ_DVBCI, ( irq_handler_t )CI_ISR_Handler , 0, "GPCI", NULL ) )
		{
			CI_ERR( ( "Request IRQ failed!\n" ) );
			return NOT_OK;
		}
	}


	return OK;
}


/**
 *	free Irq
*/
SINT32 CI_UnInitialize( void )
{
	if( lx_chip_rev() >= LX_CHIP_REV(L9, A0) )
		free_irq( L9_IRQ_DVBCI, NULL );
	else
		free_irq( L8_IRQ_DVBCI, NULL );

	return OK;
}


/**
 *	Reset CAM module
*/
SINT32 CI_ResetCI( void )
{
	guwIsPowerRestart = 1;

	return OK;
}


/**
 *	CI CAM initialize (Manual HotSwap Sequence)
*/
SINT32 CI_CAMInit( void )  	
{
	UINT16 unRegVal = 0x0;

	/* Power On */
	unRegVal = gpstCIReg->uniCntrlStat1.unCntrlStat1;
	unRegVal |= VCC_SW0;
	gpstCIReg->uniCntrlStat1.unCntrlStat1 = unRegVal;
	
	/* Enable Card */
	OS_MsecSleep( 300 );	
	unRegVal = gpstCIReg->uniCntrlStat1.unCntrlStat1;
	unRegVal |= (EN_PCCARD | CARD_RESET);
	gpstCIReg->uniCntrlStat1.unCntrlStat1 = unRegVal;

	OS_MsecSleep( 5 ); 
	unRegVal &= ~CARD_RESET;
	gpstCIReg->uniCntrlStat1.unCntrlStat1 = unRegVal;


	return OK;
}


/**
 *	CAM power off
*/
SINT32 CI_CAMPowerOff( void )
{
	guwIsPowerRestart = 1;

	gpstCIReg->uniCntrlStat1.unCntrlStat1 = 0x0;

	return OK;
}


/**
 *	CAM power on
*/
SINT32 CI_CAMPowerOnCompleted( void )
{
	guwIsPowerRestart = 0;			

	return OK;
}


/**
 *	Check CIS(Card Information Structure)
*/
SINT32 CI_CheckCIS( void )
{
	if( guwDetectCard )
	{
		/* Set POD mode & PCMCIA attribute memory R/W */
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = POD_MODE;		
		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_ATTR;

		if( CIS_OK == ( CIS_Config( MOD_A ) ) )
		{
			CI_INFO( ( "Check CIS <<S>>\n" ) );
			return OK;
		}
		else
		{
			CI_ERR( ( "Check CIS <<F>>\n" ) );			
			return NOT_OK;
		}
	}

	CI_ERR( ( "CAM not inserted <<F>>\n" ) );

	return NOT_OK;
}



/**
 *	Write COR(Configuration Option Register)
*/
SINT32 CI_WriteCOR ( void )
{
	SINT32 wRetVal = NOT_OK;

	if( guwDetectCard )
	{
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = INIT_VAL;
		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_ATTR;

	
		if ( CIS_OK == ( CIS_WriteCOR( MOD_A ) ) )
		{
			wRetVal = OK;
			CI_INFO( ( "Wirte COR <<S>>\n" ) );				
		}
		else
		{
			CI_ERR( ( "Wirte COR <<F>>\n" ) );
		}
	}
	else
	{
		CI_ERR( ("[ CI_WriteCOR: CAM not inserted <<F>> ]\n") );
	}

	return wRetVal;
}


/**
 *	Check whether CI module is detect or remove
*/
SINT32 CI_CAMDetectStatus( UINT32 *o_puwIsCardDetected )
{
	SINT32 wRetVal = NOT_OK;

	if( o_puwIsCardDetected )
	{
		if( guwDetectCard )
		{
			CI_DBG_INFO( ( "CAM Module is inserted ... \n" ) );

			*o_puwIsCardDetected = 1;
		}
		else
		{
			CI_DBG_INFO( ( "CAM Module is removed ... \n" ) );

			*o_puwIsCardDetected = 0;
		}

		wRetVal = OK;
	}
	else
	{
		CI_ERR( ( "CI_CAMDetectStatus: Invalidate argument <<F>>\n") );
	}

	return wRetVal;
}


/**
 *	Read data from CI module
*/
SINT32 CI_ReadData( UINT8 *o_pbyData, UINT16 *io_pwDataBufSize )
{
	SINT32 wRetVal = NOT_OK;

	if( o_pbyData && io_pwDataBufSize && guwDetectCard )
	{
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = ( POD_MODE | IO_INT_MODE );

		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_IO;


		if( HW_IO_OK == ( HW_IO_Read( MOD_A, o_pbyData, io_pwDataBufSize ) ) )
		{
			wRetVal = OK;

			CI_INFO( ( " Read IO Data <<S>>\n" ) );
		}
		else
		{
			CI_ERR( ( " Read IO Data <<F>>\n" ) );
		}

	}
	else
	{
		CI_ERR( ( "CI_ReadData: CAM not inserted <<F>>\n") );					
	}

	return wRetVal;
}


/**
 * Negotiate the buffer size between host and CI Module.
*/
SINT32 CI_NegoBuff( UINT32 *o_puwBufSize )
{

	if( o_puwBufSize && guwDetectCard )
	{
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = ( POD_MODE | IO_INT_MODE );
		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_IO;


		if( HW_IO_OK == ( HW_IO_NegoBuf( MOD_A, o_puwBufSize ) ) )
		{
			CI_INFO( ( "Calc Negotiation Buffer <<S>>\n" ) );
			return OK;			
		}
		else
		{
			*o_puwBufSize = 0;

			CI_ERR( ( "Calc Negotiation Buffer <<F>>\n" ) );
		}

	}
	else
	{
		CI_ERR( ( "CI_NegoBuff failed\n") );
	}

	return NOT_OK;
}



/**
 * Read DA (Data Available register) status
 */
SINT32 CI_ReadDAStatus( UINT32 *o_puwIsDataAvailable )
{
	SINT32 wRetVal = NOT_OK;

	if( o_puwIsDataAvailable && guwDetectCard )
	{
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = ( POD_MODE | IO_INT_MODE );

		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_IO;


		if ( DA_STATUS & CHECK_DA( MOD_A ) )
		{
			*o_puwIsDataAvailable = 1;
		}
		else
		{
			*o_puwIsDataAvailable = 0;
		}

		CI_DBG_INFO( ( "DA register status 0x%x\n", *o_puwIsDataAvailable ) );

		wRetVal = OK;
	}
	else
	{
		CI_ERR( ( "CI_ReadDAStatus failed\n") );
	}

	return wRetVal;
}


/**
 *	Write data to CI module
*/
SINT32 CI_WriteData( UINT8 *i_pbyData, UINT32 i_wDataBufSize )
{
	SINT32 wRetVal = NOT_OK;

	if( i_pbyData && i_wDataBufSize && guwDetectCard )
	{
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = ( POD_MODE | IO_INT_MODE );

		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_IO;

		if ( HW_IO_OK == ( HW_IO_Write( MOD_A, i_pbyData, i_wDataBufSize ) ) )
		{
			wRetVal = OK;

			CI_INFO( ( "Write IO Data <<S>>\n" ) );
		}
		else
		{
			CI_ERR( ( "Write IO Data <<F>>\n" ) );
		}

	}
	else
	{
		CI_ERR( ( "CI_WriteData failed\n") );
	}

	return wRetVal;
}

SINT32 CI_ResetPhysicalIntrf( void )
{
	SINT32 wRetVal = NOT_OK;

	if( guwDetectCard )
	{
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = ( POD_MODE | IO_INT_MODE );

		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_IO;

		STAT_RD( MOD_A ) = RESET_PHY_INT;

		CI_INFO( ( "Reset Physical Intrf <<S>>\n" ) );

		wRetVal = OK;
	}
	else
	{
		CI_ERR( ( "CI_WriteData: CAM not inserted <<F>>\n") );
	}

	return wRetVal;
}


/**
 *	Read IIR(Initialze Interface Request) status form CI module
*/
SINT32 CI_ReadIIRStatus( UINT32 *o_puwIIRStatus )
{
	SINT32 wRetVal = NOT_OK;

	if( o_puwIIRStatus && guwDetectCard )
	{
		gpstCIReg->uniCntrlStat3.unCntrlStat3 = ( POD_MODE | IO_INT_MODE );

		gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 = CHIP_MODE_IO;

		if ( IIR_STATUS & CHECK_IIR( MOD_A ) )
		{
			*o_puwIIRStatus = 1;
		}
		else
		{
			*o_puwIIRStatus = 0;
		}

		CI_INFO( ( "Read IIR Status <<S>>\n" ) );

		wRetVal = OK;
	}
	else
	{
		CI_ERR( ( "CI_ReadIIRStatus: CAM removed\n") );					
	}

	return wRetVal;
}


/**
 *	Check CAM type
*/
SINT32 CI_CheckCAMType( SINT8 *o_pRtnValue, UINT8 *o_puwCheckCAMType )
{
	SINT32 wRetVal = NOT_OK;

	if( o_puwCheckCAMType && guwDetectCard )
	{
		wRetVal = CIS_WhatCAM( (UINT8) MOD_A, o_puwCheckCAMType );
		*o_pRtnValue = wRetVal;		// if not process CIS function, return -1
	}
	else
	{
		CI_ERR( ( "CI_CheckCAMType failed\n") );					
	}


	return wRetVal;
}


/**
 *	Print CIModule's register
*/
SINT32 CI_RegPrint( void )
{
	SINT32 wRetVal = NOT_OK;

	if( gpstCIReg )
	{
		CI_ERR( ( "\nControl and Status 1 [0x%x]", gpstCIReg->uniCntrlStat1.unCntrlStat1 ) );
		CI_ERR( ( "\nControl and Status 2 [0x%x]", gpstCIReg->uniCntrlStat2.unCntrlStat2 ) );
		CI_ERR( ( "\nControl and Status 3 [0x%x]", gpstCIReg->uniCntrlStat3.unCntrlStat3 ) );
		CI_ERR( ( "\nPC Card Control 1 [0x%x]", gpstCIReg->uniPCCardCntrl1.unPCCardCntrl1 ) );
		CI_ERR( ( "\nPC Card Control 2 [0x%x]", gpstCIReg->uniPCCardCntrl2.unPCCardCntrl2 ) );
		CI_ERR( ( "\nPC Card Control 3 [0x%x]", gpstCIReg->uniPCCardCntrl3.unPCCardCntrl3 ) );
		CI_ERR( ( "\nInterrupt Flag [0x%x]", gpstCIReg->uniIntrFlag.unIntrFlag ) );
		CI_ERR( ( "\nInterrupt Mask [0x%x]", gpstCIReg->uniIntrMask.unIntrMask ) );
		CI_ERR( ( "\nInterrupt Control [0x%x]", gpstCIReg->uniIntrCntrl.unIntrCntrl ) );
		CI_ERR( ( "\nPCMCIA Address Index Register [0x%x]", gpstCIReg->unPCMCIAAddrInd ) );
		CI_ERR( ( "\nPower On Interval 1 [0x%x]", gpstCIReg->uniHWHotSwapCntrl1.unHWHotSwapCntrl1 ) );
		CI_ERR( ( "\nPower On Interval 2 [0x%x]", gpstCIReg->uniHWHotSwapCntrl2.unHWHotSwapCntrl2 ) );
		CI_ERR( ( "\nEnable Interval 1 [0x%x]", gpstCIReg->uniHWHotSwapCntrl3.unHWHotSwapCntrl3 ) );
		CI_ERR( ( "\nEnable Interval 2 [0x%x]", gpstCIReg->uniHWHotSwapCntrl4.unHWHotSwapCntrl4 ) );
		CI_ERR( ( "\nCard Insert Done Interval 1 [0x%x]", gpstCIReg->uniHWHotSwapCntrl5.unHWHotSwapCntrl5 ) );
		CI_ERR( ( "\nCard Insert Done Interval 2 [0x%x]", gpstCIReg->uniHWHotSwapCntrl6.unHWHotSwapCntrl6 ) );
		CI_ERR( ( "\nCard Removal Done Interval 1 [0x%x]", gpstCIReg->uniHWHotSwapCntrl7.unHWHotSwapCntrl7 ) );
		CI_ERR( ( "\nCard Removal Done Interval 2 [0x%x]\n", gpstCIReg->uniHWHotSwapCntrl8.unHWHotSwapCntrl8 ) );

		wRetVal = OK;
	}


	return wRetVal;
}
