#include "communication\DevPip.h"

#include <stdio.h>
#include <conio.h>
#include <tchar.h>


#define BUFSIZE 512

CDevPip::CDevPip(void) : m_pBuf(NULL),
	m_hPip(INVALID_HANDLE_VALUE),
	m_hWorkThread(NULL),
	m_bRun(FALSE),
	m_rEvent(NULL),
	m_wEvent(NULL),
	m_strErr(L"ok")
{
}

CDevPip::~CDevPip(void)
{
	Close();
}

BOOL CDevPip::Open()
{
	return ( ( m_bIsServer ) ? OpenAsServer() : OpenAsClient() );
}

void CDevPip::Close()
{
	m_bRun = FALSE;
	CloseHandle(m_wEvent);
	SetEvent(m_rEvent);
	if( m_hPip != INVALID_HANDLE_VALUE )
	{
		if( m_bIsServer ) DisconnectNamedPipe(m_hPip); 
		CloseHandle(m_hPip);
	}
	if( m_hWorkThread != NULL )
	{
		WaitForSingleObject( m_hWorkThread,INFINITE);
		CloseHandle(m_hWorkThread);
	}
	if( m_pBuf != NULL )
		delete[] m_pBuf;
	m_hPip = INVALID_HANDLE_VALUE;
	m_hWorkThread = NULL;
	m_pBuf = NULL;
	m_wEvent = NULL;
}

BOOL CDevPip::Config( wchar_t* pipName,int szbuf, BOOL bServer )
{
	wcscpy_s( m_strPortConf,pipName );
	m_szBufConf = szbuf;
	m_bIsServer = bServer;
	return TRUE;
}

BOOL CDevPip::OpenAsServer()
{
	DWORD dwThread;
	if( !m_bIsServer )
		return FALSE;

	Close();
	
	m_hPip = CreateNamedPipeW( 
		m_strPortConf,				  // pipe name 
		PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,       // read/write access 
		PIPE_TYPE_BYTE |          // message type pipe 
		PIPE_READMODE_BYTE |	  // message-read mode 
		PIPE_WAIT,     // blocking mode 
		1,//PIPE_UNLIMITED_INSTANCES, // max. instances  
		BUFSIZE,                  // output buffer size 
		BUFSIZE,                  // input buffer size 
		0,                        // client time-out 
		NULL);                    // default security attribute 

	if (m_hPip == INVALID_HANDLE_VALUE) 
	{
		//_tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError()); 
		return -1;
	}
	if( m_pBuf != NULL )
		delete[] m_pBuf;
	m_nBuf = m_szBufConf;
	m_pBuf = new unsigned char[m_nBuf];
	m_bRun = TRUE;
	memset( &m_wOverlap, 0, sizeof(OVERLAPPED) );
	m_wEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_wOverlap.hEvent = m_wEvent;
	m_hWorkThread = CreateThread( NULL,0,ServerWorkThread,this,0, &dwThread );
	if( m_hWorkThread == NULL )
	{
		//CloseHandle( m_hPip ); 
		return FALSE;
	}
	return TRUE;
}

