// LadView.cpp : implementation file
//

#include "stdafx.h"
#include "PLCSimulation.h"
#include "LadView.h"
#include "afxdialogex.h"
#include "PLCSimulationDlg.h"


// CLadView dialog

IMPLEMENT_DYNAMIC(CLadView, CDialog)

CLadView::CLadView(CWnd* pParent /*=NULL*/)
	: CDialog(CLadView::IDD, pParent)
{
	m_psFocuse.nLn = 0;
	m_psFocuse.nCol = 0;
	m_lstUpdate[0] = 0;
	m_nlstUpdate = 0;
	m_nlstFound = 0;
}

CLadView::~CLadView()
{
}

void CLadView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLadView, CDialog)
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CLadView message handlers


void CLadView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nCol;
	SCROLLINFO scrollinfo;
	scrollinfo.cbSize=sizeof(SCROLLINFO);
	GetScrollInfo(SB_VERT,&scrollinfo,SIF_POS);
	nPos=scrollinfo.nPos;
	switch( nSBCode )
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		GetScrollInfo(SB_VERT,&scrollinfo,SIF_TRACKPOS);
		nPos=scrollinfo.nTrackPos;
		break;
	case SB_LINEDOWN:	nPos++;		break;
	case SB_LINEUP:		nPos--;		break;
	case SB_PAGEDOWN:	nPos += 4;	break;
	case SB_PAGEUP:		nPos -= 4;	break;
	case SB_ENDSCROLL:
		m_schematic.GetViewPos( NULL, &nCol );
		UpdateView( nPos, nCol );
		break;
	}
	SetScrollPos( SB_VERT,nPos );
	
}


BOOL CLadView::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_trackDc.SetDrawFact( TrackDraw );
	m_memDc.Select( this->GetDC()->GetSafeHdc() );
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CLadView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//绘制查找结果
	if( !m_strFind.IsEmpty() && m_nlstFound )
	{
		RECT rc;
		for ( int i = 0; i < m_nlstFound; ++i )
		{
			m_schematic.GetPosCoil( m_lstFound[i], &rc );
			::DrawFocusRect( m_memDc.GetDC(), &rc );
		}
	}
	m_schematic.DrawHelp( dc.GetSafeHdc(),m_memDc.GetDC() );
	//m_trackDc.Draw( dc.GetSafeHdc() );
}


