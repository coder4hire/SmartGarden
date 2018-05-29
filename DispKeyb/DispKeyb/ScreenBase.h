#pragma once

#include "LcdDisplay.h"

enum EKeys
{
	KEY_UP = 1,
	KEY_DOWN = 2,
	KEY_OK = 4,
	KEY_CANCEL = 8,
	KEY_ABORT = 16
};

class CScreenBase
{
public:
	CScreenBase();
	virtual ~CScreenBase();

	virtual bool OnKeyPress(int key);
	virtual void Run();
	virtual void Paint() {};

	virtual void OnNonPreciseTimer() {};

	virtual bool OnEnter() { Paint(); return true; }
	virtual void OnLeave() {}

	int GetKey();

	int TimerSkipCount;

protected:
	int timerCount;
	CLcdDisplay& lcd;
};

