// DlgLadInput.cpp : implementation file
//

#include "stdafx.h"
#include "PlcEditor.h"
#include "DlgLadInput.h"
#include "afxdialogex.h"


// CDlgLadInput dialog

IMPLEMENT_DYNAMIC(CDlgLadInput, CDialog)

CDlgLadInput::CDlgLadInput( libSch::CSchematic* pSchematic, libSch::CoilPos& pos, libSch::eCoilInsType eType, LPTSTR strArg, BOOL bInsReadOnly, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLadInput::IDD, pParent),
	m_eType( eType ),
	m_strArg( strArg ),
	m_bInsReadOnly( bInsReadOnly ),
	m_pSchematic( pSchematic ),
	m_pos( pos )
{
	m_ptPos.x = -1;
	m_ptPos.y = -1;
}

CDlgLadInput::~CDlgLadInput()
{
}

void CDlgLadInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLadInput, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgLadInput::OnBnClickedOk)
	ON_BN_CLICKED(IDC_HELP1, &CDlgLadInput::OnBnClickedHelp1)
	ON_EN_SETFOCUS(IDC_INPUT, &CDlgLadInput::OnEnSetfocusInput)
END_MESSAGE_MAP()


// CDlgLadInput message handlers

void CDlgLadInput::MoveDoModalWindow( CPoint point )
{
	m_ptPos = point;
}
void NormalWindwoRect( RECT& rc )
{
	CPoint ptMove;
	if( rc.left < 0 || rc.top < 0 )
	{
		if( rc.left < 0 )
			ptMove.x = -rc.left;
		if( rc.top < 0 )
			ptMove.y = -rc.top;
	}else if( rc.right > GetSystemMetrics(SM_CXSCREEN) || rc.bottom > GetSystemMetrics(SM_CYSCREEN) )
	{
		if( rc.right > GetSystemMetrics(SM_CXSCREEN) )
			ptMove.x = GetSystemMetrics(SM_CXSCREEN) - rc.right;
		if( rc.bottom > GetSystemMetrics(SM_CYSCREEN) )
			ptMove.y = GetSystemMetrics(SM_CYSCREEN) - rc.bottom;
	}
	OffsetRect(&rc,ptMove.x,ptMove.y);
}
BOOL CDlgLadInput::OnInitDialog()
{
	CDialog::OnInitDialog();
	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_COMBO1 );
	pCombo->SetCurSel( m_eType );
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_INPUT );
	pEdit->SetLimitText(20);
	pEdit->SetWindowText( m_strArg );
	pCombo->EnableWindow(!m_bInsReadOnly);
	if( m_ptPos.x != -1 || m_ptPos.y != -1 )
	{
		RECT rc;
		CPoint ptMove;
		GetWindowRect( &rc );
		ptMove.x = m_ptPos.x - ( rc.left + ( rc.right - rc.left ) / 2 );
		ptMove.y = m_ptPos.y - ( rc.top + ( rc.bottom - rc.top ) / 2 );
		OffsetRect(&rc,ptMove.x,ptMove.y);
		NormalWindwoRect( rc );
		MoveWindow( &rc );
	}
	//pEdit->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgLadInput::OnBnClickedOk()
{
	if( m_pSchematic == NULL )
		CDialog::OnCancel();
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_INPUT );
	pEdit->GetWindowText( (wchar_t*)m_strArg.GetBuffer( pEdit->GetWindowTextLength() ),pEdit->GetWindowTextLength() + 1 );
	m_strArg.MakeUpper();
	CComboBox* pCombo = (CComboBox*)GetDlgItem( IDC_COMBO1 );
	m_eType = (libSch::eCoilInsType)pCombo->GetCurSel();
	if( m_pSchematic->TestInsInput(m_pos,m_eType,m_strArg ) )
		CDialog::OnOK();
	else
	{
		MessageBox(_T("ÊäÈëÖ¸Áî´íÎó£¡"),_T("´íÎó"),MB_OK );
		return;
	}
}


void CDlgLadInput::OnBnClickedHelp1()
{
	// TODO: Add your control notification handler code here
}


void CDlgLadInput::OnEnSetfocusInput()
{
	CEdit* pEdit = (CEdit*)GetDlgItem( IDC_INPUT );
	pEdit->SetSel(-1,-1);
}
