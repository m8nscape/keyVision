#pragma once
#include <iostream>
#include <numeric>
#include <list>
#include <ctime>
#include <vector>
#include <bitset>
#include <Windows.h>

std::list<time_t> eventTimes;
std::list<time_t> eventTimes10;
std::list<int> eventKeys;
std::list<int> eventKeys10;
std::bitset<128> keys{};

int keyDetect(const std::vector<int>& scanCodes)
{
	time_t time = clock();
	while (!eventTimes10.empty() && eventTimes10.front() <= time - 100)
	{
		eventTimes10.pop_front();
		eventKeys10.pop_front();
	}
	while (!eventTimes.empty() && eventTimes.front() <= time - 1000)
	{
		eventTimes.pop_front();
		eventKeys.pop_front();
	}

	size_t k = 0;
	for (size_t i = 0; i < scanCodes.size(); ++i)
	{
		int vkey = scanCodes[i];
		if ((GetAsyncKeyState(vkey) & 0x8000) != 0)
		{
			if (!keys[i])
			{
				++k;
				keys[i] = true;
			}
		}
		else
		{
			if (keys[i])
			{
				keys[i] = false;
			}
		}
	}

	eventTimes.push_back(time);
	eventKeys.push_back(k);
	eventTimes10.push_back(time);
	eventKeys10.push_back(k);
	return k;
}

unsigned getKPS()
{
	return std::accumulate(eventKeys.begin(), eventKeys.end(), 0u);
}

unsigned getKP01S()
{
	return std::accumulate(eventKeys10.begin(), eventKeys10.end(), 0u);
}