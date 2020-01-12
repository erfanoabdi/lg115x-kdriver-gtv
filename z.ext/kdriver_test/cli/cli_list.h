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

#ifndef _CLI_LIST_h
#define _CLI_LIST_h

/**
 * list item
 * singly linked list
 */
typedef struct _LIST
{
	struct _LIST *pNext;
	void 		 *pData;

} LIST_T;

LIST_T *InitList( void );
int AddItemToList(LIST_T *pList, LIST_T *pItem);
int DeleteItemFromList(LIST_T *pList, LIST_T *pItem);
int FindItemWithScript(LIST_T *pList, char *pScript);
int AddScriptToList(LIST_T *pList, char *runFileName);
int DeleteScriptFromList(LIST_T *pList, char *pScript);

#endif
