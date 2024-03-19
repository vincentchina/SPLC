// DlgWatch.cpp : implementation file
//

#include "stdafx.h"
#include "PLCSimulation.h"
#include "DlgWatch.h"
#include "afxdialogex.h"

extern void ScreenRectToClient( HWND hwnd, RECT* pRect );
// CDlgWatch dialog

IMPLEMENT_DYNAMIC(CDlgWatch, CDialogEx)

CDlgWatch::CDlgWatch(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWatch::IDD, pParent)
{
	m_dataWatch.all = 0;
	m_bitWatch.all = 0;
}

CDlgWatch::~CDlgWatch()
{
}

void CDlgWatch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWatch, CDialogEx)
	ON_BN_CLICKED(IDC_BTNFORCEON, &CDlgWatch::OnBnClickedBtnforceon)
	ON_BN_CLICKED(IDC_FORCEOFF, &CDlgWatch::OnBnClickedForceoff)
	ON_BN_CLICKED(IDC_ANTISET, &CDlgWatch::OnBnClickedAntiset)
	ON_BN_CLICKED(IDC_BTNADDBITWATCH, &CDlgWatch::OnBnClickedBtnaddbitwatch)
	ON_BN_CLICKED(IDC_BTNADDDATAWATCH, &CDlgWatch::OnBnClickedBtnadddatawatch)
	ON_BN_CLICKED(IDC_BTNSETVALUE, &CDlgWatch::OnBnClickedBtnsetvalue)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CDlgWatch::OnCbnSelchangeComboType)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CDlgWatch::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTNCLEAR, &CDlgWatch::OnBnClickedBtnclear)
END_MESSAGE_MAP()


// CDlgWatch message handlers


void CDlgWatch::OnOK()
{
}


BOOL CDlgWatch::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CListCtrl* pCtrlList = static_cast<CListCtrl*>( GetDlgItem(IDC_LIST) );
	pCtrlList->InsertColumn( 0, L"元件名", LVCFMT_LEFT, 100 );//插入列
	pCtrlList->InsertColumn( 1, L"地址", LVCFMT_LEFT, 100 );//插入列
	pCtrlList->InsertColumn( 2, L"数值", LVCFMT_LEFT, 150 );
	pCtrlList->InsertColumn( 3, L"类型", LVCFMT_LEFT, 100 );
	GetDlgItem(IDC_COLORBIT)->GetWindowRect( &m_rcColorBit );
	ScreenRectToClient( this->GetSafeHwnd(), &m_rcColorBit );

	CComboBox* pCombo = static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
	pCombo->AddString( _T("16位十进制"));
	pCombo->AddString( _T("16位十六进制"));
	pCombo->AddString( _T("32位十进制"));
	pCombo->AddString( _T("32位十六进制"));
	pCombo->SetCurSel(0);

	pCombo = static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_VALUE) );
	pCombo->AddString( _T("十进制"));
	pCombo->AddString( _T("十六进制"));
	pCombo->SetCurSel(0);

	static_cast<CEdit*>( GetDlgItem(IDC_EDIT_DATA))->SetLimitText(5);
	static_cast<CEdit*>( GetDlgItem(IDC_EDIT_BIT))->SetLimitText(5);
	static_cast<CEdit*>( GetDlgItem(IDC_EDIT_VALUE))->SetLimitText(11);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlgWatch::GetBitCoil( unsigned int* coil, unsigned int* addr )
{
	CStdString str;
	int nLen = ::GetWindowTextLength( GetDlgItem(IDC_EDIT_BIT)->m_hWnd );
	::GetWindowText( GetDlgItem(IDC_EDIT_BIT)->m_hWnd,(wchar_t*)str.GetBuffer( nLen + 1), nLen + 1 );
	if( !libSch::CSchematic::TestCoilType( ARG_COIL_X | ARG_COIL_Y | ARG_COIL_S | ARG_COIL_M | ARG_COIL_T | ARG_COIL_C,
		str,coil, addr ) )
	{
		MessageBox( _T("元件格式错误"),_T("错误"), MB_OK );
		return false;
	}
	return true;
}


