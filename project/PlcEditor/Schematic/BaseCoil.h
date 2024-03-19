#pragma once

#include "..\PLCCore\plc_instruction.h"

//#define NOTECOIL_SYMBOL //定义注释线圈占用一行
#define NAME_MAX_LEN	20

#define COIL_ERROR			-1
#define COIL_NULL			0
#define COIL_VLINE			1
#define COIL_HLINE			2
#define COIL_VHLINE			3
#define MARK_COIL_SYMBOL	0x07
#define COIL_MATHERLINE		(1<<3)
#define COIL_OUTPUTLINE		(2<<3)
#define COIL_OCCUPATION		(3<<3)
#define COIL_BASE			(4<<3)
#define COIL_NOTE			(5<<3)
#define COIL_INPUT			(6<<3)
#define COIL_OUTPUT			(7<<3)
#define COIL_SYMBOL			(8<<3)
#define COIL_LINKER			(9<<3)

class CBaseCoil
{
public:
	typedef struct tagDrawInfo{		//用于绘制的信息，使用这个信息，因此摈弃了固定大小后再缩放
		int nRowLen;
		int nColLen;
		int nLineOffset;	//横线的offset
		int nSpase;			//-||- 中间宽度
		SIZE offsetMid;		//中间顶点的偏移量，用于绘制箭头、||等
		SIZE szArrow;		//箭头大小
		int nOutPutLine;	//输出线圈的线段长度
		int nRadius;		//输出线圈半径
		int nHeightVline;	//垂线的长度，因为有注释线圈的时候，长度要加上注释线圈的高度
		HDC hdc;
		HFONT hFont;
		HFONT hOldFont;
		int nUnitLen;		//单位长度，col为4个单位长度，row为8个单位长度
		COLORREF colorFont;
		COLORREF colorNote;
		COLORREF colorOutNote;
		COLORREF colorOldFont;
	}DrawInfo,*LPDrawInfo;
	static BOOL PrepareDrawInfo(LPDrawInfo pInfo, int nRowLen, int nColLen );
	static void DeleteDrawInfo( LPDrawInfo pInfo );
	static BOOL BeginPaint( HDC hdc,LPDrawInfo pInfo );
	static void EndPaint( LPDrawInfo pInfo );

protected:
	UINT m_uCoilType;

	//在schematic中有保存该位置信息，现在不用了
	//int m_nPosRow;
	//int m_nPosCol;
	int m_nColOccupation;		//占用了几列，只能占用多列，不能占用多行
	LPTSTR m_strName;
	LPTSTR m_strNote;
	UINT32 m_uMark;
	void*	m_pUserData;
public:
	CString m_strIns;

	BOOL m_bLinkIn;//有coil连接到该coil的标志，不用关心是那个coil连接
	CBaseCoil* m_pRightUp;
	CBaseCoil* m_pRight;
	CBaseCoil* m_pLeftDown;

public:
	CBaseCoil( const CBaseCoil& coil );
	CBaseCoil( UINT uType = COIL_BASE,LPTSTR strName = NULL );
	virtual ~CBaseCoil(void){ if(m_strName) delete[] m_strName; if(m_strNote) delete[] m_strNote; }
	static void DrawLine( LPDrawInfo pInfo, UINT uType,SIZE &posOffset );
	LPTSTR GetName(){ return m_strName; }
	void EmptyLink();
	BOOL IsOrphan();
	BOOL SetName( LPTSTR str );
	BOOL SetNote( LPTSTR str );
	UINT GetType(){ return m_uCoilType; }
	int GetColOccupation(){ return m_nColOccupation; }
	void* GetUserData(){ return m_pUserData; }
	void SetUserData( void* pUserData ) { m_pUserData = pUserData; }
	virtual void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};