BOOL CLadView::LoadPLC( CString strProgram )
{
	BOOL bRes;
	char* buf;
	FILE* fb;
	fb = _wfopen( strProgram,_T("rb") );
	if( fb == NULL )
		return FALSE;
	fseek( fb, 0, SEEK_END );
	size_t len = ftell( fb );
	fseek( fb, 0, SEEK_SET );
	buf = new char[ len + 1 ];
	fread( buf, sizeof( char ), len, fb );
	buf[ len ] = 0;
	bRes = m_schematic.FromInsBuff( buf, len );
	if( bRes )
	{
		CClientDC dc(this);
		m_psFocuse.nCol = 0;
		m_psFocuse.nLn = 0;
		m_schematic.Draw( m_memDc.GetDC(),TRUE );
		m_schematic.GetPosCoil( m_psFocuse, &m_rcFocuse );
		//m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
		UpdataScroll();
		//更新
		wchar_t* pfnc;
		wchar_t* pIndex;
		CStdString strFnc;
		unsigned int coil,addr;
		libSch::LPCoilLine pLine;
		CBaseCoil* pCoil;
		FlagUpdate uFlag;
		int nMaxLn;
		int szCol;
		uFlag.all = 0;
		uFlag.bUpdate = 1;
		szCol = m_schematic.GetTotalCol() + 1;
		nMaxLn = m_schematic.GetTotalRow();
		for ( int nLn = 0; nLn < nMaxLn; ++nLn )
		{
			pLine = m_schematic.GetLine( nLn );
			if( pLine == NULL )
				break;;
			for( int nCol = 1; nCol < szCol; ++nCol )
			{
				uFlag.dataFlag = 0;
				pCoil = pLine[ nCol ].hCoil;
				UINT uType = libSch::CCoilBlock::GetCoilType( pCoil );
				if( uType == COIL_INPUT  )
				{
					if( libSch::CSchematic::TestCoilType( ARG_COIL_T | ARG_COIL_C | ARG_COIL_X | ARG_COIL_Y | ARG_COIL_S | ARG_COIL_M,
						pCoil->GetName(),&coil, &addr ) )
					{
						uFlag.bUpdate = 1;
						uFlag.coil = coil;
						uFlag.addr = addr;
						pCoil->m_userData = (void*)uFlag.all;
					}
				}else if( uType == COIL_OUTPUT )
				{
					if( static_cast<COutputCoil*>(pCoil)->GetOutputType() == COutputCoil::eCoil )
					{
						if( libSch::CSchematic::TestCoilType( ARG_COIL_T | ARG_COIL_C | ARG_COIL_X | ARG_COIL_Y | ARG_COIL_S | ARG_COIL_M,
							pCoil->GetName(),&coil, &addr ) )
						{
							uFlag.bUpdate = 1;
							uFlag.coil = coil;
							uFlag.addr = addr;
							pCoil->m_userData = (void*)uFlag.all;
						}
					}else
					{
						strFnc = pCoil->GetName();
						pfnc = (wchar_t*)strFnc.GetBuffer();
						pIndex = pfnc;
						while( *pIndex != L' ' && *pIndex != L'\t' && *pIndex != L'\0' ) ++pIndex;
						*pIndex = L'\0';
						if( wcscmp( pfnc, L"SET" ) == 0
							|| wcscmp( pfnc, L"STL" ) == 0
							|| wcscmp( pfnc, L"RST" ) == 0 )
						{
							++pIndex;
							while( ( *pIndex == L' ' || *pIndex == L'\t' ) && *pIndex != L'\0' ) ++pIndex;
							pfnc = pIndex;
							while( *pIndex != L' ' && *pIndex != L'\t' && *pIndex != L'\0' ) ++pIndex;
							*pIndex = L'\0';
							if( libSch::CSchematic::TestCoilType( ARG_COIL_T | ARG_COIL_C | ARG_COIL_X | ARG_COIL_Y | ARG_COIL_S | ARG_COIL_M,
								CStdString( pfnc ),&coil, &addr ) )
							{
								uFlag.bUpdate = 1;
								uFlag.coil = coil;
								uFlag.addr = addr;
								pCoil->m_userData = (void*)uFlag.all;
							}
						}else
							pCoil->m_userData = (void*)uFlag.all;
					}
				}
			}
		}
		UpdataCoilList();
	}
	delete[] buf;
	return bRes;
}


void CLadView::OnSize(UINT nType, int cx, int cy)
{
	CClientDC dc(this);
	CDialog::OnSize(nType, cx, cy);
	m_memDc.OnSize( cx, cy );
	m_schematic.SetViewSize( cx,cy );
	m_schematic.Draw( m_memDc.GetDC() );
	m_schematic.GetPosCoil( m_psFocuse, &m_rcFocuse );
	//m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
	UpdataScroll();
	// TODO: Add your message handler code here
}

BOOL CLadView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	return CDialog::OnEraseBkgnd(pDC);
}



void CLadView::UpdataScroll(void)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | SIF_PAGE;
	si.nMin   = 0;
	si.nMax   = m_schematic.GetTotalRow();
	si.nPage  = 1;
	SetScrollInfo( SB_VERT, &si, TRUE);
	ShowScrollBar( SB_VERT, TRUE );
}


BOOL CLadView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	int nLn,nCol;
	m_schematic.GetViewPos( &nLn, &nCol );
	nLn -= zDelta / WHEEL_DELTA;
	UpdateView( nLn, nCol );
	SetScrollPos( SB_VERT,nLn );
	return TRUE;
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CLadView::UpdateView(int nLen, int nCol)
{
	if( m_schematic.SetViewPos( nLen, nCol ) )
	{
		UpdataCoilList();
		UpdateFindList();
		CClientDC dc(this);
		m_schematic.Draw( m_memDc.GetDC() );
		m_schematic.GetPosCoil( m_psFocuse, &m_rcFocuse );
		//m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
		InvalidateRect(NULL,FALSE);
	}
}


