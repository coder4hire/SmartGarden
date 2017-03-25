#include "LcdDisplay.h"
#include "wiringPi.h"
#include "stdio.h"
#include <unistd.h>
#include "DomoticzInterface.h"
#include "main.h"
//unsigned int sleep(unsigned int seconds);

void ConfigurePins();

int main()
{
	CDomoticzInterface iDM("192.168.2.50",8080);

	wiringPiSetup();
	CLcdDisplay lcd(4,20,CLcdDisplay::BITS_4, 25, 24, 23, 7, 21, 22, 0, 0, 0, 0);
	ConfigurePins();

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
		usleep(200000);

		char c;
		printf("Cmd:");
		scanf("%c", &c);
		if (c == 'a')
		{
			printf("On command is sent\n");
			if (!iDM.SendSwitchCommand(9, true))
			{
				printf("Error sending command.\n");
			}
		}
	}

	return 0;
}

void ConfigurePins()
{
	pinMode(2, OUTPUT);
}