bool CDlgWatch::GetDataCoil(unsigned int* coil , unsigned int* addr)
{
	CStdString str;
	int nLen = ::GetWindowTextLength( GetDlgItem(IDC_EDIT_DATA)->m_hWnd );
	::GetWindowText( GetDlgItem(IDC_EDIT_DATA)->m_hWnd,(wchar_t*)str.GetBuffer( nLen + 1), nLen + 1 );
	if( !libSch::CSchematic::TestCoilType( ARG_COIL_T | ARG_COIL_C | ARG_COIL_D | ARG_COIL_V ,
		str,coil, addr ) )
	{
		MessageBox( _T("元件格式错误"),_T("错误"), MB_OK );
		return false;
	}
	return true;
}

void CDlgWatch::OnBnClickedBtnforceon()
{
	unsigned int coil;
	unsigned int addr;
	m_bitWatch.all = 0;
	if( GetBitCoil( &coil, &addr ) )
	{
		m_bitWatch.type = WATCHTYPE_1B;
		m_bitWatch.addr = addr;
		m_bitWatch.coil = coil;
		lib_API_WriteBit( coil, addr, 1 );
	}
}


void CDlgWatch::OnBnClickedForceoff()
{
	unsigned int coil;
	unsigned int addr;
	m_bitWatch.all = 0;
	if( GetBitCoil( &coil, &addr ) )
	{
		m_bitWatch.type = WATCHTYPE_1B;
		m_bitWatch.addr = addr;
		m_bitWatch.coil = coil;
		lib_API_WriteBit( coil, addr, 0 );
	}
}


void CDlgWatch::OnBnClickedAntiset()
{
	unsigned int coil;
	unsigned int addr;
	m_bitWatch.all = 0;
	if( GetBitCoil( &coil, &addr ) )
	{
		m_bitWatch.type = WATCHTYPE_1B;
		m_bitWatch.addr = addr;
		m_bitWatch.coil = coil;
		lib_API_WriteBit( coil, addr, !lib_API_ReadBit( coil, addr ) );
	}
}


void CDlgWatch::OnBnClickedBtnaddbitwatch()
{
	unsigned int coil;
	unsigned int addr;
	m_bitWatch.all = 0;
	if( GetBitCoil( &coil, &addr ) )
	{
		m_bitWatch.type = WATCHTYPE_1B;
		m_bitWatch.addr = addr;
		m_bitWatch.coil = coil;
		AddWatch( m_bitWatch );
	}
}


void CDlgWatch::OnBnClickedBtnadddatawatch()
{
	unsigned int coil;
	unsigned int addr;
	m_dataWatch.all = 0;
	if( GetDataCoil( &coil, &addr ) )
	{
		CComboBox* pCombo = static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
		m_dataWatch.type = pCombo->GetCurSel();
		m_dataWatch.addr = addr;
		m_dataWatch.coil = coil;
		if( coil == api_coilC || coil == api_coilT || m_dataWatch.type == WATCHTYPE_32D || m_dataWatch.type == WATCHTYPE_32H )
		{
			m_dataWatch.coil += 0x10;
		}
		AddWatch( m_dataWatch );
	}
}


