#include "Wlan.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


CWlan CWlan::Inst;

CWlan::CWlan()
{
}


CWlan::~CWlan()
{
}

std::string CWlan::GetWLanIP(const char* ifName)
{
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1)
	{
		return "Err";
	}


	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if (!s && (strcmp(ifa->ifa_name, ifName) == 0) && (ifa->ifa_addr->sa_family == AF_INET))
		{
			freeifaddrs(ifaddr);
			return host;
		}
	}

	freeifaddrs(ifaddr);
	return "";
}

void CWlan::Enable()
{
}

void CWlan::Disable()
{
}

bool CWlan::IsEnabled()
{
	return false;
}
