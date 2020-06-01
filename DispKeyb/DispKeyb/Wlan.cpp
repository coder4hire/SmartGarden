#include "Wlan.h"

#include <stdlib.h>
#include "OSUtils.h"

CWlan CWlan::Inst;

CWlan::CWlan()
{
}

std::string CWlan::GetWlanIP()
{
	return OSUtils::GetIfaceIP(MAIN_WLAN);
}

void CWlan::Enable()
{
	system("ifconfig " MAIN_WLAN " up");
}

void CWlan::Disable()
{
	system("ifconfig " MAIN_WLAN " down");
}

void CWlan::SetAPMode()
{
	system("nmcli c up AP");
	system("service isc-dhcp-server start");
}

void CWlan::SetClientMode()
{
	system("nmcli c up wifi1");
}


bool CWlan::IsEnabled()
{
	return OSUtils::IsInterfaceOnline(MAIN_WLAN);
}
