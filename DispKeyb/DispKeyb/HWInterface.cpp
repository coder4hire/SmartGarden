#include "HWInterface.h"
#include "wiringPi.h"
#include "PinsDefinitions.h"

int CHWInterface::OutNum2Pin[] = { 0,PIN_OUT1,PIN_OUT2,PIN_OUT3,PIN_OUT4 };

CHWInterface CHWInterface::Inst;

CHWInterface::CHWInterface()
{
}

CHWInterface::~CHWInterface()
{
}

bool CHWInterface::SetOutValue(unsigned int port, bool enable)
{
	if (port < 1 || port > 4)
	{
		return false;
	}
	digitalWrite(OutNum2Pin[port], enable);
	return true;
}

bool CHWInterface::GetOutStatus(unsigned int port)
{
	if (port < 1 || port > 4)
	{
		return false;
	}
	return digitalRead(OutNum2Pin[port]);
}
