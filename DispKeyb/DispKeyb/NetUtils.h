#pragma once
#include <string>

namespace NetUtils
{
	bool IsInterfaceOnline(const char* interface);
	std::string GetIfaceIP(const char* ifName);
}