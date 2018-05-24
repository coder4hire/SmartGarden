#pragma once
#include "ScreenBase.h"

class CMainScreen : public CScreenBase
{
public:
	CMainScreen();
	~CMainScreen();

	virtual bool OnKeyPress(int key);
	virtual void Paint();
	void OnNonPreciseTimer();

public:
	time_t oldTime;
};

