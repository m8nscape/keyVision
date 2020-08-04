#pragma once
#include <array>
#include <vector>
#include "num.h"
#include <SFML/Graphics.hpp>
#pragma warning (disable: 4244)

enum blendType
{
	No,
	Alpha,
	Add
};

enum eventType
{
	Static,
	keyPress,
	keyRelease,
	KPSHigh,
	KPSLow,
	eventTypeCount
};

enum numberType
{
	nbr_KPS,
	nbr_keyStrokesInCurrentFrame,
};

enum barType
{
	bar_KPS,
	bar_KP01S,
};

enum spLevel
{
	PAR_Number = 1000,
	PAR_Bar,
};

class Sprite : public sf::Sprite
{
public:
	Sprite() : sf::Sprite() {}
	Sprite(const sf::Texture& texture) : sf::Sprite(texture) {}
public:
	virtual void update() {}
};

class Number : public Sprite
{
protected:
	int numType;
	void setRotation(float);
	void rotate(float);
protected:
	int _value = 0;
	bool leftAlign = false;
	unsigned div_x = 1, div_y = 1;
	mutable std::vector<std::array<Sprite, 10>> _numSprites;
public:
	Number() = delete;
	Number(int type, size_t digit) : Sprite(), numType(type) { _numSprites.resize(digit); }
	Number(int type, size_t digit,
		const sf::Texture &texture, unsigned div_x = 1, unsigned div_y = 1)
		: numType(type), Sprite(texture), div_x(div_x), div_y(div_y)
	{
		_numSprites.resize(digit);
		setTextureRect();
	}
public:
	virtual void update() override
	{
		int dx = getScale().x * getTextureRect().width;
		//int dy = getScale().y * getTextureRect().height;
		int x = getPosition().x;
		int y = getPosition().y;
		x += (_numSprites.size() - 1) * dx;
		dx = -dx;

		for (size_t i = 0; i < _numSprites.size(); ++i)
		{
			for (auto& spr : _numSprites[i])
			{
				// x' = x + wi cosa
				// y' = y - wi sina
				spr.setPosition(x + i * dx, y);
				spr.setScale(getScale());
				spr.setColor(getColor());
			}
		}
	}

public:
	void setAlign(bool left) { leftAlign = left; }
	void setTextureRect(sf::IntRect rect = sf::IntRect(0, 0, 0, 0))
	{
		auto size = getTexture()->getSize();
		int x, y;
		x = y = 0;
		if (rect != sf::IntRect(0, 0, 0, 0))
		{
			x = rect.left;
			y = rect.top;
			size = sf::Vector2u(rect.width, rect.height);
		}
		unsigned w = size.x / div_x;
		unsigned h = size.y / div_y;
		for (size_t i = 0; i < _numSprites.size(); ++i)
		{
			for (size_t v = 0; v < 10; ++v)
			{
				_numSprites[i][v].setTexture(*getTexture());
				_numSprites[i][v].setTextureRect(sf::IntRect(x + v % div_x * w, y + v / div_x * h, w, h));
			}
		}
		sf::Sprite::setTextureRect(sf::IntRect(x, y, w, h));
	}
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override
	{
		int tmp = num().get(KPS);
		for (size_t i = 0; i < _numSprites.size(); ++i)
		{
			unsigned d = tmp % 10;
			tmp /= 10;
			// if (tmp == 0) ...
			target.draw(_numSprites[i][d], states);
		}
	}

};

class Bar : public Sprite
{
protected:
	int barType;
protected:
	int _value = 0;
	int dir = 5;
	int min = 0, max = 0;
public:
	Bar() = delete;
	Bar(int type, int dir, int min, int max,
		const sf::Texture& texture)
		: barType(type), Sprite(texture), dir(dir), min(min), max(max)
	{
	}
public:
	virtual void update() override
	{
		float val = 0.0;
		if (min != max)
		{
			int n = min;
			switch (barType)
			{
			case bar_KPS: n = num().get(KPS); break;
			case bar_KP01S: n = num().get(KP01S); break;
			default: break;
			}
			if (n < min) n = min;
			val = float((n - min) / (max - min));
		}
		switch (dir)
		{
		case 2:
			setScale({ 1.0, val });
			break;
		case 4:
			setScale({ -val, 1.0 });
			break;
		case 6:
			setScale({ val, 1.0 });
			break;
		case 8:
			setScale({ 1.0, -val });
			break;

		default: break;
		}
	}
};
