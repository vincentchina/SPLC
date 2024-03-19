#pragma once
#include "common\StdString.h"
#include <Windows.h>
#include <tchar.h>

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


class LIBX_API CBaseCoil
{
public:
	enum eFrameType{
		eInputCoil,
		eOutputCoil,
		eFnc,
	};
	/*
	 * 将一个单元分成4部分，1、上标文字；2、元件图标；3、下标文字；4、底部文字
	 * 最小模式只有1、2
	 * 正常模式1、2、3
	 * 最大模式1、2、3、4
	 */
	enum eDrawUnitType{	
		eDrawUnitMin,
		eDrawUnitNormal,
		eDrawUnitMax,
	};
	typedef struct tagDrawInfo{		//用于绘制的信息，使用这个信息，因此摈弃了固定大小后再缩放
		int				nRowLen;
		int				nColLen;
		eDrawUnitType	eDUType;		
		int				nLineOffset;	//横线的offset
		int				nSpase;			//-||- 中间宽度
		SIZE			offsetMid;		//中间顶点的偏移量，用于绘制箭头、||等
		SIZE			szArrow;		//箭头大小
		int				nOutPutLine;	//输出线圈的线段长度
		int				nRadius;		//输出线圈半径
		int				nHeightVline;	//垂线的长度，因为有注释线圈的时候，长度要加上注释线圈的高度
		HDC				hdc;
		HFONT			hFont;
		HFONT			hOldFont;
		int				nUnitLen;		//单位长度，col为4个单位长度，row为8个单位长度
		COLORREF		colorFont;
		COLORREF		colorNote;
		COLORREF		colorOutNote;
		COLORREF		colorOldFont;
		COLORREF		colorStateColse;	//没有使用
	}DrawInfo,*LPDrawInfo;
	static BOOL PrepareDrawInfo(LPDrawInfo pInfo, int nRowLen, int nColLen, eDrawUnitType eType );
	static void DeleteDrawInfo( LPDrawInfo pInfo );
	static BOOL BeginPaint( HDC hdc,LPDrawInfo pInfo );
	static void EndPaint( LPDrawInfo pInfo );
	static void DrawFrame( LPDrawInfo pInfo, SIZE &posOffset,eFrameType eType,int nColOccupation = 1 );

protected:
	UINT m_uCoilType;

	//在schematic中有保存该位置信息，现在不用了
	//int m_nPosRow;
	//int m_nPosCol;
	int m_nColOccupation;		//占用了几列，只能占用多列，不能占用多行
	CStdString m_strName;
	CStdString m_strNote;
	
public:
	CStdString m_strIns;

	BOOL m_bLinkIn;//有coil连接到该coil的标志，不用关心是那个coil连接
	CBaseCoil* m_pRightUp;
	CBaseCoil* m_pRight;
	CBaseCoil* m_pLeftDown;

	void* m_userData;

public:
	CBaseCoil( const CBaseCoil& coil );
	CBaseCoil( UINT uType = COIL_BASE,CStdString strName=_T("") );
	virtual ~CBaseCoil(void){}
	static void DrawLine( LPDrawInfo pInfo, UINT uType,SIZE &posOffset );
	void EmptyLink();
	BOOL IsOrphan();
	void SetName( const CStdString& str ){ m_strName = str;}
	CStdString GetName(){ return m_strName; }
	void SetNote( const CStdString& str ){ m_strNote = str; }
	CStdString GetNote(){ return m_strNote; }
	UINT GetType(){ return m_uCoilType; }
	int GetColOccupation(){ return m_nColOccupation; }
	virtual void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};
