#include "NetworkSettings.h"
#include "Wlan.h"
#include "MessageBox.h"
#include "WiFiConfigMenu.h"
#include "Modem.h"

CNetworkSettings::CNetworkSettings(): CMenuBase(0)
{
	menuItems.push_back(CMenuItem("Network:", NULL));
	menuItems.push_back(CMenuItem("Modem:", NULL));
	menuItems.push_back(CMenuItem("Wifi:", NULL));
	menuItems.push_back(CMenuItem("Switch Wifi Mode", new CWiFiConfigMenu));
	currentPosition = 2;
}

CNetworkSettings::~CNetworkSettings()
{
}

bool CNetworkSettings::OnMenuItemSelected(int index)
{
	long vol = 0;
	switch (index)
	{
	case 0:
		if (CMessageBox::Inst.Show("Reset Network ?", this))
		{
			CWlan::Inst.ResetNetworking();
		}
		break;

	case 1: // Control Modem
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

	case 2: // Control WiFi
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

	menuItems[0].Name = std::string("Network (") + (isWlanEnabled ? "WiFi/" : "/") +
		(isModemEnabled ? "Mdm)" : ")");
	menuItems[1].Name = std::string("MD ") + (isModemEnabled ? CModem::Inst.GetModemIP() : std::string("Disabled"));
	menuItems[2].Name = std::string("WF ") + (isWlanEnabled ? CWlan::Inst.GetWlanIP() : std::string("Disabled"));
	
	CMenuBase::Paint();
}