void CDlgWatch::OnBnClickedBtnsetvalue()
{
	unsigned int coil;
	unsigned int addr;

	m_dataWatch.all = 0;
	if( GetDataCoil( &coil, &addr ) )
	{
		CComboBox* pCombo = static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
		m_dataWatch.type = pCombo->GetCurSel();
		m_dataWatch.addr = addr;
		m_dataWatch.coil = coil;
		if( coil == api_coilC || coil == api_coilT || m_dataWatch.type == WATCHTYPE_32D || m_dataWatch.type == WATCHTYPE_32H )
		{
			m_dataWatch.coil += 0x10;
		}
		//if( coil == api_coilD )
		{
			int base;
			UINT32 uNum = 0;
			CString str;
			CStdString strNum;
			GetDlgItem( IDC_EDIT_VALUE )->GetWindowText( str );
			strNum = str;
			pCombo = static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_VALUE) );
			int sel = pCombo->GetCurSel();
			if( pCombo->GetCurSel() == 0 )
				base = 10;
			else
				base = 16;
			if( Str2Num( (char*)strNum.ToAnsi(),&uNum, base ) )
			{
				lib_API_WriteData( m_dataWatch.coil, addr, uNum );
			}else
				MessageBox( _T("数据格式错误"), _T("提示") );
		}
	}
}


void CDlgWatch::OnPLCScanEnd(void)
{
	WatchData wdata;
	CString str;
	CString strText;
	CListCtrl* pCtrlList = static_cast<CListCtrl*>( GetDlgItem(IDC_LIST) );
	int n = pCtrlList->GetTopIndex();
	int nLast = n + pCtrlList->GetCountPerPage();
	int nMax = pCtrlList->GetItemCount();
	for (; n < nLast && n < nMax; n++)
	{
		wdata.all = pCtrlList->GetItemData( n );
		if( pCtrlList->GetItemText( n, 2 ) != ( str = GetDataValue( wdata ) ) )
			pCtrlList->SetItemText( n, 2, str );
	}
	if( m_dataWatch.all )
	{
		GetDlgItem( IDC_STATIC_DATA )->GetWindowText( strText );
		if( strText != ( str = GetDataValue( m_dataWatch ) ) )
			GetDlgItem( IDC_STATIC_DATA )->SetWindowText( str );
	}
	if( m_bitWatch.all || m_dataWatch.all )
		InvalidateRect( &m_rcColorBit,FALSE );
}


int CDlgWatch::AddWatch( WatchData wdata )
{
	WatchData wdataGet;
	CString str;
	CListCtrl* pCtrlList = static_cast<CListCtrl*>( GetDlgItem(IDC_LIST) );
	int count = pCtrlList->GetItemCount();
	
	//先遍历有没有存在 
	for(int i = 0; i < count; i++)
	{
		wdataGet.all = pCtrlList->GetItemData(i);
		if( wdataGet.coil == wdata.coil && wdataGet.addr == wdata.addr )
		{
			//选中
			pCtrlList->SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			pCtrlList->SetItemData( i, wdata.all );
			switch( wdata.type )
			{
			case WATCHTYPE_16D:		str = _T("16位十进制");		break;
			case WATCHTYPE_16H:		str = _T("16位十六进制");	break;
			case WATCHTYPE_32D:		str = _T("32位十进制");		break;
			case WATCHTYPE_32H:		str = _T("32位十六进制");	break;
			case WATCHTYPE_1B:		str = _T("1位");			break;
			default:
				MessageBox( _T("软原件错误"), _T("提示") ); return 0;
			}
			pCtrlList->SetItemText( i, 3,str );
			str = GetDataValue( wdata );
			pCtrlList->SetItemText( i, 2,str );
			return 1;
		}
	}
	if( count > 200 )
	{
		MessageBox( _T("已经超过最大观察值"), _T("提示") ); return 0;
	}
	switch( wdata.coil )
	{
	case api_coilX:		str = _T("X");break;
	case api_coilY:		str = _T("Y");break;
	case api_coilM:		str = _T("M");break;
	case api_coilS:		str = _T("S");break;
	case api_coilT:		str = _T("T");break;
	case api_coilC:		str = _T("C");break;
	case api_coilD:		str = _T("D");break;
	case api_coilV:		str = _T("V");break;
	case api_coilDT:	str = _T("TD");break;
	case api_coilDC:	str = _T("CD");break;
	case api_coilDD:	str = _T("DD");break;
	default:
		MessageBox( _T("软原件错误"), _T("提示") ); return 0;
	}
	int nItem = pCtrlList->InsertItem( 0, str );
	pCtrlList->SetItemData( nItem, wdata.all );
	if( wdata.coil == api_coilX || wdata.coil == api_coilY )
		str.Format(_T("%03o"),wdata.addr );
	else
		str.Format( _T("%d"), wdata.addr );
	pCtrlList->SetItemText( nItem, 1,str );
	pCtrlList->SetItemText( nItem, 2,GetDataValue( wdata ) );
	switch( wdata.type )
	{
	case WATCHTYPE_16D:		str = _T("16位十进制");		break;
	case WATCHTYPE_16H:		str = _T("16位十六进制");	break;
	case WATCHTYPE_32D:		str = _T("32位十进制");		break;
	case WATCHTYPE_32H:		str = _T("32位十六进制");	break;
	case WATCHTYPE_1B:		str = _T("1位");			break;
	default:
		MessageBox( _T("软原件错误"), _T("提示") ); return 0;
	}
	pCtrlList->SetItemText( nItem, 3,str );
	return 0;
}


