#pragma once

#include "etc.h"
#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <list>
using namespace std;
#include <SFML/Graphics.hpp>

enum pattern
{
	RESOLUTION,

	COMMENT,
	EMPTYLINE,
	IMAGE_FILE,
	TEXTURE,
	NUM_TEXTURE,
	END_OF_HEADER,

	IMAGE,
	NUM_IMAGE,

	EVENT,
	EVENT_ANIM_KEY_FRAME,
	STATIC_EVENT,
	STATIC_EVENT_ANIM_KEY_FRAME,

};

struct keyFrame
{
	int time;
	int x, y, w, h;
	int r, g, b, a;
};
struct eventTy
{
	int level;
	unsigned layer;
	size_t drawable;
	vector<keyFrame> keyFrames;
	int blend;
	int loopTime;
};
struct eventStat
{
	int eventType;
	size_t eventIdx;
	long long startTime;
	unsigned layer;
	std::shared_ptr<sf::Sprite> spr;
};

class skin : public sf::Drawable
{
private:
	skin();
	~skin() = default;

private:
	// clear after loading
	// should not be used 
	vector<sf::Image> vecImage;
	unordered_map<string, size_t> imageTokenIdx;

protected:
	unordered_map<string, size_t> textureTokenIdx;
	unordered_map<string, size_t> drawableTokenIdx;
	vector<sf::Texture> vecTexture;
	vector<sf::Sprite*> vecSprite;

	vector<eventTy> events[eventTypeCount];
	mutable vector<bool> eventRunning[eventTypeCount]; // for kps; vector<bool> is ok

	mutable vector<eventStat> runningEvents;


public:
	int winw = 128, winh = 128;

	void readFile(string path);
	pattern parse(string line, int& prevevent, pattern prev = EMPTYLINE);

	void registerEvent(eventType type, int level = 0) const;
	void update(eventStat&) const;
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

public:
	friend skin& skinInst();
};
