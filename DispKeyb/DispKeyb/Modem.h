#pragma once
#include <string>

#define MODEM_IFACE "wwx0c5b8f279a64"

class CModem
{
public:
	std::string GetModemIP();
	void Enable();
	void Disable();
	bool IsEnabled();

	static CModem Inst;

protected:
	CModem();
};

