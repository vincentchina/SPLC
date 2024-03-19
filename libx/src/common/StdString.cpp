#include "common\StdString.h"

#define NDEBUG
#include <assert.h>
#define ASSERT assert
#include <string>
#include <malloc.h>

static const unsigned long m_magicKey = 3559;

CStdString::CStdString()
{
	wchar_t* pstr = (wchar_t*)m_buf.Reserver(sizeof(wchar_t));
	*pstr = 0;
}

CStdString::CStdString(const wchar_t ch)
{
	wchar_t* pstr = (wchar_t*)m_buf.Reserver(sizeof(wchar_t));
	pstr[0] = ch;
	pstr[1] = 0;
}

CStdString::CStdString(const wchar_t* lpsz, int nLen)
{      
	ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
	CStdString();
	Assign(lpsz, nLen);
}

CStdString::CStdString(const CStdString& src)
{
	CStdString();
	Assign( src.GetBuffer() );
}

CStdString::~CStdString()
{
}

int CStdString::GetLength() const
{ 
	return (int) wcslen((wchar_t*)m_buf.GetBuf()); 
}

CStdString::operator const wchar_t*() const 
{ 
	return (wchar_t*)m_buf.GetBuf(); 
}

void CStdString::Append(const wchar_t* pstr)
{
	int nNewLength = GetLength() + (int) wcslen(pstr);
	wchar_t* pWStr = (wchar_t*)m_buf.Reserver( sizeof(wchar_t) * ( nNewLength + 1 ) );
	wcscat_s(pWStr,nNewLength + 1, pstr);
}

void CStdString::Append( const char* pstr, unsigned int cp )
{
	wchar_t* pwstr;
	int cz = strlen(pstr);
	int nLen = MultiByteToWideChar(cp, 0, pstr, cz, NULL, 0 );
	pwstr = (wchar_t*)malloc( sizeof( wchar_t ) * ( nLen + 1 ) );
	MultiByteToWideChar( cp, 0, pstr, cz, pwstr, nLen );
	pwstr[nLen] = 0;
	Append( pwstr );
	free( pwstr );
}

void CStdString::Assign(const wchar_t* pstr, int cchMax)
{
	if( pstr == NULL ) pstr = L"";
	if( cchMax < 0 ) cchMax = (int) wcslen(pstr);
	wchar_t* pWStr = (wchar_t*)m_buf.Reserver( sizeof(wchar_t) * ( cchMax + 1 ) );
	wcsncpy_s( pWStr, cchMax + 1, pstr, cchMax );
	pWStr[cchMax] = L'\0';
}

void CStdString::Assign(const char* pstr, unsigned int cp, int nLength )
{
	wchar_t* pwstr;
	if( pstr == NULL )
	{
		Assign( L"" );
		return;
	}
	if( nLength == -1 )
		nLength = strlen(pstr);
	int nLen = MultiByteToWideChar(cp, 0, pstr, nLength, NULL, 0 );
	pwstr = (wchar_t*)malloc( sizeof( wchar_t ) * ( nLen + 1 ) );
	MultiByteToWideChar( cp, 0, pstr, nLength, pwstr, nLen );
	pwstr[ nLen ] = 0;
	Assign( pwstr );
	free( pwstr );
}

bool CStdString::IsEmpty() const 
{ 
	return ( (wchar_t*)m_buf.GetBuf() )[0] == L'\0'; 
}

void CStdString::Empty() 
{ 
	( (wchar_t*)m_buf.GetBuf() )[0] = L'\0'; 
}

const wchar_t* CStdString::GetBuffer() const
{
	return (const wchar_t*)m_buf.GetBuf();
}
const wchar_t* CStdString::GetBuffer( int nLen )
{
	m_buf.Reserver( nLen * sizeof(wchar_t) );
	return (const wchar_t*)m_buf.GetBuf();
}
wchar_t CStdString::GetAt(int nIndex) const
{
	return ( (wchar_t*)m_buf.GetBuf() )[nIndex];
}

