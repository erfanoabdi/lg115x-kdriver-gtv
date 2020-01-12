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

/** @file pe_cmg_coeff.h
 *
 *  driver header for picture enhance color management parameters. ( used only within kdriver )
 *	- initial settings
 *	- default settings for each format
 *	
 *	@author		Seung-Jun,Youm(sj.youm@lge.com)
 *	@version	0.1
 *	@note		
 *	@date		2011.06.11
 *	@see		
 */

#ifndef	_PE_CMG_COEFF_H_
#define	_PE_CMG_COEFF_H_

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#include "pe_reg.h"

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

/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/
/**
*	initial cen param of L9A0 : cmg_l_init_param_l9a0, cmg_r_init_param_l9a0
*
*	- change history
*	20110627, sj.youm, region enable off, 0x1604,0x3604 : 0x01FF0000 -> 0x00000000
*/
const PE_REG_PARAM_T cmg_l_init_param_l9a0[] =
{
	/*---	CEN - GAIN_H	---*/
	{0x1614,0x00001000},	{0x1618,0x0000007F},	{0x1618,0x0048007F},	{0x1618,0x0094007F},
	{0x1618,0x00DD007F},	{0x1618,0x0127007F},	{0x1618,0x0171007F},	{0x1618,0x01BB007F},
	{0x1618,0x01FE007F},	{0x1618,0x0000007F},	{0x1618,0x0048007F},	{0x1618,0x0094007F},
	{0x1618,0x00DD007F},	{0x1618,0x0127007F},	{0x1618,0x0171007F},	{0x1618,0x01BB007F},
	{0x1618,0x01FF007F},	{0x1618,0x01FE0000},	{0x1618,0x0209002D},	{0x1618,0x0215005A},
	{0x1618,0x021F007F},	{0x1618,0x0230007F},	{0x1618,0x023A005A},	{0x1618,0x0244002D},
	{0x1618,0x02510000},	{0x1618,0x005C0000},	{0x1618,0x0072002D},	{0x1618,0x0087005A},
	{0x1618,0x0095007F},	{0x1618,0x00B6007F},	{0x1618,0x00C4005A},	{0x1618,0x00D6002D},
	{0x1618,0x00E30000},	{0x1618,0x00F10000},	{0x1618,0x00F8003C},	{0x1618,0x0100006E},
	{0x1618,0x010E007F},	{0x1618,0x0131007F},	{0x1618,0x0141006E},	{0x1618,0x0159003C},
	{0x1618,0x01710000},	{0x1618,0x01B80000},	{0x1618,0x01D4002D},	{0x1618,0x01E3005A},
	{0x1618,0x01F1007F},	{0x1618,0x020E007F},	{0x1618,0x021C005A},	{0x1618,0x0229002D},
	{0x1618,0x02400000},	{0x1618,0x00430000},	{0x1618,0x0047002D},	{0x1618,0x004E005A},
	{0x1618,0x0055007F},	{0x1618,0x005C007F},	{0x1618,0x0063005A},	{0x1618,0x006A002D},
	{0x1618,0x00720000},	{0x1618,0x018D0000},	{0x1618,0x0195002D},	{0x1618,0x019C005A},
	{0x1618,0x01A3007F},	{0x1618,0x01AA007F},	{0x1618,0x01B1005A},	{0x1618,0x01B8002D},
	{0x1618,0x01C10000},	{0x1618,0x00D50000},	{0x1618,0x00DC003C},	{0x1618,0x00E3006E},
	{0x1618,0x00EA007F},	{0x1618,0x00F1007F},	{0x1618,0x00F8006E},	{0x1618,0x00FD003C},
	{0x1618,0x01000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},

	/*---	CEN - GAIN_S	---*/
	{0x1614,0x00001100},	{0x1618,0x0000007F},	{0x1618,0x0002007F},	{0x1618,0x00050078},
	{0x1618,0x000A0073},	{0x1618,0x000C006E},	{0x1618,0x0013005A},	{0x1618,0x00190028},
	{0x1618,0x00200000},	{0x1618,0x0000007F},	{0x1618,0x0006007F},	{0x1618,0x000C007F},
	{0x1618,0x0033007F},	{0x1618,0x004C006E},	{0x1618,0x00590064},	{0x1618,0x0066003C},
	{0x1618,0x007F0000},	{0x1618,0x001C0000},	{0x1618,0x0022003C},	{0x1618,0x00270064},
	{0x1618,0x002C007F},	{0x1618,0x0039006E},	{0x1618,0x00460046},	{0x1618,0x00530014},
	{0x1618,0x00600000},	{0x1618,0x00140000},	{0x1618,0x00260028},	{0x1618,0x0039005A},
	{0x1618,0x004C007F},	{0x1618,0x0059007F},	{0x1618,0x0066007F},	{0x1618,0x0073007F},
	{0x1618,0x007F007F},	{0x1618,0x00140000},	{0x1618,0x00260028},	{0x1618,0x0039005A},
	{0x1618,0x0053007F},	{0x1618,0x0059007F},	{0x1618,0x0066007F},	{0x1618,0x0073007F},
	{0x1618,0x007F007F},	{0x1618,0x00590000},	{0x1618,0x005E001E},	{0x1618,0x00630046},
	{0x1618,0x006A0064},	{0x1618,0x006F0073},	{0x1618,0x0074007F},	{0x1618,0x0078007F},
	{0x1618,0x007F007F},	{0x1618,0x00200000},	{0x1618,0x002C0028},	{0x1618,0x003D005A},
	{0x1618,0x004C007F},	{0x1618,0x0059007F},	{0x1618,0x0066007F},	{0x1618,0x0073007F},
	{0x1618,0x007F007F},	{0x1618,0x00140000},	{0x1618,0x00260028},	{0x1618,0x0039005A},
	{0x1618,0x004C007F},	{0x1618,0x0059007F},	{0x1618,0x0066007F},	{0x1618,0x0073007F},
	{0x1618,0x007F007F},	{0x1618,0x00140000},	{0x1618,0x00260028},	{0x1618,0x0039005A},
	{0x1618,0x0053007F},	{0x1618,0x0059007F},	{0x1618,0x0066007F},	{0x1618,0x0073007F},	
	{0x1618,0x007F007F},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},

	/*---	CEN - GAIN_L	---*/
	{0x1614,0x00001200},	{0x1618,0x00640000},	{0x1618,0x00780012},	{0x1618,0x008C0024},
	{0x1618,0x00A00036},	{0x1618,0x00B40048},	{0x1618,0x00C8005A},	{0x1618,0x00DC006C},
	{0x1618,0x00FF007F},	{0x1618,0x0000007F},	{0x1618,0x00030078},	{0x1618,0x00050064},
	{0x1618,0x000A0046},	{0x1618,0x000F002D},	{0x1618,0x00190019},	{0x1618,0x0023000A},
	{0x1618,0x00280000},	{0x1618,0x00460000},	{0x1618,0x005A002D},	{0x1618,0x006E005A},
	{0x1618,0x00A0007F},	{0x1618,0x00B4007F},	{0x1618,0x00C8005A},	{0x1618,0x00DC002D},
	{0x1618,0x00FF0000},	{0x1618,0x00320000},	{0x1618,0x00460014},	{0x1618,0x005A0032},
	{0x1618,0x0078005A},	{0x1618,0x0090007F},	{0x1618,0x00B4007F},	{0x1618,0x00D8007F},
	{0x1618,0x00FF0000},	{0x1618,0x00320000},	{0x1618,0x00460014},	{0x1618,0x005A0032},
	{0x1618,0x0078005A},	{0x1618,0x0090007F},	{0x1618,0x00B4007F},	{0x1618,0x00D8007F},
	{0x1618,0x00FF0000},	{0x1618,0x00320000},	{0x1618,0x00460014},	{0x1618,0x005A0032},
	{0x1618,0x0078005A},	{0x1618,0x0090007F},	{0x1618,0x00B4007F},	{0x1618,0x00D8007F},
	{0x1618,0x00FF0000},	{0x1618,0x00320000},	{0x1618,0x00460014},	{0x1618,0x005A0032},
	{0x1618,0x0078005A},	{0x1618,0x0090007F},	{0x1618,0x00B4007F},	{0x1618,0x00D8007F},
	{0x1618,0x00FF0000},	{0x1618,0x0000007F},	{0x1618,0x0024007F},	{0x1618,0x0048007F},
	{0x1618,0x006C007F},	{0x1618,0x0090007F},	{0x1618,0x00B4007F},	{0x1618,0x00D8007F},
	{0x1618,0x00FF007F},	{0x1618,0x00320000},	{0x1618,0x00460014},	{0x1618,0x005A0032},
	{0x1618,0x0078005A},	{0x1618,0x0090007F},	{0x1618,0x00B4007F},	{0x1618,0x00D8007F},
	{0x1618,0x00FF0000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},

	/*---	CEN - DBG_CO	---*/
	{0x1614,0x00001300},	{0x1618,0x0000002A},	{0x1618,0x00000000},	{0x1618,0x0000001A},
	{0x1618,0x00000028},	{0x1618,0x0000000A},	{0x1618,0x00000002},	{0x1618,0x00000021},
	{0x1618,0x0000000A},	{0x1618,0x00000008},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},

	/*---	CEN - GAIN_DELTA	---*/
	{0x1614,0x00001400},	{0x1618,0x0000EC00},	{0x1618,0x00F60A00},	{0x1618,0x000000E2},
	{0x1618,0x00000000},	{0x1618,0x00000F00},	{0x1618,0x00000000},	{0x1618,0x001E1E00},
	{0x1618,0x000500FB},	{0x1618,0x001E1E00},	{0x1618,0x00FB05FB},	{0x1618,0x00001E00},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x0005FB05},	{0x1618,0x00001400},
	{0x1618,0x00FB0505},	{0x1618,0x001E1E00},	{0x1618,0x00FB05FB},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},

	/*---	CEN - GAIN_MASTER	---*/
	{0x1614,0x00001500},	{0x1618,0x000000FF},	{0x1618,0x000000FF},	{0x1618,0x00000080},
	{0x1618,0x000000FF},	{0x1618,0x000000FF},	{0x1618,0x00000080},	{0x1618,0x000000FF},
	{0x1618,0x000000FF},	{0x1618,0x000000FF},	{0x1618,0x00000080},	{0x1618,0x00000080},
	{0x1618,0x00000080},	{0x1618,0x00000080},	{0x1618,0x00000080},	{0x1618,0x00000080},
	{0x1618,0x00000080},

	/*---	CEN - GAIN_G_DELTA	---*/
	{0x1614,0x00001600},    {0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},
	{0x1618,0x00000000},	{0x1618,0x00000000},	{0x1618,0x00000000},

	/*---	CEN - Normal Mode	---*/
	{0x1614,0x00008000},	{0x1604,0x00000000},	{0x1600,0x807800BF}    
};

