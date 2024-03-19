#include "StdAfx.h"
#include "VirtualMachine.h"


CVirtualMachine::CVirtualMachine( int nTotalAixs, funStopCallBack pStop ) :
m_funStopCallBack( pStop ),m_nTotalAixs( nTotalAixs )
{
	m_pAixs = new Data[ m_nTotalAixs ];
	memset( m_pAixs, 0, sizeof( Data ) * m_nTotalAixs );
	m_hWorkThread = ::CreateThread( NULL, 0, WorkThread, this, 0, 0 );
}


CVirtualMachine::~CVirtualMachine(void)
{
	m_bRun = FALSE;
	::WaitForSingleObject( m_hWorkThread, INFINITE );
	CloseHandle( m_hWorkThread );
	delete[] m_pAixs;
}
static char* g_strAixs[] = {
	"MY",
	"MZ",
	"X",
	"SY",
	"SZ"
};

int CVirtualMachine::Action( int aix, int speed, int location, int bAbsolute )
{
	static int i;
	if( aix < 0 || aix > m_nTotalAixs )
		return -1;
	if( speed == 0 || location == 0 )
	{
		m_pAixs[ aix ].bStart = FALSE;
		if( m_funStopCallBack )
			m_funStopCallBack( aix );
	}
	m_pAixs[ aix ].speed = ( ( speed > 0 ) ? speed : -speed );
	m_pAixs[ aix ].location = ( ( location > 0 ) ? location : -location );
	m_pAixs[ aix ].bAbsolute = bAbsolute;
	m_pAixs[ aix ].bStart = TRUE;
	if( aix >= 0 && aix < sizeof( g_strAixs ) / sizeof( char* ) )
		printf("[%d]aix:%s,speed:%d,location:%d\n", i++, g_strAixs[ aix ] , speed, location );
	else
		printf("[%d]aix:%d,speed:%d,location:%d\n", i++, aix, speed, location );
	return 0;
}

DWORD WINAPI CVirtualMachine::WorkThread(LPVOID pParam)
{
	CVirtualMachine* pThis = (CVirtualMachine*)pParam;
	while( pThis->m_bRun )
	{
		for ( int i = 0; i < pThis->m_nTotalAixs; ++i )
		{
			if( pThis->m_pAixs[ i ].bStart )
			{
				pThis->m_pAixs[ i ].location -= pThis->m_pAixs[ i ].speed;
				if( pThis->m_pAixs[ i ].location < 0 )
				{
					pThis->m_pAixs[ i ].bStart = FALSE;
					if( pThis->m_funStopCallBack )
						pThis->m_funStopCallBack( i );
				}
			}
		}
		Sleep(1000);
	}
	return 0;
}
