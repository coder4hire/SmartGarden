#include "ScreenBase.h"
#include "wiringpi.h"
#include <unistd.h>
#include <stdio.h>

CLcdDisplay CScreenBase::lcd(4, 20, CLcdDisplay::BITS_4, 29, 28, 21, 7, 27, 26, 0, 0, 0, 0, PIN_BACKLIGHT);

CScreenBase::CScreenBase()
{
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

	printf("Run1!!!\n");
	if (!OnEnter())
	{
		printf("Run2!!!\n");
		return;
	}
	printf("Run3!!!\n");	

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
				if (!OnKeyPress(key))
				{				
					OnLeave();
					return;
				}
			}
			usleep(200000);
		}
		else
		{
			usleep(50000);
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