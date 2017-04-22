#pragma once
class CDomoticzDataParser
{
public:
	CDomoticzDataParser();
	~CDomoticzDataParser();

	void LoadData();

	int Temperature;
	int CPULoad;
};

