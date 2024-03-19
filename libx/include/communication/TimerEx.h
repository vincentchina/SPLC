#pragma once
#include <windows.h>
#include "dllpreHead.h"

class LIBX_API CTimerEx
{
public:
	enum eTimeOutType{
		end,
		timeOut,
		err,
		overlapped,
	};
	typedef void (*TimeOut)( eTimeOutType eType, LPARAM lParam );

protected:
	TimeOut m_funTimeout;
	LPARAM m_lUserParam;
	DWORD m_dwTimeOut;
	HANDLE m_hTimeEvent;
	HANDLE m_hWorkEvent;
	HANDLE m_hThread;
	BOOL m_bOverlapped;
	BOOL m_bLive;

public:

	CTimerEx( DWORD dwTimeOut = 0, TimeOut pTimeOut = NULL, LPARAM lParam = NULL, BOOL bOverlapped = TRUE );
	
	~CTimerEx(void);

	eTimeOutType Start();

	eTimeOutType Start( BOOL bOverlapped );

	static DWORD WINAPI threadTimeOut( LPVOID pParam );

	void End();

	eTimeOutType ReStart();

	eTimeOutType ReStart( BOOL bOverlapped );

	void SetTimeOut( DWORD dwTimeOut );

	void SetCallBack( TimeOut pTimeOut,LPARAM lParam = NULL );

};

