#pragma once
#include "common\MemDc.h"
#define DATAFLAG_D	1
#define DATAFLAG_T	2
#define DATAFLAG_C	4

// CLadView dialog

class CLadView : public CDialog
{
	DECLARE_DYNAMIC(CLadView)

public:
	typedef union tagFlagUpdate{
		UINT32 all;
		struct{
			UINT32 bUpdate:	1;
			UINT32 bModify:	1;
			UINT32 bFound:	1;
			UINT32 dataFlag:5;
			UINT32 coil:	8;
			UINT32 addr:	16;
		};
	}FlagUpdate,*LPFlagUpdate;

	typedef struct tagCoilFind{
		libSch::CoilPos pos;
		CBaseCoil* ctrl;
	}CoilFind,*LPCoilFind;

public:
	CLadView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLadView();

	// Dialog Data
	enum { IDD = IDD_LADVIEW,
			SZ_UPDATELIST = 200 };
protected:
	CBaseCoil* m_lstUpdate[ SZ_UPDATELIST ];
	int m_nlstUpdate;
	libSch::CoilPos m_lstFound[ SZ_UPDATELIST ];
	int m_nlstFound;
	CMemDc m_memDc;
	TrackDc m_trackDc;
	libSch::CoilPos m_psFocuse;
	RECT m_rcFocuse;			//保存focuse的线圈的框
	libSch::CSchematic m_schematic;
public:
	CStdString m_strFind;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	BOOL LoadPLC(CString strProgram );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void UpdataScroll(void);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void UpdateView(int nLen, int nCol);
	static void TrackDraw(HDC hdc, RECT rt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void OnPLCScanEnd(void);
	void UpdataCoilList(void);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void GetErr(CStdString& strErr);
	BOOL FindCoil(CString& strFind );
	void UpdateFindList(void);
};
