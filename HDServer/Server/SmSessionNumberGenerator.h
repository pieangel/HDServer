#pragma once
class SmSessionNumberGenerator
{
private:
	static int _ID;
public:
	static int GetID() {
		return ++_ID;
	}
};

