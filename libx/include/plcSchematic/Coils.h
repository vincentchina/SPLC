#pragma once
#include "BaseCoil.h"

#define NAME_FNC_LEN	30

#define STATE_COIL_CLOSE	1

class LIBX_API CInputCoil : public CBaseCoil		// -| |-
{
public:
	enum eInputType{
		eOpen		=	0x01,
		eClose		=	0x02,
		eUpPluse	=	0x04,
		eDownPluse	=	0x08,

		eFnc		=	0x10,
	};
private:
	UINT m_uState;
	eInputType m_eInputType;
public:
	CInputCoil( eInputType eType,CStdString strName = _T(""),int nOccupation = 1 );
	CInputCoil( const CInputCoil& pCoil );
	~CInputCoil(){}
	void SetState( UINT uState) { m_uState = uState; }
	UINT GetState() { return m_uState; }
	eInputType GetInputType(){ return m_eInputType; }
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};

class LIBX_API COutputCoil : public CBaseCoil	//-()- -[]-
{
public:
	enum eOutputType{
		eCoil,
		eFnc,
	};
private:
	UINT m_uState;
	CStdString m_strArg;
	eOutputType m_eOutputType;
public:
	COutputCoil( eOutputType eType, CStdString strFnc = _T(""), int nOccupation = 1 );
	COutputCoil( const COutputCoil& pCoil );
	~COutputCoil(){}
	void SetState( UINT uState) { m_uState = uState; }
	UINT GetState() { return m_uState; }
	BOOL SetArg( CStdString strArg );
	BOOL IsHaveArg() { return ( !m_strArg.IsEmpty() ); }
	eOutputType GetOutputType(){ return m_eOutputType; }
	CStdString GetArg() { return m_strArg; } 
	BOOL SetFnc( CStdString strFnc );
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};

class LIBX_API CSymbolCoil : public CBaseCoil		//-/-
{
public:
	enum eSymbolType{
		eUpPluse,
		eDownPluse,
		eNot,
		eLable,
	};
private:
	UINT m_uState;
	eSymbolType m_nSymbolType;
public:
	CSymbolCoil( eSymbolType nLineType,CStdString strName = _T("") );
	CSymbolCoil( const CSymbolCoil& pCoil );
	~CSymbolCoil(){}
	void SetState( UINT uState) { m_uState = uState; }
	static void DrawSymbol( LPDrawInfo pInfo,eSymbolType eType, SIZE &posOffset );
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
};


class LIBX_API CNoteCoil : public CBaseCoil{

public:
	CNoteCoil( CStdString strNote = _T("") );
	CNoteCoil( const CNoteCoil& pCoil );
	~CNoteCoil( void ){}
	void Draw(LPDrawInfo pInfo, SIZE &posOffset );
	BOOL SetOutNote( CStdString strNote );
};

class LIBX_API CLinkCoil : public CBaseCoil{
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