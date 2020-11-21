#pragma once

#include <string>

class CDomoticzInterface
{
public:
	CDomoticzInterface(const char* domoticzIP, unsigned short port);
	~CDomoticzInterface();

	bool SendTemperature(int idx, int temp);
	bool SendHumidity(int idx, int hum);

	bool SendTempHumidity(int idx, int temp, int hum);

protected:
	std::string domoticzIP;
	unsigned short port;

	std::string SendHTTPRequest(std::string& request);
	std::string GetIP(std::string& host);
	std::string BuildGetQuery(const std::string&host, const std::string& page);
	int Hum2Status(int hum);
};

