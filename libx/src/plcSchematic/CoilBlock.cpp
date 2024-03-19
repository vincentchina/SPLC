#include "plcSchematic\CoilBlock.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif*/
using namespace libSch;

CCoilBlock::CCoilBlock( int nLn, int nCol, int nID ) :
m_nBlockID(nID)
{
	memset( &m_ILine, -1, sizeof( InputLine ) );//辅助将文件转化为块，哨兵ILine，不可能被弹出，弹出出错
	m_szBlock.nLn = nLn;
	m_szBlock.nCol = nCol + 1;	//最后一个COL存放注释
	m_nMatherLine = 1;
	m_nOutputLine = nCol - 1;
	m_nNoteCoilPos = nCol;
}

CCoilBlock::~CCoilBlock(void)
{
	vector<LPCoilLine>::iterator iter;
	for( iter = m_vCoil.begin(); iter != m_vCoil.end(); iter++ )
		delete *iter;
}

CBaseCoil* CCoilBlock::CopyCoil( CBaseCoil* pSource )
{
	if( (UINT)pSource < COIL_BASE )
		return pSource;
	switch( pSource->GetType() )
	{
	case COIL_BASE:
		return ( new CBaseCoil( *pSource ) );
	case COIL_NOTE:
		return ( new CNoteCoil( *static_cast<CNoteCoil*>( pSource ) ) );
	case COIL_INPUT:
		return new CInputCoil( *static_cast<CInputCoil*>( pSource ) );
	case COIL_OUTPUT:
		return ( new COutputCoil( *static_cast<COutputCoil*>( pSource ) ) );
	case COIL_SYMBOL:
		return ( new CSymbolCoil( *static_cast<CSymbolCoil*>( pSource ) ) );
	case COIL_LINKER:
		return ( new CLinkCoil( *static_cast<CLinkCoil*>( pSource ) ) );
	}
	return NULL;
}
void CCoilBlock::AddLineCoil( LPCoilLine pLine, int nCol,UINT uLineType )
{
	if( pLine == NULL || nCol < m_nMatherLine || nCol > m_nOutputLine ) return;
	if( uLineType & COIL_VLINE )
	{
		if( (UINT)pLine[nCol].vCoil >= COIL_BASE ) return;
		pLine[nCol].vCoil = (CBaseCoil*)COIL_VLINE;
	}
	if( uLineType & COIL_HLINE )
	{
		if( (UINT)pLine[nCol].hCoil >= COIL_BASE ) return;
		pLine[nCol].hCoil = (CBaseCoil*)COIL_HLINE;
	}
}
void CCoilBlock::AddLineCoil( UINT uLineType, CoilPos& pos )
{
	AddLineCoil( GetLine( pos.nLn ), pos.nCol, uLineType );
}
void CCoilBlock::DeleteLineCoil( CoilPos& pos,UINT uLineType )
{
	LPCoilLine pLine;
	pLine = GetLine( pos.nLn );
	if( pLine == NULL || pos.nCol < m_nMatherLine || pos.nCol > m_nOutputLine ) return;
	if( uLineType & COIL_VLINE )
	{
		if( (UINT)pLine[pos.nCol].vCoil >= COIL_BASE || (UINT)pLine[pos.nCol].vCoil == COIL_OCCUPATION ) return;
		pLine[pos.nCol].vCoil = 0;
	}
	if( uLineType & COIL_HLINE )
	{
		if( (UINT)pLine[pos.nCol].hCoil >= COIL_BASE || (UINT)pLine[pos.nCol].hCoil == COIL_OCCUPATION ) return;
		pLine[pos.nCol].hCoil = 0;
	}
}

UINT CCoilBlock::GetLineCoilType( CoilPos& pos )
{
	UINT nType = 0;
	LPCoilLine pLine = GetLine( pos.nLn );
	if( pLine == NULL || pos.nCol < m_nMatherLine || pos.nCol > m_nOutputLine ) return NULL;
	if( (UINT)pLine[pos.nCol].vCoil == COIL_VLINE ) nType |= COIL_VLINE;
	if( (UINT)pLine[pos.nCol].hCoil == COIL_HLINE ) nType |= COIL_HLINE;
	return nType;
}

BOOL CCoilBlock::InsertLine( int nLn )
{
	vector<LPCoilLine>::iterator iter,iterMove;
	if( nLn > m_vCoil.size() )
		return FALSE;
	if( UpdateLine( m_vCoil.size() ) )
	{
		++m_ILine.nMaxRow;
		for( iter = m_vCoil.begin(); nLn > 0 && iter != m_vCoil.end(); ++iter,--nLn );
		if( iter == m_vCoil.end() )
			return FALSE;
		LPCoilLine pLine = GetLine( m_vCoil.size() - 1 );
		iterMove = m_vCoil.end();
		while ( --iterMove != iter )
			*iterMove = *( iterMove - 1 );
		*iter = pLine;
		return TRUE;
	}
	return FALSE;
}

BOOL CCoilBlock::InsertCoil( CBaseCoil* pCoil, CoilPos &pos,CBaseCoil** pOld )
{
	int nBegin,nEnd;
	CBaseCoil* pCoilOld;
	pCoilOld = FindCoil( pos,&nBegin,&nEnd );
	if( pOld != NULL ) *pOld = pCoilOld;
	LPCoilLine pLine = GetLine( pos.nLn );
	while( nBegin <= nEnd )
		pLine[nBegin++].hCoil = NULL;
	return ( _InsertCoil( pCoil, pos ) > 0 );
}

BOOL CCoilBlock::InsertNoteLine( CBaseCoil* pCoil,int nLn )
{
	if ( nLn == -1 )
		nLn = 0;
	LPCoilLine pLine = UpdateLine(nLn);
	pLine[ m_nNoteCoilPos ].hCoil = pCoil;
	return TRUE;
}
BOOL CCoilBlock::IsHaveNote( int nLn )
{
	LPCoilLine pLine = GetLine( nLn );
	return ( pLine && ( GetCoilType( pLine[ m_nNoteCoilPos ].hCoil ) == COIL_NOTE ) );
}
CBaseCoil* CCoilBlock::GetNoteCoil( int nLn )
{
	LPCoilLine pLine = GetLine( nLn );
	if( pLine == NULL )
		return NULL;
	if( GetCoilType( pLine[ m_nNoteCoilPos ].hCoil ) == COIL_NOTE )
		return pLine[ m_nNoteCoilPos ].hCoil;
	else
		return NULL;
}

