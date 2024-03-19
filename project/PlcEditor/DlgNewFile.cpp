// DlgNewFile.cpp : implementation file
//

#include "stdafx.h"
#include "plcEditor.h"
#include "DlgNewFile.h"
#include "afxdialogex.h"


// CDlgNewFile dialog

IMPLEMENT_DYNAMIC(CDlgNewFile, CDialog)

	CDlgNewFile::CDlgNewFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewFile::IDD, pParent)
	, m_strProjectName(_T(""))
	, m_strProjectPath(_T(""))
	, m_nStep(0)
	, m_nCol(0)
{

}

CDlgNewFile::~CDlgNewFile()
{
}

void CDlgNewFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROJECTNAME, m_strProjectName);
	DDX_Text(pDX, IDC_PROJECTPATH, m_strProjectPath);
	DDX_Text(pDX, IDC_MAX_STEP, m_nStep);
	DDV_MinMaxInt(pDX, m_nStep, 0, 30000);
	DDX_Text(pDX, IDC_MAX_COL, m_nCol);
	DDV_MinMaxInt(pDX, m_nCol, 0, 100);
}


BEGIN_MESSAGE_MAP(CDlgNewFile, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgNewFile::OnBnClickedOk)
	ON_BN_CLICKED(IDC_VIEWPATH, &CDlgNewFile::OnBnClickedViewpath)
END_MESSAGE_MAP()


// CDlgNewFile message handlers


BOOL CDlgNewFile::OnInitDialog()
{
	CString wndID;
	CDialog::OnInitDialog();

	( (CButton*)GetDlgItem(IDC_MKDIR) )->SetCheck( TRUE );
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgNewFile::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if( m_strProjectName.IsEmpty() || m_strProjectPath.IsEmpty() )
	{
		MessageBox( _T("请输入正确的参数！"),_T("错误"),MB_OK );
		return;
	}
	m_bMKDir = ( (CButton*)GetDlgItem(IDC_MKDIR) )->GetCheck();
	CDialog::OnOK();
}


void CDlgNewFile::OnBnClickedViewpath()
{
	BROWSEINFO bi;
	TCHAR Buffer[MAX_PATH];
	bi.hwndOwner = this->GetSafeHwnd();
	bi.pidlRoot =NULL;//初始化制定的root目录很不容易，
	bi.pszDisplayName = Buffer;//此参数如为NULL则不能显示对话框
	bi.lpszTitle = _T("选择工程目录");
	//bi.ulFlags = BIF_BROWSEINCLUDEFILES;//包括文件
	bi.ulFlags = BIF_EDITBOX;//包括文件

	bi.lpfn = NULL;
	//bi.iImage=IDR_MAINFRAME;
	//初始化入口参数bi结束
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);//调用显示选择对话框
	if(pIDList)
	{
		SHGetPathFromIDList(pIDList, Buffer);
		m_strProjectPath = Buffer;
		UpdateData(FALSE);
	}
}
