#pragma once
#include "LcdDisplay.h"

enum EKeys
{
	KEY_UP=1,
	KEY_DOWN=2,
	KEY_OK=4,
	KEY_CANCEL=8,
	KEY_ABORT=16
};

enum EKeysPins
{
	PINKEY_ABORT = 9,		// pin 5
	PINKEY_CANCEL = 5,	//pin 18
	PINKEY_UP = 8,		//pin 3,
	PINKEY_DOWN = 4,	//pin 16,
	PINKEY_OK = 1	//pin 12
};

#define PIN_BACKLIGHT 15


class CMainMenu
{
public:
	CMainMenu();
	~CMainMenu();
	int Run();
	int GetKey();
	void TurnDisplayOn();

protected:
	CLcdDisplay lcd;
	int screenNum;

};

