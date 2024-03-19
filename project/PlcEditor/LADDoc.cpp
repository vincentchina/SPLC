
// LADDoc.cpp : CLADDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "PlcEditor.h"
#endif
#include "PlcEditor.h"
#include "LADDoc.h"
#include "mainfrm.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLADDoc

IMPLEMENT_DYNCREATE(CLADDoc, CDocument)

BEGIN_MESSAGE_MAP(CLADDoc, CDocument)
END_MESSAGE_MAP()


// CLADDoc 构造/析构

CLADDoc::CLADDoc()
{
	// TODO: 在此添加一次性构造代码

}

CLADDoc::~CLADDoc()
{
}

BOOL CLADDoc::OnNewDocument()
{
	theApp.OnFileNew();
	return FALSE;
}




// CLADDoc 序列化

void CLADDoc::Serialize(CArchive& ar)
{
	char* strAnsi;
	CFile* file = ar.GetFile();
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
		CStdString str;
		if( !m_schematic.ToIns( str ) )
		{
			libSch::CoilPos pos = m_schematic.GetErrorPos();
		}
		strAnsi = (char*)str.ToAnsi();
		//目前只保存ansi
		file->Write( strAnsi, strlen(strAnsi) );
	}
	else
	{
		// TODO: 在此添加加载代码
		char* buf;
		buf = new char[ file->GetLength() + 1 ];
		file->Read( buf,file->GetLength() );
		buf[ file->GetLength() ] = 0;
		m_schematic.FromInsBuff( buf, file->GetLength() );
		delete[] buf;
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CLADDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CLADDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CLADDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CLADDoc 诊断

#ifdef _DEBUG
void CLADDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLADDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLADDoc 命令

BOOL CLADDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString strFilter = lpszPathName;
	int nPos = strFilter.ReverseFind(_T('.'));
	if( nPos == -1 )
		return FALSE;
	strFilter = strFilter.Right( strFilter.GetLength() - nPos );
	if( strFilter == _T(".pproj") )
	{
		if( theApp.GetProject().IsOpen() )
		{
			if( MessageBox(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),_T("工程已经更改，是否保存？"),_T("提示"),MB_OKCANCEL ) == IDOK )
			{
				theApp.GetProject().Close();
				( (CMainFrame*)AfxGetMainWnd() )->m_wndFileView.CloseProject();
			}else
			{
				theApp.AddToRecentFileList( lpszPathName );
				return FALSE;
			}
		}
		if( theApp.GetProject().Load( CString(lpszPathName) ) )
		{
			( (CMainFrame*)AfxGetMainWnd() )->m_wndFileView.LoadProject();
			theApp.AddToRecentFileList( lpszPathName );
		}
	}else if( strFilter == _T(".lad") )
	{
		libSch::BlockSize szBlock;
		szBlock.nLn = theApp.GetProject().m_nMaxLn;
		szBlock.nCol = theApp.GetProject().m_nMaxCol + 1;
		m_schematic.SetBlockSize( &szBlock );
		if( !theApp.GetProject().IsOpen() || !CDocument::OnOpenDocument(lpszPathName) )
			return FALSE;
		return TRUE;
		//SetModifiedFlag();
	}
	return FALSE;
}
