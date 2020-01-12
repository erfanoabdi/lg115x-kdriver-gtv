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

#ifndef _ADEC_INTERNAL_BUFFER_H_
#define _ADEC_INTERNAL_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif
int InternalBuffer_GetStructSize(int _indexCount);
int InternalBuffer_CreateBuffer(
	unsigned char* _baseAddr, 
	int _bufferSize, 
	int _maxAuSize, 
	int _indexCount,
	void* _strcut);

#ifdef __cplusplus
}
#endif

#endif
