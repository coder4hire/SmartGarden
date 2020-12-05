#pragma once
#include <string>

namespace OSUtils
{
	bool IsInterfaceOnline(const char* interface);
	std::string GetIfaceIP(const char* ifName);

	void CheckAndSpawnChildProcesses();
	int GetProcIdByName(std::string procName);
	bool IsSynchronizedWithNTP();
	std::string Exec(const char* cmd);
}