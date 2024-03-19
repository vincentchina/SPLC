#pragma once
#include "communication/BaseDev.h"
#include "communication/DevPip.h"
#include "communication/DevSocket.h"
#include "communication/DevUart.h"
#include "communication/TimerEx.h"
#include "../../PLCCore/include/Protocol.h"

class CCommunication
{
public:
	enum eErr{
		eErrSuccess,
		eErrFileSize,
		eErrTimeOut,
	};
protected:
	typedef struct tagFrame{
		CTimerEx Timer;
		const unsigned char* buf;
		int sz;
		int nSendCount;
	}Frame,*LPFrame;

protected:
	eErr m_eErr;
	DWORD m_dwTimeOut;
	CBaseDev* m_pDevSel;
	Frame m_framSend;
	BOOL m_bOk;
	pRevCallBack m_pCallBack;
	void* m_userData;

	static void RevCallBack( const unsigned char* buf, int sz, void* userData );
	static void TimeOut( CTimerEx::eTimeOutType eType, LPARAM lParam );

public:
	CCommunication(void);
	~CCommunication(void);

	CBaseDev* SetSelDev( CBaseDev* pDev );

	CBaseDev* GetDev() { return m_pDevSel; }

	void SetRevCallBack( pRevCallBack pCallBack, void* userData = NULL ) 
	{ m_pCallBack = pCallBack; m_userData = userData; }

	BOOL Send( const unsigned char* buf, int sz );

	BOOL IsOk() { return m_bOk; }

	BOOL IsSending() { return ( m_framSend.buf != NULL );}

	BOOL ProtocolSend( unsigned char* pData, int sz );

	BOOL ProtocolSend( char cmd, char coil, uint16 addr, uint16* data, uint16 sz );

	eErr GetError() { return m_eErr; }

	BOOL Download( unsigned char* buf, int sz );
};

