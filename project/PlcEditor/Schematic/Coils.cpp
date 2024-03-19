#include "StdAfx.h"
#include "Coils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CInputCoil::CInputCoil( eInputType eType,LPTSTR strName ) : 
m_eInputType(eType),CBaseCoil( COIL_INPUT,strName )
{
}
CInputCoil::CInputCoil( const CInputCoil& coil ) :
m_eInputType(coil.m_eInputType),CBaseCoil( coil )
{
	SetNote( coil.m_strNote );
}
CInputCoil::~CInputCoil()
{
}
void CInputCoil::Draw( LPDrawInfo pInfo, SIZE &posOffset )
{
	CSymbolCoil::eSymbolType eSymbol;
	int nxpos,nypos;
	nxpos = posOffset.cx + pInfo->offsetMid.cx;
	nypos = posOffset.cy + pInfo->offsetMid.cy;
	// -
	MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
	LineTo(pInfo->hdc,nxpos - pInfo->nSpase, posOffset.cy + pInfo->nLineOffset );
	// |
	MoveToEx( pInfo->hdc,nxpos - pInfo->nSpase ,nypos,NULL);
	LineTo(pInfo->hdc,nxpos - pInfo->nSpase, nypos + pInfo->nUnitLen );
	if( eOpen != m_eInputType )
	{
		if( m_eInputType == eClose )
			eSymbol = CSymbolCoil::eNot;
		else if( m_eInputType == eUpPluse )
			eSymbol = CSymbolCoil::eUpPluse;
		else
			eSymbol = CSymbolCoil::eDownPluse;
		CSymbolCoil::DrawSymbol( pInfo,eSymbol, posOffset );
	}
	CBaseCoil::Draw( pInfo, posOffset );
	// |
	MoveToEx( pInfo->hdc,nxpos + pInfo->nSpase ,nypos,NULL);
	LineTo(pInfo->hdc,nxpos + pInfo->nSpase, nypos + pInfo->nUnitLen );
	// -
	MoveToEx( pInfo->hdc,nxpos + pInfo->nSpase,posOffset.cy + pInfo->nLineOffset,NULL);
	LineTo(pInfo->hdc,posOffset.cx + pInfo->nColLen, posOffset.cy + pInfo->nLineOffset );
}



