#include "plcSchematic\BaseCoil.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif*/


CBaseCoil::CBaseCoil( UINT uType,CStdString strName ): 
m_uCoilType(uType),m_userData(0),
m_nColOccupation(1)
{
	SetName( strName );
	EmptyLink();
}
CBaseCoil::CBaseCoil( const CBaseCoil& coil ): 
m_uCoilType(coil.m_uCoilType),m_userData(0),
m_nColOccupation(coil.m_nColOccupation)
{
	SetName( coil.m_strName );
	SetNote( coil.m_strNote );
}
BOOL CBaseCoil::PrepareDrawInfo(LPDrawInfo pInfo, int nRowLen, int nColLen, eDrawUnitType eDUType )
{
	if( pInfo == NULL )
		return FALSE;
	pInfo->nColLen = nColLen;
	pInfo->nRowLen = nRowLen;
	pInfo->eDUType = eDUType;
	pInfo->nUnitLen = (int)( pInfo->nRowLen / 8.0 );
	pInfo->nLineOffset = pInfo->nColLen >> 1;
	pInfo->nSpase = pInfo->nColLen / 10;
	pInfo->szArrow.cx = (int)(( pInfo->nLineOffset + 0.5 ) / 8.0);
	pInfo->szArrow.cy = (int)(( pInfo->nLineOffset + 0.5 ) / 5.0);
	pInfo->offsetMid.cx = ( pInfo->nColLen >> 1 );
	pInfo->offsetMid.cy = pInfo->nLineOffset - ( pInfo->nLineOffset >> 2 );
	pInfo->nOutPutLine = pInfo->nColLen / 20;
	pInfo->nHeightVline = pInfo->nRowLen;	//默认值
	if( pInfo->nOutPutLine == 0 ) pInfo->nOutPutLine = 2;
	pInfo->nRadius = ( pInfo->nOutPutLine << 1 ) + 2;
	pInfo->hFont = CreateFont(pInfo->nUnitLen, // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename;
	pInfo->colorFont = RGB(255,200,0);
	if( pInfo->hFont == NULL )
		return FALSE;
	return TRUE;
}
void CBaseCoil::DeleteDrawInfo( LPDrawInfo pInfo )
{
	if( pInfo != NULL )
	{
		DeleteObject(pInfo->hFont);
	}
}

BOOL CBaseCoil::BeginPaint( HDC hdc,LPDrawInfo pInfo )
{
	if( pInfo == NULL )
		return FALSE;
	pInfo->hOldFont = (HFONT)SelectObject( hdc,pInfo->hFont );
	pInfo->hdc = hdc;
	pInfo->colorOldFont = SetTextColor( hdc,pInfo->colorFont );
	SetBkMode( hdc,TRANSPARENT );
	return TRUE;
}
void CBaseCoil::EndPaint( LPDrawInfo pInfo )
{
	SelectObject( pInfo->hdc,pInfo->hOldFont );
	SetTextColor( pInfo->hdc,pInfo->colorOldFont );
	pInfo->hdc = NULL;
}

void CBaseCoil::DrawLine( LPDrawInfo pInfo, UINT uType,SIZE &posOffset ){
	if( uType & COIL_VLINE ){
		MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,posOffset.cx, posOffset.cy + pInfo->nHeightVline + pInfo->nLineOffset );
	}
	if ( uType & COIL_HLINE ){
		MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,posOffset.cx + pInfo->nColLen, posOffset.cy + pInfo->nLineOffset );
	}
}
void CBaseCoil::Draw(LPDrawInfo pInfo, SIZE &posOffset )
{
	RECT rcName;
	//draw name
	if( !m_strName.IsEmpty() )
	{
		const wchar_t* p = m_strName;
		rcName.left = posOffset.cx;
		rcName.top = posOffset.cy;
		rcName.right = posOffset.cx + pInfo->nColLen * m_nColOccupation;
		rcName.bottom = rcName.top + pInfo->offsetMid.cy;
		DrawText( pInfo->hdc,m_strName,-1,&rcName,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
	if( !m_strNote.IsEmpty() )
	{
		rcName.left = posOffset.cx ;
		rcName.top = posOffset.cy + pInfo->nUnitLen * 3;
		rcName.right = posOffset.cx + pInfo->nColLen * m_nColOccupation;
		rcName.bottom = posOffset.cy + pInfo->nRowLen;
		//Rectangle(pInfo->hdc,rcName.left,rcName.top,rcName.right,rcName.bottom);
		DrawText( pInfo->hdc,m_strNote,-1,&rcName,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
	DrawLine( pInfo,m_uCoilType,posOffset );
}

void CBaseCoil::DrawFrame( LPDrawInfo pInfo, SIZE &posOffset,eFrameType eType,int nColOccupation )
{
	int nxpos,nypos;
	nxpos = posOffset.cx + pInfo->offsetMid.cx;
	nypos = posOffset.cy + pInfo->offsetMid.cy;
	if( eType == eInputCoil )
	{
		// -
		MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,nxpos - pInfo->nSpase, posOffset.cy + pInfo->nLineOffset );
		// -
		MoveToEx( pInfo->hdc,nxpos + pInfo->nSpase,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,posOffset.cx + pInfo->nColLen, posOffset.cy + pInfo->nLineOffset );
		// |
		MoveToEx( pInfo->hdc,nxpos - pInfo->nSpase ,nypos,NULL);
		LineTo(pInfo->hdc,nxpos - pInfo->nSpase, nypos + pInfo->nUnitLen );
		// |
		MoveToEx( pInfo->hdc,nxpos + pInfo->nSpase ,nypos,NULL);
		LineTo(pInfo->hdc,nxpos + pInfo->nSpase, nypos + pInfo->nUnitLen );
	}else if( eType == eFnc ){
		// -
		MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,posOffset.cx + pInfo->nOutPutLine, posOffset.cy + pInfo->nLineOffset );
		// -
		MoveToEx( pInfo->hdc,posOffset.cx + nColOccupation * pInfo->nColLen - pInfo->nOutPutLine,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,posOffset.cx + nColOccupation * pInfo->nColLen, posOffset.cy + pInfo->nLineOffset );
		nypos = posOffset.cy + pInfo->offsetMid.cy;
		// [
		MoveToEx( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine + pInfo->nOutPutLine ,nypos,NULL);
		LineTo( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine ,nypos );
		LineTo( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine ,nypos + pInfo->nUnitLen );
		LineTo( pInfo->hdc,posOffset.cx + pInfo->nOutPutLine + pInfo->nOutPutLine ,nypos + pInfo->nUnitLen);
		// ]
		nxpos = posOffset.cx + nColOccupation * pInfo->nColLen - 1;
		MoveToEx( pInfo->hdc,nxpos - pInfo->nOutPutLine - pInfo->nOutPutLine ,nypos,NULL);
		LineTo( pInfo->hdc,nxpos - pInfo->nOutPutLine ,nypos );
		LineTo( pInfo->hdc,nxpos - pInfo->nOutPutLine ,nypos + pInfo->nUnitLen );
		LineTo( pInfo->hdc,nxpos - pInfo->nOutPutLine - pInfo->nOutPutLine ,nypos + pInfo->nUnitLen);
	}else{
		RECT rcArc;
		POINT ptStart,ptEnd;
		// -
		MoveToEx( pInfo->hdc,posOffset.cx ,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,posOffset.cx + pInfo->nOutPutLine, posOffset.cy + pInfo->nLineOffset );
		// -
		MoveToEx( pInfo->hdc,posOffset.cx + nColOccupation * pInfo->nColLen - pInfo->nOutPutLine,posOffset.cy + pInfo->nLineOffset,NULL);
		LineTo(pInfo->hdc,posOffset.cx + nColOccupation * pInfo->nColLen, posOffset.cy + pInfo->nLineOffset );
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
	}
}
void CBaseCoil::EmptyLink()
{
	m_bLinkIn = FALSE;
	m_pRightUp = NULL;
	m_pRight = NULL;
	m_pLeftDown = NULL;
}
//是否是孤儿节点
BOOL CBaseCoil::IsOrphan()
{
	return ( !m_bLinkIn 
		|| ( m_pLeftDown == NULL 
			&& m_pRight == NULL 
			&& m_pRightUp == NULL ) );
}