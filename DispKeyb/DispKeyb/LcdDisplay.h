#pragma once
#include <lcd.h>
#include <time.h>

class CLcdDisplay
{
public:
	enum BusWidth { BITS_4, BITS_8 };
	CLcdDisplay(const int rows, const int cols, BusWidth bits,
		const int rs, const int en,
		const int d0, const int d1, const int d2, const int d3, const int d4,
		const int d5, const int d6, const int d7,const int backlightPin);
	~CLcdDisplay();
	void GotoXY(int x,int y){ lcdPosition(lcdHandle, x, y); }
	void PutChar(char ch) { lcdPutchar(lcdHandle, ch); }
	void PutS(const char* str){ lcdPuts(lcdHandle, str); }
	void Home() { lcdHome(lcdHandle); }
	void Clear() { lcdClear(lcdHandle); }
	void Display(bool enabled){ lcdDisplay(lcdHandle, enabled); }
	void Cursor(bool enabled) { lcdCursor(lcdHandle, enabled); }
	void CursorBlink(bool enabled) { lcdCursorBlink(lcdHandle, enabled); }
	void SendCommand(unsigned char command) { lcdSendCommand(lcdHandle, command); }
	void DefineChar(int index, unsigned char data[8]) { lcdCharDef(lcdHandle, index, data); }
	void Printf(const char *message, ...);

	void TurnBacklightOn();
	void TurnBacklightOff();
	void CheckDisplayBacklight(bool turnOn);
	bool IsBacklightOn() { return lastTimeDisplayOn != 0; }

	time_t DisplayTimeout;

protected:
	time_t lastTimeDisplayOn;

	int backlightPin;
	int lcdHandle;
};