BOOL CCoilBlock::InsertLable( CBaseCoil* pCoil,int nLn )
{
	LPCoilLine pLine = UpdateLine( nLn );
	if( pLine[0].hCoil != NULL )
		return FALSE;
	pLine[0].hCoil = pCoil;
	return TRUE;
}

BlockSize CCoilBlock::GetMaxSize()
{ 
	return m_szBlock; 
}
BlockSize CCoilBlock::SetMaxSize( LPBlockSize pSize )
{ 
	vector<LPCoilLine>::iterator iter, iterErase;
	BlockSize old = m_szBlock; 
	int nIndex;
	LPCoilLine pLine;
	int i;
	LPCoilLine pNewLine;

	if( pSize )				
	{
		int nNewCol = pSize->nCol + 1; //最后一个COL存放注释
		int nMinCol = ( ( nNewCol > m_szBlock.nCol ) ? m_szBlock.nCol : nNewCol );
		nIndex = 0;
		for( iter = m_vCoil.begin(); iter != m_vCoil.end(); iter++ )
		{
			pLine = *iter;
			if ( nIndex++ >= pSize->nLn )
				break;
			pNewLine = new CoilPoint[ nNewCol ];
			for ( i = 0; i < nMinCol; i++ )
			{
				pNewLine[i].hCoil = pLine[i].hCoil;
				pNewLine[i].vCoil = pLine[i].vCoil;
			}
			for ( ; i < m_szBlock.nCol; i++ )
			{
				if( (UINT)pLine[i].hCoil >= COIL_BASE )
					delete pLine[i].hCoil;
				if( (UINT)pLine[i].vCoil >= COIL_BASE )
					delete pLine[i].vCoil;
			}
		}
		iterErase = iter;
		for( ; iter != m_vCoil.end(); iter++ )	//删除ln超出的coil
		{
			pLine = *iter;
			for ( int i = 0; i < m_szBlock.nCol; i++ )
			{
				if( (UINT)pLine[i].hCoil >= COIL_BASE )
					delete pLine[i].hCoil;
				if( (UINT)pLine[i].vCoil >= COIL_BASE )
					delete pLine[i].vCoil;
			}
			delete pLine;
		}
		m_vCoil.erase( iterErase,m_vCoil.end() );
		m_szBlock.nLn = pSize->nLn;
		m_szBlock.nCol = pSize->nCol + 1;	//最后一个COL存放注释
		m_nMatherLine = 1;
		m_nOutputLine = pSize->nCol - 1;
		m_nNoteCoilPos = pSize->nCol;
	}
	return  old;
}


LPCoilLine CCoilBlock::GetLine( int nLn )
{
	if( nLn < 0 || nLn >= (int)m_vCoil.size() )
		return NULL;
	return m_vCoil[nLn];
}

LPCoilLine CCoilBlock::UpdateLine( int nLn )
{
	if( nLn < 0 || nLn > m_szBlock.nLn )
		return NULL;
	if( nLn >= (int)m_vCoil.size() )
	{
		LPCoilLine pNewLine;
		do{
			pNewLine = new CoilPoint[ m_szBlock.nCol ];
			memset( pNewLine, 0, sizeof( CoilPoint ) * m_szBlock.nCol );
			m_vCoil.push_back( pNewLine );
		}while( (int)m_vCoil.size() <= nLn );
	}
	return m_vCoil[nLn];
}

CBaseCoil* CCoilBlock::FindCoil( CoilPos& pos, int *pColStart,int *pColEnd )
{
	LPCoilLine pLine = GetLine( pos.nLn );
	CBaseCoil* pCoil;
	if( !pLine || pos.nCol < 0 || pos.nCol > m_szBlock.nCol )
		return (CBaseCoil*)COIL_ERROR;
	pCoil = pLine[ pos.nCol ].hCoil;
	if( (UINT)pCoil >= COIL_BASE )
	{
		if ( pColStart ) *pColStart = pos.nCol;
		if ( pColEnd ) *pColEnd = pos.nCol + pCoil->GetColOccupation() - 1;
	}else if( (UINT)pCoil == COIL_OCCUPATION )
	{
		int nCol = pos.nCol;
		while( --nCol >= 0 && (UINT)pLine[ nCol ].hCoil == COIL_OCCUPATION );
		if( nCol < 0 || (UINT)pLine[ nCol ].hCoil < COIL_BASE ) return (CBaseCoil*)COIL_ERROR;
		pCoil = pLine[ nCol ].hCoil;
		if ( pColStart ) *pColStart = nCol;
		if ( pColEnd ) *pColEnd = nCol + pCoil->GetColOccupation() - 1;
	}else
	{
		if ( pColStart ) *pColStart = pos.nCol;
		if ( pColEnd ) *pColEnd = pos.nCol;
	}
	return pCoil;
}
UINT CCoilBlock::GetCoilType( CBaseCoil* pCoil )
{
	if( (UINT)pCoil >= COIL_BASE )
		return pCoil->GetType();
	else
		return (UINT)pCoil;
}
CBaseCoil* CCoilBlock::GetCoil( CoilPos& pos )
{
	LPCoilLine pLine = GetLine( pos.nLn );
	if ( pLine )
	{
		if( pos.nCol < 0 || pos.nCol > m_szBlock.nCol )
			return (CBaseCoil*)COIL_ERROR;
		else
			return pLine[ pos.nCol ].hCoil;
	}else
		return (CBaseCoil*)COIL_ERROR;
}