BOOL CDevPip::OpenAsClient()
{
	DWORD dwThread;

	if( m_bIsServer )
		return FALSE;

	Close();

	// Try to open a named pipe; wait for it, if necessary. 
	while (1) 
	{ 
		m_hPip = CreateFileW( 
			m_strPortConf,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE, 
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			FILE_FLAG_OVERLAPPED,  // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 
		if (m_hPip != INVALID_HANDLE_VALUE) 
			break; 

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (GetLastError() != ERROR_PIPE_BUSY) 
		{
			m_strErr = L"Could not open pipe."; 
			return FALSE;
		}

		// All pipe instances are busy, so wait for 20 seconds. 
		if ( ! WaitNamedPipeW(m_strPortConf, 20000)) 
		{ 
			m_strErr = L"Could not open pipe: 20 second wait timed out."; 
			return FALSE;
		} 
	}
	if( m_pBuf != NULL )
		delete[] m_pBuf;
	m_nBuf = m_szBufConf;
	m_pBuf = new unsigned char[m_nBuf];
	m_bRun = TRUE;
	memset( &m_wOverlap, 0, sizeof(OVERLAPPED) );
	m_wEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_wOverlap.hEvent = m_wEvent;
	m_hWorkThread = CreateThread( NULL,0,ClientWorkThread,this,0, &dwThread );
	if( m_hWorkThread == NULL )
	{
		//CloseHandle( m_hPip ); 
		return FALSE;
	}
	return TRUE;
}

DWORD WINAPI CDevPip::ServerWorkThread( LPVOID lpParam )
{
	BOOL fSuccess;
	DWORD cbRead;
	DWORD dwErr;
	CDevPip* pComPip = (CDevPip*)lpParam;
	memset( &pComPip->m_rOverlap, 0, sizeof(OVERLAPPED) );
	pComPip->m_rEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	pComPip->m_rOverlap.hEvent = pComPip->m_rEvent;

	while( pComPip->m_bRun )
	{
		fSuccess = ConnectNamedPipe( pComPip->m_hPip, &pComPip->m_rOverlap ) ? 
					TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
		dwErr = GetLastError();
		if( dwErr == ERROR_IO_PENDING )
		{
			if( !GetOverlappedResult( pComPip->m_hPip,&pComPip->m_rOverlap, &cbRead,TRUE ) )
				break;
			fSuccess = TRUE;
		}else if( dwErr == ERROR_NO_DATA )
		{
			DisconnectNamedPipe( pComPip->m_hPip);
			continue;
		}
		while ( fSuccess && pComPip->m_bRun ) 
		{
			fSuccess = ReadFile( 
				pComPip->m_hPip,    // pipe handle 
				pComPip->m_pBuf,    // buffer to receive reply 
				pComPip->m_nBuf*sizeof(unsigned char),  // size of buffer 
				&cbRead,  // number of bytes read 
				&pComPip->m_rOverlap);    // not overlapped 
			if( fSuccess )
				goto rev_ok;
			dwErr = GetLastError();
			if (dwErr == ERROR_IO_PENDING)
			{
				if( GetOverlappedResult( pComPip->m_hPip,&pComPip->m_rOverlap, &cbRead,TRUE ) )
					goto rev_ok;
			}else
				break;
			continue;
rev_ok:		
			if( !pComPip->m_bRun )
				break;
			pComPip->m_pBuf[cbRead] = 0;
			//OutputDebugStringA((LPCSTR)pComPip->m_pBuf);
			if( pComPip->m_revCallBack != NULL )
				pComPip->m_revCallBack( pComPip->m_pBuf, cbRead,pComPip->m_pUserData );
		} 
	}
	return 0;
}
DWORD WINAPI CDevPip::ClientWorkThread( LPVOID lpParam )
{
	BOOL fSuccess;
	DWORD cbRead;
	DWORD dwErr;
	CDevPip* pComPip = (CDevPip*)lpParam;
	memset( &pComPip->m_rOverlap, 0, sizeof(OVERLAPPED) );
	pComPip->m_rEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	pComPip->m_rOverlap.hEvent = pComPip->m_rEvent;
	while( pComPip->m_bRun )
	{
		fSuccess = ReadFile( 
			pComPip->m_hPip,    // pipe handle 
			pComPip->m_pBuf,    // buffer to receive reply 
			pComPip->m_nBuf*sizeof(unsigned char),  // size of buffer 
			&cbRead,  // number of bytes read 
			&pComPip->m_rOverlap);    // not overlapped 
		if( fSuccess )
			goto rev_ok;
		dwErr = GetLastError();
		if (dwErr == ERROR_IO_PENDING)
		{
			if( GetOverlappedResult( pComPip->m_hPip,&pComPip->m_rOverlap, &cbRead,TRUE ) )
				goto rev_ok;
		}else
			break;
		continue;
rev_ok:		
		if( !pComPip->m_bRun )
			break;
		pComPip->m_pBuf[cbRead] = 0;
		//OutputDebugStringA((LPCSTR)pComPip->m_pBuf);
		if( pComPip->m_revCallBack != NULL )
			pComPip->m_revCallBack( pComPip->m_pBuf, cbRead,pComPip->m_pUserData );
	}
	OutputDebugStringA( "compip rev thread exit\n" );
	CloseHandle(pComPip->m_rEvent);
	pComPip->m_rEvent = NULL;
	return 0;
}

BOOL CDevPip::Send( const unsigned char* buf, int sz )
{
	if ( WriteFile( m_hPip,buf,sz,NULL,&m_wOverlap) == 0 )
	{
		return ( GetLastError() == ERROR_IO_PENDING );
	}
	return TRUE;
}