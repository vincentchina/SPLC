#include "dataMisc.h"
#include "MemManager.h"

//base stack
api_res EmptyStack( LPStack pStack )
{
	if( pStack == NULL  )
		return RES_ERR;
	pStack->head = 0;
	//pStack->sz = sz;
	pStack->tail = -1;
	return RES_OK;
}

//bit stack
api_res BitStackPush( LPStack pStack, mbyte b )
{
	if( pStack->tail + 1 >= pStack->sz )
		return RES_ERR;
	++pStack->tail;
	b ? SETBIT( pStack->pBuf, pStack->tail ) : CLRBIT( pStack->pBuf, pStack->tail );
	return RES_OK;
}

mbyte BitStackRead( LPStack pStack )
{
	return GETBIT( pStack->pBuf, pStack->tail );
}

//word stack
api_res WordStackPush( LPStack pStack, uint16 b )
{
	if( pStack->tail + 1 >= pStack->sz )
		return RES_ERR;
	pStack->pBuf[++pStack->tail] = b;
	return RES_OK;
}

uint16 WordStackRead( LPStack pStack )
{
	if( pStack->tail < 0 )
		return 0;
	return pStack->pBuf[pStack->tail];
}

//double word stack
api_res DWordStackPush( LPStack pStack, uint32 b )
{
	if( pStack->tail + 1 >= pStack->sz )
		return RES_ERR;
	( (uint32*)(pStack->pBuf) )[++pStack->tail] = b;
	return RES_OK;
}

uint32 DWordStackRead( LPStack pStack )
{
	if( pStack->tail < 0 )
		return 0;
	return ( (uint32*)(pStack->pBuf) )[pStack->tail];
}

//¶ÓÁĞ
api_res EmptyQueue( LPQueue pQueue )
{
	if( pQueue == NULL )
		return RES_ERR;
	//pQueue->sz = sz;
	pQueue->head = 0;
	pQueue->tail = 0;
	return RES_OK;
}

api_res BitEnQueue( LPQueue pQueue, mbyte b )
{
	if( QueueIsFull( pQueue ) )
		return RES_ERR;
	b ? SETBIT( pQueue->pBuf, pQueue->tail ) : CLRBIT( pQueue->pBuf, pQueue->tail );
	++pQueue->tail;
	if( pQueue->tail >= pQueue->sz )
		pQueue->tail = 0;
	return RES_OK;
}

mbyte BitDeQueue( LPQueue pQueue )
{
	mbyte b;
	if( QueueIsEmpyt( pQueue ) )
		return 0;
	b = GETBIT( pQueue->pBuf,pQueue->head );
	++pQueue->head;
	if( pQueue->head >= pQueue->sz )
		pQueue->head = 0;
	return b;
}

api_res DWordEnQueue( LPQueue pQueue, uint32 d )
{
	if( QueueIsFull( pQueue ) )
		return RES_ERR;
	( (uint32*)pQueue->pBuf )[pQueue->tail] = d;
	++pQueue->tail;
	if( pQueue->tail >= pQueue->sz )
		pQueue->tail = 0;
	return RES_OK;
}

uint32 DWordDeQueue( LPQueue pQueue )
{
	uint32 d;
	if( QueueIsEmpyt( pQueue ) )
		return 0;
	d = ((uint32*)pQueue->pBuf)[pQueue->head];
	++pQueue->head;
	if( pQueue->head >= pQueue->sz )
		pQueue->head = 0;
	return d;
}
