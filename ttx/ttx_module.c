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
 *	TTX module control Drivers linux module
 *
 *  author      sinchul.jung (sinchul.jung@lge.com)
 *  version		1.0
 *  date
 *  note		Additional information.
 *
 *  @addtogroup lg1150_TTX
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
#include <linux/kernel.h>	/**< printk() */
#include <linux/slab.h> 	/**< kmalloc() */
#include <linux/fs.h> 		/**< everything\ldots{} */
#include <linux/types.h> 	/**< size_t */
#include <linux/fcntl.h>	/**< O_ACCMODE */
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/ioport.h>	/**< For request_region, check_region etc */
#include <asm/io.h>			/**< For ioremap_nocache */

#include "ttx_kapi.h"

#include "ttx_module.h"
#include "ttx_drv.h"
#include "ttx_hw.h"
#include "ttx_data.h"

#include "os_util.h"

/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/
#define CC_DEBUG_PRINT_LINE_VOTE_RESULT 1


/*----------------------------------------------------------------------------------------
	Type Definitions
----------------------------------------------------------------------------------------*/
void TTX_InterruptRoutine(unsigned long unused);
DECLARE_TASKLET(TTX_ProcessData , TTX_InterruptRoutine , 0);

void TTX_CC_InterruptRoutine(unsigned long unused);
DECLARE_TASKLET(TTX_CC_ProcessData , TTX_CC_InterruptRoutine , 0);
/*----------------------------------------------------------------------------------------
	External Function Prototype Declarations
----------------------------------------------------------------------------------------*/
int (*TTX_HWInitialize)(void);
int (*TTX_SetInputFormat)(eVBIDataType type);
int (*TTX_SetInterrupt)(int on_off);
int (*TTX_ParsingVBIData)(sHeaderData *data);
int (*TTX_PutVBIData)(sHeaderData *data);
int (*TTX_SetInterruptClear)(void);
int (*TTX_CC_HWInitialize)(void);
int (*TTX_CC_SetInputFormat)(eVBIDataType type);
int (*TTX_CC_SetInterrupt)(int on_off);
int (*TTX_CC_ParsingVBIData)(unsigned int *packet);
int (*TTX_CC_SetInterruptClear)(void);
int (*TTX_CC_DebugModeOnOffCCPrint)(int mode);
int (*TTX_CheckOnLine318)(int mode);
int (*TTX_SetTypeLine318)(int onoff);
int (*TTX_SetVBILPF)(int onoff);
#ifdef	KDRV_CONFIG_PM
int (*TTX_CC_RunSuspend)(void);
int (*TTX_CC_RunResume)(void);
int (*TTX_RunSuspend)(void);
int (*TTX_RunResume)(void);
#endif
/*----------------------------------------------------------------------------------------
	External Variables
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	global Variables
----------------------------------------------------------------------------------------*/
int gEnablePrintResultofLineDetect = 0;

/*----------------------------------------------------------------------------------------
	Static Function Prototypes Declarations
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Static Variables
----------------------------------------------------------------------------------------*/


/*========================================================================================
	Implementation Group
========================================================================================*/

/**
* TTX Module Chip revision converter
*
* @parm void
* @return int
*/

