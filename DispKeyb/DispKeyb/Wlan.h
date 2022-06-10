#pragma once
#include <string>

#define MAIN_WLAN "wlx000b81a27273"

class CWlan
{
public:
	std::string GetWlanIP();
	bool Enable();
	bool Disable();
	bool IsEnabled();
	bool EnableInterface();
	bool DisableInterface();
	bool IsInterfaceEnabled();
	bool SetAPMode();
	bool SetClientMode();
	void ResetNetworking();
	void BlinkWifi();

	static CWlan Inst;

protected:
	bool isWiFiEnabled;
	const int blinkDurationSec = 180;
	const int blinkIntervalSec = 3600;
	int blinksCounter;
	time_t blinkStartTime;
	int GetCurrentBlink();
	CWlan();	
};

