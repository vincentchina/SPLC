#include "communication\TimerEx.h"


CTimerEx::CTimerEx( DWORD dwTimeOut/* = 0*/, TimeOut pTimeOut/* = NULL*/, LPARAM lParam/* = NULL*/, BOOL bOverlapped/* = TRUE*/ ) 
		: m_dwTimeOut(dwTimeOut), m_funTimeout( pTimeOut ), m_lUserParam( lParam ),m_bOverlapped(bOverlapped),
		m_bLive(TRUE)
{
	m_hThread = INVALID_HANDLE_VALUE;
	m_hTimeEvent = CreateEvent( NULL, TRUE, FALSE,NULL );
	m_hWorkEvent = CreateEvent( NULL, TRUE, FALSE,NULL );
	if( m_dwTimeOut != 0 )
		Start();
}

CTimerEx::~CTimerEx(void)
{ 
	m_bLive = FALSE;
	End();
	CloseHandle( m_hTimeEvent );
	CloseHandle( m_hWorkEvent );
	CloseHandle( m_hThread );
}

CTimerEx::eTimeOutType CTimerEx::Start()
{ 
	return Start( m_bOverlapped ); 
}

CTimerEx::eTimeOutType CTimerEx::Start( BOOL bOverlapped )
{
	eTimeOutType eType;
	if( !bOverlapped )
	{
		switch ( WaitForSingleObject(m_hTimeEvent,m_dwTimeOut) )
		{
		case WAIT_OBJECT_0: eType = CTimerEx::end;		break;
		case WAIT_TIMEOUT:	eType = CTimerEx::timeOut;	break;
		default:			eType = CTimerEx::err;		break;
		}
		return eType;
	}else
	{
		if( m_hThread == INVALID_HANDLE_VALUE )
		{
			m_hThread = CreateThread( NULL,0, threadTimeOut, this, 0, NULL );
			if( m_hThread == INVALID_HANDLE_VALUE )
				return CTimerEx::err;
		}
		ResetEvent(m_hTimeEvent);
		SetEvent(m_hWorkEvent);
		return CTimerEx::overlapped;
	}
	return CTimerEx::err;
}

DWORD WINAPI CTimerEx::threadTimeOut( LPVOID pParam )
{
	eTimeOutType eType;
	CTimerEx* pthis = static_cast<CTimerEx*>( pParam );
	while( pthis->m_bLive )
	{
		switch( WaitForSingleObject( pthis->m_hWorkEvent, INFINITE ) )
		{
		case WAIT_OBJECT_0:
			{
				ResetEvent(pthis->m_hWorkEvent);
				switch ( WaitForSingleObject( pthis->m_hTimeEvent, pthis->m_dwTimeOut) )
				{
				case WAIT_OBJECT_0: eType = CTimerEx::end;		break;
				case WAIT_TIMEOUT:	eType = CTimerEx::timeOut;	break;
				default:			eType = CTimerEx::err;		break;
				}
				if( pthis->m_funTimeout )
					pthis->m_funTimeout( eType, pthis->m_lUserParam );
			}
			break;
		default:
			return -1;
		}
	}
	return 0;
}

void CTimerEx::End(){ 
	SetEvent(m_hWorkEvent);
	SetEvent(m_hTimeEvent);
}

CTimerEx::eTimeOutType CTimerEx::ReStart(){ return ReStart( m_bOverlapped ); }
CTimerEx::eTimeOutType CTimerEx::ReStart( BOOL bOverlapped ){ 
	End();
	return Start( bOverlapped );
}

void CTimerEx::SetTimeOut( DWORD dwTimeOut ) 
{ 
	m_dwTimeOut = dwTimeOut; 
}

void CTimerEx::SetCallBack( TimeOut pTimeOut,LPARAM lParam/* = NULL */)
{	
	m_funTimeout = pTimeOut;
	m_lUserParam = lParam; 
}