int TTX_InitChipRev(void)
{
	int ret = 0;


	if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
	{ /* L9 B0 revision */
		TTX_HWInitialize	= TTX_HWInitialize_L9Bx;
		TTX_SetInputFormat	= TTX_SetInputFormat_L9Bx;
		TTX_SetInterrupt	= TTX_SetInterrupt_L9Bx;
		TTX_SetInterruptClear	= TTX_SetInterruptClear_L9Bx;
		TTX_ParsingVBIData	= TTX_ParsingVBIData_L9Bx;
		TTX_PutVBIData		= TTX_PutVBIData_L9Bx;
		TTX_SetVBILPF	= TTX_SetVBILPF_L9Bx;
		TTX_CheckOnLine318	= TTX_CheckOnLine318_L9Bx;
		TTX_SetTypeLine318	= TTX_SetTypeLine318_L9Bx;
#ifdef	KDRV_CONFIG_PM
			TTX_RunSuspend		= TTX_RunSuspend_L9Bx;
		TTX_RunResume		= TTX_RunResume_L9Bx;
#endif
		//memcpy((void *)&gstMemCfgTTXDev , (void *)&gstMemCfgTTXRev[2] , sizeof(LX_TTX_MEM_CFG_S_T) );
	}
	else if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
	{ /* L9 A0 revision */
		TTX_HWInitialize	= TTX_HWInitialize_L9Ax;
		TTX_SetInputFormat	= TTX_SetInputFormat_L9Ax;
		TTX_SetInterrupt	= TTX_SetInterrupt_L9Ax;
		TTX_SetInterruptClear	= TTX_SetInterruptClear_L9Ax;
		TTX_ParsingVBIData	= TTX_ParsingVBIData_L9Ax;
		TTX_PutVBIData		= TTX_PutVBIData_L9Ax;
		TTX_SetVBILPF	= TTX_SetVBILPF_L9Ax;
		TTX_CheckOnLine318	= TTX_CheckOnLine318_L9Ax;
		TTX_SetTypeLine318	= TTX_SetTypeLine318_L9Ax;
#ifdef	KDRV_CONFIG_PM
		TTX_RunSuspend		= TTX_RunSuspend_L9Ax;
		TTX_RunResume		= TTX_RunResume_L9Ax;
#endif
		//memcpy((void *)&gstMemCfgTTXDev , (void *)&gstMemCfgTTXRev[1] , sizeof(LX_TTX_MEM_CFG_S_T) );
	}
	else if ( lx_chip_rev( ) >= LX_CHIP_REV( L8, B0 ) )
	{ /* L8 B0 revision */
       	TTX_HWInitialize	= TTX_HWInitialize_L8Bx;
		TTX_SetInputFormat	= TTX_SetInputFormat_L8Bx;
		TTX_SetInterrupt	= TTX_SetInterrupt_L8Bx;
		TTX_SetInterruptClear	= TTX_SetInterruptClear_L8Bx;
		TTX_ParsingVBIData	= TTX_ParsingVBIData_L8Bx;
		TTX_PutVBIData		= TTX_PutVBIData_L8Bx;
		TTX_SetVBILPF	= TTX_SetVBILPF_L8Bx;
		TTX_CheckOnLine318	= TTX_CheckOnLine318_L8Bx;
		TTX_SetTypeLine318	= TTX_SetTypeLine318_L8Bx;
#ifdef	KDRV_CONFIG_PM
		TTX_RunSuspend		= TTX_RunSuspend_L8Bx;
		TTX_RunResume		= TTX_RunResume_L8Bx;
#endif
		//memcpy((void *)&gstMemCfgTTXDev , (void *)&gstMemCfgTTXRev[0] , sizeof(LX_TTX_MEM_CFG_S_T) );
	}
	else
	{ /* L8 Ax  revision */
	    TTX_HWInitialize	= TTX_HWInitialize_L8Ax;
		TTX_SetInputFormat	= TTX_SetInputFormat_L8Ax;
		TTX_SetInterrupt	= TTX_SetInterrupt_L8Ax;
		TTX_SetInterruptClear	= TTX_SetInterruptClear_L8Ax;
		TTX_ParsingVBIData	= TTX_ParsingVBIData_L8Ax;
		TTX_PutVBIData		= TTX_PutVBIData_L8Ax;
		TTX_SetVBILPF	= TTX_SetVBILPF_L8Ax;
		TTX_CheckOnLine318	= TTX_CheckOnLine318_L8Ax;
		TTX_SetTypeLine318	= TTX_SetTypeLine318_L8Ax;
#ifdef	KDRV_CONFIG_PM
		TTX_RunSuspend		= TTX_RunSuspend_L8Ax;
		TTX_RunResume		= TTX_RunResume_L8Ax;
#endif
		//memcpy((void *)&gstMemCfgTTXDev , (void *)&gstMemCfgTTXRev[0] , sizeof(LX_TTX_MEM_CFG_S_T) );
	}

	return ret;
}


/**
* TTX Module initialize
*
* @parm void
* @return int
*/

int TTX_Initialize(void)
{
	TTX_InitChipRev();

	TTX_HWInitialize();
	TTX_SetInputFormat(eDisable);
	TTX_InitDataMemory();

	TTX_SetInputFormat(CURRENT_TTX);
	TTX_InterruptControl(1);

	return 0;
}


