#pragma once
#include "Coils.h"
#include <vector>
using std::vector;

//改的东西真多，经验不足啊，没有设计好
/* 读入文件的时候，插入是以输入线为准的,即所有插入都是在输入线上插入
 * 只有两种行，一种是基本线圈行和注释线圈
 * */
namespace libSch{

typedef struct tagCoilPoint{
	CBaseCoil* hCoil;
	CBaseCoil* vCoil;		//垂直Coil目前只支持垂线
}CoilPoint, *LPCoilPoint;

typedef LPCoilPoint LPCoilLine;

typedef struct tagCoilPos{
	int nLn;
	int nCol;
}CoilPos,*LPCoilPos;

typedef struct tagInputLine{
	int nStart;
	int nMaxRow;
	CoilPos pos;
}InputLine,*LPInputLine;

typedef CoilPos BlockSize,*LPBlockSize;

//lad to ins
enum eBLogicType{
	eInput,
	eOutput,
	eError,
};
typedef struct tagLinePos {
	int nRow;
	int nColStart;
	int nColEnd;
	vector<CBaseCoil*> vCoil;
}LinePos,*LPLinePos;
typedef struct tagCoilInfo{
	CBaseCoil* pCoil;
	CoilPos pos;
}CoilInfo,*LPCoilInfo;

typedef struct tagOutPutInfo{
	CoilPos pos;
	BOOL isDirect;
}OutPutInfo,*LPOutPutInfo;

template class LIBX_API std::allocator< LPCoilLine >;
template class LIBX_API std::vector<LPCoilLine ,std::allocator<LPCoilLine> >;

template class LIBX_API std::allocator< InputLine >;
template class LIBX_API std::vector<InputLine ,std::allocator<InputLine> >;

class LIBX_API CCoilBlock
{
public:
	
protected:
	int m_nBlockID;
	BlockSize m_szBlock;			//块的最大size
	//CoilPos m_posCurrent;			//当前可输入的pos

public:
	vector<LPCoilLine> m_vCoil;		//一定是连续的
	vector<InputLine> m_vILine;		//输入线位置 辅助加载ins文件转成块
	InputLine m_ILine;				//当前输入线
	int m_nLink;		
	//Coil在matherline 和 outputline之间 输入
	int m_nMatherLine;				//母线位置 垂直
	int m_nOutputLine;				//输出线位置 垂直
	int m_nNoteCoilPos;				//

	CoilPos m_posErr;
protected:
	//这个操作不会更新
	int _InsertCoil( CBaseCoil* pCoil, CoilPos &pos );
	
public:

	CCoilBlock( int nRow = 100, int nCol = 14, int nID = 0 );
	~CCoilBlock(void);

	static LPCTSTR FormatInputType( CBaseCoil* pCoil );
	static CBaseCoil* CopyCoil( CBaseCoil* pSource );
	static UINT GetCoilType( CBaseCoil* pCoil );
	int GetMaxRow(){ return m_ILine.nMaxRow; }
	void AddLineCoil( LPCoilLine pLine, int nCol,UINT uLineType );
	void AddLineCoil( UINT uLineType,CoilPos& pos );
	void DeleteLineCoil( CoilPos& pos,UINT uLineType );
	UINT GetLineCoilType( CoilPos& pos );
	BOOL InsertNoteLine( CBaseCoil* pCoil,int nRow );
	BOOL IsHaveNote( int nRow );
	CBaseCoil* GetNoteCoil( int nRow );
	BOOL InsertLable( CBaseCoil* pCoil,int nLn );
	LPCoilPoint GetCoilPoint( CoilPos& pos );
	BOOL SetErrorPos( CoilPos& pos );
	CoilPos SetPos( CoilPos& pos );
	BOOL InsertCoil( CBaseCoil* pCoil, CoilPos &pos,CBaseCoil** pOld = NULL );

	CBaseCoil* FindCoil( CoilPos& pos, int *pColStart = NULL,int *pColEnd = NULL );
	CBaseCoil* GetCoil( CoilPos& pos );
	CBaseCoil* GetCoil(  LPCoilLine pLine, int nCol  );
	BlockSize GetMaxSize();
	BlockSize SetMaxSize( LPBlockSize pSize );
	LPCoilLine GetLine( int nRow );
	LPCoilLine UpdateLine( int nRow );
	int GetCoilOccupation( CBaseCoil* pCoil );
	BOOL DeleteCoil( CoilPos pos );

	BOOL InsertLine( int nLn );

	//用于 to lad
	BOOL InputLine2Ins( int nLn, CStdString& strIns );
	void Ins2LadReset();
	BOOL LDCoil( CBaseCoil* pCoil );
	void MPP( int nCol );
	int MPS(){ return m_ILine.pos.nCol; }
	BOOL AndBlock();
	BOOL AndCoil( CBaseCoil* pCoil );
	BOOL OrBlock();
	BOOL OrCoil( CBaseCoil* pCoil );
	BOOL OutCoil( CBaseCoil* pCoil );		//不会有outblock
	BOOL OutLine( CBaseCoil* pCoil );
	BOOL P( CBaseCoil* pCoil );
	BOOL End();
	
	//用于 to ins
	void EmptyCoilIns();
	BOOL GetOutputInfo( CoilPos pos,vector<OutPutInfo>& vOutPut );
	int CopyLine( LPCoilLine pLineCopy, int sz );
	BOOL MinBlock2Ins( CoilPos pos,CStdString& strIns );
	BOOL IsMinBlock( CoilPos pos,BOOL bVLineHasDeal = FALSE );
	BOOL GetMinOr( CoilPos pos, CoilPos& posor,vector<CoilInfo>& vCoil,int& nMidCol );
	BOOL ChangeToMinBlock();
	BOOL IsEmptyBlock();
	BOOL IsInputLine( CoilPos& pos );
	BOOL AdjustConnection();
	int GetFirstInputLine();
	int GetNextInputLine( int nln );
};


}