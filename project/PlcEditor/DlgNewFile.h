#pragma once


// CDlgNewFile dialog

class CDlgNewFile : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewFile)

public:
	CDlgNewFile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewFile();
	// Dialog Data
	enum { IDD = IDD_NEWFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CString m_strProjectName;
	CString m_strProjectPath;
	BOOL m_bMKDir;
	afx_msg void OnBnClickedViewpath();
	int m_nStep;
	int m_nCol;
};
