#pragma once


#define WATCHTYPE_16D	0
#define WATCHTYPE_16H	1
#define WATCHTYPE_32D	2
#define WATCHTYPE_32H	3
#define WATCHTYPE_1B	4

typedef union tagWatchData{
	UINT32 all;
	struct{
		UINT32 coil:	8;
		UINT32 addr:	16;
		UINT32 type:	4;
		UINT32 resver:	4;
	};
}WatchData,*LPWatchData;

// CDlgWatch dialog

class CDlgWatch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWatch)

public:
	CDlgWatch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWatch();

// Dialog Data
	enum { IDD = IDD_WATCH };

protected:
	WatchData m_bitWatch;
	WatchData m_dataWatch;
	RECT m_rcColorBit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnforceon();
	afx_msg void OnBnClickedForceoff();
	afx_msg void OnBnClickedAntiset();
	afx_msg void OnBnClickedBtnaddbitwatch();
	afx_msg void OnBnClickedBtnadddatawatch();
	afx_msg void OnBnClickedBtnsetvalue();
	bool GetBitCoil(unsigned int* coil, unsigned int* addr);
	void OnPLCScanEnd(void);
	int AddWatch(WatchData wdata);
	afx_msg void OnPaint();
	bool GetDataCoil(unsigned int* coil , unsigned int* addr);
	CString GetDataValue(WatchData wData);
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnclear();
};