wchar_t CStdString::operator[] (int nIndex) const
{ 
	return ( (wchar_t*)m_buf.GetBuf() )[nIndex];
}   

const CStdString& CStdString::operator=(const CStdString& src)
{      
	Assign(src.GetBuffer());
	return *this;
}

const CStdString& CStdString::operator=(const wchar_t* lpStr)
{      
	ASSERT(!::IsBadStringPtr(lpStr,-1));
	Assign(lpStr);
	return *this;
}

const CStdString& CStdString::operator=(const wchar_t ch)
{
	Empty();
	wchar_t* pWStr = (wchar_t*)m_buf.Reserver( sizeof(wchar_t) * 2 );
	pWStr[0] = ch;
	pWStr[1] = L'\0';
	return *this;
}

CStdString CStdString::operator+(const CStdString& src)
{
	CStdString sTemp = *this;
	sTemp.Append(src.GetBuffer());
	return sTemp;
}

CStdString CStdString::operator+(const wchar_t* lpStr)
{
	ASSERT(!::IsBadStringPtr(lpStr,-1));
	CStdString sTemp = *this;
	sTemp.Append(lpStr);
	return sTemp;
}

const CStdString& CStdString::operator+=(const CStdString& src)
{      
	Append(src.GetBuffer());
	return *this;
}

const CStdString& CStdString::operator+=(const wchar_t* lpStr)
{      
	ASSERT(!::IsBadStringPtr(lpStr,-1));
	Append(lpStr);
	return *this;
}

const CStdString& CStdString::operator+=(const wchar_t ch)
{      
	wchar_t str[] = { ch, L'\0' };
	Append(str);
	return *this;
}

bool CStdString::operator == (const wchar_t* str) const { return (Compare(str) == 0); };
bool CStdString::operator != (const wchar_t* str) const { return (Compare(str) != 0); };
bool CStdString::operator <= (const wchar_t* str) const { return (Compare(str) <= 0); };
bool CStdString::operator <  (const wchar_t* str) const { return (Compare(str) <  0); };
bool CStdString::operator >= (const wchar_t* str) const { return (Compare(str) >= 0); };
bool CStdString::operator >  (const wchar_t* str) const { return (Compare(str) >  0); };

void CStdString::SetAt(int nIndex, wchar_t ch)
{
	ASSERT(nIndex>=0 && nIndex<GetLength());
	( (wchar_t*)m_buf.GetBuf() )[nIndex] = ch;
}

int CStdString::Compare(const wchar_t* lpsz) const 
{ 
	return wcscmp((wchar_t*)m_buf.GetBuf(), lpsz); 
}

int CStdString::CompareNoCase(const wchar_t* lpsz) const 
{ 
	return _wcsicmp((wchar_t*)m_buf.GetBuf(), lpsz); 
}

void CStdString::MakeUpper() 
{ 
	_wcsupr((wchar_t*)m_buf.GetBuf() ); 
}

void CStdString::MakeLower() 
{ 
	_wcslwr((wchar_t*)m_buf.GetBuf()); 
}

CStdString CStdString::Left(int iLength) const
{
	if( iLength < 0 ) iLength = 0;
	if( iLength > GetLength() ) iLength = GetLength();
	return CStdString((wchar_t*)m_buf.GetBuf(), iLength);
}

CStdString CStdString::Mid(int iPos, int iLength) const
{
	if( iLength < 0 ) iLength = GetLength() - iPos;
	if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
	if( iLength <= 0 ) return CStdString();
	return CStdString((wchar_t*)m_buf.GetBuf() + iPos, iLength);
}

CStdString CStdString::Right(int iLength) const
{
	int iPos = GetLength() - iLength;
	if( iPos < 0 ) {
		iPos = 0;
		iLength = GetLength();
	}
	return CStdString((wchar_t*)m_buf.GetBuf() + iPos, iLength);
}

