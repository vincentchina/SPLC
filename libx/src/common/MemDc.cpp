
#include "common\MemDc.h"


CMemDc::CMemDc(void) :
	m_hdc(NULL),
	m_membmp(NULL)
{
	m_szMax.cx = 0;
	m_szMax.cy = 0;
}

CMemDc::~CMemDc(void)
{
	if( m_hdc != NULL )
		DeleteDC(m_hdc);
	if( m_membmp != NULL )
		DeleteObject(m_membmp);
}

BOOL CMemDc::Select( HDC hdc, int cx, int cy )
{
	if( m_hdc != NULL )
		DeleteDC(m_hdc);
	if( m_membmp != NULL )
		DeleteObject(m_membmp);
	m_hdc = CreateCompatibleDC(hdc);
	if( m_hdc )
	{
		if( cx < 0 ) cx = GetSystemMetrics(SM_CXSCREEN);
		if( cy < 0 ) cy = GetSystemMetrics(SM_CYSCREEN);
		m_szMax.cx = cx;
		m_szMax.cy = cy;
		m_membmp = CreateCompatibleBitmap(hdc,m_szMax.cx,m_szMax.cy );
		if( m_membmp == NULL )
			return FALSE;
		SelectObject(m_hdc,m_membmp);
	}else
		return FALSE;
	return TRUE;
}

void CMemDc::OnSize( int cx, int cy )
{
	HBITMAP memBmp;
	if( m_hdc == NULL )
		return;
	if( cx > m_szMax.cx || cy > m_szMax.cy )
	{
		m_szMax.cx = cx;
		m_szMax.cy = cy;
		memBmp = CreateCompatibleBitmap( m_hdc,m_szMax.cx,m_szMax.cy );
		if( memBmp == NULL )
			return;
		SelectObject(m_hdc,memBmp);
		if( m_membmp != NULL )
			DeleteObject(m_membmp);
	}
}
SIZE CMemDc::GetSize()
{
	return m_szMax;
}

BOOL CMemDc::BitBlt( HDC hdcDest, int x, int y, int cx, int cy, int x1, int y1, DWORD rop)
{
	return ( m_hdc && ::BitBlt( hdcDest, x, y, cx, cy, m_hdc,x1, y1, rop ));
}
BOOL CMemDc::StretchBlt(  HDC hdcDest,  int nXOriginDest,  int nYOriginDest,  int nWidthDest,  int nHeightDest,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc,  DWORD dwRop)
{
	return (m_hdc &&::StretchBlt( hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, m_hdc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, dwRop ));
}


TrackDc::TrackDc()
{
	memset( &m_rcOld, 0, sizeof(RECT) );
	m_pDraw = NULL;
}
TrackDc::~TrackDc()
{
}
TrackDc::PDrawFact TrackDc::SetDrawFact( PDrawFact pDrawFact )
{
	PDrawFact pOld = m_pDraw;
	m_pDraw = pDrawFact;
	return pOld;
}
void TrackDc::Draw( HDC hdc )
{
	if( m_pDraw == NULL || IsRectEmpty(&m_rcOld ) )
		return;
	int oldRop = SetROP2( hdc,R2_NOTXORPEN  );
	m_pDraw( hdc,m_rcOld );
	SetROP2( hdc,oldRop );
}
void TrackDc::TrackDraw( HDC hdc, RECT& rc )
{
	if( m_pDraw == NULL )
		return;
	int oldRop = SetROP2( hdc,R2_NOTXORPEN  );
	if( !IsRectEmpty( &m_rcOld ) )
		m_pDraw(hdc,m_rcOld);
	m_pDraw(hdc,rc);
	SetROP2( hdc,oldRop );
	m_rcOld = rc;
}
void TrackDc::Remove( HDC hdc )
{
	if( m_pDraw == NULL )
		return;
	int oldRop = SetROP2( hdc,R2_NOTXORPEN  );
	if( !IsRectEmpty( &m_rcOld ) )
		m_pDraw(hdc,m_rcOld);
	SetROP2( hdc,oldRop );
	memset( &m_rcOld, 0, sizeof(RECT) );
}