/**
* VBI interrupt control
*
* @parm void
* @return int
*/

int TTX_InterruptControl(int onoff)
{

	TTX_SetInterrupt(onoff);

	return 0;
}

void TTX_InterruptRoutine(unsigned long unused)
{
	sHeaderData data;

	if(TTX_ParsingVBIData(&data) < 0) return;

//	if(TTX_GetDataAddr(&data) < 0) return;

	TTX_PutVBIData(&data);
}

int TTX_CC_InitChipRev(void)
{
	int ret = 0;

	//won.hur : added additional chip selection routine for L9B0
	//@2011.08.24
	if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
	{
	    TTX_CC_HWInitialize		= CC_L9B0_HWInitialize;
		TTX_CC_SetInputFormat	= CC_L9B0_SetInputFormat;
		TTX_CC_SetInterrupt		= CC_L9B0_SetInterrupt;
		TTX_CC_SetInterruptClear	= CC_L9B0_SetInterruptClear;
		TTX_CC_ParsingVBIData	= CC_L9B0_ParsingVBIData;
		TTX_CC_DebugModeOnOffCCPrint = CC_L9B0_DebugModeOnOffCCPrint;
#ifdef	KDRV_CONFIG_PM
		TTX_CC_RunSuspend		= CC_L9B0_RunSuspend;
		TTX_CC_RunResume		= CC_L9B0_RunResume;
#endif

//		memcpy((void *)&gMemCfgCc , (void *)&gRevMemCfgCc[1] , sizeof(LX_CC_MEM_CFG_T) );
	}
	else if( lx_chip_rev( ) >= LX_CHIP_REV( L9, A0 ) )
	{

	    TTX_CC_HWInitialize		= CC_L9A0_HWInitialize;
		TTX_CC_SetInputFormat	= CC_L9A0_SetInputFormat;
		TTX_CC_SetInterrupt		= CC_L9A0_SetInterrupt;
		TTX_CC_SetInterruptClear	= CC_L9A0_SetInterruptClear;
		TTX_CC_ParsingVBIData	= CC_L9A0_ParsingVBIData;
		TTX_CC_DebugModeOnOffCCPrint = CC_L9A0_DebugModeOnOffCCPrint;
#ifdef	KDRV_CONFIG_PM
		TTX_CC_RunSuspend		= CC_L9A0_RunSuspend;
		TTX_CC_RunResume		= CC_L9A0_RunResume;
#endif

//		memcpy((void *)&gMemCfgCc , (void *)&gRevMemCfgCc[1] , sizeof(LX_CC_MEM_CFG_T) );
	}
	else if( lx_chip_rev( ) == LX_CHIP_REV( L8, A1 ) )
	{ /* L8 Ax  revision */
	    TTX_CC_HWInitialize		= CC_L8Ax_HWInitialize;
		TTX_CC_SetInputFormat	= CC_L8Ax_SetInputFormat;
		TTX_CC_SetInterrupt		= CC_L8Ax_SetInterrupt;
		TTX_CC_SetInterruptClear	= CC_L8Ax_SetInterruptClear;
		TTX_CC_ParsingVBIData	= CC_L8Ax_ParsingVBIData;
		TTX_CC_DebugModeOnOffCCPrint = CC_L8Ax_DebugModeOnOffCCPrint;
#ifdef	KDRV_CONFIG_PM
		TTX_CC_RunSuspend		= CC_L8Ax_RunSuspend;
		TTX_CC_RunResume		= CC_L8Ax_RunResume;
#endif

//		memcpy((void *)&gMemCfgCc , (void *)&gRevMemCfgCc[0] , sizeof(LX_CC_MEM_CFG_T) );
	}
	else if ( lx_chip_rev( ) == LX_CHIP_REV( L8, B0 ) )
//	if(1)
	{ /* L8 B0 revision */
       	TTX_CC_HWInitialize		= CC_L8B0_HWInitialize;
		TTX_CC_SetInputFormat	= CC_L8B0_SetInputFormat;
		TTX_CC_SetInterrupt		= CC_L8B0_SetInterrupt;
		TTX_CC_SetInterruptClear	= CC_L8B0_SetInterruptClear;
		TTX_CC_ParsingVBIData	= CC_L8B0_ParsingVBIData;
		TTX_CC_DebugModeOnOffCCPrint = CC_L8B0_DebugModeOnOffCCPrint;
#ifdef	KDRV_CONFIG_PM
		TTX_CC_RunSuspend		= CC_L8B0_RunSuspend;
		TTX_CC_RunResume		= CC_L8B0_RunResume;
#endif
//		memcpy((void *)&gMemCfgCc , (void *)&gRevMemCfgCc[0] , sizeof(LX_CC_MEM_CFG_T) );
	}
	else{
		printk("chip revision error %s\n", __func__);
		ret = -1;
	}

	return ret;
}



