#include "DomoticzDataParser.h"
#include <stdio.h>
#include <unistd.h>

CDomoticzDataParser::CDomoticzDataParser()
{
	Temperature = 0;
	CPULoad = 0;
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
		fscanf(fp, "CPUTemp:%lf\nCPULoad:%lf", &Temperature, &CPULoad);

		fclose(fp);
	}

}
