// DlgProperties.cpp : implementation file
//

#include "stdafx.h"
#include "PlcEditor.h"
#include "DlgProperties.h"
#include "afxdialogex.h"
#include "PlcEditor.h"

// CDlgProperties dialog

IMPLEMENT_DYNAMIC(CDlgProperties, CDialog)

CDlgProperties::CDlgProperties( CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProperties::IDD, pParent)
{

}

CDlgProperties::~CDlgProperties()
{
}

void CDlgProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgProperties, CDialog)
	ON_BN_CLICKED(IDC_CHECK_AUTOPORT, &CDlgProperties::OnBnClickedCheckAutoport)
	ON_BN_CLICKED(IDC_BUTTON_TESTCOM, &CDlgProperties::OnBnClickedButtonTestcom)
END_MESSAGE_MAP()


// CDlgProperties message handlers


BOOL CDlgProperties::OnInitDialog()
{
	CString strNum;
	CDialog::OnInitDialog();
	CProject& proj = theApp.GetProject();
	GetDlgItem( IDC_PROJECTNAME )->SetWindowText( proj.m_strName );
	GetDlgItem( IDC_PROJECTPATH )->SetWindowText( proj.m_strFullPath );
	strNum.Format( _T( "%d" ), proj.m_nMaxLn );
	GetDlgItem( IDC_MAX_STEP2 )->SetWindowText( strNum );
	strNum.Format( _T( "%d" ), proj.m_nMaxCol );
	GetDlgItem( IDC_MAX_COL2 )->SetWindowText( strNum );
	
	CComboBox* pCombo = static_cast<CComboBox*>( GetDlgItem( IDC_COMBO_DEVICE ) );
	pCombo->SetCurSel( proj.m_uDevice );
	pCombo = static_cast<CComboBox*>( GetDlgItem( IDC_COMBO_COMTYPE ) );
	pCombo->SetCurSel( proj.m_uComType );

	GetDlgItem( IDC_EDIT_PORT )->SetWindowText( proj.m_strPort );
	GetDlgItem( IDC_EDIT_PORT )->EnableWindow( !proj.m_bAutoPort );
	static_cast<CButton*>( GetDlgItem( IDC_CHECK_AUTOPORT ) )->SetCheck( proj.m_bAutoPort );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgProperties::OnBnClickedCheckAutoport()
{
	BOOL bCheck = static_cast<CButton*>( GetDlgItem( IDC_CHECK_AUTOPORT ) )->GetCheck();
	GetDlgItem( IDC_EDIT_PORT )->EnableWindow( !bCheck );
}


void CDlgProperties::OnBnClickedButtonTestcom()
{
	CProject& proj = theApp.GetProject();
	GetDlgItem( IDC_EDIT_PORT )->GetWindowText( proj.m_strPort );
	proj.m_uComType = static_cast<CComboBox*>( GetDlgItem( IDC_COMBO_COMTYPE ) )->GetCurSel();
	proj.m_bAutoPort = static_cast<CButton*>( GetDlgItem( IDC_CHECK_AUTOPORT ) )->GetCheck();
	MessageBox( proj.TestCommunicate() ? _T("通信成功") : _T("通信失败"), _T("提示"), MB_OK );
}
