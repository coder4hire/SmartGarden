#pragma once
#include <string>

#define MAIN_WLAN "wlx000b81a27273"

class CWlan
{
public:
	~CWlan();

	std::string GetWLanIP(const char* ifName);
	void Enable();
	void Disable();
	void SetAPMode();
	void SetClientMode();
	bool IsEnabled();

	static CWlan Inst;

protected:
	CWlan();
	
	bool IsInterfaceOnline(const char* interface);
};

