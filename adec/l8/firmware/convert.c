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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE_NUM 128
void main(int argc, char *argv[])
{
	int ret = 0;
	FILE *fdFile;
	char pszStr[MAX_LINE_NUM], chTemp[12];
	char *p;

	if (argc == 1) {
		printf("usage: ./convert [filename]\n");
		return;
	}

	if (strcmp(argv[1], "-h") == 0) {
		printf("usage: ./convert [filename]\n");
		return;
	}

	fdFile = fopen(argv[1], "r+t");
	if (fdFile == NULL) {
		printf("Can't Open file %s\n", argv[1]);
		return;
	}

	do {
//		memset(pszStr, 0, MAX_LINE_NUM);
		p = fgets(pszStr, MAX_LINE_NUM, fdFile);
//		printf("%s ,(%d)\n", pszStr, sizeof(pszStr));
		if (p != NULL) {
			if (pszStr[0] == '0' && pszStr[1] == 'x' && pszStr[10] == ',') {
				memcpy(chTemp, pszStr, 12);
				chTemp[2] = pszStr[8];
				chTemp[3] = pszStr[9];
				chTemp[4] = pszStr[6];
				chTemp[5] = pszStr[7];
				chTemp[6] = pszStr[4];
				chTemp[7] = pszStr[5];
				chTemp[8] = pszStr[2];
				chTemp[9] = pszStr[3];
//				printf("\t %s \n", chTemp);
				fseek(fdFile, -13, SEEK_CUR);
				fwrite(chTemp, 12, 1, fdFile);
			}
			else {
//				fwrite(pszStr, sizeof(pszStr), 1, fdFile);
			}
		}
	} while(p != NULL);

	fclose(fdFile);
	printf("Success!\n");
}
