#pragma once
#include "BaseCoil.h"

#define NAME_FNC_LEN	30

class CInputCoil : public CBaseCoil		// -| |-
{
public:
	enum eInputType{
		eOpen,
		eClose,
		eUpPluse,
		eDownPluse,
	};
private:
	eInputType m_eInputType;
public:
	CInputCoil( eInputType eType,LPTSTR strName = NULL );
	CInputCoil( const CInputCoil& pCoil );
	~CInputCoil();
	eInputType GetInputType(){ return m_eInputType; }
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};

class COutputCoil : public CBaseCoil	//-()- -[]-
{
public:
	enum eOutputType{
		eCoil,
		eFnc,
	};
private:
	LPTSTR m_strArg;
	eOutputType m_eOutputType;
public:
	COutputCoil( eOutputType eType, LPTSTR strFnc = NULL, int nOccupation = 1 );
	COutputCoil( const COutputCoil& pCoil );
	~COutputCoil();
	BOOL SetArg( LPTSTR strArg );
	BOOL IsHaveArg() { return ( m_strArg != NULL ); }
	eOutputType GetOutputType(){ return m_eOutputType; }
	LPTSTR GetArg() { return m_strArg; } 
	BOOL SetFnc( LPTSTR strFnc );
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};

class CSymbolCoil : public CBaseCoil		//-/-
{
public:
	enum eSymbolType{
		eUpPluse,
		eDownPluse,
		eNot,
		eLable,
	};
private:
	eSymbolType m_nSymbolType;
public:
	CSymbolCoil( eSymbolType nLineType,LPSTR strName = NULL );
	CSymbolCoil( const CSymbolCoil& pCoil );
	~CSymbolCoil();
	static void DrawSymbol( LPDrawInfo pInfo,eSymbolType eType, SIZE &posOffset );
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};


class CNoteCoil : public CBaseCoil{

public:
	CNoteCoil( LPTSTR strNote = NULL );
	CNoteCoil( const CNoteCoil& pCoil );
	~CNoteCoil( void ){}
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
	BOOL SetOutNote( LPTSTR strNote );
};

class CLinkCoil : public CBaseCoil{
public:
	enum eLinkType{
		eIn,
		eOut,
	};
private:
	eLinkType m_nLinkType;
	int m_nID;
public:
	CLinkCoil( eLinkType eType, int nID );
	CLinkCoil( const CLinkCoil& pCoil );
	~CLinkCoil( void ){}
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};