CBaseCoil* CCoilBlock::GetCoil(  LPCoilLine pLine, int nCol  )
{
	if( pLine )
	{
		if( nCol < 0 || nCol > m_szBlock.nCol )
			return (CBaseCoil*)COIL_ERROR;
		else
			return pLine[ nCol ].hCoil;
	}
	return (CBaseCoil*)COIL_ERROR;
}
LPCoilPoint CCoilBlock::GetCoilPoint( CoilPos& pos )
{
	LPCoilLine pLine = GetLine( pos.nLn );
	if( pos.nCol < 0 || pos.nCol > m_szBlock.nCol )
		return (LPCoilPoint)NULL;
	return ( pLine + pos.nCol );
}
int CCoilBlock::GetCoilOccupation( CBaseCoil* pCoil )
{
	if( (UINT)pCoil >= COIL_BASE )
		return pCoil->GetColOccupation();
	else
		return 1;
}
BOOL CCoilBlock::SetErrorPos( CoilPos& pos )
{
	m_posErr = pos;
	return -1;
}
CoilPos CCoilBlock::SetPos( CoilPos& pos )
{
	CoilPos posOld = m_ILine.pos;
	m_ILine.pos = pos;
	return posOld;
}
//向后必须有足够空间，否则失败,返回插入的Coil的大小
int CCoilBlock::_InsertCoil( CBaseCoil* pCoil, CoilPos &pos )
{
	int nOccupation;
	int ntmp;
	BOOL bRes = FALSE;
	LPCoilLine pLine;
	int npos;
	CBaseCoil* origin;

	//开始插入
	pLine = UpdateLine( pos.nLn );
	if( pLine == NULL ) return 0;
	origin = pLine[ pos.nCol ].hCoil;
	if( (UINT)origin >= COIL_BASE )		//必须没有Coil
		return FALSE;
	if( (UINT)pCoil >= COIL_BASE )
	{
		nOccupation = pCoil->GetColOccupation();
	}else
		nOccupation = 1;
	if( nOccupation > 1 )	//还需要空间
	{
		//向后查找空间
		npos = pos.nCol;
		ntmp = nOccupation;
		while( --ntmp && npos < m_szBlock.nCol && ( (UINT)( pLine[npos].hCoil ) < COIL_BASE ) )++npos;
		if( ntmp != 0 )	//没有足够空间
			return -ntmp;
	}
	pLine[pos.nCol].hCoil = pCoil;
	npos = pos.nCol;
	ntmp = nOccupation;
	while( --ntmp )pLine[++npos].hCoil = (CBaseCoil*)COIL_OCCUPATION;
	pos.nCol += nOccupation;
	return nOccupation;
}

void CCoilBlock::Ins2LadReset()
{
	m_nLink = 0;
	memset( &m_ILine, -1, sizeof( InputLine ) );
	m_vILine.clear();
}
BOOL CCoilBlock::LDCoil( CBaseCoil* pCoil )
{
	m_vILine.push_back( m_ILine );
	m_ILine.nStart = m_ILine.nMaxRow + 1;
	m_ILine.nMaxRow = m_ILine.nStart;
	m_ILine.pos.nCol = m_nMatherLine;
	m_ILine.pos.nLn = m_ILine.nStart;
	return _InsertCoil( pCoil, m_ILine.pos );
}

BOOL CCoilBlock::AndCoil( CBaseCoil* pCoil )
{
	//return ( LDCoil( pCoil ) && AndBlock() );
	int nRes = 0;
	CoilPos posOut;
	posOut = m_ILine.pos;
	if( (nRes = _InsertCoil( pCoil, m_ILine.pos )) > 0 )
	{
		posOut.nCol--;
		LPCoilPoint pCoilPt;
		if( ( pCoilPt = GetCoilPoint(posOut) ) && pCoilPt->hCoil == NULL )		//为空需要画垂线
		{
			posOut.nCol++;
			posOut.nLn--;
			while( GetCoil(posOut) == COIL_NULL ){
				AddLineCoil( COIL_VLINE, posOut );
				posOut.nLn--;
			}
			AddLineCoil( COIL_VLINE, posOut );
		}
	}
	return nRes;
}

BOOL CCoilBlock::AndBlock()
{
	//将最后一条输入线移到上一条输入性上
	int nAndpos;
	int nAndCol;
	InputLine iLine;
	int nRowBackup;
	CoilPos posOut;
	LPCoilLine pLine;
	LPCoilLine pAndLine;

	if( m_vILine.size() < 2 ) return FALSE;
	iLine = m_ILine;//m_vILine.back();
	m_ILine = m_vILine.back();	//将当前输入线改到上一条
	m_vILine.pop_back();
	posOut = m_ILine.pos;
	nRowBackup = m_ILine.pos.nLn;
	nAndpos = m_ILine.pos.nCol;
	nAndCol = iLine.pos.nCol;
	for ( iLine.pos.nLn = iLine.nStart; iLine.pos.nLn <= iLine.nMaxRow; iLine.pos.nLn++,m_ILine.pos.nLn++ )
	{
		m_ILine.pos.nCol = nAndpos;
		iLine.pos.nCol = m_nMatherLine;
		pLine = GetLine( m_ILine.pos.nLn );
		pAndLine = GetLine( iLine.pos.nLn );
		while( iLine.pos.nCol <= nAndCol )
		{
			pLine[ m_ILine.pos.nCol ] = pAndLine[ iLine.pos.nCol ];
			pAndLine[ iLine.pos.nCol ].hCoil = NULL;
			pAndLine[ iLine.pos.nCol ].vCoil = NULL;
			++iLine.pos.nCol;
			++m_ILine.pos.nCol;
		}
	}
	posOut.nCol--;
	LPCoilPoint pCoilPt;
	if( ( pCoilPt = GetCoilPoint(posOut) ) && pCoilPt->hCoil == NULL )		//为空需要画垂线
	{
		posOut.nCol++;
		posOut.nLn--;
		while( GetCoil(posOut) == COIL_NULL ){
			AddLineCoil( COIL_VLINE, posOut );
			posOut.nLn--;
		}
		AddLineCoil( COIL_VLINE, posOut );
	}
	--m_ILine.pos.nCol;
	--m_ILine.pos.nLn;
	if( m_ILine.pos.nLn > m_ILine.nMaxRow )
		m_ILine.nMaxRow = m_ILine.pos.nLn;
	m_ILine.pos.nLn = nRowBackup;
	return TRUE;
}

