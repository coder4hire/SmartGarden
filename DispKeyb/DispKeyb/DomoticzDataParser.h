#pragma once
#include <time.h>
#include <thread>
#include <mutex>
#include <atomic>

struct SensorsData
{
	double Temperature;
	double CPULoad;
	double RoomTemp;
	double OutsideTemp;
	time_t lastTimeUpdated;
};

class CDomoticzDataParser
{
public:
	static CDomoticzDataParser Inst;
	~CDomoticzDataParser();

	SensorsData GetSensorsData();
	void StartUpdatingThread();
	void StopUpdatingThread();

protected:
	std::thread readingThread;
	SensorsData sensors;
	CDomoticzDataParser();
	void LoadData();
	std::atomic<int> updateInterval;
	std::mutex mtx;
};

