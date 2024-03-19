#pragma once
class CVirtualMachine
{
public:
	typedef void (*funStopCallBack)( int aix );
protected:
	typedef struct tagData{
		int bStart;
		int speed;
		int location;
		int bAbsolute;
	}Data,*LPData;
	LPData m_pAixs;
	int m_nTotalAixs;
	BOOL m_bRun;
	HANDLE m_hWorkThread;
	funStopCallBack m_funStopCallBack;
public:
	static DWORD WINAPI WorkThread(LPVOID pParam);
public:
	CVirtualMachine( int nTotalAixs = 6, funStopCallBack pStop = NULL );
	~CVirtualMachine(void);
	int Action( int aix, int speed, int location, int bAbsolute );
	void SetSopCallBack( funStopCallBack pStop ) { m_funStopCallBack = pStop;}
};

