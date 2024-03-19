#pragma once
#include <Windows.h>
#include "StdBuf.h"

class LIBX_API CStdString
{
protected:
	CStdBuf m_buf;
	CStdBuf m_pConvert;

public:
	CStdString();
	CStdString(const wchar_t ch);
	CStdString(const CStdString& src);
	CStdString(const wchar_t* lpsz, int nLen = -1);
	~CStdString();

	void Empty();
	int GetLength() const;
	int Size(){ return ( m_buf.GetSize() / sizeof(wchar_t) ); };
	bool IsEmpty() const;
	wchar_t GetAt(int nIndex) const;
	void Append(const wchar_t* pstr);
	void Append( const char* pstr, UINT cp );
	void Assign(const wchar_t* pstr, int nLength = -1);
	void Assign(const char* pstr, UINT cp, int nLength = -1);
	const wchar_t* GetBuffer() const;
	const wchar_t* GetBuffer( int nLen );

	void SetAt(int nIndex, wchar_t ch);
	operator const wchar_t*() const;

	wchar_t operator[] (int nIndex) const;
	const CStdString& operator=(const CStdString& src);
	const CStdString& operator=(const wchar_t ch);
	const CStdString& operator=(const wchar_t* pstr);

	CStdString operator+(const CStdString& src);
	CStdString operator+(const wchar_t* pstr);
	const CStdString& operator+=(const CStdString& src);
	const CStdString& operator+=(const wchar_t* pstr);
	const CStdString& operator+=(const wchar_t ch);

	bool operator == (const wchar_t* str) const;
	bool operator != (const wchar_t* str) const;
	bool operator <= (const wchar_t* str) const;
	bool operator <  (const wchar_t* str) const;
	bool operator >= (const wchar_t* str) const;
	bool operator >  (const wchar_t* str) const;

	int Compare(const wchar_t* pstr) const;
	int CompareNoCase(const wchar_t* pstr) const;

	void MakeUpper();
	void MakeLower();

	CStdString Left(int nLength) const;
	CStdString Mid(int iPos, int nLength = -1) const;
	CStdString Right(int nLength) const;

	int Find(wchar_t ch, int iPos = 0) const;
	int Find(const wchar_t* pstr, int iPos = 0) const;
	int ReverseFind(wchar_t ch) const;
	int Replace(const wchar_t* pstrFrom, const wchar_t* pstrTo);

	int __cdecl Format(const wchar_t* fmt, ...);
	int __cdecl Format(const char* fmt, ...);

	//decode
	char* GetDecode(){ return (char*)m_pConvert.GetBuf(); };
	char* Convert( UINT cp );
	const char* ToAnsi() { return Convert( CP_ACP ); }
	const char* ToUtf8() { return Convert( CP_UTF8 ); }

	static const CStdString FromAnsi( char* str, int len=-1 );

	//hash
	unsigned long Hash(){ return HashByte( m_buf.GetBuf(), GetLength() * sizeof( wchar_t ) ); }
	static unsigned long HashByte( const unsigned char* buf, unsigned long nlen );

};
