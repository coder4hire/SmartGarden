#pragma once

#include <string>

class CDomoticzInterface
{
public:
	CDomoticzInterface(const char* domoticzIP, unsigned short port);
	~CDomoticzInterface();

	bool SendSwitchCommand(int idx, bool isEnabled);

protected:
	std::string domoticzIP;
	unsigned short port;

	std::string SendHTTPRequest(std::string& request);
	std::string GetIP(std::string& host);
	std::string BuildGetQuery(const std::string&host, const std::string& page);
};

