#include "plcSchematic\Coils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CInputCoil::CInputCoil( eInputType eType,CStdString strName,int nOccupation ) : 
m_eInputType(eType),CBaseCoil( COIL_INPUT,strName ),m_uState(0)
{
	if( m_eInputType & eFnc )
		m_nColOccupation = nOccupation;
}
CInputCoil::CInputCoil( const CInputCoil& coil ) :
m_eInputType(coil.m_eInputType),CBaseCoil( coil ),m_uState(0)
{
	SetNote( coil.m_strNote );
}

void CInputCoil::Draw( LPDrawInfo pInfo, SIZE &posOffset )
{
	RECT rc;
	CSymbolCoil::eSymbolType eSymbol;
	int nxpos,nypos;
	nxpos = posOffset.cx + pInfo->offsetMid.cx;
	nypos = posOffset.cy + pInfo->offsetMid.cy;

	if( m_uState & STATE_COIL_CLOSE )
	{
		rc.left = nxpos - pInfo->nSpase + 1;
		rc.top = nypos;
		rc.right = nxpos + pInfo->nSpase;
		rc.bottom = nypos + pInfo->nUnitLen;
		FillRect( pInfo->hdc, &rc,(HBRUSH) (COLOR_GRAYTEXT) );
	}

	//if(1)
	if( m_eInputType & eFnc )
	{
		CBaseCoil::DrawFrame( pInfo, posOffset,CBaseCoil::eFnc,m_nColOccupation );
		if( !m_strName.IsEmpty() )
		{
			rc.left = posOffset.cx + pInfo->nOutPutLine + 1;
			rc.top = posOffset.cy + pInfo->offsetMid.cy;
			rc.right = rc.left + ( pInfo->nColLen * m_nColOccupation )- pInfo->nOutPutLine - pInfo->nOutPutLine;
			rc.bottom = rc.top + ( pInfo->nLineOffset >> 1 );
			DrawText( pInfo->hdc,m_strName,-1,&rc,DT_LEFT | DT_VCENTER | DT_SINGLELINE); 
		}
	}else{
		CBaseCoil::DrawFrame( pInfo, posOffset,CBaseCoil::eInputCoil,m_nColOccupation );
		if( eOpen != m_eInputType )
		{
			if( m_eInputType & eClose )
				eSymbol = CSymbolCoil::eNot;
			else if( m_eInputType & eUpPluse )
				eSymbol = CSymbolCoil::eUpPluse;
			else
				eSymbol = CSymbolCoil::eDownPluse;
			CSymbolCoil::DrawSymbol( pInfo,eSymbol, posOffset );
		}
		CBaseCoil::Draw( pInfo, posOffset );
	}
}



COutputCoil::COutputCoil( eOutputType eType, CStdString strName, int nOccupation ) 
	: m_eOutputType(eType),CBaseCoil( COIL_OUTPUT ),m_uState(0)
{
	if( m_eOutputType == eFnc )
		m_nColOccupation = nOccupation;
	SetName( strName );
}
COutputCoil::COutputCoil( const COutputCoil& coil ) :
CBaseCoil( coil ),m_uState(0)
{

}

BOOL COutputCoil::SetFnc( CStdString strFnc )
{
	if( m_eOutputType == eFnc && strFnc )
	{
		SetName(strFnc);
		return TRUE;
	}
	return FALSE;
}
BOOL COutputCoil::SetArg( CStdString strArg )
{
	m_strArg = strArg;
	return TRUE;
}
void COutputCoil::Draw( LPDrawInfo pInfo, SIZE &posOffset )
{
	int nypos;
	int rPos;
	RECT rc;

	if( m_uState & STATE_COIL_CLOSE )
	{
		RECT rc;
		nypos = posOffset.cy + pInfo->offsetMid.cy;
		rPos = posOffset.cx + m_nColOccupation * pInfo->nColLen - 1;

		rc.left = posOffset.cx + pInfo->nOutPutLine;
		rc.top = nypos;
		rc.right = rPos - pInfo->nOutPutLine + 1;
		rc.bottom = nypos + pInfo->nUnitLen + 1;
		FillRect( pInfo->hdc, &rc,(HBRUSH) (COLOR_GRAYTEXT) );
	}

	if( eCoil == m_eOutputType )
	{
		CBaseCoil::DrawFrame( pInfo, posOffset,CBaseCoil::eOutputCoil,m_nColOccupation );
	}else if( eFnc == m_eOutputType )
	{
		CBaseCoil::DrawFrame( pInfo, posOffset,CBaseCoil::eFnc ,m_nColOccupation );
		//fnc
		/*rc.left = posOffset.cx + pInfo->nOutPutLine;
		rc.top = posOffset.cy + pInfo->offsetMid.cy;
		rc.right = rc.left + ( pInfo->nColLen * m_nColOccupation )- pInfo->nOutPutLine - pInfo->nOutPutLine;
		rc.bottom = rc.top + ( pInfo->nLineOffset >> 1 );
		if( m_strName )
			DrawText( pInfo->hdc,m_strName,-1,&rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE); */
	}
	//CBaseCoil::Draw( pInfo, posOffset );
	if( !m_strName.IsEmpty() )
	{
		rc.left = posOffset.cx + pInfo->nOutPutLine;
		rc.top = posOffset.cy + pInfo->offsetMid.cy;
		rc.right = rc.left + ( pInfo->nColLen * m_nColOccupation )- pInfo->nOutPutLine - pInfo->nOutPutLine;
		rc.bottom = rc.top + ( pInfo->nLineOffset >> 1 );
		DrawText( pInfo->hdc,m_strName,-1,&rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
	if( !m_strArg.IsEmpty() )
	{
		rc.left = posOffset.cx;
		rc.top = posOffset.cy;
		rc.right = posOffset.cx + pInfo->nColLen * m_nColOccupation;
		rc.bottom = rc.top + pInfo->offsetMid.cy;
		DrawText( pInfo->hdc,m_strArg,-1,&rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
	if( !m_strNote.IsEmpty() )
	{
		rc.left = posOffset.cx ;
		rc.top = posOffset.cy + pInfo->nUnitLen * 3;
		rc.right = posOffset.cx + pInfo->nColLen * m_nColOccupation;
		rc.bottom = posOffset.cy + pInfo->nRowLen;
		//Rectangle(pInfo->hdc,rcName.left,rcName.top,rcName.right,rcName.bottom);
		DrawText( pInfo->hdc,m_strNote,-1,&rc,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
}

CSymbolCoil::CSymbolCoil( eSymbolType nLineType,CStdString strName ) : 
m_nSymbolType(nLineType),
CBaseCoil( COIL_SYMBOL ),m_uState(0)
{
	SetName( strName );
}
CSymbolCoil::CSymbolCoil( const CSymbolCoil& coil ) :
CBaseCoil( coil ),m_uState(0)
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

CNoteCoil::CNoteCoil(CStdString strNote ) :
CBaseCoil( COIL_NOTE )
{
	SetOutNote( strNote );
}
CNoteCoil::CNoteCoil( const CNoteCoil& coil ) :
CBaseCoil( coil )
{

}
BOOL CNoteCoil::SetOutNote( CStdString strNote )
{
	SetNote(strNote);
	return TRUE;
}
void CNoteCoil::Draw(LPDrawInfo pInfo, SIZE &posOffset )
{
	//RECT rc;
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

