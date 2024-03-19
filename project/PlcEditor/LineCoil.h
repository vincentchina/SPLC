#pragma once
#include "BaseCoil.h"

class CLineCoil : public CBaseCoil
{
public:
	enum eLineType{
		eVerticalLine,
		eHorizontalLine
	};
public:
	eLineType m_eType;
private:
	static SIZE m_szCoil;
	static BOOL m_bInit;
	

public:
	CLineCoil( int nRow, int nCol, eLineType eType = eHorizontalLine );
	~CLineCoil(void);

	void Draw( HDC hdc, SIZE &offset );
};

