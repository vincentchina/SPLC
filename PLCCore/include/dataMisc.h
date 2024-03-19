#ifndef __MISC_H__
#define __MISC_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "Device.h"

typedef struct tagCircleBuf{
	uint16* pBuf;
	int sz;
	int head;
	int tail;
}CircleBuf,*LPCircleBuf;

typedef CircleBuf Stack,*LPStack;
//typedef CircleBuf BitStack,*LPBitStack;
typedef CircleBuf Queue,*LPQueue;

//base stack
api_res EmptyStack( LPStack pStack );
#define StackPop( pStack ) { if( ( pStack )->tail >= 0 ) --( pStack )->tail; }
#define IsStackEmpty( pStack ) ( ( pStack )->tail < 0 )
#define IsStatckFull( pStack ) ( ( pStack )->tail + 1 >= ( pStack )->sz )

//bit stack
api_res BitStackPush( LPStack pStack, mbyte b );
mbyte BitStackRead( LPStack pStack );
//word stack
api_res WordStackPush( LPStack pStack, uint16 b );
uint16 WordStackRead( LPStack pStack );
//double word stack
api_res DWordStackPush( LPStack pStack, uint32 b );
uint32 DWordStackRead( LPStack pStack );

//base queue
api_res EmptyQueue( LPQueue pStack );
#define QueueIsEmpyt( pQueue ) ( ( pQueue )->head == ( pQueue )->tail )
#define QueueIsFull( pQueue ) ( ( pQueue )->tail + 1 == ( pQueue )->head )

//bit queue
api_res BitEnQueue( LPQueue pStack, mbyte b );
mbyte BitDeQueue( LPQueue pStack );

//double word queue
api_res DWordEnQueue( LPQueue pQueue, uint32 d );
uint32 DWordDeQueue( LPQueue pQueue );

#ifdef __cplusplus
}
#endif
#endif
