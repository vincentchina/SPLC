
// LADView.cpp : CLADView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "PlcEditor.h"
#endif

//#include "LADDoc.h"
#include "LADView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLADView

IMPLEMENT_DYNCREATE(CLADView, CView)

BEGIN_MESSAGE_MAP(CLADView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLADView::OnFilePrintPreview)

	ON_COMMAND(ID_EDIT_UNDO, &CLADView::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CLADView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CLADView::OnUpdateEditToolBar)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CLADView::OnUpdateEditToolBar)

	ON_UPDATE_COMMAND_UI(ID_INS_ARROW, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_CLOSE, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_ORCLOSE, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_OPEN, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_OROPEN, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_OUT, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_FUN, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_LINE, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_DEL, &CLADView::OnUpdateLadInsToolBar)
	ON_UPDATE_COMMAND_UI(ID_INS_LINEDEL, &CLADView::OnUpdateLadInsToolBar)
	ON_COMMAND(ID_INS_ARROW, &CLADView::OnInsArrow)
	ON_COMMAND(ID_INS_CLOSE, &CLADView::OnInsClose)
	ON_COMMAND(ID_INS_ORCLOSE, &CLADView::OnInsORClose)
	ON_COMMAND(ID_INS_OPEN, &CLADView::OnInsOpen)
	ON_COMMAND(ID_INS_OROPEN, &CLADView::OnInsOROpen)
	ON_COMMAND(ID_INS_OUT, &CLADView::OnInsOut)
	ON_COMMAND(ID_INS_FUN, &CLADView::OnInsFun)
	ON_COMMAND(ID_INS_LINE, &CLADView::OnInsLine)
	ON_COMMAND(ID_INS_DEL, &CLADView::OnInsDel)
	ON_COMMAND(ID_INS_LINEDEL, &CLADView::OnLineDel)

	ON_UPDATE_COMMAND_UI(ID_CONVERT_SETTING, &CLADView::OnUpdateConvertToolBar)
	ON_UPDATE_COMMAND_UI(ID_CONVERT, &CLADView::OnUpdateConvertToolBar)
	ON_UPDATE_COMMAND_UI(ID_COMPILE, &CLADView::OnUpdateConvertToolBar)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION, &CLADView::OnUpdateConvertToolBar)
	ON_COMMAND(ID_CONVERT_SETTING, &CLADView::OnSetting)
	ON_COMMAND(ID_CONVERT, &CLADView::OnComvert)
	ON_COMMAND(ID_COMPILE, &CLADView::OnCmpile)
	ON_COMMAND(ID_SIMULATION, &CLADView::OnSimulation)

	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
ON_COMMAND(ID_NEW_LAD_LINE, &CLADView::OnNewLadLine)
ON_WM_KEYDOWN()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CLADView 构造/析构

CLADView::CLADView():
	m_pSchematic(NULL)
{
	// TODO: 在此处添加构造代码
	m_psFocuse.nLn = 0;
	m_psFocuse.nCol = 0;
}

CLADView::~CLADView()
{
}

BOOL CLADView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	//SetScrollSizes(MM_TEXT,CSize(800,600));
	return CView::PreCreateWindow(cs);
}

// CLADView 绘制

void CLADView::OnDraw(CDC* pDC)
{
	CLADDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	CRect rectClient;
	GetClientRect(rectClient);
	/*CPoint point;
	OnPrepareDC(pDC);
	pDC->LPtoDP(&point);*/
	//m_pSchematic->Draw( m_memDc.GetDC() );
	//BitBlt( pDC->GetSafeHdc(), 0, 0, rectClient.Width(),rectClient.Height(), m_memDc.GetDC(),0, 0, SRCCOPY );
	m_pSchematic->DrawHelp( pDC->GetSafeHdc(),m_memDc.GetDC() );
	m_trackDc.Draw( pDC->GetSafeHdc() );
	//pDC->SetTextColor(RGB(250,200,0));
	//pDC->TextOut(0,0,"hello world");
	//pDC->Rectangle(rectClient);
}


