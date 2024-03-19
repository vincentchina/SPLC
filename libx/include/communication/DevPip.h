#ifndef __DEVPIP_H__
#define __DEVPIP_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "BaseDev.h"
#include <Windows.h>

class LIBX_API CDevPip : public CBaseDev
{
private:
	HANDLE m_hPip;

protected:
	wchar_t m_strPortConf[MAX_PATH];
	int m_szBufConf;
	unsigned char *m_pBuf;
	int m_nBuf;
	BOOL m_bIsServer;
	HANDLE m_hWorkThread;
	BOOL m_bRun;
	OVERLAPPED m_rOverlap; 
	OVERLAPPED m_wOverlap; 
	HANDLE m_rEvent;
	HANDLE m_wEvent;
	wchar_t* m_strErr; 

protected:
	BOOL OpenAsServer();
	BOOL OpenAsClient();

public:
	CDevPip(void);
	~CDevPip(void);
	static DWORD WINAPI ClientWorkThread( LPVOID lpParam );
	static DWORD WINAPI ServerWorkThread( LPVOID lpParam );
	BOOL Config( wchar_t* pipName,int szbuf, BOOL bServer = FALSE );
	BOOL Open();
	BOOL Send( const unsigned char* buf, int sz );
	void Close();
};

#ifdef __cplusplus
}
#endif
#endif	
