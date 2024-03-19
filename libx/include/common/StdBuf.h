#pragma once

#include "dllpreHead.h"

class LIBX_API CStdBuf
{
protected:
	unsigned char* m_pBuf;
	unsigned long m_sz;
	unsigned int m_nResever;

public:
	enum { RESEVER_LEN = 16 };

	CStdBuf( unsigned long sz = 0,int nReserver = RESEVER_LEN ) : 
	m_pBuf(NULL),m_sz( 0 ),m_nResever( nReserver )
	{ /*Reserver( sz );*/ }
	
	~CStdBuf()
	{ Empty(); }

	unsigned char* Reserver( unsigned long sz )
	{
		unsigned char* pBuf = NULL;
		unsigned char *pDest,*pSrc, *pSrcEnd;
		if( sz >= m_sz )
		{
			pBuf = new unsigned char[ sz + m_nResever ];
			pSrc = m_pBuf;
			pSrcEnd = m_pBuf + m_sz;
			pDest = pBuf;
			while( pSrc < pSrcEnd )
				*pDest++ = *pSrc++;
			m_sz = sz + m_nResever;
			pSrc = m_pBuf;
			m_pBuf = pBuf;
			delete[] pSrc;
		}
		return m_pBuf;
	}
	void SetReserverSize( int sz ) { m_nResever = sz; }
	int GetReserverSize() { return m_nResever; }

	void Empty()
	{
		if( m_pBuf ) delete[] m_pBuf;
		m_pBuf = NULL;
		m_sz = 0;
	}

	unsigned long GetSize() { return m_sz; }
	unsigned long SetSize( unsigned long sz ) 
	{ 
		unsigned long oldsz = m_sz;
		Reserver( sz ); 
		m_sz = sz; 
		return oldsz;
	}
	const unsigned char* GetBuf() const { return m_pBuf; }
	unsigned char operator[] (int nIndex) const { return m_pBuf[nIndex]; }
};
