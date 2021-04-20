#include "Wlan.h"

#include <stdlib.h>
#include <unistd.h>
#include "OSUtils.h"
#include "time.h"

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

void CWlan::Enable()
{
	isWiFiEnabled = true;
	EnableInterface();
}

void CWlan::Disable()
{
	isWiFiEnabled = false;
	DisableInterface();
	blinksCounter = GetCurrentBlink();
}

bool CWlan::IsEnabled()
{
	return isWiFiEnabled;
}

void CWlan::EnableInterface()
{
	//system("ifconfig " MAIN_WLAN " up");
	system("nmcli radio wifi on");
}

void CWlan::DisableInterface()
{
	//system("ifconfig " MAIN_WLAN " down");
	system("nmcli radio wifi off");
}

bool CWlan::IsInterfaceEnabled()
{
	return OSUtils::IsInterfaceOnline(MAIN_WLAN);
}

void CWlan::SetAPMode()
{
	system("nmcli c up AP");
	system("service isc-dhcp-server start");
}

void CWlan::SetClientMode()
{
	system("service isc-dhcp-server stop");
	system("nmcli c up wifi1");
}

void CWlan::ResetNetworking()
{
	system("nmcli networking off");
	sleep(1);
	system("nmcli networking on");
}

void CWlan::BlinkWifi()
{
	if (!IsEnabled())
	{
		int currentBlink = GetCurrentBlink();
		if (!blinkStartTime)
		{
			// Enabling interface for short time, if needed
			if (blinksCounter != currentBlink)
			{
				blinksCounter = currentBlink;
				time(&blinkStartTime);
				EnableInterface();
			}
		}
		else
		{
			// Disabling interface if time is over
			time_t now;
			time(&now);
			if (now - blinkStartTime > blinkDurationSec)
			{
				blinkStartTime = 0;
				DisableInterface();
			}
		}
	}
}
