#pragma once
#include "ScreenBase.h"

class CMainScreen : public CScreenBase
{
public:
	CMainScreen();
	~CMainScreen();

	virtual bool OnEnter();
	virtual bool OnKeyPress(int key);
	virtual void Paint();
	void OnNonPreciseTimer();

public:
	time_t oldTime;
};

