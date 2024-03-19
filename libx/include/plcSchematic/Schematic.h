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
#define ARG_COIL_V	0x200

namespace libSch{

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

class LIBX_API CSchematic
{
public:
	
private:
	SIZE m_szMax;		//schematic 大小
	int m_nRowLen;		//行大小
	int m_nColLen;		//列大小
	BOOL m_bReDraw;		//需要重绘标志
	CBaseCoil::DrawInfo	m_infoDraw;
	CStdString m_strErr;

protected:
	int m_nPosRow;							//当前显示第几行
	int m_nPosCol;							//当前显示第几列
	int m_nNumRow;							//显示几行
	int m_nNumCol;							//显示几列
	SIZE m_szView;							//在实际CSchematic的剪切区
	CCoilBlock m_cBlock;
	CBaseCoil::eDrawUnitType m_eDUType;
public:
	CSchematic(void);
	~CSchematic(void);
	CBaseCoil::DrawInfo* GetDrawInfo() { return &m_infoDraw; }
	BOOL ToIns( CStdString& str );
	BOOL ToIns( FILE* fp );
	BOOL FromIns( CStdString strFile );
	BOOL FromInsBuff( char* buff, int sz );
	BOOL Ins2Coil( char* pList[], int szlist,vector<int>& vColPos );
	BOOL IsEmptyLine( libSch::LPCoilLine pLine );
	void CleanEmptyLine();
	BOOL InsertLine( int nLn ) { return m_cBlock.InsertLine(nLn); }
	CoilPos GetErrorPos() { return m_cBlock.m_posErr; }

	void SetViewSize(int nx, int ny );
	BOOL SetViewPos( int nRow, int nCol );
	SIZE GetViewSize();
	void GetViewPos( int *pnRow,int *pnCol );
	int GetColLen(){ return m_nColLen; }
	int GetRowLen(){ return m_nRowLen; }
	int GetViewRowCount() { return m_nNumRow; }
	int GetViewColCount() { return m_nNumCol; }
	SIZE GetSize();
	BlockSize GetBlockSize() { return m_cBlock.GetMaxSize(); }
	BlockSize SetBlockSize( LPBlockSize pSize ){ return m_cBlock.SetMaxSize( pSize );}

	void Draw( HDC hdc, BOOL bForeDraw = FALSE );
	void DrawHelp( HDC hdcDest,HDC hdcSrc );

	int GetHeight();
	CBaseCoil* GetPosCoil( CoilPos &pos, RECT* prc );
	CBaseCoil* GetPosCoil( POINT &pt, CoilPos &pos, RECT* prc =NULL );
	int GetTotalRow() { return m_cBlock.GetMaxRow(); }
	int GetTotalCol() { return ( m_cBlock.GetMaxSize().nCol - 3 ); }
	BOOL InsertCoil( CBaseCoil* pCoil, CoilPos &pos,CBaseCoil** pOld = NULL ) { return m_cBlock.InsertCoil(pCoil,pos,pOld); }
	BOOL DeleteCoil( CoilPos pos ){ return m_cBlock.DeleteCoil(pos); }
	void DeleteLine( CoilPos& pos,UINT uLineType ){ m_cBlock.DeleteLineCoil( pos,uLineType ); }
	void AddLine( CoilPos& pos,UINT uLineType ){ m_cBlock.AddLineCoil( uLineType,pos ); }
	
	static BOOL TestCoilType( UINT uFormat,char* pstr, unsigned int* addr = NULL );
	static BOOL TestCoilType( UINT uFormat,CStdString& pstrTest,unsigned int* coil, unsigned int* addr = NULL );
	BOOL TestInsInput( CoilPos& pos,eCoilInsType eType, CStdString ins );
	BOOL OnInsInputChange( CoilPos& pos,eCoilInsType eType, CStdString& ins);
	
	void ResetBlock(void);

	//读取
	LPCoilLine GetLine( int nLn ) { return m_cBlock.GetLine( nLn ); }
	CBaseCoil* GetCoil( LPCoilLine pLn, int nCol ) { return m_cBlock.GetCoil(  pLn, nCol ); }
	CStdString GetErr(){ return m_strErr; }
};

}