#include <list>
#include <chrono>
#include <thread>
#include <mutex>

#define byte win_byte_override
#include <SFML/Window.hpp>

#include "config.h"
#include "key.h"
#include "skin.h"
#include "num.h"

std::mutex eventMutex;

void subthread(const sf::RenderWindow& window)
{
	std::vector<int> scanCodes;
	for (int c = 'A'; c <= 'Z'; ++c) scanCodes.push_back(c);
	for (int c = '0'; c <= '9'; ++c) scanCodes.push_back(c);
	for (int c = 0x1B; c <= 0x2F; ++c) scanCodes.push_back(c);	// special keys (esc, space, etc)
	for (int c = 0x60; c <= 0x7B; ++c) scanCodes.push_back(c);	// numpad, F1-F12
	for (int c = 0xA0; c <= 0xA5; ++c) scanCodes.push_back(c);	// shift, alt, ctrl
	for (int c = 0xBA; c <= 0xC0; ++c) scanCodes.push_back(c);	// symbol
	for (int c = 0xDB; c <= 0xDF; ++c) scanCodes.push_back(c);	// symbol

	int pk = 0, pkps = 0;
	while (window.isOpen())
	{
		auto time1 = std::chrono::system_clock::now();
		int k = keyDetect(scanCodes);
		int kps = getKPS();
		num().set(CurrentFrameKeys, k);
		num().set(KPS, getKPS());
		num().set(KP01S, getKP01S());
		{
			std::lock_guard<std::mutex> lock(eventMutex);
			if (k > 0)
				skinInst().registerEvent(keyPress, k);
			if (kps > pkps)
				skinInst().registerEvent(KPSHigh, kps);
		}
		pk = k;
		pkps = kps;
		this_thread::sleep_until(time1 + std::chrono::milliseconds(updateInterval));
	}
}

int main()
{
	loadConfig();
	if (skinPath == "")
		return 1;
	else
		skinInst().readFile("Skin/" + skinPath + "/skin.csv");


	sf::RenderWindow window(
		sf::VideoMode(skinInst().winw, skinInst().winh),
		"KeyVision",
		sf::Style::Titlebar);
	//sf::RenderWindow window(sf::VideoMode(1600, 900), "test");

	window.setFramerateLimit(maxfps);
	window.setVerticalSyncEnabled(true);

	auto t = std::thread(subthread, std::ref(window));

	while (window.isOpen())
	 {
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
;
		window.clear(sf::Color::Black);
		{
			std::lock_guard<std::mutex> lock(eventMutex);
			window.draw(skinInst());
		}
		window.display();
	}

	t.join();

	return 0;
}