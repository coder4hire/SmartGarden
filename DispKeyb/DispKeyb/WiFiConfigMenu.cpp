#include "WiFiConfigMenu.h"
#include "Wlan.h"

CWiFiConfigMenu::CWiFiConfigMenu(): CMenuBase(1)
{
	menuItems.push_back(CMenuItem("AP mode", NULL));
	menuItems.push_back(CMenuItem("Client mode", NULL));
	menuItems.push_back(CMenuItem("Cancel", NULL));
	currentPosition = 2;
}

CWiFiConfigMenu::~CWiFiConfigMenu()
{
}

void CWiFiConfigMenu::Paint()
{
	CMenuBase::Paint();
	lcd.GotoXY(0, 0);
	lcd.PutS("Set Wifi Mode");
}

bool CWiFiConfigMenu::OnMenuItemSelected(int index)
{
	switch (index)
	{
	case 0: CWlan::Inst.SetAPMode();
		break;
	case 1: CWlan::Inst.SetClientMode();
		break;
	case 2:
		break;
	}
	return false;
}