BOOL CCoilBlock::OrBlock()
{
	//只要将两条输出线用垂线连接就行了
	InputLine iLine;
	CoilPos posLine;

	if( m_vILine.size() < 2 ) return FALSE;
	iLine = m_ILine;//m_vILine.back();
	m_ILine = m_vILine.back();	//将当前输入线改到上一条
	m_vILine.pop_back();

	if( iLine.pos.nCol > m_ILine.pos.nCol )
	{
		posLine = m_ILine.pos;
		while( posLine.nCol < iLine.pos.nCol ){
			AddLineCoil( COIL_HLINE, posLine );
			posLine.nCol++;
		}
		m_ILine.pos.nCol = iLine.pos.nCol;
	}else
	{
		posLine = iLine.pos;
		while( posLine.nCol < m_ILine.pos.nCol ){
			AddLineCoil( COIL_HLINE, posLine );
			posLine.nCol++;
		}
	}

	for ( posLine.nLn = m_ILine.nStart; posLine.nLn < iLine.pos.nLn; posLine.nLn++ )
	{
		posLine.nCol = m_nMatherLine;
		AddLineCoil( COIL_VLINE, posLine );
		posLine.nCol = m_ILine.pos.nCol;
		AddLineCoil( COIL_VLINE, posLine );
	}
	m_ILine.nMaxRow = iLine.nMaxRow;
	return TRUE;
}

//OR不改变pos
BOOL CCoilBlock::OrCoil( CBaseCoil* pCoil )
{
	return ( LDCoil( pCoil ) && OrBlock() );
}

BOOL CCoilBlock::OutCoil( CBaseCoil* pCoil )
{
	CoilPos posOut;
	int nOutCol;

	if( m_vILine.size() > 2 )	//出错
		return FALSE;
	else if( m_vILine.size() == 2 )
		m_vILine.pop_back();
	posOut.nLn = m_ILine.pos.nLn;
	posOut.nCol = m_nOutputLine - GetCoilOccupation(pCoil);
	if( _InsertCoil( pCoil,posOut ) )
	{
		posOut = m_ILine.pos;
		posOut.nCol--;
		LPCoilPoint pCoilPt;
		if( posOut.nCol >= m_nMatherLine && ( pCoilPt = GetCoilPoint(posOut) ) && pCoilPt->hCoil == NULL )			
		{	//为空需要画垂线
			posOut.nCol++;
			posOut.nLn--;
			while( GetCoil(posOut) == COIL_NULL ){
				AddLineCoil( COIL_VLINE, posOut );
				posOut.nLn--;
			}
			AddLineCoil( COIL_VLINE, posOut );
		}
		posOut = m_ILine.pos;
		nOutCol = m_nOutputLine - GetCoilOccupation(pCoil);
		while( posOut.nCol < nOutCol )
		{
			AddLineCoil( COIL_HLINE, posOut );
			posOut.nCol++;
		}
		while( ++posOut.nCol < m_nOutputLine )
		{
			AddLineCoil( COIL_OCCUPATION, posOut );
		}
		if( m_ILine.pos.nLn > m_ILine.nMaxRow )
			m_ILine.nMaxRow = m_ILine.pos.nLn;
		m_ILine.pos.nLn++;
		return TRUE;
	}
	return FALSE;
}
BOOL CCoilBlock::OutLine( CBaseCoil* pCoil )
{
	m_vILine.push_back( m_ILine );
	m_ILine.nStart = m_ILine.nMaxRow + 1;
	m_ILine.nMaxRow = m_ILine.nStart;
	m_ILine.pos.nCol = m_nMatherLine;
	m_ILine.pos.nLn = m_ILine.nStart;
	return OutCoil( pCoil );
}
void CCoilBlock::MPP( int nCol )	//用于mpp，mrd
{
	m_ILine.pos.nLn = m_ILine.nMaxRow;
	m_ILine.pos.nCol = m_nOutputLine - 1;
	if( GetLine( m_ILine.pos.nLn ) != NULL && GetCoil( m_ILine.pos ) != COIL_NULL )
		m_ILine.pos.nLn++;
	m_ILine.pos.nCol = nCol;
}
BOOL CCoilBlock::P( CBaseCoil* pCoil )
{
	return InsertLable( pCoil, m_ILine.nMaxRow + 1 );
}
BOOL CCoilBlock::End()
{
	return FALSE;
}


BOOL CCoilBlock::DeleteCoil( CoilPos pos )
{
	CBaseCoil* pCoil;
	int nStart,nEnd;
	pCoil = FindCoil( pos, &nStart,&nEnd );
	if( (UINT)pCoil != COIL_ERROR )
	{
		LPCoilLine pLine = GetLine(pos.nLn);
		if( (UINT)pCoil >= COIL_BASE )
			delete pCoil;
		for ( ; nStart <= nEnd; nStart++ )
			pLine[nStart].hCoil = COIL_NULL;
		return TRUE;
	}else
		return FALSE;
}

//注意：只是做简单拷贝，没有 new coil
int CCoilBlock::CopyLine( LPCoilLine pLineCopy, int sz )
{
	int nMinsz;
	if( pLineCopy == NULL ) return -1;
	m_ILine.nMaxRow++;
	nMinsz = ( ( sz > m_szBlock.nCol ) ? m_szBlock.nCol : sz );
	LPCoilLine pLine = UpdateLine( m_ILine.nMaxRow );
	for( int i = 0; i < nMinsz; i++ )
		pLine[i] = pLineCopy[i];
	return nMinsz;
}
void CCoilBlock::EmptyCoilIns()
{
	LPCoilPoint coilpt,coilptend;
	vector<LPCoilLine>::iterator iter;
	for( iter = m_vCoil.begin(); iter != m_vCoil.end(); iter++ )
	{
		coilpt = *iter;
		coilptend = coilpt + m_nOutputLine;
		while( coilpt < coilptend )
		{
			if( coilpt->hCoil > (CBaseCoil*)COIL_BASE )
				coilpt->hCoil->m_strIns.Empty();
			if( coilpt->vCoil > (CBaseCoil*)COIL_BASE )
				coilpt->vCoil->m_strIns.Empty();
			coilpt++;
		}
	}
}
BOOL CCoilBlock::InputLine2Ins( int nLn, CStdString& strIns )
{
	CoilPos pos;
	BOOL bRes;
	if (nLn < 0 )
		return FALSE;
	CCoilBlock block( m_szBlock.nLn, m_szBlock.nCol - 1, 1 );
	block.m_ILine.nStart = 0;
	pos.nLn = 0;
	pos.nCol = m_nMatherLine;
	int nNextLn = GetNextInputLine( nLn );
	if( nNextLn == -1 )
		nNextLn = GetMaxRow() + 1;
	while( nLn < nNextLn ) 
		block.CopyLine( GetLine(nLn++), m_szBlock.nCol );
	block.EmptyCoilIns();
	while( ( bRes = block.IsMinBlock( pos ) ) == 0 )
	{
		if( ( bRes = block.ChangeToMinBlock() ) <= 0 )
			return bRes;
	}
	if( bRes < 0 ) return FALSE;
	if( !block.MinBlock2Ins( pos, strIns ) )
		return FALSE;
	//检测有没有出错,转换后必须是空块，不是的话则存在孤立节点
	if( !block.IsEmptyBlock() )	return FALSE;
	return TRUE;
}

