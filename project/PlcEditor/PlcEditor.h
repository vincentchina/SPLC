
// PlcEditor.h : PlcEditor 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "Project.h"

// CPlcEditorApp:
// 有关此类的实现，请参阅 PlcEditor.cpp
//

class CPlcEditorApp : public CWinAppEx
{
public:
	CPlcEditorApp();


protected:
	CProject m_project;
	CMultiDocTemplate* m_pDocTemplate;

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	CProject& GetProject(){ return m_project; }
	DECLARE_MESSAGE_MAP()
	
};

extern CPlcEditorApp theApp;
