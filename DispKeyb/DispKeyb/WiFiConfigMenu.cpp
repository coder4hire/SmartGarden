#include "WiFiConfigMenu.h"
#include "Wlan.h"
#include "MessageBox.h"

CWiFiConfigMenu::CWiFiConfigMenu() : CMenuBase(1)
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
	case 0:
		if (!CWlan::Inst.SetAPMode())
		{
			CMessageBox::Inst.Show("Can't enable AP", this, false);
		}
		break;
	case 1: 
		if (!CWlan::Inst.SetClientMode())
		{
			CMessageBox::Inst.Show("Can't enable Client", this, false);
		}
		break;
	case 2:
		break;
	}
	return false;
}