//最简块只有一个输入和一个输出（）
//或者只有一个输出[]
BOOL CCoilBlock::IsMinBlock( CoilPos pos,BOOL bVLineHasDeal )
{
	LPCoilLine pLine;
	BOOL bCoilGet = FALSE;
	if( ( pLine = GetLine( pos.nLn ) ) == NULL ) return SetErrorPos(pos);
	if( !bVLineHasDeal && pLine[pos.nCol].vCoil )
		return FALSE;
	
	if( pLine[pos.nCol].hCoil < (CBaseCoil*)COIL_BASE )
	{
		++pos.nCol;
		while( pos.nCol < m_nOutputLine 
			&& pLine[pos.nCol].hCoil == (CBaseCoil*)COIL_HLINE 
			&& pLine[pos.nCol].vCoil == (CBaseCoil*)COIL_NULL )
			++pos.nCol;
		if( pos.nCol >= m_nOutputLine 
			|| pLine[pos.nCol].vCoil != (CBaseCoil*)COIL_NULL )	return SetErrorPos( pos );
		else if( pLine[pos.nCol].hCoil && pLine[pos.nCol].hCoil->GetType() == COIL_OUTPUT )
		{
			if( ( (COutputCoil*)pLine[pos.nCol].hCoil )->GetOutputType() != COutputCoil::eFnc )
				return SetErrorPos( pos );
			else	//只有一个输出[]
				return TRUE;
		}
	}
	++pos.nCol;
	while( pos.nCol < m_nOutputLine 
		&& pLine[pos.nCol].hCoil == (CBaseCoil*)COIL_HLINE 
		&& pLine[pos.nCol].vCoil == (CBaseCoil*)COIL_NULL )
		++pos.nCol;
	if( pLine[pos.nCol].vCoil != (CBaseCoil*)COIL_NULL )
	{
		//有垂线，需要测试垂线输出
		//检测垂直输出
		BOOL bRes;
		vector<OutPutInfo> vOutPut;
		vector<OutPutInfo>::iterator iter;
		//++pos.nCol;
		if( ( bRes = GetOutputInfo( pos, vOutPut ) ) > 0 /*&& vOutPut.size() > 1*/)
		{
			for ( iter = vOutPut.begin(); iter != vOutPut.end(); iter++)
			{
				if( !( *iter ).isDirect && ( bRes = IsMinBlock( ( *iter ).pos,TRUE ) ) <= 0 )
					return bRes;
			}
		}else
			return bRes;
	}else
	{
		//没有垂线，接下一个必须是输出
		if( GetCoilType( pLine[pos.nCol].hCoil ) != COIL_OUTPUT )
			return FALSE;
	}
	return TRUE;
}
BOOL CCoilBlock::GetOutputInfo( CoilPos pos,vector<OutPutInfo>& vOutPut )
{
	CoilPos postest;
	OutPutInfo info;
	info.pos = pos;
	vOutPut.clear();
	LPCoilPoint pOutpt;
	LPCoilPoint pCoilpt;
	int nCol;
	
	pCoilpt = GetLine( info.pos.nLn );
	if( !pCoilpt )
		return -1;
	pCoilpt += info.pos.nCol;
	while( info.pos.nCol < m_nOutputLine )
	{
		if( pCoilpt->vCoil == (CBaseCoil*)COIL_VLINE )
		{
			nCol = info.pos.nCol;
			do 
			{
				while( ( pOutpt = GetLine(++info.pos.nLn) ) 
					&& pOutpt[info.pos.nCol].vCoil == (CBaseCoil*)COIL_VLINE 
					&& pOutpt[info.pos.nCol].hCoil == (CBaseCoil*)COIL_NULL  );
				if( !pOutpt 
					|| pOutpt[info.pos.nCol].hCoil == (CBaseCoil*)COIL_NULL )
					return SetErrorPos(info.pos);
				postest.nLn = info.pos.nLn;
				postest.nCol = info.pos.nCol + 1;
				if( !IsInputLine( postest ) )//检测是不是为输入线，即在这一条线上必须没有上连接
					return SetErrorPos( postest );
				pOutpt += info.pos.nCol;
				while( pOutpt->hCoil < (CBaseCoil*)COIL_BASE ) ++pOutpt, ++info.pos.nCol;
				if( pOutpt->hCoil == COIL_NULL )	//为空出错
					return SetErrorPos(info.pos);
				info.isDirect = ( GetCoilType( pOutpt->hCoil ) == COIL_OUTPUT );
				vOutPut.push_back( info );
				info.pos.nCol = nCol;
			} while( ( pOutpt = GetLine(info.pos.nLn) )  && pOutpt[info.pos.nCol].vCoil == (CBaseCoil*)COIL_VLINE );
		}
		if( pCoilpt->hCoil != (CBaseCoil*)COIL_HLINE )
			break;
		++info.pos.nCol;
		++pCoilpt;
	}
	//处理第一行
	info.pos = pos;
	pOutpt = GetLine(info.pos.nLn);
	pOutpt += pos.nCol;
	while( pOutpt->hCoil < (CBaseCoil*)COIL_BASE ) ++pOutpt, ++info.pos.nCol;
	info.isDirect = ( GetCoilType( pOutpt->hCoil ) == COIL_OUTPUT );
	vOutPut.insert( vOutPut.begin(),info );
	return TRUE;
}
BOOL CCoilBlock::IsInputLine( CoilPos& pos )
{
	LPCoilPoint pCoilpt = GetLine( pos.nLn - 1 );
	if( pCoilpt )
	{
		pCoilpt += pos.nCol;
		while( pos.nCol < m_nOutputLine && pCoilpt->vCoil == COIL_NULL ) ++pCoilpt,++pos.nCol;
		return ( pos.nCol == m_nOutputLine );
	}else
		return TRUE;
}
LPCTSTR CCoilBlock::FormatInputType( CBaseCoil* pCoil )
{
	if( pCoil->GetType() != COIL_INPUT )
		return NULL;
	switch( ( (CInputCoil*)pCoil )->GetInputType() )
	{
	case CInputCoil::eUpPluse:		return _T("P");
	case CInputCoil::eDownPluse:	return _T("F");	
	case CInputCoil::eClose:		return _T("I");
	case CInputCoil::eOpen:			return _T("");
	}
	return NULL;
}
BOOL CCoilBlock::ChangeToMinBlock()
{
	LPCTSTR strIType;
	BOOL bRes;
	CoilPos pos;
	CoilPos posor;
	int nMidCol;
	vector<CoilInfo> vCoil;
	CBaseCoil* pFirstCoil;
	CBaseCoil* pOrCoil;
	CoilInfo* pCoil;
	int n;
	BOOL bORB;
	//先or
	for ( pos.nLn = 0; pos.nLn < GetMaxRow(); pos.nLn++ )
	{
		for ( pos.nCol = m_nMatherLine; pos.nCol < m_nOutputLine; )
		{
			while ( ( bRes = GetMinOr( pos, posor, vCoil,nMidCol ) ) > 0 )
			{
				//处理上行
				n = 0;
				pFirstCoil = vCoil[n++].pCoil;
				if( pFirstCoil->m_strIns.IsEmpty() )
					pFirstCoil->m_strIns.Format(_T("LD %s\n"), pFirstCoil->GetName() );
				while( n < nMidCol )
				{
					pCoil = &vCoil[n++];
					if( pCoil->pCoil->m_strIns.IsEmpty() )
					{
						if( ( strIType = FormatInputType( pCoil->pCoil ) ) == NULL )
							return FALSE;
						pCoil->pCoil->m_strIns.Format(_T("AND%s %s\n"), strIType, pCoil->pCoil->GetName() );
						pFirstCoil->m_strIns += pCoil->pCoil->m_strIns;
					}
					else
						pFirstCoil->m_strIns += pCoil->pCoil->m_strIns + _T("ANB\n"); 
					GetCoilPoint( pCoil->pos )->hCoil = (CBaseCoil*)COIL_HLINE;
				}
				//处理下行
				pOrCoil = vCoil[n++].pCoil;
				bORB = !pOrCoil->m_strIns.IsEmpty();
				if( pOrCoil->m_strIns.IsEmpty() )
				{
					if( ( strIType = FormatInputType( pOrCoil ) ) == NULL )
						return FALSE;
					if( nMidCol + 1 == vCoil.size() )
					{
						pOrCoil->m_strIns.Format(_T("OR%s %s\n"), strIType, pOrCoil->GetName() );
					}else
					{
						pOrCoil->m_strIns.Format(_T("LD%s %s\n"), strIType,pOrCoil->GetName() );
					}
				}
				while( n < (int)vCoil.size() )
				{
					pCoil = &vCoil[n++];
					if( pCoil->pCoil->m_strIns.IsEmpty() )
					{
						if( ( strIType = FormatInputType( pOrCoil ) ) == NULL )
							return FALSE;
						pCoil->pCoil->m_strIns.Format(_T("AND%s %s\n"), strIType,pCoil->pCoil->GetName() );
						pOrCoil->m_strIns += pCoil->pCoil->m_strIns;
					}
					else
						pOrCoil->m_strIns += pCoil->pCoil->m_strIns + _T("ANB\n"); 
				}
				pFirstCoil->m_strIns += pOrCoil->m_strIns;
				if( nMidCol + 1 < (int)vCoil.size() )
					pFirstCoil->m_strIns +=_T("ORB\n"); 
				else if( bORB )
					pFirstCoil->m_strIns +=_T("ORB\n"); 
				//删除线
				CoilPoint* pCoilpt;
				CoilPos posLine;
				CoilPos posTest;
				posLine.nLn = posor.nLn;
				posLine.nCol = pos.nCol;
				pCoilpt = GetCoilPoint(posLine);
				while ( posLine.nCol < posor.nCol )
				{
					GetCoilPoint( posLine )->hCoil = COIL_NULL;
					posLine.nCol++;
					pCoilpt++;
				}
				while( --posLine.nLn >= pos.nLn )
				{
					posTest.nLn = posLine.nLn + 1;
					posTest.nCol = posLine.nCol;
					if( ( pCoilpt = GetCoilPoint(posTest) ) 
						&& ( pCoilpt->hCoil != NULL || pCoilpt->vCoil != NULL ) )
						continue;
					--posTest.nCol;
					if( ( pCoilpt = GetCoilPoint(posTest) ) && pCoilpt->hCoil != NULL )
						continue;
					DeleteLineCoil( posLine, COIL_VLINE );
				}
				posLine.nCol = pos.nCol;
				posLine.nLn = posor.nLn;
				while( --posLine.nLn >= pos.nLn )
				{
					posTest.nLn = posLine.nLn + 1;
					posTest.nCol = posLine.nCol;
					if( ( pCoilpt = GetCoilPoint(posTest) ) 
						&& ( pCoilpt->hCoil != NULL || pCoilpt->vCoil != NULL ) )
						continue;
					--posTest.nCol;
					if( ( pCoilpt = GetCoilPoint(posTest) ) && pCoilpt->hCoil != NULL )
						continue;
					DeleteLineCoil( posLine, COIL_VLINE );
				}
			}
			if( bRes < 0 )
				return bRes;
			else if( bRes )
				pos.nCol = posor.nCol;
			else
				pos.nCol++;
		}
	}
	//再and
	CoilInfo info;
	LPCoilPoint coilpt;
	LPCoilPoint coilptPre;
	vCoil.clear();
	for ( info.pos.nLn = 0; info.pos.nLn <= GetMaxRow(); info.pos.nLn++ )
	{
		coilptPre = GetLine( info.pos.nLn - 1 );
		coilpt = GetLine( info.pos.nLn );
		coilpt += m_nMatherLine;
		if( coilptPre ) coilptPre += m_nMatherLine;
		for ( info.pos.nCol = m_nMatherLine; info.pos.nCol < m_nOutputLine;info.pos.nCol++ )
		{
			if( coilpt->vCoil != COIL_NULL 
				|| ( coilptPre && coilptPre->vCoil != NULL )
				|| GetCoilType( coilpt->hCoil ) == COIL_OUTPUT )
			{
				if( vCoil.size() > 1 )
				{
					n = 0;
					pCoil = &vCoil[n++];
					pFirstCoil = pCoil->pCoil;
					if( pFirstCoil->m_strIns.IsEmpty() )
					{
						if( ( strIType = FormatInputType( pFirstCoil ) ) == NULL )
							return FALSE;
						if( pCoil->pos.nCol == m_nMatherLine )
						{
							pFirstCoil->m_strIns.Format(_T("LD%s %s\n"), strIType,pFirstCoil->GetName() );
						}else
						{
							pFirstCoil->m_strIns.Format(_T("AND%s %s\n"), strIType,pFirstCoil->GetName() );
						}
					}
					while( n < vCoil.size() )
					{
						pCoil = &vCoil[n++];
						if( pCoil->pCoil->m_strIns.IsEmpty() )
						{
							if( ( strIType = FormatInputType( pCoil->pCoil ) ) == NULL )
								return FALSE;
							pCoil->pCoil->m_strIns.Format(_T("AND%s %s\n"), strIType, pCoil->pCoil->GetName() );
							pFirstCoil->m_strIns += pCoil->pCoil->m_strIns;
						}else
						{
							pFirstCoil->m_strIns += pCoil->pCoil->m_strIns;
							pFirstCoil->m_strIns += _T("ANB\n");
						}
						GetCoilPoint( pCoil->pos )->hCoil = (CBaseCoil*)COIL_HLINE;
					}
				}
				vCoil.clear();
			}
			if( GetCoilType( coilpt->hCoil ) == COIL_OUTPUT )
				break;
			else if( coilpt->hCoil >= ( CBaseCoil* )COIL_BASE )
			{
				info.pCoil = coilpt->hCoil;
				vCoil.push_back( info );
			}
			++coilpt;
			if ( coilptPre ) ++coilptPre;
		}
	}
	return TRUE;
}
BOOL CCoilBlock::GetMinOr( CoilPos pos, CoilPos& posor,vector<CoilInfo>& vCoil, int& nMidCol )
{
	CoilInfo info;
	LPCoilPoint pCoilpt;
	pCoilpt = GetCoilPoint( pos );
	if( pCoilpt->hCoil == NULL || pCoilpt->vCoil == NULL )
		return FALSE;
	vCoil.clear();
	posor = pos;
	//第一列向下
	++pos.nLn;
	while( (pCoilpt = GetCoilPoint( pos )) && pCoilpt->vCoil == (CBaseCoil*)COIL_VLINE && pCoilpt->hCoil == (CBaseCoil*)COIL_NULL) ++pos.nLn;
	if( pCoilpt == NULL || pCoilpt->hCoil == (CBaseCoil*)COIL_NULL ) return FALSE;
	//第一行向左
	LPCoilLine pLine, pPreLine;
	pPreLine = GetLine( posor.nLn - 1 );
	pLine = GetLine( posor.nLn );
	if( pLine[ posor.nCol ].hCoil >= (CBaseCoil*)COIL_BASE )
	{
		info.pCoil = pLine[ posor.nCol ].hCoil;
		info.pos = posor;
		vCoil.push_back( info );
	}
	++posor.nCol;
	while( pLine[ posor.nCol ].vCoil == NULL )
	{
		if( pPreLine && pPreLine[ posor.nCol ].vCoil != NULL )
			break;
		if( pLine[ posor.nCol ].hCoil >= (CBaseCoil*)COIL_BASE )
		{
			info.pCoil = pLine[ posor.nCol ].hCoil;
			info.pos = posor;
			if( GetCoilType( info.pCoil ) == COIL_OUTPUT )
				return FALSE;
			vCoil.push_back( info );
		}
		++posor.nCol;
	}
	if( ( nMidCol = vCoil.size() ) == 0 )
		return SetErrorPos(posor);
	//最后一列向下
	++posor.nLn;
	while( (pLine = GetLine( posor.nLn )) && pLine[ posor.nCol ].vCoil == (CBaseCoil*)COIL_VLINE && pLine[ posor.nCol - 1 ].hCoil == (CBaseCoil*)COIL_NULL) ++posor.nLn;
	if( pLine == NULL || pLine[ posor.nCol - 1 ].hCoil == (CBaseCoil*)COIL_NULL ) return FALSE;
	//如果pos.nLn 和posor.nLn 不同
	if( pos.nLn != posor.nLn ) return FALSE;
	//最后一行向左
	pPreLine = GetLine( pos.nLn - 1 );
	pLine = GetLine( pos.nLn );
	if( pLine[ pos.nCol ].hCoil >= (CBaseCoil*)COIL_BASE )
	{
		info.pCoil = pLine[ pos.nCol ].hCoil;
		info.pos = pos;
		vCoil.push_back( info );
	}
	++pos.nCol;
	while( pLine[ pos.nCol ].vCoil == NULL )
	{
		if( pPreLine && pPreLine[ pos.nCol ].vCoil != NULL )
			break;
		if( pLine[ pos.nCol ].hCoil >= (CBaseCoil*)COIL_BASE )
		{
			info.pCoil = pLine[ pos.nCol ].hCoil;
			info.pos = pos;
			vCoil.push_back( info );
		}
		++pos.nCol;
	}
	if( nMidCol == vCoil.size())
		return SetErrorPos(pos);
	if( pos.nCol != posor.nCol )
		return FALSE;
	return TRUE;
}
BOOL CCoilBlock::MinBlock2Ins( CoilPos pos,CStdString& strOut )
{
	typedef enum eMPState{
		eMPS,
		eMRD,
		eMPP,
	};
	int nBeginCol = pos.nCol;
	eMPState esLast = eMPP;
	int nOutCol = 0;
	LPCoilPoint coilpt;
	CStdString strTmp;
	LPCTSTR strIType;
	CBaseCoil* pCoil;
	//先处理lable
	if( ( pCoil = GetLine(pos.nLn)[0].hCoil ) != NULL && pos.nCol == m_nMatherLine )
	{
		strOut += pCoil->GetName();
		strOut += _T("\n");
	}
	//跳过水平线
	coilpt = GetCoilPoint( pos );
	while( coilpt->hCoil == (CBaseCoil*)COIL_HLINE ) ++coilpt,++pos.nCol;
	if( nBeginCol == m_nMatherLine && coilpt->hCoil->GetType() == COIL_OUTPUT )//只有一个输出[]
	{
		strOut += coilpt->hCoil->GetName();
		strOut += _T("\n");
		return TRUE;
	}
	if( coilpt->hCoil->m_strIns.IsEmpty() )
	{
		if( ( strIType = FormatInputType( coilpt->hCoil ) ) == NULL )
			return FALSE;
		if( pos.nCol == m_nMatherLine )
		{
			strTmp.Format( _T("LD%s %s\n"), strIType,coilpt->hCoil->GetName() );
		}else
		{
			strTmp.Format( _T("AND%s %s\n"), strIType,coilpt->hCoil->GetName() );
		}
		strOut += strTmp;
	}else
	{
		strOut += coilpt->hCoil->m_strIns;
		if( nBeginCol != m_nMatherLine )
			strOut += _T("ANB\n");
	}
	//检测输出
	BOOL bRes;
	BOOL bLastDirect;
	vector<OutPutInfo> vOutPut;
	vector<OutPutInfo>::iterator iter,itertmp;
	pos.nCol++;
	if( ( bRes = GetOutputInfo( pos, vOutPut ) ) > 0 )
	{
		for ( iter = vOutPut.begin(); iter != vOutPut.end(); iter++)
		{
			if( esLast == eMPP )
			{
				if( !(*iter).isDirect )
				{
					for ( itertmp = iter; itertmp != vOutPut.end(); itertmp++ )
						if( !(*itertmp).isDirect )
							break;
					if( itertmp != vOutPut.end() )	//需要弹出
					{
						esLast = eMPS;
						strOut += _T("MPS\n");
					}
				}
			}else// if( esLast == eMRD || esLast == eMPS )
			{
				for ( itertmp = iter; itertmp != vOutPut.end(); itertmp++ )
					if( !(*itertmp).isDirect )
						break;
				if( itertmp == vOutPut.end() )	//后面不再需要弹出
				{
					strOut += _T("MPP\n");
					esLast = eMPP;
				}else
				{
					if( !bLastDirect )
					{
						esLast = eMRD;
						strOut += _T("MRD\n");
					}
				}
			}
			if( ( *iter ).isDirect )
			{
				pCoil = GetCoil( ( *iter ).pos );
				if( ( (COutputCoil*)pCoil )->GetOutputType() == COutputCoil::eFnc )
				{
					strOut += pCoil->GetName();
				}else
				{
					pCoil->m_strIns.Format( _T("OUT %s "), pCoil->GetName() );
					strOut += pCoil->m_strIns;
					if( ( (COutputCoil*)pCoil )->IsHaveArg() )
						strOut += ( (COutputCoil*)pCoil )->GetArg();
				}
				strOut += _T("\n");
			}else
			{
				if( ( bRes = MinBlock2Ins( ( *iter ).pos,strOut ) ) <= 0 )
					return bRes;
			}
			bLastDirect = (*iter).isDirect;
		}
	}else
		return bRes;
	return TRUE;
}

