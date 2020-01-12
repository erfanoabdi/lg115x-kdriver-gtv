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
 *  Brief description.
 *  Detailed description starts here.
 *
 *  @author		raxis
 *  @version	1.0
 *  @date		2010-01-21
 *  @note		Additional information.
 */

/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	File Inclusions
----------------------------------------------------------------------------------------*/
#include "simple_lib.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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

/*========================================================================================
	Implementation Group
========================================================================================*/

void	HexDump	( UINT32 address, UINT8* data, UINT32 len )
{
    if (len > 0)
    {
        int width = 16;
        char *str = data;
        int j, i = 0;

        char buf_ptr[100];
        int  buf_len = 0;

		printf("--------------------------------------------------------------------------------\n");
		printf("offset      00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F  0123456789ABCDEF\n");
		printf("--------------------------------------------------------------------------------\n");

        while (i < len)
        {
            buf_len = 0;
            buf_len += sprintf( buf_ptr + buf_len,"0x%08x  ", i+address);

            if (1)
            {
                for (j = 0; j < width; j++) {
                    if (i+j < len)
                    {
                        buf_len += sprintf( buf_ptr + buf_len,"%02x ", (unsigned char)str[j]);
                    }
                    else
                    {
                        buf_len += sprintf( buf_ptr + buf_len,"%s", "   ");
                    }

                    if ((j+1) % (width/2) == 0)
                    buf_len += sprintf( buf_ptr + buf_len,"%s", " ");
                }
            }

            for (j = 0; j < width; j++)
            {
                // 출력할 수 없는 문자의 경우 '.' 으로 출력
                if (i+j < len)
                    buf_len += sprintf( buf_ptr + buf_len, "%c", isprint(str[j])?str[j]:'.');
                else
                    buf_len += sprintf( buf_ptr + buf_len, "%s", " ");
            }

            str += width;
            i += j;

            printf("%s\n", buf_ptr );
        }
    }
}