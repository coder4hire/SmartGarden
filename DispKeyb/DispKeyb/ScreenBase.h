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
	CScreenBase(bool isLCDSaverEnabled = true);
	virtual ~CScreenBase();

	virtual bool OnKeyPress(int key);
	virtual void Run();
	virtual void Paint() {};

	virtual bool OnNonPreciseTimer();

	virtual bool OnEnter() { Paint(); return true; }
	virtual void OnLeave() {}

	int GetKey();

	int TimerSkipCount;
	bool IsLCDSaverEnabled = true;

protected:
	int timerCount;
	CLcdDisplay& lcd;
};