int CStdString::Find(wchar_t ch, int iPos /*= 0*/) const
{
	ASSERT(iPos>=0 && iPos<=GetLength());
	if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
	const wchar_t* p = wcsrchr((wchar_t*)m_buf.GetBuf() + iPos, ch);
	if( p == NULL ) return -1;
	return (int)(p - (wchar_t*)m_buf.GetBuf());
}

int CStdString::Find(const wchar_t* pstrSub, int iPos /*= 0*/) const
{
	ASSERT(!::IsBadStringPtr(pstrSub,-1));
	ASSERT(iPos>=0 && iPos<=GetLength());
	if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
	const wchar_t* p = wcsstr((wchar_t*)m_buf.GetBuf() + iPos, pstrSub);
	if( p == NULL ) return -1;
	return (int)(p - (wchar_t*)m_buf.GetBuf());
}

int CStdString::ReverseFind(wchar_t ch) const
{
	const wchar_t* p = wcsrchr((wchar_t*)m_buf.GetBuf(), ch);
	if( p == NULL ) return -1;
	return (int)(p - (wchar_t*)m_buf.GetBuf());
}

int CStdString::Replace(const wchar_t* pstrFrom, const wchar_t* pstrTo)
{
	CStdString sTemp;
	int nCount = 0;
	int iPos = Find(pstrFrom);
	if( iPos < 0 ) return 0;
	int cchFrom = (int) wcslen(pstrFrom);
	int cchTo = (int) wcslen(pstrTo);
	while( iPos >= 0 ) {
		sTemp = Left(iPos);
		sTemp += pstrTo;
		sTemp += Mid(iPos + cchFrom);
		Assign(sTemp);
		iPos = Find(pstrFrom, iPos + cchTo);
		nCount++;
	}
	return nCount;
}

int CStdString::Format(const wchar_t* fmt, ...)
{
	if ( NULL != fmt )
	{
		va_list marker = NULL;           
		va_start(marker , fmt);                            //初始化变量参数
		size_t nLength = _vscwprintf( fmt, marker ) + 1;    //获取格式化字符串长度
		int iRet = vswprintf( (wchar_t*)GetBuffer( nLength ), nLength , fmt, marker );
		va_end(marker );                                    //重置变量参数
		return iRet ;
	}
	return 0;

}
int __cdecl CStdString::Format(const char* fmt, ...)
{
	if ( NULL != fmt )
	{
		char* buf;
		va_list marker = NULL;           
		va_start( marker , fmt );                            //初始化变量参数
		size_t nLength = _vscprintf( fmt, marker ) + 1;    //获取格式化字符串长度
		buf = new char[ nLength ];
		int iRet = vsnprintf( buf, nLength , fmt, marker );
		va_end( marker );                                    //重置变量参数
		Assign( buf, CP_ACP );
		delete[] buf;
		return iRet ;
	}
	return 0;
}

//decode
char* CStdString::Convert( unsigned int cp )
{
	int nLen = WideCharToMultiByte(cp, 0, (wchar_t*)m_buf.GetBuf(), GetLength(), NULL, 0, NULL, NULL);
	char* pStr = (char*)m_pConvert.Reserver( nLen + 1 );
	WideCharToMultiByte(cp, 0, (wchar_t*)m_buf.GetBuf(), GetLength(), pStr, nLen, NULL, NULL);
	pStr[nLen] = 0;
	return pStr;
}

const CStdString CStdString::FromAnsi( char* str, int len )
{
	CStdString stdStr;
	stdStr.Assign( str, CP_ACP,len );
	return stdStr;
}
//hash
unsigned long CStdString::HashByte( const unsigned char* buf, unsigned long nlen ){
	unsigned long res = 0;
	byte* pb,*pend;

	pb = (byte*)buf;
	pend = pb + nlen;

	while( pb < pend )
	{
		res += ( ( res << 4 ) ) + ( *pb ^ m_magicKey );
		pb++;
	}
	return res;
}