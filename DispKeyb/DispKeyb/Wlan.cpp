#include "Wlan.h"

#include <stdlib.h>
#include <unistd.h>
#include "OSUtils.h"
#include "time.h"

#include "LcdDisplay.h"

CWlan CWlan::Inst;

int CWlan::GetCurrentBlink()
{
	time_t now;
	time(&now);
	tm* tmNow = localtime(&now);
	return (tmNow->tm_hour * 3600 + tmNow->tm_min * 60 + tmNow->tm_sec) / blinkIntervalSec;
}

CWlan::CWlan() :
	isWiFiEnabled(IsInterfaceEnabled()),
	blinksCounter(0),
	blinkStartTime(0)
{
}

std::string CWlan::GetWlanIP()
{
	return OSUtils::GetIfaceIP(MAIN_WLAN);
}

bool CWlan::Enable()
{
	if (EnableInterface())
	{
		isWiFiEnabled = true;
		return true;
	}
	return false;
}

bool CWlan::Disable()
{
	if (DisableInterface())
	{
		isWiFiEnabled = false;
		blinksCounter = GetCurrentBlink();
		return true;
	}
	return false;
}

bool CWlan::IsEnabled()
{
	return isWiFiEnabled;
}

bool CWlan::EnableInterface()
{
	//system("ifconfig " MAIN_WLAN " up");
	return system("nmcli radio wifi on") == 0;
}

bool CWlan::DisableInterface()
{
	//system("ifconfig " MAIN_WLAN " down");
	return system("nmcli radio wifi off") == 0;
}

bool CWlan::IsInterfaceEnabled()
{
	return OSUtils::IsInterfaceOnline(MAIN_WLAN);
}

bool CWlan::SetAPMode()
{
	return system("nmcli c down wifi1;nmcli c up AP") == 0 &&
		system("service isc-dhcp-server start") == 0;
}

bool CWlan::SetClientMode()
{
	system("service isc-dhcp-server stop");
	return system("nmcli c down AP;nmcli c up wifi1") == 0;
}

void CWlan::ResetNetworking()
{
	system("nmcli networking off;systemctl stop systemd-resolved.service;resolvectl flush-caches;systemctl stop networking;systemctl stop NetworkManager");
	sleep(1);
	system("systemctl start networking;systemctl start NetworkManager;nmcli networking on;ifup -a;systemctl start systemd-resolved.service") == 0;
}

void CWlan::BlinkWifi()
{
	// Temporary disabled for validation
	// 
	if (!IsEnabled())
	{
		//TODO: Remove these 2 lines, they are for test purposes only
		CLcdDisplay::Inst.GotoXY(18, 0);
		CLcdDisplay::Inst.PutS("*");

	//	int currentBlink = GetCurrentBlink();
	//	if (!blinkStartTime)
	//	{
	//		// Enabling interface for short time, if needed
	//		if (blinksCounter != currentBlink)
	//		{
	//			blinksCounter = currentBlink;
	//			time(&blinkStartTime);
	//			EnableInterface();
	//		}
	//	}
	//	else
	//	{
	//		// Disabling interface if time is over
	//		time_t now;
	//		time(&now);
	//		if (now - blinkStartTime > blinkDurationSec)
	//		{
	//			blinkStartTime = 0;
	//			DisableInterface();
	//		}
	//	}
	}
}
