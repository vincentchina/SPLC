#include "StdAfx.h"
#include "ViewMemDc.h"


CViewMemDc::CViewMemDc(void) :
	m_hdc(NULL),
	m_membmp(NULL)
{
	m_szMax.cx = 800;
	m_szMax.cy = 600;
	m_hdc = CreateCompatibleDC(NULL);
	//int nScreenWidth = GetDeviceCaps(m_hdc, HORZRES);  
	//int nScreenHeight = GetDeviceCaps(m_hdc, VERTRES);  
	m_membmp = CreateCompatibleBitmap(m_hdc,m_szMax.cx,m_szMax.cy );
	SelectObject(m_hdc,m_membmp);
}

CViewMemDc::~CViewMemDc(void)
{
	if( m_hdc != NULL )
		DeleteDC(m_hdc);
	if( m_membmp != NULL )
		DeleteObject(m_membmp);
}

void CViewMemDc::SetSize( int nx, int ny )
{
	if( m_szMax.cx < nx || m_szMax.cy < ny )
	{
		m_szMax.cx = nx;
		m_szMax.cy = ny;
		DeleteObject( m_membmp );
		m_membmp = CreateCompatibleBitmap(m_hdc,m_szMax.cx,m_szMax.cy );
		SelectObject(m_hdc,m_membmp);
	}
}

SIZE CViewMemDc::GetSize()
{
	return m_szMax;
}

BOOL CViewMemDc::BitBlt( HDC hdcDest, int x, int y, int cx, int cy, int x1, int y1, DWORD rop)
{
	return ( m_hdc && ::BitBlt( hdcDest, x, y, cx, cy, m_hdc,x1, y1, rop ));
}
BOOL CViewMemDc::StretchBlt(  HDC hdcDest,  int nXOriginDest,  int nYOriginDest,  int nWidthDest,  int nHeightDest,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc,  DWORD dwRop)
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
void TrackDc::Remove()
{
	memset( &m_rcOld, 0, sizeof(RECT) );
}