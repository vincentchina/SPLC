#include "StdAfx.h"
#include "LineCoil.h"

SIZE CLineCoil::m_szCoil;
BOOL CLineCoil::m_bInit;

CLineCoil::CLineCoil( int nRow, int nCol,eLineType eType ):
m_eType( eType )
{
	if( !m_bInit )
	{
		m_szCoil.cx = CBaseCoil::m_szBase.cx;
		m_szCoil.cy = CBaseCoil::m_szBase.cy;
	}
	SetPos( nRow, nCol );
}


CLineCoil::~CLineCoil(void)
{
}

void CLineCoil::Draw( HDC hdc, SIZE &offset )
{
	char str[10];
	RECT rc;
	rc.left = m_nPosCol * CBaseCoil::m_szBase.cx + offset.cx;
	rc.right = rc.left + m_szCoil.cx;
	rc.top = m_nPosRow * CBaseCoil::m_szBase.cy + offset.cy;
	rc.bottom = rc.top + m_szCoil.cy;
	if( m_eType == eVerticalLine )
	{
		MoveToEx( hdc,rc.left ,rc.top + CBaseCoil::m_nBaseLineOffset,NULL);
		LineTo(hdc,rc.left, rc.bottom + CBaseCoil::m_nBaseLineOffset );
	}else
	{
		MoveToEx( hdc,rc.left ,rc.top + CBaseCoil::m_nBaseLineOffset,NULL);
		LineTo(hdc,rc.right, rc.top + CBaseCoil::m_nBaseLineOffset );
	}
}