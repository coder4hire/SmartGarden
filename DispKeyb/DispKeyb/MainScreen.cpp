#include "MainScreen.h"
#include "Wlan.h"
#include "DomoticzDataParser.h"
#include "stdio.h"
#include "MainMenu.h"

CMainScreen::CMainScreen()
{
	oldTime = 0;
}


CMainScreen::~CMainScreen()
{
}

bool CMainScreen::OnEnter()
{
	CDomoticzDataParser::Inst.RefreshDataIfNeeded();
	return CScreenBase::OnEnter();
}

bool CMainScreen::OnKeyPress(int key)
{
	switch (key)
	{
	case KEY_OK:
	{
		CMainMenu menu;
		menu.Run();
		break;
	}
	case KEY_CANCEL | KEY_DOWN:
		CWlan::Inst.Disable();
		Paint();
		break;
	case KEY_CANCEL | KEY_UP:
		CWlan::Inst.Enable();
		Paint();
		break;
	}
	return true;
}

void CMainScreen::OnNonPreciseTimer()
{
	CDomoticzDataParser::Inst.RefreshDataIfNeeded();
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

		lcd.GotoXY(0, 3);
		lcd.Printf(CWlan::Inst.IsEnabled() ? "WiFi" : "----");

		char strTemp[20];
		sprintf(strTemp, "R:%5.1lf", CDomoticzDataParser::Inst.RoomTemp);
		lcd.GotoXY(13, 0);
		lcd.Printf(strTemp);

		sprintf(strTemp, "O:%5.1lf", CDomoticzDataParser::Inst.OutsideTemp);
		lcd.GotoXY(13, 1);
		lcd.Printf(strTemp);

	}
}
