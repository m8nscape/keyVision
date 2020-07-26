#include "skin.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
//#include <boost/tokenizer.hpp>
using namespace std;

auto staticStartTime = std::chrono::system_clock::now();
//typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;

skin& skinInst()
{
	static skin _inst;
	return _inst;
}
skin::skin()
{
	vecImage.emplace_back();
	vecTexture.emplace_back();
}
void skin::readFile(string path)
{
	ifstream file(path, ios::binary);
	string lineRaw;

	pattern prev = EMPTYLINE;
	int evt = 0;
	while (getline(file, lineRaw))
	{
		prev = parse(lineRaw, evt, prev);
	}

	for (auto& tex : vecTexture)
	{
		tex.setSmooth(true);
	}

	vecImage.clear();
	imageTokenIdx.clear();
}

static unsigned lineN = 0;
#define THROW_PARAM_NOT_MATCH \
	throw(out_of_range(string("Config file leveleters not match, Line " + lineN)))

pattern skin::parse(string lineRaw, int& prevevent, pattern prev)
{
	++lineN;
	if (lineRaw.substr(0, 2) == "//")
		return COMMENT;

	while (lineRaw.length() > 0 && lineRaw[lineRaw.length() - 1] == '\r')
		lineRaw.pop_back();

	if (lineRaw.empty())
		return EMPTYLINE;

	// replace "\" with "\\"
	std::istringstream iss(lineRaw);
	std::string buf, line;
	while (std::getline(iss, buf, '\\'))
		line += buf + R"(\\)";
	line.erase(line.length() - 2);

	std::vector<std::string> tokens;
	//auto tokenBase = tokenizer(line, boost::escaped_list_separator<char>());
	//for (auto& t : tokenBase) tokens.push_back(t);
	{
		size_t lineDelimiterOffset = 0;
		size_t prevOffset = 0;
		while (true)
		{
			lineDelimiterOffset = line.find(',', prevOffset);
			if (lineDelimiterOffset == line.npos)
			{
				tokens.push_back(line.substr(prevOffset));
				break;
			}
			else
			{
				tokens.push_back(line.substr(prevOffset, lineDelimiterOffset - prevOffset));
				prevOffset = lineDelimiterOffset + 1;
			}
		}
	}
	while (!tokens.empty() && tokens.back().empty()) tokens.pop_back();
	if (tokens.empty()) return EMPTYLINE;

	else
	{
		if (tokens[0] == "Resolution")
		{
			if (tokens.size() != 3) THROW_PARAM_NOT_MATCH;
			int w = stoi(tokens[1]);
			int h = stoi(tokens[2]);
			winw = w;
			winh = h;
			return RESOLUTION;
		}
		else if (tokens[0] == "ImageFile")
		{
			if (tokens.size() != 3) THROW_PARAM_NOT_MATCH;
			string token = tokens[1];
			string path = tokens[2];
			vecImage.emplace_back();
			vecImage.back().loadFromFile(path);
			imageTokenIdx.emplace(token, vecImage.size() - 1);
			return IMAGE_FILE;
		}
		else if (tokens[0] == "Texture")
		{
			if (tokens.size() != 7) THROW_PARAM_NOT_MATCH;
			string token = tokens[1];
			unsigned imageIdx = imageTokenIdx[tokens[2]];
			int x = stoi(tokens[3]);
			int y = stoi(tokens[4]);
			int w = stoi(tokens[5]);
			int h = stoi(tokens[6]);
			vecTexture.emplace_back();
			vecTexture.back().loadFromImage(vecImage[imageIdx], sf::IntRect(x, y, w, h));
			textureTokenIdx.emplace(token, vecTexture.size() - 1);
			return TEXTURE;
		}
		//else if (tokens[0] == "EndOfHeader")
		//{
		//	if (tokens.size() != 1) PARAM_NOT_MATCH
		//	return END_OF_HEADER;
		//}
		else if (tokens[0] == "Image")
		{
			if (tokens.size() != 7) THROW_PARAM_NOT_MATCH;
			string token = tokens[1];
			string texture = tokens[2];
			int x = stoi(tokens[3]);
			int y = stoi(tokens[4]);
			int w = stoi(tokens[5]);
			int h = stoi(tokens[6]);
			auto spr = std::make_shared<Sprite>(vecTexture[textureTokenIdx[texture]]);
			vecSprite.push_back(spr);
			spr->setTextureRect(sf::IntRect(x, y, w, h));
			drawableTokenIdx.emplace(token, vecSprite.size() - 1);
			return IMAGE;
		}
		else if (tokens[0] == "NumImage")
		{
			if (tokens.size() != 11) THROW_PARAM_NOT_MATCH;
			string token = tokens[1];
			string texture = tokens[2];
			int x = stoi(tokens[3]);
			int y = stoi(tokens[4]);
			int w = stoi(tokens[5]);
			int h = stoi(tokens[6]);
			int divx = stoi(tokens[7]);
			int divy = stoi(tokens[8]);
			int digit = stoi(tokens[9]);
			int numType = stoi(tokens[10]);
			auto spr = std::make_shared<Number>(numType, digit, vecTexture[textureTokenIdx[texture]], divx, divy);
			vecSprite.push_back(spr);
			spr->setTextureRect(sf::IntRect(x, y, w, h));
			drawableTokenIdx.emplace(token, vecSprite.size() - 1);
			return NUM_IMAGE;
		}
		else if (tokens[0] == "BarImage")
		{
			if (tokens.size() != 11) THROW_PARAM_NOT_MATCH;
				string token = tokens[1];
			string texture = tokens[2];
			int x = stoi(tokens[3]);
			int y = stoi(tokens[4]);
			int w = stoi(tokens[5]);
			int h = stoi(tokens[6]);
			int dir = stoi(tokens[7]);
			int min = stoi(tokens[8]);
			int max = stoi(tokens[9]);
			int barType = stoi(tokens[10]);
			auto spr = std::make_shared<Bar>(barType, dir, min, max, vecTexture[textureTokenIdx[texture]]);
			vecSprite.push_back(spr);
			spr->setTextureRect(sf::IntRect(x, y, w, h));
			drawableTokenIdx.emplace(token, vecSprite.size() - 1);
			return IMAGE;
		}
		else if (tokens[0] == "Event")
		{
			if (tokens.size() != 7) THROW_PARAM_NOT_MATCH;
			int type = prevevent = stoi(tokens[1]);
			int eventLevel = stoi(tokens[2]);
			int layer = stoi(tokens[3]);
			string drawable = tokens[4];
			int blend = stoi(tokens[5]);
			int loop = stoi(tokens[6]);
			events[type].emplace_back();
			eventRunning[type].push_back(false);
			auto &rf = events[type].back();
			rf.level = eventLevel;
			rf.layer = layer;
			rf.drawable = drawableTokenIdx[drawable];
			rf.blend = blend;
			rf.loopTime = loop;
			if (type == Static)
				registerEvent(Static, 0);
			return EVENT;
		}
		else if (tokens[0] == "KeyFrame")
		{
			if (prev == EVENT || prev == EVENT_ANIM_KEY_FRAME)
			{
				if (tokens.size() != 10) THROW_PARAM_NOT_MATCH;
				auto& kFrames = events[prevevent].back().keyFrames;
				int time = stoi(tokens[1]);
				int x = stoi(tokens[2]);
				int y = stoi(tokens[3]);
				int w = stoi(tokens[4]);
				int h = stoi(tokens[5]);
				int r = stoi(tokens[6]);
				int g = stoi(tokens[7]);
				int b = stoi(tokens[8]);
				int a = stoi(tokens[9]);
				kFrames.push_back({ time, x, y, w, h, r, g, b, a });
				return EVENT_ANIM_KEY_FRAME;
			}
			return EMPTYLINE;
		}
	}
	return EMPTYLINE;
}

