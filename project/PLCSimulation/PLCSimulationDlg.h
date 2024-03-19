
// PLCSimulationDlg.h : 头文件
//

#pragma once
#include "LadView.h"
#include "DlgWatch.h"
#include "Communication.h"
#include <MMSystem.h>
#include "VirtualMachine.h"

#define COLOR_PLCRUN		RGB( 0,0,255 )
#define COLOR_PLCSTOP		RGB( 180,180,180 )
#define COLOR_PLCSTEPRUN	RGB( 10,236,115 )
#define COLOR_PLCERR		RGB( 255,0,0 )
#define COLOR_PLCNOERR		RGB( 236,233,216 )

#define UMSG_PLCSTOP	( WM_USER + 1 )
#define UMSG_PLCSCANEND	( WM_USER + 2 )
#define UMSG_PLCOUTY	( WM_USER + 3 )

#pragma comment(lib, "Winmm.lib") 

#define SZ_PROGRAM_MEM	( 1024 * 60 )
typedef struct tagProgramData{
	unsigned char pFlash[SZ_PROGRAM_MEM];
	int szFlash;
	unsigned char pRam[SZ_PROGRAM_MEM];
	int szRam;
	CBaseDev* pDev;
	volatile int bRun;
	MMRESULT t;
	HANDLE hThreadScan;
	CStdString strFile;
	BOOL bWPG;
	unsigned long uScanInterval;
}ProgramData,*LPProgramData;

// CPLCSimulationDlg 对话框
class CPLCSimulationDlg : public CDialogEx
{
	// 构造
	friend class CDlgWatch;
	friend class CLadView;
	friend int MachineAction( int aix, int speed, int location, int bAbsolute );
public:

	enum{
		STATUSE_SEPARATOR,
		STATUSE_PLCERR,	
		STATUSE_LN,
		STATUSE_LN_NUM,
		STATUSE_COL,
		STATUSE_COL_NUM,
		STATUSE_STEP,
		STATUSE_STEP_NUM,
	};
public:
	CPLCSimulationDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_PLCSIMULATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
public:
	static CCommunication m_communication;
protected:
	CVirtualMachine m_cMachine;
	CDlgWatch m_dlgWatch;
	CStatusBar m_wndStatusBar;
	CStdString m_strWorkPath;
	static CDevUart m_uart;
	static CDevPip m_pip;
	static CDevSocket m_socket;
	static ProgramData m_prgData;
	RECT m_rcColorRun;
	RECT m_rcColorErr;
	HICON m_hIcon;
	CLadView m_ladView;
	int		m_nLadViewTop;
	int m_nLadViewBottom;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static ProgramData& GetPrgData() { return m_prgData; }
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnloadprogram();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedBtnreloadprogram();
	virtual void OnOK();
	BOOL LoadProgram(void);
	inline void SetStatusString( CString str );

protected:
	afx_msg LRESULT OnPlcStop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlcScanend(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBtnstep();
	afx_msg void OnBnClickedBtnrun();
	afx_msg void OnBnClickedBtnresetplc();
	void UpdatePLCState(void);
	afx_msg void OnBnClickedBtnwatch();
	afx_msg void OnBnClickedBtnloadmem();
	afx_msg void OnBnClickedBtnsavemem();
	afx_msg void OnBnClickedBtnfind();
	afx_msg void OnBnClickedBtndownload();
protected:
	BOOL OpenMemFile(FILE** fp, BOOL bOpen);
public:
	afx_msg void OnBnClickedBtnfindclear();
	afx_msg void OnBnClickedBtnopencom();
};
