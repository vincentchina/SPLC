#ifndef __BASEDEV_H__
#define __BASEDEV_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "dllpreHead.h"

typedef void (*pRevCallBack)( const unsigned char* buf, int sz, void* userData );

class LIBX_API CBaseDev
{
public:
	int m_bErr;
protected:
	pRevCallBack m_revCallBack;
	void* m_pUserData;
public:
	CBaseDev(): m_revCallBack(0),m_bErr(0){}
	~CBaseDev(){}
	void SetRevCallback( pRevCallBack revCallback, void* pUserData = 0 )
	{
		m_revCallBack = revCallback;
		m_pUserData = pUserData;
	}
	virtual int Send( const unsigned char* buf, int sz ) = 0;
	virtual void Close() = 0;
	virtual int Open() = 0;
};

#ifdef __cplusplus
}
#endif
#endif
