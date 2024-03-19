#pragma once


// CDlgProperties dialog

class CDlgProperties : public CDialog
{
	DECLARE_DYNAMIC(CDlgProperties)

public:
	CDlgProperties( CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProperties();

// Dialog Data
	enum { IDD = IDD_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckAutoport();
	afx_msg void OnBnClickedButtonTestcom();
};
