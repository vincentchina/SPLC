#pragma once
#include "resource.h"
// CDlgLadInput dialog

class CDlgLadInput : public CDialog
{
	DECLARE_DYNAMIC(CDlgLadInput)

public:
	BOOL m_bInsReadOnly;
	libSch::eCoilInsType m_eType;
	libSch::CSchematic* m_pSchematic;
	libSch::CoilPos m_pos;
	CStdString m_strArg;
	CPoint m_ptPos;
	CDlgLadInput( libSch::CSchematic* pSchematic, libSch::CoilPos& pos,libSch::eCoilInsType eType, LPTSTR strArg, BOOL bInsReadOnly = FALSE,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLadInput();

// Dialog Data
	enum { IDD = IDD_LADINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void MoveDoModalWindow( CPoint point );
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedHelp1();
	afx_msg void OnEnSetfocusInput();
};
