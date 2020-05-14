#include "LcdDisplay.h"
#include <wiringPi.h>
#include <lcd.h>
#include <stdio.h>
#include <stdarg.h>

CLcdDisplay CLcdDisplay::Inst;

bool CLcdDisplay::Init(const int rows, const int cols, BusWidth bits,
	const int rs, const int en,
	const int d0, const int d1, const int d2, const int d3, const int d4,
	const int d5, const int d6, const int d7, const int backlightPin)
{
	lcdHandle = lcdInit(rows, cols, bits==BITS_4 ? 4 : 8, rs, en, d0, d1, d2, d3, d4, d5, d6, d7);
	this->backlightPin = backlightPin;
	lastTimeDisplayOn = 0;
	DisplayTimeout = 15;

	if (lcdHandle < 0)
	{
		fprintf(stderr,"lcdInit failed\n");
		return false;
	}
	printf("LCD is initialized\n");
	return true;
}

bool CLcdDisplay::InitDefault()
{
	return Init(4, 20, CLcdDisplay::BITS_4, 29, 28, 21, 7, 27, 26, 0, 0, 0, 0, PIN_BACKLIGHT);
}

void CLcdDisplay::Printf(const char *message, ...)
{
	va_list argp;
	char buffer[1024];

	va_start(argp, message);
	vsnprintf(buffer, 1023, message, argp);
	va_end(argp);

	lcdPuts(lcdHandle, buffer);
}


CLcdDisplay::~CLcdDisplay()
{
}

void CLcdDisplay::TurnBacklightOn()
{
	if (backlightPin)
	{
		time(&lastTimeDisplayOn);
		if (!digitalRead(backlightPin))
		{
			digitalWrite(backlightPin, 1);
		}
	}
}

void CLcdDisplay::TurnBacklightOff()
{
	if (backlightPin)
	{
		lastTimeDisplayOn = 0;
		digitalWrite(backlightPin, 0);
	}
}

void CLcdDisplay::CheckDisplayBacklight(bool turnOn)
{
	if (turnOn)
	{
		TurnBacklightOn();
	}
	else
	{
		time_t now;
		time(&now);
		if (lastTimeDisplayOn && lastTimeDisplayOn + DisplayTimeout < now)
		{
			TurnBacklightOff();
		}
	}
}