// CLADView 打印


void CLADView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CLADView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CLADView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CLADView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CLADView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLADView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CLADView 诊断

#ifdef _DEBUG
void CLADView::AssertValid() const
{
	CView::AssertValid();
}

void CLADView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLADDoc* CLADView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLADDoc)));
	return (CLADDoc*)m_pDocument;
}
#endif //_DEBUG

// CLADView 消息处理程序


void CLADView::OnSize(UINT nType, int cx, int cy)
{
	CClientDC dc(this);
	//OnPrepareDC(&dc);
	CView::OnSize(nType, cx, cy);
	if( m_pSchematic )
	{
		m_memDc.OnSize( cx, cy );
		m_pSchematic->SetViewSize( cx,cy );
		m_pSchematic->GetPosCoil( m_psFocuse, &m_rcFocuse );
		m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
		m_pSchematic->Draw( m_memDc.GetDC() );
	}
}


void CLADView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
}


void CLADView::OnInitialUpdate()
{
	CLADDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	CClientDC dc(this);
	SIZE sz;
	CView::OnInitialUpdate();
	m_pSchematic = &pDoc->m_schematic;
	sz = m_pSchematic->GetViewSize();
	m_memDc.Select( dc.GetSafeHdc() );
	//m_pSchematic->Draw( m_memDc.GetDC() );
	m_trackDc.SetDrawFact( DrawFact );
	UpdataScroll();
	m_nInsCmdID = ID_INS_ARROW;
}

void CLADView::DrawFact( HDC hdc, RECT rt )
{
	HPEN hPen, hPenOld;
	hPen = CreatePen( PS_SOLID, 2, RGB(0,24,255) );
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	Rectangle(hdc,rt.left,rt.top,rt.right,rt.bottom );
	SelectObject(hdc, hPenOld);
	DeleteObject(hPen);
}

void CLADView::UpdateView( int nLn, int nCol )
{
	if( m_pSchematic->SetViewPos( nLn, nCol ) )
	{
		CClientDC dc(this);
		m_pSchematic->Draw( m_memDc.GetDC() );
		m_pSchematic->GetPosCoil( m_psFocuse, &m_rcFocuse );
		m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
		InvalidateRect(NULL,FALSE);
	}
}
void CLADView::UpdateView( )
{
	CClientDC dc(this);
	m_pSchematic->Draw( m_memDc.GetDC(),TRUE );
	m_pSchematic->GetPosCoil( m_psFocuse, &m_rcFocuse );
	m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
	InvalidateRect(NULL,FALSE);
}


void CLADView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc(this);
	CLADDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	CBaseCoil* pCoil = m_pSchematic->GetPosCoil( point,m_psFocuse, &m_rcFocuse );
	m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
	if( m_nInsCmdID != ID_INS_ARROW )
	{
		if( m_nInsCmdID == ID_INS_DEL )
		{
			if( nFlags & MK_SHIFT )
				m_pSchematic->DeleteLine( m_psFocuse,COIL_VLINE );
			else
				m_pSchematic->DeleteCoil( m_psFocuse );
		}else if( m_nInsCmdID == ID_INS_LINEDEL )
		{
			m_pSchematic->DeleteLine( m_psFocuse,( nFlags & MK_SHIFT ) ? COIL_VLINE : COIL_HLINE );
		}else if( m_nInsCmdID == ID_INS_LINE )
		{
			m_pSchematic->AddLine( m_psFocuse,( nFlags & MK_SHIFT ) ? COIL_VLINE : COIL_HLINE );
		}else
		{
			CBaseCoil* pCoilFocuse = m_pSchematic->GetPosCoil( m_psFocuse, &m_rcFocuse );
			if( (UINT)pCoilFocuse != COIL_ERROR )
			{
				libSch::eCoilInsType eType;
				switch( m_nInsCmdID )
				{
				case ID_INS_OPEN:
					eType = libSch::eOpen;
					break;
				case ID_INS_CLOSE:
					eType = libSch::eClose;
					break;
				case ID_INS_OROPEN:
					eType = libSch::eOROpen;
					break;
				case ID_INS_ORCLOSE:
					eType = libSch::eORClose;
					break;
				case ID_INS_OUT:
					eType = libSch::eOut;
					break;
				case ID_INS_FUN:
					eType = libSch::eFNC;
					break;
				default:
					eType = libSch::eNULL;
				}
				CDlgLadInput dlg( m_pSchematic,m_psFocuse, eType,_T(""),TRUE );
				ClientToScreen(&point);
				dlg.MoveDoModalWindow( point );
				if( dlg.DoModal() == IDOK )
				{
					m_pSchematic->OnInsInputChange( m_psFocuse, dlg.m_eType,dlg.m_strArg );
				}
			}
		}
		UpdateView();
		pDoc->SetModifiedFlag(TRUE);
	}
}

