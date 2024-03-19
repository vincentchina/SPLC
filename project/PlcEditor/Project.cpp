#include "StdAfx.h"
#include "Project.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CProject::CProject(void):
m_uDevice(0),
m_uComType(0),
m_bAutoPort(TRUE)
{
	/*m_strName.Empty();
	m_strPath.Empty();
	m_strFullPath.Empty();*/
}


CProject::~CProject(void)
{
	CBaseDev* pDev = m_Communication.GetDev();
	if( pDev )
		pDev->Close();
	Close();
}

BOOL CProject::Load( CString& strProject )
{
	int nPos;
	CString str;
	TCHAR strGet[ MAX_PATH ];
	Close();
	m_strFullPath = strProject;
	if( ( nPos = m_strFullPath.ReverseFind(_T('\\')) ) == -1 
		&& ( nPos = m_strFullPath.ReverseFind(_T('/')) ) == -1  )
	{
		GetModuleFileName( NULL, strGet, MAX_PATH );
		m_strFullPath = strGet;
		if( ( nPos = m_strFullPath.ReverseFind(_T('\\')) ) != -1 )
		{
			m_strFullPath = m_strFullPath.Left( nPos + 1 );
			m_strFullPath += strProject;
		}else
			return FALSE;
	}
	else
		m_strPath = m_strFullPath.Left( nPos + 1 );
	m_strName = m_strFullPath.Right( m_strFullPath.GetLength() - nPos - 1 );
	if( ::GetPrivateProfileString( _T("KPROJECT"),_T("name"),_T(""),strGet,MAX_PATH,strProject ) == 0 )
	{
		m_strFullPath.Empty();
		MessageBox(NULL,_T("不是工程文件"),_T("错误"),MB_OK );
		return FALSE;
	}
	m_nMaxCol = ::GetPrivateProfileInt( _T("SYSTEM"),_T("col"),12,strProject );
	m_nMaxLn = ::GetPrivateProfileInt( _T("SYSTEM"),_T("step"),8000,strProject );

	::GetPrivateProfileString( _T("COMMUNICATION"),_T("port"),_T("COM1"),strGet,MAX_PATH,strProject );
	m_strPort = strGet;
	m_uComType = ::GetPrivateProfileInt( _T("COMMUNICATION"),_T("type"),0,strProject );
	m_bAutoPort = ::GetPrivateProfileInt( _T("COMMUNICATION"),_T("autoPort"),0,strProject );
	
	//SetCurrentDirectory( m_strPath );
	return TRUE;
}

BOOL CProject::Create( CString& strProject )
{
	FILE* fb;
	CString str;
	Close();
	m_strFullPath = strProject;
	int nPos = m_strFullPath.ReverseFind(_T('\\'));
	if( nPos != -1 )
		m_strPath = m_strFullPath.Left( nPos );
	else
		m_strPath.Empty();
	m_strName = m_strFullPath.Right( m_strFullPath.GetLength() - nPos - 1 );
	if(::WritePrivateProfileString( _T("KPROJECT"),_T("name"),m_strName,m_strFullPath ) == 0 )
	{
		m_strFullPath.Empty();
		MessageBox(NULL,_T("创建工程失败"),_T("错误"),MB_OK );
		return FALSE;
	}
	str = m_strPath + _T("\\main.lad");
	fb = _wfopen( str,_T("wb") );
	fwrite( "END\n",1,4,fb);
	fclose( fb );
	str = m_strPath + _T("\\resource");
	fclose( _wfopen( str, _T("wb") ) );
	return TRUE;
}

BOOL CProject::Save()
{
	CString str;
	::WritePrivateProfileString( _T("KPROJECT"),_T("name"),m_strName,m_strFullPath );
	str.Format(_T("%d"),m_nMaxLn);
	::WritePrivateProfileString( _T("SYSTEM"),_T("step"),str,m_strFullPath );
	str.Format(_T("%d"),m_nMaxCol);
	::WritePrivateProfileString( _T("SYSTEM"),_T("col"),str,m_strFullPath );

	::WritePrivateProfileString( _T("COMMUNICATION"),_T("port"),m_strPort,m_strFullPath );
	::WritePrivateProfileString( _T("COMMUNICATION"),_T("type"),m_uComType ? _T("1") : _T("0"),m_strFullPath );
	::WritePrivateProfileString( _T("COMMUNICATION"),_T("autoPort"),m_bAutoPort ? _T("1") : _T("0"),m_strFullPath );
	return TRUE;
}

void CProject::Close()
{
	m_strFullPath.Empty();
}

BOOL CProject::TestCommunicate()
{
	CStdString str;
	byte byteENQ = ENQ;

	if( m_uComType == 0 ) // uart
	{
		COMCONFIG conf;
		memset( &conf, 0, sizeof( conf ) );
		wcscpy( conf.comName, m_strPort.GetBuffer() );
		conf.dwBaudRate = 9600;
		conf.bitByteSize = 8;
		conf.bitParity = NOPARITY;
		conf.bitStopBits = ONESTOPBIT;
		m_uart.Config( &conf );
		if( !m_uart.Open() )
			return FALSE;
		m_Communication.SetSelDev(&m_uart);
	}else //pip
	{
		m_pip.Config( (wchar_t*)m_strPort.GetBuffer(), 125  );
		if( !m_pip.Open() )
			return FALSE;
		m_Communication.SetSelDev(&m_pip);
	}
	m_Communication.Send( &byteENQ, 1 );
	while( m_Communication.IsSending() )
		Sleep(10);
	return m_Communication.IsOk();
}
