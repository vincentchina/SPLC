
// PlcEditorView.h : CPlcEditorView 类的接口
//

#pragma once
#include "MemDc.h"
#include "PlcEditorDoc.h"
#include "Schematic.h"

class CPlcEditorView : public CView
{
protected: // 仅从序列化创建
	CPlcEditorView();
	DECLARE_DYNCREATE(CPlcEditorView)

// 特性
public:
	CPlcEditorDoc* GetDocument() const;
// 操作
public:
	void UpdataScroll();
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CPlcEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	TrackDc m_trackDc;
	CMemDc m_memDc;
	CSchematic m_schematic;
	RECT m_rcFocuse;
	int m_nFocuseRow,m_nFocuseCol;
	static void DrawFact( HDC hdc, RECT rt );
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // PlcEditorView.cpp 中的调试版本
inline CPlcEditorDoc* CPlcEditorView::GetDocument() const
   { return reinterpret_cast<CPlcEditorDoc*>(m_pDocument); }
#endif

