#include "LcdDisplay.h"
#include "wiringPi.h"

int main()
{
	wiringPiSetup();
	CLcdDisplay lcd(4,20,CLcdDisplay::BITS_4, 25, 24, 23, 7, 21, 22, 0, 0, 0, 0);;

	lcd.GotoXY(4, 1);
	lcd.Printf("Test: %d", 1);
	lcd.CursorBlink(true);

	return 0;
}