#include "wiringPi.h"
#include "stdio.h"
#include <unistd.h>
#include <errno.h>
#include "DomoticzInterface.h"
#include "main.h"
#include "MainScreen.h"
#include "PinsDefinitions.h"
#include "HWInterface.h"

//unsigned int sleep(unsigned int seconds);

void ConfigurePins();
bool changeOwner(char *file);
bool exportPin(int pin, int mode);
bool setEdgeMode(int pin, int mode);

int main()
{
	CDomoticzInterface iDM("192.168.2.50",8080);

	wiringPiSetup();
	printf("Wiring initialized\n");
	CLcdDisplay::Inst.InitDefault();
	ConfigurePins();
	CHWInterface::Inst.RestoreVolume();

	CMainScreen mainScreen;
	mainScreen.Run();
	return 0;

	//FILE* fp = NULL;
	//while (true)
	//{
	//	double temp=0, load=0;
	//	int i = 0;
	//	int reTries = 5;
	//	for(i=0; !(fp = fopen("/tmp/dm_values", "r")) && i<reTries; i++)
	//	{
	//		usleep(500000);
	//	}
	//	if (fp)
	//	{
	//		fscanf(fp, "CPUTemp:%lf\nCPULoad:%lf", &temp, &load);

	//		lcd.GotoXY(0, 0);
	//		lcd.Printf("Temp: %.2lf    ", temp);
	//		lcd.GotoXY(0, 1);
	//		lcd.Printf("CPU:  %.2lf    ", load);

	//		fclose(fp);
	//	}
	//	//lcd.CursorBlink(true);
	//	usleep(200000);

	//	//char c;
	//	//printf("Cmd:");
	//	//scanf("%c", &c);
	//	//if (c == 'a')
	//	//{
	//	//	printf("On command is sent\n");
	//	//	if (!iDM.SendSwitchCommand(9, true))
	//	//	{
	//	//		printf("Error sending command.\n");
	//	//	}
	//	//}
	//}

	//return 0;
}

void ConfigurePins()
{
	pinMode(2, OUTPUT);
	pinMode(PIN_BACKLIGHT, OUTPUT);

	// Keyboard
	pinMode(PINKEY_UP, INPUT);
	pinMode(PINKEY_DOWN, INPUT);
	pinMode(PINKEY_OK, INPUT);
	pinMode(PINKEY_CANCEL, INPUT);
	pinMode(PINKEY_ABORT, INPUT);
	pullUpDnControl(PINKEY_UP, PUD_UP);
	pullUpDnControl(PINKEY_DOWN, PUD_UP);
	pullUpDnControl(PINKEY_OK, PUD_UP);
	pullUpDnControl(PINKEY_CANCEL, PUD_UP);
	pullUpDnControl(PINKEY_ABORT, PUD_UP);
	exportPin(11, INPUT);	// Abort key
	setEdgeMode(11, INT_EDGE_BOTH);

	//// GPIO Inputs
	pinMode(PIN_IN1, INPUT);
	pinMode(PIN_IN3, INPUT);
	pullUpDnControl(PIN_IN1, PUD_DOWN);
	pullUpDnControl(PIN_IN3, PUD_DOWN);

	//// GPIO Outputs
	pinMode(PIN_OUT1, OUTPUT);
	pinMode(PIN_OUT2, OUTPUT);
	pinMode(PIN_OUT3, OUTPUT);
	pinMode(PIN_OUT4, OUTPUT);

	// Exporting Outputs. BCM pins numbering is used here (not wiringPI as above)
	exportPin(21, OUTPUT);
	exportPin(10, OUTPUT);
	exportPin(9, OUTPUT);
    exportPin(8, OUTPUT);
	exportPin(14, INPUT);
	exportPin(67, INPUT);
	setEdgeMode(14, INT_EDGE_BOTH);
	setEdgeMode(67, INT_EDGE_BOTH);
}

bool changeOwner(char *file)
{
	uid_t uid = getuid();
	uid_t gid = getgid();

	return chown(file, uid, gid) != 0;
}

bool exportPin(int pin, int mode)
{
	FILE *fd;
	char fName[128];

	if ((fd = fopen("/sys/class/gpio/export", "w")) == NULL)
	{
		return false;
	}

	fprintf(fd, "%d\n", pin);
	fclose(fd);

	sprintf(fName, "/sys/class/gpio/gpio%d/direction", pin);
	if ((fd = fopen(fName, "w")) == NULL)
	{
		return false;
	}

	if (mode == INPUT)
	{
		fprintf(fd, "in\n");
	}
	else if (mode == OUTPUT)
	{
		fprintf(fd, "out\n");
	}
	else
	{
		fclose(fd);
		return false;
	}

	fclose(fd);

	// Change ownership so the current user can actually use it!

	bool retVal = true;
	sprintf(fName, "/sys/class/gpio/gpio%d/value", pin);
	retVal &= changeOwner(fName);

	sprintf(fName, "/sys/class/gpio/gpio%d/edge", pin);
	retVal &= changeOwner(fName);

	sprintf(fName, "/sys/class/gpio/gpio%d/active_low", pin);
	retVal &= changeOwner(fName);

	return retVal;
}

const char* strEdgeModes[] = { "none", "rising","falling","both" };

bool setEdgeMode(int pin, int edgeMode)
{
	FILE *fd;
	char fName[128];

	if (edgeMode < 0 || edgeMode>3)
	{
		return false;
	}

	if (pin == 0)
	{
		printf("%d is invalid pin,please check it over.\n", pin);
		return false;
	}

	// Export the pin and set direction to input
	if ((fd = fopen("/sys/class/gpio/export", "w")) == NULL)
	{
		fprintf(stderr, "Unable to open GPIO export interface\n");
		return false;
	}

	fprintf(fd, "%d\n", pin);
	fclose(fd);

	sprintf(fName, "/sys/class/gpio/gpio%d/direction", pin);
	if ((fd = fopen(fName, "w")) == NULL)
	{
		fprintf(stderr, "Unable to open GPIO direction interface for pin %d\n", pin);
		return false;
	}

	fprintf(fd, "none\n");
	fclose(fd);

	fd = fopen(fName, "w");
	fprintf(fd, "in\n");
	fclose(fd);

	sprintf(fName, "/sys/class/gpio/gpio%d/edge", pin);
	if ((fd = fopen(fName, "w")) == NULL)
	{
		fprintf(stderr, "Unable to open GPIO edge interface for pin %d\n", pin);
		return false;
	}

	// Always reset mode to "none" before setting a new mode
	fprintf(fd, "none\n");
	fclose(fd);
	
	fd = fopen(fName, "w");
	fprintf(fd, "%s\n",strEdgeModes[edgeMode]);

	// Change ownership of the value and edge files, so the current user can actually use it!
	sprintf(fName, "/sys/class/gpio/gpio%d/value", pin);
	changeOwner(fName);

	sprintf(fName, "/sys/class/gpio/gpio%d/edge", pin);
	changeOwner(fName);

	fclose(fd);
	return true;
}
