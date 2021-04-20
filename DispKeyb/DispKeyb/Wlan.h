#pragma once
#include <string>

#define MAIN_WLAN "wlx000b81a27273"

class CWlan
{
public:
	std::string GetWlanIP();
	void Enable();
	void Disable();
	bool IsEnabled();
	void EnableInterface();
	void DisableInterface();
	bool IsInterfaceEnabled();
	void SetAPMode();
	void SetClientMode();
	void ResetNetworking();
	void BlinkWifi();

	static CWlan Inst;

protected:
	bool isWiFiEnabled;
	const int blinkDurationSec = 3600;
	const int blinkIntervalSec = 180;
	int blinksCounter;
	time_t blinkStartTime;
	int GetCurrentBlink();
	CWlan();	
};

