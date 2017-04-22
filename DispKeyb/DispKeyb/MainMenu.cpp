#include "MainMenu.h"
#include "wiringpi.h"
#include <unistd.h>

#define MAX_SCREENS 5

CMainMenu::CMainMenu() :
	lcd(4, 20, CLcdDisplay::BITS_4, 29, 28, 21, 7, 27, 26, 0, 0, 0, 0)
{
	screenNum=0;
}


CMainMenu::~CMainMenu()
{
}

int CMainMenu::Run()
{
	TurnDisplayOn();
	while (true)
	{
		int key = GetKey();
		if (key&KEY_UP)
		{
			screenNum = screenNum ? screenNum - 1 : MAX_SCREENS;
		}
		if (key&KEY_DOWN)
		{
			screenNum = (screenNum + 1) % MAX_SCREENS;
		}

		lcd.GotoXY(0, 0);
		lcd.Printf("Screen: %d    ", screenNum);
		lcd.GotoXY(0, 1);
		if (key&KEY_UP)
			lcd.Printf("UP ");
		if (key&KEY_DOWN)
			lcd.Printf("DOWN ");
		if (key&KEY_OK)
			lcd.Printf("OK ");
		if (key&KEY_CANCEL)
			lcd.Printf("CANCEL ");
		if (key&KEY_ABORT)
			lcd.Printf("ABORT ");
		usleep(200000);
	}

	return 0;
}

// Pins: 5,18,3,16,2
int CMainMenu::GetKey()
{
	int retVal = 0;
	retVal = (!digitalRead(PINKEY_UP)) * KEY_UP +
		(!digitalRead(PINKEY_DOWN)) * KEY_DOWN +
		(!digitalRead(PINKEY_OK)) * KEY_OK +
		(!digitalRead(PINKEY_CANCEL)) * KEY_CANCEL +
		(!digitalRead(PINKEY_ABORT)) * KEY_ABORT;
}

void CMainMenu::TurnDisplayOn()
{
	digitalWrite(PIN_BACKLIGHT, 1);
}
