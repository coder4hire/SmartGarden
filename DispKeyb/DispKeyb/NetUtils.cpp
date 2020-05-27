#include "NetUtils.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

bool NetUtils::IsInterfaceOnline(const char* interface)
{
	struct ifreq ifr;
	int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IP);
	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, interface);
	if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
	{
		perror("SIOCGIFFLAGS");
	}
	close(sock);
	return !!(ifr.ifr_flags & IFF_UP);
}

std::string NetUtils::GetIfaceIP(const char* ifName)
{
	struct ifaddrs* ifaddr, * ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1)
	{
		return "Disabled";
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
