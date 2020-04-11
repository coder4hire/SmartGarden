#include "NetworkSettings.h"
#include "Wlan.h"
#include "MessageBox.h"
#include "WiFiConfigMenu.h"


CNetworkSettings::CNetworkSettings(): CMenuBase(2)
{
	menuItems.push_back(CMenuItem("Disable Wifi", NULL));
	menuItems.push_back(CMenuItem("Switch Wifi Mode", new CWiFiConfigMenu));
}

CNetworkSettings::~CNetworkSettings()
{
}

bool CNetworkSettings::OnMenuItemSelected(int index)
{
	long vol = 0;
	switch (index)
	{
	case 0: // Volume up
		if (CWlan::Inst.IsEnabled())
		{
			if (CMessageBox::Inst.Show("Disable WiFi ?", this))
			{
				CWlan::Inst.Disable();
				Paint();
			}
		}
		else
		{
			if (CMessageBox::Inst.Show("Enable WiFi ?", this))
			{
				CWlan::Inst.Enable();
				Paint();
			}
		}
		break;
	}
	return true;
}


void CNetworkSettings::Paint()
{
	bool isEnabled = CWlan::Inst.IsEnabled();
	menuItems[0].Name = isEnabled ? "Disable WiFi" : "Enable WiFi";
	CMenuBase::Paint();
	lcd.GotoXY(0, 0);
	lcd.PutS("Network Setings");
	lcd.GotoXY(0, 1);

	if (isEnabled)
	{
		std::string ip = CWlan::Inst.GetWLanIP(MAIN_WLAN);
		lcd.PutS("w0: ");
		lcd.PutS(ip.c_str());
	}
	else
	{
		lcd.PutS("w0: disabled");
	}

/*	lcd.GotoXY(0, 2);
	std::string ip = CWlan::Inst.GetWLanIP("wlan1");
	lcd.PutS("w1: ");
	lcd.PutS(ip.c_str());*/
}
