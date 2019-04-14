#pragma once
#include <time.h>

class CDomoticzDataParser
{
public:
	static CDomoticzDataParser Inst;
	~CDomoticzDataParser();

	void LoadData();
	void RefreshDataIfNeeded();

	double Temperature;
	double CPULoad;
	double RoomTemp;
	double OutsideTemp;

protected:
	CDomoticzDataParser();
	int updateInterval;
	time_t oldTime;
};