void skin::update(eventStat& evt) const
{
	auto &e = events[evt.eventType][evt.eventIdx];
	//auto &sprite = vecSprite[evt.evt->drawable];
	auto &sprite = *evt.spr;

	long long rTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now() - staticStartTime).count() - evt.startTime;

	if (evt.eventType == KPSHigh)
	{
		if (e.level > num().get(KPS))
		{
			evt.layer = INT_MAX;
			eventRunning[evt.eventType][evt.eventIdx] = false;
			registerEvent(KPSLow, e.level);
			return;
		}
	}

	if (rTime >= e.keyFrames.back().time)
	{
		if (e.loopTime >= 0)
		{
			if (e.keyFrames.back().time >= e.loopTime)
			{
				auto &f = e.keyFrames.back();

				auto scl = sprite.getTextureRect();
				sprite.setScale((float)f.w / scl.width, (float)f.h / scl.height);

				sprite.setPosition(f.x, f.y);
				sprite.setColor(sf::Color(f.r, f.g, f.b, f.a));
				return;
			}
			else
			{
				rTime %= e.keyFrames.back().time - e.loopTime;
			}
		}
		else
		{
			eventRunning[evt.eventType][evt.eventIdx] = false;
			evt.layer = INT_MAX;
			return;
		}
	}

	// get keyFrame section
	unsigned keyFrameIdx = -1, keyFrameNext = -1;
	for (size_t f = 0; f < e.keyFrames.size(); ++f)
		if (e.keyFrames[f].time <= rTime)
			keyFrameIdx = f;

	if (keyFrameIdx == -1)
		return;
	else if (keyFrameIdx == e.keyFrames.size() - 1)
		return;
	else
		keyFrameNext = keyFrameIdx + 1;

	// normalize time
	auto& curr = e.keyFrames[keyFrameIdx];
	auto& next = e.keyFrames[keyFrameNext];
	float timeFactor;
	unsigned keyFrameLength = next.time - curr.time;
	if (keyFrameLength > 0)
		timeFactor = 1.0f * rTime / keyFrameLength;
	else
		timeFactor = 1.0f;

	// calculate leveleters
	auto textureRect = sprite.getTextureRect();
	float x = curr.x + (next.x - curr.x) * timeFactor;
	float y = curr.y + (next.y - curr.y) * timeFactor;
	float w = (curr.w + (next.w - curr.w) * timeFactor) / textureRect.width;
	float h = (curr.h + (next.h - curr.h) * timeFactor) / textureRect.height;
	unsigned r = static_cast<unsigned>(curr.r + (next.r - curr.r) * timeFactor);
	unsigned g = static_cast<unsigned>(curr.g + (next.g - curr.g) * timeFactor);
	unsigned b = static_cast<unsigned>(curr.b + (next.b - curr.b) * timeFactor);
	unsigned a = static_cast<unsigned>(curr.a + (next.a - curr.a) * timeFactor);
	//float angle = curr.rotateAngle + (next.rotateAngle - curr.rotateAngle) * timeFactor;

	sprite.setPosition(x, y);
	sprite.setScale(w, h);
	sprite.setColor(sf::Color(r, g, b, a));
	//sprite.setRotation(angle);
}

