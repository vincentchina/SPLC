#include "Communication.h"
#include "plcInsSys\instruction.h"

CCommunication::CCommunication(void):
m_pDevSel(NULL),m_dwTimeOut(1000),	//1s
m_bOk(FALSE),m_eErr( eErrSuccess )
{
	m_framSend.buf = NULL;
	m_framSend.sz = 0;
	m_framSend.Timer.SetCallBack( TimeOut, (LPARAM)this );
	m_framSend.Timer.SetTimeOut( m_dwTimeOut );
}

CCommunication::~CCommunication(void)
{
	m_framSend.Timer.End();
	m_pDevSel = NULL;
}

CBaseDev* CCommunication::SetSelDev( CBaseDev* pDev )
{
	CBaseDev* old = m_pDevSel;
	m_pDevSel = pDev;
	if( m_pDevSel )
		m_pDevSel->SetRevCallback( RevCallBack, this );
	return old;
}

void CCommunication::RevCallBack( const unsigned char* buf, int sz, void* userData )
{
	CCommunication* pCommunication = static_cast<CCommunication*>(userData);
	pCommunication->m_bOk = TRUE;
	pCommunication->m_framSend.Timer.End();
	if( pCommunication->m_pCallBack )
		pCommunication->m_pCallBack( buf, sz, pCommunication->m_userData );
}

void CCommunication::TimeOut( CTimerEx::eTimeOutType eType, LPARAM lParam )
{
	CCommunication* pCommunication = (CCommunication*)lParam;
	if( eType == CTimerEx::timeOut )
	{
		if( pCommunication->m_framSend.nSendCount < 3 )
		{
			if( pCommunication->m_pDevSel->Send( pCommunication->m_framSend.buf, pCommunication->m_framSend.sz )
				&& pCommunication->m_framSend.Timer.ReStart() == CTimerEx::overlapped )
			{
				;
			}else
			{
				delete[] pCommunication->m_framSend.buf;
				pCommunication->m_framSend.buf = NULL;
			}
			++pCommunication->m_framSend.nSendCount;
		}else
		{
			pCommunication->m_bOk = FALSE;
			delete[] pCommunication->m_framSend.buf;
			pCommunication->m_framSend.buf = NULL;
		}
	}else if( eType == CTimerEx::end )
	{
		if( pCommunication->m_framSend.buf != NULL )
		{
			delete[] pCommunication->m_framSend.buf;
			pCommunication->m_framSend.buf = NULL;
		}
	}
}

BOOL CCommunication::Send( const unsigned char* buf, int sz )
{
	if( m_pDevSel == NULL )
		return FALSE;
	if( IsSending() )	//已有数据在发送
		return FALSE;

	m_framSend.buf = new unsigned char[ sz ];
	m_framSend.sz = sz;
	m_framSend.nSendCount = 0;
	memcpy( (char*)m_framSend.buf, buf, sz );
	
	if( m_pDevSel->Send( m_framSend.buf, m_framSend.sz ) && m_framSend.Timer.ReStart() == CTimerEx::overlapped )
	{
		return TRUE;
	}else
	{
		delete[] m_framSend.buf;
		m_framSend.buf = NULL;
	}
	return FALSE;
}

BOOL CCommunication::ProtocolSend( unsigned char* pData, int sz )
{
	unsigned char* p;
	unsigned char* pEnd;
	unsigned char sendBuf[ MAX_PROTOCL_BUF_SZ * 2 ];
	unsigned char* ps = sendBuf;
	unsigned char check;

	check = 0;
	*ps++ = STX;
	pEnd = pData + sz;
	p = pData;
	while( p < pEnd )
	{
		if( *p == STX 
			|| *p == ETX
			|| *p == ESC
			|| *p == ENQ )
			*ps++ = ESC;
		check += *p;
		*ps++ = *p++;
	}
	*ps++ = check;
	*ps++ = ETX;
	Send( sendBuf, ps -  sendBuf );
	return TRUE;
}

BOOL CCommunication::ProtocolSend( char cmd, char coil, uint16 addr, uint16* data, uint16 sz )
{
	unsigned char sendBuf[ MAX_PROTOCL_BUF_SZ ];
	unsigned char* p;
	p = sendBuf;
	if( cmd != CMD_READ
		&& cmd != CMD_WRITE
		&& cmd != CMD_FORCEON
		&& cmd != CMD_FORCEOFF
		&& cmd != CMD_PRO_READ
		&& cmd != CMD_PRO_WRITE )
		return FALSE;
	*p++ = cmd;
	if( cmd != CMD_PRO_READ && cmd != CMD_PRO_WRITE )
		*p++ = coil;
	*p++ = ( addr & 0xff00 ) >> 8;
	*p++ = addr & 0x00ff;

	if( cmd != CMD_FORCEON && cmd != CMD_FORCEOFF )
	{
		*p++ = (unsigned char)sz;
		if( cmd != CMD_READ && cmd != CMD_PRO_READ )
		{
			while( sz-- )
			{
				*p++ = ( *data & 0xff00 ) >> 8;
				*p++ = *data++ & 0x00ff;
			}
		}
	}
	return ProtocolSend( sendBuf, p - sendBuf );
}

BOOL CCommunication::Download( unsigned char* buf, int sz )
{
	unsigned short* pPrg;
	unsigned short* pEnd;
	unsigned short addr;

	if( sz % 2 != 0 )
	{
		m_eErr = eErrFileSize;
		return FALSE;
	}
	sz /= 2;
	//开始下载
	//先暂停PLC
	if( ProtocolSend( CMD_FORCEOFF, (char)api_coilM, 8000, 0, 0 ) )
	{
		while( IsSending() )
			Sleep(10);
		if( !IsOk() )
		{
			m_eErr = eErrTimeOut;
			return FALSE;
		}
		pPrg = (unsigned short*)buf;
		pEnd = pPrg + sz;
		addr = 0;
		while( pPrg < pEnd )
		{
			if( ProtocolSend( CMD_PRO_WRITE, 0, addr++, pPrg++, 1 ) )
			{
				while( IsSending() )
					Sleep(10);
				if( !IsOk() )
				{
					m_eErr = eErrTimeOut;
					return FALSE;
				}
			}
		}
		//验证写入

		//重新加载PLC 启动PLC
		if( ProtocolSend( CMD_FORCEON, (char)api_coilM, 8038, 0, 1 ) )
		{
			while( IsSending() )
				Sleep(10);
		}
		if( ProtocolSend( CMD_FORCEON, (char)api_coilM, 8000, 0, 1 ) )
		{
			while( IsSending() )
				Sleep(10);
		}
	}else
	{
		m_eErr = eErrTimeOut;
		return FALSE;
	}
	return TRUE;
}
