#include "plcSchematic\Schematic.h"
#include "plcInsSys\plcInsSys.h"
#include "common\misc.h"

#ifdef _DEBUG
#include "..\..\project\Leakchecker\Leakchecker.h"
#define new DEBUG_NEW
#endif

using namespace libSch;

CSchematic::CSchematic(void):
m_nPosCol(0),m_nPosRow(0),
m_bReDraw(TRUE)
{
	BlockSize szBlock;
	szBlock.nLn = 8000;//3000行
	szBlock.nCol = 14;
	m_cBlock.SetMaxSize( &szBlock );
	m_szMax.cx = GetSystemMetrics(SM_CXSCREEN) - 200;
	m_nColLen = (int)( (double)m_szMax.cx / szBlock.nCol );
	m_szMax.cx = m_nColLen * szBlock.nCol;
	m_nRowLen = m_nColLen + m_nColLen;
	m_szMax.cy = m_nRowLen * szBlock.nLn;
	m_eDUType = CBaseCoil::eDrawUnitNormal;
	CBaseCoil::PrepareDrawInfo( &m_infoDraw, m_nRowLen, m_nColLen,m_eDUType );

	//测试1
	/*
	TCHAR str[20];
	CCoilBlock* pBlock = new CCoilBlock( m_szBlock.nLn, m_szBlock.nCol );
	m_vBlock.push_back( pBlock );
	CBaseCoil* pcoil;
	COutputCoil* pOutput;
	CNoteCoil* pNote,*pNote2;
	CoilPos pos;
	for ( pos.nLn = 0; pos.nLn < 100; pos.nLn++ )
	{ 
		for ( pos.nCol = 1; pos.nCol < 10; pos.nCol+=3 )
		{
			//pcoil = new CInputCoil( CInputCoil::eClose );
			//_stprintf( str,"%d,%d",nLn, nCol );
			//pcoil->SetName(str);
			//if( !AndCoil( pcoil, nLn, nCol ) )
			//	delete pcoil;
			pNote = new CNoteCoil("让我看看你啊！");
			pNote->SetName("hello");
			if( !pBlock->AndCoil( pNote,&pos ) )
				delete pNote;
			pOutput = new COutputCoil( COutputCoil::eFnc,"mov d800 d900",2 );
			//pOutput->SetFnc("mov");
			if( !pBlock->AndCoil( pOutput,&pos ) )
				delete pOutput;
			/ *pcoil = new CSymbolCoil( CSymbolCoil::eUpPluse );
			_stprintf( str,"%d,%d",nLn, nCol + 2 );
			pcoil->SetName(str);
			if( !pBlock->AndCoil( pcoil, nLn, nCol + 2 ) )
				delete pcoil;* /
		}
	}*/
	//测试3
	//BlockSize sz = { 10,10 };
	//SetMaxSize( &sz );
	//测试2
	/*if( FromIns( "F:\\WorkSpace\\VS\\sPLC\\PlcEditor\\plcsource.txt" ) )
	{
		;//ToIns( "F:\\WorkSpace\\VS\\sPLC\\PlcEditor\\plcbin.txt" );
	}*/
}


CSchematic::~CSchematic(void)
{
	CBaseCoil::DeleteDrawInfo( &m_infoDraw );
	ResetBlock();
}

void CSchematic::ResetBlock(void)
{
	vector<LPCoilLine>::iterator iter;
	BlockSize szBlock = m_cBlock.GetMaxSize();
	for( iter = m_cBlock.m_vCoil.begin(); iter != m_cBlock.m_vCoil.end(); iter++ )
	{
		CBaseCoil* pCoil;
		LPCoilLine pLine = *iter;
		for ( int i = 0; i < szBlock.nCol; i++ )
		{
			if( ( pCoil = pLine[i].hCoil ) && (UINT)pCoil >= COIL_BASE )
				delete pCoil;
			if( ( pCoil = pLine[i].vCoil ) && (UINT)pCoil >= COIL_BASE )
				delete pCoil;
		}
		delete pLine;
	}
	m_cBlock.m_vCoil.clear();
}

