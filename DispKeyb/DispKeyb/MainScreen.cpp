#include "MainScreen.h"
#include "Wlan.h"
#include "DomoticzDataParser.h"
#include "stdio.h"
#include "MainMenu.h"
#include "RestartDlg.h"
#include "OSUtils.h"

CMainScreen::CMainScreen()
{
	oldTime = 0;
}


CMainScreen::~CMainScreen()
{
}

bool CMainScreen::OnEnter()
{
	OSUtils::CheckAndSpawnChildProcesses();
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
	case KEY_ABORT | KEY_CANCEL | KEY_OK:
		{
			CRestartDlg dlg;
			dlg.Run();
			lcd.Clear();
			Paint();
		}
	}
	return true;
}

bool CMainScreen::OnNonPreciseTimer()
{
	Paint();
	return CScreenBase::OnNonPreciseTimer();
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
		lcd.Printf(CWlan::Inst.IsEnabled() ? "WiFi" : (CWlan::Inst.IsInterfaceEnabled() ? "WTmp" : "----"));

		//--- Synchronized data
		SensorsData data = CDomoticzDataParser::Inst.GetSensorsData();
		char strTemp[20];
		sprintf(strTemp, "R:%5.1lf", data.RoomTemp);
		lcd.GotoXY(13, 0);
		lcd.Printf(strTemp);

		sprintf(strTemp, "O:%5.1lf", data.OutsideTemp);
		lcd.GotoXY(13, 1);
		lcd.Printf(strTemp);
	}
}
