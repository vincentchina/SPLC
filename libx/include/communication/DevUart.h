#ifndef __DEVUART_H__
#define __DEVUART_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "BaseDev.h"
#include <Windows.h>

typedef struct{
	wchar_t comName[MAX_PATH];
	int szBuff;
	DWORD dwBaudRate;
	BYTE bitByteSize;
	BYTE bitParity;
	BYTE bitStopBits;
}COMCONFIG,*LPCOMCONFIG;

class LIBX_API CDevUart : public CBaseDev
{
private:
	HANDLE m_hUart;

protected:
	COMCONFIG m_conf;
	LPTSTR m_strError;
	OVERLAPPED m_tOverLaped;
	OVERLAPPED m_wOverLaped;
	OVERLAPPED m_rOverLaped;
	HANDLE m_hWorkThread;
	BOOL m_bIsRun;
	unsigned char *m_pBuf;
	int m_nBuf;
	DWORD m_errorCode;
protected:
	void _Receive();
	static DWORD WINAPI UartEventThread(LPVOID pParam);

public:
	CDevUart(void);
	~CDevUart(void);

	static HANDLE OpenDev( LPTSTR strPort );
	static BOOL ConfigDev( HANDLE hUart, LPCOMCONFIG pConfig,LPTSTR strError = NULL );
	static void CloseDev( HANDLE hUart );

	void Config( LPCOMCONFIG pConf ){ m_conf = *pConf; }
	BOOL Open(); 
	BOOL Send( const unsigned char* buf, int sz );
	void Close();
};


#ifdef __cplusplus
}
#endif
#endif	
