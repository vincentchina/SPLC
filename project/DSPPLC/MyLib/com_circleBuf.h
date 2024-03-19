
#ifndef __COM_CIRCLEBUF_H__
#define __COM_CIRCLEBUF_H__

typedef struct tagCIRCLEBUF{
	unsigned int* 	pbuf;
	unsigned int   	lenBuf;
	unsigned int   	front;
	unsigned int   	rear;
}CIRCLEBUF,*LPCIRCLEBUF;

__inline int CBuf_init( LPCIRCLEBUF pcBuf, unsigned int* pBuf, unsigned int szEle, unsigned int lenBuf )
{
	pcBuf->pbuf = pBuf;
	pcBuf->lenBuf = lenBuf;
	pcBuf->front = 0;
	pcBuf->rear = 0;
	return 1;
}

__inline void CBuf_unInit( LPCIRCLEBUF pcBuf )
{
	pcBuf->pbuf = 0;
}

__inline int CBuf_IsInit( LPCIRCLEBUF pcBuf )
{
	return ( pcBuf->pbuf != 0 );
}

__inline unsigned int CBuf_IncIndex( LPCIRCLEBUF pcBuf, unsigned int uIndex )
{
	return ( ( ++uIndex >= pcBuf->lenBuf ) ? 0 : uIndex );
}

__inline int CBuf_IsFull( LPCIRCLEBUF pcBuf )
{
	return ( CBuf_IncIndex( pcBuf, pcBuf->front ) == pcBuf->rear );
}

__inline int CBuf_IsEmpty( LPCIRCLEBUF pcBuf )
{
	return ( pcBuf->rear == pcBuf->front );
}

__inline unsigned int CBuf_GetLength( LPCIRCLEBUF pcBuf )
{
	if( pcBuf->front >= pcBuf->rear ) return pcBuf->front - pcBuf->rear;
	else	return ( pcBuf->lenBuf - pcBuf->rear + pcBuf->front );
}

__inline int CBuf_Enqueue( LPCIRCLEBUF pcBuf, unsigned int data )
{
	if( CBuf_IsFull( pcBuf ) )	return 0;
	pcBuf->pbuf[ pcBuf->front ] = data;
	pcBuf->front = CBuf_IncIndex( pcBuf, pcBuf->front );
	return 1;
}

__inline unsigned int CBuf_Dequeue( LPCIRCLEBUF pcBuf )
{
	unsigned int data;
	if( CBuf_IsEmpty( pcBuf )) return 0;
	data = pcBuf->pbuf[ pcBuf->rear ];
	pcBuf->rear = CBuf_IncIndex( pcBuf, pcBuf->rear );
	return data;
}

#endif

