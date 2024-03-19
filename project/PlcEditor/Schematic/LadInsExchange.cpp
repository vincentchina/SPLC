#include "StdAfx.h"
#include "Schematic.h"
#include "insHash.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern "C" long OpenPLCFile( FILE** pFile, const char* plcFile, char** err );
extern "C" int GetInisLine(char** pstr, char* pList[], int szlist, int* plineGet );

BOOL CSchematic::Ins2Coil( char* pList[], int szlist,vector<int>& vColPos )
{
	char strArg[20] = {0};
	insItem* pins;
	CInputCoil::eInputType eIType;
	CInputCoil *pInput;
	COutputCoil::eOutputType eOType;
	COutputCoil *pOutput;
	CSymbolCoil::eSymbolType eSType;
	CSymbolCoil* pSymBol;
	CNoteCoil* pNote;
	int nTmp;

	if( szlist <= 0 )
		return FALSE;
	if( pList[0][0] == ';' )
	{
		pNote = new CNoteCoil( pList[0] );
		return m_cBlock.InsertNoteLine( pNote,m_cBlock.m_ILine.pos.nLn );
	}
	//先处理P
	if( pList[0][0] == 'P' )
	{
		if( szlist != 1 || !TestCoilType(ARG_COIL_P,pList[0]) )
			return FALSE;
		pSymBol = new CSymbolCoil( CSymbolCoil::eLable,pList[0] );
		if( !m_cBlock.P( pSymBol ) )
		{
			delete pSymBol;
			return FALSE;
		}
		return TRUE;
	}
	pins = GetInsItem( pList[0] );
	if( pins == NULL )
		return FALSE;
	if( pins->ins <= insLDF )
	{
		switch ( pins->ins )
		{
		case insLDF: eIType = CInputCoil::eDownPluse;break;
		case insLDP: eIType = CInputCoil::eUpPluse;break;
		case insLDI: eIType = CInputCoil::eClose;break;
		case insLD: eIType = CInputCoil::eOpen;break;
		}
		pInput = new CInputCoil( eIType, pList[1] );
		if( !m_cBlock.LDCoil( pInput ) )
		{
			delete pInput;
			return FALSE;
		}
	}else if( pins->ins <= insORF )
	{
		switch ( pins->ins )
		{
		case insANDF: case insORF: eIType = CInputCoil::eDownPluse;break;
		case insANDP: case insORP: eIType = CInputCoil::eUpPluse;break;
		case insANI: case insORI: eIType = CInputCoil::eClose;break;
		case insAND: case insOR: eIType = CInputCoil::eOpen;break;
		}
		pInput = new CInputCoil( eIType, pList[1] );
		if( pins->ins >= insAND && pins->ins <= insANDF )
		{
			if( !m_cBlock.AndCoil( pInput ) )
			{
				delete pInput;
				return FALSE;
			}
		}else
		{
			if( !m_cBlock.OrCoil( pInput ) )
			{
				delete pInput;
				return FALSE;
			}
		}

	}else if( pins->ins <= insORB )
	{
		if( pins->ins == insANB )
			m_cBlock.AndBlock();
		else
			m_cBlock.OrBlock();
	}else if( pins->ins <= insMPP )
	{
		if( pins->ins == insMPS )
		{
			vColPos.push_back( m_cBlock.MPS() );
		}else
		{
			if( vColPos.size() )
			{
				m_cBlock.MPP( vColPos.back() );
				if( pins->ins == insMPP )
					vColPos.pop_back();
			}else
				return FALSE;
		}
	}else if( pins->ins <= insMEF)
	{
	}else if( pins->ins <= insPLF )
	{
		if( pins->ins == insOUT )
		{
			eOType = COutputCoil::eCoil;
			
			pOutput = new COutputCoil( eOType,pList[1] );
			if( szlist == 3 )
				pOutput->SetArg( pList[2] );
			if( !m_cBlock.OutCoil( pOutput ) )
			{
				delete pOutput;
				return FALSE;
			}
		}else// if( pins->ins == insSET )
		{
			nTmp = 0;
			eOType = COutputCoil::eFnc;
			*strArg = 0;
			while( nTmp < szlist )
			{
				strcat( strArg, pList[ nTmp++ ] );
				strcat( strArg, " " );
			}
			pOutput = new COutputCoil( eOType, strArg, szlist );
			if( !m_cBlock.OutCoil( pOutput ) )
			{
				delete pOutput;
				return FALSE;
			}
		}
	}else if( pins->ins <= insCALL)
	{
		eOType = COutputCoil::eFnc;
		nTmp = 0;
		while( nTmp < szlist )
		{
			strcat( strArg, pList[nTmp] );
			strcat( strArg, " " );
			++nTmp;
		}
		pOutput = new COutputCoil( eOType,strArg,nTmp );
		if( pins->ins == insSTL 
			|| pins->ins == insRET 
			|| pins->ins == insFEND 
			|| pins->ins == insSRET 
			|| pins->ins == insEND )
		{
			if( !m_cBlock.OutLine( pOutput ) )
			{
				delete pOutput;
				return FALSE;
			}
		}else if( pins->ins == insCJ || pins->ins == insCJP )
		{
			if( !m_cBlock.OutCoil( pOutput ) )
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
	}else if( pins->ins == insP )
	{

	}else
		return FALSE;
	return TRUE;
}

BOOL CSchematic::FromIns( LPTSTR strFile )
{
	BOOL bRes = FALSE;
	FILE* fFile = NULL;
	long szFile;
	char* err;
	char* buf = NULL;

	szFile = OpenPLCFile(&fFile, strFile, &err );
	if( szFile == -1 )
	{
		m_strErr.Format(_T("打开文件%s失败！%s,error:%d\n"),strFile,err,GetLastError() );
		return FALSE;
	}
	buf = new char[ szFile + 1 ];
	if( fread(buf,1,szFile,fFile) != szFile )
	{
		m_strErr.Format(_T("读取文件%s失败！error:%d\n"),strFile,GetLastError() );
	}
	buf[szFile] = 0;
	bRes = FromInsBuff( buf,szFile );
leave:
	if( buf ) delete[] buf;
	fclose( fFile );
	return bRes;
}

BOOL CSchematic::FromInsBuff( char* buf, int sz )
{
	BOOL bRes = FALSE;
	char* strPos;
	char* strEnd;
	int szlist;
	char* pInsLine[MAXSZ_ARG];		//最多10个
	int n;
	int nLine = 0;
	vector<int> vColPos;	//用于mpp、mps

	if( initInsHash() == 0 ) return FALSE;
	//开始转换
	ResetBlock();
	m_cBlock.Ins2LadReset();
	strPos = buf;
	strEnd = buf + sz;
	while( strPos < strEnd )
	{
		n = 0;
		szlist = GetInisLine( &strPos, pInsLine, 10,&n );
		nLine += n;
		if( szlist <= 0 || ( szlist && Ins2Coil( pInsLine, szlist,vColPos ) == 0 ) )
		{
			printf("error instruction at line:%d\n", nLine );
			return FALSE;
		}
	}
	CleanEmptyLine();
	return TRUE;
}
BOOL CSchematic::ToIns( LPTSTR strFile )
{
	BOOL bRes = FALSE;
	CFile fFile;
	CFileException e;
	if( fFile.Open( strFile,CFile::modeCreate | CFile::modeWrite,&e ) )
	{
		bRes = ToIns( fFile );
		fFile.Close();
	}
	return bRes;
}
BOOL CSchematic::ToIns( CFile& fFile )
{
	CString str;
	BOOL bRes = FALSE;
	CCoilBlock::CoilPos pos;

	str.Empty();
	int nLn;
	if( !m_cBlock.AdjustConnection())					return FALSE;
	if( ( nLn = m_cBlock.GetFirstInputLine() ) == -1 )	return FALSE;
	do{
		str.Empty();
		if( m_cBlock.InputLine2Ins( nLn, str) )
		{
			//str += _T("\n");	//添加换行，好看而已
			fFile.Write( str.GetBuffer(),str.GetLength() );
			//fwrite( str.GetBuffer(),sizeof(TCHAR),str.GetLength(),fFile );
		}
		else
			return FALSE;
	}while( ( nLn = m_cBlock.GetNextInputLine( nLn ) ) != -1 );
	return TRUE;
}