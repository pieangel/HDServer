#pragma once

class SmServieReqNumGenerator
{
private:
	static int _ID;
public:
	static int GetID() {
		return ++_ID;
	}
};