void CLadView::TrackDraw(HDC hdc, RECT rt)
{
	HPEN hPen, hPenOld;
	hPen = CreatePen( PS_SOLID, 2, RGB(0,24,255) );
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	Rectangle(hdc,rt.left,rt.top,rt.right,rt.bottom );
	SelectObject(hdc, hPenOld);
	DeleteObject(hPen);
}


void CLadView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//CClientDC dc(this);
	CStdString str;
	CBaseCoil* pCoil = m_schematic.GetPosCoil( point,m_psFocuse, &m_rcFocuse );
	//m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
	::SetFocus( GetParent()->m_hWnd );
	CDialog::OnLButtonDown(nFlags, point);
	CPLCSimulationDlg* pDlg = static_cast<CPLCSimulationDlg*>( AfxGetApp()->GetMainWnd() );
	str.Format(_T("%d"), m_psFocuse.nLn + 1 );
	pDlg->m_wndStatusBar.SetPaneText( CPLCSimulationDlg::STATUSE_LN_NUM, str );
	str.Format(_T("%d"), m_psFocuse.nCol + 1 );
	pDlg->m_wndStatusBar.SetPaneText( CPLCSimulationDlg::STATUSE_COL_NUM, str );
}


void CLadView::OnPLCScanEnd(void)
{
	bool bUpdate = false;
	unsigned char bState;
	FlagUpdate uFlag;
	UINT uType;
	CBaseCoil** ppCoil = m_lstUpdate;
	if( !IsWindowVisible() )
		return;
	for( int i = 0; i < m_nlstUpdate; ++i,++ppCoil )
	{
		if( ppCoil )
		{
			uFlag.all = (UINT32)(*ppCoil)->m_userData;
			uType = libSch::CCoilBlock::GetCoilType( *ppCoil );
			if( uType == COIL_INPUT  )
			{
				bState = lib_API_ReadBit( uFlag.coil, uFlag.addr );
				if( static_cast<CInputCoil*>(*ppCoil)->GetInputType() == CInputCoil::eClose )
					bState = ~bState;
				if( static_cast<CInputCoil*>(*ppCoil)->GetState() != bState )
				{
					static_cast<CInputCoil*>(*ppCoil)->SetState( bState );
					bUpdate = true;
				}
			}else if( uType == COIL_OUTPUT )
			{
				bState = lib_API_ReadBit( uFlag.coil, uFlag.addr );
				if( static_cast<COutputCoil*>(*ppCoil)->GetOutputType() == COutputCoil::eCoil && static_cast<COutputCoil*>(*ppCoil)->GetState() != bState )
				{
					static_cast<COutputCoil*>(*ppCoil)->SetState( bState );
					bUpdate = true;
				}
			}
		}
	}
	if( bUpdate )
	{
		m_schematic.Draw( m_memDc.GetDC(),TRUE );
		InvalidateRect(NULL,FALSE);
	}
	//
	//m_schematic.l
}


void CLadView::UpdataCoilList(void)
{
	UINT uType;
	libSch::LPCoilLine pLine;
	CBaseCoil* pCoil;
	int nLn;
	int nCol;
	int szCol;
	FlagUpdate uFlag;
	libSch::CoilPos pos;

	m_nlstUpdate = 0;
	m_schematic.GetViewPos( &nLn,NULL );
	int count = m_schematic.GetViewRowCount();
	count += nLn;
	szCol = m_schematic.GetTotalCol() + 1;
	for ( ; nLn < count; ++nLn )
	{
		pLine = m_schematic.GetLine( nLn );
		if( pLine == NULL )
			break;
		for( nCol = 1; nCol < szCol; ++nCol )
		{
			pCoil = pLine[ nCol ].hCoil;
			uType = libSch::CCoilBlock::GetCoilType( pCoil );
			if( uType > COIL_BASE && uType != COIL_ERROR )
			{
				uFlag.all = UINT32( pCoil->m_userData );
				if( uFlag.bUpdate )
				{
					m_lstUpdate[ m_nlstUpdate++ ] = pCoil;
					if( m_nlstUpdate >= SZ_UPDATELIST )
					{
						OutputDebugString( TEXT( "CLadView m_lstUpdate overflow\n" ) );
						return;
					}
				}
				//pCoil->m_userData = (void*)uFlag.all;
			}
		}
	}
	
}


