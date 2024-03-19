#include "communication\DevSocket.h"

#include <tchar.h>

#pragma comment( lib, "ws2_32.lib" )

BOOL CDevSocket::m_bInit;

CDevSocket::CDevSocket(void)
{
	m_socketClient = INVALID_SOCKET;
	m_socketSrv = INVALID_SOCKET;
}

CDevSocket::~CDevSocket(void)
{
	if( m_bInit )
	{
		WSACleanup();
		m_bInit = FALSE;
	}
}

BOOL CDevSocket::OpenAsServer()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	if( !m_bInit )
	{
		wVersionRequested = MAKEWORD( 1, 1 );
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			return FALSE;
		}
		if ( LOBYTE( wsaData.wVersion ) != 1 ||
			HIBYTE( wsaData.wVersion ) != 1 ) {
				WSACleanup( );
				return FALSE;
		}
		m_bInit = TRUE;
	}
	m_hWorkThread = ::CreateThread(NULL,0,&ServerThread,this,0,NULL);
	if (INVALID_HANDLE_VALUE == m_hWorkThread )
		return FALSE;
	return TRUE;
}

BOOL CDevSocket::Send( const unsigned char* buf, int sz )
{
	//char debug[1024];
	if( m_socketClient != INVALID_SOCKET )
	{
		//send(m_socketClient,debug,1024,0);
		return send(m_socketClient,(char*)buf,sz,0);
	}else
		return FALSE;
}

BOOL CDevSocket::Config( wchar_t* pipName,int szbuf, BOOL bServer )
{
	wcscpy_s( m_strPortConf,pipName );
	m_szBufConf = szbuf;
	m_bIsServer = bServer;
	return TRUE;
}
BOOL CDevSocket::Open()
{
	return ( ( m_bIsServer ) ? OpenAsServer() : 
		FALSE);//OpenAsClient( m_strPortConf,m_szBufConf ) );
}
void CDevSocket::Close()
{
	m_bRun = FALSE;
	closesocket(m_socketClient);
	closesocket(m_socketSrv);
	m_socketClient = INVALID_SOCKET;
	m_socketSrv = INVALID_SOCKET;
	WaitForSingleObject(m_hWorkThread,INFINITE);
}

DWORD WINAPI CDevSocket::ServerThread(LPVOID pParam)
{
	CDevSocket* pthis = (CDevSocket*)pParam;
	char recvBuf[1024];
	pthis->m_socketSrv=socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(20158);
	bind(pthis->m_socketSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	listen(pthis->m_socketSrv,1);
	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	pthis->m_bRun = TRUE;
	while( pthis->m_bRun && pthis->m_socketSrv != INVALID_SOCKET )
	{
		pthis->m_socketClient = INVALID_SOCKET;
		pthis->m_socketClient=accept(pthis->m_socketSrv,(SOCKADDR*)&addrClient,&len);
		while( pthis->m_bRun )
		{
			int nlen = recv( pthis->m_socketClient,recvBuf,1024,0 );
			if( nlen == 0 )//client close
			{
				pthis->m_socketClient = INVALID_SOCKET;
				break;
			}else if( nlen == SOCKET_ERROR ) //error
			{
				pthis->m_socketClient = INVALID_SOCKET;
				break;
			}else{
				if( NULL != pthis->m_revCallBack )
					pthis->m_revCallBack( (unsigned char*)recvBuf,nlen,pthis->m_pUserData);
			}
		}
		//closesocket(sockConn);
	}
	return 0;
}
