#pragma once
#include "Communication.h"

#define INI_SYSTEM			_T("project")

class CProject
{
public:
	CString m_strName;
	CString m_strPath;
	CString m_strFullPath;
	int m_nMaxLn;
	int m_nMaxCol;

	UINT m_uDevice;
	UINT m_uComType;
	CString m_strPort;
	BOOL m_bAutoPort;

	CCommunication m_Communication;

protected:
	CDevUart m_uart;
	CDevPip m_pip;

public:
	CProject(void);
	~CProject(void);

	BOOL Load( CString& strProject );
	BOOL Create( CString& strProject );
	BOOL Save();
	void Close();
	BOOL IsOpen(){ return !m_strFullPath.IsEmpty(); }
	BOOL TestCommunicate();

};