void CLadView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	FlagUpdate uFlag;
	CBaseCoil* pCoil = m_schematic.GetPosCoil( point,m_psFocuse, &m_rcFocuse );
	UINT uType = libSch::CCoilBlock::GetCoilType( pCoil );
	if( uType != COIL_ERROR && uType > COIL_BASE && pCoil->m_userData )
	{
		uFlag.all = (UINT)pCoil->m_userData;
		lib_API_WriteBit( uFlag.coil, uFlag.addr, !lib_API_ReadBit( uFlag.coil, uFlag.addr ) );
	}
	CDialog::OnLButtonDblClk(nFlags, point);
	CStdString str;
	::SetFocus( GetParent()->m_hWnd );
	CPLCSimulationDlg* pDlg = static_cast<CPLCSimulationDlg*>( AfxGetApp()->GetMainWnd() );
	str.Format(_T("%d"), m_psFocuse.nLn + 1 );
	pDlg->m_wndStatusBar.SetPaneText( CPLCSimulationDlg::STATUSE_LN_NUM, str );
	str.Format(_T("%d"), m_psFocuse.nCol + 1 );
	pDlg->m_wndStatusBar.SetPaneText( CPLCSimulationDlg::STATUSE_COL_NUM, str );
}


void CLadView::GetErr( CStdString& strErr )
{
	strErr = m_schematic.GetErr();
}


BOOL CLadView::FindCoil(CString& strFind )
{
	unsigned int coil;
	unsigned int addr;
	CStdString str;
	if( !IsWindowVisible() || strFind.IsEmpty() )
		return FALSE;
	strFind.MakeUpper();
	m_nlstFound = 0;
	str.Assign( strFind );
	if( !libSch::CSchematic::TestCoilType( ARG_COIL_X | ARG_COIL_Y | ARG_COIL_S | ARG_COIL_M | ARG_COIL_D | ARG_COIL_T | ARG_COIL_C ,
		str,&coil, &addr ) )
	{
		MessageBox( _T("元件格式错误"),_T("错误"), MB_OK );
		return FALSE;
	}else
	{
		if( strFind[0] == _T('X') )
			m_strFind.Format(_T("%c%03o"), strFind[0], addr );
		else
			m_strFind.Format(_T("%c%03d"), strFind[0], addr );
	}
	UpdateFindList();
	return FALSE;
}


void CLadView::UpdateFindList(void)
{
	UINT uType;
	libSch::LPCoilLine pLine;
	CBaseCoil* pCoil;
	int nLn;
	int nCol;
	int szCol;
	FlagUpdate uFlag;
	libSch::CoilPos pos;

	m_nlstFound = 0;
	m_schematic.GetViewPos( &nLn,NULL );
	int count = m_schematic.GetViewRowCount();
	count += nLn;
	szCol = m_schematic.GetTotalCol() + 1;
	for ( ; nLn < count; ++nLn )
	{
		pLine = m_schematic.GetLine( nLn );
		if( pLine == NULL )
			break;
		for( nCol = 1; nCol < szCol; ++nCol )
		{
			pCoil = pLine[ nCol ].hCoil;
			uType = libSch::CCoilBlock::GetCoilType( pCoil );
			if( uType > COIL_BASE && uType != COIL_ERROR )
			{
				uFlag.all = UINT32( pCoil->m_userData );
				//判断查找
				if( !m_strFind.IsEmpty() )
				{
					if( m_nlstFound >= SZ_UPDATELIST )
					{
						OutputDebugString( TEXT( "CLadView m_nlstFound overflow\n" ) );
						return;
					}
					if(	( uType == COIL_INPUT && m_strFind == pCoil->GetName() )
						||	( uType == COIL_OUTPUT 
								&& ( pCoil->GetName().Find( m_strFind ) != -1 
									|| pCoil->GetNote().Find( m_strFind ) != -1 ) 
							)
						||	( uType == COIL_SYMBOL && m_strFind == pCoil->GetName() ) )
					{
						m_lstFound[m_nlstFound].nCol = nCol;
						m_lstFound[m_nlstFound].nLn = nLn;
						++m_nlstFound;
						uFlag.bFound = 1;
					}
				}else
					uFlag.bFound = 0;

				pCoil->m_userData = (void*)uFlag.all;
			}

		}
	}
	m_schematic.Draw( m_memDc.GetDC(),TRUE );
}
