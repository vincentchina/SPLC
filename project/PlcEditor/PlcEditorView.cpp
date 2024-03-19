
// PlcEditorView.cpp : CPlcEditorView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "PlcEditor.h"
#endif

//#include "PlcEditorDoc.h"
#include "PlcEditorView.h"
#include "DlgLadInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPlcEditorView

IMPLEMENT_DYNCREATE(CPlcEditorView, CView)

BEGIN_MESSAGE_MAP(CPlcEditorView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPlcEditorView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CPlcEditorView 构造/析构

CPlcEditorView::CPlcEditorView():
	m_nFocuseRow(0),
	m_nFocuseCol(0)
{
	// TODO: 在此处添加构造代码

}

CPlcEditorView::~CPlcEditorView()
{
}

BOOL CPlcEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	//SetScrollSizes(MM_TEXT,CSize(800,600));
	return CView::PreCreateWindow(cs);
}

// CPlcEditorView 绘制

void CPlcEditorView::OnDraw(CDC* pDC)
{
	CPlcEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	/*CRect rectClient;
	GetClientRect(rectClient);
	CPoint point;
	OnPrepareDC(pDC);
	pDC->LPtoDP(&point);*/
	//m_schematic.Draw( m_memDc.GetDC() );
	m_schematic.DrawHelp( pDC->GetSafeHdc(),m_memDc.GetDC() );
	m_trackDc.Draw( pDC->GetSafeHdc() );
	//pDC->TextOut(0,0,"hello world");
	//pDC->Rectangle(rectClient);
}


// CPlcEditorView 打印


void CPlcEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CPlcEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CPlcEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CPlcEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CPlcEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPlcEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CPlcEditorView 诊断

#ifdef _DEBUG
void CPlcEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CPlcEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPlcEditorDoc* CPlcEditorView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlcEditorDoc)));
	return (CPlcEditorDoc*)m_pDocument;
}
#endif //_DEBUG

// CPlcEditorView 消息处理程序


void CPlcEditorView::OnSize(UINT nType, int cx, int cy)
{
	CClientDC dc(this);
	//OnPrepareDC(&dc);
	CView::OnSize(nType, cx, cy);
	m_schematic.SetViewSize( cx,cy );
	SIZE sz = m_schematic.GetViewSize();
	m_memDc.SetSize(sz.cx,sz.cy);
	m_schematic.GetPosRect( &m_rcFocuse,m_nFocuseRow, m_nFocuseCol );
	m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
	m_schematic.Draw( m_memDc.GetDC() );
}


void CPlcEditorView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
}


void CPlcEditorView::OnInitialUpdate()
{
	SIZE sz;
	CView::OnInitialUpdate();
	sz = m_schematic.GetSize();
	m_memDc.SetSize(sz.cx,sz.cy);
	//m_schematic.Draw( m_memDc.GetDC() );
	m_trackDc.SetDrawFact( DrawFact );
	UpdataScroll();
}

void CPlcEditorView::DrawFact( HDC hdc, RECT rt )
{
	HPEN hPen, hPenOld;
	hPen = CreatePen( PS_SOLID, 2, RGB(0,24,255) );
	hPenOld = (HPEN)SelectObject(hdc, hPen);
	Rectangle(hdc,rt.left,rt.top,rt.right,rt.bottom );
	SelectObject(hdc, hPenOld);
	DeleteObject(hPen);
}
void CPlcEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint ptDPtoLP(0,0);
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(&ptDPtoLP);
	CPoint ptLogical=point-ptDPtoLP;
	m_nFocuseCol = ptLogical.x / m_schematic.m_nColLogicLen;
	m_nFocuseRow = ptLogical.y / m_schematic.m_nRowLogicLen;
	if( m_schematic.GetPosRect( &m_rcFocuse,m_nFocuseRow, m_nFocuseCol ) )
		m_trackDc.TrackDraw( dc.GetSafeHdc(),m_rcFocuse );
}

void CPlcEditorView::UpdataScroll()
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask  = SIF_RANGE | SIF_PAGE;
	si.nMin   = 0;
	si.nMax   = m_schematic.GetTotalRow();
	si.nPage  = 1;
	SetScrollInfo( SB_VERT, &si, TRUE);
	si.nMax   = 2;
	SetScrollInfo( SB_HORZ, &si, TRUE);
	//SetScrollPos( SB_VERT,20 );
	//m_schematic.SetViewPos( 1, 1 );
}
void CPlcEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
	case SB_LINEDOWN:
		nPos++;
		break;
	case SB_LINEUP:
		nPos--;
		break;
	case SB_PAGEDOWN:
		nPos += 4;
		break;
	case SB_PAGEUP:
		nPos -= 4;
		break;
	case SB_ENDSCROLL:
		m_schematic.GetViewPos( NULL, &nCol );
		if( m_schematic.SetViewPos( nPos, nCol ) )
		{
			m_schematic.Draw( m_memDc.GetDC() );
			InvalidateRect(NULL,FALSE);
		}
		break;
	}
	SetScrollPos( SB_VERT,nPos );
	//CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPlcEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int nRow;
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
	case SB_LINEDOWN:
		nPos++;
		break;
	case SB_LINEUP:
		nPos--;
		break;
	case SB_PAGEDOWN:
		nPos += 4;
		break;
	case SB_PAGEUP:
		nPos -= 4;
		break;
	case SB_ENDSCROLL:
		m_schematic.GetViewPos( &nRow, NULL );
		if( m_schematic.SetViewPos( nRow, nPos ) )
		{
			m_schematic.Draw( m_memDc.GetDC() );
			InvalidateRect(NULL,FALSE);
		}
		break;
	}
	SetScrollPos( SB_HORZ,nPos );
	//CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


BOOL CPlcEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nRow,nCol;
	m_schematic.GetViewPos( &nRow, &nCol );
	nRow -= zDelta / WHEEL_DELTA;
	if( m_schematic.SetViewPos( nRow, nCol ) )
	{
		SetScrollPos( SB_VERT,nRow );
		m_schematic.Draw( m_memDc.GetDC() );
		InvalidateRect(NULL,FALSE);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CPlcEditorView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CDlgLadInput dlg;
	dlg.DoModal();
	CView::OnLButtonDblClk(nFlags, point);
}
