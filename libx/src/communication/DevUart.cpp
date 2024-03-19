#include "communication\DevUart.h"
#include <tchar.h>

CDevUart::CDevUart(void) :
m_hUart( INVALID_HANDLE_VALUE ),
	m_pBuf(NULL)
{
	memset( &m_conf, 0, sizeof( m_conf ) );
	_tcscpy_s( m_conf.comName, _T("COM0") );
	m_conf.dwBaudRate = 9600;
	m_conf.bitByteSize = 8;
	m_conf.bitParity = NOPARITY;
	m_conf.bitStopBits = ONESTOPBIT;
}


CDevUart::~CDevUart(void)
{
	Close();
}

HANDLE CDevUart::OpenDev( LPTSTR strPort )
{
	LPTSTR lpCom;
	HANDLE hUart;
	int nLen = _tcslen(strPort) + 5;
	lpCom = new TCHAR[ nLen ];
	_tcscpy_s( lpCom,nLen, _T("\\\\.\\") );
	_tcscat_s( lpCom,nLen,strPort );
	hUart = CreateFile(lpCom,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL
		);
	delete[] lpCom;
	return hUart;
}
BOOL CDevUart::ConfigDev( HANDLE hUart, LPCOMCONFIG pConfig,LPTSTR strError )
{
	DCB dcb;
	COMMTIMEOUTS uartTimeout;
	BOOL bResult;
	LPTSTR strState = _T("configure success.");
	bResult = GetCommState(hUart,&dcb);
	if (bResult == FALSE)
	{
		strState = _T("GetCommState() error");
		goto leave;
	}
	dcb.fBinary = TRUE;
	dcb.fParity = TRUE;
	if (pConfig == NULL)
	{
		dcb.BaudRate = 19200;
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
	}
	else
	{
		dcb.BaudRate = pConfig->dwBaudRate;
		dcb.ByteSize = pConfig->bitByteSize;
		dcb.Parity = pConfig->bitParity;
		dcb.StopBits = pConfig->bitStopBits;
	}
	bResult = SetupComm(hUart,1024,1024);
	if (bResult == FALSE)
	{
		strState = _T("SetupComm() error");
		goto leave;
	}
	bResult = SetCommState(hUart,&dcb);
	if (bResult == FALSE)
	{
		strState = _T("SetCommState() error");
		goto leave;
	}
	//超时应该和波特率相关最好
	GetCommTimeouts( hUart,&uartTimeout );
	TCHAR buf[1024];
	_stprintf_s( buf, _T("uartTimeout.ReadIntervalTimeout = %d\n\
uartTimeout.ReadTotalTimeoutConstant = %d\n\
uartTimeout.ReadTotalTimeoutMultiplier = %d\n\
uartTimeout.WriteTotalTimeoutConstant = %d\n\
uartTimeout.WriteTotalTimeoutMultiplier = %d\n"),
	uartTimeout.ReadIntervalTimeout ,
	uartTimeout.ReadTotalTimeoutConstant ,
	uartTimeout.ReadTotalTimeoutMultiplier ,
	uartTimeout.WriteTotalTimeoutConstant,
	uartTimeout.WriteTotalTimeoutMultiplier);
	OutputDebugString(buf);
	
	uartTimeout.ReadIntervalTimeout = MAXDWORD;
		uartTimeout.ReadTotalTimeoutConstant = 0;
		uartTimeout.ReadTotalTimeoutMultiplier = 0;
		uartTimeout.WriteTotalTimeoutConstant = 0;
		uartTimeout.WriteTotalTimeoutMultiplier = 0;
		bResult = SetCommTimeouts(hUart,&uartTimeout);
		if (bResult == FALSE)
		{
			strState = _T("SetCommTimeouts() error");
			goto leave;
		}
	
	bResult = SetCommMask(hUart,EV_RXCHAR | EV_TXEMPTY);
	if (bResult == FALSE)
	{
		strState = _T("SetCommMask() error");
		goto leave;
	}

leave:
	if( strError != NULL )
		strError = strState;
	return bResult;
}

void CDevUart::CloseDev( HANDLE hUart )
{
	CloseHandle(hUart);
}