void CLADView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CString str;
	CInputCoil* pInputCoil;
	CBaseCoil* pCoilFocuse = m_pSchematic->GetPosCoil( m_psFocuse, &m_rcFocuse );
	if( (UINT)pCoilFocuse != COIL_ERROR )
	{
		UINT uType = libSch::CCoilBlock::GetCoilType( pCoilFocuse );
		libSch::eCoilInsType eType;
		if( uType == COIL_INPUT )
		{
			pInputCoil = (CInputCoil*)pCoilFocuse;
			if( pInputCoil->GetInputType() == CInputCoil::eClose )
				eType = libSch::eClose;
			else if( pInputCoil->GetInputType() == CInputCoil::eUpPluse )
				eType = libSch::ePCoil;
			else if( pInputCoil->GetInputType() == CInputCoil::eDownPluse )
				eType = libSch::eFCoil;
			else
				eType = libSch::eOpen;
		}else if( uType == COIL_OUTPUT )
		{
			if( ( (COutputCoil*)pCoilFocuse )->GetOutputType() == COutputCoil::eFnc )
				eType = libSch::eFNC;
			else
				eType = libSch::eOut;
		}else
			eType = libSch::eNULL;
		if( (UINT)pCoilFocuse > COIL_BASE )
		{
			str = pCoilFocuse->GetName();
			if( uType == COIL_OUTPUT && ( (COutputCoil*)pCoilFocuse )->IsHaveArg() )
			{
				str += _T(" ");
				str += ( (COutputCoil*)pCoilFocuse )->GetArg();
			}
		}else
			str.Empty();
		CDlgLadInput dlg( m_pSchematic,m_psFocuse, eType, str.GetBuffer() );
		str.ReleaseBuffer();
		ClientToScreen(&point);
		dlg.MoveDoModalWindow( point );
		if( dlg.DoModal() == IDOK )
		{
			m_pSchematic->OnInsInputChange( m_psFocuse, dlg.m_eType,dlg.m_strArg );
			UpdateView();
		}
		CView::OnLButtonDblClk(nFlags, point);
	}
}