BOOL CSchematic::SetViewPos( int nLn, int nCol )
{
	if( nLn >= 0 && nLn <= m_cBlock.GetMaxRow() && nCol >= 0 && nCol < m_cBlock.GetMaxSize().nCol ) 
	{
		m_nPosRow = nLn;
		m_nPosCol = nCol;
		m_bReDraw = TRUE;
		return TRUE;
	}else
		return FALSE;
}
void CSchematic::GetViewPos( int *pnRow,int *pnCol )
{
	if( pnRow ) *pnRow = m_nPosRow;
	if( pnCol ) *pnCol = m_nPosCol;
}
void CSchematic::SetViewSize(int nx, int ny )
{
	int nNewWidth;		//根据宽度缩放
	if( nx < 420 ) nx = 420;	//最小ColLen=30 30 * 14
	nNewWidth = (int)( (double)nx / m_cBlock.m_nOutputLine );
	m_bReDraw = TRUE;
	m_nColLen = nNewWidth;
	m_nRowLen = m_nColLen + m_nColLen;
	m_nNumRow = ( ny + m_nRowLen - 1 ) / ( m_nRowLen );
	m_nNumCol = ( nx + m_nColLen - 1 ) / ( m_nColLen );
	m_szView.cx =  m_nColLen * m_nNumCol;
	m_szView.cy = m_nNumRow * m_nRowLen;
	CBaseCoil::DeleteDrawInfo( &m_infoDraw );
	CBaseCoil::PrepareDrawInfo( &m_infoDraw, m_nRowLen, m_nColLen,m_eDUType );
}
SIZE CSchematic::GetViewSize()
{
	return m_szView;
}

void CSchematic::Draw( HDC hdc, BOOL bForeDraw  )
{
	RECT rc;
	if( !bForeDraw && !m_bReDraw )
		return;
	m_bReDraw = FALSE;
	SetRect( &rc, 0, 0, m_szView.cx, m_szView.cy );
	FillRect( hdc,&rc, (HBRUSH)GetSysColor(COLOR_DESKTOP) );
	if( m_nPosCol <= m_cBlock.m_nMatherLine )
	{
		MoveToEx(hdc, m_nColLen * (m_cBlock.m_nMatherLine - m_nPosCol ),0,NULL);
		LineTo(hdc,m_nColLen * (m_cBlock.m_nMatherLine - m_nPosCol ),m_szView.cy );
	}
	if( m_nNumCol + m_nPosCol >= m_cBlock.m_nOutputLine )
	{
		MoveToEx(hdc,m_nColLen * (m_cBlock.m_nOutputLine - m_nPosCol ),0,NULL);
		LineTo(hdc,m_nColLen * (m_cBlock.m_nOutputLine - m_nPosCol ),m_szView.cy );
	}

	//画线圈
	vector<LPCoilLine>::iterator iter;
	LPCoilLine pLine;
	CBaseCoil* phCoil,*pvCoil;
	SIZE szOffset;	//线圈绘制偏移量
	int nPosFind;
	nPosFind = m_nPosRow - 1;
	if( nPosFind < 0 ) nPosFind = 0;
	else if( nPosFind > m_cBlock.m_vCoil.size() )
		return;
	szOffset.cy = ( nPosFind - m_nPosRow ) * m_nRowLen;
	if( m_cBlock.IsHaveNote( m_nPosRow - 1 ) )
		szOffset.cy -= m_infoDraw.nUnitLen;
	CBaseCoil::BeginPaint( hdc, &m_infoDraw );
	for ( iter = m_cBlock.m_vCoil.begin() + nPosFind; iter != m_cBlock.m_vCoil.end(); iter++,nPosFind++ )
	{
		pLine = *iter;
		if( m_cBlock.IsHaveNote( nPosFind + 1) )
			m_infoDraw.nHeightVline = m_infoDraw.nRowLen + m_infoDraw.nUnitLen;
		else
			m_infoDraw.nHeightVline = m_infoDraw.nRowLen;
		if( m_cBlock.IsHaveNote( nPosFind ) )
		{
			szOffset.cx = ( 1 - m_nPosCol ) * m_nColLen;
			m_cBlock.GetNoteCoil( nPosFind )->Draw( &m_infoDraw, szOffset );
			szOffset.cy += m_infoDraw.nUnitLen;
		}
		if( nPosFind  <= m_nPosRow + m_nNumRow )
		{
			for ( int i = m_nPosCol; i < m_cBlock.m_nOutputLine; )
			{
				szOffset.cx = ( i - m_nPosCol ) * m_nColLen;
				phCoil = pLine[i].hCoil;
				pvCoil = pLine[i].vCoil;
				if( (int)phCoil >= COIL_BASE )
				{
					phCoil->Draw( &m_infoDraw, szOffset );
					i += phCoil->GetColOccupation();
				}
				else	//不用判断就能直接绘制
				{
					i++;
					CBaseCoil::DrawLine(&m_infoDraw,(UINT)phCoil,szOffset);
				}
				if( (int)pvCoil >= COIL_BASE )
					pvCoil->Draw( &m_infoDraw, szOffset );
				else	//不用判断就能直接绘制
					CBaseCoil::DrawLine(&m_infoDraw,(UINT)pvCoil,szOffset);
			}
		}else
			break;
		szOffset.cy += m_nRowLen;
	}
	CBaseCoil::EndPaint( &m_infoDraw );
}