BOOL CDevUart::Open()
{
	Close();
	if( m_conf.szBuff < 0 || ( m_hUart = OpenDev( m_conf.comName ) ) == INVALID_HANDLE_VALUE )
		return FALSE;
	if( !ConfigDev(m_hUart,&m_conf,m_strError) )
	{
		Close();
		return FALSE;
	}
	memset(&m_rOverLaped,0,sizeof(OVERLAPPED));
	memset(&m_tOverLaped,0,sizeof(OVERLAPPED));
	memset(&m_wOverLaped,0,sizeof(OVERLAPPED));

	if( m_pBuf != NULL )
		delete[] m_pBuf;
	m_nBuf = m_conf.szBuff;
	m_pBuf = new unsigned char[m_nBuf + 1];
	m_hWorkThread = ::CreateThread(NULL,0,&UartEventThread,this,0,NULL);
	if (INVALID_HANDLE_VALUE == m_hWorkThread )
	{
		Close();
		return FALSE;
	}
	return TRUE;
}

void CDevUart::Close()
{
	if (INVALID_HANDLE_VALUE != m_hUart)
	{
		m_bIsRun = FALSE;
		SetEvent(m_tOverLaped.hEvent);
		WaitForSingleObject(m_hWorkThread,INFINITE);
		CloseHandle(m_hUart);
		CloseHandle(m_hWorkThread);
		if( m_pBuf != NULL )
			delete[] m_pBuf;
		m_pBuf = NULL;
		m_hUart = INVALID_HANDLE_VALUE;
		m_hWorkThread = INVALID_HANDLE_VALUE;
	}
}
DWORD WINAPI CDevUart::UartEventThread(LPVOID pParam)
{
	CDevUart* pUart = static_cast<CDevUart*>( pParam );
	DWORD dwEvtMask,dwResult;
	pUart->m_tOverLaped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	pUart->m_rOverLaped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	pUart->m_wOverLaped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	pUart->m_bIsRun = true;
	while ( pUart->m_bIsRun )
	{
		try
		{
			dwResult = WaitCommEvent(pUart->m_hUart,&dwEvtMask,&pUart->m_tOverLaped);
			if (dwResult == FALSE && ERROR_IO_PENDING != GetLastError())
				goto over;
			dwResult = WaitForSingleObject(pUart->m_tOverLaped.hEvent,INFINITE);
			ResetEvent(pUart->m_tOverLaped.hEvent);
			if (!pUart->m_bIsRun)
				break;
			switch (dwResult)
			{
			case WAIT_OBJECT_0:
				switch (dwEvtMask)
				{
				case EV_RXCHAR:
					pUart->_Receive();
					break;
				case EV_TXEMPTY:
					break;
				}
				break;
			default:
				goto over;
				break;
			}
		}
		catch (...)
		{
			goto over;
		}
	}

over:
	CloseHandle(pUart->m_tOverLaped.hEvent);
	CloseHandle(pUart->m_rOverLaped.hEvent);
	CloseHandle(pUart->m_wOverLaped.hEvent);
	return 0;
}

void CDevUart::_Receive()
{

	DWORD res;
	COMSTAT rst;
	DWORD sz;
	if( ClearCommError(m_hUart,&res,&rst) )
	{
		if( rst.cbInQue != 0 )
		{
			ResetEvent(m_rOverLaped.hEvent);
			ReadFile(m_hUart,m_pBuf, m_nBuf,NULL,&m_rOverLaped);
			if( GetOverlappedResult( m_hUart, &m_rOverLaped, &sz, TRUE ) )
			{
				//m_pBuf[sz] = 0;
				if( NULL != m_revCallBack )
					m_revCallBack( m_pBuf,sz,m_pUserData);
			}
		}
		PurgeComm( m_hUart, PURGE_TXABORT|
			PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	}
}
BOOL CDevUart::Send( const unsigned char* buf, int sz )
{
	DWORD factdata = 0;
	DWORD res;
	COMSTAT rst;

	if( m_hUart == INVALID_HANDLE_VALUE )
		return FALSE;

	ClearCommError(m_hUart,&res,&rst);
	ResetEvent( m_wOverLaped.hEvent );
	if ( WriteFile( m_hUart,buf,sz,&factdata,&m_wOverLaped) == 0 )
	{
		if( GetLastError() == ERROR_IO_PENDING )
		{
			WaitForSingleObject( m_wOverLaped.hEvent,5000 );
			return TRUE;
		}else
			return TRUE;
	}
	return TRUE;
}