BOOL CCoilBlock::IsEmptyBlock()
{

	return TRUE;
}
BOOL CCoilBlock::AdjustConnection()
{
	int nLn;
	int nLnVLine,nLnBegin;
	int nNextLn;
	//连接母线的垂线，如果需要的话.方便后面转换
	if( ( nLn = GetFirstInputLine() ) == -1 )	return FALSE;
	do{
		nLnBegin = nLn;
		nLnVLine = nLn;
		nNextLn = GetNextInputLine( nLn );
		if( nNextLn == -1 )
			nNextLn = GetMaxRow() + 1;
		++nLn;
		while( nLn < nNextLn )
		{
			if( m_vCoil[ nLn ][ m_nMatherLine ].hCoil )
				nLnVLine = nLn;
				//m_vCoil[ nLn - 1 ][ m_nMatherLine ].vCoil = (CBaseCoil*)COIL_VLINE;
			nLn++;
		}
		while( nLnBegin < nLnVLine )
			m_vCoil[ nLnBegin++ ][ m_nMatherLine ].vCoil = (CBaseCoil*)COIL_VLINE;
		
	}while( nLn < GetMaxRow() + 1 );

	//下面只做简单的连接测试，检测coil是否为孤立coil，如果存在孤立coil出错

	return TRUE;
}

int CCoilBlock::GetFirstInputLine()
{
	return GetNextInputLine(-1);
}
int CCoilBlock::GetNextInputLine( int nln )
{
	LPCoilLine pLine,pPreLine;
	CoilPos pos;
	pos.nLn = nln + 1;
	pos.nCol = m_nMatherLine;
	while( pos.nLn <= GetMaxRow() )
	{
		pPreLine = GetLine( pos.nLn - 1);
		pLine = GetLine( pos.nLn );
		for ( pos.nCol = m_nMatherLine; pos.nCol < m_nOutputLine; pos.nCol++ )
		{
			if( pLine[ pos.nCol ].hCoil == NULL ||
				( pPreLine && pPreLine[pos.nCol].vCoil != NULL ) )
				break;
		}
		if( pos.nCol == m_nOutputLine )
			return pos.nLn;
		++pos.nLn;
	}
	return -1;
}
