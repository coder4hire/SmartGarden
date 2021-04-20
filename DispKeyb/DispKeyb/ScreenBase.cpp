#include "ScreenBase.h"
#include "wiringpi.h"
#include "PinsDefinitions.h"
#include "Wlan.h"
#include <unistd.h>
#include <stdio.h>

CScreenBase::CScreenBase(bool isLCDSaverEnabled): lcd(CLcdDisplay::Inst)
{
	TimerSkipCount = 5;
	timerCount = TimerSkipCount;
	this->IsLCDSaverEnabled = isLCDSaverEnabled;
}


CScreenBase::~CScreenBase()
{
}

bool CScreenBase::OnKeyPress(int key)
{

}

void CScreenBase::Run()
{
	lcd.Clear();

	if (!OnEnter())
	{
		return;
	}

	lcd.TurnBacklightOn();
	while (true)
	{
		int key = GetKey();

		if (key)
		{
			bool oldLCDStatus = lcd.IsBacklightOn();
			lcd.CheckDisplayBacklight(true);
			if (oldLCDStatus)
			{
				usleep(200000);
				if (!OnKeyPress(key))
				{				
					OnLeave();
					return;
				}
			}
		}
		else
		{
			usleep(50000);
		}

		if (--timerCount <= 0)
		{
			timerCount = TimerSkipCount;
			if (!OnNonPreciseTimer())
			{
				OnLeave();
				return;
			}
		}

		if (IsLCDSaverEnabled)
		{
			lcd.CheckDisplayBacklight(false);
		}
	}
}

bool CScreenBase::OnNonPreciseTimer()
{
	CWlan::Inst.BlinkWifi();
	return true;
}

int CScreenBase::GetKey()
{
	int retVal = 0;
	retVal = (!digitalRead(PINKEY_UP)) * KEY_UP +
		(!digitalRead(PINKEY_DOWN)) * KEY_DOWN +
		(!digitalRead(PINKEY_OK)) * KEY_OK +
		(!digitalRead(PINKEY_CANCEL)) * KEY_CANCEL +
		(!digitalRead(PINKEY_ABORT)) * KEY_ABORT;
}