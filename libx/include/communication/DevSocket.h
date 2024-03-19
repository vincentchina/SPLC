#ifndef __DEVSOCKET_H__
#define __DEVSOCKET_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "BaseDev.h"
#include <Windows.h>

class LIBX_API CDevSocket : public CBaseDev
{
private:
	SOCKET m_socketSrv;
	SOCKET m_socketClient;
protected:
	wchar_t m_strPortConf[MAX_PATH];
	int m_szBufConf;
	BOOL m_bIsServer;
	BOOL m_bRun;
	static BOOL m_bInit;
	HANDLE m_hWorkThread;
protected:
	static DWORD WINAPI ServerThread(LPVOID pParam);

public:
	CDevSocket(void);
	~CDevSocket(void);
	BOOL OpenAsServer();
	BOOL Config( wchar_t* pipName,int szbuf, BOOL bServer = FALSE );
	BOOL Open(); 
	BOOL Send( const unsigned char* buf, int sz );
	void Close();
};


#ifdef __cplusplus
}
#endif
#endif	