int TTX_CC_Initialize(void)
{
	// chip revision convert
	if(TTX_CC_InitChipRev() < 0)
	{
		return -1;
	}

	TTX_CC_HWInitialize();
	TTX_CC_SetInputFormat(eDisable);
	TTX_CC_InitDataMemory();

	TTX_CC_SetPrintResultOfLineDetect(0);

	TTX_CC_SetInputFormat(eClosedCaptionUS);
	return 0;
}

/**
* VBI interrupt service routine
*
* @parm void
* @return int
*/

int TTX_InterruptFunc(unsigned int status)
{

	tasklet_schedule(&TTX_ProcessData);

	TTX_SetInterruptClear();

	return 0;
}


int TTX_CC_InterruptControl(int onoff)
{

	TTX_CC_SetInterrupt(onoff);

	return 0;
}

int TTX_SetVBILPFOption(int onoff)
{
	TTX_SetVBILPF(onoff);

	return 0;
}

int TTX_CC_SetPrintResultOfLineDetect(int onoff)
{
	if(onoff) gEnablePrintResultofLineDetect = 1;
	else gEnablePrintResultofLineDetect = 0;

	return 0;
}

void TTX_CC_InterruptRoutine(unsigned long unused)
{
	static unsigned int data = 0;
	static unsigned int data_array[5] = {0,};
	static unsigned int line_number_count[3] ={0,} ;
	static unsigned int line_number_sum[3] ={0,} ;
	static unsigned int raw_line_number_count[3] ={0,};
	static unsigned int raw_line_number_sum[3] ={0,};
	static unsigned int curr_cc_int_time, prev_cc_int_time = 0;
	int ret = 0;
	int loop_count;
	int line_number;
	int line_data;
	int parity_valid_flag = 0;
	static unsigned int max_line_number = 21;
	static unsigned int prev_max_line_number = 21;
	static unsigned int curr_max_line_number = 21;
	static unsigned int max_value = 0;
	static unsigned int raw_max_value = 0;


	if ( lx_chip_rev( ) >= LX_CHIP_REV( L9, B0 ) )
	{
		ret = CC_L9B0_ParsingVBIData(data_array);

		curr_cc_int_time = jiffies_to_msecs(jiffies);	// get current time

		for(loop_count = 0;loop_count <ret;loop_count ++)
		{
			//CC_PRINT("loop count = [%d]\n", loop_count);

			line_number = (data_array[loop_count]&0xFFC00000) >> 22;
			line_data = (data_array[loop_count]&0x0000FFFF);

			parity_valid_flag = TTX_CheckDataParity(line_data);

			//if(line_data == 0x8080) CC_PRINT("Data on line [%d] is [0x8080]\n", line_number);

			if((line_number == 18) || (line_number == 281))
			{
				raw_line_number_count[0]++;
				if((line_data != 0x8080)&&(parity_valid_flag == 1)) line_number_count[0] ++;
			}
			else if((line_number == 21) || (line_number ==284))
			{
				raw_line_number_count[1]++;
				if((line_data != 0x8080)&&(parity_valid_flag == 1))line_number_count[1] ++;
			}
			else if((line_number == 22) || (line_number ==285))
			{
				raw_line_number_count[2]++;
				if((line_data != 0x8080) &&(parity_valid_flag == 1))line_number_count[2] ++;
			}
			else
			{
			//	CC_PRINT("ERROR BAD LINE NUMBER [%d], data_array[0x%x]\n", line_number, data_array[loop_count]);
			}

			// Select most frequent CC line number for CC slicing
			if( (curr_cc_int_time - prev_cc_int_time ) > 500)
			{
				memcpy(raw_line_number_sum, raw_line_number_count, sizeof(raw_line_number_count));
				memset(raw_line_number_count, 0, sizeof(raw_line_number_count));
				memcpy(line_number_sum, line_number_count, sizeof(line_number_count));
				memset(line_number_count, 0, sizeof(line_number_count));
				prev_cc_int_time = jiffies_to_msecs(jiffies);	// get current time
				max_value = 0;
				raw_max_value = 0;
		//		CC_PRINT("LINE18[%d], LINE21[%d], LINE22[%d]\n", line_number_sum[0], line_number_sum[1], line_number_sum[2]);


				if(raw_line_number_sum[0] > ((raw_line_number_sum[1])*3) )
				{
					curr_max_line_number = 18;
					max_line_number = curr_max_line_number;
					max_value = line_number_sum[0];
					raw_max_value = raw_line_number_sum[0];
				}
				else if(raw_line_number_sum[0] >= ((raw_line_number_sum[1]>>2)+(raw_line_number_sum[1]>>3)) )
				{
					//CC_PRINT("Difficuilt line selection #1...\n");
					if(	line_number_sum[0] > line_number_sum[1])
					{

						if(raw_line_number_sum[0] >= VALID_LOWER_RANGE_OF_CC_LINE )
						{
							curr_max_line_number = 18;
							if(prev_max_line_number == curr_max_line_number) max_line_number = curr_max_line_number;
							max_value = line_number_sum[0];
							raw_max_value = raw_line_number_sum[0];
						}
						else if(raw_line_number_sum[1] >= VALID_LOWER_RANGE_OF_CC_LINE)
						{
							curr_max_line_number = 21;
							if(prev_max_line_number == curr_max_line_number) max_line_number = curr_max_line_number;
							max_value = line_number_sum[1];
							raw_max_value = raw_line_number_sum[1];
						}
						else
						{
							//CC_PRINT(" State 1 : use previous...\n");
							// 이 경우에는 최종 판별을 보류하나, 세번째 데이터의 판별을 위하여 비교 값들은 정해둔다.
							if(raw_line_number_sum[0] >= raw_line_number_sum[1])
							{
								max_value = line_number_sum[0];
								raw_max_value = raw_line_number_sum[0];
							}
							else
							{
								max_value = line_number_sum[1];
								raw_max_value = raw_line_number_sum[1];
							}
						}
					}
					else if(line_number_sum[0] < line_number_sum[1])
					{

						if(raw_line_number_sum[1] >= VALID_LOWER_RANGE_OF_CC_LINE )
						{
							curr_max_line_number = 21;
							if(prev_max_line_number == curr_max_line_number) max_line_number = curr_max_line_number;
							max_value = line_number_sum[1];
							raw_max_value = raw_line_number_sum[1];
						}
						else
						{
							//CC_PRINT(" State 1 : use previous...\n");
							if(raw_line_number_sum[1] >= raw_line_number_sum[0])
							{
								max_value = line_number_sum[1];
								raw_max_value = raw_line_number_sum[1];
							}
							else
							{
								max_value = line_number_sum[0];
								raw_max_value = raw_line_number_sum[0];
							}
						}
					}
					else
					{
						//CC_PRINT(" State 1 : use previous...\n");
						if(raw_line_number_sum[0] >= raw_line_number_sum[1])
						{
							raw_max_value = raw_line_number_sum[0];
							max_value = line_number_sum[0];
						}
						else
						{
							raw_max_value = raw_line_number_sum[1];
							max_value = line_number_sum[1];
						}
					}
				}
				else
				{
					curr_max_line_number = 21;
					max_line_number = curr_max_line_number;
					max_value = line_number_sum[1];
					raw_max_value = raw_line_number_sum[1];
				}


				if(raw_line_number_sum[2] > (raw_max_value*3))
				{
					curr_max_line_number = 22;
					max_line_number = curr_max_line_number;
					max_value = line_number_sum[2];
					raw_max_value = raw_line_number_sum[2];
				}
				else if(raw_line_number_sum[2] >= ((raw_max_value>>2)+(raw_max_value>>3)) )
				{
					//CC_PRINT("Difficuilt line selection #2...\n");
					if(line_number_sum[2] > max_value)
					{
						if(raw_line_number_sum[2] >= VALID_LOWER_RANGE_OF_CC_LINE)
						{
							curr_max_line_number = 22;
							if(prev_max_line_number == curr_max_line_number) max_line_number = curr_max_line_number;
							max_value = line_number_sum[2];
							raw_max_value = raw_line_number_sum[2];
						}
						else if(raw_max_value >= VALID_LOWER_RANGE_OF_CC_LINE)
						{
							//CC_PRINT(" State 2 : use previous...\n");
							// do nothing...
						}
						else
						{
							// do nothing...
							//CC_PRINT(" State 2 : use previous...\n");
						}
					}
					else if(line_number_sum[2] < max_value)
					{
						// do nothing
						//CC_PRINT(" State 2 : use previous...\n");
					}
					else
					{
						// do nothing
						//CC_PRINT(" State 2 : use previous...\n");
					}
				}
				else
				{
					// do nothing
				}

#if CC_DEBUG_PRINT_LINE_VOTE_RESULT
				if(gEnablePrintResultofLineDetect)
				{
					CC_PRINT("Vote value : 18:[%d], 21:[%d], 22:[%d]\n", line_number_sum[0], line_number_sum[1], line_number_sum[2]);
					CC_PRINT("RAW Vote value : 18:[%d], 21:[%d], 22:[%d]\n", raw_line_number_sum[0], raw_line_number_sum[1], raw_line_number_sum[2]);
					CC_PRINT("Max Line Number = [%d], curr = [%d], prev = [%d]\n", max_line_number, curr_max_line_number, prev_max_line_number);
				}
#endif

				prev_max_line_number = curr_max_line_number;;
			}

			/*
			if( (line_number != 21) && (line_number !=284))
			{
			//	CC_PRINT("line number = [%d]\n", line_number);
			}
			else
				TTX_CC_PutVBIData(&data_array[loop_count] , 1);
				*/
#if 0
			if(raw_max_value > 6)	// minimum 7 packets to determine valid CC data.
			{
				if( (max_line_number == 18) && ( (line_number == 18) || (line_number ==281)) )
				{
					TTX_CC_PutVBIData(&data_array[loop_count] , 1);
				}
				else if( (max_line_number == 21) && ( (line_number == 21) || (line_number ==284)) )
				{
					TTX_CC_PutVBIData(&data_array[loop_count] , 1);
				}
				else if ( (max_line_number == 22) && ( (line_number == 22) || (line_number ==285)) )
				{
					TTX_CC_PutVBIData(&data_array[loop_count] , 1);
				}
				else
				{
					//				CC_PRINT("garbage CC packet???\n");
				}
			}
#else
			if( (max_line_number == 18) && ( (line_number == 18) || (line_number ==281)) )
			{
				if(raw_line_number_sum[0] > 6) TTX_CC_PutVBIData(&data_array[loop_count] , 1);
			}
			else if( (max_line_number == 21) && ( (line_number == 21) || (line_number ==284)) )
			{
				if(raw_line_number_sum[1] > 6)TTX_CC_PutVBIData(&data_array[loop_count] , 1);
			}
			else if ( (max_line_number == 22) && ( (line_number == 22) || (line_number ==285)) )
			{
				if(raw_line_number_sum[2] > 6)TTX_CC_PutVBIData(&data_array[loop_count] , 1);
			}
			else
			{
				//				CC_PRINT("garbage CC packet???\n");
			}
#endif
		}
	}
	else
	{
		ret = TTX_CC_ParsingVBIData(&data);

		if(ret >= 0 )
			TTX_CC_PutVBIData(&data , 1);
	}
/* 	Latch up debug
	else
		TTX_CC_PRINT("data corruption \n");
		*/
}


/**
* VBI interrupt service routine
*
* @parm void
* @return int
*/

int TTX_CC_InterruptFunc(unsigned int status)
{

	tasklet_schedule(&TTX_CC_ProcessData);
//	TTX_CC_SetInterruptClear();

	return 0;
}



/** @} */