void CDlgWatch::OnPaint()
{
	COLORREF color;
	CPaintDC dc(this); // device context for painting
	if( m_bitWatch.all )
	{
		color = ( lib_API_ReadBit( m_bitWatch.coil, m_bitWatch.addr ) ? RGB( 10,236,115 ) : RGB( 180,180,180 ) );
	}else
	{
		color = RGB( 236,233,216 );
	}
	dc.FillSolidRect( &m_rcColorBit, color );
}

CString CDlgWatch::GetDataValue( WatchData wdata )
{
	UINT32 dGet;
	UINT32 value;
	CString str;
	if( wdata.type < WATCHTYPE_1B )
	{
		value = 0;
		dGet = 0;
		str.Empty();
		if( !lib_API_ReadData( wdata.coil, wdata.addr,&value ) )
			str = _T("读取失败！");
		/*if( !lib_API_ReadData( wdata.coil, wdata.addr,&dGet ) )
			str = _T("读取失败！");
		else
		{
			if( wdata.type == WATCHTYPE_32D || wdata.type == WATCHTYPE_32H )
			{
				value = ( dGet << 16 );
				if( !lib_API_ReadData( wdata.coil, wdata.addr + 1,&dGet ) )
					str = _T("读取失败！");
				else
					value |= dGet;
			}else
				value = dGet;
		}*/
		if( str.IsEmpty() )
		{
			if( wdata.type == WATCHTYPE_32D || wdata.type == WATCHTYPE_16D )
				str.Format(_T("%d"),value );
			else
				str.Format(_T("%x"),value );
		}
	}else if( wdata.type == WATCHTYPE_1B )
	{
		str = ( lib_API_ReadBit( wdata.coil, wdata.addr ) ? _T("ON") : _T("OFF") );
	}
	return str;
}


void CDlgWatch::OnCbnSelchangeComboType()
{
	if( m_dataWatch.all )
	{
		CComboBox* pCombo = static_cast<CComboBox*>( GetDlgItem(IDC_COMBO_TYPE) );
		m_dataWatch.type = pCombo->GetCurSel();
	}
}


void CDlgWatch::OnBnClickedBtnDelete()
{
	int nItem;
	CListCtrl* pCtrlList = static_cast<CListCtrl*>( GetDlgItem(IDC_LIST) );
	while( ( nItem = pCtrlList->GetNextItem(-1,LVNI_ALL | LVNI_SELECTED) ) != -1)
		pCtrlList->DeleteItem(nItem);
}


void CDlgWatch::OnBnClickedBtnclear()
{
	CListCtrl* pCtrlList = static_cast<CListCtrl*>( GetDlgItem(IDC_LIST) );
	pCtrlList->DeleteAllItems();
}