void CSchematic::DrawHelp( HDC hdcDest,HDC hdcSrc )
{
	BitBlt( hdcDest, 0, 0, m_szView.cx,m_szView.cy, hdcSrc,0, 0, SRCCOPY );
	//StretchBlt( hdcDest,0,0,m_szView.cx,m_szView.cy,
	//	hdcSrc,0,0,m_szView.cx,m_szView.cy,SRCCOPY );
}
int CSchematic::GetHeight()
{
	return m_szMax.cy;
}
SIZE CSchematic::GetSize()
{
	return m_szMax;
}

CBaseCoil* CSchematic::GetPosCoil( CoilPos &pos, RECT* prc )
{
	int nLenTmp;
	int nRowTest;
	int nStart,nEnd;
	int nLn = 0, nCol = 0;
	CBaseCoil* pCoil = NULL;
	if( pos.nLn < m_nPosRow )
		nRowTest = m_nPosRow;
	else if( pos.nLn > m_nPosRow + m_nNumRow )
		nRowTest = m_nPosRow + m_nNumRow - 1;
	else 
		nRowTest = pos.nLn;
	if( nRowTest != pos.nLn )
	{
		if( pos.nCol == m_cBlock.m_nNoteCoilPos )
			pos.nCol = ( m_nPosCol > m_cBlock.m_nMatherLine ) ? m_nPosCol : m_cBlock.m_nMatherLine;
		pos.nLn = nRowTest;
	}
	nLenTmp = 0;
	nRowTest = m_nPosRow;
	do{
		nLn += nLenTmp;
		if( m_cBlock.IsHaveNote( nRowTest ) )
		{
			nLenTmp = m_infoDraw.nUnitLen;
			if( (pos.nCol == m_cBlock.m_nNoteCoilPos) && ( nRowTest + 1 > pos.nLn ) )
			{
				nRowTest++;
				break;
			}
			nLn += nLenTmp;
			nLenTmp = m_infoDraw.nRowLen;
		}
		else
			nLenTmp = m_infoDraw.nRowLen;
		nRowTest++;
	}while ( nRowTest <= pos.nLn );
	if( pos.nLn > m_cBlock.GetMaxRow() || pos.nCol >= m_cBlock.GetMaxSize().nCol || pos.nCol < 0 )
		return (CBaseCoil*)COIL_ERROR;
	nStart = pos.nCol;
	nEnd = pos.nCol;
	if( nLenTmp == m_infoDraw.nUnitLen )
	{
		pCoil = m_cBlock.GetNoteCoil( pos.nLn );
		nStart = m_cBlock.m_nMatherLine;
		nEnd = m_cBlock.m_nOutputLine - 1;
	}else
		pCoil = m_cBlock.FindCoil( pos, &nStart, &nEnd );
	if( prc )
	{
		prc->left = ( nStart - m_nPosCol ) * m_nColLen;
		prc->top = nLn;
		prc->right = prc->left + ( nEnd - nStart + 1 ) * m_nColLen;
		prc->bottom = prc->top + ( ( nLenTmp == m_infoDraw.nUnitLen ) ? m_infoDraw.nUnitLen : m_nRowLen ) ;
	}
	return pCoil;
}
CBaseCoil* CSchematic::GetPosCoil( POINT &pt, CoilPos &pos, RECT* prc )
{
	int nStart,nEnd;
	int nLenTmp;
	int nLn = 0, nCol = 0;
	CBaseCoil* pCoil = NULL;
	pos.nLn = m_nPosRow;
	pos.nCol = 1;
	nLenTmp = 0;
	do{
		nLn += nLenTmp;
		if( m_cBlock.IsHaveNote( pos.nLn ) )
		{
			nLenTmp = m_infoDraw.nUnitLen;
			if( nLn + nLenTmp >= pt.y )
			{
				pos.nLn++;
				break;
			}
			nLn += nLenTmp;
			nLenTmp = m_infoDraw.nRowLen;
		}
		else
			nLenTmp = m_infoDraw.nRowLen;
		pos.nLn++;
	}while ( nLn + nLenTmp < pt.y );
	pos.nLn--;
	pos.nCol = pt.x / m_nColLen + m_nPosCol;
	if( pos.nLn > m_cBlock.GetMaxRow() || pos.nCol >= m_cBlock.GetMaxSize().nCol || pos.nCol < 0 )
	{
		if( pos.nLn >= m_cBlock.GetMaxRow() ) pos.nLn = m_cBlock.GetMaxRow();
		return (CBaseCoil*)COIL_ERROR;
	}
	if( pos.nCol >= m_cBlock.m_nOutputLine )
		pos.nCol = m_cBlock.m_nOutputLine - 1;
	nStart = pos.nCol;
	nEnd = pos.nCol;
	if( nLenTmp == m_infoDraw.nUnitLen )
	{
		pCoil = m_cBlock.GetNoteCoil( pos.nLn );
		pos.nCol = m_cBlock.m_nNoteCoilPos;
		nStart = m_cBlock.m_nMatherLine;
		nEnd = m_cBlock.m_nOutputLine - 1;
	}else
		pCoil = m_cBlock.FindCoil( pos, &nStart, &nEnd );
	if( prc )
	{
		prc->left = ( nStart - m_nPosCol ) * m_nColLen;
		prc->top = nLn;
		prc->right = prc->left + ( nEnd - nStart + 1 ) * m_nColLen;
		prc->bottom = prc->top + ( ( nLenTmp == m_infoDraw.nUnitLen ) ? m_infoDraw.nUnitLen : m_nRowLen ) ;
	}
	return pCoil;
}

