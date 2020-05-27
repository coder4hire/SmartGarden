#pragma once
#include <string>

#define MAIN_WLAN "wlx000b81a27273"

class CWlan
{
public:
	std::string GetWlanIP();
	void Enable();
	void Disable();
	void SetAPMode();
	void SetClientMode();
	bool IsEnabled();

	static CWlan Inst;

protected:
	CWlan();	
};

