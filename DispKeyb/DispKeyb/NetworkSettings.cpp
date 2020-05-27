#include "NetworkSettings.h"
#include "Wlan.h"
#include "MessageBox.h"
#include "WiFiConfigMenu.h"
#include "Modem.h"

CNetworkSettings::CNetworkSettings(): CMenuBase(1)
{
	menuItems.push_back(CMenuItem("Modem:", NULL));
	menuItems.push_back(CMenuItem("Wifi:", NULL));
	menuItems.push_back(CMenuItem("Switch Wifi Mode", new CWiFiConfigMenu));
	currentPosition = 1;
}

CNetworkSettings::~CNetworkSettings()
{
}

bool CNetworkSettings::OnMenuItemSelected(int index)
{
	long vol = 0;
	switch (index)
	{
	case 0: // Control Modem
		if (CModem::Inst.IsEnabled())
		{
			if (CMessageBox::Inst.Show("Disable Modem ?", this))
			{
				CModem::Inst.Disable();
			}
		}
		else
		{
			if (CMessageBox::Inst.Show("Enable Modem ?", this))
			{
				CModem::Inst.Enable();
			}
		}
		Paint();
		break;

	case 1: // Control WiFi
		if (CWlan::Inst.IsEnabled())
		{
			if (CMessageBox::Inst.Show("Disable WiFi ?", this))
			{
				CWlan::Inst.Disable();
			}
		}
		else
		{
			if (CMessageBox::Inst.Show("Enable WiFi ?", this))
			{
				CWlan::Inst.Enable();
			}
		}
		Paint();
		break;
	}
	return true;
}


void CNetworkSettings::Paint()
{
	bool isWlanEnabled = CWlan::Inst.IsEnabled();
	bool isModemEnabled = CModem::Inst.IsEnabled();

	menuItems[0].Name = std::string("MD ") + (isModemEnabled ? CModem::Inst.GetModemIP() : std::string("Disabled"));
	menuItems[1].Name = std::string("WF ") + (isWlanEnabled ? CWlan::Inst.GetWlanIP() : std::string("Disabled"));
	
	CMenuBase::Paint();
	lcd.GotoXY(0, 0);
	lcd.PutS("Network (");
	if(isWlanEnabled)
	{ 
		lcd.PutS("WiFi");
	}
	lcd.PutS("/");
	if (isModemEnabled)
	{
		lcd.PutS("Mdm");
	}
	lcd.PutS(")");

	lcd.GotoXY(0, 1);

/*	lcd.GotoXY(0, 2);
	std::string ip = CWlan::Inst.GetWLanIP("wlan1");
	lcd.PutS("w1: ");
	lcd.PutS(ip.c_str());*/
}
