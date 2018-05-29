#include "NetworkSettings.h"
#include "Wlan.h"
#include "MessageBox.h"


CNetworkSettings::CNetworkSettings()
{
}


CNetworkSettings::~CNetworkSettings()
{
}

bool CNetworkSettings::OnKeyPress(int key)
{
	if (key == KEY_OK)
	{
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
	}
	return key != KEY_CANCEL;
}

void CNetworkSettings::Paint()
{
	lcd.Clear();
	lcd.PutS("Network Setings");
	lcd.GotoXY(0, 1);
	bool isEnabled = CWlan::Inst.IsEnabled();
	if (isEnabled)
	{
		std::string ip = CWlan::Inst.GetWLanIP("wlan0");
		lcd.PutS("w0: ");
		lcd.PutS(ip.c_str());
	}
	else
	{
		lcd.PutS("w0: disabled");
	}

	lcd.GotoXY(0, 2);
	std::string ip = CWlan::Inst.GetWLanIP("wlan1");
	lcd.PutS("w1: ");
	lcd.PutS(ip.c_str());
	lcd.GotoXY(2, 3);
	lcd.PutS(isEnabled ? "Ent-disable WiFi" : " Enter-enable WiFi");
}
