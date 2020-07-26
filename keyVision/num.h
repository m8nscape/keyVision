#pragma once

enum numbers
{
	KPS,
	CurrentFrameKeys,
	KP01S,

	NumCount
};


class _num
{
private:
	_num() = default;
	~_num() = default;
	int data[NumCount]{};
public:
	int get(numbers n) const { return data[n]; }
	void set(numbers n, int val) { data[n] = val; }
	friend _num& num();
};

_num& num();
