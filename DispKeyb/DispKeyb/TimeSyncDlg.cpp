#include "TimeSyncDlg.h"
#include <stdlib.h>
#include "OSUtils.h"

CTimeSyncDlg::CTimeSyncDlg() : CScreenBase(false),
oldNtpCheckTime(0),
oldTime(0)
{
}

bool CTimeSyncDlg::OnEnter()
{
	lcd.Clear();
	return CScreenBase::OnEnter();
}

bool CTimeSyncDlg::OnKeyPress(int key)
{
	switch (key)
	{
	case KEY_CANCEL:
		lcd.Clear();
		return false;
	}
	return true;
}

void CTimeSyncDlg::Paint()
{
	lcd.GotoXY(0, 0);
	lcd.PutS("Synchronizing time..");

	time_t now;
	time(&now);
	char strTime[32];
	strftime(strTime, 20, "%H:%M:%S", localtime(&now));
	lcd.GotoXY(6, 1);
	lcd.PutS(strTime);

	lcd.GotoXY(0, 3);
	lcd.PutS("Push Cancel to break");
}

bool CTimeSyncDlg::OnNonPreciseTimer()
{
	time_t now;
	time(&now);

	if (oldTime != now)
	{
		oldTime = now;
		Paint();
	}

	if (oldNtpCheckTime != now)
	{
		oldNtpCheckTime = now;
		if (OSUtils::IsSynchronizedWithNTP())
		{
			lcd.Clear();
			return false;
		}
	}
	return true;
}