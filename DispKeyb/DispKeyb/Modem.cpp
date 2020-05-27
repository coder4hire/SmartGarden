#include "Modem.h"

#include <stdlib.h>
#include "NetUtils.h"

CModem CModem::Inst;

CModem::CModem()
{

}

std::string CModem::GetModemIP()
{
	return NetUtils::GetIfaceIP(MODEM_IFACE);
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
	return NetUtils::IsInterfaceOnline(MODEM_IFACE);
}
