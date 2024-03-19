/*
*   file name:  link_list.c
*   given the operation of link list
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "link_list.h"

#define LISTINC(plong)			(plong++)
#define LISTDEC(plong)			(plong--)

void slListInit( slListHead* pHead )
{
	if( pHead != NULL )
		memset( pHead, 0, sizeof( slListHead ) );
}
void* slListGetHeapTop( slListHead* pHead, void* pDefaultData )
{
	if( pHead == NULL || pHead->nLen == 0 ) return pDefaultData;
	return pHead->pHead->pData;
}
int slListPush( slListHead* pHead, void* pData )
{
	LPslListNod pNewNod;
	if( pHead == NULL ) return 0;
	pNewNod = ( LPslListNod )malloc( sizeof( slListNod ) );
	if( pNewNod == NULL ) return 0;
	pNewNod->pData = pData;
	pNewNod->pNext = pHead->pHead;
	pHead->pHead = pNewNod;
	LISTINC( pHead->nLen );
	return 1;
}
void* slListPop( slListHead* pHead, void* pDefaultData )
{
	LPslListNod pNod;
	void* pPopData;
	if( pHead == NULL || pHead->nLen == 0 ) return pDefaultData;
	pNod = pHead->pHead;
	pPopData = pNod->pData;
	pHead->pHead = pNod->pNext;
	free( pNod );
	LISTDEC( pHead->nLen );
	return pPopData;
}
void* slListGetFront( slListHead* pHead, void* pDefaultData )
{
	return slListGetHeapTop( pHead, pDefaultData );
}
void* slListGetRear( slListHead* pHead, void* pDefaultData )
{
	LPslListNod pNod;
	if( pHead == NULL || pHead->nLen == 0 ) return pDefaultData;
	pNod = pHead->pHead;
	while( pNod->pNext != NULL ) pNod =  pNod->pNext;
	return pNod->pData;
}
int slListEnq( slListHead* pHead, void* pData )
{
	LPslListNod* ppNod;
	LPslListNod pNewNod;
	if( pHead == NULL ) return 0;
	ppNod = &( pHead->pHead );
	while( *ppNod != NULL ) ppNod = &( (*ppNod)->pNext );
	pNewNod = ( LPslListNod )malloc( sizeof( slListNod ) );
	if( pNewNod == NULL ) return 0;
	pNewNod->pData = pData;
	pNewNod->pNext = NULL;
	*ppNod = pNewNod;
	LISTINC( pHead->nLen );
	return 1;
}
void* slListDeq( slListHead* pHead, void* pDefaultData )
{
	return slListPop( pHead, pDefaultData );
}
int slListJumpq( slListHead* pHead, void* pData )
{
	return slListPush( pHead, pData );
}
void* slListGetAt( slListHead* pHead, int nIndex, void* pDefaultData )
{
	LPslListNod* ppNod;
	int nCount = 0;
	if( pHead == NULL || ( nIndex < 0 || nIndex > (int)pHead->nLen ) )
		return pDefaultData;
	ppNod = &( pHead->pHead );
	while( *ppNod != NULL )
	{
		if( nIndex == nCount++ )
			break;
		ppNod = &( (*ppNod)->pNext );
	}
	return ( ( *ppNod == NULL ) ? pDefaultData : (*ppNod)->pData );
}
void slListDeleteAt( slListHead* pHead, int nIndex )
{
	LPslListNod* ppNod;
	int nCount = 0;
	if( pHead == NULL || ( nIndex < 0 || nIndex > (int)pHead->nLen ) )
		return;
	ppNod = &( pHead->pHead );
	while( *ppNod != NULL )
	{
		if( nIndex == nCount++ )
			break;
		ppNod = &( (*ppNod)->pNext );
	}
	if( *ppNod != NULL )
	{
		LPslListNod pDeleteNod = *ppNod;
		*ppNod = pDeleteNod->pNext;
		free( pDeleteNod );
		LISTDEC( pHead->nLen );
	}
}
void slListAddAt( slListHead* pHead, int nIndex, void* pData )
{
	LPslListNod* ppNod;
	LPslListNod pNewNod;
	int nCount = 0;
	if( pHead == NULL || ( nIndex < 0 || nIndex > (int)pHead->nLen ) )
		return;
	ppNod = &( pHead->pHead );
	while( *ppNod != NULL )
	{
		if( nIndex == nCount++ )
			break;
		ppNod = &( (*ppNod)->pNext );
	}
	pNewNod = ( LPslListNod )malloc( sizeof( slListNod ) );
	if( pNewNod == NULL ) return;
	pNewNod->pData = pData;
	pNewNod->pNext = *ppNod;
	*ppNod = pNewNod;
	LISTINC( pHead->nLen );
}
void slListDelete( slListHead* pHead, void* pData )
{
	LPslListNod pDeleteNod;
	LPslListNod* ppNod;
	if( pHead == NULL || pHead->nLen == 0  )
		return;
	ppNod = &( pHead->pHead );
	while( *ppNod != NULL )
	{
		if( (*ppNod)->pData == pData )
			break;
		ppNod = &( (*ppNod)->pNext );
	}
	if( *ppNod != NULL )
	{
		pDeleteNod = *ppNod;
		*ppNod = pDeleteNod->pNext;
		free( pDeleteNod );
		LISTDEC( pHead->nLen );
	}
}
int slListGetLength( slListHead* pHead )
{
	return ( ( pHead == NULL ) ? -1 : (int)pHead->nLen );
}

void slListReMoveAll( slListHead* pHead )
{
	LPslListNod pDeleteNod;
	LPslListNod pNod;
	if( pHead == NULL || pHead->nLen == 0  )
		return;
	pNod = pHead->pHead;
	while( pNod != NULL )
	{
		pDeleteNod = pNod;
		pNod = pNod->pNext;
		free( pDeleteNod );
	}
	pHead->pHead = NULL;
	pHead->nLen = 0;
}
slListNod* slListGetFistNod( slListHead* pHead )
{
	return ( ( pHead == NULL ) ? NULL : pHead->pHead );
}
slListNod* slListGetNextNod( slListNod* pNod )
{
	return ( ( pNod == NULL ) ? NULL : pNod->pNext );
}

int slListIsHaveData( slListHead* pHead, void* pData )
{

	LPslListNod pNod;
	if( pHead == NULL || pHead->nLen == 0  )
		return 0;
	pNod = pHead->pHead;
	while( pNod != NULL )
	{
		if( pNod->pData == pData )
			return 1;
		pNod = pNod->pNext;
	}
	return 0;
}
