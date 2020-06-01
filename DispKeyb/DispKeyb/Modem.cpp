#include "Modem.h"

#include <stdlib.h>
#include "OSUtils.h"

CModem CModem::Inst;

CModem::CModem()
{

}

std::string CModem::GetModemIP()
{
	return OSUtils::GetIfaceIP(MODEM_IFACE);
}

void CModem::Enable()
{
	system("nmcli connection up mdm");
}

void CModem::Disable()
{
	system("nmcli connection down mdm");
}


bool CModem::IsEnabled()
{
	return OSUtils::IsInterfaceOnline(MODEM_IFACE);
}
