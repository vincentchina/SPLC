#include "plcSchematic\Schematic.h"
#include "plcInsSys\plcInsSys.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif*/

using namespace libSch;

static int TestCoil( unsigned long format, char* pCoil )
{
	char* strPart;
	switch( *pCoil )
	{
	case 'X': if( format & ARG_COIL_X ) return 0;break;
	case 'Y': if( format & ARG_COIL_Y ) return 0;break;
	case 'M': if( format & ARG_COIL_M ) return 0;break;
	case 'S': if( format & ARG_COIL_S ) return 0;break;
	case 'T': if( format & ARG_COIL_T ) return 0;break;
	case 'C': if( format & ARG_COIL_C ) return 0;break;
	case 'D': if( format & ARG_COIL_D ) return 0;break;
	case 'K': if( format & ARG_COIL_K ) return 0;break;
	case 'P': if( format & ARG_COIL_P ) return 0;break;
	case 'V': 
		{
			if( format & ARG_COIL_V )
			{
				strPart = pCoil + 1;
				while( *strPart >= '0' && *strPart <= '9' ) ++strPart;
				if( *strPart != '\0' )
					return TestCoil( format, strPart);
				return 0;
			}
		}
		break;
	default: return -1;
	}
	return -1;
}

static LPInsItem GetInsHelp( char* pList[], int szlist )
{
	char* pstr;
	LPInsItem pins;
	int uInsArgc;
	int i;
	char** pArgv;

	//先处理P
	pstr = pList[0];
	if( *pstr == 'P' )
	{
		++pstr;
		while( *pstr != '\0' )
		{
			if( *pstr < '0' || *pstr > '9') break;
			++pstr;
		}
		if( *pstr == '\0' )
			pstr = "P";
		else
			pstr = pList[0];
	}
	pins = GetInsItemByStr( pstr );
	while( pins != NULL )
	{
		//特殊指令处理
		if( pins->bin.all != dinsP )
		{
			uInsArgc = szlist - 1;
			pArgv = pList + 1;
		}else{
			uInsArgc = szlist;
			pArgv = pList;
		}
		if( pins->argc == uInsArgc )
		{
			i = uInsArgc;
			while( i )
			{
				if( TestCoil( pins->argv[i - 1], pArgv[i - 1] ) == -1 )
					break;
				--i;
			}
			if( i == 0 )
				break;
		}
		pins = GetNextInsItemByStr( pins );
	}
	return pins;
}

