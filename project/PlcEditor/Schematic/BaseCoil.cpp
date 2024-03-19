#include "StdAfx.h"
#include "BaseCoil.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBaseCoil::CBaseCoil( UINT uType,LPTSTR strName ): 
m_strName(NULL),
m_uCoilType(uType),m_uMark(0),
m_nColOccupation(1),m_strNote(NULL)
{
	SetName( strName );
	EmptyLink();
}
CBaseCoil::CBaseCoil( const CBaseCoil& coil ): 
m_strName(NULL),
m_uCoilType(coil.m_uCoilType),m_uMark(0),
m_nColOccupation(coil.m_nColOccupation),m_strNote(NULL)
{
	SetName( coil.m_strName );
	SetNote( coil.m_strNote );
}
BOOL CBaseCoil::PrepareDrawInfo(LPDrawInfo pInfo, int nRowLen, int nColLen )
{
	if( pInfo == NULL )
		return FALSE;
	pInfo->nColLen = nColLen;
	pInfo->nRowLen = nRowLen;
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

BOOL CBaseCoil::SetName( LPTSTR str ){ 
	if( str == NULL ) return FALSE;
	int n = _tcslen(str); 
	if ( n < NAME_MAX_LEN )
	{
		if ( m_strName && (int)_tcslen(m_strName) < n ) delete[] m_strName,m_strName = NULL;
		if( m_strName == NULL ) m_strName = new TCHAR[ n + 1];
		_tcscpy( m_strName, str );
		return TRUE;
	}
	return FALSE;
}
BOOL CBaseCoil::SetNote( LPTSTR str ){
	if( str == NULL ) return FALSE;
	int n = _tcslen(str); 
	if ( n < NAME_MAX_LEN )
	{
		if ( m_strNote && (int)_tcslen(m_strNote) < n ) delete[] m_strNote,m_strNote = NULL;
		if( m_strNote == NULL ) m_strNote = new TCHAR[ n + 1];
		_tcscpy( m_strNote, str );
		return TRUE;
	}
	return FALSE;
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
	if( m_strName == NULL )
		return;
	if( m_strName )
	{
		rcName.left = posOffset.cx;
		rcName.top = posOffset.cy;
		rcName.right = posOffset.cx + pInfo->nColLen * m_nColOccupation;
		rcName.bottom = rcName.top + pInfo->offsetMid.cy;
		DrawText( pInfo->hdc,m_strName,-1,&rcName,DT_CENTER | DT_VCENTER | DT_SINGLELINE); 
	}
	if( m_strNote )
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