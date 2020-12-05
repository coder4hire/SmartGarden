#pragma once
#include "ScreenBase.h"

#define NTP_CHECK_INTERVAL_SEC 5

class CTimeSyncDlg : public CScreenBase
{
public:
	CTimeSyncDlg();
	virtual bool OnEnter();
	virtual bool OnKeyPress(int key);
	virtual void Paint();
	virtual bool OnNonPreciseTimer();

protected:
	time_t oldTime;
	time_t oldNtpCheckTime;
};
