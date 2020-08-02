#pragma oncelass SmOrderNumberGenerator
{
private:
	static int _ID;
public:
	static int GetID() {
		return ++_ID;
	}
};

