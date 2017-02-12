#include "LcdDisplay.h"
#include "wiringPi.h"
#include "stdio.h"
#include <unistd.h>
//unsigned int sleep(unsigned int seconds);

int main()
{
	wiringPiSetup();
	CLcdDisplay lcd(4,20,CLcdDisplay::BITS_4, 25, 24, 23, 7, 21, 22, 0, 0, 0, 0);;

	FILE* fp = NULL;
	while (true)
	{
		double temp=0, load=0;
		int i = 0;
		int reTries = 5;
		for(i=0; !(fp = fopen("/tmp/dm_values", "r")) && i<reTries; i++)
		{
			usleep(500000);
		}
		if (fp)
		{
			fscanf(fp, "CPUTemp:%lf\nCPULoad:%lf", &temp, &load);

			lcd.GotoXY(0, 0);
			lcd.Printf("Temp: %.2lf    ", temp);
			lcd.GotoXY(0, 1);
			lcd.Printf("CPU:  %.2lf    ", load);

			fclose(fp);
		}
		//lcd.CursorBlink(true);
		usleep(2000000);
	}

	return 0;
}