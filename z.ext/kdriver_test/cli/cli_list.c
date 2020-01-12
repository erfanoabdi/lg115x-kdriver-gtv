#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "base_types.h"

#include "dbg/dbg_def.h"
#include "cli/cli_list.h"

/**
 * initailize list
 * make dummy header
 *
 * @see LIST_T
 */
LIST_T *InitList( void )
{
	LIST_T	*pList;

	pList = (LIST_T *)malloc(sizeof(LIST_T));

	if ( pList )
	{
		pList->pNext = pList;
	}

	return pList;
}


/* add item to list
 * add an item to the end of the list
 *
 * @param in: pList target list
 * @param in: pItem item to be added
 */
int AddItemToList(LIST_T *pList, LIST_T *pItem)
{
	int ret = RET_OK;
	LIST_T		 *pLast;

	if ( pList && pItem )
	{
		pLast = pList;
		while ( pLast->pNext != pList )
		{
			pLast = pLast->pNext;
		}

		pLast->pNext = pItem;
		pItem->pNext = pList;
	}
	else
	{
		ret = RET_ERROR;
	}

	return ret;
}


/**
 * delete element from list
 * remove an item from the list and free its memory
 *
 * @param in: pList previous item of the seleted item
 * @param in: pItem item to be deleted
 */
int DeleteItemFromList(LIST_T *pList, LIST_T *pItem)
{
	int ret = RET_OK;

	if ( pList && pItem )
	{
		pList->pNext = pItem->pNext;

		free((void *)(pItem->pData));
		free((void *)pItem);
	}
	else
	{
		ret = RET_ERROR;
	}

	return ret;
}

int FindItemWithScript(LIST_T *pList, char *pScript)
{
	int ret = RET_ERROR;
	LIST_T		 *pLast;

	if ( pList && pScript )
	{
		pLast = pList;
		while ( pLast->pNext != pList )
		{
			pLast = pLast->pNext;
			if (strcmp((char*)pLast->pData, pScript)) continue;
			ret = RET_OK;
			break;
		}
	}
	else
	{
		ret = RET_ERROR;
	}

	return ret;
}

int AddScriptToList(LIST_T *pList, char *runFileName)
{
	int ret = RET_OK;
	LIST_T		 *pItem = NULL;
	LIST_T		 *pLast;

	if ( pList && runFileName )
	{
		pLast = pList;
		while ( pLast->pNext != pList )
		{
			pLast = pLast->pNext;
			if (strcmp((char*)pLast->pData, runFileName)) continue;
			printf("Script %s is Recursevely called\n", runFileName);
			ret = RET_ERROR;
			break;
		}

		if (ret != RET_OK) {
			pLast = pList;
			while ( pLast->pNext != pList )
			{
				pLast = pLast->pNext;
				printf("%s => ", (char*)pLast->pData);
			}
			printf("%s\n", (char*)runFileName);
		} else {
			pItem = (LIST_T *)malloc(sizeof(pItem));
			if (pItem) {
				pItem->pData = (void *)strdup(runFileName);

				pLast->pNext = pItem;
				pItem->pNext = pList;
			} else {
				ret = RET_ERROR;
				ffprintf("No Memory for adding %s - %d\n", runFileName, sizeof(LIST_T));
			}
		}
	}

	return ret;
}

int DeleteScriptFromList(LIST_T *pList, char *pScript)
{
	int ret = RET_ERROR;
	LIST_T		 *pLast;
	LIST_T		 *pCurr;

	if ( pList && pScript )
	{
		pLast = pList;
		while ( pLast->pNext != pList )
		{
			pCurr = pLast;
			pLast = pLast->pNext;
			if (strcmp((char*)pLast->pData, pScript)) continue;

			pCurr->pNext = pLast->pNext;
			free((void *)(pLast->pData));
			free((void *)pLast);
			ret = RET_OK;
			break;
		}
		if (ret != RET_OK) {
			ffprintf("There's not Script %s\n", pScript);
		}
	}
	else
	{
		ffprintf("There's Null Item %s\n", pScript);
		ret = RET_ERROR;
	}

	return ret;
}

