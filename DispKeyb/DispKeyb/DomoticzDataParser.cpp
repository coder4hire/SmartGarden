#include "DomoticzDataParser.h"
#include <stdio.h>
#include <unistd.h>

CDomoticzDataParser CDomoticzDataParser::Inst;

CDomoticzDataParser::CDomoticzDataParser()
{
	Temperature = 0;
	CPULoad = 0;
	RoomTemp = 0;
	OutsideTemp = 0;
	
	oldTime = 0;
	updateInterval = 5; // seconds
}


CDomoticzDataParser::~CDomoticzDataParser()
{
}

void CDomoticzDataParser::LoadData()
{
	double temp = 0, load = 0;
	int i = 0;
	int reTries = 5;
	FILE* fp = NULL;

	for (i = 0; !(fp = fopen("/tmp/dm_values", "r")) && i<reTries; i++)
	{
		usleep(500000);
	}
	if (fp)
	{
		fscanf(fp, "CPUTemp:%lf\nCPULoad:%lf\nRoomTemp:%lf\nOutsideTemp:%lf", &Temperature, &CPULoad,&RoomTemp,&OutsideTemp);
		fclose(fp);
		time(&oldTime);
	}

}

void CDomoticzDataParser::RefreshDataIfNeeded()
{
	time_t now;
	time(&now);
	if (now - oldTime > updateInterval)
	{
		oldTime = now;
		LoadData();
	}
}
