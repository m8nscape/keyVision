#pragma once
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

int updateInterval = 8;		// ms
int maxfps = 240;
string skinPath = "";

int loadConfig()
{
	ifstream globalConfig("config.cfg");
	string line;
	while (getline(globalConfig, line))
	{
		size_t sep = line.find('=');
		string key = line.substr(0, sep);
		key.shrink_to_fit();
		string val = line.substr(sep + 1);
		val.shrink_to_fit();

		if (key == "Skin")
		{
			skinPath = val;
		}
		else if (key == "Interval")
		{
			updateInterval = stoi(val);
			if (updateInterval <= 0)
				updateInterval = 1;
		}
		else if (key == "FPSLimit")
		{
			maxfps = stoi(val);
			if (maxfps < 0)
				maxfps = 240;
		}
	}

	return 0;
}