void CLADView::UpdataScroll()
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | SIF_PAGE;
	si.nMin   = 0;
	si.nMax   = m_pSchematic->GetTotalRow();
	si.nPage  = 1;
	SetScrollInfo( SB_VERT, &si, TRUE);
	si.nMax   = 2;
	SetScrollInfo( SB_HORZ, &si, TRUE);
	//SetScrollPos( SB_VERT,20 );
	//m_pSchematic->SetViewPos( 1, 1 );
}
void CLADView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
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
		m_pSchematic->GetViewPos( NULL, &nCol );
		UpdateView( nPos, nCol );
		break;
	}
	SetScrollPos( SB_VERT,nPos );
	//CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CLADView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nLn;
	SCROLLINFO scrollinfo;
	scrollinfo.cbSize=sizeof(SCROLLINFO);
	GetScrollInfo(SB_HORZ,&scrollinfo,SIF_POS);
	nPos=scrollinfo.nPos;
	switch( nSBCode )
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		GetScrollInfo(SB_HORZ,&scrollinfo,SIF_TRACKPOS);
		nPos=scrollinfo.nTrackPos;
		break;
	case SB_LINEDOWN:	nPos++;		break;
	case SB_LINEUP:		nPos--;		break;
	case SB_PAGEDOWN:	nPos += 4;	break;
	case SB_PAGEUP:		nPos -= 4;	break;
	case SB_ENDSCROLL:
		m_pSchematic->GetViewPos( &nLn, NULL );
		UpdateView( nLn, nPos );
		break;
	}
	SetScrollPos( SB_HORZ,nPos );
	//CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


BOOL CLADView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nLn,nCol;
	m_pSchematic->GetViewPos( &nLn, &nCol );
	nLn -= zDelta / WHEEL_DELTA;
	UpdateView( nLn, nCol );
	SetScrollPos( SB_VERT,nLn );
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CLADView::OnEditUndo()
{

}
void CLADView::OnEditRedo()
{

}
void CLADView::OnUpdateEditToolBar(CCmdUI *pCmdUI)
{
	if( pCmdUI->m_nID == ID_EDIT_REDO )
		pCmdUI->Enable(TRUE);
	else if( pCmdUI->m_nID == ID_EDIT_UNDO )
		pCmdUI->Enable(TRUE);
}
void CLADView::OnUpdateConvertToolBar(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
void CLADView::OnUpdateLadInsToolBar(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nInsCmdID==pCmdUI->m_nID);
}

void CLADView::OnInsArrow()
{
	m_nInsCmdID = ID_INS_ARROW;
}
void CLADView::OnInsClose()
{
	m_nInsCmdID = ID_INS_CLOSE;
}
void CLADView::OnInsORClose()
{
	m_nInsCmdID = ID_INS_ORCLOSE;
}
void CLADView::OnInsOpen()
{
	m_nInsCmdID = ID_INS_OPEN;
}
void CLADView::OnInsOROpen()
{
	m_nInsCmdID = ID_INS_OROPEN;
}
void CLADView::OnInsOut()
{
	m_nInsCmdID = ID_INS_OUT;
}
void CLADView::OnInsFun()
{
	m_nInsCmdID = ID_INS_FUN;
}
void CLADView::OnInsLine()
{
	m_nInsCmdID = ID_INS_LINE;
}
void CLADView::OnInsDel()
{
	m_nInsCmdID = ID_INS_DEL;
}
void CLADView::OnLineDel()
{
	m_nInsCmdID = ID_INS_LINEDEL;
}

