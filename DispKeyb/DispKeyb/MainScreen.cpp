#include "MainScreen.h"
#include "Wlan.h"

CMainScreen::CMainScreen()
{
	oldTime = 0;
}


CMainScreen::~CMainScreen()
{
}

bool CMainScreen::OnKeyPress(int key)
{
	return key != KEY_CANCEL;
}

void CMainScreen::OnNonPreciseTimer()
{
	Paint();
}

void CMainScreen::Paint()
{
	time_t now;
	time(&now);

	if (oldTime != now)
	{
		oldTime = now;
		char strTime[20];
		strftime(strTime, 20, "%H:%M", localtime(&now));
		lcd.GotoXY(0, 0);
		lcd.Printf(strTime);

		lcd.GotoXY(0, 3);
		lcd.Printf(CWlan::Inst.IsEnabled() ? "WiFi" : "----");
	}
}