void skin::registerEvent(eventType type, int level) const
{
	long long rTime = std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now() - staticStartTime).count();
	for (size_t i = 0; i < events[type].size(); ++i)
	{
		auto &evt = events[type][i];
		if (type == Static || 
			type == KPSHigh && evt.level <= level && !eventRunning[type][i] || 
			type != KPSLow && type != KPSHigh && evt.level <= level || 
			type == KPSLow && evt.level >= level && !eventRunning[type][i])
		{
			if (type == KPSHigh || type == KPSLow)
				eventRunning[type][i] = true;
			runningEvents.emplace_back();
			auto &ptr = runningEvents.back();
			ptr.eventType = type;
			ptr.eventIdx = i;
			ptr.startTime = rTime;
			ptr.layer = evt.layer;
			ptr.spr = vecSprite[evt.drawable];
		}
	}
}

void skin::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (size_t i = 0; i < runningEvents.size(); ++i)
	{
		update(runningEvents[i]);
	}

	std::sort(runningEvents.begin(), runningEvents.end(), 
		[&](const eventStat &lhs, const eventStat &rhs)
		{
			return lhs.layer < rhs.layer;
		}
	);

	if (!runningEvents.empty())
	{
		size_t tail = runningEvents.size() - 1;
		while (tail >= 0 && runningEvents[tail].layer == INT_MAX) --tail;
		runningEvents.resize(tail + 1);
	}

	for (auto& obj : runningEvents)
	{
		switch (events[obj.eventType][obj.eventIdx].blend)
		{
		case No: states.blendMode = sf::BlendNone; break;
		case Add: states.blendMode = sf::BlendAdd; break;
		case Alpha:
		default:
			states.blendMode = sf::BlendAlpha; break;
		}
		obj.spr->update();
		target.draw(*obj.spr, states);
	}
}