BOOL CSchematic::IsEmptyLine( LPCoilLine pLine )
{
	int m_maxCol = m_cBlock.GetMaxSize().nCol;
	for( int i = 0; i < m_maxCol; i++ )
	{
		if( pLine[i].hCoil != NULL || pLine[i].vCoil != NULL )
			return FALSE;
	}
	return TRUE;
}
void CSchematic::CleanEmptyLine()
{
	vector<LPCoilLine>::iterator iter,iterMove;

	for ( iterMove = iter = m_cBlock.m_vCoil.begin(); iter != m_cBlock.m_vCoil.end(); iter++ )
	{
		if( IsEmptyLine(*iter) )
			continue;
		if( iterMove != iter )
			*iterMove = *iter;
		iterMove++;
	}
	m_cBlock.m_vCoil.erase( iterMove, m_cBlock.m_vCoil.end() );
	m_cBlock.m_ILine.nMaxRow = m_cBlock.m_vCoil.size() - 1;
}

BOOL CSchematic::TestCoilType( UINT uFormat,CStdString& strTest, unsigned int* coil, unsigned int* addr )
{
	CStdString strAnsi = strTest;
	char* pAnsi = (char*)strAnsi.ToAnsi();
	if( TestCoilType( uFormat,pAnsi,addr ) )
	{
		if( coil )
		{
			switch(*pAnsi)
			{
			case 'X': *coil = api_coilX; break;
			case 'Y': *coil = api_coilY; break;
			case 'S': *coil = api_coilS; break;
			case 'M': *coil = api_coilM; break;
			case 'T': *coil = api_coilT; break;
			case 'C': *coil = api_coilC; break;
			case 'D': *coil = api_coilD; break;
			case 'P': *coil = api_coilP; break;
			case 'K': *coil = api_coilK; break;
			case 'V': *coil = api_coilV; break;
			default:
				*coil = api_coilX;
			}
		}
		return TRUE;
	}else
		return FALSE;
}

