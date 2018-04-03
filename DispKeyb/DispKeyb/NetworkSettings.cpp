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
	lcd.PutS("Network Setings");
	lcd.GotoXY(0, 1);
	std::string ip = CWlan::Inst.GetWLanIP("wlan0");
	lcd.PutS("wlan0: "); 
	lcd.PutS(ip.c_str());
	lcd.GotoXY(0, 2);
	ip = CWlan::Inst.GetWLanIP("wlan1");
	lcd.PutS("wlan1: ");
	lcd.PutS(ip.c_str());

}
