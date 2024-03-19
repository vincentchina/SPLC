
// LADView.h : CLADView 类的接口
//

#pragma once
#include "LADDoc.h"
#include "DlgLadInput.h"
#include "common/MemDc.h"

class CLADView : public CView
{
protected: // 仅从序列化创建
	CLADView();
	DECLARE_DYNCREATE(CLADView)

// 特性
public:
	CLADDoc* GetDocument() const;
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
	virtual ~CLADView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	long m_nInsCmdID;
	TrackDc m_trackDc;
	CMemDc m_memDc;
	libSch::CSchematic* m_pSchematic;
	libSch::CoilPos m_psFocuse;
	//不保存CBaseCoil* m_pCoilFocuse;	//focuse 的线圈
	RECT m_rcFocuse;			//保存focuse的线圈的框
	static void DrawFact( HDC hdc, RECT rt );
	void UpdateView( int nRow, int nCol );
	void UpdateView();
	
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditToolBar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateConvertToolBar(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLadInsToolBar(CCmdUI *pCmdUI);
	afx_msg void OnInsArrow();
	afx_msg void OnInsClose();
	afx_msg void OnInsORClose();
	afx_msg void OnInsOpen();
	afx_msg void OnInsOROpen();
	afx_msg void OnInsOut();
	afx_msg void OnInsFun();
	afx_msg void OnInsLine();
	afx_msg void OnInsDel();
	afx_msg void OnLineDel();

	afx_msg void OnSetting();
	afx_msg void OnComvert();
	afx_msg void OnCmpile();
	afx_msg void OnSimulation();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnNewLadLine();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // LADView.cpp 中的调试版本
inline CLADDoc* CLADView::GetDocument() const
   { return reinterpret_cast<CLADDoc*>(m_pDocument); }
#endif