BOOL CSchematic::TestCoilType( UINT uFormat,char* pstr, unsigned int* addr )
{
	UINT32 num;
	_strupr( pstr );
	if( *pstr == 'X' || *pstr == 'Y' )
	{
		if( !Str2Num( pstr + 1,&num,8 ) )
			return FALSE;
	}else if( *pstr == 'H' )
	{
		if( !Str2Num( pstr + 1,&num,16 ) )
			return FALSE;
	}else
	{
		if( !Str2Num( pstr + 1,&num,10 ) )
			return FALSE;
	}
	if( *pstr == 'X' )
	{
		if( !( uFormat & ARG_COIL_X ) )
			return FALSE;
	}else if( *pstr == 'Y' )
	{
		if( !( uFormat & ARG_COIL_Y ) )
			return FALSE;
	}else if( *pstr == 'M' )
	{
		if( !( uFormat & ARG_COIL_M ) )
			return FALSE;
	}else if( *pstr == 'S' )
	{
		if( !( uFormat & ARG_COIL_S ) )
			return FALSE;
	}else if( *pstr == 'T' )
	{
		if( !( uFormat & ARG_COIL_T ) )
			return FALSE;
	}else if( *pstr == 'C' )
	{
		if( !( uFormat & ARG_COIL_C ) )
			return FALSE;
	}else if( *pstr == 'D' )
	{
		if( !( uFormat & ARG_COIL_D ) )
			return FALSE;
	}else if( *pstr == 'P' )
	{
		if( !( uFormat & ARG_COIL_P ) )
			return FALSE;
	}else if( *pstr == 'V' )
	{
		if( !( uFormat & ARG_COIL_V ) )
			return FALSE;
	}else if( *pstr == 'K' )
	{
		if( !( uFormat & ARG_COIL_K ) )
			return FALSE;
	}else
		return FALSE;
	if( addr ) *addr = num;
	return TRUE;
}
BOOL CSchematic::TestInsInput( CoilPos& pos,eCoilInsType eType, CStdString ins )
{
	int nOccupation;
	LPInsItem pins = NULL;
	int szlist;
	char* pInsLine[MAXSZ_ARG];		//最多10个
	char* pstr;
	char** pGet;
	initInsHash();
	pstr = (char*)ins.ToAnsi();
	if( pos.nCol == 0 )
	{
		if( ( szlist = GetInisLine( &pstr, pInsLine, 10,NULL ) ) != 1 || !TestCoilType(ARG_COIL_P,pInsLine[0] ) )
			return FALSE;
		else
			return TRUE;
	}
	if( pos.nCol >= m_cBlock.m_nOutputLine )
		return FALSE;
	if( ( eType == eOROpen || eType == eORClose || eType == eORPCoil || eType == eORFCoil )
		&& pos.nLn < 1 )
		return FALSE;
	if( eType == eNULL )
		szlist = GetInisLine( &pstr, pInsLine, 10,NULL );
	else
	{
		pGet = pInsLine + 1;
		switch( eType )
		{
		case eOpen:
			*pInsLine = "LD";
			break;
		case eClose:
			*pInsLine = "LDI";
			break;
		case ePCoil:
			*pInsLine = "LDP";
			break;
		case eFCoil:
			*pInsLine = "LDF";
			break;
		case eOROpen:
			*pInsLine = "OR";
			break;
		case eORClose:
			*pInsLine = "ORI";
			break;
		case eORPCoil:
			*pInsLine = "ORP";
			break;
		case eORFCoil:
			*pInsLine = "ORF";
			break;
		case eOut:
			*pInsLine = "OUT";
			break;
		case eFNC:
			pGet = pInsLine;
			break;
		case eP:
			*pInsLine = "MEP";
			break;
		case eF:
			*pInsLine = "MEF";
			break;
		case eINV:
			*pInsLine = "INV";
			break;
		}
		szlist = GetInisLine( &pstr, pGet, 9,NULL );
		szlist += pGet - pInsLine;
	}
	if( szlist <= 0 )
		return FALSE;
	if( *pInsLine[ szlist - 1] == ';' )
		--szlist;
	pins = GetInsItemByStr( pInsLine[0] );
	if ( pins == NULL )
		return FALSE;
	do{
		int i = 0;
		nOccupation = pins->argc;//( ( pins->ins > insPLF ) ? szlist : szlist - 1 );
		if( nOccupation == 0 ) 
			return FALSE;		//不可能等于0
		if( pins->argc != szlist - 1 )
			continue;
		while( --szlist )
		{
			if( !TestCoilType( pins->argv[ i ], pInsLine[ i + 1 ] ) )
				continue;
		}
		if( m_cBlock.m_nOutputLine - pos.nCol < nOccupation )
			return FALSE;
		return TRUE;
	}while( pins = GetNextInsItemByStr( pins ) );
	return FALSE;
}
BOOL CSchematic::OnInsInputChange( CoilPos& pos,eCoilInsType eType, CStdString& ins)
{
	//先调用TestInsInput（）判断，OnInsInputChange（）一定能成功
	CStdString strConvert;
	CBaseCoil* pOld = NULL;
	CInputCoil::eInputType eIType;
	CInputCoil *pInput;
	COutputCoil::eOutputType eOType;
	COutputCoil *pOutput;
	CSymbolCoil::eSymbolType eSType;
	CSymbolCoil* pSymBol;
	CNoteCoil* pNote;

	char** pGet;
	int nTmp;
	LPCoilLine pLine;
	CStdString strArg;
	LPInsItem pins;
	int szlist;
	char* pInsLine[10];		//最多10个
	char* pstr = (char*)ins.ToAnsi();
	if( eType == eNULL )
	{
		szlist = GetInisLine( &pstr, pInsLine, 10,NULL );
		if( pInsLine[0][0] == 'P' )
		{
			if( szlist != 1 )
				return FALSE;
			else
			{
				DeleteCoil( pos );
				strConvert.Assign( pInsLine[0], CP_ACP );
				pSymBol = new CSymbolCoil( CSymbolCoil::eLable, strConvert );
				m_cBlock.InsertLable( pSymBol,pos.nLn );
				return TRUE;
			}
		}
	}
	else
	{
		pGet = pInsLine + 1;
		switch( eType )
		{
		case eOpen:
			*pInsLine = "LD";
			break;
		case eClose:
			*pInsLine = "LDI";
			break;
		case ePCoil:
			*pInsLine = "LDP";
			break;
		case eFCoil:
			*pInsLine = "LDF";
			break;
		case eOROpen:
			*pInsLine = "OR";
			break;
		case eORClose:
			*pInsLine = "ORI";
			break;
		case eORPCoil:
			*pInsLine = "ORP";
			break;
		case eORFCoil:
			*pInsLine = "ORF";
			break;
		case eOut:
			*pInsLine = "OUT";
			break;
		case eFNC:
			pGet = pInsLine;
			break;
		case eP:
			*pInsLine = "MEP";
			break;
		case eF:
			*pInsLine = "MEF";
			break;
		case eINV:
			*pInsLine = "INV";
			break;
		}
		szlist = GetInisLine( &pstr, pGet, 9,NULL );
		szlist += pGet - pInsLine;
	}
	if( szlist <= 0 )
		return FALSE;
	if( *pInsLine[ szlist - 1] == ';' )
		--szlist;
	pins = GetInsItemByStr( pInsLine[0] );
	pLine = m_cBlock.GetLine( pos.nLn );
	switch ( pins->bin.iclass )
	{
	case 1:
		{
			switch ( pins->bin.all )
			{
			case dinsLDF:	eIType = CInputCoil::eDownPluse;	break;
			case dinsLDP:	eIType = CInputCoil::eUpPluse;		break;
			case dinsLDI:	eIType = CInputCoil::eClose;		break;
			case dinsLD:	eIType = CInputCoil::eOpen;			break;
			}
			strArg.Format( "%c%03s",*pInsLine[1], pInsLine[1] + 1 );
			pInput = new CInputCoil( eIType, strArg.GetBuffer() );
			DeleteCoil( pos );
			m_cBlock.InsertCoil( pInput,pos );
		}
		break;
	case 2:
		{
			switch ( pins->bin.all )
			{
			case dinsORF:	eIType = CInputCoil::eDownPluse;	break;
			case dinsORP:	eIType = CInputCoil::eUpPluse;		break;
			case dinsORI:	eIType = CInputCoil::eClose;		break;
			case dinsOR:	eIType = CInputCoil::eOpen;			break;
			}
			strArg.Format( "%c%03s",*pInsLine[1], pInsLine[1] + 1 );
			pInput = new CInputCoil( eIType, strArg.GetBuffer() );
			DeleteCoil( pos );
			m_cBlock.InsertCoil( pInput,pos );
			pos.nLn--;
			pos.nCol--;
			m_cBlock.AddLineCoil( COIL_VLINE,pos );
			pos.nCol++;
			m_cBlock.AddLineCoil( COIL_VLINE,pos );
		}
		break;
	case 6:
		{
			CoilPos posOut = pos;
			while( posOut.nCol < m_cBlock.m_nOutputLine )
				DeleteCoil( posOut ),++posOut.nCol;
			m_cBlock.SetPos(pos);
			if( pins->bin.all == dinsOUT )
			{
				eOType = COutputCoil::eCoil;
				strArg.Format( "%c%03s",*pInsLine[1], pInsLine[1] + 1 );
				pOutput = new COutputCoil( eOType,strArg.GetBuffer() );
				if( szlist == 3 )
				{
					strArg.Format( "%c%03s",*pInsLine[2], pInsLine[2] + 1 );
					pOutput->SetArg( strArg.GetBuffer() );
				}
				if( !m_cBlock.OutCoil( pOutput ) )
				{
					delete pOutput;
					return FALSE;
				}
			}else if( pins->bin.all == dinsSET || pins->bin.all == dinsRST )
			{
				eOType = COutputCoil::eFnc;
				strArg.Format( "%s %c%03s",(pins->bin.all == dinsSET)?"SET":"RST",*pInsLine[1], pInsLine[1] + 1 );
				pOutput = new COutputCoil( eOType,strArg.GetBuffer(),2 );
				if( !m_cBlock.OutCoil( pOutput ) )
				{
					delete pOutput;
					return FALSE;
				}
			}
		}
		break;
	default:
		{
			CoilPos posOut = pos;
			while( posOut.nCol++ < m_cBlock.m_nOutputLine )
				DeleteCoil( posOut );
			m_cBlock.SetPos(pos);
			eOType = COutputCoil::eFnc;
			nTmp = 0;
			strArg.Empty();
			while( nTmp < szlist )
			{
				strArg.Append(pInsLine[nTmp],CP_ACP);
				strArg.Append(_T(" "));
				++nTmp;
			}
			pOutput = new COutputCoil( eOType,strArg.GetBuffer(),nTmp );
			if( pins->bin.all == dinsSTL 
				|| pins->bin.all == dinsRET 
				|| pins->bin.all == dinsFEND 
				|| pins->bin.all == dinsEND )
			{
				if( !m_cBlock.OutLine( pOutput ) )
				{
					delete pOutput;
					return FALSE;
				}
			}else
			{
				if( !m_cBlock.OutCoil( pOutput ) )
				{
					delete pOutput;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}