void CLADView::OnSetting()
{

}
void CLADView::OnComvert()
{

}
void CLADView::OnCmpile()
{
	//编译程序
	char buff[512];
	DWORD bytesRead;
	CProject& proj = theApp.GetProject();
	CStdString str;
	str.Format( _T("plcc.exe -o \"%splcprg.txt\" \"%smain.lad\""),proj.m_strPath, proj.m_strPath );

	HANDLE hOutputRead = INVALID_HANDLE_VALUE;
	HANDLE hOutputWrite = INVALID_HANDLE_VALUE;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE; 
	if ( !CreatePipe(&hOutputRead,&hOutputWrite,&sa,0))
	{
		MessageBox( _T("Error On CreatePipe1"),_T("编译错误！"),MB_OK );
		return;
	}

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	//GetStartupInfo(&si);
	si.hStdError = hOutputWrite;
	si.hStdOutput = hOutputWrite;
	//si.hStdInput = hOutputWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	if (
		!CreateProcessA( NULL,   // No module name (use command line)
		(LPSTR)str.ToAnsi(),        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		TRUE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi )           // Pointer to PROCESS_INFORMATION structure
		)
	{
		MessageBox( _T("Error On CreateProcess"),_T("编译错误！"),MB_OK );
		return;
	}
	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	if( PeekNamedPipe( hOutputRead, buff, 512, &bytesRead,NULL,NULL ) )
	{
		if( bytesRead )
		{
			buff[ bytesRead ] = 0;
			str.Assign( buff,CP_ACP );
			MessageBox( str,_T("编译错误！"),MB_OK );
		}
	}else{
		MessageBox( _T("编译错误！"),_T("编译错误！"),MB_OK );
	}
}
void CLADView::OnSimulation()
{
	//先下载程序
	BOOL bRes;
	CProject& proj = theApp.GetProject();
	TCHAR strPath[ MAX_PATH ];
	wprintf_s( strPath, _T("%splcprg.txt"), proj.m_strPath );
	FILE* fp = _wfopen( strPath, _T("rb") );
	if( fp == NULL )
		return;
	fseek( fp,0,SEEK_END );
	size_t len = ftell( fp );
	unsigned char* pProgram = new unsigned char[ len ];
	if( pProgram == NULL ) return;
	fseek( fp, 0, SEEK_SET );
	fread( pProgram, sizeof( uint16 ), len, fp );
	fclose( fp );

	bRes = proj.m_Communication.Download( pProgram, len );

	delete[] pProgram;
}

void CLADView::OnNewLadLine()
{
	m_pSchematic->InsertLine( m_psFocuse.nLn );
	UpdateView();
}


void CLADView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	if( nChar >= 'A' && nChar <= 'Z')
	{
		CPoint point;
		TCHAR str[2] = {0,0};
		str[0] = nChar;
		int nLn,nCol;
		m_pSchematic->GetViewPos( &nLn, &nCol );
		CDlgLadInput dlg( m_pSchematic,m_psFocuse, libSch::eNULL, str );
		point.x = ( m_psFocuse.nCol - nCol ) * m_pSchematic->GetColLen();
		point.y = ( m_psFocuse.nLn - nLn )* m_pSchematic->GetColLen() * 2;
		ClientToScreen(&point);
		dlg.MoveDoModalWindow( point );
		if( dlg.DoModal() == IDOK )
		{
			m_pSchematic->OnInsInputChange( m_psFocuse, dlg.m_eType,dlg.m_strArg );
			UpdateView();
		}
	}else if( nChar >= VK_LEFT && nChar <= VK_DOWN )
	{
		int nLn,nCol;
		int nMaxLn = m_pSchematic->GetTotalRow();
		m_pSchematic->GetViewPos( &nLn, &nCol );
		switch( nChar )
		{
		case VK_DOWN:
			if( m_psFocuse.nLn > nMaxLn - 1 )
				return;
			++m_psFocuse.nLn;
			if( m_psFocuse.nLn > nLn + m_pSchematic->GetViewRowCount() - 1 )
				++nLn;
			break;
		case VK_UP:
			if( m_psFocuse.nLn == 0)
				return;
			--m_psFocuse.nLn;
			if( m_psFocuse.nLn < nLn )
				--nLn;
			break;
		case VK_LEFT:
			if( m_psFocuse.nCol <= 0 )
				return;
			--m_psFocuse.nCol;
			if( m_psFocuse.nCol <  nCol )
				--nCol;
			break;
		case VK_RIGHT:
			if( m_psFocuse.nCol >= m_pSchematic->GetTotalCol() )
				return;
			++m_psFocuse.nCol;
			if( m_psFocuse.nCol > nCol + m_pSchematic->GetViewColCount() - 1 )
				++nCol;
			break;
		}
		SetScrollPos( SB_VERT,nLn );
		UpdateView( nLn, nCol );
	}else
		CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CLADView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}
