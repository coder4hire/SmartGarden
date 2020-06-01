#include "DomoticzDataParser.h"
#include <stdio.h>
#include <unistd.h>
#include <future>
#include <string.h>

CDomoticzDataParser CDomoticzDataParser::Inst;

CDomoticzDataParser::CDomoticzDataParser()
{
	memset(&sensors, 0, sizeof(sensors));
	
	StartUpdatingThread();
}


CDomoticzDataParser::~CDomoticzDataParser()
{
	StopUpdatingThread();
}

SensorsData CDomoticzDataParser::GetSensorsData()
{
	std::lock_guard<std::mutex> lock(mtx);
	return sensors;
}

void CDomoticzDataParser::StartUpdatingThread()
{
	updateInterval = 5; // seconds
	readingThread = std::thread([this] {this->LoadData(); });
}

void CDomoticzDataParser::StopUpdatingThread()
{
	updateInterval = 0;
	if (readingThread.joinable())
	{
		readingThread.join();
	}
}

void CDomoticzDataParser::LoadData()
{
	while (updateInterval != 0)
	{
		double temp = 0, load = 0;
		int i = 0;
		int reTries = 5;
		FILE* fp = NULL;

		for (i = 0; !(fp = fopen("/tmp/dm_values", "r")) && i < reTries && updateInterval != 0; i++)
		{
			usleep(500000);
		}
		if (fp)
		{
			std::lock_guard<std::mutex> lock(mtx);
			fscanf(fp, "CPUTemp:%lf\nCPULoad:%lf\nRoomTemp:%lf\nOutsideTemp:%lf",
				&sensors.Temperature, &sensors.CPULoad, &sensors.RoomTemp, &sensors.OutsideTemp);
			fclose(fp);
			time(&sensors.lastTimeUpdated);
		}

		sleep(updateInterval);
	}
}