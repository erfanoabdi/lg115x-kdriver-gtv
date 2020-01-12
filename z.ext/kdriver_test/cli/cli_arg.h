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

#ifndef _CLI_ARG_h
#define _CLI_ARG_h

#include "base_types.h"

#define START_ARGS      	 1
#define A2I                 s2i(argv[nArg++])
#define A2U                 s2u(argv[nArg++])
#define A2X                 s2x(argv[nArg++])
#define A2H                 s2h(argv[nArg++])
#define A2S                 (argv[nArg++])

#define uA2I(ord)           s2i(argv[nArg+ord])
#define uA2U(ord)           s2u(argv[nArg+ord])
#define uA2X(ord)           s2x(argv[nArg+ord])
#define uA2H(ord)           s2h(argv[nArg+ord])
#define uA2S(ord)           (argv[nArg+ord])

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define e2str(val) {(unsigned int)val, #val}

typedef struct {
	unsigned int val;
	char  *str;
} ENUM2STR_T;

typedef struct {
	ENUM2STR_T *e2s;
	unsigned int     size;
} CLI2ARGS_T;

BOOLEAN isXDigit(char *pString);
SINT32 s2i(char *pString);
unsigned int s2u(char *pString);
unsigned int s2x(char *pString);
unsigned int s2h(char *pString);

#endif