COutputCoil::COutputCoil( eOutputType eType, LPTSTR strName, int nOccupation ) 
	: m_eOutputType(eType),CBaseCoil( COIL_OUTPUT ),m_strArg(NULL)
{
	if( m_eOutputType == eFnc )
		m_nColOccupation = nOccupation;
	SetName( strName );
}
COutputCoil::COutputCoil( const COutputCoil& coil ) :
CBaseCoil( coil ),m_strArg(NULL)
{

}
COutputCoil::~COutputCoil()
{
	if( m_strArg )
		delete[] m_strArg;
}
BOOL COutputCoil::SetFnc( LPTSTR strFnc )
{
	if( m_eOutputType == eFnc && strFnc )
	{
		return SetName(strFnc);
	}
	return FALSE;
}
BOOL COutputCoil::SetArg( LPTSTR strArg )
{
	if( m_strArg )
		delete[] m_strArg;
	m_strArg = new TCHAR[ _tcslen(strArg) + 1 ];
	_tcscpy( m_strArg, strArg );
	return TRUE;
}
void COutputCoil::Draw( LPDrawInfo pInfo, SIZE &posOffset )
{
	RECT rcArc;
	POINT ptStart,ptEnd;
	// -
	MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
	LineTo(pInfo->hdc,posOffset.cx + pInfo->nOutPutLine, posOffset.cy + pInfo->nLineOffset );
	if( eCoil == m_eOutputType )
	{
		//(
		rcArc.left = posOffset.cx + pInfo->nOutPutLine;
		rcArc.top = posOffset.cy + pInfo->nLineOffset - pInfo->nRadius;
		rcArc.right = rcArc.left + pInfo->nRadius;
		rcArc.bottom = rcArc.top + ( pInfo->nRadius << 1 ) + 1;
		ptEnd.x = rcArc.right - ( pInfo->nRadius >> 1 );
		ptEnd.y = rcArc.bottom;
		ptStart.x = ptEnd.x;
		ptStart.y = rcArc.top;
		//Rectangle(pInfo->hdc,rcArc.left,rcArc.top,rcArc.right,rcArc.bottom);
		Arc( pInfo->hdc,rcArc.left,rcArc.top,rcArc.right,rcArc.bottom,
			ptStart.x,ptStart.y,ptEnd.x,ptEnd.y );
		//AngleArc( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine + pInfo->nRadius,
		//	posOffset.cy + pInfo->nLineOffset, pInfo->nRadius,90,90 );
		//)
		rcArc.left = posOffset.cx + pInfo->nColLen - pInfo->nOutPutLine;//一定是占用一个col
		rcArc.top = posOffset.cy + pInfo->nLineOffset - pInfo->nRadius;
		rcArc.right = rcArc.left - pInfo->nRadius;
		rcArc.bottom = rcArc.top + ( pInfo->nRadius << 1 ) + 1;
		ptStart.x = rcArc.right + ( pInfo->nRadius >> 1 );
		ptStart.y = rcArc.bottom;
		ptEnd.x = ptStart.x;
		ptEnd.y = rcArc.top;
		//Rectangle(pInfo->hdc,rcArc.left,rcArc.top,rcArc.right,rcArc.bottom);
		Arc( pInfo->hdc,rcArc.left,rcArc.top,rcArc.right,rcArc.bottom,
			ptStart.x,ptStart.y,ptEnd.x,ptEnd.y );
	}else if( eFnc == m_eOutputType )
	{
		int nypos = posOffset.cy + pInfo->offsetMid.cy;
		// [
		MoveToEx( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine + pInfo->nOutPutLine ,nypos,NULL);
		LineTo( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine ,nypos );
		LineTo( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine ,nypos + pInfo->nUnitLen );
		LineTo( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine + pInfo->nOutPutLine ,nypos + pInfo->nUnitLen);
		// ]
		int rPos;
		rPos = posOffset.cx + m_nColOccupation * pInfo->nColLen - 1;
		MoveToEx( pInfo->hdc,rPos - pInfo->nOutPutLine - pInfo->nOutPutLine ,nypos,NULL);
		LineTo( pInfo->hdc,rPos - pInfo->nOutPutLine ,nypos );
		LineTo( pInfo->hdc,rPos - pInfo->nOutPutLine ,nypos + pInfo->nUnitLen );
		LineTo( pInfo->hdc,rPos - pInfo->nOutPutLine - pInfo->nOutPutLine ,nypos + pInfo->nUnitLen);
		//fnc
		/*rcArc.left = posOffset.cx + pInfo->nOutPutLine;
		rcArc.top = posOffset.cy + pInfo->offsetMid.cy;
		rcArc.right = rcArc.left + ( pInfo->nColLen * m_nColOccupation )- pInfo->nOutPutLine - pInfo->nOutPutLine;
		rcArc.bottom = rcArc.top + ( pInfo->nLineOffset >> 1 );
		if( m_strName )
			DrawText( pInfo->hdc,m_strName,-1,&rcArc,DT_CENTER | DT_VCENTER | DT_SINGLELINE); */
	}
	if( m_strName )
	{
		rcArc.left = posOffset.cx + pInfo->nOutPutLine;
		rcArc.top = posOffset.cy + pInfo->offsetMid.cy;
		rcArc.right = rcArc.left + ( pInfo->nColLen * m_nColOccupation )- pInfo->nOutPutLine - pInfo->nOutPutLine;
		rcArc.bottom = rcArc.top + ( pInfo->nLineOffset >> 1 );
		DrawText( pInfo->hdc,m_strName,-1,&rcArc,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
	if( m_strArg )
	{
		rcArc.left = posOffset.cx;
		rcArc.top = posOffset.cy;
		rcArc.right = posOffset.cx + pInfo->nColLen * m_nColOccupation;
		rcArc.bottom = rcArc.top + pInfo->offsetMid.cy;
		DrawText( pInfo->hdc,m_strArg,-1,&rcArc,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
	// -
	MoveToEx( pInfo->hdc,posOffset.cx + m_nColOccupation * pInfo->nColLen - pInfo->nOutPutLine,posOffset.cy + pInfo->nLineOffset,NULL);
	LineTo(pInfo->hdc,posOffset.cx + m_nColOccupation * pInfo->nColLen, posOffset.cy + pInfo->nLineOffset );
}

CSymbolCoil::CSymbolCoil( eSymbolType nLineType,LPSTR strName ) : 
m_nSymbolType(nLineType),
CBaseCoil( COIL_SYMBOL )
{
	SetName( strName );
}
CSymbolCoil::CSymbolCoil( const CSymbolCoil& coil ) :
CBaseCoil( coil )
{

}
CSymbolCoil::~CSymbolCoil()
{

}
void CSymbolCoil::DrawSymbol( LPDrawInfo pInfo,eSymbolType eType, SIZE &posOffset )
{
	int nxpos,nypos;
	nxpos = posOffset.cx + pInfo->offsetMid.cx;
	nypos = posOffset.cy + pInfo->offsetMid.cy;
	int nspase;
	switch( eType )
	{
	case eDownPluse:
		MoveToEx( pInfo->hdc,nxpos,nypos,NULL);
		LineTo(pInfo->hdc,nxpos, nypos + pInfo->nUnitLen + 1 );
		MoveToEx( pInfo->hdc,nxpos - pInfo->szArrow.cx,nypos + pInfo->nUnitLen - pInfo->szArrow.cy,NULL);
		LineTo(pInfo->hdc,nxpos, nypos + pInfo->nUnitLen + 1 );
		LineTo(pInfo->hdc,nxpos + pInfo->szArrow.cx + 1, nypos + pInfo->nUnitLen - pInfo->szArrow.cy - 1 );
		break;
	case eUpPluse:
		MoveToEx( pInfo->hdc,nxpos,nypos,NULL);
		LineTo(pInfo->hdc,nxpos, nypos + pInfo->nUnitLen );
		MoveToEx( pInfo->hdc,nxpos - pInfo->szArrow.cx,nypos + pInfo->szArrow.cy,NULL);
		LineTo(pInfo->hdc,nxpos, nypos - 1 );
		LineTo(pInfo->hdc,nxpos + pInfo->szArrow.cx + 1, nypos + pInfo->szArrow.cy + 1 );
		break;
	case eNot:
		nspase = pInfo->nSpase / 3;
		MoveToEx( pInfo->hdc,nxpos + nspase ,nypos,NULL);
		LineTo(pInfo->hdc,nxpos - nspase, nypos + pInfo->nUnitLen );
		break;
	}
}
void CSymbolCoil::Draw( LPDrawInfo pInfo, SIZE &posOffset )
{
	if( m_nSymbolType == eLable )
		__super::Draw(pInfo,posOffset);
	else
	{
		CBaseCoil::DrawLine( pInfo, COIL_HLINE, posOffset );
		DrawSymbol( pInfo,m_nSymbolType, posOffset );
	}
}

CNoteCoil::CNoteCoil(LPTSTR strNote ) :
CBaseCoil( COIL_NOTE )
{
	SetOutNote( strNote );
}
CNoteCoil::CNoteCoil( const CNoteCoil& coil ) :
CBaseCoil( coil )
{

}
BOOL CNoteCoil::SetOutNote( LPTSTR strNote )
{
	return SetNote(strNote);
}
void CNoteCoil::Draw(LPDrawInfo pInfo, SIZE &posOffset )
{
	RECT rc;
	// -
	MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nUnitLen,NULL);
	LineTo(pInfo->hdc,posOffset.cx + pInfo->nOutPutLine, posOffset.cy + pInfo->nUnitLen );
	/*rc.left = posOffset.cx + pInfo->nOutPutLine;
	rc.top = posOffset.cy;
	rc.right = posOffset.cx + pInfo->nColLen - 1;
	rc.bottom = posOffset.cy + pInfo->nRowLen - 1;
	Rectangle(pInfo->hdc,rc.left,rc.top,rc.right,rc.bottom);*/
	if( m_strNote )
	{
		TextOut( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine,posOffset.cy, m_strNote, _tcslen(m_strNote) ); 
	}
}

CLinkCoil::CLinkCoil( eLinkType eType, int nID ) :
CBaseCoil(COIL_LINKER)
{
	m_nID = nID;
	m_nLinkType = eType;
}
CLinkCoil::CLinkCoil( const CLinkCoil& coil ):
CBaseCoil( coil )
{
}

void CLinkCoil::Draw(LPDrawInfo pInfo, SIZE &posOffset )
{
	RECT rc;
	// -
	MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
	LineTo(pInfo->hdc,posOffset.cx + pInfo->nOutPutLine, posOffset.cy + pInfo->nLineOffset );
	rc.left = posOffset.cx + pInfo->nOutPutLine;
	rc.top = posOffset.cy;
	rc.right = posOffset.cx + pInfo->nColLen - 1;
	rc.bottom = posOffset.cy + pInfo->nRowLen - 1;
	Rectangle(pInfo->hdc,rc.left,rc.top,rc.right,rc.bottom);
	if( m_strNote )
	{
		rc.left++;
		rc.top += 2;
		DrawText( pInfo->hdc,m_strNote,-1,&rc,DT_TOP | DT_LEFT | DT_WORDBREAK  ); 
	}
}