BOOL CSchematic::Ins2Coil( char* pList[], int szlist,vector<int>& vColPos )
{
	char strArg[60] = {0};
	LPInsItem pins;
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
		pNote = new CNoteCoil( CStdString::FromAnsi( pList[0] ) );
		return m_cBlock.InsertNoteLine( pNote,m_cBlock.m_ILine.pos.nLn );
	}
	if( pList[ szlist - 1][0] == ';' )
		--szlist;
	/*
	if( pList[0][0] == 'P' )
	{
		if( szlist != 1 || !TestCoilType(ARG_COIL_P,pList[0]) )
			return FALSE;
		pSymBol = new CSymbolCoil( CSymbolCoil::eLable,CStdString::FromAnsi( pList[0] ) );
		if( !m_cBlock.P( pSymBol ) )
		{
			delete pSymBol;
			return FALSE;
		}
		return TRUE;
	}*/
	pins = GetInsHelp( pList, szlist );
	if( pins == NULL )
		return FALSE;
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
			default:		return FALSE;
			}
			pInput = new CInputCoil( eIType, CStdString::FromAnsi( pList[1] ) );
			if( !m_cBlock.LDCoil( pInput ) )
			{
				delete pInput;
				return FALSE;
			}
		}
		break;
	case 2:
		{
			switch ( pins->bin.ins )
			{
			case 4:	eIType = CInputCoil::eDownPluse;	break;
			case 3:	eIType = CInputCoil::eUpPluse;		break;
			case 2:	eIType = CInputCoil::eClose;		break;
			case 1:	eIType = CInputCoil::eOpen;			break;
			default:		return FALSE;
			}
			pInput = new CInputCoil( eIType, CStdString::FromAnsi( pList[1] ) );
			if( pins->bin.itype == 2 )
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
		}
		break;
	case 3:
		{
			if( pins->bin.all == dinsANB )
				m_cBlock.AndBlock();
			else
				m_cBlock.OrBlock();
		}
		break;
	case 4:
		{
			if( pins->bin.all == dinsMPS )
			{
				vColPos.push_back( m_cBlock.MPS() );
			}else
			{
				if( vColPos.size() )
				{
					m_cBlock.MPP( vColPos.back() );
					if( pins->bin.all == dinsMPP )
						vColPos.pop_back();
				}else
					return FALSE;
			}
		}
		break;
	case 5:
		{
			if( pins->bin.all == dinsINV )
			{
				eSType = CSymbolCoil::eNot;
				pSymBol = new CSymbolCoil( CSymbolCoil::eNot );
				if( !m_cBlock.AndCoil( pSymBol ) )
				{
					delete pSymBol;
					return FALSE;
				}
				return TRUE;
			}
		}
		break;
	case 6:
	case 0xb:
		{
			if( ( pins->bin.all & 0x0f0f ) == 0x0601 )
			{
				eOType = COutputCoil::eCoil;
				pOutput = new COutputCoil( eOType, CStdString::FromAnsi( pList[1] ) );
				if( szlist == 3 )
					pOutput->SetArg( CStdString::FromAnsi( pList[2] ) );
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
				pOutput = new COutputCoil( eOType, CStdString::FromAnsi( strArg ), szlist );
				if( !m_cBlock.OutCoil( pOutput ) )
				{
					delete pOutput;
					return FALSE;
				}
			}
		}
		break;
	case 7:
		break;
	case 8:
	case 9:
	case 0xa:
	case 0xd:
		{
			if( pins->bin.all == dinsP )
			{
				if( szlist != 1 || !TestCoilType(ARG_COIL_P,pList[0]) )
					return FALSE;
				pSymBol = new CSymbolCoil( CSymbolCoil::eLable,CStdString::FromAnsi( pList[0] ) );
				if( !m_cBlock.P( pSymBol ) )
				{
					delete pSymBol;
					return FALSE;
				}
				return TRUE;
			}
			if( dinsOUTS == pins->bin.all )
			{
				eOType = COutputCoil::eCoil;
				nTmp = 1;
				strcat( strArg, pList[nTmp] );
			}else{
				eOType = COutputCoil::eFnc;
				nTmp = 0;
				while( nTmp < szlist )
				{
					strcat( strArg, pList[nTmp] );
					strcat( strArg, " " );
					++nTmp;
				}
			}
			pOutput = new COutputCoil( eOType,CStdString::FromAnsi( strArg ),nTmp );
			if( pins->bin.all == dinsSTL 
				|| pins->bin.all == dinsRET 
				|| pins->bin.all == dinsFEND 
				|| pins->bin.all == dinsSRET 
				|| pins->bin.all == dinsEND )
			{
				if( !m_cBlock.OutLine( pOutput ) )
				{
					delete pOutput;
					return FALSE;
				}
			}else if( pins->bin.all == dinsCJ || pins->bin.all == dinsCJP )
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
		}
		break;
	case 0xc:
		{
			char* pstr = *pList;
			while( *pstr >= 'A' ) ++pstr;
			if( pins->bin.ins >= 7 )
				--pstr;
			strcpy( strArg, pstr );
			nTmp = 1;
			while( nTmp < szlist )
			{
				strcat( strArg, " " );
				strcat( strArg, pList[ nTmp++ ] );
			}
			pInput = new CInputCoil( CInputCoil::eFnc, CStdString::FromAnsi( strArg ), 2 );
			switch( pins->bin.itype )
			{
			case 0:
				if( !m_cBlock.LDCoil( pInput ) )
				{
					delete pInput;
					return FALSE;
				}
				break;
			case 2:
				if( !m_cBlock.AndCoil( pInput ) )
				{
					delete pInput;
					return FALSE;
				}
				break;
			case 3:
				if( !m_cBlock.OrCoil( pInput ) )
				{
					delete pInput;
					return FALSE;
				}
				break;
			default:		return FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CSchematic::FromIns( CStdString strFile )
{
	BOOL bRes = FALSE;
	FILE* fFile = NULL;
	long szFile;
	char* err;
	char* buf = NULL;

	szFile = OpenPLCFile(&fFile, strFile.ToAnsi(), &err );
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

	if( buf ) delete[] buf;
	fclose( fFile );
	return bRes;
}

BOOL CSchematic::FromInsBuff( char* buf, int sz )
{
	CStdString str;
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
	m_strErr.Empty();
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
			str.Format( _T("\n错误行号：%d"), nLine );
			m_strErr.Append( str );
			return FALSE;
		}
	}
	CleanEmptyLine();
	return TRUE;
}
BOOL CSchematic::ToIns( CStdString& str )
{
	CStdString strLine;
	BOOL bRes = FALSE;

	str.Empty();
	strLine.Empty();
	int nLn;
	if( !m_cBlock.AdjustConnection())					return FALSE;
	if( ( nLn = m_cBlock.GetFirstInputLine() ) == -1 )	return FALSE;
	do{
		strLine.Empty();
		if( m_cBlock.InputLine2Ins( nLn, strLine) )
		{
			//str.Append( _T("\n") );	//添加换行，好看而已
			//wchar_t* p = (wchar_t*)strLine.GetBuffer();
			str.Append( strLine );
		}
		else
			return FALSE;
	}while( ( nLn = m_cBlock.GetNextInputLine( nLn ) ) != -1 );
	return TRUE;
}
BOOL CSchematic::ToIns( FILE* fp )
{
	char* strAnsi;
	CStdString str;
	BOOL bRes = FALSE;

	str.Empty();
	int nLn;
	if( !m_cBlock.AdjustConnection())					return FALSE;
	if( ( nLn = m_cBlock.GetFirstInputLine() ) == -1 )	return FALSE;
	do{
		str.Empty();
		if( m_cBlock.InputLine2Ins( nLn, str) )
		{
			//str += _T("\n");	//添加换行，好看而已
			strAnsi = (char*)str.ToAnsi();
			fwrite( strAnsi,1,strlen( strAnsi ), fp );
			//fFile.Write( str.GetBuffer(),str.GetLength() );
			//fwrite( str.GetBuffer(),sizeof(TCHAR),str.GetLength(),fFile );
		}
		else
			return FALSE;
	}while( ( nLn = m_cBlock.GetNextInputLine( nLn ) ) != -1 );
	return TRUE;
}
