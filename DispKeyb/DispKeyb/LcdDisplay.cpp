#include "LcdDisplay.h"
#include <wiringPi.h>
#include <lcd.h>
#include <stdio.h>
#include <stdarg.h>

CLcdDisplay::CLcdDisplay(const int rows, const int cols, BusWidth bits,
	const int rs, const int en,
	const int d0, const int d1, const int d2, const int d3, const int d4,
	const int d5, const int d6, const int d7)
{
	lcdHandle = lcdInit(rows, cols, bits==BITS_4 ? 4 : 8, rs, en, d0, d1, d2, d3, d4, d5, d6, d7);

	if (lcdHandle < 0)
	{
		fprintf(stderr,"lcdInit failed\n");
		return;
	}
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