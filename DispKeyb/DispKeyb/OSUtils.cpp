#include "OSUtils.h"
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
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <fstream>
#include <array>
#include <memory>

bool OSUtils::IsInterfaceOnline(const char* interface)
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

std::string OSUtils::GetIfaceIP(const char* ifName)
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

void OSUtils::CheckAndSpawnChildProcesses()
{
	if (GetProcIdByName("CameraServer") == -1)
	{
		printf("CameraServer is not running, restarting it\n");
		system("./CameraServer&");
	}

	if (GetProcIdByName("domoticz") == -1)
	{
		printf("Domoticz is not running, restarting it\n");
		system("../domoticz/domoticz&");
	}

}

int OSUtils::GetProcIdByName(std::string procName)
{
	int pid = -1;

	// Open the /proc directory
	DIR* dp = opendir("/proc");
	if (dp != NULL)
	{
		// Enumerate all entries in directory until process found
		struct dirent* dirp;
		while (pid < 0 && (dirp = readdir(dp)))
		{
			// Skip non-numeric entries
			int id = atoi(dirp->d_name);
			if (id > 0)
			{
				// Read contents of virtual /proc/{pid}/cmdline file
				std::string cmdPath = std::string("/proc/") + dirp->d_name + "/cmdline";
				std::ifstream cmdFile(cmdPath.c_str());
				std::string cmdLine;
				getline(cmdFile, cmdLine);
				if (!cmdLine.empty())
				{
					// Keep first cmdline item which contains the program path
					size_t pos = cmdLine.find('\0');
					if (pos != std::string::npos)
						cmdLine = cmdLine.substr(0, pos);
					// Keep program name only, removing the path
					pos = cmdLine.rfind('/');
					if (pos != std::string::npos)
						cmdLine = cmdLine.substr(pos + 1);
					// Compare against requested process name
					if (procName == cmdLine)
						pid = id;
				}
			}
		}
	}

	closedir(dp);

	return pid;
}

bool OSUtils::IsSynchronizedWithNTP()
{
	std::string output = Exec("timedatectl");
	return output.find("synchronized: yes") != std::string::npos;
}

std::string OSUtils::Exec(const char* cmd) 
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe)
	{
		return "";
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	return result;
}