#pragma once
#include "Coils.h"
#include "CoilBlock.h"


#define  MAXSZ_ARG	10

#define ARG_COIL_X	0x001
#define ARG_COIL_Y	0x002
#define ARG_COIL_M	0x004
#define ARG_COIL_S	0x008
#define ARG_COIL_T	0x010
#define ARG_COIL_C	0x020
#define ARG_COIL_D	0x040
#define ARG_COIL_P	0x080
#define ARG_COIL_K	0x100

class CSchematic
{
public:
	enum eCoilInsType{
		eNULL,
		eOpen,
		eClose,
		eOROpen,
		eORClose,
		eOut,
		eFNC,
		ePCoil,
		eFCoil,
		eORPCoil,
		eORFCoil,
		eP,
		eF,
		eINV
	};
private:
	SIZE m_szMax;		//schematic 大小
	int m_nRowLen;		//行大小
	int m_nColLen;		//列大小
	BOOL m_bReDraw;		//需要重绘标志
	CBaseCoil::DrawInfo	m_infoDraw;
	CString m_strErr;

protected:
	int m_nPosRow;							//当前显示第几行
	int m_nPosCol;							//当前显示第几列
	int m_nNumRow;							//显示几行
	int m_nNumCol;							//显示几列
	SIZE m_szView;							//在实际CSchematic的剪切区
	CCoilBlock m_cBlock;

public:
	CSchematic(void);
	~CSchematic(void);
	CBaseCoil::DrawInfo* GetDrawInfo() { return &m_infoDraw; }
	BOOL ToIns( LPTSTR strFile );
	BOOL ToIns( CFile& strFile );
	BOOL FromIns( LPTSTR strFile );
	BOOL FromInsBuff( char* buff, int sz );
	BOOL Ins2Coil( char* pList[], int szlist,vector<int>& vColPos );
	BOOL IsEmptyLine( CCoilBlock::LPCoilLine pLine );
	void CleanEmptyLine();
	BOOL InsertLine( int nLn ) { return m_cBlock.InsertLine(nLn); }
	CCoilBlock::CoilPos GetErrorPos() { return m_cBlock.m_posErr; }

	void SetViewSize(int nx, int ny );
	BOOL SetViewPos( int nRow, int nCol );
	SIZE GetViewSize();
	void GetViewPos( int *pnRow,int *pnCol );
	int GetColLen(){ return m_nColLen; }
	int GetRowLen(){ return m_nRowLen; }
	int GetViewRowCount() { return m_nNumRow; }
	int GetViewColCount() { return m_nNumCol; }
	SIZE GetSize();
	CCoilBlock::BlockSize GetBlockSize() { return m_cBlock.GetMaxSize(); }
	CCoilBlock::BlockSize SetBlockSize( CCoilBlock::LPBlockSize pSize ){ return m_cBlock.SetMaxSize( pSize );}

	void Draw( HDC hdc, BOOL bForeDraw = FALSE );
	void DrawHelp( HDC hdcDest,HDC hdcSrc );
	int GetHeight();
	CBaseCoil* GetPosCoil( CCoilBlock::CoilPos &pos, RECT* prc );
	CBaseCoil* GetPosCoil( POINT &pt, CCoilBlock::CoilPos &pos, RECT* prc =NULL );
	int GetTotalRow() { return m_cBlock.GetMaxRow(); }
	int GetTotalCol() { return ( m_cBlock.GetMaxSize().nCol - 3 ); }
	BOOL InsertCoil( CBaseCoil* pCoil, CCoilBlock::CoilPos &pos,CBaseCoil** pOld = NULL ) { return m_cBlock.InsertCoil(pCoil,pos,pOld); }
	BOOL DeleteCoil( CCoilBlock::CoilPos pos ){ return m_cBlock.DeleteCoil(pos); }
	void DeleteLine( CCoilBlock::CoilPos& pos,UINT uLineType ){ m_cBlock.DeleteLineCoil( pos,uLineType ); }
	void AddLine( CCoilBlock::CoilPos& pos,UINT uLineType ){ m_cBlock.AddLineCoil( uLineType,pos ); }
	
	static BOOL TestCoilType( UINT uFormat,LPTSTR pstr );
	BOOL TestInsInput( CCoilBlock::CoilPos& pos,eCoilInsType eType, CString ins );
	BOOL OnInsInputChange( CCoilBlock::CoilPos& pos,eCoilInsType eType, CString& ins);
	void ResetBlock(void);
};

