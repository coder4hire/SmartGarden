#include "NetworkSettings.h"
#include "Wlan.h"



CNetworkSettings::CNetworkSettings()
{
}


CNetworkSettings::~CNetworkSettings()
{
}

bool CNetworkSettings::OnKeyPress(int key)
{
	return key != KEY_CANCEL;
}

void CNetworkSettings::Paint()
{
	lcd.Clear();
	std::string ip = CWlan::Inst.GetWLanIP();
	lcd.PutS(ip.c_str());
}
