
// PlcEditor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "PlcEditor.h"
#include "MainFrm.h"
#include "DlgNewFile.h"

#include "ChildFrm.h"
#include "LADDoc.h"
#include "LADView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPlcEditorApp

BEGIN_MESSAGE_MAP(CPlcEditorApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CPlcEditorApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CPlcEditorApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CPlcEditorApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CPlcEditorApp 构造

CPlcEditorApp::CPlcEditorApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("PlcEditor.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CPlcEditorApp 对象

CPlcEditorApp theApp;


// CPlcEditorApp 初始化

BOOL CPlcEditorApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	m_pDocTemplate = new CMultiDocTemplate(IDR_PlcEditorTYPE,
		RUNTIME_CLASS(CLADDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CLADView));
	if (!m_pDocTemplate)
		return FALSE;
	AddDocTemplate(m_pDocTemplate);

	// 创建主 MDI 框架窗口
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 MDI 应用程序中，这应在设置 m_pMainWnd 之后立即发生
	// 启用拖/放
	m_pMainWnd->DragAcceptFiles();

	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	cmdInfo.m_nShellCommand=CCommandLineInfo::FileNothing;

	// 启用“DDE 执行”
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	//初始化ins列表
	initInsHash();
	return TRUE;
}

int CPlcEditorApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CPlcEditorApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CPlcEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CPlcEditorApp 自定义加载/保存方法

void CPlcEditorApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CPlcEditorApp::LoadCustomState()
{
}

void CPlcEditorApp::SaveCustomState()
{
}

// CPlcEditorApp 消息处理程序
void CPlcEditorApp::OnFileOpen()
{
	// TODO: Add your command handler code here
#ifdef _DEBUG
	const BOOL VistaStyle = FALSE;
#else
	const BOOL VistaStyle = TRUE;
#endif
	if( m_project.IsOpen() )
	{
		if( MessageBox(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),_T("工程已经更改，是否保存？"),_T("提示"),MB_OKCANCEL ) == IDOK )
		{
			m_project.Save();
			//( (CMainFrame*)AfxGetMainWnd() )->m_wndFileView.CloseProject();
		}
		m_project.Close();
	}
	CFileDialog dlg(TRUE, _T("*.pproj"), _T("*.pproj"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, _T("Project File(*.pproj)|*.pproj|All Files(*.*)|*.*||"),NULL,0,VistaStyle);
	dlg.m_ofn.lpstrTitle=_T("Open Project File");
	if(dlg.DoModal() == IDOK)
	{
		m_pDocTemplate->OpenDocumentFile( dlg.GetPathName() );
	}
}
void CPlcEditorApp::OnFileNew()
{
	// TODO: Add your command handler code here
	ASSERT(m_pDocTemplate);
	CDlgNewFile Dlg;
	if( m_project.IsOpen() )
	{
		
	}
	if( Dlg.DoModal() == IDOK )
	{
		Dlg.m_strProjectPath += _T("\\") + Dlg.m_strProjectName;
		if( Dlg.m_bMKDir )
		{
			if( !CreateDirectory(Dlg.m_strProjectPath, NULL ) && GetLastError() != ERROR_ALREADY_EXISTS )
			{
				MessageBox(NULL,_T("创建文件夹失败！"),_T("错误"),MB_OK );
				return;
			}
			Dlg.m_strProjectPath += _T("\\") + Dlg.m_strProjectName;
		}
		Dlg.m_strProjectPath += _T(".pproj");
		if( m_project.Create( Dlg.m_strProjectPath ) )
		{
			m_project.m_nMaxLn = Dlg.m_nStep;
			m_project.m_nMaxCol = Dlg.m_nCol;
			m_project.Save();
			AddToRecentFileList( Dlg.m_strProjectPath );
			m_pDocTemplate->OpenDocumentFile( Dlg.m_strProjectPath );
		}
	}
}
