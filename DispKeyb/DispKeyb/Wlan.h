#pragma once
#include <string>

class CWlan
{
public:
	~CWlan();

	std::string GetWLanIP(const char* ifName);
	void Enable();
	void Disable();
	bool IsEnabled();

	static CWlan Inst;

protected:
	CWlan();
};

