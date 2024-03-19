/*
*   file name:  link_list.h
*   give the opration of link list
*/

#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	typedef struct tagslList{
		void* pData;
		struct tagslList* pNext;
	}slListNod,*LPslListNod;
	
	typedef struct tsgslListHead{
		LPslListNod pHead;
		long nLen;
	}slListHead,*LPslListHead;

	//无锁
	void slListInit( slListHead* pHead );								//初始化
	void* slListGetHeapTop( slListHead* pHead, void* pDefaultData );	//获取栈顶
	int slListPush( slListHead* pHead, void* pData );					//入栈
	void* slListPop( slListHead* pHead, void* pDefaultData );			//出栈
	void* slListGetFront( slListHead* pHead, void* pDefaultData );		//获取队头
	void* slListGetRear( slListHead* pHead, void* pDefaultData );		//获取队尾
	int slListEnq( slListHead* pHead, void* pData );					//入队
	void* slListDeq( slListHead* pHead, void* pDefaultData );			//出队
	int slListJumpq( slListHead* pHead, void* pData );					//插队到队头

	void* slListGetAt( slListHead* pHead, int nIndex, void* pDefaultData );	//获取第nIndex个数据
	void slListDeleteAt( slListHead* pHead, int nIndex );
	void slListAddAt( slListHead* pHead, int nIndex, void* pData );
	void slListDelete( slListHead* pHead, void* pData );
	int slListGetLength( slListHead* pHead );
	void slListReMoveAll( slListHead* pHead );
	slListNod* slListGetFistNod( slListHead* pHead );
	slListNod* slListGetNextNod( slListNod* pHead );
	int slListIsHaveData( slListHead* pHead, void* pData );					//列表中是否存在数据

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  /* __LINK_LIST_H__ */