const PE_REG_PARAM_T cmg_r_init_param_l9a0[] =
{   
	/*---	CEN - GAIN_H	---*/
	{0x3614,0x00001000},	{0x3618,0x0000007F},	{0x3618,0x0048007F},	{0x3618,0x0094007F},
	{0x3618,0x00DD007F},	{0x3618,0x0127007F},	{0x3618,0x0171007F},	{0x3618,0x01BB007F},
	{0x3618,0x01FE007F},	{0x3618,0x0000007F},	{0x3618,0x0048007F},	{0x3618,0x0094007F},
	{0x3618,0x00DD007F},	{0x3618,0x0127007F},	{0x3618,0x0171007F},	{0x3618,0x01BB007F},
	{0x3618,0x01FF007F},	{0x3618,0x01FE0000},	{0x3618,0x0209002D},	{0x3618,0x0215005A},
	{0x3618,0x021F007F},	{0x3618,0x0230007F},	{0x3618,0x023A005A},	{0x3618,0x0244002D},
	{0x3618,0x02510000},	{0x3618,0x005C0000},	{0x3618,0x0072002D},	{0x3618,0x0087005A},
	{0x3618,0x0095007F},	{0x3618,0x00B6007F},	{0x3618,0x00C4005A},	{0x3618,0x00D6002D},
	{0x3618,0x00E30000},	{0x3618,0x00F10000},	{0x3618,0x00F8003C},	{0x3618,0x0100006E},
	{0x3618,0x010E007F},	{0x3618,0x0131007F},	{0x3618,0x0141006E},	{0x3618,0x0159003C},
	{0x3618,0x01710000},	{0x3618,0x01B80000},	{0x3618,0x01D4002D},	{0x3618,0x01E3005A},
	{0x3618,0x01F1007F},	{0x3618,0x020E007F},	{0x3618,0x021C005A},	{0x3618,0x0229002D},
	{0x3618,0x02400000},	{0x3618,0x00430000},	{0x3618,0x0047002D},	{0x3618,0x004E005A},
	{0x3618,0x0055007F},	{0x3618,0x005C007F},	{0x3618,0x0063005A},	{0x3618,0x006A002D},
	{0x3618,0x00720000},	{0x3618,0x018D0000},	{0x3618,0x0195002D},	{0x3618,0x019C005A},
	{0x3618,0x01A3007F},	{0x3618,0x01AA007F},	{0x3618,0x01B1005A},	{0x3618,0x01B8002D},
	{0x3618,0x01C10000},	{0x3618,0x00D50000},	{0x3618,0x00DC003C},	{0x3618,0x00E3006E},
	{0x3618,0x00EA007F},	{0x3618,0x00F1007F},	{0x3618,0x00F8006E},	{0x3618,0x00FD003C},
	{0x3618,0x01000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},

	/*---	CEN - GAIN_S	---*/
	{0x3614,0x00001100},	{0x3618,0x0000007F},	{0x3618,0x0002007F},	{0x3618,0x00050078},
	{0x3618,0x000A0073},	{0x3618,0x000C006E},	{0x3618,0x0013005A},	{0x3618,0x00190028},
	{0x3618,0x00200000},	{0x3618,0x0000007F},	{0x3618,0x0006007F},	{0x3618,0x000C007F},
	{0x3618,0x0033007F},	{0x3618,0x004C006E},	{0x3618,0x00590064},	{0x3618,0x0066003C},
	{0x3618,0x007F0000},	{0x3618,0x001C0000},	{0x3618,0x0022003C},	{0x3618,0x00270064},
	{0x3618,0x002C007F},	{0x3618,0x0039006E},	{0x3618,0x00460046},	{0x3618,0x00530014},
	{0x3618,0x00600000},	{0x3618,0x00140000},	{0x3618,0x00260028},	{0x3618,0x0039005A},
	{0x3618,0x004C007F},	{0x3618,0x0059007F},	{0x3618,0x0066007F},	{0x3618,0x0073007F},
	{0x3618,0x007F007F},	{0x3618,0x00140000},	{0x3618,0x00260028},	{0x3618,0x0039005A},
	{0x3618,0x0053007F},	{0x3618,0x0059007F},	{0x3618,0x0066007F},	{0x3618,0x0073007F},
	{0x3618,0x007F007F},	{0x3618,0x00590000},	{0x3618,0x005E001E},	{0x3618,0x00630046},
	{0x3618,0x006A0064},	{0x3618,0x006F0073},	{0x3618,0x0074007F},	{0x3618,0x0078007F},
	{0x3618,0x007F007F},	{0x3618,0x00200000},	{0x3618,0x002C0028},	{0x3618,0x003D005A},
	{0x3618,0x004C007F},	{0x3618,0x0059007F},	{0x3618,0x0066007F},	{0x3618,0x0073007F},
	{0x3618,0x007F007F},	{0x3618,0x00140000},	{0x3618,0x00260028},	{0x3618,0x0039005A},
	{0x3618,0x004C007F},	{0x3618,0x0059007F},	{0x3618,0x0066007F},	{0x3618,0x0073007F},
	{0x3618,0x007F007F},	{0x3618,0x00140000},	{0x3618,0x00260028},	{0x3618,0x0039005A},
	{0x3618,0x0053007F},	{0x3618,0x0059007F},	{0x3618,0x0066007F},	{0x3618,0x0073007F},	
	{0x3618,0x007F007F},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},

	/*---	CEN - GAIN_L	---*/
	{0x3614,0x00001200},	{0x3618,0x00640000},	{0x3618,0x00780012},	{0x3618,0x008C0024},
	{0x3618,0x00A00036},	{0x3618,0x00B40048},	{0x3618,0x00C8005A},	{0x3618,0x00DC006C},
	{0x3618,0x00FF007F},	{0x3618,0x0000007F},	{0x3618,0x00030078},	{0x3618,0x00050064},
	{0x3618,0x000A0046},	{0x3618,0x000F002D},	{0x3618,0x00190019},	{0x3618,0x0023000A},
	{0x3618,0x00280000},	{0x3618,0x00460000},	{0x3618,0x005A002D},	{0x3618,0x006E005A},
	{0x3618,0x00A0007F},	{0x3618,0x00B4007F},	{0x3618,0x00C8005A},	{0x3618,0x00DC002D},
	{0x3618,0x00FF0000},	{0x3618,0x00320000},	{0x3618,0x00460014},	{0x3618,0x005A0032},
	{0x3618,0x0078005A},	{0x3618,0x0090007F},	{0x3618,0x00B4007F},	{0x3618,0x00D8007F},
	{0x3618,0x00FF0000},	{0x3618,0x00320000},	{0x3618,0x00460014},	{0x3618,0x005A0032},
	{0x3618,0x0078005A},	{0x3618,0x0090007F},	{0x3618,0x00B4007F},	{0x3618,0x00D8007F},
	{0x3618,0x00FF0000},	{0x3618,0x00320000},	{0x3618,0x00460014},	{0x3618,0x005A0032},
	{0x3618,0x0078005A},	{0x3618,0x0090007F},	{0x3618,0x00B4007F},	{0x3618,0x00D8007F},
	{0x3618,0x00FF0000},	{0x3618,0x00320000},	{0x3618,0x00460014},	{0x3618,0x005A0032},
	{0x3618,0x0078005A},	{0x3618,0x0090007F},	{0x3618,0x00B4007F},	{0x3618,0x00D8007F},
	{0x3618,0x00FF0000},	{0x3618,0x0000007F},	{0x3618,0x0024007F},	{0x3618,0x0048007F},
	{0x3618,0x006C007F},	{0x3618,0x0090007F},	{0x3618,0x00B4007F},	{0x3618,0x00D8007F},
	{0x3618,0x00FF007F},	{0x3618,0x00320000},	{0x3618,0x00460014},	{0x3618,0x005A0032},
	{0x3618,0x0078005A},	{0x3618,0x0090007F},	{0x3618,0x00B4007F},	{0x3618,0x00D8007F},
	{0x3618,0x00FF0000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},

	/*---	CEN - DBG_CO	---*/
	{0x3614,0x00001300},	{0x3618,0x0000002A},	{0x3618,0x00000000},	{0x3618,0x0000001A},
	{0x3618,0x00000028},	{0x3618,0x0000000A},	{0x3618,0x00000002},	{0x3618,0x00000021},
	{0x3618,0x0000000A},	{0x3618,0x00000008},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},

	/*---	CEN - GAIN_DELTA	---*/
	{0x3614,0x00001400},	{0x3618,0x0000EC00},	{0x3618,0x00F60A00},	{0x3618,0x000000E2},
	{0x3618,0x00000000},	{0x3618,0x00000F00},	{0x3618,0x00000000},	{0x3618,0x001E1E00},
	{0x3618,0x000500FB},	{0x3618,0x001E1E00},	{0x3618,0x00FB05FB},	{0x3618,0x00001E00},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x0005FB05},	{0x3618,0x00001400},
	{0x3618,0x00FB0505},	{0x3618,0x001E1E00},	{0x3618,0x00FB05FB},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},

	/*---	CEN - GAIN_MASTER	---*/	
	{0x3614,0x00001500},	{0x3618,0x000000FF},	{0x3618,0x000000FF},	{0x3618,0x00000080},
	{0x3618,0x000000FF},	{0x3618,0x000000FF},	{0x3618,0x00000080},	{0x3618,0x000000FF},
	{0x3618,0x000000FF},	{0x3618,0x000000FF},	{0x3618,0x00000080},	{0x3618,0x00000080},
	{0x3618,0x00000080},	{0x3618,0x00000080},	{0x3618,0x00000080},	{0x3618,0x00000080},
	{0x3618,0x00000080},

	/*---	CEN - GAIN_G_DELTA	---*/	
	{0x3614,0x00001600},    {0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},
	{0x3618,0x00000000},	{0x3618,0x00000000},	{0x3618,0x00000000},

	/*---	CEN - Normal Mode	---*/	
	{0x3614,0x00008000},	{0x3604,0x00000000},	{0x3600,0x807800BF}      
};  
    
#ifdef	__cplusplus
}   
#endif /* __cplusplus */
    
#endif /* _PE_CMG_COEFF_H_ */
