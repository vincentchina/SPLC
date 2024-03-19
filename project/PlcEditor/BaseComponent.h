#pragma once
class CBaseComponent
{
public:
	CBaseComponent(void);
	virtual ~CBaseComponent(void);

	void OnDraw(HDC hdc);
};

