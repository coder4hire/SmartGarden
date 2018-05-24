#include "ScreenBase.h"
#include "wiringpi.h"
#include <unistd.h>
#include <stdio.h>

CScreenBase::CScreenBase(): lcd(CLcdDisplay::Inst)
{
	TimerSkipCount = 5;
	timerCount = TimerSkipCount;
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
			OnNonPreciseTimer();
		}

		lcd.CheckDisplayBacklight(false);
	}
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